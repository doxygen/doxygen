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
class MemberNameInfoSDict;
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

/** A class representing of a compound symbol.
 *
 *  A compound can be a class, struct, union, interface, service, singleton,
 *  or exception.
 *  \note This class should be renamed to CompoundDef
 */
class ClassDef : public Definition
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

    /** Creates a new compound definition.
     *  \param fileName  full path and file name in which this compound was
     *                   found.
     *  \param startLine line number where the definition of this compound
     *                   starts.
     *  \param startColumn column number where the definition of this compound
     *                   starts.
     *  \param name      the name of this compound (including scope)
     *  \param ct        the kind of Compound
     *  \param ref       the tag file from which this compound is extracted
     *                   or 0 if the compound doesn't come from a tag file
     *  \param fName     the file name as found in the tag file. 
     *                   This overwrites the file that doxygen normally 
     *                   generates based on the compound type & name.
     *  \param isSymbol  If TRUE this class name is added as a publicly 
     *                   visible (and referencable) symbol.
     *  \param isJavaEnum If TRUE this class is actually a Java enum.
     *                    I didn't add this to CompoundType to avoid having
     *                    to adapt all translators.
     */
    ClassDef(const char *fileName,int startLine,int startColumn,
             const char *name,CompoundType ct,
             const char *ref=0,const char *fName=0,
             bool isSymbol=TRUE,bool isJavaEnum=FALSE);
    /** Destroys a compound definition. */
   ~ClassDef();

    //-----------------------------------------------------------------------------------
    // --- getters 
    //-----------------------------------------------------------------------------------

    /** Used for RTTI, this is a class */
    DefType definitionType() const { return TypeClass; }

    /** Returns the unique base name (without extension) of the class's file on disk */
    QCString getOutputFileBase() const;
    QCString getInstanceOutputFileBase() const; 

    /** Returns the base name for the source code file */
    QCString getSourceFileBase() const; 

    /** If this class originated from a tagfile, this will return the tag file reference */
    QCString getReference() const;

    /** Returns TRUE if this class is imported via a tag file */
    bool isReference() const;

    /** Returns TRUE if this is a local class definition, see EXTRACT_LOCAL_CLASSES */
    bool isLocal() const;

    /** returns the classes nested into this class */
    ClassSDict *getClassSDict();

    /** returns TRUE if this class has documentation */
    bool hasDocumentation() const;

    /** returns TRUE if this class has a non-empty detailed description */
    bool hasDetailedDescription() const;
    
    /** returns the file name to use for the collaboration graph */
    QCString collaborationGraphFileName() const;

    /** returns the file name to use for the inheritance graph */
    QCString inheritanceGraphFileName() const;

    /** Returns the name as it is appears in the documentation */
    QCString displayName(bool includeScope=TRUE) const;

    /** Returns the type of compound this is, i.e. class/struct/union/.. */
    CompoundType compoundType() const;

    /** Returns the type of compound as a string */
    QCString compoundTypeString() const;

    /** Returns the list of base classes from which this class directly
     *  inherits.
     */
    BaseClassList *baseClasses() const;
    
    /** Returns the list of sub classes that directly derive from this class
     */
    BaseClassList *subClasses() const;

    /** Returns a dictionary of all members. This includes any inherited 
     *  members. Members are sorted alphabetically.
     */ 
    MemberNameInfoSDict *memberNameInfoSDict() const;

    /** Return the protection level (Public,Protected,Private) in which 
     *  this compound was found.
     */
    Protection protection() const;

    /** returns TRUE iff a link is possible to this item within this project.
     */
    bool isLinkableInProject() const;

    /** return TRUE iff a link to this class is possible (either within 
     *  this project, or as a cross-reference to another project).
     */
    bool isLinkable() const;

    /** the class is visible in a class diagram, or class hierarchy */
    bool isVisibleInHierarchy();
    
    /** show this class in the declaration section of its parent? */
    bool visibleInParentsDeclList() const;

    /** Returns the template arguments of this class 
     *  Will return 0 if not applicable.
     */
    ArgumentList *templateArguments() const;

    /** Returns the namespace this compound is in, or 0 if it has a global
     *  scope.
     */
    NamespaceDef *getNamespaceDef() const;

    /** Returns the file in which this compound's definition can be found.
     *  Should not return 0 (but it might be a good idea to check anyway).
     */
    FileDef      *getFileDef() const;

    /** Returns the Java package this class is in or 0 if not applicable. 
     */ 

    MemberDef    *getMemberByName(const QCString &) const;
    
    /** Returns TRUE iff \a bcd is a direct or indirect base class of this
     *  class. This function will recusively traverse all branches of the
     *  inheritance tree.
     */
    bool isBaseClass(ClassDef *bcd,bool followInstances,int level=0);

    /** Returns TRUE iff \a bcd is a direct or indirect sub class of this
     *  class.
     */
    bool isSubClass(ClassDef *bcd,int level=0);

    /** returns TRUE iff \a md is a member of this class or of the
     *  the public/protected members of a base class 
     */
    bool isAccessibleMember(MemberDef *md);

    /** Returns a sorted dictionary with all template instances found for
     *  this template class. Returns 0 if not a template or no instances.
     */
    QDict<ClassDef> *getTemplateInstances() const;

    /** Returns the template master of which this class is an instance.
     *  Returns 0 if not applicable.
     */
    ClassDef *templateMaster() const;

    /** Returns TRUE if this class is a template */
    bool isTemplate() const;

    IncludeInfo *includeInfo() const;
    
    UsesClassDict *usedImplementationClasses() const;

    UsesClassDict *usedByImplementationClasses() const;

    UsesClassDict *usedInterfaceClasses() const;

    ConstraintClassDict *templateTypeConstraints() const;

    bool isTemplateArgument() const;

    /** Returns the definition of a nested compound if
     *  available, or 0 otherwise.
     *  @param name The name of the nested compound
     */
    virtual Definition *findInnerCompound(const char *name);

    /** Returns the template parameter lists that form the template
     *  declaration of this class.
     *  
     *  Example: <code>template<class T> class TC {};</code>
     *  will return a list with one ArgumentList containing one argument
     *  with type="class" and name="T".
     */
    void getTemplateParameterLists(QList<ArgumentList> &lists) const;

    QCString qualifiedNameWithTemplateParameters(
        QList<ArgumentList> *actualParams=0,int *actualParamIndex=0) const;

    /** Returns TRUE if there is at least one pure virtual member in this
     *  class.
     */
    bool isAbstract() const;

    /** Returns TRUE if this class is implemented in Objective-C */
    bool isObjectiveC() const;

    /** Returns TRUE if this class is implemented in C# */
    bool isCSharp() const;

    /** Returns TRUE if this class is marked as final */
    bool isFinal() const;

    /** Returns TRUE if this class is marked as sealed */
    bool isSealed() const;

    /** Returns TRUE if this class is marked as published */
    bool isPublished() const;

    /** Returns TRUE if this class represents an Objective-C 2.0 extension (nameless category) */
    bool isExtension() const;

    /** Returns TRUE if this class represents a forward declaration of a template class */
    bool isForwardDeclared() const;

    /** Returns the class of which this is a category (Objective-C only) */
    ClassDef *categoryOf() const;

    /** Returns the name of the class including outer classes, but not
     *  including namespaces.
     */
    QCString className() const;

    /** Returns the members in the list identified by \a lt */
    MemberList *getMemberList(MemberListType lt);

    /** Returns the list containing the list of members sorted per type */
    const QList<MemberList> &getMemberLists() const;

    /** Returns the member groups defined for this class */
    MemberGroupSDict *getMemberGroupSDict() const;

    QDict<int> *getTemplateBaseClassNames() const;

    ClassDef *getVariableInstance(const char *templSpec);

    bool isUsedOnly() const;

    QCString anchor() const;
    bool isEmbeddedInOuterScope() const;

    bool isSimple() const;

    const ClassList *taggedInnerClasses() const;
    ClassDef *tagLessReference() const;

    MemberDef *isSmartPointer() const;

    bool isJavaEnum() const;

    bool isGeneric() const;
    bool isAnonymous() const;
    const ClassSDict *innerClasses() const;
    QCString title() const;

    QCString generatedFromFiles() const;
    const FileList &usedFiles() const;

    const ArgumentList *typeConstraints() const;
    const ExampleSDict *exampleList() const;
    bool hasExamples() const;
    QCString getMemberListFileName() const;
    bool subGrouping() const;


    //-----------------------------------------------------------------------------------
    // --- setters ----
    //-----------------------------------------------------------------------------------

    void insertBaseClass(ClassDef *,const char *name,Protection p,Specifier s,const char *t=0);
    void insertSubClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    void setIncludeFile(FileDef *fd,const char *incName,bool local,bool force); 
    void insertMember(MemberDef *);
    void insertUsedFile(FileDef *);
    bool addExample(const char *anchor,const char *name, const char *file);
    void mergeCategory(ClassDef *category);
    void setNamespace(NamespaceDef *nd);
    void setFileDef(FileDef *fd);
    void setSubGrouping(bool enabled);
    void setProtection(Protection p);
    void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs);
    void addInnerCompound(Definition *d);
    ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                const QCString &templSpec,bool &freshInstance);
    void addUsedClass(ClassDef *cd,const char *accessName,Protection prot);
    void addUsedByClass(ClassDef *cd,const char *accessName,Protection prot);
    void setIsStatic(bool b);
    void setCompoundType(CompoundType t);
    void setClassName(const char *name);
    void setClassSpecifier(uint64 spec);

    void setTemplateArguments(ArgumentList *al);
    void setTemplateBaseClassNames(QDict<int> *templateNames);
    void setTemplateMaster(ClassDef *tm);
    void setTypeConstraints(ArgumentList *al);
    void addMembersToTemplateInstance(ClassDef *cd,const char *templSpec);
    void makeTemplateArgument(bool b=TRUE);
    void setCategoryOf(ClassDef *cd);
    void setUsedOnly(bool b);

    void addTaggedInnerClass(ClassDef *cd);
    void setTagLessReference(ClassDef *cd);
    void setName(const char *name);

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    void findSectionsInDocumentation();
    void addMembersToMemberGroup();
    void addListReferences();
    void addTypeConstraints();
    void computeAnchors();
    void mergeMembers();
    void sortMemberLists();
    void distributeMemberGroupDocumentation();
    void writeDocumentation(OutputList &ol);
    void writeDocumentationForInnerClasses(OutputList &ol);
    void writeMemberPages(OutputList &ol);
    void writeMemberList(OutputList &ol);
    void writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup,
                          ClassDef *inheritedFrom,const char *inheritId);
    void writeQuickMemberLinks(OutputList &ol,MemberDef *md) const;
    void writeSummaryLinks(OutputList &ol);
    void reclassifyMember(MemberDef *md,MemberType t);
    void writeInlineDocumentation(OutputList &ol);
    void writeDeclarationLink(OutputList &ol,bool &found,
                              const char *header,bool localNames);
    void removeMemberFromLists(MemberDef *md);
    void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                              ClassDef *inheritedFrom,const QCString &inheritId);
    int countMembersIncludingGrouped(MemberListType lt,ClassDef *inheritedFrom,bool additional);
    int countInheritanceNodes();
    void writeTagFile(FTextStream &);
    
    bool visited;

  protected:
    void addUsedInterfaceClasses(MemberDef *md,const char *typeStr);
    bool hasNonReferenceSuperClass();
    void showUsedFiles(OutputList &ol);

  private: 
    void writeDocumentationContents(OutputList &ol,const QCString &pageTitle);
    void internalInsertMember(MemberDef *md,Protection prot,bool addToAllList);
    void addMemberToList(MemberListType lt,MemberDef *md,bool isBrief);
    MemberList *createMemberList(MemberListType lt);
    void writeInheritedMemberDeclarations(OutputList &ol,MemberListType lt,int lt2,const QCString &title,ClassDef *inheritedFrom,bool invert,bool showAlways,QPtrDict<void> *visitedClasses);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title,
                                 const char *subTitle=0,bool showInline=FALSE,ClassDef *inheritedFrom=0,int lt2=-1,bool invert=FALSE,bool showAlways=FALSE,QPtrDict<void> *visitedClasses=0);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title,bool showInline=FALSE);
    void writeSimpleMemberDocumentation(OutputList &ol,MemberListType lt);
    void writePlainMemberDeclaration(OutputList &ol,MemberListType lt,bool inGroup,ClassDef *inheritedFrom,const char *inheritId);
    void writeBriefDescription(OutputList &ol,bool exampleFlag);
    void writeDetailedDescription(OutputList &ol,const QCString &pageType,bool exampleFlag,
                                  const QCString &title,const QCString &anchor=QCString());
    void writeIncludeFiles(OutputList &ol);
    //void writeAllMembersLink(OutputList &ol);
    void writeInheritanceGraph(OutputList &ol);
    void writeCollaborationGraph(OutputList &ol);
    void writeMemberGroups(OutputList &ol,bool showInline=FALSE);
    void writeNestedClasses(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void writeMoreLink(OutputList &ol,const QCString &anchor);
    void writeDetailedDocumentationBody(OutputList &ol);
    
    int countAdditionalInheritedMembers();
    void writeAdditionalInheritedMembers(OutputList &ol);
    void addClassAttributes(OutputList &ol);
    int countMemberDeclarations(MemberListType lt,ClassDef *inheritedFrom,
                                int lt2,bool invert,bool showAlways,QPtrDict<void> *visitedClasses);
    int countInheritedDecMembers(MemberListType lt,
                                 ClassDef *inheritedFrom,bool invert,bool showAlways,
                                 QPtrDict<void> *visitedClasses);
    void getTitleForMemberListType(MemberListType type,
               QCString &title,QCString &subtitle);
    QCString includeStatement() const;
    void addTypeConstraint(const QCString &typeConstraint,const QCString &type);

    ClassDefImpl *m_impl;
};

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
    UsesClassDict(int size) : QDict<UsesClassDef>(size) {}
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
    ConstraintClassDict(int size) : QDict<ConstraintClassDef>(size) {}
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
