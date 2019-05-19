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

#ifndef MEMBERGROUP_H
#define MEMBERGROUP_H

#include <qlist.h>
#include "sortdict.h"
#include "types.h"

#define DOX_NOGROUP -1

class MemberList;
class MemberDef;
class ClassDef;
class NamespaceDef;
class FileDef;
class GroupDef;
class OutputList;
class Definition;
class StorageIntf;
class FTextStream;
struct ListItemInfo;

/** A class representing a group of members. */
class MemberGroup 
{
  public:
    MemberGroup();
    MemberGroup(const Definition *parent,int id,const char *header,
                const char *docs,const char *docFile,int docLine);
   ~MemberGroup();
    QCString header() const { return grpHeader; }
    int groupId() const { return grpId; }
    void insertMember(MemberDef *md);
    void setAnchors();
    void writePlainDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               const ClassDef *inheritedFrom,const char *inheritId) const;
    void writeDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               bool showInline=FALSE) const;
    void writeDocumentation(OutputList &ol,const char *scopeName,
               const Definition *container,bool showEnumValues,bool showInline) const;
    void writeDocumentationPage(OutputList &ol,const char *scopeName,
               const Definition *container) const;
    void writeTagFile(FTextStream &);
    void addGroupedInheritedMembers(OutputList &ol,const ClassDef *cd,
               MemberListType lt,
               const ClassDef *inheritedFrom,const QCString &inheritId) const;
    void setAnonymousEnumType();

    const QCString &documentation() const { return doc; }
    bool allMembersInSameSection() const { return inSameSection; }
    void addToDeclarationSection();
    void countDecMembers();
    void countDocMembers();
    int countGroupedInheritedMembers(MemberListType lt);
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    /*
    int varCount() const;
    int funcCount() const;
    int enumCount() const;
    int enumValueCount() const;
    int typedefCount() const;
    int sequenceCount() const;
    int dictionaryCount() const;
    int protoCount() const;
    int defineCount() const;
    int friendCount() const;
    */
    int numDecMembers() const;
    int numDecEnumValues() const;
    int numDocMembers() const;
    int numDocEnumValues() const;

    int countInheritableMembers(const ClassDef *inheritedFrom) const;
    void setInGroup(bool b);
    void addListReferences(Definition *d);
    void setRefItems(const QList<ListItemInfo> *sli);
    MemberList *members() const { return memberList; }
    const Definition *parent() const { return m_parent; }
    QCString anchor() const;

    QCString docFile() const { return m_docFile; }
    int docLine() const { return m_docLine; }

  private: 
    MemberList *memberList;      // list of all members in the group
    MemberList *inDeclSection;
    int grpId;
    QCString grpHeader;
    QCString fileName;           // base name of the generated file
    QCString doc;
    bool inSameSection;
    int  m_numDecMembers;
    int  m_numDocMembers;
    const Definition *m_parent;
    QCString m_docFile;
    int m_docLine;
    QList<ListItemInfo> *m_xrefListItems;
};

/** A list of MemberGroup objects. */
class MemberGroupList : public QList<MemberGroup>
{
};

/** An iterator for MemberGroup objects in a MemberGroupList. */
class MemberGroupListIterator : public QListIterator<MemberGroup>
{
  public:
    MemberGroupListIterator(const MemberGroupList &l) : 
      QListIterator<MemberGroup>(l) {}
};

/** A sorted dictionary of MemberGroup objects. */
class MemberGroupSDict : public SIntDict<MemberGroup>
{
  public:
    MemberGroupSDict(int size=17) : SIntDict<MemberGroup>(size) {}
   ~MemberGroupSDict() {}
 private:
    int compareValues(const MemberGroup *item1,const MemberGroup *item2) const
    {
      return item1->groupId() - item2->groupId();
    }
};

/** Data collected for a member group */
struct MemberGroupInfo
{
  MemberGroupInfo() : docLine(-1), m_sli(0) {}
 ~MemberGroupInfo() { delete m_sli; m_sli=0; }
  void setRefItems(const QList<ListItemInfo> *sli);
  QCString header;
  QCString doc;
  QCString docFile;
  int docLine;
  QCString compoundName;
  QList<ListItemInfo> *m_sli;
};

#endif
