/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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

#ifndef FILENAME_H
#define FILENAME_H

#include "qtbc.h"
#include <qdict.h>
#include "filedef.h"

class FileName : public FileList
{ 
  public:
    FileName(const char *fn,const char *name);
   ~FileName();
    const char *fileName() const { return name; }
    const char *fullName() const { return fName; }
    void generateDiskNames();
    int compareItems(GCI item1,GCI item2);

  private:
    QCString name;
    QCString fName;
};

class FileNameIterator : public QListIterator<FileDef>
{
  public:
    FileNameIterator(const FileName &list);
};

class FileNameList : public QList<FileName>
{
  public:
    FileNameList();
   ~FileNameList();
    void generateDiskNames();
    int compareItems(GCI item1,GCI item2);
};

class FileNameListIterator : public QListIterator<FileName>
{
  public:
    FileNameListIterator( const FileNameList &list );
};

class FileNameDict : public QDict<FileName>
{
  public:
    FileNameDict(uint size) : 
       QDict<FileName>(size,Config_getBool("CASE_SENSE_NAMES")) {}
   ~FileNameDict() {}
};

#endif
