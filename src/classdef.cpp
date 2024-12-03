/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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

#include <cstdio>
#include <algorithm>

#include "types.h"
#include "classdef.h"
#include "classlist.h"
#include "entry.h"
#include "doxygen.h"
#include "membername.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "htmlhelp.h"
#include "example.h"
#include "outputlist.h"
#include "dot.h"
#include "dotclassgraph.h"
#include "dotrunner.h"
#include "defargs.h"
#include "debug.h"
#include "docparser.h"
#include "searchindex.h"
#include "vhdldocgen.h"
#include "layout.h"
#include "arguments.h"
#include "memberlist.h"
#include "groupdef.h"
#include "filedef.h"
#include "namespacedef.h"
#include "membergroup.h"
#include "definitionimpl.h"
#include "symbolresolver.h"
#include "fileinfo.h"
#include "trace.h"
#include "moduledef.h"

//-----------------------------------------------------------------------------

static QCString makeQualifiedNameWithTemplateParameters(const ClassDef *cd,
    const ArgumentLists *actualParams,uint32_t *actualParamIndex)
{
  //bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  bool hideScopeNames = Config_getBool(HIDE_SCOPE_NAMES);
  //printf("qualifiedNameWithTemplateParameters() localName=%s\n",qPrint(cd->localName()));
  QCString scName;
  const Definition *d=cd->getOuterScope();
  if (d)
  {
    if (d->definitionType()==Definition::TypeClass)
    {
      const ClassDef *ocd=toClassDef(d);
      scName = ocd->qualifiedNameWithTemplateParameters(actualParams,actualParamIndex);
    }
    else if (!hideScopeNames)
    {
      scName = d->qualifiedName();
    }
  }

  SrcLangExt lang = cd->getLanguage();
  QCString scopeSeparator = getLanguageSpecificSeparator(lang);
  if (!scName.isEmpty()) scName+=scopeSeparator;

  bool isSpecialization = cd->localName().find('<')!=-1;

  QCString clName = cd->className();
  scName+=clName;
  if (!cd->templateArguments().empty())
  {
    if (actualParams && *actualParamIndex<actualParams->size())
    {
      const ArgumentList &al = actualParams->at(*actualParamIndex);
      if (!isSpecialization)
      {
        scName+=tempArgListToString(al,lang);
      }
      (*actualParamIndex)++;
    }
    else
    {
      if (!isSpecialization)
      {
        scName+=tempArgListToString(cd->templateArguments(),lang);
      }
    }
  }
  //printf("qualifiedNameWithTemplateParameters: scope=%s qualifiedName=%s\n",qPrint(name()),qPrint(scName));
  return scName;
}

static QCString makeDisplayName(const ClassDef *cd,bool includeScope)
{
  //bool optimizeOutputForJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  SrcLangExt lang = cd->getLanguage();
  //bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString n;
  if (lang==SrcLangExt::VHDL)
  {
    n = VhdlDocGen::getClassName(cd);
  }
  else
  {
    if (includeScope)
    {
      n=cd->qualifiedNameWithTemplateParameters();
    }
    else
    {
      n=cd->className();
    }
  }
  if (cd->isAnonymous())
  {
    n = removeAnonymousScopes(n);
  }
  QCString sep=getLanguageSpecificSeparator(lang);
  if (sep!="::")
  {
    n=substitute(n,"::",sep);
  }
  if (cd->compoundType()==ClassDef::Protocol && n.endsWith("-p"))
  {
    n="<"+n.left(n.length()-2)+">";
  }
  return n;
}

//-----------------------------------------------------------------------------

static QCString getCompoundTypeString(SrcLangExt lang,ClassDef::CompoundType compType,bool isJavaEnum)
{
  if (lang==SrcLangExt::Fortran)
  {
    switch (compType)
    {
      case ClassDef::Class:     return "module";
      case ClassDef::Struct:    return "type";
      case ClassDef::Union:     return "union";
      case ClassDef::Interface: return "interface";
      case ClassDef::Protocol:  return "protocol";
      case ClassDef::Category:  return "category";
      case ClassDef::Exception: return "exception";
      default:                  return "unknown";
    }
  }
  else
  {
    switch (compType)
    {
      case ClassDef::Class:     return isJavaEnum ? "enum" : "class";
      case ClassDef::Struct:    return "struct";
      case ClassDef::Union:     return "union";
      case ClassDef::Interface: return lang==SrcLangExt::ObjC ? "class" : "interface";
      case ClassDef::Protocol:  return "protocol";
      case ClassDef::Category:  return "category";
      case ClassDef::Exception: return "exception";
      case ClassDef::Service:   return "service";
      case ClassDef::Singleton: return "singleton";
      default:                  return "unknown";
    }
  }
}

//-----------------------------------------------------------------------------


/** Implementation of the ClassDef interface */
class ClassDefImpl : public DefinitionMixin<ClassDefMutable>
{
  public:
    ClassDefImpl(const QCString &fileName,int startLine,int startColumn,
             const QCString &name,CompoundType ct,
             const QCString &ref=QCString(),const QCString &fName=QCString(),
             bool isSymbol=TRUE,bool isJavaEnum=FALSE);

    DefType definitionType() const override { return TypeClass; }
    std::unique_ptr<ClassDef> deepCopy(const QCString &name) const override;
    void moveTo(Definition *) override;
    CodeSymbolType codeSymbolType() const override;
    QCString getOutputFileBase() const override;
    QCString getInstanceOutputFileBase() const override;
    QCString getSourceFileBase() const override;
    QCString getReference() const override;
    bool isReference() const override;
    bool isLocal() const override;
    ClassLinkedRefMap getClasses() const override;
    bool hasDocumentation() const override;
    bool hasDetailedDescription() const override;
    QCString collaborationGraphFileName() const override;
    QCString inheritanceGraphFileName() const override;
    QCString displayName(bool includeScope=TRUE) const override;
    CompoundType compoundType() const override;
    QCString compoundTypeString() const override;
    const BaseClassList &baseClasses() const override;
    void updateBaseClasses(const BaseClassList &bcd) override;
    const BaseClassList &subClasses() const override;
    void updateSubClasses(const BaseClassList &bcd) override;
    const MemberNameInfoLinkedMap &memberNameInfoLinkedMap() const override;
    Protection protection() const override;
    bool isLinkableInProject() const override;
    bool isLinkable() const override;
    bool isVisibleInHierarchy() const override;
    bool visibleInParentsDeclList() const override;
    const ArgumentList &templateArguments() const override;
    FileDef *getFileDef() const override;
    ModuleDef *getModuleDef() const override;
    const MemberDef *getMemberByName(const QCString &) const override;
    int isBaseClass(const ClassDef *bcd,bool followInstances,const QCString &templSpec) const override;
    bool isSubClass(ClassDef *bcd,int level=0) const override;
    bool isAccessibleMember(const MemberDef *md) const override;
    const TemplateInstanceList &getTemplateInstances() const override;
    const ClassDef *templateMaster() const override;
    bool isTemplate() const override;
    const IncludeInfo *includeInfo() const override;
    const UsesClassList &usedImplementationClasses() const override;
    const UsesClassList &usedByImplementationClasses() const override;
    const ConstraintClassList &templateTypeConstraints() const override;
    bool isTemplateArgument() const override;
    const Definition *findInnerCompound(const QCString &name) const override;
    ArgumentLists getTemplateParameterLists() const override;
    QCString qualifiedNameWithTemplateParameters(
        const ArgumentLists *actualParams=nullptr,uint32_t *actualParamIndex=nullptr) const override;
    bool isAbstract() const override;
    bool isObjectiveC() const override;
    bool isFortran() const override;
    bool isCSharp() const override;
    bool isFinal() const override;
    bool isSealed() const override;
    bool isPublished() const override;
    bool isExtension() const override;
    bool isForwardDeclared() const override;
    bool isInterface() const override;
    ClassDef *categoryOf() const override;
    QCString className() const override;
    MemberList *getMemberList(MemberListType lt) const override;
    const MemberLists &getMemberLists() const override;
    const MemberGroupList &getMemberGroups() const override;
    const TemplateNameMap &getTemplateBaseClassNames() const override;
    bool isUsedOnly() const override;
    QCString anchor() const override;
    bool isEmbeddedInOuterScope() const override;
    bool isSimple() const override;
    const ClassDef *tagLessReference() const override;
    const MemberDef *isSmartPointer() const override;
    bool isJavaEnum() const override;
    QCString title() const override;
    QCString generatedFromFiles() const override;
    const FileList &usedFiles() const override;
    const ArgumentList &typeConstraints() const override;
    const ExampleList &getExamples() const override;
    bool hasExamples() const override;
    QCString getMemberListFileName() const override;
    bool subGrouping() const override;
    bool isSliceLocal() const override;
    bool hasNonReferenceSuperClass() const override;
    QCString requiresClause() const override;
    StringVector getQualifiers() const override;
    bool containsOverload(const MemberDef *md) const override;
    ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                const QCString &templSpec,bool &freshInstance) const override;
    bool isImplicitTemplateInstance() const override;

    void insertBaseClass(ClassDef *,const QCString &name,Protection p,Specifier s,const QCString &t=QCString()) override;
    void insertSubClass(ClassDef *,Protection p,Specifier s,const QCString &t=QCString()) override;
    void insertExplicitTemplateInstance(ClassDef *instance,const QCString &spec) override;
    void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force) override;
    void insertMember(MemberDef *) override;
    void insertUsedFile(const FileDef *) override;
    bool addExample(const QCString &anchor,const QCString &name, const QCString &file) override;
    void mergeCategory(ClassDef *category) override;
    void setFileDef(FileDef *fd) override;
    void setModuleDef(ModuleDef *mod) override;
    void setSubGrouping(bool enabled) override;
    void setProtection(Protection p) override;
    void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs) override;
    void addInnerCompound(Definition *d) override;
    void addUsedClass(ClassDef *cd,const QCString &accessName,Protection prot) override;
    void addUsedByClass(ClassDef *cd,const QCString &accessName,Protection prot) override;
    void setIsStatic(bool b) override;
    void setCompoundType(CompoundType t) override;
    void setClassName(const QCString &name) override;
    void setClassSpecifier(TypeSpecifier spec) override;
    void addQualifiers(const StringVector &qualifiers) override;
    void setTemplateArguments(const ArgumentList &al) override;
    void setTemplateBaseClassNames(const TemplateNameMap &templateNames) override;
    void setTemplateMaster(const ClassDef *tm) override;
    void setImplicitTemplateInstance(bool b) override;
    void setTypeConstraints(const ArgumentList &al) override;
    void addMembersToTemplateInstance(const ClassDef *cd,const ArgumentList &templateArguments,const QCString &templSpec) override;
    void makeTemplateArgument(bool b=TRUE) override;
    void setCategoryOf(ClassDef *cd) override;
    void setUsedOnly(bool b) override;
    void setTagLessReference(const ClassDef *cd) override;
    void setMetaData(const QCString &md) override;
    void findSectionsInDocumentation() override;
    void addMembersToMemberGroup() override;
    void addListReferences() override;
    void addTypeConstraints() override;
    void computeAnchors() override;
    void mergeMembers() override;
    void sortMemberLists() override;
    void distributeMemberGroupDocumentation() override;
    void writeDocumentation(OutputList &ol) const override;
    void writeDocumentationForInnerClasses(OutputList &ol) const override;
    void writeMemberPages(OutputList &ol) const override;
    void writeMemberList(OutputList &ol) const override;
    void writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,int indentLevel,
                          const ClassDef *inheritedFrom,const QCString &inheritId) const override;
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const override;
    void writeSummaryLinks(OutputList &ol) const override;
    void reclassifyMember(MemberDefMutable *md,MemberType t) override;
    void writeInlineDocumentation(OutputList &ol) const override;
    void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const override;
    void removeMemberFromLists(MemberDef *md) override;
    void setAnonymousEnumType() override;
    void countMembers() override;
    void sortAllMembersList() override;

    void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                              const ClassDef *inheritedFrom,const QCString &inheritId) const override;
    void writeTagFile(TextStream &) const override;

    int countMembersIncludingGrouped(MemberListType lt,const ClassDef *inheritedFrom,bool additional) const override;
    int countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                MemberListType lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const override;
    void writeMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                 MemberListType lt,const QCString &title,
                 const QCString &subTitle=QCString(),
                 bool showInline=FALSE,const ClassDef *inheritedFrom=nullptr,
                 MemberListType lt2=MemberListType::Invalid(),bool invert=FALSE,bool showAlways=FALSE) const override;
    void setRequiresClause(const QCString &req) override;

    // inheritance graph related members
    CLASS_GRAPH_t hasInheritanceGraph() const override;
    void overrideInheritanceGraph(CLASS_GRAPH_t e) override;

    // collaboration graph related members
    bool hasCollaborationGraph() const override;
    void overrideCollaborationGraph(bool e) override;
  private:
    int countInheritedByNodes() const;
    int countInheritsNodes() const;
    int countInheritanceNodes() const;
    void addUsedInterfaceClasses(MemberDef *md,const QCString &typeStr);
    void showUsedFiles(OutputList &ol) const;

    void writeDocumentationContents(OutputList &ol,const QCString &pageTitle) const;
    void internalInsertMember(MemberDef *md,Protection prot,bool addToAllList);
    void addMemberToList(MemberListType lt,MemberDef *md,bool isBrief);
    void writeInheritedMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                                          MemberListType lt,MemberListType lt2,const QCString &title,
                                          const ClassDef *inheritedFrom,bool invert,
                                          bool showAlways) const;
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title,bool showInline=FALSE) const;
    void writeSimpleMemberDocumentation(OutputList &ol,MemberListType lt) const;
    void writePlainMemberDeclaration(OutputList &ol,MemberListType lt,bool inGroup,
                                     int indentLevel,const ClassDef *inheritedFrom,const QCString &inheritId) const;
    void writeBriefDescription(OutputList &ol,bool exampleFlag) const;
    void writeDetailedDescription(OutputList &ol,const QCString &pageType,bool exampleFlag,
                                  const QCString &title,const QCString &anchor=QCString()) const;
    void writeIncludeFiles(OutputList &ol) const;
    void writeIncludeFilesForSlice(OutputList &ol) const;
    void writeInheritanceGraph(OutputList &ol) const;
    void writeCollaborationGraph(OutputList &ol) const;
    void writeMemberGroups(OutputList &ol,bool showInline=FALSE) const;
    void writeNestedClasses(OutputList &ol,const QCString &title) const;
    void writeInlineClasses(OutputList &ol) const;
    void startMemberDeclarations(OutputList &ol) const;
    void endMemberDeclarations(OutputList &ol) const;
    void startMemberDocumentation(OutputList &ol) const;
    void endMemberDocumentation(OutputList &ol) const;
    void writeAuthorSection(OutputList &ol) const;
    void writeMoreLink(OutputList &ol,const QCString &anchor) const;
    void writeDetailedDocumentationBody(OutputList &ol) const;

    int countAdditionalInheritedMembers() const;
    void writeAdditionalInheritedMembers(OutputList &ol) const;
    void addClassAttributes(OutputList &ol) const;
    int countInheritedDecMembers(MemberListType lt,
                                 const ClassDef *inheritedFrom,bool invert,bool showAlways,
                                 ClassDefSet &visitedClasses) const;
    void getTitleForMemberListType(MemberListType type,
               QCString &title,QCString &subtitle) const;
    void addTypeConstraint(const QCString &typeConstraint,const QCString &type);
    void writeTemplateSpec(OutputList &ol,const Definition *d,
            const QCString &type,SrcLangExt lang) const;
    void mergeMembersFromBaseClasses(bool mergeVirtualBaseClass);

    // PIMPL idiom
    class IMPL;
    std::unique_ptr<IMPL> m_impl;
};

std::unique_ptr<ClassDef> createClassDef(
             const QCString &fileName,int startLine,int startColumn,
             const QCString &name,ClassDef::CompoundType ct,
             const QCString &ref,const QCString &fName,
             bool isSymbol,bool isJavaEnum)
{
  return std::make_unique<ClassDefImpl>(fileName,startLine,startColumn,name,ct,ref,fName,isSymbol,isJavaEnum);
}
//-----------------------------------------------------------------------------

class ClassDefAliasImpl : public DefinitionAliasMixin<ClassDef>
{
  public:
    ClassDefAliasImpl(const Definition *newScope,const ClassDef *cd)
      : DefinitionAliasMixin(newScope,cd) { init(); }
   ~ClassDefAliasImpl() override { deinit(); }
    NON_COPYABLE(ClassDefAliasImpl)

    DefType definitionType() const override { return TypeClass; }

    const ClassDef *getCdAlias() const { return toClassDef(getAlias()); }
    std::unique_ptr<ClassDef> deepCopy(const QCString &name) const override  {
      return createClassDefAlias(getScope(),getCdAlias());
    }
    void moveTo(Definition *) override {}

    CodeSymbolType codeSymbolType() const override
    { return getCdAlias()->codeSymbolType(); }
    QCString getOutputFileBase() const override
    { return getCdAlias()->getOutputFileBase(); }
    QCString getInstanceOutputFileBase() const override
    { return getCdAlias()->getInstanceOutputFileBase(); }
    QCString getSourceFileBase() const override
    { return getCdAlias()->getSourceFileBase(); }
    QCString getReference() const override
    { return getCdAlias()->getReference(); }
    bool isReference() const override
    { return getCdAlias()->isReference(); }
    bool isLocal() const override
    { return getCdAlias()->isLocal(); }
    ClassLinkedRefMap getClasses() const override
    { return getCdAlias()->getClasses(); }
    bool hasDocumentation() const override
    { return getCdAlias()->hasDocumentation(); }
    bool hasDetailedDescription() const override
    { return getCdAlias()->hasDetailedDescription(); }
    QCString collaborationGraphFileName() const override
    { return getCdAlias()->collaborationGraphFileName(); }
    QCString inheritanceGraphFileName() const override
    { return getCdAlias()->inheritanceGraphFileName(); }
    QCString displayName(bool includeScope=TRUE) const override
    { return makeDisplayName(this,includeScope); }
    CompoundType compoundType() const override
    { return getCdAlias()->compoundType(); }
    QCString compoundTypeString() const override
    { return getCdAlias()->compoundTypeString(); }
    const BaseClassList &baseClasses() const override
    { return getCdAlias()->baseClasses(); }
    const BaseClassList &subClasses() const override
    { return getCdAlias()->subClasses(); }
    const MemberNameInfoLinkedMap &memberNameInfoLinkedMap() const override
    { return getCdAlias()->memberNameInfoLinkedMap(); }
    Protection protection() const override
    { return getCdAlias()->protection(); }
    bool isLinkableInProject() const override
    { return getCdAlias()->isLinkableInProject(); }
    bool isLinkable() const override
    { return getCdAlias()->isLinkable(); }
    bool isVisibleInHierarchy() const override
    { return getCdAlias()->isVisibleInHierarchy(); }
    bool visibleInParentsDeclList() const override
    { return getCdAlias()->visibleInParentsDeclList(); }
    const ArgumentList &templateArguments() const override
    { return getCdAlias()->templateArguments(); }
    FileDef *getFileDef() const override
    { return getCdAlias()->getFileDef(); }
    ModuleDef *getModuleDef() const override
    { return getCdAlias()->getModuleDef(); }
    const MemberDef *getMemberByName(const QCString &s) const override
    { return getCdAlias()->getMemberByName(s); }
    int isBaseClass(const ClassDef *bcd,bool followInstances,const QCString &templSpec) const override
    { return getCdAlias()->isBaseClass(bcd,followInstances,templSpec); }
    bool isSubClass(ClassDef *bcd,int level=0) const override
    { return getCdAlias()->isSubClass(bcd,level); }
    bool isAccessibleMember(const MemberDef *md) const override
    { return getCdAlias()->isAccessibleMember(md); }
    const TemplateInstanceList &getTemplateInstances() const override
    { return getCdAlias()->getTemplateInstances(); }
    const ClassDef *templateMaster() const override
    { return getCdAlias()->templateMaster(); }
    bool isTemplate() const override
    { return getCdAlias()->isTemplate(); }
    const IncludeInfo *includeInfo() const override
    { return getCdAlias()->includeInfo(); }
    const UsesClassList &usedImplementationClasses() const override
    { return getCdAlias()->usedImplementationClasses(); }
    const UsesClassList &usedByImplementationClasses() const override
    { return getCdAlias()->usedByImplementationClasses(); }
    const ConstraintClassList &templateTypeConstraints() const override
    { return getCdAlias()->templateTypeConstraints(); }
    bool isTemplateArgument() const override
    { return getCdAlias()->isTemplateArgument(); }
    const Definition *findInnerCompound(const QCString &name) const override
    { return getCdAlias()->findInnerCompound(name); }
    ArgumentLists getTemplateParameterLists() const override
    { return getCdAlias()->getTemplateParameterLists(); }
    QCString qualifiedNameWithTemplateParameters(
        const ArgumentLists *actualParams=nullptr,uint32_t *actualParamIndex=nullptr) const override
    { return makeQualifiedNameWithTemplateParameters(this,actualParams,actualParamIndex); }
    bool isAbstract() const override
    { return getCdAlias()->isAbstract(); }
    bool isObjectiveC() const override
    { return getCdAlias()->isObjectiveC(); }
    bool isFortran() const override
    { return getCdAlias()->isFortran(); }
    bool isCSharp() const override
    { return getCdAlias()->isCSharp(); }
    bool isFinal() const override
    { return getCdAlias()->isFinal(); }
    bool isSealed() const override
    { return getCdAlias()->isSealed(); }
    bool isPublished() const override
    { return getCdAlias()->isPublished(); }
    bool isExtension() const override
    { return getCdAlias()->isExtension(); }
    bool isForwardDeclared() const override
    { return getCdAlias()->isForwardDeclared(); }
    bool isInterface() const override
    { return getCdAlias()->isInterface(); }
    ClassDef *categoryOf() const override
    { return getCdAlias()->categoryOf(); }
    QCString className() const override
    { return getCdAlias()->className(); }
    MemberList *getMemberList(MemberListType lt) const override
    { return getCdAlias()->getMemberList(lt); }
    const MemberLists &getMemberLists() const override
    { return getCdAlias()->getMemberLists(); }
    const MemberGroupList &getMemberGroups() const override
    { return getCdAlias()->getMemberGroups(); }
    const TemplateNameMap &getTemplateBaseClassNames() const override
    { return getCdAlias()->getTemplateBaseClassNames(); }
    bool isUsedOnly() const override
    { return getCdAlias()->isUsedOnly(); }
    QCString anchor() const override
    { return getCdAlias()->anchor(); }
    bool isEmbeddedInOuterScope() const override
    { return getCdAlias()->isEmbeddedInOuterScope(); }
    bool isSimple() const override
    { return getCdAlias()->isSimple(); }
    const ClassDef *tagLessReference() const override
    { return getCdAlias()->tagLessReference(); }
    const MemberDef *isSmartPointer() const override
    { return getCdAlias()->isSmartPointer(); }
    bool isJavaEnum() const override
    { return getCdAlias()->isJavaEnum(); }
    QCString title() const override
    { return getCdAlias()->title(); }
    QCString generatedFromFiles() const override
    { return getCdAlias()->generatedFromFiles(); }
    const FileList &usedFiles() const override
    { return getCdAlias()->usedFiles(); }
    const ArgumentList &typeConstraints() const override
    { return getCdAlias()->typeConstraints(); }
    const ExampleList &getExamples() const override
    { return getCdAlias()->getExamples(); }
    bool hasExamples() const override
    { return getCdAlias()->hasExamples(); }
    QCString getMemberListFileName() const override
    { return getCdAlias()->getMemberListFileName(); }
    bool subGrouping() const override
    { return getCdAlias()->subGrouping(); }
    bool isSliceLocal() const override
    { return getCdAlias()->isSliceLocal(); }
    bool hasNonReferenceSuperClass() const override
    { return getCdAlias()->hasNonReferenceSuperClass(); }
    QCString requiresClause() const override
    { return getCdAlias()->requiresClause(); }
    StringVector getQualifiers() const override
    { return getCdAlias()->getQualifiers(); }
    bool containsOverload(const MemberDef *md) const override
    { return getCdAlias()->containsOverload(md); }

    int countMembersIncludingGrouped(MemberListType lt,const ClassDef *inheritedFrom,bool additional) const override
    { return getCdAlias()->countMembersIncludingGrouped(lt,inheritedFrom,additional); }
    int countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                MemberListType lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const override
    { return getCdAlias()->countMemberDeclarations(lt,inheritedFrom,lt2,invert,showAlways,visitedClasses); }

    void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const override
    { getCdAlias()->writeDeclarationLink(ol,found,header,localNames); }
    ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                             const QCString &templSpec,bool &freshInstance) const override
    { return getCdAlias()->insertTemplateInstance(fileName,startLine,startColumn,templSpec,freshInstance); }
    bool isImplicitTemplateInstance() const override
    { return getCdAlias()->isImplicitTemplateInstance(); }

    void writeDocumentation(OutputList &ol) const override
    { getCdAlias()->writeDocumentation(ol); }
    void writeDocumentationForInnerClasses(OutputList &ol) const override
    { getCdAlias()->writeDocumentationForInnerClasses(ol); }
    void writeMemberPages(OutputList &ol) const override
    { getCdAlias()->writeMemberPages(ol); }
    void writeMemberList(OutputList &ol) const override
    { getCdAlias()->writeMemberList(ol); }
    void writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,
                 int indentLevel, const ClassDef *inheritedFrom,const QCString &inheritId) const override
    { getCdAlias()->writeDeclaration(ol,md,inGroup,indentLevel,inheritedFrom,inheritId); }
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const override
    { getCdAlias()->writeQuickMemberLinks(ol,md); }
    void writeSummaryLinks(OutputList &ol) const override
    { getCdAlias()->writeSummaryLinks(ol); }
    void writeInlineDocumentation(OutputList &ol) const override
    { getCdAlias()->writeInlineDocumentation(ol); }
    void writeTagFile(TextStream &ol) const override
    { getCdAlias()->writeTagFile(ol); }
    void writeMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                 MemberListType lt,const QCString &title,
                 const QCString &subTitle=QCString(),
                 bool showInline=FALSE,const ClassDef *inheritedFrom=nullptr,
                 MemberListType lt2=MemberListType::Invalid(),bool invert=FALSE,bool showAlways=FALSE) const override
    { getCdAlias()->writeMemberDeclarations(ol,visitedClasses,lt,title,subTitle,showInline,inheritedFrom,lt2,invert,showAlways); }
    void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                 const ClassDef *inheritedFrom,const QCString &inheritId) const override
    { getCdAlias()->addGroupedInheritedMembers(ol,lt,inheritedFrom,inheritId); }

    void updateBaseClasses(const BaseClassList &) override {}
    void updateSubClasses(const BaseClassList &) override {}
};

std::unique_ptr<ClassDef> createClassDefAlias(const Definition *newScope,const ClassDef *cd)
{
  auto acd = std::make_unique<ClassDefAliasImpl>(newScope,cd);
  //printf("cd name=%s localName=%s qualifiedName=%s qualifiedNameWith=%s displayName()=%s\n",
  //    qPrint(acd->name()),qPrint(acd->localName()),qPrint(acd->qualifiedName()),
  //    qPrint(acd->qualifiedNameWithTemplateParameters()),qPrint(acd->displayName()));
  return acd;
}

//-----------------------------------------------------------------------------

/** Private data associated with a ClassDef object. */
class ClassDefImpl::IMPL
{
  public:
    void init(const QCString &defFileName, const QCString &name,
              const QCString &ctStr, const QCString &fName);
    std::unique_ptr<ClassDefImpl::IMPL> deepCopy() const;

    /*! file name that forms the base for the output file containing the
     *  class documentation. For compatibility with Qt (e.g. links via tag
     *  files) this name cannot be derived from the class name directly.
     */
    QCString fileName;

    /*! file name used for the list of all members */
    QCString memberListFileName;

    /*! file name used for the collaboration diagram */
    QCString collabFileName;

    /*! file name used for the inheritance graph */
    QCString inheritFileName;

    /*! Include information about the header file should be included
     *  in the documentation. 0 by default, set by setIncludeFile().
     */
    std::unique_ptr<IncludeInfo> incInfo;

    /*! List of base class (or super-classes) from which this class derives
     *  directly.
     */
    BaseClassList inherits;

    /*! List of sub-classes that directly derive from this class
     */
    BaseClassList inheritedBy;

    /*! Namespace this class is part of
     *  (this is the inner most namespace in case of nested namespaces)
     */
    //NamespaceDef  *nspace = nullptr;

    /*! File this class is defined in */
    FileDef *fileDef = nullptr;

    /*! Module this class is defined in */
    ModuleDef *moduleDef = nullptr;

    /*! List of all members (including inherited members) */
    MemberNameInfoLinkedMap allMemberNameInfoLinkedMap;

    /*! Template arguments of this class */
    ArgumentList tempArgs;

    /*! Type constraints for template parameters */
    ArgumentList typeConstraints;

    /*! Files that were used for generating the class documentation. */
    FileList files;

    /*! Examples that use this class */
    ExampleList examples;

    /*! Holds the kind of "class" this is. */
    ClassDef::CompoundType compType;

    /*! The protection level in which this class was found.
     *  Typically Public, but for nested classes this can also be Protected
     *  or Private.
     */
    Protection prot;

    /*! The inner classes contained in this class. Will be 0 if there are
     *  no inner classes.
     */
    ClassLinkedRefMap innerClasses;

    /* classes for the collaboration diagram */
    UsesClassList usesImplClassList;
    UsesClassList usedByImplClassList;

    ConstraintClassList constraintClassList;

    /*! Template instances that exists of this class, the key in the
     *  dictionary is the template argument list.
     */
    TemplateInstanceList templateInstances;

    TemplateNameMap templBaseClassNames;

    /*! The class this class is an instance of. */
    const ClassDef *templateMaster = nullptr;

    /*! local class name which could be a typedef'ed alias name. */
    QCString className;

    /*! If this class is a Objective-C category, then this points to the
     *  class which is extended.
     */
    ClassDef *categoryOf = nullptr;

    MemberLists memberLists;

    /* user defined member groups */
    MemberGroupList memberGroups;

    /*! Is this an abstract class? */
    bool isAbstract = false;

    /*! Is the class part of an unnamed namespace? */
    bool isStatic = false;

    /*! TRUE if classes members are merged with those of the base classes. */
    bool membersMerged = false;

    /*! TRUE if the class is defined in a source file rather than a header file. */
    bool isLocal = false;

    bool isTemplArg = false;

    /*! Does this class group its user-grouped members
     *  as a sub-section of the normal (public/protected/..)
     *  groups?
     */
    bool subGrouping = false;

    /** Reason of existence is a "use" relation */
    bool usedOnly = false;

    /** List of titles to use for the summary */
    StringSet vhdlSummaryTitles;

    /** Is this a simple (non-nested) C structure? */
    bool isSimple = false;

    /** Does this class overloaded the -> operator? */
    const MemberDef *arrowOperator = nullptr;

    const ClassDef *tagLessRef = nullptr;

    /** Does this class represent a Java style enum? */
    bool isJavaEnum = false;

    TypeSpecifier spec;

    QCString metaData;

    /** C++20 requires clause */
    QCString requiresClause;

    StringVector qualifiers;

    bool hasCollaborationGraph = false;
    CLASS_GRAPH_t typeInheritanceGraph = CLASS_GRAPH_t::NO;

    bool implicitTemplateInstance = false;
};

void ClassDefImpl::IMPL::init(const QCString &defFileName, const QCString &name,
                        const QCString &ctStr, const QCString &fName)
{
  if (!fName.isEmpty())
  {
    fileName=stripExtension(fName);
  }
  else
  {
    fileName=ctStr+name;
  }
  prot=Protection::Public;
  //nspace=nullptr;
  fileDef=nullptr;
  moduleDef=nullptr;
  subGrouping=Config_getBool(SUBGROUPING);
  templateMaster =nullptr;
  isAbstract = FALSE;
  isStatic = FALSE;
  isTemplArg = FALSE;
  membersMerged = FALSE;
  categoryOf = nullptr;
  usedOnly = FALSE;
  isSimple = Config_getBool(INLINE_SIMPLE_STRUCTS);
  arrowOperator = nullptr;
  tagLessRef = nullptr;
  spec=TypeSpecifier();
  //QCString ns;
  //extractNamespaceName(name,className,ns);
  //printf("m_name=%s m_className=%s ns=%s\n",qPrint(m_name),qPrint(m_className),qPrint(ns));

  // we cannot use getLanguage at this point, as setLanguage has not been called.
  SrcLangExt lang = getLanguageFromFileName(defFileName);
  if ((lang==SrcLangExt::Cpp || lang==SrcLangExt::ObjC) && guessSection(defFileName).isSource())
  {
    isLocal=TRUE;
  }
  else
  {
    isLocal=FALSE;
  }
  hasCollaborationGraph = Config_getBool(COLLABORATION_GRAPH);
  typeInheritanceGraph = Config_getEnum(CLASS_GRAPH);
}

//-------------------------------------------------------------------------------------------

// constructs a new class definition
ClassDefImpl::ClassDefImpl(
    const QCString &defFileName,int defLine,int defColumn,
    const QCString &nm,CompoundType ct,
    const QCString &lref,const QCString &fName,
    bool isSymbol,bool isJavaEnum)
 : DefinitionMixin(defFileName,defLine,defColumn,removeRedundantWhiteSpace(nm),nullptr,nullptr,isSymbol),
  m_impl(std::make_unique<IMPL>())
{
  setReference(lref);
  m_impl->compType = ct;
  m_impl->isJavaEnum = isJavaEnum;
  QCString compTypeString = getCompoundTypeString(getLanguage(),ct,isJavaEnum);
  m_impl->init(defFileName,name(),compTypeString,fName);
  m_impl->memberListFileName = convertNameToFile(compTypeString+name()+"-members");
  m_impl->collabFileName = convertNameToFile(m_impl->fileName+"_coll_graph");
  m_impl->inheritFileName = convertNameToFile(m_impl->fileName+"_inherit_graph");
  if (lref.isEmpty())
  {
    m_impl->fileName = convertNameToFile(m_impl->fileName);
  }
}

std::unique_ptr<ClassDefImpl::IMPL> ClassDefImpl::IMPL::deepCopy() const
{
  auto result = std::make_unique<ClassDefImpl::IMPL>();

  result->memberListFileName = memberListFileName;
  result->collabFileName = collabFileName;
  result->inheritFileName = inheritFileName;
  if (incInfo)
  {
    result->incInfo = std::make_unique<IncludeInfo>();
    *(result->incInfo) = *incInfo;
  }
  result->inherits  = inherits;
  result->inheritedBy  = inheritedBy;
  result->fileDef = fileDef;
  result->moduleDef = moduleDef;
  result->tempArgs = tempArgs;
  result->typeConstraints = typeConstraints;
  result->files = files;
  result->examples = examples;
  result->compType = compType;
  result->prot = prot;
  result->usesImplClassList = usesImplClassList;
  result->usedByImplClassList = usedByImplClassList;
  result->constraintClassList = constraintClassList;
  result->templateInstances = templateInstances;
  result->templBaseClassNames = templBaseClassNames;
  result->templateMaster = templateMaster;
  result->className = className;
  result->categoryOf = categoryOf;
  result->isAbstract = isAbstract;
  result->isStatic = isStatic;
  result->membersMerged = membersMerged;
  result->isLocal = isLocal;
  result->isTemplArg = isTemplArg;
  result->subGrouping = subGrouping;
  result->usedOnly = usedOnly;
  result->vhdlSummaryTitles = vhdlSummaryTitles;
  result->isSimple = isSimple;
  result->arrowOperator = arrowOperator;
  result->tagLessRef = tagLessRef;
  result->isJavaEnum = isJavaEnum;
  result->spec = spec;
  result->metaData = metaData;
  result->requiresClause = requiresClause;
  result->qualifiers = qualifiers;
  result->hasCollaborationGraph = hasCollaborationGraph;
  result->typeInheritanceGraph = typeInheritanceGraph;

  return result;
}

std::unique_ptr<ClassDef> ClassDefImpl::deepCopy(const QCString &name) const
{
  AUTO_TRACE("name='{}'",name);
  auto result = std::make_unique<ClassDefImpl>(
        getDefFileName(),getDefLine(),getDefColumn(),name,compoundType(),
        std::string(),std::string(),true,m_impl->isJavaEnum);
  // copy other members
  result->m_impl = m_impl->deepCopy();

  // set new file name
  QCString compTypeString = getCompoundTypeString(getLanguage(),m_impl->compType,m_impl->isJavaEnum);
  result->m_impl->fileName = compTypeString+name;
  result->m_impl->memberListFileName = convertNameToFile(compTypeString+name+"-members");
  result->m_impl->collabFileName = convertNameToFile(result->m_impl->fileName+"_coll_graph");
  result->m_impl->inheritFileName = convertNameToFile(result->m_impl->fileName+"_inherit_graph");
  result->m_impl->fileName = convertNameToFile(result->m_impl->fileName);

  // deep copy nested classes
  for (const auto &innerCd : m_impl->innerClasses)
  {
    QCString innerName = name+"::"+innerCd->localName();
    if (Doxygen::classLinkedMap->find(innerName)==nullptr)
    {
      auto cd = Doxygen::classLinkedMap->add(innerName,innerCd->deepCopy(innerName));
      result->addInnerCompound(cd);
      ClassDefMutable *cdm = toClassDefMutable(cd);
      if (cdm)
      {
        cdm->setOuterScope(result.get());
      }
    }
  }

  // copy all member list (and make deep copies of members)
  for (auto &mni : m_impl->allMemberNameInfoLinkedMap)
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md=mi->memberDef();
      auto newMd = md->deepCopy();
      if (newMd)
      {
        auto mmd = toMemberDefMutable(newMd.get());
        AUTO_TRACE_ADD("Copying member {}",mmd->name());
        mmd->moveTo(result.get());

        result->internalInsertMember(newMd.get(),newMd->protection(),true);

        // also add to the global list (which will own newMd)
        MemberName *mn = Doxygen::memberNameLinkedMap->add(newMd->name());
        mn->push_back(std::move(newMd));
      }
    }
  }

  return result;
}

void ClassDefImpl::moveTo(Definition *scope)
{
  //printf("%s::moveTo(%s)\n",qPrint(name()),qPrint(scope->name()));
  setOuterScope(scope);
  if (scope->definitionType()==Definition::TypeFile)
  {
    m_impl->fileDef = toFileDef(scope);
  }
  else if (scope->definitionType()==Definition::TypeModule)
  {
    m_impl->moduleDef = toModuleDef(scope);
  }
}

QCString ClassDefImpl::getMemberListFileName() const
{
  return m_impl->memberListFileName;
}

QCString ClassDefImpl::displayName(bool includeScope) const
{
  return makeDisplayName(this,includeScope);
}

// inserts a base/super class in the inheritance list
void ClassDefImpl::insertBaseClass(ClassDef *cd,const QCString &n,Protection p,
                               Specifier s,const QCString &t)
{
  //printf("*** insert base class %s into %s\n",qPrint(cd->name()),qPrint(name()));
  m_impl->inherits.emplace_back(cd,n,p,s,t);
  m_impl->isSimple = FALSE;
}

// inserts a derived/sub class in the inherited-by list
void ClassDefImpl::insertSubClass(ClassDef *cd,Protection p,
                                Specifier s,const QCString &t)
{
  //printf("*** insert sub class %s into %s\n",qPrint(cd->name()),qPrint(name()));
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  if (!extractPrivate && cd->protection()==Protection::Private) return;
  m_impl->inheritedBy.emplace_back(cd,QCString(),p,s,t);
  m_impl->isSimple = FALSE;
}

void ClassDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_impl->memberLists)
  {
    if (!ml->listType().isDetailed())
    {
      ::addMembersToMemberGroup(ml.get(),&m_impl->memberGroups,this);
    }
  }

  // add members inside sections to their groups
  for (const auto &mg : m_impl->memberGroups)
  {
    if (mg->allMembersInSameSection() && m_impl->subGrouping)
    {
      //printf("addToDeclarationSection(%s)\n",qPrint(mg->header()));
      mg->addToDeclarationSection();
    }
  }
}

// adds new member definition to the class
void ClassDefImpl::internalInsertMember(MemberDef *md,
                                    Protection prot,
                                    bool addToAllList
                                   )
{
  AUTO_TRACE("{} name={} isHidden={}",name(),md->name(),md->isHidden());
  if (md->isHidden()) return;

  if (getLanguage()==SrcLangExt::VHDL)
  {
    QCString title=theTranslator->trVhdlType(md->getVhdlSpecifiers(),FALSE);
    m_impl->vhdlSummaryTitles.insert(title.str());
  }

  if (1 /*!isReference()*/) // changed to 1 for showing members of external
                            // classes when HAVE_DOT and UML_LOOK are enabled.
  {
    bool isSimple=FALSE;

    /********************************************/
    /* insert member in the declaration section */
    /********************************************/
    if (md->isRelated() && protectionLevelVisible(prot))
    {
      addMemberToList(MemberListType::Related(),md,TRUE);
    }
    else if (md->isFriend())
    {
      addMemberToList(MemberListType::Friends(),md,TRUE);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberType::Service: // UNO IDL
          addMemberToList(MemberListType::Services(),md,TRUE);
          break;
        case MemberType::Interface: // UNO IDL
          addMemberToList(MemberListType::Interfaces(),md,TRUE);
          break;
        case MemberType::Signal: // Qt specific
          addMemberToList(MemberListType::Signals(),md,TRUE);
          break;
        case MemberType::DCOP:   // KDE2 specific
          addMemberToList(MemberListType::DcopMethods(),md,TRUE);
          break;
        case MemberType::Property:
          addMemberToList(MemberListType::Properties(),md,TRUE);
          break;
        case MemberType::Event:
          addMemberToList(MemberListType::Events(),md,TRUE);
          break;
        case MemberType::Slot:   // Qt specific
          switch (prot)
          {
            case Protection::Protected:
            case Protection::Package: // slots in packages are not possible!
              addMemberToList(MemberListType::ProSlots(),md,TRUE);
              break;
            case Protection::Public:
              addMemberToList(MemberListType::PubSlots(),md,TRUE);
              break;
            case Protection::Private:
              addMemberToList(MemberListType::PriSlots(),md,TRUE);
              break;
          }
          break;
        default: // any of the other members
          if (md->isStatic())
          {
            if (md->isVariable())
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType::ProStaticAttribs(),md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType::PacStaticAttribs(),md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType::PubStaticAttribs(),md,TRUE);
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType::PriStaticAttribs(),md,TRUE);
                  break;
              }
            }
            else // function
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType::ProStaticMethods(),md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType::PacStaticMethods(),md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType::PubStaticMethods(),md,TRUE);
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType::PriStaticMethods(),md,TRUE);
                  break;
              }
            }
          }
          else // not static
          {
            if (md->isVariable())
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType::ProAttribs(),md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType::PacAttribs(),md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType::PubAttribs(),md,TRUE);
                  isSimple=TRUE;
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType::PriAttribs(),md,TRUE);
                  break;
              }
            }
            else if (md->isTypedef() || md->isEnumerate() || md->isEnumValue())
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType::ProTypes(),md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType::PacTypes(),md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType::PubTypes(),md,TRUE);
                  isSimple=!md->isEnumerate() &&
                           !md->isEnumValue() &&
                           QCString(md->typeString()).find(")(")==-1; // func ptr typedef
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType::PriTypes(),md,TRUE);
                  break;
              }
            }
            else // member function
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType::ProMethods(),md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType::PacMethods(),md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType::PubMethods(),md,TRUE);
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType::PriMethods(),md,TRUE);
                  break;
              }
            }
          }
          break;
      }
    }
    if (!isSimple) // not a simple field -> not a simple struct
    {
      m_impl->isSimple = FALSE;
    }
    //printf("adding %s simple=%d total_simple=%d\n",qPrint(name()),isSimple,m_impl->isSimple);

    /*******************************************************/
    /* insert member in the detailed documentation section */
    /*******************************************************/
    if ((md->isRelated() && protectionLevelVisible(prot)) || md->isFriend())
    {
      addMemberToList(MemberListType::RelatedMembers(),md,FALSE);
    }
    else if (md->isFunction() &&
             md->protection()==Protection::Private &&
             (md->virtualness()!=Specifier::Normal || md->isOverride() || md->isFinal()) &&
             Config_getBool(EXTRACT_PRIV_VIRTUAL))
    {
      addMemberToList(MemberListType::FunctionMembers(),md,FALSE);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberType::Service: // UNO IDL
          addMemberToList(MemberListType::ServiceMembers(),md,FALSE);
          break;
        case MemberType::Interface: // UNO IDL
          addMemberToList(MemberListType::InterfaceMembers(),md,FALSE);
          break;
        case MemberType::Property:
          addMemberToList(MemberListType::PropertyMembers(),md,FALSE);
          break;
        case MemberType::Event:
          addMemberToList(MemberListType::EventMembers(),md,FALSE);
          break;
        case MemberType::Signal: // fall through
        case MemberType::DCOP:
          addMemberToList(MemberListType::FunctionMembers(),md,FALSE);
          break;
        case MemberType::Slot:
          if (protectionLevelVisible(prot))
          {
            addMemberToList(MemberListType::FunctionMembers(),md,FALSE);
          }
          break;
        default: // any of the other members
          if (protectionLevelVisible(prot))
          {
            switch (md->memberType())
            {
              case MemberType::Typedef:
                addMemberToList(MemberListType::TypedefMembers(),md,FALSE);
                break;
              case MemberType::Enumeration:
                addMemberToList(MemberListType::EnumMembers(),md,FALSE);
                break;
              case MemberType::EnumValue:
                addMemberToList(MemberListType::EnumValMembers(),md,FALSE);
                break;
              case MemberType::Function:
                if (md->isConstructor() || md->isDestructor())
                {
                  m_impl->memberLists.get(MemberListType::Constructors(),MemberListContainer::Class)->push_back(md);
                }
                else
                {
                  addMemberToList(MemberListType::FunctionMembers(),md,FALSE);
                }
                break;
              case MemberType::Variable:
                addMemberToList(MemberListType::VariableMembers(),md,FALSE);
                break;
              case MemberType::Define:
                warn(md->getDefFileName(),md->getDefLine()-1,"A define (%s) cannot be made a member of %s",
                     qPrint(md->name()), qPrint(this->name()));
                break;
              default:
                err("Unexpected member type '%s' found!\n",qPrint(md->memberTypeName()));
            }
          }
          break;
      }
    }

    /*************************************************/
    /* insert member in the appropriate member group */
    /*************************************************/
    // Note: this must be done AFTER inserting the member in the
    // regular groups
    //addMemberToGroup(md,groupId);

  }

  if (md->virtualness()==Specifier::Pure)
  {
    m_impl->isAbstract=true;
  }

  if (md->name()=="operator->")
  {
    m_impl->arrowOperator=md;
  }

  if (addToAllList &&
      !(Config_getBool(HIDE_FRIEND_COMPOUNDS) &&
        md->isFriend() &&
        (QCString(md->typeString())=="friend class" ||
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union")))
  {
    //printf("=======> adding member %s to class %s\n",qPrint(md->name()),qPrint(name()));

    MemberNameInfo *mni = m_impl->allMemberNameInfoLinkedMap.add(md->name());
    mni->push_back(std::make_unique<MemberInfo>(md,prot,md->virtualness(),false,false));
  }
}

void ClassDefImpl::insertMember(MemberDef *md)
{
  internalInsertMember(md,md->protection(),TRUE);
}

// compute the anchors for all members
void ClassDefImpl::computeAnchors()
{
  for (auto &ml : m_impl->memberLists)
  {
    if (!ml->listType().isDetailed())
    {
      ml->setAnchors();
    }
  }

  for (const auto &mg : m_impl->memberGroups)
  {
    mg->setAnchors();
  }
}

void ClassDefImpl::distributeMemberGroupDocumentation()
{
  for (const auto &mg : m_impl->memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void ClassDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
  docFindSections(inbodyDocumentation(),this,docFile());
  for (const auto &mg : m_impl->memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }
  for (auto &ml : m_impl->memberLists)
  {
    if (!ml->listType().isDetailed())
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}


// add a file name to the used files set
void ClassDefImpl::insertUsedFile(const FileDef *fd)
{
  if (fd==nullptr) return;
  auto it = std::find(m_impl->files.begin(),m_impl->files.end(),fd);
  if (it==m_impl->files.end())
  {
    m_impl->files.push_back(fd);
  }
  for (const auto &ti : m_impl->templateInstances)
  {
    ClassDefMutable *cdm = toClassDefMutable(ti.classDef);
    if (cdm)
    {
      cdm->insertUsedFile(fd);
    }
  }
}

static void writeInheritanceSpecifier(OutputList &ol,const BaseClassDef &bcd)
{
  if (bcd.prot!=Protection::Public || bcd.virt!=Specifier::Normal)
  {
    ol.startTypewriter();
    ol.docify(" [");
    StringVector sl;
    if      (bcd.prot==Protection::Protected) sl.emplace_back("protected");
    else if (bcd.prot==Protection::Private)   sl.emplace_back("private");
    if      (bcd.virt==Specifier::Virtual)    sl.emplace_back("virtual");
    bool first=true;
    for (const auto &s : sl)
    {
      if (!first) ol.docify(", ");
      ol.docify(s.c_str());
      first=false;
    }
    ol.docify("]");
    ol.endTypewriter();
  }
}

void ClassDefImpl::setIncludeFile(FileDef *fd,
             const QCString &includeName,bool local, bool force)
{
  //printf("ClassDefImpl::setIncludeFile(%p,%s,%d,%d)\n",fd,includeName,local,force);
  if (!m_impl->incInfo) m_impl->incInfo = std::make_unique<IncludeInfo>();
  if ((!includeName.isEmpty() && m_impl->incInfo->includeName.isEmpty()) ||
      (fd!=nullptr && m_impl->incInfo->fileDef==nullptr)
     )
  {
    //printf("Setting file info\n");
    m_impl->incInfo->fileDef     = fd;
    m_impl->incInfo->includeName = includeName;
    m_impl->incInfo->kind        = local ? IncludeKind::IncludeLocal : IncludeKind::IncludeSystem;
  }
  if (force && !includeName.isEmpty())
  {
    m_impl->incInfo->includeName = includeName;
    m_impl->incInfo->kind        = local ? IncludeKind::IncludeLocal : IncludeKind::IncludeSystem;
  }
}

// TODO: fix this: a nested template class can have multiple outer templates
//ArgumentList *ClassDefImpl::outerTemplateArguments() const
//{
//  int ti;
//  ClassDef *pcd=nullptr;
//  int pi=0;
//  if (m_impl->tempArgs) return m_impl->tempArgs;
//  // find the outer most class scope
//  while ((ti=name().find("::",pi))!=-1 &&
//      (pcd=getClass(name().left(ti)))==0
//        ) pi=ti+2;
//  if (pcd)
//  {
//    return pcd->templateArguments();
//  }
//  return nullptr;
//}

static void searchTemplateSpecs(/*in*/  const Definition *d,
                                /*out*/ ArgumentLists &result,
                                /*out*/ QCString &name,
                                /*in*/  SrcLangExt lang)
{
  if (d->definitionType()==Definition::TypeClass)
  {
    if (d->getOuterScope())
    {
      searchTemplateSpecs(d->getOuterScope(),result,name,lang);
    }
    const ClassDef *cd=toClassDef(d);
    if (!name.isEmpty()) name+="::";
    QCString clName = d->localName();
    if (clName.endsWith("-p"))
    {
      clName = clName.left(clName.length()-2);
    }
    name+=clName;
    bool isSpecialization = d->localName().find('<')!=-1;
    if (!cd->templateArguments().empty())
    {
      result.push_back(cd->templateArguments());
      if (!isSpecialization)
      {
        name+=tempArgListToString(cd->templateArguments(),lang);
      }
    }
  }
  else
  {
    name+=d->qualifiedName();
  }
}

void ClassDefImpl::writeTemplateSpec(OutputList &ol,const Definition *d,
            const QCString &type,SrcLangExt lang) const
{
  ArgumentLists specs;
  QCString name;
  searchTemplateSpecs(d,specs,name,lang);
  if (!specs.empty()) // class has template scope specifiers
  {
    ol.startCompoundTemplateParams();
    for (const ArgumentList &al : specs)
    {
      ol.docify("template<");
      auto it = al.begin();
      while (it!=al.end())
      {
        Argument a = *it;
        linkifyText(TextGeneratorOLImpl(ol), // out
          d,                       // scope
          getFileDef(),            // fileScope
          this,                    // self
          a.type,                  // text
          FALSE                    // autoBreak
          );
        if (!a.name.isEmpty())
        {
          ol.docify(" ");
          ol.docify(a.name);
        }
        if (a.defval.length()!=0)
        {
          ol.docify(" = ");
          ol.docify(a.defval);
        }
        ++it;
        if (it!=al.end()) ol.docify(", ");
      }
      ol.docify(">");
      ol.lineBreak();
    }
    if (!m_impl->requiresClause.isEmpty())
    {
      ol.docify("requires ");
      linkifyText(TextGeneratorOLImpl(ol), // out
          d,                       // scope
          getFileDef(),            // fileScope
          this,                    // self
          m_impl->requiresClause,  // text
          FALSE                    // autoBreak
          );
      ol.lineBreak();
    }
    ol.docify(type.lower()+" "+name);
    ol.endCompoundTemplateParams();
  }
}

void ClassDefImpl::writeBriefDescription(OutputList &ol,bool exampleFlag) const
{
  if (hasBriefDescription())
  {
    ol.startParagraph();
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Man);
    ol.writeString(" - ");
    ol.popGeneratorState();
    ol.generateDoc(briefFile(),briefLine(),this,nullptr,
                   briefDescription(),TRUE,FALSE,QCString(),
                   TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    ol.pushGeneratorState();
    ol.disable(OutputType::RTF);
    ol.writeString(" \n");
    ol.enable(OutputType::RTF);
    ol.popGeneratorState();

    if (hasDetailedDescription() || exampleFlag)
    {
      writeMoreLink(ol,anchor());
    }

    ol.endParagraph();
  }
  ol.writeSynopsis();
}

void ClassDefImpl::writeDetailedDocumentationBody(OutputList &ol) const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);

  ol.startTextBlock();

  if (getLanguage()==SrcLangExt::Cpp)
  {
    writeTemplateSpec(ol,this,compoundTypeString(),getLanguage());
  }

  // repeat brief description
  if (!briefDescription().isEmpty() && repeatBrief)
  {
    ol.generateDoc(briefFile(),briefLine(),this,nullptr,briefDescription(),FALSE,FALSE,
                   QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
  }
  if (!briefDescription().isEmpty() && repeatBrief &&
      !documentation().isEmpty())
  {
    ol.pushGeneratorState();
    ol.disable(OutputType::Html);
    ol.writeString("\n\n");
    ol.popGeneratorState();
  }
  // write documentation
  if (!documentation().isEmpty())
  {
    ol.generateDoc(docFile(),docLine(),this,nullptr,documentation(),TRUE,FALSE,
                   QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
  }
  // write type constraints
  writeTypeConstraints(ol,this,m_impl->typeConstraints);

  // write examples
  if (hasExamples())
  {
    ol.startExamples();
    ol.startDescForItem();
    writeExamples(ol,m_impl->examples);
    ol.endDescForItem();
    ol.endExamples();
  }
  writeSourceDef(ol);
  ol.endTextBlock();
}

bool ClassDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().isEmpty() ||
          (sourceBrowser && getStartBodyLine()!=-1 && getBodyDef()));
}

// write the detailed description for this class
void ClassDefImpl::writeDetailedDescription(OutputList &ol, const QCString &/*pageType*/, bool exampleFlag,
                                        const QCString &title,const QCString &anchor) const
{
  if (hasDetailedDescription() || exampleFlag)
  {
    ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.writeRuler();
    ol.popGeneratorState();

    ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.writeAnchor(QCString(),anchor.isEmpty() ? QCString("details") : anchor);
    ol.popGeneratorState();

    if (!anchor.isEmpty())
    {
      ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.disable(OutputType::Man);
      ol.writeAnchor(getOutputFileBase(),anchor);
      ol.popGeneratorState();
    }

    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();

    writeDetailedDocumentationBody(ol);
  }
  else
  {
    //writeTemplateSpec(ol,this,pageType);
  }
}

QCString ClassDefImpl::generatedFromFiles() const
{
  QCString result;
  SrcLangExt lang = getLanguage();
  size_t numFiles = m_impl->files.size();
  if (lang==SrcLangExt::Fortran)
  {
    result = theTranslator->trGeneratedFromFilesFortran(
          getLanguage()==SrcLangExt::ObjC && m_impl->compType==Interface ? Class : m_impl->compType,
          numFiles==1);
  }
  else if (isJavaEnum())
  {
    result = theTranslator->trEnumGeneratedFromFiles(numFiles==1);
  }
  else if (m_impl->compType==Service)
  {
    result = theTranslator->trServiceGeneratedFromFiles(numFiles==1);
  }
  else if (m_impl->compType==Singleton)
  {
    result = theTranslator->trSingletonGeneratedFromFiles(numFiles==1);
  }
  else
  {
    result = theTranslator->trGeneratedFromFiles(
          getLanguage()==SrcLangExt::ObjC && m_impl->compType==Interface ? Class : m_impl->compType,
          numFiles==1);
  }
  return result;
}

void ClassDefImpl::showUsedFiles(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);


  ol.writeRuler();
  ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Docbook);
    ol.startParagraph();
    ol.parseText(generatedFromFiles());
    ol.endParagraph();
  ol.popGeneratorState();
  ol.disable(OutputType::Docbook);
    ol.parseText(generatedFromFiles());
  ol.enable(OutputType::Docbook);

  bool first=TRUE;
  for (const auto &fd : m_impl->files)
  {
    if (first)
    {
      first=FALSE;
      ol.startItemList();
    }

    ol.startItemListItem();
    QCString path=fd->getPath();
    if (Config_getBool(FULL_PATH_NAMES))
    {
      ol.docify(stripFromPath(path));
    }

    QCString fname = fd->name();
    if (!fd->getVersion().isEmpty()) // append version if available
    {
      fname += " (" + fd->getVersion() + ")";
    }

    // for HTML
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    if (fd->generateSourceFile())
    {
      ol.writeObjectLink(QCString(),fd->getSourceFileBase(),QCString(),fname);
    }
    else if (fd->isLinkable())
    {
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),fname);
    }
    else
    {
      ol.startBold();
      ol.docify(fname);
      ol.endBold();
    }
    ol.popGeneratorState();

    // for other output formats
    ol.pushGeneratorState();
    ol.disable(OutputType::Html);
    if (fd->isLinkable())
    {
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),fname);
    }
    else
    {
      ol.docify(fname);
    }
    ol.popGeneratorState();

    ol.endItemListItem();
  }
  if (!first) ol.endItemList();

  ol.popGeneratorState();
}

int ClassDefImpl::countInheritedByNodes() const
{
  int count=0;
  for (const auto &ibcd : m_impl->inheritedBy)
  {
    const ClassDef *icd=ibcd.classDef;
    if ( icd->isVisibleInHierarchy()) count++;
  }
  return count;
}

int ClassDefImpl::countInheritsNodes() const
{
  int count=0;
  for (const auto &ibcd : m_impl->inherits)
  {
    const ClassDef *icd=ibcd.classDef;
    if ( icd->isVisibleInHierarchy()) count++;
  }
  return count;
}

int ClassDefImpl::countInheritanceNodes() const
{
  return countInheritedByNodes()+countInheritsNodes();
}

void ClassDefImpl::writeInheritanceGraph(OutputList &ol) const
{
  bool haveDot    = Config_getBool(HAVE_DOT);
  auto classGraph = m_impl->typeInheritanceGraph;

  if (classGraph == CLASS_GRAPH_t::NO) return;
  // count direct inheritance relations
  int count=countInheritanceNodes();

  bool renderDiagram = FALSE;
  if (haveDot && (classGraph==CLASS_GRAPH_t::YES || classGraph==CLASS_GRAPH_t::GRAPH))
    // write class diagram using dot
  {
    DotClassGraph inheritanceGraph(this,GraphType::Inheritance);
    if (inheritanceGraph.isTooBig())
    {
       warn_uncond("Inheritance graph for '%s' not generated, too many nodes (%d), threshold is %d. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qPrint(name()), inheritanceGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!inheritanceGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputType::Man);
      ol.startDotGraph();
      ol.parseText(theTranslator->trClassDiagram(displayName()));
      ol.endDotGraph(inheritanceGraph);
      ol.popGeneratorState();
      renderDiagram = TRUE;
    }
  }
  else if ((classGraph==CLASS_GRAPH_t::YES || classGraph==CLASS_GRAPH_t::GRAPH || classGraph==CLASS_GRAPH_t::BUILTIN) && count>0)
    // write class diagram using built-in generator
  {
    ClassDiagram diagram(this); // create a diagram of this class.
    ol.startClassDiagram();
    ol.disable(OutputType::Man);
    ol.parseText(theTranslator->trClassDiagram(displayName()));
    ol.enable(OutputType::Man);
    ol.endClassDiagram(diagram,getOutputFileBase(),displayName());
    renderDiagram = TRUE;
  }

  if (renderDiagram) // if we already show the inheritance relations graphically,
                     // then hide the text version
  {
    ol.disableAllBut(OutputType::Man);
  }

  count = countInheritsNodes();
  if (count>0)
  {
    auto replaceFunc = [this,&ol](size_t entryIndex)
    {
      for (size_t index=0; index<m_impl->inherits.size() ; index++)
      {
        const BaseClassDef &bcd=m_impl->inherits[index];
        const ClassDef *cd=bcd.classDef;

        if (cd->isVisibleInHierarchy()) // filter on the class we want to show
        {
          if (index==entryIndex) // found the requested index
          {
            // use the class name but with the template arguments as given
            // in the inheritance relation
            QCString displayName = insertTemplateSpecifierInScope(
                cd->displayName(),bcd.templSpecifiers);

            if (cd->isLinkable())
            {
              ol.writeObjectLink(cd->getReference(),
                  cd->getOutputFileBase(),
                  cd->anchor(),
                  displayName);
            }
            else
            {
              ol.docify(displayName);
            }
            return;
          }
        }
      }
    };

    ol.startParagraph();
    writeMarkerList(ol,
                    theTranslator->trInheritsList(count).str(),
                    static_cast<size_t>(count),
                    replaceFunc);
    ol.endParagraph();
  }

  // write subclasses
  count = countInheritedByNodes();
  if (count>0)
  {
    auto replaceFunc = [this,&ol](size_t entryIndex)
    {
      for (size_t index=0; index<m_impl->inheritedBy.size() ; index++)
      {
        const BaseClassDef &bcd=m_impl->inheritedBy[index];
        const ClassDef *cd=bcd.classDef;
        if (cd->isVisibleInHierarchy()) // filter on the class we want to show
        {
          if (index==entryIndex) // found the requested index
          {
            if (cd->isLinkable())
            {
              ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),cd->displayName());
              writeInheritanceSpecifier(ol,bcd);
            }
            else
            {
              ol.docify(cd->displayName());
            }
            return;
          }
        }
      }
    };

    ol.startParagraph();
    writeMarkerList(ol,
                    theTranslator->trInheritedByList(count).str(),
                    static_cast<size_t>(count),
                    replaceFunc);
    ol.endParagraph();
  }

  if (renderDiagram)
  {
    ol.enableAll();
  }
}

void ClassDefImpl::writeCollaborationGraph(OutputList &ol) const
{
  if (Config_getBool(HAVE_DOT) && m_impl->hasCollaborationGraph /*&& Config_getBool(COLLABORATION_GRAPH)*/)
  {
    DotClassGraph usageImplGraph(this,GraphType::Collaboration);
    if (usageImplGraph.isTooBig())
    {
       warn_uncond("Collaboration graph for '%s' not generated, too many nodes (%d), threshold is %d. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qPrint(name()), usageImplGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!usageImplGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputType::Man);
      ol.startDotGraph();
      ol.parseText(theTranslator->trCollaborationDiagram(displayName()));
      ol.endDotGraph(usageImplGraph);
      ol.popGeneratorState();
    }
  }
}


void ClassDefImpl::writeIncludeFilesForSlice(OutputList &ol) const
{
  if (m_impl->incInfo)
  {
    QCString nm;
    const StringVector &paths = Config_getList(STRIP_FROM_PATH);
    if (!paths.empty() && m_impl->incInfo->fileDef)
    {
      QCString abs = m_impl->incInfo->fileDef->absFilePath();
      QCString potential;
      size_t length = 0;
      for (const auto &s : paths)
      {
        FileInfo info(s);
        if (info.exists())
        {
          QCString prefix = info.absFilePath();
          if (prefix.at(prefix.length() - 1) != '/')
          {
            prefix += '/';
          }

          if (prefix.length() > length &&
              qstricmp(abs.left(prefix.length()).data(), prefix.data()) == 0) // case insensitive compare
          {
            length = prefix.length();
            potential = abs.right(abs.length() - prefix.length());
          }
        }
      }

      if (length > 0)
      {
        nm = potential;
      }
    }

    if (nm.isEmpty())
    {
      nm = m_impl->incInfo->includeName;
    }

    ol.startParagraph();
    ol.docify(theTranslator->trDefinedIn()+" ");
    ol.startTypewriter();
    ol.docify("<");
    if (m_impl->incInfo->fileDef)
    {
      ol.writeObjectLink(QCString(),m_impl->incInfo->fileDef->includeName(),QCString(),nm);
    }
    else
    {
      ol.docify(nm);
    }
    ol.docify(">");
    ol.endTypewriter();
    ol.endParagraph();
  }

  // Write a summary of the Slice definition including metadata.
  ol.startParagraph();
  ol.startTypewriter();
  if (!m_impl->metaData.isEmpty())
  {
    ol.docify(m_impl->metaData);
    ol.lineBreak();
  }
  if (m_impl->spec.isLocal())
  {
    ol.docify("local ");
  }
  if (m_impl->spec.isInterface())
  {
    ol.docify("interface ");
  }
  else if (m_impl->spec.isStruct())
  {
    ol.docify("struct ");
  }
  else if (m_impl->spec.isException())
  {
    ol.docify("exception ");
  }
  else
  {
    ol.docify("class ");
  }
  ol.docify(stripScope(name()));
  if (!m_impl->inherits.empty())
  {
    if (m_impl->spec.isInterface() || m_impl->spec.isException())
    {
      ol.docify(" extends ");
      bool first=true;
      for (const auto &ibcd : m_impl->inherits)
      {
        if (!first) ol.docify(", ");
        ClassDef *icd = ibcd.classDef;
        ol.docify(icd->name());
        first=false;
      }
    }
    else
    {
      // Must be a class.
      bool implements = false;
      for (const auto &ibcd : m_impl->inherits)
      {
        ClassDef *icd = ibcd.classDef;
        if (icd->isInterface())
        {
          implements = true;
        }
        else
        {
          ol.docify(" extends ");
          ol.docify(icd->name());
        }
      }
      if (implements)
      {
        ol.docify(" implements ");
        bool first = true;
        for (const auto &ibcd : m_impl->inherits)
        {
          ClassDef *icd = ibcd.classDef;
          if (icd->isInterface())
          {
            if (!first) ol.docify(", ");
            first = false;
            ol.docify(icd->name());
          }
        }
      }
    }
  }
  ol.docify(" { ... }");
  ol.endTypewriter();
  ol.endParagraph();
}

void ClassDefImpl::writeIncludeFiles(OutputList &ol) const
{
  if (m_impl->incInfo /*&& Config_getBool(SHOW_HEADERFILE)*/)
  {
    SrcLangExt lang = getLanguage();
    QCString nm=m_impl->incInfo->includeName.isEmpty() ?
      (m_impl->incInfo->fileDef ?
       m_impl->incInfo->fileDef->docName() : QCString()
      ) :
      m_impl->incInfo->includeName;
    if (!nm.isEmpty())
    {
      ol.startParagraph();
      ol.startTypewriter();
      ol.docify(::includeStatement(lang,m_impl->incInfo->kind));
      ol.docify(::includeOpen(lang,m_impl->incInfo->kind));
      ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.docify(nm);
      ol.disableAllBut(OutputType::Html);
      ol.enable(OutputType::Html);
      if (m_impl->incInfo->fileDef)
      {
        ol.writeObjectLink(QCString(),m_impl->incInfo->fileDef->includeName(),QCString(),nm);
      }
      else
      {
        ol.docify(nm);
      }
      ol.popGeneratorState();
      ol.docify(::includeClose(lang,m_impl->incInfo->kind));
      ol.endTypewriter();
      ol.endParagraph();
    }
  }
}

void ClassDefImpl::writeMemberGroups(OutputList &ol,bool showInline) const
{
  // write user defined member groups
  for (const auto &mg : m_impl->memberGroups)
  {
    if (!mg->allMembersInSameSection() || !m_impl->subGrouping) // group is in its own section
    {
      mg->writeDeclarations(ol,this,nullptr,nullptr,nullptr,nullptr,showInline);
    }
    else // add this group to the corresponding member section
    {
      //printf("addToDeclarationSection(%s)\n",qPrint(mg->header()));
      //mg->addToDeclarationSection();
    }
  }
}

void ClassDefImpl::writeNestedClasses(OutputList &ol,const QCString &title) const
{
  // nested classes
  m_impl->innerClasses.writeDeclaration(ol,nullptr,title,TRUE);
}

void ClassDefImpl::writeInlineClasses(OutputList &ol) const
{
  m_impl->innerClasses.writeDocumentation(ol,this);
}

void ClassDefImpl::startMemberDocumentation(OutputList &ol) const
{
  //printf("%s: ClassDefImpl::startMemberDocumentation()\n",qPrint(name()));
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.disable(OutputType::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void ClassDefImpl::endMemberDocumentation(OutputList &ol) const
{
  //printf("%s: ClassDefImpl::endMemberDocumentation()\n",qPrint(name()));
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputType::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void ClassDefImpl::startMemberDeclarations(OutputList &ol) const
{
  //printf("%s: ClassDefImpl::startMemberDeclarations()\n",qPrint(name()));
  ol.startMemberSections();
}

void ClassDefImpl::endMemberDeclarations(OutputList &ol) const
{
  //printf("%s: ClassDefImpl::endMemberDeclarations()\n",qPrint(name()));
  bool inlineInheritedMembers = Config_getBool(INLINE_INHERITED_MEMB);
  if (!inlineInheritedMembers && countAdditionalInheritedMembers()>0)
  {
    ol.startMemberHeader("inherited");
    ol.parseText(theTranslator->trAdditionalInheritedMembers());
    ol.endMemberHeader();
    writeAdditionalInheritedMembers(ol);
  }
  ol.endMemberSections();
}

void ClassDefImpl::writeAuthorSection(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Man);
  ol.writeString("\n");
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}


void ClassDefImpl::writeSummaryLinks(OutputList &ol) const
{
  static bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=TRUE;
  SrcLangExt lang = getLanguage();

  if (lang!=SrcLangExt::VHDL)
  {
    for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
    {
      if (lde->kind()==LayoutDocEntry::ClassNestedClasses &&
          m_impl->innerClasses.declVisible()
         )
      {
        for (const auto &innerCd : m_impl->innerClasses)
        {
          if (!innerCd->isAnonymous() &&
              !innerCd->isExtension() &&
              (innerCd->protection()!=Protection::Private || extractPrivate) &&
              innerCd->visibleInParentsDeclList()
             )
          {
            const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
            ol.writeSummaryLink(QCString(),"nested-classes",ls->title(lang),first);
            first=FALSE;
            break;
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::ClassAllMembersLink &&
               !m_impl->allMemberNameInfoLinkedMap.empty() &&
               !Config_getBool(OPTIMIZE_OUTPUT_FOR_C)
              )
      {
        ol.writeSummaryLink(getMemberListFileName(),"all-members-list",theTranslator->trListOfAllMembers(),first);
        first=FALSE;
      }
      else if (lde->kind()==LayoutDocEntry::MemberDecl)
      {
        const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
        if (lmd)
        {
          MemberList * ml = getMemberList(lmd->type);
          if (ml && ml->declVisible())
          {
            ol.writeSummaryLink(QCString(),ml->listType().toLabel(),lmd->title(lang),first);
            first=FALSE;
          }
        }
      }
    }
  }
  else // VDHL only
  {
    for (const auto &s : m_impl->vhdlSummaryTitles)
    {
      ol.writeSummaryLink(QCString(),convertToId(QCString(s)),QCString(s),first);
      first=FALSE;
    }
  }
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void ClassDefImpl::writeTagFile(TextStream &tagFile) const
{
  if (!isLinkableInProject() || isArtificial()) return;
  tagFile << "  <compound kind=\"";
  if (isFortran() && (compoundTypeString() == "type"))
    tagFile << "struct";
  else
    tagFile << compoundTypeString();
  tagFile << "\"";
  if (isObjectiveC()) { tagFile << " objc=\"yes\""; }
  tagFile << ">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  QCString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "    <filename>" << convertToXML(fn) << "</filename>\n";
  if (!anchor().isEmpty())
  {
    tagFile << "    <anchor>" << convertToXML(anchor()) << "</anchor>\n";
  }
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "    <clangid>" << convertToXML(idStr) << "</clangid>\n";
  }
  for (const Argument &a : m_impl->tempArgs)
  {
    tagFile << "    <templarg>" << convertToXML(a.type);
    if (!a.name.isEmpty())
    {
      tagFile << " " << convertToXML(a.name);
    }
    tagFile << "</templarg>\n";
  }
  for (const auto &ibcd : m_impl->inherits)
  {
    ClassDef *cd=ibcd.classDef;
    if (cd && cd->isLinkable())
    {
      tagFile << "    <base";
      if (ibcd.prot==Protection::Protected)
      {
        tagFile << " protection=\"protected\"";
      }
      else if (ibcd.prot==Protection::Private)
      {
        tagFile << " protection=\"private\"";
      }
      if (ibcd.virt==Specifier::Virtual)
      {
        tagFile << " virtualness=\"virtual\"";
      }
      QCString displayName = insertTemplateSpecifierInScope(
          cd->displayName(),ibcd.templSpecifiers);
      tagFile << ">" << convertToXML(displayName) << "</base>\n";
    }
  }
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::ClassNestedClasses:
        {
          for (const auto &innerCd : m_impl->innerClasses)
          {
            if (innerCd->isLinkableInProject() && innerCd->templateMaster()==nullptr &&
                protectionLevelVisible(innerCd->protection()) &&
                !innerCd->isEmbeddedInOuterScope()
               )
            {
              tagFile << "    <class kind=\"" << innerCd->compoundTypeString() <<
                "\">" << convertToXML(innerCd->name()) << "</class>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd)
          {
            MemberList * ml = getMemberList(lmd->type);
            if (ml)
            {
              ml->writeTagFile(tagFile);
            }
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        {
          for (const auto &mg : m_impl->memberGroups)
          {
            mg->writeTagFile(tagFile);
          }
        }
        break;
     default:
        break;
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>\n";
}

/** Write class documentation inside another container (i.e. a group) */
void ClassDefImpl::writeInlineDocumentation(OutputList &ol) const
{
  bool isSimple = m_impl->isSimple;

  ol.addIndexItem(name(),QCString());
  //printf("ClassDefImpl::writeInlineDocumentation(%s)\n",qPrint(name()));

  // part 1: anchor and title
  QCString s = compoundTypeString()+" "+name();

  // part 1a
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  { // only HTML only
    ol.writeAnchor(QCString(),anchor());
    ol.startMemberDoc(QCString(),QCString(),anchor(),name(),1,1,FALSE);
    ol.startMemberDocName(FALSE);
    ol.parseText(s);
    ol.endMemberDocName();
    ol.endMemberDoc(FALSE);
    ol.writeString("</div>");
    ol.startIndent();
  }
  ol.popGeneratorState();

  // part 1b
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);
  ol.disable(OutputType::Man);
  { // for LaTeX/RTF only
    ol.writeAnchor(getOutputFileBase(),anchor());
  }
  ol.popGeneratorState();

  // part 1c
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);
  {
    // for LaTeX/RTF/Man
    ol.startGroupHeader(1);
    ol.parseText(s);
    ol.endGroupHeader(1);
  }
  ol.popGeneratorState();

  SrcLangExt lang=getLanguage();

  // part 2: the header and detailed description
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        {
          // since we already shown the brief description in the
          // declaration part of the container, so we use this to
          // show the details on top.
          writeDetailedDocumentationBody(ol);
        }
        break;
      case LayoutDocEntry::ClassInheritanceGraph:
        writeInheritanceGraph(ol);
        break;
      case LayoutDocEntry::ClassCollaborationGraph:
        writeCollaborationGraph(ol);
        break;
      case LayoutDocEntry::MemberDeclStart:
        if (!isSimple) startMemberDeclarations(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd)
          {
            ClassDefSet visitedClasses;
            if (!isSimple) writeMemberDeclarations(ol,visitedClasses,lmd->type,lmd->title(lang),lmd->subtitle(lang),TRUE);
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        if (!isSimple) writeMemberGroups(ol,TRUE);
        break;
      case LayoutDocEntry::MemberDeclEnd:
        if (!isSimple) endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::MemberDefStart:
        if (!isSimple) startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
          if (lmd)
          {
            if (isSimple)
            {
              writeSimpleMemberDocumentation(ol,lmd->type);
            }
            else
            {
              writeMemberDocumentation(ol,lmd->type,lmd->title(lang),TRUE);
            }
          }
        }
        break;
      case LayoutDocEntry::MemberDefEnd:
        if (!isSimple) endMemberDocumentation(ol);
        break;
      default:
        break;
    }
  }

  // part 3: close the block
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  { // HTML only
    ol.endIndent();
  }
  ol.popGeneratorState();
}

void ClassDefImpl::writeMoreLink(OutputList &ol,const QCString &anchor) const
{
  // TODO: clean up this mess by moving it to
  // the output generators...
  bool pdfHyperlinks = Config_getBool(PDF_HYPERLINKS);
  bool rtfHyperlinks = Config_getBool(RTF_HYPERLINKS);
  bool usePDFLatex   = Config_getBool(USE_PDFLATEX);

  // HTML only
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  ol.docify(" ");
  ol.startTextLink(getOutputFileBase(),
      anchor.isEmpty() ? QCString("details") : anchor);
  ol.parseText(theTranslator->trMore());
  ol.endTextLink();
  ol.popGeneratorState();

  if (!anchor.isEmpty())
  {
    ol.pushGeneratorState();
    // LaTeX + RTF
    ol.disable(OutputType::Html);
    ol.disable(OutputType::Man);
    ol.disable(OutputType::Docbook);
    if (!(usePDFLatex && pdfHyperlinks))
    {
      ol.disable(OutputType::Latex);
    }
    if (!rtfHyperlinks)
    {
      ol.disable(OutputType::RTF);
    }
    ol.docify(" ");
    ol.startTextLink(getOutputFileBase(), anchor);
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    // RTF only
    ol.disable(OutputType::Latex);
    ol.writeString("\\par");
    ol.popGeneratorState();
  }
}

bool ClassDefImpl::visibleInParentsDeclList() const
{
  bool extractPrivate      = Config_getBool(EXTRACT_PRIVATE);
  bool hideUndocClasses = Config_getBool(HIDE_UNDOC_CLASSES);
  bool extractLocalClasses = Config_getBool(EXTRACT_LOCAL_CLASSES);
  bool linkable = isLinkable();
  return (!isAnonymous() && !isExtension() &&
          (protection()!=Protection::Private || extractPrivate) &&
          (linkable || (!hideUndocClasses && (!isLocal() || extractLocalClasses)))
         );
}

void ClassDefImpl::writeDeclarationLink(OutputList &ol,bool &found,const QCString &header,bool localNames) const
{
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  //bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  SrcLangExt lang = getLanguage();
  if (visibleInParentsDeclList())
  {
    if (!found) // first class
    {
      if (sliceOpt)
      {
        if (compoundType()==Interface)
        {
          ol.startMemberHeader("interfaces");
        }
        else if (compoundType()==Struct)
        {
          ol.startMemberHeader("structs");
        }
        else if (compoundType()==Exception)
        {
          ol.startMemberHeader("exceptions");
        }
        else // compoundType==Class
        {
          ol.startMemberHeader("nested-classes");
        }
      }
      else // non-Slice optimization: single header for class/struct/..
      {
        ol.startMemberHeader("nested-classes");
      }
      if (!header.isEmpty())
      {
        ol.parseText(header);
      }
      else if (lang==SrcLangExt::VHDL)
      {
        ol.parseText(theTranslator->trVhdlType(VhdlSpecifier::ARCHITECTURE,FALSE));
      }
      else
      {
        ol.parseText(lang==SrcLangExt::Fortran ?
            theTranslator->trDataTypes() :
            theTranslator->trCompounds());
      }
      ol.endMemberHeader();
      ol.startMemberList();
      found=TRUE;
    }
    ol.startMemberDeclaration();
    ol.startMemberItem(anchor(),OutputGenerator::MemberItemType::Normal);
    QCString ctype = compoundTypeString();
    QCString cname = displayName(!localNames);

    if (lang!=SrcLangExt::VHDL) // for VHDL we swap the name and the type
    {
      if (isSliceLocal())
      {
        ol.writeString("local ");
      }
      ol.writeString(ctype);
      ol.writeString(" ");
      ol.insertMemberAlign();
    }
    if (isLinkable())
    {
      ol.writeObjectLink(getReference(),
          getOutputFileBase(),
          anchor(),
          cname
          );
    }
    else
    {
      ol.startBold();
      ol.docify(cname);
      ol.endBold();
    }
    if (lang==SrcLangExt::VHDL) // now write the type
    {
      ol.writeString(" ");
      ol.insertMemberAlign();
      ol.writeString(VhdlDocGen::getProtectionName(VhdlDocGen::convert(protection())));
    }
    ol.endMemberItem(OutputGenerator::MemberItemType::Normal);

    // add the brief description if available
    if (!briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
    {
      auto parser { createDocParser() };
      auto ast    { validatingParseDoc(*parser.get(),
                                briefFile(),briefLine(),this,nullptr,
                                briefDescription(),FALSE,FALSE,
                                QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
      if (!ast->isEmpty())
      {
        ol.startMemberDescription(anchor());
        ol.writeDoc(ast.get(),this,nullptr);
        if (isLinkableInProject())
        {
          writeMoreLink(ol,anchor());
        }
        ol.endMemberDescription();
      }
    }
    ol.endMemberDeclaration(anchor(),QCString());
  }
}

void ClassDefImpl::addClassAttributes(OutputList &ol) const
{
  StringVector sl;
  if (isFinal())    sl.emplace_back("final");
  if (isSealed())   sl.emplace_back("sealed");
  if (isAbstract()) sl.emplace_back("abstract");
  if (isExported()) sl.emplace_back("export");
  if (getLanguage()==SrcLangExt::IDL && isPublished()) sl.emplace_back("published");

  for (const auto &sx : m_impl->qualifiers)
  {
    bool alreadyAdded = std::find(sl.begin(), sl.end(), sx) != sl.end();
    if (!alreadyAdded)
    {
      sl.push_back(sx);
    }
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  if (!sl.empty())
  {
    ol.startLabels();
    size_t i=0;
    for (const auto &s : sl)
    {
      i++;
      ol.writeLabel(s.c_str(),i==sl.size());
    }
    ol.endLabels();
  }
  ol.popGeneratorState();
}

void ClassDefImpl::writeDocumentationContents(OutputList &ol,const QCString & /*pageTitle*/) const
{
  ol.startContents();

  QCString pageType = " ";
  pageType += compoundTypeString();

  bool exampleFlag=hasExamples();

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();

  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol,exampleFlag);
        break;
      case LayoutDocEntry::ClassIncludes:
        if (lang==SrcLangExt::Slice)
        {
          writeIncludeFilesForSlice(ol);
        }
        else
        {
          writeIncludeFiles(ol);
        }
        break;
      case LayoutDocEntry::ClassInheritanceGraph:
        writeInheritanceGraph(ol);
        break;
      case LayoutDocEntry::ClassCollaborationGraph:
        writeCollaborationGraph(ol);
        break;
      case LayoutDocEntry::ClassAllMembersLink:
        //writeAllMembersLink(ol); // this is now part of the summary links
        break;
      case LayoutDocEntry::MemberDeclStart:
        startMemberDeclarations(ol);
        break;
      case LayoutDocEntry::MemberGroups:
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          ClassDefSet visitedClasses;
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd)
          {
            writeMemberDeclarations(ol,visitedClasses,lmd->type,lmd->title(lang),lmd->subtitle(lang));
          }
        }
        break;
      case LayoutDocEntry::ClassNestedClasses:
        {
          const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
          if (ls)
          {
            writeNestedClasses(ol,ls->title(lang));
          }
        }
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
          if (ls)
          {
            writeDetailedDescription(ol,pageType,exampleFlag,ls->title(lang));
          }
        }
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::ClassInlineClasses:
        writeInlineClasses(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
          if (lmd)
          {
            writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
          }
        }
        break;
      case LayoutDocEntry::MemberDefEnd:
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::ClassUsedFiles:
        showUsedFiles(ol);
        break;
      case LayoutDocEntry::AuthorSection:
        writeAuthorSection(ol);
        break;
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceNestedConstantGroups:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceConcepts:
      case LayoutDocEntry::NamespaceInterfaces:
      case LayoutDocEntry::NamespaceStructs:
      case LayoutDocEntry::NamespaceExceptions:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::ConceptDefinition:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileConcepts:
      case LayoutDocEntry::FileInterfaces:
      case LayoutDocEntry::FileStructs:
      case LayoutDocEntry::FileExceptions:
      case LayoutDocEntry::FileNamespaces:
      case LayoutDocEntry::FileConstantGroups:
      case LayoutDocEntry::FileIncludes:
      case LayoutDocEntry::FileIncludeGraph:
      case LayoutDocEntry::FileIncludedByGraph:
      case LayoutDocEntry::FileSourceLink:
      case LayoutDocEntry::FileInlineClasses:
      case LayoutDocEntry::GroupClasses:
      case LayoutDocEntry::GroupConcepts:
      case LayoutDocEntry::GroupModules:
      case LayoutDocEntry::GroupInlineClasses:
      case LayoutDocEntry::GroupNamespaces:
      case LayoutDocEntry::GroupDirs:
      case LayoutDocEntry::GroupNestedGroups:
      case LayoutDocEntry::GroupFiles:
      case LayoutDocEntry::GroupGraph:
      case LayoutDocEntry::GroupPageDocs:
      case LayoutDocEntry::ModuleExports:
      case LayoutDocEntry::ModuleClasses:
      case LayoutDocEntry::ModuleConcepts:
      case LayoutDocEntry::ModuleUsedFiles:
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::Class entry list\n",lde->kind());
        break;
    }
  }

  ol.endContents();
}

QCString ClassDefImpl::title() const
{
  QCString pageTitle;
  SrcLangExt lang = getLanguage();

  if (lang==SrcLangExt::Fortran)
  {
    pageTitle = theTranslator->trCompoundReferenceFortran(displayName(),
              m_impl->compType,
              !m_impl->tempArgs.empty());
  }
  else if (lang==SrcLangExt::Slice)
  {
    pageTitle = theTranslator->trCompoundReferenceSlice(displayName(),
              m_impl->compType,
              isSliceLocal());
  }
  else if (lang==SrcLangExt::VHDL)
  {
    pageTitle = theTranslator->trCustomReference(VhdlDocGen::getClassTitle(this));
  }
  else if (isJavaEnum())
  {
    pageTitle = theTranslator->trEnumReference(displayName());
  }
  else if (m_impl->compType==Service)
  {
    pageTitle = theTranslator->trServiceReference(displayName());
  }
  else if (m_impl->compType==Singleton)
  {
    pageTitle = theTranslator->trSingletonReference(displayName());
  }
  else
  {
    if (Config_getBool(HIDE_COMPOUND_REFERENCE))
    {
      pageTitle = displayName();
    }
    else
    {
      pageTitle = theTranslator->trCompoundReference(displayName(),
                m_impl->compType == Interface && getLanguage()==SrcLangExt::ObjC ? Class : m_impl->compType,
                !m_impl->tempArgs.empty());
    }
  }
  return pageTitle;
}

// write all documentation for this class
void ClassDefImpl::writeDocumentation(OutputList &ol) const
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  //bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  QCString pageTitle = title();

  HighlightedItem hli = HighlightedItem::None;
  if (sliceOpt)
  {
    if (compoundType()==Interface)
    {
      hli = HighlightedItem::InterfaceVisible;
    }
    else if (compoundType()==Struct)
    {
      hli = HighlightedItem::StructVisible;
    }
    else if (compoundType()==Exception)
    {
      hli = HighlightedItem::ExceptionVisible;
    }
    else
    {
      hli = HighlightedItem::ClassVisible;
    }
  }
  else
  {
    hli = HighlightedItem::ClassVisible;
  }

  startFile(ol,getOutputFileBase(),name(),pageTitle,hli,!generateTreeView);
  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope)
    {
      writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }

  startTitle(ol,getOutputFileBase(),this);
  ol.parseText(pageTitle);
  addClassAttributes(ol);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),displayName());
  writeDocumentationContents(ol,pageTitle);

  endFileWithNavPath(ol,this);

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    writeMemberPages(ol);
  }
}

void ClassDefImpl::writeMemberPages(OutputList &ol) const
{
  ///////////////////////////////////////////////////////////////////////////
  //// Member definitions on separate pages
  ///////////////////////////////////////////////////////////////////////////

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  for (const auto &ml : m_impl->memberLists)
  {
    if (ml->numDocMembers()>ml->numDocEnumValues() && ml->listType().isDetailed())
    {
      ml->writeDocumentationPage(ol,displayName(),this);
    }
  }

  ol.popGeneratorState();
}

void ClassDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  for (auto &mni : m_impl->allMemberNameInfoLinkedMap)
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md=mi->memberDef();
      if (md->getClassDef()==this && md->isLinkable() && !md->isEnumValue())
      {
        if (md->isLinkableInProject())
        {
          if (md==currentMd) // selected item => highlight
          {
            ol.writeString("          <tr><td class=\"navtabHL\">");
          }
          else
          {
            ol.writeString("          <tr><td class=\"navtab\">");
          }
          ol.writeString("<a class=\"navtab\" ");
          ol.writeString("href=\"");
          if (createSubDirs) ol.writeString("../../");
          QCString url = md->getOutputFileBase();
          addHtmlExtensionIfMissing(url);
          ol.writeString(url+"#"+md->anchor());
          ol.writeString("\">");
          ol.writeString(convertToHtml(md->name()));
          ol.writeString("</a>");
          ol.writeString("</td></tr>\n");
        }
      }
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}



void ClassDefImpl::writeDocumentationForInnerClasses(OutputList &ol) const
{
  // write inner classes after the parent, so the tag files contain
  // the definition in proper order!
  for (const auto &innerCd : m_impl->innerClasses)
  {
    if (
        innerCd->isLinkableInProject() && innerCd->templateMaster()==nullptr &&
        protectionLevelVisible(innerCd->protection()) &&
        !innerCd->isEmbeddedInOuterScope()
       )
    {
      msg("Generating docs for nested compound %s...\n",qPrint(innerCd->name()));
      innerCd->writeDocumentation(ol);
      innerCd->writeMemberList(ol);
    }
    innerCd->writeDocumentationForInnerClasses(ol);
  }
}

// write the list of all (inherited) members for this class
void ClassDefImpl::writeMemberList(OutputList &ol) const
{
  bool cOpt    = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
  //bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  if (m_impl->allMemberNameInfoLinkedMap.empty() || cOpt) return;
  // only for HTML
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  HighlightedItem hli = HighlightedItem::None;
  if (sliceOpt)
  {
    if (compoundType()==Interface)
    {
      hli = HighlightedItem::InterfaceVisible;
    }
    else if (compoundType()==Struct)
    {
      hli = HighlightedItem::StructVisible;
    }
    else if (compoundType()==Exception)
    {
      hli = HighlightedItem::ExceptionVisible;
    }
    else
    {
      hli = HighlightedItem::ClassVisible;
    }
  }
  else
  {
    hli = HighlightedItem::ClassVisible;
  }

  QCString memListFile = getMemberListFileName();
  startFile(ol,memListFile,memListFile,theTranslator->trMemberList(),hli,!generateTreeView,getOutputFileBase());
  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope)
    {
      writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }
  startTitle(ol,QCString());
  ol.parseText(displayName()+" "+theTranslator->trMemberList());
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startParagraph();
  ol.parseText(theTranslator->trThisIsTheListOfAllMembers());
  ol.docify(" ");
  ol.writeObjectLink(getReference(),getOutputFileBase(),anchor(),displayName());
  ol.parseText(theTranslator->trIncludingInheritedMembers());
  ol.endParagraph();

  //ol.startItemList();

  bool first = true; // to prevent empty table
  int idx=0;
  for (auto &mni : m_impl->allMemberNameInfoLinkedMap)
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md=mi->memberDef();
      const ClassDef  *cd=md->getClassDef();
      Protection prot = mi->prot();
      Specifier virt=md->virtualness();

      //printf("%s: Member %s of class %s md->protection()=%d mi->prot=%d prot=%d inherited=%d\n",
      //    qPrint(name()),qPrint(md->name()),qPrint(cd->name()),md->protection(),mi->prot,prot,mi->inherited);

      if (cd && !md->name().isEmpty() && !md->isAnonymous())
      {
        bool memberWritten=FALSE;
        if (cd->isLinkable() && md->isLinkable())
          // create a link to the documentation
        {
          QCString name=mi->ambiguityResolutionScope()+md->name();
          //ol.writeListItem();
          if (first)
          {
            ol.writeString("<table class=\"directory\">\n");
            first = false;
          }
          ol.writeString("  <tr");
          if ((idx&1)==0) ol.writeString(" class=\"even\""); else ol.writeString(" class=\"odd\"");
          idx++;
          ol.writeString("><td class=\"entry\">");
          if (cd->isObjectiveC())
          {
            if (md->isObjCMethod())
            {
              if (md->isStatic())
                ol.writeString("+&#160;</td><td>");
              else
                ol.writeString("-&#160;</td><td>");
            }
            else
              ol.writeString("</td><td class=\"entry\">");
          }
          if (md->isObjCMethod())
          {
            ol.writeObjectLink(md->getReference(),
                md->getOutputFileBase(),
                md->anchor(),md->name());
          }
          else
          {
            //Definition *bd = md->getGroupDef();
            //if (bd==nullptr) bd=cd;
            ol.writeObjectLink(md->getReference(),
                md->getOutputFileBase(),
                md->anchor(),name);

            if ( md->isFunction() || md->isSignal() || md->isSlot() ||
                (md->isFriend() && !md->argsString().isEmpty()))
              ol.docify(md->argsString());
            else if (md->isEnumerate())
              ol.parseText(" "+theTranslator->trEnumName());
            else if (md->isEnumValue())
              ol.parseText(" "+theTranslator->trEnumValue());
            else if (md->isTypedef())
              ol.docify(" typedef");
            else if (md->isFriend() && md->typeString()=="friend class")
              ol.docify(" class");
            //ol.writeString("\n");
          }
          ol.writeString("</td>");
          memberWritten=TRUE;
        }
        else if (!cd->isArtificial() &&
                 !Config_getBool(HIDE_UNDOC_MEMBERS) &&
                  (protectionLevelVisible(md->protection()) || md->isFriend())
                ) // no documentation,
                  // generate link to the class instead.
        {
          //ol.writeListItem();
          if (first)
          {
            ol.writeString("<table class=\"directory\">\n");
            first = false;
          }
          ol.writeString("  <tr bgcolor=\"#f0f0f0\"");
          if ((idx&1)==0) ol.writeString(" class=\"even\""); else ol.writeString(" class=\"odd\"");
          idx++;
          ol.writeString("><td class=\"entry\">");
          if (cd->isObjectiveC())
          {
            if (md->isObjCMethod())
            {
              if (md->isStatic())
                ol.writeString("+&#160;</td><td class=\"entry\">");
              else
                ol.writeString("-&#160;</td><td class=\"entry\">");
            }
            else
              ol.writeString("</td><td class=\"entry\">");
          }
          ol.startBold();
          ol.docify(md->name());
          ol.endBold();
          if (!md->isObjCMethod())
          {
            if ( md->isFunction() || md->isSignal() || md->isSlot() )
              ol.docify(md->argsString());
            else if (md->isEnumerate())
              ol.parseText(" "+theTranslator->trEnumName());
            else if (md->isEnumValue())
              ol.parseText(" "+theTranslator->trEnumValue());
            else if (md->isTypedef())
              ol.docify(" typedef");
          }
          ol.writeString(" (");
          ol.parseText(theTranslator->trDefinedIn()+" ");
          if (cd->isLinkable())
          {
            ol.writeObjectLink(
                cd->getReference(),
                cd->getOutputFileBase(),
                cd->anchor(),
                cd->displayName());
          }
          else
          {
            ol.startBold();
            ol.docify(cd->displayName());
            ol.endBold();
          }
          ol.writeString(")");
          ol.writeString("</td>");
          memberWritten=TRUE;
        }
        if (memberWritten)
        {
          ol.writeString("<td class=\"entry\">");
          ol.writeObjectLink(cd->getReference(),
                             cd->getOutputFileBase(),
                             cd->anchor(),
                             md->category() ?
                                md->category()->displayName() :
                                cd->displayName());
          ol.writeString("</td>");
          ol.writeString("<td class=\"entry\">");
        }
        SrcLangExt lang = md->getLanguage();
        if (
            (prot!=Protection::Public || (virt!=Specifier::Normal && getLanguage()!=SrcLangExt::ObjC) ||
             md->isFriend() || md->isRelated() || md->isExplicit() ||
             md->isMutable() || (md->isInline() && Config_getBool(INLINE_INFO)) ||
             md->isSignal() || md->isSlot() ||
             (getLanguage()==SrcLangExt::IDL &&
              (md->isOptional() || md->isAttribute() || md->isUNOProperty())) ||
             md->isStatic() || lang==SrcLangExt::VHDL
            )
            && memberWritten)
        {
          StringVector sl;
          if (lang==SrcLangExt::VHDL)
          {
            sl.push_back(theTranslator->trVhdlType(md->getVhdlSpecifiers(),TRUE).str()); //append vhdl type
          }
          else if (md->isFriend()) sl.emplace_back("friend");
          else if (md->isRelated()) sl.emplace_back("related");
          else
          {
            if (Config_getBool(INLINE_INFO) && md->isInline())
                                                   sl.emplace_back("inline");
            if (md->isExplicit())                  sl.emplace_back("explicit");
            if (md->isMutable())                   sl.emplace_back("mutable");
            if (prot==Protection::Protected)       sl.emplace_back("protected");
            else if (prot==Protection::Private)    sl.emplace_back("private");
            else if (prot==Protection::Package)    sl.emplace_back("package");
            if (virt==Specifier::Virtual && getLanguage()!=SrcLangExt::ObjC)
                                                   sl.emplace_back("virtual");
            else if (virt==Specifier::Pure)        sl.emplace_back("pure virtual");
            if (md->isStatic())                    sl.emplace_back("static");
            if (md->isSignal())                    sl.emplace_back("signal");
            if (md->isSlot())                      sl.emplace_back("slot");
// this is the extra member page
            if (md->isOptional())                  sl.emplace_back("optional");
            if (md->isAttribute())                 sl.emplace_back("attribute");
            if (md->isUNOProperty())               sl.emplace_back("property");
            if (md->isReadonly())                  sl.emplace_back("readonly");
            if (md->isBound())                     sl.emplace_back("bound");
            if (md->isRemovable())                 sl.emplace_back("removable");
            if (md->isConstrained())               sl.emplace_back("constrained");
            if (md->isTransient())                 sl.emplace_back("transient");
            if (md->isMaybeVoid())                 sl.emplace_back("maybevoid");
            if (md->isMaybeDefault())              sl.emplace_back("maybedefault");
            if (md->isMaybeAmbiguous())            sl.emplace_back("maybeambiguous");
          }
          bool firstSpan=true;
          for (const auto &s : sl)
          {
            if (!firstSpan)
            {
              ol.writeString("</span><span class=\"mlabel\">");
            }
            else
            {
              ol.writeString("<span class=\"mlabel\">");
              firstSpan=false;
            }
            ol.docify(s.c_str());
          }
          if (!firstSpan) ol.writeString("</span>");
        }
        if (memberWritten)
        {
          ol.writeString("</td>");
          ol.writeString("</tr>\n");
        }
      }
    }
  }
  //ol.endItemList();

  if (!first) ol.writeString("</table>");

  endFile(ol);
  ol.popGeneratorState();
}

// add a reference to an example
bool ClassDefImpl::addExample(const QCString &anchor,const QCString &nameStr, const QCString &file)
{
  return m_impl->examples.inSort(Example(anchor,nameStr,file));
}

// returns TRUE if this class is used in an example
bool ClassDefImpl::hasExamples() const
{
  return !m_impl->examples.empty();
}

void ClassDefImpl::addTypeConstraint(const QCString &typeConstraint,const QCString &type)
{
  //printf("addTypeConstraint(%s,%s)\n",qPrint(type),qPrint(typeConstraint));
  bool hideUndocRelation = Config_getBool(HIDE_UNDOC_RELATIONS);
  if (typeConstraint.isEmpty() || type.isEmpty()) return;
  SymbolResolver resolver(getFileDef());
  ClassDefMutable *cd = resolver.resolveClassMutable(this,typeConstraint);
  if (cd==nullptr && !hideUndocRelation)
  {
    cd = toClassDefMutable(
           Doxygen::hiddenClassLinkedMap->add(typeConstraint,
             std::unique_ptr<ClassDef>(
               new ClassDefImpl(
                 getDefFileName(),getDefLine(),
                 getDefColumn(),
                 typeConstraint,
                 ClassDef::Class))));
    if (cd)
    {
      cd->setUsedOnly(TRUE);
      cd->setLanguage(getLanguage());
      //printf("Adding undocumented constraint '%s' to class %s on type %s\n",
      //       qPrint(typeConstraint),qPrint(name()),qPrint(type));
    }
  }
  if (cd)
  {
    auto it = std::find_if(m_impl->constraintClassList.begin(),
                           m_impl->constraintClassList.end(),
                           [&cd](const auto &ccd) { return ccd.classDef==cd; });

    if (it==m_impl->constraintClassList.end())
    {
      m_impl->constraintClassList.emplace_back(cd);
      it = m_impl->constraintClassList.end()-1;
    }
    (*it).addAccessor(type);
    //printf("Adding constraint '%s' to class %s on type %s\n",
    //       qPrint(typeConstraint),qPrint(name()),qPrint(type));
  }
}

// Java Type Constrains: A<T extends C & I>
void ClassDefImpl::addTypeConstraints()
{
  for (const Argument &a : m_impl->tempArgs)
  {
    if (!a.typeConstraint.isEmpty())
    {
      QCString typeConstraint;
      int i=0,p=0;
      while ((i=a.typeConstraint.find('&',p))!=-1) // typeConstraint="A &I" for C<T extends A & I>
      {
        typeConstraint = a.typeConstraint.mid(p,i-p).stripWhiteSpace();
        addTypeConstraint(typeConstraint,a.type);
        p=i+1;
      }
      typeConstraint = a.typeConstraint.right(a.typeConstraint.length()-p).stripWhiteSpace();
      addTypeConstraint(typeConstraint,a.type);
    }
  }
}

// C# Type Constraints: D<T> where T : C, I
void ClassDefImpl::setTypeConstraints(const ArgumentList &al)
{
  m_impl->typeConstraints = al;
}

void ClassDefImpl::setTemplateArguments(const ArgumentList &al)
{
  m_impl->tempArgs = al;
}

static bool hasNonReferenceSuperClassRec(const ClassDef *cd,int level)
{
  bool found=!cd->isReference() && cd->isLinkableInProject() && !cd->isHidden();
  if (found)
  {
    return TRUE; // we're done if this class is not a reference
  }
  for (const auto &ibcd : cd->subClasses())
  {
    const ClassDef *bcd=ibcd.classDef;
    if (level>256)
    {
      err("Possible recursive class relation while inside %s and looking for base class %s\n",qPrint(cd->name()),qPrint(bcd->name()));
      return FALSE;
    }
    // recurse into the super class branch
    found = found || hasNonReferenceSuperClassRec(bcd,level+1);
    if (!found)
    {
      // look for template instances that might have non-reference super classes
      for (const auto &cil : bcd->getTemplateInstances())
      {
        // recurse into the template instance branch
        found = hasNonReferenceSuperClassRec(cil.classDef,level+1);
        if (found) break;
      }
    }
    else
    {
      break;
    }
  }
  return found;
}

/*! Returns \c TRUE iff this class or a class inheriting from this class
 *  is \e not defined in an external tag file.
 */
bool ClassDefImpl::hasNonReferenceSuperClass() const
{
  return hasNonReferenceSuperClassRec(this,0);
}

QCString ClassDefImpl::requiresClause() const
{
  return m_impl->requiresClause;
}

void ClassDefImpl::setRequiresClause(const QCString &req)
{
  m_impl->requiresClause = req;
}

/*! called from MemberDef::writeDeclaration() to (recursively) write the
 *  definition of an anonymous struct, union or class.
 */
void ClassDefImpl::writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,int indentLevel,
    const ClassDef *inheritedFrom,const QCString &inheritId) const
{
  //printf("ClassName='%s' inGroup=%d\n",qPrint(name()),inGroup);

  ol.docify(compoundTypeString());
  QCString cn = displayName(FALSE);
  if (!cn.isEmpty())
  {
    ol.docify(" ");
    if (md && isLinkable())
    {
      ol.writeObjectLink(QCString(),QCString(),md->anchor(),cn);
    }
    else
    {
      ol.startBold();
      ol.docify(cn);
      ol.endBold();
    }
  }
  ol.docify(" {");
  ol.endMemberItem(OutputGenerator::MemberItemType::AnonymousStart);
  ol.endMemberDeclaration(md ? md->anchor() : QCString(),inheritId);

  // write user defined member groups
  for (const auto &mg : m_impl->memberGroups)
  {
    mg->writePlainDeclarations(ol,inGroup,this,nullptr,nullptr,nullptr,nullptr,indentLevel,inheritedFrom,inheritId);
  }

  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd)
      {
        writePlainMemberDeclaration(ol,lmd->type,inGroup,indentLevel,inheritedFrom,inheritId);
      }
    }
  }
}

/*! a link to this class is possible within this project */
bool ClassDefImpl::isLinkableInProject() const
{
  bool extractLocal   = Config_getBool(EXTRACT_LOCAL_CLASSES);
  bool extractStatic  = Config_getBool(EXTRACT_STATIC);
  bool hideUndoc      = Config_getBool(HIDE_UNDOC_CLASSES);
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkableInProject();
  }
  else
  {
    //printf("%s::isLinkableInProject() conditions: artificial=%d hidden=%d anonymous=%d protection=%d local=%d docs=%d static=%d ref=%d\n",
    //      qPrint(name()),
    //      !isArtificial(),
    //      !isHidden(),
    //      !isAnonymous(),
    //      m_impl->prot,
    //      !m_impl->isLocal   || extractLocal,
    //      hasDocumentation() || !hideUndoc,
    //      !m_impl->isStatic  || extractStatic,
    //      !isReference());
    return
      !isArtificial() && !isHidden() &&            /* not hidden */
      !isAnonymous() &&                            /* not anonymous */
      protectionLevelVisible(m_impl->prot)      && /* private/internal */
      (!m_impl->isLocal      || extractLocal)   && /* local */
      (hasDocumentation()    || !hideUndoc)     && /* documented */
      (!m_impl->isStatic     || extractStatic)  && /* static */
      !isReference();                              /* not an external reference */
  }
}

bool ClassDefImpl::isLinkable() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkable();
  }
  else
  {
    return isReference() || isLinkableInProject();
  }
}


/*! the class is visible in a class diagram, or class hierarchy */
bool ClassDefImpl::isVisibleInHierarchy() const
{
  bool allExternals     = Config_getBool(ALLEXTERNALS);
  bool hideUndocClasses = Config_getBool(HIDE_UNDOC_CLASSES);
  bool extractStatic    = Config_getBool(EXTRACT_STATIC);

  return // show all classes or a subclass is visible
      ((allExternals && !isArtificial()) || hasNonReferenceSuperClass()) &&
      // and not an anonymous compound
      !isAnonymous() &&
      // and not privately inherited
      protectionLevelVisible(m_impl->prot) &&
      // documented or shown anyway or documentation is external
      (hasDocumentation() ||
       !hideUndocClasses ||
       (m_impl->templateMaster && m_impl->templateMaster->hasDocumentation()) ||
       isReference()
      ) &&
      // is not part of an unnamed namespace or shown anyway
      (!m_impl->isStatic || extractStatic);
}

bool ClassDefImpl::hasDocumentation() const
{
  return DefinitionMixin::hasDocumentation();
}

//----------------------------------------------------------------------
// recursive function:
// returns the distance to the base class definition 'bcd' represents an (in)direct base
// class of class definition 'cd' or nullptr if it does not.

int ClassDefImpl::isBaseClass(const ClassDef *bcd, bool followInstances,const QCString &templSpec) const
{
  int distance=0;
  //printf("isBaseClass(cd=%s) looking for %s templSpec=%s\n",qPrint(name()),qPrint(bcd->name()),qPrint(templSpec));
  for (const auto &bclass : baseClasses())
  {
    const ClassDef *ccd = bclass.classDef;
    if (!followInstances && ccd->templateMaster())
    {
      ccd=ccd->templateMaster();
    }
    if (ccd==bcd && (templSpec.isEmpty() || templSpec==bclass.templSpecifiers))
    {
      distance=1;
      break; // no shorter path possible
    }
    else
    {
      int d = ccd->isBaseClass(bcd,followInstances,templSpec);
      if (d>256)
      {
        err("Possible recursive class relation while inside %s and looking for base class %s\n",qPrint(name()),qPrint(bcd->name()));
        return 0;
      }
      else if (d>0) // path found
      {
        if (distance==0 || d+1<distance) // update if no path found yet or shorter path found
        {
          distance=d+1;
        }
      }
    }
  }
  return distance;
}

//----------------------------------------------------------------------

bool ClassDefImpl::isSubClass(ClassDef *cd,int level) const
{
  bool found=FALSE;
  if (level>256)
  {
    err("Possible recursive class relation while inside %s and looking for derived class %s\n",qPrint(name()),qPrint(cd->name()));
    return FALSE;
  }
  for (const auto &iscd : subClasses())
  {
    ClassDef *ccd=iscd.classDef;
    found = (ccd==cd) || ccd->isSubClass(cd,level+1);
    if (found) break;
  }
  return found;
}

//----------------------------------------------------------------------------

static bool isStandardFunc(const MemberDef *md)
{
  return md->name()=="operator=" || // assignment operator
         md->isConstructor() ||     // constructor
         md->isDestructor();        // destructor
}

void ClassDefImpl::mergeMembersFromBaseClasses(bool mergeVirtualBaseClass)
{
  SrcLangExt lang = getLanguage();
  QCString sep=getLanguageSpecificSeparator(lang,TRUE);
  size_t sepLen = sep.length();
  bool inlineInheritedMembers = Config_getBool(INLINE_INHERITED_MEMB);
  bool extractPrivate         = Config_getBool(EXTRACT_PRIVATE);

  //printf("  mergeMembers for %s mergeVirtualBaseClass=%d\n",qPrint(name()),mergeVirtualBaseClass);
  // the merge the base members with this class' members
  for (const auto &bcd : baseClasses())
  {
    ClassDefMutable *bClass=toClassDefMutable(bcd.classDef);
    if (bClass)
    {
      const MemberNameInfoLinkedMap &srcMnd  = bClass->memberNameInfoLinkedMap();
      MemberNameInfoLinkedMap &dstMnd        = m_impl->allMemberNameInfoLinkedMap;

      for (auto &srcMni : srcMnd)
      {
        MemberNameInfo *dstMni=dstMnd.find(srcMni->memberName());
        if (dstMni)
          // a member with that name is already in the class.
          // the member may hide or reimplement the one in the sub class
          // or there may be another path to the base class that is already
          // visited via another branch in the class hierarchy.
        {
          //printf("    %s hides member name %s\n",qPrint(bClass->name()),qPrint(srcMni->memberName()));
          for (auto &srcMi : *srcMni)
          {
            MemberDef *srcMd = srcMi->memberDef();
            bool found=FALSE;
            bool ambiguous=FALSE;
            bool hidden=FALSE;
            const ClassDef *srcCd = srcMd->getClassDef();
            for (auto &dstMi : *dstMni)
            {
              const MemberDef *dstMd = dstMi->memberDef();
              if (srcMd!=dstMd) // different members
              {
                const ClassDef *dstCd = dstMd->getClassDef();
                //printf("  Is %s a base class of %s?\n",qPrint(srcCd->name()),qPrint(dstCd->name()));
                if (srcCd==dstCd || dstCd->isBaseClass(srcCd,TRUE))
                  // member is in the same or a base class
                {
                  const ArgumentList &srcAl = srcMd->argumentList();
                  const ArgumentList &dstAl = dstMd->argumentList();
                  found=matchArguments2(
                      srcMd->getOuterScope(),srcMd->getFileDef(),&srcAl,
                      dstMd->getOuterScope(),dstMd->getFileDef(),&dstAl,
                      TRUE,getLanguage()
                      );
                  //printf("      Yes, matching (%s<->%s): %d\n",
                  //    qPrint(argListToString(srcMd->argumentList())),
                  //    qPrint(argListToString(dstMd->argumentList())),
                  //    found);
                  hidden = hidden  || !found;
                }
                else // member is in a non base class => multiple inheritance
                     // using the same base class.
                {
                  //printf("       $$ Existing member %s %s add scope %s\n",
                  //    qPrint(dstMi->ambiguityResolutionScope()),
                  //    qPrint(dstMd->name()),
                  //    qPrint(dstMi->scopePath().left(dstMi->scopePath().find("::")+2)));

                  QCString scope=dstMi->scopePath().left(dstMi->scopePath().find(sep)+sepLen);
                  if (scope!=dstMi->ambiguityResolutionScope().left(scope.length()))
                  {
                    dstMi->setAmbiguityResolutionScope(scope+dstMi->ambiguityResolutionScope());
                  }
                  ambiguous=TRUE;
                }
              }
              else // same members
              {
                // do not add if base class is virtual or
                // if scope paths are equal or
                // if base class is an interface (and thus implicitly virtual).
                //printf("      same member found srcMi->virt=%d dstMi->virt=%d\n",srcMi->virt(),dstMi->virt());
                if ((srcMi->virt()!=Specifier::Normal && dstMi->virt()!=Specifier::Normal) ||
                    bClass->name()+sep+srcMi->scopePath() == dstMi->scopePath() ||
                    dstMd->getClassDef()->compoundType()==Interface
                   )
                {
                  found=TRUE;
                }
                else // member can be reached via multiple paths in the
                     // inheritance tree
                {
                  //printf("      $$ Existing member %s %s add scope %s\n",
                  //    qPrint(dstMi->ambiguityResolutionScope()),
                  //    qPrint(dstMd->name()),
                  //    qPrint(dstMi->scopePath().left(dstMi->scopePath().find("::")+2)));

                  QCString scope=dstMi->scopePath().left(dstMi->scopePath().find(sep)+sepLen);
                  if (scope!=dstMi->ambiguityResolutionScope().left(scope.length()))
                  {
                    dstMi->setAmbiguityResolutionScope(dstMi->ambiguityResolutionScope()+scope);
                  }
                  ambiguous=TRUE;
                }
              }
              if (found) break;
            }
            //printf("      member %s::%s hidden %d ambiguous %d srcMi->ambigClass=%p found=%d\n",
            //    qPrint(srcCd->name()),qPrint(srcMd->name()),hidden,ambiguous,
            //    (void*)srcMi->ambigClass(),found);

            // TODO: fix the case where a member is hidden by inheritance
            //       of a member with the same name but with another prototype,
            //       while there is more than one path to the member in the
            //       base class due to multiple inheritance. In this case
            //       it seems that the member is not reachable by prefixing a
            //       scope name either (according to my compiler). Currently,
            //       this case is shown anyway.
            if (!found && srcMd->protection()!=Protection::Private && !srcMd->isFriend() && srcMi->virtualBaseClass()==mergeVirtualBaseClass)
            {
              Protection prot = srcMd->protection();
              if (bcd.prot==Protection::Protected && prot==Protection::Public)
              {
                prot = bcd.prot;
              }
              else if (bcd.prot==Protection::Private)
              {
                prot = bcd.prot;
              }

              if (inlineInheritedMembers)
              {
                if (!isStandardFunc(srcMd))
                {
                  //printf("      %s::insertMember(%s)\n",qPrint(name()),qPrint(srcMd->name()));
                  internalInsertMember(srcMd,prot,FALSE);
                }
              }

              Specifier virt=srcMi->virt();
              if (virt==Specifier::Normal && bcd.virt!=Specifier::Normal) virt=bcd.virt;
              bool virtualBaseClass = bcd.virt!=Specifier::Normal;

              std::unique_ptr<MemberInfo> newMi = std::make_unique<MemberInfo>(srcMd,prot,virt,TRUE,virtualBaseClass);
              newMi->setScopePath(bClass->name()+sep+srcMi->scopePath());
              if (ambiguous)
              {
                //printf("$$ New member %s %s add scope %s::\n",
                //     qPrint(srcMi->ambiguityResolutionScope),
                //     qPrint(srcMd->name()),
                //     qPrint(bClass->name()));

                QCString scope=bClass->name()+sep;
                if (scope!=srcMi->ambiguityResolutionScope().left(scope.length()))
                {
                  newMi->setAmbiguityResolutionScope(scope+srcMi->ambiguityResolutionScope());
                }
              }
              if (hidden)
              {
                if (srcMi->ambigClass()==nullptr)
                {
                  newMi->setAmbigClass(bClass);
                  newMi->setAmbiguityResolutionScope(bClass->name()+sep);
                }
                else
                {
                  newMi->setAmbigClass(srcMi->ambigClass());
                  newMi->setAmbiguityResolutionScope(srcMi->ambigClass()->name()+sep);
                }
              }
              dstMni->push_back(std::move(newMi));
            }
          }
        }
        else // base class has a member that is not in the sub class => copy
        {
          //printf("    %s adds member name %s\n",qPrint(bClass->name()),qPrint(srcMni->memberName()));
          // create a deep copy of the list (only the MemberInfo's will be
          // copied, not the actual MemberDef's)
          MemberNameInfo *newMni = dstMnd.add(srcMni->memberName());

          // copy the member(s) from the base to the sub class
          for (auto &mi : *srcMni)
          {
            if (mi->virtualBaseClass()==mergeVirtualBaseClass && !mi->memberDef()->isFriend()) // don't inherit friends
            {
              Protection prot = mi->prot();
              if (bcd.prot==Protection::Protected)
              {
                if (prot==Protection::Public) prot=Protection::Protected;
              }
              else if (bcd.prot==Protection::Private)
              {
                prot=Protection::Private;
              }
              Specifier virt=mi->virt();
              bool virtualBaseClass = bcd.virt!=Specifier::Normal || mi->virtualBaseClass();
              if (virt==Specifier::Normal && bcd.virt!=Specifier::Normal) virt=bcd.virt;
              //printf("      %s::%s: [mi.prot=%d, bcd.prot=%d => prot=%d], [mi.virt=%d, bcd.virt=%d => virt=%d] virtualBase=%d\n",
              //    qPrint(name()),qPrint(mi->memberDef()->name()),
              //    mi->prot(),bcd.prot,prot,
              //    mi->virt(),bcd.virt,virt,
              //    virtualBaseClass
              //    );

              if (prot!=Protection::Private || extractPrivate)
              {

                if (inlineInheritedMembers)
                {
                  if (!isStandardFunc(mi->memberDef()))
                  {
                    //printf("      %s::insertMember '%s'\n",qPrint(name()),qPrint(mi->memberDef()->name()));
                    internalInsertMember(mi->memberDef(),prot,FALSE);
                  }
                }
                //printf("Adding!\n");
                std::unique_ptr<MemberInfo> newMi = std::make_unique<MemberInfo>(mi->memberDef(),prot,virt,TRUE,virtualBaseClass);
                newMi->setScopePath(bClass->name()+sep+mi->scopePath());
                newMi->setAmbigClass(mi->ambigClass());
                newMi->setAmbiguityResolutionScope(mi->ambiguityResolutionScope());
                newMni->push_back(std::move(newMi));
              }
            }
          }
        }
      }
    }
  }
}

/*!
 * recursively merges the 'all members' lists of a class base
 * with that of this class. Must only be called for classes without
 * subclasses!
 */
void ClassDefImpl::mergeMembers()
{
  if (m_impl->membersMerged) return;
  if (getLanguage()==SrcLangExt::Python) return; // python does not have member overloading, see issue 8480

  //printf("> %s::mergeMembers()\n",qPrint(name()));

  m_impl->membersMerged=TRUE;

  // first merge the members of the base class recursively
  for (const auto &bcd : baseClasses())
  {
    ClassDefMutable *bClass=toClassDefMutable(bcd.classDef);
    if (bClass)
    {
      // merge the members in the base class of this inheritance branch first
      bClass->mergeMembers();
    }
  }

  // first merge the member that are not inherited via a virtual base class
  // (as this can end up reimplemented via multiple paths, see #10717 for examples)
  mergeMembersFromBaseClasses(false);
  // then process the member that are inherited via a virtual base class to add the
  // ones that are not reimplemented via any path
  mergeMembersFromBaseClasses(true);

  //printf("< %s::mergeMembers()\n",qPrint(name()));
}

//----------------------------------------------------------------------------

/*! Merges the members of a Objective-C category into this class.
 */
void ClassDefImpl::mergeCategory(ClassDef *cat)
{
  AUTO_TRACE();
  ClassDefMutable *category = toClassDefMutable(cat);
  if (category)
  {
    bool extractLocalMethods = Config_getBool(EXTRACT_LOCAL_METHODS);
    bool makePrivate = category->isLocal();
    // in case extract local methods is not enabled we don't add the methods
    // of the category in case it is defined in the .m file.
    if (makePrivate && !extractLocalMethods) return;
    bool isExtension = category->isExtension();

    category->setCategoryOf(this);
    if (isExtension)
    {
      category->setArtificial(TRUE);

      // copy base classes/protocols from extension
      for (const auto &bcd : category->baseClasses())
      {
        insertBaseClass(bcd.classDef,bcd.usedName,bcd.prot,bcd.virt,bcd.templSpecifiers);
        // correct bcd.classDef so that they do no longer derive from
        // category, but from this class!
        BaseClassList scl = bcd.classDef->subClasses();
        for (auto &scd : scl)
        {
          if (scd.classDef==category)
          {
            scd.classDef=this;
          }
        }
        bcd.classDef->updateSubClasses(scl);
      }
    }
    // make methods private for categories defined in the .m file
    //printf("%s::mergeCategory makePrivate=%d\n",qPrint(name()),makePrivate);

    const MemberNameInfoLinkedMap &srcMnd  = category->memberNameInfoLinkedMap();
    MemberNameInfoLinkedMap &dstMnd        = m_impl->allMemberNameInfoLinkedMap;

    for (auto &srcMni : srcMnd)
    {
      MemberNameInfo *dstMni=dstMnd.find(srcMni->memberName());
      if (dstMni) // method is already defined in the class
      {
        AUTO_TRACE_ADD("Existing member {}",srcMni->memberName());
        const auto &dstMi = dstMni->front();
        const auto &srcMi = srcMni->front();
        if (srcMi && dstMi)
        {
          MemberDefMutable *smdm = toMemberDefMutable(srcMi->memberDef());
          MemberDefMutable *dmdm = toMemberDefMutable(dstMi->memberDef());
          if (smdm && dmdm)
          {
            combineDeclarationAndDefinition(smdm,dmdm);
            dmdm->setCategory(category);
            dmdm->setCategoryRelation(smdm);
            smdm->setCategoryRelation(dmdm);
          }
        }
      }
      else // new method name
      {
        AUTO_TRACE_ADD("New member {}",srcMni->memberName());
        // create a deep copy of the list
        MemberNameInfo *newMni = dstMnd.add(srcMni->memberName());

        // copy the member(s) from the category to this class
        for (auto &mi : *srcMni)
        {
          //printf("Adding '%s'\n",qPrint(mi->memberDef->name()));
          Protection prot = mi->prot();
          //if (makePrivate) prot = Private;
          auto newMd = mi->memberDef()->deepCopy();
          if (newMd)
          {
            auto mmd = toMemberDefMutable(newMd.get());
            AUTO_TRACE_ADD("Copying member {}",mmd->name());
            mmd->moveTo(this);

            auto newMi=std::make_unique<MemberInfo>(newMd.get(),prot,mi->virt(),mi->inherited(),mi->virtualBaseClass());
            newMi->setScopePath(mi->scopePath());
            newMi->setAmbigClass(mi->ambigClass());
            newMi->setAmbiguityResolutionScope(mi->ambiguityResolutionScope());
            newMni->push_back(std::move(newMi));

            // also add the newly created member to the global members list

            QCString name = newMd->name();
            MemberName *mn = Doxygen::memberNameLinkedMap->add(name);

            mmd->setCategory(category);
            mmd->setCategoryRelation(mi->memberDef());
            auto miMmd = toMemberDefMutable(mi->memberDef());
            if (miMmd) miMmd->setCategoryRelation(newMd.get());

            if (makePrivate || isExtension)
            {
              mmd->makeImplementationDetail();
            }
            internalInsertMember(newMd.get(),prot,FALSE);
            mn->push_back(std::move(newMd));
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

void ClassDefImpl::addUsedClass(ClassDef *cd,const QCString &accessName,
               Protection prot)
{
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  bool umlLook = Config_getBool(UML_LOOK);
  if (prot==Protection::Private && !extractPrivate) return;
  //printf("%s::addUsedClass(%s,%s)\n",qPrint(name()),qPrint(cd->name()),accessName);

  auto it = std::find_if(m_impl->usesImplClassList.begin(),
                         m_impl->usesImplClassList.end(),
                         [&cd](const auto &ucd) { return ucd.classDef==cd; });
  if (it==m_impl->usesImplClassList.end())
  {
    m_impl->usesImplClassList.emplace_back(cd);
    //printf("Adding used class %s to class %s via accessor %s\n",
    //    qPrint(cd->name()),qPrint(name()),accessName);
    it = m_impl->usesImplClassList.end()-1;
  }
  QCString acc = accessName;
  if (umlLook)
  {
    switch(prot)
    {
      case Protection::Public:    acc.prepend("+"); break;
      case Protection::Private:   acc.prepend("-"); break;
      case Protection::Protected: acc.prepend("#"); break;
      case Protection::Package:   acc.prepend("~"); break;
    }
  }
  (*it).addAccessor(acc);
}

void ClassDefImpl::addUsedByClass(ClassDef *cd,const QCString &accessName,
               Protection prot)
{
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  bool umlLook = Config_getBool(UML_LOOK);
  if (prot==Protection::Private && !extractPrivate) return;
  //printf("%s::addUsedByClass(%s,%s)\n",qPrint(name()),qPrint(cd->name()),accessName);
  //
  auto it = std::find_if(m_impl->usedByImplClassList.begin(),
                         m_impl->usedByImplClassList.end(),
                         [&cd](const auto &ucd) { return ucd.classDef==cd; });
  if (it==m_impl->usedByImplClassList.end())
  {
     m_impl->usedByImplClassList.emplace_back(cd);
     //printf("Adding used by class %s to class %s\n",
     //    qPrint(cd->name()),qPrint(name()));
     it = m_impl->usedByImplClassList.end()-1;
  }
  QCString acc = accessName;
  if (umlLook)
  {
    switch(prot)
    {
      case Protection::Public:    acc.prepend("+"); break;
      case Protection::Private:   acc.prepend("-"); break;
      case Protection::Protected: acc.prepend("#"); break;
      case Protection::Package:   acc.prepend("~"); break;
    }
  }
  (*it).addAccessor(acc);
}


QCString ClassDefImpl::compoundTypeString() const
{
  return getCompoundTypeString(getLanguage(),m_impl->compType,isJavaEnum());
}

QCString ClassDefImpl::getOutputFileBase() const
{
  bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  if (!Doxygen::generatingXmlOutput)
  {
    Definition *scope=nullptr;
    if (inlineGroupedClasses && !partOfGroups().empty())
    {
      // point to the group that embeds this class
      return partOfGroups().front()->getOutputFileBase();
    }
    else if (inlineSimpleClasses && m_impl->isSimple && !partOfGroups().empty())
    {
      // point to simple struct inside a group
      return partOfGroups().front()->getOutputFileBase();
    }
    else if (inlineSimpleClasses && m_impl->isSimple && (scope=getOuterScope()))
    {
      if (scope==Doxygen::globalScope && getFileDef() && getFileDef()->isLinkableInProject()) // simple struct embedded in file
      {
        return getFileDef()->getOutputFileBase();
      }
      else if (scope->isLinkableInProject()) // simple struct embedded in other container (namespace/group/class)
      {
        return getOuterScope()->getOutputFileBase();
      }
    }
  }
  if (m_impl->templateMaster)
  {
    // point to the template of which this class is an instance
    return m_impl->templateMaster->getOutputFileBase();
  }
  return m_impl->fileName;
}

QCString ClassDefImpl::getInstanceOutputFileBase() const
{
  return m_impl->fileName;
}

QCString ClassDefImpl::getSourceFileBase() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getSourceFileBase();
  }
  else
  {
    return DefinitionMixin::getSourceFileBase();
  }
}

void ClassDefImpl::setGroupDefForAllMembers(GroupDef *gd,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs)
{
  gd->addClass(this);
  //printf("ClassDefImpl::setGroupDefForAllMembers(%s)\n",qPrint(gd->name()));
  for (auto &mni : m_impl->allMemberNameInfoLinkedMap)
  {
    for (auto &mi : *mni)
    {
      MemberDefMutable *md = toMemberDefMutable(mi->memberDef());
      if (md)
      {
        md->setGroupDef(gd,pri,fileName,startLine,hasDocs);
        gd->insertMember(md,TRUE);
        ClassDefMutable *innerClass = toClassDefMutable(md->getClassDefOfAnonymousType());
        if (innerClass) innerClass->setGroupDefForAllMembers(gd,pri,fileName,startLine,hasDocs);
      }
    }
  }
}

void ClassDefImpl::addInnerCompound(Definition *d)
{
  //printf("**** %s::addInnerCompound(%s)\n",qPrint(name()),qPrint(d->name()));
  if (d->definitionType()==Definition::TypeClass) // only classes can be
                                                  // nested in classes.
  {
    m_impl->innerClasses.add(d->localName(),toClassDef(d));
  }
}

const Definition *ClassDefImpl::findInnerCompound(const QCString &name) const
{
  return m_impl->innerClasses.find(name);
}

ClassDef *ClassDefImpl::insertTemplateInstance(const QCString &fileName,
    int startLine, int startColumn, const QCString &templSpec,bool &freshInstance) const
{
  freshInstance = FALSE;
  auto it = std::find_if(m_impl->templateInstances.begin(),
                         m_impl->templateInstances.end(),
                         [&templSpec](const auto &ti) { return templSpec==ti.templSpec; });
  ClassDefMutable *templateClass=nullptr;
  if (it!=m_impl->templateInstances.end())
  {
    templateClass = toClassDefMutable((*it).classDef);
  }
  if (templateClass==nullptr)
  {
    QCString tcname = removeRedundantWhiteSpace(name()+templSpec);
    AUTO_TRACE("New template instance class name='{}' templSpec='{}' inside '{}' hidden={}",
        name(),templSpec,name(),isHidden());

    ClassDef *foundCd = Doxygen::classLinkedMap->find(tcname);
    if (foundCd)
    {
      return foundCd;
    }
    templateClass =
      toClassDefMutable(
          Doxygen::classLinkedMap->add(tcname,
            std::unique_ptr<ClassDef>(
              new ClassDefImpl(fileName,startLine,startColumn,tcname,ClassDef::Class))));
    if (templateClass)
    {
      templateClass->setTemplateMaster(this);
      templateClass->setOuterScope(getOuterScope());
      templateClass->setHidden(isHidden());
      templateClass->setArtificial(isArtificial());
      templateClass->setImplicitTemplateInstance(true);
      m_impl->templateInstances.emplace_back(templSpec,templateClass);

      // also add nested classes
      for (const auto &innerCd : m_impl->innerClasses)
      {
        QCString innerName = tcname+"::"+innerCd->localName();
        ClassDefMutable *innerClass =
          toClassDefMutable(
              Doxygen::classLinkedMap->add(innerName,
                std::unique_ptr<ClassDef>(
                  new ClassDefImpl(fileName,startLine,startColumn,innerName,ClassDef::Class))));
        if (innerClass)
        {
          templateClass->addInnerCompound(innerClass);
          innerClass->setOuterScope(templateClass);
          innerClass->setHidden(isHidden());
          innerClass->setArtificial(TRUE);
          innerClass->setImplicitTemplateInstance(true);
        }
      }
      freshInstance=TRUE;
    }
  }
  return templateClass;
}

void ClassDefImpl::insertExplicitTemplateInstance(ClassDef *templateClass,const QCString &templSpec)
{
  m_impl->templateInstances.emplace_back(templSpec,templateClass);
}

void ClassDefImpl::setTemplateBaseClassNames(const TemplateNameMap &templateNames)
{
  m_impl->templBaseClassNames = templateNames;
}

const TemplateNameMap &ClassDefImpl::getTemplateBaseClassNames() const
{
  return m_impl->templBaseClassNames;
}

void ClassDefImpl::addMembersToTemplateInstance(const ClassDef *cd,const ArgumentList &templateArguments,const QCString &templSpec)
{
  //printf("%s::addMembersToTemplateInstance(%s,%s)\n",qPrint(name()),qPrint(cd->name()),templSpec);
  for (const auto &mni : cd->memberNameInfoLinkedMap())
  {
    for (const auto &mi : *mni)
    {
      auto actualArguments_p = stringToArgumentList(getLanguage(),templSpec);
      MemberDef *md = mi->memberDef();
      auto imd = md->createTemplateInstanceMember(templateArguments,actualArguments_p);
      //printf("%s->setMemberClass(%p)\n",qPrint(imd->name()),this);
      auto mmd = toMemberDefMutable(imd.get());
      mmd->setMemberClass(this);
      mmd->setTemplateMaster(md);
      mmd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
      mmd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
      mmd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
      mmd->setMemberSpecifiers(md->getMemberSpecifiers());
      mmd->setMemberGroupId(md->getMemberGroupId());
      mmd->setArtificial(true);
      insertMember(imd.get());
      //printf("Adding member=%s %s%s to class %s templSpec %s\n",
      //    imd->typeString(),qPrint(imd->name()),imd->argsString(),
      //    qPrint(imd->getClassDef()->name()),templSpec);
      // insert imd in the list of all members
      //printf("Adding member=%s class=%s\n",qPrint(imd->name()),qPrint(name()));
      MemberName *mn = Doxygen::memberNameLinkedMap->add(imd->name());
      mn->push_back(std::move(imd));
    }
  }
  // also instantatie members for nested classes
  for (const auto &innerCd : cd->getClasses())
  {
    ClassDefMutable *ncd = toClassDefMutable(m_impl->innerClasses.find(innerCd->localName()));
    if (ncd)
    {
      ncd->addMembersToTemplateInstance(innerCd,cd->templateArguments(),templSpec);
    }
  }
}

QCString ClassDefImpl::getReference() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getReference();
  }
  else
  {
    return DefinitionMixin::getReference();
  }
}

bool ClassDefImpl::isReference() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isReference();
  }
  else
  {
    return DefinitionMixin::isReference();
  }
}

ArgumentLists ClassDefImpl::getTemplateParameterLists() const
{
  ArgumentLists result;
  Definition *d=getOuterScope();
  while (d && d->definitionType()==Definition::TypeClass)
  {
    result.insert(result.begin(),toClassDef(d)->templateArguments());
    d = d->getOuterScope();
  }
  if (!templateArguments().empty())
  {
    result.push_back(templateArguments());
  }
  return result;
}

QCString ClassDefImpl::qualifiedNameWithTemplateParameters(
    const ArgumentLists *actualParams,uint32_t *actualParamIndex) const
{
  return makeQualifiedNameWithTemplateParameters(this,actualParams,actualParamIndex);
}

QCString ClassDefImpl::className() const
{
  if (m_impl->className.isEmpty())
  {
    return localName();
  }
  else
  {
    return m_impl->className;
  }
}

void ClassDefImpl::setClassName(const QCString &name)
{
  m_impl->className = name;
}

void ClassDefImpl::addListReferences()
{
  SrcLangExt lang = getLanguage();
  if (!isLinkableInProject()) return;
  //printf("ClassDef(%s)::addListReferences()\n",qPrint(name()));
  {
    const RefItemVector &xrefItems = xrefListItems();
    addRefItem(xrefItems,
             qualifiedName(),
             theTranslator->trCompoundType(compoundType(), lang),
             getOutputFileBase(),
             displayName(),
             QCString(),
             this
            );
  }
  for (const auto &mg : m_impl->memberGroups)
  {
    mg->addListReferences(this);
  }
  for (auto &ml : m_impl->memberLists)
  {
    if (ml->listType().isDetailed())
    {
      ml->addListReferences(this);
    }
  }
}

const MemberDef *ClassDefImpl::getMemberByName(const QCString &name) const
{
  const MemberDef *xmd = nullptr;
  MemberNameInfo *mni = m_impl->allMemberNameInfoLinkedMap.find(name);
  if (mni)
  {
    const int maxInheritanceDepth = 100000;
    int mdist=maxInheritanceDepth;
    for (auto &mi : *mni)
    {
      const ClassDef *mcd=mi->memberDef()->getClassDef();
      int m=minClassDistance(this,mcd);
      //printf("found member in %s linkable=%d m=%d\n",
      //    qPrint(mcd->name()),mcd->isLinkable(),m);
      if (m<mdist)
      {
        mdist=m;
        xmd=mi->memberDef();
      }
    }
  }
  //printf("getMemberByName(%s)=%p\n",qPrint(name),xmd);
  return xmd;
}

bool ClassDefImpl::isAccessibleMember(const MemberDef *md) const
{
  return md->getClassDef() && isBaseClass(md->getClassDef(),TRUE,QCString());
}

MemberList *ClassDefImpl::getMemberList(MemberListType lt) const
{
  for (auto &ml : m_impl->memberLists)
  {
    if (ml->listType()==lt)
    {
      return ml.get();
    }
  }
  return nullptr;
}

void ClassDefImpl::addMemberToList(MemberListType lt,MemberDef *md,bool isBrief)
{
  AUTO_TRACE("{} md={} lt={} isBrief={}",name(),md->name(),lt,isBrief);
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_impl->memberLists.get(lt,MemberListContainer::Class);
  ml->setNeedsSorting((isBrief && sortBriefDocs) || (!isBrief && sortMemberDocs));
  ml->push_back(md);

  // for members in the declaration lists we set the section, needed for member grouping
  if (!ml->listType().isDetailed())
  {
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm)
    {
      mdm->setSectionList(this,ml.get());
    }
  }
}

void ClassDefImpl::sortMemberLists()
{
  for (auto &ml : m_impl->memberLists)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }
  std::stable_sort(m_impl->innerClasses.begin(),
            m_impl->innerClasses.end(),
            [](const auto &c1,const auto &c2)
            {
               return Config_getBool(SORT_BY_SCOPE_NAME)                ?
                      qstricmp_sort(c1->name(),      c2->name()     )<0 :
                      qstricmp_sort(c1->className(), c2->className())<0 ;
            });
}

int ClassDefImpl::countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                                      MemberListType lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const
{
  //printf("%s: countMemberDeclarations for %d and %d\n",qPrint(name()),lt.to_int(),lt2.to_int());
  int count=0;
  MemberList * ml  = getMemberList(lt);
  MemberList * ml2 = getMemberList(lt2);
  if (getLanguage()!=SrcLangExt::VHDL) // use specific declarations function
  {
    if (ml)
    {
      count+=ml->numDecMembers();
      //printf("-> ml=%d\n",ml->numDecMembers());
    }
    if (ml2)
    {
      count+=ml2->numDecMembers();
      //printf("-> ml2=%d\n",ml2->numDecMembers());
    }
    // also include grouped members that have their own section in the class (see bug 722759)
    if (inheritedFrom)
    {
      for (const auto &mg : m_impl->memberGroups)
      {
        count+=mg->countGroupedInheritedMembers(lt);
        if (!lt2.isInvalid()) count+=mg->countGroupedInheritedMembers(lt2);
      }
    }
    bool inlineInheritedMembers = Config_getBool(INLINE_INHERITED_MEMB);
    if (!inlineInheritedMembers) // show inherited members as separate lists
    {
      count+=countInheritedDecMembers(lt,inheritedFrom,invert,showAlways,visitedClasses);
    }
  }
  //printf("-> %d\n",count);
  return count;
}

void ClassDefImpl::setAnonymousEnumType()
{
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd)
      {
        MemberList * ml = getMemberList(lmd->type);
        if (ml)
        {
          ml->setAnonymousEnumType();
        }
      }
    }
    else if (lde->kind()==LayoutDocEntry::MemberGroups)
    {
      for (const auto &mg : m_impl->memberGroups)
      {
        mg->setAnonymousEnumType();
      }
    }
  }
}

void ClassDefImpl::countMembers()
{
  for (auto &ml : m_impl->memberLists)
  {
    ml->countDecMembers();
    ml->countDocMembers();
  }
  for (const auto &mg : m_impl->memberGroups)
  {
    mg->countDecMembers();
    mg->countDocMembers();
  }
}

int ClassDefImpl::countInheritedDecMembers(MemberListType lt,
                                       const ClassDef *inheritedFrom,bool invert,bool showAlways,
                                       ClassDefSet &visitedClasses) const
{
  int inhCount = 0;
  int count = countMembersIncludingGrouped(lt,inheritedFrom,FALSE);
  bool process = count>0;
  //printf("%s: countInheritedDecMembers: lt=%d process=%d count=%d invert=%d\n",
  //    qPrint(name()),lt.to_int(),process,count,invert);
  if ((process^invert) || showAlways)
  {
    for (const auto &ibcd : m_impl->inherits)
    {
      ClassDefMutable *icd = toClassDefMutable(ibcd.classDef);
      MemberListType lt1 = MemberListType::Invalid();
      MemberListType lt2 = MemberListType::Invalid();
      if (icd && icd->isLinkable())
      {
        convertProtectionLevel(lt,ibcd.prot,&lt1,&lt2);
        //printf("%s: convert %d->(%d,%d) prot=%d\n",
        //    qPrint(icd->name()),lt.to_int(),lt1.to_int(),lt2.to_int(),ibcd.prot);
        if (visitedClasses.find(icd)==visitedClasses.end())
        {
          visitedClasses.insert(icd); // guard for multiple virtual inheritance
          if (!lt1.isInvalid())
          {
            inhCount+=icd->countMemberDeclarations(lt1,inheritedFrom,lt2,FALSE,TRUE,visitedClasses);
          }
        }
      }
    }
  }
  return inhCount;
}

void ClassDefImpl::getTitleForMemberListType(MemberListType type,
               QCString &title,QCString &subtitle) const
{
  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd && lmd->type==type)
      {
        title = lmd->title(lang);
        subtitle = lmd->subtitle(lang);
        return;
      }
    }
  }
  title="";
  subtitle="";
}

int ClassDefImpl::countAdditionalInheritedMembers() const
{
  int totalCount=0;
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd && lmd->type!=MemberListType::Friends()) // friendship is not inherited
      {
        ClassDefSet visited;
        totalCount+=countInheritedDecMembers(lmd->type,this,TRUE,FALSE,visited);
      }
    }
  }
  //printf("countAdditionalInheritedMembers()=%d\n",totalCount);
  return totalCount;
}

void ClassDefImpl::writeAdditionalInheritedMembers(OutputList &ol) const
{
  //printf("**** writeAdditionalInheritedMembers()\n");
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd && lmd->type!=MemberListType::Friends())
      {
        ClassDefSet visited;
        writeInheritedMemberDeclarations(ol,visited,lmd->type,MemberListType::Invalid(),lmd->title(getLanguage()),this,TRUE,FALSE);
      }
    }
  }
}

int ClassDefImpl::countMembersIncludingGrouped(MemberListType lt,
              const ClassDef *inheritedFrom,bool additional) const
{
  int count=0;
  MemberList *ml = getMemberList(lt);
  if (ml)
  {
    count=ml->countInheritableMembers(inheritedFrom);
  }
  //printf("%s:countMembersIncludingGrouped: count=%d\n",qPrint(name()),count);
  for (const auto &mg : m_impl->memberGroups)
  {
    bool hasOwnSection = !mg->allMembersInSameSection() ||
                         !m_impl->subGrouping; // group is in its own section
    if ((additional && hasOwnSection) || (!additional && !hasOwnSection))
    {
      count+=mg->countGroupedInheritedMembers(lt);
    }
  }
  //printf("%s:countMembersIncludingGrouped(lt=%s,%s)=%d\n",
  //    qPrint(name()),qPrint(lt.to_string()),ml?qPrint(ml->listType().toLabel()):"<none>",count);
  return count;
}


void ClassDefImpl::writeInheritedMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
               MemberListType lt,MemberListType lt2,const QCString &title,
               const ClassDef *inheritedFrom,bool invert,bool showAlways) const
{
  int count = countMembersIncludingGrouped(lt,inheritedFrom,FALSE);
  bool process = count>0;
  //printf("%s: writeInheritedMemberDec: lt=%s process=%d invert=%d always=%d\n",
  //    qPrint(name()),qPrint(lt.to_string()),process,invert,showAlways);
  if ((process^invert) || showAlways)
  {
    for (const auto &ibcd : m_impl->inherits)
    {
      ClassDefMutable *icd=toClassDefMutable(ibcd.classDef);
      if (icd && icd->isLinkable())
      {
        MemberListType lt1 = MemberListType::Invalid();
        MemberListType lt3 = MemberListType::Invalid();
        convertProtectionLevel(lt,ibcd.prot,&lt1,&lt3);
        if (lt2.isInvalid() && !lt3.isInvalid())
        {
          lt2=lt3;
        }
        //printf("%s:convert %s->(%s,%s) prot=%d\n",qPrint(icd->name()),qPrint(lt.to_string()),
        //                                          qPrint(lt1.to_string()),qPrint(lt2.to_string()),ibcd.prot);
        if (visitedClasses.find(icd)==visitedClasses.end())
        {
          visitedClasses.insert(icd); // guard for multiple virtual inheritance
          if (!lt1.isInvalid())
          {
            //printf("--> writeMemberDeclarations for type %s\n",qPrint(lt1.to_string()));
            icd->writeMemberDeclarations(ol,visitedClasses,lt1,
                title,QCString(),FALSE,inheritedFrom,lt2,FALSE,TRUE);
          }
        }
        else
        {
          //printf("%s: class already visited!\n",qPrint(icd->name()));
        }
      }
    }
  }
}

void ClassDefImpl::writeMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
               MemberListType lt,const QCString &title,
               const QCString &subTitle,bool showInline,const ClassDef *inheritedFrom,MemberListType lt2,
               bool invert,bool showAlways) const
{
  //printf("%s: ClassDefImpl::writeMemberDeclarations lt=%s lt2=%s\n",qPrint(name()),qPrint(lt.to_string()),qPrint(lt2.to_string()));
  MemberList * ml = getMemberList(lt);
  MemberList * ml2 = getMemberList(lt2);
  if (getLanguage()==SrcLangExt::VHDL) // use specific declarations function
  {
    static const ClassDef *cdef;
    if (cdef!=this)
    { // only one inline link
      VhdlDocGen::writeInlineClassLink(this,ol);
      cdef=this;
    }
    if (ml)
    {
      VhdlDocGen::writeVhdlDeclarations(ml,ol,nullptr,this,nullptr,nullptr,nullptr);
    }
  }
  else
  {
    //printf("%s::writeMemberDeclarations(%s) ml=%p ml2=%p\n",qPrint(name()),qPrint(title),ml,ml2);
    QCString tt = title, st = subTitle;
    if (ml)
    {
      //printf("  writeDeclarations type=%s count=%d\n",qPrint(lt.to_string()),ml->numDecMembers());
      ml->writeDeclarations(ol,this,nullptr,nullptr,nullptr,nullptr,tt,st,FALSE,showInline,inheritedFrom,lt,true);
      tt.clear();
      st.clear();
    }
    if (ml2)
    {
      //printf("  writeDeclarations type=%s count=%d\n",qPrint(lt2.to_string()),ml2->numDecMembers());
      ml2->writeDeclarations(ol,this,nullptr,nullptr,nullptr,nullptr,tt,st,FALSE,showInline,inheritedFrom,lt,ml==nullptr);
    }
    bool inlineInheritedMembers = Config_getBool(INLINE_INHERITED_MEMB);
    if (!inlineInheritedMembers) // show inherited members as separate lists
    {
      writeInheritedMemberDeclarations(ol,visitedClasses,lt,lt2,title,
          inheritedFrom ? inheritedFrom : this,
          invert,showAlways);
    }
  }
}

void ClassDefImpl::addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                        const ClassDef *inheritedFrom,const QCString &inheritId) const
{
  //printf("** %s::addGroupedInheritedMembers() inheritId=%s\n",qPrint(name()),qPrint(inheritId));
  for (const auto &mg : m_impl->memberGroups)
  {
    if (!mg->allMembersInSameSection() || !m_impl->subGrouping) // group is in its own section
    {
      mg->addGroupedInheritedMembers(ol,this,lt,inheritedFrom,inheritId);
    }
  }
}

void ClassDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title,bool showInline) const
{
  //printf("%s: ClassDefImpl::writeMemberDocumentation()\n",qPrint(name()));
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,displayName(),this,title,FALSE,showInline);
}

void ClassDefImpl::writeSimpleMemberDocumentation(OutputList &ol,MemberListType lt) const
{
  //printf("%s: ClassDefImpl::writeSimpleMemberDocumentation()\n",qPrint(name()));
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeSimpleDocumentation(ol,this);
}

void ClassDefImpl::writePlainMemberDeclaration(OutputList &ol,
         MemberListType lt,bool inGroup,
         int indentLevel,const ClassDef *inheritedFrom,const QCString &inheritId) const
{
  //printf("%s: ClassDefImpl::writePlainMemberDeclaration()\n",qPrint(name()));
  MemberList * ml = getMemberList(lt);
  if (ml)
  {
    ml->writePlainDeclarations(ol,inGroup,this,nullptr,nullptr,nullptr,nullptr,indentLevel,inheritedFrom,inheritId);
  }
}

bool ClassDefImpl::isLocal() const
{
  return m_impl->isLocal;
}

ClassLinkedRefMap ClassDefImpl::getClasses() const
{
  return m_impl->innerClasses;
}

ClassDefImpl::CompoundType ClassDefImpl::compoundType() const
{
  return m_impl->compType;
}

const BaseClassList &ClassDefImpl::baseClasses() const
{
  return m_impl->inherits;
}

void ClassDefImpl::updateBaseClasses(const BaseClassList &bcd)
{
  m_impl->inherits = bcd;
}

const BaseClassList &ClassDefImpl::subClasses() const
{
  return m_impl->inheritedBy;
}

void ClassDefImpl::updateSubClasses(const BaseClassList &bcd)
{
  m_impl->inheritedBy = bcd;
}

const MemberNameInfoLinkedMap &ClassDefImpl::memberNameInfoLinkedMap() const
{
  return m_impl->allMemberNameInfoLinkedMap;
}

void ClassDefImpl::sortAllMembersList()
{
  std::stable_sort(m_impl->allMemberNameInfoLinkedMap.begin(),
            m_impl->allMemberNameInfoLinkedMap.end(),
            [](const auto &m1,const auto &m2)
            {
              return qstricmp_sort(m1->memberName(),m2->memberName())<0;
            });
}

Protection ClassDefImpl::protection() const
{
  return m_impl->prot;
}

const ArgumentList &ClassDefImpl::templateArguments() const
{
  return m_impl->tempArgs;
}

FileDef *ClassDefImpl::getFileDef() const
{
  return m_impl->fileDef;
}

ModuleDef *ClassDefImpl::getModuleDef() const
{
  return m_impl->moduleDef;
}

const TemplateInstanceList &ClassDefImpl::getTemplateInstances() const
{
  return m_impl->templateInstances;
}

const ClassDef *ClassDefImpl::templateMaster() const
{
  return m_impl->templateMaster;
}

bool ClassDefImpl::isImplicitTemplateInstance() const
{
  return m_impl->implicitTemplateInstance;
}

void ClassDefImpl::setImplicitTemplateInstance(bool b)
{
  m_impl->implicitTemplateInstance = b;
}

bool ClassDefImpl::isTemplate() const
{
  return !m_impl->tempArgs.empty();
}

const IncludeInfo *ClassDefImpl::includeInfo() const
{
  return m_impl->incInfo.get();
}

const UsesClassList &ClassDefImpl::usedImplementationClasses() const
{
  return m_impl->usesImplClassList;
}

const UsesClassList &ClassDefImpl::usedByImplementationClasses() const
{
  return m_impl->usedByImplClassList;
}

const ConstraintClassList &ClassDefImpl::templateTypeConstraints() const
{
  return m_impl->constraintClassList;
}

bool ClassDefImpl::isTemplateArgument() const
{
  return m_impl->isTemplArg;
}

bool ClassDefImpl::isAbstract() const
{
  return m_impl->isAbstract || m_impl->spec.isAbstract();
}

bool ClassDefImpl::isFinal() const
{
  return m_impl->spec.isFinal();
}

bool ClassDefImpl::isSealed() const
{
  return m_impl->spec.isSealed();
}

bool ClassDefImpl::isPublished() const
{
  return m_impl->spec.isPublished();
}

bool ClassDefImpl::isForwardDeclared() const
{
  return m_impl->spec.isForwardDecl();
}

bool ClassDefImpl::isInterface() const
{
  return m_impl->spec.isInterface();
}

bool ClassDefImpl::isObjectiveC() const
{
  return getLanguage()==SrcLangExt::ObjC;
}

bool ClassDefImpl::isFortran() const
{
  return getLanguage()==SrcLangExt::Fortran;
}

bool ClassDefImpl::isCSharp() const
{
  return getLanguage()==SrcLangExt::CSharp;
}

ClassDef *ClassDefImpl::categoryOf() const
{
  return m_impl->categoryOf;
}

const MemberLists &ClassDefImpl::getMemberLists() const
{
  return m_impl->memberLists;
}

const MemberGroupList &ClassDefImpl::getMemberGroups() const
{
  return m_impl->memberGroups;
}

void ClassDefImpl::setFileDef(FileDef *fd)
{
  m_impl->fileDef = fd;
}

void ClassDefImpl::setModuleDef(ModuleDef *mod)
{
  m_impl->moduleDef = mod;
}

void ClassDefImpl::setSubGrouping(bool enabled)
{
  m_impl->subGrouping = enabled;
}

void ClassDefImpl::setProtection(Protection p)
{
  m_impl->prot=p;
  if (getLanguage()==SrcLangExt::VHDL && VhdlDocGen::convert(p)==VhdlDocGen::ARCHITECTURECLASS)
  {
    m_impl->className = name();
  }
}

void ClassDefImpl::setIsStatic(bool b)
{
  m_impl->isStatic=b;
}

void ClassDefImpl::setCompoundType(CompoundType t)
{
  m_impl->compType = t;
}

void ClassDefImpl::setTemplateMaster(const ClassDef *tm)
{
  assert(tm!=this);
  m_impl->templateMaster=tm;
}

void ClassDefImpl::makeTemplateArgument(bool b)
{
  m_impl->isTemplArg = b;
}

void ClassDefImpl::setCategoryOf(ClassDef *cd)
{
  m_impl->categoryOf = cd;
}

void ClassDefImpl::setUsedOnly(bool b)
{
  m_impl->usedOnly = b;
}

bool ClassDefImpl::isUsedOnly() const
{
  return m_impl->usedOnly;
}

bool ClassDefImpl::isSimple() const
{
  return m_impl->isSimple;
}

const MemberDef *ClassDefImpl::isSmartPointer() const
{
  return m_impl->arrowOperator;
}

void ClassDefImpl::reclassifyMember(MemberDefMutable *md,MemberType t)
{
  md->setMemberType(t);
  for (auto &ml : m_impl->memberLists)
  {
    ml->remove(md);
  }
  insertMember(md);
}

QCString ClassDefImpl::anchor() const
{
  QCString anc;
  if (isEmbeddedInOuterScope() && !Doxygen::generatingXmlOutput)
  {
    if (m_impl->templateMaster)
    {
      // point to the template of which this class is an instance
      anc = m_impl->templateMaster->getOutputFileBase();
    }
    else
    {
      anc = m_impl->fileName;
    }
  }
  return anc;
}

bool ClassDefImpl::isEmbeddedInOuterScope() const
{
  bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);

  Definition *container = getOuterScope();

  bool containerLinkable =
    container &&
       (
        (container==Doxygen::globalScope && getFileDef() && getFileDef()->isLinkableInProject()) || // global class in documented file
        container->isLinkableInProject() // class in documented scope
       );

  // inline because of INLINE_GROUPED_CLASSES=YES ?
  bool b1 = (inlineGroupedClasses && !partOfGroups().empty()); // a grouped class
  // inline because of INLINE_SIMPLE_STRUCTS=YES ?
  bool b2 = (inlineSimpleClasses && m_impl->isSimple && // a simple class
             (containerLinkable || // in a documented container
              !partOfGroups().empty()    // or part of a group
             )
           );
  //printf("%s::isEmbeddedInOuterScope(): inlineGroupedClasses=%d "
  //       "inlineSimpleClasses=%d partOfGroups()=%p m_impl->isSimple=%d "
  //       "getOuterScope()=%s b1=%d b2=%d\n",
  //    qPrint(name()),inlineGroupedClasses,inlineSimpleClasses,
  //    partOfGroups().pointer(),m_impl->isSimple,getOuterScope()?qPrint(getOuterScope()->name()):"<none>",b1,b2);
  return b1 || b2;  // either reason will do
}

const ClassDef *ClassDefImpl::tagLessReference() const
{
  return m_impl->tagLessRef;
}

void ClassDefImpl::setTagLessReference(const ClassDef *cd)
{
  m_impl->tagLessRef = cd;
}

void ClassDefImpl::removeMemberFromLists(MemberDef *md)
{
  for (auto &ml : m_impl->memberLists)
  {
    ml->remove(md);
  }
}

bool ClassDefImpl::isJavaEnum() const
{
  return m_impl->isJavaEnum;
}

void ClassDefImpl::setClassSpecifier(TypeSpecifier spec)
{
  m_impl->spec = spec;
}

void ClassDefImpl::addQualifiers(const StringVector &qualifiers)
{
  for (const auto &sx : qualifiers)
  {
    bool alreadyAdded = std::find(m_impl->qualifiers.begin(), m_impl->qualifiers.end(), sx) != m_impl->qualifiers.end();
    if (!alreadyAdded)
    {
      m_impl->qualifiers.push_back(sx);
    }
  }
}

StringVector ClassDefImpl::getQualifiers() const
{
  return m_impl->qualifiers;
}

bool ClassDefImpl::containsOverload(const MemberDef *md) const
{
  AUTO_TRACE("name={}",md->name());
  const auto &mni = m_impl->allMemberNameInfoLinkedMap.find(md->name());
  if (mni)
  {
    for (const auto &mi : *mni)
    {
      const MemberDef *classMd = mi->memberDef();
      const ArgumentList &classAl = classMd->argumentList();
      const ArgumentList &al      = md->argumentList();
      bool found = matchArguments2(
          classMd->getOuterScope(),classMd->getFileDef(),&classAl,
          md->getOuterScope(),md->getFileDef(),&al,
          true,getLanguage()
          );
      if (found)
      {
        AUTO_TRACE_EXIT("true");
        return true;
      }
    }
  }
  AUTO_TRACE_EXIT("false");
  return false;
}

bool ClassDefImpl::isExtension() const
{
  QCString n = name();
  int si = n.find('(');
  int ei = n.find(')');
  bool b = ei>si && n.mid(si+1,ei-si-1).stripWhiteSpace().isEmpty();
  return b;
}

const FileList &ClassDefImpl::usedFiles() const
{
  return m_impl->files;
}

const ArgumentList &ClassDefImpl::typeConstraints() const
{
  return m_impl->typeConstraints;
}

const ExampleList &ClassDefImpl::getExamples() const
{
  return m_impl->examples;
}

bool ClassDefImpl::subGrouping() const
{
  return m_impl->subGrouping;
}

bool ClassDefImpl::isSliceLocal() const
{
  return m_impl->spec.isLocal();
}

void ClassDefImpl::setMetaData(const QCString &md)
{
  m_impl->metaData = md;
}

QCString ClassDefImpl::collaborationGraphFileName() const
{
  return m_impl->collabFileName;
}

QCString ClassDefImpl::inheritanceGraphFileName() const
{
  return m_impl->inheritFileName;
}

void ClassDefImpl::overrideInheritanceGraph(CLASS_GRAPH_t e)
{
  m_impl->typeInheritanceGraph=e;
}

CLASS_GRAPH_t ClassDefImpl::hasInheritanceGraph() const
{
  return m_impl->typeInheritanceGraph;
}

CodeSymbolType ClassDefImpl::codeSymbolType() const
{
  switch (compoundType())
  {
    case Class:     return CodeSymbolType::Class;     break;
    case Struct:    return CodeSymbolType::Struct;    break;
    case Union:     return CodeSymbolType::Union;     break;
    case Interface: return CodeSymbolType::Interface; break;
    case Protocol:  return CodeSymbolType::Protocol;  break;
    case Category:  return CodeSymbolType::Category;  break;
    case Exception: return CodeSymbolType::Exception; break;
    case Service:   return CodeSymbolType::Service;   break;
    case Singleton: return CodeSymbolType::Singleton; break;
  }
  return CodeSymbolType::Class;
}

void ClassDefImpl::overrideCollaborationGraph(bool e)
{
  m_impl->hasCollaborationGraph=e;
}

bool ClassDefImpl::hasCollaborationGraph() const
{
  return m_impl->hasCollaborationGraph;
}


// --- Cast functions
//
ClassDef *toClassDef(Definition *d)
{
  if (d && (typeid(*d)==typeid(ClassDefImpl) || typeid(*d)==typeid(ClassDefAliasImpl)))
  {
    return static_cast<ClassDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

ClassDef *toClassDef(DefinitionMutable *md)
{
  Definition *d = toDefinition(md);
  if (d && typeid(*d)==typeid(ClassDefImpl))
  {
    return static_cast<ClassDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

const ClassDef *toClassDef(const Definition *d)
{
  if (d && (typeid(*d)==typeid(ClassDefImpl) || typeid(*d)==typeid(ClassDefAliasImpl)))
  {
    return static_cast<const ClassDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

ClassDefMutable *toClassDefMutable(Definition *d)
{
  if (d && typeid(*d)==typeid(ClassDefImpl))
  {
    return static_cast<ClassDefMutable*>(d);
  }
  else
  {
    return nullptr;
  }
}

// --- Helpers

/*! Get a class definition given its name.
 *  Returns nullptr if the class is not found.
 */
ClassDef *getClass(const QCString &n)
{
  if (n.isEmpty()) return nullptr;
  return Doxygen::classLinkedMap->find(n);
}

bool classHasVisibleRoot(const BaseClassList &bcl)
{
  for (const auto &bcd : bcl)
  {
    const ClassDef *cd=bcd.classDef;
    if (cd->isVisibleInHierarchy()) return true;
    if (classHasVisibleRoot(cd->baseClasses())) return true;
  }
  return false;
}

bool classHasVisibleChildren(const ClassDef *cd)
{
  BaseClassList bcl;

  if (cd->getLanguage()==SrcLangExt::VHDL) // reverse baseClass/subClass relation
  {
    if (cd->baseClasses().empty()) return FALSE;
    bcl=cd->baseClasses();
  }
  else
  {
    if (cd->subClasses().empty()) return FALSE;
    bcl=cd->subClasses();
  }

  for (const auto &bcd : bcl)
  {
    if (bcd.classDef->isVisibleInHierarchy())
    {
      return TRUE;
    }
  }
  return FALSE;
}

bool classVisibleInIndex(const ClassDef *cd)
{
  bool allExternals = Config_getBool(ALLEXTERNALS);
  return (allExternals && cd->isLinkable()) || cd->isLinkableInProject();
}

//----------------------------------------------------------------------
// recursive function that returns the number of branches in the
// inheritance tree that the base class 'bcd' is below the class 'cd'

int minClassDistance(const ClassDef *cd,const ClassDef *bcd,int level)
{
  const int maxInheritanceDepth = 100000;
  if (bcd->categoryOf()) // use class that is being extended in case of
    // an Objective-C category
  {
    bcd=bcd->categoryOf();
  }
  if (cd==bcd) return level;
  if (level==256)
  {
    warn_uncond("class %s seem to have a recursive "
        "inheritance relation!\n",qPrint(cd->name()));
    return -1;
  }
  int m=maxInheritanceDepth;
  for (const auto &bcdi : cd->baseClasses())
  {
    int mc=minClassDistance(bcdi.classDef,bcd,level+1);
    if (mc<m) m=mc;
    if (m<0) break;
  }
  return m;
}

Protection classInheritedProtectionLevel(const ClassDef *cd,const ClassDef *bcd,Protection prot,int level)
{
  if (bcd->categoryOf()) // use class that is being extended in case of
    // an Objective-C category
  {
    bcd=bcd->categoryOf();
  }
  if (cd==bcd)
  {
    goto exit;
  }
  if (level==256)
  {
    err("Internal inconsistency: found class %s seem to have a recursive "
        "inheritance relation! Please send a bug report to doxygen@gmail.com\n",qPrint(cd->name()));
  }
  else if (prot!=Protection::Private)
  {
    for (const auto &bcdi : cd->baseClasses())
    {
      Protection baseProt = classInheritedProtectionLevel(bcdi.classDef,bcd,bcdi.prot,level+1);
      if (baseProt==Protection::Private)        prot=Protection::Private;
      else if (baseProt==Protection::Protected) prot=Protection::Protected;
    }
  }
exit:
  //printf("classInheritedProtectionLevel(%s,%s)=%d\n",qPrint(cd->name()),qPrint(bcd->name()),prot);
  return prot;
}


