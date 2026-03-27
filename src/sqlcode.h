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


#ifndef SQLCODE_H
#define SQLCODE_H

#include "parserintf.h"

class FileDef;
class MemberDef;
class QCString;
class Definition;

/** SQL scanner. Only support syntax highlighting of code at the moment.
 */
class SQLCodeParser : public CodeParserInterface
{
  public:
    SQLCodeParser();
   ~SQLCodeParser() override;
    NON_COPYABLE(SQLCodeParser)

    void parseCode(OutputCodeList &codeOutIntf,
                   const QCString &scopeName,
                   const QCString &input,
                   SrcLangExt,
                   bool stripCodeComments,
                   const CodeParserOptions &options
                  ) override;
    void resetCodeParserState() override;
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


#endif
