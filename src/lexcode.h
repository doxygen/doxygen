/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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


#ifndef LEXCODE_H
#define LEXCODE_H

#include "parserintf.h"

/** LEX code scanner.
 */
class LexCodeParser final : public CodeParserInterface
{
  public:
    LexCodeParser();
   ~LexCodeParser() override;
    NON_COPYABLE(LexCodeParser)

    void parseCode(OutputCodeList &codeOutIntf,
                   const DString &scopeName,
                   const DString &input,
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
