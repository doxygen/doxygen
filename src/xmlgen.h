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

class XMLCodeGenerator : public CodeOutputInterface
{
  public:

    XMLCodeGenerator(FTextStream &t) : m_t(t), m_lineNumber(-1), m_isMemberRef(FALSE), m_col(0),
      m_insideCodeLine(FALSE), m_normalHLNeedStartTag(TRUE), m_insideSpecialHL(FALSE) {}
    virtual ~XMLCodeGenerator() { }

    void codify(const char *text);
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip);
    void writeTooltip(const char *, const DocLinkInfo &, const char *,
                      const char *, const SourceLinkInfo &, const SourceLinkInfo &
                     );
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const char *colorClass);
    void endFontClass();
    void writeCodeAnchor(const char *);
    void writeLineNumber(const char *extRef,const char *compId,
                         const char *anchorId,int l);
    void setCurrentDoc(const Definition *,const char *,bool){}
    void addWord(const char *,bool){}
    void startCodeFragment(const char *);
    void endCodeFragment();

    void finish();

  private:
    FTextStream &m_t;
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
