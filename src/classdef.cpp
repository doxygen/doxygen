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

//-----------------------------------------------------------------------------

static QCString makeQualifiedNameWithTemplateParameters(const ClassDef *cd,
    const ArgumentLists *actualParams,uint32_t *actualParamIndex)
{
  //bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  bool hideScopeNames = Config_getBool(HIDE_SCOPE_NAMES);
  //printf("qualifiedNameWithTemplateParameters() localName=%s\n",qPrint(localName()));
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
  if (lang==SrcLangExt_VHDL)
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


/** Implementation of the ClassDef interface */
class ClassDefImpl : public DefinitionMixin<ClassDefMutable>
{
  public:
    ClassDefImpl(const QCString &fileName,int startLine,int startColumn,
             const QCString &name,CompoundType ct,
             const QCString &ref=QCString(),const QCString &fName=QCString(),
             bool isSymbol=TRUE,bool isJavaEnum=FALSE);
    /** Destroys a compound definition. */
   ~ClassDefImpl();

    virtual ClassDef *resolveAlias() { return this; }
    virtual DefType definitionType() const { return TypeClass; }
    virtual CodeSymbolType codeSymbolType() const;
    virtual QCString getOutputFileBase() const;
    virtual QCString getInstanceOutputFileBase() const;
    virtual QCString getSourceFileBase() const;
    virtual QCString getReference() const;
    virtual bool isReference() const;
    virtual bool isLocal() const;
    virtual ClassLinkedRefMap getClasses() const;
    virtual bool hasDocumentation() const;
    virtual bool hasDetailedDescription() const;
    virtual QCString collaborationGraphFileName() const;
    virtual QCString inheritanceGraphFileName() const;
    virtual QCString displayName(bool includeScope=TRUE) const;
    virtual CompoundType compoundType() const;
    virtual QCString compoundTypeString() const;
    virtual const BaseClassList &baseClasses() const;
    virtual void updateBaseClasses(const BaseClassList &bcd);
    virtual const BaseClassList &subClasses() const;
    virtual void updateSubClasses(const BaseClassList &bcd);
    virtual const MemberNameInfoLinkedMap &memberNameInfoLinkedMap() const;
    virtual Protection protection() const;
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual bool isVisibleInHierarchy() const;
    virtual bool visibleInParentsDeclList() const;
    virtual const ArgumentList &templateArguments() const;
    virtual FileDef *getFileDef() const;
    virtual const MemberDef *getMemberByName(const QCString &) const;
    virtual int isBaseClass(const ClassDef *bcd,bool followInstances,const QCString &templSpec) const;
    virtual bool isSubClass(ClassDef *bcd,int level=0) const;
    virtual bool isAccessibleMember(const MemberDef *md) const;
    virtual const TemplateInstanceList &getTemplateInstances() const;
    virtual const ClassDef *templateMaster() const;
    virtual bool isTemplate() const;
    virtual const IncludeInfo *includeInfo() const;
    virtual const UsesClassList &usedImplementationClasses() const;
    virtual const UsesClassList &usedByImplementationClasses() const;
    virtual const ConstraintClassList &templateTypeConstraints() const;
    virtual bool isTemplateArgument() const;
    virtual const Definition *findInnerCompound(const QCString &name) const;
    virtual ArgumentLists getTemplateParameterLists() const;
    virtual QCString qualifiedNameWithTemplateParameters(
        const ArgumentLists *actualParams=0,uint32_t *actualParamIndex=0) const;
    virtual bool isAbstract() const;
    virtual bool isObjectiveC() const;
    virtual bool isFortran() const;
    virtual bool isCSharp() const;
    virtual bool isFinal() const;
    virtual bool isSealed() const;
    virtual bool isPublished() const;
    virtual bool isExtension() const;
    virtual bool isForwardDeclared() const;
    virtual bool isInterface() const;
    virtual ClassDef *categoryOf() const;
    virtual QCString className() const;
    virtual MemberList *getMemberList(MemberListType lt) const;
    virtual const MemberLists &getMemberLists() const;
    virtual const MemberGroupList &getMemberGroups() const;
    virtual const TemplateNameMap &getTemplateBaseClassNames() const;
    virtual bool isUsedOnly() const;
    virtual QCString anchor() const;
    virtual bool isEmbeddedInOuterScope() const;
    virtual bool isSimple() const;
    virtual const ClassDef *tagLessReference() const;
    virtual const MemberDef *isSmartPointer() const;
    virtual bool isJavaEnum() const;
    virtual QCString title() const;
    virtual QCString generatedFromFiles() const;
    virtual const FileList &usedFiles() const;
    virtual const ArgumentList &typeConstraints() const;
    virtual const ExampleList &getExamples() const;
    virtual bool hasExamples() const;
    virtual QCString getMemberListFileName() const;
    virtual bool subGrouping() const;
    virtual bool isSliceLocal() const;
    virtual bool hasNonReferenceSuperClass() const;
    virtual QCString requiresClause() const;
    virtual StringVector getQualifiers() const;
    virtual ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                const QCString &templSpec,bool &freshInstance) const;

    virtual void insertBaseClass(ClassDef *,const QCString &name,Protection p,Specifier s,const QCString &t=QCString());
    virtual void insertSubClass(ClassDef *,Protection p,Specifier s,const QCString &t=QCString());
    virtual void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force);
    virtual void insertMember(MemberDef *);
    virtual void insertUsedFile(const FileDef *);
    virtual bool addExample(const QCString &anchor,const QCString &name, const QCString &file);
    virtual void mergeCategory(ClassDef *category);
    virtual void setFileDef(FileDef *fd);
    virtual void setSubGrouping(bool enabled);
    virtual void setProtection(Protection p);
    virtual void setGroupDefForAllMembers(GroupDef *g,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs);
    virtual void addInnerCompound(Definition *d);
    virtual void addUsedClass(ClassDef *cd,const QCString &accessName,Protection prot);
    virtual void addUsedByClass(ClassDef *cd,const QCString &accessName,Protection prot);
    virtual void setIsStatic(bool b);
    virtual void setCompoundType(CompoundType t);
    virtual void setClassName(const QCString &name);
    virtual void setClassSpecifier(uint64_t spec);
    virtual void addQualifiers(const StringVector &qualifiers);
    virtual void setTemplateArguments(const ArgumentList &al);
    virtual void setTemplateBaseClassNames(const TemplateNameMap &templateNames);
    virtual void setTemplateMaster(const ClassDef *tm);
    virtual void setTypeConstraints(const ArgumentList &al);
    virtual void addMembersToTemplateInstance(const ClassDef *cd,const ArgumentList &templateArguments,const QCString &templSpec);
    virtual void makeTemplateArgument(bool b=TRUE);
    virtual void setCategoryOf(ClassDef *cd);
    virtual void setUsedOnly(bool b);
    virtual void setTagLessReference(const ClassDef *cd);
    virtual void setName(const QCString &name);
    virtual void setMetaData(const QCString &md);
    virtual void findSectionsInDocumentation();
    virtual void addMembersToMemberGroup();
    virtual void addListReferences();
    virtual void addTypeConstraints();
    virtual void computeAnchors();
    virtual void mergeMembers();
    virtual void sortMemberLists();
    virtual void distributeMemberGroupDocumentation();
    virtual void writeDocumentation(OutputList &ol) const;
    virtual void writeDocumentationForInnerClasses(OutputList &ol) const;
    virtual void writeMemberPages(OutputList &ol) const;
    virtual void writeMemberList(OutputList &ol) const;
    virtual void writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,int indentLevel,
                          const ClassDef *inheritedFrom,const QCString &inheritId) const;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const;
    virtual void writeSummaryLinks(OutputList &ol) const;
    virtual void reclassifyMember(MemberDefMutable *md,MemberType t);
    virtual void writeInlineDocumentation(OutputList &ol) const;
    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const;
    virtual void removeMemberFromLists(MemberDef *md);
    virtual void setAnonymousEnumType();
    virtual void countMembers();
    virtual void sortAllMembersList();

    virtual void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                              const ClassDef *inheritedFrom,const QCString &inheritId) const;
    virtual void writeTagFile(TextStream &) const;

    virtual int countMembersIncludingGrouped(MemberListType lt,const ClassDef *inheritedFrom,bool additional) const;
    virtual int countInheritanceNodes() const;
    virtual int countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                int lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const;
    virtual void writeMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                 MemberListType lt,const QCString &title,
                 const QCString &subTitle=QCString(),
                 bool showInline=FALSE,const ClassDef *inheritedFrom=0,
                 int lt2=-1,bool invert=FALSE,bool showAlways=FALSE) const;
    virtual void setRequiresClause(const QCString &req);

  private:
    void addUsedInterfaceClasses(MemberDef *md,const QCString &typeStr);
    void showUsedFiles(OutputList &ol) const;

    void writeDocumentationContents(OutputList &ol,const QCString &pageTitle) const;
    void internalInsertMember(MemberDef *md,Protection prot,bool addToAllList);
    void addMemberToList(MemberListType lt,MemberDef *md,bool isBrief);
    void writeInheritedMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                                          MemberListType lt,int lt2,const QCString &title,
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
    QCString includeStatement() const;
    void addTypeConstraint(const QCString &typeConstraint,const QCString &type);
    void writeTemplateSpec(OutputList &ol,const Definition *d,
            const QCString &type,SrcLangExt lang) const;

    // PIMPL idiom
    class IMPL;
    IMPL *m_impl = 0;
};

ClassDefMutable *createClassDef(
             const QCString &fileName,int startLine,int startColumn,
             const QCString &name,ClassDef::CompoundType ct,
             const QCString &ref,const QCString &fName,
             bool isSymbol,bool isJavaEnum)
{
  return new ClassDefImpl(fileName,startLine,startColumn,name,ct,ref,fName,isSymbol,isJavaEnum);
}
//-----------------------------------------------------------------------------

class ClassDefAliasImpl : public DefinitionAliasMixin<ClassDef>
{
  public:
    ClassDefAliasImpl(const Definition *newScope,const ClassDef *cd)
      : DefinitionAliasMixin(newScope,cd) { init(); }
    virtual ~ClassDefAliasImpl() { deinit(); }
    virtual DefType definitionType() const { return TypeClass; }

    const ClassDef *getCdAlias() const { return toClassDef(getAlias()); }
    virtual ClassDef *resolveAlias() { return const_cast<ClassDef*>(getCdAlias()); }

    virtual CodeSymbolType codeSymbolType() const
    { return getCdAlias()->codeSymbolType(); }
    virtual QCString getOutputFileBase() const
    { return getCdAlias()->getOutputFileBase(); }
    virtual QCString getInstanceOutputFileBase() const
    { return getCdAlias()->getInstanceOutputFileBase(); }
    virtual QCString getSourceFileBase() const
    { return getCdAlias()->getSourceFileBase(); }
    virtual QCString getReference() const
    { return getCdAlias()->getReference(); }
    virtual bool isReference() const
    { return getCdAlias()->isReference(); }
    virtual bool isLocal() const
    { return getCdAlias()->isLocal(); }
    virtual ClassLinkedRefMap getClasses() const
    { return getCdAlias()->getClasses(); }
    virtual bool hasDocumentation() const
    { return getCdAlias()->hasDocumentation(); }
    virtual bool hasDetailedDescription() const
    { return getCdAlias()->hasDetailedDescription(); }
    virtual QCString collaborationGraphFileName() const
    { return getCdAlias()->collaborationGraphFileName(); }
    virtual QCString inheritanceGraphFileName() const
    { return getCdAlias()->inheritanceGraphFileName(); }
    virtual QCString displayName(bool includeScope=TRUE) const
    { return makeDisplayName(this,includeScope); }
    virtual CompoundType compoundType() const
    { return getCdAlias()->compoundType(); }
    virtual QCString compoundTypeString() const
    { return getCdAlias()->compoundTypeString(); }
    virtual const BaseClassList &baseClasses() const
    { return getCdAlias()->baseClasses(); }
    virtual const BaseClassList &subClasses() const
    { return getCdAlias()->subClasses(); }
    virtual const MemberNameInfoLinkedMap &memberNameInfoLinkedMap() const
    { return getCdAlias()->memberNameInfoLinkedMap(); }
    virtual Protection protection() const
    { return getCdAlias()->protection(); }
    virtual bool isLinkableInProject() const
    { return getCdAlias()->isLinkableInProject(); }
    virtual bool isLinkable() const
    { return getCdAlias()->isLinkable(); }
    virtual bool isVisibleInHierarchy() const
    { return getCdAlias()->isVisibleInHierarchy(); }
    virtual bool visibleInParentsDeclList() const
    { return getCdAlias()->visibleInParentsDeclList(); }
    virtual const ArgumentList &templateArguments() const
    { return getCdAlias()->templateArguments(); }
    //virtual NamespaceDef *getNamespaceDef() const
    //{ return getCdAlias()->getNamespaceDef(); }
    virtual FileDef *getFileDef() const
    { return getCdAlias()->getFileDef(); }
    virtual const MemberDef *getMemberByName(const QCString &s) const
    { return getCdAlias()->getMemberByName(s); }
    virtual int isBaseClass(const ClassDef *bcd,bool followInstances,const QCString &templSpec) const
    { return getCdAlias()->isBaseClass(bcd,followInstances,templSpec); }
    virtual bool isSubClass(ClassDef *bcd,int level=0) const
    { return getCdAlias()->isSubClass(bcd,level); }
    virtual bool isAccessibleMember(const MemberDef *md) const
    { return getCdAlias()->isAccessibleMember(md); }
    virtual const TemplateInstanceList &getTemplateInstances() const
    { return getCdAlias()->getTemplateInstances(); }
    virtual const ClassDef *templateMaster() const
    { return getCdAlias()->templateMaster(); }
    virtual bool isTemplate() const
    { return getCdAlias()->isTemplate(); }
    virtual const IncludeInfo *includeInfo() const
    { return getCdAlias()->includeInfo(); }
    virtual const UsesClassList &usedImplementationClasses() const
    { return getCdAlias()->usedImplementationClasses(); }
    virtual const UsesClassList &usedByImplementationClasses() const
    { return getCdAlias()->usedByImplementationClasses(); }
    virtual const ConstraintClassList &templateTypeConstraints() const
    { return getCdAlias()->templateTypeConstraints(); }
    virtual bool isTemplateArgument() const
    { return getCdAlias()->isTemplateArgument(); }
    virtual const Definition *findInnerCompound(const QCString &name) const
    { return getCdAlias()->findInnerCompound(name); }
    virtual ArgumentLists getTemplateParameterLists() const
    { return getCdAlias()->getTemplateParameterLists(); }
    virtual QCString qualifiedNameWithTemplateParameters(
        const ArgumentLists *actualParams=0,uint32_t *actualParamIndex=0) const
    { return makeQualifiedNameWithTemplateParameters(this,actualParams,actualParamIndex); }
    virtual bool isAbstract() const
    { return getCdAlias()->isAbstract(); }
    virtual bool isObjectiveC() const
    { return getCdAlias()->isObjectiveC(); }
    virtual bool isFortran() const
    { return getCdAlias()->isFortran(); }
    virtual bool isCSharp() const
    { return getCdAlias()->isCSharp(); }
    virtual bool isFinal() const
    { return getCdAlias()->isFinal(); }
    virtual bool isSealed() const
    { return getCdAlias()->isSealed(); }
    virtual bool isPublished() const
    { return getCdAlias()->isPublished(); }
    virtual bool isExtension() const
    { return getCdAlias()->isExtension(); }
    virtual bool isForwardDeclared() const
    { return getCdAlias()->isForwardDeclared(); }
    virtual bool isInterface() const
    { return getCdAlias()->isInterface(); }
    virtual ClassDef *categoryOf() const
    { return getCdAlias()->categoryOf(); }
    virtual QCString className() const
    { return getCdAlias()->className(); }
    virtual MemberList *getMemberList(MemberListType lt) const
    { return getCdAlias()->getMemberList(lt); }
    virtual const MemberLists &getMemberLists() const
    { return getCdAlias()->getMemberLists(); }
    virtual const MemberGroupList &getMemberGroups() const
    { return getCdAlias()->getMemberGroups(); }
    virtual const TemplateNameMap &getTemplateBaseClassNames() const
    { return getCdAlias()->getTemplateBaseClassNames(); }
    virtual bool isUsedOnly() const
    { return getCdAlias()->isUsedOnly(); }
    virtual QCString anchor() const
    { return getCdAlias()->anchor(); }
    virtual bool isEmbeddedInOuterScope() const
    { return getCdAlias()->isEmbeddedInOuterScope(); }
    virtual bool isSimple() const
    { return getCdAlias()->isSimple(); }
    virtual const ClassDef *tagLessReference() const
    { return getCdAlias()->tagLessReference(); }
    virtual const MemberDef *isSmartPointer() const
    { return getCdAlias()->isSmartPointer(); }
    virtual bool isJavaEnum() const
    { return getCdAlias()->isJavaEnum(); }
    virtual QCString title() const
    { return getCdAlias()->title(); }
    virtual QCString generatedFromFiles() const
    { return getCdAlias()->generatedFromFiles(); }
    virtual const FileList &usedFiles() const
    { return getCdAlias()->usedFiles(); }
    virtual const ArgumentList &typeConstraints() const
    { return getCdAlias()->typeConstraints(); }
    virtual const ExampleList &getExamples() const
    { return getCdAlias()->getExamples(); }
    virtual bool hasExamples() const
    { return getCdAlias()->hasExamples(); }
    virtual QCString getMemberListFileName() const
    { return getCdAlias()->getMemberListFileName(); }
    virtual bool subGrouping() const
    { return getCdAlias()->subGrouping(); }
    virtual bool isSliceLocal() const
    { return getCdAlias()->isSliceLocal(); }
    virtual bool hasNonReferenceSuperClass() const
    { return getCdAlias()->hasNonReferenceSuperClass(); }
    virtual QCString requiresClause() const
    { return getCdAlias()->requiresClause(); }
    virtual StringVector getQualifiers() const
    { return getCdAlias()->getQualifiers(); }

    virtual int countMembersIncludingGrouped(MemberListType lt,const ClassDef *inheritedFrom,bool additional) const
    { return getCdAlias()->countMembersIncludingGrouped(lt,inheritedFrom,additional); }
    virtual int countInheritanceNodes() const
    { return getCdAlias()->countInheritanceNodes(); }
    virtual int countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                int lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const
    { return getCdAlias()->countMemberDeclarations(lt,inheritedFrom,lt2,invert,showAlways,visitedClasses); }

    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const
    { getCdAlias()->writeDeclarationLink(ol,found,header,localNames); }
    virtual ClassDef *insertTemplateInstance(const QCString &fileName,int startLine,int startColumn,
                                             const QCString &templSpec,bool &freshInstance) const
    { return getCdAlias()->insertTemplateInstance(fileName,startLine,startColumn,templSpec,freshInstance); }

    virtual void writeDocumentation(OutputList &ol) const
    { getCdAlias()->writeDocumentation(ol); }
    virtual void writeDocumentationForInnerClasses(OutputList &ol) const
    { getCdAlias()->writeDocumentationForInnerClasses(ol); }
    virtual void writeMemberPages(OutputList &ol) const
    { getCdAlias()->writeMemberPages(ol); }
    virtual void writeMemberList(OutputList &ol) const
    { getCdAlias()->writeMemberList(ol); }
    virtual void writeDeclaration(OutputList &ol,const MemberDef *md,bool inGroup,
                 int indentLevel, const ClassDef *inheritedFrom,const QCString &inheritId) const
    { getCdAlias()->writeDeclaration(ol,md,inGroup,indentLevel,inheritedFrom,inheritId); }
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const
    { getCdAlias()->writeQuickMemberLinks(ol,md); }
    virtual void writeSummaryLinks(OutputList &ol) const
    { getCdAlias()->writeSummaryLinks(ol); }
    virtual void writeInlineDocumentation(OutputList &ol) const
    { getCdAlias()->writeInlineDocumentation(ol); }
    virtual void writeTagFile(TextStream &ol) const
    { getCdAlias()->writeTagFile(ol); }
    virtual void writeMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
                 MemberListType lt,const QCString &title,
                 const QCString &subTitle=QCString(),
                 bool showInline=FALSE,const ClassDef *inheritedFrom=0,
                 int lt2=-1,bool invert=FALSE,bool showAlways=FALSE) const
    { getCdAlias()->writeMemberDeclarations(ol,visitedClasses,lt,title,subTitle,showInline,inheritedFrom,lt2,invert,showAlways); }
    virtual void addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                 const ClassDef *inheritedFrom,const QCString &inheritId) const
    { getCdAlias()->addGroupedInheritedMembers(ol,lt,inheritedFrom,inheritId); }

    virtual void updateBaseClasses(const BaseClassList &) {}
    virtual void updateSubClasses(const BaseClassList &) {}
};

ClassDef *createClassDefAlias(const Definition *newScope,const ClassDef *cd)
{
  ClassDef *acd = new ClassDefAliasImpl(newScope,cd);
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
    IMPL();
   ~IMPL();
    void init(const QCString &defFileName, const QCString &name,
              const QCString &ctStr, const QCString &fName);

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
    //NamespaceDef  *nspace = 0;

    /*! File this class is defined in */
    FileDef *fileDef = 0;

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
    const ClassDef *templateMaster = 0;

    /*! local class name which could be a typedef'ed alias name. */
    QCString className;

    /*! If this class is a Objective-C category, then this points to the
     *  class which is extended.
     */
    ClassDef *categoryOf = 0;

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
    const MemberDef *arrowOperator = 0;

    const ClassDef *tagLessRef = 0;

    /** Does this class represent a Java style enum? */
    bool isJavaEnum = false;

    uint64_t spec = 0;

    QCString metaData;

    /** C++20 requires clause */
    QCString requiresClause;

    StringVector qualifiers;
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
  //nspace=0;
  fileDef=0;
  subGrouping=Config_getBool(SUBGROUPING);
  templateMaster =0;
  isAbstract = FALSE;
  isStatic = FALSE;
  isTemplArg = FALSE;
  membersMerged = FALSE;
  categoryOf = 0;
  usedOnly = FALSE;
  isSimple = Config_getBool(INLINE_SIMPLE_STRUCTS);
  arrowOperator = 0;
  tagLessRef = 0;
  spec=0;
  //QCString ns;
  //extractNamespaceName(name,className,ns);
  //printf("m_name=%s m_className=%s ns=%s\n",qPrint(m_name),qPrint(m_className),qPrint(ns));

  // we cannot use getLanguage at this point, as setLanguage has not been called.
  SrcLangExt lang = getLanguageFromFileName(defFileName);
  if ((lang==SrcLangExt_Cpp || lang==SrcLangExt_ObjC) &&
      guessSection(defFileName)==Entry::SOURCE_SEC)
  {
    isLocal=TRUE;
  }
  else
  {
    isLocal=FALSE;
  }
}

ClassDefImpl::IMPL::IMPL()
{
}

ClassDefImpl::IMPL::~IMPL()
{
}

//-------------------------------------------------------------------------------------------

// constructs a new class definition
ClassDefImpl::ClassDefImpl(
    const QCString &defFileName,int defLine,int defColumn,
    const QCString &nm,CompoundType ct,
    const QCString &lref,const QCString &fName,
    bool isSymbol,bool isJavaEnum)
 : DefinitionMixin(defFileName,defLine,defColumn,removeRedundantWhiteSpace(nm),0,0,isSymbol)
{
  setReference(lref);
  m_impl = new ClassDefImpl::IMPL;
  m_impl->compType = ct;
  m_impl->isJavaEnum = isJavaEnum;
  m_impl->init(defFileName,name(),compoundTypeString(),fName);
  m_impl->memberListFileName = convertNameToFile(compoundTypeString()+name()+"-members");
  m_impl->collabFileName = convertNameToFile(m_impl->fileName+"_coll_graph");
  m_impl->inheritFileName = convertNameToFile(m_impl->fileName+"_inherit_graph");
  if (lref.isEmpty())
  {
    m_impl->fileName = convertNameToFile(m_impl->fileName);
  }
}

// destroy the class definition
ClassDefImpl::~ClassDefImpl()
{
  delete m_impl;
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
  m_impl->inherits.push_back(BaseClassDef(cd,n,p,s,t));
  m_impl->isSimple = FALSE;
}

// inserts a derived/sub class in the inherited-by list
void ClassDefImpl::insertSubClass(ClassDef *cd,Protection p,
                                Specifier s,const QCString &t)
{
  //printf("*** insert sub class %s into %s\n",qPrint(cd->name()),qPrint(name()));
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  if (!extractPrivate && cd->protection()==Protection::Private) return;
  m_impl->inheritedBy.push_back(BaseClassDef(cd,QCString(),p,s,t));
  m_impl->isSimple = FALSE;
}

void ClassDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_impl->memberLists)
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
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
  //printf("insertInternalMember(%s) isHidden()=%d\n",qPrint(md->name()),md->isHidden());
  if (md->isHidden()) return;

  if (getLanguage()==SrcLangExt_VHDL)
  {
    QCString title=theTranslator->trVhdlType(md->getMemberSpecifiers(),FALSE);
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
      addMemberToList(MemberListType_related,md,TRUE);
    }
    else if (md->isFriend())
    {
      addMemberToList(MemberListType_friends,md,TRUE);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberType_Service: // UNO IDL
          addMemberToList(MemberListType_services,md,TRUE);
          break;
        case MemberType_Interface: // UNO IDL
          addMemberToList(MemberListType_interfaces,md,TRUE);
          break;
        case MemberType_Signal: // Qt specific
          addMemberToList(MemberListType_signals,md,TRUE);
          break;
        case MemberType_DCOP:   // KDE2 specific
          addMemberToList(MemberListType_dcopMethods,md,TRUE);
          break;
        case MemberType_Property:
          addMemberToList(MemberListType_properties,md,TRUE);
          break;
        case MemberType_Event:
          addMemberToList(MemberListType_events,md,TRUE);
          break;
        case MemberType_Slot:   // Qt specific
          switch (prot)
          {
            case Protection::Protected:
            case Protection::Package: // slots in packages are not possible!
              addMemberToList(MemberListType_proSlots,md,TRUE);
              break;
            case Protection::Public:
              addMemberToList(MemberListType_pubSlots,md,TRUE);
              break;
            case Protection::Private:
              addMemberToList(MemberListType_priSlots,md,TRUE);
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
                  addMemberToList(MemberListType_proStaticAttribs,md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType_pacStaticAttribs,md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType_pubStaticAttribs,md,TRUE);
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType_priStaticAttribs,md,TRUE);
                  break;
              }
            }
            else // function
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType_proStaticMethods,md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType_pacStaticMethods,md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType_pubStaticMethods,md,TRUE);
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType_priStaticMethods,md,TRUE);
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
                  addMemberToList(MemberListType_proAttribs,md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType_pacAttribs,md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType_pubAttribs,md,TRUE);
                  isSimple=!md->isFunctionPtr();
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType_priAttribs,md,TRUE);
                  break;
              }
            }
            else if (md->isTypedef() || md->isEnumerate() || md->isEnumValue())
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType_proTypes,md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType_pacTypes,md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType_pubTypes,md,TRUE);
                  isSimple=!md->isEnumerate() &&
                           !md->isEnumValue() &&
                           QCString(md->typeString()).find(")(")==-1; // func ptr typedef
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType_priTypes,md,TRUE);
                  break;
              }
            }
            else // member function
            {
              switch (prot)
              {
                case Protection::Protected:
                  addMemberToList(MemberListType_proMethods,md,TRUE);
                  break;
                case Protection::Package:
                  addMemberToList(MemberListType_pacMethods,md,TRUE);
                  break;
                case Protection::Public:
                  addMemberToList(MemberListType_pubMethods,md,TRUE);
                  break;
                case Protection::Private:
                  addMemberToList(MemberListType_priMethods,md,TRUE);
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
      addMemberToList(MemberListType_relatedMembers,md,FALSE);
    }
    else if (md->isFunction() &&
             md->protection()==Protection::Private &&
             (md->virtualness()!=Specifier::Normal || md->isOverride() || md->isFinal()) &&
             Config_getBool(EXTRACT_PRIV_VIRTUAL))
    {
      addMemberToList(MemberListType_functionMembers,md,FALSE);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberType_Service: // UNO IDL
          addMemberToList(MemberListType_serviceMembers,md,FALSE);
          break;
        case MemberType_Interface: // UNO IDL
          addMemberToList(MemberListType_interfaceMembers,md,FALSE);
          break;
        case MemberType_Property:
          addMemberToList(MemberListType_propertyMembers,md,FALSE);
          break;
        case MemberType_Event:
          addMemberToList(MemberListType_eventMembers,md,FALSE);
          break;
        case MemberType_Signal: // fall through
        case MemberType_DCOP:
          addMemberToList(MemberListType_functionMembers,md,FALSE);
          break;
        case MemberType_Slot:
          if (protectionLevelVisible(prot))
          {
            addMemberToList(MemberListType_functionMembers,md,FALSE);
          }
          break;
        default: // any of the other members
          if (protectionLevelVisible(prot))
          {
            switch (md->memberType())
            {
              case MemberType_Typedef:
                addMemberToList(MemberListType_typedefMembers,md,FALSE);
                break;
              case MemberType_Enumeration:
                addMemberToList(MemberListType_enumMembers,md,FALSE);
                break;
              case MemberType_EnumValue:
                addMemberToList(MemberListType_enumValMembers,md,FALSE);
                break;
              case MemberType_Function:
                if (md->isConstructor() || md->isDestructor())
                {
                  m_impl->memberLists.get(MemberListType_constructors,MemberListContainer::Class)->push_back(md);
                }
                else
                {
                  addMemberToList(MemberListType_functionMembers,md,FALSE);
                }
                break;
              case MemberType_Variable:
                addMemberToList(MemberListType_variableMembers,md,FALSE);
                break;
              case MemberType_Define:
                warn(md->getDefFileName(),md->getDefLine()-1,"A define (%s) cannot be made a member of %s",
                     qPrint(md->name()), qPrint(this->name()));
                break;
              default:
                err("Unexpected member type %d found!\n",md->memberType());
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
    mni->push_back(std::make_unique<MemberInfo>(md,prot,md->virtualness(),FALSE));
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
    if ((ml->listType()&MemberListType_detailedLists)==0)
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
  for (const auto &mg : m_impl->memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }
  for (auto &ml : m_impl->memberLists)
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}


// add a file name to the used files set
void ClassDefImpl::insertUsedFile(const FileDef *fd)
{
  if (fd==0) return;
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
    if      (bcd.prot==Protection::Protected) sl.push_back("protected");
    else if (bcd.prot==Protection::Private)   sl.push_back("private");
    if      (bcd.virt==Specifier::Virtual)    sl.push_back("virtual");
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
      (fd!=0 && m_impl->incInfo->fileDef==0)
     )
  {
    //printf("Setting file info\n");
    m_impl->incInfo->fileDef     = fd;
    m_impl->incInfo->includeName = includeName;
    m_impl->incInfo->local       = local;
  }
  if (force && !includeName.isEmpty())
  {
    m_impl->incInfo->includeName = includeName;
    m_impl->incInfo->local       = local;
  }
}

// TODO: fix this: a nested template class can have multiple outer templates
//ArgumentList *ClassDefImpl::outerTemplateArguments() const
//{
//  int ti;
//  ClassDef *pcd=0;
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
//  return 0;
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
    ol.generateDoc(briefFile(),briefLine(),this,0,
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

  if (getLanguage()==SrcLangExt_Cpp)
  {
    writeTemplateSpec(ol,this,compoundTypeString(),getLanguage());
  }

  // repeat brief description
  if (!briefDescription().isEmpty() && repeatBrief)
  {
    ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE,
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
    ol.generateDoc(docFile(),docLine(),this,0,documentation(),TRUE,FALSE,
                   QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
  }
  // write type constraints
  writeTypeConstraints(ol,this,m_impl->typeConstraints);

  // write examples
  if (hasExamples())
  {
    ol.startExamples();
    ol.startDescForItem();
    //ol.startParagraph();
    writeExamples(ol,m_impl->examples);
    //ol.endParagraph();
    ol.endDescForItem();
    ol.endExamples();
  }
  //ol.newParagraph();
  writeSourceDef(ol,name());
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
  if (lang==SrcLangExt_Fortran)
  {
    result = theTranslator->trGeneratedFromFilesFortran(
          getLanguage()==SrcLangExt_ObjC && m_impl->compType==Interface ? Class : m_impl->compType,
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
          getLanguage()==SrcLangExt_ObjC && m_impl->compType==Interface ? Class : m_impl->compType,
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
      ol.docify(fname);
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

int ClassDefImpl::countInheritanceNodes() const
{
  int count=0;
  for (const auto &ibcd : m_impl->inheritedBy)
  {
    const ClassDef *icd=ibcd.classDef;
    if ( icd->isVisibleInHierarchy()) count++;
  }
  for (const auto &ibcd : m_impl->inherits)
  {
    const ClassDef *icd=ibcd.classDef;
    if ( icd->isVisibleInHierarchy()) count++;
  }
  return count;
}

void ClassDefImpl::writeInheritanceGraph(OutputList &ol) const
{
  bool haveDot    = Config_getBool(HAVE_DOT);
  auto classGraph = Config_getEnum(CLASS_GRAPH);

  if (classGraph == CLASS_GRAPH_t::NO) return;
  // count direct inheritance relations
  const int count=countInheritanceNodes();

  bool renderDiagram = FALSE;
  if (haveDot && (classGraph==CLASS_GRAPH_t::YES || classGraph==CLASS_GRAPH_t::GRAPH))
    // write class diagram using dot
  {
    DotClassGraph inheritanceGraph(this,Inheritance);
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
  else if ((classGraph==CLASS_GRAPH_t::YES || classGraph==CLASS_GRAPH_t::GRAPH) && count>0)
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

  if (!m_impl->inherits.empty())
  {
    auto replaceFunc = [this,&ol](size_t entryIndex)
    {
      BaseClassDef &bcd=m_impl->inherits[entryIndex];
      ClassDef *cd=bcd.classDef;

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
    };

    ol.startParagraph();
    writeMarkerList(ol,
                    theTranslator->trInheritsList(static_cast<int>(m_impl->inherits.size())).str(),
                    m_impl->inherits.size(),
                    replaceFunc);
    ol.endParagraph();
  }

  // write subclasses
  if (!m_impl->inheritedBy.empty())
  {

    auto replaceFunc = [this,&ol](size_t entryIndex)
    {
      BaseClassDef &bcd=m_impl->inheritedBy[entryIndex];
      ClassDef *cd=bcd.classDef;
      if (cd->isLinkable())
      {
        ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),cd->displayName());
      }
      else
      {
        ol.docify(cd->displayName());
      }
      writeInheritanceSpecifier(ol,bcd);
    };

    ol.startParagraph();
    writeMarkerList(ol,
                    theTranslator->trInheritedByList(static_cast<int>(m_impl->inheritedBy.size())).str(),
                    m_impl->inheritedBy.size(),
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
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(COLLABORATION_GRAPH)*/)
  {
    DotClassGraph usageImplGraph(this,Collaboration);
    if (!usageImplGraph.isTrivial())
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

QCString ClassDefImpl::includeStatement() const
{
  SrcLangExt lang = getLanguage();
  bool isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
  if (isIDLorJava)
  {
    return "import";
  }
  else if (isObjectiveC())
  {
    return "#import ";
  }
  else
  {
    return "#include ";
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
      unsigned int length = 0;
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
  if (m_impl->spec & Entry::Local)
  {
    ol.docify("local ");
  }
  if (m_impl->spec & Entry::Interface)
  {
    ol.docify("interface ");
  }
  else if (m_impl->spec & Entry::Struct)
  {
    ol.docify("struct ");
  }
  else if (m_impl->spec & Entry::Exception)
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
    if (m_impl->spec & (Entry::Interface|Entry::Exception))
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
      ol.docify(includeStatement());
      bool isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
      if (m_impl->incInfo->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify("<");
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
      if (m_impl->incInfo->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify(">");
      if (isIDLorJava)
        ol.docify(";");
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
      mg->writeDeclarations(ol,this,0,0,0,showInline);
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
  m_impl->innerClasses.writeDeclaration(ol,0,title,TRUE);
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
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=TRUE;
  SrcLangExt lang = getLanguage();

  if (lang!=SrcLangExt_VHDL)
  {
    for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Class))
    {
      if (lde->kind()==LayoutDocEntry::ClassNestedClasses &&
          m_impl->innerClasses.declVisible()
         )
      {
        const LayoutDocEntrySection *ls  = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
        if (ls)
        {
          ol.writeSummaryLink(QCString(),"nested-classes",ls->title(lang),first);
          first=FALSE;
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
            ol.writeSummaryLink(QCString(),MemberList::listTypeAsString(ml->listType()),lmd->title(lang),first);
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
  tagFile << "    <filename>" << convertToXML(addHtmlExtensionIfMissing(getOutputFileBase())) << "</filename>\n";
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
      if (!Config_getString(GENERATE_TAGFILE).isEmpty())
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
        tagFile << ">" << convertToXML(cd->name()) << "</base>\n";
      }
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
            if (innerCd->isLinkableInProject() && innerCd->templateMaster()==0 &&
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
      else if (lang==SrcLangExt_VHDL)
      {
        ol.parseText(theTranslator->trVhdlType(VhdlDocGen::ARCHITECTURE,FALSE));
      }
      else
      {
        ol.parseText(lang==SrcLangExt_Fortran ?
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

    if (lang!=SrcLangExt_VHDL) // for VHDL we swap the name and the type
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
    if (lang==SrcLangExt_VHDL) // now write the type
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
                                briefFile(),briefLine(),this,0,
                                briefDescription(),FALSE,FALSE,
                                QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
      if (!ast->isEmpty())
      {
        ol.startMemberDescription(anchor());
        ol.writeDoc(ast.get(),this,0);
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
  if (isFinal())    sl.push_back("final");
  if (isSealed())   sl.push_back("sealed");
  if (isAbstract()) sl.push_back("abstract");
  if (getLanguage()==SrcLangExt_IDL && isPublished()) sl.push_back("published");

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
        if (lang==SrcLangExt_Slice)
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
      case LayoutDocEntry::GroupInlineClasses:
      case LayoutDocEntry::GroupNamespaces:
      case LayoutDocEntry::GroupDirs:
      case LayoutDocEntry::GroupNestedGroups:
      case LayoutDocEntry::GroupFiles:
      case LayoutDocEntry::GroupGraph:
      case LayoutDocEntry::GroupPageDocs:
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

  if (lang==SrcLangExt_Fortran)
  {
    pageTitle = theTranslator->trCompoundReferenceFortran(displayName(),
              m_impl->compType,
              !m_impl->tempArgs.empty());
  }
  else if (lang==SrcLangExt_Slice)
  {
    pageTitle = theTranslator->trCompoundReferenceSlice(displayName(),
              m_impl->compType,
              isSliceLocal());
  }
  else if (lang==SrcLangExt_VHDL)
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
                m_impl->compType == Interface && getLanguage()==SrcLangExt_ObjC ? Class : m_impl->compType,
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

  HighlightedItem hli;
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
    if (ml->numDocMembers()>ml->numDocEnumValues() && (ml->listType()&MemberListType_detailedLists))
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
          ol.writeString(addHtmlExtensionIfMissing(md->getOutputFileBase())+"#"+md->anchor());
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
        innerCd->isLinkableInProject() && innerCd->templateMaster()==0 &&
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

  HighlightedItem hli;
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
            //if (bd==0) bd=cd;
            ol.writeObjectLink(md->getReference(),
                md->getOutputFileBase(),
                md->anchor(),name);

            if ( md->isFunction() || md->isSignal() || md->isSlot() ||
                (md->isFriend() && md->argsString().isEmpty()))
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
            (prot!=Protection::Public || (virt!=Specifier::Normal && getLanguage()!=SrcLangExt_ObjC) ||
             md->isFriend() || md->isRelated() || md->isExplicit() ||
             md->isMutable() || (md->isInline() && Config_getBool(INLINE_INFO)) ||
             md->isSignal() || md->isSlot() ||
             (getLanguage()==SrcLangExt_IDL &&
              (md->isOptional() || md->isAttribute() || md->isUNOProperty())) ||
             md->isStatic() || lang==SrcLangExt_VHDL
            )
            && memberWritten)
        {
          StringVector sl;
          if (lang==SrcLangExt_VHDL)
          {
            sl.push_back(theTranslator->trVhdlType(md->getMemberSpecifiers(),TRUE).str()); //append vhdl type
          }
          else if (md->isFriend()) sl.push_back("friend");
          else if (md->isRelated()) sl.push_back("related");
          else
          {
            if (Config_getBool(INLINE_INFO) && md->isInline())
                                                   sl.push_back("inline");
            if (md->isExplicit())                  sl.push_back("explicit");
            if (md->isMutable())                   sl.push_back("mutable");
            if (prot==Protection::Protected)       sl.push_back("protected");
            else if (prot==Protection::Private)    sl.push_back("private");
            else if (prot==Protection::Package)    sl.push_back("package");
            if (virt==Specifier::Virtual && getLanguage()!=SrcLangExt_ObjC)
                                                   sl.push_back("virtual");
            else if (virt==Specifier::Pure)        sl.push_back("pure virtual");
            if (md->isStatic())                    sl.push_back("static");
            if (md->isSignal())                    sl.push_back("signal");
            if (md->isSlot())                      sl.push_back("slot");
// this is the extra member page
            if (md->isOptional())                  sl.push_back("optional");
            if (md->isAttribute())                 sl.push_back("attribute");
            if (md->isUNOProperty())               sl.push_back("property");
            if (md->isReadonly())                  sl.push_back("readonly");
            if (md->isBound())                     sl.push_back("bound");
            if (md->isRemovable())                 sl.push_back("removable");
            if (md->isConstrained())               sl.push_back("constrained");
            if (md->isTransient())                 sl.push_back("transient");
            if (md->isMaybeVoid())                 sl.push_back("maybevoid");
            if (md->isMaybeDefault())              sl.push_back("maybedefault");
            if (md->isMaybeAmbiguous())            sl.push_back("maybeambiguous");
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
  if (cd==0 && !hideUndocRelation)
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
    mg->writePlainDeclarations(ol,inGroup,this,0,0,0,indentLevel,inheritedFrom,inheritId);
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
// class of class definition 'cd' or 0 if it does not.

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

/*!
 * recursively merges the 'all members' lists of a class base
 * with that of this class. Must only be called for classes without
 * subclasses!
 */
void ClassDefImpl::mergeMembers()
{
  if (m_impl->membersMerged) return;

  //bool optimizeOutputForJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  SrcLangExt lang = getLanguage();
  QCString sep=getLanguageSpecificSeparator(lang,TRUE);
  uint32_t sepLen = sep.length();

  m_impl->membersMerged=TRUE;
  //printf("  mergeMembers for %s\n",qPrint(name()));
  bool inlineInheritedMembers = Config_getBool(INLINE_INHERITED_MEMB);
  bool extractPrivate         = Config_getBool(EXTRACT_PRIVATE);
  for (const auto &bcd : baseClasses())
  {
    ClassDefMutable *bClass=toClassDefMutable(bcd.classDef);
    if (bClass)
    {
      // merge the members in the base class of this inheritance branch first
      bClass->mergeMembers();
      if (bClass->getLanguage()==SrcLangExt_Python) continue; // python does not have member overloading, see issue 8480

      const MemberNameInfoLinkedMap &srcMnd  = bClass->memberNameInfoLinkedMap();
      MemberNameInfoLinkedMap &dstMnd        = m_impl->allMemberNameInfoLinkedMap;

      for (auto &srcMni : srcMnd)
      {
        //printf("    Base member name %s\n",srcMni->memberName());
        MemberNameInfo *dstMni;
        if ((dstMni=dstMnd.find(srcMni->memberName())))
          // a member with that name is already in the class.
          // the member may hide or reimplement the one in the sub class
          // or there may be another path to the base class that is already
          // visited via another branch in the class hierarchy.
        {
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
                  ArgumentList &srcAl = const_cast<ArgumentList&>(srcMd->argumentList());
                  ArgumentList &dstAl = const_cast<ArgumentList&>(dstMd->argumentList());
                  found=matchArguments2(
                      srcMd->getOuterScope(),srcMd->getFileDef(),&srcAl,
                      dstMd->getOuterScope(),dstMd->getFileDef(),&dstAl,
                      TRUE,getLanguage()
                      );
                  //printf("  Yes, matching (%s<->%s): %d\n",
                  //    qPrint(argListToString(srcMd->argumentList())),
                  //    qPrint(argListToString(dstMd->argumentList())),
                  //    found);
                  hidden = hidden  || !found;
                }
                else // member is in a non base class => multiple inheritance
                  // using the same base class.
                {
                  //printf("$$ Existing member %s %s add scope %s\n",
                  //    qPrint(dstMi->ambiguityResolutionScope),
                  //    qPrint(dstMd->name()),
                  //    qPrint(dstMi->scopePath.left(dstMi->scopePath.find("::")+2));

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
                //printf("same member found srcMi->virt=%d dstMi->virt=%d\n",srcMi->virt,dstMi->virt);
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
                  //printf("$$ Existing member %s %s add scope %s\n",
                  //    qPrint(dstMi->ambiguityResolutionScope),
                  //    qPrint(dstMd->name()),
                  //    qPrint(dstMi->scopePath.left(dstMi->scopePath.find("::")+2));

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
            //printf("member %s::%s hidden %d ambiguous %d srcMi->ambigClass=%p\n",
            //    qPrint(srcCd->name()),qPrint(srcMd->name()),hidden,ambiguous,srcMi->ambigClass);

            // TODO: fix the case where a member is hidden by inheritance
            //       of a member with the same name but with another prototype,
            //       while there is more than one path to the member in the
            //       base class due to multiple inheritance. In this case
            //       it seems that the member is not reachable by prefixing a
            //       scope name either (according to my compiler). Currently,
            //       this case is shown anyway.
            if (!found && srcMd->protection()!=Protection::Private && !srcMd->isFriend())
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
                  //printf("    insertMember '%s'\n",qPrint(srcMd->name()));
                  internalInsertMember(srcMd,prot,FALSE);
                }
              }

              Specifier virt=srcMi->virt();
              if (virt==Specifier::Normal && bcd.virt!=Specifier::Normal) virt=bcd.virt;

              std::unique_ptr<MemberInfo> newMi = std::make_unique<MemberInfo>(srcMd,prot,virt,TRUE);
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
                if (srcMi->ambigClass()==0)
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
          // create a deep copy of the list (only the MemberInfo's will be
          // copied, not the actual MemberDef's)
          MemberNameInfo *newMni = dstMnd.add(srcMni->memberName());

          // copy the member(s) from the base to the sub class
          for (auto &mi : *srcMni)
          {
            if (!mi->memberDef()->isFriend()) // don't inherit friends
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
              //printf("%s::%s: prot=%d bcd.prot=%d result=%d\n",
              //    qPrint(name()),qPrint(mi->memberDef->name()),mi->prot,
              //    bcd.prot,prot);

              if (prot!=Protection::Private || extractPrivate)
              {
                Specifier virt=mi->virt();
                if (virt==Specifier::Normal && bcd.virt!=Specifier::Normal) virt=bcd.virt;

                if (inlineInheritedMembers)
                {
                  if (!isStandardFunc(mi->memberDef()))
                  {
                    //printf("    insertMember '%s'\n",qPrint(mi->memberDef->name()));
                    internalInsertMember(mi->memberDef(),prot,FALSE);
                  }
                }
                //printf("Adding!\n");
                std::unique_ptr<MemberInfo> newMi = std::make_unique<MemberInfo>(mi->memberDef(),prot,virt,TRUE);
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
  //printf("  end mergeMembers\n");
}

//----------------------------------------------------------------------------

/*! Merges the members of a Objective-C category into this class.
 */
void ClassDefImpl::mergeCategory(ClassDef *cat)
{
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
        //printf("Existing member %s\n",srcMni->memberName());
        auto &dstMi = dstMni->front();
        auto &srcMi = srcMni->front();
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
        //printf("New member %s\n",srcMni->memberName());
        // create a deep copy of the list
        MemberNameInfo *newMni = dstMnd.add(srcMni->memberName());

        // copy the member(s) from the category to this class
        for (auto &mi : *srcMni)
        {
          //printf("Adding '%s'\n",qPrint(mi->memberDef->name()));
          Protection prot = mi->prot();
          //if (makePrivate) prot = Private;
          std::unique_ptr<MemberDefMutable> newMd { toMemberDefMutable(mi->memberDef()->deepCopy()) };
          if (newMd)
          {
            //printf("Copying member %s\n",qPrint(mi->memberDef->name()));
            newMd->moveTo(this);

            std::unique_ptr<MemberInfo> newMi=std::make_unique<MemberInfo>(newMd.get(),prot,mi->virt(),mi->inherited());
            newMi->setScopePath(mi->scopePath());
            newMi->setAmbigClass(mi->ambigClass());
            newMi->setAmbiguityResolutionScope(mi->ambiguityResolutionScope());
            newMni->push_back(std::move(newMi));

            // also add the newly created member to the global members list

            QCString name = newMd->name();
            MemberName *mn = Doxygen::memberNameLinkedMap->add(name);

            if (newMd)
            {
              newMd->setCategory(category);
              newMd->setCategoryRelation(mi->memberDef());
            }
            MemberDefMutable *mdm = toMemberDefMutable(mi->memberDef());
            if (mdm)
            {
              mdm->setCategoryRelation(newMd.get());
            }
            if (newMd && (makePrivate || isExtension))
            {
              newMd->makeImplementationDetail();
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
  if (getLanguage()==SrcLangExt_Fortran)
  {
    switch (m_impl->compType)
    {
      case Class:     return "module";
      case Struct:    return "type";
      case Union:     return "union";
      case Interface: return "interface";
      case Protocol:  return "protocol";
      case Category:  return "category";
      case Exception: return "exception";
      default:        return "unknown";
    }
  }
  else
  {
    switch (m_impl->compType)
    {
      case Class:     return isJavaEnum() ? "enum" : "class";
      case Struct:    return "struct";
      case Union:     return "union";
      case Interface: return getLanguage()==SrcLangExt_ObjC ? "class" : "interface";
      case Protocol:  return "protocol";
      case Category:  return "category";
      case Exception: return "exception";
      case Service:   return "service";
      case Singleton: return "singleton";
      default:        return "unknown";
    }
  }
}

QCString ClassDefImpl::getOutputFileBase() const
{
  bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  if (!Doxygen::generatingXmlOutput)
  {
    Definition *scope=0;
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
  ClassDefMutable *templateClass=0;
  if (it!=m_impl->templateInstances.end())
  {
    templateClass = toClassDefMutable((*it).classDef);
  }
  if (templateClass==0)
  {
    QCString tcname = removeRedundantWhiteSpace(localName()+templSpec);
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
      m_impl->templateInstances.push_back(TemplateInstanceDef(templSpec,templateClass));

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
        }
      }
      freshInstance=TRUE;
    }
  }
  return templateClass;
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
      std::unique_ptr<MemberDefMutable> imd { md->createTemplateInstanceMember(
                          templateArguments,actualArguments_p) };
      //printf("%s->setMemberClass(%p)\n",qPrint(imd->name()),this);
      imd->setMemberClass(this);
      imd->setTemplateMaster(md);
      imd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
      imd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
      imd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
      imd->setMemberSpecifiers(md->getMemberSpecifiers());
      imd->setMemberGroupId(md->getMemberGroupId());
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
    if (ml->listType()&MemberListType_detailedLists)
    {
      ml->addListReferences(this);
    }
  }
}

const MemberDef *ClassDefImpl::getMemberByName(const QCString &name) const
{
  const MemberDef *xmd = 0;
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
      if (m<mdist && mcd->isLinkable())
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
  return 0;
}

void ClassDefImpl::addMemberToList(MemberListType lt,MemberDef *md,bool isBrief)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_impl->memberLists.get(lt,MemberListContainer::Class);
  ml->setNeedsSorting((isBrief && sortBriefDocs) || (!isBrief && sortMemberDocs));
  ml->push_back(md);

  // for members in the declaration lists we set the section, needed for member grouping
  if ((ml->listType()&MemberListType_detailedLists)==0)
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
  std::sort(m_impl->innerClasses.begin(),
            m_impl->innerClasses.end(),
            [](const auto &c1,const auto &c2)
            {
               return Config_getBool(SORT_BY_SCOPE_NAME)           ?
                      qstricmp(c1->name(),      c2->name()     )<0 :
                      qstricmp(c1->className(), c2->className())<0 ;
            });
}

int ClassDefImpl::countMemberDeclarations(MemberListType lt,const ClassDef *inheritedFrom,
                                      int lt2,bool invert,bool showAlways,ClassDefSet &visitedClasses) const
{
  //printf("%s: countMemberDeclarations for %d and %d\n",qPrint(name()),lt,lt2);
  int count=0;
  MemberList * ml  = getMemberList(lt);
  MemberList * ml2 = getMemberList(static_cast<MemberListType>(lt2));
  if (getLanguage()!=SrcLangExt_VHDL) // use specific declarations function
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
        if (lt2!=-1) count+=mg->countGroupedInheritedMembers(static_cast<MemberListType>(lt2));
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
  //    qPrint(name()),lt,process,count,invert);
  if ((process^invert) || showAlways)
  {
    for (const auto &ibcd : m_impl->inherits)
    {
      ClassDefMutable *icd=toClassDefMutable(ibcd.classDef);
      int lt1,lt2;
      if (icd && icd->isLinkable())
      {
        convertProtectionLevel(lt,ibcd.prot,&lt1,&lt2);
        //printf("%s: convert %d->(%d,%d) prot=%d\n",
        //    qPrint(icd->name()),lt,lt1,lt2,ibcd->prot);
        if (visitedClasses.find(icd)==visitedClasses.end())
        {
          visitedClasses.insert(icd); // guard for multiple virtual inheritance
          if (lt1!=-1)
          {
            inhCount+=icd->countMemberDeclarations(static_cast<MemberListType>(lt1),inheritedFrom,lt2,FALSE,TRUE,visitedClasses);
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
      if (lmd && lmd->type!=MemberListType_friends) // friendship is not inherited
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
      if (lmd && lmd->type!=MemberListType_friends)
      {
        ClassDefSet visited;
        writeInheritedMemberDeclarations(ol,visited,lmd->type,-1,lmd->title(getLanguage()),this,TRUE,FALSE);
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
  //printf("%s:countMembersIncludingGrouped(lt=%d,%s)=%d\n",
  //    qPrint(name()),lt,ml?qPrint(ml->listTypeAsString(ml->listType())):"<none>",count);
  return count;
}


void ClassDefImpl::writeInheritedMemberDeclarations(OutputList &ol,ClassDefSet &visitedClasses,
               MemberListType lt,int lt2,const QCString &title,
               const ClassDef *inheritedFrom,bool invert,bool showAlways) const
{
  int count = countMembersIncludingGrouped(lt,inheritedFrom,FALSE);
  bool process = count>0;
  //printf("%s: writeInheritedMemberDec: lt=%d process=%d invert=%d always=%d\n",
  //    qPrint(name()),lt,process,invert,showAlways);
  if ((process^invert) || showAlways)
  {
    for (const auto &ibcd : m_impl->inherits)
    {
      ClassDefMutable *icd=toClassDefMutable(ibcd.classDef);
      if (icd && icd->isLinkable())
      {
        int lt1,lt3;
        convertProtectionLevel(lt,ibcd.prot,&lt1,&lt3);
        if (lt2==-1 && lt3!=-1)
        {
          lt2=lt3;
        }
        //printf("%s:convert %d->(%d,%d) prot=%d\n",qPrint(icd->name()),lt,lt1,lt2,ibcd->prot);
        if (visitedClasses.find(icd)==visitedClasses.end())
        {
          visitedClasses.insert(icd); // guard for multiple virtual inheritance
          if (lt1!=-1)
          {
            icd->writeMemberDeclarations(ol,visitedClasses,static_cast<MemberListType>(lt1),
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
               const QCString &subTitle,bool showInline,const ClassDef *inheritedFrom,int lt2,
               bool invert,bool showAlways) const
{
  //printf("%s: ClassDefImpl::writeMemberDeclarations lt=%d lt2=%d\n",qPrint(name()),lt,lt2);
  MemberList * ml = getMemberList(lt);
  MemberList * ml2 = getMemberList(static_cast<MemberListType>(lt2));
  if (getLanguage()==SrcLangExt_VHDL) // use specific declarations function
  {
    static const ClassDef *cdef;
    if (cdef!=this)
    { // only one inline link
      VhdlDocGen::writeInlineClassLink(this,ol);
      cdef=this;
    }
    if (ml)
    {
      VhdlDocGen::writeVhdlDeclarations(ml,ol,0,this,0,0);
    }
  }
  else
  {
    //printf("%s::writeMemberDeclarations(%s) ml=%p ml2=%p\n",qPrint(name()),qPrint(title),ml,ml2);
    QCString tt = title, st = subTitle;
    if (ml)
    {
      //printf("  writeDeclaration type=%d count=%d\n",lt,ml->numDecMembers());
      ml->writeDeclarations(ol,this,0,0,0,tt,st,FALSE,showInline,inheritedFrom,lt);
      tt.resize(0);
      st.resize(0);
    }
    if (ml2)
    {
      //printf("  writeDeclaration type=%d count=%d\n",lt2,ml2->numDecMembers());
      ml2->writeDeclarations(ol,this,0,0,0,tt,st,FALSE,showInline,inheritedFrom,lt);
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
  //printf("** %s::addGroupedInheritedMembers(%p) inheritId=%s\n",qPrint(name()),m_impl->memberGroupSDict,qPrint(inheritId));
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
    ml->writePlainDeclarations(ol,inGroup,this,0,0,0,indentLevel,inheritedFrom,inheritId);
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
  std::sort(m_impl->allMemberNameInfoLinkedMap.begin(),
            m_impl->allMemberNameInfoLinkedMap.end(),
            [](const auto &m1,const auto &m2)
            {
              return qstricmp(m1->memberName(),m2->memberName())<0;
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

//NamespaceDef *ClassDefImpl::getNamespaceDef() const
//{
//  return m_impl->nspace;
//}

FileDef *ClassDefImpl::getFileDef() const
{
  return m_impl->fileDef;
}

const TemplateInstanceList &ClassDefImpl::getTemplateInstances() const
{
  return m_impl->templateInstances;
}

const ClassDef *ClassDefImpl::templateMaster() const
{
  return m_impl->templateMaster;
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
  return m_impl->isAbstract || (m_impl->spec&Entry::Abstract);
}

bool ClassDefImpl::isFinal() const
{
  return m_impl->spec&Entry::Final;
}

bool ClassDefImpl::isSealed() const
{
  return m_impl->spec&Entry::Sealed;
}

bool ClassDefImpl::isPublished() const
{
  return m_impl->spec&Entry::Published;
}

bool ClassDefImpl::isForwardDeclared() const
{
  return m_impl->spec&Entry::ForwardDecl;
}

bool ClassDefImpl::isInterface() const
{
  return m_impl->spec&Entry::Interface;
}

bool ClassDefImpl::isObjectiveC() const
{
  return getLanguage()==SrcLangExt_ObjC;
}

bool ClassDefImpl::isFortran() const
{
  return getLanguage()==SrcLangExt_Fortran;
}

bool ClassDefImpl::isCSharp() const
{
  return getLanguage()==SrcLangExt_CSharp;
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
  m_impl->fileDef=fd;
}

void ClassDefImpl::setSubGrouping(bool enabled)
{
  m_impl->subGrouping = enabled;
}

void ClassDefImpl::setProtection(Protection p)
{
  m_impl->prot=p;
  if (getLanguage()==SrcLangExt_VHDL && VhdlDocGen::convert(p)==VhdlDocGen::ARCHITECTURECLASS)
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

void ClassDefImpl::setClassSpecifier(uint64_t spec)
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
  return m_impl->spec&Entry::Local;
}

void ClassDefImpl::setName(const QCString &name)
{
  DefinitionMixin::setName(name);
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
    return 0;
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
    return 0;
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
    return 0;
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
    return 0;
  }
}

// --- Helpers

/*! Get a class definition given its name.
 *  Returns 0 if the class is not found.
 */
ClassDef *getClass(const QCString &n)
{
  if (n.isEmpty()) return 0;
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

  if (cd->getLanguage()==SrcLangExt_VHDL) // reverse baseClass/subClass relation
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


