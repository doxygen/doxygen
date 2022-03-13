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


//----------------------------------------------------------------------

class DirDefImpl : public DefinitionMixin<DirDef>
{
  public:
    DirDefImpl(const QCString &path);
    virtual ~DirDefImpl();

    virtual DefType definitionType() const { return TypeDir; }
    virtual CodeSymbolType codeSymbolType() const { return CodeSymbolType::Default; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual QCString displayName(bool=TRUE) const { return m_dispName; }
    virtual const QCString shortName() const { return m_shortName; }
    virtual void addSubDir(DirDef *subdir);
    virtual const FileList &getFiles() const { return m_fileList; }
    virtual void addFile(const FileDef *fd);
    virtual const DirList &subDirs() const { return m_subdirs; }
    virtual bool hasSubdirs() const { return !m_subdirs.empty(); }
    virtual int level() const { return m_level; }
    virtual DirDef *parent() const { return m_parent; }
    virtual int dirCount() const { return m_dirCount; }
    virtual const UsedDirLinkedMap &usedDirs() const { return m_usedDirs; }
    virtual bool isParentOf(const DirDef *dir) const;
    virtual bool depGraphIsTrivial() const;
    virtual QCString shortTitle() const;
    virtual bool hasDetailedDescription() const;
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeTagFile(TextStream &t);
    virtual void setDiskName(const QCString &name) { m_diskName = name; }
    virtual void sort();
    virtual void setParent(DirDef *parent);
    virtual void setDirCount(int count);
    virtual void setLevel();
    virtual void addUsesDependency(const DirDef *usedDir,const FileDef *srcFd,
                                   const FileDef *dstFd,bool srcDirect, bool dstDirect);
    virtual void computeDependencies();

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
    int m_dirCount = -1;
    int m_level;
    DirDef *m_parent;
    UsedDirLinkedMap m_usedDirs;
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
  m_parent=0;
}

DirDefImpl::~DirDefImpl()
{
}

bool DirDefImpl::isLinkableInProject() const
{
  return !isReference();
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

void DirDefImpl::setDirCount(int count)
{
  m_dirCount=count;
}

void DirDefImpl::addFile(const FileDef *fd)
{
  m_fileList.push_back(fd);
  const_cast<FileDef*>(fd)->setDirDef(this);
}

void DirDefImpl::sort()
{
  std::sort(m_subdirs.begin(), m_subdirs.end(), compareDirDefs);
  std::sort(m_fileList.begin(), m_fileList.end(), compareFileDefs);
}

static QCString encodeDirName(const QCString &anchor)
{
  // convert to md5 hash
  uchar md5_sig[16];
  char sigStr[33];
  MD5Buffer(anchor.data(),anchor.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr);
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
  //printf("DirDefImpl::getOutputFileBase() %s->dir_%s\n",
  //    qPrint(m_diskName),qPrint(encodeDirName(m_diskName)));
  return "dir_"+encodeDirName(m_diskName);
}

void DirDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF)) ||
      !documentation().isEmpty())
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

    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
    // separator between brief and details
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Man);
        ol.disable(OutputGenerator::RTF);
        // ol.newParagraph();  // FIXME:PARA
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.enable(OutputGenerator::Latex);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
  }
}

void DirDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    std::unique_ptr<IDocParser> parser { createDocParser() };
    std::unique_ptr<DocNodeVariant> rootNode  { validatingParseDoc(
         *parser.get(), briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE,
         QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    const DocRoot *root = std::get_if<DocRoot>(rootNode.get());
    if (root && !root->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(*rootNode,this,0);
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
  }
  ol.writeSynopsis();
}

void DirDefImpl::writeDirectoryGraph(OutputList &ol)
{
  // write graph dependency graph
  if (Config_getBool(DIRECTORY_GRAPH) && Config_getBool(HAVE_DOT))
  {
    DotDirDeps dirDep(this);
    if (!dirDep.isTrivial())
    {
      msg("Generating dependency graph for directory %s\n",qPrint(displayName()));
      ol.disable(OutputGenerator::Man);
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
  int numSubdirs = 0;
  for(const auto dd : m_subdirs)
  {
    if (dd->hasDocumentation() || !dd->getFiles().empty())
    {
      numSubdirs++;
    }
  }

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
        ol.startMemberItem(dd->anchor(),0);
        ol.parseText(theTranslator->trDir(FALSE,TRUE)+" ");
        ol.insertMemberAlign();
        ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),QCString(),dd->shortName());
        ol.endMemberItem();
        if (!dd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(dd->getOutputFileBase());
          ol.generateDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),
              FALSE, // indexWords
              FALSE, // isExample
              QCString(), // exampleName
              TRUE,  // single line
              TRUE,  // link from index
              Config_getBool(MARKDOWN_SUPPORT)
              );
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
  int numFiles = 0;
  for (const auto &fd : m_fileList)
  {
    bool genSourceFile;
    if (fileVisibleInIndex(fd,genSourceFile))
    {
      numFiles++;
    }
    else if (genSourceFile)
    {
      numFiles++;
    }
  }

  // write file list
  if (numFiles>0)
  {
    ol.startMemberHeader("files");
    ol.parseText(theTranslator->trFile(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    for (const auto &fd : m_fileList)
    {
      bool doc,src;
      doc = fileVisibleInIndex(fd,src);
      if (doc || src)
      {
        ol.startMemberDeclaration();
        ol.startMemberItem(fd->anchor(),0);
        ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
        ol.insertMemberAlign();
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
        if (fd->generateSourceFile())
        {
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          ol.docify(" ");
          ol.startTextLink(fd->includeName(),QCString());
          ol.docify("[");
          ol.parseText(theTranslator->trCode());
          ol.docify("]");
          ol.endTextLink();
          ol.popGeneratorState();
        }
        ol.endMemberItem();
        if (!fd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(fd->getOutputFileBase());
          ol.generateDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),
              FALSE, // indexWords
              FALSE, // isExample
              QCString(), // exampleName
              TRUE,  // single line
              TRUE,  // link from index
              Config_getBool(MARKDOWN_SUPPORT)
              );
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
  return theTranslator->trDirReference(m_shortName);
}

bool DirDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return (!briefDescription().isEmpty() && repeatBrief) || !documentation().isEmpty();
}

void DirDefImpl::writeTagFile(TextStream &tagFile)
{
  tagFile << "  <compound kind=\"dir\">\n";
  tagFile << "    <name>" << convertToXML(displayName()) << "</name>\n";
  tagFile << "    <path>" << convertToXML(name()) << "</path>\n";
  tagFile << "    <filename>" << addHtmlExtensionIfMissing(getOutputFileBase()) << "</filename>\n";
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

  QCString title=theTranslator->trDirReference(m_dispName);
  startFile(ol,getOutputFileBase(),name(),title,HLI_Files,!generateTreeView);

  if (!generateTreeView)
  {
    // write navigation path
    writeNavigationPath(ol);
    ol.endQuickIndices();
  }

  startTitle(ol,getOutputFileBase());
  ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.parseText(shortTitle());
    ol.enableAll();
    ol.disable(OutputGenerator::Html);
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
      case LayoutDocEntry::GroupInlineClasses:
      case LayoutDocEntry::GroupNamespaces:
      case LayoutDocEntry::GroupDirs:
      case LayoutDocEntry::GroupNestedGroups:
      case LayoutDocEntry::GroupFiles:
      case LayoutDocEntry::GroupGraph:
      case LayoutDocEntry::GroupPageDocs:
      case LayoutDocEntry::AuthorSection:
      case LayoutDocEntry::MemberGroups:
      case LayoutDocEntry::MemberDecl:
      case LayoutDocEntry::MemberDef:
      case LayoutDocEntry::MemberDefStart:
      case LayoutDocEntry::MemberDefEnd:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::Directory entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  ol.endContents();

  endFileWithNavPath(this,ol);

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
  //static int count=0;
  //printf("  %d add dependency %s->%s due to %s->%s\n",
  //    count++,qPrint(shortName()),
  //    qPrint(dir->shortName()),
  //    qPrint(srcFd->name()),
  //    qPrint(dstFd->name()));

  // levels match => add direct dependency
  bool added=FALSE;
  UsedDir *usedDir = m_usedDirs.find(dir->getOutputFileBase());
  if (usedDir) // dir dependency already present
  {
     const FilePair *usedPair = usedDir->findFilePair(FilePair::key(srcFd,dstFd));
     if (usedPair==0) // new file dependency
     {
       //printf("  => new file\n");
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
    //printf("  => new file\n");
    auto newUsedDir = std::make_unique<UsedDir>(dir);
    newUsedDir->addFileDep(srcFd,dstFd, srcDirect, dstDirect);
    usedDir = m_usedDirs.add(dir->getOutputFileBase(),std::move(newUsedDir));
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

/** Computes the dependencies between directories
 */
void DirDefImpl::computeDependencies()
{
  for (const auto &fd : m_fileList)
  {
    //printf("  File %s\n",qPrint(fd->name()));
    //printf("** dir=%s file=%s\n",qPrint(shortName()),qPrint(fd->name()));
    for (const auto &ii : fd->includeFileList())
    {
      //printf("  > %s\n",qPrint(ii->includeName));
      //printf("    #include %s\n",qPrint(ii->includeName));
      if (ii.fileDef && ii.fileDef->isLinkable()) // linkable file
      {
        DirDef *usedDir = ii.fileDef->getDirDef();
        if (usedDir)
        {
          // add dependency: thisDir->usedDir
          //static int count=0;
          //printf("      %d: add dependency %s->%s\n",count++,qPrint(name()),qPrint(usedDir->name()));
          addUsesDependency(usedDir,fd,ii.fileDef,true,true);
        }
      }
    }
  }

  std::sort(m_usedDirs.begin(),m_usedDirs.end(),
            [](const auto &u1,const auto &u2)
            { return qstricmp(u1->dir()->getOutputFileBase(),u2->dir()->getOutputFileBase())<0; });

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

UsedDir::~UsedDir()
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
  std::sort(m_filePairs.begin(),
            m_filePairs.end(),
            [](const auto &left,const auto &right)
            {
              int orderHi = qstricmp(left->source()->name(),right->source()->name());
              if (orderHi!=0) return orderHi<0;
              int orderLo = qstricmp(left->destination()->name(),right->destination()->name());
              return orderLo<0;
            });
}

FilePair *UsedDir::findFilePair(const QCString &name)
{
  return m_filePairs.find(name);
}

DirDef *DirDefImpl::createNewDir(const QCString &path)
{
  ASSERT(path!=0);
  DirDef *dir = Doxygen::dirLinkedMap->find(path);
  if (dir==0) // new dir
  {
    dir = Doxygen::dirLinkedMap->add(path,
            std::unique_ptr<DirDef>(
              createDirDef(path)));
    //printf("Adding new dir %s\n",path);
    //printf("createNewDir %s short=%s\n",path,qPrint(dir->shortName()));
  }
  return dir;
}

bool DirDefImpl::matchPath(const QCString &path,const StringVector &l)
{
  for (const auto &s : l)
  {
    std::string prefix = s.substr(0,path.length());
    if (qstricmp(prefix.c_str(),path)==0) // case insensitive compare
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
  //printf("DirDefImpl::mergeDirectoryInTree(%s)\n",qPrint(path));
  int p=0,i=0;
  DirDef *dir=0;
  while ((i=path.find('/',p))!=-1)
  {
    QCString part=path.left(i+1);
    if (!matchPath(part,Config_getList(STRIP_FROM_PATH)) && (part!="/" && part!="//"))
    {
      dir=createNewDir(part);
    }
    p=i+1;
  }
  return dir;
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
  ol.disableAllBut(OutputGenerator::Html);

  QCString shortTitle=theTranslator->trDirRelation(
                      (m_src->shortName()+" &rarr; "+m_dst->dir()->shortName()));
  QCString title=theTranslator->trDirRelation(
                 (m_src->displayName()+" -> "+m_dst->dir()->shortName()));
  startFile(ol,getOutputFileBase(),getOutputFileBase(),
            title,HLI_None,!generateTreeView,m_src->getOutputFileBase());

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

  endFileWithNavPath(m_src,ol);

  ol.popGeneratorState();
}

//----------------------------------------------------------------------
// external functions

/** In order to create stable, but unique directory names,
 *  we compute the common part of the path shared by all directories.
 */
static void computeCommonDirPrefix()
{
  QCString path;
  auto it = Doxygen::dirLinkedMap->begin();
  if (!Doxygen::dirLinkedMap->empty()) // we have at least one dir
  {
    // start will full path of first dir
    path=(*it)->name();
    int i=path.findRev('/',path.length()-2);
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
        uint l = path.length();
        size_t count=0;
        for (const auto &dir : *Doxygen::dirLinkedMap)
        {
          QCString dirName = dir->name();
          if (dirName.length()>path.length())
          {
            if (dirName.left(l)!=path) // dirName does not start with path
            {
              i=path.findRev('/',l-2);
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
            l=path.length();
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
    //printf("set disk name: %s -> %s\n",qPrint(dir->name()),qPrint(diskName));
  }
}

void buildDirectories()
{
  // for each input file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      if (fd->getReference().isEmpty())
      {
        DirDef *dir;
        if ((dir=Doxygen::dirLinkedMap->find(fd->getPath()))==0) // new directory
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
    int i=name.findRev('/',name.length()-2);
    if (i>0)
    {
      DirDef *parent = Doxygen::dirLinkedMap->find(name.left(i+1));
      //if (parent==0) parent=root;
      if (parent)
      {
        parent->addSubDir(dir.get());
        //printf("DirDefImpl::addSubdir(): Adding subdir\n%s to\n%s\n",
        //  qPrint(dir->displayName()), qPrint(parent->displayName()));
      }
    }
  }

  // sort the directory contents
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    dir->sort();
  }

  // short the directories themselves
  std::sort(Doxygen::dirLinkedMap->begin(),
            Doxygen::dirLinkedMap->end(),
            [](const auto &d1,const auto &d2)
            {
              QCString s1 = d1->shortName(), s2 = d2->shortName();
              int i = qstricmp(s1,s2);
              if (i==0) // if sort name are equal, sort on full path
              {
                QCString n1 = d1->name(), n2 = d2->name();
                int n = qstricmp(n1,n2);
                return n < 0;
              }
              return i < 0;
            });

  // set the directory count identifier
  int dirCount=0;
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    dir->setDirCount(dirCount++);
  }

  computeCommonDirPrefix();
}

void computeDirDependencies()
{
  // compute nesting level for each directory
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    dir->setLevel();
  }

  // compute uses dependencies between directories
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    //printf("computeDependencies for %s: #dirs=%d\n",qPrint(dir->name()),Doxygen::directories.count());
    dir->computeDependencies();
  }
}

void generateDirDocs(OutputList &ol)
{
  for (const auto &dir : *Doxygen::dirLinkedMap)
  {
    ol.pushGeneratorState();
    if (!dir->hasDocumentation())
    {
      ol.disableAllBut(OutputGenerator::Html);
    }
    dir->writeDocumentation(ol);
    ol.popGeneratorState();
  }
  if (Config_getBool(DIRECTORY_GRAPH))
  {
    for (const auto &dr : Doxygen::dirRelations)
    {
      dr->writeDocumentation(ol);
    }
  }
}

bool compareDirDefs(const DirDef *item1, const DirDef *item2)
{
  return qstricmp(item1->shortName(),item2->shortName()) < 0;
}

// --- Cast functions

DirDef *toDirDef(Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(DirDefImpl))
  {
    return static_cast<DirDef*>(d);
  }
  else
  {
    return 0;
  }
}

const DirDef *toDirDef(const Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(DirDefImpl))
  {
    return static_cast<const DirDef*>(d);
  }
  else
  {
    return 0;
  }
}

