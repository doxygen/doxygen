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

#ifndef XMLSCANNER_H
#define XMLSCANNER_H

#include "parserintf.h"
#include "xmlcode.h"

/** XML scanner. Only support syntax highlighting of code at the moment.
 */
class XMLScanner : public ParserInterface
{
public:
    XMLScanner() {}
    virtual ~XMLScanner() {}
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char *, const char *, Entry *, bool , QStrList &) {}
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
                   MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE,
                   Definition *searchCtx=0,
                   bool collectXRefs=TRUE
                  )
    {
      parseXmlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                    fileDef,startLine,endLine,inlineFragment,memberDef,
                    showLineNumbers,searchCtx,collectXRefs);
    }

    void resetCodeParserState()
    {
      resetXmlCodeParserState();
    }

    void parsePrototype(const char *) {}

private:
};

#endif
