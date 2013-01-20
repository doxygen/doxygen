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

#ifndef FILENAME_H
#define FILENAME_H

#include <qdict.h>
#include <qlist.h>
#include "filedef.h"

/** Class representing all files with a certain base name */
class FileName : public FileList
{ 
  public:
    FileName(const char *fn,const char *name);
   ~FileName();
    const char *fileName() const { return name; }
    const char *fullName() const { return fName; }
    void generateDiskNames();
    int compareItems(QCollection::Item item1,QCollection::Item item2);

  private:
    QCString name;
    QCString fName;
};

/** Iterator for FileDef objects in a FileName list. */
class FileNameIterator : public QListIterator<FileDef>
{
  public:
    FileNameIterator(const FileName &list);
};

/** Class representing a list of FileName objects. */
class FileNameList : public QList<FileName>
{
  public:
    FileNameList();
   ~FileNameList();
    void generateDiskNames();
    int compareItems(QCollection::Item item1,QCollection::Item item2);
};

/** Iterator for FileName objects in a FileNameList. */
class FileNameListIterator : public QListIterator<FileName>
{
  public:
    FileNameListIterator( const FileNameList &list );
};

/** Unsorted dictionary of FileName objects. */
class FileNameDict : public QDict<FileName>
{
  public:
    FileNameDict(uint size);
   ~FileNameDict() {}
};

#endif
