/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#ifndef MEMBERNAME_H
#define MEMBERNAME_H

#include <qlist.h>
#include "memberdef.h"
#include "sortdict.h"

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

#if 0
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
#endif

class MemberNameSDict : public SDict<MemberName>
{
  public:
    MemberNameSDict(int size) : SDict<MemberName>(size) {}
   ~MemberNameSDict() {}

   int compareItems(GCI item1,GCI item2);
};

struct MemberInfo
{
  MemberInfo(MemberDef *md,Protection p,Specifier v,bool inh) :
        memberDef(md), prot(p), virt(v), inherited(inh), ambigClass(0) {}
 ~MemberInfo() {}
  MemberDef *memberDef;
  Protection prot;
  Specifier  virt;
  bool       inherited;
  QCString   scopePath;
  QCString   ambiguityResolutionScope; 
  ClassDef  *ambigClass;
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

#if 0
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


class MemberNameInfoSDict : public SDict<MemberNameInfo>
{
  public:
    MemberNameInfoSDict(int size) : SDict<MemberNameInfo>(size) {}
   ~MemberNameInfoSDict() {}
    int compareItems(GCI item1,GCI item2) 
    { return stricmp(
                    ((MemberNameInfo *)item1)->memberName(),
                    ((MemberNameInfo *)item2)->memberName()
                   );
    }
};

#endif
