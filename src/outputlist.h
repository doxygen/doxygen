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
#include <variant>
#include <stack>
#include <functional>

#include "outputgen.h"
#include "doxygen.h"

#include "htmlgen.h"
#include "latexgen.h"
#include "rtfgen.h"
#include "mangen.h"
#include "docbookgen.h"
#include "xmlgen.h"
#include "devnullgen.h"
#include "docoptions.h"
#include "linkifytext.h"

class OutputCodeList;

//-------------------------------------------------------------------------------------------

/** Helper template class which defers all methods of OutputCodeIntf to an existing object of the templated type.
 *  This allows to add the same generator to multiple lists (one that owns the element, and others that refers to it).
 */
template<class OutputCodeGen>
class OutputCodeDefer final : public OutputCodeIntf
{
  public:
    OutputCodeDefer(OutputCodeGen *codeGen) : m_codeGen(codeGen) {}
    OutputType type() const override { return m_codeGen->type(); }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<OutputCodeDefer>(*this); }
    void codify(const DString &s) override { m_codeGen->codify(s); }
    void stripCodeComments(bool b) override { m_codeGen->stripCodeComments(b); }
    void startSpecialComment() override { m_codeGen->startSpecialComment(); }
    void endSpecialComment() override { m_codeGen->endSpecialComment(); }
    void setStripIndentAmount(size_t amount) override { m_codeGen->setStripIndentAmount(amount); }
    void writeCodeLink(CodeSymbolType type,
                       const DString &ref,const DString &file,
                       const DString &anchor,const DString &name,
                       const DString &tooltip) override
    { m_codeGen->writeCodeLink(type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const DString &ref,const DString &file,const DString &anchor,
                         int lineNumber, bool writeLineAnchor) override
    { m_codeGen->writeLineNumber(ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const DString &id, const DocLinkInfo &docInfo, const DString &decl,
                      const DString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) override
    { m_codeGen->writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(int lineNr) override
    { m_codeGen->startCodeLine(lineNr); }

    void endCodeLine() override
    { m_codeGen->endCodeLine(); }

    void startFontClass(const DString &c) override
    { m_codeGen->startFontClass(c); }

    void endFontClass() override
    { m_codeGen->endFontClass(); }

    void writeCodeAnchor(const DString &name) override
    { m_codeGen->writeCodeAnchor(name); }

    void startCodeFragment(const DString &style) override
    { m_codeGen->startCodeFragment(style); }

    void endCodeFragment(const DString &style) override
    { m_codeGen->endCodeFragment(style); }

    void startFold(int lineNr,const DString &startMarker,const DString &endMarker) override
    { m_codeGen->startFold(lineNr,startMarker,endMarker); }

    void endFold() override
    { m_codeGen->endFold(); }

  private:
    OutputCodeGen *m_codeGen;
};

using HtmlCodeGeneratorDefer    = OutputCodeDefer<HtmlCodeGenerator>;
using LatexCodeGeneratorDefer   = OutputCodeDefer<LatexCodeGenerator>;
using RTFCodeGeneratorDefer     = OutputCodeDefer<RTFCodeGenerator>;
using ManCodeGeneratorDefer     = OutputCodeDefer<ManCodeGenerator>;
using DocbookCodeGeneratorDefer = OutputCodeDefer<DocbookCodeGenerator>;
using OutputCodeDeferExtension  = OutputCodeDefer<OutputCodeIntf>;

/** Implementation that allows capturing calls made to the code interface to later
 *  invoke them on a #OutputCodeList via replay().
 */
class OutputCodeRecorder final : public OutputCodeIntf
{
  public:
    OutputType type() const override { return OutputType::Recorder; }
    void codify(const DString &s) override;
    void stripCodeComments(bool) override {}
    void startSpecialComment() override;
    void endSpecialComment() override;
    void setStripIndentAmount(size_t) override {}
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<OutputCodeRecorder>(*this); }
    void writeCodeLink(CodeSymbolType type,
                       const DString &ref,const DString &file,
                       const DString &anchor,const DString &name,
                       const DString &tooltip) override;
    void writeLineNumber(const DString &ref,const DString &file,const DString &anchor,
                         int lineNumber, bool writeLineAnchor) override;
    void writeTooltip(const DString &id, const DocLinkInfo &docInfo, const DString &decl,
                      const DString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const DString &c) override;
    void endFontClass() override;
    void writeCodeAnchor(const DString &name) override;
    void startCodeFragment(const DString &style) override;
    void endCodeFragment(const DString &style) override;
    void startFold(int lineNr,const DString &startMarker,const DString &endMarker) override;
    void endFold() override;

    void replay(OutputCodeList &ol,int startLine,int endLine,bool showLineNumbers,bool stripComment,size_t stripIndentAmount);
  private:
    void startNewLine(int lineNr);
    struct CallInfo
    {
      using ConditionFunc = std::function<bool()>;
      using OutputFunc    = std::function<void(OutputCodeList*)>;
      CallInfo(ConditionFunc &&c,OutputFunc &&f,bool ic)
        : condition(std::move(c)), function(std::move(f)), insideSpecialComment(ic) {}
      ConditionFunc  condition;
      OutputFunc     function;
      bool           insideSpecialComment = false;
    };
    std::vector<CallInfo> m_calls;
    std::vector<size_t>   m_lineOffset;
    bool m_showLineNumbers = false;
    bool m_insideSpecialComment = false;
};


/** Class representing a list of different code generators.
 *  It offers the same interface as the specific code generators,
 *  and will forward each method to all generators that are enabled.
 */
class OutputCodeList
{
  public:
    using OutputCodeIntfPtr = std::unique_ptr<OutputCodeIntf>;

  private:
    struct OutputCodeElem
    {
      explicit OutputCodeElem(OutputCodeIntfPtr &&p) : intf(std::move(p)) {}
      OutputCodeElem(const OutputCodeElem &other)
      {
        intf = other.intf->clone();
        enabled = other.enabled;
      }
      OutputCodeElem &operator=(const OutputCodeElem &other)
      {
        if (&other!=this)
        {
          intf = other.intf->clone();
          enabled = other.enabled;
        }
        return *this;
      }
      OutputCodeIntfPtr intf;
      bool enabled = true;
    };
  public:

    int id() const     { return m_id; }
    void setId(int id) { m_id = id;   }

    void add(OutputCodeIntfPtr &&p)
    {
      m_outputCodeList.emplace_back(std::move(p));
    }

    /** Add a code generator to the list, using a syntax similar to std::make_unique<T>() */
    template<class T,class... As>
    T* add(As&&... args)
    {
      add(std::make_unique<T>(std::forward<As>(args)...));
      return static_cast<T*>(m_outputCodeList.back().intf.get());
    }

    /** Returns a pointer to a specific generator in the list */
    template<class T>
    T *get(OutputType o)
    {
      for (auto &e : m_outputCodeList)
      {
        if (e.intf->type()==o) { return static_cast<T*>(e.intf.get()); }
      }
      return nullptr;
    }

    /** Enable or disable a specific generator */
    void setEnabledFiltered(OutputType o,bool enabled)
    {
      for (auto &e : m_outputCodeList)
      {
        if (e.intf->type()==o) e.enabled = enabled;
      }
    }

    void clear()
    {
      m_outputCodeList.clear();
    }

    // ---- OutputCodeIntf forwarding

    void codify(const DString &s)
    { foreach(&OutputCodeIntf::codify,s); }

    void stripCodeComments(bool b)
    { foreach(&OutputCodeIntf::stripCodeComments,b); }

    void startSpecialComment()
    { foreach(&OutputCodeIntf::startSpecialComment); }

    void endSpecialComment()
    { foreach(&OutputCodeIntf::endSpecialComment); }

    void setStripIndentAmount(size_t amount)
    { foreach(&OutputCodeIntf::setStripIndentAmount,amount); }

    void writeCodeLink(CodeSymbolType type,
                       const DString &ref,const DString &file,
                       const DString &anchor,const DString &name,
                       const DString &tooltip)
    { foreach(&OutputCodeIntf::writeCodeLink,type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const DString &ref,const DString &file,const DString &anchor,
                         int lineNumber, bool writeLineAnchor)
    { foreach(&OutputCodeIntf::writeLineNumber,ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const DString &id, const DocLinkInfo &docInfo, const DString &decl,
                      const DString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { foreach(&OutputCodeIntf::writeTooltip,id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(int lineNr)
    { foreach(&OutputCodeIntf::startCodeLine,lineNr); }

    void endCodeLine()
    { foreach(&OutputCodeIntf::endCodeLine); }

    void startFontClass(const DString &c)
    { foreach(&OutputCodeIntf::startFontClass,c); }

    void endFontClass()
    { foreach(&OutputCodeIntf::endFontClass); }

    void writeCodeAnchor(const DString &name)
    { foreach(&OutputCodeIntf::writeCodeAnchor,name); }

    void startCodeFragment(const DString &style)
    { foreach(&OutputCodeIntf::startCodeFragment,style); }

    void endCodeFragment(const DString &style)
    { foreach(&OutputCodeIntf::endCodeFragment,style); }

    void startFold(int lineNr, const DString &startMarker, const DString &endMarker)
    { foreach(&OutputCodeIntf::startFold,lineNr,startMarker,endMarker); }

    void endFold()
    { foreach(&OutputCodeIntf::endFold); }

  private:
    template<class... Ts, class... As>
    void foreach(void (OutputCodeIntf::*methodPtr)(Ts...),As&&... args)
    {
      for (auto &e : m_outputCodeList)
      {
        if (e.enabled)
        {
          (e.intf.get()->*methodPtr)(std::forward<As>(args)...);
        }
      }
    }

    std::vector<OutputCodeElem> m_outputCodeList;
    int m_id = -1;

};

//-------------------------------------------------------------------------------------------

/** Class representing a list of output generators that are written to in parallel.
 */
class OutputList
{
  public:
    using OutputGenIntfPtr = std::unique_ptr<OutputGenIntf>;

  private:
    struct OutputGenElem
    {
      explicit OutputGenElem(OutputGenIntfPtr &&v) : intf(std::move(v)) {}
      OutputGenElem(const OutputGenElem &other)
      {
        intf = other.intf->clone();
        enabled = other.enabled;
      }
      OutputGenElem &operator=(const OutputGenElem &other)
      {
        if (&other!=this)
        {
          intf = other.intf->clone();
          enabled = other.enabled;
        }
        return *this;
      }
      OutputGenIntfPtr intf;
      void setEnabled(bool e) { enabled = e && !enabledStack.empty() ? enabledStack.top() : e; }
      bool enabled = true;
      std::stack<bool> enabledStack;
    };

  public:
    OutputList();
    OutputList(const OutputList &ol);
    OutputList &operator=(const OutputList &ol);
    OutputList(OutputList &&) = delete;
    OutputList &operator=(OutputList &&) = delete;
   ~OutputList() = default;

    template<class DocGenerator>
    void add()
    {
      m_outputGenList.emplace_back(std::make_unique<DocGenerator>());
      refreshCodeGenerators();
    }

    const OutputCodeList &codeGenerators() const { return m_codeGenList; }
    OutputCodeList &codeGenerators()             { return m_codeGenList; }

    size_t size() const { return m_outputGenList.size(); }

    void enableAll();
    void disableAll();
    void disable(OutputType o);
    void enable(OutputType o);
    bool isEnabled(OutputType o);
    void disableAllBut(OutputType o);
    void pushGeneratorState();
    void popGeneratorState();

    void generateDoc(const DString &fileName,int startLine,
                     const Definition *ctx,const MemberDef *md,const DString &docStr,
                     const DocOptions &options);

    void startFile(const DString &name, bool isSource,const DString &manName,const DString &title, int hierarchyLevel=0);
    void parseText(const DString &textStr);

    //////////////////////////////////////////////////
    // OutputGenIntf implementation
    //////////////////////////////////////////////////

    void writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md,int sectionLevel=-1)
    { foreach(&OutputGenIntf::writeDoc,ast,ctx,md,m_id,sectionLevel); }
    void startIndexSection(IndexSection is)
    { foreach(&OutputGenIntf::startIndexSection,is); }
    void endIndexSection(IndexSection is)
    { foreach(&OutputGenIntf::endIndexSection,is); }
    void writePageLink(const DString &name,bool first)
    { foreach(&OutputGenIntf::writePageLink,name,first); }
    void startProjectNumber()
    { foreach(&OutputGenIntf::startProjectNumber); }
    void endProjectNumber()
    { foreach(&OutputGenIntf::endProjectNumber); }
    void writeStyleInfo(int part)
    { foreach(&OutputGenIntf::writeStyleInfo,part); }
    void writeSearchInfo()
    { foreach(&OutputGenIntf::writeSearchInfo); }
    void writeFooter(const DString &navPath)
    { foreach(&OutputGenIntf::writeFooter,navPath); }
    void endFile()
    { foreach(&OutputGenIntf::endFile); }
    void startTitleHead(const DString &fileName)
    { foreach(&OutputGenIntf::startTitleHead,fileName); }
    void endTitleHead(const DString &fileName,const DString &name)
    { foreach(&OutputGenIntf::endTitleHead,fileName,name); }
    void startParagraph(const DString &classDef=DString())
    { foreach(&OutputGenIntf::startParagraph,classDef); }
    void endParagraph()
    { foreach(&OutputGenIntf::endParagraph); }
    void writeString(const DString &text)
    { foreach(&OutputGenIntf::writeString,text); }
    void startIndexListItem()
    { foreach(&OutputGenIntf::startIndexListItem); }
    void endIndexListItem()
    { foreach(&OutputGenIntf::endIndexListItem); }
    void startIndexList()
    { foreach(&OutputGenIntf::startIndexList); }
    void endIndexList()
    { foreach(&OutputGenIntf::endIndexList); }
    void startIndexKey()
    { foreach(&OutputGenIntf::startIndexKey); }
    void endIndexKey()
    { foreach(&OutputGenIntf::endIndexKey); }
    void startIndexValue(bool b)
    { foreach(&OutputGenIntf::startIndexValue,b); }
    void endIndexValue(const DString &name,bool b)
    { foreach(&OutputGenIntf::endIndexValue,name,b); }
    void startItemList()
    { foreach(&OutputGenIntf::startItemList); }
    void endItemList()
    { foreach(&OutputGenIntf::endItemList); }
    void startIndexItem(const DString &ref,const DString &file)
    { foreach(&OutputGenIntf::startIndexItem,ref,file); }
    void endIndexItem(const DString &ref,const DString &file)
    { foreach(&OutputGenIntf::endIndexItem,ref,file); }
    void docify(const DString &s)
    { foreach(&OutputGenIntf::docify,s); }
    void writeObjectLink(const DString &ref,const DString &file,
                         const DString &anchor, const DString &name)
    { foreach(&OutputGenIntf::writeObjectLink,ref,file,anchor,name); }
    void startTextLink(const DString &file,const DString &anchor)
    { foreach(&OutputGenIntf::startTextLink,file,anchor); }
    void endTextLink()
    { foreach(&OutputGenIntf::endTextLink); }
    void writeStartAnnoItem(const DString &type,const DString &file,
                            const DString &path,const DString &name)
    { foreach(&OutputGenIntf::writeStartAnnoItem,type,file,path,name); }
    void startTypewriter()
    { foreach(&OutputGenIntf::startTypewriter); }
    void endTypewriter()
    { foreach(&OutputGenIntf::endTypewriter); }
    void startGroupHeader(const DString &id=DString(),int extraLevels=0)
    { foreach(&OutputGenIntf::startGroupHeader,id,extraLevels); }
    void endGroupHeader(int extraLevels=0)
    { foreach(&OutputGenIntf::endGroupHeader,extraLevels); }
    void startItemListItem()
    { foreach(&OutputGenIntf::startItemListItem); }
    void endItemListItem()
    { foreach(&OutputGenIntf::endItemListItem); }
    void startMemberSections()
    { foreach(&OutputGenIntf::startMemberSections); }
    void endMemberSections()
    { foreach(&OutputGenIntf::endMemberSections); }
    void startHeaderSection()
    { foreach(&OutputGenIntf::startHeaderSection); }
    void endHeaderSection()
    { foreach(&OutputGenIntf::endHeaderSection); }
    void startMemberHeader(const DString &anchor, int typ = 2)
    { foreach(&OutputGenIntf::startMemberHeader,anchor,typ); }
    void endMemberHeader()
    { foreach(&OutputGenIntf::endMemberHeader); }
    void startMemberSubtitle()
    { foreach(&OutputGenIntf::startMemberSubtitle); }
    void endMemberSubtitle()
    { foreach(&OutputGenIntf::endMemberSubtitle); }
    void startMemberDocList()
    { foreach(&OutputGenIntf::startMemberDocList); }
    void endMemberDocList()
    { foreach(&OutputGenIntf::endMemberDocList); }
    void startMemberList()
    { foreach(&OutputGenIntf::startMemberList); }
    void endMemberList()
    { foreach(&OutputGenIntf::endMemberList); }
    void startInlineHeader()
    { foreach(&OutputGenIntf::startInlineHeader); }
    void endInlineHeader()
    { foreach(&OutputGenIntf::endInlineHeader); }
    void startAnonTypeScope(int i1)
    { foreach(&OutputGenIntf::startAnonTypeScope,i1); }
    void endAnonTypeScope(int i1)
    { foreach(&OutputGenIntf::endAnonTypeScope,i1); }
    void startMemberItem(const DString &anchor,OutputGenerator::MemberItemType type,const DString &id=DString())
    { foreach(&OutputGenIntf::startMemberItem,anchor,type,id); }
    void endMemberItem(OutputGenerator::MemberItemType type)
    { foreach(&OutputGenIntf::endMemberItem,type); }
    void startMemberTemplateParams()
    { foreach(&OutputGenIntf::startMemberTemplateParams); }
    void endMemberTemplateParams(const DString &anchor,const DString &inheritId)
    { foreach(&OutputGenIntf::endMemberTemplateParams,anchor,inheritId); }
    void startCompoundTemplateParams()
    { foreach(&OutputGenIntf::startCompoundTemplateParams); }
    void endCompoundTemplateParams()
    { foreach(&OutputGenIntf::endCompoundTemplateParams); }
    void startMemberGroupHeader(const DString &id,bool b)
    { foreach(&OutputGenIntf::startMemberGroupHeader,id,b); }
    void endMemberGroupHeader(bool b)
    { foreach(&OutputGenIntf::endMemberGroupHeader,b); }
    void startMemberGroupDocs()
    { foreach(&OutputGenIntf::startMemberGroupDocs); }
    void endMemberGroupDocs()
    { foreach(&OutputGenIntf::endMemberGroupDocs); }
    void startMemberGroup()
    { foreach(&OutputGenIntf::startMemberGroup); }
    void endMemberGroup(bool last)
    { foreach(&OutputGenIntf::endMemberGroup,last); }
    void insertMemberAlign(bool templ=false)
    { foreach(&OutputGenIntf::insertMemberAlign,templ); }
    void insertMemberAlignLeft(OutputGenerator::MemberItemType typ=OutputGenerator::MemberItemType::Normal, bool templ=false)
    { foreach(&OutputGenIntf::insertMemberAlignLeft,typ,templ); }
    void writeRuler()
    { foreach(&OutputGenIntf::writeRuler); }
    void writeAnchor(const DString &fileName,const DString &name)
    { foreach(&OutputGenIntf::writeAnchor,fileName,name); }
    void startEmphasis()
    { foreach(&OutputGenIntf::startEmphasis); }
    void endEmphasis()
    { foreach(&OutputGenIntf::endEmphasis); }
    void writeChar(char c)
    { foreach(&OutputGenIntf::writeChar,c); }
    void startMemberDoc(const DString &clName,const DString &memName,
                        const DString &anchor,const DString &title,
                        int memCount,int memTotal,bool showInline)
    { foreach(&OutputGenIntf::startMemberDoc,clName,memName,anchor,title,memCount,memTotal,showInline); }
    void endMemberDoc(bool hasArgs)
    { foreach(&OutputGenIntf::endMemberDoc,hasArgs); }
    void startDoxyAnchor(const DString &fName,const DString &manName,
                         const DString &anchor, const DString &name,
                         const DString &args)
    { foreach(&OutputGenIntf::startDoxyAnchor,fName,manName,anchor,name,args); }
    void endDoxyAnchor(const DString &fn,const DString &anchor)
    { foreach(&OutputGenIntf::endDoxyAnchor,fn,anchor); }
    void addLabel(const DString &fName,const DString &anchor)
    { foreach(&OutputGenIntf::addLabel,fName,anchor); }
    void writeLatexSpacing()
    { foreach(&OutputGenIntf::writeLatexSpacing); }
    void startDescForItem()
    { foreach(&OutputGenIntf::startDescForItem); }
    void endDescForItem()
    { foreach(&OutputGenIntf::endDescForItem); }
    void startCenter()
    { foreach(&OutputGenIntf::startCenter); }
    void endCenter()
    { foreach(&OutputGenIntf::endCenter); }
    void startSmall()
    { foreach(&OutputGenIntf::startSmall); }
    void endSmall()
    { foreach(&OutputGenIntf::endSmall); }
    void lineBreak(const DString &style=DString())
    { foreach(&OutputGenIntf::lineBreak,style); }
    void startBold()
    { foreach(&OutputGenIntf::startBold); }
    void endBold()
    { foreach(&OutputGenIntf::endBold); }
    void startMemberDescription(const DString &anchor,const DString &inheritId=DString(), bool typ = false)
    { foreach(&OutputGenIntf::startMemberDescription,anchor,inheritId, typ); }
    void endMemberDescription()
    { foreach(&OutputGenIntf::endMemberDescription); }
    void startMemberDeclaration()
    { foreach(&OutputGenIntf::startMemberDeclaration); }
    void endMemberDeclaration(const DString &anchor,const DString &inheritId)
    { foreach(&OutputGenIntf::endMemberDeclaration,anchor,inheritId); }
    void writeInheritedSectionTitle(const DString &id,   const DString &ref,
                                    const DString &file, const DString &anchor,
                                    const DString &title,const DString &name)
    { foreach(&OutputGenIntf::writeInheritedSectionTitle,id,ref,
                                    file,anchor,title,name); }
    void startExamples()
    { foreach(&OutputGenIntf::startExamples); }
    void endExamples()
    { foreach(&OutputGenIntf::endExamples); }
    void startIndent()
    { foreach(&OutputGenIntf::startIndent); }
    void endIndent()
    { foreach(&OutputGenIntf::endIndent); }
    void startSection(const DString &lab,const DString &title,SectionType t)
    { foreach(&OutputGenIntf::startSection,lab,title,t); }
    void endSection(const DString &lab,SectionType t)
    { foreach(&OutputGenIntf::endSection,lab,t); }
    void addIndexItem(const DString &s1,const DString &s2)
    { foreach(&OutputGenIntf::addIndexItem,s1,s2); }
    void writeSynopsis()
    { foreach(&OutputGenIntf::writeSynopsis); }
    void startClassDiagram()
    { foreach(&OutputGenIntf::startClassDiagram); }
    void endClassDiagram(const ClassDiagram &d,const DString &f,const DString &n)
    { foreach(&OutputGenIntf::endClassDiagram,d,f,n); }
    void startPageRef()
    { foreach(&OutputGenIntf::startPageRef); }
    void endPageRef(const DString &c,const DString &a)
    { foreach(&OutputGenIntf::endPageRef,c,a); }
    void startQuickIndices()
    { foreach(&OutputGenIntf::startQuickIndices); }
    void endQuickIndices()
    { foreach(&OutputGenIntf::endQuickIndices); }
    void writeSplitBar(const DString &name,const DString &allMembersFile)
    { foreach(&OutputGenIntf::writeSplitBar,name,allMembersFile); }
    void writeNavigationPath(const DString &s)
    { foreach(&OutputGenIntf::writeNavigationPath,s); }
    void writeLogo()
    { foreach(&OutputGenIntf::writeLogo); }
    void writeQuickLinks(HighlightedItem hli,const DString &file,bool extraTabs=false)
    { foreach(&OutputGenIntf::writeQuickLinks,hli,file,extraTabs); }
    void writeSummaryLink(const DString &file,const DString &anchor,const DString &title,bool first)
    { foreach(&OutputGenIntf::writeSummaryLink,file,anchor,title,first); }
    void writePageOutline()
    { foreach(&OutputGenIntf::writePageOutline); }
    void startContents()
    { foreach(&OutputGenIntf::startContents); }
    void endContents()
    { foreach(&OutputGenIntf::endContents); }
    void startPageDoc(const DString &pageTitle)
    { foreach(&OutputGenIntf::startPageDoc, pageTitle); }
    void endPageDoc()
    { foreach(&OutputGenIntf::endPageDoc); }
    void writeNonBreakableSpace(int num)
    { foreach(&OutputGenIntf::writeNonBreakableSpace,num); }
    void startDescTable(const DString &title,const bool hasInits)
    { foreach(&OutputGenIntf::startDescTable,title,hasInits); }
    void endDescTable()
    { foreach(&OutputGenIntf::endDescTable); }
    void startDescTableRow()
    { foreach(&OutputGenIntf::startDescTableRow); }
    void endDescTableRow()
    { foreach(&OutputGenIntf::endDescTableRow); }
    void startDescTableTitle()
    { foreach(&OutputGenIntf::startDescTableTitle); }
    void endDescTableTitle()
    { foreach(&OutputGenIntf::endDescTableTitle); }
    void startDescTableInit()
    { foreach(&OutputGenIntf::startDescTableInit); }
    void endDescTableInit()
    { foreach(&OutputGenIntf::endDescTableInit); }
    void startDescTableData()
    { foreach(&OutputGenIntf::startDescTableData); }
    void endDescTableData()
    { foreach(&OutputGenIntf::endDescTableData); }
    void startDotGraph()
    { foreach(&OutputGenIntf::startDotGraph); }
    void endDotGraph(DotClassGraph &g)
    { foreach(&OutputGenIntf::endDotGraph,g); }
    void startInclDepGraph()
    { foreach(&OutputGenIntf::startInclDepGraph); }
    void endInclDepGraph(DotInclDepGraph &g)
    { foreach(&OutputGenIntf::endInclDepGraph,g); }
    void startCallGraph()
    { foreach(&OutputGenIntf::startCallGraph); }
    void endCallGraph(DotCallGraph &g)
    { foreach(&OutputGenIntf::endCallGraph,g); }
    void startDirDepGraph()
    { foreach(&OutputGenIntf::startDirDepGraph); }
    void endDirDepGraph(DotDirDeps &g)
    { foreach(&OutputGenIntf::endDirDepGraph,g); }
    void startGroupCollaboration()
    { foreach(&OutputGenIntf::startGroupCollaboration); }
    void endGroupCollaboration(DotGroupCollaboration &g)
    { foreach(&OutputGenIntf::endGroupCollaboration,g); }
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g)
    { foreach(&OutputGenIntf::writeGraphicalHierarchy,g); }
    void startTextBlock(bool dense=false)
    { foreach(&OutputGenIntf::startTextBlock,dense); }
    void endTextBlock(bool paraBreak=false)
    { foreach(&OutputGenIntf::endTextBlock,paraBreak); }
    void lastIndexPage()
    { foreach(&OutputGenIntf::lastIndexPage); }
    void startMemberDocPrefixItem()
    { foreach(&OutputGenIntf::startMemberDocPrefixItem); }
    void endMemberDocPrefixItem()
    { foreach(&OutputGenIntf::endMemberDocPrefixItem); }
    void startMemberDocName(bool align)
    { foreach(&OutputGenIntf::startMemberDocName,align); }
    void endMemberDocName()
    { foreach(&OutputGenIntf::endMemberDocName); }
    void startParameterType(bool first,const DString &key)
    { foreach(&OutputGenIntf::startParameterType,first,key); }
    void endParameterType()
    { foreach(&OutputGenIntf::endParameterType); }
    void startParameterName(bool one)
    { foreach(&OutputGenIntf::startParameterName,one); }
    void endParameterName()
    { foreach(&OutputGenIntf::endParameterName); }
    void startParameterExtra()
    { foreach(&OutputGenIntf::startParameterExtra); }
    void endParameterExtra(bool last,bool one,bool bracket)
    { foreach(&OutputGenIntf::endParameterExtra,last,one,bracket); }
    void startParameterDefVal(const char *separator)
    { foreach(&OutputGenIntf::startParameterDefVal,separator); }
    void endParameterDefVal()
    { foreach(&OutputGenIntf::endParameterDefVal); }
    void startParameterList(bool openBracket)
    { foreach(&OutputGenIntf::startParameterList,openBracket); }
    void endParameterList()
    { foreach(&OutputGenIntf::endParameterList); }
    void exceptionEntry(const DString &prefix,bool closeBracket)
    { foreach(&OutputGenIntf::exceptionEntry,prefix,closeBracket); }
    void startConstraintList(const DString &header)
    { foreach(&OutputGenIntf::startConstraintList,header); }
    void startConstraintParam()
    { foreach(&OutputGenIntf::startConstraintParam); }
    void endConstraintParam()
    { foreach(&OutputGenIntf::endConstraintParam); }
    void startConstraintType()
    { foreach(&OutputGenIntf::startConstraintType); }
    void endConstraintType()
    { foreach(&OutputGenIntf::endConstraintType); }
    void startConstraintDocs()
    { foreach(&OutputGenIntf::startConstraintDocs); }
    void endConstraintDocs()
    { foreach(&OutputGenIntf::endConstraintDocs); }
    void endConstraintList()
    { foreach(&OutputGenIntf::endConstraintList); }
    void startMemberDocSimple(bool b)
    { foreach(&OutputGenIntf::startMemberDocSimple,b); }
    void endMemberDocSimple(bool b)
    { foreach(&OutputGenIntf::endMemberDocSimple,b); }
    void startInlineMemberType()
    { foreach(&OutputGenIntf::startInlineMemberType); }
    void endInlineMemberType()
    { foreach(&OutputGenIntf::endInlineMemberType); }
    void startInlineMemberName()
    { foreach(&OutputGenIntf::startInlineMemberName); }
    void endInlineMemberName()
    { foreach(&OutputGenIntf::endInlineMemberName); }
    void startInlineMemberDoc()
    { foreach(&OutputGenIntf::startInlineMemberDoc); }
    void endInlineMemberDoc()
    { foreach(&OutputGenIntf::endInlineMemberDoc); }
    void startLabels()
    { foreach(&OutputGenIntf::startLabels); }
    void writeLabel(const DString &l,bool isLast)
    { foreach(&OutputGenIntf::writeLabel,l,isLast); }
    void endLabels()
    { foreach(&OutputGenIntf::endLabels); }
    void startLocalToc(int level)
    { foreach(&OutputGenIntf::startLocalToc,level); }
    void endLocalToc()
    { foreach(&OutputGenIntf::endLocalToc); }
    void startTocEntry(const SectionInfo *si)
    { foreach(&OutputGenIntf::startTocEntry,si); }
    void endTocEntry(const SectionInfo *si)
    { foreach(&OutputGenIntf::endTocEntry,si); }
    void cleanup()
    { foreach(&OutputGenIntf::cleanup); }
    void startPlainFile(const DString &name)
    { foreach(&OutputGenIntf::startPlainFile,name); }
    void endPlainFile()
    { foreach(&OutputGenIntf::endPlainFile); }
    void startEmbeddedDoc(int indent)
    { foreach(&OutputGenIntf::startEmbeddedDoc,indent); }
    void endEmbeddedDoc()
    { foreach(&OutputGenIntf::endEmbeddedDoc); }

  private:
    void newId();
    void syncEnabled();
    void refreshCodeGenerators();

    template<class... Ts, class... As>
    void foreach(void (OutputGenIntf::*methodPtr)(Ts...),As&&... args)
    {
      for (auto &e : m_outputGenList)
      {
        if (e.enabled)
        {
          (e.intf.get()->*methodPtr)(std::forward<As>(args)...);
        }
      }
    }

    std::vector<OutputGenElem> m_outputGenList;
    OutputCodeList m_codeGenList;
    int m_id;

};

//-------------------------------------------------------------------------------------------

/** Implements TextGeneratorIntf for an OutputDocInterface stream. */
class TextGeneratorOLImpl final : public TextGeneratorIntf
{
  public:
    TextGeneratorOLImpl(OutputList &ol);
    void writeString(std::string_view s,bool keepSpaces) const override;
    void writeBreak(int indent) const override;
    void writeLink(const DString &extRef,const DString &file,
                   const DString &anchor,std::string_view text
                  ) const override;
  private:
    OutputList &m_ol;
};


#endif
