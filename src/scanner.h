/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef SCANNER_H
#define SCANNER_H

#include "parserintf.h"

/** \brief C-like language parser using state-based lexical scanning.
 *
 *  This is the language parser for doxygen. It is somewhat fuzzy and
 *  supports C++ and various languages that are closely related to C++, 
 *  such as C, C#, Objective-C, Java, PHP, and IDL.
 */
class CLanguageScanner : public ParserInterface
{
  public:
    virtual ~CLanguageScanner() {}
    void startTranslationUnit(const char *fileName);
    void finishTranslationUnit();
    void parseInput(const char *fileName,
                    const char *fileBuf,
                    Entry *root,
                    bool sameTranslationUnit,
                    QStrList &filesInSameTranslationUnit);
    bool needsPreprocessing(const QCString &extension);
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
                   MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE,
                   Definition *searchCtx=0
                  );
    void resetCodeParserState();
    void parsePrototype(const char *text);
};

void scanFreeScanner();

#endif
