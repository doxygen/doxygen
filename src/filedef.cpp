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

//---------------------------------------------------------------------------

using DefinitionLineMap = std::unordered_map<int,const Definition *>;
using MemberDefLineMap  = std::unordered_map<int,const MemberDef *>;
using IncludeInfoMap = std::unordered_map<std::string, const IncludeInfo *>;

class FileDefImpl : public DefinitionMixin<FileDef>
{
  public:
    FileDefImpl(const QCString &p,const QCString &n,const QCString &ref=QCString(),const QCString &dn=QCString());
    virtual ~FileDefImpl();

    virtual DefType definitionType() const { return TypeFile; }
    virtual QCString name() const;

    virtual QCString displayName(bool=TRUE) const { return name(); }
    virtual QCString fileName() const { return m_fileName; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual QCString getSourceFileBase() const;
    virtual QCString includeName() const;
    virtual QCString includeDependencyGraphFileName() const;
    virtual QCString includedByDependencyGraphFileName() const;
    virtual QCString absFilePath() const { return m_filePath; }
    virtual const QCString &docName() const { return m_docname; }
    virtual bool isSource() const { return m_isSource; }
    virtual bool isDocumentationFile() const;
    virtual const Definition *getSourceDefinition(int lineNr) const;
    virtual const MemberDef *getSourceMember(int lineNr) const;
    virtual QCString getPath() const { return m_path; }
    virtual QCString getVersion() const { return m_fileVersion; }
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const { return isLinkableInProject() || isReference(); }
    virtual bool isIncluded(const QCString &name) const;
    virtual PackageDef *packageDef() const { return m_package; }
    virtual DirDef *getDirDef() const      { return m_dir; }
    virtual LinkedRefMap<const NamespaceDef> getUsedNamespaces() const;
    virtual LinkedRefMap<const ClassDef> getUsedClasses() const  { return m_usingDeclList; }
    virtual const IncludeInfoList &includeFileList() const    { return m_includeList; }
    virtual const IncludeInfoList &includedByFileList() const { return m_includedByList; }
    virtual void getAllIncludeFilesRecursively(StringVector &incFiles) const;
    virtual MemberList *getMemberList(MemberListType lt) const;
    virtual const MemberLists &getMemberLists() const { return m_memberLists; }
    virtual const MemberGroupList &getMemberGroups() const  { return m_memberGroups; }
    virtual const NamespaceLinkedRefMap &getNamespaces() const    { return m_namespaces; }
    virtual const ConceptLinkedRefMap &getConcepts() const        { return m_concepts; }
    virtual const ClassLinkedRefMap &getClasses() const           { return m_classes; }
    virtual QCString title() const;
    virtual bool hasDetailedDescription() const;
    virtual QCString fileVersion() const;
    virtual bool subGrouping() const { return m_subGrouping; }
    virtual void countMembers();
    virtual int numDocMembers() const;
    virtual int numDecMembers() const;
    virtual void addSourceRef(int line,const Definition *d,const MemberDef *md);
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeMemberPages(OutputList &ol);
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const;
    virtual void writeSummaryLinks(OutputList &ol) const;
    virtual void writeTagFile(TextStream &t);
    virtual void writeSourceHeader(OutputList &ol);
    virtual void writeSourceBody(OutputList &ol,ClangTUParser *clangParser);
    virtual void writeSourceFooter(OutputList &ol);
    virtual void parseSource(ClangTUParser *clangParser);
    virtual void setDiskName(const QCString &name);
    virtual void insertMember(MemberDef *md);
    virtual void insertClass(const ClassDef *cd);
    virtual void insertConcept(const ConceptDef *cd);
    virtual void insertNamespace(const NamespaceDef *nd);
    virtual void computeAnchors();
    virtual void setPackageDef(PackageDef *pd) { m_package=pd; }
    virtual void setDirDef(DirDef *dd) { m_dir=dd; }
    virtual void addUsingDirective(const NamespaceDef *nd);
    virtual void addUsingDeclaration(const ClassDef *cd);
    virtual void combineUsingRelations();
    virtual bool generateSourceFile() const;
    virtual void sortMemberLists();
    virtual void addIncludeDependency(const FileDef *fd,const QCString &incName,bool local,bool imported);
    virtual void addIncludedByDependency(const FileDef *fd,const QCString &incName,bool local,bool imported);
    virtual void addMembersToMemberGroup();
    virtual void distributeMemberGroupDocumentation();
    virtual void findSectionsInDocumentation();
    virtual void addIncludedUsingDirectives(FileDefSet &visitedFiles);
    virtual void addListReferences();

  private:
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

    IncludeInfoMap        m_includeMap;
    IncludeInfoList       m_includeList;
    IncludeInfoMap        m_includedByMap;
    IncludeInfoList       m_includedByList;
    LinkedRefMap<const NamespaceDef> m_usingDirList;
    LinkedRefMap<const ClassDef> m_usingDeclList;
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
    PackageDef           *m_package;
    DirDef               *m_dir;
    MemberLists           m_memberLists;
    MemberGroupList       m_memberGroups;
    NamespaceLinkedRefMap m_namespaces;
    ClassLinkedRefMap     m_classes;
    ClassLinkedRefMap     m_interfaces;
    ClassLinkedRefMap     m_structs;
    ClassLinkedRefMap     m_exceptions;
    ConceptLinkedRefMap   m_concepts;
    bool                  m_subGrouping;
};

FileDef *createFileDef(const QCString &p,const QCString &n,const QCString &ref,const QCString &dn)
{
  return new FileDefImpl(p,n,ref,dn);
}


//---------------------------------------------------------------------------

/** Class implementing CodeOutputInterface by throwing away everything. */
class DevNullCodeDocInterface : public CodeOutputInterface
{
  public:
    virtual void codify(const QCString &) override {}
    virtual void writeCodeLink(const QCString &,const QCString &,
                               const QCString &,const QCString &,
                               const QCString &) override {}
    virtual void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                              const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                             ) override {}
    virtual void writeLineNumber(const QCString &,const QCString &,
                                 const QCString &,int) override {}
    virtual void startCodeLine(bool) override {}
    virtual void endCodeLine() override {}
    virtual void startFontClass(const QCString &) override {}
    virtual void endFontClass() override {}
    virtual void writeCodeAnchor(const QCString &) override {}
    virtual void setCurrentDoc(const Definition *,const QCString &,bool) override {}
    virtual void addWord(const QCString &,bool) override {}
    virtual void startCodeFragment(const QCString &) override {}
    virtual void endCodeFragment(const QCString &) override {}
};

//---------------------------------------------------------------------------

/*! create a new file definition, where \a p is the file path,
    \a nm the file name, and \a lref is an HTML anchor name if the
    file was read from a tag file or 0 otherwise
*/
FileDefImpl::FileDefImpl(const QCString &p,const QCString &nm,
                 const QCString &lref,const QCString &dn)
   : DefinitionMixin((QCString)p+nm,1,1,nm)
{
  m_path=p;
  m_filePath=m_path+nm;
  m_fileName=nm;
  setReference(lref);
  setDiskName(!dn.isEmpty() ? dn : nm);
  m_package           = 0;
  m_isSource          = guessSection(nm)==Entry::SOURCE_SEC;
  m_docname           = nm;
  m_dir               = 0;
  if (Config_getBool(FULL_PATH_NAMES))
  {
    m_docname.prepend(stripFromPath(m_path));
  }
  setLanguage(getLanguageFromFileName(name()));
  acquireFileVersion();
  m_subGrouping=Config_getBool(SUBGROUPING);
}

/*! destroy the file definition */
FileDefImpl::~FileDefImpl()
{
}

void FileDefImpl::setDiskName(const QCString &name)
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

/*! Compute the HTML anchor names for all members in the class */
void FileDefImpl::computeAnchors()
{
  MemberList *ml = getMemberList(MemberListType_allMembersList);
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
  for (const auto &mg : m_memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }

  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}

bool FileDefImpl::hasDetailedDescription() const
{
  static bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  static bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().stripWhiteSpace().isEmpty() || // avail empty section
          (sourceBrowser && getStartBodyLine()!=-1 && getBodyDef())
         );
}

void FileDefImpl::writeTagFile(TextStream &tagFile)
{
  tagFile << "  <compound kind=\"file\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <path>" << convertToXML(getPath()) << "</path>\n";
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>\n";
  for (const auto &ii : m_includeList)
  {
    const FileDef *fd=ii.fileDef;
    if (fd && fd->isLinkable() && !fd->isReference())
    {
      bool isIDLorJava = FALSE;
      SrcLangExt lang = fd->getLanguage();
      isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
      const char *locStr = (ii.local    || isIDLorJava) ? "yes" : "no";
      const char *impStr = (ii.imported || isIDLorJava) ? "yes" : "no";
      tagFile << "    <includes id=\""
        << convertToXML(fd->getOutputFileBase()) << "\" "
        << "name=\"" << convertToXML(fd->name()) << "\" "
        << "local=\"" << locStr << "\" "
        << "imported=\"" << impStr << "\">"
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
          const LayoutDocEntryMemberDecl *lmd = (const LayoutDocEntryMemberDecl*)lde.get();
          MemberList * ml = getMemberList(lmd->type);
          if (ml)
          {
            ml->writeTagFile(tagFile);
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
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Man);
        ol.disable(OutputGenerator::RTF);
        // ol.newParagraph(); // FIXME:PARA
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.enable(OutputGenerator::Latex);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    //printf("Writing source ref for file %s\n",qPrint(name()));
    if (Config_getBool(SOURCE_BROWSER))
    {
      //if Latex enabled and LATEX_SOURCE_CODE isn't -> skip, bug_738548
      ol.pushGeneratorState();
      if (ol.isEnabled(OutputGenerator::Latex) && !Config_getBool(LATEX_SOURCE_CODE))
      {
        ol.disable(OutputGenerator::Latex);
      }
      if (ol.isEnabled(OutputGenerator::Docbook) && !Config_getBool(DOCBOOK_PROGRAMLISTING))
      {
        ol.disable(OutputGenerator::Docbook);
      }
      if (ol.isEnabled(OutputGenerator::RTF) && !Config_getBool(RTF_SOURCE_CODE))
      {
        ol.disable(OutputGenerator::RTF);
      }

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
      //Restore settings, bug_738548
      ol.popGeneratorState();
    }
    ol.endTextBlock();
  }
}

void FileDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),this,0,
                       briefDescription(),TRUE,FALSE,
                       QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));

    if (rootNode && !rootNode->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(rootNode,this,0);
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.writeString(" \n");
      ol.enable(OutputGenerator::RTF);

      if (Config_getBool(REPEAT_BRIEF) ||
          !documentation().isEmpty()
         )
      {
        ol.disableAllBut(OutputGenerator::Html);
        ol.startTextLink(QCString(),"details");
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
      }
      ol.popGeneratorState();
      ol.endParagraph();
    }
    delete rootNode;
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
      bool isIDLorJava = FALSE;
      if (fd)
      {
        SrcLangExt lang   = fd->getLanguage();
        isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
      }
      ol.startTypewriter();
      if (isIDLorJava) // IDL/Java include
      {
        ol.docify("import ");
      }
      else if (ii.imported) // Objective-C include
      {
        ol.docify("#import ");
      }
      else // C/C++ include
      {
        ol.docify("#include ");
      }
      if (ii.local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.disable(OutputGenerator::Html);
      ol.docify(ii.includeName);
      ol.enableAll();
      ol.disableAllBut(OutputGenerator::Html);

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
        ol.docify(ii.includeName);
      }

      ol.enableAll();
      if (ii.local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify(">");
      if (isIDLorJava)
        ol.docify(";");
      ol.endTypewriter();
      ol.lineBreak();
    }
    ol.endTextBlock();
  }
}

void FileDefImpl::writeIncludeGraph(OutputList &ol)
{
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(INCLUDE_GRAPH)*/)
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
      ol.disable(OutputGenerator::Man);
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
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(INCLUDED_BY_GRAPH)*/)
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
      ol.disable(OutputGenerator::Man);
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
    ol.disableAllBut(OutputGenerator::Html);
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
  list.writeDeclaration(ol,0,title,FALSE);
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
  bool isEnabled = ol.isEnabled(OutputGenerator::Html);
  ol.enable(OutputGenerator::Html);

  m_classes.writeDocumentation(ol,this);

  // restore the initial state if needed
  if (!isEnabled) ol.disable(OutputGenerator::Html);
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
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void FileDefImpl::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void FileDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  for (const auto &mg : m_memberGroups)
  {
    if ((!mg->allMembersInSameSection() || !m_subGrouping)
        && mg->header()!="[NOHEADER]")
    {
      mg->writeDeclarations(ol,0,0,this,0);
    }
  }
}

void FileDefImpl::writeAuthorSection(OutputList &ol)
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

void FileDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  bool first=TRUE;
  SrcLangExt lang=getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::File))
  {
    if (lde->kind()==LayoutDocEntry::FileClasses && m_classes.declVisible())
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = "nested-classes";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileInterfaces && m_interfaces.declVisible())
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = "interfaces";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileStructs && m_structs.declVisible())
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = "structs";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileExceptions && m_exceptions.declVisible())
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = "exceptions";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileNamespaces && m_namespaces.declVisible())
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = "namespaces";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::FileConcepts && m_concepts.declVisible())
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = "concepts";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = (const LayoutDocEntryMemberDecl*)lde.get();
      MemberList * ml = getMemberList(lmd->type);
      if (ml && ml->declVisible())
      {
        ol.writeSummaryLink(QCString(),MemberList::listTypeAsString(ml->listType()),lmd->title(lang),first);
        first=FALSE;
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
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
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
    startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_FileVisible,!generateTreeView);
    if (!generateTreeView)
    {
      getDirDef()->writeNavigationPath(ol);
      ol.endQuickIndices();
    }
    QCString pageTitleShort=theTranslator->trFileReference(name());
    startTitle(ol,getOutputFileBase(),this);
    ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.parseText(pageTitleShort); // Html only
      ol.enableAll();
      ol.disable(OutputGenerator::Html);
      ol.parseText(pageTitle); // other output formats
    ol.popGeneratorState();
    addGroupListToTitle(ol,this);
    endTitle(ol,getOutputFileBase(),title);
  }
  else
  {
    startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_FileVisible,!generateTreeView);
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
    ol.disableAllBut(OutputGenerator::Html);
    ol.startProjectNumber();
    ol.docify(versionTitle);
    ol.endProjectNumber();
    ol.enableAll();
  }

  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }


  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::File))
  {
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
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeClassDeclarations(ol,ls->title(lang),m_classes);
        }
        break;
      case LayoutDocEntry::FileInterfaces:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeClassDeclarations(ol,ls->title(lang),m_interfaces);
        }
        break;
      case LayoutDocEntry::FileStructs:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeClassDeclarations(ol,ls->title(lang),m_structs);
        }
        break;
      case LayoutDocEntry::FileExceptions:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeClassDeclarations(ol,ls->title(lang),m_exceptions);
        }
        break;
      case LayoutDocEntry::FileConcepts:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeConcepts(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::FileNamespaces:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeNamespaceDeclarations(ol,ls->title(lang),false);
        }
        break;
      case LayoutDocEntry::FileConstantGroups:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeNamespaceDeclarations(ol,ls->title(lang),true);
        }
        break;
      case LayoutDocEntry::MemberGroups:
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = (const LayoutDocEntryMemberDecl*)lde.get();
          writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeDetailedDescription(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::FileInlineClasses:
        writeInlineClasses(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = (const LayoutDocEntryMemberDef*)lde.get();
          writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
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
            "LayoutDocManager::File entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(this,ol);

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    MemberList *ml = getMemberList(MemberListType_allMembersList);
    if (ml) ml->sort();
    writeMemberPages(ol);
  }
}

void FileDefImpl::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  for (const auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->writeDocumentationPage(ol,name(),this);
    }
  }

  ol.popGeneratorState();
}

void FileDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  static bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList)
  {
    for (const auto &md : *allMemberList)
    {
      if (md->getFileDef()==this && md->getNamespaceDef()==0 && md->isLinkable() && !md->isEnumValue())
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
          ol.writeString(md->getOutputFileBase()+Doxygen::htmlFileExtension+"#"+md->anchor());
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
  bool latexSourceCode   = Config_getBool(LATEX_SOURCE_CODE);
  bool docbookSourceCode = Config_getBool(DOCBOOK_PROGRAMLISTING);
  bool rtfSourceCode     = Config_getBool(RTF_SOURCE_CODE);
  QCString title = m_docname;
  if (!m_fileVersion.isEmpty())
  {
    title+=(" ("+m_fileVersion+")");
  }
  QCString pageTitle = theTranslator->trSourceFile(title);
  ol.disable(OutputGenerator::Man);
  if (!latexSourceCode) ol.disable(OutputGenerator::Latex);
  if (!docbookSourceCode) ol.disable(OutputGenerator::Docbook);
  if (!rtfSourceCode) ol.disable(OutputGenerator::RTF);

  bool isDocFile = isDocumentationFile();
  bool genSourceFile = !isDocFile && generateSourceFile();
  if (getDirDef())
  {
    startFile(ol,getSourceFileBase(),QCString(),pageTitle,HLI_FileVisible,
        !generateTreeView,
        !isDocFile && genSourceFile ? QCString() : getOutputFileBase());
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
    startFile(ol,getSourceFileBase(),QCString(),pageTitle,HLI_FileVisible,FALSE,
        !isDocFile && genSourceFile ? QCString() : getOutputFileBase());
    startTitle(ol,getSourceFileBase());
    ol.parseText(title);
    endTitle(ol,getSourceFileBase(),QCString());
  }

  ol.startContents();

  if (isLinkable())
  {
    ol.pushGeneratorState();
    if (latexSourceCode) ol.disable(OutputGenerator::Latex);
    if (rtfSourceCode) ol.disable(OutputGenerator::RTF);
    if (docbookSourceCode) ol.disable(OutputGenerator::Docbook);
    ol.startTextLink(getOutputFileBase(),QCString());
    ol.parseText(theTranslator->trGotoDocumentation());
    ol.endTextLink();
    ol.popGeneratorState();
  }
}

void FileDefImpl::writeSourceBody(OutputList &ol,ClangTUParser *clangParser)
{
  bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  DevNullCodeDocInterface devNullIntf;
#if USE_LIBCLANG
  if (Doxygen::clangAssistedParsing && clangParser &&
      (getLanguage()==SrcLangExt_Cpp || getLanguage()==SrcLangExt_ObjC))
  {
    ol.startCodeFragment("DoxyCode");
    clangParser->switchToFile(this);
    clangParser->writeSources(ol,this);
    ol.endCodeFragment("DoxyCode");
  }
  else
#endif
  {
    auto intf = Doxygen::parserManager->getCodeParser(getDefFileExtension());
    intf->resetCodeParserState();
    ol.startCodeFragment("DoxyCode");
    bool needs2PassParsing =
        Doxygen::parseSourcesNeeded &&                // we need to parse (filtered) sources for cross-references
        !filterSourceFiles &&                         // but user wants to show sources as-is
        !getFileFilter(absFilePath(),TRUE).isEmpty(); // and there is a filter used while parsing

    if (needs2PassParsing)
    {
      // parse code for cross-references only (see bug707641)
      intf->parseCode(devNullIntf,QCString(),
                       fileToString(absFilePath(),TRUE,TRUE),
                       getLanguage(),
                       FALSE,QCString(),this
                      );
    }
    intf->parseCode(ol,QCString(),
        fileToString(absFilePath(),filterSourceFiles,TRUE),
        getLanguage(),      // lang
        FALSE,              // isExampleBlock
        QCString(),         // exampleName
        this,               // fileDef
        -1,                 // startLine
        -1,                 // endLine
        FALSE,              // inlineFragment
        0,                  // memberDef
        TRUE,               // showLineNumbers
        0,                  // searchCtx
        !needs2PassParsing  // collectXRefs
        );
    ol.endCodeFragment("DoxyCode");
  }
}

void FileDefImpl::writeSourceFooter(OutputList &ol)
{
  ol.endContents();
  endFileWithNavPath(this,ol);
  ol.enableAll();
}

void FileDefImpl::parseSource(ClangTUParser *clangParser)
{
  static bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  DevNullCodeDocInterface devNullIntf;
#if USE_LIBCLANG
  if (Doxygen::clangAssistedParsing && clangParser &&
      (getLanguage()==SrcLangExt_Cpp || getLanguage()==SrcLangExt_ObjC))
  {
    clangParser->switchToFile(this);
    clangParser->writeSources(devNullIntf,this);
  }
  else
#endif
  {
    auto intf = Doxygen::parserManager->getCodeParser(getDefFileExtension());
    intf->resetCodeParserState();
    intf->parseCode(
            devNullIntf,QCString(),
            fileToString(absFilePath(),filterSourceFiles,TRUE),
            getLanguage(),
            FALSE,QCString(),this
           );
  }
}

void FileDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_declarationLists)
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
  MemberList *allMemberList = getMemberList(MemberListType_allMembersList);
  if (allMemberList && allMemberList->contains(md))
  {
    return;
  }

  if (allMemberList==0)
  {
    m_memberLists.emplace_back(std::make_unique<MemberList>(MemberListType_allMembersList));
    allMemberList = m_memberLists.back().get();
  }
  allMemberList->push_back(md);
  //::addFileMemberNameToIndex(md);
  switch (md->memberType())
  {
    case MemberType_Variable:
    case MemberType_Property:
      addMemberToList(MemberListType_decVarMembers,md);
      addMemberToList(MemberListType_docVarMembers,md);
      break;
    case MemberType_Function:
      addMemberToList(MemberListType_decFuncMembers,md);
      addMemberToList(MemberListType_docFuncMembers,md);
      break;
    case MemberType_Typedef:
      addMemberToList(MemberListType_decTypedefMembers,md);
      addMemberToList(MemberListType_docTypedefMembers,md);
      break;
    case MemberType_Sequence:
      addMemberToList(MemberListType_decSequenceMembers,md);
      addMemberToList(MemberListType_docSequenceMembers,md);
      break;
    case MemberType_Dictionary:
      addMemberToList(MemberListType_decDictionaryMembers,md);
      addMemberToList(MemberListType_docDictionaryMembers,md);
      break;
    case MemberType_Enumeration:
      addMemberToList(MemberListType_decEnumMembers,md);
      addMemberToList(MemberListType_docEnumMembers,md);
      break;
    case MemberType_EnumValue:    // enum values are shown inside their enums
      break;
    case MemberType_Define:
      addMemberToList(MemberListType_decDefineMembers,md);
      addMemberToList(MemberListType_docDefineMembers,md);
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

/*! Adds compound definition \a cd to the list of all compounds of this file */
void FileDefImpl::insertClass(const ClassDef *cd)
{
  if (cd->isHidden()) return;

  ClassLinkedRefMap &list = m_classes;

  if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
  {
    if (cd->compoundType()==ClassDef::Interface)
    {
      list = m_interfaces;
    }
    else if (cd->compoundType()==ClassDef::Struct)
    {
      list = m_structs;
    }
    else if (cd->compoundType()==ClassDef::Exception)
    {
      list = m_exceptions;
    }
  }

  list.add(cd->name(),cd);
}

void FileDefImpl::insertConcept(const ConceptDef *cd)
{
  if (cd->isHidden()) return;
  m_concepts.add(cd->name(),cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDefImpl::insertNamespace(const NamespaceDef *nd)
{
  if (nd->isHidden()) return;
  m_namespaces.add(nd->name(),nd);
}

QCString FileDefImpl::name() const
{
  if (Config_getBool(FULL_PATH_NAMES))
    return m_fileName;
  else
    return DefinitionMixin::name();
}

void FileDefImpl::addSourceRef(int line,const Definition *d,const MemberDef *md)
{
  //printf("FileDefImpl::addSourceDef(%d,%p,%p)\n",line,d,md);
  if (d)
  {
    m_srcDefMap.insert(std::make_pair(line,d));
    if (md) m_srcMemberMap.insert(std::make_pair(line,md));
    //printf("Adding member %s with anchor %s at line %d to file %s\n",
    //    md?qPrint(md->name()):"<none>",md?qPrint(md->anchor()):"<none>",line,qPrint(name()));
  }
}

const Definition *FileDefImpl::getSourceDefinition(int lineNr) const
{
  auto it = m_srcDefMap.find(lineNr);
  //printf("%s::getSourceDefinition(%d)=%s\n",qPrint(name()),lineNr,it!=m_srcDefMap.end()?qPrint(it->second->name()):"none");
  return it!=m_srcDefMap.end() ? it->second : 0;
}

const MemberDef *FileDefImpl::getSourceMember(int lineNr) const
{
  auto it = m_srcMemberMap.find(lineNr);
  //printf("%s::getSourceMember(%d)=%s\n",qPrint(name()),lineNr,it!=m_srcMemberMap.end()?qPrint(it->second->name()):"none");
  return it!=m_srcMemberMap.end() ? it->second : 0;
}


void FileDefImpl::addUsingDirective(const NamespaceDef *nd)
{
  m_usingDirList.add(nd->qualifiedName(),nd);
  //printf("%p: FileDefImpl::addUsingDirective: %s:%d\n",this,qPrint(name()),usingDirList->count());
}

LinkedRefMap<const NamespaceDef> FileDefImpl::getUsedNamespaces() const
{
  //printf("%p: FileDefImpl::getUsedNamespace: %s:%d\n",this,qPrint(name()),usingDirList?usingDirList->count():0);
  return m_usingDirList;
}

void FileDefImpl::addUsingDeclaration(const ClassDef *cd)
{
  m_usingDeclList.add(cd->qualifiedName(),cd);
}

void FileDefImpl::addIncludeDependency(const FileDef *fd,const QCString &incName,bool local,bool imported)
{
  //printf("FileDefImpl::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath() : incName;
  if (!iName.isEmpty() && m_includeMap.find(iName.str())==m_includeMap.end())
  {
    m_includeList.emplace_back(fd,incName,local,imported);
    m_includeMap.insert(std::make_pair(iName.str(),&m_includeList.back()));
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
        const auto &ii = *ii_it;
        if (ii.fileDef && ii.fileDef!=this)
        {
          // add using directives
          auto unl = ii.fileDef->getUsedNamespaces();
          for (auto it = unl.rbegin(); it!=unl.rend(); ++it)
          {
            const auto *nd = *it;
            m_usingDirList.prepend(nd->qualifiedName(),nd);
          }
          // add using declarations
          auto  udl = ii.fileDef->getUsedClasses();
          for (auto it = udl.rbegin(); it!=udl.rend(); ++it)
          {
            const auto *cd = *it;
            m_usingDeclList.prepend(cd->qualifiedName(),cd);
          }
        }
      }
    }
  }
  //printf(") end FileDefImpl::addIncludedUsingDirectives for file %s\n",qPrint(name()));
}


void FileDefImpl::addIncludedByDependency(const FileDef *fd,const QCString &incName,
                                      bool local,bool imported)
{
  //printf("FileDefImpl::addIncludedByDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath() : incName;
  if (!iName.isEmpty() && m_includedByMap.find(iName.str())==m_includedByMap.end())
  {
    m_includedByList.emplace_back(fd,incName,local,imported);
    m_includedByMap.insert(std::make_pair(iName.str(),&m_includedByList.back()));
  }
}

bool FileDefImpl::isIncluded(const QCString &name) const
{
  if (name.isEmpty()) return FALSE;
  return m_includeMap.find(name.str())!=m_includeMap.end();
}

bool FileDefImpl::generateSourceFile() const
{
  static bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  static bool verbatimHeaders = Config_getBool(VERBATIM_HEADERS);
  return !isReference() &&
         (sourceBrowser ||
           (verbatimHeaders && guessSection(name())==Entry::HEADER_SEC)
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
               0
              );
  }
  for (const auto &mg : m_memberGroups)
  {
    mg->addListReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

//-------------------------------------------------------------------

void FileDefImpl::combineUsingRelations()
{
  LinkedRefMap<const NamespaceDef> usingDirList = m_usingDirList;
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
    for (const auto &ucd : nd->getUsedClasses())
    {
      addUsingDeclaration(ucd);
    }
  }
}

bool FileDefImpl::isDocumentationFile() const
{
  return name().right(4)==".doc" ||
         name().right(4)==".txt" ||
         name().right(4)==".dox" ||
         name().right(3)==".md"  ||
         name().right(9)==".markdown" ||
         getLanguageFromFileName(getFileNameExtension(name())) == SrcLangExt_Markdown;
}

void FileDefImpl::acquireFileVersion()
{
  QCString vercmd = Config_getString(FILE_VERSION_FILTER);
  if (!vercmd.isEmpty() && !m_filePath.isEmpty() &&
      m_filePath!="generated" && m_filePath!="graph_legend")
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
    int numRead = (int)fread(buf,1,bufSize-1,f);
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
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  static bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_memberLists.get(lt);
  ml->setNeedsSorting(
       ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
       ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
  ml->push_back(md);
  if (lt&MemberListType_documentationLists)
  {
    ml->setInFile(TRUE);
  }
  if (ml->listType()&MemberListType_declarationLists)
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

  std::sort(m_includedByList.begin(),m_includedByList.end(),
      [](const auto &fi1,const auto &fi2) { return fi1.includeName < fi2.includeName; });

  if (Config_getBool(SORT_BRIEF_DOCS))
  {
    auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
    {
      return Config_getBool(SORT_BY_SCOPE_NAME)     ?
        qstricmp(c1->name(),      c2->name())<0     :
        qstricmp(c1->className(), c2->className())<0;
    };

    std::sort(m_classes.begin(),   m_classes.end(),   classComp);
    std::sort(m_interfaces.begin(),m_interfaces.end(),classComp);
    std::sort(m_structs.begin(),   m_structs.end(),   classComp);
    std::sort(m_exceptions.begin(),m_exceptions.end(),classComp);

    auto namespaceComp = [](const NamespaceLinkedRefMap::Ptr &n1,const NamespaceLinkedRefMap::Ptr &n2)
    {
      return qstricmp(n1->name(),n2->name())<0;
    };

    std::sort(m_namespaces.begin(),m_namespaces.end(),namespaceComp);
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
  return 0;
}

void FileDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  static bool optVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  MemberList * ml = getMemberList(lt);
  if (ml)
  {
    if (optVhdl) // use specific declarations function
    {

      VhdlDocGen::writeVhdlDeclarations(ml,ol,0,0,this,0);
    }
    else
    {
      ml->writeDeclarations(ol,0,0,this,0,title,QCString());
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
  static bool showFiles = Config_getBool(SHOW_FILES);
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
  MemberList *ml = getMemberList(MemberListType_allMembersList);
  return ml ? ml->numDocMembers() : 0;
}

int FileDefImpl::numDecMembers() const
{
  MemberList *ml = getMemberList(MemberListType_allMembersList);
  return ml ? ml->numDecMembers() : 0;
}

// -----------------------

bool compareFileDefs(const FileDef *fd1, const FileDef *fd2)
{
  return qstricmp(fd1->name(),fd2->name()) < 0;
}

// --- Cast functions

FileDef *toFileDef(Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(FileDefImpl))
  {
    return static_cast<FileDef*>(d);
  }
  else
  {
    return 0;
  }
}

const FileDef *toFileDef(const Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(FileDefImpl))
  {
    return static_cast<const FileDef*>(d);
  }
  else
  {
    return 0;
  }
}

