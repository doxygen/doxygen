/*****************************************************************************
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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
static void handleBrief(const QCString &);
static void handleFn(const QCString &);
static void handleDef(const QCString &);
static void handleOverload(const QCString &);
static void handleEnum(const QCString &);
static void handleDefGroup(const QCString &);
static void handleAddToGroup(const QCString &);
static void handleWeakGroup(const QCString &);
static void handleNamespace(const QCString &);
static void handlePackage(const QCString &);
static void handleClass(const QCString &);
static void handleProtocol(const QCString &);
static void handleCategory(const QCString &);
static void handleUnion(const QCString &);
static void handleStruct(const QCString &);
static void handleInterface(const QCString &);
static void handleIdlException(const QCString &);
static void handlePage(const QCString &);
static void handleMainpage(const QCString &);
static void handleFile(const QCString &);
static void handleDir(const QCString &);
static void handleExample(const QCString &);
static void handleDetails(const QCString &);
static void handleName(const QCString &);
static void handleTodo(const QCString &);
static void handleTest(const QCString &);
static void handleBug(const QCString &);
static void handleSubpage(const QCString &s);
static void handleDeprecated(const QCString &);
static void handleXRefItem(const QCString &);
static void handleRelated(const QCString &);
static void handleRelatedAlso(const QCString &);
static void handleRefItem(const QCString &);
static void handleSection(const QCString &);
static void handleAnchor(const QCString &);
static void handleFormatBlock(const QCString &);
static void handleAddIndex(const QCString &);
static void handleIf(const QCString &);
static void handleIfNot(const QCString &);
static void handleElseIf(const QCString &);
static void handleElse(const QCString &);
static void handleEndIf(const QCString &);
static void handleIngroup(const QCString &);
static void handleNoSubGrouping(const QCString &);
static void handleShowInitializer(const QCString &);
static void handleHideInitializer(const QCString &);
static void handleCallgraph(const QCString &);
static void handleInternal(const QCString &);
static void handleLineBr(const QCString &);
static void handleStatic(const QCString &);
static void handlePure(const QCString &);
static void handlePrivate(const QCString &);
static void handlePrivateSection(const QCString &);
static void handleProtected(const QCString &);
static void handleProtectedSection(const QCString &);
static void handlePublic(const QCString &s);
static void handlePublicSection(const QCString &s);
static void handleInherit(const QCString &);


typedef void (*DocCmdFunc)(const QCString &name);
  
struct DocCmdMap
{
  const char *cmdName;
  DocCmdFunc handler;
};

// map of command to handler function
static DocCmdMap docCmdMap[] =
{
  { "brief",           &handleBrief },
  { "short",           &handleBrief },
  { "fn",              &handleFn },
  { "var",             &handleFn },
  { "typedef",         &handleFn },
  { "property",        &handleFn },
  { "def",             &handleDef },
  { "overload",        &handleOverload },
  { "enum",            &handleEnum },
  { "defgroup",        &handleDefGroup },
  { "addtogroup",      &handleAddToGroup },
  { "weakgroup",       &handleWeakGroup },
  { "namespace",       &handleNamespace },
  { "package",         &handlePackage },
  { "class",           &handleClass },
  { "protocol",        &handleProtocol },
  { "category",        &handleCategory },
  { "union",           &handleUnion },
  { "struct",          &handleStruct },
  { "interface",       &handleInterface },
  { "idlexcept",       &handleIdlException },
  { "page",            &handlePage },
  { "mainpage",        &handleMainpage },
  { "file",            &handleFile },
  { "dir",             &handleDir },
  { "example",         &handleExample },
  { "details",         &handleDetails },
  { "name",            &handleName },
  { "todo",            &handleTodo },
  { "test",            &handleTest },
  { "bug",             &handleBug },
  { "deprecated",      &handleDeprecated },
  { "xrefitem",        &handleXRefItem },
  { "related",         &handleRelated },
  { "relates",         &handleRelated },
  { "relatedalso",     &handleRelatedAlso },
  { "relatesalso",     &handleRelatedAlso },
  { "refitem",         &handleRefItem },
  { "subpage",         &handleSubpage },
  { "section",         &handleSection },
  { "subsection",      &handleSection },
  { "subsubsection",   &handleSection },
  { "paragraph",       &handleSection },
  { "anchor",          &handleAnchor },
  { "verbatim",        &handleFormatBlock },
  { "latexonly",       &handleFormatBlock },
  { "htmlonly",        &handleFormatBlock },
  { "xmlonly",         &handleFormatBlock },
  { "rtfonly",         &handleFormatBlock },
  { "manonly",         &handleFormatBlock },
  { "dot",             &handleFormatBlock },
  { "code",            &handleFormatBlock },
  { "addindex",        &handleAddIndex },
  { "if",              &handleIf },
  { "ifnot",           &handleIfNot },
  { "elseif",          &handleElseIf },
  { "else",            &handleElse },
  { "endif",           &handleEndIf },
  { "ingroup",         &handleIngroup },
  { "nosubgrouping",   &handleNoSubGrouping },
  { "showinitializer", &handleShowInitializer },
  { "hideinitializer", &handleHideInitializer },
  { "callgraph",       &handleCallgraph },
  { "internal",        &handleInternal },
  { "_linebr",         &handleLineBr },
  { "static",          &handleStatic },
  { "pure",            &handlePure },
  { "private",         &handlePrivate},
  { "privatesection",  &handlePrivateSection },
  { "protected",       &handleProtected },
  { "protectedsection",&handleProtectedSection },
  { "public",          &handlePublic },
  { "publicsection",   &handlePublicSection },
  { "inherit",         &handleInherit },
  { 0, 0 }
};



/** @brief Command mapper.
 *
 *  Maps a command name (as found in a comment block) onto a
 *  specific handler function.
 */
class DocCmdMapper
{
  public:
    /** maps a command name to a handler function */
    static DocCmdFunc *map(const char *name)
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

    DocCmdMapper() : m_map(89)
    {
      DocCmdMap *p = docCmdMap;
      while (p->cmdName)
      {
	m_map.insert(p->cmdName,&p->handler);
	p++;
      }
    }

    DocCmdFunc *find(const char *name)
    {
      return m_map.find(name);
    }
    QDict<DocCmdFunc> m_map;
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
  OutputXRef
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
static const char *     inputString;         // input string
static int		inputPosition;       // read pointer
static QCString		yyFileName;          // file name that is read from
static int		yyLineNr;            // line number in the input
//static bool             inBody;              // was the comment found inside the body of a function?
static OutputContext    inContext;           // are we inside the brief, details or xref part
static bool             briefEndsAtDot;      // does the brief description stop at a dot?
static QCString         formulaText;         // Running text of a formula
static QCString         formulaEnv;          // environment name
static QCString        *pOutputString;       // pointer to string to which the output is appended.
static QCString         outputXRef;          // temp argument of todo/test/../xrefitem commands
static QCString         blockName;           // preformatted block name (e.g. verbatim, latexonly,...)
static XRefKind         xrefKind;            // kind of cross-reference command
static XRefKind         newXRefKind;         // 
static GuardType        guardType;           // kind of guard for conditional section
static bool             enabledSectionFound;
static QCString         nameHeader;          // heading of the @name command
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

//-----------------------------------------------------------------------------

static void initParser()
{
  sectionLabel.resize(0);
  sectionTitle.resize(0);
  nameHeader.resize(0);
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

static void makeStructuralIndicator(Entry::Sections s)
{
  if (!getDocSectionName(current->section).isEmpty())
  {
    warn(yyFileName,yyLineNr,
	    "Warning: found a structural command %s for a section already "
	    "marked with structural command %s. Ignoring the latter command.",
	     getDocSectionName(s).data(),
             getDocSectionName(current->section).data()
	);
  }
  else
  {
    needNewEntry = TRUE;
    current->section = s;
    current->fileName = yyFileName;
    current->startLine = yyLineNr;
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
    sprintf(anchorLabel,"_%s%06d",listName,lii ? lii->itemId : itemId);
    RefItem *item = refList->getRefItem(itemId);
    ASSERT(item!=0);
    item->text = outputXRef;
    item->listAnchor = anchorLabel;
    docEntry->addSpecialListItem(listName,itemId);
    QCString cmdString;
    cmdString.sprintf("\\xrefitem %s %d\n",listName,itemId);
    docEntry->doc += cmdString;
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
  return formLabel;
}

//-----------------------------------------------------------------------------

static void checkFormula();
//-----------------------------------------------------------------------------

static void prependScope()
{
#if 0
  Entry *current_root = current->parent;
  if (current_root && current_root->section & Entry::SCOPE_MASK)
  {
    current->name.prepend(current_root->name+"::");
    if (current_root->tArgLists)
    {
      if (current->tArgLists==0)
      {
	current->tArgLists = new QList<ArgumentList>;
	current->tArgLists->setAutoDelete(TRUE);
      }
      QListIterator<ArgumentList> talsi(*current_root->tArgLists);
      ArgumentList *srcAl=0;
      for (talsi.toLast();(srcAl=talsi.current());--talsi)
      {
        ArgumentList *dstAl = new ArgumentList;
	dstAl->setAutoDelete(TRUE);
	QListIterator<Argument> tali(*srcAl);
        Argument *a;
        for (;(a=tali.current());++tali)
        {
          dstAl->append(new Argument(*a));
        }	  
        current->tArgLists->insert(0,dstAl);	
      }
    }
  }
#endif
}

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
  xrefAppendFlag = inContext==OutputXRef && ctx==OutputXRef && // two consecutive xref items
                   newXRefKind==xrefKind &&                    // of the same kind
                   (xrefKind!=XRef_Item || 
		    newXRefItemKey==xrefItemKey);              // with the same key if \xrefitem
  //printf("refKind=%d newXRefKind=%d xrefAppendToPrev=%d xrefAppendFlag=%d\n",
  //   	  xrefKind,newXRefKind,xrefAppendToPrev,xrefAppendFlag);
  xrefItemKey = newXRefItemKey;

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

  inContext = ctx;
  switch(inContext)
  {
    case OutputDoc:
      stripTrailingWhiteSpace(current->doc);
      if (current->docFile.isEmpty())
      {
        current->docFile = yyFileName;
        current->docLine = yyLineNr;
      }
      pOutputString = &current->doc;
      break;
    case OutputBrief:
      if (current->briefFile.isEmpty())
      {
        current->briefFile = yyFileName;
        current->briefLine = yyLineNr;
      }
      pOutputString = &current->brief;
      break;
    case OutputXRef:
      pOutputString = &outputXRef;
      // first item found, so can't append to previous
      //xrefAppendFlag = FALSE;
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

/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
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
DCMD1     ("arg"|"attention"|"author"|"bug"|"code")
DCMD2     ("date"|"deprecated"|"dot"|"dotfile"|"example")
DCMD3     ("htmlinclude"|"htmlonly"|"image"|"include")
DCMD4     ("includelineno"|"internal"|"invariant")
DCMD5     ("latexonly"|"li"|"line"|"manonly"|"name") 
DCMD6     ("note"|"par"|"paragraph"|"param"|"post")
DCMD7     ("pre"|"remarks"|(("relate"[sd])("also")?))
DCMD8     ("remarks"|("return"[s]?)|"retval"|"sa"|"section")
DCMD9     ("see"|"since"|"subsection"|"subsubsection")
DCMD10    ("test"|"throw"|"todo"|"until"|"verbatim")
DCMD11    ("verbinclude"|"version"|"warning"|"xrefitem")
DETAILEDCMD {CMD}({DCMD1}|{DCMD2}|{DCMD3}|{DCMD4}|{DCMD5}|{DCMD6}|{DCMD7}|{DCMD8}|{DCMD9}|{DCMD10}|{DCMD11})
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
FILESCHAR [a-z_A-Z0-9\\:\\\/\-\+]
FILEECHAR [a-z_A-Z0-9\-\+]
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]+"\"")
ID        "$"?[a-z_A-Z][a-z_A-Z0-9]*
LABELID   [a-z_A-Z][a-z_A-Z0-9\-]*
SCOPEID   {ID}({ID}*{BN}*"::"{BN}*)*({ID}?)
SCOPENAME "$"?(({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
MAILADR   [a-z_A-Z0-9.+\-]+"@"[a-z_A-Z0-9\-]+("."[a-z_A-Z0-9\-]+)+[a-z_A-Z0-9\-]+ 

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
%x	SkipGuardedSection
%x	SkipInternal
%x      NameParam
%x	InGroupParam
%x	FnParam
%x	OverloadParam
%x	InheritParam
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
<Comment>{DETAILEDCMD}/[^a-z_A-Z]*	{ // command that can end a brief description
					  if (inContext!=OutputXRef) setOutput(OutputDoc);
					  // continue with the same input
                                          REJECT;
					}
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
<Comment>"<!--"				{ 
  					  BEGIN(HtmlComment);
					}
<Comment>{CMD}[a-z_A-Z]+{B}*		{ // potentially interesting command
  					  QCString cmdName = QCString(&yytext[1]).stripWhiteSpace();
  					  DocCmdFunc *funcPtr = DocCmdMapper::map(cmdName);
					  if (funcPtr) // special action is required
					  {
					    //printf("Special command '%s'\n",yytext);
					    (*funcPtr)(cmdName);
					  }
					  else // command not relevant
					  {
					    addOutput(yytext);
					  }
  					}
<Comment>("\\\\"|"@@")"f"[$\[{]		{ // escaped formula command
  					  addOutput(yytext);
  					}
<Comment>{CMD}"~"[a-z_A-Z]*		{ // language switch command
                                          QCString langId = &yytext[2];
			       	          if (!langId.isEmpty() &&
					      stricmp(Config_getEnum("OUTPUT_LANGUAGE"),langId)!=0)
				          { // enable language specific section
				            BEGIN(SkipLang);
				          }
  					}
<Comment>{CMD}"f{"[^}\n]+"}"		{ // start of a formula with custom environment
					  formulaText="\\begin";
					  formulaEnv=&yytext[2];
					  formulaText+=formulaEnv;
					  BEGIN(ReadFormulaLong);
  					}
<Comment>{CMD}"f$"			{ // start of a inline formula
					  formulaText="$";
					  BEGIN(ReadFormulaShort);
  					}
<Comment>{CMD}"f["			{ // start of a block formula
					  formulaText="\\[";
					  BEGIN(ReadFormulaLong);
  					}
<Comment>{CMD}"{"                       { // begin of a group
                                          langParser->handleGroupStartCommand(nameHeader);
                                        }
<Comment>{CMD}"}"                       { // end of a group
                                          langParser->handleGroupEndCommand();
                                          nameHeader.resize(0);
                                        }
<Comment>{CMD}[$@\\&~<>#%]		{ // escaped character
  					  addOutput(yytext);
  					}
<Comment>[a-z_A-Z]+			{ // normal word
					  addOutput(yytext);
  					}
<Comment>^{B}*"."{B}*/\n                { // explicit end autolist: e.g "  ."
  				          addOutput(yytext); 
					}
<Comment>"."[a-z_A-Z0-9]		{ // . at start or in the middle of a word
  					  addOutput(yytext);
  					}
<Comment>".\\"[ \t]			{ // . with escaped space.
  					  addOutput(yytext[0]);
  					  addOutput(yytext[2]);
  					}
<Comment>(\n|\\_linebr)({B}*(\n|\\_linebr))+	{ // at least one blank line (or blank line command)
  					  if (inContext!=OutputBrief)
					  {
  					    addOutput(yytext);
					    setOutput(OutputDoc);
					  }
					  else
					  {
					    setOutput(OutputDoc);
  					    addOutput(yytext);
					  }
  					  lineCount();
  					}
<Comment>"."				{ // potential end of a JavaDoc style comment
  					  addOutput(*yytext);
  					  if (briefEndsAtDot)
					  {
					    setOutput(OutputDoc);
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
					  addOutput(addFormula());
					  addOutput(' ');
					  BEGIN(Comment);
  					}
<ReadFormulaLong>{CMD}"f]"		{ // end of block formula
					  formulaText+="\\]";
					  addOutput(addFormula());
					  addOutput(' ');
					  BEGIN(Comment);
  					}
<ReadFormulaLong>{CMD}"f}"		{ // end of custom env formula
					  formulaText+="\\end";
					  formulaText+=formulaEnv;
					  addOutput(addFormula());
					  addOutput(' ');
					  BEGIN(Comment);
  					}
<ReadFormulaLong,ReadFormulaShort>[^\\@\n]+ { // any non-special character
                                          formulaText+=yytext; 
 					} 
<ReadFormulaLong,ReadFormulaShort>\n	{ // new line
                                          formulaText+=*yytext; 
					  yyLineNr++; 
					}
<ReadFormulaLong,ReadFormulaShort>.     { // any othe character
                                          formulaText+=*yytext; 
					}

  /* ------------ handle argument of enum command --------------- */

<EnumDocArg1>{SCOPEID}			{ // handle argument
  					  current->name = yytext;
					  prependScope();
					  BEGIN( Comment );
  					}
<EnumDocArg1>{LC}			{ // line continuation
  					  yyLineNr++;
					  addOutput('\n');
                                        }
<EnumDocArg1>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after \\enum."
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<EnumDocArg1>.				{ // ignore other stuff
  					}

  /* ------------ handle argument of namespace command --------------- */

<NameSpaceDocArg1>{SCOPENAME}		{ // handle argument
  					  current->name = yytext;
					  BEGIN( Comment );
  					}
<NameSpaceDocArg1>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<NameSpaceDocArg1>{DOCNL}		{ // missing argument
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after "
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
                                               "Warning: missing argument after "
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
					  current->name = yytext;
					  if (current->section==Entry::PROTOCOLDOC_SEC)
					  {
					    current->name+="-p";
					  }
					  // prepend outer scope name 
					  prependScope();
					  BEGIN( ClassDocArg2 );
					}
<CategoryDocArg1>{SCOPENAME}{B}*"("[^\)]+")" {
					  current->name = yytext;
					  prependScope();
					  BEGIN( ClassDocArg2 );
   					}
<ClassDocArg1,CategoryDocArg1>{LC}      { // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
                                        }
<ClassDocArg1,CategoryDocArg1>{DOCNL}	{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after "
					       "\\%s.",YY_START==ClassDocArg1?"class":"category"
                                              );
					  addOutput('\n');
  					  if (*yytext=='\n') yyLineNr++;
					  BEGIN( Comment );
  					}
<ClassDocArg1,CategoryDocArg1>.		{ // ignore other stuff
  					}

<ClassDocArg2>{FILE}			{ // second argument; include file
					  current->includeFile = stripQuotes(yytext);
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

<ClassDocArg3>[<]?{FILE}[>]?		{ // third argument; include file name
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

<GroupDocArg1>{ID}(".html"?)		{ // group name
  					  current->name = yytext;
					  //lastDefGroup.groupname = yytext;
					  //lastDefGroup.pri = current->groupingPri();
  					  // the .html stuff is for Qt compatibility
					  if (current->name.right(5)==".html") 
					  {
					    current->name=current->name.left(current->name.length()-5);
					  }
					  BEGIN(GroupDocArg2);
  					}
<GroupDocArg1>"\\"{B}*"\n"		{ // line continuation
  					  yyLineNr++; 
					  addOutput('\n');
                                        }
<GroupDocArg1>{DOCNL}			{ // missing argument!
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing group name after %s",
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
                                                 "Warning: missing title after "
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
                                               "Warning: missing argument after "
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

<FileDocArg1>{FILE}			{ // first argument; name
  					  current->name = stripQuotes(yytext);
					  BEGIN( Comment );
  				        }
<FileDocArg1>{LC}			{ yyLineNr++; 
					  addOutput('\n');
                                        }
<FileDocArg1>{DOCNL}			{ // no file name specfied
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
					}
<FileDocArg1>.				{ // ignore other stuff
  					}

  /* --------- handle arguments of the xrefitem command ------------ */

<XRefItemParam1>{ID}			{ // first argument
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
					       "Warning: Missing first argument of \\xrefitem"
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
					      "Warning: Missing second argument of \\xrefitem"
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
<XRefItemParam2>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<XRefItemParam3>{DOCNL}			{ // missing argument
  					  warn(yyFileName,yyLineNr,
					      "Warning: Missing third argument of \\xrefitem"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  inContext = OutputDoc;
					  BEGIN( Comment );
  					}
<XRefItemParam3>.			{ // ignore other stuff
  					}


  /* --------- handle arguments of the relates(also) command ------------ */

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
					      "Warning: Missing argument of \\relates command"
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
					      "Warning: \\section command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<SectionLabel>.				{ // invalid character for section label
  					  warn(yyFileName,yyLineNr,
					      "Warning: Invalid or missing section label"
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
					      "Warning: \\subpage command has no label"
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
					      "Warning: \\anchor command has no label"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<AnchorLabel>.				{ // invalid character for anchor label
  					  warn(yyFileName,yyLineNr,
					      "Warning: Invalid or missing anchor label"
					      );
					  BEGIN(Comment);
  					}


  /* ----- handle arguments of the preformatted block commands ------- */

<FormatBlock>{CMD}("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode")/{NW} { // possible ends
  					  addOutput(yytext);
					  if (&yytext[4]==blockName) // found end of the block
					  {
  					    BEGIN(Comment);
					  }
  					}
<FormatBlock>[^ \@\\\n]*		{ // some word
  					  addOutput(yytext);
  					}
<FormatBlock>{DOCNL}			{ // new line
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
  					}
<FormatBlock>.				{
  					  addOutput(*yytext);
  					}
<FormatBlock><<EOF>>			{
                                          warn(yyFileName,yyLineNr,
	                                    "Warning: reached end of comment while inside a @%s block; check for missing @end%s tag!",
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
					      BEGIN( Comment );
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
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<GuardParam>{LC}			{ // line continuation
                                          yyLineNr++; 
					  addOutput('\n');
  					}
<GuardParam>.				{ // ignore other stuff
  					  addOutput(*yytext);
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
						"Warning: found @endif without matching start command");
					  }
					  else
					  {
					    delete guards.pop();
					    BEGIN( Comment );
					  }
  					}
<SkipGuardedSection>{CMD}"else"/{NW}	{
  					  if (guards.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
						"Warning: found @else without matching start command");
					  }
					  else
					  {
					    if (!enabledSectionFound && guards.top()->parentVisible())
					    {
					      delete guards.pop();
					      guards.push(new GuardedSection(TRUE,TRUE));
					      enabledSectionFound=TRUE;
					      BEGIN( Comment );
					    }
					  }
  					}
<SkipGuardedSection>{CMD}"elseif"/{NW}  {
  					  if (guards.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
						"Warning: found @elseif without matching start command");
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
					  addOutput('\n');
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
					  nameHeader+=' ';
  					}
<NameParam>.				{ // ignore other stuff
					  nameHeader+=*yytext;
					  current->name+=*yytext;
  					}

  /* ----- handle argument of ingroup command ------- */

<InGroupParam>{ID}			{ // group id
  					  current->groups->append(
					    new Grouping(yytext, Grouping::GROUPING_INGROUP)
					  );
					  inGroupParamFound=TRUE;
  					}
<InGroupParam>{DOCNL}			{ // missing argument
  					  if (!inGroupParamFound)
					  {
  					    warn(yyFileName,yyLineNr,
					        "Warning: Missing group name for \\ingroup command"
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
					    //printf("functionProto=%s\n",functionProto.data());
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
					      "Warning: \\inherit command has no argument"
					      );
  					  if (*yytext=='\n') yyLineNr++;
					  addOutput('\n');
					  BEGIN( Comment );
  					}
<InheritParam>.				{ // invalid character for anchor label
  					  warn(yyFileName,yyLineNr,
					      "Warning: Invalid or missing name for \\inherit command"
					      );
					  BEGIN(Comment);
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

static void handleBrief(const QCString &)
{
  //printf("handleBrief\n");
  setOutput(OutputBrief);
}

static void handleFn(const QCString &)
{
  makeStructuralIndicator(Entry::MEMBERDOC_SEC);
  functionProto.resize(0);
  braceCount=0;
  BEGIN(FnParam);
}

static void handleDef(const QCString &)
{
  makeStructuralIndicator(Entry::DEFINEDOC_SEC);
  functionProto.resize(0);
  BEGIN(FnParam);
}

static void handleOverload(const QCString &)
{
  functionProto.resize(0);
  BEGIN(OverloadParam);
}

static void handleEnum(const QCString &)
{
  makeStructuralIndicator(Entry::ENUMDOC_SEC);
  BEGIN(EnumDocArg1);
}

static void handleDefGroup(const QCString &)
{
  makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_NORMAL;
  BEGIN( GroupDocArg1 );
}

static void handleAddToGroup(const QCString &)
{
  makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_ADD;
  BEGIN( GroupDocArg1 );
}

static void handleWeakGroup(const QCString &)
{
  makeStructuralIndicator(Entry::GROUPDOC_SEC);
  current->groupDocType = Entry::GROUPDOC_WEAK;
  BEGIN( GroupDocArg1 );
}

static void handleNamespace(const QCString &)
{
  makeStructuralIndicator(Entry::NAMESPACEDOC_SEC);
  BEGIN( NameSpaceDocArg1 );
}

static void handlePackage(const QCString &)
{
  makeStructuralIndicator(Entry::PACKAGEDOC_SEC);
  BEGIN( PackageDocArg1 );
}

static void handleClass(const QCString &)
{
  makeStructuralIndicator(Entry::CLASSDOC_SEC);
  BEGIN( ClassDocArg1 ); 
}

static void handleProtocol(const QCString &)
{ // Obj-C protocol
  makeStructuralIndicator(Entry::PROTOCOLDOC_SEC);
  BEGIN( ClassDocArg1 ); 
}

static void handleCategory(const QCString &)
{ // Obj-C category
  makeStructuralIndicator(Entry::CATEGORYDOC_SEC);
  BEGIN( CategoryDocArg1 ); 
}

static void handleUnion(const QCString &)
{
  makeStructuralIndicator(Entry::UNIONDOC_SEC);
  BEGIN( ClassDocArg1 ); 
}

static void handleStruct(const QCString &)
{
  makeStructuralIndicator(Entry::STRUCTDOC_SEC);
  BEGIN( ClassDocArg1 ); 
}

static void handleInterface(const QCString &)
{
  makeStructuralIndicator(Entry::INTERFACEDOC_SEC);
  BEGIN( ClassDocArg1 ); 
}

static void handleIdlException(const QCString &)
{
  makeStructuralIndicator(Entry::EXCEPTIONDOC_SEC);
  BEGIN( ClassDocArg1 ); 
}

static void handlePage(const QCString &)
{
  makeStructuralIndicator(Entry::PAGEDOC_SEC);
  BEGIN( PageDocArg1 );
}

static void handleMainpage(const QCString &)
{
  makeStructuralIndicator(Entry::MAINPAGEDOC_SEC);
  current->name = "mainpage";
  BEGIN( PageDocArg2 );
}

static void handleFile(const QCString &)
{
  makeStructuralIndicator(Entry::FILEDOC_SEC);
  current->name = yyFileName;
  BEGIN( FileDocArg1 );
}

static void handleDir(const QCString &)
{
  makeStructuralIndicator(Entry::DIRDOC_SEC);
  current->name = yyFileName;
  BEGIN( FileDocArg1 );
}

static void handleExample(const QCString &)
{
  makeStructuralIndicator(Entry::EXAMPLE_SEC);
  current->name = yyFileName;
  BEGIN( FileDocArg1 );
}

static void handleDetails(const QCString &)
{
  setOutput(OutputDoc);
}

static void handleName(const QCString &)
{
  makeStructuralIndicator(Entry::MEMBERGRP_SEC);
  nameHeader.resize(0);
  BEGIN( NameParam );
}

static void handleTodo(const QCString &)
{
  newXRefKind = XRef_Todo;
  setOutput(OutputXRef);
  xrefKind = XRef_Todo;
}

static void handleTest(const QCString &)
{
  newXRefKind = XRef_Test;
  setOutput(OutputXRef);
  xrefKind = XRef_Test;
}

static void handleBug(const QCString &)
{
  newXRefKind = XRef_Bug;
  setOutput(OutputXRef);
  xrefKind = XRef_Bug;
}

static void handleDeprecated(const QCString &)
{
  newXRefKind = XRef_Deprecated;
  setOutput(OutputXRef);
  xrefKind = XRef_Deprecated;
}

static void handleXRefItem(const QCString &)
{
  BEGIN(XRefItemParam1);
}

static void handleRelated(const QCString &)
{
  BEGIN(RelatesParam1);
}

static void handleRelatedAlso(const QCString &)
{
  current->relatesDup = TRUE;
  BEGIN(RelatesParam1);
}

static void handleRefItem(const QCString &)
{
  addOutput("@refitem ");
  BEGIN(LineParam);
}

static void handleSection(const QCString &s)
{
  setOutput(OutputDoc);
  addOutput("@"+s+" ");
  BEGIN(SectionLabel);
}

static void handleSubpage(const QCString &s)
{
  if (current->section!=Entry::EMPTY_SEC && 
      current->section!=Entry::PAGEDOC_SEC &&
      current->section!=Entry::MAINPAGEDOC_SEC
     )
  {
    warn(yyFileName,yyLineNr,
	"Warning: found \\subpage command in a comment block that is not marked as a page!");
  }
  addOutput("@"+s+" ");
  BEGIN(SubpageLabel);
}

static void handleAnchor(const QCString &s)
{
  addOutput("@"+s+" ");
  BEGIN(AnchorLabel);
}

static void handleFormatBlock(const QCString &s)
{
  addOutput("@"+s+" ");
  //printf("handleFormatBlock(%s)\n",s.data());
  blockName=s;
  BEGIN(FormatBlock);
}

static void handleAddIndex(const QCString &)
{
  addOutput("@addindex ");
  BEGIN(LineParam);
}

static void handleIf(const QCString &)
{
  enabledSectionFound=FALSE;
  guardType = Guard_If;
  BEGIN(GuardParam);
}

static void handleIfNot(const QCString &)
{
  enabledSectionFound=FALSE;
  guardType = Guard_IfNot;
  BEGIN(GuardParam);
}

static void handleElseIf(const QCString &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"Warning: found \\else without matching start command");
  }
  else
  {
    guardType = enabledSectionFound ? Guard_Skip : Guard_If;
    BEGIN(GuardParam);
  }
}

static void handleElse(const QCString &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"Warning: found \\else without matching start command");
  }
  else
  {
    BEGIN( SkipGuardedSection );
  }
}

static void handleEndIf(const QCString &)
{
  if (guards.isEmpty())
  {
    warn(yyFileName,yyLineNr,
	"Warning: found \\endif without matching start command");
  }
  else
  {
    delete guards.pop();
  }
  enabledSectionFound=FALSE;
}

static void handleIngroup(const QCString &)
{
  inGroupParamFound=FALSE;
  BEGIN( InGroupParam );
}

static void handleNoSubGrouping(const QCString &)
{
  current->subGrouping = FALSE; 
}

static void handleShowInitializer(const QCString &)
{
  current->initLines = 100000; // ON
}

static void handleHideInitializer(const QCString &)
{
  current->initLines = 0; // OFF
}

static void handleCallgraph(const QCString &)
{
  current->callGraph = TRUE; // ON
}

static void handleInternal(const QCString &)
{
  if (!Config_getBool("INTERNAL_DOCS"))
  {
    BEGIN( SkipInternal );
  }
  else
  {
    addOutput("\\internal "); 
  }
}

static void handleLineBr(const QCString &)
{
  addOutput('\n');
}

static void handleStatic(const QCString &)
{
  current->stat = TRUE; 
}

static void handlePure(const QCString &)
{
  current->virt = Pure; 
}

static void handlePrivate(const QCString &)
{
  current->protection = Private; 
}

static void handlePrivateSection(const QCString &)
{
  current->protection = protection = Private; 
}

static void handleProtected(const QCString &)
{
  current->protection = Protected; 
}

static void handleProtectedSection(const QCString &)
{
  current->protection = protection = Protected ;
}

static void handlePublic(const QCString &)
{
  current->protection = Public;
}

static void handlePublicSection(const QCString &)
{
  current->protection = protection = Public; 
}

static void handleInherit(const QCString &)
{
  BEGIN(InheritParam);
}

//----------------------------------------------------------------------------

static void checkFormula()
{
  if (YY_START==ReadFormulaShort || YY_START==ReadFormulaLong)
  {
    warn(yyFileName,yyLineNr,"Warning: End of comment block while inside formula.");
  }
}

//----------------------------------------------------------------------------

bool parseCommentBlock(/* in */     ParserInterface *parser,
                       /* in,out */ Entry *curEntry,
                       /* in */     const QCString &comment,
		       /* in */     const QCString &fileName,
		       /* in */     int lineNr,
		       /* in */     bool isBrief,
		       /* in */     bool isJavaDocStyle,
		       /* in,out */ Protection &prot
		      )
{
  //fprintf(stderr,"parseCommentBlock() isBrief=%d isJavaDocStyle=%d lineNr=%d\n",
  //    isBrief,isJavaDocStyle,lineNr);

  initParser();
  guards.setAutoDelete(TRUE);
  guards.clear();
  langParser     = parser;
  current        = curEntry;
  inputString    = comment;
  if (inputString==0) return FALSE; // avoid empty strings
  inputPosition  = 0;
  yyLineNr       = lineNr;
  yyFileName     = fileName;
  protection     = prot;
  needNewEntry   = FALSE;
  xrefKind       = XRef_None;
  xrefAppendFlag = FALSE;
  insidePre      = FALSE;
  outputXRef.resize(0);
  setOutput( isBrief || isJavaDocStyle ? OutputBrief : OutputDoc );
  briefEndsAtDot = isJavaDocStyle;
  commentScanYYrestart( commentScanYYin );
  BEGIN( Comment );
  commentScanYYlex();
  setOutput( OutputDoc );

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

  Debug::print(Debug::CommentScan,0,"CommentScanner\n%s:%d\n"
               "input=[%s]\nbrief=[%s]\ndocs=[%s]\n------------\n",
	       fileName.data(),lineNr,comment.data(),
	       current->brief.data(),current->doc.data()
              );
  
  checkFormula();
  prot = protection;

  return needNewEntry;
}


#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void commentScanYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

