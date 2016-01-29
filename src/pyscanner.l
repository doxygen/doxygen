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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada)
 *  in Spring 2005 as part of CS 179E: Compiler Design Project
 *  at the University of California, Riverside; the course was
 *  taught by Peter H. Froehlich <phf@acm.org>.
 */

%option never-interactive
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
#include <qfileinfo.h>
  
#include "pyscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "pycode.h"
#include "arguments.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

/* -----------------------------------------------------------------
 *
 *	statics
 */

  
static ParserInterface *g_thisParser;
static const char *     inputString;
static int		inputPosition;
static QFile            inputFile;

static Protection	protection;

static Entry*		current_root = 0 ;
static Entry*		current      = 0 ;
static Entry*		previous     = 0 ;
static Entry*		bodyEntry    = 0 ;
static int		yyLineNr     = 1 ;
static QCString		yyFileName;
static MethodTypes 	mtype;
static bool    		gstat;
static Specifier 	virt;

static int              docBlockContext;
static QCString         docBlock;
static bool             docBlockInBody;
static bool             docBlockJavaStyle;
static bool             docBrief;
static bool             docBlockSpecial;

static bool             g_doubleQuote;
static bool             g_specialBlock;
static int              g_stringContext;
static QGString *       g_copyString;
static int              g_indent = 0;
static int              g_curIndent = 0;

static QDict<QCString>  g_packageNameCache(257);

static char             g_atomStart;
static char             g_atomEnd;
static int              g_atomCount;

//static bool             g_insideConstructor;

static QCString         g_moduleScope;
static QCString         g_packageName;

//static bool             g_hideClassDocs;

static QCString         g_defVal;
static int              g_braceCount;

static bool             g_lexInit = FALSE;
static bool             g_packageCommentAllowed;

static bool             g_start_init = FALSE;
static int              g_search_count = 0;

//-----------------------------------------------------------------------------


static void initParser()
{
  protection = Public;
  mtype = Method;
  gstat = FALSE;
  virt = Normal;
  previous = 0;
  g_packageCommentAllowed = TRUE;
  g_packageNameCache.setAutoDelete(TRUE);
}

static void initEntry()
{
  //current->python = TRUE;
  current->protection = protection ;
  current->mtype      = mtype;
  current->virt       = virt;
  current->stat       = gstat;
  current->lang       = SrcLangExt_Python; 
  current->setParent(current_root);
  initGroupInfo(current);
  gstat = FALSE;
}

static void newEntry()
{
  previous = current;
  current_root->addSubEntry(current);
  current = new Entry ;
  initEntry();
}

static void newVariable()
{
  if (!current->name.isEmpty() && current->name.at(0)=='_') // mark as private
  {
    current->protection=Private;
  }
  if (current_root->section&Entry::COMPOUND_MASK) // mark as class variable
  {
    current->stat = TRUE;
  }
  newEntry();
}

static void newFunction()
{
  if (current->name.left(2)=="__" && current->name.right(2)=="__")
  {
    // special method name, see
    // http://docs.python.org/ref/specialnames.html
    current->protection=Public;
  }
  else if (current->name.at(0)=='_')
  {
    current->protection=Private;
  }
}

static inline int computeIndent(const char *s)
{
  int col=0;
  static int tabSize=Config_getInt(TAB_SIZE);
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    if (c==' ') col++;
    else if (c=='\t') col+=tabSize-(col%tabSize);
    else break;
  }
  return col;
}

static QCString findPackageScopeFromPath(const QCString &path)
{
  QCString *pScope = g_packageNameCache.find(path);
  if (pScope)
  {
    return *pScope;
  }
  QFileInfo pf(path+"/__init__.py"); // found package initialization file
  if (pf.exists())
  {
    int i=path.findRev('/');
    if (i!=-1)
    {
      QCString scope = findPackageScopeFromPath(path.left(i));
      if (!scope.isEmpty())
      {
	scope+="::";
      }
      scope+=path.mid(i+1);
      g_packageNameCache.insert(path,new QCString(scope));
      return scope;
    }
  }
  return "";
}
  
static QCString findPackageScope(const char *fileName)
{
  if (fileName==0) return "";
  QFileInfo fi(fileName);
  return findPackageScopeFromPath(fi.dirPath(TRUE).data());
}

//-----------------------------------------------------------------------------

static void lineCount()
{
  DBG_CTX((stderr,"yyLineNr=%d\n",yyLineNr));
  for (const char *p = yytext; *p; ++p)
  {
    yyLineNr += (*p == '\n') ;
  }
}

static void incLineNr()
{
  DBG_CTX((stderr,"yyLineNr=%d\n",yyLineNr));
  yyLineNr++;
}

#if 0
// Appends the current-name to current-type;
// Destroys current-name.
// Destroys current->args and current->argList
static void addType( Entry* current )
{
    uint tl=current->type.length();
    if ( tl>0 && !current->name.isEmpty() && current->type.at(tl-1)!='.') 
    {
      current->type += ' ' ;
    }
    current->type += current->name ;
    current->name.resize(0) ;
    tl=current->type.length();
    if ( tl>0 && !current->args.isEmpty() && current->type.at(tl-1)!='.') 
    {
      current->type += ' ' ;
    }
    current->type += current->args ;
    current->args.resize(0) ;
    current->argList->clear();
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
#endif
//-----------------------------------------------------------------

//-----------------------------------------------------------------
static void startCommentBlock(bool brief)
{
  if (brief)
  {
    current->briefFile = yyFileName;
    current->briefLine = yyLineNr;
  }
  else
  {
    current->docFile = yyFileName;
    current->docLine = yyLineNr;
  }
}

/*
static void appendDocBlock() {
  previous = current;
  current_root->addSubEntry(current);
  current = new Entry;
  initEntry();
}
*/

static void handleCommentBlock(const QCString &doc,bool brief)
{
  //printf("handleCommentBlock(doc=[%s] brief=%d docBlockInBody=%d docBlockJavaStyle=%d\n",
  //    doc.data(),brief,docBlockInBody,docBlockJavaStyle);

  // TODO: Fix me
  docBlockInBody=FALSE;
  
  if (docBlockInBody && previous && !previous->doc.isEmpty())
  {
    previous->doc=previous->doc.stripWhiteSpace()+"\n\n";
  }

  int position = 0;
  bool needsEntry;
  int lineNr = brief ? current->briefLine : current->docLine;
  while (parseCommentBlock(
	g_thisParser,
	(docBlockInBody && previous) ? previous : current,
	doc,     // text
	yyFileName, // file
	lineNr,
	docBlockInBody ? FALSE : brief, 
    docBlockJavaStyle, // javadoc style // or FALSE,
	docBlockInBody,
	protection,
        position,
        needsEntry)
     ) // need to start a new entry
  {
    if (needsEntry)
    {
      newEntry();
    }
  }
  if (needsEntry)
  {
    newEntry();
  }

}

static void endOfDef(int correction=0)
{
  //printf("endOfDef at=%d\n",yyLineNr);
  if (bodyEntry)
  {
    bodyEntry->endBodyLine  = yyLineNr-correction;
    bodyEntry = 0;
  }
  newEntry();
  //g_insideConstructor = FALSE;
}

static inline void addToString(const char *s)
{
  if (g_copyString) (*g_copyString)+=s;
}

static void initTriDoubleQuoteBlock()
{
  docBlockContext   = YY_START;
  docBlockInBody    = FALSE;
  docBlockJavaStyle = TRUE;
  docBlockSpecial   = yytext[strlen(yytext) - 1]=='!';
  docBlock.resize(0);
  g_doubleQuote = TRUE;
  startCommentBlock(FALSE);
}

static void initTriSingleQuoteBlock()
{
  docBlockContext   = YY_START;
  docBlockInBody    = FALSE;
  docBlockJavaStyle = TRUE;
  docBlockSpecial   = yytext[strlen(yytext) - 1]=='!';
  docBlock.resize(0);
  g_doubleQuote = FALSE;
  startCommentBlock(FALSE);
}

static void initSpecialBlock()
{
  docBlockContext   = YY_START;
  docBlockInBody    = FALSE;
  docBlockJavaStyle = TRUE;
  docBrief = TRUE;
  docBlock.resize(0);
  startCommentBlock(TRUE);
}

static void searchFoundDef()
{
  current->fileName  = yyFileName;
  current->startLine = yyLineNr;
  current->bodyLine  = yyLineNr;
  current->section = Entry::FUNCTION_SEC;
  current->lang = SrcLangExt_Python;
  current->virt = Normal;
  current->stat = gstat;
  current->mtype = mtype = Method;
  current->type.resize(0);
  current->name.resize(0);
  current->args.resize(0);
  current->argList->clear();
  g_packageCommentAllowed = FALSE;
  gstat=FALSE;
  //printf("searchFoundDef at=%d\n",yyLineNr);
}

static void searchFoundClass()
{
  current->section = Entry::CLASS_SEC;
  current->argList->clear();
  current->type += "class" ;
  current->fileName  = yyFileName;
  current->startLine  = yyLineNr;
  current->bodyLine  = yyLineNr;
  g_packageCommentAllowed = FALSE;
}

//-----------------------------------------------------------------------------
/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  while ( c < max_size && inputString[inputPosition] )
  {
    *buf = inputString[inputPosition++] ;
    //printf("%d (%c)\n",*buf,*buf);
    c++; buf++;
  }
  return c;
}

%}

       /* start command character */



BB                [ \t]+
B                 [ \t]*
NEWLINE           \n
BN                [ \t\n]

DIGIT             [0-9]

HEXNUMBER         "0"[xX][0-9a-fA-F]+[lL]?
OCTNUMBER         "0"[0-7]+[lL]?
NUMBER            {DIGIT}+[lLjJ]?
INTNUMBER         {HEXNUMBER}|{OCTNUMBER}|{NUMBER}
FLOATNUMBER       {DIGIT}+"."{DIGIT}+([eE][+\-]?{DIGIT}+)?[jJ]?
BOOL              ("True"|"False")
LETTER            [A-Za-z\x80-\xFF]
NONEMPTY          [A-Za-z0-9_\x80-\xFF]
EXPCHAR           [#(){}\[\],:.%/\\=`*~|&<>!;+-]
NONEMPTYEXP       [^ \t\n:]
PARAMNONEMPTY     [^ \t\n():]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*  
SCOPE             {IDENTIFIER}("."{IDENTIFIER})*
BORDER            ([^A-Za-z0-9])

TRISINGLEQUOTE    {STRINGPREFIX}?"'''"(!)?
TRIDOUBLEQUOTE    {STRINGPREFIX}?"\"\"\""(!)?
ENDTRISINGLEQUOTE "'''"
ENDTRIDOUBLEQUOTE "\"\"\""
LONGSTRINGCHAR    [^\\"']
ESCAPESEQ         ("\\")(.)
LONGSTRINGITEM    ({LONGSTRINGCHAR}|{ESCAPESEQ})
SMALLQUOTE        ("\"\""|"\""|"'"|"''")
LONGSTRINGBLOCK   ({LONGSTRINGITEM}+|{SMALLQUOTE})

SHORTSTRING       ("'"{SHORTSTRINGITEM}*"'"|'"'{SHORTSTRINGITEM}*'"')
SHORTSTRINGITEM   ({SHORTSTRINGCHAR}|{ESCAPESEQ})
SHORTSTRINGCHAR   [^\\\n"]
STRINGLITERAL     {STRINGPREFIX}?( {SHORTSTRING} | {LONGSTRING})  
STRINGPREFIX      ("r"|"u"|"ur"|"R"|"U"|"UR"|"Ur"|"uR")
KEYWORD           ("lambda"|"import"|"class"|"assert"|"as"|"from"|"global"|"def"|"True"|"False")
FLOWKW            ("or"|"and"|"is"|"not"|"print"|"for"|"in"|"if"|"try"|"except"|"yield"|"raise"|"break"|"continue"|"pass"|"if"|"return"|"while"|"elif"|"else"|"finally")
POUNDCOMMENT      "#"[^#\n][^\n]* 
SCRIPTCOMMENT      "#!".* 

STARTDOCSYMS      "##"

%option noyywrap

  /* Main start state */

%x Search
%x SearchMemVars

  /* Mid-comment states */

  /* %x FuncDoubleComment */
  /* %x ClassDoubleComment */
%x TryClassDocString
%x TripleComment
%x SpecialComment

  /* Function states */

%x FunctionDec
%x FunctionParams
%x FunctionBody
%x FunctionParamDefVal

  /* Class states */

%x ClassDec
%x ClassInheritance
%x ClassCaptureIndent
%x ClassBody

  /* Variable states */
%x VariableDec
%x VariableEnd
%x VariableAtom

  /* String states */

%x SingleQuoteString
%x DoubleQuoteString
%x TripleString

  /* import */
%x FromMod
%x FromModItem
%x Import

%%

  /* ------------ Function recognition rules -------------- */

<Search>{

    ^{B}"def"{BB}       { // start of a function/method definition with indent
      			  DBG_CTX((stderr,"Found def at %d\n",yyLineNr));
      			  g_indent=computeIndent(yytext);
			  searchFoundDef();
			  BEGIN( FunctionDec );
      			}
    "def"{BB}           { // start of a function/method definition
			  searchFoundDef();
			  BEGIN( FunctionDec );
                        }

     ^{B}"class"{BB}	{ // start of a class definition with indent
       			  DBG_CTX((stderr,"Found class at %d\n",yyLineNr));
      			  g_indent=computeIndent(yytext);
			  searchFoundClass();
			  BEGIN( ClassDec ) ;
       			}
     "class"{BB}        {  // start of a class definition
			  searchFoundClass();
			  BEGIN( ClassDec ) ;
                       }
     ^{B}"from"{BB}    |
     "from"{BB}	       { // start of an from import
			  g_packageCommentAllowed = FALSE;
                          BEGIN( FromMod );
                       }

     ^{B}"import"{BB}  |
     "import"{BB}      { // start of an import statement
			  g_packageCommentAllowed = FALSE;
                          BEGIN( Import );
                       }
     ^{B}{IDENTIFIER}/{B}"="{B}"property" { // property
      			current->section   = Entry::VARIABLE_SEC;
  			current->mtype     = Property;
			current->name      = QCString(yytext).stripWhiteSpace();
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			g_packageCommentAllowed = FALSE;
			BEGIN(VariableDec);
       	              }
     ^{B}{IDENTIFIER}/{B}"="[^=] { // variable
                        if (g_search_count) REJECT;
      			g_indent=computeIndent(yytext);
      			current->section   = Entry::VARIABLE_SEC;
			current->name      = QCString(yytext).stripWhiteSpace();
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			g_packageCommentAllowed = FALSE;
			BEGIN(VariableDec);
      		      }
     {B}{IDENTIFIER}/({B},{B}{IDENTIFIER})*{B}")"*{B}"="[^=] { // list of variables, we cannot place the default value
                                                               // so we will skip it later on in a general rule
                                                               // Also note ")" this is to catch also (a,b). the "("
                                                               // is caught in the rule: [(], the ")" will be handled in [)]
                        if (g_search_count > 1) REJECT;
      			g_indent=computeIndent(yytext);
      			current->section   = Entry::VARIABLE_SEC;
			current->name      = QCString(yytext).stripWhiteSpace();
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			g_packageCommentAllowed = FALSE;
			newVariable();
      		      }
     "'"	      { // start of a single quoted string
       		        g_stringContext=YY_START;
		        g_copyString=0;
			g_packageCommentAllowed = FALSE;
                        BEGIN( SingleQuoteString );
                      }
     "\""	      { // start of a double quoted string
       	                g_stringContext=YY_START;
			g_copyString=0;
			g_packageCommentAllowed = FALSE;
                        BEGIN( DoubleQuoteString );
                      }
    "@staticmethod"  {
       			gstat=TRUE;
      		      }
    {SCRIPTCOMMENT}   { // Unix type script comment
                        if (yyLineNr != 1) REJECT;
      		      }
    {POUNDCOMMENT}    { // normal comment 
			g_packageCommentAllowed = FALSE;
      		      }
    {IDENTIFIER}      { // some other identifier
			g_packageCommentAllowed = FALSE;
		      }
    ^{BB}	      {
      			g_curIndent=computeIndent(yytext);
                      }

    {NEWLINE}+        { // new line
                        lineCount();
                      }

    {TRIDOUBLEQUOTE}  { // start of a comment block
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}/[^#]    {  // start of a special comment
      			g_curIndent=computeIndent(yytext);
			g_packageCommentAllowed = FALSE;
			initSpecialBlock();
			BEGIN(SpecialComment);
                      }
    [(]               { // we have to do something with (
                        g_search_count += 1;
                      }
    [)]               { // we have to do something with )
                        g_search_count -= 1;
                      }
    [^\n]             { // any other character...
                        // This is the major default
                        // that should catch everything
                        // else in Body.
                      }
}

<FromMod>{
  "."                 { // python3 style imports
                      }
  {IDENTIFIER}({B}"."{B}{IDENTIFIER})* { // from package import 
                        g_packageName=yytext;
		      }
  "import"{B}	      {
    			BEGIN(FromModItem);
    		      }
  \n		      {
                        incLineNr();
                        BEGIN(Search);
                      }
  {B}		      {
		      }
  .                   {
                        unput(*yytext);
                        BEGIN(Search);
                      }
}

<FromModItem>{
  "*"		{ // import all
                  QCString item=g_packageName;
		  current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
		  current->fileName = yyFileName; 
		  //printf("Adding using directive: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
		  current->section=Entry::USINGDIR_SEC;
		  current_root->addSubEntry(current);
		  current = new Entry ;
		  initEntry();
                  BEGIN(Search);
    		}
  {IDENTIFIER}/{B}","{B} {
                  QCString item=g_packageName+"."+yytext;
		  current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
		  current->fileName = yyFileName; 
		  //printf("Adding using declaration: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
		  current->section=Entry::USINGDECL_SEC;
		  current_root->addSubEntry(current);
		  current = new Entry ;
		  initEntry();
		}
  {IDENTIFIER}  {
                  QCString item=g_packageName+"."+yytext;
		  current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
		  current->fileName = yyFileName; 
		  //printf("Adding using declaration: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
		  current->section=Entry::USINGDECL_SEC;
		  current_root->addSubEntry(current);
		  current = new Entry ;
		  initEntry();
                  BEGIN(Search);
		}
  \n		{
                  incLineNr();
                  BEGIN(Search);
                }
  {B}		{
		}
  ","		{ 
  		}
  .             {
                  unput(*yytext);
                  BEGIN(Search);
                }
}

<Import>{
  {IDENTIFIER}({B}"."{B}{IDENTIFIER})* {
			current->name=removeRedundantWhiteSpace(substitute(yytext,".","::"));
			current->fileName = yyFileName; 
			//printf("Adding using declaration: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
			current->section=Entry::USINGDECL_SEC;
			current_root->addSubEntry(current);
			current = new Entry ;
			initEntry();
			BEGIN(Search);
                      }
  \n		{
                  incLineNr();
                  BEGIN(Search);
                }
  {B}		{
		}
  .             {
                  unput(*yytext);
                  BEGIN(Search);
                }
}

<SearchMemVars>{
    "self."{IDENTIFIER}/{B}"=" {
                        DBG_CTX((stderr,"Found instance method variable %s in %s at %d\n",&yytext[5],current_root->name.data(),yyLineNr));
			current->name=&yytext[5];
			current->section=Entry::VARIABLE_SEC;
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			current->type.resize(0);
			if (current->name.at(0)=='_') // mark as private
			{
			  current->protection=Private;
			}
			newEntry();
                      }
    "cls."{IDENTIFIER}/{B}"=" {
                        DBG_CTX((stderr,"Found class method variable %s in %s at %d\n",&yytext[4],current_root->name.data(),yyLineNr));
			current->name=&yytext[4];
			current->section=Entry::VARIABLE_SEC;
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			current->type.resize(0);
			if (current->name.at(0)=='_') // mark as private
			{
			  current->protection=Private;
			}
			newEntry();
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}/[^#]    {  // start of a special comment
			initSpecialBlock();
			BEGIN(SpecialComment);
                      }
    {POUNDCOMMENT}    { // #
	              }
    "'"	              { // start of a single quoted string
       			g_stringContext=YY_START;
			g_copyString=0;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
       			g_stringContext=YY_START;
			g_copyString=0;
                        BEGIN( DoubleQuoteString );
                      }
    \n		      { incLineNr(); }
    {IDENTIFIER}      // identifiers
    [^'"\.#a-z_A-Z\n]+  // other uninteresting stuff
    .                 // anything else
}

<FunctionBody>{
    \n{B}/{IDENTIFIER}{BB}  {
                        DBG_CTX((stderr,"indent %d<=%d\n",computeIndent(&yytext[1]),g_indent));
                        if (computeIndent(&yytext[1])<=g_indent) 
			{
			  int i;
			  for (i=(int)yyleng-1;i>=0;i--)
			  {
			    unput(yytext[i]);
			  }
			  endOfDef();
			  //YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                          BEGIN(Search);
			}
			else
			{
                          incLineNr();
      		          current->program+=yytext;
			}
                      }
    \n{B}/"##"	      {
                        if (computeIndent(&yytext[1])<=g_indent)
			{
			  int i;
			  for (i=(int)yyleng-1;i>=0;i--)
			  {
			    unput(yytext[i]);
			  }
			  endOfDef();
			  //YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                          BEGIN(Search);
			}
			else
			{
			  incLineNr();
      		          current->program+=yytext;
			}
      		      }
    <<EOF>>	      {
			endOfDef();
			yyterminate();
      		      }
    ^{BB}/\n	      { // skip empty line
      		        current->program+=yytext;
      	              }
    ^{BB}	      { // something at indent >0
      		        current->program+=yytext;
			g_curIndent = computeIndent(yytext);
                        if (g_curIndent<=g_indent) 
			  // jumped out of the function
			{
			  endOfDef(1);
                          BEGIN(Search);
			}
      		      }
    "'"	              { // start of a single quoted string
      		        current->program+=yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
      		        current->program+=yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( DoubleQuoteString );
                      }
    [^ \t\n#'".]+     { // non-special stuff
      		        current->program+=yytext;
		        g_specialBlock = FALSE; 
                      }
    ^{POUNDCOMMENT}   { // normal comment 
      		        current->program+=yytext;
      		      }
    "#".*             { // comment half way
      		        current->program+=yytext;
                      }
    {NEWLINE}	      { 
			incLineNr(); 
      		        current->program+=yytext;
		      }
    .                 { // any character
      		        current->program+=*yytext;
		        g_specialBlock = FALSE; 
                      }

    {TRIDOUBLEQUOTE}  { // start of a comment block
			current->program+=yytext;
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			current->program+=yytext;
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}/[^#]  {  // start of a special comment
			initSpecialBlock();
			BEGIN(SpecialComment);
                      }
    
}

<FunctionDec>{

    {IDENTIFIER}            {
			      //found function name
			      if (current->type.isEmpty()) 
			      {
				  current->type = "def";
			      }
			      current->name = yytext;
			      current->name = current->name.stripWhiteSpace();
			      newFunction();
                            }
    {B}":"		    { // function without arguments
			      g_specialBlock = TRUE; // expecting a docstring
			      bodyEntry = current;
                              current->bodyLine  = yyLineNr;
                              BEGIN( FunctionBody );
			    }

    {B}"("                  {
			       BEGIN( FunctionParams );
		            }
}

<FunctionParams>{
    ({BB}|",")          {
                        }

    {IDENTIFIER}        { // Name of parameter
			  lineCount();
			  Argument *a = new Argument;
			  current->argList->append(a);
			  current->argList->getLast()->name = QCString(yytext).stripWhiteSpace();
			  current->argList->getLast()->type = "";
                        }
    "="		        { // default value
                          // TODO: this rule is too simple, need to be able to
                          // match things like =")" as well!
			  g_defVal.resize(0);
			  g_braceCount=0;
			  BEGIN(FunctionParamDefVal);
      			}

     ")"                { // end of parameter list
        		  current->args = argListToString(current->argList);
                        }

     ":"{B}             {
			  g_specialBlock = TRUE; // expecting a docstring
			  bodyEntry = current;
                          current->bodyLine  = yyLineNr;
                          BEGIN( FunctionBody );
                        }
    {POUNDCOMMENT}	{ // a comment
			}
    {PARAMNONEMPTY}     { // Default rule inside arguments.
                        }

}

<FunctionParamDefVal>{
     "("		{ // internal opening brace
       			  g_braceCount++;
			  g_defVal+=*yytext;
       			}
     ","		| 
     ")"		{
       			  if (g_braceCount==0)  // end of default argument
			  {
			    if (current->argList->getLast())
			    {
			      current->argList->getLast()->defval=g_defVal.stripWhiteSpace();
			    }
			    if (*yytext == ')')
			      current->args = argListToString(current->argList);
			    BEGIN(FunctionParams);
			  }
			  else // continue
			  {
			    if (*yytext == ')')g_braceCount--;
			    g_defVal+=*yytext;
			  }
       			}
     .			{
			    g_defVal+=*yytext;
       			}
     \n                 {
       			    g_defVal+=*yytext;
			    incLineNr();
       			}
}


<ClassBody>{
    \n/{IDENTIFIER}{BB}  { // new def at indent 0
                        incLineNr();
			endOfDef();
			//g_hideClassDocs = FALSE;
			//YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                        BEGIN(Search);
                      }
    \n/"##"[^#]       {  // start of a special comment at indent 0
                        incLineNr();
			endOfDef();
			//g_hideClassDocs = FALSE;
			//YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                        BEGIN(Search);
                      }
    ^{BB}/\n	      { // skip empty line
      		        current->program+=yytext;
      	              }
    <<EOF>>	      {
			endOfDef();
			yyterminate();
      		      }
    ^{BB}	      { // something at indent >0
                        g_curIndent=computeIndent(yytext);
			DBG_CTX((stderr,"g_curIndent=%d g_indent=%d\n",g_curIndent,g_indent));
                        if (g_curIndent<=g_indent) 
			  // jumped out of the class/method
			{
			  endOfDef(1);
			  g_indent=g_curIndent;
			  // make sure the next rule matches ^...
			  //YY_CURRENT_BUFFER->yy_at_bol=TRUE;
			  //g_hideClassDocs = FALSE;
                          BEGIN(Search);
			}
			else
			{
      		          current->program+=yytext;
			}
      		      }
    "'"	              { // start of a single quoted string
      		        current->program+=*yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
      		        current->program+=*yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( DoubleQuoteString );
                      }
    [^ \t\n#'"]+      { // non-special stuff
      		        current->program+=yytext;
		        g_specialBlock = FALSE; 
			//g_hideClassDocs = FALSE;
                      }
    {NEWLINE}	      { 
      		        current->program+=*yytext;
			incLineNr(); 
		      }
    {POUNDCOMMENT}    { // normal comment
      		        current->program+=yytext;
                      }
    .                 { // any character
		        g_specialBlock = FALSE; 
      		        current->program+=*yytext;
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
      		        //if (!g_hideClassDocs) 
			current->program+=yytext;
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
      		        //if (!g_hideClassDocs) 
			current->program+=yytext;
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }
}

<ClassDec>{IDENTIFIER} {
			  if (current->type.isEmpty()) 
			  {
			      current->type = "class";
			  }

			  current->section = Entry::CLASS_SEC;
			  current->name = yytext;

			  // prepend scope in case of nested classes
			  if (current_root->section&Entry::SCOPE_MASK)
			  {
			    //printf("*** Prepending scope %s to class %s\n",current_root->name.data(),current->name.data());
			    current->name.prepend(current_root->name+"::");
			  }
			  
			  current->name = current->name.stripWhiteSpace();
			  current->fileName = yyFileName;        
			  docBlockContext   = YY_START;
			  docBlockInBody    = FALSE;
			  docBlockJavaStyle = FALSE;
			  docBlock.resize(0);

			  BEGIN(ClassInheritance);                 
                        }

<ClassInheritance>{
   ({BB}|[\(,\)])      { // syntactic sugar for the list
                       }

    ":"                { // begin of the class definition
			 g_specialBlock = TRUE; // expecting a docstring
                         current->bodyLine  = yyLineNr;
			 current->program.resize(0);
      			 BEGIN(ClassCaptureIndent);
                       }

    {SCOPE}            {
                         current->extends->append(
					      new BaseInfo(substitute(yytext,".","::"),Public,Normal)
					    );
                         //Has base class-do stuff
                       }
}


<ClassCaptureIndent>{
    "\n"|({BB}"\n")            {
                                 // Blankline - ignore, keep looking for indentation.
				 lineCount();
      				 current->program+=yytext;
                               }

    {TRIDOUBLEQUOTE}           { // start of a comment block
				 initTriDoubleQuoteBlock();
      				 current->program+=yytext;
			 	 BEGIN(TripleComment);
                      	       }

    {TRISINGLEQUOTE}           { // start of a comment block
			         initTriSingleQuoteBlock();
      				 current->program+=yytext;
			         BEGIN(TripleComment);
                               }

    ^{BB} 		       {
      				 current->program+=yytext;
			         //current->startLine = yyLineNr;
				 g_curIndent=computeIndent(yytext);
				 bodyEntry = current;
				 DBG_CTX((stderr,"setting indent %d\n",g_curIndent));
				 //printf("current->program=[%s]\n",current->program.data());
				 //g_hideClassDocs = TRUE;
			         BEGIN(ClassBody);
                               }

    ""/({NONEMPTY}|{EXPCHAR})  {
				 
				 // Just pushback an empty class, and
				 // resume parsing the body.
                                 newEntry();
      				 current->program+=yytext;

				 // printf("Failed to find indent - skipping!");
				 BEGIN( Search );
                               }
}


<VariableDec>{
   "="                { // the assignment operator
                        //printf("====== VariableDec at line %d\n",yyLineNr);
                        g_start_init = TRUE;
			current->initializer = yytext;
			current->initializer += " ";
                      }
   {B}                { // spaces
	              }
   {INTNUMBER}	      { // integer value
			current->type = "int";
			current->initializer += yytext;
			BEGIN(VariableEnd);
		      }
   {FLOATNUMBER}      { // floating point value
			current->type = "float";
			current->initializer += yytext;
			BEGIN(VariableEnd);
		      }
   {BOOL}             { // boolean value
			current->type = "bool";
			current->initializer += yytext;
			BEGIN(VariableEnd);
		      }
   {STRINGPREFIX}?"'" { // string
			current->type = "string";
			current->initializer += yytext;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
                        BEGIN( SingleQuoteString );
     		      }
   {STRINGPREFIX}?"\"" { // string
			current->type = "string";
			current->initializer += yytext;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
                        BEGIN( DoubleQuoteString );
     		      }
   {TRIDOUBLEQUOTE}   { // start of a comment block
			current->type = "string";
			current->initializer += yytext;
			g_doubleQuote=TRUE;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
			BEGIN(TripleString);
                      }

   {TRISINGLEQUOTE}   { // start of a comment block
			current->type = "string";
			current->initializer += yytext;
			g_doubleQuote=FALSE;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
			BEGIN(TripleString);
                      }
   "("		      { // tuple, only when direct after =
                        if (current->mtype!=Property && g_start_init)
			{
			  current->type = "tuple";
			}
      		        current->initializer+=*yytext;
                        g_atomStart='(';
			g_atomEnd=')';
			g_atomCount=1;
			BEGIN( VariableAtom );
                      }
   "["                { // list
                        if (g_start_init) current->type = "list";
      		        current->initializer+=*yytext;
                        g_atomStart='[';
			g_atomEnd=']';
			g_atomCount=1;
			BEGIN( VariableAtom );
                      }
   "{"		      { // dictionary
                        if (g_start_init) current->type = "dictionary";
      		        current->initializer+=*yytext;
                        g_atomStart='{';
			g_atomEnd='}';
			g_atomCount=1;
			BEGIN( VariableAtom );
                      }
   "#".*              { // comment
                        BEGIN( VariableEnd ); 
                      }
   {IDENTIFIER}	      {
                        g_start_init = FALSE;
      		        current->initializer+=yytext;
	              }
   .		      {
                        g_start_init = FALSE;
      		        current->initializer+=*yytext;
                      }
   \n		      {
     		        unput('\n');
     			BEGIN( VariableEnd );
     		      }
}

<VariableAtom>{
    [\(\[\{]	      {
      		        current->initializer+=*yytext;
                        if (g_atomStart==*yytext)
			{
			  g_atomCount++;
			}
                      }
    [\)\]\}]	      {
      		        current->initializer+=*yytext;
                        if (g_atomEnd==*yytext)
			{
			  g_atomCount--;
			}
			if (g_atomCount==0)
			{
                          g_start_init = FALSE;
			  BEGIN(VariableDec);
			}
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
                        g_specialBlock = FALSE;
			current->program+=yytext;
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
                        g_specialBlock = FALSE;
			current->program+=yytext;
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }
   "'"	              {
       			g_stringContext=YY_START;
			current->initializer+="'";
			g_copyString=&current->initializer;
                        BEGIN( SingleQuoteString );
                      }
   "\""	              {
       			g_stringContext=YY_START;
			current->initializer+="\"";
			g_copyString=&current->initializer;
                        BEGIN( DoubleQuoteString );
                      }
   {IDENTIFIER}       {
      		        current->initializer+=yytext;
	              }
   .		      {
      		        current->initializer+=*yytext;
                      }
   \n		      {
      		        current->initializer+=*yytext;
     			incLineNr();
     		      }

}

<VariableEnd>{
    \n                {
                        incLineNr();
			newVariable();
                        BEGIN(Search);
                      }
    .		      { 
                        unput(*yytext);
			newVariable();
                        BEGIN(Search);
      		      }
    <<EOF>>           { yyterminate();
                        newEntry();
                      }
}

<TripleComment>{
    {ENDTRIDOUBLEQUOTE}   | 
    {ENDTRISINGLEQUOTE}   {
			  // printf("Expected module block %d special=%d\n",g_expectModuleDocs,g_specialBlock);
			  if (g_doubleQuote==(yytext[0]=='"')) 
			  {
			    if (g_specialBlock) // expecting a docstring
			    {
			      QCString actualDoc=docBlock;
			      if (!docBlockSpecial) // legacy unformatted docstring
			      {
			        actualDoc.prepend("\\verbatim ");
			        actualDoc.append("\\endverbatim ");
			      }
			      //printf("-------> current=%p bodyEntry=%p\n",current,bodyEntry);
			      handleCommentBlock(actualDoc, FALSE);
			    }
			    else if (g_packageCommentAllowed) // expecting module docs
			    {
			      QCString actualDoc=docBlock;
			      if (!docBlockSpecial) // legacy unformatted docstring
			      {
			        actualDoc.prepend("\\verbatim ");
			        actualDoc.append("\\endverbatim ");
			      }
			      actualDoc.prepend("\\namespace "+g_moduleScope+"\\_linebr ");
			      handleCommentBlock(actualDoc, FALSE);
			    }
			    if ((docBlockContext==ClassBody /*&& !g_hideClassDocs*/) ||
				docBlockContext==FunctionBody)
			    {
			      current->program+=docBlock;
			      current->program+=yytext;
			    }
                            //if (g_hideClassDocs)
			    //{
                            //  current->startLine = yyLineNr;
			    //}
			    //g_hideClassDocs=FALSE;
			    BEGIN(docBlockContext);
			  }
                          else 
			  {
                            docBlock += yytext;
                          }
			  g_packageCommentAllowed = FALSE;
                        }


    ^{BB}		{ // leading whitespace
      			  int indent = computeIndent(yytext);
			  if (indent>=g_curIndent)
			  { // strip g_curIndent amount of whitespace
			    int i;
			    for (i=0;i<indent-g_curIndent;i++) docBlock+=' ';
			    DBG_CTX((stderr,"stripping indent %d\n",g_curIndent));
			  }
			  else
			  {
			    DBG_CTX((stderr,"not stripping: %d<%d\n",indent,g_curIndent));
			    docBlock += yytext;
			  }
      			}
    [^"'\n \t\\]+       {
			  docBlock += yytext;
                        }
    \n			{
      			  incLineNr();
			  docBlock += yytext;
      			}
    \\.		        { // espaced char
			  docBlock += yytext;
      			}
    .			{
			  docBlock += yytext;
      			}
}

<SpecialComment>{
    ^{B}"#"("#")*	{ // skip leading hashes
      			}
    \n/{B}"#"		{ // continuation of the comment on the next line
      			  docBlock+='\n';
			  docBrief = FALSE;
                          startCommentBlock(FALSE);
			  incLineNr();
      			}
    [^#\n]+             { // any other stuff
      			  docBlock+=yytext;
      			}
    \n			{ // new line that ends the comment
			  handleCommentBlock(docBlock, docBrief);
      			  incLineNr();
			  BEGIN(docBlockContext);
      			}
    .			{ // anything we missed
      			  docBlock+=*yytext;
      			}
}

<SingleQuoteString>{
    \\{B}\n                    { // line continuation
      			         addToString(yytext);
      				 incLineNr();
                               }
    \\.			       { // espaced char
      			         addToString(yytext);
                               }
    "\"\"\""		       { // tripple double quotes
      			         addToString(yytext);
      			       }
    "'"			       { // end of the string
      			         addToString(yytext);
      		                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
      			         addToString(yytext);
                               }
    .			       { // normal char
      			         addToString(yytext);
                               }
}

<DoubleQuoteString>{
    \\{B}\n                    { // line continuation
      			         addToString(yytext);
      				 incLineNr();
                               }
    \\.			       { // espaced char
      			         addToString(yytext);
                               }
    "'''"		       { // tripple single quotes
      			         addToString(yytext);
      			       }
    "\""		       { // end of the string
      			         addToString(yytext);
      		                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
      			         addToString(yytext);
                               }
    .			       { // normal char
      			         addToString(yytext);
                               }
}

<TripleString>{
    {ENDTRIDOUBLEQUOTE}    | 
    {ENDTRISINGLEQUOTE}    {
                          *g_copyString += yytext;
			  if (g_doubleQuote==(yytext[0]=='"')) 
			  {
			    BEGIN(g_stringContext);
			  }
                        }


    ({LONGSTRINGBLOCK}) {
			  lineCount();
                          *g_copyString += yytext;
                        }
    \n			{
      			  incLineNr();
                          *g_copyString += yytext;
      			}
    .			{
                          *g_copyString += *yytext;
      			}
}

  /* ------------ End rules -------------- */

  /*
<*>({NONEMPTY}|{EXPCHAR}|{BB})           { // This should go one character at a time.
				 // printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyLineNr);

                               }
  */

<*>{NEWLINE}                   {
				 //printf("[pyscanner] %d NEWLINE [line %d] no match\n",
				 //       YY_START, yyLineNr);

                                 lineCount();
                               }

<*>.                           {
				 //printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyLineNr);

                               }


%%

//----------------------------------------------------------------------------

static void parseCompounds(Entry *rt)
{
  //printf("parseCompounds(%s)\n",rt->name.data());
  EntryListIterator eli(*rt->children());
  Entry *ce;
  for (;(ce=eli.current());++eli)
  {
    if (!ce->program.isEmpty())
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  ce->name.data(),ce->program.data());
      // init scanner state
      inputString = ce->program;
      inputPosition = 0;
      pyscannerYYrestart( pyscannerYYin ) ;
      if (ce->section&Entry::COMPOUND_MASK)
      {
        current_root = ce ;
        BEGIN( Search );
      }
      else if (ce->parent())
      {
        current_root = ce->parent();
	//printf("Searching for member variables in %s parent=%s\n",
	//    ce->name.data(),ce->parent->name.data());
	BEGIN( SearchMemVars );
      }
      yyFileName = ce->fileName;
      yyLineNr   = ce->bodyLine ;
      if (current) delete current;
      current = new Entry;
      initEntry();

      groupEnterCompound(yyFileName,yyLineNr,ce->name);
      
      pyscannerYYlex() ;
      g_lexInit=TRUE;
      delete current; current=0;
      ce->program.resize(0);

      groupLeaveCompound(yyFileName,yyLineNr,ce->name);

    }
    parseCompounds(ce);
  }
}

//----------------------------------------------------------------------------


static void parseMain(const char *fileName,const char *fileBuf,Entry *rt)
{
  initParser();

  inputString = fileBuf;
  inputPosition = 0;

  protection    = Public;
  mtype         = Method;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  g_specialBlock = FALSE;


  inputFile.setName(fileName);
  if (inputFile.open(IO_ReadOnly))
  {
    yyLineNr= 1 ; 
    yyFileName = fileName;
    //setContext();
    msg("Parsing file %s...\n",yyFileName.data());

    QFileInfo fi(fileName);
    g_moduleScope = findPackageScope(fileName);
    QCString baseName=fi.baseName().utf8();
    if (baseName!="__init__") // package initializer file is not a package itself
    {
      if (!g_moduleScope.isEmpty())
      {
	g_moduleScope+="::";
      }
      g_moduleScope+=baseName;
    }

    current            = new Entry;
    initEntry();
    current->name      = g_moduleScope;
    current->section   = Entry::NAMESPACE_SEC;
    current->type      = "namespace";
    current->fileName  = yyFileName;
    current->startLine = yyLineNr;
    current->bodyLine  = yyLineNr;

    rt->addSubEntry(current);

    current_root  = current ;
    initParser();
    current       = new Entry;

    groupEnterFile(yyFileName,yyLineNr);
    
    current->reset();
    initEntry();
    pyscannerYYrestart( pyscannerYYin );
    BEGIN( Search );
    pyscannerYYlex();
    g_lexInit=TRUE;

    groupLeaveFile(yyFileName,yyLineNr);

    current_root->program.resize(0);
    delete current; current=0;

    parseCompounds(current_root);

    inputFile.close();
  }
  
}

//----------------------------------------------------------------------------

static void parsePrototype(const QCString &text)
{
  //printf("**** parsePrototype(%s) begin\n",text.data());
  if (text.isEmpty()) 
  {
    warn(yyFileName,yyLineNr,"Empty prototype found!");
    return;
  }

  g_specialBlock = FALSE;
  g_packageCommentAllowed = FALSE;

  const char *orgInputString;
  int orgInputPosition;
  YY_BUFFER_STATE orgState;
  
  // save scanner state
  orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(pyscannerYYin, YY_BUF_SIZE));
  orgInputString = inputString; 
  orgInputPosition = inputPosition;

  // set new string
  inputString = text;
  inputPosition = 0;
  pyscannerYYrestart( pyscannerYYin );

  BEGIN( FunctionDec );

  pyscannerYYlex();
  g_lexInit=TRUE;

  current->name = current->name.stripWhiteSpace();
  if (current->section == Entry::MEMBERDOC_SEC && current->args.isEmpty())
    current->section = Entry::VARIABLEDOC_SEC;

  // restore original scanner state

  YY_BUFFER_STATE tmpBuf = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(orgState);
  yy_delete_buffer(tmpBuf);

  inputString = orgInputString; 
  inputPosition = orgInputPosition;

  //printf("**** parsePrototype end\n");
}

void pyscanFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION) 
  if (g_lexInit)
  {
    pyscannerYYlex_destroy();
  }
#endif
}

//----------------------------------------------------------------------------

void PythonLanguageScanner::parseInput(const char *fileName,
                                       const char *fileBuf,
                                       Entry *root,
                                       bool /*sameTranslationUnit*/,
                                       QStrList & /*filesInSameTranslationUnit*/)
{
  g_thisParser = this;
  printlex(yy_flex_debug, TRUE, __FILE__, fileName);
  ::parseMain(fileName,fileBuf,root);
  printlex(yy_flex_debug, FALSE, __FILE__, fileName);

  // May print the AST for debugging purposes
  // printAST(global_root);
}

bool PythonLanguageScanner::needsPreprocessing(const QCString &)
{
  return FALSE;
}

void PythonLanguageScanner::parseCode(CodeOutputInterface &codeOutIntf,
    const char *scopeName,
    const QCString &input,
    SrcLangExt /*lang*/,
    bool isExampleBlock,
    const char *exampleName,
    FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    MemberDef *memberDef,
    bool showLineNumbers,
    Definition *searchCtx,
    bool collectXRefs
    )
{
  ::parsePythonCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                    fileDef,startLine,endLine,inlineFragment,memberDef,
                    showLineNumbers,searchCtx,collectXRefs);
}

void PythonLanguageScanner::parsePrototype(const char *text)
{
  ::parsePrototype(text);

}

void PythonLanguageScanner::resetCodeParserState()
{
  ::resetPythonCodeParserState();
}

//----------------------------------------------------------------------------

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void pyscannerYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

