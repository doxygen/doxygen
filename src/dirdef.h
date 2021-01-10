/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include "linkedmap.h"
#include "definition.h"

#include <vector>
#include <map>
#include <qglobal.h>
#include <qcstring.h>

class FileList;
class QStrList;
class FileDef;
class OutputList;
class UsedDir;
class FTextStream;
class FilePair;
class FilePairDict;
class DirDef;
class DirList;

bool compareDirDefs(const DirDef *item1, const DirDef *item2);

// ------------------

/** Class representing a pair of FileDef objects */
class FilePair
{
  public:
    FilePair(const FileDef *src,const FileDef *dst) : m_src(src), m_dst(dst) {}
    const FileDef *source() const { return m_src; }
    const FileDef *destination() const { return m_dst; }
    static QCString key(const FileDef *srcFd,const FileDef *dstFd);
  private:
    const FileDef *m_src;
    const FileDef *m_dst;
};

// ------------------

/** A linked map of file pairs */
class FilePairLinkedMap : public LinkedMap<FilePair>
{
};

// ------------------

/** Usage information of a directory. */
class UsedDir
{
  public:
    UsedDir(const DirDef *dir,bool inherited);
    virtual ~UsedDir();
    void addFileDep(const FileDef *srcFd,const FileDef *dstFd);
    FilePair *findFilePair(const char *name);
    const FilePairLinkedMap &filePairs() const { return m_filePairs; }
    const DirDef *dir() const { return m_dir; }
    bool inherited() const { return m_inherited; }
    void sort();

  private:
    const DirDef *m_dir;
    FilePairLinkedMap m_filePairs;
    bool m_inherited;
};

// ------------------

/** A model of a directory symbol. */
class DirDef : public DefinitionMutable, public Definition
{
  public:
    virtual ~DirDef() {}

    class UsedDirLinkedMap : public LinkedMap<UsedDir> {};

    // accessors
    virtual DefType definitionType() const = 0;
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString anchor() const = 0;
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual QCString displayName(bool=TRUE) const = 0;
    virtual const QCString &shortName() const = 0;
    virtual void addSubDir(DirDef *subdir) = 0;
    virtual FileList *   getFiles() const = 0;
    virtual void addFile(FileDef *fd) = 0;
    virtual const DirList &subDirs() const = 0;
    virtual bool isCluster() const = 0;
    virtual int level() const = 0;
    virtual DirDef *parent() const = 0;
    virtual int dirCount() const = 0;
    virtual const UsedDirLinkedMap &usedDirs() const = 0;
    virtual bool isParentOf(const DirDef *dir) const = 0;
    virtual bool depGraphIsTrivial() const = 0;
    virtual QCString shortTitle() const = 0;
    virtual bool hasDetailedDescription() const = 0;

    // generate output
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeTagFile(FTextStream &t) = 0;

    virtual void setDiskName(const QCString &name) = 0;
    virtual void sort() = 0;
    virtual void setParent(DirDef *parent) = 0;
    virtual void setLevel() = 0;
    virtual void addUsesDependency(const DirDef *usedDir,const FileDef *srcFd,
                                   const FileDef *dstFd,bool inherited) = 0;
    virtual void computeDependencies() = 0;
};

// --- Cast functions

DirDef            *toDirDef(Definition *d);
const DirDef      *toDirDef(const Definition *d);

/** A usage relation between two directories. */
class DirRelation
{
  public:
    DirRelation(const QCString &name,const DirDef *src,UsedDir *dst)
      : m_name(name), m_src(src), m_dst(dst) {}
    const DirDef  *source() const      { return m_src; }
    UsedDir *destination() const { return m_dst; }
    void writeDocumentation(OutputList &ol);
    QCString getOutputFileBase() const { return m_name; }

  private:
    QCString m_name;
    const DirDef  *m_src;
    UsedDir *m_dst;
};

/** A linked map of directories */
class DirLinkedMap : public LinkedMap<DirDef>
{
};

/** A list of directories. */
class DirList : public std::vector<const DirDef*>
{
};

class DirRelationLinkedMap : public LinkedMap<DirRelation>
{
};

// ------------------

void buildDirectories();
void generateDirDocs(OutputList &ol);
void computeDirDependencies();
void writeDirDependencyGraph(const char *file);

#endif
