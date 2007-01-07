/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

class ManGenerator : public OutputGenerator
{
  public:
    ManGenerator();
   ~ManGenerator();
    
    //OutputGenerator *copy()  { return new ManGenerator; } 
    //OutputGenerator *clone() { return new ManGenerator(*this); }
    //void append(const OutputGenerator *o);
    void enable() 
    { if (genStack->top()) active=*genStack->top(); else active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==Man) active=TRUE;  }
    void disableIf(OutputType o) { if (o==Man) active=FALSE; }
    void disableIfNot(OutputType o) { if (o!=Man) active=FALSE; }
    bool isEnabled(OutputType o) { return (o==Man && active); } 
    OutputGenerator *get(OutputType o) { return (o==Man) ? this : 0; }

    void printDoc(DocNode *,const char *);

    static void init();
    void startFile(const char *name,const char *manName,const char *title);
    void writeFooter() {}
    void endFile();
    void clearBuffer();

    //void writeIndex();
    void startIndexSection(IndexSections) {}
    void endIndexSection(IndexSections) {}
    void startProjectNumber() {}
    void endProjectNumber() {}
    void writeStyleInfo(int) {}
    void startTitleHead(const char *) {}
    void endTitleHead(const char *,const char *);
    void startTitle();
    void endTitle();
    
    void newParagraph();
    void writeString(const char *text);
    void startIndexList() {}
    void endIndexList()   { newParagraph(); } 
    void startIndexKey() {}
    void endIndexKey()   {} 
    void startIndexValue(bool) {}
    void endIndexValue(const char *,bool)   {} 
    void startItemList()  {}
    void endItemList()    { newParagraph(); }
    //void startEnumList()  {}
    //void endEnumList()    { newParagraph(); }
    //void startAlphabeticalIndexList() {}
    //void endAlphabeticalIndexList() {} 
    //void writeIndexHeading(const char *) {}
    void startIndexItem(const char *ref,const char *file);
    void endIndexItem(const char *ref,const char *file);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip);
    void startTextLink(const char *,const char *) {}
    void endTextLink() {}
    void startHtmlLink(const char *url);
    void endHtmlLink();
    //void writeMailLink(const char *url);
    void startTypewriter() { t << "\\fC"; firstCol=FALSE; }
    void endTypewriter()   { t << "\\fP"; firstCol=FALSE; }
    void startGroupHeader();
    void endGroupHeader();
    void startMemberSections() {}
    void endMemberSections() {}
    void startMemberHeader();
    void endMemberHeader();
    void insertMemberAlign(bool) {}
    void startMemberSubtitle() {}
    void endMemberSubtitle() {}
    void writeListItem();
    void startMemberDocList() {}
    void endMemberDocList() {}
    void startMemberList();
    void endMemberList();
    void startAnonTypeScope(int);
    void endAnonTypeScope(int);
    void startMemberItem(int);
    void endMemberItem();
    void startMemberTemplateParams() {}
    void endMemberTemplateParams() {}

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void writeRuler()    {}
    void writeAnchor(const char *,const char *) {}
    void startCodeFragment();
    void endCodeFragment();
    //void startPreFragment()      { startCodeFragment(); }
    //void endPreFragment()        { endCodeFragment(); }
    //void startVerbatimFragment() { startCodeFragment(); }
    //void endVerbatimFragment()   { endCodeFragment(); }
    void writeLineNumber(const char *,const char *,const char *,int l) { t << l << " "; }
    void startCodeLine() {}
    void endCodeLine() { codify("\n"); col=0; }
    //void writeBoldString(const char *text) 
    //                     { t << "\\fB"; docify(text); t << "\\fP"; firstCol=FALSE; }
    void startEmphasis() { t << "\\fI"; firstCol=FALSE; }
    void endEmphasis()   { t << "\\fP"; firstCol=FALSE; }
    void startBold()     { t << "\\fB"; firstCol=FALSE; }
    void endBold()       { t << "\\fP"; firstCol=FALSE; }
    void startDescription() {}
    void endDescription()   {}
    void startDescItem();
    void endDescItem();
    void lineBreak() { t << "\n.br" << endl; }
    void writeChar(char c);
    void startMemberDoc(const char *,const char *,const char *,const char *);
    void endMemberDoc(bool);
    void startDoxyAnchor(const char *,const char *,const char *,const char *,const char *);
    void endDoxyAnchor(const char *,const char *) {}
    void startCodeAnchor(const char *) {}
    void endCodeAnchor() {}
    void writeLatexSpacing() {}
    //void writeLatexLabel(const char *,const char *) {}
    void writeStartAnnoItem(const char *type,const char *file,
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *) { t << endl; firstCol=TRUE; }
    void startSubsection();
    void endSubsection();
    void startSubsubsection();
    void endSubsubsection();
    void startCenter()        {}
    void endCenter()          {}
    void startSmall()         {}
    void endSmall()           {}
    //void startSubscript()     { t << "\\*<"; firstCol=FALSE; }
    //void endSubscript()       { t << "\\*>"; firstCol=FALSE; }
    //void startSuperscript()   { t << "\\*{"; firstCol=FALSE; }
    //void endSuperscript()     { t << "\\*}"; firstCol=FALSE; }
    //void startTable(bool,int) {}
    //void endTable(bool)       {}
    //void startCaption()       {}
    //void endCaption()         {}
    //void nextTableRow()       {}
    //void endTableRow()        {}
    //void nextTableColumn()    {}
    //void endTableColumn()     {}
    //void writeCopyright()     { t << "(c)"; firstCol=FALSE; }
    //void writeQuote()         { t << "`"; firstCol=FALSE; }
    //void writeUmlaut(char c)  { t << c << "\\*'"; firstCol=FALSE; }
    //void writeAcute(char c)   { t << c << "\\*`"; firstCol=FALSE; }
    //void writeGrave(char c)   { t << c << "\\*:"; firstCol=FALSE; }
    //void writeCirc(char c)    { t << c << "\\*^"; firstCol=FALSE; }
    //void writeTilde(char c)   { t << c << "\\*~"; firstCol=FALSE; } 
    //void writeRing(char c)    { t << c << "\\*0"; /* just a wild guess, 
    //                                                 need to check! */ 
    //                                              firstCol=FALSE; }
    //void writeSharpS()        { t << "s\\*:";     /* just a wild guess,
    //                                                 need to check! */
    //                                              firstCol=FALSE; }
    //void writeCCedil(char c)  { t << c;           /* TODO: fix this */
    //                            firstCol=FALSE; }
    void startMemberDescription() { t << "\n.RI \"\\fI"; firstCol=FALSE; }
    void endMemberDescription()   { t << "\\fP\""; firstCol=FALSE; }
    void startDescList(SectionTypes);
    void endDescList()        {}
    void startSimpleSect(SectionTypes,const char *,const char *,const char *);
    void endSimpleSect();
    void startParamList(ParamListTypes,const char *title);
    void endParamList();
    //void endDescTitle();
    void writeDescItem();
    void startSection(const char *,const char *,SectionInfo::SectionType);
    void endSection(const char *,SectionInfo::SectionType);
    //void writeSectionRef(const char *,const char *,const char *,const char *) {}
    //void writeSectionRefItem(const char *,const char *,const char *) {}
    //void writeSectionRefAnchor(const char *,const char *,const char *) {}
    void addIndexItem(const char *,const char *) {}
    void startIndent()        {}
    void endIndent()          {}
    void writeSynopsis();
    //void generateExternalIndex() {}
    void startClassDiagram() {}
    void endClassDiagram(ClassDiagram &,const char *,const char *) {}
    //void startColorFont(uchar,uchar,uchar) {}
    //void endColorFont()   {}
    void startPageRef() {}
    void endPageRef(const char *,const char *) {}
    //void startQuickIndexItem(const char *,const char *) {}
    //void endQuickIndexItem() {}
    void writeQuickLinks(bool,HighlightedItem) {}
    //void writeFormula(const char *,const char *) {}
    void writeNonBreakableSpace(int n) { int i; for (i=0;i<n;i++) t << " "; }
    //void startImage(const char *,const char *,bool) {}
    //void endImage(bool) {}
    //void startDotFile(const char *,bool) {}
    //void endDotFile(bool) {}
    
    void startDescTable() {}
    void endDescTable() {}
    void startDescTableTitle() { writeListItem(); startBold(); startEmphasis(); }
    void endDescTableTitle() { endEmphasis(); endBold(); }
    void startDescTableData() { t << endl; firstCol=TRUE; }
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

    void startFontClass(const char *) {}
    void endFontClass() {}

    //void startHtmlOnly()  {}
    //void endHtmlOnly()    {}
    //void startLatexOnly() {}
    //void endLatexOnly()   {}

    //void startSectionRefList() {}
    //void endSectionRefList() {}

    void writeCodeAnchor(const char *) {}
    void linkableSymbol(int,const char *,Definition *,Definition *) {}

  private:
    bool firstCol;
    bool paragraph;
    int col;
    bool upperCase;
    bool insideTabbing;
    bool inHeader;

    ManGenerator(const ManGenerator &g);
    ManGenerator &operator=(const ManGenerator &g);
};

#endif
