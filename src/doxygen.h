/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include "packagedef.h"

class PageSList;
class PageSDict;
class PageInfo;

typedef QList<QCString>    StringList;
typedef QDict<FileDef>     FileDict;
typedef QDict<GroupDef>    GroupDict;

class StringDict : public QDict<QCString>
{
  public: 
    StringDict(uint size=17) : QDict<QCString>(size) {}
    virtual ~StringDict() {}
};


extern QCString spaces;

/*! \brief This class serves as a namespace for global variables used by doxygen.
 *
 *  All fields in this class are public and static, so they can be used directly.
 */
class Doxygen
{
  public:
    static ClassSDict classSDict;
    static ClassSDict hiddenClasses;
    static PageSDict *exampleSDict;
    static PageSDict *pageSDict;
    static PageInfo  *mainPage;
    static FileNameDict *includeNameDict;
    static FileNameDict *exampleNameDict;
    static FileNameDict *inputNameDict;
    static FileNameList inputNameList;
    static FileNameDict *imageNameDict;
    static FileNameDict *dotFileNameDict;
    static QStrList tagfileList;
    static MemberNameSDict memberNameSDict;
    static MemberNameSDict functionNameSDict;
    static FileList fileList;
    static FileDict fileDict;
    static ClassDef unrelatedClass; 
    static QTextStream tagFile;
    static SectionDict sectionDict;
    static StringDict namespaceAliasDict;
    //static GroupList groupList;
    static GroupSDict groupSDict;
    static NamespaceSDict namespaceSDict;
    static FormulaList formulaList;
    static FormulaDict formulaDict;
    static FormulaDict formulaNameDict;
    static StringDict tagDestinationDict; 
    static StringDict aliasDict; 
    static QIntDict<QCString> memberHeaderDict; // dictionary of the member groups heading
    static QIntDict<QCString> memberDocDict;    // dictionary of the member groups heading
    static QDict<void> expandAsDefinedDict;
    static PackageSDict packageDict;
    static NamespaceDef *globalScope;
};

void initDoxygen();
void readConfiguration(int argc, char **argv);
void parseInput();
void generateOutput();

#endif
