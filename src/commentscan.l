/*****************************************************************************
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

%option never-interactive
%option prefix="commentscanYY"
%option reentrant
%option extra-type="struct commentscanYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve typesafety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

/*
 *        includes
 */

#include <map>
#include <stack>
#include <string>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "qcstring.h"
#include "fileinfo.h"
#include "cite.h"
#include "commentscan.h"
#include "condparser.h"
#include "config.h"
#include "debug.h"
#include "docgroup.h"
#include "doxygen.h"
#include "entry.h"
#include "formula.h"
#include "language.h"
#include "message.h"
#include "parserintf.h"
#include "reflist.h"
#include "section.h"
#include "regex.h"
#include "util.h"
#include "reflist.h"
#include "trace.h"
#include "debug.h"
#include "stringutil.h"

// forward declarations
static bool handleBrief(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleFn(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleDef(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleOverload(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleEnum(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleDefGroup(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleAddToGroup(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleWeakGroup(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleRequirement(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleSatisfies(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleVerifies(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleNamespace(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handlePackage(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleConcept(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleClass(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHeaderFile(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleProtocol(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCategory(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleUnion(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleStruct(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleInterface(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIdlException(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handlePage(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleMainpage(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleFile(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleDir(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleExample(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleDetails(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleRaiseWarning(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleNoop(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleName(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleTodo(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleTest(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleBug(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleSubpage(yyscan_t yyscanner,const QCString &s, const StringVector &);
static bool handleDeprecated(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleXRefItem(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleRelated(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleRelatedAlso(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleMemberOf(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleRefItem(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleSection(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleAnchor(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleImage(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCite(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleFormatBlock(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleAddIndex(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIf(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIfNot(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleElseIf(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleElse(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleEndIf(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIngroup(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleNoSubGrouping(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleShowInitializer(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideInitializer(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCallgraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideCallgraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCallergraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideCallergraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIncludegraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIncludedBygraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleShowEnumValues(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideEnumValues(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleShowInlineSource(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideInlineSource(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideIncludegraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideIncludedBygraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleDirectoryGraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideDirectoryGraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCollaborationgraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideCollaborationgraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleInheritanceGraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideInheritanceGraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleReferencedByRelation(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideReferencedByRelation(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleReferencesRelation(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideReferencesRelation(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleGroupgraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleHideGroupgraph(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleInternal(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleStatic(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handlePure(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handlePrivate(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handlePrivateSection(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleProtected(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleProtectedSection(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handlePublic(yyscan_t yyscanner,const QCString &s, const StringVector &);
static bool handlePublicSection(yyscan_t yyscanner,const QCString &s, const StringVector &);
static bool handleQualifier(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleToc(yyscan_t yyscanner,const QCString &s, const StringVector &);
static bool handleInherit(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleExtends(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCopyDoc(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCopyBrief(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleCopyDetails(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleParBlock(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleEndParBlock(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleParam(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleRetval(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleFileInfo(yyscan_t yyscanner,const QCString &cmdName, const StringVector &optList);
static bool handleFileInfoSection(yyscan_t yyscanner,const QCString &cmdName, const StringVector &optList);
static bool handleFileInfoResult(yyscan_t yyscanner,const QCString &, const StringVector &optList, bool isSection);
static bool handleLineInfo(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleModule(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIFile(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleILine(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIRaise(yyscan_t yyscanner,const QCString &, const StringVector &);
static bool handleIPrefix(yyscan_t yyscanner,const QCString &, const StringVector &);

[[maybe_unused]] static const char *stateToString(int state);

static QCString fileInfoLookup(const FileInfo &fi,const std::string &name);

typedef bool (*DocCmdFunc)(yyscan_t yyscanner,const QCString &name, const StringVector &optList);
typedef EntryType (*MakeEntryType)();

enum class CommandSpacing
{
  Invisible, //!< command sets some property but does not appear in the output.
  Inline,    //!< command appears inline in the output which can be a brief description.
  Block,     //!< command starts a new paragraphs / ends a brief description.
  XRef       //!< command is a cross reference (todo, bug, test, deprecated, xrefitem).
};

enum class SectionHandling
{
  Allowed, //!< command is allowed without restrictions in section title
  Replace, //!< command will be handled in here / needs special handling here
  Escape,  //!< command is not-allowed in section title, it will be escaped
  Break    //!< command is not-allowed in section title, it will end the section title
};

struct DocCmdMap
{
  DocCmdMap(DocCmdFunc h,CommandSpacing s,SectionHandling sh) : handler(h), spacing(s), sectionHandling(sh) {}
  DocCmdFunc      handler;
  CommandSpacing  spacing;
  SectionHandling sectionHandling;
};

// map of command to handler function
static const std::unordered_map< std::string, DocCmdMap > docCmdMap =
{
  // command name             handler function                   command spacing            section handling
  { "addindex",               { &handleAddIndex,                 CommandSpacing::Invisible, SectionHandling::Allowed }},
  { "addtogroup",             { &handleAddToGroup,               CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "anchor",                 { &handleAnchor,                   CommandSpacing::Invisible, SectionHandling::Replace }},
  { "arg",                    { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "attention",              { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "author",                 { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "authors",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "brief",                  { &handleBrief,                    CommandSpacing::Invisible, SectionHandling::Break   }},
  { "bug",                    { &handleBug,                      CommandSpacing::XRef,      SectionHandling::Break   }},
  { "callergraph",            { &handleCallergraph,              CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "callgraph",              { &handleCallgraph,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "category",               { &handleCategory,                 CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "cite",                   { &handleCite,                     CommandSpacing::Inline,    SectionHandling::Replace }},
  { "class",                  { &handleClass,                    CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "code",                   { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "collaborationgraph",     { &handleCollaborationgraph,       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "concept",                { &handleConcept,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "copybrief",              { &handleCopyBrief,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "copydetails",            { &handleCopyDetails,              CommandSpacing::Block,     SectionHandling::Escape  }},
  { "copydoc",                { &handleCopyDoc,                  CommandSpacing::Block,     SectionHandling::Escape  }},
  { "copyright",              { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "date",                   { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "def",                    { &handleDef,                      CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "defgroup",               { &handleDefGroup,                 CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "deprecated",             { &handleDeprecated,               CommandSpacing::XRef,      SectionHandling::Break   }},
  { "details",                { &handleDetails,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "diafile",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "dir",                    { &handleDir,                      CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "directorygraph",         { &handleDirectoryGraph,           CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "docbookinclude",         { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "docbookonly",            { &handleFormatBlock,              CommandSpacing::Invisible, SectionHandling::Break   }},
  { "dontinclude",            { nullptr,                         CommandSpacing::Invisible, SectionHandling::Break   }},
  { "dot",                    { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "dotfile",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "else",                   { &handleElse,                     CommandSpacing::Inline,    SectionHandling::Escape  }},
  { "elseif",                 { &handleElseIf,                   CommandSpacing::Inline,    SectionHandling::Escape  }},
  { "endcode",                { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "enddocbookonly",         { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "enddot",                 { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endhtmlonly",            { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endicode",               { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endif",                  { &handleEndIf,                    CommandSpacing::Inline,    SectionHandling::Escape  }},
  { "endlatexonly",           { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endlink",                { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endmanonly",             { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endmsc",                 { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endparblock",            { &handleEndParBlock,              CommandSpacing::Block,     SectionHandling::Escape  }},
  { "endrtfonly",             { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endsecreflist",          { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "enduml",                 { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endverbatim",            { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "endxmlonly",             { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "enum",                   { &handleEnum,                     CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "example",                { &handleExample,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "exception",              { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "extends",                { &handleExtends,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "file",                   { &handleFile,                     CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "fileinfo",               { &handleFileInfo,                 CommandSpacing::Inline,    SectionHandling::Replace }},
  { "fn",                     { &handleFn,                       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "groupgraph",             { &handleGroupgraph,               CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "headerfile",             { &handleHeaderFile,               CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hidecallergraph",        { &handleHideCallergraph,          CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hidecallgraph",          { &handleHideCallgraph,            CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hidecollaborationgraph", { &handleHideCollaborationgraph,   CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hidedirectorygraph",     { &handleHideDirectoryGraph,       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hideenumvalues",         { &handleHideEnumValues,           CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hidegroupgraph",         { &handleHideGroupgraph,           CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hideincludedbygraph",    { &handleHideIncludedBygraph,      CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hideincludegraph",       { &handleHideIncludegraph,         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hideinheritancegraph",   { &handleHideInheritanceGraph,     CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hideinitializer",        { &handleHideInitializer,          CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hideinlinesource",       { &handleHideInlineSource,         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hiderefby",              { &handleHideReferencedByRelation, CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "hiderefs",               { &handleHideReferencesRelation,   CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "htmlinclude",            { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "htmlonly",               { &handleFormatBlock,              CommandSpacing::Invisible, SectionHandling::Break   }},
  { "ianchor",                { &handleAnchor,                   CommandSpacing::Invisible, SectionHandling::Replace }},
  { "icode",                  { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "idlexcept",              { &handleIdlException,             CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "if",                     { &handleIf,                       CommandSpacing::Inline,    SectionHandling::Break   }},
  { "ifile",                  { &handleIFile,                    CommandSpacing::Invisible, SectionHandling::Replace }},
  { "ifnot",                  { &handleIfNot,                    CommandSpacing::Inline,    SectionHandling::Break   }},
  { "iline",                  { &handleILine,                    CommandSpacing::Invisible, SectionHandling::Replace }},
  { "iliteral",               { &handleFormatBlock,              CommandSpacing::Inline,    SectionHandling::Break   }},
  { "image",                  { &handleImage,                    CommandSpacing::Block,     SectionHandling::Break   }},
  { "implements",             { &handleExtends,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "important",              { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "include",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "includedbygraph",        { &handleIncludedBygraph,          CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "includegraph",           { &handleIncludegraph,             CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "includelineno",          { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "ingroup",                { &handleIngroup,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "inherit",                { &handleInherit,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "inheritancegraph",       { &handleInheritanceGraph,         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "interface",              { &handleInterface,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "internal",               { &handleInternal,                 CommandSpacing::Block,     SectionHandling::Break   }},
  { "invariant",              { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "iprefix",                { &handleIPrefix,                  CommandSpacing::Invisible, SectionHandling::Replace }},
  { "iraise",                 { &handleIRaise,                   CommandSpacing::Invisible, SectionHandling::Replace }},
  { "iverbatim",              { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "latexinclude",           { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "latexonly",              { &handleFormatBlock,              CommandSpacing::Invisible, SectionHandling::Break   }},
  { "li",                     { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "line",                   { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "line",                   { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "lineinfo",               { &handleLineInfo,                 CommandSpacing::Inline,    SectionHandling::Replace }},
  { "link",                   { nullptr,                         CommandSpacing::Invisible, SectionHandling::Replace }},
  { "mainpage",               { &handleMainpage,                 CommandSpacing::Invisible, SectionHandling::Break   }},
  { "maninclude",             { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "manonly",                { &handleFormatBlock,              CommandSpacing::Invisible, SectionHandling::Break   }},
  { "memberof",               { &handleMemberOf,                 CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "module",                 { &handleModule,                   CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "msc",                    { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "mscfile",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "name",                   { &handleName,                     CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "namespace",              { &handleNamespace,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "noop",                   { &handleNoop,                     CommandSpacing::Invisible, SectionHandling::Replace }},
  { "nosubgrouping",          { &handleNoSubGrouping,            CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "note",                   { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "overload",               { &handleOverload,                 CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "package",                { &handlePackage,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "page",                   { &handlePage,                     CommandSpacing::Invisible, SectionHandling::Break   }},
  { "par",                    { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "paragraph",              { &handleSection,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "param",                  { &handleParam,                    CommandSpacing::Block,     SectionHandling::Break   }},
  { "parblock",               { &handleParBlock,                 CommandSpacing::Block,     SectionHandling::Break   }},
  { "plantumlfile",           { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "post",                   { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "pre",                    { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "private",                { &handlePrivate,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "privatesection",         { &handlePrivateSection,           CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "property",               { &handleFn,                       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "protected",              { &handleProtected,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "protectedsection",       { &handleProtectedSection,         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "protocol",               { &handleProtocol,                 CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "public",                 { &handlePublic,                   CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "publicsection",          { &handlePublicSection,            CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "pure",                   { &handlePure,                     CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "qualifier",              { &handleQualifier,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "raisewarning",           { &handleRaiseWarning,             CommandSpacing::Invisible, SectionHandling::Replace }},
  { "refitem",                { &handleRefItem,                  CommandSpacing::Inline,    SectionHandling::Escape  }},
  { "related",                { &handleRelated,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "relatedalso",            { &handleRelatedAlso,              CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "relates",                { &handleRelated,                  CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "relatesalso",            { &handleRelatedAlso,              CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "remark",                 { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "remarks",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "requirement",            { &handleRequirement,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "result",                 { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "return",                 { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "returns",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "retval",                 { &handleRetval,                   CommandSpacing::Block,     SectionHandling::Break   }},
  { "rtfinclude",             { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "rtfonly",                { &handleFormatBlock,              CommandSpacing::Invisible, SectionHandling::Break   }},
  { "sa",                     { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "satisfies",              { &handleSatisfies,                CommandSpacing::Inline,    SectionHandling::Break   }},
  { "secreflist",             { nullptr,                         CommandSpacing::Invisible, SectionHandling::Break   }},
  { "section",                { &handleSection,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "see",                    { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "short",                  { &handleBrief,                    CommandSpacing::Invisible, SectionHandling::Break   }},
  { "showdate",               { nullptr,                         CommandSpacing::Inline,    SectionHandling::Allowed }},
  { "showenumvalues",         { &handleShowEnumValues,           CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "showinitializer",        { &handleShowInitializer,          CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "showinlinesource",       { &handleShowInlineSource,         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "showrefby",              { &handleReferencedByRelation,     CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "showrefs",               { &handleReferencesRelation,       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "since",                  { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "skip",                   { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "skipline",               { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "snippet",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "snippetlineno",          { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "startuml",               { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "static",                 { &handleStatic,                   CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "struct",                 { &handleStruct,                   CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "subpage",                { &handleSubpage,                  CommandSpacing::Inline,    SectionHandling::Allowed }},
  { "subparagraph",           { &handleSection,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "subsection",             { &handleSection,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "subsubparagraph",        { &handleSection,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "subsubsection",          { &handleSection,                  CommandSpacing::Block,     SectionHandling::Break   }},
  { "tableofcontents",        { &handleToc,                      CommandSpacing::Invisible, SectionHandling::Break   }},
  { "test",                   { &handleTest,                     CommandSpacing::XRef,      SectionHandling::Break   }},
  { "throw",                  { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "throws",                 { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "todo",                   { &handleTodo,                     CommandSpacing::XRef,      SectionHandling::Break   }},
  { "tparam",                 { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "typedef",                { &handleFn,                       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "union",                  { &handleUnion,                    CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "until",                  { nullptr,                         CommandSpacing::Block,     SectionHandling::Escape  }},
  { "until",                  { nullptr,                         CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "var",                    { &handleFn,                       CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "verbatim",               { &handleFormatBlock,              CommandSpacing::Block,     SectionHandling::Break   }},
  { "verbinclude",            { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "verifies",               { &handleVerifies,                 CommandSpacing::Inline,    SectionHandling::Break   }},
  { "version",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "vhdlflow",               { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "warning",                { nullptr,                         CommandSpacing::Block,     SectionHandling::Break   }},
  { "weakgroup",              { &handleWeakGroup,                CommandSpacing::Invisible, SectionHandling::Escape  }},
  { "xmlinclude",             { nullptr,                         CommandSpacing::Inline,    SectionHandling::Break   }},
  { "xmlonly",                { &handleFormatBlock,              CommandSpacing::Invisible, SectionHandling::Break   }},
  { "xrefitem",               { &handleXRefItem,                 CommandSpacing::XRef,      SectionHandling::Break   }}
};

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1
#define YY_NEVER_INTERACTIVE 1


enum XRefKind
{
  XRef_Item,
  XRef_Todo,
  XRef_Test,
  XRef_Bug,
  XRef_Deprecated,
  XRef_None
};

enum OutputContext
{
  OutputDoc,
  OutputBrief,
  OutputXRef,
  OutputInbody
};

enum GuardType
{
  Guard_If,
  Guard_IfNot,
  Guard_ElseIf
};

/* -----------------------------------------------------------------
 *
 *        statics
 */

struct HtmlContextInfo
{
  HtmlContextInfo(const QCString &tn,OutputContext ctx) : tagName(tn), context(ctx) {}
  QCString      tagName;
  OutputContext context;
};

using HtmlContextStack = std::vector<HtmlContextInfo>;

struct commentscanYY_state
{
  OutlineParserInterface *langParser = nullptr;  // the language parser that is calling us
  QCString         inputString;            // input string
  QCString         currentCmd;             // the command used
  MakeEntryType    currentMakeEntryType = nullptr;
  int              inputPosition = 0;      // read pointer
  QCString         fileName;               // file name that is read from
  int              lineNr = 0;             // line number in the input
  int              raiseLevel = 0;         // section level raise amount
  QCString         raisePrefix;             // section label to append
  bool             inBody = FALSE;         // was the comment found inside the body of a function?
  OutputContext    inContext;              // are we inside the brief, details or xref part
  bool             briefEndsAtDot = FALSE; // does the brief description stop at a dot?
  QCString         formulaText;            // Running text of a formula
  QCString         formulaPreText;         // Start command of a formula
  QCString         formulaPostText;        // End command of a formula
  QCString         formulaEnv;             // environment name
  int              formulaNewLines = 0;    // amount of new lines in the formula
  QCString        *pOutputString = nullptr;  // pointer to string to which the output is appended.
  QCString         outputXRef;               // temp argument of todo/test/../xrefitem commands
  QCString         blockName;                // preformatted block name (e.g. verbatim, latexonly,...)
  XRefKind         xrefKind    = XRef_Item;  // kind of cross-reference command
  XRefKind         newXRefKind = XRef_Item;  //
  GuardType        guardType = Guard_If;     // kind of guards for conditional section
  QCString         functionProto;            // function prototype
  GuardedSectionStack *guards = nullptr;     // tracks nested conditional sections (if,ifnot,..)
  Entry           *current = nullptr;        // working entry

  bool             needNewEntry = FALSE;
  HtmlContextStack htmlContextStack;

  QCString         sectionLabel;
  QCString         sectionTitle;
  int              sectionLevel = 0;
  QCString         xrefItemKey;
  QCString         newXRefItemKey;
  QCString         xrefItemTitle;
  QCString         xrefListTitle;
  Protection       protection = Protection::Public;

  bool             xrefAppendFlag = FALSE;
  bool             inGroupParamFound = FALSE;
  int              braceCount = 0;
  bool             insidePre = FALSE;
  bool             parseMore = FALSE;
  int              condCount = 0;

  int              commentCount = 0;
  QCString         spaceBeforeCmd;
  QCString         spaceBeforeIf;
  QCString         copyDocArg;

  QCString         guardExpr;
  int              roundCount = 0;
  std::vector<int> htmlDetailsStack;

  bool             insideParBlock = FALSE;
  bool             inInternalDocs = FALSE;
  int              prevPosition = 0;
  DocGroup         docGroup;
  bool             markdownSupport = TRUE;

  QCString         raiseWarning;

  QCString         anchorTitle;
  QCString         htmlAnchorStr;
  bool             htmlAnchor = false;
  bool             CScode = false;
  QCString         reqId;
  QCString         reqTitle;
};


static std::mutex g_sectionMutex;
static std::mutex g_formulaMutex;
static std::mutex g_citeMutex;

//-----------------------------------------------------------------------------

static QCString stripQuotes(const char *s);
static SectionType sectionLevelToType(int level);
static void stripTrailingWhiteSpace(QCString &s);

static void initParser(yyscan_t yyscanner);
static bool checkStructuralIndicator(yyscan_t yyscanner);
[[maybe_unused]] static bool makeStructuralIndicator(yyscan_t yyscanner,MakeEntryType maker);
static void lineCount(yyscan_t yyscanner);
static void addRequirementId(yyscan_t yyscanner);
static void addXRefItem(yyscan_t yyscanner,
                        const QCString &listName,const QCString &itemTitle,
                        const QCString &listTitle,bool append);
static QCString addFormula(yyscan_t yyscanner);
static void checkFormula(yyscan_t yyscanner);
static void addSection(yyscan_t yyscanner, bool addYYtext = true);
static inline void setOutput(yyscan_t yyscanner,OutputContext ctx);
static void addAnchor(yyscan_t yyscanner,const QCString &anchor, const QCString &title="");
static inline void addOutput(yyscan_t yyscanner,const char *s);
static inline void addOutput(yyscan_t yyscanner,const QCString &s);
static inline void addOutput(yyscan_t yyscanner,char c);
static void endBrief(yyscan_t yyscanner);
static void handleGuard(yyscan_t yyscanner,const QCString &expr);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void addCite(yyscan_t yyscanner);
static void addIline(yyscan_t yyscanner,int lineNr);
static void addIlineBreak(yyscan_t yyscanner,int lineNr);
static void escapeLabel(QCString &label);

#define unput_string(yytext,yyleng) do { for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]); } while(0)

//-----------------------------------------------------------------------------

#undef        YY_INPUT
#define        YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

       /* start command character */
CMD       ("\\"|"@")
PRE       ("pre"|"PRE")
TABLE     ("table"|"TABLE")
TABLEDEL  ("table"|"tr"|"th"|"td"|"TABLE"|"TR"|"TH"|"TD")
P         [pP]
UL        ("ul"|"UL")
OL        ("ol"|"OL")
DL        ("dl"|"DL")
IMG       ("img"|"IMG")
HR        ("hr"|"HR")
PARA      ("para"|"PARA")
CODE      ("code"|"CODE")
ENDCODE   "/"{CODE}
CAPTION   ("caption"|"CAPTION")
CENTER    ("center"|"CENTER")
DIV       ("div"|"DIV")
DETAILS   ("details"|"DETAILS")
BLOCKQUOTE ("blockquote"|"BLOCKQUOTE")
DETAILEDHTML {CENTER}|{DIV}|{PRE}|{UL}|{TABLE}|{OL}|{DL}|{P}|[Hh][1-6]|{IMG}|{HR}|{PARA}|{BLOCKQUOTE}
DETAILEDHTMLOPT {CODE}
DETAILEDHTMLOPTEND {ENDCODE}
SUMMARY   ("summary"|"SUMMARY")
REMARKS   ("remarks"|"REMARKS")
AHTML     [aA]{BN}*
ANCHTML   ("id"|"name"|"ID"|"NAME")"="("\""{LABELID}"\""|"'"{LABELID}"'"|{LABELID})
BN        [ \t\n\r]
B         [ \t]
Bopt      {B}*
ATTR      ({B}+[^>\n]*)?
DOCNL     "\n"|"\\ilinebr"
LC        "\\"{B}*"\n"
NW        [^a-z_A-Z0-9]
FILESCHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+=@&#~]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+=@&#~]
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]*"\"")
ID        [$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*
LABELID   [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*
REQID     [a-z_A-Z0-9\x80-\xFF\-]+
CITESCHAR [a-z_A-Z0-9\x80-\xFF\-\?]
CITEECHAR [a-z_A-Z0-9\x80-\xFF\-\+:\/\?]*
CITEID    {CITESCHAR}{CITEECHAR}*("."{CITESCHAR}{CITEECHAR}*)*|"\""{CITESCHAR}{CITEECHAR}*("."{CITESCHAR}{CITEECHAR}*)*"\""
SCOPEID   {ID}({ID}*{BN}*"::"{BN}*)*({ID}?)
SCOPENAME "$"?(({ID}?{BN}*("::"|"."){BN}*)*)((~{BN}*)?{ID})
TMPLSPEC  "<"{BN}*[^>]+{BN}*">"
MAILADDR  ("mailto:")?[a-z_A-Z0-9\x80-\xff.+-]+"@"[a-z_A-Z0-9\x80-\xff-]+("."[a-z_A-Z0-9\x80-\xff\-]+)+[a-z_A-Z0-9\x80-\xff\-]+
RCSTAG    "$"{ID}":"[^\n$]+"$"
MODULE_ID ({ID}".")*{ID}
LINENR    {Bopt}[1-9][0-9]*
IFILELINE  ("\\ifile \""[^"]*"\" \\iline "[0-9]+" "("iprefix \""[^"]*"\" ")?("iraise "[0-9]+" ")?)

  // C start comment 
CCS   "/\*"
  // C end comment
CCE   "*\/"

  // end of section title with asterisk
STAopt [^\n@\\*]*
  // end of section title without asterisk
STopt  [^\n@\\]*

%option noyywrap

  /* comment parsing states. */
%x      Comment
%x      PageDocArg1
%x      PageDocArg2
%x      RelatesParam1
%x      ClassDocArg1
%x      ClassDocArg2
%x      ClassDocArg3
%x      CategoryDocArg1
%x      XRefItemParam1
%x      XRefItemParam2
%x      XRefItemParam3
%x      FileDocArg1
%x      ParamArg1
%x      EnumDocArg1
%x      NameSpaceDocArg1
%x      PackageDocArg1
%x      ConceptDocArg1
%x      ModuleDocArg1
%x      GroupDocArg1
%x      GroupDocArg2
%x      SectionLabel
%x      SectionTitle
%x      SubpageLabel
%x      SubpageTitle
%x      SatisfiesLabel
%x      SatisfiesTitle
%x      VerifiesLabel
%x      VerifiesTitle
%x      RequirementLabel
%x      RequirementTitle
%x      FormatBlock
%x      LineParam
%x      GuardParam
%x      GuardParamEnd
%x      SkipGuardedSection
%x      SkipInternal
%x      NameParam
%x      InGroupParam
%x      FnParam
%x      OverloadParam
%x      InheritParam
%x      ExtendsParam
%x      ReadFormulaShort
%x      ReadFormulaShortSection
%x      ReadFormulaRound
%x      ReadFormulaRoundSection
%x      ReadFormulaLong
%x      AnchorLabel
%x      AnchorLabelSection
%x      HtmlComment
%x      HtmlA
%x      SkipLang
%x      CiteLabel
%x      CiteLabelSection
%x      CopyDoc
%x      GuardExpr
%x      CdataSection
%x      Noop
%x      RaiseWarning
%x      RaiseWarningSection
%x      Qualifier
%x      LinkSection
%x      IFile
%x      IFileSection
%x      ILine
%x      ILineSection
%x      IRaise
%x      IRaisePrefix

%%

  /* What can happen in while parsing a comment block:
   *   commands (e.g. @page, or \page)
   *   escaped commands (e.g. @@page or \\page).
   *   formulas (e.g. \f$...\f$ \f[...\f] \f{...\f} \f(...\f) )
   *   directories (e.g. \doxygen\src\)
   *   autolist end. (e.g. a dot on an otherwise empty line)
   *   newlines.
   *   end of brief description due to blank line.
   *   end of brief description due to some command (@command, or <command>).
   *   words and whitespace and other characters (#,?!, etc).
   *   grouping commands (e.g. @{ and @})
   *   language switch (e.g. \~english or \~).
   *   mail address (e.g. doxygen@gmail.com).
   *   quoted text, such as "foo@bar"
   *   XML commands, <summary></summary><remarks></remarks>
   */

<Comment>{CMD}{CMD}[a-z_A-Z]+{B}*       { // escaped command
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>{CMD}{CMD}"~"[a-z_A-Z]*        { // escaped command
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>{MAILADDR}                     { // mail address
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"\""[^"\n]*"\""                { // quoted text
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>("\\"[a-z_A-Z]+)+"\\"          { // directory (or chain of commands!)
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"<"{DETAILEDHTML}{ATTR}">"     { // HTML command that ends a brief description
                                          QCString htmlOpenTag(yytext);
                                          int spacePos = htmlOpenTag.find(' '); // check for optional attributes
                                          if (spacePos==-1) spacePos=yyleng-1;
                                          QCString htmlTagName = htmlOpenTag.mid(1,spacePos-1);
                                          //printf("found open tag '%s'\n",qPrint(htmlTagName));
                                          yyextra->htmlContextStack.emplace_back(htmlTagName,yyextra->inContext);
                                          if (yyextra->inContext==OutputBrief)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                          // continue with the same input
                                          REJECT;
                                        }
<Comment>"</"{DETAILEDHTML}">"          { // HTML command that ends a brief description
                                          QCString htmlCloseTag(yytext);
                                          QCString htmlTagName = htmlCloseTag.mid(2,htmlCloseTag.length()-3);
                                          //printf("found close tag '%s'\n",qPrint(htmlTagName));
                                          if (!yyextra->htmlContextStack.empty() &&
                                              yyextra->htmlContextStack.back().tagName==htmlTagName)
                                          {
                                            if (yyextra->inContext==OutputXRef && yyextra->htmlContextStack.back().context!=OutputXRef)
                                            {
                                              //printf("switching back to OutputDoc\n");
                                              setOutput(yyscanner,OutputDoc);
                                            }
                                            yyextra->htmlContextStack.pop_back();
                                          }
                                          REJECT;
                                        }
<Comment>"<"{DETAILEDHTMLOPT}">"        { // HTML <code> command that ends a brief description
                                          // without attributes
                                          if (yyextra->current->lang==SrcLangExt::CSharp)
                                          {
                                            yyextra->CScode=true;
                                            setOutput(yyscanner,OutputDoc);
                                            addOutput(yyscanner,"@code{cs}");
                                          }
                                          else
                                          {
                                            // continue with the same input
                                            REJECT;
                                          }
                                        }
<Comment>"<"{DETAILEDHTMLOPTEND}">"     { // HTML command that ends a brief description
                                          if (yyextra->CScode)
                                          {
                                            addOutput(yyscanner,"@endcode");
                                            yyextra->CScode=false;
                                          }
                                          else
                                          {
                                            yyextra->CScode=false;
                                            // continue with the same input
                                            REJECT;
                                          }
                                        }
<Comment>"<"{DETAILEDHTMLOPT}{ATTR}">"  { // HTML <code> command that ends a brief description
                                          // with attributes, so cannot be CS.
                                          if (yyextra->current->lang==SrcLangExt::CSharp)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                          // continue with the same input
                                          REJECT;
                                        }
<Comment>"<"{DETAILS}{ATTR}">"          { // start of a HTML style details description
                                          yyextra->htmlDetailsStack.push_back(0);
                                          yyextra->htmlContextStack.emplace_back("details",yyextra->inContext);
                                          if (yyextra->inContext==OutputBrief)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"</"{DETAILS}">"               { // end of a HTML style details description
                                          if (!yyextra->htmlDetailsStack.empty())
                                          {
                                            yyextra->htmlDetailsStack.pop_back();
                                          }
                                          if (!yyextra->htmlContextStack.empty() &&
                                              yyextra->htmlContextStack.back().tagName=="details")
                                          {
                                            if (yyextra->inContext==OutputXRef && yyextra->htmlContextStack.back().context!=OutputXRef)
                                            {
                                              //printf("switching back to OutputDoc\n");
                                              setOutput(yyscanner,OutputDoc);
                                            }
                                            yyextra->htmlContextStack.pop_back();
                                          }
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"<"{AHTML}                     { // potential start of HTML anchor, see issue 9200
                                          yyextra->htmlAnchorStr = yytext;
                                          yyextra->htmlAnchor = false;
                                          BEGIN(HtmlA);
                                        }
<HtmlA>{ANCHTML}                        { // only labels that can be converted to doxygen anchor
                                          yyextra->htmlAnchorStr += yytext;
                                          QCString tag(yytext);
                                          int s=tag.find("=");
                                          char c=tag[s+1];
                                          QCString id;
                                          if (c=='\'' || c=='"') // valid start
                                          {
                                            int e=tag.find(c,s+2);
                                            if (e!=-1) // found matching end
                                            {
                                              id=tag.mid(s+2,e-s-2); // extract id
                                              addAnchor(yyscanner,id);
                                            }
                                          }
                                          else
                                          {
                                            id=tag.mid(s+1);
                                            addAnchor(yyscanner,id);
                                          }
                                          if (!id.isEmpty() && !yyextra->htmlAnchor)
                                          {
                                            // only use first analogous to what is in docparser
                                            addOutput(yyscanner,"@anchor ");
                                            addOutput(yyscanner,id.data());
                                            addOutput(yyscanner," ");
                                            yyextra->htmlAnchor = true;
                                          }
                                        }
<HtmlA>("\""[^\n\"]*"\""|"'"[^\n']*"'") {
                                          yyextra->htmlAnchorStr += yytext;
                                        }
<HtmlA>">"|"/>"                         {
                                          if (!yyextra->htmlAnchor)
                                          {
                                            addOutput(yyscanner,yyextra->htmlAnchorStr);
                                            addOutput(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            if (yyleng == 1) // to keep <a></a> pairs, otherwise single </a> present
                                            {
                                              addOutput(yyscanner,"<a>");
                                            }
                                          }
                                          BEGIN(Comment);
                                        }
<HtmlA>{DOCNL}                          { // newline
                                          yyextra->htmlAnchorStr += yytext;
                                          if (*yytext == '\n') yyextra->lineNr++;
                                        }
<HtmlA>.                                { // catch-all for anything else
                                          yyextra->htmlAnchorStr += yytext;
                                        }
<Comment>"<"{SUMMARY}">"                { // start of a .NET XML style brief description
                                          if (yyextra->htmlDetailsStack.empty())
                                          {
                                            setOutput(yyscanner,OutputBrief);
                                          }
                                          else // HTML5 style <summary> as part of <details> section.
                                          {
                                            addOutput(yyscanner,yytext);
                                          }
                                        }
<Comment>"<"{REMARKS}">"                { // start of a .NET XML style detailed description
                                          setOutput(yyscanner,OutputDoc);
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"</"{SUMMARY}">"               { // start of a .NET XML style detailed description
                                          if (!yyextra->htmlDetailsStack.empty())
                                          {
                                            addOutput(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                        }
<Comment>"</"{REMARKS}">"               { // end of a brief or detailed description
                                          setOutput(yyscanner,OutputDoc);
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"<"{CAPTION}{ATTR}">"          {
                                          QCString tag(yytext);
                                          int s=tag.find("id=");
                                          if (s!=-1) // command has id attribute
                                          {
                                            char c=tag[s+3];
                                            if (c=='\'' || c=='"') // valid start
                                            {
                                              int e=tag.find(c,s+4);
                                              if (e!=-1) // found matching end
                                              {
                                                QCString id=tag.mid(s+4,e-s-4); // extract id
                                                addAnchor(yyscanner,id);
                                              }
                                            }
                                          }
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"<"{PRE}{ATTR}">"              {
                                          yyextra->insidePre=TRUE;
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"</"{PRE}">"                   {
                                          yyextra->insidePre=FALSE;
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>{RCSTAG}                       { // RCS tag which end a brief description
                                          setOutput(yyscanner,OutputDoc);
                                          REJECT;
                                        }
<Comment>"<!--"                         {
                                          BEGIN(HtmlComment);
                                        }
<Comment>"<!\[CDATA\["                  {
                                          BEGIN(CdataSection);
                                        }
<Comment>{B}*{CMD}"endinternal"{B}*     {
                                          addOutput(yyscanner," \\endinternal ");
                                          if (!yyextra->inInternalDocs)
                                              warn(yyextra->fileName,yyextra->lineNr,
                                               "found \\endinternal without matching \\internal"
                                              );
                                          yyextra->inInternalDocs = FALSE;
                                        }
<Comment>{B}*"\\ilinebr "{B}*           { // preserve spacing around \\ilinebr
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>(\n|"\\ilinebr ")/({B}*(\n|{IFILELINE}?"\\ilinebr "))+ { // at least one blank line (or blank line command)
                                          if (yyextra->inContext==OutputBrief)
                                          {
                                            endBrief(yyscanner);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<Comment>{B}*{CMD}[a-z_A-Z]+"{"[^}]*"}"{B}*  |
<Comment>{B}*{CMD}[a-z_A-Z]+{B}*        { // potentially interesting command
                                          // the {B}* in the front was added for bug620924
                                          QCString fullMatch = yytext;
                                          int idx = fullMatch.find('{');
                                          /* handle `f{` command as special case */
                                          if ((idx > 1) && (yytext[idx-1] == 'f') && (yytext[idx-2] == '\\' || yytext[idx-2] =='@')) REJECT;
                                          int idxEnd = fullMatch.find("}",idx+1);
                                          QCString cmdName;
                                          StringVector optList;
                                          if (idx == -1) // no options
                                          {
                                            cmdName = fullMatch.stripWhiteSpace().mid(1); // to remove {CMD}
                                          }
                                          else // options present
                                          {
                                            cmdName = fullMatch.left(idx).stripWhiteSpace().mid(1); // to remove {CMD}
                                            QCString optStr = fullMatch.mid(idx+1,idxEnd-idx-1).stripWhiteSpace();
                                            optList = split(optStr.str(),",");
                                          }
                                          auto it = docCmdMap.find(cmdName.str());
                                          //printf("lookup command '%s' found=%d\n",qPrint(cmdName),it!=docCmdMap.end());
                                          if (it!=docCmdMap.end()) // special action is required
                                          {
                                            int i=0;
                                            while (yytext[i]==' ' || yytext[i]=='\t') i++;
                                            yyextra->spaceBeforeCmd = fullMatch.left(i);
                                            CommandSpacing spacing = it->second.spacing;
                                            if ((spacing==CommandSpacing::Block || spacing==CommandSpacing::XRef) &&
                                                !(yyextra->inContext==OutputXRef && cmdName=="parblock"))
                                            {
                                              yyextra->briefEndsAtDot=FALSE;
                                              bool insideXRef = yyextra->inContext==OutputXRef && spacing==CommandSpacing::XRef;
                                              // this command forces the end of brief description
                                              setOutput(yyscanner,insideXRef ? OutputXRef : OutputDoc);
                                            }
                                            //if (i>0) addOutput(yyscanner,QCString(yytext).left(i)); // removed for bug 689341
                                            if (it->second.handler && it->second.handler(yyscanner, cmdName, optList))
                                            {
                                              // implicit split of the comment block into two
                                              // entries. Restart the next block at the start
                                              // of this command.
                                              yyextra->parseMore=TRUE;

                                              yyextra->inputPosition=yyextra->prevPosition + (int)(yy_bp - YY_CURRENT_BUFFER_LVALUE->yy_ch_buf);
                                              yyterminate();
                                            }
                                            else if (it->second.handler==nullptr)
                                            {
                                              // command without handler, to be processed
                                              // later by parsedoc.cpp
                                              addOutput(yyscanner,yytext);
                                            }
                                          }
                                          else // command not relevant
                                          {
                                            addOutput(yyscanner,yytext);
                                          }
                                        }
<Comment>{B}*({CMD}{CMD})"f"[$\[{]      { // escaped formula command
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>{B}*{CMD}"~"[a-z_A-Z-]*        { // language switch command
                                          QCString langId = QCString(yytext).stripWhiteSpace().mid(2);
                                          if (!langId.isEmpty() &&
                                              qstricmp(Config_getEnumAsString(OUTPUT_LANGUAGE),langId)!=0)
                                          { // enable language specific section
                                            if (!Config_isAvailableEnum(OUTPUT_LANGUAGE,langId))
                                            {
                                               warn(yyextra->fileName,yyextra->lineNr,
                                               "non supported language '{}' specified in '{}'",langId,QCString(yytext).stripWhiteSpace());
                                            }
                                            BEGIN(SkipLang);
                                          }
                                        }
<Comment>{B}*{CMD}"f{"[^}\n]+"}"("{"?)  { // start of a formula with custom environment
                                          setOutput(yyscanner,OutputDoc);
                                          yyextra->formulaText="";
                                          yyextra->formulaPreText="\\begin";
                                          yyextra->formulaPostText="";
                                          yyextra->formulaEnv=QCString(yytext).stripWhiteSpace().mid(2);
                                          if (yyextra->formulaEnv.at(yyextra->formulaEnv.length()-1)=='{')
                                          {
                                            // remove trailing open brace
                                            yyextra->formulaEnv=yyextra->formulaEnv.left(yyextra->formulaEnv.length()-1);
                                          }
                                          yyextra->formulaPreText+=yyextra->formulaEnv;
                                          yyextra->formulaNewLines=0;
                                          BEGIN(ReadFormulaLong);
                                        }
<Comment>{B}*{CMD}"f$"                  { // start of a inline formula
                                          yyextra->formulaText="";
                                          yyextra->formulaPreText="$";
                                          yyextra->formulaPostText="";
                                          yyextra->formulaNewLines=0;
                                          BEGIN(ReadFormulaShort);
                                        }
<Comment>{B}*{CMD}"f("                  { // start of a inline formula
                                          yyextra->formulaText="";
                                          yyextra->formulaPreText="";
                                          yyextra->formulaPostText="";
                                          yyextra->formulaNewLines=0;
                                          BEGIN(ReadFormulaRound);
                                        }
<Comment>{B}*{CMD}"f["                  { // start of a block formula
                                          setOutput(yyscanner,OutputDoc);
                                          yyextra->formulaText="";
                                          yyextra->formulaPreText="\\[";
                                          yyextra->formulaPostText="";
                                          yyextra->formulaNewLines=0;
                                          BEGIN(ReadFormulaLong);
                                        }
<Comment>{B}*{CMD}"{"                   { // begin of a group
                                          //yyextra->langParser->handleGroupStartCommand(yyextra->memberGroupHeader);
                                          yyextra->docGroup.open(yyextra->current,yyextra->fileName,yyextra->lineNr);
                                        }
<Comment>{B}*{CMD}"}"                   { // end of a group
                                          //yyextra->langParser->handleGroupEndCommand();
                                          yyextra->docGroup.close(yyextra->current,yyextra->fileName,yyextra->lineNr,TRUE);
                                          yyextra->docGroup.clearHeader();
                                          yyextra->parseMore=TRUE;
                                          yyextra->needNewEntry = TRUE;
                                          yyextra->inputPosition=yyextra->prevPosition + (int)(yy_bp - YY_CURRENT_BUFFER_LVALUE->yy_ch_buf) + (int)strlen(yytext);
                                          yyterminate();
                                        }
<Comment>{B}*{CMD}[$@\\&~<>#%]          { // escaped character
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>[a-z_A-Z]+                     { // normal word
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>^{B}*"."{Bopt}/\n              { // explicit end autolist: e.g "  ."
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>^{B}*[1-9][0-9]*"."{B}+        |
<Comment>^{B}*[*+]{B}+                  { // start of autolist
                                          if (!yyextra->markdownSupport)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            if (yyextra->inContext!=OutputXRef)
                                            {
                                              yyextra->briefEndsAtDot=FALSE;
                                              setOutput(yyscanner,OutputDoc);
                                            }
                                            addOutput(yyscanner,yytext);
                                          }
                                          }
<Comment>^{B}*"-"{B}+                        { // start of autolist
                                          if (yyextra->inContext!=OutputXRef)
                                          {
                                            yyextra->briefEndsAtDot=FALSE;
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>^{B}*([\-:|]{B}*)*("--"|"---")({B}*[\-:|])*{Bopt}/\n { // horizontal line (dashed)
                                            addOutput(yyscanner,yytext);
                                        }
<Comment>{CMD}"---"                     { // escaped mdash
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>{CMD}"--"                      { // escaped mdash
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"---"                          { // mdash
                                          addOutput(yyscanner,yyextra->insidePre || yyextra->markdownSupport ? yytext : "&mdash;");
                                        }
<Comment>"--"                           { // ndash
                                          addOutput(yyscanner,yyextra->insidePre || yyextra->markdownSupport ? yytext : "&ndash;");
                                        }
<Comment>"-#"{B}+                       { // numbered item
                                          if (yyextra->inContext!=OutputXRef)
                                          {
                                            yyextra->briefEndsAtDot=FALSE;
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>[?!][a-z_A-Z0-9\(\)=<]         |
<Comment>("."+)[a-z_A-Z0-9\)]           { // . at start or in the middle of a word, or ellipsis
                                          // ? or ! in middle of word or followed by equal sign or round bracket.
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>{CMD}[\.?!]                    { // we have to be a bit careful with the special commands
                                          // \. \? and \! as they might otherwise terminate a brief description
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>".\\"[ \t]                     { // . with escaped space.
                                          addOutput(yyscanner,yytext[0]);
                                          addOutput(yyscanner,yytext[2]);
                                        }
<Comment>"."[,:;]                       { // . with some puntuations such as "e.g.," or "e.g.:"
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>"...\\"[ \t]                   { // ellipsis with escaped space.
                                          addOutput(yyscanner,"... ");
                                        }
<Comment>"..."/[^\.]                    { // ellipsis
                                          addOutput(yyscanner,"...");
                                        }
<Comment>".."[\.]?/[^ \t\n]             { // internal ellipsis
                                          addOutput(yyscanner,yytext);
                                        }
<Comment>(\n|"\\ilinebr ")({B}*(\n|"\\ilinebr "))+ { // at least one blank line (or blank line command)
                                          if (yyextra->inContext==OutputXRef)
                                          {
                                            // see bug 613024, we need to put the newlines after ending the XRef section.
                                            if (!yyextra->insideParBlock) setOutput(yyscanner,OutputDoc);
                                            yy_size_t i;
                                            for (i=0;i<(yy_size_t)yyleng;)
                                            {
                                              if (yytext[i]=='\n') addOutput(yyscanner,'\n'),i++;
                                              else if (strncmp(yytext+i,"\\ilinebr ",9)==0) addOutput(yyscanner,"\\ilinebr "),i+=9;
                                              else i++;
                                            }
                                          }
                                          else if (yyextra->inContext!=OutputBrief)
                                          {
                                            yy_size_t i;
                                            for (i=0;i<(yy_size_t)yyleng;)
                                            {
                                              if (yytext[i]=='\n') addOutput(yyscanner,'\n'),i++;
                                              else if (strncmp(yytext+i,"\\ilinebr ",9)==0) addOutput(yyscanner,"\\ilinebr "),i+=9;
                                              else i++;
                                            }
                                            setOutput(yyscanner,OutputDoc);
                                          }
                                          else // yyextra->inContext==OutputBrief
                                          { // only go to the detailed description if we have
                                            // found some brief description and not just whitespace
                                            endBrief(yyscanner);
                                          }
                                          lineCount(yyscanner);
                                        }
<Comment>"."[?!]                        |
<Comment>[\.?!]                         { // potential end of a JavaDoc style comment
                                          addOutput(yyscanner,yytext);
                                          if (yyextra->briefEndsAtDot)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                            yyextra->briefEndsAtDot=FALSE;
                                          }
                                        }
<Comment>{DOCNL}                        { // newline
                                          addOutput(yyscanner,yytext);
                                          if (*yytext == '\n') yyextra->lineNr++;
                                        }
<Comment>"<"[/]?{TABLEDEL}">"           { // In case in xrefitem type some special handling is required
                                          if (yyextra->inContext==OutputXRef)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                            addOutput(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<Comment>.                              { // catch-all for anything else
                                          addOutput(yyscanner,*yytext);
                                        }


 /* --------------   Rules for handling HTML comments ----------- */

<HtmlComment>"---"[!]?">"{B}*           {
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "incorrect HTML end comment --->"
                                              );
                                        }
<HtmlComment>"--"[!]?">"{B}*            { BEGIN( Comment ); }
<HtmlComment>{DOCNL}                    {
                                          if (*yytext=='\n')
                                          {
                                            yyextra->lineNr++;
                                            addOutput(yyscanner," \\iline "+QCString().setNum(yyextra->lineNr)+" "); 
                                          }
                                        }
<HtmlComment>[^\\\n\-]+                 { // ignore unimportant characters
                                        }
<HtmlComment>.                          { // ignore every else
                                        }

<CdataSection>"\]\]>"                   {
                                          BEGIN( Comment );
                                        }
<CdataSection>{DOCNL}                   {
                                          addOutput(yyscanner,'\n');
                                          if (*yytext=='\n') yyextra->lineNr++;
                                        }
<CdataSection>[<>&]                     { // the special XML characters for iwhich the CDATA section is especially used
                                          addOutput(yyscanner,'\\');
                                          addOutput(yyscanner,*yytext);
                                        }
<CdataSection>[^\\\n\]<>&]+             {
                                          addOutput(yyscanner,yytext);
                                        }
<CdataSection>.                         {
                                          addOutput(yyscanner,*yytext);
                                        }

 /* --------------   Rules for handling formulas ---------------- */

<ReadFormulaShort,ReadFormulaShortSection>{CMD}"f$" { // end of inline formula
                                          yyextra->formulaPostText+="$";
                                          QCString form = addFormula(yyscanner);
                                          addOutput(yyscanner," "+form);
                                          if (YY_START == ReadFormulaShort)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+= " "+form;
                                            BEGIN(SectionTitle);
                                          }
                                        }
<ReadFormulaRound,ReadFormulaRoundSection>{CMD}"f)" { // end of inline formula
                                          QCString form = addFormula(yyscanner);
                                          addOutput(yyscanner," "+form);
                                          if (YY_START == ReadFormulaRound)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+= " "+form;
                                            BEGIN(SectionTitle);
                                          }
                                        }
<ReadFormulaLong>{CMD}"f]"              { // end of block formula
                                          yyextra->formulaPostText+="\\]";
                                          addOutput(yyscanner," "+addFormula(yyscanner));
                                          BEGIN(Comment);
                                        }
<ReadFormulaLong>{CMD}"f}"              { // end of custom env formula
                                          yyextra->formulaPostText+="\\end";
                                          yyextra->formulaPostText+=yyextra->formulaEnv;
                                          addOutput(yyscanner," "+addFormula(yyscanner));
                                          BEGIN(Comment);
                                        }
<ReadFormulaLong,ReadFormulaShort,ReadFormulaShortSection,ReadFormulaRound,ReadFormulaRoundSection>[^\\@\n]+ { // any non-special character
                                          yyextra->formulaText+=yytext;
                                        }
<ReadFormulaLong,ReadFormulaShort,ReadFormulaShortSection,ReadFormulaRound,ReadFormulaRoundSection>\n    { // new line
                                          yyextra->formulaNewLines++;
                                          yyextra->formulaText+=*yytext;
                                          yyextra->lineNr++;
                                          addIline(yyscanner,yyextra->lineNr);
                                        }
<ReadFormulaLong,ReadFormulaShort,ReadFormulaShortSection,ReadFormulaRound,ReadFormulaRoundSection>.     { // any other character
                                          yyextra->formulaText+=*yytext;
                                        }

  /* ------------ handle argument of enum command --------------- */

<EnumDocArg1>{SCOPEID}                  { // handle argument
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          yyextra->current->name = yytext;
                                          BEGIN( Comment );
                                        }
<EnumDocArg1>{LC}                       { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<EnumDocArg1>{DOCNL}                    { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after '\\enum'."
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<EnumDocArg1>.                          { // ignore other stuff
                                        }

  /* ------------ handle argument of namespace command --------------- */

<NameSpaceDocArg1>{SCOPENAME}           { // handle argument
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          lineCount(yyscanner);
                                          yyextra->current->name = substitute(removeRedundantWhiteSpace(yytext),".","::");
                                          BEGIN( Comment );
                                        }
<NameSpaceDocArg1>{LC}                  { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<NameSpaceDocArg1>{DOCNL}               { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after '\\namespace'."
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<NameSpaceDocArg1>.                     { // ignore other stuff
                                        }

  /* ------------ handle argument of package command --------------- */

<PackageDocArg1>{ID}("."{ID})*          { // handle argument
                                          yyextra->current->name = yytext;
                                          BEGIN( Comment );
                                        }
<PackageDocArg1>{LC}                    { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<PackageDocArg1>{DOCNL}                 { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after \\package."
                                              );
                                          unput_string(yytext,yyleng);
                                          //addOutput(yyscanner,'\n');
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          BEGIN( Comment );
                                        }
<PackageDocArg1>.                       { // ignore other stuff
                                        }

  /* ------------ handle argument of concept command --------------- */

<ConceptDocArg1>{SCOPEID}               { // handle argument
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          yyextra->current->name = yytext;
                                          BEGIN( Comment );
                                        }
<ConceptDocArg1>{LC}                    { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<ConceptDocArg1>{DOCNL}                 { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after '\\concept'."
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<ConceptDocArg1>.                       { // ignore other stuff
                                        }

  /* ------------ handle argument of module command --------------- */
<ModuleDocArg1>{MODULE_ID}              { // handle argument
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          yyextra->current->name = yytext;
                                          BEGIN( Comment );
                                        }
<ModuleDocArg1>{LC}                     { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<ModuleDocArg1>{DOCNL}                  { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after '\\module'."
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<ModuleDocArg1>.                        { // ignore other stuff
                                        }

  /* ------ handle argument of class/struct/union command --------------- */

<ClassDocArg1>{SCOPENAME}{TMPLSPEC}     {
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          lineCount(yyscanner);
                                          yyextra->current->name = substitute(removeRedundantWhiteSpace(yytext),".","::");
                                          BEGIN( ClassDocArg2 );
                                        }
<ClassDocArg1>{SCOPENAME}               { // first argument
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          lineCount(yyscanner);
                                          yyextra->current->name = substitute(yytext,".","::");
                                          if (yyextra->current->section.isProtocolDoc())
                                          {
                                            yyextra->current->name+="-p";
                                          }
                                          // prepend outer scope name
                                          BEGIN( ClassDocArg2 );
                                        }
<CategoryDocArg1>{SCOPENAME}{B}*"("[^\)]+")" {
                                          makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                          lineCount(yyscanner);
                                          yyextra->current->name = substitute(yytext,".","::");
                                          BEGIN( ClassDocArg2 );
                                        }
<ClassDocArg1,CategoryDocArg1>{LC}      { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<ClassDocArg1,CategoryDocArg1>{DOCNL}   {
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after '\\{}'.",yyextra->currentCmd
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<ClassDocArg1,CategoryDocArg1>.         { // ignore other stuff
                                        }

<ClassDocArg2>{DOCNL}                   {
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<ClassDocArg2>{FILE}|"<>"               { // second argument; include file
                                          yyextra->current->includeFile = yytext;
                                          BEGIN( ClassDocArg3 );
                                        }
<ClassDocArg2>{LC}                      { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<ClassDocArg2>.                         { // ignore other stuff
                                        }

<ClassDocArg3>[<"]?{FILE}?[">]?         { // third argument; include file name
                                          yyextra->current->includeName = yytext;
                                          BEGIN( Comment );
                                        }
<ClassDocArg3>{LC}                      { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<ClassDocArg3>{DOCNL}                   {
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<ClassDocArg3>.                         { // ignore other stuff
                                        }

  /* --------- handle arguments of {def,add,weak}group commands --------- */

<GroupDocArg1>{LABELID}(".html"|".xhtml")? { // group name
                                           yyextra->current->name = yytext;
                                          //lastDefGroup.groupname = yytext;
                                          //lastDefGroup.pri = yyextra->current->groupingPri();
                                          // the .html stuff is for Qt compatibility
                                          if (yyextra->current->name.endsWith(".html"))
                                          {
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-5);
                                          }
                                          else if (yyextra->current->name.endsWith(".xhtml"))
                                          {
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-6);
                                          }
                                          yyextra->current->type.clear();
                                          BEGIN(GroupDocArg2);
                                        }
<GroupDocArg1>"\\"{B}*"\n"              { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<GroupDocArg1>{DOCNL}                   { // missing argument!
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing group name after {}",
                                               yyextra->current->groupDocCmd()
                                              );
                                          //addOutput(yyscanner,'\n');
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<GroupDocArg1>.                         { // ignore other stuff
                                        }
<GroupDocArg2>"\\"{B}*"\n"              { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<GroupDocArg2>[^\n\\]+                  { // title (stored in type)
                                          yyextra->current->type += yytext;
                                        }
<GroupDocArg2>{DOCNL}+                  {
                                          yyextra->current->type = yyextra->current->type.stripWhiteSpace();
                                          if ( yyextra->current->groupDocType==Entry::GROUPDOC_NORMAL &&
                                               yyextra->current->type.isEmpty()
                                             ) // defgroup requires second argument
                                          {
                                              warn(yyextra->fileName,yyextra->lineNr,
                                                 "missing title after "
                                                 "\\defgroup {}", yyextra->current->name
                                                );
                                          }
                                          unput_string(yytext,yyleng);
                                          int extraLineNr = 0;
                                          if (yyextra->inContext == OutputBrief)
                                          {
                                            for (int i = 0; i < yyleng; i++)
                                            {
                                              if (yytext[i]=='\n') extraLineNr++;
                                            }
                                          }
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          if ( yyextra->current->groupDocType!=Entry::GROUPDOC_NORMAL)
                                          {
                                            addOutput(yyscanner," \\ifile \""+ yyextra->fileName);
                                            addOutput(yyscanner,"\" \\iline " + QCString().setNum(yyextra->lineNr + extraLineNr) + " \\ilinebr ");
                                          }
                                          BEGIN( Comment );
                                        }
<GroupDocArg2>.                         { // title (stored in type)
                                          yyextra->current->type += yytext;
                                        }

  /* --------- handle arguments of page/mainpage command ------------------- */

<PageDocArg1>[^\n]*"\\ilinebr @ianchor"\{[^\]\n]*\}{B}{FILE} { // special case where the Markdown processor has rewritten
                                                               // "@page label Title" as
                                                               // "@page md_label Title\ilinebr @ianchor{Title} label"
                                          QCString text = yytext;
                                          int start = text.find('{');
                                          int end   = text.find('}',start+1);
                                          yyextra->current->name = text.mid(end+2);
                                          int istart = yyextra->current->name.find("\\ilinebr");
                                          if (istart != -1)
                                          {
                                            QCString rest =  yyextra->current->name.mid(istart);
                                            unput_string(rest,rest.length());
                                            yyextra->current->name = yyextra->current->name.mid(0,istart);
                                          }
                                          yyextra->current->args = text.mid(start+1,end-start-1);
                                          //printf("name='%s' title='%s'\n",qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                          BEGIN( PageDocArg2 );
                                        }
<PageDocArg1>{FILE}                     { // first argument; page name
                                          yyextra->current->name = stripQuotes(yytext);
                                          yyextra->current->args = "";
                                          BEGIN( PageDocArg2 );
                                        }
<PageDocArg1>{LC}                       { yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<PageDocArg1>{DOCNL}                    {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                               "missing argument after \\page."
                                              );
                                          unput_string(yytext,yyleng);
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          BEGIN( Comment );
                                        }
<PageDocArg1>.                          { // ignore other stuff
                                        }
<PageDocArg2>{DOCNL}                    { // second argument; page title
                                          unput_string(yytext,yyleng);
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          addOutput(yyscanner," \\ifile \""+ yyextra->fileName);
                                          addOutput(yyscanner,"\" \\iline " + QCString().setNum(yyextra->lineNr) + " \\ilinebr ");
                                          BEGIN( Comment );
                                        }
<PageDocArg2>{CMD}[<>]                  {
                                          // bug 748927
                                          QCString tmp(yytext);
                                          tmp = substitute(substitute(tmp,"@<","&lt;"),"@>","&gt;");
                                          tmp = substitute(substitute(tmp,"\\<","&lt;"),"\\>","&gt;");
                                          yyextra->current->args += tmp;
                                        }
<PageDocArg2>.                          {
                                          yyextra->current->args += yytext;
                                        }
  /* --------- handle arguments of the param command ------------ */
<ParamArg1>{ID}/{B}*","                 {
                                          addOutput(yyscanner,yytext);
                                        }
<ParamArg1>","                          {
                                          addOutput(yyscanner," , ");
                                        }
<ParamArg1>{DOCNL}                      {
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner," ");
                                        }
<ParamArg1>{ID}                         {
                                          addOutput(yyscanner,yytext);
                                          BEGIN( Comment );
                                        }
<ParamArg1>.                            {
                                          unput(yytext[0]);
                                          BEGIN( Comment );
                                        }

  /* --------- handle arguments of the file/dir/example command ------------ */

<FileDocArg1>{DOCNL}                    { // no file name specified
                                          unput_string(yytext,yyleng);
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          BEGIN( Comment );
                                        }
<FileDocArg1>{FILE}                     { // first argument; name
                                          yyextra->current->name = stripQuotes(yytext);
                                          BEGIN( Comment );
                                        }
<FileDocArg1>{LC}                       { yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<FileDocArg1>.                          { // ignore other stuff
                                        }

  /* --------- handle arguments of the xrefitem command ------------ */

<XRefItemParam1>{LABELID}               { // first argument
                                          yyextra->newXRefItemKey=yytext;
                                          setOutput(yyscanner,OutputXRef);
                                          BEGIN(XRefItemParam2);
                                        }
<XRefItemParam1>{LC}                    { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<XRefItemParam1>{DOCNL}                 { // missing arguments
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "Missing first argument of \\xrefitem"
                                              );
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          yyextra->inContext = OutputDoc;
                                          BEGIN( Comment );
                                        }
<XRefItemParam1>.                       { // ignore other stuff
                                        }

<XRefItemParam2>"\""[^\n\"]*"\""        { // second argument
                                          yyextra->xrefItemTitle = stripQuotes(yytext);
                                          BEGIN(XRefItemParam3);
                                        }
<XRefItemParam2>{LC}                    { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<XRefItemParam2>{DOCNL}                 { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Missing second argument of \\xrefitem"
                                              );
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          yyextra->inContext = OutputDoc;
                                          BEGIN( Comment );
                                        }
<XRefItemParam2>.                       { // ignore other stuff
                                        }

<XRefItemParam3>"\""[^\n\"]*"\""        { // third argument
                                          yyextra->xrefListTitle = stripQuotes(yytext);
                                          yyextra->xrefKind = XRef_Item;
                                          BEGIN( Comment );
                                        }
<XRefItemParam2,XRefItemParam3>{LC}     { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<XRefItemParam3>{DOCNL}                 { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Missing third argument of \\xrefitem"
                                              );
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          yyextra->inContext = OutputDoc;
                                          BEGIN( Comment );
                                        }
<XRefItemParam3>.                       { // ignore other stuff
                                        }


  /* ----- handle arguments of the relates(also)/memberof command ------- */

<RelatesParam1>({ID}("::"|"."))*{ID}    { // argument
                                          yyextra->current->relates = yytext;
                                          //if (yyextra->current->mGrpId!=DOX_NOGROUP)
                                          //{
                                          //  memberGroupRelates = yytext;
                                          //}
                                          BEGIN( Comment );
                                        }
<RelatesParam1>{LC}                     { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<RelatesParam1>{DOCNL}                  { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Missing argument of '\\{}' command",yyextra->currentCmd
                                              );
                                          unput_string(yytext,yyleng);
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          BEGIN( Comment );
                                        }
<RelatesParam1>.                        { // ignore other stuff
                                        }


  /* ----- handle arguments of the qualifier command ----- */
<Qualifier>{LABELID}                    { // unquoted version, simple label
                                          yyextra->current->qualifiers.emplace_back(yytext);
                                          BEGIN( Comment );
                                        }
<Qualifier>"\""[^\"]*"\""               { // quotes version, add without quotes
                                          std::string inp(yytext);
                                          yyextra->current->qualifiers.push_back(inp.substr(1,yyleng-2));
                                          BEGIN( Comment );
                                        }
<Qualifier>{DOCNL}                      { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Missing argument of '\\{}' command",yyextra->currentCmd
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<Qualifier>.                            {
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Argument of '\\{}' command should be quoted",yyextra->currentCmd
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
  /* ----- handle arguments of the iline command ----- */
<ILine>{LINENR}/[\\@\n\.]               |
<ILine>{LINENR}{B}                      {
                                          bool ok = false;
                                          int nr = QCString(yytext).toInt(&ok);
                                          if (!ok)
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,"Invalid line number '{}' for iline command",yytext);
                                          }
                                          else
                                          {
                                            yyextra->lineNr = nr;
                                          }
                                          addOutput(yyscanner,yytext);
                                          if (YY_START == ILine)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            BEGIN(SectionTitle);
                                          }
                                        }
<ILine,ILineSection>.                   {
                                          addOutput(yyscanner,yytext);
                                          if (YY_START == ILine)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            BEGIN(SectionTitle);
                                          }
                                        }

  /* ----- handle arguments of the iraise command ----- */
<IRaise>{B}*[0-9]+/[\\@\n\.]               |
<IRaise>{B}*[0-9]+{B}                      {
                                          bool ok = false;
                                          int nr = QCString(yytext).toInt(&ok);
                                          if (!ok)
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,"Invalid level '{}' for iraise command",yytext);
                                          }
                                          else
                                          {
                                            yyextra->raiseLevel = nr;
                                          }
                                          BEGIN(Comment);
                                        }
<IRaise>.                               {
                                          unput(yytext[0]);
                                          BEGIN(Comment);
                                        }
  /* ----- handle arguments of the iprefix command ----- */

<IRaisePrefix>{B}*"\""({LABELID})?"\""  {
                                          QCString text(yytext);
                                          yyextra->raisePrefix = text.stripWhiteSpace().mid(1,text.length()-2);
                                          addOutput(yyscanner,yytext);
                                          BEGIN(Comment);
                                        }
<IRaisePrefix>.                         {
                                          unput(yytext[0]);
                                          BEGIN(Comment);
                                        }


  /* ----- handle arguments of the ifile command ----- */

<IFile,IFileSection>{FILE}              {
                                          addOutput(yyscanner,yytext);
                                          QCString text(yytext);
                                          if (yytext[0] == '\"') yyextra->fileName = text.mid(1,text.length()-2);
                                          else yyextra->fileName = yytext;
                                          if (YY_START == IFile)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            BEGIN(SectionTitle);
                                          }
                                        }

<LinkSection>[^\\@\n]*                  {
                                          yyextra->sectionTitle+=yytext;
                                        }
<LinkSection>{CMD}{CMD}                 {
                                          yyextra->sectionTitle+=yytext;
                                        }
<LinkSection>{DOCNL}                    {
                                          addOutput(yyscanner,yytext);
                                          if (*yytext == '\n') yyextra->lineNr++;
                                          yyextra->sectionTitle+=yytext;
                                        }
<LinkSection>{CMD}"endlink"             {
                                          yyextra->sectionTitle+=yytext;
                                          BEGIN(SectionTitle);
                                        }
<LinkSection>.                          {
                                          yyextra->sectionTitle+=yytext;
                                        }
<LinkSection><<EOF>>                    {
                                          warn(yyextra->fileName,yyextra->lineNr,
                                            "reached end of comment while inside a '\\link' command, missing '\\endlink' command"
                                          );
                                          yyterminate();
                                        }
  /* ----- handle arguments of the relates(also)/addindex commands ----- */

<LineParam>{CMD}{CMD}                   { // escaped command
                                          addOutput(yyscanner,yytext);
                                        }
<LineParam>{DOCNL}                      { // end of argument
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<LineParam>{LC}                         { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<LineParam>({CMD}{CMD}){ID}             { // escaped command
                                          addOutput(yyscanner,yytext);
                                        }
<LineParam>.                            { // ignore other stuff
                                          addOutput(yyscanner,*yytext);
                                        }

  /* ----- handle arguments of the section/subsection/.. commands ------- */

<SectionLabel>{LABELID}                 {
                                          yyextra->sectionLabel+=yytext;
                                        }
<SectionLabel>{CMD}"lineinfo"("{}")?    {
                                          yyextra->sectionLabel += QCString().setNum(yyextra->lineNr);
                                        }
<SectionLabel>{CMD}"fileinfo"("{"[^}]*"}")? {
                                          FileInfo fi(yyextra->fileName.str());
                                          bool hasOption = false;
                                          QCString label;
                                          if (yytext[yyleng-1] == '}') // has parameters
                                          {
                                            StringVector optList;
                                            QCString txt = yytext;
                                            QCString optStr = txt.mid(10,yyleng-11).stripWhiteSpace();
                                            optList = split(optStr.str(),",");
                                            for (const auto &opt_ : optList)
                                            {
                                              QCString optStripped = QCString(opt_).stripWhiteSpace();
                                              std::string opt = optStripped.lower().str();
                                              QCString result = fileInfoLookup(fi,opt);
                                              if (!result.isEmpty())
                                              {
                                                if (hasOption) // oops: already found an option
                                                {
                                                  warn(yyextra->fileName,yyextra->lineNr,"Multiple options specified with \\fileinfo, discarding '{}'", optStripped);
                                                }
                                                else
                                                {
                                                  label = result;
                                                }
                                                hasOption = true;
                                              }
                                              else
                                              {
                                                warn(yyextra->fileName,yyextra->lineNr,"Unknown option specified with \\fileinfo: '{}'", optStripped);
                                              }
                                            }
                                          }
                                          if (!hasOption)
                                          {
                                            if (Config_getBool(FULL_PATH_NAMES))
                                            {
                                              label=stripFromPath(yyextra->fileName);
                                            }
                                            else
                                            {
                                              label=yyextra->fileName;
                                            }
                                          }
                                          escapeLabel(label);
                                          yyextra->sectionLabel+=label;
                                        }
<SectionLabel>{DOCNL}                   {
                                          yyextra->sectionTitle.clear();
                                          if (yyextra->sectionLabel.isEmpty())
                                          { // missing argument
                                            warn(yyextra->fileName,yyextra->lineNr,
                                              "\\section command has no label"
                                              );
                                          }
                                          else
                                          {
                                            yyextra->sectionLabel=yyextra->raisePrefix+yyextra->sectionLabel;
                                            addOutput(yyscanner,yyextra->sectionLabel.data());
                                            addSection(yyscanner);
                                          }
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          BEGIN( Comment );
                                        }
<SectionLabel>.                         { // invalid character for section label
                                          if (yyextra->sectionLabel.isEmpty())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                              "Invalid or missing section label"
                                              );
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionLabel=yyextra->raisePrefix+yyextra->sectionLabel;
                                            addOutput(yyscanner,yyextra->sectionLabel.data());
                                            yyextra->sectionTitle.clear();
                                            unput_string(yytext,yyleng);
                                            BEGIN(SectionTitle);
                                          }
                                        }
<SectionTitle>{STAopt}/"\n"             { // end of section title
                                          addSection(yyscanner);
                                          addOutput(yyscanner,yytext);
                                          BEGIN( Comment );
                                        }
<SectionTitle>{STopt}"\\\\ilinebr"      { // escaped end of section title
                                          yyextra->sectionTitle+=yytext;
                                        }
<SectionTitle>{STopt}/"\\ilinebr"       { // end of section title
                                          addSection(yyscanner);
                                          addOutput(yyscanner,yytext);
                                          BEGIN( Comment );
                                        }
<SectionTitle>{B}*{CMD}"f$"             {
                                          yyextra->formulaText="";
                                          yyextra->formulaPreText="$";
                                          yyextra->formulaPostText="";
                                          yyextra->formulaNewLines=0;
                                          BEGIN(ReadFormulaShortSection);
                                        }
<SectionTitle>{B}*{CMD}"f("             { // start of a inline formula
                                          yyextra->formulaText="";
                                          yyextra->formulaPreText="";
                                          yyextra->formulaPostText="";
                                          yyextra->formulaNewLines=0;
                                          BEGIN(ReadFormulaRoundSection);
                                        }
<SectionTitle>{B}*{CMD}"~"[a-z_A-Z-]*   | // language switch command
<SectionTitle>{B}*{CMD}"f"[\[{]         { // block formula
                                          setOutput(yyscanner,OutputDoc);
                                          addOutput(yyscanner," \\ilinebr ");
                                          addSection(yyscanner,false);
                                          warn(yyextra->fileName,yyextra->lineNr,
                                                "'\\{}' command is not allowed in section title, ending section title.",
                                               QCString(yytext).stripWhiteSpace()
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN(Comment);
                                        }
<SectionTitle>{LC}                      { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<SectionTitle>[^\n@\\]*                 { // any character without special meaning
                                          yyextra->sectionTitle+=yytext;
                                          addOutput(yyscanner,yytext);
                                        }
<SectionTitle>{B}*{CMD}{CMD}            {
                                          yyextra->sectionTitle+=yytext;
                                          addOutput(yyscanner,yytext);
                                        }
<SectionTitle>{B}*{CMD}[a-z_A-Z]+"{"[^}]*"}"{B}*  |
<SectionTitle>{B}*{CMD}[a-z_A-Z]+{B}*   { // handling command in section title
                                          QCString fullMatch = yytext;
                                          int idx = fullMatch.find('{');
                                          /* handle `f{` command as special case */
                                          if ((idx > 1) && (yytext[idx-1] == 'f') && (yytext[idx-2] == '\\' || yytext[idx-2] =='@')) REJECT;
                                          int idxEnd = fullMatch.find("}",idx+1);
                                          QCString cmdName;
                                          StringVector optList;
                                          if (idx == -1) // no options
                                          {
                                            cmdName = fullMatch.stripWhiteSpace().mid(1); // to remove {CMD}
                                          }
                                          else // options present
                                          {
                                            cmdName = fullMatch.left(idx).stripWhiteSpace().mid(1); // to remove {CMD}
                                            QCString optStr = fullMatch.mid(idx+1,idxEnd-idx-1).stripWhiteSpace();
                                            optList = split(optStr.str(),",");
                                          }
                                          auto it = docCmdMap.find(cmdName.str());
                                          if (it!=docCmdMap.end()) // special action is required
                                          {
                                            switch (it->second.sectionHandling)
                                            {
                                              case SectionHandling::Escape:
                                                {
                                                  int i=0;
                                                  while (yytext[i]==' ' || yytext[i]=='\t') i++;
                                                  yyextra->sectionTitle+=fullMatch.left(i);
                                                  yyextra->sectionTitle+='@';
                                                  yyextra->sectionTitle+=fullMatch.mid(i);
                                                  addOutput(yyscanner,qPrint(fullMatch.left(i)));
                                                  addOutput(yyscanner,'@');
                                                  addOutput(yyscanner,qPrint(fullMatch.mid(i)));
                                                  warn(yyextra->fileName,yyextra->lineNr,
                                                    "'\\{}' command is not allowed in section title, escaping command.",cmdName
                                                  );
                                                }
                                                break;
                                              case SectionHandling::Break:
                                                {
                                                  addSection(yyscanner,false);
                                                  addOutput(yyscanner," \\ilinebr ");
                                                  warn(yyextra->fileName,yyextra->lineNr,
                                                    "'\\{}' command is not allowed in section title, ending section title.",cmdName
                                                  );
                                                  unput_string(yytext,yyleng);
                                                  BEGIN(Comment);
                                                }
                                                break;
                                              case SectionHandling::Replace:
                                                {
                                                  if (cmdName == "fileinfo")
                                                  {
                                                    int i=0;
                                                    while (yytext[i]==' ' || yytext[i]=='\t') i++;
                                                    yyextra->sectionTitle+=fullMatch.left(i);
                                                    addOutput(yyscanner,fullMatch.left(i));
                                                    handleFileInfoSection(yyscanner,cmdName,optList);
                                                    if (idxEnd == -1)
                                                    {
                                                      yyextra->sectionTitle+=fullMatch.mid(i+9);
                                                      addOutput(yyscanner,fullMatch.mid(i+9));
                                                    }
                                                    else
                                                    {
                                                      yyextra->sectionTitle+=fullMatch.mid(idxEnd+1);
                                                      addOutput(yyscanner,fullMatch.mid(idxEnd+1));
                                                    }
                                                  }
                                                  else if (cmdName == "lineinfo")
                                                  {
                                                    int i=0;
                                                    while (yytext[i]==' ' || yytext[i]=='\t') i++;
                                                    yyextra->sectionTitle+=fullMatch.left(i);
                                                    yyextra->sectionTitle+=QCString().setNum(yyextra->lineNr);
                                                    yyextra->sectionTitle+=' ';
                                                    yyextra->sectionTitle+=fullMatch.mid(i+9);
                                                    addOutput(yyscanner,fullMatch.left(i));
                                                    addOutput(yyscanner,QCString().setNum(yyextra->lineNr));
                                                    addOutput(yyscanner,' ');
                                                    addOutput(yyscanner,fullMatch.mid(i+9));
                                                  }
                                                  else if (cmdName == "raisewarning")
                                                  {
                                                    yyextra->raiseWarning = "";
                                                    BEGIN(RaiseWarningSection);
                                                  }
                                                  else if (cmdName == "noop")
                                                  {
                                                    addSection(yyscanner,false);
                                                    BEGIN(Noop);
                                                  }
                                                  else if (cmdName == "cite")
                                                  {
                                                    yyextra->sectionTitle+=yytext;
                                                    addOutput(yyscanner,yytext);
                                                    BEGIN(CiteLabelSection);
                                                  }
                                                  else if (cmdName == "iline")
                                                  {
                                                    yyextra->sectionTitle+=yytext;
                                                    addOutput(yyscanner,yytext);
                                                    BEGIN(ILineSection);
                                                  }
                                                  else if (cmdName == "ifile")
                                                  {
                                                    yyextra->sectionTitle+=yytext;
                                                    addOutput(yyscanner,yytext);
                                                    BEGIN(IFileSection);
                                                  }
                                                  else if ((cmdName == "anchor") || (cmdName == "ianchor"))
                                                  {
                                                    addOutput(yyscanner,"@"+cmdName);
                                                    if (optList.empty())
                                                    {
                                                      yyextra -> anchorTitle = "";
                                                    }
                                                    else
                                                    {
                                                      addOutput(yyscanner,"{"+join(optList," ")+"}");
                                                      yyextra -> anchorTitle = join(optList," ");
                                                    }
                                                    addOutput(yyscanner," ");
                                                    BEGIN(AnchorLabelSection);
                                                  }
                                                  else if (cmdName == "link")
                                                  {
                                                    yyextra->sectionTitle+=yytext;
                                                    BEGIN(LinkSection);
                                                  }
                                                  else
                                                  {
                                                    yyextra->sectionTitle+=yytext;
                                                    warn(yyextra->fileName,yyextra->lineNr,
                                                      "internal error '\\{}' command is to be replaced in section title.",cmdName
                                                    );
                                                  }
                                                }
                                                break;
                                              case SectionHandling::Allowed:
                                                {
                                                  yyextra->sectionTitle+=yytext;
                                                  addOutput(yyscanner,yytext);
                                                }
                                                break;
                                            }
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            addOutput(yyscanner,yytext);
                                          }
                                        }
<SectionTitle>.                         { // anything else
                                          yyextra->sectionTitle+=yytext;
                                          addOutput(yyscanner,*yytext);
                                        }

  /* ----- handle arguments of the subpage command ------- */

<SubpageLabel>{FILE}                    { // first argument
                                          addOutput(yyscanner,yytext);
                                          // we add subpage labels as a kind of "inheritance" relation to prevent
                                          // needing to add another list to the Entry class.
                                          yyextra->current->extends.emplace_back(yytext,Protection::Public,Specifier::Normal);
                                          BEGIN(SubpageTitle);
                                        }
<SubpageLabel>{DOCNL}                   { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "\\subpage command has no label"
                                              );
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          BEGIN( Comment );
                                        }
<SubpageLabel>.                         {
                                          unput(yytext[0]);
                                          BEGIN( Comment );
                                        }
<SubpageTitle>{DOCNL}                   { // no title, end command
                                          addOutput(yyscanner,yytext);
                                          BEGIN( Comment );
                                        }
<SubpageTitle>[ \t]*"\""[^\"\n]*"\""    { // add title, end of command
                                          addOutput(yyscanner,yytext);
                                          BEGIN( Comment );
                                        }
<SubpageTitle>.                         { // no title, end of command
                                          unput(*yytext);
                                          BEGIN( Comment );
                                        }

  /* ----- handle arguments of the anchor command ------- */

<AnchorLabel,AnchorLabelSection>{LABELID} { // found argument
                                          QCString lbl = yyextra->raisePrefix+yytext;
                                          addAnchor(yyscanner,lbl, yyextra->anchorTitle);
                                          addOutput(yyscanner,lbl.data());
                                          if (YY_START == AnchorLabel)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            BEGIN(SectionTitle);
                                          }
                                        }
<AnchorLabel,AnchorLabelSection>{DOCNL} { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "\\anchor command has no label"
                                              );
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          if (YY_START == AnchorLabel)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            BEGIN(SectionTitle);
                                          }
                                        }
<AnchorLabel,AnchorLabelSection>.       { // invalid character for anchor label
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Invalid or missing anchor label"
                                              );
                                          addOutput(yyscanner,yytext);
                                          if (YY_START == AnchorLabel)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            BEGIN(SectionTitle);
                                          }
                                        }

  /* ----- handle arguments of the anchor command ------- */

<RequirementLabel>{REQID}               { // found argument
                                          yyextra->current->name = yytext;
                                          yyextra->current->type.clear();
                                          BEGIN( RequirementTitle );
                                        }
<SatisfiesLabel>{REQID}                 { // found argument
                                          yyextra->reqId = yytext;
                                          yyextra->reqTitle.clear();
                                          BEGIN( SatisfiesTitle );
                                        }
<VerifiesLabel>{REQID}                  { // found argument
                                          yyextra->reqId = yytext;
                                          yyextra->reqTitle.clear();
                                          BEGIN( VerifiesTitle );
                                        }
<RequirementLabel,SatisfiesLabel,VerifiesLabel>"\\"{B}*"\n"  { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<RequirementLabel,SatisfiesLabel,VerifiesLabel>{DOCNL}       { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                               "missing requirement ID after \\requirement command"
                                              );
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<RequirementLabel,SatisfiesLabel,VerifiesLabel>.  { // invalid character for requirement label
                                        }

<RequirementTitle,SatisfiesTitle,VerifiesTitle>"\\"{B}*"\n"          { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<RequirementTitle>[^\n\\]+              { // title (stored in type)
                                          yyextra->current->type += yytext;
                                        }
<SatisfiesTitle,VerifiesTitle>[^\n\\@]+ { // title (stored in type)
                                          yyextra->reqTitle += yytext;
                                        }
<RequirementTitle>({CMD}("verifies"|"satisfies"|"requirement"){B}+)|{DOCNL}+              {
                                          yyextra->current->type = yyextra->current->type.stripWhiteSpace();
                                          unput_string(yytext,yyleng);
                                          addRequirementId(yyscanner);
                                          BEGIN( Comment );
                                        }
<SatisfiesTitle>({CMD}("verifies"|"satisfies"|"requirement"){B}+)|{DOCNL}+              {
                                          yyextra->reqTitle = yyextra->reqTitle.stripWhiteSpace();
                                          unput_string(yytext,yyleng);
                                          if (yyextra->current->section.isRequirementDoc())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                 "@satisfies command is not allowed in documentation of a requirement. Put in at the place where the requirement is implemented."
                                                );
                                          }
                                          else
                                          {
                                            yyextra->current->rqli.emplace_back(RequirementRefType::Satisfies,
                                                yyextra->reqId,
                                                yyextra->reqTitle,
                                                yyextra->fileName,
                                                yyextra->lineNr);
                                          }
                                          BEGIN( Comment );
                                        }
<VerifiesTitle>({CMD}("verifies"|"satisfies"|"requirement"){B}+)|{DOCNL}+              {
                                          yyextra->reqTitle = yyextra->reqTitle.stripWhiteSpace();
                                          unput_string(yytext,yyleng);
                                          if (yyextra->current->section.isRequirementDoc())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                 "@verifies command is not allowed in documentation of a requirement. Put in at the place where the requirement is tested."
                                                );
                                          }
                                          else
                                          {
                                            yyextra->current->rqli.emplace_back(RequirementRefType::Verifies,
                                                yyextra->reqId,
                                                yyextra->reqTitle,
                                                yyextra->fileName,
                                                yyextra->lineNr);
                                          }
                                          BEGIN( Comment );
                                        }
<RequirementTitle>.                     { // title (stored in type)
                                          yyextra->current->type += yytext;
                                        }
<SatisfiesTitle,VerifiesTitle>.         { // title (stored in reqTitle)
                                          yyextra->reqTitle += yytext;
                                        }


  /* ----- handle arguments of the preformatted block commands ------- */

<FormatBlock>{CMD}("endverbatim"|"endiverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"endicode"|"endmsc")/{NW} { // possible ends
                                          addOutput(yyscanner,yytext);
                                          if (&yytext[4]==yyextra->blockName) // found end of the block
                                          {
                                              BEGIN(Comment);
                                          }
                                        }
<FormatBlock>{CMD}"enduml"              {
                                          addOutput(yyscanner,yytext);
                                          if (yyextra->blockName=="startuml") // found end of the block
                                          {
                                              BEGIN(Comment);
                                          }
                                        }
<FormatBlock>[^ \@\*\/\\\n]*            { // some word
                                          addOutput(yyscanner,yytext);
                                        }
<FormatBlock>{DOCNL}                    { // new line
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<FormatBlock>{CCS}                       { // start of a C-comment
                                          if (!(yyextra->blockName=="code"  || yyextra->blockName=="verbatim" ||
                                                yyextra->blockName=="icode" || yyextra->blockName=="iverbatim"||
                                                yyextra->blockName=="iliteral"
                                               )
                                             ) yyextra->commentCount++;
                                          addOutput(yyscanner,yytext);
                                        }
<FormatBlock>{CCE}                       { // end of a C-comment
                                          addOutput(yyscanner,yytext);
                                          if (!(yyextra->blockName=="code"  || yyextra->blockName=="verbatim" ||
                                                yyextra->blockName=="icode" || yyextra->blockName=="iverbatim"||
                                                yyextra->blockName=="iliteral"
                                               )
                                             )
                                          {
                                            yyextra->commentCount--;
                                            if (yyextra->commentCount<0)
                                            {
                                              QCString endTag = "end"+yyextra->blockName;
                                              if (yyextra->blockName=="startuml") endTag="enduml";
                                              warn(yyextra->fileName,yyextra->lineNr,
                                                 "found */ without matching /* while inside a \\{} block! Perhaps a missing \\{}?",
                                                 yyextra->blockName,endTag);
                                            }
                                          }
                                        }
<FormatBlock>.                          {
                                          addOutput(yyscanner,*yytext);
                                        }
<FormatBlock><<EOF>>                    {
                                          QCString endTag = "end"+yyextra->blockName;
                                          if (yyextra->blockName=="startuml") endTag="enduml";
                                          warn(yyextra->fileName,yyextra->lineNr,
                                            "reached end of comment while inside a \\{} block; check for missing \\{} tag!",
                                            yyextra->blockName,endTag
                                          );
                                          yyterminate();
                                        }

  /* ----- handle arguments of if/ifnot commands ------- */

<GuardParam>{B}*"("                     {
                                          yyextra->guardExpr=yytext;
                                          yyextra->roundCount=1;
                                          BEGIN(GuardExpr);
                                        }
<GuardExpr>[^()]*                       {
                                          yyextra->guardExpr+=yytext;
                                          lineCount(yyscanner);
                                        }
<GuardExpr>"("                          {
                                          yyextra->guardExpr+=yytext;
                                          yyextra->roundCount++;
                                        }
<GuardExpr>")"                          {
                                          yyextra->guardExpr+=yytext;
                                          yyextra->roundCount--;
                                          if (yyextra->roundCount==0)
                                          {
                                            handleGuard(yyscanner,yyextra->guardExpr);
                                          }
                                        }
<GuardExpr>\n                           {
                                          warn(yyextra->fileName,yyextra->lineNr,
                                                "invalid expression '{}' for yyextra->guards",yyextra->guardExpr);
                                          unput(*yytext);
                                          BEGIN(GuardParam);
                                        }
<GuardParam>{B}*[a-z_A-Z0-9.\-]+        { // parameter of if/ifnot yyextra->guards
                                          handleGuard(yyscanner,yytext);
                                        }
<GuardParam>{DOCNL}                     { // end of argument
                                          //next line is commented out due to bug620924
                                          //addOutput(yyscanner,'\n');
                                          addIlineBreak(yyscanner,yyextra->lineNr);
                                          unput_string(yytext,yyleng);
                                          handleGuard(yyscanner,QCString());
                                        }
<GuardParam>{LC}                        { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<GuardParam>.                           { // empty condition
                                          unput(*yytext);
                                          handleGuard(yyscanner,QCString());
                                        }
<GuardParamEnd>{B}*{DOCNL}              {
                                          lineCount(yyscanner);
                                          yyextra->spaceBeforeIf.clear();
                                          addIlineBreak(yyscanner,yyextra->lineNr);
                                          BEGIN(Comment);
                                        }
<GuardParamEnd>{B}*                     {
                                          if (!yyextra->spaceBeforeIf.isEmpty()) // needed for 665313 in combination with bug620924
                                          {
                                            addOutput(yyscanner,yyextra->spaceBeforeIf);
                                          }
                                          yyextra->spaceBeforeIf.clear();
                                          addIlineBreak(yyscanner,yyextra->lineNr);
                                          BEGIN(Comment);
                                        }
<GuardParamEnd>.                        {
                                          unput(*yytext);
                                          addIlineBreak(yyscanner,yyextra->lineNr);
                                          BEGIN(Comment);
                                        }

  /* ----- handle skipping of conditional sections ------- */

<SkipGuardedSection>{CMD}"ifnot"/{NW}   {
                                          yyextra->guardType = Guard_IfNot;
                                          yyextra->guards->emplace(false);
                                          BEGIN( GuardParam );
                                        }
<SkipGuardedSection>{CMD}"if"/{NW}      {
                                          yyextra->guardType = Guard_If;
                                          yyextra->guards->emplace(false);
                                          BEGIN( GuardParam );
                                        }
<SkipGuardedSection>{CMD}"endif"/{NW}   {
                                          if (yyextra->guards->empty())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                "found \\endif without matching start command");
                                            BEGIN( Comment );
                                          }
                                          else
                                          {
                                            yyextra->guards->pop();
                                            if (yyextra->guards->empty())
                                            {
                                              BEGIN( GuardParamEnd );
                                            }
                                            else
                                            {
                                              if (yyextra->guards->top().isEnabled())
                                              {
                                                BEGIN( GuardParamEnd );
                                              }
                                              else
                                              {
                                                BEGIN( SkipGuardedSection );
                                              }
                                            }
                                          }
                                        }
<SkipGuardedSection>{CMD}"else"/{NW}    {
                                          if (yyextra->guards->empty())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                "found \\else without matching start command");
                                          }
                                          else if (yyextra->guards->top().hasElse())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                "found multiple \\else commands in same \\if construct");
                                            yyextra->guards->top().setEnabled(false);
                                            BEGIN( SkipGuardedSection );
                                          }
                                          else if (!yyextra->guards->top().parentVisible())
                                          {
                                            yyextra->guards->top().setEnabled(false);
                                            BEGIN( SkipGuardedSection );
                                          }
                                          else
                                          {
                                            yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
                                            yyextra->guards->top().setElse();
                                            if (!yyextra->guards->top().parentVisible())
                                            {
                                              yyextra->guards->top().setEnabled(false);
                                              BEGIN( SkipGuardedSection );
                                            }
                                            else if (yyextra->guards->top().isEnabledFound())
                                            {
                                              yyextra->guards->top().setEnabled(false);
                                              BEGIN( SkipGuardedSection );
                                            }
                                            else
                                            {
                                              yyextra->guards->top().setEnabled(true);
                                              BEGIN( GuardParamEnd );
                                            }
                                          }
                                        }
<SkipGuardedSection>{CMD}"elseif"/{NW}  {
                                          if (yyextra->guards->empty())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                "found \\elseif without matching start command");
                                          }
                                          else if (yyextra->guards->top().hasElse())
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,
                                                "found \\elseif command after \\else command was given in \\if construct");
                                            yyextra->guardType = Guard_ElseIf;
                                            yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
                                            yyextra->guards->top().setEnabled(false);
                                            BEGIN( GuardParam );
                                          }
                                          else
                                          {
                                            yyextra->guardType = Guard_ElseIf;
                                            yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
                                            yyextra->guards->top().setEnabled(false);
                                            BEGIN( GuardParam );
                                          }
                                        }
<SkipGuardedSection>{DOCNL}             { // skip line
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                        }
<SkipGuardedSection>[^ \\@\n]+          { // skip non-special characters
                                        }
<SkipGuardedSection>{CMD}{CMD}          | 
<SkipGuardedSection>.                   { // any other character
                                        }


  /* ----- handle skipping of internal section ------- */

<SkipInternal>{DOCNL}                   { // skip line
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<SkipInternal>{CMD}"if"/[ \t]           {
                                          yyextra->condCount++;
                                          }
<SkipInternal>{CMD}"ifnot"/[ \t]        {
                                          yyextra->condCount++;
                                        }
<SkipInternal>{CMD}/"endif"             {
                                          yyextra->condCount--;
                                          if (yyextra->condCount<0) // handle conditional section around of \internal, see bug607743
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}/"section"[ \t]      {
                                          if (yyextra->sectionLevel>0)
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}/"subsection"[ \t]   {
                                          if (yyextra->sectionLevel>1)
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}/"subsubsection"[ \t] {
                                          if (yyextra->sectionLevel>2)
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}/"paragraph"[ \t]    {
                                          if (yyextra->sectionLevel>3)
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}/"subparagraph"[ \t] {
                                          if (yyextra->sectionLevel>4)
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}/"subsubparagraph"[ \t] {
                                          if (yyextra->sectionLevel>5)
                                          {
                                            unput('\\');
                                            BEGIN(Comment);
                                          }
                                        }
<SkipInternal>{CMD}"endinternal"[ \t]*  {
                                          BEGIN(Comment);
                                        }
<SkipInternal>[^ \\@\n]+                { // skip non-special characters
                                        }
<SkipInternal>.                         { // any other character
                                        }


  /* ----- handle argument of name command ------- */

<NameParam>{DOCNL}                      { // end of argument
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<NameParam>{LC}                         { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          yyextra->docGroup.appendHeader(' ');
                                        }
<NameParam>.                            { // ignore other stuff
                                          yyextra->docGroup.appendHeader(*yytext);
                                          yyextra->current->name+=*yytext;
                                        }

  /* ----- handle argument of noop command ------- */
<Noop>{DOCNL}                           { // end of argument
                                          if (*yytext=='\n')
                                          {
                                            yyextra->lineNr++;
                                            addOutput(yyscanner,'\n');
                                          }
                                          BEGIN( Comment );
                                        }
<Noop>.                                 { // ignore other stuff
                                        }
  /* ----- handle argument of raisewarning command ------- */
<RaiseWarning,RaiseWarningSection>{DOCNL} { // end of argument
                                          warn_doc_error(yyextra->fileName,yyextra->lineNr,
                                                         "{}",yyextra->raiseWarning);
                                          yyextra->raiseWarning = "";
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          if (YY_START == RaiseWarning)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            BEGIN(SectionTitle);
                                          }
                                        }
<RaiseWarning,RaiseWarningSection>.     { // ignore other stuff
                                          yyextra->raiseWarning += yytext;
                                        }
  /* ----- handle argument of ingroup command ------- */

<InGroupParam>{LABELID}                 { // group id
                                          yyextra->current->groups.emplace_back(
                                             yytext, Grouping::GROUPING_INGROUP
                                          );
                                          yyextra->inGroupParamFound=TRUE;
                                        }
<InGroupParam>{DOCNL}                   { // missing argument
                                          if (!yyextra->inGroupParamFound)
                                          {
                                             warn(yyextra->fileName,yyextra->lineNr,
                                                "Missing group name for \\ingroup command"
                                                );
                                          }
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<InGroupParam>{LC}                      { // line continuation
                                          yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                        }
<InGroupParam>.                         { // ignore other stuff
                                          addOutput(yyscanner,*yytext);
                                        }

  /* ----- handle argument of fn command ------- */

<FnParam>{DOCNL}                        { // end of argument
                                          if (yyextra->braceCount==0)
                                          {
                                            if (yyextra->functionProto.stripWhiteSpace().isEmpty())
                                            {
                                              warn(yyextra->fileName,yyextra->lineNr,
                                                   "missing argument after '\\{}'.",yyextra->currentCmd
                                                  );
                                            }
                                            else
                                            {
                                              makeStructuralIndicator(yyscanner,yyextra->currentMakeEntryType);
                                              yyextra->langParser->parsePrototype(yyextra->functionProto);
                                            }
                                            unput_string(yytext,yyleng);
                                            BEGIN( Comment );
                                          }
                                        }
<FnParam>{LC}                           { // line continuation
                                          yyextra->lineNr++;
                                          yyextra->functionProto+=' ';
                                        }
<FnParam>[^@\\\n()]+                    { // non-special characters
                                          yyextra->functionProto+=yytext;
                                        }
<FnParam>"("                            {
                                          yyextra->functionProto+=yytext;
                                          yyextra->braceCount++;
                                        }
<FnParam>")"                            {
                                          yyextra->functionProto+=yytext;
                                          yyextra->braceCount--;
                                        }
<FnParam>.                              { // add other stuff
                                          yyextra->functionProto+=*yytext;
                                        }


  /* ----- handle argument of overload command ------- */


<OverloadParam>{DOCNL}                  { // end of argument
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          if (yyextra->functionProto.stripWhiteSpace().isEmpty())
                                          { // plain overload command
                                            addOutput(yyscanner,getOverloadDocs());
                                            addOutput(yyscanner,'\n');
                                          }
                                          else // overload declaration
                                          {
                                            makeStructuralIndicator(yyscanner,EntryType::makeOverloadDoc);
                                            yyextra->langParser->parsePrototype(yyextra->functionProto);
                                          }
                                          BEGIN( Comment );
                                        }
<OverloadParam>{LC}                     { // line continuation
                                          yyextra->lineNr++;
                                          yyextra->functionProto+=' ';
                                        }
<OverloadParam>.                        { // add other stuff
                                          yyextra->functionProto+=*yytext;
                                        }

  /* ----- handle argument of inherit command ------- */

<InheritParam>({ID}("::"|"."))*{ID}     { // found argument
                                          yyextra->current->extends.emplace_back(
                                            removeRedundantWhiteSpace(yytext),Protection::Public,Specifier::Normal
                                          );
                                          BEGIN( Comment );
                                        }
<InheritParam>{DOCNL}                   { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "\\inherit command has no argument"
                                              );
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          addOutput(yyscanner,'\n');
                                          BEGIN( Comment );
                                        }
<InheritParam>.                         { // invalid character for anchor label
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "Invalid or missing name for \\inherit command"
                                              );
                                          BEGIN(Comment);
                                        }

  /* ----- handle argument of extends and implements commands ------- */

<ExtendsParam>({ID}("::"|"."))*{ID}     { // found argument
                                          yyextra->current->extends.emplace_back(
                                            removeRedundantWhiteSpace(yytext),Protection::Public,Specifier::Normal
                                          );
                                          BEGIN( Comment );
                                        }
<ExtendsParam>{DOCNL}                   { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "'\\{}' command has no argument",yyextra->currentCmd
                                              );
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          unput_string(yytext,yyleng);
                                          BEGIN( Comment );
                                        }
<ExtendsParam>.                         { // ignore other stuff
                                        }

  /* ----- handle language specific sections ------- */

<SkipLang>{CMD}"~"[a-zA-Z-]*            { /* language switch */
                                          QCString langId(&yytext[2]);
                                          if (!langId.isEmpty() && !Config_isAvailableEnum(OUTPUT_LANGUAGE,langId))
                                          {
                                             warn(yyextra->fileName,yyextra->lineNr,
                                             "non supported language '{}' specified in '{}'",langId,QCString(yytext).stripWhiteSpace());
                                          }
                                          else if (langId.isEmpty() ||
                                              qstricmp(Config_getEnumAsString(OUTPUT_LANGUAGE),langId)==0)
                                          { // enable language specific section
                                            BEGIN(Comment);
                                          }
                                        }
<SkipLang>[^*@\\\n]*                    { /* any character not a *, @, backslash or new line */
                                        }
<SkipLang>{DOCNL}                       { /* new line in verbatim block */
                                          if (*yytext=='\n') yyextra->lineNr++;
                                        }
<SkipLang>.                             { /* any other character */
                                        }

  /* ----- handle arguments of the cite command ------- */

<CiteLabel,CiteLabelSection>{CITEID}    { // found argument
                                          addCite(yyscanner);
                                          addOutput(yyscanner,yytext);
                                          if (YY_START == CiteLabel)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            BEGIN(SectionTitle);
                                          }
                                        }
<CiteLabel,CiteLabelSection>{DOCNL}     { // missing argument
                                          warn(yyextra->fileName,yyextra->lineNr,
                                              "\\cite command has no label"
                                              );
                                          //if (*yytext=='\n') yyextra->lineNr++;
                                          //addOutput(yyscanner,'\n');
                                          if (YY_START == CiteLabel)
                                          {
                                            unput_string(yytext,yyleng);
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            unput_string(yytext,yyleng);
                                            BEGIN(SectionTitle);
                                          }
                                        }
<CiteLabel,CiteLabelSection>.           { // invalid character for cite label
                                           warn(yyextra->fileName,yyextra->lineNr,
                                              "Invalid or missing cite label"
                                              );
                                          if (YY_START == CiteLabel)
                                          {
                                            BEGIN(Comment);
                                          }
                                          else
                                          {
                                            yyextra->sectionTitle+=yytext;
                                            BEGIN(SectionTitle);
                                          }
                                        }

  /* ----- handle argument of the copydoc command ------- */

<CopyDoc><<EOF>>                        {
                                          setOutput(yyscanner,OutputDoc);
                                          addOutput(yyscanner," \\ilinebr\\ilinebr\\copydetails ");
                                          addOutput(yyscanner,yyextra->copyDocArg);
                                          addOutput(yyscanner,"\n");
                                          BEGIN(Comment);
                                        }
<CopyDoc>"<"[/]?{TABLEDEL}">"           {
                                          if (yyextra->braceCount==0)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                            addOutput(yyscanner," \\ilinebr\\ilinebr\\copydetails ");
                                            addOutput(yyscanner,yyextra->copyDocArg);
                                            addOutput(yyscanner,yytext);
                                            BEGIN(Comment);
                                          }
                                        }
<CopyDoc>{DOCNL}                        {
                                          if (*yytext=='\n') yyextra->lineNr++;
                                          if (yyextra->braceCount==0)
                                          {
                                            setOutput(yyscanner,OutputDoc);
                                            addOutput(yyscanner," \\ilinebr\\ilinebr\\copydetails ");
                                            addOutput(yyscanner,yyextra->copyDocArg);
                                            addOutput(yyscanner,"\n");
                                            BEGIN(Comment);
                                          }
                                        }
<CopyDoc>{LC}                           { // line continuation
                                          yyextra->lineNr++;
                                        }
<CopyDoc>[^@\\\n()<]+                   { // non-special characters
                                          yyextra->copyDocArg+=yytext;
                                          addOutput(yyscanner,yytext);
                                        }
<CopyDoc>"("                            {
                                          yyextra->copyDocArg+=yytext;
                                          addOutput(yyscanner,yytext);
                                          yyextra->braceCount++;
                                        }
<CopyDoc>")"                            {
                                          yyextra->copyDocArg+=yytext;
                                          addOutput(yyscanner,yytext);
                                          yyextra->braceCount--;
                                        }
<CopyDoc>.                              {
                                          yyextra->copyDocArg+=yytext;
                                          addOutput(yyscanner,yytext);
                                        }

 /*
<*>.  { fprintf(stderr,"Lex scanner %s %sdefault rule for state %s: #%s#\n", __FILE__,!yyextra->fileName.isEmpty() ? ("(" + yyextra->fileName +") ").data(): "",stateToString(YY_START),yytext);}
<*>\n  { fprintf(stderr,"Lex scanner %s %sdefault rule newline for state %s.\n", __FILE__, !yyextra->fileName.isEmpty() ? ("(" + yyextra->fileName +") ").data(): "",stateToString(YY_START));}
 */

%%

//----------------------------------------------------------------------------

static bool handleBrief(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  //printf("handleBrief\n");
  setOutput(yyscanner,OutputBrief);
  return FALSE;
}

static bool handleFn(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->braceCount=0;
  yyextra->functionProto.clear();
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeMemberDoc;
  BEGIN( FnParam );
  return checkStructuralIndicator(yyscanner);
}

static bool handleDef(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->functionProto.clear();
  yyextra->braceCount=0;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeDefineDoc;
  BEGIN( FnParam );
  return checkStructuralIndicator(yyscanner);
}

static bool handleOverload(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->functionProto.clear();
  BEGIN(OverloadParam);
  return FALSE;
}

static bool handleEnum(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentMakeEntryType = EntryType::makeEnumDoc;
  BEGIN( EnumDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleDefGroup(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeGroupDoc);
  yyextra->current->groupDocType = Entry::GROUPDOC_NORMAL;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleAddToGroup(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeGroupDoc);
  yyextra->current->groupDocType = Entry::GROUPDOC_ADD;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleWeakGroup(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeGroupDoc);
  yyextra->current->groupDocType = Entry::GROUPDOC_WEAK;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleNamespace(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentMakeEntryType = EntryType::makeNamespaceDoc;
  BEGIN( NameSpaceDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handlePackage(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makePackageDoc);
  BEGIN( PackageDocArg1 );
  return stop;
}

static bool handleClass(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeClassDoc;
  BEGIN( ClassDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleConcept(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeConceptDoc;
  BEGIN( ConceptDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleModule(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeModuleDoc;
  BEGIN( ModuleDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleHeaderFile(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN( ClassDocArg2 );
  return FALSE;
}

static bool handleProtocol(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{ // Obj-C protocol
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeProtocolDoc;
  BEGIN( ClassDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleCategory(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{ // Obj-C category
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeCategoryDoc;
  BEGIN( CategoryDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleUnion(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeUnionDoc;
  BEGIN( ClassDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleStruct(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeStructDoc;
  BEGIN( ClassDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleInterface(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeInterfaceDoc;
  BEGIN( ClassDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handleIdlException(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  yyextra->currentMakeEntryType = EntryType::makeExceptionDoc;
  BEGIN( ClassDocArg1 );
  return checkStructuralIndicator(yyscanner);
}

static bool handlePage(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makePageDoc);
  BEGIN( PageDocArg1 );
  return stop;
}

static bool handleMainpage(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeMainpageDoc);
  if (!stop)
  {
    yyextra->current->name = "mainpage";
  }
  setOutput(yyscanner,OutputDoc);
  BEGIN( PageDocArg2 );
  return stop;
}

static bool handleFile(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeFileDoc);
  if (!stop)
  {
    yyextra->current->name = yyextra->fileName;
  }
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleParam(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // we need process param and retval arguments to escape leading underscores in case of
  // markdown processing, see bug775493
  addOutput(yyscanner,"@param ");
  BEGIN( ParamArg1 );
  return FALSE;
}

static bool handleRetval(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,"@retval ");
  BEGIN( ParamArg1 );
  return FALSE;
}

static bool handleDir(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeDirDoc);
  if (!stop) yyextra->current->name = yyextra->fileName;
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleExample(yyscan_t yyscanner,const QCString &cmd, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  MakeEntryType sectionMaker = EntryType::makeExample;
  for (const auto &opt : optList)
  {
    if (opt=="lineno")
    {
      sectionMaker=EntryType::makeExampleLineno;
    }
    else
    {
      warn(yyextra->fileName,yyextra->lineNr,
          "unsupported option '{}' for command '\\{}'",opt,cmd);
    }
  }
  bool stop=makeStructuralIndicator(yyscanner,sectionMaker);
  if (!stop) yyextra->current->name = yyextra->fileName;
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleDetails(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->inContext!=OutputBrief)
  {
    addOutput(yyscanner," \\ilinebr\\ilinebr "); // treat @details outside brief description
                                                // as a new paragraph
  }
  setOutput(yyscanner,OutputDoc);
  return FALSE;
}

static bool handleRaiseWarning(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->raiseWarning = "";
  BEGIN( RaiseWarning );
  return FALSE;
}

static bool handleNoop(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN( Noop );
  return FALSE;
}

static bool handleName(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeMemberGrp);
  if (!stop)
  {
    yyextra->docGroup.clearHeader();
    BEGIN( NameParam );
    if (!yyextra->docGroup.isEmpty()) // end of previous member group
    {
      yyextra->docGroup.close(yyextra->current,yyextra->fileName,yyextra->lineNr,TRUE,true);
    }
  }
  return stop;
}

static bool handleTodo(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->newXRefKind = XRef_Todo;
  setOutput(yyscanner,OutputXRef);
  yyextra->xrefKind = XRef_Todo;
  return FALSE;
}

static bool handleTest(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->newXRefKind = XRef_Test;
  setOutput(yyscanner,OutputXRef);
  yyextra->xrefKind = XRef_Test;
  return FALSE;
}

static bool handleBug(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->newXRefKind = XRef_Bug;
  setOutput(yyscanner,OutputXRef);
  yyextra->xrefKind = XRef_Bug;
  return FALSE;
}

static bool handleDeprecated(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->newXRefKind = XRef_Deprecated;
  setOutput(yyscanner,OutputXRef);
  yyextra->xrefKind = XRef_Deprecated;
  return FALSE;
}

static bool handleXRefItem(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->newXRefKind = XRef_Item;
  BEGIN(XRefItemParam1);
  return FALSE;
}

static bool handleParBlock(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->insideParBlock)
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\parblock command while already in a parblock!");
  }
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  addOutput(yyscanner,"@parblock ");
  yyextra->insideParBlock = TRUE;
  return FALSE;
}

static bool handleEndParBlock(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->insideParBlock)
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\endparblock command without matching \\parblock!");
  }
  addOutput(yyscanner,"@endparblock");
  setOutput(yyscanner,OutputDoc); // to end a parblock inside a xrefitem like context
  yyextra->insideParBlock = FALSE;
  return FALSE;
}

static bool handleRelated(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->current->relates.isEmpty())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found multiple \\relates, \\relatesalso or \\memberof commands in a comment block, using last definition");
  }
  yyextra->current->relatesType = RelatesType::Simple;
  yyextra->currentCmd = cmd;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleRelatedAlso(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->current->relates.isEmpty())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found multiple \\relates, \\relatesalso or \\memberof commands in a comment block, using last definition");
  }
  yyextra->current->relatesType = RelatesType::Duplicate;
  yyextra->currentCmd = cmd;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleMemberOf(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->current->relates.isEmpty())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found multiple \\relates, \\relatesalso or \\memberof commands in a comment block, using last definition");
  }
  yyextra->current->relatesType = RelatesType::MemberOf;
  yyextra->currentCmd = cmd;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleRefItem(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,"@refitem ");
  BEGIN(LineParam);
  return FALSE;
}

static bool handleSection(yyscan_t yyscanner,const QCString &s, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  setOutput(yyscanner,OutputDoc);
  //printf("handleSection(%s) raiseLevel=%d\n",qPrint(s),yyextra->raiseLevel);
  BEGIN(SectionLabel);
  yyextra->sectionLabel.clear();
  // determine natural section level
  if      (s=="section")         yyextra->sectionLevel=SectionType::Section;
  else if (s=="subsection")      yyextra->sectionLevel=SectionType::Subsection;
  else if (s=="subsubsection")   yyextra->sectionLevel=SectionType::Subsubsection;
  else if (s=="paragraph")       yyextra->sectionLevel=SectionType::Paragraph;
  else if (s=="subparagraph")    yyextra->sectionLevel=SectionType::Subparagraph;
  else if (s=="subsubparagraph") yyextra->sectionLevel=SectionType::Subsubparagraph;
  // raise it if requested
  yyextra->sectionLevel = std::min(yyextra->sectionLevel + yyextra->raiseLevel,SectionType::MaxLevel);
  // rewrite the update section level to the output
  switch (yyextra->sectionLevel)
  {
    case SectionType::Section:         addOutput(yyscanner,"@section ");         break;
    case SectionType::Subsection:      addOutput(yyscanner,"@subsection ");      break;
    case SectionType::Subsubsection:   addOutput(yyscanner,"@subsubsection ");   break;
    case SectionType::Paragraph:       addOutput(yyscanner,"@paragraph ");       break;
    case SectionType::Subparagraph:    addOutput(yyscanner,"@subparagraph ");    break;
    case SectionType::Subsubparagraph: addOutput(yyscanner,"@subsubparagraph "); break;
    default: addOutput(yyscanner,"@"+s+" ");           break;
  }
  return FALSE;
}

static bool handleRequirement(yyscan_t yyscanner,const QCString &s,const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool stop=makeStructuralIndicator(yyscanner,EntryType::makeRequirementDoc);
  BEGIN( RequirementLabel );
  return stop;
}

static bool handleSatisfies(yyscan_t yyscanner,const QCString &,const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN( SatisfiesLabel );
  return false;
}

static bool handleVerifies(yyscan_t yyscanner,const QCString &,const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN( VerifiesLabel );
  return false;
}

static bool handleSubpage(yyscan_t yyscanner,const QCString &s, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->current->section.isEmpty() &&
      !yyextra->current->section.isPageDoc() &&
      !yyextra->current->section.isMainpageDoc()
     )
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\subpage command in a comment block that is not marked as a page!");
  }
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  addOutput(yyscanner,"@"+s+" ");
  BEGIN(SubpageLabel);
  return FALSE;
}

static bool handleAnchor(yyscan_t yyscanner,const QCString &s, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,"@"+s+" ");
  if (optList.empty())
  {
    yyextra -> anchorTitle = "";
  }
  else
  {
    yyextra -> anchorTitle = join(optList," ");
  }
  BEGIN(AnchorLabel);
  return FALSE;
}

static bool handleImage(yyscan_t yyscanner,const QCString &s, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (const auto &opt : optList)
  {
    QCString locOpt(opt);
    locOpt = locOpt.stripWhiteSpace();
    if (locOpt.lower().startsWith("anchor:"))
    {
      addAnchor(yyscanner,locOpt.mid(7));
      break; // real option handling will be done later on
    }
  }
  if (optList.empty())
  {
    addOutput(yyscanner,"@"+s+" ");
  }
  else
  {
    addOutput(yyscanner,"@"+s+"{"+QCString(join(optList,","))+"} ");
  }
  BEGIN(Comment);
  return FALSE;
}

static bool handleCite(yyscan_t yyscanner,const QCString &s, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  if (optList.empty())
  {
    addOutput(yyscanner,"@"+s+" ");
  }
  else
  {
    addOutput(yyscanner,"@"+s+"{"+QCString(join(optList,","))+"} ");
  }
  BEGIN(CiteLabel);
  return FALSE;
}

static bool handleFormatBlock(yyscan_t yyscanner,const QCString &s, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  if (optList.empty())
  {
    addOutput(yyscanner,"@"+s+" ");
  }
  else
  {
    addOutput(yyscanner,"@"+s+"{"+QCString(join(optList,","))+"} ");
  }
  //printf("handleFormatBlock(%s) with option(%s)\n",qPrint(s),qPrint(opt));
  yyextra->blockName=s;
  yyextra->commentCount=0;
  BEGIN(FormatBlock);
  return FALSE;
}

static bool handleAddIndex(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,yytext);
  BEGIN(LineParam);
  return FALSE;
}

static bool handleFileInfo(yyscan_t yyscanner,const QCString &cmdName, const StringVector &optList)
{
  return handleFileInfoResult(yyscanner,cmdName, optList, false);
}
static bool handleFileInfoSection(yyscan_t yyscanner,const QCString &cmdName, const StringVector &optList)
{
  return handleFileInfoResult(yyscanner,cmdName, optList, true);
}

static QCString fileInfoLookup(const FileInfo &fi,const std::string &optionName)
{
  using OptionFunc = std::function<QCString(const FileInfo &)>;
  static std::unordered_map<std::string,OptionFunc> options =
  {
    // name,       function producing the value
    { "name",      [](const FileInfo &fi_) -> QCString { return fi_.baseName();      } },
    { "extension", [](const FileInfo &fi_) -> QCString { return fi_.extension(true); } },
    { "filename",  [](const FileInfo &fi_) -> QCString { return fi_.fileName();      } },
    { "directory", [](const FileInfo &fi_) -> QCString { return fi_.dirPath();       } },
    { "full",      [](const FileInfo &fi_) -> QCString { return fi_.absFilePath();   } }
  };
  auto it = options.find(optionName);
  return (it!=options.end()) ? it->second(fi) : QCString();
}

static bool handleFileInfoResult(yyscan_t yyscanner,const QCString &, const StringVector &optList, bool isSection)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    if (isSection) yyextra->sectionTitle+=yyextra->spaceBeforeCmd;
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  bool first = true;
  FileInfo fi(yyextra->fileName.str());
  for (const auto &opt_ : optList)
  {
    QCString optStripped = QCString(opt_).stripWhiteSpace();
    std::string opt = optStripped.lower().str();
    QCString result = fileInfoLookup(fi,opt);
    if (!result.isEmpty())
    {
      if (!first)
      {
        warn(yyextra->fileName,yyextra->lineNr,"Multiple options specified with \\fileinfo, discarding '{}'", optStripped);
      }
      else
      {
        addOutput(yyscanner,result);
        if (isSection)
        {
          yyextra->sectionTitle+=result;
        }
      }
      first = false;
    }
    else
    {
      warn(yyextra->fileName,yyextra->lineNr,"Unknown option specified with \\fileinfo: '{}'", optStripped);
    }
  }
  if (first) // no options specified
  {
    if (Config_getBool(FULL_PATH_NAMES))
    {
      if (isSection) yyextra->sectionTitle+=stripFromPath(yyextra->fileName);
      addOutput(yyscanner,stripFromPath(yyextra->fileName));
    }
    else
    {
      if (isSection) yyextra->sectionTitle+=yyextra->fileName;
      addOutput(yyscanner,yyextra->fileName);
    }
  }
  return false;
}

static bool handleLineInfo(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  addOutput(yyscanner,QCString().setNum(yyextra->lineNr));
  return FALSE;
}

static bool handleILine(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,yytext);
  BEGIN(ILine);
  return FALSE;
}

static bool handleIFile(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,yytext);
  BEGIN(IFile);
  return FALSE;
}

static bool handleIRaise(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(IRaise);
  return FALSE;
}

static bool handleIPrefix(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  addOutput(yyscanner,"@iprefix ");
  BEGIN(IRaisePrefix);
  return FALSE;
}

static bool handleIf(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->guardType = Guard_If;
  yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
  if (yyextra->guards->empty())
  {
    yyextra->guards->emplace(true);
  }
  else
  {
    bool enabled  = yyextra->guards->top().isEnabled();
    yyextra->guards->emplace(enabled);
  }
  BEGIN(GuardParam);
  return FALSE;
}

static bool handleIfNot(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->guardType = Guard_IfNot;
  yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
  if (yyextra->guards->empty())
  {
    yyextra->guards->emplace(true);
  }
  else
  {
    bool enabled  = yyextra->guards->top().isEnabled();
    yyextra->guards->emplace(enabled);
  }
  BEGIN(GuardParam);
  return FALSE;
}

static bool handleElseIf(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->guards->empty())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\elseif without matching start command");
  }
  else if (yyextra->guards->top().hasElse())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\elseif command after \\else command was given in \\if construct");
    yyextra->guardType = Guard_ElseIf;
    yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
    yyextra->guards->top().setEnabled(false);
    BEGIN(GuardParam);
  }
  else
  {
    yyextra->guardType = Guard_ElseIf;
    yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
    yyextra->guards->top().setEnabled(false);
    BEGIN(GuardParam);
  }
  return FALSE;
}

static bool handleElse(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->guards->empty())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\else without matching start command");
  }
  else if (yyextra->guards->top().hasElse())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found multiple \\else commands in same \\if construct");
    yyextra->guards->top().setEnabled(false);
    yyextra->guards->top().setElse();
    BEGIN( SkipGuardedSection );
  }
  else
  {
    yyextra->guards->top().setElse();
    yyextra->spaceBeforeIf = yyextra->spaceBeforeCmd;
    if (yyextra->guards->top().isEnabledFound())
    {
      yyextra->guards->top().setEnabled(false);
      BEGIN( SkipGuardedSection );
    }
    else
    {
      yyextra->guards->top().setEnabled(true);
      BEGIN( GuardParamEnd );
    }
  }
  return FALSE;
}

static bool handleEndIf(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->guards->empty())
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "found \\endif without matching start command");
  }
  else
  {
    yyextra->guards->pop();
  }
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  if (yyextra->guards->empty())
  {
    BEGIN( GuardParamEnd );
  }
  else
  {
    if (yyextra->guards->top().isEnabled())
    {
      BEGIN( GuardParamEnd );
    }
    else
    {
      BEGIN( SkipGuardedSection );
    }
  }
  return FALSE;
}

static bool handleIngroup(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->inGroupParamFound=FALSE;
  BEGIN( InGroupParam );
  return FALSE;
}

static bool handleNoSubGrouping(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->subGrouping = FALSE;
  return FALSE;
}

static bool handleShowInitializer(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->initLines = 100000; // ON
  return FALSE;
}

static bool handleHideInitializer(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->initLines = 0; // OFF
  return FALSE;
}

static bool handleCallgraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_callGraph(true); // ON
  return FALSE;
}

static bool handleHideCallgraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_callGraph(false); // OFF
  return FALSE;
}

static bool handleCallergraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_callerGraph(true); // ON
  return FALSE;
}

static bool handleHideCallergraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_callerGraph(false); // OFF
  return FALSE;
}

static bool handleShowEnumValues(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_enumValues(true); // ON
  return FALSE;
}

static bool handleHideEnumValues(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_enumValues(false); // OFF
  return FALSE;
}

static bool handleShowInlineSource(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_inlineSource(true); // ON
  return FALSE;
}

static bool handleHideInlineSource(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_inlineSource(false); // OFF
  return FALSE;
}

static bool handleIncludegraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_includeGraph(true); // ON
  return FALSE;
}

static bool handleIncludedBygraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_includedByGraph(true); // ON
  return FALSE;
}

static bool handleHideIncludegraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_includeGraph(false); // OFF
  return FALSE;
}

static bool handleHideIncludedBygraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_includedByGraph(false); // OFF
  return FALSE;
}

static bool handleDirectoryGraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_directoryGraph(true); // ON
  return FALSE;
}

static bool handleHideDirectoryGraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_directoryGraph(false); // OFF
  return FALSE;
}

static bool handleCollaborationgraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_collaborationGraph(true); // ON
  return FALSE;
}

static bool handleHideCollaborationgraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_collaborationGraph(false); // OFF
  return FALSE;
}

static bool handleGroupgraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_groupGraph(true); // ON
  return FALSE;
}

static bool handleHideGroupgraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_groupGraph(false); // OFF
  return FALSE;
}

static bool handleInheritanceGraph(yyscan_t yyscanner,const QCString &, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::YES);
  for (const auto &opt_ : optList)
  {
    QCString opt = QCString(opt_).stripWhiteSpace().lower();
    if (!opt.isEmpty())
    {
      if (opt == "yes")
      {
        yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::YES);
      }
      else if (opt == "graph")
      {
        yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::GRAPH);
      }
      else if (opt == "builtin")
      {
        yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::BUILTIN);
      }
      else if (opt == "text")
      {
        yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::TEXT);
      }
      else if (opt == "no")
      {
        yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::NO);
      }
      else
      {
        warn(yyextra->fileName,yyextra->lineNr,"Unknown option specified with \\inheritancegraph: '{}'",
            QCString(opt_).stripWhiteSpace());
      }
    }
  }
  return FALSE;
}

static bool handleHideInheritanceGraph(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_inheritanceGraph(CLASS_GRAPH_t::NO); // OFF
  return FALSE;
}

static bool handleReferencedByRelation(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_referencedByRelation(true); // ON
  return FALSE;
}

static bool handleHideReferencedByRelation(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_referencedByRelation(false); // OFF
  return FALSE;
}

static bool handleReferencesRelation(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_referencesRelation(true); // ON
  return FALSE;
}

static bool handleHideReferencesRelation(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->commandOverrides.override_referencesRelation(false); // OFF
  return FALSE;
}

static bool handleQualifier(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  BEGIN(Qualifier);
  return FALSE;
}

static bool handleInternal(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!Config_getBool(INTERNAL_DOCS))
  {
    // make sure some whitespace before a \internal command
    // is not treated as "documentation"
    if (yyextra->current->doc.stripWhiteSpace().isEmpty())
    {
      yyextra->current->doc.clear();
    }
    yyextra->condCount=0;
    BEGIN( SkipInternal );
  }
  else
  {
    // re-enabled for bug640828
    addOutput(yyscanner," \\internal ");
    yyextra->inInternalDocs = TRUE;
  }
  return FALSE;
}

static bool handleStatic(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->isStatic = TRUE;
  return FALSE;
}

static bool handlePure(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->virt = Specifier::Pure;
  return FALSE;
}

static bool handlePrivate(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = Protection::Private;
  return FALSE;
}

static bool handlePrivateSection(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = yyextra->protection = Protection::Private;
  return FALSE;
}

static bool handleProtected(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = Protection::Protected;
  return FALSE;
}

static bool handleProtectedSection(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = yyextra->protection = Protection::Protected ;
  return FALSE;
}

static bool handlePublic(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = Protection::Public;
  return FALSE;
}

static bool handlePublicSection(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = yyextra->protection = Protection::Public;
  return FALSE;
}

static bool handleToc(yyscan_t yyscanner,const QCString &, const StringVector &optList)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current->section.isPageDoc() ||
      yyextra->current->section.isMainpageDoc())
  {
    for (const auto &opt_ : optList)
    {
      QCString opt = QCString(opt_).stripWhiteSpace().lower();
      int level = SectionType::MaxLevel;
      int i = opt.find(':');
      if (i>0)  // found ':' but not on position 0 what would mean just a level
      {
        char dum=0;
        if (sscanf(opt.right(opt.length() - i - 1).data(),"%d%c",&level,&dum) != 1)
        {
          warn(yyextra->fileName,yyextra->lineNr,"Unknown option:level specified with \\tableofcontents: '{}'",
              qPrint(QCString(opt_).stripWhiteSpace()));
          opt = "";
        }
        else
        {
          level = level<=0 ? SectionType::MaxLevel : std::min(level,SectionType::MaxLevel);
          opt = opt.left(i).stripWhiteSpace();
        }
      }
      if (!opt.isEmpty())
      {
        if (opt == "html")
        {
          yyextra->current->localToc.enableHtml(level);
        }
        else if (opt == "latex")
        {
          yyextra->current->localToc.enableLatex(level);
        }
        else if (opt == "xml")
        {
          yyextra->current->localToc.enableXml(level);
        }
        else if (opt == "docbook")
        {
          yyextra->current->localToc.enableDocbook(level);
        }
        else
        {
          warn(yyextra->fileName,yyextra->lineNr,"Unknown option specified with \\tableofcontents: '{}'",
              QCString(opt_).stripWhiteSpace());
        }
      }
    }
    if (yyextra->current->localToc.nothingEnabled())
    {
      // for backward compatibility
      yyextra->current->localToc.enableHtml(SectionType::MaxLevel);
      yyextra->current->localToc.enableXml(SectionType::MaxLevel);
    }
  }
  return FALSE;
}

static bool handleInherit(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(InheritParam);
  return FALSE;
}

static bool handleExtends(yyscan_t yyscanner,const QCString &cmd, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentCmd = cmd;
  BEGIN(ExtendsParam);
  return FALSE;
}

static bool handleCopyBrief(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current->brief.stripWhiteSpace().isEmpty() && yyextra->current->doc.stripWhiteSpace().isEmpty())
  { // if we don't have a brief or detailed description yet,
    // then the @copybrief should end up in the brief description.
    // otherwise it will be copied inline (see bug691315 & bug700788)
    setOutput(yyscanner,OutputBrief);
  }
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  addOutput(yyscanner,"\\copybrief ");
  return FALSE;
}

static bool handleCopyDetails(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  setOutput(yyscanner,OutputDoc);
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  addOutput(yyscanner,"\\copydetails ");
  return FALSE;
}

static bool handleCopyDoc(yyscan_t yyscanner,const QCString &, const StringVector &)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current->brief.stripWhiteSpace().isEmpty() && yyextra->current->doc.stripWhiteSpace().isEmpty())
  { // if we don't have a brief or detailed description yet,
    // then the @copybrief should end up in the brief description.
    // otherwise it will be copied inline (see bug691315 & bug700788)
    setOutput(yyscanner,OutputBrief);
  }
  if (!yyextra->spaceBeforeCmd.isEmpty())
  {
    addOutput(yyscanner,yyextra->spaceBeforeCmd);
    yyextra->spaceBeforeCmd.clear();
  }
  addOutput(yyscanner,"\\copybrief ");
  yyextra->copyDocArg.clear();
  yyextra->braceCount = 0;
  BEGIN(CopyDoc);
  return FALSE;
}

//-----------------------------------------------------------------------------------------

static void initParser(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->sectionLabel.clear();
  yyextra->sectionTitle.clear();
  yyextra->docGroup.clearHeader();
  yyextra->insideParBlock = FALSE;
}

//-----------------------------------------------------------------------------

static bool checkStructuralIndicator(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("yyextra->current->section=%x\n",yyextra->current->section);
  return yyextra->current->section.isDoc();
}

static bool makeStructuralIndicator(yyscan_t yyscanner,MakeEntryType maker)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  assert(maker!=nullptr); // detect programming error
  //printf("yyextra->current->section=%x\n",yyextra->current->section);
  if (yyextra->current->section.isDoc())
  {
    return true;
  }
  else if (maker)
  {
    yyextra->needNewEntry = true;
    yyextra->current->section = maker();
    yyextra->current->fileName = yyextra->fileName;
    yyextra->current->startLine = yyextra->lineNr;
    if (yyextra->current->docLine == -1) yyextra->current->docLine = yyextra->lineNr;
    return false;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------

static void lineCount(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for( const char* c = yytext ; *c ; ++c )
    yyextra->lineNr += (*c == '\n') ;
}

//-----------------------------------------------------------------

static QCString stripQuotes(const char *s)
{
  QCString name;
  if (s==nullptr || *s==0) return name;
  name=s;
  if (name.at(0)=='"' && name.at(name.length()-1)=='"')
  {
    name=name.mid(1,name.length()-2);
  }
  return name;
}

//-----------------------------------------------------------------

static void addRequirementId(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  std::unique_lock<std::mutex> lock(g_sectionMutex);

  QCString reqId = yyextra->current->name;
  //printf("### add reqId='%s' reqTitle='%s'\n",qPrint(reqId),qPrint(yyextra->current->type));
  SectionManager &sm = SectionManager::instance();
  const SectionInfo *si = sm.find(reqId);
  if (si==nullptr)
  {
    si = sm.add(reqId,"requirements",yyextra->lineNr,
               yyextra->current->type,SectionType::Requirement,1);
  }
  else // duplicate requirement
  {
    warn(yyextra->fileName,yyextra->lineNr,"duplicate requirement ID '{}' found at {} line {}, (first occurrence: {}, line {})",
         reqId,
         yyextra->fileName,yyextra->lineNr,
         si->fileName(),si->lineNr());
  }
}

//-----------------------------------------------------------------

static void addXRefItem(yyscan_t yyscanner,
                        const QCString &listName,const QCString &itemTitle,
                        const QCString &listTitle,bool append)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (listName.isEmpty()) return;
  //printf("addXRefItem(%s,%s,%s,%d)\n",listName,itemTitle,listTitle,append);

  std::unique_lock<std::mutex> lock(g_sectionMutex);

  RefList *refList = RefListManager::instance().add(listName,listTitle,itemTitle);
  RefItem *item = nullptr;
  for (auto it = yyextra->current->sli.rbegin(); it != yyextra->current->sli.rend(); ++it)
  {
    RefItem *i = *it;
    if (i && i->list()->listName()==listName)
    {
      //printf("found %s lii->type=%s\n",listName,qPrint(i->list()->listName()));
      item = i;
      break;
    }
  }
  if (item && append) // already found item of same type just before this one
  {
    //printf("listName=%s item id = %d existing\n",listName,item->id());
    item->setText(item->text() + " <p>" + yyextra->outputXRef);
    //printf("%s: text +=%s\n",listName,qPrint(item->text));
  }
  else // new item
  {

    // if we have already an item from the same list type (e.g. a second @todo)
    // in the same Entry (i.e. lii!=0) then we reuse its link anchor.
    item = refList->add();
    //printf("listName=%s item id = %d new yyextra->current=%p\n",listName,item->id(),yyextra->current);
    QCString anchorLabel;
    anchorLabel.sprintf("_%s%06d",listName.data(),item->id());
    item->setText(yyextra->outputXRef);
    item->setAnchor(anchorLabel);
    yyextra->current->sli.push_back(item);
    QCString cmdString;
    cmdString.sprintf(" \\xrefitem %s %d.",qPrint(listName),item->id());
    if (yyextra->inBody)
    {
      yyextra->current->inbodyDocs += cmdString;
    }
    else
    {
      yyextra->current->doc += cmdString;
    }

    {
      SectionManager &sm = SectionManager::instance();
      const SectionInfo *si = sm.find(anchorLabel);
      if (si)
      {
        if (!si->ref().isEmpty()) // we are from a tag file
        {
          si = sm.replace(anchorLabel,listName,yyextra->lineNr,
              yyextra->sectionTitle,SectionType::Anchor,
              yyextra->sectionLevel);
          yyextra->current->anchors.push_back(si);
        }
        else if (si->lineNr() != -1)
        {
          warn(listName,yyextra->lineNr,"multiple use of section label '{}', (first occurrence: {}, line {})",
              anchorLabel,si->fileName(),si->lineNr());
        }
        else
        {
          warn(listName,yyextra->lineNr,"multiple use of section label '{}', (first occurrence: {})",
              anchorLabel,si->fileName());
        }
      }
      else
      {
        si = sm.add(anchorLabel,listName,yyextra->lineNr,
            yyextra->sectionTitle,SectionType::Anchor,
            yyextra->sectionLevel);
        yyextra->current->anchors.push_back(si);
      }
    }
  }
  yyextra->outputXRef.clear();
}

//-----------------------------------------------------------------------------

// make label match LABELID pattern
static void escapeLabel(QCString &label)
{
  if (label.isEmpty()) return;
  char c = label[0];
  if (!((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || c<0))
  {
    label[0]='_'; // replace invalid starting char by _
  }
  for (size_t i=1; i<label.size(); i++)
  {
    c = label[i];
    if (!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_' || c<0))
    {
      label[i]='_'; // replace invalid char by _
    }
  }
}


//-----------------------------------------------------------------------------

// Adds a formula text to the list/dictionary of formulas if it was
// not already added. Returns the label of the formula.
static QCString addFormula(yyscan_t yyscanner)
{
  std::unique_lock<std::mutex> lock(g_formulaMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString formLabel;
  QCString formula = (yyextra->formulaPreText +
                      yyextra->formulaText.stripLeadingAndTrailingEmptyLines() +
                      yyextra->formulaPostText).stripWhiteSpace();
  //printf("formulaText=\n=1=\n{%s}\n=2=\n{%s}\n=3=\n{%s}\n===",
  //    qPrint(yyextra->formulaText),
  //    qPrint(yyextra->formulaText.stripLeadingAndTrailingEmptyLines()),
  //    qPrint(formula)
  //    );
  int id = FormulaManager::instance().addFormula(formula);
  formLabel.sprintf("\\_form#%d",id);
  for (int i=0;i<yyextra->formulaNewLines;i++) formLabel+="@_fakenl"; // add fake newlines to
                                                         // keep the warnings
                                                         // correctly aligned.
  return formLabel;
}

//-----------------------------------------------------------------------------

static SectionType sectionLevelToType(int level)
{
  if (level>=0 && level<SectionType::MaxLevel) return SectionType(level);
  return SectionType::Anchor;
}

static void addSection(yyscan_t yyscanner, bool addYYtext)
{
  std::unique_lock<std::mutex> lock(g_sectionMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  SectionManager &sm = SectionManager::instance();
  const SectionInfo *si = sm.find(yyextra->sectionLabel);
  if (si)
  {
    if (!si->ref().isEmpty()) // we are from a tag file
    {
      // create a new section element
      if (addYYtext) yyextra->sectionTitle+=yytext;
      yyextra->sectionTitle=yyextra->sectionTitle.stripWhiteSpace();
      si = sm.replace(yyextra->sectionLabel,yyextra->fileName,yyextra->lineNr,
                      yyextra->sectionTitle,sectionLevelToType(yyextra->sectionLevel),
                      yyextra->sectionLevel);

      // add section to this entry
      yyextra->current->anchors.push_back(si);
    }
    else if (si->lineNr() != -1)
    {
      warn(yyextra->fileName,yyextra->lineNr,"multiple use of section label '{}' while adding section, (first occurrence: {}, line {})",
          yyextra->sectionLabel,si->fileName(),si->lineNr());
    }
    else
    {
      warn(yyextra->fileName,yyextra->lineNr,"multiple use of section label '{}' while adding section, (first occurrence: {})",
          yyextra->sectionLabel,si->fileName());
    }
  }
  else
  {
    // create a new section element
    if (addYYtext) yyextra->sectionTitle+=yytext;
    yyextra->sectionTitle=yyextra->sectionTitle.stripWhiteSpace();
    si = sm.add(yyextra->sectionLabel,yyextra->fileName,yyextra->lineNr,
                yyextra->sectionTitle,sectionLevelToType(yyextra->sectionLevel),
                yyextra->sectionLevel);

    // add section to this entry
    yyextra->current->anchors.push_back(si);
  }
}

//-----------------------------------------------------------------------------

static void addCite(yyscan_t yyscanner)
{
  std::unique_lock<std::mutex> lock(g_citeMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString name(yytext);
  if (yytext[0] =='"')
  {
    name=yytext+1;
    name=name.left((int)yyleng-2);
  }
  CitationManager::instance().insert(name);
}

//-----------------------------------------------------------------------------
static const reg::Ex nonBrief_re(R"( *[\\@]ifile \"[^\"]*\" [\\@]iline (\d+) [\\@]ilinebr ([ \n]*))");

// strip trailing whitespace (excluding newlines) from string s
static void stripTrailingWhiteSpace(QCString &s)
{
  size_t len = s.length();
  int i = (int)len-1;
  while (i>=0)
  {
    char c = s.at(i);
    if (c==' ' || c=='\t' || c=='\r') // normal whitespace
    {
      i--;
    }
    else if (c=='r' && i>=7 && literal_at(s.data()+i-7,"\\ilinebr")) // special line break marker
    {
      i-=8;
    }
    else // non-whitespace
    {
      break;
    }
  }
  //printf("stripTrailingWhitespace(%s) i=%d len=%d\n",qPrint(s),i,len);
  if (i!=(int)len-1)
  {
    s.resize(i+1); // string up to and including char at pos i
  }
}

// selects the output to write to
static inline void setOutput(yyscan_t yyscanner,OutputContext ctx)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool xrefAppendToPrev = yyextra->xrefAppendFlag;
  // determine append flag for the next item (i.e. the end of this item)
  yyextra->xrefAppendFlag = !yyextra->inBody &&
                   yyextra->inContext==OutputXRef && ctx==OutputXRef && // two consecutive xref items
                   yyextra->newXRefKind==yyextra->xrefKind &&           // of the same kind
                   (yyextra->xrefKind!=XRef_Item ||
                    yyextra->newXRefItemKey==yyextra->xrefItemKey);     // with the same key if \xrefitem
  //printf("%d && %d && %d && (%d || %d)\n",
  //                 yyextra->inContext==OutputXRef,
  //                 ctx==OutputXRef,
  //                 yyextra->newXRefKind==yyextra->xrefKind,
  //                 yyextra->xrefKind!=XRef_Item,
  //                 yyextra->newXRefItemKey==yyextra->xrefItemKey);
  //printf("refKind=%d yyextra->newXRefKind=%d xrefAppendToPrev=%d yyextra->xrefAppendFlag=%d\n",
  //             yyextra->xrefKind,yyextra->newXRefKind,xrefAppendToPrev,yyextra->xrefAppendFlag);

  //printf("setOutput(yyscanner,yyextra->inContext=%d ctx=%d)\n",yyextra->inContext,ctx);
  if (yyextra->inContext==OutputXRef) // end of XRef section => add the item
  {
    // See if we can append this new xref item to the previous one.
    // We know this at the start of the next item of the same
    // type and need to remember this until the end of that item.
    switch(yyextra->xrefKind)
    {
      case XRef_Todo:
        addXRefItem(yyscanner,QCString("todo"),
            theTranslator->trTodo(),
            theTranslator->trTodoList(),
            xrefAppendToPrev
            );
        break;
      case XRef_Test:
        addXRefItem(yyscanner,QCString("test"),
            theTranslator->trTest(),
            theTranslator->trTestList(),
            xrefAppendToPrev
            );
        break;
      case XRef_Bug:
        addXRefItem(yyscanner,QCString("bug"),
            theTranslator->trBug(),
            theTranslator->trBugList(),
            xrefAppendToPrev
            );
        break;
      case XRef_Deprecated:
        addXRefItem(yyscanner,QCString("deprecated"),
            theTranslator->trDeprecated(),
            theTranslator->trDeprecatedList(),
            xrefAppendToPrev
            );
        break;
      case XRef_Item:  // user defined list
        addXRefItem(yyscanner,yyextra->xrefItemKey,
            yyextra->xrefItemTitle,
            yyextra->xrefListTitle,
            xrefAppendToPrev
            );
        break;
      case XRef_None:
        ASSERT(0);
        break;
    }
  }
  yyextra->xrefItemKey = yyextra->newXRefItemKey;

  int oldContext = yyextra->inContext;
  yyextra->inContext = ctx;
  if (yyextra->inContext!=OutputXRef && yyextra->inBody) yyextra->inContext=OutputInbody;
  switch(yyextra->inContext)
  {
    case OutputDoc:
      if (oldContext!=yyextra->inContext)
      {
        stripTrailingWhiteSpace(yyextra->current->doc);
        if (yyextra->current->doc.isEmpty()) yyextra->current->docLine = yyextra->lineNr;
        if (yyextra->current->docFile.isEmpty())
        {
          yyextra->current->docFile = yyextra->fileName;
          yyextra->current->docLine = yyextra->lineNr;
        }
      }
      yyextra->pOutputString = &yyextra->current->doc;
      break;
    case OutputBrief:
      {
        if (oldContext!=yyextra->inContext)
        {
          if (yyextra->current->brief.isEmpty()) yyextra->current->briefLine = yyextra->lineNr;
          if (yyextra->current->briefFile.isEmpty())
          {
            yyextra->current->briefFile = yyextra->fileName;
            yyextra->current->briefLine = yyextra->lineNr;
          }
        }
        bool foundMatch = false;
        if (yyextra->current->brief.stripWhiteSpace().isEmpty()) // we only want one brief
          // description even if multiple
          // are given...
        {
          foundMatch = true;
        }
        else
        {
          std::string str = yyextra->current->brief.str();
          reg::Match match;
          if (reg::match(str,match,nonBrief_re)) // match found
          {
            size_t cnt = 0;
            for (size_t i = 0; i < match[2].str().size(); i++)
            {
              if (match[2].str()[i] == '\n') cnt++;
            }
            if (cnt>0)
            {
              yyextra->current->brief = yyextra->current->brief.left(yyextra->current->brief.length()-cnt);
              // set warning line correct
              yyextra->current->brief += " \\iline " + QCString().setNum(cnt + static_cast<int>(std::stoul(match[1].str()))) + " \\ilinebr ";
            }
            foundMatch = true;
          }
        }
        if (foundMatch)
        {
            yyextra->pOutputString = &yyextra->current->brief;
        }
        else
        {
          if (!yyextra->current->doc.isEmpty()) // when appending parts add a new line
          {
            yyextra->current->doc += "\n";
          }
          yyextra->pOutputString = &yyextra->current->doc;
          yyextra->inContext = OutputDoc; // need to switch to detailed docs, see bug 631380
        }
      }
      break;
    case OutputXRef:
      yyextra->pOutputString = &yyextra->outputXRef;
      // first item found, so can't append to previous
      //yyextra->xrefAppendFlag = FALSE;
      break;
    case OutputInbody:
      yyextra->pOutputString = &yyextra->current->inbodyDocs;
      break;
  }
}


static void addAnchor(yyscan_t yyscanner,const QCString &anchor, const QCString &title)
{
  std::unique_lock<std::mutex> lock(g_sectionMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  SectionManager &sm = SectionManager::instance();
  const SectionInfo *si = sm.find(anchor);
  if (si)
  {
    if (!si->ref().isEmpty()) // we are from a tag file
    {
      si = sm.replace(anchor,yyextra->fileName,yyextra->lineNr,QCString(),SectionType::Anchor,0);
      yyextra->current->anchors.push_back(si);
    }
    else if (si->lineNr() != -1)
    {
      warn(yyextra->fileName,yyextra->lineNr,
          "multiple use of section label '{}' while adding anchor, (first occurrence: {}, line {})",
          anchor,si->fileName(),si->lineNr());
    }
    else
    {
      warn(yyextra->fileName,yyextra->lineNr,"multiple use of section label '{}' while adding anchor, (first occurrence: {})",
          anchor,si->fileName());
    }
  }
  else
  {
    si = sm.add(anchor,yyextra->fileName,yyextra->lineNr,title,SectionType::Anchor,0);
    yyextra->current->anchors.push_back(si);
  }
}

// add a string to the output
static inline void addOutput(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("addOutput(yyscanner,%s)\n",s);
  *yyextra->pOutputString+=s;
}

// add a string to the output
static inline void addOutput(yyscan_t yyscanner,const QCString &s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("addOutput(yyscanner,%s)\n",s);
  *yyextra->pOutputString+=s;
}

// add a character to the output
static inline void addOutput(yyscan_t yyscanner,char c)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  *yyextra->pOutputString+=c;
}

static void addIline(yyscan_t yyscanner,int lineNr)
{
  char cmd[30];
  qsnprintf(cmd,30," \\iline %d ",lineNr);
  addOutput(yyscanner, cmd);
}

static void addIlineBreak(yyscan_t yyscanner,int lineNr)
{
  char cmd[30];
  qsnprintf(cmd,30," \\iline %d \\ilinebr ",lineNr);
  addOutput(yyscanner, cmd);
}

static void endBrief(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  std::string_view str = yyextra->current->brief.view();
  reg::Match match;
  if (!stripWhiteSpace(str).empty() && !reg::match(str,match,nonBrief_re))
  { // only go to the detailed description if we have
    // found some brief description and not just whitespace
    yyextra->briefEndsAtDot=FALSE;
    setOutput(yyscanner,OutputDoc);
    if (yyextra->current->doc.stripWhiteSpace().isEmpty())
    {
      yyextra->current->docLine = yyextra->lineNr;
      yyextra->current->doc = "";
    }
    else
    {
      addIline(yyscanner,yyextra->lineNr);
    }
    addOutput(yyscanner,yytext);
  }
  else
  {
    int saveLineNr = yyextra->lineNr;
    lineCount(yyscanner);
    yyextra->current->briefLine = yyextra->lineNr;
    yyextra->lineNr = saveLineNr;
  }
}

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->prevPosition=yyextra->inputPosition;
  int c=0;
  while( c < max_size && yyextra->inputString[yyextra->inputPosition] )
  {
    *buf = yyextra->inputString[yyextra->inputPosition++] ;
    //printf("%d (%c)\n",*buf,*buf);
    c++; buf++;
  }
  return c;
}

//----------------------------------------------------------------------------

static void checkFormula(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (YY_START==ReadFormulaShort || YY_START==ReadFormulaShortSection ||
      YY_START==ReadFormulaRound || YY_START==ReadFormulaRoundSection ||
      YY_START==ReadFormulaLong)
  {
    warn(yyextra->fileName,yyextra->lineNr,"End of comment block while inside formula.");
  }
}

//----------------------------------------------------------------------------

struct CommentScanner::Private
{
  yyscan_t yyscanner;
  commentscanYY_state extra;
};

CommentScanner::CommentScanner() : p(std::make_unique<Private>())
{
  commentscanYYlex_init_extra(&p->extra,&p->yyscanner);
#ifdef FLEX_DEBUG
  commentscanYYset_debug(Debug::isFlagSet(Debug::Lex_commentscan)?1:0,p->yyscanner);
#endif
}

CommentScanner::~CommentScanner()
{
  commentscanYYlex_destroy(p->yyscanner);
}

bool CommentScanner::parseCommentBlock(/* in */     OutlineParserInterface *parser,
                       /* in */     Entry *curEntry,
                       /* in */     const QCString &comment,
                       /* in */     const QCString &fileName,
                       /* in,out */ int  &lineNr,
                       /* in */     bool isBrief,
                       /* in */     bool isAutoBriefOn,
                       /* in */     bool isInbody,
                       /* in,out */ Protection &prot,
                       /* in,out */ int &position,
                       /* out */    bool &newEntryNeeded,
                       /* in */     bool markdownSupport,
                       /* inout */  GuardedSectionStack *guards
                      )
{
  AUTO_TRACE("comment='{}' fileName={} lineNr={} isBrief={} isAutoBriefOn={} inInbody={}"
             " prot={} markdownSupport={}",Trace::trunc(comment),fileName,lineNr,isBrief,
             isAutoBriefOn,isInbody,prot,markdownSupport);
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  initParser(yyscanner);
  yyextra->guards = guards;
  yyextra->langParser     = parser;
  yyextra->current        = curEntry;
  yyextra->current->docLine = (lineNr > 1 ? lineNr : 1);
  if (comment.isEmpty()) return FALSE; // avoid empty strings
  yyextra->inputString    = comment;
  yyextra->inputString.append(" ");
  yyextra->inputPosition  = position;
  yyextra->lineNr         = lineNr;
  yyextra->fileName       = fileName;
  yyextra->protection     = prot;
  yyextra->needNewEntry   = FALSE;
  yyextra->xrefKind       = XRef_None;
  yyextra->xrefAppendFlag = FALSE;
  yyextra->insidePre      = FALSE;
  yyextra->parseMore      = FALSE;
  yyextra->inBody         = isInbody;
  yyextra->markdownSupport= markdownSupport;
  yyextra->outputXRef.clear();
  if (!isBrief && !isAutoBriefOn && !yyextra->current->doc.isEmpty())
  { // add newline separator between detailed comment blocks
    yyextra->current->doc += '\n';
  }
  setOutput(yyscanner, isBrief || isAutoBriefOn ? OutputBrief : OutputDoc );
  yyextra->briefEndsAtDot = isAutoBriefOn;
  yyextra->condCount    = 0;
  yyextra->sectionLevel = 0;
  yyextra->spaceBeforeCmd.clear();
  yyextra->spaceBeforeIf.clear();
  yyextra->htmlContextStack.clear();

  DebugLex debugLex(Debug::Lex_commentscan, __FILE__, !fileName.isEmpty() ? qPrint(fileName): nullptr);
  if (!yyextra->current->inbodyDocs.isEmpty() && isInbody) // separate in body fragments
  {
    char cmd[30];
    qsnprintf(cmd,30,"\n\n\\iline %d \\ilinebr ",lineNr);
    yyextra->current->inbodyDocs+=cmd;
  }

  Debug::print(Debug::CommentScan,0,"-----------\nCommentScanner: {}:{}\n"
               "input=[\n{}]\n",fileName,lineNr,yyextra->inputString
              );

  commentscanYYrestart( nullptr, yyscanner );
  BEGIN( Comment );
  commentscanYYlex(yyscanner);
  setOutput(yyscanner, OutputDoc );

  if (YY_START==OverloadParam) // comment ended with \overload
  {
    addOutput(yyscanner,getOverloadDocs());
  }

  if (yyextra->insideParBlock)
  {
    warn(yyextra->fileName,yyextra->lineNr,
        "Documentation block ended while inside a \\parblock. Missing \\endparblock");
  }

  yyextra->current->doc=stripLeadingAndTrailingEmptyLines(yyextra->current->doc,yyextra->current->docLine);
  yyextra->current->brief=stripLeadingAndTrailingEmptyLines(yyextra->current->brief,yyextra->current->docLine);

  if (yyextra->current->section.isFileDoc() && yyextra->current->doc.isEmpty())
  {
    // to allow a comment block with just a @file command.
    yyextra->current->doc="\n\n";
  }

  if (yyextra->current->section.isMemberGrp() &&
      yyextra->docGroup.isEmpty()) // @name section but no group started yet
  {
    yyextra->docGroup.open(yyextra->current,yyextra->fileName,yyextra->lineNr,true);
  }

  Debug::print(Debug::CommentScan,0,"-----------\nCommentScanner: {}:{}\noutput=[\n"
               "brief=[line={}\n{}]\ndocs=[line={}\n{}]\ninbody=[line={}\n{}]\n]\n===========\n",
               fileName,lineNr,
               yyextra->current->briefLine,yyextra->current->brief,
               yyextra->current->docLine,yyextra->current->doc,
               yyextra->current->inbodyLine,yyextra->current->inbodyDocs
              );

  checkFormula(yyscanner);
  prot = yyextra->protection;

  yyextra->docGroup.addDocs(curEntry);

  newEntryNeeded = yyextra->needNewEntry;

  // if we did not proceed during this call, it does not make
  // sense to continue, since we get stuck. See bug 567346 for situations
  // were this happens
  if (yyextra->parseMore && position==yyextra->inputPosition) yyextra->parseMore=FALSE;

  if (!yyextra->parseMore && !yyextra->guards->empty())
  {
    warn(yyextra->fileName,yyextra->lineNr,"Documentation block ended in the middle of a conditional section!");
  }

  if (yyextra->parseMore) position=yyextra->inputPosition; else position=0;

  lineNr = yyextra->lineNr;
  AUTO_TRACE_EXIT("position={} parseMore={} newEntryNeeded={}",
      position,yyextra->parseMore,newEntryNeeded);

  return yyextra->parseMore;
}

static void handleGuard(yyscan_t yyscanner,const QCString &expr)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  CondParser prs;
  bool sectionEnabled = false;
  if (!expr.isEmpty())
  {
    sectionEnabled=prs.parse(yyextra->fileName,yyextra->lineNr,expr.stripWhiteSpace());
  }
  bool parentEnabled = yyextra->guards->top().parentVisible();
  if (parentEnabled)
  {
    if (
        (sectionEnabled && yyextra->guardType==Guard_If) ||
        (!sectionEnabled && yyextra->guardType==Guard_IfNot)
       ) // section is visible
    {

      yyextra->guards->top().setEnabled(true);
      yyextra->guards->top().setEnabledFound();
      BEGIN( GuardParamEnd );
    }
    else if (yyextra->guardType==Guard_ElseIf)
    {
      if (yyextra->guards->top().isEnabledFound())
      {
        yyextra->guards->top().setEnabled(false);
        BEGIN( SkipGuardedSection );
      }
      else if (sectionEnabled)
      {
        yyextra->guards->top().setEnabled(true);
        yyextra->guards->top().setEnabledFound();
        BEGIN( GuardParamEnd );
      }
      else
      {
        yyextra->guards->top().setEnabled(false);
        BEGIN( SkipGuardedSection );
      }
    }
    else // section is invisible
    {
      BEGIN( SkipGuardedSection );
    }
  }
  else // invisible because of parent
  {
    BEGIN( SkipGuardedSection );
  }
}

void CommentScanner::initGroupInfo(Entry *entry)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.initGroupInfo(entry);
}

void CommentScanner::enterFile(const QCString &fileName,int lineNr)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.enterFile(fileName,lineNr);
}

void CommentScanner::leaveFile(const QCString &fileName,int lineNr)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.leaveFile(fileName,lineNr);
}

void CommentScanner::enterCompound(const QCString &fileName,int lineNr,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.enterCompound(fileName,lineNr,name);
}

void CommentScanner::leaveCompound(const QCString &fileName,int lineNr,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.leaveCompound(fileName,lineNr,name);
}

void CommentScanner::open(Entry *e,const QCString &fileName,int lineNr,bool implicit)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.open(e,fileName,lineNr,implicit);
}

void CommentScanner::close(Entry *e,const QCString &fileName,int lineNr,bool foundInline,bool implicit)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->docGroup.close(e,fileName,lineNr,foundInline,implicit);
}

#include "commentscan.l.h"
