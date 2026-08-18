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
#include "containers.h"

class OutputCodeList;

/** Generator for HTML code fragments */
class HtmlCodeGenerator final : public OutputCodeIntf
{
  public:
    HtmlCodeGenerator(TextStream *t,const DString &relPath);
    HtmlCodeGenerator(TextStream *t);

    void setTextStream(TextStream *t) { m_t = t; }
    void setFileName(const DString fileName) { m_fileName = fileName; }
    DString fileName() { return m_fileName; }

    OutputType type() const override { return OutputType::Html; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<HtmlCodeGenerator>(*this); }
    void codify(const DString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void setStripIndentAmount(size_t amount) override;
    void writeCodeLink(CodeSymbolType type,
                       const DString &ref,const DString &file,
                       const DString &anchor,const DString &name,
                       const DString &tooltip) override;
    void writeTooltip(const DString &id,
                      const DocLinkInfo &docInfo,
                      const DString &decl,
                      const DString &desc,
                      const SourceLinkInfo &defInfo,
                      const SourceLinkInfo &declInfo
                     ) override;
    void writeLineNumber(const DString &,const DString &,const DString &,int, bool) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const DString &s) override;
    void endFontClass() override;
    void writeCodeAnchor(const DString &anchor) override;
    void startCodeFragment(const DString &style) override;
    void endCodeFragment(const DString &) override;
    void startFold(int,const DString &,const DString &) override;
    void endFold() override;

    void setRelativePath(const DString &path);
  private:
    void _writeCodeLink(const DString &className,
                        const DString &ref,const DString &file,
                        const DString &anchor,const DString &name,
                        const DString &tooltip);
    void _startOpenLine();
    //void docify(const DString &str);
    TextStream *m_t;
    size_t m_col = 0;
    DString m_relPath;
    DString m_fileName;
    bool m_lineOpen = false;
    bool m_stripCodeComments = false;
    bool m_hide = false;
    struct LineInfo
    {
      LineInfo() {}
      LineInfo(const DString &r,const DString &f,const DString &a,int l,bool wa)
        : ref(r), fileName(f), anchor(a), line(l), writeAnchor(wa) {}
      DString ref;
      DString fileName;
      DString anchor;
      int line = -1;
      bool writeAnchor = false;
    };
    LineInfo m_lastLineInfo;
    size_t m_stripIndentAmount = 0;
};

/** Generator for HTML output */
class HtmlGenerator final : public OutputGenerator, public OutputGenIntf
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
    static void writeHeaderFile(TextStream &t, const DString &cssname);
    static void writeFooterFile(TextStream &t);
    static void writeTabData();
    static void writeSearchInfoStatic(TextStream &t,const DString &relPath);
    static void writeSearchData(const DString &dir);
    static void writeSearchPage();
    static void writeExternalSearchPage();
    static DString writeLogoAsString(const DString &path);
    static DString writeSplitBarAsString(const DString &name,const DString &relpath,const DString &allMembersFile);
    static DString getNavTreeCss();
    static DString fixSpaces(const DString &s);
    void clearBuffer();

    //---------------------------------------------------------------------

    OutputType type() const override { return OutputType::Html; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<HtmlGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *node,const Definition *,const MemberDef *,int id,int sectionLevel) override;
    void startFile(const DString &name,bool isSource,const DString &manName,const DString &title,int id, int hierarchyLevel) override;
    void endFile() override;

    void writeFooter(const DString &navPath) override;
    void writeSearchInfo() override;

    void startIndexSection(IndexSection) override {}
    void endIndexSection(IndexSection) override {}
    void writePageLink(const DString &,bool) override {}
    void startProjectNumber() override;
    void endProjectNumber() override;
    void writeStyleInfo(int part) override;
    void startTitleHead(const DString &) override;
    void endTitleHead(const DString &,const DString &) override;

    void startParagraph(const DString &classDef) override;
    void endParagraph() override;
    void writeString(const DString &text) override;
    void startIndexListItem() override;
    void endIndexListItem() override;
    void startIndexList() override;
    void endIndexList() override;
    void startIndexKey() override;
    void endIndexKey() override;
    void startIndexValue(bool) override;
    void endIndexValue(const DString &,bool) override;
    void startItemList() override  { m_t << "<ul>\n"; }
    void endItemList() override    { m_t << "</ul>\n"; }
    void startIndexItem(const DString &ref,const DString &file) override;
    void endIndexItem(const DString &ref,const DString &file) override;
    void docify(const DString &text) override;

    void writeObjectLink(const DString &ref,const DString &file,
                         const DString &anchor,const DString &name) override;

    void startTextLink(const DString &file,const DString &anchor) override;
    void endTextLink() override;
    void startTypewriter() override { m_t << "<code>"; }
    void endTypewriter() override   { m_t << "</code>"; }
    void startGroupHeader(const DString &,int) override;
    void endGroupHeader(int) override;
    void startItemListItem() override { m_t << "<li>"; }
    void endItemListItem() override { m_t << "</li>\n"; }

    void startMemberSections() override;
    void endMemberSections() override;
    void startHeaderSection() override;
    void endHeaderSection() override;
    void startMemberHeader(const DString &, int) override;
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
    void startMemberItem(const DString &anchor,MemberItemType,const DString &inheritId) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override;
    void endMemberTemplateParams(const DString &anchor,const DString &inheritId) override;
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;

    void startMemberGroupHeader(const DString &,bool) override;
    void endMemberGroupHeader(bool) override;
    void startMemberGroupDocs() override;
    void endMemberGroupDocs() override;
    void startMemberGroup() override;
    void endMemberGroup(bool) override;

    void insertMemberAlign(bool) override;
    void insertMemberAlignLeft(MemberItemType,bool) override;
    void startMemberDescription(const DString &anchor,const DString &inheritId, bool typ) override;
    void endMemberDescription() override;
    void startMemberDeclaration() override {}
    void endMemberDeclaration(const DString &anchor,const DString &inheritId) override;
    void writeInheritedSectionTitle(const DString &id,   const DString &ref,
                                    const DString &file, const DString &anchor,
                                    const DString &title,const DString &name) override;

    void writeRuler() override    { m_t << "<hr/>"; }
    void writeAnchor(const DString &,const DString &name)
                        override  { m_t << "<a name=\"" << name <<"\" id=\"" << name << "\"></a>"; }
    void startEmphasis() override { m_t << "<em>";  }
    void endEmphasis() override   { m_t << "</em>"; }
    void startBold() override     { m_t << "<b>"; }
    void endBold() override       { m_t << "</b>"; }
    void startDescForItem() override { m_t << "<dd>"; }
    void endDescForItem() override   { m_t << "</dd>\n"; }
    void lineBreak(const DString &style) override;
    void writeChar(char c) override;
    void startMemberDoc(const DString &clName, const DString &memName,
                        const DString &anchor, const DString &title,
                        int memCount, int memTotal, bool showInline) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const DString &fName,const DString &manName,
                         const DString &anchor,const DString &name,
                         const DString &args) override;
    void endDoxyAnchor(const DString &fName,const DString &anchor) override;
    void addLabel(const DString &,const DString &) override;
    void writeLatexSpacing() override {}
    void writeStartAnnoItem(const DString &type,const DString &file,
                            const DString &path,const DString &name) override;
    void startCenter() override        { m_t << "<center>\n"; }
    void endCenter() override          { m_t << "</center>\n"; }
    void startSmall() override         { m_t << "<small>\n"; }
    void endSmall() override           { m_t << "</small>\n"; }
    void startExamples() override;
    void endExamples() override;
    void startSection(const DString &,const DString &,SectionType) override;
    void endSection(const DString &,SectionType) override;
    void addIndexItem(const DString &,const DString &) override;
    void startIndent() override;
    void endIndent() override;
    void writeSynopsis() override {}
    void startClassDiagram() override;
    void endClassDiagram(const ClassDiagram &,const DString &,const DString &) override;
    void startPageRef() override {}
    void endPageRef(const DString &,const DString &) override {}
    void startQuickIndices() override {}
    void endQuickIndices() override;
    void writeSplitBar(const DString &name,const DString &allMembersFile) override;
    void writeNavigationPath(const DString &s) override;
    void writeLogo() override;
    void writeQuickLinks(HighlightedItem hli,const DString &file,bool extraTabs) override;
    void writeSummaryLink(const DString &file,const DString &anchor,const DString &title,bool first) override;
    void writePageOutline() override;
    void startContents() override;
    void endContents() override;
    void startPageDoc(const DString &pageTitle) override;
    void endPageDoc() override;
    void writeNonBreakableSpace(int) override;

    void startDescTable(const DString &title,const bool hasInits) override;
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
    void startParameterType(bool first,const DString &key) override;
    void endParameterType() override;
    void startParameterName(bool) override;
    void endParameterName() override;
    void startParameterExtra() override;
    void endParameterExtra(bool last,bool emptyList,bool closeBracket) override;
    void startParameterDefVal(const char *sep) override;
    void endParameterDefVal() override;
    void startParameterList(bool) override;
    void endParameterList() override;
    void exceptionEntry(const DString &,bool) override;

    void startConstraintList(const DString &) override;
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
    void writeLabel(const DString &l,bool isLast) override;
    void endLabels() override;

    void startLocalToc(int level) override;
    void endLocalToc() override;
    void startTocEntry(const SectionInfo *si) override;
    void endTocEntry(const SectionInfo *si) override;

    void startPlainFile(const DString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

    void startEmbeddedDoc(size_t) override;
    void endEmbeddedDoc() override;

  private:
    void startTitle() { m_t << "<div class=\"title\">"; }
    void endTitle() { m_t << "</div>"; }
    static void writePageFooter(TextStream &t,const DString &,const DString &,const DString &);
    void docify_(const DString &text,bool inHtmlComment);

    DString                        m_lastTitle;
    DString                        m_lastFile;
    DString                        m_relPath;
    int                             m_sectionCount = 0;
    bool                            m_emptySection = false;
    std::unique_ptr<OutputCodeList> m_codeList;
    HtmlCodeGenerator              *m_codeGen = nullptr;
    int                             m_pageOutlineIndent = 0;

    struct TocState
    {
      int level = 0;
      int indent = 0;
      int maxLevel = 0;
      BoolVector inLi;
      void writeIndent(TextStream &t) { for (int i=0;i<indent*2;i++) t << " "; }
      void incIndent(TextStream &t,const DString &text) { writeIndent(t); t << text << "\n"; indent++; }
      void decIndent(TextStream &t,const DString &text) { indent--; writeIndent(t); t << text << "\n"; }
    };
    TocState m_tocState;
};

#endif
