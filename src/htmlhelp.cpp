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

//----------------------------------------------------------------------------

/** Helper class to deal with recoding the UTF8 encoded text back to the native encoding
 *  specified by CHM_INDEX_ENCODING.
 */
class HtmlHelpRecoder
{
  public:
    HtmlHelpRecoder() {}
   ~HtmlHelpRecoder() { finalize(); }
    HtmlHelpRecoder(const HtmlHelpRecoder &) = delete;
    HtmlHelpRecoder &operator=(const HtmlHelpRecoder &) = delete;

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
      int iSize        = s.length();
      int oSize        = iSize*4+1;
      QCString output(oSize);
      size_t iLeft     = iSize;
      size_t oLeft     = oSize;
      const char *iPtr = s.data();
      char *oPtr       = output.rawData();
      if (!portable_iconv(m_fromUtf8,&iPtr,&iLeft,&oPtr,&oLeft))
      {
        oSize -= (int)oLeft;
        output.resize(oSize+1);
        output.at(oSize)='\0';
        return output;
      }
      else
      {
        return s;
      }
    }
  private:
    void *m_iconv_null = (void*)(-1);
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
HtmlHelpIndex::~HtmlHelpIndex()
{
}

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
  QCString result = addHtmlExtensionIfMissing(f->url);
  if (!f->anchor.isEmpty() && (!checkReversed || f->reversed))
  {
    // HTML Help needs colons in link anchors to be escaped in the .hhk file.
    result+="#"+substitute(f->anchor,":","%3A");
  }
  return result;
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
  std::sort(std::begin(m_map),
            std::end(m_map),
            [](const auto &e1,const auto &e2) { return e1->name < e2->name; }
           );
  QCString prevLevel1;
  bool level2Started=FALSE;
  for (auto it = std::begin(m_map); it!=std::end(m_map); ++it)
  {
    auto &f = *it;
    QCString level1,level2;
    int i;
    if ((i=f->name.find('?'))!=-1)
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
        t << "<param name=\"Name\" value=\"" << convertToHtml(m_recoder.recode(level1),TRUE) << "\">"
           "</OBJECT>\n";
      }
      else
      {
        if (f->link)
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"Local\" value=\"" << field2URL(f.get(),TRUE);
          t << "\">";
          t << "<param name=\"Name\" value=\"" << convertToHtml(m_recoder.recode(level1),TRUE) << "\">"
               "</OBJECT>\n";
        }
        else
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"See Also\" value=\"" << convertToHtml(m_recoder.recode(level1),TRUE) << "\">";
          t << "<param name=\"Name\" value=\"" << convertToHtml(m_recoder.recode(level1),TRUE) << "\">"
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
      t << "<param name=\"Name\" value=\"" << convertToHtml(m_recoder.recode(level2),TRUE) << "\">"
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
    bool ctsItemPresent = false;
    int dc = 0;
    StringSet indexFiles;
    StringSet imageFiles;
    HtmlHelpRecoder recoder;
    HtmlHelpIndex index;
};


/*! Constructs an html object.
 *  The object has to be \link initialize() initialized\endlink before it can
 *  be used.
 */
HtmlHelp::HtmlHelp() : p(std::make_unique<Private>())
{
}

HtmlHelp::~HtmlHelp()
{
}

/* language codes for Html help
   0x405 Czech
   0x406 Danish
   0x413 Dutch
   0xC09 English (Australia)
   0x809 English (Britain)
   0x1009 English (Canada)
   0x1809 English (Ireland)
   0x1409 English (New Zealand)
   0x1C09 English (South Africa)
   0x409 English (United States)
   0x40B Finnish
   0x40C French
   0x407 German
   0x408 Greece
   0x40E Hungarian
   0x410 Italian
   0x814 Norwegian
   0x415 Polish
   0x816 Portuguese(Portugal)
   0x416 Portuguese(Brazil)
   0x419 Russian
   0x80A Spanish(Mexico)
   0xC0A Spanish(Modern Sort)
   0x40A Spanish(Traditional Sort)
   0x41D Swedish
   0x41F Turkey
   0x411 Japanese
   0x412 Korean
   0x804 Chinese (PRC)
   0x404 Chinese (Taiwan)

   New LCIDs:
   0x421 Indonesian
   0x41A Croatian
   0x418 Romanian
   0x424 Slovenian
   0x41B Slovak
   0x422 Ukrainian
   0x81A Serbian (Serbia, Latin)
   0x403 Catalan
   0x426 Latvian
   0x427 Lithuanian
   0x436 Afrikaans
   0x42A Vietnamese
   0x429 Persian (Iran)
   0xC01 Arabic (Egypt) - I don't know which version of arabic is used inside translator_ar.h ,
   so I have chosen Egypt at random

*/
static StringUnorderedMap s_languageDict =
{
  { "czech",       "0x405 Czech"                     },
  { "danish",      "0x406 Danish"                    },
  { "dutch",       "0x413 Dutch"                     },
  { "finnish",     "0x40B Finnish"                   },
  { "french",      "0x40C French"                    },
  { "german",      "0x407 German"                    },
  { "greek",       "0x408 Greece"                    },
  { "hungarian",   "0x40E Hungarian"                 },
  { "italian",     "0x410 Italian"                   },
  { "norwegian",   "0x814 Norwegian"                 },
  { "polish",      "0x415 Polish"                    },
  { "portuguese",  "0x816 Portuguese(Portugal)"      },
  { "brazilian",   "0x416 Portuguese(Brazil)"        },
  { "russian",     "0x419 Russian"                   },
  { "spanish",     "0x40A Spanish(Traditional Sort)" },
  { "swedish",     "0x41D Swedish"                   },
  { "turkish",     "0x41F Turkey"                    },
  { "japanese",    "0x411 Japanese"                  },
  { "japanese-en", "0x411 Japanese"                  },
  { "korean",      "0x412 Korean"                    },
  { "korean-en",   "0x412 Korean"                    },
  { "chinese",     "0x804 Chinese (PRC)"             },
  { "chinese-traditional", "0x404 Chinese (Taiwan)"  },
  { "indonesian",  "0x421 Indonesian"                },
  { "croatian",    "0x41A Croatian"                  },
  { "romanian",    "0x418 Romanian"                  },
  { "slovene",     "0x424 Slovenian"                 },
  { "slovak",      "0x41B Slovak"                    },
  { "ukrainian",   "0x422 Ukrainian"                 },
  { "serbian",     "0x81A Serbian (Serbia, Latin)"   },
  { "catalan",     "0x403 Catalan"                   },
  { "lithuanian",  "0x427 Lithuanian"                },
  { "afrikaans",   "0x436 Afrikaans"                 },
  { "vietnamese",  "0x42A Vietnamese"                },
  { "persian",     "0x429 Persian (Iran)"            },
  { "arabic",      "0xC01 Arabic (Egypt)"            },
  { "latvian",     "0x426 Latvian"                   },
  { "macedonian",  "0x042f Macedonian (Former Yugoslav Republic of Macedonia)" },
  { "armenian",    "0x42b Armenian"                  },
  //Code for Esperanto should be as shown below but the htmlhelp compiler 1.3 does not support this
  // (and no newer version is available).
  //So do a fallback to the default language (see getLanguageString())
  //{ "esperanto",   "0x48f Esperanto" },
  { "serbian-cyrillic", "0xC1A Serbian (Serbia, Cyrillic)" }
};

/*! This will create a contents file (index.hhc) and a index file (index.hhk)
 *  and write the header of those files.
 *  It also creates a project file (index.hhp)
 *  \sa finalize()
 */
void HtmlHelp::initialize()
{
  p->recoder.initialize();

  /* open the contents file */
  QCString fName = Config_getString(HTML_OUTPUT) + "/index.hhc";
  p->cts.open(fName.str(),std::ofstream::out | std::ofstream::binary);
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

  /* open the contents file */
  fName = Config_getString(HTML_OUTPUT) + "/index.hhk";
  p->kts.open(fName.str(),std::ofstream::out | std::ofstream::binary);
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


QCString HtmlHelp::getLanguageString()
{
  if (!theTranslator->idLanguage().isEmpty())
  {
    auto it = s_languageDict.find(theTranslator->idLanguage().str());
    if (it!=s_languageDict.end())
    {
      return QCString(it->second);
    }
  }
  // default language
  return "0x409 English (United States)";
}



void HtmlHelp::Private::createProjectFile()
{
  /* Write the project file */
  QCString fName = Config_getString(HTML_OUTPUT) + "/index.hhp";
  std::ofstream t(fName.str(),std::ofstream::out | std::ofstream::binary);
  if (t.is_open())
  {
    const char *hhcFile = "\"index.hhc\"";
    const char *hhkFile = "\"index.hhk\"";
    bool hhkPresent = index.size()>0;
    if (!ctsItemPresent) hhcFile = "";
    if (!hhkPresent) hhkFile = "";

    QCString indexName="index"+Doxygen::htmlFileExtension;
    t << "[OPTIONS]\n";
    if (!Config_getString(CHM_FILE).isEmpty())
    {
      t << "Compiled file=" << Config_getString(CHM_FILE) << "\n";
    }
    t << "Compatibility=1.1\n"
         "Full-text search=Yes\n";
    if (ctsItemPresent) t << "Contents file=index.hhc\n";
    t << "Default Window=main\n"
         "Default topic=" << indexName << "\n";
    if (hhkPresent) t << "Index file=index.hhk\n";
    t << "Language=" << getLanguageString() << "\n";
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
 *  contents file (index.hhc) and the index file (index.hhk).
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
  int i; for (i=0;i<p->dc+1;i++) p->cts << "  ";
  p->cts << "<UL>\n";
  ++p->dc;
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the unnumber HTML list.
 *  \sa incContentsDepth()
 */
void HtmlHelp::decContentsDepth()
{
  int i; for (i=0;i<p->dc;i++) p->cts << "  ";
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
                               const QCString & /*ref*/,
                               const QCString &file,
                               const QCString &anchor,
                               bool /* separateIndex */,
                               bool /* addToNavIndex */,
                               const Definition * /* def */)
{
  static bool binaryTOC = Config_getBool(BINARY_TOC);
  // If we're using a binary toc then folders cannot have links.
  // Tried this and I didn't see any problems, when not using
  // the resetting of file and anchor the TOC works better
  // (prev / next button)
  //if(Config_getBool(BINARY_TOC) && isDir)
  //{
    //file = 0;
    //anchor = 0;
  //}
  p->ctsItemPresent = true;
  int i; for (i=0;i<p->dc;i++) p->cts << "  ";
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
      if (!(binaryTOC && isDir))
      {
        p->cts << "<param name=\"Local\" value=\"";
        p->cts << addHtmlExtensionIfMissing(file);
        if (!anchor.isEmpty()) p->cts << "#" << anchor;
        p->cts << "\">";
      }
    }
  }
  p->cts << "<param name=\"ImageNumber\" value=\"";
  if (isDir)  // added - KPW
  {
    p->cts << (int)BOOK_CLOSED ;
  }
  else
  {
    p->cts << (int)TEXT;
  }
  p->cts << "\">";
  p->cts << "</OBJECT>\n";
}


void HtmlHelp::addIndexItem(const Definition *context,const MemberDef *md,
                            const QCString &sectionAnchor,const QCString &word)
{
  if (md)
  {
    static bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
    if (context==0) // global member
    {
      if (md->getGroupDef())
        context = md->getGroupDef();
      else if (md->getFileDef())
        context = md->getFileDef();
    }
    if (context==0) return; // should not happen

    QCString cfname  = md->getOutputFileBase();
    QCString cfiname = context->getOutputFileBase();
    QCString level1  = context->name();
    QCString level2  = md->name();
    QCString contRef = separateMemberPages ? cfname : cfiname;
    QCString memRef  = cfname;
    QCString anchor  = !sectionAnchor.isEmpty() ? sectionAnchor : md->anchor();
    p->index.addItem(level1,level2,contRef,anchor,TRUE,FALSE);
    p->index.addItem(level2,level1,memRef,anchor,TRUE,TRUE);
  }
  else if (context)
  {
    QCString level1  = !word.isEmpty() ? word : context->name();
    p->index.addItem(level1,QCString(),context->getOutputFileBase(),sectionAnchor,TRUE,FALSE);
  }
}

void HtmlHelp::addImageFile(const QCString &fileName)
{
  p->imageFiles.insert(fileName.str());
}

