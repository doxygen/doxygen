/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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
#include <qdatetime.h>
#include <qcache.h>
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
#include "reflist.h"
#include "dirdef.h"

class PageSList;
class PageSDict;
class PageDef;
class SearchIndex;
class DirDef;
class ParserManager;
class ObjCache;
class Store;
class QFileInfo;
class BufStr;

typedef QList<QCString>    StringList;
typedef QDict<FileDef>     FileDict;
typedef QDict<GroupDef>    GroupDict;

class StringDict : public QDict<QCString>
{
  public: 
    StringDict(uint size=17) : QDict<QCString>(size) {}
    virtual ~StringDict() {}
};

struct LookupInfo
{
  LookupInfo() : classDef(0), typeDef(0) {}
  LookupInfo(ClassDef *cd,MemberDef *td,QCString ts,QCString rt) 
    : classDef(cd), typeDef(td), templSpec(ts),resolvedType(rt) {}
  ClassDef  *classDef;
  MemberDef *typeDef;
  QCString   templSpec;
  QCString   resolvedType;
};

extern QCString spaces;

/*! \brief This class serves as a namespace for global variables used by doxygen.
 *
 *  All fields in this class are public and static, so they can be used directly.
 */
class Doxygen
{
  public:
    static ClassSDict               *classSDict;
    static ClassSDict               *hiddenClasses;
    static PageSDict                *exampleSDict;
    static PageSDict                *pageSDict;
    static PageDef                  *mainPage;
    static bool                      insideMainPage;
    static FileNameDict             *includeNameDict;
    static FileNameDict             *exampleNameDict;
    static FileNameDict             *inputNameDict;
    static FileNameList             *inputNameList;
    static FileNameDict             *imageNameDict;
    static FileNameDict             *dotFileNameDict;
    static QStrList                  tagfileList;
    static MemberNameSDict          *memberNameSDict;
    static MemberNameSDict          *functionNameSDict;
    static QTextStream               tagFile;
    static SectionDict               sectionDict;
    static StringDict                namespaceAliasDict;
    static GroupSDict               *groupSDict;
    static NamespaceSDict           *namespaceSDict;
    static FormulaList               formulaList;
    static FormulaDict               formulaDict;
    static FormulaDict               formulaNameDict;
    static StringDict                tagDestinationDict; 
    static StringDict                aliasDict; 
    static QIntDict<MemberGroupInfo> memGrpInfoDict;
    static QDict<void>               expandAsDefinedDict;
    static NamespaceDef             *globalScope;
    static QDict<RefList>           *xrefLists; // array of xref lists: todo, test, bug, deprecated ...
    static QCString                  htmlFileExtension;
    static bool                      parseSourcesNeeded;
    static double                    sysElapsedTime;
    static QTime                     runningTime;
    static SearchIndex              *searchIndex;
    static QDict<DefinitionIntf>    *symbolMap;
    static bool                      outputToWizard;
    static QDict<int>               *htmlDirMap;
    static QCache<LookupInfo>        lookupCache;
    static DirSDict                 *directories;
    static SDict<DirRelation>        dirRelations;
    static ParserManager            *parserManager;
    static bool                      suppressDocWarnings;
    static ObjCache                 *symbolCache;
    static Store                    *symbolStorage;
    static QCString                  objDBFileName;
    static QCString                  entryDBFileName;
    static bool                      gatherDefines;
};

void initDoxygen();
void readConfiguration(int argc, char **argv);
void checkConfiguration();
void parseInput();
void generateOutput();
void readAliases();
void readFormulaRepository();
int readFileOrDirectory(const char *s,
                        FileNameList *fnList,
                        FileNameDict *fnDict,
                        StringDict *exclDict,
                        QStrList *patList,
                        QStrList *exclPatList,
                        StringList *resultList,
                        StringDict *resultDict,
                        bool recursive,
                        bool errorIfNotExist=TRUE,
                        QDict<void> *killDict = 0
                       );
int readDir(QFileInfo *fi,
            FileNameList *fnList,
            FileNameDict *fnDict,
            StringDict  *exclDict,
            QStrList *patList,
            QStrList *exclPatList,
            StringList *resultList,
            StringDict *resultDict,
            bool errorIfNotExist,
            bool recursive,
            QDict<void> *killDict
           );
void copyAndFilterFile(const char *fileName,BufStr &dest);

#define NEWMATCH

#endif
