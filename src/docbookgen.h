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

#include "config.h"
#include "outputgen.h"

class DocbookCodeGenerator : public CodeOutputInterface
{
  public:
    DocbookCodeGenerator(FTextStream &t);
    DocbookCodeGenerator();
    virtual ~DocbookCodeGenerator();
    void setTextStream(FTextStream &t)
    {
      m_streamSet = t.device()!=0;
      m_t.setDevice(t.device());
    }
    void setRelativePath(const QCString &path) { m_relPath = path; }
    void setSourceFileName(const QCString &sourceFileName) { m_sourceFileName = sourceFileName; }
    QCString sourceFileName() { return m_sourceFileName; }

    void codify(const char *text);
    void writeCodeLink(const char *ref,const char *file,
        const char *anchor,const char *name,
        const char *tooltip);
    void writeCodeLinkLine(const char *ref,const char *file,
        const char *anchor,const char *name,
        const char *tooltip);
    void writeTooltip(const char *, const DocLinkInfo &, const char *,
                      const char *, const SourceLinkInfo &, const SourceLinkInfo &
                     );
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const char *colorClass);
    void endFontClass();
    void writeCodeAnchor(const char *);
    void writeLineNumber(const char *extRef,const char *compId,
        const char *anchorId,int l);
    void setCurrentDoc(const Definition *,const char *,bool);
    void addWord(const char *,bool);
    void finish();
    void startCodeFragment(const char *style);
    void endCodeFragment(const char *style);

  private:
    FTextStream m_t;
    bool m_streamSet = false;
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

    OutputType type() const { return Docbook; }

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

    void writeDoc(DocNode *,const Definition *ctx,const MemberDef *md,int id);

    ///////////////////////////////////////////////////////////////
    // structural output interface
    ///////////////////////////////////////////////////////////////
    void startFile(const char *name,const char *manName,
                           const char *title,int id);
    void writeSearchInfo(){DB_GEN_EMPTY};
    void writeFooter(const char *){DB_GEN_NEW};
    void endFile();
    void startIndexSection(IndexSections);
    void endIndexSection(IndexSections);
    void writePageLink(const char *,bool);
    void startProjectNumber(){DB_GEN_NEW};
    void endProjectNumber(){DB_GEN_NEW};
    void writeStyleInfo(int){DB_GEN_EMPTY};
    void startTitleHead(const char *);
    void endTitleHead(const char *fileName,const char *name);
    void startIndexListItem(){DB_GEN_NEW};
    void endIndexListItem(){DB_GEN_NEW};
    void startIndexList(){DB_GEN_NEW};
    void endIndexList(){DB_GEN_NEW};
    void startIndexKey(){DB_GEN_NEW};
    void endIndexKey(){DB_GEN_NEW};
    void startIndexValue(bool){DB_GEN_NEW};
    void endIndexValue(const char *,bool){DB_GEN_NEW};
    void startItemList()  {DB_GEN_EMPTY};
    void endItemList()    {DB_GEN_EMPTY};

    void startIndexItem(const char *,const char *){DB_GEN_NEW};
    void endIndexItem(const char *,const char *){DB_GEN_NEW};
    void startItemListItem() {DB_GEN_EMPTY};
    void endItemListItem() {DB_GEN_EMPTY};
    void docify(const char *text);
    void writeChar(char);
    void writeString(const char *);
    void startParagraph(const char *);
    void endParagraph();
    void writeObjectLink(const char *,const char *,const char *,const char *);
    void startHtmlLink(const char *){DB_GEN_NEW};
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
    void startParamList(BaseOutputDocInterface::ParamListTypes,const char *){DB_GEN_NEW};
    void endParamList(){DB_GEN_NEW};
    void startTitle(){DB_GEN_NEW};
    void endTitle(){DB_GEN_NEW};
    void writeAnchor(const char *,const char *){DB_GEN_EMPTY};
    void startSection(const char *,const char *,SectionType);
    void endSection(const char *,SectionType);
    void lineBreak(const char *);
    void addIndexItem(const char *,const char *);
    void writeNonBreakableSpace(int);
    void startDescTable(const char *,const bool hasInits);
    void endDescTable();
    void startDescTableRow();
    void endDescTableRow();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableInit();
    void endDescTableInit();
    void startDescTableData();
    void endDescTableData();
    void startTextLink(const char *,const char *){DB_GEN_NEW};
    void endTextLink(){DB_GEN_NEW};
    void startPageRef(){DB_GEN_NEW};
    void endPageRef(const char *,const char *){DB_GEN_NEW};
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
    void startMemberHeader(const char *anchor, int typ);
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
    void startMemberItem(const char *,int,const char *);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams(const char *,const char *);
    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs(){DB_GEN_EMPTY};
    void endMemberGroupDocs(){DB_GEN_EMPTY};
    void startMemberGroup();
    void endMemberGroup(bool);
    void insertMemberAlign(bool){DB_GEN_EMPTY};
    void insertMemberAlignLeft(int,bool){DB_GEN_EMPTY};
    void startMemberDoc(const char *,const char *,
                        const char *,const char *,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const char *fName,const char *manName,
                         const char *anchor,const char *name,
                         const char *args);
    void endDoxyAnchor(const char *fileName,const char *anchor);
    void writeLatexSpacing(){DB_GEN_EMPTY}
    void writeStartAnnoItem(const char *,const char *,
                            const char *,const char *){DB_GEN_NEW};
    void writeEndAnnoItem(const char *){DB_GEN_NEW};
    void startMemberDescription(const char *,const char *,bool){DB_GEN_EMPTY};
    void endMemberDescription(){DB_GEN_EMPTY};
    void startMemberDeclaration(){DB_GEN_EMPTY};
    void endMemberDeclaration(const char *,const char *){DB_GEN_EMPTY};
    void writeInheritedSectionTitle(const char *,const char *,
                                    const char *,const char *,
                                    const char *,const char *){DB_GEN_NEW};
    void startIndent(){DB_GEN_EMPTY};
    void endIndent(){DB_GEN_EMPTY};
    void writeSynopsis(){DB_GEN_EMPTY};
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const char *,const char *);
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
    void writeSplitBar(const char *){DB_GEN_EMPTY};
    void writeNavigationPath(const char *){DB_GEN_NEW};
    void writeLogo(){DB_GEN_NEW};
    void writeQuickLinks(bool,HighlightedItem,const char *){DB_GEN_EMPTY};
    void writeSummaryLink(const char *,const char *,const char *,bool){DB_GEN_EMPTY};
    void startContents(){DB_GEN_EMPTY};
    void endContents(){DB_GEN_EMPTY};
    void startPageDoc(const char *){DB_GEN_EMPTY}
    void endPageDoc() {DB_GEN_EMPTY}
    void startTextBlock(bool);
    void endTextBlock(bool);
    void lastIndexPage(){DB_GEN_EMPTY};
    void startMemberDocPrefixItem();
    void endMemberDocPrefixItem();
    void startMemberDocName(bool);
    void endMemberDocName();
    void startParameterType(bool,const char *){DB_GEN_EMPTY};
    void endParameterType(){DB_GEN_EMPTY};
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

    void startMemberDocSimple(bool){DB_GEN_NEW};
    void endMemberDocSimple(bool){DB_GEN_NEW};
    void startInlineMemberType(){DB_GEN_NEW};
    void endInlineMemberType(){DB_GEN_NEW};
    void startInlineMemberName(){DB_GEN_NEW};
    void endInlineMemberName(){DB_GEN_NEW};
    void startInlineMemberDoc(){DB_GEN_NEW};
    void endInlineMemberDoc(){DB_GEN_NEW};

    void startLabels();
    void writeLabel(const char *,bool);
    void endLabels();

    void setCurrentDoc(const Definition *,const char *,bool) {DB_GEN_EMPTY}
    void addWord(const char *,bool) {DB_GEN_EMPTY}

private:

    QCString relPath;
    DocbookCodeGenerator m_codeGen;
    bool m_prettyCode = Config_getBool(DOCBOOK_PROGRAMLISTING);
    bool m_denseText = false;
    bool m_inGroup = false;
    bool m_inDetail = false;
    int  m_levelListItem = 0;
    bool m_inListItem[20] = { false, };
    bool m_inSimpleSect[20] = { false, };
    bool m_descTable = false;
    int m_inLevel = -1;
    bool m_firstMember = false;
};

#endif
