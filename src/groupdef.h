/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include "qtbc.h"
#include "sortdict.h"
#include "definition.h"
#include "memberlist.h"
#include "memberdef.h"
#include "htmlhelp.h"

class FileList;
class ClassSDict;
class FileDef;
class ClassDef;
class NamespaceDef;
class GroupList;
class OutputList;
class NamespaceList;
class MemberGroupSDict;
class MemberNameInfoSDict;
class PageSDict;
class PageInfo;

class GroupDef : public Definition
{
  public:
    GroupDef(const char *fileName,int line,const char *name,const char *title);
   ~GroupDef();
    DefType definitionType() { return TypeGroup; }
    QCString getOutputFileBase() const;
    const char *groupTitle() const { return title; }
    void setGroupTitle( const char *newtitle );
    bool hasGroupTitle( ) { return titleSet; }
    void addFile(const FileDef *def); 
    void addClass(const ClassDef *def);
    void addNamespace(const NamespaceDef *def);
    void addGroup(const GroupDef *def);
    void addParentGroup(const GroupDef *def);
    void addPage(PageInfo *def);                // pages in this group
    void addExample(const PageInfo *def);       // examples in this group
    void insertMember(MemberDef *def,bool docOnly=FALSE);
    void removeMember(MemberDef *md);
    bool containsGroup(const GroupDef *def);    // true if def is already a subgroup
    void writeDocumentation(OutputList &ol);
    int countMembers() const;
    bool isLinkableInProject() const
    {
      return !isReference();
    }
    bool isLinkable() const
    {
      return TRUE;
    }
    bool isASubGroup() const;
    void computeAnchors();

    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();

    void addListReferences();

    bool visited;    // number of times accessed for output - KPW

    friend void writeGroupTreeNode(OutputList&, GroupDef*,bool);      
                    // make accessible for writing tree view of group in index.cpp - KPW

    // members in the declaration part of the documentation
    MemberList decDefineMembers;
    MemberList decProtoMembers;
    MemberList decTypedefMembers;
    MemberList decEnumMembers;
    MemberList decEnumValMembers;
    MemberList decFuncMembers;
    MemberList decVarMembers;

    // members in the documentation part of the documentation
    MemberList docDefineMembers;
    MemberList docProtoMembers;
    MemberList docTypedefMembers;
    MemberList docEnumMembers;
    MemberList docFuncMembers;
    MemberList docVarMembers;

    /* user defined member groups */
    MemberGroupSDict *memberGroupSDict;

    FileList *      getFiles() const        { return fileList; }
    ClassSDict *    getClasses() const      { return classSDict; }
    NamespaceList * getNamespaces() const   { return namespaceList; }
    GroupList *     getSubGroups() const    { return groupList; }
    PageSDict *     getPages() const        { return pageDict; }
    
  protected:
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);

  private: 
    QCString title;                     // title of the group
    bool titleSet;                      // true if title is not the same as the name
    QCString fileName;                  // base name of the generated file
    FileList *fileList;                 // list of files in the group
    ClassSDict *classSDict;             // list of classes in the group
    NamespaceList *namespaceList;       // list of namespaces in the group
    GroupList *groupList;              // list of sub groups.
    GroupList *parentGroupList;        // list of parent groups.
    PageSDict *pageDict;                // list of pages in the group
    PageSDict *exampleDict;             // list of examples in the group

    MemberList *allMemberList;
    MemberNameInfoSDict *allMemberNameInfoSDict;
    

};

class GroupSDict : public SDict<GroupDef>
{
  public:
    GroupSDict(uint size) : SDict<GroupDef>(size) {}
    virtual ~GroupSDict() {}
};

class GroupList : public QList<GroupDef>
{
};

void addClassToGroups(Entry *root,ClassDef *cd);
void addNamespaceToGroups(Entry *root,NamespaceDef *nd);
void addGroupToGroups(Entry *root,GroupDef *subGroup);
void addMemberToGroups(Entry *root,MemberDef *md);
void addPageToGroups(Entry *root,PageInfo *pi);
void addExampleToGroups(Entry *root,PageInfo *eg);

#endif

