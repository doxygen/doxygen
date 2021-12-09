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

#include <utility>
#include <vector>
#include <memory>

#include "qcstring.h"

class Definition;
class DefinitionMutable;
class NamespaceDef;
class MemberDef;
class OutputList;

/** \brief Abstract interface for index generators. */
class IndexIntf
{
  public:
    virtual ~IndexIntf() {}
    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual void incContentsDepth() = 0;
    virtual void decContentsDepth() = 0;
    virtual void addContentsItem(bool isDir, const QCString &name, const QCString &ref,
                                 const QCString &file, const QCString &anchor, bool separateIndex,
                                 bool addToNavIndex,const Definition *def) = 0;
    virtual void addIndexItem(const Definition *context,const MemberDef *md,
                              const QCString &sectionAnchor,const QCString &title) = 0;
    virtual void addIndexFile(const QCString &name) = 0;
    virtual void addImageFile(const QCString &name) = 0;
    virtual void addStyleSheetFile(const QCString &name) = 0;
};

/** \brief A list of index interfaces.
 *
 *  This class itself implements all methods of IndexIntf and
 *  just forwards the calls to all items in the list.
 */
class IndexList : public IndexIntf
{
  private:
    std::vector< std::unique_ptr<IndexIntf> > m_intfs;

    // For each index format we forward the method call.
    // We use C++11 variadic templates and perfect forwarding to implement foreach() generically,
    // and split the types of the methods from the arguments passed to allow implicit conversions.
    template<class... Ts,class... As>
    void foreach(void (IndexIntf::*methodPtr)(Ts...),As&&... args)
    {
      for (const auto &intf : m_intfs)
      {
        (intf.get()->*methodPtr)(std::forward<As>(args)...);
      }
    }

  public:
    /** Creates a list of indexes */
    IndexList() { m_enabled=TRUE; }

    /** Add an index generator to the list, using a syntax similar to std::make_unique<T>() */
    template<class T,class... As>
    void addIndex(As&&... args)
    { m_intfs.push_back(std::make_unique<T>(std::forward<As>(args)...)); }

    void disable()
    { m_enabled = FALSE; }
    void enable()
    { m_enabled = TRUE; }
    bool isEnabled() const
    { return m_enabled; }

    // IndexIntf implementation
    void initialize()
    { foreach(&IndexIntf::initialize); }
    void finalize()
    { foreach(&IndexIntf::finalize); }
    void incContentsDepth()
    { if (m_enabled) foreach(&IndexIntf::incContentsDepth); }
    void decContentsDepth()
    { if (m_enabled) foreach(&IndexIntf::decContentsDepth); }
    void addContentsItem(bool isDir, const QCString &name, const QCString &ref,
                         const QCString &file, const QCString &anchor,bool separateIndex=FALSE,bool addToNavIndex=FALSE,
                         const Definition *def=0)
    { if (m_enabled) foreach(&IndexIntf::addContentsItem,isDir,name,ref,file,anchor,separateIndex,addToNavIndex,def); }
    void addIndexItem(const Definition *context,const MemberDef *md,const QCString &sectionAnchor=QCString(),const QCString &title=QCString())
    { if (m_enabled) foreach(&IndexIntf::addIndexItem,context,md,sectionAnchor,title); }
    void addIndexFile(const QCString &name)
    { if (m_enabled) foreach(&IndexIntf::addIndexFile,name); }
    void addImageFile(const QCString &name)
    { if (m_enabled) foreach(&IndexIntf::addImageFile,name); }
    void addStyleSheetFile(const QCString &name)
    { if (m_enabled) foreach(&IndexIntf::addStyleSheetFile,name); }

  private:
    bool m_enabled;
};


enum IndexSections
{
  isTitlePageStart,
  isTitlePageAuthor,
  isMainPage,
  isModuleIndex,
  isDirIndex,
  isNamespaceIndex,
  isConceptIndex,
  isClassHierarchyIndex,
  isCompoundIndex,
  isFileIndex,
  isPageIndex,
  isModuleDocumentation,
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

enum HighlightedItem
{
  HLI_None=0,
  HLI_Main,
  HLI_Modules,
  //HLI_Directories,
  HLI_Namespaces,
  HLI_ClassHierarchy,
  HLI_InterfaceHierarchy,
  HLI_ExceptionHierarchy,
  HLI_Classes,
  HLI_Concepts,
  HLI_Interfaces,
  HLI_Structs,
  HLI_Exceptions,
  HLI_AnnotatedClasses,
  HLI_AnnotatedInterfaces,
  HLI_AnnotatedStructs,
  HLI_AnnotatedExceptions,
  HLI_Files,
  HLI_NamespaceMembers,
  HLI_Functions,
  HLI_Globals,
  HLI_Pages,
  HLI_Examples,
  HLI_Search,
  HLI_UserGroup,

  HLI_ClassVisible,
  HLI_ConceptVisible,
  HLI_InterfaceVisible,
  HLI_StructVisible,
  HLI_ExceptionVisible,
  HLI_NamespaceVisible,
  HLI_FileVisible
};

enum ClassMemberHighlight
{
  CMHL_All = 0,
  CMHL_Functions,
  CMHL_Variables,
  CMHL_Typedefs,
  CMHL_Enums,
  CMHL_EnumValues,
  CMHL_Properties,
  CMHL_Events,
  CMHL_Related,
  CMHL_Total = CMHL_Related+1
};

enum FileMemberHighlight
{
  FMHL_All = 0,
  FMHL_Functions,
  FMHL_Variables,
  FMHL_Typedefs,
  FMHL_Sequences,
  FMHL_Dictionaries,
  FMHL_Enums,
  FMHL_EnumValues,
  FMHL_Defines,
  FMHL_Total = FMHL_Defines+1
};

enum NamespaceMemberHighlight
{
  NMHL_All = 0,
  NMHL_Functions,
  NMHL_Variables,
  NMHL_Typedefs,
  NMHL_Sequences,
  NMHL_Dictionaries,
  NMHL_Enums,
  NMHL_EnumValues,
  NMHL_Total = NMHL_EnumValues+1
};

enum ClassHighlight
{
  CHL_All = 0,
  CHL_Classes,
  CHL_Structs,
  CHL_Unions,
  CHL_Interfaces,
  CHL_Protocols,
  CHL_Categories,
  CHL_Exceptions,
  CHL_Total = CHL_Exceptions+1
};

void writeGraphInfo(OutputList &ol);
void writeIndexHierarchy(OutputList &ol);

void countDataStructures();

extern int annotatedClasses;
extern int annotatedInterfaces;
extern int annotatedStructs;
extern int annotatedExceptions;
extern int hierarchyClasses;
extern int hierarchyInterfaces;
extern int hierarchyExceptions;
extern int documentedFiles;
extern int documentedGroups;
extern int documentedNamespaces;
extern int documentedConcepts;
extern int indexedPages;
extern int documentedClassMembers[CMHL_Total];
extern int documentedFileMembers[FMHL_Total];
extern int documentedNamespaceMembers[NMHL_Total];
extern int documentedDirs;
extern int documentedPages;

void startTitle(OutputList &ol,const QCString &fileName,const DefinitionMutable *def=0);
void endTitle(OutputList &ol,const QCString &fileName,const QCString &name);
void startFile(OutputList &ol,const QCString &name,const QCString &manName,
               const QCString &title,HighlightedItem hli=HLI_None,
               bool additionalIndices=FALSE,const QCString &altSidebarName=QCString());
void endFile(OutputList &ol,bool skipNavIndex=FALSE,bool skipEndContents=FALSE,
             const QCString &navPath=QCString());
void endFileWithNavPath(const Definition *d,OutputList &ol);

void initClassMemberIndices();
void initFileMemberIndices();
void initNamespaceMemberIndices();
void addClassMemberNameToIndex(const MemberDef *md);
void addFileMemberNameToIndex(const MemberDef *md);
void addNamespaceMemberNameToIndex(const MemberDef *md);
void sortMemberIndexLists();
QCString fixSpaces(const QCString &s);

int countVisibleMembers(const NamespaceDef *nd);

#endif
