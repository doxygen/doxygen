/******************************************************************************
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

#ifndef DOXYGEN_H
#define DOXYGEN_H

#include <qdatetime.h>
#include <qstrlist.h>
#include <qdict.h>
#include <qintdict.h>

#include "containers.h"
#include "ftextstream.h"
#include "sortdict.h"
#include "membergroup.h"
#include "dirdef.h"
#include "memberlist.h"
#include "define.h"
#include "cache.h"

#define THREAD_LOCAL thread_local
#define AtomicInt    std::atomic_int
#define AtomicBool   std::atomic_bool

class RefList;
class PageSList;
class PageSDict;
class PageDef;
class SearchIndexIntf;
class ParserManager;
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
class MemberNameLinkedMap;
class FileNameLinkedMap;
class NamespaceSDict;
class NamespaceDef;
class DefinitionIntf;
class DirSDict;
class DirRelation;
class IndexList;
class FormulaList;
class FormulaDict;
class FormulaNameDict;
class Preprocessor;
struct MemberGroupInfo;

typedef QList<QCString>    StringList;
typedef QListIterator<QCString>    StringListIterator;
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
  LookupInfo() = default;
  LookupInfo(const ClassDef *cd,const MemberDef *td,QCString ts,QCString rt)
    : classDef(cd), typeDef(td), templSpec(ts),resolvedType(rt) {}
  const ClassDef  *classDef = 0;
  const MemberDef *typeDef = 0;
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
    static FileNameLinkedMap        *includeNameLinkedMap;
    static FileNameLinkedMap        *exampleNameLinkedMap;
    static StringSet                 inputPaths;
    static FileNameLinkedMap        *inputNameLinkedMap;
    static FileNameLinkedMap        *imageNameLinkedMap;
    static FileNameLinkedMap        *dotFileNameLinkedMap;
    static FileNameLinkedMap        *mscFileNameLinkedMap;
    static FileNameLinkedMap        *diaFileNameLinkedMap;
    static MemberNameLinkedMap      *memberNameLinkedMap;
    static MemberNameLinkedMap      *functionNameLinkedMap;
    static StringUnorderedMap        namespaceAliasMap;
    static GroupSDict               *groupSDict;
    static NamespaceSDict           *namespaceSDict;
    static StringDict                tagDestinationDict;
    static StringDict                aliasDict;
    static QIntDict<MemberGroupInfo> memGrpInfoDict;
    static StringUnorderedSet        expandAsDefinedSet;
    static NamespaceDef             *globalScope;
    static QCString                  htmlFileExtension;
    static bool                      parseSourcesNeeded;
    static SearchIndexIntf          *searchIndex;
    static QDict<DefinitionIntf>    *symbolMap;
    static QDict<Definition>        *clangUsrMap;
    static bool                      outputToWizard;
    static QDict<int>               *htmlDirMap;
    static Cache<std::string,LookupInfo> *lookupCache;
    static DirSDict                 *directories;
    static SDict<DirRelation>        dirRelations;
    static ParserManager            *parserManager;
    static bool                      suppressDocWarnings;
    static QCString                  objDBFileName;
    static QCString                  entryDBFileName;
    static QCString                  filterDBFileName;
    static bool                      userComments;
    static IndexList                *indexList;
    static int                       subpageNestingLevel;
    static QCString                  spaces;
    static bool                      generatingXmlOutput;
    static GenericsSDict            *genericsDict;
    static DefinesPerFileList        macroDefinitions;
    static bool                      clangAssistedParsing;
};

void initDoxygen();
void readConfiguration(int argc, char **argv);
void checkConfiguration();
void adjustConfiguration();
void parseInput();
void generateOutput();
void readAliases();
void readFormulaRepository(QCString dir, bool cmp = FALSE);
void cleanUpDoxygen();
int readFileOrDirectory(const char *s,
                        FileNameLinkedMap *fnDict,
                        StringUnorderedSet *exclSet,
                        const StringVector *patList,
                        const StringVector *exclPatList,
                        StringVector *resultList,
                        StringUnorderedSet *resultSet,
                        bool recursive,
                        bool errorIfNotExist=TRUE,
                        StringUnorderedSet *killSet = 0,
                        StringSet *paths = 0
                       );
void copyAndFilterFile(const char *fileName,BufStr &dest);

#endif
