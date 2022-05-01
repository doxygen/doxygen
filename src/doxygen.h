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

#include "containers.h"
#include "membergroup.h"
#include "dirdef.h"
#include "memberlist.h"
#include "define.h"
#include "cache.h"
#include "symbolmap.h"

#define THREAD_LOCAL thread_local
#define AtomicInt    std::atomic_int
#define AtomicBool   std::atomic_bool

class RefList;
class PageLinkedMap;
class PageDef;
class SearchIndexIntf;
class ParserManager;
class BufStr;
class CiteDict;
class MemberDef;
class GroupDef;
class GroupLinkedMap;
class FileDef;
class ClassDef;
class ClassLinkedMap;
class ConceptLinkedMap;
class MemberNameLinkedMap;
class FileNameLinkedMap;
class NamespaceLinkedMap;
class NamespaceDef;
class DirRelationLinkedMap;
class IndexList;
class FormulaList;
class FormulaDict;
class FormulaNameDict;
class Preprocessor;
struct MemberGroupInfo;
class NamespaceDefMutable;

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

using ClangUsrMap = std::unordered_map<std::string,const Definition *>;

/*! \brief This class serves as a namespace for global variables used by doxygen.
 *
 *  All fields in this class are public and static, so they can be used directly.
 */
class Doxygen
{
  public:
    static ClassLinkedMap           *classLinkedMap;
    static ClassLinkedMap           *hiddenClassLinkedMap;
    static ConceptLinkedMap         *conceptLinkedMap;
    static PageLinkedMap            *exampleLinkedMap;
    static PageLinkedMap            *pageLinkedMap;
    static std::unique_ptr<PageDef>  mainPage;
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
    static GroupLinkedMap           *groupLinkedMap;
    static NamespaceLinkedMap       *namespaceLinkedMap;
    static StringMap                 tagDestinationMap;
    static StringMap                 aliasMap;
    static MemberGroupInfoMap        memberGroupInfoMap;
    static StringUnorderedSet        expandAsDefinedSet;
    static NamespaceDefMutable      *globalScope;
    static QCString                  htmlFileExtension;
    static bool                      parseSourcesNeeded;
    static SearchIndexIntf          *searchIndex;
    static SymbolMap<Definition>    *symbolMap;
    static ClangUsrMap              *clangUsrMap;
    static Cache<std::string,LookupInfo> *lookupCache;
    static DirLinkedMap             *dirLinkedMap;
    static DirRelationLinkedMap      dirRelations;
    static ParserManager            *parserManager;
    static bool                      suppressDocWarnings;
    static QCString                  filterDBFileName;
    static IndexList                *indexList;
    static int                       subpageNestingLevel;
    static QCString                  spaces;
    static bool                      generatingXmlOutput;
    static DefinesPerFileList        macroDefinitions;
    static bool                      clangAssistedParsing;
    static QCString                  verifiedDotPath;
    static volatile bool             terminating;
};

/** Deleter that only deletes an object if doxygen is not already terminating */
template<class T>
struct NonTerminatingDeleter
{
  void operator()(T *obj)
  {
    if (!Doxygen::terminating) delete obj;
  }
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
void readFileOrDirectory(const QCString &s,
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
