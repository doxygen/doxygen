/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

/** A model of a group of symbols. */
class GroupDef : public Definition
{
  public:
    GroupDef(const char *fileName,int line,const char *name,const char *title,const char *refFileName=0);
   ~GroupDef();
    DefType definitionType() const { return TypeGroup; }
    QCString getOutputFileBase() const;
    QCString anchor() const { return QCString(); }
    QCString displayName(bool=TRUE) const { return hasGroupTitle() ? title : Definition::name(); }
    const char *groupTitle() const { return title; }
    void setGroupTitle( const char *newtitle );
    bool hasGroupTitle( ) const { return titleSet; }
    void addFile(const FileDef *def); 
    bool addClass(const ClassDef *def);
    bool addNamespace(const NamespaceDef *def);
    void addGroup(const GroupDef *def);
    void addParentGroup(const GroupDef *def);
    void addPage(PageDef *def);
    void addExample(const PageDef *def);
    void addDir(const DirDef *dd);
    bool insertMember(MemberDef *def,bool docOnly=FALSE);
    void removeMember(MemberDef *md);
    bool containsGroup(const GroupDef *def);    // true if def is already a subgroup
    void writeDocumentation(OutputList &ol);
    void writeMemberPages(OutputList &ol);
    void writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const;
    int  countMembers() const;
    bool isLinkableInProject() const;
    bool isLinkable() const;
    bool isASubGroup() const;
    void computeAnchors();

    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();

    void addListReferences();
    void sortMemberLists();

    bool visited;    // number of times accessed for output - KPW

    //friend void writeGroupTreeNode(OutputList&, GroupDef*, int, FTVHelp*);      
                    // make accessible for writing tree view of group in index.cpp - KPW

    void setGroupScope(Definition *d) { groupScope = d; }
    Definition *getGroupScope() const { return groupScope; }

    MemberList *getMemberList(MemberListType lt) const;
    const QList<MemberList> &getMemberLists() const { return m_memberLists; }

    /* user defined member groups */
    MemberGroupSDict *getMemberGroupSDict() const { return memberGroupSDict; }

    FileList *      getFiles() const        { return fileList; }
    ClassSDict *    getClasses() const      { return classSDict; }
    NamespaceSDict * getNamespaces() const   { return namespaceSDict; }
    GroupList *     getSubGroups() const    { return groupList; }
    PageSDict *     getPages() const        { return pageDict; }
    DirList *       getDirs() const         { return dirList; }
    PageSDict *     getExamples() const     { return exampleDict; }
    //MemberList*     getMembers() const      { return allMemberList; }
    void sortSubGroups();
    
  protected:
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);

  private: 
    MemberList *createMemberList(MemberListType lt);
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void removeMemberFromList(MemberListType lt,MemberDef *md);
    void writeGroupGraph(OutputList &ol);
    void writeFiles(OutputList &ol,const QCString &title);
    void writeNamespaces(OutputList &ol,const QCString &title);
    void writeNestedGroups(OutputList &ol,const QCString &title);
    void writeDirs(OutputList &ol,const QCString &title);
    void writeClasses(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void writePageDocumentation(OutputList &ol);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void writeSummaryLinks(OutputList &ol);
    void updateLanguage(const Definition *);

    QCString title;                      // title of the group
    bool titleSet;                       // true if title is not the same as the name
    QCString fileName;                   // base name of the generated file
    FileList *fileList;                  // list of files in the group
    ClassSDict *classSDict;              // list of classes in the group
    NamespaceSDict *namespaceSDict;      // list of namespaces in the group
    GroupList *groupList;                // list of sub groups.
    PageSDict *pageDict;                 // list of pages in the group
    PageSDict *exampleDict;              // list of examples in the group
    DirList *dirList;                    // list of directories in the group

    MemberList *allMemberList;
    MemberNameInfoSDict *allMemberNameInfoSDict;
    
    Definition *groupScope;

    QList<MemberList> m_memberLists;
    MemberGroupSDict *memberGroupSDict;

};

/** A sorted dictionary of GroupDef objects. */
class GroupSDict : public SDict<GroupDef>
{
  public:
    GroupSDict(uint size) : SDict<GroupDef>(size) {}
    virtual ~GroupSDict() {}
    int compareItems(QCollection::Item item1,QCollection::Item item2)
    {
      return qstrcmp(((GroupDef*)item1)->groupTitle(),((GroupDef*)item2)->groupTitle());
    }
};

/** A list of GroupDef objects. */
class GroupList : public QList<GroupDef>
{
  public:
    int compareItems(QCollection::Item item1,QCollection::Item item2)
    {
      return qstrcmp(((GroupDef*)item1)->groupTitle(),((GroupDef*)item2)->groupTitle());
    }
};

/** An iterator for GroupDef objects in a GroupList. */
class GroupListIterator : public QListIterator<GroupDef>
{
  public:
    GroupListIterator(const GroupList &l) : QListIterator<GroupDef>(l) {}
    virtual ~GroupListIterator() {}
};

void addClassToGroups(Entry *root,ClassDef *cd);
void addNamespaceToGroups(Entry *root,NamespaceDef *nd);
void addGroupToGroups(Entry *root,GroupDef *subGroup);
void addMemberToGroups(Entry *root,MemberDef *md);
void addPageToGroups(Entry *root,PageDef *pd);
void addExampleToGroups(Entry *root,PageDef *eg);
void addDirToGroups(Entry *root,DirDef *dd);

#endif

