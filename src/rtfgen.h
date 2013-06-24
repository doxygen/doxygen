/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Parker Waechter & Dimitri van Heesch.
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

#include "outputgen.h"

class QFile;

/** Generator for RTF output. */
class RTFGenerator : public OutputGenerator
{
  public:
    RTFGenerator();
   ~RTFGenerator();
    static void init();
    static void writeStyleSheetFile(QFile &f);
    static void writeExtensionsFile(QFile &file);

    void enable() 
    { if (genStack->top()) active=*genStack->top(); else active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==RTF) enable();  }
    void disableIf(OutputType o) { if (o==RTF) disable(); }
    void disableIfNot(OutputType o) { if (o!=RTF) disable(); }
    bool isEnabled(OutputType o) { return (o==RTF && active); } 
    OutputGenerator *get(OutputType o) { return (o==RTF) ? this : 0; }

    void writeDoc(DocNode *,Definition *,MemberDef *);

    void startFile(const char *name,const char *manName,const char *title);
    void writeSearchInfo() {}
    void writeFooter(const char *) {}
    void endFile();
    void clearBuffer();
    //void postProcess(QByteArray &);
    
    void startIndexSection(IndexSections);
    void endIndexSection(IndexSections);
    void writePageLink(const char *,bool);
    void startProjectNumber();
    void endProjectNumber();
    void writeStyleInfo(int part);
    void startTitleHead(const char *);
    void startTitle();
    void endTitleHead(const char *,const char *name);
    void endTitle() {} 

    void newParagraph();
    void startParagraph();
    void endParagraph();
    void writeString(const char *text);
    void startIndexListItem();
    void endIndexListItem();
    void startIndexList();
    void endIndexList();
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const char *,bool);
    void startItemList();
    void endItemList();
    void startIndexItem(const char *ref,const char *file);
    void endIndexItem(const char *ref,const char *file);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref, const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip);
    void writeTooltip(const char *, const DocLinkInfo &, const char *,
                      const char *, const SourceLinkInfo &, const SourceLinkInfo &
                     ) {}
    void startTextLink(const char *f,const char *anchor);
    void endTextLink();
    void startHtmlLink(const char *url);
    void endHtmlLink();
    void startTypewriter() { t << "{\\f2 "; }
    void endTypewriter()   { t << "}";      }
    void startGroupHeader(int);
    void endGroupHeader(int);
    //void writeListItem();
    void startItemListItem();
    void endItemListItem();

    void startMemberSections() {}
    void endMemberSections() {} 
    void startHeaderSection() {}
    void endHeaderSection() {}
    void startMemberHeader(const char *) { startGroupHeader(FALSE); }
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
    void startMemberItem(const char *,int,const char *);
    void endMemberItem();
    void startMemberTemplateParams() {}
    void endMemberTemplateParams(const char *,const char *) {}
    void insertMemberAlign(bool) {}

    void writeRuler() { rtfwriteRuler_thin(); }
	
    void writeAnchor(const char *fileName,const char *name);
    void startCodeFragment();
    void endCodeFragment();
    void writeLineNumber(const char *,const char *,const char *,int l) { t << l << " "; }
    void startCodeLine(bool) { col=0; }
    void endCodeLine() { lineBreak(); }
    void startEmphasis() { t << "{\\i ";  }
    void endEmphasis()   { t << "}"; }
    void startBold()     { t << "{\\b "; }
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
    void writeLatexSpacing() {};//{ t << "\\hspace{0.3cm}"; }
    void writeStartAnnoItem(const char *type,const char *file, 
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *name);
    void startSubsection();
    void endSubsection();
    void startSubsubsection();
    void endSubsubsection();
    void startCenter()      { t << "{\\qc" << endl; }
    void endCenter()        { t << "}"; }
    void startSmall()       { t << "{\\sub "; }
    void endSmall()         { t << "}"; }

    void startMemberDescription(const char *,const char *);
    void endMemberDescription();
    void startMemberDeclaration() {} 
    void endMemberDeclaration(const char *,const char *) {}
    void writeInheritedSectionTitle(const char *,const char *,const char *,
                      const char *,const char *,const char *) {}
    void startDescList(SectionTypes);
    void startSimpleSect(SectionTypes,const char *,const char *,const char *);
    void endSimpleSect();
    void startParamList(ParamListTypes,const char *);
    void endParamList();
    //void writeDescItem();
    void startDescForItem();
    void endDescForItem();
    void startSection(const char *,const char *,SectionInfo::SectionType);
    void endSection(const char *,SectionInfo::SectionType);
    void addIndexItem(const char *,const char *);
    void startIndent();
    void endIndent();
    void writeSynopsis()     {}
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const char *filename,const char *name);
    void startPageRef();
    void endPageRef(const char *,const char *);
    void startQuickIndices() {}
    void endQuickIndices() {}
    void writeSplitBar(const char *) {}
    void writeNavigationPath(const char *) {}
    void writeLogo() {}
    void writeQuickLinks(bool,HighlightedItem,const char *) {}
    void writeSummaryLink(const char *,const char *,const char *,bool) {}
    void startContents() {}
    void endContents() {}
    void writeNonBreakableSpace(int);
	
    void startDescTable(const char *title);
    void endDescTable();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableData();
    void endDescTableData();
    
    void startDotGraph();
    void endDotGraph(const DotClassGraph &);
    void startInclDepGraph();
    void endInclDepGraph(const DotInclDepGraph &);
    void startGroupCollaboration();
    void endGroupCollaboration(const DotGroupCollaboration &g);
    void startCallGraph();
    void endCallGraph(const DotCallGraph &);
    void startDirDepGraph();
    void endDirDepGraph(const DotDirDeps &g);
    void writeGraphicalHierarchy(const DotGfxHierarchyTable &) {}

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
    void startParameterType(bool,const char *);
    void endParameterType();
    void startParameterName(bool) {}
    void endParameterName(bool,bool,bool) {}
    void startParameterList(bool) {}
    void endParameterList() {}
    void exceptionEntry(const char*,bool);

    void startConstraintList(const char  *);
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

    void startFontClass(const char *) {}
    void endFontClass() {}

    void writeCodeAnchor(const char *) {}
    void setCurrentDoc(Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}

    static bool preProcessFileInplace(const char *path,const char *name);
    
  private:
    RTFGenerator(const RTFGenerator &);
    RTFGenerator &operator=(const RTFGenerator &);

    const char *rtf_BList_DepthStyle();
    const char *rtf_CList_DepthStyle();
    const char *rtf_EList_DepthStyle();
    const char *rtf_LCList_DepthStyle();
    const char *rtf_DList_DepthStyle();
    const char *rtf_Code_DepthStyle();
    void incrementIndentLevel();
    void decrementIndentLevel();
    int  col;

    bool m_bstartedBody;  // has startbody been called yet?
    int  m_listLevel; // // RTF does not really have a addative indent...manually set list level.
    bool m_omitParagraph; // should a the next paragraph command be ignored?
    int  m_numCols; // number of columns in a table
    QCString relPath;

    void beginRTFDocument();
    void beginRTFChapter();
    void beginRTFSection();
    void rtfwriteRuler_doubleline();
    void rtfwriteRuler_emboss();
    void rtfwriteRuler_thick();
    void rtfwriteRuler_thin();
    void writeRTFReference(const char *label);
    //char *getMultiByte(int c);
};

#endif
