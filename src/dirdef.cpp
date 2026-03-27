/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include <algorithm>

#include "dirdef.h"
#include "md5.h"
#include "filename.h"
#include "doxygen.h"
#include "util.h"
#include "outputlist.h"
#include "language.h"
#include "message.h"
#include "dot.h"
#include "dotdirdeps.h"
#include "layout.h"
#include "config.h"
#include "docparser.h"
#include "definitionimpl.h"
#include "filedef.h"
#include "trace.h"

//----------------------------------------------------------------------

class DirDefImpl : public DefinitionMixin<DirDef>
{
  public:
    DirDefImpl(const QCString &path);
   ~DirDefImpl() override;
    NON_COPYABLE(DirDefImpl)

    DefType definitionType() const override { return TypeDir; }
    CodeSymbolType codeSymbolType() const override { return CodeSymbolType::Default; }
    QCString getOutputFileBase() const override;
    QCString anchor() const override { return QCString(); }
    bool isLinkableInProject() const override;
    bool isLinkable() const override;
    QCString displayName(bool=TRUE) const override { return m_dispName; }
    const QCString shortName() const override { return m_shortName; }
    void addSubDir(DirDef *subdir) override;
    const FileList &getFiles() const override { return m_fileList; }
    void addFile(FileDef *fd) override;
    const DirList &subDirs() const override { return m_subdirs; }
    bool hasSubdirs() const override { return !m_subdirs.empty(); }
    int level() const override { return m_level; }
    DirDef *parent() const override { return m_parent; }
    int dirIndex() const override { return m_dirIndex; }
    const UsedDirLinkedMap &usedDirs() const override { return m_usedDirs; }
    bool isParentOf(const DirDef *dir) const override;
    bool depGraphIsTrivial() const override;
    QCString shortTitle() const override;
    bool hasDetailedDescription() const override;
    void writeDocumentation(OutputList &ol) override;
    void writePageNavigation(OutputList &ol) const override;
    void writeTagFile(TextStream &t) override;
    void setDiskName(const QCString &name) override { m_diskName = name; }
    void sort() override;
    void setParent(DirDef *parent) override;
    void setDirIndex(int index) override;
    void setLevel() override;
    void addUsesDependency(const DirDef *usedDir,const FileDef *srcFd,
                                   const FileDef *dstFd,bool srcDirect, bool dstDirect) override;
    void computeDependencies() override;
    void findSectionsInDocumentation() override;
    void addListReferences() override;
    void addRequirementReferences() override;

    bool hasDirectoryGraph() const override;
    void overrideDirectoryGraph(bool e) override;

  public:
    static DirDef *mergeDirectoryInTree(const QCString &path);

  private:

    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void writeDirectoryGraph(OutputList &ol);
    void writeSubDirList(OutputList &ol);
    void writeFileList(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);

    static DirDef *createNewDir(const QCString &path);
    static bool matchPath(const QCString &path,const StringVector &l);

    DirList m_subdirs;
    QCString m_dispName;
    QCString m_shortName;
    QCString m_diskName;
    FileList m_fileList;                 // list of files in the group
    int m_dirIndex = -1;
    int m_level;
    DirDef *m_parent;
    UsedDirLinkedMap m_usedDirs;
    bool m_hasDirectoryGraph = false;
};

DirDef *createDirDef(const QCString &path)
{
  return new DirDefImpl(path);
}


//----------------------------------------------------------------------
// method implementation

DirDefImpl::DirDefImpl(const QCString &path) : DefinitionMixin(path,1,1,path)
{
  bool fullPathNames = Config_getBool(FULL_PATH_NAMES);
  // get display name (stripping the paths mentioned in STRIP_FROM_PATH)
  // get short name (last part of path)
  m_shortName = path;
  m_diskName = path;
  if (m_shortName.at(m_shortName.length()-1)=='/')
  { // strip trailing /
    m_shortName = m_shortName.left(m_shortName.length()-1);
  }
  int pi=m_shortName.findRev('/');
  if (pi!=-1)
  { // remove everything till the last /
    m_shortName = m_shortName.mid(pi+1);
  }
  setLocalName(m_shortName);
  m_dispName = fullPathNames ? stripFromPath(path) : m_shortName;
  if (m_dispName.length()>0 && m_dispName.at(m_dispName.length()-1)=='/')
  { // strip trailing /
    m_dispName = m_dispName.left(m_dispName.length()-1);
  }

  m_level=-1;
  m_parent=nullptr;
  m_hasDirectoryGraph=Config_getBool(DIRECTORY_GRAPH);

}

DirDefImpl::~DirDefImpl()
{
}

bool DirDefImpl::isLinkableInProject() const
{
  return !isReference() && hasDocumentation();
}

bool DirDefImpl::isLinkable() const
{
  return isReference() || isLinkableInProject();
}

void DirDefImpl::addSubDir(DirDef *subdir)
{
  m_subdirs.push_back(subdir);
  subdir->setOuterScope(this);
  subdir->setParent(this);
}

void DirDefImpl::setParent(DirDef *p)
{
   m_parent=p;
}

void DirDefImpl::setDirIndex(int index)
{
  m_dirIndex=index;
}

void DirDefImpl::addFile(FileDef *fd)
{
  m_fileList.push_back(fd);
  fd->setDirDef(this);
}

void DirDefImpl::sort()
{
  std::stable_sort(m_subdirs.begin(), m_subdirs.end(), compareDirDefs);
  std::stable_sort(m_fileList.begin(), m_fileList.end(), compareFileDefs);
}

static QCString encodeDirName(const QCString &anchor)
{
  AUTO_TRACE();
  // convert to md5 hash
  uint8_t md5_sig[16];
  char sigStr[33];
  MD5Buffer(anchor.data(),static_cast<unsigned int>(anchor.length()),md5_sig);
  MD5SigToString(md5_sig,sigStr);
  AUTO_TRACE_EXIT("result={}",sigStr);
  return sigStr;

  // old algorithm
//  QCString result;

//  int l = anchor.length(),i;
//  for (i=0;i<l;i++)
//  {
//    char c = anchor.at(i);
//    if ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
//    {
//      result+=c;
//    }
//    else
//    {
//      static char hexStr[]="0123456789ABCDEF";
//      char escChar[]={ '_', 0, 0, 0 };
//      escChar[1]=hexStr[c>>4];
//      escChar[2]=hexStr[c&0xf];
//      result+=escChar;
//    }
//  }
//  return result;
}

QCString DirDefImpl::getOutputFileBase() const
{
  QCString dir = "dir_"+encodeDirName(m_diskName);
  AUTO_TRACE("diskName={} result={}",m_diskName,dir);
  return dir;
}

void DirDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  AUTO_TRACE();
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
    ol.startGroupHeader("details");
    ol.parseText(title);
    ol.endGroupHeader();

    // repeat brief description
    ol.startTextBlock();
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),
                     briefLine(),
                     this,
                     nullptr,
                     briefDescription(),
                     DocOptions());
    }
    // separator between brief and details
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

    // write documentation
    if (!documentation().isEmpty())
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

void DirDefImpl::writeBriefDescription(OutputList &ol)
{
  AUTO_TRACE();
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
                                     .setIndexWords(true))
                };
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
          !documentation().isEmpty()
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

void DirDefImpl::writePageNavigation(OutputList &ol) const
{
  ol.writePageOutline();
}

void DirDefImpl::writeDirectoryGraph(OutputList &ol)
{
  // write graph dependency graph
  if (Config_getBool(HAVE_DOT) && m_hasDirectoryGraph /*&& Config_getBool(DIRECTORY_GRAPH)*/)
  {
    DotDirDeps dirDep(this);
    if (!dirDep.isTrivial())
    {
      msg("Generating dependency graph for directory {}\n",displayName());
      ol.disable(OutputType::Man);
      //ol.startParagraph();
      ol.startDirDepGraph();
      ol.parseText(theTranslator->trDirDepGraph(shortName()));
      ol.endDirDepGraph(dirDep);
      //ol.endParagraph();
      ol.enableAll();
    }
  }
}

void DirDefImpl::writeSubDirList(OutputList &ol)
{
  AUTO_TRACE();
  int numSubdirs = 0;
  for(const auto dd : m_subdirs)
  {
    if (dd->hasDocumentation() || !dd->getFiles().empty())
    {
      numSubdirs++;
    }
  }

  AUTO_TRACE_ADD("numSubdirs={}",numSubdirs);
  // write subdir list
  if (numSubdirs>0)
  {
    ol.startMemberHeader("subdirs");
    ol.parseText(theTranslator->trDir(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    for(const auto dd : m_subdirs)
    {
      if (dd->hasDocumentation() || !dd->getFiles().empty())
      {
        ol.startMemberDeclaration();
        QCString anc=dd->anchor();
        if (anc.isEmpty()) anc=dd->shortName(); else anc.prepend(dd->shortName()+"_");
        ol.startMemberItem(anc,OutputGenerator::MemberItemType::Normal);
        {
          ol.pushGeneratorState();
          ol.disableAllBut(OutputType::Html);
          ol.writeString("<span class=\"iconfolder\"><div class=\"folder-icon\"></div></span>");
          ol.enableAll();
          ol.disable(OutputType::Html);
          ol.parseText(theTranslator->trDir(FALSE,TRUE)+" ");
          ol.popGeneratorState();
        }
        ol.insertMemberAlign();
        ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),QCString(),dd->shortName());
        ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
        if (!dd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(dd->getOutputFileBase());
          ol.generateDoc(briefFile(),
                         briefLine(),
                         dd,
                         nullptr,
                         dd->briefDescription(),
                         DocOptions()
                         .setSingleLine(true)
                         .setLinkFromIndex(true));
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(dd->anchor(),QCString());
      }
    }

    ol.endMemberList();
  }
}

void DirDefImpl::writeFileList(OutputList &ol)
{
  AUTO_TRACE();
  int numFiles = 0;
  for (const auto &fd : m_fileList)
  {
    bool genSourceFile=false;
    if (fileVisibleInIndex(fd,genSourceFile))
    {
      numFiles++;
    }
    else if (genSourceFile)
    {
      numFiles++;
    }
  }

  AUTO_TRACE_ADD("numFiles={}",numFiles);
  // write file list
  if (numFiles>0)
  {
    ol.startMemberHeader("files");
    ol.parseText(theTranslator->trFile(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    for (const auto &fd : m_fileList)
    {
      bool src = false;
      bool doc = fileVisibleInIndex(fd,src);
      if (doc || src)
      {
        ol.startMemberDeclaration();
        QCString anc = fd->anchor();
        if (anc.isEmpty()) anc=fd->displayName(); else anc.prepend(fd->displayName()+"_");
        ol.startMemberItem(anc,OutputGenerator::MemberItemType::Normal);
        {
          ol.pushGeneratorState();
          ol.disableAllBut(OutputType::Html);
          bool genSrc = fd->generateSourceFile();
          if (genSrc)
          {
            ol.startTextLink(fd->includeName(),QCString());
          }
          ol.writeString("<span class=\"icondoc\"><div class=\"doc-icon\"></div></span>");
          if (genSrc)
          {
            ol.endTextLink();
          }
          ol.enableAll();
          ol.disable(OutputType::Html);
          ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
          ol.popGeneratorState();
        }
        ol.insertMemberAlign();
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
          ol.generateDoc(briefFile(),
                         briefLine(),
                         fd,
                         nullptr,
                         fd->briefDescription(),
                         DocOptions()
                         .setSingleLine(true)
                         .setLinkFromIndex(true));
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(fd->anchor(),QCString());
      }
    }
    ol.endMemberList();
  }
}

void DirDefImpl::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void DirDefImpl::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

QCString DirDefImpl::shortTitle() const
{
  if (Config_getBool(HIDE_COMPOUND_REFERENCE))
  {
    return m_shortName;
  }
  else
  {
    return theTranslator->trDirReference(m_shortName);
  }
}

bool DirDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return (!briefDescription().isEmpty() && repeatBrief) || !documentation().isEmpty() || hasRequirementRefs();
}

void DirDefImpl::writeTagFile(TextStream &tagFile)
{
  tagFile << "  <compound kind=\"dir\">\n";
  tagFile << "    <name>" << convertToXML(displayName()) << "</name>\n";
  tagFile << "    <path>" << convertToXML(stripFromPath(name())) << "</path>\n";
  QCString fn=getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "    <filename>" << fn << "</filename>\n";
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Directory))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::DirSubDirs:
        {
          if (m_subdirs.size()>0)
          {
            for(const auto dd : m_subdirs)
            {
              tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::DirFiles:
        {
          for (const auto &fd : m_fileList)
          {
              tagFile << "    <file>" << convertToXML(fd->name()) << "</file>\n";
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

void DirDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  ol.pushGeneratorState();

  QCString title;
  if (Config_getBool(HIDE_COMPOUND_REFERENCE))
  {
    title=m_dispName;
  }
  else
  {
    title=theTranslator->trDirReference(m_dispName);
  }
  AUTO_TRACE("title={}",title);
  startFile(ol,getOutputFileBase(),false,name(),title,HighlightedItem::Files,!generateTreeView);

  if (!generateTreeView)
  {
    // write navigation path
    writeNavigationPath(ol);
    ol.endQuickIndices();
  }

  startTitle(ol,getOutputFileBase());
  ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.parseText(shortTitle());
    ol.enableAll();
    ol.disable(OutputType::Html);
    ol.parseText(title);
  ol.popGeneratorState();
  endTitle(ol,getOutputFileBase(),title);
  ol.startContents();

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Directory))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol);
        break;
      case LayoutDocEntry::DirGraph:
        writeDirectoryGraph(ol);
        break;
      case LayoutDocEntry::MemberDeclStart:
        startMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DirSubDirs:
        writeSubDirList(ol);
        break;
      case LayoutDocEntry::DirFiles:
        writeFileList(ol);
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
          if (ls)
          {
            writeDetailedDescription(ol,ls->title(lang));
          }
        }
        break;
      case LayoutDocEntry::ClassIncludes:
      case LayoutDocEntry::ClassInlineClasses:
      case LayoutDocEntry::ClassInheritanceGraph:
      case LayoutDocEntry::ClassNestedClasses:
      case LayoutDocEntry::ClassCollaborationGraph:
      case LayoutDocEntry::ClassAllMembersLink:
      case LayoutDocEntry::ClassUsedFiles:
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
      case LayoutDocEntry::AuthorSection:
      case LayoutDocEntry::MemberGroups:
      case LayoutDocEntry::MemberDecl:
      case LayoutDocEntry::MemberDef:
      case LayoutDocEntry::MemberDefStart:
      case LayoutDocEntry::MemberDefEnd:
        err("Internal inconsistency: member '{}' should not be part of "
            "LayoutDocManager::Directory entry list\n",qPrint(lde->entryToString()));
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(ol,this);

  ol.popGeneratorState();
}

void DirDefImpl::setLevel()
{
  if (m_level==-1) // level not set before
  {
    DirDef *p = parent();
    if (p)
    {
      p->setLevel();
      m_level = p->level()+1;
    }
    else
    {
      m_level = 0;
    }
  }
}

/** Add as "uses" dependency between \a this dir and \a dir,
 *  that was caused by a dependency on file \a fd.
 *  srcDirect and dstDirect indicate if it is a direct dependencies (true) or if
 *  the dependencies was indirect (e.g. a parent dir that has a child dir that has the dependencies)
 */
void DirDefImpl::addUsesDependency(const DirDef *dir,const FileDef *srcFd,
                                   const FileDef *dstFd,bool srcDirect, bool dstDirect)
{
  if (this==dir) return; // do not add self-dependencies
  AUTO_TRACE("add dependency {}->{} due to {}->{}",
      qPrint(shortName()),
      qPrint(dir->shortName()),
      qPrint(srcFd->name()),
      qPrint(dstFd->name()));

  // levels match => add direct dependency
  bool added=FALSE;
  UsedDir *usedDir = m_usedDirs.find(dir->getOutputFileBase());
  if (usedDir) // dir dependency already present
  {
     const FilePair *usedPair = usedDir->findFilePair(FilePair::key(srcFd,dstFd));
     if (usedPair==nullptr) // new file dependency
     {
       AUTO_TRACE_ADD("{} => {} new file dependency",srcFd->name(),dstFd->name());
       usedDir->addFileDep(srcFd,dstFd, srcDirect, dstDirect);
       added=TRUE;
     }
     else
     {
       // dir & file dependency already added
     }
  }
  else // new directory dependency
  {
    AUTO_TRACE_ADD("{} => {} new file dependency",srcFd->name(),dstFd->name());
    auto newUsedDir = std::make_unique<UsedDir>(dir);
    newUsedDir->addFileDep(srcFd,dstFd, srcDirect, dstDirect);
    m_usedDirs.add(dir->getOutputFileBase(),std::move(newUsedDir));
    added=TRUE;
  }
  if (added)
  {
    if (dir->parent())
    {
      // add relation to parent of used dir
      addUsesDependency(dir->parent(),
                        srcFd,
                        dstFd,
                        srcDirect,
                        false); // indirect dependency on dest dir
    }
    if (parent())
    {
      // add relation for the parent of this dir as well
      parent()->addUsesDependency(dir,
                                 srcFd,
                                 dstFd,
                                 false, // indirect dependency from source dir
                                 dstDirect);
    }
  }
}

void DirDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
}

void DirDefImpl::addListReferences()
{
  QCString name = getOutputFileBase();
  addRefItem(xrefListItems(), name,
             theTranslator->trDir(TRUE,TRUE),
             name,displayName(),QCString(),nullptr);
}

void DirDefImpl::addRequirementReferences()
{
  RequirementManager::instance().addRequirementRefsForSymbol(this);
}

/** Computes the dependencies between directories
 */
void DirDefImpl::computeDependencies()
{
  AUTO_TRACE();
  for (const auto &fd : m_fileList)
  {
    AUTO_TRACE_ADD("dir={} file={}",shortName(),fd->name());
    for (const auto &ii : fd->includeFileList())
    {
      AUTO_TRACE_ADD("#include {}",ii.includeName);
      if (ii.fileDef && ii.fileDef->isLinkable()) // linkable file
      {
        DirDef *usedDir = ii.fileDef->getDirDef();
        if (usedDir)
        {
          // add dependency: thisDir->usedDir
          AUTO_TRACE_ADD("add dependency {}->{}",name(),usedDir->name());
          addUsesDependency(usedDir,fd,ii.fileDef,true,true);
        }
      }
    }
  }

  std::stable_sort(m_usedDirs.begin(),m_usedDirs.end(),
            [](const auto &u1,const auto &u2)
            { return qstricmp_sort(u1->dir()->getOutputFileBase(),u2->dir()->getOutputFileBase())<0; });

  for (const auto& usedDirectory : m_usedDirs)
  {
    usedDirectory->sort();
  }
}

bool DirDefImpl::isParentOf(const DirDef *dir) const
{
  if (dir->parent()==this) // this is a parent of dir
    return TRUE;
  else if (dir->parent()) // repeat for the parent of dir
    return isParentOf(dir->parent());
  else
    return FALSE;
}

bool DirDefImpl::depGraphIsTrivial() const
{
  return m_usedDirs.empty() && m_parent==nullptr;
}

//----------------------------------------------------------------------

UsedDir::UsedDir(const DirDef *dir) :
   m_dir(dir)
{
}

void UsedDir::addFileDep(const FileDef *srcFd,const FileDef *dstFd, bool srcDirect, bool dstDirect)
{
  m_filePairs.add(FilePair::key(srcFd,dstFd),std::make_unique<FilePair>(srcFd,dstFd));
  m_hasDirectDeps    = m_hasDirectDeps    || (srcDirect && dstDirect);
  m_hasDirectSrcDeps = m_hasDirectSrcDeps || srcDirect;
  m_hasDirectDstDeps = m_hasDirectDstDeps || dstDirect;
}

void UsedDir::sort()
{
  std::stable_sort(m_filePairs.begin(),
            m_filePairs.end(),
            [](const auto &left,const auto &right)
            {
              int orderHi = qstricmp_sort(left->source()->name(),right->source()->name());
              if (orderHi!=0) return orderHi<0;
              int orderLo = qstricmp_sort(left->destination()->name(),right->destination()->name());
              return orderLo<0;
            });
}

FilePair *UsedDir::findFilePair(const QCString &name)
{
  return m_filePairs.find(name);
}

DirDef *DirDefImpl::createNewDir(const QCString &path)
{
  AUTO_TRACE();
  ASSERT(path!=nullptr);
  DirDef *dir = Doxygen::dirLinkedMap->find(path);
  if (dir==nullptr) // new dir
  {
    dir = Doxygen::dirLinkedMap->add(path,
            std::unique_ptr<DirDef>(
              createDirDef(path)));
    AUTO_TRACE_ADD("Adding new dir {} shortName {}",path,dir->shortName());
  }
  return dir;
}

bool DirDefImpl::matchPath(const QCString &path,const StringVector &l)
{
  for (const auto &s : l)
  {
    std::string prefix = s.substr(0,path.length());
    if (qstricmp_sort(prefix.c_str(),path)==0) // case insensitive compare
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList(STRIP_FROM_PATH) list
 */
DirDef *DirDefImpl::mergeDirectoryInTree(const QCString &path)
{
  AUTO_TRACE("path={}",path);
  int p=0,i=0;
  DirDef *dir=nullptr;
  while ((i=path.find('/',p))!=-1)
  {
    QCString part=path.left(i+1);
    if (!matchPath(part,Config_getList(STRIP_FROM_PATH)) && (part!="/" && part!="//" && part!="//?/"))
    {
      dir=createNewDir(removeLongPathMarker(part));
    }
    p=i+1;
  }
  return dir;
}

void DirDefImpl::overrideDirectoryGraph(bool e)
{
  m_hasDirectoryGraph=e;
}

bool DirDefImpl::hasDirectoryGraph() const
{
  return m_hasDirectoryGraph;
}

//----------------------------------------------------------------------

QCString FilePair::key(const FileDef *srcFd,const FileDef *dstFd)
{
  return srcFd->getOutputFileBase()+";"+dstFd->getOutputFileBase();
}

//----------------------------------------------------------------------

static void writePartialDirPath(OutputList &ol,const DirDef *root,const DirDef *target)
{
  if (target->parent()!=root)
  {
    writePartialDirPath(ol,root,target->parent());
    ol.writeString("&#160;/&#160;");
  }
  ol.writeObjectLink(target->getReference(),target->getOutputFileBase(),QCString(),target->shortName());
}

static void writePartialFilePath(OutputList &ol,const DirDef *root,const FileDef *fd)
{
  if (fd->getDirDef() && fd->getDirDef()!=root)
  {
    writePartialDirPath(ol,root,fd->getDirDef());
    ol.writeString("&#160;/&#160;");
  }
  if (fd->isLinkable())
  {
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),fd->name());
  }
  else
  {
    ol.startBold();
    ol.docify(fd->name());
    ol.endBold();
  }
}

void DirRelation::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  QCString shortTitle=theTranslator->trDirRelation(
                      (m_src->shortName()+" &rarr; "+m_dst->dir()->shortName()));
  QCString title=theTranslator->trDirRelation(
                 (m_src->displayName()+" -> "+m_dst->dir()->shortName()));
  AUTO_TRACE("title={}",title);
  startFile(ol,getOutputFileBase(),false,getOutputFileBase(),
            title,HighlightedItem::None,!generateTreeView,m_src->getOutputFileBase());

  if (!generateTreeView)
  {
    // write navigation path
    m_src->writeNavigationPath(ol);
    ol.endQuickIndices();
  }
  ol.startContents();

  ol.writeString("<h3>"+shortTitle+"</h3>");
  ol.writeString("<table class=\"dirtab\">");
  ol.writeString("<tr class=\"dirtab\">");
  ol.writeString("<th class=\"dirtab\">");
  ol.parseText(theTranslator->trFileIn(m_src->pathFragment()));
  ol.writeString("</th>");
  ol.writeString("<th class=\"dirtab\">");
  ol.parseText(theTranslator->trIncludesFileIn(m_dst->dir()->pathFragment()));
  ol.writeString("</th>");
  ol.writeString("</tr>");

  for (const auto &fp : m_dst->filePairs())
  {
    ol.writeString("<tr class=\"dirtab\">");
    ol.writeString("<td class=\"dirtab\">");
    writePartialFilePath(ol,m_src,fp->source());
    ol.writeString("</td>");
    ol.writeString("<td class=\"dirtab\">");
    writePartialFilePath(ol,m_dst->dir(),fp->destination());
    ol.writeString("</td>");
    ol.writeString("</tr>");
  }
  ol.writeString("</table>");

  ol.endContents();

  endFileWithNavPath(ol,m_src);

  ol.popGeneratorState();
}

//----------------------------------------------------------------------
// external functions

/** In order to create stable, but unique directory names,
 *  we compute the common part of the path shared by all directories.
 */
static void computeCommonDirPrefix()
{
  AUTO_TRACE();
  QCString path;
  auto it = Doxygen::dirLinkedMap->begin();
  if (!Doxygen::dirLinkedMap->empty()) // we have at least one dir
  {
    // start will full path of first dir
    path=removeLongPathMarker((*it)->name());
    int i=path.findRev('/',static_cast<int>(path.length())-2);
    path=path.left(i+1);
    bool done=FALSE;
    if (i==-1)
    {
      path="";
    }
    else
    {
      while (!done)
      {
        int l = static_cast<int>(path.length());
        size_t count=0;
        for (const auto &dir : *Doxygen::dirLinkedMap)
        {
          QCString dirName = removeLongPathMarker(dir->name());
          //printf("dirName='%s' (l=%d) path='%s' (l=%d)\n",qPrint(dirName),dirName.length(),qPrint(path),path.length());
          if (dirName.length()>path.length())
          {
            if (dirName.left(l)!=path) // dirName does not start with path
            {
              i = l>=2 ? path.findRev('/',l-2) : -1;
              if (i==-1) // no unique prefix -> stop
              {
                path="";
                done=TRUE;
              }
              else // restart with shorter path
              {
                path=path.left(i+1);
                break;
              }
            }
          }
          else // dir is shorter than path -> take path of dir as new start
          {
            path=dir->name();
            l=static_cast<int>(path.length());
            i=path.findRev('/',l-2);
            if (i==-1) // no unique prefix -> stop
            {
              path="";
              done=TRUE;
            }
            else // restart with shorter path
            {
              path=path.left(i+1);
            }
            break;
          }
          count++;
        }
        if (count==Doxygen::dirLinkedMap->size())
          // path matches for all directories -> found the common prefix
        {
          done=TRUE;
        }
      }
    }
  }
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    QCString diskName = dir->name().right(dir->name().length()-path.length());
    dir->setDiskName(diskName);
    AUTO_TRACE_ADD("set disk name: {} -> {}",dir->name(),diskName);
  }
}

void buildDirectories()
{
  AUTO_TRACE();
  // for each input file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      if (fd->getReference().isEmpty())
      {
        DirDef *dir=Doxygen::dirLinkedMap->find(fd->getPath());
        if (dir==nullptr) // new directory
        {
          dir = DirDefImpl::mergeDirectoryInTree(fd->getPath());
        }
        if (dir && !fd->isDocumentationFile()) dir->addFile(fd.get());
      }
      else
      {
        // do something for file imported via tag files.
      }
    }
  }

  // compute relations between directories => introduce container dirs.
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    QCString name = dir->name();
    int i=name.findRev('/',static_cast<int>(name.length())-2);
    if (i>0)
    {
      DirDef *parent = Doxygen::dirLinkedMap->find(name.left(i+1));
      //if (parent==0) parent=root;
      if (parent)
      {
        parent->addSubDir(dir.get());
        AUTO_TRACE_ADD("DirDefImpl::addSubdir(): Adding subdir {} to {}",
                        dir->displayName(), parent->displayName());
      }
    }
  }

  // sort the directory contents
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    dir->sort();
  }

  // short the directories themselves
  std::stable_sort(Doxygen::dirLinkedMap->begin(),
            Doxygen::dirLinkedMap->end(),
            [](const auto &d1,const auto &d2)
            {
              QCString s1 = d1->shortName(), s2 = d2->shortName();
              int i = qstricmp_sort(s1,s2);
              if (i==0) // if sort name are equal, sort on full path
              {
                QCString n1 = d1->name(), n2 = d2->name();
                int n = qstricmp_sort(n1,n2);
                return n < 0;
              }
              return i < 0;
            });

  // set the directory index identifier
  int dirIndex=0;
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    dir->setDirIndex(dirIndex++);
  }

  computeCommonDirPrefix();
}

void computeDirDependencies()
{
  AUTO_TRACE();
  // compute nesting level for each directory
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    dir->setLevel();
  }

  // compute uses dependencies between directories
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    AUTO_TRACE_ADD("computeDependencies for {}: #dirs={}",dir->name(),Doxygen::dirLinkedMap->size());
    dir->computeDependencies();
  }
}

void generateDirDocs(OutputList &ol)
{
  AUTO_TRACE();
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    ol.pushGeneratorState();
    if (!dir->hasDocumentation())
    {
      ol.disableAllBut(OutputType::Html);
    }
    dir->writeDocumentation(ol);
    ol.popGeneratorState();
  }
  //if (Config_getBool(DIRECTORY_GRAPH))
  {
    for (const auto &dr : Doxygen::dirRelations)
    {
      dr->writeDocumentation(ol);
    }
  }
}

bool compareDirDefs(const DirDef *item1, const DirDef *item2)
{
  return qstricmp_sort(item1->shortName(),item2->shortName()) < 0;
}

// --- Cast functions

DirDef *toDirDef(Definition *d)
{
  if (d==nullptr) return nullptr;
  if (d && typeid(*d)==typeid(DirDefImpl))
  {
    return static_cast<DirDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

const DirDef *toDirDef(const Definition *d)
{
  if (d==nullptr) return nullptr;
  if (d && typeid(*d)==typeid(DirDefImpl))
  {
    return static_cast<const DirDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

