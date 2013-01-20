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

#ifndef MEMBERNAME_H
#define MEMBERNAME_H

#include <qlist.h>
#include "memberdef.h"
#include "sortdict.h"

/** Class representing all MemberDef objects with the same name */
class MemberName : public QList<MemberDef>
{ 
  public:
    MemberName(const char *name);
   ~MemberName();
    const char *memberName() const { return name; }
   
    int compareItems(QCollection::Item item1,QCollection::Item item2);
  private:
    QCString name;
};

/** Iterator for MemberDef objects in a MemberName list. */
class MemberNameIterator : public QListIterator<MemberDef>
{
  public:
    MemberNameIterator( const MemberName &list);
};

/** Sorted dictionary of MemberName objects. */
class MemberNameSDict : public SDict<MemberName>
{
  public:
    MemberNameSDict(int size) : SDict<MemberName>(size) {}
   ~MemberNameSDict() {}

   int compareItems(QCollection::Item item1,QCollection::Item item2);
};

/** Data associated with a MemberDef in an inheritance relation. */
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

/** Class representing all MemberInfo objects with the same name */
class MemberNameInfo : public QList<MemberInfo>
{
  public:
    MemberNameInfo(const char *name);  
   ~MemberNameInfo() {}
    const char *memberName() const { return name; }
    int compareItems(QCollection::Item item1,QCollection::Item item2);
  private:
    QCString name;
};

/** Iterator for MemberInfo objects in a MemberNameInfo list. */
class MemberNameInfoIterator : public QListIterator<MemberInfo>
{
  public:
    MemberNameInfoIterator(const MemberNameInfo &mnii) 
      : QListIterator<MemberInfo>(mnii) {}
};

/** Sorted dictionary of MemberNameInfo objects. */
class MemberNameInfoSDict : public SDict<MemberNameInfo>
{
  public:
    MemberNameInfoSDict(int size) : SDict<MemberNameInfo>(size) {}
   ~MemberNameInfoSDict() {}
    int compareItems(QCollection::Item item1,QCollection::Item item2) 
    { return qstricmp(
                    ((MemberNameInfo *)item1)->memberName(),
                    ((MemberNameInfo *)item2)->memberName()
                   );
    }
};

#endif
