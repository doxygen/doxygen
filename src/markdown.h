/******************************************************************************
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

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <qcstring.h>
#include "parserintf.h"

class Entry;

/** processes string \a s and converts markdown into doxygen/html commands. */
QCString processMarkdown(const QCString &fileName,const int lineNr,Entry *e,const QCString &s);
QCString markdownFileNameToId(const QCString &fileName);

class MarkdownFileParser : public ParserInterface
{
  public:
    virtual ~MarkdownFileParser() {}
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char *fileName, 
                    const char *fileBuf, 
                    Entry *root,
                    bool sameTranslationUnit,
                    QStrList &filesInSameTranslationUnit);
    bool needsPreprocessing(const QCString &) { return FALSE; }
    void parseCode(CodeOutputInterface &codeOutIntf,
                   const char *scopeName,
                   const QCString &input,
                   SrcLangExt lang,
                   bool isExampleBlock,
                   const char *exampleName=0,
                   FileDef *fileDef=0,
                   int startLine=-1,
                   int endLine=-1,
                   bool inlineFragment=FALSE,
                   const MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE,
                   const Definition *searchCtx=0,
                   bool collectXRefs=TRUE
                  );
    void resetCodeParserState();
    void parsePrototype(const char *text);
};




#endif
