/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#ifndef MEMBERDEF_H
#define MEMBERDEF_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>

#include "entry.h"
#include "definition.h"
#include "sortdict.h"

class ClassDef;
class NamespaceDef;
class GroupDef;
class FileDef;
class MemberList;
class MemberGroup;
class ExampleSDict;
class OutputList;
class GroupDef;
class QTextStream;
class ArgumentList;

struct SourceReference
{
  FileDef *fd;
  QCString anchor;
};

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
      Friend,
      DCOP,
      Property
    };

    MemberDef(const char *defFileName,int defLine,
              const char *type,const char *name,const char *args,
              const char *excp,Protection prot,Specifier virt,bool stat,
              bool related,MemberType t,const ArgumentList *tal,
              const ArgumentList *al);
   ~MemberDef(); 
    DefType definitionType() { return TypeMember; }
    
    QCString getOutputFileBase() const;
    const char *declaration() const       { return decl; }
    const char *definition() const        { return def; }
    const char *typeString() const        { return type; }
    const char *argsString() const        { return args; }
    const char *excpString() const        { return exception; }     
    const char *bitfieldString() const    { return bitfields; }     
    QCString anchor() const;
    const QCString &initializer() const   { return init; }
    int initializerLines() const          { return initLines; }
    int  getMemberSpecifiers() const      { return memSpec; }
    MemberList *getSectionList(Definition *d) const;

    // scope query members
    ClassDef *getClassDef() const         { return classDef; }
    FileDef  *getFileDef() const          { return fileDef; }
    NamespaceDef* getNamespaceDef() const { return nspace; }
    //Definition *getCompoundDef() const;

    // querying the grouping definition
    GroupDef *getGroupDef() const            { return group; }
    Grouping::GroupPri_t getGroupPri() const { return grouppri; }
    const char *getGroupFileName() const     { return groupFileName; }
    int getGroupStartLine() const            { return groupStartLine; }
    bool getGroupHasDocs() const             { return groupHasDocs; }

    // direct kind info 
    Protection protection() const         { return prot; }
    Specifier virtualness() const;
    MemberType memberType() const         { return mtype; }

    // convenience methods
    bool isSignal() const                 { return mtype==Signal;      }
    bool isSlot() const                   { return mtype==Slot;        }
    bool isVariable() const               { return mtype==Variable;    }
    bool isEnumerate() const              { return mtype==Enumeration; }
    bool isEnumValue() const              { return mtype==EnumValue;   }
    bool isTypedef() const                { return mtype==Typedef;     }
    bool isFunction() const               { return mtype==Function;    } 
    bool isDefine() const                 { return mtype==Define;      }
    bool isFriend() const                 { return mtype==Friend;      }
    bool isDCOP() const                   { return mtype==DCOP;        }
    bool isProperty() const               { return mtype==Property;    }
    bool isRelated() const                { return related; }
    bool isStatic() const                 { return stat; }
    bool isInline() const                 { return (memSpec&Entry::Inline)!=0; }
    bool isExplicit() const               { return (memSpec&Entry::Explicit)!=0; }
    bool isMutable() const                { return (memSpec&Entry::Mutable)!=0; }
    bool isExternal() const               { return explExt; }
    bool hasOneLineInitializer() const;
    bool hasMultiLineInitializer() const;

    // output info
    bool isLinkableInProject() const;
    bool isLinkable() const;
    bool hasDocumentation() const;  // overrides hasDocumentation in definition.h
    bool isBriefSectionVisible() const;
    bool isDetailedSectionVisible(bool inGroup=FALSE) const;
    bool isDetailedSectionLinkable() const;
    bool isDocumentedFriendClass() const;

    // set functions
    void setMemberType(MemberType t)      { mtype=t; }
    void setDefinition(const char *d)     { def=d; }
    void setFileDef(FileDef *fd)          { fileDef=fd; }
    void setAnchor(const char *a);
    void setProtection(Protection p)      { prot=p; }
    void setMemberSpecifiers(int s)       { memSpec=s; }
    void mergeMemberSpecifiers(int s)     { memSpec|=s; }
    void setInitializer(const char *i);
    void setBitfields(const char *s)      { bitfields = s; }
    void setMaxInitLines(int lines)       { userInitLines=lines; }
    void setMemberClass(ClassDef *cd);
    void setSectionList(Definition *d,MemberList *sl);
    void setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs);
    void setExplicitExternal(bool b)      { explExt=b; }
    
    void makeRelated()                    { related=TRUE; } 

    // output generatation
    void writeLink(OutputList &ol,
                   ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);
    void writeDeclaration(OutputList &ol,
                   ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
                   bool inGroup); 
    void writeDocumentation(MemberList *ml,OutputList &ol,
                            const char *scopeName,Definition *container,
                            bool inGroup);
    void warnIfUndocumented();
    
    // relation to other members
    void setReimplements(MemberDef *md);
    void insertReimplementedBy(MemberDef *md);
    MemberDef  *reimplements() const;
    MemberList *reimplementedBy() const;
    
    // enumeration specific members
    void insertEnumField(MemberDef *md);
    void setEnumScope(MemberDef *md);
    MemberDef *getEnumScope() const          { return enumScope; }
    void setEnumDecl(OutputList &ed);
    //void setEnumUsed()                       { eUsed=TRUE; }
    //bool enumUsed() const                    { return eUsed; }
    OutputList *enumDecl() const             { return enumDeclList; }
    MemberList *enumFieldList() const        { return enumFields; }
    void setDocumentedEnumValues(bool value) { docEnumValues=value; }
    bool hasDocumentedEnumValues() const     { return docEnumValues; }
    void setAnonymousEnumType(MemberDef *md) { annEnumType = md; }
    MemberDef *getAnonymousEnumType() const  { return annEnumType; }
    bool isDocsForDefinition() const         { return docsForDefinition; }
    
    // example related members
    bool addExample(const char *anchor,const char *name,const char *file);
    bool hasExamples();
    ExampleSDict *getExamples() const { return exampleSDict; }
    
    // prototype related members
    void setPrototype(bool p) { proto=p; }
    bool isPrototype() const { return proto; }
    
    // argument related members
    ArgumentList *argumentList() const { return defArgList; }
    ArgumentList *declArgumentList() const { return declArgList; }
    void setArgumentList(ArgumentList *al) 
    { 
      if (defArgList) delete defArgList;
      defArgList = al;
    }
    void setDeclArgumentList(ArgumentList *al)
    {
      if (declArgList) delete declArgList;
      declArgList = al;
    }
    ArgumentList *templateArguments() const { return tArgList; }
    void setDefinitionTemplateParameterLists(QList<ArgumentList> *lists);
    QList<ArgumentList> *definitionTemplateParameterLists() const 
    { return m_defTmpArgLists; }
    
    // namespace related members
    void setNamespace(NamespaceDef *nd);

    // member group related members
    void setMemberGroup(MemberGroup *grp);
    MemberGroup *getMemberGroup() const { return memberGroup; }
    void setMemberGroupId(int id) { grpId=id; }
    int getMemberGroupId() const { return grpId; }
    
    // anonymous scope members
    void setFromAnonymousScope(bool b) { annScope=b; }    
    void setFromAnonymousMember(MemberDef *m) { annMemb=m; }    
    bool fromAnonymousScope() const { return annScope; }
    bool anonymousDeclShown() const { return annUsed; }
    void setIndentDepth( int i) { indDepth=i; }
    int  indentDepth() { return indDepth; }

    bool visibleMemberGroup(bool hideNoHeader);
    MemberDef *templateMaster() const { return m_templateMaster; } 

    QCString getScopeString() const;
    
    ClassDef *getClassDefOfAnonymousType();
    MemberDef *createTemplateInstanceMember(ArgumentList *formalArgs,
               ArgumentList *actualArgs);
    void setTemplateMaster(MemberDef *mt) { m_templateMaster=mt; }
    void addListReference(Definition *d);

    QCString getBodyAnchor() const 
    { 
      return bodyMemb ? bodyMemb->anchor() : anchor(); 
    }
    void setBodyMember(MemberDef *md) { bodyMemb = md; }
    void setDocsForDefinition(bool b) { docsForDefinition = b; }
        
    bool visited;
    
  private:
    ClassDef   *classDef;     // member of or related to 
    FileDef    *fileDef;      // member of file definition 
    MemberDef  *redefines;    // the member that is redefined by this one
    MemberDef  *enumScope;    // the enclosing scope, if this is an enum field
    MemberDef  *annEnumType;  // the annonymous enum that is the type of this member
    MemberList *redefinedBy;  // the list of members that redefine this one
    MemberDef  *memDef;       // member definition for this declaration
    MemberDef  *memDec;       // member declaration for this definition

    ExampleSDict *exampleSDict; // a dictionary of all examples for quick access

    MemberList *enumFields;   // enumeration fields
    OutputList *enumDeclList; // stored piece of documentation for enumeration.
    NamespaceDef *nspace;     // the namespace this member is in.
    QCString type;            // return type
    QCString args;            // function arguments/variable array specifiers
    QCString bitfields;       // struct member bitfields
    QCString exception;       // exceptions that can be thrown
    QCString init;            // initializer
    int initLines;            // number of lines in the initializer
    QCString decl;            // member declaration in class
    //QCString declFile;        // file where the declaration was found
    //int      declLine;        // line where the declaration was found
    QCString def;             // member definition in code (fully qualified name)
    QCString anc;             // HTML anchor name
    MemberDef *bodyMemb;      // Member containing the definition
    Specifier virt;           // normal/virtual/pure virtual
    Protection prot;          // protection type [Public/Protected/Private]
    bool    related;          // is this a member that is only related to a class
    bool    stat;             // is it a static function?
    int     memSpec;          // The specifiers present for this member
    MemberType mtype;         // returns the kind of member
    //bool eUsed;               // is the enumerate already placed in a list
    bool proto;               // is it a prototype;
    bool docEnumValues;       // is an enum with documented enum values.
    bool annScope;            // member is part of an annoymous scope
    bool annUsed;             
    bool annShown;           
    int  indDepth;            // indentation depth for this member if inside an annonymous scope
    int  maxInitLines;        // when the initializer will be displayed 
    int userInitLines;        // result of explicit \hideinitializer or \showinitializer
    MemberList *section;      // declation list containing this member 
    MemberDef  *annMemb;
    ArgumentList *defArgList;    // argument list of this member definition
    ArgumentList *declArgList;   // argument list of this member declaration
    ArgumentList *tArgList;   // template argument list of function template
    QList<ArgumentList> *m_defTmpArgLists;
    int grpId;                // group id
    MemberGroup *memberGroup; // group's member definition

    GroupDef *group;          // group in which this member is in
    bool explExt;             // member was explicitly declared external

    ClassDef *cachedAnonymousType; // if the member has an anonymous compound
                                   // as its type then this is computed by
                                   // getClassDefOfAnonymousType() and 
                                   // cached here. 
    Grouping::GroupPri_t grouppri; // priority of this definition
    QCString groupFileName;   // file where this grouping was defined
    int groupStartLine;       // line  "      "      "     "     "
    bool groupHasDocs;        // true if the entry that caused the grouping was documented
    MemberDef *m_templateMaster;
    SIntDict<MemberList> *classSectionSDict;
    bool docsForDefinition;   // TRUE => documentation block is put before
                              //         definition.
                              // FALSE => block is put before declaration.


    // disable copying of member defs
    MemberDef(const MemberDef &);
    MemberDef &operator=(const MemberDef &);
};

#endif
