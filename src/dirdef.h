/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#ifndef DIRDEF_H
#define DIRDEF_H

#include "qtbc.h"
#include "sortdict.h"
#include "definition.h"

#include <qlist.h>

class FileList;
class ClassSDict;
class QStrList;
class FileDef;
class OutputList;

class DirDef;

class DirList : public QList<DirDef>
{
  public:
   int compareItems(GCI item1,GCI item2);
};

class DirDef : public Definition
{
  public:
    DirDef(const char *path);
    virtual ~DirDef();
    virtual DefType definitionType() { return TypeDir; }
    virtual QCString getOutputFileBase() const;
    virtual bool isLinkableInProject() const { return !isReference() && hasDocumentation(); }
    virtual bool isLinkable() const { return isReference() || isLinkableInProject(); }
    QCString displayName() const { return m_dispName; }
    QCString shortName() const { return m_shortName; }
    void addSubDir(DirDef *subdir);
    FileList *   getFiles() const        { return m_fileList; }
    ClassSDict * getClasses() const      { return m_classSDict; }
    void addFile(FileDef *fd);
    void writeDetailedDocumentation(OutputList &ol);
    void writeDocumentation(OutputList &ol);
    void writeNavigationPath(OutputList &ol);
    const QList<DirDef> &subDirs() const { return m_subdirs; }
    

    static DirDef *mergeDirectoryInTree(const QCString &path);
    bool visited;

  private:
    void writePathFragment(OutputList &ol);
    static DirDef *createNewDir(const char *path);
    static bool matchPath(const QCString &path,QStrList &l);

    DirList m_subdirs;
    QCString m_dispName;
    QCString m_shortName;
    FileList *m_fileList;                 // list of files in the group
    ClassSDict *m_classSDict;             // list of classes in the group
    int m_dirCount;
};

inline int DirList::compareItems(GCI item1,GCI item2)
{
  return stricmp(((DirDef *)item1)->shortName(),((DirDef *)item2)->shortName());
}

class DirSDict : public SDict<DirDef>
{
  public:
    DirSDict(int size) : SDict<DirDef>(size) {}
    int compareItems(GCI item1,GCI item2)
    {
      return stricmp(((DirDef *)item1)->shortName(),((DirDef *)item2)->shortName());
    }
};


void buildDirectories();
void generateDirDocs(OutputList &ol);

#endif
