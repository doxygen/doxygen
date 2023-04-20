/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Parker Waechter & Dimitri van Heesch.
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

#include "config.h"
#include "outputgen.h"

class OutputCodeList;

/** Generator for RTF code fragments */
class RTFCodeGenerator
{
  public:
    RTFCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const { return OutputType::RTF; }
    void codify(const QCString &text);
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip);
    void writeTooltip(const QCString &,
                      const DocLinkInfo &,
                      const QCString &,
                      const QCString &,
                      const SourceLinkInfo &,
                      const SourceLinkInfo &
                     ) {}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int l, bool);
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const QCString &);
    void endFontClass();
    void writeCodeAnchor(const QCString &) {}
    void startCodeFragment(const QCString &style);
    void endCodeFragment(const QCString &);

  private:
    friend class RTFGenerator;
    void setSourceFileName(const QCString &name);
    void setIndentLevel(int level) { m_indentLevel=level; }
    QCString rtf_Code_DepthStyle();
    int  m_col = 0;
    TextStream *m_t;
    bool m_doxyCodeLineOpen = false;
    QCString m_sourceFileName;
    int m_indentLevel = 0;
};


/** Generator for RTF output. */
class RTFGenerator : public OutputGenerator
{
  public:
    RTFGenerator();
    RTFGenerator(const RTFGenerator &);
    RTFGenerator &operator=(const RTFGenerator &);
    RTFGenerator(RTFGenerator &&);
    RTFGenerator &operator=(RTFGenerator &&) = delete;
   ~RTFGenerator();

    static void init();
    void cleanup();
    static void writeStyleSheetFile(TextStream &t);
    static void writeExtensionsFile(TextStream &t);
    OutputType type() const { return OutputType::RTF; }

    void setRelativePath(const QCString &path);
    void setSourceFileName(const QCString &sourceFileName);

    void writeDoc(const IDocNodeAST *ast,const Definition *,const MemberDef *,int);

    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id,int hierarchyLevel);
    void writeSearchInfo() {}
    void writeFooter(const QCString &) {}
    void endFile();
    void clearBuffer();

    void startPageDoc(const QCString &) {}
    void endPageDoc() {}
    void startIndexSection(IndexSection);
    void endIndexSection(IndexSection);
    void writePageLink(const QCString &,bool);
    void startProjectNumber();
    void endProjectNumber();
    void writeStyleInfo(int part);
    void startTitleHead(const QCString &);
    void endTitleHead(const QCString &,const QCString &name);

    void startParagraph(const QCString &classDef);
    void endParagraph();
    void writeString(const QCString &text);
    void startIndexListItem();
    void endIndexListItem();
    void startIndexList();
    void endIndexList();
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const QCString &,bool);
    void startItemList();
    void endItemList();
    void startIndexItem(const QCString &ref,const QCString &file);
    void endIndexItem(const QCString &ref,const QCString &file);
    void docify(const QCString &text);
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor,const QCString &name);
    void startTextLink(const QCString &f,const QCString &anchor);
    void endTextLink();
    void startTypewriter() { m_t << "{\\f2 "; }
    void endTypewriter()   { m_t << "}";      }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startItemListItem();
    void endItemListItem();

    void startMemberSections() {}
    void endMemberSections() {}
    void startHeaderSection() {}
    void endHeaderSection() {}
    void startMemberHeader(const QCString &,int) { startGroupHeader(0); }
    void endMemberHeader() { endGroupHeader(FALSE); }
    void startMemberSubtitle();
    void endMemberSubtitle();
    void startMemberDocList() {}
    void endMemberDocList() {}
    void startMemberList();
    void endMemberList();
    void startInlineHeader();
    void endInlineHeader();
    void startAnonTypeScope(int) {}
    void endAnonTypeScope(int) {}
    void startMemberItem(const QCString &,MemberItemType,const QCString &);
    void endMemberItem(MemberItemType);
    void startMemberTemplateParams() {}
    void endMemberTemplateParams(const QCString &,const QCString &) {}
    void startCompoundTemplateParams();
    void endCompoundTemplateParams();
    void insertMemberAlign(bool) {}
    void insertMemberAlignLeft(MemberItemType,bool){}

    void writeRuler() { rtfwriteRuler_thin(); }

    void writeAnchor(const QCString &fileName,const QCString &name);
    void startEmphasis() { m_t << "{\\i ";  }
    void endEmphasis()   { m_t << "}"; }
    void startBold()     { m_t << "{\\b "; }
    void endBold()       { m_t << "}"; }
    void lineBreak(const QCString &style=QCString());
    void startMemberDoc(const QCString &,const QCString &,const QCString &,const QCString &,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &,const QCString &,const QCString &,const QCString &,const QCString &);
    void endDoxyAnchor(const QCString &,const QCString &);
    void writeChar(char c);
    void writeLatexSpacing() {};//{ m_t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name);
    void startCenter()      { m_t << "{\\qc\n"; }
    void endCenter()        { m_t << "}"; }
    void startSmall()       { m_t << "{\\sub "; }
    void endSmall()         { m_t << "}"; }

    void startMemberDescription(const QCString &,const QCString &,bool);
    void endMemberDescription();
    void startMemberDeclaration() {}
    void endMemberDeclaration(const QCString &,const QCString &) {}
    void writeInheritedSectionTitle(const QCString &,const QCString &,const QCString &,
                      const QCString &,const QCString &,const QCString &);
    void startExamples();
    void endExamples();
    void startDescForItem();
    void endDescForItem();
    void startSection(const QCString &,const QCString &,SectionType);
    void endSection(const QCString &,SectionType);
    void addIndexItem(const QCString &,const QCString &);
    void startIndent();
    void endIndent();
    void writeSynopsis()     {}
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const QCString &filename,const QCString &name);
    void startPageRef();
    void endPageRef(const QCString &,const QCString &);
    void startQuickIndices() {}
    void endQuickIndices() {}
    void writeSplitBar(const QCString &) {}
    void writeNavigationPath(const QCString &) {}
    void writeLogo() {}
    void writeQuickLinks(bool,HighlightedItem,const QCString &) {}
    void writeSummaryLink(const QCString &,const QCString &,const QCString &,bool) {}
    void startContents() {}
    void endContents() {}
    void writeNonBreakableSpace(int);

    void startDescTable(const QCString &title);
    void endDescTable();
    void startDescTableRow();
    void endDescTableRow();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableData();
    void endDescTableData();

    void startDotGraph();
    void endDotGraph(DotClassGraph &);
    void startInclDepGraph();
    void endInclDepGraph(DotInclDepGraph &);
    void startGroupCollaboration();
    void endGroupCollaboration(DotGroupCollaboration &g);
    void startCallGraph();
    void endCallGraph(DotCallGraph &);
    void startDirDepGraph();
    void endDirDepGraph(DotDirDeps &g);
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) {}

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void startTextBlock(bool dense);
    void endTextBlock(bool);
    void lastIndexPage();

    void startMemberDocPrefixItem() {}
    void endMemberDocPrefixItem() {}
    void startMemberDocName(bool) {}
    void endMemberDocName() {}
    void startParameterType(bool,const QCString &);
    void endParameterType();
    void startParameterName(bool) {}
    void endParameterName(bool,bool,bool) {}
    void startParameterList(bool) {}
    void endParameterList() {}
    void exceptionEntry(const QCString &,bool);

    void startConstraintList(const QCString &);
    void startConstraintParam();
    void endConstraintParam();
    void startConstraintType();
    void endConstraintType();
    void startConstraintDocs();
    void endConstraintDocs();
    void endConstraintList();

    void startMemberDocSimple(bool);
    void endMemberDocSimple(bool);
    void startInlineMemberType();
    void endInlineMemberType();
    void startInlineMemberName();
    void endInlineMemberName();
    void startInlineMemberDoc();
    void endInlineMemberDoc();

    void startLabels();
    void writeLabel(const QCString &l,bool isLast);
    void endLabels();

    void writeLocalToc(const SectionRefs &,const LocalToc &) {}

    static bool preProcessFileInplace(const QCString &path,const QCString &name);

    void addCodeGen(OutputCodeList &list);

  private:
    QCString rtf_BList_DepthStyle();
    QCString rtf_CList_DepthStyle();
    QCString rtf_EList_DepthStyle();
    QCString rtf_LCList_DepthStyle();
    QCString rtf_DList_DepthStyle();
    void beginRTFDocument();
    void beginRTFChapter();
    void beginRTFSection();
    void rtfwriteRuler_doubleline();
    void rtfwriteRuler_emboss();
    void rtfwriteRuler_thick();
    void rtfwriteRuler_thin();
    void writeRTFReference(const QCString &label);
    void newParagraph();

    int indentLevel() const;
    void incIndentLevel();
    void decIndentLevel();

    bool m_bstartedBody = false;  // has startbody been called yet?
    bool m_omitParagraph = false; // should a the next paragraph command be ignored?
    int  m_numCols = 0; // number of columns in a table
    QCString m_relPath;
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

QCString rtfFormatBmkStr(const QCString &name);


#endif
