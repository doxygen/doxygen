/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Parker Waechter & Dimitri van Heesch.
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

class RTFGenerator : public OutputGenerator
{
  public:
    RTFGenerator();
   ~RTFGenerator();
    static void init();
    static void writeStyleSheetFile(QFile &f);
    static void writeExtensionsFile(QFile &file);

    //OutputGenerator *copy();
    //OutputGenerator *clone() { return new RTFGenerator(*this); }
    //void append(const OutputGenerator *o);
    void enable() { active=TRUE; }
    void disable() { active=FALSE; }
    void enableIf(OutputType o)  { if (o==RTF) active=TRUE;  }
    void disableIf(OutputType o) { if (o==RTF) active=FALSE; }
    void disableIfNot(OutputType o) { if (o!=RTF) active=FALSE; }
    bool isEnabled(OutputType o) { return (o==RTF && active); } 
    OutputGenerator *get(OutputType o) { return (o==RTF) ? this : 0; }

    void printDoc(DocNode *);

    void startFile(const char *name,const char *manName,const char *title);
    void writeFooter() {}
    void endFile();
    void clearBuffer();
    
    //void writeIndex();
	
    void startIndexSection(IndexSections);
    void endIndexSection(IndexSections);
    void startProjectNumber();
    void endProjectNumber();
    void writeStyleInfo(int part);
    void startTitleHead(const char *);
    void startTitle();
    void endTitleHead(const char *,const char *name);
    void endTitle() {} 

    void newParagraph();
    void writeString(const char *text);
    void startIndexList();
    void endIndexList();
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const char *,bool);
    void startItemList();
    void endItemList();
    //void startEnumList();
    //void endEnumList();
    //void startAlphabeticalIndexList() {}
    //void endAlphabeticalIndexList() {} 
    //void writeIndexHeading(const char *) {}
    void writeIndexItem(const char *ref,const char *file,const char *name);
    void docify(const char *text);
    void codify(const char *text);
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);
    void writeCodeLink(const char *ref, const char *file,
                       const char *anchor,const char *name);
    void startTextLink(const char *f,const char *anchor);
    void endTextLink();
    void startHtmlLink(const char *url);
    void endHtmlLink();
    //void writeMailLink(const char *url);
    void startTypewriter() { t << "{\\f2 "; }
    void endTypewriter()   { t << "}";      }
    void startGroupHeader();
    void endGroupHeader();
    void writeListItem();

    void startMemberSections() {}
    void endMemberSections() {} 
    void startMemberHeader() { startGroupHeader(); }
    void endMemberHeader() { endGroupHeader(); }
    void startMemberSubtitle(); 
    void endMemberSubtitle(); 
    void startMemberList();
    void endMemberList();
    void startAnonTypeScope(int) {}
    void endAnonTypeScope(int) {}
    void startMemberItem(int);
    void endMemberItem();
    //void memberGroupSpacing(bool) {}
    //void memberGroupSeparator() {}
    void insertMemberAlign() {}

    void writeRuler() { rtfwriteRuler_thin(); }
	
    void writeAnchor(const char *fileName,const char *name);
    void startCodeFragment();
    void endCodeFragment();
    //void startPreFragment()      { startCodeFragment(); }
    //void endPreFragment()        { endCodeFragment(); }
    //void startVerbatimFragment() { startCodeFragment(); }
    //void endVerbatimFragment()   { endCodeFragment(); }
    void writeLineNumber(const char *,const char *,const char *,int l) { t << l << " "; }
    void startCodeLine() { col=0; }
    void endCodeLine() { lineBreak(); }
    //void writeBoldString(const char *text) 
    //                     { t << "{\\b "; docify(text); t << "}"; }
    void startEmphasis() { t << "{\\i ";  }
    void endEmphasis()   { t << "}"; }
    void startBold()     { t << "{\\b "; }
    void endBold()       { t << "}"; }
    void startDescription();
    void endDescription();
    void startDescItem();
    void endDescItem();
    void lineBreak();
    void startMemberDoc(const char *,const char *,const char *,const char *);
    void endMemberDoc();
    void startDoxyAnchor(const char *,const char *,const char *,const char *);
    void endDoxyAnchor(const char *,const char *);
    void startCodeAnchor(const char *) {};
    void endCodeAnchor() {};
    void writeChar(char c);
    void writeLatexSpacing() {};//{ t << "\\hspace{0.3cm}"; }
    //void writeLatexLabel(const char *scope,const char *anchor);
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
    //void startSubscript()   { t << "{\\sub " << endl;}
    //void endSubscript()     { t << "}"; }
    //void startSuperscript() { t << "{\\super " << endl;}
    //void endSuperscript()   { t << "}"; }
    //void startTable(bool,int);
    //void endTable(bool);
    //void startCaption();
    //void endCaption();
    //void nextTableRow();
    //void endTableRow();
    //void nextTableColumn();
    //void endTableColumn();
    //void writeCopyright()    { t << "\251"; }
    //void writeQuote()        { t << "\""; }
    //void writeUmlaut(char c);
    //void writeAcute(char c);
    //void writeGrave(char c);
    //void writeCirc(char c);
    //void writeTilde(char c);
    //void writeRing(char c);
    //void writeSharpS()       { t << "\337"; }
    //void writeCCedil(char c);

    void startMemberDescription();
    void endMemberDescription();
    void startDescList(SectionTypes);
    //void endDescList();
    void startSimpleSect(SectionTypes,const char *,const char *,const char *);
    void endSimpleSect();
    void startParamList(ParamListTypes,const char *);
    void endParamList();
    //void endDescTitle();
    void writeDescItem();
    void startSection(const char *,const char *,SectionInfo::SectionType);
    void endSection(const char *,SectionInfo::SectionType);
    //void writeSectionRef(const char *,const char *,const char *,const char *);
    //void writeSectionRefItem(const char *,const char *,const char *);
    //void writeSectionRefAnchor(const char *,const char *,const char *);
    void addIndexItem(const char *,const char *);
    void startIndent();
    void endIndent();
    void writeSynopsis()     {}
    //void generateExternalIndex() {}
    void startClassDiagram();
    void endClassDiagram(ClassDiagram &,const char *filename,const char *name);
    //void startColorFont(uchar,uchar,uchar) {}
    //void endColorFont()   {}
    void startPageRef();
    void endPageRef(const char *,const char *);
    //void startQuickIndexItem(const char *,const char *) {}
    //void endQuickIndexItem() {}
    void writeQuickLinks(bool,HighlightedItem) {}
    //void writeFormula(const char *,const char *);
    void writeNonBreakableSpace(int);
    //void startImage(const char *,const char *,bool);
    //void endImage(bool);
    //void startDotFile(const char *,bool);
    //void endDotFile(bool);
	
    void startDescTable();
    void endDescTable();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableData();
    void endDescTableData();
    
    void startDotGraph();
    void endDotGraph(DotClassGraph &);
    void startInclDepGraph();
    void endInclDepGraph(DotInclDepGraph &);
    void startCallGraph();
    void endCallGraph(DotCallGraph &);
    void writeGraphicalHierarchy(DotGfxHierarchyTable &) {}

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);
    
    void startTextBlock(bool dense);
    void endTextBlock();
    void lastIndexPage();

    void startMemberDocPrefixItem() {}
    void endMemberDocPrefixItem() {}
    void startMemberDocName(bool) {}
    void endMemberDocName() {}
    void startParameterType(bool,const char *) {}
    void endParameterType() {}
    void startParameterName(bool) {}
    void endParameterName(bool,bool) {}
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

    void beginRTFDocument();
    void beginRTFChapter();
    void beginRTFSection();
    void rtfwriteRuler_doubleline() { t << "{\\pard\\widctlpar\\brdrb\\brdrdb\\brdrw15\\brsp20 \\adjustright \\par}" << endl; };
    void rtfwriteRuler_emboss() { t << "{\\pard\\widctlpar\\brdrb\\brdremboss\\brdrw15\\brsp20 \\adjustright \\par}" << endl; };
    void rtfwriteRuler_thick() { t << "{\\pard\\widctlpar\\brdrb\\brdrs\\brdrw75\\brsp20 \\adjustright \\par}" << endl; };
    void rtfwriteRuler_thin() { t << "{\\pard\\widctlpar\\brdrb\\brdrs\\brdrw5\\brsp20 \\adjustright \\par}" << endl; };
    void writeRTFReference(const char *label);
    char *getMultiByte(int c);
};

#endif
