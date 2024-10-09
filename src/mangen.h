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
class ManCodeGenerator : public OutputCodeIntf
{
  public:
    ManCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const override { return OutputType::Man; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<ManCodeGenerator>(*this); }
    void codify(const QCString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) override;
    void writeTooltip(const QCString &,
                      const DocLinkInfo &,
                      const QCString &,
                      const QCString &,
                      const SourceLinkInfo &,
                      const SourceLinkInfo &
                     ) override {}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int l, bool) override;
    void startCodeLine(int) override {}
    void endCodeLine() override { codify("\n"); m_col=0; }
    void startFontClass(const QCString &) override {}
    void endFontClass() override {}
    void writeCodeAnchor(const QCString &) override {}
    void startCodeFragment(const QCString &style) override;
    void endCodeFragment(const QCString &) override;
    void startFold(int,const QCString &,const QCString &) override {}
    void endFold() override {}

  private:
    int  m_col = 0;
    TextStream *m_t;
    bool m_stripCodeComments = false;
    bool m_hide = false;
};

/** Generator for Man page output. */
class ManGenerator : public OutputGenerator, public OutputGenIntf
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
    void writeDoc(const IDocNodeAST *ast,const Definition *,const MemberDef *,int) override;
    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id, int hierarchyLevel) override;
    void endFile() override;

    void writeSearchInfo() override {}
    void writeFooter(const QCString &) override {}

    void startPageDoc(const QCString &) override {}
    void endPageDoc() override {}
    void startIndexSection(IndexSection) override {}
    void endIndexSection(IndexSection) override {}
    void writePageLink(const QCString &,bool) override {}
    void startProjectNumber() override {}
    void endProjectNumber() override {}
    void writeStyleInfo(int) override {}
    void startTitleHead(const QCString &) override {}
    void endTitleHead(const QCString &,const QCString &) override;

    void startParagraph(const QCString &classDef) override;
    void endParagraph() override;
    void writeString(const QCString &text) override;
    void startIndexListItem() override {}
    void endIndexListItem() override {}
    void startIndexList() override {}
    void endIndexList() override { newParagraph(); }
    void startIndexKey() override {}
    void endIndexKey() override {}
    void startIndexValue(bool) override {}
    void endIndexValue(const QCString &,bool) override {}
    void startItemList() override {}
    void endItemList() override { newParagraph(); }
    void startIndexItem(const QCString &ref,const QCString &file) override;
    void endIndexItem(const QCString &ref,const QCString &file) override;
    void docify(const QCString &text) override;
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor,const QCString &name) override;
    void startTextLink(const QCString &,const QCString &) override {}
    void endTextLink() override {}
    void startTypewriter() override { m_t << "\\fR"; m_firstCol=FALSE; }
    void endTypewriter() override { m_t << "\\fP"; m_firstCol=FALSE; }
    void startGroupHeader(int) override;
    void endGroupHeader(int) override;
    void startMemberSections() override {}
    void endMemberSections() override {}
    void startHeaderSection() override {}
    void endHeaderSection() override;
    void startMemberHeader(const QCString &,int) override;
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
    void startMemberItem(const QCString &,MemberItemType,const QCString &) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override {}
    void endMemberTemplateParams(const QCString &,const QCString &) override {}
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;

    void startMemberGroupHeader(bool) override;
    void endMemberGroupHeader() override;
    void startMemberGroupDocs() override;
    void endMemberGroupDocs() override;
    void startMemberGroup() override;
    void endMemberGroup(bool) override;

    void writeRuler() override {}
    void writeAnchor(const QCString &,const QCString &) override {}
    void startEmphasis() override { m_t << "\\fI"; m_firstCol=FALSE; }
    void endEmphasis() override { m_t << "\\fP"; m_firstCol=FALSE; }
    void startBold() override { m_t << "\\fB"; m_firstCol=FALSE; }
    void endBold() override { m_t << "\\fP"; m_firstCol=FALSE; }
    void startBoldEmphasis() { m_t << "\\f(BI"; m_firstCol=FALSE; }
    void endBoldEmphasis() { m_t << "\\fP"; m_firstCol=FALSE; }
    void lineBreak(const QCString &) override { m_t << "\n.br\n"; }
    void writeChar(char c) override;
    void startMemberDoc(const QCString &,const QCString &,const QCString &,const QCString &,int,int,bool) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const QCString &,const QCString &,const QCString &,const QCString &,const QCString &) override;
    void endDoxyAnchor(const QCString &,const QCString &) override {}
    void addLabel(const QCString &,const QCString &) override;
    void writeLatexSpacing() override {}

    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name) override;
    void startCenter() override {}
    void endCenter() override {}
    void startSmall() override {}
    void endSmall() override {}
    void startMemberDescription(const QCString &,const QCString &,bool) override { m_t << "\n.RI \""; m_firstCol=FALSE; }
    void endMemberDescription() override { m_t << "\""; m_firstCol=FALSE; }
    void startMemberDeclaration() override {}
    void endMemberDeclaration(const QCString &,const QCString &) override {}
    void writeInheritedSectionTitle(const QCString &,const QCString &,const QCString &,
                      const QCString &,const QCString &,const QCString &) override;
    void startExamples() override;
    void endExamples() override;
    void startDescForItem() override;
    void endDescForItem() override;
    void startSection(const QCString &,const QCString &,SectionType) override;
    void endSection(const QCString &,SectionType) override;
    void addIndexItem(const QCString &,const QCString &) override {}
    void startIndent() override {}
    void endIndent() override {}
    void writeSynopsis() override;
    void startClassDiagram() override {}
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &) override {}
    void startPageRef() override {}
    void endPageRef(const QCString &,const QCString &) override {}
    void startQuickIndices() override {}
    void endQuickIndices() override {}
    void writeSplitBar(const QCString &) override {}
    void writeNavigationPath(const QCString &) override {}
    void writeLogo() override {}
    void writeQuickLinks(HighlightedItem,const QCString &) override {}
    void writeSummaryLink(const QCString &,const QCString &,const QCString &,bool) override {}
    void startContents() override {}
    void endContents() override {}
    void writeNonBreakableSpace(int n) override { for (int i=0;i<n;i++) m_t << " "; }

    void startDescTable(const QCString &title,const bool hasInits) override;
    void endDescTable() override;
    void startDescTableTitle() override { startItemListItem(); startBoldEmphasis(); endItemListItem(); }
    void endDescTableTitle() override { endBoldEmphasis(); }
    void startDescTableRow() override {}
    void endDescTableRow() override {}
    void startDescTableInit() override {};
    void endDescTableInit() override {};
    void startDescTableData() override { m_t << "\n"; m_firstCol=TRUE; }
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
    void startParameterType(bool,const QCString &) override {}
    void endParameterType() override;
    void startParameterName(bool) override {}
    void endParameterName() override {}
    void startParameterExtra() override {}
    void endParameterExtra(bool,bool,bool) override;
    void startParameterDefVal(const char *s) override { docify(s); startTypewriter(); }
    void endParameterDefVal() override { endTypewriter(); }
    void startParameterList(bool) override;
    void endParameterList() override {}
    void exceptionEntry(const QCString &,bool) override {}

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

    void writeLocalToc(const SectionRefs &,const LocalToc &) override {}

    void startPlainFile(const QCString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

  private:
    void startTitle();
    void endTitle();

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
