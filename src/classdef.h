/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#include "entry.h"
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

/*! \brief This class contains all information about a compound.
 *
 *  A compound can be a class, struct, union, interface, or exception.
 *  \note This class should be renamed to CompoundDef
 */
class ClassDef : public Definition
{
  public:
    /*! \name Public API
     *  \{
     */

    /*! The various compound types */
    enum CompoundType { Class=Entry::CLASS_SEC, 
                        Struct=Entry::STRUCT_SEC, 
                        Union=Entry::UNION_SEC,
                        Interface=Entry::INTERFACE_SEC,
                        Protocol=Entry::PROTOCOL_SEC,
                        Category=Entry::CATEGORY_SEC,
                        Exception=Entry::EXCEPTION_SEC
                      };
    DefType definitionType() { return TypeClass; }
    QCString getOutputFileBase() const; 
    QCString getInstanceOutputFileBase() const; 
    QCString getFileBase() const;
    QCString getSourceFileBase() const; 
    QCString getReference() const;
    bool isReference() const;
    bool isLocal() const { return m_isLocal; }

    bool hasDocumentation() const;

    /*! Returns the name as it is appears in the documentation */
    QCString displayName() const;

    /*! Returns the type of compound this is */
    CompoundType compoundType() const { return m_compType; } 

    /*! Returns the type of compound as a string */
    QCString compoundTypeString() const;

    /*! Returns the list of base classes from which this class directly
     *  inherits.
     */
    BaseClassList *baseClasses() { return m_inherits; }
    
    /*! Returns the list of sub classes that directly inherit from this class
     */
    BaseClassList *subClasses() { return m_inheritedBy; }

    /*! Returns a dictionary of all members. This includes any inherited 
     *  members. Members are sorted alphabetically.
     */ 
    MemberNameInfoSDict *memberNameInfoSDict() { return m_allMemberNameInfoSDict; }

    void writeDocumentation(OutputList &ol);
    void writeDocumentationForInnerClasses(OutputList &ol);
    void writeMemberList(OutputList &ol);
    void writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup);
    void writeDetailedDescription(OutputList &ol,const QCString &pageType,bool exampleFlag);

    /*! Return the protection level (Public,Protected,Private) in which 
     *  this compound was found.
     */
    Protection protection() const { return m_prot; }

    /*! returns TRUE iff a link is possible to an item within this project.
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
    ArgumentList *templateArguments() const { return m_tempArgs; }

    /*! Returns the namespace this compound is in, or 0 if it has a global
     *  scope.
     */
    NamespaceDef *getNamespaceDef() { return m_nspace; }

    /*! Returns the file in which this compound's definition can be found.
     *  Should not return 0 (but it might be a good idea to check anyway).
     */
    FileDef      *getFileDef() const { return m_fileDef; }

    /*! Returns the Java package this class is in or 0 if not applicable. 
     */ 
    //PackageDef   *packageDef() const;

    MemberDef    *getMemberByName(const QCString &);
    
    /*! Returns TRUE iff \a bcd is a direct or indirect base class of this
     *  class. This function will recusively traverse all branches of the
     *  inheritance tree.
     */
    bool isBaseClass(ClassDef *bcd,bool followInstances,int level=0);

    /*! Returns a sorted dictionary with all template instances found for
     *  this template class. Returns 0 if not a template or no instances.
     */
    QDict<ClassDef> *getTemplateInstances() const { return m_templateInstances; }
    /*! Returns the template master of which this class is an instance.
     *  Returns 0 if not applicable.
     */
    ClassDef *templateMaster() const { return m_templateMaster; } 

    IncludeInfo *includeInfo() const { return m_incInfo; }
    
    UsesClassDict *usedImplementationClasses() const 
    { 
      return m_usesImplClassDict; 
    }

    UsesClassDict *usedByImplementationClasses() const 
    { 
      return m_usedByImplClassDict; 
    }

    UsesClassDict *usedInterfaceClasses() const
    {
      return m_usesIntfClassDict;
    }

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
    bool isAbstract() const { return m_isAbstract; }

    /*! Returns TRUE if this class is implemented in Objective-C */
    bool isObjectiveC() const { return m_isObjC; }

    ClassDef *categoryOf() const { return m_categoryOf; }

    /*! returns the name of the class including outer classes, but not
     *  including namespaces.
     */
    QCString className() const;

    /* member lists by protection */
    MemberList pubMethods;
    MemberList proMethods;
    MemberList pacMethods;
    MemberList priMethods;
    MemberList pubStaticMethods;
    MemberList proStaticMethods;
    MemberList pacStaticMethods;
    MemberList priStaticMethods;
    MemberList pubSlots;
    MemberList proSlots;
    MemberList priSlots;
    MemberList pubAttribs;
    MemberList proAttribs;
    MemberList pacAttribs;
    MemberList priAttribs;
    MemberList pubStaticAttribs;
    MemberList proStaticAttribs;
    MemberList pacStaticAttribs;
    MemberList priStaticAttribs;
    MemberList pubTypes;
    MemberList proTypes;
    MemberList pacTypes;
    MemberList priTypes;
    MemberList related;
    MemberList signals;
    MemberList friends;
    MemberList dcopMethods;
    MemberList properties;
    MemberList events;
    
    /* member list by types */
    MemberList constructors;
    MemberList typedefMembers;
    MemberList enumMembers;
    MemberList enumValMembers;
    MemberList functionMembers;
    MemberList relatedMembers;
    MemberList variableMembers;
    MemberList propertyMembers;
    MemberList eventMembers;

    /* user defined member groups */
    MemberGroupSDict *memberGroupSDict;

    /*! \} Public API */

    /*! \name Doxygen internal API
     *  \{
     */
    void insertBaseClass(ClassDef *,const char *name,Protection p,Specifier s,const char *t=0);
    void insertSubClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    void setIncludeFile(FileDef *fd,const char *incName,bool local,bool force); 
    void insertMember(MemberDef *);
    void insertUsedFile(const char *);
    void computeAnchors();
    //void computeMemberGroups();
    //void setAnchor(MemberDef *);
    //void dumpMembers();
    bool addExample(const char *anchor,const char *name, const char *file);
    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    void setNamespace(NamespaceDef *nd) { m_nspace = nd; }
    void setTemplateArguments(ArgumentList *al);
    void mergeMembers();
    void mergeCategory(ClassDef *category);
    void setFileDef(FileDef *fd) { m_fileDef=fd; }
    //void determineImplUsageRelation();
    //void determineIntfUsageRelation();
    void setSubGrouping(bool enabled) { m_subGrouping = enabled; }
    void setProtection(Protection p) { m_prot=p; }
    void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs);
    void addInnerCompound(Definition *d);
    void addUsedClass(ClassDef *cd,const char *accessName);
    void addUsedByClass(ClassDef *cd,const char *accessName);
    //void initTemplateMapping();
    //void setTemplateArgumentMapping(const char *formal,const char *actual);
    //QCString getTemplateArgumentMapping(const char *formal) const;
    ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,
                                const QCString &templSpec,bool &freshInstance);
    ClassDef *getVariableInstance(const char *templSpec);
    void setTemplateBaseClassNames(QDict<int> *templateNames);
    QDict<int> *getTemplateBaseClassNames() const;
    void setTemplateMaster(ClassDef *tm) { m_templateMaster=tm; }
    void addMembersToTemplateInstance(ClassDef *cd,const char *templSpec);
    void setClassIsArtificial() { m_artificial = TRUE; }
    void setIsStatic(bool b) { m_isStatic=b; }
    void setIsObjectiveC(bool b) { m_isObjC=b; }
    void addListReferences();

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

    ClassSDict *getInnerClasses() { return m_innerClasses; }
    
    bool visited;

  protected:
    void addUsedInterfaceClasses(MemberDef *md,const char *typeStr);
    bool hasExamples();
    bool hasNonReferenceSuperClass();

    /*! \} Interal API */

  private: 
    void internalInsertMember(MemberDef *md,Protection prot,bool addToAllList);
    
    /*! file name that forms the base for the output file containing the
     *  class documentation. For compatibility with Qt (e.g. links via tag 
     *  files) this name cannot be derived from the class name directly.
     */
    QCString m_fileName;                   

    /*! Include information about the header file should be included
     *  in the documentation. 0 by default, set by setIncludeFile().
     */
    IncludeInfo *m_incInfo;                

    /*! file name that forms the base for the "list of members" for this
     *  class.
     */
    QCString m_memListFileName;            

    /*! Bare name of the class without any scoping prefixes 
     *  (like for nested classes and classes inside namespaces)
     */ 
    //QCString m_scopelessName;              

    /*! List of base class (or super-classes) from which this class derives
     *  directly. 
     */
    BaseClassList *m_inherits;

    /*! List of sub-classes that directly derive from this class 
     */
    BaseClassList *m_inheritedBy;

    /*! Namespace this class is part of 
     *  (this is the inner most namespace in case of nested namespaces)
     */
    NamespaceDef  *m_nspace;              

    /*! File this class is defined in */
    FileDef *m_fileDef;

    /*! List of all members (including inherited members) */
    MemberNameInfoSDict *m_allMemberNameInfoSDict;

    /*! Template arguments of this class */
    ArgumentList *m_tempArgs;

    /*! Files that were used for generating the class documentation. */
    QStrList m_files;

    /*! Examples that use this class */
    ExampleSDict *m_exampleSDict;

    /*! Holds the kind of "class" this is. */
    CompoundType m_compType;

    /*! The protection level in which this class was found. 
     *  Typically Public, but for nested classes this can also be Protected
     *  or Private.
     */ 
    Protection m_prot;

    /*! Does this class group its user-grouped members
     *  as a sub-section of the normal (public/protected/..) 
     *  groups?
     */
    bool m_subGrouping; 

    /*! The inner classes contained in this class. Will be 0 if there are
     *  no inner classes.
     */
    ClassSDict *m_innerClasses;


    /* classes for the collaboration diagram */
    UsesClassDict *m_usesImplClassDict;
    UsesClassDict *m_usedByImplClassDict;
    UsesClassDict *m_usesIntfClassDict;

    /*! Template instances that exists of this class, the key in the
     *  dictionary is the template argument list.
     */ 
    QDict<ClassDef> *m_templateInstances;

    /*! Template instances that exists of this class, as defined by variables.
     *  We do NOT want to document these individually. The key in the
     *  dictionary is the template argument list.
     */ 
    QDict<ClassDef> *m_variableInstances;

    QDict<int> *m_templBaseClassNames;

    /*! The class this class is an instance of. */
    ClassDef *m_templateMaster;

    /*! Indicated whether this class exists because it is used by
     *  some other class only (TRUE) or if some class inherits from
     *  it (FALSE). This is need to remove used-only classes from
     *  the inheritance tree.
     */
    bool m_artificial;

    /*! Is this an abstact class? */
    bool m_isAbstract;

    /*! Is the class part of an unnamed namespace? */
    bool m_isStatic;

    /*! Is the class part implemented in Objective C? */
    bool m_isObjC;

    /*! TRUE if classes members are merged with those of the base classes. */
    bool m_membersMerged;

    /*! TRUE if the class is defined in a source file rather than a header file. */
    bool m_isLocal;

    /*! class name with outer class scope, but without namespace scope. */
    QCString m_className;

    /*! If this class is a Objective-C category, then this points to the
     *  class which is extended.
     */
    ClassDef *m_categoryOf;
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
