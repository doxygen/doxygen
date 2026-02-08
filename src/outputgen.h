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
  QCString name;
  QCString ref;
  QCString url;
  QCString anchor;
};

struct SourceLinkInfo
{
  QCString file;
  int line;
  QCString ref;
  QCString url;
  QCString anchor;
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
    virtual void codify(const QCString &s) = 0;
    virtual void stripCodeComments(bool b) = 0;
    virtual void startSpecialComment() = 0;
    virtual void endSpecialComment() = 0;
    virtual void setStripIndentAmount(size_t) = 0;
    virtual void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) = 0;
    virtual void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor) = 0;
    virtual void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) = 0;
    virtual void startCodeLine(int lineNr) = 0;
    virtual void endCodeLine()= 0;
    virtual void startFontClass(const QCString &c) = 0;
    virtual void endFontClass() = 0;
    virtual void writeCodeAnchor(const QCString &name) = 0;
    virtual void startCodeFragment(const QCString &style) = 0;
    virtual void endCodeFragment(const QCString &style) = 0;
    virtual void startFold(int lineNr,const QCString &startMarker,const QCString &endMarker) = 0;
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

    OutputGenerator(const QCString &dir);
    virtual ~OutputGenerator() = default;
    ONLY_DEFAULT_MOVABLE(OutputGenerator)

    QCString dir() const;
    QCString fileName() const;

    void startPlainFile(const QCString &name);
    void endPlainFile();
  protected:
    TextStream m_t;
    QCString m_dir;
  private:
    QCString m_fileName;
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
    virtual void writePageLink(const QCString &name,bool first) = 0;
    virtual void startProjectNumber() = 0;
    virtual void endProjectNumber() = 0;
    virtual void writeStyleInfo(int part) = 0;
    virtual void writeSearchInfo() = 0;
    virtual void writeFooter(const QCString &navPath) = 0;
    virtual void startFile(const QCString &name,bool isSource,const QCString &manName,const QCString &title,int id, int hierarchyLevel) = 0;
    virtual void endFile() = 0;
    virtual void startTitleHead(const QCString &fileName) = 0;
    virtual void endTitleHead(const QCString &fileName,const QCString &name) = 0;
    virtual void startParagraph(const QCString &classDef=QCString()) = 0;
    virtual void endParagraph() = 0;
    virtual void writeString(const QCString &text) = 0;
    virtual void startIndexListItem() = 0;
    virtual void endIndexListItem() = 0;
    virtual void startIndexList() = 0;
    virtual void endIndexList() = 0;
    virtual void startIndexKey() = 0;
    virtual void endIndexKey() = 0;
    virtual void startIndexValue(bool b) = 0;
    virtual void endIndexValue(const QCString &name,bool b) = 0;
    virtual void startItemList() = 0;
    virtual void endItemList() = 0;
    virtual void startIndexItem(const QCString &ref,const QCString &file) = 0;
    virtual void endIndexItem(const QCString &ref,const QCString &file) = 0;
    virtual void docify(const QCString &s) = 0;
    virtual void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor, const QCString &name) = 0;
    virtual void startTextLink(const QCString &file,const QCString &anchor) = 0;
    virtual void endTextLink() = 0;
    virtual void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name) = 0;
    virtual void startTypewriter() = 0;
    virtual void endTypewriter() = 0;
    virtual void startGroupHeader(const QCString &id,int extraLevels=0) = 0;
    virtual void endGroupHeader(int extraLevels=0) = 0;
    virtual void startItemListItem() = 0;
    virtual void endItemListItem() = 0;
    virtual void startMemberSections() = 0;
    virtual void endMemberSections() = 0;
    virtual void startHeaderSection() = 0;
    virtual void endHeaderSection() = 0;
    virtual void startMemberHeader(const QCString &anchor, int typ = 2) = 0;
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
    virtual void startMemberItem(const QCString &anchor,OutputGenerator::MemberItemType type,const QCString &id=QCString()) = 0;
    virtual void endMemberItem(OutputGenerator::MemberItemType type) = 0;
    virtual void startMemberTemplateParams() = 0;
    virtual void endMemberTemplateParams(const QCString &anchor,const QCString &inheritId) = 0;
    virtual void startCompoundTemplateParams() = 0;
    virtual void endCompoundTemplateParams() = 0;
    virtual void startMemberGroupHeader(const QCString &id,bool b) = 0;
    virtual void endMemberGroupHeader() = 0;
    virtual void startMemberGroupDocs() = 0;
    virtual void endMemberGroupDocs() = 0;
    virtual void startMemberGroup() = 0;
    virtual void endMemberGroup(bool last) = 0;
    virtual void insertMemberAlign(bool templ=FALSE) = 0;
    virtual void insertMemberAlignLeft(OutputGenerator::MemberItemType typ=OutputGenerator::MemberItemType::Normal, bool templ=FALSE) = 0;
    virtual void writeRuler() = 0;
    virtual void writeAnchor(const QCString &fileName,const QCString &name) = 0;
    virtual void startEmphasis() = 0;
    virtual void endEmphasis() = 0;
    virtual void writeChar(char c) = 0;
    virtual void startMemberDoc(const QCString &clName,const QCString &memName,
                        const QCString &anchor,const QCString &title,
                        int memCount,int memTotal,bool showInline) = 0;
    virtual void endMemberDoc(bool hasArgs) = 0;
    virtual void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor, const QCString &name,
                         const QCString &args) = 0;
    virtual void endDoxyAnchor(const QCString &fn,const QCString &anchor) = 0;
    virtual void addLabel(const QCString &fName,const QCString &anchor) = 0;
    virtual void writeLatexSpacing() = 0;
    virtual void startDescForItem() = 0;
    virtual void endDescForItem() = 0;
    virtual void startCenter() = 0;
    virtual void endCenter() = 0;
    virtual void startSmall() = 0;
    virtual void endSmall() = 0;
    virtual void lineBreak(const QCString &style=QCString()) = 0;
    virtual void startBold() = 0;
    virtual void endBold() = 0;
    virtual void startMemberDescription(const QCString &anchor,const QCString &inheritId=QCString(), bool typ = false) = 0;
    virtual void endMemberDescription() = 0;
    virtual void startMemberDeclaration() = 0;
    virtual void endMemberDeclaration(const QCString &anchor,const QCString &inheritId) = 0;
    virtual void writeInheritedSectionTitle(const QCString &id,   const QCString &ref,
                                    const QCString &file, const QCString &anchor,
                                    const QCString &title,const QCString &name) = 0;
    virtual void startExamples() = 0;
    virtual void endExamples() = 0;
    virtual void startIndent() = 0;
    virtual void endIndent() = 0;
    virtual void startSection(const QCString &lab,const QCString &title,SectionType t) = 0;
    virtual void endSection(const QCString &lab,SectionType t) = 0;
    virtual void addIndexItem(const QCString &s1,const QCString &s2) = 0;
    virtual void writeSynopsis() = 0;
    virtual void startClassDiagram() = 0;
    virtual void endClassDiagram(const ClassDiagram &d,const QCString &f,const QCString &n) = 0;
    virtual void startPageRef() = 0;
    virtual void endPageRef(const QCString &c,const QCString &a) = 0;
    virtual void startQuickIndices() = 0;
    virtual void endQuickIndices() = 0;
    virtual void writeSplitBar(const QCString &name,const QCString &allMembersFile) = 0;
    virtual void writeNavigationPath(const QCString &s) = 0;
    virtual void writeLogo() = 0;
    virtual void writeQuickLinks(HighlightedItem hli,const QCString &file,bool extraTabs) = 0;
    virtual void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first) = 0;
    virtual void writePageOutline() = 0;
    virtual void startContents() = 0;
    virtual void endContents() = 0;
    virtual void startPageDoc(const QCString &pageTitle) = 0;
    virtual void endPageDoc() = 0;
    virtual void writeNonBreakableSpace(int num) = 0;
    virtual void startDescTable(const QCString &title,const bool hasInits) = 0;
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
    virtual void startTextBlock(bool dense=FALSE) = 0;
    virtual void endTextBlock(bool paraBreak=FALSE) = 0;
    virtual void lastIndexPage() = 0;
    virtual void startMemberDocPrefixItem() = 0;
    virtual void endMemberDocPrefixItem() = 0;
    virtual void startMemberDocName(bool align) = 0;
    virtual void endMemberDocName() = 0;
    virtual void startParameterType(bool first,const QCString &key) = 0;
    virtual void endParameterType() = 0;
    virtual void startParameterName(bool one) = 0;
    virtual void endParameterName() = 0;
    virtual void startParameterExtra() = 0;
    virtual void endParameterExtra(bool last,bool one,bool bracket) = 0;
    virtual void startParameterDefVal(const char *separator) = 0;
    virtual void endParameterDefVal() = 0;
    virtual void startParameterList(bool openBracket) = 0;
    virtual void endParameterList() = 0;
    virtual void exceptionEntry(const QCString &prefix,bool closeBracket) = 0;
    virtual void startConstraintList(const QCString &header) = 0;
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
    virtual void writeLabel(const QCString &l,bool isLast) = 0;
    virtual void endLabels() = 0;
    virtual void startLocalToc(int level) = 0;
    virtual void endLocalToc() = 0;
    virtual void startTocEntry(const SectionInfo *si) = 0;
    virtual void endTocEntry(const SectionInfo *si) = 0;
    virtual void cleanup() = 0;
    virtual void startPlainFile(const QCString &name) = 0;
    virtual void endPlainFile() = 0;
    virtual void startEmbeddedDoc(int) = 0;
    virtual void endEmbeddedDoc() = 0;
};


#endif
