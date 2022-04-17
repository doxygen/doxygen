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

class TextStream;

class LatexCodeGenerator : public CodeOutputInterface
{
  public:
    LatexCodeGenerator(TextStream &t,const QCString &relPath,const QCString &sourceFile);
    LatexCodeGenerator(TextStream &t);
    void setRelativePath(const QCString &path);
    void setSourceFileName(const QCString &sourceFileName);
    void codify(const QCString &text) override;
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
                     )  override{}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int,bool) override;
    void startCodeLine(bool) override;
    void endCodeLine() override;
    void startFontClass(const QCString &) override;
    void endFontClass() override;
    void writeCodeAnchor(const QCString &) override {}
    void startCodeFragment(const QCString &style) override;
    void endCodeFragment(const QCString &style) override;

    // extra methods not part of CodeOutputInterface
    void incUsedTableLevel() { m_usedTableLevel++; }
    void decUsedTableLevel() { m_usedTableLevel--; }
    int usedTableLevel() const { return m_usedTableLevel; }

  private:
    void _writeCodeLink(const QCString &className,
                        const QCString &ref,const QCString &file,
                        const QCString &anchor,const QCString &name,
                        const QCString &tooltip);
    void docify(const QCString &str);
    bool m_streamSet = false;
    TextStream &m_t;
    QCString m_relPath;
    QCString m_sourceFileName;
    int m_col = 0;
    bool m_doxyCodeLineOpen = false;
    int m_usedTableLevel = 0;
};

/** Generator for LaTeX output. */
class LatexGenerator : public OutputGenerator
{
  public:
    LatexGenerator();
    LatexGenerator(const LatexGenerator &);
    LatexGenerator &operator=(const LatexGenerator &);
    virtual ~LatexGenerator();
    virtual std::unique_ptr<OutputGenerator> clone() const;

    static void init();
    void cleanup();
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t);
    static void writeFooterFile(TextStream &t);

    virtual OutputType type() const { return Latex; }

    // --- CodeOutputInterface
    void codify(const QCString &text)
    { m_codeGen.codify(text); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref, const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { m_codeGen.writeCodeLink(type,ref,file,anchor,name,tooltip); }
    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,int lineNumber, bool writeLineAnchor)
    { m_codeGen.writeLineNumber(ref,file,anchor,lineNumber,writeLineAnchor); }
    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo
                     )
    { m_codeGen.writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }
    void startCodeLine(bool hasLineNumbers)
    { m_codeGen.startCodeLine(hasLineNumbers); }
    void endCodeLine()
    { m_codeGen.endCodeLine(); }
    void startFontClass(const QCString &s)
    { m_codeGen.startFontClass(s); }
    void endFontClass()
    { m_codeGen.endFontClass(); }
    void writeCodeAnchor(const QCString &anchor)
    { m_codeGen.writeCodeAnchor(anchor); }
    void startCodeFragment(const QCString &style)
    { m_codeGen.startCodeFragment(style); }
    void endCodeFragment(const QCString &style)
    { m_codeGen.endCodeFragment(style); }
    // ---------------------------


    void writeDoc(const IDocNodeAST *node,const Definition *ctx,const MemberDef *,int id);

    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id);
    void writeSearchInfo() {}
    void writeFooter(const QCString &) {}
    void endFile();
    void clearBuffer();

    void startIndexSection(IndexSections);
    void endIndexSection(IndexSections);
    void writePageLink(const QCString &,bool);
    void startProjectNumber();
    void endProjectNumber() {}
    void writeStyleInfo(int part);
    void startTitleHead(const QCString &);
    void startTitle();
    void endTitleHead(const QCString &,const QCString &name);
    void endTitle()   { m_t << "}"; }

    void newParagraph();
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
    void startHtmlLink(const QCString &url);
    void endHtmlLink();
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
    void startMemberItem(const QCString &,int,const QCString &);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams(const QCString &,const QCString &);
    void startCompoundTemplateParams() { startSubsubsection(); }
    void endCompoundTemplateParams() { endSubsubsection(); }

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign(bool) {}
    void insertMemberAlignLeft(int,bool){}

    void writeRuler() { m_t << "\n\n"; }
    void writeAnchor(const QCString &fileName,const QCString &name);
    void startEmphasis() { m_t << "{\\em ";  }
    void endEmphasis()   { m_t << "}"; }
    void startBold()     { m_t << "{\\bfseries "; }
    void endBold()       { m_t << "}"; }
    void startDescription();
    void endDescription();
    void startDescItem();
    void endDescItem();
    void lineBreak(const QCString &style=QCString());
    void startMemberDoc(const QCString &,const QCString &,const QCString &,const QCString &,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &,const QCString &,const QCString &,const QCString &,const QCString &);
    void endDoxyAnchor(const QCString &,const QCString &);
    void writeChar(char c);
    void writeLatexSpacing() { m_t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name);
    void writeEndAnnoItem(const QCString &name);
    void startSubsection() { m_t << "\\subsection*{"; }
    void endSubsection() { m_t << "}\n"; }
    void startSubsubsection() { m_t << "\\subsubsection*{"; }
    void endSubsubsection() { m_t << "}\n"; }
    void startCenter()      { m_t << "\\begin{center}\n"; }
    void endCenter()        { m_t << "\\end{center}\n"; }
    void startSmall()       { m_t << "\\footnotesize "; }
    void endSmall()         { m_t << "\\normalsize "; }
    void startMemberDescription(const QCString &,const QCString &,bool);
    void endMemberDescription();
    void startMemberDeclaration() {}
    void endMemberDeclaration(const QCString &,const QCString &) {}
    void writeInheritedSectionTitle(const QCString &,const QCString &,const QCString &,
                      const QCString &,const QCString &,const QCString &) {}
    void startDescList(SectionTypes)     { m_t << "\\begin{Desc}\n\\item["; }
    void endDescList()       { m_t << "\\end{Desc}\n"; }
    void startExamples();
    void endExamples();
    void startParamList(ParamListTypes,const QCString &title);
    void endParamList();
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

    void setCurrentDoc(const Definition *,const QCString &,bool) {}
    void addWord(const QCString &,bool) {}


  private:
    bool m_insideTabbing = false;
    bool m_firstDescItem = true;
    bool m_disableLinks = false;
    QCString m_relPath;
    int m_indent = 0;
    bool templateMemberItem = false;
    LatexCodeGenerator m_codeGen;
};

#endif
