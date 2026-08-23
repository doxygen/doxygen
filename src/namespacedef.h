/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include <memory>
#include <unordered_set>

#include "classdef.h"
#include "definition.h"
#include "linkedmap.h"

class ClassDef;
class ClassLinkedRefMap;
class ConceptDef;
class ConceptLinkedRefMap;
class FileDef;
class MemberDef;
class MemberGroupList;
class MemberList;
class NamespaceDef;
class OutputList;

// --- Set of namespaces

using NamespaceDefSet = std::unordered_set<const NamespaceDef*>;

class NamespaceLinkedMap final : public LinkedMap<NamespaceDef>
{
};

class NamespaceLinkedRefMap final : public LinkedRefMap<const NamespaceDef>
{
  public:
    void writeDeclaration(OutputList &ol,const DString &title,
            bool isConstantGroup=false, bool localName=false);
    bool declVisible(bool isContantGroup) const;
};

/** An abstract interface of a namespace symbol. */
class NamespaceDef : public Definition
{
  public:
    ABSTRACT_BASE_CLASS(NamespaceDef)

    // ---- getters
    virtual int numDocMembers() const = 0;
    virtual const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const = 0;
    virtual const LinkedRefMap<const Definition> &getUsedDefinitions() const = 0;
    virtual bool isConstantGroup() const = 0;
    virtual bool isModule()        const = 0;
    virtual bool isLibrary() const = 0;
    virtual bool isInline() const = 0;
    virtual bool isVisibleInHierarchy() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual bool subGrouping() const = 0;
    virtual MemberList *getMemberList(MemberListType lt) const = 0;
    virtual const MemberLists &getMemberLists() const = 0;
    virtual const MemberDef *getMemberByName(const DString &) const = 0;
    virtual int countVisibleMembers() const = 0;

    /*! Returns the user defined member groups */
    virtual const MemberGroupList &getMemberGroups() const = 0;

    /*! Returns the classes contained in this namespace */
    virtual ClassLinkedRefMap getClasses() const = 0;

    /*! Returns the Slice interfaces contained in this namespace */
    virtual ClassLinkedRefMap getInterfaces() const = 0;

    /*! Returns the Slice structs contained in this namespace */
    virtual ClassLinkedRefMap getStructs() const = 0;

    /*! Returns the Slice exceptions contained in this namespace */
    virtual ClassLinkedRefMap getExceptions() const = 0;

    /*! Returns the namespaces contained in this namespace */
    virtual NamespaceLinkedRefMap getNamespaces() const = 0;

    /*! Returns the concepts contained in this namespace */
    virtual ConceptLinkedRefMap getConcepts() const = 0;

    virtual DString title() const = 0;
    virtual DString compoundTypeString() const = 0;
};

class NamespaceDefMutable : public DefinitionMutable, public NamespaceDef
{
  public:
    ABSTRACT_BASE_CLASS(NamespaceDefMutable)

    // --- setters/actions
    virtual void setMetaData(const DString &m) = 0;
    virtual void insertUsedFile(FileDef *fd) = 0;
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeMemberPages(OutputList &ol) = 0;
    virtual void writeTagFile(TextStream &) = 0;
    virtual void insertClass(ClassDef *cd) = 0;
    virtual void insertConcept(ConceptDef *cd) = 0;
    virtual void insertNamespace(NamespaceDef *nd) = 0;
    virtual void insertMember(MemberDef *md) = 0;
    virtual void computeAnchors() = 0;
    virtual void countMembers() = 0;
    virtual void addMembersToMemberGroup() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void sortMemberLists() = 0;
    virtual void addListReferences() = 0;
    virtual void addRequirementReferences() = 0;
    virtual void setFileName(const DString &fn) = 0;
    virtual void combineUsingRelations(NamespaceDefSet &visitedNamespace) = 0;
    virtual void addUsingDirective(NamespaceDef *nd) = 0;
    virtual void addUsingDeclaration(const Definition *d) = 0;
    virtual void setInline(bool isInline) = 0;
};

/** Factory method to create new NamespaceDef instance */
std::unique_ptr<NamespaceDef> createNamespaceDef(const DString &defFileName,int defLine,size_t defColumn,
                 const DString &name,const DString &ref=DString(),
                 const DString &refFile=DString(),const DString &type=DString(),
                 bool isPublished=false);

/** Factory method to create an alias of an existing namespace. Used for inline namespaces. */
std::unique_ptr<NamespaceDef> createNamespaceDefAlias(const Definition *newScope,const NamespaceDef *nd);


// --- Cast functions

NamespaceDef            *toNamespaceDef(Definition *d);
NamespaceDef            *toNamespaceDef(DefinitionMutable *d);
const NamespaceDef      *toNamespaceDef(const Definition *d);
NamespaceDefMutable     *toNamespaceDefMutable(Definition *d);

// --- Helpers

void replaceNamespaceAliases(DString &name);
NamespaceDef *getResolvedNamespace(const DString &key);
inline NamespaceDefMutable *getResolvedNamespaceMutable(const DString &key)
{
  return toNamespaceDefMutable(getResolvedNamespace(key));
}
bool namespaceHasNestedNamespace(const NamespaceDef *nd);
bool namespaceHasNestedConcept(const NamespaceDef *nd);
bool namespaceHasNestedClass(const NamespaceDef *nd,bool filterClasses,ClassDef::CompoundType ct);

//------------------------------------------------------------------------

#endif
