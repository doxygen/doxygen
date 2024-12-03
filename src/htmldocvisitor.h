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

#ifndef HTMLDOCVISITOR_H
#define HTMLDOCVISITOR_H

#include "docvisitor.h"
#include "docnode.h"
#include "qcstring.h"

class Definition;
class MemberDef;
class OutputCodeList;
class TextStream;

/*! @brief Concrete visitor implementation for HTML output. */
class HtmlDocVisitor : public DocVisitor
{
  public:
    HtmlDocVisitor(TextStream &t,OutputCodeList &ci,const Definition *ctx,const QCString &fn = QCString());

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
    void operator()(const DocPara &) ;
    void operator()(const DocRoot &);
    void operator()(const DocSimpleSect &);
    void operator()(const DocTitle &);
    void operator()(const DocSimpleList &);
    void operator()(const DocSimpleListItem &);
    void operator()(const DocSection &);
    void operator()(const DocHtmlList &);
    void operator()(const DocHtmlListItem &);
    void operator()(const DocHtmlDescList &);
    void operator()(const DocHtmlDescTitle &);
    void operator()(const DocHtmlDescData &);
    void operator()(const DocHtmlTable &);
    void operator()(const DocHtmlRow &);
    void operator()(const DocHtmlCell &);
    void operator()(const DocHtmlCaption &);
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
    void operator()(const DocLink &);
    void operator()(const DocRef &);
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
    template<class T>
    void visitCaption(TextStream &t,const T &n);

    //--------------------------------------
    // helper functions
    //--------------------------------------

    void writeObfuscatedMailAddress(const QCString &url);
    void filter(const QCString &str, const bool retainNewline = false);
    QCString filterQuotedCdataAttr(const QCString &str);
    void startLink(const QCString &ref,const QCString &file,
                   const QCString &relPath,const QCString &anchor,
                   const QCString &tooltip = "");
    void endLink();
    void writeDotFile(const QCString &fileName,const QCString &relPath,const QCString &context,
                      const QCString &srcFile,int srcLine);
    void writeMscFile(const QCString &fileName,const QCString &relPath,const QCString &context,
                      const QCString &srcFile,int srcLine);
    void writeDiaFile(const QCString &fileName,const QCString &relPath,const QCString &context,
                      const QCString &srcFile,int srcLine);
    void writePlantUMLFile(const QCString &fileName,const QCString &relPath,const QCString &context,
                           const QCString &srcFile,int srcLine);

    template<class DocNode>
    void forceEndParagraph(const DocNode &n);
    template<class DocNode>
    void forceStartParagraph(const DocNode &n);

    //--------------------------------------
    // state variables
    //--------------------------------------

    TextStream &m_t;
    OutputCodeList &m_ci;
    bool m_insidePre = false;
    bool m_hide = false;
    bool m_insideTitle = false;
    const Definition *m_ctx;
    QCString m_fileName;
    QCString m_langExt;
};

#endif
