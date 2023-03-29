
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
class DevNullCodeGenerator
{
  public:
    OutputType type() const { return OutputType::Null; }
    void codify(const QCString &) {}
    void writeCodeLink(CodeSymbolType,
                               const QCString &,const QCString &,
                               const QCString &,const QCString &,
                               const QCString &) {}
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                              const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                             ) {}
    void writeLineNumber(const QCString &,const QCString &,
                                 const QCString &,int,bool) {}
    void startCodeLine(bool) {}
    void endCodeLine() {}
    void startFontClass(const QCString &) {}
    void endFontClass() {}
    void writeCodeAnchor(const QCString &) {}
    void startCodeFragment(const QCString &) {}
    void endCodeFragment(const QCString &) {}
};

#endif // DEVNULLGEN_H
