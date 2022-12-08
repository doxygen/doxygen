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

#include <ctype.h>
#include "types.h"
#include "docparser.h"
#include "containers.h"
#include "outputgen.h"
#include "conceptdef.h"

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
    virtual ~TextGeneratorIntf() = default;
    virtual void writeString(const QCString &,bool) const = 0;
    virtual void writeBreak(int indent) const = 0;
    virtual void writeLink(const QCString &extRef,const QCString &file,
                      const QCString &anchor,const QCString &text
                     ) const = 0;
};

/** Implements TextGeneratorIntf for an OutputDocInterface stream. */
class TextGeneratorOLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorOLImpl(BaseOutputDocInterface &od);
    void writeString(const QCString &s,bool keepSpaces) const;
    void writeBreak(int indent) const;
    void writeLink(const QCString &extRef,const QCString &file,
                   const QCString &anchor,const QCString &text
                  ) const;
  private:
    BaseOutputDocInterface &m_od;
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

bool getDefs(const QCString &scopeName,
                    const QCString &memberName,
                    const QCString &args,
                    const MemberDef *&md,
                    const ClassDef *&cd,
                    const FileDef *&fd,
                    const NamespaceDef *&nd,
                    const GroupDef *&gd,
                    bool forceEmptyScope=FALSE,
                    const FileDef *currentFile=0,
                    bool checkCV=FALSE
                   );

QCString getFileFilter(const QCString &name,bool isSourceCode);

bool resolveRef(/* in */  const QCString &scName,
                /* in */  const QCString &name,
                /* in */  bool inSeeBlock,
                /* out */ const Definition **resContext,
                /* out */ const MemberDef  **resMember,
                /* in */  bool lookForSpecializations = TRUE,
                /* in */  const FileDef *currentFile = 0,
                /* in */  bool checkScope = FALSE
               );

bool resolveLink(/* in */  const QCString &scName,
                 /* in */  const QCString &lr,
                 /* in */  bool inSeeBlock,
                 /* out */ const Definition **resContext,
                 /* out */ QCString &resAnchor
                );

bool generateLink(OutputList &ol,const QCString &,
                         const QCString &,bool inSeeBlock,const QCString &);

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


QCString clearBlock(const char *s,const char *begin,const char *end);
QCString selectBlock(const QCString& s,const QCString &name,bool enable, OutputType o);
QCString removeEmptyLines(const QCString &s);


FileDef *findFileDef(const FileNameLinkedMap *fnMap,const QCString &n,
                bool &ambig);

QCString showFileDefMatches(const FileNameLinkedMap *fnMap,const QCString &n);

int guessSection(const QCString &name);

inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c) || c=='$';
}
inline bool isIdJS(int c)
{
  return c>=128 || c<0 || isalnum(c);
}

QCString removeRedundantWhiteSpace(const QCString &s);

QCString argListToString(const ArgumentList &al,bool useCanonicalType=FALSE,bool showDefVals=TRUE);

QCString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefaults=true);

QCString generateMarker(int id);

void writeExamples(OutputList &ol,const ExampleList &el);

QCString stripAnonymousNamespaceScope(const QCString &s);

QCString stripFromPath(const QCString &path);

QCString stripFromIncludePath(const QCString &path);

bool rightScopeMatch(const QCString &scope, const QCString &name);

bool leftScopeMatch(const QCString &scope, const QCString &name);

QCString substituteKeywords(const QCString &s,const QCString &title,
         const QCString &projName,const QCString &projNum,const QCString &projBrief);

int getPrefixIndex(const QCString &name);

QCString removeAnonymousScopes(const QCString &s);

QCString replaceAnonymousScopes(const QCString &s,const QCString &replacement=QCString());

QCString convertNameToFile(const QCString &name,bool allowDots=FALSE,bool allowUnderscore=FALSE);

void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName,
                          bool allowEmptyClass=FALSE);

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);

QCString stripScope(const QCString &name);

QCString convertToId(const QCString &s);
QCString correctId(const QCString &s);

QCString convertToHtml(const QCString &s,bool keepEntities=TRUE);

QCString convertToLaTeX(const QCString &s,bool insideTabbing=FALSE,bool keepSpaces=FALSE);

QCString convertToXML(const QCString &s, bool keepEntities=FALSE);

QCString convertToDocBook(const QCString &s, const bool retainNewline = false);

QCString convertToJSString(const QCString &s);

QCString convertToPSString(const QCString &s);

QCString getOverloadDocs();

void addMembersToMemberGroup(/* in,out */ MemberList *ml,
                             /* in,out */ MemberGroupList *pMemberGroups,
                             /* in */     const Definition *context);

int extractClassNameFromType(const QCString &type,int &pos,
                              QCString &name,QCString &templSpec,SrcLangExt=SrcLangExt_Unknown);

QCString normalizeNonTemplateArgumentsInString(
       const QCString &name,
       const Definition *context,
       const ArgumentList &formalArgs);

QCString substituteTemplateArgumentsInString(
       const QCString &name,
       const ArgumentList &formalArgs,
       const std::unique_ptr<ArgumentList> &actualArgs);

QCString stripTemplateSpecifiersFromScope(const QCString &fullName,
                                          bool parentOnly=TRUE,
                                          QCString *lastScopeStripped=0);

QCString resolveTypeDef(const Definition *d,const QCString &name,
                        const Definition **typedefContext=0);

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
                        GroupDef *gd=0,
                        const TagInfo *tagInfo=0,
                        bool xref=FALSE,
                        SrcLangExt lang=SrcLangExt_Unknown
                       );

bool getCaseSenseNames();

QCString escapeCharsInString(const QCString &name,bool allowDots,bool allowUnderscore=FALSE);
QCString unescapeCharsInString(const QCString &s);

void addGroupListToTitle(OutputList &ol,const Definition *d);

void filterLatexString(TextStream &t,const QCString &str,
                       bool insideTabbing,
                       bool insidePre,
                       bool insideItem,
                       bool insideTable,
                       bool keepSpaces,
                       const bool retainNewline = false);

QCString latexEscapeLabelName(const QCString &s);
QCString latexEscapeIndexChars(const QCString &s);
QCString latexEscapePDFString(const QCString &s);
QCString latexFilterURL(const QCString &s);

QCString rtfFormatBmkStr(const QCString &name);

QCString linkToText(SrcLangExt lang,const QCString &link,bool isFileName);

bool checkExtension(const QCString &fName, const QCString &ext);

QCString addHtmlExtensionIfMissing(const QCString &fName);

QCString stripExtensionGeneral(const QCString &fName, const QCString &ext);

QCString stripExtension(const QCString &fName);

void replaceNamespaceAliases(QCString &scope,int i);

//! Return the index of the last :: in the string \a name that is still before the first <
inline int computeQualifiedIndex(const QCString &name)
{
  int l = static_cast<int>(name.length());
  int lastSepPos = -1;
  const char *p = name.data();
  for (int i=0;i<l-1;i++)
  {
    char c=*p++;
    if (c==':' && *p==':') lastSepPos=i;
    if (c=='<') break;
  }
  return lastSepPos;
}

void addDirPrefix(QCString &fileName);

QCString relativePathToRoot(const QCString &name);

void createSubDirs(const Dir &d);
void clearSubDirs(const Dir &d);

QCString stripPath(const QCString &s);

bool containsWord(const QCString &s,const char *word);

bool findAndRemoveWord(QCString &s,const char *word);

QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine);

bool updateLanguageMapping(const QCString &extension,const QCString &parser);
SrcLangExt getLanguageFromFileName(const QCString& fileName, SrcLangExt defLang=SrcLangExt_Cpp);
SrcLangExt getLanguageFromCodeLang(QCString &fileName);
QCString getFileNameExtension(const QCString &fn);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();

bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const QCString &n);

QCString parseCommentAsText(const Definition *scope,const MemberDef *member,const QCString &doc,const QCString &fileName,int lineNr);

QCString transcodeCharacterStringToUTF8(const QCString &inputEncoding,const QCString &input);

QCString recodeString(const QCString &str,const char *fromEncoding,const char *toEncoding);

QCString extractAliasArgs(const QCString &args,size_t pos);

int countAliasArguments(const QCString &argList);

QCString resolveAliasCmd(const QCString &aliasCmd);
std::string expandAlias(const std::string &aliasName,const std::string &aliasValue);

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al);

QCString convertCharEntitiesToUTF8(const QCString &s);

void stackTrace();

bool readInputFile(const QCString &fileName,BufStr &inBuf,
                   bool filter=TRUE,bool isSourceCode=FALSE);
QCString filterTitle(const QCString &title);

bool patternMatch(const FileInfo &fi,const StringVector &patList);

QCString externalLinkTarget(const bool parent = false);
QCString externalRef(const QCString &relPath,const QCString &ref,bool href);
int nextUtf8CharPosition(const QCString &utf8Str,uint len,uint startPos);

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
QCString extractBlock(const QCString &text,const QCString &marker);
int lineBlock(const QCString &text,const QCString &marker);

bool isURL(const QCString &url);

QCString correctURL(const QCString &url,const QCString &relPath);

QCString processMarkup(const QCString &s);

bool protectionLevelVisible(Protection prot);

QCString stripIndentation(const QCString &s);
void stripIndentation(QCString &doc,const int indentationLevel);

QCString getDotImageExtension();

bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile);

QCString extractDirection(QCString &docs);

void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   int *outListType1,
                   int *outListType2
                  );

bool mainPageHasTitle();
bool openOutputFile(const QCString &outFile,std::ofstream &f);
void writeExtraLatexPackages(TextStream &t);
void writeLatexSpecialFormulaChars(TextStream &t);

StringVector split(const std::string &s,const std::string &delimiter);

// Splits std::string \a str into an a StringVector \a result
// using delimiter \a DELIM which is a regular expression in the form
// of a string literal
//
// Note: we need to implement this as a macro as it is not possble to
// pass a constant expression as a parameter (which DELIM needs to be)
#define SPLIT_CTRE(str,DELIM)                                 \
  [](const std::string &s_) -> StringVector {                 \
    StringVector result_;                                     \
    static constexpr auto re_ = ctll::fixed_string{ DELIM };  \
    size_t p_ = 0;                                            \
    for (auto match_ : ctre::range<re_>(s_)) {                \
      size_t i_ = match_.begin() - s_.begin();                \
      if (i_>p_) result_.push_back(s_.substr(p_,i_-p_));      \
      p_ = i_+match_.size();                                  \
    }                                                         \
    if (p_<s_.length()) {                                     \
      result_.push_back(s_.substr(p_));                       \
    }                                                         \
    return result_;                                           \
  }(str)                                                      \


// Replaces occurrences of pattern \a PATTERN in string \a str
// by string \a replacement, and return the result after
// substitution
//
// Note: we need to implement this as a macro as it is not possble to
// pass a constant expression as a parameter (which PATTERN needs to be)
#define REPLACE_CTRE(str,PATTERN,replacement)                      \
  [](const std::string &s_,const std::string &r_) -> std::string { \
    static constexpr auto re_ = ctll::fixed_string{ PATTERN };     \
    std::string result_;                                           \
    result_.reserve(s_.length());                                  \
    size_t p_ = 0;                                                 \
    for (auto match_ : ctre::range<re_>(s_)) {                     \
      size_t i_ = match_.begin()-s_.begin();                       \
      result_+=s_.substr(p_,i_-p_);                                \
      result_+=r_;                                                 \
      p_ = i_+match_.size();                                       \
    }                                                              \
    result_+=s_.substr(p_);                                        \
    return result_;                                                \
  }(str,replacement)

int findIndex(const StringVector &sv,const std::string &s);

// Return the index inside \a str at which the first occurrence of
// \a PATTERN is found or -1 if no such pattern can be found.
//
// Note: we need to implement this as a macro as it is not possble to
// pass a constant expression as a parameter (which PATTERN needs to be)
#define FIND_INDEX_CTRE(str,PATTERN)                                  \
  [](const std::string &s_) -> int {                                  \
    static constexpr auto re_ = ctll::fixed_string{ PATTERN };        \
    auto match_ = ctre::search<re_>(s_);                              \
    return match_ ? static_cast<int>(match_.begin()-s_.begin()) : -1; \
  }(str)

std::string join(const StringVector &s,const std::string &delimiter);

bool recognizeFixedForm(const QCString &contents, FortranFormat format);
FortranFormat convertFileNameFortranParserCode(QCString fn);

QCString integerToAlpha(int n, bool upper=true);
QCString integerToRoman(int n, bool upper=true);

QCString getEncoding(const FileInfo &fi);

inline QCString fixSpaces(const QCString &s) { return substitute(s," ","&#160;"); }

#endif
