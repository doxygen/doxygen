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

#ifndef OUTPUTLIST_H
#define OUTPUTLIST_H

#include "qtbc.h"
#include <qlist.h>
#include "index.h" // for IndexSections
#include "outputgen.h"

#define FORALLPROTO1(arg1) \
  void forall(void (OutputGenerator::*func)(arg1),arg1)
#define FORALLPROTO2(arg1,arg2) \
  void forall(void (OutputGenerator::*func)(arg1,arg2),arg1,arg2)
#define FORALLPROTO3(arg1,arg2,arg3) \
  void forall(void (OutputGenerator::*func)(arg1,arg2,arg3),arg1,arg2,arg3)
#define FORALLPROTO4(arg1,arg2,arg3,arg4) \
  void forall(void (OutputGenerator::*func)(arg1,arg2,arg3,arg4),arg1,arg2,arg3,arg4)
  
class ClassDiagram;

class OutputList
{
  public:
    OutputList(bool);
   ~OutputList();
    OutputList(const OutputList *ol);
    OutputList &operator=(const OutputList &ol);
    OutputList &operator+=(const OutputList &ol);

    void add(const OutputGenerator *);
    
    void disableAllBut(OutputGenerator::OutputType o);
    void enableAll();
    void disable(OutputGenerator::OutputType o);
    void enable(OutputGenerator::OutputType o);
    bool isEnabled(OutputGenerator::OutputType o);
    
    //void writeIndex() 
    //{ forall(&OutputGenerator::writeIndex); }
    void startIndexSection(IndexSections is)
    { forall(&OutputGenerator::startIndexSection,is); }
    void endIndexSection(IndexSections is)
    { forall(&OutputGenerator::endIndexSection,is); }
    void startProjectNumber()
    { forall(&OutputGenerator::startProjectNumber); }
    void endProjectNumber()
    { forall(&OutputGenerator::endProjectNumber); }
    void writeStyleInfo(int part) 
    { forall(&OutputGenerator::writeStyleInfo,part); }
    void startFile(const char *name,const char *title,bool external)
    { forall(&OutputGenerator::startFile,name,title,external); }
    void startPlainFile(const char *name)
    { forall(&OutputGenerator::startPlainFile,name); }
    void writeFooter(int fase,bool external)
    { forall(&OutputGenerator::writeFooter,fase,external); }
    void endFile() 
    { forall(&OutputGenerator::endFile); }
    void endPlainFile() 
    { forall(&OutputGenerator::endPlainFile); }
    void startTitleHead(const char *fileName) 
    { forall(&OutputGenerator::startTitleHead,fileName); }
    void endTitleHead(const char *fileName,const char *name)
    { forall(&OutputGenerator::endTitleHead,fileName,name); }
    void startTitle() 
    { forall(&OutputGenerator::startTitle); }
    void endTitle() 
    { forall(&OutputGenerator::endTitle); }
    void newParagraph() 
    { forall(&OutputGenerator::newParagraph); }
    void writeString(const char *text) 
    { forall(&OutputGenerator::writeString,text); }
    void startIndexList() 
    { forall(&OutputGenerator::startIndexList); }
    void endIndexList() 
    { forall(&OutputGenerator::endIndexList); }
    void startItemList() 
    { forall(&OutputGenerator::startItemList); }
    void endItemList() 
    { forall(&OutputGenerator::endItemList); }
    void startEnumList() 
    { forall(&OutputGenerator::startEnumList); }
    void endEnumList() 
    { forall(&OutputGenerator::endEnumList); }
    void startAlphabeticalIndexList()
    { forall(&OutputGenerator::startAlphabeticalIndexList); }
    void endAlphabeticalIndexList()
    { forall(&OutputGenerator::endAlphabeticalIndexList); }
    void writeIndexHeading(const char *s)
    { forall(&OutputGenerator::writeIndexHeading,s); }
    void writeIndexItem(const char *ref,const char *file,const char *text)
    { forall(&OutputGenerator::writeIndexItem,ref,file,text); }
    void docify(const char *s)
    { forall(&OutputGenerator::docify,s); }
    void codify(const char *s)
    { forall(&OutputGenerator::codify,s); }
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor, const char *text)
    { forall(&OutputGenerator::writeObjectLink,ref,file,anchor,text); }
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *text)
    { forall(&OutputGenerator::writeCodeLink,ref,file,anchor,text); }
    void startTextLink(const char *file,const char *anchor)
    { forall(&OutputGenerator::startTextLink,file,anchor); }
    void endTextLink()
    { forall(&OutputGenerator::endTextLink); }
    void writeHtmlLink(const char *url,const char *text)
    { forall(&OutputGenerator::writeHtmlLink,url,text); }
    void writeStartAnnoItem(const char *type,const char *file, 
                            const char *path,const char *name)
    { forall(&OutputGenerator::writeStartAnnoItem,type,file,path,name); }
    void writeEndAnnoItem(const char *name)
    { forall(&OutputGenerator::writeEndAnnoItem,name); }
    void startTypewriter() 
    { forall(&OutputGenerator::startTypewriter); }
    void endTypewriter() 
    { forall(&OutputGenerator::endTypewriter); }
    void startGroupHeader()
    { forall(&OutputGenerator::startGroupHeader); }
    void endGroupHeader()
    { forall(&OutputGenerator::endGroupHeader); }
    void writeListItem() 
    { forall(&OutputGenerator::writeListItem); }
    void startMemberSections()
    { forall(&OutputGenerator::startMemberSections); }
    void endMemberSections()
    { forall(&OutputGenerator::endMemberSections); }
    void startMemberHeader()
    { forall(&OutputGenerator::startMemberHeader); }
    void endMemberHeader()
    { forall(&OutputGenerator::endMemberHeader); }
    void startMemberSubtitle()
    { forall(&OutputGenerator::startMemberSubtitle); }
    void endMemberSubtitle()
    { forall(&OutputGenerator::endMemberSubtitle); }
    void startMemberList() 
    { forall(&OutputGenerator::startMemberList); }
    void endMemberList() 
    { forall(&OutputGenerator::endMemberList); }
    void startMemberItem(bool b1,int i1) 
    { forall(&OutputGenerator::startMemberItem,b1,i1); }
    void endMemberItem(bool b1,const char *n1,const char *n2,bool b2) 
    { forall(&OutputGenerator::endMemberItem,b1,n1,n2,b2); }
    void memberGroupSpacing(bool b) 
    { forall(&OutputGenerator::memberGroupSpacing,b); }
    void memberGroupSeparator() 
    { forall(&OutputGenerator::memberGroupSeparator); }
    void insertMemberAlign() 
    { forall(&OutputGenerator::insertMemberAlign); }
    void writeRuler() 
    { forall(&OutputGenerator::writeRuler); }
    void writeAnchor(const char *name)
    { forall(&OutputGenerator::writeAnchor,name); }
    void startCodeFragment() 
    { forall(&OutputGenerator::startCodeFragment); }
    void endCodeFragment() 
    { forall(&OutputGenerator::endCodeFragment); }
    void startCodeLine() 
    { forall(&OutputGenerator::startCodeLine); }
    void endCodeLine() 
    { forall(&OutputGenerator::endCodeLine); }
    void writeBoldString(const char *text)
    { forall(&OutputGenerator::writeBoldString,text); }
    void startEmphasis() 
    { forall(&OutputGenerator::startEmphasis); }
    void endEmphasis() 
    { forall(&OutputGenerator::endEmphasis); }
    void writeChar(char c)
    { forall(&OutputGenerator::writeChar,c); }
    void startMemberDoc(const char *clName,const char *memName,
                        const char *anchor,const char *title)
    { forall(&OutputGenerator::startMemberDoc,clName,memName,anchor,title); }
    void endMemberDoc() 
    { forall(&OutputGenerator::endMemberDoc); }
    void startDoxyAnchor(const char *fn,    const char *cn,
                         const char *anchor,const char *name)
    { forall(&OutputGenerator::startDoxyAnchor,fn,cn,anchor,name); }
    void endDoxyAnchor()
    { forall(&OutputGenerator::endDoxyAnchor); }
    void startCodeAnchor(const char *label)
    { forall(&OutputGenerator::startCodeAnchor,label); }
    void endCodeAnchor()
    { forall(&OutputGenerator::endCodeAnchor); }
    void writeLatexSpacing() 
    { forall(&OutputGenerator::writeLatexSpacing); }
    //void writeLatexLabel(const char *scope,const char *anchor) 
    //{ forall(&OutputGenerator::writeLatexLabel,scope,anchor); }
    void startDescription() 
    { forall(&OutputGenerator::startDescription); }
    void endDescription() 
    { forall(&OutputGenerator::endDescription); }
    void startDescItem() 
    { forall(&OutputGenerator::startDescItem); }
    void endDescItem() 
    { forall(&OutputGenerator::endDescItem); }
    void startSubsection() 
    { forall(&OutputGenerator::startSubsection); }
    void endSubsection() 
    { forall(&OutputGenerator::endSubsection); }
    void startSubsubsection() 
    { forall(&OutputGenerator::startSubsubsection); }
    void endSubsubsection() 
    { forall(&OutputGenerator::endSubsubsection); }
    void startCenter() 
    { forall(&OutputGenerator::startCenter); }
    void endCenter() 
    { forall(&OutputGenerator::endCenter); }
    void startSmall() 
    { forall(&OutputGenerator::startSmall); }
    void endSmall() 
    { forall(&OutputGenerator::endSmall); }
    void startSubscript() 
    { forall(&OutputGenerator::startSubscript); }
    void endSubscript() 
    { forall(&OutputGenerator::endSubscript); }
    void startSuperscript() 
    { forall(&OutputGenerator::startSuperscript); }
    void endSuperscript() 
    { forall(&OutputGenerator::endSuperscript); }
    void startTable(int cols)
    { forall(&OutputGenerator::startTable,cols); }
    void endTable() 
    { forall(&OutputGenerator::endTable); }
    void nextTableRow() 
    { forall(&OutputGenerator::nextTableRow); }
    void endTableRow() 
    { forall(&OutputGenerator::endTableRow); }
    void nextTableColumn() 
    { forall(&OutputGenerator::nextTableColumn); }
    void endTableColumn() 
    { forall(&OutputGenerator::endTableColumn); }
    void lineBreak() 
    { forall(&OutputGenerator::lineBreak); }
    void startBold() 
    { forall(&OutputGenerator::startBold); }
    void endBold() 
    { forall(&OutputGenerator::endBold); }
    void writeCopyright() 
    { forall(&OutputGenerator::writeCopyright); }
    void writeQuote() 
    { forall(&OutputGenerator::writeQuote); }
    void writeUmlaut(char c)
    { forall(&OutputGenerator::writeUmlaut,c); }
    void writeAcute(char c)
    { forall(&OutputGenerator::writeAcute,c); }
    void writeGrave(char c)
    { forall(&OutputGenerator::writeGrave,c); }
    void writeCirc(char c)
    { forall(&OutputGenerator::writeCirc,c); }
    void writeTilde(char c)
    { forall(&OutputGenerator::writeTilde,c); }
    void writeRing(char c)
    { forall(&OutputGenerator::writeRing,c); }
    void writeSharpS()
    { forall(&OutputGenerator::writeSharpS); }
    void startMemberDescription() 
    { forall(&OutputGenerator::startMemberDescription); }
    void endMemberDescription() 
    { forall(&OutputGenerator::endMemberDescription); }
    void startDescList() 
    { forall(&OutputGenerator::startDescList); }
    void endDescTitle() 
    { forall(&OutputGenerator::endDescTitle); }
    void writeDescItem() 
    { forall(&OutputGenerator::writeDescItem); }
    void endDescList() 
    { forall(&OutputGenerator::endDescList); }
    void startIndent() 
    { forall(&OutputGenerator::startIndent); }
    void endIndent() 
    { forall(&OutputGenerator::endIndent); }
    void writeSection(const char *lab,const char *title,bool sub)
    { forall(&OutputGenerator::writeSection,lab,title,sub); }
    void writeSectionRef(const char *page,const char *lab, const char *title)
    { forall(&OutputGenerator::writeSectionRef,page,lab,title); }
    void writeSectionRefItem(const char *page,const char *lab, const char *title)
    { forall(&OutputGenerator::writeSectionRefItem,page,lab,title); }
    void writeSectionRefAnchor(const char *page,const char *lab, const char *title)
    { forall(&OutputGenerator::writeSectionRefAnchor,page,lab,title); }
    void addToIndex(const char *s1,const char *s2)
    { forall(&OutputGenerator::addToIndex,s1,s2); }
    void writeSynopsis() 
    { forall(&OutputGenerator::writeSynopsis); }
    //void generateExternalIndex()
    //{ forall(&OutputGenerator::generateExternalIndex); }
    void startClassDiagram()
    { forall(&OutputGenerator::startClassDiagram); }
    void endClassDiagram(ClassDiagram &d,const char *f,const char *n)
    { forall(&OutputGenerator::endClassDiagram,d,f,n); }
    void startColorFont(uchar r,uchar g,uchar b)
    { forall(&OutputGenerator::startColorFont,r,g,b); }
    void endColorFont()
    { forall(&OutputGenerator::endColorFont); }
    void startPageRef()
    { forall(&OutputGenerator::startPageRef); }
    void endPageRef(const char *c,const char *a)
    { forall(&OutputGenerator::endPageRef,c,a); }
    void startQuickIndexItem(const char *s,const char *l)
    { forall(&OutputGenerator::startQuickIndexItem,s,l); }
    void endQuickIndexItem()
    { forall(&OutputGenerator::endQuickIndexItem); }
    void writeFormula(const char *n,const char *t)
    { forall(&OutputGenerator::writeFormula,n,t); }
    void writeNonBreakableSpace()
    { forall(&OutputGenerator::writeNonBreakableSpace); }
    void writeImage(const char *n,const char *w,const char *h)
    { forall(&OutputGenerator::writeImage,n,w,h); }

    void startDescTable()
    { forall(&OutputGenerator::startDescTable); }
    void endDescTable()
    { forall(&OutputGenerator::endDescTable); }
    void startDescTableTitle()
    { forall(&OutputGenerator::startDescTableTitle); }
    void endDescTableTitle()
    { forall(&OutputGenerator::endDescTableTitle); }
    void startDescTableData()
    { forall(&OutputGenerator::startDescTableData); }
    void endDescTableData()
    { forall(&OutputGenerator::endDescTableData); }

  private:
    void debug();
    void clear();
    
    void forall(void (OutputGenerator::*func)());
    FORALLPROTO1(const char *);
    FORALLPROTO1(char);
    FORALLPROTO1(IndexSections);
    FORALLPROTO1(int);
#if defined(HAS_BOOL_TYPE)
    FORALLPROTO1(bool);
    FORALLPROTO2(bool,int);
    FORALLPROTO2(bool,bool);
#endif
    FORALLPROTO2(int,bool);
    FORALLPROTO2(const char *,const char *);
    FORALLPROTO3(const char *,const char *,bool);
    FORALLPROTO3(uchar,uchar,uchar);
    FORALLPROTO3(const char *,const char *,const char *);
    FORALLPROTO3(ClassDiagram &,const char *,const char *);
    FORALLPROTO4(const char *,const char *,const char *,const char *);
    FORALLPROTO4(bool,const char *,const char *,bool);
  
    OutputList(const OutputList &ol);
    QList<OutputGenerator> *outputs;
};

#endif
