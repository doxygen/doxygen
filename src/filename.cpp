/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include "filename.h"
#include "util.h"

FileName::FileName(const char *n) : QList<FileDef>()
{
  name=n;
}

FileName::~FileName()
{
}

//static QCString convertSlashes(const char *s)
//{
//  QCString result=s;
//  int i,l=result.length();
//  for (i=0;i<l;i++) if (result.at(i)=='/') result.at(i)='_';
//  return result;
//}

void FileName::generateDiskNames()
{
  QCString commonPrefix;
  FileDef *fd=first();
  int count=0;
  while (fd) { if (!fd->isReference()) count++; fd=next(); }
  if (count==1)
  {
    fd=first();
    // skip references
    while (fd && fd->isReference()) fd=next();
    // name if unique, so diskname is simply the name
    fd->diskname=name;
  }
  else if (count>1) // multiple occurrences of the same file name
  {
    //printf("Multiple occurrences of %s\n",name.data());
    int i=0,j=0;
    bool found=FALSE;
    while (!found)
    {
      fd=first();
      while (fd && fd->isReference()) fd=next();
      char c=fd->path.at(i);
      if (c=='/') j=i; // remember last position of dirname
      while (fd && !found)
      {
        if (!fd->isReference())
        {
          //printf("i=%d fd->path=`%s' fd->name=`%s'\n",i,fd->path.data(),fd->name().data());
          if (i==(int)fd->path.length())
          {
            warning("Warning: Input file %s found multiple times!\n"
                    "         The generated documentation for this file may not be correct!\n",fd->absFilePath().data());
            found=TRUE;
          }
          else if (fd->path[i]!=c)
          {
            found=TRUE;  
          }
        } 
        fd=next();
      }
      i++;
    }
    fd=first();
    while (fd)
    {
      //printf("fd->setName(%s)\n",(fd->path.right(fd->path.length()-j-1)+name).data());
      fd->setName(fd->path.right(fd->path.length()-j-1)+name);
      fd->diskname=convertSlashes(fd->name());
      fd=next();
    }
  }
}

int FileName::compareItems(GCI item1, GCI item2)
{
  FileName *f1=(FileName *)item1;
  FileName *f2=(FileName *)item2;
  return stricmp(f1->fileName(),f2->fileName());
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
  FileName *fn=first();
  while (fn)
  {
    fn->generateDiskNames();
    fn=next();
  }
}

int FileNameList::compareItems(GCI item1, GCI item2)
{
  FileName *f1=(FileName *)item1;
  FileName *f2=(FileName *)item2;
  return stricmp(f1->fileName(),f2->fileName());
}

FileNameListIterator::FileNameListIterator(const FileNameList &fnlist) :
  QListIterator<FileName>(fnlist)
{
}
