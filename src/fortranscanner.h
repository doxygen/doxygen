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

#ifndef SCANNER_FORTRAN_H
#define SCANNER_FORTRAN_H

#include "parserintf.h"

/** \brief Fortran language parser using state-based lexical scanning.
 *
 *  This is the Fortran language parser for doxygen.
 */
class FortranOutlineParser : public OutlineParserInterface
{
  public:
    FortranOutlineParser(FortranFormat format=FortranFormat::Unknown);
   ~FortranOutlineParser() override;
    NON_COPYABLE(FortranOutlineParser)
    void parseInput(const QCString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser) override;
    bool needsPreprocessing(const QCString &extension) const override;
    void parsePrototype(const QCString &text) override;

  private:
    struct Private;
    std::unique_ptr<Private> p;
};

class FortranOutlineParserFree : public FortranOutlineParser
{
  public:
    FortranOutlineParserFree() : FortranOutlineParser(FortranFormat::Free) { }
};

class FortranOutlineParserFixed : public FortranOutlineParser
{
  public:
    FortranOutlineParserFixed() : FortranOutlineParser(FortranFormat::Fixed) { }
};

const char* prepassFixedForm(const char* contents, int *hasContLine, int fixedCommentAfter);

#endif
