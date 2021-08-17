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
   ~ConceptDefImpl();

    virtual ConceptDef *resolveAlias() { return this; }

    //---------- ConceptDef
    virtual DefType definitionType() const;
    virtual CodeSymbolType codeSymbolType() const { return CodeSymbolType::Concept; }
    virtual QCString getOutputFileBase() const;
    virtual bool hasDetailedDescription() const;
    virtual QCString displayName(bool includeScope=true) const;
    virtual const IncludeInfo *includeInfo() const;
    virtual ArgumentList getTemplateParameterList() const;
    virtual QCString anchor() const;
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual QCString initializer() const;
    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const;
    virtual const NamespaceDef *getNamespaceDef() const;
    virtual const FileDef *getFileDef() const;
    virtual QCString title() const;

    //---------- ConceptDefMutable
    virtual void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force);
    virtual void setTemplateArguments(const ArgumentList &al);
    virtual void setNamespace(NamespaceDef *nd);
    virtual void setFileDef(FileDef *fd);
    virtual void writeTagFile(TextStream &);
    virtual void writeDocumentation(OutputList &);
    virtual void setInitializer(const QCString &init);
    virtual void findSectionsInDocumentation();

    //---------- Helpers
    void writeBriefDescription(OutputList &) const;
    void writeAuthorSection(OutputList &ol) const;
    void writeIncludeFiles(OutputList &ol) const;
    void writeDetailedDescription(OutputList &ol,const QCString &title) const;
    void writeDefinition(OutputList &ol,const QCString &title) const;

  private:
    QCString                     m_fileName;
    std::unique_ptr<IncludeInfo> m_incInfo;
    NamespaceDef                *m_nspace = 0;
    FileDef                     *m_fileDef = 0;
    ArgumentList                 m_tArgList;
    QCString                     m_initializer;
};

ConceptDefMutable *createConceptDef(
             const QCString &fileName,int startLine,int startColumn,
             const QCString &name, const QCString &tagRef,const QCString &tagFile)
{
  return new ConceptDefImpl(fileName,startLine,startColumn,name,tagRef,tagFile);
}

//------------------------------------------------------------------------------------

class ConceptDefAliasImpl : public DefinitionAliasMixin<ConceptDef>
{
  public:
    ConceptDefAliasImpl(const Definition *newScope,const ConceptDef *cd)
      : DefinitionAliasMixin(newScope,cd) { init(); }
    virtual ~ConceptDefAliasImpl() { deinit(); }

    const ConceptDef *getCdAlias() const { return toConceptDef(getAlias()); }
    virtual ConceptDef *resolveAlias() { return const_cast<ConceptDef*>(getCdAlias()); }

    virtual DefType definitionType() const { return TypeConcept; }
    virtual CodeSymbolType codeSymbolType() const
    { return getCdAlias()->codeSymbolType(); }
    virtual QCString getOutputFileBase() const
    { return getCdAlias()->getOutputFileBase(); }
    virtual QCString getReference() const
    { return getCdAlias()->getReference(); }
    virtual bool isReference() const
    { return getCdAlias()->isReference(); }
    virtual bool hasDetailedDescription() const
    { return getCdAlias()->hasDetailedDescription(); }
    virtual QCString displayName(bool includeScope=true) const
    { return getCdAlias()->displayName(includeScope); }
    virtual const IncludeInfo *includeInfo() const
    { return getCdAlias()->includeInfo(); }
    virtual ArgumentList getTemplateParameterList() const
    { return getCdAlias()->getTemplateParameterList(); }
    virtual QCString anchor() const
    { return getCdAlias()->anchor(); }
    virtual bool isLinkableInProject() const
    { return getCdAlias()->isLinkableInProject(); }
    virtual bool isLinkable() const
    { return getCdAlias()->isLinkable(); }
    virtual QCString initializer() const
    { return getCdAlias()->initializer(); }
    virtual const NamespaceDef *getNamespaceDef() const
    { return getCdAlias()->getNamespaceDef(); }
    virtual const FileDef *getFileDef() const
    { return getCdAlias()->getFileDef(); }
    virtual QCString title() const
    { return getCdAlias()->title(); }
    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const
    { getCdAlias()->writeDeclarationLink(ol,found,header,localNames); }
};

ConceptDef *createConceptDefAlias(const Definition *newScope,const ConceptDef *cd)
{
  ConceptDef *acd = new ConceptDefAliasImpl(newScope,cd);
  return acd;
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
  return hasDocumentation() && !isReference() && !isHidden();
}

bool ConceptDefImpl::isLinkable() const
{
  return isLinkableInProject() || isReference();
}

void ConceptDefImpl::setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force)
{
  if (!m_incInfo) m_incInfo = std::make_unique<IncludeInfo>();
  if ((!incName.isEmpty() && m_incInfo->includeName.isEmpty()) ||
      (fd!=0 && m_incInfo->fileDef==0)
     )
  {
    m_incInfo->fileDef     = fd;
    m_incInfo->includeName = incName;
    m_incInfo->local       = local;
  }
  if (force && !incName.isEmpty())
  {
    m_incInfo->includeName = incName;
    m_incInfo->local       = local;
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

const FileDef *ConceptDefImpl::getFileDef() const
{
  return m_fileDef;
}

QCString ConceptDefImpl::title() const
{
  return theTranslator->trConceptReference(displayName());
}

void ConceptDefImpl::writeTagFile(TextStream &tagFile)
{
  tagFile << "  <compound kind=\"concept\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>\n";
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
    std::unique_ptr<IDocParser> parser { createDocParser() };
    std::unique_ptr<DocRoot>  rootNode { validatingParseDoc(
                        *parser.get(),briefFile(),briefLine(),this,0,
                        briefDescription(),TRUE,FALSE,
                        QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    if (rootNode && !rootNode->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(rootNode.get(),this,0);
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.writeString(" \n");
      ol.enable(OutputGenerator::RTF);

      if (hasDetailedDescription())
      {
        ol.disableAllBut(OutputGenerator::Html);
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
      ol.docify("#include ");
      if (m_incInfo->local)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.docify(nm);
      ol.disableAllBut(OutputGenerator::Html);
      ol.enable(OutputGenerator::Html);
      if (m_incInfo->fileDef)
      {
        ol.writeObjectLink(QCString(),m_incInfo->fileDef->includeName(),QCString(),nm);
      }
      else
      {
        ol.docify(nm);
      }
      ol.popGeneratorState();
      if (m_incInfo->local)
        ol.docify("\"");
      else
        ol.docify(">");
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
    ol.startCodeFragment("DoxyCode");
    QCString scopeName;
    if (getOuterScope()!=Doxygen::globalScope) scopeName=getOuterScope()->name();
    TextStream conceptDef;
    conceptDef << m_initializer;
    intf->parseCode(ol,scopeName,conceptDef.str(),SrcLangExt_Cpp,false,QCString(),
                    m_fileDef, -1,-1,true,0,false,this);
    ol.endCodeFragment("DoxyCode");
}

void ConceptDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title) const
{
  static bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  if (hasDetailedDescription())
  {
    ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.writeRuler();
    ol.popGeneratorState();

    ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.writeAnchor(QCString(),"details");
    ol.popGeneratorState();

    ol.startGroupHeader();
      ol.parseText(title);
    ol.endGroupHeader();

    ol.startTextBlock();
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
      ol.disable(OutputGenerator::Html);
      ol.writeString("\n\n");
      ol.popGeneratorState();
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,0,documentation(),TRUE,FALSE,
          QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }

    writeSourceDef(ol,name());
    ol.endTextBlock();
  }
}

void ConceptDefImpl::writeAuthorSection(OutputList &ol) const
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void ConceptDefImpl::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString pageTitle = theTranslator->trConceptReference(displayName());
  startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_ConceptVisible,!generateTreeView);

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
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),displayName());

  // ---- contents part

  ol.startContents();

  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }

  Doxygen::indexList->addIndexItem(this,0);
  //---------------------------------------- start flexible part -------------------------------

  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Concept))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol);
        break;
      case LayoutDocEntry::ConceptDefinition:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeDefinition(ol,ls->title(getLanguage()));
        }
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeDetailedDescription(ol,ls->title(getLanguage()));
        }
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
            "LayoutDocManager::Namespace entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(this,ol);
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
    ol.startMemberItem(anchor(),FALSE);
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
    ol.endMemberItem();
    // add the brief description if available
    if (!briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
    {
      std::unique_ptr<IDocParser> parser { createDocParser() };
      std::unique_ptr<DocRoot>  rootNode { validatingParseDoc(
                                *parser.get(),briefFile(),briefLine(),this,0,
                                briefDescription(),FALSE,FALSE,
                                QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
      if (rootNode && !rootNode->isEmpty())
      {
        ol.startMemberDescription(anchor());
        ol.writeDoc(rootNode.get(),this,0);
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
    return 0;
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
    return 0;
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
    return 0;
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
    return 0;
  }
}

ConceptDefMutable *toConceptDefMutable(const Definition *d)
{
  if (d && typeid(*d)==typeid(ConceptDefImpl))
  {
    return const_cast<ConceptDefMutable*>(static_cast<const ConceptDefMutable*>(d));
  }
  else
  {
    return 0;
  }
}

// -- helpers

ConceptDef *getConcept(const QCString &n)
{
  if (n.isEmpty()) return 0;
  return Doxygen::conceptLinkedMap->find(n);
}

ConceptDef *getResolvedConcept(const Definition *d,const QCString &name)
{
  ConceptDef *cd=0;
  while (d && d!=Doxygen::globalScope)
  {
    cd = getConcept(d->name()+"::"+name);
    if (cd) return cd;
    d = d->getOuterScope();
  }
  cd = getConcept(name);
  return cd;
}


