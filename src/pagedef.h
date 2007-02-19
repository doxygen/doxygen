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
 */

#include "definition.h"
#include "sortdict.h"

class PageSDict;
class OutputList;

class PageDef : public Definition
{
  public:
    PageDef(const char *f,int l,const char *n,const char *d,const char *t);
   ~PageDef();
    DefType definitionType() const { return TypePage; }
    bool isLinkableInProject() const 
    { 
      return hasDocumentation() && !isReference();
    }
    bool isLinkable() const 
    {
      return isLinkableInProject() || isReference();
    } 
    void addSubPage(PageDef *def);
    void writeDocumentation(OutputList &ol);

    // functions to get a uniform interface with Definitions
    QCString getOutputFileBase() const;
    void findSectionsInDocumentation();
    QCString title() const { return m_title; }
    GroupDef *  getGroupDef() const;
    PageSDict * getSubPages() const { return subPageDict; }
    void setFileName(const char *name) { m_fileName = name; }
    void addInnerCompound(Definition *d);
    bool visibleInIndex() const;
    bool documentedPage() const;
    bool hasSubPages() const;
    void setPageScope(Definition *d){ pageScope = d; }
    Definition *getPageScope() const { return pageScope; }

  private:
    QCString m_fileName;
    QCString m_title;
    GroupDef *m_inGroup;
    PageSDict *subPageDict;                 // list of pages in the group
    Definition *pageScope;
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

