/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
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

class FileList;
class ClassList;
class FileDef;
class ClassDef;
class NamespaceDef;
class GroupList;
class OutputList;
class NamespaceList;

class GroupDef : public Definition
{
  public:
    GroupDef(const char *name,const char *title);
   ~GroupDef();
    //const char *groupFile() const { return fileName; }
    QCString getOutputFileBase() const { return fileName; }
    const char *groupTitle() const { return title; }
    void addFile(const FileDef *def); 
    void addClass(const ClassDef *def);
    void addNamespace(const NamespaceDef *def);
    void addMember(const MemberDef *def);
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

  private: 
    QCString title;                     // title of the group
    QCString fileName;                  // base name of the generated file
    FileList *fileList;                 // list of all files in the group
    ClassList *classList;               // list of all classes in the group
    NamespaceList *namespaceList;       // list of all namespace in the group

    MemberList *allMemberList;          // list of all members in the group
    QDict<MemberDef> *allMemberDict;
    // members sorted to type
    MemberList defineMembers;
    MemberList protoMembers;
    MemberList typedefMembers;
    MemberList enumMembers;
    MemberList enumValMembers;
    MemberList funcMembers;
    MemberList varMembers;
};

class GroupList : public QList<GroupDef>
{
};

class GroupListIterator : public QListIterator<GroupDef>
{
  public:
    GroupListIterator(const GroupList &l) : QListIterator<GroupDef>(l) {}
};

#endif
