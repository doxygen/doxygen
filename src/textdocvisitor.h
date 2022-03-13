/******************************************************************************
 *
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

#ifndef TEXTDOCVISITOR_H
#define TEXTDOCVISITOR_H

#include "qcstring.h"
#include "docvisitor.h"
#include "docparser.h"
#include "textstream.h"

/*! @brief Concrete visitor implementation for TEXT output. */
class TextDocVisitor : public DocVisitor
{
  public:
    TextDocVisitor(TextStream &t) : m_t(t) {}

    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------

    void operator()(const DocWord &w)        { filter(w.word()); }
    void operator()(const DocLinkedWord &w)  { filter(w.word()); }
    void operator()(const DocWhiteSpace &)   { m_t << " ";       }
    void operator()(const DocSymbol &);
    void operator()(const DocEmoji &);
    void operator()(const DocURL &u)         { filter(u.url());  }
    void operator()(const DocLineBreak &)    { m_t << " ";       }
    void operator()(const DocHorRuler &)     {}
    void operator()(const DocStyleChange &)  {}
    void operator()(const DocVerbatim &s)    { filter(s.text()); }
    void operator()(const DocAnchor &)       {}
    void operator()(const DocInclude &)      {}
    void operator()(const DocIncOperator &)  {}
    void operator()(const DocFormula &)      {}
    void operator()(const DocIndexEntry &)   {}
    void operator()(const DocSimpleSectSep &){}
    void operator()(const DocCite &)         {}
    void operator()(const DocSeparator &)    { m_t << " "; }

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------

    void operator()(const DocAutoList &) {}
    void operator()(const DocAutoListItem &) {}
    void operator()(const DocPara &)  {}
    void operator()(const DocRoot &) {}
    void operator()(const DocSimpleSect &) {}
    void operator()(const DocTitle &) {}
    void operator()(const DocSimpleList &) {}
    void operator()(const DocSimpleListItem &) {}
    void operator()(const DocSection &) {}
    void operator()(const DocHtmlList &) {}
    void operator()(const DocHtmlListItem &) {}
    void operator()(const DocHtmlDescList &) {}
    void operator()(const DocHtmlDescTitle &) {}
    void operator()(const DocHtmlDescData &) {}
    void operator()(const DocHtmlTable &) {}
    void operator()(const DocHtmlRow &) {}
    void operator()(const DocHtmlCell &) {}
    void operator()(const DocHtmlCaption &) {}
    void operator()(const DocInternal &) {}
    void operator()(const DocHRef &) {}
    void operator()(const DocHtmlHeader &) {}
    void operator()(const DocImage &) {}
    void operator()(const DocDotFile &) {}
    void operator()(const DocMscFile &) {}
    void operator()(const DocDiaFile &) {}
    void operator()(const DocLink &) {}
    void operator()(const DocRef &) {}
    void operator()(const DocSecRefItem &) {}
    void operator()(const DocSecRefList &) {}
    void operator()(const DocParamSect &) {}
    void operator()(const DocParamList &) {}
    void operator()(const DocXRefItem &) {}
    void operator()(const DocInternalRef &) {}
    void operator()(const DocText &) {}
    void operator()(const DocHtmlBlockQuote &) {}
    void operator()(const DocVhdlFlow &) {}
    void operator()(const DocParBlock &) {}

  private:

    void filter(const QCString &str);

    TextStream &m_t;
};

#endif
