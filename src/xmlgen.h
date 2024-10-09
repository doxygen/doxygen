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
 */

#ifndef XMLGEN_H
#define XMLGEN_H

#include "outputgen.h"

class TextStream;

class XMLCodeGenerator : public OutputCodeIntf
{
  public:
    XMLCodeGenerator(TextStream *t);

    OutputType type() const override { return OutputType::XML; }

    void codify(const QCString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<XMLCodeGenerator>(*this); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) override;
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                      const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                     ) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const QCString &colorClass) override;
    void endFontClass() override;
    void writeCodeAnchor(const QCString &) override;
    void writeLineNumber(const QCString &extRef,const QCString &compId,
                         const QCString &anchorId,int l,bool writeLineAnchor) override;
    void startCodeFragment(const QCString &) override;
    void endCodeFragment(const QCString &) override;
    void startFold(int,const QCString &,const QCString &) override {}
    void endFold() override {}

    void finish();

  private:
    TextStream *m_t;
    QCString m_refId;
    QCString m_external;
    int m_lineNumber = -1;
    bool m_isMemberRef = false;
    size_t m_col = 0;

    bool m_insideCodeLine = false;
    bool m_normalHLNeedStartTag = true;
    bool m_insideSpecialHL = false;
    bool m_stripCodeComments = false;
    bool m_hide = false;
};

void generateXML();

#endif
