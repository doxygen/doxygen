/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef CLASSDEF_H
#define CLASSDEF_H

#include <qstring.h>
#include <qfileinf.h>
#include <qlist.h>
#include <qdict.h>
#include <qtstream.h>
#include <qstrlist.h>

#include "membername.h"
#include "memberlist.h"
#include "memberdef.h"
#include "example.h"
#include "entry.h"
#include "definition.h"

typedef QDict<MemberDef> MemberDict;
class ClassList;
class OutputList;
class FileDef;
class BaseClassList;
class MemberInfoList;
class MemberInfoDict;
class NamespaceDef;

class ClassDef : public Definition
{
  public:
    
    enum CompoundType { Class=Entry::CLASS_SEC, 
                        Struct=Entry::STRUCT_SEC, 
                        Union=Entry::UNION_SEC 
                      };
    
    ClassDef(const char *name,int ct,const char *ref=0,const char *fName=0);
   ~ClassDef();
    QString classFile() const { return fileName; }
    int compoundType() const { return compType; } 
    const char *memberListFileName() const { return memListFileName; }
    void insertBaseClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    BaseClassList *baseClasses() { return inherits; }
    void insertSuperClass(ClassDef *,Protection p,Specifier s,const char *t=0);
    BaseClassList *superClasses() { return inheritedBy; }
    void setIncludeFile(FileDef *fd) { incFile=fd; }
    FileDef *includeFile() const { return incFile; }
    void setIncludeName(const char *n_) { incName=n_; }
    MemberNameInfoList *memberNameInfoList() { return allMemberNameInfoList; }
    MemberNameInfoDict *memberNameInfoDict() { return allMemberNameInfoDict; }
    bool isReference() { return !reference.isNull(); }
    const char *getReference() const { return reference; }
    void insertMember(const MemberDef *);
    void insertUsedFile(const char *);
    void computeAnchors();
    void setAnchor(MemberDef *);
    void dumpMembers();
    void writeDocumentation(OutputList &ol);
    void writeMemberList(OutputList &ol);
    void writeIncludeFile(OutputList &ol);
    bool addExample(const char *anchor,const char *name, const char *file);
    bool hasExamples();
    void writeExample(OutputList &ol);
    void setProtection(Protection p) { prot=p; }
    Protection protection() const { return prot; }
    bool isVisible() 
      { return !name().isEmpty() && name().at(0)!='@' &&
               (prot!=Private || extractPrivateFlag) &&
               hasDocumentation();
      }
    bool hasNonReferenceSuperClass();
    bool isVisibleExt() 
      { return (allExtFlag || hasNonReferenceSuperClass()) &&
               !name().isEmpty() && name().at(0)!='@' &&
               (prot!=Private || extractPrivateFlag) &&
               (hasDocumentation() || !hideClassFlag || !reference.isNull());
      }
    
    // template argument functions
    ArgumentList *templateArguments() const { return tempArgs; }
    void setTemplateArguments(ArgumentList *al);
    QString getTemplateNameString();
    void setNamespace(NamespaceDef *nd) { nspace = nd; }
    NamespaceDef *getNamespace() { return nspace; }
    
    bool visited;
   
  private: 
    //QString name;                       // name of the class
    QString fileName;                   // HTML containing the class docs
    //QString doc;                        // general class documentation
    FileDef *incFile;                   // header file to refer to
    QString incName;                    // alternative include file name
    //QString brief;                      // brief class discription
    QString memListFileName;            
    BaseClassList *inherits;
    BaseClassList *inheritedBy;
    NamespaceDef  *nspace;              // the namespace this class is in
    MemberList pubMembers;
    MemberList proMembers;
    MemberList priMembers;
    MemberList pubStaticMembers;
    MemberList proStaticMembers;
    MemberList priStaticMembers;
    MemberList pubSlots;
    MemberList proSlots;
    MemberList priSlots;
    MemberList related;
    MemberList signals;
    MemberList friends;
    MemberNameInfoList *allMemberNameInfoList;
    MemberNameInfoDict *allMemberNameInfoDict;
    ArgumentList     *tempArgs;
    QStrList          files;
    QString           reference;
    ExampleList      *exampleList;
    ExampleDict      *exampleDict;
    int               compType;
    Protection        prot;
};


struct BaseClassDef
{
  BaseClassDef(ClassDef *cd,Protection p,Specifier v,const char *t) : 
        classDef(cd), prot(p), virt(v), templSpecifiers(t) {}
  ClassDef *classDef;
  Protection prot;
  Specifier  virt;
  QString templSpecifiers;
};

class BaseClassList : public QList<BaseClassDef>
{
  public:
    int compareItems(GCI item1,GCI item2)
    {
      ClassDef *c1=((BaseClassDef *)item1)->classDef;
      ClassDef *c2=((BaseClassDef *)item2)->classDef;
      if (c1==0 || c2==0) 
        return FALSE;
      else
        return strcmp(c1->name(),c2->name());
    }
};

class BaseClassListIterator : public QListIterator<BaseClassDef>
{
  public:
    BaseClassListIterator(const BaseClassList &bcl) : 
      QListIterator<BaseClassDef>(bcl) {}
};

#endif
