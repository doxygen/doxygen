/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "outputgen.h"

class HtmlCodeGenerator : public CodeOutputInterface
{
  public:
    HtmlCodeGenerator(TextStream &t,const QCString &relPath);
    HtmlCodeGenerator(TextStream &t);
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    void setRelativePath(const QCString &path);
    void codify(const QCString &text);
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip);
    void writeTooltip(const QCString &id,
                      const DocLinkInfo &docInfo,
                      const QCString &decl,
                      const QCString &desc,
                      const SourceLinkInfo &defInfo,
                      const SourceLinkInfo &declInfo
                     );
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int, bool);
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const QCString &s);
    void endFontClass();
    void writeCodeAnchor(const QCString &anchor);
    void setCurrentDoc(const Definition *,const QCString &,bool) {}
    void addWord(const QCString &,bool) {}
    void startCodeFragment(const QCString &style);
    void endCodeFragment(const QCString &);

  private:
    void _writeCodeLink(const QCString &className,
                        const QCString &ref,const QCString &file,
                        const QCString &anchor,const QCString &name,
                        const QCString &tooltip);
    void docify(const QCString &str);
    TextStream &m_t;
    int m_col = 0;
    QCString m_relPath;
    bool m_lineOpen = false;
    int m_id = 0;
};

/** Generator for HTML output */
class HtmlGenerator : public OutputGenerator
{
  public:
    HtmlGenerator();
    HtmlGenerator &operator=(const HtmlGenerator &g);
    HtmlGenerator(const HtmlGenerator &g);
    virtual ~HtmlGenerator();
    virtual std::unique_ptr<OutputGenerator> clone() const;

    virtual OutputType type() const { return Html; }
    static void init();
    void cleanup();
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t, const QCString &cssname);
    static void writeFooterFile(TextStream &t);
    static void writeTabData();
    static void writeSearchInfo(TextStream &t,const QCString &relPath);
    static void writeSearchData(const QCString &dir);
    static void writeSearchPage();
    static void writeExternalSearchPage();
    static QCString writeLogoAsString(const QCString &path);
    static QCString writeSplitBarAsString(const QCString &name,const QCString &relpath);
    static QCString getMathJaxMacros();

    // ---- CodeOutputInterface
    void codify(const QCString &text)
    { m_codeGen.codify(text); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
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

    void setCurrentDoc(const Definition *context,const QCString &anchor,bool isSourceFile);
    void addWord(const QCString &word,bool hiPriority);
    void writeDoc(DocNode *,const Definition *,const MemberDef *,int id);

    void startFile(const QCString &name,const QCString &manName,const QCString &title,int id);
    void writeFooter(const QCString &navPath);
    void endFile();
    void clearBuffer();
    void writeSearchInfo();

    void startIndexSection(IndexSections) {}
    void endIndexSection(IndexSections) {}
    void writePageLink(const QCString &,bool) {}
    void startProjectNumber();
    void endProjectNumber();
    void writeStyleInfo(int part);
    void startTitleHead(const QCString &);
    void endTitleHead(const QCString &,const QCString &);
    void startTitle() { m_t << "<div class=\"title\">"; }
    void endTitle() { m_t << "</div>"; }

    void startParagraph(const QCString &classDef);
    void endParagraph();
    void writeString(const QCString &text);
    void startIndexListItem();
    void endIndexListItem();
    void startIndexList();
    void endIndexList();
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const QCString &,bool);
    void startItemList()  { m_t << "<ul>\n"; }
    void endItemList()    { m_t << "</ul>\n"; }
    void startIndexItem(const QCString &ref,const QCString &file);
    void endIndexItem(const QCString &ref,const QCString &file);
    void docify(const QCString &text);

    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor,const QCString &name);

    void startTextLink(const QCString &file,const QCString &anchor);
    void endTextLink();
    void startHtmlLink(const QCString &url);
    void endHtmlLink();
    void startTypewriter() { m_t << "<code>"; }
    void endTypewriter()   { m_t << "</code>"; }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startItemListItem() { m_t << "<li>"; }
    void endItemListItem() { m_t << "</li>\n"; }

    void startMemberSections();
    void endMemberSections();
    void startHeaderSection();
    void endHeaderSection();
    void startMemberHeader(const QCString &, int);
    void endMemberHeader();
    void startMemberSubtitle();
    void endMemberSubtitle();
    void startMemberDocList();
    void endMemberDocList();
    void startMemberList();
    void endMemberList();
    void startInlineHeader();
    void endInlineHeader();
    void startAnonTypeScope(int) {}
    void endAnonTypeScope(int) {}
    void startMemberItem(const QCString &anchor,int,const QCString &inheritId);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams(const QCString &anchor,const QCString &inheritId);
    void startCompoundTemplateParams();
    void endCompoundTemplateParams();

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign(bool);
    void insertMemberAlignLeft(int,bool);
    void startMemberDescription(const QCString &anchor,const QCString &inheritId, bool typ);
    void endMemberDescription();
    void startMemberDeclaration() {}
    void endMemberDeclaration(const QCString &anchor,const QCString &inheritId);
    void writeInheritedSectionTitle(const QCString &id,   const QCString &ref,
                                    const QCString &file, const QCString &anchor,
                                    const QCString &title,const QCString &name);

    void writeRuler()    { m_t << "<hr/>"; }
    void writeAnchor(const QCString &,const QCString &name)
                         { m_t << "<a name=\"" << name <<"\" id=\"" << name << "\"></a>"; }
    void startEmphasis() { m_t << "<em>";  }
    void endEmphasis()   { m_t << "</em>"; }
    void startBold()     { m_t << "<b>"; }
    void endBold()       { m_t << "</b>"; }
    void startDescription() { m_t << "\n<dl>\n"; }
    void endDescription()   { m_t << "\n</dl>\n\n"; }
    void startDescItem()    { m_t << "<dt>"; }
    void endDescItem()      { m_t << "</dt>"; }
    void startDescForItem() { m_t << "<dd>"; }
    void endDescForItem()   { m_t << "</dd>\n"; }
    void lineBreak(const QCString &style);
    void writeChar(char c);
    void startMemberDoc(const QCString &clName, const QCString &memName,
                        const QCString &anchor, const QCString &title,
                        int memCount, int memTotal, bool showInline);
    void endMemberDoc(bool);
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor,const QCString &name,
                         const QCString &args);
    void endDoxyAnchor(const QCString &fName,const QCString &anchor);
    void writeLatexSpacing() {}
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name);
    void writeEndAnnoItem(const QCString &) { m_t << "\n"; }
    void startSubsection()    { m_t << "<h2>"; }
    void endSubsection()      { m_t << "</h2>\n"; }
    void startSubsubsection() { m_t << "<h3>"; }
    void endSubsubsection()   { m_t << "</h3>\n"; }
    void startCenter()        { m_t << "<center>\n"; }
    void endCenter()          { m_t << "</center>\n"; }
    void startSmall()         { m_t << "<small>\n"; }
    void endSmall()           { m_t << "</small>\n"; }
    void startExamples();
    void endExamples();
    void startParamList(ParamListTypes,const QCString &);
    void endParamList();
    void startSection(const QCString &,const QCString &,SectionType);
    void endSection(const QCString &,SectionType);
    void addIndexItem(const QCString &,const QCString &);
    void startIndent();
    void endIndent();
    void writeSynopsis() {}
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &);
    void startPageRef() {}
    void endPageRef(const QCString &,const QCString &) {}
    void startQuickIndices() {}
    void endQuickIndices();
    void writeSplitBar(const QCString &name);
    void writeNavigationPath(const QCString &s);
    void writeLogo();
    void writeQuickLinks(bool compact,HighlightedItem hli,const QCString &file);
    void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first);
    void startContents();
    void endContents();
    void startPageDoc(const QCString &pageTitle);
    void endPageDoc();
    void writeNonBreakableSpace(int);

    void startDescTable(const QCString &title);
    void endDescTable();
    void startDescTableRow();
    void endDescTableRow();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableData();
    void endDescTableData();

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
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g);

    void startTextBlock(bool)
    { m_t << "<div class=\"textblock\">"; }
    void endTextBlock(bool)
    { m_t << "</div>"; }
    void lastIndexPage() {}

    void startMemberDocPrefixItem();
    void endMemberDocPrefixItem();
    void startMemberDocName(bool);
    void endMemberDocName();
    void startParameterType(bool first,const QCString &key);
    void endParameterType();
    void startParameterName(bool);
    void endParameterName(bool last,bool emptyList,bool closeBracket);
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

  private:
    static void writePageFooter(TextStream &t,const QCString &,const QCString &,const QCString &);
    QCString m_lastTitle;
    QCString m_lastFile;
    QCString m_relPath;
    void docify(const QCString &text,bool inHtmlComment);

    int m_sectionCount = 0;
    bool m_emptySection = false;
    HtmlCodeGenerator m_codeGen;
};

#endif
