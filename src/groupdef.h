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

#include "sortdict.h"
#include "definition.h"

class MemberList;
class FileList;
class ClassSDict;
class FileDef;
class ClassDef;
class NamespaceDef;
class GroupList;
class OutputList;
class NamespaceSDict;
class MemberGroupSDict;
class MemberNameInfoSDict;
class PageSDict;
class PageDef;
class DirDef;
class DirList;
class FTVHelp;
class Entry;
class MemberDef;
class FTextStream;

/** A model of a group of symbols. */
class GroupDef : virtual public Definition
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
    virtual bool addNamespace(const NamespaceDef *def) = 0;
    virtual void addGroup(const GroupDef *def) = 0;
    virtual void addPage(PageDef *def) = 0;
    virtual void addExample(const PageDef *def) = 0;
    virtual void addDir(const DirDef *dd) = 0;
    virtual bool insertMember(MemberDef *def,bool docOnly=FALSE) = 0;
    virtual void removeMember(MemberDef *md) = 0;
    virtual bool findGroup(const GroupDef *def) const = 0;
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeMemberPages(OutputList &ol) = 0;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const = 0;
    virtual void writeTagFile(FTextStream &) = 0;
    virtual int  numDocMembers() const = 0;
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
    virtual const QList<MemberList> &getMemberLists() const = 0;

    /* user defined member groups */
    virtual MemberGroupSDict *getMemberGroupSDict() const = 0;

    virtual FileList *      getFiles() const = 0;
    virtual ClassSDict *    getClasses() const = 0;
    virtual NamespaceSDict * getNamespaces() const = 0;
    virtual GroupList *     getSubGroups() const = 0;
    virtual PageSDict *     getPages() const = 0;
    virtual DirList *       getDirs() const = 0;
    virtual PageSDict *     getExamples() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual void sortSubGroups() = 0;

};

GroupDef *createGroupDef(const char *fileName,int line,const char *name,
                                const char *title,const char *refFileName=0);

/** A sorted dictionary of GroupDef objects. */
class GroupSDict : public SDict<GroupDef>
{
  public:
    GroupSDict(uint size) : SDict<GroupDef>(size) {}
    virtual ~GroupSDict() {}
  private:
    int compareValues(const GroupDef *item1,const GroupDef *item2) const
    {
      return qstrcmp(item1->groupTitle(),item2->groupTitle());
    }
};

/** A list of GroupDef objects. */
class GroupList : public QList<GroupDef>
{
  public:
    int compareValues(const GroupDef *item1,const GroupDef *item2) const
    {
      return qstrcmp(item1->groupTitle(),item2->groupTitle());
    }
};

/** An iterator for GroupDef objects in a GroupList. */
class GroupListIterator : public QListIterator<GroupDef>
{
  public:
    GroupListIterator(const GroupList &l) : QListIterator<GroupDef>(l) {}
    virtual ~GroupListIterator() {}
};

void addClassToGroups    (const Entry *root,ClassDef *cd);
void addNamespaceToGroups(const Entry *root,NamespaceDef *nd);
void addGroupToGroups    (const Entry *root,GroupDef *subGroup);
void addMemberToGroups   (const Entry *root,MemberDef *md);
void addPageToGroups     (const Entry *root,PageDef *pd);
void addExampleToGroups  (const Entry *root,PageDef *eg);
void addDirToGroups      (const Entry *root,DirDef *dd);

#endif

