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
class BufStr;
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
    virtual void writeLink(const QCString &extRef,const QCString &file,
                      const QCString &anchor,std::string_view text
                     ) const = 0;
};

/** Implements TextGeneratorIntf for an OutputDocInterface stream. */
class TextGeneratorOLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorOLImpl(OutputList &ol);
    void writeString(std::string_view s,bool keepSpaces) const override;
    void writeBreak(int indent) const override;
    void writeLink(const QCString &extRef,const QCString &file,
                   const QCString &anchor,std::string_view text
                  ) const override;
  private:
    OutputList &m_ol;
};

//--------------------------------------------------------------------

QCString langToString(SrcLangExt lang);
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope=FALSE);

//--------------------------------------------------------------------

void linkifyText(const TextGeneratorIntf &ol,
                 const Definition *scope,
                 const FileDef *fileScope,
                 const Definition *self,
                 const QCString &text,
                 bool autoBreak=FALSE,
                 bool external=TRUE,
                 bool keepSpaces=FALSE,
                 int indentLevel=0
                );

QCString fileToString(const QCString &name,bool filter=FALSE,bool isSourceCode=FALSE);

struct GetDefInput
{
  GetDefInput(const QCString &scName,const QCString &memName,const QCString &a) :
    scopeName(scName),memberName(memName),args(a) {}
  QCString scopeName;
  QCString memberName;
  QCString args;
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
};

GetDefResult getDefs(const GetDefInput &input);

QCString getFileFilter(const QCString &name,bool isSourceCode);

bool resolveRef(/* in */  const QCString &scName,
                /* in */  const QCString &name,
                /* in */  bool inSeeBlock,
                /* out */ const Definition **resContext,
                /* out */ const MemberDef  **resMember,
                /* in */  bool lookForSpecializations = TRUE,
                /* in */  const FileDef *currentFile = nullptr,
                /* in */  bool checkScope = FALSE
               );

bool resolveLink(/* in */  const QCString &scName,
                 /* in */  const QCString &lr,
                 /* in */  bool inSeeBlock,
                 /* out */ const Definition **resContext,
                 /* out */ QCString &resAnchor,
                 /* in */  const QCString &prefix=QCString()
                );

void generateFileRef(OutputList &ol,const QCString &,
                             const QCString &linkTxt=QCString());

void writePageRef(OutputList &ol,const QCString &cn,const QCString &mn);

//QCString getCanonicalTemplateSpec(const Definition *d,const FileDef *fs,const QCString& spec);

bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const ArgumentList *srcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const ArgumentList *dstAl,
                     bool checkCV,SrcLangExt lang
                    );

void mergeArguments(ArgumentList &,ArgumentList &,bool forceNameOverwrite=FALSE);

QCString substituteClassNames(const QCString &s);

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

QCString selectBlocks(const QCString& s,const SelectionBlockList &blockList, const SelectionMarkerInfo &markerInfo);
void checkBlocks(const QCString& s,const QCString fileName, const SelectionMarkerInfo &markerInfo);

QCString removeEmptyLines(const QCString &s);


FileDef *findFileDef(const FileNameLinkedMap *fnMap, const QCString &n, bool &ambig);
QCString findFilePath(const QCString &file, bool &ambig);

QCString showFileDefMatches(const FileNameLinkedMap *fnMap,const QCString &n);

EntryType guessSection(const QCString &name);

inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c) || c=='$';
}
inline bool isIdJS(int c)
{
  return c>=128 || c<0 || isalnum(c);
}

QCString removeRedundantWhiteSpace(const QCString &s);

QCString inlineArgListToDoc(const ArgumentList &al);

QCString argListToString(const ArgumentList &al,bool useCanonicalType=FALSE,bool showDefVals=TRUE);

QCString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefaults=true);

QCString generateMarker(int id);

void writeExamples(OutputList &ol,const ExampleList &el);

QCString stripAnonymousNamespaceScope(const QCString &s);

QCString stripFromPath(const QCString &path);

QCString stripFromIncludePath(const QCString &path);

bool rightScopeMatch(const QCString &scope, const QCString &name);

bool leftScopeMatch(const QCString &scope, const QCString &name);

struct KeywordSubstitution
{
  const char *keyword;
  using GetValue          = std::function<QCString()>;
  using GetValueWithParam = std::function<QCString(const QCString &)>;
  std::variant<GetValue,GetValueWithParam> getValueVariant;
};

using KeywordSubstitutionList = std::vector<KeywordSubstitution>;

QCString substituteKeywords(const QCString &s,const KeywordSubstitutionList &keywords);

QCString substituteKeywords(const QCString &s,const QCString &title,
         const QCString &projName,const QCString &projNum,const QCString &projBrief);

int getPrefixIndex(const QCString &name);

QCString removeAnonymousScopes(const QCString &s);

QCString replaceAnonymousScopes(const QCString &s,const QCString &replacement=QCString());

QCString convertNameToFile(const QCString &name,bool allowDots=FALSE,bool allowUnderscore=FALSE);

QCString generateAnonymousAnchor(const QCString &fileName,int count);

void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName,
                          bool allowEmptyClass=FALSE);

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);

QCString stripScope(const QCString &name);

QCString convertToId(const QCString &s);
QCString correctId(const QCString &s);

QCString convertToHtml(const QCString &s,bool keepEntities=TRUE);

QCString convertToXML(const QCString &s, bool keepEntities=FALSE);

QCString convertToJSString(const QCString &s);

QCString getOverloadDocs();

void addMembersToMemberGroup(/* in,out */ MemberList *ml,
                             /* in,out */ MemberGroupList *pMemberGroups,
                             /* in */     const Definition *context);

int extractClassNameFromType(const QCString &type,int &pos,
                              QCString &name,QCString &templSpec,SrcLangExt=SrcLangExt::Unknown);

QCString normalizeNonTemplateArgumentsInString(
       const QCString &name,
       const Definition *context,
       const ArgumentList &formalArgs);

QCString substituteTemplateArgumentsInString(
       const QCString &name,
       const ArgumentList &formalArgs,
       const ArgumentList *actualArgs);

QCString stripTemplateSpecifiersFromScope(const QCString &fullName,
                                          bool parentOnly=TRUE,
                                          QCString *lastScopeStripped=nullptr,
                                          QCString scopeName=QCString(),
                                          bool allowArtificial=true);

QCString resolveTypeDef(const Definition *d,const QCString &name,
                        const Definition **typedefContext=nullptr);

QCString mergeScopes(const QCString &leftScope,const QCString &rightScope);

int getScopeFragment(const QCString &s,int p,int *l);

void addRefItem(const RefItemVector &sli,
                const QCString &key,
                const QCString &prefix,
                const QCString &name,
                const QCString &title,
                const QCString &args,
                const Definition *scope);

PageDef *addRelatedPage(const QCString &name,
                        const QCString &ptitle,
                        const QCString &doc,
                        const QCString &fileName,
                        int docLine,
                        int startLine,
                        const RefItemVector &sli = RefItemVector(),
                        GroupDef *gd=nullptr,
                        const TagInfo *tagInfo=nullptr,
                        bool xref=FALSE,
                        SrcLangExt lang=SrcLangExt::Unknown
                       );

bool getCaseSenseNames();

QCString escapeCharsInString(const QCString &name,bool allowDots,bool allowUnderscore=FALSE);
QCString unescapeCharsInString(const QCString &s);

void addGroupListToTitle(OutputList &ol,const Definition *d);

QCString linkToText(SrcLangExt lang,const QCString &link,bool isFileName);

bool checkExtension(const QCString &fName, const QCString &ext);

void addHtmlExtensionIfMissing(QCString &fName);

QCString stripExtensionGeneral(const QCString &fName, const QCString &ext);

QCString stripExtension(const QCString &fName);

//void replaceNamespaceAliases(QCString &scope,size_t i);

int computeQualifiedIndex(const QCString &name);

void addDirPrefix(QCString &fileName);

QCString relativePathToRoot(const QCString &name);
QCString determineAbsoluteIncludeName(const QCString &curFile,const QCString &incFileName);

void createSubDirs(const Dir &d);
void clearSubDirs(const Dir &d);

QCString removeLongPathMarker(QCString path);
QCString stripPath(const QCString &s);

bool containsWord(const QCString &s,const char *word);

bool findAndRemoveWord(QCString &s,const char *word);

QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine);

bool updateLanguageMapping(const QCString &extension,const QCString &parser);
SrcLangExt getLanguageFromFileName(const QCString& fileName, SrcLangExt defLang=SrcLangExt::Cpp);
SrcLangExt getLanguageFromCodeLang(QCString &fileName);
QCString getFileNameExtension(const QCString &fn);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();

bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const QCString &n);

QCString parseCommentAsText(const Definition *scope,const MemberDef *member,const QCString &doc,const QCString &fileName,int lineNr);

bool transcodeCharacterStringToUTF8(std::string &input,const char *inputEncoding);

QCString recodeString(const QCString &str,const char *fromEncoding,const char *toEncoding);

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al);

QCString convertCharEntitiesToUTF8(const QCString &s);

void stackTrace();

bool readInputFile(const QCString &fileName,std::string &contents,
                   bool filter=TRUE,bool isSourceCode=FALSE);
QCString filterTitle(const QCString &title);

bool patternMatch(const FileInfo &fi,const StringVector &patList);

QCString externalLinkTarget(const bool parent = false);
QCString createHtmlUrl(const QCString &relPath,
                       const QCString &ref,
                       bool href,
                       bool islocalFile,
                       const QCString &targetFileName,
                       const QCString &anchor);
QCString externalRef(const QCString &relPath,const QCString &ref,bool href);
int nextUtf8CharPosition(const QCString &utf8Str,uint32_t len,uint32_t startPos);

void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc);

/** Data associated with a HSV colored image. */
struct ColoredImgDataItem
{
  const char *name;
  unsigned short width;
  unsigned short height;
  const unsigned char *content;
  const unsigned char *alpha;
};

void writeColoredImgData(const QCString &dir,ColoredImgDataItem data[]);
QCString replaceColorMarkers(const QCString &str);

bool copyFile(const QCString &src,const QCString &dest);

int lineBlock(const QCString &text,const QCString &marker);

bool isURL(const QCString &url);

QCString correctURL(const QCString &url,const QCString &relPath);

QCString processMarkup(const QCString &s);

bool protectionLevelVisible(Protection prot);

QCString stripIndentation(const QCString &s);
void stripIndentationVerbatim(QCString &doc,const int indentationLevel);

QCString getDotImageExtension();

bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile);

QCString extractDirection(QCString &docs);

void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   MemberListType *outListType1,
                   MemberListType *outListType2
                  );

bool mainPageHasTitle();
bool openOutputFile(const QCString &outFile,std::ofstream &f);

StringVector split(const std::string &s,const std::string &delimiter);
StringVector split(const std::string &s,const reg::Ex &delimiter);
int findIndex(const StringVector &sv,const std::string &s);
int findIndex(const std::string &s,const reg::Ex &re);
std::string join(const StringVector &s,const std::string &delimiter);

bool recognizeFixedForm(const QCString &contents, FortranFormat format);
FortranFormat convertFileNameFortranParserCode(QCString fn);

QCString integerToAlpha(int n, bool upper=true);
QCString integerToRoman(int n, bool upper=true);

QCString getEncoding(const FileInfo &fi);

inline QCString fixSpaces(const QCString &s) { return substitute(s," ","&#160;"); }

QCString detab(const QCString &s,size_t &refIndent);

QCString getProjectId();
QCString projectLogoFile();

void mergeMemberOverrideOptions(MemberDefMutable *md1,MemberDefMutable *md2);

#endif
