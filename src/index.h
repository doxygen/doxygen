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

#include "qcstring.h"

class Definition;
class OutputList;
class DefinitionMutable;
class NamespaceDef;
class MemberDef;

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
