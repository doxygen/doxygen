/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

//--------------------------------------------------------------------

/** Abstract interface for a hyperlinked text fragment. */
class TextGeneratorIntf
{
  public:
    ABSTRACT_BASE_CLASS(TextGeneratorIntf)

    virtual void writeString(std::string_view,bool) const = 0;
    virtual void writeBreak(int indent) const = 0;
    virtual void writeLink(const DString &extRef,const DString &file,
                      const DString &anchor,std::string_view text
                     ) const = 0;
};

/** Implements TextGeneratorIntf for an OutputDocInterface stream. */
class TextGeneratorOLImpl final : public TextGeneratorIntf
{
  public:
    TextGeneratorOLImpl(OutputList &ol);
    void writeString(std::string_view s,bool keepSpaces) const override;
    void writeBreak(int indent) const override;
    void writeLink(const DString &extRef,const DString &file,
                   const DString &anchor,std::string_view text
                  ) const override;
  private:
    OutputList &m_ol;
};

//--------------------------------------------------------------------

DString langToString(SrcLangExt lang);
DString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope=false);

//--------------------------------------------------------------------

struct LinkifyTextOptions
{
  public:
    // === getters for optional params
    const Definition *scope()          const { return m_scope; }
    const FileDef *fileScope()         const { return m_fileScope; }
    const Definition *self()           const { return m_self; }
    const ArgumentList *argumentList() const { return m_al; }
    bool  autoBreak()                  const { return m_autoBreak; }
    bool  external()                   const { return m_external; }
    bool  keepSpaces()                 const { return m_keepSpaces; }
    int   indentLevel()                const { return m_indentLevel; }
    size_t breakThreshold()            const { return m_breakThreshold; }

    // === setters for optional params
    LinkifyTextOptions & setScope(const Definition *scope)
    { m_scope = scope; return *this; }

    LinkifyTextOptions & setFileScope(const FileDef *fileScope)
    { m_fileScope = fileScope; return *this; }

    LinkifyTextOptions & setSelf(const Definition *self)
    { m_self = self;  return *this;}

    LinkifyTextOptions & setArgumentList(const ArgumentList *al)
    { m_al = al; return *this; }

    LinkifyTextOptions & setAutoBreak(bool autoBreak)
    { m_autoBreak = autoBreak; return *this; }

    LinkifyTextOptions & setExternal(bool external)
    { m_external = external; return *this; }

    LinkifyTextOptions & setKeepSpaces(bool keepSpaces)
    { m_keepSpaces = keepSpaces; return *this; }

    LinkifyTextOptions & setIndentLevel(int indentLevel)
    { m_indentLevel = indentLevel; return *this; }

    LinkifyTextOptions & setBreakThreshold(size_t breakThreshold)
    { m_breakThreshold = breakThreshold; return *this; }

  private:
    // optional params with defaults
    const Definition *  m_scope          = nullptr;
    const FileDef *     m_fileScope      = nullptr;
    const Definition *  m_self           = nullptr;
    const ArgumentList *m_al             = nullptr;
    bool                m_autoBreak      = false;
    bool                m_external       = true;
    bool                m_keepSpaces     = false;
    int                 m_indentLevel    = 0;
    size_t              m_breakThreshold = 30;
};

void linkifyText(const TextGeneratorIntf &ol,
                 const DString &text,
                 const LinkifyTextOptions &options
                );

DString fileToString(const DString &name,bool filter=false,bool isSourceCode=false);

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

DString getFileFilter(const DString &name,bool isSourceCode);

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

bool resolveLink(/* in */  const DString &scName,
                 /* in */  const DString &lr,
                 /* in */  bool inSeeBlock,
                 /* out */ const Definition **resContext,
                 /* out */ DString &resAnchor,
                 /* in */  SrcLangExt lang,
                 /* in */  const DString &prefix=DString()
                );

void generateFileRef(OutputList &ol,const DString &,
                             const DString &linkTxt=DString());

void writePageRef(OutputList &ol,const DString &cn,const DString &mn);

//DString getCanonicalTemplateSpec(const Definition *d,const FileDef *fs,const DString& spec);

bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const DString &srcReturnType,const ArgumentList *srcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const DString &dstReturnType,const ArgumentList *dstAl,
                     bool checkCV,SrcLangExt lang
                    );

void mergeArguments(ArgumentList &,ArgumentList &,bool forceNameOverwrite=false);

bool matchTemplateArguments(const ArgumentList &srcAl,const ArgumentList &dstAl);

DString substituteClassNames(const DString &s);

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

DString removeEmptyLines(const DString &s);


FileDef *findFileDef(const FileNameLinkedMap *fnMap, const DString &n, bool &ambig);
DString findFilePath(const DString &file, bool &ambig);

DString showFileDefMatches(const FileNameLinkedMap *fnMap,const DString &n);

EntryType guessSection(const DString &name);

inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c) || c=='$';
}
inline bool isIdJS(int c)
{
  return c>=128 || c<0 || isalnum(c);
}

DString removeRedundantWhiteSpace(const DString &s);

DString inlineArgListToDoc(const ArgumentList &al);
DString inlineTemplateArgListToDoc(const ArgumentList &al);

DString argListToString(const ArgumentList &al,bool useCanonicalType=false,bool showDefVals=true);

DString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefaults=true);

DString generateMarker(int id);

void writeExamples(OutputList &ol,const ExampleList &el);

DString stripAnonymousNamespaceScope(const DString &s);

DString stripFromPath(const DString &path);

DString stripFromIncludePath(const DString &path);

bool rightScopeMatch(const DString &scope, const DString &name);

bool leftScopeMatch(const DString &scope, const DString &name);

struct KeywordSubstitution
{
  const char *keyword;
  using GetValue          = std::function<DString()>;
  using GetValueWithParam = std::function<DString(const DString &)>;
  std::variant<GetValue,GetValueWithParam> getValueVariant;
};

using KeywordSubstitutionList = std::vector<KeywordSubstitution>;

DString substituteKeywords(const DString &file,const DString &s,const KeywordSubstitutionList &keywords);

DString substituteKeywords(const DString &file,const DString &s,const DString &title,
         const DString &projName,const DString &projNum,const DString &projBrief);

int getPrefixIndex(const DString &name);

DString removeAnonymousScopes(const DString &s);

DString replaceAnonymousScopes(const DString &s,const DString &replacement=DString());

DString convertNameToFile(const DString &name,bool allowDots=false,bool allowUnderscore=false);

DString generateAnonymousAnchor(const DString &fileName,int count);

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

DString getOverloadDocs();

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

bool getCaseSenseNames();

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

DString removeLongPathMarker(DString path);
DString stripPath(const DString &s);

bool containsWord(const DString &s,const char *word);

bool findAndRemoveWord(DString &s,const char *word);

DString stripLeadingAndTrailingEmptyLines(const DString &s,int &docLine);

bool updateLanguageMapping(const DString &extension,const DString &parser);
SrcLangExt getLanguageFromFileName(const DString& fileName, SrcLangExt defLang=SrcLangExt::Cpp);
SrcLangExt getLanguageFromCodeLang(DString &fileName);
DString getFileNameExtension(const DString &fn);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();

bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const DString &n);

DString parseCommentAsText(const Definition *scope,const MemberDef *member,const DString &doc,const DString &fileName,int lineNr);
DString parseCommentAsHtml(const Definition *scope,const MemberDef *member,const DString &doc,const DString &fileName,int lineNr);

bool transcodeCharacterStringToUTF8(std::string &input,const char *inputEncoding);

DString recodeString(const DString &str,const char *fromEncoding,const char *toEncoding);

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al);

DString convertCharEntitiesToUTF8(const DString &s);

void stackTrace();

bool readInputFile(const DString &fileName,std::string &contents,
                   bool filter=true,bool isSourceCode=false);
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
int nextUtf8CharPosition(const DString &utf8Str,uint32_t len,uint32_t startPos);

void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc);
DString writeMarkerList(const std::string &markerText,size_t numMarkers,
                     std::function<DString(size_t)> replaceFunc);

/** Data associated with a HSV colored image. */
struct ColoredImgDataItem
{
  const char *name;
  unsigned short width;
  unsigned short height;
  const unsigned char *content;
  const unsigned char *alpha;
};

DString replaceColorMarkers(const DString &str);

bool copyFile(const DString &src,const DString &dest);

int lineBlock(const DString &text,const DString &marker);

bool isURL(const DString &url);

DString correctURL(const DString &url,const DString &relPath);

DString processMarkup(const DString &s);

bool protectionLevelVisible(Protection prot);

DString stripIndentation(const DString &s,bool skipFirstLine=false);
void stripIndentationVerbatim(DString &doc,const int indentationLevel, bool skipFirstLine=true);

DString getDotImageExtension();

bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile);

DString extractDirection(DString &docs);

void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   MemberListType *outListType1,
                   MemberListType *outListType2
                  );

bool mainPageHasTitle();
bool openOutputFile(const DString &outFile,std::ofstream &f);

StringVector split(const std::string &s,const std::string &delimiter);
StringVector split(const std::string &s,const reg::Ex &delimiter);
int findIndex(const StringVector &sv,const std::string &s);
int findIndex(const std::string &s,const reg::Ex &re);
std::string join(const StringVector &s,const std::string &delimiter);

bool recognizeFixedForm(const DString &contents, FortranFormat format);
FortranFormat convertFileNameFortranParserCode(DString fn);

DString integerToAlpha(int n, bool upper=true);
DString integerToRoman(int n, bool upper=true);

DString getEncoding(const FileInfo &fi);

inline DString fixSpaces(const DString &s) { return substitute(s," ","&#160;"); }

DString detab(const DString &s,size_t &refIndent);

DString getProjectId();
DString projectLogoFile();

void mergeMemberOverrideOptions(MemberDefMutable *md1,MemberDefMutable *md2);

size_t updateColumnCount(const char *s,size_t col);

DString mangleCSharpGenericName(const DString &name);
DString demangleCSharpGenericName(const DString &name,const DString &templArgs);

DString extractBeginRawStringDelimiter(const char *rawStart);
DString extractEndRawStringDelimiter(const char *rawEnd);

DString writeFileContents(const DString &baseName,const DString &extension,const DString &content,bool &exists);
void cleanupInlineGraph();

using HtmlEntityMapperFunc = std::function<DString(HtmlEntityMapper::SymType)>;

/*! Writes an HTML entity for the current symbol and advances the input pointer.
 *  \tparam T Type of the output sink used to write encoded output.
 *  \param t Output target receiving the encoded entity or fallback text.
 *  \param s Pointer to the start of a potential HTML entity in the input text.
 *  \param mapper Callback that maps a entity symbol type to its HTML entity string.
 *  \param fallback Fallback string written when no entity mapping is available.
 *  \return Pointer to the position after the processed HTML entity in the input text.
 */
template<class T>
const char *writeHtmlEntity(T& t, const char *s, HtmlEntityMapperFunc &&mapper, const char *fallback);

#endif
