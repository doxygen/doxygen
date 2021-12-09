/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

class ClassDiagram;
class DotClassGraph;
class DotInclDepGraph;
class DotCallGraph;
class DotDirDeps;
class DotGfxHierarchyTable;
class DotGroupCollaboration;
class DocNode;
class MemberDef;
class Definition;

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

/** Output interface for code parser.
 */
class CodeOutputInterface
{
  public:
    virtual ~CodeOutputInterface() {}
    CodeOutputInterface() {}
    CodeOutputInterface(const CodeOutputInterface &) = delete;
    CodeOutputInterface &operator=(const CodeOutputInterface &) = delete;

    /** Identifier for the output file */
    virtual int id() const { return 0; }

    /*! Writes an code fragment to the output. This function should keep
     *  spaces visible, should break lines at a newline and should convert
     *  tabs to the right number of spaces.
     */
    virtual void codify(const QCString &s) = 0;

    /*! Writes a link to an object in a code fragment.
     *  \param type     The type of symbol, used for semantic syntax
     *                  highlighting, may be Default is no info is available.
     *  \param ref      If this is non-zero, the object is to be found in
     *                  an external documentation file.
     *  \param file     The file in which the object is located.
     *  \param anchor   The anchor uniquely identifying the object within
     *                  the file.
     *  \param name     The text to display as a placeholder for the link.
     *  \param tooltip  The tooltip to display when the mouse is on the link.
     */
    virtual void writeCodeLink(CodeSymbolType type,
                               const QCString &ref,const QCString &file,
                               const QCString &anchor,const QCString &name,
                               const QCString &tooltip) = 0;

    /*! Writes the line number of a source listing
     *  \param ref        External reference (when imported from a tag file)
     *  \param file       The file part of the URL pointing to the docs.
     *  \param anchor     The anchor part of the URL pointing to the docs.
     *  \param lineNumber The line number to write
     *  \param writeLineAnchor Indicates if an anchor for the line number needs to be written
     */
    virtual void writeLineNumber(const QCString &ref,const QCString &file,
                                 const QCString &anchor,int lineNumber, bool writeLineAnchor) = 0;

    /*! Writes a tool tip definition
     *  \param id       unique identifier for the tooltip
     *  \param docInfo  Info about the symbol's documentation.
     *  \param decl     full declaration of the symbol (for functions)
     *  \param desc     brief description for the symbol
     *  \param defInfo  Info about the symbol's definition in the source code
     *  \param declInfo Info about the symbol's declaration in the source code
     */
    virtual void writeTooltip(const QCString &id,
                              const DocLinkInfo &docInfo,
                              const QCString &decl,
                              const QCString &desc,
                              const SourceLinkInfo &defInfo,
                              const SourceLinkInfo &declInfo
                             ) = 0;

    virtual void startCodeLine(bool hasLineNumbers) = 0;

    /*! Ends a line of code started with startCodeLine() */
    virtual void endCodeLine() = 0;

    /*! Starts a block with a certain meaning. Used for syntax highlighting,
     *  which elements of the same type are rendered using the same 'font class'.
     *  \param clsName The category name.
     */
    virtual void startFontClass(const QCString &clsName) = 0;

    /*! Ends a block started with startFontClass() */
    virtual void endFontClass() = 0;

    /*! Write an anchor to a source listing.
     *  \param name The name of the anchor.
     */
    virtual void writeCodeAnchor(const QCString &name) = 0;

    virtual void setCurrentDoc(const Definition *context,const QCString &anchor,bool isSourceFile) = 0;
    virtual void addWord(const QCString &word,bool hiPriority) = 0;

    /*! Starts a source code fragment. The fragment will be
     *  fed to the code parser (see code.h) for syntax highlighting
     *  and cross-referencing. The fragment ends by a call to
     *  endCodeFragment()
     *  @param style The kind of code fragment.
     */
    virtual void startCodeFragment(const QCString &style) = 0;
    /*! Ends a block of code */
    virtual void endCodeFragment(const QCString &style) = 0;
};

/** Base Interface used for generating output outside of the
 *  comment blocks.
 *
 *  This abstract class is used by output generation functions
 *  to generate the output for a specific format,
 *  or a list of formats (see OutputList). This interface
 *  contains functions that generate fragments of the output.
 */
class BaseOutputDocInterface : public CodeOutputInterface
{
  public:
    virtual ~BaseOutputDocInterface() {}
    enum ParamListTypes { Param, RetVal, Exception };
    enum SectionTypes { /*See, Return, Author, Version,
                        Since, Date, Bug, Note,
                        Warning, Par, Deprecated, Pre,
                        Post, Invar, Remark, Attention,
                        Todo, Test, RCS, */
	                EnumValues,
                        Examples
                      };

    virtual void parseText(const QCString &) {}

    /*! Start of a bullet list: e.g. \c \<ul\> in html. startItemListItem() is
     *  Used for the bullet items.
     */
    virtual void startItemList()  = 0;

    /*! Writes a list item for a bullet or enumerated
     *  list: e.g. \c \<li\> in html
     */
    virtual void startItemListItem()  = 0;

    /*! Writes a list item for a bullet or enumerated
     *  list: e.g. \c \</li\> in html
     */
    virtual void endItemListItem()  = 0;

    /*! Ends a bullet list: e.g. \c \</ul\> in html */
    virtual void endItemList()    = 0;

    /*! Writes an ASCII string to the output. Converts characters that have
     *  A special meaning, like \c & in html.
     */
    virtual void docify(const QCString &s) = 0;

    /*! Writes a single ASCII character to the output. Converts characters
     *  that have a special meaning.
     */
    virtual void writeChar(char c) = 0;

    /*! Writes an ASCII string to the output, \e without converting
     *  special characters.
     */
    virtual void writeString(const QCString &text) = 0;

    /*! Starts a new paragraph */
    //virtual void newParagraph()   = 0;

    /*! Starts a new paragraph */
    virtual void startParagraph(const QCString &classDef) = 0;
    /*! Ends a paragraph */
    virtual void endParagraph() = 0;

    /*! Writes a link to an object in the documentation.
     *  \param ref    If this is non-zero, the object is to be found in
     *                an external documentation file.
     *  \param file   The file in which the object is located.
     *  \param anchor The anchor uniquely identifying the object within
     *                the file.
     *  \param name   The text to display as a placeholder for the link.
     */
    virtual void writeObjectLink(const QCString &ref,const QCString &file,
                                 const QCString &anchor, const QCString &name) = 0;


    /*! Starts a (link to an) URL found in the documentation.
     *  \param url    The URL to link to.
     */
    virtual void startHtmlLink(const QCString &url) = 0;

    /*! Ends a link started by startHtmlLink().
     */
    virtual void endHtmlLink() = 0;


    /*! Changes the text font to bold face. The bold section ends with
     *  endBold()
     */
    virtual void startBold()      = 0;

    /*! End a section of text displayed in bold face. */
    virtual void endBold()        = 0;

    /*! Changes the text font to fixed size. The section ends with
     *  endTypewriter()
     */
    virtual void startTypewriter() = 0;

    /*! End a section of text displayed in typewriter style. */
    virtual void endTypewriter() = 0;

    /*! Changes the text font to italic. The italic section ends with
     *  endEmphasis()
     */
    virtual void startEmphasis() = 0;

    /*! Ends a section of text displayed in italic. */
    virtual void endEmphasis() = 0;

    /*! Writes a horizontal ruler to the output */
    virtual void writeRuler() = 0;

    /*! Starts a description list: e.g. \c \<dl\> in HTML
     *  Items are surrounded by startDescItem() and endDescItem()
     */
    virtual void startDescription() = 0;

    /*! Ends a description list: e.g. \c \</dl\> in HTML */
    virtual void endDescription() = 0;

    /*! Starts an item of a description list: e.g. \c \<dt\> in HTML. */
    virtual void startDescItem() = 0;

    virtual void startDescForItem() = 0;
    virtual void endDescForItem() = 0;

    /*! Ends an item of a description list and starts the
     *  description itself: e.g. \c \</dt\> in HTML.
     */
    virtual void endDescItem() = 0;

    virtual void startCenter() = 0;
    virtual void endCenter() = 0;
    virtual void startSmall() = 0;
    virtual void endSmall() = 0;

    virtual void startExamples() = 0;
    virtual void endExamples() = 0;
    virtual void startParamList(ParamListTypes t,const QCString &title) = 0;
    virtual void endParamList() = 0;

    //virtual void writeDescItem() = 0;
    virtual void startTitle() = 0;
    virtual void endTitle()   = 0;

    virtual void writeAnchor(const QCString &fileName,const QCString &name) = 0;
    virtual void startSection(const QCString &,const QCString &,SectionType) = 0;
    virtual void endSection(const QCString &,SectionType) = 0;

    virtual void lineBreak(const QCString &style) = 0;
    virtual void addIndexItem(const QCString &s1,const QCString &s2) = 0;

    virtual void writeNonBreakableSpace(int) = 0;
    virtual void startDescTable(const QCString &title) = 0;
    virtual void endDescTable() = 0;
    virtual void startDescTableRow() = 0;
    virtual void endDescTableRow() = 0;
    virtual void startDescTableTitle() = 0;
    virtual void endDescTableTitle() = 0;
    virtual void startDescTableData() = 0;
    virtual void endDescTableData() = 0;
    virtual void startTextLink(const QCString &file,const QCString &anchor) = 0;
    virtual void endTextLink() = 0;
    virtual void startPageRef() = 0;
    virtual void endPageRef(const QCString &,const QCString &) = 0;
    virtual void startSubsection() = 0;
    virtual void endSubsection() = 0;
    virtual void startSubsubsection() = 0;
    virtual void endSubsubsection() = 0;
};

/** Abstract output generator.
 *
 *  Subclass this class to add support for a new output format
 */
class OutputGenerator : public BaseOutputDocInterface
{
  public:
    enum OutputType { Html, Latex, Man, RTF, XML, DEF, Perl , Docbook};

    OutputGenerator(const QCString &dir);
    OutputGenerator(const OutputGenerator &o);
    OutputGenerator &operator=(const OutputGenerator &o);
    virtual ~OutputGenerator();

    virtual OutputType type() const = 0;
    virtual std::unique_ptr<OutputGenerator> clone() const = 0;

    ///////////////////////////////////////////////////////////////
    // generic generator methods
    ///////////////////////////////////////////////////////////////
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
    //QCString getContents() const;
    bool isEnabled() const { return m_active; }
    void pushGeneratorState();
    void popGeneratorState();
    //void setEncoding(const QCString &enc) { encoding = enc; }
    //virtual void postProcess(QByteArray &) { }

    virtual void writeDoc(DocNode *,const Definition *ctx,const MemberDef *md,int id) = 0;

    ///////////////////////////////////////////////////////////////
    // structural output interface
    ///////////////////////////////////////////////////////////////
    virtual void startFile(const QCString &name,const QCString &manName,
                           const QCString &title,int id=0) = 0;
    virtual void writeSearchInfo() = 0;
    virtual void writeFooter(const QCString &navPath) = 0;
    virtual void endFile() = 0;
    virtual void startIndexSection(IndexSections) = 0;
    virtual void endIndexSection(IndexSections) = 0;
    virtual void writePageLink(const QCString &,bool) = 0;
    virtual void startProjectNumber() = 0;
    virtual void endProjectNumber() = 0;
    virtual void writeStyleInfo(int part) = 0;
    virtual void startTitleHead(const QCString &) = 0;
    virtual void endTitleHead(const QCString &fileName,const QCString &name) = 0;
    virtual void startIndexListItem() = 0;
    virtual void endIndexListItem()   = 0;
    virtual void startIndexList() = 0;
    virtual void endIndexList()   = 0;
    virtual void startIndexKey() = 0;
    virtual void endIndexKey()   = 0;
    virtual void startIndexValue(bool) = 0;
    virtual void endIndexValue(const QCString &,bool) = 0;
    virtual void startIndexItem(const QCString &ref,const QCString &file) = 0;
    virtual void endIndexItem(const QCString &ref,const QCString &file) = 0;
    virtual void startGroupHeader(int) = 0;
    virtual void endGroupHeader(int) = 0;
    virtual void startMemberSections() = 0;
    virtual void endMemberSections() = 0;
    virtual void startHeaderSection() = 0;
    virtual void endHeaderSection() = 0;
    virtual void startMemberHeader(const QCString &anchor, int typ) = 0;
    virtual void endMemberHeader() = 0;
    virtual void startMemberSubtitle() = 0;
    virtual void endMemberSubtitle() = 0;
    virtual void startMemberDocList() = 0;
    virtual void endMemberDocList() = 0;
    virtual void startMemberList() = 0;
    virtual void endMemberList() = 0;
    virtual void startInlineHeader() = 0;
    virtual void endInlineHeader() = 0;
    virtual void startAnonTypeScope(int) = 0;
    virtual void endAnonTypeScope(int) = 0;
    virtual void startMemberItem(const QCString &,int,const QCString &) = 0;
    virtual void endMemberItem() = 0;
    virtual void startMemberTemplateParams() = 0;
    virtual void endMemberTemplateParams(const QCString &,const QCString &) = 0;
    virtual void startCompoundTemplateParams() = 0;
    virtual void endCompoundTemplateParams() = 0;
    virtual void startMemberGroupHeader(bool) = 0;
    virtual void endMemberGroupHeader() = 0;
    virtual void startMemberGroupDocs() = 0;
    virtual void endMemberGroupDocs() = 0;
    virtual void startMemberGroup() = 0;
    virtual void endMemberGroup(bool) = 0;
    virtual void insertMemberAlign(bool) = 0;
    virtual void insertMemberAlignLeft(int,bool) = 0;
    virtual void startMemberDoc(const QCString &,const QCString &,
                                const QCString &,const QCString &,int,int,bool) = 0;
    virtual void endMemberDoc(bool) = 0;
    virtual void startDoxyAnchor(const QCString &fName,const QCString &manName,
                                 const QCString &anchor,const QCString &name,
                                 const QCString &args) = 0;
    virtual void endDoxyAnchor(const QCString &fileName,const QCString &anchor) = 0;
    virtual void writeLatexSpacing() = 0;
    virtual void writeStartAnnoItem(const QCString &type,const QCString &file,
                                    const QCString &path,const QCString &name) = 0;
    virtual void writeEndAnnoItem(const QCString &name) = 0;
    virtual void startMemberDescription(const QCString &anchor,const QCString &inheritId, bool typ) = 0;
    virtual void endMemberDescription() = 0;
    virtual void startMemberDeclaration() = 0;
    virtual void endMemberDeclaration(const QCString &anchor,const QCString &inheritId) = 0;
    virtual void writeInheritedSectionTitle(const QCString &id,const QCString &ref,
                                            const QCString &file,const QCString &anchor,
                                            const QCString &title,const QCString &name) = 0;
    virtual void startIndent() = 0;
    virtual void endIndent() = 0;
    virtual void writeSynopsis() = 0;
    virtual void startClassDiagram() = 0;
    virtual void endClassDiagram(const ClassDiagram &,const QCString &,const QCString &) = 0;
    virtual void startDotGraph() = 0;
    virtual void endDotGraph(DotClassGraph &g) = 0;
    virtual void startInclDepGraph() = 0;
    virtual void endInclDepGraph(DotInclDepGraph &g) = 0;
    virtual void startGroupCollaboration() = 0;
    virtual void endGroupCollaboration(DotGroupCollaboration &g) = 0;
    virtual void startCallGraph() = 0;
    virtual void endCallGraph(DotCallGraph &g) = 0;
    virtual void startDirDepGraph() = 0;
    virtual void endDirDepGraph(DotDirDeps &g) = 0;
    virtual void writeGraphicalHierarchy(DotGfxHierarchyTable &g) = 0;
    virtual void startQuickIndices() = 0;
    virtual void endQuickIndices() = 0;
    virtual void writeSplitBar(const QCString &) = 0;
    virtual void writeNavigationPath(const QCString &) = 0;
    virtual void writeLogo() = 0;
    virtual void writeQuickLinks(bool compact,HighlightedItem hli,const QCString &file) = 0;
    virtual void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first) = 0;
    virtual void startContents() = 0;
    virtual void endContents() = 0;
    virtual void startPageDoc(const QCString &) {}
    virtual void endPageDoc() {}
    virtual void startTextBlock(bool) = 0;
    virtual void endTextBlock(bool) = 0;
    virtual void lastIndexPage() = 0;
    virtual void startMemberDocPrefixItem() = 0;
    virtual void endMemberDocPrefixItem() = 0;
    virtual void startMemberDocName(bool) = 0;
    virtual void endMemberDocName() = 0;
    virtual void startParameterType(bool,const QCString &key) = 0;
    virtual void endParameterType() = 0;
    virtual void startParameterName(bool) = 0;
    virtual void endParameterName(bool,bool,bool) = 0;
    virtual void startParameterList(bool) = 0;
    virtual void endParameterList() = 0;
    virtual void exceptionEntry(const QCString &,bool) = 0;

    virtual void startConstraintList(const QCString &) = 0;
    virtual void startConstraintParam() = 0;
    virtual void endConstraintParam() = 0;
    virtual void startConstraintType() = 0;
    virtual void endConstraintType() = 0;
    virtual void startConstraintDocs() = 0;
    virtual void endConstraintDocs() = 0;
    virtual void endConstraintList() = 0;

    virtual void startMemberDocSimple(bool) = 0;
    virtual void endMemberDocSimple(bool) = 0;
    virtual void startInlineMemberType() = 0;
    virtual void endInlineMemberType() = 0;
    virtual void startInlineMemberName() = 0;
    virtual void endInlineMemberName() = 0;
    virtual void startInlineMemberDoc() = 0;
    virtual void endInlineMemberDoc() = 0;


    virtual void startLabels() = 0;
    virtual void writeLabel(const QCString &,bool) = 0;
    virtual void endLabels() = 0;

    virtual void cleanup() = 0;

  protected:
    TextStream m_t;
  private:
    QCString m_dir;
    QCString m_fileName;
    FILE *m_file;
    bool m_active = true;
    std::stack<bool> m_genStack;
};

/** Interface used for generating documentation.
 *
 *  This abstract class is used by several functions
 *  to generate the output for a specific format.
 *  This interface contains some state saving and changing
 *  functions for dealing with format specific output.
 */
class OutputDocInterface : public BaseOutputDocInterface
{
  public:
    virtual ~OutputDocInterface() {}

    /*! Disables all output formats except format \a o
     *  (useful for OutputList only)
     */
    virtual void disableAllBut(OutputGenerator::OutputType o) = 0;

    /*! Enables all output formats as far as they have been enabled in
     *  the config file. (useful for OutputList only)
     */
    virtual void enableAll() = 0;

    /*! Disables all output formats (useful for OutputList only) */
    virtual void disableAll()= 0;

    /*! Disables a specific output format (useful for OutputList only) */
    virtual void disable(OutputGenerator::OutputType o) = 0;

    /*! Enables a specific output format (useful for OutputList only) */
    virtual void enable(OutputGenerator::OutputType o) = 0;

    /*! Check whether a specific output format is currently enabled
     *  (useful for OutputList only)
     */
    virtual bool isEnabled(OutputGenerator::OutputType o) = 0;

    /*! Appends the output generated by generator \a g to this
     *  generator.
     */
    //virtual void append(const OutputDocInterface *g) = 0;

    /*! Pushes the state of the current generator (or list of
     *  generators) on a stack.
     */
    virtual void pushGeneratorState() = 0;

    /*! Pops the state of the current generator (or list of
     *  generators) on a stack. Should be preceded by a call
     *  the pushGeneratorState().
     */
    virtual void popGeneratorState() = 0;
};

#endif
