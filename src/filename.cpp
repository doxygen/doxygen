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

#include "filename.h"
#include "util.h"
#include "config.h"

FileName::FileName(const char *fn,const char *n) : FileList()
{
  setAutoDelete(TRUE);
  fName=fn;
  name=n;
}

FileName::~FileName()
{
}


void FileName::generateDiskNames()
{
  //QCString commonPrefix;
  QListIterator<FileDef> it(*this);
  FileDef *fd;
  int count=0;
  for (;(fd=it.current());++it)
  {
    if (!fd->isReference()) count++;
  }
  if (count==1)
  {
    // skip references
    for (it.toFirst();(fd=it.current()) && fd->isReference();++it) { }
    if (fd)
    {
      // name if unique, so diskname is simply the name
      //printf("!!!!!!!! Unique disk name=%s for fd=%s\n",name.data(),fd->diskname.data());
      fd->setDiskName(name);
    }
  }
  else if (count>1) // multiple occurrences of the same file name
  {
    //printf("Multiple occurrences of %s\n",name.data());
    int i=0,j=0;
    bool found=FALSE;
    while (!found) // search for the common prefix of all paths
    {
      for (it.toFirst();(fd=it.current()) && fd->isReference();++it) { }
      if (fd)
      {
        char c=fd->getPath().at(i);
        if (c=='/') j=i; // remember last position of dirname
        ++it;
        while ((fd=it.current()) && !found)
        {
          QCString path = fd->getPath();
          if (!fd->isReference())
          {
            //printf("i=%d j=%d fd->path=`%s' fd->name=`%s'\n",i,j,fd->path.left(i).data(),fd->name().data());
            if (i==(int)path.length())
            {
              //warning("Input file %s found multiple times!\n"
              //        "         The generated documentation for this file may not be correct!\n",fd->absFilePath().data());
              found=TRUE;
            }
            else if (path[i]!=c)
            {
              found=TRUE;
            }
          }
          ++it;
        }
        i++;
      }
    }
    for (it.toFirst();(fd=it.current());++it)
    {
      //printf("fd->setName(%s)\n",(fd->path.right(fd->path.length()-j-1)+name).data());
      if (!fd->isReference())
      {
        QCString path   = fd->getPath();
        QCString prefix = path.right(path.length()-j-1);
        fd->setName(prefix+name);
        //printf("!!!!!!!! non unique disk name=%s:%s\n",prefix.data(),name.data());
        fd->setDiskName(prefix+name);
      }
    }
  }
}

int FileName::compareValues(const FileDef *f1, const FileDef *f2) const
{
  return qstricmp(f1->fileName(),f2->fileName());
}

FileNameIterator::FileNameIterator(const FileName &fname) :
  QListIterator<FileDef>(fname)
{
}

FileNameList::FileNameList() : QList<FileName>()
{
}

FileNameList::~FileNameList()
{
}

void FileNameList::generateDiskNames()
{
  FileNameListIterator it(*this);
  FileName *fn;
  for (;(fn=it.current());++it)
  {
    fn->generateDiskNames();
  }
}

int FileNameList::compareValues(const FileName *f1, const FileName *f2) const
{
  return Config_getBool(FULL_PATH_NAMES) ?
         qstricmp(f1->fullName(),f2->fullName()) :
         qstricmp(f1->fileName(),f2->fileName());
}

FileNameListIterator::FileNameListIterator(const FileNameList &fnlist) :
  QListIterator<FileName>(fnlist)
{
}

static bool getCaseSenseNames()
{
  static bool caseSenseNames = Config_getBool(CASE_SENSE_NAMES);
  return caseSenseNames;
}

FileNameDict::FileNameDict(uint size) : QDict<FileName>(size,getCaseSenseNames()) 
{
}

