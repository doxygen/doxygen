#include "md5.h"

#include "dirdef.h"
#include "filename.h"
#include "doxygen.h"
#include "util.h"
#include "outputlist.h"
#include "language.h"
#include "message.h"
#include "dot.h"
#include "layout.h"
#include "ftextstream.h"

//----------------------------------------------------------------------
// method implementation

static int g_dirCount=0;

DirDef::DirDef(const char *path) : Definition(path,1,path)
{
  bool fullPathNames = Config_getBool("FULL_PATH_NAMES");
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
  if (m_dispName.at(m_dispName.length()-1)=='/')
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

DirDef::~DirDef()
{
  delete m_fileList;
  delete m_usedDirs;
}

bool DirDef::isLinkableInProject() const 
{ 
  return !isReference(); 
}

bool DirDef::isLinkable() const 
{ 
  return isReference() || isLinkableInProject(); 
}

void DirDef::addSubDir(DirDef *subdir)
{
  m_subdirs.inSort(subdir);
  subdir->setOuterScope(this);
  subdir->m_parent=this;
}

void DirDef::addFile(FileDef *fd)
{
  m_fileList->inSort(fd);
  fd->setDirDef(this);
}

static QCString encodeDirName(const QCString &anchor)
{
  QCString result;

  // convert to md5 hash
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)anchor.data(),anchor.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.data(),33);
  return sigStr;

  // old algorithm

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

QCString DirDef::getOutputFileBase() const
{
  //printf("DirDef::getOutputFileBase() %s->dir_%s\n",
  //    m_diskName.data(),encodeDirName(m_diskName).data());
  return "dir_"+encodeDirName(m_diskName);
}

void DirDef::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
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
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
    }
    // separator between brief and details
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF") && 
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Man);
        ol.disable(OutputGenerator::RTF);
        // ol.newParagraph();  // FIXME:PARA
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
  }
}

void DirDef::writeBriefDescription(OutputList &ol)
{
  if (!briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
  {
    ol.startParagraph();
    ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::RTF);
    ol.writeString(" \n");
    ol.enable(OutputGenerator::RTF);

    if (Config_getBool("REPEAT_BRIEF") ||
        !documentation().isEmpty()
       )
    {
      ol.disableAllBut(OutputGenerator::Html);
      ol.startTextLink(0,"details");
      ol.parseText(theTranslator->trMore());
      ol.endTextLink();
    }
    ol.popGeneratorState();

    //ol.pushGeneratorState();
    //ol.disable(OutputGenerator::RTF);
    //ol.newParagraph();
    //ol.popGeneratorState();
    ol.endParagraph();
  }
  ol.writeSynopsis();
}

void DirDef::writeDirectoryGraph(OutputList &ol)
{
  // write graph dependency graph
  if (Config_getBool("DIRECTORY_GRAPH") && Config_getBool("HAVE_DOT"))
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

void DirDef::writeSubDirList(OutputList &ol)
{
  // write subdir list
  if (m_subdirs.count()>0)
  {
    ol.startMemberHeader("subdirs");
    ol.parseText(theTranslator->trDir(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    DirDef *dd=m_subdirs.first();
    while (dd)
    {
      ol.startMemberDeclaration();
      ol.startMemberItem(dd->getOutputFileBase(),0);
      ol.parseText(theTranslator->trDir(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),0,dd->shortName());
      ol.endMemberItem();
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>" << endl;
      }
      if (!dd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription(dd->getOutputFileBase());
        ol.parseDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),
            FALSE, // indexWords
            FALSE, // isExample
            0,     // exampleName
            TRUE,  // single line
            TRUE   // link from index
           );
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
      dd=m_subdirs.next();
    }

    ol.endMemberList();
  }
}

void DirDef::writeFileList(OutputList &ol)
{
  // write file list
  if (m_fileList->count()>0)
  {
    ol.startMemberHeader("files");
    ol.parseText(theTranslator->trFile(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    FileDef *fd=m_fileList->first();
    while (fd)
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
      }
      ol.endMemberItem();
      if (!fd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription(fd->getOutputFileBase());
        ol.parseDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),
            FALSE, // indexWords
            FALSE, // isExample
            0,     // exampleName
            TRUE,  // single line
            TRUE   // link from index
           );
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
      fd=m_fileList->next();
    }
    ol.endMemberList();
  }
}

void DirDef::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void DirDef::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void DirDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  ol.pushGeneratorState();
  
  QCString shortTitle=theTranslator->trDirReference(m_shortName);
  QCString title=theTranslator->trDirReference(m_dispName);
  startFile(ol,getOutputFileBase(),name(),title,HLI_None,!generateTreeView);

  if (!generateTreeView)
  {
    // write navigation path
    writeNavigationPath(ol);
    ol.endQuickIndices();
  }

  startTitle(ol,getOutputFileBase());
  ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.parseText(shortTitle);
    ol.enableAll();
    ol.disable(OutputGenerator::Html);
    ol.parseText(title);
  ol.popGeneratorState();
  endTitle(ol,getOutputFileBase(),title);
  ol.startContents();

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"dir\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(displayName()) << "</name>" << endl;
    Doxygen::tagFile << "    <path>" << convertToXML(name()) << "</path>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }
  
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
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileNamespaces:
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

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  ol.endContents();

  endFileWithNavPath(this,ol);

  ol.popGeneratorState();
}

void DirDef::setLevel()
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
void DirDef::addUsesDependency(DirDef *dir,FileDef *srcFd,
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
void DirDef::computeDependencies()
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
}

bool DirDef::isParentOf(DirDef *dir) const
{
  if (dir->parent()==this) // this is a parent of dir 
    return TRUE;
  else if (dir->parent()) // repeat for the parent of dir
    return isParentOf(dir->parent()); 
  else
    return FALSE;
}

bool DirDef::depGraphIsTrivial() const
{
  return FALSE;
}

//----------------------------------------------------------------------

int FilePairDict::compareItems(GCI item1,GCI item2)
{
  FilePair *left  = (FilePair*)item1;
  FilePair *right = (FilePair*)item2;
  int orderHi = stricmp(left->source()->name(),right->source()->name());
  int orderLo = stricmp(left->destination()->name(),right->destination()->name());
  return orderHi==0 ? orderLo : orderHi;
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
  m_filePairs.inSort(srcFd->getOutputFileBase()+dstFd->getOutputFileBase(),
                     new FilePair(srcFd,dstFd));
}

FilePair *UsedDir::findFilePair(const char *name)
{
  QCString n=name;
  return n.isEmpty() ? 0 : m_filePairs.find(n);
}

DirDef *DirDef::createNewDir(const char *path)
{
  ASSERT(path!=0);
  DirDef *dir = Doxygen::directories->find(path);
  if (dir==0) // new dir
  {
    //printf("Adding new dir %s\n",path);
    dir = new DirDef(path);
    //printf("createNewDir %s short=%s\n",path,dir->shortName().data());
    Doxygen::directories->inSort(path,dir);
  }
  return dir;
}

bool DirDef::matchPath(const QCString &path,QStrList &l)
{
  const char *s=l.first();
  while (s)
  {
    QCString prefix = s;
    if (stricmp(prefix.left(path.length()),path)==0) // case insensitive compare
    {
      return TRUE;
    }
    s = l.next();
  }
  return FALSE;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList("STRIP_FROM_PATH") list
 */
DirDef *DirDef::mergeDirectoryInTree(const QCString &path)
{
  //printf("DirDef::mergeDirectoryInTree(%s)\n",path.data());
  int p=0,i=0;
  DirDef *dir=0;
  while ((i=path.find('/',p))!=-1)
  {
    QCString part=path.left(i+1);
    if (!matchPath(part,Config_getList("STRIP_FROM_PATH")) && part!="/")
    {
      dir=createNewDir(part); 
    }
    p=i+1;
  }
  return dir;
}

void DirDef::writeDepGraph(FTextStream &t)
{
    writeDotDirDepGraph(t,this);
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
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
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
            if (strncmp(dirName,path,l)!=0) // dirName does not start with path
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
      if (fd->getReference().isEmpty() && !fd->isDocumentationFile())
      {
        DirDef *dir;
        if ((dir=Doxygen::directories->find(fd->getPath()))==0) // new directory
        {
          dir = DirDef::mergeDirectoryInTree(fd->getPath());
        }
        if (dir) dir->addFile(fd);
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
    //printf("New dir %s\n",dir->displayName().data());
    QCString name = dir->name();
    int i=name.findRev('/',name.length()-2);
    if (i>0)
    {
      DirDef *parent = Doxygen::directories->find(name.left(i+1));
      //if (parent==0) parent=root;
      if (parent) 
      {
        parent->addSubDir(dir); 
        //printf("DirDef::addSubdir(): Adding subdir\n%s to\n%s\n",
        //  dir->displayName().data(), parent->displayName().data());
      }
    }
  }
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
    dir->writeDocumentation(ol);
  }
  if (Config_getBool("DIRECTORY_GRAPH"))
  {
    SDict<DirRelation>::Iterator rdi(Doxygen::dirRelations);
    DirRelation *dr;
    for (rdi.toFirst();(dr=rdi.current());++rdi)
    {
      dr->writeDocumentation(ol);
    }
  }
}

