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

#ifndef OUTPUTLIST_H
#define OUTPUTLIST_H

#include <utility>
#include <vector>
#include <memory>

#include "index.h" // for IndexSections
#include "outputgen.h"

class ClassDiagram;
class DotClassGraph;
class DotDirDeps;
class DotInclDepGraph;
class DotGfxHierarchyTable;
class DotGroupCollaboration;
class DocRoot;

/** Class representing a list of output generators that are written to
 *  in parallel.
 */
class OutputList : public OutputDocInterface
{
  public:
    OutputList(bool);
    virtual ~OutputList();

    void add(OutputGenerator *);
    uint count() const { return static_cast<uint>(m_outputs.size()); }

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

    void generateDoc(const char *fileName,int startLine,
                     const Definition *ctx,const MemberDef *md,const QCString &docStr,
                     bool indexWords,bool isExample,const char *exampleName=0,
                     bool singleLine=FALSE,bool linkFromIndex=FALSE);
    void writeDoc(DocRoot *root,const Definition *ctx,const MemberDef *md);
    void parseText(const QCString &textStr);

    void startIndexSection(IndexSections is)
    { forall(&OutputGenerator::startIndexSection,is); }
    void endIndexSection(IndexSections is)
    { forall(&OutputGenerator::endIndexSection,is); }
    void writePageLink(const char *name,bool first)
    { forall(&OutputGenerator::writePageLink,name,first); }
    void startProjectNumber()
    { forall(&OutputGenerator::startProjectNumber); }
    void endProjectNumber()
    { forall(&OutputGenerator::endProjectNumber); }
    void writeStyleInfo(int part)
    { forall(&OutputGenerator::writeStyleInfo,part); }
    void startFile(const char *name,const char *manName,const char *title)
    { forall(&OutputGenerator::startFile,name,manName,title); }
    void writeSearchInfo()
    { forall(&OutputGenerator::writeSearchInfo); }
    void writeFooter(const char *navPath)
    { forall(&OutputGenerator::writeFooter,navPath); }
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
    void startParagraph(const char *classDef=0)
    { forall(&OutputGenerator::startParagraph,classDef); }
    void endParagraph()
    { forall(&OutputGenerator::endParagraph); }
    void writeString(const char *text)
    { forall(&OutputGenerator::writeString,text); }
    void startIndexListItem()
    { forall(&OutputGenerator::startIndexListItem); }
    void endIndexListItem()
    { forall(&OutputGenerator::endIndexListItem); }
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
    void startIndexItem(const char *ref,const char *file)
    { forall(&OutputGenerator::startIndexItem,ref,file); }
    void endIndexItem(const char *ref,const char *file)
    { forall(&OutputGenerator::endIndexItem,ref,file); }
    void docify(const char *s)
    { forall(&OutputGenerator::docify,s); }
    void codify(const char *s)
    { forall(&OutputGenerator::codify,s); }
    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor, const char *name)
    { forall(&OutputGenerator::writeObjectLink,ref,file,anchor,name); }
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip)
    { forall(&OutputGenerator::writeCodeLink,ref,file,anchor,name,tooltip); }
    void writeTooltip(const char *id, const DocLinkInfo &docInfo, const char *decl,
                      const char *desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { forall(&OutputGenerator::writeTooltip,id,docInfo,decl,desc,defInfo,declInfo); }
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
    void startGroupHeader(int extraLevels=0)
    { forall(&OutputGenerator::startGroupHeader,extraLevels); }
    void endGroupHeader(int extraLevels=0)
    { forall(&OutputGenerator::endGroupHeader,extraLevels); }
    void startItemListItem()
    { forall(&OutputGenerator::startItemListItem); }
    void endItemListItem()
    { forall(&OutputGenerator::endItemListItem); }
    void startMemberSections()
    { forall(&OutputGenerator::startMemberSections); }
    void endMemberSections()
    { forall(&OutputGenerator::endMemberSections); }
    void startHeaderSection()
    { forall(&OutputGenerator::startHeaderSection); }
    void endHeaderSection()
    { forall(&OutputGenerator::endHeaderSection); }
    void startMemberHeader(const char *anchor, int typ = 2)
    { forall(&OutputGenerator::startMemberHeader,anchor,typ); }
    void endMemberHeader()
    { forall(&OutputGenerator::endMemberHeader); }
    void startMemberSubtitle()
    { forall(&OutputGenerator::startMemberSubtitle); }
    void endMemberSubtitle()
    { forall(&OutputGenerator::endMemberSubtitle); }
    void startMemberDocList()
    { forall(&OutputGenerator::startMemberDocList); }
    void endMemberDocList()
    { forall(&OutputGenerator::endMemberDocList); }
    void startMemberList()
    { forall(&OutputGenerator::startMemberList); }
    void endMemberList()
    { forall(&OutputGenerator::endMemberList); }
    void startInlineHeader()
    { forall(&OutputGenerator::startInlineHeader); }
    void endInlineHeader()
    { forall(&OutputGenerator::endInlineHeader); }
    void startAnonTypeScope(int i1)
    { forall(&OutputGenerator::startAnonTypeScope,i1); }
    void endAnonTypeScope(int i1)
    { forall(&OutputGenerator::endAnonTypeScope,i1); }
    void startMemberItem(const char *anchor,int i1,const char *id=0)
    { forall(&OutputGenerator::startMemberItem,anchor,i1,id); }
    void endMemberItem()
    { forall(&OutputGenerator::endMemberItem); }
    void startMemberTemplateParams()
    { forall(&OutputGenerator::startMemberTemplateParams); }
    void endMemberTemplateParams(const char *anchor,const char *inheritId)
    { forall(&OutputGenerator::endMemberTemplateParams,anchor,inheritId); }
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
    void insertMemberAlign(bool templ=FALSE)
    { forall(&OutputGenerator::insertMemberAlign,templ); }
    void insertMemberAlignLeft(int typ=0, bool templ=FALSE)
    { forall(&OutputGenerator::insertMemberAlignLeft,typ,templ); }
    void writeRuler()
    { forall(&OutputGenerator::writeRuler); }
    void writeAnchor(const char *fileName,const char *name)
    { forall(&OutputGenerator::writeAnchor,fileName,name); }
    void startCodeFragment()
    { forall(&OutputGenerator::startCodeFragment); }
    void endCodeFragment()
    { forall(&OutputGenerator::endCodeFragment); }
    void startCodeLine(bool hasLineNumbers)
    { forall(&OutputGenerator::startCodeLine,hasLineNumbers); }
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
                        const char *anchor,const char *title,
                        int memCount,int memTotal,bool showInline)
    { forall(&OutputGenerator::startMemberDoc,clName,memName,anchor,title,memCount,memTotal,showInline); }
    void endMemberDoc(bool hasArgs)
    { forall(&OutputGenerator::endMemberDoc,hasArgs); }
    void startDoxyAnchor(const char *fName,const char *manName,
                         const char *anchor, const char *name,
                         const char *args)
    { forall(&OutputGenerator::startDoxyAnchor,fName,manName,anchor,name,args); }
    void endDoxyAnchor(const char *fn,const char *anchor)
    { forall(&OutputGenerator::endDoxyAnchor,fn,anchor); }
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
    void startDescForItem()
    { forall(&OutputGenerator::startDescForItem); }
    void endDescForItem()
    { forall(&OutputGenerator::endDescForItem); }
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
    void lineBreak(const char *style=0)
    { forall(&OutputGenerator::lineBreak,style); }
    void startBold()
    { forall(&OutputGenerator::startBold); }
    void endBold()
    { forall(&OutputGenerator::endBold); }
    void startMemberDescription(const char *anchor,const char *inheritId=0, bool typ = false)
    { forall(&OutputGenerator::startMemberDescription,anchor,inheritId, typ); }
    void endMemberDescription()
    { forall(&OutputGenerator::endMemberDescription); }
    void startMemberDeclaration()
    { forall(&OutputGenerator::startMemberDeclaration); }
    void endMemberDeclaration(const char *anchor,const char *inheritId)
    { forall(&OutputGenerator::endMemberDeclaration,anchor,inheritId); }
    void writeInheritedSectionTitle(const char *id,   const char *ref,
                                    const char *file, const char *anchor,
                                    const char *title,const char *name)
    { forall(&OutputGenerator::writeInheritedSectionTitle,id,ref,
                                    file,anchor,title,name); }
    void startExamples()
    { forall(&OutputGenerator::startExamples); }
    void endExamples()
    { forall(&OutputGenerator::endExamples); }
    void startParamList(ParamListTypes t,const char *title)
    { forall(&OutputGenerator::startParamList,t,title); }
    void endParamList()
    { forall(&OutputGenerator::endParamList); }
    void startIndent()
    { forall(&OutputGenerator::startIndent); }
    void endIndent()
    { forall(&OutputGenerator::endIndent); }
    void startSection(const char *lab,const char *title,SectionType t)
    { forall(&OutputGenerator::startSection,lab,title,t); }
    void endSection(const char *lab,SectionType t)
    { forall(&OutputGenerator::endSection,lab,t); }
    void addIndexItem(const char *s1,const char *s2)
    { forall(&OutputGenerator::addIndexItem,s1,s2); }
    void writeSynopsis()
    { forall(&OutputGenerator::writeSynopsis); }
    void startClassDiagram()
    { forall(&OutputGenerator::startClassDiagram); }
    void endClassDiagram(const ClassDiagram &d,const char *f,const char *n)
    { forall(&OutputGenerator::endClassDiagram,d,f,n); }
    void startPageRef()
    { forall(&OutputGenerator::startPageRef); }
    void endPageRef(const char *c,const char *a)
    { forall(&OutputGenerator::endPageRef,c,a); }
    void startQuickIndices()
    { forall(&OutputGenerator::startQuickIndices); }
    void endQuickIndices()
    { forall(&OutputGenerator::endQuickIndices); }
    void writeSplitBar(const char *name)
    { forall(&OutputGenerator::writeSplitBar,name); }
    void writeNavigationPath(const char *s)
    { forall(&OutputGenerator::writeNavigationPath,s); }
    void writeLogo()
    { forall(&OutputGenerator::writeLogo); }
    void writeQuickLinks(bool compact,HighlightedItem hli,const char *file)
    { forall(&OutputGenerator::writeQuickLinks,compact,hli,file); }
    void writeSummaryLink(const char *file,const char *anchor,const char *title,bool first)
    { forall(&OutputGenerator::writeSummaryLink,file,anchor,title,first); }
    void startContents()
    { forall(&OutputGenerator::startContents); }
    void endContents()
    { forall(&OutputGenerator::endContents); }
    void startPageDoc(const char *pageTitle)
    { forall(&OutputGenerator::startPageDoc, pageTitle); }
    void endPageDoc()
    { forall(&OutputGenerator::endPageDoc); }
    void writeNonBreakableSpace(int num)
    { forall(&OutputGenerator::writeNonBreakableSpace,num); }
    void startDescTable(const char *title)
    { forall(&OutputGenerator::startDescTable,title); }
    void endDescTable()
    { forall(&OutputGenerator::endDescTable); }
    void startDescTableRow()
    { forall(&OutputGenerator::startDescTableRow); }
    void endDescTableRow()
    { forall(&OutputGenerator::endDescTableRow); }
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
    void startCallGraph()
    { forall(&OutputGenerator::startCallGraph); }
    void endCallGraph(DotCallGraph &g)
    { forall(&OutputGenerator::endCallGraph,g); }
    void startDirDepGraph()
    { forall(&OutputGenerator::startDirDepGraph); }
    void endDirDepGraph(DotDirDeps &g)
    { forall(&OutputGenerator::endDirDepGraph,g); }
    void startGroupCollaboration()
    { forall(&OutputGenerator::startGroupCollaboration); }
    void endGroupCollaboration(DotGroupCollaboration &g)
    { forall(&OutputGenerator::endGroupCollaboration,g); }
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g)
    { forall(&OutputGenerator::writeGraphicalHierarchy,g); }
    void startTextBlock(bool dense=FALSE)
    { forall(&OutputGenerator::startTextBlock,dense); }
    void endTextBlock(bool paraBreak=FALSE)
    { forall(&OutputGenerator::endTextBlock,paraBreak); }
    void lastIndexPage()
    { forall(&OutputGenerator::lastIndexPage); }
    void startMemberDocPrefixItem()
    { forall(&OutputGenerator::startMemberDocPrefixItem); }
    void endMemberDocPrefixItem()
    { forall(&OutputGenerator::endMemberDocPrefixItem); }
    void startMemberDocName(bool align)
    { forall(&OutputGenerator::startMemberDocName,align); }
    void endMemberDocName()
    { forall(&OutputGenerator::endMemberDocName); }
    void startParameterType(bool first,const char *key)
    { forall(&OutputGenerator::startParameterType,first,key); }
    void endParameterType()
    { forall(&OutputGenerator::endParameterType); }
    void startParameterName(bool one)
    { forall(&OutputGenerator::startParameterName,one); }
    void endParameterName(bool last,bool one,bool bracket)
    { forall(&OutputGenerator::endParameterName,last,one,bracket); }
    void startParameterList(bool openBracket)
    { forall(&OutputGenerator::startParameterList,openBracket); }
    void endParameterList()
    { forall(&OutputGenerator::endParameterList); }
    void exceptionEntry(const char* prefix,bool closeBracket)
    { forall(&OutputGenerator::exceptionEntry,prefix,closeBracket); }

    void startConstraintList(const char *header)
    { forall(&OutputGenerator::startConstraintList,header); }
    void startConstraintParam()
    { forall(&OutputGenerator::startConstraintParam); }
    void endConstraintParam()
    { forall(&OutputGenerator::endConstraintParam); }
    void startConstraintType()
    { forall(&OutputGenerator::startConstraintType); }
    void endConstraintType()
    { forall(&OutputGenerator::endConstraintType); }
    void startConstraintDocs()
    { forall(&OutputGenerator::startConstraintDocs); }
    void endConstraintDocs()
    { forall(&OutputGenerator::endConstraintDocs); }
    void endConstraintList()
    { forall(&OutputGenerator::endConstraintList); }

    void startMemberDocSimple(bool b)
    { forall(&OutputGenerator::startMemberDocSimple,b); }
    void endMemberDocSimple(bool b)
    { forall(&OutputGenerator::endMemberDocSimple,b); }
    void startInlineMemberType()
    { forall(&OutputGenerator::startInlineMemberType); }
    void endInlineMemberType()
    { forall(&OutputGenerator::endInlineMemberType); }
    void startInlineMemberName()
    { forall(&OutputGenerator::startInlineMemberName); }
    void endInlineMemberName()
    { forall(&OutputGenerator::endInlineMemberName); }
    void startInlineMemberDoc()
    { forall(&OutputGenerator::startInlineMemberDoc); }
    void endInlineMemberDoc()
    { forall(&OutputGenerator::endInlineMemberDoc); }

    void startLabels()
    { forall(&OutputGenerator::startLabels); }
    void writeLabel(const char *l,bool isLast)
    { forall(&OutputGenerator::writeLabel,l,isLast); }
    void endLabels()
    { forall(&OutputGenerator::endLabels); }

    void startFontClass(const char *c)
    { forall(&OutputGenerator::startFontClass,c); }
    void endFontClass()
    { forall(&OutputGenerator::endFontClass); }
    void writeCodeAnchor(const char *name)
    { forall(&OutputGenerator::writeCodeAnchor,name); }
    void setCurrentDoc(const Definition *context,const char *anchor,bool isSourceFile)
    { forall(&OutputGenerator::setCurrentDoc,context,anchor,isSourceFile); }
    void addWord(const char *word,bool hiPriority)
    { forall(&OutputGenerator::addWord,word,hiPriority); }

    void startPlainFile(const char *name)
    { forall(&OutputGenerator::startPlainFile,name); }
    void endPlainFile()
    { forall(&OutputGenerator::endPlainFile); }

  private:
    void debug();
    void clear();

    // For each output format that is enabled (OutputGenerator::isEnabled()) we forward
    // the method call.
    // We use C++11 variadic templates and perfect forwarding to implement forall() generically,
    // and split the types of the methods from the arguments passed to allow implicit conversions.
    template<typename T,class... Ts,class... As>
    void forall(void (T::*methodPtr)(Ts...),As&&... args)
    {
      for (const auto &og : m_outputs)
      {
        if (og->isEnabled()) (og.get()->*methodPtr)(std::forward<As>(args)...);
      }
    }

    OutputList(const OutputList &ol);
    std::vector< std::unique_ptr<OutputGenerator> > m_outputs;
};

#endif
