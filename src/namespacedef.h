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

#ifndef NAMESPACEDEF_H
#define NAMESPACEDEF_H

#include <qstrlist.h>
#include <qdict.h>
#include "sortdict.h"
#include "definition.h"

class MemberList;
class ClassDef;
class ClassList;
class OutputList;
class ClassSDict;
class MemberDef;
class NamespaceList;
class MemberGroupSDict;
class NamespaceSDict;

/** A model of a namespace symbol. */
class NamespaceDef : public Definition
{
  public:
    NamespaceDef(const char *defFileName,int defLine,int defColumn,
                 const char *name,const char *ref=0,
                 const char *refFile=0);
   ~NamespaceDef();
    DefType definitionType() const { return TypeNamespace; }
    QCString getOutputFileBase() const;
    QCString anchor() const { return QCString(); }
    void insertUsedFile(const char *fname);
    
    void writeDocumentation(OutputList &ol);
    void writeMemberPages(OutputList &ol);
    void writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const;

    void insertClass(ClassDef *cd);
    void insertNamespace(NamespaceDef *nd);
    void insertMember(MemberDef *md);

    void computeAnchors();
    int countMembers();
    void addUsingDirective(NamespaceDef *nd);
    NamespaceSDict *getUsedNamespaces() const;
    void addUsingDeclaration(Definition *def);
    SDict<Definition> *getUsedClasses() const { return usingDeclList; }
    void combineUsingRelations();
    QCString displayName(bool=TRUE) const;
    QCString localName() const;
    
    bool isLinkableInProject() const;
    bool isLinkable() const;
    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    void sortMemberLists();

    virtual Definition *findInnerCompound(const char *name);
    void addInnerCompound(Definition *d);
    void addListReferences();
    
    MemberList *getMemberList(MemberListType lt) const;
    const QList<MemberList> &getMemberLists() const { return m_memberLists; }
    MemberDef    *getMemberByName(const QCString &) const;

    /*! Returns the user defined member groups */
    MemberGroupSDict *getMemberGroupSDict() const { return memberGroupSDict; }

    /*! Returns the classes contained in this namespace */
    ClassSDict *getClassSDict() const { return classSDict; }

    /*! Returns the namespaces contained in this namespace */
    NamespaceSDict *getNamespaceSDict() const { return namespaceSDict; }

    bool visited;

  private:
    MemberList *createMemberList(MemberListType lt);
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void writeClassDeclarations(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void writeNamespaceDeclarations(OutputList &ol,const QCString &title);
    void writeMemberGroups(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeSummaryLinks(OutputList &ol);

    QCString              fileName;
    QStrList              files;

    NamespaceSDict       *usingDirList;
    SDict<Definition>    *usingDeclList;
    SDict<Definition>    *m_innerCompounds;

    MemberSDict          *m_allMembersDict;
    QList<MemberList>     m_memberLists;
    MemberGroupSDict     *memberGroupSDict;
    ClassSDict           *classSDict;
    NamespaceSDict       *namespaceSDict;
    bool                  m_subGrouping;
};

/** A list of NamespaceDef objects. */
class NamespaceList : public QList<NamespaceDef>
{ 
  public:
   ~NamespaceList() {}
    int compareItems(QCollection::Item item1,QCollection::Item item2)
    {
      return qstricmp(((NamespaceDef *)item1)->name(),
                    ((NamespaceDef *)item2)->name()
                   );
    }
};

/** An iterator for NamespaceDef objects in a NamespaceList. */
class NamespaceListIterator : public QListIterator<NamespaceDef>
{
  public:
    NamespaceListIterator(const NamespaceList &l) : 
      QListIterator<NamespaceDef>(l) {}
};

/** An unsorted dictionary of NamespaceDef objects. */
class NamespaceDict : public QDict<NamespaceDef>
{
  public:
    NamespaceDict(int size) : QDict<NamespaceDef>(size) {}
   ~NamespaceDict() {}
};

/** A sorted dictionary of NamespaceDef objects. */
class NamespaceSDict : public SDict<NamespaceDef>
{
  public:
    NamespaceSDict(int size=17) : SDict<NamespaceDef>(size) {}
   ~NamespaceSDict() {}
    int compareItems(QCollection::Item item1,QCollection::Item item2)
    {
      return qstricmp(((NamespaceDef *)item1)->name(),
                    ((NamespaceDef *)item2)->name()
                   );
    }
    void writeDeclaration(OutputList &ol,const char *title,bool localName=FALSE);
    bool declVisible() const;
};



#endif
