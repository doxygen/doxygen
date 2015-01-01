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

#include "outputgen.h"

class QFile;

static const char *latexStyleExtension = ".sty";

class LatexCodeGenerator : public CodeOutputInterface
{
  public:
    LatexCodeGenerator(FTextStream &t,const QCString &relPath,const QCString &sourceFile);
    LatexCodeGenerator();
    void setTextStream(FTextStream &t);
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
    void setCurrentDoc(Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}

  private:
    void _writeCodeLink(const char *className,
                        const char *ref,const char *file,
                        const char *anchor,const char *name,
                        const char *tooltip);
    void docify(const char *str);
    bool m_streamSet;
    FTextStream m_t;
    QCString m_relPath;
    QCString m_sourceFileName;
    int m_col;
    bool m_prettyCode;
};

/** Generator for LaTeX output. */
class LatexGenerator : public OutputGenerator
{
  public:
    LatexGenerator();
   ~LatexGenerator();
    static void init();
    static void writeStyleSheetFile(QFile &f);
    static void writeHeaderFile(QFile &f);
    static void writeFooterFile(QFile &f);

    //OutputGenerator *copy();
    //OutputGenerator *clone() { return new LatexGenerator(*this); }
    //void append(const OutputGenerator *o);
    void enable() 
    { if (genStack->top()) active=*genStack->top(); else active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==Latex) enable();  }
    void disableIf(OutputType o) { if (o==Latex) disable(); }
    void disableIfNot(OutputType o) { if (o!=Latex) disable(); }
    bool isEnabled(OutputType o) { return (o==Latex && active); } 
    OutputGenerator *get(OutputType o) { return (o==Latex) ? this : 0; }

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
    // ---------------------------


    void writeDoc(DocNode *,Definition *ctx,MemberDef *);

    void startFile(const char *name,const char *manName,const char *title);
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
    void endTitle()   { t << "}"; }

    void newParagraph();
    void startParagraph();
    void endParagraph();
    void writeString(const char *text);
    void startIndexListItem() {}
    void endIndexListItem() {}
    void startIndexList() { t << "\\begin{DoxyCompactList}"    << endl; }
    void endIndexList()   { t << "\\end{DoxyCompactList}"      << endl; }
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const char *,bool);
    void startItemList()  { t << "\\begin{DoxyCompactItemize}" << endl; }
    void endItemList()    { t << "\\end{DoxyCompactItemize}"   << endl; }
    void startIndexItem(const char *ref,const char *file);
    void endIndexItem(const char *ref,const char *file);
    void docify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);

    void startTextLink(const char *,const char *);
    void endTextLink();
    void startHtmlLink(const char *url);
    void endHtmlLink();
    void startTypewriter() { t << "{\\ttfamily "; }
    void endTypewriter()   { t << "}";      }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startItemListItem() { t << "\\item " << endl; }
    void endItemListItem()   {}

    void startMemberSections() {}
    void endMemberSections() {} 
    void startHeaderSection() {}
    void endHeaderSection() {}
    void startMemberHeader(const char *);
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

    void writeRuler() { t << endl << endl; }
    void writeAnchor(const char *fileName,const char *name);
    void startCodeFragment();
    void endCodeFragment();
    void startEmphasis() { t << "{\\em ";  }
    void endEmphasis()   { t << "}"; }
    void startBold()     { t << "{\\bfseries "; }
    void endBold()       { t << "}"; }
    void startDescription();
    void endDescription();
    void startDescItem();
    void endDescItem();
    void lineBreak(const char *style=0);
    void startMemberDoc(const char *,const char *,const char *,const char *,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const char *,const char *,const char *,const char *,const char *);
    void endDoxyAnchor(const char *,const char *);
    void writeChar(char c);
    void writeLatexSpacing() { t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const char *type,const char *file, 
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *name);
    void startSubsection() { t << "\\subsection*{"; }
    void endSubsection() { t << "}" << endl; }
    void startSubsubsection() { t << "\\subsubsection*{"; }
    void endSubsubsection() { t << "}" << endl; }
    void startCenter()      { t << "\\begin{center}" << endl; }
    void endCenter()        { t << "\\end{center}" << endl; }
    void startSmall()       { t << "\\footnotesize "; }
    void endSmall()         { t << "\\normalsize "; }
    void startMemberDescription(const char *,const char *);
    void endMemberDescription();
    void startMemberDeclaration() {} 
    void endMemberDeclaration(const char *,const char *) {}
    void writeInheritedSectionTitle(const char *,const char *,const char *,
                      const char *,const char *,const char *) {}
    void startDescList(SectionTypes)     { t << "\\begin{Desc}\n\\item["; }
    void endDescList()       { t << "\\end{Desc}" << endl; }
    void startSimpleSect(SectionTypes,const char *,const char *,const char *);
    void endSimpleSect();
    void startParamList(ParamListTypes,const char *title);
    void endParamList();
    void startDescForItem()     { t << "\\par" << endl; }
    void endDescForItem()       {}
    void startSection(const char *,const char *,SectionInfo::SectionType);
    void endSection(const char *,SectionInfo::SectionType);
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
    
    void startDescTable(const char *title)
    { startSimpleSect(EnumValues,0,0,title);
      startDescForItem();
      t << "\\begin{description}" << endl; }
    void endDescTable()
    { t << "\\end{description}" << endl; 
      endDescForItem();
      endSimpleSect();
    }
    void startDescTableTitle()
    { t << "\\item[{\\em " << endl; }
    void endDescTableTitle()
    { t << "}]"; }
    void startDescTableData() {}
    void endDescTableData() {}
    void lastIndexPage() {}

    void startDotGraph();
    void endDotGraph(const DotClassGraph &);
    void startInclDepGraph();
    void endInclDepGraph(const DotInclDepGraph &);
    void startCallGraph();
    void startGroupCollaboration();
    void endGroupCollaboration(const DotGroupCollaboration &g);
    void endCallGraph(const DotCallGraph &);
    void startDirDepGraph();
    void endDirDepGraph(const DotDirDeps &g);
    void writeGraphicalHierarchy(const DotGfxHierarchyTable &) {}

    void startTextBlock(bool) {}
    void endTextBlock(bool) {}

    void startMemberDocPrefixItem() {}
    void endMemberDocPrefixItem() {}
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

    void startMemberDocSimple();
    void endMemberDocSimple();
    void startInlineMemberType();
    void endInlineMemberType();
    void startInlineMemberName();
    void endInlineMemberName();
    void startInlineMemberDoc();
    void endInlineMemberDoc();

    void startLabels();
    void writeLabel(const char *l,bool isLast);
    void endLabels();

    void setCurrentDoc(Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}


  private:
    LatexGenerator(const LatexGenerator &);
    LatexGenerator &operator=(const LatexGenerator &);
    bool insideTabbing;
    bool firstDescItem;
    bool disableLinks;
    QCString relPath;
    int m_indent;
    bool templateMemberItem;
    bool m_prettyCode;
    LatexCodeGenerator m_codeGen;
};

#endif
