/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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
class StorageIntf;

class MemberList : public QList<MemberDef> 
{ 
  public:
    enum ListType
    {
      privateLists       = 0x0800,
      detailedLists      = 0x1000,
      declarationLists   = 0x2000,
      documentationLists = 0x4000,

      pubMethods              = 0,
      proMethods              = 1,
      pacMethods              = 2,
      priMethods              = 3 + privateLists,
      pubStaticMethods        = 4,
      proStaticMethods        = 5,
      pacStaticMethods        = 6,
      priStaticMethods        = 7 + privateLists,
      pubSlots                = 8,
      proSlots                = 9,
      priSlots                = 10 + privateLists,
      pubAttribs              = 11,
      proAttribs              = 12,
      pacAttribs              = 13,
      priAttribs              = 14 + privateLists,
      pubStaticAttribs        = 15,
      proStaticAttribs        = 16,
      pacStaticAttribs        = 17,
      priStaticAttribs        = 18 + privateLists,
      pubTypes                = 19,
      proTypes                = 20,
      pacTypes                = 21,
      priTypes                = 22 + privateLists,
      related                 = 23,
      signals                 = 24,
      friends                 = 25,
      dcopMethods             = 26,
      properties              = 27,
      events                  = 28,

      typedefMembers          = 29 + detailedLists,
      enumMembers             = 30 + detailedLists,
      enumValMembers          = 31 + detailedLists,
      functionMembers         = 32 + detailedLists,
      relatedMembers          = 33 + detailedLists,
      variableMembers         = 34 + detailedLists,
      propertyMembers         = 35 + detailedLists,
      eventMembers            = 36 + detailedLists,
      constructors            = 37 + detailedLists,

      allMembersList          = 38,

      decDefineMembers        = 39 + declarationLists,
      decProtoMembers         = 40 + declarationLists, 
      decTypedefMembers       = 41 + declarationLists,
      decEnumMembers          = 42 + declarationLists,
      decFuncMembers          = 43 + declarationLists,
      decVarMembers           = 44 + declarationLists,
      decEnumValMembers       = 45 + declarationLists,

      docDefineMembers        = 46 + documentationLists,
      docProtoMembers         = 47 + documentationLists,
      docTypedefMembers       = 48 + documentationLists,
      docEnumMembers          = 49 + documentationLists,
      docFuncMembers          = 50 + documentationLists,
      docVarMembers           = 51 + documentationLists,
      docEnumValMembers       = 52 + documentationLists,

      redefinedBy             = 53,
      enumFields              = 54,
      memberGroup             = 55
    };

    MemberList();
    MemberList(ListType lt);
   ~MemberList();
    ListType listType() const { return m_listType; }
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
    void countDecMembers(bool countEnumValues=FALSE);
    void countDocMembers(bool countEnumValues=FALSE);
    void writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);
    void writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               const char *title,const char *subtitle,bool showEnumValues=FALSE);
    void writeDocumentation(OutputList &ol,const char *scopeName,
               Definition *container,const char *title,bool showEnumValues=FALSE);
    void writeDocumentationPage(OutputList &ol,
               const char *scopeName, Definition *container);
    void addMemberGroup(MemberGroup *mg);
    void setInGroup(bool inGroup) { m_inGroup=inGroup; }
    void setInFile(bool inFile) { m_inFile=inFile; }
    void addListReferences(Definition *def);
    void findSectionsInDocumentation();
    MemberGroupList *getMemberGroupList() const { return memberGroupList; }

    void marshal(StorageIntf *s);
    void unmarshal(StorageIntf *s);

  private:
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
    ListType m_listType;
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
