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
class FTextStream;

class DirDef;

/** A list of directories */
class DirList : public QList<DirDef>
{
  public:
   int compareItems(GCI item1,GCI item2);
};

/** A directory */
class DirDef : public Definition
{
  public:
    DirDef(const char *path);
    virtual ~DirDef();

    // accessors
    virtual DefType definitionType() const { return TypeDir; }
    virtual QCString getOutputFileBase() const;
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    QCString displayName() const { return m_dispName; }
    QCString shortName() const { return m_shortName; }
    void addSubDir(DirDef *subdir);
    FileList *   getFiles() const        { return m_fileList; }
    void addFile(FileDef *fd);
    const DirList &subDirs() const { return m_subdirs; }
    bool isCluster() const { return m_subdirs.count()>0; }
    int level() const { return m_level; }
    DirDef *parent() const { return m_parent; }
    int dirCount() const { return m_dirCount; }
    const QDict<UsedDir> *usedDirs() const { return m_usedDirs; }
    bool isParentOf(DirDef *dir) const;
    bool depGraphIsTrivial() const;

    // generate output
    void writeDocumentation(OutputList &ol);
    void writeDepGraph(FTextStream &t);

    static DirDef *mergeDirectoryInTree(const QCString &path);
    bool visited;

  private:
    friend void computeDirDependencies();

    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void writeDirectoryGraph(OutputList &ol);
    void writeSubDirList(OutputList &ol);
    void writeFileList(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);

    void setLevel();
    static DirDef *createNewDir(const char *path);
    static bool matchPath(const QCString &path,QStrList &l);
    void addUsesDependency(DirDef *usedDir,FileDef *srcFd,
                           FileDef *dstFd,bool inherited);
    void computeDependencies();

    DirList m_subdirs;
    QCString m_dispName;
    QCString m_shortName;
    FileList *m_fileList;                 // list of files in the group
    int m_dirCount;
    int m_level;
    DirDef *m_parent;
    QDict<UsedDir> *m_usedDirs;
};

class FilePair 
{
  public:
    FilePair(FileDef *src,FileDef *dst) : m_src(src), m_dst(dst) {}
    const FileDef *source() const { return m_src; }
    const FileDef *destination() const { return m_dst; }
  private:
    FileDef *m_src;
    FileDef *m_dst;
};

class FilePairDict : public SDict<FilePair>
{
  public:
    FilePairDict(int size) : SDict<FilePair>(size) {}
    int compareItems(GCI item1,GCI item2);
};

/** Usage information of a directory . */
class UsedDir
{
  public:
    UsedDir(DirDef *dir,bool inherited);
    virtual ~UsedDir();
    void addFileDep(FileDef *srcFd,FileDef *dstFd);
    FilePair *findFilePair(const char *name);
    const FilePairDict &filePairs() const { return m_filePairs; }
    const DirDef *dir() const { return m_dir; }
    bool inherited() const { return m_inherited; }

  private:
    DirDef *m_dir;
    FilePairDict m_filePairs;
    bool m_inherited;
};

/** A usage relation between two direction. */
class DirRelation
{
  public:
    DirRelation(const QCString &name,DirDef *src,UsedDir *dst) 
      : m_name(name), m_src(src), m_dst(dst) {}
    DirDef  *source() const      { return m_src; }
    UsedDir *destination() const { return m_dst; }
    void writeDocumentation(OutputList &ol);
    QCString getOutputFileBase() const { return m_name; }

  private:
    QCString m_name;
    DirDef  *m_src;
    UsedDir *m_dst;
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
