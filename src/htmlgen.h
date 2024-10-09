/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "outputgen.h"

class OutputCodeList;

/** Generator for HTML code fragments */
class HtmlCodeGenerator : public OutputCodeIntf
{
  public:
    HtmlCodeGenerator(TextStream *t,const QCString &relPath);
    HtmlCodeGenerator(TextStream *t);

    void setTextStream(TextStream *t) { m_t = t; }
    void setFileName(const QCString fileName) { m_fileName = fileName; }
    QCString fileName() { return m_fileName; }

    OutputType type() const override { return OutputType::Html; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<HtmlCodeGenerator>(*this); }
    void codify(const QCString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) override;
    void writeTooltip(const QCString &id,
                      const DocLinkInfo &docInfo,
                      const QCString &decl,
                      const QCString &desc,
                      const SourceLinkInfo &defInfo,
                      const SourceLinkInfo &declInfo
                     ) override;
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int, bool) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const QCString &s) override;
    void endFontClass() override;
    void writeCodeAnchor(const QCString &anchor) override;
    void startCodeFragment(const QCString &style) override;
    void endCodeFragment(const QCString &) override;
    void startFold(int,const QCString &,const QCString &) override;
    void endFold() override;

    void setRelativePath(const QCString &path);
  private:
    void _writeCodeLink(const QCString &className,
                        const QCString &ref,const QCString &file,
                        const QCString &anchor,const QCString &name,
                        const QCString &tooltip);
    //void docify(const QCString &str);
    TextStream *m_t;
    size_t m_col = 0;
    QCString m_relPath;
    QCString m_fileName;
    bool m_lineOpen = false;
    bool m_stripCodeComments = false;
    bool m_hide = false;
};

/** Generator for HTML output */
class HtmlGenerator : public OutputGenerator, public OutputGenIntf
{
  public:
    HtmlGenerator();
    HtmlGenerator(const HtmlGenerator &);
    HtmlGenerator &operator=(const HtmlGenerator &);
    HtmlGenerator(HtmlGenerator &&) = delete;
    HtmlGenerator &operator=(HtmlGenerator &&) = delete;
   ~HtmlGenerator();

    static void init();
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t, const QCString &cssname);
    static void writeFooterFile(TextStream &t);
    static void writeTabData();
    static void writeSearchInfoStatic(TextStream &t,const QCString &relPath);
    static void writeSearchData(const QCString &dir);
    static void writeSearchPage();
    static void writeExternalSearchPage();
    static QCString writeLogoAsString(const QCString &path);
    static QCString writeSplitBarAsString(const QCString &name,const QCString &relpath);
    static QCString getMathJaxMacros();
    static QCString getNavTreeCss();
    void clearBuffer();

    //---------------------------------------------------------------------

    OutputType type() const override { return OutputType::Html; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<HtmlGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *node,const Definition *,const MemberDef *,int id) override;
    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id, int hierarchyLevel) override;
    void endFile() override;

    void writeFooter(const QCString &navPath) override;
    void writeSearchInfo() override;

    void startIndexSection(IndexSection) override {}
    void endIndexSection(IndexSection) override {}
    void writePageLink(const QCString &,bool) override {}
    void startProjectNumber() override;
    void endProjectNumber() override;
    void writeStyleInfo(int part) override;
    void startTitleHead(const QCString &) override;
    void endTitleHead(const QCString &,const QCString &) override;

    void startParagraph(const QCString &classDef) override;
    void endParagraph() override;
    void writeString(const QCString &text) override;
    void startIndexListItem() override;
    void endIndexListItem() override;
    void startIndexList() override;
    void endIndexList() override;
    void startIndexKey() override;
    void endIndexKey() override;
    void startIndexValue(bool) override;
    void endIndexValue(const QCString &,bool) override;
    void startItemList() override  { m_t << "<ul>\n"; }
    void endItemList() override    { m_t << "</ul>\n"; }
    void startIndexItem(const QCString &ref,const QCString &file) override;
    void endIndexItem(const QCString &ref,const QCString &file) override;
    void docify(const QCString &text) override;

    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor,const QCString &name) override;

    void startTextLink(const QCString &file,const QCString &anchor) override;
    void endTextLink() override;
    void startTypewriter() override { m_t << "<code>"; }
    void endTypewriter() override   { m_t << "</code>"; }
    void startGroupHeader(int) override;
    void endGroupHeader(int) override;
    void startItemListItem() override { m_t << "<li>"; }
    void endItemListItem() override { m_t << "</li>\n"; }

    void startMemberSections() override;
    void endMemberSections() override;
    void startHeaderSection() override;
    void endHeaderSection() override;
    void startMemberHeader(const QCString &, int) override;
    void endMemberHeader() override;
    void startMemberSubtitle() override;
    void endMemberSubtitle() override;
    void startMemberDocList() override;
    void endMemberDocList() override;
    void startMemberList() override;
    void endMemberList() override;
    void startInlineHeader() override;
    void endInlineHeader() override;
    void startAnonTypeScope(int) override {}
    void endAnonTypeScope(int) override {}
    void startMemberItem(const QCString &anchor,MemberItemType,const QCString &inheritId) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override;
    void endMemberTemplateParams(const QCString &anchor,const QCString &inheritId) override;
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;

    void startMemberGroupHeader(bool) override;
    void endMemberGroupHeader() override;
    void startMemberGroupDocs() override;
    void endMemberGroupDocs() override;
    void startMemberGroup() override;
    void endMemberGroup(bool) override;

    void insertMemberAlign(bool) override;
    void insertMemberAlignLeft(MemberItemType,bool) override;
    void startMemberDescription(const QCString &anchor,const QCString &inheritId, bool typ) override;
    void endMemberDescription() override;
    void startMemberDeclaration() override {}
    void endMemberDeclaration(const QCString &anchor,const QCString &inheritId) override;
    void writeInheritedSectionTitle(const QCString &id,   const QCString &ref,
                                    const QCString &file, const QCString &anchor,
                                    const QCString &title,const QCString &name) override;

    void writeRuler() override    { m_t << "<hr/>"; }
    void writeAnchor(const QCString &,const QCString &name)
                        override  { m_t << "<a name=\"" << name <<"\" id=\"" << name << "\"></a>"; }
    void startEmphasis() override { m_t << "<em>";  }
    void endEmphasis() override   { m_t << "</em>"; }
    void startBold() override     { m_t << "<b>"; }
    void endBold() override       { m_t << "</b>"; }
    void startDescForItem() override { m_t << "<dd>"; }
    void endDescForItem() override   { m_t << "</dd>\n"; }
    void lineBreak(const QCString &style) override;
    void writeChar(char c) override;
    void startMemberDoc(const QCString &clName, const QCString &memName,
                        const QCString &anchor, const QCString &title,
                        int memCount, int memTotal, bool showInline) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor,const QCString &name,
                         const QCString &args) override;
    void endDoxyAnchor(const QCString &fName,const QCString &anchor) override;
    void addLabel(const QCString &,const QCString &) override;
    void writeLatexSpacing() override {}
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name) override;
    void startCenter() override        { m_t << "<center>\n"; }
    void endCenter() override          { m_t << "</center>\n"; }
    void startSmall() override         { m_t << "<small>\n"; }
    void endSmall() override           { m_t << "</small>\n"; }
    void startExamples() override;
    void endExamples() override;
    void startSection(const QCString &,const QCString &,SectionType) override;
    void endSection(const QCString &,SectionType) override;
    void addIndexItem(const QCString &,const QCString &) override;
    void startIndent() override;
    void endIndent() override;
    void writeSynopsis() override {}
    void startClassDiagram() override;
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &) override;
    void startPageRef() override {}
    void endPageRef(const QCString &,const QCString &) override {}
    void startQuickIndices() override {}
    void endQuickIndices() override;
    void writeSplitBar(const QCString &name) override;
    void writeNavigationPath(const QCString &s) override;
    void writeLogo() override;
    void writeQuickLinks(HighlightedItem hli,const QCString &file) override;
    void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first) override;
    void startContents() override;
    void endContents() override;
    void startPageDoc(const QCString &pageTitle) override;
    void endPageDoc() override;
    void writeNonBreakableSpace(int) override;

    void startDescTable(const QCString &title,const bool hasInits) override;
    void endDescTable() override;
    void startDescTableRow() override;
    void endDescTableRow() override;
    void startDescTableTitle() override;
    void endDescTableTitle() override;
    void startDescTableInit() override;
    void endDescTableInit() override;
    void startDescTableData() override;
    void endDescTableData() override;

    void startDotGraph() override;
    void endDotGraph(DotClassGraph &g) override;
    void startInclDepGraph() override;
    void endInclDepGraph(DotInclDepGraph &g) override;
    void startGroupCollaboration() override;
    void endGroupCollaboration(DotGroupCollaboration &g) override;
    void startCallGraph() override;
    void endCallGraph(DotCallGraph &g) override;
    void startDirDepGraph() override;
    void endDirDepGraph(DotDirDeps &g) override;
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g) override;

    void startTextBlock(bool) override { m_t << "<div class=\"textblock\">"; }
    void endTextBlock(bool) override { m_t << "</div>"; }
    void lastIndexPage() override {}

    void startMemberDocPrefixItem() override;
    void endMemberDocPrefixItem() override;
    void startMemberDocName(bool) override;
    void endMemberDocName() override;
    void startParameterType(bool first,const QCString &key) override;
    void endParameterType() override;
    void startParameterName(bool) override;
    void endParameterName() override;
    void startParameterExtra() override;
    void endParameterExtra(bool last,bool emptyList,bool closeBracket) override;
    void startParameterDefVal(const char *sep) override;
    void endParameterDefVal() override;
    void startParameterList(bool) override;
    void endParameterList() override;
    void exceptionEntry(const QCString &,bool) override;

    void startConstraintList(const QCString &) override;
    void startConstraintParam() override;
    void endConstraintParam() override;
    void startConstraintType() override;
    void endConstraintType() override;
    void startConstraintDocs() override;
    void endConstraintDocs() override;
    void endConstraintList() override;

    void startMemberDocSimple(bool) override;
    void endMemberDocSimple(bool) override;
    void startInlineMemberType() override;
    void endInlineMemberType() override;
    void startInlineMemberName() override;
    void endInlineMemberName() override;
    void startInlineMemberDoc() override;
    void endInlineMemberDoc() override;

    void startLabels() override;
    void writeLabel(const QCString &l,bool isLast) override;
    void endLabels() override;

    void writeLocalToc(const SectionRefs &sr,const LocalToc &lt) override;

    void startPlainFile(const QCString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

  private:
    void startTitle() { m_t << "<div class=\"title\">"; }
    void endTitle() { m_t << "</div>"; }
    static void writePageFooter(TextStream &t,const QCString &,const QCString &,const QCString &);
    void docify_(const QCString &text,bool inHtmlComment);

    QCString                        m_lastTitle;
    QCString                        m_lastFile;
    QCString                        m_relPath;
    int                             m_sectionCount = 0;
    bool                            m_emptySection = false;
    std::unique_ptr<OutputCodeList> m_codeList;
    HtmlCodeGenerator              *m_codeGen = nullptr;
};

#endif
