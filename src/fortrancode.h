/******************************************************************************
 *
 * 
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

#ifndef FORTRANCODE_H
#define FORTRANCODE_H

#include "parserintf.h"

class CodeOutputInterface;
class FileDef;
class MemberDef;
class QCString;
class Definition;

void codeFreeScanner();


class FortranCodeParser : public CodeParserInterface
{
  public:
    FortranCodeParser(FortranFormat format=FortranFormat_Unknown) : m_format(format) { }
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

  private:
    FortranFormat m_format;
};

class FortranCodeParserFree : public FortranCodeParser
{
  public:
    FortranCodeParserFree() : FortranCodeParser(FortranFormat_Free) { }
};

class FortranCodeParserFixed : public FortranCodeParser
{
  public:
    FortranCodeParserFixed() : FortranCodeParser(FortranFormat_Fixed) { }
};

#endif
