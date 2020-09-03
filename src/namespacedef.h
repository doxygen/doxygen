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

#ifndef NAMESPACEDEF_H
#define NAMESPACEDEF_H

#include <qstrlist.h>
#include <qdict.h>
#include "sortdict.h"
#include "definition.h"
#include "filedef.h"

class MemberList;
class ClassDef;
class OutputList;
class ClassSDict;
class MemberDef;
class MemberGroupSDict;
class NamespaceSDict;
class FTextStream;

/** An abstract interface of a namespace symbol. */
class NamespaceDef : virtual public Definition
{
  public:
    virtual ~NamespaceDef() {}
    virtual DefType definitionType() const = 0;
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString anchor() const = 0;
    virtual void insertUsedFile(FileDef *fd) = 0;

    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeMemberPages(OutputList &ol) = 0;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const = 0;
    virtual void writeTagFile(FTextStream &) = 0;

    virtual void insertClass(const ClassDef *cd) = 0;
    virtual void insertNamespace(const NamespaceDef *nd) = 0;
    virtual void insertMember(MemberDef *md) = 0; // md cannot be const, since setSectionList is called on it

    virtual void computeAnchors() = 0;
    virtual void countMembers() = 0;
    virtual int numDocMembers() const = 0;
    virtual void addUsingDirective(const NamespaceDef *nd) = 0;
    virtual const NamespaceSDict *getUsedNamespaces() const = 0;
    virtual void addUsingDeclaration(const Definition *def) = 0;
    virtual const SDict<Definition> *getUsedClasses() const = 0;
    virtual void combineUsingRelations() = 0;
    virtual QCString displayName(bool=TRUE) const = 0;
    virtual QCString localName() const = 0;
    virtual void setInline(bool isInline) = 0;

    virtual bool isConstantGroup() const = 0;
    virtual bool isModule()        const = 0;
    virtual bool isLibrary() const = 0;
    virtual bool isInline() const = 0;

    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual void addMembersToMemberGroup() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void sortMemberLists() = 0;

    virtual Definition *findInnerCompound(const char *name) const = 0;
    virtual void addInnerCompound(const Definition *d) = 0;
    virtual void addListReferences() = 0;
    virtual void setFileName(const QCString &fn) = 0;

    virtual bool subGrouping() const = 0;

    virtual MemberList *getMemberList(MemberListType lt) const = 0;
    virtual const QList<MemberList> &getMemberLists() const = 0;
    virtual MemberDef    *getMemberByName(const QCString &) const = 0;

    /*! Returns the user defined member groups */
    virtual MemberGroupSDict *getMemberGroupSDict() const = 0;

    /*! Returns the classes contained in this namespace */
    virtual ClassSDict *getClassSDict() const = 0;

    /*! Returns the Slice interfaces contained in this namespace */
    virtual ClassSDict *getInterfaceSDict() const = 0;

    /*! Returns the Slice structs contained in this namespace */
    virtual ClassSDict *getStructSDict() const = 0;

    /*! Returns the Slice exceptions contained in this namespace */
    virtual ClassSDict *getExceptionSDict() const = 0;

    /*! Returns the namespaces contained in this namespace */
    virtual const NamespaceSDict *getNamespaceSDict() const = 0;

    virtual QCString title() const = 0;
    virtual QCString compoundTypeString() const = 0;

    virtual void setMetaData(const QCString &m) = 0;
    virtual void setVisited(bool v) = 0;
    virtual bool isVisited() const = 0;
};

/** Factory method to create new NamespaceDef instance */
NamespaceDef *createNamespaceDef(const char *defFileName,int defLine,int defColumn,
                 const char *name,const char *ref=0,
                 const char *refFile=0,const char*type=0,
                 bool isPublished=false);

/** Factory method to create an alias of an existing namespace. Used for inline namespaces. */
NamespaceDef *createNamespaceDefAlias(const Definition *newScope, const NamespaceDef *nd);

/** A list of NamespaceDef objects. */
class NamespaceList : public QList<NamespaceDef>
{
  public:
   ~NamespaceList() {}
    int compareValues(const NamespaceDef *nd1,const NamespaceDef *nd2) const
    {
      return qstricmp(nd1->name(), nd2->name());
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
    NamespaceDict(uint size) : QDict<NamespaceDef>(size) {}
   ~NamespaceDict() {}
};

/** A sorted dictionary of NamespaceDef objects. */
class NamespaceSDict : public SDict<NamespaceDef>
{
  public:
    NamespaceSDict(uint size=17) : SDict<NamespaceDef>(size) {}
   ~NamespaceSDict() {}
    void writeDeclaration(OutputList &ol,const char *title,
            bool isConstantGroup=false, bool localName=FALSE);
    bool declVisible() const;
  private:
    int compareValues(const NamespaceDef *item1,const NamespaceDef *item2) const
    {
      return qstricmp(item1->name(),item2->name());
    }
};



#endif
