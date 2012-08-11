/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
 * Copyright (C) 2010-2011 by Rene Zaumseil
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

#ifndef SCANNER_TCL_H
#define SCANNER_TCL_H

#include "parserintf.h"

/** \brief Tcl language parser using state-based lexical scanning.
 *
 *  This is the Tcl language parser for doxygen.
 */
class TclLanguageScanner : public ParserInterface
{
  public:
    virtual ~TclLanguageScanner() {}
    void parseInput(const char *fileName,
                    const char *fileBuf,
                    Entry *root);
    bool needsPreprocessing(const QCString &extension);
    void parseCode(CodeOutputInterface &codeOutIntf,
                   const char *scopeName,
                   const QCString &input,
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

#endif
