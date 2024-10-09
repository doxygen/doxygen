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

class DocbookCodeGenerator : public OutputCodeIntf
{
  public:
    DocbookCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const override { return OutputType::Docbook; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<DocbookCodeGenerator>(m_t); }
    void codify(const QCString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void writeCodeLink(CodeSymbolType type,
        const QCString &ref,const QCString &file,
        const QCString &anchor,const QCString &name,
        const QCString &tooltip) override;
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                      const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                     ) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const QCString &colorClass) override;
    void endFontClass() override;
    void writeCodeAnchor(const QCString &) override;
    void writeLineNumber(const QCString &extRef,const QCString &compId,
        const QCString &anchorId,int l, bool writeLineAnchor) override;
    void startCodeFragment(const QCString &style) override;
    void endCodeFragment(const QCString &style) override;
    void startFold(int,const QCString &,const QCString &) override {}
    void endFold() override {}

    void setRelativePath(const QCString &path) { m_relPath = path; }
    void setSourceFileName(const QCString &sourceFileName) { m_sourceFileName = sourceFileName; }
    QCString sourceFileName() { return m_sourceFileName; }
    void finish();

  private:
    void writeCodeLinkLine(CodeSymbolType type,
        const QCString &ref,const QCString &file,
        const QCString &anchor,const QCString &name,
        const QCString &tooltip, bool);
    TextStream *m_t;
    QCString    m_refId;
    QCString    m_external;
    int         m_lineNumber = -1;
    size_t      m_col = 0;
    bool        m_insideCodeLine = false;
    bool        m_insideSpecialHL = false;
    QCString    m_relPath;
    QCString    m_sourceFileName;
    bool        m_stripCodeComments = false;
    bool        m_hide = false;
};

class DocbookGenerator : public OutputGenerator, public OutputGenIntf
{
  public:
    DocbookGenerator();
    DocbookGenerator(const DocbookGenerator &);
    DocbookGenerator &operator=(const DocbookGenerator &);
    DocbookGenerator(DocbookGenerator &&) = delete;
    DocbookGenerator &operator=(DocbookGenerator &&) = delete;
   ~DocbookGenerator();

    static void init();

    //void setCurrentDoc(const Definition *,const QCString &,bool) override {DB_GEN_EMPTY}
    //void addWord(const QCString &,bool) override {DB_GEN_EMPTY}

    //--------------------------------------

    OutputType type() const override { return OutputType::Docbook; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<DocbookGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *node,const Definition *ctx,const MemberDef *md,int id) override;
    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id,int hierarchyLevel) override;
    void endFile() override;

    void writeSearchInfo() override {DB_GEN_EMPTY};
    void writeFooter(const QCString &) override {DB_GEN_NEW};
    void startIndexSection(IndexSection) override;
    void endIndexSection(IndexSection) override;
    void writePageLink(const QCString &,bool) override;
    void startProjectNumber() override {DB_GEN_NEW};
    void endProjectNumber() override {DB_GEN_NEW};
    void writeStyleInfo(int) override {DB_GEN_EMPTY};
    void startTitleHead(const QCString &) override;
    void endTitleHead(const QCString &fileName,const QCString &name) override;
    void startIndexListItem() override {DB_GEN_NEW};
    void endIndexListItem() override {DB_GEN_NEW};
    void startIndexList() override {DB_GEN_NEW};
    void endIndexList() override {DB_GEN_NEW};
    void startIndexKey() override {DB_GEN_NEW};
    void endIndexKey() override {DB_GEN_NEW};
    void startIndexValue(bool) override {DB_GEN_NEW};
    void endIndexValue(const QCString &,bool) override {DB_GEN_NEW};
    void startItemList() override {DB_GEN_EMPTY};
    void endItemList() override {DB_GEN_EMPTY};

    void startIndexItem(const QCString &,const QCString &) override {DB_GEN_NEW};
    void endIndexItem(const QCString &,const QCString &) override {DB_GEN_NEW};
    void startItemListItem() override {DB_GEN_EMPTY};
    void endItemListItem() override {DB_GEN_EMPTY};
    void docify(const QCString &text) override;
    void writeChar(char) override;
    void writeString(const QCString &) override;
    void startParagraph(const QCString &) override;
    void endParagraph() override;
    void writeObjectLink(const QCString &,const QCString &,const QCString &,const QCString &) override;
    void startBold() override;
    void endBold() override;
    void startTypewriter() override;
    void endTypewriter() override;
    void startEmphasis() override {DB_GEN_NEW};
    void endEmphasis() override {DB_GEN_NEW};
    void writeRuler() override;
    void startDescForItem() override {DB_GEN_EMPTY};
    void endDescForItem() override {DB_GEN_EMPTY};
    void startCenter() override {DB_GEN_NEW};
    void endCenter() override {DB_GEN_NEW};
    void startSmall() override {DB_GEN_NEW};
    void endSmall() override {DB_GEN_NEW};
    void startExamples() override;
    void endExamples() override;
    void writeAnchor(const QCString &,const QCString &) override {DB_GEN_EMPTY};
    void startSection(const QCString &,const QCString &,SectionType) override;
    void endSection(const QCString &,SectionType) override;
    void lineBreak(const QCString &) override;
    void addIndexItem(const QCString &,const QCString &) override;
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
    void startTextLink(const QCString &,const QCString &) override {DB_GEN_NEW};
    void endTextLink() override {DB_GEN_NEW};
    void startPageRef() override {DB_GEN_NEW};
    void endPageRef(const QCString &,const QCString &) override {DB_GEN_NEW};

    void startGroupHeader(int) override;
    void endGroupHeader(int) override;
    void startMemberSections() override {DB_GEN_EMPTY};
    void endMemberSections() override {DB_GEN_EMPTY};
    void startHeaderSection() override {DB_GEN_EMPTY};
    void endHeaderSection() override {DB_GEN_EMPTY};
    void startMemberHeader(const QCString &anchor, int typ) override;
    void endMemberHeader() override;
    void startMemberSubtitle() override {DB_GEN_EMPTY};
    void endMemberSubtitle() override {DB_GEN_EMPTY};
    void startMemberDocList() override;
    void endMemberDocList() override;
    void startMemberList() override;
    void endMemberList() override;
    void startInlineHeader() override {DB_GEN_NEW};
    void endInlineHeader() override {DB_GEN_NEW};
    void startAnonTypeScope(int) override {DB_GEN_EMPTY};
    void endAnonTypeScope(int) override {DB_GEN_EMPTY};
    void startMemberItem(const QCString &,MemberItemType,const QCString &) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override;
    void endMemberTemplateParams(const QCString &,const QCString &) override;
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;
    void startMemberGroupHeader(bool) override;
    void endMemberGroupHeader() override;
    void startMemberGroupDocs() override {DB_GEN_EMPTY};
    void endMemberGroupDocs() override {DB_GEN_EMPTY};
    void startMemberGroup() override;
    void endMemberGroup(bool) override;
    void insertMemberAlign(bool) override {DB_GEN_EMPTY};
    void insertMemberAlignLeft(MemberItemType,bool) override {DB_GEN_EMPTY};
    void startMemberDoc(const QCString &,const QCString &,
                        const QCString &,const QCString &,int,int,bool) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor,const QCString &name,
                         const QCString &args) override;
    void endDoxyAnchor(const QCString &fileName,const QCString &anchor) override;
    void addLabel(const QCString &,const QCString &) override;
    void writeLatexSpacing() override {DB_GEN_EMPTY}
    void writeStartAnnoItem(const QCString &,const QCString &,
                            const QCString &,const QCString &) override {DB_GEN_NEW};
    void startMemberDescription(const QCString &,const QCString &,bool) override {DB_GEN_EMPTY};
    void endMemberDescription() override {DB_GEN_EMPTY};
    void startMemberDeclaration() override {DB_GEN_EMPTY};
    void endMemberDeclaration(const QCString &,const QCString &) override {DB_GEN_EMPTY};
    void writeInheritedSectionTitle(const QCString &,const QCString &,
                                    const QCString &,const QCString &,
                                    const QCString &,const QCString &) override;
    void startIndent() override {DB_GEN_EMPTY};
    void endIndent() override {DB_GEN_EMPTY};
    void writeSynopsis() override {DB_GEN_EMPTY};
    void startClassDiagram() override;
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &) override;
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
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) override {DB_GEN_NEW};
    void startQuickIndices() override {DB_GEN_EMPTY};
    void endQuickIndices() override {DB_GEN_EMPTY};
    void writeSplitBar(const QCString &) override {DB_GEN_EMPTY};
    void writeNavigationPath(const QCString &) override {DB_GEN_NEW};
    void writeLogo() override {DB_GEN_NEW};
    void writeQuickLinks(HighlightedItem,const QCString &) override {DB_GEN_EMPTY};
    void writeSummaryLink(const QCString &,const QCString &,const QCString &,bool) override {DB_GEN_EMPTY};
    void startContents() override {DB_GEN_EMPTY};
    void endContents() override {DB_GEN_EMPTY};
    void startPageDoc(const QCString &) override {DB_GEN_EMPTY}
    void endPageDoc() override {DB_GEN_EMPTY}
    void startTextBlock(bool) override;
    void endTextBlock(bool) override;
    void lastIndexPage() override {DB_GEN_EMPTY};
    void startMemberDocPrefixItem() override;
    void endMemberDocPrefixItem() override;
    void startMemberDocName(bool) override;
    void endMemberDocName() override;
    void startParameterType(bool,const QCString &) override {DB_GEN_EMPTY};
    void endParameterType() override {DB_GEN_EMPTY};
    void startParameterName(bool) override;
    void endParameterName() override;
    void startParameterExtra() override;
    void endParameterExtra(bool,bool,bool) override;
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
    void writeLabel(const QCString &,bool) override;
    void endLabels() override;

    void writeLocalToc(const SectionRefs &sr,const LocalToc &lt) override;

    void startPlainFile(const QCString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

private:
    void openSection(const QCString &attr=QCString());
    void closeSection();
    void closeAllSections();

    QCString relPath;
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
    QCString m_pageLinks;
};

QCString convertToDocBook(const QCString &s, const bool retainNewline = false);


#endif
