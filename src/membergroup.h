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

#include <vector>

#include <qlist.h>
#include "sortdict.h"
#include "types.h"
#include "reflist.h"

#define DOX_NOGROUP -1

class MemberList;
class MemberDef;
class ClassDef;
class NamespaceDef;
class FileDef;
class GroupDef;
class OutputList;
class Definition;
class FTextStream;
class RefItem;

/** A class representing a group of members. */
class MemberGroup
{
  public:
    //MemberGroup();
    MemberGroup(const Definition *container,int id,const char *header,
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
    void findSectionsInDocumentation(const Definition *d);
    int numDecMembers() const;
    int numDecEnumValues() const;
    int numDocMembers() const;
    int numDocEnumValues() const;
    const Definition *container() const;

    int countInheritableMembers(const ClassDef *inheritedFrom) const;
    void setInGroup(bool b);
    void addListReferences(Definition *d);
    void setRefItems(const RefItemVector &sli);
    MemberList *members() const { return memberList; }
    QCString anchor() const;

    QCString docFile() const { return m_docFile; }
    int docLine() const { return m_docLine; }

  private:
    const Definition *m_container;
    MemberList *memberList = 0;      // list of all members in the group
    MemberList *inDeclSection = 0;
    int grpId = 0;
    QCString grpHeader;
    QCString fileName;           // base name of the generated file
    QCString doc;
    bool inSameSection = true;
    QCString m_docFile;
    int m_docLine;
    RefItemVector m_xrefListItems;
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
  void setRefItems(const RefItemVector &sli);
  QCString header;
  QCString doc;
  QCString docFile;
  int docLine = -1;
  QCString compoundName;
  RefItemVector m_sli;
};

#endif
