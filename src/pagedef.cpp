#include "pagedef.h"
#include "groupdef.h"
#include "docparser.h"


PageDef::PageDef(const char *f,int l,const char *n,
                 const char *d,const char *t)
 : Definition(f,l,n), m_title(t)
{
  setDocumentation(d,f,l);

}

PageDef::~PageDef()
{
}

void PageDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
}

GroupDef *PageDef::getGroupDef() const 
{ 
  return partOfGroups() ? partOfGroups()->getFirst() : 0; 
}

QCString PageDef::getOutputFileBase() const 
{ 
  if (getGroupDef()) 
    return getGroupDef()->getOutputFileBase();
  else 
    return m_fileName; 
}

