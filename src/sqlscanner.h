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

#ifndef SQLSCANNER_H
#define SQLSCANNER_H

#include "parserintf.h"
#include "sqlcode.h"

/** SQL scanner. Only support syntax highlighting of code at the moment.
 */
class SQLScanner : public ParserInterface
{
public:
    SQLScanner() {}
    virtual ~SQLScanner() {}
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char *, const char *, const std::unique_ptr<Entry> &, bool , QStrList &) {}
    bool needsPreprocessing(const QCString &) { return FALSE; }

    void parseCode(CodeOutputInterface &codeOutIntf,
                   const char *scopeName,
                   const QCString &input,
                   SrcLangExt,
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
                  )
    {
      parseSqlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                    fileDef,startLine,endLine,inlineFragment,memberDef,
                    showLineNumbers,searchCtx,collectXRefs);
    }

    void resetCodeParserState()
    {
      resetSqlCodeParserState();
    }

    void parsePrototype(const char *) {}

private:
};

#endif
