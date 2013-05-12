/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include <qdatetime.h>
#include <qcache.h>
#include <qstrlist.h>
#include <qdict.h>
#include <qintdict.h>

#include "ftextstream.h"
#include "sortdict.h"
#include "membergroup.h"
#include "dirdef.h"
#include "memberlist.h"

class RefList;
class PageSList;
class PageSDict;
class PageDef;
class SearchIndexIntf;
class ParserManager;
class ObjCache;
class Store;
class QFileInfo;
class BufStr;
class CiteDict;
class MemberDef;
class GroupDef;
class GroupSDict;
class FileDef;
class ClassDef;
class ClassSDict;
class GenericsSDict;
class MemberNameSDict;
class FileNameDict;
class FileNameList;
class NamespaceSDict;
class NamespaceDef;
class DefinitionIntf;
class DirSDict;
class DirRelation;
class IndexList;
class FormulaList;
class FormulaDict;
class FormulaNameDict;
class SectionDict;
struct MemberGroupInfo;

typedef QList<QCString>    StringList;
//typedef QDict<FileDef>     FileDict;
//typedef QDict<GroupDef>    GroupDict;

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

extern QCString g_spaces;

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
    static QDict<void>               inputPaths;
    static FileNameDict             *inputNameDict;
    static FileNameList             *inputNameList;
    static FileNameDict             *imageNameDict;
    static FileNameDict             *dotFileNameDict;
    static FileNameDict             *mscFileNameDict;
    static QStrList                  tagfileList;
    static MemberNameSDict          *memberNameSDict;
    static MemberNameSDict          *functionNameSDict;
    static FTextStream               tagFile;
    static SectionDict              *sectionDict;
    static StringDict                namespaceAliasDict;
    static GroupSDict               *groupSDict;
    static NamespaceSDict           *namespaceSDict;
    static FormulaList              *formulaList;
    static FormulaDict              *formulaDict;
    static FormulaDict              *formulaNameDict;
    static StringDict                tagDestinationDict; 
    static StringDict                aliasDict; 
    static QIntDict<MemberGroupInfo> memGrpInfoDict;
    static QDict<void>               expandAsDefinedDict;
    static NamespaceDef             *globalScope;
    static QDict<RefList>           *xrefLists; // array of xref lists: todo, test, bug, deprecated ...
    static QCString                  htmlFileExtension;
    static bool                      parseSourcesNeeded;
    static QTime                     runningTime;
    static SearchIndexIntf          *searchIndex;
    static QDict<DefinitionIntf>    *symbolMap;
    static QDict<Definition>        *clangUsrMap;
    static bool                      outputToWizard;
    static QDict<int>               *htmlDirMap;
    static QCache<LookupInfo>       *lookupCache;
    static DirSDict                 *directories;
    static SDict<DirRelation>        dirRelations;
    static ParserManager            *parserManager;
    static bool                      suppressDocWarnings;
    static Store                    *symbolStorage;
    static QCString                  objDBFileName;
    static QCString                  entryDBFileName;
    static CiteDict                 *citeDict;
    static bool                      gatherDefines;
    static bool                      userComments;
    static IndexList                *indexList;
    static int                       subpageNestingLevel;
    static QCString                  spaces;
    static bool                      generatingXmlOutput;
    static bool                      markdownSupport;
    static GenericsSDict            *genericsDict;
};

void initDoxygen();
void readConfiguration(int argc, char **argv);
void checkConfiguration();
void adjustConfiguration();
void searchInputFiles(StringList &inputFiles);
void parseInput();
void generateOutput();
void readAliases();
void readFormulaRepository();
void cleanUpDoxygen();
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
                        QDict<void> *killDict = 0,
                        QDict<void> *paths = 0
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

#endif
