/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
#include <qintdict.h>

//#include "definition.h"

#define NOGROUP -1

class MemberDef;
class ClassDef;
class NamespaceDef;
class FileDef;
class MemberList;
class GroupDef;
class OutputList;
class Definition;

class MemberGroup /* : public Definition */
{
  public:
    MemberGroup(int id,const char *header,const char *docs);
   ~MemberGroup();
    QCString header() const { return grpHeader; }
    int groupId() const { return grpId; }
    void insertMember(MemberDef *);
    void setAnchors();
    void writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool inGroup=TRUE);
    void writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);
    QCString documentation() { return doc; }
    bool allMembersInSameSection() { return inSameSection; }
    void addToDeclarationSection();
    int countDecMembers();
    void distributeMemberGroupDocumentation();

  private: 
    MemberList *memberList;      // list of all members in the group
    int grpId;
    QCString grpHeader;
    QCString fileName;           // base name of the generated file
    Definition *scope;
    QCString doc;
    MemberList *inDeclSection;
    bool inSameSection;
    int  numDeclMembers;
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

class MemberGroupDict : public QIntDict<MemberGroup>
{
  public:
    MemberGroupDict(int size) : QIntDict<MemberGroup>(size) {}
   ~MemberGroupDict() {}
};

class MemberGroupDictIterator : public QIntDictIterator<MemberGroup>
{
  public:
    MemberGroupDictIterator(const MemberGroupDict &d) : 
      QIntDictIterator<MemberGroup>(d) {}
   ~MemberGroupDictIterator() {}
};

#endif
