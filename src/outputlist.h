/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
class DotClassGraph;
class DotInclDepGraph;
class DotGfxHierarchyTable;

class OutputList : public OutputDocInterface
{
  public:
    OutputList(bool);
    virtual ~OutputList();

    void add(const OutputGenerator *);
    
    void disableAllBut(OutputGenerator::OutputType o);
    void enableAll();
    void disableAll();
    void disable(OutputGenerator::OutputType o);
    void enable(OutputGenerator::OutputType o);
    bool isEnabled(OutputGenerator::OutputType o);
    void pushGeneratorState();
    void popGeneratorState();


    //////////////////////////////////////////////////
    // OutputDocInterface implementation
    //////////////////////////////////////////////////

    void parseDoc(const char *fileName,int startLine,
                  const char *clName,MemberDef *md,const QCString &docStr,
                  bool isExample);
    void parseText(const QCString &textStr);
    

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
    void startFile(const char *name,const char *manName,
                   const char *title,bool external)
    { forall(&OutputGenerator::startFile,name,manName,title,external); }
    void writeFooter(int fase,bool external)
    { forall(&OutputGenerator::writeFooter,fase,external); }
    void endFile() 
    { forall(&OutputGenerator::endFile); }
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
    void startIndexKey()
    { forall(&OutputGenerator::startIndexKey); }
    void endIndexKey()
    { forall(&OutputGenerator::endIndexKey); }
    void startIndexValue(bool b)
    { forall(&OutputGenerator::startIndexValue,b); }
    void endIndexValue(const char *name,bool b)
    { forall(&OutputGenerator::endIndexValue,name,b); }
    void startItemList() 
    { forall(&OutputGenerator::startItemList); }
    void endItemList() 
    { forall(&OutputGenerator::endItemList); }
    void writeIndexItem(const char *ref,const char *file,const char *text)
    { forall(&OutputGenerator::writeIndexItem,ref,file,text); }
    void docify(const char *s)
    { forall(&OutputGenerator::docify,s); }
    void codify(const char *s)
    { forall(&OutputGenerator::codify,s); }
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor, const char *name)
    { forall(&OutputGenerator::writeObjectLink,ref,file,anchor,name); }
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name)
    { forall(&OutputGenerator::writeCodeLink,ref,file,anchor,name); }
    void startTextLink(const char *file,const char *anchor)
    { forall(&OutputGenerator::startTextLink,file,anchor); }
    void endTextLink()
    { forall(&OutputGenerator::endTextLink); }
    void startHtmlLink(const char *url)
    { forall(&OutputGenerator::startHtmlLink,url); }
    void endHtmlLink()
    { forall(&OutputGenerator::endHtmlLink); }
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
    void startMemberItem(int i1) 
    { forall(&OutputGenerator::startMemberItem,i1); }
    void endMemberItem(bool b2) 
    { forall(&OutputGenerator::endMemberItem,b2); }
    void startMemberGroupHeader(bool b) 
    { forall(&OutputGenerator::startMemberGroupHeader,b); }
    void endMemberGroupHeader()
    { forall(&OutputGenerator::endMemberGroupHeader); }
    void startMemberGroupDocs()
    { forall(&OutputGenerator::startMemberGroupDocs); }
    void endMemberGroupDocs()
    { forall(&OutputGenerator::endMemberGroupDocs); }
    void startMemberGroup()
    { forall(&OutputGenerator::startMemberGroup); }
    void endMemberGroup(bool last)
    { forall(&OutputGenerator::endMemberGroup,last); }
    void insertMemberAlign() 
    { forall(&OutputGenerator::insertMemberAlign); }
    void writeRuler() 
    { forall(&OutputGenerator::writeRuler); }
    void writeAnchor(const char *fileName,const char *name)
    { forall(&OutputGenerator::writeAnchor,fileName,name); }
    void startCodeFragment() 
    { forall(&OutputGenerator::startCodeFragment); }
    void endCodeFragment() 
    { forall(&OutputGenerator::endCodeFragment); }
    void startCodeLine() 
    { forall(&OutputGenerator::startCodeLine); }
    void endCodeLine() 
    { forall(&OutputGenerator::endCodeLine); }
    void writeLineNumber(const char *ref,const char *file,const char *anchor,
                         int lineNumber) 
    { forall(&OutputGenerator::writeLineNumber,ref,file,anchor,lineNumber); }
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
    void startDoxyAnchor(const char *fName,const char *manName,
                         const char *anchor, const char *name)
    { forall(&OutputGenerator::startDoxyAnchor,fName,manName,anchor,name); }
    void endDoxyAnchor(const char *fn,const char *anchor)
    { forall(&OutputGenerator::endDoxyAnchor,fn,anchor); }
    void startCodeAnchor(const char *label)
    { forall(&OutputGenerator::startCodeAnchor,label); }
    void endCodeAnchor()
    { forall(&OutputGenerator::endCodeAnchor); }
    void writeLatexSpacing() 
    { forall(&OutputGenerator::writeLatexSpacing); }
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
    void lineBreak() 
    { forall(&OutputGenerator::lineBreak); }
    void startBold() 
    { forall(&OutputGenerator::startBold); }
    void endBold() 
    { forall(&OutputGenerator::endBold); }
    void startMemberDescription() 
    { forall(&OutputGenerator::startMemberDescription); }
    void endMemberDescription() 
    { forall(&OutputGenerator::endMemberDescription); }
    void startSimpleSect(SectionTypes t,const char *file,const char *anchor,
                         const char *title) 
    { forall(&OutputGenerator::startSimpleSect,t,file,anchor,title); }
    void endSimpleSect() 
    { forall(&OutputGenerator::endSimpleSect); }
    void startParamList(ParamListTypes t,const char *title) 
    { forall(&OutputGenerator::startParamList,t,title); }
    void endParamList() 
    { forall(&OutputGenerator::endParamList); }
    void writeDescItem() 
    { forall(&OutputGenerator::writeDescItem); }
    void startIndent() 
    { forall(&OutputGenerator::startIndent); }
    void endIndent() 
    { forall(&OutputGenerator::endIndent); }
    void startSection(const char *lab,const char *title,SectionInfo::SectionType t)
    { forall(&OutputGenerator::startSection,lab,title,t); }
    void endSection(const char *lab,SectionInfo::SectionType t)
    { forall(&OutputGenerator::endSection,lab,t); }
    void addIndexItem(const char *s1,const char *s2)
    { forall(&OutputGenerator::addIndexItem,s1,s2); }
    void writeSynopsis() 
    { forall(&OutputGenerator::writeSynopsis); }
    void startClassDiagram()
    { forall(&OutputGenerator::startClassDiagram); }
    void endClassDiagram(ClassDiagram &d,const char *f,const char *n)
    { forall(&OutputGenerator::endClassDiagram,d,f,n); }
    void startPageRef()
    { forall(&OutputGenerator::startPageRef); }
    void endPageRef(const char *c,const char *a)
    { forall(&OutputGenerator::endPageRef,c,a); }
    void startQuickIndexItem(const char *s,const char *l)
    { forall(&OutputGenerator::startQuickIndexItem,s,l); }
    void endQuickIndexItem()
    { forall(&OutputGenerator::endQuickIndexItem); }
    void writeNonBreakableSpace(int num)
    { forall(&OutputGenerator::writeNonBreakableSpace,num); }
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
    void startDotGraph()
    { forall(&OutputGenerator::startDotGraph); }
    void endDotGraph(DotClassGraph &g)
    { forall(&OutputGenerator::endDotGraph,g); }
    void startInclDepGraph()
    { forall(&OutputGenerator::startInclDepGraph); }
    void endInclDepGraph(DotInclDepGraph &g)
    { forall(&OutputGenerator::endInclDepGraph,g); }
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g)
    { forall(&OutputGenerator::writeGraphicalHierarchy,g); }
    void startTextBlock(bool dense=FALSE)
    { forall(&OutputGenerator::startTextBlock,dense); }
    void endTextBlock()
    { forall(&OutputGenerator::endTextBlock); }
    void lastIndexPage()
    { forall(&OutputGenerator::lastIndexPage); }
    void startMemberDocPrefixItem()
    { forall(&OutputGenerator::startMemberDocPrefixItem); }
    void endMemberDocPrefixItem()
    { forall(&OutputGenerator::endMemberDocPrefixItem); }
    void startMemberDocName()
    { forall(&OutputGenerator::startMemberDocName); }
    void endMemberDocName()
    { forall(&OutputGenerator::endMemberDocName); }
    void startParameterType(bool first)
    { forall(&OutputGenerator::startParameterType,first); }
    void endParameterType()
    { forall(&OutputGenerator::endParameterType); }
    void startParameterName(bool one)
    { forall(&OutputGenerator::startParameterName,one); }
    void endParameterName(bool last,bool one)
    { forall(&OutputGenerator::endParameterName,last,one); }
    void startParameterList()
    { forall(&OutputGenerator::startParameterList); }
    void endParameterList()
    { forall(&OutputGenerator::endParameterList); }
    void startFontClass(const char *c)
    { forall(&OutputGenerator::startFontClass,c); }
    void endFontClass()
    { forall(&OutputGenerator::endFontClass); }
    void writeCodeAnchor(const char *name)
    { forall(&OutputGenerator::writeCodeAnchor,name); }
    void startPlainFile(const char *name)
    { 
      OutputGenerator *og=outputs->first();
      while (og)
      {
        if (og->isEnabled()) (og->startPlainFile)(name);
        og=outputs->next();
      }
    }
    void endPlainFile() 
    { 
      OutputGenerator *og=outputs->first();
      while (og)
      {
        if (og->isEnabled()) (og->endPlainFile)();
        og=outputs->next();
      }
    }

  private:
    void debug();
    void clear();
    
    void forall(void (OutputGenerator::*func)());
    FORALLPROTO1(const char *);
    FORALLPROTO1(char);
    FORALLPROTO1(IndexSections);
    FORALLPROTO1(int);
    FORALLPROTO1(DotClassGraph &);
    FORALLPROTO1(DotInclDepGraph &);
    FORALLPROTO1(DotGfxHierarchyTable &);
    FORALLPROTO1(SectionTypes);
#if defined(HAS_BOOL_TYPE) || defined(Q_HAS_BOOL_TYPE)
    FORALLPROTO1(bool);
    FORALLPROTO2(bool,int);
    FORALLPROTO2(bool,bool);
    FORALLPROTO4(const char *,const char *,const char *,int);
#endif
    FORALLPROTO2(int,bool);
    FORALLPROTO2(ParamListTypes,const char *);
    FORALLPROTO2(const char *,const char *);
    FORALLPROTO2(const char *,bool);
    FORALLPROTO2(const char *,SectionInfo::SectionType);
    FORALLPROTO3(const char *,const char *,bool);
    FORALLPROTO3(const char *,const char *,SectionInfo::SectionType);
    FORALLPROTO3(uchar,uchar,uchar);
    FORALLPROTO3(const char *,const char *,const char *);
    FORALLPROTO3(ClassDiagram &,const char *,const char *);
    FORALLPROTO4(SectionTypes,const char *,const char *,const char *);
    FORALLPROTO4(const char *,const char *,const char *,const char *);
    FORALLPROTO4(const char *,const char *,const char *,bool);
  
    OutputList(const OutputList &ol);
    QList<OutputGenerator> *outputs;
};

#endif
