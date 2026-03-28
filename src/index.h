/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef INDEX_H
#define INDEX_H

#include <memory>
#include <vector>
#include <map>
#include "qcstring.h"
#include "construct.h"

class Definition;
class OutputList;
class DefinitionMutable;
class NamespaceDef;
class MemberDef;

enum class IndexSection
{
  isTitlePageStart,
  isTitlePageAuthor,
  isMainPage,
  isModuleIndex,
  isTopicIndex,
  isDirIndex,
  isNamespaceIndex,
  isConceptIndex,
  isClassHierarchyIndex,
  isCompoundIndex,
  isFileIndex,
  isPageIndex,
  isModuleDocumentation,
  isTopicDocumentation,
  isDirDocumentation,
  isNamespaceDocumentation,
  isClassDocumentation,
  isConceptDocumentation,
  isFileDocumentation,
  isExampleDocumentation,
  isPageDocumentation,
  isPageDocumentation2,
  isEndIndex
};

enum class HighlightedItem
{
  None=0,
  Main,
  Modules,
  Namespaces,
  Topics,
  ClassHierarchy,
  InterfaceHierarchy,
  ExceptionHierarchy,
  Classes,
  Concepts,
  Interfaces,
  Structs,
  Exceptions,
  AnnotatedClasses,
  AnnotatedInterfaces,
  AnnotatedStructs,
  AnnotatedExceptions,
  Files,
  NamespaceMembers,
  ModuleMembers,
  Functions,
  Globals,
  Pages,
  Examples,
  Search,
  UserGroup,

  ClassVisible,
  ConceptVisible,
  InterfaceVisible,
  StructVisible,
  ExceptionVisible,
  NamespaceVisible,
  FileVisible,
  ModuleVisible
};

// Note: we can't use enum class for the enums below as they are also used as an array index,
// so we wrap them in a namespace instead

namespace ClassMemberHighlight
{
  enum Enum : int
  {
    All = 0,
    Functions,
    Variables,
    Typedefs,
    Enums,
    EnumValues,
    Properties,
    Events,
    Related,
    Total
  };
} // namespace ClassMemberHighlight

namespace FileMemberHighlight
{
  enum Enum : int
  {
    All = 0,
    Functions,
    Variables,
    Typedefs,
    Sequences,
    Dictionaries,
    Enums,
    EnumValues,
    Defines,
    Total
  };
} // namespace FileMemberHighlight

namespace NamespaceMemberHighlight
{
  enum Enum : int
  {
    All = 0,
    Functions,
    Variables,
    Typedefs,
    Sequences,
    Dictionaries,
    Enums,
    EnumValues,
    Total
  };
} // namespace NamespaceMemberHighlight

namespace ModuleMemberHighlight
{
  enum Enum : int
  {
    All = 0,
    Functions,
    Variables,
    Typedefs,
    Enums,
    EnumValues,
    Total
  };
} // namespace ModuleMemberHighlight

class Index
{
  public:
    using MemberIndexList = std::vector<const MemberDef *>;
    using MemberIndexMap = std::map<std::string,MemberIndexList>;

    static Index &instance();

    void countDataStructures();
    void addClassMemberNameToIndex(const MemberDef *md);
    void addFileMemberNameToIndex(const MemberDef *md);
    void addNamespaceMemberNameToIndex(const MemberDef *md);
    void addModuleMemberNameToIndex(const MemberDef *md);
    void sortMemberIndexLists();

    // ---- getters
    int numAnnotatedClasses() const;
    int numAnnotatedClassesPrinted() const;
    int numHierarchyClasses() const;
    int numAnnotatedInterfaces() const;
    int numAnnotatedInterfacesPrinted() const;
    int numHierarchyInterfaces() const;
    int numAnnotatedStructs() const;
    int numAnnotatedStructsPrinted() const;
    int numAnnotatedExceptions() const;
    int numAnnotatedExceptionsPrinted() const;
    int numHierarchyExceptions() const;
    int numDocumentedGroups() const;
    int numDocumentedNamespaces() const;
    int numDocumentedConcepts() const;
    int numDocumentedModules() const;
    int numIndexedPages() const;
    int numDocumentedFiles() const;
    int numDocumentedPages() const;
    int numDocumentedDirs() const;
    int numDocumentedClassMembers(ClassMemberHighlight::Enum e) const;
    int numDocumentedFileMembers(FileMemberHighlight::Enum e) const;
    int numDocumentedNamespaceMembers(NamespaceMemberHighlight::Enum e) const;
    int numDocumentedModuleMembers(ModuleMemberHighlight::Enum e) const;
    MemberIndexMap isClassIndexLetterUsed(ClassMemberHighlight::Enum e) const;
    MemberIndexMap isFileIndexLetterUsed(FileMemberHighlight::Enum e) const;
    MemberIndexMap isNamespaceIndexLetterUsed(NamespaceMemberHighlight::Enum e) const;
    MemberIndexMap isModuleIndexLetterUsed(ModuleMemberHighlight::Enum e) const;

  private:
    void resetDocumentedClassMembers(int i);
    void resetDocumentedFileMembers(int i);
    void resetDocumentedNamespaceMembers(int i);
    void resetDocumentedModuleMembers(int i);
    void incrementDocumentedClassMembers(int i,const std::string &letter,const MemberDef *md);
    void incrementDocumentedFileMembers(int i,const std::string &letter,const MemberDef *md);
    void incrementDocumentedNamespaceMembers(int i,const std::string &letter,const MemberDef *md);
    void incrementDocumentedModuleMembers(int i,const std::string &letter,const MemberDef *md);
    Index();
    ~Index();
    NON_COPYABLE(Index)
    struct Private;
    std::unique_ptr<Private> p;
};

void writeGraphInfo(OutputList &ol);
void writeIndexHierarchy(OutputList &ol);
void startTitle(OutputList &ol,const QCString &fileName,const DefinitionMutable *def=nullptr);
void endTitle(OutputList &ol,const QCString &fileName,const QCString &name);
void startFile(OutputList &ol,const QCString &name,bool isSource,const QCString &manName,
               const QCString &title,HighlightedItem hli=HighlightedItem::None,
               bool additionalIndices=false,const QCString &altSidebarName=QCString(), int hierarchyLevel=0,
               const QCString &allMembersFile=QCString());
void endFile(OutputList &ol,bool skipNavIndex=FALSE,bool skipEndContents=FALSE,
             const QCString &navPath=QCString());
void endFileWithNavPath(OutputList &ol,const DefinitionMutable *d,bool showPageNavigation=true);

#endif
