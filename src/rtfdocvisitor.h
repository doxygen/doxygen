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

#ifndef RTFDOCVISITOR_H
#define RTFDOCVISITOR_H

#include <iostream>

#include "docvisitor.h"
#include "docnode.h"

class OutputCodeList;
class TextStream;

/*! @brief Concrete visitor implementation for RTF output. */
class RTFDocVisitor : public DocVisitor
{
  public:
    RTFDocVisitor(TextStream &t,OutputCodeList &ci,const QCString &langExt, int hierarchyLevel = 0);

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
    void operator()(const DocLink &);
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

    //--------------------------------------
    // helper functions
    //--------------------------------------

    void filter(const QCString &str,bool verbatim=FALSE);
    void startLink(const QCString &ref,const QCString &file,
                   const QCString &anchor);
    void endLink(const QCString &ref);
    QCString getStyle(const QCString &name);
    QCString getListTable(const int id);

    int indentLevel() const;
    void incIndentLevel();
    void decIndentLevel();

    void includePicturePreRTF(const QCString &name, bool isTypeRTF, bool hasCaption, bool inlineImage = FALSE);
    void includePicturePostRTF(bool isTypeRTF, bool hasCaption, bool inlineImage = FALSE);
    void writeDotFile(const QCString &fileName, bool hasCaption,const QCString &srcFile,int srcLine,bool newFile = true);
    void writeMscFile(const QCString &fileName, bool hasCaption,const QCString &srcFile,int srcLine,bool newFile = true);
    void writeDiaFile(const QCString &fileName, bool hasCaption,const QCString &srcFile,int srcLine,bool newFile = true);
    void writePlantUMLFile(const QCString &fileName, bool hasCaption);

    //--------------------------------------
    // state variables
    //--------------------------------------

    TextStream &m_t;
    OutputCodeList &m_ci;
    bool m_insidePre = false;
    bool m_hide = false;
    bool m_lastIsPara = false;
    QCString m_langExt;

    static const int maxIndentLevels = 13;
    int m_indentLevel = 0;
    int m_hierarchyLevel = 0;
    struct RTFListItemInfo
    {
      bool isEnum = false;
      bool isCheck = false;
      int number = 1;
      char type = '1';
    };
    RTFListItemInfo m_listItemInfo[maxIndentLevels];
};

#endif
