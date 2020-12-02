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

#ifndef FILEDEF_H
#define FILEDEF_H

#include <set>

#include "index.h"
#include <qlist.h>
#include <qintdict.h>
#include <qdict.h>
#include "definition.h"
#include "sortdict.h"
#include "memberlist.h"
#include "containers.h"
#include "classlist.h"

class MemberList;
class FileDef;
class FileList;
class ClassSDict;
class ClassDef;
class ClassList;
class MemberDef;
class OutputList;
class NamespaceDef;
class NamespaceSDict;
class MemberGroupSDict;
class PackageDef;
class DirDef;
class FTextStream;
class ClangTUParser;

// --- Set of files

using FileDefSet = std::set<const FileDef*>;

/** Class representing the data associated with a \#include statement. */
struct IncludeInfo
{
  IncludeInfo() : fileDef(0), local(FALSE), imported(FALSE) {}
  ~IncludeInfo() {}
  FileDef *fileDef;
  QCString includeName;
  bool local;
  bool imported;
};

/** A model of a file symbol.
 *
 *  An object of this class contains all file information that is gathered.
 *  This includes the members and compounds defined in the file.
 *
 *  The member writeDocumentation() can be used to generate the page of
 *  documentation to HTML and LaTeX.
 */
class FileDef : public DefinitionMutable, public Definition
{
  public:
   ~FileDef() {}

    // ----------------------------------------------------------------------

    virtual DefType definitionType() const = 0;

    /*! Returns the unique file name (this may include part of the path). */
    virtual QCString name() const = 0;
    virtual QCString displayName(bool=TRUE) const = 0;
    virtual QCString fileName() const = 0;

    virtual QCString getOutputFileBase() const = 0;

    virtual QCString anchor() const = 0;

    virtual QCString getSourceFileBase() const = 0;

    /*! Returns the name of the verbatim copy of this file (if any). */
    virtual QCString includeName() const = 0;

    virtual QCString includeDependencyGraphFileName() const = 0;

    virtual QCString includedByDependencyGraphFileName() const = 0;

    /*! Returns the absolute path including the file name. */
    virtual QCString absFilePath() const = 0;

    /*! Returns the name as it is used in the documentation */
    virtual const QCString &docName() const = 0;

    /*! Returns TRUE if this file is a source file. */
    virtual bool isSource() const = 0;

    virtual bool isDocumentationFile() const = 0;

    virtual Definition *getSourceDefinition(int lineNr) const = 0;
    virtual MemberDef *getSourceMember(int lineNr) const = 0;

    /*! Returns the absolute path of this file. */
    virtual QCString getPath() const = 0;

    /*! Returns version of this file. */
    virtual QCString getVersion() const = 0;

    virtual bool isLinkableInProject() const = 0;

    virtual bool isLinkable() const = 0;
    virtual bool isIncluded(const QCString &name) const = 0;

    virtual PackageDef *packageDef() const = 0;
    virtual DirDef *getDirDef() const = 0;
    virtual LinkedRefMap<const NamespaceDef> getUsedNamespaces() const = 0;
    virtual LinkedRefMap<const ClassDef> getUsedClasses() const = 0;
    virtual QList<IncludeInfo> *includeFileList() const = 0;
    virtual QList<IncludeInfo> *includedByFileList() const = 0;
    virtual void getAllIncludeFilesRecursively(StringVector &incFiles) const = 0;

    virtual MemberList *getMemberList(MemberListType lt) const = 0;
    virtual const QList<MemberList> &getMemberLists() const = 0;

    /* user defined member groups */
    virtual MemberGroupSDict *getMemberGroupSDict() const = 0;
    virtual NamespaceSDict *getNamespaceSDict() const = 0;
    virtual ClassSDict *getClassSDict() const = 0;

    virtual QCString title() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual QCString fileVersion() const = 0;

    virtual bool subGrouping() const = 0;

    virtual void countMembers() = 0;
    virtual int numDocMembers() const = 0;
    virtual int numDecMembers() const = 0;

    //---------------------------------

    virtual void addSourceRef(int line,Definition *d,MemberDef *md) = 0;

    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeMemberPages(OutputList &ol) = 0;
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const = 0;
    virtual void writeSummaryLinks(OutputList &ol) const = 0;
    virtual void writeTagFile(FTextStream &t) = 0;

    virtual void writeSourceHeader(OutputList &ol) = 0;
    virtual void writeSourceBody(OutputList &ol,ClangTUParser *clangParser) = 0;
    virtual void writeSourceFooter(OutputList &ol) = 0;
    virtual void parseSource(ClangTUParser *clangParser) = 0;
    virtual void setDiskName(const QCString &name) = 0;

    virtual void insertMember(MemberDef *md) = 0;
    virtual void insertClass(ClassDef *cd) = 0;
    virtual void insertNamespace(NamespaceDef *nd) = 0;
    virtual void computeAnchors() = 0;

    virtual void setPackageDef(PackageDef *pd) = 0;
    virtual void setDirDef(DirDef *dd) = 0;

    virtual void addUsingDirective(const NamespaceDef *nd) = 0;
    virtual void addUsingDeclaration(const ClassDef *cd) = 0;
    virtual void combineUsingRelations() = 0;

    virtual bool generateSourceFile() const = 0;
    virtual void sortMemberLists() = 0;

    virtual void addIncludeDependency(FileDef *fd,const char *incName,bool local,bool imported) = 0;
    virtual void addIncludedByDependency(FileDef *fd,const char *incName,bool local,bool imported) = 0;

    virtual void addMembersToMemberGroup() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void addIncludedUsingDirectives(FileDefSet &visitedFiles) = 0;

    virtual void addListReferences() = 0;
};

FileDef *createFileDef(const char *p,const char *n,const char *ref=0,const char *dn=0);


// --- Cast functions

FileDef            *toFileDef(Definition *d);
const FileDef      *toFileDef(const Definition *d);

// ------------------

/** Class representing a list of FileDef objects. */
class FileList : public QList<FileDef>
{
  public:
    FileList() : m_pathName("tmp") {}
    FileList(const char *path) : QList<FileDef>(), m_pathName(path) {}
   ~FileList() {}
    QCString path() const { return m_pathName; }
  private:
    int compareValues(const FileDef *md1,const FileDef *md2) const
    {
      return qstricmp(md1->name(),md2->name());
    }
    QCString m_pathName;
};

class OutputNameList : public QList<FileList>
{
  public:
    OutputNameList() : QList<FileList>() {}
   ~OutputNameList() {}
 private:
    int compareValues(const FileList *fl1,const FileList *fl2) const
    {
      return qstricmp(fl1->path(),fl2->path());
    }
};

class OutputNameDict : public QDict<FileList>
{
  public:
    OutputNameDict(uint size) : QDict<FileList>(size) {}
   ~OutputNameDict() {}
};

class Directory;

/** Class representing an entry (file or sub directory) in a directory */
class DirEntry
{
  public:
    enum EntryKind { Dir, File };
    DirEntry(DirEntry *parent,FileDef *fd)
       : m_parent(parent), m_name(fd->name()), m_kind(File), m_fd(fd),
         m_isLast(FALSE) { }
    DirEntry(DirEntry *parent,QCString name)
       : m_parent(parent), m_name(name), m_kind(Dir),
         m_fd(0), m_isLast(FALSE) { }
    virtual ~DirEntry() { }
    EntryKind kind() const { return m_kind; }
    FileDef *file()  const { return m_fd; }
    bool isLast() const    { return m_isLast; }
    void setLast(bool b)   { m_isLast=b; }
    DirEntry *parent() const { return m_parent; }
    QCString name() const  { return m_name; }
    QCString path() const  { return parent() ? parent()->path()+"/"+name() : name(); }

  protected:
    DirEntry *m_parent;
    QCString m_name;

  private:
    EntryKind m_kind;
    FileDef   *m_fd;
    bool m_isLast;
};

/** Class representing a directory tree of DirEntry objects. */
class Directory : public DirEntry
{
  public:
    Directory(Directory *parent,const QCString &name)
       : DirEntry(parent,name)
    { m_children.setAutoDelete(TRUE); }
    virtual ~Directory()              {}
    void addChild(DirEntry *d)        { m_children.append(d); d->setLast(TRUE); }
    QList<DirEntry> &children()       { return m_children; }
    void rename(const QCString &name) { m_name=name; }
    void reParent(Directory *parent)  { m_parent=parent; }

  private:
    QList<DirEntry> m_children;
};

void generateFileTree();

#endif

