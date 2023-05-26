/******************************************************************************
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

#ifndef LATEXGEN_H
#define LATEXGEN_H

#include "config.h"
#include "outputgen.h"

#define LATEX_STYLE_EXTENSION ".sty"

class OutputCodeList;

/** Generator for LaTeX code fragments */
class LatexCodeGenerator
{
  public:
    LatexCodeGenerator(TextStream *t,const QCString &relPath,const QCString &sourceFile);
    LatexCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const { return OutputType::Latex; }

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
                     )  {}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int,bool);
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const QCString &);
    void endFontClass();
    void writeCodeAnchor(const QCString &) {}
    void startCodeFragment(const QCString &style);
    void endCodeFragment(const QCString &style);

    // extra methods not part of CodeOutputInterface
    void incUsedTableLevel() { m_usedTableLevel++; }
    void decUsedTableLevel() { m_usedTableLevel--; }
    int usedTableLevel() const { return m_usedTableLevel; }

    void setRelativePath(const QCString &path);
    void setSourceFileName(const QCString &sourceFileName);
    void setInsideTabbing(bool b) { m_insideTabbing=b; }
    bool insideTabbing() const { return m_insideTabbing; }

  private:
    void _writeCodeLink(const QCString &className,
                        const QCString &ref,const QCString &file,
                        const QCString &anchor,const QCString &name,
                        const QCString &tooltip);
    void docify(const QCString &str);
    bool m_streamSet = false;
    TextStream *m_t;
    QCString m_relPath;
    QCString m_sourceFileName;
    int m_col = 0;
    bool m_doxyCodeLineOpen = false;
    int m_usedTableLevel = 0;
    bool m_insideTabbing = false;
};

/** Generator for LaTeX output. */
class LatexGenerator : public OutputGenerator
{
  public:
    LatexGenerator();
    LatexGenerator(const LatexGenerator &);
    LatexGenerator &operator=(const LatexGenerator &);
    LatexGenerator(LatexGenerator &&);
    LatexGenerator &operator=(LatexGenerator &&) = delete;
   ~LatexGenerator();
    OutputType type() const { return OutputType::Latex; }

    static void init();
    void cleanup();
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t);
    static void writeFooterFile(TextStream &t);

    void writeDoc(const IDocNodeAST *node,const Definition *ctx,const MemberDef *,int id);

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
    void endProjectNumber() {}
    void writeStyleInfo(int part);
    void startTitleHead(const QCString &);
    void endTitleHead(const QCString &,const QCString &name);

    void startParagraph(const QCString &classDef);
    void endParagraph();
    void writeString(const QCString &text);
    void startIndexListItem() {}
    void endIndexListItem() {}
    void startIndexList() { m_t << "\\begin{DoxyCompactList}\n"; }
    void endIndexList()   { m_t << "\\end{DoxyCompactList}\n"; }
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const QCString &,bool);
    void startItemList()  { m_t << "\\begin{DoxyCompactItemize}\n"; }
    void endItemList()    { m_t << "\\end{DoxyCompactItemize}\n"; }
    void startIndexItem(const QCString &ref,const QCString &file);
    void endIndexItem(const QCString &ref,const QCString &file);
    void docify(const QCString &text);
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor,const QCString &name);

    void startTextLink(const QCString &,const QCString &);
    void endTextLink();
    void startTypewriter() { m_t << "{\\ttfamily "; }
    void endTypewriter()   { m_t << "}";      }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startItemListItem() { m_t << "\\item \n"; }
    void endItemListItem()   {}

    void startMemberSections() {}
    void endMemberSections() {}
    void startHeaderSection() {}
    void endHeaderSection() {}
    void startMemberHeader(const QCString &,int);
    void endMemberHeader();
    void startMemberSubtitle() {}
    void endMemberSubtitle() {}
    void startMemberDocList() {}
    void endMemberDocList() {}
    void startMemberList();
    void endMemberList();
    void startInlineHeader();
    void endInlineHeader();
    void startAnonTypeScope(int);
    void endAnonTypeScope(int);
    void startMemberItem(const QCString &,MemberItemType,const QCString &);
    void endMemberItem(MemberItemType);
    void startMemberTemplateParams();
    void endMemberTemplateParams(const QCString &,const QCString &);
    void startCompoundTemplateParams() { m_t << "\\subsubsection*{";}
    void endCompoundTemplateParams()   { m_t << "}\n"; }

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign(bool) {}
    void insertMemberAlignLeft(MemberItemType,bool){}

    void writeRuler() { m_t << "\n\n"; }
    void writeAnchor(const QCString &fileName,const QCString &name);
    void startEmphasis() { m_t << "{\\em ";  }
    void endEmphasis()   { m_t << "}"; }
    void startBold()     { m_t << "{\\bfseries "; }
    void endBold()       { m_t << "}"; }
    void lineBreak(const QCString &style=QCString());
    void startMemberDoc(const QCString &,const QCString &,const QCString &,const QCString &,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &,const QCString &,const QCString &,const QCString &,const QCString &);
    void endDoxyAnchor(const QCString &,const QCString &);
    void writeChar(char c);
    void writeLatexSpacing() { m_t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name);
    void startCenter()      { m_t << "\\begin{center}\n"; }
    void endCenter()        { m_t << "\\end{center}\n"; }
    void startSmall()       { m_t << "\\footnotesize "; }
    void endSmall()         { m_t << "\\normalsize "; }
    void startMemberDescription(const QCString &,const QCString &,bool);
    void endMemberDescription();
    void startMemberDeclaration() {}
    void endMemberDeclaration(const QCString &,const QCString &) {}
    void writeInheritedSectionTitle(const QCString &,const QCString &,const QCString &,
                      const QCString &,const QCString &,const QCString &);
    void startExamples();
    void endExamples();
    void startDescForItem()     { m_t << "\\par\n"; }
    void endDescForItem()       {}
    void startSection(const QCString &,const QCString &,SectionType);
    void endSection(const QCString &,SectionType);
    void addIndexItem(const QCString &,const QCString &);
    void startIndent()       {}
    void endIndent()         {}
    void writeSynopsis()     {}
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &);
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
    void lastIndexPage();

    void startDotGraph();
    void endDotGraph(DotClassGraph &);
    void startInclDepGraph();
    void endInclDepGraph(DotInclDepGraph &);
    void startCallGraph();
    void startGroupCollaboration();
    void endGroupCollaboration(DotGroupCollaboration &g);
    void endCallGraph(DotCallGraph &);
    void startDirDepGraph();
    void endDirDepGraph(DotDirDeps &g);
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) {}

    void startTextBlock(bool) {}
    void endTextBlock(bool) {}

    void startMemberDocPrefixItem() {}
    void endMemberDocPrefixItem() { m_t << "\\\\\n"; }
    void startMemberDocName(bool) {}
    void endMemberDocName() {}
    void startParameterType(bool,const QCString &);
    void endParameterType();
    void startParameterName(bool);
    void endParameterName(bool,bool,bool);
    void startParameterList(bool);
    void endParameterList();
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

    void writeLocalToc(const SectionRefs &sr,const LocalToc &lt);

    void setCurrentDoc(const Definition *,const QCString &,bool) {}
    void addWord(const QCString &,bool) {}

    void addCodeGen(OutputCodeList &list);

  private:
    void startTitle();
    void endTitle()   { m_t << "}"; }

    bool m_firstDescItem = true;
    bool m_disableLinks = false;
    QCString m_relPath;
    int m_indent = 0;
    bool m_templateMemberItem = false;
    std::unique_ptr<OutputCodeList> m_codeList;
    LatexCodeGenerator *m_codeGen;
    bool m_insideTableEnv = false;
    int m_hierarchyLevel = 0;
};

void writeExtraLatexPackages(TextStream &t);
void writeLatexSpecialFormulaChars(TextStream &t);
QCString convertToLaTeX(const QCString &s,bool insideTabbing,bool keepSpaces=FALSE);

void filterLatexString(TextStream &t,const QCString &str,
                       bool insideTabbing,
                       bool insidePre,
                       bool insideItem,
                       bool insideTable,
                       bool keepSpaces,
                       const bool retainNewline = false);

QCString latexEscapeLabelName(const QCString &s);
QCString latexEscapeIndexChars(const QCString &s);
QCString latexEscapePDFString(const QCString &s);
QCString latexFilterURL(const QCString &s);


#endif
