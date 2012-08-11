/******************************************************************************
 *
 *
 *
 * Copyright (C) 2009 by Tobias Hunger <tobias@aquazul.com>
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

#ifndef SCANNER_DBUSXML_H
#define SCANNER_DBUSXML_H

#include "parserintf.h"

/** D-Bus XML parser.
 *
 *  This is the D-Bus XML parser for doxygen.
 */
class DBusXMLScanner : public ParserInterface
{
public:
    DBusXMLScanner();
    virtual ~DBusXMLScanner();
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

private:
};

#endif
