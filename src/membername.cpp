/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include "membername.h"
#include "classdef.h"
#include "util.h"
#include "filedef.h"

MemberName::MemberName(const char *n) : QList<MemberDef>()
{
  name=n;
}

MemberName::~MemberName()
{
}

int MemberName::compareItems(GCI item1, GCI item2)
{
  MemberDef *m1=(MemberDef *)item1;
  MemberDef *m2=(MemberDef *)item2;
  ClassDef *c1=m1->memberClass();
  ClassDef *c2=m2->memberClass();
  FileDef  *f1=m1->getFileDef();
  FileDef  *f2=m2->getFileDef();
  if (c1 && c2)
    return strcmp(c1->name(),c2->name());
  else if (f1 && f2)
    return strcmp(f1->name(),f2->name());
  else 
    return 0;
}

MemberNameInfo::MemberNameInfo(const char *n) : QList<MemberInfo>()
{
  name=n;
  setAutoDelete(TRUE);
}

int MemberNameInfo::compareItems(GCI item1, GCI item2)
{
  MemberInfo *m1=(MemberInfo *)item1;
  MemberInfo *m2=(MemberInfo *)item2;
  ClassDef *c1=m1->memberDef->memberClass();
  ClassDef *c2=m2->memberDef->memberClass();
  FileDef  *f1=m1->memberDef->getFileDef();
  FileDef  *f2=m2->memberDef->getFileDef();
  if (c1 && c2)
    return strcmp(c1->name(),c2->name());
  else if (f1 && f2)
    return strcmp(f1->name(),f2->name());
  else 
    return 0;
}
MemberNameIterator::MemberNameIterator(const MemberName &mnlist) :
  QListIterator<MemberDef>(mnlist)
{
}

MemberNameList::MemberNameList() : QList<MemberName>()
{
}

MemberNameList::~MemberNameList()
{
}

int MemberNameList::compareItems(GCI item1, GCI item2)
{
  MemberName *n1=(MemberName *)item1;
  MemberName *n2=(MemberName *)item2;
  return stricmp(n1->memberName(),n2->memberName());
}

MemberNameListIterator::MemberNameListIterator(const MemberNameList &mnlist) :
  QListIterator<MemberName>(mnlist)
{
}

