/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#ifndef NAMESPACEDEF_H
#define NAMESPACEDEF_H

#include "qtbc.h"
#include <qstrlist.h>
#include <qdict.h>
#include "sortdict.h"
#include "definition.h"
#include "memberlist.h"

class ClassDef;
class ClassList;
class OutputList;
class ClassSDict;
class MemberDef;
class NamespaceList;
class MemberGroupSDict;
class NamespaceSDict;

class NamespaceDef : public Definition
{
  public:
    NamespaceDef(const char *defFileName,int defLine,
                 const char *name,const char *ref=0,
                 const char *refFile=0);
   ~NamespaceDef();
    DefType definitionType() const { return TypeNamespace; }
    QCString getOutputFileBase() const;
    void insertUsedFile(const char *fname);
    
    void writeDetailedDocumentation(OutputList &ol);
    void writeDocumentation(OutputList &ol);
    void writeMemberDocumentation(OutputList &ol);
    void writeMemberPages(OutputList &ol);
    void writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const;

    void insertClass(ClassDef *cd);
    void insertNamespace(NamespaceDef *nd);
    void insertMember(MemberDef *md);

    void computeAnchors();
    int countMembers();
    void addUsingDirective(NamespaceDef *nd);
    NamespaceSDict *getUsedNamespaces() const { return usingDirList; }
    void addUsingDeclaration(Definition *def);
    SDict<Definition> *getUsedClasses() const { return usingDeclList; }
    void combineUsingRelations();
    QCString displayName() const;
    
    bool isLinkableInProject() const
    {
      int i = name().findRev("::");
      if (i==-1) i=0; else i+=2;
      return !name().isEmpty() && name().at(i)!='@' &&
              hasDocumentation() && !isReference();
    }
    bool isLinkable() const
    {
      return isLinkableInProject() || isReference();
    }
    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();

    virtual Definition *findInnerCompound(const char *name);
    void addInnerCompound(Definition *d);
    void addListReferences();
    
  //protected:
  //  void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);

  public:
    
    // members in the declaration part of the documentation
    MemberList decDefineMembers;
    MemberList decProtoMembers;
    MemberList decTypedefMembers;
    MemberList decEnumMembers;
    MemberList decFuncMembers;
    MemberList decVarMembers;

    // members in the documentation part of the documentation
    MemberList docAllMemberList;
    MemberList docDefineMembers;
    MemberList docProtoMembers;
    MemberList docTypedefMembers;
    MemberList docEnumMembers;
    MemberList docFuncMembers;
    MemberList docVarMembers;

    /* user defined member groups */
    MemberGroupSDict    *memberGroupSDict;

    /*! Classes inside this namespace */
    ClassSDict *classSDict;
    /*! Namespaces inside this namespace */
    NamespaceSDict *namespaceSDict;

    bool visited;

  private:
    //QCString reference;
    QCString fileName;
    QStrList files;


    NamespaceSDict *usingDirList;
    SDict<Definition> *usingDeclList;
    SDict<Definition> *m_innerCompounds;

    MemberList allMemberList;

};

class NamespaceList : public QList<NamespaceDef>
{ 
  public:
   ~NamespaceList() {}
    int compareItems(GCI item1,GCI item2)
    {
      return stricmp(((NamespaceDef *)item1)->name(),
                    ((NamespaceDef *)item2)->name()
                   );
    }
};

class NamespaceListIterator : public QListIterator<NamespaceDef>
{
  public:
    NamespaceListIterator(const NamespaceList &l) : 
      QListIterator<NamespaceDef>(l) {}
};

class NamespaceDict : public QDict<NamespaceDef>
{
  public:
    NamespaceDict(int size) : QDict<NamespaceDef>(size) {}
   ~NamespaceDict() {}
};

class NamespaceSDict : public SDict<NamespaceDef>
{
  public:
    NamespaceSDict(int size=17) : SDict<NamespaceDef>(size) {}
   ~NamespaceSDict() {}
    int compareItems(GCI item1,GCI item2)
    {
      return stricmp(((NamespaceDef *)item1)->name(),
                    ((NamespaceDef *)item2)->name()
                   );
    }
    void writeDeclaration(OutputList &ol,bool localName=FALSE);
};



#endif
