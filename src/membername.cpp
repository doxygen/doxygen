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

#include "membername.h"
#include "classdef.h"
#include "util.h"
#include "filedef.h"

MemberNameInfo::MemberNameInfo(const char *n) : QList<MemberInfo>()
{
  name=n;
  setAutoDelete(TRUE);
}

int MemberNameInfo::compareValues(const MemberInfo *m1,const MemberInfo *m2) const
{
  const ClassDef *c1=m1->memberDef->getClassDef();
  const ClassDef *c2=m2->memberDef->getClassDef();
  const FileDef  *f1=m1->memberDef->getFileDef();
  const FileDef  *f2=m2->memberDef->getFileDef();
  if (c1 && c2)
    return qstrcmp(c1->name(),c2->name());
  else if (f1 && f2)
    return qstrcmp(f1->name(),f2->name());
  else
    return 0;
}

