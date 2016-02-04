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

#include <qlist.h>
#include <ctype.h>
#include "types.h"
#include "sortdict.h"
#include "docparser.h"

//--------------------------------------------------------------------

class ClassDef;
class FileDef;
class MemberList;
class NamespaceDef;
class FileNameDict;
class ArgumentList;
class OutputList;
class OutputDocInterface;
class MemberDef;
class ExampleSDict;
class ClassSDict;
class BaseClassList;
class GroupDef;
class NamespaceSDict;
class ClassList;
class MemberGroupSDict;
struct TagInfo;
class MemberNameInfoSDict;
struct ListItemInfo;
class PageDef;
struct SectionInfo;
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

/** @brief maps a unicode character code to a list of T::ElementType's
 */
template<class T>
class LetterToIndexMap : public SIntDict<T>
{
  public:
    LetterToIndexMap() { SIntDict<T>::setAutoDelete(TRUE); }
    void append(uint letter,typename T::ElementType *elem)
    {
      T *l = SIntDict<T>::find((int)letter);
      if (l==0)
      {
        l = new T(letter);
        SIntDict<T>::inSort((int)letter,l);
      }
      l->append(elem);
    }
  private:
    int compareValues(const T *l1, const T *l2) const
    {
      return (int)l1->letter()-(int)l2->letter();
    }
};

//--------------------------------------------------------------------

QCString langToString(SrcLangExt lang);
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope=FALSE);

//--------------------------------------------------------------------

void linkifyText(const TextGeneratorIntf &ol,
                 Definition *scope,
                 FileDef *fileScope,
                 Definition *self,
                 const char *text,
                 bool autoBreak=FALSE,
                 bool external=TRUE,
                 bool keepSpaces=FALSE,
                 int indentLevel=0
                );

void setAnchors(MemberList *ml);

QCString fileToString(const char *name,bool filter=FALSE,bool isSourceCode=FALSE);

QCString dateToString(bool);

bool getDefs(const QCString &scopeName,
                    const QCString &memberName, 
                    const char *, 
                    MemberDef *&md, 
                    ClassDef *&cd,
                    FileDef *&fd, 
                    NamespaceDef *&nd,
                    GroupDef *&gd,
                    bool forceEmptyScope=FALSE,
                    FileDef *currentFile=0,
                    bool checkCV=FALSE,
                    const char *forceTagFile=0
                   );

QCString getFileFilter(const char* name,bool isSourceCode);

bool resolveRef(/* in */  const char *scName,
                /* in */  const char *name,
                /* in */  bool inSeeBlock,
                /* out */ Definition **resContext,
                /* out */ MemberDef  **resMember,
                /* in */  bool lookForSpecializations = TRUE,
                /* in */  FileDef *currentFile = 0,
                /* in */  bool checkScope = FALSE
               );

bool resolveLink(/* in */  const char *scName,
                 /* in */  const char *lr,
                 /* in */  bool inSeeBlock,
                 /* out */ Definition **resContext,
                 /* out */ QCString &resAnchor
                );

//bool generateRef(OutputDocInterface &od,const char *,
//                        const char *,bool inSeeBlock,const char * =0);

bool generateLink(OutputDocInterface &od,const char *,
                         const char *,bool inSeeBlock,const char *);

void generateFileRef(OutputDocInterface &od,const char *,
                             const char *linkTxt=0);

void writePageRef(OutputDocInterface &od,const char *cn,const char *mn);

QCString getCanonicalTemplateSpec(Definition *d,FileDef *fs,const QCString& spec);

bool matchArguments2(Definition *srcScope,FileDef *srcFileScope,ArgumentList *srcAl,
                     Definition *dstScope,FileDef *dstFileScope,ArgumentList *dstAl,
                     bool checkCV
                    );

void mergeArguments(ArgumentList *,ArgumentList *,bool forceNameOverwrite=FALSE);

QCString substituteClassNames(const QCString &s);

QCString substitute(const QCString &s,const QCString &src,const QCString &dst);

QCString clearBlock(const char *s,const char *begin,const char *end);

QCString selectBlock(const QCString& s,const QCString &name,bool which);

QCString resolveDefines(const char *n);

ClassDef *getClass(const char *key);

ClassDef *getResolvedClass(Definition *scope,
                           FileDef *fileScope,
                           const char *key,
                           MemberDef **pTypeDef=0,
                           QCString *pTemplSpec=0,
                           bool mayBeUnlinkable=FALSE,
                           bool mayBeHidden=FALSE,
                           QCString *pResolvedType=0);

NamespaceDef *getResolvedNamespace(const char *key);

FileDef *findFileDef(const FileNameDict *fnDict,const char *n,
                bool &ambig);

QCString showFileDefMatches(const FileNameDict *fnDict,const char *n);

int guessSection(const char *name);

inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c);
}

QCString removeRedundantWhiteSpace(const QCString &s);

QCString argListToString(ArgumentList *al,bool useCanonicalType=FALSE,bool showDefVals=TRUE);

QCString tempArgListToString(ArgumentList *al,SrcLangExt lang);

QCString generateMarker(int id);

void writeExample(OutputList &ol,ExampleSDict *el);

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

void initClassHierarchy(ClassSDict *cl);

bool hasVisibleRoot(BaseClassList *bcl);
bool classHasVisibleChildren(ClassDef *cd);
bool namespaceHasVisibleChild(NamespaceDef *nd,bool includeClasses);
bool classVisibleInIndex(ClassDef *cd);

int minClassDistance(const ClassDef *cd,const ClassDef *bcd,int level=0);
Protection classInheritedProtectionLevel(ClassDef *cd,ClassDef *bcd,Protection prot=Public,int level=0);

QCString convertNameToFile(const char *name,bool allowDots=FALSE,bool allowUnderscore=FALSE);

void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName,
                          bool allowEmptyClass=FALSE);

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);

QCString stripScope(const char *name);

QCString convertToId(const char *s);

QCString convertToHtml(const char *s,bool keepEntities=TRUE);

QCString convertToLaTeX(const QCString &s,bool insideTabbing=FALSE,bool keepSpaces=FALSE);

QCString convertToXML(const char *s);

QCString convertToJSString(const char *s);

QCString getOverloadDocs();

void addMembersToMemberGroup(/* in */     MemberList *ml,
                             /* in,out */ MemberGroupSDict **ppMemberGroupSDict,
                             /* in */     Definition *context);

int extractClassNameFromType(const QCString &type,int &pos,
                              QCString &name,QCString &templSpec,SrcLangExt=SrcLangExt_Unknown);

QCString normalizeNonTemplateArgumentsInString(
       const QCString &name,
       Definition *context,
       const ArgumentList *formalArgs);

QCString substituteTemplateArgumentsInString(
       const QCString &name,
       ArgumentList *formalArgs,
       ArgumentList *actualArgs);

QList<ArgumentList> *copyArgumentLists(const QList<ArgumentList> *srcLists);

QCString stripTemplateSpecifiersFromScope(const QCString &fullName,
                                          bool parentOnly=TRUE,
                                          QCString *lastScopeStripped=0);

QCString resolveTypeDef(Definition *d,const QCString &name,
                        Definition **typedefContext=0);

QCString mergeScopes(const QCString &leftScope,const QCString &rightScope);

int getScopeFragment(const QCString &s,int p,int *l);

int filterCRLF(char *buf,int len);

void addRefItem(const QList<ListItemInfo> *sli,const char *prefix,
                const char *key,
                const char *name,const char *title,const char *args,Definition *scope);

PageDef *addRelatedPage(const char *name,const QCString &ptitle,
                           const QCString &doc,QList<SectionInfo> *anchors,
                           const char *fileName,int startLine,
                           const QList<ListItemInfo> *sli,
                           GroupDef *gd=0,
                           TagInfo *tagInfo=0,
                           SrcLangExt lang=SrcLangExt_Unknown
                          );

QCString escapeCharsInString(const char *name,bool allowDots,bool allowUnderscore=FALSE);

void addGroupListToTitle(OutputList &ol,Definition *d);

void filterLatexString(FTextStream &t,const char *str,
                       bool insideTabbing=FALSE,
                       bool insidePre=FALSE,
                       bool insideItem=FALSE,
                       bool keepSpaces=FALSE);

QCString latexEscapeLabelName(const char *s,bool insideTabbing);
QCString latexEscapeIndexChars(const char *s,bool insideTabbing);
QCString latexEscapePDFString(const char *s);

QCString rtfFormatBmkStr(const char *name);

QCString linkToText(SrcLangExt lang,const char *link,bool isFileName);

bool checkExtension(const char *fName, const char *ext);

QCString stripExtensionGeneral(const char *fName, const char *ext);

QCString stripExtension(const char *fName);

void replaceNamespaceAliases(QCString &scope,int i);

int isAccessibleFrom(Definition *scope,FileDef *fileScope,Definition *item);

int isAccessibleFromWithExpScope(Definition *scope,FileDef *fileScope,Definition *item,
                     const QCString &explicitScopePart);

int computeQualifiedIndex(const QCString &name);

void addDirPrefix(QCString &fileName);

QCString relativePathToRoot(const char *name);

void createSubDirs(QDir &d);

QCString stripPath(const char *s);

bool containsWord(const QCString &s,const QCString &word);

bool findAndRemoveWord(QCString &s,const QCString &word);

QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine);

//void stringToSearchIndex(const QCString &docUrlBase,const QCString &title,
//                         const QCString &str, bool priority=FALSE,
//                         const QCString &anchor="");

bool updateLanguageMapping(const QCString &extension,const QCString &parser);
SrcLangExt getLanguageFromFileName(const QCString fileName);
void initDefaultExtensionMapping();
void addCodeOnlyMappings();

MemberDef *getMemberFromSymbol(Definition *scope,FileDef *fileScope, 
                                const char *n);
bool checkIfTypedef(Definition *scope,FileDef *fileScope,const char *n);

ClassDef *newResolveTypedef(FileDef *fileScope,MemberDef *md,
                            MemberDef **pMemType=0,QCString *pTemplSpec=0,
                            QCString *pResolvedType=0,
                            ArgumentList *actTemplParams=0);

QCString parseCommentAsText(const Definition *scope,const MemberDef *member,const QCString &doc,const QCString &fileName,int lineNr);

QCString transcodeCharacterStringToUTF8(const QCString &input);

QCString recodeString(const QCString &str,const char *fromEncoding,const char *toEncoding);

QCString extractAliasArgs(const QCString &args,int pos);

int countAliasArguments(const QCString argList);

//QCString replaceAliasArguments(const QCString &aliasValue,const QCString &argList);

QCString resolveAliasCmd(const QCString aliasCmd);
QCString expandAlias(const QCString &aliasName,const QCString &aliasValue);

void writeTypeConstraints(OutputList &ol,Definition *d,ArgumentList *al);

QCString convertCharEntitiesToUTF8(const QCString &s);

void stackTrace();

bool readInputFile(const char *fileName,BufStr &inBuf,
                   bool filter=TRUE,bool isSourceCode=FALSE);
QCString filterTitle(const QCString &title);

bool patternMatch(const QFileInfo &fi,const QStrList *patList);

QCString externalLinkTarget();
QCString externalRef(const QCString &relPath,const QCString &ref,bool href);
int nextUtf8CharPosition(const QCString &utf8Str,int len,int startPos);
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

QCString correctURL(const QCString &url,const QCString &relPath);

QCString processMarkup(const QCString &s);

bool protectionLevelVisible(Protection prot);

QCString stripIndentation(const QCString &s);

QCString getDotImageExtension(void);

bool fileVisibleInIndex(FileDef *fd,bool &genSourceFile);

void addDocCrossReference(MemberDef *src,MemberDef *dst);

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
const QDict<int> &getExtensionLookup();
bool openOutputFile(const char *outFile,QFile &f);

#endif

