/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

class QFile;

class HtmlGenerator : public OutputGenerator
{
  public:
    HtmlGenerator();
   ~HtmlGenerator();
    static void init();
    static void writeStyleSheetFile(QFile &f);
    static void writeHeaderFile(QFile &f);
    static void writeFooterFile(QFile &f);
   
    OutputGenerator *copy() { return new HtmlGenerator; }
    //OutputGenerator *clone() { return new HtmlGenerator(*this); }
    void append(const OutputGenerator *o);
    void enable() { active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==Html) active=TRUE;  }
    void disableIf(OutputType o) { if (o==Html) active=FALSE; }
    void disableIfNot(OutputType o) { if (o!=Html) active=FALSE; }
    bool isEnabled(OutputType o) { return (o==Html && active); } 
    OutputGenerator *get(OutputType o) { return (o==Html) ? this : 0; }
    
    //void generateExternalIndex();
    void startFile(const char *name,const char *title,bool external);
    void writeFooter(int,bool);
    void endFile();
    void clearBuffer();

    //void writeIndex();
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
    void startItemList()  { t << "<ul>"  << endl; }
    void endItemList()    { t << "</ul>" << endl; }
    void startEnumList()  { t << "<ol>"  << endl; }
    void endEnumList()    { t << "</ol>" << endl; }
    void startAlphabeticalIndexList();
    void endAlphabeticalIndexList();
    void writeIndexHeading(const char *s);
    void writeIndexItem(const char *ref,const char *file,const char *name);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref,const char *f,
                       const char *anchor,const char *name);
    void startTextLink(const char *file,const char *anchor);
    void endTextLink();
    void writeHtmlLink(const char *url,const char *text);
    void writeMailLink(const char *url);
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
    void startMemberItem(int);

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign();
    void endMemberItem(bool);
    void startMemberDescription();
    void endMemberDescription();

    void writeRuler()    { t << "<hr>"; }
    void writeAnchor(const char *,const char *name) 
                         { t << "<a name=\"" << name <<"\"></a>"; }
    void startCodeFragment() { t << "<div class=\"fragment\"><pre>"; }
    void endCodeFragment()   { t << "</div></pre>"; } // <- I know this is 
                                                      //    ordered the wrong
                                                      //    way, but it is
                                                      //    the only way I know
                                                      //    to prevent an extra
                                                      //    blank line 
                                                      //    (with netscape)
    void startPreFragment() { t << "<pre>"; }
    void endPreFragment()   { t << "</pre>"; }
    void startCodeLine() { col=0; }
    void endCodeLine()   { codify("\n"); }
    //void writeBoldString(const char *text) 
    //                     { t << "<b>"; docify(text); t << "</b>"; }
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
    void endMemberDoc(); 
    //void writeDoxyAnchor(const char *fName,const char *clName,
    //                     const char *anchor,const char *name);
    void startDoxyAnchor(const char *fName,const char *anchor,const char *name);
    void endDoxyAnchor(const char *fName,const char *anchor);
    void startCodeAnchor(const char *label) { t << "<a name=\"" << label << "\"></a>"; }
    void endCodeAnchor() { }
    void writeLatexSpacing() {}
    //void writeLatexLabel(const char *,const char *) {}
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
    void startSubscript()     { t << "<sub>"; }
    void endSubscript()       { t << "</sub>"; }
    void startSuperscript()   { t << "<sup>"; }
    void endSuperscript()     { t << "</sup>"; }
    void startTable(int)      { t << "<table border=1 cellspacing=3 cellpadding=3>"; }
    void endTable()           { t << "</table>" << endl; }
    void nextTableRow()       { t << "<tr><td>"; }
    void endTableRow()        { t << "</tr>" << endl; }
    void nextTableColumn()    { t << "<td>"; }
    void endTableColumn()     { t << "</td>"; }
    void writeCopyright()     { t << "&copy;"; }
    void writeQuote()         { t << "&quot;"; }
    void writeUmlaut(char c)  { t << "&" << c << "uml;"; }
    void writeAcute(char c)   { t << "&" << c << "acute;"; }
    void writeGrave(char c)   { t << "&" << c << "grave;"; }
    void writeCirc(char c)    { t << "&" << c << "circ;"; }
    void writeTilde(char c)   { t << "&" << c << "tilde;"; } 
    void writeRing(char c)    { t << "&" << c << "ring;"; }
    void writeSharpS()        { t << "&szlig;"; }
    void startDescList()      { t << "<dl compact><dt>" << endl; }
    void endDescTitle()       {}
    void writeDescItem()      { t << "<dd>" << endl; }
    void endDescList()        { t << "</dl>"; }
    void startSection(const char *,const char *,bool);
    void endSection(const char *,bool);
    void writeSectionRef(const char *,const char *,const char *,const char *);
    void writeSectionRefItem(const char *,const char *,const char *);
    //void writeSectionRefAnchor(const char *,const char *,const char *);
    void addIndexItem(const char *,const char *) {}
    void startIndent();
    void endIndent();
    void writeSynopsis() {}
    void startClassDiagram();
    void endClassDiagram(ClassDiagram &,const char *,const char *);
    //void startColorFont(uchar r,uchar g,uchar b);
    //void endColorFont();
    void startPageRef() {}
    void endPageRef(const char *,const char *) {}
    void startQuickIndexItem(const char *,const char *);
    void endQuickIndexItem();
    void writeFormula(const char *,const char *);
    void writeNonBreakableSpace() { t << "&nbsp;&nbsp;&nbsp;"; }
    void startImage(const char *,const char *,bool);
    void endImage(bool);
    
    void startDescTable()
    { t << "<table border=0 cellspacing=2 cellpadding=0>" << endl; }
    void endDescTable()
    { t << "</table>" << endl; }
    void startDescTableTitle()
    { t << "<tr><td valign=top>"; }
    void endDescTableTitle()
    { t << endl << "&nbsp;</td>"; }
    void startDescTableData()
    { t << "<td>" << endl; }
    void endDescTableData()
    { t << "</td></tr>" << endl; }
    //static void docifyStatic(QTextStream &t,const char *str);
    
    void startDotGraph();
    void endDotGraph(DotClassGraph &g);
    void startInclDepGraph();
    void endInclDepGraph(DotInclDepGraph &g);
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g);

    void startTextBlock(bool) {}
    void endTextBlock() {}
    void lastIndexPage() {}

    void startMemberDocPrefixItem();
    void endMemberDocPrefixItem();
    void startMemberDocName();
    void endMemberDocName();
    void startParameter(bool first);
    void endParameter(bool last);

    void startFontClass(const char *s) { t << "<font class=\"" << s << "\">"; }
    void endFontClass() { t << "</font>"; }

  private:
    QCString header;
    QCString footer;
    QCString lastTitle;
    QCString lastFile;

    HtmlGenerator &operator=(const HtmlGenerator &g);
    HtmlGenerator(const HtmlGenerator &g);

    int col;
};

#endif
