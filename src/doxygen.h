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

#include <mutex>

#include "containers.h"
#include "membergroup.h"
#include "dirdef.h"
#include "memberlist.h"
#include "define.h"
#include "cache.h"
#include "symbolmap.h"
#include "searchindex.h"

#define THREAD_LOCAL thread_local
#define AtomicInt    std::atomic_int

class RefList;
class PageLinkedMap;
class PageDef;
class ParserManager;
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
class Preprocessor;
struct MemberGroupInfo;
class NamespaceDefMutable;

struct LookupInfo
{
  LookupInfo() = default;
  LookupInfo(const Definition *d,const MemberDef *td,const QCString &ts,const QCString &rt)
    : definition(d), typeDef(td), templSpec(ts), resolvedType(rt) {}
  const Definition  *definition = nullptr;
  const MemberDef *typeDef = nullptr;
  QCString   templSpec;
  QCString   resolvedType;
};

struct InputFileEncoding
{
  InputFileEncoding() {}
  InputFileEncoding(const QCString &pat, const QCString &enc) : pattern(pat), encoding(enc) {}
  QCString pattern;
  QCString encoding;
};

struct NamespaceAliasInfo
{
  NamespaceAliasInfo(const std::string &a,const std::string &ctx=std::string()) : alias(a), context(ctx) {}
  std::string alias;
  std::string context;
};

using InputFileEncodingList = std::vector<InputFileEncoding>;

using ClangUsrMap = std::unordered_map<std::string,const Definition *>;

using StaticInitMap = std::unordered_map<std::string,BodyInfo>;

using NamespaceAliasInfoMap = std::unordered_map<std::string,NamespaceAliasInfo>;

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
    static FileNameLinkedMap        *includeNameLinkedMap;
    static FileNameLinkedMap        *exampleNameLinkedMap;
    static StringUnorderedSet        inputPaths;
    static FileNameLinkedMap        *inputNameLinkedMap;
    static FileNameLinkedMap        *imageNameLinkedMap;
    static FileNameLinkedMap        *dotFileNameLinkedMap;
    static FileNameLinkedMap        *mscFileNameLinkedMap;
    static FileNameLinkedMap        *diaFileNameLinkedMap;
    static FileNameLinkedMap        *plantUmlFileNameLinkedMap;
    static MemberNameLinkedMap      *memberNameLinkedMap;
    static MemberNameLinkedMap      *functionNameLinkedMap;
    static NamespaceAliasInfoMap     namespaceAliasMap;
    static GroupLinkedMap           *groupLinkedMap;
    static NamespaceLinkedMap       *namespaceLinkedMap;
    static StringMap                 tagDestinationMap;
    static StringUnorderedSet        tagFileSet;
    static MemberGroupInfoMap        memberGroupInfoMap;
    static StringUnorderedSet        expandAsDefinedSet;
    static std::unique_ptr<NamespaceDef> globalNamespaceDef;
    static NamespaceDefMutable      *globalScope;
    static QCString                  htmlFileExtension;
    static bool                      parseSourcesNeeded;
    static SearchIndexIntf           searchIndex;
    static SymbolMap<Definition>    *symbolMap;
    static ClangUsrMap              *clangUsrMap;
    static Cache<std::string,LookupInfo> *typeLookupCache;
    static Cache<std::string,LookupInfo> *symbolLookupCache;
    static DirLinkedMap             *dirLinkedMap;
    static DirRelationLinkedMap      dirRelations;
    static ParserManager            *parserManager;
    static bool                      suppressDocWarnings;
    static QCString                  filterDBFileName;
    static IndexList                *indexList;
    static QCString                  spaces;
    static bool                      generatingXmlOutput;
    static DefinesPerFileList        macroDefinitions;
    static bool                      clangAssistedParsing;
    static QCString                  verifiedDotPath;
    static InputFileEncodingList     inputFileEncodingList;
    static std::mutex                countFlowKeywordsMutex;
    static std::mutex                addExampleMutex;
    static StaticInitMap             staticInitMap;
};

void initDoxygen();
void readConfiguration(int argc, char **argv);
void checkConfiguration();
void adjustConfiguration();
void parseInput();
void generateOutput();
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
                        StringUnorderedSet *killSet = nullptr,
                        StringUnorderedSet *paths = nullptr
                       );

#endif
