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

class XMLCodeGenerator
{
  public:
    XMLCodeGenerator(TextStream *t);
   ~XMLCodeGenerator();

    OutputType type() const { return OutputType::XML; }

    void codify(const QCString &text);
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip);
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                      const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                     );
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const QCString &colorClass);
    void endFontClass();
    void writeCodeAnchor(const QCString &);
    void writeLineNumber(const QCString &extRef,const QCString &compId,
                         const QCString &anchorId,int l,bool writeLineAnchor);
    void startCodeFragment(const QCString &);
    void endCodeFragment(const QCString &);

    void finish();

  private:
    TextStream *m_t;
    QCString m_refId;
    QCString m_external;
    int m_lineNumber;
    bool m_isMemberRef;
    int m_col;

    bool m_insideCodeLine;
    bool m_normalHLNeedStartTag;
    bool m_insideSpecialHL;
};

void generateXML();

#endif
