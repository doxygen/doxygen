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

#ifndef MANGEN_H
#define MANGEN_H

#include "outputgen.h"

class OutputCodeList;

/** Generator for Man page code fragments */
class ManCodeGenerator final : public OutputCodeIntf
{
  public:
    ManCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const override { return OutputType::Man; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<ManCodeGenerator>(*this); }
    void codify(const DString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void setStripIndentAmount(size_t amount) override;
    void writeCodeLink(CodeSymbolType type,
                       const DString &ref,const DString &file,
                       const DString &anchor,const DString &name,
                       const DString &tooltip) override;
    void writeTooltip(const DString &,
                      const DocLinkInfo &,
                      const DString &,
                      const DString &,
                      const SourceLinkInfo &,
                      const SourceLinkInfo &
                     ) override {}
    void writeLineNumber(const DString &,const DString &,const DString &,int l, bool) override;
    void startCodeLine(int) override {}
    void endCodeLine() override { codify("\n"); m_col=0; }
    void startFontClass(const DString &) override {}
    void endFontClass() override {}
    void writeCodeAnchor(const DString &) override {}
    void startCodeFragment(const DString &style) override;
    void endCodeFragment(const DString &) override;
    void startFold(int,const DString &,const DString &) override {}
    void endFold() override {}

  private:
    size_t m_col = 0;
    TextStream *m_t;
    bool m_stripCodeComments = false;
    bool m_hide = false;
    size_t m_stripIndentAmount = 0;
};

/** Generator for Man page output. */
class ManGenerator final : public OutputGenerator, public OutputGenIntf
{
  public:
    ManGenerator();
    ManGenerator(const ManGenerator &);
    ManGenerator &operator=(const ManGenerator &);
    ManGenerator(ManGenerator &&) = delete;
    ManGenerator &operator=(ManGenerator &&) = delete;
   ~ManGenerator();

    static void init();
    void clearBuffer();
    void newParagraph();

    //--------------------------------------------------------------

    OutputType type() const override { return OutputType::Man; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<ManGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *ast,const Definition *,const MemberDef *,int,int) override;
    void startFile(const DString &name,bool isSource,const DString &manName,const DString &title,int id, int hierarchyLevel) override;
    void endFile() override;

    void writeSearchInfo() override {}
    void writeFooter(const DString &) override {}

    void startPageDoc(const DString &) override {}
    void endPageDoc() override {}
    void startIndexSection(IndexSection) override {}
    void endIndexSection(IndexSection) override {}
    void writePageLink(const DString &,bool) override {}
    void startProjectNumber() override {}
    void endProjectNumber() override {}
    void writeStyleInfo(int) override {}
    void startTitleHead(const DString &) override {}
    void endTitleHead(const DString &,const DString &) override;

    void startParagraph(const DString &classDef) override;
    void endParagraph() override;
    void writeString(const DString &text) override;
    void startIndexListItem() override {}
    void endIndexListItem() override {}
    void startIndexList() override {}
    void endIndexList() override { newParagraph(); }
    void startIndexKey() override {}
    void endIndexKey() override {}
    void startIndexValue(bool) override {}
    void endIndexValue(const DString &,bool) override {}
    void startItemList() override {}
    void endItemList() override { newParagraph(); }
    void startIndexItem(const DString &ref,const DString &file) override;
    void endIndexItem(const DString &ref,const DString &file) override;
    void docify(const DString &text) override;
    void writeObjectLink(const DString &ref,const DString &file,
                         const DString &anchor,const DString &name) override;
    void startTextLink(const DString &,const DString &) override {}
    void endTextLink() override {}
    void startTypewriter() override { m_t << "\\fR"; m_firstCol=false; }
    void endTypewriter() override { m_t << "\\fP"; m_firstCol=false; }
    void startGroupHeader(const DString &,int) override;
    void endGroupHeader(int) override;
    void startMemberSections() override {}
    void endMemberSections() override {}
    void startHeaderSection() override {}
    void endHeaderSection() override;
    void startMemberHeader(const DString &,int) override;
    void endMemberHeader() override;
    void insertMemberAlign(bool) override {}
    void insertMemberAlignLeft(MemberItemType,bool) override {}
    void startMemberSubtitle() override {}
    void endMemberSubtitle() override {}
    void startItemListItem() override;
    void endItemListItem() override;
    void startMemberDocList() override {}
    void endMemberDocList() override {}
    void startMemberList() override;
    void endMemberList() override;
    void startInlineHeader() override;
    void endInlineHeader() override;
    void startAnonTypeScope(int) override;
    void endAnonTypeScope(int) override;
    void startMemberItem(const DString &,MemberItemType,const DString &) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override {}
    void endMemberTemplateParams(const DString &,const DString &) override {}
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;

    void startMemberGroupHeader(const DString &,bool) override;
    void endMemberGroupHeader(bool) override;
    void startMemberGroupDocs() override;
    void endMemberGroupDocs() override;
    void startMemberGroup() override;
    void endMemberGroup(bool) override;

    void writeRuler() override {}
    void writeAnchor(const DString &,const DString &) override {}
    void startEmphasis() override { m_t << "\\fI"; m_firstCol=false; }
    void endEmphasis() override { m_t << "\\fP"; m_firstCol=false; }
    void startBold() override { m_t << "\\fB"; m_firstCol=false; }
    void endBold() override { m_t << "\\fP"; m_firstCol=false; }
    void startBoldEmphasis() { m_t << "\\f(BI"; m_firstCol=false; }
    void endBoldEmphasis() { m_t << "\\fP"; m_firstCol=false; }
    void lineBreak(const DString &) override { m_t << "\n.br\n"; }
    void writeChar(char c) override;
    void startMemberDoc(const DString &,const DString &,const DString &,const DString &,int,int,bool) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const DString &,const DString &,const DString &,const DString &,const DString &) override;
    void endDoxyAnchor(const DString &,const DString &) override {}
    void addLabel(const DString &,const DString &) override;
    void writeLatexSpacing() override {}

    void writeStartAnnoItem(const DString &type,const DString &file,
                            const DString &path,const DString &name) override;
    void startCenter() override {}
    void endCenter() override {}
    void startSmall() override {}
    void endSmall() override {}
    void startMemberDescription(const DString &,const DString &,bool) override { m_t << "\n.RI \""; m_firstCol=false; }
    void endMemberDescription() override { m_t << "\""; m_firstCol=false; }
    void startMemberDeclaration() override {}
    void endMemberDeclaration(const DString &,const DString &) override {}
    void writeInheritedSectionTitle(const DString &,const DString &,const DString &,
                      const DString &,const DString &,const DString &) override;
    void startExamples() override;
    void endExamples() override;
    void startDescForItem() override;
    void endDescForItem() override;
    void startSection(const DString &,const DString &,SectionType) override;
    void endSection(const DString &,SectionType) override;
    void addIndexItem(const DString &,const DString &) override {}
    void startIndent() override {}
    void endIndent() override {}
    void writeSynopsis() override;
    void startClassDiagram() override {}
    void endClassDiagram(const ClassDiagram &,const DString &,const DString &) override {}
    void startPageRef() override {}
    void endPageRef(const DString &,const DString &) override {}
    void startQuickIndices() override {}
    void endQuickIndices() override {}
    void writeSplitBar(const DString &,const DString &) override {}
    void writeNavigationPath(const DString &) override {}
    void writeLogo() override {}
    void writeQuickLinks(HighlightedItem,const DString &,bool) override {}
    void writeSummaryLink(const DString &,const DString &,const DString &,bool) override {}
    void writePageOutline() override {}
    void startContents() override {}
    void endContents() override {}
    void writeNonBreakableSpace(int n) override { for (int i=0;i<n;i++) m_t << " "; }

    void startDescTable(const DString &title,const bool hasInits) override;
    void endDescTable() override;
    void startDescTableTitle() override { startItemListItem(); startBoldEmphasis(); endItemListItem(); }
    void endDescTableTitle() override { endBoldEmphasis(); }
    void startDescTableRow() override {}
    void endDescTableRow() override {}
    void startDescTableInit() override {}
    void endDescTableInit() override {}
    void startDescTableData() override { m_t << "\n"; m_firstCol=true; }
    void endDescTableData() override {}

    void startDotGraph() override {}
    void endDotGraph(DotClassGraph &) override {}
    void startInclDepGraph() override {}
    void endInclDepGraph(DotInclDepGraph &) override {}
    void startGroupCollaboration() override {}
    void endGroupCollaboration(DotGroupCollaboration &) override {}
    void startCallGraph() override {}
    void endCallGraph(DotCallGraph &) override {}
    void startDirDepGraph() override {}
    void endDirDepGraph(DotDirDeps &) override {}
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) override {}

    void startTextBlock(bool) override {}
    void endTextBlock(bool) override {}
    void lastIndexPage() override {}

    void startMemberDocPrefixItem() override {}
    void endMemberDocPrefixItem() override {}
    void startMemberDocName(bool) override {}
    void endMemberDocName() override {}
    void startParameterType(bool,const DString &) override {}
    void endParameterType() override;
    void startParameterName(bool) override {}
    void endParameterName() override {}
    void startParameterExtra() override {}
    void endParameterExtra(bool,bool,bool) override;
    void startParameterDefVal(const char *s) override { docify(s); startTypewriter(); }
    void endParameterDefVal() override { endTypewriter(); }
    void startParameterList(bool) override;
    void endParameterList() override {}
    void exceptionEntry(const DString &,bool) override {}

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

    void startLocalToc(int) override {}
    void endLocalToc() override {}
    void startTocEntry(const SectionInfo *) override {}
    void endTocEntry(const SectionInfo *) override {}

    void startPlainFile(const DString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

    void startEmbeddedDoc(size_t) override {}
    void endEmbeddedDoc() override {}

  private:
    bool m_firstCol = true;
    int  m_col = 0;
    bool m_paragraph = true;
    bool m_upperCase = false;
    bool m_insideTabbing = false;
    bool m_inHeader = false;
    std::unique_ptr<OutputCodeList> m_codeList;
    ManCodeGenerator *m_codeGen;

};

#endif
