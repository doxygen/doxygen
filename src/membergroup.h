/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#ifndef MEMBERGROUP_H
#define MEMBERGROUP_H

#include "qtbc.h"
#include <qlist.h>
#include "sortdict.h"

#define DOX_NOGROUP -1

class MemberDef;
class ClassDef;
class NamespaceDef;
class FileDef;
class MemberList;
class GroupDef;
class OutputList;
class Definition;

class MemberGroup 
{
  public:
    MemberGroup(Definition *parent,int id,const char *header,
                const char *docs,const char *docFile);
   ~MemberGroup();
    QCString header() const { return grpHeader; }
    int groupId() const { return grpId; }
    void insertMember(MemberDef *md);
    void setAnchors(ClassDef *);
    void writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);
    void writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);

    void writeDocumentation(OutputList &ol,const char *scopeName,
               Definition *container);
    QCString documentation() { return doc; }
    bool allMembersInSameSection() { return inSameSection; }
    void addToDeclarationSection();
    int countDecMembers(/*bool sectionPerType*/);
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    int varCount() const;
    int funcCount() const;
    int enumCount() const;
    int enumValueCount() const;
    int typedefCount() const;
    int protoCount() const;
    int defineCount() const;
    int friendCount() const;
    int numDecMembers() const;
    int numDocMembers() const;
    void setInGroup(bool b);
    void addListReferences(Definition *d);
    MemberList *members() const { return memberList; }
    Definition *parent() const { return m_parent; }

  private: 
    MemberList *memberList;      // list of all members in the group
    int grpId;
    QCString grpHeader;
    QCString fileName;           // base name of the generated file
    Definition *scope;
    QCString doc;
    MemberList *inDeclSection;
    bool inSameSection;
    int  m_numDecMembers;
    Definition *m_parent;
    QCString m_docFile;
};

class MemberGroupList : public QList<MemberGroup>
{
};

class MemberGroupListIterator : public QListIterator<MemberGroup>
{
  public:
    MemberGroupListIterator(const MemberGroupList &l) : 
      QListIterator<MemberGroup>(l) {}
};

class MemberGroupSDict : public SIntDict<MemberGroup>
{
  public:
    MemberGroupSDict(int size=17) : SIntDict<MemberGroup>(size) {}
   ~MemberGroupSDict() {}
};

struct MemberGroupInfo
{
  QCString header;
  QCString doc;
  QCString docFile;
};

//class MemberGroupDict : public QIntDict<MemberGroup>
//{
//  public:
//    MemberGroupDict(int size) : QIntDict<MemberGroup>(size) {}
//   ~MemberGroupDict() {}
//};

//class MemberGroupDictIterator : public QIntDictIterator<MemberGroup>
//{
//  public:
//    MemberGroupDictIterator(const MemberGroupDict &d) : 
//      QIntDictIterator<MemberGroup>(d) {}
//   ~MemberGroupDictIterator() {}
//};

#endif
