/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef DEVNULLGEN_H
#define DEVNULLGEN_H

#include "outputgen.h"

/** Class implementing OutputCodeIntf by throwing away everything. */
class DevNullCodeGenerator final : public OutputCodeIntf
{
  public:
    OutputType type() const override { return OutputType::Null; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<DevNullCodeGenerator>(*this); }
    void codify(const DString &) override {}
    void stripCodeComments(bool) override {}
    void startSpecialComment() override {}
    void endSpecialComment() override {}
    void setStripIndentAmount(size_t) override {}
    void writeCodeLink(CodeSymbolType,
                               const DString &,const DString &,
                               const DString &,const DString &,
                               const DString &) override {}
    void writeTooltip(const DString &, const DocLinkInfo &, const DString &,
                              const DString &, const SourceLinkInfo &, const SourceLinkInfo &
                             ) override {}
    void writeLineNumber(const DString &,const DString &,
                                 const DString &,int,bool) override {}
    void startCodeLine(int) override {}
    void endCodeLine() override {}
    void startFontClass(const DString &) override {}
    void endFontClass() override {}
    void writeCodeAnchor(const DString &) override {}
    void startCodeFragment(const DString &) override {}
    void endCodeFragment(const DString &) override {}
    void startFold(int,const DString &,const DString &) override {}
    void endFold() override {}
};

#endif // DEVNULLGEN_H
