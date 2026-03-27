/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
#include <map>
#include <memory>

#include "types.h"
#include "reflist.h"
#include "requirement.h"

#define DOX_NOGROUP -1

class MemberList;
class MemberDef;
class ClassDef;
class NamespaceDef;
class FileDef;
class ModuleDef;
class GroupDef;
class OutputList;
class Definition;
class DefinitionMutable;
class RefItem;
class TextStream;

/** A class representing a group of members. */
class MemberGroup
{
  public:
    //MemberGroup();
    MemberGroup(const Definition *container,int id,const QCString &header,
                const QCString &docs,const QCString &docFile,int docLine,MemberListContainer con);
    QCString header() const { return grpHeader; }
    int groupId() const { return grpId; }
    void insertMember(MemberDef *md);
    void setAnchors();
    void writePlainDeclarations(OutputList &ol,bool inGroup,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
               int indentLevel, const ClassDef *inheritedFrom,const QCString &inheritId) const;
    void writeDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
               bool showInline=FALSE) const;
    void writeDocumentation(OutputList &ol,const QCString &scopeName,
               const Definition *container,bool showEnumValues,bool showInline) const;
    void writeDocumentationPage(OutputList &ol,const QCString &scopeName,
               const DefinitionMutable *container) const;
    void writeTagFile(TextStream &,bool qualifiedName=false);
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
    const Definition *memberContainer() const;

    int countInheritableMembers(const ClassDef *inheritedFrom) const;
    void addListReferences(const Definition *def);
    void addRequirementReferences(const Definition *def);
    void setRefItems(const RefItemVector &sli);
    void setRequirementReferences(const RequirementRefs &rqli);
    const MemberList &members() const { return *memberList.get(); }

    QCString docFile() const { return m_docFile; }
    int docLine() const { return m_docLine; }

  private:
    const Definition *m_container;
    std::unique_ptr<MemberList> memberList;      // list of all members in the group
    MemberList *inDeclSection = nullptr;
    int grpId = 0;
    QCString grpHeader;
    QCString fileName;           // base name of the generated file
    QCString doc;
    bool inSameSection = true;
    QCString m_docFile;
    int m_docLine;
    RefItemVector m_xrefListItems;
    RequirementRefs m_requirementRefs;
};

class MemberGroupRefList : public std::vector<MemberGroup *>
{
};

class MemberGroupList : public std::vector< std::unique_ptr<MemberGroup> >
{
};

/** Data collected for a member group */
struct MemberGroupInfo
{
  void setRefItems(const RefItemVector &sli);
  void setRequirementReferences(const RequirementRefs &rqli);
  QCString header;
  QCString doc;
  QCString docFile;
  int docLine = -1;
  QCString compoundName;
  RefItemVector m_sli;
  RequirementRefs m_rqli;
};

using MemberGroupInfoMap = std::unordered_map< int,std::unique_ptr<MemberGroupInfo> >;

#endif
