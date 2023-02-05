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

#include "outputgen.h"
#include "doxygen.h"

#include "dispatcher.h"

#include "htmlgen.h"
#include "latexgen.h"
#include "rtfgen.h"
#include "mangen.h"
#include "docbookgen.h"
#include "xmlgen.h"
#include "devnullgen.h"

class ClassDiagram;
class DotClassGraph;
class DotDirDeps;
class DotInclDepGraph;
class DotGfxHierarchyTable;
class DotGroupCollaboration;

/** Namespace containing typed wrappers to refer to member functions using a fixed method name and fixed condition function.
 *  To be used in combination with display_call_if()
 */
namespace OutputCodeIntf
{
  template <class T> struct codify            { static constexpr auto method = &T::codify;            };
  template <class T> struct writeCodeLink     { static constexpr auto method = &T::writeCodeLink;     };
  template <class T> struct writeLineNumber   { static constexpr auto method = &T::writeLineNumber;   };
  template <class T> struct writeTooltip      { static constexpr auto method = &T::writeTooltip;      };
  template <class T> struct startCodeLine     { static constexpr auto method = &T::startCodeLine;     };
  template <class T> struct endCodeLine       { static constexpr auto method = &T::endCodeLine;       };
  template <class T> struct startFontClass    { static constexpr auto method = &T::startFontClass;    };
  template <class T> struct endFontClass      { static constexpr auto method = &T::endFontClass;      };
  template <class T> struct writeCodeAnchor   { static constexpr auto method = &T::writeCodeAnchor;   };
  template <class T> struct startCodeFragment { static constexpr auto method = &T::startCodeFragment; };
  template <class T> struct endCodeFragment   { static constexpr auto method = &T::endCodeFragment;   };
}

/** Helper template class which defers all methods of OutputCodeIntf to an existing object of the templated type.
 *  This allows to add the same generator to multiple lists (one that owns the element, and others that reference to it).
 */
template<class OutputCodeGen>
class OutputCodeDefer
{
  public:
    OutputCodeDefer(OutputCodeGen *codeGen) : m_codeGen(codeGen) {}
    OutputType type() const { return m_codeGen->type(); }
    void codify(const QCString &s) { m_codeGen->codify(s); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { m_codeGen->writeCodeLink(type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor)
    { m_codeGen->writeLineNumber(ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { m_codeGen->writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(bool hasLineNumbers)
    { m_codeGen->startCodeLine(hasLineNumbers); }

    void endCodeLine()
    { m_codeGen->endCodeLine(); }

    void startFontClass(const QCString &c)
    { m_codeGen->startFontClass(c); }

    void endFontClass()
    { m_codeGen->endFontClass(); }

    void writeCodeAnchor(const QCString &name)
    { m_codeGen->writeCodeAnchor(name); }

    void startCodeFragment(const QCString &style)
    { m_codeGen->startCodeFragment(style); }

    void endCodeFragment(const QCString &style)
    { m_codeGen->endCodeFragment(style); }

  private:
    OutputCodeGen *m_codeGen;
};

/** Base class that allows alternative implementations outside of the fixed set supported by OutputCodeList.
 *  Used by doxyapp and doxyparse for instance.
 */
class OutputCodeExtension
{
  public:
    virtual ~OutputCodeExtension() = default;
    virtual OutputType type() const = 0;
    virtual void codify(const QCString &s) = 0;
    virtual void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) = 0;
    virtual void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor) = 0;
    virtual void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) = 0;
    virtual void startCodeLine(bool hasLineNumbers) = 0;
    virtual void endCodeLine() = 0;
    virtual void startFontClass(const QCString &c) = 0;
    virtual void endFontClass() = 0;
    virtual void writeCodeAnchor(const QCString &name) = 0;
    virtual void startCodeFragment(const QCString &style) = 0;
    virtual void endCodeFragment(const QCString &style) = 0;
};

using OutputCodeDeferHtml      = OutputCodeDefer<HtmlCodeGenerator>;
using OutputCodeDeferLatex     = OutputCodeDefer<LatexCodeGenerator>;
using OutputCodeDeferRTF       = OutputCodeDefer<RTFCodeGenerator>;
using OutputCodeDeferMan       = OutputCodeDefer<ManCodeGenerator>;
using OutputCodeDeferDocbook   = OutputCodeDefer<DocbookCodeGenerator>;
using OutputCodeDeferExtension = OutputCodeDefer<OutputCodeExtension>;

/** Class representing a list of different code generators.
 *  It offers the same interface as the specific code generators,
 *  and will forward each method to all generators that are enabled.
 */
class OutputCodeList
{
  public:
    using OutputCodeVariant = std::variant<HtmlCodeGenerator,
                                           LatexCodeGenerator,
                                           RTFCodeGenerator,
                                           ManCodeGenerator,
                                           XMLCodeGenerator,
                                           DocbookCodeGenerator,
                                           DevNullCodeGenerator,
                                           OutputCodeDeferHtml,
                                           OutputCodeDeferLatex,
                                           OutputCodeDeferRTF,
                                           OutputCodeDeferMan,
                                           OutputCodeDeferDocbook,
                                           OutputCodeDeferExtension
                                          >;

    int id() const     { return m_id; }
    void setId(int id) { m_id = id;   }

    /** Add a code generator already wrapped in a variant type. */
    void add(OutputCodeVariant &&v)
    {
      m_outputCodeList.push_back(OutputCodeElem(std::move(v),true));
    }

    /** Add a code generator to the list, using a syntax similar to std::make_unique<T>() */
    template<class T,class... As>
    void add(As&&... args)
    {
      add(OutputCodeVariant{std::in_place_type<T>,std::forward<As>(args)...});
    }

    /** Returns a pointer to a specific generator in the list */
    template<class T>
    T *get()
    {
      for (auto &e : m_outputCodeList)
      {
        T *t = std::get_if<T>(&e.variant);
        if (t) return t;
      }
      return nullptr;
    }

    /** Enable or disable a specific generator */
    void setEnabledFiltered(OutputType o,bool enabled)
    {
      for (auto &e : m_outputCodeList)
      {
        std::visit([&](auto &&arg) { if (arg.type()==o) e.enabled = enabled; },e.variant);
      }
    }

    // ---- OutputCodeIntf forwarding

    void codify(const QCString &s)
    { foreach<OutputCodeIntf::codify>(s); }

    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { foreach<OutputCodeIntf::writeCodeLink>(type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor)
    { foreach<OutputCodeIntf::writeLineNumber>(ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { foreach<OutputCodeIntf::writeTooltip>(id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(bool hasLineNumbers)
    { foreach<OutputCodeIntf::startCodeLine>(hasLineNumbers); }

    void endCodeLine()
    { foreach<OutputCodeIntf::endCodeLine>(); }

    void startFontClass(const QCString &c)
    { foreach<OutputCodeIntf::startFontClass>(c); }

    void endFontClass()
    { foreach<OutputCodeIntf::endFontClass>(); }

    void writeCodeAnchor(const QCString &name)
    { foreach<OutputCodeIntf::writeCodeAnchor>(name); }

    void startCodeFragment(const QCString &style)
    { foreach<OutputCodeIntf::startCodeFragment>(style); }

    void endCodeFragment(const QCString &style)
    { foreach<OutputCodeIntf::endCodeFragment>(style); }

  private:
    template<template <class> class GeneratorT, class... As>
    void foreach(As&&... args)
    {
      for (auto &e : m_outputCodeList)
      {
        if (e.enabled)
        {
          dispatch_call<GeneratorT>(e.variant,std::forward<As>(args)...);
        }
      }
    }

    struct OutputCodeElem
    {
      OutputCodeElem(OutputCodeVariant &&v,bool e) : variant(std::move(v)),enabled(e) {}
      OutputCodeVariant variant;
      bool enabled;
    };

    std::vector<OutputCodeElem> m_outputCodeList;
    int m_id = -1;

};

/** Class representing a list of output generators that are written to
 *  in parallel.
 */
class OutputList
{
  public:
    OutputList();
    OutputList(const OutputList &ol);
    OutputList &operator=(const OutputList &ol);
    virtual ~OutputList();

    template<class DocGenerator>
    void add()
    {
      auto docGen = std::make_unique<DocGenerator>();
      docGen->addCodeGen(m_codeGenList);
      m_codeGenList.setId(m_id);
      m_outputs.emplace_back(std::move(docGen));
    }

    const OutputCodeList &codeGenerators() const { return m_codeGenList; }
    OutputCodeList &codeGenerators()             { return m_codeGenList; }

    size_t size() const { return m_outputs.size(); }

    void enableAll();
    void disableAll();
    void disable(OutputType o);
    void enable(OutputType o);
    bool isEnabled(OutputType o);
    void disableAllBut(OutputType o);
    void pushGeneratorState();
    void popGeneratorState();

    //////////////////////////////////////////////////
    // OutputDocInterface implementation
    //////////////////////////////////////////////////

    void generateDoc(const QCString &fileName,int startLine,
                     const Definition *ctx,const MemberDef *md,const QCString &docStr,
                     bool indexWords,bool isExample,const QCString &exampleName /*=0*/,
                     bool singleLine /*=FALSE*/,bool linkFromIndex /*=FALSE*/,
                     bool markdownSupport /*=FALSE*/);
    void writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md);
    void parseText(const QCString &textStr);
    void startFile(const QCString &name,const QCString &manName,const QCString &title);

    void startIndexSection(IndexSection is)
    { forall(&OutputGenerator::startIndexSection,is); }
    void endIndexSection(IndexSection is)
    { forall(&OutputGenerator::endIndexSection,is); }
    void writePageLink(const QCString &name,bool first)
    { forall(&OutputGenerator::writePageLink,name,first); }
    void startProjectNumber()
    { forall(&OutputGenerator::startProjectNumber); }
    void endProjectNumber()
    { forall(&OutputGenerator::endProjectNumber); }
    void writeStyleInfo(int part)
    { forall(&OutputGenerator::writeStyleInfo,part); }
    void writeSearchInfo()
    { forall(&OutputGenerator::writeSearchInfo); }
    void writeFooter(const QCString &navPath)
    { forall(&OutputGenerator::writeFooter,navPath); }
    void endFile()
    { forall(&OutputGenerator::endFile); }
    void startTitleHead(const QCString &fileName)
    { forall(&OutputGenerator::startTitleHead,fileName); }
    void endTitleHead(const QCString &fileName,const QCString &name)
    { forall(&OutputGenerator::endTitleHead,fileName,name); }
    void startTitle()
    { forall(&OutputGenerator::startTitle); }
    void endTitle()
    { forall(&OutputGenerator::endTitle); }
    void startParagraph(const QCString &classDef=QCString())
    { forall(&OutputGenerator::startParagraph,classDef); }
    void endParagraph()
    { forall(&OutputGenerator::endParagraph); }
    void writeString(const QCString &text)
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
    void endIndexValue(const QCString &name,bool b)
    { forall(&OutputGenerator::endIndexValue,name,b); }
    void startItemList()
    { forall(&OutputGenerator::startItemList); }
    void endItemList()
    { forall(&OutputGenerator::endItemList); }
    void startIndexItem(const QCString &ref,const QCString &file)
    { forall(&OutputGenerator::startIndexItem,ref,file); }
    void endIndexItem(const QCString &ref,const QCString &file)
    { forall(&OutputGenerator::endIndexItem,ref,file); }
    void docify(const QCString &s)
    { forall(&OutputGenerator::docify,s); }
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor, const QCString &name)
    { forall(&OutputGenerator::writeObjectLink,ref,file,anchor,name); }
    void startTextLink(const QCString &file,const QCString &anchor)
    { forall(&OutputGenerator::startTextLink,file,anchor); }
    void endTextLink()
    { forall(&OutputGenerator::endTextLink); }
    void startHtmlLink(const QCString &url)
    { forall(&OutputGenerator::startHtmlLink,url); }
    void endHtmlLink()
    { forall(&OutputGenerator::endHtmlLink); }
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name)
    { forall(&OutputGenerator::writeStartAnnoItem,type,file,path,name); }
    void writeEndAnnoItem(const QCString &name)
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
    void startMemberHeader(const QCString &anchor, int typ = 2)
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
    void startMemberItem(const QCString &anchor,OutputGenerator::MemberItemType type,const QCString &id=QCString())
    { forall(&OutputGenerator::startMemberItem,anchor,type,id); }
    void endMemberItem(OutputGenerator::MemberItemType type)
    { forall(&OutputGenerator::endMemberItem,type); }
    void startMemberTemplateParams()
    { forall(&OutputGenerator::startMemberTemplateParams); }
    void endMemberTemplateParams(const QCString &anchor,const QCString &inheritId)
    { forall(&OutputGenerator::endMemberTemplateParams,anchor,inheritId); }
    void startCompoundTemplateParams()
    { forall(&OutputGenerator::startCompoundTemplateParams); }
    void endCompoundTemplateParams()
    { forall(&OutputGenerator::endCompoundTemplateParams); }
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
    void insertMemberAlignLeft(OutputGenerator::MemberItemType typ=OutputGenerator::MemberItemType::Normal, bool templ=FALSE)
    { forall(&OutputGenerator::insertMemberAlignLeft,typ,templ); }
    void writeRuler()
    { forall(&OutputGenerator::writeRuler); }
    void writeAnchor(const QCString &fileName,const QCString &name)
    { forall(&OutputGenerator::writeAnchor,fileName,name); }
    void startEmphasis()
    { forall(&OutputGenerator::startEmphasis); }
    void endEmphasis()
    { forall(&OutputGenerator::endEmphasis); }
    void writeChar(char c)
    { forall(&OutputGenerator::writeChar,c); }
    void startMemberDoc(const QCString &clName,const QCString &memName,
                        const QCString &anchor,const QCString &title,
                        int memCount,int memTotal,bool showInline)
    { forall(&OutputGenerator::startMemberDoc,clName,memName,anchor,title,memCount,memTotal,showInline); }
    void endMemberDoc(bool hasArgs)
    { forall(&OutputGenerator::endMemberDoc,hasArgs); }
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor, const QCString &name,
                         const QCString &args)
    { forall(&OutputGenerator::startDoxyAnchor,fName,manName,anchor,name,args); }
    void endDoxyAnchor(const QCString &fn,const QCString &anchor)
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
    void lineBreak(const QCString &style=QCString())
    { forall(&OutputGenerator::lineBreak,style); }
    void startBold()
    { forall(&OutputGenerator::startBold); }
    void endBold()
    { forall(&OutputGenerator::endBold); }
    void startMemberDescription(const QCString &anchor,const QCString &inheritId=QCString(), bool typ = false)
    { forall(&OutputGenerator::startMemberDescription,anchor,inheritId, typ); }
    void endMemberDescription()
    { forall(&OutputGenerator::endMemberDescription); }
    void startMemberDeclaration()
    { forall(&OutputGenerator::startMemberDeclaration); }
    void endMemberDeclaration(const QCString &anchor,const QCString &inheritId)
    { forall(&OutputGenerator::endMemberDeclaration,anchor,inheritId); }
    void writeInheritedSectionTitle(const QCString &id,   const QCString &ref,
                                    const QCString &file, const QCString &anchor,
                                    const QCString &title,const QCString &name)
    { forall(&OutputGenerator::writeInheritedSectionTitle,id,ref,
                                    file,anchor,title,name); }
    void startExamples()
    { forall(&OutputGenerator::startExamples); }
    void endExamples()
    { forall(&OutputGenerator::endExamples); }
    void startParamList(OutputGenerator::ParamListTypes t,const QCString &title)
    { forall(&OutputGenerator::startParamList,t,title); }
    void endParamList()
    { forall(&OutputGenerator::endParamList); }
    void startIndent()
    { forall(&OutputGenerator::startIndent); }
    void endIndent()
    { forall(&OutputGenerator::endIndent); }
    void startSection(const QCString &lab,const QCString &title,SectionType t)
    { forall(&OutputGenerator::startSection,lab,title,t); }
    void endSection(const QCString &lab,SectionType t)
    { forall(&OutputGenerator::endSection,lab,t); }
    void addIndexItem(const QCString &s1,const QCString &s2)
    { forall(&OutputGenerator::addIndexItem,s1,s2); }
    void writeSynopsis()
    { forall(&OutputGenerator::writeSynopsis); }
    void startClassDiagram()
    { forall(&OutputGenerator::startClassDiagram); }
    void endClassDiagram(const ClassDiagram &d,const QCString &f,const QCString &n)
    { forall(&OutputGenerator::endClassDiagram,d,f,n); }
    void startPageRef()
    { forall(&OutputGenerator::startPageRef); }
    void endPageRef(const QCString &c,const QCString &a)
    { forall(&OutputGenerator::endPageRef,c,a); }
    void startQuickIndices()
    { forall(&OutputGenerator::startQuickIndices); }
    void endQuickIndices()
    { forall(&OutputGenerator::endQuickIndices); }
    void writeSplitBar(const QCString &name)
    { forall(&OutputGenerator::writeSplitBar,name); }
    void writeNavigationPath(const QCString &s)
    { forall(&OutputGenerator::writeNavigationPath,s); }
    void writeLogo()
    { forall(&OutputGenerator::writeLogo); }
    void writeQuickLinks(bool compact,HighlightedItem hli,const QCString &file)
    { forall(&OutputGenerator::writeQuickLinks,compact,hli,file); }
    void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first)
    { forall(&OutputGenerator::writeSummaryLink,file,anchor,title,first); }
    void startContents()
    { forall(&OutputGenerator::startContents); }
    void endContents()
    { forall(&OutputGenerator::endContents); }
    void startPageDoc(const QCString &pageTitle)
    { forall(&OutputGenerator::startPageDoc, pageTitle); }
    void endPageDoc()
    { forall(&OutputGenerator::endPageDoc); }
    void writeNonBreakableSpace(int num)
    { forall(&OutputGenerator::writeNonBreakableSpace,num); }
    void startDescTable(const QCString &title)
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
    void startParameterType(bool first,const QCString &key)
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
    void exceptionEntry(const QCString &prefix,bool closeBracket)
    { forall(&OutputGenerator::exceptionEntry,prefix,closeBracket); }

    void startConstraintList(const QCString &header)
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
    void writeLabel(const QCString &l,bool isLast)
    { forall(&OutputGenerator::writeLabel,l,isLast); }
    void endLabels()
    { forall(&OutputGenerator::endLabels); }

    void writeLocalToc(const SectionRefs &refs,const LocalToc &lt)
    { forall(&OutputGenerator::writeLocalToc,refs,lt); }

    void cleanup()
    { forall(&OutputGenerator::cleanup); }

    void startPlainFile(const QCString &name)
    { forall(&OutputGenerator::startPlainFile,name); }
    void endPlainFile()
    { forall(&OutputGenerator::endPlainFile); }

  private:
    void debug();
    void clear();
    void newId();
    void syncEnabled();

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

    std::vector< std::unique_ptr<OutputGenerator> > m_outputs;
    int m_id;
    OutputCodeList m_codeGenList;

};

#endif
