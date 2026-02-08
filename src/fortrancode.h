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

class OutputCodeList;
class FileDef;
class MemberDef;
class QCString;
class Definition;

class FortranCodeParser : public CodeParserInterface
{
  public:
    FortranCodeParser(FortranFormat format=FortranFormat::Unknown);
   ~FortranCodeParser() override;
    NON_COPYABLE(FortranCodeParser)

    void parseCode(OutputCodeList &codeOutIntf,
                   const QCString &scopeName,
                   const QCString &input,
                   SrcLangExt lang,
                   bool stripCodeComments,
                   const CodeParserOptions &options
                  ) override;
    void resetCodeParserState() override;

  private:
    struct Private;
    std::unique_ptr<Private> p;
};

class FortranCodeParserFree : public FortranCodeParser
{
  public:
    FortranCodeParserFree() : FortranCodeParser(FortranFormat::Free) { }
};

class FortranCodeParserFixed : public FortranCodeParser
{
  public:
    FortranCodeParserFixed() : FortranCodeParser(FortranFormat::Fixed) { }
};

#endif
