/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qdict.h>
#include <qregexp.h>
#include "qtextcodec.h"
#include "sortdict.h"

#include "htmlhelp.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"
#include "portable.h"

//----------------------------------------------------------------------------

/** Class representing a field in the HTML help index. */
struct IndexField
{
  QCString name;
  QCString url;
  QCString anchor;
  bool     link;
  bool     reversed;
};

/** Sorted dictionary of IndexField objects. */
class IndexFieldSDict : public SDict<IndexField>
{
  public:
    IndexFieldSDict() : SDict<IndexField>(17) {}
   ~IndexFieldSDict() {}
    int compareItems(GCI item1, GCI item2)
    {
      return stricmp(((IndexField *)item1)->name,((IndexField *)item2)->name);
    }
};

/** A helper class for HtmlHelp that manages a two level index in 
 *  alphabetical order.
 */
class HtmlHelpIndex
{
  public:
    HtmlHelpIndex(HtmlHelp *help);
   ~HtmlHelpIndex();
    void addItem(const char *first,const char *second, 
                 const char *url, const char *anchor,
                 bool hasLink,bool reversed);
    void writeFields(FTextStream &t);
  private:
    IndexFieldSDict *dict;   
    HtmlHelp *m_help;
};

/*! Constructs a new HtmlHelp index */
HtmlHelpIndex::HtmlHelpIndex(HtmlHelp *help) : m_help(help)
{
  dict = new IndexFieldSDict;
  dict->setAutoDelete(TRUE);
}

/*! Destroys the HtmlHelp index */
HtmlHelpIndex::~HtmlHelpIndex()
{
  delete dict;
}

/*! Stores an item in the index if it is not already present. 
 *  Items are stored in alphetical order, by sorting on the
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
void HtmlHelpIndex::addItem(const char *level1,const char *level2,
                       const char *url,const char *anchor,bool hasLink,
                       bool reversed)
{
  QCString key = level1; 
  if (level2) key+= (QCString)"?" + level2;
  if (key.find(QRegExp("@[0-9]+"))!=-1) // skip anonymous stuff
  {
    return;
  }
  if (dict->find(key)==0) // new key
  {
    //printf(">>>>>>>>> HtmlHelpIndex::addItem(%s,%s,%s,%s)\n",
    //      level1,level2,url,anchor);
    IndexField *f = new IndexField;
    f->name     = key;
    f->url      = url;
    f->anchor   = anchor;
    f->link     = hasLink;
    f->reversed = reversed;
    dict->append(key,f);
  }
}

static QCString field2URL(const IndexField *f,bool checkReversed)
{
  QCString result = f->url + Doxygen::htmlFileExtension;
  if (!f->anchor.isEmpty() && (!checkReversed || f->reversed)) 
  {
    result+="#"+f->anchor;  
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
void HtmlHelpIndex::writeFields(FTextStream &t)
{
  dict->sort();
  IndexFieldSDict::Iterator ifli(*dict);
  IndexField *f;
  QCString lastLevel1;
  bool level2Started=FALSE;
  for (;(f=ifli.current());++ifli)
  {
    QCString level1,level2;
    int i;
    if ((i=f->name.find('?'))!=-1)
    {
      level1 = f->name.left(i);
      level2 = f->name.right(f->name.length()-i-1); 
    }
    else
    {
      level1  = f->name.copy();
    }

    if (level1!=lastLevel1)
    { // finish old list at level 2
      if (level2Started) t << "  </UL>" << endl;
      level2Started=FALSE;
    
      // <Antony>
      // Added this code so that an item with only one subitem is written
      // without any subitem.
      // For example:
      //   a1, b1 -> will create only a1, not separate subitem for b1
      //   a2, b2
      //   a2, b3
      QCString nextLevel1;
      IndexField* fnext = ++ifli;
      if (fnext)
      {
        nextLevel1 = fnext->name.left(fnext->name.find('?'));
        --ifli;
      }
      if (level1 != nextLevel1)
      {
        level2 = "";
      }
      // </Antony>

      if (level2.isEmpty())
      {
        t << "  <LI><OBJECT type=\"text/sitemap\">";
        t << "<param name=\"Local\" value=\"" << field2URL(f,TRUE);
        t << "\">";
        t << "<param name=\"Name\" value=\"" << m_help->recode(level1) << "\">"
           "</OBJECT>\n";
      }
      else
      {
        if (f->link)
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"Local\" value=\"" << field2URL(f,TRUE);
          t << "\">";
          t << "<param name=\"Name\" value=\"" << m_help->recode(level1) << "\">"
               "</OBJECT>\n";
        }
        else
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"See Also\" value=\"" << m_help->recode(level1) << "\">";
          t << "<param name=\"Name\" value=\"" << m_help->recode(level1) << "\">"
               "</OBJECT>\n";
        }
      }
    }
    if (!level2Started && !level2.isEmpty())
    { // start new list at level 2
      t << "  <UL>" << endl;
      level2Started=TRUE;
    }
    else if (level2Started && level2.isEmpty())
    { // end list at level 2
      t << "  </UL>" << endl;
      level2Started=FALSE;
    }
    if (level2Started)
    {
      t << "    <LI><OBJECT type=\"text/sitemap\">";
      t << "<param name=\"Local\" value=\"" << field2URL(f,FALSE);
      t << "\">";
      t << "<param name=\"Name\" value=\"" << m_help->recode(level2) << "\">"
         "</OBJECT>\n";
    }
    lastLevel1 = level1.copy();
  } 
  if (level2Started) t << "  </UL>" << endl;
}

//----------------------------------------------------------------------------

HtmlHelp *HtmlHelp::theInstance = 0;

/*! Constructs an html object. 
 *  The object has to be \link initialize() initialized\endlink before it can 
 *  be used.
 */
HtmlHelp::HtmlHelp() : indexFileDict(1009)
{
  /* initial depth */
  dc = 0;
  cf = kf = 0;
  index = new HtmlHelpIndex(this);
  m_fromUtf8 = (void *)(-1);
}

HtmlHelp::~HtmlHelp()
{
  if (m_fromUtf8!=(void *)(-1))   portable_iconv_close(m_fromUtf8);
}
#if 0
/*! return a reference to the one and only instance of this class. 
 */
HtmlHelp *HtmlHelp::getInstance()
{
  if (theInstance==0) theInstance = new HtmlHelp;
  return theInstance;
}
#endif

static QDict<QCString> s_languageDict;

/*! This will create a contents file (index.hhc) and a index file (index.hhk)
 *  and write the header of those files. 
 *  It also creates a project file (index.hhp)
 *  \sa finalize()
 */
void HtmlHelp::initialize()
{
  const char *str = Config_getString("CHM_INDEX_ENCODING");
  if (!str) str = "CP1250"; // use safe and likely default
  m_fromUtf8 = portable_iconv_open(str,"UTF-8"); 
  if (m_fromUtf8==(void *)(-1))
  {
    err("Error: unsupported character conversion for CHM_INDEX_ENCODING: '%s'->'UTF-8'\n", str);
    exit(1);
  }

  /* open the contents file */
  QCString fName = Config_getString("HTML_OUTPUT") + "/index.hhc";
  cf = new QFile(fName);
  if (!cf->open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",fName.data());
    exit(1);
  }
  /* Write the header of the contents file */
  cts.setDevice(cf);
  cts << "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
         "<HTML><HEAD></HEAD><BODY>\n"
         "<OBJECT type=\"text/site properties\">\n"
         "<param name=\"FrameName\" value=\"right\">\n"
         "</OBJECT>\n"
         "<UL>\n";
  
  /* open the contents file */
  fName = Config_getString("HTML_OUTPUT") + "/index.hhk";
  kf = new QFile(fName);
  if (!kf->open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",fName.data());
    exit(1);
  }
  /* Write the header of the contents file */
  kts.setDevice(kf);
  kts << "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
         "<HTML><HEAD></HEAD><BODY>\n"
         "<OBJECT type=\"text/site properties\">\n"
         "<param name=\"FrameName\" value=\"right\">\n"
         "</OBJECT>\n"
         "<UL>\n";

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
	 0x427 Lithuanian
	 0x436 Afrikaans
	 0x42A Vietnamese
	 0x429 Persian (Iran)
	 0xC01 Arabic (Egypt) - I don't know which version of arabic is used inside translator_ar.h ,
     so I have chosen Egypt at random

  */
  s_languageDict.setAutoDelete(TRUE);
  s_languageDict.clear();
  s_languageDict.insert("czech",       new QCString("0x405 Czech"));
  s_languageDict.insert("danish",      new QCString("0x406 Danish"));
  s_languageDict.insert("dutch",       new QCString("0x413 Dutch"));
  s_languageDict.insert("finnish",     new QCString("0x40B Finnish"));
  s_languageDict.insert("french",      new QCString("0x40C French"));
  s_languageDict.insert("german",      new QCString("0x407 German"));
  s_languageDict.insert("greek",       new QCString("0x408 Greece"));
  s_languageDict.insert("hungarian",   new QCString("0x40E Hungarian"));
  s_languageDict.insert("italian",     new QCString("0x410 Italian"));
  s_languageDict.insert("norwegian",   new QCString("0x814 Norwegian"));
  s_languageDict.insert("polish",      new QCString("0x415 Polish"));
  s_languageDict.insert("portuguese",  new QCString("0x816 Portuguese(Portugal)"));
  s_languageDict.insert("brazil",      new QCString("0x416 Portuguese(Brazil)"));
  s_languageDict.insert("russian",     new QCString("0x419 Russian"));
  s_languageDict.insert("spanish",     new QCString("0x40A Spanish(Traditional Sort)"));
  s_languageDict.insert("swedish",     new QCString("0x41D Swedish"));
  s_languageDict.insert("turkish",     new QCString("0x41F Turkey"));
  s_languageDict.insert("japanese",    new QCString("0x411 Japanese"));
  s_languageDict.insert("japanese-en", new QCString("0x411 Japanese"));
  s_languageDict.insert("korean",      new QCString("0x412 Korean"));
  s_languageDict.insert("korean-en",   new QCString("0x412 Korean"));
  s_languageDict.insert("chinese",     new QCString("0x804 Chinese (PRC)"));
  s_languageDict.insert("chinese-traditional", new QCString("0x404 Chinese (Taiwan)"));

  // new LCIDs
  s_languageDict.insert("indonesian",  new QCString("0x412 Indonesian"));
  s_languageDict.insert("croatian",    new QCString("0x41A Croatian"));
  s_languageDict.insert("romanian",    new QCString("0x418 Romanian"));
  s_languageDict.insert("slovene",     new QCString("0x424 Slovenian"));
  s_languageDict.insert("slovak",      new QCString("0x41B Slovak"));
  s_languageDict.insert("ukrainian",   new QCString("0x422 Ukrainian"));
  s_languageDict.insert("serbian",     new QCString("0x81A Serbian (Serbia, Latin)"));
  s_languageDict.insert("catalan",     new QCString("0x403 Catalan"));
  s_languageDict.insert("lithuanian",  new QCString("0x427 Lithuanian"));
  s_languageDict.insert("afrikaans",   new QCString("0x436 Afrikaans"));
  s_languageDict.insert("vietnamese",  new QCString("0x42A Vietnamese"));
  s_languageDict.insert("persian",     new QCString("0x429 Persian (Iran)"));
  s_languageDict.insert("arabic",      new QCString("0xC01 Arabic (Egypt)"));
}


static QCString getLanguageString()
{
  if (!theTranslator->idLanguage().isEmpty())
  {
    QCString *s = s_languageDict[theTranslator->idLanguage()];
    if (s)
    {
      return *s;
    }
  }
  // default language
  return "0x409 English (United States)";
}
  


void HtmlHelp::createProjectFile()
{
  /* Write the project file */
  QCString fName = Config_getString("HTML_OUTPUT") + "/index.hhp";
  QFile f(fName);
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    
    QCString indexName="index"+Doxygen::htmlFileExtension;
    //if (Config_getBool("GENERATE_TREEVIEW")) indexName="main"+Doxygen::htmlFileExtension;
    t << "[OPTIONS]\n";
    if (!Config_getString("CHM_FILE").isEmpty())
    {
      t << "Compiled file=" << Config_getString("CHM_FILE") << "\n";
    }
    t << "Compatibility=1.1\n"
         "Full-text search=Yes\n"
         "Contents file=index.hhc\n"
         "Default Window=main\n"
         "Default topic=" << indexName << "\n"
         "Index file=index.hhk\n"
         "Language=" << getLanguageString() << endl;
    if (Config_getBool("BINARY_TOC")) t << "Binary TOC=YES\n";
    if (Config_getBool("GENERATE_CHI")) t << "Create CHI file=YES\n";
    t << "Title=" << recode(Config_getString("PROJECT_NAME")) << endl << endl;
    
    t << "[WINDOWS]" << endl;

    // NOTE: the 0x10387e number is a set of bits specifying the buttons
    //       which should appear in the CHM viewer; that specific value
    //       means "show all buttons including the font-size one";
    //       the font-size one is not normally settable by the HTML Help Workshop
    //       utility but the way to set it is described here:
    //          http://support.microsoft.com/?scid=kb%3Ben-us%3B240062&x=17&y=18
    t << "main=\"" << recode(Config_getString("PROJECT_NAME")) << "\",\"index.hhc\","
         "\"index.hhk\",\"" << indexName << "\",\"" << 
         indexName << "\",,,,,0x23520,,0x10387e,,,,,,,,0" << endl << endl;
    
    t << "[FILES]" << endl;
    char *s = indexFiles.first();
    while (s)
    {
      t << s << endl;
      s = indexFiles.next();
    }
#if 0
    // items not found by the html help compiler scan.
    t << "tabs.css" << endl;
    t << "tab_a.png" << endl;
    t << "tab_b.png" << endl;
    t << "tab_h.png" << endl;
    t << "tab_s.png" << endl;
    t << "nav_h.png" << endl;
    t << "nav_f.png" << endl;
    t << "bc_s.png" << endl;
    if (Config_getBool("HTML_DYNAMIC_SECTIONS"))
    {
      t << "open.png" << endl;
      t << "closed.png" << endl;
    }
    if (Config_getBool("GENERATE_HTMLHELP"))
    {
      t << "ftv2blank.png" << endl;
      t << "ftv2doc.png" << endl;
      t << "ftv2folderclosed.png" << endl;
      t << "ftv2folderopen.png" << endl;
      t << "ftv2lastnode.png" << endl;
      t << "ftv2link.png" << endl;
      t << "ftv2mlastnode.png" << endl;
      t << "ftv2mnode.png" << endl;
      t << "ftv2node.png" << endl;
      t << "ftv2plastnode.png" << endl;
      t << "ftv2pnode.png" << endl;
      t << "ftv2vertline.png" << endl;
    }
    if (Config_getBool("SEARCHENGINE"))
    {
      t << "search_l.png" << endl;
      t << "search_m.png" << endl;
      t << "search_r.png" << endl;
      if (Config_getBool("SERVER_BASED_SEARCH"))
      {
        t << "mag.png" << endl;
      }
      else
      {
        t << "mag_sel.png" << endl;
        t << "close.png" << endl;
      }
    }
#endif
    uint i;
    for (i=0;i<imageFiles.count();i++)
    {
      t << imageFiles.at(i) << endl;
    }
    f.close();
  }
  else
  {
    err("Could not open file %s for writing\n",fName.data());
  }
}

void HtmlHelp::addIndexFile(const char *s)
{
  if (indexFileDict.find(s)==0)
  {
    indexFiles.append(s);
    indexFileDict.insert(s,(void *)0x8);
  }
}

/*! Finalizes the HTML help. This will finish and close the
 *  contents file (index.hhc) and the index file (index.hhk).
 *  \sa initialize()
 */
void HtmlHelp::finalize()
{
  // end the contents file
  cts << "</UL>\n";
  cts << "</BODY>\n";
  cts << "</HTML>\n";
  cts.unsetDevice();
  cf->close();
  delete cf;
  
  index->writeFields(kts);
  
  // end the index file
  kts << "</UL>\n";
  kts << "</BODY>\n";
  kts << "</HTML>\n";
  kts.unsetDevice();
  kf->close();
  delete kf;

  createProjectFile();
  s_languageDict.clear();
}

/*! Increase the level of the contents hierarchy. 
 *  This will start a new unnumbered HTML list in contents file.
 *  \sa decContentsDepth()
 */
void HtmlHelp::incContentsDepth()
{
  int i; for (i=0;i<dc+1;i++) cts << "  ";
  cts << "<UL>\n";
  ++dc;
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the unnumber HTML list.
 *  \sa incContentsDepth()
 */
void HtmlHelp::decContentsDepth()
{
  int i; for (i=0;i<dc;i++) cts << "  ";
  cts << "</UL>\n";
  --dc;
}

QCString HtmlHelp::recode(const QCString &s) 
{
  int iSize        = s.length();
  int oSize        = iSize*4+1;
  QCString output(oSize);
  size_t iLeft     = iSize;
  size_t oLeft     = oSize;
  char *iPtr       = s.data();
  char *oPtr       = output.data();
  if (!portable_iconv(m_fromUtf8,&iPtr,&iLeft,&oPtr,&oLeft))
  {
    oSize -= oLeft;
    output.resize(oSize+1);
    output.at(oSize)='\0';
    return output;
  }
  else
  {
    return s;
  }
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
                               const char *name,
                               const char * /*ref*/, 
                               const char *file,
                               const char *anchor,
                               bool /* separateIndex */,
                               bool /* addToNavIndex */,
                               Definition * /* def */)
{
  // If we're using a binary toc then folders cannot have links. 
  if(Config_getBool("BINARY_TOC") && isDir) 
  {
    file = 0;
    anchor = 0;
  }
  int i; for (i=0;i<dc;i++) cts << "  ";
  cts << "<LI><OBJECT type=\"text/sitemap\">";
  cts << "<param name=\"Name\" value=\"" << recode(name) << "\">";
  if (file)      // made file optional param - KPW
  {
    if (file && (file[0]=='!' || file[0]=='^')) // special markers for user defined URLs
    {
      cts << "<param name=\"";
      if (file[0]=='^') cts << "URL"; else cts << "Local";
      cts << "\" value=\"";
      cts << &file[1];
    }
    else
    {
      cts << "<param name=\"Local\" value=\"";
      cts << file << Doxygen::htmlFileExtension;
      if (anchor) cts << "#" << anchor;  
    }
    cts << "\">";
  }
  cts << "<param name=\"ImageNumber\" value=\"";
  if (isDir)  // added - KPW
  {
    cts << (int)BOOK_CLOSED ;
  }
  else
  {
    cts << (int)TEXT;
  }
  cts << "\">";
  cts << "</OBJECT>\n";
}


void HtmlHelp::addIndexItem(Definition *context,MemberDef *md,
                            const char *word)
{
  if (md)
  {
    static bool separateMemberPages = Config_getBool("SEPARATE_MEMBER_PAGES");
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
    QCString anchor  = md->anchor();
    index->addItem(level1,level2,contRef,anchor,TRUE,FALSE);
    index->addItem(level2,level1,memRef,anchor,TRUE,TRUE);
  }
  else if (context)
  {
    QCString level1  = word ? QCString(word) : context->name();
    index->addItem(level1,0,context->getOutputFileBase(),0,TRUE,FALSE);
  }
}

void HtmlHelp::addImageFile(const char *fileName)
{
  imageFiles.append(fileName);
}

