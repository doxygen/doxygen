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
 *  A compound can be a class, struct, union, interface, or exception 
 */
class ClassDef : public Definition
{
  public:
    enum CompoundType { Class=Entry::CLASS_SEC, 
                        Struct=Entry::STRUCT_SEC, 
                        Union=Entry::UNION_SEC,
                        Interface=Entry::INTERFACE_SEC,
                        Exception=Entry::EXCEPTION_SEC
                      };
    ClassDef(const char *fileName,int startLine,
             const char *name,CompoundType ct,
             const char *ref=0,const char *fName=0);
   ~ClassDef();
    DefType definitionType() { return TypeClass; }
    QCString getOutputFileBase() const { return fileName; }
    QCString displayName() const;
    CompoundType compoundType() const { return compType; } 
    QCString compoundTypeString() const;
    void insertBaseClass(ClassDef *,const char *name,Protection p,Specifier s,const char *t=0);
    BaseClassList *baseClasses() { return inherits; }
    void insertSuperClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    BaseClassList *superClasses() { return inheritedBy; }
    void setIncludeFile(FileDef *fd,const char *incName,bool local); 
    MemberNameInfoList *memberNameInfoList() { return allMemberNameInfoList; }
    MemberNameInfoDict *memberNameInfoDict() { return allMemberNameInfoDict; }
    void insertMember(MemberDef *);
    void insertUsedFile(const char *);
    void computeAnchors();
    void computeMemberGroups();
    void setAnchor(MemberDef *);
    void dumpMembers();
    void writeDocumentation(OutputList &ol);
    void writeMemberList(OutputList &ol);
    void writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup);
    bool addExample(const char *anchor,const char *name, const char *file);
    bool hasExamples();
    void setProtection(Protection p) { prot=p; }
    Protection protection() const { return prot; }
    bool isLinkableInProject();
    /*! a link to this class is possible (either within this project,
     *  or as a cross-reference to another project
     */
    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }
    bool hasNonReferenceSuperClass();
    /*! the class is visible in a class diagram, or class hierarchy */
    bool isVisibleInHierarchy();
    
    // template argument functions
    ArgumentList *templateArguments() const { return tempArgs; }
    ArgumentList *outerTemplateArguments() const;
    void setTemplateArguments(ArgumentList *al);
    //QCString getTemplateNameString();
    
    NamespaceDef *getNamespaceDef() { return nspace; }
    FileDef *getFileDef() const { return fileDef; }
    
    void setNamespace(NamespaceDef *nd) { nspace = nd; }
    void setFileDef(FileDef *fd) { fileDef=fd; }
    void mergeMembers();
    bool isBaseClass(ClassDef *bcd);
    void determineImplUsageRelation();
    void determineIntfUsageRelation();
    UsesClassDict *usedImplementationClasses() const 
    { 
      return usesImplClassDict; 
    }
    UsesClassDict *usedInterfaceClasses() const
    {
      return usesIntfClassDict;
    }
    void setSubGrouping(bool enabled) { subGrouping = enabled; }
    
    bool visited;

    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();

    //void generateXML(QTextStream &t);
    //void generateXMLSection(QTextStream &t,MemberList *ml,const char *type);

    PackageDef *packageDef() const;
   
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

  protected:
    void addUsedInterfaceClasses(MemberDef *md,const char *typeStr);
    void addMemberListToGroup(MemberList *);

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
