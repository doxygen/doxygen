/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
class NamespaceList;
class ClassList;
class MemberGroupSDict;
class Definition;
struct TagInfo;
class MemberNameInfoSDict;
struct ListItemInfo;
class PageDef;

//--------------------------------------------------------------------

class TextGeneratorIntf
{
  public:
    virtual void writeString(const char *) const = 0;
    virtual void writeBreak() const = 0;
    virtual void writeLink(const char *extRef,const char *file,
                      const char *anchor,const char *text
                     ) const = 0; 
};

class TextGeneratorOLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorOLImpl(OutputDocInterface &od);
    void writeString(const char *s) const;
    void writeBreak() const;
    void writeLink(const char *extRef,const char *file,
                   const char *anchor,const char *text
                  ) const;
  private:
    OutputDocInterface &m_od;
};

//--------------------------------------------------------------------

void linkifyText(const TextGeneratorIntf &ol,Definition *scope,const char *name,
                        const char *text,bool autoBreak=FALSE,bool external=TRUE);
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

bool matchArguments(ArgumentList *,ArgumentList *,
                           const char *cl=0,const char *ns=0,bool checkCV=TRUE,
                           NamespaceList *usingNamespaces=0,
                           ClassList *usingClasses=0);
void mergeArguments(ArgumentList *,ArgumentList *,bool forceNameOverwrite=FALSE);
QCString substituteClassNames(const QCString &s);
QCString substitute(const char *s,const char *src,const char *dst);
QCString resolveDefines(const char *n);
ClassDef *getClass(const char *key);
ClassDef *getResolvedClass(Definition *scope,
                                  const char *key,
                                  bool *pIsTypeDef=0,
                                  QCString *pTemplSpec=0);
NamespaceDef *getResolvedNamespace(const char *key);
FileDef *findFileDef(const FileNameDict *fnDict,const char *n,
                bool &ambig);
QCString showFileDefMatches(const FileNameDict *fnDict,const char *n);
int guessSection(const char *name);
bool isId(char c);
QCString removeRedundantWhiteSpace(const QCString &s);
QCString argListToString(ArgumentList *al);
QCString tempArgListToString(ArgumentList *al);
QCString generateMarker(int id);
void writeExample(OutputList &ol,ExampleSDict *el);
QCString stripAnonymousNamespaceScope(const QCString &s);
QCString stripFromPath(const QCString &path);
bool rightScopeMatch(const QCString &scope, const QCString &name);
bool leftScopeMatch(const QCString &scope, const QCString &name);
QCString substituteKeywords(const QCString &s,const char *title);
int getPrefixIndex(const QCString &name);
QCString removeAnonymousScopes(const QCString &s);
QCString replaceAnonymousScopes(const QCString &s);
void initClassHierarchy(ClassSDict *cl);
bool hasVisibleRoot(BaseClassList *bcl);
int minClassDistance(ClassDef *cd,ClassDef *bcd,int level=0);
QCString convertNameToFile(const char *name,bool allowDots=FALSE);
void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName,
                          bool allowEmptyClass=FALSE);
QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);
QCString stripScope(const char *name);
int iSystem(const char *command,const char *args,bool isBatchFile=FALSE);
QCString convertToHtml(const char *s);
QCString convertToXML(const char *s);
const char * getOverloadDocs();
void addMembersToMemberGroup(MemberList *ml,
                             MemberGroupSDict *memberGroupSDict,
                             Definition *context);
bool extractClassNameFromType(const QCString &type,int &pos,
                              QCString &name,QCString &templSpec);
QCString substituteTemplateArgumentsInString(
       const QCString &name,
       ArgumentList *formalArgs,
       ArgumentList *actualArgs);

ArgumentList *copyArgumentList(const ArgumentList *src);
QList<ArgumentList> *copyArgumentLists(const QList<ArgumentList> *srcLists);
QCString stripTemplateSpecifiersFromScope(const QCString &fullName,
                                          bool parentOnly=TRUE);
QCString resolveTypeDef(Definition *d,const QCString &name,
                        Definition **typedefContext=0);
QCString mergeScopes(const QCString &leftScope,const QCString &rightScope);
int getScopeFragment(const QCString &s,int p,int *l);
int filterCRLF(char *buf,int len);
void addRefItem(const QList<ListItemInfo> *sli,const char *prefix,
                        const char *name,const char *title,const char *args=0);
PageDef *addRelatedPage(const char *name,const QCString &ptitle,
                           const QCString &doc,QList<QCString> *anchors,
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
QCString linkToText(const char *link);
QCString stripExtension(const char *fName);
void replaceNamespaceAliases(QCString &scope,int i);

#endif

