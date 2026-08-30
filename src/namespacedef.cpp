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

// own header
#include "namespacedef.h"

// other includes
#include "classdef.h"
#include "classlist.h"
#include "conceptdef.h"
#include "config.h"
#include "definitionimpl.h"
#include "docparser.h"
#include "doxygen.h"
#include "language.h"
#include "layout.h"
#include "membergroup.h"
#include "memberlist.h"
#include "membername.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"

//------------------------------------------------------------------
static DString makeDisplayName(const NamespaceDef *nd,bool includeScope)
{
  DString result=includeScope ? nd->name() : nd->localName();
  SrcLangExt lang = nd->getLanguage();
  DString sep = getLanguageSpecificSeparator(lang);
  if (sep!="::")
  {
    result = substitute(result,"::",sep);
  }
  if (nd->isAnonymous())
  {
    result = removeAnonymousScopes(result);
  }
  //printf("makeDisplayName() %s->%s lang=%d\n",qPrint(name()),qPrint(result),lang);
  return result;
}
//------------------------------------------------------------------

class NamespaceDefImpl final : public DefinitionMixin<NamespaceDefMutable>
{
  public:
    NamespaceDefImpl(const DString &defFileName,int defLine,size_t defColumn,
                 const DString &name,const DString &ref=DString(),
                 const DString &refFile=DString(),const DString &type=DString(),
                 bool isPublished=false);
   ~NamespaceDefImpl() override;
    NON_COPYABLE(NamespaceDefImpl)

    DefType definitionType() const override { return TypeNamespace; }
    CodeSymbolType codeSymbolType() const override
    { return getLanguage()==SrcLangExt::Java ? CodeSymbolType::Package : CodeSymbolType::Namespace; }
    DString getOutputFileBase() const override;
    DString anchor() const override { return DString(); }
    void insertUsedFile(FileDef *fd) override;
    void writeDocumentation(OutputList &ol) override;
    void writeMemberPages(OutputList &ol) override;
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const override;
    void writeTagFile(TextStream &) override;
    void insertClass(ClassDef *cd) override;
    void insertConcept(ConceptDef *cd) override;
    void insertNamespace(NamespaceDef *nd) override;
    void insertMember(MemberDef *md) override;
    void computeAnchors() override;
    void countMembers() override;
    int  numDocMembers() const override;
    void addUsingDirective(NamespaceDef *nd) override;
    const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const override { return m_usingDirList; }
    void addUsingDeclaration(const Definition *cd) override;
    const LinkedRefMap<const Definition> &getUsedDefinitions() const override { return m_usingDeclList; }
    void combineUsingRelations(NamespaceDefSet &visitedNamespace) override;
    DString displayName(bool=true) const override;
    void setInline(bool isInline) override { m_inline = isInline; }
    bool isConstantGroup() const override { return CONSTANT_GROUP == m_type; }
    bool isModule()        const override { return NAMESPACE == m_type || MODULE == m_type; }
    bool isLibrary() const override { return LIBRARY == m_type; }
    bool isInline() const override { return m_inline; }
    bool isLinkableInProject() const override;
    bool isLinkable() const override;
    bool isVisibleInHierarchy() const override;
    bool hasDetailedDescription() const override;
    void addMembersToMemberGroup() override;
    void distributeMemberGroupDocumentation() override;
    void findSectionsInDocumentation() override;
    void sortMemberLists() override;
    const Definition *findInnerCompound(const DString &name) const override;
    void addInnerCompound(Definition *d) override;
    void addListReferences() override;
    void addRequirementReferences() override;
    void setFileName(const DString &fn) override;
    bool subGrouping() const override { return m_subGrouping; }
    MemberList *getMemberList(MemberListType lt) const override;
    const MemberLists &getMemberLists() const override { return m_memberLists; }
    const MemberDef *getMemberByName(const DString &) const override;
    const MemberGroupList &getMemberGroups() const override { return m_memberGroups; }
    ClassLinkedRefMap getClasses() const override { return m_classes; }
    ClassLinkedRefMap getInterfaces() const override { return m_interfaces; }
    ClassLinkedRefMap getStructs() const override { return m_structs; }
    ClassLinkedRefMap getExceptions() const override { return m_exceptions; }
    NamespaceLinkedRefMap getNamespaces() const override { return m_namespaces; }
    ConceptLinkedRefMap getConcepts() const override { return m_concepts; }
    void setName(const DString &name) override;

    DString title() const override;
    DString compoundTypeString() const override;

    void setMetaData(const DString &m) override;
    int countVisibleMembers() const override;
    void writeSummaryLinks(OutputList &ol) const override;
    void writePageNavigation(OutputList &ol) const override;

  private:
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const DString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const DString &title);
    void writeDetailedDescription(OutputList &ol,const DString &title);
    void writeBriefDescription(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void writeClassDeclarations(OutputList &ol,const DString &title,const ClassLinkedRefMap &d);
    void writeConcepts(OutputList &ol,const DString &title);
    void writeInlineClasses(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void addNamespaceAttributes(OutputList &ol);
    void writeClassesToTagFile(TextStream &,const ClassLinkedRefMap &d);
    void writeConceptsToTagFile(TextStream &);
    void setFileNameLocal(const DString &fn);

    void writeNamespaceDeclarations(OutputList &ol,const DString &title,
            bool isConstantGroup=false);
    void updateLocalName();

    DString              fileName;
    FileList              files;

    LinkedRefMap<NamespaceDef> m_usingDirList;
    LinkedRefMap<const Definition> m_usingDeclList;
    LinkedRefMap<const Definition> m_innerCompounds;

    MemberLinkedRefMap    m_allMembers;
    MemberLists           m_memberLists;
    MemberGroupList       m_memberGroups;
    ClassLinkedRefMap     m_classes;
    ClassLinkedRefMap     m_interfaces;
    ClassLinkedRefMap     m_structs;
    ClassLinkedRefMap     m_exceptions;
    ConceptLinkedRefMap   m_concepts;
    NamespaceLinkedRefMap m_namespaces;
    bool                  m_subGrouping = false;
    enum { NAMESPACE, MODULE, CONSTANT_GROUP, LIBRARY } m_type;
    bool m_isPublished = false;
    DString              metaData;
    bool                  m_inline = false;
};

std::unique_ptr<NamespaceDef> createNamespaceDef(const DString &defFileName,int defLine,size_t defColumn,
                 const DString &name,const DString &ref,
                 const DString &refFile,const DString &type,
                 bool isPublished)
{
  //printf("createNamespaceDef(%s)\n",qPrint(name));
  return std::make_unique<NamespaceDefImpl>(defFileName,defLine,defColumn,name,ref,refFile,type,isPublished);
}

//------------------------------------------------------------------

class NamespaceDefAliasImpl final : public DefinitionAliasMixin<NamespaceDef>
{
  public:
    NamespaceDefAliasImpl(const Definition *newScope,const NamespaceDef *nd)
      : DefinitionAliasMixin(newScope,nd) { init(); }
    ~NamespaceDefAliasImpl()  override { deinit(); }
    NON_COPYABLE(NamespaceDefAliasImpl)

    DefType definitionType() const override { return TypeNamespace; }

    const NamespaceDef *getNSAlias() const { return toNamespaceDef(getAlias()); }

    // ---- getters
    CodeSymbolType codeSymbolType() const override
    { return getNSAlias()->codeSymbolType(); }
    DString getOutputFileBase() const override
    { return getNSAlias()->getOutputFileBase(); }
    DString anchor() const override
    { return getNSAlias()->anchor(); }
    int numDocMembers() const override
    { return getNSAlias()->numDocMembers(); }
    const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const override
    { return getNSAlias()->getUsedNamespaces(); }
    const LinkedRefMap<const Definition> &getUsedDefinitions() const override
    { return getNSAlias()->getUsedDefinitions(); }
    DString displayName(bool b=true) const override
    { return makeDisplayName(this,b); }
    const DString &localName() const override
    { return getNSAlias()->localName(); }
    bool isConstantGroup() const override
    { return getNSAlias()->isConstantGroup(); }
    bool isModule() const override
    { return getNSAlias()->isModule(); }
    bool isLibrary() const override
    { return getNSAlias()->isLibrary(); }
    bool isInline() const override
    { return getNSAlias()->isInline(); }
    bool isLinkableInProject() const override
    { return getNSAlias()->isLinkableInProject(); }
    bool isLinkable() const override
    { return getNSAlias()->isLinkable(); }
    bool isVisibleInHierarchy() const override
    { return getNSAlias()->isVisibleInHierarchy(); }
    bool hasDetailedDescription() const override
    { return getNSAlias()->hasDetailedDescription(); }
    const Definition *findInnerCompound(const DString &name) const override
    { return getNSAlias()->findInnerCompound(name); }
    bool subGrouping() const override
    { return getNSAlias()->subGrouping(); }
    MemberList *getMemberList(MemberListType lt) const override
    { return getNSAlias()->getMemberList(lt); }
    const MemberLists &getMemberLists() const override
    { return getNSAlias()->getMemberLists(); }
    const MemberDef *getMemberByName(const DString &name) const override
    { return getNSAlias()->getMemberByName(name); }
    const MemberGroupList &getMemberGroups() const override
    { return getNSAlias()->getMemberGroups(); }
    ClassLinkedRefMap getClasses() const override
    { return getNSAlias()->getClasses(); }
    ClassLinkedRefMap getInterfaces() const override
    { return getNSAlias()->getInterfaces(); }
    ClassLinkedRefMap getStructs() const override
    { return getNSAlias()->getStructs(); }
    ClassLinkedRefMap getExceptions() const override
    { return getNSAlias()->getExceptions(); }
    NamespaceLinkedRefMap getNamespaces() const override
    { return getNSAlias()->getNamespaces(); }
    ConceptLinkedRefMap getConcepts() const override
    { return getNSAlias()->getConcepts(); }
    DString title() const override
    { return getNSAlias()->title(); }
    DString compoundTypeString() const override
    { return getNSAlias()->compoundTypeString(); }
    int countVisibleMembers() const override
    { return getNSAlias()->countVisibleMembers(); }
};

std::unique_ptr<NamespaceDef> createNamespaceDefAlias(const Definition *newScope,const NamespaceDef *nd)
{
  auto alnd = std::make_unique<NamespaceDefAliasImpl>(newScope,nd);
  //printf("alnd name=%s localName=%s qualifiedName=%s displayName()=%s\n",
  //    qPrint(alnd->name()),qPrint(alnd->localName()),qPrint(alnd->qualifiedName()),
  //    qPrint(alnd->displayName()));
  return alnd;
}

//------------------------------------------------------------------

NamespaceDefImpl::NamespaceDefImpl(const DString &df,int dl,size_t dc,
                           const DString &name,const DString &lref,
                           const DString &fName, const DString &type,
                           bool isPublished) :
   DefinitionMixin(df,dl,dc,name)
  ,m_isPublished(isPublished)
{
  if (!fName.empty())
  {
    if (!lref.empty())
    {
      fileName = stripExtension(fName);
    }
    else
    {
      fileName = convertNameToFile(stripExtension(fName));
    }
  }
  else
  {
    setFileNameLocal(name);
  }
  setReference(lref);
  m_inline=false;
  m_subGrouping=Config_getBool(SUBGROUPING);
  if (type=="module")
  {
    m_type = MODULE;
  }
  else if (type=="constants")
  {
    m_type = CONSTANT_GROUP;
  }
  else if (type=="library")
  {
    m_type = LIBRARY;
  }
  else
  {
    m_type = NAMESPACE;
  }

  updateLocalName();
}

void NamespaceDefImpl::updateLocalName()
{
  DString locName=name();
  if (size_t i=locName.rfind("::"); i!=DString::npos) locName=locName.mid(i+2);
  setLocalName(locName);
}

void NamespaceDefImpl::setName(const DString &name)
{
  DefinitionMixin<NamespaceDefMutable>::setName(name);
  updateLocalName();
}

NamespaceDefImpl::~NamespaceDefImpl()
{
}

void NamespaceDefImpl::setFileNameLocal(const DString &fn)
{
  if (isReference())
  {
    fileName = "namespace"+fn;
  }
  else
  {
    fileName = convertNameToFile("namespace"+fn);
  }
}

void NamespaceDefImpl::setFileName(const DString &fn)
{
  setFileNameLocal(fn);
}

void NamespaceDefImpl::distributeMemberGroupDocumentation()
{
  for (const auto &mg : m_memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void NamespaceDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
  docFindSections(inbodyDocumentation(),this,docFile());
  for (const auto &mg : m_memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDeclaration())
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}

void NamespaceDefImpl::insertUsedFile(FileDef *fd)
{
  if (fd==nullptr) return;
  auto it = std::find(files.begin(),files.end(),fd);
  if (it==files.end())
  {
    files.push_back(fd);
  }
}

void NamespaceDefImpl::addInnerCompound(Definition *d)
{
  //printf("%s:NamespaceDefImpl::addInnerCompound(%s)\n",qPrint(name()),qPrint(d->name()));
  m_innerCompounds.add(d->localName(),d);
  if (d->definitionType()==Definition::TypeNamespace)
  {
    insertNamespace(toNamespaceDef(d));
  }
  else if (d->definitionType()==Definition::TypeClass)
  {
    insertClass(toClassDef(d));
  }
  else if (d->definitionType()==Definition::TypeConcept)
  {
    insertConcept(toConceptDef(d));
  }
}

void NamespaceDefImpl::insertClass(ClassDef *cd)
{
  ClassLinkedRefMap *d = &m_classes;

  if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
  {
    if (cd->compoundType()==ClassDef::Interface)
    {
      d = &m_interfaces;
    }
    else if (cd->compoundType()==ClassDef::Struct)
    {
      d = &m_structs;
    }
    else if (cd->compoundType()==ClassDef::Exception)
    {
      d = &m_exceptions;
    }
  }

  d->add(cd->name(),cd);
}

void NamespaceDefImpl::insertConcept(ConceptDef *cd)
{
  m_concepts.add(cd->name(),cd);
}

void NamespaceDefImpl::insertNamespace(NamespaceDef *nd)
{
  m_namespaces.add(nd->name(),nd);
}


void NamespaceDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDeclaration())
    {
      ::addMembersToMemberGroup(ml.get(),&m_memberGroups,this);
    }
  }

  // add members inside sections to their groups
  for (const auto &mg : m_memberGroups)
  {
    if (mg->allMembersInSameSection() && m_subGrouping)
    {
      //printf("----> addToDeclarationSection(%s)\n",qPrint(mg->header()));
      mg->addToDeclarationSection();
    }
  }
}

void NamespaceDefImpl::insertMember(MemberDef *md)
{
  //printf("%s::insertMember(%s) isInline=%d hasDocs=%d\n",qPrint(name()),qPrint(md->name()),
  //    isInline(),hasDocumentation());
  if (md->isHidden()) return;
  MemberDefMutable *mdm = toMemberDefMutable(md);

  // if this is an inline namespace that is not documented, then insert the
  // member in the parent scope instead
  if (isInline() && !hasDocumentation())
  {
    Definition *outerScope = getOuterScope();
    if (outerScope)
    {
      if (outerScope->definitionType()==Definition::TypeNamespace)
      {
        NamespaceDefMutable *nd = toNamespaceDefMutable(outerScope);
        if (nd && nd!=Doxygen::globalScope)
        {
          nd->insertMember(md);
          if (mdm)
          {
            mdm->setNamespace(nd);
          }
        }
      }
      else if (outerScope->definitionType()==Definition::TypeFile)
      {
        FileDef *fd = toFileDef(outerScope);
        fd->insertMember(md);
        if (mdm)
        {
          mdm->setFileDef(fd);
          mdm->setOuterScope(fd);
        }
      }
    }
  }
  else // member is a non-inline namespace or a documented inline namespace
  {
    MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
    if (allMemberList==nullptr)
    {
      m_memberLists.emplace_back(std::make_unique<MemberList>(MemberListType::AllMembersList(),MemberListContainer::Namespace));
      allMemberList = m_memberLists.back().get();
    }
    allMemberList->push_back(md);
    //printf("%s::m_allMembersDict->append(%s)\n",qPrint(name()),qPrint(md->localName()));
    m_allMembers.add(md->localName(),md);
    switch(md->memberType())
    {
      case MemberType::Variable:
        addMemberToList(MemberListType::DecVarMembers(),md);
        addMemberToList(MemberListType::DocVarMembers(),md);
        break;
      case MemberType::Function:
        addMemberToList(MemberListType::DecFuncMembers(),md);
        addMemberToList(MemberListType::DocFuncMembers(),md);
        break;
      case MemberType::Typedef:
        addMemberToList(MemberListType::DecTypedefMembers(),md);
        addMemberToList(MemberListType::DocTypedefMembers(),md);
        break;
      case MemberType::Sequence:
        addMemberToList(MemberListType::DecSequenceMembers(),md);
        addMemberToList(MemberListType::DocSequenceMembers(),md);
        break;
      case MemberType::Dictionary:
        addMemberToList(MemberListType::DecDictionaryMembers(),md);
        addMemberToList(MemberListType::DocDictionaryMembers(),md);
        break;
      case MemberType::Enumeration:
        addMemberToList(MemberListType::DecEnumMembers(),md);
        addMemberToList(MemberListType::DocEnumMembers(),md);
        break;
      case MemberType::EnumValue:
        break;
      case MemberType::Define:
        addMemberToList(MemberListType::DecDefineMembers(),md);
        addMemberToList(MemberListType::DocDefineMembers(),md);
        break;
      case MemberType::Property:
        if (md->getLanguage() == SrcLangExt::Python)
        {
          addMemberToList(MemberListType::PropertyMembers(),md);
          addMemberToList(MemberListType::Properties(),md);
          break;
        }
        //  fallthrough, explicitly no break here
      default:
        err("NamespaceDefImpl::insertMembers(): "
            "member '{}' with unexpected type '{}' and class scope '{}' inserted in namespace scope '{}'!\n",
            md->name(), md->memberTypeName(), md->getClassDef() ? md->getClassDef()->name() : "", name());
    }
    // if this is an inline namespace, then insert an alias of this member in the outer scope.
    if (isInline())
    {
      Definition *outerScope = getOuterScope();
      if (outerScope)
      {
        std::unique_ptr<MemberDef> aliasMd = createMemberDefAlias(outerScope,md);
        if (outerScope->definitionType()==Definition::TypeNamespace)
        {
          NamespaceDefMutable *ndm = toNamespaceDefMutable(outerScope);
          if (ndm)
          {
            ndm->insertMember(aliasMd.get());
          }
        }
        else if (outerScope->definitionType()==Definition::TypeFile)
        {
          toFileDef(outerScope)->insertMember(aliasMd.get());
        }
        if (aliasMd)
        {
          MemberName *mn = Doxygen::functionNameLinkedMap->add(md->name());
          mn->push_back(std::move(aliasMd));
        }
      }
    }
  }
}

void NamespaceDefImpl::computeAnchors()
{
  MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
  if (allMemberList) allMemberList->setAnchors();
}

bool NamespaceDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return ((!briefDescription().empty() && repeatBrief) ||
          !documentation().empty());
}

void NamespaceDefImpl::writeTagFile(TextStream &tagFile)
{
  DString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "  <compound kind=\"namespace\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <filename>" << fn << "</filename>\n";
  DString idStr = id();
  if (!idStr.empty())
  {
    tagFile << "    <clangid>" << convertToXML(idStr) << "</clangid>\n";
  }
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::NamespaceNestedNamespaces:
        {
          for (const auto &nd : m_namespaces)
          {
            if (nd->isLinkableInProject())
            {
              tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::NamespaceClasses:
        {
          writeClassesToTagFile(tagFile, m_classes);
        }
        break;
      case LayoutDocEntry::NamespaceInterfaces:
        {
          writeClassesToTagFile(tagFile, m_interfaces);
        }
        break;
      case LayoutDocEntry::NamespaceStructs:
        {
          writeClassesToTagFile(tagFile, m_structs);
        }
        break;
      case LayoutDocEntry::NamespaceExceptions:
        {
          writeClassesToTagFile(tagFile, m_exceptions);
        }
        break;
      case LayoutDocEntry::NamespaceConcepts:
        {
          writeConceptsToTagFile(tagFile);
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
          for (const auto &mg : m_memberGroups)
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

void NamespaceDefImpl::writeDetailedDescription(OutputList &ol,const DString &title)
{
  if (hasDetailedDescription())
  {
    ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.writeRuler();
    ol.popGeneratorState();
    ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.writeAnchor(DString(),"details");
    ol.popGeneratorState();
    ol.startGroupHeader("details");
    ol.parseText(title);
    ol.endGroupHeader();

    ol.startTextBlock();
    if (!briefDescription().empty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),
                     briefLine(),
                     this,
                     nullptr,
                     briefDescription(),
                     DocOptions());
    }
    if (!briefDescription().empty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().empty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputType::Man);
        ol.disable(OutputType::RTF);
        ol.enableAll();
        ol.disableAllBut(OutputType::Man);
        ol.enable(OutputType::Latex);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().empty())
    {
      ol.generateDoc(docFile(),
                     docLine(),
                     this,
                     nullptr,
                     documentation()+"\n",
                     DocOptions()
                     .setIndexWords(true));
    }
    if (hasRequirementRefs()) writeRequirementRefs(ol);
    ol.endTextBlock();
  }
}

void NamespaceDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     briefFile(),
                                     briefLine(),
                                     this,
                                     nullptr,
                                     briefDescription(),
                                     DocOptions()
                                     .setIndexWords(true)
                                     .setSingleLine(true))
                 };
    if (!ast->empty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(ast.get(),this,nullptr);
      ol.pushGeneratorState();
      ol.disable(OutputType::RTF);
      ol.writeString(" \n");
      ol.enable(OutputType::RTF);

      if (hasDetailedDescription())
      {
        ol.disableAllBut(OutputType::Html);
        ol.startTextLink(getOutputFileBase(),"details");
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
      }
      ol.popGeneratorState();
      ol.endParagraph();
    }
  }

  // Write a summary of the Slice definition including metadata.
  if (getLanguage() == SrcLangExt::Slice)
  {
    ol.startParagraph();
    ol.startTypewriter();
    if (!metaData.empty())
    {
      ol.docify(metaData);
      ol.lineBreak();
    }
    ol.docify("module ");
    ol.docify(stripScope(name()));
    ol.docify(" { ... }");
    ol.endTypewriter();
    ol.endParagraph();
  }

  ol.writeSynopsis();
}

void NamespaceDefImpl::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void NamespaceDefImpl::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void NamespaceDefImpl::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.disable(OutputType::Html);
    Doxygen::suppressDocWarnings = true;
  }
}

void NamespaceDefImpl::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputType::Html);
    Doxygen::suppressDocWarnings = false;
  }
}

void NamespaceDefImpl::writeClassDeclarations(OutputList &ol,const DString &title,const ClassLinkedRefMap &d)
{
  d.writeDeclaration(ol,nullptr,title,true);
}

void NamespaceDefImpl::writeConcepts(OutputList &ol,const DString &title)
{
  m_concepts.writeDeclaration(ol,title,true);
}

void NamespaceDefImpl::writeInlineClasses(OutputList &ol)
{
  m_classes.writeDocumentation(ol,this);
}

void NamespaceDefImpl::writeNamespaceDeclarations(OutputList &ol,const DString &title,
            bool const isConstantGroup)
{
  m_namespaces.writeDeclaration(ol,title,isConstantGroup,true);
}

void NamespaceDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  for (const auto &mg : m_memberGroups)
  {
    if (!mg->allMembersInSameSection() || !m_subGrouping)
    {
      mg->writeDeclarations(ol,nullptr,this,nullptr,nullptr,nullptr);
    }
  }
}

void NamespaceDefImpl::writeAuthorSection(OutputList &ol)
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(true,true));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void NamespaceDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=true;
  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    if (lde->kind()==LayoutDocEntry::NamespaceClasses && m_classes.declVisible() && ls)
    {
      DString label = "nested-classes";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceInterfaces && m_interfaces.declVisible() && ls)
    {
      DString label = "interfaces";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceStructs && m_structs.declVisible() && ls)
    {
      DString label = "structs";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceExceptions && m_exceptions.declVisible() && ls)
    {
      DString label = "exceptions";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceNestedNamespaces && m_namespaces.declVisible(false) && ls)
    {
      DString label = "namespaces";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceNestedConstantGroups && m_namespaces.declVisible(true) && ls)
    {
      DString label = "constantgroups";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()==LayoutDocEntry::NamespaceConcepts && m_concepts.declVisible() && ls)
    {
      DString label = "concepts";
      ol.writeSummaryLink(DString(),label,ls->title(lang),first);
      first=false;
    }
    else if (lde->kind()== LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd)
      {
        MemberList * ml = getMemberList(lmd->type);
        if (ml && ml->declVisible())
        {
          ol.writeSummaryLink(DString(),ml->listType().toLabel(),lmd->title(lang),first);
          first=false;
        }
      }
    }
  }
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void NamespaceDefImpl::writePageNavigation(OutputList &ol) const
{
  ol.writePageOutline();
}

void NamespaceDefImpl::addNamespaceAttributes(OutputList &ol)
{
  // UNO IDL constant groups may be published
  if (getLanguage()==SrcLangExt::IDL && isConstantGroup() && m_isPublished)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.startLabels();
    ol.writeLabel("published",false);
    ol.endLabels();
    ol.popGeneratorState();
  }
  else if (isExported())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.startLabels();
    ol.writeLabel("export",false);
    ol.endLabels();
    ol.popGeneratorState();
  }
}

void NamespaceDefImpl::writeClassesToTagFile(TextStream &tagFile,const ClassLinkedRefMap &list)
{
  for (const auto &cd : list)
  {
    if (cd->isLinkableInProject())
    {
      tagFile << "    <class kind=\"" << cd->compoundTypeString()
              << "\">" << convertToXML(cd->name()) << "</class>\n";
    }
  }
}

void NamespaceDefImpl::writeConceptsToTagFile(TextStream &tagFile)
{
  for (const auto &cd : m_concepts)
  {
    if (cd->isLinkableInProject())
    {
      tagFile << "    <concept>" << convertToXML(cd->name()) << "</concept>\n";
    }
  }
}

void NamespaceDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  //bool outputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);

  DString pageTitle = title();
  startFile(ol,getOutputFileBase(),false,name(),pageTitle,HighlightedItem::NamespaceVisible,!generateTreeView);

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
  addGroupListToTitle(ol,this);
  addNamespaceAttributes(ol);
  endTitle(ol,getOutputFileBase(),displayName());
  ol.startContents();

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol);
        break;
      case LayoutDocEntry::MemberDeclStart:
        startMemberDeclarations(ol);
        break;
      case LayoutDocEntry::NamespaceClasses:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),m_classes);
        }
        break;
      case LayoutDocEntry::NamespaceInterfaces:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),m_interfaces);
        }
        break;
      case LayoutDocEntry::NamespaceStructs:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),m_structs);
        }
        break;
      case LayoutDocEntry::NamespaceExceptions:
        {
          if (ls) writeClassDeclarations(ol,ls->title(lang),m_exceptions);
        }
        break;
      case LayoutDocEntry::NamespaceConcepts:
        {
          if (ls) writeConcepts(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::NamespaceNestedNamespaces:
        {
          if (ls) writeNamespaceDeclarations(ol,ls->title(lang),false);
        }
        break;
      case LayoutDocEntry::NamespaceNestedConstantGroups:
        {
          if (ls) writeNamespaceDeclarations(ol,ls->title(lang),true);
        }
        break;
      case LayoutDocEntry::MemberGroups:
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd) writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          if (ls) writeDetailedDescription(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::NamespaceInlineClasses:
        writeInlineClasses(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
          if (lmd) writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefEnd:
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::AuthorSection:
        writeAuthorSection(ol);
        break;
      case LayoutDocEntry::ClassIncludes:
      case LayoutDocEntry::ClassInheritanceGraph:
      case LayoutDocEntry::ClassNestedClasses:
      case LayoutDocEntry::ClassCollaborationGraph:
      case LayoutDocEntry::ClassAllMembersLink:
      case LayoutDocEntry::ClassUsedFiles:
      case LayoutDocEntry::ClassInlineClasses:
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
        err("Internal inconsistency: member '{}' should not be part of "
            "LayoutDocManager::Namespace entry list\n",lde->entryToString());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(ol,this);

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
    if (allMemberList) allMemberList->sort();
    writeMemberPages(ol);
  }
}

void NamespaceDefImpl::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  for (const auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
      ml->writeDocumentationPage(ol,displayName(),this);
    }
  }
  ol.popGeneratorState();
}

void NamespaceDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
  if (allMemberList)
  {
    for (const auto &md : *allMemberList)
    {
      if (md->getNamespaceDef()==this && md->isLinkable() && !md->isEnumValue())
      {
        if (md->isLinkableInProject())
        {
          DString fn = md->getOutputFileBase();
          addHtmlExtensionIfMissing(fn);
          if (md==currentMd) // selected item => highlight
          {
            ol.writeString("          <tr><td class=\"navtabHL\">");
          }
          else
          {
            ol.writeString("          <tr><td class=\"navtab\">");
          }
          ol.writeString("<span class=\"label\"><a ");
          ol.writeString("href=\"");
          if (createSubDirs) ol.writeString("../../");
          ol.writeString(fn+"#"+md->anchor());
          ol.writeString("\">");
          ol.writeString(convertToHtml(md->localName()));
          ol.writeString("</a></span>");
          ol.writeString("</td></tr>\n");
        }
      }
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}

void NamespaceDefImpl::countMembers()
{
  for (auto &ml : m_memberLists)
  {
    ml->countDecMembers();
    ml->countDocMembers();
  }
  for (const auto &mg : m_memberGroups)
  {
    mg->countDecMembers();
    mg->countDocMembers();
  }
}

int NamespaceDefImpl::numDocMembers() const
{
  MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
  return (allMemberList ? allMemberList->numDocMembers() : 0) + static_cast<int>(m_innerCompounds.size());
}

void NamespaceDefImpl::addUsingDirective(NamespaceDef *nd)
{
  m_usingDirList.add(nd->qualifiedName(),nd);
  //printf("%s: NamespaceDefImpl::addUsingDirective: %s:%zu\n",qPrint(name()),qPrint(nd->qualifiedName()),m_usingDirList.size());
}

void NamespaceDefImpl::addUsingDeclaration(const Definition *d)
{
  m_usingDeclList.add(d->qualifiedName(),d);
}

DString NamespaceDefImpl::getOutputFileBase() const
{
  return fileName;
}

const Definition *NamespaceDefImpl::findInnerCompound(const DString &n) const
{
  if (n==nullptr) return nullptr;
  const Definition *d = m_innerCompounds.find(n);
  if (d==nullptr)
  {
    if (!m_usingDirList.empty())
    {
      d = m_usingDirList.find(n);
    }
    if (d==nullptr && !m_usingDeclList.empty())
    {
      d = m_usingDeclList.find(n);
    }
  }
  return d;
}

void NamespaceDefImpl::addListReferences()
{
  addRefItem(xrefListItems(),
             qualifiedName(),
             getLanguage()==SrcLangExt::Fortran ?
               theTranslator->trModule(true,true) :
               theTranslator->trNamespace(true,true),
             getOutputFileBase(),displayName(),
             DString(),
             this
            );
  for (const auto &mg : m_memberGroups)
  {
    mg->addListReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
      ml->addListReferences(this);
    }
  }
}

void NamespaceDefImpl::addRequirementReferences()
{
  RequirementManager::instance().addRequirementRefsForSymbol(this);
  for (const auto &mg : m_memberGroups)
  {
    mg->addRequirementReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
      ml->addRequirementReferences(this);
    }
  }
}

DString NamespaceDefImpl::displayName(bool includeScope) const
{
  return makeDisplayName(this,includeScope);
}

void NamespaceDefImpl::combineUsingRelations(NamespaceDefSet &visitedNamespaces)
{
  if (visitedNamespaces.find(this)!=visitedNamespaces.end()) return; // already processed
  visitedNamespaces.insert(this);

  LinkedRefMap<NamespaceDef> usingDirList = m_usingDirList;
  for (auto &nd : usingDirList)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd);
    if (ndm)
    {
      ndm->combineUsingRelations(visitedNamespaces);
    }
  }

  for (auto &nd : usingDirList)
  {
    // add used namespaces of namespace nd to this namespace
    for (const auto &und : nd->getUsedNamespaces())
    {
      addUsingDirective(und);
    }
    // add used classes of namespace nd to this namespace
    for (const auto &ud : nd->getUsedDefinitions())
    {
      addUsingDeclaration(ud);
    }
  }
}

int NamespaceDefImpl::countVisibleMembers() const
{
  int count=0;
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    if (lde->kind()==LayoutDocEntry::MemberDef)
    {
      const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
      if (lmd)
      {
        MemberList *ml = getMemberList(lmd->type);
        if (ml)
        {
          for (const auto &md : *ml)
          {
            if (md->visibleInIndex())
            {
              count++;
            }
          }
        }
      }
    }
  }
  return count;
}



//-------------------------------------------------------------------------------

bool NamespaceLinkedRefMap::declVisible(bool isConstantGroup) const
{
  bool found=false;
  for (const auto &nd : *this)
  {
    if (nd->isLinkable() && nd->hasDocumentation())
    {
      SrcLangExt lang = nd->getLanguage();
      if (SrcLangExt::IDL==lang)
      {
        if (isConstantGroup == nd->isConstantGroup())
        {
          found=true;
          break;
        }
      }
      else if (!isConstantGroup) // ensure we only get extra section in IDL
      {
        if (nd->isConstantGroup())
        {
          err("Internal inconsistency: constant group but not IDL?\n");
        }
        found=true;
        break;
      }
    }
  }
  return found;
}

void NamespaceLinkedRefMap::writeDeclaration(OutputList &ol,const DString &title,
        bool const isConstantGroup,bool localName)
{


  if (empty()) return; // no namespaces in the list

  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL)) return;

  if (!declVisible(isConstantGroup)) return;

  // write list of namespaces
  ol.startMemberHeader(isConstantGroup ? "constantgroups" : "namespaces");
  //bool javaOpt    = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  ol.parseText(title);
  ol.endMemberHeader();
  ol.startMemberList();
  for (const auto &nd : *this)
  {
    if (nd->isLinkable() && nd->hasDocumentation())
    {
      SrcLangExt lang = nd->getLanguage();
      if (lang==SrcLangExt::IDL && (isConstantGroup != nd->isConstantGroup()))
          continue; // will be output in another pass, see layout_default.xml
      ol.startMemberDeclaration();
      DString name = localName ? nd->localName() : nd->displayName();
      DString anc = nd->anchor();
      if (anc.empty()) anc=name; else anc.prepend(name+"_");
      ol.startMemberItem(anc,OutputGenerator::MemberItemType::Normal);
      DString ct = nd->compoundTypeString();
      ol.docify(ct);
      ol.docify(" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),DString(),name);
      ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
      if (!nd->briefDescription().empty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(nd->getOutputFileBase());
        ol.generateDoc(nd->briefFile(),
                       nd->briefLine(),
                       nd,
                       nullptr,
                       nd->briefDescription(),
                       DocOptions()
                       .setSingleLine(true));
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(DString(),DString());
    }
  }
  ol.endMemberList();
}

//-------------------------------------------------------------------------------

void NamespaceDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_memberLists.get(lt,MemberListContainer::Namespace);
  ml->setNeedsSorting(
      (ml->listType().isDeclaration() && sortBriefDocs) ||
      (ml->listType().isDocumentation() && sortMemberDocs));
  ml->push_back(md);

  if (ml->listType().isDeclaration())
  {
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm)
    {
      mdm->setSectionList(this,ml.get());
    }
  }
}

template<class Container>
bool needsSorting(const Container &container)
{
  DString prev;
  bool allSame = true;
  for (const auto &c : container)
  {
    DString name = c->getDefFileName();
    if (!prev.empty() && prev!=name)
    {
      allSame = false;
      break;
    }
    prev = name;
  }
  return !allSame || Config_getBool(SORT_BRIEF_DOCS);
}

void NamespaceDefImpl::sortMemberLists()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(false); }
  }

  auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
  {
    return Config_getBool(SORT_BY_SCOPE_NAME)          ?
      dstricmp_sort(c1->name(), c2->name())<0          :
      dstricmp_sort(c1->className(), c2->className())<0;
  };

  if (needsSorting(m_classes))    std::stable_sort(m_classes.begin(),   m_classes.end(),   classComp);
  if (needsSorting(m_interfaces)) std::stable_sort(m_interfaces.begin(),m_interfaces.end(),classComp);
  if (needsSorting(m_structs))    std::stable_sort(m_structs.begin(),   m_structs.end(),   classComp);
  if (needsSorting(m_exceptions)) std::stable_sort(m_exceptions.begin(),m_exceptions.end(),classComp);

  auto conceptComp = [](const ConceptLinkedRefMap::Ptr &c1,const ConceptLinkedRefMap::Ptr &c2)
  {
    return dstricmp_sort(c1->name(),c2->name())<0;
  };

  if (needsSorting(m_concepts)) std::stable_sort(m_concepts.begin(), m_concepts.end(), conceptComp);

  auto namespaceComp = [](const NamespaceLinkedRefMap::Ptr &n1,const NamespaceLinkedRefMap::Ptr &n2)
  {
    return dstricmp_sort(n1->name(),n2->name())<0;
  };

  if (needsSorting(m_namespaces)) std::stable_sort(m_namespaces.begin(),m_namespaces.end(),namespaceComp);

}

MemberList *NamespaceDefImpl::getMemberList(MemberListType lt) const
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()==lt)
    {
      return ml.get();
    }
  }
  return nullptr;
}

void NamespaceDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const DString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,nullptr,this,nullptr,nullptr,nullptr,title,DString());
}

void NamespaceDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const DString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,displayName(),this,title,ml->listType().toLabel());
}

static bool hasNonReferenceNestedNamespaceRec(const NamespaceDef *nd,int level)
{
  if (level>30)
  {
    err("Possible recursive namespace relation while inside {}\n",nd->name());
    return false;
  }
  bool found=nd->isLinkableInProject();
  if (found)
  {
    return true;
  }
  else
  {
    for (const auto &ind : nd->getNamespaces())
    {
      found = found || hasNonReferenceNestedNamespaceRec(ind,level+1);
      if (found) break;
    }
  }
  return found;
}

bool NamespaceDefImpl::isVisibleInHierarchy() const
{
  bool allExternals = Config_getBool(ALLEXTERNALS);
  return allExternals || hasNonReferenceNestedNamespaceRec(this,0) || isLinkable();
}

bool NamespaceDefImpl::isLinkableInProject() const
{
  size_t i = name().rfind("::");
  if (i==DString::npos) i=0; else i+=2;
  bool extractAnonNs = Config_getBool(EXTRACT_ANON_NSPACES);
  bool hideUndoc     = Config_getBool(HIDE_UNDOC_NAMESPACES);
  if (extractAnonNs &&                             // extract anonymous ns
      name().mid(i,20)=="anonymous_namespace{"     // correct prefix
     )                                             // not disabled by config
  {
    return true;
  }
  return !name().empty() && name().at(i)!='@' && // not anonymous
    (hasDocumentation() || !hideUndoc || getLanguage()==SrcLangExt::CSharp) &&  // documented
    !isReference() &&      // not an external reference
    !isHidden() &&         // not hidden
    !isArtificial();       // or artificial
}

bool NamespaceDefImpl::isLinkable() const
{
  return isLinkableInProject() || isReference();
}

const MemberDef * NamespaceDefImpl::getMemberByName(const DString &n) const
{
  return m_allMembers.find(n);
}

DString NamespaceDefImpl::title() const
{
  DString pageTitle;
  SrcLangExt lang = getLanguage();

  auto getReferenceTitle = [this](std::function<DString()> translateFunc) -> DString
  {
    return Config_getBool(HIDE_COMPOUND_REFERENCE) ? displayName() : translateFunc();
  };

  if (lang==SrcLangExt::Java)
  {
    pageTitle = theTranslator->trPackage(displayName());
  }
  else if (lang==SrcLangExt::Fortran || lang==SrcLangExt::Slice)
  {
    pageTitle = getReferenceTitle([this](){
        return theTranslator->trModuleReference(displayName());
    });
  }
  else if (lang==SrcLangExt::IDL)
  {
    pageTitle = getReferenceTitle([this](){
        return isConstantGroup()
        ? theTranslator->trConstantGroupReference(displayName())
        : theTranslator->trModuleReference(displayName());
    });
  }
  else
  {
    pageTitle = getReferenceTitle([this](){
      return theTranslator->trNamespaceReference(displayName());
    });
  }
  return pageTitle;
}

DString NamespaceDefImpl::compoundTypeString() const
{
  SrcLangExt lang = getLanguage();
  if (lang==SrcLangExt::Java)
  {
    return "package";
  }
  else if(lang==SrcLangExt::CSharp)
  {
    return "namespace";
  }
  else if (lang==SrcLangExt::Fortran)
  {
    return "module";
  }
  else if (lang==SrcLangExt::IDL)
  {
    if (isModule())
    {
      return "module";
    }
    else if (isConstantGroup())
    {
      return "constants";
    }
    else if (isLibrary())
    {
      return "library";
    }
    else
    {
      err_full(getDefFileName(),getDefLine(),"Internal inconsistency: namespace in IDL not module, library or constant group");
    }
  }
  return "namespace";
}

void NamespaceDefImpl::setMetaData(const DString &m)
{
  metaData = m;
}

// --- Cast functions
//
NamespaceDef *toNamespaceDef(Definition *d)
{
  if (d && (typeid(*d)==typeid(NamespaceDefImpl) || typeid(*d)==typeid(NamespaceDefAliasImpl)))
  {
    return static_cast<NamespaceDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

NamespaceDef *toNamespaceDef(DefinitionMutable *md)
{
  Definition *d = toDefinition(md);
  if (d && typeid(*d)==typeid(NamespaceDefImpl))
  {
    return static_cast<NamespaceDef*>(d);
  }
  else
  {
    return nullptr;
  }
}


const NamespaceDef *toNamespaceDef(const Definition *d)
{
  if (d && (typeid(*d)==typeid(NamespaceDefImpl) || typeid(*d)==typeid(NamespaceDefAliasImpl)))
  {
    return static_cast<const NamespaceDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

NamespaceDefMutable *toNamespaceDefMutable(Definition *d)
{
  if (d && typeid(*d)==typeid(NamespaceDefImpl))
  {
    return static_cast<NamespaceDefMutable*>(d);
  }
  else
  {
    return nullptr;
  }
}

// --- Helpers

static NamespaceDef *getResolvedNamespaceRec(StringSet &namespacesTried,const NamespaceAliasInfo &aliasInfo);

static DString replaceNamespaceAliasesRec(StringSet &namespacesTried,const DString &name)
{
  DString result = name;
  //printf("> replaceNamespaceAliasesRec(%s)\n",qPrint(name));
  if (namespacesTried.find(name.str())==namespacesTried.end())
  {
    namespacesTried.insert(name.str());
    size_t p = 0;
    for (;;)
    {
      size_t i = name.str().find("::",p);
      if (i==std::string::npos)
      {
        auto it = Doxygen::namespaceAliasMap.find(name.str());
        if (it != Doxygen::namespaceAliasMap.end())
        {
          //printf("found map %s->%s\n",qPrint(name),qPrint(it->second.alias));
          auto ns = getResolvedNamespaceRec(namespacesTried,it->second);
          if (ns)
          {
            result = replaceNamespaceAliasesRec(namespacesTried,ns->qualifiedName());
          }
        }
        break;
      }
      else
      {
        auto it = Doxygen::namespaceAliasMap.find(name.left(i).str());
        if (it != Doxygen::namespaceAliasMap.end())
        {
          //printf("found map %s|%s->%s\n",qPrint(name.left(i)),qPrint(name.mid(i)),qPrint(it->second.alias));
          auto ns = getResolvedNamespaceRec(namespacesTried,it->second);
          if (ns)
          {
            result = replaceNamespaceAliasesRec(namespacesTried,ns->qualifiedName()+name.mid(i));
            break;
          }
        }
      }
      p = i+2;
    }
  }
  //printf("< replaceNamespaceAliasesRec(%s)=%s\n",qPrint(name),qPrint(result));
  return result;
}

static NamespaceDef *getResolvedNamespaceRec(StringSet &namespacesTried,const NamespaceAliasInfo &aliasInfo)
{
  size_t j = aliasInfo.context.length();
  for (;;)
  {
    if (j>0)
    {
      //printf("candidate %s|::%s\n",qPrint(aliasInfo.context.substr(0,j)),qPrint(aliasInfo.alias));
      auto candidate = replaceNamespaceAliasesRec(namespacesTried,aliasInfo.context.substr(0,j)+"::"+aliasInfo.alias);
      auto nd = Doxygen::namespaceLinkedMap->find(candidate);
      if (nd)
      {
        return nd;
      }
    }
    if (j>0) // strip one level from context, i.e. given N1::N2::N3
             // j==10 -> j==6 (N1::N2::N3->N1::N2), and then
             // j==6  -> j==2 (N1::N2->N1), and then
             // j==2  -> j==std::string::npos (N1->"")
    {
      j = aliasInfo.context.rfind("::",j-1);
    }
    else
    {
      j = std::string::npos;
    }
    if (j==std::string::npos)
    {
      //printf("candidate %s\n",qPrint(aliasInfo.alias));
      auto candidate = replaceNamespaceAliasesRec(namespacesTried,aliasInfo.alias);
      auto nd = Doxygen::namespaceLinkedMap->find(candidate);
      if (nd)
      {
        return nd;
      }
      break;
    }
  }
  return nullptr;
}

void replaceNamespaceAliases(DString &name)
{
  //printf("> replaceNamespaceAliases(%s)\n",qPrint(name));
  StringSet namespacesTried;
  name = replaceNamespaceAliasesRec(namespacesTried,name);
  //printf("< replaceNamespaceAliases: result=%s\n",qPrint(name));
}

NamespaceDef *getResolvedNamespace(const DString &name)
{
  //printf("> getResolvedNamespace(%s)\n",qPrint(name));
  if (name.empty()) return nullptr;
  StringSet namespacesTried;
  auto ns = getResolvedNamespaceRec(namespacesTried,NamespaceAliasInfo(name.str()));
  //printf("< getResolvedNamespace(%s)=%s\n",qPrint(name),ns?qPrint(ns->qualifiedName()):"nullptr");
  return ns;
}

//--------------------------------------------------------------------------------------
//
bool namespaceHasNestedNamespace(const NamespaceDef *nd)
{
  for (const auto &cnd : nd->getNamespaces())
  {
    if (cnd->isLinkableInProject() && !cnd->isAnonymous())
    {
      return true;
    }
  }
  return false;
}

bool namespaceHasNestedConcept(const NamespaceDef *nd)
{
  //printf(">namespaceHasNestedConcept(%s)\n",qPrint(nd->name()));
  for (const auto &cnd : nd->getNamespaces())
  {
    if (namespaceHasNestedConcept(cnd))
    {
      //printf("<namespaceHasNestedConcept(%s): case1\n",qPrint(nd->name()));
      return true;
    }
  }
  for (const auto &cnd : nd->getConcepts())
  {
    //printf("candidate %s isLinkableInProject()=%d\n",qPrint(cnd->name()),cnd->isLinkableInProject());
    if (cnd->isLinkableInProject())
    {
      //printf("<namespaceHasNestedConcept(%s): case2\n",qPrint(nd->name()));
      return true;
    }
  }
  //printf("<namespaceHasNestedConcept(%s): case3\n",qPrint(nd->name()));
  return false;
}

bool namespaceHasNestedClass(const NamespaceDef *nd,bool filterClasses,ClassDef::CompoundType ct)
{
  //printf(">namespaceHasNestedClass(%s,filterClasses=%d)\n",qPrint(nd->name()),filterClasses);
  for (const auto &cnd : nd->getNamespaces())
  {
    if (namespaceHasNestedClass(cnd,filterClasses,ct))
    {
      //printf("<namespaceHasNestedClass(%s,filterClasses=%d): case1\n",qPrint(nd->name()),filterClasses);
      return true;
    }
  }

  ClassLinkedRefMap list = nd->getClasses();
  if (filterClasses)
  {
    switch (ct)
    {
      case ClassDef::Interface:
        list = nd->getInterfaces();
        break;
      case ClassDef::Struct:
        list = nd->getStructs();
        break;
      case ClassDef::Exception:
        list = nd->getExceptions();
        break;
      default:
        break;
    }
  }

  for (const auto &cd : list)
  {
    //printf("candidate %s isLinkableInProject()=%d\n",qPrint(cd->name()),cd->isLinkableInProject());
    if (cd->isLinkableInProject())
    {
      //printf("<namespaceHasNestedClass(%s,filterClasses=%d): case2\n",qPrint(nd->name()),filterClasses);
      return true;
    }
  }
  //printf("<namespaceHasNestedClass(%s,filterClasses=%d): case3\n",qPrint(nd->name()),filterClasses);
  return false;
}


