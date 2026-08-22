/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef OUTPUTGEN_H
#define OUTPUTGEN_H

#include <memory>
#include <stack>
#include <iostream>
#include <fstream>

#include "types.h"
#include "index.h"
#include "section.h"
#include "textstream.h"
#include "docparser.h"
#include "construct.h"

class ClassDiagram;
class DotClassGraph;
class DotInclDepGraph;
class DotCallGraph;
class DotDirDeps;
class DotGfxHierarchyTable;
class DotGroupCollaboration;
class MemberDef;
class Definition;
class OutputCodeList;

struct DocLinkInfo
{
  DString name;
  DString ref;
  DString url;
  DString anchor;
};

struct SourceLinkInfo
{
  DString file;
  int line;
  DString ref;
  DString url;
  DString anchor;
};

enum class OutputType { List, Html, Latex, Man, RTF, Docbook, XML, Null, Extension, Recorder };


/** Base class for code generators
 */
class OutputCodeIntf
{
  public:
    OutputCodeIntf() = default;
    DEFAULT_COPYABLE(OutputCodeIntf)

    virtual OutputType type() const = 0;
    virtual std::unique_ptr<OutputCodeIntf> clone() = 0;
    virtual void codify(const DString &s) = 0;
    virtual void stripCodeComments(bool b) = 0;
    virtual void startSpecialComment() = 0;
    virtual void endSpecialComment() = 0;
    virtual void setStripIndentAmount(size_t) = 0;
    virtual void writeCodeLink(CodeSymbolType type,
                       const DString &ref,const DString &file,
                       const DString &anchor,const DString &name,
                       const DString &tooltip) = 0;
    virtual void writeLineNumber(const DString &ref,const DString &file,const DString &anchor,
                         int lineNumber, bool writeLineAnchor) = 0;
    virtual void writeTooltip(const DString &id, const DocLinkInfo &docInfo, const DString &decl,
                      const DString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) = 0;
    virtual void startCodeLine(int lineNr) = 0;
    virtual void endCodeLine()= 0;
    virtual void startFontClass(const DString &c) = 0;
    virtual void endFontClass() = 0;
    virtual void writeCodeAnchor(const DString &name) = 0;
    virtual void startCodeFragment(const DString &style) = 0;
    virtual void endCodeFragment(const DString &style) = 0;
    virtual void startFold(int lineNr,const DString &startMarker,const DString &endMarker) = 0;
    virtual void endFold() = 0;
};

/** Base class for shared implementation for all output generators.
 */
class OutputGenerator
{
  public:
    OutputGenerator() : m_t(40*1024) {}

    enum class ParamListTypes { Param, RetVal, Exception };
    enum class MemberItemType { Normal, AnonymousStart, AnonymousEnd, Templated };

    OutputGenerator(const DString &dir);
    virtual ~OutputGenerator() = default;
    ONLY_DEFAULT_MOVABLE(OutputGenerator)

    DString dir() const;
    DString fileName() const;

    void startPlainFile(const DString &name);
    void endPlainFile();
  protected:
    TextStream m_t;
    DString m_dir;
  private:
    DString m_fileName;
    FILE *m_file = nullptr;
};


/** Abstract interface for output generators
 */
class OutputGenIntf
{
  public:
    ABSTRACT_BASE_CLASS(OutputGenIntf)

    virtual OutputType type() const = 0;
    virtual std::unique_ptr<OutputGenIntf> clone() = 0;
    virtual void addCodeGen(OutputCodeList &list) = 0;
    virtual void writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md, int id,int sectionLevel) = 0;
    virtual void startIndexSection(IndexSection is) = 0;
    virtual void endIndexSection(IndexSection is) = 0;
    virtual void writePageLink(const DString &name,bool first) = 0;
    virtual void startProjectNumber() = 0;
    virtual void endProjectNumber() = 0;
    virtual void writeStyleInfo(int part) = 0;
    virtual void writeSearchInfo() = 0;
    virtual void writeFooter(const DString &navPath) = 0;
    virtual void startFile(const DString &name,bool isSource,const DString &manName,const DString &title,int id, int hierarchyLevel) = 0;
    virtual void endFile() = 0;
    virtual void startTitleHead(const DString &fileName) = 0;
    virtual void endTitleHead(const DString &fileName,const DString &name) = 0;
    virtual void startParagraph(const DString &classDef=DString()) = 0;
    virtual void endParagraph() = 0;
    virtual void writeString(const DString &text) = 0;
    virtual void startIndexListItem() = 0;
    virtual void endIndexListItem() = 0;
    virtual void startIndexList() = 0;
    virtual void endIndexList() = 0;
    virtual void startIndexKey() = 0;
    virtual void endIndexKey() = 0;
    virtual void startIndexValue(bool b) = 0;
    virtual void endIndexValue(const DString &name,bool b) = 0;
    virtual void startItemList() = 0;
    virtual void endItemList() = 0;
    virtual void startIndexItem(const DString &ref,const DString &file) = 0;
    virtual void endIndexItem(const DString &ref,const DString &file) = 0;
    virtual void docify(const DString &s) = 0;
    virtual void writeObjectLink(const DString &ref,const DString &file,
                         const DString &anchor, const DString &name) = 0;
    virtual void startTextLink(const DString &file,const DString &anchor) = 0;
    virtual void endTextLink() = 0;
    virtual void writeStartAnnoItem(const DString &type,const DString &file,
                            const DString &path,const DString &name) = 0;
    virtual void startTypewriter() = 0;
    virtual void endTypewriter() = 0;
    virtual void startGroupHeader(const DString &id,int extraLevels=0) = 0;
    virtual void endGroupHeader(int extraLevels=0) = 0;
    virtual void startItemListItem() = 0;
    virtual void endItemListItem() = 0;
    virtual void startMemberSections() = 0;
    virtual void endMemberSections() = 0;
    virtual void startHeaderSection() = 0;
    virtual void endHeaderSection() = 0;
    virtual void startMemberHeader(const DString &anchor, int typ = 2) = 0;
    virtual void endMemberHeader() = 0;
    virtual void startMemberSubtitle() = 0;
    virtual void endMemberSubtitle() = 0;
    virtual void startMemberDocList() = 0;
    virtual void endMemberDocList() = 0;
    virtual void startMemberList() = 0;
    virtual void endMemberList() = 0;
    virtual void startInlineHeader() = 0;
    virtual void endInlineHeader() = 0;
    virtual void startAnonTypeScope(int i1) = 0;
    virtual void endAnonTypeScope(int i1) = 0;
    virtual void startMemberItem(const DString &anchor,OutputGenerator::MemberItemType type,const DString &id=DString()) = 0;
    virtual void endMemberItem(OutputGenerator::MemberItemType type) = 0;
    virtual void startMemberTemplateParams() = 0;
    virtual void endMemberTemplateParams(const DString &anchor,const DString &inheritId) = 0;
    virtual void startCompoundTemplateParams() = 0;
    virtual void endCompoundTemplateParams() = 0;
    virtual void startMemberGroupHeader(const DString &id,bool b) = 0;
    virtual void endMemberGroupHeader(bool b) = 0;
    virtual void startMemberGroupDocs() = 0;
    virtual void endMemberGroupDocs() = 0;
    virtual void startMemberGroup() = 0;
    virtual void endMemberGroup(bool last) = 0;
    virtual void insertMemberAlign(bool templ=false) = 0;
    virtual void insertMemberAlignLeft(OutputGenerator::MemberItemType typ=OutputGenerator::MemberItemType::Normal, bool templ=false) = 0;
    virtual void writeRuler() = 0;
    virtual void writeAnchor(const DString &fileName,const DString &name) = 0;
    virtual void startEmphasis() = 0;
    virtual void endEmphasis() = 0;
    virtual void writeChar(char c) = 0;
    virtual void startMemberDoc(const DString &clName,const DString &memName,
                        const DString &anchor,const DString &title,
                        int memCount,int memTotal,bool showInline) = 0;
    virtual void endMemberDoc(bool hasArgs) = 0;
    virtual void startDoxyAnchor(const DString &fName,const DString &manName,
                         const DString &anchor, const DString &name,
                         const DString &args) = 0;
    virtual void endDoxyAnchor(const DString &fn,const DString &anchor) = 0;
    virtual void addLabel(const DString &fName,const DString &anchor) = 0;
    virtual void writeLatexSpacing() = 0;
    virtual void startDescForItem() = 0;
    virtual void endDescForItem() = 0;
    virtual void startCenter() = 0;
    virtual void endCenter() = 0;
    virtual void startSmall() = 0;
    virtual void endSmall() = 0;
    virtual void lineBreak(const DString &style=DString()) = 0;
    virtual void startBold() = 0;
    virtual void endBold() = 0;
    virtual void startMemberDescription(const DString &anchor,const DString &inheritId=DString(), bool typ = false) = 0;
    virtual void endMemberDescription() = 0;
    virtual void startMemberDeclaration() = 0;
    virtual void endMemberDeclaration(const DString &anchor,const DString &inheritId) = 0;
    virtual void writeInheritedSectionTitle(const DString &id,   const DString &ref,
                                    const DString &file, const DString &anchor,
                                    const DString &title,const DString &name) = 0;
    virtual void startExamples() = 0;
    virtual void endExamples() = 0;
    virtual void startIndent() = 0;
    virtual void endIndent() = 0;
    virtual void startSection(const DString &lab,const DString &title,SectionType t) = 0;
    virtual void endSection(const DString &lab,SectionType t) = 0;
    virtual void addIndexItem(const DString &s1,const DString &s2) = 0;
    virtual void writeSynopsis() = 0;
    virtual void startClassDiagram() = 0;
    virtual void endClassDiagram(const ClassDiagram &d,const DString &f,const DString &n) = 0;
    virtual void startPageRef() = 0;
    virtual void endPageRef(const DString &c,const DString &a) = 0;
    virtual void startQuickIndices() = 0;
    virtual void endQuickIndices() = 0;
    virtual void writeSplitBar(const DString &name,const DString &allMembersFile) = 0;
    virtual void writeNavigationPath(const DString &s) = 0;
    virtual void writeLogo() = 0;
    virtual void writeQuickLinks(HighlightedItem hli,const DString &file,bool extraTabs) = 0;
    virtual void writeSummaryLink(const DString &file,const DString &anchor,const DString &title,bool first) = 0;
    virtual void writePageOutline() = 0;
    virtual void startContents() = 0;
    virtual void endContents() = 0;
    virtual void startPageDoc(const DString &pageTitle) = 0;
    virtual void endPageDoc() = 0;
    virtual void writeNonBreakableSpace(int num) = 0;
    virtual void startDescTable(const DString &title,const bool hasInits) = 0;
    virtual void endDescTable() = 0;
    virtual void startDescTableRow() = 0;
    virtual void endDescTableRow() = 0;
    virtual void startDescTableTitle() = 0;
    virtual void endDescTableTitle() = 0;
    virtual void startDescTableInit() = 0;
    virtual void endDescTableInit() = 0;
    virtual void startDescTableData() = 0;
    virtual void endDescTableData() = 0;
    virtual void startDotGraph() = 0;
    virtual void endDotGraph(DotClassGraph &g) = 0;
    virtual void startInclDepGraph() = 0;
    virtual void endInclDepGraph(DotInclDepGraph &g) = 0;
    virtual void startCallGraph() = 0;
    virtual void endCallGraph(DotCallGraph &g) = 0;
    virtual void startDirDepGraph() = 0;
    virtual void endDirDepGraph(DotDirDeps &g) = 0;
    virtual void startGroupCollaboration() = 0;
    virtual void endGroupCollaboration(DotGroupCollaboration &g) = 0;
    virtual void writeGraphicalHierarchy(DotGfxHierarchyTable &g) = 0;
    virtual void startTextBlock(bool dense=false) = 0;
    virtual void endTextBlock(bool paraBreak=false) = 0;
    virtual void lastIndexPage() = 0;
    virtual void startMemberDocPrefixItem() = 0;
    virtual void endMemberDocPrefixItem() = 0;
    virtual void startMemberDocName(bool align) = 0;
    virtual void endMemberDocName() = 0;
    virtual void startParameterType(bool first,const DString &key) = 0;
    virtual void endParameterType() = 0;
    virtual void startParameterName(bool one) = 0;
    virtual void endParameterName() = 0;
    virtual void startParameterExtra() = 0;
    virtual void endParameterExtra(bool last,bool one,bool bracket) = 0;
    virtual void startParameterDefVal(const char *separator) = 0;
    virtual void endParameterDefVal() = 0;
    virtual void startParameterList(bool openBracket) = 0;
    virtual void endParameterList() = 0;
    virtual void exceptionEntry(const DString &prefix,bool closeBracket) = 0;
    virtual void startConstraintList(const DString &header) = 0;
    virtual void startConstraintParam() = 0;
    virtual void endConstraintParam() = 0;
    virtual void startConstraintType() = 0;
    virtual void endConstraintType() = 0;
    virtual void startConstraintDocs() = 0;
    virtual void endConstraintDocs() = 0;
    virtual void endConstraintList() = 0;
    virtual void startMemberDocSimple(bool b) = 0;
    virtual void endMemberDocSimple(bool b) = 0;
    virtual void startInlineMemberType() = 0;
    virtual void endInlineMemberType() = 0;
    virtual void startInlineMemberName() = 0;
    virtual void endInlineMemberName() = 0;
    virtual void startInlineMemberDoc() = 0;
    virtual void endInlineMemberDoc() = 0;
    virtual void startLabels() = 0;
    virtual void writeLabel(const DString &l,bool isLast) = 0;
    virtual void endLabels() = 0;
    virtual void startLocalToc(int level) = 0;
    virtual void endLocalToc() = 0;
    virtual void startTocEntry(const SectionInfo *si) = 0;
    virtual void endTocEntry(const SectionInfo *si) = 0;
    virtual void cleanup() = 0;
    virtual void startPlainFile(const DString &name) = 0;
    virtual void endPlainFile() = 0;
    virtual void startEmbeddedDoc(size_t) = 0;
    virtual void endEmbeddedDoc() = 0;
};

size_t updateColumnCount(const char *s,size_t col);

#endif
