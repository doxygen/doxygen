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

#include <memory>
#include <unordered_set>

#include "definition.h"
#include "memberlist.h"
#include "containers.h"

class MemberList;
class FileDef;
class FileList;
class ClassDef;
class ConceptDef;
class MemberDef;
class OutputList;
class NamespaceDef;
class NamespaceLinkedRefMap;
class ConceptLinkedRefMap;
class ClassLinkedRefMap;
class DirDef;
class ClangTUParser;

// --- Set of files

using FileDefSet = std::unordered_set<const FileDef*>;

enum class IncludeKind : uint32_t
{
  // bits
  IncludeSystem     = 0x0001,
  IncludeLocal      = 0x0002,
  ImportSystemObjC  = 0x0004,
  ImportLocalObjC   = 0x0008,
  ImportSystem      = 0x0010, // C++20 header import
  ImportLocal       = 0x0020, // C++20 header import
  ImportModule      = 0x0040  // C++20/Java module import
};

inline constexpr uint32_t operator|(IncludeKind a, IncludeKind b) { return static_cast<uint32_t>(a) | static_cast<uint32_t>(b); }
inline constexpr uint32_t operator|(uint32_t a, IncludeKind b) { return a | static_cast<uint32_t>(b); }
inline constexpr uint32_t operator&(IncludeKind a, uint32_t mask) { return static_cast<uint32_t>(a) & mask; }

// masks
constexpr uint32_t IncludeKind_LocalMask  = IncludeKind::IncludeLocal     | IncludeKind::ImportLocalObjC  | IncludeKind::ImportLocal;
constexpr uint32_t IncludeKind_SystemMask = IncludeKind::IncludeSystem    | IncludeKind::ImportSystemObjC | IncludeKind::ImportSystem;
constexpr uint32_t IncludeKind_ImportMask = IncludeKind::ImportSystemObjC | IncludeKind::ImportLocalObjC  | IncludeKind::ImportSystem |
                                            IncludeKind::ImportLocal      | IncludeKind::ImportModule;
constexpr uint32_t IncludeKind_ObjCMask   = IncludeKind::ImportSystemObjC | IncludeKind::ImportLocalObjC;

QCString includeStatement(SrcLangExt lang,IncludeKind kind);
QCString includeOpen(SrcLangExt lang,IncludeKind kind);
QCString includeClose(SrcLangExt lang,IncludeKind kind);

/** Class representing the data associated with a \#include statement. */
struct IncludeInfo
{
  IncludeInfo() {}
  IncludeInfo(const FileDef *fd,const QCString &in,IncludeKind k)
    : fileDef(fd), includeName(in), kind(k) {}
  const FileDef *fileDef = nullptr;
  QCString includeName;
  IncludeKind kind = IncludeKind::IncludeSystem;
};

class IncludeInfoList : public std::vector<IncludeInfo>
{
};

bool compareFileDefs(const FileDef *fd1, const FileDef *fd2);

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
    ABSTRACT_BASE_CLASS(FileDef)
    // ----------------------------------------------------------------------

    /*! Returns the unique file name (this may include part of the path). */
    virtual QCString fileName() const = 0;

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

    virtual const Definition *getSourceDefinition(int lineNr) const = 0;
    virtual const MemberDef *getSourceMember(int lineNr) const = 0;

    /*! Returns the absolute path of this file. */
    virtual QCString getPath() const = 0;

    /*! Returns version of this file. */
    virtual QCString getVersion() const = 0;

    virtual bool isIncluded(const QCString &name) const = 0;

    virtual DirDef *getDirDef() const = 0;
    virtual ModuleDef *getModuleDef() const = 0;
    virtual const LinkedRefMap<NamespaceDef> &getUsedNamespaces() const = 0;
    virtual const LinkedRefMap<const Definition> &getUsedDefinitions() const = 0;
    virtual const IncludeInfoList &includeFileList() const = 0;
    virtual const IncludeInfoList &includedByFileList() const = 0;
    virtual void getAllIncludeFilesRecursively(StringVector &incFiles) const = 0;

    virtual MemberList *getMemberList(MemberListType lt) const = 0;
    virtual const MemberLists &getMemberLists() const = 0;

    /* user defined member groups */
    virtual const MemberGroupList &getMemberGroups() const = 0;
    virtual const NamespaceLinkedRefMap &getNamespaces() const = 0;
    virtual const ConceptLinkedRefMap &getConcepts() const = 0;
    virtual const ClassLinkedRefMap &getClasses() const = 0;

    virtual QCString title() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual QCString fileVersion() const = 0;

    virtual bool subGrouping() const = 0;

    virtual void countMembers() = 0;
    virtual int numDocMembers() const = 0;
    virtual int numDecMembers() const = 0;

    //---------------------------------

    virtual void addSourceRef(int line,const Definition *d,const MemberDef *md) = 0;

    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void writeMemberPages(OutputList &ol) = 0;
    virtual void writeTagFile(TextStream &t) = 0;

    virtual void writeSourceHeader(OutputList &ol) = 0;
    virtual void writeSourceBody(OutputList &ol,ClangTUParser *clangParser) = 0;
    virtual void writeSourceFooter(OutputList &ol) = 0;
    virtual void parseSource(ClangTUParser *clangParser) = 0;
    virtual void setDiskName(const QCString &name) = 0;

    virtual void insertMember(MemberDef *md) = 0;
    virtual void insertClass(ClassDef *cd) = 0;
    virtual void insertConcept(ConceptDef *cd) = 0;
    virtual void insertNamespace(NamespaceDef *nd) = 0;
    virtual void removeMember(MemberDef *md) = 0;
    virtual void computeAnchors() = 0;

    virtual void setDirDef(DirDef *dd) = 0;
    virtual void setModuleDef(ModuleDef *mod) = 0;

    virtual void addUsingDirective(NamespaceDef *nd) = 0;
    virtual void addUsingDeclaration(const Definition *d) = 0;
    virtual void combineUsingRelations() = 0;

    virtual bool generateSourceFile() const = 0;
    virtual void sortMemberLists() = 0;

    virtual void addIncludeDependency(const FileDef *fd,const QCString &incName,IncludeKind kind) = 0;
    virtual void addIncludedByDependency(const FileDef *fd,const QCString &incName,IncludeKind kind) = 0;

    virtual void addMembersToMemberGroup() = 0;
    virtual void distributeMemberGroupDocumentation() = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void addIncludedUsingDirectives(FileDefSet &visitedFiles) = 0;

    virtual void addListReferences() = 0;
    virtual void addRequirementReferences() = 0;

    // include graph related members
    virtual bool hasIncludeGraph() const = 0;
    virtual bool hasIncludedByGraph() const = 0;

    virtual void overrideIncludeGraph(bool e) = 0;
    virtual void overrideIncludedByGraph(bool e) = 0;
};

std::unique_ptr<FileDef> createFileDef(const QCString &p,const QCString &n,const QCString &ref=QCString(),const QCString &dn=QCString());


// --- Cast functions

FileDef            *toFileDef(Definition *d);
const FileDef      *toFileDef(const Definition *d);

// ------------------

class FileList : public std::vector<const FileDef *>
{
};

struct FilesInDir
{
  FilesInDir(const QCString &p) : path(p) {}
  QCString path;
  std::vector<const FileDef *> files;
};

#endif

