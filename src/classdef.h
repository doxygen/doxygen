/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>
#include <qstrlist.h>

//#include "entry.h"
#include "memberlist.h"
#include "definition.h"
#include "sortdict.h"

class MemberDict;
class ClassList;
class ClassSDict;
class OutputList;
class FileDef;
class BaseClassList;
class NamespaceDef;
class MemberDef;
class ExampleSDict;
class MemberNameInfoSDict;
class UsesClassDict;
class MemberGroupSDict;
class QTextStream;
class PackageDef;
class GroupDef;
class StringDict;
struct IncludeInfo;
class ClassDefImpl;

/*! \brief This class contains all information about a compound.
 *
 *  A compound can be a class, struct, union, interface, or exception.
 *  \note This class should be renamed to CompoundDef
 */
class ClassDef : public Definition
{
  public:
    /*! The various compound types */
    enum CompoundType { Class, //=Entry::CLASS_SEC, 
                        Struct, //=Entry::STRUCT_SEC, 
                        Union, //=Entry::UNION_SEC,
                        Interface, //=Entry::INTERFACE_SEC,
                        Protocol, //=Entry::PROTOCOL_SEC,
                        Category, //=Entry::CATEGORY_SEC,
                        Exception //=Entry::EXCEPTION_SEC
                      };

    /*! Creates a new compound definition.
     *  \param fileName  full path and file name in which this compound was
     *                   found.
     *  \param startLine line number where the definition of this compound
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
     */
    ClassDef(const char *fileName,int startLine,
             const char *name,CompoundType ct,
             const char *ref=0,const char *fName=0,
             bool isSymbol=TRUE);
    /*! Destroys a compound definition. */
   ~ClassDef();

    //-----------------------------------------------------------------------------------
    // --- getters 
    //-----------------------------------------------------------------------------------

    /*! Used for RTTI, this is a class */
    DefType definitionType() const { return TypeClass; }

    /*! Returns the unique base name (without extension) of the class's file on disk */
    QCString getOutputFileBase() const; 
    QCString getInstanceOutputFileBase() const; 
    QCString getFileBase() const;

    /*! Returns the base name for the source code file */
    QCString getSourceFileBase() const; 

    /*! If this class originated from a tagfile, this will return the tag file reference */
    QCString getReference() const;

    /*! Returns TRUE if this class is imported via a tag file */
    bool isReference() const;

    /*! Returns TRUE if this is a local class definition, see EXTRACT_LOCAL_CLASSES */
    bool isLocal() const;

    /*! returns the classes nested into this class */
    ClassSDict *getInnerClasses();

    /*! returns TRUE if this class has documentation */
    bool hasDocumentation() const;

    /*! Returns the name as it is appears in the documentation */
    QCString displayName() const;

    /*! Returns the type of compound this is, i.e. class/struct/union/.. */
    CompoundType compoundType() const;

    /*! Returns the type of compound as a string */
    QCString compoundTypeString() const;

    /*! Returns the list of base classes from which this class directly
     *  inherits.
     */
    BaseClassList *baseClasses() const;
    
    /*! Returns the list of sub classes that directly derive from this class
     */
    BaseClassList *subClasses() const;

    /*! Returns a dictionary of all members. This includes any inherited 
     *  members. Members are sorted alphabetically.
     */ 
    MemberNameInfoSDict *memberNameInfoSDict() const;

    /*! Return the protection level (Public,Protected,Private) in which 
     *  this compound was found.
     */
    Protection protection() const;

    /*! returns TRUE iff a link is possible to this item within this project.
     */
    bool isLinkableInProject() const;

    /*! return TRUE iff a link to this class is possible (either within 
     *  this project, or as a cross-reference to another project).
     */
    bool isLinkable() const;

    /*! the class is visible in a class diagram, or class hierarchy */
    bool isVisibleInHierarchy();
    
    /*! Returns the template arguments of this class 
     *  Will return 0 if not applicable.
     */
    ArgumentList *templateArguments() const;

    /*! Returns the namespace this compound is in, or 0 if it has a global
     *  scope.
     */
    NamespaceDef *getNamespaceDef() const;

    /*! Returns the file in which this compound's definition can be found.
     *  Should not return 0 (but it might be a good idea to check anyway).
     */
    FileDef      *getFileDef() const;

    /*! Returns the Java package this class is in or 0 if not applicable. 
     */ 

    MemberDef    *getMemberByName(const QCString &) const;
    
    /*! Returns TRUE iff \a bcd is a direct or indirect base class of this
     *  class. This function will recusively traverse all branches of the
     *  inheritance tree.
     */
    bool isBaseClass(ClassDef *bcd,bool followInstances,int level=0);

    /*! returns TRUE iff \a md is a member of this class or of the
     *  the public/protected members of a base class 
     */
    bool isAccessibleMember(MemberDef *md);

    /*! Returns a sorted dictionary with all template instances found for
     *  this template class. Returns 0 if not a template or no instances.
     */
    QDict<ClassDef> *getTemplateInstances() const;

    /*! Returns the template master of which this class is an instance.
     *  Returns 0 if not applicable.
     */
    ClassDef *templateMaster() const;

    /*! Returns TRUE if this class is a template */
    bool isTemplate() const;

    IncludeInfo *includeInfo() const;
    
    UsesClassDict *usedImplementationClasses() const;

    UsesClassDict *usedByImplementationClasses() const;

    UsesClassDict *usedInterfaceClasses() const;

    bool isTemplateArgument() const;

    /*! Returns the definition of a nested compound if
     *  available, or 0 otherwise.
     *  @param name The name of the nested compound
     */
    virtual Definition *findInnerCompound(const char *name);

    /*! Returns the template parameter lists that form the template
     *  declaration of this class.
     *  
     *  Example: <code>template<class T> class TC {};</code>
     *  will return a list with one ArgumentList containing one argument
     *  with type="class" and name="T".
     */
    void getTemplateParameterLists(QList<ArgumentList> &lists) const;

    QCString qualifiedNameWithTemplateParameters(
        QList<ArgumentList> *actualParams=0) const;

    /*! Returns TRUE if there is at least one pure virtual member in this
     *  class.
     */
    bool isAbstract() const;

    /*! Returns TRUE if this class is implemented in Objective-C */
    bool isObjectiveC() const;

    /*! Returns the class of which this is a category (Objective-C only) */
    ClassDef *categoryOf() const;

    /*! Returns the name of the class including outer classes, but not
     *  including namespaces.
     */
    QCString className() const;

    /*! Returns the members in the list identified by \a lt */
    MemberList *getMemberList(MemberList::ListType lt);

    /*! Returns the list containing the list of members sorted per type */
    const QList<MemberList> &getMemberLists() const;

    /*! Returns the member groups defined for this class */
    MemberGroupSDict *getMemberGroupSDict() const;

    QDict<int> *getTemplateBaseClassNames() const;

    ClassDef *getVariableInstance(const char *templSpec);

    bool isUsedOnly() const;

    //-----------------------------------------------------------------------------------
    // --- setters ----
    //-----------------------------------------------------------------------------------

    void insertBaseClass(ClassDef *,const char *name,Protection p,Specifier s,const char *t=0);
    void insertSubClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    void setIncludeFile(FileDef *fd,const char *incName,bool local,bool force); 
    void insertMember(MemberDef *);
    void insertUsedFile(const char *);
    bool addExample(const char *anchor,const char *name, const char *file);
    void mergeCategory(ClassDef *category);
    void setNamespace(NamespaceDef *nd);
    void setFileDef(FileDef *fd);
    void setSubGrouping(bool enabled);
    void setProtection(Protection p);
    void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs);
    void addInnerCompound(Definition *d);
    ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,
                                const QCString &templSpec,bool &freshInstance);
    void addUsedClass(ClassDef *cd,const char *accessName);
    void addUsedByClass(ClassDef *cd,const char *accessName);
    void setIsStatic(bool b);
    void setIsObjectiveC(bool b);
    void setCompoundType(CompoundType t);
    void setClassName(const char *name);

    void setTemplateArguments(ArgumentList *al);
    void setTemplateBaseClassNames(QDict<int> *templateNames);
    void setTemplateMaster(ClassDef *tm);
    void setTypeConstraints(ArgumentList *al);
    void addMembersToTemplateInstance(ClassDef *cd,const char *templSpec);
    void makeTemplateArgument(bool b=TRUE);
    void setCategoryOf(ClassDef *cd);
    void setUsedOnly(bool b);

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    void findSectionsInDocumentation();
    void addMembersToMemberGroup();
    void addListReferences();
    void computeAnchors();
    void mergeMembers();
    void distributeMemberGroupDocumentation();
    void writeDocumentation(OutputList &ol);
    void writeClassDiagrams(OutputList &ol);
    void writeDocumentationForInnerClasses(OutputList &ol);
    void writeMemberDocumentation(OutputList &ol);
    void writeMemberPages(OutputList &ol);
    void writeMemberList(OutputList &ol);
    void writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup);
    void writeDetailedDescription(OutputList &ol,const QCString &pageType,bool exampleFlag);
    void writeQuickMemberLinks(OutputList &ol,MemberDef *md) const;
    
    bool visited;

  protected:
    void addUsedInterfaceClasses(MemberDef *md,const char *typeStr);
    bool hasExamples();
    bool hasNonReferenceSuperClass();
    void showUsedFiles(OutputList &ol);

  private: 
    void internalInsertMember(MemberDef *md,Protection prot,bool addToAllList);
    QCString getMemberListFileName() const;
    void addMemberToList(MemberList::ListType lt,MemberDef *md);
    MemberList *createMemberList(MemberList::ListType lt);
    void writeMemberDeclarations(OutputList &ol,MemberList::ListType lt,const QCString &title,
                                 const char *subTitle=0);
    void writeMemberDocumentation(OutputList &ol,MemberList::ListType lt,const QCString &title);
    void writePlainMemberDeclaration(OutputList &ol,MemberList::ListType lt,bool inGroup);
    
    ClassDefImpl *m_impl;

};

/*! \brief Class that contains information about a usage relation. 
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
  /*! Class definition that this relation uses. */
  ClassDef *classDef;

  /*! Dictionary of member variable names that form the edge labels of the
   *  usage relation.
   */
  QDict<void> *accessors;

  /*! Template arguments used for the base class */
  QCString templSpecifiers;

  bool containment;
};

/*! \brief Dictionary of usage relations. 
 */
class UsesClassDict : public QDict<UsesClassDef>
{
  public:
    UsesClassDict(int size) : QDict<UsesClassDef>(size) {}
   ~UsesClassDict() {}
};

/*! \brief Iterator class to iterate over a dictionary of usage relations. 
 */
class UsesClassDictIterator : public QDictIterator<UsesClassDef>
{
  public:
    UsesClassDictIterator(const QDict<UsesClassDef> &d) 
      : QDictIterator<UsesClassDef>(d) {}
   ~UsesClassDictIterator() {}
};

/*! \brief Class that contains information about an inheritance relation. 
 */
struct BaseClassDef
{
  BaseClassDef(ClassDef *cd,const char *n,Protection p,
               Specifier v,const char *t) : 
        classDef(cd), usedName(n), prot(p), virt(v), templSpecifiers(t) {}

  /*! Class definition that this relation inherits from. */
  ClassDef *classDef;

  /*! name used in the inheritance list 
   * (may be a typedef name instead of the class name)
   */
  QCString   usedName; 
  
  /*! Protection level of the inheritance relation: 
   *  Public, Protected, or Private 
   */
  Protection prot;     

  /*! Virtualness of the inheritance relation:
   *  Normal, or Virtual
   */
  Specifier  virt;

  /*! Template arguments used for the base class */
  QCString templSpecifiers;
};

/*! \brief list of base classes 
 *  
 *  The classes are alphabetically sorted on name if inSort() is used.
 */
class BaseClassList : public QList<BaseClassDef>
{
  public:
   ~BaseClassList() {}
    int compareItems(GCI item1,GCI item2)
    {
      ClassDef *c1=((BaseClassDef *)item1)->classDef;
      ClassDef *c2=((BaseClassDef *)item2)->classDef;
      if (c1==0 || c2==0) 
        return FALSE;
      else
        return stricmp(c1->name(),c2->name());
    }
};

/*! \brief Iterator for a list of base classes
 */
class BaseClassListIterator : public QListIterator<BaseClassDef>
{
  public:
    BaseClassListIterator(const BaseClassList &bcl) : 
      QListIterator<BaseClassDef>(bcl) {}
};

#endif
