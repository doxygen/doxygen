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

#ifndef GROUPDEF_H
#define GROUPDEF_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>
#include "definition.h"
#include "memberlist.h"
#include "memberdef.h"
#include "htmlhelp.h"

class FileList;
class ClassList;
class FileDef;
class ClassDef;
class NamespaceDef;
class GroupList;
class OutputList;
class NamespaceList;
class MemberGroupList;
class MemberGroupDict;
class PageSDict;
class PageInfo;

class GroupDef : public Definition
{
  public:
    GroupDef(const char *fileName,int line,const char *name,const char *title);
   ~GroupDef();
    DefType definitionType() { return TypeGroup; }
    QCString getOutputFileBase() const { return fileName; }
    const char *groupTitle() const { return title; }
    void addFile(const FileDef *def); 
    void addClass(const ClassDef *def);
    void addNamespace(const NamespaceDef *def);
    void addGroup(const GroupDef *def);
    void addPage(PageInfo *def);                // pages in this group
    void addExample(const PageInfo *def);       // examples in this group
    void insertMember(MemberDef *def);
    void writeDocumentation(OutputList &ol);
    int countMembers() const;
    bool isLinkableInProject()
    {
      return hasDocumentation() && !isReference();
    }
    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }
    void computeAnchors();

    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();

    bool visited;    // number of times accessed for output - KPW

    friend void writeGroupTreeNode(OutputList&, GroupDef*);      // make accessible for writing tree view of group in index.cpp - KPW

  protected:
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);

  private: 
    QCString title;                     // title of the group
    QCString fileName;                  // base name of the generated file
    FileList *fileList;                 // list of files in the group
    ClassList *classList;               // list of classes in the group
    NamespaceList *namespaceList;       // list of namespaces in the group
    GroupList *groupList;               // list of sub groups.
    PageSDict *pageDict;                // list of pages in the group
    PageSDict *exampleDict;             // list of examples in the group

    MemberList *allMemberList;          // list of all members in the group
    QDict<MemberDef> *allMemberDict;
    
    // members sorted by type
    MemberList defineMembers;
    MemberList protoMembers;
    MemberList typedefMembers;
    MemberList enumMembers;
    MemberList enumValMembers;
    MemberList funcMembers;
    MemberList varMembers;

    /* user defined member groups */
    MemberGroupList *memberGroupList;   // list of member groups in this group
    MemberGroupDict *memberGroupDict;

};

class GroupList : public QList<GroupDef>
{
};

class GroupListIterator : public QListIterator<GroupDef>
{
  public:
    GroupListIterator(const GroupList &l) : QListIterator<GroupDef>(l) {}
};

void addClassToGroups(Entry *root,ClassDef *cd);
void addNamespaceToGroups(Entry *root,NamespaceDef *nd);
void addGroupToGroups(Entry *root,GroupDef *subGroup);
void addMemberToGroups(Entry *root,MemberDef *md);
void addPageToGroups(Entry *root,PageInfo *pi);
void addExampleToGroups(Entry *root,PageInfo *eg);

#endif

