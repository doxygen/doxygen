#include "dirdef.h"
#include "filename.h"
#include "doxygen.h"
#include "util.h"
#include "outputlist.h"
#include "language.h"

//----------------------------------------------------------------------
// method implementation

static int g_dirCount=0;

DirDef::DirDef(const char *path) : Definition(path,1,path)
{
  // get display name (stipping the paths mentioned in STRIP_FROM_PATH)
  m_dispName = stripFromPath(path);
  // get short name (last part of path)
  m_shortName = path;
  if (m_shortName.at(m_shortName.length()-1)=='/')
  { // strip trailing /
    m_shortName = m_shortName.left(m_shortName.length()-1);
  }
  int pi=m_shortName.findRev('/');
  if (pi!=-1) 
  { // remove everything till the last /
    m_shortName = m_shortName.mid(pi+1);
  }
  
  m_subdirs.setAutoDelete(TRUE);
  m_fileList = new FileList;
  m_classSDict = new ClassSDict(17);
  m_dirCount = g_dirCount++;
}

DirDef::~DirDef()
{
}

void DirDef::addSubDir(DirDef *subdir)
{
  m_subdirs.inSort(subdir);
  subdir->setOuterScope(this);
}

void DirDef::addFile(FileDef *fd)
{
  m_fileList->inSort(fd);
  fd->setDirDef(this);
}

QCString DirDef::getOutputFileBase() const
{
  //return "dir_"+convertNameToFile(name());
  return QCString().sprintf("dir_%06d",m_dirCount);
}

void DirDef::writeDetailedDocumentation(OutputList &ol)
{
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.pushGeneratorState();
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::RTF);
    ol.writeAnchor(0,"_details");
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(theTranslator->trDetailedDescription());
    ol.endGroupHeader();

    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
      ol.newParagraph();
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
  }
}

void DirDef::writeDocumentation(OutputList &ol)
{
  ol.pushGeneratorState();
  
  QCString shortTitle=theTranslator->trDirReference(m_shortName);
  QCString title=theTranslator->trDirReference(m_dispName);
  startFile(ol,getOutputFileBase(),name(),title);

  // write navigation path
  writeNavigationPath(ol);

  startTitle(ol,getOutputFileBase());
  ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.parseText(shortTitle);
    ol.enableAll();
    ol.disable(OutputGenerator::Html);
    ol.parseText(title);
  ol.popGeneratorState();
  endTitle(ol,getOutputFileBase(),title);

  // write brief or details (if DETAILS_AT_TOP)
  if (Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
    ol.newParagraph();
  }
  else if (!briefDescription().isEmpty())
  {
    ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.disable(OutputGenerator::Man);
    ol.startTextLink(0,"_details");
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();
    ol.disableAllBut(OutputGenerator::Man);
    ol.newParagraph();
    ol.popGeneratorState();
  }

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"dir\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(displayName()) << "</name>" << endl;
    Doxygen::tagFile << "    <path>" << convertToXML(name()) << "</path>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }
  

  ol.startMemberSections();
  // write subdir list
  if (m_subdirs.count()>0)
  {
    ol.startMemberHeader();
    ol.parseText(theTranslator->trDir(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    DirDef *dd=m_subdirs.first();
    while (dd)
    {
      ol.startMemberItem(0);
      ol.parseText(theTranslator->trDir(FALSE,TRUE));
      ol.insertMemberAlign();
      ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),0,dd->shortName());
      ol.endMemberItem();
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>" << endl;
      }
      if (!dd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.newParagraph();
      }
      dd=m_subdirs.next();
    }

    ol.endMemberList();
  }
  
  // write file list
  if (m_fileList->count()>0)
  {
    ol.startMemberHeader();
    ol.parseText(theTranslator->trFile(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    FileDef *fd=m_fileList->first();
    while (fd)
    {
      ol.startMemberItem(0);
      ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      if (fd->isLinkable())
      {
        ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      }
      else
      {
        ol.startBold();
        ol.writeString(fd->name()); 
        ol.endBold();
      }
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
      }
      ol.endMemberItem();
      if (!fd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.newParagraph();
      }
      fd=m_fileList->next();
    }
    ol.endMemberList();
  }
  ol.endMemberSections();

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }


  if (!Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
  }
  
  endFile(ol); 
  ol.popGeneratorState();
}

void DirDef::writePathFragment(OutputList &ol)
{
  if (getOuterScope()!=Doxygen::globalScope && 
      getOuterScope()->definitionType()==Definition::TypeDir)
  {
    //printf("getOuterScope %s\n",getOuterScope()->name().data());
    ((DirDef*)getOuterScope())->writePathFragment(ol);
    ol.writeString("&nbsp;/&nbsp;");
  }
  ol.writeObjectLink(getReference(),getOutputFileBase(),0,shortName());
}

void DirDef::writeNavigationPath(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  ol.writeString("<div class=\"nav\">\n");
  writePathFragment(ol);
  ol.writeString("</div>\n");

  ol.popGeneratorState();
}

//----------------------------------------------------------------------
// helper functions

DirDef *DirDef::createNewDir(const char *path)
{
  ASSERT(path!=0);
  DirDef *dir = Doxygen::directories.find(path);
  if (dir==0) // new dir
  {
    //printf("Adding new dir %s\n",path);
    dir = new DirDef(path);
    //printf("createNewDir %s short=%s\n",path,dir->shortName().data());
    Doxygen::directories.inSort(path,dir);
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

//----------------------------------------------------------------------
// external functions

void buildDirectories()
{
  // for each input file
  FileNameListIterator fnli(Doxygen::inputNameList); 
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
        if ((dir=Doxygen::directories.find(fd->getPath()))==0) // new directory
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
  DirSDict::Iterator sdi(Doxygen::directories);
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    //printf("New dir %s\n",dir->displayName().data());
    QCString name = dir->name();
    int i=name.findRev('/',name.length()-2);
    if (i>0)
    {
      DirDef *parent = Doxygen::directories.find(name.left(i+1));
      //if (parent==0) parent=root;
      if (parent) 
      {
        parent->addSubDir(dir); 
        //printf("DirDef::addSubdir(): Adding subdir\n%s to\n%s\n",
        //  dir->displayName().data(), parent->displayName().data());
      }
    }
  }
}

void generateDirDocs(OutputList &ol)
{
  DirDef *dir;
  DirSDict::Iterator sdi(Doxygen::directories);
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    dir->writeDocumentation(ol);
  }
}

