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
* Documents produced by Doxygen are derivative works derived from the
* input used in their production; they are not affected by this license.
*
*/

#ifndef DOCBOOKDOCVISITOR_H
#define DOCBOOKDOCVISITOR_H

#include <iostream>

#include "containers.h"
#include "docvisitor.h"
#include "docnode.h"
#include "qcstring.h"

class OutputCodeList;
class QCString;
class TextStream;

/*! @brief Concrete visitor implementation for Docbook output. */
class DocbookDocVisitor : public DocVisitor
{
  public:
    DocbookDocVisitor(TextStream &t,OutputCodeList &ci,const QCString &langExt);
    //-----------------------------------------
    template<class T>
    void visitChildren(const T &t)
    {
      for (const auto &child : t.children())
      {
        std::visit(*this, child);
      }
    }
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
    //--------------------------------------
    // helper functions
    //--------------------------------------
    void filter(const QCString &str, const bool retainNewLine = false);
    void startLink(const QCString &file,
    const QCString &anchor);
    void endLink();
    void startMscFile(const QCString &fileName,const QCString &relPath, const QCString &width,
                      const QCString &height, bool hasCaption,const DocNodeList &children,
                      const QCString &srcFile, int srcLine);
    void endMscFile(bool hasCaption);
    void writeMscFile(const QCString &fileName, const DocVerbatim &s);
    void startDiaFile(const QCString &fileName,const QCString &relPath, const QCString &width,
                      const QCString &height, bool hasCaption,const DocNodeList &children,
                      const QCString &srcFile, int srcLine);
    void endDiaFile(bool hasCaption);
    void writeDiaFile(const QCString &fileName, const DocVerbatim &s);
    void startDotFile(const QCString &fileName,const QCString &relPath, const QCString &width,
                      const QCString &height, bool hasCaption,const DocNodeList &children,
                      const QCString &srcFile, int srcLine);
    void endDotFile(bool hasCaption);
    void writeDotFile(const QCString &fileName, const DocVerbatim &s);
    void writePlantUMLFile(const QCString &fileName, const DocVerbatim &s);
    void startPlantUmlFile(const QCString &fileName,const QCString &relPath, const QCString &width,
                      const QCString &height, bool hasCaption,const DocNodeList &children,
                      const QCString &srcFile, int srcLine);
    void endPlantUmlFile(bool hasCaption);
    void visitPreStart(TextStream &t,
                   const DocNodeList &children,
                   bool hasCaption,
                   const QCString &name,
                   const QCString &width,
                   const QCString &height,
                   bool inlineImage = FALSE);
    void visitPostEnd(TextStream &t, bool hasCaption, bool inlineImage = FALSE);
    void visitCaption(const DocNodeList &children);
    //--------------------------------------
    // state variables
    //--------------------------------------
    TextStream &m_t;
    OutputCodeList &m_ci;
    bool m_insidePre = false;
    bool m_hide = false;
    BoolStack m_enabled;
    QCString m_langExt;
    int m_colCnt = 0;
    BoolStack m_bodySet; // it is possible to have tables without a header, needs to be an array as we can have tables in tables
};

#endif
