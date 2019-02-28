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

#ifndef MEMBERDEF_H
#define MEMBERDEF_H

#include <qlist.h>
#include <sys/types.h>

#include "types.h"
#include "definition.h"

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
class QStrList;
struct TagInfo;

/** A model of a class/file/namespace member symbol. */
class MemberDef : virtual public Definition
{
  public:
    virtual ~MemberDef() {}
    virtual DefType definitionType() const = 0;
    // move this member into a different scope
    virtual MemberDef *deepCopy() const =0;
    virtual void moveTo(Definition *) = 0;
    
    //-----------------------------------------------------------------------------------
    // ----  getters -----
    //-----------------------------------------------------------------------------------

    // link id
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString getReference() const = 0;
    virtual QCString anchor() const = 0;

    virtual const char *declaration() const = 0;
    virtual const char *definition() const = 0;
    virtual const char *typeString() const = 0;
    virtual const char *argsString() const = 0;
    virtual const char *excpString() const = 0;
    virtual const char *bitfieldString() const = 0;
    virtual const char *extraTypeChars() const = 0;
    virtual const QCString &initializer() const = 0;
    virtual int initializerLines() const = 0;
    virtual uint64 getMemberSpecifiers() const = 0;
    virtual MemberList *getSectionList(Definition *d) const = 0;
    virtual QCString    displayDefinition() const = 0;

    // scope query members
    virtual ClassDef *getClassDef() const = 0;
    virtual FileDef  *getFileDef() const = 0;
    virtual NamespaceDef* getNamespaceDef() const = 0;
    virtual ClassDef *accessorClass() const = 0;

    // grabbing the property read/write accessor names
    virtual const char *getReadAccessor() const = 0;
    virtual const char *getWriteAccessor() const = 0;
    
    // querying the grouping definition
    virtual GroupDef *getGroupDef() const = 0;
    virtual Grouping::GroupPri_t getGroupPri() const = 0;
    virtual const char *getGroupFileName() const = 0;
    virtual int getGroupStartLine() const = 0;
    virtual bool getGroupHasDocs() const = 0;
    virtual QCString qualifiedName() const = 0;
    virtual QCString objCMethodName(bool localLink,bool showStatic) const = 0; 

    // direct kind info 
    virtual Protection protection() const = 0;
    virtual Specifier virtualness(int count=0) const = 0;
    virtual MemberType memberType() const = 0;
    virtual QCString   memberTypeName() const = 0;

    // getter methods
    virtual bool isSignal() const = 0;
    virtual bool isSlot() const = 0;
    virtual bool isVariable() const = 0;
    virtual bool isEnumerate() const = 0;
    virtual bool isEnumValue() const = 0;
    virtual bool isTypedef() const = 0;
    virtual bool isSequence() const = 0;
    virtual bool isDictionary() const = 0;
    virtual bool isFunction() const = 0;
    virtual bool isFunctionPtr() const = 0;
    virtual bool isDefine() const = 0;
    virtual bool isFriend() const = 0;
    virtual bool isDCOP() const = 0;
    virtual bool isProperty() const = 0;
    virtual bool isEvent() const = 0;
    virtual bool isRelated() const = 0;
    virtual bool isForeign() const = 0;
    virtual bool isStatic() const = 0;
    virtual bool isInline() const = 0;
    virtual bool isExplicit() const = 0;
    virtual bool isMutable() const = 0;
    virtual bool isGettable() const = 0;
    virtual bool isPrivateGettable() const = 0;
    virtual bool isProtectedGettable() const = 0;
    virtual bool isSettable() const = 0;
    virtual bool isPrivateSettable() const = 0;
    virtual bool isProtectedSettable() const = 0;
    virtual bool isReadable() const = 0;
    virtual bool isWritable() const = 0;
    virtual bool isAddable() const = 0;
    virtual bool isRemovable() const = 0;
    virtual bool isRaisable() const = 0;
    virtual bool isFinal() const = 0;
    virtual bool isAbstract() const = 0;
    virtual bool isOverride() const = 0;
    virtual bool isInitonly() const = 0;
    virtual bool isOptional() const = 0;
    virtual bool isRequired() const = 0;
    virtual bool isNonAtomic() const = 0;
    virtual bool isCopy() const = 0;
    virtual bool isAssign() const = 0;
    virtual bool isRetain() const = 0;
    virtual bool isWeak() const = 0;
    virtual bool isStrong() const = 0;
    virtual bool isUnretained() const = 0;
    virtual bool isNew() const = 0;
    virtual bool isSealed() const = 0;
    virtual bool isImplementation() const = 0;
    virtual bool isExternal() const = 0;
    virtual bool isAlias() const = 0;
    virtual bool isDefault() const = 0;
    virtual bool isDelete() const = 0;
    virtual bool isNoExcept() const = 0;
    virtual bool isAttribute() const = 0; // UNO IDL attribute
    virtual bool isUNOProperty() const = 0; // UNO IDL property
    virtual bool isReadonly() const = 0;
    virtual bool isBound() const = 0;
    virtual bool isConstrained() const = 0;
    virtual bool isTransient() const = 0;
    virtual bool isMaybeVoid() const = 0;
    virtual bool isMaybeDefault() const = 0;
    virtual bool isMaybeAmbiguous() const = 0;
    virtual bool isPublished() const = 0; // UNO IDL published
    virtual bool isTemplateSpecialization() const = 0;
    virtual bool hasDocumentedParams() const = 0;
    virtual bool hasDocumentedReturnType() const = 0;
    virtual bool isObjCMethod() const = 0;
    virtual bool isObjCProperty() const = 0;
    virtual bool isConstructor() const = 0;
    virtual bool isDestructor() const = 0;
    virtual bool hasOneLineInitializer() const = 0;
    virtual bool hasMultiLineInitializer() const = 0;
    virtual bool showInCallGraph() const = 0;
    virtual bool isStrongEnumValue() const = 0;
    virtual bool livesInsideEnum() const = 0;
    virtual bool isSliceLocal() const = 0;
    virtual bool isConstExpr() const = 0;

    // derived getters
    virtual bool isFriendToHide() const = 0;
    virtual bool isNotFriend() const = 0;
    virtual bool isFunctionOrSignalSlot() const = 0;
    virtual bool isRelatedOrFriend() const = 0;

    // output info
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual bool hasDocumentation() const = 0;  // overrides hasDocumentation in definition.h
    //bool hasUserDocumentation() const = 0; // overrides hasUserDocumentation
    virtual bool isDeleted() const = 0;
    virtual bool isBriefSectionVisible() const = 0;
    virtual bool isDetailedSectionVisible(bool inGroup,bool inFile) const = 0;
    virtual bool isDetailedSectionLinkable() const = 0;
    virtual bool isFriendClass() const = 0;
    virtual bool isDocumentedFriendClass() const = 0;

    virtual MemberDef *reimplements() const = 0;
    virtual MemberList *reimplementedBy() const = 0;
    virtual bool isReimplementedBy(ClassDef *cd) const = 0;

    virtual ClassDef *relatedAlso() const = 0;

    virtual bool hasDocumentedEnumValues() const = 0;
    virtual MemberDef *getAnonymousEnumType() const = 0;
    virtual bool isDocsForDefinition() const = 0;
    virtual MemberDef *getEnumScope() const = 0;
    virtual MemberList *enumFieldList() const = 0;
    virtual void setEnumBaseType(const QCString &type) = 0;
    virtual QCString enumBaseType() const = 0;

    virtual bool hasExamples() = 0;
    virtual ExampleSDict *getExamples() const = 0;
    virtual bool isPrototype() const = 0;

    // argument related members
    virtual ArgumentList *argumentList() const = 0;
    virtual ArgumentList *declArgumentList() const = 0;
    virtual ArgumentList *templateArguments() const = 0;
    virtual QList<ArgumentList> *definitionTemplateParameterLists() const = 0;

    // member group related members
    virtual int getMemberGroupId() const = 0;
    virtual MemberGroup *getMemberGroup() const = 0;

    virtual bool fromAnonymousScope() const = 0;
    virtual bool anonymousDeclShown() const = 0;
    virtual MemberDef *fromAnonymousMember() const = 0;

    // callgraph related members
    virtual bool hasCallGraph() const = 0;
    virtual bool hasCallerGraph() const = 0;
    virtual bool visibleMemberGroup(bool hideNoHeader) = 0;
    // refrenced related members
    virtual bool hasReferencesRelation() const = 0;
    virtual bool hasReferencedByRelation() const = 0;

    virtual MemberDef *templateMaster() const = 0;
    virtual QCString getScopeString() const = 0;
    virtual ClassDef *getClassDefOfAnonymousType() = 0;

    // cached typedef functions
    virtual bool isTypedefValCached() const = 0;
    virtual ClassDef *getCachedTypedefVal() const = 0;
    virtual QCString getCachedTypedefTemplSpec() const = 0;
    virtual QCString getCachedResolvedTypedef() const = 0;

    virtual MemberDef *memberDefinition() const = 0;
    virtual MemberDef *memberDeclaration() const = 0;
    virtual MemberDef *inheritsDocsFrom() const = 0;
    virtual MemberDef *getGroupAlias() const = 0;

    virtual ClassDef *category() const = 0;
    virtual MemberDef *categoryRelation() const = 0;

    virtual QCString displayName(bool=TRUE) const = 0;
    virtual QCString getDeclType() const = 0;
    virtual void getLabels(QStrList &sl,Definition *container) const = 0;

    virtual const ArgumentList *typeConstraints() const = 0;

    // overrules
    virtual QCString documentation() const = 0;
    virtual QCString briefDescription(bool abbr=FALSE) const = 0;
    virtual QCString fieldType() const = 0;
    virtual bool isReference() const = 0;

    virtual QCString getDeclFileName() const = 0;
    virtual int getDeclLine() const = 0;
    virtual int getDeclColumn() const = 0;

    //-----------------------------------------------------------------------------------
    // ----  setters -----
    //-----------------------------------------------------------------------------------

    // set functions
    virtual void setMemberType(MemberType t) = 0;
    virtual void setDefinition(const char *d) = 0;
    virtual void setFileDef(FileDef *fd) = 0;
    virtual void setAnchor() = 0;
    virtual void setProtection(Protection p) = 0;
    virtual void setMemberSpecifiers(uint64 s) = 0;
    virtual void mergeMemberSpecifiers(uint64 s) = 0;
    virtual void setInitializer(const char *i) = 0;
    virtual void setBitfields(const char *s) = 0;
    virtual void setMaxInitLines(int lines) = 0;
    virtual void setMemberClass(ClassDef *cd) = 0;
    virtual void setSectionList(Definition *d,MemberList *sl) = 0;
    virtual void setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                     const QCString &fileName,int startLine,bool hasDocs,
                     MemberDef *member=0) = 0;
    virtual void setReadAccessor(const char *r) = 0;
    virtual void setWriteAccessor(const char *w) = 0;
    virtual void setTemplateSpecialization(bool b) = 0;
    
    virtual void makeRelated() = 0;
    virtual void makeForeign() = 0;
    virtual void setHasDocumentedParams(bool b) = 0;
    virtual void setHasDocumentedReturnType(bool b) = 0;
    virtual void setInheritsDocsFrom(MemberDef *md) = 0;
    virtual void setTagInfo(TagInfo *i) = 0;
    virtual void setArgsString(const char *as) = 0;

    // relation to other members
    virtual void setReimplements(MemberDef *md) = 0;
    virtual void insertReimplementedBy(MemberDef *md) = 0;

    virtual void setRelatedAlso(ClassDef *cd) = 0;

    // enumeration specific members
    virtual void insertEnumField(MemberDef *md) = 0;
    virtual void setEnumScope(MemberDef *md,bool livesInsideEnum=FALSE) = 0;
    virtual void setEnumClassScope(ClassDef *cd) = 0;
    virtual void setDocumentedEnumValues(bool value) = 0;
    virtual void setAnonymousEnumType(MemberDef *md) = 0;

    // example related members
    virtual bool addExample(const char *anchor,const char *name,const char *file) = 0;
    
    // prototype related members
    virtual void setPrototype(bool p,const QCString &df,int line, int column) = 0;
    virtual void setExplicitExternal(bool b,const QCString &df,int line,int column) = 0;
    virtual void setDeclFile(const QCString &df,int line,int column) = 0;

    // argument related members
    virtual void setArgumentList(ArgumentList *al) = 0;
    virtual void setDeclArgumentList(ArgumentList *al) = 0;
    virtual void setDefinitionTemplateParameterLists(QList<ArgumentList> *lists) = 0;
    virtual void setTypeConstraints(ArgumentList *al) = 0;
    virtual void setType(const char *t) = 0;
    virtual void setAccessorType(ClassDef *cd,const char *t) = 0;

    // namespace related members
    virtual void setNamespace(NamespaceDef *nd) = 0;

    // member group related members
    virtual void setMemberGroup(MemberGroup *grp) = 0;
    virtual void setMemberGroupId(int id) = 0;
    virtual void makeImplementationDetail() = 0;

    // anonymous scope members
    virtual void setFromAnonymousScope(bool b) = 0;
    virtual void setFromAnonymousMember(MemberDef *m) = 0;

    virtual void enableCallGraph(bool e) = 0;
    virtual void enableCallerGraph(bool e) = 0;

    virtual void enableReferencedByRelation(bool e) = 0;
    virtual void enableReferencesRelation(bool e) = 0;

    virtual void setTemplateMaster(MemberDef *mt) = 0;
    virtual void addListReference(Definition *d) = 0;
    virtual void setDocsForDefinition(bool b) = 0;
    virtual void setGroupAlias(MemberDef *md) = 0;

    virtual void cacheTypedefVal(ClassDef *val,const QCString &templSpec,const QCString &resolvedType) = 0;
    virtual void invalidateTypedefValCache() = 0;

    virtual void invalidateCachedArgumentTypes() = 0;
    
    // declaration <-> definition relation
    virtual void setMemberDefinition(MemberDef *md) = 0;
    virtual void setMemberDeclaration(MemberDef *md) = 0;
        
    virtual void setAnonymousUsed() = 0;
    virtual void copyArgumentNames(MemberDef *bmd) = 0;

    virtual void setCategory(ClassDef *) = 0;
    virtual void setCategoryRelation(MemberDef *) = 0;

    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE) = 0;
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine) = 0;
    virtual void setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine) = 0;

    virtual void setHidden(bool b) = 0;

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    // output generation
    virtual void writeDeclaration(OutputList &ol,
                   ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
                   bool inGroup, ClassDef *inheritFrom=0,const char *inheritId=0) = 0; 
    virtual void writeDocumentation(MemberList *ml,int memCount,int memTotal,OutputList &ol,
                            const char *scopeName,Definition *container,
                            bool inGroup,bool showEnumValues=FALSE,bool
                            showInline=FALSE) = 0;
    virtual void writeMemberDocSimple(OutputList &ol,Definition *container) = 0;
    virtual void writeEnumDeclaration(OutputList &typeDecl,
            ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd) = 0;
    virtual void writeTagFile(FTextStream &) = 0;
    virtual void warnIfUndocumented() = 0;
    virtual void warnIfUndocumentedParams() = 0;

    virtual MemberDef *createTemplateInstanceMember(ArgumentList *formalArgs,
               ArgumentList *actualArgs) = 0;

    virtual void findSectionsInDocumentation() = 0;
    virtual void writeLink(OutputList &ol,
                   ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
                   bool onlyText=FALSE) = 0;
    virtual void addToSearchIndex() = 0;

};

/** Factory method to create a new instance of a MemberDef */
MemberDef *createMemberDef(const char *defFileName,int defLine,int defColumn,
              const char *type,const char *name,const char *args,
              const char *excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList *tal,
              const ArgumentList *al,const char *metaData);

void combineDeclarationAndDefinition(MemberDef *mdec,MemberDef *mdef);

#endif
