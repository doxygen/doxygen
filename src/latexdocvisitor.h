/******************************************************************************
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

#ifndef LATEXDOCVISITOR_H
#define LATEXDOCVISITOR_H

#include <stack>

#include "qcstring.h"
#include "docvisitor.h"

class LatexCodeGenerator;
class TextStream;

/*! @brief Concrete visitor implementation for LaTeX output. */
class LatexDocVisitor : public DocVisitor
{
  public:
    LatexDocVisitor(TextStream &t,LatexCodeGenerator &ci,
                    const QCString &langExt,bool insideTabbing);

    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------

    void visit(DocWord *);
    void visit(DocLinkedWord *);
    void visit(DocWhiteSpace *);
    void visit(DocSymbol *);
    void visit(DocEmoji *);
    void visit(DocURL *);
    void visit(DocLineBreak *);
    void visit(DocHorRuler *);
    void visit(DocStyleChange *);
    void visit(DocVerbatim *);
    void visit(DocAnchor *);
    void visit(DocInclude *);
    void visit(DocIncOperator *);
    void visit(DocFormula *);
    void visit(DocIndexEntry *);
    void visit(DocSimpleSectSep *);
    void visit(DocCite *);

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------

    void visitPre(DocAutoList *);
    void visitPost(DocAutoList *);
    void visitPre(DocAutoListItem *);
    void visitPost(DocAutoListItem *);
    void visitPre(DocPara *);
    void visitPost(DocPara *);
    void visitPre(DocRoot *);
    void visitPost(DocRoot *);
    void visitPre(DocSimpleSect *);
    void visitPost(DocSimpleSect *);
    void visitPre(DocTitle *);
    void visitPost(DocTitle *);
    void visitPre(DocSimpleList *);
    void visitPost(DocSimpleList *);
    void visitPre(DocSimpleListItem *);
    void visitPost(DocSimpleListItem *);
    void visitPre(DocSection *s);
    void visitPost(DocSection *);
    void visitPre(DocHtmlList *s);
    void visitPost(DocHtmlList *s);
    void visitPre(DocHtmlListItem *);
    void visitPost(DocHtmlListItem *);
    //void visitPre(DocHtmlPre *);
    //void visitPost(DocHtmlPre *);
    void visitPre(DocHtmlDescList *);
    void visitPost(DocHtmlDescList *);
    void visitPre(DocHtmlDescTitle *);
    void visitPost(DocHtmlDescTitle *);
    void visitPre(DocHtmlDescData *);
    void visitPost(DocHtmlDescData *);
    void visitPre(DocHtmlTable *t);
    void visitPost(DocHtmlTable *t);
    void visitPre(DocHtmlCaption *);
    void visitPost(DocHtmlCaption *);
    void visitPre(DocHtmlRow *);
    void visitPost(DocHtmlRow *) ;
    void visitPre(DocHtmlCell *);
    void visitPost(DocHtmlCell *);
    void visitPre(DocInternal *);
    void visitPost(DocInternal *);
    void visitPre(DocHRef *);
    void visitPost(DocHRef *);
    void visitPre(DocHtmlHeader *);
    void visitPost(DocHtmlHeader *) ;
    void visitPre(DocImage *);
    void visitPost(DocImage *);
    void visitPre(DocDotFile *);
    void visitPost(DocDotFile *);
    void visitPre(DocMscFile *);
    void visitPost(DocMscFile *);
    void visitPre(DocDiaFile *);
    void visitPost(DocDiaFile *);
    void visitPre(DocLink *lnk);
    void visitPost(DocLink *);
    void visitPre(DocRef *ref);
    void visitPost(DocRef *);
    void visitPre(DocSecRefItem *);
    void visitPost(DocSecRefItem *);
    void visitPre(DocSecRefList *);
    void visitPost(DocSecRefList *);
    void visitPre(DocParamSect *);
    void visitPost(DocParamSect *);
    void visitPre(DocParamList *);
    void visitPost(DocParamList *);
    void visitPre(DocXRefItem *);
    void visitPost(DocXRefItem *);
    void visitPre(DocInternalRef *);
    void visitPost(DocInternalRef *);
    void visitPre(DocText *);
    void visitPost(DocText *);
    void visitPre(DocHtmlBlockQuote *);
    void visitPost(DocHtmlBlockQuote *);
    void visitPre(DocVhdlFlow *);
    void visitPost(DocVhdlFlow *);
    void visitPre(DocParBlock *);
    void visitPost(DocParBlock *);

  private:

    struct ActiveRowSpan
    {
      ActiveRowSpan(DocHtmlCell *c,int rs,int cs,int col)
        : cell(c), rowSpan(rs), colSpan(cs), column(col) {}
      DocHtmlCell *cell;
      int rowSpan;
      int colSpan;
      int column;
    };

    typedef std::vector<ActiveRowSpan> RowSpanList;

    //--------------------------------------
    // helper functions
    //--------------------------------------

    void filter(const QCString &str);
    void startLink(const QCString &ref,const QCString &file,
                   const QCString &anchor,bool refToTable=FALSE);
    void endLink(const QCString &ref,const QCString &file,
                 const QCString &anchor,bool refToTable=FALSE);
    QCString escapeMakeIndexChars(const char *s);
    void startDotFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endDotFile(bool hasCaption);

    void startMscFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endMscFile(bool hasCaption);
    void writeMscFile(const QCString &fileName, DocVerbatim *s);

    void startDiaFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endDiaFile(bool hasCaption);
    void writeDiaFile(const QCString &fileName, DocVerbatim *s);
    void writePlantUMLFile(const QCString &fileName, DocVerbatim *s);

    //--------------------------------------
    // state variables
    //--------------------------------------

    TextStream &m_t;
    LatexCodeGenerator &m_ci;
    bool m_insidePre;
    bool m_insideItem;
    bool m_hide;
    bool m_hideCaption;
    bool m_insideTabbing;
    QCString m_langExt;

    struct TableState
    {
      RowSpanList rowSpans;
      int  numCols = 0;
      int currentColumn = 0;
      bool inRowSpan = false;
      bool inColSpan = false;
      bool firstRow = false;
    };
    std::stack<TableState> m_tableStateStack; // needed for nested tables
    RowSpanList m_emptyRowSpanList;

    void pushTableState()
    {
      m_tableStateStack.push(TableState());
    }
    void popTableState()
    {
      m_tableStateStack.pop();
    }
    int currentColumn() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().currentColumn : 0;
    }
    void setCurrentColumn(int col)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().currentColumn = col;
    }
    int numCols() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().numCols : 0;
    }
    void setNumCols(int num)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().numCols = num;
    }
    bool inRowSpan() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().inRowSpan : FALSE;
    }
    void setInRowSpan(bool b)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().inRowSpan = b;
    }
    bool inColSpan() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().inColSpan : FALSE;
    }
    void setInColSpan(bool b)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().inColSpan = b;
    }
    bool firstRow() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().firstRow : FALSE;
    }
    void setFirstRow(bool b)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().firstRow = b;
    }
    RowSpanList &rowSpans()
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().rowSpans : m_emptyRowSpanList;
    }
    void addRowSpan(ActiveRowSpan &&span)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().rowSpans.push_back(std::move(span));
    }
    bool insideTable() const
    {
      return !m_tableStateStack.empty();
    }

};

#endif
