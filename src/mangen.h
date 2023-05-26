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

#ifndef MANGEN_H
#define MANGEN_H

#include "outputgen.h"

class OutputCodeList;

/** Generator for Man page code fragments */
class ManCodeGenerator
{
  public:
    ManCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const { return OutputType::Man; }

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
    void startCodeLine(bool) {}
    void endCodeLine() { codify("\n"); m_col=0; }
    void startFontClass(const QCString &) {}
    void endFontClass() {}
    void writeCodeAnchor(const QCString &) {}
    void startCodeFragment(const QCString &style);
    void endCodeFragment(const QCString &);

  private:
    int  m_col = 0;
    TextStream *m_t;
};

/** Generator for Man page output. */
class ManGenerator : public OutputGenerator
{
  public:
    ManGenerator();
    ManGenerator(const ManGenerator &);
    ManGenerator &operator=(const ManGenerator &);
    ManGenerator(ManGenerator &&);
    ManGenerator &operator=(ManGenerator &&) = delete;
   ~ManGenerator();

    OutputType type() const { return OutputType::Man; }

    void writeDoc(const IDocNodeAST *ast,const Definition *,const MemberDef *,int);

    static void init();
    void cleanup();
    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id, int hierarchyLevel);
    void writeSearchInfo() {}
    void writeFooter(const QCString &) {}
    void endFile();
    void clearBuffer();

    void startPageDoc(const QCString &) {}
    void endPageDoc() {}
    void startIndexSection(IndexSection) {}
    void endIndexSection(IndexSection) {}
    void writePageLink(const QCString &,bool) {}
    void startProjectNumber() {}
    void endProjectNumber() {}
    void writeStyleInfo(int) {}
    void startTitleHead(const QCString &) {}
    void endTitleHead(const QCString &,const QCString &);

    void newParagraph();
    void startParagraph(const QCString &classDef);
    void endParagraph();
    void writeString(const QCString &text);
    void startIndexListItem() {}
    void endIndexListItem() {}
    void startIndexList() {}
    void endIndexList()   { newParagraph(); }
    void startIndexKey() {}
    void endIndexKey()   {}
    void startIndexValue(bool) {}
    void endIndexValue(const QCString &,bool)   {}
    void startItemList()  {}
    void endItemList()    { newParagraph(); }
    void startIndexItem(const QCString &ref,const QCString &file);
    void endIndexItem(const QCString &ref,const QCString &file);
    void docify(const QCString &text);
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor,const QCString &name);
    void startTextLink(const QCString &,const QCString &) {}
    void endTextLink() {}
    void startTypewriter() { m_t << "\\fC"; m_firstCol=FALSE; }
    void endTypewriter()   { m_t << "\\fP"; m_firstCol=FALSE; }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startMemberSections() {}
    void endMemberSections() {}
    void startHeaderSection() {}
    void endHeaderSection();
    void startMemberHeader(const QCString &,int);
    void endMemberHeader();
    void insertMemberAlign(bool) {}
    void insertMemberAlignLeft(MemberItemType,bool){}
    void startMemberSubtitle() {}
    void endMemberSubtitle() {}
    void startItemListItem();
    void endItemListItem();
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
    void startMemberTemplateParams() {}
    void endMemberTemplateParams(const QCString &,const QCString &) {}
    void startCompoundTemplateParams();
    void endCompoundTemplateParams();

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void writeRuler()    {}
    void writeAnchor(const QCString &,const QCString &) {}
    void startEmphasis() { m_t << "\\fI"; m_firstCol=FALSE; }
    void endEmphasis()   { m_t << "\\fP"; m_firstCol=FALSE; }
    void startBold()     { m_t << "\\fB"; m_firstCol=FALSE; }
    void endBold()       { m_t << "\\fP"; m_firstCol=FALSE; }
    void lineBreak(const QCString &) { m_t << "\n.br\n"; }
    void writeChar(char c);
    void startMemberDoc(const QCString &,const QCString &,const QCString &,const QCString &,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &,const QCString &,const QCString &,const QCString &,const QCString &);
    void endDoxyAnchor(const QCString &,const QCString &) {}
    void writeLatexSpacing() {}
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name);
    void startCenter()        {}
    void endCenter()          {}
    void startSmall()         {}
    void endSmall()           {}
    void startMemberDescription(const QCString &,const QCString &,bool) { m_t << "\n.RI \""; m_firstCol=FALSE; }
    void endMemberDescription()   { m_t << "\""; m_firstCol=FALSE; }
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
    void addIndexItem(const QCString &,const QCString &) {}
    void startIndent()        {}
    void endIndent()          {}
    void writeSynopsis();
    void startClassDiagram() {}
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &) {}
    void startPageRef() {}
    void endPageRef(const QCString &,const QCString &) {}
    void startQuickIndices() {}
    void endQuickIndices() {}
    void writeSplitBar(const QCString &) {}
    void writeNavigationPath(const QCString &) {}
    void writeLogo() {}
    void writeQuickLinks(bool,HighlightedItem,const QCString &) {}
    void writeSummaryLink(const QCString &,const QCString &,const QCString &,bool) {}
    void startContents() {}
    void endContents() {}
    void writeNonBreakableSpace(int n) { int i; for (i=0;i<n;i++) m_t << " "; }

    void startDescTable(const QCString &t);
    void endDescTable();
    void startDescTableRow() {}
    void endDescTableRow() {}
    void startDescTableTitle() { startItemListItem(); startBold(); startEmphasis(); endItemListItem(); }
    void endDescTableTitle() { endEmphasis(); endBold(); }
    void startDescTableData() { m_t << "\n"; m_firstCol=TRUE; }
    void endDescTableData() {}

    void startDotGraph() {}
    void endDotGraph(DotClassGraph &) {}
    void startInclDepGraph() {}
    void endInclDepGraph(DotInclDepGraph &) {}
    void startGroupCollaboration() {}
    void endGroupCollaboration(DotGroupCollaboration &) {}
    void startCallGraph() {}
    void endCallGraph(DotCallGraph &) {}
    void startDirDepGraph() {}
    void endDirDepGraph(DotDirDeps &) {}
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) {}

    void startTextBlock(bool) {}
    void endTextBlock(bool) {}
    void lastIndexPage() {}

    void startMemberDocPrefixItem() {}
    void endMemberDocPrefixItem() {}
    void startMemberDocName(bool) {}
    void endMemberDocName() {}
    void startParameterType(bool,const QCString &) {}
    void endParameterType() {}
    void startParameterName(bool) {}
    void endParameterName(bool,bool,bool) {}
    void startParameterList(bool) {}
    void endParameterList() {}
    void exceptionEntry(const QCString &,bool) {}

    void startFontClass(const QCString &) {}
    void endFontClass() {}

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

    void addCodeGen(OutputCodeList &list);

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
