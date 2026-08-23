/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Parker Waechter & Dimitri van Heesch.
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

#ifndef RTFGEN_H
#define RTFGEN_H

#include <array>
#include <memory>

#include "outputgen.h"

class OutputCodeList;

/** Generator for RTF code fragments */
class RTFCodeGenerator final : public OutputCodeIntf
{
  public:
    RTFCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const override { return OutputType::RTF; }
    void codify(const DString &text) override;
    void stripCodeComments(bool b) override;
    void startSpecialComment() override;
    void endSpecialComment() override;
    void setStripIndentAmount(size_t amount) override;
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<RTFCodeGenerator>(*this); }
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
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const DString &) override;
    void endFontClass() override;
    void writeCodeAnchor(const DString &) override {}
    void startCodeFragment(const DString &style) override;
    void endCodeFragment(const DString &) override;
    void startFold(int,const DString &,const DString &) override {}
    void endFold() override {}

  private:
    friend class RTFGenerator;
    void setSourceFileName(const DString &name);
    void setIndentLevel(int level) { m_indentLevel=level; }
    DString rtf_Code_DepthStyle();
    size_t  m_col = 0;
    TextStream *m_t;
    bool m_doxyCodeLineOpen = false;
    DString m_sourceFileName;
    int m_indentLevel = 0;
    bool m_stripCodeComments = false;
    bool m_hide = false;
    size_t m_stripIndentAmount = 0;
};


/** Generator for RTF output. */
class RTFGenerator final : public OutputGenerator, public OutputGenIntf
{
  public:
    RTFGenerator();
    RTFGenerator(const RTFGenerator &);
    RTFGenerator &operator=(const RTFGenerator &);
    RTFGenerator(RTFGenerator &&) = delete;
    RTFGenerator &operator=(RTFGenerator &&) = delete;
   ~RTFGenerator();

    static void init();
    static void writeStyleSheetFile(TextStream &t);
    static void writeExtensionsFile(TextStream &t);

    void setRelativePath(const DString &path);
    void setSourceFileName(const DString &sourceFileName);
    static bool preProcessFileInplace(const DString &path,const DString &name);

    void clearBuffer();

    //-------------------------------------------------------

    OutputType type() const override { return OutputType::RTF; }
    std::unique_ptr<OutputGenIntf> clone() override { return std::make_unique<RTFGenerator>(*this); }
    void addCodeGen(OutputCodeList &list) override;
    void cleanup() override;
    void writeDoc(const IDocNodeAST *ast,const Definition *,const MemberDef *,int,int) override;
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
    void endProjectNumber() override;
    void writeStyleInfo(int part) override;
    void startTitleHead(const DString &) override;
    void endTitleHead(const DString &,const DString &name) override;

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
    void startItemList() override;
    void endItemList() override;
    void startIndexItem(const DString &ref,const DString &file) override;
    void endIndexItem(const DString &ref,const DString &file) override;
    void docify(const DString &text) override;
    void writeObjectLink(const DString &ref,const DString &file,
                         const DString &anchor,const DString &name) override;
    void startTextLink(const DString &f,const DString &anchor) override;
    void endTextLink() override;
    void startTypewriter() override { m_t << "{\\f2 "; }
    void endTypewriter() override { m_t << "}";      }
    void startGroupHeader(const DString &,int) override;
    void endGroupHeader(int) override;
    void startItemListItem() override;
    void endItemListItem() override;

    void startMemberSections() override {}
    void endMemberSections() override {}
    void startHeaderSection() override {}
    void endHeaderSection() override {}
    void startMemberHeader(const DString &,int) override { startGroupHeader("",0); }
    void endMemberHeader() override { endGroupHeader(false); }
    void startMemberSubtitle() override;
    void endMemberSubtitle() override;
    void startMemberDocList() override {}
    void endMemberDocList() override {}
    void startMemberList() override;
    void endMemberList() override;
    void startInlineHeader() override;
    void endInlineHeader() override;
    void startAnonTypeScope(int) override {}
    void endAnonTypeScope(int) override {}
    void startMemberItem(const DString &,MemberItemType,const DString &) override;
    void endMemberItem(MemberItemType) override;
    void startMemberTemplateParams() override {}
    void endMemberTemplateParams(const DString &,const DString &) override {}
    void startCompoundTemplateParams() override;
    void endCompoundTemplateParams() override;
    void insertMemberAlign(bool) override {}
    void insertMemberAlignLeft(MemberItemType,bool) override {}

    void writeRuler() override { rtfwriteRuler_thin(); }

    void writeAnchor(const DString &fileName,const DString &name) override;
    void startEmphasis() override { m_t << "{\\i ";  }
    void endEmphasis() override { m_t << "}"; }
    void startBold() override { m_t << "{\\b "; }
    void endBold() override { m_t << "}"; }
    void lineBreak(const DString &style=DString()) override;
    void startMemberDoc(const DString &,const DString &,const DString &,const DString &,int,int,bool) override;
    void endMemberDoc(bool) override;
    void startDoxyAnchor(const DString &,const DString &,const DString &,const DString &,const DString &) override;
    void endDoxyAnchor(const DString &,const DString &) override;
    void addLabel(const DString &,const DString &) override;
    void writeChar(char c) override;
    void writeLatexSpacing() override {}//{ m_t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const DString &type,const DString &file,
                            const DString &path,const DString &name) override;
    void startCenter() override { m_t << "{\\qc\n"; }
    void endCenter() override { m_t << "}"; }
    void startSmall() override { m_t << "{\\sub "; }
    void endSmall() override { m_t << "}"; }

    void startMemberDescription(const DString &,const DString &,bool) override;
    void endMemberDescription() override;
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
    void addIndexItem(const DString &,const DString &) override;
    void startIndent() override;
    void endIndent() override;
    void writeSynopsis() override {}
    void startClassDiagram() override;
    void endClassDiagram(const ClassDiagram &,const DString &filename,const DString &name) override;
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

    void startDotGraph() override;
    void endDotGraph(DotClassGraph &) override;
    void startInclDepGraph() override;
    void endInclDepGraph(DotInclDepGraph &) override;
    void startGroupCollaboration() override;
    void endGroupCollaboration(DotGroupCollaboration &g) override;
    void startCallGraph() override;
    void endCallGraph(DotCallGraph &) override;
    void startDirDepGraph() override;
    void endDirDepGraph(DotDirDeps &g) override;
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) override {}

    void startMemberGroupHeader(const DString &,bool) override;
    void endMemberGroupHeader(bool) override;
    void startMemberGroupDocs() override;
    void endMemberGroupDocs() override;
    void startMemberGroup() override;
    void endMemberGroup(bool) override;

    void startTextBlock(bool dense) override;
    void endTextBlock(bool) override;
    void lastIndexPage() override;

    void startMemberDocPrefixItem() override {}
    void endMemberDocPrefixItem() override {}
    void startMemberDocName(bool) override {}
    void endMemberDocName() override {}
    void startParameterType(bool,const DString &) override;
    void endParameterType() override;
    void startParameterName(bool) override {}
    void endParameterName() override {}
    void startParameterExtra() override {}
    void endParameterExtra(bool,bool,bool) override;
    void startParameterDefVal(const char *s) override { docify(s); startTypewriter(); }
    void endParameterDefVal() override { endTypewriter(); }
    void startParameterList(bool) override;
    void endParameterList() override {}
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

    void startLocalToc(int) override {}
    void endLocalToc() override {}
    void startTocEntry(const SectionInfo *) override {}
    void endTocEntry(const SectionInfo *) override {}

    void startPlainFile(const DString &name) override { OutputGenerator::startPlainFile(name); }
    void endPlainFile() override { OutputGenerator::endPlainFile(); }

    void startEmbeddedDoc(size_t) override {}
    void endEmbeddedDoc() override {}

  private:
    DString rtf_BList_DepthStyle();
    DString rtf_CList_DepthStyle();
    DString rtf_EList_DepthStyle();
    DString rtf_LCList_DepthStyle();
    DString rtf_DList_DepthStyle();
    void beginRTFDocument();
    void beginRTFChapter();
    void beginRTFSection();
    void rtfwriteRuler_doubleline();
    void rtfwriteRuler_emboss();
    void rtfwriteRuler_thick();
    void rtfwriteRuler_thin();
    void writeRTFReference(const DString &label);
    void newParagraph();

    int indentLevel() const;
    void incIndentLevel();
    void decIndentLevel();

    bool m_bstartedBody = false;  // has startbody been called yet?
    bool m_omitParagraph = false; // should a the next paragraph command be ignored?
    size_t  m_numCols = 0; // number of columns in a table
    DString m_relPath;
    int  m_hierarchyLevel = 0;

    // RTF does not really have a additive indent...manually set list level.
    static const int maxIndentLevels = 13;
    int m_indentLevel = 0;
    struct RTFListItemInfo
    {
      bool isEnum = false;
      int number = 1;
      char type = '1';
    };
    std::array<RTFListItemInfo,maxIndentLevels> m_listItemInfo;
    std::unique_ptr<OutputCodeList> m_codeList;
    RTFCodeGenerator *m_codeGen;
};

DString rtfFormatBmkStr(const DString &name);


#endif
