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
#include "docnode.h"

class OutputCodeList;
class LatexCodeGenerator;
class TextStream;

enum class TexOrPdf
{
   NO,  //!< not called through texorpdf
   TEX, //!< called through texorpdf as TeX (first) part
   PDF, //!< called through texorpdf as PDF (second) part
};

/*! @brief Concrete visitor implementation for LaTeX output. */
class LatexDocVisitor : public DocVisitor
{
  public:
    LatexDocVisitor(TextStream &t,OutputCodeList &ci,LatexCodeGenerator &lcg,
                    const QCString &langExt, int hierarchyLevel = 0);

    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------

    void operator()(const DocWord &);
    void operator()(const DocLinkedWord &);
    void operator()(const DocWhiteSpace &);
    void operator()(const DocSymbol &);
    void operator()(const DocEmoji &);
    void operator()(const DocURL &);
    void operator()(const DocLineBreak &);
    void operator()(const DocHorRuler &);
    void operator()(const DocStyleChange &);
    void operator()(const DocVerbatim &);
    void operator()(const DocAnchor &);
    void operator()(const DocInclude &);
    void operator()(const DocIncOperator &);
    void operator()(const DocFormula &);
    void operator()(const DocIndexEntry &);
    void operator()(const DocSimpleSectSep &);
    void operator()(const DocCite &);
    void operator()(const DocSeparator &);

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------

    void operator()(const DocAutoList &);
    void operator()(const DocAutoListItem &);
    void operator()(const DocPara &);
    void operator()(const DocRoot &);
    void operator()(const DocSimpleSect &);
    void operator()(const DocTitle &);
    void operator()(const DocSimpleList &);
    void operator()(const DocSimpleListItem &);
    void operator()(const DocSection &s);
    void operator()(const DocHtmlList &s);
    void operator()(const DocHtmlListItem &);
    void operator()(const DocHtmlDescList &);
    void operator()(const DocHtmlDescTitle &);
    void operator()(const DocHtmlDescData &);
    void operator()(const DocHtmlTable &t);
    void operator()(const DocHtmlCaption &);
    void operator()(const DocHtmlRow &);
    void operator()(const DocHtmlCell &);
    void operator()(const DocInternal &);
    void operator()(const DocHRef &);
    void operator()(const DocHtmlSummary &);
    void operator()(const DocHtmlDetails &);
    void operator()(const DocHtmlHeader &);
    void operator()(const DocImage &);
    void operator()(const DocDotFile &);
    void operator()(const DocMscFile &);
    void operator()(const DocDiaFile &);
    void operator()(const DocPlantUmlFile &);
    void operator()(const DocLink &lnk);
    void operator()(const DocRef &ref);
    void operator()(const DocSecRefItem &);
    void operator()(const DocSecRefList &);
    void operator()(const DocParamSect &);
    void operator()(const DocParamList &);
    void operator()(const DocXRefItem &);
    void operator()(const DocInternalRef &);
    void operator()(const DocText &);
    void operator()(const DocHtmlBlockQuote &);
    void operator()(const DocVhdlFlow &);
    void operator()(const DocParBlock &);

  private:
    template<class T>
    void visitChildren(const T &t)
    {
      for (const auto &child : t.children())
      {
        std::visit(*this, child);
      }
    }

    struct ActiveRowSpan
    {
      ActiveRowSpan(const DocHtmlCell &c,size_t rs,size_t cs,size_t col)
        : cell(c), rowSpan(rs), colSpan(cs), column(col) {}
      const DocHtmlCell &cell;
      size_t rowSpan;
      size_t colSpan;
      size_t column;
    };

    typedef std::vector<ActiveRowSpan> RowSpanList;

    //--------------------------------------
    // helper functions
    //--------------------------------------

    void filter(const QCString &str, const bool retainNewLine = false);
    void startLink(const QCString &ref,const QCString &file,
                   const QCString &anchor,bool refToTable=false,bool refToSection=false);
    void endLink(const QCString &ref,const QCString &file,
                 const QCString &anchor,bool refToTable=false,bool refToSection=false, SectionType sectionType = SectionType::Anchor);
    QCString escapeMakeIndexChars(const char *s);
    void startDotFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endDotFile(bool hasCaption);

    void startMscFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endMscFile(bool hasCaption);
    void writeMscFile(const QCString &fileName, const DocVerbatim &s);

    void startDiaFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endDiaFile(bool hasCaption);
    void writeDiaFile(const QCString &fileName, const DocVerbatim &s);
    void writePlantUMLFile(const QCString &fileName, const DocVerbatim &s);
    void startPlantUmlFile(const QCString &fileName,const QCString &width,
                      const QCString &height, bool hasCaption,
                      const QCString &srcFile,int srcLine);
    void endPlantUmlFile(bool hasCaption);

    void visitCaption(const DocNodeList &children);

    void incIndentLevel();
    void decIndentLevel();
    int indentLevel() const;
    const char *getSectionName(int level) const;

    //--------------------------------------
    // state variables
    //--------------------------------------

    TextStream &m_t;
    OutputCodeList &m_ci;
    LatexCodeGenerator &m_lcg;
    bool m_insidePre;
    bool m_insideItem;
    bool m_hide;
    QCString m_langExt;
    int m_hierarchyLevel;
    TexOrPdf m_texOrPdf = TexOrPdf::NO;

    struct TableState
    {
      RowSpanList rowSpans;
      size_t numCols = 0;
      size_t currentColumn = 0;
      bool inRowSpan = false;
      bool inColSpan = false;
      bool firstRow = false;
    };
    std::stack<TableState> m_tableStateStack; // needed for nested tables
    RowSpanList m_emptyRowSpanList;

    static const int maxIndentLevels = 13;
    int m_indentLevel = 0;

    struct LatexListItemInfo
    {
      bool isEnum = false;
    };

    LatexListItemInfo m_listItemInfo[maxIndentLevels];

    void pushTableState()
    {
      m_tableStateStack.emplace();
    }
    void popTableState()
    {
      m_tableStateStack.pop();
    }
    size_t currentColumn() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().currentColumn : 0;
    }
    void setCurrentColumn(size_t col)
    {
      if (!m_tableStateStack.empty()) m_tableStateStack.top().currentColumn = col;
    }
    size_t numCols() const
    {
      return !m_tableStateStack.empty() ? m_tableStateStack.top().numCols : 0;
    }
    void setNumCols(size_t num)
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
