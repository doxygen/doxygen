/*****************************************************************************
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
  
%{

/*
 *	includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "qtbc.h"
#include <qarray.h>
#include <qstack.h>
#include <qregexp.h>
#include <unistd.h>
#include <qfile.h>
  
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

// forward declarations
static bool handleBrief(const QCString &);
static bool handleFn(const QCString &);
static bool handleDef(const QCString &);
static bool handleOverload(const QCString &);
static bool handleEnum(const QCString &);
static bool handleDefGroup(const QCString &);
static bool handleAddToGroup(const QCString &);
static bool handleWeakGroup(const QCString &);
static bool handleNamespace(const QCString &);
static bool handlePackage(const QCString &);
static bool handleClass(const QCString &);
static bool handleHeaderFile(const QCString &);
static bool handleProtocol(const QCString &);
static bool handleCategory(const QCString &);
static bool handleUnion(const QCString &);
static bool handleStruct(const QCString &);
static bool handleInterface(const QCString &);
static bool handleIdlException(const QCString &);
static bool handlePage(const QCString &);
static bool handleMainpage(const QCString &);
static bool handleFile(const QCString &);
static bool handleDir(const QCString &);
static bool handleExample(const QCString &);
static bool handleDetails(const QCString &);
static bool handleName(const QCString &);
static bool handleTodo(const QCString &);
static bool handleTest(const QCString &);
static bool handleBug(const QCString &);
static bool handleSubpage(const QCString &s);
static bool handleDeprecated(const QCString &);
static bool handleXRefItem(const QCString &);
static bool handleRelated(const QCString &);
static bool handleRelatedAlso(const QCString &);
static bool handleMemberOf(const QCString &);
static bool handleRefItem(const QCString &);
static bool handleSection(const QCString &);
static bool handleAnchor(const QCString &);
static bool handleFormatBlock(const QCString &);
static bool handleAddIndex(const QCString &);
static bool handleIf(const QCString &);
static bool handleIfNot(const QCString &);
static bool handleElseIf(const QCString &);
static bool handleElse(const QCString &);
static bool handleEndIf(const QCString &);
static bool handleIngroup(const QCString &);
static bool handleNoSubGrouping(const QCString &);
static bool handleShowInitializer(const QCString &);
static bool handleHideInitializer(const QCString &);
static bool handleCallgraph(const QCString &);
static bool handleCallergraph(const QCString &);
static bool handleInternal(const QCString &);
static bool handleLineBr(const QCString &);
static bool handleStatic(const QCString &);
static bool handlePure(const QCString &);
static bool handlePrivate(const QCString &);
static bool handlePrivateSection(const QCString &);
static bool handleProtected(const QCString &);
static bool handleProtectedSection(const QCString &);
static bool handlePublic(const QCString &s);
static bool handlePublicSection(const QCString &s);
static bool handleInherit(const QCString &);
static bool handleExtends(const QCString &);

typedef bool (*DocCmdFunc)(const QCString &name);

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
  { "fn",              &handleFn,               FALSE },
  { "var",             &handleFn,               FALSE },
  { "typedef",         &handleFn,               FALSE },
  { "property",        &handleFn,               FALSE },
  { "def",             &handleDef,              FALSE },
  { "overload",        &handleOverload,         FALSE },
  { "enum",            &handleEnum,             FALSE },
  { "defgroup",        &handleDefGroup,         FALSE },
  { "addtogroup",      &handleAddToGroup,       FALSE },
  { "weakgroup",       &handleWeakGroup,        FALSE },
  { "namespace",       &handleNamespace,        FALSE },
  { "package",         &handlePackage,          FALSE },
  { "class",           &handleClass,            FALSE },
  { "headerfile",      &handleHeaderFile,       FALSE },
  { "protocol",        &handleProtocol,         FALSE },
  { "category",        &handleCategory,         FALSE },
  { "union",           &handleUnion,            FALSE },
  { "struct",          &handleStruct,           FALSE },
  { "interface",       &handleInterface,        FALSE },
  { "idlexcept",       &handleIdlException,     FALSE },
  { "page",            &handlePage,             FALSE },
  { "mainpage",        &handleMainpage,         FALSE },
  { "file",            &handleFile,             FALSE },
  { "dir",             &handleDir,              FALSE },
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
  { "refitem",         &handleRefItem,          TRUE  },
  { "subpage",         &handleSubpage,          TRUE  },
  { "section",         &handleSection,          TRUE  },
  { "subsection",      &handleSection,          TRUE  },
  { "subsubsection",   &handleSection,          TRUE  },
  { "paragraph",       &handleSection,          TRUE  },
  { "anchor",          &handleAnchor,           TRUE  },
  { "verbatim",        &handleFormatBlock,      TRUE },
  { "latexonly",       &handleFormatBlock,      FALSE },
  { "htmlonly",        &handleFormatBlock,      FALSE },
  { "xmlonly",         &handleFormatBlock,      FALSE },
  { "rtfonly",         &handleFormatBlock,      FALSE },
  { "manonly",         &handleFormatBlock,      FALSE },
  { "dot",             &handleFormatBlock,      TRUE  },
  { "msc",             &handleFormatBlock,      TRUE  },
  { "code",            &handleFormatBlock,      TRUE  },
  { "addindex",        &handleAddIndex,         FALSE },
  { "if",              &handleIf,               FALSE },
  { "ifnot",           &handleIfNot,            FALSE },
  { "elseif",          &handleElseIf,           FALSE },
  { "else",            &handleElse,             FALSE },
  { "endif",           &handleEndIf,            FALSE },
  { "ingroup",         &handleIngroup,          FALSE },
  { "nosubgrouping",   &handleNoSubGrouping,    FALSE },
  { "showinitializer", &handleShowInitializer,  FALSE },
  { "hideinitializer", &handleHideInitializer,  FALSE },
  { "callgraph",       &handleCallgraph,        FALSE },
  { "callergraph",     &handleCallergraph,      FALSE },
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
  { "inherit",         &handleInherit,          TRUE  },
  { "extends",         &handleExtends,          TRUE  },
  { "implements",      &handleExtends,          TRUE  },
  { "memberof",        &handleMemberOf,         TRUE  },
  { "arg",             0,                       TRUE  },
  { "attention",       0,                       TRUE  },
  { "author",          0,                       TRUE  },
  { "authors",         0,                       TRUE  },
  { "copydoc",         0,                       TRUE  },
  { "copybrief",       0,                       FALSE },
  { "copydetails",     0,                       TRUE  },
  { "date",            0,                       TRUE  },
  { "dotfile",         0,                       TRUE  },
  { "htmlinclude",     0,                       FALSE },
  { "image",           0,                       TRUE  },
  { "include",         0,                       TRUE  },
  { "includelineno",   0,                       TRUE  },
  { "invariant",       0,                       TRUE  },
  { "li",              0,                       TRUE  },
  { "line",            0,                       TRUE  },
  { "note",            0,                       TRUE  },
  { "par",             0,                       TRUE  },
  { "param",           0,                       TRUE  },
  { "tparam",          0,                       TRUE  },
  { "post",            0,                       TRUE  },
  { "pre",             0,                       TRUE  },
  { "remark",          0,                       TRUE  },
  { "remarks",         0,                       TRUE  },
  { "result",          0,                       TRUE  },
  { "return",          0,                       TRUE  },
  { "returns",         0,                       TRUE  },
  { "retval",          0,                       TRUE  },
  { "sa",              0,                       TRUE  },
  { "see",             0,                       TRUE  },
  { "since",           0,                       TRUE  },
  { "throw",           0,                       TRUE  },
  { "throws",          0,                       TRUE  },
  { "until",           0,                       TRUE  },
  { "verbinclude",     0,                       FALSE },
  { "version",         0,                       TRUE  },
  { "warning",         0,                       TRUE  },
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
	  printf("Error: DocCmdMapper: command %s already added\n",p->cmdName);
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

void openGroup(Entry *e,const char *file,int line);
void closeGroup(Entry *e,const char *file,int line,bool foundInline=FALSE);
void initGroupInfo(Entry *e);
static void groupAddDocs(Entry *e,const char *fileName);

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

static QCString         sectionLabel;
static QCString		sectionTitle;
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
static int              g_sectionLevel;

static int              g_commentCount;

//-----------------------------------------------------------------------------

static QStack<Grouping> g_autoGroupStack;
static int              g_memberGroupId = DOX_NOGROUP;
static QCString         g_memberGroupHeader;
static QCString         g_memberGroupDocs;
static QCString         g_memberGroupRelates;
static QCString         g_compoundName;

//-----------------------------------------------------------------------------

static void initParser()
{
  sectionLabel.resize(0);
  sectionTitle.resize(0);
  g_memberGroupHeader.resize(0);
}

//-----------------------------------------------------------------------------

static QCString getDocSectionName(int s)
{
  switch(s)
  {
    case Entry::CLASSDOC_SEC:       return "\\class";
    case Entry::STRUCTDOC_SEC:      return "\\struct";
    case Entry::UNIONDOC_SEC:       return "\\union";
    case Entry::EXCEPTIONDOC_SEC:   return "\\exception";
    case Entry::NAMESPACEDOC_SEC:   return "\\namespace";
    case Entry::PROTOCOLDOC_SEC:    return "\\protocol";
    case Entry::CATEGORYDOC_SEC:    return "\\category";
    case Entry::ENUMDOC_SEC:        return "\\enum";
    case Entry::PAGEDOC_SEC:        return "\\page";
    case Entry::VARIABLEDOC_SEC:    return "\\var";
    case Entry::MEMBERDOC_SEC:      return "\\fn";
    case Entry::OVERLOADDOC_SEC:    return "\\overload";
    case Entry::FILEDOC_SEC:        return "\\file";
    case Entry::DEFINEDOC_SEC:      return "\\def";
    case Entry::GROUPDOC_SEC:       return "\\defgroup";
    case Entry::MAINPAGEDOC_SEC:    return "\\mainpage";
    case Entry::PACKAGEDOC_SEC:     return "\\package";
    case Entry::DIRDOC_SEC:         return "\\dir";
    case Entry::EXAMPLE_SEC:        return "\\example";
    case Entry::MEMBERGRP_SEC:      return "\\name";
    default: return "";
  }
}

//-----------------------------------------------------------------------------

static bool makeStructuralIndicator(Entry::Sections s)
{
  //printf("current->section=%x\n",current->section);
  if (!getDocSectionName(current->section).isEmpty())
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
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      if (strcmp(lii->type,listName)==0) 
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
    cmdString.sprintf("\\xrefitem %s %d.",listName,itemId);
    if (inBody)
    {
      docEntry->inbodyDocs += cmdString;
    }
    else
    {
      docEntry->doc += cmdString;
    }
    SectionInfo *si=new SectionInfo(listName,anchorLabel,
	                            sectionTitle,SectionInfo::Anchor);
    Doxygen::sectionDict.insert(anchorLabel,si);
    docEntry->anchors->append(si);
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
  if ((f=Doxygen::formulaDict[fText])==0)
  {
    f = new Formula(fText);
    Doxygen::formulaList.append(f);
    Doxygen::formulaDict.insert(fText,f);
    formLabel.sprintf("\\form#%d",f->getId());
    Doxygen::formulaNameDict.insert(formLabel,f);
  }
  else
  {
    formLabel.sprintf("\\form#%d",f->getId());
  }
  int i;
  for (i=0;i<formulaNewLines;i++) formLabel+="\\_fakenl"; // add fake newlines to
                                                          // keep the warnings 
                                                          // correctly aligned.
  return formLabel;
}

//-----------------------------------------------------------------------------

static void checkFormula();
//-----------------------------------------------------------------------------

static void addSection()
{
  sectionTitle+=yytext;
  sectionTitle=sectionTitle.stripWhiteSpace();
  //printf("Adding new section file=%s label=%s title=%s\n",yyFileName,sectionLabel.data(),sectionTitle.data()); 
  SectionInfo *si = new SectionInfo(yyFileName,sectionLabel,sectionTitle,SectionInfo::Anchor);
  current->anchors->append(si);
  Doxygen::sectionDict.insert(yytext,si);
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
    s.resize(i+2); // string upto and including char at pos i and \0 terminator
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

// add a string to the output
static inline void addOutput(const char *s)
{
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
DCMD1     ("arg"|"attention"|"author"|"code")
DCMD2     ("date"|"dot"|"msc"|"dotfile"|"example")
DCMD3     ("htmlinclude"|"htmlonly"|"image"|"include")
DCMD4     ("includelineno"|"internal"|"invariant")
DCMD5     ("latexonly"|"li"|"line"|"manonly"|"name") 
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
DETAILEDHTML {PRE}|{UL}|{TABLE}|{OL}|{DL}|{P}|[Hh][1-6]|{IMG}|{HR}
BN        [ \t\n\r]
BL        [ \t\r]*"\n" 
B         [ \t]
BS        ^(({B}*"//")?)(({B}*"*"+)?){B}*
ATTR      ({B}+[^>\n]*)?
DOCNL     "\n"|"\\_linebr"
LC        "\\"{B}*"\n"
NW	  [^a-z_A-Z0-9]
FILESCHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+]
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]*"\"")
ID        "$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
LABELID   [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*
SCOPEID   {ID}({ID}*{BN}*"::"{BN}*)*({ID}?)
SCOPENAME "$"?(({ID}?{BN}*("::"|"."){BN}*)*)((~{BN}*)?{ID})
MAILADR   [a-z_A-Z0-9.+\-]+"@"[a-z_A-Z0-9\-]+("."[a-z_A-Z0-9\-]+)+[a-z_A-Z0-9\-]+ 
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
   *   mail adress (e.g. dimitri@stack.nl).
   *   quoted text, such as "foo@bar"
   *   XML commands, <summary></summary><remarks></remarks>
   */

<Comment>{CMD}{CMD}[a-z_A-Z]+{B}*	{ // escaped command
  					  addOutput(yytext);
  					}
<Comment>{CMD}{CMD}"~"[a-z_A-Z]*	{ // escaped command
  					  addOutput(yytext);
  					}
<Comment>{MAILADR}			{ // mail adress
  					  addOutput(yytext);
  					}
<Comment>"\""[^"\n]*"\""		{ // quoted text
                                          addOutput(yytext);
  					}
<Comment>("\\"[a-z_A-Z]+)+"\\"		{ // directory (or chain of commands!)
  					  addOutput(yytext);
  					}
<Comment>{XREFCMD}/[^a-z_A-Z]*		{ // xref command 
					  if (inContext!=OutputXRef) 
					  {
					    briefEndsAtDot=FALSE;
					    setOutput(OutputDoc);
					  }
					  // continue with the same input
                                          REJECT;
					}
   /*
<Comment>{DETAILEDCMD}/[^a-z_A-Z]*	{ // command that can end a brief description
				          briefEndsAtDot=FALSE;
				          setOutput(OutputDoc);
					  // continue with the same input
                                          REJECT;
					}
   */
<Comment>"<"{DETAILEDHTML}{ATTR}">"	{ // HTML command that ends a brief description
					  setOutput(OutputDoc);
					  // continue with the same input
                                          REJECT;
					}
<Comment>"<summary>"			{ // start of a .NET XML style brief description
					  setOutput(OutputBrief);
  					}
<Comment>"<remarks>"|"</summary>"	{ // start of a .NET XML style detailed description
					  setOutput(OutputDoc);
  					}
<Comment>"</remarks>"			{ // end of a brief or detailed description
  					}
<Comment>{RCSTAG}			{ // RCS tag which end a brief description
  					  setOutput(OutputDoc);
					  REJECT;
  					}
<Comment>"<!--"				{ 
  					  BEGIN(HtmlComment);
					}
<Comment>{B}*{CMD}"endinternal"{B}*	{
  					  warn(yyFileName,yyLineNr,
                                               "warning: found \\endinternal without matching \\internal"
                                              );
  					}
<Comment>{B}*{CMD}[a-z_A-Z]+{B}*	{ // potentially interesting command
                                          // the {B}* in the front was added for bug620924
  					  QCString cmdName = QCString(yytext).stripWhiteSpace().data()+1;
					  DocCmdMapper::Cmd *cmdPtr = DocCmdMapper::map(cmdName);
					  if (cmdPtr) // special action is required
					  {
					    if (cmdPtr->endsBrief)
					    { 
					      briefEndsAtDot=FALSE;
					      // this command forces the end of brief description
					      setOutput(OutputDoc);
					    }
					    if (cmdPtr->func && cmdPtr->func(cmdName))
					    {
					      // implicit split of the comment block into two
					      // entries. Restart the next block at the start
					      // of this command.
					      parseMore=TRUE;

					      // yuk, this is probably not very portable across lex implementations, 
					      // but we need to know the position in the input buffer where this 
					      // rule matched.
					      // for flex 2.5.33+ we should use YY_CURRENT_BUFFER_LVALUE
#if YY_FLEX_MINOR_VERSION>=5 && YY_FLEX_SUBMINOR_VERSION>=33
					      inputPosition=prevPosition + yy_bp - YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
#else
					      inputPosition=prevPosition + yy_bp - yy_current_buffer->yy_ch_buf;
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
<Comment>{B}*("\\\\"|"@@")"f"[$\[{]	{ // escaped formula command
  					  addOutput(yytext);
  					}
<Comment>{B}*{CMD}"~"[a-z_A-Z]*		{ // language switch command
                                          QCString langId = QString(yytext).stripWhiteSpace().data()+2;
			       	          if (!langId.isEmpty() &&
					      stricmp(Config_getEnum("OUTPUT_LANGUAGE"),langId)!=0)
				          { // enable language specific section
				            BEGIN(SkipLang);
				          }
  					}
<Comment>{B}*{CMD}"f{"[^}\n]+"}"("{"?)  { // start of a formula with custom environment
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
					  formulaText="\\[";
					  formulaNewLines=0;
					  BEGIN(ReadFormulaLong);
  					}
<Comment>{B}*{CMD}"{"                   { // begin of a group
                                          //langParser->handleGroupStartCommand(g_memberGroupHeader);
                                          openGroup(current,yyFileName,yyLineNr);
                                        }
<Comment>{B}*{CMD}"}"                   { // end of a group
                                          //langParser->handleGroupEndCommand();
                                          closeGroup(current,yyFileName,yyLineNr,TRUE);
                                          g_memberGroupHeader.resize(0);
					  parseMore=TRUE;
                                          needNewEntry = TRUE;
#if YY_FLEX_MINOR_VERSION>=5 && YY_FLEX_SUBMINOR_VERSION>=33
				          inputPosition=prevPosition + yy_bp - YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + strlen(yytext);
#else
				          inputPosition=prevPosition + yy_bp - yy_current_buffer->yy_ch_buf + strlen(yytext);
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
<Comment>^{B}*"-"			{ // start of autolist
					  setOutput(OutputDoc);
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
					    setOutput(OutputDoc);
  					    addOutput("\n\n");
					  }
					  else if (inContext!=OutputBrief)
					  {
  					    addOutput("\n\n");
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
<ReadFormulaLong,ReadFormulaShort>.     { // any othe character
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
                                               "warning: missing argument after \\enum."
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
                                               "warning: missing argument after "
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
                                               "warning: missing argument after "
					       "\\package."
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<PackageDocArg1>.			{ // ignore other stuff
  					}

  /* ------ handle argument of class/struct/union command --------------- */

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
                                               "warning: missing argument after "
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

<ClassDocArg3>[<]?{FILE}?[>]?		{ // third argument; include file name
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
                                               "warning: missing group name after %s",
					       current->groupDocCmd()
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<GroupDocArg2>"\\"{B}*"\n"		{ // line continuation
  					  yyLineNr++; 
					  addOutput('\n');
                                        }
<GroupDocArg2>[^\n\\\*]+		{ // title (stored in type)
					  current->type += yytext;
					  current->type = current->type.stripWhiteSpace();
  					}
<GroupDocArg2>{DOCNL}			{
                                          if ( current->groupDocType==Entry::GROUPDOC_NORMAL &&
                                               current->type.isEmpty() 
					     ) // defgroup requires second argument
					  {
  					    warn(yyFileName,yyLineNr,
                                                 "warning: missing title after "
					         "\\defgroup %s", current->name.data()
                                                );
					  }
  					  if (*yytext=='\n') yyLineNr++; 
					  addOutput('\n');
					  BEGIN( Comment );
  					}

  /* --------- handle arguments of page/mainpage command ------------------- */

<PageDocArg1>{FILE}			{ // first argument; page name
					  current->name = stripQuotes(yytext);
					  BEGIN( PageDocArg2 ); 
					}
<PageDocArg1>{LC}			{ yyLineNr++; 
					  addOutput('\n');
                                        }
<PageDocArg1>{DOCNL}			{
  					  warn(yyFileName,yyLineNr,
                                               "warning: missing argument after "
					       "\\page."
                                              );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<PageDocArg1>.				{ // ignore other stuff
  					}
<PageDocArg2>.*"\n"			{ // second argument; page title
  				          yyLineNr++;
					  current->args = yytext;
					  addOutput('\n');
					  BEGIN( Comment );
					}

  /* --------- handle arguments of the file/dir/example command ------------ */

<FileDocArg1>{DOCNL}			{ // no file name specfied
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
					       "warning: Missing first argument of \\xrefitem"
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
					      "warning: Missing second argument of \\xrefitem"
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
					      "warning: Missing third argument of \\xrefitem"
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
					      "warning: Missing argument of \\relates or \\memberof command"
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

<SectionLabel>{LABELID}			{ // first argyment
  					  sectionLabel=yytext;
                                          addOutput(yytext);
					  sectionTitle.resize(0);
					  BEGIN(SectionTitle);
  					}
<SectionLabel>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "warning: \\section command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<SectionLabel>.				{ // invalid character for section label
  					  warn(yyFileName,yyLineNr,
					      "warning: Invalid or missing section label"
					      );
					  BEGIN(Comment);
  					}

<SectionTitle>[^\n@\\*]*/"\n"            { // end of section title
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
  					  sectionTitle+=yytext;
					  addOutput(yytext);
  					}
<SectionTitle>("\\\\"|"@@"){ID}		{ // unescape escaped command
  					  sectionTitle+=&yytext[1];
					  addOutput(yytext);
  					}
<SectionTitle>{CMD}[$@\\&~<>#%]		{ // unescape escaped character
  					  sectionTitle+=yytext[1];
  					  addOutput(yytext);
  					}
<SectionTitle>.				{ // anything else
  					  sectionTitle+=yytext;
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
					      "warning: \\subpage command has no label"
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
                                          SectionInfo  *si = new SectionInfo(yyFileName,yytext,0,SectionInfo::Anchor);
                                          Doxygen::sectionDict.insert(yytext,si);
  					  current->anchors->append(si);
					  addOutput(yytext);
					  BEGIN( Comment );
  					}
<AnchorLabel>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "warning: \\anchor command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<AnchorLabel>.				{ // invalid character for anchor label
  					  warn(yyFileName,yyLineNr,
					      "warning: Invalid or missing anchor label"
					      );
					  BEGIN(Comment);
  					}


  /* ----- handle arguments of the preformatted block commands ------- */

<FormatBlock>{CMD}("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"endmsc")/{NW} { // possible ends
  					  addOutput(yytext);
					  if (&yytext[4]==blockName) // found end of the block
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
  					  g_commentCount++;
  					  addOutput(yytext);
  					}
<FormatBlock>"*/"			{ // end of a C-comment
  					  addOutput(yytext);
  					  g_commentCount--;
					  if (g_commentCount<0 && blockName!="verbatim")
					  {
					    warn(yyFileName,yyLineNr,
					         "warning: found */ without matching /* while inside a \\%s block! Perhaps a missing \\end%s?\n",blockName.data(),blockName.data());
					  }
  					}
<FormatBlock>.				{
  					  addOutput(*yytext);
  					}
<FormatBlock><<EOF>>			{
                                          warn(yyFileName,yyLineNr,
	                                    "warning: reached end of comment while inside a @%s block; check for missing @end%s tag!",
	                                    blockName.data(),blockName.data()
	                                  );				
					  yyterminate();
    					}

  /* ----- handle arguments of if/ifnot commands ------- */

<GuardParam>{LABELID}			{ // parameter of if/ifnot guard
                                          bool sectionEnabled = Config_getList("ENABLED_SECTIONS").find(yytext)!=-1;
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
  					  BEGIN(Comment);
  					}
<GuardParamEnd>{B}*			{
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
						"warning: found @endif without matching start command");
					  }
					  else
					  {
					    delete guards.pop();
					    BEGIN( GuardParamEnd );
					  }
  					}
<SkipGuardedSection>{CMD}"else"/{NW}	{
  					  if (guards.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
						"warning: found @else without matching start command");
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
						"warning: found @elseif without matching start command");
					  }
					  else
					  {
					    if (!enabledSectionFound && guards.top()->parentVisible())
					    {
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
					  g_memberGroupHeader+=' ';
  					}
<NameParam>.				{ // ignore other stuff
					  g_memberGroupHeader+=*yytext;
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
					        "warning: Missing group name for \\ingroup command"
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
					  addOutput('\n');
					  if (functionProto.stripWhiteSpace().isEmpty())
					  { // plain overload command
					    addOutput(getOverloadDocs());
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
					      "warning: \\inherit command has no argument"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<InheritParam>.				{ // invalid character for anchor label
  					  warn(yyFileName,yyLineNr,
					      "warning: Invalid or missing name for \\inherit command"
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
					      "warning: \\extends or \\implements command has no argument"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<ExtendsParam>.				{ // ignore other stuff
  					}

  /* ----- handle language specific sections ------- */

<SkipLang>[\\@]"~"[a-zA-Z]*        { /* language switch */
                                     QCString langId = &yytext[2];
				     if (langId.isEmpty() ||
					 stricmp(Config_getEnum("OUTPUT_LANGUAGE"),langId)==0)
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


%%

//----------------------------------------------------------------------------

static bool handleBrief(const QCString &)
{
  //printf("handleBrief\n");
  setOutput(OutputBrief);
  return FALSE;
}

static bool handleFn(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::MEMBERDOC_SEC);
  functionProto.resize(0);
  braceCount=0;
  BEGIN(FnParam);
  return stop;
}

static bool handleDef(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::DEFINEDOC_SEC);
  functionProto.resize(0);
  BEGIN(FnParam);
  return stop;
}

static bool handleOverload(const QCString &)
{
  functionProto.resize(0);
  BEGIN(OverloadParam);
  return FALSE;
}

static bool handleEnum(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::ENUMDOC_SEC);
  BEGIN(EnumDocArg1);
  return stop;
}

static bool handleDefGroup(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_NORMAL;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleAddToGroup(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_ADD;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleWeakGroup(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_WEAK;
  BEGIN( GroupDocArg1 );
  return stop;
}

static bool handleNamespace(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::NAMESPACEDOC_SEC);
  BEGIN( NameSpaceDocArg1 );
  return stop;
}

static bool handlePackage(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::PACKAGEDOC_SEC);
  BEGIN( PackageDocArg1 );
  return stop;
}

static bool handleClass(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::CLASSDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleHeaderFile(const QCString &)
{
  BEGIN( ClassDocArg2 );
  return FALSE;
}

static bool handleProtocol(const QCString &)
{ // Obj-C protocol
  bool stop=makeStructuralIndicator(Entry::PROTOCOLDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleCategory(const QCString &)
{ // Obj-C category
  bool stop=makeStructuralIndicator(Entry::CATEGORYDOC_SEC);
  BEGIN( CategoryDocArg1 ); 
  return stop;
}

static bool handleUnion(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::UNIONDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleStruct(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::STRUCTDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleInterface(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::INTERFACEDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handleIdlException(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::EXCEPTIONDOC_SEC);
  BEGIN( ClassDocArg1 ); 
  return stop;
}

static bool handlePage(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::PAGEDOC_SEC);
  BEGIN( PageDocArg1 );
  return stop;
}

static bool handleMainpage(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::MAINPAGEDOC_SEC);
  if (!stop) 
  {
    current->name = "mainpage";
  }
  BEGIN( PageDocArg2 );
  return stop;
}

static bool handleFile(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::FILEDOC_SEC);
  if (!stop) 
  {
    current->name = yyFileName;
  }
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleDir(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::DIRDOC_SEC);
  if (!stop) current->name = yyFileName;
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleExample(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::EXAMPLE_SEC);
  if (!stop) current->name = yyFileName;
  BEGIN( FileDocArg1 );
  return stop;
}

static bool handleDetails(const QCString &)
{
  if (inContext!=OutputBrief)
  {
    addOutput("\n\n"); // treat @details outside brief description
                       // as a new paragraph
  }
  setOutput(OutputDoc);
  return FALSE;
}

static bool handleName(const QCString &)
{
  bool stop=makeStructuralIndicator(Entry::MEMBERGRP_SEC);
  if (!stop)
  {
    g_memberGroupHeader.resize(0);
    BEGIN( NameParam );
    if (g_memberGroupId!=DOX_NOGROUP) // end of previous member group
    {
      closeGroup(current,yyFileName,yyLineNr,TRUE);
    }
  }
  return stop;
}

static bool handleTodo(const QCString &)
{
  newXRefKind = XRef_Todo;
  setOutput(OutputXRef);
  xrefKind = XRef_Todo;
  return FALSE;
}

static bool handleTest(const QCString &)
{
  newXRefKind = XRef_Test;
  setOutput(OutputXRef);
  xrefKind = XRef_Test;
  return FALSE;
}

static bool handleBug(const QCString &)
{
  newXRefKind = XRef_Bug;
  setOutput(OutputXRef);
  xrefKind = XRef_Bug;
  return FALSE;
}

static bool handleDeprecated(const QCString &)
{
  newXRefKind = XRef_Deprecated;
  setOutput(OutputXRef);
  xrefKind = XRef_Deprecated;
  return FALSE;
}

static bool handleXRefItem(const QCString &)
{
  BEGIN(XRefItemParam1);
  return FALSE;
}

static bool handleRelated(const QCString &)
{
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleRelatedAlso(const QCString &)
{
  current->relatesType = Duplicate;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleMemberOf(const QCString &)
{
  current->relatesType = MemberOf;
  BEGIN(RelatesParam1);
  return FALSE;
}

static bool handleRefItem(const QCString &)
{
  addOutput(" @refitem ");
  BEGIN(LineParam);
  return FALSE;
}

static bool handleSection(const QCString &s)
{
  setOutput(OutputDoc);
  addOutput(" @"+s+" ");
  BEGIN(SectionLabel);
  if      (s=="section")       g_sectionLevel=1;
  else if (s=="subsection")    g_sectionLevel=2;
  else if (s=="subsubsection") g_sectionLevel=3;
  else if (s=="paragraph")     g_sectionLevel=4;
  return FALSE;
}

static bool handleSubpage(const QCString &s)
{
  if (current->section!=Entry::EMPTY_SEC && 
      current->section!=Entry::PAGEDOC_SEC &&
      current->section!=Entry::MAINPAGEDOC_SEC
     )
  {
    warn(yyFileName,yyLineNr,
	"warning: found \\subpage command in a comment block that is not marked as a page!");
  }
  addOutput(" @"+s+" ");
  BEGIN(SubpageLabel);
  return FALSE;
}

static bool handleAnchor(const QCString &s)
{
  addOutput(" @"+s+" ");
  BEGIN(AnchorLabel);
  return FALSE;
}

static bool handleFormatBlock(const QCString &s)
{
  addOutput(" @"+s+" ");
  //printf("handleFormatBlock(%s)\n",s.data());
  blockName=s;
  g_commentCount=0;
  BEGIN(FormatBlock);
  return FALSE;
}

static bool handleAddIndex(const QCString &)
{
  addOutput(" @addindex ");
  BEGIN(LineParam);
  return FALSE;
}

static bool handleIf(const QCString &)
{
  enabledSectionFound=FALSE;
  guardType = Guard_If;
  BEGIN(GuardParam);
  return FALSE;
}

static bool handleIfNot(const QCString &)
{
  enabledSectionFound=FALSE;
  guardType = Guard_IfNot;
  BEGIN(GuardParam);
  return FALSE;
}

static bool handleElseIf(const QCString &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"warning: found \\else without matching start command");
  }
  else
  {
    guardType = enabledSectionFound ? Guard_Skip : Guard_If;
    BEGIN(GuardParam);
  }
  return FALSE;
}

static bool handleElse(const QCString &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"warning: found \\else without matching start command");
  }
  else
  {
    BEGIN( SkipGuardedSection );
  }
  return FALSE;
}

static bool handleEndIf(const QCString &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"warning: found \\endif without matching start command");
  }
  else
  {
    delete guards.pop();
  }
  enabledSectionFound=FALSE;
  BEGIN( GuardParamEnd );
  return FALSE;
}

static bool handleIngroup(const QCString &)
{
  inGroupParamFound=FALSE;
  BEGIN( InGroupParam );
  return FALSE;
}

static bool handleNoSubGrouping(const QCString &)
{
  current->subGrouping = FALSE; 
  return FALSE;
}

static bool handleShowInitializer(const QCString &)
{
  current->initLines = 100000; // ON
  return FALSE;
}

static bool handleHideInitializer(const QCString &)
{
  current->initLines = 0; // OFF
  return FALSE;
}

static bool handleCallgraph(const QCString &)
{
  current->callGraph = TRUE; // ON
  return FALSE;
}

static bool handleCallergraph(const QCString &)
{
  current->callerGraph = TRUE; // ON
  return FALSE;
}

static bool handleInternal(const QCString &)
{
  if (!Config_getBool("INTERNAL_DOCS"))
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
    addOutput("\\internal "); 
  }
  return FALSE;
}

static bool handleLineBr(const QCString &)
{
  addOutput('\n');
  return FALSE;
}

static bool handleStatic(const QCString &)
{
  endBrief();
  current->stat = TRUE; 
  return FALSE;
}

static bool handlePure(const QCString &)
{
  endBrief();
  current->virt = Pure; 
  return FALSE;
}

static bool handlePrivate(const QCString &)
{
  current->protection = Private;
  return FALSE;
}

static bool handlePrivateSection(const QCString &)
{
  current->protection = protection = Private;
  return FALSE;
}

static bool handleProtected(const QCString &)
{
  current->protection = Protected;
  return FALSE;
}

static bool handleProtectedSection(const QCString &)
{
  current->protection = protection = Protected ;
  return FALSE;
}

static bool handlePublic(const QCString &)
{
  current->protection = Public;
  return FALSE;
}

static bool handlePublicSection(const QCString &)
{
  current->protection = protection = Public;
  return FALSE;
}

static bool handleInherit(const QCString &)
{
  BEGIN(InheritParam);
  return FALSE;
}

static bool handleExtends(const QCString &)
{
  BEGIN(ExtendsParam);
  return FALSE;
}

//----------------------------------------------------------------------------

static void checkFormula()
{
  if (YY_START==ReadFormulaShort || YY_START==ReadFormulaLong)
  {
    warn(yyFileName,yyLineNr,"warning: End of comment block while inside formula.");
  }
}

//----------------------------------------------------------------------------

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
  setOutput( isBrief || isAutoBriefOn ? OutputBrief : OutputDoc );
  briefEndsAtDot = isAutoBriefOn;
  g_condCount    = 0;
  g_sectionLevel = 0;

  if (!current->inbodyDocs.isEmpty() && isInbody) // separate in body fragments
  {
    current->inbodyDocs+="\n\n";
  }

  Debug::print(Debug::CommentScan,0,"-----------\nCommentScanner: %s:%d\n"
               "input=[%s]\n",fileName.data(),lineNr,comment.data()
              );
  
  commentScanYYrestart( commentScanYYin );
  BEGIN( Comment );
  commentScanYYlex();
  setOutput( OutputDoc );

  if (YY_START==OverloadParam) // comment ended with \overload
  {
    addOutput(getOverloadDocs());
  }

  if (!guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,"Documentation block ended in the middle of a conditional section!");
  }

  current->doc=stripLeadingAndTrailingEmptyLines(current->doc);

  if (current->section==Entry::FILEDOC_SEC && current->doc.isEmpty())
  {
    // to allow a comment block with just a @file command.
    current->doc="\n\n";
  }

  if (current->section==Entry::MEMBERGRP_SEC &&
      g_memberGroupId==DOX_NOGROUP) // @name section but no group started yet
  {
    openGroup(current,yyFileName,yyLineNr);
  }

  Debug::print(Debug::CommentScan,0,
               "brief=[%s]\ndocs=[%s]\ninbody=[%s]\n===========\n",
	       current->brief.data(),current->doc.data(),current->inbodyDocs.data()
              );
  
  checkFormula();
  prot = protection;
  
  groupAddDocs(curEntry,fileName);

  newEntryNeeded = needNewEntry;

  // if we did not proceed during this call, it does not make
  // sense to continue, since we get stuck. See bug 567346 for situations
  // were this happens
  if (parseMore && position==inputPosition) parseMore=FALSE;

  if (parseMore) position=inputPosition; else position=0;

  lineNr = yyLineNr;
  //printf("position=%d parseMore=%d\n",position,parseMore);

  return parseMore;
}

//---------------------------------------------------------------------------

void groupEnterFile(const char *fileName,int)
{
  g_autoGroupStack.setAutoDelete(TRUE);
  g_autoGroupStack.clear();
  g_memberGroupId = DOX_NOGROUP;
  g_memberGroupDocs.resize(0);
  g_memberGroupRelates.resize(0);
  g_compoundName=fileName;
}

void groupLeaveFile(const char *fileName,int line)
{
  //if (g_memberGroupId!=DOX_NOGROUP)
  //{
  //  warn(fileName,line,"warning: end of file while inside a member group\n");
  //}
  g_memberGroupId=DOX_NOGROUP;
  g_memberGroupRelates.resize(0);
  g_memberGroupDocs.resize(0);
  if (!g_autoGroupStack.isEmpty())
  {
    warn(fileName,line,"warning: end of file while inside a group\n");
  }
}

void groupEnterCompound(const char *fileName,int line,const char *name)
{
  if (g_memberGroupId!=DOX_NOGROUP)
  {
    warn(fileName,line,"warning: try to put compound %s inside a member group\n",name);
  }
  g_memberGroupId=DOX_NOGROUP;
  g_memberGroupRelates.resize(0);
  g_memberGroupDocs.resize(0);
  g_compoundName = name;
  int i = g_compoundName.find('(');
  if (i!=-1) 
  {
    g_compoundName=g_compoundName.left(i); // strip category (Obj-C)
  }
  if (g_compoundName.isEmpty())
  {
    g_compoundName=fileName;
  }
  //printf("groupEnterCompound(%s)\n",name);
}

void groupLeaveCompound(const char *,int,const char * /*name*/)
{
  //printf("groupLeaveCompound(%s)\n",name);
  //if (g_memberGroupId!=DOX_NOGROUP)
  //{
  //  warn(fileName,line,"warning: end of compound %s while inside a member group\n",name);
  //}
  g_memberGroupId=DOX_NOGROUP;
  g_memberGroupRelates.resize(0);
  g_memberGroupDocs.resize(0);
  g_compoundName.resize(0);
}

static int findExistingGroup(int &groupId,const MemberGroupInfo *info)
{
  //printf("findExistingGroup %s:%s\n",info->header.data(),info->compoundName.data());
  QIntDictIterator<MemberGroupInfo> di(Doxygen::memGrpInfoDict);
  MemberGroupInfo *mi;
  for (di.toFirst();(mi=di.current());++di)
  {
    if (g_compoundName==mi->compoundName &&  // same file or scope
	!mi->header.isEmpty() &&             // not a nameless group
	stricmp(mi->header,info->header)==0  // same header name
       )
    {
      //printf("Found it!\n");
      return di.currentKey(); // put the item in this group
    }
  }
  groupId++; // start new group
  return groupId;
}

void openGroup(Entry *e,const char *,int)
{
  //printf("==> openGroup(name=%s,sec=%x) g_autoGroupStack=%d\n",
  //  	e->name.data(),e->section,g_autoGroupStack.count());
  if (e->section==Entry::GROUPDOC_SEC) // auto group
  {
    g_autoGroupStack.push(new Grouping(e->name,e->groupingPri()));
  }
  else // start of a member group
  {
    //printf("    membergroup id=%d %s\n",g_memberGroupId,g_memberGroupHeader.data());
    if (g_memberGroupId==DOX_NOGROUP) // no group started yet
    {
      static int curGroupId=0;

      MemberGroupInfo *info = new MemberGroupInfo;
      info->header = g_memberGroupHeader.stripWhiteSpace();
      info->compoundName = g_compoundName;
      g_memberGroupId = findExistingGroup(curGroupId,info);
      //printf("    use membergroup %d\n",g_memberGroupId);
      Doxygen::memGrpInfoDict.insert(g_memberGroupId,info);

      g_memberGroupRelates = e->relates;
      e->mGrpId = g_memberGroupId;
    }
  }
}

void closeGroup(Entry *e,const char *fileName,int,bool foundInline)
{
  //printf("==> closeGroup(name=%s,sec=%x) g_autoGroupStack=%d\n",
  //    e->name.data(),e->section,g_autoGroupStack.count());
  if (g_memberGroupId!=DOX_NOGROUP) // end of member group
  {
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(g_memberGroupId);
    if (info) // known group
    {
      info->doc = g_memberGroupDocs;
      info->docFile = fileName;
    }
    g_memberGroupId=DOX_NOGROUP;
    g_memberGroupRelates.resize(0);
    g_memberGroupDocs.resize(0);
    e->mGrpId=DOX_NOGROUP;
    //printf("new group id=%d\n",g_memberGroupId);
  }
  else if (!g_autoGroupStack.isEmpty()) // end of auto group
  {
    Grouping *grp = g_autoGroupStack.pop();
    // see bug577005: we should not remove the last group for e
    if (!foundInline) e->groups->removeLast();
    //printf("Removing %s e=%p\n",grp->groupname.data(),e);
    delete grp;
    if (!foundInline) initGroupInfo(e);
  }
}

void initGroupInfo(Entry *e)
{
  //printf("==> initGroup(id=%d,related=%s,e=%p)\n",g_memberGroupId,
  //       g_memberGroupRelates.data(),e);
  e->mGrpId     = g_memberGroupId;
  e->relates    = g_memberGroupRelates;
  if (!g_autoGroupStack.isEmpty())
  {
    //printf("Appending group %s to %s: count=%d entry=%p\n",
    //	g_autoGroupStack.top()->groupname.data(),
    //	e->name.data(),e->groups->count(),e);
    e->groups->append(new Grouping(*g_autoGroupStack.top()));
  }
}

static void groupAddDocs(Entry *e,const char *fileName)
{
  if (e->section==Entry::MEMBERGRP_SEC)
  {
    g_memberGroupDocs=e->brief.stripWhiteSpace();
    e->doc = stripLeadingAndTrailingEmptyLines(e->doc);
    if (!g_memberGroupDocs.isEmpty() && !e->doc.isEmpty())
    {
      g_memberGroupDocs+="\n\n";
    }
    g_memberGroupDocs+=e->doc;
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(g_memberGroupId);
    if (info) 
    {
      info->doc = g_memberGroupDocs;
      info->docFile = fileName;
      info->setRefItems(e->sli);
    }
    e->doc.resize(0);
    e->brief.resize(0);
  }
}


#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void commentScanYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

