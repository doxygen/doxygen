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

class MemberDefImpl final : public DefinitionMixin<MemberDefMutable>
{
  public:
    MemberDefImpl(const DString &defFileName,int defLine,int defColumn,
              const DString &type,const DString &name,const DString &args,
              const DString &excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList &tal,
              const ArgumentList &al,const DString &metaData);
   ~MemberDefImpl() override = default;
    NON_COPYABLE(MemberDefImpl)

    DefType definitionType() const override        { return TypeMember; }
          MemberDef *resolveAlias() override       { return this; }
    const MemberDef *resolveAlias() const override { return this; }
    CodeSymbolType codeSymbolType() const override;
    std::unique_ptr<MemberDef> deepCopy() const override;
    void moveTo(Definition *) override;
    DString getOutputFileBase() const override;
    DString getReference() const override;
    DString anchor() const override;
    DString declaration() const override;
    DString definition() const override;
    DString typeString() const override;
    DString argsString() const override;
    DString excpString() const override;
    DString bitfieldString() const override;
    DString extraTypeChars() const override;
    const DString &initializer() const override;
    int initializerLines() const override;
    TypeSpecifier getMemberSpecifiers() const override;
    VhdlSpecifier getVhdlSpecifiers() const override;
    const MemberList *getSectionList(const Definition *) const override;
    DString displayDefinition() const override;
    const ClassDef *getClassDef() const override;
          ClassDef *getClassDef() override;
    const FileDef  *getFileDef() const override;
          FileDef  *getFileDef() override;
    const NamespaceDef* getNamespaceDef() const override;
          NamespaceDef* getNamespaceDef() override;
    const GroupDef *getGroupDef() const override;
          GroupDef *getGroupDef() override;
    const ModuleDef *getModuleDef() const override;
    DString getReadAccessor() const override;
    DString getWriteAccessor() const override;
    Grouping::GroupPri_t getGroupPri() const override;
    DString getGroupFileName() const override;
    int getGroupStartLine() const override;
    bool getGroupHasDocs() const override;
    DString qualifiedName() const override;
    DString objCMethodName(bool localLink,bool showStatic) const override;
    Protection protection() const override;
    Specifier virtualness(int count=0) const override;
    MemberType memberType() const override;
    DString memberTypeName() const override;
    bool isSignal() const override;
    bool isSlot() const override;
    bool isVariable() const override;
    bool isEnumerate() const override;
    bool isEnumValue() const override;
    bool isTypedef() const override;
    bool isSequence() const override;
    bool isDictionary() const override;
    bool isFunction() const override;
    bool isFunctionPtr() const override;
    bool isDefine() const override;
    bool isFriend() const override;
    bool isDCOP() const override;
    bool isProperty() const override;
    bool isEvent() const override;
    bool isRelated() const override;
    bool isForeign() const override;
    bool isStatic() const override;
    bool isInline() const override;
    bool isExplicit() const override;
    bool isMutable() const override;
    bool isThreadLocal() const override;
    bool isGettable() const override;
    bool isPrivateGettable() const override;
    bool isProtectedGettable() const override;
    bool isSettable() const override;
    bool isPrivateSettable() const override;
    bool isProtectedSettable() const override;
    bool isReadable() const override;
    bool isWritable() const override;
    bool isAddable() const override;
    bool isRemovable() const override;
    bool isRaisable() const override;
    bool isFinal() const override;
    bool isAbstract() const override;
    bool isOverride() const override;
    bool isInitonly() const override;
    bool isOptional() const override;
    bool isRequired() const override;
    bool isNonAtomic() const override;
    bool isCopy() const override;
    bool isAssign() const override;
    bool isRetain() const override;
    bool isWeak() const override;
    bool isStrong() const override;
    bool isEnumStruct() const override;
    bool isUnretained() const override;
    bool isNew() const override;
    bool isSealed() const override;
    bool isImplementation() const override;
    bool isExternal() const override;
    bool isTypeAlias() const override;
    bool isDefault() const override;
    bool isDelete() const override;
    bool isNoExcept() const override;
    bool isAttribute() const override;
    bool isUNOProperty() const override;
    bool isReadonly() const override;
    bool isBound() const override;
    bool isConstrained() const override;
    bool isTransient() const override;
    bool isMaybeVoid() const override;
    bool isMaybeDefault() const override;
    bool isMaybeAmbiguous() const override;
    bool isPublished() const override;
    bool isTemplateSpecialization() const override;
    bool isObjCMethod() const override;
    bool isObjCProperty() const override;
    bool isCSharpProperty() const override;
    bool isConstructor() const override;
    bool isDestructor() const override;
    bool hasOneLineInitializer() const override;
    bool hasMultiLineInitializer() const override;
    bool isCallable() const override;
    bool isStrongEnumValue() const override;
    bool livesInsideEnum() const override;
    bool isSliceLocal() const override;
    bool isNoDiscard() const override;
    bool isConstExpr() const override;
    bool isConstEval() const override;
    bool isConstInit() const override;
    int  numberOfFlowKeyWords() const override;
    bool isFriendToHide() const override;
    bool isNotFriend() const override;
    bool isFunctionOrSignalSlot() const override;
    bool isRelatedOrFriend() const override;
    bool isLinkableInProject() const override;
    bool isLinkable() const override;
    bool hasDocumentation() const override;
    bool hasUserDocumentation() const override;
    bool isDeleted() const override;
    bool isBriefSectionVisible() const override;
    bool isDetailedSectionVisible(MemberListContainer container) const override;
    bool hasDetailedDescription() const override;
    bool isFriendClass() const override;
    bool isDocumentedFriendClass() const override;
    const MemberDef *reimplements() const override;
    const MemberVector &reimplementedBy() const override;
    bool isReimplementedBy(const ClassDef *cd) const override;
    ClassDef *relatedAlso() const override;
    bool hasDocumentedEnumValues() const override;
    const MemberDef *getAnonymousEnumType() const override;
    bool isDocsForDefinition() const override;
    const MemberDef *getEnumScope() const override;
    const MemberVector &enumFieldList() const override;
    void setEnumBaseType(const DString &type) override;
    DString enumBaseType() const override;
    bool hasExamples() const override;
    const ExampleList &getExamples() const override;
    bool isPrototype() const override;
    const ArgumentList &argumentList() const override;
    const ArgumentList &declArgumentList() const override;
    const ArgumentList &templateArguments() const override;
    const ArgumentLists &definitionTemplateParameterLists() const override;
    std::optional<ArgumentList> formalTemplateArguments() const override;
    int getMemberGroupId() const override;
    MemberGroup *getMemberGroup() const override;
    bool hasCallGraph() const override;
    bool hasCallerGraph() const override;
    bool hasReferencesRelation() const override;
    bool hasReferencedByRelation() const override;
    bool hasEnumValues() const override;
    bool hasInlineSource() const override;
    bool isDocTransferDone() const override;
    DString sourceRefName() const override;
    const MemberDef *templateMaster() const override;
    DString getScopeString() const override;
    const ClassDef *getClassDefOfAnonymousType() const override;
    bool isTypedefValCached() const override;
    const ClassDef *getCachedTypedefVal() const override;
    DString getCachedTypedefTemplSpec() const override;
    DString getCachedResolvedTypedef() const override;
    MemberDef *memberDefinition() const override;
    MemberDef *memberDeclaration() const override;
    const MemberDef *inheritsDocsFrom() const override;
    const MemberDef *getGroupAlias() const override;
    ClassDef *category() const override;
    const MemberDef *categoryRelation() const override;
    DString displayName(bool=true) const override;
    DString getDeclType() const override;
    StringVector getLabels(const Definition *container) const override;
    const ArgumentList &typeConstraints() const override;
    DString requiresClause() const override;
    DString documentation() const override;
    DString briefDescription(bool abbr=false) const override;
    DString fieldType() const override;
    bool isReference() const override;
    DString getDeclFileName() const override;
    int getDeclLine() const override;
    int getDeclColumn() const override;
    void setMemberType(MemberType t) override;
    void setDefinition(const DString &d) override;
    void setFileDef(FileDef *fd) override;
    void setAnchor() override;
    void setProtection(Protection p) override;
    void setMemberSpecifiers(TypeSpecifier s) override;
    void setVhdlSpecifiers(VhdlSpecifier s) override;
    void mergeMemberSpecifiers(TypeSpecifier s) override;
    void setInitializer(const DString &i) override;
    void setBitfields(const DString &s) override;
    void setMaxInitLines(int lines) override;
    void setMemberClass(ClassDef *cd) override;
    void setSectionList(const Definition *container,const MemberList *sl) override;
    void setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                     const DString &fileName,int startLine,bool hasDocs,
                     MemberDef *member=nullptr) override;
    void setReadAccessor(const DString &r) override;
    void setWriteAccessor(const DString &w) override;
    void setTemplateSpecialization(bool b) override;
    void makeRelated() override;
    void makeForeign() override;
    void setInheritsDocsFrom(const MemberDef *md) override;
    void setTagInfo(const TagInfo *i) override;
    void setArgsString(const DString &as) override;
    void setReimplements(MemberDef *md) override;
    void insertReimplementedBy(MemberDef *md) override;
    void setRelatedAlso(ClassDef *cd) override;
    void insertEnumField(MemberDef *md) override;
    void setEnumScope(MemberDef *md,bool livesInsideEnum=false) override;
    void setEnumClassScope(ClassDef *cd) override;
    void setDocumentedEnumValues(bool value) override;
    void setAnonymousEnumType(const MemberDef *md) override;
    bool addExample(const DString &anchor,const DString &name,const DString &file) override;
    void setPrototype(bool p,const DString &df,int line, int column) override;
    void setExplicitExternal(bool b,const DString &df,int line,int column) override;
    void setDeclFile(const DString &df,int line,int column) override;
    void moveArgumentList(std::unique_ptr<ArgumentList> al) override;
    void moveDeclArgumentList(std::unique_ptr<ArgumentList> al) override;
    void setDefinitionTemplateParameterLists(const ArgumentLists &lists) override;
    void setTypeConstraints(const ArgumentList &al) override;
    void setType(const DString &t) override;
    void setNamespace(NamespaceDef *nd) override;
    void setMemberGroup(MemberGroup *grp) override;
    void setMemberGroupId(int id) override;
    void makeImplementationDetail() override;
    void overrideCallGraph(bool e) override;
    void overrideCallerGraph(bool e) override;
    void overrideReferencedByRelation(bool e) override;
    void overrideReferencesRelation(bool e) override;
    void overrideEnumValues(bool e) override;
    void overrideInlineSource(bool e) override;
    void setTemplateMaster(const MemberDef *mt) override;
    void setFormalTemplateArguments(const ArgumentList &al) override;
    void addListReference(const Definition *) override;
    void addRequirementReferences(const Definition *) override;
    void setDocsForDefinition(bool b) override;
    void setGroupAlias(const MemberDef *md) override;
    void cacheTypedefVal(const ClassDef *val,const DString &templSpec,const DString &resolvedType) override;
    void invalidateTypedefValCache() override;
    void invalidateCachedArgumentTypes() override;
    void setMemberDefinition(MemberDef *md) override;
    void setMemberDeclaration(MemberDef *md) override;
    void copyArgumentNames(const MemberDef *bmd) override;
    void setCategory(ClassDef *) override;
    void setCategoryRelation(const MemberDef *) override;
    void setDocumentation(const DString &d,const DString &docFile,int docLine,bool stripWhiteSpace=true) override;
    void setBriefDescription(const DString &b,const DString &briefFile,int briefLine) override;
    void setInbodyDocumentation(const DString &d,const DString &inbodyFile,int inbodyLine) override;
    void setHidden(bool b) override;
    void setDocTransferDone() override;
    void setExplicitInherited(bool b) override;
    void setRequiresClause(const DString &req) override;
    void incrementFlowKeyWordCount() override;
    void writeDeclaration(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool inGroup, int indentLevel,const ClassDef *inheritFrom=nullptr,const DString &inheritId=DString()) const override;
    void writeDocumentation(const MemberList *ml,int memCount,int memTotal,OutputList &ol,
                            const DString &scopeName,const Definition *container,
                            bool inGroup,bool showEnumValues=false,bool
                            showInline=false) const override;
    void writeMemberDocSimple(OutputList &ol,const Definition *container) const override;
    void writeEnumDeclaration(OutputList &typeDecl,
            const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod) const override;
    void writeTagFile(TextStream &,bool useQualifiedName,bool showNamespaceMembers) const override;
    void warnIfUndocumented() const override;
    void warnIfUndocumentedParams() const override;
    bool visibleInIndex() const override;
    void detectUndocumentedParams(bool hasParamCommand,bool hasReturnCommand) const override;
    std::unique_ptr<MemberDef> createTemplateInstanceMember(const ArgumentList &formalArgs,
               const std::unique_ptr<ArgumentList> &actualArgs) const override;
    void findSectionsInDocumentation() override;
    void writeLink(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool onlyText=false) const override;
    void resolveUnnamedParameters(const MemberDef *md) override;
    void addQualifiers(const StringVector &qualifiers) override;
    StringVector getQualifiers() const override;
    ClassDefMutable *getClassDefMutable() override;
    void setModuleDef(ModuleDef *mod) override;
    int redefineCount() const override;
    void setRedefineCount(int) override;
    void setClassDefOfAnonymousType(const ClassDef *cd) override;

  private:
    void _computeLinkableInProject();
    void _computeIsConstructor();
    void _computeIsDestructor();
    void _writeGroupInclude(OutputList &ol,bool inGroup) const;
    void _writeMultiLineInitializer(OutputList &ol,const DString &scopeName) const;
    void _writeCallGraph(OutputList &ol) const;
    void _writeCallerGraph(OutputList &ol) const;
    void _writeReimplements(OutputList &ol) const;
    bool _isReimplements() const;
    void _writeReimplementedBy(OutputList &ol) const;
    size_t _countReimplementedBy() const;
    void _writeExamples(OutputList &ol) const;
    void _writeTypeConstraints(OutputList &ol) const;
    void _writeEnumValues(OutputList &ol,const Definition *container,
                          const DString &cfname,const DString &ciname,
                          const DString &cname) const;
    void _writeCategoryRelation(OutputList &ol) const;
    void _writeTagData(const DefType) const;
    void _writeTemplatePrefix(OutputList &ol, const Definition *def,
                              const ArgumentList &al, bool writeReqClause=true) const;
    bool _hasVisibleCallGraph() const;
    bool _hasVisibleCallerGraph() const;
    bool _isAnonymousBitField() const;


    void init(Definition *def,const DString &t,const DString &a,const DString &e,
              Protection p,Specifier v,bool s,Relationship r,
              MemberType mt,const ArgumentList &tal,
              const ArgumentList &al,const DString &meta
             );

    uint8_t m_isLinkableCached;    // 0 = not cached, 1=false, 2=true
    uint8_t m_isConstructorCached; // 0 = not cached, 1=false, 2=true
    uint8_t m_isDestructorCached;  // 1 = not cached, 1=false, 2=true

    ClassDef     *m_classDef = nullptr; // member of or related to
    FileDef      *m_fileDef  = nullptr; // member of file definition
    NamespaceDef *m_nspace   = nullptr; // the namespace this member is in.
    ModuleDef    *m_moduleDef = nullptr;

    const MemberDef  *m_enumScope = nullptr;    // the enclosing scope, if this is an enum field
    bool        m_livesInsideEnum = false;
    const MemberDef  *m_annEnumType = nullptr;  // the anonymous enum that is the type of this member
    MemberVector m_enumFields;              // enumeration fields

    MemberDef  *m_redefines = nullptr;    // the members that this member redefines
    MemberVector m_redefinedBy;             // the list of members that redefine this one

    MemberDef  *m_memDef = nullptr;       // member definition for this declaration
    MemberDef  *m_memDec = nullptr;       // member declaration for this definition
    ClassDef   *m_relatedAlso = nullptr;  // points to class marked by relatedAlso

    ExampleList m_examples;     // a dictionary of all examples for quick access

    DString m_type;            // return actual type
    DString m_args;            // function arguments/variable array specifiers
    DString m_def;             // member definition in code (fully qualified name)
    DString m_anc;             // HTML anchor name
    Specifier m_virt = Specifier::Normal;  // normal/virtual/pure virtual
    Protection m_prot = Protection::Public; // protection type [Public/Protected/Private]
    DString m_decl;            // member declaration in class

    DString m_bitfields;       // struct member bitfields
    DString m_read;            // property read accessor
    DString m_write;           // property write accessor
    DString m_exception;       // exceptions that can be thrown
    DString m_initializer;     // initializer
    DString m_extraTypeChars;  // extra type info found after the argument list
    DString m_enumBaseType;    // base type of the enum (C++11)
    DString m_requiresClause;  // requires clause (C++20)
    int m_initLines = 0;            // number of lines in the initializer
    bool m_docTransferDone = false;

    TypeSpecifier m_memSpec;          // The specifiers present for this member
    VhdlSpecifier m_vhdlSpec;
    MemberType m_mtype = MemberType::Define; // returns the kind of member
    int m_maxInitLines = 0;         // when the initializer will be displayed
    int m_userInitLines = 0;        // result of explicit \hideinitializer or \showinitializer

    ArgumentList m_defArgList;    // argument list of this member definition
    ArgumentList m_declArgList;   // argument list of this member declaration

    ArgumentList m_tArgList;      // template argument list of function template
    ArgumentList m_typeConstraints; // type constraints for template parameters
    const MemberDef *m_templateMaster = nullptr;
    std::optional<ArgumentList> m_formalTemplateArguments;
    ArgumentLists m_defTmpArgLists; // lists of template argument lists
                                         // (for template functions in nested template classes)

    DString m_metaData;        // Slice metadata.

    const ClassDef *m_anonymousType = nullptr; // if the member has an anonymous compound
                                   // as its type then this is computed by
                                   // getClassDefOfAnonymousType() and
                                   // cached here.
    std::map<const Definition *,const MemberList *> m_sectionMap;

    const MemberDef *m_groupAlias = nullptr;    // Member containing the definition
    int m_grpId = 0;                // group id
    MemberGroup *m_memberGroup = nullptr; // group's member definition
    GroupDef *m_group = nullptr;          // group in which this member is in
    Grouping::GroupPri_t m_grouppri = Grouping::GROUPING_AUTO_DEF; // priority of this definition
    DString m_groupFileName;   // file where this grouping was defined
    int m_groupStartLine = 0;       // line  "      "      "     "     "
    MemberDef *m_groupMember = nullptr;

    bool m_isTypedefValCached = false;
    const ClassDef *m_cachedTypedefValue = nullptr;
    DString m_cachedTypedefTemplSpec;
    DString m_cachedResolvedType;

    // documentation inheritance
    const MemberDef *m_docProvider = nullptr;

    // to store the output file base from tag files
    DString m_explicitOutputFileBase;
    bool m_explicitInherited = false;

    // to store extra qualifiers
    StringVector m_qualifiers;

    // objective-c
    bool m_implOnly = false; // function found in implementation but not
                           // in the interface
    mutable bool m_hasDocumentedParams = false;      // guard to show only the first warning, acts as cache
    mutable bool m_hasDocumentedReturnType = false;  // guard to show only the first warning, acts as cache
    bool m_isDMember = false;
    Relationship m_related = Relationship::Member;    // relationship of this to the class
    bool m_stat = false;                // is it a static function?
    bool m_proto = false;               // is it a prototype?
    bool m_docEnumValues = false;       // is an enum with documented enum values.

    mutable bool m_hasDetailedDescriptionCached = false;
    mutable bool m_detailedDescriptionCachedValue = false;
                                      // const member.
    bool m_hasCallGraph = false;
    bool m_hasCallerGraph = false;
    bool m_hasReferencedByRelation = false;
    bool m_hasReferencesRelation = false;
    bool m_hasInlineSource = false;
    bool m_hasEnumValues = false;
    bool m_explExt = false;             // member was explicitly declared external
    bool m_tspec = false;               // member is a template specialization
    bool m_groupHasDocs = false;        // true if the entry that caused the grouping was documented
    bool m_docsForDefinition = false;   // true => documentation block is put before
                                      //         definition.
                                      // false => block is put before declaration.
    ClassDef *m_category = nullptr;
    const MemberDef *m_categoryRelation = nullptr;
    DString m_declFileName;
    int m_declLine = -1;
    int m_declColumn = -1;
    int m_numberOfFlowKW = 0;
    int m_redefineCount = 0;
};

std::unique_ptr<MemberDef> createMemberDef(const DString &defFileName,int defLine,int defColumn,
              const DString &type,const DString &name,const DString &args,
              const DString &excp,Protection prot,Specifier virt,bool stat,
              Relationship related,MemberType t,const ArgumentList &tal,
              const ArgumentList &al,const DString &metaData)
{
  return std::make_unique<MemberDefImpl>(defFileName,defLine,defColumn,type,name,args,excp,prot,virt,
                           stat,related,t,tal,al,metaData);
}

//-----------------------------------------------------------------------------

class MemberDefAliasImpl final : public DefinitionAliasMixin<MemberDef>
{
  public:
    MemberDefAliasImpl(const Definition *newScope,const MemberDef *md)
    : DefinitionAliasMixin(newScope,md), m_memberGroup(nullptr) { init(); }
    ~MemberDefAliasImpl() override { deinit(); }
    NON_COPYABLE(MemberDefAliasImpl)

    DefType definitionType() const override { return TypeMember; }

    const MemberDef *getMdAlias() const            { return toMemberDef(getAlias()); }
          MemberDef *getMdAlias()                  { return toMemberDef(const_cast<Definition*>(getAlias())); }
          MemberDef *resolveAlias() override       { return const_cast<MemberDef*>(getMdAlias()); }
    const MemberDef *resolveAlias() const override { return getMdAlias(); }

    std::unique_ptr<MemberDef> deepCopy() const override  {
      return createMemberDefAlias(getScope(),getMdAlias());
    }
    void moveTo(Definition *) override {}

    const DString &name() const override
    { return getMdAlias()->name(); }
    CodeSymbolType codeSymbolType() const override
    { return getMdAlias()->codeSymbolType(); }
    DString getOutputFileBase() const override
    { return getMdAlias()->getOutputFileBase(); }
    DString getReference() const override
    { return getMdAlias()->getReference(); }
    DString anchor() const override
    { return getMdAlias()->anchor(); }
    DString declaration() const override
    { return getMdAlias()->declaration(); }
    DString definition() const override
    { return getMdAlias()->definition(); }
    DString typeString() const override
    { return getMdAlias()->typeString(); }
    DString argsString() const override
    { return getMdAlias()->argsString(); }
    DString excpString() const override
    { return getMdAlias()->excpString(); }
    DString bitfieldString() const override
    { return getMdAlias()->bitfieldString(); }
    DString extraTypeChars() const override
    { return getMdAlias()->extraTypeChars(); }
    const DString &initializer() const override
    { return getMdAlias()->initializer(); }
    int initializerLines() const override
    { return getMdAlias()->initializerLines(); }
    TypeSpecifier getMemberSpecifiers() const override
    { return getMdAlias()->getMemberSpecifiers(); }
    VhdlSpecifier getVhdlSpecifiers() const override
    { return getMdAlias()->getVhdlSpecifiers(); }
    const MemberList *getSectionList(const Definition *container) const override
    { return getMdAlias()->getSectionList(container); }
    DString displayDefinition() const override
    { return getMdAlias()->displayDefinition(); }

    const ClassDef *getClassDef() const override
    { return getMdAlias()->getClassDef(); }
          ClassDef *getClassDef() override
    { return getMdAlias()->getClassDef(); }

    const FileDef *getFileDef() const override
    { return getMdAlias()->getFileDef(); }
          FileDef *getFileDef() override
    { return getMdAlias()->getFileDef(); }

    const ModuleDef *getModuleDef() const override
    { return getMdAlias()->getModuleDef(); }

    const NamespaceDef* getNamespaceDef() const override
    { return getMdAlias()->getNamespaceDef(); }
          NamespaceDef* getNamespaceDef() override
    { return getMdAlias()->getNamespaceDef(); }

    DString getReadAccessor() const override
    { return getMdAlias()->getReadAccessor(); }
    DString getWriteAccessor() const override
    { return getMdAlias()->getWriteAccessor(); }
    const GroupDef *getGroupDef() const override
    { return getMdAlias()->getGroupDef(); }
    GroupDef *getGroupDef() override
    { return getMdAlias()->getGroupDef(); }
    Grouping::GroupPri_t getGroupPri() const override
    { return getMdAlias()->getGroupPri(); }
    DString getGroupFileName() const override
    { return getMdAlias()->getGroupFileName(); }
    int getGroupStartLine() const override
    { return getMdAlias()->getGroupStartLine(); }
    bool getGroupHasDocs() const override
    { return getMdAlias()->getGroupHasDocs(); }
    DString qualifiedName() const override
    { return getMdAlias()->qualifiedName(); }
    DString objCMethodName(bool localLink,bool showStatic) const override
    { return getMdAlias()->objCMethodName(localLink,showStatic); }
    Protection protection() const override
    { return getMdAlias()->protection(); }
    Specifier virtualness(int /* count */) const override
    { return getMdAlias()->virtualness(); }
    MemberType memberType() const override
    { return getMdAlias()->memberType(); }
    DString memberTypeName() const override
    { return getMdAlias()->memberTypeName(); }
    bool isSignal() const override
    { return getMdAlias()->isSignal(); }
    bool isSlot() const override
    { return getMdAlias()->isSlot(); }
    bool isVariable() const override
    { return getMdAlias()->isVariable(); }
    bool isEnumerate() const override
    { return getMdAlias()->isEnumerate(); }
    bool isEnumValue() const override
    { return getMdAlias()->isEnumValue(); }
    bool isTypedef() const override
    { return getMdAlias()->isTypedef(); }
    bool isSequence() const override
    { return getMdAlias()->isSequence(); }
    bool isDictionary() const override
    { return getMdAlias()->isDictionary(); }
    bool isFunction() const override
    { return getMdAlias()->isFunction(); }
    bool isFunctionPtr() const override
    { return getMdAlias()->isFunctionPtr(); }
    bool isDefine() const override
    { return getMdAlias()->isDefine(); }
    bool isFriend() const override
    { return getMdAlias()->isFriend(); }
    bool isDCOP() const override
    { return getMdAlias()->isDCOP(); }
    bool isProperty() const override
    { return getMdAlias()->isProperty(); }
    bool isEvent() const override
    { return getMdAlias()->isEvent(); }
    bool isRelated() const override
    { return getMdAlias()->isRelated(); }
    bool isForeign() const override
    { return getMdAlias()->isForeign(); }
    bool isStatic() const override
    { return getMdAlias()->isStatic(); }
    bool isInline() const override
    { return getMdAlias()->isInline(); }
    bool isExplicit() const override
    { return getMdAlias()->isExplicit(); }
    bool isMutable() const override
    { return getMdAlias()->isMutable(); }
    bool isThreadLocal() const override
    { return getMdAlias()->isThreadLocal(); }
    bool isGettable() const override
    { return getMdAlias()->isGettable(); }
    bool isPrivateGettable() const override
    { return getMdAlias()->isPrivateGettable(); }
    bool isProtectedGettable() const override
    { return getMdAlias()->isProtectedGettable(); }
    bool isSettable() const override
    { return getMdAlias()->isSettable(); }
    bool isPrivateSettable() const override
    { return getMdAlias()->isPrivateSettable(); }
    bool isProtectedSettable() const override
    { return getMdAlias()->isProtectedSettable(); }
    bool isReadable() const override
    { return getMdAlias()->isReadable(); }
    bool isWritable() const override
    { return getMdAlias()->isWritable(); }
    bool isAddable() const override
    { return getMdAlias()->isAddable(); }
    bool isRemovable() const override
    { return getMdAlias()->isRemovable(); }
    bool isRaisable() const override
    { return getMdAlias()->isRaisable(); }
    bool isFinal() const override
    { return getMdAlias()->isFinal(); }
    bool isAbstract() const override
    { return getMdAlias()->isAbstract(); }
    bool isOverride() const override
    { return getMdAlias()->isOverride(); }
    bool isInitonly() const override
    { return getMdAlias()->isInitonly(); }
    bool isOptional() const override
    { return getMdAlias()->isOptional(); }
    bool isRequired() const override
    { return getMdAlias()->isRequired(); }
    bool isNonAtomic() const override
    { return getMdAlias()->isNonAtomic(); }
    bool isCopy() const override
    { return getMdAlias()->isCopy(); }
    bool isAssign() const override
    { return getMdAlias()->isAssign(); }
    bool isRetain() const override
    { return getMdAlias()->isRetain(); }
    bool isWeak() const override
    { return getMdAlias()->isWeak(); }
    bool isStrong() const override
    { return getMdAlias()->isStrong(); }
    bool isEnumStruct() const override
    { return getMdAlias()->isEnumStruct(); }
    bool isUnretained() const override
    { return getMdAlias()->isUnretained(); }
    bool isNew() const override
    { return getMdAlias()->isNew(); }
    bool isSealed() const override
    { return getMdAlias()->isSealed(); }
    bool isImplementation() const override
    { return getMdAlias()->isImplementation(); }
    bool isExternal() const override
    { return getMdAlias()->isExternal(); }
    bool isTypeAlias() const override
    { return getMdAlias()->isTypeAlias(); }
    bool isDefault() const override
    { return getMdAlias()->isDefault(); }
    bool isDelete() const override
    { return getMdAlias()->isDelete(); }
    bool isNoExcept() const override
    { return getMdAlias()->isNoExcept(); }
    bool isAttribute() const override
    { return getMdAlias()->isAttribute(); }
    bool isUNOProperty() const override
    { return getMdAlias()->isUNOProperty(); }
    bool isReadonly() const override
    { return getMdAlias()->isReadable(); }
    bool isBound() const override
    { return getMdAlias()->isBound(); }
    bool isConstrained() const override
    { return getMdAlias()->isConstrained(); }
    bool isTransient() const override
    { return getMdAlias()->isTransient(); }
    bool isMaybeVoid() const override
    { return getMdAlias()->isMaybeVoid(); }
    bool isMaybeDefault() const override
    { return getMdAlias()->isMaybeDefault(); }
    bool isMaybeAmbiguous() const override
    { return getMdAlias()->isMaybeAmbiguous(); }
    bool isPublished() const override
    { return getMdAlias()->isPublished(); }
    bool isTemplateSpecialization() const override
    { return getMdAlias()->isTemplateSpecialization(); }
    bool isObjCMethod() const override
    { return getMdAlias()->isObjCMethod(); }
    bool isObjCProperty() const override
    { return getMdAlias()->isObjCProperty(); }
    bool isCSharpProperty() const override
    { return getMdAlias()->isCSharpProperty(); }
    bool isConstructor() const override
    { return getMdAlias()->isConstructor(); }
    bool isDestructor() const override
    { return getMdAlias()->isDestructor(); }
    bool hasOneLineInitializer() const override
    { return getMdAlias()->hasOneLineInitializer(); }
    bool hasMultiLineInitializer() const override
    { return getMdAlias()->hasMultiLineInitializer(); }
    bool isCallable() const override
    { return getMdAlias()->isCallable(); }
    bool isStrongEnumValue() const override
    { return getMdAlias()->isStrongEnumValue(); }
    bool livesInsideEnum() const override
    { return getMdAlias()->livesInsideEnum(); }
    bool isSliceLocal() const override
    { return getMdAlias()->isSliceLocal(); }
    bool isNoDiscard() const override
    { return getMdAlias()->isNoDiscard(); }
    bool isConstExpr() const override
    { return getMdAlias()->isConstExpr(); }
    bool isConstEval() const override
    { return getMdAlias()->isConstEval(); }
    bool isConstInit() const override
    { return getMdAlias()->isConstInit(); }
    int  numberOfFlowKeyWords() const override
    { return getMdAlias()->numberOfFlowKeyWords(); }
    bool isFriendToHide() const override
    { return getMdAlias()->isFriendToHide(); }
    bool isNotFriend() const override
    { return getMdAlias()->isNotFriend(); }
    bool isFunctionOrSignalSlot() const override
    { return getMdAlias()->isFunctionOrSignalSlot(); }
    bool isRelatedOrFriend() const override
    { return getMdAlias()->isRelatedOrFriend(); }
    bool isLinkableInProject() const override
    { return getMdAlias()->isLinkableInProject(); }
    bool isLinkable() const override
    { return getMdAlias()->isLinkable(); }
    bool hasDocumentation() const override
    { return getMdAlias()->hasDocumentation(); }
    bool hasUserDocumentation() const override
    { return getMdAlias()->hasUserDocumentation(); }
    bool isDeleted() const override
    { return getMdAlias()->isDeleted(); }
    bool isBriefSectionVisible() const override
    { return getMdAlias()->isBriefSectionVisible(); }
    bool isDetailedSectionVisible(MemberListContainer container) const override
    { return getMdAlias()->isDetailedSectionVisible(container); }
    bool hasDetailedDescription() const override
    { return getMdAlias()->hasDetailedDescription(); }
    bool isFriendClass() const override
    { return getMdAlias()->isFriendClass(); }
    bool isDocumentedFriendClass() const override
    { return getMdAlias()->isDocumentedFriendClass(); }
    const MemberDef *reimplements() const override
    { return getMdAlias()->reimplements(); }
    const MemberVector &reimplementedBy() const override
    { return getMdAlias()->reimplementedBy(); }
    bool isReimplementedBy(const ClassDef *cd) const override
    { return getMdAlias()->isReimplementedBy(cd); }
    ClassDef *relatedAlso() const override
    { return getMdAlias()->relatedAlso(); }
    bool hasDocumentedEnumValues() const override
    { return getMdAlias()->hasDocumentedEnumValues(); }
    const MemberDef *getAnonymousEnumType() const override
    { return getMdAlias()->getAnonymousEnumType(); }
    bool isDocsForDefinition() const override
    { return getMdAlias()->isDocsForDefinition(); }
    const MemberDef *getEnumScope() const override
    { return getMdAlias()->getEnumScope(); }
    const MemberVector &enumFieldList() const override
    { return getMdAlias()->enumFieldList(); }
    DString enumBaseType() const override
    { return getMdAlias()->enumBaseType(); }
    bool hasExamples() const override
    { return getMdAlias()->hasExamples(); }
    const ExampleList &getExamples() const override
    { return getMdAlias()->getExamples(); }
    bool isPrototype() const override
    { return getMdAlias()->isPrototype(); }
    const ArgumentList &argumentList() const override
    { return getMdAlias()->argumentList(); }
    const ArgumentList &declArgumentList() const override
    { return getMdAlias()->declArgumentList(); }
    const ArgumentList &templateArguments() const override
    { return getMdAlias()->templateArguments(); }
    const ArgumentLists &definitionTemplateParameterLists() const override
    { return getMdAlias()->definitionTemplateParameterLists(); }
    std::optional<ArgumentList> formalTemplateArguments() const override
    { return getMdAlias()->formalTemplateArguments(); }
    int getMemberGroupId() const override
    { return getMdAlias()->getMemberGroupId(); }
    MemberGroup *getMemberGroup() const override
    { return m_memberGroup; }
    bool hasCallGraph() const override
    { return getMdAlias()->hasCallGraph(); }
    bool hasCallerGraph() const override
    { return getMdAlias()->hasCallerGraph(); }
    bool hasReferencesRelation() const override
    { return getMdAlias()->hasReferencesRelation(); }
    bool hasReferencedByRelation() const override
    { return getMdAlias()->hasReferencedByRelation(); }
    bool hasInlineSource() const override
    { return getMdAlias()->hasInlineSource(); }
    bool isDocTransferDone() const override
    { return getMdAlias()->isDocTransferDone(); }
    DString sourceRefName() const override
    { return getMdAlias()->sourceRefName(); }
    bool hasEnumValues() const override
    { return getMdAlias()->hasEnumValues(); }
    StringVector getQualifiers() const override
    { return getMdAlias()->getQualifiers(); }
    const MemberDef *templateMaster() const override
    { return getMdAlias()->templateMaster(); }
    DString getScopeString() const override
    { return getMdAlias()->getScopeString(); }
    const ClassDef *getClassDefOfAnonymousType() const override
    { return getMdAlias()->getClassDefOfAnonymousType(); }
    bool isTypedefValCached() const override
    { return getMdAlias()->isTypedefValCached(); }
    const ClassDef *getCachedTypedefVal() const override
    { return getMdAlias()->getCachedTypedefVal(); }
    DString getCachedTypedefTemplSpec() const override
    { return getMdAlias()->getCachedTypedefTemplSpec(); }
    DString getCachedResolvedTypedef() const override
    { return getMdAlias()->getCachedResolvedTypedef(); }
    MemberDef *memberDefinition() const override
    { return getMdAlias()->memberDefinition(); }
    MemberDef *memberDeclaration() const override
    { return getMdAlias()->memberDeclaration(); }
    const MemberDef *inheritsDocsFrom() const override
    { return getMdAlias()->inheritsDocsFrom(); }
    const MemberDef *getGroupAlias() const override
    { return getMdAlias()->getGroupAlias(); }
    ClassDef *category() const override
    { return getMdAlias()->category(); }
    const MemberDef *categoryRelation() const override
    { return getMdAlias()->categoryRelation(); }
    DString displayName(bool b=true) const override
    { return getMdAlias()->displayName(b); }
    DString getDeclType() const override
    { return getMdAlias()->getDeclType(); }
    StringVector getLabels(const Definition *container) const override
    { return getMdAlias()->getLabels(container); }
    const ArgumentList &typeConstraints() const override
    { return getMdAlias()->typeConstraints(); }
    DString documentation() const override
    { return getMdAlias()->documentation(); }
    DString briefDescription(bool /* abbr=false */) const override
    { return getMdAlias()->briefDescription(); }
    DString fieldType() const override
    { return getMdAlias()->fieldType(); }
    bool isReference() const override
    { return getMdAlias()->isReference(); }
    DString getDeclFileName() const override
    { return getMdAlias()->getDeclFileName(); }
    int getDeclLine() const override
    { return getMdAlias()->getDeclLine(); }
    int getDeclColumn() const override
    { return getMdAlias()->getDeclColumn(); }
    DString requiresClause() const override
    { return getMdAlias()->requiresClause(); }
    bool visibleInIndex() const override
    { return getMdAlias()->visibleInIndex(); }
    int redefineCount() const override
    { return getMdAlias()->redefineCount(); }

    void warnIfUndocumented() const override {}
    void warnIfUndocumentedParams() const override {}
    void detectUndocumentedParams(bool /* hasParamCommand */,bool /* hasReturnCommand */) const override {}
    void setMemberGroup(MemberGroup *grp) override { m_memberGroup = grp; }
    std::unique_ptr<MemberDef> createTemplateInstanceMember(const ArgumentList &formalArgs,
               const std::unique_ptr<ArgumentList> &actualArgs) const override
    { return getMdAlias()->createTemplateInstanceMember(formalArgs,actualArgs); }

    void writeDeclaration(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool inGroup, int indentLevel, const ClassDef *inheritFrom=nullptr,const DString &inheritId=DString()) const override
    {
      getMdAlias()->writeDeclaration(ol,cd,nd,fd,gd,mod,inGroup,indentLevel,inheritFrom,inheritId);
    }
    void writeEnumDeclaration(OutputList &typeDecl,
            const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod) const override
    {
      getMdAlias()->writeEnumDeclaration(typeDecl,cd,nd,fd,gd,mod);
    }
    void writeLink(OutputList &ol,
                   const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
                   bool onlyText=false) const override
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

static DString addTemplateNames(const DString &s,const DString &n,const DString &t)
{
  DString result;
  DString clRealName=n;
  size_t p=0,i=0;
  if ((i=clRealName.find('<'))!=DString::npos)
  {
    clRealName=clRealName.left(i); // strip template specialization
  }
  if ((i=clRealName.rfind("::"))!=DString::npos)
  {
    clRealName=clRealName.mid(i+2);
  }
  while ((i=s.find(clRealName,p))!=DString::npos)
  {
    result+=s.mid(p,i-p);
    size_t j=clRealName.length()+i;
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
  result+=s.mid(p);
  //printf("addTemplateNames(%s,%s,%s)=%s\n",qPrint(s),qPrint(n),qPrint(t),qPrint(result));
  return result;
}

// ol.startMemberDocName has already been done before this is called.
// when this function returns true, ol.endParameterList will be called.
//
// typical sequence:
//   ol.startMemberDoc
//   ol.startMemberDocName
//   --- enter writeDefArgumentList
//   ol.endMemberDocName
//   ol.startParameterList
//     ...
//     ol.startParameterType(first=true)
//     ol.endParameterType
//     ol.startParameterName
//     ol.endParameterName
//     ol.startParameterExtra
//     ol.startParameterDefVal       [optional]
//     ol.endParameterDefVal         [optional]
//     ol.endParameterExtra(last==false)
//     ...
//     ol.startParameterType(first=false)
//     ol.endParameterType
//     ol.startParameterName
//     ol.endParameterName
//     ol.startParameterExtra
//     ol.endParameterExtra(last==true)
//     ...
//   --- leave writeDefArgumentList with return value true
//   ol.endParameterList
//   ol.endMemberDoc(hasArgs=true)
//
//  For an empty list the function should return false, the sequence is
//   ol.startMemberDoc
//   ol.startMemberDocName
//   --- enter writeDefArgumentList
//   --- leave writeDefArgumentList with return value false
//   ol.endMemberDocName
//   ol.endMemberDoc(hasArgs=false);
//

static bool writeDefArgumentList(OutputList &ol,const Definition *scope,const MemberDef *md)
{
  const ArgumentList &defArgList=(md->isDocsForDefinition()) ?
                             md->argumentList() : md->declArgumentList();
  //printf("writeDefArgumentList '%s' isDocsForDefinition()=%d hasParameters()=%d (%s)\n",
  //    qPrint(md->name()),md->isDocsForDefinition(),defArgList.hasParameters(),qPrint(argListToString(defArgList)));
  if (!defArgList.hasParameters() || md->isProperty() || md->isTypedef())
  {
    return false; // member has no function like argument list
  }

  bool isDefine = md->isDefine();
  if (!isDefine) ol.docify(" ");

  //printf("writeDefArgList(%d)\n",defArgList->count());
  ol.endMemberDocName();
  ol.startParameterList(!md->isObjCMethod());
  //printf("===> name=%s isDefine=%d\n",qPrint(md->name()),md->isDefine());

  DString cName;
  if (scope)
  {
    cName=scope->name();
    size_t il=cName.find('<');
    size_t ir=cName.rfind('>');
    if (il!=DString::npos && ir!=DString::npos && ir>il)
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
      cName.clear();
    }
  }
  //printf("~~~ %s cName=%s\n",qPrint(md->name()),qPrint(cName));

  DString sep = getLanguageSpecificSeparator(md->getLanguage(),true);

  LinkifyTextOptions options;
  options.setScope(scope).setFileScope(md->getBodyDef()).setSelf(md);

  bool first=true;
  bool paramTypeStarted=false;
  auto alIt = defArgList.begin();
  while (alIt!=defArgList.end())
  {
    Argument a = *alIt;
    if (isDefine || first)
    {
      ol.startParameterType(first,DString());
      paramTypeStarted=true;
      if (isDefine)
      {
        ol.endParameterType();
        ol.startParameterName(true);
      }
    }

    if (!a.attrib.empty() && !md->isObjCMethod()) // argument has an IDL attribute
    {
      ol.docify(a.attrib+" ");
    }

    DString atype = a.type;
    if (sep!="::") { atype=substitute(atype,"::",sep); }

    size_t funcPtrPos=DString::npos;
    {
      if (md->isObjCMethod()) { atype.prepend("("); atype.append(")"); }
      if (atype!="...")
      {
        if (!cName.empty() && scope && scope!=Doxygen::globalScope)
        {
          atype=addTemplateNames(atype,scope->name(),cName);
        }
        // 1. split ...*)(... -> '*' + name + ')(...'
        // 2. split ...*[some thing])(... -> '*' + name + '[some thing])(...'
        size_t starPos = atype.find('*');                       // find pointer
        if (starPos==DString::npos) starPos = atype.find('&'); // can also be reference
        funcPtrPos = atype.find(")(");
        if (starPos!=DString::npos && funcPtrPos!=DString::npos && funcPtrPos>starPos)
        {
          funcPtrPos=starPos+1;
        }
        else
        {
          funcPtrPos=DString::npos;
        }
        linkifyText(TextGeneratorOLImpl(ol),
                    funcPtrPos==DString::npos ? atype : atype.left(funcPtrPos),
                    options);
      }
    }

    if (!isDefine)
    {
      if (paramTypeStarted)
      {
        ol.endParameterType();
        paramTypeStarted=false;
      }
      ol.startParameterName(defArgList.size()<2);
    }
    else
    {
      ol.endParameterName();
    }

    if (atype=="...")
    {
      ol.docify(atype);
    }
    else if (!a.name.empty()) // argument has a name
    {
      ol.docify(a.name);
    }
    if (!isDefine)
    {
      if (funcPtrPos!=DString::npos)
      {
        ol.writeNonBreakableSpace(1);
      }
      ol.endParameterName();
    }
    ol.startParameterExtra();
    if (funcPtrPos!=DString::npos)
    {
      linkifyText(TextGeneratorOLImpl(ol),atype.mid(funcPtrPos),options);
    }
    if (!a.array.empty())
    {
      ol.docify(a.array);
    }
    if (!a.defval.empty()) // write the default value
    {
      DString n=a.defval;
      if (scope && scope!=Doxygen::globalScope && !cName.empty())
      {
        n=addTemplateNames(n,scope->name(),cName);
      }
      ol.startParameterDefVal(" = ");
      linkifyText(TextGeneratorOLImpl(ol),n,LinkifyTextOptions(options).setKeepSpaces(true));
      ol.endParameterDefVal();
    }
    ++alIt;
    if (alIt!=defArgList.end())
    {
      a = *alIt;
      if (!md->isObjCMethod()) ol.docify(", "); // there are more arguments
      if (!isDefine)
      {
        DString key;
        if (md->isObjCMethod() && a.attrib.length()>=2)
        {
          //printf("Found parameter keyword %s\n",a.qPrint(attrib));
          // strip [ and ]
          key=a.attrib.mid(1,a.attrib.length()-2);
          if (key!=",") key+=":"; // for normal keywords add colon
        }
        ol.endParameterExtra(false,false,!md->isObjCMethod());
        ol.startParameterType(false,key);
        paramTypeStarted=true;
      }
      else // isDefine
      {
        ol.endParameterExtra(false,false,true);
      }
    }
    first=false;
  }
  if (first)
  {
    ol.startParameterName(defArgList.size()<2);
    ol.endParameterName();
    ol.startParameterExtra();
  }
  ol.endParameterExtra(true,defArgList.size()<2,!md->isObjCMethod());
  if (!md->extraTypeChars().empty())
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
  if (defArgList.refQualifier()==RefQualifierType::LValue)
  {
    ol.docify(" &");
  }
  else if (defArgList.refQualifier()==RefQualifierType::RValue)
  {
    ol.docify(" &&");
  }
  if (!defArgList.trailingReturnType().empty())
  {
    linkifyText(TextGeneratorOLImpl(ol), defArgList.trailingReturnType(), options);
  }
  return true;
}

static void writeExceptionListImpl(
        OutputList &ol, const ClassDef *cd, const MemberDef *md, DString const& exception)
{
  // this is ordinary exception spec - there must be a '('
  //printf("exception='%s'\n",qPrint(exception));
  size_t index = exception.find('(');
  LinkifyTextOptions options;
  options.setScope(cd).setFileScope(md->getBodyDef()).setSelf(md);
  if (index!=DString::npos)
  {
    ol.exceptionEntry(exception.left(index),false);
    ++index; // paren in second column so skip it here
    for (size_t comma = exception.find(',', index); comma!=DString::npos; )
    {
      ++comma; // include comma
      linkifyText(TextGeneratorOLImpl(ol),exception.mid(index,comma-index),options);
      ol.exceptionEntry(DString(),false);
      index=comma;
      comma = exception.find(',', index);
    }
    if (size_t close = exception.find(')', index); close!=DString::npos)
    {
      DString type=removeRedundantWhiteSpace(exception.mid(index,close-index));
      linkifyText(TextGeneratorOLImpl(ol),type,options);
      ol.exceptionEntry(DString(),true);
    }
    else
    {
      warn(md->getDefFileName(),md->getDefLine(),
          "missing ) in exception list on member {}",md->name());
    }
  }
  else // Java Exception
  {
    ol.docify(" ");
    linkifyText(TextGeneratorOLImpl(ol),exception,options);
  }
}

static void writeExceptionList(OutputList &ol, const ClassDef *cd, const MemberDef *md)
{
  DString exception = md->excpString().stripWhiteSpace();
  if (exception.at(0)=='{')
  {
    // this is an UNO IDL attribute - need special handling
    size_t index = exception.find(';');
    size_t oldIndex = 1;
    while (index!=DString::npos) // there should be no more than 2 (set / get)
    {
      // omit '{' and ';' -> "set raises (...)"
      writeExceptionListImpl(ol,cd,md,exception.mid(oldIndex,index-oldIndex));
      oldIndex = index+1;
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
                     const DString &t,const DString &a,const DString &e,
                     Protection p,Specifier v,bool s,Relationship r,
                     MemberType mt,const ArgumentList &tal,
                     const ArgumentList &al,const DString &meta
                    )
{
  m_classDef=nullptr;
  m_fileDef=nullptr;
  m_moduleDef=nullptr;
  m_redefines=nullptr;
  m_relatedAlso=nullptr;
  m_nspace=nullptr;
  m_memDef=nullptr;
  m_memDec=nullptr;
  m_group=nullptr;
  m_grpId=-1;
  m_enumScope=nullptr;
  m_livesInsideEnum=false;
  m_hasCallGraph            = Config_getBool(CALL_GRAPH);
  m_hasCallerGraph          = Config_getBool(CALLER_GRAPH);
  m_hasReferencedByRelation = Config_getBool(REFERENCED_BY_RELATION);
  m_hasReferencesRelation   = Config_getBool(REFERENCES_RELATION);
  m_hasEnumValues           = Config_getBool(SHOW_ENUM_VALUES);
  m_hasInlineSource         = Config_getBool(INLINE_SOURCES);
  m_initLines=0;
  m_type=t;
  if (mt==MemberType::Typedef) m_type.stripPrefix("typedef ");
  //  type.stripPrefix("struct ");
  //  type.stripPrefix("class " );
  //  type.stripPrefix("union " );
  m_type=removeRedundantWhiteSpace(m_type);
  m_args=a;
  m_args=removeRedundantWhiteSpace(m_args);
  if (m_type.empty()) m_decl=d->name()+m_args; else m_decl=m_type+" "+d->name()+m_args;

  m_memberGroup=nullptr;
  m_virt=v;
  m_prot=p;
  m_related=r;
  m_stat=s;
  m_mtype=mt;
  m_exception=e;
  m_proto=false;
  m_memSpec=TypeSpecifier();
  m_vhdlSpec=VhdlSpecifier::UNKNOWN;
  m_annEnumType=nullptr;
  m_groupAlias=nullptr;
  m_explExt=false;
  m_tspec=false;
  m_anonymousType=nullptr;
  m_maxInitLines=Config_getInt(MAX_INITIALIZER_LINES);
  m_userInitLines=-1;
  m_docEnumValues=false;
  // copy function template arguments (if any)
  m_tArgList = tal;
  //printf("new member al=%p\n",al);
  // copy function definition arguments (if any)
  m_defArgList = al;
  // convert function declaration arguments (if any)
  if (!m_args.empty())
  {
    m_declArgList = *stringToArgumentList(d->getLanguage(),m_args,&m_extraTypeChars);
    //printf("setDeclArgList %s to %s const=%d\n",qPrint(args),
    //    qPrint(argListToString(declArgList)),declArgList->constSpecifier);
  }
  m_metaData = meta;
  m_templateMaster = nullptr;
  m_docsForDefinition = true;
  m_isTypedefValCached = false;
  m_cachedTypedefValue = nullptr;
  m_implOnly=false;
  m_groupMember = nullptr;
  m_hasDocumentedParams = false;
  m_hasDocumentedReturnType = false;
  m_docProvider = nullptr;
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

MemberDefImpl::MemberDefImpl(const DString &df,int dl,int dc,
                     const DString &t,const DString &na,const DString &a,const DString &e,
                     Protection p,Specifier v,bool s,Relationship r,MemberType mt,
                     const ArgumentList &tal,const ArgumentList &al,const DString &meta
                    ) : DefinitionMixin(df,dl,dc,removeRedundantWhiteSpace(na))
{
  //printf("MemberDefImpl::MemberDef(%s)\n",qPrint(na));
  init(this,t,a,e,p,v,s,r,mt,tal,al,meta);
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

std::unique_ptr<MemberDef> MemberDefImpl::deepCopy() const
{
  std::unique_ptr<MemberDefImpl> result(new MemberDefImpl(
        getDefFileName(),getDefLine(),getDefColumn(),m_type,localName(),m_args,m_exception,
        m_prot,m_virt,m_stat,m_related,m_mtype,m_tArgList,m_defArgList,m_metaData));
  // first copy base members
  result->DefinitionMixin<MemberDefMutable>::operator=(*this);
  // then copy other members
  result->m_typeConstraints                = m_typeConstraints                ;
  result->m_declArgList                    = m_declArgList                    ;
  result->m_classDef                       = m_classDef                       ;
  result->m_fileDef                        = m_fileDef                        ;
  result->m_moduleDef                      = m_moduleDef                      ;
  result->m_nspace                         = m_nspace                         ;
  result->m_enumScope                      = m_enumScope                      ;
  result->m_livesInsideEnum                = m_livesInsideEnum                ;
  result->m_annEnumType                    = m_annEnumType                    ;
  result->m_enumFields                     = m_enumFields                     ;
  result->m_redefines                      = m_redefines                      ;
  result->m_redefinedBy                    = m_redefinedBy                    ;
  result->m_memDef                         = m_memDef                         ;
  result->m_memDec                         = m_memDec                         ;
  result->m_relatedAlso                    = m_relatedAlso                    ;
  result->m_examples                       = m_examples                       ;
  result->m_def                            = m_def                            ;
  result->m_anc                            = m_anc                            ;
  result->m_decl                           = m_decl                           ;
  result->m_bitfields                      = m_bitfields                      ;
  result->m_read                           = m_read                           ;
  result->m_write                          = m_write                          ;
  result->m_exception                      = m_exception                      ;
  result->m_initializer                    = m_initializer                    ;
  result->m_extraTypeChars                 = m_extraTypeChars                 ;
  result->m_enumBaseType                   = m_enumBaseType                   ;
  result->m_requiresClause                 = m_requiresClause                 ;
  result->m_initLines                      = m_initLines                      ;
  result->m_memSpec                        = m_memSpec                        ;
  result->m_vhdlSpec                       = m_vhdlSpec                       ;
  result->m_maxInitLines                   = m_maxInitLines                   ;
  result->m_userInitLines                  = m_userInitLines                  ;
  result->m_defArgList                     = m_defArgList                     ;
  result->m_declArgList                    = m_declArgList                    ;
  result->m_tArgList                       = m_tArgList                       ;
  result->m_typeConstraints                = m_typeConstraints                ;
  result->m_templateMaster                 = m_templateMaster                 ;
  result->m_formalTemplateArguments        = m_formalTemplateArguments        ;
  result->m_defTmpArgLists                 = m_defTmpArgLists                 ;
  result->m_anonymousType                  = m_anonymousType                  ;
  result->m_sectionMap                     = m_sectionMap                     ;
  result->m_groupAlias                     = m_groupAlias                     ;
  result->m_grpId                          = m_grpId                          ;
  result->m_memberGroup                    = m_memberGroup                    ;
  result->m_group                          = m_group                          ;
  result->m_grouppri                       = m_grouppri                       ;
  result->m_groupFileName                  = m_groupFileName                  ;
  result->m_groupStartLine                 = m_groupStartLine                 ;
  result->m_groupMember                    = m_groupMember                    ;
  result->m_isTypedefValCached             = m_isTypedefValCached             ;
  result->m_cachedTypedefValue             = m_cachedTypedefValue             ;
  result->m_cachedTypedefTemplSpec         = m_cachedTypedefTemplSpec         ;
  result->m_cachedResolvedType             = m_cachedResolvedType             ;
  result->m_docProvider                    = m_docProvider                    ;
  result->m_explicitOutputFileBase         = m_explicitOutputFileBase         ;
  result->m_qualifiers                     = m_qualifiers                     ;
  result->m_implOnly                       = m_implOnly                       ;
  result->m_hasDocumentedParams            = m_hasDocumentedParams            ;
  result->m_hasDocumentedReturnType        = m_hasDocumentedReturnType        ;
  result->m_isDMember                      = m_isDMember                      ;
  result->m_proto                          = m_proto                          ;
  result->m_docEnumValues                  = m_docEnumValues                  ;
  result->m_hasDetailedDescriptionCached   = m_hasDetailedDescriptionCached   ;
  result->m_detailedDescriptionCachedValue = m_detailedDescriptionCachedValue ;
  result->m_hasCallGraph                   = m_hasCallGraph                   ;
  result->m_hasCallerGraph                 = m_hasCallerGraph                 ;
  result->m_hasReferencedByRelation        = m_hasReferencedByRelation        ;
  result->m_hasReferencesRelation          = m_hasReferencesRelation          ;
  result->m_hasEnumValues                  = m_hasEnumValues                  ;
  result->m_hasInlineSource                = m_hasInlineSource                ;
  result->m_explExt                        = m_explExt                        ;
  result->m_tspec                          = m_tspec                          ;
  result->m_groupHasDocs                   = m_groupHasDocs                   ;
  result->m_docsForDefinition              = m_docsForDefinition              ;
  result->m_category                       = m_category                       ;
  result->m_categoryRelation               = m_categoryRelation               ;
  result->m_declFileName                   = m_declFileName                   ;
  result->m_declLine                       = m_declLine                       ;
  result->m_declColumn                     = m_declColumn                     ;
  result->m_numberOfFlowKW                 = m_numberOfFlowKW                 ;
  result->setDefinitionTemplateParameterLists(m_defTmpArgLists);

  result->m_isLinkableCached    = 0;
  result->m_isConstructorCached = 0;
  result->m_isDestructorCached  = 0;
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
    MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef *>(m_templateMaster));
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
        if (cd==mcd || cd->isBaseClass(mcd,true))
        {
          return true;
        }
      }
    }
  }
  return false;
}

void MemberDefImpl::insertEnumField(MemberDef *md)
{
  m_enumFields.push_back(md);
}

bool MemberDefImpl::addExample(const DString &anchor,const DString &nameStr, const DString &file)
{
  //printf("%s::addExample(%s,%s,%s)\n",qPrint(name()),anchor,nameStr,file);
  return m_examples.inSort(Example(anchor,nameStr,file));
}

bool MemberDefImpl::hasExamples() const
{
  return !m_examples.empty();
}

DString MemberDefImpl::sourceRefName() const
{
  DString n  = name();
  DString s = getScopeString();

  if (!s.empty())
  {
    n.prepend(s+"::");
  }
  else if (isStatic() && getFileDef())
  {
    n.prepend(getFileDef()->name()+":");
  }
  if (isCallable())
  {
    n.append(argsString());
  }
  return n;
}

DString MemberDefImpl::getOutputFileBase() const
{
  bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  DString baseName;

  //printf("Member: %s: templateMaster=%p group=%p classDef=%p nspace=%p fileDef=%p\n",
  //    qPrint(name()),m_templateMaster,m_group,m_classDef,
  //    m_nspace,m_fileDef);
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  const ClassDef *classDef = getClassDef();
  const ModuleDef *moduleDef = getModuleDef();
  const GroupDef *groupDef = getGroupDef();
  if (!m_explicitOutputFileBase.empty())
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

  if (baseName.empty())
  {
    warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: member {} does not belong to any container!",name()
      );
    return "dummy";
  }
  else if (separateMemberPages && hasDetailedDescription())
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

DString MemberDefImpl::getReference() const
{
  DString ref = DefinitionMixin::getReference();
  if (!ref.empty())
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

DString MemberDefImpl::anchor() const
{
  DString result=m_anc;
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
    else if (getReference().empty())
    {
      result.prepend("g");
    }
  }
  return result;
}

void MemberDefImpl::_computeLinkableInProject()
{
  AUTO_TRACE("name={}",qualifiedName());
  bool extractStatic  = Config_getBool(EXTRACT_STATIC);
  bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
  m_isLinkableCached = 2; // linkable
  //printf("MemberDefImpl::isLinkableInProject(name=%s)\n",qPrint(name()));
  if (isHidden())
  {
    AUTO_TRACE_ADD("is hidden");
    m_isLinkableCached = 1;
    return;
  }
  if (templateMaster())
  {
    AUTO_TRACE_ADD("has master template");
    m_isLinkableCached = templateMaster()->isLinkableInProject() ? 2 : 1;
    return;
  }
  if (isAnonymous())
  {
    AUTO_TRACE_ADD("name invalid");
    m_isLinkableCached = 1; // not a valid or a dummy name
    return;
  }
  if (!hasDocumentation() || isReference())
  {
    AUTO_TRACE_ADD("no docs or reference");
    m_isLinkableCached = 1; // no documentation
    return;
  }
  const GroupDef *groupDef = getGroupDef();
  const ClassDef *classDef = getClassDef();
  if (groupDef && !groupDef->isLinkableInProject())
  {
    AUTO_TRACE_ADD("in not linkable group");
    m_isLinkableCached = 1; // group but group not linkable
    return;
  }
  if (!groupDef && classDef && !classDef->isLinkableInProject())
  {
    AUTO_TRACE_ADD("in not linkable class");
    m_isLinkableCached = 1; // in class but class not linkable
    return;
  }
  const NamespaceDef *nspace = getNamespaceDef();
  const FileDef *fileDef = getFileDef();
  if (!groupDef && nspace /*&& m_related==Relationship::Member*/ && !nspace->isLinkableInProject()
      && (fileDef==nullptr || !fileDef->isLinkableInProject()))
  {
    AUTO_TRACE_ADD("in not linkable namespace");
    m_isLinkableCached = 1; // in namespace but namespace not linkable
    return;
  }
  if (!groupDef && !nspace &&
      /*m_related==Relationship::Member &&*/ !classDef &&
      fileDef && !fileDef->isLinkableInProject())
  {
    AUTO_TRACE_ADD("in not linkable file");
    m_isLinkableCached = 1; // in file (and not in namespace) but file not linkable
    return;
  }
  if ((!protectionLevelVisible(m_prot) && m_mtype!=MemberType::Friend) &&
       !(m_prot==Protection::Private && (m_virt!=Specifier::Normal || isOverride() || isFinal()) && extractPrivateVirtual))
  {
    AUTO_TRACE_ADD("private and invisible");
    m_isLinkableCached = 1; // hidden due to protection
    return;
  }
  if (m_stat && classDef==nullptr && !extractStatic)
  {
    AUTO_TRACE_ADD("static and invisible");
    m_isLinkableCached = 1; // hidden due to staticness
    return;
  }
  AUTO_TRACE_ADD("linkable");
  return; // linkable!
}

void MemberDefImpl::setDocumentation(const DString &d,const DString &docFile,int docLine,bool stripWhiteSpace)
{
  DefinitionMixin::setDocumentation(d,docFile,docLine,stripWhiteSpace);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setBriefDescription(const DString &b,const DString &briefFile,int briefLine)
{
  DefinitionMixin::setBriefDescription(b,briefFile,briefLine);
  m_isLinkableCached = 0;
}

void MemberDefImpl::setDocTransferDone()
{
  m_docTransferDone = true;
}

void MemberDefImpl::setExplicitInherited(bool b)
{
  m_explicitInherited = b;
}

void MemberDefImpl::setInbodyDocumentation(const DString &d,const DString &inbodyFile,int inbodyLine)
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
  DString sep = getLanguageSpecificSeparator(lang,true);
  DString n = name();
  const ClassDef *classDef = getClassDef();
  const NamespaceDef *nspace = getNamespaceDef();
  if (!hideScopeNames)
  {
    if (m_enumScope && m_livesInsideEnum && getGroupDef()!=nullptr)
    {
      n.prepend(m_enumScope->displayName()+sep);
    }
    if (classDef && (gd || mod) && !isRelated())
    {
      n.prepend(classDef->displayName()+sep);
    }
    else if (nspace && (gd || fd || mod))
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
    if (m_mtype==MemberType::EnumValue && getGroupDef()==nullptr &&          // enum value is not grouped
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

void MemberDefImpl::setClassDefOfAnonymousType(const ClassDef *cd)
{
  m_anonymousType = cd;
}

/*! If this member has an anonymous class/struct/union as its type, then
 *  this method will return the ClassDef that describes this return type.
 */
const ClassDef *MemberDefImpl::getClassDefOfAnonymousType() const
{
  return m_anonymousType;
}

/*! This methods returns true iff the brief section (also known as
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

  bool hasDocs = hasDocumentation();

  // only include static members with file/namespace scope if
  // explicitly enabled in the config file
  bool visibleIfStatic = !(getClassDef()==nullptr &&
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
                            documentation().empty() &&
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
                           m_mtype==MemberType::Friend ||
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
  //       "visibleIfFriendCompound=%d\n",
  //       visibleIfStatic,visibleIfDocumented,
  //       visibleIfEnabled,visibleIfPrivate,visibleIfNotDefaultCDTor,
  //       visibleIfFriendCompound);

  bool visible = visibleIfStatic     && visibleIfDocumented      &&
                 visibleIfEnabled    && visibleIfPrivate         &&
                 /*visibleIfDocVirtual &&*/ visibleIfNotDefaultCDTor &&
                 visibleIfFriendCompound &&
                 !isHidden();
  //printf("MemberDefImpl::isBriefSectionVisible() %d\n",visible);
  return visible;
}

DString MemberDefImpl::getDeclType() const
{
  SrcLangExt lang = getLanguage();
  DString ltype(m_type);
  if (lang==SrcLangExt::Cpp && isEnumerate() && isStrong())
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
  if (isTypedef() && getLanguage() != SrcLangExt::Slice)
  {
    ltype.prepend("typedef ");
  }
  if (isTypeAlias())
  {
    if (lang==SrcLangExt::Python)
    {
      ltype="type";
    }
    else
    {
      ltype="using";
    }
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
  LinkifyTextOptions options;
  options.setScope(def).setFileScope(getFileDef()).setSelf(this);
  for (auto it = al.begin(); it!=al.end();)
  {
    Argument a = *it;
    linkifyText(TextGeneratorOLImpl(ol),a.type,options);
    if (!a.name.empty())
    {
      ol.docify(" ");
      ol.docify(a.name);
    }
    if (!a.defval.empty())
    {
      ol.docify(" = ");
      ol.docify(a.defval);
    }
    ++it;
    if (it!=al.end()) ol.docify(", ");
  }
  ol.docify("> ");
  if (writeReqClause && !m_requiresClause.empty())
  {
    ol.lineBreak();
    ol.docify("requires ");
    linkifyText(TextGeneratorOLImpl(ol), // out
        m_requiresClause,        // text
        LinkifyTextOptions(options).setAutoBreak(true).setBreakThreshold(120));
  }
}

static DString combineArgsAndException(DString args,DString exception)
{
  if (exception.empty()) return args;                      // no exception, nothing to combine args
  size_t pos = args.rfind(')');
  size_t eqPos = pos!=DString::npos ? args.find('=',pos) : DString::npos; // look for '=' in '(args) = something'
  if (eqPos==DString::npos) return args+" "+exception;      // append exception at the end
  return args.left(eqPos)+" "+exception+" "+args.mid(eqPos); // insert exception before =
}

bool MemberDefImpl::_isAnonymousBitField() const
{
  return !m_bitfields.empty() && name().startsWith("__pad"); // anonymous bitfield
}

void MemberDefImpl::writeDeclaration(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
               bool inGroup, int indentLevel, const ClassDef *inheritedFrom,const DString &inheritId) const
{
  //printf("> %s MemberDefImpl::writeDeclaration() inGroup=%d inheritId=%s\n",qPrint(qualifiedName()),inGroup,qPrint(inheritId));

  // hide enum value, since they appear already as part of the enum, unless they
  // are explicitly grouped.
  if (!inGroup && m_mtype==MemberType::EnumValue) return;

  const Definition *d=nullptr;
  ASSERT (cd!=nullptr || nd!=nullptr || fd!=nullptr || gd!=nullptr || mod!=nullptr); // member should belong to something
  if (cd) d=cd;
  else if (nd) d=nd;
  else if (fd) d=fd;
  else d=gd;
  if (d==gd || d==mod) // see bug 753608
  {
    if (getClassDef())          d = getClassDef();
    else if (getNamespaceDef()) d = getNamespaceDef();
    else if (getFileDef())      d = getFileDef();
  }
  else if (d==fd) // see issue #9850, namespace member can be shown in file scope as well
  {
    if (getNamespaceDef()) d = getNamespaceDef();
  }
  if (d==nullptr)
  {
    err("No context could be derived for member '{}'\n",name());
    return; // should not happen
  }

  DString cname  = d->name();
  DString cdname = d->displayName();
  DString cfname = getOutputFileBase();

  // search for the last anonymous scope in the member type
  const ClassDef *annoClassDef=getClassDefOfAnonymousType();

  ol.startMemberDeclaration();

  // start a new member declaration
  OutputGenerator::MemberItemType anonType = !m_tArgList.empty() ? OutputGenerator::MemberItemType::Templated      :
                                                                   OutputGenerator::MemberItemType::Normal;
  ol.startMemberItem(annoClassDef ? DString() : anchor(), anonType, inheritId);


  // If there is no detailed description we need to write the anchor here.
  bool detailsVisible = hasDetailedDescription();
  bool writeAnchor = (inGroup || getGroupDef()==nullptr) &&     // only write anchors for member that have no details and are
                     !detailsVisible &&                         // rendered inside the group page or are not grouped at all
                     inheritId.empty();

  if (writeAnchor)
  {
    DString doxyArgs=argsString();
    DString doxyName=name();
    if (!cname.empty())
    {
      doxyName.prepend(cdname+getLanguageSpecificSeparator(getLanguage()));
    }
    ol.startDoxyAnchor(cfname,cname,anchor(),doxyName,doxyArgs);
    ol.addLabel(cfname,anchor());
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

  if (annoClassDef)
  {
    for (int j=0;j<indentLevel;j++)
    {
      ol.writeNonBreakableSpace(3);
    }
  }

  // *** write template lists
  if (m_tArgList.hasParameters() && getLanguage()==SrcLangExt::Cpp)
  {
     ol.startMemberTemplateParams();
    _writeTemplatePrefix(ol,d,m_tArgList);
     ol.endMemberTemplateParams(anchor(),inheritId);
  }


  // *** write type
  DString ltype(m_type);
  auto lang = getLanguage();
  if (isTypedef() && lang != SrcLangExt::Slice)
  {
    ltype.prepend("typedef ");
  }
  if (isTypeAlias())
  {
    if (lang==SrcLangExt::Python)
    {
      ltype="type";
    }
    else
    {
      ltype="using";
    }
  }
  // strip 'friend' keyword from ltype
  ltype.stripPrefix("friend ");
  static const reg::Ex r(R"(@\d+)"); // anonymous type marker
  reg::Match match;
  std::string stype = ltype.str();
  bool endAnonScopeNeeded=false;
  LinkifyTextOptions options;
  options.setScope(d).setFileScope(getBodyDef()).setSelf(this);
  if (reg::search(stype,match,r)) // member has an anonymous type marker
  {
    //printf("name=%s annoClassDef=%p stype='%s' scopeName='%s'\n",
    //    qPrint(name()),(void*)annoClassDef,qPrint(stype),qPrint(cname));

    static const reg::Ex ar(R"([\w@:]*@\d+[\w@:]*)"); // anonymous type marker(s) including scope
    reg::Match amatch;
    reg::search(stype,amatch,ar);
    size_t ai = amatch.position();
    size_t al = amatch.length();
    const MemberDef *amd = getAnonymousEnumType();
    if (annoClassDef && amd==nullptr) // type is an anonymous compound
    {
      ol.writeObjectLink(annoClassDef->getReference(),annoClassDef->getOutputFileBase(),
                         annoClassDef->anchor(),ltype.left(ai)+ltype.mid(ai+al));
    }
    else if (amd) // type is an anonymous enum
    {
      DString typePlaceholder = " { ... } ";
      if (lang==SrcLangExt::Cpp)
      {
        if (amd->isEnumStruct()) typePlaceholder.prepend(" struct");
        else if (amd->isStrong()) typePlaceholder.prepend(" class");
      }
      ol.writeObjectLink(amd->getReference(),amd->getOutputFileBase(),
                         amd->anchor(),ltype.left(ai)+typePlaceholder+ltype.mid(ai+al));
    }
    else // fallback if anonymous type is not found
    {
      size_t i = match.position();
      size_t l = match.length();
      ltype = ltype.left(i) + " { ... } " + removeAnonymousScopes(ltype.mid(i+l));
      linkifyText(TextGeneratorOLImpl(ol), ltype, options);
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
    linkifyText(TextGeneratorOLImpl(ol),ltype,options);
  }

  if (!ltype.empty())
  {
    ol.pushGeneratorState();
    ol.disable(OutputType::Html);
    ol.docify(" ");
    ol.popGeneratorState();
  }

  ol.insertMemberAlign(m_tArgList.hasParameters());

  // *** write name
  if (!isAnonymous() && !_isAnonymousBitField()) // hide anonymous stuff
  {
    bool extractPrivateVirtual = Config_getBool(EXTRACT_PRIV_VIRTUAL);
    bool extractStatic  = Config_getBool(EXTRACT_STATIC);
    bool visibleIfPrivate = (protectionLevelVisible(protection()) ||
                             m_mtype==MemberType::Friend ||
                             (m_prot==Protection::Private &&
                                (m_virt!=Specifier::Normal || isOverride() || isFinal()) &&
                                extractPrivateVirtual && hasDocumentation()
                             ));
    //printf("Member name=`%s gd=%p md->groupDef=%p inGroup=%d isLinkable()=%d hasDocumentation=%d\n",qPrint(name()),gd,getGroupDef(),inGroup,isLinkable(),hasDocumentation());
    if (!name().empty() && // name valid
        (hasDetailedDescription() || isReference()) && // has docs
        visibleIfPrivate &&
        !(isStatic() && getClassDef()==nullptr && !extractStatic) // hidden due to static-ness
       )
    {
      //printf("writeLink %s->%d\n",qPrint(name),hasDocumentation());
      const ClassDef *rcd = cd;
      if (isReference() && getClassDef()) rcd = getClassDef();
      writeLink(ol,rcd,nd,fd,gd,mod);
    }
    else if (isDocumentedFriendClass())
      // if the member is an undocumented friend declaration for some class,
      // then maybe we can link to the class
    {
      writeLink(ol,getClass(name()),nullptr,nullptr,nullptr,nullptr);
    }
    else
      // there is a brief member description and brief member
      // descriptions are enabled or there is no detailed description.
    {
      const ClassDef *rcd = cd;
      if (isReference() && getClassDef()) rcd = getClassDef();
      writeLink(ol,rcd,nd,fd,gd,mod,true);
    }
  }

  // *** write arguments
  if (!argsString().empty() && !isObjCMethod())
  {
    if (!isDefine() && !isTypedef()) ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol), // out
                isDefine() ?
                   substitute(argsString(),",",", ") :
                isTypedef() ?
                   substitute(argsString(),")(",") (") :
                   combineArgsAndException(argsString(),excpString()), // text
                LinkifyTextOptions(options)
                .setArgumentList(&m_defArgList)
                .setAutoBreak(false)
                .setIndentLevel(indentLevel)
               );
  }

  // *** write bitfields
  if (!m_bitfields.empty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),m_bitfields,options);
  }
  else if (hasOneLineInitializer()
      //!init.empty() && initLines==0 && // one line initializer
      //((maxInitLines>0 && userInitLines==-1) || userInitLines>0) // enabled by default or explicitly
          ) // add initializer
  {
    if (isTypeAlias()) // using statement
    {
      ol.writeString(" = ");
      linkifyText(TextGeneratorOLImpl(ol),m_initializer.simplifyWhiteSpace(),options);
    }
    else if (!isDefine())
    {
      //ol.writeString(" = ");
      ol.writeString(" ");
      linkifyText(TextGeneratorOLImpl(ol),m_initializer.simplifyWhiteSpace(),options);
    }
    else
    {
      ol.writeNonBreakableSpace(3);
      linkifyText(TextGeneratorOLImpl(ol),m_initializer,options);
    }
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

      if (isGettable())             sl.emplace_back("get");
      if (isProtectedGettable())    sl.emplace_back("protected get");
      if (isSettable())             sl.emplace_back("set");
      if (isProtectedSettable())    sl.emplace_back("protected set");
      if (extractPrivate)
      {
        if (isPrivateGettable())    sl.emplace_back("private get");
        if (isPrivateSettable())    sl.emplace_back("private set");
      }
      bool first=true;
      for (const auto &s : sl)
      {
         if (!first)
         {
           ol.docify(", ");
         }
         first=false;
         ol.docify(s);
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
      if (isAddable())   sl.emplace_back("add");
      if (isRemovable()) sl.emplace_back("remove");
      if (isRaisable())  sl.emplace_back("raise");
      bool first=true;
      for (const auto &s : sl)
      {
        if (!first)
        {
          ol.docify(", ");
        }
        first=false;
        ol.docify(s);
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
  if (!briefDescription().empty() &&
      Config_getBool(BRIEF_MEMBER_DESC)
     )
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     briefFile(),
                                     briefLine(),
                                     getOuterScope()?getOuterScope():d,
                                     this,
                                     briefDescription(),
                                     DocOptions()
                                     .setIndexWords(inheritedFrom==nullptr)
                                     .setSingleLine(true))
                };
    if (!ast->empty())
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
           !documentation().empty() ||
           // has inbody docs
           !inbodyDocumentation().empty() ||
           // is an enum with values that are documented
           (isEnumerate() && hasDocumentedEnumValues()) ||
           // is documented enum value
           (m_mtype==MemberType::EnumValue && !briefDescription().empty()) ||
           // has brief description that is part of the detailed description
           (!briefDescription().empty() &&           // has brief docs
            (alwaysDetailedSec &&                      // they are visible in
             (repeatBrief ||                           // detailed section or
              !briefMemberDesc                         // they are explicitly not
             )                                         // shown in brief section
            )
           ) ||
           // has one or more documented arguments
           (m_templateMaster ?
            m_templateMaster->argumentList().hasDocumentation() :
            m_defArgList.hasDocumentation()) ||
           (m_templateMaster ?
            m_templateMaster->templateArguments().hasTemplateDocumentation() :
            m_tArgList.hasTemplateDocumentation()) ||
           // user-specified qualifiers
           !m_qualifiers.empty();

    // generate function                  guard
    // ==================                 =======
    // _writeGroupInclude              -> ignored in calculation
    // multiLineInitializer()          -> hasMultiLineInitializer()
    // _writeReimplements              -> _isReimplements()
    // _writeReimplementedBy           -> _countReimplementedBy()>0
    // _writeExamples                  -> hasExamples()
    // _writeTypeConstraints           -> m_typeConstraints.hasParameters()
    // writeSourceDef                  -> !getSourceFileBase().empty();
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
           !getSourceFileBase().empty() ||
           // has inline sources
           (inlineSources && hasSources()) ||
           // has references
           (hasReferencesRelation() && hasSourceRefs()) ||
           (hasReferencedByRelation() && hasSourceReffedBy()) ||
           // call graph
           _hasVisibleCallGraph() ||
           // caller graph
           _hasVisibleCallerGraph() ||
           // requirement references
           hasRequirementRefs();

    if (!hideUndocMembers) // if HIDE_UNDOC_MEMBERS is NO we also show the detailed section
                           // if there is only some generated info
    {
      docFilter = docFilter || docInfo;
    }

    // this is not a global static or global statics should be extracted
    bool staticFilter = getClassDef()!=nullptr || !isStatic() || extractStatic;

    // a module does not contain details for members, so either the namespace or file should be linkable
    bool moduleFilter = getModuleDef()==nullptr || (getFileDef() && getFileDef()->isLinkable()) ||
                                                   (getNamespaceDef() && getNamespaceDef()->isLinkable());

    // only include members that are non-private unless EXTRACT_PRIVATE is
    // set to YES or the member is part of a   group
    bool privateFilter = protectionLevelVisible(protection()) || m_mtype==MemberType::Friend ||
                         (m_prot==Protection::Private && (m_virt!=Specifier::Normal || isOverride() || isFinal()) && extractPrivateVirtual);

    // hide friend (class|struct|union) member if HIDE_FRIEND_COMPOUNDS
    // is true
    bool friendCompoundFilter = !(Config_getBool(HIDE_FRIEND_COMPOUNDS) && isFriend());

    m_detailedDescriptionCachedValue =
        (docFilter && staticFilter && moduleFilter && privateFilter && friendCompoundFilter && !isHidden());
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
  bool groupFilter = getGroupDef()==nullptr || container==MemberListContainer::Group || separateMemPages;
  bool fileFilter  = getNamespaceDef()==nullptr || !getNamespaceDef()->isLinkable() || container!=MemberListContainer::File;
  bool simpleFilter = (hasBriefDescription() || !hideUndocMembers) && inlineSimpleStructs &&
                      getClassDef()!=nullptr && getClassDef()->isSimple();

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
  bool inherited = getClassDef() &&
                   (getClassDef()!=container || m_explicitInherited) &&
                   container->definitionType()==TypeClass &&
                   !isRelated();
  Specifier lvirt=virtualness();
  if ((!isObjCMethod() || isOptional() || isRequired()) &&
      (protection()!=Protection::Public || lvirt!=Specifier::Normal ||
       isFriend() || isRelated() ||
       (isInline() && inlineInfo) ||
       isSignal() || isSlot() ||
       isStatic() || isExternal() ||
       isExported() ||
       inherited ||
       TypeSpecifier(m_memSpec).setInline(false)!=TypeSpecifier()
      )
     )
  {
    // write the member specifier list
    //ol.writeLatexSpacing();
    //ol.startTypewriter();
    //ol.docify(" [");
    SrcLangExt lang = getLanguage();
    bool optVhdl = lang==SrcLangExt::VHDL;
    bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
    if (optVhdl)
    {
      sl.push_back(theTranslator->trVhdlType(getVhdlSpecifiers(),true).str());
    }
    else
    {
      if (isFriend())                                   sl.emplace_back("friend");
      else if (isRelated())                             sl.emplace_back("related");
      else
      {
        if      (isExternal())                          sl.emplace_back("extern");
        if      (inlineInfo && isInline())              sl.emplace_back("inline");
        if      (isExplicit())                          sl.emplace_back("explicit");
        if      (isMutable())                           sl.emplace_back("mutable");
        if      (isThreadLocal())                       sl.emplace_back("thread_local");
        if      (isStatic())                            sl.emplace_back("static");
        if      (isGettable())                          sl.emplace_back("get");
        if      (isProtectedGettable())                 sl.emplace_back("protected get");
        if      (isSettable())                          sl.emplace_back("set");
        if      (isProtectedSettable())                 sl.emplace_back("protected set");
        if (extractPrivate)
        {
          if    (isPrivateGettable())                   sl.emplace_back("private get");
          if    (isPrivateSettable())                   sl.emplace_back("private set");
        }
        if      (isNoDiscard())                         sl.emplace_back("nodiscard");
        if      (isConstExpr())                         sl.emplace_back("constexpr");
        if      (isConstEval())                         sl.emplace_back("consteval");
        if      (isConstInit())                         sl.emplace_back("constinit");
        if      (isAddable())                           sl.emplace_back("add");
        if      (!isUNOProperty() && isRemovable())     sl.emplace_back("remove");
        if      (isRaisable())                          sl.emplace_back("raise");
        if      (isReadable())                          sl.emplace_back("read");
        if      (isWritable())                          sl.emplace_back("write");
        if      (isFinal())                             sl.emplace_back("final");
        if      (isAbstract())                          sl.emplace_back("abstract");
        if      (isOverride())                          sl.emplace_back("override");
        if      (isInitonly())                          sl.emplace_back("initonly");
        if      (isSealed())                            sl.emplace_back("sealed");
        if      (isNew())                               sl.emplace_back("new");
        if      (isOptional())                          sl.emplace_back("optional");
        if      (isRequired())                          sl.emplace_back("required");
        if      (isExported())                          sl.emplace_back("export");

        if      (isNonAtomic())                         sl.emplace_back("nonatomic");
        else if (isObjCProperty())                      sl.emplace_back("atomic");

        // mutual exclusive Objective 2.0 property attributes
        if      (isAssign())                            sl.emplace_back("assign");
        else if (isCopy())                              sl.emplace_back("copy");
        else if (isRetain())                            sl.emplace_back("retain");
        else if (isWeak())                              sl.emplace_back("weak");
        else if (lang!=SrcLangExt::CSharp && isStrong()) sl.emplace_back("strong");
        else if (isUnretained())                        sl.emplace_back("unsafe_unretained");

        if (!isObjCMethod())
        {
          if      (protection()==Protection::Protected) sl.emplace_back("protected");
          else if (protection()==Protection::Private)   sl.emplace_back("private");
          else if (protection()==Protection::Package)   sl.emplace_back("package");

          if      (lvirt==Specifier::Virtual)           sl.emplace_back("virtual");
          else if (lvirt==Specifier::Pure)              sl.emplace_back("pure virtual");
          if      (isSignal())                          sl.emplace_back("signal");
          if      (isSlot())                            sl.emplace_back("slot");
          if      (isDefault())                         sl.emplace_back("default");
          if      (isDelete())                          sl.emplace_back("delete");
          if      (isNoExcept())                        sl.emplace_back("noexcept");
          if      (isAttribute())                       sl.emplace_back("attribute");
          if      (isUNOProperty())                     sl.emplace_back("property");
          if      (isReadonly())                        sl.emplace_back("readonly");
          if      (isBound())                           sl.emplace_back("bound");
          if      (isUNOProperty() && isRemovable())    sl.emplace_back("removable");
          if      (isConstrained())                     sl.emplace_back("constrained");
          if      (isTransient())                       sl.emplace_back("transient");
          if      (isMaybeVoid())                       sl.emplace_back("maybevoid");
          if      (isMaybeDefault())                    sl.emplace_back("maybedefault");
          if      (isMaybeAmbiguous())                  sl.emplace_back("maybeambiguous");
          if      (isPublished())                       sl.emplace_back("published"); // enum
        }
        if (isObjCProperty() && isImplementation())
        {
          sl.emplace_back("implementation");
        }
      }
      if (inherited)
      {
        sl.emplace_back("inherited");
      }
    }
  }
  else if (isObjCMethod() && isImplementation())
  {
    sl.emplace_back("implementation");
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
    DotCallGraph callGraph(this,false);
    if (callGraph.isTooBig())
    {
       warn_uncond("Call graph for '{}' not generated, too many nodes ({}), threshold is {}. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qualifiedName(), callGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!callGraph.isTrivial())
    {
      msg("Generating call graph for function {}\n",qualifiedName());
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
    DotCallGraph callerGraph(this, true);
    if (callerGraph.isTooBig())
    {
       warn_uncond("Caller graph for '{}' not generated, too many nodes ({}), threshold is {}. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qualifiedName(), callerGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!callerGraph.isTrivial())
    {
      msg("Generating caller graph for function {}\n",qualifiedName());
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
  const ClassDef *bcd=nullptr;
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
  const ClassDef *bcd=nullptr;
  if (bmd && (bcd=bmd->getClassDef()))
  {
    // write class that contains a member that is reimplemented by this one
    if (bcd->isLinkable())
    {
      ol.startParagraph();
      DString reimplFromLine;
      if (bmd->virtualness()!=Specifier::Pure && bcd->compoundType()!=ClassDef::Interface)
      {
        reimplFromLine = theTranslator->trReimplementedFromList(1);
      }
      else
      {
        reimplFromLine = theTranslator->trImplementedFromList(1);
      }
      if (size_t markerPos = reimplFromLine.find("@0"); markerPos!=DString::npos) // should always pass this.
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
              DString(),bcd->displayName());
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
      const MemberDef *bmd = nullptr;
      const ClassDef *bcd = nullptr;
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

    DString reimplInLine;
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
    DString text;
    DString ref;
    DString file;
    DString anc;
    DString name;
    size_t i=DString::npos;
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
      if (i!=DString::npos)
      {
        const MemberDef *md = m_categoryRelation;
        ref  = md->getReference();
        file = md->getOutputFileBase();
        anc  = md->anchor();
      }
    }
    if (i!=DString::npos && !name.empty())
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
                                 const DString &cfname,const DString &ciname,
                                 const DString &cname) const
{
  // For enum, we also write the documented enum values
  if (isEnumerate())
  {
    bool first=true;
    //printf("** %s: enum values=%zu\n",qPrint(name()),enumFieldList().size());
    bool hasInits = false;
    if (hasEnumValues())
    {
      for (const auto &fmd : enumFieldList())
      {
        if (fmd->isLinkable())
        {
          if (!fmd->initializer().empty())
          {
            hasInits = true;
            break;
          }
        }
      }
    }
    for (const auto &fmd : enumFieldList())
    {
      //printf("Enum %p: isLinkable()=%d\n",fmd,fmd->isLinkable());
      if (fmd->isLinkable())
      {
        if (first)
        {
          ol.startDescTable(theTranslator->trEnumerationValues(),hasInits);
          first=false;
        }

        ol.startDescTableRow();
        ol.addIndexItem(fmd->name(),ciname);
        ol.addIndexItem(ciname,fmd->name());

        ol.startDescTableTitle();
        ol.startDoxyAnchor(cfname,cname,fmd->anchor(),fmd->name(),fmd->argsString());
        ol.addLabel(cfname,fmd->anchor());
        ol.docify(fmd->name());
        ol.disableAllBut(OutputType::Man);
        ol.writeString(" ");
        ol.enableAll();
        ol.endDoxyAnchor(cfname,fmd->anchor());
        ol.endDescTableTitle();
        if (hasInits)
        {
          ol.startDescTableInit();
          if (!fmd->initializer().empty())
          {
            DString initStr = fmd->initializer().stripWhiteSpace();
            if (initStr.startsWith("=")) initStr = initStr.mid(1).stripWhiteSpace();
            ol.disableAllBut(OutputType::Man);
            ol.writeString("(");
            ol.enableAll();
            ol.docify(initStr);
            ol.disableAllBut(OutputType::Man);
            ol.writeString(")");
            ol.enableAll();
          }
          ol.endDescTableInit();
        }
        ol.startDescTableData();

        bool hasBrief = !fmd->briefDescription().empty();
        bool hasDetails = !fmd->documentation().empty();

        if (hasBrief)
        {
          ol.generateDoc(fmd->briefFile(),
                         fmd->briefLine(),
                         getOuterScope()?getOuterScope():container,
                         fmd,
                         fmd->briefDescription(),
                         DocOptions()
                         .setIndexWords(true));
        }
        if (hasDetails)
        {
          ol.generateDoc(fmd->docFile(),
                         fmd->docLine(),
                         getOuterScope()?getOuterScope():container,
                         fmd,
                         fmd->documentation()+"\n",
                         DocOptions()
                         .setIndexWords(true));
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

DString MemberDefImpl::displayDefinition() const
{
  DString ldef = definition();
  DString title = name();
  if (isEnumerate())
  {
    if (isAnonymous())
    {
      ldef = title = "anonymous enum";
      if (!m_enumBaseType.empty())
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
    const ClassDef *annoClassDef=getClassDefOfAnonymousType();
    if (annoClassDef)
    {
      static const reg::Ex ar(R"([\w@:]*@\d+[\w@:]*)");
      reg::Match amatch;
      reg::search(sdef,amatch,ar);
      ldef = amatch.prefix().str() + annoClassDef->displayName() + amatch.suffix().str();
    }
    else
    {
      ldef = match.prefix().str() + " { ... } " + removeAnonymousScopes(match.suffix().str());
    }
  }

  const ClassDef *cd=getClassDef();
  if (cd && cd->isObjectiveC())
  {
    // strip scope name
    if (size_t ep = ldef.find("::"); ep!=DString::npos)
    {
      if (size_t sp=ldef.rfind(' ',ep); sp!=DString::npos)
      {
        ldef=ldef.left(sp+1)+ldef.mid(ep+2);
      }
    }
    // strip keywords
    if (size_t dp = ldef.find(':'); dp!=DString::npos)
    {
      ldef=ldef.left(dp+1);
    }
    int l=static_cast<int>(ldef.length());
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
  DString sep = getLanguageSpecificSeparator(lang,true);
  return substitute(ldef,"::",sep);
}

void MemberDefImpl::_writeGroupInclude(OutputList &ol,bool inGroup) const
{
  // only write out the include file if this is not part of a class or file
  // definition
  bool showGroupedMembInc = Config_getBool(SHOW_GROUPED_MEMB_INC);
  bool forceLocalIncludes = Config_getBool(FORCE_LOCAL_INCLUDES);
  const FileDef *fd = getFileDef();
  DString nm;
  if (inGroup && fd && showGroupedMembInc)
  {
    if (!Config_getList(STRIP_FROM_INC_PATH).empty())
    {
      nm = stripFromIncludePath(fd->absFilePath());
    }
    else
    {
      nm = fd->name();
    }
  }
  if (!nm.empty())
  {
    ol.startParagraph();
    ol.startTypewriter();
    SrcLangExt lang = getLanguage();
    bool isIDLorJava = lang==SrcLangExt::IDL || lang==SrcLangExt::Java;
    if (isIDLorJava)
    {
      ol.docify("import ");
    }
    else
    {
      ol.docify("#include ");
    }

    if (isIDLorJava || forceLocalIncludes) ol.docify("\""); else ol.docify("<");

    if (fd->isLinkable())
    {
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),fd->anchor(),nm);
    }
    else
    {
      ol.docify(nm);
    }

    if (isIDLorJava || forceLocalIncludes) ol.docify("\""); else ol.docify(">");

    ol.endTypewriter();
    ol.endParagraph();
  }
}

void MemberDefImpl::_writeMultiLineInitializer(OutputList &ol,const DString &scopeName) const
{
    //printf("md=%s initLines=%d init='%s'\n",qPrint(name()),initLines,qPrint(init));
    ol.startBold();
    if (m_mtype==MemberType::Define)
      ol.parseText(theTranslator->trDefineValue());
    else
      ol.parseText(theTranslator->trInitialValue());
    ol.endBold();
    DString langCorrected = getDefFileExtension();
    SrcLangExt srcLangExt = getLanguageFromFileName(getDefFileExtension());
    if (srcLangExt == SrcLangExt::Lex)
    {
      langCorrected = ".doxygen_lex_c";
      srcLangExt = SrcLangExt::Cpp;
    }
    auto intf = Doxygen::parserManager->getCodeParser(langCorrected);
    intf->resetCodeParserState();
    auto &codeOL = ol.codeGenerators();
    codeOL.startCodeFragment("DoxyCode");
    intf->parseCode(codeOL,scopeName,m_initializer,srcLangExt,Config_getBool(STRIP_CODE_COMMENTS),
                    CodeParserOptions()
                    .setFileDef(getFileDef())
                    .setInlineFragment(true)
                    .setMemberDef(this)
                    .setShowLineNumbers(false)
                    .setSearchCtx(this)
                   );
    codeOL.endCodeFragment("DoxyCode");
}

/*! Writes the "detailed documentation" section of this member to
 *  all active output formats.
 */
void MemberDefImpl::writeDocumentation(const MemberList *ml,
                                   int memCount,int memTotal,
                                   OutputList &ol,
                                   const DString &scName,
                                   const Definition *container,
                                   bool inGroup,
                                   bool  /* showEnumValues */,
                                   bool showInline
                                  ) const
{
  // if this member is in a group find the real scope name.
  bool hasParameterList = false;

  //printf("MemberDefImpl::writeDocumentation(): name='%s' hasDocs='%d' containerType=%d inGroup=%d sectionLinkable=%d\n",
  //    qPrint(name()),hasDocs,container->definitionType(),inGroup,hasDetailedDescription());

  //if ( !hasDocs ) return;
  //if (isEnumValue() && !showEnumValues) return;

  SrcLangExt lang = getLanguage();
  //printf("member=%s lang=%d\n",qPrint(name()),lang);
  bool optVhdl = lang==SrcLangExt::VHDL;
  DString sep = getLanguageSpecificSeparator(lang,true);

  DString scopeName = scName;
  DString memAnchor = anchor();
  DString ciname = container->displayName();
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

  DString cname   = container->name();
  DString cfname  = getOutputFileBase();

  // get member name
  DString doxyName=name();
  // prepend scope if there is any. TODO: make this optional for C only docs
  if (!scopeName.empty())
  {
    doxyName.prepend(scopeName+sep);
  }
  DString doxyArgs=argsString();

  DString ldef = definition();
  DString title = name();

  //printf("member '%s' def='%s'\n",qPrint(name()),qPrint(ldef));
  if (isEnumerate())
  {
    if (title.at(0)=='@')
    {
      ldef = title = "anonymous enum";
      if (isSliceLocal())
      {
        ldef.prepend("local ");
      }
      if (lang==SrcLangExt::Cpp)
      {
        if (isEnumStruct()) ldef.append(" struct");
        else if (isStrong()) ldef.append(" class");
      }
      if (!m_enumBaseType.empty())
      {
        ldef+=" : "+m_enumBaseType;
      }
    }
    else
    {
      if (lang==SrcLangExt::Cpp)
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
  if (lang == SrcLangExt::Slice)
  {
    // Remove the container scope from the member name.
    DString prefix = scName + sep;
    if (size_t pos = ldef.rfind(prefix); pos != DString::npos)
    {
      ldef.remove(pos, prefix.length());
    }
  }
  ldef.stripPrefix("constexpr ");
  ldef.stripPrefix("consteval ");
  ldef.stripPrefix("constinit ");
  ldef.stripPrefix("static ");

  //----------------------------------------

  ol.pushGeneratorState();

  bool htmlEndLabelTable=false;
  StringVector sl = getLabels(scopedContainer);

  static const reg::Ex r(R"(@\d+)");
  reg::Match match;
  std::string sdef = ldef.str();
  LinkifyTextOptions options;
  options.setScope(scopedContainer).setFileScope(getBodyDef()).setSelf(this);
  if ((isVariable() || isTypedef()) && reg::search(sdef,match,r))
  {
    // find enum type and insert it in the definition
    bool found=false;
    for (const auto &vmd : *ml)
    {
      if (vmd->isEnumerate() && match.str()==vmd->name())
      {
        ol.startDoxyAnchor(cfname, cname, memAnchor, doxyName, doxyArgs);
        ol.startMemberDoc(ciname,name(),memAnchor,name(),memCount,memTotal,showInline);
        ol.addLabel(cfname, memAnchor);
        DString prefix = match.prefix().str();
        DString suffix = match.suffix().str();
        linkifyText(TextGeneratorOLImpl(ol),prefix,options);
        vmd->writeEnumDeclaration(ol,getClassDef(),getNamespaceDef(),getFileDef(),getGroupDef(),getModuleDef());
        linkifyText(TextGeneratorOLImpl(ol),suffix,options);

        found=true;
        break;
      }
    }
    if (!found) // anonymous compound
    {
      const ClassDef *annoClassDef=getClassDefOfAnonymousType();
      DString typeName;
      if (annoClassDef) typeName=annoClassDef->compoundTypeString();
      ol.startDoxyAnchor(cfname, cname, memAnchor, doxyName, doxyArgs);
      ol.startMemberDoc(ciname,name(),memAnchor,title,memCount,memTotal,showInline);
      ol.addLabel(cfname, memAnchor);
      // search for the last anonymous compound name in the definition

      ol.startMemberDocName(isObjCMethod());
      if (reg::search(sdef,match,reAnonymous))
      {
        static const reg::Ex ar(R"([\w@:]*@\d+[\w@:]*)");
        reg::Match amatch;
        reg::search(sdef,amatch,ar);
        DString prefix = amatch.prefix().str();
        DString suffix = removeAnonymousScopes(amatch.suffix().str());
        //printf("writeDocumentation(): anonymous compound in definition: sdef='%s' prefix='%s' suffix='%s' annoClassDef=%p\n",
        //    qPrint(sdef), qPrint(prefix),qPrint(suffix),(void*)annoClassDef);
        const MemberDef *amd = getAnonymousEnumType();
        if (annoClassDef && amd==nullptr)
        {
          ol.docify(prefix);
          DString annTypeName = annoClassDef->displayName(true);
          if (suffix.empty())
          {
            suffix=" "+localName();
          }
          ol.writeObjectLink(annoClassDef->getReference(),annoClassDef->getOutputFileBase(),
              annoClassDef->anchor(),annTypeName);
          linkifyText(TextGeneratorOLImpl(ol),suffix,options);
        }
        else if (amd) // type is an anonymous enum
        {
          DString typePlaceholder = " { ... } ";
          if (lang==SrcLangExt::Cpp)
          {
            if (amd->isEnumStruct()) typePlaceholder.prepend(" struct");
            else if (amd->isStrong()) typePlaceholder.prepend(" class");
          }
          ol.writeObjectLink(amd->getReference(),amd->getOutputFileBase(),amd->anchor(),prefix+typePlaceholder+suffix);
        }
        else
        {
          prefix = match.prefix().str();
          suffix = removeAnonymousScopes(match.suffix().str());
          ol.docify(prefix);
          ol.docify(" { ... } ");
          linkifyText(TextGeneratorOLImpl(ol),suffix,options);
        }
      }
      else
      {
        linkifyText(TextGeneratorOLImpl(ol),ldef,options);
      }
    }
  }
  else // not an enum value or anonymous compound
  {
    ol.startDoxyAnchor(cfname, cname, memAnchor, doxyName, doxyArgs);
    ol.startMemberDoc(ciname,name(),memAnchor,title,memCount,memTotal,showInline);
    ol.addLabel(cfname, memAnchor);

    if (!m_metaData.empty() && getLanguage()==SrcLangExt::Slice)
    {
      ol.startMemberDocPrefixItem();
      ol.docify(m_metaData);
      ol.endMemberDocPrefixItem();
    }

    const ClassDef *cd=getClassDef();
    const NamespaceDef *nd=getNamespaceDef();
    if (!m_defTmpArgLists.empty() && lang==SrcLangExt::Cpp)
      // definition has explicit template parameter declarations
    {
      for (const ArgumentList &tal : m_defTmpArgLists)
      {
        if (!tal.empty())
        {
          ol.startMemberDocPrefixItem();
          _writeTemplatePrefix(ol,scopedContainer,tal);
          ol.endMemberDocPrefixItem();
        }
      }
    }
    else // definition gets it template parameters from its class
      // (since no definition was found)
    {
      if (cd && lang==SrcLangExt::Cpp && !isTemplateSpecialization())
      {
        for (const ArgumentList &tal : cd->getTemplateParameterLists())
        {
          if (!tal.empty())
          {
            ol.startMemberDocPrefixItem();
            _writeTemplatePrefix(ol,scopedContainer,tal,false);
            ol.endMemberDocPrefixItem();
          }
        }
      }
      if (m_tArgList.hasParameters() && lang==SrcLangExt::Cpp) // function template prefix
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
      htmlEndLabelTable=true;
    }

    ol.startMemberDocName(isObjCMethod());
    if (cd && cd->isObjectiveC())
    {
      // strip scope name
      if (size_t ep = ldef.find("::"); ep!=DString::npos)
      {
        if (size_t sp=ldef.rfind(' ',ep); sp!=DString::npos)
        {
          ldef=ldef.left(sp+1)+ldef.mid(ep+2);
        } else {
          ldef=ldef.mid(ep+2);
        }
      }
      // strip keywords
      if (size_t dp = ldef.find(':'); dp!=DString::npos)
      {
        ldef=ldef.left(dp+1);
      }
      int dl=static_cast<int>(ldef.length());
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
    else if (lang==SrcLangExt::Slice)
    {
      // Eliminate the self-reference.
      size_t pos = ldef.rfind(' ');
      if (pos==DString::npos) pos=0;
      if (pos>0)
      {
        linkifyText(TextGeneratorOLImpl(ol),ldef.left(pos),options);
      }
      ol.docify(ldef.mid(pos));
      const Definition *scope = cd;
      if (scope==nullptr) scope = nd;
      hasParameterList=writeDefArgumentList(ol,scope,this);
    }
    else
    {
      linkifyText(TextGeneratorOLImpl(ol),substitute(ldef,"::",sep),options);
      const Definition *scope = cd;
      if (scope==nullptr) scope = nd;
      hasParameterList=writeDefArgumentList(ol,scope,this);
    }

    if (hasOneLineInitializer()) // add initializer
    {
      if (isTypeAlias())
      {
        ol.docify(" = ");
        DString init = m_initializer.simplifyWhiteSpace();
        linkifyText(TextGeneratorOLImpl(ol),init,options);
      }
      else if (!isDefine())
      {
        ol.docify(" ");
        DString init = m_initializer.simplifyWhiteSpace();
        linkifyText(TextGeneratorOLImpl(ol),init,options);
      }
      else
      {
        ol.writeNonBreakableSpace(3);
        linkifyText(TextGeneratorOLImpl(ol),m_initializer,options);
      }
    }
    if (!excpString().empty()) // add exception list
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
      ol.writeLabel(s,count==sl.size());
    }
    ol.endLabels();
  }
  ol.popGeneratorState();

  if (hasParameterList)
  {
    ol.endParameterList();
    ol.endMemberDoc(true);
  }
  else
  {
    ol.endMemberDocName();
    ol.endMemberDoc(false);
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
      ol.writeLabel(s,count==sl.size());
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
  DString brief = briefDescription();
  if (!brief.empty() &&
      (Config_getBool(REPEAT_BRIEF) ||
       !Config_getBool(BRIEF_MEMBER_DESC)
      )
     )
  {
    ol.startParagraph();
    ol.generateDoc(briefFile(),
                   briefLine(),
                   scopedContainer,
                   this,
                   brief,
                   DocOptions()
                   .setSingleLine(true));
    ol.endParagraph();
  }

  /* write detailed description */
  DString detailed = documentation();
  if (!detailed.empty() ||
      !inbodyDocumentation().empty())
  {
    // write vhdl inline code with or without option INLINE_SOURCE
    if (optVhdl && VhdlDocGen::isMisc(this))
    {
      VhdlDocGen::writeSource(this,ol,cname);
      return;
    }
    else
    {
      ol.generateDoc(docFile(),
                     docLine(),
                     scopedContainer,
                     this,
                     detailed+"\n",
                     DocOptions()
                     .setIndexWords(true));
    }

    if (!inbodyDocumentation().empty())
    {
      ol.generateDoc(inbodyFile(),
                     inbodyLine(),
                     scopedContainer,
                     this,
                     inbodyDocumentation()+"\n",
                     DocOptions()
                     .setIndexWords(true));
    }
  }
  else if (!brief.empty() && (Config_getBool(REPEAT_BRIEF) || !Config_getBool(BRIEF_MEMBER_DESC)))
  {
    if (!inbodyDocumentation().empty())
    {
      ol.generateDoc(inbodyFile(),
                     inbodyLine(),
                     scopedContainer,
                     this,
                     inbodyDocumentation()+"\n",
                     DocOptions()
                     .setIndexWords(true));
    }
  }


  //printf("***** defArgList=%p name=%s docs=%s hasDocs=%d\n",
  //     defArgList,
  //     defArgList?defArgList->hasDocumentation():-1);
  const ArgumentList &docArgList = m_templateMaster ?
                                   m_templateMaster->argumentList() :
                                   m_defArgList;
  ol.generateDoc(docFile(),
                 docLine(),
                 scopedContainer,
                 this,         // memberDef
                 inlineArgListToDoc(docArgList),    // docStr
                 DocOptions()
                 .setIndexWords(true));

  const ArgumentList &docTemplateArgList = m_templateMaster ?
                                   m_templateMaster->templateArguments() :
                                   m_tArgList;
  ol.generateDoc(docFile(),
                 docLine(),
                 scopedContainer,
                 this,         // memberDef
                 inlineTemplateArgListToDoc(docTemplateArgList),    // docStr
                 DocOptions()
                 .setIndexWords(true));

  _writeEnumValues(ol,scopedContainer,cfname,ciname,cname);
  _writeReimplements(ol);
  _writeReimplementedBy(ol);
  _writeCategoryRelation(ol);
  _writeExamples(ol);
  _writeTypeConstraints(ol);
  DString scopeStr = getScopeString();
  writeSourceDef(ol);
  writeInlineCode(ol,scopeStr);
  if (hasReferencesRelation()) writeSourceRefs(ol,scopeStr);
  if (hasReferencedByRelation()) writeSourceReffedBy(ol,scopeStr);
  _writeCallGraph(ol);
  _writeCallerGraph(ol);
  if (hasRequirementRefs()) writeRequirementRefs(ol);

  ol.endIndent();

  // enable LaTeX again
  //if (Config_getBool(EXTRACT_ALL) && !hasDocs) ol.enable(OutputType::Latex);
  ol.popGeneratorState();

  warnIfUndocumentedParams();
}

// strip scope and field name from the type
// example: "struct N<K::J>::S.v.c" will become "struct v"
static DString simplifyTypeForTable(const DString &s)
{
  DString ts=removeAnonymousScopes(s);
  if (ts.endsWith("::")) ts = ts.left(ts.length()-2);
  static const reg::Ex re1(R"(\a\w*::)");       // non-template version
  static const reg::Ex re2(R"(\a\w*<[^>]*>::)"); // template version
  reg::Match match;
  std::string t = ts.str();
  while (reg::search(t,match,re2) || reg::search(t,match,re1))
  {
    t = match.prefix().str() + match.suffix().str(); // remove the matched part
  }
  //printf("simplifyTypeForTable(%s)->%s\n",qPrint(s),qPrint(t));
  return t;
}

DString MemberDefImpl::fieldType() const
{
  DString type = m_type;
  if (isTypedef() && getLanguage() != SrcLangExt::Slice) type.prepend("typedef ");
  return simplifyTypeForTable(type);
}

void MemberDefImpl::writeMemberDocSimple(OutputList &ol, const Definition *container) const
{
  Definition *scope  = getOuterScope();
  DString doxyName  = name();
  DString doxyArgs  = argsString();
  DString memAnchor = anchor();
  DString cfname    = getOutputFileBase();
  DString cname;
  if (scope) cname   = scope->name();
  if (doxyName.at(0)=='@')
  {
    doxyName="__unnamed__";
  }

  LinkifyTextOptions options;
  options.setScope(scope).setFileScope(getBodyDef()).setSelf(this);

  if (container && container->definitionType()==Definition::TypeClass &&
      !(toClassDef(container))->isJavaEnum())
  {
    ol.startInlineMemberType();
    ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
    ol.addLabel(cfname,memAnchor);

    bool written=false;
    const ClassDef *annoClassDef=getClassDefOfAnonymousType();
    if (annoClassDef)
    {
      static const reg::Ex ar(R"([\w@:]*@\d+[\w@:]*)"); // anonymous type marker(s) including scope
      reg::Match amatch;
      std::string stype = m_type.str();
      if (reg::search(stype,amatch,ar))
      {
        size_t ai = amatch.position();
        size_t al = amatch.length();
        ol.writeObjectLink(annoClassDef->getReference(),annoClassDef->getOutputFileBase(),
                           annoClassDef->anchor(),m_type.left(ai)+m_type.mid(ai+al));
        written=true;
      }
    }
    if (!written)
    {
      DString ts;
      if (isFunctionPtr())
      {
        ts = m_type + m_args;
      }
      else
      {
        ts = fieldType();
      }
      linkifyText(TextGeneratorOLImpl(ol),ts,options);
    }
    ol.endDoxyAnchor(cfname,memAnchor);
    ol.endInlineMemberType();
  }

  ol.startInlineMemberName();
  ol.docify(doxyName);
  if (isVariable() && !argsString().empty() && !isObjCMethod() && !isFunctionPtr())
  {
    linkifyText(TextGeneratorOLImpl(ol),argsString(),LinkifyTextOptions(options).setArgumentList(&m_defArgList));
  }
  if (!m_bitfields.empty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),m_bitfields,options);
  }
  if (hasOneLineInitializer() && !isDefine())
  {
    ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol),m_initializer.simplifyWhiteSpace(),options);
  }
  ol.endInlineMemberName();

  ol.startInlineMemberDoc();

  DString brief           = briefDescription();
  DString detailed        = documentation();

  /* write brief description */
  if (!brief.empty())
  {
    ol.generateDoc(briefFile(),
                   briefLine(),
                   getOuterScope()?getOuterScope():container,this,
                   brief,
                   DocOptions()
                   .setSingleLine(true));
  }

  /* write detailed description */
  if (!detailed.empty())
  {
    if (!brief.empty())
    {
      ol.disable(OutputType::Html);
      ol.lineBreak();
      ol.enable(OutputType::Html);
    }
    ol.generateDoc(docFile(),
                   docLine(),
                   getOuterScope()?getOuterScope():container,this,
                   detailed+"\n",
                   DocOptions());
  }

  ol.endInlineMemberDoc();
}

DString MemberDefImpl::memberTypeName() const
{
  if (m_mtype == MemberType::Define)
  {
    return "macro definition";
  }
  else if (m_mtype == MemberType::Enumeration)
  {
    return "enumeration";
  }
  return to_string_lower(m_mtype);
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
  const Definition *d=nullptr;
  DString t;
  if (cd)
  {
    t=cd->compoundTypeString();
    d=cd;
  }
  else if (nd)
  {
    t=nd->compoundTypeString();
    d=nd;
  }
  else if (gd)
  {
    t="group";
    d=gd;
  }
  else
  {
    t="file";
    d=fd;
  }
  bool extractAll = Config_getBool(EXTRACT_ALL);

  //printf("%s:warnIfUndoc: hasUserDocs=%d isFriendClass=%d protection=%d isRef=%d isDel=%d\n",
  //    qPrint(name()),
  //    hasUserDocumentation(),isFriendClass(),protectionLevelVisible(m_prot),isReference(),isDeleted());
  if ((!hasUserDocumentation() && !extractAll) &&
      !isFriendClass() &&
      name().find('@')==DString::npos && d && d->name().find('@')==DString::npos &&
      !_isAnonymousBitField() &&
      protectionLevelVisible(m_prot) &&
      !isReference() && !isDeleted()
     )
  {
    SrcLangExt lang = getLanguage();
    DString sep = getLanguageSpecificSeparator(lang,true);
    warn_undoc(getDefFileName(),getDefLine(),"Member {}{} ({}) of {} {} is not documented.",
         name(),argsString(),memberTypeName(),t,
         substitute(d->name(),"::",sep));
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
        DString sep = getLanguageSpecificSeparator(lang,true);
        warn(fmd->getDefFileName(),fmd->getDefLine(), "Documentation for enum member '{}{}{}' is missing.",
             qualifiedName(),sep,fmd->name());
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

static DString stripTrailingReturn(const DString &trailRet)
{
  DString ret = trailRet;

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
  //printf("%s:detectUndocumentedParams(%d,%d)\n",qPrint(name()),hasParamCommand,hasReturnCommand);
  bool isPython = getLanguage()==SrcLangExt::Python;

  // this function is called while parsing the documentation. A member can have multiple
  // documentation blocks, which could be handled by multiple threads, hence this guard.
  std::lock_guard<std::mutex> lock(g_detectUndocumentedParamsMutex);

  if (!m_hasDocumentedParams && hasParamCommand)
  {
    //printf("%s:hasDocumentedParams=true;\n",qPrint(name()));
    m_hasDocumentedParams = true;
  }
  else if (!m_hasDocumentedParams)
  {
    const ArgumentList &al     = argumentList();
    const ArgumentList &declAl = declArgumentList();
    bool allDoc=true; // no parameter => all parameters are documented
    if ( // member has parameters
        al.hasParameters()  // with at least one parameter (that is not void)
       )
    {
      // see if all parameters have documentation
      for (auto it = al.begin(); it!=al.end() && allDoc; ++it)
      {
        const Argument &a = *it;
        if (!a.name.empty() && a.type!="void" && a.name!="..." &&
            !(isPython && (a.name=="self" || a.name=="cls"))
           )
        {
          allDoc = !a.docs.empty();
        }
        //printf("a.type=%s a.name=%s doc=%s\n",
        //        qPrint(a.type),qPrint(a.name),qPrint(a.docs));
      }
      if (!allDoc && declAl.empty()) // try declaration arguments as well
      {
        allDoc=true;
        for (auto it = al.begin(); it!=al.end() && allDoc; ++it)
        {
          const Argument &a = *it;
          if (!a.name.empty() && a.type!="void" && a.name!="..." &&
              !(isPython && (a.name=="self" || a.name=="cls"))
             )
          {
            allDoc = !a.docs.empty();
          }
          //printf("a.name=%s doc=%s\n",qPrint(a.name),qPrint(a.docs));
        }
      }
    }
    if (allDoc)
    {
      //printf("%s:hasDocumentedParams=true;\n",qPrint(name()));
      m_hasDocumentedParams = true;
    }
  }

  //printf("Member %s hasDocumentedReturnType=%d hasReturnCommand=%d\n",
  //    qPrint(name()),m_hasDocumentedReturnType,hasReturnCommand);
  m_hasDocumentedReturnType = m_hasDocumentedReturnType || hasReturnCommand;
}

void MemberDefImpl::warnIfUndocumentedParams() const
{
  DString returnType = typeString();
  bool isFortran = getLanguage()==SrcLangExt::Fortran;
  bool isFortranSubroutine = isFortran && returnType.find("subroutine")!=DString::npos;

  bool isVoidReturn =   returnType=="void" || returnType.endsWith(" void");
  if (!isVoidReturn && (returnType=="auto" || returnType.endsWith(" auto")))
  {
    const ArgumentList &defArgList=isDocsForDefinition() ?  argumentList() : declArgumentList();
    if (!defArgList.trailingReturnType().empty())
    {
      DString strippedTrailingReturn = stripTrailingReturn(defArgList.trailingReturnType());
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
          "parameters of member {} are not documented",
          qualifiedName());
    }
    if (!m_hasDocumentedReturnType &&
        hasDocumentation() && !returnType.empty() &&
        !( // not one of the cases where nothing is returned
          isVoidReturn        || // void return type
          isFortranSubroutine || // fortran subroutine
          isConstructor()     || // a constructor
          isDestructor()      || // or a destructor
          isFriend()             // or a friend
         )
       )
    {
      warn_doc_error(docFile(),docLine(),
          "return type of member {} is not documented",
          qualifiedName());
    }
  }
  if (Config_getBool(WARN_IF_DOC_ERROR) &&
            m_hasDocumentedReturnType &&
            (isVoidReturn        || // void return type
             isFortranSubroutine || // fortran subroutine
             isConstructor()     || // a constructor
             isDestructor()))       // or destructor
  {
    warn_doc_error(docFile(),docLine(),"found documented return type for {} that does not return anything",
                   qualifiedName());
  }
}

bool MemberDefImpl::isDocumentedFriendClass() const
{
  ClassDef *fcd=nullptr;
  DString baseName=name();
  if (size_t i=baseName.find('<'); i!=DString::npos) baseName=baseName.left(i);
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
         (m_mtype==MemberType::Enumeration && m_docEnumValues) ||  // has enum values
         (m_defArgList.hasDocumentation()|| m_tArgList.hasTemplateDocumentation());   // has doc (template) arguments
}


void MemberDefImpl::setMemberGroup(MemberGroup *grp)
{
  m_memberGroup = grp;
}

DString MemberDefImpl::getScopeString() const
{
  DString result;
  if (isStrong()) result=name();
  else if (getClassDef()) result=getClassDef()->displayName();
  else if (getNamespaceDef()) result=getNamespaceDef()->displayName();
  return result;
}

void MemberDefImpl::setAnchor()
{
  DString memAnchor = name();
  if (!m_args.empty()) memAnchor+=m_args;
  if (m_memSpec.isAlias()) // this is for backward compatibility
  {
    memAnchor.prepend(" =  "+m_initializer);
  }
  memAnchor.prepend(definition()); // actually the method name is now included
            // twice, which is silly, but we keep it this way for backward
            // compatibility.

  // include number of template arguments as well,
  // to distinguish between two template
  // specializations that only differ in the template parameters.
  if (m_tArgList.hasParameters())
  {
    char buf[20];
    snprintf(buf,20,"%d:",static_cast<int>(m_tArgList.size()));
    buf[19]='\0';
    memAnchor.prepend(buf);
  }
  if (!m_requiresClause.empty())
  {
    memAnchor+=" "+m_requiresClause;
  }
  if (m_redefineCount>0)
  {
    char buf[20];
    snprintf(buf,20,":%d",m_redefineCount);
    buf[19]='\0';
    memAnchor.append(buf);
  }

  // convert to md5 hash
  uint8_t md5_sig[16];
  char sigStr[33];
  MD5Buffer(memAnchor.data(),static_cast<unsigned int>(memAnchor.length()),md5_sig);
  MD5SigToString(md5_sig,sigStr);
  m_anc = DString("a")+sigStr;
}

void MemberDefImpl::setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                            const DString &fileName,int startLine,
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

void MemberDefImpl::setEnumScope(MemberDef *md,bool livesInsideEnum)
{
  m_enumScope=md;
  m_livesInsideEnum=livesInsideEnum;
  if (md->getGroupDef())
  {
    m_group           = md->getGroupDef();
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
  setOuterScope(cd);
}

void MemberDefImpl::setNamespace(NamespaceDef *nd)
{
  m_nspace=nd;
  setOuterScope(nd);
}

std::unique_ptr<MemberDef> MemberDefImpl::createTemplateInstanceMember(
        const ArgumentList &formalArgs,const std::unique_ptr<ArgumentList> &actualArgs) const
{
  //printf("  Member %s %s %s\n",qPrint(typeString()),qPrint(name()),qPrint(argsString()));
  std::unique_ptr<ArgumentList> actualArgList = std::make_unique<ArgumentList>(m_defArgList);
  if (!m_defArgList.empty())
  {
    // replace formal arguments with actuals
    for (Argument &arg : *actualArgList)
    {
      arg.type = substituteTemplateArgumentsInString(arg.type,formalArgs,actualArgs.get());
    }
    actualArgList->setTrailingReturnType(
       substituteTemplateArgumentsInString(actualArgList->trailingReturnType(),formalArgs,actualArgs.get()));
  }

  DString methodName=name();
  if (methodName.startsWith("operator ")) // conversion operator
  {
    methodName=substituteTemplateArgumentsInString(methodName,formalArgs,actualArgs.get());
  }

  auto imd = createMemberDef(
                       getDefFileName(),getDefLine(),getDefColumn(),
                       substituteTemplateArgumentsInString(m_type,formalArgs,actualArgs.get()),
                       methodName,
                       substituteTemplateArgumentsInString(m_args,formalArgs,actualArgs.get()),
                       m_exception, m_prot,
                       m_virt, m_stat, m_related, m_mtype,
                       ArgumentList(), ArgumentList(), ""
                   );
  auto mmd = toMemberDefMutable(imd.get());
  mmd->moveArgumentList(std::move(actualArgList));
  mmd->setDefinition(substituteTemplateArgumentsInString(m_def,formalArgs,actualArgs.get()));
  mmd->setBodyDef(getBodyDef());
  mmd->setBodySegment(getDefLine(),getStartBodyLine(),getEndBodyLine());
  mmd->setFormalTemplateArguments(formalArgs);

  // TODO: init other member variables (if needed).
  // TODO: reimplemented info
  return imd;
}

bool MemberDefImpl::hasOneLineInitializer() const
{
  //printf("%s: init=%s, initLines=%d maxInitLines=%d userInitLines=%d\n",
  //    qPrint(name()),qPrint(m_initializer),m_initLines,
  //    m_maxInitLines,m_userInitLines);
  bool isFuncLikeMacro = m_mtype==MemberType::Define && m_defArgList.hasParameters();
  return !m_initializer.empty() && m_initLines==0 && // one line initializer
         !isFuncLikeMacro &&
         ((m_maxInitLines>0 && m_userInitLines==-1) || m_userInitLines>0); // enabled by default or explicitly
}

bool MemberDefImpl::hasMultiLineInitializer() const
{
  //printf("initLines=%d userInitLines=%d maxInitLines=%d\n",
  //    initLines,userInitLines,maxInitLines);
  bool isFuncLikeMacro = m_mtype==MemberType::Define && m_defArgList.hasParameters();
  return (m_initLines>0 || (!m_initializer.empty() && isFuncLikeMacro)) &&
         ((m_initLines<m_maxInitLines && m_userInitLines==-1) // implicitly enabled
          || m_initLines<m_userInitLines // explicitly enabled
         );
}

void MemberDefImpl::setInitializer(const DString &initializer)
{
  size_t indent=0;
  m_initializer=detab(initializer,indent);
  int l=static_cast<int>(m_initializer.length());
  int p=l-1;
  while (p>=0 && isspace(static_cast<uint8_t>(m_initializer.at(p)))) p--;
  m_initializer=m_initializer.left(p+1);
  m_initLines=m_initializer.contains('\n');
  stripIndentationVerbatim(m_initializer,indent, false);

  //printf("%s::setInitializer(%s)\n",qPrint(name()),qPrint(m_initializer));
}

void MemberDefImpl::addListReference(const Definition *)
{
  bool optimizeOutputForC = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
  SrcLangExt lang = getLanguage();
  if (!isLinkableInProject()) return;
  DString memLabel;
  if (optimizeOutputForC)
  {
    memLabel=theTranslator->trGlobal(true,true);
  }
  else if (lang==SrcLangExt::Fortran)
  {
    memLabel=theTranslator->trSubprogram(true,true);
  }
  else
  {
    memLabel=theTranslator->trMember(true,true);
  }
  DString memName = name();
  Definition *pd=getOuterScope();
  DString pdName = pd->definitionType()==Definition::TypeClass ?
                    (toClassDef(pd))->displayName() : pd->name();
  DString sep = getLanguageSpecificSeparator(lang,true);
  DString memArgs;
  if (!isRelated())
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
  if (sep!="::")
  {
    memName = substitute(memName,"::",sep);
  }
  addRefItem(xrefItems,
        qualifiedName()+argsString(), // argsString is needed for overloaded functions (see bug 609624)
        memLabel,
        getOutputFileBase()+"#"+anchor(),memName,memArgs,pd);
}

void MemberDefImpl::addRequirementReferences(const Definition *)
{
  RequirementManager::instance().addRequirementRefsForSymbol(this);
}

const MemberList *MemberDefImpl::getSectionList(const Definition *container) const
{
  auto it = m_sectionMap.find(container);
  return it!=m_sectionMap.end() ? it->second : nullptr;
}

void MemberDefImpl::setSectionList(const Definition *container,const MemberList *sl)
{
  //printf("MemberDefImpl::setSectionList(%s,%p) name=%s\n",qPrint(d->name()),sl,qPrint(name()));
  m_sectionMap.emplace(container,sl);
}

Specifier MemberDefImpl::virtualness(int count) const
{
  if (count>25)
  {
     warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: recursion detected in overload relation for member {}!",name());
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
  if (m_mtype == MemberType::Enumeration)
  {
    tagFile << "enumeration";
  }
  else
  {
    tagFile << to_string_lower(m_mtype);
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
  if (typeString()!=DString("@"))
  {
    tagFile << "      <type>" << convertToXML(typeString()) << "</type>\n";
  }
  DString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "      <name>" << convertToXML(useQualifiedName ? qualifiedName() : name()) << "</name>\n";
  tagFile << "      <anchorfile>" << convertToXML(fn) << "</anchorfile>\n";
  tagFile << "      <anchor>" << convertToXML(anchor()) << "</anchor>\n";
  DString idStr = id();
  if (!idStr.empty())
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
        if (!idStr.empty())
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
  m_isConstructorCached=1; // false
  if (getClassDef())
  {
    if (m_isDMember) // for D
    {
      m_isConstructorCached = name()=="this" ? 2 : 1;
      return;
    }
    else if (getLanguage()==SrcLangExt::PHP) // for PHP
    {
      m_isConstructorCached = name()=="__construct" ? 2 : 1;
      return;
    }
    else if (name()=="__init__" &&
             getLanguage()==SrcLangExt::Python) // for Python
    {
      m_isConstructorCached = 2; // true
      return;
    }
    else // for other languages
    {
      DString locName = getClassDef()->localName();
      size_t i=locName.find('<');
      if (i==DString::npos) // not a template class
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
  bool isDestructor = false;
  if (m_isDMember) // for D
  {
    isDestructor = name()=="~this";
  }
  else if (getLanguage()==SrcLangExt::PHP) // for PHP
  {
    isDestructor = name()=="__destruct";
  }
  else if (name()=="__del__" &&
           getLanguage()==SrcLangExt::Python) // for Python
  {
    isDestructor = true;
  }
  else if (getLanguage()==SrcLangExt::Fortran) // for Fortran
  {
    isDestructor = typeString()=="final";
  }
  else // other languages
  {
    isDestructor =
           (name().find('~')!=DString::npos || name().find('!')!=DString::npos)  // The ! is for C++/CLI
           && name().find("operator")==DString::npos;
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

  DString n = name();
  if (size_t i=n.rfind("::"); i!=DString::npos) n=n.mid(i+2); // strip scope (TODO: is this needed?)
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
  if (!m_enumBaseType.empty())
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
          fmd=nullptr;
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

void MemberDefImpl::setType(const DString &t)
{
  m_type = t;
}

void MemberDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
  docFindSections(inbodyDocumentation(),this,docFile());
}

void MemberDefImpl::overrideCallGraph(bool e)
{
  m_hasCallGraph=e;
  if (e) Doxygen::parseSourcesNeeded = true;
}

void MemberDefImpl::overrideCallerGraph(bool e)
{
  m_hasCallerGraph=e;
  if (e) Doxygen::parseSourcesNeeded = true;
}

void MemberDefImpl::overrideReferencedByRelation(bool e)
{
  m_hasReferencedByRelation=e;
  if (e) Doxygen::parseSourcesNeeded = true;
}

void MemberDefImpl::overrideReferencesRelation(bool e)
{
  m_hasReferencesRelation=e;
  if (e) Doxygen::parseSourcesNeeded = true;
}

void MemberDefImpl::overrideEnumValues(bool e)
{
  m_hasEnumValues=e;
}

void MemberDefImpl::overrideInlineSource(bool e)
{
  m_hasInlineSource=e;
}

bool MemberDefImpl::isObjCMethod() const
{
  if (getClassDef() && getClassDef()->isObjectiveC() && isFunction()) return true;
  return false;
}

bool MemberDefImpl::isObjCProperty() const
{
  if (getClassDef() && getClassDef()->isObjectiveC() && isProperty()) return true;
  return false;
}

bool MemberDefImpl::isCSharpProperty() const
{
  if (getClassDef() && getClassDef()->isCSharp() && isProperty()) return true;
  return false;
}

DString MemberDefImpl::qualifiedName() const
{
  if (isObjCMethod())
  {
    DString qm;
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

DString MemberDefImpl::objCMethodName(bool localLink,bool showStatic) const
{
  DString qm;
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

DString MemberDefImpl::declaration() const
{
  return m_decl;
}

DString MemberDefImpl::definition() const
{
  return m_def;
}

DString MemberDefImpl::extraTypeChars() const
{
  return m_extraTypeChars;
}

DString MemberDefImpl::typeString() const
{
  return m_type;
}

DString MemberDefImpl::argsString() const
{
  return m_args;
}

DString MemberDefImpl::excpString() const
{
  return m_exception;
}

DString MemberDefImpl::bitfieldString() const
{
  return m_bitfields;
}

const DString &MemberDefImpl::initializer() const
{
  return m_initializer;
}

int MemberDefImpl::initializerLines() const
{
  return m_initLines;
}

bool MemberDefImpl::isDocTransferDone() const
{
  return m_docTransferDone;
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

DString MemberDefImpl::getReadAccessor() const
{
  return m_read;
}

DString MemberDefImpl::getWriteAccessor() const
{
  return m_write;
}

const GroupDef *MemberDefImpl::getGroupDef() const
{
  return m_group;
}

GroupDef *MemberDefImpl::getGroupDef()
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

DString MemberDefImpl::getGroupFileName() const
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
  return m_mtype==MemberType::Signal;
}

bool MemberDefImpl::isSlot() const
{
  return m_mtype==MemberType::Slot;
}

bool MemberDefImpl::isVariable() const
{
  return m_mtype==MemberType::Variable;
}

bool MemberDefImpl::isEnumerate() const
{
  return m_mtype==MemberType::Enumeration;
}

bool MemberDefImpl::isEnumValue() const
{
  return m_mtype==MemberType::EnumValue;
}

bool MemberDefImpl::isTypedef() const
{
  return m_mtype==MemberType::Typedef;
}

bool MemberDefImpl::isSequence() const
{
  return m_mtype==MemberType::Sequence;
}

bool MemberDefImpl::isDictionary() const
{
  return m_mtype==MemberType::Dictionary;
}

bool MemberDefImpl::isFunction() const
{
  return m_mtype==MemberType::Function;
}

bool MemberDefImpl::isFunctionPtr() const
{
  return m_mtype==MemberType::Variable &&
         DString(argsString()).find(")(")!=DString::npos;
}

bool MemberDefImpl::isDefine() const
{
  return m_mtype==MemberType::Define;
}

bool MemberDefImpl::isFriend() const
{
  return m_mtype==MemberType::Friend;
}

bool MemberDefImpl::isDCOP() const
{
  return m_mtype==MemberType::DCOP;
}

bool MemberDefImpl::isProperty() const
{
  return m_mtype==MemberType::Property;
}

bool MemberDefImpl::isEvent() const
{
  return m_mtype==MemberType::Event;
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

bool MemberDefImpl::isThreadLocal() const
{
  return m_memSpec.isThreadLocal();
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
  return m_mtype==MemberType::EnumValue &&
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
         isObjCMethod() ||
         isFriend();
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

bool MemberDefImpl::isNoDiscard() const
{
  return m_memSpec.isNoDiscard();
}

bool MemberDefImpl::isConstExpr() const
{
  return m_memSpec.isConstExpr();
}

bool MemberDefImpl::isConstEval() const
{
  return m_memSpec.isConstEval();
}

bool MemberDefImpl::isConstInit() const
{
  return m_memSpec.isConstInit();
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
    if (decA.name.empty())
    {
      if (!defA.name.empty())
      {
        decA.name = defA.name;
      }
      else if (!decAS.name.empty())
      {
        decA.name = decAS.name;
      }
      else if (!defAS.name.empty())
      {
        decA.name = defAS.name;
      }
    }
    if (defA.name.empty())
    {
      if (!decA.name.empty())
      {
        defA.name = decA.name;
      }
      else if (!decAS.name.empty())
      {
        defA.name = decAS.name;
      }
      else if (!defAS.name.empty())
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
    bool trivial = DotCallGraph::isTrivial(this,false);
    return !trivial;
  }
  return false;
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
    bool trivial = DotCallGraph::isTrivial(this,true);
    return !trivial;
  }
  return false;
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

bool MemberDefImpl::hasEnumValues() const
{
  return m_hasEnumValues;
}

const MemberDef *MemberDefImpl::templateMaster() const
{
  return m_templateMaster;
}

std::optional<ArgumentList> MemberDefImpl::formalTemplateArguments() const
{
  return m_formalTemplateArguments;
}

bool MemberDefImpl::isTypedefValCached() const
{
  return m_isTypedefValCached;
}

const ClassDef *MemberDefImpl::getCachedTypedefVal() const
{
  return m_cachedTypedefValue;
}

DString MemberDefImpl::getCachedTypedefTemplSpec() const
{
  return m_cachedTypedefTemplSpec;
}

DString MemberDefImpl::getCachedResolvedTypedef() const
{
  //printf("MemberDefImpl::getCachedResolvedTypedef()=%s\n",qPrint(m_cachedResolvedType));
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

DString MemberDefImpl::getDeclFileName() const
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

void MemberDefImpl::setDefinition(const DString &d)
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

void MemberDefImpl::setBitfields(const DString &s)
{
  m_bitfields = DString(s).simplifyWhiteSpace();
}

void MemberDefImpl::setMaxInitLines(int lines)
{
  if (lines!=-1)
  {
    m_userInitLines=lines;
  }
}

void MemberDefImpl::setReadAccessor(const DString &r)
{
  m_read=r;
}

void MemberDefImpl::setWriteAccessor(const DString &w)
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

void MemberDefImpl::setArgsString(const DString &as)
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

void MemberDefImpl::setPrototype(bool p,const DString &df,int line,int column)
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

void MemberDefImpl::setExplicitExternal(bool b,const DString &df,int line,int column)
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

void MemberDefImpl::setDeclFile(const DString &df,int line,int column)
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
  m_implOnly=true;
}

void MemberDefImpl::setTemplateMaster(const MemberDef *mt)
{
  m_templateMaster=mt;
  m_isLinkableCached = 0;
}

void MemberDefImpl::setFormalTemplateArguments(const ArgumentList &al)
{
  m_formalTemplateArguments = al;
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
  m_isTypedefValCached=false;
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

void MemberDefImpl::setEnumBaseType(const DString &type)
{
  m_enumBaseType = type;
}

DString MemberDefImpl::enumBaseType() const
{
  return m_enumBaseType;
}

void MemberDefImpl::setRequiresClause(const DString &req)
{
  m_requiresClause = req;
}

DString MemberDefImpl::requiresClause() const
{
  return m_requiresClause;
}

void MemberDefImpl::cacheTypedefVal(const ClassDef*val, const DString & templSpec, const DString &resolvedType)
{
  m_isTypedefValCached=true;
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
      if (!argSrc.name.empty())
      {
        argDst.name = argSrc.name;
      }
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
      if (!argSrc.name.empty())
      {
        argDst.name = argSrc.name;
      }
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
    a.canType.clear();
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

DString MemberDefImpl::displayName(bool) const
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
    if (decA.docs.empty() && !defA.docs.empty())
    {
      decA.docs = defA.docs;
    }
    else if (defA.docs.empty() && !decA.docs.empty())
    {
      defA.docs = decA.docs;
    }
    //printf("transferArgumentDocumentation(%s<->%s)\n",qPrint(decA.name),qPrint(defA.name));
    if (Config_getBool(RESOLVE_UNNAMED_PARAMS))
    {
      if (decA.name.empty() && !defA.name.empty())
      {
        decA.name = defA.name;
      }
      else if (defA.name.empty() && !decA.name.empty())
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
      (!mdef->isDocTransferDone() || !mdec->isDocTransferDone()) &&
      (
       (mdef->isFunction() && !mdef->isStatic() && !mdef->isPrototype()) ||
       (mdef->isVariable() && !mdef->isExternal() && !mdef->isStatic())
      )
     )
  {
    bool sameNumTemplateArgs = mdef->templateArguments().size()==mdec->templateArguments().size();

    ArgumentList &mdefAl = const_cast<ArgumentList&>(mdef->argumentList());
    ArgumentList &mdecAl = const_cast<ArgumentList&>(mdec->argumentList());
    if (sameNumTemplateArgs &&
        matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),mdef->typeString(),&mdefAl,
                        mdec->getOuterScope(),mdec->getFileDef(),mdec->typeString(),&mdecAl,
                        true,mdef->getLanguage()
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

      // copy brief description between definition and declaration
      DString mdefBrief     = mdef->briefDescription();
      DString mdecBrief     = mdec->briefDescription();
      DString mdefBriefFile = mdef->briefFile();
      DString mdecBriefFile = mdec->briefFile();
      int mdefBriefLine      = mdef->briefLine();
      int mdecBriefLine      = mdec->briefLine();
      if (!mdef->isDocTransferDone() && !mdecBrief.empty())
      {
        mdef->setBriefDescription(mdecBrief,mdecBriefFile,mdecBriefLine);
      }
      if (!mdec->isDocTransferDone() && !mdefBrief.empty())
      {
        mdec->setBriefDescription(mdefBrief,mdefBriefFile,mdefBriefLine);
      }

      // copy detailed description between definition and declaration
      DString mdefDocs   = mdef->documentation();
      DString mdecDocs   = mdec->documentation();
      DString mdefFile   = mdef->docFile();
      DString mdecFile   = mdec->docFile();
      int mdefLine        = mdef->docLine();
      int mdecLine        = mdec->docLine();
      bool mdefDocsForDef = mdef->isDocsForDefinition();
      bool mdecDocsForDef = mdec->isDocsForDefinition();
      if (!mdec->isDocTransferDone() && !mdefDocs.empty())
      {
        //printf("transferring docs mdef->mdec (%s->%s)\n",mdef->argsString(),mdec->argsString());
        mdec->setDocumentation(mdefDocs,mdefFile,mdefLine);
        mdec->setDocsForDefinition(mdefDocsForDef);
        if (mdefAl.hasParameters())
        {
          auto mdefAlComb = stringToArgumentList(mdef->getLanguage(),mdef->argsString());
          transferArgumentDocumentation(mdefAl,*mdefAlComb);
          mdec->moveArgumentList(std::move(mdefAlComb));
        }
      }
      if (!mdef->isDocTransferDone() && !mdecDocs.empty())
      {
        //printf("transferring docs mdec->mdef (%s->%s)\n",mdec->argsString(),mdef->argsString());
        mdef->setDocumentation(mdecDocs,mdecFile,mdecLine);
        mdef->setDocsForDefinition(mdecDocsForDef);
        if (mdecAl.hasParameters())
        {
          auto mdecAlComb = stringToArgumentList(mdec->getLanguage(),mdec->argsString());
          transferArgumentDocumentation(mdecAl,*mdecAlComb);
          mdef->moveDeclArgumentList(std::move(mdecAlComb));
        }
      }

      // copy inbody documentation between definition and declaration
      DString mdefInbodyDocs = mdef->inbodyDocumentation();
      DString mdecInbodyDocs = mdec->inbodyDocumentation();
      DString mdefInbodyFile = mdef->inbodyFile();
      DString mdecInbodyFile = mdec->inbodyFile();
      int mdefInbodyLine      = mdef->inbodyLine();
      int mdecInbodyLine      = mdec->inbodyLine();
      if (!mdec->isDocTransferDone() && !mdefInbodyDocs.empty())
      {
        mdec->setInbodyDocumentation(mdefInbodyDocs,mdefInbodyFile,mdefInbodyLine);
      }
      if (!mdef->isDocTransferDone() && !mdecInbodyDocs.empty())
      {
        mdef->setInbodyDocumentation(mdecInbodyDocs,mdecInbodyFile,mdecInbodyLine);
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
      if (mdec->getGroupDef()==nullptr && mdef->getGroupDef()!=nullptr)
      {
        mdec->setGroupDef(mdef->getGroupDef(),
            mdef->getGroupPri(),
            mdef->docFile(),
            mdef->docLine(),
            mdef->hasDocumentation(),
            mdef
            );
      }
      else if (mdef->getGroupDef()==nullptr && mdec->getGroupDef()!=nullptr)
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

      mergeMemberOverrideOptions(mdec,mdef);

      mdef->addQualifiers(mdec->getQualifiers());
      mdec->addQualifiers(mdef->getQualifiers());

      mdef->setDocTransferDone();
      mdec->setDocTransferDone();
    }
  }
}

DString MemberDefImpl::briefDescription(bool abbr) const
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

DString MemberDefImpl::documentation() const
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

bool MemberDefImpl::hasUserDocumentation() const
{
  if (m_templateMaster)
  {
    return m_templateMaster->hasUserDocumentation();
  }
  else
  {
    return DefinitionMixin::hasUserDocumentation();
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
    case MemberType::Define:      return CodeSymbolType::Define;
    case MemberType::Function:    return CodeSymbolType::Function;
    case MemberType::Variable:    return CodeSymbolType::Variable;
    case MemberType::Typedef:     return CodeSymbolType::Typedef;
    case MemberType::Enumeration: return CodeSymbolType::Enumeration;
    case MemberType::EnumValue:   return CodeSymbolType::EnumValue;
    case MemberType::Signal:      return CodeSymbolType::Signal;
    case MemberType::Slot:        return CodeSymbolType::Slot;
    case MemberType::Friend:      return CodeSymbolType::Friend;
    case MemberType::DCOP:        return CodeSymbolType::DCOP;
    case MemberType::Property:    return CodeSymbolType::Property;
    case MemberType::Event:       return CodeSymbolType::Event;
    case MemberType::Interface:   return CodeSymbolType::Interface;
    case MemberType::Service:     return CodeSymbolType::Service;
    case MemberType::Sequence:    return CodeSymbolType::Sequence;
    case MemberType::Dictionary:  return CodeSymbolType::Dictionary;
  }
  return CodeSymbolType::Default;
}

int MemberDefImpl::redefineCount() const
{
  return m_redefineCount;
}

void MemberDefImpl::setRedefineCount(int count)
{
  m_redefineCount=count;
}

//-------------------------------------------------------------------------------
// Helpers

static std::mutex g_docCrossReferenceMutex;

void addDocCrossReference(const MemberDef *s,const MemberDef *d)
{
  MemberDefMutable *src = toMemberDefMutable(const_cast<MemberDef*>(s));
  MemberDefMutable *dst = toMemberDefMutable(const_cast<MemberDef*>(d));
  if (src==nullptr || dst==nullptr) return;
  //printf("--> addDocCrossReference src=%s,dst=%s\n",qPrint(src->name()),qPrint(dst->name()));
  if (dst->isTypedef() || dst->isEnumerate()) return; // don't add types
  if ((dst->hasReferencedByRelation() || dst->hasCallerGraph()) &&
      src->isCallable()
     )
  {
    DString sourceRefName = src->sourceRefName();
    MemberDefMutable *mdDef = toMemberDefMutable(dst->memberDefinition());
    MemberDefMutable *mdDecl = toMemberDefMutable(dst->memberDeclaration());

    // ---- critical section
    std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
    dst->addSourceReferencedBy(src,sourceRefName);
    if (mdDef)
    {
      mdDef->addSourceReferencedBy(src,sourceRefName);
    }
    if (mdDecl)
    {
      mdDecl->addSourceReferencedBy(src,sourceRefName);
    }
    // ---- end critical section
  }
  if ((src->hasReferencesRelation() || src->hasCallGraph()) &&
      src->isCallable()
     )
  {
    DString sourceRefName = dst->sourceRefName();
    MemberDefMutable *mdDef = toMemberDefMutable(src->memberDefinition());
    MemberDefMutable *mdDecl = toMemberDefMutable(src->memberDeclaration());

    // ---- critical section
    std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
    src->addSourceReferences(dst,sourceRefName);
    if (mdDef)
    {
      mdDef->addSourceReferences(dst,sourceRefName);
    }
    if (mdDecl)
    {
      mdDecl->addSourceReferences(dst,sourceRefName);
    }
    // ---- end critical section
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
    return nullptr;
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
    return nullptr;
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
    return nullptr;
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
    return nullptr;
  }
}


