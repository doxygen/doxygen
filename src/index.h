/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef INDEX_H
#define INDEX_H

#include <qfile.h>
#include <qtstream.h>

enum IndexSections
{
  isTitlePageStart,
  isTitlePageAuthor,
  isModuleIndex,
  isClassHierarchyIndex,
  isCompoundIndex,
  isFileIndex,
  isModuleDocumentation,
  isClassDocumentation,
  isFileDocumentation,
  isExampleDocumentation,
  isPageDocumentation,
  isEndIndex
};

class OutputList;

void writeIndex(OutputList &ol);
void writeHierarchicalIndex(OutputList &ol);
void writeClassHierarchy(OutputList &ol);
void writeFileIndex(OutputList &ol);
void writeAnnotatedIndex(OutputList &ol);
void writeAnnotatedClassList(OutputList &ol);
void writeMemberIndex(OutputList &ol);
void writeMemberList(OutputList &ol);
void writeHeaderIndex(OutputList &ol);
void writeHeaderFileList(OutputList &ol);
void writeExampleIndex(OutputList &ol);
void writePageIndex(OutputList &ol);
void writeFunctionIndex(OutputList &ol);
void writeGroupIndex(OutputList &ol);
void writeNamespaceIndex(OutputList &ol);

int countClassHierarchy();
int countMemberList();
int countFunctionList();
int countFileList();
int countGroupList();
int countNamespaceList();
int countAnnotatedClasses();

#endif
