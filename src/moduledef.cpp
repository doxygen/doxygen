/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#include "moduledef.h"
#include "definitionimpl.h"
#include "entry.h"
#include "memberdef.h"
#include "classlist.h"
#include "namespacedef.h"
#include "conceptdef.h"
#include "config.h"
#include "outputlist.h"
#include "language.h"
#include "util.h"
#include "groupdef.h"
#include "message.h"
#include "membergroup.h"
#include "classdef.h"
#include "textstream.h"
#include "trace.h"

class ModuleDefImpl;

struct HeaderInfo
{
  HeaderInfo(const QCString &fn,const QCString &name,bool sys)
    : fileName(fn), headerName(name), isSystem(sys) {}
  QCString fileName;   // file containing the import
  QCString headerName; // name of the imported header
  bool isSystem;       // <...> => true, "..." => false
};

using HeaderInfoVector = std::vector<HeaderInfo>;


class ModuleDefImpl : public DefinitionMixin<ModuleDef>
{
  public:
    ModuleDefImpl(const QCString &fileName,int startLine,int startColom,
                  const QCString &name, Type type, const QCString &partitionName)
      : DefinitionMixin<ModuleDef>(fileName,startLine,startColom,name,nullptr,nullptr,true),
        m_type(type), m_partitionName(partitionName) {}

    // --- Definition
    DefType definitionType() const override { return TypeModule; }
    CodeSymbolType codeSymbolType() const override { return CodeSymbolType::Module; }
    QCString displayName(bool=TRUE) const override { return name(); }
    QCString getOutputFileBase() const override;
    QCString anchor() const override { return ""; }
    bool isLinkableInProject() const override { return isLinkable() && !isHidden() && !isReference(); }
    bool isLinkable() const override { return hasDocumentation(); }
    QCString qualifiedName() const override;
    void writeSummaryLinks(OutputList &ol) const override;

    // --- ModuleDef
    Type moduleType() const override { return m_type; }
    QCString partitionName() const override { return m_partitionName; }
    void writeDocumentation(OutputList &ol) override;
    bool isPrimaryInterface() const override { return m_type==Type::Interface && m_partitionName.isEmpty(); }
    MemberList *getMemberList(MemberListType lt) const override;
    const MemberLists &getMemberLists() const override { return m_memberLists; }
    const MemberGroupList &getMemberGroups() const override { return m_memberGroups; }
    const ClassLinkedRefMap &getClasses()  const override { return m_classes;  }
    const ConceptLinkedRefMap &getConcepts() const override { return m_concepts; }
    int countVisibleMembers() const override;
    FileDef *getFileDef() const override { return m_fileDef; }
    const ImportInfoMap &getImports() const override { return m_imports; }
    const ImportInfoMap &getExports() const override { return m_exportedModules; }
    const ModuleMap &partitions() const override { return m_partitions; }
    void writeTagFile(TextStream &t) const override;
    FileList getUsedFiles() const override;

    void writeExports(OutputList &ol,const QCString &title);
    void writeClassDeclarations(OutputList &ol,const QCString &title);
    void writeConcepts(OutputList &ol,const QCString &title);
    void writeFiles(OutputList &ol,const QCString &title);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void writeAuthorSection(OutputList &ol);
    void writeDeclarationLink(OutputList &ol,bool &found,const QCString &header,bool localNames) const;

    void addHeader(int line,const QCString &headerName,bool isSystem);
    void addImport(int line,const QCString &moduleName,const QCString &partitionName,bool isExported);
    void addClassToModule(const Entry *root,ClassDef *cd);
    void addConceptToModule(const Entry *root,ConceptDef *cd);
    void addMemberToModule(const Entry *root,MemberDef *md);
    void addPartition(ModuleDefImpl *mod);
    void addContributingModule(ModuleDefImpl *mod);
    void setPrimaryInterface(const ModuleDef *mod);
    void setFileDef(FileDef *fd);
    void addMemberToList(MemberListType lt,MemberDef *md);
    void addExportedModule(const QCString &moduleName,const ImportInfo &info);
    void addListReferences();
    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    void sortMemberLists();

    //ModuleMap &partitions() { return m_partitions; }
    void mergeSymbolsFrom(ModuleDefImpl *other);
    bool hasDetailedDescription() const;
    void countMembers();

  private:
    Type                  m_type;
    QCString              m_partitionName;
    ImportInfoMap         m_imports;
    ImportInfoMap         m_exportedModules;
    ClassLinkedRefMap     m_classes;
    ConceptLinkedRefMap   m_concepts;
    MemberLists           m_memberLists;
    ModuleMap             m_partitions;
    ModuleList            m_contributing;
    MemberGroupList       m_memberGroups;
    const ModuleDef      *m_primaryInterface = nullptr;
    FileDef              *m_fileDef = nullptr;  // file holding this module
};

QCString ModuleDefImpl::getOutputFileBase() const
{
  return convertNameToFile("module_" + name());
}

QCString ModuleDefImpl::qualifiedName() const
{
  QCString result=name();
  if (!m_partitionName.isEmpty())
  {
    result+=":"+m_partitionName;
  }
  return result;
}

void ModuleDefImpl::addPartition(ModuleDefImpl *mod)
{
  std::string qName = mod->qualifiedName().str();
  if (m_partitions.find(qName)==m_partitions.end())
  {
    m_partitions.emplace(qName,mod);
  }
}

void ModuleDefImpl::addContributingModule(ModuleDefImpl *mod)
{
  if (std::find(m_contributing.begin(),m_contributing.end(),mod)==m_contributing.end())
  {
    m_contributing.push_back(mod);
  }
}

void ModuleDefImpl::setPrimaryInterface(const ModuleDef *mod)
{
  m_primaryInterface = mod;
}

void ModuleDefImpl::setFileDef(FileDef *fd)
{
  m_fileDef = fd;
}

void ModuleDefImpl::addHeader(int line,const QCString &headerName,bool isSystem)
{
  AUTO_TRACE("name={}:line={},header={},isSystem={}",name(),line,headerName,isSystem);
}

void ModuleDefImpl::addImport(int line,const QCString &moduleName,const QCString &partitionName,bool isExported)
{
  AUTO_TRACE("name={}:line={},module={},partition={}",name(),line,moduleName,partitionName);
  m_imports.emplace(getDefFileName().str()+":"+std::to_string(line),ImportInfo(this,moduleName,line,partitionName,isExported));
}

void ModuleDefImpl::addExportedModule(const QCString &moduleName,const ImportInfo &info)
{
  AUTO_TRACE("name={}:moduleName={},import={}",name(),moduleName,info.importName);
  m_exportedModules.emplace(moduleName.str(),info);
}

void ModuleDefImpl::addClassToModule(const Entry *root,ClassDef *cd)
{
  QCString className = cd->qualifiedName();
  AUTO_TRACE("{}:{} class {} of module {} exported={}",
      root->fileName,root->startLine, className, name(), root->exported);
  bool isExported = m_classes.find(className)!=nullptr;
  if (root->exported && !isExported)
  {
    m_classes.add(className,cd);
  }
  auto cdm = toClassDefMutable(cd);
  if (cdm && root->exported && !cd->isExported())
  {
    cdm->setExported(true);
  }
}

void ModuleDefImpl::addConceptToModule(const Entry *root,ConceptDef *cd)
{
  QCString conceptName = cd->qualifiedName();
  AUTO_TRACE("{}:{} concept {} of module {} exported={}",
     root->fileName,root->startLine,
     cd->qualifiedName(),name(),
     root->exported);
  bool isExported = m_classes.find(conceptName)!=nullptr;
  if (root->exported && !isExported)
  {
    m_concepts.add(conceptName,cd);
  }
  auto cdm = toConceptDefMutable(cd);
  if (cdm && root->exported && !cd->isExported())
  {
    cdm->setExported(true);
  }
}

MemberList *ModuleDefImpl::getMemberList(MemberListType lt) const
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

void ModuleDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  auto &ml = m_memberLists.get(lt,MemberListContainer::Module);
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

void ModuleDefImpl::addMemberToModule(const Entry *root,MemberDef *md)
{
  AUTO_TRACE("{}:{} member {} of module {} exported={}",
      qPrint(root->fileName),root->startLine,
      qPrint(md->qualifiedName()),qPrint(name()),
      root->exported);
  MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
  if (allMemberList==nullptr)
  {
    m_memberLists.emplace_back(std::make_unique<MemberList>(MemberListType::AllMembersList(),MemberListContainer::Module));
    allMemberList = m_memberLists.back().get();
  }
  if (allMemberList->contains(md))
  {
    return;
  }
  allMemberList->push_back(md);
  switch (md->memberType())
  {
    case MemberType::Variable:
      addMemberToList(MemberListType::DecVarMembers(),md);
      break;
    case MemberType::Function:
      addMemberToList(MemberListType::DecFuncMembers(),md);
      break;
    case MemberType::Typedef:
      addMemberToList(MemberListType::DecTypedefMembers(),md);
      break;
    case MemberType::Enumeration:
      addMemberToList(MemberListType::DecEnumMembers(),md);
      break;
    default:
      break;
  }
  auto mdm = toMemberDefMutable(md);
  if (mdm && root->exported && !md->isExported())
  {
    mdm->setExported(true);
  }
}

void ModuleDefImpl::mergeSymbolsFrom(ModuleDefImpl *other)
{
  AUTO_TRACE("{} merging symbols of {} ({}:{})",
      name(),other->qualifiedName(),other->getDefFileName(),other->getDefLine());
  for (const auto &cd : other->getClasses())
  {
    m_classes.add(cd->qualifiedName(),cd);
  }
  for (const auto &cd : other->getConcepts())
  {
    m_concepts.add(cd->qualifiedName(),cd);
  }
  auto mergeMemberList = [this,other](MemberListType lt)
  {
    const auto srcMl = other->getMemberList(lt);
    if (srcMl)
    {
      auto &dstMl = m_memberLists.get(lt,srcMl->container());
      for (const auto &md : *srcMl)
      {
        dstMl->push_back(md);
      }
    }
  };
  mergeMemberList(MemberListType::DecVarMembers());
  mergeMemberList(MemberListType::DecFuncMembers());
  mergeMemberList(MemberListType::DecTypedefMembers());
  mergeMemberList(MemberListType::DecEnumMembers());
}

void ModuleDefImpl::writeDocumentation(OutputList &ol)
{
  if (isReference()) return;
  ol.pushGeneratorState();
  AUTO_TRACE("%s file=%s",name(),getDefFileName());
  SrcLangExt lang = getLanguage();
  QCString pageTitle = theTranslator->trModuleReference(displayName());
  startFile(ol,getOutputFileBase(),name(),pageTitle,HighlightedItem::ModuleVisible,false,QCString(),0);

  // ---- title part
  ol.startHeaderSection();
  writeSummaryLinks(ol);
  ol.startTitleHead(getOutputFileBase());

  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.parseText(pageTitle);
  ol.popGeneratorState();

  addGroupListToTitle(ol,this);

  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.endTitleHead(getOutputFileBase(),pageTitle);
  ol.popGeneratorState();

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Man);
  ol.endTitleHead(getOutputFileBase(),name());
  ol.writeString(" - ");
  ol.parseText(pageTitle);
  ol.popGeneratorState();

  ol.endHeaderSection();
  ol.startContents();

  //---------------------------------------- start flexible part -------------------------------
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Module))
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
      case LayoutDocEntry::ModuleClasses:
        if (ls) writeClassDeclarations(ol,ls->title(lang));
        break;
      case LayoutDocEntry::ModuleConcepts:
        if (ls) writeConcepts(ol,ls->title(lang));
        break;
      case LayoutDocEntry::ModuleExports:
        if (ls) writeExports(ol,ls->title(lang));
        break;
      case LayoutDocEntry::ModuleUsedFiles:
        if (ls) writeFiles(ol,ls->title(lang));
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
        if (ls) writeDetailedDescription(ol,ls->title(lang));
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
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
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceNestedConstantGroups:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceConcepts:
      case LayoutDocEntry::NamespaceInterfaces:
      case LayoutDocEntry::NamespaceStructs:
      case LayoutDocEntry::NamespaceExceptions:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::ConceptDefinition:
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
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::Module entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------
  endFile(ol);

  ol.popGeneratorState();
}

void ModuleDefImpl::writeClassDeclarations(OutputList &ol,const QCString &title)
{
  m_classes.writeDeclaration(ol,nullptr,title,FALSE);
}

void ModuleDefImpl::writeConcepts(OutputList &ol,const QCString &title)
{
  m_concepts.writeDeclaration(ol,title,FALSE);
}

void ModuleDefImpl::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void ModuleDefImpl::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void ModuleDefImpl::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.disable(OutputType::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void ModuleDefImpl::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputType::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void ModuleDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
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
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,nullptr,briefDescription(),FALSE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
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
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,nullptr,documentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    ol.endTextBlock();
  }
}

void ModuleDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     briefFile(),briefLine(),this,nullptr,
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
        ol.startTextLink(QCString(),"details");
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
      }
      ol.popGeneratorState();
      ol.endParagraph();
    }
  }
  ol.writeSynopsis();
}

void ModuleDefImpl::writeMemberGroups(OutputList &ol)
{
  for (const auto &mg : m_memberGroups)
  {
    mg->writeDeclarations(ol,nullptr,nullptr,nullptr,nullptr,this);
  }
}

void ModuleDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,nullptr,nullptr,nullptr,nullptr,this,title,QCString());
}

void ModuleDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

void ModuleDefImpl::writeAuthorSection(OutputList &ol)
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

bool ModuleDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return (!briefDescription().isEmpty() && repeatBrief) || !documentation().isEmpty();
}

void ModuleDefImpl::countMembers()
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

void ModuleDefImpl::addListReferences()
{
  const RefItemVector &xrefItems = xrefListItems();
  addRefItem(xrefItems,
      qualifiedName(),
      getLanguage()==SrcLangExt::Fortran ?
      theTranslator->trModule(TRUE,TRUE) :
      theTranslator->trNamespace(TRUE,TRUE),
      getOutputFileBase(),displayName(),
      QCString(),
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

void ModuleDefImpl::addMembersToMemberGroup()
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
    if (mg->allMembersInSameSection() && Config_getBool(SUBGROUPING))
    {
      //printf("----> addToDeclarationSection(%s)\n",qPrint(mg->header()));
      mg->addToDeclarationSection();
    }
  }
}

void ModuleDefImpl::distributeMemberGroupDocumentation()
{
  for (const auto &mg : m_memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void ModuleDefImpl::findSectionsInDocumentation()
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

void ModuleDefImpl::sortMemberLists()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }

  if (Config_getBool(SORT_BRIEF_DOCS))
  {
    auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
    {
      return Config_getBool(SORT_BY_SCOPE_NAME)          ?
        qstricmp_sort(c1->name(), c2->name())<0          :
        qstricmp_sort(c1->className(), c2->className())<0;
    };
    std::stable_sort(m_classes.begin(), m_classes.end(), classComp);

    auto conceptComp = [](const ConceptLinkedRefMap::Ptr &c1,const ConceptLinkedRefMap::Ptr &c2)
    {
      return Config_getBool(SORT_BY_SCOPE_NAME)                   ?
        qstricmp_sort(c1->qualifiedName(), c2->qualifiedName())<0 :
        qstricmp_sort(c1->name(), c2->name())<0;
    };
    std::stable_sort(m_concepts.begin(), m_concepts.end(), conceptComp);
  }

  static auto contrComp = [](const ModuleDef *m1, const ModuleDef *m2)
  {
    FileDef *f1 = m1->getFileDef();
    FileDef *f2 = m2->getFileDef();
    QCString fn1 = f1 ? f1->name() : m1->name();
    QCString fn2 = f2 ? f2->name() : m2->name();
    static auto typeRank = [](const ModuleDef *m) -> int
    {
      if (m->moduleType()==ModuleDef::Type::Interface)
      {
        if (m->partitionName().isEmpty()) return 0;  // primary interface unit
        return 1;                                    // partition interface unit
      }
      else
      {
        if (!m->partitionName().isEmpty()) return 2;  // partition implementation unit
        return 3;                                    // implementation unit
      }
    };
    auto tr1 = typeRank(m1);
    auto tr2 = typeRank(m2);
    int diff = qstricmp_sort(fn1,fn2);
    return tr1<tr2 || (tr1==tr2 && diff<0);
  };

  std::stable_sort(m_contributing.begin(), m_contributing.end(), contrComp);
}

void ModuleDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=TRUE;
  SrcLangExt lang=getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Module))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    if (lde->kind()==LayoutDocEntry::ModuleClasses && m_classes.declVisible() && ls)
    {
      QCString label = "classes";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::ModuleConcepts && m_concepts.declVisible() && ls)
    {
      QCString label = "concepts";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::ModuleUsedFiles && ls)
    {
      QCString label = "files";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
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
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void ModuleDefImpl::writeDeclarationLink(OutputList &ol,bool &found,const QCString &header,bool localNames) const
{
  if (isLinkable())
  {
    if (!found) // first module
    {
      ol.startMemberHeader("modules");
      if (!header.isEmpty())
      {
        ol.parseText(header);
      }
      else
      {
        theTranslator->trModule(true,false);
      }
      ol.endMemberHeader();
      ol.startMemberList();
      found=TRUE;
    }
    ol.startMemberDeclaration();
    ol.startMemberItem(anchor(),OutputGenerator::MemberItemType::Normal);
    ol.writeString("module ");
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


void ModuleDefImpl::writeExports(OutputList &ol,const QCString &title)
{
  AUTO_TRACE("name={} count={}",name(),m_exportedModules.size());
  if (!m_exportedModules.empty())
  {
    ol.startMemberHeader("exports");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    for (auto &[moduleName,importInfo] : m_exportedModules)
    {
      ModuleDef *mod = ModuleManager::instance().getPrimaryInterface(importInfo.importName);
      ol.startMemberDeclaration();
      ol.startMemberItem(importInfo.importName,OutputGenerator::MemberItemType::Normal);
      ol.docify(theTranslator->trModule(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      if (mod && mod->isLinkable())
      {
        ol.writeObjectLink(mod->getReference(),mod->getOutputFileBase(),QCString(),mod->displayName());
      }
      else
      {
        ol.startBold();
        ol.docify(importInfo.importName);
        ol.endBold();
      }
      ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
      if (mod && !mod->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(mod->getOutputFileBase());
        ol.generateDoc(briefFile(),briefLine(),mod,nullptr,mod->briefDescription(),FALSE,FALSE,
            QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(QCString(),QCString());
    }
    ol.endMemberList();
  }
}

void ModuleDefImpl::writeFiles(OutputList &ol,const QCString &title)
{
  AUTO_TRACE("{} count={}",name(),m_contributing.size());
  if (!m_contributing.empty())
  {
    ol.startMemberHeader("files");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    for (const auto &mod : m_contributing)
    {
      FileDef *fd = mod->getFileDef();
      if (fd)
      {
        ol.startMemberDeclaration();
        ol.startMemberItem(fd->anchor(),OutputGenerator::MemberItemType::Normal);
        ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
        ol.insertMemberAlign();
        QCString path=fd->getPath();
        if (Config_getBool(FULL_PATH_NAMES))
        {
          ol.docify(stripFromPath(path));
        }
        if (fd->isLinkable())
        {
          ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),fd->displayName());
        }
        else
        {
          ol.startBold();
          ol.docify(fd->displayName());
          ol.endBold();
        }
        ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
        if (!fd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(fd->getOutputFileBase());
          ol.generateDoc(briefFile(),briefLine(),fd,nullptr,fd->briefDescription(),FALSE,FALSE,
              QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(QCString(),QCString());
      }
    }
    ol.endMemberList();
  }
}

FileList ModuleDefImpl::getUsedFiles() const
{
  FileList result;
  for (const auto &mod : m_contributing)
  {
    FileDef *fd = mod->getFileDef();
    if (fd) result.push_back(fd);
  }
  return result;
}

int ModuleDefImpl::countVisibleMembers() const
{
  int count=0;
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Module))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
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
    else if (lde->kind()==LayoutDocEntry::ModuleClasses)
    {
      for (const auto &cd : getClasses())
      {
        if (cd->isLinkableInProject())
        {
          count++;
        }
      }
    }
    else if (lde->kind()==LayoutDocEntry::ModuleConcepts)
    {
      for (const auto &cd : getConcepts())
      {
        if (cd->isLinkableInProject())
        {
          count++;
        }
      }
    }
  }
  return count;
}

void ModuleDefImpl::writeTagFile(TextStream &tagFile) const
{
  if (!isPrimaryInterface() || !isLinkableInProject()) return;
  tagFile << "  <compound kind=\"module\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  const FileDef *fd = getFileDef();
  QCString fn = fd ? fd->getOutputFileBase() : getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "    <filename>" << convertToXML(fn) << "</filename>\n";
#if 0 // at the moment we do not export the members of a module to a tag file.
      // We let the project using a tag file directly link to the implementation of the
      // symbols (which have the same scope).
      //
      // When we support linking to a module's interface instead we need to
      // export the module's members as well. Then we probably need a way to
      // disambiguate/prioritize a link to a module over a link to the implementation,
      // for instance by hiding non-exported symbols from the tag file.
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::ModuleExports:
        {
          for (const auto &[modName,importInfo] : m_exportedModules)
          {
            tagFile << "    <export>" << convertToXML(importInfo.importName) << "</export>\n";
          }
        }
        break;
      case LayoutDocEntry::ModuleClasses:
        {
          for (const auto &cd : m_classes)
          {
            if (cd->isLinkableInProject())
            {
              tagFile << "    <class kind=\"" << cd->compoundTypeString()
                      << "\">" << convertToXML(cd->name()) << "</class>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::ModuleConcepts:
        {
          for (const auto &cd : m_concepts)
          {
            if (cd->isLinkableInProject())
            {
              tagFile << "    <concept>" << convertToXML(cd->name())
                      << "</concept>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::ModuleUsedFiles:
        {
          for (const auto &usedFd : getUsedFiles())
          {
            if (usedFd->isLinkableInProject())
            {
              tagFile << "    <file>" << convertToXML(usedFd->name()) << "</file>\n";
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
              ml->writeTagFile(tagFile,true);
            }
          }
        }
        break;
      default:
        break;
    }
  }
#endif
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "    <clangid>" << convertToXML(idStr) << "</clangid>\n";
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>\n";
}

//------------------------------------------------------------------------------------------------------------

ModuleDef *toModuleDef(Definition *d)
{
  if (d==nullptr) return nullptr;
  return (typeid(*d)==typeid(ModuleDefImpl)) ? static_cast<ModuleDef*>(d) : nullptr;
}

const ModuleDef *toModuleDef(const Definition *d)
{
  if (d==nullptr) return nullptr;
  return (typeid(*d)==typeid(ModuleDefImpl)) ? static_cast<const ModuleDef*>(d) : nullptr;
}

static inline ModuleDefImpl *toModuleDefImpl(ModuleDef *m)
{ return static_cast<ModuleDefImpl*>(m); }

//static inline const ModuleDefImpl *toModuleDefImpl(const ModuleDef *m)
//{ return static_cast<const ModuleDefImpl*>(m); }

static inline ModuleDefImpl *toModuleDefImpl(const std::unique_ptr<ModuleDef> &m)
{ return static_cast<ModuleDefImpl*>(m.get()); }

//------------------------------------------------------------------------------------

bool ModuleLinkedRefMap::declVisible() const
{
  bool hideUndocClasses = Config_getBool(HIDE_UNDOC_CLASSES);
  for (const auto &mod : *this)
  {
    bool isLink = mod->isLinkable();
    if (isLink || !hideUndocClasses)
    {
      return true;
    }
  }
  return false;
}

void ModuleLinkedRefMap::writeDeclaration(OutputList &ol,const QCString &header,bool localNames) const
{
  bool found=FALSE;
  for (const auto &mod : *this)
  {
    toModuleDefImpl(mod)->writeDeclarationLink(ol,found,header,localNames);
  }
  if (found) ol.endMemberList();
}

//------------------------------------------------------------------------------------------------------------

struct ModuleManager::Private
{
  ModuleLinkedMap     moduleFileMap; // file->module mapping
  std::unordered_map<std::string,ModuleList> moduleNameMap; // name->module mapping
  ImportInfoMap       externalImports;
  HeaderInfoVector    headers;
  std::mutex          mutex;
};

ModuleManager &ModuleManager::instance()
{
  static ModuleManager m;
  return m;
}

ModuleManager::ModuleManager() : p(std::make_unique<Private>())
{
}

void ModuleManager::createModuleDef(const QCString &fileName,int line,int column,bool exported,
                                    const QCString &moduleName,const QCString &partitionName)
{
  AUTO_TRACE("{}:{}: Found module name='{}' partition='{}' exported='{}'",
      fileName,line,moduleName,partitionName,exported);
  std::lock_guard lock(p->mutex);
  ModuleDef::Type mt = exported ? ModuleDef::Type::Interface : ModuleDef::Type::Implementation;
  std::unique_ptr<ModuleDef> modDef = std::make_unique<ModuleDefImpl>(fileName,line,column,moduleName,mt,partitionName);
  auto mod = p->moduleFileMap.add(fileName,std::move(modDef));
  auto it = p->moduleNameMap.find(moduleName.str());
  if (it == p->moduleNameMap.end())
  {
    ModuleList ml;
    ml.push_back(mod);
    p->moduleNameMap.emplace(moduleName.str(),ml);
  }
  else
  {
    it->second.push_back(mod);
  }
}

void ModuleManager::clear()
{
  std::lock_guard lock(p->mutex);
  p->headers.clear();
  p->externalImports.clear();
  p->moduleNameMap.clear();
  p->moduleFileMap.clear();
}

void ModuleManager::addHeader(const QCString &moduleFile,int line,const QCString &headerName,bool isSystem)
{
  AUTO_TRACE("{}:{} headerName={} isSystem={}",moduleFile,line,headerName,isSystem);
  std::lock_guard lock(p->mutex);
  auto mod = p->moduleFileMap.find(moduleFile);
  if (mod)
  {
    toModuleDefImpl(mod)->addHeader(line,headerName,isSystem);
  }
  else
  {
    AUTO_TRACE_ADD("imported header '{}' found in file '{}' that is not a module",headerName,moduleFile);
  }
  p->headers.emplace_back(moduleFile,headerName,isSystem);
}

void ModuleManager::addImport(const QCString &moduleFile,int line,const QCString &importName,
                              bool isExported,const QCString &partitionName)
{
  AUTO_TRACE("{}:{} importName={},isExported={},partitionName={}",
      moduleFile,line,importName,isExported,partitionName);
  std::lock_guard lock(p->mutex);
  auto mod = p->moduleFileMap.find(moduleFile);
  if (mod) // import inside a module
  {
    AUTO_TRACE_ADD("in module");
    toModuleDefImpl(mod)->addImport(line,importName.isEmpty()?mod->name():importName,partitionName,isExported);
  }
  else // import outside of a module
  {
    AUTO_TRACE_ADD("outside module");
    p->externalImports.emplace(moduleFile.str(),ImportInfo(nullptr,importName,line,partitionName));
  }
}

void ModuleManager::addClassToModule(const Entry *root,ClassDef *cd)
{
  std::lock_guard lock(p->mutex);
  auto mod = p->moduleFileMap.find(root->fileName);
  if (mod)
  {
    toModuleDefImpl(mod)->addClassToModule(root,cd);
    auto cdm = toClassDefMutable(cd);
    if (cdm) cdm->setModuleDef(mod);
  }
}

void ModuleManager::addConceptToModule(const Entry *root,ConceptDef *cd)
{
  std::lock_guard lock(p->mutex);
  auto mod = p->moduleFileMap.find(root->fileName);
  if (mod)
  {
    toModuleDefImpl(mod)->addConceptToModule(root,cd);
    auto cdm = toConceptDefMutable(cd);
    if (cdm) cdm->setModuleDef(mod);
  }
}

void ModuleManager::addMemberToModule(const Entry *root,MemberDef *md)
{
  std::lock_guard lock(p->mutex);
  auto mod = p->moduleFileMap.find(root->fileName);
  if (mod && root->exported)
  {
    toModuleDefImpl(mod)->addMemberToModule(root,md);
    auto mdm = toMemberDefMutable(md);
    if (mdm) mdm->setModuleDef(mod);
  }
}

void ModuleManager::addTagInfo(const QCString &fileName,const QCString &tagFile,const QCString &clangId)
{
  std::lock_guard lock(p->mutex);
  auto mod = p->moduleFileMap.find(fileName);
  if (mod)
  {
    ModuleDefImpl *modi = toModuleDefImpl(mod);
    modi->setReference(tagFile);
    modi->setId(clangId);
  }
}

void ModuleManager::resolvePartitionsRecursively(ModuleDef *intfMod, ModuleDef *mod)
{
  AUTO_TRACE();
  for (auto &[partitionFileName,importInfo] : mod->getImports()) // foreach import
  {
    AUTO_TRACE_ADD("partitionFileName={} importName={} partitionName={}",
        partitionFileName,importInfo.importName,importInfo.partitionName);
    if (importInfo.importName==intfMod->name() && !importInfo.partitionName.isEmpty() &&
        importInfo.exported) // that is an exported partition of this module
    {
      auto it = p->moduleNameMap.find(importInfo.importName.str());
      if (it != p->moduleNameMap.end())
      {
        for (auto importedMod : it->second)
        {
          if (importedMod->qualifiedName()==importInfo.importName+":"+importInfo.partitionName)
          {
            AUTO_TRACE_ADD("Interface module {} exports partition {}:{}",
                mod->name(),importedMod->name(),importedMod->partitionName());
            toModuleDefImpl(intfMod)->addPartition(toModuleDefImpl(importedMod));
            toModuleDefImpl(importedMod)->setPrimaryInterface(intfMod);
            for (const auto &[partitionFileName_,partitionImportInfo] : importedMod->getImports())
            {
              if (partitionImportInfo.exported && intfMod->name()!=partitionImportInfo.importName)
              {
                toModuleDefImpl(intfMod)->addExportedModule(partitionImportInfo.importName,partitionImportInfo);
              }
            }
            resolvePartitionsRecursively(intfMod,importedMod);
          }
        }
      }
    }
  }
}

void ModuleManager::resolvePartitions()
{
  AUTO_TRACE();
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->moduleType()==ModuleDef::Type::Interface && mod->partitionName().isEmpty())
    { // that is a primary interface
      resolvePartitionsRecursively(mod.get(),mod.get());
    }

    // copy exported imports to m_exportedModules
    for (const auto &[fileName,importInfo] : mod->getImports())
    {
      if (importInfo.exported && mod->name()!=importInfo.importName)
      {
        toModuleDefImpl(mod)->addExportedModule(importInfo.importName,importInfo);
      }
    }

    // also link the ModuleDef and FileDef together
    bool ambig = false;
    FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap,mod->getDefFileName(),ambig);
    if (fd)
    {
      fd->setModuleDef(mod.get());
      toModuleDefImpl(mod)->setFileDef(fd);
    }
  }
}

void ModuleManager::resolveImports()
{
  AUTO_TRACE();
  for (auto &mod : p->moduleFileMap)
  {
    FileDef *fd = mod->getFileDef();
    if (fd)
    {
      for (const auto &[fileName,importInfo] : mod->getImports())
      {
        ModuleDef *importedModule = getPrimaryInterface(importInfo.importName);
        const FileDef *importedFd = importedModule ? importedModule->getFileDef() : nullptr;
        AUTO_TRACE_ADD("module: addIncludeDependency {}->{}:{} fd={}",
             mod->qualifiedName(), importInfo.qualifiedName(), importInfo.line, fd?fd->absFilePath():"");
        fd->addIncludeDependency(importedFd,importInfo.qualifiedName(),IncludeKind::ImportModule);
      }
    }
  }
  for (const auto &[fileName,importInfo] : p->externalImports)
  {
    bool ambig = false;
    FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap,QCString(fileName),ambig);
    if (fd)
    {
      ModuleDef *mod = getPrimaryInterface(importInfo.importName);
      FileDef *importedFd = mod ? mod->getFileDef() : nullptr;
      fd->addIncludeDependency(importedFd,importInfo.importName,IncludeKind::ImportModule);
    }
  }
  for (const auto &headerInfo : p->headers)
  {
    bool ambig = false;
    FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap,headerInfo.fileName,ambig);
    if (fd)
    {
      QCString resolvedHeader = determineAbsoluteIncludeName(headerInfo.fileName,headerInfo.headerName);
      FileDef *importFd = findFileDef(Doxygen::inputNameLinkedMap,resolvedHeader,ambig);
      fd->addIncludeDependency(importFd, headerInfo.headerName,
                               headerInfo.isSystem ? IncludeKind::ImportSystem : IncludeKind::ImportLocal);
    }
  }
}

void ModuleManager::collectExportedSymbolsRecursively(ModuleDef *intfMod, ModuleDef *partitionMod)
{
  AUTO_TRACE("{}: collecting symbols for partition {}",intfMod->qualifiedName(),partitionMod->qualifiedName());
  auto intfModImpl      = toModuleDefImpl(intfMod);
  auto partitionModImpl = toModuleDefImpl(partitionMod);
  intfModImpl->mergeSymbolsFrom(partitionModImpl);
}

void ModuleManager::collectExportedSymbols()
{
  AUTO_TRACE();
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) // that is a primary interface
    {
      for (auto &[partitionName,partitionMod] : mod->partitions())
      {
        collectExportedSymbolsRecursively(mod.get(),partitionMod);
      }

      // collect all files that contribute to this module (e.g. implementation/partition modules)
      auto it = p->moduleNameMap.find(mod->name().str());
      if (it != p->moduleNameMap.end())
      {
        for (auto contributingMod : it->second)
        {
          AUTO_TRACE_ADD("  adding contributing module {} to interface module {} type={} partition={} isPrimaryIntf={}",
              contributingMod->qualifiedName(),
              mod->name(),
              contributingMod->moduleType()==ModuleDef::Type::Interface ? "Interface" : "Implementation",
              contributingMod->partitionName(),
              contributingMod->isPrimaryInterface());
          toModuleDefImpl(mod)->addContributingModule(toModuleDefImpl(contributingMod));
        }
      }
    }
  }
}

void ModuleManager::countMembers()
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    toModuleDefImpl(mod)->countMembers();
  }
}

void ModuleManager::writeDocumentation(OutputList &ol)
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface())
    {
      mod->writeDocumentation(ol);
    }
  }
}

int ModuleManager::numDocumentedModules() const
{
  int count=0;
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) count++;
  }
  return count;
}

const ModuleLinkedMap &ModuleManager::modules() const
{
  return p->moduleFileMap;
}

void ModuleManager::addDocs(const Entry *root)
{
  AUTO_TRACE("file={} module={}",root->fileName,root->name);
  if (root->doc.isEmpty() && root->brief.isEmpty()) return;
  if (root->name.find(':')!=-1)
  {
    warn(root->fileName,root->startLine,"Ignoring documentation for module partition %s. Please place documentation at the primary module name",
        qPrint(root->name));
  }
  else
  {
    auto it = p->moduleNameMap.find(root->name.str());
    if (it != p->moduleNameMap.end())
    {
      ModuleDef *mod = getPrimaryInterface(root->name);
      if (mod)
      {
        mod->setDocumentation(root->doc,root->docFile,root->docLine);
        mod->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        mod->setId(root->id);
        mod->setHidden(root->hidden);
        mod->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
        mod->setRefItems(root->sli);
        //mod->addSectionsToDefinition(root->anchors);
        addModuleToGroups(root,mod);
      }
      else
      {
        warn(root->fileName,root->startLine,"Found documentation for module %s but it has no primary interface unit.",qPrint(root->name));
      }
    }
    else
    {
      warn(root->fileName,root->startLine,"Found documentation for unknown module %s.",qPrint(root->name));
    }
  }
}

ModuleDef *ModuleManager::getPrimaryInterface(const QCString &moduleName) const
{
  auto it = p->moduleNameMap.find(moduleName.str());
  if (it != p->moduleNameMap.end())
  {
    for (auto &mod : it->second)
    {
      if (mod->isPrimaryInterface())
      {
        return mod;
      }
    }
  }
  return nullptr;
}

void ModuleManager::addListReferences()
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) toModuleDefImpl(mod)->addListReferences();
  }
}

void ModuleManager::addMembersToMemberGroup()
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) toModuleDefImpl(mod)->addMembersToMemberGroup();
  }
}

void ModuleManager::distributeMemberGroupDocumentation()
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) toModuleDefImpl(mod)->distributeMemberGroupDocumentation();
  }
}

void ModuleManager::findSectionsInDocumentation()
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) toModuleDefImpl(mod)->findSectionsInDocumentation();
  }
}

void ModuleManager::sortMemberLists()
{
  for (auto &mod : p->moduleFileMap) // foreach module
  {
    if (mod->isPrimaryInterface()) toModuleDefImpl(mod)->sortMemberLists();
  }
}

