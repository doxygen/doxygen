/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include "defargs.h"
#include "arguments.h"

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
  //printf("compItems: %d %s<->%s\n",b,c1->name().data(),c2->name().data());
  if (b) 
  { 
     return qstricmp(c1->name(),
                    c2->name());
  }
  else
  {
     return qstricmp(c1->className(),
                    c2->className());
  }
}

int ClassList::compareItems(QCollection::Item item1, QCollection::Item item2)
{
  return compItems(item1,item2);
}

int ClassSDict::compareItems(QCollection::Item item1, QCollection::Item item2)
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
  static bool extractPrivate = Config_getBool("EXTRACT_PRIVATE");
  if (count()>0)
  {
    ClassSDict::Iterator sdi(*this);
    ClassDef *cd=0;
    bool found=FALSE;
    for (sdi.toFirst();(cd=sdi.current());++sdi)
    {
      //printf("  ClassSDict::writeDeclaration for %s\n",cd->name().data());
      if (cd->name().find('@')==-1 && 
          !cd->isExtension() && 
          (cd->protection()!=Private || extractPrivate) &&
          (filter==0 || *filter==cd->compoundType())
         )
      {
        cd->writeDeclarationLink(ol,found,header,localNames);
      }
    }
    if (found) ol.endMemberList();
  }
}
  
void ClassSDict::writeDocumentation(OutputList &ol,Definition * container)
{
  static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");

  static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  static bool inlineSimpleClasses = Config_getBool("INLINE_SIMPLE_STRUCTS");
  if (!inlineGroupedClasses && !inlineSimpleClasses) return;

  if (count()>0)
  {
    bool found=FALSE;

    ClassSDict::Iterator sdi(*this);
    ClassDef *cd=0;
    for (sdi.toFirst();(cd=sdi.current());++sdi)
    {
      //printf("%s:writeDocumentation() %p embedded=%d container=%p\n",
      //  cd->name().data(),cd->getOuterScope(),cd->isEmbeddedInOuterScope(),
      //  container);

      if (cd->name().find('@')==-1 && 
          cd->isLinkableInProject() &&
          cd->isEmbeddedInOuterScope() &&
          (container==0 || cd->partOfGroups()==0) // if container==0 -> show as part of the group docs, otherwise only show if not part of a group
          //&&
          //(container==0 || // no container -> used for groups
          // cd->getOuterScope()==container || // correct container -> used for namespaces and classes
          // (container->definitionType()==Definition::TypeFile && cd->getOuterScope()==Doxygen::globalScope && cd->partOfGroups()==0) // non grouped class with file scope -> used for files
          //)
         )
      {
        if (!found)
        {
          ol.writeRuler();
          ol.startGroupHeader();
          ol.parseText(fortranOpt?theTranslator->trTypeDocumentation():
              theTranslator->trClassDocumentation());
          ol.endGroupHeader();
          found=TRUE;
        }
        cd->writeInlineDocumentation(ol);
      }
    }
  }
}

//-------------------------------------------

void GenericsSDict::insert(const QCString &key,ClassDef *cd)
{
  int i=key.find('<');
  if (i==-1) return;
  ArgumentList argList;
  stringToArgumentList(key.mid(i),&argList);
  int c = argList.count();
  if (c==0) return;
  GenericsCollection *collection = m_dict.find(key.left(i));
  if (collection==0) // new name
  {
    collection = new GenericsCollection;
    m_dict.append(key.left(i),collection);
  }
  if (collection->find(c)==0) // should always be 0!
  {
    collection->insert(c,cd);
  }
}

ClassDef *GenericsSDict::find(const QCString &key)
{
  int i=key.find('<');
  if (i==-1)
  {
    GenericsCollection *collection = m_dict.find(key);
    if (collection && collection->count()==1)
    {
      QIntDictIterator<ClassDef> it(*collection);
      return it.current();
    }
  }
  else
  {
    GenericsCollection *collection = m_dict.find(key.left(i));
    if (collection)
    {
      ArgumentList argList;
      stringToArgumentList(key.mid(i),&argList);
      int c = argList.count();
      return collection->find(c);
    }
  }
  return 0;
}




