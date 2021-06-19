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
*/

#ifndef DOCBOOKGEN_H
#define DOCBOOKGEN_H

#include <iostream>

#include "config.h"
#include "outputgen.h"

class DocbookCodeGenerator : public CodeOutputInterface
{
  public:
    DocbookCodeGenerator(TextStream &t);
    virtual ~DocbookCodeGenerator();
    void setRelativePath(const QCString &path) { m_relPath = path; }
    void setSourceFileName(const QCString &sourceFileName) { m_sourceFileName = sourceFileName; }
    QCString sourceFileName() { return m_sourceFileName; }

    void codify(const QCString &text);
    void writeCodeLink(const QCString &ref,const QCString &file,
        const QCString &anchor,const QCString &name,
        const QCString &tooltip);
    void writeCodeLinkLine(const QCString &ref,const QCString &file,
        const QCString &anchor,const QCString &name,
        const QCString &tooltip);
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                      const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                     );
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const QCString &colorClass);
    void endFontClass();
    void writeCodeAnchor(const QCString &);
    void writeLineNumber(const QCString &extRef,const QCString &compId,
        const QCString &anchorId,int l);
    void setCurrentDoc(const Definition *,const QCString &,bool);
    void addWord(const QCString &,bool);
    void finish();
    void startCodeFragment(const QCString &style);
    void endCodeFragment(const QCString &style);

  private:
    TextStream &m_t;
    QCString m_refId;
    QCString m_external;
    int m_lineNumber = -1;
    int m_col = 0;
    bool m_insideCodeLine = false;
    bool m_insideSpecialHL = false;
    QCString m_relPath;
    QCString m_sourceFileName;
    bool m_prettyCode = Config_getBool(DOCBOOK_PROGRAMLISTING);
};


#if 0
// define for cases that have been implemented with an empty body
#define DB_GEN_EMPTY  t << "<!-- DBG_GEN_head_check " << __LINE__ << " -->\n";
#else
#define DB_GEN_EMPTY
#endif

#if 0
// Generic debug statements
#define DB_GEN_H DB_GEN_H1(t)
#define DB_GEN_H1(x) x << "<!-- DBG_GEN_head " << __LINE__ << " -->\n";
#define DB_GEN_H2(y) DB_GEN_H2a(t,y)
#define DB_GEN_H2a(x,y) x << "<!-- DBG_GEN_head " << __LINE__ << " " << y << " -->\n";
// define for cases that have NOT yet been implemented / considered
#define DB_GEN_NEW fprintf(stderr,"DBG_GEN_head %d\n",__LINE__); DB_GEN_H
#else
#define DB_GEN_H
#define DB_GEN_H1(x)
#define DB_GEN_H2(y)
#define DB_GEN_H2a(x,y)
#define DB_GEN_NEW
#endif

class DocbookGenerator : public OutputGenerator
{
  public:
    DocbookGenerator();
    DocbookGenerator(const DocbookGenerator &o);
    DocbookGenerator &operator=(const DocbookGenerator &o);
    virtual ~DocbookGenerator();
    virtual std::unique_ptr<OutputGenerator> clone() const;

    static void init();
    void cleanup();

    OutputType type() const { return Docbook; }

    // --- CodeOutputInterface
    void codify(const QCString &text)
    { m_codeGen.codify(text); }
    void writeCodeLink(const QCString &ref, const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { m_codeGen.writeCodeLink(ref,file,anchor,name,tooltip); }
    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,int lineNumber)
    { m_codeGen.writeLineNumber(ref,file,anchor,lineNumber); }
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

    void writeDoc(DocNode *,const Definition *ctx,const MemberDef *md,int id);

    ///////////////////////////////////////////////////////////////
    // structural output interface
    ///////////////////////////////////////////////////////////////
    void startFile(const QCString &name,const QCString &manName,
                           const QCString &title,int id);
    void writeSearchInfo(){DB_GEN_EMPTY};
    void writeFooter(const QCString &){DB_GEN_NEW};
    void endFile();
    void startIndexSection(IndexSections);
    void endIndexSection(IndexSections);
    void writePageLink(const QCString &,bool);
    void startProjectNumber(){DB_GEN_NEW};
    void endProjectNumber(){DB_GEN_NEW};
    void writeStyleInfo(int){DB_GEN_EMPTY};
    void startTitleHead(const QCString &);
    void endTitleHead(const QCString &fileName,const QCString &name);
    void startIndexListItem(){DB_GEN_NEW};
    void endIndexListItem(){DB_GEN_NEW};
    void startIndexList(){DB_GEN_NEW};
    void endIndexList(){DB_GEN_NEW};
    void startIndexKey(){DB_GEN_NEW};
    void endIndexKey(){DB_GEN_NEW};
    void startIndexValue(bool){DB_GEN_NEW};
    void endIndexValue(const QCString &,bool){DB_GEN_NEW};
    void startItemList()  {DB_GEN_EMPTY};
    void endItemList()    {DB_GEN_EMPTY};

    void startIndexItem(const QCString &,const QCString &){DB_GEN_NEW};
    void endIndexItem(const QCString &,const QCString &){DB_GEN_NEW};
    void startItemListItem() {DB_GEN_EMPTY};
    void endItemListItem() {DB_GEN_EMPTY};
    void docify(const QCString &text);
    void writeChar(char);
    void writeString(const QCString &);
    void startParagraph(const QCString &);
    void endParagraph();
    void writeObjectLink(const QCString &,const QCString &,const QCString &,const QCString &);
    void startHtmlLink(const QCString &){DB_GEN_NEW};
    void endHtmlLink(){DB_GEN_NEW};
    void startBold();
    void endBold();
    void startTypewriter();
    void endTypewriter();
    void startEmphasis(){DB_GEN_NEW};
    void endEmphasis(){DB_GEN_NEW};
    void writeRuler();
    void startDescription(){DB_GEN_NEW};
    void endDescription(){DB_GEN_NEW};
    void startDescItem(){DB_GEN_NEW};
    void startDescForItem(){DB_GEN_EMPTY};
    void endDescForItem(){DB_GEN_EMPTY};
    void endDescItem(){DB_GEN_NEW};
    void startCenter(){DB_GEN_NEW};
    void endCenter(){DB_GEN_NEW};
    void startSmall(){DB_GEN_NEW};
    void endSmall(){DB_GEN_NEW};
    void startExamples();
    void endExamples();
    void startParamList(BaseOutputDocInterface::ParamListTypes,const QCString &){DB_GEN_NEW};
    void endParamList(){DB_GEN_NEW};
    void startTitle(){DB_GEN_NEW};
    void endTitle(){DB_GEN_NEW};
    void writeAnchor(const QCString &,const QCString &){DB_GEN_EMPTY};
    void startSection(const QCString &,const QCString &,SectionType);
    void endSection(const QCString &,SectionType);
    void lineBreak(const QCString &);
    void addIndexItem(const QCString &,const QCString &);
    void writeNonBreakableSpace(int);
    void startDescTable(const QCString &);
    void endDescTable();
    void startDescTableRow();
    void endDescTableRow();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableData();
    void endDescTableData();
    void startTextLink(const QCString &,const QCString &){DB_GEN_NEW};
    void endTextLink(){DB_GEN_NEW};
    void startPageRef(){DB_GEN_NEW};
    void endPageRef(const QCString &,const QCString &){DB_GEN_NEW};
    void startSubsection(){DB_GEN_NEW};
    void endSubsection(){DB_GEN_NEW};
    void startSubsubsection();
    void endSubsubsection();


    void startGroupHeader(int);
    void endGroupHeader(int);
    void startMemberSections(){DB_GEN_EMPTY};
    void endMemberSections(){DB_GEN_EMPTY};
    void startHeaderSection(){DB_GEN_EMPTY};
    void endHeaderSection(){DB_GEN_EMPTY};
    void startMemberHeader(const QCString &anchor, int typ);
    void endMemberHeader();
    void startMemberSubtitle(){DB_GEN_EMPTY};
    void endMemberSubtitle(){DB_GEN_EMPTY};
    void startMemberDocList();
    void endMemberDocList();
    void startMemberList();
    void endMemberList();
    void startInlineHeader(){DB_GEN_NEW};
    void endInlineHeader(){DB_GEN_NEW};
    void startAnonTypeScope(int){DB_GEN_EMPTY};
    void endAnonTypeScope(int){DB_GEN_EMPTY};
    void startMemberItem(const QCString &,int,const QCString &);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams(const QCString &,const QCString &);
    void startCompoundTemplateParams() { startSubsubsection(); }
    void endCompoundTemplateParams() { endSubsubsection(); }
    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs(){DB_GEN_EMPTY};
    void endMemberGroupDocs(){DB_GEN_EMPTY};
    void startMemberGroup();
    void endMemberGroup(bool);
    void insertMemberAlign(bool){DB_GEN_EMPTY};
    void insertMemberAlignLeft(int,bool){DB_GEN_EMPTY};
    void startMemberDoc(const QCString &,const QCString &,
                        const QCString &,const QCString &,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor,const QCString &name,
                         const QCString &args);
    void endDoxyAnchor(const QCString &fileName,const QCString &anchor);
    void writeLatexSpacing(){DB_GEN_EMPTY}
    void writeStartAnnoItem(const QCString &,const QCString &,
                            const QCString &,const QCString &){DB_GEN_NEW};
    void writeEndAnnoItem(const QCString &){DB_GEN_NEW};
    void startMemberDescription(const QCString &,const QCString &,bool){DB_GEN_EMPTY};
    void endMemberDescription(){DB_GEN_EMPTY};
    void startMemberDeclaration(){DB_GEN_EMPTY};
    void endMemberDeclaration(const QCString &,const QCString &){DB_GEN_EMPTY};
    void writeInheritedSectionTitle(const QCString &,const QCString &,
                                    const QCString &,const QCString &,
                                    const QCString &,const QCString &){DB_GEN_NEW};
    void startIndent(){DB_GEN_EMPTY};
    void endIndent(){DB_GEN_EMPTY};
    void writeSynopsis(){DB_GEN_EMPTY};
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &);
    void startDotGraph();
    void endDotGraph(DotClassGraph &g);
    void startInclDepGraph();
    void endInclDepGraph(DotInclDepGraph &g);
    void startGroupCollaboration();
    void endGroupCollaboration(DotGroupCollaboration &g);
    void startCallGraph();
    void endCallGraph(DotCallGraph &g);
    void startDirDepGraph();
    void endDirDepGraph(DotDirDeps &g);
    void writeGraphicalHierarchy(DotGfxHierarchyTable &){DB_GEN_NEW};
    void startQuickIndices(){DB_GEN_EMPTY};
    void endQuickIndices(){DB_GEN_EMPTY};
    void writeSplitBar(const QCString &){DB_GEN_EMPTY};
    void writeNavigationPath(const QCString &){DB_GEN_NEW};
    void writeLogo(){DB_GEN_NEW};
    void writeQuickLinks(bool,HighlightedItem,const QCString &){DB_GEN_EMPTY};
    void writeSummaryLink(const QCString &,const QCString &,const QCString &,bool){DB_GEN_EMPTY};
    void startContents(){DB_GEN_EMPTY};
    void endContents(){DB_GEN_EMPTY};
    void startPageDoc(const QCString &){DB_GEN_EMPTY}
    void endPageDoc() {DB_GEN_EMPTY}
    void startTextBlock(bool);
    void endTextBlock(bool);
    void lastIndexPage(){DB_GEN_EMPTY};
    void startMemberDocPrefixItem();
    void endMemberDocPrefixItem();
    void startMemberDocName(bool);
    void endMemberDocName();
    void startParameterType(bool,const QCString &){DB_GEN_EMPTY};
    void endParameterType(){DB_GEN_EMPTY};
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
    void writeLabel(const QCString &,bool);
    void endLabels();

    void setCurrentDoc(const Definition *,const QCString &,bool) {DB_GEN_EMPTY}
    void addWord(const QCString &,bool) {DB_GEN_EMPTY}

private:

    QCString relPath;
    DocbookCodeGenerator m_codeGen;
    bool m_prettyCode = Config_getBool(DOCBOOK_PROGRAMLISTING);
    bool m_denseText = false;
    bool m_inGroup = false;
    int  m_levelListItem = 0;
    bool m_inListItem[20] = { false, };
    bool m_inSimpleSect[20] = { false, };
    bool m_descTable = false;
    bool m_simpleTable = false;
    int m_inLevel = -1;
    bool m_firstMember = false;
    int m_openSection = 0;
};

#endif
