/******************************************************************************
 *
 *
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

#ifndef SCANNER_LEX_H
#define SCANNER_LEX_H

#include "parserintf.h"

/** \brief Lex language parser using state-based lexical scanning.
 *
 *  This is the Lex language parser for doxygen.
 */
class LexOutlineParser : public OutlineParserInterface
{
  public:
    LexOutlineParser();
   ~LexOutlineParser() override;
    NON_COPYABLE(LexOutlineParser)
    void parseInput(const QCString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser) override;
    bool needsPreprocessing(const QCString &/* extension */) const override { return TRUE; }
    void parsePrototype(const QCString &/* text */) override {}

  private:
    struct Private;
    std::unique_ptr<Private> p;
};
#endif
