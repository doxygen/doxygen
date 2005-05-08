/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#include "qtbc.h"
#include "outputgen.h"

#define PREFRAG_START "<div class=\"fragment\"><pre class=\"fragment\">"
#define PREFRAG_END   "</pre></div>"

class QFile;

class HtmlGenerator : public OutputGenerator
{
  public:
    HtmlGenerator();
    virtual ~HtmlGenerator();
    static void init();
    static void writeStyleSheetFile(QFile &f);
    static void writeHeaderFile(QFile &f);
    static void writeFooterFile(QFile &f);
   
    void enable() 
    { if (genStack->top()) active=*genStack->top(); else active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==Html) active=TRUE;  }
    void disableIf(OutputType o) { if (o==Html) active=FALSE; }
    void disableIfNot(OutputType o) { if (o!=Html) active=FALSE; }
    bool isEnabled(OutputType o) { return (o==Html && active); } 
    OutputGenerator *get(OutputType o) { return (o==Html) ? this : 0; }

    void printDoc(DocNode *);

    void startFile(const char *name,const char *manName,const char *title);
    void writeFooter();
    void endFile();
    void clearBuffer();

    void startIndexSection(IndexSections) {}
    void endIndexSection(IndexSections) {}
    void startProjectNumber();
    void endProjectNumber();
    void writeStyleInfo(int part);
    void startTitleHead(const char *) { startTitle(); }
    void endTitleHead(const char *,const char *) { endTitle(); }
    void startTitle() { t << "<h1>"; }
    void endTitle() { t << "</h1>"; }
    
    void newParagraph();
    void writeString(const char *text);
    void startIndexList();
    void endIndexList();
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const char *,bool);
    void startItemList()  { t << "<ul>"  << endl; }
    void endItemList()    { t << "</ul>" << endl; }
    void writeIndexItem(const char *ref,const char *file,const char *name);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name);
    void startTextLink(const char *file,const char *anchor);
    void endTextLink();
    void startHtmlLink(const char *url);
    void endHtmlLink();
    void startTypewriter() { t << "<code>"; }
    void endTypewriter()   { t << "</code>"; }
    void startGroupHeader();
    void endGroupHeader();
    void writeListItem() { t << "<li>"; }

    void startMemberSections();
    void endMemberSections();
    void startMemberHeader();
    void endMemberHeader();
    void startMemberSubtitle();
    void endMemberSubtitle();
    void startMemberList();
    void endMemberList();
    void startAnonTypeScope(int) {}
    void endAnonTypeScope(int) {}
    void startMemberItem(int);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams();

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign(bool);
    void startMemberDescription();
    void endMemberDescription();

    void writeRuler()    { t << "<hr>"; }
    void writeAnchor(const char *,const char *name) 
                         { t << "<a name=\"" << name <<"\"></a>"; }
    void startCodeFragment() { t << PREFRAG_START; }
    void endCodeFragment()   { t << PREFRAG_END; } 
    void writeLineNumber(const char *,const char *,const char *,int);
    void startCodeLine() { col=0; }
    void endCodeLine()   { codify("\n"); }
    void startEmphasis() { t << "<em>";  }
    void endEmphasis()   { t << "</em>"; }
    void startBold()     { t << "<b>"; }
    void endBold()       { t << "</b>"; }
    void startDescription() { t << endl << "<dl compact>" << endl; }
    void endDescription()   { t << endl << "</dl>" << endl; }
    void startDescItem()    { t << "<dt>"; }
    void endDescItem()      { t << "<dd>"; }
    void lineBreak() { t << "<br>" << endl; }
    void writeChar(char c);
    void startMemberDoc(const char *,const char *,const char *,const char *);
    void endMemberDoc(bool); 
    void startDoxyAnchor(const char *fName,const char *manName,
                         const char *anchor,const char *name,
                         const char *args);
    void endDoxyAnchor(const char *fName,const char *anchor);
    void startCodeAnchor(const char *label) { t << "<a name=\"" << label << "\"></a>"; }
    void endCodeAnchor() { }
    void writeLatexSpacing() {}
    void writeStartAnnoItem(const char *type,const char *file,
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *) { t << endl; }
    void startSubsection()    { t << "<h2>"; }
    void endSubsection()      { t << "</h2>" << endl; }
    void startSubsubsection() { t << "<h3>"; }
    void endSubsubsection()   { t << "</h3>" << endl; }
    void startCenter()        { t << "<center>" << endl; }
    void endCenter()          { t << "</center>" << endl; }
    void startSmall()         { t << "<small>" << endl; }
    void endSmall()           { t << "</small>" << endl; }
    void startDescList(SectionTypes)      { t << "<dl compact><dt><b>" << endl; }
    void endDescList()        { t << "</dl>"; }
    void startSimpleSect(SectionTypes,const char *,const char *,const char *);
    void endSimpleSect();
    void startParamList(ParamListTypes,const char *);
    void endParamList();
    void writeDescItem()      { t << "<dd>" << endl; }
    void startSection(const char *,const char *,SectionInfo::SectionType);
    void endSection(const char *,SectionInfo::SectionType);
    void addIndexItem(const char *,const char *);
    void startIndent();
    void endIndent();
    void writeSynopsis() {}
    void startClassDiagram();
    void endClassDiagram(ClassDiagram &,const char *,const char *);
    void startPageRef() {}
    void endPageRef(const char *,const char *) {}
    //void startQuickIndexItem(const char *,const char *);
    //void endQuickIndexItem();
    void writeQuickLinks(bool compact,HighlightedItem hli);
    void writeNonBreakableSpace(int);
    
    void startDescTable()
    { t << "<table border=\"0\" cellspacing=\"2\" cellpadding=\"0\">" << endl; }
    void endDescTable()
    { t << "</table>" << endl; }
    void startDescTableTitle()
    { t << "<tr><td valign=\"top\"><em>"; }
    void endDescTableTitle()
    { t << "</em>&nbsp;</td>"; }
    void startDescTableData()
    { t << "<td>" << endl; }
    void endDescTableData()
    { t << "</td></tr>" << endl; }
    
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

    void startTextBlock(bool) {}
    void endTextBlock(bool) {}
    void lastIndexPage() {}

    void startMemberDocPrefixItem();
    void endMemberDocPrefixItem();
    void startMemberDocName(bool);
    void endMemberDocName();
    void startParameterType(bool first,const char *key);
    void endParameterType();
    void startParameterName(bool);
    void endParameterName(bool last,bool emptyList,bool closeBracket);
    void startParameterList(bool);
    void endParameterList();

    void startFontClass(const char *s) { t << "<span class=\"" << s << "\">"; }
    void endFontClass() { t << "</span>"; }

    void writeCodeAnchor(const char *anchor) 
    { t << "<a name=\"" << anchor << "\"></a>"; }
    static void writeSearchPage();

  private:
    QCString lastTitle;
    QCString lastFile;
    QCString relPath;

    HtmlGenerator &operator=(const HtmlGenerator &g);
    HtmlGenerator(const HtmlGenerator &g);

    int col;
};

#endif
