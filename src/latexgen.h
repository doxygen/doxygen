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
    void codify(const char *text);
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip);
    void writeTooltip(const char *,
                      const DocLinkInfo &,
                      const char *,
                      const char *,
                      const SourceLinkInfo &,
                      const SourceLinkInfo &
                     ) {}
    void writeLineNumber(const char *,const char *,const char *,int);
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const char *);
    void endFontClass();
    void writeCodeAnchor(const char *) {}
    void setCurrentDoc(const Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}
    void startCodeFragment(const char *style);
    void endCodeFragment(const char *style);

    // extra methods not part of CodeOutputInterface
    void incUsedTableLevel() { m_usedTableLevel++; }
    void decUsedTableLevel() { m_usedTableLevel--; }
    int usedTableLevel() const { return m_usedTableLevel; }

  private:
    void _writeCodeLink(const char *className,
                        const char *ref,const char *file,
                        const char *anchor,const char *name,
                        const char *tooltip);
    void docify(const char *str);
    bool m_streamSet = false;
    TextStream &m_t;
    QCString m_relPath;
    QCString m_sourceFileName;
    int m_col = 0;
    bool m_prettyCode = false;
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
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t);
    static void writeFooterFile(TextStream &t);

    virtual OutputType type() const { return Latex; }

    // --- CodeOutputInterface
    void codify(const char *text)
    { m_codeGen.codify(text); }
    void writeCodeLink(const char *ref, const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip)
    { m_codeGen.writeCodeLink(ref,file,anchor,name,tooltip); }
    void writeLineNumber(const char *ref,const char *file,const char *anchor,int lineNumber)
    { m_codeGen.writeLineNumber(ref,file,anchor,lineNumber); }
    void writeTooltip(const char *id, const DocLinkInfo &docInfo, const char *decl,
                      const char *desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo
                     )
    { m_codeGen.writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }
    void startCodeLine(bool hasLineNumbers)
    { m_codeGen.startCodeLine(hasLineNumbers); }
    void endCodeLine()
    { m_codeGen.endCodeLine(); }
    void startFontClass(const char *s)
    { m_codeGen.startFontClass(s); }
    void endFontClass()
    { m_codeGen.endFontClass(); }
    void writeCodeAnchor(const char *anchor)
    { m_codeGen.writeCodeAnchor(anchor); }
    void startCodeFragment(const char *style)
    { m_codeGen.startCodeFragment(style); }
    void endCodeFragment(const char *style)
    { m_codeGen.endCodeFragment(style); }
    // ---------------------------


    void writeDoc(DocNode *,const Definition *ctx,const MemberDef *,int id);

    void startFile(const char *name,const char *manName,const char *title,int id);
    void writeSearchInfo() {}
    void writeFooter(const char *) {}
    void endFile();
    void clearBuffer();

    void startIndexSection(IndexSections);
    void endIndexSection(IndexSections);
    void writePageLink(const char *,bool);
    void startProjectNumber();
    void endProjectNumber() {}
    void writeStyleInfo(int part);
    void startTitleHead(const char *);
    void startTitle();
    void endTitleHead(const char *,const char *name);
    void endTitle()   { m_t << "}"; }

    void newParagraph();
    void startParagraph(const char *classDef);
    void endParagraph();
    void writeString(const char *text);
    void startIndexListItem() {}
    void endIndexListItem() {}
    void startIndexList() { m_t << "\\begin{DoxyCompactList}\n"; }
    void endIndexList()   { m_t << "\\end{DoxyCompactList}\n"; }
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const char *,bool);
    void startItemList()  { m_t << "\\begin{DoxyCompactItemize}\n"; }
    void endItemList()    { m_t << "\\end{DoxyCompactItemize}\n"; }
    void startIndexItem(const char *ref,const char *file);
    void endIndexItem(const char *ref,const char *file);
    void docify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);

    void startTextLink(const char *,const char *);
    void endTextLink();
    void startHtmlLink(const char *url);
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
    void startMemberHeader(const char *,int);
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
    void startMemberItem(const char *,int,const char *);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams(const char *,const char *);

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign(bool) {}
    void insertMemberAlignLeft(int,bool){}

    void writeRuler() { m_t << "\n\n"; }
    void writeAnchor(const char *fileName,const char *name);
    void startEmphasis() { m_t << "{\\em ";  }
    void endEmphasis()   { m_t << "}"; }
    void startBold()     { m_t << "{\\bfseries "; }
    void endBold()       { m_t << "}"; }
    void startDescription();
    void endDescription();
    void startDescItem();
    void endDescItem();
    void lineBreak(const char *style=0);
    void startMemberDoc(const char *,const char *,const char *,const char *,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const char *,const char *,const char *,const char *,const char *);
    void endDoxyAnchor(const char *,const char *);
    void writeChar(char c);
    void writeLatexSpacing() { m_t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const char *type,const char *file,
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *name);
    void startSubsection() { m_t << "\\subsection*{"; }
    void endSubsection() { m_t << "}\n"; }
    void startSubsubsection() { m_t << "\\subsubsection*{"; }
    void endSubsubsection() { m_t << "}\n"; }
    void startCenter()      { m_t << "\\begin{center}\n"; }
    void endCenter()        { m_t << "\\end{center}\n"; }
    void startSmall()       { m_t << "\\footnotesize "; }
    void endSmall()         { m_t << "\\normalsize "; }
    void startMemberDescription(const char *,const char *,bool);
    void endMemberDescription();
    void startMemberDeclaration() {}
    void endMemberDeclaration(const char *,const char *) {}
    void writeInheritedSectionTitle(const char *,const char *,const char *,
                      const char *,const char *,const char *) {}
    void startDescList(SectionTypes)     { m_t << "\\begin{Desc}\n\\item["; }
    void endDescList()       { m_t << "\\end{Desc}\n"; }
    void startExamples();
    void endExamples();
    void startParamList(ParamListTypes,const char *title);
    void endParamList();
    void startDescForItem()     { m_t << "\\par\n"; }
    void endDescForItem()       {}
    void startSection(const char *,const char *,SectionType);
    void endSection(const char *,SectionType);
    void addIndexItem(const char *,const char *);
    void startIndent()       {}
    void endIndent()         {}
    void writeSynopsis()     {}
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const char *,const char *);
    void startPageRef();
    void endPageRef(const char *,const char *);
    void startQuickIndices() {}
    void endQuickIndices() {}
    void writeSplitBar(const char *) {}
    void writeNavigationPath(const char *) {}
    void writeLogo() {}
    void writeQuickLinks(bool,HighlightedItem,const char*) {}
    void writeSummaryLink(const char *,const char *,const char *,bool) {}
    void startContents() {}
    void endContents() {}
    void writeNonBreakableSpace(int);

    void startDescTable(const char *title);
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
    void startParameterType(bool,const char *);
    void endParameterType();
    void startParameterName(bool);
    void endParameterName(bool,bool,bool);
    void startParameterList(bool);
    void endParameterList();
    void exceptionEntry(const char*,bool);

    void startConstraintList(const char *);
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
    void writeLabel(const char *l,bool isLast);
    void endLabels();

    void setCurrentDoc(const Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}


  private:
    bool m_insideTabbing = false;
    bool m_firstDescItem = true;
    bool m_disableLinks = false;
    QCString m_relPath;
    int m_indent = 0;
    bool templateMemberItem = false;
    bool m_prettyCode = Config_getBool(LATEX_SOURCE_CODE);
    LatexCodeGenerator m_codeGen;
};

#endif
