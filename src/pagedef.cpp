#include "pagedef.h"


PageDef::PageDef(const char *f,int l,const char *n,
                 const char *d,const char *t)
 : Definition(f,l,n), m_title(t)
{
  setDocumentation(d,f,l);

}

PageDef::~PageDef()
{
}
