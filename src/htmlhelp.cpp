/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 * The original version of this file is largely based on a contribution from
 * Harm van der Heijden.
 */

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

#include "htmlhelp.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"
#include "portable.h"
#include "groupdef.h"
#include "memberdef.h"
#include "filedef.h"
#include "util.h"
#include "linkedmap.h"
#include "regex.h"
#include "fileinfo.h"

//----------------------------------------------------------------------------

/** Helper class to deal with recoding the UTF8 encoded text back to the native encoding
 *  specified by CHM_INDEX_ENCODING.
 */
class HtmlHelpRecoder
{
  public:
    HtmlHelpRecoder() {}
   ~HtmlHelpRecoder() { finalize(); }
    NON_COPYABLE(HtmlHelpRecoder)

    void initialize()
    {
      QCString str = Config_getString(CHM_INDEX_ENCODING);
      if (str.isEmpty()) str = "CP1250"; // use safe and likely default
      m_fromUtf8 = portable_iconv_open(str.data(),"UTF-8");
      if (m_fromUtf8==m_iconv_null)
      {
        term("unsupported character conversion for CHM_INDEX_ENCODING: '%s'->'UTF-8'\n", qPrint(str));
      }
    }
    void finalize()
    {
      if (m_fromUtf8!=m_iconv_null)
      {
        portable_iconv_close(m_fromUtf8);
        m_fromUtf8 = m_iconv_null;
      }
    }

    QCString recode(const QCString &s)
    {
      size_t iSize     = s.length();
      size_t oSize     = iSize*4;
      QCString output(oSize, QCString::ExplicitSize);
      size_t iLeft     = iSize;
      size_t oLeft     = oSize;
      const char *iPtr = s.data();
      char *oPtr       = output.rawData();
      if (!portable_iconv(m_fromUtf8,&iPtr,&iLeft,&oPtr,&oLeft))
      {
        oSize -= oLeft;
        output.resize(oSize);
        output.at(oSize)='\0';
        return output;
      }
      else
      {
        return s;
      }
    }
  private:
    void *m_iconv_null = reinterpret_cast<void*>(-1);
    void *m_fromUtf8 = m_iconv_null;

};

//----------------------------------------------------------------------------

/** Class representing a field in the HTML help index. */
struct IndexField
{
  IndexField(const QCString &k,const QCString &n,const QCString &u,const QCString &a,bool l,bool r) :
    key(k), name(n), url(u), anchor(a), link(l), reversed(r) {}
  QCString key;
  QCString name;
  QCString url;
  QCString anchor;
  bool     link;
  bool     reversed;
};

/** A helper class for HtmlHelp that manages a two level index in
 *  alphabetical order.
 */
class HtmlHelpIndex
{
  public:
    HtmlHelpIndex(HtmlHelpRecoder &recoder);
   ~HtmlHelpIndex();
    NON_COPYABLE(HtmlHelpIndex)
    void addItem(const QCString &first,const QCString &second,
                 const QCString &url, const QCString &anchor,
                 bool hasLink,bool reversed);
    void writeFields(std::ostream &t);
    size_t size() const { return m_map.size(); }
  private:
    LinkedMap<IndexField> m_map;
    HtmlHelpRecoder &m_recoder;
};

/*! Constructs a new HtmlHelp index */
HtmlHelpIndex::HtmlHelpIndex(HtmlHelpRecoder &recoder) : m_recoder(recoder)
{
}

/*! Destroys the HtmlHelp index */
HtmlHelpIndex::~HtmlHelpIndex() = default;


/*! Stores an item in the index if it is not already present.
 *  Items are stored in alphabetical order, by sorting on the
 *  concatenation of \a level1 and \a level2 (if present).
 *
 *  \param level1 the string at level 1 in the index.
 *  \param level2 the string at level 2 in the index (or 0 if not applicable).
 *  \param url the url of the documentation (without .html extension).
 *  \param anchor the anchor of the documentation within the page.
 *  \param hasLink if true, the url (without anchor) can be used in the
 *         level1 item, when writing the header of a list of level2 items.
 *  \param reversed TRUE if level1 is the member name and level2 the compound
 *         name.
 */
void HtmlHelpIndex::addItem(const QCString &level1,const QCString &level2,
                       const QCString &url,const QCString &anchor,bool hasLink,
                       bool reversed)
{
  static const reg::Ex re(R"(@\d+)");
  std::string key = level1.str();
  if (!level2.isEmpty()) key+= std::string("?") + level2.str();
  if (reg::search(key,re)) // skip anonymous stuff
  {
    return;
  }
  std::string key_anchor;
  if (!anchor.isEmpty())
  {
    key_anchor = key+anchor.str();
  }
  else
  {
    key_anchor = key;
  }
  m_map.add(key_anchor.c_str(),key.c_str(),url,anchor,hasLink,reversed);
}

static QCString field2URL(const IndexField *f,bool checkReversed)
{
  QCString result = f->url;
  addHtmlExtensionIfMissing(result);
  if (!f->anchor.isEmpty() && (!checkReversed || f->reversed))
  {
    result+="#"+f->anchor;
  }
  return result;
}

static QCString convertToHtmlAndTruncate(const QCString &s)
{
  /* to prevent
   *  Warning: Keyword string:
   *    ...
   *  is too long.  The maximum size is 488 characters.
   */
  int maxLen = 400;
  size_t maxExpandedLen = maxLen+50;
  QCString result = convertToHtml(s,true);
  if (result.length()>maxExpandedLen) // we need to truncate the string
  {
    // in the unlikely case that the string after conversion grows from maxLen to maxExpandedLen, we try smaller parts
    // until we end up below the limit
    while (maxLen>0 && result.length()>maxExpandedLen)
    {
      result = convertToHtml(s.left(maxLen));
      maxLen-=20;
    }
    return result+"...";
  }
  else
  {
    return result;
  }
}

/*! Writes the sorted list of index items into a html like list.
 *
 *  An list of calls with <code>name = level1,level2</code> as follows:
 *  <pre>
 *    a1,b1
 *    a1,b2
 *    a2,b1
 *    a2,b2
 *    a3
 *    a4,b1
 *  </pre>
 *
 *  Will result in the following list:
 *
 *  <pre>
 *    a1       -> link to url if hasLink==TRUE
 *      b1     -> link to url#anchor
 *      b2     -> link to url#anchor
 *    a2       -> link to url if hasLink==TRUE
 *      b1     -> link to url#anchor
 *      b2     -> link to url#anchor
 *    a3       -> link to url if hasLink==TRUE
 *    a4       -> link to url if hasLink==TRUE
 *      b1     -> link to url#anchor
 *  </pre>
 */
void HtmlHelpIndex::writeFields(std::ostream &t)
{
  std::stable_sort(std::begin(m_map),
            std::end(m_map),
            [](const auto &e1,const auto &e2) { return qstricmp_sort(e1->name,e2->name)<0; }
           );
  QCString prevLevel1;
  bool level2Started=FALSE;
  for (auto it = std::begin(m_map); it!=std::end(m_map); ++it)
  {
    auto &f = *it;
    QCString level1,level2;
    int i = f->name.find('?');
    if (i!=-1)
    {
      level1 = f->name.left(i);
      level2 = f->name.right(f->name.length()-i-1);
    }
    else
    {
      level1  = f->name;
    }

    { // finish old list at level 2
      if (level2Started) t << "  </UL>\n";
      level2Started=FALSE;

      // <Antony>
      // Added this code so that an item with only one subitem is written
      // without any subitem.
      // For example:
      //   a1, b1 -> will create only a1, not separate subitem for b1
      //   a2, b2
      //   a2, b3
      QCString nextLevel1;
      auto it_next = std::next(it);
      if (it_next!=std::end(m_map))
      {
        auto &fnext = *it_next;
        int j = fnext->name.find('?');
        if (j<0) j=0;
        nextLevel1 = fnext->name.left(j);
      }
      if (!(level1 == prevLevel1 || level1 == nextLevel1))
      {
        level2 = "";
      }
      prevLevel1 = level1;
      // </Antony>

      if (level2.isEmpty())
      {
        t << "  <LI><OBJECT type=\"text/sitemap\">";
        t << "<param name=\"Local\" value=\"" << field2URL(f.get(),FALSE);
        t << "\">";
        t << "<param name=\"Name\" value=\"" << convertToHtmlAndTruncate(m_recoder.recode(level1)) << "\">"
           "</OBJECT>\n";
      }
      else
      {
        if (f->link)
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"Local\" value=\"" << field2URL(f.get(),TRUE);
          t << "\">";
          t << "<param name=\"Name\" value=\"" << convertToHtmlAndTruncate(m_recoder.recode(level1)) << "\">"
               "</OBJECT>\n";
        }
        else
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"See Also\" value=\"" << convertToHtml(m_recoder.recode(level1)) << "\">";
          t << "<param name=\"Name\" value=\"" << convertToHtmlAndTruncate(m_recoder.recode(level1)) << "\">"
               "</OBJECT>\n";
        }
      }
    }
    if (!level2Started && !level2.isEmpty())
    { // start new list at level 2
      t << "  <UL>\n";
      level2Started=TRUE;
    }
    else if (level2Started && level2.isEmpty())
    { // end list at level 2
      t << "  </UL>\n";
      level2Started=FALSE;
    }
    if (level2Started)
    {
      t << "    <LI><OBJECT type=\"text/sitemap\">";
      t << "<param name=\"Local\" value=\"" << field2URL(f.get(),FALSE);
      t << "\">";
      t << "<param name=\"Name\" value=\"" << convertToHtmlAndTruncate(m_recoder.recode(level2)) << "\">"
         "</OBJECT>\n";
    }
  }
  if (level2Started) t << "  </UL>\n";
}

//----------------------------------------------------------------------------
//
class HtmlHelp::Private
{
  public:
    Private() : index(recoder) {}
    void createProjectFile();
    std::ofstream cts,kts;
    QCString prevFile;
    QCString prevAnc;
    bool ctsItemPresent = false;
    int dc = 0;
    StringSet indexFiles;
    StringSet imageFiles;
    StringSet styleFiles;
    HtmlHelpRecoder recoder;
    HtmlHelpIndex index;
};


/*! Constructs an html object.
 *  The object has to be \link initialize() initialized\endlink before it can
 *  be used.
 */
HtmlHelp::HtmlHelp() : p(std::make_unique<Private>()) {}
HtmlHelp::~HtmlHelp() = default;

/*! This will create a contents file (index.hhc) and a index file (index.hhk)
 *  and write the header of those files.
 *  It also creates a project file (index.hhp)
 *  \sa finalize()
 */
void HtmlHelp::initialize()
{
  p->recoder.initialize();

  /* open the contents file */
  QCString fName = Config_getString(HTML_OUTPUT) + "/" + hhcFileName;
  p->cts = Portable::openOutputStream(fName);
  if (!p->cts.is_open())
  {
    term("Could not open file %s for writing\n",qPrint(fName));
  }
  /* Write the header of the contents file */
  p->cts << "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
         "<HTML><HEAD></HEAD><BODY>\n"
         "<OBJECT type=\"text/site properties\">\n"
         "<param name=\"FrameName\" value=\"right\">\n"
         "</OBJECT>\n"
         "<UL>\n";

  /* open the index file */
  fName = Config_getString(HTML_OUTPUT) + "/" + hhkFileName;
  p->kts = Portable::openOutputStream(fName);
  if (!p->kts.is_open())
  {
    term("Could not open file %s for writing\n",qPrint(fName));
  }
  /* Write the header of the contents file */
  p->kts << "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
         "<HTML><HEAD></HEAD><BODY>\n"
         "<OBJECT type=\"text/site properties\">\n"
         "<param name=\"FrameName\" value=\"right\">\n"
         "</OBJECT>\n"
         "<UL>\n";

}

void HtmlHelp::Private::createProjectFile()
{
  /* Write the project file */
  QCString fName = Config_getString(HTML_OUTPUT) + "/" + hhpFileName;
  std::ofstream t = Portable::openOutputStream(fName);
  if (t.is_open())
  {
    QCString hhcFile = "\"" + hhcFileName  + "\"";
    QCString hhkFile = "\"" + hhkFileName  + "\"";
    bool hhkPresent = index.size()>0;
    if (!ctsItemPresent) hhcFile = "";
    if (!hhkPresent) hhkFile = "";

    QCString indexName="index"+Doxygen::htmlFileExtension;
    t << "[OPTIONS]\n";
    if (!Config_getString(CHM_FILE).isEmpty())
    {
      t << "Compiled file=" << Config_getString(CHM_FILE) << "\n";
    }
    else
    {
      t << "Compiled file=index.chm\n";
    }
    t << "Compatibility=1.1\n"
         "Full-text search=Yes\n";
    if (ctsItemPresent) t << "Contents file=" + hhcFileName + "\n";
    t << "Default Window=main\n"
         "Default topic=" << indexName << "\n";
    if (hhkPresent) t << "Index file=" + hhkFileName + "\n";
    t << "Language=" << theTranslator->getLanguageString() << "\n";
    if (Config_getBool(BINARY_TOC)) t << "Binary TOC=YES\n";
    if (Config_getBool(GENERATE_CHI)) t << "Create CHI file=YES\n";
    t << "Title=" << recoder.recode(Config_getString(PROJECT_NAME)) << "\n\n";

    t << "[WINDOWS]\n";

    // NOTE: the 0x10387e number is a set of bits specifying the buttons
    //       which should appear in the CHM viewer; that specific value
    //       means "show all buttons including the font-size one";
    //       the font-size one is not normally settable by the HTML Help Workshop
    //       utility but the way to set it is described here:
    //          http://support.microsoft.com/?scid=kb%3Ben-us%3B240062&x=17&y=18
    // NOTE: the 0x70387e number in addition to the above the Next and Prev button
    //       are shown. They can only be shown in case of a binary toc.
    //          dee http://www.mif2go.com/xhtml/htmlhelp_0016_943addingtabsandtoolbarbuttonstohtmlhelp.htm#Rz108x95873
    //       Value has been taken from htmlhelp.h file of the HTML Help Workshop
    if (Config_getBool(BINARY_TOC))
    {
      t << "main=\"" << recoder.recode(Config_getString(PROJECT_NAME)) << "\"," << hhcFile << ","
         << hhkFile << ",\"" << indexName << "\",\"" <<
         indexName << "\",,,,,0x23520,,0x70387e,,,,,,,,0\n\n";
    }
    else
    {
      t << "main=\"" << recoder.recode(Config_getString(PROJECT_NAME)) << "\"," << hhcFile << ","
         << hhkFile << ",\"" << indexName << "\",\"" <<
         indexName << "\",,,,,0x23520,,0x10387e,,,,,,,,0\n\n";
    }

    t << "[FILES]\n";
    for (auto &s : indexFiles)
    {
      t << s.c_str() << "\n";
    }
    for (auto &s : imageFiles)
    {
      t << s.c_str() << "\n";
    }
    for (auto &s : styleFiles)
    {
      t << s.c_str() << "\n";
    }
    t.close();
  }
  else
  {
    err("Could not open file %s for writing\n",qPrint(fName));
  }
}

void HtmlHelp::addIndexFile(const QCString &s)
{
  p->indexFiles.insert(s.str());
}

/*! Finalizes the HTML help. This will finish and close the
 *  htmlhelp contents file  and the htmlhelp index file.
 *  \sa initialize()
 */
void HtmlHelp::finalize()
{
  // end the contents file
  p->cts << "</UL>\n";
  p->cts << "</BODY>\n";
  p->cts << "</HTML>\n";
  p->cts.close();

  p->index.writeFields(p->kts);

  // end the index file
  p->kts << "</UL>\n";
  p->kts << "</BODY>\n";
  p->kts << "</HTML>\n";
  p->kts.close();

  p->createProjectFile();

  p->recoder.finalize();
}

/*! Increase the level of the contents hierarchy.
 *  This will start a new unnumbered HTML list in contents file.
 *  \sa decContentsDepth()
 */
void HtmlHelp::incContentsDepth()
{
  for (int i=0; i<p->dc+1; i++) p->cts << "  ";
  p->cts << "<UL>\n";
  ++p->dc;
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the unnumber HTML list.
 *  \sa incContentsDepth()
 */
void HtmlHelp::decContentsDepth()
{
  for (int i=0; i<p->dc; i++) p->cts << "  ";
  p->cts << "</UL>\n";
  --p->dc;
}

/*! Add an list item to the contents file.
 *  \param isDir boolean indicating if this is a dir or file entry
 *  \param name the name of the item.
 *  \param ref  the URL of to the item.
 *  \param file the file in which the item is defined.
 *  \param anchor the anchor of the item.
 *  \param separateIndex not used.
 *  \param addToNavIndex not used.
 *  \param def not used.
 */
void HtmlHelp::addContentsItem(bool isDir,
                               const QCString &name,
                               const QCString &ref,
                               const QCString &file,
                               const QCString &anchor,
                               bool /* separateIndex */,
                               bool /* addToNavIndex */,
                               const Definition * /* def */)
{
  p->ctsItemPresent = true;
  for (int i=0; i<p->dc; i++) p->cts << "  ";
  p->cts << "<LI><OBJECT type=\"text/sitemap\">";
  p->cts << "<param name=\"Name\" value=\"" << convertToHtml(p->recoder.recode(name),TRUE) << "\">";
  if (!file.isEmpty())      // made file optional param - KPW
  {
    if (file[0]=='!' || file[0]=='^') // special markers for user defined URLs
    {
      p->cts << "<param name=\"";
      if (file[0]=='^') p->cts << "URL"; else p->cts << "Local";
      p->cts << "\" value=\"";
      p->cts << &file[1];
      p->cts << "\">";
    }
    else
    {
      QCString currFile = file;
      addHtmlExtensionIfMissing(currFile);
      QCString currAnc = anchor;
      p->cts << "<param name=\"Local\" value=\"";
      if (!ref.isEmpty()) p->cts << externalRef("",ref,true);
      p->cts << currFile;
      if (p->prevFile == currFile && p->prevAnc.isEmpty() && currAnc.isEmpty())
      {
        currAnc = "top";
      }
      if (!currAnc.isEmpty()) p->cts << "#" << currAnc;
      p->cts << "\">";
      p->prevFile = currFile;
      p->prevAnc = currAnc;
    }
  }
  p->cts << "<param name=\"ImageNumber\" value=\"";
  if (isDir)  // added - KPW
  {
    p->cts << static_cast<int>(BOOK_CLOSED);
  }
  else
  {
    p->cts << static_cast<int>(TEXT);
  }
  p->cts << "\">";
  p->cts << "</OBJECT>\n";
}


void HtmlHelp::addIndexItem(const Definition *context,const MemberDef *md,
                            const QCString &sectionAnchor,const QCString &word)
{
  if (context && md)
  {
    QCString cfname  = md->getOutputFileBase();
    QCString argStr  = md->argsString();
    QCString level1  = context->name();
    QCString level2  = md->name() + argStr;
    QCString anchor  = !sectionAnchor.isEmpty() ? sectionAnchor : md->anchor();
    p->index.addItem(level1,level2,cfname,anchor,TRUE,FALSE);
    p->index.addItem(level2,level1,cfname,anchor,TRUE,TRUE);
  }
  else if (context)
  {
    QCString level1  = !word.isEmpty() ? word : context->name();
    p->index.addItem(level1,QCString(),context->getOutputFileBase(),sectionAnchor,TRUE,FALSE);
  }
}

void HtmlHelp::addStyleSheetFile(const QCString &fileName)
{
  p->styleFiles.insert(fileName.str());
}

void HtmlHelp::addImageFile(const QCString &fileName)
{
  p->imageFiles.insert(fileName.str());
}

