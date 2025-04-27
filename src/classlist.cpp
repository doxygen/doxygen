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

bool ClassLinkedRefMap::declVisible(const ClassDef::CompoundType *filter) const
{
  bool hideUndocClasses = Config_getBool(HIDE_UNDOC_CLASSES);
  bool extractLocalClasses = Config_getBool(EXTRACT_LOCAL_CLASSES);
  for (const auto &cd : *this)
  {
    if (!cd->isAnonymous() &&
        (filter==nullptr || *filter==cd->compoundType())
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
                                      const QCString &header,bool localNames) const
{
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  bool found=FALSE;
  for (const auto &cd : *this)
  {
    //printf("  ClassLinkedRefMap::writeDeclaration for %s\n",cd->name().data());
    if (!cd->isAnonymous() &&
        !cd->isExtension() &&
        (cd->protection()!=Protection::Private || extractPrivate) &&
        (filter==nullptr || *filter==cd->compoundType())
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
  bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);

  bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
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
        (container==nullptr || cd->partOfGroups().empty()) // if container==nullptr -> show as part of the group docs, otherwise only show if not part of a group
       )
    {
      //printf("  showing class %s\n",cd->name().data());
      if (!found)
      {
        ol.writeRuler();
        ol.startGroupHeader("inline_classes");
        ol.parseText(fortranOpt?theTranslator->trTypeDocumentation():
            theTranslator->trClassDocumentation());
        ol.endGroupHeader();
        found=TRUE;
      }
      cd->writeInlineDocumentation(ol);
    }
  }
}


