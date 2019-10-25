/******************************************************************************
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

#include <stdio.h>
#include <qglobal.h>
#include <qregexp.h>
#include <assert.h>
#include "md5.h"
#include "memberdef.h"
#include "membername.h"
#include "doxygen.h"
#include "util.h"
#include "code.h"
#include "message.h"
#include "htmlhelp.h"
#include "language.h"
#include "outputlist.h"
#include "example.h"
#include "membergroup.h"
#include "groupdef.h"
#include "defargs.h"
#include "docparser.h"
#include "dot.h"
#include "dotcallgraph.h"
#include "searchindex.h"
#include "parserintf.h"
#include "objcache.h"

#include "vhdldocgen.h"
#include "arguments.h"
#include "memberlist.h"
#include "namespacedef.h"
#include "filedef.h"
#include "config.h"
#include "definitionimpl.h"

//-----------------------------------------------------------------------------

class MemberDefImpl : public DefinitionImpl, public MemberDef
{
  public:
    MemberDefImpl(const char *defFileName,int defLine,int defColumn,
              const char *type,const char *name,const char *args,
              const char *excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList *tal,
              const ArgumentList *al,const char *metaData);
    virtual ~MemberDefImpl();

    virtual DefType definitionType() const        { return TypeMember; }
    virtual       MemberDef *resolveAlias()       { return this; }
    virtual const MemberDef *resolveAlias() const { return this; }
    virtual MemberDef *deepCopy() const;
    virtual void moveTo(Definition *);
    virtual QCString getOutputFileBase() const;
    virtual QCString getReference() const;
    virtual QCString anchor() const;
    virtual const char *declaration() const;
    virtual const char *definition() const;
    virtual const char *typeString() const;
    virtual const char *argsString() const;
    virtual const char *excpString() const;
    virtual const char *bitfieldString() const;
    virtual const char *extraTypeChars() const;
    virtual const QCString &initializer() const;
    virtual int initializerLines() const;
    virtual uint64 getMemberSpecifiers() const;
    virtual const MemberList *getSectionList() const;
    virtual QCString    displayDefinition() const;
    virtual const ClassDef *getClassDef() const;
    virtual ClassDef *getClassDef();
    virtual const FileDef  *getFileDef() const;
    virtual FileDef  *getFileDef();
    virtual const NamespaceDef* getNamespaceDef() const;
    virtual NamespaceDef* getNamespaceDef();
    virtual const GroupDef *getGroupDef() const;
    virtual GroupDef *getGroupDef();
    virtual ClassDef *accessorClass() const;
    virtual const char *getReadAccessor() const;
    virtual const char *getWriteAccessor() const;
    virtual Grouping::GroupPri_t getGroupPri() const;
    virtual const char *getGroupFileName() const;
    virtual int getGroupStartLine() const;
    virtual bool getGroupHasDocs() const;
    virtual QCString qualifiedName() const;
    virtual QCString objCMethodName(bool localLink,bool showStatic) const; 
    virtual Protection protection() const;
    virtual Specifier virtualness(int count=0) const;
    virtual MemberType memberType() const;
    virtual QCString   memberTypeName() const;
    virtual bool isSignal() const;
    virtual bool isSlot() const;
    virtual bool isVariable() const;
    virtual bool isEnumerate() const;
    virtual bool isEnumValue() const;
    virtual bool isTypedef() const;
    virtual bool isSequence() const;
    virtual bool isDictionary() const;
    virtual bool isFunction() const;
    virtual bool isFunctionPtr() const;
    virtual bool isDefine() const;
    virtual bool isFriend() const;
    virtual bool isDCOP() const;
    virtual bool isProperty() const;
    virtual bool isEvent() const;
    virtual bool isRelated() const;
    virtual bool isForeign() const;
    virtual bool isStatic() const;
    virtual bool isInline() const;
    virtual bool isExplicit() const;
    virtual bool isMutable() const;
    virtual bool isGettable() const;
    virtual bool isPrivateGettable() const;
    virtual bool isProtectedGettable() const;
    virtual bool isSettable() const;
    virtual bool isPrivateSettable() const;
    virtual bool isProtectedSettable() const;
    virtual bool isReadable() const;
    virtual bool isWritable() const;
    virtual bool isAddable() const;
    virtual bool isRemovable() const;
    virtual bool isRaisable() const;
    virtual bool isFinal() const;
    virtual bool isAbstract() const;
    virtual bool isOverride() const;
    virtual bool isInitonly() const;
    virtual bool isOptional() const;
    virtual bool isRequired() const;
    virtual bool isNonAtomic() const;
    virtual bool isCopy() const;
    virtual bool isAssign() const;
    virtual bool isRetain() const;
    virtual bool isWeak() const;
    virtual bool isStrong() const;
    virtual bool isUnretained() const;
    virtual bool isNew() const;
    virtual bool isSealed() const;
    virtual bool isImplementation() const;
    virtual bool isExternal() const;
    virtual bool isTypeAlias() const;
    virtual bool isDefault() const;
    virtual bool isDelete() const;
    virtual bool isNoExcept() const;
    virtual bool isAttribute() const;
    virtual bool isUNOProperty() const;
    virtual bool isReadonly() const;
    virtual bool isBound() const;
    virtual bool isConstrained() const;
    virtual bool isTransient() const;
    virtual bool isMaybeVoid() const;
    virtual bool isMaybeDefault() const;
    virtual bool isMaybeAmbiguous() const;
    virtual bool isPublished() const;
    virtual bool isTemplateSpecialization() const;
    virtual bool isObjCMethod() const;
    virtual bool isObjCProperty() const;
    virtual bool isConstructor() const;
    virtual bool isDestructor() const;
    virtual bool hasOneLineInitializer() const;
    virtual bool hasMultiLineInitializer() const;
    virtual bool showInCallGraph() const;
    virtual bool isStrongEnumValue() const;
    virtual bool livesInsideEnum() const;
    virtual bool isSliceLocal() const;
    virtual bool isConstExpr() const;
    virtual int  numberOfFlowKeyWords() const;
    virtual bool isFriendToHide() const;
    virtual bool isNotFriend() const;
    virtual bool isFunctionOrSignalSlot() const;
    virtual bool isRelatedOrFriend() const;
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual bool hasDocumentation() const;
    virtual bool isDeleted() const;
    virtual bool isBriefSectionVisible() const;
    virtual bool isDetailedSectionVisible(bool inGroup,bool inFile) const;
    virtual bool isDetailedSectionLinkable() const;
    virtual bool isFriendClass() const;
    virtual bool isDocumentedFriendClass() const;
    virtual MemberDef *reimplements() const;
    virtual MemberList *reimplementedBy() const;
    virtual bool isReimplementedBy(const ClassDef *cd) const;
    virtual ClassDef *relatedAlso() const;
    virtual bool hasDocumentedEnumValues() const;
    virtual const MemberDef *getAnonymousEnumType() const;
    virtual bool isDocsForDefinition() const;
    virtual const MemberDef *getEnumScope() const;
    virtual const MemberList *enumFieldList() const;
    virtual void setEnumBaseType(const QCString &type);
    virtual QCString enumBaseType() const;
    virtual bool hasExamples() const;
    virtual ExampleSDict *getExamples() const;
    virtual bool isPrototype() const;
    virtual const ArgumentList *argumentList() const;
    virtual ArgumentList *argumentList();
    virtual const ArgumentList *declArgumentList() const;
    virtual const ArgumentList *templateArguments() const;
    virtual const QList<ArgumentList> *definitionTemplateParameterLists() const;
    virtual int getMemberGroupId() const;
    virtual MemberGroup *getMemberGroup() const;
    virtual bool fromAnonymousScope() const;
    virtual bool anonymousDeclShown() const;
    virtual MemberDef *fromAnonymousMember() const;
    virtual bool hasCallGraph() const;
    virtual bool hasCallerGraph() const;
    virtual bool visibleMemberGroup(bool hideNoHeader) const;
    virtual bool hasReferencesRelation() const;
    virtual bool hasReferencedByRelation() const;
    virtual MemberDef *templateMaster() const;
    virtual QCString getScopeString() const;
    virtual ClassDef *getClassDefOfAnonymousType() const;
    virtual bool isTypedefValCached() const;
    virtual const ClassDef *getCachedTypedefVal() const;
    virtual QCString getCachedTypedefTemplSpec() const;
    virtual QCString getCachedResolvedTypedef() const;
    virtual MemberDef *memberDefinition() const;
    virtual MemberDef *memberDeclaration() const;
    virtual MemberDef *inheritsDocsFrom() const;
    virtual const MemberDef *getGroupAlias() const;
    virtual ClassDef *category() const;
    virtual MemberDef *categoryRelation() const;
    virtual QCString displayName(bool=TRUE) const;
    virtual QCString getDeclType() const;
    virtual void getLabels(QStrList &sl,const Definition *container) const;
    virtual const ArgumentList *typeConstraints() const;
    virtual QCString documentation() const;
    virtual QCString briefDescription(bool abbr=FALSE) const;
    virtual QCString fieldType() const;
    virtual bool isReference() const;
    virtual QCString getDeclFileName() const;
    virtual int getDeclLine() const;
    virtual int getDeclColumn() const;
    virtual void setMemberType(MemberType t);
    virtual void setDefinition(const char *d);
    virtual void setFileDef(FileDef *fd);
    virtual void setAnchor();
    virtual void setProtection(Protection p);
    virtual void setMemberSpecifiers(uint64 s);
    virtual void mergeMemberSpecifiers(uint64 s);
    virtual void setInitializer(const char *i);
    virtual void setBitfields(const char *s);
    virtual void setMaxInitLines(int lines);
    virtual void setMemberClass(ClassDef *cd);
    virtual void setSectionList(MemberList *sl);
    virtual void setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                     const QCString &fileName,int startLine,bool hasDocs,
                     MemberDef *member=0);
    virtual void setReadAccessor(const char *r);
    virtual void setWriteAccessor(const char *w);
    virtual void setTemplateSpecialization(bool b);
    virtual void makeRelated();
    virtual void makeForeign();
    virtual void setInheritsDocsFrom(MemberDef *md);
    virtual void setTagInfo(TagInfo *i);
    virtual void setArgsString(const char *as);
    virtual void setReimplements(MemberDef *md);
    virtual void insertReimplementedBy(MemberDef *md);
    virtual void setRelatedAlso(ClassDef *cd);
    virtual void insertEnumField(MemberDef *md);
    virtual void setEnumScope(MemberDef *md,bool livesInsideEnum=FALSE);
    virtual void setEnumClassScope(ClassDef *cd);
    virtual void setDocumentedEnumValues(bool value);
    virtual void setAnonymousEnumType(const MemberDef *md);
    virtual bool addExample(const char *anchor,const char *name,const char *file);
    virtual void setPrototype(bool p,const QCString &df,int line, int column);
    virtual void setExplicitExternal(bool b,const QCString &df,int line,int column);
    virtual void setDeclFile(const QCString &df,int line,int column);
    virtual void setArgumentList(ArgumentList *al);
    virtual void setDeclArgumentList(ArgumentList *al);
    virtual void setDefinitionTemplateParameterLists(QList<ArgumentList> *lists);
    virtual void setTypeConstraints(ArgumentList *al);
    virtual void setType(const char *t);
    virtual void setAccessorType(ClassDef *cd,const char *t);
    virtual void setNamespace(NamespaceDef *nd);
    virtual void setMemberGroup(MemberGroup *grp);
    virtual void setMemberGroupId(int id);
    virtual void makeImplementationDetail();
    virtual void setFromAnonymousScope(bool b) const;
    virtual void setFromAnonymousMember(MemberDef *m);
    virtual void enableCallGraph(bool e);
    virtual void enableCallerGraph(bool e);
    virtual void enableReferencedByRelation(bool e);
    virtual void enableReferencesRelation(bool e);
    virtual void setTemplateMaster(MemberDef *mt);
    virtual void addListReference(Definition *d);
    virtual void setDocsForDefinition(bool b);
    virtual void setGroupAlias(const MemberDef *md);
    virtual void cacheTypedefVal(const ClassDef *val,const QCString &templSpec,const QCString &resolvedType);
    virtual void invalidateTypedefValCache();
    virtual void invalidateCachedArgumentTypes();
    virtual void setMemberDefinition(MemberDef *md);
    virtual void setMemberDeclaration(MemberDef *md);
    virtual void setAnonymousUsed() const;
    virtual void copyArgumentNames(MemberDef *bmd);
    virtual void setCategory(ClassDef *);
    virtual void setCategoryRelation(MemberDef *);
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine);
    virtual void setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine);
    virtual void setHidden(bool b);
    virtual void incrementFlowKeyWordCount();
    virtual void writeDeclaration(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
                   bool inGroup, const ClassDef *inheritFrom=0,const char *inheritId=0) const;
    virtual void writeDocumentation(const MemberList *ml,int memCount,int memTotal,OutputList &ol,
                            const char *scopeName,const Definition *container,
                            bool inGroup,bool showEnumValues=FALSE,bool
                            showInline=FALSE) const;
    virtual void writeMemberDocSimple(OutputList &ol,const Definition *container) const;
    virtual void writeEnumDeclaration(OutputList &typeDecl,
            const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd) const;
    virtual void writeTagFile(FTextStream &) const;
    virtual void warnIfUndocumented() const;
    virtual void warnIfUndocumentedParams() const;
    virtual void detectUndocumentedParams(bool hasParamCommand,bool hasReturnCommand) const;
    virtual MemberDef *createTemplateInstanceMember(ArgumentList *formalArgs,
               ArgumentList *actualArgs) const;
    virtual void findSectionsInDocumentation();
    virtual void writeLink(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
                   bool onlyText=FALSE) const;
    virtual void addToSearchIndex() const;

  private:
    void _computeLinkableInProject();
    void _computeIsConstructor();
    void _computeIsDestructor();
    void _writeGroupInclude(OutputList &ol,bool inGroup) const;
    void _writeCallGraph(OutputList &ol) const;
    void _writeCallerGraph(OutputList &ol) const;
    void _writeReimplements(OutputList &ol) const;
    void _writeReimplementedBy(OutputList &ol) const;
    void _writeExamples(OutputList &ol) const;
    void _writeTypeConstraints(OutputList &ol) const;
    void _writeEnumValues(OutputList &ol,const Definition *container,
                          const QCString &cfname,const QCString &ciname,
                          const QCString &cname) const;
    void _writeCategoryRelation(OutputList &ol) const;
    void _writeTagData(const DefType) const;

    static int s_indentLevel;

    // disable copying of member defs
    MemberDefImpl(const MemberDefImpl &);
    MemberDefImpl &operator=(const MemberDefImpl &);


    // PIMPL idiom
    class IMPL;
    IMPL *m_impl;
    uchar m_isLinkableCached;    // 0 = not cached, 1=FALSE, 2=TRUE
    uchar m_isConstructorCached; // 0 = not cached, 1=FALSE, 2=TRUE
    uchar m_isDestructorCached;  // 0 = not cached, 1=FALSE, 2=TRUE
};

MemberDef *createMemberDef(const char *defFileName,int defLine,int defColumn,
              const char *type,const char *name,const char *args,
              const char *excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList *tal,
              const ArgumentList *al,const char *metaData)
{
  return new MemberDefImpl(defFileName,defLine,defColumn,type,name,args,excp,prot,virt,
                           stat,related,t,tal,al,metaData);
}

//-----------------------------------------------------------------------------

class MemberDefAliasImpl : public DefinitionAliasImpl, public MemberDef
{
  public:
    MemberDefAliasImpl(const Definition *newScope,const MemberDef *md) 
    : DefinitionAliasImpl(newScope,md), m_memberGroup(0) {}
    virtual ~MemberDefAliasImpl() {}
    virtual DefType definitionType() const { return TypeMember; }

    const MemberDef *getMdAlias() const           { return dynamic_cast<const MemberDef*>(getAlias()); }
    virtual       MemberDef *resolveAlias()       { return const_cast<MemberDef*>(getMdAlias()); }
    virtual const MemberDef *resolveAlias() const { return getMdAlias(); }

    virtual MemberDef *deepCopy() const  {
      return createMemberDefAlias(getScope(),getMdAlias());
    }
    virtual void moveTo(Definition *) {}

    virtual QCString getOutputFileBase() const
    { return getMdAlias()->getOutputFileBase(); }
    virtual QCString getReference() const
    { return getMdAlias()->getReference(); }
    virtual QCString anchor() const
    { return getMdAlias()->anchor(); }
    virtual const char *declaration() const
    { return getMdAlias()->declaration(); }
    virtual const char *definition() const
    { return getMdAlias()->definition(); }
    virtual const char *typeString() const
    { return getMdAlias()->typeString(); }
    virtual const char *argsString() const
    { return getMdAlias()->argsString(); }
    virtual const char *excpString() const
    { return getMdAlias()->excpString(); }
    virtual const char *bitfieldString() const
    { return getMdAlias()->bitfieldString(); }
    virtual const char *extraTypeChars() const
    { return getMdAlias()->extraTypeChars(); }
    virtual const QCString &initializer() const
    { return getMdAlias()->initializer(); }
    virtual int initializerLines() const
    { return getMdAlias()->initializerLines(); }
    virtual uint64 getMemberSpecifiers() const
    { return getMdAlias()->getMemberSpecifiers(); }
    virtual const MemberList *getSectionList() const
    { return getMdAlias()->getSectionList(); }
    virtual QCString displayDefinition() const
    { return getMdAlias()->displayDefinition(); }
    virtual const ClassDef *getClassDef() const
    { return getMdAlias()->getClassDef(); }
    virtual const FileDef *getFileDef() const
    { return getMdAlias()->getFileDef(); }
    virtual const NamespaceDef* getNamespaceDef() const
    { return getMdAlias()->getNamespaceDef(); }
    virtual ClassDef *accessorClass() const
    { return getMdAlias()->accessorClass(); }
    virtual const char *getReadAccessor() const
    { return getMdAlias()->getReadAccessor(); }
    virtual const char *getWriteAccessor() const
    { return getMdAlias()->getWriteAccessor(); }
    virtual const GroupDef *getGroupDef() const
    { return getMdAlias()->getGroupDef(); }
    virtual Grouping::GroupPri_t getGroupPri() const
    { return getMdAlias()->getGroupPri(); }
    virtual const char *getGroupFileName() const
    { return getMdAlias()->getGroupFileName(); }
    virtual int getGroupStartLine() const
    { return getMdAlias()->getGroupStartLine(); }
    virtual bool getGroupHasDocs() const
    { return getMdAlias()->getGroupHasDocs(); }
    virtual QCString qualifiedName() const
    { return getMdAlias()->qualifiedName(); }
    virtual QCString objCMethodName(bool localLink,bool showStatic) const
    { return getMdAlias()->objCMethodName(localLink,showStatic); }
    virtual Protection protection() const
    { return getMdAlias()->protection(); }
    virtual Specifier virtualness(int count) const
    { return getMdAlias()->virtualness(); }
    virtual MemberType memberType() const
    { return getMdAlias()->memberType(); }
    virtual QCString memberTypeName() const
    { return getMdAlias()->memberTypeName(); }
    virtual bool isSignal() const
    { return getMdAlias()->isSignal(); }
    virtual bool isSlot() const
    { return getMdAlias()->isSlot(); }
    virtual bool isVariable() const
    { return getMdAlias()->isVariable(); }
    virtual bool isEnumerate() const
    { return getMdAlias()->isEnumerate(); }
    virtual bool isEnumValue() const
    { return getMdAlias()->isEnumValue(); }
    virtual bool isTypedef() const
    { return getMdAlias()->isTypedef(); }
    virtual bool isSequence() const
    { return getMdAlias()->isSequence(); }
    virtual bool isDictionary() const
    { return getMdAlias()->isDictionary(); }
    virtual bool isFunction() const
    { return getMdAlias()->isFunction(); }
    virtual bool isFunctionPtr() const
    { return getMdAlias()->isFunctionPtr(); }
    virtual bool isDefine() const
    { return getMdAlias()->isDefine(); }
    virtual bool isFriend() const
    { return getMdAlias()->isFriend(); }
    virtual bool isDCOP() const
    { return getMdAlias()->isDCOP(); }
    virtual bool isProperty() const
    { return getMdAlias()->isProperty(); }
    virtual bool isEvent() const
    { return getMdAlias()->isEvent(); }
    virtual bool isRelated() const
    { return getMdAlias()->isRelated(); }
    virtual bool isForeign() const
    { return getMdAlias()->isForeign(); }
    virtual bool isStatic() const
    { return getMdAlias()->isStatic(); }
    virtual bool isInline() const
    { return getMdAlias()->isInline(); }
    virtual bool isExplicit() const
    { return getMdAlias()->isExplicit(); }
    virtual bool isMutable() const
    { return getMdAlias()->isMutable(); }
    virtual bool isGettable() const
    { return getMdAlias()->isGettable(); }
    virtual bool isPrivateGettable() const
    { return getMdAlias()->isPrivateGettable(); }
    virtual bool isProtectedGettable() const
    { return getMdAlias()->isProtectedGettable(); }
    virtual bool isSettable() const
    { return getMdAlias()->isSettable(); }
    virtual bool isPrivateSettable() const
    { return getMdAlias()->isPrivateSettable(); }
    virtual bool isProtectedSettable() const
    { return getMdAlias()->isProtectedSettable(); }
    virtual bool isReadable() const
    { return getMdAlias()->isReadable(); }
    virtual bool isWritable() const
    { return getMdAlias()->isWritable(); }
    virtual bool isAddable() const
    { return getMdAlias()->isAddable(); }
    virtual bool isRemovable() const
    { return getMdAlias()->isRemovable(); }
    virtual bool isRaisable() const
    { return getMdAlias()->isRaisable(); }
    virtual bool isFinal() const
    { return getMdAlias()->isFinal(); }
    virtual bool isAbstract() const
    { return getMdAlias()->isAbstract(); }
    virtual bool isOverride() const
    { return getMdAlias()->isOverride(); }
    virtual bool isInitonly() const
    { return getMdAlias()->isInitonly(); }
    virtual bool isOptional() const
    { return getMdAlias()->isOptional(); }
    virtual bool isRequired() const
    { return getMdAlias()->isRequired(); }
    virtual bool isNonAtomic() const
    { return getMdAlias()->isNonAtomic(); }
    virtual bool isCopy() const
    { return getMdAlias()->isCopy(); }
    virtual bool isAssign() const
    { return getMdAlias()->isAssign(); }
    virtual bool isRetain() const
    { return getMdAlias()->isRetain(); }
    virtual bool isWeak() const
    { return getMdAlias()->isWeak(); }
    virtual bool isStrong() const
    { return getMdAlias()->isStrong(); }
    virtual bool isUnretained() const
    { return getMdAlias()->isUnretained(); }
    virtual bool isNew() const
    { return getMdAlias()->isNew(); }
    virtual bool isSealed() const
    { return getMdAlias()->isSealed(); }
    virtual bool isImplementation() const
    { return getMdAlias()->isImplementation(); }
    virtual bool isExternal() const
    { return getMdAlias()->isExternal(); }
    virtual bool isTypeAlias() const
    { return getMdAlias()->isTypeAlias(); }
    virtual bool isDefault() const
    { return getMdAlias()->isDefault(); }
    virtual bool isDelete() const
    { return getMdAlias()->isDelete(); }
    virtual bool isNoExcept() const
    { return getMdAlias()->isNoExcept(); }
    virtual bool isAttribute() const
    { return getMdAlias()->isAttribute(); }
    virtual bool isUNOProperty() const
    { return getMdAlias()->isUNOProperty(); }
    virtual bool isReadonly() const
    { return getMdAlias()->isReadable(); }
    virtual bool isBound() const
    { return getMdAlias()->isBound(); }
    virtual bool isConstrained() const
    { return getMdAlias()->isConstrained(); }
    virtual bool isTransient() const
    { return getMdAlias()->isTransient(); }
    virtual bool isMaybeVoid() const
    { return getMdAlias()->isMaybeVoid(); }
    virtual bool isMaybeDefault() const
    { return getMdAlias()->isMaybeDefault(); }
    virtual bool isMaybeAmbiguous() const
    { return getMdAlias()->isMaybeAmbiguous(); }
    virtual bool isPublished() const
    { return getMdAlias()->isPublished(); }
    virtual bool isTemplateSpecialization() const
    { return getMdAlias()->isTemplateSpecialization(); }
    virtual bool isObjCMethod() const
    { return getMdAlias()->isObjCMethod(); }
    virtual bool isObjCProperty() const
    { return getMdAlias()->isObjCProperty(); }
    virtual bool isConstructor() const
    { return getMdAlias()->isConstructor(); }
    virtual bool isDestructor() const
    { return getMdAlias()->isDestructor(); }
    virtual bool hasOneLineInitializer() const
    { return getMdAlias()->hasOneLineInitializer(); }
    virtual bool hasMultiLineInitializer() const
    { return getMdAlias()->hasMultiLineInitializer(); }
    virtual bool showInCallGraph() const
    { return getMdAlias()->showInCallGraph(); }
    virtual bool isStrongEnumValue() const
    { return getMdAlias()->isStrongEnumValue(); }
    virtual bool livesInsideEnum() const
    { return getMdAlias()->livesInsideEnum(); }
    virtual bool isSliceLocal() const
    { return getMdAlias()->isSliceLocal(); }
    virtual bool isConstExpr() const
    { return getMdAlias()->isConstExpr(); }
    virtual int  numberOfFlowKeyWords() const
    { return getMdAlias()->numberOfFlowKeyWords(); }
    virtual bool isFriendToHide() const
    { return getMdAlias()->isFriendToHide(); }
    virtual bool isNotFriend() const
    { return getMdAlias()->isNotFriend(); }
    virtual bool isFunctionOrSignalSlot() const
    { return getMdAlias()->isFunctionOrSignalSlot(); }
    virtual bool isRelatedOrFriend() const
    { return getMdAlias()->isRelatedOrFriend(); }
    virtual bool isLinkableInProject() const
    { return getMdAlias()->isLinkableInProject(); }
    virtual bool isLinkable() const
    { return getMdAlias()->isLinkable(); }
    virtual bool hasDocumentation() const
    { return getMdAlias()->hasDocumentation(); }
    virtual bool isDeleted() const
    { return getMdAlias()->isDeleted(); }
    virtual bool isBriefSectionVisible() const
    { return getMdAlias()->isBriefSectionVisible(); }
    virtual bool isDetailedSectionVisible(bool inGroup,bool inFile) const
    { return getMdAlias()->isDetailedSectionVisible(inGroup,inFile); }
    virtual bool isDetailedSectionLinkable() const
    { return getMdAlias()->isDetailedSectionLinkable(); }
    virtual bool isFriendClass() const
    { return getMdAlias()->isFriendClass(); }
    virtual bool isDocumentedFriendClass() const
    { return getMdAlias()->isDocumentedFriendClass(); }
    virtual MemberDef *reimplements() const
    { return getMdAlias()->reimplements(); }
    virtual MemberList *reimplementedBy() const
    { return getMdAlias()->reimplementedBy(); }
    virtual bool isReimplementedBy(const ClassDef *cd) const
    { return getMdAlias()->isReimplementedBy(cd); }
    virtual ClassDef *relatedAlso() const
    { return getMdAlias()->relatedAlso(); }
    virtual bool hasDocumentedEnumValues() const
    { return getMdAlias()->hasDocumentedEnumValues(); }
    virtual const MemberDef *getAnonymousEnumType() const
    { return getMdAlias()->getAnonymousEnumType(); }
    virtual bool isDocsForDefinition() const
    { return getMdAlias()->isDocsForDefinition(); }
    virtual const MemberDef *getEnumScope() const
    { return getMdAlias()->getEnumScope(); }
    virtual const MemberList *enumFieldList() const
    { return getMdAlias()->enumFieldList(); }
    virtual QCString enumBaseType() const
    { return getMdAlias()->enumBaseType(); }
    virtual bool hasExamples() const
    { return getMdAlias()->hasExamples(); }
    virtual ExampleSDict *getExamples() const
    { return getMdAlias()->getExamples(); }
    virtual bool isPrototype() const
    { return getMdAlias()->isPrototype(); }
    virtual const ArgumentList *argumentList() const
    { return getMdAlias()->argumentList(); }
    virtual const ArgumentList *declArgumentList() const
    { return getMdAlias()->declArgumentList(); }
    virtual const ArgumentList *templateArguments() const
    { return getMdAlias()->templateArguments(); }
    virtual const QList<ArgumentList> *definitionTemplateParameterLists() const
    { return getMdAlias()->definitionTemplateParameterLists(); }
    virtual int getMemberGroupId() const
    { return getMdAlias()->getMemberGroupId(); }
    virtual MemberGroup *getMemberGroup() const
    { return m_memberGroup; }
    virtual bool fromAnonymousScope() const
    { return getMdAlias()->fromAnonymousScope(); }
    virtual bool anonymousDeclShown() const
    { return getMdAlias()->anonymousDeclShown(); }
    virtual MemberDef *fromAnonymousMember() const
    { return getMdAlias()->fromAnonymousMember(); }
    virtual bool hasCallGraph() const
    { return getMdAlias()->hasCallGraph(); }
    virtual bool hasCallerGraph() const
    { return getMdAlias()->hasCallerGraph(); }
    virtual bool visibleMemberGroup(bool hideNoHeader) const
    { return getMdAlias()->visibleMemberGroup(hideNoHeader); }
    virtual bool hasReferencesRelation() const
    { return getMdAlias()->hasReferencesRelation(); }
    virtual bool hasReferencedByRelation() const
    { return getMdAlias()->hasReferencedByRelation(); }
    virtual MemberDef *templateMaster() const
    { return getMdAlias()->templateMaster(); }
    virtual QCString getScopeString() const
    { return getMdAlias()->getScopeString(); }
    virtual ClassDef *getClassDefOfAnonymousType() const
    { return getMdAlias()->getClassDefOfAnonymousType(); }
    virtual bool isTypedefValCached() const
    { return getMdAlias()->isTypedefValCached(); }
    virtual const ClassDef *getCachedTypedefVal() const
    { return getMdAlias()->getCachedTypedefVal(); }
    virtual QCString getCachedTypedefTemplSpec() const
    { return getMdAlias()->getCachedTypedefTemplSpec(); }
    virtual QCString getCachedResolvedTypedef() const
    { return getMdAlias()->getCachedResolvedTypedef(); }
    virtual MemberDef *memberDefinition() const
    { return getMdAlias()->memberDefinition(); }
    virtual MemberDef *memberDeclaration() const
    { return getMdAlias()->memberDeclaration(); }
    virtual MemberDef *inheritsDocsFrom() const
    { return getMdAlias()->inheritsDocsFrom(); }
    virtual const MemberDef *getGroupAlias() const
    { return getMdAlias()->getGroupAlias(); }
    virtual ClassDef *category() const
    { return getMdAlias()->category(); }
    virtual MemberDef *categoryRelation() const
    { return getMdAlias()->categoryRelation(); }
    virtual QCString displayName(bool b=TRUE) const
    { return getMdAlias()->displayName(b); }
    virtual QCString getDeclType() const
    { return getMdAlias()->getDeclType(); }
    virtual void getLabels(QStrList &sl,const Definition *container) const
    { return getMdAlias()->getLabels(sl,container); }
    virtual const ArgumentList *typeConstraints() const
    { return getMdAlias()->typeConstraints(); }
    virtual QCString documentation() const
    { return getMdAlias()->documentation(); }
    virtual QCString briefDescription(bool abbr=FALSE) const
    { return getMdAlias()->briefDescription(); }
    virtual QCString fieldType() const
    { return getMdAlias()->fieldType(); }
    virtual bool isReference() const
    { return getMdAlias()->isReference(); }
    virtual QCString getDeclFileName() const
    { return getMdAlias()->getDeclFileName(); }
    virtual int getDeclLine() const
    { return getMdAlias()->getDeclLine(); }
    virtual int getDeclColumn() const
    { return getMdAlias()->getDeclColumn(); }

    // non-const getters should not be called
    virtual ClassDef *getClassDef()
    { err("non-const getClassDef() called on aliased member. Please report as a bug.\n"); return 0; }
    virtual FileDef *getFileDef()
    { err("non-const getFileDef() called on aliased member. Please report as a bug.\n"); return 0; }
    virtual NamespaceDef* getNamespaceDef()
    { err("non-const getNamespaceDef() called on aliased member. Please report as a bug.\n"); return 0; }
    virtual GroupDef *getGroupDef()
    { err("non-const getGroupDef() called on aliased member. Please report as a bug.\n"); return 0; }
    virtual ArgumentList *argumentList()
    { err("non-const argumentList() called on aliased member. Please report as bug.\n"); return 0; }

    virtual void setEnumBaseType(const QCString &type) {}
    virtual void setMemberType(MemberType t) {}
    virtual void setDefinition(const char *d) {}
    virtual void setFileDef(FileDef *fd) {}
    virtual void setAnchor() {}
    virtual void setProtection(Protection p) {}
    virtual void setMemberSpecifiers(uint64 s) {}
    virtual void mergeMemberSpecifiers(uint64 s) {}
    virtual void setInitializer(const char *i) {}
    virtual void setBitfields(const char *s) {}
    virtual void setMaxInitLines(int lines) {}
    virtual void setMemberClass(ClassDef *cd) {}
    virtual void setSectionList(MemberList *sl) {}
    virtual void setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                     const QCString &fileName,int startLine,bool hasDocs,
                     MemberDef *member=0) {}
    virtual void setReadAccessor(const char *r) {}
    virtual void setWriteAccessor(const char *w) {}
    virtual void setTemplateSpecialization(bool b) {}
    virtual void makeRelated() {}
    virtual void makeForeign() {}
    virtual void setInheritsDocsFrom(MemberDef *md) {}
    virtual void setTagInfo(TagInfo *i) {}
    virtual void setArgsString(const char *as) {}
    virtual void setReimplements(MemberDef *md) {}
    virtual void insertReimplementedBy(MemberDef *md) {}
    virtual void setRelatedAlso(ClassDef *cd) {}
    virtual void insertEnumField(MemberDef *md) {}
    virtual void setEnumScope(MemberDef *md,bool livesInsideEnum=FALSE) {}
    virtual void setEnumClassScope(ClassDef *cd) {}
    virtual void setDocumentedEnumValues(bool value) {}
    virtual void setAnonymousEnumType(const MemberDef *md) {}
    virtual bool addExample(const char *anchor,const char *name,const char *file) { return FALSE; }
    virtual void setPrototype(bool p,const QCString &df,int line, int column) {}
    virtual void setExplicitExternal(bool b,const QCString &df,int line,int column) {}
    virtual void setDeclFile(const QCString &df,int line,int column) {}
    virtual void setArgumentList(ArgumentList *al) {}
    virtual void setDeclArgumentList(ArgumentList *al) {}
    virtual void setDefinitionTemplateParameterLists(QList<ArgumentList> *lists) {}
    virtual void setTypeConstraints(ArgumentList *al) {}
    virtual void setType(const char *t) {}
    virtual void setAccessorType(ClassDef *cd,const char *t) {}
    virtual void setNamespace(NamespaceDef *nd) {}
    virtual void setMemberGroup(MemberGroup *grp) { m_memberGroup = grp; }
    virtual void setMemberGroupId(int id) {}
    virtual void makeImplementationDetail() {}
    virtual void setFromAnonymousScope(bool b) const {}
    virtual void setFromAnonymousMember(MemberDef *m) {}
    virtual void enableCallGraph(bool e) {}
    virtual void enableCallerGraph(bool e) {}
    virtual void enableReferencedByRelation(bool e) {}
    virtual void enableReferencesRelation(bool e) {}
    virtual void setTemplateMaster(MemberDef *mt) {}
    virtual void addListReference(Definition *d) {}
    virtual void setDocsForDefinition(bool b) {}
    virtual void setGroupAlias(const MemberDef *md) {}
    virtual void cacheTypedefVal(const ClassDef *val,const QCString &templSpec,const QCString &resolvedType) {}
    virtual void invalidateTypedefValCache() {}
    virtual void invalidateCachedArgumentTypes() {}
    virtual void setMemberDefinition(MemberDef *md) {}
    virtual void setMemberDeclaration(MemberDef *md) {}
    virtual void setAnonymousUsed() const {}
    virtual void copyArgumentNames(MemberDef *bmd) {}
    virtual void setCategory(ClassDef *) {}
    virtual void setCategoryRelation(MemberDef *) {}
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE) {}
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine) {}
    virtual void setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine) {}
    virtual void setHidden(bool b) {}
    virtual void addToSearchIndex() const {}
    virtual void findSectionsInDocumentation() {}
    virtual MemberDef *createTemplateInstanceMember(ArgumentList *formalArgs,
               ArgumentList *actualArgs) const
    { return getMdAlias()->createTemplateInstanceMember(formalArgs,actualArgs); }
    virtual void incrementFlowKeyWordCount() {}

    virtual void writeDeclaration(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
                   bool inGroup, const ClassDef *inheritFrom=0,const char *inheritId=0) const
    {
      getMdAlias()->writeDeclaration(ol,cd,nd,fd,gd,inGroup,inheritFrom,inheritId);
    }
    virtual void writeEnumDeclaration(OutputList &typeDecl,
            const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd) const
    {
      getMdAlias()->writeEnumDeclaration(typeDecl,cd,nd,fd,gd);
    }
    virtual void writeDocumentation(const MemberList *ml,int memCount,int memTotal,OutputList &ol,
                            const char *scopeName,const Definition *container,
                            bool inGroup,bool showEnumValues=FALSE,bool
                            showInline=FALSE) const {}
    virtual void writeMemberDocSimple(OutputList &ol,const Definition *container) const {}
    virtual void writeLink(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
                   bool onlyText=FALSE) const {}
    virtual void writeTagFile(FTextStream &) const {}
    virtual void warnIfUndocumented() const {}
    virtual void warnIfUndocumentedParams() const {}
    virtual void detectUndocumentedParams(bool hasParamCommand,bool hasReturnCommand) const {}
  private:
    MemberGroup *m_memberGroup; // group's member definition
};


MemberDef *createMemberDefAlias(const Definition *newScope,const MemberDef *aliasMd)
{
  return new MemberDefAliasImpl(newScope,aliasMd);
}

//-----------------------------------------------------------------------------

int MemberDefImpl::s_indentLevel = 0;

//-----------------------------------------------------------------------------

static QCString addTemplateNames(const QCString &s,const QCString &n,const QCString &t)
{
  QCString result;
  QCString clRealName=n;
  int p=0,i;
  if ((i=clRealName.find('<'))!=-1)
  {
    clRealName=clRealName.left(i); // strip template specialization
  }
  if ((i=clRealName.findRev("::"))!=-1)
  {
    clRealName=clRealName.right(clRealName.length()-i-2);
  }
  while ((i=s.find(clRealName,p))!=-1)
  {
    result+=s.mid(p,i-p);
    uint j=clRealName.length()+i;
    if (s.length()==j || (s.at(j)!='<' && !isId(s.at(j))))
    { // add template names
      //printf("Adding %s+%s\n",clRealName.data(),t.data());
      result+=clRealName+t;
    }
    else
    { // template names already present
      //printf("Adding %s\n",clRealName.data());
      result+=clRealName;
    }
    p=i+clRealName.length();
  }
  result+=s.right(s.length()-p);
  //printf("addTemplateNames(%s,%s,%s)=%s\n",s.data(),n.data(),t.data(),result.data());
  return result;
}

// ol.startMemberDocName has already been done before this is called.
// when this function returns TRUE, ol.endParameterList will be called.
//
// typical sequence:
//   ol.startMemberDoc
//   ol.startMemberDocName
//   --- enter writeDefArgumentList
//   ol.endMemberDocName
//   ol.startParameterList
//     ...
//     ol.startParameterType(first=TRUE)
//     ol.endParameterType
//     ol.startParameterName
//     ol.endParameterName(last==FALSE)
//     ...
//     ol.startParameterType(first=FALSE)
//     ol.endParameterType
//     ol.startParameterName
//     ol.endParameterName(last==TRUE)
//     ...
//   --- leave writeDefArgumentList with return value TRUE
//   ol.endParameterList
//   ol.endMemberDoc(hasArgs=TRUE)
//
//  For an empty list the function should return FALSE, the sequence is
//   ol.startMemberDoc
//   ol.startMemberDocName
//   --- enter writeDefArgumentList
//   --- leave writeDefArgumentList with return value FALSE
//   ol.endMemberDocName
//   ol.endMemberDoc(hasArgs=FALSE);
//

static bool writeDefArgumentList(OutputList &ol,const Definition *scope,const MemberDef *md)
{
  const ArgumentList *defArgList=(md->isDocsForDefinition()) ?
                             md->argumentList() : md->declArgumentList();
  //printf("writeDefArgumentList '%s' isDocsForDefinition()=%d\n",md->name().data(),md->isDocsForDefinition());
  if (defArgList==0 || md->isProperty())
  {
    return FALSE; // member has no function like argument list
  }

  // simple argument list for tcl
  if (md->getLanguage()==SrcLangExt_Tcl)
  {
    if (defArgList->count()==0) return FALSE;
    ArgumentListIterator ali(*defArgList);
    Argument *a;
    ol.endMemberDocName();
    ol.startParameterList(FALSE);
    ol.startParameterType(TRUE,0);
    ol.endParameterType();
    ol.startParameterName(FALSE);
    for (;(a=ali.current());++ali)
    {
      if (a->defval.isEmpty())
      {
        ol.docify(a->name+" ");
      }
      else
      {
        ol.docify("?"+a->name+"? ");
      }
    }
    ol.endParameterName(TRUE,FALSE,FALSE);
    return TRUE;
  }

  if (!md->isDefine()) ol.docify(" ");

  //printf("writeDefArgList(%d)\n",defArgList->count());
  ol.pushGeneratorState();
  //ol.disableAllBut(OutputGenerator::Html);
  bool htmlOn  = ol.isEnabled(OutputGenerator::Html);
  bool latexOn = ol.isEnabled(OutputGenerator::Latex);
  bool docbookOn = ol.isEnabled(OutputGenerator::Docbook);
  {
    // html and latex
    if (htmlOn)  ol.enable(OutputGenerator::Html);
    if (latexOn) ol.enable(OutputGenerator::Latex);
    if (docbookOn) ol.enable(OutputGenerator::Docbook);

    ol.endMemberDocName();
    ol.startParameterList(!md->isObjCMethod());
  }
  ol.enableAll();
  ol.disable(OutputGenerator::Html);
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::Docbook);
  {
    // other formats
    if (!md->isObjCMethod()) ol.docify("("); // start argument list
    ol.endMemberDocName();
  }
  ol.popGeneratorState();
  //printf("===> name=%s isDefine=%d\n",md->name().data(),md->isDefine());

  QCString cName;
  if (scope)
  {
    cName=scope->name();
    int il=cName.find('<');
    int ir=cName.findRev('>');
    if (il!=-1 && ir!=-1 && ir>il)
    {
      cName=cName.mid(il,ir-il+1);
      //printf("1. cName=%s\n",cName.data());
    }
    else if (scope->definitionType()==Definition::TypeClass && (dynamic_cast<const ClassDef*>(scope))->templateArguments())
    {
      cName=tempArgListToString((dynamic_cast<const ClassDef*>(scope))->templateArguments(),scope->getLanguage());
      //printf("2. cName=%s\n",cName.data());
    }
    else // no template specifier
    {
      cName.resize(0);
    }
  }
  //printf("~~~ %s cName=%s\n",md->name().data(),cName.data());

  bool first=TRUE;
  bool paramTypeStarted=FALSE;
  bool isDefine = md->isDefine();
  ArgumentListIterator ali(*defArgList);
  Argument *a=ali.current();
  while (a)
  {
    if (isDefine || first)
    {
      ol.startParameterType(first,0);
      paramTypeStarted=TRUE;
      if (isDefine)
      {
        ol.endParameterType();
        ol.startParameterName(TRUE);
      }
    }
    QRegExp re(")("),res("(.*\\*");
    int vp=a->type.find(re);
    int wp=a->type.find(res);

    // use the following to put the function pointer type before the name
    bool hasFuncPtrType=FALSE;

    if (!a->attrib.isEmpty() && !md->isObjCMethod()) // argument has an IDL attribute
    {
      ol.docify(a->attrib+" ");
    }
    if (hasFuncPtrType) // argument type is a function pointer
    {
      //printf("a->type='%s' a->name='%s'\n",a->type.data(),a->name.data());
      QCString n=a->type.left(vp);
      if (hasFuncPtrType) n=a->type.left(wp);
      if (md->isObjCMethod()) { n.prepend("("); n.append(")"); }
      if (!cName.isEmpty()) n=addTemplateNames(n,scope->name(),cName);
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n);
    }
    else // non-function pointer type
    {
      QCString n=a->type;
      if (md->isObjCMethod()) { n.prepend("("); n.append(")"); }
      if (a->type!="...")
      {
        if (!cName.isEmpty()) n=addTemplateNames(n,scope->name(),cName);
        linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n);
      }
    }
    if (!isDefine)
    {
      if (paramTypeStarted)
      {
        ol.endParameterType();
        paramTypeStarted=FALSE;
      }
      ol.startParameterName(defArgList->count()<2);
    }
    if (hasFuncPtrType)
    {
      ol.docify(a->type.mid(wp,vp-wp));
    }
    if (!a->name.isEmpty() || a->type=="...") // argument has a name
    {
      //if (!hasFuncPtrType)
      //{
      //  ol.docify(" ");
      //}
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::Docbook);
      ol.disable(OutputGenerator::Html);
      ol.docify(" "); /* man page */
      if (htmlOn) ol.enable(OutputGenerator::Html);
      ol.disable(OutputGenerator::Man);
      ol.startEmphasis();
      ol.enable(OutputGenerator::Man);
      if (latexOn) ol.enable(OutputGenerator::Latex);
      if (docbookOn) ol.enable(OutputGenerator::Docbook);
      if (a->name.isEmpty()) ol.docify(a->type); else ol.docify(a->name);
      ol.disable(OutputGenerator::Man);
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::Docbook);
      ol.endEmphasis();
      ol.enable(OutputGenerator::Man);
      if (latexOn) ol.enable(OutputGenerator::Latex);
      if (docbookOn) ol.enable(OutputGenerator::Docbook);
    }
    if (!a->array.isEmpty())
    {
      ol.docify(a->array);
    }
    if (hasFuncPtrType) // write the part of the argument type
                        // that comes after the name
    {
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),
                  md,a->type.right(a->type.length()-vp));
    }
    if (!a->defval.isEmpty()) // write the default value
    {
      QCString n=a->defval;
      if (!cName.isEmpty()) n=addTemplateNames(n,scope->name(),cName);
      ol.docify(" = ");

      ol.startTypewriter();
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n,FALSE,TRUE,TRUE);
      ol.endTypewriter();

    }
    ++ali;
    a=ali.current();
    if (a)
    {
      if (!md->isObjCMethod()) ol.docify(", "); // there are more arguments
      if (!isDefine)
      {
        QCString key;
        if (md->isObjCMethod() && a->attrib.length()>=2)
        {
          //printf("Found parameter keyword %s\n",a->attrib.data());
          // strip [ and ]
          key=a->attrib.mid(1,a->attrib.length()-2);
          if (key!=",") key+=":"; // for normal keywords add colon
        }
        ol.endParameterName(FALSE,FALSE,!md->isObjCMethod());
        if (paramTypeStarted)
        {
          ol.endParameterType();
        }
        ol.startParameterType(FALSE,key);
        paramTypeStarted=TRUE;
      }
      else // isDefine
      {
        ol.endParameterName(FALSE,FALSE,TRUE);
      }
    }
    first=FALSE;
  }
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::Docbook);
  if (!md->isObjCMethod()) ol.docify(")"); // end argument list
  ol.enableAll();
  if (htmlOn) ol.enable(OutputGenerator::Html);
  if (latexOn) ol.enable(OutputGenerator::Latex);
  if (docbookOn) ol.enable(OutputGenerator::Docbook);
  if (first) ol.startParameterName(defArgList->count()<2);
  ol.endParameterName(TRUE,defArgList->count()<2,!md->isObjCMethod());
  ol.popGeneratorState();
  if (md->extraTypeChars())
  {
    ol.docify(md->extraTypeChars());
  }
  if (defArgList->constSpecifier)
  {
    ol.docify(" const");
  }
  if (defArgList->volatileSpecifier)
  {
    ol.docify(" volatile");
  }
  if (defArgList->refQualifier==RefQualifierLValue)
  {
    ol.docify(" &");
  }
  else if (defArgList->refQualifier==RefQualifierRValue)
  {
    ol.docify(" &&");
  }
  if (!defArgList->trailingReturnType.isEmpty())
  {
    linkifyText(TextGeneratorOLImpl(ol), // out
                scope,                   // scope
                md->getBodyDef(),        // fileScope
                md,                      // self
                defArgList->trailingReturnType, // text
                FALSE                    // autoBreak
               );

  }
  return TRUE;
}

static void writeExceptionListImpl(
        OutputList &ol, const ClassDef *cd, const MemberDef *md, QCString const& exception)
{
  // this is ordinary exception spec - there must be a '('
  //printf("exception='%s'\n",exception.data());
  int index = exception.find('(');
  if (index!=-1)
  {
    ol.exceptionEntry(exception.left(index),false);
    ++index; // paren in second column so skip it here
    for (int comma = exception.find(',', index); comma!=-1; )
    {
      ++comma; // include comma
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,
                  exception.mid(index,comma-index));
      ol.exceptionEntry(0,false);
      index=comma;
      comma = exception.find(',', index);
    }
    int close = exception.find(')', index);
    if (close!=-1)
    {
      QCString type=removeRedundantWhiteSpace(exception.mid(index,close-index));
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,type);
      ol.exceptionEntry(0,true);
    }
    else
    {
      warn(md->getDefFileName(),md->getDefLine(),
          "missing ) in exception list on member %s",qPrint(md->name()));
    }
  }
  else // Java Exception
  {
    ol.docify(" ");
    linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,exception);
  }
}

static void writeExceptionList(OutputList &ol, const ClassDef *cd, const MemberDef *md)
{
  QCString exception(QCString(md->excpString()).stripWhiteSpace());
  if ('{'==exception.at(0))
  {
    // this is an UNO IDL attribute - need special handling
    int index = exception.find(';');
    int oldIndex = 1;
    while (-1 != index) // there should be no more than 2 (set / get)
    {
      // omit '{' and ';' -> "set raises (...)"
      writeExceptionListImpl(ol,cd,md,exception.mid(oldIndex,index-oldIndex));
      oldIndex=index+1;
      index = exception.find(';',oldIndex);
    }
    // the rest is now just '}' - omit that
  }
  else
  {
    writeExceptionListImpl(ol,cd,md,exception);
  }
}

static void writeTemplatePrefix(OutputList &ol,const ArgumentList *al)
{
  ol.docify("template<");
  ArgumentListIterator ali(*al);
  Argument *a = ali.current();
  while (a)
  {
    ol.docify(a->type);
    ol.docify(" ");
    ol.docify(a->name);
    if (a->defval.length()!=0)
    {
      ol.docify(" = ");
      ol.docify(a->defval);
    }
    ++ali;
    a=ali.current();
    if (a) ol.docify(", ");
  }
  ol.docify("> ");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class MemberDefImpl::IMPL
{
  public:
    IMPL();
   ~IMPL();
    void init(Definition *def,const char *t,const char *a,const char *e,
              Protection p,Specifier v,bool s,Relationship r,
              MemberType mt,const ArgumentList *tal,
              const ArgumentList *al,const char *meta
             );

    ClassDef     *classDef;   // member of or related to
    FileDef      *fileDef;    // member of file definition
    NamespaceDef *nspace;     // the namespace this member is in.

    MemberDef  *enumScope;    // the enclosing scope, if this is an enum field
    bool        livesInsideEnum;
    const MemberDef  *annEnumType;  // the anonymous enum that is the type of this member
    MemberList *enumFields;   // enumeration fields

    MemberDef  *redefines;    // the members that this member redefines
    MemberList *redefinedBy;  // the list of members that redefine this one

    MemberDef  *memDef;       // member definition for this declaration
    MemberDef  *memDec;       // member declaration for this definition
    ClassDef   *relatedAlso;  // points to class marked by relatedAlso

    ExampleSDict *exampleSDict; // a dictionary of all examples for quick access

    QCString type;            // return actual type
    QCString accessorType;    // return type that tell how to get to this member
    ClassDef *accessorClass;  // class that this member accesses (for anonymous types)
    QCString args;            // function arguments/variable array specifiers
    QCString def;             // member definition in code (fully qualified name)
    QCString anc;             // HTML anchor name
    Specifier virt;           // normal/virtual/pure virtual
    Protection prot;          // protection type [Public/Protected/Private]
    QCString decl;            // member declaration in class

    QCString bitfields;       // struct member bitfields
    QCString read;            // property read accessor
    QCString write;           // property write accessor
    QCString exception;       // exceptions that can be thrown
    QCString initializer;     // initializer
    QCString extraTypeChars;  // extra type info found after the argument list
    QCString enumBaseType;    // base type of the enum (C++11)
    int initLines;            // number of lines in the initializer

    uint64  memSpec;          // The specifiers present for this member
    MemberType mtype;         // returns the kind of member
    int maxInitLines;         // when the initializer will be displayed
    int userInitLines;        // result of explicit \hideinitializer or \showinitializer
    MemberDef  *annMemb;

    ArgumentList *defArgList;    // argument list of this member definition
    ArgumentList *declArgList;   // argument list of this member declaration

    ArgumentList *tArgList;      // template argument list of function template
    ArgumentList *typeConstraints; // type constraints for template parameters
    MemberDef *templateMaster;
    QList<ArgumentList> *defTmpArgLists; // lists of template argument lists
                                         // (for template functions in nested template classes)

    QCString metaData;        // Slice metadata.

    ClassDef *cachedAnonymousType; // if the member has an anonymous compound
                                   // as its type then this is computed by
                                   // getClassDefOfAnonymousType() and
                                   // cached here.
    SDict<MemberList> *classSectionSDict; // not accessible

    const MemberDef *groupAlias;    // Member containing the definition
    int grpId;                // group id
    MemberGroup *memberGroup; // group's member definition
    GroupDef *group;          // group in which this member is in
    Grouping::GroupPri_t grouppri; // priority of this definition
    QCString groupFileName;   // file where this grouping was defined
    int groupStartLine;       // line  "      "      "     "     "
    MemberDef *groupMember;

    bool isTypedefValCached;
    const ClassDef *cachedTypedefValue;
    QCString cachedTypedefTemplSpec;
    QCString cachedResolvedType;

    // inbody documentation
    //int inbodyLine;
    //QCString inbodyFile;
    //QCString inbodyDocs;

    // documentation inheritance
    MemberDef *docProvider;

    // to store the output file base from tag files
    QCString explicitOutputFileBase;

    // objective-c
    bool implOnly; // function found in implementation but not
                     // in the interface
    mutable bool hasDocumentedParams;
    mutable bool hasDocumentedReturnType;
    bool isDMember;
    Relationship related;     // relationship of this to the class
    bool stat;                // is it a static function?
    bool proto;               // is it a prototype;
    bool docEnumValues;       // is an enum with documented enum values.

    mutable bool annScope;    // member is part of an anonymous scope
    mutable bool annUsed;     // ugly: needs to be mutable to allow setAnonymousUsed to act as a
                              // const member.
    bool hasCallGraph;
    bool hasCallerGraph;
    bool hasReferencedByRelation;
    bool hasReferencesRelation;
    bool explExt;             // member was explicitly declared external
    bool tspec;               // member is a template specialization
    bool groupHasDocs;        // true if the entry that caused the grouping was documented
    bool docsForDefinition;   // TRUE => documentation block is put before
                              //         definition.
                              // FALSE => block is put before declaration.
    ClassDef *category;
    MemberDef *categoryRelation;
    QCString declFileName;
    int declLine;
    int declColumn;
    int numberOfFlowKW;
};

MemberDefImpl::IMPL::IMPL() :
    enumFields(0),
    redefinedBy(0),
    exampleSDict(0),
    defArgList(0),
    declArgList(0),
    tArgList(0),
    typeConstraints(0),
    defTmpArgLists(0),
    classSectionSDict(0),
    category(0),
    categoryRelation(0),
    declLine(-1),
    declColumn(-1),
    numberOfFlowKW(0)
{
}

MemberDefImpl::IMPL::~IMPL()
{
  delete redefinedBy;
  delete exampleSDict;
  delete enumFields;
  delete defArgList;
  delete tArgList;
  delete typeConstraints;
  delete defTmpArgLists;
  delete classSectionSDict;
  delete declArgList;
}

void MemberDefImpl::IMPL::init(Definition *def,
                     const char *t,const char *a,const char *e,
                     Protection p,Specifier v,bool s,Relationship r,
                     MemberType mt,const ArgumentList *tal,
                     const ArgumentList *al,const char *meta
                    )
{
  classDef=0;
  fileDef=0;
  redefines=0;
  relatedAlso=0;
  redefinedBy=0;
  accessorClass=0;
  nspace=0;
  memDef=0;
  memDec=0;
  group=0;
  grpId=-1;
  exampleSDict=0;
  enumFields=0;
  enumScope=0;
  livesInsideEnum=FALSE;
  defTmpArgLists=0;
  hasCallGraph = FALSE;
  hasCallerGraph = FALSE;
  hasReferencedByRelation = FALSE;
  hasReferencesRelation = FALSE;
  initLines=0;
  type=t;
  if (mt==MemberType_Typedef) type.stripPrefix("typedef ");
  //  type.stripPrefix("struct ");
  //  type.stripPrefix("class " );
  //  type.stripPrefix("union " );
  type=removeRedundantWhiteSpace(type);
  args=a;
  args=removeRedundantWhiteSpace(args);
  if (type.isEmpty()) decl=def->name()+args; else decl=type+" "+def->name()+args;

  memberGroup=0;
  virt=v;
  prot=p;
  related=r;
  stat=s;
  mtype=mt;
  exception=e;
  proto=FALSE;
  annScope=FALSE;
  memSpec=0;
  annMemb=0;
  annUsed=FALSE;
  annEnumType=0;
  groupAlias=0;
  explExt=FALSE;
  tspec=FALSE;
  cachedAnonymousType=0;
  maxInitLines=Config_getInt(MAX_INITIALIZER_LINES);
  userInitLines=-1;
  docEnumValues=FALSE;
  // copy function template arguments (if any)
  if (tal)
  {
    tArgList = tal->deepCopy();
  }
  else
  {
    tArgList=0;
  }
  //printf("new member al=%p\n",al);
  // copy function definition arguments (if any)
  if (al)
  {
    defArgList = al->deepCopy();
  }
  else
  {
    defArgList=0;
  }
  // convert function declaration arguments (if any)
  if (!args.isEmpty())
  {
    declArgList = new ArgumentList;
    stringToArgumentList(args,declArgList,&extraTypeChars);
    //printf("setDeclArgList %s to %s const=%d\n",args.data(),
    //    argListToString(declArgList).data(),declArgList->constSpecifier);
  }
  else
  {
    declArgList = 0;
  }
  metaData = meta;
  templateMaster = 0;
  classSectionSDict = 0;
  docsForDefinition = TRUE;
  isTypedefValCached = FALSE;
  cachedTypedefValue = 0;
  //inbodyLine = -1;
  implOnly=FALSE;
  groupMember = 0;
  hasDocumentedParams = FALSE;
  hasDocumentedReturnType = FALSE;
  docProvider = 0;
  isDMember = def->getDefFileName().right(2).lower()==".d";
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*! Creates a new member definition.
 *
 * \param df File containing the definition of this member.
 * \param dl Line at which the member definition was found.
 * \param dc Column at which the member definition was found.
 * \param t  A string representing the type of the member.
 * \param na A string representing the name of the member.
 * \param a  A string representing the arguments of the member.
 * \param e  A string representing the throw clause of the members.
 * \param p  The protection context of the member, possible values are:
 *           \c Public, \c Protected, \c Private.
 * \param v  The degree of 'virtualness' of the member, possible values are:
 *           \c Normal, \c Virtual, \c Pure.
 * \param s  A boolean that is true iff the member is static.
 * \param r  The relationship between the class and the member.
 * \param mt The kind of member. See #MemberType for a list of
 *           all types.
 * \param tal The template arguments of this member.
 * \param al  The arguments of this member. This is a structured form of
 *            the string past as argument \a a.
 * \param meta Slice metadata.
 */

MemberDefImpl::MemberDefImpl(const char *df,int dl,int dc,
                     const char *t,const char *na,const char *a,const char *e,
                     Protection p,Specifier v,bool s,Relationship r,MemberType mt,
                     const ArgumentList *tal,const ArgumentList *al,const char *meta
                    ) : DefinitionImpl(df,dl,dc,removeRedundantWhiteSpace(na))
{
  //printf("MemberDefImpl::MemberDef(%s)\n",na);
  m_impl = new MemberDefImpl::IMPL;
  m_impl->init(this,t,a,e,p,v,s,r,mt,tal,al,meta);
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDefImpl::MemberDefImpl(const MemberDefImpl &md) : DefinitionImpl(md)
{
  m_impl = new MemberDefImpl::IMPL;
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDef *MemberDefImpl::deepCopy() const
{
  //MemberDef *result = new MemberDef(getDefFileName(),getDefLine(),name());
  MemberDefImpl *result = new MemberDefImpl(*this);
  // first copy everything by reference
  *result->m_impl = *m_impl;
  // clear pointers owned by object
  result->m_impl->redefinedBy= 0;
  result->m_impl->exampleSDict=0;
  result->m_impl->enumFields=0;
  result->m_impl->defArgList=0;
  result->m_impl->tArgList=0;
  result->m_impl->typeConstraints=0;
  result->m_impl->defTmpArgLists=0;
  result->m_impl->classSectionSDict=0;
  result->m_impl->declArgList=0;
  // replace pointers owned by the object by deep copies
  if (m_impl->redefinedBy)
  {
    MemberListIterator mli(*m_impl->redefinedBy);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      result->insertReimplementedBy(md);
    }
  }
  if (m_impl->exampleSDict)
  {
    ExampleSDict::Iterator it(*m_impl->exampleSDict);
    Example *e;
    for (it.toFirst();(e=it.current());++it)
    {
      result->addExample(e->anchor,e->name,e->file);
    }
  }
  if (m_impl->enumFields)
  {
    MemberListIterator mli(*m_impl->enumFields);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      result->insertEnumField(md);
    }
  }
  if (m_impl->defArgList)
  {
    result->m_impl->defArgList = m_impl->defArgList->deepCopy();
  }
  if (m_impl->tArgList)
  {
    result->m_impl->tArgList = m_impl->tArgList->deepCopy();
  }
  if (m_impl->typeConstraints)
  {
    result->m_impl->typeConstraints = m_impl->typeConstraints->deepCopy();
  }
  result->setDefinitionTemplateParameterLists(m_impl->defTmpArgLists);
  if (m_impl->classSectionSDict)
  {
    result->m_impl->classSectionSDict = new SDict<MemberList>(7);
    SDict<MemberList>::IteratorDict it(*m_impl->classSectionSDict);
    MemberList *ml;
    for (it.toFirst();(ml=it.current());++it)
    {
      result->m_impl->classSectionSDict->append(it.currentKey(),ml);
    }
  }
  if (m_impl->declArgList)
  {
    result->m_impl->declArgList = m_impl->declArgList->deepCopy();
  }
  return result;
}

void MemberDefImpl::moveTo(Definition *scope)
{
   setOuterScope(scope);
   if (scope->definitionType()==Definition::TypeClass)
   {
     m_impl->classDef = dynamic_cast<ClassDef*>(scope);
   }
   else if (scope->definitionType()==Definition::TypeFile)
   {
     m_impl->fileDef = dynamic_cast<FileDef*>(scope);
   }
   else if (scope->definitionType()==Definition::TypeNamespace)
   {
     m_impl->nspace = dynamic_cast<NamespaceDef*>(scope);
   }
   m_isLinkableCached = 0;
   m_isConstructorCached = 0;
}


/*! Destroys the member definition. */
MemberDefImpl::~MemberDefImpl()
{
  delete m_impl;
  //printf("%p: ~MemberDef()\n",this);
  m_impl=0;
}

void MemberDefImpl::setReimplements(MemberDef *md)
{
  m_impl->redefines = md;
}

void MemberDefImpl::insertReimplementedBy(MemberDef *md)
{
  if (m_impl->templateMaster)
  {
    m_impl->templateMaster->insertReimplementedBy(md);
  }
  if (m_impl->redefinedBy==0) m_impl->redefinedBy = new MemberList(MemberListType_redefinedBy);
  if (m_impl->redefinedBy->findRef(md)==-1)
  {
    m_impl->redefinedBy->inSort(md);
  }
}

MemberDef *MemberDefImpl::reimplements() const
{
  return m_impl->redefines;
}

MemberList *MemberDefImpl::reimplementedBy() const
{
  return m_impl->redefinedBy;
}

bool MemberDefImpl::isReimplementedBy(const ClassDef *cd) const
{
  if (cd && m_impl->redefinedBy)
  {
    MemberListIterator mi(*m_impl->redefinedBy);
    MemberDef *md;
    for (mi.toFirst();(md=mi.current());++mi)
    {
      const ClassDef *mcd = md->getClassDef();
      if (mcd)
      {
        if (cd==mcd || cd->isBaseClass(mcd,TRUE))
        {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

void MemberDefImpl::insertEnumField(MemberDef *md)
{
  if (m_impl->enumFields==0) m_impl->enumFields=new MemberList(MemberListType_enumFields);
  m_impl->enumFields->append(md);
}

bool MemberDefImpl::addExample(const char *anchor,const char *nameStr,
                           const char *file)
{
  //printf("%s::addExample(%s,%s,%s)\n",name().data(),anchor,nameStr,file);
  if (m_impl->exampleSDict==0) m_impl->exampleSDict = new ExampleSDict;
  if (m_impl->exampleSDict->find(nameStr)==0)
  {
    //printf("Add reference to example %s to member %s\n",nameStr,name.data());
    Example *e=new Example;
    e->anchor=anchor;
    e->name=nameStr;
    e->file=file;
    m_impl->exampleSDict->inSort(nameStr,e);
    return TRUE;
  }
  return FALSE;
}

bool MemberDefImpl::hasExamples() const
{
  if (m_impl->exampleSDict==0)
    return FALSE;
  else
    return m_impl->exampleSDict->count()>0;
}

QCString MemberDefImpl::getOutputFileBase() const
{
  static bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  static bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  QCString baseName;

  //printf("Member: %s: templateMaster=%p group=%p classDef=%p nspace=%p fileDef=%p\n",
  //    name().data(),m_impl->templateMaster,m_impl->group,m_impl->classDef,
  //    m_impl->nspace,m_impl->fileDef);
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  const ClassDef *classDef = getClassDef();
  const GroupDef *groupDef = getGroupDef();
  if (!m_impl->explicitOutputFileBase.isEmpty())
  {
    return m_impl->explicitOutputFileBase;
  }
  else if (templateMaster())
  {
    return templateMaster()->getOutputFileBase();
  }
  else if (groupDef)
  {
    baseName=groupDef->getOutputFileBase();
  }
  else if (classDef)
  {
    baseName=classDef->getOutputFileBase();
    if (inlineSimpleClasses && classDef->isSimple())
    {
      return baseName;
    }
  }
  else if (nspace && nspace->isLinkableInProject())
  {
    baseName=nspace->getOutputFileBase();
  }
  else if (fileDef)
  {
    baseName=fileDef->getOutputFileBase();
  }

  if (baseName.isEmpty())
  {
    warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: member %s does not belong to any"
       " container!",qPrint(name())
      );
    return "dummy";
  }
  else if (separateMemberPages && isDetailedSectionLinkable())
  {
    if (getEnumScope()) // enum value, which is part of enum's documentation
    {
      baseName+="_"+getEnumScope()->anchor();
    }
    else
    {
      baseName+="_"+anchor();
    }
  }
  return baseName;
}

QCString MemberDefImpl::getReference() const
{
  QCString ref = DefinitionImpl::getReference();
  if (!ref.isEmpty())
  {
    return ref;
  }
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  const ClassDef *classDef = getClassDef();
  const GroupDef *groupDef = getGroupDef();
  if (templateMaster())
  {
    return templateMaster()->getReference();
  }
  else if (groupDef)
  {
    return groupDef->getReference();
  }
  else if (classDef)
  {
    return classDef->getReference();
  }
  else if (nspace)
  {
    return nspace->getReference();
  }
  else if (fileDef)
  {
    return fileDef->getReference();
  }
  return "";
}

QCString MemberDefImpl::anchor() const
{
  QCString result=m_impl->anc;
  if (m_impl->groupAlias)     return m_impl->groupAlias->anchor();
  if (m_impl->templateMaster) return m_impl->templateMaster->anchor();
  if (m_impl->enumScope && m_impl->enumScope!=this) // avoid recursion for C#'s public enum E { E, F }
  {
    result.prepend(m_impl->enumScope->anchor());
  }
  if (getGroupDef())
  {
    if (m_impl->groupMember)
    {
      result=m_impl->groupMember->anchor();
    }
    else if (getReference().isEmpty())
    {
      result.prepend("g");
    }
  }
  return result;
}

void MemberDefImpl::_computeLinkableInProject()
{
  static bool extractStatic  = Config_getBool(EXTRACT_STATIC);
  static bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
  m_isLinkableCached = 2; // linkable
  //printf("MemberDefImpl::isLinkableInProject(name=%s)\n",name().data());
  if (isHidden())
  {
    //printf("is hidden\n");
    m_isLinkableCached = 1;
    return;
  }
  if (templateMaster())
  {
    //printf("has template master\n");
    m_isLinkableCached = templateMaster()->isLinkableInProject() ? 2 : 1;
    return;
  }
  if (name().isEmpty() || name().at(0)=='@')
  {
    //printf("name invalid\n");
    m_isLinkableCached = 1; // not a valid or a dummy name
    return;
  }
  if (!hasDocumentation() || isReference())
  {
    //printf("no docs or reference\n");
    m_isLinkableCached = 1; // no documentation
    return;
  }
  const GroupDef *groupDef = getGroupDef();
  const ClassDef *classDef = getClassDef();
  if (groupDef && !groupDef->isLinkableInProject())
  {
    //printf("group but group not linkable!\n");
    m_isLinkableCached = 1; // group but group not linkable
    return;
  }
  if (!groupDef && classDef && !classDef->isLinkableInProject())
  {
    //printf("in a class but class not linkable!\n");
    m_isLinkableCached = 1; // in class but class not linkable
    return;
  }
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  if (!groupDef && nspace && !m_impl->related && !nspace->isLinkableInProject()
      && (fileDef==0 || !fileDef->isLinkableInProject()))
  {
    //printf("in a namespace but namespace not linkable!\n");
    m_isLinkableCached = 1; // in namespace but namespace not linkable
    return;
  }
  if (!groupDef && !nspace &&
      !m_impl->related && !classDef &&
      fileDef && !fileDef->isLinkableInProject())
  {
    //printf("in a file but file not linkable!\n");
    m_isLinkableCached = 1; // in file (and not in namespace) but file not linkable
    return;
  }
  if ((!protectionLevelVisible(m_impl->prot) && m_impl->mtype!=MemberType_Friend) &&
       !(m_impl->prot==Private && m_impl->virt!=Normal && extractPrivateVirtual))
  {
    //printf("private and invisible!\n");
    m_isLinkableCached = 1; // hidden due to protection
    return;
  }
  if (m_impl->stat && classDef==0 && !extractStatic)
  {
    //printf("static and invisible!\n");
    m_isLinkableCached = 1; // hidden due to staticness
    return;
  }
  //printf("linkable!\n");
  return; // linkable!
}

void MemberDefImpl::setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace)
{
  DefinitionImpl::setDocumentation(d,docFile,docLine,stripWhiteSpace);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setBriefDescription(const char *b,const char *briefFile,int briefLine)
{
  DefinitionImpl::setBriefDescription(b,briefFile,briefLine);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine)
{
  DefinitionImpl::setInbodyDocumentation(d,inbodyFile,inbodyLine);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setHidden(bool b)
{
  DefinitionImpl::setHidden(b);
  m_isLinkableCached = 0;
}

bool MemberDefImpl::isLinkableInProject() const
{
  if (m_isLinkableCached==0)
  {
    MemberDefImpl *that = (MemberDefImpl*)this;
    that->_computeLinkableInProject();
  }
  ASSERT(m_isLinkableCached>0);
  return m_isLinkableCached==2;
}

bool MemberDefImpl::isLinkable() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkable();
  }
  else
  {
    return isLinkableInProject() || isReference();
  }
}


void MemberDefImpl::setDefinitionTemplateParameterLists(QList<ArgumentList> *lists)
{
  if (lists)
  {
    if (m_impl->defTmpArgLists) delete m_impl->defTmpArgLists;
    m_impl->defTmpArgLists = copyArgumentLists(lists);
  }
}

void MemberDefImpl::writeLink(OutputList &ol,
                      const ClassDef *,const NamespaceDef *,const FileDef *fd,const GroupDef *gd,
                      bool onlyText) const
{
  SrcLangExt lang = getLanguage();
  static bool hideScopeNames     = Config_getBool(HIDE_SCOPE_NAMES);
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  QCString n = name();
  const ClassDef *classDef = getClassDef();
  const NamespaceDef *nspace = getNamespaceDef();
  if (!hideScopeNames)
  {
    if (m_impl->enumScope && m_impl->livesInsideEnum)
    {
      n.prepend(m_impl->enumScope->displayName()+sep);
    }
    if (classDef && gd && !isRelated())
    {
      n.prepend(classDef->displayName()+sep);
    }
    else if (nspace && (gd || fd))
    {
      n.prepend(nspace->displayName()+sep);
    }
  }

  if (isObjCMethod())
  {
    if (isStatic()) ol.docify("+ "); else ol.docify("- ");
  }
  if (!onlyText && isLinkable()) // write link
  {
    if (m_impl->mtype==MemberType_EnumValue && getGroupDef()==0 &&          // enum value is not grouped
        getEnumScope() && getEnumScope()->getGroupDef()) // but its container is
    {
      const GroupDef *enumValGroup = getEnumScope()->getGroupDef();
      ol.writeObjectLink(enumValGroup->getReference(),
                         enumValGroup->getOutputFileBase(),
                         anchor(),n);
    }
    else
    {
      ol.writeObjectLink(getReference(),getOutputFileBase(),anchor(),n);
    }
  }
  else // write only text
  {
    ol.startBold();
    ol.docify(n);
    ol.endBold();
  }
}

/*! If this member has an anonymous class/struct/union as its type, then
 *  this method will return the ClassDef that describes this return type.
 */
ClassDef *MemberDefImpl::getClassDefOfAnonymousType() const
{
  if (m_impl->cachedAnonymousType) return m_impl->cachedAnonymousType;

  QCString cname;
  if (getClassDef()!=0)
  {
    cname=getClassDef()->name();
  }
  else if (getNamespaceDef()!=0)
  {
    cname=getNamespaceDef()->name();
  }
  QCString ltype(m_impl->type);
  // strip 'static' keyword from ltype
  //if (ltype.left(7)=="static ") ltype=ltype.right(ltype.length()-7);
  // strip 'friend' keyword from ltype
  ltype.stripPrefix("friend ");
  static QRegExp r("@[0-9]+");
  int l,i=r.match(ltype,0,&l);
  //printf("ltype='%s' i=%d\n",ltype.data(),i);
  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=0;
  if (i!=-1) // found anonymous scope in type
  {
    int il=i-1,ir=i+l;
    // extract anonymous scope
    while (il>=0 && (isId(ltype.at(il)) || ltype.at(il)==':' || ltype.at(il)=='@')) il--;
    if (il>0) il++; else if (il<0) il=0;
    while (ir<(int)ltype.length() && (isId(ltype.at(ir)) || ltype.at(ir)==':' || ltype.at(ir)=='@')) ir++;

    QCString annName = ltype.mid(il,ir-il);

    // if inside a class or namespace try to prepend the scope name
    if (!cname.isEmpty() && annName.left(cname.length()+2)!=cname+"::")
    {
      QCString ts=stripAnonymousNamespaceScope(cname+"::"+annName);
      annoClassDef=getClass(ts);
    }
    // if not found yet, try without scope name
    if (annoClassDef==0)
    {
      QCString ts=stripAnonymousNamespaceScope(annName);
      annoClassDef=getClass(ts);
    }
  }
  m_impl->cachedAnonymousType = annoClassDef;
  return annoClassDef;
}

/*! This methods returns TRUE iff the brief section (also known as
 *  declaration section) is visible in the documentation.
 */
bool MemberDefImpl::isBriefSectionVisible() const
{
  static bool extractStatic       = Config_getBool(EXTRACT_STATIC);
  static bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
  static bool hideUndocMembers    = Config_getBool(HIDE_UNDOC_MEMBERS);
  static bool briefMemberDesc     = Config_getBool(BRIEF_MEMBER_DESC);
  static bool repeatBrief         = Config_getBool(REPEAT_BRIEF);
  static bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);

  //printf("Member %s grpId=%d docs=%s file=%s args=%s\n",
  //    name().data(),
  //    0,"", //grpId,grpId==-1?"<none>":Doxygen::memberDocDict[grpId]->data(),
  //    "", //getFileDef()->name().data(),
  //    argsString());

  MemberGroupInfo *info = Doxygen::memGrpInfoDict[m_impl->grpId];
  //printf("name=%s m_impl->grpId=%d info=%p\n",name().data(),m_impl->grpId,info);
  //QCString *pMemGrp = Doxygen::memberDocDict[grpId];
  bool hasDocs = hasDocumentation() ||
                  // part of a documented member group
                 (m_impl->grpId!=-1 && info && !(info->doc.isEmpty() && info->header.isEmpty()));

  // only include static members with file/namespace scope if
  // explicitly enabled in the config file
  bool visibleIfStatic = !(getClassDef()==0 &&
                           isStatic() &&
                           !extractStatic
                          );

  // only include members is the are documented or
  // HIDE_UNDOC_MEMBERS is NO in the config file
  bool visibleIfDocumented = (!hideUndocMembers ||
                              hasDocs ||
                              isDocumentedFriendClass()
                             );

  // hide members with no detailed description and brief descriptions
  // explicitly disabled.
  bool visibleIfEnabled = !(hideUndocMembers &&
                            documentation().isEmpty() &&
                            !briefMemberDesc &&
                            !repeatBrief
                           );

  // Hide friend (class|struct|union) declarations if HIDE_FRIEND_COMPOUNDS is true
  bool visibleIfFriendCompound = !(hideFriendCompounds &&
                                   isFriend() &&
                                   (m_impl->type=="friend class" ||
                                    m_impl->type=="friend struct" ||
                                    m_impl->type=="friend union"
                                   )
                                  );

  // only include members that are non-private unless EXTRACT_PRIVATE is
  // set to YES or the member is part of a group. And as a special case,
  // private *documented* virtual members are shown if EXTRACT_PRIV_VIRTUAL
  // is set to YES
  bool visibleIfPrivate = (protectionLevelVisible(protection()) ||
                           m_impl->mtype==MemberType_Friend ||
                           (m_impl->prot==Private && m_impl->virt!=Normal && extractPrivateVirtual && hasDocs)
                          );

  // hide member if it overrides a member in a superclass and has no
  // documentation of its own
  //bool visibleIfDocVirtual = !reimplements() ||
  //                           !Config_getBool(INHERIT_DOCS) ||
  //                           hasDocs;

  // true if this member is a constructor or destructor
  bool cOrDTor = isConstructor() || isDestructor();

  // hide default constructors or destructors (no args) without
  // documentation
  bool visibleIfNotDefaultCDTor = !(cOrDTor &&
                                   m_impl->defArgList &&
                                   (m_impl->defArgList->isEmpty() ||
                                    m_impl->defArgList->getFirst()->type == "void"
                                   ) &&
                                   !hasDocs
                                  );


  //printf("visibleIfStatic=%d visibleIfDocumented=%d visibleIfEnabled=%d "
  //       "visibleIfPrivate=%d visibltIfNotDefaultCDTor=%d "
  //       "visibleIfFriendCompound=%d !annScope=%d\n",
  //       visibleIfStatic,visibleIfDocumented,
  //       visibleIfEnabled,visibleIfPrivate,visibleIfNotDefaultCDTor,
  //       visibleIfFriendCompound,!m_impl->annScope);

  bool visible = visibleIfStatic     && visibleIfDocumented      &&
                 visibleIfEnabled    && visibleIfPrivate         &&
                 /*visibleIfDocVirtual &&*/ visibleIfNotDefaultCDTor &&
                 visibleIfFriendCompound &&
                 !m_impl->annScope && !isHidden();
  //printf("MemberDefImpl::isBriefSectionVisible() %d\n",visible);
  return visible;
}

QCString MemberDefImpl::getDeclType() const
{
  QCString ltype(m_impl->type);
  if (isTypedef() && getLanguage() != SrcLangExt_Slice)
  {
    ltype.prepend("typedef ");
  }
  if (isTypeAlias())
  {
    ltype="using";
  }
  // strip 'friend' keyword from ltype
  ltype.stripPrefix("friend ");
  if (ltype=="@") // rename type from enum values
  {
    ltype="";
  }
  else
  {
    if (isObjCMethod())
    {
      ltype.prepend("(");
      ltype.append(")");
    }
  }
  return ltype;
}

void MemberDefImpl::writeDeclaration(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               bool inGroup, const ClassDef *inheritedFrom,const char *inheritId) const
{
  //printf("%s MemberDefImpl::writeDeclaration() inGroup=%d\n",qualifiedName().data(),inGroup);

  // hide enum value, since they appear already as part of the enum, unless they
  // are explicitly grouped.
  if (!inGroup && m_impl->mtype==MemberType_EnumValue) return;


  const Definition *d=0;
  ASSERT (cd!=0 || nd!=0 || fd!=0 || gd!=0); // member should belong to something
  if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;
  if (d==gd) // see bug 753608
  {
    if (getClassDef())          d = getClassDef();
    else if (getNamespaceDef()) d = getNamespaceDef();
    else if (getFileDef())      d = getFileDef();
  }

  addToSearchIndex();

  QCString cname  = d->name();
  QCString cdname = d->displayName();
  QCString cfname = getOutputFileBase();

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=getClassDefOfAnonymousType();

  ol.startMemberDeclaration();

  // start a new member declaration
  bool isAnonymous = annoClassDef || m_impl->annMemb || m_impl->annEnumType;
  ///printf("startMemberItem for %s\n",name().data());
  ol.startMemberItem(anchor(),
                     isAnonymous ? 1 : m_impl->tArgList ? 3 : 0,
                     inheritId
                    );

  // If there is no detailed description we need to write the anchor here.
  bool detailsVisible = isDetailedSectionLinkable();
  bool writeAnchor = (inGroup || getGroupDef()==0) &&     // only write anchors for member that have no details and are
                     !detailsVisible && !m_impl->annMemb; // rendered inside the group page or are not grouped at all
  if (writeAnchor)
  {
    QCString doxyArgs=argsString();
    QCString doxyName=name();
    if (!cname.isEmpty())
    {
      doxyName.prepend(cdname+getLanguageSpecificSeparator(getLanguage()));
    }
    ol.startDoxyAnchor(cfname,cname,anchor(),doxyName,doxyArgs);
  }

  if (!detailsVisible)
  {
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::Docbook);
    ol.docify("\n");
    ol.popGeneratorState();
  }

  if (annoClassDef || m_impl->annMemb)
  {
    int j;
    for (j=0;j<s_indentLevel;j++)
    {
      ol.writeNonBreakableSpace(3);
    }
  }

  // *** write template lists
  if (m_impl->tArgList && getLanguage()==SrcLangExt_Cpp)
  {
    if (!isAnonymous) ol.startMemberTemplateParams();
    writeTemplatePrefix(ol,m_impl->tArgList);
    if (!isAnonymous) ol.endMemberTemplateParams(anchor(),inheritId);
  }

  // *** write type
  QCString ltype(m_impl->type);
  if (isTypedef() && getLanguage() != SrcLangExt_Slice)
  {
    ltype.prepend("typedef ");
  }
  if (isTypeAlias())
  {
    ltype="using";
  }
  // strip 'friend' keyword from ltype
  ltype.stripPrefix("friend ");
  static QRegExp r("@[0-9]+");

  bool endAnonScopeNeeded=FALSE;
  int l,i=r.match(ltype,0,&l);
  if (i!=-1) // member has an anonymous type
  {
    //printf("annoClassDef=%p annMemb=%p scopeName='%s' anonymous='%s'\n",
    //    annoClassDef,annMemb,cname.data(),ltype.mid(i,l).data());

    if (annoClassDef) // type is an anonymous compound
    {
      int ir=i+l;
      //printf("<<<<<<<<<<<<<<\n");
      ol.startAnonTypeScope(s_indentLevel++);
      annoClassDef->writeDeclaration(ol,m_impl->annMemb,inGroup,inheritedFrom,inheritId);
      //printf(">>>>>>>>>>>>>> startMemberItem(2)\n");
      ol.startMemberItem(anchor(),2,inheritId);
      int j;
      for (j=0;j< s_indentLevel-1;j++)
      {
        ol.writeNonBreakableSpace(3);
      }
      QCString varName=ltype.right(ltype.length()-ir).stripWhiteSpace();
      //printf(">>>>>> ltype='%s' varName='%s'\n",ltype.data(),varName.data());
      ol.docify("}");
      if (varName.isEmpty() && (name().isEmpty() || name().at(0)=='@'))
      {
        ol.docify(";");
      }
      else if (!varName.isEmpty() && (varName.at(0)=='*' || varName.at(0)=='&'))
      {
        ol.docify(" ");
        ol.docify(varName);
      }
      endAnonScopeNeeded=TRUE;
    }
    else
    {
      if (getAnonymousEnumType()) // type is an anonymous enum
      {
        linkifyText(TextGeneratorOLImpl(ol), // out
                    d,                       // scope
                    getBodyDef(),            // fileScope
                    this,                    // self
                    ltype.left(i),           // text
                    FALSE                    // autoBreak
                   );
        getAnonymousEnumType()->writeEnumDeclaration(ol,cd,nd,fd,gd);
        //ol+=*getAnonymousEnumType()->enumDecl();
        linkifyText(TextGeneratorOLImpl(ol),d,getFileDef(),this,ltype.right(ltype.length()-i-l),TRUE);
      }
      else
      {
        ltype = ltype.left(i) + " { ... } " + removeAnonymousScopes(ltype.right(ltype.length()-i-l));
        linkifyText(TextGeneratorOLImpl(ol), // out
                    d,                       // scope
                    getBodyDef(),            // fileScope
                    this,                    // self
                    ltype,                   // text
                    FALSE                    // autoBreak
                   );
      }
    }
  }
  else if (ltype=="@") // rename type from enum values
  {
    ltype="";
  }
  else
  {
    if (isObjCMethod())
    {
      ltype.prepend("(");
      ltype.append(")");
    }
    linkifyText(TextGeneratorOLImpl(ol), // out
                d,                       // scope
                getBodyDef(),            // fileScope
                this,                    // self
                ltype,                   // text
                FALSE                    // autoBreak
               );
  }
  bool htmlOn = ol.isEnabled(OutputGenerator::Html);
  if (htmlOn && !ltype.isEmpty())
  {
    ol.disable(OutputGenerator::Html);
  }
  if (!ltype.isEmpty()) ol.docify(" ");
  if (htmlOn)
  {
    ol.enable(OutputGenerator::Html);
  }

  if (m_impl->annMemb)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeNonBreakableSpace(3);
    ol.popGeneratorState();
  }
  else
  {
    ol.insertMemberAlign(m_impl->tArgList!=0);
  }

  // *** write name
  if (!name().isEmpty() && name().at(0)!='@') // hide anonymous stuff
  {
    static bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
    static bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
    static bool extractStatic  = Config_getBool(EXTRACT_STATIC);
    //printf("Member name=`%s gd=%p md->groupDef=%p inGroup=%d isLinkable()=%d hasDocumentation=%d\n",name().data(),gd,getGroupDef(),inGroup,isLinkable(),hasDocumentation());
    if (!(name().isEmpty() || name().at(0)=='@') && // name valid
        (hasDocumentation() || isReference()) && // has docs
        !(m_impl->prot==Private && !extractPrivate && (m_impl->virt==Normal || !extractPrivateVirtual) && m_impl->mtype!=MemberType_Friend) && // hidden due to protection
        !(isStatic() && getClassDef()==0 && !extractStatic) // hidden due to static-ness
       )
    {
      if (m_impl->annMemb)
      {
        //printf("anchor=%s ann_anchor=%s\n",anchor(),annMemb->anchor());
        m_impl->annMemb->writeLink(ol,
            m_impl->annMemb->getClassDef(),
            m_impl->annMemb->getNamespaceDef(),
            m_impl->annMemb->getFileDef(),
            m_impl->annMemb->getGroupDef()
                          );
        m_impl->annMemb->setAnonymousUsed();
        setAnonymousUsed();
      }
      else
      {
        //printf("writeLink %s->%d\n",name.data(),hasDocumentation());
        const ClassDef *rcd = cd;
        if (isReference() && getClassDef()) rcd = getClassDef();
        writeLink(ol,rcd,nd,fd,gd);
      }
    }
    else if (isDocumentedFriendClass())
      // if the member is an undocumented friend declaration for some class,
      // then maybe we can link to the class
    {
      writeLink(ol,getClass(name()),0,0,0);
    }
    else
      // there is a brief member description and brief member
      // descriptions are enabled or there is no detailed description.
    {
      if (m_impl->annMemb)
      {
        m_impl->annMemb->setAnonymousUsed();
        setAnonymousUsed();
      }
      const ClassDef *rcd = cd;
      if (isReference() && getClassDef()) rcd = getClassDef();
      writeLink(ol,rcd,nd,fd,gd,TRUE);
    }
  }

  // add to index
  if (isEnumerate() && name().at(0)=='@')
  {
    // don't add to index
  }
  else // index member
  {
    //static bool separateMemPages = Config_getBool(SEPARATE_MEMBER_PAGES);
    //QCString cfname = getOutputFileBase();
    //QCString cfiname = d->getOutputFileBase();
    //Doxygen::indexList->addIndexItem(
    //    cname,                                 // level1
    //    name(),                                // level2
    //    separateMemPages ? cfname : cfiname,   // contRef
    //    cfname,                                // memRef
    //    anchor(),                              // anchor
    //    this);                                 // memberdef
    Doxygen::indexList->addIndexItem(d,this);
  }

  // *** write arguments
  if (argsString() && !isObjCMethod())
  {
    if (!isDefine() && !isTypedef()) ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol), // out
                d,                       // scope
                getBodyDef(),            // fileScope
                this,                    // self
                isDefine() ?
                   (const char*)substitute(argsString(),",",", ") :
                isTypedef() ?
                   (const char*)substitute(argsString(),")(",") (") :
                   argsString(),         // text
                m_impl->annMemb,         // autoBreak
                TRUE,                    // external
                FALSE,                   // keepSpaces
                s_indentLevel
               );
  }
  // *** write exceptions
  if (excpString())
  {
    ol.writeString(" ");
    ol.docify(excpString());
  }

  // *** write bitfields
  if (!m_impl->bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->bitfields);
  }
  else if (hasOneLineInitializer()
      //!init.isEmpty() && initLines==0 && // one line initializer
      //((maxInitLines>0 && userInitLines==-1) || userInitLines>0) // enabled by default or explicitly
          ) // add initializer
  {
    if (!isDefine())
    {
      //ol.writeString(" = ");
      ol.writeString(" ");
      linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->initializer.simplifyWhiteSpace());
    }
    else
    {
      ol.writeNonBreakableSpace(3);
      linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->initializer);
    }
  }
  else if (isTypeAlias()) // using template alias
  {
    ol.writeString(" = ");
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->type);
  }


  if ((isObjCMethod() || isObjCProperty()) && isImplementation())
  {
    ol.startTypewriter();
    ol.docify(" [implementation]");
    ol.endTypewriter();
  }
  
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);

  if (isProperty() && (isSettable() || isGettable() ||
      isPrivateSettable() || isPrivateGettable() ||
      isProtectedSettable() || isProtectedGettable()))
  {
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      QStrList sl;
      
      if (isGettable())             sl.append("get");
      if (isProtectedGettable())    sl.append("protected get");
      if (isSettable())             sl.append("set");
      if (isProtectedSettable())    sl.append("protected set");
      if (extractPrivate)
      {
        if (isPrivateGettable())    sl.append("private get");
        if (isPrivateSettable())    sl.append("private set");
      }
      const char *s=sl.first();
      while (s)
      {
         ol.docify(s);
         s=sl.next();
         if (s) ol.docify(", ");
      }
      ol.docify("]");
      ol.endTypewriter();
  }

  if (isEvent() && (isAddable() || isRemovable() || isRaisable()))
  {
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      QStrList sl;
      if (isAddable())   sl.append("add");
      if (isRemovable()) sl.append("remove");
      if (isRaisable())  sl.append("raise");
      const char *s=sl.first();
      while (s)
      {
         ol.docify(s);
         s=sl.next();
         if (s) ol.docify(", ");
      }
      ol.docify("]");
      ol.endTypewriter();
  }

  if (writeAnchor)
  {
    ol.endDoxyAnchor(cfname,anchor());
  }

  //printf("endMember %s annoClassDef=%p annEnumType=%p\n",
  //    name().data(),annoClassDef,annEnumType);
  ol.endMemberItem();
  if (endAnonScopeNeeded)
  {
    ol.endAnonTypeScope(--s_indentLevel);
  }

  // write brief description
  if (!briefDescription().isEmpty() &&
      Config_getBool(BRIEF_MEMBER_DESC)
      /* && !annMemb */
     )
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():d,this,briefDescription(),
                TRUE,FALSE,0,TRUE,FALSE);

    if (rootNode && !rootNode->isEmpty())
    {
      ol.startMemberDescription(anchor(),inheritId);
      ol.writeDoc(rootNode,getOuterScope()?getOuterScope():d,this);
      if (detailsVisible)
      {
        static bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
        ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        //ol.endEmphasis();
        ol.docify(" ");
        ol.startTextLink(getOutputFileBase(),anchor());
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
        //ol.startEmphasis();
        ol.popGeneratorState();
      }
      // for RTF we need to add an extra empty paragraph
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::RTF);
      ol.startParagraph();
      ol.endParagraph();
      ol.popGeneratorState();
      ol.endMemberDescription();
    }
    delete rootNode;
  }

  ol.endMemberDeclaration(anchor(),inheritId);

  warnIfUndocumented();
}

bool MemberDefImpl::isDetailedSectionLinkable() const
{
  static bool extractAll        = Config_getBool(EXTRACT_ALL);
  static bool alwaysDetailedSec = Config_getBool(ALWAYS_DETAILED_SEC);
  static bool repeatBrief       = Config_getBool(REPEAT_BRIEF);
  static bool briefMemberDesc   = Config_getBool(BRIEF_MEMBER_DESC);
  static bool hideUndocMembers  = Config_getBool(HIDE_UNDOC_MEMBERS);
  static bool extractStatic     = Config_getBool(EXTRACT_STATIC);
  static bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);

  // the member has details documentation for any of the following reasons
  bool docFilter =
         // treat everything as documented
         extractAll ||
         // has detailed docs
         !documentation().isEmpty() ||
         // has inbody docs
         !inbodyDocumentation().isEmpty() ||
         // is an enum with values that are documented
         (m_impl->mtype==MemberType_Enumeration && m_impl->docEnumValues) ||
         // is documented enum value
         (m_impl->mtype==MemberType_EnumValue && !briefDescription().isEmpty()) ||
         // has brief description that is part of the detailed description
         (!briefDescription().isEmpty() &&           // has brief docs
          (alwaysDetailedSec &&                      // they are visible in
           (repeatBrief ||                           // detailed section or
            !briefMemberDesc                         // they are explicitly not
           )                                         // shown in brief section
          )
         ) ||
         // has a multi-line initialization block
         //(initLines>0 && initLines<maxInitLines) ||
         (hasMultiLineInitializer() && !hideUndocMembers) ||
         // has one or more documented arguments
         (m_impl->defArgList!=0 && m_impl->defArgList->hasDocumentation()) ||
         // is an attribute or property - need to display that tag
         (m_impl->memSpec & (Entry::Attribute|Entry::Property)) ||
         // has user comments
         Doxygen::userComments
         ;

  // this is not a global static or global statics should be extracted
  bool staticFilter = getClassDef()!=0 || !isStatic() || extractStatic;

  // only include members that are non-private unless EXTRACT_PRIVATE is
  // set to YES or the member is part of a   group
  bool privateFilter = protectionLevelVisible(protection()) || m_impl->mtype==MemberType_Friend ||
                       (m_impl->prot==Private && m_impl->virt!=Normal && extractPrivateVirtual);

  // hide friend (class|struct|union) member if HIDE_FRIEND_COMPOUNDS
  // is true
  bool friendCompoundFilter = !(Config_getBool(HIDE_FRIEND_COMPOUNDS) &&
                                isFriend() &&
                                (m_impl->type=="friend class" ||
                                 m_impl->type=="friend struct" ||
                                 m_impl->type=="friend union"
                                )
                               );


  bool result = (docFilter && staticFilter && privateFilter && friendCompoundFilter && !isHidden());
  return result;
}

bool MemberDefImpl::isDetailedSectionVisible(bool inGroup,bool inFile) const
{
  static bool separateMemPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  static bool inlineSimpleStructs = Config_getBool(INLINE_SIMPLE_STRUCTS);
  static bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
  bool groupFilter = getGroupDef()==0 || inGroup || separateMemPages;
  bool fileFilter  = getNamespaceDef()==0 || !getNamespaceDef()->isLinkable() || !inFile;
  bool simpleFilter = (hasBriefDescription() || !hideUndocMembers) && inlineSimpleStructs &&
                      getClassDef()!=0 && getClassDef()->isSimple();

  bool visible = isDetailedSectionLinkable() && groupFilter && fileFilter &&
                 !isReference();
  bool result = visible || simpleFilter;
  //printf("%s::isDetailedSectionVisble: %d groupFilter=%d fileFilter=%d\n",
  //    name().data(),result,groupFilter,fileFilter);
  return result;
}

void MemberDefImpl::getLabels(QStrList &sl,const Definition *container) const
{
  static bool inlineInfo = Config_getBool(INLINE_INFO);

  Specifier lvirt=virtualness();
  if ((!isObjCMethod() || isOptional() || isRequired()) &&
      (protection()!=Public || lvirt!=Normal ||
       isFriend() || isRelated() ||
       (isInline() && inlineInfo) ||
       isSignal() || isSlot() ||
       isStatic() ||
       (getClassDef() && getClassDef()!=container && container->definitionType()==TypeClass) ||
       (m_impl->memSpec & ~Entry::Inline)!=0
      )
     )
  {
    // write the member specifier list
    //ol.writeLatexSpacing();
    //ol.startTypewriter();
    //ol.docify(" [");
    SrcLangExt lang = getLanguage();
    bool optVhdl = lang==SrcLangExt_VHDL;
    bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
    if (optVhdl)
    {
      sl.append(theTranslator->trVhdlType(getMemberSpecifiers(),TRUE));
    }
    else
    {
      if (isFriend()) sl.append("friend");
      else if (isRelated()) sl.append("related");
      else
      {
        if      (Config_getBool(INLINE_INFO) && isInline()) sl.append("inline");
        if      (isExplicit())            sl.append("explicit");
        if      (isMutable())             sl.append("mutable");
        if      (isStatic())              sl.append("static");
        if      (isGettable())            sl.append("get");
        if      (isProtectedGettable())   sl.append("protected get");
        if      (isSettable())            sl.append("set");
        if      (isProtectedSettable())   sl.append("protected set");
        if (extractPrivate)
        {
          if    (isPrivateGettable())     sl.append("private get");
          if    (isPrivateSettable())     sl.append("private set");
        }
        if      (isConstExpr())           sl.append("constexpr");
        if      (isAddable())             sl.append("add");
        if      (!isUNOProperty() && isRemovable()) sl.append("remove");
        if      (isRaisable())            sl.append("raise");
        if      (isReadable())            sl.append("read");
        if      (isWritable())            sl.append("write");
        if      (isFinal())               sl.append("final");
        if      (isAbstract())            sl.append("abstract");
        if      (isOverride())            sl.append("override");
        if      (isInitonly())            sl.append("initonly");
        if      (isSealed())              sl.append("sealed");
        if      (isNew())                 sl.append("new");
        if      (isOptional())            sl.append("optional");
        if      (isRequired())            sl.append("required");

        if      (isNonAtomic())           sl.append("nonatomic");
        else if (isObjCProperty())        sl.append("atomic");

        // mutual exclusive Objective 2.0 property attributes
        if      (isAssign())              sl.append("assign");
        else if (isCopy())                sl.append("copy");
        else if (isRetain())              sl.append("retain");
        else if (isWeak())                sl.append("weak");
        else if (isStrong())              sl.append("strong");
        else if (isUnretained())          sl.append("unsafe_unretained");

        if (!isObjCMethod())
        {
          if      (protection()==Protected) sl.append("protected");
          else if (protection()==Private)   sl.append("private");
          else if (protection()==Package)   sl.append("package");

          if      (lvirt==Virtual)          sl.append("virtual");
          else if (lvirt==Pure)             sl.append("pure virtual");
          if      (isSignal())              sl.append("signal");
          if      (isSlot())                sl.append("slot");
          if      (isDefault())             sl.append("default");
          if      (isDelete())              sl.append("delete");
          if      (isNoExcept())            sl.append("noexcept");
          if      (isAttribute())           sl.append("attribute");
          if      (isUNOProperty())         sl.append("property");
          if      (isReadonly())            sl.append("readonly");
          if      (isBound())               sl.append("bound");
          if      (isUNOProperty() && isRemovable()) sl.append("removable");
          if      (isConstrained())         sl.append("constrained");
          if      (isTransient())           sl.append("transient");
          if      (isMaybeVoid())           sl.append("maybevoid");
          if      (isMaybeDefault())        sl.append("maybedefault");
          if      (isMaybeAmbiguous())      sl.append("maybeambiguous");
          if      (isPublished())           sl.append("published"); // enum
        }
        if (isObjCProperty() && isImplementation())
        {
          sl.append("implementation");
        }
      }
      if (getClassDef() &&
          container->definitionType()==TypeClass &&
          getClassDef()!=container &&
          !isRelated()
         )
      {
        sl.append("inherited");
      }
    }
  }
  else if (isObjCMethod() && isImplementation())
  {
    sl.append("implementation");
  }
}

void MemberDefImpl::_writeCallGraph(OutputList &ol) const
{
  // write call graph
  if (m_impl->hasCallGraph
      && (isFunction() || isSlot() || isSignal()) && Config_getBool(HAVE_DOT)
     )
  {
    DotCallGraph callGraph(this,FALSE);
    int numNodes;
    if (callGraph.isTooBig(numNodes))
    {
       warn_uncond("Call graph for '%s' not generated, too many nodes(%d). Consider increasing DOT_GRAPH_MAX_NODES.\n",qPrint(qualifiedName()),numNodes);
    }
    else if (!callGraph.isTrivial())
    {
      msg("Generating call graph for function %s\n",qPrint(qualifiedName()));
      ol.disable(OutputGenerator::Man);
      ol.startCallGraph();
      ol.parseText(theTranslator->trCallGraph());
      ol.endCallGraph(callGraph);
      ol.enableAll();
    }
  }
}

void MemberDefImpl::_writeCallerGraph(OutputList &ol) const
{
  if (m_impl->hasCallerGraph
      && (isFunction() || isSlot() || isSignal()) && Config_getBool(HAVE_DOT)
     )
  {
    DotCallGraph callerGraph(this, TRUE);
    int numNodes;
    if (callerGraph.isTooBig(numNodes))
    {
       warn_uncond("Caller graph for '%s' not generated, too many nodes(%d). Consider increasing DOT_GRAPH_MAX_NODES.\n",qPrint(qualifiedName()),numNodes);
    }
    else if (!callerGraph.isTrivial())
    {
      msg("Generating caller graph for function %s\n",qPrint(qualifiedName()));
      ol.disable(OutputGenerator::Man);
      ol.startCallGraph();
      ol.parseText(theTranslator->trCallerGraph());
      ol.endCallGraph(callerGraph);
      ol.enableAll();
    }
  }
}

void MemberDefImpl::_writeReimplements(OutputList &ol) const
{
  MemberDef *bmd=reimplements();
  const ClassDef *bcd=0;
  if (bmd && (bcd=bmd->getClassDef()))
  {
    // write class that contains a member that is reimplemented by this one
    if (bcd->isLinkable())
    {
      ol.startParagraph();
      QCString reimplFromLine;
      if (bmd->virtualness()!=Pure && bcd->compoundType()!=ClassDef::Interface)
      {
        reimplFromLine = theTranslator->trReimplementedFromList(1);
      }
      else
      {
        reimplFromLine = theTranslator->trImplementedFromList(1);
      }
      int markerPos = reimplFromLine.find("@0");
      if (markerPos!=-1) // should always pass this.
      {
        ol.parseText(reimplFromLine.left(markerPos)); //text left from marker
        if (bmd->isLinkable()) // replace marker with link
        {
          //Definition *bd=bmd->group;
          //if (bd==0) bd=bcd;
          ol.writeObjectLink(bmd->getReference(),bmd->getOutputFileBase(),
              bmd->anchor(),bcd->displayName());

          //ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
          //    bmd->anchor(),bcd->name());
          if ( bmd->isLinkableInProject() )
          {
            writePageRef(ol,bmd->getOutputFileBase(),bmd->anchor());
          }
        }
        else
        {
          ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
              0,bcd->displayName());
          if (bcd->isLinkableInProject()/* && !Config_getBool(PDF_HYPERLINKS)*/ )
          {
            writePageRef(ol,bcd->getOutputFileBase(),bcd->anchor());
          }
        }
        ol.parseText(reimplFromLine.right(
              reimplFromLine.length()-markerPos-2)); // text right from marker

      }
      else
      {
        err("translation error: no marker in trReimplementsFromList()\n");
      }
      ol.endParagraph();
    }
  }
}

void MemberDefImpl::_writeReimplementedBy(OutputList &ol) const
{
  MemberList *bml=reimplementedBy();
  if (bml)
  {
    MemberListIterator mli(*bml);
    MemberDef *bmd=0;
    uint count=0;
    const ClassDef *bcd=0;
    for (mli.toFirst();(bmd=mli.current()) && (bcd=bmd->getClassDef());++mli)
    {
      // count the members that directly inherit from md and for
      // which the member and class are visible in the docs.
      if ( bmd->isLinkable() && bcd->isLinkable() )
      {
        count++;
      }
    }
    if (count>0)
    {
      mli.toFirst();
      // write the list of classes that overwrite this member
      ol.startParagraph();

      QCString reimplInLine;
      if (m_impl->virt==Pure || (getClassDef() && getClassDef()->compoundType()==ClassDef::Interface))
      {
        reimplInLine = theTranslator->trImplementedInList(count);
      }
      else
      {
        reimplInLine = theTranslator->trReimplementedInList(count);
      }
      static QRegExp marker("@[0-9]+");
      int index=0,newIndex,matchLen;
      // now replace all markers in reimplInLine with links to the classes
      while ((newIndex=marker.match(reimplInLine,index,&matchLen))!=-1)
      {
        ol.parseText(reimplInLine.mid(index,newIndex-index));
        bool ok;
        uint entryIndex = reimplInLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
        //bmd=bml->at(entryIndex);

        count=0;
        // find the entryIndex-th documented entry in the inheritance list.
        for (mli.toLast();(bmd=mli.current()) && (bcd=bmd->getClassDef());--mli)
        {
          if ( bmd->isLinkable() && bcd->isLinkable())
          {
            if (count==entryIndex) break;
            count++;
          }
        }

        if (ok && bcd && bmd) // write link for marker
        {
          //ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
          //    bmd->anchor(),bcd->name());
          ol.writeObjectLink(bmd->getReference(),bmd->getOutputFileBase(),
              bmd->anchor(),bcd->displayName());

          if (bmd->isLinkableInProject() )
          {
            writePageRef(ol,bmd->getOutputFileBase(),bmd->anchor());
          }
        }
        ++mli;
        index=newIndex+matchLen;
      }
      ol.parseText(reimplInLine.right(reimplInLine.length()-index));
      ol.endParagraph();
    }
  }
}

void MemberDefImpl::_writeCategoryRelation(OutputList &ol) const
{
  if (getClassDef()) // this should be a member of a class/category
  {
    //printf("%s: category %s relation %s class=%s categoryOf=%s\n",
    //    name().data(),
    //    m_impl->category ? m_impl->category->name().data() : "<none>",
    //    m_impl->categoryRelation ? m_impl->categoryRelation->name().data() : "<none>",
    //    m_impl->classDef->name().data(),
    //    m_impl->classDef->categoryOf() ? m_impl->classDef->categoryOf()->name().data() : "<none>"
    //    );
    QCString text;
    QCString ref;
    QCString file;
    QCString anc;
    QCString name;
    int i=-1;
    if (m_impl->categoryRelation && m_impl->categoryRelation->isLinkable())
    {
      if (m_impl->category)
      {
        // this member is in a normal class and implements method categoryRelation from category
        // so link to method 'categoryRelation' with 'provided by category 'category' text.
        text = theTranslator->trProvidedByCategory();
        name = m_impl->category->displayName();
      }
      else if (getClassDef()->categoryOf())
      {
        // this member is part of a category so link to the corresponding class member of the class we extend
        // so link to method 'categoryRelation' with 'extends class 'classDef->categoryOf()'
        text = theTranslator->trExtendsClass();
        name = getClassDef()->categoryOf()->displayName();
      }
      i=text.find("@0");
      if (i!=-1)
      {
        MemberDef *md = m_impl->categoryRelation;
        ref  = md->getReference();
        file = md->getOutputFileBase();
        anc  = md->anchor();
      }
    }
    if (i!=-1 && !name.isEmpty())
    {
      ol.startParagraph();
      ol.parseText(text.left(i));
      ol.writeObjectLink(ref,file,anc,name);
      ol.parseText(text.mid(i+2));
      ol.endParagraph();
    }
  }
}

void MemberDefImpl::_writeExamples(OutputList &ol) const
{
  // write the list of examples that use this member
  if (hasExamples())
  {
    ol.startExamples();
    ol.startDescForItem();
    writeExample(ol,m_impl->exampleSDict);
    ol.endDescForItem();
    ol.endExamples();
  }
}

void MemberDefImpl::_writeTypeConstraints(OutputList &ol) const
{
  if (m_impl->typeConstraints)
  {
    writeTypeConstraints(ol,this,m_impl->typeConstraints);
  }
}

void MemberDefImpl::_writeEnumValues(OutputList &ol,const Definition *container,
                                 const QCString &cfname,const QCString &ciname,
                                 const QCString &cname) const
{
  // For enum, we also write the documented enum values
  if (isEnumerate())
  {
    bool first=TRUE;
    const MemberList *fmdl=enumFieldList();
    //printf("** %s: enum values=%d\n",name().data(),fmdl!=0 ? fmdl->count() : 0);
    if (fmdl)
    {
      MemberListIterator it(*fmdl);
      MemberDef *fmd;
      for (;(fmd=it.current());++it)
      {
        //printf("Enum %p: isLinkable()=%d\n",fmd,fmd->isLinkable());
        if (fmd->isLinkable())
        {
          if (first)
          {
            ol.startDescTable(theTranslator->trEnumerationValues());
          }

          ol.startDescTableRow();
          ol.addIndexItem(fmd->name(),ciname);
          ol.addIndexItem(ciname,fmd->name());

          Doxygen::indexList->addIndexItem(container,fmd);

          ol.startDescTableTitle();
          ol.startDoxyAnchor(cfname,cname,fmd->anchor(),fmd->name(),fmd->argsString());
          first=FALSE;
          ol.docify(fmd->name());
          ol.disableAllBut(OutputGenerator::Man);
          ol.writeString(" ");
          ol.enableAll();
          ol.endDoxyAnchor(cfname,fmd->anchor());
          ol.endDescTableTitle();
          ol.startDescTableData();

          bool hasBrief = !fmd->briefDescription().isEmpty();
          bool hasDetails = !fmd->documentation().isEmpty();

          if (hasBrief)
          {
            ol.generateDoc(fmd->briefFile(),fmd->briefLine(),
                getOuterScope()?getOuterScope():container,
                fmd,fmd->briefDescription(),TRUE,FALSE);
          }
          // FIXME:PARA
          //if (!fmd->briefDescription().isEmpty() &&
          //    !fmd->documentation().isEmpty())
          //{
          //  ol.newParagraph();
          //}
          if (hasDetails)
          {
            ol.generateDoc(fmd->docFile(),fmd->docLine(),
                getOuterScope()?getOuterScope():container,
                fmd,fmd->documentation()+"\n",TRUE,FALSE);
          }
          ol.endDescTableData();
          ol.endDescTableRow();
        }
      }
    }
    if (!first)
    {
      ol.endDescTable();
    }
  }
}

QCString MemberDefImpl::displayDefinition() const
{
  QCString ldef = definition();
  QCString title = name();
  if (isEnumerate())
  {
    if (title.at(0)=='@')
    {
      ldef = title = "anonymous enum";
      if (!m_impl->enumBaseType.isEmpty())
      {
        ldef+=" : "+m_impl->enumBaseType;
      }
    }
    else
    {
      ldef.prepend("enum ");
      if (isSliceLocal())
      {
        ldef.prepend("local ");
      }
    }
  }
  else if (isEnumValue())
  {
    if (ldef.at(0)=='@')
    {
      ldef=ldef.mid(2);
    }
  }
  static QRegExp r("@[0-9]+");
  int l,i=r.match(ldef,0,&l);
  if (i!=-1) // replace anonymous parts with { ... }
  {
    int si=ldef.find(' '),pi,ei=i+l;
    if (si==-1) si=0;
    while ((pi=r.match(ldef,i+l,&l))!=-1)
    {
      i=pi;
      ei=i+l;
    }
    int ni=ldef.find("::",si);
    if (ni>=ei) ei=ni+2;
    ldef = ldef.left(si) + " { ... } " + ldef.right(ldef.length()-ei);
  }
  const ClassDef *cd=getClassDef();
  if (cd && cd->isObjectiveC())
  {
    // strip scope name
    int ep = ldef.find("::");
    if (ep!=-1)
    {
      int sp=ldef.findRev(' ',ep);
      if (sp!=-1)
      {
        ldef=ldef.left(sp+1)+ldef.mid(ep+2);
      }
    }
    // strip keywords
    int dp = ldef.find(':');
    if (dp!=-1)
    {
      ldef=ldef.left(dp+1);
    }
    l=ldef.length();
    //printf("start >%s<\n",ldef.data());
    i=l-1;
    while (i>=0 && (isId(ldef.at(i)) || ldef.at(i)==':')) i--;
    while (i>=0 && isspace((uchar)ldef.at(i))) i--;
    if (i>0)
    {
      // insert braches around the type
      QCString tmp("("+ldef.left(i+1)+")"+ldef.mid(i+1));
      ldef=tmp;
    }
    //printf("end   >%s< i=%d\n",ldef.data(),i);
    if (isStatic()) ldef.prepend("+ "); else ldef.prepend("- ");
  }
  SrcLangExt lang = getLanguage();
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  return substitute(ldef,"::",sep);
}

void MemberDefImpl::_writeGroupInclude(OutputList &ol,bool inGroup) const
{
  // only write out the include file if this is not part of a class or file
  // definition
  static bool showGroupedMembInc = Config_getBool(SHOW_GROUPED_MEMB_INC);
  const FileDef *fd = getFileDef();
  QCString nm;
  if (fd) nm = getFileDef()->docName();
  if (inGroup && fd && showGroupedMembInc && !nm.isEmpty())
  {
    ol.startParagraph();
    ol.startTypewriter();
    SrcLangExt lang = getLanguage();
    bool isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
    if (isIDLorJava)
    {
      ol.docify("import ");
    }
    else
    {
      ol.docify("#include ");
    }

    if (isIDLorJava) ol.docify("\""); else ol.docify("<");

    if (fd->isLinkable())
    {
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),fd->anchor(),nm);
    }
    else
    {
      ol.docify(nm);
    }

    if (isIDLorJava) ol.docify("\""); else ol.docify(">");

    ol.endTypewriter();
    ol.endParagraph();
  }
}

/*! Writes the "detailed documentation" section of this member to
 *  all active output formats.
 */
void MemberDefImpl::writeDocumentation(const MemberList *ml,
                                   int memCount,int memTotal,
                                   OutputList &ol,
                                   const char *scName,
                                   const Definition *container,
                                   bool inGroup,
                                   bool showEnumValues,
                                   bool showInline
                                  ) const
{
  // if this member is in a group find the real scope name.
  bool hasParameterList = FALSE;
  bool inFile = container->definitionType()==Definition::TypeFile;
  bool hasDocs = isDetailedSectionVisible(inGroup,inFile);

  //printf("MemberDefImpl::writeDocumentation(): name='%s' hasDocs='%d' containerType=%d inGroup=%d sectionLinkable=%d\n",
  //    name().data(),hasDocs,container->definitionType(),inGroup,isDetailedSectionLinkable());

  //if ( !hasDocs ) return;
  //if (isEnumValue() && !showEnumValues) return;

  SrcLangExt lang = getLanguage();
  //printf("member=%s lang=%d\n",name().data(),lang);
  bool optVhdl = lang==SrcLangExt_VHDL;
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);

  QCString scopeName = scName;
  QCString memAnchor = anchor();
  QCString ciname = container->displayName();
  const Definition *scopedContainer = container; // see bug 753608
  if (container->definitionType()==TypeGroup)
  {
    if (getClassDef())          { scopeName=getClassDef()->displayName();     scopedContainer=getClassDef(); }
    else if (getNamespaceDef()) { scopeName=getNamespaceDef()->displayName(); scopedContainer=getNamespaceDef(); }
    else if (getFileDef())      { scopeName=getFileDef()->displayName();      scopedContainer=getFileDef(); }
    ciname = (dynamic_cast<const GroupDef *>(container))->groupTitle();
  }
  else if (container->definitionType()==TypeFile && getNamespaceDef() && lang != SrcLangExt_Python)
  { // member is in a namespace, but is written as part of the file documentation
    // as well, so we need to make sure its label is unique.
    memAnchor.prepend("file_");
  }

  QCString cname   = container->name();
  QCString cfname  = getOutputFileBase();

  // get member name
  QCString doxyName=name();
  // prepend scope if there is any. TODO: make this optional for C only docs
  if (!scopeName.isEmpty())
  {
    doxyName.prepend(scopeName+sep);
  }
  QCString doxyArgs=argsString();

  QCString ldef = definition();
  QCString title = name();
  //printf("member '%s' def='%s'\n",name().data(),ldef.data());
  if (isEnumerate())
  {
    if (title.at(0)=='@')
    {
      ldef = title = "anonymous enum";
      if (!m_impl->enumBaseType.isEmpty())
      {
        ldef+=" : "+m_impl->enumBaseType;
      }
    }
    else
    {
      ldef.prepend("enum ");
      if (isSliceLocal())
      {
        ldef.prepend("local ");
      }
    }
  }
  else if (isEnumValue())
  {
    if (ldef.at(0)=='@')
    {
      ldef=ldef.mid(2);
    }
  }
  else if (isFunction())
  {
    title += "()";
  }
  int i=0,l;
  static QRegExp r("@[0-9]+");

  if (lang == SrcLangExt_Slice)
  {
    // Remove the container scope from the member name.
    QCString prefix = scName + sep;
    int pos = ldef.findRev(prefix.data());
    if(pos != -1)
    {
      ldef.remove(pos, prefix.length());
    }
  }

  //----------------------------------------

  ol.pushGeneratorState();

  bool htmlEndLabelTable=FALSE;
  QStrList sl;
  getLabels(sl,scopedContainer);

  if ((isVariable() || isTypedef()) && (i=r.match(ldef,0,&l))!=-1)
  {
    // find enum type and insert it in the definition
    MemberListIterator vmli(*ml);
    MemberDef *vmd;
    bool found=FALSE;
    for ( ; (vmd=vmli.current()) && !found ; ++vmli)
    {
      if (vmd->isEnumerate() && ldef.mid(i,l)==vmd->name())
      {
        ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
        ol.startMemberDoc(ciname,name(),memAnchor,name(),memCount,memTotal,showInline);
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef.left(i));
        vmd->writeEnumDeclaration(ol,getClassDef(),getNamespaceDef(),getFileDef(),getGroupDef());
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef.right(ldef.length()-i-l));

        found=TRUE;
      }
    }
    if (!found) // anonymous compound
    {
      //printf("Anonymous compound '%s'\n",cname.data());
      ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
      ol.startMemberDoc(ciname,name(),memAnchor,name(),memCount,memTotal,showInline);
      // search for the last anonymous compound name in the definition
      int si=ldef.find(' '),pi,ei=i+l;
      if (si==-1) si=0;
      while ((pi=r.match(ldef,i+l,&l))!=-1)
      {
        i=pi;
        ei=i+l;
      }
      // first si characters of ldef contain compound type name
      ol.startMemberDocName(isObjCMethod());
      ol.docify(ldef.left(si));
      ol.docify(" { ... } ");
      // last ei characters of ldef contain pointer/reference specifiers
      int ni=ldef.find("::",si);
      if (ni>=ei) ei=ni+2;
      linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef.right(ldef.length()-ei));
    }
  }
  else // not an enum value or anonymous compound
  {
    ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
    ol.startMemberDoc(ciname,name(),memAnchor,title,memCount,memTotal,showInline);

    if (!m_impl->metaData.isEmpty() && getLanguage()==SrcLangExt_Slice)
    {
      ol.startMemberDocPrefixItem();
      ol.docify(m_impl->metaData);
      ol.endMemberDocPrefixItem();
    }

    const ClassDef *cd=getClassDef();
    const NamespaceDef *nd=getNamespaceDef();
    if (!Config_getBool(HIDE_SCOPE_NAMES))
    {
      bool first=TRUE;
      SrcLangExt lang = getLanguage();
      if (m_impl->defTmpArgLists && lang==SrcLangExt_Cpp)
        // definition has explicit template parameter declarations
      {
        QListIterator<ArgumentList> ali(*m_impl->defTmpArgLists);
        ArgumentList *tal;
        for (ali.toFirst();(tal=ali.current());++ali)
        {
          if (tal->count()>0)
          {
            if (!first) ol.docify(" ");
            ol.startMemberDocPrefixItem();
            writeTemplatePrefix(ol,tal);
            ol.endMemberDocPrefixItem();
          }
        }
      }
      else // definition gets it template parameters from its class
        // (since no definition was found)
      {
        if (cd && lang==SrcLangExt_Cpp && !isTemplateSpecialization())
        {
          QList<ArgumentList> tempParamLists;
          cd->getTemplateParameterLists(tempParamLists);
          //printf("#tempParamLists=%d\n",tempParamLists.count());
          QListIterator<ArgumentList> ali(tempParamLists);
          ArgumentList *tal;
          for (ali.toFirst();(tal=ali.current());++ali)
          {
            if (tal->count()>0)
            {
              if (!first) ol.docify(" ");
              ol.startMemberDocPrefixItem();
              writeTemplatePrefix(ol,tal);
              ol.endMemberDocPrefixItem();
            }
          }
        }
        if (m_impl->tArgList && lang==SrcLangExt_Cpp) // function template prefix
        {
          ol.startMemberDocPrefixItem();
          writeTemplatePrefix(ol,m_impl->tArgList);
          ol.endMemberDocPrefixItem();
        }
      }
    }

    if (sl.count()>0)
    {
      ol.pushGeneratorState();
      ol.disableAll();
      ol.enable(OutputGenerator::Html);
      ol.writeString("<table class=\"mlabels\">\n");
      ol.writeString("  <tr>\n");
      ol.writeString("  <td class=\"mlabels-left\">\n");
      ol.popGeneratorState();
      htmlEndLabelTable=TRUE;
    }

    ol.startMemberDocName(isObjCMethod());
    if (cd && cd->isObjectiveC())
    {
      // strip scope name
      int ep = ldef.find("::");
      if (ep!=-1)
      {
        int sp=ldef.findRev(' ',ep);
        if (sp!=-1)
        {
          ldef=ldef.left(sp+1)+ldef.mid(ep+2);
        } else {
          ldef=ldef.mid(ep+2);
        }
      }
      // strip keywords
      int dp = ldef.find(':');
      if (dp!=-1)
      {
        ldef=ldef.left(dp+1);
      }
      int l=ldef.length();
      //printf("start >%s<\n",ldef.data());
      int i=l-1;
      while (i>=0 && (isId(ldef.at(i)) || ldef.at(i)==':')) i--;
      while (i>=0 && isspace((uchar)ldef.at(i))) i--;
      if (i>0)
      {
        // insert braches around the type
        QCString tmp("("+ldef.left(i+1)+")"+ldef.mid(i+1));
        ldef=tmp;
      }
      //printf("end   >%s< i=%d\n",ldef.data(),i);
      if (isStatic()) ldef.prepend("+ "); else ldef.prepend("- ");
    }

    if (optVhdl)
    {
      hasParameterList=VhdlDocGen::writeVHDLTypeDocumentation(this,scopedContainer,ol);
    }
    else if (lang==SrcLangExt_Slice)
    {
      // Eliminate the self-reference.
      int pos = ldef.findRev(' ');
      linkifyText(TextGeneratorOLImpl(ol),
                  scopedContainer,
                  getBodyDef(),
                  this,
                  ldef.left(pos)
                 );
      ol.docify(ldef.mid(pos));
      const Definition *scope = cd;
      if (scope==0) scope = nd;
      hasParameterList=writeDefArgumentList(ol,scope,this);
    }
    else
    {
      linkifyText(TextGeneratorOLImpl(ol),
                  scopedContainer,
                  getBodyDef(),
                  this,
                  substitute(ldef,"::",sep)
                 );
      const Definition *scope = cd;
      if (scope==0) scope = nd;
      hasParameterList=writeDefArgumentList(ol,scope,this);
    }

    if (hasOneLineInitializer()) // add initializer
    {
      if (!isDefine())
      {
        //ol.docify(" = ");
        ol.docify(" ");
        QCString init = m_impl->initializer.simplifyWhiteSpace();
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,init);
      }
      else
      {
        ol.writeNonBreakableSpace(3);
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,m_impl->initializer);
      }
    }
    if (excpString()) // add exception list
    {
      writeExceptionList(ol,cd,this);
      hasParameterList=true; // call endParameterList below
    }
  }

  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  if (sl.count()>0)
  {
    ol.startLabels();
    const char *s=sl.first();
    while (s)
    {
      const char *ns = sl.next();
      ol.writeLabel(s,ns==0);
      s=ns;
    }
    ol.endLabels();
  }
  ol.popGeneratorState();

  if (hasParameterList)
  {
    ol.endParameterList();
    ol.endMemberDoc(TRUE);
  }
  else
  {
    ol.endMemberDocName();
    ol.endMemberDoc(FALSE);
  }

  // for HTML write the labels here
  ol.pushGeneratorState();
  ol.disableAll();
  ol.enable(OutputGenerator::Html);
  if (htmlEndLabelTable)
  {
    ol.writeString("  </td>\n");
    ol.writeString("  <td class=\"mlabels-right\">\n");
    ol.startLabels();
    const char *s=sl.first();
    while (s)
    {
      const char *ns = sl.next();
      ol.writeLabel(s,ns==0);
      s=ns;
    }
    ol.endLabels();
    ol.writeString("  </td>\n");
    ol.writeString("  </tr>\n");
    ol.writeString("</table>\n");
  }
  ol.writeString("</div>");
  ol.popGeneratorState();


  ol.endDoxyAnchor(cfname,memAnchor);
  ol.startIndent();

  _writeGroupInclude(ol,inGroup);

  /* write multi-line initializer (if any) */
  if (hasMultiLineInitializer()
      //initLines>0 && ((initLines<maxInitLines && userInitLines==-1) // implicitly enabled
      //                || initLines<userInitLines // explicitly enabled
      //               )
     )
  {
    //printf("md=%s initLines=%d init='%s'\n",name().data(),initLines,init.data());
    ol.startBold();
    if (m_impl->mtype==MemberType_Define)
      ol.parseText(theTranslator->trDefineValue());
    else
      ol.parseText(theTranslator->trInitialValue());
    ol.endBold();
    ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
    pIntf->resetCodeParserState();
    ol.startCodeFragment();
    pIntf->parseCode(ol,scopeName,m_impl->initializer,lang,FALSE,0,const_cast<FileDef*>(getFileDef()),
                     -1,-1,TRUE,this,FALSE,this);
    ol.endCodeFragment();
  }

  QCString brief           = briefDescription();
  QCString detailed        = documentation();
  ArgumentList *docArgList = m_impl->defArgList;
  if (m_impl->templateMaster)
  {
    brief      = m_impl->templateMaster->briefDescription();
    detailed   = m_impl->templateMaster->documentation();
    docArgList = m_impl->templateMaster->argumentList();
  }

  /* write brief description */
  if (!brief.isEmpty() &&
      (Config_getBool(REPEAT_BRIEF) ||
       !Config_getBool(BRIEF_MEMBER_DESC)
      )
     )
  {
    ol.startParagraph();
    ol.generateDoc(briefFile(),briefLine(),
                scopedContainer,this,
                brief,FALSE,FALSE,0,TRUE,FALSE);
    ol.endParagraph();
  }

  /* write detailed description */
  if (!detailed.isEmpty() ||
      !inbodyDocumentation().isEmpty())
  {
    // write vhdl inline code with or without option INLINE_SOURCE
    if (optVhdl && VhdlDocGen::isMisc(this))
    {
      VhdlDocGen::writeSource(this,ol,cname);
      return;
    }
    else
    {
      ol.generateDoc(docFile(),docLine(),scopedContainer,this,detailed+"\n",TRUE,FALSE);
    }

    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),
                  scopedContainer,this,
                  inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }
  else if (!brief.isEmpty() && (Config_getBool(REPEAT_BRIEF) ||
        !Config_getBool(BRIEF_MEMBER_DESC)))
  {
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),scopedContainer,this,inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }


  //printf("***** defArgList=%p name=%s docs=%s hasDocs=%d\n",
  //     defArgList,
  //     defArgList?defArgList->hasDocumentation():-1);
  if (docArgList!=0 && docArgList->hasDocumentation())
  {
    QCString paramDocs;
    ArgumentListIterator ali(*docArgList);
    Argument *a;
    // convert the parameter documentation into a list of @param commands
    for (ali.toFirst();(a=ali.current());++ali)
    {
      if (a->hasDocumentation())
      {
        QCString direction = extractDirection(a->docs);
        paramDocs+="@param"+direction+" "+a->name+" "+a->docs;
      }
    }
    // feed the result to the documentation parser
    ol.generateDoc(
        docFile(),docLine(),
        scopedContainer,
        this,         // memberDef
        paramDocs,    // docStr
        TRUE,         // indexWords
        FALSE         // isExample
        );

  }

  _writeEnumValues(ol,scopedContainer,cfname,ciname,cname);
  _writeReimplements(ol);
  _writeReimplementedBy(ol);
  _writeCategoryRelation(ol);
  _writeExamples(ol);
  _writeTypeConstraints(ol);
  writeSourceDef(ol,cname);
  writeInlineCode(ol,cname);
  if (hasReferencesRelation()) writeSourceRefs(ol,cname);
  if (hasReferencedByRelation()) writeSourceReffedBy(ol,cname);
  _writeCallGraph(ol);
  _writeCallerGraph(ol);

  if (Doxygen::userComments)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    QCString cmd = "<? $root=$_SERVER['DOCUMENT_ROOT']; "
                   "passthru(\"$root/doxynotes --lookup "+
                   getOutputFileBase()+":"+anchor()+"\") ?>";
    ol.writeString(cmd);
    ol.popGeneratorState();
  }

  ol.endIndent();

  // enable LaTeX again
  //if (Config_getBool(EXTRACT_ALL) && !hasDocs) ol.enable(OutputGenerator::Latex);
  ol.popGeneratorState();

  warnIfUndocumentedParams();
}

// strip scope and field name from the type
// example: "struct N::S.v.c" will become "struct v"
static QCString simplifyTypeForTable(const QCString &s)
{
  QCString ts=removeAnonymousScopes(s);
  if (ts.right(2)=="::") ts = ts.left(ts.length()-2);
  static QRegExp re("[A-Z_a-z0-9]+::");
  int i,l;
  while ((i=re.match(ts,0,&l))!=-1)
  {
    ts=ts.left(i)+ts.mid(i+l);
  }
  i=ts.findRev('.');
  if (i!=-1) ts = ts.left(i);
  i=ts.findRev('.');
  if (i!=-1) ts = ts.right(ts.length()-i-1);
  //printf("simplifyTypeForTable(%s)->%s\n",s.data(),ts.data());
  return ts;
}

#if 0
/** Returns the type definition corresponding to a member's return type.
 *  @param[in]  scope The scope in which to search for the class definition.
 *  @param[in]  type  The string representing the member's return type.
 *  @param[in]  lang  The programming language in which the class is defined.
 *  @param[out] start The string position where the class definition name was found.
 *  @param[out] length The length of the class definition's name.
 */
static Definition *getClassFromType(Definition *scope,const QCString &type,SrcLangExt lang,int &start,int &length)
{
  int pos=0;
  int i;
  QCString name;
  QCString templSpec;
  while ((i=extractClassNameFromType(type,pos,name,templSpec,lang))!=-1)
  {
    ClassDef *cd=0;
    MemberDef *md=0;
    int l = name.length()+templSpec.length();
    if (!templSpec.isEmpty())
    {
      cd = getResolvedClass(scope,0,name+templSpec,&md);
    }
    cd = getResolvedClass(scope,0,name);
    if (cd)
    {
      start=i;
      length=l;
      printf("getClassFromType: type=%s name=%s start=%d length=%d\n",type.data(),name.data(),start,length);
      return cd;
    }
    else if (md)
    {
      start=i;
      length=l;
      printf("getClassFromType: type=%s name=%s start=%d length=%d\n",type.data(),name.data(),start,length);
      return md;
    }
    pos=i+l;
  }
  return 0;
}
#endif

QCString MemberDefImpl::fieldType() const
{
  QCString type = m_impl->accessorType;
  if (type.isEmpty())
  {
    type = m_impl->type;
  }

  if (isTypedef() && getLanguage() != SrcLangExt_Slice) type.prepend("typedef ");
  return simplifyTypeForTable(type);
}

void MemberDefImpl::writeMemberDocSimple(OutputList &ol, const Definition *container) const
{
  Definition *scope  = getOuterScope();
  QCString doxyName  = name();
  QCString doxyArgs  = argsString();
  QCString memAnchor = anchor();
  QCString cfname    = getOutputFileBase();
  QCString cname;
  if (scope) cname   = scope->name();
  if (doxyName.at(0)=='@')
  {
    doxyName="__unnamed__";
  }

  ClassDef *cd = m_impl->accessorClass;
  //printf("===> %s::anonymous: %s\n",name().data(),cd?cd->name().data():"<none>");

  if (container && container->definitionType()==Definition::TypeClass &&
      !(dynamic_cast<const ClassDef*>(container))->isJavaEnum())
  {
    ol.startInlineMemberType();
    ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);

    QCString ts = fieldType();

    if (cd) // cd points to an anonymous struct pointed to by this member
      // so we add a link to it from the type column.
    {
      int i=0;
      const char *prefixes[] = { "struct ","union ","class ", 0 };
      const char **p = prefixes;
      while (*p)
      {
        int l=qstrlen(*p);
        if (ts.left(l)==*p)
        {
          ol.writeString(*p);
          i=l;
        }
        p++;
      }
      ol.writeObjectLink(cd->getReference(),
          cd->getOutputFileBase(),
          cd->anchor(),ts.mid(i));
    }
    else // use standard auto linking
    {
      linkifyText(TextGeneratorOLImpl(ol), // out
          scope,                   // scope
          getBodyDef(),            // fileScope
          this,                    // self
          ts                       // text
          );
    }
    ol.endDoxyAnchor(cfname,memAnchor);
    ol.endInlineMemberType();
  }

  ol.startInlineMemberName();
  ol.docify(doxyName);
  if (isVariable() && argsString() && !isObjCMethod())
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,argsString());
  }
  if (!m_impl->bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,m_impl->bitfields);
  }
  ol.endInlineMemberName();

  ol.startInlineMemberDoc();

  QCString brief           = briefDescription();
  QCString detailed        = documentation();

  /* write brief description */
  if (!brief.isEmpty())
  {
    ol.generateDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():container,this,
                brief,FALSE,FALSE,0,TRUE,FALSE);
  }

  /* write detailed description */
  if (!detailed.isEmpty())
  {
    ol.generateDoc(docFile(),docLine(),
                getOuterScope()?getOuterScope():container,this,
                detailed+"\n",FALSE,FALSE,0,FALSE,FALSE);

  }

  ol.endInlineMemberDoc();
}

QCString MemberDefImpl::memberTypeName() const
{
  switch (m_impl->mtype)
  {
    case MemberType_Define:      return "macro definition";
    case MemberType_Function:    return "function";
    case MemberType_Variable:    return "variable";
    case MemberType_Typedef:     return "typedef";
    case MemberType_Enumeration: return "enumeration";
    case MemberType_EnumValue:   return "enumvalue";
    case MemberType_Signal:      return "signal";
    case MemberType_Slot:        return "slot";
    case MemberType_Friend:      return "friend";
    case MemberType_DCOP:        return "dcop";
    case MemberType_Property:    return "property";
    case MemberType_Event:       return "event";
    case MemberType_Interface:   return "interface";
    case MemberType_Service:     return "service";
    case MemberType_Sequence:    return "sequence";
    case MemberType_Dictionary:  return "dictionary";
    default:          return "unknown";
  }
}

void MemberDefImpl::warnIfUndocumented() const
{
  /*
   *  Removed bug_303020:
   *  if (m_impl->memberGroup) return;
   */
  const ClassDef     *cd = getClassDef();
  const NamespaceDef *nd = getNamespaceDef();
  const FileDef      *fd = getFileDef();
  const GroupDef     *gd = getGroupDef();
  const Definition *d=0;
  const char *t=0;
  if (cd)
    t="class", d=cd;
  else if (nd)
  {
    d=nd;
    if (d->getLanguage() == SrcLangExt_Fortran)
      t="module";
    else
      t="namespace";
  }
  else if (gd)
    t="group", d=gd;
  else
    t="file", d=fd;
  static bool extractAll = Config_getBool(EXTRACT_ALL);

  //printf("%s:warnIfUndoc: hasUserDocs=%d isFriendClass=%d protection=%d isRef=%d isDel=%d\n",
  //    name().data(),
  //    hasUserDocumentation(),isFriendClass(),protectionLevelVisible(m_impl->prot),isReference(),isDeleted());
  if ((!hasUserDocumentation() && !extractAll) &&
      !isFriendClass() &&
      name().find('@')==-1 && d && d->name().find('@')==-1 &&
      protectionLevelVisible(m_impl->prot) &&
      !isReference() && !isDeleted()
     )
  {
    warn_undoc(getDefFileName(),getDefLine(),"Member %s%s (%s) of %s %s is not documented.",
         qPrint(name()),qPrint(argsString()),qPrint(memberTypeName()),t,qPrint(d->name()));
  }
  else if (!isDetailedSectionLinkable())
  {
    warnIfUndocumentedParams();
  }
}

void MemberDefImpl::detectUndocumentedParams(bool hasParamCommand,bool hasReturnCommand) const
{
  if (!Config_getBool(WARN_NO_PARAMDOC)) return;
  QCString returnType   = typeString();
  bool isPython = getLanguage()==SrcLangExt_Python;

  if (!m_impl->hasDocumentedParams && hasParamCommand)
  {
    //printf("%s:hasDocumentedParams=TRUE;\n",name().data());
    m_impl->hasDocumentedParams = TRUE;
  }
  else if (!m_impl->hasDocumentedParams)
  {
    const ArgumentList *al     = argumentList();
    const ArgumentList *declAl = declArgumentList();
    bool allDoc=TRUE; // no parameter => all parameters are documented
    if ( // member has parameters
        al!=0 &&       // but the member has a parameter list
        al->count()>0  // with at least one parameter (that is not void)
       )
    {
      ArgumentListIterator ali(*al);
      Argument *a;

      // see if all parameters have documentation
      for (ali.toFirst();(a=ali.current()) && allDoc;++ali)
      {
        if (!a->name.isEmpty() && a->type!="void" &&
            !(isPython && (a->name=="self" || a->name=="cls"))
           )
        {
          allDoc = !a->docs.isEmpty();
        }
        //printf("a->type=%s a->name=%s doc=%s\n",
        //        a->type.data(),a->name.data(),a->docs.data());
      }
      if (!allDoc && declAl!=0) // try declaration arguments as well
      {
        allDoc=TRUE;
        ArgumentListIterator ali(*declAl);
        Argument *a;
        for (ali.toFirst();(a=ali.current()) && allDoc;++ali)
        {
          if (!a->name.isEmpty() && a->type!="void" &&
              !(isPython && (a->name=="self" || a->name=="cls"))
             )
          {
            allDoc = !a->docs.isEmpty();
          }
          //printf("a->name=%s doc=%s\n",a->name.data(),a->docs.data());
        }
      }
    }
    if (allDoc)
    {
      //printf("%s:hasDocumentedParams=TRUE;\n",name().data());
      m_impl->hasDocumentedParams = TRUE;
    }
  }

  //printf("Member %s hasDocumentedReturnType=%d hasReturnCommand=%d\n",
  //    name().data(),m_impl->hasDocumentedReturnType,hasReturnCommand);
  if (!m_impl->hasDocumentedReturnType && // docs not yet found
      hasReturnCommand)
  {
    m_impl->hasDocumentedReturnType = TRUE;
  }
  else if ( // see if return type is documented in a function w/o return type
            hasReturnCommand &&
            (
              returnType=="void"   || // void return type
              returnType.find("subroutine")!=-1 || // fortran subroutine
              isConstructor()      || // a constructor
              isDestructor()          // or destructor
            )
          )
  {
    warn_doc_error(getDefFileName(),getDefLine(),"documented empty return type of %s",
                          qualifiedName().data());
  }
  else if ( // see if return needs to documented 
            m_impl->hasDocumentedReturnType ||
           returnType=="void" || // void return type
           returnType.find("subroutine")!=-1 || // fortran subroutine
           isConstructor()    || // a constructor
           isDestructor()        // or destructor
          )
  {
    m_impl->hasDocumentedReturnType = TRUE;
  }
}

void MemberDefImpl::warnIfUndocumentedParams() const
{
  if (!Config_getBool(EXTRACT_ALL) &&
      Config_getBool(WARN_IF_UNDOCUMENTED) &&
      Config_getBool(WARN_NO_PARAMDOC) &&
      !isDeleted() &&
      !isReference() &&
      !Doxygen::suppressDocWarnings)
  {
    if (!m_impl->hasDocumentedParams)
    {
      warn_doc_error(getDefFileName(),getDefLine(),
          "parameters of member %s are not (all) documented",
          qPrint(qualifiedName()));
    }
    if (!m_impl->hasDocumentedReturnType &&
        isFunction() && hasDocumentation())
    {
      warn_doc_error(getDefFileName(),getDefLine(),
          "return type of member %s is not documented",
          qPrint(qualifiedName()));
    }
  }
}

bool MemberDefImpl::isFriendClass() const
{
  return (isFriend() &&
         (m_impl->type=="friend class" || m_impl->type=="friend struct" ||
          m_impl->type=="friend union"));
}

bool MemberDefImpl::isDocumentedFriendClass() const
{
  ClassDef *fcd=0;
  QCString baseName=name();
  int i=baseName.find('<');
  if (i!=-1) baseName=baseName.left(i);
  return (isFriendClass() &&
         (fcd=getClass(baseName)) && fcd->isLinkable());
}

bool MemberDefImpl::isDeleted() const
{
  return m_impl->defArgList && m_impl->defArgList->isDeleted;
}

bool MemberDefImpl::hasDocumentation() const
{
  return DefinitionImpl::hasDocumentation() ||
         (m_impl->mtype==MemberType_Enumeration && m_impl->docEnumValues) ||  // has enum values
         (m_impl->defArgList!=0 && m_impl->defArgList->hasDocumentation());   // has doc arguments
}

#if 0
bool MemberDefImpl::hasUserDocumentation() const
{
  bool hasDocs = DefinitionImpl::hasUserDocumentation();
  return hasDocs;
}
#endif


void MemberDefImpl::setMemberGroup(MemberGroup *grp)
{
  m_impl->memberGroup = grp;
}

bool MemberDefImpl::visibleMemberGroup(bool hideNoHeader) const
{
  return m_impl->memberGroup!=0 &&
          (!hideNoHeader || m_impl->memberGroup->header()!="[NOHEADER]");
}

QCString MemberDefImpl::getScopeString() const
{
  QCString result;
  if (getClassDef()) result=getClassDef()->displayName();
  else if (getNamespaceDef()) result=getNamespaceDef()->displayName();
  return result;
}

#if 0
static QCString escapeAnchor(const QCString &anchor)
{
  QCString result;
  int l = anchor.length(),i;
  for (i=0;i<l;i++)
  {
    char c = anchor.at(i);
    if ((c>='a' && c<='z') || (c>='A' && c<='Z'))
    {
      result+=c;
    }
    else
    {
      static char hexStr[]="0123456789ABCDEF";
      char escChar[]={ '_', 0, 0, 0 };
      escChar[1]=hexStr[c>>4];
      escChar[2]=hexStr[c&0xf];
      result+=escChar;
    }
  }
  return result;
}
#endif

void MemberDefImpl::setAnchor()
{
  QCString memAnchor = name();
  if (!m_impl->args.isEmpty()) memAnchor+=m_impl->args;

  memAnchor.prepend(definition()); // actually the method name is now included
            // twice, which is silly, but we keep it this way for backward
            // compatibility.

  // include number of template arguments as well,
  // to distinguish between two template
  // specializations that only differ in the template parameters.
  if (m_impl->tArgList)
  {
    char buf[20];
    qsnprintf(buf,20,"%d:",m_impl->tArgList->count());
    buf[19]='\0';
    memAnchor.prepend(buf);
  }

  // convert to md5 hash
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)memAnchor.data(),memAnchor.length(),md5_sig);
  //printf("memAnchor=%s\n",memAnchor.data());
  MD5SigToString(md5_sig,sigStr.rawData(),33);
  m_impl->anc = "a"+sigStr;
}

void MemberDefImpl::setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                            const QCString &fileName,int startLine,
                            bool hasDocs,MemberDef *member)
{
  //printf("%s MemberDefImpl::setGroupDef(%s)\n",name().data(),gd->name().data());
  m_impl->group=gd;
  m_impl->grouppri=pri;
  m_impl->groupFileName=fileName;
  m_impl->groupStartLine=startLine;
  m_impl->groupHasDocs=hasDocs;
  m_impl->groupMember=member;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setEnumScope(MemberDef *md,bool livesInsideEnum)
{
  m_impl->enumScope=md;
  m_impl->livesInsideEnum=livesInsideEnum;
  if (md->getGroupDef())
  {
    m_impl->group=md->getGroupDef();
    m_impl->grouppri=md->getGroupPri();
    m_impl->groupFileName=md->getGroupFileName();
    m_impl->groupStartLine=md->getGroupStartLine();
    m_impl->groupHasDocs=md->getGroupHasDocs();
    m_isLinkableCached = 0;
  }
}

void MemberDefImpl::setMemberClass(ClassDef *cd)
{
  m_impl->classDef=cd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  setOuterScope(cd);
}

void MemberDefImpl::setNamespace(NamespaceDef *nd)
{
  m_impl->nspace=nd;
  setOuterScope(nd);
}

MemberDef *MemberDefImpl::createTemplateInstanceMember(
        ArgumentList *formalArgs,ArgumentList *actualArgs) const
{
  //printf("  Member %s %s %s\n",typeString(),name().data(),argsString());
  ArgumentList *actualArgList = 0;
  if (m_impl->defArgList)
  {
    actualArgList = m_impl->defArgList->deepCopy();

    // replace formal arguments with actuals
    ArgumentListIterator ali(*actualArgList);
    Argument *arg;
    for (;(arg=ali.current());++ali)
    {
      arg->type = substituteTemplateArgumentsInString(arg->type,formalArgs,actualArgs);
    }
    actualArgList->trailingReturnType =
       substituteTemplateArgumentsInString(actualArgList->trailingReturnType,formalArgs,actualArgs);
  }

  QCString methodName=name();
  if (methodName.left(9)=="operator ") // conversion operator
  {
    methodName=substituteTemplateArgumentsInString(methodName,formalArgs,actualArgs);
  }

  MemberDef *imd = createMemberDef(
                       getDefFileName(),getDefLine(),getDefColumn(),
                       substituteTemplateArgumentsInString(m_impl->type,formalArgs,actualArgs),
                       methodName,
                       substituteTemplateArgumentsInString(m_impl->args,formalArgs,actualArgs),
                       m_impl->exception, m_impl->prot,
                       m_impl->virt, m_impl->stat, m_impl->related, m_impl->mtype, 0, 0, ""
                   );
  imd->setArgumentList(actualArgList);
  imd->setDefinition(substituteTemplateArgumentsInString(m_impl->def,formalArgs,actualArgs));
  imd->setBodyDef(getBodyDef());
  imd->setBodySegment(getStartBodyLine(),getEndBodyLine());
  //imd->setBodyMember(this);

  // TODO: init other member variables (if needed).
  // TODO: reimplemented info
  return imd;
}

bool MemberDefImpl::hasOneLineInitializer() const
{
  //printf("%s: init=%s, initLines=%d maxInitLines=%d userInitLines=%d\n",
  //    name().data(),m_impl->initializer.data(),m_impl->initLines,
  //    m_impl->maxInitLines,m_impl->userInitLines);
  return !m_impl->initializer.isEmpty() && m_impl->initLines==0 && // one line initializer
         ((m_impl->maxInitLines>0 && m_impl->userInitLines==-1) || m_impl->userInitLines>0); // enabled by default or explicitly
}

bool MemberDefImpl::hasMultiLineInitializer() const
{
  //printf("initLines=%d userInitLines=%d maxInitLines=%d\n",
  //    initLines,userInitLines,maxInitLines);
  return m_impl->initLines>0 &&
         ((m_impl->initLines<m_impl->maxInitLines && m_impl->userInitLines==-1) // implicitly enabled
          || m_impl->initLines<m_impl->userInitLines // explicitly enabled
         );
}

void MemberDefImpl::setInitializer(const char *initializer)
{
  m_impl->initializer=initializer;
  int l=m_impl->initializer.length();
  int p=l-1;
  while (p>=0 && isspace((uchar)m_impl->initializer.at(p))) p--;
  m_impl->initializer=m_impl->initializer.left(p+1);
  m_impl->initLines=m_impl->initializer.contains('\n');
  //printf("%s::setInitializer(%s)\n",name().data(),m_impl->initializer.data());
}

void MemberDefImpl::addListReference(Definition *)
{
  static bool optimizeOutputForC = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
  //static bool hideScopeNames     = Config_getBool(HIDE_SCOPE_NAMES);
  //static bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  SrcLangExt lang = getLanguage();
  if (!isLinkableInProject()) return;
  QCString memLabel;
  if (optimizeOutputForC)
  {
    memLabel=theTranslator->trGlobal(TRUE,TRUE);
  }
  else if (lang==SrcLangExt_Fortran)
  {
    memLabel=theTranslator->trSubprogram(TRUE,TRUE);
  }
  else
  {
    memLabel=theTranslator->trMember(TRUE,TRUE);
  }
  QCString memName = name();
  Definition *pd=getOuterScope();
  QCString pdName = pd->definitionType()==Definition::TypeClass ?
                    (dynamic_cast<ClassDef*>(pd))->displayName() : pd->name();
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  QCString memArgs;
  if (!isRelated()
      /* && commented out as a result of bug 597016
      (
       (!hideScopeNames &&                    // there is a scope
        pd && pd!=Doxygen::globalScope)       // and we can show it
       ||
       (pd=getClassDef())                     // it's a class so we
                                              // show the scope anyway
      )
      */
     )
  {
    if (isObjCMethod())
    {
      memName = "[" + pd->name() + " " + name() + "]";
    }
    else
    {
      if (pd!=Doxygen::globalScope) memName.prepend(pdName+sep);
      memArgs = argsString();
    }
  }
  QList<ListItemInfo> *xrefItems = xrefListItems();
  if (xrefItems)
  {
    addRefItem(xrefItems,
        qualifiedName()+argsString(), // argsString is needed for overloaded functions (see bug 609624)
        memLabel,
        getOutputFileBase()+"#"+anchor(),memName,memArgs,pd);
  }
}

const MemberList *MemberDefImpl::getSectionList() const
{
  const Definition *d= resolveAlias()->getOuterScope();
  char key[20];
  sprintf(key,"%p",d);
  return (d!=0 && m_impl->classSectionSDict) ? m_impl->classSectionSDict->find(key) : 0;
}

void MemberDefImpl::setSectionList(MemberList *sl)
{
  //printf("MemberDefImpl::setSectionList(%p,%p) name=%s\n",d,sl,name().data());
  const Definition *d= resolveAlias()->getOuterScope();
  char key[20];
  sprintf(key,"%p",d);
  if (m_impl->classSectionSDict==0)
  {
    m_impl->classSectionSDict = new SDict<MemberList>(7);
  }
  m_impl->classSectionSDict->append(key,sl);
}

Specifier MemberDefImpl::virtualness(int count) const
{
  if (count>25)
  {
     warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: recursion detected in overload relation for member %s!"
       ,qPrint(name())
      );
     return Normal;
  }
  Specifier v = m_impl->virt;
  MemberDef *rmd = reimplements();
  while (rmd && v==Normal)
  {
    v = rmd->virtualness(count+1)==Normal ? Normal : Virtual;
    rmd = rmd->reimplements();
  }
  return v;
}

void MemberDefImpl::writeTagFile(FTextStream &tagFile) const
{
  if (!isLinkableInProject()) return;
  tagFile << "    <member kind=\"";
  switch (m_impl->mtype)
  {
    case MemberType_Define:      tagFile << "define";      break;
    case MemberType_EnumValue:   tagFile << "enumvalue";   break;
    case MemberType_Property:    tagFile << "property";    break;
    case MemberType_Event:       tagFile << "event";       break;
    case MemberType_Variable:    tagFile << "variable";    break;
    case MemberType_Typedef:     tagFile << "typedef";     break;
    case MemberType_Enumeration: tagFile << "enumeration"; break;
    case MemberType_Function:    tagFile << "function";    break;
    case MemberType_Signal:      tagFile << "signal";      break;
    case MemberType_Friend:      tagFile << "friend";      break;
    case MemberType_DCOP:        tagFile << "dcop";        break;
    case MemberType_Slot:        tagFile << "slot";        break;
    case MemberType_Interface:   tagFile << "interface";   break;
    case MemberType_Service:     tagFile << "service";     break;
    case MemberType_Sequence:    tagFile << "sequence";    break;
    case MemberType_Dictionary:  tagFile << "dictionary";  break;
  }
  if (m_impl->prot!=Public)
  {
    tagFile << "\" protection=\"";
    if (m_impl->prot==Protected)    tagFile << "protected";
    else if (m_impl->prot==Package) tagFile << "package";
    else /* Private */              tagFile << "private";
  }
  if (m_impl->virt!=Normal)
  {
    tagFile << "\" virtualness=\"";
    if (m_impl->virt==Virtual) tagFile << "virtual";
    else /* Pure */            tagFile << "pure";
  }
  if (isStatic())
  {
    tagFile << "\" static=\"yes";
  }
  tagFile << "\">" << endl;
  if (typeString()!=QCString("@"))
  {
    tagFile << "      <type>" << convertToXML(typeString()) << "</type>" << endl;
  }
  tagFile << "      <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "      <anchorfile>" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension) << "</anchorfile>" << endl;
  tagFile << "      <anchor>" << convertToXML(anchor()) << "</anchor>" << endl;
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "      <clangid>" << convertToXML(idStr) << "</clangid>" << endl;
  }
  tagFile << "      <arglist>" << convertToXML(argsString()) << "</arglist>" << endl;
  if (isStrong())
  {
    MemberList *fmdl=m_impl->enumFields;
    if (fmdl)
    {
      MemberListIterator mli(*fmdl);
      MemberDef *fmd;
      for (mli.toFirst();(fmd=mli.current());++mli)
      {
        if (!fmd->isReference())
        {
          tagFile << "      <enumvalue file=\"" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension);
          tagFile << "\" anchor=\"" << convertToXML(fmd->anchor());
          QCString idStr = fmd->id();
          if (!idStr.isEmpty())
          {
            tagFile << "\" clangid=\"" << convertToXML(idStr);
          }
          tagFile  << "\">" << convertToXML(fmd->name()) << "</enumvalue>" << endl;
        }
      }
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "    </member>" << endl;
}

void MemberDefImpl::_computeIsConstructor()
{
  m_isConstructorCached=1; // FALSE
  if (getClassDef())
  {
    if (m_impl->isDMember) // for D
    {
      m_isConstructorCached = name()=="this" ? 2 : 1;
      return;
    }
    else if (getLanguage()==SrcLangExt_PHP) // for PHP
    {
      m_isConstructorCached = name()=="__construct" ? 2 : 1;
      return;
    }
    else if (name()=="__init__" &&
             getLanguage()==SrcLangExt_Python) // for Python
    {
      m_isConstructorCached = 2; // TRUE
      return;
    }
    else if (getLanguage()==SrcLangExt_Tcl) // for Tcl
    {
      m_isConstructorCached = name()=="constructor" ? 2 : 1;
      return;
    }
    else // for other languages
    {
      QCString locName = getClassDef()->localName();
      int i=locName.find('<');
      if (i==-1) // not a template class
      {
        m_isConstructorCached = name()==locName ? 2 : 1;
      }
      else
      {
        m_isConstructorCached = name()==locName.left(i) ? 2 : 1;
      }
      return;
    }
  }
}

bool MemberDefImpl::isConstructor() const
{
  if (m_isConstructorCached==0)
  {
    MemberDefImpl *that = (MemberDefImpl*)this;
    that->_computeIsConstructor();
  }
  ASSERT(m_isConstructorCached>0);
  return m_isConstructorCached==2;

}

void MemberDefImpl::_computeIsDestructor()
{
  bool isDestructor;
  if (m_impl->isDMember) // for D
  {
    isDestructor = name()=="~this";
  }
  else if (getLanguage()==SrcLangExt_PHP) // for PHP
  {
    isDestructor = name()=="__destruct";
  }
  else if (getLanguage()==SrcLangExt_Tcl) // for Tcl
  {
    isDestructor = name()=="destructor";
  }
  else if (name()=="__del__" &&
           getLanguage()==SrcLangExt_Python) // for Python
  {
    isDestructor=TRUE;
  }
  else // other languages
  {
    isDestructor =
           (name().find('~')!=-1 || name().find('!')!=-1)  // The ! is for C++/CLI
           && name().find("operator")==-1;
  }
  m_isDestructorCached = isDestructor ? 2 : 1;
}

bool MemberDefImpl::isDestructor() const
{
  if (m_isDestructorCached==0)
  {
    MemberDefImpl *that=(MemberDefImpl*)this;
    that->_computeIsDestructor();
  }
  ASSERT(m_isDestructorCached>0);
  return m_isDestructorCached==2;
}

void MemberDefImpl::writeEnumDeclaration(OutputList &typeDecl,
     const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd) const
{
  int enumMemCount=0;

  MemberList *fmdl=m_impl->enumFields;
  uint numVisibleEnumValues=0;
  if (fmdl)
  {
    MemberListIterator mli(*fmdl);
    MemberDef *fmd;
    for (mli.toFirst();(fmd=mli.current());++mli)
    {
      if (fmd->isBriefSectionVisible()) numVisibleEnumValues++;
    }
  }
  if (numVisibleEnumValues==0 && !isBriefSectionVisible())
  {
    return;
  }

  QCString n = name();
  int i=n.findRev("::");
  if (i!=-1) n=n.right(n.length()-i-2); // strip scope (TODO: is this needed?)
  if (n[0]!='@') // not an anonymous enum
  {
    if (isLinkableInProject() || hasDocumentedEnumValues())
    {
      //_writeTagData(compoundType);
      addToSearchIndex();
      writeLink(typeDecl,cd,nd,fd,gd);
    }
    else
    {
      typeDecl.startBold();
      typeDecl.docify(n);
      typeDecl.endBold();
    }
    typeDecl.writeChar(' ');
  }
  if (!m_impl->enumBaseType.isEmpty())
  {
    typeDecl.writeChar(':');
    typeDecl.writeChar(' ');
    typeDecl.docify(m_impl->enumBaseType);
    typeDecl.writeChar(' ');
  }

  uint enumValuesPerLine = (uint)Config_getInt(ENUM_VALUES_PER_LINE);
  if (numVisibleEnumValues>0 && enumValuesPerLine>0)
  {
    typeDecl.docify("{ ");
    if (fmdl)
    {
      MemberListIterator mli(*fmdl);
      MemberDef *fmd=mli.current();
      bool fmdVisible = fmd ? fmd->isBriefSectionVisible() : TRUE;
      while (fmd)
      {
        if (fmdVisible)
        {
          /* in html we start a new line after a number of items */
          if (numVisibleEnumValues>enumValuesPerLine
              && (enumMemCount%enumValuesPerLine)==0
             )
          {
            typeDecl.pushGeneratorState();
            typeDecl.disableAllBut(OutputGenerator::Html);
            typeDecl.enable(OutputGenerator::Latex);
            typeDecl.enable(OutputGenerator::Docbook);
            typeDecl.lineBreak();
            typeDecl.disable(OutputGenerator::Latex);
            typeDecl.disable(OutputGenerator::Docbook);
            typeDecl.writeString("&#160;&#160;");
            typeDecl.popGeneratorState();
          }

          if (fmd->hasDocumentation()) // enum value has docs
          {
            //fmd->_writeTagData(compoundType);
            fmd->addToSearchIndex();
            fmd->writeLink(typeDecl,cd,nd,fd,gd);
          }
          else // no docs for this enum value
          {
            typeDecl.startBold();
            typeDecl.docify(fmd->name());
            typeDecl.endBold();
          }
          if (fmd->hasOneLineInitializer()) // enum value has initializer
          {
            //typeDecl.writeString(" = ");
            typeDecl.writeString(" ");
            typeDecl.parseText(fmd->initializer());
          }
        }

        bool prevVisible = fmdVisible;
        ++mli;
        fmd=mli.current();
        if (fmd && (fmdVisible=fmd->isBriefSectionVisible()))
        {
          typeDecl.writeString(", ");
        }
        if (prevVisible)
        {
          typeDecl.disable(OutputGenerator::Man);
          typeDecl.writeString("\n"); // to prevent too long lines in LaTeX
          typeDecl.enable(OutputGenerator::Man);
          enumMemCount++;
        }
      }
      if (numVisibleEnumValues>enumValuesPerLine)
      {
        typeDecl.pushGeneratorState();
        typeDecl.disableAllBut(OutputGenerator::Html);
        typeDecl.lineBreak();
        typeDecl.popGeneratorState();
      }
    }
    typeDecl.docify(" }");
  }
}

void MemberDefImpl::setArgumentList(ArgumentList *al)
{
  if (m_impl->defArgList) delete m_impl->defArgList;
  m_impl->defArgList = al;
}

void MemberDefImpl::setDeclArgumentList(ArgumentList *al)
{
  if (m_impl->declArgList) delete m_impl->declArgList;
  m_impl->declArgList = al;
}

void MemberDefImpl::setTypeConstraints(ArgumentList *al)
{
  if (al==0) return;
  if (m_impl->typeConstraints) delete m_impl->typeConstraints;
  m_impl->typeConstraints = new ArgumentList;
  m_impl->typeConstraints->setAutoDelete(TRUE);
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    m_impl->typeConstraints->append(new Argument(*a));
  }
}

void MemberDefImpl::setType(const char *t)
{
  m_impl->type = t;
}

void MemberDefImpl::setAccessorType(ClassDef *cd,const char *t)
{
  m_impl->accessorClass = cd;
  m_impl->accessorType = t;
}

ClassDef *MemberDefImpl::accessorClass() const
{
  return m_impl->accessorClass;
}

void MemberDefImpl::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,docFile());
}

void MemberDefImpl::enableCallGraph(bool e)
{
  m_impl->hasCallGraph=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableCallerGraph(bool e)
{
  m_impl->hasCallerGraph=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableReferencedByRelation(bool e)
{
  m_impl->hasReferencedByRelation=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableReferencesRelation(bool e)
{
  m_impl->hasReferencesRelation=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

#if 0
bool MemberDefImpl::protectionVisible() const
{
  return m_impl->prot==Public ||
         (m_impl->prot==Private   && Config_getBool(EXTRACT_PRIVATE))   ||
         (m_impl->prot==Protected && Config_getBool(EXTRACT_PROTECTED)) ||
         (m_impl->prot==Package   && Config_getBool(EXTRACT_PACKAGE));
}
#endif

#if 0
void MemberDefImpl::setInbodyDocumentation(const char *docs,
                  const char *docFile,int docLine)
{
  m_impl->inbodyDocs = docs;
  m_impl->inbodyDocs = m_impl->inbodyDocs.stripWhiteSpace();
  m_impl->inbodyLine = docLine;
  m_impl->inbodyFile = docFile;
}
#endif

bool MemberDefImpl::isObjCMethod() const
{
  if (getClassDef() && getClassDef()->isObjectiveC() && isFunction()) return TRUE;
  return FALSE;
}

bool MemberDefImpl::isObjCProperty() const
{
  if (getClassDef() && getClassDef()->isObjectiveC() && isProperty()) return TRUE;
  return FALSE;
}

QCString MemberDefImpl::qualifiedName() const
{
  if (isObjCMethod())
  {
    QCString qm;
    if (isStatic()) qm="+"; else qm="-";
    qm+="[";
    qm+=getClassDef()->name()+" ";
    qm+=name();
    qm+="]";
    return qm;
  }
  else if (m_impl->enumScope && m_impl->enumScope->isStrong())
  {
    return m_impl->enumScope->qualifiedName()+
           getLanguageSpecificSeparator(getLanguage())+
           localName();
  }
  else
  {
    return DefinitionImpl::qualifiedName();
  }
}

void MemberDefImpl::setTagInfo(TagInfo *ti)
{
  if (ti)
  {
    //printf("%s: Setting tag name=%s anchor=%s\n",name().data(),ti->tagName.data(),ti->anchor.data());
    m_impl->anc=ti->anchor;
    setReference(ti->tagName);
    m_impl->explicitOutputFileBase = stripExtension(ti->fileName);
  }
}

QCString MemberDefImpl::objCMethodName(bool localLink,bool showStatic) const
{
  QCString qm;
  if (showStatic)
  {
    if (isStatic()) qm="+ "; else qm="- ";
  }
  qm+=name();
  if (!localLink) // link to method of same class
  {
    qm+=" (";
    qm+=getClassDef()->name();
    qm+=")";
  }
  return qm;
}

const char *MemberDefImpl::declaration() const
{
  return m_impl->decl;
}

const char *MemberDefImpl::definition() const
{
  return m_impl->def;
}

const char *MemberDefImpl::extraTypeChars() const
{
  return m_impl->extraTypeChars;
}

const char *MemberDefImpl::typeString() const
{
  return m_impl->type;
}

const char *MemberDefImpl::argsString() const
{
  return m_impl->args;
}

const char *MemberDefImpl::excpString() const
{
  return m_impl->exception;
}

const char *MemberDefImpl::bitfieldString() const
{
  return m_impl->bitfields;
}

const QCString &MemberDefImpl::initializer() const
{
  return m_impl->initializer;
}

int MemberDefImpl::initializerLines() const
{
  return m_impl->initLines;
}

uint64 MemberDefImpl::getMemberSpecifiers() const
{
  return m_impl->memSpec;
}

const ClassDef *MemberDefImpl::getClassDef() const
{
  return m_impl->classDef;
}

ClassDef *MemberDefImpl::getClassDef()
{
  return m_impl->classDef;
}

const FileDef  *MemberDefImpl::getFileDef() const
{
  return m_impl->fileDef;
}

FileDef  *MemberDefImpl::getFileDef()
{
  return m_impl->fileDef;
}

const NamespaceDef* MemberDefImpl::getNamespaceDef() const
{
  return m_impl->nspace;
}

NamespaceDef* MemberDefImpl::getNamespaceDef()
{
  return m_impl->nspace;
}

const char *MemberDefImpl::getReadAccessor() const
{
  return m_impl->read;
}

const char *MemberDefImpl::getWriteAccessor() const
{
  return m_impl->write;
}

const GroupDef *MemberDefImpl::getGroupDef() const
{
  return m_impl->group;
}

GroupDef *MemberDefImpl::getGroupDef()
{
  return m_impl->group;
}

Grouping::GroupPri_t MemberDefImpl::getGroupPri() const
{
  return m_impl->grouppri;
}

const char *MemberDefImpl::getGroupFileName() const
{
  return m_impl->groupFileName;
}

int MemberDefImpl::getGroupStartLine() const
{
  return m_impl->groupStartLine;
}

bool MemberDefImpl::getGroupHasDocs() const
{
  return m_impl->groupHasDocs;
}

Protection MemberDefImpl::protection() const
{
  return m_impl->prot;
}

MemberType MemberDefImpl::memberType() const
{
  return m_impl->mtype;
}

bool MemberDefImpl::isSignal() const
{
  return m_impl->mtype==MemberType_Signal;
}

bool MemberDefImpl::isSlot() const
{
  return m_impl->mtype==MemberType_Slot;
}

bool MemberDefImpl::isVariable() const
{
  return m_impl->mtype==MemberType_Variable;
}

bool MemberDefImpl::isEnumerate() const
{
  return m_impl->mtype==MemberType_Enumeration;
}

bool MemberDefImpl::isEnumValue() const
{
  return m_impl->mtype==MemberType_EnumValue;
}

bool MemberDefImpl::isTypedef() const
{
  return m_impl->mtype==MemberType_Typedef;
}

bool MemberDefImpl::isSequence() const
{
  return m_impl->mtype==MemberType_Sequence;
}

bool MemberDefImpl::isDictionary() const
{
  return m_impl->mtype==MemberType_Dictionary;
}

bool MemberDefImpl::isFunction() const
{
  return m_impl->mtype==MemberType_Function;
}

bool MemberDefImpl::isFunctionPtr() const
{
  return m_impl->mtype==MemberType_Variable && QCString(argsString()).find(")(")!=-1;
}

bool MemberDefImpl::isDefine() const
{
  return m_impl->mtype==MemberType_Define;
}

bool MemberDefImpl::isFriend() const
{
  return m_impl->mtype==MemberType_Friend;
}

bool MemberDefImpl::isDCOP() const
{
  return m_impl->mtype==MemberType_DCOP;
}

bool MemberDefImpl::isProperty() const
{
  return m_impl->mtype==MemberType_Property;
}

bool MemberDefImpl::isEvent() const
{
  return m_impl->mtype==MemberType_Event;
}

bool MemberDefImpl::isRelated() const
{
  return m_impl->related == Related;
}

bool MemberDefImpl::isForeign() const
{
  return m_impl->related == Foreign;
}

bool MemberDefImpl::isStatic() const
{
  return m_impl->stat;
}

bool MemberDefImpl::isInline() const
{
  return (m_impl->memSpec&Entry::Inline)!=0;
}

bool MemberDefImpl::isExplicit() const
{
  return (m_impl->memSpec&Entry::Explicit)!=0;
}

bool MemberDefImpl::isMutable() const
{
  return (m_impl->memSpec&Entry::Mutable)!=0;
}

bool MemberDefImpl::isGettable() const
{
  return (m_impl->memSpec&Entry::Gettable)!=0;
}

bool MemberDefImpl::isPrivateGettable() const
{
  return (m_impl->memSpec&Entry::PrivateGettable)!=0;
}

bool MemberDefImpl::isProtectedGettable() const
{
  return (m_impl->memSpec&Entry::ProtectedGettable)!=0;
}

bool MemberDefImpl::isSettable() const
{
  return (m_impl->memSpec&Entry::Settable)!=0;
}

bool MemberDefImpl::isPrivateSettable() const
{
  return (m_impl->memSpec&Entry::PrivateSettable)!=0;
}

bool MemberDefImpl::isProtectedSettable() const
{
  return (m_impl->memSpec&Entry::ProtectedSettable)!=0;
}

bool MemberDefImpl::isAddable() const
{
  return (m_impl->memSpec&Entry::Addable)!=0;
}

bool MemberDefImpl::isRemovable() const
{
  return (m_impl->memSpec&Entry::Removable)!=0;
}

bool MemberDefImpl::isRaisable() const
{
  return (m_impl->memSpec&Entry::Raisable)!=0;
}

bool MemberDefImpl::isReadable() const
{
  return (m_impl->memSpec&Entry::Readable)!=0;
}

bool MemberDefImpl::isWritable() const
{
  return (m_impl->memSpec&Entry::Writable)!=0;
}

bool MemberDefImpl::isFinal() const
{
  return (m_impl->memSpec&Entry::Final)!=0;
}

bool MemberDefImpl::isNew() const
{
  return (m_impl->memSpec&Entry::New)!=0;
}

bool MemberDefImpl::isSealed() const
{
  return (m_impl->memSpec&Entry::Sealed)!=0;
}

bool MemberDefImpl::isOverride() const
{
  return (m_impl->memSpec&Entry::Override)!=0;
}

bool MemberDefImpl::isInitonly() const
{
  return (m_impl->memSpec&Entry::Initonly)!=0;
}

bool MemberDefImpl::isAbstract() const
{
  return (m_impl->memSpec&Entry::Abstract)!=0;
}

bool MemberDefImpl::isOptional() const
{
  return (m_impl->memSpec&Entry::Optional)!=0;
}

bool MemberDefImpl::isRequired() const
{
  return (m_impl->memSpec&Entry::Required)!=0;
}

bool MemberDefImpl::isNonAtomic() const
{
  return (m_impl->memSpec&Entry::NonAtomic)!=0;
}

bool MemberDefImpl::isCopy() const
{
  return (m_impl->memSpec&Entry::Copy)!=0;
}

bool MemberDefImpl::isAssign() const
{
  return (m_impl->memSpec&Entry::Assign)!=0;
}

bool MemberDefImpl::isRetain() const
{
  return (m_impl->memSpec&Entry::Retain)!=0;
}

bool MemberDefImpl::isWeak() const
{
  return (m_impl->memSpec&Entry::Weak)!=0;
}

bool MemberDefImpl::isStrong() const
{
  return (m_impl->memSpec&Entry::Strong)!=0;
}

bool MemberDefImpl::isStrongEnumValue() const
{
  return m_impl->mtype==MemberType_EnumValue &&
         m_impl->enumScope &&
         m_impl->enumScope->isStrong();
}

bool MemberDefImpl::isUnretained() const
{
  return (m_impl->memSpec&Entry::Unretained)!=0;
}

bool MemberDefImpl::isTypeAlias() const
{
  return (m_impl->memSpec&Entry::Alias)!=0;
}

bool MemberDefImpl::isDefault() const
{
  return (m_impl->memSpec&Entry::Default)!=0;
}

bool MemberDefImpl::isDelete() const
{
  return (m_impl->memSpec&Entry::Delete)!=0;
}

bool MemberDefImpl::isNoExcept() const
{
  return (m_impl->memSpec&Entry::NoExcept)!=0;
}

bool MemberDefImpl::isAttribute() const
{
  return (m_impl->memSpec&Entry::Attribute)!=0;
}

bool MemberDefImpl::isUNOProperty() const
{
  return (m_impl->memSpec&Entry::Property)!=0;
}

bool MemberDefImpl::isReadonly() const
{
  return (m_impl->memSpec&Entry::Readonly)!=0;
}

bool MemberDefImpl::isBound() const
{
  return (m_impl->memSpec&Entry::Bound)!=0;
}

bool MemberDefImpl::isConstrained() const
{
  return (m_impl->memSpec&Entry::Constrained)!=0;
}

bool MemberDefImpl::isTransient() const
{
  return (m_impl->memSpec&Entry::Transient)!=0;
}

bool MemberDefImpl::isMaybeVoid() const
{
  return (m_impl->memSpec&Entry::MaybeVoid)!=0;
}

bool MemberDefImpl::isMaybeDefault() const
{
  return (m_impl->memSpec&Entry::MaybeDefault)!=0;
}

bool MemberDefImpl::isMaybeAmbiguous() const
{
  return (m_impl->memSpec&Entry::MaybeAmbiguous)!=0;
}

bool MemberDefImpl::isPublished() const
{
  return (m_impl->memSpec&Entry::Published)!=0;
}


bool MemberDefImpl::isImplementation() const
{
  return m_impl->implOnly;
}

bool MemberDefImpl::isExternal() const
{
  return m_impl->explExt;
}

bool MemberDefImpl::isTemplateSpecialization() const
{
  return m_impl->tspec;
}

bool MemberDefImpl::showInCallGraph() const
{
  return isFunction() ||
         isSlot() ||
         isConstructor() ||
         isDestructor() ||
         isObjCMethod();
}

ClassDef *MemberDefImpl::relatedAlso() const
{
  return m_impl->relatedAlso;
}

bool MemberDefImpl::hasDocumentedEnumValues() const
{
  return m_impl->docEnumValues;
}

const MemberDef *MemberDefImpl::getAnonymousEnumType() const
{
  return m_impl->annEnumType;
}

bool MemberDefImpl::isDocsForDefinition() const
{
  return m_impl->docsForDefinition;
}

const MemberDef *MemberDefImpl::getEnumScope() const
{
  return m_impl->enumScope;
}

bool MemberDefImpl::livesInsideEnum() const
{
  return m_impl->livesInsideEnum;
}

bool MemberDefImpl::isSliceLocal() const
{
  return (m_impl->memSpec&Entry::Local)!=0;
}

bool MemberDefImpl::isConstExpr() const
{
  return (m_impl->memSpec&Entry::ConstExpr)!=0;
}

const MemberList *MemberDefImpl::enumFieldList() const
{
  return m_impl->enumFields;
}

ExampleSDict *MemberDefImpl::getExamples() const
{
  return m_impl->exampleSDict;
}

bool MemberDefImpl::isPrototype() const
{
  return m_impl->proto;
}

const ArgumentList *MemberDefImpl::argumentList() const
{
  return m_impl->defArgList;
}

ArgumentList *MemberDefImpl::argumentList()
{
  return m_impl->defArgList;
}

const ArgumentList *MemberDefImpl::declArgumentList() const
{
  return m_impl->declArgList;
}

const ArgumentList *MemberDefImpl::templateArguments() const
{
  return m_impl->tArgList;
}

const QList<ArgumentList> *MemberDefImpl::definitionTemplateParameterLists() const
{
  return m_impl->defTmpArgLists;
}

int MemberDefImpl::getMemberGroupId() const
{
  return m_impl->grpId;
}

MemberGroup *MemberDefImpl::getMemberGroup() const
{
  return m_impl->memberGroup;
}

bool MemberDefImpl::fromAnonymousScope() const
{
  return m_impl->annScope;
}

bool MemberDefImpl::anonymousDeclShown() const
{
  return m_impl->annUsed;
}

void MemberDefImpl::setAnonymousUsed() const
{
  m_impl->annUsed = TRUE;
}

bool MemberDefImpl::hasCallGraph() const
{
  return m_impl->hasCallGraph;
}

bool MemberDefImpl::hasCallerGraph() const
{
  return m_impl->hasCallerGraph;
}

bool MemberDefImpl::hasReferencedByRelation() const
{
  return m_impl->hasReferencedByRelation;
}

bool MemberDefImpl::hasReferencesRelation() const
{
  return m_impl->hasReferencesRelation;
}

MemberDef *MemberDefImpl::templateMaster() const
{
  return m_impl->templateMaster;
}

bool MemberDefImpl::isTypedefValCached() const
{
  return m_impl->isTypedefValCached;
}

const ClassDef *MemberDefImpl::getCachedTypedefVal() const
{
  return m_impl->cachedTypedefValue;
}

QCString MemberDefImpl::getCachedTypedefTemplSpec() const
{
  return m_impl->cachedTypedefTemplSpec;
}

QCString MemberDefImpl::getCachedResolvedTypedef() const
{
  //printf("MemberDefImpl::getCachedResolvedTypedef()=%s m_impl=%p\n",m_impl->cachedResolvedType.data(),m_impl);
  return m_impl->cachedResolvedType;
}

MemberDef *MemberDefImpl::memberDefinition() const
{
  return m_impl->memDef;
}

MemberDef *MemberDefImpl::memberDeclaration() const
{
  return m_impl->memDec;
}

MemberDef *MemberDefImpl::inheritsDocsFrom() const
{
  return m_impl->docProvider;
}

const MemberDef *MemberDefImpl::getGroupAlias() const
{
  return m_impl->groupAlias;
}

QCString MemberDefImpl::getDeclFileName() const
{
  return m_impl->declFileName;
}

int MemberDefImpl::getDeclLine() const
{
  return m_impl->declLine;
}

int MemberDefImpl::getDeclColumn() const
{
  return m_impl->declColumn;
}


//----------------------------------------------

void MemberDefImpl::setMemberType(MemberType t)
{
  m_impl->mtype=t;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setDefinition(const char *d)
{
  m_impl->def=d;
}

void MemberDefImpl::setFileDef(FileDef *fd)
{
  m_impl->fileDef=fd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached = 0;
}

void MemberDefImpl::setProtection(Protection p)
{
  m_impl->prot=p;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setMemberSpecifiers(uint64 s)
{
  m_impl->memSpec=s;
}

void MemberDefImpl::mergeMemberSpecifiers(uint64 s)
{
  m_impl->memSpec|=s;
}

void MemberDefImpl::setBitfields(const char *s)
{
  m_impl->bitfields = QCString(s).simplifyWhiteSpace();
}

void MemberDefImpl::setMaxInitLines(int lines)
{
  if (lines!=-1)
  {
    m_impl->userInitLines=lines;
  }
}

void MemberDefImpl::setReadAccessor(const char *r)
{
  m_impl->read=r;
}

void MemberDefImpl::setWriteAccessor(const char *w)
{
  m_impl->write=w;
}

void MemberDefImpl::setTemplateSpecialization(bool b)
{
  m_impl->tspec=b;
}

void MemberDefImpl::makeRelated()
{
  m_impl->related = Related;
  m_isLinkableCached = 0;
}

void MemberDefImpl::makeForeign()
{
  m_impl->related = Foreign;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setInheritsDocsFrom(MemberDef *md)
{
  m_impl->docProvider = md;
}

void MemberDefImpl::setArgsString(const char *as)
{
  m_impl->args = as;
}

void MemberDefImpl::setRelatedAlso(ClassDef *cd)
{
  m_impl->relatedAlso=cd;
}

void MemberDefImpl::setEnumClassScope(ClassDef *cd)
{
  m_impl->classDef = cd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
}

void MemberDefImpl::setDocumentedEnumValues(bool value)
{
  m_impl->docEnumValues=value;
}

void MemberDefImpl::setAnonymousEnumType(const MemberDef *md)
{
  m_impl->annEnumType = md;
}

void MemberDefImpl::setPrototype(bool p,const QCString &df,int line,int column)
{
  m_impl->proto=p;
  if (p)
  {
    setDeclFile(df,line,column);
  }
  else
  {
    setDefFile(df,line,column);
  }
}

void MemberDefImpl::setExplicitExternal(bool b,const QCString &df,int line,int column)
{
  m_impl->explExt=b;
  if (b)
  {
    setDeclFile(df,line,column);
  }
  else
  {
    setDefFile(df,line,column);
  }
}


void MemberDefImpl::setDeclFile(const QCString &df,int line,int column)
{
  m_impl->declFileName = df;
  m_impl->declLine = line;
  m_impl->declColumn = column;
}

void MemberDefImpl::setMemberGroupId(int id)
{
  m_impl->grpId=id;
}

void MemberDefImpl::makeImplementationDetail()
{
  m_impl->implOnly=TRUE;
}

void MemberDefImpl::setFromAnonymousScope(bool b) const
{
  m_impl->annScope=b;
}

void MemberDefImpl::setFromAnonymousMember(MemberDef *m)
{
  m_impl->annMemb=m;
}

MemberDef *MemberDefImpl::fromAnonymousMember() const
{
  return m_impl->annMemb;
}

void MemberDefImpl::setTemplateMaster(MemberDef *mt)
{
  m_impl->templateMaster=mt;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setDocsForDefinition(bool b)
{
  m_impl->docsForDefinition = b;
}

void MemberDefImpl::setGroupAlias(const MemberDef *md)
{
  m_impl->groupAlias = md;
}

void MemberDefImpl::invalidateTypedefValCache()
{
  m_impl->isTypedefValCached=FALSE;
}

void MemberDefImpl::setMemberDefinition(MemberDef *md)
{
  m_impl->memDef=md;
}

void MemberDefImpl::setMemberDeclaration(MemberDef *md)
{
  m_impl->memDec=md;
}

ClassDef *MemberDefImpl::category() const
{
  return m_impl->category;
}

void MemberDefImpl::setCategory(ClassDef *def)
{
  m_impl->category = def;
}

MemberDef *MemberDefImpl::categoryRelation() const
{
  return m_impl->categoryRelation;
}

void MemberDefImpl::setCategoryRelation(MemberDef *md)
{
  m_impl->categoryRelation = md;
}

void MemberDefImpl::setEnumBaseType(const QCString &type)
{
  m_impl->enumBaseType = type;
}

QCString MemberDefImpl::enumBaseType() const
{
  return m_impl->enumBaseType;
}


void MemberDefImpl::cacheTypedefVal(const ClassDef*val, const QCString & templSpec, const QCString &resolvedType)
{
  m_impl->isTypedefValCached=TRUE;
  m_impl->cachedTypedefValue=val;
  m_impl->cachedTypedefTemplSpec=templSpec;
  m_impl->cachedResolvedType=resolvedType;
  //printf("MemberDefImpl::cacheTypedefVal=%s m_impl=%p\n",m_impl->cachedResolvedType.data(),m_impl);
}

void MemberDefImpl::copyArgumentNames(MemberDef *bmd)
{
  {
    const ArgumentList *arguments = bmd->argumentList();
    if (m_impl->defArgList && arguments)
    {
      ArgumentListIterator aliDst(*m_impl->defArgList);
      ArgumentListIterator aliSrc(*arguments);
      Argument *argDst;
      const Argument *argSrc;
      for (;(argDst=aliDst.current()) && (argSrc=aliSrc.current());++aliDst,++aliSrc)
      {
        argDst->name = argSrc->name;
        argDst->docs = argSrc->docs;
      }
    }
  }
  {
    const ArgumentList *arguments = bmd->declArgumentList();
    if (m_impl->declArgList && arguments)
    {
      ArgumentListIterator aliDst(*m_impl->declArgList);
      ArgumentListIterator aliSrc(*arguments);
      Argument *argDst;
      const Argument *argSrc;
      for (;(argDst=aliDst.current()) && (argSrc=aliSrc.current());++aliDst,++aliSrc)
      {
        argDst->name = argSrc->name;
        argDst->docs = argSrc->docs;
      }
    }
  }
}

static void invalidateCachedTypesInArgumentList(ArgumentList *al)
{
  if (al)
  {
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      a->canType.resize(0);
    }
  }
}

void MemberDefImpl::invalidateCachedArgumentTypes()
{
  invalidateCachedTypesInArgumentList(m_impl->defArgList);
  invalidateCachedTypesInArgumentList(m_impl->declArgList);
}

void MemberDefImpl::incrementFlowKeyWordCount()
{
  m_impl->numberOfFlowKW++;
}

int MemberDefImpl::numberOfFlowKeyWords() const
{
  return m_impl->numberOfFlowKW;
}

//----------------

QCString MemberDefImpl::displayName(bool) const
{
  return DefinitionImpl::name();
}

void MemberDefImpl::addToSearchIndex() const
{
  // write search index info
  if (Doxygen::searchIndex && isLinkableInProject())
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    QCString ln=localName(),qn=qualifiedName();
    Doxygen::searchIndex->addWord(ln,TRUE);
    if (ln!=qn)
    {
      Doxygen::searchIndex->addWord(qn,TRUE);
      if (getClassDef())
      {
        Doxygen::searchIndex->addWord(getClassDef()->displayName(),TRUE);
      }
      else if (getNamespaceDef())
      {
        Doxygen::searchIndex->addWord(getNamespaceDef()->displayName(),TRUE);
      }
    }
  }
}



//----------------

static void transferArgumentDocumentation(ArgumentList *decAl,ArgumentList *defAl)
{
  if (decAl && defAl)
  {
    ArgumentListIterator decAli(*decAl);
    ArgumentListIterator defAli(*defAl);
    Argument *decA,*defA;
    for (decAli.toFirst(),defAli.toFirst();
        (decA=decAli.current()) && (defA=defAli.current());
        ++decAli,++defAli)
    {
      //printf("Argument decA->name=%s (doc=%s) defA->name=%s (doc=%s)\n",
      //    decA->name.data(),decA->docs.data(),
      //    defA->name.data(),defA->docs.data()
      //      );
      if (decA->docs.isEmpty() && !defA->docs.isEmpty())
      {
        decA->docs = defA->docs.copy();
      }
      else if (defA->docs.isEmpty() && !decA->docs.isEmpty())
      {
        defA->docs = decA->docs.copy();
      }
    }
  }
}

void combineDeclarationAndDefinition(MemberDef *mdec,MemberDef *mdef)
{
  //printf("mdec=%s isPrototype()=%d\n",mdec->name().data(),mdec->isPrototype());
  if (
      (mdef->isFunction() && !mdef->isStatic() && !mdef->isPrototype()) ||
      (mdef->isVariable() && !mdef->isExternal() && !mdef->isStatic())
     )
  {
    //printf("mdef=(%p,%s) mdec=(%p,%s)\n",
    //    mdef, mdef ? mdef->name().data() : "",
    //    mdec, mdec ? mdec->name().data() : "");

    const MemberDef *cmdec = const_cast<const MemberDef*>(mdec);
    const MemberDef *cmdef = const_cast<const MemberDef*>(mdef);
    ArgumentList *mdefAl = mdef->argumentList();
    ArgumentList *mdecAl = mdec->argumentList();
    if (matchArguments2(cmdef->getOuterScope(),cmdef->getFileDef(),mdefAl,
                        cmdec->getOuterScope(),cmdec->getFileDef(),mdecAl,
                        TRUE
                       )
       ) /* match found */
    {
      //printf("Found member %s: definition in %s (doc='%s') and declaration in %s (doc='%s')\n",
      //    mn->memberName(),
      //    mdef->getFileDef()->name().data(),mdef->documentation().data(),
      //    mdec->getFileDef()->name().data(),mdec->documentation().data()
      //    );

      // first merge argument documentation
      transferArgumentDocumentation(mdecAl,mdefAl);

      /* copy documentation between function definition and declaration */
      if (!mdec->briefDescription().isEmpty())
      {
        mdef->setBriefDescription(mdec->briefDescription(),mdec->briefFile(),mdec->briefLine());
      }
      else if (!mdef->briefDescription().isEmpty())
      {
        mdec->setBriefDescription(mdef->briefDescription(),mdef->briefFile(),mdef->briefLine());
      }
      if (!mdef->documentation().isEmpty())
      {
        //printf("transferring docs mdef->mdec (%s->%s)\n",mdef->argsString(),mdec->argsString());
        mdec->setDocumentation(mdef->documentation(),mdef->docFile(),mdef->docLine());
        mdec->setDocsForDefinition(mdef->isDocsForDefinition());
        if (mdefAl!=0)
        {
          ArgumentList *mdefAlComb = new ArgumentList;
          stringToArgumentList(mdef->argsString(),mdefAlComb);
          transferArgumentDocumentation(mdefAl,mdefAlComb);
          mdec->setArgumentList(mdefAlComb);
        }
      }
      else if (!mdec->documentation().isEmpty())
      {
        //printf("transferring docs mdec->mdef (%s->%s)\n",mdec->argsString(),mdef->argsString());
        mdef->setDocumentation(mdec->documentation(),mdec->docFile(),mdec->docLine());
        mdef->setDocsForDefinition(mdec->isDocsForDefinition());
        if (mdecAl!=0)
        {
          ArgumentList *mdecAlComb = new ArgumentList;
          stringToArgumentList(mdec->argsString(),mdecAlComb);
          transferArgumentDocumentation(mdecAl,mdecAlComb);
          mdef->setDeclArgumentList(mdecAlComb);
        }
      }
      if (!mdef->inbodyDocumentation().isEmpty())
      {
        mdec->setInbodyDocumentation(mdef->inbodyDocumentation(),mdef->inbodyFile(),mdef->inbodyLine());
      }
      else if (!mdec->inbodyDocumentation().isEmpty())
      {
        mdef->setInbodyDocumentation(mdec->inbodyDocumentation(),mdec->inbodyFile(),mdec->inbodyLine());
      }
      if (mdec->getStartBodyLine()!=-1 && mdef->getStartBodyLine()==-1)
      {
        //printf("body mdec->mdef %d-%d\n",mdec->getStartBodyLine(),mdef->getEndBodyLine());
        mdef->setBodySegment(mdec->getStartBodyLine(),mdec->getEndBodyLine());
        mdef->setBodyDef(mdec->getBodyDef());
        //mdef->setBodyMember(mdec);
      }
      else if (mdef->getStartBodyLine()!=-1 && mdec->getStartBodyLine()==-1)
      {
        //printf("body mdef->mdec %d-%d\n",mdef->getStartBodyLine(),mdec->getEndBodyLine());
        mdec->setBodySegment(mdef->getStartBodyLine(),mdef->getEndBodyLine());
        mdec->setBodyDef(mdef->getBodyDef());
        //mdec->setBodyMember(mdef);
      }
      mdec->mergeMemberSpecifiers(mdef->getMemberSpecifiers());
      mdef->mergeMemberSpecifiers(mdec->getMemberSpecifiers());


      // copy group info.
      if (mdec->getGroupDef()==0 && mdef->getGroupDef()!=0)
      {
        mdec->setGroupDef(mdef->getGroupDef(),
            mdef->getGroupPri(),
            mdef->docFile(),
            mdef->docLine(),
            mdef->hasDocumentation(),
            mdef
            );
      }
      else if (mdef->getGroupDef()==0 && mdec->getGroupDef()!=0)
      {
        mdef->setGroupDef(mdec->getGroupDef(),
            mdec->getGroupPri(),
            mdec->docFile(),
            mdec->docLine(),
            mdec->hasDocumentation(),
            mdec
            );
      }


      mdec->mergeRefItems(mdef);
      mdef->mergeRefItems(mdec);

      mdef->setMemberDeclaration(mdec);
      mdec->setMemberDefinition(mdef);

      mdef->enableCallGraph(mdec->hasCallGraph() || mdef->hasCallGraph());
      mdef->enableCallerGraph(mdec->hasCallerGraph() || mdef->hasCallerGraph());
      mdec->enableCallGraph(mdec->hasCallGraph() || mdef->hasCallGraph());
      mdec->enableCallerGraph(mdec->hasCallerGraph() || mdef->hasCallerGraph());

      mdef->enableReferencedByRelation(mdec->hasReferencedByRelation() || mdef->hasReferencedByRelation());
      mdef->enableReferencesRelation(mdec->hasReferencesRelation() || mdef->hasReferencesRelation());
      mdec->enableReferencedByRelation(mdec->hasReferencedByRelation() || mdef->hasReferencedByRelation());
      mdec->enableReferencesRelation(mdec->hasReferencesRelation() || mdef->hasReferencesRelation());
    }
  }
}

QCString MemberDefImpl::briefDescription(bool abbr) const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->briefDescription(abbr);
  }
  else
  {
    return DefinitionImpl::briefDescription(abbr);
  }
}

QCString MemberDefImpl::documentation() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->documentation();
  }
  else
  {
    return DefinitionImpl::documentation();
  }
}

const ArgumentList *MemberDefImpl::typeConstraints() const
{
  return m_impl->typeConstraints;
}

bool MemberDefImpl::isFriendToHide() const
{
  static bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  bool isFriendToHide = hideFriendCompounds &&
     (m_impl->type=="friend class"  ||
      m_impl->type=="friend struct" ||
      m_impl->type=="friend union");
  return isFriendToHide;
}

bool MemberDefImpl::isNotFriend() const
{
  return !(isFriend() && isFriendToHide());
}

bool MemberDefImpl::isFunctionOrSignalSlot() const
{
  return isFunction() || isSlot() || isSignal();
}

bool MemberDefImpl::isRelatedOrFriend() const
{
  return isRelated() || isForeign() || (isFriend() && !isFriendToHide());
}

bool MemberDefImpl::isReference() const
{
  return DefinitionImpl::isReference() ||
         (m_impl->templateMaster && m_impl->templateMaster->isReference());
}

