/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
 */

#include "definition.h"
#include "sortdict.h"
//#include "config.h"
//#include "docparser.h"

//#include "section.h"
//#include "doxygen.h"

class PageDef : public Definition
{
  public:
    PageDef(const char *f,int l,const char *n,const char *d,const char *t);
   ~PageDef();
    DefType definitionType() { return TypePage; }
    bool isLinkableInProject() const 
    { 
      return hasDocumentation() && !isReference();
    }
    bool isLinkable() const 
    {
      return isLinkableInProject() || isReference();
    } 

    // functions to get a uniform interface with Definitions
    QCString getOutputFileBase() const { return m_fileName; }
    void findSectionsInDocumentation();
    QCString title() const { return m_title; }
    GroupDef *getGroupDef() const;
    void setFileName(const char *name) { m_fileName = name; }
#if 0
   
    bool isReference() const { return !reference.isEmpty(); }
    QCString getReference() const { return reference; }
    PageInfo(const char *f, int l,const char *n,const char *d,const char *t) :
      defFileName(f), defLine(l), name(n), 
      doc(d), title(t), context(0), xrefListItems(0),m_inGroup(0)
    {
      sectionDict = new SectionDict(17);
    }
    ~PageInfo()
    {
      delete sectionDict;
    }

    // where the page definition was found
    QCString defFileName;
    int defLine;

    // contents of the page
    QCString name;
    QCString doc;
    QCString title;

    // external reference? if so then this is the tag file name
    QCString reference;
    QCString fileName;

    // class, file or namespace in which the page was found
    Definition *context;

    
    //void addSections(QList<QCString> *anchorList)
    //{
    //  if (anchorList)
    //  {
    //    QCString *s=anchorList->first();
    //    while (s)
    //    {
    //      SectionInfo *si=0;
    //      if (!s->isEmpty() && (si=Doxygen::sectionDict[*s]))
    //      {
    //        //printf("Add section `%s' to definition `%s'\n",
    //        //    si->label.data(),n.data());
    //        if (sectionDict==0) 
    //        {
    //          sectionDict = new SectionDict(17);
    //        }
    //        if (sectionDict->find(*s)==0)
    //        {
    //          sectionDict->insert(*s,si);
    //        }
    //        si->pageRef = this;
    //        si->fileName = fileName;
    //      }
    //      s=anchorList->next();
    //    }
    //  }
    //}
    void findSectionsInDocumentation()
    {
      docFindSections(doc,this,0,0,defFileName);
    }
    
    void writeDocAnchorsToTagFile()
    {
      if (!Config_getString("GENERATE_TAGFILE").isEmpty() && sectionDict)
      {
        QDictIterator<SectionInfo> sdi(*sectionDict);
        SectionInfo *si;
        for (;(si=sdi.current());++sdi)
        {
          if (!si->generated)
          {
            Doxygen::tagFile << "    <docanchor>" << si->label << "</docanchor>" << endl;
          }
        }
      }
    }


    void makePartOfGroup(GroupDef *gd) { m_inGroup = gd; }
    GroupDef *getGroupDef() const { return m_inGroup; }

    // is this page part of a group
    SectionDict *sectionDict;
    QList<ListItemInfo> *xrefListItems;

  private:
    GroupDef *m_inGroup;
#endif
  private:
    QCString m_fileName;
    QCString m_title;
    GroupDef *m_inGroup;
};

class PageSDict : public SDict<PageDef>
{
  public:
    PageSDict(int size) : SDict<PageDef>(size) {}
    virtual ~PageSDict() {}
    int compareItems(GCI i1,GCI i2)
    {
      return stricmp(((PageDef *)i1)->name(),((PageDef *)i2)->name());
    }
};

