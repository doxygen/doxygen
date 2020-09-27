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

class QFile;

/** Generator for Man page output. */
class ManGenerator : public OutputGenerator
{
  public:
    ManGenerator();
    ManGenerator(const ManGenerator &g);
    ManGenerator &operator=(const ManGenerator &g);
    virtual ~ManGenerator();
    virtual std::unique_ptr<OutputGenerator> clone() const;

    OutputType type() const { return Man; }

    void writeDoc(DocNode *,const Definition *,const MemberDef *);

    static void init();
    void startFile(const char *name,const char *manName,const char *title);
    void writeSearchInfo() {}
    void writeFooter(const char *) {}
    void endFile();
    void clearBuffer();

    void startIndexSection(IndexSections) {}
    void endIndexSection(IndexSections) {}
    void writePageLink(const char *,bool) {}
    void startProjectNumber() {}
    void endProjectNumber() {}
    void writeStyleInfo(int) {}
    void startTitleHead(const char *) {}
    void endTitleHead(const char *,const char *);
    void startTitle();
    void endTitle();

    void newParagraph();
    void startParagraph(const char *classDef);
    void endParagraph();
    void writeString(const char *text);
    void startIndexListItem() {}
    void endIndexListItem() {}
    void startIndexList() {}
    void endIndexList()   { newParagraph(); }
    void startIndexKey() {}
    void endIndexKey()   {}
    void startIndexValue(bool) {}
    void endIndexValue(const char *,bool)   {}
    void startItemList()  {}
    void endItemList()    { newParagraph(); }
    void startIndexItem(const char *ref,const char *file);
    void endIndexItem(const char *ref,const char *file);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip);
    void writeTooltip(const char *, const DocLinkInfo &, const char *,
                      const char *, const SourceLinkInfo &, const SourceLinkInfo &
                     ) {}
    void startTextLink(const char *,const char *) {}
    void endTextLink() {}
    void startHtmlLink(const char *url);
    void endHtmlLink();
    void startTypewriter() { t << "\\fC"; m_firstCol=FALSE; }
    void endTypewriter()   { t << "\\fP"; m_firstCol=FALSE; }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startMemberSections() {}
    void endMemberSections() {}
    void startHeaderSection() {}
    void endHeaderSection();
    void startMemberHeader(const char *,int);
    void endMemberHeader();
    void insertMemberAlign(bool) {}
    void insertMemberAlignLeft(int,bool){}
    void startMemberSubtitle() {}
    void endMemberSubtitle() {}
    //void writeListItem();
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
    void startMemberItem(const char *,int,const char *);
    void endMemberItem();
    void startMemberTemplateParams() {}
    void endMemberTemplateParams(const char *,const char *) {}

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void writeRuler()    {}
    void writeAnchor(const char *,const char *) {}
    void startCodeFragment(const char *);
    void endCodeFragment();
    void writeLineNumber(const char *,const char *,const char *,int l) { t << l << " "; m_col=0; }
    void startCodeLine(bool) {}
    void endCodeLine() { codify("\n"); m_col=0; }
    void startEmphasis() { t << "\\fI"; m_firstCol=FALSE; }
    void endEmphasis()   { t << "\\fP"; m_firstCol=FALSE; }
    void startBold()     { t << "\\fB"; m_firstCol=FALSE; }
    void endBold()       { t << "\\fP"; m_firstCol=FALSE; }
    void startDescription() {}
    void endDescription()   {}
    void startDescItem();
    void endDescItem();
    void lineBreak(const char *) { t << "\n.br" << endl; }
    void writeChar(char c);
    void startMemberDoc(const char *,const char *,const char *,const char *,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const char *,const char *,const char *,const char *,const char *);
    void endDoxyAnchor(const char *,const char *) {}
    void writeLatexSpacing() {}
    void writeStartAnnoItem(const char *type,const char *file,
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *) { t << endl; m_firstCol=TRUE; }
    void startSubsection();
    void endSubsection();
    void startSubsubsection();
    void endSubsubsection();
    void startCenter()        {}
    void endCenter()          {}
    void startSmall()         {}
    void endSmall()           {}
    void startMemberDescription(const char *,const char *,bool) { t << "\n.RI \""; m_firstCol=FALSE; }
    void endMemberDescription()   { t << "\""; m_firstCol=FALSE; }
    void startMemberDeclaration() {}
    void endMemberDeclaration(const char *,const char *) {}
    void writeInheritedSectionTitle(const char *,const char *,const char *,
                      const char *,const char *,const char *) {}
    void startDescList(SectionTypes);
    void endDescList()        {}
    void startExamples();
    void endExamples();
    void startParamList(ParamListTypes,const char *title);
    void endParamList();
    //void writeDescItem();
    void startDescForItem();
    void endDescForItem();
    void startSection(const char *,const char *,SectionType);
    void endSection(const char *,SectionType);
    void addIndexItem(const char *,const char *) {}
    void startIndent()        {}
    void endIndent()          {}
    void writeSynopsis();
    void startClassDiagram() {}
    void endClassDiagram(const ClassDiagram &,const char *,const char *) {}
    void startPageRef() {}
    void endPageRef(const char *,const char *) {}
    void startQuickIndices() {}
    void endQuickIndices() {}
    void writeSplitBar(const char *) {}
    void writeNavigationPath(const char *) {}
    void writeLogo() {}
    void writeQuickLinks(bool,HighlightedItem,const char *) {}
    void writeSummaryLink(const char *,const char *,const char *,bool) {}
    void startContents() {}
    void endContents() {}
    void writeNonBreakableSpace(int n) { int i; for (i=0;i<n;i++) t << " "; }

    void startDescTable(const char *t);
    void endDescTable();
    void startDescTableRow() {}
    void endDescTableRow() {}
    void startDescTableTitle() { startItemListItem(); startBold(); startEmphasis(); endItemListItem(); }
    void endDescTableTitle() { endEmphasis(); endBold(); }
    void startDescTableData() { t << endl; m_firstCol=TRUE; }
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
    void startParameterType(bool,const char *) {}
    void endParameterType() {}
    void startParameterName(bool) {}
    void endParameterName(bool,bool,bool) {}
    void startParameterList(bool) {}
    void endParameterList() {}
    void exceptionEntry(const char*,bool) {}

    void startFontClass(const char *) {}
    void endFontClass() {}

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

    void writeCodeAnchor(const char *) {}
    void setCurrentDoc(const Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}

  private:
    bool m_firstCol = true;
    bool m_paragraph = true;
    int  m_col = 0;
    bool m_upperCase = false;
    bool m_insideTabbing = false;
    bool m_inHeader = false;

};

#endif
