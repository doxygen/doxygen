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

class OutputCodeList;

//-------------------------------------------------------------------------------------------

/** Helper template class which defers all methods of OutputCodeIntf to an existing object of the templated type.
 *  This allows to add the same generator to multiple lists (one that owns the element, and others that refers to it).
 */
template<class OutputCodeGen>
class OutputCodeDefer : public OutputCodeIntf
{
  public:
    OutputCodeDefer(OutputCodeGen *codeGen) : m_codeGen(codeGen) {}
    OutputType type() const override { return m_codeGen->type(); }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<OutputCodeDefer>(*this); }
    void codify(const QCString &s) override { m_codeGen->codify(s); }
    void stripCodeComments(bool b) override { m_codeGen->stripCodeComments(b); }
    void startSpecialComment() override { m_codeGen->startSpecialComment(); }
    void endSpecialComment() override { m_codeGen->endSpecialComment(); }
    void setStripIndentAmount(size_t amount) override { m_codeGen->setStripIndentAmount(amount); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) override
    { m_codeGen->writeCodeLink(type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor) override
    { m_codeGen->writeLineNumber(ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) override
    { m_codeGen->writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(int lineNr) override
    { m_codeGen->startCodeLine(lineNr); }

    void endCodeLine() override
    { m_codeGen->endCodeLine(); }

    void startFontClass(const QCString &c) override
    { m_codeGen->startFontClass(c); }

    void endFontClass() override
    { m_codeGen->endFontClass(); }

    void writeCodeAnchor(const QCString &name) override
    { m_codeGen->writeCodeAnchor(name); }

    void startCodeFragment(const QCString &style) override
    { m_codeGen->startCodeFragment(style); }

    void endCodeFragment(const QCString &style) override
    { m_codeGen->endCodeFragment(style); }

    void startFold(int lineNr,const QCString &startMarker,const QCString &endMarker) override
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
class OutputCodeRecorder : public OutputCodeIntf
{
  public:
    OutputType type() const override { return OutputType::Recorder; }
    void codify(const QCString &s) override;
    void stripCodeComments(bool) override {}
    void startSpecialComment() override;
    void endSpecialComment() override;
    void setStripIndentAmount(size_t) override {}
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<OutputCodeRecorder>(*this); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) override;
    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor) override;
    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) override;
    void startCodeLine(int) override;
    void endCodeLine() override;
    void startFontClass(const QCString &c) override;
    void endFontClass() override;
    void writeCodeAnchor(const QCString &name) override;
    void startCodeFragment(const QCString &style) override;
    void endCodeFragment(const QCString &style) override;
    void startFold(int lineNr,const QCString &startMarker,const QCString &endMarker) override;
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

    void codify(const QCString &s)
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
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { foreach(&OutputCodeIntf::writeCodeLink,type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor)
    { foreach(&OutputCodeIntf::writeLineNumber,ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { foreach(&OutputCodeIntf::writeTooltip,id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(int lineNr)
    { foreach(&OutputCodeIntf::startCodeLine,lineNr); }

    void endCodeLine()
    { foreach(&OutputCodeIntf::endCodeLine); }

    void startFontClass(const QCString &c)
    { foreach(&OutputCodeIntf::startFontClass,c); }

    void endFontClass()
    { foreach(&OutputCodeIntf::endFontClass); }

    void writeCodeAnchor(const QCString &name)
    { foreach(&OutputCodeIntf::writeCodeAnchor,name); }

    void startCodeFragment(const QCString &style)
    { foreach(&OutputCodeIntf::startCodeFragment,style); }

    void endCodeFragment(const QCString &style)
    { foreach(&OutputCodeIntf::endCodeFragment,style); }

    void startFold(int lineNr, const QCString &startMarker, const QCString &endMarker)
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

    void generateDoc(const QCString &fileName,int startLine,
                     const Definition *ctx,const MemberDef *md,const QCString &docStr,
                     const DocOptions &options);

    void startFile(const QCString &name, bool isSource,const QCString &manName,const QCString &title, int hierarchyLevel=0);
    void parseText(const QCString &textStr);

    //////////////////////////////////////////////////
    // OutputGenIntf implementation
    //////////////////////////////////////////////////

    void writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md,int sectionLevel=-1)
    { foreach(&OutputGenIntf::writeDoc,ast,ctx,md,m_id,sectionLevel); }
    void startIndexSection(IndexSection is)
    { foreach(&OutputGenIntf::startIndexSection,is); }
    void endIndexSection(IndexSection is)
    { foreach(&OutputGenIntf::endIndexSection,is); }
    void writePageLink(const QCString &name,bool first)
    { foreach(&OutputGenIntf::writePageLink,name,first); }
    void startProjectNumber()
    { foreach(&OutputGenIntf::startProjectNumber); }
    void endProjectNumber()
    { foreach(&OutputGenIntf::endProjectNumber); }
    void writeStyleInfo(int part)
    { foreach(&OutputGenIntf::writeStyleInfo,part); }
    void writeSearchInfo()
    { foreach(&OutputGenIntf::writeSearchInfo); }
    void writeFooter(const QCString &navPath)
    { foreach(&OutputGenIntf::writeFooter,navPath); }
    void endFile()
    { foreach(&OutputGenIntf::endFile); }
    void startTitleHead(const QCString &fileName)
    { foreach(&OutputGenIntf::startTitleHead,fileName); }
    void endTitleHead(const QCString &fileName,const QCString &name)
    { foreach(&OutputGenIntf::endTitleHead,fileName,name); }
    void startParagraph(const QCString &classDef=QCString())
    { foreach(&OutputGenIntf::startParagraph,classDef); }
    void endParagraph()
    { foreach(&OutputGenIntf::endParagraph); }
    void writeString(const QCString &text)
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
    void endIndexValue(const QCString &name,bool b)
    { foreach(&OutputGenIntf::endIndexValue,name,b); }
    void startItemList()
    { foreach(&OutputGenIntf::startItemList); }
    void endItemList()
    { foreach(&OutputGenIntf::endItemList); }
    void startIndexItem(const QCString &ref,const QCString &file)
    { foreach(&OutputGenIntf::startIndexItem,ref,file); }
    void endIndexItem(const QCString &ref,const QCString &file)
    { foreach(&OutputGenIntf::endIndexItem,ref,file); }
    void docify(const QCString &s)
    { foreach(&OutputGenIntf::docify,s); }
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor, const QCString &name)
    { foreach(&OutputGenIntf::writeObjectLink,ref,file,anchor,name); }
    void startTextLink(const QCString &file,const QCString &anchor)
    { foreach(&OutputGenIntf::startTextLink,file,anchor); }
    void endTextLink()
    { foreach(&OutputGenIntf::endTextLink); }
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name)
    { foreach(&OutputGenIntf::writeStartAnnoItem,type,file,path,name); }
    void startTypewriter()
    { foreach(&OutputGenIntf::startTypewriter); }
    void endTypewriter()
    { foreach(&OutputGenIntf::endTypewriter); }
    void startGroupHeader(const QCString &id=QCString(),int extraLevels=0)
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
    void startMemberHeader(const QCString &anchor, int typ = 2)
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
    void startMemberItem(const QCString &anchor,OutputGenerator::MemberItemType type,const QCString &id=QCString())
    { foreach(&OutputGenIntf::startMemberItem,anchor,type,id); }
    void endMemberItem(OutputGenerator::MemberItemType type)
    { foreach(&OutputGenIntf::endMemberItem,type); }
    void startMemberTemplateParams()
    { foreach(&OutputGenIntf::startMemberTemplateParams); }
    void endMemberTemplateParams(const QCString &anchor,const QCString &inheritId)
    { foreach(&OutputGenIntf::endMemberTemplateParams,anchor,inheritId); }
    void startCompoundTemplateParams()
    { foreach(&OutputGenIntf::startCompoundTemplateParams); }
    void endCompoundTemplateParams()
    { foreach(&OutputGenIntf::endCompoundTemplateParams); }
    void startMemberGroupHeader(const QCString &id,bool b)
    { foreach(&OutputGenIntf::startMemberGroupHeader,id,b); }
    void endMemberGroupHeader()
    { foreach(&OutputGenIntf::endMemberGroupHeader); }
    void startMemberGroupDocs()
    { foreach(&OutputGenIntf::startMemberGroupDocs); }
    void endMemberGroupDocs()
    { foreach(&OutputGenIntf::endMemberGroupDocs); }
    void startMemberGroup()
    { foreach(&OutputGenIntf::startMemberGroup); }
    void endMemberGroup(bool last)
    { foreach(&OutputGenIntf::endMemberGroup,last); }
    void insertMemberAlign(bool templ=FALSE)
    { foreach(&OutputGenIntf::insertMemberAlign,templ); }
    void insertMemberAlignLeft(OutputGenerator::MemberItemType typ=OutputGenerator::MemberItemType::Normal, bool templ=FALSE)
    { foreach(&OutputGenIntf::insertMemberAlignLeft,typ,templ); }
    void writeRuler()
    { foreach(&OutputGenIntf::writeRuler); }
    void writeAnchor(const QCString &fileName,const QCString &name)
    { foreach(&OutputGenIntf::writeAnchor,fileName,name); }
    void startEmphasis()
    { foreach(&OutputGenIntf::startEmphasis); }
    void endEmphasis()
    { foreach(&OutputGenIntf::endEmphasis); }
    void writeChar(char c)
    { foreach(&OutputGenIntf::writeChar,c); }
    void startMemberDoc(const QCString &clName,const QCString &memName,
                        const QCString &anchor,const QCString &title,
                        int memCount,int memTotal,bool showInline)
    { foreach(&OutputGenIntf::startMemberDoc,clName,memName,anchor,title,memCount,memTotal,showInline); }
    void endMemberDoc(bool hasArgs)
    { foreach(&OutputGenIntf::endMemberDoc,hasArgs); }
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor, const QCString &name,
                         const QCString &args)
    { foreach(&OutputGenIntf::startDoxyAnchor,fName,manName,anchor,name,args); }
    void endDoxyAnchor(const QCString &fn,const QCString &anchor)
    { foreach(&OutputGenIntf::endDoxyAnchor,fn,anchor); }
    void addLabel(const QCString &fName,const QCString &anchor)
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
    void lineBreak(const QCString &style=QCString())
    { foreach(&OutputGenIntf::lineBreak,style); }
    void startBold()
    { foreach(&OutputGenIntf::startBold); }
    void endBold()
    { foreach(&OutputGenIntf::endBold); }
    void startMemberDescription(const QCString &anchor,const QCString &inheritId=QCString(), bool typ = false)
    { foreach(&OutputGenIntf::startMemberDescription,anchor,inheritId, typ); }
    void endMemberDescription()
    { foreach(&OutputGenIntf::endMemberDescription); }
    void startMemberDeclaration()
    { foreach(&OutputGenIntf::startMemberDeclaration); }
    void endMemberDeclaration(const QCString &anchor,const QCString &inheritId)
    { foreach(&OutputGenIntf::endMemberDeclaration,anchor,inheritId); }
    void writeInheritedSectionTitle(const QCString &id,   const QCString &ref,
                                    const QCString &file, const QCString &anchor,
                                    const QCString &title,const QCString &name)
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
    void startSection(const QCString &lab,const QCString &title,SectionType t)
    { foreach(&OutputGenIntf::startSection,lab,title,t); }
    void endSection(const QCString &lab,SectionType t)
    { foreach(&OutputGenIntf::endSection,lab,t); }
    void addIndexItem(const QCString &s1,const QCString &s2)
    { foreach(&OutputGenIntf::addIndexItem,s1,s2); }
    void writeSynopsis()
    { foreach(&OutputGenIntf::writeSynopsis); }
    void startClassDiagram()
    { foreach(&OutputGenIntf::startClassDiagram); }
    void endClassDiagram(const ClassDiagram &d,const QCString &f,const QCString &n)
    { foreach(&OutputGenIntf::endClassDiagram,d,f,n); }
    void startPageRef()
    { foreach(&OutputGenIntf::startPageRef); }
    void endPageRef(const QCString &c,const QCString &a)
    { foreach(&OutputGenIntf::endPageRef,c,a); }
    void startQuickIndices()
    { foreach(&OutputGenIntf::startQuickIndices); }
    void endQuickIndices()
    { foreach(&OutputGenIntf::endQuickIndices); }
    void writeSplitBar(const QCString &name,const QCString &allMembersFile)
    { foreach(&OutputGenIntf::writeSplitBar,name,allMembersFile); }
    void writeNavigationPath(const QCString &s)
    { foreach(&OutputGenIntf::writeNavigationPath,s); }
    void writeLogo()
    { foreach(&OutputGenIntf::writeLogo); }
    void writeQuickLinks(HighlightedItem hli,const QCString &file,bool extraTabs=false)
    { foreach(&OutputGenIntf::writeQuickLinks,hli,file,extraTabs); }
    void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first)
    { foreach(&OutputGenIntf::writeSummaryLink,file,anchor,title,first); }
    void writePageOutline()
    { foreach(&OutputGenIntf::writePageOutline); }
    void startContents()
    { foreach(&OutputGenIntf::startContents); }
    void endContents()
    { foreach(&OutputGenIntf::endContents); }
    void startPageDoc(const QCString &pageTitle)
    { foreach(&OutputGenIntf::startPageDoc, pageTitle); }
    void endPageDoc()
    { foreach(&OutputGenIntf::endPageDoc); }
    void writeNonBreakableSpace(int num)
    { foreach(&OutputGenIntf::writeNonBreakableSpace,num); }
    void startDescTable(const QCString &title,const bool hasInits)
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
    void startTextBlock(bool dense=FALSE)
    { foreach(&OutputGenIntf::startTextBlock,dense); }
    void endTextBlock(bool paraBreak=FALSE)
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
    void startParameterType(bool first,const QCString &key)
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
    void exceptionEntry(const QCString &prefix,bool closeBracket)
    { foreach(&OutputGenIntf::exceptionEntry,prefix,closeBracket); }
    void startConstraintList(const QCString &header)
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
    void writeLabel(const QCString &l,bool isLast)
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
    void startPlainFile(const QCString &name)
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

#endif
