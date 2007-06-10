/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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

/*! \file util.h 
 *  \brief A bunch of utility functions.
 */

#include "qtbc.h"
#include <qlist.h>
#include <qtextstream.h>
#include <ctype.h>
#include "sortdict.h"

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

//--------------------------------------------------------------------

class TextGeneratorIntf
{
  public:
    virtual ~TextGeneratorIntf() {}
    virtual void writeString(const char *,bool) const = 0;
    virtual void writeBreak() const = 0;
    virtual void writeLink(const char *extRef,const char *file,
                      const char *anchor,const char *text
                     ) const = 0; 
};

class TextGeneratorOLImpl : public TextGeneratorIntf
{
  public:
    virtual ~TextGeneratorOLImpl() {}
    TextGeneratorOLImpl(OutputDocInterface &od);
    void writeString(const char *s,bool keepSpaces) const;
    void writeBreak() const;
    void writeLink(const char *extRef,const char *file,
                   const char *anchor,const char *text
                  ) const;
  private:
    OutputDocInterface &m_od;
};

//--------------------------------------------------------------------

enum SrcLangExt
{
  SrcLangExt_IDL    = 0x008,
  SrcLangExt_Java   = 0x010,
  SrcLangExt_CSharp = 0x020,
  SrcLangExt_D      = 0x040,
  SrcLangExt_PHP    = 0x080,
  SrcLangExt_ObjC   = 0x100,
  SrcLangExt_Cpp    = 0x200,
};

//--------------------------------------------------------------------

void linkifyText(const TextGeneratorIntf &ol,
                 Definition *scope,
                 FileDef *fileScope,
                 const char *name,
                 const char *text,
                 bool autoBreak=FALSE,
                 bool external=TRUE,
                 bool keepSpaces=FALSE
                );

void setAnchors(ClassDef *cd,char id,MemberList *ml,int groupId=-1);

QCString fileToString(const char *name,bool filter=FALSE);

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
                    bool checkCV=FALSE
                   );

QCString getFileFilter(const char* name);

bool resolveRef(/* in */  const char *scName,
                /* in */  const char *name,
                /* in */  bool inSeeBlock,
                /* out */ Definition **resContext,
                /* out */ MemberDef  **resMember
               );

bool resolveLink(/* in */  const char *scName,
                 /* in */  const char *lr,
                 /* in */  bool inSeeBlock,
                 /* out */ Definition **resContext,
                 /* out */ QCString &resAnchor
                );

bool generateRef(OutputDocInterface &od,const char *,
                        const char *,bool inSeeBlock,const char * =0);

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

QCString substitute(const char *s,const char *src,const char *dst);

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

bool isId(int c);

QCString removeRedundantWhiteSpace(const QCString &s);

QCString argListToString(ArgumentList *al,bool useCanonicalType=FALSE);

QCString tempArgListToString(ArgumentList *al);

QCString generateMarker(int id);

void writeExample(OutputList &ol,ExampleSDict *el);

QCString stripAnonymousNamespaceScope(const QCString &s);

QCString stripFromPath(const QCString &path);

QCString stripFromIncludePath(const QCString &path);

bool rightScopeMatch(const QCString &scope, const QCString &name);

bool leftScopeMatch(const QCString &scope, const QCString &name);

QCString substituteKeywords(const QCString &s,const char *title,const QCString &relPath="");

int getPrefixIndex(const QCString &name);

QCString removeAnonymousScopes(const QCString &s);

QCString replaceAnonymousScopes(const QCString &s);

void initClassHierarchy(ClassSDict *cl);

bool hasVisibleRoot(BaseClassList *bcl);

int minClassDistance(const ClassDef *cd,const ClassDef *bcd,int level=0);

QCString convertNameToFile(const char *name,bool allowDots=FALSE);

void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName,
                          bool allowEmptyClass=FALSE);

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);

QCString stripScope(const char *name);

QCString convertToHtml(const char *s);

QCString convertToXML(const char *s);

QCString getOverloadDocs();

void addMembersToMemberGroup(/* in */     MemberList *ml,
                             /* in,out */ MemberGroupSDict **ppMemberGroupSDict,
                             /* in */     Definition *context);

int extractClassNameFromType(const QCString &type,int &pos,
                              QCString &name,QCString &templSpec);

QCString substituteTemplateArgumentsInString(
       const QCString &name,
       ArgumentList *formalArgs,
       ArgumentList *actualArgs);

ArgumentList *copyArgumentList(const ArgumentList *src);

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
                        const char *name,const char *title,const char *args=0);

PageDef *addRelatedPage(const char *name,const QCString &ptitle,
                           const QCString &doc,QList<SectionInfo> *anchors,
                           const char *fileName,int startLine,
                           const QList<ListItemInfo> *sli,
                           GroupDef *gd=0,
                           TagInfo *tagInfo=0
                          );

QCString escapeCharsInString(const char *name,bool allowDots);

void addGroupListToTitle(OutputList &ol,Definition *d);

void filterLatexString(QTextStream &t,const char *str,
                       bool insideTabbing=FALSE,bool insidePre=FALSE,
                       bool insideItem=FALSE);

QCString rtfFormatBmkStr(const char *name);

QCString linkToText(const char *link,bool isFileName);

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

QCString stripLeadingAndTrailingEmptyLines(const QCString &s);

void stringToSearchIndex(const QCString &docUrlBase,const QCString &title,
                         const QCString &str, bool priority=FALSE,
                         const QCString &anchor="");

SrcLangExt getLanguageFromFileName(const QCString fileName);

bool checkIfTypedef(Definition *scope,FileDef *fileScope,const char *n);

ClassDef *newResolveTypedef(FileDef *fileScope,MemberDef *md,
                            MemberDef **pMemType=0,QCString *pTemplSpec=0,
                            QCString *pResolvedType=0);

QCString parseCommentAsText(const QString &doc,const QCString &fileName,int lineNr);

QCString transcodeCharacterStringToUTF8(const QCString &input);

QCString recodeString(const QCString &str,const char *fromEncoding,const char *toEncoding);

QCString extractAliasArgs(const QCString &args,int pos);

int countAliasArguments(const QCString argList);

QCString replaceAliasArguments(const QCString &aliasValue,const QCString &argList);

QCString resolveAliasCmd(const QCString aliasCmd);
QCString expandAlias(const QCString &aliasName,const QCString &aliasValue);

#endif

