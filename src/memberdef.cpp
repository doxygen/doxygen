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
#include <assert.h>
#include <mutex>

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
#include "vhdldocgen.h"
#include "arguments.h"
#include "memberlist.h"
#include "namespacedef.h"
#include "moduledef.h"
#include "filedef.h"
#include "config.h"
#include "definitionimpl.h"
#include "regex.h"
#include "trace.h"

//-----------------------------------------------------------------------------

class MemberDefImpl : public DefinitionMixin<MemberDefMutable>
{
  public:
    MemberDefImpl(const QCString &defFileName,int defLine,int defColumn,
              const QCString &type,const QCString &name,const QCString &args,
              const QCString &excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList &tal,
              const ArgumentList &al,const QCString &metaData);

    virtual DefType definitionType() const override        { return TypeMember; }
    virtual       MemberDef *resolveAlias() override       { return this; }
    virtual const MemberDef *resolveAlias() const override { return this; }
    virtual CodeSymbolType codeSymbolType() const override;
    virtual std::unique_ptr<MemberDef> deepCopy() const override;
    virtual void moveTo(Definition *) override;
    virtual QCString getOutputFileBase() const override;
    virtual QCString getReference() const override;
    virtual QCString anchor() const override;
    virtual QCString declaration() const override;
    virtual QCString definition() const override;
    virtual QCString typeString() const override;
    virtual QCString argsString() const override;
    virtual QCString excpString() const override;
    virtual QCString bitfieldString() const override;
    virtual QCString extraTypeChars() const override;
    virtual const QCString &initializer() const override;
    virtual int initializerLines() const override;
    virtual TypeSpecifier getMemberSpecifiers() const override;
    virtual VhdlSpecifier getVhdlSpecifiers() const override;
    virtual const MemberList *getSectionList(const Definition *) const override;
    virtual QCString displayDefinition() const override;
    virtual const ClassDef *getClassDef() const override;
    virtual       ClassDef *getClassDef() override;
    virtual const FileDef  *getFileDef() const override;
    virtual       FileDef  *getFileDef() override;
    virtual const NamespaceDef* getNamespaceDef() const override;
    virtual       NamespaceDef* getNamespaceDef() override;
    virtual const GroupDef *getGroupDef() const override;
    virtual const ModuleDef *getModuleDef() const override;
    virtual ClassDef *accessorClass() const override;
    virtual QCString getReadAccessor() const override;
    virtual QCString getWriteAccessor() const override;
    virtual Grouping::GroupPri_t getGroupPri() const override;
    virtual QCString getGroupFileName() const override;
    virtual int getGroupStartLine() const override;
    virtual bool getGroupHasDocs() const override;
    virtual QCString qualifiedName() const override;
    virtual QCString objCMethodName(bool localLink,bool showStatic) const override;
    virtual Protection protection() const override;
    virtual Specifier virtualness(int count=0) const override;
    virtual MemberType memberType() const override;
    virtual QCString memberTypeName() const override;
    virtual bool isSignal() const override;
    virtual bool isSlot() const override;
    virtual bool isVariable() const override;
    virtual bool isEnumerate() const override;
    virtual bool isEnumValue() const override;
    virtual bool isTypedef() const override;
    virtual bool isSequence() const override;
    virtual bool isDictionary() const override;
    virtual bool isFunction() const override;
    virtual bool isFunctionPtr() const override;
    virtual bool isDefine() const override;
    virtual bool isFriend() const override;
    virtual bool isDCOP() const override;
    virtual bool isProperty() const override;
    virtual bool isEvent() const override;
    virtual bool isRelated() const override;
    virtual bool isForeign() const override;
    virtual bool isStatic() const override;
    virtual bool isInline() const override;
    virtual bool isExplicit() const override;
    virtual bool isMutable() const override;
    virtual bool isGettable() const override;
    virtual bool isPrivateGettable() const override;
    virtual bool isProtectedGettable() const override;
    virtual bool isSettable() const override;
    virtual bool isPrivateSettable() const override;
    virtual bool isProtectedSettable() const override;
    virtual bool isReadable() const override;
    virtual bool isWritable() const override;
    virtual bool isAddable() const override;
    virtual bool isRemovable() const override;
    virtual bool isRaisable() const override;
    virtual bool isFinal() const override;
    virtual bool isAbstract() const override;
    virtual bool isOverride() const override;
    virtual bool isInitonly() const override;
    virtual bool isOptional() const override;
    virtual bool isRequired() const override;
    virtual bool isNonAtomic() const override;
    virtual bool isCopy() const override;
    virtual bool isAssign() const override;
    virtual bool isRetain() const override;
    virtual bool isWeak() const override;
    virtual bool isStrong() const override;
    virtual bool isEnumStruct() const override;
    virtual bool isUnretained() const override;
    virtual bool isNew() const override;
    virtual bool isSealed() const override;
    virtual bool isImplementation() const override;
    virtual bool isExternal() const override;
    virtual bool isTypeAlias() const override;
    virtual bool isDefault() const override;
    virtual bool isDelete() const override;
    virtual bool isNoExcept() const override;
    virtual bool isAttribute() const override;
    virtual bool isUNOProperty() const override;
    virtual bool isReadonly() const override;
    virtual bool isBound() const override;
    virtual bool isConstrained() const override;
    virtual bool isTransient() const override;
    virtual bool isMaybeVoid() const override;
    virtual bool isMaybeDefault() const override;
    virtual bool isMaybeAmbiguous() const override;
    virtual bool isPublished() const override;
    virtual bool isTemplateSpecialization() const override;
    virtual bool isObjCMethod() const override;
    virtual bool isObjCProperty() const override;
    virtual bool isCSharpProperty() const override;
    virtual bool isConstructor() const override;
    virtual bool isDestructor() const override;
    virtual bool hasOneLineInitializer() const override;
    virtual bool hasMultiLineInitializer() const override;
    virtual bool isCallable() const override;
    virtual bool isStrongEnumValue() const override;
    virtual bool livesInsideEnum() const override;
    virtual bool isSliceLocal() const override;
    virtual bool isConstExpr() const override;
    virtual int  numberOfFlowKeyWords() const override;
    virtual bool isFriendToHide() const override;
    virtual bool isNotFriend() const override;
    virtual bool isFunctionOrSignalSlot() const override;
    virtual bool isRelatedOrFriend() const override;
    virtual bool isLinkableInProject() const override;
    virtual bool isLinkable() const override;
    virtual bool hasDocumentation() const override;
    virtual bool isDeleted() const override;
    virtual bool isBriefSectionVisible() const override;
    virtual bool isDetailedSectionVisible(MemberListContainer container) const override;
    virtual bool hasDetailedDescription() const override;
    virtual bool isFriendClass() const override;
    virtual bool isDocumentedFriendClass() const override;
    virtual const MemberDef *reimplements() const override;
    virtual const MemberVector &reimplementedBy() const override;
    virtual bool isReimplementedBy(const ClassDef *cd) const override;
    virtual ClassDef *relatedAlso() const override;
    virtual bool hasDocumentedEnumValues() const override;
    virtual const MemberDef *getAnonymousEnumType() const override;
    virtual bool isDocsForDefinition() const override;
    virtual const MemberDef *getEnumScope() const override;
    virtual const MemberVector &enumFieldList() const override;
    virtual void setEnumBaseType(const QCString &type) override;
    virtual QCString enumBaseType() const override;
    virtual bool hasExamples() const override;
    virtual const ExampleList &getExamples() const override;
    virtual bool isPrototype() const override;
    virtual const ArgumentList &argumentList() const override;
    virtual const ArgumentList &declArgumentList() const override;
    virtual const ArgumentList &templateArguments() const override;
    virtual const ArgumentLists &definitionTemplateParameterLists() const override;
    virtual int getMemberGroupId() const override;
    virtual MemberGroup *getMemberGroup() const override;
    virtual bool fromAnonymousScope() const override;
    virtual MemberDef *fromAnonymousMember() const override;
    virtual bool hasCallGraph() const override;
    virtual bool hasCallerGraph() const override;
    virtual bool hasReferencesRelation() const override;
    virtual bool hasReferencedByRelation() const override;
    virtual bool hasInlineSource() const override;
    virtual const MemberDef *templateMaster() const override;
    virtual QCString getScopeString() const override;
    virtual ClassDef *getClassDefOfAnonymousType() const override;
    virtual bool isTypedefValCached() const override;
    virtual const ClassDef *getCachedTypedefVal() const override;
    virtual QCString getCachedTypedefTemplSpec() const override;
    virtual QCString getCachedResolvedTypedef() const override;
    virtual MemberDef *memberDefinition() const override;
    virtual MemberDef *memberDeclaration() const override;
    virtual const MemberDef *inheritsDocsFrom() const override;
    virtual const MemberDef *getGroupAlias() const override;
    virtual ClassDef *category() const override;
    virtual const MemberDef *categoryRelation() const override;
    virtual QCString displayName(bool=TRUE) const override;
    virtual QCString getDeclType() const override;
    virtual StringVector getLabels(const Definition *container) const override;
    virtual const ArgumentList &typeConstraints() const override;
    virtual QCString requiresClause() const override;
    virtual QCString documentation() const override;
    virtual QCString briefDescription(bool abbr=FALSE) const override;
    virtual QCString fieldType() const override;
    virtual bool isReference() const override;
    virtual QCString getDeclFileName() const override;
    virtual int getDeclLine() const override;
    virtual int getDeclColumn() const override;
    virtual void setMemberType(MemberType t) override;
    virtual void setDefinition(const QCString &d) override;
    virtual void setFileDef(FileDef *fd) override;
    virtual void setAnchor() override;
    virtual void setProtection(Protection p) override;
    virtual void setMemberSpecifiers(TypeSpecifier s) override;
    virtual void setVhdlSpecifiers(VhdlSpecifier s) override;
    virtual void mergeMemberSpecifiers(TypeSpecifier s) override;
    virtual void setInitializer(const QCString &i) override;
    virtual void setBitfields(const QCString &s) override;
    virtual void setMaxInitLines(int lines) override;
    virtual void setMemberClass(ClassDef *cd) override;
    virtual void setSectionList(const Definition *container,const MemberList *sl) override;
    virtual void setGroupDef(const GroupDef *gd,Grouping::GroupPri_t pri,
                     const QCString &fileName,int startLine,bool hasDocs,
                     MemberDef *member=0) override;
    virtual void setReadAccessor(const QCString &r) override;
    virtual void setWriteAccessor(const QCString &w) override;
    virtual void setTemplateSpecialization(bool b) override;
    virtual void makeRelated() override;
    virtual void makeForeign() override;
    virtual void setInheritsDocsFrom(const MemberDef *md) override;
    virtual void setTagInfo(const TagInfo *i) override;
    virtual void setArgsString(const QCString &as) override;
    virtual void setReimplements(MemberDef *md) override;
    virtual void insertReimplementedBy(MemberDef *md) override;
    virtual void setRelatedAlso(ClassDef *cd) override;
    virtual void insertEnumField(MemberDef *md) override;
    virtual void setEnumScope(const MemberDef *md,bool livesInsideEnum=FALSE) override;
    virtual void setEnumClassScope(ClassDef *cd) override;
    virtual void setDocumentedEnumValues(bool value) override;
    virtual void setAnonymousEnumType(const MemberDef *md) override;
    virtual bool addExample(const QCString &anchor,const QCString &name,const QCString &file) override;
    virtual void setPrototype(bool p,const QCString &df,int line, int column) override;
    virtual void setExplicitExternal(bool b,const QCString &df,int line,int column) override;
    virtual void setDeclFile(const QCString &df,int line,int column) override;
    virtual void moveArgumentList(std::unique_ptr<ArgumentList> al) override;
    virtual void moveDeclArgumentList(std::unique_ptr<ArgumentList> al) override;
    virtual void setDefinitionTemplateParameterLists(const ArgumentLists &lists) override;
    virtual void setTypeConstraints(const ArgumentList &al) override;
    virtual void setType(const QCString &t) override;
    virtual void setAccessorType(ClassDef *cd,const QCString &t) override;
    virtual void setNamespace(NamespaceDef *nd) override;
    virtual void setMemberGroup(MemberGroup *grp) override;
    virtual void setMemberGroupId(int id) override;
    virtual void makeImplementationDetail() override;
    virtual void setFromAnonymousScope(bool b) override;
    virtual void setFromAnonymousMember(MemberDef *m) override;
    virtual void enableCallGraph(bool e) override;
    virtual void enableCallerGraph(bool e) override;
    virtual void enableReferencedByRelation(bool e) override;
    virtual void enableReferencesRelation(bool e) override;
    virtual void enableInlineSource(bool e) override;
    virtual void mergeEnableInlineSource(bool other) override;
    virtual void setTemplateMaster(MemberDef *mt) override;
    virtual void addListReference(Definition *d) override;
    virtual void setDocsForDefinition(bool b) override;
    virtual void setGroupAlias(const MemberDef *md) override;
    virtual void cacheTypedefVal(const ClassDef *val,const QCString &templSpec,const QCString &resolvedType) override;
    virtual void invalidateTypedefValCache() override;
    virtual void invalidateCachedArgumentTypes() override;
    virtual void setMemberDefinition(MemberDef *md) override;
    virtual void setMemberDeclaration(MemberDef *md) override;
    virtual void copyArgumentNames(const MemberDef *bmd) override;
    virtual void setCategory(ClassDef *) override;
    virtual void setCategoryRelation(const MemberDef *) override;
    virtual void setDocumentation(const QCString &d,const QCString &docFile,int docLine,bool stripWhiteSpace=TRUE) override;
    virtual void setBriefDescription(const QCString &b,const QCString &briefFile,int briefLine) override;
    virtual void setInbodyDocumentation(const QCString &d,const QCString &inbodyFile,int inbodyLine) override;
    virtual void setHidden(bool b) override;
    virtual void setRequiresClause(const QCString &req) override;
    virtual void incrementFlowKeyWordCount() override;
    virtual void writeDeclaration(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool inGroup, int indentLevel,const ClassDef *inheritFrom=0,const QCString &inheritId=QCString()) const override;
    virtual void writeDocumentation(const MemberList *ml,int memCount,int memTotal,OutputList &ol,
                            const QCString &scopeName,const Definition *container,
                            bool inGroup,bool showEnumValues=FALSE,bool
                            showInline=FALSE) const override;
    virtual void writeMemberDocSimple(OutputList &ol,const Definition *container) const override;
    virtual void writeEnumDeclaration(OutputList &typeDecl,
            const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod) const override;
    virtual void writeTagFile(TextStream &,bool useQualifiedName,bool showNamespaceMembers) const override;
    virtual void warnIfUndocumented() const override;
    virtual void warnIfUndocumentedParams() const override;
    virtual bool visibleInIndex() const override;
    virtual void detectUndocumentedParams(bool hasParamCommand,bool hasReturnCommand) const override;
    virtual std::unique_ptr<MemberDef> createTemplateInstanceMember(const ArgumentList &formalArgs,
               const std::unique_ptr<ArgumentList> &actualArgs) const override;
    virtual void findSectionsInDocumentation() override;
    virtual void writeLink(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool onlyText=FALSE) const override;
    virtual void resolveUnnamedParameters(const MemberDef *md) override;
    virtual void addQualifiers(const StringVector &qualifiers) override;
    virtual StringVector getQualifiers() const override;
    virtual ClassDefMutable *getClassDefMutable() override;
    virtual void setModuleDef(ModuleDef *mod) override;

  private:
    void _computeLinkableInProject();
    void _computeIsConstructor();
    void _computeIsDestructor();
    void _writeGroupInclude(OutputList &ol,bool inGroup) const;
    void _writeMultiLineInitializer(OutputList &ol,const QCString &scopeName) const;
    void _writeCallGraph(OutputList &ol) const;
    void _writeCallerGraph(OutputList &ol) const;
    void _writeReimplements(OutputList &ol) const;
    bool _isReimplements() const;
    void _writeReimplementedBy(OutputList &ol) const;
    size_t _countReimplementedBy() const;
    void _writeExamples(OutputList &ol) const;
    void _writeTypeConstraints(OutputList &ol) const;
    void _writeEnumValues(OutputList &ol,const Definition *container,
                          const QCString &cfname,const QCString &ciname,
                          const QCString &cname) const;
    void _writeCategoryRelation(OutputList &ol) const;
    void _writeTagData(const DefType) const;
    void _writeTemplatePrefix(OutputList &ol, const Definition *def,
                              const ArgumentList &al, bool writeReqClause=true) const;
    bool _hasVisibleCallGraph() const;
    bool _hasVisibleCallerGraph() const;
    bool _isAnonymousBitField() const;

    // disable copying of member defs
    MemberDefImpl(const MemberDefImpl &);
    MemberDefImpl &operator=(const MemberDefImpl &);


    void init(Definition *def,const QCString &t,const QCString &a,const QCString &e,
              Protection p,Specifier v,bool s,Relationship r,
              MemberType mt,const ArgumentList &tal,
              const ArgumentList &al,const QCString &meta
             );

    uint8_t m_isLinkableCached;    // 0 = not cached, 1=FALSE, 2=TRUE
    uint8_t m_isConstructorCached; // 0 = not cached, 1=FALSE, 2=TRUE
    uint8_t m_isDestructorCached;  // 1 = not cached, 1=FALSE, 2=TRUE

    ClassDef     *m_classDef = 0; // member of or related to
    FileDef      *m_fileDef  = 0; // member of file definition
    NamespaceDef *m_nspace   = 0; // the namespace this member is in.
    ModuleDef    *m_moduleDef = 0;

    const MemberDef  *m_enumScope = 0;    // the enclosing scope, if this is an enum field
    bool        m_livesInsideEnum = false;
    const MemberDef  *m_annEnumType = 0;  // the anonymous enum that is the type of this member
    MemberVector m_enumFields;              // enumeration fields

    MemberDef  *m_redefines = 0;    // the members that this member redefines
    MemberVector m_redefinedBy;             // the list of members that redefine this one

    MemberDef  *m_memDef = 0;       // member definition for this declaration
    MemberDef  *m_memDec = 0;       // member declaration for this definition
    ClassDef   *m_relatedAlso = 0;  // points to class marked by relatedAlso

    ExampleList m_examples;     // a dictionary of all examples for quick access

    QCString m_type;            // return actual type
    QCString m_accessorType;    // return type that tell how to get to this member
    ClassDef *m_accessorClass = 0;  // class that this member accesses (for anonymous types)
    QCString m_args;            // function arguments/variable array specifiers
    QCString m_def;             // member definition in code (fully qualified name)
    QCString m_anc;             // HTML anchor name
    Specifier m_virt = Specifier::Normal;  // normal/virtual/pure virtual
    Protection m_prot = Protection::Public; // protection type [Public/Protected/Private]
    QCString m_decl;            // member declaration in class

    QCString m_bitfields;       // struct member bitfields
    QCString m_read;            // property read accessor
    QCString m_write;           // property write accessor
    QCString m_exception;       // exceptions that can be thrown
    QCString m_initializer;     // initializer
    QCString m_extraTypeChars;  // extra type info found after the argument list
    QCString m_enumBaseType;    // base type of the enum (C++11)
    QCString m_requiresClause;  // requires clause (C++20)
    int m_initLines = 0;            // number of lines in the initializer

    TypeSpecifier m_memSpec;          // The specifiers present for this member
    VhdlSpecifier m_vhdlSpec;
    MemberType m_mtype = MemberType_Define; // returns the kind of member
    int m_maxInitLines = 0;         // when the initializer will be displayed
    int m_userInitLines = 0;        // result of explicit \hideinitializer or \showinitializer
    MemberDef  *m_annMemb = 0;

    ArgumentList m_defArgList;    // argument list of this member definition
    ArgumentList m_declArgList;   // argument list of this member declaration

    ArgumentList m_tArgList;      // template argument list of function template
    ArgumentList m_typeConstraints; // type constraints for template parameters
    MemberDef *m_templateMaster = 0;
    ArgumentLists m_defTmpArgLists; // lists of template argument lists
                                         // (for template functions in nested template classes)

    QCString m_metaData;        // Slice metadata.

    mutable ClassDef *m_cachedAnonymousType = 0; // if the member has an anonymous compound
                                   // as its type then this is computed by
                                   // getClassDefOfAnonymousType() and
                                   // cached here.
    std::map<const Definition *,const MemberList *> m_sectionMap;

    const MemberDef *m_groupAlias = 0;    // Member containing the definition
    int m_grpId = 0;                // group id
    MemberGroup *m_memberGroup = 0; // group's member definition
    const GroupDef *m_group = 0;          // group in which this member is in
    Grouping::GroupPri_t m_grouppri = Grouping::GROUPING_AUTO_DEF; // priority of this definition
    QCString m_groupFileName;   // file where this grouping was defined
    int m_groupStartLine = 0;       // line  "      "      "     "     "
    MemberDef *m_groupMember = 0;

    bool m_isTypedefValCached = false;
    const ClassDef *m_cachedTypedefValue = 0;
    QCString m_cachedTypedefTemplSpec;
    QCString m_cachedResolvedType;

    // documentation inheritance
    const MemberDef *m_docProvider = 0;

    // to store the output file base from tag files
    QCString m_explicitOutputFileBase;

    // to store extra qualifiers
    StringVector m_qualifiers;

    // objective-c
    bool m_implOnly = false; // function found in implementation but not
                           // in the interface
    mutable bool m_hasDocumentedParams = false;      // guard to show only the first warning
    mutable bool m_hasDocumentedReturnType = false;  // guard to show only the first warning
    bool m_isDMember = false;
    Relationship m_related = Relationship::Member;    // relationship of this to the class
    bool m_stat = false;                // is it a static function?
    bool m_proto = false;               // is it a prototype?
    bool m_docEnumValues = false;       // is an enum with documented enum values.

    mutable bool m_annScope = false;    // member is part of an anonymous scope
    mutable bool m_hasDetailedDescriptionCached = false;
    mutable bool m_detailedDescriptionCachedValue = false;
                                      // const member.
    bool m_hasCallGraph = false;
    bool m_hasCallerGraph = false;
    bool m_hasReferencedByRelation = false;
    bool m_hasReferencesRelation = false;
    bool m_hasInlineSource = false;
    bool m_explExt = false;             // member was explicitly declared external
    bool m_tspec = false;               // member is a template specialization
    bool m_groupHasDocs = false;        // true if the entry that caused the grouping was documented
    bool m_docsForDefinition = false;   // TRUE => documentation block is put before
                                      //         definition.
                                      // FALSE => block is put before declaration.
    ClassDef *m_category = 0;
    const MemberDef *m_categoryRelation = 0;
    QCString m_declFileName;
    int m_declLine = -1;
    int m_declColumn = -1;
    int m_numberOfFlowKW = 0;
};

std::unique_ptr<MemberDef> createMemberDef(const QCString &defFileName,int defLine,int defColumn,
              const QCString &type,const QCString &name,const QCString &args,
              const QCString &excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList &tal,
              const ArgumentList &al,const QCString &metaData)
{
  return std::make_unique<MemberDefImpl>(defFileName,defLine,defColumn,type,name,args,excp,prot,virt,
                           stat,related,t,tal,al,metaData);
}

//-----------------------------------------------------------------------------

class MemberDefAliasImpl : public DefinitionAliasMixin<MemberDef>
{
  public:
    MemberDefAliasImpl(const Definition *newScope,const MemberDef *md)
    : DefinitionAliasMixin(newScope,md), m_memberGroup(0) { init(); }
    virtual ~MemberDefAliasImpl() { deinit(); }
    virtual DefType definitionType() const { return TypeMember; }

    const MemberDef *getMdAlias() const           { return toMemberDef(getAlias()); }
          MemberDef *getMdAlias()                 { return toMemberDef(const_cast<Definition*>(getAlias())); }
    virtual       MemberDef *resolveAlias()       { return const_cast<MemberDef*>(getMdAlias()); }
    virtual const MemberDef *resolveAlias() const { return getMdAlias(); }

    virtual std::unique_ptr<MemberDef> deepCopy() const  {
      return createMemberDefAlias(getScope(),getMdAlias());
    }
    virtual void moveTo(Definition *) {}

    virtual const QCString &name() const
    { return getMdAlias()->name(); }
    virtual CodeSymbolType codeSymbolType() const
    { return getMdAlias()->codeSymbolType(); }
    virtual QCString getOutputFileBase() const
    { return getMdAlias()->getOutputFileBase(); }
    virtual QCString getReference() const
    { return getMdAlias()->getReference(); }
    virtual QCString anchor() const
    { return getMdAlias()->anchor(); }
    virtual QCString declaration() const
    { return getMdAlias()->declaration(); }
    virtual QCString definition() const
    { return getMdAlias()->definition(); }
    virtual QCString typeString() const
    { return getMdAlias()->typeString(); }
    virtual QCString argsString() const
    { return getMdAlias()->argsString(); }
    virtual QCString excpString() const
    { return getMdAlias()->excpString(); }
    virtual QCString bitfieldString() const
    { return getMdAlias()->bitfieldString(); }
    virtual QCString extraTypeChars() const
    { return getMdAlias()->extraTypeChars(); }
    virtual const QCString &initializer() const
    { return getMdAlias()->initializer(); }
    virtual int initializerLines() const
    { return getMdAlias()->initializerLines(); }
    virtual TypeSpecifier getMemberSpecifiers() const
    { return getMdAlias()->getMemberSpecifiers(); }
    virtual VhdlSpecifier getVhdlSpecifiers() const
    { return getMdAlias()->getVhdlSpecifiers(); }
    virtual const MemberList *getSectionList(const Definition *container) const
    { return getMdAlias()->getSectionList(container); }
    virtual QCString displayDefinition() const
    { return getMdAlias()->displayDefinition(); }

    virtual const ClassDef *getClassDef() const
    { return getMdAlias()->getClassDef(); }
    virtual       ClassDef *getClassDef()
    { return getMdAlias()->getClassDef(); }

    virtual const FileDef *getFileDef() const
    { return getMdAlias()->getFileDef(); }
    virtual       FileDef *getFileDef()
    { return getMdAlias()->getFileDef(); }

    virtual const ModuleDef *getModuleDef() const
    { return getMdAlias()->getModuleDef(); }

    virtual const NamespaceDef* getNamespaceDef() const
    { return getMdAlias()->getNamespaceDef(); }
    virtual       NamespaceDef* getNamespaceDef()
    { return getMdAlias()->getNamespaceDef(); }

    virtual const ClassDef *accessorClass() const
    { return getMdAlias()->accessorClass(); }
    virtual QCString getReadAccessor() const
    { return getMdAlias()->getReadAccessor(); }
    virtual QCString getWriteAccessor() const
    { return getMdAlias()->getWriteAccessor(); }
    virtual const GroupDef *getGroupDef() const
    { return getMdAlias()->getGroupDef(); }
    virtual Grouping::GroupPri_t getGroupPri() const
    { return getMdAlias()->getGroupPri(); }
    virtual QCString getGroupFileName() const
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
    virtual Specifier virtualness(int /* count */) const
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
    virtual bool isEnumStruct() const
    { return getMdAlias()->isEnumStruct(); }
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
    virtual bool isCSharpProperty() const
    { return getMdAlias()->isCSharpProperty(); }
    virtual bool isConstructor() const
    { return getMdAlias()->isConstructor(); }
    virtual bool isDestructor() const
    { return getMdAlias()->isDestructor(); }
    virtual bool hasOneLineInitializer() const
    { return getMdAlias()->hasOneLineInitializer(); }
    virtual bool hasMultiLineInitializer() const
    { return getMdAlias()->hasMultiLineInitializer(); }
    virtual bool isCallable() const
    { return getMdAlias()->isCallable(); }
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
    virtual bool isDetailedSectionVisible(MemberListContainer container) const
    { return getMdAlias()->isDetailedSectionVisible(container); }
    virtual bool hasDetailedDescription() const
    { return getMdAlias()->hasDetailedDescription(); }
    virtual bool isFriendClass() const
    { return getMdAlias()->isFriendClass(); }
    virtual bool isDocumentedFriendClass() const
    { return getMdAlias()->isDocumentedFriendClass(); }
    virtual const MemberDef *reimplements() const
    { return getMdAlias()->reimplements(); }
    virtual const MemberVector &reimplementedBy() const
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
    virtual const MemberVector &enumFieldList() const
    { return getMdAlias()->enumFieldList(); }
    virtual QCString enumBaseType() const
    { return getMdAlias()->enumBaseType(); }
    virtual bool hasExamples() const
    { return getMdAlias()->hasExamples(); }
    virtual const ExampleList &getExamples() const
    { return getMdAlias()->getExamples(); }
    virtual bool isPrototype() const
    { return getMdAlias()->isPrototype(); }
    virtual const ArgumentList &argumentList() const
    { return getMdAlias()->argumentList(); }
    virtual const ArgumentList &declArgumentList() const
    { return getMdAlias()->declArgumentList(); }
    virtual const ArgumentList &templateArguments() const
    { return getMdAlias()->templateArguments(); }
    virtual const ArgumentLists &definitionTemplateParameterLists() const
    { return getMdAlias()->definitionTemplateParameterLists(); }
    virtual int getMemberGroupId() const
    { return getMdAlias()->getMemberGroupId(); }
    virtual MemberGroup *getMemberGroup() const
    { return m_memberGroup; }
    virtual bool fromAnonymousScope() const
    { return getMdAlias()->fromAnonymousScope(); }
    virtual MemberDef *fromAnonymousMember() const
    { return getMdAlias()->fromAnonymousMember(); }
    virtual bool hasCallGraph() const
    { return getMdAlias()->hasCallGraph(); }
    virtual bool hasCallerGraph() const
    { return getMdAlias()->hasCallerGraph(); }
    virtual bool hasReferencesRelation() const
    { return getMdAlias()->hasReferencesRelation(); }
    virtual bool hasReferencedByRelation() const
    { return getMdAlias()->hasReferencedByRelation(); }
    virtual bool hasInlineSource() const
    { return getMdAlias()->hasInlineSource(); }
    virtual StringVector getQualifiers() const
    { return getMdAlias()->getQualifiers(); }
    virtual const MemberDef *templateMaster() const
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
    virtual const MemberDef *inheritsDocsFrom() const
    { return getMdAlias()->inheritsDocsFrom(); }
    virtual const MemberDef *getGroupAlias() const
    { return getMdAlias()->getGroupAlias(); }
    virtual ClassDef *category() const
    { return getMdAlias()->category(); }
    virtual const MemberDef *categoryRelation() const
    { return getMdAlias()->categoryRelation(); }
    virtual QCString displayName(bool b=TRUE) const
    { return getMdAlias()->displayName(b); }
    virtual QCString getDeclType() const
    { return getMdAlias()->getDeclType(); }
    virtual StringVector getLabels(const Definition *container) const
    { return getMdAlias()->getLabels(container); }
    virtual const ArgumentList &typeConstraints() const
    { return getMdAlias()->typeConstraints(); }
    virtual QCString documentation() const
    { return getMdAlias()->documentation(); }
    virtual QCString briefDescription(bool /* abbr=FALSE */) const
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
    virtual QCString requiresClause() const
    { return getMdAlias()->requiresClause(); }
    virtual bool visibleInIndex() const
    { return getMdAlias()->visibleInIndex(); }

    virtual void warnIfUndocumented() const {}
    virtual void warnIfUndocumentedParams() const {}
    virtual void detectUndocumentedParams(bool /* hasParamCommand */,bool /* hasReturnCommand */) const {}
    virtual void setMemberGroup(MemberGroup *grp) { m_memberGroup = grp; }
    virtual std::unique_ptr<MemberDef> createTemplateInstanceMember(const ArgumentList &formalArgs,
               const std::unique_ptr<ArgumentList> &actualArgs) const
    { return getMdAlias()->createTemplateInstanceMember(formalArgs,actualArgs); }

    virtual void writeDeclaration(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool inGroup, int indentLevel, const ClassDef *inheritFrom=0,const QCString &inheritId=QCString()) const
    {
      getMdAlias()->writeDeclaration(ol,cd,nd,fd,gd,mod,inGroup,indentLevel,inheritFrom,inheritId);
    }
    virtual void writeEnumDeclaration(OutputList &typeDecl,
            const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod) const
    {
      getMdAlias()->writeEnumDeclaration(typeDecl,cd,nd,fd,gd,mod);
    }
    virtual void writeLink(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool onlyText=FALSE) const
    {
      getMdAlias()->writeLink(ol,cd,nd,fd,gd,mod,onlyText);
    }
  private:
    MemberGroup *m_memberGroup; // group's member definition
};


std::unique_ptr<MemberDef> createMemberDefAlias(const Definition *newScope,const MemberDef *aliasMd)
{
  auto amd = std::make_unique<MemberDefAliasImpl>(newScope,aliasMd);
  //printf("amd: name=%s displayName=%s\n",qPrint(amd->name()),qPrint(amd->displayName()));
  return amd;
}

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
    uint32_t j=clRealName.length()+i;
    if (s.length()==j || (s.at(j)!='<' && !isId(s.at(j))))
    { // add template names
      //printf("Adding %s+%s\n",qPrint(clRealName),qPrint(t));
      result+=clRealName+t;
    }
    else
    { // template names already present
      //printf("Adding %s\n",qPrint(clRealName));
      result+=clRealName;
    }
    p=i+clRealName.length();
  }
  result+=s.right(s.length()-p);
  //printf("addTemplateNames(%s,%s,%s)=%s\n",qPrint(s),qPrint(n),qPrint(t),qPrint(result));
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
  const ArgumentList &defArgList=(md->isDocsForDefinition()) ?
                             md->argumentList() : md->declArgumentList();
  //printf("writeDefArgumentList '%s' isDocsForDefinition()=%d hasParameters()=%d (%s)\n",
  //    qPrint(md->name()),md->isDocsForDefinition(),defArgList.hasParameters(),qPrint(argListToString(defArgList)));
  if (!defArgList.hasParameters() || md->isProperty() || md->isTypedef())
  {
    return FALSE; // member has no function like argument list
  }

  if (!md->isDefine()) ol.docify(" ");

  //printf("writeDefArgList(%d)\n",defArgList->count());
  ol.pushGeneratorState();
  //ol.disableAllBut(OutputType::Html);
  bool htmlOn  = ol.isEnabled(OutputType::Html);
  bool latexOn = ol.isEnabled(OutputType::Latex);
  bool docbookOn = ol.isEnabled(OutputType::Docbook);
  {
    // html and latex
    if (htmlOn)  ol.enable(OutputType::Html);
    if (latexOn) ol.enable(OutputType::Latex);
    if (docbookOn) ol.enable(OutputType::Docbook);

    ol.endMemberDocName();
    ol.startParameterList(!md->isObjCMethod());
  }
  ol.enableAll();
  ol.disable(OutputType::Html);
  ol.disable(OutputType::Latex);
  ol.disable(OutputType::Docbook);
  {
    // other formats
    if (!md->isObjCMethod()) ol.docify("("); // start argument list
    ol.endMemberDocName();
  }
  ol.popGeneratorState();
  //printf("===> name=%s isDefine=%d\n",qPrint(md->name()),md->isDefine());

  QCString cName;
  if (scope)
  {
    cName=scope->name();
    int il=cName.find('<');
    int ir=cName.findRev('>');
    if (il!=-1 && ir!=-1 && ir>il)
    {
      cName=cName.mid(il,ir-il+1);
      //printf("1. cName=%s\n",qPrint(cName));
    }
    else if (scope->definitionType()==Definition::TypeClass)
    {
      cName=tempArgListToString((toClassDef(scope))->templateArguments(),
                             scope->getLanguage());
      //printf("2. cName=%s\n",qPrint(cName));
    }
    else // no template specifier
    {
      cName.resize(0);
    }
  }
  //printf("~~~ %s cName=%s\n",qPrint(md->name()),qPrint(cName));

  bool first=TRUE;
  bool paramTypeStarted=FALSE;
  bool isDefine = md->isDefine();
  auto alIt = defArgList.begin();
  while (alIt!=defArgList.end())
  {
    Argument a = *alIt;
    if (isDefine || first)
    {
      ol.startParameterType(first,QCString());
      paramTypeStarted=TRUE;
      if (isDefine)
      {
        ol.endParameterType();
        ol.startParameterName(TRUE);
      }
    }

    if (!a.attrib.isEmpty() && !md->isObjCMethod()) // argument has an IDL attribute
    {
      ol.docify(a.attrib+" ");
    }

    {
      QCString n=a.type;
      if (md->isObjCMethod()) { n.prepend("("); n.append(")"); }
      if (a.type!="...")
      {
        if (!cName.isEmpty() && scope && scope!=Doxygen::globalScope)
        {
          n=addTemplateNames(n,scope->name(),cName);
        }
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
      ol.startParameterName(defArgList.size()<2);
    }
    if (!a.name.isEmpty() || a.type=="...") // argument has a name
    {
      ol.disable(OutputType::Latex);
      ol.disable(OutputType::Docbook);
      ol.disable(OutputType::Html);
      ol.docify(" "); /* man page */
      if (htmlOn) ol.enable(OutputType::Html);
      ol.disable(OutputType::Man);
      ol.startEmphasis();
      ol.enable(OutputType::Man);
      if (latexOn) ol.enable(OutputType::Latex);
      if (docbookOn) ol.enable(OutputType::Docbook);
      if (a.name.isEmpty()) ol.docify(a.type); else ol.docify(a.name);
      ol.disable(OutputType::Man);
      ol.disable(OutputType::Latex);
      ol.disable(OutputType::Docbook);
      ol.endEmphasis();
      ol.enable(OutputType::Man);
      if (latexOn) ol.enable(OutputType::Latex);
      if (docbookOn) ol.enable(OutputType::Docbook);
    }
    if (!a.array.isEmpty())
    {
      ol.docify(a.array);
    }
    if (!a.defval.isEmpty()) // write the default value
    {
      QCString n=a.defval;
      if (scope && scope!=Doxygen::globalScope && !cName.isEmpty())
      {
        n=addTemplateNames(n,scope->name(),cName);
      }
      ol.docify(" = ");

      ol.startTypewriter();
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n,FALSE,TRUE,TRUE);
      ol.endTypewriter();

    }
    ++alIt;
    if (alIt!=defArgList.end())
    {
      a = *alIt;
      if (!md->isObjCMethod()) ol.docify(", "); // there are more arguments
      if (!isDefine)
      {
        QCString key;
        if (md->isObjCMethod() && a.attrib.length()>=2)
        {
          //printf("Found parameter keyword %s\n",a.qPrint(attrib));
          // strip [ and ]
          key=a.attrib.mid(1,a.attrib.length()-2);
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
  ol.disable(OutputType::Html);
  ol.disable(OutputType::Latex);
  ol.disable(OutputType::Docbook);
  if (!md->isObjCMethod()) ol.docify(")"); // end argument list
  ol.enableAll();
  if (htmlOn) ol.enable(OutputType::Html);
  if (latexOn) ol.enable(OutputType::Latex);
  if (docbookOn) ol.enable(OutputType::Docbook);
  if (first) ol.startParameterName(defArgList.size()<2);
  ol.endParameterName(TRUE,defArgList.size()<2,!md->isObjCMethod());
  ol.popGeneratorState();
  if (!md->extraTypeChars().isEmpty())
  {
    ol.docify(md->extraTypeChars());
  }
  if (defArgList.constSpecifier())
  {
    ol.docify(" const");
  }
  if (defArgList.volatileSpecifier())
  {
    ol.docify(" volatile");
  }
  if (defArgList.refQualifier()==RefQualifierLValue)
  {
    ol.docify(" &");
  }
  else if (defArgList.refQualifier()==RefQualifierRValue)
  {
    ol.docify(" &&");
  }
  if (!defArgList.trailingReturnType().isEmpty())
  {
    linkifyText(TextGeneratorOLImpl(ol), // out
                scope,                   // scope
                md->getBodyDef(),        // fileScope
                md,                      // self
                defArgList.trailingReturnType(), // text
                FALSE                    // autoBreak
               );

  }
  return TRUE;
}

static void writeExceptionListImpl(
        OutputList &ol, const ClassDef *cd, const MemberDef *md, QCString const& exception)
{
  // this is ordinary exception spec - there must be a '('
  //printf("exception='%s'\n",qPrint(exception));
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
      ol.exceptionEntry(QCString(),false);
      index=comma;
      comma = exception.find(',', index);
    }
    int close = exception.find(')', index);
    if (close!=-1)
    {
      QCString type=removeRedundantWhiteSpace(exception.mid(index,close-index));
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,type);
      ol.exceptionEntry(QCString(),true);
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void MemberDefImpl::init(Definition *d,
                     const QCString &t,const QCString &a,const QCString &e,
                     Protection p,Specifier v,bool s,Relationship r,
                     MemberType mt,const ArgumentList &tal,
                     const ArgumentList &al,const QCString &meta
                    )
{
  m_classDef=0;
  m_fileDef=0;
  m_moduleDef=0;
  m_redefines=0;
  m_relatedAlso=0;
  m_accessorClass=0;
  m_nspace=0;
  m_memDef=0;
  m_memDec=0;
  m_group=0;
  m_grpId=-1;
  m_enumScope=0;
  m_livesInsideEnum=FALSE;
  m_hasCallGraph = FALSE;
  m_hasCallerGraph = FALSE;
  m_hasReferencedByRelation = FALSE;
  m_hasReferencesRelation = FALSE;
  m_hasInlineSource = FALSE;
  m_initLines=0;
  m_type=t;
  if (mt==MemberType_Typedef) m_type.stripPrefix("typedef ");
  //  type.stripPrefix("struct ");
  //  type.stripPrefix("class " );
  //  type.stripPrefix("union " );
  m_type=removeRedundantWhiteSpace(m_type);
  m_args=a;
  m_args=removeRedundantWhiteSpace(m_args);
  if (m_type.isEmpty()) m_decl=d->name()+m_args; else m_decl=m_type+" "+d->name()+m_args;

  m_memberGroup=0;
  m_virt=v;
  m_prot=p;
  m_related=r;
  m_stat=s;
  m_mtype=mt;
  m_exception=e;
  m_proto=FALSE;
  m_annScope=FALSE;
  m_memSpec=TypeSpecifier();
  m_vhdlSpec=VhdlSpecifier::UNKNOWN;
  m_annMemb=0;
  m_annEnumType=0;
  m_groupAlias=0;
  m_explExt=FALSE;
  m_tspec=FALSE;
  m_cachedAnonymousType=0;
  m_maxInitLines=Config_getInt(MAX_INITIALIZER_LINES);
  m_userInitLines=-1;
  m_docEnumValues=FALSE;
  // copy function template arguments (if any)
  m_tArgList = tal;
  //printf("new member al=%p\n",al);
  // copy function definition arguments (if any)
  m_defArgList = al;
  // convert function declaration arguments (if any)
  if (!m_args.isEmpty())
  {
    m_declArgList = *stringToArgumentList(d->getLanguage(),m_args,&m_extraTypeChars);
    //printf("setDeclArgList %s to %s const=%d\n",qPrint(args),
    //    qPrint(argListToString(declArgList)),declArgList->constSpecifier);
  }
  m_metaData = meta;
  m_templateMaster = 0;
  m_docsForDefinition = TRUE;
  m_isTypedefValCached = FALSE;
  m_cachedTypedefValue = 0;
  m_implOnly=FALSE;
  m_groupMember = 0;
  m_hasDocumentedParams = FALSE;
  m_hasDocumentedReturnType = FALSE;
  m_docProvider = 0;
  m_isDMember = d->getDefFileName().lower().endsWith(".d");
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

MemberDefImpl::MemberDefImpl(const QCString &df,int dl,int dc,
                     const QCString &t,const QCString &na,const QCString &a,const QCString &e,
                     Protection p,Specifier v,bool s,Relationship r,MemberType mt,
                     const ArgumentList &tal,const ArgumentList &al,const QCString &meta
                    ) : DefinitionMixin(df,dl,dc,removeRedundantWhiteSpace(na))
{
  //printf("MemberDefImpl::MemberDef(%s)\n",na);
  init(this,t,a,e,p,v,s,r,mt,tal,al,meta);
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDefImpl::MemberDefImpl(const MemberDefImpl &md) : DefinitionMixin(md)
{
  m_classDef                       = md.m_classDef                       ;
  m_fileDef                        = md.m_fileDef                        ;
  m_moduleDef                      = md.m_moduleDef                      ;
  m_nspace                         = md.m_nspace                         ;
  m_enumScope                      = md.m_enumScope                      ;
  m_livesInsideEnum                = md.m_livesInsideEnum                ;
  m_annEnumType                    = md.m_annEnumType                    ;
  m_enumFields                     = md.m_enumFields                     ;
  m_redefines                      = md.m_redefines                      ;
  m_redefinedBy                    = md.m_redefinedBy                    ;
  m_memDef                         = md.m_memDef                         ;
  m_memDec                         = md.m_memDec                         ;
  m_relatedAlso                    = md.m_relatedAlso                    ;
  m_examples                       = md.m_examples                       ;
  m_type                           = md.m_type                           ;
  m_accessorType                   = md.m_accessorType                   ;
  m_accessorClass                  = md.m_accessorClass                  ;
  m_args                           = md.m_args                           ;
  m_def                            = md.m_def                            ;
  m_anc                            = md.m_anc                            ;
  m_virt                           = md.m_virt                           ;
  m_prot                           = md.m_prot                           ;
  m_decl                           = md.m_decl                           ;
  m_bitfields                      = md.m_bitfields                      ;
  m_read                           = md.m_read                           ;
  m_write                          = md.m_write                          ;
  m_exception                      = md.m_exception                      ;
  m_initializer                    = md.m_initializer                    ;
  m_extraTypeChars                 = md.m_extraTypeChars                 ;
  m_enumBaseType                   = md.m_enumBaseType                   ;
  m_requiresClause                 = md.m_requiresClause                 ;
  m_initLines                      = md.m_initLines                      ;
  m_memSpec                        = md.m_memSpec                        ;
  m_vhdlSpec                       = md.m_vhdlSpec                       ;
  m_mtype                          = md.m_mtype                          ;
  m_maxInitLines                   = md.m_maxInitLines                   ;
  m_userInitLines                  = md.m_userInitLines                  ;
  m_annMemb                        = md.m_annMemb                        ;
  m_defArgList                     = md.m_defArgList                     ;
  m_declArgList                    = md.m_declArgList                    ;
  m_tArgList                       = md.m_tArgList                       ;
  m_typeConstraints                = md.m_typeConstraints                ;
  m_templateMaster                 = md.m_templateMaster                 ;
  m_defTmpArgLists                 = md.m_defTmpArgLists                 ;
  m_metaData                       = md.m_metaData                       ;
  m_cachedAnonymousType            = md.m_cachedAnonymousType            ;
  m_sectionMap                     = md.m_sectionMap                     ;
  m_groupAlias                     = md.m_groupAlias                     ;
  m_grpId                          = md.m_grpId                          ;
  m_memberGroup                    = md.m_memberGroup                    ;
  m_group                          = md.m_group                          ;
  m_grouppri                       = md.m_grouppri                       ;
  m_groupFileName                  = md.m_groupFileName                  ;
  m_groupStartLine                 = md.m_groupStartLine                 ;
  m_groupMember                    = md.m_groupMember                    ;
  m_isTypedefValCached             = md.m_isTypedefValCached             ;
  m_cachedTypedefValue             = md.m_cachedTypedefValue             ;
  m_cachedTypedefTemplSpec         = md.m_cachedTypedefTemplSpec         ;
  m_cachedResolvedType             = md.m_cachedResolvedType             ;
  m_docProvider                    = md.m_docProvider                    ;
  m_explicitOutputFileBase         = md.m_explicitOutputFileBase         ;
  m_qualifiers                     = md.m_qualifiers                     ;
  m_implOnly                       = md.m_implOnly                       ;
  m_hasDocumentedParams            = md.m_hasDocumentedParams            ;
  m_hasDocumentedReturnType        = md.m_hasDocumentedReturnType        ;
  m_isDMember                      = md.m_isDMember                      ;
  m_related                        = md.m_related                        ;
  m_stat                           = md.m_stat                           ;
  m_proto                          = md.m_proto                          ;
  m_docEnumValues                  = md.m_docEnumValues                  ;
  m_annScope                       = md.m_annScope                       ;
  m_hasDetailedDescriptionCached   = md.m_hasDetailedDescriptionCached   ;
  m_detailedDescriptionCachedValue = md.m_detailedDescriptionCachedValue ;
  m_hasCallGraph                   = md.m_hasCallGraph                   ;
  m_hasCallerGraph                 = md.m_hasCallerGraph                 ;
  m_hasReferencedByRelation        = md.m_hasReferencedByRelation        ;
  m_hasReferencesRelation          = md.m_hasReferencesRelation          ;
  m_hasInlineSource                = md.m_hasInlineSource                ;
  m_explExt                        = md.m_explExt                        ;
  m_tspec                          = md.m_tspec                          ;
  m_groupHasDocs                   = md.m_groupHasDocs                   ;
  m_docsForDefinition              = md.m_docsForDefinition              ;
  m_category                       = md.m_category                       ;
  m_categoryRelation               = md.m_categoryRelation               ;
  m_declFileName                   = md.m_declFileName                   ;
  m_declLine                       = md.m_declLine                       ;
  m_declColumn                     = md.m_declColumn                     ;
  m_numberOfFlowKW                 = md.m_numberOfFlowKW                 ;

  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

std::unique_ptr<MemberDef> MemberDefImpl::deepCopy() const
{
  //MemberDef *result = new MemberDef(getDefFileName(),getDefLine(),name());
  std::unique_ptr<MemberDefImpl> result(new MemberDefImpl(*this));
  // first copy everything by reference
  result->m_defArgList = m_defArgList;
  result->m_tArgList = m_tArgList;
  result->m_typeConstraints = m_typeConstraints;
  result->setDefinitionTemplateParameterLists(m_defTmpArgLists);
  result->m_declArgList = m_declArgList;
  return result;
}

void MemberDefImpl::moveTo(Definition *scope)
{
   setOuterScope(scope);
   if (scope->definitionType()==Definition::TypeClass)
   {
     m_classDef = toClassDef(scope);
   }
   else if (scope->definitionType()==Definition::TypeFile)
   {
     m_fileDef = toFileDef(scope);
   }
   else if (scope->definitionType()==Definition::TypeModule)
   {
     m_moduleDef = toModuleDef(scope);
   }
   else if (scope->definitionType()==Definition::TypeNamespace)
   {
     m_nspace = toNamespaceDef(scope);
   }
   m_isLinkableCached = 0;
   m_isConstructorCached = 0;
}


void MemberDefImpl::setReimplements(MemberDef *md)
{
  m_redefines = md;
}

void MemberDefImpl::insertReimplementedBy(MemberDef *md)
{
  if (m_templateMaster)
  {
    MemberDefMutable *mdm = toMemberDefMutable(m_templateMaster);
    if (mdm)
    {
      mdm->insertReimplementedBy(md);
    }
  }
  if (!m_redefinedBy.contains(md))
  {
    m_redefinedBy.inSort(md);
  }
}

const MemberDef *MemberDefImpl::reimplements() const
{
  return m_redefines;
}

const MemberVector &MemberDefImpl::reimplementedBy() const
{
  return m_redefinedBy;
}

bool MemberDefImpl::isReimplementedBy(const ClassDef *cd) const
{
  if (cd)
  {
    for (const auto &md : m_redefinedBy)
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
  m_enumFields.push_back(md);
}

bool MemberDefImpl::addExample(const QCString &anchor,const QCString &nameStr, const QCString &file)
{
  //printf("%s::addExample(%s,%s,%s)\n",qPrint(name()),anchor,nameStr,file);
  return m_examples.inSort(Example(anchor,nameStr,file));
}

bool MemberDefImpl::hasExamples() const
{
  return !m_examples.empty();
}

QCString MemberDefImpl::getOutputFileBase() const
{
  bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  bool predictableURLs = Config_getBool(PREDICTABLE_URLS);
  bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  QCString baseName;

  //printf("Member: %s: templateMaster=%p group=%p classDef=%p nspace=%p fileDef=%p\n",
  //    qPrint(name()),m_templateMaster,m_group,m_classDef,
  //    m_nspace,m_fileDef);
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  const ClassDef *classDef = getClassDef();
  const ModuleDef *moduleDef = getModuleDef();
  const GroupDef *groupDef = getGroupDef();
  if (!m_explicitOutputFileBase.isEmpty())
  {
    return m_explicitOutputFileBase;
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
  else if (nspace && (nspace->isLinkable() || nspace->isAnonymous()))
  {
    baseName=nspace->getOutputFileBase();
  }
  else if (fileDef)
  {
    baseName=fileDef->getOutputFileBase();
  }
  else if (moduleDef)
  {
    baseName=moduleDef->getOutputFileBase();
  }

  if (baseName.isEmpty())
  {
    warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: member %s does not belong to any"
       " container!",qPrint(name())
      );
    return "dummy";
  }
  else if ((separateMemberPages && hasDetailedDescription()) || predictableURLs)
  {
    // put only enums on separate page when PREDICTABLE_URLS is set
    if (getEnumScope()) // enum value, which is part of enum's documentation
    {
      baseName+="_"+getEnumScope()->anchor();
    }
    else if (separateMemberPages || isEnumerate())
    {
      baseName+="_"+anchor();
    }
  }
  return baseName;
}

QCString MemberDefImpl::getReference() const
{
  QCString ref = DefinitionMixin::getReference();
  if (!ref.isEmpty())
  {
    return ref;
  }
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  const ClassDef *classDef = getClassDef();
  const ModuleDef *moduleDef = getModuleDef();
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
  else if (moduleDef)
  {
    return moduleDef->getReference();
  }
  else if (fileDef)
  {
    return fileDef->getReference();
  }
  return "";
}

QCString MemberDefImpl::anchor() const
{
  QCString result=m_anc;
  if (m_groupAlias)     return m_groupAlias->anchor();
  if (m_templateMaster) return m_templateMaster->anchor();
  if (m_enumScope && m_enumScope!=this) // avoid recursion for C#'s public enum E { E, F }
  {
    result.prepend(m_enumScope->anchor());
  }
  if (getGroupDef())
  {
    if (m_groupMember)
    {
      result=m_groupMember->anchor();
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
  bool extractStatic  = Config_getBool(EXTRACT_STATIC);
  bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
  m_isLinkableCached = 2; // linkable
  //printf("MemberDefImpl::isLinkableInProject(name=%s)\n",qPrint(name()));
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
  if (isAnonymous())
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
  const ModuleDef *moduleDef = getModuleDef();
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
  if (!groupDef && moduleDef && !moduleDef->isLinkableInProject())
  {
    m_isLinkableCached = 1; // in module but module not linkable
    return;
  }
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  if (!groupDef && nspace && m_related==Relationship::Member && !nspace->isLinkableInProject()
      && (fileDef==0 || !fileDef->isLinkableInProject()))
  {
    //printf("in a namespace but namespace not linkable!\n");
    m_isLinkableCached = 1; // in namespace but namespace not linkable
    return;
  }
  if (!groupDef && !nspace &&
      m_related==Relationship::Member && !classDef &&
      fileDef && !fileDef->isLinkableInProject())
  {
    //printf("in a file but file not linkable!\n");
    m_isLinkableCached = 1; // in file (and not in namespace) but file not linkable
    return;
  }
  if ((!protectionLevelVisible(m_prot) && m_mtype!=MemberType_Friend) &&
       !(m_prot==Protection::Private && (m_virt!=Specifier::Normal || isOverride() || isFinal()) && extractPrivateVirtual))
  {
    //printf("private and invisible!\n");
    m_isLinkableCached = 1; // hidden due to protection
    return;
  }
  if (m_stat && classDef==0 && !extractStatic)
  {
    //printf("static and invisible!\n");
    m_isLinkableCached = 1; // hidden due to staticness
    return;
  }
  //printf("linkable!\n");
  return; // linkable!
}

void MemberDefImpl::setDocumentation(const QCString &d,const QCString &docFile,int docLine,bool stripWhiteSpace)
{
  DefinitionMixin::setDocumentation(d,docFile,docLine,stripWhiteSpace);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setBriefDescription(const QCString &b,const QCString &briefFile,int briefLine)
{
  DefinitionMixin::setBriefDescription(b,briefFile,briefLine);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setInbodyDocumentation(const QCString &d,const QCString &inbodyFile,int inbodyLine)
{
  DefinitionMixin::setInbodyDocumentation(d,inbodyFile,inbodyLine);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setHidden(bool b)
{
  DefinitionMixin::setHidden(b);
  m_isLinkableCached = 0;
}

bool MemberDefImpl::isLinkableInProject() const
{
  if (m_isLinkableCached==0)
  {
    MemberDefImpl *that = const_cast<MemberDefImpl*>(this);
    that->_computeLinkableInProject();
  }
  ASSERT(m_isLinkableCached>0);
  return m_isLinkableCached==2;
}

bool MemberDefImpl::isLinkable() const
{
  if (m_templateMaster)
  {
    return m_templateMaster->isLinkable();
  }
  else
  {
    return isLinkableInProject() || isReference();
  }
}


void MemberDefImpl::setDefinitionTemplateParameterLists(const ArgumentLists &lists)
{
  m_defTmpArgLists = lists;
}

void MemberDefImpl::writeLink(OutputList &ol,
                      const ClassDef *,const NamespaceDef *,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                      bool onlyText) const
{
  SrcLangExt lang = getLanguage();
  bool hideScopeNames     = Config_getBool(HIDE_SCOPE_NAMES);
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  QCString n = name();
  const ClassDef *classDef = getClassDef();
  const ModuleDef *moduleDef = getModuleDef();
  const NamespaceDef *nspace = getNamespaceDef();
  if (!hideScopeNames)
  {
    if (m_enumScope && m_livesInsideEnum && getGroupDef()!=0)
    {
      n.prepend(m_enumScope->displayName()+sep);
    }
    if (classDef && gd && !isRelated())
    {
      n.prepend(classDef->displayName()+sep);
    }
    else if (nspace && (gd || fd || moduleDef))
    {
      n.prepend(nspace->displayName()+sep);
    }
  }

  if (isObjCMethod())
  {
    if (isStatic()) ol.docify("+ "); else ol.docify("- ");
  }
  if (!onlyText && (isLinkable() || hasDetailedDescription())) // write link
  {
    if (m_mtype==MemberType_EnumValue && getGroupDef()==0 &&          // enum value is not grouped
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

static std::mutex g_cachedAnonymousTypeMutex;

/*! If this member has an anonymous class/struct/union as its type, then
 *  this method will return the ClassDef that describes this return type.
 */
ClassDef *MemberDefImpl::getClassDefOfAnonymousType() const
{
  std::lock_guard<std::mutex> lock(g_cachedAnonymousTypeMutex);
  //printf("%s:getClassDefOfAnonymousType() cache=%s\n",qPrint(name()),
  //                   m_cachedAnonymousType?qPrint(m_cachedAnonymousType->name()):"<empty>");
  if (m_cachedAnonymousType) return m_cachedAnonymousType;

  QCString cname;
  if (getClassDef()!=0)
  {
    cname=getClassDef()->name();
  }
  else if (getNamespaceDef()!=0)
  {
    cname=getNamespaceDef()->name();
  }
  QCString ltype(m_type);
  // strip 'friend' keyword from ltype
  ltype.stripPrefix("friend ");

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=0;

  // match expression if it contains at least one @1 marker, e.g.
  // 'struct A::@1::@2::B' matches 'A::@1::@2::B' but 'struct A::B' does not match.
  std::string stype = ltype.str();
  static const reg::Ex r(R"([\w@:]*@\d+[\w@:]*)");
  reg::Match match;
  if (reg::search(stype,match,r)) // found anonymous scope in type
  {
    QCString annName = match.str();

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
  m_cachedAnonymousType = annoClassDef;
  return annoClassDef;
}

/*! This methods returns TRUE iff the brief section (also known as
 *  declaration section) is visible in the documentation.
 */
bool MemberDefImpl::isBriefSectionVisible() const
{
  bool extractStatic       = Config_getBool(EXTRACT_STATIC);
  bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
  bool hideUndocMembers    = Config_getBool(HIDE_UNDOC_MEMBERS);
  bool briefMemberDesc     = Config_getBool(BRIEF_MEMBER_DESC);
  bool repeatBrief         = Config_getBool(REPEAT_BRIEF);
  bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);

  //printf("Member %s grpId=%d docs=%s file=%s args=%s\n",
  //    qPrint(name()),
  //    0,"", //grpId,grpId==-1?"<none>":Doxygen::memberDocDict[grpId]->data(),
  //    "", //qPrint(getFileDef()->name()),
  //    argsString());

  auto it = Doxygen::memberGroupInfoMap.find(m_grpId);
  bool hasDocs = hasDocumentation();
  if (it!=Doxygen::memberGroupInfoMap.end())
  {
    auto &info = it->second;
    //printf("name=%s m_grpId=%d info=%p\n",qPrint(name()),m_grpId,info);
    //QCString *pMemGrp = Doxygen::memberDocDict[grpId];
    hasDocs = hasDocs &&
                  // part of a documented member group
                 (m_grpId!=-1 && !(info->doc.isEmpty() && info->header.isEmpty()));
  }

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
                                   isFriend() /*&&
                                   (m_type=="friend class" ||
                                    m_type=="friend struct" ||
                                    m_type=="friend union"
                                   )*/
                                  );

  // only include members that are non-private unless EXTRACT_PRIVATE is
  // set to YES or the member is part of a group. And as a special case,
  // private *documented* virtual members are shown if EXTRACT_PRIV_VIRTUAL
  // is set to YES
  bool visibleIfPrivate = (protectionLevelVisible(protection()) ||
                           m_mtype==MemberType_Friend ||
                           (m_prot==Protection::Private &&
                               (m_virt!=Specifier::Normal || isOverride() || isFinal()) && extractPrivateVirtual && hasDocs)
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
                                   (m_defArgList.empty() ||
                                    m_defArgList.front().type == "void"
                                   ) &&
                                   !hasDocs
                                  );


  //printf("visibleIfStatic=%d visibleIfDocumented=%d visibleIfEnabled=%d "
  //       "visibleIfPrivate=%d visibleIfNotDefaultCDTor=%d "
  //       "visibleIfFriendCompound=%d !annScope=%d\n",
  //       visibleIfStatic,visibleIfDocumented,
  //       visibleIfEnabled,visibleIfPrivate,visibleIfNotDefaultCDTor,
  //       visibleIfFriendCompound,!m_annScope);

  bool visible = visibleIfStatic     && visibleIfDocumented      &&
                 visibleIfEnabled    && visibleIfPrivate         &&
                 /*visibleIfDocVirtual &&*/ visibleIfNotDefaultCDTor &&
                 visibleIfFriendCompound &&
                 !m_annScope && !isHidden();
  //printf("MemberDefImpl::isBriefSectionVisible() %d\n",visible);
  return visible;
}

QCString MemberDefImpl::getDeclType() const
{
  SrcLangExt lang = getLanguage();
  QCString ltype(m_type);
  if (lang==SrcLangExt_Cpp && isEnumerate() && isStrong())
  {
    if (isEnumStruct())
    {
      ltype+=" struct";
    }
    else
    {
      ltype+=" class";
    }
  }
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

void MemberDefImpl::_writeTemplatePrefix(OutputList &ol, const Definition *def,
                                         const ArgumentList &al, bool writeReqClause) const
{
  ol.docify("template<");
  for (auto it = al.begin(); it!=al.end();)
  {
    Argument a = *it;
    linkifyText(TextGeneratorOLImpl(ol), // out
        def,                     // scope
        getFileDef(),            // fileScope
        this,                    // self
        a.type,                  // text
        FALSE                    // autoBreak
        );
    ol.docify(" ");
    ol.docify(a.name);
    if (a.defval.length()!=0)
    {
      ol.docify(" = ");
      ol.docify(a.defval);
    }
    ++it;
    if (it!=al.end()) ol.docify(", ");
  }
  ol.docify("> ");
  if (writeReqClause && !m_requiresClause.isEmpty())
  {
    ol.lineBreak();
    ol.docify("requires ");
    linkifyText(TextGeneratorOLImpl(ol), // out
        def,                     // scope
        getFileDef(),            // fileScope
        this,                    // self
        m_requiresClause,  // text
        FALSE                    // autoBreak
        );
  }
}

static QCString combineArgsAndException(QCString args,QCString exception)
{
  if (exception.isEmpty()) return args;                      // no exception, nothing to combine args
  int pos   = args.findRev(')');
  int eqPos = pos!=-1 ? args.find('=',pos) : -1;             // look for '=' in '(args) = something'
  if (eqPos==-1) return args+" "+exception;                  // append exception at the end
  return args.left(eqPos)+" "+exception+" "+args.mid(eqPos); // insert exception before =
}

bool MemberDefImpl::_isAnonymousBitField() const
{
  return !m_bitfields.isEmpty() && name().startsWith("__pad"); // anonymous bitfield
}

void MemberDefImpl::writeDeclaration(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
               bool inGroup, int indentLevel, const ClassDef *inheritedFrom,const QCString &inheritId) const
{
  //printf("> %s MemberDefImpl::writeDeclaration() inGroup=%d\n",qPrint(qualifiedName()),inGroup);

  // hide enum value, since they appear already as part of the enum, unless they
  // are explicitly grouped.
  if (!inGroup && m_mtype==MemberType_EnumValue) return;

  const Definition *d=0;
  ASSERT (cd!=0 || nd!=0 || fd!=0 || gd!=0 || mod!=0); // member should belong to something
  if (cd) d=cd;
  else if (nd) d=nd;
  else if (mod) d=mod;
  else if (fd) d=fd;
  else d=gd;
  if (d==gd) // see bug 753608
  {
    if (getClassDef())          d = getClassDef();
    else if (getNamespaceDef()) d = getNamespaceDef();
    else if (getFileDef())      d = getFileDef();
  }
  else if (d==fd) // see issue #9850, namespace member can be shown in file scope as well
  {
    if (getNamespaceDef()) d = getNamespaceDef();
  }
  if (d==0)
  {
    err("No context could be derived for member '%s'\n",qPrint(name()));
    return; // should not happen
  }

  QCString cname  = d->name();
  QCString cdname = d->displayName();
  QCString cfname = getOutputFileBase();

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=getClassDefOfAnonymousType();

  ol.startMemberDeclaration();

  // start a new member declaration
  bool isAnonType = annoClassDef || m_annMemb || m_annEnumType;
  OutputGenerator::MemberItemType anonType = isAnonType ? OutputGenerator::MemberItemType::AnonymousStart :
                              !m_tArgList.empty() ? OutputGenerator::MemberItemType::Templated      :
                                                          OutputGenerator::MemberItemType::Normal;
  ol.startMemberItem(anchor(), anonType, inheritId);

  // If there is no detailed description we need to write the anchor here.
  bool detailsVisible = hasDetailedDescription();
  bool writeAnchor = (inGroup || getGroupDef()==0) &&     // only write anchors for member that have no details and are
                     !detailsVisible && !m_annMemb && // rendered inside the group page or are not grouped at all
                     inheritId.isEmpty();
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
    ol.disable(OutputType::Man);
    ol.disable(OutputType::Latex);
    ol.disable(OutputType::Docbook);
    ol.docify("\n");
    ol.popGeneratorState();
  }

  if (annoClassDef || m_annMemb)
  {
    int j;
    for (j=0;j<indentLevel;j++)
    {
      ol.writeNonBreakableSpace(3);
    }
  }

  // *** write template lists
  if (m_tArgList.hasParameters() && getLanguage()==SrcLangExt_Cpp)
  {
    if (!isAnonType) ol.startMemberTemplateParams();
    _writeTemplatePrefix(ol,d,m_tArgList);
    if (!isAnonType) ol.endMemberTemplateParams(anchor(),inheritId);
  }


  // *** write type
  QCString ltype(m_type);
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
  static const reg::Ex r(R"(@\d+)");
  reg::Match match;
  std::string stype = ltype.str();
  bool endAnonScopeNeeded=FALSE;
  if (reg::search(stype,match,r)) // member has an anonymous type
  {
    int i = static_cast<int>(match.position());
    int l = static_cast<int>(match.length());
    //printf("annoClassDef=%p annMemb=%p scopeName='%s' anonymous='%s'\n",
    //    annoClassDef,annMemb,qPrint(cname),qPrint(ltype.mid(i,l)));

    if (annoClassDef) // type is an anonymous compound
    {
      int ir=i+l;
      //printf("<<<<<<<<<<<<<<\n");
      ol.startAnonTypeScope(indentLevel);
      annoClassDef->writeDeclaration(ol,m_annMemb,inGroup,indentLevel+1,inheritedFrom,inheritId);
      //printf(">>>>>>>>>>>>>> startMemberItem(2)\n");
      anonType = OutputGenerator::MemberItemType::AnonymousEnd;
      ol.startMemberItem(anchor(),anonType,inheritId);
      int j;
      for (j=0;j< indentLevel;j++)
      {
        ol.writeNonBreakableSpace(3);
      }
      QCString varName=ltype.right(ltype.length()-ir).stripWhiteSpace();
      //printf(">>>>>> ltype='%s' varName='%s'\n",qPrint(ltype),qPrint(varName));
      ol.docify("}");
      if (varName.isEmpty() && isAnonymous())
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
        getAnonymousEnumType()->writeEnumDeclaration(ol,cd,nd,fd,gd,mod);
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
  bool htmlOn = ol.isEnabled(OutputType::Html);
  if (htmlOn && !ltype.isEmpty())
  {
    ol.disable(OutputType::Html);
  }
  if (!ltype.isEmpty()) ol.docify(" ");
  if (htmlOn)
  {
    ol.enable(OutputType::Html);
  }

  if (m_annMemb)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.writeNonBreakableSpace(3);
    ol.popGeneratorState();
  }
  else
  {
    ol.insertMemberAlign(m_tArgList.hasParameters());
  }

  // *** write name
  if (!isAnonymous() && !_isAnonymousBitField()) // hide anonymous stuff
  {
    bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
    bool extractStatic  = Config_getBool(EXTRACT_STATIC);
    MemberDefMutable *annMemb = toMemberDefMutable(m_annMemb);
    bool visibleIfPrivate = (protectionLevelVisible(protection()) ||
                             m_mtype==MemberType_Friend ||
                             (m_prot==Protection::Private &&
                                (m_virt!=Specifier::Normal || isOverride() || isFinal()) &&
                                extractPrivateVirtual && hasDocumentation()
                             ));
    //printf("Member name=`%s gd=%p md->groupDef=%p inGroup=%d isLinkable()=%d hasDocumentation=%d\n",qPrint(name()),gd,getGroupDef(),inGroup,isLinkable(),hasDocumentation());
    if (!name().isEmpty() && // name valid
        (hasDetailedDescription() || isReference()) && // has docs
        visibleIfPrivate &&
        !(isStatic() && getClassDef()==0 && !extractStatic) // hidden due to static-ness
       )
    {
      if (annMemb)
      {
        //printf("anchor=%s ann_anchor=%s\n",anchor(),annMemb->anchor());
        annMemb->writeLink(ol,
            annMemb->getClassDef(),
            annMemb->getNamespaceDef(),
            annMemb->getFileDef(),
            annMemb->getGroupDef(),
            annMemb->getModuleDef());
      }
      else
      {
        //printf("writeLink %s->%d\n",qPrint(name),hasDocumentation());
        const ClassDef *rcd = cd;
        if (isReference() && getClassDef()) rcd = getClassDef();
        writeLink(ol,rcd,nd,fd,gd,mod);
      }
    }
    else if (isDocumentedFriendClass())
      // if the member is an undocumented friend declaration for some class,
      // then maybe we can link to the class
    {
      writeLink(ol,getClass(name()),0,0,0,0);
    }
    else
      // there is a brief member description and brief member
      // descriptions are enabled or there is no detailed description.
    {
      const ClassDef *rcd = cd;
      if (isReference() && getClassDef()) rcd = getClassDef();
      writeLink(ol,rcd,nd,fd,gd,mod,TRUE);
    }
  }

  // *** write arguments
  if (!argsString().isEmpty() && !isObjCMethod())
  {
    if (!isDefine() && !isTypedef()) ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol), // out
                d,                       // scope
                getBodyDef(),            // fileScope
                this,                    // self
                isDefine() ?
                   substitute(argsString(),",",", ") :
                isTypedef() ?
                   substitute(argsString(),")(",") (") :
                   combineArgsAndException(argsString(),excpString()), // text
                m_annMemb!=0,      // autoBreak
                TRUE,                    // external
                FALSE,                   // keepSpaces
                indentLevel
               );
  }

  // *** write bitfields
  if (!m_bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_bitfields);
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
      linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_initializer.simplifyWhiteSpace());
    }
    else
    {
      ol.writeNonBreakableSpace(3);
      linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_initializer);
    }
  }
  else if (isTypeAlias()) // using template alias
  {
    ol.writeString(" = ");
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_type);
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
      StringVector sl;

      if (isGettable())             sl.push_back("get");
      if (isProtectedGettable())    sl.push_back("protected get");
      if (isSettable())             sl.push_back("set");
      if (isProtectedSettable())    sl.push_back("protected set");
      if (extractPrivate)
      {
        if (isPrivateGettable())    sl.push_back("private get");
        if (isPrivateSettable())    sl.push_back("private set");
      }
      bool first=true;
      for (const auto &s : sl)
      {
         if (!first)
         {
           ol.docify(", ");
         }
         first=false;
         ol.docify(s.c_str());
      }
      ol.docify("]");
      ol.endTypewriter();
  }

  if (isEvent() && (isAddable() || isRemovable() || isRaisable()))
  {
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      StringVector sl;
      if (isAddable())   sl.push_back("add");
      if (isRemovable()) sl.push_back("remove");
      if (isRaisable())  sl.push_back("raise");
      bool first=true;
      for (const auto &s : sl)
      {
        if (!first)
        {
          ol.docify(", ");
        }
        first=false;
        ol.docify(s.c_str());
      }
      ol.docify("]");
      ol.endTypewriter();
  }

  if (writeAnchor)
  {
    ol.endDoxyAnchor(cfname,anchor());
  }

  //printf("endMember %s annoClassDef=%p annEnumType=%p\n",
  //    qPrint(name()),annoClassDef,annEnumType);
  ol.endMemberItem(anonType);
  if (endAnonScopeNeeded)
  {
    ol.endAnonTypeScope(indentLevel);
  }

  // write brief description
  if (!briefDescription().isEmpty() &&
      Config_getBool(BRIEF_MEMBER_DESC)
     )
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     briefFile(),briefLine(),
                                     getOuterScope()?getOuterScope():d,
                                     this,briefDescription(),TRUE,FALSE,
                                     QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    if (!ast->isEmpty())
    {
      ol.startMemberDescription(anchor(),inheritId);
      ol.writeDoc(ast.get(),getOuterScope()?getOuterScope():d,this);
      if (detailsVisible) // add More.. link only when both brief and details are visible
      {
        if (!isAnonymous()) // hide anonymous stuff
        {
          ol.pushGeneratorState();
          ol.disableAllBut(OutputType::Html);
          ol.docify(" ");
          MemberDefMutable *annMemb = toMemberDefMutable(m_annMemb);
          if (annMemb)
          {
            ol.startTextLink(annMemb->getOutputFileBase(),annMemb->anchor());
            ol.parseText(theTranslator->trMore());
            ol.endTextLink();
          }
          ol.popGeneratorState();
        }
      }
      // for RTF we need to add an extra empty paragraph
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::RTF);
      ol.startParagraph();
      ol.endParagraph();
      ol.popGeneratorState();
      ol.endMemberDescription();
    }
  }

  ol.endMemberDeclaration(anchor(),inheritId);

  warnIfUndocumented();
  //printf("< %s MemberDefImpl::writeDeclaration() inGroup=%d\n",qPrint(qualifiedName()),inGroup);
}

static std::mutex g_hasDetailedDescriptionMutex;

bool MemberDefImpl::hasDetailedDescription() const
{
  std::lock_guard<std::mutex> lock(g_hasDetailedDescriptionMutex);
  //printf(">hasDetailedDescription(cached=%d)\n",m_hasDetailedDescriptionCached);
  if (!m_hasDetailedDescriptionCached)
  {
    bool extractAll            = Config_getBool(EXTRACT_ALL);
    bool alwaysDetailedSec     = Config_getBool(ALWAYS_DETAILED_SEC);
    bool repeatBrief           = Config_getBool(REPEAT_BRIEF);
    bool briefMemberDesc       = Config_getBool(BRIEF_MEMBER_DESC);
    bool hideUndocMembers      = Config_getBool(HIDE_UNDOC_MEMBERS);
    bool extractStatic         = Config_getBool(EXTRACT_STATIC);
    bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
    bool inlineSources         = hasInlineSource();

    // the member has detailed documentation because the user added some comments
    bool docFilter =
           // extract all is enabled
           extractAll ||
           // has detailed docs
           !documentation().isEmpty() ||
           // has inbody docs
           !inbodyDocumentation().isEmpty() ||
           // is an enum with values that are documented
           (isEnumerate() && hasDocumentedEnumValues()) ||
           // is documented enum value
           (m_mtype==MemberType_EnumValue && !briefDescription().isEmpty()) ||
           // has brief description that is part of the detailed description
           (!briefDescription().isEmpty() &&           // has brief docs
            (alwaysDetailedSec &&                      // they are visible in
             (repeatBrief ||                           // detailed section or
              !briefMemberDesc                         // they are explicitly not
             )                                         // shown in brief section
            )
           ) ||
           // has one or more documented arguments
           (m_templateMaster ?
            m_templateMaster->argumentList().hasDocumentation() :
            m_defArgList.hasDocumentation());

    // generate function                  guard
    // ==================                 =======
    // _writeGroupInclude              -> ignored in calculation
    // multiLineInitializer()          -> hasMultiLineInitializer()
    // _writeReimplements              -> _isReimplements()
    // _writeReimplementedBy           -> _countReimplementedBy()>0
    // _writeExamples                  -> hasExamples()
    // _writeTypeConstraints           -> m_typeConstraints.hasParameters()
    // writeSourceDef                  -> !getSourceFileBase().isEmpty();
    // writeInlineCode                 -> hasInlineSource() && hasSources()
    // writeSourceRefs                 -> hasReferencesRelation() && hasSourceRefs()
    // writeSourceReffedBy             -> hasReferencedByRelation() && hasSourceReffedBy()
    // _writeCallGraph                 -> _hasVisibleCallGraph()
    // _writeCallerGraph               -> _hasVisibleCallerGraph()

    // the member has detailed documentation because there is some generated info
    bool docInfo =
           // has a multi-line initialization block
           (hasMultiLineInitializer()) ||
           // reimplements / reimplemented by
           _isReimplements() || (_countReimplementedBy() > 0) ||
           // examples
           hasExamples() ||
           // type constraints
           m_typeConstraints.hasParameters() ||
           // has source definition
           !getSourceFileBase().isEmpty() ||
           // has inline sources
           (inlineSources && hasSources()) ||
           // has references
           (hasReferencesRelation() && hasSourceRefs()) ||
           (hasReferencedByRelation() && hasSourceReffedBy()) ||
           // call graph
           _hasVisibleCallGraph() ||
           // caller graph
           _hasVisibleCallerGraph();

    if (!hideUndocMembers) // if HIDE_UNDOC_MEMBERS is NO we also show the detailed section
                           // if there is only some generated info
    {
      docFilter = docFilter || docInfo;
    }

    // this is not a global static or global statics should be extracted
    bool staticFilter = getClassDef()!=0 || !isStatic() || extractStatic;

    // only include members that are non-private unless EXTRACT_PRIVATE is
    // set to YES or the member is part of a   group
    bool privateFilter = protectionLevelVisible(protection()) || m_mtype==MemberType_Friend ||
                         (m_prot==Protection::Private && (m_virt!=Specifier::Normal || isOverride() || isFinal()) && extractPrivateVirtual);

    // hide friend (class|struct|union) member if HIDE_FRIEND_COMPOUNDS
    // is true
    bool friendCompoundFilter = !(Config_getBool(HIDE_FRIEND_COMPOUNDS) && isFriend());

    m_detailedDescriptionCachedValue =
        (docFilter && staticFilter && privateFilter && friendCompoundFilter && !isHidden());
    //printf("docFilter=%d docInfo=%d staticFilter=%d privateFilter=%d friendCompoundFilter=%d !isHidden()=%d",
    //    docFilter,docInfo,staticFilter,privateFilter,friendCompoundFilter,!isHidden());
    m_hasDetailedDescriptionCached = true;
  }
  //printf("<hasDetailedDescription(cached=%d)\n",m_hasDetailedDescriptionCached);
  return m_detailedDescriptionCachedValue;
}

bool MemberDefImpl::isDetailedSectionVisible(MemberListContainer container) const
{
  bool separateMemPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  bool inlineSimpleStructs = Config_getBool(INLINE_SIMPLE_STRUCTS);
  bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
  bool groupFilter = getGroupDef()==0 || container==MemberListContainer::Group || separateMemPages;
  bool fileFilter  = getNamespaceDef()==0 || !getNamespaceDef()->isLinkable() || container!=MemberListContainer::File;
  bool simpleFilter = (hasBriefDescription() || !hideUndocMembers) && inlineSimpleStructs &&
                      getClassDef()!=0 && getClassDef()->isSimple();

  bool visible = hasDetailedDescription() && groupFilter && fileFilter &&
                 !isReference();
  bool result = visible || simpleFilter;
  //printf("%s::isDetailedSectionVisible: %d groupFilter=%d fileFilter=%d\n",
  //    qPrint(name()),result,groupFilter,fileFilter);
  return result;
}

StringVector MemberDefImpl::getLabels(const Definition *container) const
{
  StringVector sl;
  bool inlineInfo = Config_getBool(INLINE_INFO);

  Specifier lvirt=virtualness();
  if ((!isObjCMethod() || isOptional() || isRequired()) &&
      (protection()!=Protection::Public || lvirt!=Specifier::Normal ||
       isFriend() || isRelated() ||
       (isInline() && inlineInfo) ||
       isSignal() || isSlot() ||
       isStatic() || isExternal() ||
       isExported() ||
       (getClassDef() && getClassDef()!=container && container->definitionType()==TypeClass) ||
       TypeSpecifier(m_memSpec).setInline(false)!=TypeSpecifier()
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
      sl.push_back(theTranslator->trVhdlType(getVhdlSpecifiers(),TRUE).str());
    }
    else
    {
      if (isFriend())                                   sl.push_back("friend");
      else if (isRelated())                             sl.push_back("related");
      else
      {
        if      (isExternal())                          sl.push_back("extern");
        if      (inlineInfo && isInline())              sl.push_back("inline");
        if      (isExplicit())                          sl.push_back("explicit");
        if      (isMutable())                           sl.push_back("mutable");
        if      (isStatic())                            sl.push_back("static");
        if      (isGettable())                          sl.push_back("get");
        if      (isProtectedGettable())                 sl.push_back("protected get");
        if      (isSettable())                          sl.push_back("set");
        if      (isProtectedSettable())                 sl.push_back("protected set");
        if (extractPrivate)
        {
          if    (isPrivateGettable())                   sl.push_back("private get");
          if    (isPrivateSettable())                   sl.push_back("private set");
        }
        if      (isConstExpr())                         sl.push_back("constexpr");
        if      (isAddable())                           sl.push_back("add");
        if      (!isUNOProperty() && isRemovable())     sl.push_back("remove");
        if      (isRaisable())                          sl.push_back("raise");
        if      (isReadable())                          sl.push_back("read");
        if      (isWritable())                          sl.push_back("write");
        if      (isFinal())                             sl.push_back("final");
        if      (isAbstract())                          sl.push_back("abstract");
        if      (isOverride())                          sl.push_back("override");
        if      (isInitonly())                          sl.push_back("initonly");
        if      (isSealed())                            sl.push_back("sealed");
        if      (isNew())                               sl.push_back("new");
        if      (isOptional())                          sl.push_back("optional");
        if      (isRequired())                          sl.push_back("required");
        if      (isExported())                          sl.push_back("export");

        if      (isNonAtomic())                         sl.push_back("nonatomic");
        else if (isObjCProperty())                      sl.push_back("atomic");

        // mutual exclusive Objective 2.0 property attributes
        if      (isAssign())                            sl.push_back("assign");
        else if (isCopy())                              sl.push_back("copy");
        else if (isRetain())                            sl.push_back("retain");
        else if (isWeak())                              sl.push_back("weak");
        else if (lang!=SrcLangExt_CSharp && isStrong()) sl.push_back("strong");
        else if (isUnretained())                        sl.push_back("unsafe_unretained");

        if (!isObjCMethod())
        {
          if      (protection()==Protection::Protected) sl.push_back("protected");
          else if (protection()==Protection::Private)   sl.push_back("private");
          else if (protection()==Protection::Package)   sl.push_back("package");

          if      (lvirt==Specifier::Virtual)           sl.push_back("virtual");
          else if (lvirt==Specifier::Pure)              sl.push_back("pure virtual");
          if      (isSignal())                          sl.push_back("signal");
          if      (isSlot())                            sl.push_back("slot");
          if      (isDefault())                         sl.push_back("default");
          if      (isDelete())                          sl.push_back("delete");
          if      (isNoExcept())                        sl.push_back("noexcept");
          if      (isAttribute())                       sl.push_back("attribute");
          if      (isUNOProperty())                     sl.push_back("property");
          if      (isReadonly())                        sl.push_back("readonly");
          if      (isBound())                           sl.push_back("bound");
          if      (isUNOProperty() && isRemovable())    sl.push_back("removable");
          if      (isConstrained())                     sl.push_back("constrained");
          if      (isTransient())                       sl.push_back("transient");
          if      (isMaybeVoid())                       sl.push_back("maybevoid");
          if      (isMaybeDefault())                    sl.push_back("maybedefault");
          if      (isMaybeAmbiguous())                  sl.push_back("maybeambiguous");
          if      (isPublished())                       sl.push_back("published"); // enum
        }
        if (isObjCProperty() && isImplementation())
        {
          sl.push_back("implementation");
        }
      }
      if (getClassDef() &&
          container->definitionType()==TypeClass &&
          getClassDef()!=container &&
          !isRelated()
         )
      {
        sl.push_back("inherited");
      }
    }
  }
  else if (isObjCMethod() && isImplementation())
  {
    sl.push_back("implementation");
  }

  for (const auto &sx : m_qualifiers)
  {
    bool alreadyAdded = std::find(sl.begin(), sl.end(), sx) != sl.end();
    if (!alreadyAdded)
    {
      sl.push_back(sx);
    }
  }

  return sl;
}

void MemberDefImpl::_writeCallGraph(OutputList &ol) const
{
  // write call graph
  if (_hasVisibleCallGraph())
  {
    DotCallGraph callGraph(this,FALSE);
    if (callGraph.isTooBig())
    {
       warn_uncond("Call graph for '%s' not generated, too many nodes (%d), threshold is %d. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qPrint(qualifiedName()), callGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!callGraph.isTrivial())
    {
      msg("Generating call graph for function %s\n",qPrint(qualifiedName()));
      ol.disable(OutputType::Man);
      ol.startCallGraph();
      ol.parseText(theTranslator->trCallGraph());
      ol.endCallGraph(callGraph);
      ol.enableAll();
    }
  }
}

void MemberDefImpl::_writeCallerGraph(OutputList &ol) const
{
  if (_hasVisibleCallerGraph())
  {
    DotCallGraph callerGraph(this, TRUE);
    if (callerGraph.isTooBig())
    {
       warn_uncond("Caller graph for '%s' not generated, too many nodes (%d), threshold is %d. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qPrint(qualifiedName()), callerGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!callerGraph.isTrivial())
    {
      msg("Generating caller graph for function %s\n",qPrint(qualifiedName()));
      ol.disable(OutputType::Man);
      ol.startCallGraph();
      ol.parseText(theTranslator->trCallerGraph());
      ol.endCallGraph(callerGraph);
      ol.enableAll();
    }
  }
}

bool MemberDefImpl::_isReimplements() const
{
  const MemberDef *bmd=reimplements();
  const ClassDef *bcd=0;
  if (bmd && (bcd=bmd->getClassDef()))
  {
    // write class that contains a member that is reimplemented by this one
    if (bcd->isLinkable())
    {
      return true;
    }
  }
  return false;
}
void MemberDefImpl::_writeReimplements(OutputList &ol) const
{
  const MemberDef *bmd=reimplements();
  const ClassDef *bcd=0;
  if (bmd && (bcd=bmd->getClassDef()))
  {
    // write class that contains a member that is reimplemented by this one
    if (bcd->isLinkable())
    {
      ol.startParagraph();
      QCString reimplFromLine;
      if (bmd->virtualness()!=Specifier::Pure && bcd->compoundType()!=ClassDef::Interface)
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
              QCString(),bcd->displayName());
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

size_t MemberDefImpl::_countReimplementedBy() const
{
  const MemberVector &bml=reimplementedBy();
  size_t count=0;
  for (const auto &bmd : bml)
  {
    const ClassDef *bcd=bmd->getClassDef();
    // count the members that directly inherit from md and for
    // which the member and class are visible in the docs.
    if ( bcd && bmd->isLinkable() && bcd->isLinkable() )
    {
      count++;
    }
  }
  return count;
}

void MemberDefImpl::_writeReimplementedBy(OutputList &ol) const
{
  const MemberVector &bml=reimplementedBy();
  size_t count = _countReimplementedBy();
  if (count>0)
  {
    auto replaceFunc = [&bml,&ol](size_t entryIndex)
    {
      size_t cnt=0;
      auto it = bml.begin();
      // find the entryIndex-th documented entry in the inheritance list.
      const MemberDef *bmd = 0;
      const ClassDef *bcd = 0;
      while (it!=bml.end())
      {
        bmd = *it;
        bcd = bmd->getClassDef();
        if ( bmd->isLinkable() && bcd->isLinkable())
        {
          if (cnt==entryIndex) break;
          cnt++;
        }
        ++it;
      }

      if (bcd && bmd) // write link for marker
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
    };

    QCString reimplInLine;
    if (m_virt==Specifier::Pure || (getClassDef() && getClassDef()->compoundType()==ClassDef::Interface))
    {
      reimplInLine = theTranslator->trImplementedInList(static_cast<int>(count));
    }
    else
    {
      reimplInLine = theTranslator->trReimplementedInList(static_cast<int>(count));
    }

    // write the list of classes that overwrite this member
    ol.startParagraph();
    writeMarkerList(ol,
                    reimplInLine.str(),
                    count,
                    replaceFunc);
    ol.endParagraph();
  }
}

void MemberDefImpl::_writeCategoryRelation(OutputList &ol) const
{
  if (getClassDef()) // this should be a member of a class/category
  {
    //printf("%s: category %s relation %s class=%s categoryOf=%s\n",
    //    qPrint(name()),
    //    m_category ? qPrint(m_category->name()) : "<none>",
    //    m_categoryRelation ? qPrint(m_categoryRelation->name()) : "<none>",
    //    qPrint(m_classDef->name()),
    //    m_classDef->categoryOf() ? qPrint(m_classDef->categoryOf()->name()) : "<none>"
    //    );
    QCString text;
    QCString ref;
    QCString file;
    QCString anc;
    QCString name;
    int i=-1;
    if (m_categoryRelation && m_categoryRelation->isLinkable())
    {
      if (m_category)
      {
        // this member is in a normal class and implements method categoryRelation from category
        // so link to method 'categoryRelation' with 'provided by category 'category' text.
        text = theTranslator->trProvidedByCategory();
        name = m_category->displayName();
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
        const MemberDef *md = m_categoryRelation;
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
    writeExamples(ol,m_examples);
    ol.endDescForItem();
    ol.endExamples();
  }
}

void MemberDefImpl::_writeTypeConstraints(OutputList &ol) const
{
  if (m_typeConstraints.hasParameters())
  {
    writeTypeConstraints(ol,this,m_typeConstraints);
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
    //printf("** %s: enum values=%zu\n",qPrint(name()),enumFieldList().size());
    for (const auto &fmd : enumFieldList())
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

        ol.startDescTableTitle();
        ol.startDoxyAnchor(cfname,cname,fmd->anchor(),fmd->name(),fmd->argsString());
        first=FALSE;
        ol.docify(fmd->name());
        ol.disableAllBut(OutputType::Man);
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
              fmd,fmd->briefDescription(),TRUE,FALSE,
              QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
        }
        if (hasDetails)
        {
          ol.generateDoc(fmd->docFile(),fmd->docLine(),
              getOuterScope()?getOuterScope():container,
              fmd,fmd->documentation()+"\n",TRUE,FALSE,
              QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
        }
        ol.endDescTableData();
        ol.endDescTableRow();
      }
    }
    if (!first)
    {
      ol.endDescTable();
    }
  }
}

// match from the start of the scope until the last marker
static const reg::Ex reAnonymous(R"([\w:@]*@\d+)");

QCString MemberDefImpl::displayDefinition() const
{
  QCString ldef = definition();
  QCString title = name();
  if (isEnumerate())
  {
    if (isAnonymous())
    {
      ldef = title = "anonymous enum";
      if (!m_enumBaseType.isEmpty())
      {
        ldef+=" : "+m_enumBaseType;
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
    if (isAnonymous())
    {
      ldef=ldef.mid(2);
    }
  }

  std::string sdef = ldef.str();
  reg::Match match;
  if (reg::search(sdef,match,reAnonymous))
  {
    ldef = match.prefix().str() + " { ... } " + removeAnonymousScopes(match.suffix().str());
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
    int l=ldef.length();
    //printf("start >%s<\n",qPrint(ldef));
    int i=l-1;
    while (i>=0 && (isId(ldef.at(i)) || ldef.at(i)==':')) i--;
    while (i>=0 && isspace(static_cast<uint8_t>(ldef.at(i)))) i--;
    if (i>0)
    {
      // insert branches around the type
      ldef = "("+ldef.left(i+1)+")"+ldef.mid(i+1);
    }
    //printf("end   >%s< i=%d\n",qPrint(ldef),i);
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
  bool showGroupedMembInc = Config_getBool(SHOW_GROUPED_MEMB_INC);
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

void MemberDefImpl::_writeMultiLineInitializer(OutputList &ol,const QCString &scopeName) const
{
    //printf("md=%s initLines=%d init='%s'\n",qPrint(name()),initLines,qPrint(init));
    ol.startBold();
    if (m_mtype==MemberType_Define)
      ol.parseText(theTranslator->trDefineValue());
    else
      ol.parseText(theTranslator->trInitialValue());
    ol.endBold();
    QCString langCorrected = getDefFileExtension();
    SrcLangExt srcLangExt = getLanguageFromFileName(getDefFileExtension());
    if (srcLangExt == SrcLangExt_Lex)
    {
      langCorrected = ".doxygen_lex_c";
      srcLangExt = SrcLangExt_Cpp;
    }
    auto intf = Doxygen::parserManager->getCodeParser(langCorrected);
    intf->resetCodeParserState();
    auto &codeOL = ol.codeGenerators();
    codeOL.startCodeFragment("DoxyCode");
    intf->parseCode(codeOL,scopeName,m_initializer,srcLangExt,FALSE,QCString(),const_cast<FileDef*>(getFileDef()),
                     -1,-1,TRUE,this,FALSE,this);
    codeOL.endCodeFragment("DoxyCode");
}

/*! Writes the "detailed documentation" section of this member to
 *  all active output formats.
 */
void MemberDefImpl::writeDocumentation(const MemberList *ml,
                                   int memCount,int memTotal,
                                   OutputList &ol,
                                   const QCString &scName,
                                   const Definition *container,
                                   bool inGroup,
                                   bool  /* showEnumValues */,
                                   bool showInline
                                  ) const
{
  // if this member is in a group find the real scope name.
  bool hasParameterList = FALSE;

  //printf("MemberDefImpl::writeDocumentation(): name='%s' hasDocs='%d' containerType=%d inGroup=%d sectionLinkable=%d\n",
  //    qPrint(name()),hasDocs,container->definitionType(),inGroup,hasDetailedDescription());

  //if ( !hasDocs ) return;
  //if (isEnumValue() && !showEnumValues) return;

  SrcLangExt lang = getLanguage();
  //printf("member=%s lang=%d\n",qPrint(name()),lang);
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
    ciname = (toGroupDef(container))->groupTitle();
  }
  else if (container->definitionType()==TypeFile && getNamespaceDef() && getNamespaceDef()->isLinkable())
  { // member is in a namespace, but is written as part of the file documentation
    // as well, so we need to make sure its anchor is unique (it is not really used).
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
  //printf("member '%s' def='%s'\n",qPrint(name()),qPrint(ldef));
  if (isEnumerate())
  {
    if (title.at(0)=='@')
    {
      ldef = title = "anonymous enum";
      if (!m_enumBaseType.isEmpty())
      {
        ldef+=" : "+m_enumBaseType;
      }
    }
    else
    {
      if (lang==SrcLangExt_Cpp)
      {
        if (isEnumStruct()) ldef.prepend("struct ");
        else if (isStrong()) ldef.prepend("class ");
      }
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
  else if (isFunction() && !isObjCMethod())
  {
    title += "()";
  }
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
  StringVector sl = getLabels(scopedContainer);

  static const reg::Ex r(R"(@\d+)");
  reg::Match match;
  std::string sdef = ldef.str();
  if ((isVariable() || isTypedef()) && reg::search(sdef,match,r))
  {
    // find enum type and insert it in the definition
    bool found=false;
    for (const auto &vmd : *ml)
    {
      if (vmd->isEnumerate() && match.str()==vmd->name())
      {
        ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
        ol.startMemberDoc(ciname,name(),memAnchor,name(),memCount,memTotal,showInline);
        std::string prefix = match.prefix().str();
        std::string suffix = match.suffix().str();
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,prefix.c_str());
        vmd->writeEnumDeclaration(ol,getClassDef(),getNamespaceDef(),getFileDef(),getGroupDef(),getModuleDef());
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,suffix.c_str());

        found=true;
        break;
      }
    }
    if (!found) // anonymous compound
    {
      ClassDef *annoClassDef=getClassDefOfAnonymousType();
      QCString typeName;
      if (annoClassDef) typeName=annoClassDef->compoundTypeString();
      ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
      ol.startMemberDoc(ciname,name(),memAnchor,"["+typeName+"]",memCount,memTotal,showInline);
      // search for the last anonymous compound name in the definition

      ol.startMemberDocName(isObjCMethod());
      if (reg::search(sdef,match,reAnonymous))
      {
        std::string prefix = match.prefix().str();
        std::string suffix = match.suffix().str();
        ol.docify(prefix.c_str());
        ol.docify(" { ... } ");
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,removeAnonymousScopes(suffix.c_str()));
      }
      else
      {
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef);
      }
    }
  }
  else // not an enum value or anonymous compound
  {
    ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
    ol.startMemberDoc(ciname,name(),memAnchor,title,memCount,memTotal,showInline);

    if (!m_metaData.isEmpty() && getLanguage()==SrcLangExt_Slice)
    {
      ol.startMemberDocPrefixItem();
      ol.docify(m_metaData);
      ol.endMemberDocPrefixItem();
    }

    const ClassDef *cd=getClassDef();
    const NamespaceDef *nd=getNamespaceDef();
    bool first=TRUE;
    if (!m_defTmpArgLists.empty() && lang==SrcLangExt_Cpp)
      // definition has explicit template parameter declarations
    {
      for (const ArgumentList &tal : m_defTmpArgLists)
      {
        if (!tal.empty())
        {
          if (!first) ol.docify(" ");
          ol.startMemberDocPrefixItem();
          _writeTemplatePrefix(ol,scopedContainer,tal);
          ol.endMemberDocPrefixItem();
        }
      }
    }
    else // definition gets it template parameters from its class
      // (since no definition was found)
    {
      if (cd && lang==SrcLangExt_Cpp && !isTemplateSpecialization())
      {
        for (const ArgumentList &tal : cd->getTemplateParameterLists())
        {
          if (!tal.empty())
          {
            if (!first) ol.docify(" ");
            ol.startMemberDocPrefixItem();
            _writeTemplatePrefix(ol,scopedContainer,tal,false);
            ol.endMemberDocPrefixItem();
          }
        }
      }
      if (m_tArgList.hasParameters() && lang==SrcLangExt_Cpp) // function template prefix
      {
        ol.startMemberDocPrefixItem();
        _writeTemplatePrefix(ol,scopedContainer,m_tArgList);
        ol.endMemberDocPrefixItem();
      }
    }

    if (!sl.empty())
    {
      ol.pushGeneratorState();
      ol.disableAll();
      ol.enable(OutputType::Html);
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
      int dl=ldef.length();
      //printf("start >%s<\n",qPrint(ldef));
      int i=dl-1;
      while (i>=0 && (isId(ldef.at(i)) || ldef.at(i)==':')) i--;
      while (i>=0 && isspace(static_cast<uint8_t>(ldef.at(i)))) i--;
      if (i>0)
      {
        // insert branches around the type
        ldef="("+ldef.left(i+1)+")"+ldef.mid(i+1);
      }
      //printf("end   >%s< i=%d\n",qPrint(ldef),i);
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
      if (pos<0) pos=0;
      if (pos>0)
      {
        linkifyText(TextGeneratorOLImpl(ol),
                    scopedContainer,
                    getBodyDef(),
                    this,
                    ldef.left(pos)
                   );
      }
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
        QCString init = m_initializer.simplifyWhiteSpace();
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,init);
      }
      else
      {
        ol.writeNonBreakableSpace(3);
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,m_initializer);
      }
    }
    if (!excpString().isEmpty()) // add exception list
    {
      writeExceptionList(ol,cd,this);
      hasParameterList=true; // call endParameterList below
    }
  }

  ol.pushGeneratorState();
  ol.disable(OutputType::Html);
  if (!sl.empty())
  {
    ol.startLabels();
    size_t count=0;
    for (const auto &s : sl)
    {
      count++;
      ol.writeLabel(s.c_str(),count==sl.size());
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
  ol.enable(OutputType::Html);
  if (htmlEndLabelTable)
  {
    ol.writeString("  </td>\n");
    ol.writeString("  <td class=\"mlabels-right\">\n");
    ol.startLabels();
    size_t count=0;
    for (const auto &s : sl)
    {
      count++;
      ol.writeLabel(s.c_str(),count==sl.size());
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
  if (hasMultiLineInitializer()) _writeMultiLineInitializer(ol,scopeName);

  /* write brief description */
  QCString brief = briefDescription();
  if (!brief.isEmpty() &&
      (Config_getBool(REPEAT_BRIEF) ||
       !Config_getBool(BRIEF_MEMBER_DESC)
      )
     )
  {
    ol.startParagraph();
    ol.generateDoc(briefFile(),briefLine(),
                scopedContainer,this,
                brief,FALSE,FALSE,
                QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    ol.endParagraph();
  }

  /* write detailed description */
  QCString detailed = documentation();
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
      ol.generateDoc(docFile(),docLine(),scopedContainer,this,detailed+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }

    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),
                     scopedContainer,this,
                     inbodyDocumentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
  }
  else if (!brief.isEmpty() && (Config_getBool(REPEAT_BRIEF) || !Config_getBool(BRIEF_MEMBER_DESC)))
  {
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),scopedContainer,this,inbodyDocumentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
  }


  //printf("***** defArgList=%p name=%s docs=%s hasDocs=%d\n",
  //     defArgList,
  //     defArgList?defArgList->hasDocumentation():-1);
  const ArgumentList &docArgList = m_templateMaster ?
                                   m_templateMaster->argumentList() :
                                   m_defArgList;
  ol.generateDoc(
        docFile(),docLine(),
        scopedContainer,
        this,         // memberDef
        inlineArgListToDoc(docArgList),    // docStr
        TRUE,         // indexWords
        FALSE,        // isExample
        QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT)
        );

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

  ol.endIndent();

  // enable LaTeX again
  //if (Config_getBool(EXTRACT_ALL) && !hasDocs) ol.enable(OutputType::Latex);
  ol.popGeneratorState();

  warnIfUndocumentedParams();
}

// strip scope and field name from the type
// example: "struct N<K::J>::S.v.c" will become "struct v"
static QCString simplifyTypeForTable(const QCString &s)
{
  QCString ts=removeAnonymousScopes(s);
  if (ts.endsWith("::")) ts = ts.left(ts.length()-2);
  static const reg::Ex re1(R"(\a\w*::)");       // non-template version
  static const reg::Ex re2(R"(\a\w*<[^>]*>::)"); // template version
  reg::Match match;
  std::string t = ts.str();
  while (reg::search(t,match,re2) || reg::search(t,match,re1))
  {
    t = match.prefix().str() + match.suffix().str(); // remove the matched part
  }
  //printf("simplifyTypeForTable(%s)->%s\n",qPrint(s),t.c_str());
  return QCString(t);
}

QCString MemberDefImpl::fieldType() const
{
  QCString type = m_accessorType;
  if (type.isEmpty())
  {
    type = m_type;
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

  ClassDef *cd = m_accessorClass;
  //printf("===> %s::anonymous: %s\n",qPrint(name()),cd?qPrint(cd->name()):"<none>");

  if (container && container->definitionType()==Definition::TypeClass &&
      !(toClassDef(container))->isJavaEnum())
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
  if (isVariable() && !argsString().isEmpty() && !isObjCMethod())
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,argsString());
  }
  if (!m_bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,m_bitfields);
  }
  if (hasOneLineInitializer() && !isDefine())
  {
    ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,m_initializer.simplifyWhiteSpace());
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
                brief,FALSE,FALSE,
                QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
  }

  /* write detailed description */
  if (!detailed.isEmpty())
  {
    if (!brief.isEmpty())
    {
      ol.disable(OutputType::Html);
      ol.lineBreak();
      ol.enable(OutputType::Html);
    }
    ol.generateDoc(docFile(),docLine(),
                getOuterScope()?getOuterScope():container,this,
                detailed+"\n",FALSE,FALSE,
                QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));

  }

  ol.endInlineMemberDoc();
}

QCString MemberDefImpl::memberTypeName() const
{
  switch (m_mtype)
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
   *  if (m_memberGroup) return;
   */
  const ClassDef     *cd = getClassDef();
  const NamespaceDef *nd = getNamespaceDef();
  const FileDef      *fd = getFileDef();
  const GroupDef     *gd = getGroupDef();
  const Definition *d=0;
  QCString t;
  if (cd)
    t=cd->compoundTypeString(), d=cd;
  else if (nd)
  {
    d=nd;
    t=nd->compoundTypeString();
  }
  else if (gd)
    t="group", d=gd;
  else
    t="file", d=fd;
  bool extractAll = Config_getBool(EXTRACT_ALL);

  //printf("%s:warnIfUndoc: hasUserDocs=%d isFriendClass=%d protection=%d isRef=%d isDel=%d\n",
  //    qPrint(name()),
  //    hasUserDocumentation(),isFriendClass(),protectionLevelVisible(m_prot),isReference(),isDeleted());
  if ((!hasUserDocumentation() && !extractAll) &&
      !isFriendClass() &&
      name().find('@')==-1 && d && d->name().find('@')==-1 &&
      !_isAnonymousBitField() &&
      protectionLevelVisible(m_prot) &&
      !isReference() && !isDeleted()
     )
  {
    SrcLangExt lang = getLanguage();
    QCString sep = getLanguageSpecificSeparator(lang,TRUE);
    warn_undoc(getDefFileName(),getDefLine(),"Member %s%s (%s) of %s %s is not documented.",
         qPrint(name()),qPrint(argsString()),qPrint(memberTypeName()),qPrint(t),
         qPrint(sep=="::"?d->name():substitute(d->name(),"::",sep)));
  }
  else if (!hasDetailedDescription())
  {
    warnIfUndocumentedParams();
  }

  // if it is an enum, we check that its members are documented
  if (!extractAll && isEnumerate() && Config_getBool(WARN_IF_UNDOC_ENUM_VAL))
  {
    for (const auto &fmd : enumFieldList())
    {
      if (!fmd->isLinkableInProject())
      {
        SrcLangExt lang = getLanguage();
        QCString sep = getLanguageSpecificSeparator(lang,TRUE);
        warn(fmd->getDefFileName(),fmd->getDefLine(), "Documentation for enum member '%s%s%s' is missing.",
             qPrint(qualifiedName()),qPrint(sep),qPrint(fmd->name()));
      }
    }
  }
}

bool MemberDefImpl::visibleInIndex() const
{
  bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
  bool extractStatic = Config_getBool(EXTRACT_STATIC);
  return (!isAnonymous() &&
      (!hideUndocMembers || hasDocumentation()) &&
      (!isStatic() || extractStatic) &&
      isLinkable()
     );
}

static QCString stripTrailingReturn(const QCString &trailRet)
{
  QCString ret = trailRet;

  ret = ret.stripWhiteSpace();
  if (ret.startsWith("->"))
  {
    ret = ret.mid(2).stripWhiteSpace();
    return ret;
  }
  return trailRet;
}

static std::mutex g_detectUndocumentedParamsMutex;

void MemberDefImpl::detectUndocumentedParams(bool hasParamCommand,bool hasReturnCommand) const
{
  // this function is called while parsing the documentation. A member can have multiple
  // documentation blocks, which could be handled by multiple threads, hence this guard.
  std::lock_guard<std::mutex> lock(g_detectUndocumentedParamsMutex);

  bool isPython = getLanguage()==SrcLangExt_Python;

  if (!m_hasDocumentedParams && hasParamCommand)
  {
    //printf("%s:hasDocumentedParams=TRUE;\n",qPrint(name()));
    m_hasDocumentedParams = TRUE;
  }
  else if (!m_hasDocumentedParams)
  {
    const ArgumentList &al     = argumentList();
    const ArgumentList &declAl = declArgumentList();
    bool allDoc=TRUE; // no parameter => all parameters are documented
    if ( // member has parameters
        al.hasParameters()  // with at least one parameter (that is not void)
       )
    {
      // see if all parameters have documentation
      for (auto it = al.begin(); it!=al.end() && allDoc; ++it)
      {
        Argument a = *it;
        if (!a.name.isEmpty() && a.type!="void" &&
            !(isPython && (a.name=="self" || a.name=="cls"))
           )
        {
          allDoc = !a.docs.isEmpty();
        }
        //printf("a->type=%s a->name=%s doc=%s\n",
        //        qPrint(a->type),qPrint(a->name),qPrint(a->docs));
      }
      if (!allDoc && declAl.empty()) // try declaration arguments as well
      {
        allDoc=TRUE;
        for (auto it = al.begin(); it!=al.end() && allDoc; ++it)
        {
          Argument a = *it;
          if (!a.name.isEmpty() && a.type!="void" &&
              !(isPython && (a.name=="self" || a.name=="cls"))
             )
          {
            allDoc = !a.docs.isEmpty();
          }
          //printf("a->name=%s doc=%s\n",qPrint(a->name),qPrint(a->docs));
        }
      }
    }
    if (allDoc)
    {
      //printf("%s:hasDocumentedParams=TRUE;\n",qPrint(name()));
      m_hasDocumentedParams = TRUE;
    }
  }

  //printf("Member %s hasDocumentedReturnType=%d hasReturnCommand=%d\n",
  //    qPrint(name()),m_hasDocumentedReturnType,hasReturnCommand);
  if (!m_hasDocumentedReturnType && // docs not yet found
      hasReturnCommand)
  {
    m_hasDocumentedReturnType = TRUE;
  }
}

void MemberDefImpl::warnIfUndocumentedParams() const
{
  QCString returnType = typeString();
  bool isFortran = getLanguage()==SrcLangExt_Fortran;
  bool isFortranSubroutine = isFortran && returnType.find("subroutine")!=-1;

  bool isVoidReturn =   returnType=="void" || returnType.endsWith(" void");
  if (!isVoidReturn && (returnType=="auto" || returnType.endsWith(" auto")))
  {
    const ArgumentList &defArgList=isDocsForDefinition() ?  argumentList() : declArgumentList();
    if (!defArgList.trailingReturnType().isEmpty())
    {
      QCString strippedTrailingReturn = stripTrailingReturn(defArgList.trailingReturnType());
      isVoidReturn = (strippedTrailingReturn=="void") || (strippedTrailingReturn.endsWith(" void"));
    }
  }
  if (!Config_getBool(EXTRACT_ALL) &&
      Config_getBool(WARN_IF_UNDOCUMENTED) &&
      Config_getBool(WARN_NO_PARAMDOC) &&
      isFunction() &&
      !isDeleted() &&
      !isReference() &&
      !Doxygen::suppressDocWarnings)
  {
    if (!m_hasDocumentedParams)
    {
      warn_doc_error(docFile(),docLine(),
          "parameters of member %s are not documented",
          qPrint(qualifiedName()));
    }
    if (!m_hasDocumentedReturnType &&
        hasDocumentation() && !returnType.isEmpty() &&
        !( // not one of the cases where nothing is returned
          isVoidReturn        || // void return type
          isFortranSubroutine || // fortran subroutine
          isConstructor()     || // a constructor
          isDestructor()         // or destructor
         )
       )
    {
      warn_doc_error(docFile(),docLine(),
          "return type of member %s is not documented",
          qPrint(qualifiedName()));
    }
  }
  if (Config_getBool(WARN_IF_DOC_ERROR) &&
            m_hasDocumentedReturnType &&
            (isVoidReturn        || // void return type
             isFortranSubroutine || // fortran subroutine
             isConstructor()     || // a constructor
             isDestructor()))       // or destructor
  {
    warn_doc_error(docFile(),docLine(),"found documented return type for %s that does not return anything",
                   qPrint(qualifiedName()));
  }
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
  return m_defArgList.isDeleted();
}

bool MemberDefImpl::hasDocumentation() const
{
  return DefinitionMixin::hasDocumentation() ||
         (m_mtype==MemberType_Enumeration && m_docEnumValues) ||  // has enum values
         (m_defArgList.hasDocumentation());   // has doc arguments
}


void MemberDefImpl::setMemberGroup(MemberGroup *grp)
{
  m_memberGroup = grp;
}

QCString MemberDefImpl::getScopeString() const
{
  QCString result;
  if (getClassDef()) result=getClassDef()->displayName();
  else if (getNamespaceDef()) result=getNamespaceDef()->displayName();
  return result;
}

void MemberDefImpl::setAnchor()
{
  static bool predictableURLs = Config_getBool(PREDICTABLE_URLS);
  QCString memAnchor = name();
  if (predictableURLs)
  {
    // assumes name() contains only valid fragment chars
    m_anc = memAnchor;
    return;
  }
  if (!m_args.isEmpty()) memAnchor+=m_args;

  memAnchor.prepend(definition()); // actually the method name is now included
            // twice, which is silly, but we keep it this way for backward
            // compatibility.

  // include number of template arguments as well,
  // to distinguish between two template
  // specializations that only differ in the template parameters.
  if (m_tArgList.hasParameters())
  {
    char buf[20];
    qsnprintf(buf,20,"%d:",static_cast<int>(m_tArgList.size()));
    buf[19]='\0';
    memAnchor.prepend(buf);
  }
  if (!m_requiresClause.isEmpty())
  {
    memAnchor+=" "+m_requiresClause;
  }

  // convert to md5 hash
  uint8_t md5_sig[16];
  char sigStr[33];
  MD5Buffer(memAnchor.data(),memAnchor.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr);
  m_anc = QCString("a")+sigStr;
}

void MemberDefImpl::setGroupDef(const GroupDef *gd,Grouping::GroupPri_t pri,
                            const QCString &fileName,int startLine,
                            bool hasDocs,MemberDef *member)
{
  //printf("%s MemberDefImpl::setGroupDef(%s)\n",qPrint(name()),qPrint(gd->name()));
  m_group=gd;
  m_grouppri=pri;
  m_groupFileName=fileName;
  m_groupStartLine=startLine;
  m_groupHasDocs=hasDocs;
  m_groupMember=member;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setEnumScope(const MemberDef *md,bool livesInsideEnum)
{
  m_enumScope=md;
  m_livesInsideEnum=livesInsideEnum;
  if (md->getGroupDef())
  {
    m_group           = const_cast<GroupDef*>(md->getGroupDef());
    m_grouppri        = md->getGroupPri();
    m_groupFileName   = md->getGroupFileName();
    m_groupStartLine  = md->getGroupStartLine();
    m_groupHasDocs    = md->getGroupHasDocs();
    m_isLinkableCached = 0;
  }
}

void MemberDefImpl::setMemberClass(ClassDef *cd)
{
  m_classDef=cd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  setOuterScope(const_cast<ClassDef*>(cd));
}

void MemberDefImpl::setNamespace(NamespaceDef *nd)
{
  m_nspace=nd;
  setOuterScope(const_cast<NamespaceDef*>(nd));
}

std::unique_ptr<MemberDef> MemberDefImpl::createTemplateInstanceMember(
        const ArgumentList &formalArgs,const std::unique_ptr<ArgumentList> &actualArgs) const
{
  //printf("  Member %s %s %s\n",typeString(),qPrint(name()),argsString());
  std::unique_ptr<ArgumentList> actualArgList = std::make_unique<ArgumentList>(m_defArgList);
  if (!m_defArgList.empty())
  {
    // replace formal arguments with actuals
    for (Argument &arg : *actualArgList)
    {
      arg.type = substituteTemplateArgumentsInString(arg.type,formalArgs,actualArgs);
    }
    actualArgList->setTrailingReturnType(
       substituteTemplateArgumentsInString(actualArgList->trailingReturnType(),formalArgs,actualArgs));
  }

  QCString methodName=name();
  if (methodName.startsWith("operator ")) // conversion operator
  {
    methodName=substituteTemplateArgumentsInString(methodName,formalArgs,actualArgs);
  }

  auto imd = createMemberDef(
                       getDefFileName(),getDefLine(),getDefColumn(),
                       substituteTemplateArgumentsInString(m_type,formalArgs,actualArgs),
                       methodName,
                       substituteTemplateArgumentsInString(m_args,formalArgs,actualArgs),
                       m_exception, m_prot,
                       m_virt, m_stat, m_related, m_mtype,
                       ArgumentList(), ArgumentList(), ""
                   );
  auto mmd = toMemberDefMutable(imd.get());
  mmd->moveArgumentList(std::move(actualArgList));
  mmd->setDefinition(substituteTemplateArgumentsInString(m_def,formalArgs,actualArgs));
  mmd->setBodyDef(getBodyDef());
  mmd->setBodySegment(getDefLine(),getStartBodyLine(),getEndBodyLine());
  //imd->setBodyMember(this);

  // TODO: init other member variables (if needed).
  // TODO: reimplemented info
  return imd;
}

bool MemberDefImpl::hasOneLineInitializer() const
{
  //printf("%s: init=%s, initLines=%d maxInitLines=%d userInitLines=%d\n",
  //    qPrint(name()),qPrint(m_initializer),m_initLines,
  //    m_maxInitLines,m_userInitLines);
  return !m_initializer.isEmpty() && m_initLines==0 && // one line initializer
         ((m_maxInitLines>0 && m_userInitLines==-1) || m_userInitLines>0); // enabled by default or explicitly
}

bool MemberDefImpl::hasMultiLineInitializer() const
{
  //printf("initLines=%d userInitLines=%d maxInitLines=%d\n",
  //    initLines,userInitLines,maxInitLines);
  return m_initLines>0 &&
         ((m_initLines<m_maxInitLines && m_userInitLines==-1) // implicitly enabled
          || m_initLines<m_userInitLines // explicitly enabled
         );
}

void MemberDefImpl::setInitializer(const QCString &initializer)
{
  m_initializer=initializer;
  int l=m_initializer.length();
  int p=l-1;
  while (p>=0 && isspace(static_cast<uint8_t>(m_initializer.at(p)))) p--;
  m_initializer=m_initializer.left(p+1);
  m_initLines=m_initializer.contains('\n');
  //printf("%s::setInitializer(%s)\n",qPrint(name()),qPrint(m_initializer));
}

void MemberDefImpl::addListReference(Definition *)
{
  bool optimizeOutputForC = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
  //bool hideScopeNames     = Config_getBool(HIDE_SCOPE_NAMES);
  //bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
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
                    (toClassDef(pd))->displayName() : pd->name();
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
  const RefItemVector &xrefItems = xrefListItems();
  addRefItem(xrefItems,
        qualifiedName()+argsString(), // argsString is needed for overloaded functions (see bug 609624)
        memLabel,
        getOutputFileBase()+"#"+anchor(),memName,memArgs,pd);
}

const MemberList *MemberDefImpl::getSectionList(const Definition *container) const
{
  auto it = m_sectionMap.find(container);
  return it!=m_sectionMap.end() ? it->second : 0;
}

void MemberDefImpl::setSectionList(const Definition *container,const MemberList *sl)
{
  //printf("MemberDefImpl::setSectionList(%s,%p) name=%s\n",qPrint(d->name()),sl,qPrint(name()));
  m_sectionMap.insert(std::make_pair(container,sl));
}

Specifier MemberDefImpl::virtualness(int count) const
{
  if (count>25)
  {
     warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: recursion detected in overload relation for member %s!"
       ,qPrint(name())
      );
     return Specifier::Normal;
  }
  Specifier v = m_virt;
  const MemberDef *rmd = reimplements();
  while (rmd && v==Specifier::Normal)
  {
    v = rmd->virtualness(count+1)==Specifier::Normal ? Specifier::Normal : Specifier::Virtual;
    rmd = rmd->reimplements();
  }
  return v;
}

void MemberDefImpl::writeTagFile(TextStream &tagFile,bool useQualifiedName,bool showNamespaceMembers) const
{
  if (!isLinkableInProject()) return;
  if (!showNamespaceMembers && getNamespaceDef()) return;
  tagFile << "    <member kind=\"";
  switch (m_mtype)
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
  if (m_prot!=Protection::Public)
  {
    tagFile << "\" protection=\"";
    if (m_prot==Protection::Protected)    tagFile << "protected";
    else if (m_prot==Protection::Package) tagFile << "package";
    else /* Private */                          tagFile << "private";
  }
  if (m_virt!=Specifier::Normal)
  {
    tagFile << "\" virtualness=\"";
    if (m_virt==Specifier::Virtual) tagFile << "virtual";
    else /* Pure */                       tagFile << "pure";
  }
  if (isStatic())
  {
    tagFile << "\" static=\"yes";
  }
  tagFile << "\">\n";
  if (typeString()!=QCString("@"))
  {
    tagFile << "      <type>" << convertToXML(typeString()) << "</type>\n";
  }
  QCString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "      <name>" << convertToXML(useQualifiedName ? qualifiedName() : name()) << "</name>\n";
  tagFile << "      <anchorfile>" << convertToXML(fn) << "</anchorfile>\n";
  tagFile << "      <anchor>" << convertToXML(anchor()) << "</anchor>\n";
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "      <clangid>" << convertToXML(idStr) << "</clangid>\n";
  }
  tagFile << "      <arglist>" << convertToXML(argsString()) << "</arglist>\n";
  if (isStrong())
  {
    for (const auto &fmd : m_enumFields)
    {
      if (!fmd->isReference())
      {
        tagFile << "      <enumvalue file=\"" << convertToXML(fn);
        tagFile << "\" anchor=\"" << convertToXML(fmd->anchor());
        idStr = fmd->id();
        if (!idStr.isEmpty())
        {
          tagFile << "\" clangid=\"" << convertToXML(idStr);
        }
        tagFile  << "\">" << convertToXML(fmd->name()) << "</enumvalue>\n";
      }
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "    </member>\n";
}

void MemberDefImpl::_computeIsConstructor()
{
  m_isConstructorCached=1; // FALSE
  if (getClassDef())
  {
    if (m_isDMember) // for D
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
    MemberDefImpl *that = const_cast<MemberDefImpl*>(this);
    that->_computeIsConstructor();
  }
  ASSERT(m_isConstructorCached>0);
  return m_isConstructorCached==2;

}

void MemberDefImpl::_computeIsDestructor()
{
  bool isDestructor;
  if (m_isDMember) // for D
  {
    isDestructor = name()=="~this";
  }
  else if (getLanguage()==SrcLangExt_PHP) // for PHP
  {
    isDestructor = name()=="__destruct";
  }
  else if (name()=="__del__" &&
           getLanguage()==SrcLangExt_Python) // for Python
  {
    isDestructor=TRUE;
  }
  else if (getLanguage()==SrcLangExt_Fortran) // for Fortran
  {
    isDestructor = typeString()=="final";
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
    MemberDefImpl *that=const_cast<MemberDefImpl*>(this);
    that->_computeIsDestructor();
  }
  ASSERT(m_isDestructorCached>0);
  return m_isDestructorCached==2;
}

void MemberDefImpl::writeEnumDeclaration(OutputList &typeDecl,
     const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod) const
{
  int enumMemCount=0;

  uint32_t numVisibleEnumValues=0;
  for (const auto &fmd : m_enumFields)
  {
    if (fmd->isBriefSectionVisible()) numVisibleEnumValues++;
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
      writeLink(typeDecl,cd,nd,fd,gd,mod);
    }
    else
    {
      typeDecl.startBold();
      typeDecl.docify(n);
      typeDecl.endBold();
    }
    typeDecl.writeChar(' ');
  }
  if (!m_enumBaseType.isEmpty())
  {
    typeDecl.writeChar(':');
    typeDecl.writeChar(' ');
    typeDecl.docify(m_enumBaseType);
    typeDecl.writeChar(' ');
  }

  uint32_t enumValuesPerLine = static_cast<uint32_t>(Config_getInt(ENUM_VALUES_PER_LINE));
  if (numVisibleEnumValues>0 && enumValuesPerLine>0)
  {
    typeDecl.docify("{ ");

    auto it = m_enumFields.begin();
    if (it!=m_enumFields.end())
    {
      const MemberDef *fmd=*it;
      bool fmdVisible = fmd->isBriefSectionVisible();
      bool first=true;
      while (fmd)
      {
        if (fmdVisible)
        {
          if (!first)
          {
            typeDecl.writeString(", ");
          }
          /* in html we start a new line after a number of items */
          if (numVisibleEnumValues>enumValuesPerLine
              && (enumMemCount%enumValuesPerLine)==0
             )
          {
            typeDecl.pushGeneratorState();
            typeDecl.disableAllBut(OutputType::Html);
            typeDecl.enable(OutputType::Latex);
            typeDecl.enable(OutputType::Docbook);
            typeDecl.lineBreak();
            typeDecl.disable(OutputType::Latex);
            typeDecl.disable(OutputType::Docbook);
            typeDecl.writeString("&#160;&#160;");
            typeDecl.popGeneratorState();
          }

          if (fmd->hasDocumentation()) // enum value has docs
          {
            fmd->writeLink(typeDecl,cd,nd,fd,gd,mod);
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
          first=false;
        }

        bool prevVisible = fmdVisible;
        ++it;
        if (it!=m_enumFields.end())
        {
          fmd=*it;
        }
        else
        {
          fmd=0;
        }
        if (prevVisible)
        {
          typeDecl.disable(OutputType::Man);
          typeDecl.writeString("\n"); // to prevent too long lines in LaTeX
          typeDecl.enable(OutputType::Man);
          enumMemCount++;
        }
      }
      if (numVisibleEnumValues>enumValuesPerLine)
      {
        typeDecl.pushGeneratorState();
        typeDecl.disableAllBut(OutputType::Html);
        typeDecl.lineBreak();
        typeDecl.popGeneratorState();
      }
    }
    typeDecl.docify(" }");
  }
}

void MemberDefImpl::moveArgumentList(std::unique_ptr<ArgumentList> al)
{
  m_defArgList = *al;
}

void MemberDefImpl::moveDeclArgumentList(std::unique_ptr<ArgumentList> al)
{
  m_declArgList = *al;
}

void MemberDefImpl::setTypeConstraints(const ArgumentList &al)
{
  m_typeConstraints = al;
}

void MemberDefImpl::setType(const QCString &t)
{
  m_type = t;
}

void MemberDefImpl::setAccessorType(ClassDef *cd,const QCString &t)
{
  m_accessorClass = cd;
  m_accessorType = t;
}

ClassDef *MemberDefImpl::accessorClass() const
{
  return m_accessorClass;
}

void MemberDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
}

void MemberDefImpl::enableCallGraph(bool e)
{
  m_hasCallGraph=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableCallerGraph(bool e)
{
  m_hasCallerGraph=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableReferencedByRelation(bool e)
{
  m_hasReferencedByRelation=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableReferencesRelation(bool e)
{
  m_hasReferencesRelation=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDefImpl::enableInlineSource(bool e)
{
  m_hasInlineSource=e;
}

void MemberDefImpl::mergeEnableInlineSource(bool other)
{
  if (Config_getBool(INLINE_SOURCES))
  {
    enableInlineSource(m_hasInlineSource && other); // enabled if neither deviate from config value
  }
  else
  {
    enableInlineSource(m_hasInlineSource || other); // enabled if either deviate from config value
  }
}

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

bool MemberDefImpl::isCSharpProperty() const
{
  if (getClassDef() && getClassDef()->isCSharp() && isProperty()) return TRUE;
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
  else if (m_enumScope && m_enumScope->isStrong())
  {
    return m_enumScope->qualifiedName()+
           getLanguageSpecificSeparator(getLanguage())+
           localName();
  }
  else
  {
    return DefinitionMixin::qualifiedName();
  }
}

void MemberDefImpl::setTagInfo(const TagInfo *ti)
{
  if (ti)
  {
    //printf("%s: Setting tag name=%s anchor=%s\n",qPrint(name()),qPrint(ti->tagName),qPrint(ti->anchor));
    m_anc=ti->anchor;
    setReference(ti->tagName);
    m_explicitOutputFileBase = stripExtension(ti->fileName);
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

QCString MemberDefImpl::declaration() const
{
  return m_decl;
}

QCString MemberDefImpl::definition() const
{
  return m_def;
}

QCString MemberDefImpl::extraTypeChars() const
{
  return m_extraTypeChars;
}

QCString MemberDefImpl::typeString() const
{
  return m_type;
}

QCString MemberDefImpl::argsString() const
{
  return m_args;
}

QCString MemberDefImpl::excpString() const
{
  return m_exception;
}

QCString MemberDefImpl::bitfieldString() const
{
  return m_bitfields;
}

const QCString &MemberDefImpl::initializer() const
{
  return m_initializer;
}

int MemberDefImpl::initializerLines() const
{
  return m_initLines;
}

TypeSpecifier MemberDefImpl::getMemberSpecifiers() const
{
  return m_memSpec;
}

VhdlSpecifier MemberDefImpl::getVhdlSpecifiers() const
{
  return m_vhdlSpec;
}

const ClassDef *MemberDefImpl::getClassDef() const
{
  return m_classDef;
}

ClassDef *MemberDefImpl::getClassDef()
{
  return m_classDef;
}

ClassDefMutable *MemberDefImpl::getClassDefMutable()
{
  return toClassDefMutable(m_classDef);
}

const FileDef  *MemberDefImpl::getFileDef() const
{
  return m_fileDef;
}

FileDef  *MemberDefImpl::getFileDef()
{
  return m_fileDef;
}

const NamespaceDef* MemberDefImpl::getNamespaceDef() const
{
  return m_nspace;
}

NamespaceDef* MemberDefImpl::getNamespaceDef()
{
  return m_nspace;
}

QCString MemberDefImpl::getReadAccessor() const
{
  return m_read;
}

QCString MemberDefImpl::getWriteAccessor() const
{
  return m_write;
}

const GroupDef *MemberDefImpl::getGroupDef() const
{
  return m_group;
}

const ModuleDef *MemberDefImpl::getModuleDef() const
{
  return m_moduleDef;
}

Grouping::GroupPri_t MemberDefImpl::getGroupPri() const
{
  return m_grouppri;
}

QCString MemberDefImpl::getGroupFileName() const
{
  return m_groupFileName;
}

int MemberDefImpl::getGroupStartLine() const
{
  return m_groupStartLine;
}

bool MemberDefImpl::getGroupHasDocs() const
{
  return m_groupHasDocs;
}

Protection MemberDefImpl::protection() const
{
  return m_prot;
}

MemberType MemberDefImpl::memberType() const
{
  return m_mtype;
}

bool MemberDefImpl::isSignal() const
{
  return m_mtype==MemberType_Signal;
}

bool MemberDefImpl::isSlot() const
{
  return m_mtype==MemberType_Slot;
}

bool MemberDefImpl::isVariable() const
{
  return m_mtype==MemberType_Variable;
}

bool MemberDefImpl::isEnumerate() const
{
  return m_mtype==MemberType_Enumeration;
}

bool MemberDefImpl::isEnumValue() const
{
  return m_mtype==MemberType_EnumValue;
}

bool MemberDefImpl::isTypedef() const
{
  return m_mtype==MemberType_Typedef;
}

bool MemberDefImpl::isSequence() const
{
  return m_mtype==MemberType_Sequence;
}

bool MemberDefImpl::isDictionary() const
{
  return m_mtype==MemberType_Dictionary;
}

bool MemberDefImpl::isFunction() const
{
  return m_mtype==MemberType_Function;
}

bool MemberDefImpl::isFunctionPtr() const
{
  return m_mtype==MemberType_Variable && QCString(argsString()).find(")(")!=-1;
}

bool MemberDefImpl::isDefine() const
{
  return m_mtype==MemberType_Define;
}

bool MemberDefImpl::isFriend() const
{
  return m_mtype==MemberType_Friend;
}

bool MemberDefImpl::isDCOP() const
{
  return m_mtype==MemberType_DCOP;
}

bool MemberDefImpl::isProperty() const
{
  return m_mtype==MemberType_Property;
}

bool MemberDefImpl::isEvent() const
{
  return m_mtype==MemberType_Event;
}

bool MemberDefImpl::isRelated() const
{
  return m_related == Relationship::Related;
}

bool MemberDefImpl::isForeign() const
{
  return m_related == Relationship::Foreign;
}

bool MemberDefImpl::isStatic() const
{
  return m_stat;
}

bool MemberDefImpl::isInline() const
{
  return m_memSpec.isInline();
}

bool MemberDefImpl::isExplicit() const
{
  return m_memSpec.isExplicit();
}

bool MemberDefImpl::isMutable() const
{
  return m_memSpec.isMutable();
}

bool MemberDefImpl::isGettable() const
{
  return m_memSpec.isGettable();
}

bool MemberDefImpl::isPrivateGettable() const
{
  return m_memSpec.isPrivateGettable();
}

bool MemberDefImpl::isProtectedGettable() const
{
  return m_memSpec.isProtectedGettable();
}

bool MemberDefImpl::isSettable() const
{
  return m_memSpec.isSettable();
}

bool MemberDefImpl::isPrivateSettable() const
{
  return m_memSpec.isPrivateSettable();
}

bool MemberDefImpl::isProtectedSettable() const
{
  return m_memSpec.isProtectedSettable();
}

bool MemberDefImpl::isAddable() const
{
  return m_memSpec.isAddable();
}

bool MemberDefImpl::isRemovable() const
{
  return m_memSpec.isRemovable();
}

bool MemberDefImpl::isRaisable() const
{
  return m_memSpec.isRaisable();
}

bool MemberDefImpl::isReadable() const
{
  return m_memSpec.isReadable();
}

bool MemberDefImpl::isWritable() const
{
  return m_memSpec.isWritable();
}

bool MemberDefImpl::isFinal() const
{
  return m_memSpec.isFinal();
}

bool MemberDefImpl::isNew() const
{
  return m_memSpec.isNew();
}

bool MemberDefImpl::isSealed() const
{
  return m_memSpec.isSealed();
}

bool MemberDefImpl::isOverride() const
{
  return m_memSpec.isOverride();
}

bool MemberDefImpl::isInitonly() const
{
  return m_memSpec.isInitonly();
}

bool MemberDefImpl::isAbstract() const
{
  return m_memSpec.isAbstract();
}

bool MemberDefImpl::isOptional() const
{
  return m_memSpec.isOptional();
}

bool MemberDefImpl::isRequired() const
{
  return m_memSpec.isRequired();
}

bool MemberDefImpl::isNonAtomic() const
{
  return m_memSpec.isNonAtomic();
}

bool MemberDefImpl::isCopy() const
{
  return m_memSpec.isCopy();
}

bool MemberDefImpl::isAssign() const
{
  return m_memSpec.isAssign();
}

bool MemberDefImpl::isRetain() const
{
  return m_memSpec.isRetain();
}

bool MemberDefImpl::isWeak() const
{
  return m_memSpec.isWeak();
}

bool MemberDefImpl::isStrong() const
{
  return m_memSpec.isStrong();
}

bool MemberDefImpl::isEnumStruct() const
{
  return m_memSpec.isEnumStruct();
}

bool MemberDefImpl::isStrongEnumValue() const
{
  return m_mtype==MemberType_EnumValue &&
         m_enumScope &&
         m_enumScope->isStrong();
}

bool MemberDefImpl::isUnretained() const
{
  return m_memSpec.isUnretained();
}

bool MemberDefImpl::isTypeAlias() const
{
  return m_memSpec.isAlias();
}

bool MemberDefImpl::isDefault() const
{
  return m_memSpec.isDefault();
}

bool MemberDefImpl::isDelete() const
{
  return m_memSpec.isDelete();
}

bool MemberDefImpl::isNoExcept() const
{
  return m_memSpec.isNoExcept();
}

bool MemberDefImpl::isAttribute() const
{
  return m_memSpec.isAttribute();
}

bool MemberDefImpl::isUNOProperty() const
{
  return m_memSpec.isProperty();
}

bool MemberDefImpl::isReadonly() const
{
  return m_memSpec.isReadonly();
}

bool MemberDefImpl::isBound() const
{
  return m_memSpec.isBound();
}

bool MemberDefImpl::isConstrained() const
{
  return m_memSpec.isConstrained();
}

bool MemberDefImpl::isTransient() const
{
  return m_memSpec.isTransient();
}

bool MemberDefImpl::isMaybeVoid() const
{
  return m_memSpec.isMaybeVoid();
}

bool MemberDefImpl::isMaybeDefault() const
{
  return m_memSpec.isMaybeDefault();
}

bool MemberDefImpl::isMaybeAmbiguous() const
{
  return m_memSpec.isMaybeAmbiguous();
}

bool MemberDefImpl::isPublished() const
{
  return m_memSpec.isPublished();
}


bool MemberDefImpl::isImplementation() const
{
  return m_implOnly;
}

bool MemberDefImpl::isExternal() const
{
  return m_explExt;
}

bool MemberDefImpl::isTemplateSpecialization() const
{
  return m_tspec;
}

bool MemberDefImpl::isCallable() const
{
  return isFunction() ||
         isSlot() ||
         isSignal() ||
         isConstructor() ||
         isDestructor() ||
         isObjCMethod();
}

ClassDef *MemberDefImpl::relatedAlso() const
{
  return m_relatedAlso;
}

bool MemberDefImpl::hasDocumentedEnumValues() const
{
  return m_docEnumValues;
}

const MemberDef *MemberDefImpl::getAnonymousEnumType() const
{
  return m_annEnumType;
}

bool MemberDefImpl::isDocsForDefinition() const
{
  return m_docsForDefinition;
}

const MemberDef *MemberDefImpl::getEnumScope() const
{
  return m_enumScope;
}

bool MemberDefImpl::livesInsideEnum() const
{
  return m_livesInsideEnum;
}

bool MemberDefImpl::isSliceLocal() const
{
  return m_memSpec.isLocal();
}

bool MemberDefImpl::isConstExpr() const
{
  return m_memSpec.isConstExpr();
}

const MemberVector &MemberDefImpl::enumFieldList() const
{
  return m_enumFields;
}

const ExampleList &MemberDefImpl::getExamples() const
{
  return m_examples;
}

bool MemberDefImpl::isPrototype() const
{
  return m_proto;
}

const ArgumentList &MemberDefImpl::argumentList() const
{
  return m_defArgList;
}

const ArgumentList &MemberDefImpl::declArgumentList() const
{
  return m_declArgList;
}

void MemberDefImpl::resolveUnnamedParameters(const MemberDef *md)
{
  ArgumentList &decAl = m_declArgList;
  ArgumentList &defAl = m_defArgList;
  const ArgumentList &decAlSrc = md->declArgumentList();
  const ArgumentList &defAlSrc = md->argumentList();
  auto decSrc = decAlSrc.begin(), defSrc = defAlSrc.begin();
  for (auto decIt = decAl.begin(), defIt = defAl.begin();
       decIt != decAl.end() && defIt != defAl.end() && decSrc != decAlSrc.end() && defSrc != defAlSrc.end();
       ++decIt, ++defIt, ++decSrc, ++defSrc++)
  {
    Argument &decA = *decIt;
    Argument &defA = *defIt;
    const Argument &decAS = *decSrc;
    const Argument &defAS = *defSrc;
    if (decA.name.isEmpty())
    {
      if (!defA.name.isEmpty())
      {
        decA.name = defA.name;
      }
      else if (!decAS.name.isEmpty())
      {
        decA.name = decAS.name;
      }
      else if (!defAS.name.isEmpty())
      {
        decA.name = defAS.name;
      }
    }
    if (defA.name.isEmpty())
    {
      if (!decA.name.isEmpty())
      {
        defA.name = decA.name;
      }
      else if (!decAS.name.isEmpty())
      {
        defA.name = decAS.name;
      }
      else if (!defAS.name.isEmpty())
      {
        defA.name = defAS.name;
      }
    }
  }
}

const ArgumentList &MemberDefImpl::templateArguments() const
{
  return m_tArgList;
}

const ArgumentLists &MemberDefImpl::definitionTemplateParameterLists() const
{
  return m_defTmpArgLists;
}

int MemberDefImpl::getMemberGroupId() const
{
  return m_grpId;
}

MemberGroup *MemberDefImpl::getMemberGroup() const
{
  return m_memberGroup;
}

bool MemberDefImpl::fromAnonymousScope() const
{
  return m_annScope;
}

bool MemberDefImpl::hasCallGraph() const
{
  return m_hasCallGraph;
}

bool MemberDefImpl::_hasVisibleCallGraph() const
{
  bool enabled = m_hasCallGraph &&
         (isFunction() || isSlot() || isSignal()) &&
         Config_getBool(HAVE_DOT);
  if (enabled)
  {
    bool trivial = DotCallGraph::isTrivial(this,FALSE);
    return !trivial;
  }
  return FALSE;
}

bool MemberDefImpl::hasCallerGraph() const
{
  return m_hasCallerGraph;
}

bool MemberDefImpl::_hasVisibleCallerGraph() const
{
  bool enabled = m_hasCallerGraph &&
         (isFunction() || isSlot() || isSignal()) &&
         Config_getBool(HAVE_DOT);
  if (enabled)
  {
    bool trivial = DotCallGraph::isTrivial(this,TRUE);
    return !trivial;
  }
  return FALSE;
}

bool MemberDefImpl::hasReferencedByRelation() const
{
  return m_hasReferencedByRelation;
}

bool MemberDefImpl::hasReferencesRelation() const
{
  return m_hasReferencesRelation;
}

bool MemberDefImpl::hasInlineSource() const
{
  return m_hasInlineSource;
}

const MemberDef *MemberDefImpl::templateMaster() const
{
  return m_templateMaster;
}

bool MemberDefImpl::isTypedefValCached() const
{
  return m_isTypedefValCached;
}

const ClassDef *MemberDefImpl::getCachedTypedefVal() const
{
  return m_cachedTypedefValue;
}

QCString MemberDefImpl::getCachedTypedefTemplSpec() const
{
  return m_cachedTypedefTemplSpec;
}

QCString MemberDefImpl::getCachedResolvedTypedef() const
{
  //printf("MemberDefImpl::getCachedResolvedTypedef()=%s m_impl=%p\n",qPrint(m_cachedResolvedType),m_impl);
  return m_cachedResolvedType;
}

MemberDef *MemberDefImpl::memberDefinition() const
{
  return m_memDef;
}

MemberDef *MemberDefImpl::memberDeclaration() const
{
  return m_memDec;
}

const MemberDef *MemberDefImpl::inheritsDocsFrom() const
{
  return m_docProvider;
}

const MemberDef *MemberDefImpl::getGroupAlias() const
{
  return m_groupAlias;
}

QCString MemberDefImpl::getDeclFileName() const
{
  return m_declFileName;
}

int MemberDefImpl::getDeclLine() const
{
  return m_declLine;
}

int MemberDefImpl::getDeclColumn() const
{
  return m_declColumn;
}


//----------------------------------------------

void MemberDefImpl::setMemberType(MemberType t)
{
  m_mtype=t;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setDefinition(const QCString &d)
{
  m_def=d;
}

void MemberDefImpl::setFileDef(FileDef *fd)
{
  m_fileDef=fd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached = 0;
}

void MemberDefImpl::setModuleDef(ModuleDef *mod)
{
  m_moduleDef=mod;
}

void MemberDefImpl::setProtection(Protection p)
{
  m_prot=p;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setMemberSpecifiers(TypeSpecifier s)
{
  m_memSpec=s;
}

void MemberDefImpl::setVhdlSpecifiers(VhdlSpecifier s)
{
  m_vhdlSpec=s;
}

void MemberDefImpl::mergeMemberSpecifiers(TypeSpecifier s)
{
  m_memSpec.merge(s);
}

StringVector MemberDefImpl::getQualifiers() const
{
  return m_qualifiers;
}

void MemberDefImpl::addQualifiers(const StringVector &qualifiers)
{
  for (const auto &sx : qualifiers)
  {
    bool alreadyAdded = std::find(m_qualifiers.begin(), m_qualifiers.end(), sx) != m_qualifiers.end();
    if (!alreadyAdded)
    {
      m_qualifiers.push_back(sx);
    }
  }
}

void MemberDefImpl::setBitfields(const QCString &s)
{
  m_bitfields = QCString(s).simplifyWhiteSpace();
}

void MemberDefImpl::setMaxInitLines(int lines)
{
  if (lines!=-1)
  {
    m_userInitLines=lines;
  }
}

void MemberDefImpl::setReadAccessor(const QCString &r)
{
  m_read=r;
}

void MemberDefImpl::setWriteAccessor(const QCString &w)
{
  m_write=w;
}

void MemberDefImpl::setTemplateSpecialization(bool b)
{
  m_tspec=b;
}

void MemberDefImpl::makeRelated()
{
  m_related = Relationship::Related;
  m_isLinkableCached = 0;
}

void MemberDefImpl::makeForeign()
{
  m_related = Relationship::Foreign;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setInheritsDocsFrom(const MemberDef *md)
{
  m_docProvider = md;
}

void MemberDefImpl::setArgsString(const QCString &as)
{
  m_args = as;
}

void MemberDefImpl::setRelatedAlso(ClassDef *cd)
{
  m_relatedAlso=cd;
}

void MemberDefImpl::setEnumClassScope(ClassDef *cd)
{
  m_classDef = cd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
}

void MemberDefImpl::setDocumentedEnumValues(bool value)
{
  m_docEnumValues=value;
}

void MemberDefImpl::setAnonymousEnumType(const MemberDef *md)
{
  m_annEnumType = md;
}

void MemberDefImpl::setPrototype(bool p,const QCString &df,int line,int column)
{
  m_proto=p;
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
  m_explExt=b;
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
  m_declFileName = df;
  m_declLine = line;
  m_declColumn = column;
}

void MemberDefImpl::setMemberGroupId(int id)
{
  m_grpId=id;
}

void MemberDefImpl::makeImplementationDetail()
{
  m_implOnly=TRUE;
}

void MemberDefImpl::setFromAnonymousScope(bool b)
{
  m_annScope=b;
}

void MemberDefImpl::setFromAnonymousMember(MemberDef *m)
{
  m_annMemb=m;
}

MemberDef *MemberDefImpl::fromAnonymousMember() const
{
  return m_annMemb;
}

void MemberDefImpl::setTemplateMaster(MemberDef *mt)
{
  m_templateMaster=mt;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setDocsForDefinition(bool b)
{
  m_docsForDefinition = b;
}

void MemberDefImpl::setGroupAlias(const MemberDef *md)
{
  m_groupAlias = md;
}

void MemberDefImpl::invalidateTypedefValCache()
{
  m_isTypedefValCached=FALSE;
}

void MemberDefImpl::setMemberDefinition(MemberDef *md)
{
  m_memDef=md;
}

void MemberDefImpl::setMemberDeclaration(MemberDef *md)
{
  m_memDec=md;
}

ClassDef *MemberDefImpl::category() const
{
  return m_category;
}

void MemberDefImpl::setCategory(ClassDef *def)
{
  m_category = def;
}

const MemberDef *MemberDefImpl::categoryRelation() const
{
  return m_categoryRelation;
}

void MemberDefImpl::setCategoryRelation(const MemberDef *md)
{
  m_categoryRelation = md;
}

void MemberDefImpl::setEnumBaseType(const QCString &type)
{
  m_enumBaseType = type;
}

QCString MemberDefImpl::enumBaseType() const
{
  return m_enumBaseType;
}

void MemberDefImpl::setRequiresClause(const QCString &req)
{
  m_requiresClause = req;
}

QCString MemberDefImpl::requiresClause() const
{
  return m_requiresClause;
}

void MemberDefImpl::cacheTypedefVal(const ClassDef*val, const QCString & templSpec, const QCString &resolvedType)
{
  m_isTypedefValCached=TRUE;
  m_cachedTypedefValue=val;
  m_cachedTypedefTemplSpec=templSpec;
  m_cachedResolvedType=resolvedType;
  //printf("MemberDefImpl::cacheTypedefVal=%s m_impl=%p\n",qPrint(m_cachedResolvedType),m_impl);
}

void MemberDefImpl::copyArgumentNames(const MemberDef *bmd)
{
  {
    const ArgumentList &srcAl = bmd->argumentList();
    ArgumentList &dstAl = m_defArgList;
    auto srcIt = srcAl.begin();
    auto dstIt = dstAl.begin();
    while ( srcIt!=srcAl.end() && dstIt!=dstAl.end())
    {
      Argument &argDst       = *dstIt;
      const Argument &argSrc = *srcIt;
      argDst.name = argSrc.name;
      argDst.docs = argSrc.docs;
      ++srcIt;
      ++dstIt;
    }
  }

  {
    const ArgumentList &srcAl = bmd->declArgumentList();
    ArgumentList &dstAl = m_declArgList;
    auto srcIt = srcAl.begin();
    auto dstIt = dstAl.begin();

    while ( srcIt!=srcAl.end() && dstIt!=dstAl.end())
    {
      Argument &argDst       = *dstIt;
      const Argument &argSrc = *srcIt;
      argDst.name = argSrc.name;
      argDst.docs = argSrc.docs;
      ++srcIt;
      ++dstIt;
    }
  }
}

static void invalidateCachedTypesInArgumentList(ArgumentList &al)
{
  for (Argument &a : al)
  {
    a.canType.resize(0);
  }
}

void MemberDefImpl::invalidateCachedArgumentTypes()
{
  invalidateCachedTypesInArgumentList(m_defArgList);
  invalidateCachedTypesInArgumentList(m_declArgList);
}

void MemberDefImpl::incrementFlowKeyWordCount()
{
  m_numberOfFlowKW++;
}

int MemberDefImpl::numberOfFlowKeyWords() const
{
  return m_numberOfFlowKW;
}

//----------------

QCString MemberDefImpl::displayName(bool) const
{
  return DefinitionMixin::name();
}

//----------------

static void transferArgumentDocumentation(ArgumentList &decAl,ArgumentList &defAl)
{
  for (auto decIt = decAl.begin(), defIt = defAl.begin();
            decIt!= decAl.end() && defIt!= defAl.end();
          ++decIt,               ++defIt)
  {
    Argument &decA = *decIt;
    Argument &defA = *defIt;
    if (decA.docs.isEmpty() && !defA.docs.isEmpty())
    {
      decA.docs = defA.docs;
    }
    else if (defA.docs.isEmpty() && !decA.docs.isEmpty())
    {
      defA.docs = decA.docs;
    }
    if (Config_getBool(RESOLVE_UNNAMED_PARAMS))
    {
      if (decA.name.isEmpty() && !defA.name.isEmpty())
      {
        decA.name = defA.name;
      }
      else if (defA.name.isEmpty() && !decA.name.isEmpty())
      {
        defA.name = decA.name;
      }
    }
  }
}

void combineDeclarationAndDefinition(MemberDefMutable *mdec,MemberDefMutable *mdef)
{
  AUTO_TRACE("mdec='{}' mdef='{}' mdec.isPrototype={} mdef.isPrototype={}",
              mdec->name(), mdef->name(), mdec->isPrototype(), mdef->isPrototype());
  if (
      (mdef->isFunction() && !mdef->isStatic() && !mdef->isPrototype()) ||
      (mdef->isVariable() && !mdef->isExternal() && !mdef->isStatic())
     )
  {
    bool sameNumTemplateArgs = mdef->templateArguments().size()==mdec->templateArguments().size();

    ArgumentList &mdefAl = const_cast<ArgumentList&>(mdef->argumentList());
    ArgumentList &mdecAl = const_cast<ArgumentList&>(mdec->argumentList());
    if (sameNumTemplateArgs &&
        matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),&mdefAl,
                        mdec->getOuterScope(),mdec->getFileDef(),&mdecAl,
                        TRUE,mdef->getLanguage()
                       )
       ) /* match found */
    {
      AUTO_TRACE_ADD("combining definition and declaration");

      if (Config_getBool(RESOLVE_UNNAMED_PARAMS))
      {
        mdec->resolveUnnamedParameters(mdef);
      }

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
        if (mdefAl.hasParameters())
        {
          auto mdefAlComb = stringToArgumentList(mdef->getLanguage(),mdef->argsString());
          transferArgumentDocumentation(mdefAl,*mdefAlComb);
          mdec->moveArgumentList(std::move(mdefAlComb));
        }
      }
      else if (!mdec->documentation().isEmpty())
      {
        //printf("transferring docs mdec->mdef (%s->%s)\n",mdec->argsString(),mdef->argsString());
        mdef->setDocumentation(mdec->documentation(),mdec->docFile(),mdec->docLine());
        mdef->setDocsForDefinition(mdec->isDocsForDefinition());
        if (mdecAl.hasParameters())
        {
          auto mdecAlComb = stringToArgumentList(mdec->getLanguage(),mdec->argsString());
          transferArgumentDocumentation(mdecAl,*mdecAlComb);
          mdef->moveDeclArgumentList(std::move(mdecAlComb));
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
        mdef->setBodySegment(mdec->getDefLine(),mdec->getStartBodyLine(),mdec->getEndBodyLine());
        mdef->setBodyDef(mdec->getBodyDef());
        //mdef->setBodyMember(mdec);
      }
      else if (mdef->getStartBodyLine()!=-1 && mdec->getStartBodyLine()==-1)
      {
        //printf("body mdef->mdec %d-%d\n",mdef->getStartBodyLine(),mdec->getEndBodyLine());
        mdec->setBodySegment(mdef->getDefLine(),mdef->getStartBodyLine(),mdef->getEndBodyLine());
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

      mdef->mergeEnableInlineSource(mdec->hasInlineSource());
      mdec->mergeEnableInlineSource(mdef->hasInlineSource());

      mdef->addQualifiers(mdec->getQualifiers());
      mdec->addQualifiers(mdef->getQualifiers());
    }
  }
}

QCString MemberDefImpl::briefDescription(bool abbr) const
{
  if (m_templateMaster)
  {
    return m_templateMaster->briefDescription(abbr);
  }
  else
  {
    return DefinitionMixin::briefDescription(abbr);
  }
}

QCString MemberDefImpl::documentation() const
{
  if (m_templateMaster)
  {
    return m_templateMaster->documentation();
  }
  else
  {
    return DefinitionMixin::documentation();
  }
}

const ArgumentList &MemberDefImpl::typeConstraints() const
{
  return m_typeConstraints;
}

bool MemberDefImpl::isFriendToHide() const
{
  bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  bool isFriendToHide = hideFriendCompounds &&
     (m_type=="friend class"  ||
      m_type=="friend struct" ||
      m_type=="friend union");
  return isFriendToHide;
}

bool MemberDefImpl::isFriendClass() const
{
  return (isFriend() &&
         (m_type=="friend class" || m_type=="friend struct" ||
          m_type=="friend union"));
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
  return DefinitionMixin::isReference() ||
         (m_templateMaster && m_templateMaster->isReference());
}

CodeSymbolType MemberDefImpl::codeSymbolType() const
{
  switch (memberType())
  {
    case MemberType_Define:      return CodeSymbolType::Define;
    case MemberType_Function:    return CodeSymbolType::Function;
    case MemberType_Variable:    return CodeSymbolType::Variable;
    case MemberType_Typedef:     return CodeSymbolType::Typedef;
    case MemberType_Enumeration: return CodeSymbolType::Enumeration;
    case MemberType_EnumValue:   return CodeSymbolType::EnumValue;
    case MemberType_Signal:      return CodeSymbolType::Signal;
    case MemberType_Slot:        return CodeSymbolType::Slot;
    case MemberType_Friend:      return CodeSymbolType::Friend;
    case MemberType_DCOP:        return CodeSymbolType::DCOP;
    case MemberType_Property:    return CodeSymbolType::Property;
    case MemberType_Event:       return CodeSymbolType::Event;
    case MemberType_Interface:   return CodeSymbolType::Interface;
    case MemberType_Service:     return CodeSymbolType::Service;
    case MemberType_Sequence:    return CodeSymbolType::Sequence;
    case MemberType_Dictionary:  return CodeSymbolType::Dictionary;
  }
  return CodeSymbolType::Default;
}

//-------------------------------------------------------------------------------
// Helpers

static std::mutex g_docCrossReferenceMutex;

void addDocCrossReference(const MemberDef *s,const MemberDef *d)
{
  MemberDefMutable *src = toMemberDefMutable(const_cast<MemberDef*>(s));
  MemberDefMutable *dst = toMemberDefMutable(const_cast<MemberDef*>(d));
  if (src==0 || dst==0) return;
  std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
  //printf("--> addDocCrossReference src=%s,dst=%s\n",qPrint(src->name()),qPrint(dst->name()));
  if (dst->isTypedef() || dst->isEnumerate()) return; // don't add types
  if ((dst->hasReferencedByRelation() || dst->hasCallerGraph()) &&
      src->isCallable()
     )
  {
    dst->addSourceReferencedBy(src);
    MemberDefMutable *mdDef = toMemberDefMutable(dst->memberDefinition());
    if (mdDef)
    {
      mdDef->addSourceReferencedBy(src);
    }
    MemberDefMutable *mdDecl = toMemberDefMutable(dst->memberDeclaration());
    if (mdDecl)
    {
      mdDecl->addSourceReferencedBy(src);
    }
  }
  if ((src->hasReferencesRelation() || src->hasCallGraph()) &&
      src->isCallable()
     )
  {
    src->addSourceReferences(dst);
    MemberDefMutable *mdDef = toMemberDefMutable(src->memberDefinition());
    if (mdDef)
    {
      mdDef->addSourceReferences(dst);
    }
    MemberDefMutable *mdDecl = toMemberDefMutable(src->memberDeclaration());
    if (mdDecl)
    {
      mdDecl->addSourceReferences(dst);
    }
  }
}

// --- Cast functions
//
MemberDef *toMemberDef(Definition *d)
{
  if (d && (typeid(*d)==typeid(MemberDefImpl) || typeid(*d)==typeid(MemberDefAliasImpl)))
  {
    return static_cast<MemberDef*>(d);
  }
  else
  {
    return 0;
  }
}

MemberDef *toMemberDef(DefinitionMutable *md)
{
  Definition *d = toDefinition(md);
  if (d && typeid(*d)==typeid(MemberDefImpl))
  {
    return static_cast<MemberDef*>(d);
  }
  else
  {
    return 0;
  }
}

const MemberDef *toMemberDef(const Definition *d)
{
  if (d && (typeid(*d)==typeid(MemberDefImpl) || typeid(*d)==typeid(MemberDefAliasImpl)))
  {
    return static_cast<const MemberDef*>(d);
  }
  else
  {
    return 0;
  }
}

MemberDefMutable *toMemberDefMutable(Definition *d)
{
  if (d && typeid(*d)==typeid(MemberDefImpl))
  {
    return static_cast<MemberDefMutable*>(d);
  }
  else
  {
    return 0;
  }
}

