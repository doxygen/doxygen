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

#ifndef MEMBERNAME_H
#define MEMBERNAME_H

#include <qlist.h>
#include "memberdef.h"
#include "sortdict.h"
#include "linkedmap.h"

class MemberName
{
  public:
    using Ptr = std::unique_ptr<MemberDef>;
    using Vec = std::vector<Ptr>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;
    using reverse_iterator = typename Vec::reverse_iterator;
    using const_reverse_iterator = typename Vec::const_reverse_iterator;

    MemberName(const char *name) : m_name(name) {}
    const char *memberName() const { return m_name; }

    iterator begin()                       { return m_members.begin();   }
    iterator end()                         { return m_members.end();     }
    const_iterator cbegin() const          { return m_members.cbegin();  }
    const_iterator cend() const            { return m_members.cend();    }
    reverse_iterator rbegin()              { return m_members.rbegin();  }
    reverse_iterator rend()                { return m_members.rend();    }
    const_reverse_iterator crbegin() const { return m_members.crbegin(); }
    const_reverse_iterator crend() const   { return m_members.crend();   }
    bool empty() const                     { return m_members.empty();   }
    int size() const                       { return m_members.size();    }
    Ptr &back()                            { return m_members.back();    }
    const Ptr &back() const                { return m_members.back();    }
    Ptr &front()                           { return m_members.front();   }
    const Ptr &front() const               { return m_members.front();   }
    void push_back(Ptr &&p)                { m_members.push_back(std::move(p)); }

  private:
    QCString m_name;
    Vec m_members;
};

/** Ordered dictionary of MemberName objects. */
class MemberNameLinkedMap : public LinkedMap<MemberName>
{
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
  private:
    int compareValues(const MemberInfo *item1,const MemberInfo *item2) const;
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
    MemberNameInfoSDict(uint size) : SDict<MemberNameInfo>(size) {}
   ~MemberNameInfoSDict() {}
  private:
    int compareValues(const MemberNameInfo *item1,const MemberNameInfo *item2) const
    {
      return qstricmp(item1->memberName(), item2->memberName());
    }
};

#endif
