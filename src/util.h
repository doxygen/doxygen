/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef UTIL_H
#define UTIL_H

#include <qlist.h>
#include <qtstream.h>
#include <ctype.h>
//#include "filedef.h"
//#include "filename.h"
#include "memberdef.h"
//#include "outputlist.h"


class ClassDef;
class FileDef;
class MemberList;
class NamespaceDef;
class FileNameDict;
class ArgumentList;
class OutputList;

extern void writeMemberDecs(OutputList &ol, ClassDef *cd, NamespaceDef *nd,
                 FileDef *fd, const char *title, const char *subtitle,
                 MemberList *ml);
extern void writeMemberDocs(OutputList &ol,
                 MemberList *ml,const char *scopeName,MemberDef::MemberType m);
extern void setAnchors(char id,MemberList *ml);
//extern int  countMemberDocs(MemberList *ml,MemberDef::MemberType m);
extern QString fileToString(const char *name);
extern QString dateToString(bool);
//extern OutputList linkifyText(const char *clName,const char *name,
//                        const char *text);
extern bool getDefs(const QString &memberName,const QString &className, 
                    const char *, MemberDef *&md, ClassDef *&cd,FileDef *&fd);
extern void generateRef(OutputList &ol,const char *,
                        const char *,bool inSeeBlock,const char * =0);
extern void generateLink(OutputList &ol,const char *,
                         const char *,bool inSeeBlock,const char *);
extern void generateClassRef(OutputList &ol,const char *clName,
                             const char *linkTxt=0);
extern void generateFileRef(OutputList &ol,const char *,
                             const char *linkTxt=0);
extern bool matchArguments(ArgumentList *,ArgumentList *,
                           const char *cl=0,const char *ns=0);
extern void mergeArguments(ArgumentList *,ArgumentList *);
extern QString substituteClassNames(const QString &s);
extern QString convertSlashes(const QString &s,bool dots=FALSE);
extern QString substitute(const char *s,const char *src,const char *dst);
extern QString resolveDefines(const char *n);
extern ClassDef *getClass(const char *key);
//extern int strcscmp(const char *s1,const char *s2);
extern FileDef *findFileDef(const FileNameDict *fnDict,const char *n,
                bool &ambig);
extern void showFileDefMatches(const FileNameDict *fnDict,const char *n);
extern int guessSection(const char *name);
//extern QString nameToFile(const char *name);
extern bool isId(char c);
extern QString removeRedundantWhiteSpace(const QString &s);
extern void startTitle(OutputList &ol);
extern void endTitle(OutputList &ol,const char *name);
void startFile(OutputList &ol,const char *name,
               const char *title,bool external=FALSE);
void endFile(OutputList &ol,bool external=FALSE);
void writeQuickLinks(OutputList &ol,bool compact,bool external=FALSE);
QString argListToString(ArgumentList *al);

#endif
