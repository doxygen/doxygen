#include "dirdef.h"
#include "filename.h"
#include "doxygen.h"
#include "util.h"

//----------------------------------------------------------------------
// method implementation

DirDef::DirDef(const char *path) : Definition(path,1,path),
   m_parent(0)
{
  // get short name (stipping the paths mentioned in STRIP_FROM_PATH)
  m_dispName = stripFromPath(path);
  m_subdirs.setAutoDelete(TRUE);
  m_fileList = new FileList;
  m_classSDict = new ClassSDict(17);
}

DirDef::~DirDef()
{
}

void DirDef::addSubDir(DirDef *subdir)
{
  m_subdirs.append(subdir);
  subdir->m_parent=this;
}

void DirDef::addFile(FileDef *fd)
{
  m_fileList->append(fd);
  fd->setDirDef(this);
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
  int p=0,i=0;
  DirDef *dir=0;
  while ((i=path.find('/',p))!=-1)
  {
    QCString part=path.left(i+1);
    if (!matchPath(part,Config_getList("STRIP_FROM_PATH")))
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
      if (!fd->getReference().isEmpty())
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

  DirDef *root = new DirDef("root:");
  // compute relations between directories => introduce container dirs.
  DirDef *dir;
  SDict<DirDef>::Iterator sdi(Doxygen::directories);
  for (sdi.toFirst();(dir=sdi.current());++sdi)
  {
    printf("New dir %s\n",dir->displayName().data());
    QCString name = dir->name();
    int i=name.findRev('/',name.length()-2);
    if (i!=-1)
    {
      DirDef *parent = Doxygen::directories.find(name.left(i+1));
      if (parent==0) parent=root;
      parent->addSubDir(dir);
      printf("DirDef::addSubdir(): Adding subdir\n%s to\n%s\n",
          dir->displayName().data(), parent->displayName().data());
    }
  }
}

