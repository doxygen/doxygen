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

#ifndef _TEXTDOCVISITOR_H
#define _TEXTDOCVISITOR_H

#include "docvisitor.h"
#include "docparser.h"
#include <qstack.h>
#include <qcstring.h>
#include "ftextstream.h"


/*! @brief Concrete visitor implementation for TEXT output. */
class TextDocVisitor : public DocVisitor
{
  public:
    TextDocVisitor(FTextStream &t) : DocVisitor(DocVisitor_Text), m_t(t) {}
    
    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------
    
    void visit(DocWord *w)        { filter(w->word()); }
    void visit(DocLinkedWord *w)  { filter(w->word()); }
    void visit(DocWhiteSpace *)   { m_t << " ";       }
    void visit(DocSymbol *);
    void visit(DocURL *u)         { filter(u->url());  }
    void visit(DocLineBreak *)    { m_t << " ";       }
    void visit(DocHorRuler *)     {}
    void visit(DocStyleChange *)  {}
    void visit(DocVerbatim *s)    { filter(s->text()); }
    void visit(DocAnchor *)       {}
    void visit(DocInclude *)      {}
    void visit(DocIncOperator *)  {}
    void visit(DocFormula *)      {}
    void visit(DocIndexEntry *)   {}
    void visit(DocSimpleSectSep *){}
    void visit(DocCite *)         {}

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------
    
    void visitPre(DocAutoList *) {}
    void visitPost(DocAutoList *) {}
    void visitPre(DocAutoListItem *) {}
    void visitPost(DocAutoListItem *) {}
    void visitPre(DocPara *)  {}
    void visitPost(DocPara *) {}
    void visitPre(DocRoot *) {}
    void visitPost(DocRoot *) {}
    void visitPre(DocSimpleSect *) {}
    void visitPost(DocSimpleSect *) {}
    void visitPre(DocTitle *) {}
    void visitPost(DocTitle *) {}
    void visitPre(DocSimpleList *) {}
    void visitPost(DocSimpleList *) {}
    void visitPre(DocSimpleListItem *) {}
    void visitPost(DocSimpleListItem *) {}
    void visitPre(DocSection *) {}
    void visitPost(DocSection *) {}
    void visitPre(DocHtmlList *) {}
    void visitPost(DocHtmlList *)  {}
    void visitPre(DocHtmlListItem *) {}
    void visitPost(DocHtmlListItem *) {}
    void visitPre(DocHtmlDescList *) {}
    void visitPost(DocHtmlDescList *) {}
    void visitPre(DocHtmlDescTitle *) {}
    void visitPost(DocHtmlDescTitle *) {}
    void visitPre(DocHtmlDescData *) {}
    void visitPost(DocHtmlDescData *) {}
    void visitPre(DocHtmlTable *) {}
    void visitPost(DocHtmlTable *) {}
    void visitPre(DocHtmlRow *) {}
    void visitPost(DocHtmlRow *)  {}
    void visitPre(DocHtmlCell *) {}
    void visitPost(DocHtmlCell *) {}
    void visitPre(DocHtmlCaption *) {}
    void visitPost(DocHtmlCaption *) {}
    void visitPre(DocInternal *) {}
    void visitPost(DocInternal *) {}
    void visitPre(DocHRef *) {}
    void visitPost(DocHRef *) {}
    void visitPre(DocHtmlHeader *) {}
    void visitPost(DocHtmlHeader *) {}
    void visitPre(DocImage *) {}
    void visitPost(DocImage *) {}
    void visitPre(DocDotFile *) {}
    void visitPost(DocDotFile *) {}

    void visitPre(DocMscFile *) {}
    void visitPost(DocMscFile *) {}
    void visitPre(DocDiaFile *) {}
    void visitPost(DocDiaFile *) {}
    void visitPre(DocLink *) {}
    void visitPost(DocLink *) {}
    void visitPre(DocRef *) {}
    void visitPost(DocRef *) {}
    void visitPre(DocSecRefItem *) {}
    void visitPost(DocSecRefItem *) {}
    void visitPre(DocSecRefList *) {}
    void visitPost(DocSecRefList *) {}
    void visitPre(DocParamSect *) {}
    void visitPost(DocParamSect *) {}
    void visitPre(DocParamList *) {}
    void visitPost(DocParamList *) {}
    void visitPre(DocXRefItem *) {}
    void visitPost(DocXRefItem *) {}
    void visitPre(DocInternalRef *) {}
    void visitPost(DocInternalRef *) {}
    void visitPre(DocCopy *) {}
    void visitPost(DocCopy *) {}
    void visitPre(DocText *) {}
    void visitPost(DocText *) {}
    void visitPre(DocHtmlBlockQuote *) {}
    void visitPost(DocHtmlBlockQuote *) {}
    void visitPre(DocVhdlFlow *) {}
    void visitPost(DocVhdlFlow *) {}
    void visitPre(DocParBlock *) {}
    void visitPost(DocParBlock *) {}

  private:

   void filter(const char *str);

    FTextStream &m_t;
};

#endif
