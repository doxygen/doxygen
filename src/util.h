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

#include <cctype>
#include <functional>
#include <variant>

#include "construct.h"
#include "containers.h"
#include "types.h"

//--------------------------------------------------------------------

class ArgumentList;
class ClassDef;
class ConceptDef;
class Definition;
class Dir;
class ExampleList;
class FileDef;
class FileInfo;
class GroupDef;
class MemberDef;
class ModuleDef;
class NamespaceDef;
class OutputList;

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

DString resolveTypeDef(const Definition *d,const DString &name,
                       const Definition **typedefContext=nullptr);

/*! Resolve a reference via a tagfile reference \a ref.
 *  If after resolving the reference the result points to a relative path
 *  then \a relPath is prepended to create the correct link.
 */
DString externalRef(const DString &relPath,const DString &ref);

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

/*! Returns a representation of \a name where all known types have been normalized to their canonical form.
 *  The normalization is done in the context of \a context and using the formal arguments in \a formalArgs.
 */
DString normalizeNonTemplateArgumentsInString(
       const DString &name,
       const Definition *context,
       const ArgumentList &formalArgs);

/*! Substitutes any occurrence of a formal argument from argument list
 *  \a formalArgs in \a name by the corresponding actual argument in
 *  argument list \a actualArgs. The result after substitution
 *  is returned as a string. The argument \a name is used to
 *  prevent recursive substitution.
 */
DString substituteTemplateArgumentsInString(
       const DString &name,
       const ArgumentList &formalArgs,
       const ArgumentList *actualArgs);

/*! Returns a documentation string combining the inline documentation for each parameter in the list \a al. */
DString inlineArgListToDoc(const ArgumentList &al);

/*! Returns a documentation string combining the inline documentation for each template parameter in the list \a al. */
DString inlineTemplateArgListToDoc(const ArgumentList &al);

/*! Returns a string representation of the parameter list \a al.
 *  If \a useCanonicalType is true then the canonical type is used for each argument.
 *  If \a showDefVals is true then default values are included in the string representation.
 */
DString argListToString(const ArgumentList &al,bool useCanonicalType=false,bool showDefVals=true);

/*! Returns a string representation of the template parameter list \a al.
 *  The \a lang parameter is used to determine the correct syntax for the template parameters.
 *  If \a includeDefaults is true then default values are included in the string representation.
 */
DString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefaults=true);

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

//----------------------------------------------------------------

DString stripScope(const DString &name);
DString stripAnonymousNamespaceScope(const DString &s);
DString stripFromPath(const DString &path);
DString stripFromIncludePath(const DString &path);
DString stripPath(const DString &s);
DString stripIndentation(const DString &s,bool skipFirstLine=false);
DString stripExtensionGeneral(const DString &fName, const DString &ext);
DString stripExtension(const DString &fName);
DString stripLeadingAndTrailingEmptyLines(const DString &s,int &docLine);
void stripIndentationVerbatim(DString &doc,size_t indentationLevel, bool skipFirstLine=true);

DString removeRedundantWhiteSpace(const DString &s);
DString detab(const DString &s,size_t &refIndent);

//---------------------------------------------------------------

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

void addDirPrefix(DString &fileName);
int getPrefixIndex(const DString &name);
int computeQualifiedIndex(const DString &name);

//---------------------------------------------------------------

DString convertNameToFile(const DString &name,bool allowDots=false,bool allowUnderscore=false);
DString escapeCharsInString(const DString &name,bool allowDots,bool allowUnderscore=false);
DString unescapeCharsInString(const DString &s);
DString linkToText(SrcLangExt lang,const DString &link,bool ignoreDots);

//---------------------------------------------------------------

DString removeAnonymousScopes(const DString &s);

DString replaceAnonymousScopes(const DString &s,const DString &replacement=DString());

DString insertTemplateSpecifierInScope(const DString &scope,const DString &templ);

DString stripTemplateSpecifiersFromScope(const DString &fullName,
                                          bool parentOnly=true,
                                          DString *lastScopeStripped=nullptr,
                                          DString scopeName=DString(),
                                          bool allowArtificial=true);

DString mergeScopes(const DString &leftScope,const DString &rightScope);

int getScopeFragment(const DString &s,int p,int *l);

//---------------------------------------------------------------

DString convertToId(const DString &s);
DString convertToHtml(const DString &s,bool keepEntities=true);
DString convertToXML(const DString &s, bool keepEntities=false, bool citeEntry = false);
DString convertToJSString(const DString &s,bool keepEntities=false,bool singleQuotes=false);

//---------------------------------------------------------------

bool updateLanguageMapping(const DString &extension,const DString &parser);
SrcLangExt getLanguageFromFileName(const DString& fileName, SrcLangExt defLang=SrcLangExt::Cpp);
SrcLangExt getLanguageFromCodeLang(DString &fileName);
DString getFileNameExtension(const DString &fn);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();

//---------------------------------------------------------------

void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc);
DString writeMarkerList(const std::string &markerText,size_t numMarkers,
                     std::function<DString(size_t)> replaceFunc);

DString replaceColorMarkers(const DString &str);

//---------------------------------------------------------------

void createSubDirs(const Dir &d);
void clearSubDirs(const Dir &d);
DString relativePathToRoot(const DString &name);

/*! Copies the file \a src to \a dest. Returns true if the copy was successful, false otherwise.
 *  When a relative path is used, is is based on the current working directory.
 */
bool copyFile(const DString &src,const DString &dest);

/*! Helper to open an output search \a f for writing. If the file already exists it will be renamed to .bak first.
 *  If \a outFile is "-" then the output will be written to standard output.
 */
bool openOutputFile(const DString &outFile,std::ofstream &f);

//---------------------------------------------------------------

bool isURL(const DString &url);
DString correctURL(const DString &url,const DString &relPath);
DString createHtmlUrl(const DString &relPath,
                       const DString &ref,
                       bool islocalFile,
                       const DString &targetFileName,
                       const DString &anchor);

//---------------------------------------------------------------

DString mangleCSharpGenericName(const DString &name);
DString demangleCSharpGenericName(const DString &name,const DString &templArgs);

//---------------------------------------------------------------
DString extractBeginRawStringDelimiter(const char *rawStart);
DString extractEndRawStringDelimiter(const char *rawEnd);

//---------------------------------------------------------------

void extractNamespaceName(const DString &scopeName,
                          DString &className,DString &namespaceName,
                          bool allowEmptyClass=false);

int extractClassNameFromType(const DString &type,int &pos,
                              DString &name,DString &templSpec,SrcLangExt=SrcLangExt::Unknown);


/** Returns true if the names of the symbols can be case sensitive. */
bool useCaseSenseNames();

bool checkExtension(const DString &fName, const DString &ext);

void addHtmlExtensionIfMissing(DString &fName);

bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const DString &n);

void addGroupListToTitle(OutputList &ol,const Definition *d);

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al);

void stackTrace();

bool protectionLevelVisible(Protection prot);

void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   MemberListType *outListType1,
                   MemberListType *outListType2
                  );

DString makeBaseName(const DString &name, const DString &ext);

DString determineAbsoluteIncludeName(const DString &curFile,const DString &incFileName);

DString filterTitle(const DString &title);

/*! Returns the file extension to use for dot files as specified via the DOT_IMAGE_FORMAT configuration option. */
DString getDotImageExtension();

DString externalLinkTarget(const bool parent = false);

DString getEncoding(const FileInfo &fi);

bool mainPageHasTitle();
DString getProjectId();
DString projectLogoFile();
DString projectLogoSize();
DString showDate(const DString &fmt);

#endif
