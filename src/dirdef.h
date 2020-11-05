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

#include "sortdict.h"
#include "definition.h"

#include <vector>
#include <qglobal.h>
#include <qcstring.h>

class FileList;
class ClassSDict;
class QStrList;
class FileDef;
class OutputList;
class UsedDir;
class FTextStream;

class DirDef;

/** A list of directories. */
typedef std::vector<DirDef*> DirList;

bool compareDirDefs(const DirDef *item1, const DirDef *item2);

/** A model of a directory symbol. */
class DirDef : virtual public DefinitionMutable
{
  public:
    virtual ~DirDef() {}

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
    virtual const QDict<UsedDir> *usedDirs() const = 0;
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
    virtual void addUsesDependency(DirDef *usedDir,FileDef *srcFd,
                           FileDef *dstFd,bool inherited) = 0;
    virtual void computeDependencies() = 0;
};

/** Class representing a pair of FileDef objects */
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

/** A sorted dictionary of FilePair objects. */
class FilePairDict : public SDict<FilePair>
{
  public:
    FilePairDict(uint size) : SDict<FilePair>(size) {}
  private:
    int compareValues(const FilePair *item1,const FilePair *item2) const;
};

/** Usage information of a directory. */
class UsedDir
{
  public:
    UsedDir(const DirDef *dir,bool inherited);
    virtual ~UsedDir();
    void addFileDep(FileDef *srcFd,FileDef *dstFd);
    FilePair *findFilePair(const char *name);
    const FilePairDict &filePairs() const { return m_filePairs; }
    const DirDef *dir() const { return m_dir; }
    bool inherited() const { return m_inherited; }
    void sort();

  private:
    const DirDef *m_dir;
    FilePairDict m_filePairs;
    bool m_inherited;
};

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

/** A sorted dictionary of DirDef objects. */
class DirSDict : public SDict<DirDef>
{
  public:
    DirSDict(uint size) : SDict<DirDef>(size) {}
    int compareValues(const DirDef *item1,const DirDef *item2) const
    {
      return qstricmp(item1->shortName(),item2->shortName());
    }
};


void buildDirectories();
void generateDirDocs(OutputList &ol);
void computeDirDependencies();
void writeDirDependencyGraph(const char *file);

#endif
