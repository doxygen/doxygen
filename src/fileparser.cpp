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

#include "fileparser.h"
#include "outputgen.h"
#include "outputlist.h"

void FileCodeParser::parseCode(OutputCodeList &codeOutIntf,
               const QCString &,    // scopeName
               const QCString &     input,
               SrcLangExt,          // lang
               bool,                // stripCodeComments
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
  size_t length = input.length();
  size_t i=0;
  while (i<length && (endLine==-1 || lineNr<=endLine))
  {
    size_t j=i;
    while (j<length && input[j]!='\n') j++;
    QCString lineStr = input.mid(i,j-i);
    codeOutIntf.startCodeLine(lineNr);
    if (fileDef != nullptr && showLineNumbers)
    {
      codeOutIntf.writeLineNumber(QCString(),QCString(),QCString(),lineNr,!inlineFragment);
    }
    if (!lineStr.isEmpty()) codeOutIntf.codify(lineStr.data());
    codeOutIntf.endCodeLine();
    lineNr++;
    i=j+1;
  }
}

