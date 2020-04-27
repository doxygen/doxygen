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

#ifndef CLASSDEF_H
#define CLASSDEF_H

#include <vector>

#include <qlist.h>
#include <qdict.h>
#include <qptrdict.h>

#include "definition.h"

struct Argument;
class MemberDef;
class MemberList;
class MemberDict;
class ClassList;
class ClassSDict;
class OutputList;
class FileDef;
class FileList;
class BaseClassList;
class NamespaceDef;
class MemberDef;
class ExampleSDict;
class MemberNameInfoLinkedMap;
class UsesClassDict;
class ConstraintClassDict;
class MemberGroupSDict;
class QTextStream;
class PackageDef;
class GroupDef;
class StringDict;
struct IncludeInfo;
class ClassDefImpl;
class ArgumentList;
class FTextStream;

/** A abstract class representing of a compound symbol.
 *
 *  A compound can be a class, struct, union, interface, service, singleton,
 *  or exception.
 */
class ClassDef : virtual public Definition
{
  public:
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

    virtual ~ClassDef() {}


    //-----------------------------------------------------------------------------------
    // --- getters
    //-----------------------------------------------------------------------------------

    /** Used for RTTI, this is a class */
    virtual DefType definitionType() const = 0;

    /** Returns the unique base name (without extension) of the class's file on disk */
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString getInstanceOutputFileBase() const = 0;

    /** Returns the base name for the source code file */
    virtual QCString getSourceFileBase() const = 0;

    /** If this class originated from a tagfile, this will return the tag file reference */
    virtual QCString getReference() const = 0;

    /** Returns TRUE if this class is imported via a tag file */
    virtual bool isReference() const = 0;

    /** Returns TRUE if this is a local class definition, see EXTRACT_LOCAL_CLASSES */
    virtual bool isLocal() const = 0;

    /** returns the classes nested into this class */
    virtual ClassSDict *getClassSDict() const = 0;

    /** returns TRUE if this class has documentation */
    virtual bool hasDocumentation() const = 0;

    /** returns TRUE if this class has a non-empty detailed description */
    virtual bool hasDetailedDescription() const = 0;

    /** returns the file name to use for the collaboration graph */
    virtual QCString collaborationGraphFileName() const = 0;

    /** returns the file name to use for the inheritance graph */
    virtual QCString inheritanceGraphFileName() const = 0;

    /** Returns the name as it is appears in the documentation */
    virtual QCString displayName(bool includeScope=TRUE) const = 0;

    /** Returns the type of compound this is, i.e. class/struct/union/.. */
    virtual CompoundType compoundType() const = 0;

    /** Returns the type of compound as a string */
    virtual QCString compoundTypeString() const = 0;

    /** Returns the list of base classes from which this class directly
     *  inherits.
     */
    virtual BaseClassList *baseClasses() const = 0;

    /** Returns the list of sub classes that directly derive from this class
     */
    virtual BaseClassList *subClasses() const = 0;

    /** Returns a dictionary of all members. This includes any inherited
     *  members. Members are sorted alphabetically.
     */
    virtual const MemberNameInfoLinkedMap &memberNameInfoLinkedMap() const = 0;

    /** Return the protection level (Public,Protected,Private) in which
     *  this compound was found.
     */
    virtual Protection protection() const = 0;

    /** returns TRUE iff a link is possible to this item within this project.
     */
    virtual bool isLinkableInProject() const = 0;

    /** return TRUE iff a link to this class is possible (either within
     *  this project, or as a cross-reference to another project).
     */
    virtual bool isLinkable() const = 0;

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
    virtual NamespaceDef *getNamespaceDef() const = 0;

    /** Returns the file in which this compound's definition can be found.
     *  Should not return 0 (but it might be a good idea to check anyway).
     */
    virtual FileDef      *getFileDef() const = 0;

    /** Returns the Java package this class is in or 0 if not applicable.
     */

    virtual MemberDef    *getMemberByName(const QCString &) const = 0;

    /** Returns TRUE iff \a bcd is a direct or indirect base class of this
     *  class. This function will recursively traverse all branches of the
     *  inheritance tree.
     */
    virtual bool isBaseClass(const ClassDef *bcd,bool followInstances,int level=0) const = 0;

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
    virtual QDict<ClassDef> *getTemplateInstances() const = 0;

    /** Returns the template master of which this class is an instance.
     *  Returns 0 if not applicable.
     */
    virtual const ClassDef *templateMaster() const = 0;

    /** Returns TRUE if this class is a template */
    virtual bool isTemplate() const = 0;

    virtual IncludeInfo *includeInfo() const = 0;

    virtual UsesClassDict *usedImplementationClasses() const = 0;

    virtual UsesClassDict *usedByImplementationClasses() const = 0;

    virtual UsesClassDict *usedInterfaceClasses() const = 0;

    virtual ConstraintClassDict *templateTypeConstraints() const = 0;

    virtual bool isTemplateArgument() const = 0;

    /** Returns the definition of a nested compound if
     *  available, or 0 otherwise.
     *  @param name The name of the nested compound
     */
    virtual Definition *findInnerCompound(const char *name) const = 0;

    /** Returns the template parameter lists that form the template
     *  declaration of this class.
     *
     *  Example: <code>template<class T> class TC {} = 0;</code>
     *  will return a list with one ArgumentList containing one argument
     *  with type="class" and name="T".
     */
    virtual std::vector<ArgumentList> getTemplateParameterLists() const = 0;

    virtual QCString qualifiedNameWithTemplateParameters(
        const std::vector<ArgumentList> *actualParams=0,uint *actualParamIndex=0) const = 0;

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
    virtual const QList<MemberList> &getMemberLists() const = 0;

    /** Returns the member groups defined for this class */
    virtual MemberGroupSDict *getMemberGroupSDict() const = 0;

    virtual QDict<int> *getTemplateBaseClassNames() const = 0;

    virtual ClassDef *getVariableInstance(const char *templSpec) const = 0;

    virtual bool isUsedOnly() const = 0;

    virtual QCString anchor() const = 0;
    virtual bool isEmbeddedInOuterScope() const = 0;

    virtual bool isSimple() const = 0;

    virtual const ClassList *taggedInnerClasses() const = 0;
    virtual ClassDef *tagLessReference() const = 0;

    virtual MemberDef *isSmartPointer() const = 0;

    virtual bool isJavaEnum() const = 0;

    virtual bool isGeneric() const = 0;
    virtual const ClassSDict *innerClasses() const = 0;
    virtual QCString title() const = 0;

    virtual QCString generatedFromFiles() const = 0;
    virtual const FileList &usedFiles() const = 0;

    virtual const ArgumentList &typeConstraints() const = 0;
    virtual const ExampleSDict *exampleList() const = 0;
    virtual bool hasExamples() const = 0;
    virtual QCString getMemberListFileName() const = 0;
    virtual bool subGrouping() const = 0;

    virtual bool isSliceLocal() const = 0;
    virtual bool hasNonReferenceSuperClass() const = 0;

    //-----------------------------------------------------------------------------------
    // --- setters ----
    //-----------------------------------------------------------------------------------

    virtual void setIncludeFile(FileDef *fd,const char *incName,bool local,bool force) = 0;
    virtual void setNamespace(NamespaceDef *nd) = 0;
    virtual void setFileDef(FileDef *fd) = 0;
    virtual void setSubGrouping(bool enabled) = 0;
    virtual void setProtection(Protection p) = 0;
    virtual void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs) = 0;
    virtual void setIsStatic(bool b) = 0;
    virtual void setCompoundType(CompoundType t) = 0;
    virtual void setClassName(const char *name) = 0;
    virtual void setClassSpecifier(uint64 spec) = 0;
    virtual void setTemplateArguments(const ArgumentList &al) = 0;
    virtual void setTemplateBaseClassNames(QDict<int> *templateNames) = 0;
    virtual void setTemplateMaster(const ClassDef *tm) = 0;
    virtual void setTypeConstraints(const ArgumentList &al) = 0;
    virtual void setCategoryOf(ClassDef *cd) = 0;
    virtual void setUsedOnly(bool b) = 0;
    virtual void setTagLessReference(ClassDef *cd) = 0;
    virtual void setName(const char *name) = 0;
    virtual void setMetaData(const char *md) = 0;

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    virtual void insertBaseClass(ClassDef *,const char *name,Protection p,Specifier s,const char *t=0) = 0;
    virtual void insertSubClass(ClassDef *,Protection p,Specifier s,const char *t=0) = 0;
    virtual void insertMember(MemberDef *) = 0;
    virtual void insertUsedFile(FileDef *) = 0;
    virtual void addMembersToTemplateInstance(const ClassDef *cd,const char *templSpec) = 0;
    virtual void addTaggedInnerClass(ClassDef *cd) = 0;
    virtual void addInnerCompound(const Definition *d) = 0;
    virtual bool addExample(const char *anchor,const char *name, const char *file) = 0;
    virtual ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                const QCString &templSpec,bool &freshInstance) const = 0;
    virtual void addUsedClass(ClassDef *cd,const char *accessName,Protection prot) = 0;
    virtual void addUsedByClass(ClassDef *cd,const char *accessName,Protection prot) = 0;
    virtual void makeTemplateArgument(bool b=TRUE) = 0;
    virtual void mergeCategory(ClassDef *category) = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void addMembersToMemberGroup() = 0;
    virtual void addListReferences() = 0;
    virtual void addTypeConstraints() = 0;
    virtual void computeAnchors() = 0;
    virtual void mergeMembers() = 0;
    virtual void sortMemberLists() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void reclassifyMember(MemberDef *md,MemberType t) = 0;
    virtual void removeMemberFromLists(MemberDef *md) = 0;
    virtual void setAnonymousEnumType() = 0;
    virtual void countMembers() = 0;
    virtual void sortAllMembersList() = 0;

    //-----------------------------------------------------------------------------------
    // --- write output ----
    //-----------------------------------------------------------------------------------

    virtual void writeDocumentation(OutputList &ol) const = 0;
    virtual void writeDocumentationForInnerClasses(OutputList &ol) const = 0;
    virtual void writeMemberPages(OutputList &ol) const = 0;
    virtual void writeMemberList(OutputList &ol) const = 0;
    virtual void writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,
                 const ClassDef *inheritedFrom,const char *inheritId) const = 0;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const = 0;
    virtual void writeSummaryLinks(OutputList &ol) const = 0;
    virtual void writeInlineDocumentation(OutputList &ol) const = 0;
    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                 const char *header,bool localNames) const = 0;
    virtual void writeTagFile(FTextStream &) = 0;
    virtual void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title,
                 const char *subTitle=0,bool showInline=FALSE,const ClassDef *inheritedFrom=0,
                 int lt2=-1,bool invert=FALSE,bool showAlways=FALSE,
                 QPtrDict<void> *visitedClasses=0) const = 0;
    virtual void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                 const ClassDef *inheritedFrom,const QCString &inheritId) const = 0;

    //-----------------------------------------------------------------------------------
    // --- count members ----
    //-----------------------------------------------------------------------------------

    virtual int countMembersIncludingGrouped(MemberListType lt,
                const ClassDef *inheritedFrom,bool additional) const = 0;
    virtual int countInheritanceNodes() const = 0;
    virtual int countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                int lt2,bool invert,bool showAlways,QPtrDict<void> *visitedClasses) const = 0;


    //-----------------------------------------------------------------------------------
    // --- visiting administration ----
    //-----------------------------------------------------------------------------------

    virtual void setVisited(bool visited) const = 0;
    virtual bool isVisited() const = 0;
};

/** Factory method to create a new ClassDef object */
ClassDef *createClassDef(
             const char *fileName,int startLine,int startColumn,
             const char *name,ClassDef::CompoundType ct,
             const char *ref=0,const char *fName=0,
             bool isSymbol=TRUE,bool isJavaEnum=FALSE);

ClassDef *createClassDefAlias(const Definition *newScope,const ClassDef *cd);


//------------------------------------------------------------------------

/** Class that contains information about a usage relation.
 */
struct UsesClassDef
{
  UsesClassDef(ClassDef *cd) : classDef(cd)
  {
    accessors = new QDict<void>(17);
    containment = TRUE;
  }
 ~UsesClassDef()
  {
    delete accessors;
  }
  void addAccessor(const char *s)
  {
    if (accessors->find(s)==0)
    {
      accessors->insert(s,(void *)666);
    }
  }
  /** Class definition that this relation uses. */
  ClassDef *classDef;

  /** Dictionary of member variable names that form the edge labels of the
   *  usage relation.
   */
  QDict<void> *accessors;

  /** Template arguments used for the base class */
  QCString templSpecifiers;

  bool containment;
};

/** Dictionary of usage relations.
 */
class UsesClassDict : public QDict<UsesClassDef>
{
  public:
    UsesClassDict(uint size) : QDict<UsesClassDef>(size) {}
   ~UsesClassDict() {}
};

/** Iterator class to iterate over a dictionary of usage relations.
 */
class UsesClassDictIterator : public QDictIterator<UsesClassDef>
{
  public:
    UsesClassDictIterator(const QDict<UsesClassDef> &d)
      : QDictIterator<UsesClassDef>(d) {}
   ~UsesClassDictIterator() {}
};

//------------------------------------------------------------------------

/** Class that contains information about an inheritance relation.
 */
struct BaseClassDef
{
  BaseClassDef(ClassDef *cd,const char *n,Protection p,
               Specifier v,const char *t) :
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

/** List of base classes.
 *
 *  The classes are alphabetically sorted on name if inSort() is used.
 */
class BaseClassList : public QList<BaseClassDef>
{
  public:
   ~BaseClassList() {}
    int compareValues(const BaseClassDef *item1,const BaseClassDef *item2) const
    {
      const ClassDef *c1=item1->classDef;
      const ClassDef *c2=item2->classDef;
      if (c1==0 || c2==0)
        return FALSE;
      else
        return qstricmp(c1->name(),c2->name());
    }
};

/** Iterator for a list of base classes.
 */
class BaseClassListIterator : public QListIterator<BaseClassDef>
{
  public:
    BaseClassListIterator(const BaseClassList &bcl) :
      QListIterator<BaseClassDef>(bcl) {}
};

//------------------------------------------------------------------------


/** Class that contains information about a type constraint relations.
 */
struct ConstraintClassDef
{
  ConstraintClassDef(ClassDef *cd) : classDef(cd)
  {
    accessors = new QDict<void>(17);
  }
 ~ConstraintClassDef()
  {
    delete accessors;
  }
  void addAccessor(const char *s)
  {
    if (accessors->find(s)==0)
    {
      accessors->insert(s,(void *)666);
    }
  }
  /** Class definition that this relation uses. */
  ClassDef *classDef;

  /** Dictionary of member types names that form the edge labels of the
   *  constraint relation.
   */
  QDict<void> *accessors;
};

/** Dictionary of constraint relations.
 */
class ConstraintClassDict : public QDict<ConstraintClassDef>
{
  public:
    ConstraintClassDict(uint size) : QDict<ConstraintClassDef>(size) {}
   ~ConstraintClassDict() {}
};

/** Iterator class to iterate over a dictionary of constraint relations.
 */
class ConstraintClassDictIterator : public QDictIterator<ConstraintClassDef>
{
  public:
    ConstraintClassDictIterator(const QDict<ConstraintClassDef> &d)
      : QDictIterator<ConstraintClassDef>(d) {}
   ~ConstraintClassDictIterator() {}
};

//------------------------------------------------------------------------

#endif
