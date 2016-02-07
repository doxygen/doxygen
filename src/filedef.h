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

#include "index.h"
#include <qlist.h>
#include <qintdict.h>
#include <qdict.h>
#include "definition.h"
#include "sortdict.h"
#include "memberlist.h"

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

/** Class representing the data associated with a \#include statement. */
struct IncludeInfo
{
  IncludeInfo() : fileDef(0), local(FALSE), imported(FALSE), indirect(FALSE) {}
  ~IncludeInfo() {}
  FileDef *fileDef;
  QCString includeName;
  bool local;
  bool imported;
  bool indirect;
};

/** A model of a file symbol. 
 *   
 *  An object of this class contains all file information that is gathered.
 *  This includes the members and compounds defined in the file.
 *   
 *  The member writeDocumentation() can be used to generate the page of
 *  documentation to HTML and LaTeX.
 */
class FileDef : public Definition
{
  public:
    //enum FileType { Source, Header, Unknown };

    FileDef(const char *p,const char *n,const char *ref=0,const char *dn=0);
   ~FileDef();

    // ----------------------------------------------------------------------

    DefType definitionType() const { return TypeFile; }

    /*! Returns the unique file name (this may include part of the path). */
    QCString name() const;
    QCString displayName(bool=TRUE) const { return name(); }
    QCString fileName() const { return m_fileName; }
    
    QCString getOutputFileBase() const;

    QCString anchor() const { return QCString(); }

    QCString getSourceFileBase() const;
    
    /*! Returns the name of the verbatim copy of this file (if any). */
    QCString includeName() const;

    QCString includeDependencyGraphFileName() const;

    QCString includedByDependencyGraphFileName() const;
    
    /*! Returns the absolute path including the file name. */
    QCString absFilePath() const { return m_filePath; }
    
    /*! Returns the name as it is used in the documentation */
    const QCString &docName() const { return m_docname; }

    /*! Returns TRUE if this file is a source file. */
    bool isSource() const { return m_isSource; }

    bool isDocumentationFile() const;
    
    Definition *getSourceDefinition(int lineNr) const;
    MemberDef *getSourceMember(int lineNr) const;

    /*! Returns the absolute path of this file. */ 
    QCString getPath() const { return m_path; }

    /*! Returns version of this file. */
    QCString getVersion() const { return m_fileVersion; }
    
    bool isLinkableInProject() const;

    bool isLinkable() const { return isLinkableInProject() || isReference(); }
    bool isIncluded(const QCString &name) const;

    PackageDef *packageDef() const { return m_package; }
    DirDef *getDirDef() const      { return m_dir; }
    NamespaceSDict *getUsedNamespaces() const;
    SDict<Definition> *getUsedClasses() const      { return m_usingDeclList; }
    QList<IncludeInfo> *includeFileList() const    { return m_includeList; }
    QList<IncludeInfo> *includedByFileList() const { return m_includedByList; }
    void getAllIncludeFilesRecursively(QStrList &incFiles) const;

    MemberList *getMemberList(MemberListType lt) const;
    const QList<MemberList> &getMemberLists() const { return m_memberLists; }

    /* user defined member groups */
    MemberGroupSDict *getMemberGroupSDict() const { return m_memberGroupSDict; }
    NamespaceSDict *getNamespaceSDict() const     { return m_namespaceSDict; }
    ClassSDict *getClassSDict() const             { return m_classSDict; }

    QCString title() const;
    bool hasDetailedDescription() const;
    QCString fileVersion() const;

    bool subGrouping() const { return m_subGrouping; }
    
    //---------------------------------

    void addSourceRef(int line,Definition *d,MemberDef *md);

    void writeDocumentation(OutputList &ol);
    void writeMemberPages(OutputList &ol);
    void writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const;
    void writeSummaryLinks(OutputList &ol);
    void writeTagFile(FTextStream &t);

    void startParsing();
    void writeSource(OutputList &ol,bool sameTu,QStrList &filesInSameTu);
    void parseSource(bool sameTu,QStrList &filesInSameTu);
    void finishParsing();
    void setDiskName(const QCString &name);

    void insertMember(MemberDef *md);
    void insertClass(ClassDef *cd);
    void insertNamespace(NamespaceDef *nd);
    void computeAnchors();

    void setPackageDef(PackageDef *pd) { m_package=pd; }
    void setDirDef(DirDef *dd) { m_dir=dd; }

    void addUsingDirective(NamespaceDef *nd);
    void addUsingDeclaration(Definition *def);
    void combineUsingRelations();

    bool generateSourceFile() const;
    void sortMemberLists();

    void addIncludeDependency(FileDef *fd,const char *incName,bool local,bool imported,bool indirect);
    void addIncludedByDependency(FileDef *fd,const char *incName,bool local,bool imported);

    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    void addIncludedUsingDirectives();

    void addListReferences();
    //bool includes(FileDef *incFile,QDict<FileDef> *includedFiles) const;
    //bool includesByName(const QCString &name) const;
    bool visited;

  protected:
    /**
     * Retrieves the file version from version control system.
     */
    void acquireFileVersion();

  private: 
    MemberList *createMemberList(MemberListType lt);
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void writeIncludeFiles(OutputList &ol);
    void writeIncludeGraph(OutputList &ol);
    void writeIncludedByGraph(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void writeSourceLink(OutputList &ol);
    void writeNamespaceDeclarations(OutputList &ol,const QCString &title,
            bool isConstantGroup);
    void writeClassDeclarations(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);

    QDict<IncludeInfo>   *m_includeDict;
    QList<IncludeInfo>   *m_includeList;
    QDict<IncludeInfo>   *m_includedByDict;
    QList<IncludeInfo>   *m_includedByList;
    NamespaceSDict       *m_usingDirList;
    SDict<Definition>    *m_usingDeclList;
    QCString              m_path;
    QCString              m_filePath;
    QCString              m_inclDepFileName;
    QCString              m_inclByDepFileName;
    QCString              m_outputDiskName;
    QCString              m_fileName;
    QCString              m_docname;
    QIntDict<Definition> *m_srcDefDict;
    QIntDict<MemberDef>  *m_srcMemberDict;
    bool                  m_isSource;
    QCString              m_fileVersion;
    PackageDef           *m_package;
    DirDef               *m_dir;
    QList<MemberList>     m_memberLists;
    MemberGroupSDict     *m_memberGroupSDict;
    NamespaceSDict       *m_namespaceSDict;
    ClassSDict           *m_classSDict;
    bool                  m_subGrouping;
};

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
    OutputNameDict(int size) : QDict<FileList>(size) {}
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

