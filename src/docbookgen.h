/******************************************************************************
*
* Copyright (C) 1997-2022 by Dimitri van Heesch.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation under the terms of the GNU General Public License is hereby
* granted. No representations are made about the suitability of this software
* for any purpose. It is provided "as is" without express or implied warranty.
* See the GNU General Public License for more details.
*
*/

#ifndef DOCBOOKGEN_H
#define DOCBOOKGEN_H

#include <memory>
#include <array>

#include "config.h"
#include "outputgen.h"

#if 0
// define for cases that have been implemented with an empty body
#define DB_GEN_EMPTY  m_t << "<!-- DBG_GEN_head_check " << __LINE__ << " -->\n";
#else
#define DB_GEN_EMPTY
#endif

#if 0
// Generic debug statements
#define DB_GEN_H DB_GEN_H1(m_t)
#define DB_GEN_H1(x) x << "<!-- DBG_GEN_head " << __LINE__ << " -->\n";
#define DB_GEN_H2(y) DB_GEN_H2a(m_t,y)
#define DB_GEN_H2a(x,y) x << "<!-- DBG_GEN_head " << __LINE__ << " " << y << " -->\n";
// define for cases that have NOT yet been implemented / considered
#define DB_GEN_NEW fprintf(stderr,"DBG_GEN_head %d\n",__LINE__); DB_GEN_H
#else
#define DB_GEN_H
#define DB_GEN_H1(x)
#define DB_GEN_H2(y)
#define DB_GEN_H2a(x,y)
#define DB_GEN_NEW
#endif

class OutputCodeList;

class DocbookCodeGenerator final : public OutputCodeIntf
{
  public:
    DocbookCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const override { return OutputType::Docbook; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<DocbookCodeGenerator>(m_t); }
    void codify(const DString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void setStripIndentAmount(size_t amount) override;
    void writeCodeLink(CodeSymbolType type,
        const DString &ref,const DString &file,
        const DString &anchor,const DString &name,
        const DString &tooltip) override;
    void writeTooltip(const DString &, const DocLinkInfo &, const DString &,
                      const DString &, const SourceLinkInfo &, const SourceLinkInfo &
                     ) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const DString &colorClass) override;
    void endFontClass() override;
    void writeCodeAnchor(const DString &) override;
    void writeLineNumber(const DString &extRef,const DString &compId,
        const DString &anchorId,int l, bool writeLineAnchor) override;
    void startCodeFragment(const DString &style) override;
    void endCodeFragment(const DString &style) override;
    void startFold(int,const DString &,const DString &) override {}
    void endFold() override {}

    void setRelativePath(const DString &path) { m_relPath = path; }
    void setSourceFileName(const DString &sourceFileName) { m_sourceFileName = sourceFileName; }
    DString sourceFileName() { return m_sourceFileName; }
    void finish();

  private:
    void writeCodeLinkLine(CodeSymbolType type,
        const DString &ref,const DString &file,
        const DString &anchor,const DString &name,
        const DString &tooltip, bool);
    TextStream *m_t;
    DString    m_refId;
    DString    m_external;
    int         m_lineNumber = -1;
    size_t      m_col = 0;
    bool        m_insideCodeLine = false;
    bool        m_insideSpecialHL = false;
    DString    m_relPath;
    DString    m_sourceFileName;
    bool        m_stripCodeComments = false;
    bool        m_hide = false;
    size_t      m_stripIndentAmount = 0;
};

class DocbookGenerator final : public OutputGenerator, public OutputGenIntf
{
  public:
    DocbookGenerator();
    DocbookGenerator(const DocbookGenerator &);
    DocbookGenerator &operator=(const DocbookGenerator &);
    DocbookGenerator(DocbookGenerator &&) = delete;
    DocbookGenerator &operator=(DocbookGenerator &&) = delete;
   ~DocbookGenerator();

    static void init();

    //void setCurrentDoc(const Definition *,const DString &,bool) override {DB_GEN_EMPTY}
    //void addWord(const DString &,bool) override {DB_GEN_EMPTY}

    //--------------------------------------

    OutputType type() const override { return OutputType::Docbook; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<DocbookGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *node,const Definition *ctx,const MemberDef *md,int id,int sectionLevel) override;
    void startFile(const DString &name,bool isSource,const DString &manName,const DString &title,int id,int hierarchyLevel) override;
    void endFile() override;

    void writeSearchInfo() override {DB_GEN_EMPTY}
    void writeFooter(const DString &) override {DB_GEN_NEW}
    void startIndexSection(IndexSection) override;
    void endIndexSection(IndexSection) override;
    void writePageLink(const DString &,bool) override;
    void startProjectNumber() override {DB_GEN_NEW}
    void endProjectNumber() override {DB_GEN_NEW}
    void writeStyleInfo(int) override {DB_GEN_EMPTY}
    void startTitleHead(const DString &) override;
    void endTitleHead(const DString &fileName,const DString &name) override;
    void startIndexListItem() override {DB_GEN_NEW}
    void endIndexListItem() override {DB_GEN_NEW}
    void startIndexList() override {DB_GEN_NEW}
    void endIndexList() override {DB_GEN_NEW}
    void startIndexKey() override {DB_GEN_NEW}
    void endIndexKey() override {DB_GEN_NEW}
    void startIndexValue(bool) override {DB_GEN_NEW}
    void endIndexValue(const DString &,bool) override {DB_GEN_NEW}
    void startItemList() override {DB_GEN_EMPTY}
    void endItemList() override {DB_GEN_EMPTY}

    void startIndexItem(const DString &,const DString &) override {DB_GEN_NEW}
    void endIndexItem(const DString &,const DString &) override {DB_GEN_NEW}
    void startItemListItem() override {DB_GEN_EMPTY}
    void endItemListItem() override {DB_GEN_EMPTY}
    void docify(const DString &text) override;
    void writeChar(char) override;
    void writeString(const DString &) override;
    void startParagraph(const DString &) override;
    void endParagraph() override;
    void writeObjectLink(const DString &,const DString &,const DString &,const DString &) override;
    void startBold() override;
    void endBold() override;
    void startTypewriter() override;
    void endTypewriter() override;
    void startEmphasis() override {DB_GEN_NEW}
    void endEmphasis() override {DB_GEN_NEW}
    void writeRuler() override;
    void startDescForItem() override {DB_GEN_EMPTY}
    void endDescForItem() override {DB_GEN_EMPTY}
    void startCenter() override {DB_GEN_NEW}
    void endCenter() override {DB_GEN_NEW}
    void startSmall() override {DB_GEN_NEW}
    void endSmall() override {DB_GEN_NEW}
    void startExamples() override;
    void endExamples() override;
    void writeAnchor(const DString &,const DString &) override {DB_GEN_EMPTY}
    void startSection(const DString &,const DString &,SectionType) override;
    void endSection(const DString &,SectionType) override;
    void lineBreak(const DString &) override;
    void addIndexItem(const DString &,const DString &) override;
    void writeNonBreakableSpace(int) override;

    void startDescTable(const DString &title,bool hasInits) override;
    void endDescTable() override;
    void startDescTableRow() override;
    void endDescTableRow() override;
    void startDescTableTitle() override;
    void endDescTableTitle() override;
    void startDescTableInit() override;
    void endDescTableInit() override;
    void startDescTableData() override;
    void endDescTableData() override;
    void startTextLink(const DString &,const DString &) override {DB_GEN_NEW}
    void endTextLink() override {DB_GEN_NEW}
    void startPageRef() override {DB_GEN_NEW}
    void endPageRef(const DString &,const DString &) override {DB_GEN_NEW}

    void startGroupHeader(const DString &,int) override;
    void endGroupHeader(int) override;
    void startMemberSections() override {DB_GEN_EMPTY}
    void endMemberSections() override {DB_GEN_EMPTY}
    void startHeaderSection() override {DB_GEN_EMPTY}
    void endHeaderSection() override {DB_GEN_EMPTY}
    void startMemberHeader(const DString &anchor, int typ) override;
    void endMemberHeader() override;
    void startMemberSubtitle() override {DB_GEN_EMPTY}
    void endMemberSubtitle() override {DB_GEN_EMPTY}
    void startMemberDocList() override;
    void endMemberDocList() override;
    void startMemberList() override;
    void endMemberList() override;
    void startInlineHeader() override {DB_GEN_NEW}
    void endInlineHeader() override {DB_GEN_NEW}
    void startAnonTypeScope(int) override {DB_GEN_EMPTY}
    void endAnonTypeScope(int) override {DB_GEN_EMPTY}
    void startMemberItem(const DString &,MemberItemType,const DString &) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override;
    void endMemberTemplateParams(const DString &,const DString &) override;
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;
    void startMemberGroupHeader(const DString &,bool) override;
    void endMemberGroupHeader(bool) override;
    void startMemberGroupDocs() override {DB_GEN_EMPTY}
    void endMemberGroupDocs() override {DB_GEN_EMPTY}
    void startMemberGroup() override;
    void endMemberGroup(bool) override;
    void insertMemberAlign(bool) override {DB_GEN_EMPTY}
    void insertMemberAlignLeft(MemberItemType,bool) override {DB_GEN_EMPTY}
    void startMemberDoc(const DString &,const DString &,
                        const DString &,const DString &,int,int,bool) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const DString &fName,const DString &manName,
                         const DString &anchor,const DString &name,
                         const DString &args) override;
    void endDoxyAnchor(const DString &fileName,const DString &anchor) override;
    void addLabel(const DString &,const DString &) override;
    void writeLatexSpacing() override {DB_GEN_EMPTY}
    void writeStartAnnoItem(const DString &,const DString &,
                            const DString &,const DString &) override {DB_GEN_NEW}
    void startMemberDescription(const DString &,const DString &,bool) override {DB_GEN_EMPTY}
    void endMemberDescription() override {DB_GEN_EMPTY}
    void startMemberDeclaration() override {DB_GEN_EMPTY}
    void endMemberDeclaration(const DString &,const DString &) override {DB_GEN_EMPTY}
    void writeInheritedSectionTitle(const DString &,const DString &,
                                    const DString &,const DString &,
                                    const DString &,const DString &) override;
    void startIndent() override {DB_GEN_EMPTY}
    void endIndent() override {DB_GEN_EMPTY}
    void writeSynopsis() override {DB_GEN_EMPTY}
    void startClassDiagram() override;
    void endClassDiagram(const ClassDiagram &,const DString &,const DString &) override;
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
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) override {DB_GEN_NEW}
    void startQuickIndices() override {DB_GEN_EMPTY}
    void endQuickIndices() override {DB_GEN_EMPTY}
    void writeSplitBar(const DString &,const DString &) override {DB_GEN_EMPTY}
    void writeNavigationPath(const DString &) override {DB_GEN_NEW}
    void writeLogo() override {DB_GEN_NEW}
    void writeQuickLinks(HighlightedItem,const DString &,bool) override {DB_GEN_EMPTY}
    void writeSummaryLink(const DString &,const DString &,const DString &,bool) override {DB_GEN_EMPTY}
    void writePageOutline() override {DB_GEN_EMPTY}
    void startContents() override {DB_GEN_EMPTY}
    void endContents() override {DB_GEN_EMPTY}
    void startPageDoc(const DString &) override {DB_GEN_EMPTY}
    void endPageDoc() override {DB_GEN_EMPTY}
    void startTextBlock(bool) override;
    void endTextBlock(bool) override;
    void lastIndexPage() override {DB_GEN_EMPTY}
    void startMemberDocPrefixItem() override;
    void endMemberDocPrefixItem() override;
    void startMemberDocName(bool) override;
    void endMemberDocName() override;
    void startParameterType(bool,const DString &) override {DB_GEN_EMPTY}
    void endParameterType() override {DB_GEN_EMPTY}
    void startParameterName(bool) override;
    void endParameterName() override;
    void startParameterExtra() override;
    void endParameterExtra(bool,bool,bool) override;
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
    void writeLabel(const DString &,bool) override;
    void endLabels() override;

    void startLocalToc(int level) override;
    void endLocalToc() override;
    void startTocEntry(const SectionInfo *si) override;
    void endTocEntry(const SectionInfo *si) override;

    void startPlainFile(const DString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

    void startEmbeddedDoc(int) override {}
    void endEmbeddedDoc() override {}
    static DString convertToDocbook(const DString &s, bool retainNewline = false, bool citeEntry = false);

private:
    void openSection(const DString &attr=DString());
    void closeSection();
    void closeAllSections();

    DString relPath;
    std::unique_ptr<OutputCodeList>  m_codeList;
    DocbookCodeGenerator *m_codeGen = nullptr;
    bool m_denseText = false;
    bool m_inGroup = false;
    int  m_levelListItem = 0;
    std::array<bool,20> m_inListItem = { false, };
    std::array<bool,20> m_inSimpleSect = { false, };
    bool m_descTable = false;
    bool m_simpleTable = false;
    int m_inLevel = -1;
    bool m_firstMember = false;
    int m_openSectionCount = 0;
    DString m_pageLinks;

    struct TocState
    {
      int level = 0;
      int maxLevel = 0;
      BoolVector inLi;
    };
    TocState m_tocState;
};



#endif
