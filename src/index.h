/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qfile.h>

enum IndexSections
{
  isTitlePageStart,
  isTitlePageAuthor,
  isMainPage,
//  isPackageIndex,
  isModuleIndex,
  isNamespaceIndex,
  isClassHierarchyIndex,
  isCompoundIndex,
  isFileIndex,
  isPageIndex,
//  isPackageDocumentation,
  isModuleDocumentation,
  isNamespaceDocumentation,
  isClassDocumentation,
  isFileDocumentation,
  isExampleDocumentation,
  isPageDocumentation,
  isEndIndex
};

class OutputList;

void writeIndex(OutputList &ol);
void writeHierarchicalIndex(OutputList &ol);
void writeAlphabeticalIndex(OutputList &ol);
void writeClassHierarchy(OutputList &ol);
void writeFileIndex(OutputList &ol);
void writeSourceIndex(OutputList &ol);
void writeAnnotatedIndex(OutputList &ol);
void writeAnnotatedClassList(OutputList &ol);
void writeMemberIndex(OutputList &ol);
void writeMemberList(OutputList &ol,bool useSections);
void writeHeaderIndex(OutputList &ol);
void writeHeaderFileList(OutputList &ol);
void writeExampleIndex(OutputList &ol);
void writePageIndex(OutputList &ol);
void writeFileMemberIndex(OutputList &ol);
void writeGroupIndex(OutputList &ol);
void writeNamespaceIndex(OutputList &ol);
void writeNamespaceMemberIndex(OutputList &ol);
void writeGraphicalClassHierarchy(OutputList &ol);
void writeGraphInfo(OutputList &ol);
void writePackageIndex(OutputList &ol);

void countDataStructures();

#if 0
extern int annotatedClasses;
extern int hierarchyClasses;
extern int documentedFunctions;
extern int documentedMembers;
extern int documentedDefines;
extern int documentedFiles;
extern int documentedGroups;
extern int documentedNamespaces;
extern int documentedNamespaceMembers;
extern int documentedIncludeFiles;
extern int indexedPages;
#endif
extern int documentedHtmlFiles;
extern int documentedPages;

enum HighlightedItem
{
  HLI_None=0,
  HLI_Main,
  HLI_Modules,
  HLI_Namespaces,
  HLI_Hierarchy,
  HLI_Classes,
  HLI_Annotated,
  HLI_Files,
  HLI_NamespaceMembers,
  HLI_Functions,
  HLI_Globals,
  HLI_Pages,
  HLI_Examples,
  HLI_Search
};

void startTitle(OutputList &ol,const char *fileName);
void endTitle(OutputList &ol,const char *fileName,const char *name);
void startFile(OutputList &ol,const char *name,const char *manName,
               const char *title,bool external=FALSE,HighlightedItem hli=HLI_None);
void endFile(OutputList &ol,bool external=FALSE);

#endif
