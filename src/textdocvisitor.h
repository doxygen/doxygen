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
#include "docnode.h"
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
    void operator()(const DocCite &);
    void operator()(const DocSeparator &)    { m_t << " "; }

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------

    template<class T>
    void visitChildren(const T &t)
    {
      for (const auto &child : t.children())
      {
        std::visit(*this, child);
      }
    }
    void operator()(const DocAutoList &l)        { visitChildren(l);  }
    void operator()(const DocAutoListItem &li)   { visitChildren(li); }
    void operator()(const DocPara &p)            { visitChildren(p);  }
    void operator()(const DocRoot &r)            { visitChildren(r);  }
    void operator()(const DocSimpleSect &s)      { visitChildren(s);  }
    void operator()(const DocTitle &t)           { visitChildren(t);  }
    void operator()(const DocSimpleList &l)      { visitChildren(l);  }
    void operator()(const DocSimpleListItem &)   {                    }
    void operator()(const DocSection &s)         { visitChildren(s);  }
    void operator()(const DocHtmlList &l)        { visitChildren(l);  }
    void operator()(const DocHtmlListItem &li)   { visitChildren(li); }
    void operator()(const DocHtmlDescList &dl)   { visitChildren(dl); }
    void operator()(const DocHtmlDescTitle &dt)  { visitChildren(dt); }
    void operator()(const DocHtmlDescData &dd)   { visitChildren(dd); }
    void operator()(const DocHtmlTable &t)       { visitChildren(t);  }
    void operator()(const DocHtmlRow &r)         { visitChildren(r);  }
    void operator()(const DocHtmlCell &c)        { visitChildren(c);  }
    void operator()(const DocHtmlCaption &c)     { visitChildren(c);  }
    void operator()(const DocInternal &i)        { visitChildren(i);  }
    void operator()(const DocHRef &h)            { visitChildren(h);  }
    void operator()(const DocHtmlSummary &s)     { visitChildren(s);  }
    void operator()(const DocHtmlDetails &d)     { visitChildren(d);  }
    void operator()(const DocHtmlHeader &h)      { visitChildren(h);  }
    void operator()(const DocImage &i)           { visitChildren(i);  }
    void operator()(const DocDotFile &df)        { visitChildren(df); }
    void operator()(const DocMscFile &df)        { visitChildren(df); }
    void operator()(const DocDiaFile &df)        { visitChildren(df); }
    void operator()(const DocPlantUmlFile &df)   { visitChildren(df); }
    void operator()(const DocLink &l)            { visitChildren(l);  }
    void operator()(const DocRef &r)             { visitChildren(r);  }
    void operator()(const DocSecRefItem &s)      { visitChildren(s);  }
    void operator()(const DocSecRefList &l)      { visitChildren(l);  }
    void operator()(const DocParamSect &s)       { visitChildren(s);  }
    void operator()(const DocParamList &)        {                    }
    void operator()(const DocXRefItem &x)        { visitChildren(x);  }
    void operator()(const DocInternalRef &r)     { visitChildren(r);  }
    void operator()(const DocText &t)            { visitChildren(t);  }
    void operator()(const DocHtmlBlockQuote &q)  { visitChildren(q);  }
    void operator()(const DocVhdlFlow &)         {                    }
    void operator()(const DocParBlock &pb)       { visitChildren(pb); }

  private:

    void filter(const QCString &str);

    TextStream &m_t;
};

#endif
