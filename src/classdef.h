/******************************************************************************
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

#ifndef CLASSDEF_H
#define CLASSDEF_H

#include <memory>
#include <vector>
#include <unordered_set>

#include "containers.h"
#include "definition.h"
#include "arguments.h"
#include "membergroup.h"
#include "configvalues.h"

struct Argument;
class MemberDef;
class MemberDefMutable;
class MemberList;
class MemberLists;
class ClassLinkedRefMap;
class OutputList;
class FileDef;
class FileList;
class NamespaceDef;
class MemberDef;
class ExampleList;
class MemberNameInfoLinkedMap;
class GroupDef;
struct IncludeInfo;
class ClassDefImpl;
class ClassDef;
class ClassDefMutable;
class UsesClassList;
class ConstraintClassList;
class MemberGroupList;
class ModuleDef;

/** Class that contains information about an inheritance relation.
 */
struct BaseClassDef
{
  BaseClassDef(ClassDef *cd,const QCString &n,Protection p, Specifier v,const QCString &t) :
        classDef(cd), usedName(n), prot(p), virt(v), templSpecifiers(t) {}

  /** Class definition that this relation inherits from. */
  ClassDef *classDef;

  /** name used in the inheritance list
   * (may be a typedef name instead of the class name)
   */
  QCString   usedName;

  /** Protection level of the inheritance relation:
   *  Public, Protected, or Private
   */
  Protection prot;

  /** Virtualness of the inheritance relation:
   *  Normal, or Virtual
   */
  Specifier  virt;

  /** Template arguments used for the base class */
  QCString templSpecifiers;
};

using BaseClassList = std::vector<BaseClassDef>;

/** Class that contains information about a template instance relation */
struct TemplateInstanceDef
{
  TemplateInstanceDef(const QCString &ts,ClassDef *cd) : templSpec(ts), classDef(cd) {}
  QCString templSpec;
  ClassDef *classDef;
};

using TemplateInstanceList = std::vector<TemplateInstanceDef>;

using TemplateNameMap = std::map<std::string,int>;

using ClassDefSet = std::unordered_set<const ClassDef*>;


/** A abstract class representing of a compound symbol.
 *
 *  A compound can be a class, struct, union, interface, service, singleton,
 *  or exception.
 */
class ClassDef : public Definition
{
  public:
    ABSTRACT_BASE_CLASS(ClassDef)

    /** The various compound types */
    enum CompoundType { Class,     //=Entry::CLASS_SEC,
                        Struct,    //=Entry::STRUCT_SEC,
                        Union,     //=Entry::UNION_SEC,
                        Interface, //=Entry::INTERFACE_SEC,
                        Protocol,  //=Entry::PROTOCOL_SEC,
                        Category,  //=Entry::CATEGORY_SEC,
                        Exception, //=Entry::EXCEPTION_SEC
                        Service,   //=Entry::CLASS_SEC
                        Singleton, //=Entry::CLASS_SEC
                      };

    virtual std::unique_ptr<ClassDef> deepCopy(const QCString &name) const = 0;
    virtual void moveTo(Definition *) = 0;

    //-----------------------------------------------------------------------------------
    // --- getters
    //-----------------------------------------------------------------------------------

    virtual QCString getInstanceOutputFileBase() const = 0;

    /** Returns TRUE if this is a local class definition, see EXTRACT_LOCAL_CLASSES */
    virtual bool isLocal() const = 0;

    /** returns the classes nested into this class */
    virtual ClassLinkedRefMap getClasses() const = 0;

    /** returns TRUE if this class has a non-empty detailed description */
    virtual bool hasDetailedDescription() const = 0;

    /** returns the file name to use for the collaboration graph */
    virtual QCString collaborationGraphFileName() const = 0;

    /** returns the file name to use for the inheritance graph */
    virtual QCString inheritanceGraphFileName() const = 0;

    /** Returns the type of compound this is, i.e. class/struct/union/.. */
    virtual CompoundType compoundType() const = 0;

    /** Returns the type of compound as a string */
    virtual QCString compoundTypeString() const = 0;

    /** Returns the list of base classes from which this class directly
     *  inherits.
     */
    virtual const BaseClassList &baseClasses() const = 0;

    /** Update the list of base classes to the one passed */
    virtual void updateBaseClasses(const BaseClassList &bcd) = 0;

    /** Returns the list of sub classes that directly derive from this class
     */
    virtual const BaseClassList &subClasses() const = 0;

    /** Update the list of sub classes to the one passed */
    virtual void updateSubClasses(const BaseClassList &bcd) = 0;

    /** Returns a dictionary of all members. This includes any inherited
     *  members. Members are sorted alphabetically.
     */
    virtual const MemberNameInfoLinkedMap &memberNameInfoLinkedMap() const = 0;

    /** Return the protection level (Public,Protected,Private) in which
     *  this compound was found.
     */
    virtual Protection protection() const = 0;

    /** the class is visible in a class diagram, or class hierarchy */
    virtual bool isVisibleInHierarchy() const = 0;

    /** show this class in the declaration section of its parent? */
    virtual bool visibleInParentsDeclList() const = 0;

    /** Returns the template arguments of this class
     */
    virtual const ArgumentList &templateArguments() const = 0;

    /** Returns the namespace this compound is in, or 0 if it has a global
     *  scope.
     */
    //virtual NamespaceDef *getNamespaceDef() const = 0;

    /** Returns the file in which this compound's definition can be found.
     *  Should not return 0 (but it might be a good idea to check anyway).
     */
    virtual FileDef      *getFileDef() const = 0;

    /** Returns the C++20 module in which this compound's definition can be found.
     */
    virtual ModuleDef    *getModuleDef() const = 0;

    /** Returns the member with the given name */
    virtual const MemberDef *getMemberByName(const QCString &) const = 0;

    /** Returns TRUE iff \a bcd is a direct or indirect base class of this
     *  class. This function will recursively traverse all branches of the
     *  inheritance tree.
     */
    virtual int isBaseClass(const ClassDef *bcd,bool followInstances,const QCString &templSpec=QCString()) const = 0;

    /** Returns TRUE iff \a bcd is a direct or indirect sub class of this
     *  class.
     */
    virtual bool isSubClass(ClassDef *bcd,int level=0) const = 0;

    /** returns TRUE iff \a md is a member of this class or of the
     *  the public/protected members of a base class
     */
    virtual bool isAccessibleMember(const MemberDef *md) const = 0;

    /** Returns a sorted dictionary with all template instances found for
     *  this template class. Returns 0 if not a template or no instances.
     */
    virtual const TemplateInstanceList &getTemplateInstances() const = 0;

    /** Returns the template master of which this class is an instance.
     *  Returns 0 if not applicable.
     */
    virtual const ClassDef *templateMaster() const = 0;

    /** Returns TRUE if this class is a template */
    virtual bool isTemplate() const = 0;

    virtual const IncludeInfo *includeInfo() const = 0;

    virtual const UsesClassList &usedImplementationClasses() const = 0;

    virtual const UsesClassList &usedByImplementationClasses() const = 0;

    virtual const ConstraintClassList &templateTypeConstraints() const = 0;

    virtual bool isTemplateArgument() const = 0;

    /** Returns the template parameter lists that form the template
     *  declaration of this class.
     *
     *  Example: <code>template<class T> class TC {} = 0;</code>
     *  will return a list with one ArgumentList containing one argument
     *  with type="class" and name="T".
     */
    virtual ArgumentLists getTemplateParameterLists() const = 0;

    virtual QCString qualifiedNameWithTemplateParameters(
        const ArgumentLists *actualParams=nullptr,uint32_t *actualParamIndex=nullptr) const = 0;

    /** Returns TRUE if there is at least one pure virtual member in this
     *  class.
     */
    virtual bool isAbstract() const = 0;

    /** Returns TRUE if this class is implemented in Objective-C */
    virtual bool isObjectiveC() const = 0;

    /** Returns TRUE if this class is implemented in Fortran */
    virtual bool isFortran() const = 0;

    /** Returns TRUE if this class is implemented in C# */
    virtual bool isCSharp() const = 0;

    /** Returns TRUE if this class is marked as final */
    virtual bool isFinal() const = 0;

    /** Returns TRUE if this class is marked as sealed */
    virtual bool isSealed() const = 0;

    /** Returns TRUE if this class is marked as published */
    virtual bool isPublished() const = 0;

    /** Returns TRUE if this class represents an Objective-C 2.0 extension (nameless category) */
    virtual bool isExtension() const = 0;

    /** Returns TRUE if this class represents a forward declaration of a template class */
    virtual bool isForwardDeclared() const = 0;

    /** Returns TRUE if this class represents an interface */
    virtual bool isInterface() const = 0;

    /** Returns the class of which this is a category (Objective-C only) */
    virtual ClassDef *categoryOf() const = 0;

    /** Returns the name of the class including outer classes, but not
     *  including namespaces.
     */
    virtual QCString className() const = 0;

    /** Returns the members in the list identified by \a lt */
    virtual MemberList *getMemberList(MemberListType lt) const = 0;

    /** Returns the list containing the list of members sorted per type */
    virtual const MemberLists &getMemberLists() const = 0;

    /** Returns the member groups defined for this class */
    virtual const MemberGroupList &getMemberGroups() const = 0;

    virtual const TemplateNameMap &getTemplateBaseClassNames() const = 0;

    virtual bool isUsedOnly() const = 0;

    virtual bool isEmbeddedInOuterScope() const = 0;

    virtual bool isSimple() const = 0;

    virtual const ClassDef *tagLessReference() const = 0;

    virtual const MemberDef *isSmartPointer() const = 0;

    virtual bool isJavaEnum() const = 0;

    virtual QCString title() const = 0;

    virtual QCString generatedFromFiles() const = 0;
    virtual const FileList &usedFiles() const = 0;

    virtual const ArgumentList &typeConstraints() const = 0;
    virtual const ExampleList &getExamples() const = 0;
    virtual bool hasExamples() const = 0;
    virtual QCString getMemberListFileName() const = 0;
    virtual bool subGrouping() const = 0;

    virtual bool isSliceLocal() const = 0;
    virtual bool hasNonReferenceSuperClass() const = 0;

    virtual QCString requiresClause() const = 0;
    virtual StringVector getQualifiers() const = 0;

    virtual bool containsOverload(const MemberDef *md) const = 0;

    virtual bool isImplicitTemplateInstance() const = 0;

    //-----------------------------------------------------------------------------------
    // --- count members ----
    //-----------------------------------------------------------------------------------

    virtual int countMembersIncludingGrouped(MemberListType lt,
                const ClassDef *inheritedFrom,bool additional) const = 0;
    virtual int countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                MemberListType lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const = 0;

    //-----------------------------------------------------------------------------------
    // --- write output ----
    //-----------------------------------------------------------------------------------

    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                 const QCString &header,bool localNames) const = 0;
    virtual void writeDocumentation(OutputList &ol) const = 0;
    virtual void writeDocumentationForInnerClasses(OutputList &ol) const = 0;
    virtual void writeMemberPages(OutputList &ol) const = 0;
    virtual void writeMemberList(OutputList &ol) const = 0;
    virtual void writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,
                 int indentLevel, const ClassDef *inheritedFrom,const QCString &inheritId) const = 0;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const = 0;
    virtual void writeSummaryLinks(OutputList &ol) const = 0;
    virtual void writePageNavigation(OutputList &ol) const = 0;
    virtual void writeInlineDocumentation(OutputList &ol) const = 0;
    virtual void writeTagFile(TextStream &) const = 0;
    virtual void writeMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                 MemberListType lt,const QCString &title,
                 const QCString &subTitle=QCString(),
                 bool showInline=FALSE,const ClassDef *inheritedFrom=nullptr,
                 MemberListType lt2=MemberListType::Invalid(),bool invert=FALSE,bool showAlways=FALSE) const = 0;
    virtual void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                 const ClassDef *inheritedFrom,const QCString &inheritId) const = 0;
};

class ClassDefMutable : public DefinitionMutable, public ClassDef
{
  public:
    ABSTRACT_BASE_CLASS(ClassDefMutable)

    //-----------------------------------------------------------------------------------
    // --- setters ----
    //-----------------------------------------------------------------------------------

    virtual void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force) = 0;
    virtual void setFileDef(FileDef *fd) = 0;
    virtual void setModuleDef(ModuleDef *md) = 0;
    virtual void setSubGrouping(bool enabled) = 0;
    virtual void setProtection(Protection p) = 0;
    virtual void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs) = 0;
    virtual void setIsStatic(bool b) = 0;
    virtual void setCompoundType(CompoundType t) = 0;
    virtual void setClassName(const QCString &name) = 0;
    virtual void setClassSpecifier(TypeSpecifier spec) = 0;
    virtual void setTemplateArguments(const ArgumentList &al) = 0;
    virtual void setTemplateBaseClassNames(const TemplateNameMap &templateNames) = 0;
    virtual void setTemplateMaster(const ClassDef *tm) = 0;
    virtual void setTypeConstraints(const ArgumentList &al) = 0;
    virtual void setCategoryOf(ClassDef *cd) = 0;
    virtual void setUsedOnly(bool b) = 0;
    virtual void setTagLessReference(const ClassDef *cd) = 0;
    virtual void setMetaData(const QCString &md) = 0;
    virtual void setRequiresClause(const QCString &req) = 0;
    virtual void addQualifiers(const StringVector &qualifiers) = 0;
        // inheritance graph related members
    virtual CLASS_GRAPH_t hasInheritanceGraph() const = 0;
    virtual void overrideInheritanceGraph(CLASS_GRAPH_t e) = 0;
    virtual void setImplicitTemplateInstance(bool b) = 0;

    // collaboration graph related members
    virtual bool hasCollaborationGraph() const = 0;
    virtual void overrideCollaborationGraph(bool e) = 0;

    //-----------------------------------------------------------------------------------
    // --- helpers ----
    //-----------------------------------------------------------------------------------

    virtual ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                const QCString &templSpec,bool &freshInstance) = 0;

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    virtual void insertBaseClass(ClassDef *,const QCString &name,Protection p,Specifier s,const QCString &t=QCString()) = 0;
    virtual void insertSubClass(ClassDef *,Protection p,Specifier s,const QCString &t=QCString()) = 0;
    virtual void insertExplicitTemplateInstance(ClassDef *instance,const QCString &spec) = 0;
    virtual void insertMember(MemberDef *) = 0;
    virtual void insertUsedFile(const FileDef *) = 0;
    virtual void addMemberToTemplateInstance(const MemberDef *md, const ArgumentList &templateArguments, const QCString &templSpec) = 0;
    virtual void addMembersToTemplateInstance(const ClassDef *cd,const ArgumentList &templateArguments,const QCString &templSpec) = 0;
    virtual bool addExample(const QCString &anchor,const QCString &name, const QCString &file) = 0;
    virtual void addUsedClass(ClassDef *cd,const QCString &accessName,Protection prot) = 0;
    virtual void addUsedByClass(ClassDef *cd,const QCString &accessName,Protection prot) = 0;
    virtual void makeTemplateArgument(bool b=TRUE) = 0;
    virtual void mergeCategory(ClassDef *category) = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void addMembersToMemberGroup() = 0;
    virtual void addListReferences() = 0;
    virtual void addRequirementReferences() = 0;
    virtual void addTypeConstraints() = 0;
    virtual void computeAnchors() = 0;
    virtual void mergeMembers() = 0;
    virtual void sortMemberLists() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void reclassifyMember(MemberDefMutable *md,MemberType t) = 0;
    virtual void removeMemberFromLists(MemberDef *md) = 0;
    virtual void setAnonymousEnumType() = 0;
    virtual void countMembers() = 0;
    virtual void sortAllMembersList() = 0;
    virtual void setPrimaryConstructorParams(const ArgumentList &list) = 0;


};

/** Factory method to create a new ClassDef object */
std::unique_ptr<ClassDef> createClassDef(
             const QCString &fileName,int startLine,int startColumn,
             const QCString &name,ClassDef::CompoundType ct,
             const QCString &ref=QCString(),const QCString &fName=QCString(),
             bool isSymbol=TRUE,bool isJavaEnum=FALSE);

std::unique_ptr<ClassDef> createClassDefAlias(const Definition *newScope,const ClassDef *cd);

// --- Cast functions

ClassDef            *toClassDef(Definition *d);
ClassDef            *toClassDef(DefinitionMutable *d);
const ClassDef      *toClassDef(const Definition *d);
ClassDefMutable     *toClassDefMutable(Definition *d);

// --- Helpers
//
ClassDef *getClass(const QCString &key);
inline ClassDefMutable *getClassMutable(const QCString &key)
{
  return toClassDefMutable(getClass(key));
}
bool classHasVisibleRoot(const BaseClassList &bcl);
bool classHasVisibleChildren(const ClassDef *cd);
bool classVisibleInIndex(const ClassDef *cd);
int minClassDistance(const ClassDef *cd,const ClassDef *bcd,int level=0);
Protection classInheritedProtectionLevel(const ClassDef *cd,const ClassDef *bcd,Protection prot=Protection::Public,int level=0);

//------------------------------------------------------------------------

/** Class that contains information about a usage relation.
 */
struct UsesClassDef
{
  UsesClassDef(ClassDef *cd) : classDef(cd) {}
  void addAccessor(const QCString &s)
  {
    if (accessors.find(s.str())==accessors.end())
    {
      accessors.insert(s.str());
    }
  }
  /** Class definition that this relation uses. */
  ClassDef *classDef;

  /** Dictionary of member variable names that form the edge labels of the
   *  usage relation.
   */
  StringSet accessors;

  /** Template arguments used for the base class */
  QCString templSpecifiers;

  bool containment = true;
};

class UsesClassList : public std::vector<UsesClassDef>
{
};

//------------------------------------------------------------------------

/** Class that contains information about a type constraint relations.
 */
struct ConstraintClassDef
{
  ConstraintClassDef(ClassDef *cd) : classDef(cd) {}
  void addAccessor(const QCString &s)
  {
    if (accessors.find(s.str())==accessors.end())
    {
      accessors.insert(s.str());
    }
  }
  /** Class definition that this relation uses. */
  ClassDef *classDef;

  /** Dictionary of member types names that form the edge labels of the
   *  constraint relation.
   */
  StringSet accessors;
};

class ConstraintClassList : public std::vector<ConstraintClassDef>
{
};

//------------------------------------------------------------------------

#endif
