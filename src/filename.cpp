/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
  FileDef *fd=first();
  int count=0;
  while (fd) 
  { 
    if (!fd->isReference()) count++; 
    fd=next(); 
  }
  if (count==1)
  {
    fd=first();
    // skip references
    while (fd && fd->isReference()) fd=next();
    // name if unique, so diskname is simply the name
    //printf("!!!!!!!! Unique disk name=%s for fd=%s\n",name.data(),fd->diskname.data());
    fd->diskname=name.copy();
  }
  else if (count>1) // multiple occurrences of the same file name
  {
    //printf("Multiple occurrences of %s\n",name.data());
    int i=0,j=0;
    bool found=FALSE;
    while (!found) // search for the common prefix of all paths
    {
      fd=first();
      while (fd && fd->isReference()) fd=next();
      char c=fd->path.at(i);
      if (c=='/') j=i; // remember last position of dirname
      fd=next();
      while (fd && !found)
      {
        if (!fd->isReference())
        {
          //printf("i=%d j=%d fd->path=`%s' fd->name=`%s'\n",i,j,fd->path.left(i).data(),fd->name().data());
          if (i==(int)fd->path.length())
          {
            //warning("Warning: Input file %s found multiple times!\n"
            //        "         The generated documentation for this file may not be correct!\n",fd->absFilePath().data());
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
      if (!fd->isReference())
      {
        QCString prefix = fd->path.right(fd->path.length()-j-1);
        fd->setName(prefix+name);
        //printf("!!!!!!!! non unique disk name=%s for fd=%s\n",(prefix+name).data(),fd->diskname.data());
        fd->diskname=prefix+name;
      }
      fd=next();
    }
  }
}

int FileName::compareItems(QCollection::Item item1, QCollection::Item item2)
{
  FileName *f1=(FileName *)item1;
  FileName *f2=(FileName *)item2;
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
  FileName *fn=first();
  while (fn)
  {
    fn->generateDiskNames();
    fn=next();
  }
}

int FileNameList::compareItems(QCollection::Item item1, QCollection::Item item2)
{
  FileName *f1=(FileName *)item1;
  FileName *f2=(FileName *)item2;
  //printf("FileNameList::compareItems `%s'<->`%s'\n",
  //    f1->fileName(),f2->fileName());
  return Config_getBool("FULL_PATH_NAMES") ?
         qstricmp(f1->fullName(),f2->fullName()) :
         qstricmp(f1->fileName(),f2->fileName());
}

FileNameListIterator::FileNameListIterator(const FileNameList &fnlist) :
  QListIterator<FileName>(fnlist)
{
}

static bool getCaseSenseNames()
{
  static bool caseSenseNames = Config_getBool("CASE_SENSE_NAMES");
  return caseSenseNames;
}

FileNameDict::FileNameDict(uint size) : QDict<FileName>(size,getCaseSenseNames()) 
{
}

