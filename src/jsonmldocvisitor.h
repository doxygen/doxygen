/******************************************************************************
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

#ifndef _JSONMLDOCVISITOR_H
#define _JSONMLDOCVISITOR_H

#include "qcstring.h"
#include "docvisitor.h"
#include "docnode.h"
#include "textstream.h"

class OutputCodeList;
class QCString;

/*! @brief Concrete visitor implementation for JsonML output. */
class JsonMLDocVisitor : public DocVisitor
{
  public:
    JsonMLDocVisitor(QCString &t);

    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------

    void operator()(const DocWord &);
    void operator()(const DocLinkedWord &) {}
    void operator()(const DocWhiteSpace &);
    void operator()(const DocSymbol &) {}
    void operator()(const DocEmoji &) {}
    void operator()(const DocURL &) {}
    void operator()(const DocLineBreak &) {}
    void operator()(const DocHorRuler &) {}
    void operator()(const DocStyleChange &);
    void operator()(const DocVerbatim &) {}
    void operator()(const DocAnchor &);
    void operator()(const DocInclude &) {}
    void operator()(const DocIncOperator &) {}
    void operator()(const DocFormula &) {}
    void operator()(const DocIndexEntry &);
    void operator()(const DocSimpleSectSep &);
    void operator()(const DocCite &) {}
    void operator()(const DocSeparator &) {}

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
    void operator()(const DocSection &) {}
    void operator()(const DocHtmlList &) {}
    void operator()(const DocHtmlListItem &) {}
    void operator()(const DocHtmlDescList &);
    void operator()(const DocHtmlDescTitle &) {}
    void operator()(const DocHtmlDescData &) {}
    void operator()(const DocHtmlTable &) {}
    void operator()(const DocHtmlRow &) {}
    void operator()(const DocHtmlCell &) {}
    void operator()(const DocHtmlCaption &) {}
    void operator()(const DocInternal &) {}
    void operator()(const DocHRef &) {}
    void operator()(const DocHtmlSummary &) {}
    void operator()(const DocHtmlDetails &) {}
    void operator()(const DocHtmlHeader &) {}
    void operator()(const DocImage &) {}
    void operator()(const DocDotFile &) {}
    void operator()(const DocMscFile &) {}
    void operator()(const DocDiaFile &) {}
    void operator()(const DocPlantUmlFile &) {}
    void operator()(const DocLink &) {}
    void operator()(const DocRef &);
    void operator()(const DocSecRefItem &) {}
    void operator()(const DocSecRefList &) {}
    void operator()(const DocParamSect &);
    void operator()(const DocParamList &);
    void operator()(const DocXRefItem &) {}
    void operator()(const DocInternalRef &) {}
    void operator()(const DocText &) {}
    void operator()(const DocHtmlBlockQuote &) {}
    void operator()(const DocVhdlFlow &) {}
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

    //--------------------------------------
    // helper functions
    //--------------------------------------
    void startSimpleSect(const DocSimpleSect &s);
    void endSimpleSect(const DocSimpleSect &s);
    bool needsComma();
    void beginScope();
    void addComma();
    void endScope();
    void json_string_start();
    void json_string_end();
    void json_string(const char*str);
    void json_object_start();
    void json_object_end();
    void json_array_start();
    void json_array_end();
    //--------------------------------------
    // state variables
    //--------------------------------------
    bool m_insideString;
    int m_lvl;
    uint64_t m_ic;
    QCString &m_t;
    QCString m_tmpstr;
    void filter(const QCString &str);
};

#endif
