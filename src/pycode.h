/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada)
 *  in Spring 2005 as part of CS 179E: Compiler Design Project
 *  at the University of California, Riverside; the course was
 *  taught by Peter H. Froehlich <phf@acm.org>.
 */


#ifndef PYCODE_H
#define PYCODE_H

#include "parserintf.h"

class FileDef;
class MemberDef;
class QCString;
class Definition;

class PythonCodeParser : public CodeParserInterface
{
  public:
    PythonCodeParser();
    virtual ~PythonCodeParser();
    void parseCode(CodeOutputInterface &codeOutIntf,
                   const QCString &scopeName,
                   const QCString &input,
                   SrcLangExt lang,
                   bool isExampleBlock,
                   const QCString &exampleName=QCString(),
                   const FileDef *fileDef=0,
                   int startLine=-1,
                   int endLine=-1,
                   bool inlineFragment=FALSE,
                   const MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE,
                   const Definition *searchCtx=0,
                   bool collectXrefs=TRUE
                  );
    void resetCodeParserState();
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


#endif
