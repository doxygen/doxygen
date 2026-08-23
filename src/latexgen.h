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

#ifndef LATEXGEN_H
#define LATEXGEN_H

#include "outputgen.h"

#define LATEX_STYLE_EXTENSION ".sty"

class OutputCodeList;

/** Generator for LaTeX code fragments */
class LatexCodeGenerator final : public OutputCodeIntf
{
  public:
    LatexCodeGenerator(TextStream *t,const DString &relPath,const DString &sourceFile);
    LatexCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const override { return OutputType::Latex; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<LatexCodeGenerator>(*this); }
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
                     ) override  {}
    void writeLineNumber(const DString &,const DString &,const DString &,int,bool) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const DString &) override;
    void endFontClass() override;
    void writeCodeAnchor(const DString &) override {}
    void startCodeFragment(const DString &style) override;
    void endCodeFragment(const DString &style) override;
    void startFold(int,const DString &,const DString &) override {}
    void endFold() override {}

    // extra methods not part of CodeOutputInterface
    void incUsedTableLevel() { m_usedTableLevel++; }
    void decUsedTableLevel() { m_usedTableLevel--; }
    int usedTableLevel() const { return m_usedTableLevel; }

    void setRelativePath(const DString &path);
    void setSourceFileName(const DString &sourceFileName);
    void setInsideTabbing(bool b) { m_insideTabbing=b; }
    bool insideTabbing() const { return m_insideTabbing; }

  private:
    void _writeCodeLink(const DString &className,
                        const DString &ref,const DString &file,
                        const DString &anchor,const DString &name,
                        const DString &tooltip);
    void docify(const DString &str);
    bool m_streamSet = false;
    TextStream *m_t;
    DString m_relPath;
    DString m_sourceFileName;
    size_t m_col = 0;
    bool m_doxyCodeLineOpen = false;
    int m_usedTableLevel = 0;
    bool m_insideTabbing = false;
    bool m_stripCodeComments = false;
    bool m_hide = false;
    size_t m_stripIndentAmount = 0;
};

/** Generator for LaTeX output. */
class LatexGenerator final : public OutputGenerator, public OutputGenIntf
{
  public:
    LatexGenerator();
    LatexGenerator(const LatexGenerator &);
    LatexGenerator &operator=(const LatexGenerator &);
    LatexGenerator(LatexGenerator &&) = delete;
    LatexGenerator &operator=(LatexGenerator &&) = delete;
   ~LatexGenerator();

    static void init();
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t);
    static void writeFooterFile(TextStream &t);
    void clearBuffer();

    //----------------------------------------------------------------------

    OutputType type() const override { return OutputType::Latex; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<LatexGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *node,const Definition *ctx,const MemberDef *,int id,int sectionLevel) override;
    void startFile(const DString &name,bool isSource,const DString &manName,const DString &title,int id,int hierarchyLevel) override;
    void endFile() override;

    void writeSearchInfo() override {}
    void writeFooter(const DString &) override {}

    void startPageDoc(const DString &) override {}
    void endPageDoc() override {}
    void startIndexSection(IndexSection) override;
    void endIndexSection(IndexSection) override;
    void writePageLink(const DString &,bool) override;
    void startProjectNumber() override;
    void endProjectNumber() override {}
    void writeStyleInfo(int part) override;
    void startTitleHead(const DString &) override;
    void endTitleHead(const DString &,const DString &name) override;

    void startParagraph(const DString &classDef) override;
    void endParagraph() override;
    void writeString(const DString &text) override;
    void startIndexListItem() override {}
    void endIndexListItem() override {}
    void startIndexList() override { m_t << "\\begin{DoxyCompactList}\n"; }
    void endIndexList() override { m_t << "\\end{DoxyCompactList}\n"; }
    void startIndexKey() override;
    void endIndexKey() override;
    void startIndexValue(bool) override;
    void endIndexValue(const DString &,bool) override;
    void startItemList() override { m_t << "\\begin{DoxyCompactItemize}\n"; }
    void endItemList() override { m_t << "\\end{DoxyCompactItemize}\n"; }
    void startIndexItem(const DString &ref,const DString &file) override;
    void endIndexItem(const DString &ref,const DString &file) override;
    void docify(const DString &text) override;
    void writeObjectLink(const DString &ref,const DString &file,
                         const DString &anchor,const DString &name) override;

    void startTextLink(const DString &,const DString &) override;
    void endTextLink() override;
    void startTypewriter() override { m_t << "{\\ttfamily "; }
    void endTypewriter() override { m_t << "}";      }
    void startGroupHeader(const DString &,int) override;
    void endGroupHeader(int) override;
    void startItemListItem() override { m_t << "\\item \n"; }
    void endItemListItem() override {}

    void startMemberSections() override {}
    void endMemberSections() override {}
    void startHeaderSection() override {}
    void endHeaderSection() override {}
    void startMemberHeader(const DString &,int) override;
    void endMemberHeader() override;
    void startMemberSubtitle() override {}
    void endMemberSubtitle() override {}
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
    void startMemberTemplateParams() override;
    void endMemberTemplateParams(const DString &,const DString &) override;
    void startCompoundTemplateParams() override { m_t << "\\subsubsection*{";}
    void endCompoundTemplateParams() override { m_t << "}\n"; }

    void startMemberGroupHeader(const DString &,bool) override;
    void endMemberGroupHeader(bool) override;
    void startMemberGroupDocs() override;
    void endMemberGroupDocs() override;
    void startMemberGroup() override;
    void endMemberGroup(bool) override;

    void insertMemberAlign(bool) override {}
    void insertMemberAlignLeft(MemberItemType,bool) override {}

    void writeRuler() override { m_t << "\n\n"; }
    void writeAnchor(const DString &fileName,const DString &name) override;
    void startEmphasis() override { m_t << "{\\em ";  }
    void endEmphasis() override { m_t << "}"; }
    void startBold() override { m_t << "{\\bfseries "; }
    void endBold() override { m_t << "}"; }
    void lineBreak(const DString &style=DString()) override;
    void startMemberDoc(const DString &,const DString &,const DString &,const DString &,int,int,bool) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const DString &,const DString &,const DString &,const DString &,const DString &) override;
    void endDoxyAnchor(const DString &,const DString &) override;
    void addLabel(const DString &,const DString &) override;
    void writeChar(char c) override;
    void writeLatexSpacing() override { m_t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const DString &type,const DString &file,
                            const DString &path,const DString &name) override;
    void startCenter() override { m_t << "\\begin{center}\n"; }
    void endCenter() override { m_t << "\\end{center}\n"; }
    void startSmall() override { m_t << "\\footnotesize "; }
    void endSmall() override { m_t << "\\normalsize "; }
    void startMemberDescription(const DString &,const DString &,bool) override;
    void endMemberDescription() override;
    void startMemberDeclaration() override {}
    void endMemberDeclaration(const DString &,const DString &) override {}
    void writeInheritedSectionTitle(const DString &,const DString &,const DString &,
                      const DString &,const DString &,const DString &) override;
    void startExamples() override;
    void endExamples() override;
    void startDescForItem() override { m_t << "\\par\n"; }
    void endDescForItem() override {}
    void startSection(const DString &,const DString &,SectionType) override;
    void endSection(const DString &,SectionType) override;
    void addIndexItem(const DString &,const DString &) override;
    void startIndent() override {}
    void endIndent() override {}
    void writeSynopsis() override {}
    void startClassDiagram() override;
    void endClassDiagram(const ClassDiagram &,const DString &,const DString &) override;
    void startPageRef() override;
    void endPageRef(const DString &,const DString &) override;
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
    void lastIndexPage() override;

    void startDotGraph() override;
    void endDotGraph(DotClassGraph &) override;
    void startInclDepGraph() override;
    void endInclDepGraph(DotInclDepGraph &) override;
    void startCallGraph() override;
    void startGroupCollaboration() override;
    void endGroupCollaboration(DotGroupCollaboration &g) override;
    void endCallGraph(DotCallGraph &) override;
    void startDirDepGraph() override;
    void endDirDepGraph(DotDirDeps &g) override;
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) override {}

    void startTextBlock(bool) override {}
    void endTextBlock(bool) override {}

    void startMemberDocPrefixItem() override {}
    void endMemberDocPrefixItem() override { m_t << "\\\\\n"; }
    void startMemberDocName(bool) override {}
    void endMemberDocName() override {}
    void startParameterType(bool,const DString &) override;
    void endParameterType() override;
    void startParameterName(bool) override;
    void endParameterName() override;
    void startParameterExtra() override;
    void endParameterExtra(bool last,bool one,bool bracket) override;
    void startParameterDefVal(const char *s) override { docify(s); startTypewriter(); }
    void endParameterDefVal() override { endTypewriter(); }
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
    void endLocalToc() override {}
    void startTocEntry(const SectionInfo *) override {}
    void endTocEntry(const SectionInfo *) override {}

    void startPlainFile(const DString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

    void startEmbeddedDoc(size_t) override;
    void endEmbeddedDoc() override;

  private:
    bool m_firstDescItem = true;
    bool m_disableLinks = false;
    DString m_relPath;
    int m_indent = 0;
    bool m_templateMemberItem = false;
    std::unique_ptr<OutputCodeList> m_codeList;
    LatexCodeGenerator *m_codeGen;
    bool m_insideTableEnv = false;
    int m_hierarchyLevel = 0;
};

void writeExtraLatexPackages(TextStream &t);
void writeLatexSpecialFormulaChars(TextStream &t);
DString convertToLaTeX(const DString &s,bool insideTabbing,bool keepSpaces=false);

void filterLatexString(TextStream &t,const DString &str,
                       bool insideTabbing,
                       bool insidePre,
                       bool insideItem,
                       bool insideTable,
                       bool keepSpaces,
                       const bool retainNewline = false);

DString latexEscapeLabelName(const DString &s);
DString latexEscapeIndexChars(const DString &s);
DString latexEscapePDFString(const DString &s);
DString latexFilterURL(const DString &s);
void latexWriteIndexItem(TextStream &t,const DString &r1,const DString &s2 = "");


#endif
