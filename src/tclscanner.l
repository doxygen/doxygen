/*****************************************************************************
 * Parser for Tcl subset
 *
 * Copyright (C) 2010      by Rene Zaumseil
 * based on the work of Dimitri van Heesch.
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
#include <qdict.h>
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h" 
#include "pre.h"
#include "tclscanner.h"
#include "outputlist.h"
#include "membername.h"
#include "searchindex.h"
#include "commentcnv.h"
#include "bufstr.h"
#include "portable.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>
#include <qmap.h>

#define YY_NEVER_INTERACTIVE 1

#define MAX_INCLUDE_DEPTH 10

//! Application error.
#define tcl_err \
  printf("Error %d %s() in %s line %d! ",__LINE__,__FUNCTION__,tcl.file_name.data(),yylineno); \
  yy_push_state(ERROR); \
  yyless(0); \
  printf

//! Application warning.
#define tcl_war \
  printf("Warning %d %s() in %s line %d: ",__LINE__,__FUNCTION__,tcl.file_name.data(),yylineno); \
  printf

//! Application message.
#define tcl_inf \
  if (0) printf("--- %.4d %s() %d@%d: ",__LINE__,__FUNCTION__,yylineno,yy_start_stack_ptr) && printf

//! Debug message.
#define D\
  if (0) printf("--- %.4d %s() %d@%d: %s\n",__LINE__,__FUNCTION__,yylineno,yy_start_stack_ptr,yytext);

// BEGIN of copy from tclUtil.c
// - Tcl_Interp removed
// - changes are marked with RZ
// #define's to adapt the code:
#define CONST		const
#define UCHAR		(unsigned char)
#define TCL_ERROR	1
#define TCL_OK		0
#define ckalloc		malloc
#define ckfree		free
#define TclCopyAndCollapse(size,src,dest) memcpy(dest,src,size); *(dest+size)=0
int TclFindElement(
      CONST char *list,		/* Points to the first byte of a string
				 * containing a Tcl list with zero or more
				 * elements (possibly in braces). */
      int listLength,		/* Number of bytes in the list's string. */
      CONST char **elementPtr,	/* Where to put address of first significant
				 * character in first element of list. */
      CONST char **nextPtr,	/* Fill in with location of character just
				 * after all white space following end of
				 * argument (next arg or end of list). */
      int *sizePtr,		/* If non-zero, fill in with size of
				 * element. */
      int *bracePtr)		/* If non-zero, fill in with non-zero/zero to
				 * indicate that arg was/wasn't in braces. */
{
  CONST char *p = list;
  CONST char *elemStart;	/* Points to first byte of first element. */
  CONST char *limit;		/* Points just after list's last byte. */
  int openBraces = 0;		/* Brace nesting level during parse. */
  int inQuotes = 0;
  int size = 0;		/* lint. */
  //RZ    int numChars;

  /*
   * Skim off leading white space and check for an opening brace or quote.
   * We treat embedded NULLs in the list as bytes belonging to a list
   * element.
   */

  limit = (list + listLength);
  while ((p < limit) && (isspace(UCHAR(*p)))) 
  { /* INTL: ISO space. */
    p++;
  }
  if (p == limit) 
  {		/* no element found */
    elemStart = limit;
    goto done;
  }

  if (*p == '{') 
  {
    openBraces = 1;
    p++;
  } 
  else if (*p == '"') 
  {
    inQuotes = 1;
    p++;
  }
  elemStart = p;
  if (bracePtr != 0) 
  {
    *bracePtr = openBraces;
  }

  /*
   * Find element's end (a space, close brace, or the end of the string).
   */

  while (p < limit) 
  {
    switch (*p) 
    {
      /*
       * Open brace: don't treat specially unless the element is in
       * braces. In this case, keep a nesting count.
       */

      case '{':
	if (openBraces != 0) 
	{
	  openBraces++;
	}
	break;

	/*
	 * Close brace: if element is in braces, keep nesting count and
	 * quit when the last close brace is seen.
	 */

      case '}':
	if (openBraces > 1) 
	{
	  openBraces--;
	} 
	else if (openBraces == 1) 
	{
	  size = (p - elemStart);
	  p++;
	  if ((p >= limit) || isspace(UCHAR(*p))) 
	  {	/* INTL: ISO space. */
	    goto done;
	  }

	  /*
	   * Garbage after the closing brace; return an error.
	   */

	  return TCL_ERROR;
	}
	break;

	/*
	 * Backslash: skip over everything up to the end of the backslash
	 * sequence.
	 */

      case '\\':
	//RZ	    Tcl_UtfBackslash(p, &numChars, NULL);
	//RZ	    p += (numChars - 1);
	p++; //RZ
	break;

	/*
	 * Space: ignore if element is in braces or quotes; otherwise
	 * terminate element.
	 */

      case ' ':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
      case '\v':
	if ((openBraces == 0) && !inQuotes) 
	{
	  size = (p - elemStart);
	  goto done;
	}
	break;

	/*
	 * Double-quote: if element is in quotes then terminate it.
	 */

      case '"':
	if (inQuotes) 
	{
	  size = (p - elemStart);
	  p++;
	  if ((p >= limit) || isspace(UCHAR(*p))) 
	  {	/* INTL: ISO space */
	    goto done;
	  }

	  /*
	   * Garbage after the closing quote; return an error.
	   */
	  return TCL_ERROR;
	}
	break;
    }
    p++;
  }

  /*
   * End of list: terminate element.
   */

  if (p == limit) 
  {
    if (openBraces != 0) 
    {
      return TCL_ERROR;
    } 
    else if (inQuotes) 
    {
      return TCL_ERROR;
    }
    size = (p - elemStart);
  }

done:
  while ((p < limit) && (isspace(UCHAR(*p)))) 
  { /* INTL: ISO space. */
    p++;
  }
  *elementPtr = elemStart;
  *nextPtr = p;
  if (sizePtr != 0) 
  {
    *sizePtr = size;
  }
  return TCL_OK;
}

int Tcl_SplitList(
    CONST char *list,		/* Pointer to string with list structure. */
    int *argcPtr,		/* Pointer to location to fill in with the
				 * number of elements in the list. */
    CONST char ***argvPtr)	/* Pointer to place to store pointer to array
				 * of pointers to list elements. */
{
  CONST char **argv, *l, *element;
  char *p;
  int length, size, i, result, elSize, brace;

  /*
   * Figure out how much space to allocate. There must be enough space for
   * both the array of pointers and also for a copy of the list. To estimate
   * the number of pointers needed, count the number of space characters in
   * the list.
   */

  for (size = 2, l = list; *l != 0; l++) 
  {
    if (isspace(UCHAR(*l))) 
    {			/* INTL: ISO space. */
      size++;

      /*
       * Consecutive space can only count as a single list delimiter.
       */

      while (1) 
      {
	char next = *(l + 1);

	if (next == '\0') 
	{
	  break;
	}
	++l;
	if (isspace(UCHAR(next))) 
	{		/* INTL: ISO space. */
	  continue;
	}
	break;
      }
    }
  }
  length = l - list;
  argv = (CONST char **) ckalloc((unsigned)
      ((size * sizeof(char *)) + length + 1));
  for (i = 0, p = ((char *) argv) + size*sizeof(char *);
      *list != 0;  i++) 
  {
    CONST char *prevList = list;

    result = TclFindElement(list, length, &element, &list,
	&elSize, &brace);
    length -= (list - prevList);
    if (result != TCL_OK) 
    {
      ckfree((char *) argv);
      return result;
    }
    if (*element == 0) 
    {
      break;
    }
    if (i >= size) 
    {
      ckfree((char *) argv);
      return TCL_ERROR;
    }
    argv[i] = p;
    if (brace) 
    {
      memcpy(p, element, (size_t) elSize);
      p += elSize;
      *p = 0;
      p++;
    } 
    else 
    {
      TclCopyAndCollapse(elSize, element, p);
      p += elSize+1;
    }
  }

  argv[i] = NULL;
  *argvPtr = argv;
  *argcPtr = i;
  return TCL_OK;
}
// END of tclUtil.c

void tcl_split_list(QString &str, QStringList &list)
{
  int argc;
  const char **argv;

  list.clear();
  if (str.left(1)=="{" && str.right(1)=="}")
  {
    str=str.mid(1,str.length()-2);
  }
  else if (str.left(1)=="\"" && str.right(1)=="\"")
  {
    str=str.mid(1,str.length()-2);
  }
  if (Tcl_SplitList(str.ascii(),&argc,&argv) != TCL_OK)
  {
    list.append(str);
  }
  else
  {
    for (int i = 0; i < argc; i++)
    {
      list.append(argv[i]);
    }
    ckfree((char *) argv);
  }
}

//! Structure containing information about current scan context.
typedef struct
{
  int type;
  QCString string_after;
  int line0; // start line of scan context
  int line1; // end line of scan context
  YY_BUFFER_STATE buffer_state; // value of scan context
  QString ns; // current namespace
  Entry *entry_fn; // if set contains the current proc/method/constructor/destructor
  Entry *entry_cl; // if set contain the current class
  Entry *entry_scan; // current scan entry
  Protection protection; // current protections state
} tcl_scan;

//* Structure containing all internal global variables.
static struct
{
  CodeOutputInterface * code; // if set then we are codifying the file
  int code_line; // current line of code
  int code_linenumbers; // if true create line numbers in code
  const char *code_font; // used font to codify
  bool config_autobrief; // value of configuration option
  QMap<QString,QString> config_subst; // map of configuration option values
  QCString input_string; // file contents
  int input_position; // position in file
  QCString file_name; // name of used file
  ParserInterface *this_parser; // myself
  int command; // true if command was found
  int comment; // set true if comment was scaned
  int brace_level; // bookkeeping of braces
  int bracket_level; // bookkeeping of brackets
  int bracket_quote; // bookkeeping of quotes (toggles)
  int word_is; // type of current word
  int line_comment; // line number of comment
  int line_commentline; // line number of comment after command
  int line_command; // line number of command
  int line_body0; // start line of body
  int line_body1; // end line of body
  QString string_command; // contain current command
  QString string_commentline; // contain current comment after command
  QString string_commentcodify; // current comment string used in codifying
  QString string_comment; // contain current comment
  QString string_last; // contain last read word or part of word
  QString string; // temporary string value
  Entry*		entry_main; // top level entry
  Entry*		entry_file; // entry of current file
  Entry*		entry_current; // currently used entry
  Entry*		entry_inside; // contain entry of current scan context
  QStringList list_commandwords; // list of command words
  QList<tcl_scan> scan; // stack of scan contexts
  QDict<Entry> ns; // all read namespace entries
  QDict<Entry> cl; // all read class entries
  QDict<Entry> fn; // all read function entries
  QList<Entry> entry; // list of all created entries, will be deleted after codifying
  Protection protection; // current protections state
  MemberDef *memberdef; // contain current MemberDef when codifying
} tcl;

// scanner functions
static int yyread(char *buf,int max_size);
static void tcl_scan_start(int type, QString content, const char *after, QString ns, Entry *entry_cls, Entry *entry_fn);
static void tcl_scan_end();
static void tcl_comment(int what,const char *text);
static void tcl_word(int what,const char *text);
static void tcl_command(int what,const char *text);

// helper functions

//! Create new entry.
// @return new initialised entry
Entry* tcl_entry_new()
{
  Entry *myEntry = new Entry;
  myEntry->section    = Entry::EMPTY_SEC;
  myEntry->name       = "";
//  myEntry->type       = "";
  myEntry->brief      = "";
//  myEntry->doc        = "";
  myEntry->protection = Package;
//  myEntry->mtype      = Method;
//  myEntry->virt       = Normal;
//  myEntry->stat       = FALSE;
  myEntry->fileName   = tcl.file_name;
  myEntry->lang       = SrcLangExt_Tcl;
  initGroupInfo(myEntry);
  // collect entries
  if (tcl.code==NULL)
  {
    tcl.entry.insert(0,myEntry);
  }
  return myEntry;
}

//! Set protection level.
void tcl_protection(Entry *entry) 
{
  if (entry->protection!=Public&&entry->protection!=Protected&&entry->protection!=Private)
  {
    entry->protection = tcl.protection;
  }
  if (entry->protection!=Protected&&entry->protection!=Private)
  {
    entry->protection = Public;
  }
}

//! Check name.
// @return 'ns' and 'name' of given current 'ns0' and 'name0'
static void tcl_name(const QString ns0, const QString name0, QString &ns, QString &name)
{
  QString myNm;
  int myStart;

  if (strncmp(name0.ascii(),"::",2)==0)
  {
    myNm = name0.mid(2);
  }
  else if (ns0.length())
  {
    myNm = ns0 + "::" + name0;
  }
  else
  {
    myNm = name0;
  }
  myStart = myNm.findRev("::");
  if (myStart == -1)
  {
    ns = "";
    name = myNm;
  }
  else
  {
    ns = myNm.mid(0,myStart);
    name = myNm.mid(myStart+2);
  }
}

// Check and return namespace entry.
// @return namespace entry
Entry* tcl_entry_namespace(const QString ns)
{
  Entry *myEntry;
  QString myNs = " ";
  if (strlen(ns.ascii())) {myNs = ns;}

  myEntry = tcl.ns.find(myNs);
  if (myEntry == NULL)
  {
    myEntry = tcl_entry_new();
    myEntry->section    = Entry::NAMESPACE_SEC;
    myEntry->name       = ns.ascii();
    tcl.entry_main->addSubEntry(myEntry);
    tcl.ns.insert(ns,myEntry);
  }
  return myEntry;
}

// Check and return class entry.
// @return class entry
Entry* tcl_entry_class(const QString cl)
{
  Entry *myEntry;
  QString myCl = " ";
  if (strlen(cl.ascii())) {myCl = cl;}

  myEntry = tcl.cl.find(myCl);
  if (myEntry == NULL)
  {
    myEntry = tcl_entry_new();
    myEntry->section    = Entry::CLASS_SEC;
    myEntry->name       = cl.ascii();
    tcl.entry_main->addSubEntry(myEntry);
    tcl.cl.insert(cl,myEntry);
  }
  return myEntry;
}

//! Check for keywords.
// @return 1 if keyword and 0 otherwise
static int tcl_keyword(QString str)
{
  static QStringList myList;
  static int myInit=1;
  if (myInit)
  {
    // tcl keywords TODO default, else, provide, require
    myList <<"append"<<"apply"<<"array"<<"auto_execok"<<"auto_import"<<"auto_load"<<"auto_mkindex"<<"auto_qualify"<<"auto_reset";
    myList <<"binary";
    myList <<"catch"<<"cd"<<"close"<<"clock"<<"concat";
    myList <<"eof"<<"eval"<<"exec"<<"exit"<<"expr";
    myList <<"fblocked"<<"fconfigure"<<"file"<<"fileevent"<<"flush"<<"for"<<"foreach"<<"format";
    myList <<"gets"<<"global";
    myList <<"http";
    myList <<"if"<<"incr"<<"info"<<"interp";
    myList <<"join";
    myList <<"lappend"<<"lassign"<<"lindex"<<"linsert"<<"llength"<<"load"<<"lrange"<<"lrepeat"<<"lreplace"<<"lreverse"<<"lset";
    myList <<"namespace";
    myList <<"package"<<"parray"<<"pid"<<"pkg_mkIndex"<<"proc"<<"puts"<<"pwd";
    myList <<"registry"<<"rename"<<"return";
    myList <<"scan"<<"set"<<"split"<<"string"<<"switch";
    myList <<"tclLog"<<"tcl_endOfWord"<<"tcl_findLibrary"<<"tcl_startOfNextWord"<<"tcl_startOfPreviousWord"<<"tcl_wordBreakAfter"<<"tcl_wordBreakBefore"<<"tell"<<"time";
    myList <<"unknown"<<"upvar";
    myList <<"variable"<<"vwait";
// tk keywords
    myList <<"bell"<<"bind"<<"bindtags";
    myList <<"clipboard"<<"console"<<"consoleinterp";
    myList <<"destroy";
    myList <<"event";
    myList <<"focus";
    myList <<"grid";
    myList <<"lower";
    myList <<"option";
    myList <<"pack"<<"place";
    myList <<"raise";
    myList <<"send";
    myList <<"tkerror"<<"tkwait"<<"tk_bisque"<<"tk_focusNext"<<"tk_focusPrev"<<"tk_focusFollowsMouse"<<"tk_popup"<<"tk_setPalette"<<"tk_textCut"<<"tk_TextCopy"<<"tk_textPaste"<<"chooseColor"<<"tk_chooseColor"<<"tk_chooseDirectory"<<"tk_dialog"<<"tk_getOpenFile"<<"tkDialog"<<"tk_getSaveFile"<<"tk_messageBox";
    myList <<"winfo"<<"wm";
    myList <<"button"<<"canvas"<<"checkbutton"<<"entry"<<"frame"<<"image"<<"label"<<"labelframe"<<"listbox"<<"menu"<<"menubutton"<<"message"<<"panedwindow"<<"radiobutton"<<"scale"<<"scrollbar"<<"spinbox"<<"toplevel";
    myList.sort();
    myInit=0;
  }
  str=str.stripWhiteSpace();
  if (str.left(2)=="::") {str=str.mid(2);}
  if (myList.findIndex(str) != -1) return(1);
  return 0;
}

//! End codifying with special font class.
static void tcl_font_end()
{
  if (tcl.code==NULL) return;
  if (tcl.code_font)
  {
    tcl.code->endFontClass();
    tcl.code_font=NULL;
  }
}

//! Codify 'str' with special font class 's'.
static void tcl_codify(const char *s,char *str)
{
  if (tcl.code==NULL||str==NULL) return;
  if (s)
  {
    tcl_font_end();
    tcl.code->startFontClass(s);
    tcl.code_font=s;
  }
  char *p=str,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') {}
    if (c=='\n')
    {
      tcl.code_line++;
      *(p-1)='\0';
      tcl.code->codify(sp);
      //tcl_font_end();
      tcl.code->endCodeLine();
      if (tcl.code_linenumbers)
      {
        if (tcl.code_font!=NULL)
	{
          tcl.code->endFontClass();
          tcl.code->writeLineNumber(0,0,0,tcl.code_line);
	  tcl.code->startFontClass(tcl.code_font);
	}
	else
	{
          tcl.code->writeLineNumber(0,0,0,tcl.code_line);
	}
      }
    }
    else
    {
      tcl.code->codify(sp);
      done=TRUE;
    }
  }
  tcl_font_end();
}

//! Codify 'str' with special font class 's'.
static void tcl_codify(const char *s,const char *str)
{
  if (tcl.code==NULL) return;
  char *tmp= (char *) malloc(strlen(str)+1);
  strcpy(tmp, str);
  tcl_codify(s,tmp);
  free(tmp);
}

//! Codify 'str' with special font class 's'.
static void tcl_codify(const char *s,QString str)
{
  if (tcl.code==NULL) return;
  char *tmp= (char *) malloc(str.length()+1);
  strcpy(tmp, str.data());
  tcl_codify(s,tmp);
  free(tmp);
}

//! Codify 'str' with special font class 's'.
static void tcl_codify(const char *s,QCString str)
{
  if (tcl.code==NULL) return;
  char *tmp= (char *) malloc(str.length()+1);
  strcpy(tmp, str);
  tcl_codify(s,tmp);
  free(tmp);
}

//-----------------------------------------------------------------------------
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);
//-----------------------------------------------------------------------------
%}
ws		([ \t]|\\\n)

%option yylineno
%option noyywrap
%option stack

%x ERROR
%x TOP
%x COMMAND
%x WORD
%x COMMENT
%x COMMENT_NL
%x COMMENT_CODE
%x COMMENT_VERB
%x COMMENTLINE
%x COMMENTLINE_NL
%x STRING
%x QUOTE
%x BRACE
%x BRACKET
%%
<ERROR>. {
D
  yyterminate();
}
<<EOF>> {
D
  if (tcl.scan.count()<1)
  {// error
D
    tcl_err("Tcl parser stack empty! Parser error in file '%s'.\n",tcl.file_name.data());
    yyterminate();
  }
  else if (tcl.scan.count()==1)
  {// exit, check on input?
D
    yyterminate();
  }
  else
  {// continue
D
    tcl_command(-1,"");
    tcl_scan_end();
  }
}
<TOP>"#" {
D
  yyless(0);
  tcl.line_comment=yylineno;
  tcl_comment(0,"");
}
<TOP>({ws}|[\;\n])+ {
D
  tcl_codify(NULL,yytext);
}
<TOP>. {
D
  yyless(0);
  tcl.line_command=yylineno;
  tcl_command(0,"");
}

<COMMENT>[ \t]* {
D
  tcl_codify("comment",yytext);
}
<COMMENT>"##".*\\\n {
D
  tcl_codify("comment",yytext);
  QString t=yytext;
  t = t.mid(2,t.length()-3);
  t.append('\n');
  tcl_comment(1,t.ascii());
  yy_push_state(COMMENT_NL);
}
<COMMENT>"##".*\n {
D
  tcl_codify("comment",yytext);
  tcl_comment(1,yytext+2);
}
<COMMENT>"#"[@\\]"code"\n[ \t]*[^#] {
D
  QString t=yytext;
  tcl_codify("comment",t.left(7));
  tcl_comment(2,"\n@code\n");
  yyless(7);
  yy_push_state(COMMENT_CODE);
}
<COMMENT>"#"[@\\]"verbatim"\n[ \t]*[^#] {
D
  QString t=yytext;
  tcl_codify("comment",t.left(11));
  tcl_comment(2,"\n@verbatim\n");
  yyless(11);
  yy_push_state(COMMENT_VERB);
}
<COMMENT>"#".*\\\n {
D
  tcl_codify("comment",yytext);
  QString t=yytext;
  t = t.mid(1,t.length()-3);
  t.append('\n');
  tcl_comment(2,t.ascii());
  yy_push_state(COMMENT_NL);
}
<COMMENT>"#".*\n {
D
  tcl_codify("comment",yytext);
  tcl_comment(2,yytext+1);
}
<COMMENT>"#".*\x1A {
D
  QString t=yytext;
  t = t.mid(0,t.length()-1);
  tcl_codify("comment",t.ascii());
  t = t.mid(1,t.length());
  tcl_comment(-2,t.ascii());
  unput(0x1A);
}
<COMMENT>\x1A {
D
  tcl_comment(-2,"");
  unput(0x1A);
}
<COMMENT>.|\n {
D
  tcl_comment(-2,yytext);
  yyless(0);
}

<COMMENT_CODE>"#"[@\\]"endcode"\n {
D
  QString t=yytext;
  t = t.left(t.length()-10);
  tcl_comment(2,t.ascii());
  tcl_comment(2,"\n@endcode\n");
  yy_pop_state();
  yyless(0);
}
<COMMENT_CODE>.*\n {
D
  yymore();
}
<COMMENT_CODE>.*\x1A {
D
  yy_pop_state();
  yyless(0);
}

<COMMENT_VERB>"#"[@\\]"endverbatim"\n {
D
  QString t=yytext;
  t = t.left(t.length()-14);
  tcl_comment(2,t.ascii());
  tcl_comment(2,"\n@endverbatim\n");
  yy_pop_state();
  yyless(0);
}
<COMMENT_VERB>.*\n {
D
  yymore();
}
<COMMENT_VERB>.*\x1A {
D
  yy_pop_state();
  yyless(0);
}

<COMMENT_NL>.*\\\n {
D
  tcl_codify("comment",yytext);
  tcl_comment(2,yytext);
}
<COMMENT_NL>.*\n {
D
  tcl_codify("comment",yytext);
  tcl_comment(2,yytext);
  yy_pop_state();
}
<COMMENT_NL>.*\x1A {
D
  yy_pop_state();
  yyless(0);
}

<COMMENTLINE>.*\x1A {
D
  yy_pop_state();
  yyless(0);
}
<COMMENTLINE>[ \t]* {
D
  tcl.string_commentcodify += yytext;
}
<COMMENTLINE>"#<".*\\\n {
D
  tcl.string_commentcodify += yytext;
  QString t=yytext;
  t = t.mid(2,t.length()-4);
  t.append('\n');
  tcl.string_commentline += t;
  yy_push_state(COMMENTLINE_NL);
}
<COMMENTLINE>"#<".*\n {
D
  tcl.string_commentcodify += yytext;
  tcl.string_commentline += (yytext+2);
}
<COMMENTLINE>.|\n {
D
  yy_pop_state();
  if (tcl.string_commentline.length())
  {
    tcl.entry_current->brief = tcl.string_commentline;
    tcl.entry_current->briefLine = tcl.line_commentline;
    tcl.entry_current->briefFile = tcl.file_name;
  }
  yyless(0);
  tcl_command(-1,tcl.string_commentcodify.ascii());
  tcl.string_commentline="";
  tcl.string_commentcodify="";
}

<COMMENTLINE_NL>.*\\\n {
D
  tcl.string_commentcodify += yytext;
  QString t=yytext;
  t = t.left(t.length()-3);
  t.append('\n');
  tcl.string_commentline += t;
}
<COMMENTLINE_NL>.*\n {
D
  tcl.string_commentcodify += yytext;
  tcl.string_commentline += yytext;
  yy_pop_state();
}
<COMMENTLINE_NL>.*\x1A {
D
  QString t=yytext;
  t = t.left(t.length()-1);
  tcl.string_commentcodify += t;
  tcl.string_commentline += t;
  yy_pop_state();
  unput(0x1A);
}

<COMMAND>{ws}*[\;]{ws}*"#<" {
D
  tcl.string_commentcodify = yytext;
  tcl.string_commentcodify = tcl.string_commentcodify.left(tcl.string_commentcodify.length()-2);
  tcl.string_commentline = "";
  tcl.line_commentline = yylineno;
  tcl.line_body1=yylineno;
  unput('<');
  unput('#');
  yy_push_state(COMMENTLINE);
}
<COMMAND>{ws}*\x1A {
D
  tcl.string_commentcodify = "";
  tcl.string_commentline = "";
  tcl.line_body1=yylineno;
  tcl_command(-1,"");
}
<COMMAND>{ws}*; {
D
  tcl.string_commentcodify = "";
  tcl.string_commentline = "";
  tcl.line_body1=yylineno;
  tcl_command(-1,yytext);
}
<COMMAND>{ws}*\n {
D
  tcl.string_commentcodify = "";
  tcl.string_commentline = "";
  tcl.line_body1=yylineno-1;
  tcl_command(-1,yytext);
}
<COMMAND>{ws}+ {
D
  tcl_command(1,yytext);
}
<COMMAND>"{*}". {
D
  tcl.word_is = WORD;
  tcl.string_last = "{*}";
  tcl_word(0,&yytext[3]);
}
<COMMAND>. {
D
  switch (yytext[0])
  {
    case '{': tcl.word_is = BRACE; break;
    case '[': tcl.word_is = BRACKET; break;
    case '"': tcl.word_is = QUOTE; break;
    default: tcl.word_is = WORD;
  }
  tcl.string_last = "";
  tcl_word(0,yytext);
}

<WORD>"\\\\" |
<WORD>"\\"[\{\}\[\]\;\" \t] {
  tcl_word(1,yytext);
}
<WORD>"\\\n" {
  tcl_word(2,yytext);
}
<WORD>"{" {
  tcl_word(3,yytext);
}
<WORD>"}" {
  tcl_word(4,yytext);
}
<WORD>"[" {
  tcl_word(5,yytext);
}
<WORD>"]" {
  tcl_word(6,yytext);
}
<WORD>"\"" {
  tcl_word(7,yytext);
}
<WORD>" " {
  tcl_word(8,yytext);
}
<WORD>"\t" {
  tcl_word(9,yytext);
}
<WORD>";" {
  tcl_word(10,yytext);
}
<WORD>"\n" {
  tcl_word(11,yytext);
}
<WORD>\x1A {
  tcl_word(12,yytext);
}
<WORD>. {
  tcl_word(1,yytext);
}
%%

//! Start new scan context for given 'content'.
// @return created new scan context.
static void tcl_scan_start(int type, QString content, const char *after, QString ns, Entry *entry_cl, Entry *entry_fn)
{
  tcl_scan *myScan=tcl.scan.at(0);
  QString myName;
  char c[2]=" ";
tcl_inf("line=%d type=%d '%s'\n",tcl.line_body0,type,content.ascii());

  myScan->line1=yylineno;
  yy_push_state(TOP);

  myScan=new tcl_scan;
  myScan->type = type;
  myScan->string_after=after;

  switch (myScan->type)
  {
    case QUOTE: c[0]='"';
      break;
    case BRACE: c[0]='{';
      break;
    case BRACKET: c[0]='[';
      break;
    default:
      break;
  }
  if (c[0]!=' ')
  {
    tcl_codify(NULL,c);
    content = content.mid(1,content.length()-2);
  }
  content += (char)0x1A;// for detection end of scan context
  myScan->ns = ns;
  myScan->entry_cl = entry_cl;
  myScan->entry_fn = entry_fn;
  myScan->entry_scan = tcl.entry_current;
  myScan->buffer_state=yy_scan_string(content.ascii());
  myScan->line0=tcl.line_body0;
  myScan->line1=tcl.line_body1;
  yylineno=myScan->line0;
  myScan->protection = tcl.protection;

  tcl.entry_inside = myScan->entry_scan;
  tcl.entry_current = tcl_entry_new();
  tcl.scan.insert(0,myScan);
  yy_switch_to_buffer(myScan->buffer_state);
}

//! Close current scan context.
static void tcl_scan_end()
{
  tcl_scan *myScan=tcl.scan.at(0);
  char c[2]=" ";
tcl_inf("line=%d\n",myScan->line1);

  switch (myScan->type)
  {
    case QUOTE: c[0]='"'; break;
    case BRACE: c[0]='}'; break;
    case BRACKET: c[0]=']'; break;
  }
  if (c[0]!=' ') {tcl_codify(NULL,c);}
  if (myScan->string_after.length())
  {
    tcl_codify("comment",myScan->string_after);
  }
  yy_delete_buffer(myScan->buffer_state);
  tcl.scan.removeFirst();
  yy_pop_state();
  myScan=tcl.scan.at(0);
  tcl.entry_inside = myScan->entry_scan;
  yy_switch_to_buffer(myScan->buffer_state);
  yylineno=myScan->line1;
  tcl.protection = myScan->protection;
}

//! Handling of word parsing.
static void tcl_word(int what,const char *text) 
{
  static char myList[1024]="";// nesting level list
  static int myLevel=0;// number of current nesting level
  static int myWhite=0;// set true when next char should be whitespace
  static char myWord;// internal state

  switch (what)
  {
  case 0:// start
    yy_push_state(WORD);
    switch (text[0])
    {
      case '{':
      case '[':
      case '"': myWord = text[0]; break;
      default: myWord = '.';
    }
    myList[0]=myWord;
    myLevel=1;
    myWhite=0;
  break;
  case 1:// all other chars
    if (myWhite)
    {// {x}y "x"y
      tcl_err("expected word separator: %s\n",text);
      return;
    }
    if (myLevel==0)
    {
      myWord='.';
      myList[0]=myWord;
      myLevel=1;
    }
  break;
  case 2:// \\\n
    if (myLevel==0)
    {
      myWord=' ';
      yy_pop_state();
      yyless(0);
tcl_inf("(\\\n) ?%s?\n",tcl.string_last.ascii());
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':
      case '[':
      case '"':
      break;
      case '.':
        if (myLevel==1)
        {
          myWord=' ';
          yy_pop_state();
          yyless(0);
tcl_inf("(\\\n) ?%s?\n",tcl.string_last.ascii());
          return;
        }
      break;
    }
    myWhite=0;
  break;
  case 3:// {
    if (myWhite)
    {// {x}{ "x"{
      tcl_err("expected word separator: %s\n",text);
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':
      case '[':
        myList[myLevel++]='{';
      break;
      case '"':
      case '.':
      break;
    }
    myWhite=0;
  break;
  case 4:// }
    if (myWhite)
    {// {x}{ "x"{
      tcl_err("expected word separator: %s\n",text);
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':// {{x}}
        myLevel--;
        if (myLevel==0 && tcl.code==NULL) 
	{
          myWhite=1;
        }
      break;
      case '[':
      case '"':
      case '.':
      break;
    }
  break;
  case 5:// [
    if (myWhite)
    {// {x}[
      tcl_err("expected word separator: %s\n",text);
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':
      break;
      case '[':
      case '"':
      case '.':
        myList[myLevel++]='[';
      break;
    }
    myWhite=0;
  break;
  case 6:// ]
    if (myWhite)
    {// {x}]
      tcl_err("expected word separator: %s\n",text);
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':
      break;
      case '[':
	myLevel--;
      break;
      case '"':
      case '.':
      break;
    }
    myWhite=0;
  break;
  case 7:// "
    if (myWhite)
    {// {x}"
      tcl_err("expected word separator: %s\n",text);
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':
      break;
      case '[':
        myList[myLevel++]='"';
      break;
      case '"':
        myLevel--;
      case '.':
      break;
    }
  break;
  case 8:// ' '
  case 9:// \t
  case 10:// ;
  case 11:// \n
    if (myLevel==0)
    {
      myWord=' ';
      yy_pop_state();
      yyless(0);
tcl_inf("(%d) ?%s?\n",what,tcl.string_last.ascii());
      return;
    }
    switch (myList[myLevel-1])
    {
      case '{':
      case '[':
      case '"':
      break;
      case '.':
        if (myLevel==1)
        {
          myWord=' ';
          yy_pop_state();
          yyless(0);
tcl_inf("(.%d) ?%s?\n",what,tcl.string_last.ascii());
          return;
	}
        else
        {
          myLevel--;
        }
      break;
    }
    myWhite=0;
  break;
  case 12:// \x1A
    if (myLevel==0)
    {
      myWord=' ';
      yy_pop_state();
      yyless(0);
tcl_inf("(%d) ?%s?\n",what,tcl.string_last.ascii());
      return;
    }
    if (myLevel!=1 || myList[0] != '.')
    {
      tcl_war("level=%d expected=%c\n",myLevel,myList[myLevel-1]);
    }
    myWord=' ';
    yy_pop_state();
    yyless(0);
tcl_inf("(.%d) ?%s?\n",what,tcl.string_last.ascii());
    return;
    myWhite=0;
  break;
  default:
    tcl_err("wrong state: %d\n",what);
    return;
  }
  tcl.string_last += text;
}

//! Handling of comment parsing.
static void tcl_comment(int what,const char *text)
{
  if (what==0)
  { // begin of comment
    if (tcl.comment)
    {
      tcl_err("comment in comment\n");
      return;
    }
    yy_push_state(COMMENT);
tcl_inf("<- %s\n",text);
    tcl.string_comment="";
    tcl.comment=0;
  }
  else if (what==1)
  { // start new comment
    if (tcl.comment)
    {
      tcl_comment(99,""); // inbody
    }
    tcl.string_comment=text;
    tcl.comment=1;
  }
  else if (what==2)
  { // add to comment
    if (tcl.comment)
    {
      tcl.string_comment+=text;
    }
  }
  else if (what==-1 || what == -2)
  { // end of comment without/with command
    if (tcl.comment)
    {
      tcl.string_last=tcl.string_comment;
      tcl_comment(100+what,"");
    }
    else
    {
      tcl.string_last = "";
tcl_inf("-> %s\n",(const char *)tcl.string_comment);
    }
    yy_pop_state();
    tcl.string_comment="";
    tcl.comment=0;
  }
  else if (what==98 || what==99)
  { // 98=new 99=inbody
    if (tcl.this_parser && tcl.string_comment.length())
    {
tcl_inf("-> %s\n",(const char *)tcl.string_comment);
      int myPos=0;
      bool myNew=0;
      int myLine=tcl.line_comment;
      BufStr myI(1024);
      BufStr myO(1024);
      Protection myProt=tcl.protection;

      // resolve ALIASES
      myI.addArray("/*!",3);
      myI.addArray(tcl.string_comment.ascii(),tcl.string_comment.length());
      myI.addArray("*/",2);
      convertCppComments(&myI,&myO,tcl.file_name);
      myO.dropFromStart(3);
      myO.shrink(myO.curPos()-2);
      myO.addChar('\0');
      QCString myDoc = myO.data();
      if (what==99)
      { // inbody comment file or namespace or class or proc/method
        int myPos0;
        int myLine0;
        Entry myEntry0; // used to test parsing
        Entry *myEntry;

        Entry *myEntry1=NULL;
        if (tcl.scan.at(0)->entry_fn)
        {
          myEntry1=tcl.scan.at(0)->entry_fn;
        }
        else if (tcl.scan.at(0)->entry_cl)
        {
          myEntry1=tcl.scan.at(0)->entry_cl;
        }

        myPos0=myPos;
        myLine0=myLine;
        while (parseCommentBlock(tcl.this_parser, &myEntry0, myDoc, tcl.file_name,
	  myLine, FALSE, tcl.config_autobrief, FALSE, myProt, myPos, myNew))
        {
          if (myNew)
          { // we need a new entry in this case
            myNew=0;
            myEntry = tcl_entry_new();
            parseCommentBlock(tcl.this_parser, myEntry, myDoc, tcl.file_name,
	      myLine0, FALSE, tcl.config_autobrief, FALSE, myProt, myPos0, myNew);
	    tcl.entry_inside->addSubEntry(myEntry);
          }
          else
          { // we can add to current entry in this case
            if (myEntry1==NULL)
            {
              myEntry1=tcl_entry_namespace(tcl.scan.at(0)->ns);
            }
            parseCommentBlock(tcl.this_parser, myEntry1, myDoc, tcl.file_name,
	      myLine0, FALSE, tcl.config_autobrief, FALSE, myProt, myPos0, myNew);
          }
          myPos0=myPos;
          myLine0=myLine;
        }
        if (myNew)
        { // we need a new entry
          myNew=0;
          myEntry = tcl_entry_new();
          parseCommentBlock(tcl.this_parser, myEntry, myDoc, tcl.file_name,
	    myLine0, FALSE, tcl.config_autobrief, FALSE, myProt, myPos0, myNew);
	  tcl.entry_inside->addSubEntry(myEntry);
        }
        else
        { // we can add to current entry
          if (myEntry1==NULL)
          {
            myEntry1=tcl_entry_namespace(tcl.scan.at(0)->ns);
          }
          parseCommentBlock(tcl.this_parser, myEntry1, myDoc, tcl.file_name,
	    myLine0, FALSE, tcl.config_autobrief, FALSE, myProt, myPos0, myNew);
        }
      }
      else
      { // new entry
        tcl.entry_current = tcl_entry_new();
        while (parseCommentBlock(tcl.this_parser, tcl.entry_current, myDoc,
  	  tcl.file_name, myLine, FALSE, tcl.config_autobrief, FALSE,
	  myProt, myPos, myNew))
        {
          if (myNew)
          {
	    tcl.entry_inside->addSubEntry(tcl.entry_current);
	    tcl.entry_current = tcl_entry_new();
          }
          else
          {
	    tcl.entry_current->section = tcl.entry_inside->section;
	    tcl.entry_current->name = tcl.entry_inside->name;
          }
        }
        if (myNew)
        {
          tcl.entry_inside->addSubEntry(tcl.entry_current);
          tcl.entry_current = tcl_entry_new();
        }
        else
        {
          tcl.entry_current->section = tcl.entry_inside->section;
          tcl.entry_current->name = tcl.entry_inside->name;
        }
      }
      if (tcl.protection != myProt)
      {
        tcl.scan.at(0)->protection = tcl.protection = myProt;
      }
    }
  }
  else
  {
    tcl_err("what %d\n",what);
    return;
  }
}

//! Parse given \c arglist .
static void tcl_command_ARGLIST(QString &arglist)
{
D
  Argument *myArg;
  QStringList myArgs;
  QString myArglist="";

  if (tcl.entry_current->argList==NULL)
  {
    tcl.entry_current->argList=new ArgumentList;
  }
  tcl_split_list(arglist,myArgs);
  for (uint i=0;i<myArgs.count();i++)
  {
    QStringList myArgs1;
    myArg=new Argument;

    tcl_split_list(*myArgs.at(i),myArgs1);
    if (myArgs1.count()==2)
    {
      myArg->name= *myArgs1.at(0);
      myArg->defval= *myArgs1.at(1);
      if (myArg->defval.isEmpty())
      {
	myArg->defval = " ";
      }
      myArglist = myArglist + "?" + myArg->name.data() + "? ";
    }
    else
    {
      myArg->name= *myArgs.at(i);
      myArglist = myArglist + myArg->name.data() + " ";
    }
    tcl.entry_current->argList->append(myArg);
  }
  arglist = myArglist;
  tcl.entry_current->args = arglist;
}

//! Create link.
static void tcl_codify_link(const char *name)
{
  if (tcl.code == NULL || name == NULL) return;
  static int init=0;
  static QDict<MemberDef> fn;
  if (init==0) 
  {
    init=1;
    MemberNameSDict::Iterator mni(*Doxygen::memberNameSDict);
    MemberNameSDict::Iterator fni(*Doxygen::functionNameSDict);
    MemberName *mn=0;
    MemberDef *md;
    for (mni.toFirst();(mn=mni.current());++mni) 
    {
      MemberNameIterator mi(*mn);
      for (mi.toFirst();(md=mi.current());++mi) 
      {
	fn.insert(md->qualifiedName(),md);
      }
    }
    for (fni.toFirst();(mn=fni.current());++fni) 
    {
      MemberNameIterator fi(*mn);
      for (fi.toFirst();(md=fi.current());++fi) 
      {
	fn.insert(md->qualifiedName(),md);
      }
    }
  }
  MemberDef *myDef;
  QString myName=name; 

  if (strncmp(name,"::",2)==0) // fully qualified global command
  {
    myName = myName.mid(2);
    myDef = fn.find(myName);
  }
  else // not qualified name
  {
    myDef = NULL;
    myName = tcl.scan.at(0)->ns;
    myName = myName + "::" + myName;
    myDef = fn.find(myName); // search namespace command
    if (myDef == NULL)
    {
      myDef = fn.find(name); // search global command
    }
  }
  if (myDef != NULL) // documented command
  {
    tcl.code->writeCodeLink(myDef->getReference().data(),
		myDef->getOutputFileBase().data(),
		myDef->anchor().data(),
		name,
		myDef->qualifiedName().data());
    if (tcl.memberdef)
    {
	myDef->addSourceReferencedBy(tcl.memberdef);
	tcl.memberdef->addSourceReferences(myDef);
    }
  }
  else if (tcl_keyword(myName)) // check keyword
  {
    tcl_codify("keyword",name);
  }
  else
  {
    tcl_codify(NULL,name); // something else
  }
  
}
//! Handle all other commands.
// Create links of first command word or first command word inside [].
static void tcl_command_OTHER(const char *text)
{
  if (tcl.code == NULL) return;
D
  QCString myName; 

  for (unsigned int i=0; i< tcl.list_commandwords.count(); i++)
  {
    myName = *tcl.list_commandwords.at(i);
    if (i==0)
    {
      tcl_codify_link(myName.data());
    }
    else //TODO check on [... inside current string
    {
      QCString myStr="";
      int myCmd=0;
      for (unsigned int i=0;i<myName.length();i++)
      {
        char c = myName[i];
	if (myCmd) 
	{
	  if (c==' '||c=='\t'||c=='\n'||c==']') 
	  {//end of command
	    tcl_codify_link(myStr.data());
	    myStr="";
	    myCmd=0;
	  }
	  myStr+=c;
	} 
	else 
	{
	  myStr+=c;
	  if (c=='[') 
	  {//start of command
            tcl_codify(NULL,myStr);
	    myStr="";
	    myCmd=1;
	  }
	}
      }
      tcl_codify(NULL,myStr);
    }
  }
  tcl_codify(NULL,text);
}

//! Handle \c proc statements.
static void tcl_command_PROC(const char *text)
{
D
  QString myNs, myName;
  Entry *myEntryNs, *myEntry;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify(NULL,*tcl.list_commandwords.at(2));
  tcl_codify(NULL,*tcl.list_commandwords.at(3));
  tcl_codify(NULL,*tcl.list_commandwords.at(4));
  tcl_codify(NULL,*tcl.list_commandwords.at(5));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(2),myNs,myName);
  if (myNs.length())
  {
    myEntryNs = tcl_entry_namespace(myNs);
  }
  else
  {
    myEntryNs = tcl_entry_namespace(myScan->ns);
  }
  //why not needed here? tcl.fn.remove(myName);
  tcl.entry_current->section = Entry::FUNCTION_SEC;
  tcl.entry_current->mtype = Method;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl_protection(tcl.entry_current);
  tcl_command_ARGLIST(*tcl.list_commandwords.at(4));
  myEntryNs->addSubEntry(tcl.entry_current);
  myEntry = tcl.entry_current;
  tcl.fn.insert(myName,myEntry);
  tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(6),text,
	myEntryNs->name,NULL,myEntry);
}

//! Handle \c itcl::body statements and \c oo::define method and method inside \c itcl::class statements.
static void tcl_command_METHOD(const char *text)
{
D
  QString myNs, myName;
  Entry *myEntryCl, *myEntry;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify(NULL,*tcl.list_commandwords.at(2));
  tcl_codify(NULL,*tcl.list_commandwords.at(3));
  tcl_codify(NULL,*tcl.list_commandwords.at(4));
  tcl_codify(NULL,*tcl.list_commandwords.at(5));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(2),myNs,myName);
  if (myNs.length())
  {
    myEntryCl = tcl_entry_class(myNs);
  }
  else
  {
    myNs = myScan->ns;
    myEntryCl = myScan->entry_cl;
  }
  // needed in case of more then one definition p.e. itcl::method and itcl::body
  // see also bug #
  tcl.fn.remove(myName);
  tcl.entry_current->section = Entry::FUNCTION_SEC;
  tcl.entry_current->mtype = Method;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl_protection(tcl.entry_current);
  tcl_command_ARGLIST(*tcl.list_commandwords.at(4));
  myEntryCl->addSubEntry(tcl.entry_current);
  tcl.fn.insert(myName,tcl.entry_current);
  myEntry = tcl.entry_current;
  tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(6),text,
	myNs, myEntryCl, myEntry);
}

//! Handle \c constructor statements inside class definitions.
static void tcl_command_CONSTRUCTOR(const char *text)
{
D
  QString myNs, myName;
  Entry *myEntryCl, *myEntry;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify(NULL,*tcl.list_commandwords.at(2));
  tcl_codify(NULL,*tcl.list_commandwords.at(3));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(0),myNs,myName);
  if (myNs.length())
  {
    myEntryCl = tcl_entry_class(myNs);
  }
  else
  {
    myNs = myScan->ns;
    myEntryCl = myScan->entry_cl;
  }
  tcl.entry_current->section = Entry::FUNCTION_SEC;
  tcl.entry_current->mtype = Method;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl_protection(tcl.entry_current);
  tcl_command_ARGLIST(*tcl.list_commandwords.at(2));
  myEntryCl->addSubEntry(tcl.entry_current);
  myEntry = tcl.entry_current;
  tcl.fn.insert(myName,myEntry);
  tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(4),text,
	myNs, myEntryCl, myEntry);
}

//! Handle \c destructor statements inside class definitions.
static void tcl_command_DESTRUCTOR(const char *text)
{
D
  QString myNs, myName;
  Entry *myEntryCl, *myEntry;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(0),myNs,myName);
  if (myNs.length())
  {
    myEntryCl = tcl_entry_class(myNs);
  }
  else
  {
    myNs = myScan->ns;
    myEntryCl = myScan->entry_cl;
  }
  tcl.entry_current->section = Entry::FUNCTION_SEC;
  tcl.entry_current->mtype = Method;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl_protection(tcl.entry_current);
  myEntryCl->addSubEntry(tcl.entry_current);
  myEntry = tcl.entry_current;
  tcl.fn.insert(myName,myEntry);
  tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(2),text,
	myNs, myEntryCl, myEntry);
}

//! Handle \c namespace statements.
static void tcl_command_NAMESPACE(const char *text)
{
D
  QString myNs, myName, myStr;
  Entry *myEntryNs=NULL;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify("keyword",*tcl.list_commandwords.at(2));
  tcl_codify(NULL,*tcl.list_commandwords.at(3));
  tcl_codify(NULL,*tcl.list_commandwords.at(4));
  tcl_codify(NULL,*tcl.list_commandwords.at(5));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(4),myNs,myName);
  if (myNs.length())
  {
    myName = myNs+"::"+myName;
  }
  tcl.entry_current->section = Entry::NAMESPACE_SEC;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl.entry_main->addSubEntry(tcl.entry_current);
  tcl.ns.insert(myName,tcl.entry_current);
  myEntryNs = tcl.entry_current;
  myStr = *tcl.list_commandwords.at(6);
  if (tcl.list_commandwords.count() > 7)
  {
    for (uint i=7;i<tcl.list_commandwords.count();i++)
    {
      myStr.append(*tcl.list_commandwords.at(i));
    }
    tcl.word_is=STRING;
  }
  tcl_scan_start(tcl.word_is,myStr,text, myName, NULL, NULL);
}

//! Handle \c itcl::class statements.
static void tcl_command_ITCL_CLASS(const char *text)
{
D
  QString myNs, myName, myStr;
  Entry *myEntryCl;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify("NULL",*tcl.list_commandwords.at(2));
  tcl_codify("NULL",*tcl.list_commandwords.at(3));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(2),myNs,myName);
  if (myNs.length())
  {
    myName = myNs+"::"+myName;
  }
  tcl.entry_current->section = Entry::CLASS_SEC;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl.entry_main->addSubEntry(tcl.entry_current);
  tcl.cl.insert(myName,tcl.entry_current);
  myEntryCl = tcl.entry_current;
  tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(4),text,
	myName, myEntryCl, NULL);
}

//! Handle \c oo::class statements.
static void tcl_command_OO_CLASS(const char *text)
{
D
  QString myNs, myName, myStr;
  Entry *myEntryNs, *myEntryCl;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify("NULL",*tcl.list_commandwords.at(2));
  tcl_codify("NULL",*tcl.list_commandwords.at(3));
  tcl_codify("NULL",*tcl.list_commandwords.at(4));
  tcl_codify("NULL",*tcl.list_commandwords.at(5));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(4),myNs,myName);
  if (myNs.length())
  {
    myName = myNs+"::"+myName;
  }
  tcl.entry_current->section = Entry::CLASS_SEC;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl.entry_main->addSubEntry(tcl.entry_current);
  myEntryNs = tcl_entry_namespace(myName);
  tcl.cl.insert(myName,tcl.entry_current);
  myEntryCl = tcl.entry_current;
  tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(6),text,
	myName, myEntryCl, NULL);
}

//! Handle \c oo::define statements.
static void tcl_command_OO_DEFINE(const char *text)
{
D
  QString myNs, myName, myStr;
  Entry *myEntryCl;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  tcl_codify(NULL,*tcl.list_commandwords.at(1));
  tcl_codify("NULL",*tcl.list_commandwords.at(2));
  tcl_codify("NULL",*tcl.list_commandwords.at(3));
  tcl_name(myScan->ns,*tcl.list_commandwords.at(2),myNs,myName);
  if (myNs.length())
  {
    myName = myNs+"::"+myName;
  }
  myEntryCl = tcl_entry_class(myName);
  myStr = *tcl.list_commandwords.at(4);
  if (tcl.list_commandwords.count() > 5)
  {
    for (uint i=5;i<tcl.list_commandwords.count();i++)
    {
      myStr.append(*tcl.list_commandwords.at(i));
    }
    tcl.word_is=STRING;
  }
  tcl_scan_start(tcl.word_is,myStr,text,myName,myEntryCl,NULL);
}

//! Handle \c variable statements.
static void tcl_command_VARIABLE(const char *text, int inclass)
{
D
  QString myNs, myName;
  Entry *myEntry;
  tcl_scan *myScan = tcl.scan.at(0);

  tcl_codify("keyword",*tcl.list_commandwords.at(0));
  for (unsigned int i=1; i< tcl.list_commandwords.count(); i++)
  {
    tcl_codify(NULL,*tcl.list_commandwords.at(i));
  }
  tcl_name(myScan->ns,*tcl.list_commandwords.at(2),myNs,myName);
  if (myNs.length())
  {// qualified variables go into namespace
    myEntry = tcl_entry_namespace(myNs);
    tcl.entry_current->stat = true;
  }
  else
  {
    if (inclass)
    {
      myEntry = myScan->entry_cl;
      tcl.entry_current->stat = false;
    }
    else
    {
      myEntry = tcl_entry_namespace(myScan->ns);
      tcl.entry_current->stat = true;
    }
  }
  tcl.entry_current->section = Entry::VARIABLE_SEC;
  tcl.entry_current->name = myName;
  tcl.entry_current->startLine = tcl.line_command;
  tcl.entry_current->bodyLine = tcl.line_body0;
  tcl.entry_current->endBodyLine = tcl.line_body1;
  tcl_protection(tcl.entry_current);
  myEntry->addSubEntry(tcl.entry_current);
  tcl.entry_current = tcl_entry_new();
  tcl_codify("comment",text);
}

//! Handling of command parsing.
static void tcl_command(int what,const char *text)
{
  int myLine=0;
  if (what==0)
  {
    tcl.scan.at(0)->line1=yylineno;// current line in scan context
    tcl.line_body0=yylineno;// start line of command
tcl_inf("<- %s\n",text);
    yy_push_state(COMMAND);
    tcl.list_commandwords.clear();
    tcl.string_command="";
    tcl.string_last="";
    tcl.command=1;
    return;
  }
  if (what==1)
  {
    if (tcl.string_last.length())
    {
      tcl.list_commandwords.append(tcl.string_last);
      tcl.string_last="";
    }
    if (text) 
    {
      tcl.list_commandwords.append(text);
    }
    return;
  }
  if (what!=-1)
  {// should not happen
    tcl_err("what %d\n",what);
    return;
  }
tcl_inf("->\n");
  if (tcl.command==0)
  {
    return;//TODO check on inside comment
  }
  if (tcl.string_last != "")
  {// get last word
    tcl.list_commandwords.append(tcl.string_last);
    tcl.string_last="";
  }
  yy_pop_state();

  // check command
  QString myStr = *tcl.list_commandwords.at(0);
  int myLevel = 0;
  Protection myProt = tcl.protection;

  if (tcl.list_commandwords.count() < 3)
  {
    tcl_command_OTHER(text);
    goto command_end;
  }
  // remove leading "::" and apply TCL_SUBST
  if (myStr.left(2)=="::") myStr = myStr.mid(2);
  if (tcl.config_subst.contains(myStr))
  {
    myStr=tcl.config_subst[myStr];
  }
  if (strcmp("private",myStr)==0)
  {
    tcl.protection = Private;
    myLevel = 1;
  }
  else if (strcmp("protected",myStr)==0)
  {
    tcl.protection = Protected;
    myLevel = 1;
  }
  else if (strcmp("public",myStr)==0)
  {
    tcl.protection = Public;
    myLevel = 1;
  }
  if (myLevel)
  {
    tcl_codify("keyword",*tcl.list_commandwords.at(0));
    tcl_codify(NULL,*tcl.list_commandwords.at(1));
    tcl.list_commandwords.remove(tcl.list_commandwords.at(1));
    tcl.list_commandwords.remove(tcl.list_commandwords.at(0));
    if (tcl.list_commandwords.count()==1)
    {
      tcl_scan *myScan = tcl.scan.at(0);
      tcl_scan_start(tcl.word_is,*tcl.list_commandwords.at(0),text,
	myScan->ns,myScan->entry_cl,myScan->entry_fn);
      myProt = tcl.protection;
      goto command_end;
    }
    myStr	= *tcl.list_commandwords.at(0);
    // remove leading "::" and apply TCL_SUBST
    if (myStr.left(2)=="::") myStr = myStr.mid(2);
    if (tcl.config_subst.contains(myStr))
    {
      myStr=tcl.config_subst[myStr];
    }
  }
  if (strcmp("proc",myStr)==0)
  {
    if (tcl.list_commandwords.count() == 5)
    {// itcl::proc
      tcl.list_commandwords.append("");
      tcl.list_commandwords.append("");
    }
    if (tcl.list_commandwords.count() != 7) {myLine=__LINE__;goto command_warn;}
    tcl_command_PROC(text);
    goto command_end;
  }
  if (strcmp("method",myStr)==0)
  {
    if (tcl.list_commandwords.count() == 5)
    {// itcl::method
      tcl.list_commandwords.append("");
      tcl.list_commandwords.append("");
    }
    if (tcl.list_commandwords.count() != 7) {myLine=__LINE__;goto command_warn;}
    tcl_command_METHOD(text);
    goto command_end;
  }
  if (strcmp("constructor",myStr)==0)
  {
    if (tcl.list_commandwords.count() != 5) {myLine=__LINE__;goto command_warn;}
    tcl_command_CONSTRUCTOR(text);
    goto command_end;
  }
  if (strcmp("destructor",myStr)==0)
  {
    if (tcl.list_commandwords.count() != 3) {myLine=__LINE__;goto command_warn;}
    tcl_command_DESTRUCTOR(text);
    goto command_end;
  }
  if (strcmp("namespace",myStr)==0)
  {
    if (strcmp("eval",*tcl.list_commandwords.at(2))==0)
    {
      if (tcl.list_commandwords.count() < 7) {myLine=__LINE__;goto command_warn;}
      tcl_command_NAMESPACE(text);
      goto command_end;
    }
    tcl_command_OTHER(text);
    goto command_end;
  }
  if (strcmp("itcl::class",myStr)==0)
  {
    if (tcl.list_commandwords.count() != 5) {myLine=__LINE__;goto command_warn;}
    tcl_command_ITCL_CLASS(text);
    goto command_end;
  }
  if (strcmp("itcl::body",myStr)==0)
  {
    if (tcl.list_commandwords.count() != 7) {myLine=__LINE__;goto command_warn;}
    tcl_command_METHOD(text);
    goto command_end;
  }
  if (strcmp("oo::class",myStr)==0)
  {
    if (strcmp("create",*tcl.list_commandwords.at(2))==0)
    {
      if (tcl.list_commandwords.count() != 7) {myLine=__LINE__;goto command_warn;}
      tcl_command_OO_CLASS(text);
      goto command_end;
    }
    tcl_command_OTHER(text);
    goto command_end;
  }
  if (strcmp("oo::define",myStr)==0)
  {
    if (tcl.list_commandwords.count() < 5) {myLine=__LINE__;goto command_warn;}
    tcl_command_OO_DEFINE(text);
    goto command_end;
  }
  if (strcmp("variable",myStr)==0)
  {
    if (tcl.list_commandwords.count() < 3) {myLine=__LINE__;goto command_warn;}
    if (tcl.scan.at(0)->entry_fn == NULL)
    {// only parsed outside functions
      tcl_command_VARIABLE(text,tcl.scan.at(0)->entry_cl!=NULL&&tcl.scan.at(0)->entry_cl->name!="");
      goto command_end;
    }
  }
  if (strcmp("common",myStr)==0)
  {
    if (tcl.list_commandwords.count() < 3) {myLine=__LINE__;goto command_warn;}
    if (tcl.scan.at(0)->entry_fn == NULL)
    {// only parsed outside functions
      tcl_command_VARIABLE(text,0);
      goto command_end;
    }
  }
  if (strcmp("inherit",myStr)==0 || strcmp("superclass",myStr)==0)
  {
    if (tcl.list_commandwords.count() < 3) {myLine=__LINE__;goto command_warn;}
    if (tcl.scan.at(0)->entry_cl!=NULL&&tcl.scan.at(0)->entry_cl->name!="")
    {
      for (unsigned int i = 2; i < tcl.list_commandwords.count(); i++)
      {
        tcl.scan.at(0)->entry_cl->extends->append(new BaseInfo(*tcl.list_commandwords.at(i),Public,Normal));
      }
    }
  }
  tcl_command_OTHER(text);
  goto command_end;
  command_warn:
    tcl_war("%d count=%d: %s\n",myLine,tcl.list_commandwords.count(),tcl.list_commandwords.join(" ").ascii());
    tcl_command_OTHER(text);
  command_end:
    tcl.list_commandwords.clear();
    tcl.command = 0;
    tcl.protection = myProt;
}

//----------------------------------------------------------------------------
//! Common initializations.
static void tcl_init()
{
  // Get values from option TCL_SUBST
  tcl.config_subst.clear();
  if (Config::instance()->get("TCL_SUBST")) 
  {
    QStrList myStrList = Config_getList("TCL_SUBST");
    const char *s=myStrList.first();
    while (s) 
    {
      QCString myStr=s;
      int i=myStr.find('=');
      if (i>0)
      {
        QCString myName=myStr.left(i).stripWhiteSpace();
        QCString myValue=myStr.right(myStr.length()-i-1).stripWhiteSpace();
        if (!myName.isEmpty() && !myValue.isEmpty())
tcl_inf("TCL_SUBST: use '%s'\n",s);
        tcl.config_subst[myName] = myValue;
      }
      s = myStrList.next();
    }
  }

  if (tcl.input_string.at(tcl.input_string.length()-1) == '\n') 
  {
    tcl.input_string[tcl.input_string.length()-1] = 0x1A;
  } 
  else 
  {
    tcl.input_string += 0x1A;
  }
  tcl.code = NULL;
  tcl.code_font=NULL;
  tcl.code_line=1;
  tcl.code_linenumbers=1;
  tcl.config_autobrief = Config_getBool("JAVADOC_AUTOBRIEF");
  tcl.input_position = 0;
  tcl.file_name = NULL;
  tcl.this_parser = NULL;
  tcl.command=0;
  tcl.comment=0;
  tcl.brace_level=0;
  tcl.bracket_level=0;
  tcl.bracket_quote=0;
  tcl.word_is=WORD;
  tcl.string_command="";
  tcl.string_commentline="";
  tcl.string_commentcodify="";
  tcl.string_comment	= "";
  tcl.string_last	= "";
  tcl.entry_main	= NULL;
  tcl.entry_file	= NULL;
  tcl.entry_current	= NULL;
  tcl.entry_inside	= NULL;
  tcl.list_commandwords.clear();
  tcl.scan.clear();
  tcl.ns.clear();
  tcl.cl.clear();
  tcl.fn.clear();
  yylineno		= 1;
  tcl.protection	= Public;
  tcl.memberdef		= NULL;
}

//! Start parsing.
static void tcl_parse(const QString ns, const QString cls)
{
  tcl_scan *myScan;

  tcl.entry_file          = tcl_entry_new();
  tcl.entry_file->name    = tcl.file_name;
  tcl.entry_file->section = Entry::SOURCE_SEC;
  tcl.entry_file->protection = Public;
  tcl.entry_main->addSubEntry(tcl.entry_file);
  tcl.entry_current = tcl_entry_new();

  tclscanYYrestart( tclscanYYin );
  BEGIN( TOP );
  yylineno=1;
  myScan = new tcl_scan;
  myScan->type=STRING;
  myScan->string_after="";
  myScan->line0=yylineno;
  myScan->line1=yylineno;
  myScan->buffer_state=YY_CURRENT_BUFFER;
  myScan->ns=ns;
  myScan->entry_cl=tcl_entry_class(cls);
  myScan->entry_fn=NULL;
  tcl.entry_inside = tcl.entry_file;
  myScan->entry_scan = tcl.entry_inside;
  tcl.scan.insert(0,myScan);

  tclscanYYlex();
  tcl.scan.clear();
  tcl.ns.clear();
  tcl.cl.clear();
  tcl.fn.clear();
  tcl.entry.clear();
}

//! Parse text file and build up entry tree.
void TclLanguageScanner::parseInput(const char *fileName,const char *input,Entry *root)
{
  QFile            myFile;
tcl_inf("%s\n",fileName);
  myFile.setName(fileName);
  if (!myFile.open(IO_ReadOnly)) return;
  if (strlen(input)<1) return;

  tcl.input_string = input;
  if (tcl.input_string.length()<1) return;

  msg("Parsing %s...\n",fileName);
  groupEnterFile(fileName,yylineno);

  tcl_init();
  tcl.code = NULL;
  tcl.file_name = fileName;
  tcl.this_parser = this;
  tcl.entry_main          = root; /* toplevel entry */
  tcl_parse("","");

  groupLeaveFile(tcl.file_name,yylineno);
  root->program.resize(0);
  myFile.close();
}

//! Parse file and codify.
void TclLanguageScanner::parseCode(CodeOutputInterface & codeOutIntf,
                   const char * scopeName,
                   const QCString & input,
                   bool isExampleBlock,
                   const char * exampleName,
                   FileDef * fileDef,
                   int startLine,
                   int endLine,
                   bool inlineFragment,
		   MemberDef *memberDef,
                   bool showLineNumbers
                  )
{
  (void)scopeName;
  (void)exampleName;
  (void)fileDef;
  (void)endLine;
  (void)inlineFragment;

  if (input.length()<1) return;
  tcl.input_string = input;

  QString myNs="";
  QString myCls="";
  if (memberDef)
  {
    if (memberDef->getClassDef())
    {
      myCls = memberDef->getClassDef()->displayName();
      myNs = myCls;
    }
    else if (memberDef->getNamespaceDef())
    {
      myNs = memberDef->getNamespaceDef()->displayName();
    }
  }

  QString myStr="Codifying..";
  if (scopeName)
  {
    myStr +=" scope=";
    myStr+=scopeName;
  }
  if (exampleName)
  {
    myStr+=" example=";
    myStr+=exampleName;
  }
  if (memberDef)
  {
    myStr+=" member=";
    myStr+=memberDef->memberTypeName();
    myStr+=" ";
    myStr+=memberDef->qualifiedName();
  }
  if (fileDef)
  {
    myStr+=" file=";
    myStr+=fileDef->fileName();
  }
tcl_inf("%s (%d,%d) %d %d\n",myStr.ascii(),startLine,endLine,isExampleBlock,inlineFragment);
//tcl_inf("%s\n"input.data());

  if (isExampleBlock)
  {
    tcl_codify(NULL,input);
    return;
  }
  tcl_init();
  tcl.memberdef = memberDef;
  tcl.code = &codeOutIntf;
  if (startLine<0) 
  {
    startLine=1;
  }
  yylineno=startLine;
  tcl.code_linenumbers = showLineNumbers;
  tcl.code_line=yylineno;
  if (tcl.code_linenumbers) 
  {
    tcl.code->writeLineNumber(0,0,0,tcl.code_line);
  }
  tcl.file_name = "";
  tcl.this_parser = NULL;
  tcl.entry_main = tcl_entry_new();
  tcl_parse(myNs,myCls);
  tcl.scan.clear();
  tcl.ns.clear();
  tcl.cl.clear();
  tcl.fn.clear();
  tcl.entry.clear();
}

bool TclLanguageScanner::needsPreprocessing(const QCString &extension)
{
  (void)extension;
  return FALSE;
}

void TclLanguageScanner::resetCodeParserState()
{
}

void TclLanguageScanner::parsePrototype(const char *text)
{
  (void)text;
}

static int yyread(char *buf,int max_size)
{
  int c=0;

  *buf = '\0';
  while ( c < max_size && tcl.input_string.at(tcl.input_position) )
  {
    *buf = tcl.input_string.at(tcl.input_position++) ;
    c++; buf++;
  }
  //printf("Read from=%d size=%d max=%d c=%d\n",tcl.input_position,strlen(&tcl.input_string[tcl.input_position]),max_size,c);
  return c;
}

//----------------------------------------------------------------------------

// to avoid a warning
void tclDummy()
{
  yy_top_state();
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void tclscannerYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

