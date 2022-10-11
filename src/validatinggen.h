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

#ifndef VALIDATINGGEN_H
#define VALIDATINGGEN_H

#include "outputgen.h"

class ValidatingCodeGenerator : public CodeOutputInterface
{
  public:
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    void codify(const QCString &s) {}
    void writeCodeLink(CodeSymbolType type,
                               const QCString &ref,const QCString &file,
                               const QCString &anchor,const QCString &name,
                               const QCString &tooltip) {}
    void writeLineNumber(const QCString &ref,const QCString &file,
                                 const QCString &anchor,int lineNumber, bool writeLineAnchor) {}
    void writeTooltip(const QCString &id,
                              const DocLinkInfo &docInfo,
                              const QCString &decl,
                              const QCString &desc,
                              const SourceLinkInfo &defInfo,
                              const SourceLinkInfo &declInfo
                             ) {}
    void startCodeLine(bool hasLineNumbers) {}
    void endCodeLine() {}
    void startFontClass(const QCString &clsName) {}
    void endFontClass() {}
    void writeCodeAnchor(const QCString &name) {}
    void setCurrentDoc(const Definition *context,const QCString &anchor,bool isSourceFile) {}
    void addWord(const QCString &word,bool hiPriority) {}
    void startCodeFragment(const QCString &style) {}
    void endCodeFragment(const QCString &style) {}

  private:
    int m_id = 0;
};

/** Generator so the tests will be executed */
class ValidatingGenerator : public OutputGenerator 
{
  public:
    ValidatingGenerator() : OutputGenerator(QCString()) {}
    ValidatingGenerator(const ValidatingGenerator &o) : OutputGenerator(o) {}
    ValidatingGenerator &operator=(const ValidatingGenerator &o){  OutputGenerator::operator=(o); return *this;}
     ~ValidatingGenerator(){}

     OutputType type() const { return Validating;}
    static void init() {}
     std::unique_ptr<OutputGenerator> clone() const {return std::make_unique<ValidatingGenerator>(*this);}

    void enable();
    void disable();
    void enableIf(OutputType o);
    void disableIf(OutputType o);
    void disableIfNot(OutputType o);
    bool isEnabled(OutputType o);
    OutputGenerator *get(OutputType o);
    QCString dir() const;
    QCString fileName() const;

    void startPlainFile(const QCString &name);
    void endPlainFile();
    bool isEnabled() const { return m_active; }
    void pushGeneratorState();
    void popGeneratorState();

     void writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md,int id) {}

     void startFile(const QCString &name,const QCString &manName,
                           const QCString &title,int id=0) {}
     void writeSearchInfo() {}
     void writeFooter(const QCString &navPath) {}
     void endFile() {}
     void startIndexSection(IndexSections) {}
     void endIndexSection(IndexSections) {}
     void writePageLink(const QCString &,bool) {}
     void startProjectNumber() {}
     void endProjectNumber() {}
     void writeStyleInfo(int part) {}
     void startTitleHead(const QCString &) {}
     void endTitleHead(const QCString &fileName,const QCString &name) {}
     void startIndexListItem() {}
     void endIndexListItem()   {}
     void startIndexList() {}
     void endIndexList()   {}
     void startIndexKey() {}
     void endIndexKey()   {}
     void startIndexValue(bool) {}
     void endIndexValue(const QCString &,bool) {}
     void startIndexItem(const QCString &ref,const QCString &file) {}
     void endIndexItem(const QCString &ref,const QCString &file) {}
     void startGroupHeader(int) {}
     void endGroupHeader(int) {}
     void startMemberSections() {}
     void endMemberSections() {}
     void startHeaderSection() {}
     void endHeaderSection() {}
     void startMemberHeader(const QCString &anchor, int typ) {}
     void endMemberHeader() {}
     void startMemberSubtitle() {}
     void endMemberSubtitle() {}
     void startMemberDocList() {}
     void endMemberDocList() {}
     void startMemberList() {}
     void endMemberList() {}
     void startInlineHeader() {}
     void endInlineHeader() {}
     void startAnonTypeScope(int) {}
     void endAnonTypeScope(int) {}
     void startMemberItem(const QCString &,int,const QCString &) {}
     void endMemberItem() {}
     void startMemberTemplateParams() {}
     void endMemberTemplateParams(const QCString &,const QCString &) {}
     void startCompoundTemplateParams() {}
     void endCompoundTemplateParams() {}
     void startMemberGroupHeader(bool) {}
     void endMemberGroupHeader() {}
     void startMemberGroupDocs() {}
     void endMemberGroupDocs() {}
     void startMemberGroup() {}
     void endMemberGroup(bool) {}
     void insertMemberAlign(bool) {}
     void insertMemberAlignLeft(int,bool) {}
     void startMemberDoc(const QCString &,const QCString &,
                                const QCString &,const QCString &,int,int,bool) {}
     void endMemberDoc(bool) {}
     void startDoxyAnchor(const QCString &fName,const QCString &manName,
                                 const QCString &anchor,const QCString &name,
                                 const QCString &args) {}
     void endDoxyAnchor(const QCString &fileName,const QCString &anchor) {}
     void writeLatexSpacing() {}
     void writeStartAnnoItem(const QCString &type,const QCString &file,
                                    const QCString &path,const QCString &name) {}
     void writeEndAnnoItem(const QCString &name) {}
     void startMemberDescription(const QCString &anchor,const QCString &inheritId, bool typ) {}
     void endMemberDescription() {}
     void startMemberDeclaration() {}
     void endMemberDeclaration(const QCString &anchor,const QCString &inheritId) {}
     void writeInheritedSectionTitle(const QCString &id,const QCString &ref,
                                            const QCString &file,const QCString &anchor,
                                            const QCString &title,const QCString &name) {}
     void startIndent() {}
     void endIndent() {}
     void writeSynopsis() {}
     void startClassDiagram() {}
     void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &) {}
     void startDotGraph() {}
     void endDotGraph(DotClassGraph &g) {}
     void startInclDepGraph() {}
     void endInclDepGraph(DotInclDepGraph &g) {}
     void startGroupCollaboration() {}
     void endGroupCollaboration(DotGroupCollaboration &g) {}
     void startCallGraph() {}
     void endCallGraph(DotCallGraph &g) {}
     void startDirDepGraph() {}
     void endDirDepGraph(DotDirDeps &g) {}
     void writeGraphicalHierarchy(DotGfxHierarchyTable &g) {}
     void startQuickIndices() {}
     void endQuickIndices() {}
     void writeSplitBar(const QCString &) {}
     void writeNavigationPath(const QCString &) {}
     void writeLogo() {}
     void writeQuickLinks(bool compact,HighlightedItem hli,const QCString &file) {}
     void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first) {}
     void startContents() {}
     void endContents() {}
     void startPageDoc(const QCString &) {}
     void endPageDoc() {}
     void startTextBlock(bool) {}
     void endTextBlock(bool) {}
     void lastIndexPage() {}
     void startMemberDocPrefixItem() {}
     void endMemberDocPrefixItem() {}
     void startMemberDocName(bool) {}
     void endMemberDocName() {}
     void startParameterType(bool,const QCString &key) {}
     void endParameterType() {}
     void startParameterName(bool) {}
     void endParameterName(bool,bool,bool) {}
     void startParameterList(bool) {}
     void endParameterList() {}
     void exceptionEntry(const QCString &,bool) {}

     void startConstraintList(const QCString &) {}
     void startConstraintParam() {}
     void endConstraintParam() {}
     void startConstraintType() {}
     void endConstraintType() {}
     void startConstraintDocs() {}
     void endConstraintDocs() {}
     void endConstraintList() {}

     void startMemberDocSimple(bool) {}
     void endMemberDocSimple(bool) {}
     void startInlineMemberType() {}
     void endInlineMemberType() {}
     void startInlineMemberName() {}
     void endInlineMemberName() {}
     void startInlineMemberDoc() {}
     void endInlineMemberDoc() {}

     void startLabels() {}
     void writeLabel(const QCString &,bool) {}
     void endLabels() {}

     void cleanup() {}

     void codify(const QCString &text){}
     void writeCodeLink(CodeSymbolType type,
                        const QCString &ref,const QCString &file,
                        const QCString &anchor,const QCString &name,
                        const QCString &tooltip){}
     void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,int lineNumber, bool writeLineAnchor){}
     void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                       const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo
                      ){}
     void startCodeLine(bool hasLineNumbers){}
     void endCodeLine(){}
     void startFontClass(const QCString &s){}
     void endFontClass(){}
     void writeCodeAnchor(const QCString &anchor){}
     void startCodeFragment(const QCString &style){}
     void endCodeFragment(const QCString &style){}


     void startItemList()  {}
     void startItemListItem()  {}
     void endItemListItem()  {}
     void endItemList()    {}
     void docify(const QCString &s) {}
     void writeChar(char c) {}
     void writeString(const QCString &text) {}
     void startParagraph(const QCString &classDef) {}
     void endParagraph() {}
     void writeObjectLink(const QCString &ref,const QCString &file,
                                 const QCString &anchor, const QCString &name) {}
     void startHtmlLink(const QCString &url) {}
     void endHtmlLink() {}
     void startBold()      {}
     void endBold()        {}
     void startTypewriter() {}
     void endTypewriter() {}
     void startEmphasis() {}
     void endEmphasis() {}
     void writeRuler() {}
     void startDescription() {}
     void endDescription() {}
     void startDescItem() {}
     void startDescForItem() {}
     void endDescForItem() {}
     void endDescItem() {}
     void startCenter() {}
     void endCenter() {}
     void startSmall() {}
     void endSmall() {}

     void startExamples() {}
     void endExamples() {}
     void startParamList(ParamListTypes t,const QCString &title) {}
     void endParamList() {}

     void startTitle() {}
     void endTitle()   {}

     void writeAnchor(const QCString &fileName,const QCString &name) {}
     void startSection(const QCString &,const QCString &,SectionType) {}
     void endSection(const QCString &,SectionType) {}

     void lineBreak(const QCString &style) {}
     void addIndexItem(const QCString &s1,const QCString &s2) {}

     void writeNonBreakableSpace(int) {}
     void startDescTable(const QCString &title) {}
     void endDescTable() {}
     void startDescTableRow() {}
     void endDescTableRow() {}
     void startDescTableTitle() {}
     void endDescTableTitle() {}
     void startDescTableData() {}
     void endDescTableData() {}
     void startTextLink(const QCString &file,const QCString &anchor) {}
     void endTextLink() {}
     void startPageRef() {}
     void endPageRef(const QCString &,const QCString &) {}
     void startSubsection() {}
     void endSubsection() {}
     void startSubsubsection() {}
     void endSubsubsection() {}
  private:
    bool m_active = true;
};

#endif
