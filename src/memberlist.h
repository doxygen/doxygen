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

#ifndef MEMBERLIST_H
#define MEMBERLIST_H

#include <qlist.h>
#include "memberdef.h"
#include "sortdict.h"
#include "types.h"

class GroupDef;
class MemberGroup;
class MemberGroupList;
class StorageIntf;

/** A list of MemberDef objects. */
class MemberList : public QList<MemberDef> 
{ 
  public:
    MemberList();
    MemberList(MemberListType lt);
   ~MemberList();
    MemberListType listType() const { return m_listType; }
    static QCString listTypeAsString(MemberListType type);
    bool insert(uint index,const MemberDef *md);
    void inSort(const MemberDef *md);
    void append(const MemberDef *md);
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
    bool needsSorting() const  { return m_needsSorting; }
    void countDecMembers(bool countEnumValues=FALSE,GroupDef *gd=0);
    void countDocMembers(bool countEnumValues=FALSE);
    int countInheritableMembers(ClassDef *inheritedFrom) const;
    void writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd, GroupDef *gd,
               ClassDef *inheritedFrom,const char *inheritId);
    void writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               const char *title,const char *subtitle,
               bool showEnumValues=FALSE,bool showInline=FALSE,
               ClassDef *inheritedFrom=0,MemberListType lt=MemberListType_pubMethods);
    void writeDocumentation(OutputList &ol,const char *scopeName,
               Definition *container,const char *title,bool showEnumValues=FALSE,bool showInline=FALSE);
    void writeSimpleDocumentation(OutputList &ol,Definition *container);
    void writeDocumentationPage(OutputList &ol,
               const char *scopeName, Definition *container);
    void writeTagFile(FTextStream &);
    bool declVisible() const;
    void addMemberGroup(MemberGroup *mg);
    void setInGroup(bool inGroup) { m_inGroup=inGroup; }
    void setInFile(bool inFile) { m_inFile=inFile; }
    void addListReferences(Definition *def);
    void findSectionsInDocumentation();
    void setNeedsSorting(bool b);
    MemberGroupList *getMemberGroupList() const { return memberGroupList; }

    void marshal(StorageIntf *s);
    void unmarshal(StorageIntf *s);

  private:
    int compareValues(const MemberDef *item1,const MemberDef *item2) const;
    int m_varCnt;
    int m_funcCnt;
    int m_enumCnt;
    int m_enumValCnt;
    int m_typeCnt;
    int m_protoCnt;
    int m_defCnt;
    int m_friendCnt; 
    int m_numDecMembers; // number of members in the brief part of the memberlist
    int m_numDocMembers; // number of members in the detailed part of the memberlist
    MemberGroupList *memberGroupList;
    bool m_inGroup; // is this list part of a group definition
    bool m_inFile;  // is this list part of a file definition
    MemberListType m_listType;
    bool m_needsSorting;
};

/** An iterator for MemberDef objects in a MemberList. */
class MemberListIterator : public QListIterator<MemberDef>
{
  public:
    MemberListIterator(const QList<MemberDef> &list);
    virtual ~MemberListIterator() {}
};

/** An unsorted dictionary of MemberDef objects. */
class MemberDict : public QDict<MemberDef>
{
  public:
    MemberDict(int size) : QDict<MemberDef>(size) {}
    virtual ~MemberDict() {}
};

/** A sorted dictionary of MemberDef objects. */
class MemberSDict : public SDict<MemberDef>
{
  public:
    MemberSDict(int size=17) : SDict<MemberDef>(size) {}
    virtual ~MemberSDict() {}
  private:
    int compareValues(const MemberDef *item1,const MemberDef *item2) const;
};


#endif
