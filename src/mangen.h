/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
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
    
    OutputGenerator *copy()  { return new ManGenerator; } 
    //OutputGenerator *clone() { return new ManGenerator(*this); }
    void append(const OutputGenerator *o);
    void enable() { active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==Man) active=TRUE;  }
    void disableIf(OutputType o) { if (o==Man) active=FALSE; }
    void disableIfNot(OutputType o) { if (o!=Man) active=FALSE; }
    bool isEnabled(OutputType o) { return (o==Man && active); } 
    OutputGenerator *get(OutputType o) { return (o==Man) ? this : 0; }

    static void init();
    void startFile(const char *name,const char *title,bool external);
    void writeFooter(int,bool) {}
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
    void endTitle() {}
    
    void newParagraph();
    void writeString(const char *text);
    void startIndexList() {}
    void endIndexList()   {} 
    void startItemList()  {}
    void endItemList()    {}
    void startEnumList()  {}
    void endEnumList()    {}
    void startAlphabeticalIndexList() {}
    void endAlphabeticalIndexList() {} 
    void writeIndexHeading(const char *) {}
    void writeIndexItem(const char *ref,const char *file,const char *name);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref,const char *f,
                       const char *anchor,const char *name);
    void startTextLink(const char *,const char *) {}
    void endTextLink() {}
    void writeHtmlLink(const char *url,const char *text);
    void startTypewriter() { t << "\\fC"; firstCol=FALSE; }
    void endTypewriter()   { t << "\\fR"; firstCol=FALSE; }
    void startGroupHeader();
    void endGroupHeader();
    void startMemberSections() {}
    void endMemberSections() {}
    void startMemberHeader();
    void endMemberHeader();
    void insertMemberAlign() {}
    void startMemberSubtitle() {}
    void endMemberSubtitle() {}
    void writeListItem();
    void startMemberList();
    void endMemberList();
    void startMemberItem(bool,int);
    void endMemberItem(bool,const char *,const char *,bool);
    void memberGroupSpacing(bool) {}
    void memberGroupSeparator() {}
    void writeRuler()    {}
    void writeAnchor(const char *) {}
    void startCodeFragment();
    void endCodeFragment();
    void startCodeLine() {}
    void endCodeLine() { t << endl; }
    void writeBoldString(const char *text) 
                         { t << "\\fB"; docify(text); t << "\\fR"; firstCol=FALSE; }
    void startEmphasis() { t << "\\fI"; firstCol=FALSE; }
    void endEmphasis()   { t << "\\fR"; firstCol=FALSE; }
    void startBold()     { t << "\\fB"; firstCol=FALSE; }
    void endBold()       { t << "\\fR"; firstCol=FALSE; }
    void startDescription() {}
    void endDescription()   {}
    void startDescItem();
    void endDescItem();
    void lineBreak() { t << "\n.br" << endl; }
    void writeChar(char c);
    void startMemberDoc(const char *,const char *,const char *,const char *);
    void endMemberDoc() {}
    void startDoxyAnchor(const char *,const char *,
                         const char *,const char *) {}
    void endDoxyAnchor() {}
    void startCodeAnchor(const char *) {}
    void endCodeAnchor() {}
    void writeLatexSpacing() {}
    //void writeLatexLabel(const char *,const char *) {}
    void writeStartAnnoItem(const char *type,const char *file,
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *) { t << endl; firstCol=TRUE; }
    void startSubsection();
    void endSubsection()      {}
    void startSubsubsection();
    void endSubsubsection()   {}
    void startCenter()        {}
    void endCenter()          {}
    void startSmall()         {}
    void endSmall()           {}
    void startSubscript()     { t << "\\*<"; firstCol=FALSE; }
    void endSubscript()       { t << "\\*>"; firstCol=FALSE; }
    void startSuperscript()   { t << "\\*{"; firstCol=FALSE; }
    void endSuperscript()     { t << "\\*}"; firstCol=FALSE; }
    void startTable(int)      {}
    void endTable()           {}
    void nextTableRow()       {}
    void endTableRow()        {}
    void nextTableColumn()    {}
    void endTableColumn()     {}
    void writeCopyright()     { t << "(c)"; firstCol=FALSE; }
    void writeQuote()         { t << "\""; firstCol=FALSE; }
    void writeUmlaut(char c)  { t << c << "\\*'"; firstCol=FALSE; }
    void writeAcute(char c)   { t << c << "\\*`"; firstCol=FALSE; }
    void writeGrave(char c)   { t << c << "\\*:"; firstCol=FALSE; }
    void writeCirc(char c)    { t << c << "\\*^"; firstCol=FALSE; }
    void writeTilde(char c)   { t << c << "\\*~"; firstCol=FALSE; } 
    void writeRing(char c)    { t << c << "\\*0"; /* just a wild guess, 
                                                     need to check! */ 
                                                  firstCol=FALSE; }
    void writeSharpS()        { t << "s\\*:";     /* just a wild guess,
                                                     need to check! */
                                                  firstCol=FALSE; }
    void startMemberDescription() { t << "\n.RI \"\\fI"; firstCol=FALSE; }
    void endMemberDescription()   { t << "\\fR\""; firstCol=FALSE; }
    void startDescList();
    void endDescTitle();
    void writeDescItem();
    void endDescList()        {}
    void writeSection(const char *,const char *,bool) {}
    void writeSectionRef(const char *,const char *,const char *) {}
    void writeSectionRefItem(const char *,const char *,const char *) {}
    void writeSectionRefAnchor(const char *,const char *,const char *) {}
    void addToIndex(const char *,const char *) {}
    void startIndent()        {}
    void endIndent()          {}
    void writeSynopsis();
    //void generateExternalIndex() {}
    void startClassDiagram() {}
    void endClassDiagram(ClassDiagram &,const char *,const char *) {}
    void startColorFont(uchar,uchar,uchar) {}
    void endColorFont()   {}
    void startPageRef() {}
    void endPageRef(const char *,const char *) {}
    void startQuickIndexItem(const char *,const char *) {}
    void endQuickIndexItem() {}
    void writeFormula(const char *,const char *) {}
    void writeNonBreakableSpace() { t << "  "; }
    void writeImage(const char *,const char *,const char *) {}
    
    void startDescTable() {}
    void endDescTable() {}
    void startDescTableTitle() { writeListItem(); startBold(); }
    void endDescTableTitle() { endBold(); }
    void startDescTableData() { }
    void endDescTableData() {}

  private:
    bool firstCol;
    bool paragraph;
    int col;
    bool upperCase;
    bool insideTabbing;

    ManGenerator(const ManGenerator &g);
    ManGenerator &operator=(const ManGenerator &g);
};

#endif
