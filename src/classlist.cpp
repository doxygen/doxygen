/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
#include "groupdef.h"

ClassList::ClassList() : QList<ClassDef>()
{
}

ClassList::~ClassList()
{
}

static int compItems(const ClassDef *c1,const ClassDef *c2)
{
  static bool b = Config_getBool(SORT_BY_SCOPE_NAME);
  if (b)
  {
     return qstricmp(c1->name(), c2->name());
  }
  else
  {
     return qstricmp(c1->className(), c2->className());
  }
}

int ClassList::compareValues(const ClassDef *item1, const ClassDef *item2) const
{
  return compItems(item1,item2);
}

ClassListIterator::ClassListIterator(const ClassList &cllist) :
  QListIterator<ClassDef>(cllist)
{
}

//-------------------------------------------

bool ClassLinkedRefMap::declVisible(const ClassDef::CompoundType *filter) const
{
  bool hideUndocClasses = Config_getBool(HIDE_UNDOC_CLASSES);
  bool extractLocalClasses = Config_getBool(EXTRACT_LOCAL_CLASSES);
  for (const auto &cd : *this)
  {
    if (!cd->isAnonymous() &&
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
        return true;
      }
    }
  }
  return false;
}

void ClassLinkedRefMap::writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter,
                                      const char *header,bool localNames) const
{
  static bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  bool found=FALSE;
  for (const auto &cd : *this)
  {
    //printf("  ClassLinkedRefMap::writeDeclaration for %s\n",cd->name().data());
    if (!cd->isAnonymous() &&
        !cd->isExtension() &&
        (cd->protection()!=Private || extractPrivate) &&
        (filter==0 || *filter==cd->compoundType())
       )
    {
      //printf("writeDeclarationLink()\n");
      cd->writeDeclarationLink(ol,found,header,localNames);
    }
  }
  if (found) ol.endMemberList();
}

void ClassLinkedRefMap::writeDocumentation(OutputList &ol,const Definition * container) const
{
  static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);

  static bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  static bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  if (!inlineGroupedClasses && !inlineSimpleClasses) return;

  bool found=FALSE;

  for (const auto &cd : *this)
  {
    //printf("%s:writeDocumentation() %p linkable=%d embedded=%d container=%p partOfGroups=%zu\n",
    //  cd->name().data(),cd->getOuterScope(),cd->isLinkableInProject(),cd->isEmbeddedInOuterScope(),
    //  container,cd->partOfGroups()->size());

    if (!cd->isAnonymous() &&
        cd->isLinkableInProject() &&
        cd->isEmbeddedInOuterScope() &&
        !cd->isAlias() &&
        (container==0 || cd->partOfGroups().empty()) // if container==0 -> show as part of the group docs, otherwise only show if not part of a group
       )
    {
      //printf("  showing class %s\n",cd->name().data());
      if (!found)
      {
        ol.writeRuler();
        ol.startGroupHeader();
        ol.parseText(fortranOpt?theTranslator->trTypeDocumentation():
            theTranslator->trClassDocumentation());
        ol.endGroupHeader();
        found=TRUE;
      }
      ClassDefMutable *cdm = toClassDefMutable(cd);
      if (cdm)
      {
        cdm->writeInlineDocumentation(ol);
      }
    }
  }
}

//-------------------------------------------

void GenericsSDict::insert(const QCString &key,ClassDef *cd)
{
  int i=key.find('<');
  if (i==-1) return;
  auto argList = stringToArgumentList(SrcLangExt_CSharp, key.mid(i));
  int c = (int)argList->size();
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
      auto argList = stringToArgumentList(SrcLangExt_CSharp,key.mid(i));
      int c = (int)argList->size();
      return collection->find(c);
    }
  }
  return 0;
}




