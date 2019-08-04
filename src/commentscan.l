/*****************************************************************************
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

%option never-interactive
%option prefix="commentscanYY"

%{

/*
 *	includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include <qarray.h>
#include <qstack.h>
#include <qregexp.h>
#include <qfile.h>
#include <qcstringlist.h>
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "index.h"
#include "defargs.h"
#include "language.h"
#include "outputlist.h"
#include "membergroup.h"
#include "reflist.h"
#include "debug.h"
#include "parserintf.h"
#include "cite.h"
#include "markdown.h"
#include "condparser.h"
#include "formula.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

// forward declarations
static bool handleBrief(const QCString &, const QCStringList &);
static bool handleFn(const QCString &, const QCStringList &);
static bool handleDef(const QCString &, const QCStringList &);
static bool handleOverload(const QCString &, const QCStringList &);
static bool handleEnum(const QCString &, const QCStringList &);
static bool handleDefGroup(const QCString &, const QCStringList &);
static bool handleAddToGroup(const QCString &, const QCStringList &);
static bool handleWeakGroup(const QCString &, const QCStringList &);
static bool handleNamespace(const QCString &, const QCStringList &);
static bool handlePackage(const QCString &, const QCStringList &);
static bool handleClass(const QCString &, const QCStringList &);
static bool handleHeaderFile(const QCString &, const QCStringList &);
static bool handleProtocol(const QCString &, const QCStringList &);
static bool handleCategory(const QCString &, const QCStringList &);
static bool handleUnion(const QCString &, const QCStringList &);
static bool handleStruct(const QCString &, const QCStringList &);
static bool handleInterface(const QCString &, const QCStringList &);
static bool handleIdlException(const QCString &, const QCStringList &);
static bool handlePage(const QCString &, const QCStringList &);
static bool handleMainpage(const QCString &, const QCStringList &);
static bool handleFile(const QCString &, const QCStringList &);
static bool handleDir(const QCString &, const QCStringList &);
static bool handleExample(const QCString &, const QCStringList &);
static bool handleDetails(const QCString &, const QCStringList &);
static bool handleName(const QCString &, const QCStringList &);
static bool handleTodo(const QCString &, const QCStringList &);
static bool handleTest(const QCString &, const QCStringList &);
static bool handleBug(const QCString &, const QCStringList &);
static bool handleSubpage(const QCString &s, const QCStringList &);
static bool handleDeprecated(const QCString &, const QCStringList &);
static bool handleXRefItem(const QCString &, const QCStringList &);
static bool handleRelated(const QCString &, const QCStringList &);
static bool handleRelatedAlso(const QCString &, const QCStringList &);
static bool handleMemberOf(const QCString &, const QCStringList &);
static bool handleRefItem(const QCString &, const QCStringList &);
static bool handleSection(const QCString &, const QCStringList &);
static bool handleAnchor(const QCString &, const QCStringList &);
static bool handleCite(const QCString &, const QCStringList &);
static bool handleFormatBlock(const QCString &, const QCStringList &);
static bool handleAddIndex(const QCString &, const QCStringList &);
static bool handleIf(const QCString &, const QCStringList &);
static bool handleIfNot(const QCString &, const QCStringList &);
static bool handleElseIf(const QCString &, const QCStringList &);
static bool handleElse(const QCString &, const QCStringList &);
static bool handleEndIf(const QCString &, const QCStringList &);
static bool handleIngroup(const QCString &, const QCStringList &);
static bool handleNoSubGrouping(const QCString &, const QCStringList &);
static bool handleShowInitializer(const QCString &, const QCStringList &);
static bool handleHideInitializer(const QCString &, const QCStringList &);
static bool handleCallgraph(const QCString &, const QCStringList &);
static bool handleHideCallgraph(const QCString &, const QCStringList &);
static bool handleCallergraph(const QCString &, const QCStringList &);
static bool handleHideCallergraph(const QCString &, const QCStringList &);
static bool handleReferencedByRelation(const QCString &, const QCStringList &);
static bool handleHideReferencedByRelation(const QCString &, const QCStringList &);
static bool handleReferencesRelation(const QCString &, const QCStringList &);
static bool handleHideReferencesRelation(const QCString &, const QCStringList &);
static bool handleInternal(const QCString &, const QCStringList &);
static bool handleLineBr(const QCString &, const QCStringList &);
static bool handleStatic(const QCString &, const QCStringList &);
static bool handlePure(const QCString &, const QCStringList &);
static bool handlePrivate(const QCString &, const QCStringList &);
static bool handlePrivateSection(const QCString &, const QCStringList &);
static bool handleProtected(const QCString &, const QCStringList &);
static bool handleProtectedSection(const QCString &, const QCStringList &);
static bool handlePublic(const QCString &s, const QCStringList &);
static bool handlePublicSection(const QCString &s, const QCStringList &);
static bool handleToc(const QCString &s, const QCStringList &);
static bool handleInherit(const QCString &, const QCStringList &);
static bool handleExtends(const QCString &, const QCStringList &);
static bool handleCopyDoc(const QCString &, const QCStringList &);
static bool handleCopyBrief(const QCString &, const QCStringList &);
static bool handleCopyDetails(const QCString &, const QCStringList &);
static bool handleParBlock(const QCString &, const QCStringList &);
static bool handleEndParBlock(const QCString &, const QCStringList &);
static bool handleParam(const QCString &, const QCStringList &);
static bool handleRetval(const QCString &, const QCStringList &);

typedef bool (*DocCmdFunc)(const QCString &name, const QCStringList &optList);

struct DocCmdMap
{
  const char *cmdName;
  DocCmdFunc handler;
  bool endsBrief;
};

// map of command to handler function
static DocCmdMap docCmdMap[] =
{
  // command name      handler function         ends brief description
  { "brief",           &handleBrief,            FALSE },
  { "short",           &handleBrief,            FALSE },
  { "fn",              &handleFn,               TRUE },
  { "var",             &handleFn,               TRUE },
  { "typedef",         &handleFn,               TRUE },
  { "property",        &handleFn,               TRUE },
  { "def",             &handleDef,              TRUE },
  { "overload",        &handleOverload,         FALSE },
  { "enum",            &handleEnum,             TRUE },
  { "defgroup",        &handleDefGroup,         TRUE },
  { "addtogroup",      &handleAddToGroup,       TRUE },
  { "weakgroup",       &handleWeakGroup,        TRUE },
  { "namespace",       &handleNamespace,        TRUE },
  { "package",         &handlePackage,          TRUE },
  { "class",           &handleClass,            TRUE },
  { "headerfile",      &handleHeaderFile,       FALSE },
  { "protocol",        &handleProtocol,         TRUE },
  { "category",        &handleCategory,         TRUE },
  { "union",           &handleUnion,            TRUE },
  { "struct",          &handleStruct,           TRUE },
  { "interface",       &handleInterface,        TRUE },
  { "idlexcept",       &handleIdlException,     TRUE },
  { "page",            &handlePage,             TRUE },
  { "mainpage",        &handleMainpage,         TRUE },
  { "file",            &handleFile,             TRUE },
  { "dir",             &handleDir,              TRUE },
  { "example",         &handleExample,          FALSE },
  { "details",         &handleDetails,          TRUE  },
  { "name",            &handleName,             FALSE },
  { "todo",            &handleTodo,             FALSE }, // end brief will be done differently
  { "test",            &handleTest,             FALSE }, // end brief will be done differently
  { "bug",             &handleBug,              FALSE }, // end brief will be done differently
  { "deprecated",      &handleDeprecated,       FALSE }, // end brief will be done differently
  { "xrefitem",        &handleXRefItem,         FALSE }, // end brief will be done differently
  { "related",         &handleRelated,          TRUE  },
  { "relates",         &handleRelated,          TRUE  },
  { "relatedalso",     &handleRelatedAlso,      TRUE  },
  { "relatesalso",     &handleRelatedAlso,      TRUE  },
  { "parblock",        &handleParBlock,         TRUE  },
  { "endparblock",     &handleEndParBlock,      TRUE  },
  { "refitem",         &handleRefItem,          TRUE  },
  { "cite",            &handleCite,             FALSE },
  { "subpage",         &handleSubpage,          TRUE  },
  { "section",         &handleSection,          TRUE  },
  { "subsection",      &handleSection,          TRUE  },
  { "subsubsection",   &handleSection,          TRUE  },
  { "paragraph",       &handleSection,          TRUE  },
  { "anchor",          &handleAnchor,           TRUE  },
  { "verbatim",        &handleFormatBlock,      TRUE  },
  { "latexonly",       &handleFormatBlock,      FALSE },
  { "htmlonly",        &handleFormatBlock,      FALSE },
  { "xmlonly",         &handleFormatBlock,      FALSE },
  { "docbookonly",     &handleFormatBlock,      FALSE },
  { "rtfonly",         &handleFormatBlock,      FALSE },
  { "manonly",         &handleFormatBlock,      FALSE },
  { "dot",             &handleFormatBlock,      TRUE  },
  { "msc",             &handleFormatBlock,      TRUE  },
  { "startuml",        &handleFormatBlock,      TRUE  },
  { "code",            &handleFormatBlock,      TRUE  },
  { "addindex",        &handleAddIndex,         FALSE },
  { "if",              &handleIf,               FALSE },
  { "ifnot",           &handleIfNot,            FALSE },
  { "elseif",          &handleElseIf,           FALSE },
  { "else",            &handleElse,             FALSE },
  { "endif",           &handleEndIf,            FALSE },
  { "ingroup",         &handleIngroup,          TRUE },
  { "nosubgrouping",   &handleNoSubGrouping,    FALSE },
  { "showinitializer", &handleShowInitializer,  FALSE },
  { "hideinitializer", &handleHideInitializer,  FALSE },
  { "callgraph",       &handleCallgraph,        FALSE },
  { "hidecallgraph",   &handleHideCallgraph,    FALSE },
  { "callergraph",     &handleCallergraph,      FALSE },
  { "hidecallergraph", &handleHideCallergraph,  FALSE },
  { "showrefby",       &handleReferencedByRelation,     FALSE },
  { "hiderefby",       &handleHideReferencedByRelation, FALSE },
  { "showrefs",        &handleReferencesRelation,       FALSE },
  { "hiderefs",        &handleHideReferencesRelation,   FALSE },
  { "internal",        &handleInternal,         TRUE  },
  { "_linebr",         &handleLineBr,           FALSE },
  { "static",          &handleStatic,           FALSE },
  { "pure",            &handlePure,             FALSE },
  { "private",         &handlePrivate,          FALSE },
  { "privatesection",  &handlePrivateSection,   FALSE },
  { "protected",       &handleProtected,        FALSE },
  { "protectedsection",&handleProtectedSection, FALSE },
  { "public",          &handlePublic,           FALSE },
  { "publicsection",   &handlePublicSection,    FALSE },
  { "tableofcontents", &handleToc,              FALSE },
  { "inherit",         &handleInherit,          TRUE  },
  { "extends",         &handleExtends,          TRUE  },
  { "implements",      &handleExtends,          TRUE  },
  { "memberof",        &handleMemberOf,         TRUE  },
  { "arg",             0,                       TRUE  },
  { "attention",       0,                       TRUE  },
  { "author",          0,                       TRUE  },
  { "authors",         0,                       TRUE  },
  { "copydoc",         &handleCopyDoc,          TRUE  },
  { "copybrief",       &handleCopyBrief,        FALSE },
  { "copydetails",     &handleCopyDetails,      TRUE  },
  { "copyright",       0,                       TRUE  },
  { "date",            0,                       TRUE  },
  { "dotfile",         0,                       TRUE  },
  { "htmlinclude",     0,                       FALSE },
  { "image",           0,                       TRUE  },
  { "include",         0,                       TRUE  },
  { "includelineno",   0,                       TRUE  },
  { "invariant",       0,                       TRUE  },
  { "latexinclude",    0,                       FALSE },
  { "li",              0,                       TRUE  },
  { "line",            0,                       TRUE  },
  { "note",            0,                       TRUE  },
  { "par",             0,                       TRUE  },
  { "param",           &handleParam,            TRUE  },
  { "tparam",          0,                       TRUE  },
  { "post",            0,                       TRUE  },
  { "pre",             0,                       TRUE  },
  { "remark",          0,                       TRUE  },
  { "remarks",         0,                       TRUE  },
  { "result",          0,                       TRUE  },
  { "return",          0,                       TRUE  },
  { "returns",         0,                       TRUE  },
  { "exception",       0,                       TRUE  },
  { "retval",          &handleRetval,           TRUE  },
  { "sa",              0,                       TRUE  },
  { "see",             0,                       TRUE  },
  { "since",           0,                       TRUE  },
  { "throw",           0,                       TRUE  },
  { "throws",          0,                       TRUE  },
  { "until",           0,                       TRUE  },
  { "verbinclude",     0,                       FALSE },
  { "version",         0,                       TRUE  },
  { "warning",         0,                       TRUE  },
  { "snippet",         0,                       TRUE  },
  { "snippetlineno",   0,                       TRUE  },
  { 0, 0, FALSE }
};

/** @brief Command mapper.
 *
 *  Maps a command name (as found in a comment block) onto a
 *  specific handler function.
 */
class DocCmdMapper
{
  public:
    struct Cmd
    {
      DocCmdFunc func;
      bool endsBrief;
    };

    /** maps a command name to a handler function */
    static Cmd *map(const char *name)
    {
      return instance()->find(name);
    }

    /** release the singleton */
    static void freeInstance()
    {
      delete s_instance; s_instance=0;
    }

  private:
    static DocCmdMapper *instance()
    {
      if (s_instance==0) s_instance = new DocCmdMapper;
      return s_instance;
    }

    DocCmdMapper() : m_map(113)
    {
      m_map.setAutoDelete(TRUE);
      DocCmdMap *p = docCmdMap;
      while (p->cmdName)
      {
	if (m_map.find(p->cmdName)!=0)
	{
	  err("DocCmdMapper: command %s already added\n",p->cmdName);
	  exit(1);
	}
	Cmd *cmd = new Cmd;
	cmd->func = p->handler;
	cmd->endsBrief = p->endsBrief;
	m_map.insert(p->cmdName,cmd);
	p++;
      }
    }

    Cmd *find(const char *name)
    {
      return m_map.find(name);
    }
    QDict<Cmd> m_map;
    static DocCmdMapper *s_instance;
};

DocCmdMapper *DocCmdMapper::s_instance=0;
  
bool inInternalDocs = FALSE;
  
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
  Guard_Skip
};

class GuardedSection
{
  public:
    GuardedSection(bool enabled,bool parentVisible) 
      : m_enabled(enabled),m_parentVisible(parentVisible) {}
    bool isEnabled() const { return m_enabled; }
    bool parentVisible() const { return m_parentVisible; }
  
  private:
    bool m_enabled;
    bool m_parentVisible;
};

/* -----------------------------------------------------------------
 *
 *	statics
 */

static ParserInterface *langParser;          // the language parser that is calling us
static QCString         inputString;         // input string
static int		inputPosition;       // read pointer
static QCString		yyFileName;          // file name that is read from
static int		yyLineNr;            // line number in the input
static bool             inBody;              // was the comment found inside the body of a function?
static OutputContext    inContext;           // are we inside the brief, details or xref part
static bool             briefEndsAtDot;      // does the brief description stop at a dot?
static QCString         formulaText;         // Running text of a formula
static QCString         formulaEnv;          // environment name
static int              formulaNewLines;     // amount of new lines in the formula
static QCString        *pOutputString;       // pointer to string to which the output is appended.
static QCString         outputXRef;          // temp argument of todo/test/../xrefitem commands
static QCString         blockName;           // preformatted block name (e.g. verbatim, latexonly,...)
static XRefKind         xrefKind;            // kind of cross-reference command
static XRefKind         newXRefKind;         // 
static GuardType        guardType;           // kind of guard for conditional section
static bool             enabledSectionFound;
static QCString         functionProto;       // function prototype
static QStack<GuardedSection> guards;        // tracks nested conditional sections (if,ifnot,..)
static Entry*		current      = 0 ;   // working entry
//static Entry*		current_root = 0 ;   // parent of working entry


//static Entry*		previous     = 0 ;   // TODO: remove need for this
static bool             needNewEntry;

static QCString         g_sectionLabel;
static QCString		g_sectionTitle;
static int              g_sectionLevel;
static QCString         xrefItemKey;
static QCString         newXRefItemKey;
static QCString         xrefItemTitle;
static QCString         xrefListTitle;
static Protection	protection;

static bool             xrefAppendFlag;
static bool             inGroupParamFound;
static int              braceCount;
static bool             insidePre;
static bool             parseMore;
static int              g_condCount;

static int              g_commentCount;
static QCString         g_spaceBeforeCmd;
static QCString         g_spaceBeforeIf;
static QCString         g_copyDocArg;

static QCString         g_guardExpr;
static int              g_roundCount;

static bool             g_insideParBlock;

//-----------------------------------------------------------------------------

static void initParser()
{
  g_sectionLabel.resize(0);
  g_sectionTitle.resize(0);
  Doxygen::docGroup.clearHeader();
  g_insideParBlock = FALSE;
}

//-----------------------------------------------------------------------------

static bool getDocSectionName(int s)
{
  switch(s)
  {
    case Entry::CLASSDOC_SEC:
    case Entry::STRUCTDOC_SEC:
    case Entry::UNIONDOC_SEC:
    case Entry::EXCEPTIONDOC_SEC:
    case Entry::NAMESPACEDOC_SEC:
    case Entry::PROTOCOLDOC_SEC:
    case Entry::CATEGORYDOC_SEC:
    case Entry::ENUMDOC_SEC:
    case Entry::PAGEDOC_SEC:
    case Entry::VARIABLEDOC_SEC:
    case Entry::MEMBERDOC_SEC:
    case Entry::OVERLOADDOC_SEC:
    case Entry::FILEDOC_SEC:
    case Entry::DEFINEDOC_SEC:
    case Entry::GROUPDOC_SEC:
    case Entry::MAINPAGEDOC_SEC:
    case Entry::PACKAGEDOC_SEC:
    case Entry::DIRDOC_SEC:
    case Entry::EXAMPLE_SEC:
    case Entry::MEMBERGRP_SEC:
      return TRUE;
    default: 
      return FALSE;
  }
}

//-----------------------------------------------------------------------------

static bool makeStructuralIndicator(Entry::Sections s)
{
  //printf("current->section=%x\n",current->section);
  if (getDocSectionName(current->section))
  {
    return TRUE;
  }
  else
  {
    needNewEntry = TRUE;
    current->section = s;
    current->fileName = yyFileName;
    current->startLine = yyLineNr;
    return FALSE;
  }
}

static void lineCount()
{
  for( const char* c = yytext ; *c ; ++c )
    yyLineNr += (*c == '\n') ;
}


static QCString stripQuotes(const char *s)
{
  QCString name;
  if (s==0 || *s==0) return name;
  name=s;
  if (name.at(0)=='"' && name.at(name.length()-1)=='"')
  {
    name=name.mid(1,name.length()-2);
  }
  return name;
}

//-----------------------------------------------------------------

static void addXRefItem(const char *listName,const char *itemTitle,
                        const char *listTitle,bool append)
{
  Entry *docEntry = current; // inBody && previous ? previous : current;
  if (listName==0) return;
  //printf("addXRefItem(%s,%s,%s,%d)\n",listName,itemTitle,listTitle,append);

  ListItemInfo *lii=0;
  RefList *refList = Doxygen::xrefLists->find(listName);
  if (refList==0) // new list
  {
    refList = new RefList(listName,listTitle,itemTitle);
    Doxygen::xrefLists->insert(listName,refList);
    //printf("new list!\n");
  }
  if (docEntry->sli)
  {
    QListIterator<ListItemInfo> slii(*docEntry->sli);
    for (slii.toLast();(lii=slii.current());--slii)
    {
      if (qstrcmp(lii->type,listName)==0) 
      {
        //printf("found %s lii->type=%s\n",listName,lii->type);
        break;
      }
    }
  }
  if (lii && append) // already found item of same type just before this one
  {
    //printf("listName=%s item id = %d existing\n",listName,lii->itemId);
    RefItem *item = refList->getRefItem(lii->itemId);
    ASSERT(item!=0);
    item->text += " <p>";
    item->text += outputXRef;
    //printf("%s: text +=%s\n",listName,item->text.data());
  }
  else // new item
  {
    int itemId  = refList->addRefItem();
    //printf("listName=%s item id = %d new current=%p\n",listName,itemId,current);

    // if we have already an item from the same list type (e.g. a second @todo)
    // in the same Entry (i.e. lii!=0) then we reuse its link anchor.
    char anchorLabel[1024];
    //sprintf(anchorLabel,"_%s%06d",listName,lii ? lii->itemId : itemId);
    sprintf(anchorLabel,"_%s%06d",listName,itemId);
    RefItem *item = refList->getRefItem(itemId);
    ASSERT(item!=0);
    item->text = outputXRef;
    item->listAnchor = anchorLabel;
    docEntry->addSpecialListItem(listName,itemId);
    QCString cmdString;
    cmdString.sprintf(" \\xrefitem %s %d.",listName,itemId);
    if (inBody)
    {
      docEntry->inbodyDocs += cmdString;
    }
    else
    {
      docEntry->doc += cmdString;
    }
    SectionInfo *si = Doxygen::sectionDict->find(anchorLabel);
    if (si)
    {
      if (si->lineNr != -1)
      {
        warn(listName,yyLineNr,"multiple use of section label '%s', (first occurrence: %s, line %d)",anchorLabel,si->fileName.data(),si->lineNr);
      }
      else
      {
        warn(listName,yyLineNr,"multiple use of section label '%s', (first occurrence: %s)",anchorLabel,si->fileName.data());
      }
    }
    else
    {
      si=new SectionInfo(listName,yyLineNr,anchorLabel,
                         g_sectionTitle,SectionInfo::Anchor,
                         g_sectionLevel);
      Doxygen::sectionDict->append(anchorLabel,si);
      docEntry->anchors->append(si);
    }
  }
  outputXRef.resize(0);
}

//-----------------------------------------------------------------------------

// Adds a formula text to the list/dictionary of formulas if it was
// not already added. Returns the label of the formula.
static QCString addFormula()
{
  QCString formLabel;
  QCString fText=formulaText.simplifyWhiteSpace();
  Formula *f=0;
  if ((f=Doxygen::formulaDict->find(fText))==0)
  {
    f = new Formula(fText);
    Doxygen::formulaList->append(f);
    Doxygen::formulaDict->insert(fText,f);
    formLabel.sprintf("\\form#%d",f->getId());
    Doxygen::formulaNameDict->insert(formLabel,f);
  }
  else
  {
    formLabel.sprintf("\\form#%d",f->getId());
  }
  int i;
  for (i=0;i<formulaNewLines;i++) formLabel+="@_fakenl"; // add fake newlines to
                                                         // keep the warnings 
                                                         // correctly aligned.
  return formLabel;
}

//-----------------------------------------------------------------------------

static void checkFormula();
//-----------------------------------------------------------------------------

static SectionInfo::SectionType sectionLevelToType(int level)
{
  if (level>=0 && level<5) return (SectionInfo::SectionType)level;
  return SectionInfo::Anchor;
}

static void addSection()
{
  SectionInfo *si = Doxygen::sectionDict->find(g_sectionLabel);
  if (si)
  {
    if (si->lineNr != -1)
    {
      warn(yyFileName,yyLineNr,"multiple use of section label '%s' while adding section, (first occurrence: %s, line %d)",g_sectionLabel.data(),si->fileName.data(),si->lineNr);
    }
    else
    {
      warn(yyFileName,yyLineNr,"multiple use of section label '%s' while adding section, (first occurrence: %s)",g_sectionLabel.data(),si->fileName.data());
    }
  }
  else
  {
    // create a new section element
    g_sectionTitle+=yytext;
    g_sectionTitle=g_sectionTitle.stripWhiteSpace();
    si = new SectionInfo(yyFileName,yyLineNr,g_sectionLabel,
                          g_sectionTitle,sectionLevelToType(g_sectionLevel),g_sectionLevel);

    // add section to this entry
    current->anchors->append(si);

    // add section to the global dictionary
    Doxygen::sectionDict->append(g_sectionLabel,si);
  }
}

//-----------------------------------------------------------------------------

static void addCite()
{
  QCString name=yytext;
  if (yytext[0] =='"')
  {
    name=yytext+1;
    name=name.left(yyleng-2);
  }
  Doxygen::citeDict->insert(name.data());
}

//-----------------------------------------------------------------------------

// strip trailing whitespace (excluding newlines) from string s
static void stripTrailingWhiteSpace(QCString &s)
{
  uint len = s.length();
  int i = (int)len-1;
  char c;
  while (i>=0 && ((c = s.at(i))==' ' || c=='\t' || c=='\r')) i--;
  if (i!=(int)len-1) 
  {
    s.resize(i+2); // string up to and including char at pos i and \0 terminator
  }
}

// selects the output to write to
static inline void setOutput(OutputContext ctx)
{
  bool xrefAppendToPrev = xrefAppendFlag;
  // determine append flag for the next item (i.e. the end of this item)
  xrefAppendFlag = !inBody &&
                   inContext==OutputXRef && ctx==OutputXRef && // two consecutive xref items
                   newXRefKind==xrefKind &&                    // of the same kind
                   (xrefKind!=XRef_Item || 
		    newXRefItemKey==xrefItemKey);              // with the same key if \xrefitem
  //printf("%d && %d && %d && (%d || %d)\n",
  //                 inContext==OutputXRef,
  //                 ctx==OutputXRef,
  //                 newXRefKind==xrefKind,
  //                 xrefKind!=XRef_Item,
  //	  	     newXRefItemKey==xrefItemKey);
      
  //printf("refKind=%d newXRefKind=%d xrefAppendToPrev=%d xrefAppendFlag=%d\n",
  //   	  xrefKind,newXRefKind,xrefAppendToPrev,xrefAppendFlag);

  //printf("setOutput(inContext=%d ctx=%d)\n",inContext,ctx);
  if (inContext==OutputXRef) // end of XRef section => add the item 
  {
    // See if we can append this new xref item to the previous one.
    // We know this at the start of the next item of the same
    // type and need to remember this until the end of that item.
    switch(xrefKind)
    {
      case XRef_Todo:
	addXRefItem("todo",
	            theTranslator->trTodo(),
	            theTranslator->trTodoList(),
		    xrefAppendToPrev
		   );
	break;
      case XRef_Test:
	addXRefItem("test",
	            theTranslator->trTest(),
		    theTranslator->trTestList(),
		    xrefAppendToPrev
		   );
	break;
      case XRef_Bug:
	addXRefItem("bug",
	            theTranslator->trBug(),
		    theTranslator->trBugList(),
		    xrefAppendToPrev
		   );
	break;
      case XRef_Deprecated:
	addXRefItem("deprecated",
	            theTranslator->trDeprecated(),
		    theTranslator->trDeprecatedList(),
		    xrefAppendToPrev
		   );
	break;
      case XRef_Item:  // user defined list
	addXRefItem(xrefItemKey,
	            xrefItemTitle,
		    xrefListTitle,
		    xrefAppendToPrev
		   );
	break;
      case XRef_None:
	ASSERT(0);
	break;
    }
  }
  xrefItemKey = newXRefItemKey;

  int oldContext = inContext;
  inContext = ctx;
  if (inContext!=OutputXRef && inBody) inContext=OutputInbody;
  switch(inContext)
  {
    case OutputDoc:
      if (oldContext!=inContext)
      {
        stripTrailingWhiteSpace(current->doc);
        if (current->docFile.isEmpty())
        {
          current->docFile = yyFileName;
          current->docLine = yyLineNr;
        }
      }
      pOutputString = &current->doc;
      break;
    case OutputBrief:
      if (oldContext!=inContext)
      {
	if (current->briefFile.isEmpty())
	{
	  current->briefFile = yyFileName;
	  current->briefLine = yyLineNr;
	}
      }
      if (current->brief.stripWhiteSpace().isEmpty()) // we only want one brief
	                                              // description even if multiple
	                                              // are given...
      {
        pOutputString = &current->brief;
      }
      else
      {
        if (!current->doc.isEmpty()) // when appending parts add a new line
        {
          current->doc += "\n";
        }
        pOutputString = &current->doc;
	inContext = OutputDoc; // need to switch to detailed docs, see bug 631380
      }
      break;
    case OutputXRef:
      pOutputString = &outputXRef;
      // first item found, so can't append to previous
      //xrefAppendFlag = FALSE;
      break;
    case OutputInbody:
      pOutputString = &current->inbodyDocs;
      break;
  }
}


static void addAnchor(const char *anchor)
{
  SectionInfo *si = Doxygen::sectionDict->find(anchor);
  if (si)
  {
    if (si->lineNr != -1)
    {
      warn(yyFileName,yyLineNr,"multiple use of section label '%s' while adding anchor, (first occurrence: %s, line %d)",anchor,si->fileName.data(),si->lineNr);
    }
    else
    {
      warn(yyFileName,yyLineNr,"multiple use of section label '%s' while adding anchor, (first occurrence: %s)",anchor,si->fileName.data());
    }
  }
  else
  {
    si = new SectionInfo(yyFileName,yyLineNr,anchor,0,SectionInfo::Anchor,0);
    Doxygen::sectionDict->append(anchor,si);
    current->anchors->append(si);
  }
}

// add a string to the output
static inline void addOutput(const char *s)
{
  //printf("addOutput(%s)\n",s);
  *pOutputString+=s;
}

// add a character to the output
static inline void addOutput(char c)
{
  *pOutputString+=c;
}

static void endBrief(bool addToOutput=TRUE)
{
  if (!current->brief.stripWhiteSpace().isEmpty())
  { // only go to the detailed description if we have
    // found some brief description and not just whitespace
    briefEndsAtDot=FALSE;
    setOutput(OutputDoc);
    if (addToOutput) addOutput(yytext);
  }
}

static void handleGuard(const QCString &expr);
/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int prevPosition=0;

static int yyread(char *buf,int max_size)
{
    prevPosition=inputPosition;
    int c=0;
    while( c < max_size && inputString[inputPosition] )
    {
	*buf = inputString[inputPosition++] ;
	//printf("%d (%c)\n",*buf,*buf);
	c++; buf++;
    }
    return c;
}

%}

       /* start command character */
CMD	  ("\\"|"@")
DCMD1     ("arg"|"attention"|"author"|"cite"|"code")
DCMD2     ("date"|"dot"|"msc"|"dotfile"|"example"|"startuml")
DCMD3     ("htmlinclude"|"htmlonly"|"image"|"include")
DCMD4     ("includelineno"|"internal"|"invariant")
DCMD5     ("latexinclude"|"latexonly"|"li"|"line"|"manonly"|"name")
DCMD6     ("note"|"par"|"paragraph"|"param"|"post")
DCMD7     ("pre"|"remarks"|(("relate"[sd])("also")?))
DCMD8     ("remarks"|("return"[s]?)|"retval"|"sa"|"section")
DCMD9     ("see"|"since"|"subsection"|"subsubsection")
DCMD10    ("throw"|"until"|"verbatim")
DCMD11    ("verbinclude"|"version"|"warning")
DETAILEDCMD {CMD}({DCMD1}|{DCMD2}|{DCMD3}|{DCMD4}|{DCMD5}|{DCMD6}|{DCMD7}|{DCMD8}|{DCMD9}|{DCMD10}|{DCMD11})
XREFCMD   {CMD}("bug"|"deprecated"|"test"|"todo"|"xrefitem")  
PRE       [pP][rR][eE]
TABLE	  [tT][aA][bB][lL][eE]
P	  [pP]
UL        [uU][lL]
OL	  [oO][lL]
DL	  [dD][lL]
IMG       [iI][mM][gG]
HR        [hH][rR]
PARA      [pP][aA][rR][aA]
CODE      [cC][oO][dD][eE]
CAPTION   [cC][aA][pP][tT][iI][oO][nN]
CENTER    [cC][eE][nN][tT][eE][rR]
DIV       [dD][iI][vV]
DETAILEDHTML {CENTER}|{DIV}|{PRE}|{UL}|{TABLE}|{OL}|{DL}|{P}|[Hh][1-6]|{IMG}|{HR}|{PARA}
DETAILEDHTMLOPT {CODE}
BN        [ \t\n\r]
BL        [ \t\r]*"\n" 
B         [ \t]
BS        ^(({B}*"//")?)(({B}*"*"+)?){B}*
ATTR      ({B}+[^>\n]*)?
DOCNL     "\n"|"\\_linebr"
LC        "\\"{B}*"\n"
NW	  [^a-z_A-Z0-9]
FILESCHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+@&#]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+@&#]
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]*"\"")
ID        "$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
LABELID   [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*
CITESCHAR [a-z_A-Z0-9\x80-\xFF\-\?]
CITEECHAR [a-z_A-Z0-9\x80-\xFF\-\+:\/\?]*
CITEID    {CITESCHAR}{CITEECHAR}*("."{CITESCHAR}{CITEECHAR}*)*|"\""{CITESCHAR}{CITEECHAR}*("."{CITESCHAR}{CITEECHAR}*)*"\""
SCOPEID   {ID}({ID}*{BN}*"::"{BN}*)*({ID}?)
SCOPENAME "$"?(({ID}?{BN}*("::"|"."){BN}*)*)((~{BN}*)?{ID})
TMPLSPEC  "<"{BN}*[^>]+{BN}*">"
MAILADDR   [a-z_A-Z0-9.+\-]+"@"[a-z_A-Z0-9\-]+("."[a-z_A-Z0-9\-]+)+[a-z_A-Z0-9\-]+ 
RCSTAG    "$"{ID}":"[^\n$]+"$"

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
%x	EnumDocArg1
%x	NameSpaceDocArg1
%x	PackageDocArg1
%x	GroupDocArg1
%x	GroupDocArg2
%x	SectionLabel
%x	SectionTitle
%x	SubpageLabel
%x	SubpageTitle
%x	FormatBlock
%x	LineParam
%x	GuardParam
%x	GuardParamEnd
%x	SkipGuardedSection
%x	SkipInternal
%x      NameParam
%x	InGroupParam
%x	FnParam
%x	OverloadParam
%x	InheritParam
%x	ExtendsParam
%x      ReadFormulaShort
%x	ReadFormulaLong
%x	AnchorLabel
%x      HtmlComment
%x      SkipLang
%x	CiteLabel
%x	CopyDoc
%x      GuardExpr
%x      CdataSection

%%

  /* What can happen in while parsing a comment block:
   *   commands (e.g. @page, or \page)
   *   escaped commands (e.g. @@page or \\page).
   *   formulas (e.g. \f$ \f[ \f{..)
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

<Comment>{CMD}{CMD}[a-z_A-Z]+{B}*	{ // escaped command
  					  addOutput(yytext);
  					}
<Comment>{CMD}{CMD}"~"[a-z_A-Z]*	{ // escaped command
  					  addOutput(yytext);
  					}
<Comment>{MAILADDR}			{ // mail address
  					  addOutput(yytext);
  					}
<Comment>"\""[^"\n]*"\""		{ // quoted text
                                          addOutput(yytext);
  					}
<Comment>("\\"[a-z_A-Z]+)+"\\"		{ // directory (or chain of commands!)
  					  addOutput(yytext);
  					}
<Comment>"<"{DETAILEDHTML}{ATTR}">"	{ // HTML command that ends a brief description
					  setOutput(OutputDoc);
					  // continue with the same input
                                          REJECT;
					}
<Comment>"<"{DETAILEDHTMLOPT}{ATTR}">"	{ // HTML command that ends a brief description
					  if (current->lang==SrcLangExt_CSharp)
                                          {
                                            setOutput(OutputDoc);
                                          }
					  // continue with the same input
                                          REJECT;
					}
<Comment>"<summary>"			{ // start of a .NET XML style brief description
					  setOutput(OutputBrief);
                                          addOutput(yytext);
  					}
<Comment>"<remarks>"            	{ // start of a .NET XML style detailed description
					  setOutput(OutputDoc);
                                          addOutput(yytext);
  					}
<Comment>"</summary>"	                { // start of a .NET XML style detailed description
					  setOutput(OutputBrief);
                                          addOutput(yytext);
					  setOutput(OutputDoc);
  					}
<Comment>"</remarks>"			{ // end of a brief or detailed description
                                          
					  setOutput(OutputDoc);
                                          addOutput(yytext);
  					}
<Comment>"<"{CAPTION}{ATTR}">"          {
                                          QCString tag=yytext;
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
                                                addAnchor(id);
                                              }
                                            }
                                          }
                                          addOutput(yytext);
                                        }
<Comment>"<"{PRE}{ATTR}">"              {
                                          insidePre=TRUE;
                                          addOutput(yytext);
                                        }
<Comment>"</"{PRE}">"                   {
                                          insidePre=FALSE;
                                          addOutput(yytext);
                                        }
<Comment>{RCSTAG}                       { // RCS tag which end a brief description
                                          setOutput(OutputDoc);
                                          REJECT;
                                        }
<Comment>"<!--"                         { 
                                          BEGIN(HtmlComment);
                                        }
<Comment>"<!\[CDATA\["                  {
                                          BEGIN(CdataSection);
                                        }
<Comment>{B}*{CMD}"endinternal"{B}*	{
                                          addOutput(" \\endinternal "); 
                                          if (!inInternalDocs)
  					    warn(yyFileName,yyLineNr,
                                               "found \\endinternal without matching \\internal"
                                              );
                                          inInternalDocs = FALSE;
  					}
<Comment>{B}*{CMD}[a-z_A-Z]+"{"[a-zA-Z_,:0-9\. ]*"}"{B}*  |
<Comment>{B}*{CMD}[a-z_A-Z]+{B}*	{ // potentially interesting command
                                          // the {B}* in the front was added for bug620924
                                          QCString fullMatch = QCString(yytext);
                                          int idx = fullMatch.find('{');
                                          /* handle `\f{` and `@f{` as special cases */
                                          if ((idx > 1) && (yytext[idx-1] == 'f') && (yytext[idx-2] == '\\' || yytext[idx-2] =='@')) REJECT;
                                          int idxEnd = fullMatch.find("}",idx+1);
                                          QCString cmdName;
                                          QCStringList optList;
                                          if (idx == -1) // no options
                                          {
                                            cmdName = QCString(yytext).stripWhiteSpace().data()+1; // to remove {CMD}
                                          }
                                          else // options present
                                          {
                                            cmdName = fullMatch.left(idx).stripWhiteSpace().data()+1; // to remove {CMD}
                                            QCString optStr = fullMatch.mid(idx+1,idxEnd-idx-1).stripWhiteSpace();
                                            optList = QCStringList::split(',',optStr);
                                          }
					  DocCmdMapper::Cmd *cmdPtr = DocCmdMapper::map(cmdName);
					  if (cmdPtr) // special action is required
					  {
                                            int i=0;
                                            while (yytext[i]==' ' || yytext[i]=='\t') i++;
					    g_spaceBeforeCmd = QCString(yytext).left(i);
					    if (cmdPtr->endsBrief && !(inContext==OutputXRef && cmdName=="parblock"))
					    {
					      briefEndsAtDot=FALSE;
					      // this command forces the end of brief description
					      setOutput(OutputDoc);
					    }
                                            //if (i>0) addOutput(QCString(yytext).left(i)); // removed for bug 689341
					    if (cmdPtr->func && cmdPtr->func(cmdName, optList))
					    {
					      // implicit split of the comment block into two
					      // entries. Restart the next block at the start
					      // of this command.
					      parseMore=TRUE;

					      // yuk, this is probably not very portable across lex implementations, 
					      // but we need to know the position in the input buffer where this 
					      // rule matched.
					      // for flex 2.5.33+ we should use YY_CURRENT_BUFFER_LVALUE
#if YY_FLEX_MAJOR_VERSION>=2 && (YY_FLEX_MINOR_VERSION>5 || (YY_FLEX_MINOR_VERSION==5 && YY_FLEX_SUBMINOR_VERSION>=33))
					      inputPosition=prevPosition + (int)(yy_bp - YY_CURRENT_BUFFER_LVALUE->yy_ch_buf);
#else
					      inputPosition=prevPosition + (int)(yy_bp - yy_current_buffer->yy_ch_buf);
#endif
					      yyterminate();
					    }
					    else if (cmdPtr->func==0)
					    {
					      // command without handler, to be processed
					      // later by parsedoc.cpp
					      addOutput(yytext);
					    }
					  }
					  else // command not relevant
					  {
					    addOutput(yytext);
					  }
  					}
<Comment>{B}*({CMD}{CMD})"f"[$\[{]	{ // escaped formula command
  					  addOutput(yytext);
  					}
<Comment>{B}*{CMD}"~"[a-z_A-Z-]*		{ // language switch command
                                          QCString langId = QString(yytext).stripWhiteSpace().data()+2;
			       	          if (!langId.isEmpty() &&
					      qstricmp(Config_getEnum(OUTPUT_LANGUAGE),langId)!=0)
				          { // enable language specific section
				            BEGIN(SkipLang);
				          }
  					}
<Comment>{B}*{CMD}"f{"[^}\n]+"}"("{"?)  { // start of a formula with custom environment
					  setOutput(OutputDoc);
					  formulaText="\\begin";
					  formulaEnv=QString(yytext).stripWhiteSpace().data()+2;
					  if (formulaEnv.at(formulaEnv.length()-1)=='{')
					  {
					    // remove trailing open brace
					    formulaEnv=formulaEnv.left(formulaEnv.length()-1);
					  }
					  formulaText+=formulaEnv;
					  formulaNewLines=0;
					  BEGIN(ReadFormulaLong);
  					}
<Comment>{B}*{CMD}"f$"			{ // start of a inline formula
					  formulaText="$";
					  formulaNewLines=0;
					  BEGIN(ReadFormulaShort);
  					}
<Comment>{B}*{CMD}"f["			{ // start of a block formula
					  setOutput(OutputDoc);
					  formulaText="\\[";
					  formulaNewLines=0;
					  BEGIN(ReadFormulaLong);
  					}
<Comment>{B}*{CMD}"{"                   { // begin of a group
                                          //langParser->handleGroupStartCommand(g_memberGroupHeader);
                                          Doxygen::docGroup.open(current,yyFileName,yyLineNr);
                                        }
<Comment>{B}*{CMD}"}"                   { // end of a group
                                          //langParser->handleGroupEndCommand();
                                          Doxygen::docGroup.close(current,yyFileName,yyLineNr,TRUE);
					  Doxygen::docGroup.clearHeader();
					  parseMore=TRUE;
                                          needNewEntry = TRUE;
#if YY_FLEX_MAJOR_VERSION>=2 && (YY_FLEX_MINOR_VERSION>5 || (YY_FLEX_MINOR_VERSION==5 && YY_FLEX_SUBMINOR_VERSION>=33))
				          inputPosition=prevPosition + (int)(yy_bp - YY_CURRENT_BUFFER_LVALUE->yy_ch_buf) + strlen(yytext);
#else
				          inputPosition=prevPosition + (int)(yy_bp - yy_current_buffer->yy_ch_buf) + strlen(yytext);
#endif
					  yyterminate();
                                        }
<Comment>{B}*{CMD}[$@\\&~<>#%]		{ // escaped character
  					  addOutput(yytext);
  					}
<Comment>[a-z_A-Z]+			{ // normal word
					  addOutput(yytext);
  					}
<Comment>^{B}*"."{B}*/\n                { // explicit end autolist: e.g "  ."
  				          addOutput(yytext); 
					}
<Comment>^{B}*[1-9][0-9]*"."{B}+        |
<Comment>^{B}*[*+]{B}+			{ // start of autolist
                                          if (!Doxygen::markdownSupport)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            if (inContext!=OutputXRef) 
                                            {
                                              briefEndsAtDot=FALSE;
                                              setOutput(OutputDoc);
                                            }
                                            addOutput(yytext); 
                                          }
  					}
<Comment>^{B}*"-"{B}+			{ // start of autolist
					  if (inContext!=OutputXRef) 
					  {
					    briefEndsAtDot=FALSE;
					    setOutput(OutputDoc);
					  }
  				          addOutput(yytext); 
  					}
<Comment>^{B}*([\-:|]{B}*)*("--"|"---")({B}*[\-:|])*{B}*/\n { // horizontal line (dashed)
  				          addOutput(yytext); 
                                        }
<Comment>{CMD}"---"                     { // escaped mdash
                                          addOutput(yytext);
                                        }
<Comment>{CMD}"--"                      { // escaped mdash
                                          addOutput(yytext);
                                        }
<Comment>"---"                          { // mdash
                                          addOutput(insidePre || Doxygen::markdownSupport ? yytext : "&mdash;");
                                        }
<Comment>"--"                           { // ndash
                                          addOutput(insidePre || Doxygen::markdownSupport ? yytext : "&ndash;");
                                        }
<Comment>"-#"{B}+                       { // numbered item
                                          if (inContext!=OutputXRef)
                                          {
                                            briefEndsAtDot=FALSE;
                                            setOutput(OutputDoc);
                                          }
                                          addOutput(yytext);
                                        }
<Comment>("."+)[a-z_A-Z0-9\)]		{ // . at start or in the middle of a word, or ellipsis
  					  addOutput(yytext);
  					}
<Comment>".\\"[ \t]			{ // . with escaped space.
  					  addOutput(yytext[0]);
  					  addOutput(yytext[2]);
  					}
<Comment>".,"				{ // . with comma such as "e.g.," 
  					  addOutput(yytext);
  					}
<Comment>"...\\"[ \t]			{ // ellipsis with escaped space.
  					  addOutput("... ");
  					}
<Comment>".."[\.]?/[^ \t\n] 		{ // internal ellipsis 
  					  addOutput(yytext);
  					}
<Comment>(\n|\\_linebr)({B}*(\n|\\_linebr))+	{ // at least one blank line (or blank line command)
					  if (inContext==OutputXRef)
					  {
					    // see bug 613024, we need to put the newlines after ending the XRef section.
					    if (!g_insideParBlock) setOutput(OutputDoc);
                                            int i;
                                            for (i=0;i<yyleng;)
                                            {
                                              if (yytext[i]=='\n') addOutput('\n'),i++;
                                              else if (strcmp(yytext+i,"\\_linebr")==0) addOutput('\n'),i+=8;
                                              else i++;
                                            }
					  }
					  else if (inContext!=OutputBrief)
					  {
                                            int i;
                                            for (i=0;i<yyleng;)
                                            {
                                              if (yytext[i]=='\n') addOutput('\n'),i++;
                                              else if (strcmp(yytext+i,"\\_linebr")==0) addOutput('\n'),i+=8;
                                              else i++;
                                            }
					    setOutput(OutputDoc);
					  }
					  else // inContext==OutputBrief
					  { // only go to the detailed description if we have
					    // found some brief description and not just whitespace
					    endBrief(FALSE);
					  }
  					  lineCount();
  					}
<Comment>"."				{ // potential end of a JavaDoc style comment
  					  addOutput(*yytext);
  					  if (briefEndsAtDot)
					  {
					    setOutput(OutputDoc);
					    briefEndsAtDot=FALSE;
					  }
  					}
<Comment>\n				{ // newline
  					  addOutput(*yytext);
  					  yyLineNr++;
  					}
<Comment>.				{ // catch-all for anything else
  					  addOutput(*yytext);
  					}


 /* --------------   Rules for handling HTML comments ----------- */

<HtmlComment>"--"[!]?">"{B}*		{ BEGIN( Comment ); }
<HtmlComment>{DOCNL}			{ 
  					  if (*yytext=='\n') yyLineNr++;
  					}
<HtmlComment>[^\\\n\-]+			{ // ignore unimportant characters
  					}
<HtmlComment>.				{ // ignore every else
  					}

<CdataSection>"\]\]>"                   {
                                          BEGIN( Comment );
                                        }
<CdataSection>{DOCNL}                   { 
                                          addOutput('\n');
                                          if (*yytext=='\n') yyLineNr++;
                                        }
<CdataSection>[<>&]                     { // the special XML characters for iwhich the CDATA section is especially used
                                          addOutput('\\');
                                          addOutput(*yytext);
                                        }
<CdataSection>[^\\\n\]<>&]+             {
                                          addOutput(yytext);
                                        }
<CdataSection>.                         { 
                                          addOutput(*yytext);
                                        }

 /* --------------   Rules for handling formulas ---------------- */
 
<ReadFormulaShort>{CMD}"f$"		{ // end of inline formula
  					  formulaText+="$";
					  addOutput(" "+addFormula());
					  BEGIN(Comment);
  					}
<ReadFormulaLong>{CMD}"f]"		{ // end of block formula
					  formulaText+="\\]";
					  addOutput(" "+addFormula());
					  BEGIN(Comment);
  					}
<ReadFormulaLong>{CMD}"f}"		{ // end of custom env formula
					  formulaText+="\\end";
					  formulaText+=formulaEnv;
					  addOutput(" "+addFormula());
					  BEGIN(Comment);
  					}
<ReadFormulaLong,ReadFormulaShort>[^\\@\n]+ { // any non-special character
                                          formulaText+=yytext; 
 					} 
<ReadFormulaLong,ReadFormulaShort>\n	{ // new line
                                          formulaNewLines++;
                                          formulaText+=*yytext; 
					  yyLineNr++; 
					}
<ReadFormulaLong,ReadFormulaShort>.     { // any other character
                                          formulaText+=*yytext; 
					}

  /* ------------ handle argument of enum command --------------- */

<EnumDocArg1>{SCOPEID}			{ // handle argument
  					  current->name = yytext;
					  BEGIN( Comment );
  					}
<EnumDocArg1>{LC}			{ // line continuation
  					  yyLineNr++;
					  addOutput('\n');
                                        }
<EnumDocArg1>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
                                               "missing argument after \\enum."
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<EnumDocArg1>.				{ // ignore other stuff
  					}

  /* ------------ handle argument of namespace command --------------- */

<NameSpaceDocArg1>{SCOPENAME}		{ // handle argument
  					  current->name = substitute(yytext,".","::");
					  BEGIN( Comment );
  					}
<NameSpaceDocArg1>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<NameSpaceDocArg1>{DOCNL}		{ // missing argument
  					  warn(yyFileName,yyLineNr,
                                               "missing argument after "
					       "\\namespace."
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<NameSpaceDocArg1>.			{ // ignore other stuff
  					}

  /* ------------ handle argument of package command --------------- */

<PackageDocArg1>{ID}("."{ID})*		{ // handle argument
  					  current->name = yytext;
					  BEGIN( Comment );
  					}
<PackageDocArg1>{LC}			{ // line continuation 
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<PackageDocArg1>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
                                               "missing argument after "
					       "\\package."
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<PackageDocArg1>.			{ // ignore other stuff
  					}

  /* ------ handle argument of class/struct/union command --------------- */

<ClassDocArg1>{SCOPENAME}{TMPLSPEC}     { 
  					  current->name = substitute(removeRedundantWhiteSpace(yytext),".","::");
					  BEGIN( ClassDocArg2 );
                                        }
<ClassDocArg1>{SCOPENAME}		{ // first argument
  					  current->name = substitute(yytext,".","::");
					  if (current->section==Entry::PROTOCOLDOC_SEC)
					  {
					    current->name+="-p";
					  }
					  // prepend outer scope name 
					  BEGIN( ClassDocArg2 );
					}
<CategoryDocArg1>{SCOPENAME}{B}*"("[^\)]+")" {
  					  current->name = substitute(yytext,".","::");
					  BEGIN( ClassDocArg2 );
   					}
<ClassDocArg1,CategoryDocArg1>{LC}      { // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<ClassDocArg1,CategoryDocArg1>{DOCNL}	{
  					  warn(yyFileName,yyLineNr,
                                               "missing argument after "
					       "\\%s.",YY_START==ClassDocArg1?"class":"category"
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<ClassDocArg1,CategoryDocArg1>.		{ // ignore other stuff
  					}

<ClassDocArg2>{FILE}|"<>"		{ // second argument; include file
					  current->includeFile = yytext;
					  BEGIN( ClassDocArg3 );
					}
<ClassDocArg2>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<ClassDocArg2>{DOCNL}			{ 
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
					}
<ClassDocArg2>.				{ // ignore other stuff
  					}

<ClassDocArg3>[<"]?{FILE}?[">]?		{ // third argument; include file name
 					  current->includeName = yytext;
					  BEGIN( Comment );
					}
<ClassDocArg3>{LC}			{ // line continuation
                                          yyLineNr++;
					  addOutput('\n');
                                        }
<ClassDocArg3>{DOCNL}			{ 
  					  if (*yytext=='\n') yyLineNr++;
  					  BEGIN( Comment );
					}
<ClassDocArg3>.				{ // ignore other stuff
  					}

  /* --------- handle arguments of {def,add,weak}group commands --------- */

<GroupDocArg1>{LABELID}(".html"?)	{ // group name
  					  current->name = yytext;
					  //lastDefGroup.groupname = yytext;
					  //lastDefGroup.pri = current->groupingPri();
  					  // the .html stuff is for Qt compatibility
					  if (current->name.right(5)==".html") 
					  {
					    current->name=current->name.left(current->name.length()-5);
					  }
					  current->type.resize(0);
					  BEGIN(GroupDocArg2);
  					}
<GroupDocArg1>"\\"{B}*"\n"		{ // line continuation
  					  yyLineNr++; 
					  addOutput('\n');
                                        }
<GroupDocArg1>{DOCNL}			{ // missing argument!
  					  warn(yyFileName,yyLineNr,
                                               "missing group name after %s",
					       current->groupDocCmd()
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<GroupDocArg1>.				{ // ignore other stuff
  					}
<GroupDocArg2>"\\"{B}*"\n"		{ // line continuation
  					  yyLineNr++; 
					  addOutput('\n');
                                        }
<GroupDocArg2>[^\n\\]+			{ // title (stored in type)
					  current->type += yytext;
					  current->type = current->type.stripWhiteSpace();
                                        }
<GroupDocArg2>{DOCNL}			{
                                          if ( current->groupDocType==Entry::GROUPDOC_NORMAL &&
                                               current->type.isEmpty() 
					     ) // defgroup requires second argument
					  {
  					    warn(yyFileName,yyLineNr,
                                                 "missing title after "
					         "\\defgroup %s", current->name.data()
                                                );
					  }
  					  if (*yytext=='\n') yyLineNr++; 
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<GroupDocArg2>.                         { // title (stored in type)
					  current->type += yytext;
					  current->type = current->type.stripWhiteSpace();
                                        }

  /* --------- handle arguments of page/mainpage command ------------------- */

<PageDocArg1>{FILE}			{ // first argument; page name
					  current->name = stripQuotes(yytext);
					  current->args = "";
					  BEGIN( PageDocArg2 ); 
					}
<PageDocArg1>{LC}			{ yyLineNr++; 
					  addOutput('\n');
                                        }
<PageDocArg1>{DOCNL}			{
  					  warn(yyFileName,yyLineNr,
                                               "missing argument after "
					       "\\page."
                                              );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<PageDocArg1>.				{ // ignore other stuff
  					}
<PageDocArg2>{DOCNL}			{ // second argument; page title
					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
					}
<PageDocArg2>{CMD}[<>]       		{ 
					  // bug 748927
					  QCString tmp = yytext;
					  tmp = substitute(substitute(tmp,"@<","&lt;"),"@>","&gt;");
					  tmp = substitute(substitute(tmp,"\\<","&lt;"),"\\>","&gt;");
					  current->args += tmp;
					}
<PageDocArg2>.				{ 
					  current->args += yytext;
                                        }
  /* --------- handle arguments of the param command ------------ */
<ParamArg1>{ID}/{B}*","                 {
                                          addOutput(yytext);
                                        }
<ParamArg1>","                          {
                                          addOutput(" , ");
                                        }
<ParamArg1>{ID}                         {
                                          addOutput(yytext);
                                          BEGIN( Comment );
                                        }
<ParamArg1>.                            {
                                          unput(yytext[0]);
                                          BEGIN( Comment );
                                        }

  /* --------- handle arguments of the file/dir/example command ------------ */

<FileDocArg1>{DOCNL}			{ // no file name specified
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
					}
<FileDocArg1>{FILE}			{ // first argument; name
  					  current->name = stripQuotes(yytext);
					  BEGIN( Comment );
  				        }
<FileDocArg1>{LC}			{ yyLineNr++; 
					  addOutput('\n');
                                        }
<FileDocArg1>.				{ // ignore other stuff
  					}

  /* --------- handle arguments of the xrefitem command ------------ */

<XRefItemParam1>{LABELID}		{ // first argument
  					  newXRefItemKey=yytext;
                                          setOutput(OutputXRef);
					  BEGIN(XRefItemParam2);
					}
<XRefItemParam1>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<XRefItemParam1>{DOCNL}			{ // missing arguments
  					  warn(yyFileName,yyLineNr,
					       "Missing first argument of \\xrefitem"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  inContext = OutputDoc;
					  BEGIN( Comment );
  					}
<XRefItemParam1>.			{ // ignore other stuff 
  					}

<XRefItemParam2>"\""[^\n\"]*"\""	{ // second argument
  					  xrefItemTitle = stripQuotes(yytext);
					  BEGIN(XRefItemParam3);
  					}
<XRefItemParam2>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<XRefItemParam2>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "Missing second argument of \\xrefitem"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  inContext = OutputDoc;
					  BEGIN( Comment );
  					}
<XRefItemParam2>.			{ // ignore other stuff
  					}

<XRefItemParam3>"\""[^\n\"]*"\""	{ // third argument
  					  xrefListTitle = stripQuotes(yytext);
                                          xrefKind = XRef_Item;
					  BEGIN( Comment );
  					}
<XRefItemParam2,XRefItemParam3>{LC}	{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<XRefItemParam3>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "Missing third argument of \\xrefitem"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  inContext = OutputDoc;
					  BEGIN( Comment );
  					}
<XRefItemParam3>.			{ // ignore other stuff
  					}


  /* ----- handle arguments of the relates(also)/memberof command ------- */

<RelatesParam1>({ID}("::"|"."))*{ID}	{ // argument
  					  current->relates = yytext;
                                          //if (current->mGrpId!=DOX_NOGROUP) 
                                          //{
                                          //  memberGroupRelates = yytext;
                                          //}
					  BEGIN( Comment );
					}
<RelatesParam1>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<RelatesParam1>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "Missing argument of \\relates or \\memberof command"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<RelatesParam1>.			{ // ignore other stuff
  					}


  /* ----- handle arguments of the relates(also)/addindex commands ----- */

<LineParam>{DOCNL}			{ // end of argument
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<LineParam>{LC}				{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<LineParam>.				{ // ignore other stuff
  					  addOutput(*yytext);
  					}

  /* ----- handle arguments of the section/subsection/.. commands ------- */

<SectionLabel>{LABELID}			{ // first argument
  					  g_sectionLabel=yytext;
                                          addOutput(yytext);
					  g_sectionTitle.resize(0);
					  BEGIN(SectionTitle);
  					}
<SectionLabel>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "\\section command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<SectionLabel>.				{ // invalid character for section label
  					  warn(yyFileName,yyLineNr,
					      "Invalid or missing section label"
					      );
					  BEGIN(Comment);
  					}
<SectionTitle>[^\n@\\*]*/"\n"           { // end of section title
  					  addSection();
                                          addOutput(yytext);
					  BEGIN( Comment );
  					}
<SectionTitle>[^\n@\\]*/"\\_linebr"     { // end of section title
  					  addSection();
                                          addOutput(yytext);
					  BEGIN( Comment );
  					}
<SectionTitle>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<SectionTitle>[^\n@\\]*			{ // any character without special meaning
  					  g_sectionTitle+=yytext;
					  addOutput(yytext);
  					}
<SectionTitle>({CMD}{CMD}){ID}		{ // unescape escaped command
  					  g_sectionTitle+=&yytext[1];
					  addOutput(yytext);
  					}
<SectionTitle>{CMD}[$@\\&~<>#%]		{ // unescape escaped character
  					  g_sectionTitle+=yytext[1];
  					  addOutput(yytext);
  					}
<SectionTitle>.				{ // anything else
  					  g_sectionTitle+=yytext;
					  addOutput(*yytext);
  					}

  /* ----- handle arguments of the subpage command ------- */

<SubpageLabel>{LABELID}			{ // first argument
                                          addOutput(yytext);
					  // we add subpage labels as a kind of "inheritance" relation to prevent
					  // needing to add another list to the Entry class.
					  current->extends->append(new BaseInfo(yytext,Public,Normal));
					  BEGIN(SubpageTitle);
  					}
<SubpageLabel>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "\\subpage command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<SubpageTitle>{DOCNL}			{ // no title, end command
  					  addOutput(yytext);
					  BEGIN( Comment );
  					}
<SubpageTitle>[ \t]*"\""[^\"\n]*"\""	{ // add title, end of command
  					  addOutput(yytext);
					  BEGIN( Comment );
  					}
<SubpageTitle>.				{ // no title, end of command
  					  unput(*yytext);
					  BEGIN( Comment );
  					}

  /* ----- handle arguments of the anchor command ------- */

<AnchorLabel>{LABELID}			{ // found argument
                                          addAnchor(yytext);
					  addOutput(yytext);
					  BEGIN( Comment );
  					}
<AnchorLabel>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "\\anchor command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<AnchorLabel>.				{ // invalid character for anchor label
  					  warn(yyFileName,yyLineNr,
					      "Invalid or missing anchor label"
					      );
					  BEGIN(Comment);
  					}


  /* ----- handle arguments of the preformatted block commands ------- */

<FormatBlock>{CMD}("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"endmsc"|"endvhdlflow")/{NW} { // possible ends
  					  addOutput(yytext);
					  if (&yytext[4]==blockName) // found end of the block
					  {
  					    BEGIN(Comment);
					  }
  					}
<FormatBlock>{CMD}"enduml"              {
  					  addOutput(yytext);
					  if (blockName=="startuml") // found end of the block
					  {
  					    BEGIN(Comment);
					  }
                                        }
<FormatBlock>[^ \@\*\/\\\n]*		{ // some word
  					  addOutput(yytext);
  					}
<FormatBlock>{DOCNL}			{ // new line
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
  					}
<FormatBlock>"/*"			{ // start of a C-comment
					  if (!(blockName=="code" || blockName=="verbatim")) g_commentCount++;
  					  addOutput(yytext);
  					}
<FormatBlock>"*/"			{ // end of a C-comment
  					  addOutput(yytext);
					  if (!(blockName=="code" || blockName=="verbatim"))
                                          {
                                            g_commentCount--;
					    if (g_commentCount<0)
					    {
					      warn(yyFileName,yyLineNr,
					         "found */ without matching /* while inside a \\%s block! Perhaps a missing \\end%s?\n",blockName.data(),blockName.data());
					    }
  					  }
  					}
<FormatBlock>.				{
  					  addOutput(*yytext);
  					}
<FormatBlock><<EOF>>			{
                                          QCString endTag = "end"+blockName;
                                          if (blockName=="startuml") endTag="enduml";
                                          warn(yyFileName,yyLineNr,
	                                    "reached end of comment while inside a \\%s block; check for missing \\%s tag!",
	                                    blockName.data(),endTag.data()
	                                  );				
					  yyterminate();
    					}

  /* ----- handle arguments of if/ifnot commands ------- */

<GuardParam>{B}*"("                         {
                                          g_guardExpr=yytext;
                                          g_roundCount=1;
                                          BEGIN(GuardExpr);
                                        }
<GuardExpr>[^()]*                       {
                                          g_guardExpr+=yytext;
                                        }
<GuardExpr>"("                          {
                                          g_guardExpr+=yytext;
                                          g_roundCount++;
                                        }
<GuardExpr>")"                          {
                                          g_guardExpr+=yytext;
                                          g_roundCount--;
                                          if (g_roundCount==0)
                                          {
                                            handleGuard(g_guardExpr);
                                          }
                                        }
<GuardExpr>\n                           {
					  warn(yyFileName,yyLineNr,
						"invalid expression '%s' for guard",g_guardExpr.data());
                                          unput(*yytext);
                                          BEGIN(GuardParam);
                                        }
<GuardParam>{B}*[a-z_A-Z0-9.\-]+            { // parameter of if/ifnot guard
                                          handleGuard(yytext);
  					}
<GuardParam>{DOCNL}			{ // end of argument
  					  if (*yytext=='\n') yyLineNr++;
					  //next line is commented out due to bug620924
					  //addOutput('\n');
					  BEGIN( Comment );
  					}
<GuardParam>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<GuardParam>.				{ // ignore other stuff
  					  addOutput(*yytext);
  					}
<GuardParamEnd>{B}*{DOCNL}		{
  					  lineCount();
  					  g_spaceBeforeIf.resize(0);
  					  BEGIN(Comment);
  					}
<GuardParamEnd>{B}*			{
  					  if (!g_spaceBeforeIf.isEmpty()) // needed for 665313 in combation with bug620924
					  {
					    addOutput(g_spaceBeforeIf);
					  }
  					  g_spaceBeforeIf.resize(0);
  					  BEGIN(Comment);
  					}
<GuardParamEnd>.			{
  					  unput(*yytext);
  					  BEGIN(Comment);
  					}

  /* ----- handle skipping of conditional sections ------- */

<SkipGuardedSection>{CMD}"ifnot"/{NW}	{
                                          guardType = Guard_IfNot;
  					  BEGIN( GuardParam );
  					}
<SkipGuardedSection>{CMD}"if"/{NW}	{
                                          guardType = Guard_If;
  					  BEGIN( GuardParam );
  					}
<SkipGuardedSection>{CMD}"endif"/{NW}	{
  					  if (guards.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
						"found \\endif without matching start command");
					  }
					  else
					  {
					    GuardedSection *s = guards.pop();
                                            bool parentVisible = s->parentVisible();
                                            delete s;
                                            if (parentVisible)
                                            {
					      enabledSectionFound=TRUE;
					      BEGIN( GuardParamEnd );
                                            }
					  }
  					}
<SkipGuardedSection>{CMD}"else"/{NW}	{
  					  if (guards.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
						"found \\else without matching start command");
					  }
					  else
					  {
					    if (!enabledSectionFound && guards.top()->parentVisible())
					    {
					      delete guards.pop();
					      guards.push(new GuardedSection(TRUE,TRUE));
					      enabledSectionFound=TRUE;
					      BEGIN( GuardParamEnd );
					    }
					  }
  					}
<SkipGuardedSection>{CMD}"elseif"/{NW}  {
  					  if (guards.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
						"found \\elseif without matching start command");
					  }
					  else
					  {
					    if (!enabledSectionFound && guards.top()->parentVisible())
					    {
                                              guardType=Guard_If;
					      delete guards.pop();
					      BEGIN( GuardParam );
					    }
					  }
  					}
<SkipGuardedSection>{DOCNL}		{ // skip line
  					  if (*yytext=='\n') yyLineNr++;
					  //addOutput('\n');
  					}
<SkipGuardedSection>[^ \\@\n]+		{ // skip non-special characters
  					}
<SkipGuardedSection>.			{ // any other character
  					}


  /* ----- handle skipping of internal section ------- */

<SkipInternal>{DOCNL}			{ // skip line
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
  					}
<SkipInternal>[@\\]"if"/[ \t]		{
                                          g_condCount++;
  					}
<SkipInternal>[@\\]"ifnot"/[ \t]	{
                                          g_condCount++;
  					}
<SkipInternal>[@\\]/"endif"		{
                                          g_condCount--;
					  if (g_condCount<0) // handle conditional section around of \internal, see bug607743  
					  {
					    unput('\\');
					    BEGIN(Comment);
					  }
  					}
<SkipInternal>[@\\]/"section"[ \t]	{
  					  if (g_sectionLevel>0)
					  {
					    unput('\\');
					    BEGIN(Comment);
					  }
  					}
<SkipInternal>[@\\]/"subsection"[ \t]	{
  					  if (g_sectionLevel>1)
					  {
					    unput('\\');
					    BEGIN(Comment);
					  }
  					}
<SkipInternal>[@\\]/"subsubsection"[ \t]	{
  					  if (g_sectionLevel>2)
					  {
					    unput('\\');
					    BEGIN(Comment);
					  }
  					}
<SkipInternal>[@\\]/"paragraph"[ \t]	{
  					  if (g_sectionLevel>3)
					  {
					    unput('\\');
					    BEGIN(Comment);
					  }
  					}
<SkipInternal>[@\\]"endinternal"[ \t]*  {
					  BEGIN(Comment);
					}
<SkipInternal>[^ \\@\n]+		{ // skip non-special characters
  					}
<SkipInternal>.				{ // any other character
  					}


  /* ----- handle argument of name command ------- */

<NameParam>{DOCNL}			{ // end of argument
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<NameParam>{LC}				{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
					  Doxygen::docGroup.appendHeader(' ');
  					}
<NameParam>.				{ // ignore other stuff
					  Doxygen::docGroup.appendHeader(*yytext);
					  current->name+=*yytext;
  					}

  /* ----- handle argument of ingroup command ------- */

<InGroupParam>{LABELID}			{ // group id
  					  current->groups->append(
					    new Grouping(yytext, Grouping::GROUPING_INGROUP)
					  );
					  inGroupParamFound=TRUE;
  					}
<InGroupParam>{DOCNL}			{ // missing argument
  					  if (!inGroupParamFound)
					  {
  					    warn(yyFileName,yyLineNr,
					        "Missing group name for \\ingroup command"
					        );
					  }
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<InGroupParam>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<InGroupParam>.				{ // ignore other stuff
  					  addOutput(*yytext);
  					}

  /* ----- handle argument of fn command ------- */

<FnParam>{DOCNL}			{ // end of argument
  					  if (braceCount==0)
					  {
					    if (*yytext=='\n') yyLineNr++;
					    addOutput('\n');
					    langParser->parsePrototype(functionProto);
					    BEGIN( Comment );
					  }
  					}
<FnParam>{LC}				{ // line continuation
                                          yyLineNr++; 
					  functionProto+=' ';
  					}
<FnParam>[^@\\\n()]+			{ // non-special characters
                                          functionProto+=yytext;
  					}
<FnParam>"("				{
                                          functionProto+=yytext;
					  braceCount++;
					}
<FnParam>")"				{
                                          functionProto+=yytext;
					  braceCount--;
					}
<FnParam>.				{ // add other stuff
                                          functionProto+=*yytext;
  					}


  /* ----- handle argument of overload command ------- */


<OverloadParam>{DOCNL}			{ // end of argument
  					  if (*yytext=='\n') yyLineNr++;
					  if (functionProto.stripWhiteSpace().isEmpty())
					  { // plain overload command
					    addOutput(getOverloadDocs());
					    addOutput('\n');
					  }
					  else // overload declaration
					  {
                                            makeStructuralIndicator(Entry::OVERLOADDOC_SEC);
					    langParser->parsePrototype(functionProto);
					  }
					  BEGIN( Comment );
  					}
<OverloadParam>{LC}			{ // line continuation
                                          yyLineNr++; 
					  functionProto+=' ';
  					}
<OverloadParam>.			{ // add other stuff
                                          functionProto+=*yytext;
  					}

  /* ----- handle argument of inherit command ------- */

<InheritParam>({ID}("::"|"."))*{ID}	{ // found argument
					  current->extends->append(
					    new BaseInfo(removeRedundantWhiteSpace(yytext),Public,Normal)
					  );
					  BEGIN( Comment );
  					}
<InheritParam>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "\\inherit command has no argument"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<InheritParam>.				{ // invalid character for anchor label
  					  warn(yyFileName,yyLineNr,
					      "Invalid or missing name for \\inherit command"
					      );
					  BEGIN(Comment);
  					}

  /* ----- handle argument of extends and implements commands ------- */

<ExtendsParam>({ID}("::"|"."))*{ID}	{ // found argument
					  current->extends->append(
					    new BaseInfo(removeRedundantWhiteSpace(yytext),Public,Normal)
					  );
					  BEGIN( Comment );
  					}
<ExtendsParam>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "\\extends or \\implements command has no argument"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<ExtendsParam>.				{ // ignore other stuff
  					}

  /* ----- handle language specific sections ------- */

<SkipLang>[\\@]"~"[a-zA-Z-]*        { /* language switch */
                                     QCString langId = &yytext[2];
				     if (langId.isEmpty() ||
					 qstricmp(Config_getEnum(OUTPUT_LANGUAGE),langId)==0)
				     { // enable language specific section
				       BEGIN(Comment);
				     }
                                   }
<SkipLang>[^*@\\\n]*		   { /* any character not a *, @, backslash or new line */
                                   }
<SkipLang>{DOCNL}		   { /* new line in verbatim block */
  		                     if (*yytext=='\n') yyLineNr++;
                                   }
<SkipLang>.			   { /* any other character */
                                   }

  /* ----- handle arguments of the cite command ------- */

<CiteLabel>{CITEID}			{ // found argument
  					  addCite();
                                          addOutput(yytext);
					  BEGIN(Comment);
  					}
<CiteLabel>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "\\cite command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<CiteLabel>.				{ // invalid character for cite label
  					  warn(yyFileName,yyLineNr,
					      "Invalid or missing cite label"
					      );
					  BEGIN(Comment);
  					}

  /* ----- handle argument of the copydoc command ------- */

<CopyDoc><<EOF>>			|
<CopyDoc>{DOCNL}			{
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  setOutput(OutputDoc);
                                          addOutput(" \\copydetails ");
					  addOutput(g_copyDocArg);
					  addOutput("\n");
					  BEGIN(Comment);
  					}
<CopyDoc>[^\n\\]+			{
  					  g_copyDocArg+=yytext;
					  addOutput(yytext);
  					}
<CopyDoc>.				{
  					  g_copyDocArg+=yytext;
					  addOutput(yytext);
  					}


%%

//----------------------------------------------------------------------------

static bool handleBrief(const QCString &, const QCStringList &)
{
  //printf("handleBrief\n");
  setOutput(OutputBrief);
  return FALSE;
}

static bool handleFn(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::MEMBERDOC_SEC);
  functionProto.resize(0);
  braceCount=0;
  BEGIN(FnParam);
  return stop;
}

static bool handleDef(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::DEFINEDOC_SEC);
  functionProto.resize(0);
  BEGIN(FnParam);
  return stop;
}

static bool handleOverload(const QCString &, const QCStringList &)
{
  functionProto.resize(0);
  BEGIN(OverloadParam);
  return FALSE;
}

static bool handleEnum(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::ENUMDOC_SEC);
  BEGIN(EnumDocArg1);
  return stop;
}

static bool handleDefGroup(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_NORMAL;
  setOutput(OutputBrief);
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleAddToGroup(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_ADD;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleWeakGroup(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_WEAK;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleNamespace(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::NAMESPACEDOC_SEC);
  BEGIN( NameSpaceDocArg1 );
  return stop;
}

static bool handlePackage(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::PACKAGEDOC_SEC);
  BEGIN( PackageDocArg1 );
  return stop;
}

static bool handleClass(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::CLASSDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleHeaderFile(const QCString &, const QCStringList &)
{
  BEGIN( ClassDocArg2 );
  return FALSE;
}

static bool handleProtocol(const QCString &, const QCStringList &)
{ // Obj-C protocol
  bool stop=makeStructuralIndicator(Entry::PROTOCOLDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleCategory(const QCString &, const QCStringList &)
{ // Obj-C category
  bool stop=makeStructuralIndicator(Entry::CATEGORYDOC_SEC);
  BEGIN( CategoryDocArg1 ); 
  return stop;
}

static bool handleUnion(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::UNIONDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleStruct(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::STRUCTDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleInterface(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::INTERFACEDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleIdlException(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::EXCEPTIONDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handlePage(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::PAGEDOC_SEC);
  BEGIN( PageDocArg1 );
  return stop;
}

static bool handleMainpage(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::MAINPAGEDOC_SEC);
  if (!stop) 
  {
    current->name = "mainpage";
  }
  current->name = "";
  BEGIN( PageDocArg2 );
  return stop;
}

static bool handleFile(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::FILEDOC_SEC);
  if (!stop) 
  {
    current->name = yyFileName;
  }
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleParam(const QCString &, const QCStringList &)
{
  // we need process param and retval arguments to escape leading underscores in case of
  // markdown processing, see bug775493
  addOutput("@param ");
  BEGIN( ParamArg1 );
  return FALSE;
}

static bool handleRetval(const QCString &, const QCStringList &)
{
  addOutput("@retval ");
  BEGIN( ParamArg1 );
  return FALSE;
}

static bool handleDir(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::DIRDOC_SEC);
  if (!stop) current->name = yyFileName;
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleExample(const QCString &cmd, const QCStringList &optList)
{
  Entry::Sections section=Entry::EXAMPLE_SEC;
  QCStringList::ConstIterator it;
  for ( it = optList.begin(); it != optList.end(); ++it )
  {
    QCString opt = (*it).stripWhiteSpace().lower();
    if (opt=="lineno")
    {
      section=Entry::EXAMPLE_LINENO_SEC;
    }
    else
    {
      warn(yyFileName,yyLineNr,
  	"unsupported option '%s' for command '\\%s'",qPrint(opt),qPrint(cmd));
    }
  }
  bool stop=makeStructuralIndicator(section);
  if (!stop) current->name = yyFileName;
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleDetails(const QCString &, const QCStringList &)
{
  if (inContext!=OutputBrief)
  {
    addOutput("\n\n"); // treat @details outside brief description
                       // as a new paragraph
  }
  setOutput(OutputDoc);
  return FALSE;
}

static bool handleName(const QCString &, const QCStringList &)
{
  bool stop=makeStructuralIndicator(Entry::MEMBERGRP_SEC);
  if (!stop)
  {
    Doxygen::docGroup.clearHeader();
    BEGIN( NameParam );
    if (!Doxygen::docGroup.isEmpty()) // end of previous member group
    {
      Doxygen::docGroup.close(current,yyFileName,yyLineNr,TRUE,true);
    }
  }
  return stop;
}

static bool handleTodo(const QCString &, const QCStringList &)
{
  newXRefKind = XRef_Todo;
  setOutput(OutputXRef);
  xrefKind = XRef_Todo;
  return FALSE;
}

static bool handleTest(const QCString &, const QCStringList &)
{
  newXRefKind = XRef_Test;
  setOutput(OutputXRef);
  xrefKind = XRef_Test;
  return FALSE;
}

static bool handleBug(const QCString &, const QCStringList &)
{
  newXRefKind = XRef_Bug;
  setOutput(OutputXRef);
  xrefKind = XRef_Bug;
  return FALSE;
}

static bool handleDeprecated(const QCString &, const QCStringList &)
{
  newXRefKind = XRef_Deprecated;
  setOutput(OutputXRef);
  xrefKind = XRef_Deprecated;
  return FALSE;
}

static bool handleXRefItem(const QCString &, const QCStringList &)
{
  newXRefKind = XRef_Item;
  BEGIN(XRefItemParam1);
  return FALSE;
}

static bool handleParBlock(const QCString &, const QCStringList &)
{
  if (g_insideParBlock)
  {
    warn(yyFileName,yyLineNr,
	"found \\parblock command while already in a parblock!");
  }
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  addOutput("@parblock ");
  g_insideParBlock = TRUE;
  return FALSE;
}

static bool handleEndParBlock(const QCString &, const QCStringList &)
{
  if (!g_insideParBlock)
  {
    warn(yyFileName,yyLineNr,
	"found \\endparblock command without matching \\parblock!");
  }
  addOutput("@endparblock");
  setOutput(OutputDoc); // to end a parblock inside a xrefitem like context
  g_insideParBlock = FALSE;
  return FALSE;
}

static bool handleRelated(const QCString &, const QCStringList &)
{
  if (!current->relates.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"found multiple \\relates, \\relatesalso or \\memberof commands in a comment block, using last definition");
  }
  current->relatesType = Simple;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleRelatedAlso(const QCString &, const QCStringList &)
{
  if (!current->relates.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"found multiple \\relates, \\relatesalso or \\memberof commands in a comment block, using last definition");
  }
  current->relatesType = Duplicate;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleMemberOf(const QCString &, const QCStringList &)
{
  if (!current->relates.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"found multiple \\relates, \\relatesalso or \\memberof commands in a comment block, using last definition");
  }
  current->relatesType = MemberOf;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleRefItem(const QCString &, const QCStringList &)
{
  addOutput("@refitem ");
  BEGIN(LineParam);
  return FALSE;
}

static bool handleSection(const QCString &s, const QCStringList &)
{
  setOutput(OutputDoc);
  addOutput("@"+s+" ");
  BEGIN(SectionLabel);
  if      (s=="section")       g_sectionLevel=1;
  else if (s=="subsection")    g_sectionLevel=2;
  else if (s=="subsubsection") g_sectionLevel=3;
  else if (s=="paragraph")     g_sectionLevel=4;
  return FALSE;
}

static bool handleSubpage(const QCString &s, const QCStringList &)
{
  if (current->section!=Entry::EMPTY_SEC && 
      current->section!=Entry::PAGEDOC_SEC &&
      current->section!=Entry::MAINPAGEDOC_SEC
     )
  {
    warn(yyFileName,yyLineNr,
	"found \\subpage command in a comment block that is not marked as a page!");
  }
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  addOutput("@"+s+" ");
  BEGIN(SubpageLabel);
  return FALSE;
}

static bool handleAnchor(const QCString &s, const QCStringList &)
{
  addOutput("@"+s+" ");
  BEGIN(AnchorLabel);
  return FALSE;
}

static bool handleCite(const QCString &s, const QCStringList &)
{
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  addOutput("@"+s+" ");
  BEGIN(CiteLabel);
  return FALSE;
}

static bool handleFormatBlock(const QCString &s, const QCStringList &optList)
{
  if (optList.isEmpty())
  {
    addOutput("@"+s+" ");
  }
  else
  {
    addOutput("@"+s+"{"+optList.join(",")+"} ");
  }
  //printf("handleFormatBlock(%s) with option(%s)\n",s.data(),opt.data());
  blockName=s;
  g_commentCount=0;
  BEGIN(FormatBlock);
  return FALSE;
}

static bool handleAddIndex(const QCString &, const QCStringList &)
{
  addOutput("@addindex ");
  BEGIN(LineParam);
  return FALSE;
}

static bool handleIf(const QCString &, const QCStringList &)
{
  enabledSectionFound=FALSE;
  guardType = Guard_If;
  g_spaceBeforeIf = g_spaceBeforeCmd;
  BEGIN(GuardParam);
  return FALSE;
}

static bool handleIfNot(const QCString &, const QCStringList &)
{
  enabledSectionFound=FALSE;
  guardType = Guard_IfNot;
  g_spaceBeforeIf = g_spaceBeforeCmd;
  BEGIN(GuardParam);
  return FALSE;
}

static bool handleElseIf(const QCString &, const QCStringList &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"found \\else without matching start command");
  }
  else
  {
    guardType = enabledSectionFound ? Guard_Skip : Guard_If;
    BEGIN(GuardParam);
  }
  return FALSE;
}

static bool handleElse(const QCString &, const QCStringList &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"found \\else without matching start command");
  }
  else
  {
    BEGIN( SkipGuardedSection );
  }
  return FALSE;
}

static bool handleEndIf(const QCString &, const QCStringList &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"found \\endif without matching start command");
  }
  else
  {
    delete guards.pop();
  }
  enabledSectionFound=FALSE;
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  BEGIN( GuardParamEnd );
  return FALSE;
}

static bool handleIngroup(const QCString &, const QCStringList &)
{
  inGroupParamFound=FALSE;
  BEGIN( InGroupParam );
  return FALSE;
}

static bool handleNoSubGrouping(const QCString &, const QCStringList &)
{
  current->subGrouping = FALSE; 
  return FALSE;
}

static bool handleShowInitializer(const QCString &, const QCStringList &)
{
  current->initLines = 100000; // ON
  return FALSE;
}

static bool handleHideInitializer(const QCString &, const QCStringList &)
{
  current->initLines = 0; // OFF
  return FALSE;
}

static bool handleCallgraph(const QCString &, const QCStringList &)
{
  current->callGraph = TRUE; // ON
  return FALSE;
}

static bool handleHideCallgraph(const QCString &, const QCStringList &)
{
  current->callGraph = FALSE; // OFF
  return FALSE;
}

static bool handleCallergraph(const QCString &, const QCStringList &)
{
  current->callerGraph = TRUE; // ON
  return FALSE;
}

static bool handleHideCallergraph(const QCString &, const QCStringList &)
{
  current->callerGraph = FALSE; // OFF
  return FALSE;
}

static bool handleReferencedByRelation(const QCString &, const QCStringList &)
{
  current->referencedByRelation = TRUE; // ON
  return FALSE;
}

static bool handleHideReferencedByRelation(const QCString &, const QCStringList &)
{
  current->referencedByRelation = FALSE; // OFF
  return FALSE;
}

static bool handleReferencesRelation(const QCString &, const QCStringList &)
{
  current->referencesRelation = TRUE; // ON
  return FALSE;
}

static bool handleHideReferencesRelation(const QCString &, const QCStringList &)
{
  current->referencesRelation = FALSE; // OFF
  return FALSE;
}

static bool handleInternal(const QCString &, const QCStringList &)
{
  if (!Config_getBool(INTERNAL_DOCS))
  {
    // make sure some whitespace before a \internal command
    // is not treated as "documentation"
    if (current->doc.stripWhiteSpace().isEmpty())
    { 
      current->doc.resize(0);
    }
    g_condCount=0;
    BEGIN( SkipInternal );
  }
  else
  {
    // re-enabled for bug640828
    addOutput(" \\internal "); 
    inInternalDocs = TRUE;
  }
  return FALSE;
}

static bool handleLineBr(const QCString &, const QCStringList &)
{
  addOutput('\n');
  return FALSE;
}

static bool handleStatic(const QCString &, const QCStringList &)
{
  endBrief();
  current->stat = TRUE; 
  return FALSE;
}

static bool handlePure(const QCString &, const QCStringList &)
{
  endBrief();
  current->virt = Pure; 
  return FALSE;
}

static bool handlePrivate(const QCString &, const QCStringList &)
{
  current->protection = Private;
  return FALSE;
}

static bool handlePrivateSection(const QCString &, const QCStringList &)
{
  current->protection = protection = Private;
  return FALSE;
}

static bool handleProtected(const QCString &, const QCStringList &)
{
  current->protection = Protected;
  return FALSE;
}

static bool handleProtectedSection(const QCString &, const QCStringList &)
{
  current->protection = protection = Protected ;
  return FALSE;
}

static bool handlePublic(const QCString &, const QCStringList &)
{
  current->protection = Public;
  return FALSE;
}

static bool handlePublicSection(const QCString &, const QCStringList &)
{
  current->protection = protection = Public;
  return FALSE;
}

static bool handleToc(const QCString &, const QCStringList &optList)
{
  if (current->section==Entry::PAGEDOC_SEC || 
      current->section==Entry::MAINPAGEDOC_SEC)
  {
    QCStringList::ConstIterator it;
    for ( it = optList.begin(); it != optList.end(); ++it )
    {
      QCString opt = (*it).stripWhiteSpace().lower();
      char dum;
      int level = 5;
      int i = opt.find(':');
      if (i>0)  // found ':' but not on position 0 what would mean just a level
      {
        if (sscanf(opt.right(opt.length() - i - 1).data(),"%d%c",&level,&dum) != 1)
        {
          warn(yyFileName,yyLineNr,"Unknown option:level specified with \\tableofcontents: '%s'", (*it).stripWhiteSpace().data());
          opt = "";
        }
        else
        {
          level = (level > 5 ? 5 : level);
          level = (level <= 0 ? 5 : level);
          opt = opt.left(i).stripWhiteSpace();
        }
      }
      if (!opt.isEmpty())
      {
        if (opt == "html")
        {
          current->localToc.enableHtml(level);
        }
        else if (opt == "latex")
        {
          current->localToc.enableLatex(level);
        }
        else if (opt == "xml")
        {
          current->localToc.enableXml(level);
        }
        else if (opt == "docbook")
        {
          current->localToc.enableDocbook(level);
        }
        else
        {
          warn(yyFileName,yyLineNr,"Unknown option specified with \\tableofcontents: '%s'", (*it).stripWhiteSpace().data());
        }
      }
    }
    if (current->localToc.nothingEnabled())
    {
      // for backward compatibility
      current->localToc.enableHtml(5);
      current->localToc.enableXml(5);
    }
  }
  return FALSE;
}

static bool handleInherit(const QCString &, const QCStringList &)
{
  BEGIN(InheritParam);
  return FALSE;
}

static bool handleExtends(const QCString &, const QCStringList &)
{
  BEGIN(ExtendsParam);
  return FALSE;
}

static bool handleCopyBrief(const QCString &, const QCStringList &)
{
  if (current->brief.isEmpty() && current->doc.isEmpty())
  { // if we don't have a brief or detailed description yet,
    // then the @copybrief should end up in the brief description.
    // otherwise it will be copied inline (see bug691315 & bug700788)
    setOutput(OutputBrief);
  }
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  addOutput("\\copybrief ");
  return FALSE;
}

static bool handleCopyDetails(const QCString &, const QCStringList &)
{
  setOutput(OutputDoc);
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  addOutput("\\copydetails ");
  return FALSE;
}

static bool handleCopyDoc(const QCString &, const QCStringList &)
{
  setOutput(OutputBrief);
  if (!g_spaceBeforeCmd.isEmpty()) 
  {
    addOutput(g_spaceBeforeCmd);
    g_spaceBeforeCmd.resize(0);
  }
  addOutput("\\copybrief ");
  g_copyDocArg.resize(0);
  BEGIN(CopyDoc);
  return FALSE;
}

//----------------------------------------------------------------------------

static void checkFormula()
{
  if (YY_START==ReadFormulaShort || YY_START==ReadFormulaLong)
  {
    warn(yyFileName,yyLineNr,"End of comment block while inside formula.");
  }
}

//----------------------------------------------------------------------------

QCString preprocessCommentBlock(const QCString &comment,
                                const QCString &fileName,
                                int lineNr)
{
  if (!comment.isEmpty() && Doxygen::markdownSupport)
  {
    QCString result = processMarkdown(fileName,lineNr,0,comment);
    const char *p = result.data();
    if (p)
    {
      while (*p==' ')  p++; // skip over spaces
      while (*p=='\n') p++; // skip over newlines
      if (qstrncmp(p,"<br>",4)==0) p+=4; // skip over <br>
    }
    if (p>result.data())
    {
      // strip part of the input
      result = result.mid(p-result.data());
    }
    return result;
  }
  else
  {
    return comment;
  }
}

bool parseCommentBlock(/* in */     ParserInterface *parser,
                       /* in */     Entry *curEntry,
                       /* in */     const QCString &comment,
		       /* in */     const QCString &fileName,
		       /* in,out */ int  &lineNr,
		       /* in */     bool isBrief,
		       /* in */     bool isAutoBriefOn,
		       /* in */     bool isInbody,
		       /* in,out */ Protection &prot,
		       /* in,out */ int &position,
		       /* out */    bool &newEntryNeeded
		      )
{
  //printf("parseCommentBlock() isBrief=%d isAutoBriefOn=%d lineNr=%d\n",
  //    isBrief,isAutoBriefOn,lineNr);

  initParser();
  guards.setAutoDelete(TRUE);
  guards.clear();
  langParser     = parser;
  current        = curEntry;
  if (comment.isEmpty()) return FALSE; // avoid empty strings
  inputString    = comment;
  inputString.append(" ");
  inputPosition  = position;
  yyLineNr       = lineNr;
  yyFileName     = fileName;
  protection     = prot;
  needNewEntry   = FALSE;
  xrefKind       = XRef_None;
  xrefAppendFlag = FALSE;
  insidePre      = FALSE;
  parseMore      = FALSE;
  inBody         = isInbody;
  outputXRef.resize(0);
  if (!isBrief && !isAutoBriefOn && !current->doc.isEmpty())
  { // add newline separator between detailed comment blocks
    current->doc += '\n';
  }
  setOutput( isBrief || isAutoBriefOn ? OutputBrief : OutputDoc );
  briefEndsAtDot = isAutoBriefOn;
  g_condCount    = 0;
  g_sectionLevel = 0;
  g_spaceBeforeCmd.resize(0);
  g_spaceBeforeIf.resize(0);

  printlex(yy_flex_debug, TRUE, __FILE__, fileName ? fileName.data(): NULL);
  if (!current->inbodyDocs.isEmpty() && isInbody) // separate in body fragments
  {
    current->inbodyDocs+="\n\n";
  }

  Debug::print(Debug::CommentScan,0,"-----------\nCommentScanner: %s:%d\n"
               "input=[\n%s]\n",qPrint(fileName),lineNr,qPrint(inputString)
              );
  
  commentscanYYrestart( commentscanYYin );
  BEGIN( Comment );
  commentscanYYlex();
  setOutput( OutputDoc );

  if (YY_START==OverloadParam) // comment ended with \overload
  {
    addOutput(getOverloadDocs());
  }

  if (!guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,"Documentation block ended in the middle of a conditional section!");
  }

  if (g_insideParBlock)
  {
    warn(yyFileName,yyLineNr,
	"Documentation block ended while inside a \\parblock. Missing \\endparblock");
  }

  current->doc=stripLeadingAndTrailingEmptyLines(current->doc,current->docLine);

  if (current->section==Entry::FILEDOC_SEC && current->doc.isEmpty())
  {
    // to allow a comment block with just a @file command.
    current->doc="\n\n";
  }

  if (current->section==Entry::MEMBERGRP_SEC &&
      Doxygen::docGroup.isEmpty()) // @name section but no group started yet
  {
    Doxygen::docGroup.open(current,yyFileName,yyLineNr,true);
  }

  Debug::print(Debug::CommentScan,0,"-----------\nCommentScanner: %s:%d\noutput=[\n"
               "brief=[line=%d\n%s]\ndocs=[line=%d\n%s]\ninbody=[line=%d\n%s]\n]\n===========\n",
               qPrint(fileName),lineNr,
	       current->briefLine,qPrint(current->brief),
               current->docLine,qPrint(current->doc),
               current->inbodyLine,qPrint(current->inbodyDocs)
              );
  
  checkFormula();
  prot = protection;
  
  Doxygen::docGroup.addDocs(curEntry);

  newEntryNeeded = needNewEntry;

  // if we did not proceed during this call, it does not make
  // sense to continue, since we get stuck. See bug 567346 for situations
  // were this happens
  if (parseMore && position==inputPosition) parseMore=FALSE;

  if (parseMore) position=inputPosition; else position=0;

  lineNr = yyLineNr;
  //printf("position=%d parseMore=%d newEntryNeeded=%d\n",
  //  position,parseMore,newEntryNeeded);

  printlex(yy_flex_debug, FALSE, __FILE__, fileName ? fileName.data(): NULL);
  return parseMore;
}

static void handleGuard(const QCString &expr)
{
  CondParser prs;
  bool sectionEnabled=prs.parse(yyFileName,yyLineNr,expr.stripWhiteSpace());
  bool parentEnabled = TRUE;
  if (!guards.isEmpty()) parentEnabled = guards.top()->isEnabled();
  if (parentEnabled)
  {
    if (
        (sectionEnabled && guardType==Guard_If) ||  
        (!sectionEnabled && guardType==Guard_IfNot)
       ) // section is visible
    {
      guards.push(new GuardedSection(TRUE,TRUE));
      enabledSectionFound=TRUE;
      BEGIN( GuardParamEnd );
    }
    else // section is invisible
    {
      if (guardType!=Guard_Skip)
      {
        guards.push(new GuardedSection(FALSE,TRUE));
      }
      BEGIN( SkipGuardedSection );
    }
  }
  else // invisible because of parent
  {
    guards.push(new GuardedSection(FALSE,FALSE));
    BEGIN( SkipGuardedSection );
  }
}



#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void commentscanYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

