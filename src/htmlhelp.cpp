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

// own header
#include "htmlhelp.h"

// standard includes
#include <algorithm>
#include <cstdio>
#include <cstdlib>

// other includes
#include "config.h"
#include "doxygen.h"
#include "filedef.h"
#include "fileinfo.h"
#include "language.h"
#include "linkedmap.h"
#include "memberdef.h"
#include "message.h"
#include "portable.h"
#include "regex.h"
#include "util.h"

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
      DString str = Config_getString(CHM_INDEX_ENCODING);
      if (str.empty()) str = "CP1250"; // use safe and likely default
      m_fromUtf8 = portable_iconv_open(str.data(),"UTF-8");
      if (m_fromUtf8==m_iconv_null)
      {
        term("unsupported character conversion for CHM_INDEX_ENCODING: '{}'->'UTF-8'\n", str);
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

    DString recode(const DString &s)
    {
      size_t iSize     = s.length();
      size_t oSize     = iSize*4;
      DString output(oSize, DString::ExplicitSize);
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
  IndexField(const DString &k,const DString &n,const DString &u,const DString &a,bool l,bool r) :
    key(k), name(n), url(u), anchor(a), link(l), reversed(r) {}
  DString key;
  DString name;
  DString url;
  DString anchor;
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
    void addItem(const DString &first,const DString &second,
                 const DString &url, const DString &anchor,
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
 *  \param reversed true if level1 is the member name and level2 the compound
 *         name.
 */
void HtmlHelpIndex::addItem(const DString &level1,const DString &level2,
                       const DString &url,const DString &anchor,bool hasLink,
                       bool reversed)
{
  static const reg::Ex re(R"(@\d+)");
  DString key = substitute(level1,"?","&quest;");
  if (!level2.empty()) key+= "?" + substitute(level2,"?","&quest;");
  if (reg::search(key.str(),re)) // skip anonymous stuff
  {
    return;
  }
  DString key_anchor = key;
  if (!anchor.empty())
  {
    key_anchor += anchor;
  }
  m_map.add(key_anchor,key,url,anchor,hasLink,reversed);
}

static DString field2URL(const IndexField *f,bool checkReversed)
{
  DString result = f->url;
  addHtmlExtensionIfMissing(result);
  if (!f->anchor.empty() && (!checkReversed || f->reversed))
  {
    result+="#"+f->anchor;
  }
  return result;
}

static DString convertToHtmlAndTruncate(const DString &s)
{
  /* to prevent
   *  Warning: Keyword string:
   *    ...
   *  is too long.  The maximum size is 488 characters.
   */
  int maxLen = 400;
  size_t maxExpandedLen = maxLen+50;
  DString result = convertToHtml(s,true);
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
 *    a1       -> link to url if hasLink==true
 *      b1     -> link to url#anchor
 *      b2     -> link to url#anchor
 *    a2       -> link to url if hasLink==true
 *      b1     -> link to url#anchor
 *      b2     -> link to url#anchor
 *    a3       -> link to url if hasLink==true
 *    a4       -> link to url if hasLink==true
 *      b1     -> link to url#anchor
 *  </pre>
 */
void HtmlHelpIndex::writeFields(std::ostream &t)
{
  std::stable_sort(std::begin(m_map),
            std::end(m_map),
            [](const auto &e1,const auto &e2) { return dstricmp_sort(e1->name,e2->name)<0; }
           );
  DString prevLevel1;
  bool level2Started=false;
  for (auto it = std::begin(m_map); it!=std::end(m_map); ++it)
  {
    auto &f = *it;
    DString level1,level2;
    if (size_t i = f->name.find('?'); i!=DString::npos)
    {
      level1 = f->name.left(i);
      level2 = f->name.mid(i+1);
    }
    else
    {
      level1  = f->name;
    }

    { // finish old list at level 2
      if (level2Started) t << "  </UL>\n";
      level2Started=false;

      // <Antony>
      // Added this code so that an item with only one subitem is written
      // without any subitem.
      // For example:
      //   a1, b1 -> will create only a1, not separate subitem for b1
      //   a2, b2
      //   a2, b3
      DString nextLevel1;
      auto it_next = std::next(it);
      if (it_next!=std::end(m_map))
      {
        auto &fnext = *it_next;
        size_t j = fnext->name.find('?');
        if (j==DString::npos) j=0;
        nextLevel1 = fnext->name.left(j);
      }
      if (!(level1 == prevLevel1 || level1 == nextLevel1))
      {
        level2 = "";
      }
      prevLevel1 = level1;
      // </Antony>

      if (level2.empty())
      {
        t << "  <LI><OBJECT type=\"text/sitemap\">";
        t << "<param name=\"Local\" value=\"" << field2URL(f.get(),false);
        t << "\">";
        t << "<param name=\"Name\" value=\"" << convertToHtmlAndTruncate(m_recoder.recode(level1)) << "\">"
           "</OBJECT>\n";
      }
      else
      {
        if (f->link)
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"Local\" value=\"" << field2URL(f.get(),true);
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
    if (!level2Started && !level2.empty())
    { // start new list at level 2
      t << "  <UL>\n";
      level2Started=true;
    }
    else if (level2Started && level2.empty())
    { // end list at level 2
      t << "  </UL>\n";
      level2Started=false;
    }
    if (level2Started)
    {
      t << "    <LI><OBJECT type=\"text/sitemap\">";
      t << "<param name=\"Local\" value=\"" << field2URL(f.get(),false);
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
    DString prevFile;
    DString prevAnc;
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
  DString fName = Config_getString(HTML_OUTPUT) + "/" + hhcFileName;
  p->cts = Portable::openOutputStream(fName);
  if (!p->cts.is_open())
  {
    term("Could not open file {} for writing\n",fName);
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
    term("Could not open file {} for writing\n",fName);
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
  DString fName = Config_getString(HTML_OUTPUT) + "/" + hhpFileName;
  std::ofstream t = Portable::openOutputStream(fName);
  if (t.is_open())
  {
    DString hhcFile = "\"" + hhcFileName  + "\"";
    DString hhkFile = "\"" + hhkFileName  + "\"";
    bool hhkPresent = index.size()>0;
    if (!ctsItemPresent) hhcFile = "";
    if (!hhkPresent) hhkFile = "";

    DString indexName="index"+Doxygen::htmlFileExtension;
    t << "[OPTIONS]\n";
    if (!Config_getString(CHM_FILE).empty())
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
      t << s << "\n";
    }
    for (auto &s : imageFiles)
    {
      t << s << "\n";
    }
    for (auto &s : styleFiles)
    {
      t << s << "\n";
    }
    t.close();
  }
  else
  {
    err("Could not open file {} for writing\n",fName);
  }
}

void HtmlHelp::addIndexFile(const DString &s)
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
 *  \param nameAsHtml name parameter in HTML format
 */
void HtmlHelp::addContentsItem(bool isDir,
                               const DString &name,
                               const DString &ref,
                               const DString &file,
                               const DString &anchor,
                               bool /* separateIndex */,
                               bool /* addToNavIndex */,
                               const Definition * /* def */,
                               const DString & /* nameAsHtml */)
{
  p->ctsItemPresent = true;
  for (int i=0; i<p->dc; i++) p->cts << "  ";
  p->cts << "<LI><OBJECT type=\"text/sitemap\">";
  p->cts << "<param name=\"Name\" value=\"" << convertToHtml(p->recoder.recode(name),true) << "\">";
  if (!file.empty())      // made file optional param - KPW
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
      DString currFile = file;
      addHtmlExtensionIfMissing(currFile);
      DString currAnc = anchor;
      p->cts << "<param name=\"Local\" value=\"";
      if (!ref.empty()) p->cts << externalRef("",ref);
      p->cts << currFile;
      if (p->prevFile == currFile && p->prevAnc.empty() && currAnc.empty())
      {
        currAnc = "top";
      }
      if (!currAnc.empty()) p->cts << "#" << currAnc;
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
                            const DString &sectionAnchor,const DString &word)
{
  if (context && md)
  {
    if (sectionAnchor.empty() && !md->hasDocumentation()) return;
    DString cfname  = md->getOutputFileBase();
    DString argStr  = md->argsString();
    DString level1  = context->name();
    DString level2  = md->name() + argStr;
    DString anchor  = !sectionAnchor.empty() ? sectionAnchor : md->anchor();
    p->index.addItem(level1,level2,cfname,anchor,true,false);
    p->index.addItem(level2,level1,cfname,anchor,true,true);
  }
  else if (context)
  {
    DString level1  = !word.empty() ? word : context->name();
    p->index.addItem(level1,DString(),context->getOutputFileBase(),sectionAnchor,true,false);
  }
}

void HtmlHelp::addStyleSheetFile(const DString &fileName)
{
  p->styleFiles.insert(fileName.str());
}

void HtmlHelp::addImageFile(const DString &fileName)
{
  p->imageFiles.insert(fileName.str());
}

