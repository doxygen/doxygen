/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
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

//----------------------------------------------------------------------------

struct IndexField
{
  QCString name;
  QCString url;
  QCString anchor;
  bool     link;
};

class IndexFieldList : public QList<IndexField>
{
  public:
    int compareItems(GCI item1, GCI item2)
    {
      return stricmp(((IndexField *)item1)->name,((IndexField *)item2)->name);
    }
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
                 const char *url, const char *anchor,bool hasLink);
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
 */
void HtmlHelpIndex::addItem(const char *level1,const char *level2,
                       const char *url,const char *anchor,bool hasLink)
{
  QCString key = level1; 
  if (level2) key+= (QCString)"?" + level2;
  if (dict->find(key)==0) // new key
  {
    //printf(">>>>>>>>> HtmlHelpIndex::addItem(%s,%s,%s,%s)\n",
    //      level1,level2,url,anchor);
    IndexField *f = new IndexField;
    f->name   = key;
    f->url    = url;
    f->anchor = anchor;
    f->link   = hasLink;
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
    
      if (level2.isEmpty())
      {
        t << "  <LI><OBJECT type=\"text/sitemap\">";
        t << "<param name=\"Local\" value=\"" << f->url << ".html";
        if (!f->anchor.isEmpty()) t << "#" << f->anchor;  
        t << "\">";
        t << "<param name=\"Name\" value=\"" << level1 << "\">"
           "</OBJECT>\n";
      }
      else
      {
        if (f->link)
        {
          t << "  <LI><OBJECT type=\"text/sitemap\">";
          t << "<param name=\"Local\" value=\"" << f->url << ".html\">";
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
      t << "<param name=\"Local\" value=\"" << f->url << ".html";
      if (!f->anchor.isEmpty()) t << "#" << f->anchor;  
      t << "\">";
      t << "<param name=\"Name\" value=\"" << level2 << "\">"
         "</OBJECT>\n";
    }
    lastLevel1 = level1.copy();
  } 
}

//----------------------------------------------------------------------------

HtmlHelp *HtmlHelp::theInstance = 0;

/*! Constructs an html object. 
 *  The object has to be \link initialize() initialized\endlink before it can 
 *  be used.
 */
HtmlHelp::HtmlHelp()
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

/*! This will create a contents file (index.hhc) and a index file (index.hhk)
 *  and write the header of those files. 
 *  It also creates a project file (index.hhp)
 *  \sa finalize()
 */
void HtmlHelp::initialize()
{
  /* open the contents file */
  QCString fName = Config::htmlOutputDir + "/index.hhc";
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
         "<param name=\"ImageType\" value=\"Folder\">\n"
         "</OBJECT>\n"
         "<UL>\n";
  
  /* open the contents file */
  fName = Config::htmlOutputDir + "/index.hhk";
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
         "<param name=\"ImageType\" value=\"Folder\">\n"
         "</OBJECT>\n"
         "<UL>\n";

  /* Write the project file */
  fName = Config::htmlOutputDir + "/index.hhp";
  QFile f(fName);
  if (f.open(IO_WriteOnly))
  {
    QTextStream t(&f);
    t << "[OPTIONS]\n"
         "Compatibility=1.1\n"
         "Full-text search=Yes\n"
         "Contents file=index.hhc\n"
         "Default Window=indexHelp\n"
         "Default topic=index.html\n"
         "Index file=index.hhk\n"
         "Title=" << Config::projectName << endl << endl 
      << "[FILES]\n"
         "index.html";
    f.close();
  }
  else
  {
    err("Could not open file %s for writing\n",fName.data());
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
  cts.unsetDevice();
  cf->close();
  delete cf;
  
  index->writeFields(kts);
  
  // end the index file
  kts << "</UL>\n";
  kts.unsetDevice();
  kf->close();
  delete kf;
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
 *  \param name the name of the item.
 *  \param ref  the URL of to the item.
 */
void HtmlHelp::addContentsItem(const char *name,const char *ref, 
                               const char *anchor)
{
  int i; for (i=0;i<dc;i++) cts << "  ";
  cts << "<LI><OBJECT type=\"text/sitemap\">";
  cts << "<param name=\"Local\" value=\"" << ref << ".html";
  if (anchor) cts << "#" << anchor;  
  cts << "\">";
  cts << "<param name=\"Name\" value=\"" << name << "\">"
         "</OBJECT>\n";
}

/*! Add an list item to the index file.
 *  \param name the name of the item.
 *  \param ref  the URL of to the item.
 *  \sa HtmlHelpIndex
 */
void HtmlHelp::addIndexItem(const char *level1, const char *level2,
                            const char *ref, const char *anchor)
{
  index->addItem(level1,level2,ref,anchor,TRUE);
  index->addItem(level2,level1,ref,anchor,FALSE);
}
