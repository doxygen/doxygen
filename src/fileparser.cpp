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
               const CodeParserOptions &options
              )
{
  int lineNr = options.startLine()!=-1 ? options.startLine() : 1;
  size_t length = input.length();
  size_t i=0;
  while (i<length && (options.endLine()==-1 || lineNr<=options.endLine()))
  {
    size_t j=i;
    while (j<length && input[j]!='\n') j++;
    QCString lineStr = input.mid(i,j-i);
    codeOutIntf.startCodeLine(lineNr);
    if (options.fileDef() && options.showLineNumbers())
    {
      codeOutIntf.writeLineNumber(QCString(),QCString(),QCString(),lineNr,!options.inlineFragment());
    }
    if (!lineStr.isEmpty()) codeOutIntf.codify(lineStr.data());
    codeOutIntf.endCodeLine();
    lineNr++;
    i=j+1;
  }
}

