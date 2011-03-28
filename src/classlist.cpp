/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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

#include "classlist.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "language.h"
#include "doxygen.h"
#include "vhdldocgen.h"

ClassList::ClassList() : QList<ClassDef>()
{
}

ClassList::~ClassList()
{
}

static int compItems(void *item1,void *item2)
{
  ClassDef *c1=(ClassDef *)item1;
  ClassDef *c2=(ClassDef *)item2;
  static bool b = Config_getBool("SORT_BY_SCOPE_NAME");
  //printf("compItems: %d %s<->%s\n",b,c1->qualifiedName().data(),c2->qualifiedName().data());
  if (b) 
  { 
     return stricmp(c1->name(),
                    c2->name());
  }
  else
  {
     return stricmp(c1->className(),
                    c2->className());
  }
}

int ClassList::compareItems(GCI item1, GCI item2)
{
  return compItems(item1,item2);
}

int ClassSDict::compareItems(GCI item1, GCI item2)
{
  return compItems(item1,item2);
}

ClassListIterator::ClassListIterator(const ClassList &cllist) :
  QListIterator<ClassDef>(cllist)
{
}

bool ClassSDict::declVisible(const ClassDef::CompoundType *filter) const
{
  static bool hideUndocClasses = Config_getBool("HIDE_UNDOC_CLASSES");
  static bool extractLocalClasses = Config_getBool("EXTRACT_LOCAL_CLASSES");
  if (count()>0)
  {
    ClassSDict::Iterator sdi(*this);
    ClassDef *cd=0;
    for (sdi.toFirst();(cd=sdi.current());++sdi)
    {
      if (cd->name().find('@')==-1 && 
          (filter==0 || *filter==cd->compoundType())
         )
      {
        bool isLink = cd->isLinkable();
        if (isLink || 
             (!hideUndocClasses && 
              (!cd->isLocal() || extractLocalClasses)
             )
           )
        {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

void ClassSDict::writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter,
                                  const char *header,bool localNames)
{
//  static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
//  bool first=TRUE;
  if (count()>0)
  {
    ClassSDict::Iterator sdi(*this);
    ClassDef *cd=0;
    bool found=FALSE;
    for (sdi.toFirst();(cd=sdi.current());++sdi)
    {
      //printf("  ClassSDict::writeDeclaration for %s\n",cd->name().data());
      if (cd->name().find('@')==-1 && 
          (filter==0 || *filter==cd->compoundType())
         )
      {
//        //bool isLink = cd->isLinkable();
//        if (inlineGroupedClasses && cd->partOfGroups()->count()>0)
//        {
//          cd->writeInlineDeclaration(ol,first);
//          first=FALSE;
//        }
//        else // show link's only
//        {
          cd->writeDeclarationLink(ol,found,header,localNames);
//        }
      }
    }
    if (found) ol.endMemberList();
  }
}
  
void ClassSDict::writeDocumentation(OutputList &ol)
{
  static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");

  static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  if (!inlineGroupedClasses) return;

  if (count()>0)
  {
    ol.writeRuler();
    ol.startGroupHeader();
    ol.parseText(fortranOpt?theTranslator->trTypeDocumentation():
        theTranslator->trClassDocumentation());
    ol.endGroupHeader();

    ClassSDict::Iterator sdi(*this);
    ClassDef *cd=0;
    for (sdi.toFirst();(cd=sdi.current());++sdi)
    {
      if (cd->name().find('@')==-1 && 
          cd->partOfGroups()->count()==1
         )
      {
        cd->writeInlineDocumentation(ol);
      }
    }
  }
}

