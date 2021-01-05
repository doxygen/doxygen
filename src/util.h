/******************************************************************************
 *
 *
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

#ifndef UTIL_H
#define UTIL_H

/*! \file
 *  \brief A bunch of utility functions.
 */

#include <memory>
#include <unordered_map>
#include <algorithm>

#include <qlist.h>
#include <ctype.h>
#include "types.h"
#include "docparser.h"
#include "classdef.h"
#include "arguments.h"
#include "containers.h"
#include "namespacedef.h"
#include "outputgen.h"

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
class QDir;
class Definition;
class BufStr;
class QFileInfo;
class QStrList;
class FTextStream;
class QFile;

//--------------------------------------------------------------------

/** Abstract interface for a hyperlinked text fragment. */
class TextGeneratorIntf
{
  public:
    virtual ~TextGeneratorIntf() {}
    virtual void writeString(const char *,bool) const = 0;
    virtual void writeBreak(int indent) const = 0;
    virtual void writeLink(const char *extRef,const char *file,
                      const char *anchor,const char *text
                     ) const = 0;
};

/** Implements TextGeneratorIntf for an OutputDocInterface stream. */
class TextGeneratorOLImpl : public TextGeneratorIntf
{
  public:
    virtual ~TextGeneratorOLImpl() {}
    TextGeneratorOLImpl(OutputDocInterface &od);
    void writeString(const char *s,bool keepSpaces) const;
    void writeBreak(int indent) const;
    void writeLink(const char *extRef,const char *file,
                   const char *anchor,const char *text
                  ) const;
  private:
    OutputDocInterface &m_od;
};

//--------------------------------------------------------------------

QCString langToString(SrcLangExt lang);
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope=FALSE);

//--------------------------------------------------------------------

void linkifyText(const TextGeneratorIntf &ol,
                 const Definition *scope,
                 const FileDef *fileScope,
                 const Definition *self,
                 const char *text,
                 bool autoBreak=FALSE,
                 bool external=TRUE,
                 bool keepSpaces=FALSE,
                 int indentLevel=0
                );

QCString fileToString(const char *name,bool filter=FALSE,bool isSourceCode=FALSE);

QCString dateToString(bool);

bool getDefs(const QCString &scopeName,
                    const QCString &memberName,
                    const char *,
                    const MemberDef *&md,
                    const ClassDef *&cd,
                    const FileDef *&fd,
                    const NamespaceDef *&nd,
                    const GroupDef *&gd,
                    bool forceEmptyScope=FALSE,
                    const FileDef *currentFile=0,
                    bool checkCV=FALSE
                   );

QCString getFileFilter(const char* name,bool isSourceCode);

bool resolveRef(/* in */  const char *scName,
                /* in */  const char *name,
                /* in */  bool inSeeBlock,
                /* out */ const Definition **resContext,
                /* out */ const MemberDef  **resMember,
                /* in */  bool lookForSpecializations = TRUE,
                /* in */  const FileDef *currentFile = 0,
                /* in */  bool checkScope = FALSE
               );

bool resolveLink(/* in */  const char *scName,
                 /* in */  const char *lr,
                 /* in */  bool inSeeBlock,
                 /* out */ const Definition **resContext,
                 /* out */ QCString &resAnchor
                );

//bool generateRef(OutputDocInterface &od,const char *,
//                        const char *,bool inSeeBlock,const char * =0);

bool generateLink(OutputDocInterface &od,const char *,
                         const char *,bool inSeeBlock,const char *);

void generateFileRef(OutputDocInterface &od,const char *,
                             const char *linkTxt=0);

void writePageRef(OutputDocInterface &od,const char *cn,const char *mn);

QCString getCanonicalTemplateSpec(const Definition *d,const FileDef *fs,const QCString& spec);

bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const ArgumentList *srcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const ArgumentList *dstAl,
                     bool checkCV
                    );

void mergeArguments(ArgumentList &,ArgumentList &,bool forceNameOverwrite=FALSE);

QCString substituteClassNames(const QCString &s);


QCString clearBlock(const char *s,const char *begin,const char *end);
QCString selectBlock(const QCString& s,const QCString &name,bool enable, OutputGenerator::OutputType o);
QCString removeEmptyLines(const QCString &s);

QCString resolveDefines(const char *n);

ClassDef *getClass(const char *key);
inline ClassDefMutable *getClassMutable(const char *key)
{
  return toClassDefMutable(getClass(key));
}

NamespaceDef *getResolvedNamespace(const char *key);
inline NamespaceDefMutable *getResolvedNamespaceMutable(const char *key)
{
  return toNamespaceDefMutable(getResolvedNamespace(key));
}

FileDef *findFileDef(const FileNameLinkedMap *fnMap,const char *n,
                bool &ambig);

QCString showFileDefMatches(const FileNameLinkedMap *fnMap,const char *n);

int guessSection(const char *name);

inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c);
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

QCString substituteKeywords(const QCString &s,const char *title,
         const char *projName,const char *projNum,const char *projBrief);

int getPrefixIndex(const QCString &name);

QCString removeAnonymousScopes(const QCString &s);

QCString replaceAnonymousScopes(const QCString &s,const char *replacement=0);

bool hasVisibleRoot(const BaseClassList &bcl);
bool classHasVisibleChildren(const ClassDef *cd);
bool namespaceHasNestedNamespace(const NamespaceDef *nd);
bool namespaceHasNestedClass(const NamespaceDef *nd,bool filterClasses,ClassDef::CompoundType ct);
bool classVisibleInIndex(const ClassDef *cd);

int minClassDistance(const ClassDef *cd,const ClassDef *bcd,int level=0);
Protection classInheritedProtectionLevel(const ClassDef *cd,const ClassDef *bcd,Protection prot=Public,int level=0);

QCString convertNameToFile(const char *name,bool allowDots=FALSE,bool allowUnderscore=FALSE);

void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName,
                          bool allowEmptyClass=FALSE);

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);

QCString stripScope(const char *name);

QCString convertToId(const char *s);
QCString correctId(QCString s);

QCString convertToHtml(const char *s,bool keepEntities=TRUE);

QCString convertToLaTeX(const QCString &s,bool insideTabbing=FALSE,bool keepSpaces=FALSE);

QCString convertToXML(const char *s, bool keepEntities=FALSE);

QCString convertToDocBook(const char *s);

QCString convertToJSString(const char *s, bool applyTextDir = true);

QCString convertToPSString(const char *s);

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

int filterCRLF(char *buf,int len);

void addRefItem(const RefItemVector &sli,
                const char *key,
                const char *prefix,
                const char *name,
                const char *title,
                const char *args,
                const Definition *scope);

PageDef *addRelatedPage(const char *name,
                        const QCString &ptitle,
                        const QCString &doc,
                        const char *fileName,
                        int docLine,
                        int startLine,
                        const RefItemVector &sli = RefItemVector(),
                        GroupDef *gd=0,
                        const TagInfo *tagInfo=0,
                        bool xref=FALSE,
                        SrcLangExt lang=SrcLangExt_Unknown
                       );

QCString escapeCharsInString(const char *name,bool allowDots,bool allowUnderscore=FALSE);
QCString unescapeCharsInString(const char *s);

void addGroupListToTitle(OutputList &ol,const Definition *d);

void filterLatexString(FTextStream &t,const char *str,
                       bool insideTabbing,
                       bool insidePre,
                       bool insideItem,
                       bool insideTable,
                       bool keepSpaces);

QCString latexEscapeLabelName(const char *s);
QCString latexEscapeIndexChars(const char *s);
QCString latexEscapePDFString(const char *s);
QCString latexFilterURL(const char *s);

QCString rtfFormatBmkStr(const char *name);

QCString linkToText(SrcLangExt lang,const char *link,bool isFileName);

bool checkExtension(const char *fName, const char *ext);

QCString addHtmlExtensionIfMissing(const char *fName);

QCString stripExtensionGeneral(const char *fName, const char *ext);

QCString stripExtension(const char *fName);

void replaceNamespaceAliases(QCString &scope,int i);

int computeQualifiedIndex(const QCString &name);

void addDirPrefix(QCString &fileName);

QCString relativePathToRoot(const char *name);

void createSubDirs(QDir &d);

QCString stripPath(const char *s);

bool containsWord(const QCString &s,const QCString &word);

bool findAndRemoveWord(QCString &s,const QCString &word);

QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine);

bool updateLanguageMapping(const QCString &extension,const QCString &parser);
SrcLangExt getLanguageFromFileName(const QCString& fileName);
QCString getFileNameExtension(QCString fn);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();

bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const char *n);

QCString parseCommentAsText(const Definition *scope,const MemberDef *member,const QCString &doc,const QCString &fileName,int lineNr);

QCString transcodeCharacterStringToUTF8(const QCString &input);

QCString recodeString(const QCString &str,const char *fromEncoding,const char *toEncoding);

QCString extractAliasArgs(const QCString &args,int pos);

int countAliasArguments(const QCString argList);

QCString resolveAliasCmd(const QCString aliasCmd);
QCString expandAlias(const QCString &aliasName,const QCString &aliasValue);

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al);

QCString convertCharEntitiesToUTF8(const QCString &s);

void stackTrace();

bool readInputFile(const char *fileName,BufStr &inBuf,
                   bool filter=TRUE,bool isSourceCode=FALSE);
QCString filterTitle(const QCString &title);

bool patternMatch(const QFileInfo &fi,const StringVector &patList);

QCString externalLinkTarget(const bool parent = false);
QCString externalRef(const QCString &relPath,const QCString &ref,bool href);
int nextUtf8CharPosition(const QCString &utf8Str,uint len,uint startPos);
const char *writeUtf8Char(FTextStream &t,const char *s);


/** Data associated with a HSV colored image. */
struct ColoredImgDataItem
{
  const char *name;
  unsigned short width;
  unsigned short height;
  unsigned char *content;
  unsigned char *alpha;
};

void writeColoredImgData(const char *dir,ColoredImgDataItem data[]);
QCString replaceColorMarkers(const char *str);

bool copyFile(const QCString &src,const QCString &dest);
QCString extractBlock(const QCString text,const QCString marker);
int lineBlock(const QCString text,const QCString marker);

bool isURL(const QCString &url);

QCString correctURL(const QCString &url,const QCString &relPath);

QCString processMarkup(const QCString &s);

bool protectionLevelVisible(Protection prot);

QCString stripIndentation(const QCString &s);
void stripIndentation(QCString &doc,const int indentationLevel);

QCString getDotImageExtension();

bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile);

const int MAX_UTF8_CHAR_LEN = 4;
const int MAX_UTF8_CHAR_SIZE = MAX_UTF8_CHAR_LEN+1; // include 0 terminator
enum class CaseModifier
{
  None,
  ToUpper,
  ToLower
};
int getUtf8Char(const char *input,char ids[MAX_UTF8_CHAR_SIZE],CaseModifier modifier=CaseModifier::None);
uint getUtf8Code( const QCString& s, int idx );
uint getUtf8CodeToLower( const QCString& s, int idx );
uint getUtf8CodeToUpper( const QCString& s, int idx );

QCString extractDirection(QCString &docs);

void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   int *outListType1,
                   int *outListType2
                  );

bool mainPageHasTitle();
bool openOutputFile(const char *outFile,QFile &f);
void writeExtraLatexPackages(FTextStream &t);
void writeLatexSpecialFormulaChars(FTextStream &t);

bool recognizeFixedForm(const char* contents, FortranFormat format);
FortranFormat convertFileNameFortranParserCode(QCString fn);

#endif
