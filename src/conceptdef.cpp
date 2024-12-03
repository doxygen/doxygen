/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include "conceptdef.h"
#include "definitionimpl.h"
#include "util.h"
#include "config.h"
#include "doxygen.h"
#include "language.h"
#include "outputlist.h"
#include "searchindex.h"
#include "message.h"
#include "parserintf.h"
#include "layout.h"
#include "namespacedef.h"

//------------------------------------------------------------------------------------

class ConceptDefImpl : public DefinitionMixin<ConceptDefMutable>
{
  public:
    ConceptDefImpl(const QCString &fileName,int startLine,int startColumn,
                   const QCString &name,const QCString &tagRef=QCString(),const QCString &tagFile=QCString());
   ~ConceptDefImpl() override;
    NON_COPYABLE(ConceptDefImpl)

    //---------- ConceptDef
    DefType definitionType() const override;
    CodeSymbolType codeSymbolType() const override { return CodeSymbolType::Concept; }
    QCString getOutputFileBase() const override;
    bool hasDetailedDescription() const override;
    QCString displayName(bool includeScope=true) const override;
    const IncludeInfo *includeInfo() const override;
    ArgumentList getTemplateParameterList() const override;
    QCString anchor() const override;
    bool isLinkableInProject() const override;
    bool isLinkable() const override;
    QCString initializer() const override;
    void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const override;
    const NamespaceDef *getNamespaceDef() const override;
    const FileDef *getFileDef() const override;
    const ModuleDef *getModuleDef() const override;
    QCString title() const override;
    int groupId() const override;

    //---------- ConceptDefMutable
    void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force) override;
    void setTemplateArguments(const ArgumentList &al) override;
    void setNamespace(NamespaceDef *nd) override;
    void setFileDef(FileDef *fd) override;
    void setModuleDef(ModuleDef *mod) override;
    void writeTagFile(TextStream &) override;
    void writeDocumentation(OutputList &) override;
    void setInitializer(const QCString &init) override;
    void findSectionsInDocumentation() override;
    void setGroupId(int id) override;

    //---------- Helpers
    void writeBriefDescription(OutputList &) const;
    void writeAuthorSection(OutputList &ol) const;
    void writeIncludeFiles(OutputList &ol) const;
    void writeDetailedDescription(OutputList &ol,const QCString &title) const;
    void writeDefinition(OutputList &ol,const QCString &title) const;
    void addConceptAttributes(OutputList &ol) const;

  private:
    QCString                     m_fileName;
    std::unique_ptr<IncludeInfo> m_incInfo;
    NamespaceDef                *m_nspace = nullptr;
    FileDef                     *m_fileDef = nullptr;
    ModuleDef                   *m_moduleDef = nullptr;
    ArgumentList                 m_tArgList;
    QCString                     m_initializer;
    int                          m_groupId = -1;
};

std::unique_ptr<ConceptDef> createConceptDef(
             const QCString &fileName,int startLine,int startColumn,
             const QCString &name, const QCString &tagRef,const QCString &tagFile)
{
  return std::make_unique<ConceptDefImpl>(fileName,startLine,startColumn,name,tagRef,tagFile);
}

//------------------------------------------------------------------------------------

class ConceptDefAliasImpl : public DefinitionAliasMixin<ConceptDef>
{
  public:
    ConceptDefAliasImpl(const Definition *newScope,const ConceptDef *cd)
      : DefinitionAliasMixin(newScope,cd) { init(); }
    ~ConceptDefAliasImpl() override { deinit(); }
    NON_COPYABLE(ConceptDefAliasImpl)

    const ConceptDef *getCdAlias() const { return toConceptDef(getAlias()); }

    DefType definitionType() const override { return TypeConcept; }
    CodeSymbolType codeSymbolType() const override
    { return getCdAlias()->codeSymbolType(); }
    QCString getOutputFileBase() const override
    { return getCdAlias()->getOutputFileBase(); }
    QCString getReference() const override
    { return getCdAlias()->getReference(); }
    bool isReference() const override
    { return getCdAlias()->isReference(); }
    bool hasDetailedDescription() const override
    { return getCdAlias()->hasDetailedDescription(); }
    QCString displayName(bool includeScope=true) const override
    { return getCdAlias()->displayName(includeScope); }
    const IncludeInfo *includeInfo() const override
    { return getCdAlias()->includeInfo(); }
    ArgumentList getTemplateParameterList() const override
    { return getCdAlias()->getTemplateParameterList(); }
    QCString anchor() const override
    { return getCdAlias()->anchor(); }
    bool isLinkableInProject() const override
    { return getCdAlias()->isLinkableInProject(); }
    bool isLinkable() const override
    { return getCdAlias()->isLinkable(); }
    QCString initializer() const override
    { return getCdAlias()->initializer(); }
    const NamespaceDef *getNamespaceDef() const override
    { return getCdAlias()->getNamespaceDef(); }
    const FileDef *getFileDef() const override
    { return getCdAlias()->getFileDef(); }
    const ModuleDef *getModuleDef() const override
    { return getCdAlias()->getModuleDef(); }
    QCString title() const override
    { return getCdAlias()->title(); }
    void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const override
    { getCdAlias()->writeDeclarationLink(ol,found,header,localNames); }
    int groupId() const override
    { return getCdAlias()->groupId(); }
};

std::unique_ptr<ConceptDef> createConceptDefAlias(const Definition *newScope,const ConceptDef *cd)
{
  return std::make_unique<ConceptDefAliasImpl>(newScope,cd);
}

//------------------------------------------------------------------------------------

ConceptDefImpl::ConceptDefImpl(const QCString &fileName,int startLine,int startColumn,
    const QCString &name,const QCString &tagRef,const QCString &tagFile) : DefinitionMixin(fileName,startLine,startColumn,name)
{
  if (!tagFile.isEmpty())
  {
    if (!tagRef.isEmpty())
    {
      m_fileName = stripExtension(tagFile);
    }
    else
    {
      m_fileName = convertNameToFile(stripExtension(tagFile));
    }
  }
  else
  {
    m_fileName = convertNameToFile(QCString("concept")+name);
  }
  setReference(tagRef);
}

ConceptDefImpl::~ConceptDefImpl()
{
}

Definition::DefType ConceptDefImpl::definitionType() const
{
  return TypeConcept;
}

QCString ConceptDefImpl::getOutputFileBase() const
{
  return m_fileName;
}

bool ConceptDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().isEmpty() ||
          (sourceBrowser && getStartBodyLine()!=-1 && getBodyDef()));
}

QCString ConceptDefImpl::anchor() const
{
  return "";
}

QCString ConceptDefImpl::displayName(bool includeScope) const
{
  return includeScope ? name() : localName();
}

const IncludeInfo *ConceptDefImpl::includeInfo() const
{
  return m_incInfo.get();
}

ArgumentList ConceptDefImpl::getTemplateParameterList() const
{
  return m_tArgList;
}

bool ConceptDefImpl::isLinkableInProject() const
{
  bool hideUndoc = Config_getBool(HIDE_UNDOC_CLASSES);
  return (hasDocumentation() || !hideUndoc) && /* documented */
         !isHidden() &&                        /* not hidden */
         !isReference();                       /* not an external reference */
}

bool ConceptDefImpl::isLinkable() const
{
  return isLinkableInProject() || isReference();
}

void ConceptDefImpl::setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force)
{
  if (!m_incInfo) m_incInfo = std::make_unique<IncludeInfo>();
  if ((!incName.isEmpty() && m_incInfo->includeName.isEmpty()) ||
      (fd!=nullptr && m_incInfo->fileDef==nullptr)
     )
  {
    m_incInfo->fileDef     = fd;
    m_incInfo->includeName = incName;
    m_incInfo->kind        = local ? IncludeKind::IncludeLocal : IncludeKind::IncludeSystem;
  }
  if (force && !incName.isEmpty())
  {
    m_incInfo->includeName = incName;
    m_incInfo->kind        = local ? IncludeKind::IncludeLocal : IncludeKind::IncludeSystem;
  }
}

void ConceptDefImpl::setTemplateArguments(const ArgumentList &al)
{
  m_tArgList = al;
}

void ConceptDefImpl::setNamespace(NamespaceDef *nd)
{
  m_nspace = nd;
}

const NamespaceDef *ConceptDefImpl::getNamespaceDef() const
{
  return m_nspace;
}

void ConceptDefImpl::setFileDef(FileDef *fd)
{
  m_fileDef=fd;
}

void ConceptDefImpl::setModuleDef(ModuleDef *mod)
{
  m_moduleDef=mod;
}

const FileDef *ConceptDefImpl::getFileDef() const
{
  return m_fileDef;
}

const ModuleDef *ConceptDefImpl::getModuleDef() const
{
  return m_moduleDef;
}

QCString ConceptDefImpl::title() const
{
  return theTranslator->trConceptReference(displayName());
}

int ConceptDefImpl::groupId() const
{
  return m_groupId;
}

void ConceptDefImpl::setGroupId(int id)
{
  m_groupId = id;
}

void ConceptDefImpl::writeTagFile(TextStream &tagFile)
{
  tagFile << "  <compound kind=\"concept\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  QCString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "    <filename>" << convertToXML(fn) << "</filename>\n";
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "    <clangid>" << convertToXML(idStr) << "</clangid>\n";
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>\n";
}

void ConceptDefImpl::writeBriefDescription(OutputList &ol) const
{
  if (hasBriefDescription())
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(
                        *parser.get(),briefFile(),briefLine(),this,nullptr,
                        briefDescription(),TRUE,FALSE,
                        QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    if (!ast->isEmpty())
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
  ol.writeSynopsis();
}

void ConceptDefImpl::writeIncludeFiles(OutputList &ol) const
{
  if (m_incInfo)
  {
    QCString nm=m_incInfo->includeName.isEmpty() ?
      (m_incInfo->fileDef ?
       m_incInfo->fileDef->docName() : QCString()
      ) :
      m_incInfo->includeName;
    if (!nm.isEmpty())
    {
      ol.startParagraph();
      ol.startTypewriter();
      ol.docify(::includeStatement(SrcLangExt::Cpp,m_incInfo->kind));
      ol.docify(::includeOpen(SrcLangExt::Cpp,m_incInfo->kind));
      ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.docify(nm);
      ol.disableAllBut(OutputType::Html);
      ol.enable(OutputType::Html);
      if (m_incInfo->fileDef)
      {
        ol.writeObjectLink(QCString(),m_incInfo->fileDef->includeName(),QCString(),nm);
      }
      else
      {
        ol.docify(nm);
      }
      ol.popGeneratorState();
      ol.docify(::includeClose(SrcLangExt::Cpp,m_incInfo->kind));
      ol.endTypewriter();
      ol.endParagraph();
    }
  }
}

static QCString templateSpec(const ArgumentList &al)
{
  TextStream t;
  t << "template<";
  bool first=true;
  for (const auto &a : al)
  {
    if (!first) t << ", ";
    t << a.type;
    if (!a.name.isEmpty())
    {
      t << " " << a.name;
    }
    if (a.defval.length()!=0)
    {
      t << " " << a.defval;
    }
    first=false;
  }
  t << ">";
  return t.str();
}

void ConceptDefImpl::setInitializer(const QCString &init)
{
  m_initializer = templateSpec(m_tArgList)+"\nconcept "+name()+" = "+init;
}

QCString ConceptDefImpl::initializer() const
{
  return m_initializer;
}

void ConceptDefImpl::writeDefinition(OutputList &ol,const QCString &title) const
{
    ol.startGroupHeader();
      ol.parseText(title);
    ol.endGroupHeader();

    auto intf = Doxygen::parserManager->getCodeParser(".cpp");
    intf->resetCodeParserState();
    auto &codeOL = ol.codeGenerators();
    codeOL.startCodeFragment("DoxyCode");
    QCString scopeName;
    if (getOuterScope()!=Doxygen::globalScope) scopeName=getOuterScope()->name();
    TextStream conceptDef;
    conceptDef << m_initializer;
    intf->parseCode(codeOL,scopeName,conceptDef.str(),SrcLangExt::Cpp,Config_getBool(STRIP_CODE_COMMENTS),false,QCString(),
                    m_fileDef, -1,-1,true,nullptr,false,this);
    codeOL.endCodeFragment("DoxyCode");
}

void ConceptDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title) const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  if (hasDetailedDescription())
  {
    ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.writeRuler();
    ol.popGeneratorState();

    ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.writeAnchor(QCString(),"details");
    ol.popGeneratorState();

    ol.startGroupHeader();
      ol.parseText(title);
    ol.endGroupHeader();

    ol.startTextBlock();
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

    writeSourceDef(ol);
    ol.endTextBlock();
  }
}

void ConceptDefImpl::writeAuthorSection(OutputList &ol) const
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void ConceptDefImpl::addConceptAttributes(OutputList &ol) const
{
  if (isExported())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.startLabels();
    ol.writeLabel("export",false);
    ol.endLabels();
    ol.popGeneratorState();
  }
}

void ConceptDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString pageTitle = theTranslator->trConceptReference(displayName());
  startFile(ol,getOutputFileBase(),name(),pageTitle,HighlightedItem::ConceptVisible,!generateTreeView);

  // ---- navigation part
  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope)
    {
      writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }

  // ---- title part
  startTitle(ol,getOutputFileBase(),this);
  ol.parseText(pageTitle);
  addConceptAttributes(ol);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),displayName());

  // ---- contents part

  ol.startContents();

  //---------------------------------------- start flexible part -------------------------------

  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Concept))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol);
        break;
      case LayoutDocEntry::ConceptDefinition:
        if (ls) writeDefinition(ol,ls->title(getLanguage()));
        break;
      case LayoutDocEntry::DetailedDesc:
        if (ls) writeDetailedDescription(ol,ls->title(getLanguage()));
        break;
      case LayoutDocEntry::AuthorSection:
        writeAuthorSection(ol);
        break;
      case LayoutDocEntry::ClassIncludes:
        writeIncludeFiles(ol);
        break;
      case LayoutDocEntry::MemberDeclStart:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceInterfaces:
      case LayoutDocEntry::NamespaceStructs:
      case LayoutDocEntry::NamespaceExceptions:
      case LayoutDocEntry::NamespaceConcepts:
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceNestedConstantGroups:
      case LayoutDocEntry::MemberGroups:
      case LayoutDocEntry::MemberDecl:
      case LayoutDocEntry::MemberDeclEnd:
      case LayoutDocEntry::MemberDefStart:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::MemberDef:
      case LayoutDocEntry::MemberDefEnd:
      case LayoutDocEntry::ClassInheritanceGraph:
      case LayoutDocEntry::ClassNestedClasses:
      case LayoutDocEntry::ClassCollaborationGraph:
      case LayoutDocEntry::ClassAllMembersLink:
      case LayoutDocEntry::ClassUsedFiles:
      case LayoutDocEntry::ClassInlineClasses:
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
      case LayoutDocEntry::FileInlineClasses:
      case LayoutDocEntry::FileSourceLink:
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
            "LayoutDocManager::Concept entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(ol,this);
}

void ConceptDefImpl::writeDeclarationLink(OutputList &ol,bool &found,const QCString &header,bool localNames) const
{
  if (isLinkable())
  {
    if (!found) // first concept
    {
      ol.startMemberHeader("concepts");
      if (!header.isEmpty())
      {
        ol.parseText(header);
      }
      else
      {
        theTranslator->trConcept(true,false);
      }
      ol.endMemberHeader();
      ol.startMemberList();
      found=TRUE;
    }
    ol.startMemberDeclaration();
    ol.startMemberItem(anchor(),OutputGenerator::MemberItemType::Normal);
    ol.writeString("concept ");
    QCString cname = displayName(!localNames);
    ol.insertMemberAlign();
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
    ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
    // add the brief description if available
    if (!briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
    {
      auto parser { createDocParser() };
      auto ast    { validatingParseDoc(
                                *parser.get(),briefFile(),briefLine(),this,nullptr,
                                briefDescription(),FALSE,FALSE,
                                QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
      if (!ast->isEmpty())
      {
        ol.startMemberDescription(anchor());
        ol.writeDoc(ast.get(),this,nullptr);
        ol.endMemberDescription();
      }
    }
    ol.endMemberDeclaration(anchor(),QCString());
  }
}

void ConceptDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
  docFindSections(inbodyDocumentation(),this,docFile());
}

//------------------------------------------------------------------------------------

bool ConceptLinkedRefMap::declVisible() const
{
  bool hideUndocClasses = Config_getBool(HIDE_UNDOC_CLASSES);
  for (const auto &cd : *this)
  {
    bool isLink = cd->isLinkable();
    if (isLink || !hideUndocClasses)
    {
      return true;
    }
  }
  return false;
}

void ConceptLinkedRefMap::writeDeclaration(OutputList &ol,const QCString &header,bool localNames) const
{
  bool found=FALSE;
  for (const auto &cd : *this)
  {
    cd->writeDeclarationLink(ol,found,header,localNames);
  }
  if (found) ol.endMemberList();
}

//------------------------------------------------------------------------------------

// --- Cast functions
//
ConceptDef *toConceptDef(Definition *d)
{
  if (d && (typeid(*d)==typeid(ConceptDefImpl) || typeid(*d)==typeid(ConceptDefAliasImpl)))
  {
    return static_cast<ConceptDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

ConceptDef *toConceptDef(DefinitionMutable *md)
{
  Definition *d = toDefinition(md);
  if (d && typeid(*d)==typeid(ConceptDefImpl))
  {
    return static_cast<ConceptDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

const ConceptDef *toConceptDef(const Definition *d)
{
  if (d && (typeid(*d)==typeid(ConceptDefImpl) || typeid(*d)==typeid(ConceptDefAliasImpl)))
  {
    return static_cast<const ConceptDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

ConceptDefMutable *toConceptDefMutable(Definition *d)
{
  if (d && typeid(*d)==typeid(ConceptDefImpl))
  {
    return static_cast<ConceptDefMutable*>(d);
  }
  else
  {
    return nullptr;
  }
}

// -- helpers

ConceptDef *getConcept(const QCString &n)
{
  if (n.isEmpty()) return nullptr;
  return Doxygen::conceptLinkedMap->find(n);
}

ConceptDef *getResolvedConcept(const Definition *d,const QCString &name)
{
  ConceptDef *cd=nullptr;
  while (d && d!=Doxygen::globalScope)
  {
    cd = getConcept(d->name()+"::"+name);
    if (cd) return cd;
    d = d->getOuterScope();
  }
  cd = getConcept(name);
  return cd;
}


