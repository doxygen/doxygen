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

#ifndef MEMBERDEF_H
#define MEMBERDEF_H

#include <qstring.h>
#include <qlist.h>
#include <qdict.h>

#include "entry.h"
#include "example.h"
#include "config.h"
#include "outputlist.h"
#include "definition.h"
#include "config.h"

class FileDef;
class MemberList;
class NamespaceDef;

class MemberDef : public Definition
{
  public:
    
    enum MemberType { 
      Define,
      Function, 
      Variable, 
      Typedef, 
      Enumeration, 
      EnumValue,
      Prototype,
      Signal,
      Slot,
      Friend
    };
    
    MemberDef(const char *type,const char *name,const char *args,
              const char *excp,Protection prot,Specifier virt,bool stat,
              bool related,MemberType t,const ArgumentList *tal,
              const ArgumentList *al);
   ~MemberDef(); 
    
    const char *declaration() const      { return decl; }
    const char *definition() const       { return def; }
    const char *typeString() const       { return type; }
    const char *argsString() const       { return args; }
    const char *excpString() const       { return exception; }     
    const char *anchor() const           { return ref; }
    ClassDef *memberClass()              { return classDef; }
    Protection protection() const        { return prot; }
    Specifier virtualness() const        { return virt; }
    MemberType memberType() const        { return mtype; }
    void setMemberType(MemberType t)     { mtype=t; }
    void setDefinition(const char *d)    { def=d; }
    void setDefFile(const char *f)       { defFile=f; }
    void setDefLine(int l)               { defLine=l; }
    void setFileDef(FileDef *fd)         { fileDef=fd; }
    void setFileDec(FileDef *fd)         { fileDec=fd; }
    void setAnchor(const char *a)        { ref=a; }
    FileDef *getFileDef()                { return fileDef; }
    FileDef *getFileDec()                { return fileDec; }
    void setMemberClass(ClassDef *cd)    { classDef=cd; }
    bool isRelated() const               { return related; }
    bool isStatic() const                { return stat; }
    bool detailsAreVisible() const          
      { return !documentation().isEmpty() || 
               (mtype==Enumeration && docEnumValues) || 
               (mtype==EnumValue && !briefDescription().isEmpty()) ||
               (!briefDescription().isEmpty() && 
                !briefMemDescFlag && repeatBriefFlag);
      }
    
    // relation to other members
    void setReimplements(MemberDef *md) { redefines=md; }
    void insertReimplementedBy(MemberDef *md);
    MemberDef  *reimplements()    { return redefines; }
    MemberList *reimplementedBy() { return redefinedBy; }
    
    // enumeration specific members
    void insertEnumField(MemberDef *md);
    void setEnumScope(MemberDef *md) { enumScope=md; }
    MemberDef *getEnumScope() const { return enumScope; }
    void setEnumDecl(OutputList &ed) { enumDeclList=new OutputList(&ed); }
    void setEnumUsed() { eUsed=TRUE; }
    bool enumUsed() const        { return eUsed; }
    OutputList *enumDecl() const { return enumDeclList; }
    MemberList *enumFieldList()  { return enumFields; }
    void setDocumentedEnumValues(bool value) { docEnumValues=value; }
    bool hasDocumentedEnumValues() const { return docEnumValues; }
    
    // example related members
    bool addExample(const char *anchor,const char *name,const char *file);
    void writeExample(OutputList &ol);
    bool hasExamples();

    // prototype related members
    const char *getDefFile()        { return defFile; }
    int getDefLine()                { return defLine; }
    void setDecFile(const char *f)  { declFile=f; }
    void setDecLine(int l)          { defLine=l; }
    
    // convenience members
    bool isSignal() const    { return mtype==Signal;      }
    bool isSlot() const      { return mtype==Slot;        }
    bool isVariable() const  { return mtype==Variable;    }
    bool isEnumerate() const { return mtype==Enumeration; }
    bool isEnumValue() const { return mtype==EnumValue;   }
    bool isTypedef() const   { return mtype==Typedef;     }
    bool isFunction() const  { return mtype==Function;    } 
    bool isDefine() const    { return mtype==Define;      }
    bool isFriend() const    { return mtype==Friend;      }
    
    // prototype related members
    void setPrototype(bool p) { proto=p; }
    bool isPrototype() const { return proto; }
    
    // tag file related members
    void setReference(const char *r) { external=r; } 
    bool isReference() { return !external.isNull(); }
    
    // argument related members
    ArgumentList *argumentList() const { return argList; }
    ArgumentList *templateArguments() const { return tArgList; }
    
    // namespace related members
    NamespaceDef *getNamespace() { return nspace; }
    void setNamespace(NamespaceDef *nd) { nspace=nd; }
    
  private:
    ClassDef   *classDef;     // member of or related to 
    FileDef    *fileDef;      // member of file definition 
    FileDef    *fileDec;      // member of file declaration
    MemberDef  *redefines;    // the member that is redefined by this one
    MemberDef  *enumScope;    // the enclosing scope, if this is an enum field
    MemberList *redefinedBy;  // the list of members that redefine this one
    MemberDef  *memDef;       // member definition for this declaration
    MemberDef  *memDec;       // member declaration for this definition
    ExampleList *exampleList; // a list of all examples using this member
    ExampleDict *exampleDict; // a dictionary of all examples for quick access
    MemberList *enumFields;   // enumeration fields
    OutputList *enumDeclList; // stored piece of documentation for enumeration.
    NamespaceDef *nspace;     // the namespace this member is in.
    QString type;             // return type
    QString args;             // function arguments/variable array specifiers
    QString exception;        // exceptions that can be thrown
    QString decl;             // member declaration in class
    QString declFile;         // file where the declaration was found
    int     declLine;         // line where the declaration was found
    QString def;              // member definition in code (fully qualified name)
    QString defFile;          // file where the definition was found
    int     defLine;          // line where the definition was found
    QString ref;              // HTML anchor name
    Specifier virt;           // normal/virtual/pure virtual
    Protection prot;          // protection type [Public/Protected/Private]
    bool    related;          // is this a member that is only related to a class
    QString external;         // anchor of a member if extracted from a tag file
    bool    stat;             // is it a static function?
    MemberType mtype;         // returns the kind of member
    bool eUsed;               // is the enumerate already placed in a list
    bool proto;               // is it a prototype;
    bool docEnumValues;       // is an enum with documented enum values.
    ArgumentList *argList;    // argument list of this member
    ArgumentList *tArgList;   // template argument list of function template

    // disable copying of member defs
    MemberDef(const MemberDef &);
    MemberDef &operator=(const MemberDef &);
};


#endif
