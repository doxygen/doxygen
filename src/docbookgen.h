/******************************************************************************
*
* Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#include <memory>
#include <array>

#include "config.h"
#include "outputgen.h"

#if 0
// define for cases that have been implemented with an empty body
#define DB_GEN_EMPTY  m_t << "<!-- DBG_GEN_head_check " << __LINE__ << " -->\n";
#else
#define DB_GEN_EMPTY
#endif

#if 0
// Generic debug statements
#define DB_GEN_H DB_GEN_H1(m_t)
#define DB_GEN_H1(x) x << "<!-- DBG_GEN_head " << __LINE__ << " -->\n";
#define DB_GEN_H2(y) DB_GEN_H2a(m_t,y)
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

class OutputCodeList;

class DocbookCodeGenerator
{
  public:
    DocbookCodeGenerator(TextStream *t);
    void setTextStream(TextStream *t) { m_t = t; }

    OutputType type() const { return OutputType::Docbook; }

    void codify(const QCString &text);
    void writeCodeLink(CodeSymbolType type,
        const QCString &ref,const QCString &file,
        const QCString &anchor,const QCString &name,
        const QCString &tooltip);
    void writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                      const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                     );
    void startCodeLine(int);
    void endCodeLine();
    void startFontClass(const QCString &colorClass);
    void endFontClass();
    void writeCodeAnchor(const QCString &);
    void writeLineNumber(const QCString &extRef,const QCString &compId,
        const QCString &anchorId,int l, bool writeLineAnchor);
    void startCodeFragment(const QCString &style);
    void endCodeFragment(const QCString &style);
    void startFold(int,const QCString &,const QCString &) {}
    void endFold() {}

    void setRelativePath(const QCString &path) { m_relPath = path; }
    void setSourceFileName(const QCString &sourceFileName) { m_sourceFileName = sourceFileName; }
    QCString sourceFileName() { return m_sourceFileName; }
    void finish();

  private:
    void writeCodeLinkLine(CodeSymbolType type,
        const QCString &ref,const QCString &file,
        const QCString &anchor,const QCString &name,
        const QCString &tooltip, bool);
    TextStream *m_t;
    QCString    m_refId;
    QCString    m_external;
    int         m_lineNumber = -1;
    size_t      m_col = 0;
    bool        m_insideCodeLine = false;
    bool        m_insideSpecialHL = false;
    QCString    m_relPath;
    QCString    m_sourceFileName;
};

class DocbookGenerator : public OutputGenerator
{
  public:
    DocbookGenerator();
    DocbookGenerator(const DocbookGenerator &);
    DocbookGenerator &operator=(const DocbookGenerator &);
    DocbookGenerator(DocbookGenerator &&);
    DocbookGenerator &operator=(DocbookGenerator &&) = delete;
   ~DocbookGenerator();

    static void init();
    void cleanup();

    OutputType type() const { return OutputType::Docbook; }

    void writeDoc(const IDocNodeAST *node,const Definition *ctx,const MemberDef *md,int id);

    ///////////////////////////////////////////////////////////////
    // structural output interface
    ///////////////////////////////////////////////////////////////
    void startFile(const QCString &name,const QCString &manName,
                           const QCString &title,int id,int hierarchyLevel);
    void writeSearchInfo(){DB_GEN_EMPTY};
    void writeFooter(const QCString &){DB_GEN_NEW};
    void endFile();
    void startIndexSection(IndexSection);
    void endIndexSection(IndexSection);
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
    void startBold();
    void endBold();
    void startTypewriter();
    void endTypewriter();
    void startEmphasis(){DB_GEN_NEW};
    void endEmphasis(){DB_GEN_NEW};
    void writeRuler();
    void startDescForItem(){DB_GEN_EMPTY};
    void endDescForItem(){DB_GEN_EMPTY};
    void startCenter(){DB_GEN_NEW};
    void endCenter(){DB_GEN_NEW};
    void startSmall(){DB_GEN_NEW};
    void endSmall(){DB_GEN_NEW};
    void startExamples();
    void endExamples();
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
    void startMemberItem(const QCString &,MemberItemType,const QCString &);
    void endMemberItem(MemberItemType);
    void startMemberTemplateParams();
    void endMemberTemplateParams(const QCString &,const QCString &);
    void startCompoundTemplateParams();
    void endCompoundTemplateParams();
    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs(){DB_GEN_EMPTY};
    void endMemberGroupDocs(){DB_GEN_EMPTY};
    void startMemberGroup();
    void endMemberGroup(bool);
    void insertMemberAlign(bool){DB_GEN_EMPTY};
    void insertMemberAlignLeft(MemberItemType,bool){DB_GEN_EMPTY};
    void startMemberDoc(const QCString &,const QCString &,
                        const QCString &,const QCString &,int,int,bool);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor,const QCString &name,
                         const QCString &args);
    void endDoxyAnchor(const QCString &fileName,const QCString &anchor);
    void addLabel(const QCString &,const QCString &);
    void writeLatexSpacing(){DB_GEN_EMPTY}
    void writeStartAnnoItem(const QCString &,const QCString &,
                            const QCString &,const QCString &){DB_GEN_NEW};
    void startMemberDescription(const QCString &,const QCString &,bool){DB_GEN_EMPTY};
    void endMemberDescription(){DB_GEN_EMPTY};
    void startMemberDeclaration(){DB_GEN_EMPTY};
    void endMemberDeclaration(const QCString &,const QCString &){DB_GEN_EMPTY};
    void writeInheritedSectionTitle(const QCString &,const QCString &,
                                    const QCString &,const QCString &,
                                    const QCString &,const QCString &);
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
    void writeQuickLinks(HighlightedItem,const QCString &){DB_GEN_EMPTY};
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
    void endParameterName();
    void startParameterExtra();
    void endParameterExtra(bool,bool,bool);
    void startParameterDefVal(const char *sep);
    void endParameterDefVal();
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

    void writeLocalToc(const SectionRefs &sr,const LocalToc &lt);

    void setCurrentDoc(const Definition *,const QCString &,bool) {DB_GEN_EMPTY}
    void addWord(const QCString &,bool) {DB_GEN_EMPTY}

    void addCodeGen(OutputCodeList &list);
private:
    void openSection(const QCString &attr=QCString());
    void closeSection();
    void closeAllSections();

    QCString relPath;
    std::unique_ptr<OutputCodeList>  m_codeList;
    DocbookCodeGenerator *m_codeGen = nullptr;
    bool m_denseText = false;
    bool m_inGroup = false;
    int  m_levelListItem = 0;
    std::array<bool,20> m_inListItem = { false, };
    std::array<bool,20> m_inSimpleSect = { false, };
    bool m_descTable = false;
    bool m_simpleTable = false;
    int m_inLevel = -1;
    bool m_firstMember = false;
    int m_openSectionCount = 0;
    QCString m_pageLinks;
};

QCString convertToDocBook(const QCString &s, const bool retainNewline = false);


#endif
