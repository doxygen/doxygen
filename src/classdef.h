/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

class MemberDict;
class ClassList;
class OutputList;
class FileDef;
class BaseClassList;
class MemberInfoList;
class MemberInfoDict;
class NamespaceDef;
class MemberDef;
class ExampleList;
class MemberNameInfoList;
class MemberNameInfoDict;
class UsesClassDict;
class MemberGroupList;
class MemberGroupDict;
class QTextStream;
class PackageDef;
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
                        Exception=Entry::EXCEPTION_SEC
                      };
    DefType definitionType() { return TypeClass; }
    QCString getOutputFileBase() const; 
    QCString getFileBase() const;
    QCString getSourceFileBase() const; 

    bool hasDocumentation() const;

    /*! Returns the name as it is appears in the documentation */
    QCString displayName() const;

    /*! Returns the type of compound this is */
    CompoundType compoundType() const { return compType; } 

    /*! Returns the type of compound as a string */
    QCString compoundTypeString() const;

    /*! Returns the list of base classes from which this class directly
     *  inherits.
     */
    BaseClassList *baseClasses() { return inherits; }
    
    /*! Returns the list of sub classes that directly inherit from this class
     */
    BaseClassList *subClasses() { return inheritedBy; }

    /*! Returns a list of all members. This includes any inherited members.
     *  Members are sorted alphabetically.
     */ 
    MemberNameInfoList *memberNameInfoList() { return allMemberNameInfoList; }

    /*! Returns a dictionary of all members. This includes any inherited 
     *  members. Members are sorted alphabetically.
     */ 
    MemberNameInfoDict *memberNameInfoDict() { return allMemberNameInfoDict; }

    void writeDocumentation(OutputList &ol);
    void writeMemberList(OutputList &ol);
    void writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup);

    /*! Return the protection level (Public,Protected,Private) in which 
     *  this compound was found.
     */
    Protection protection() const { return prot; }

    /*! returns TRUE iff a link is possible to an item within this project.
     */
    bool isLinkableInProject();

    /*! return TRUE iff a link to this class is possible (either within 
     *  this project, or as a cross-reference to another project).
     */
    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }

    /*! the class is visible in a class diagram, or class hierarchy */
    bool isVisibleInHierarchy();
    
    /*! Returns the template arguments of this class 
     *  Will return 0 if not applicable.
     */
    ArgumentList *templateArguments() const { return tempArgs; }

    /*! Returns the template arguments that this nested class "inherits" 
     *  from its outer class (doxygen assumes there is only one!). 
     *  Will return 0 if not applicable.
     */
    ArgumentList *outerTemplateArguments() const;
    
    /*! Returns the namespace this compound is in, or 0 if it has a global
     *  scope.
     */
    NamespaceDef *getNamespaceDef() { return nspace; }

    /*! Returns the file in which this compound's definition can be found.
     *  Should not return 0 (but it might be a good idea to check anyway).
     */
    FileDef      *getFileDef() const { return fileDef; }

    /*! Returns the Java package this class is in or 0 if not applicable. 
     */ 
    PackageDef   *packageDef() const;
    
    /*! Returns TRUE iff \a bcd is a direct or indirect base class of this
     *  class. This function will recusively traverse all branches of the
     *  inheritance tree.
     */
    bool isBaseClass(ClassDef *bcd);

    UsesClassDict *usedImplementationClasses() const 
    { 
      return usesImplClassDict; 
    }

    UsesClassDict *usedInterfaceClasses() const
    {
      return usesIntfClassDict;
    }
   
    /* member lists by protection */
    MemberList pubMembers;
    MemberList proMembers;
    MemberList priMembers;
    MemberList pubStaticMembers;
    MemberList proStaticMembers;
    MemberList priStaticMembers;
    MemberList pubSlots;
    MemberList proSlots;
    MemberList priSlots;
    MemberList pubAttribs;
    MemberList proAttribs;
    MemberList priAttribs;
    MemberList pubStaticAttribs;
    MemberList proStaticAttribs;
    MemberList priStaticAttribs;
    MemberList pubTypes;
    MemberList proTypes;
    MemberList priTypes;
    MemberList related;
    MemberList signals;
    MemberList friends;
    MemberList dcopMethods;
    MemberList properties;
    
    /* member list by types */
    MemberList constructors;
    MemberList typedefMembers;
    MemberList enumMembers;
    MemberList enumValMembers;
    MemberList functionMembers;
    MemberList relatedMembers;
    MemberList variableMembers;
    MemberList propertyMembers;

    /*! \} */

    /*! \name Doxygen internal API
     *  \{
     */
    void insertBaseClass(ClassDef *,const char *name,Protection p,Specifier s,const char *t=0);
    void insertSubClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    void setIncludeFile(FileDef *fd,const char *incName,bool local); 
    void insertMember(MemberDef *);
    void insertUsedFile(const char *);
    void computeAnchors();
    void computeMemberGroups();
    void setAnchor(MemberDef *);
    void dumpMembers();
    bool addExample(const char *anchor,const char *name, const char *file);
    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void setNamespace(NamespaceDef *nd) { nspace = nd; }
    void setTemplateArguments(ArgumentList *al);
    void mergeMembers();
    void setFileDef(FileDef *fd) { fileDef=fd; }
    void determineImplUsageRelation();
    void determineIntfUsageRelation();
    void setSubGrouping(bool enabled) { subGrouping = enabled; }
    void setProtection(Protection p) { prot=p; }

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
     */
    ClassDef(const char *fileName,int startLine,
             const char *name,CompoundType ct,
             const char *ref=0,const char *fName=0);
    /*! Destroys a compound definition. */
   ~ClassDef();
    
    bool visited;

  protected:
    void addUsedInterfaceClasses(MemberDef *md,const char *typeStr);
    bool hasExamples();
    bool hasNonReferenceSuperClass();

    /*! \} */

  private: 
    QCString fileName;                   // HTML containing the class docs
    IncludeInfo *incInfo;                // header file to refer to
    QCString incName;                    // alternative include file name
    QCString memListFileName;            
    QCString scopelessName;              // name without any scopes
    BaseClassList *inherits;
    BaseClassList *inheritedBy;
    NamespaceDef  *nspace;              // the namespace this class is in

    /* user defined member groups */
    MemberGroupList    *memberGroupList;
    MemberGroupDict    *memberGroupDict;

    MemberNameInfoList *allMemberNameInfoList;
    MemberNameInfoDict *allMemberNameInfoDict;
    ArgumentList       *tempArgs;
    QStrList            files;
    ExampleList        *exampleList;
    ExampleDict        *exampleDict;
    CompoundType        compType;
    Protection          prot;
    FileDef            *fileDef;
    UsesClassDict      *usesImplClassDict;
    UsesClassDict      *usesIntfClassDict;

    bool subGrouping; // does this class group its user-grouped members
                      // as a sub-section of the normal (public/protected/..) 
                      // groups?
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
