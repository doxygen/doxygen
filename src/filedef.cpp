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

#include <unordered_set>

#include "memberlist.h"
#include "classlist.h"
#include "filedef.h"
#include "doxygen.h"
#include "memberdef.h"
#include "classdef.h"
#include "namespacedef.h"
#include "util.h"
#include "language.h"
#include "outputlist.h"
#include "dot.h"
#include "dotincldepgraph.h"
#include "message.h"
#include "docparser.h"
#include "searchindex.h"
#include "htags.h"
#include "parserintf.h"
#include "portable.h"
#include "vhdldocgen.h"
#include "debug.h"
#include "layout.h"
#include "entry.h"
#include "groupdef.h"
#include "filename.h"
#include "membergroup.h"
#include "dirdef.h"
#include "config.h"
#include "clangparser.h"
#include "settings.h"
#include "definitionimpl.h"
#include "conceptdef.h"
#include "outputlist.h"
#include "moduledef.h"

//---------------------------------------------------------------------------

QCString includeStatement(SrcLangExt lang,IncludeKind kind)
{
  bool isIDLorJava = lang==SrcLangExt::IDL || lang==SrcLangExt::Java;
  if (isIDLorJava || (kind & IncludeKind_ImportMask))
  {
    return "import ";
  }
  else if (kind & IncludeKind_ObjCMask)
  {
    return "#import ";
  }
  else
  {
    return "#include ";
  }
}

QCString includeOpen(SrcLangExt lang,IncludeKind kind)
{
  if (lang==SrcLangExt::Java || kind==IncludeKind::ImportModule) return "";
  if (kind & IncludeKind_LocalMask)
  {
    return "\"";
  }
  else
  {
    return "<";
  }
}

QCString includeClose(SrcLangExt lang,IncludeKind kind)
{
  if (lang==SrcLangExt::Java || lang==SrcLangExt::IDL) return ";";
  switch (kind)
  {
    case IncludeKind::ImportLocal:       return "\";";
    case IncludeKind::ImportLocalObjC:   return "\"";
    case IncludeKind::IncludeLocal:      return "\"";
    case IncludeKind::ImportSystem:      return ">;";
    case IncludeKind::ImportSystemObjC:  return ">";
    case IncludeKind::IncludeSystem:     return ">";
    case IncludeKind::ImportModule:      return ";";
  }
  return "";
}

QCString includeTagFileAttributes(SrcLangExt lang,IncludeKind kind)
{
  QCString result;
  bool isIDLorJava = lang==SrcLangExt::IDL || lang==SrcLangExt::Java;
  result.sprintf("local=\"%s\" import=\"%s\" module=\"%s\" objc=\"%s\"",
           (kind & IncludeKind_LocalMask)  ? "yes" : "no",
           (isIDLorJava || (kind & IncludeKind_ImportMask)) ? "yes" : "no",
           (kind==IncludeKind::ImportModule) ? "yes" : "no",
           (kind & IncludeKind_ObjCMask) ? "yes" : "no");
  return result;
}

//---------------------------------------------------------------------------

using DefinitionLineMap = std::unordered_map<int,const Definition *>;
using MemberDefLineMap  = std::unordered_map<int,const MemberDef *>;
using IncludeInfoMap = std::unordered_map<std::string, const IncludeInfo *>;

class FileDefImpl : public DefinitionMixin<FileDef>
{
  public:
    FileDefImpl(const QCString &p,const QCString &n,const QCString &ref=QCString(),const QCString &dn=QCString());
    ~FileDefImpl() override;
    NON_COPYABLE(FileDefImpl)

    DefType definitionType() const override { return TypeFile; }
    CodeSymbolType codeSymbolType() const override { return CodeSymbolType::Default; }
    const QCString &name() const override;

    QCString displayName(bool=TRUE) const override { return localName(); }
    QCString fileName() const override { return m_fileName; }
    QCString getOutputFileBase() const override;
    QCString anchor() const override { return QCString(); }
    QCString getSourceFileBase() const override;
    QCString includeName() const override;
    QCString includeDependencyGraphFileName() const override;
    QCString includedByDependencyGraphFileName() const override;
    QCString absFilePath() const override { return m_filePath; }
    const QCString &docName() const override { return m_docname; }
    bool isSource() const override { return m_isSource; }
    bool isDocumentationFile() const override;
    const Definition *getSourceDefinition(int lineNr) const override;
    const MemberDef *getSourceMember(int lineNr) const override;
    QCString getPath() const override { return m_path; }
    QCString getVersion() const override { return m_fileVersion; }
    bool isLinkableInProject() const override;
    bool isLinkable() const override { return isLinkableInProject() || isReference(); }
    bool isIncluded(const QCString &name) const override;
    DirDef *getDirDef() const override       { return m_dir; }
    ModuleDef *getModuleDef() const override { return m_moduleDef; }
    const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const override;
    const LinkedRefMap<const Definition> &getUsedDefinitions() const override  { return m_usingDeclList; }
    const IncludeInfoList &includeFileList() const override    { return m_includeList; }
    const IncludeInfoList &includedByFileList() const override { return m_includedByList; }
    void getAllIncludeFilesRecursively(StringVector &incFiles) const override;
    MemberList *getMemberList(MemberListType lt) const override;
    const MemberLists &getMemberLists() const override { return m_memberLists; }
    const MemberGroupList &getMemberGroups() const override  { return m_memberGroups; }
    const NamespaceLinkedRefMap &getNamespaces() const override    { return m_namespaces; }
    const ConceptLinkedRefMap &getConcepts() const override        { return m_concepts; }
    const ClassLinkedRefMap &getClasses() const override           { return m_classes; }
    QCString title() const override;
    bool hasDetailedDescription() const override;
    QCString fileVersion() const override;
    bool subGrouping() const override { return m_subGrouping; }
    void countMembers() override;
    int numDocMembers() const override;
    int numDecMembers() const override;
    void addSourceRef(int line,const Definition *d,const MemberDef *md) override;
    void writeDocumentation(OutputList &ol) override;
    void writeMemberPages(OutputList &ol) override;
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const override;
    void writeSummaryLinks(OutputList &ol) const override;
    void writeTagFile(TextStream &t) override;
    void writeSourceHeader(OutputList &ol) override;
    void writeSourceBody(OutputList &ol,ClangTUParser *clangParser) override;
    void writeSourceFooter(OutputList &ol) override;
    void parseSource(ClangTUParser *clangParser) override;
    void setDiskName(const QCString &name) override;
    void insertMember(MemberDef *md) override;
    void removeMember(MemberDef *md) override;
    void insertClass(ClassDef *cd) override;
    void insertConcept(ConceptDef *cd) override;
    void insertNamespace(NamespaceDef *nd) override;
    void computeAnchors() override;
    void setDirDef(DirDef *dd) override { m_dir=dd; }
    void setModuleDef(ModuleDef *mod) override { m_moduleDef=mod; }
    void addUsingDirective(NamespaceDef *nd) override;
    void addUsingDeclaration(const Definition *d) override;
    void combineUsingRelations() override;
    bool generateSourceFile() const override;
    void sortMemberLists() override;
    void addIncludeDependency(const FileDef *fd,const QCString &incName,IncludeKind kind) override;
    void addIncludedByDependency(const FileDef *fd,const QCString &incName,IncludeKind kind) override;
    void addMembersToMemberGroup() override;
    void distributeMemberGroupDocumentation() override;
    void findSectionsInDocumentation() override;
    void addIncludedUsingDirectives(FileDefSet &visitedFiles) override;
    void addListReferences() override;

    bool hasIncludeGraph() const override;
    bool hasIncludedByGraph() const override;
    void overrideIncludeGraph(bool e) override;
    void overrideIncludedByGraph(bool e) override;

  private:
    void setDiskNameLocal(const QCString &name);
    void acquireFileVersion();
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void writeIncludeFiles(OutputList &ol);
    void writeIncludeGraph(OutputList &ol);
    void writeIncludedByGraph(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void writeSourceLink(OutputList &ol);
    void writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool isConstantGroup);
    void writeClassDeclarations(OutputList &ol,const QCString &title,const ClassLinkedRefMap &list);
    void writeConcepts(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void writeClassesToTagFile(TextStream &t,const ClassLinkedRefMap &list);
    void removeMemberFromList(MemberListType lt,MemberDef *md);

    IncludeInfoMap        m_includeMap;
    IncludeInfoList       m_includeList;
    IncludeInfoMap        m_includedByMap;
    IncludeInfoList       m_includedByList;
    LinkedRefMap<NamespaceDef> m_usingDirList;
    LinkedRefMap<const Definition> m_usingDeclList;
    QCString              m_path;
    QCString              m_filePath;
    QCString              m_inclDepFileName;
    QCString              m_inclByDepFileName;
    QCString              m_outputDiskName;
    QCString              m_fileName;
    QCString              m_docname;
    DefinitionLineMap     m_srcDefMap;
    MemberDefLineMap      m_srcMemberMap;
    bool                  m_isSource;
    QCString              m_fileVersion;
    DirDef               *m_dir = nullptr;
    ModuleDef            *m_moduleDef = nullptr;
    MemberLists           m_memberLists;
    MemberGroupList       m_memberGroups;
    NamespaceLinkedRefMap m_namespaces;
    ClassLinkedRefMap     m_classes;
    ClassLinkedRefMap     m_interfaces;
    ClassLinkedRefMap     m_structs;
    ClassLinkedRefMap     m_exceptions;
    ConceptLinkedRefMap   m_concepts;
    bool                  m_subGrouping;
    bool                  m_hasIncludeGraph = Config_getBool(INCLUDE_GRAPH);
    bool                  m_hasIncludedByGraph = Config_getBool(INCLUDED_BY_GRAPH);
};

std::unique_ptr<FileDef> createFileDef(const QCString &p,const QCString &n,const QCString &ref,const QCString &dn)
{
  return std::make_unique<FileDefImpl>(p,n,ref,dn);
}


//---------------------------------------------------------------------------

/*! create a new file definition, where \a p is the file path,
    \a nm the file name, and \a lref is an HTML anchor name if the
    file was read from a tag file or nullptr otherwise
*/
FileDefImpl::FileDefImpl(const QCString &p,const QCString &nm,
                 const QCString &lref,const QCString &dn)
   : DefinitionMixin(QCString(p)+nm,1,1,nm,nullptr,nullptr,!p.isEmpty())
{
  m_path=removeLongPathMarker(p);
  m_filePath=p+nm;
  m_fileName=nm;

  setReference(lref);
  setDiskNameLocal(!dn.isEmpty() ? dn : nm);
  m_isSource          = guessSection(nm).isSource();
  m_docname           = nm;
  m_dir               = nullptr;
  if (Config_getBool(FULL_PATH_NAMES))
  {
    m_docname.prepend(stripFromPath(m_path));
  }
  setLanguage(getLanguageFromFileName(
     Config_getBool(FULL_PATH_NAMES) ? m_fileName : DefinitionMixin::name()));
  acquireFileVersion();
  m_subGrouping=Config_getBool(SUBGROUPING);
}

/*! destroy the file definition */
FileDefImpl::~FileDefImpl()
{
}

void FileDefImpl::setDiskNameLocal(const QCString &name)
{
  if (isReference())
  {
    m_outputDiskName = name;
    m_inclDepFileName = name+"_incl";
    m_inclByDepFileName = name+"_dep_incl";
  }
  else
  {
    m_outputDiskName = convertNameToFile(name);
    m_inclDepFileName = convertNameToFile(name+"_incl");
    m_inclByDepFileName = convertNameToFile(name+"_dep_incl");
  }
}

void FileDefImpl::setDiskName(const QCString &name)
{
  setDiskNameLocal(name);
}

/*! Compute the HTML anchor names for all members in the class */
void FileDefImpl::computeAnchors()
{
  MemberList *ml = getMemberList(MemberListType::AllMembersList());
  if (ml) ml->setAnchors();
}

void FileDefImpl::distributeMemberGroupDocumentation()
{
  //printf("FileDefImpl::distributeMemberGroupDocumentation()\n");
  for (const auto &mg : m_memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void FileDefImpl::findSectionsInDocumentation()
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

bool FileDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().stripWhiteSpace().isEmpty() || // avail empty section
          (sourceBrowser && getStartBodyLine()!=-1 && getBodyDef())
         );
}

void FileDefImpl::writeTagFile(TextStream &tagFile)
{
  QCString fn=getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "  <compound kind=\"file\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <path>" << convertToXML(stripFromPath(getPath())) << "</path>\n";
  tagFile << "    <filename>" << fn << "</filename>\n";
  for (const auto &ii : m_includeList)
  {
    const FileDef *fd=ii.fileDef;
    if (fd && fd->isLinkable() && !fd->isReference())
    {
      QCString attr = includeTagFileAttributes(fd->getLanguage(),ii.kind);
      tagFile << "    <includes id=\""
        << convertToXML(fd->getOutputFileBase()) << "\" "
        << "name=\"" << convertToXML(fd->name()) << "\" "
        << attr << ">"
        << convertToXML(ii.includeName)
        << "</includes>\n";
    }
  }
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::File))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::FileClasses:
        {
          writeClassesToTagFile(tagFile, m_classes);
        }
        break;
      case LayoutDocEntry::FileInterfaces:
        {
          writeClassesToTagFile(tagFile, m_interfaces);
        }
        break;
      case LayoutDocEntry::FileStructs:
        {
          writeClassesToTagFile(tagFile, m_structs);
        }
        break;
      case LayoutDocEntry::FileExceptions:
        {
          writeClassesToTagFile(tagFile, m_exceptions);
        }
        break;
      case LayoutDocEntry::FileConcepts:
        {
          for (const auto *nd : m_concepts)
          {
            if (nd->isLinkableInProject())
            {
              tagFile << "    <concept>" << convertToXML(nd->name()) << "</concept>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::FileNamespaces:
        {
          for (const auto *nd : m_namespaces)
          {
            if (nd->isLinkableInProject())
            {
              tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>\n";
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
              ml->writeTagFile(tagFile,false,false);
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

void FileDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
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
    //printf("Writing source ref for file %s\n",qPrint(name()));
    if (Config_getBool(SOURCE_BROWSER))
    {
      ol.startParagraph("definition");
      QCString refText = theTranslator->trDefinedInSourceFile();
      int fileMarkerPos = refText.find("@0");
      if (fileMarkerPos!=-1) // should always pass this.
      {
        ol.parseText(refText.left(fileMarkerPos)); //text left from marker 1
        ol.writeObjectLink(QCString(),getSourceFileBase(),QCString(),name());
        ol.parseText(refText.right(
              refText.length()-fileMarkerPos-2)); // text right from marker 2
      }
      else
      {
        err("translation error: invalid marker in trDefinedInSourceFile()\n");
      }
      ol.endParagraph();
    }
    ol.endTextBlock();
  }
}

void FileDefImpl::writeBriefDescription(OutputList &ol)
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

      if (Config_getBool(REPEAT_BRIEF) ||
          !documentation().stripWhiteSpace().isEmpty()
         )
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

void FileDefImpl::writeClassesToTagFile(TextStream &tagFile, const ClassLinkedRefMap &list)
{
  for (const auto &cd : list)
  {
    if (cd->isLinkableInProject())
    {
      tagFile << "    <class kind=\"" << cd->compoundTypeString() <<
        "\">" << convertToXML(cd->name()) << "</class>\n";
    }
  }
}

void FileDefImpl::writeIncludeFiles(OutputList &ol)
{
  if (!m_includeList.empty())
  {
    ol.startTextBlock(TRUE);
    for (const auto &ii : m_includeList)
    {
      const FileDef *fd=ii.fileDef;
      ol.startTypewriter();
      SrcLangExt lang = fd ? fd->getLanguage() : SrcLangExt::Cpp;
      ol.docify(::includeStatement(lang,ii.kind));
      ol.docify(::includeOpen(lang,ii.kind));
      ol.disable(OutputType::Html);
      ol.docify(ii.includeName);
      ol.enableAll();
      ol.disableAllBut(OutputType::Html);

      // Here we use the include file name as it appears in the file.
      // we could also we the name as it is used within doxygen,
      // then we should have used fd->docName() instead of ii->includeName
      if (fd && fd->isLinkable())
      {
        ol.writeObjectLink(fd->getReference(),
            fd->generateSourceFile() ? fd->includeName() : fd->getOutputFileBase(),
            QCString(),ii.includeName);
      }
      else
      {
        ModuleDef *mod = ModuleManager::instance().getPrimaryInterface(ii.includeName);
        if (ii.kind==IncludeKind::ImportModule && mod && mod->isLinkable())
        {
          ol.writeObjectLink(mod->getReference(),mod->getOutputFileBase(),
                             QCString(),ii.includeName);
        }
        else
        {
          ol.docify(ii.includeName);
        }
      }

      ol.enableAll();
      ol.docify(::includeClose(lang,ii.kind));
      ol.endTypewriter();
      ol.lineBreak();
    }
    ol.endTextBlock();
  }
}

void FileDefImpl::writeIncludeGraph(OutputList &ol)
{
  if (Config_getBool(HAVE_DOT) && m_hasIncludeGraph /*&& Config_getBool(INCLUDE_GRAPH)*/)
  {
    //printf("Graph for file %s\n",qPrint(name()));
    DotInclDepGraph incDepGraph(this,FALSE);
    if (incDepGraph.isTooBig())
    {
       warn_uncond("Include graph for '%s' not generated, too many nodes (%d), threshold is %d. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qPrint(name()), incDepGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock();
      ol.disable(OutputType::Man);
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclDepGraph(name()));
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
    }
    //incDepGraph.writeGraph(Config_getString(HTML_OUTPUT),fd->getOutputFileBase());
  }
}

void FileDefImpl::writeIncludedByGraph(OutputList &ol)
{
  if (Config_getBool(HAVE_DOT) && m_hasIncludedByGraph /*&& Config_getBool(INCLUDED_BY_GRAPH)*/)
  {
    //printf("Graph for file %s\n",qPrint(name()));
    DotInclDepGraph incDepGraph(this,TRUE);
    if (incDepGraph.isTooBig())
    {
       warn_uncond("Included by graph for '%s' not generated, too many nodes (%d), threshold is %d. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           qPrint(name()), incDepGraph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!incDepGraph.isTrivial())
    {
      ol.startTextBlock();
      ol.disable(OutputType::Man);
      ol.startInclDepGraph();
      ol.parseText(theTranslator->trInclByDepGraph());
      ol.endInclDepGraph(incDepGraph);
      ol.enableAll();
      ol.endTextBlock(TRUE);
    }
    //incDepGraph.writeGraph(Config_getString(HTML_OUTPUT),fd->getOutputFileBase());
  }
}


void FileDefImpl::writeSourceLink(OutputList &ol)
{
  //printf("%s: generateSourceFile()=%d\n",qPrint(name()),generateSourceFile());
  if (generateSourceFile())
  {
    ol.disableAllBut(OutputType::Html);
    ol.startParagraph();
    ol.startTextLink(includeName(),QCString());
    ol.parseText(theTranslator->trGotoSourceCode());
    ol.endTextLink();
    ol.endParagraph();
    ol.enableAll();
  }
}

void FileDefImpl::writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool const isConstantGroup)
{
  // write list of namespaces
  m_namespaces.writeDeclaration(ol,title,isConstantGroup);
}

void FileDefImpl::writeClassDeclarations(OutputList &ol,const QCString &title,const ClassLinkedRefMap &list)
{
  // write list of classes
  list.writeDeclaration(ol,nullptr,title,FALSE);
}

void FileDefImpl::writeConcepts(OutputList &ol,const QCString &title)
{
  // write list of classes
  m_concepts.writeDeclaration(ol,title,FALSE);
}

void FileDefImpl::writeInlineClasses(OutputList &ol)
{
  // temporarily undo the disabling could be done by startMemberDocumentation()
  // as a result of setting SEPARATE_MEMBER_PAGES to YES; see bug730512
  bool isEnabled = ol.isEnabled(OutputType::Html);
  ol.enable(OutputType::Html);

  m_classes.writeDocumentation(ol,this);

  // restore the initial state if needed
  if (!isEnabled) ol.disable(OutputType::Html);
}

void FileDefImpl::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void FileDefImpl::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void FileDefImpl::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.disable(OutputType::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void FileDefImpl::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputType::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void FileDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  for (const auto &mg : m_memberGroups)
  {
    if (!mg->allMembersInSameSection() || !m_subGrouping)
    {
      mg->writeDeclarations(ol,nullptr,nullptr,this,nullptr,nullptr);
    }
  }
}

void FileDefImpl::writeAuthorSection(OutputList &ol)
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

void FileDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=TRUE;
  SrcLangExt lang=getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::File))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    if (lde->kind()==LayoutDocEntry::FileClasses && m_classes.declVisible() && ls)
    {
      QCString label = "nested-classes";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileInterfaces && m_interfaces.declVisible() && ls)
    {
      QCString label = "interfaces";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileStructs && m_structs.declVisible() && ls)
    {
      QCString label = "structs";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileExceptions && m_exceptions.declVisible() && ls)
    {
      QCString label = "exceptions";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileNamespaces && m_namespaces.declVisible(false) && ls)
    {
      QCString label = "namespaces";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileConcepts && m_concepts.declVisible() && ls)
    {
      QCString label = "concepts";
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

/*! Write the documentation page for this file to the file of output
    generators \a ol.
*/
void FileDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  //funcList->countDecMembers();

  //QCString fn = name();
  //if (Config_getBool(FULL_PATH_NAMES))
  //{
  //  fn.prepend(stripFromPath(getPath()));
  //}

  //printf("WriteDocumentation diskname=%s\n",qPrint(diskname));

  QCString versionTitle;
  if (!m_fileVersion.isEmpty())
  {
    versionTitle=("("+m_fileVersion+")");
  }
  QCString title = m_docname+versionTitle;
  QCString pageTitle=theTranslator->trFileReference(m_docname);

  if (getDirDef())
  {
    startFile(ol,getOutputFileBase(),name(),pageTitle,HighlightedItem::FileVisible,!generateTreeView);
    if (!generateTreeView)
    {
      getDirDef()->writeNavigationPath(ol);
      ol.endQuickIndices();
    }
    startTitle(ol,getOutputFileBase(),this);
    ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.parseText(theTranslator->trFileReference(displayName())); // Html only
      ol.enableAll();
      ol.disable(OutputType::Html);
      ol.parseText(Config_getBool(FULL_PATH_NAMES) ?  // other output formats
                   pageTitle :
                   theTranslator->trFileReference(name()));
    ol.popGeneratorState();
    addGroupListToTitle(ol,this);
    endTitle(ol,getOutputFileBase(),title);
  }
  else
  {
    startFile(ol,getOutputFileBase(),name(),pageTitle,HighlightedItem::FileVisible,!generateTreeView);
    if (!generateTreeView)
    {
      ol.endQuickIndices();
    }
    startTitle(ol,getOutputFileBase(),this);
    ol.parseText(pageTitle);
    addGroupListToTitle(ol,this);
    endTitle(ol,getOutputFileBase(),title);
  }

  ol.startContents();

  if (!m_fileVersion.isEmpty())
  {
    ol.disableAllBut(OutputType::Html);
    ol.startProjectNumber();
    ol.docify(versionTitle);
    ol.endProjectNumber();
    ol.enableAll();
  }

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::File))
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
      case LayoutDocEntry::FileIncludes:
        writeIncludeFiles(ol);
        break;
      case LayoutDocEntry::FileIncludeGraph:
        writeIncludeGraph(ol);
        break;
      case LayoutDocEntry::FileIncludedByGraph:
        writeIncludedByGraph(ol);
        break;
      case LayoutDocEntry::FileSourceLink:
        writeSourceLink(ol);
        break;
      case LayoutDocEntry::FileClasses:
        if (ls) writeClassDeclarations(ol,ls->title(lang),m_classes);
        break;
      case LayoutDocEntry::FileInterfaces:
        if (ls) writeClassDeclarations(ol,ls->title(lang),m_interfaces);
        break;
      case LayoutDocEntry::FileStructs:
        if (ls) writeClassDeclarations(ol,ls->title(lang),m_structs);
        break;
      case LayoutDocEntry::FileExceptions:
        if (ls) writeClassDeclarations(ol,ls->title(lang),m_exceptions);
        break;
      case LayoutDocEntry::FileConcepts:
        if (ls) writeConcepts(ol,ls->title(lang));
        break;
      case LayoutDocEntry::FileNamespaces:
        if (ls) writeNamespaceDeclarations(ol,ls->title(lang),false);
        break;
      case LayoutDocEntry::FileConstantGroups:
        if (ls) writeNamespaceDeclarations(ol,ls->title(lang),true);
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
      case LayoutDocEntry::FileInlineClasses:
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
      case LayoutDocEntry::ModuleExports:
      case LayoutDocEntry::ModuleClasses:
      case LayoutDocEntry::ModuleConcepts:
      case LayoutDocEntry::ModuleUsedFiles:
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::File entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(ol,this);

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    MemberList *ml = getMemberList(MemberListType::AllMembersList());
    if (ml) ml->sort();
    writeMemberPages(ol);
  }
}

void FileDefImpl::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  for (const auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
      ml->writeDocumentationPage(ol,name(),this);
    }
  }

  ol.popGeneratorState();
}

void FileDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
  if (allMemberList)
  {
    for (const auto &md : *allMemberList)
    {
      if (md->getFileDef()==this && md->getNamespaceDef()==nullptr && md->isLinkable() && !md->isEnumValue())
      {
        if (md->isLinkableInProject())
        {
          QCString fn=md->getOutputFileBase();
          addHtmlExtensionIfMissing(fn);
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
          ol.writeString(fn+"#"+md->anchor());
          ol.writeString("\">");
          ol.writeString(convertToHtml(md->localName()));
          ol.writeString("</a>");
          ol.writeString("</td></tr>\n");
        }
      }
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}

/*! Write a source listing of this file to the output */
void FileDefImpl::writeSourceHeader(OutputList &ol)
{
  bool generateTreeView  = Config_getBool(GENERATE_TREEVIEW);
  QCString title = m_docname;
  if (!m_fileVersion.isEmpty())
  {
    title+=(" ("+m_fileVersion+")");
  }
  QCString pageTitle = theTranslator->trSourceFile(title);
  ol.disable(OutputType::Man);

  bool isDocFile = isDocumentationFile();
  bool genSourceFile = !isDocFile && generateSourceFile();
  if (getDirDef())
  {
    startFile(ol,getSourceFileBase(),QCString(),pageTitle,HighlightedItem::FileVisible,
        !generateTreeView,
        !isDocFile && genSourceFile ? QCString() : getOutputFileBase(),
        0);
    if (!generateTreeView)
    {
      getDirDef()->writeNavigationPath(ol);
      ol.endQuickIndices();
    }
    startTitle(ol,getSourceFileBase());
    ol.parseText(name());
    endTitle(ol,getSourceFileBase(),title);
  }
  else
  {
    startFile(ol,getSourceFileBase(),QCString(),pageTitle,HighlightedItem::FileVisible,false,
        !isDocFile && genSourceFile ? QCString() : getOutputFileBase(),
        0);
    startTitle(ol,getSourceFileBase());
    ol.parseText(title);
    endTitle(ol,getSourceFileBase(),QCString());
  }

  ol.startContents();

  if (isLinkable())
  {
    ol.startTextLink(getOutputFileBase(),QCString());
    ol.parseText(theTranslator->trGotoDocumentation());
    ol.endTextLink();

    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::RTF);
    ol.writeString("\\par\n");
    ol.popGeneratorState();
  }
}

void FileDefImpl::writeSourceBody(OutputList &ol,[[maybe_unused]] ClangTUParser *clangParser)
{
  bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  OutputCodeList devNullList;
  devNullList.add<DevNullCodeGenerator>();
#if USE_LIBCLANG
  if (Doxygen::clangAssistedParsing && clangParser &&
      (getLanguage()==SrcLangExt::Cpp || getLanguage()==SrcLangExt::ObjC))
  {
    auto &codeOL = ol.codeGenerators();
    codeOL.startCodeFragment("DoxyCode");
    clangParser->switchToFile(this);
    clangParser->writeSources(codeOL,this);
    codeOL.endCodeFragment("DoxyCode");
  }
  else
#endif
  {
    auto intf = Doxygen::parserManager->getCodeParser(getDefFileExtension());
    intf->resetCodeParserState();
    auto &codeOL = ol.codeGenerators();
    codeOL.startCodeFragment("DoxyCode");
    bool needs2PassParsing =
        Doxygen::parseSourcesNeeded &&                // we need to parse (filtered) sources for cross-references
        !filterSourceFiles &&                         // but user wants to show sources as-is
        !getFileFilter(absFilePath(),TRUE).isEmpty(); // and there is a filter used while parsing

    if (needs2PassParsing)
    {
      // parse code for cross-references only (see bug707641)
      intf->parseCode(devNullList,QCString(),
                       fileToString(absFilePath(),TRUE,TRUE),
                       getLanguage(),
                       Config_getBool(STRIP_CODE_COMMENTS),
                       FALSE,QCString(),this
                      );
    }
    intf->parseCode(codeOL,QCString(),
        fileToString(absFilePath(),filterSourceFiles,TRUE),
        getLanguage(),      // lang
        Config_getBool(STRIP_CODE_COMMENTS),
        FALSE,              // isExampleBlock
        QCString(),         // exampleName
        this,               // fileDef
        -1,                 // startLine
        -1,                 // endLine
        FALSE,              // inlineFragment
        nullptr,                  // memberDef
        TRUE,               // showLineNumbers
        nullptr,                  // searchCtx
        !needs2PassParsing  // collectXRefs
        );
    codeOL.endCodeFragment("DoxyCode");
  }
}

void FileDefImpl::writeSourceFooter(OutputList &ol)
{
  ol.endContents();
  endFileWithNavPath(ol,this);
  ol.enableAll();
}

void FileDefImpl::parseSource([[maybe_unused]] ClangTUParser *clangParser)
{
  bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  OutputCodeList devNullList;
  devNullList.add<DevNullCodeGenerator>();
#if USE_LIBCLANG
  if (Doxygen::clangAssistedParsing && clangParser &&
      (getLanguage()==SrcLangExt::Cpp || getLanguage()==SrcLangExt::ObjC))
  {
    clangParser->switchToFile(this);
    clangParser->writeSources(devNullList,this);
  }
  else
#endif
  {
    auto intf = Doxygen::parserManager->getCodeParser(getDefFileExtension());
    intf->resetCodeParserState();
    intf->parseCode(
            devNullList,QCString(),
            fileToString(absFilePath(),filterSourceFiles,TRUE),
            getLanguage(),
            Config_getBool(STRIP_CODE_COMMENTS),
            FALSE,QCString(),this
           );
  }
}

void FileDefImpl::addMembersToMemberGroup()
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

/*! Adds member definition \a md to the list of all members of this file */
void FileDefImpl::insertMember(MemberDef *md)
{
  if (md->isHidden()) return;
  //printf("%s:FileDefImpl::insertMember(%s (=%p) list has %d elements)\n",
  //    qPrint(name()),qPrint(md->name()),md,allMemberList.count());
  MemberList *allMemberList = getMemberList(MemberListType::AllMembersList());
  if (allMemberList && allMemberList->contains(md))
  {
    return;
  }

  if (allMemberList==nullptr)
  {
    m_memberLists.emplace_back(std::make_unique<MemberList>(MemberListType::AllMembersList(),MemberListContainer::File));
    allMemberList = m_memberLists.back().get();
  }
  allMemberList->push_back(md);
  switch (md->memberType())
  {
    case MemberType::Property:
      if (md->getLanguage() == SrcLangExt::Python)
      {
        addMemberToList(MemberListType::PropertyMembers(),md);
        addMemberToList(MemberListType::Properties(),md);
        break;
      }
      //  fallthrough, explicitly no break here
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
    case MemberType::EnumValue:    // enum values are shown inside their enums
      break;
    case MemberType::Define:
      addMemberToList(MemberListType::DecDefineMembers(),md);
      addMemberToList(MemberListType::DocDefineMembers(),md);
      break;
    default:
       err("FileDefImpl::insertMembers(): "
           "member '%s' with class scope '%s' inserted in file scope '%s'!\n",
           qPrint(md->name()),
           md->getClassDef() ? qPrint(md->getClassDef()->name()) : "<global>",
           qPrint(name()));
  }
  //addMemberToGroup(md,groupId);
}

void FileDefImpl::removeMemberFromList(MemberListType lt,MemberDef *md)
{
  MemberList *ml = getMemberList(lt);
  if (ml) ml->remove(md);
}

void FileDefImpl::removeMember(MemberDef *md)
{
  removeMemberFromList(MemberListType::AllMembersList(),md);
  switch(md->memberType())
  {
    case MemberType::Property:
      if (md->getLanguage() == SrcLangExt::Python)
      {
        removeMemberFromList(MemberListType::PropertyMembers(),md);
        removeMemberFromList(MemberListType::Properties(),md);
        break;
      }
      //  fallthrough, explicitly no break here
    case MemberType::Variable:
      removeMemberFromList(MemberListType::DecVarMembers(),md);
      removeMemberFromList(MemberListType::DocVarMembers(),md);
      break;
    case MemberType::Function:
      removeMemberFromList(MemberListType::DecFuncMembers(),md);
      removeMemberFromList(MemberListType::DocFuncMembers(),md);
      break;
    case MemberType::Typedef:
      removeMemberFromList(MemberListType::DecTypedefMembers(),md);
      removeMemberFromList(MemberListType::DocTypedefMembers(),md);
      break;
    case MemberType::Sequence:
      removeMemberFromList(MemberListType::DecSequenceMembers(),md);
      removeMemberFromList(MemberListType::DocSequenceMembers(),md);
      break;
    case MemberType::Dictionary:
      removeMemberFromList(MemberListType::DecDictionaryMembers(),md);
      removeMemberFromList(MemberListType::DocDictionaryMembers(),md);
      break;
    case MemberType::Enumeration:
      removeMemberFromList(MemberListType::DecEnumMembers(),md);
      removeMemberFromList(MemberListType::DocEnumMembers(),md);
      break;
    case MemberType::EnumValue:    // enum values are shown inside their enums
      break;
    case MemberType::Define:
      removeMemberFromList(MemberListType::DecDefineMembers(),md);
      removeMemberFromList(MemberListType::DocDefineMembers(),md);
      break;
    default:
      err("FileDefImpl::removeMember(): unexpected member remove in file!\n");
  }
}

/*! Adds compound definition \a cd to the list of all compounds of this file */
void FileDefImpl::insertClass(ClassDef *cd)
{
  if (cd->isHidden()) return;

  ClassLinkedRefMap *list = &m_classes;

  if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
  {
    if (cd->compoundType()==ClassDef::Interface)
    {
      list = &m_interfaces;
    }
    else if (cd->compoundType()==ClassDef::Struct)
    {
      list = &m_structs;
    }
    else if (cd->compoundType()==ClassDef::Exception)
    {
      list = &m_exceptions;
    }
  }

  list->add(cd->name(),cd);
}

void FileDefImpl::insertConcept(ConceptDef *cd)
{
  if (cd->isHidden()) return;
  m_concepts.add(cd->name(),cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDefImpl::insertNamespace(NamespaceDef *nd)
{
  if (nd->isHidden()) return;
  m_namespaces.add(nd->name(),nd);
}

const QCString &FileDefImpl::name() const
{
  if (Config_getBool(FULL_PATH_NAMES))
    return m_fileName;
  else
    return DefinitionMixin::name();
}

void FileDefImpl::addSourceRef(int line,const Definition *d,const MemberDef *md)
{
  //printf("FileDefImpl::addSourceRef(%d,%s,%s)\n",line,qPrint(d?d->name():"nullptr"),qPrint(md?md->name():"nullptr"));
  if (d)
  {
    m_srcDefMap.emplace(line,d);
    if (md) m_srcMemberMap.emplace(line,md);
    //printf("Adding member %s with anchor %s at line %d to file %s\n",
    //    md?qPrint(md->name()):"<none>",md?qPrint(md->anchor()):"<none>",line,qPrint(name()));
  }
}

const Definition *FileDefImpl::getSourceDefinition(int lineNr) const
{
  auto it = m_srcDefMap.find(lineNr);
  //printf("%s::getSourceDefinition(%d)=%s\n",qPrint(name()),lineNr,it!=m_srcDefMap.end()?qPrint(it->second->name()):"none");
  return it!=m_srcDefMap.end() ? it->second : nullptr;
}

const MemberDef *FileDefImpl::getSourceMember(int lineNr) const
{
  auto it = m_srcMemberMap.find(lineNr);
  //printf("%s::getSourceMember(%d)=%s\n",qPrint(name()),lineNr,it!=m_srcMemberMap.end()?qPrint(it->second->name()):"none");
  return it!=m_srcMemberMap.end() ? it->second : nullptr;
}


void FileDefImpl::addUsingDirective(NamespaceDef *nd)
{
  m_usingDirList.add(nd->qualifiedName(),nd);
  //printf("%p: FileDefImpl::addUsingDirective: %s:%d\n",this,qPrint(name()),usingDirList->count());
}

const LinkedRefMap<NamespaceDef> &FileDefImpl::getUsedNamespaces() const
{
  //printf("%p: FileDefImpl::getUsedNamespace: %s:%d\n",this,qPrint(name()),usingDirList?usingDirList->count():0);
  return m_usingDirList;
}

void FileDefImpl::addUsingDeclaration(const Definition *d)
{
  m_usingDeclList.add(d->qualifiedName(),d);
}

void FileDefImpl::addIncludeDependency(const FileDef *fd,const QCString &incName,IncludeKind kind)
{
  //printf("FileDefImpl::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath() : incName;
  if (!iName.isEmpty() && m_includeMap.find(iName.str())==m_includeMap.end())
  {
    m_includeList.emplace_back(fd,incName,kind);
    m_includeMap.emplace(iName.str(),&m_includeList.back());
  }
}

void FileDefImpl::addIncludedUsingDirectives(FileDefSet &visitedFiles)
{
  if (visitedFiles.find(this)!=visitedFiles.end()) return; // file already processed
  visitedFiles.insert(this);
  //printf("( FileDefImpl::addIncludedUsingDirectives for file %s\n",qPrint(name()));

  if (!m_includeList.empty()) // file contains #includes
  {
    {
      for (const auto &ii : m_includeList) // foreach #include...
      {
        if (ii.fileDef) // ...that is a known file
        {
          // recurse into this file
          const_cast<FileDef*>(ii.fileDef)->addIncludedUsingDirectives(visitedFiles);
        }
      }
    }
    {
      // iterate through list from last to first
      for (auto ii_it = m_includeList.rbegin(); ii_it!=m_includeList.rend(); ++ii_it)
      {
        auto &ii = *ii_it;
        if (ii.fileDef && ii.fileDef!=this)
        {
          // add using directives
          auto unl = ii.fileDef->getUsedNamespaces();
          for (auto it = unl.rbegin(); it!=unl.rend(); ++it)
          {
            auto *nd = *it;
            //printf("  adding using directive for %s\n",qPrint(nd->qualifiedName()));
            m_usingDirList.prepend(nd->qualifiedName(),nd);
          }
          // add using declarations
          auto  udl = ii.fileDef->getUsedDefinitions();
          for (auto it = udl.rbegin(); it!=udl.rend(); ++it)
          {
            auto *d = *it;
            m_usingDeclList.prepend(d->qualifiedName(),d);
          }
        }
      }
    }
  }
  //printf(") end FileDefImpl::addIncludedUsingDirectives for file %s\n",qPrint(name()));
}


void FileDefImpl::addIncludedByDependency(const FileDef *fd,const QCString &incName,IncludeKind kind)
{
  //printf("FileDefImpl::addIncludedByDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath() : incName;
  if (!iName.isEmpty() && m_includedByMap.find(iName.str())==m_includedByMap.end())
  {
    m_includedByList.emplace_back(fd,incName,kind);
    m_includedByMap.emplace(iName.str(),&m_includedByList.back());
  }
}

bool FileDefImpl::isIncluded(const QCString &name) const
{
  if (name.isEmpty()) return FALSE;
  return m_includeMap.find(name.str())!=m_includeMap.end();
}

bool FileDefImpl::generateSourceFile() const
{
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  bool verbatimHeaders = Config_getBool(VERBATIM_HEADERS);
  return !isReference() &&
         (sourceBrowser ||
           (verbatimHeaders && (guessSection(name()).isHeader() || !m_includedByMap.empty()))
         ) &&
         !isDocumentationFile();
}


void FileDefImpl::addListReferences()
{
  {
    const RefItemVector &xrefItems = xrefListItems();
    addRefItem(xrefItems,
               getOutputFileBase(),
               theTranslator->trFile(TRUE,TRUE),
               getOutputFileBase(),name(),
               QCString(),
               nullptr
              );
  }
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

//-------------------------------------------------------------------

void FileDefImpl::combineUsingRelations()
{
  LinkedRefMap<NamespaceDef> usingDirList = m_usingDirList;
  NamespaceDefSet visitedNamespaces;
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

bool FileDefImpl::isDocumentationFile() const
{
  static const std::unordered_set<std::string> docExtensions =
  { "doc", "txt", "dox", "md", "markdown" };

  int lastDot = name().findRev('.');
  return (lastDot!=-1 && docExtensions.find(name().mid(lastDot+1).str())!=docExtensions.end()) ||
         getLanguageFromFileName(getFileNameExtension(name())) == SrcLangExt::Markdown;
}

void FileDefImpl::acquireFileVersion()
{
  QCString vercmd = Config_getString(FILE_VERSION_FILTER);
  if (!vercmd.isEmpty() && !m_filePath.isEmpty() && !isReference() &&
      m_filePath!="generated" && m_filePath!="graph_legend.dox")
  {
    msg("Version of %s : ",qPrint(m_filePath));
    QCString cmd = vercmd+" \""+m_filePath+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",qPrint(cmd));
    FILE *f=Portable::popen(cmd,"r");
    if (!f)
    {
      err("could not execute %s\n",qPrint(vercmd));
      return;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead = static_cast<int>(fread(buf,1,bufSize-1,f));
    Portable::pclose(f);
    if (numRead>0 && numRead<bufSize)
    {
      buf[numRead]='\0';
      m_fileVersion=QCString(buf,numRead).stripWhiteSpace();
      if (!m_fileVersion.isEmpty())
      {
        msg("%s\n",qPrint(m_fileVersion));
        return;
      }
    }
    msg("no version available\n");
  }
}


QCString FileDefImpl::getSourceFileBase() const
{
  if (Htags::useHtags)
  {
    return Htags::path2URL(m_filePath);
  }
  else
  {
    return m_outputDiskName+"_source";
  }
}

QCString FileDefImpl::getOutputFileBase() const
{
  return m_outputDiskName;
}

/*! Returns the name of the verbatim copy of this file (if any). */
QCString FileDefImpl::includeName() const
{
  return getSourceFileBase();
}

void FileDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_memberLists.get(lt,MemberListContainer::File);
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

void FileDefImpl::sortMemberLists()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }

  for (const auto &mg : m_memberGroups)
  {
    MemberList &mlg = const_cast<MemberList&>(mg->members());
    if (mlg.needsSorting()) { mlg.sort(); mlg.setNeedsSorting(FALSE); }
  }

  std::stable_sort(m_includedByList.begin(),m_includedByList.end(),
      [](const auto &fi1,const auto &fi2) { return fi1.includeName < fi2.includeName; });

  if (Config_getBool(SORT_BRIEF_DOCS))
  {
    auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
    {
      return Config_getBool(SORT_BY_SCOPE_NAME)          ?
        qstricmp_sort(c1->name(),      c2->name())<0     :
        qstricmp_sort(c1->className(), c2->className())<0;
    };

    std::stable_sort(m_classes.begin(),   m_classes.end(),   classComp);
    std::stable_sort(m_interfaces.begin(),m_interfaces.end(),classComp);
    std::stable_sort(m_structs.begin(),   m_structs.end(),   classComp);
    std::stable_sort(m_exceptions.begin(),m_exceptions.end(),classComp);

    auto namespaceComp = [](const NamespaceLinkedRefMap::Ptr &n1,const NamespaceLinkedRefMap::Ptr &n2)
    {
      return qstricmp_sort(n1->name(),n2->name())<0;
    };

    std::stable_sort(m_namespaces.begin(),m_namespaces.end(),namespaceComp);
  }
}

MemberList *FileDefImpl::getMemberList(MemberListType lt) const
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

void FileDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  bool optVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  MemberList * ml = getMemberList(lt);
  if (ml)
  {
    if (optVhdl) // use specific declarations function
    {

      VhdlDocGen::writeVhdlDeclarations(ml,ol,nullptr,nullptr,this,nullptr,nullptr);
    }
    else
    {
      ml->writeDeclarations(ol,nullptr,nullptr,this,nullptr,nullptr,title,QCString());
    }
  }
}

void FileDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

bool FileDefImpl::isLinkableInProject() const
{
  bool showFiles = Config_getBool(SHOW_FILES);
  return hasDocumentation() && !isReference() && (showFiles || isLinkableViaGroup());
}

static void getAllIncludeFilesRecursively(
    StringUnorderedSet &filesVisited,const FileDef *fd,StringVector &incFiles)
{
  for (const auto &ii : fd->includeFileList())
  {
    if (ii.fileDef && !ii.fileDef->isReference() &&
        filesVisited.find(ii.fileDef->absFilePath().str())==filesVisited.end())
    {
      //printf("FileDefImpl::addIncludeDependency(%s)\n",qPrint(ii->fileDef->absFilePath()));
      incFiles.push_back(ii.fileDef->absFilePath().str());
      filesVisited.insert(ii.fileDef->absFilePath().str());
      getAllIncludeFilesRecursively(filesVisited,ii.fileDef,incFiles);
    }
  }
}

void FileDefImpl::getAllIncludeFilesRecursively(StringVector &incFiles) const
{
  StringUnorderedSet includes;
  ::getAllIncludeFilesRecursively(includes,this,incFiles);
}

QCString FileDefImpl::title() const
{
  return theTranslator->trFileReference(name());
}

QCString FileDefImpl::fileVersion() const
{
  return m_fileVersion;
}

QCString FileDefImpl::includeDependencyGraphFileName() const
{
  return m_inclDepFileName;
}

QCString FileDefImpl::includedByDependencyGraphFileName() const
{
  return m_inclByDepFileName;
}

void FileDefImpl::countMembers()
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

int FileDefImpl::numDocMembers() const
{
  MemberList *ml = getMemberList(MemberListType::AllMembersList());
  return ml ? ml->numDocMembers() : 0;
}

int FileDefImpl::numDecMembers() const
{
  MemberList *ml = getMemberList(MemberListType::AllMembersList());
  return ml ? ml->numDecMembers() : 0;
}

void FileDefImpl::overrideIncludeGraph(bool e)
{
  m_hasIncludeGraph=e;
}

void FileDefImpl::overrideIncludedByGraph(bool e)
{
  m_hasIncludedByGraph=e;
}

bool FileDefImpl::hasIncludeGraph() const
{
  return m_hasIncludeGraph;
}

bool FileDefImpl::hasIncludedByGraph() const
{
  return m_hasIncludedByGraph;
}

// -----------------------

bool compareFileDefs(const FileDef *fd1, const FileDef *fd2)
{
  return qstricmp_sort(fd1->name(),fd2->name()) < 0;
}

// --- Cast functions

FileDef *toFileDef(Definition *d)
{
  if (d==nullptr) return nullptr;
  if (d && typeid(*d)==typeid(FileDefImpl))
  {
    return static_cast<FileDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

const FileDef *toFileDef(const Definition *d)
{
  if (d==nullptr) return nullptr;
  if (d && typeid(*d)==typeid(FileDefImpl))
  {
    return static_cast<const FileDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

