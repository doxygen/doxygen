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

#ifndef MEMBERLIST_H
#define MEMBERLIST_H

#include <qlist.h>
#include "memberdef.h"
#include "sortdict.h"

class GroupDef;
class MemberGroup;
class MemberGroupList;

class MemberList : public QList<MemberDef> 
{ 
  public:
    MemberList();
   ~MemberList();
    bool insert(uint index,const MemberDef *md);
    void inSort(const MemberDef *md);
    void append(const MemberDef *md);
    int compareItems(GCI item1,GCI item2);
    int varCount() const       { ASSERT(m_numDecMembers!=-1); return m_varCnt;     }
    int funcCount() const      { ASSERT(m_numDecMembers!=-1); return m_funcCnt;    }
    int enumCount() const      { ASSERT(m_numDecMembers!=-1); return m_enumCnt;    }
    int enumValueCount() const { ASSERT(m_numDecMembers!=-1); return m_enumValCnt; }
    int typedefCount() const   { ASSERT(m_numDecMembers!=-1); return m_typeCnt;    }
    int protoCount() const     { ASSERT(m_numDecMembers!=-1); return m_protoCnt;   }
    int defineCount() const    { ASSERT(m_numDecMembers!=-1); return m_defCnt;     }
    int friendCount() const    { ASSERT(m_numDecMembers!=-1); return m_friendCnt;  }
    int numDecMembers() const  { ASSERT(m_numDecMembers!=-1); return m_numDecMembers; }
    int numDocMembers() const  { ASSERT(m_numDocMembers!=-1); return m_numDocMembers; }
    void countDecMembers(/*bool inGroup,bool countSubGroups,bool sectionPerType*/);
    void countDocMembers();
    void writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);
    void writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               const char *title,const char *subtitle);
    void writeDocumentation(OutputList &ol,const char *scopeName,
               Definition *container,const char *title);
    void addMemberGroup(MemberGroup *mg);
    void setInGroup(bool group) { m_inGroup=group; }
    void addListReferences(Definition *def);

  private:
    int m_varCnt,m_funcCnt,m_enumCnt,m_enumValCnt,m_typeCnt;
    int m_protoCnt,m_defCnt,m_friendCnt; 
    int m_numDecMembers; // number of members in the brief part of the memberlist
    int m_numDocMembers; // number of members in the detailed part of the memberlist
    MemberGroupList *memberGroupList;
    bool m_inGroup; // is this list part of a group
};

class MemberListIterator : public QListIterator<MemberDef>
{
  public:
    MemberListIterator(const QList<MemberDef> &list);
    virtual ~MemberListIterator() {}
};

class MemberDict : public QDict<MemberDef>
{
  public:
    MemberDict(int size) : QDict<MemberDef>(size) {}
    virtual ~MemberDict() {}
};

class MemberSDict : public SDict<MemberDef>
{
  public:
    MemberSDict(int size=17) : SDict<MemberDef>(size) {}
    virtual ~MemberSDict() {}
    int compareItems(GCI item1,GCI item2);
};


#endif
