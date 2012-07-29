/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
#include <qstack.h>

#include "types.h"
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
class MemberDefImpl;

/** A model of a class/file/namespace member symbol. */
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
      Signal,
      Slot,
      Friend,
      DCOP,
      Property,
      Event
    };

    MemberDef(const char *defFileName,int defLine,
              const char *type,const char *name,const char *args,
              const char *excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList *tal,
              const ArgumentList *al);
   ~MemberDef(); 
    DefType definitionType() const        { return TypeMember; }
    // move this member into a different scope
    MemberDef *deepCopy() const;
    void moveTo(Definition *);
    
    //-----------------------------------------------------------------------------------
    // ----  getters -----
    //-----------------------------------------------------------------------------------

    // link id
    QCString getOutputFileBase() const;
    QCString getReference() const;
    QCString anchor() const;

    const char *declaration() const;
    const char *definition() const;
    const char *typeString() const;
    const char *argsString() const;
    const char *excpString() const;
    const char *bitfieldString() const;
    const char *extraTypeChars() const;
    const QCString &initializer() const;
    int initializerLines() const;
    int  getMemberSpecifiers() const;
    MemberList *getSectionList(Definition *d) const;

    // scope query members
    ClassDef *getClassDef() const;
    FileDef  *getFileDef() const;
    NamespaceDef* getNamespaceDef() const;

    // grabbing the property read/write accessor names
    const char *getReadAccessor() const;
    const char *getWriteAccessor() const;
    
    // querying the grouping definition
    GroupDef *getGroupDef() const;
    Grouping::GroupPri_t getGroupPri() const;
    const char *getGroupFileName() const;
    int getGroupStartLine() const;
    bool getGroupHasDocs() const;
    QCString qualifiedName() const;
    QCString objCMethodName(bool localLink,bool showStatic) const; 

    // direct kind info 
    Protection protection() const;
    Specifier virtualness(int count=0) const;
    MemberType memberType() const;
    QCString   memberTypeName() const;

    // getter methods
    bool isSignal() const;
    bool isSlot() const;
    bool isVariable() const;
    bool isEnumerate() const;
    bool isEnumValue() const;
    bool isTypedef() const;
    bool isFunction() const;
    bool isFunctionPtr() const;
    bool isDefine() const;
    bool isFriend() const;
    bool isDCOP() const;
    bool isProperty() const;
    bool isEvent() const;
    bool isRelated() const;
    bool isForeign() const;
    bool isStatic() const;
    bool isInline() const;
    bool isExplicit() const;
    bool isMutable() const;
    bool isGettable() const;
    bool isSettable() const;
    bool isReadable() const;
    bool isWritable() const;
    bool isAddable() const;
    bool isRemovable() const;
    bool isRaisable() const;
    bool isFinal() const;
    bool isAbstract() const;
    bool isOverride() const;
    bool isInitonly() const;
    bool isOptional() const;
    bool isRequired() const;
    bool isNonAtomic() const;
    bool isCopy() const;
    bool isAssign() const;
    bool isRetain() const;
    bool isWeak() const;
    bool isStrong() const;
    bool isUnretained() const;
    bool isNew() const;
    bool isSealed() const;
    bool isImplementation() const;
    bool isExternal() const;
    bool isAlias() const;
    bool isDefault() const;
    bool isDelete() const;
    bool isNoExcept() const;
    bool isTemplateSpecialization() const;
    bool hasDocumentedParams() const;
    bool hasDocumentedReturnType() const;
    bool isObjCMethod() const;
    bool isObjCProperty() const;
    bool isConstructor() const;
    bool isDestructor() const;
    bool hasOneLineInitializer() const;
    bool hasMultiLineInitializer() const;
    bool protectionVisible() const;

    // output info
    bool isLinkableInProject() const;
    bool isLinkable() const;
    bool hasDocumentation() const;  // overrides hasDocumentation in definition.h
    //bool hasUserDocumentation() const; // overrides hasUserDocumentation
    bool isBriefSectionVisible() const;
    bool isDetailedSectionVisible(bool inGroup,bool inFile) const;
    bool isDetailedSectionLinkable() const;
    bool isFriendClass() const;
    bool isDocumentedFriendClass() const;

    MemberDef *reimplements() const;
    LockingPtr< MemberList > reimplementedBy() const;
    bool isReimplementedBy(ClassDef *cd) const;

    //int inbodyLine() const;
    //QCString inbodyFile() const;
    //const QCString &inbodyDocumentation() const;

    ClassDef *relatedAlso() const;

    bool hasDocumentedEnumValues() const;
    MemberDef *getAnonymousEnumType() const;
    bool isDocsForDefinition() const;
    MemberDef *getEnumScope() const;
    LockingPtr< MemberList > enumFieldList() const;
    void setEnumBaseType(const QCString &type);
    QCString enumBaseType() const;

    bool hasExamples();
    LockingPtr<ExampleSDict> getExamples() const;
    bool isPrototype() const;

    // argument related members
    LockingPtr<ArgumentList> argumentList() const;
    LockingPtr<ArgumentList> declArgumentList() const;
    LockingPtr<ArgumentList> templateArguments() const;
    LockingPtr< QList<ArgumentList> > definitionTemplateParameterLists() const;

    // member group related members
    int getMemberGroupId() const;
    MemberGroup *getMemberGroup() const;

    bool fromAnonymousScope() const;
    bool anonymousDeclShown() const;

    // callgraph related members
    bool hasCallGraph() const;
    bool hasCallerGraph() const;
    bool visibleMemberGroup(bool hideNoHeader);

    MemberDef *templateMaster() const;
    QCString getScopeString() const;
    ClassDef *getClassDefOfAnonymousType();

    // cached typedef functions
    bool isTypedefValCached() const;
    ClassDef *getCachedTypedefVal() const;
    QCString getCachedTypedefTemplSpec() const;
    QCString getCachedResolvedTypedef() const;

    MemberDef *memberDefinition() const;
    MemberDef *memberDeclaration() const;
    MemberDef *inheritsDocsFrom() const;
    MemberDef *getGroupAlias() const;

    ClassDef *category() const;
    MemberDef *categoryRelation() const;

    QCString displayName(bool=TRUE) const;

    //-----------------------------------------------------------------------------------
    // ----  setters -----
    //-----------------------------------------------------------------------------------

    // set functions
    void setMemberType(MemberType t);
    void setDefinition(const char *d);
    void setFileDef(FileDef *fd);
    void setAnchor(const char *a);
    void setProtection(Protection p);
    void setMemberSpecifiers(int s);
    void mergeMemberSpecifiers(int s);
    void setInitializer(const char *i);
    void setBitfields(const char *s);
    void setMaxInitLines(int lines);
    void setMemberClass(ClassDef *cd);
    void setSectionList(Definition *d,MemberList *sl);
    void setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                     const QCString &fileName,int startLine,bool hasDocs,
                     MemberDef *member=0);
    void setExplicitExternal(bool b);
    void setReadAccessor(const char *r);
    void setWriteAccessor(const char *w);
    void setTemplateSpecialization(bool b);
    
    void makeRelated();
    void makeForeign();
    void setHasDocumentedParams(bool b);
    void setHasDocumentedReturnType(bool b);
    void setInheritsDocsFrom(MemberDef *md);
    void setTagInfo(TagInfo *i);
    void setArgsString(const char *as);

    // relation to other members
    void setReimplements(MemberDef *md);
    void insertReimplementedBy(MemberDef *md);

    // in-body documentation
    //void setInbodyDocumentation(const char *docs,const char *file,int line);

    void setRelatedAlso(ClassDef *cd);

    // enumeration specific members
    void insertEnumField(MemberDef *md);
    void setEnumScope(MemberDef *md);
    void setEnumClassScope(ClassDef *cd);
    void setDocumentedEnumValues(bool value);
    void setAnonymousEnumType(MemberDef *md);

    // example related members
    bool addExample(const char *anchor,const char *name,const char *file);
    
    // prototype related members
    void setPrototype(bool p);

    // argument related members
    void setArgumentList(ArgumentList *al);
    void setDeclArgumentList(ArgumentList *al);
    void setDefinitionTemplateParameterLists(QList<ArgumentList> *lists);
    void setTypeConstraints(ArgumentList *al);
    void setType(const char *t);
    void setAccessorType(ClassDef *cd,const char *t);

    // namespace related members
    void setNamespace(NamespaceDef *nd);

    // member group related members
    void setMemberGroup(MemberGroup *grp);
    void setMemberGroupId(int id);
    void makeImplementationDetail();

    // anonymous scope members
    void setFromAnonymousScope(bool b);
    void setFromAnonymousMember(MemberDef *m);

    void enableCallGraph(bool e);
    void enableCallerGraph(bool e);

    void setTemplateMaster(MemberDef *mt);
    void addListReference(Definition *d);
    void setDocsForDefinition(bool b);
    void setGroupAlias(MemberDef *md);

    void cacheTypedefVal(ClassDef *val,const QCString &templSpec,const QCString &resolvedType);
    void invalidateTypedefValCache();

    void invalidateCachedArgumentTypes();
    
    // declaration <-> definition relation
    void setMemberDefinition(MemberDef *md);
    void setMemberDeclaration(MemberDef *md);
        
    void setAnonymousUsed();
    void copyArgumentNames(MemberDef *bmd);

    void setCategory(ClassDef *);
    void setCategoryRelation(MemberDef *);

    void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);
    void setBriefDescription(const char *b,const char *briefFile,int briefLine);
    void setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine);

    void setHidden(bool b);

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    // output generation
    void writeDeclaration(OutputList &ol,
                   ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
                   bool inGroup,ClassDef *inheritFrom=0,const char *inheritId=0); 
    void writeDocumentation(MemberList *ml,OutputList &ol,
                            const char *scopeName,Definition *container,
                            bool inGroup,bool showEnumValues=FALSE,bool
                            showInline=FALSE);
    void writeMemberDocSimple(OutputList &ol,Definition *container);
    void warnIfUndocumented();
    
    MemberDef *createTemplateInstanceMember(ArgumentList *formalArgs,
               ArgumentList *actualArgs);

    void writeEnumDeclaration(OutputList &typeDecl,
            ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd);

    void findSectionsInDocumentation();
    
    bool visited;
   
  protected:
    void flushToDisk() const;
    void loadFromDisk() const;
  private:
    void lock() const;
    void unlock() const;
    void saveToDisk() const;
    void makeResident() const;
    void _computeLinkableInProject();
    void _computeIsConstructor();
    void _computeIsDestructor();
    void _getLabels(QStrList &sl,Definition *container) const;
    void _writeCallGraph(OutputList &ol);
    void _writeCallerGraph(OutputList &ol);
    void _writeReimplements(OutputList &ol);
    void _writeReimplementedBy(OutputList &ol);
    void _writeExamples(OutputList &ol);
    void _writeTypeConstraints(OutputList &ol);
    void _writeEnumValues(OutputList &ol,Definition *container,
                          const QCString &cfname,const QCString &ciname,
                          const QCString &cname);
    void _writeCategoryRelation(OutputList &ol);

    static int s_indentLevel;
    // disable copying of member defs
    MemberDef(const MemberDef &);
    MemberDef &operator=(const MemberDef &);

    void writeLink(OutputList &ol,
                   ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
                   bool onlyText=FALSE);

    MemberDefImpl *m_impl;
    int m_cacheHandle;
    off_t m_storagePos;     // location where the item is stored in file (if impl==0)
    bool m_flushPending;
    uchar m_isLinkableCached;    // 0 = not cached, 1=FALSE, 2=TRUE
    uchar m_isConstructorCached; // 0 = not cached, 1=FALSE, 2=TRUE
    uchar m_isDestructorCached;  // 0 = not cached, 1=FALSE, 2=TRUE
};

void combineDeclarationAndDefinition(MemberDef *mdec,MemberDef *mdef);

#endif
