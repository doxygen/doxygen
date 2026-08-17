/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef UTIL_H
#define UTIL_H

/*! \file
 *  \brief A bunch of utility functions.
 */

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <fstream>
#include <variant>
#include <string_view>

#include <ctype.h>
#include "types.h"
#include "docparser.h"
#include "containers.h"
#include "outputgen.h"
#include "regex.h"
#include "conceptdef.h"
#include "construct.h"
#include "htmlentity.h"

//--------------------------------------------------------------------

class ClassDef;
class FileDef;
class MemberList;
class NamespaceDef;
class FileNameLinkedMap;
class ArgumentList;
class OutputList;
class OutputDocInterface;
class MemberDef;
class GroupDef;
struct TagInfo;
class PageDef;
class SectionInfo;
class Definition;
class FileInfo;
class Dir;

/*! Returns the scope separator to use given the programming language \a lang */
DString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope=false);

//----------------------------------------------------------------

/*! reads a file with name \a name and returns it as a string. If \a filter
 *  is true the file will be filtered by any user specified input filter.
 *  If \a name is "-" the string will be read from standard input.
 */
DString fileToString(const DString &name,bool filter=false,bool isSourceCode=false);

//! read a file name \a fileName and optionally filter and transcode it
bool readInputFile(const DString &fileName,std::string &contents,
                   bool filter=true,bool isSourceCode=false);

/*! Thread-safe function to write a string representing an inline graph to a file.
 *  The contents will be used to create a hash that will be used to make the name unique.
 *  @param[in] baseName the base name of the file to write including path.
 *  @param[in] extension the file extension to use.
 *  @param[in] content the data to write to the file
 *  @param[out] exists is set to true if the file was already written before.
 *  @returns the name of the file written or an empty string in case of an error.
 */
DString writeInlineGraph(const DString &baseName,const DString &extension,const DString &content,bool &exists);

/*! Deletes all graph files written with writeInlineGraph() */
void cleanupInlineGraphs();

//----------------------------------------------------------------

/*! Helper to pass the input parameters to getDefs() */
struct GetDefInput
{
  GetDefInput(const DString &scName,const DString &memName,const DString &a) :
    scopeName(scName),memberName(memName),args(a) {}
  DString scopeName;
  DString memberName;
  DString args;
  bool forceEmptyScope = false;
  const FileDef *currentFile = nullptr;
  bool checkCV = false;
  bool insideCode = false;
};

/*! Helper to pass the result parameters from getDefs() */
struct GetDefResult
{
  bool found = false;
  const MemberDef    *md=nullptr;
  const ClassDef     *cd=nullptr;
  const FileDef      *fd=nullptr;
  const NamespaceDef *nd=nullptr;
  const GroupDef     *gd=nullptr;
  const ConceptDef   *cnd=nullptr;
  const ModuleDef    *modd=nullptr;
};

GetDefResult getDefs(const GetDefInput &input);

/*! looks for a filter for the file \a name.  Returns the name of the filter
 *  if there is a match for the file name, otherwise an empty string.
 *  In case \a inSourceCode is true then first the source filter list is
 *  considered.
 */
DString getFileFilter(const DString &name,bool isSourceCode);

/*! Returns a symbol definition (compound and/or member) given its name and context.
 *  @post return value true implies *resContext!=0 or *resMember!=0
 */
bool resolveRef(/* in */  const DString &scName,
                /* in */  const DString &name,
                /* in */  bool inSeeBlock,
                /* out */ const Definition **resContext,
                /* out */ const MemberDef  **resMember,
                /* in */  SrcLangExt lang,
                /* in */  bool lookForSpecializations = true,
                /* in */  const FileDef *currentFile = nullptr,
                /* in */  bool checkScope = false
               );

/*! Returns an symbol definition given its name and context for an explicitly linked symbol.
 *  @post return value true implies *resContext!=0
 */
bool resolveLink(/* in */  const DString &scName,
                 /* in */  const DString &lr,
                 /* in */  bool inSeeBlock,
                 /* out */ const Definition **resContext,
                 /* out */ DString &resAnchor,
                 /* in */  SrcLangExt lang,
                 /* in */  const DString &prefix=DString()
                );

//----------------------------------------------------------------

/*! Compares two parameter lists \a srcAl and \a dstAl and returns true if they match. */
bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const DString &srcReturnType,const ArgumentList *srcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const DString &dstReturnType,const ArgumentList *dstAl,
                     bool checkCV,SrcLangExt lang
                    );

/*! Merges the information of two parameter lists (typically a declaration and a definition).
 *  Missing information is added to either list.
 *  The name of parameter of srcAl is only overwritten if \a forceNameOverwrite is true.
 */
void mergeArguments(ArgumentList &srcAl,ArgumentList &dstAl,bool forceNameOverwrite=false);

/*! Returns true if the template parameter lists \a srcAl and \a dstAl match.
 *  The lists are considered to match if they have the same number of parameters and
 *  each matching parameter in \a srcAl and \a dstAl has the same constraints (or at least one parameter has no constraints).
 */
bool matchTemplateArguments(const ArgumentList &srcAl,const ArgumentList &dstAl);

//----------------------------------------------------------------

struct SelectionBlock
{
  const char *name;
  bool enabled;
};

using SelectionBlockList = std::vector<SelectionBlock>;

struct SelectionMarkerInfo
{
  char        markerChar;
  const char *beginStr;
  size_t      beginLen;
  const char *endStr;
  size_t      endLen;
  const char *closeStr;
  size_t      closeLen;
};

DString selectBlocks(const DString& s,const SelectionBlockList &blockList, const SelectionMarkerInfo &markerInfo);
void checkBlocks(const DString& s,const DString fileName, const SelectionMarkerInfo &markerInfo);

//----------------------------------------------------------------

DString findExampleFilePath(const DString &file, bool &ambig);
void writeExamples(OutputList &ol,const ExampleList &el);

DString removeRedundantWhiteSpace(const DString &s);

DString inlineArgListToDoc(const ArgumentList &al);
DString inlineTemplateArgListToDoc(const ArgumentList &al);

DString argListToString(const ArgumentList &al,bool useCanonicalType=false,bool showDefVals=true);

DString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefaults=true);

DString stripAnonymousNamespaceScope(const DString &s);

DString stripFromPath(const DString &path);

DString stripFromIncludePath(const DString &path);

bool rightScopeMatch(const DString &scope, const DString &name);

bool leftScopeMatch(const DString &scope, const DString &name);

//---------------------------------------------------------------

struct KeywordSubstitution
{
  const char *keyword;
  using GetValue          = std::function<DString()>;
  using GetValueWithParam = std::function<DString(const DString &)>;
  std::variant<GetValue,GetValueWithParam> getValueVariant;
};

using KeywordSubstitutionList = std::vector<KeywordSubstitution>;

DString substituteKeywords(const DString &file,const DString &s,const KeywordSubstitutionList &keywords);

//---------------------------------------------------------------

int getPrefixIndex(const DString &name);

DString removeAnonymousScopes(const DString &s);

DString replaceAnonymousScopes(const DString &s,const DString &replacement=DString());

DString convertNameToFile(const DString &name,bool allowDots=false,bool allowUnderscore=false);

void extractNamespaceName(const DString &scopeName,
                          DString &className,DString &namespaceName,
                          bool allowEmptyClass=false);

DString insertTemplateSpecifierInScope(const DString &scope,const DString &templ);

DString stripScope(const DString &name);

DString convertToId(const DString &s);
DString correctId(const DString &s);

DString convertToHtml(const DString &s,bool keepEntities=true);

DString convertToXML(const DString &s, bool keepEntities=false, bool citeEntry = false);

DString convertToJSString(const DString &s,bool keepEntities=false,bool singleQuotes=false);

void addMembersToMemberGroup(/* in,out */ MemberList *ml,
                             /* in,out */ MemberGroupList *pMemberGroups,
                             /* in */     const Definition *context);

int extractClassNameFromType(const DString &type,int &pos,
                              DString &name,DString &templSpec,SrcLangExt=SrcLangExt::Unknown);

DString normalizeNonTemplateArgumentsInString(
       const DString &name,
       const Definition *context,
       const ArgumentList &formalArgs);

DString substituteTemplateArgumentsInString(
       const DString &name,
       const ArgumentList &formalArgs,
       const ArgumentList *actualArgs);

DString stripTemplateSpecifiersFromScope(const DString &fullName,
                                          bool parentOnly=true,
                                          DString *lastScopeStripped=nullptr,
                                          DString scopeName=DString(),
                                          bool allowArtificial=true);

DString resolveTypeDef(const Definition *d,const DString &name,
                        const Definition **typedefContext=nullptr);

DString mergeScopes(const DString &leftScope,const DString &rightScope);

int getScopeFragment(const DString &s,int p,int *l);

void addRefItem(const RefItemVector &sli,
                const DString &key,
                const DString &prefix,
                const DString &name,
                const DString &title,
                const DString &args,
                const Definition *scope);

PageDef *addRelatedPage(const DString &name,
                        const DString &ptitle,
                        const DString &doc,
                        const DString &fileName,
                        int docLine,
                        int startLine,
                        const RefItemVector &sli = RefItemVector(),
                        GroupDef *gd=nullptr,
                        const TagInfo *tagInfo=nullptr,
                        bool xref=false,
                        SrcLangExt lang=SrcLangExt::Unknown
                       );

/** Returns true if the names of the symbols can be case sensitive. */
bool useCaseSenseNames();

DString escapeCharsInString(const DString &name,bool allowDots,bool allowUnderscore=false);
DString unescapeCharsInString(const DString &s);

void addGroupListToTitle(OutputList &ol,const Definition *d);

DString linkToText(SrcLangExt lang,const DString &link,bool ignoreDots);

bool checkExtension(const DString &fName, const DString &ext);
void addHtmlExtensionIfMissing(DString &fName);
DString stripExtensionGeneral(const DString &fName, const DString &ext);
DString stripExtension(const DString &fName);

DString makeBaseName(const DString &name, const DString &ext);

int computeQualifiedIndex(const DString &name);

void addDirPrefix(DString &fileName);

DString relativePathToRoot(const DString &name);
DString determineAbsoluteIncludeName(const DString &curFile,const DString &incFileName);

void createSubDirs(const Dir &d);
void clearSubDirs(const Dir &d);

DString removeLongPathMarker(const DString &path);
DString stripPath(const DString &s);

bool findAndRemoveWord(DString &s,const char *word);

DString stripLeadingAndTrailingEmptyLines(const DString &s,int &docLine);

//---------------------------------------------------------------
bool updateLanguageMapping(const DString &extension,const DString &parser);
SrcLangExt getLanguageFromFileName(const DString& fileName, SrcLangExt defLang=SrcLangExt::Cpp);
SrcLangExt getLanguageFromCodeLang(DString &fileName);
DString getFileNameExtension(const DString &fn);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();
//---------------------------------------------------------------

bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const DString &n);

DString parseCommentAsText(const Definition *scope,const MemberDef *member,const DString &doc,const DString &fileName,int lineNr);
DString parseCommentAsHtml(const Definition *scope,const MemberDef *member,const DString &doc,const DString &fileName,int lineNr);

bool transcodeCharacterStringToUTF8(std::string &input,const char *inputEncoding);

DString recodeString(const DString &str,const char *fromEncoding,const char *toEncoding);

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al);

void stackTrace();

DString filterTitle(const DString &title);

bool patternMatch(const FileInfo &fi,const StringVector &patList);

DString externalLinkTarget(const bool parent = false);
DString createHtmlUrl(const DString &relPath,
                       const DString &ref,
                       bool href,
                       bool islocalFile,
                       const DString &targetFileName,
                       const DString &anchor);
DString externalRef(const DString &relPath,const DString &ref,bool href);

//---------------------------------------------------------------
void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc);
DString writeMarkerList(const std::string &markerText,size_t numMarkers,
                     std::function<DString(size_t)> replaceFunc);

DString replaceColorMarkers(const DString &str);
//---------------------------------------------------------------

bool copyFile(const DString &src,const DString &dest);

int lineBlock(const DString &text,const DString &marker);

bool isURL(const DString &url);

DString correctURL(const DString &url,const DString &relPath);

DString processMarkup(const DString &s);

bool protectionLevelVisible(Protection prot);

DString stripIndentation(const DString &s,bool skipFirstLine=false);
void stripIndentationVerbatim(DString &doc,size_t indentationLevel, bool skipFirstLine=true);

DString getDotImageExtension();

bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile);

void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   MemberListType *outListType1,
                   MemberListType *outListType2
                  );

bool mainPageHasTitle();
bool openOutputFile(const DString &outFile,std::ofstream &f);

bool recognizeFixedForm(const DString &contents, FortranFormat format);
FortranFormat convertFileNameFortranParserCode(DString fn);

DString getEncoding(const FileInfo &fi);

DString detab(const DString &s,size_t &refIndent);

DString getProjectId();
DString projectLogoFile();
DString projectLogoSize();
DString showDate(const DString &fmt);

void mergeMemberOverrideOptions(MemberDefMutable *md1,MemberDefMutable *md2);

size_t updateColumnCount(const char *s,size_t col);

DString mangleCSharpGenericName(const DString &name);
DString demangleCSharpGenericName(const DString &name,const DString &templArgs);

DString extractBeginRawStringDelimiter(const char *rawStart);
DString extractEndRawStringDelimiter(const char *rawEnd);

#endif
