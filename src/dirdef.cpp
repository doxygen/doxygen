#include "md5.h"

#include "dirdef.h"
#include "filename.h"
#include "doxygen.h"
#include "util.h"
#include "outputlist.h"
#include "language.h"
#include "message.h"
#include "dot.h"
#include "dotdirdeps.h"
#include "layout.h"
#include "ftextstream.h"
#include "config.h"
#include "docparser.h"
#include "definitionimpl.h"

//----------------------------------------------------------------------

class DirDefImpl : public DefinitionImpl, public DirDef
{
  public:
    DirDefImpl(const char *path);
    virtual ~DirDefImpl();

    virtual DefType definitionType() const { return TypeDir; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual QCString displayName(bool=TRUE) const { return m_dispName; }
    virtual const QCString &shortName() const { return m_shortName; }
    virtual void addSubDir(DirDef *subdir);
    virtual FileList *   getFiles() const        { return m_fileList; }
    virtual void addFile(FileDef *fd);
    virtual const DirList &subDirs() const { return m_subdirs; }
    virtual bool isCluster() const { return m_subdirs.count()>0; }
    virtual int level() const { return m_level; }
    virtual DirDef *parent() const { return m_parent; }
    virtual int dirCount() const { return m_dirCount; }
    virtual const QDict<UsedDir> *usedDirs() const { return m_usedDirs; }
    virtual bool isParentOf(const DirDef *dir) const;
    virtual bool depGraphIsTrivial() const;
    virtual QCString shortTitle() const;
    virtual bool hasDetailedDescription() const;
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeTagFile(FTextStream &t);
    virtual void setDiskName(const QCString &name) { m_diskName = name; }
    virtual void sort();
    virtual void setParent(DirDef *parent);
    virtual void setLevel();
    virtual void addUsesDependency(DirDef *usedDir,FileDef *srcFd,
                           FileDef *dstFd,bool inherited);
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

    static DirDef *createNewDir(const char *path);
    static bool matchPath(const QCString &path,QStrList &l);

    DirList m_subdirs;
    QCString m_dispName;
    QCString m_shortName;
    QCString m_diskName;
    FileList *m_fileList;                 // list of files in the group
    int m_dirCount;
    int m_level;
    DirDef *m_parent;
    QDict<UsedDir> *m_usedDirs;
};

DirDef *createDirDef(const char *path)
{
  return new DirDefImpl(path);
}


//----------------------------------------------------------------------
// method implementation

static int g_dirCount=0;

DirDefImpl::DirDefImpl(const char *path) : DefinitionImpl(path,1,1,path)
{
  bool fullPathNames = Config_getBool(FULL_PATH_NAMES);
  // get display name (stipping the paths mentioned in STRIP_FROM_PATH)
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
  
  m_fileList   = new FileList;
  m_usedDirs   = new QDict<UsedDir>(257);
  m_usedDirs->setAutoDelete(TRUE);
  m_dirCount   = g_dirCount++;
  m_level=-1;
  m_parent=0;
}

DirDefImpl::~DirDefImpl()
{
  delete m_fileList;
  delete m_usedDirs;
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
  m_subdirs.append(subdir);
  subdir->setOuterScope(this);
  subdir->setParent(this);
}

void DirDefImpl::setParent(DirDef *p)
{
   m_parent=p;
}

void DirDefImpl::addFile(FileDef *fd)
{
  m_fileList->append(fd);
  fd->setDirDef(this);
}

void DirDefImpl::sort()
{
  m_subdirs.sort();
  m_fileList->sort();
}

static QCString encodeDirName(const QCString &anchor)
{
  // convert to md5 hash
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)anchor.data(),anchor.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.rawData(),33);
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
  //    m_diskName.data(),encodeDirName(m_diskName).data());
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
      ol.writeAnchor(0,"details");
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();

    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
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
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
  }
}

void DirDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    DocRoot *rootNode = validatingParseDoc(
         briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
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
        ol.startTextLink(0,"details");
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

void DirDefImpl::writeDirectoryGraph(OutputList &ol)
{
  // write graph dependency graph
  if (Config_getBool(DIRECTORY_GRAPH) && Config_getBool(HAVE_DOT))
  {
    DotDirDeps dirDep(this);
    if (!dirDep.isTrivial())
    {
      msg("Generating dependency graph for directory %s\n",displayName().data());
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
  QListIterator<DirDef> it(m_subdirs);
  DirDef *dd;
  for (it.toFirst();(dd=it.current());++it)
  {
    if (dd->hasDocumentation() || dd->getFiles()->count()>0)
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
    for (it.toFirst();(dd=it.current());++it)
    {
      if (dd->hasDocumentation() || dd->getFiles()->count()==0)
      {
        ol.startMemberDeclaration();
        ol.startMemberItem(dd->getOutputFileBase(),0);
        ol.parseText(theTranslator->trDir(FALSE,TRUE)+" ");
        ol.insertMemberAlign();
        ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),0,dd->shortName());
        ol.endMemberItem();
        if (!dd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(dd->getOutputFileBase());
          ol.generateDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),
              FALSE, // indexWords
              FALSE, // isExample
              0,     // exampleName
              TRUE,  // single line
              TRUE   // link from index
              );
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(0,0);
      }
    }

    ol.endMemberList();
  }
}

void DirDefImpl::writeFileList(OutputList &ol)
{
  int numFiles = 0;
  QListIterator<FileDef> it(*m_fileList);
  FileDef *fd;
  for (it.toFirst();(fd=it.current());++it)
  {
    if (fd->hasDocumentation())
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
    QListIterator<FileDef> it(*m_fileList);
    FileDef *fd;
    for (;(fd=it.current());++it)
    {
      if (fd->hasDocumentation())
      {
        ol.startMemberDeclaration();
        ol.startMemberItem(fd->getOutputFileBase(),0);
        ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
        ol.insertMemberAlign();
        if (fd->isLinkable())
        {
          ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
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
          ol.startTextLink(fd->includeName(),0);
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
              0,     // exampleName
              TRUE,  // single line
              TRUE   // link from index
              );
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(0,0);
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
  static bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return (!briefDescription().isEmpty() && repeatBrief) || !documentation().isEmpty();
}

void DirDefImpl::writeTagFile(FTextStream &tagFile)
{
  tagFile << "  <compound kind=\"dir\">" << endl;
  tagFile << "    <name>" << convertToXML(displayName()) << "</name>" << endl;
  tagFile << "    <path>" << convertToXML(name()) << "</path>" << endl;
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Directory));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::DirSubDirs:
        {
          if (m_subdirs.count()>0)
          {
            DirDef *dd;
            QListIterator<DirDef> it(m_subdirs);
            for (;(dd=it.current());++it)
            {
              tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>" << endl;
            }
          }
        }
        break;
      case LayoutDocEntry::DirFiles:
        {
          if (m_fileList->count()>0)
          {
            QListIterator<FileDef> it(*m_fileList);
            FileDef *fd;
            for (;(fd=it.current());++it)
            {
              tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
            }
          }
        }
        break;
      default:
        break;
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>" << endl;
}

void DirDefImpl::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
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
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Directory));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
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
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDetailedDescription(ol,ls->title(lang));
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
      case LayoutDocEntry::NamespaceInterfaces:
      case LayoutDocEntry::NamespaceStructs:
      case LayoutDocEntry::NamespaceExceptions:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::FileClasses:
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
 */ 
void DirDefImpl::addUsesDependency(DirDef *dir,FileDef *srcFd,
                               FileDef *dstFd,bool inherited)
{
  if (this==dir) return; // do not add self-dependencies
  //static int count=0;
  //printf("  %d add dependency %s->%s due to %s->%s\n",
  //    count++,shortName().data(),
  //    dir->shortName().data(),
  //    srcFd->name().data(),
  //    dstFd->name().data());

  // levels match => add direct dependency
  bool added=FALSE;
  UsedDir *usedDir = m_usedDirs->find(dir->getOutputFileBase());
  if (usedDir) // dir dependency already present
  {
     FilePair *usedPair = usedDir->findFilePair(
         srcFd->getOutputFileBase()+dstFd->getOutputFileBase());
     if (usedPair==0) // new file dependency
     {
       //printf("  => new file\n");
       usedDir->addFileDep(srcFd,dstFd); 
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
    usedDir = new UsedDir(dir,inherited);
    usedDir->addFileDep(srcFd,dstFd); 
    m_usedDirs->insert(dir->getOutputFileBase(),usedDir);
    added=TRUE;
  }
  if (added)
  {
    if (dir->parent())
    {
      // add relation to parent of used dir
      addUsesDependency(dir->parent(),srcFd,dstFd,inherited);
    }
    if (parent())
    {
      // add relation for the parent of this dir as well
      parent()->addUsesDependency(dir,srcFd,dstFd,TRUE);
    }
  }
}

/** Computes the dependencies between directories
 */
void DirDefImpl::computeDependencies()
{
  FileList *fl = m_fileList;
  if (fl) 
  {
    QListIterator<FileDef> fli(*fl);
    FileDef *fd;
    for (fli.toFirst();(fd=fli.current());++fli) // foreach file in dir dd
    {
      //printf("  File %s\n",fd->name().data());
      //printf("** dir=%s file=%s\n",shortName().data(),fd->name().data());
      QList<IncludeInfo> *ifl = fd->includeFileList();
      if (ifl)
      {
        QListIterator<IncludeInfo> ifli(*ifl); 
        IncludeInfo *ii;
        for (ifli.toFirst();(ii=ifli.current());++ifli) // foreach include file
        {
          //printf("  > %s\n",ii->includeName.data());
          //printf("    #include %s\n",ii->includeName.data());
          if (ii->fileDef && ii->fileDef->isLinkable()) // linkable file
          {
            DirDef *usedDir = ii->fileDef->getDirDef();
            if (usedDir)
            {
              // add dependency: thisDir->usedDir
              //static int count=0;
              //printf("      %d: add dependency %s->%s\n",count++,name().data(),usedDir->name().data());
              addUsesDependency(usedDir,fd,ii->fileDef,FALSE);
            }
          }
        }
      }
    }
  }
  if (m_usedDirs)
  {
    QDictIterator<UsedDir> udi(*m_usedDirs);
    UsedDir *udir;
    for (udi.toFirst();(udir=udi.current());++udi)
    {
      udir->sort();
    }
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
  return m_usedDirs->count()==0;
}

//----------------------------------------------------------------------

int FilePairDict::compareValues(const FilePair *left,const FilePair *right) const
{
  int orderHi = qstricmp(left->source()->name(),right->source()->name());
  if (orderHi!=0) return orderHi;
  int orderLo = qstricmp(left->destination()->name(),right->destination()->name());
  return orderLo;
}

//----------------------------------------------------------------------

UsedDir::UsedDir(DirDef *dir,bool inherited) :
   m_dir(dir), m_filePairs(7), m_inherited(inherited)
{
  m_filePairs.setAutoDelete(TRUE);
}

UsedDir::~UsedDir()
{
}


void UsedDir::addFileDep(FileDef *srcFd,FileDef *dstFd)
{
  m_filePairs.append(srcFd->getOutputFileBase()+dstFd->getOutputFileBase(),
                     new FilePair(srcFd,dstFd));
}

void UsedDir::sort()
{
  m_filePairs.sort();
}

FilePair *UsedDir::findFilePair(const char *name)
{
  QCString n=name;
  return n.isEmpty() ? 0 : m_filePairs.find(n);
}

DirDef *DirDefImpl::createNewDir(const char *path)
{
  ASSERT(path!=0);
  DirDef *dir = Doxygen::directories->find(path);
  if (dir==0) // new dir
  {
    //printf("Adding new dir %s\n",path);
    dir = createDirDef(path);
    //printf("createNewDir %s short=%s\n",path,dir->shortName().data());
    Doxygen::directories->append(path,dir);
  }
  return dir;
}

bool DirDefImpl::matchPath(const QCString &path,QStrList &l)
{
  const char *s=l.first();
  while (s)
  {
    QCString prefix = s;
    if (qstricmp(prefix.left(path.length()),path)==0) // case insensitive compare
    {
      return TRUE;
    }
    s = l.next();
  }
  return FALSE;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList(STRIP_FROM_PATH) list
 */
DirDef *DirDefImpl::mergeDirectoryInTree(const QCString &path)
{
  //printf("DirDefImpl::mergeDirectoryInTree(%s)\n",path.data());
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

static void writePartialDirPath(OutputList &ol,const DirDef *root,const DirDef *target)
{
  if (target->parent()!=root) 
  {
    writePartialDirPath(ol,root,target->parent());
    ol.writeString("&#160;/&#160;");
  }
  ol.writeObjectLink(target->getReference(),target->getOutputFileBase(),0,target->shortName());
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
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
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
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  QCString shortTitle=theTranslator->trDirRelation(
                      m_src->shortName()+" &rarr; "+
                      m_dst->dir()->shortName());
  QCString title=theTranslator->trDirRelation(
                 m_src->displayName()+" -> "+
                 m_dst->dir()->shortName());
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

  SDict<FilePair>::Iterator fpi(m_dst->filePairs());
  FilePair *fp;
  for (fpi.toFirst();(fp=fpi.current());++fpi)
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
  DirDef *dir;
  DirSDict::Iterator sdi(*Doxygen::directories);
  if (Doxygen::directories->count()>0) // we have at least one dir
  {
    // start will full path of first dir
    sdi.toFirst();
    dir=sdi.current();
    path=dir->name();
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
        int l = path.length();
        int count=0;
        for (sdi.toFirst();(dir=sdi.current());++sdi)
        {
          QCString dirName = dir->name();
          if (dirName.length()>path.length())
          {
            if (qstrncmp(dirName,path,l)!=0) // dirName does not start with path
            {
              int i=path.findRev('/',l-2);
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
            int i=path.findRev('/',l-2);
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
        if (count==Doxygen::directories->count())
          // path matches for all directories -> found the common prefix
        {
          done=TRUE;
        }
      }
    }
  }
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    QCString diskName = dir->name().right(dir->name().length()-path.length());
    dir->setDiskName(diskName);
    //printf("set disk name: %s -> %s\n",dir->name().data(),diskName.data());
  }
}

void buildDirectories()
{
  // for each input file
  FileNameListIterator fnli(*Doxygen::inputNameList); 
  FileName *fn;
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      //printf("buildDirectories %s\n",fd->name().data());
      if (fd->getReference().isEmpty())
      {
        DirDef *dir;
        if ((dir=Doxygen::directories->find(fd->getPath()))==0) // new directory
        {
          dir = DirDefImpl::mergeDirectoryInTree(fd->getPath());
        }
        if (dir && !fd->isDocumentationFile()) dir->addFile(fd);
      }
      else
      {
        // do something for file imported via tag files.
      }
    }
  }

  //DirDef *root = new DirDef("root:");
  // compute relations between directories => introduce container dirs.
  DirDef *dir;
  DirSDict::Iterator sdi(*Doxygen::directories);
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    QCString name = dir->name();
    int i=name.findRev('/',name.length()-2);
    if (i>0)
    {
      DirDef *parent = Doxygen::directories->find(name.left(i+1));
      //if (parent==0) parent=root;
      if (parent) 
      {
        parent->addSubDir(dir); 
        //printf("DirDefImpl::addSubdir(): Adding subdir\n%s to\n%s\n",
        //  dir->displayName().data(), parent->displayName().data());
      }
    }
  }
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    dir->sort();
  }
  Doxygen::directories->sort();
  computeCommonDirPrefix();
}

void computeDirDependencies()
{
  DirDef *dir;
  DirSDict::Iterator sdi(*Doxygen::directories);
  // compute nesting level for each directory
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    dir->setLevel();
  }
  // compute uses dependencies between directories
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    //printf("computeDependencies for %s: #dirs=%d\n",dir->name().data(),Doxygen::directories.count());
    dir->computeDependencies();
  }

}

void generateDirDocs(OutputList &ol)
{
  DirDef *dir;
  DirSDict::Iterator sdi(*Doxygen::directories);
  for (sdi.toFirst();(dir=sdi.current());++sdi)
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
    SDict<DirRelation>::Iterator rdi(Doxygen::dirRelations);
    DirRelation *dr;
    for (rdi.toFirst();(dr=rdi.current());++rdi)
    {
      dr->writeDocumentation(ol);
    }
  }
}

