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

#ifndef MEMBERNAME_H
#define MEMBERNAME_H

#include <qlist.h>
#include "memberdef.h"

class MemberName : public QList<MemberDef>
{ 
  public:
    MemberName(const char *name);
   ~MemberName();
    const char *memberName() const { return name; }
   
    int compareItems(GCI item1,GCI item2);
  private:
    QCString name;
};

class MemberNameIterator : public QListIterator<MemberDef>
{
  public:
    MemberNameIterator( const MemberName &list);
};

class MemberNameList : public QList<MemberName>
{ 
  public:
    MemberNameList();
   ~MemberNameList();
   
   int compareItems(GCI item1,GCI item2);
};

class MemberNameListIterator : public QListIterator<MemberName>
{
  public:
    MemberNameListIterator( const MemberNameList &list );
};

typedef QDict<MemberName>  MemberNameDict;

struct MemberInfo
{
  MemberInfo(MemberDef *md,Protection p,Specifier v) :
        memberDef(md), prot(p), virt(v),ambigClass(0) {}
 ~MemberInfo() {}
  MemberDef *memberDef;
  Protection prot;
  Specifier virt;
  QCString scopePath;
  QCString ambiguityResolutionScope; 
  ClassDef *ambigClass;
};

class MemberNameInfo : public QList<MemberInfo>
{
  public:
    MemberNameInfo(const char *name);  
   ~MemberNameInfo() {}
    const char *memberName() const { return name; }
    int compareItems(GCI item1,GCI item2);
  private:
    QCString name;
};

class MemberNameInfoIterator : public QListIterator<MemberInfo>
{
  public:
    MemberNameInfoIterator(const MemberNameInfo &mnii) 
      : QListIterator<MemberInfo>(mnii) {}
};

class MemberNameInfoList : public QList<MemberNameInfo>
{
  public:
   ~MemberNameInfoList() {}
    int compareItems(GCI item1,GCI item2) 
    { return stricmp(
                     ((MemberNameInfo *)item1)->memberName(),
                     ((MemberNameInfo *)item2)->memberName()
                    );
    }
};

class MemberNameInfoDict : public QDict<MemberNameInfo>
{
  public:
    MemberNameInfoDict(int size) : QDict<MemberNameInfo>(size) {}
   ~MemberNameInfoDict() {}
};

class MemberNameInfoListIterator : public QListIterator<MemberNameInfo>
{
  public:
    MemberNameInfoListIterator(const MemberNameInfoList &mil) : 
      QListIterator<MemberNameInfo>(mil) {}
};

#endif
