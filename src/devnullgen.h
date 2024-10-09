
/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef DEVNULLGEN_H
#define DEVNULLGEN_H

#include "outputgen.h"

/** Class implementing OutputCodeIntf by throwing away everything. */
class DevNullCodeGenerator : public OutputCodeIntf
{
  public:
    OutputType type() const override { return OutputType::Null; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<DevNullCodeGenerator>(*this); }
    void codify(const QCString &) override {}
    void stripCodeComments(bool) override {}
    void startSpecialComment() override {}
    void endSpecialComment() override {}
    void writeCodeLink(CodeSymbolType,
                               const QCString &,const QCString &,
                               const QCString &,const QCString &,
                               const QCString &) override {}
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                              const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                             ) override {}
    void writeLineNumber(const QCString &,const QCString &,
                                 const QCString &,int,bool) override {}
    void startCodeLine(int) override {}
    void endCodeLine() override {}
    void startFontClass(const QCString &) override {}
    void endFontClass() override {}
    void writeCodeAnchor(const QCString &) override {}
    void startCodeFragment(const QCString &) override {}
    void endCodeFragment(const QCString &) override {}
    void startFold(int,const QCString &,const QCString &) override {}
    void endFold() override {}
};

#endif // DEVNULLGEN_H
