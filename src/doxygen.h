/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef DOXYGEN_H
#define DOXYGEN_H

#include "qtbc.h"
#include <qtextstream.h>
#include "groupdef.h"
#include "filedef.h"
#include "classdef.h"
#include "memberdef.h"
#include "classlist.h"
#include "membername.h"
#include "filename.h"
#include "namespacedef.h"
#include "formula.h"
#include "section.h"
#include "membergroup.h"

class BufStr : public QCString
{
  public:
    BufStr(int size) : QCString(size), offset(0), spareRoom(10240) {}
    void addChar(char c)
    {
      if (offset>=size()) resize(size()+spareRoom);
      data()[offset++]=c;
    }
    void addArray(const char *a,int len)
    {
      if (offset+len>=size()) resize(size()+len+spareRoom);
      memcpy(data()+offset,a,len);
      offset+=len;
    }
    uint curPos() { return offset; }
    void skip(uint s)
    {
      if (offset+s>=size()) resize(size()+s+spareRoom);
      offset+=s;
    }
  private:
    uint offset;
    const int spareRoom; // 10Kb extra room to avoid frequent resizing
};

class PageSList;
class PageSDict;
class PageInfo;

extern PageSDict *exampleSDict;
extern PageSDict *pageSDict;
extern PageInfo *mainPage;
extern FileNameDict *includeNameDict;
extern FileNameDict *exampleNameDict;
extern FileNameDict *inputNameDict;
extern FileNameDict *imageNameDict;

typedef QList<QCString>    StringList;
typedef QDict<FileDef>     FileDict;
typedef QDict<QCString>    StringDict;
typedef QDict<GroupDef>    GroupDict;

extern ClassList classList;
extern ClassDict classDict;
extern QStrList tagfileList;
extern MemberNameList memberNameList;
extern MemberNameList functionNameList;
extern MemberNameDict memberNameDict;
extern MemberNameDict functionNameDict;
//extern StringDict substituteDict;
extern FileList fileList;
extern FileDict fileDict;
extern ClassDef unrelatedClass; 
extern QTextStream tagFile;
extern SectionDict sectionDict;
extern FileNameList inputNameList;
extern StringDict typedefDict;
extern StringDict namespaceAliasDict;
extern GroupList groupList;
extern GroupDict groupDict;
extern NamespaceList namespaceList;
extern NamespaceDict namespaceDict;
extern FormulaList formulaList;
extern FormulaDict formulaDict;
extern FormulaDict formulaNameDict;
extern StringDict tagDestinationDict; 
extern StringDict aliasDict; 
extern QIntDict<QCString> memberHeaderDict; // dictionary of the member groups heading
extern QIntDict<QCString> memberDocDict;    // dictionary of the member groups heading
extern QDict<void> expandAsDefinedDict;

extern int annotatedClasses;
extern int hierarchyClasses;
extern int documentedFunctions;
extern int documentedMembers;
extern int documentedDefines;
extern int documentedFiles;
extern int documentedHtmlFiles;
extern int documentedGroups;
extern int documentedNamespaces;
extern int documentedNamespaceMembers;
extern int documentedIncludeFiles;
extern QCString spaces;

extern const char * getOverloadDocs();

#endif
