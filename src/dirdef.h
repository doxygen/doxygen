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
class UsedDir;
class QTextStream;

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

    // accessors
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
    const QList<DirDef> &subDirs() const { return m_subdirs; }
    bool isCluster() const { return m_subdirs.count()>0; }
    int level() const { return m_level; }
    DirDef *parent() const { return m_parent; }
    const QDict<UsedDir> *usedDirs() const { return m_usedDirs; }
    bool isParentOf(DirDef *dir) const;

    // generate output
    void writeDetailedDocumentation(OutputList &ol);
    void writeDocumentation(OutputList &ol);
    void writeNavigationPath(OutputList &ol);
    void writeDepGraph(QTextStream &t);

    static DirDef *mergeDirectoryInTree(const QCString &path);
    bool visited;

  private:
    friend void computeDirDependencies();
    void writePathFragment(OutputList &ol);
    void setLevel();
    static DirDef *createNewDir(const char *path);
    static bool matchPath(const QCString &path,QStrList &l);
    void addUsesDependency(DirDef *usedDir,FileDef *fd,bool inherited);
    void computeDependencies();

    DirList m_subdirs;
    QCString m_dispName;
    QCString m_shortName;
    FileList *m_fileList;                 // list of files in the group
    ClassSDict *m_classSDict;             // list of classes in the group
    int m_dirCount;
    int m_level;
    DirDef *m_parent;
    QDict<UsedDir> *m_usedDirs;
};

class UsedDir
{
  public:
    UsedDir(DirDef *dir,bool inherited);
    virtual ~UsedDir();
    void addFile(FileDef *fd);
    FileDef *findFile(const char *name);
    const QDict<FileDef> &files() const { return m_files; }
    const DirDef *dir() const { return m_dir; }
    bool inherited() const { return m_inherited; }

  private:
    DirDef *m_dir;
    QDict<FileDef> m_files;
    bool m_inherited;
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
void computeDirDependencies();
void writeDirDependencyGraph(const char *file);

#endif
