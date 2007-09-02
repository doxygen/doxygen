/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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
 * The code is this file is largely based on a contribution from
 * Harm van der Heijden <H.v.d.Heijden@phys.tue.nl>
 * Please send thanks to him and bug reports to me :-)
 */

#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qdict.h>

#include "htmlhelp.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"

//----------------------------------------------------------------------------

struct IndexField
{
  QCString name;
  QCString url;
  QCString anchor;
  bool     link;
  bool     reversed;
};

class IndexFieldList : public QList<IndexField>
{
  public:
    int compareItems(GCI item1, GCI item2)
    {
      return stricmp(((IndexField *)item1)->name,((IndexField *)item2)->name);
    }
   ~IndexFieldList() {}
};

class IndexFieldListIterator : public QListIterator<IndexField>
{
  public:
    IndexFieldListIterator( const IndexFieldList &list) :
       QListIterator<IndexField>(list) {}
};

class IndexFieldDict : public QDict<IndexField>
{
  public:
    IndexFieldDict(int size) : QDict<IndexField>(size) {}
   ~IndexFieldDict() {}
};

/*! A helper class for HtmlHelp that manages a two level index in 
 * alphabetical order 
 */
class HtmlHelpIndex
{
  public:
    HtmlHelpIndex();
   ~HtmlHelpIndex();
    void addItem(const char *first,const char *second, 
                 const char *url, const char *anchor,
                 bool hasLink,bool reversed);
    void writeFields(QTextStream &t);
  private:
    IndexFieldList *list;
    IndexFieldDict *dict;   
};

/*! Constructs a new HtmlHelp index */
HtmlHelpIndex::HtmlHelpIndex()
{
  list = new IndexFieldList;
  dict = new IndexFieldDict(10007);
  list->setAutoDelete(TRUE);
}

/*! Destroys the HtmlHelp index */
HtmlHelpIndex::~HtmlHelpIndex()
{
  delete list;
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
    list->inSort(f);
    dict->insert(key,f);
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
void HtmlHelpIndex::writeFields(QTextStream &t)
{
  IndexFieldListIterator ifli(*list);
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
        t << "<param name=\"Local\" value=\"" << f->url << Doxygen::htmlFileExtension;
        if (!f->anchor.isEmpty() && f->reversed) t << "#" << f->anchor;  
        t << "\">";
        t << "<param name=\"Name\" value=\"" << level1 << "\">"
           "</OBJECT>\n";
      }
      else
      {
        if (f->link)
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"Local\" value=\"" << f->url << Doxygen::htmlFileExtension;
          if (!f->anchor.isEmpty() && f->reversed) t << "#" << f->anchor;  
          t << "\">";
          t << "<param name=\"Name\" value=\"" << level1 << "\">"
               "</OBJECT>\n";
        }
        else
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"See Also\" value=\"" << level1 << "\">";
          t << "<param name=\"Name\" value=\"" << level1 << "\">"
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
      t << "<param name=\"Local\" value=\"" << f->url << Doxygen::htmlFileExtension;
      if (!f->anchor.isEmpty()) t << "#" << f->anchor;  
      t << "\">";
      t << "<param name=\"Name\" value=\"" << level2 << "\">"
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
  index = new HtmlHelpIndex;
}

/*! return a reference to the one and only instance of this class. 
 */
HtmlHelp *HtmlHelp::getInstance()
{
  if (theInstance==0) theInstance = new HtmlHelp;
  return theInstance;
}

static QDict<QCString> s_languageDict;

/*! This will create a contents file (index.hhc) and a index file (index.hhk)
 *  and write the header of those files. 
 *  It also creates a project file (index.hhp)
 *  \sa finalize()
 */
void HtmlHelp::initialize()
{
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
#if QT_VERSION >= 200
  cts.setEncoding(QTextStream::Latin1);
#endif
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
#if QT_VERSION >= 200
  kts.setEncoding(QTextStream::Latin1);
#endif
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
  */
  s_languageDict.setAutoDelete(TRUE);
  s_languageDict.clear();
  s_languageDict.insert("czech",       new QCString("0x405 Czech"));
  s_languageDict.insert("danish",      new QCString("0x406 Danish"));
  s_languageDict.insert("dutch",       new QCString("0x413 Dutch"));
  s_languageDict.insert("finnish",     new QCString("0x40B Finnish"));
  s_languageDict.insert("french",      new QCString("0x40C French"));
  s_languageDict.insert("german",      new QCString("0x407 German"));
  s_languageDict.insert("greece",      new QCString("0x408 Greece"));
  s_languageDict.insert("hungarian",   new QCString("0x40E Hungarian"));
  s_languageDict.insert("italian",     new QCString("0x410 Italian"));
  s_languageDict.insert("norwegian",   new QCString("0x814 Norwegian"));
  s_languageDict.insert("polish",      new QCString("0x415 Polish"));
  s_languageDict.insert("portugese",   new QCString("0x816 Portuguese(Portugal)"));
  s_languageDict.insert("brazil",      new QCString("0x416 Portuguese(Brazil)"));
  s_languageDict.insert("russian",     new QCString("0x419 Russian"));
  s_languageDict.insert("spanish",     new QCString("0x40A Spannish(Traditional Sort)"));
  s_languageDict.insert("swedish",     new QCString("0x41D Swedish"));
  s_languageDict.insert("turkey",      new QCString("0x41F Turkey"));
  s_languageDict.insert("japanese",    new QCString("0x411 Japanese"));
  s_languageDict.insert("japanese-en", new QCString("0x411 Japanese"));
  s_languageDict.insert("korean",      new QCString("0x412 Korean"));
  s_languageDict.insert("chinese",     new QCString("0x804 Chinese (PRC)"));
  s_languageDict.insert("chinese-traditional", new QCString("0x404 Chinese (Taiwan)"));
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
    QTextStream t(&f);
#if QT_VERSION >= 200
    t.setEncoding(QTextStream::Latin1);
#endif

   
    
    QCString indexName="index"+Doxygen::htmlFileExtension;
    if (Config_getBool("GENERATE_TREEVIEW")) indexName="main"+Doxygen::htmlFileExtension;
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
    t << "Title=" << Config_getString("PROJECT_NAME") << endl << endl;
    
    t << "[WINDOWS]" << endl;
    t << "main=\"" << Config_getString("PROJECT_NAME") << "\",\"index.hhc\","
         "\"index.hhk\",\"" << indexName << "\",\"" << 
         indexName << "\",,,,,0x23520,,0x387e,,,,,,,,0" << endl << endl;
    
    t << "[FILES]" << endl;
    char *s = indexFiles.first();
    while (s)
    {
      t << s << endl;
      s = indexFiles.next();
    }
    t << "tabs.css" << endl;
    t << "tab_b.gif" << endl;
    t << "tab_l.gif" << endl;
    t << "tab_r.gif" << endl;
    if (Config_getBool("HTML_DYNAMIC_SECTIONS"))
    {
      t << "open.gif" << endl;
      t << "closed.gif" << endl;
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
int HtmlHelp::incContentsDepth()
{
  int i; for (i=0;i<dc+1;i++) cts << "  ";
  cts << "<UL>\n";
  return ++dc;
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the unnumber HTML list.
 *  \sa incContentsDepth()
 */
int HtmlHelp::decContentsDepth()
{
  int i; for (i=0;i<dc;i++) cts << "  ";
  cts << "</UL>\n";
  return --dc;
}

/*! Add an list item to the contents file.
 *  \param isDir boolean indicating if this is a dir or file entry
 *  \param name the name of the item.
 *  \param ref  the URL of to the item.
 *  \param anchor the anchor of the item.
 */
void HtmlHelp::addContentsItem(bool isDir,
                               const char *name,const char *ref, 
                               const char *anchor)
{
  // If we're using a binary toc then folders cannot have links. 
  if(Config_getBool("BINARY_TOC") && isDir) 
  {
    ref = 0;
    anchor = 0;
  }
  
  int i; for (i=0;i<dc;i++) cts << "  ";
  cts << "<LI><OBJECT type=\"text/sitemap\">";
  cts << "<param name=\"Name\" value=\"" << name << "\">";
  if (ref)      // made ref optional param - KPW
  {
    cts << "<param name=\"Local\" value=\"" << ref << Doxygen::htmlFileExtension;
    if (anchor) cts << "#" << anchor;  
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

/*! Add an list item to the index file.
 *  \param level1 the main index of the item.
 *  \param level2 the sub index of the item.
 *  \param contRef the output file refering to the container.
 *  \param memRef  the output file containing to the member documentation.
 *  \param anchor  the anchor of the item.
 *  \sa HtmlHelpIndex
 */
void HtmlHelp::addIndexItem(const char *level1, const char *level2,
                            const char *contRef, const char *memRef, 
                            const char *anchor)
{
  index->addItem(level1,level2,contRef,anchor,TRUE,FALSE);
  index->addItem(level2,level1,memRef,anchor,TRUE,TRUE);
}

