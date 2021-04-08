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

#ifndef GROUPDEF_H
#define GROUPDEF_H

#include <memory>

#include "definition.h"
#include "dirdef.h"
#include "layout.h"
#include "membergroup.h"
#include "linkedmap.h"

class MemberList;
class MemberLists;
class FileList;
class ClassLinkedRefMap;
class ConceptLinkedRefMap;
class NamespaceLinkedRefMap;
class FileDef;
class ClassDef;
class ConceptDef;
class NamespaceDef;
class GroupList;
class OutputList;
class PageLinkedRefMap;
class PageDef;
class DirDef;
class FTVHelp;
class Entry;
class MemberDef;

/** A model of a group of symbols. */
class GroupDef : public DefinitionMutable, public Definition
{
  public:
   ~GroupDef() {}

    virtual DefType definitionType() const = 0;
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString anchor() const = 0;
    virtual QCString displayName(bool=TRUE) const = 0;
    virtual const char *groupTitle() const = 0;
    virtual void setGroupTitle( const char *newtitle ) = 0;
    virtual bool hasGroupTitle( ) const = 0;
    virtual void addFile(const FileDef *def) = 0;
    virtual bool addClass(const ClassDef *def) = 0;
    virtual bool addConcept(const ConceptDef *def) = 0;
    virtual bool addNamespace(const NamespaceDef *def) = 0;
    virtual void addGroup(const GroupDef *def) = 0;
    virtual void addPage(const PageDef *def) = 0;
    virtual void addExample(const PageDef *def) = 0;
    virtual void addDir(DirDef *dd) = 0;
    virtual bool insertMember(const MemberDef *def,bool docOnly=FALSE) = 0;
    virtual void removeMember(MemberDef *md) = 0;
    virtual bool findGroup(const GroupDef *def) const = 0;
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeMemberPages(OutputList &ol) = 0;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const = 0;
    virtual void writeTagFile(TextStream &) = 0;
    virtual size_t numDocMembers() const = 0;
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual bool isASubGroup() const = 0;
    virtual void computeAnchors() = 0;
    virtual void countMembers() = 0;

    virtual void addMembersToMemberGroup() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void findSectionsInDocumentation() = 0;

    virtual void addListReferences() = 0;
    virtual void sortMemberLists() = 0;
    virtual bool subGrouping() const = 0;

    virtual void setGroupScope(Definition *d) = 0;
    virtual Definition *getGroupScope() const = 0;

    virtual MemberList *getMemberList(MemberListType lt) const = 0;
    virtual const MemberLists &getMemberLists() const = 0;

    /* user defined member groups */
    virtual const MemberGroupList &getMemberGroups() const = 0;

    virtual const FileList &getFiles() const = 0;
    virtual const ClassLinkedRefMap &getClasses() const = 0;
    virtual const ConceptLinkedRefMap &getConcepts() const = 0;
    virtual const NamespaceLinkedRefMap &getNamespaces() const = 0;
    virtual const GroupList &getSubGroups() const = 0;
    virtual const PageLinkedRefMap &getPages() const = 0;
    virtual const DirList & getDirs() const = 0;
    virtual const PageLinkedRefMap &getExamples() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual void sortSubGroups() = 0;

};

GroupDef *createGroupDef(const char *fileName,int line,const char *name,
                                const char *title,const char *refFileName=0);

// --- Cast functions

GroupDef            *toGroupDef(Definition *d);
const GroupDef      *toGroupDef(const Definition *d);

// ------------------

class GroupLinkedMap : public LinkedMap<GroupDef>
{
};

class GroupList : public std::vector<const GroupDef *>
{
};

void addClassToGroups    (const Entry *root,ClassDef *cd);
void addConceptToGroups  (const Entry *root,ConceptDef *cd);
void addNamespaceToGroups(const Entry *root,NamespaceDef *nd);
void addGroupToGroups    (const Entry *root,GroupDef *subGroup);
void addMemberToGroups   (const Entry *root,MemberDef *md);
void addPageToGroups     (const Entry *root,PageDef *pd);
void addExampleToGroups  (const Entry *root,PageDef *eg);
void addDirToGroups      (const Entry *root,DirDef *dd);

#endif

