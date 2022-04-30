/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#include "fileparser.h"
#include "outputgen.h"

void FileCodeParser::parseCode(CodeOutputInterface &codeOutIntf,
               const QCString &,    // scopeName
               const QCString &     input,
               SrcLangExt,          // lang
               bool,                // isExampleBlock
               const QCString &,    // exampleName
               const FileDef *      fileDef,
               int                  startLine,
               int                  endLine,
               bool                 inlineFragment,
               const MemberDef *,   // memberDef
               bool                 showLineNumbers,
               const Definition *,  // searchCtx,
               bool                 // collectXRefs
              )
{
  int lineNr = startLine!=-1 ? startLine : 1;
  int length = input.length();
  int i=0;
  while (i<length && (endLine==-1 || lineNr<=endLine))
  {
    int j=i;
    while (j<length && input[j]!='\n') j++;
    QCString lineStr = input.mid(i,j-i);
    codeOutIntf.startCodeLine(fileDef != 0 && showLineNumbers);
    if (fileDef != 0 && showLineNumbers)
    {
      codeOutIntf.writeLineNumber(QCString(),QCString(),QCString(),lineNr,!inlineFragment);
    }
    if (!lineStr.isEmpty()) codeOutIntf.codify(lineStr.data());
    codeOutIntf.endCodeLine();
    lineNr++;
    i=j+1;
  }
}

