/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
class ExampleList;
class ClassList;
class BaseClassList;
class GroupDef;
class NamespaceList;

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

extern void linkifyText(const TextGeneratorIntf &ol,const char *clName,const char *name,
                        const char *text,bool autoBreak=FALSE,bool external=TRUE);

extern void setAnchors(char id,MemberList *ml,int groupId=-1);
extern QCString fileToString(const char *name,bool filter=FALSE);
extern QCString dateToString(bool);
extern bool getDefs(const QCString &scopeName,
                    const QCString &memberName, 
                    const char *, 
                    MemberDef *&md, 
                    ClassDef *&cd,
                    FileDef *&fd, 
                    NamespaceDef *&nd,
                    GroupDef *&gd,
                    bool forceEmptyScope=FALSE,
                    FileDef *currentFile=0
                   );

extern bool generateRef(OutputDocInterface &od,const char *,
                        const char *,bool inSeeBlock,const char * =0);
extern bool generateLink(OutputDocInterface &od,const char *,
                         const char *,bool inSeeBlock,const char *);
extern void generateFileRef(OutputDocInterface &od,const char *,
                             const char *linkTxt=0);
void writePageRef(OutputDocInterface &od,const char *cn,const char *mn);

extern bool matchArguments(ArgumentList *,ArgumentList *,
                           const char *cl=0,const char *ns=0,bool checkCV=TRUE,
                           NamespaceList *usingList=0);
extern void mergeArguments(ArgumentList *,ArgumentList *);
extern QCString substituteClassNames(const QCString &s);
extern QCString convertFileName(const QCString &s);
extern QCString substitute(const char *s,const char *src,const char *dst);
extern QCString resolveDefines(const char *n);
extern ClassDef *getClass(const char *key);
extern ClassDef *getResolvedClass(const char *key,bool *pIsTypeDef=0);
extern NamespaceDef *getResolvedNamespace(const char *key);
extern FileDef *findFileDef(const FileNameDict *fnDict,const char *n,
                bool &ambig);
extern QCString showFileDefMatches(const FileNameDict *fnDict,const char *n);
extern int guessSection(const char *name);
extern bool isId(char c);
extern QCString removeRedundantWhiteSpace(const QCString &s);
//extern void startTitle(OutputList &ol,const char *fileName);
//extern void endTitle(OutputList &ol,const char *fileName,const char *name);
//void startFile(OutputList &ol,const char *name,
//               const char *title,bool external=FALSE);
//void endFile(OutputList &ol,bool external=FALSE);
//void writeQuickLinks(OutputList &ol,bool compact,bool external=FALSE);
QCString argListToString(ArgumentList *al);
QCString tempArgListToString(ArgumentList *al);
QCString generateMarker(int id);
void writeExample(OutputList &ol,ExampleList *el);
//void setFileNameForSections(QList<QCString> *anchorList,const char *fileName,
//                            PageInfo *pi=0);
QCString stripAnnonymousScope(const QCString &s);
QCString stripAnnonymousNamespaceScope(const QCString &s);
QCString stripFromPath(const QCString &path);
bool rightScopeMatch(const QCString &scope, const QCString &name);
bool leftScopeMatch(const QCString &scope, const QCString &name);
QCString substituteKeywords(const QCString &s,const char *title);
int getPrefixIndex(const QCString &name);
QCString removeAnonymousScopes(const QCString &s);
QCString replaceAnonymousScopes(const QCString &s);
void initClassHierarchy(ClassList *cl);
bool hasVisibleRoot(BaseClassList *bcl);
int minClassDistance(ClassDef *cd,ClassDef *bcd,int level=0);
QCString convertNameToFile(const char *name,bool allowDots=FALSE);
void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName);
QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ);
QCString stripScope(const char *name);
int iSystem(const char *command,const char *args,bool isBatchFile=FALSE);
QCString convertToHtml(const char *s);
QCString convertToXML(const char *s);
const char * getOverloadDocs();

#endif

