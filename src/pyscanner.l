/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
%option prefix="pyscannerYY"
%option reentrant
%option extra-type="struct pyscannerYY_state *"
%top{
#include <stdint.h>
}

%{

/*
 *	includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "pyscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "arguments.h"
#include "markdown.h"
#include "fileinfo.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

/* ----------------------------------------------------------------- */

struct pyscannerYY_state
{
  pyscannerYY_state() : defVal(std::ios_base::ate) {}
  CommentScanner          commentScanner;
  OutlineParserInterface *thisParser = 0;
  QCString                inputString;
  yy_size_t               inputPosition = 0;
  Protection              protection = Public;
  std::shared_ptr<Entry>  current_root;
  std::shared_ptr<Entry>  current;
  std::shared_ptr<Entry>  previous;
  std::shared_ptr<Entry>  bodyEntry;
  int                     yyLineNr = 1 ;
  QCString                yyFileName;
  MethodTypes             mtype = Method;
  bool                    stat = FALSE;
  Specifier               virt = Normal;
  int                     docBlockContext = 0;
  QCString                docBlock;
  bool                    docBlockInBody = FALSE;
  bool                    docBlockJavaStyle = FALSE;
  bool                    docBrief = FALSE;
  bool                    docBlockSpecial = FALSE;
  bool                    doubleQuote = FALSE;
  bool                    specialBlock = FALSE;
  int                     stringContext = 0;
  std::ostringstream *    copyString = 0;
  int                     indent = 0;
  int                     curIndent = 0;
  int                     commentIndent = 0;
  bool                    importTuple = FALSE;
  StringUnorderedMap      packageNameCache;
  char                    atomStart = 0;
  char                    atomEnd = 0;
  int                     atomCount = 0;
  QCString                moduleScope;
  QCString                packageName;
  std::ostringstream      defVal;
  int                     braceCount = 0;
  bool                    lexInit = FALSE;
  bool                    packageCommentAllowed = FALSE;
  bool                    start_init = FALSE;
  int                     search_count = 0;
  QCString                argType;
  bool                    funcParamsEnd = FALSE;
  std::vector<QCString>   decorators;
};

//-----------------------------------------------------------------------------
#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static inline int computeIndent(const char *s);

static void initParser(yyscan_t yyscanner);
static void initEntry(yyscan_t yyscanner);
static void newEntry(yyscan_t yyscanner);
static void newVariable(yyscan_t yyscanner);
static void newFunction(yyscan_t yyscanner);
static QCString findPackageScopeFromPath(yyscan_t yyscanner,const QCString &path);
static void addFrom(yyscan_t yyscanner,bool all);
static void lineCount(yyscan_t yyscanner);
static void incLineNr(yyscan_t yyscanner);
static void startCommentBlock(yyscan_t yyscanner,bool brief);
static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief);
static void endOfDef(yyscan_t yyscanner,int correction=0);
static inline void addToString(yyscan_t yyscanner,const char *s);
static void initTriDoubleQuoteBlock(yyscan_t yyscanner);
static void initTriSingleQuoteBlock(yyscan_t yyscanner);
static void initSpecialBlock(yyscan_t yyscanner);
static void searchFoundDef(yyscan_t yyscanner);
static void searchFoundClass(yyscan_t yyscanner);
static QCString findPackageScope(yyscan_t yyscanner,const char *fileName);

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);

//-----------------------------------------------------------------------------
/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

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
CALL              "("[^)]*")"
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
%x FunctionAnnotation
%x FunctionTypeAnnotation
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
%x SingleQuoteStringIgnore
%x DoubleQuoteStringIgnore

  /* import */
%x FromMod
%x FromModItem
%x Import

%%

  /* ------------ Function recognition rules -------------- */

<Search>{

    ^{B}"def"{BB}       { // start of a function/method definition with indent
      			  DBG_CTX((stderr,"Found def at %d\n",yyextra->yyLineNr));
      			  yyextra->indent=computeIndent(yytext);
			  searchFoundDef(yyscanner);
			  BEGIN( FunctionDec );
      			}
    "def"{BB}           { // start of a function/method definition
			  searchFoundDef(yyscanner);
			  BEGIN( FunctionDec );
                        }

     ^{B}"class"{BB}	{ // start of a class definition with indent
       			  DBG_CTX((stderr,"Found class at %d\n",yyextra->yyLineNr));
      			  yyextra->indent=computeIndent(yytext);
			  searchFoundClass(yyscanner);
			  BEGIN( ClassDec ) ;
       			}
     "class"{BB}        {  // start of a class definition
			  searchFoundClass(yyscanner);
			  BEGIN( ClassDec ) ;
                       }
     ^{B}"from"{BB}    |
     "from"{BB}	       { // start of an from import
			  yyextra->packageCommentAllowed = FALSE;
                          BEGIN( FromMod );
                       }

     ^{B}"import"{BB}  |
     "import"{BB}      { // start of an import statement
			  yyextra->packageCommentAllowed = FALSE;
                          BEGIN( Import );
                       }
     ^{B}{IDENTIFIER}/{B}"="{B}"property" { // property
      			yyextra->current->section   = Entry::VARIABLE_SEC;
  			yyextra->current->mtype     = Property;
			yyextra->current->name      = QCString(yytext).stripWhiteSpace();
			yyextra->current->fileName  = yyextra->yyFileName;
			yyextra->current->startLine = yyextra->yyLineNr;
			yyextra->current->bodyLine  = yyextra->yyLineNr;
			yyextra->packageCommentAllowed = FALSE;
			BEGIN(VariableDec);
       	              }
     ^{B}{IDENTIFIER}/{B}"="[^=] { // variable
                        if (yyextra->search_count) REJECT;
      			yyextra->indent=computeIndent(yytext);
      			yyextra->current->section   = Entry::VARIABLE_SEC;
			yyextra->current->name      = QCString(yytext).stripWhiteSpace();
			yyextra->current->fileName  = yyextra->yyFileName;
			yyextra->current->startLine = yyextra->yyLineNr;
			yyextra->current->bodyLine  = yyextra->yyLineNr;
			yyextra->packageCommentAllowed = FALSE;
			BEGIN(VariableDec);
      		      }
     {B}{IDENTIFIER}/({B},{B}{IDENTIFIER})*{B}")"*{B}"="[^=] { // list of variables, we cannot place the default value
                                                               // so we will skip it later on in a general rule
                                                               // Also note ")" this is to catch also (a,b). the "("
                                                               // is caught in the rule: [(], the ")" will be handled in [)]
                        if (yyextra->search_count > 1) REJECT;
      			yyextra->indent=computeIndent(yytext);
      			yyextra->current->section   = Entry::VARIABLE_SEC;
			yyextra->current->name      = QCString(yytext).stripWhiteSpace();
			yyextra->current->fileName  = yyextra->yyFileName;
			yyextra->current->startLine = yyextra->yyLineNr;
			yyextra->current->bodyLine  = yyextra->yyLineNr;
			yyextra->packageCommentAllowed = FALSE;
			newVariable(yyscanner);
      		      }
     "'"	      { // start of a single quoted string
       		        yyextra->stringContext=YY_START;
		        yyextra->copyString=0;
			yyextra->packageCommentAllowed = FALSE;
                        BEGIN( SingleQuoteString );
                      }
     "\""	      { // start of a double quoted string
       	                yyextra->stringContext=YY_START;
			yyextra->copyString=0;
			yyextra->packageCommentAllowed = FALSE;
                        BEGIN( DoubleQuoteString );
                      }
    "@staticmethod"  {
       			yyextra->stat=TRUE;
      		      }
    "@"{SCOPE}{CALL}? { // decorator
                      }
    {SCRIPTCOMMENT}   { // Unix type script comment
                        if (yyextra->yyLineNr != 1) REJECT;
      		      }
    {POUNDCOMMENT}    { // normal comment 
			yyextra->packageCommentAllowed = FALSE;
      		      }
    {IDENTIFIER}      { // some other identifier
			yyextra->packageCommentAllowed = FALSE;
		      }
    ^{BB}	      {
      			yyextra->curIndent=computeIndent(yytext);
                      }

    {NEWLINE}+        { // new line
                        lineCount(yyscanner);
                      }

    {TRIDOUBLEQUOTE}  { // start of a comment block
			initTriDoubleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			initTriSingleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}/[^#]    {  // start of a special comment
      			yyextra->curIndent=computeIndent(yytext);
			yyextra->packageCommentAllowed = FALSE;
			initSpecialBlock(yyscanner);
			BEGIN(SpecialComment);
                      }
    [(]               { // we have to do something with (
                        yyextra->search_count += 1;
                      }
    [)]               { // we have to do something with )
                        yyextra->search_count -= 1;
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
                        yyextra->packageName=yytext;
		      }
  "import"{B}	      {
    			BEGIN(FromModItem);
    		      }
  \n		      {
                        incLineNr(yyscanner);
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
                  addFrom(yyscanner,TRUE);
                  BEGIN(Search);
    		}
  {IDENTIFIER}/{B}","{B} {
                  addFrom(yyscanner,FALSE);
		}
  {IDENTIFIER}/{B}")" {
                  addFrom(yyscanner,FALSE);
                }
  {IDENTIFIER}  {
                  addFrom(yyscanner,FALSE);
                  if (!yyextra->importTuple)
                  {
                    BEGIN(Search);
                  }
		}
  \n		{
                  incLineNr(yyscanner);
                  if (!yyextra->importTuple)
                  {
                    BEGIN(Search);
                  }
                }
  {B}		{
		}
  "("           {
                  yyextra->importTuple=TRUE;
                }
  ")"           {
                  yyextra->importTuple=FALSE;
                  BEGIN(Search);
                }
  ","		{
  		}
  "\\"{B}\n     { // line continuation
                  incLineNr(yyscanner);
                }
  .             {
                  unput(*yytext);
                  BEGIN(Search);
                }
}

<Import>{
  {IDENTIFIER}({B}"."{B}{IDENTIFIER})* {
			yyextra->current->name=removeRedundantWhiteSpace(substitute(yytext,".","::"));
			yyextra->current->fileName = yyextra->yyFileName; 
			//printf("Adding using declaration: found:%s:%d name=%s\n",yyextra->yyFileName.data(),yyextra->yyLineNr,yyextra->current->name.data());
			yyextra->current->section=Entry::USINGDECL_SEC;
			yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
			initEntry(yyscanner);
			BEGIN(Search);
                      }
  \n		{
                  incLineNr(yyscanner);
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
                        DBG_CTX((stderr,"Found instance method variable %s in %s at %d\n",&yytext[5],yyextra->current_root->name.data(),yyextra->yyLineNr));
			yyextra->current->name=&yytext[5];
			yyextra->current->section=Entry::VARIABLE_SEC;
			yyextra->current->fileName  = yyextra->yyFileName;
			yyextra->current->startLine = yyextra->yyLineNr;
			yyextra->current->bodyLine  = yyextra->yyLineNr;
			yyextra->current->type.resize(0);
			if (yyextra->current->name.at(0)=='_') // mark as private
			{
			  yyextra->current->protection=Private;
			}
			newEntry(yyscanner);
                      }
    "cls."{IDENTIFIER}/{B}"=" {
                        DBG_CTX((stderr,"Found class method variable %s in %s at %d\n",&yytext[4],yyextra->current_root->name.data(),yyextra->yyLineNr));
			yyextra->current->name=&yytext[4];
			yyextra->current->section=Entry::VARIABLE_SEC;
			yyextra->current->fileName  = yyextra->yyFileName;
			yyextra->current->startLine = yyextra->yyLineNr;
			yyextra->current->bodyLine  = yyextra->yyLineNr;
			yyextra->current->type.resize(0);
			if (yyextra->current->name.at(0)=='_') // mark as private
			{
			  yyextra->current->protection=Private;
			}
			newEntry(yyscanner);
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
			initTriDoubleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			initTriSingleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}/[^#]    {  // start of a special comment
			initSpecialBlock(yyscanner);
			BEGIN(SpecialComment);
                      }
    {POUNDCOMMENT}    { // #
	              }
    "'"	              { // start of a single quoted string
       			yyextra->stringContext=YY_START;
			yyextra->copyString=0;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
       			yyextra->stringContext=YY_START;
			yyextra->copyString=0;
                        BEGIN( DoubleQuoteString );
                      }
    \n		      { incLineNr(yyscanner); }
    {IDENTIFIER}      // identifiers
    [^'"\.#a-z_A-Z\n]+  // other uninteresting stuff
    .                 // anything else
}

<FunctionBody>{
    \n{B}/{IDENTIFIER}{BB}  {
                        DBG_CTX((stderr,"indent %d<=%d\n",computeIndent(&yytext[1]),yyextra->indent));
                        if (computeIndent(&yytext[1])<=yyextra->indent) 
			{
			  int i;
			  for (i=(int)yyleng-1;i>=0;i--)
			  {
			    unput(yytext[i]);
			  }
			  endOfDef(yyscanner);
			  //YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                          BEGIN(Search);
			}
			else
			{
                          incLineNr(yyscanner);
      		          yyextra->current->program << yytext;
			}
                      }
    \n{B}/"##"	      {
                        if (computeIndent(&yytext[1])<=yyextra->indent)
			{
			  int i;
			  for (i=(int)yyleng-1;i>=0;i--)
			  {
			    unput(yytext[i]);
			  }
			  endOfDef(yyscanner);
			  //YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                          BEGIN(Search);
			}
			else
			{
			  incLineNr(yyscanner);
      		          yyextra->current->program << yytext;
			}
      		      }
    <<EOF>>	      {
			endOfDef(yyscanner);
			yyterminate();
      		      }
    ^{BB}/\n	      { // skip empty line
      		        yyextra->current->program << yytext;
      	              }
    ^{BB}	      { // something at indent >0
      		        yyextra->current->program << yytext;
			yyextra->curIndent = computeIndent(yytext);
                        if (yyextra->curIndent<=yyextra->indent) 
			  // jumped out of the function
			{
			  endOfDef(yyscanner,1);
                          BEGIN(Search);
			}
      		      }
    "'"	              { // start of a single quoted string
      		        yyextra->current->program << yytext;
       			yyextra->stringContext=YY_START;
		        yyextra->specialBlock = FALSE; 
			yyextra->copyString=&yyextra->current->program;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
      		        yyextra->current->program << yytext;
       			yyextra->stringContext=YY_START;
		        yyextra->specialBlock = FALSE; 
			yyextra->copyString=&yyextra->current->program;
                        BEGIN( DoubleQuoteString );
                      }
    [^ \t\n#'".]+     { // non-special stuff
      		        yyextra->current->program << yytext;
		        yyextra->specialBlock = FALSE; 
                      }
    ^{POUNDCOMMENT}   { // normal comment 
      		        yyextra->current->program << yytext;
      		      }
    "#".*             { // comment half way
      		        yyextra->current->program << yytext;
                      }
    {NEWLINE}	      { 
			incLineNr(yyscanner); 
      		        yyextra->current->program << yytext;
		      }
    .                 { // any character
      		        yyextra->current->program << *yytext;
		        yyextra->specialBlock = FALSE; 
                      }

    {TRIDOUBLEQUOTE}  { // start of a comment block
			yyextra->current->program << yytext;
			initTriDoubleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			yyextra->current->program << yytext;
			initTriSingleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}/[^#]  {  // start of a special comment
			initSpecialBlock(yyscanner);
			BEGIN(SpecialComment);
                      }
    
}

<FunctionDec>{
    {IDENTIFIER}            {
			      //found function name
			      if (yyextra->current->type.isEmpty()) 
			      {
				  yyextra->current->type = "def";
			      }
			      yyextra->current->name = yytext;
			      yyextra->current->name = yyextra->current->name.stripWhiteSpace();
			      newFunction(yyscanner);
                            }
    {B}":"{B}		    { // function without arguments
			      yyextra->specialBlock = TRUE; // expecting a docstring
			      yyextra->bodyEntry = yyextra->current;
                              yyextra->current->bodyLine  = yyextra->yyLineNr;
                              BEGIN(FunctionBody);
			    }

    "->"                    {
			      yyextra->defVal.str(std::string());
			      yyextra->braceCount = 0;
			      BEGIN(FunctionTypeAnnotation);
		            }
    {B}"("                  {
			      yyextra->funcParamsEnd = FALSE;
                              yyextra->current->bodyLine  = yyextra->yyLineNr;
			      BEGIN(FunctionParams);
		            }
    ")"                     { // end of parameter list
                              if (yyextra->current->argList.empty())
                              {
                                yyextra->current->argList.setNoParameters(TRUE);
                              }
        		      yyextra->current->args = argListToString(yyextra->current->argList);
        		      yyextra->funcParamsEnd = TRUE;
                            }
}

<FunctionParams>{
    ({BB}|",")          {
                        }

    [\*]+               {
                          yyextra->argType = yytext;
                        }
    {IDENTIFIER}        { // Name of parameter
			  lineCount(yyscanner);
			  Argument a;
			  a.name = QCString(yytext).stripWhiteSpace();
			  a.type = yyextra->argType;
			  yyextra->current->argList.push_back(a);
                          yyextra->argType = "";
                        }
    "="		        { // default value
                          // TODO: this rule is too simple, need to be able to
                          // match things like =")" as well!
			  yyextra->defVal.str(std::string());
			  yyextra->braceCount = 0;
			  BEGIN(FunctionParamDefVal);
      			}
     ")"                {
			  unput(*yytext);
			  BEGIN(FunctionDec);
                        }
     ":"{B}             {
			  yyextra->defVal.str(std::string());
			  yyextra->braceCount = 0;
			  BEGIN(FunctionAnnotation);
			}
    {POUNDCOMMENT}	{ // a comment
			}
    {PARAMNONEMPTY}     { // Default rule inside arguments.
                        }

}

<FunctionTypeAnnotation>{
     "{"		|
     "["		|
     "("		{
     			  ++yyextra->braceCount;
     			  yyextra->defVal << *yytext;
     			}
     "}"		|
     "]"		|
     ")"		{
			  --yyextra->braceCount;
			  yyextra->defVal << *yytext;
       			}
     ":"		{
			  if (yyextra->braceCount == 0)
			  {
			    yyextra->current->type = yyextra->defVal.str();
			    unput(*yytext);
			    BEGIN(FunctionDec);
			  }
			  else
     			    yyextra->defVal << *yytext;
       			}
     "'"                {
			  yyextra->defVal << *yytext;
			  yyextra->copyString=&yyextra->defVal;
			  yyextra->stringContext=FunctionTypeAnnotation;
			  BEGIN(SingleQuoteString);
                        }
     "\""               {
			  yyextra->defVal << *yytext;
			  yyextra->copyString=&yyextra->defVal;
			  yyextra->stringContext=FunctionTypeAnnotation;
			  BEGIN(DoubleQuoteString);
                        }
     \n                 {
			  yyextra->defVal << *yytext;
			  incLineNr(yyscanner);
			}
     .			{
			  yyextra->defVal << *yytext;
       			}
}

<FunctionAnnotation>{
     "{"		|
     "["		|
     "("		{
     			  ++yyextra->braceCount;
     			  yyextra->defVal << *yytext;
     			}
     "}"		|
     "]"		{
			  --yyextra->braceCount;
			  yyextra->defVal << *yytext;
       			}
     ")"		|
     "="		|
     ","		{
			  if (yyextra->braceCount == 0)
			  {
			    if (!yyextra->current->argList.empty())
			      yyextra->current->argList.back().type += yyextra->defVal.str();
			    if (*yytext != ',')
			      unput(*yytext);
			    BEGIN(FunctionParams);
			  }
			  else
			  {
			    if (*yytext == ')')
			      --yyextra->braceCount;
			    yyextra->defVal << *yytext;
     			  }
       			}
     "'"                {
			  yyextra->defVal << *yytext;
			  yyextra->copyString=&yyextra->defVal;
			  yyextra->stringContext=FunctionAnnotation;
			  BEGIN(SingleQuoteString);
                        }
     "\""               {
			  yyextra->defVal << *yytext;
			  yyextra->copyString=&yyextra->defVal;
			  yyextra->stringContext=FunctionAnnotation;
			  BEGIN(DoubleQuoteString);
                        }
     \n                 {
			  yyextra->defVal << *yytext;
			  incLineNr(yyscanner);
       			}
     .			{
			  yyextra->defVal << *yytext;
       			}
}

<FunctionParamDefVal>{
     "{"		| 
     "["		| 
     "("		{ // internal opening brace, assumption is that we have correct code so braces do match
       			  ++yyextra->braceCount;
			  yyextra->defVal << *yytext;
       			}
     "}"		|
     "]"		{
			  --yyextra->braceCount;
			  yyextra->defVal << *yytext;
       			}
     ")"		|
     ","		{
			  if (yyextra->braceCount == 0)
			  {
			    if (!yyextra->current->argList.empty())
			      yyextra->current->argList.back().defval=QCString(yyextra->defVal.str()).stripWhiteSpace();
			    if (*yytext == ')')
			      unput(*yytext);
			    BEGIN(FunctionParams);
			  }
     			  else
     			  {
			    if (*yytext == ')')
			      --yyextra->braceCount;
     			    yyextra->defVal << *yytext;
     			  }
       			}

     "'"                {
                          yyextra->defVal << *yytext;
                          yyextra->copyString=&yyextra->defVal;
                          yyextra->stringContext=FunctionParamDefVal;
                          BEGIN( SingleQuoteString );
                        }
     "\""               {
                          yyextra->defVal << *yytext;
                          yyextra->copyString=&yyextra->defVal;
                          yyextra->stringContext=FunctionParamDefVal;
                          BEGIN( DoubleQuoteString );
                        }
     \n                 {
       			    yyextra->defVal << *yytext;
			    incLineNr(yyscanner);
       			}
     .			{
			    yyextra->defVal << *yytext;
       			}
}


<ClassBody>{
    \n/{IDENTIFIER}{BB}  { // new def at indent 0
                        incLineNr(yyscanner);
			endOfDef(yyscanner);
			//yyextra->hideClassDocs = FALSE;
			//YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                        BEGIN(Search);
                      }
    \n/"##"[^#]       {  // start of a special comment at indent 0
                        incLineNr(yyscanner);
			endOfDef(yyscanner);
			//yyextra->hideClassDocs = FALSE;
			//YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                        BEGIN(Search);
                      }
    ^{BB}/\n	      { // skip empty line
      		        yyextra->current->program << yytext;
      	              }
    <<EOF>>	      {
			endOfDef(yyscanner);
			yyterminate();
      		      }
    ^{BB}	      { // something at indent >0
                        yyextra->curIndent=computeIndent(yytext);
			DBG_CTX((stderr,"yyextra->curIndent=%d yyextra->indent=%d\n",yyextra->curIndent,yyextra->indent));
                        if (yyextra->curIndent<=yyextra->indent) 
			  // jumped out of the class/method
			{
			  endOfDef(yyscanner,1);
			  yyextra->indent=yyextra->curIndent;
			  // make sure the next rule matches ^...
			  //YY_CURRENT_BUFFER->yy_at_bol=TRUE;
			  //yyextra->hideClassDocs = FALSE;
                          BEGIN(Search);
			}
			else
			{
      		          yyextra->current->program << yytext;
			}
      		      }
    "'"	              { // start of a single quoted string
      		        yyextra->current->program << *yytext;
       			yyextra->stringContext=YY_START;
		        yyextra->specialBlock = FALSE; 
			yyextra->copyString=&yyextra->current->program;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
      		        yyextra->current->program << *yytext;
       			yyextra->stringContext=YY_START;
		        yyextra->specialBlock = FALSE; 
			yyextra->copyString=&yyextra->current->program;
                        BEGIN( DoubleQuoteString );
                      }
    [^ \t\n#'"]+      { // non-special stuff
      		        yyextra->current->program << yytext;
		        yyextra->specialBlock = FALSE; 
			//yyextra->hideClassDocs = FALSE;
                      }
    {NEWLINE}	      { 
      		        yyextra->current->program << *yytext;
			incLineNr(yyscanner); 
		      }
    {POUNDCOMMENT}    { // normal comment
      		        yyextra->current->program << yytext;
                      }
    .                 { // any character
		        yyextra->specialBlock = FALSE; 
      		        yyextra->current->program << *yytext;
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
      		        //if (!yyextra->hideClassDocs) 
			yyextra->current->program << yytext;
			initTriDoubleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
      		        //if (!yyextra->hideClassDocs) 
			yyextra->current->program << yytext;
			initTriSingleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }
}

<ClassDec>{IDENTIFIER} {
			  if (yyextra->current->type.isEmpty()) 
			  {
			      yyextra->current->type = "class";
			  }

			  yyextra->current->section = Entry::CLASS_SEC;
			  yyextra->current->name = yytext;

			  // prepend scope in case of nested classes
			  if (yyextra->current_root->section&Entry::SCOPE_MASK)
			  {
			    //printf("*** Prepending scope %s to class %s\n",yyextra->current_root->name.data(),yyextra->current->name.data());
			    yyextra->current->name.prepend(yyextra->current_root->name+"::");
			  }
			  
			  yyextra->current->name = yyextra->current->name.stripWhiteSpace();
			  yyextra->current->fileName = yyextra->yyFileName;        
			  yyextra->docBlockContext   = YY_START;
			  yyextra->docBlockInBody    = FALSE;
			  yyextra->docBlockJavaStyle = FALSE;
			  yyextra->docBlock.resize(0);

			  BEGIN(ClassInheritance);                 
                        }

<ClassInheritance>{
   ({BB}|[\(,\)])      { // syntactic sugar for the list
                       }

    ":"                { // begin of the class definition
			 yyextra->specialBlock = TRUE; // expecting a docstring
                         yyextra->current->bodyLine  = yyextra->yyLineNr;
			 yyextra->current->program.str(std::string());
      			 BEGIN(ClassCaptureIndent);
                       }

    {SCOPE}            {
                         yyextra->current->extends.push_back(
					      BaseInfo(substitute(yytext,".","::"),Public,Normal)
					    );
                         //Has base class-do stuff
                       }
    "'"	               { // start of a single quoted string
       			 yyextra->stringContext=YY_START;
                         BEGIN( SingleQuoteStringIgnore );
                       }
    "\""               { // start of a double quoted string
       			 yyextra->stringContext=YY_START;
                         BEGIN( DoubleQuoteStringIgnore );
                       }
}

<SingleQuoteStringIgnore>{
    "'"	               { // end of a single quoted string
			 BEGIN(yyextra->stringContext);
                       }
    .	               { }
}
<DoubleQuoteStringIgnore>{
    "\""	       { // end of a double quoted string
			 BEGIN(yyextra->stringContext);
                       }
    .	               { }
}

<ClassCaptureIndent>{
    "\n"|({BB}"\n")            {
                                 // Blankline - ignore, keep looking for indentation.
				 lineCount(yyscanner);
      				 yyextra->current->program << yytext;
                               }

    {TRIDOUBLEQUOTE}           { // start of a comment block
				 initTriDoubleQuoteBlock(yyscanner);
      				 yyextra->current->program << yytext;
			 	 BEGIN(TripleComment);
                      	       }
    {TRISINGLEQUOTE}           { // start of a comment block
			         initTriSingleQuoteBlock(yyscanner);
      				 yyextra->current->program << yytext;
			         BEGIN(TripleComment);
                               }
    {STARTDOCSYMS}[#]*         {  // start of a special comment
                                 initSpecialBlock(yyscanner);
                                 BEGIN(SpecialComment);
                               }
    {POUNDCOMMENT}             { // ignore comment with just one #
                               }
    ^{BB} 		       {
      				 yyextra->current->program << yytext;
			         //yyextra->current->startLine = yyextra->yyLineNr;
				 yyextra->curIndent=computeIndent(yytext);
				 yyextra->bodyEntry = yyextra->current;
				 DBG_CTX((stderr,"setting indent %d\n",yyextra->curIndent));
				 //printf("yyextra->current->program=[%s]\n",yyextra->current->program.data());
				 //yyextra->hideClassDocs = TRUE;
			         BEGIN(ClassBody);
                               }

    ""/({NONEMPTY}|{EXPCHAR})  {
				 // Just pushback an empty class, and
				 // resume parsing the body.
                                 newEntry(yyscanner);
      				 yyextra->current->program << yytext;

				 // printf("Failed to find indent - skipping!");
				 BEGIN( Search );
                               }
}


<VariableDec>{
   "="                { // the assignment operator
                        //printf("====== VariableDec at line %d\n",yyextra->yyLineNr);
                        yyextra->start_init = TRUE;
			yyextra->current->initializer.str(yytext);
			yyextra->current->initializer << " ";
                      }
   {B}                { // spaces
			yyextra->current->initializer << yytext;
	              }
   {INTNUMBER}	      { // integer value
			if (yyextra->current-> type.isEmpty()) yyextra->current->type = "int";
			yyextra->current->initializer << yytext;
		      }
   {FLOATNUMBER}      { // floating point value
			if (yyextra->current->type.isEmpty()) yyextra->current->type = "float";
			yyextra->current->initializer << yytext;
		      }
   {BOOL}             { // boolean value
			if (yyextra->current->type.isEmpty()) yyextra->current->type = "bool";
			yyextra->current->initializer << yytext;
		      }
   {STRINGPREFIX}?"'" { // string
			if (yyextra->current->type.isEmpty()) yyextra->current->type = "string";
			yyextra->current->initializer << yytext;
			yyextra->copyString=&yyextra->current->initializer;
       			yyextra->stringContext=VariableDec;
                        BEGIN( SingleQuoteString );
     		      }
   {STRINGPREFIX}?"\"" { // string
			if (yyextra->current->type.isEmpty()) yyextra->current->type = "string";
			yyextra->current->initializer << yytext;
			yyextra->copyString=&yyextra->current->initializer;
       			yyextra->stringContext=VariableDec;
                        BEGIN( DoubleQuoteString );
     		      }
   {TRIDOUBLEQUOTE}   { // start of a comment block
			if (yyextra->current->type.isEmpty()) yyextra->current->type = "string";
			yyextra->current->initializer << yytext;
			yyextra->doubleQuote=TRUE;
			yyextra->copyString=&yyextra->current->initializer;
       			yyextra->stringContext=VariableDec;
			BEGIN(TripleString);
                      }

   {TRISINGLEQUOTE}   { // start of a comment block
			if (yyextra->current->type.isEmpty()) yyextra->current->type = "string";
			yyextra->current->initializer << yytext;
			yyextra->doubleQuote=FALSE;
			yyextra->copyString=&yyextra->current->initializer;
       			yyextra->stringContext=VariableDec;
			BEGIN(TripleString);
                      }
   "("		      { // tuple, only when direct after =
                        if (yyextra->current->mtype!=Property && yyextra->start_init)
			{
			  yyextra->current->type = "tuple";
			}
      		        yyextra->current->initializer << *yytext;
                        yyextra->atomStart='(';
			yyextra->atomEnd=')';
			yyextra->atomCount=1;
			BEGIN( VariableAtom );
                      }
   "["                { // list
                        if (yyextra->start_init) yyextra->current->type = "list";
      		        yyextra->current->initializer << *yytext;
                        yyextra->atomStart='[';
			yyextra->atomEnd=']';
			yyextra->atomCount=1;
			BEGIN( VariableAtom );
                      }
   "{"		      { // dictionary
                        if (yyextra->start_init) yyextra->current->type = "dictionary";
      		        yyextra->current->initializer << *yytext;
                        yyextra->atomStart='{';
			yyextra->atomEnd='}';
			yyextra->atomCount=1;
			BEGIN( VariableAtom );
                      }
   "#".*              { // comment
                        BEGIN( VariableEnd ); 
                      }
   {IDENTIFIER}	      {
			// do something based on the type of the IDENTIFIER
			if (yyextra->current->type.isEmpty())
                        {
                          for (const auto &child : yyextra->current_root->children())
                          {
                            if (child->name == QCString(yytext))
                            {
                               yyextra->current->type = child->type;
                               break;
                            }
                          }
                        }
                        yyextra->start_init = FALSE;
      		        yyextra->current->initializer << yytext;
	              }
   .		      {
                        yyextra->start_init = FALSE;
      		        yyextra->current->initializer << *yytext;
                      }
   \n		      {
     		        unput('\n');
     			BEGIN( VariableEnd );
     		      }
}

<VariableAtom>{
    [\(\[\{]	      {
      		        yyextra->current->initializer << *yytext;
                        if (yyextra->atomStart==*yytext)
			{
			  yyextra->atomCount++;
			}
                      }
    [\)\]\}]	      {
      		        yyextra->current->initializer << *yytext;
                        if (yyextra->atomEnd==*yytext)
			{
			  yyextra->atomCount--;
			}
			if (yyextra->atomCount==0)
			{
                          yyextra->start_init = FALSE;
			  BEGIN(VariableDec);
			}
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
                        yyextra->specialBlock = FALSE;
			yyextra->current->program << yytext;
			initTriDoubleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
                        yyextra->specialBlock = FALSE;
			yyextra->current->program << yytext;
			initTriSingleQuoteBlock(yyscanner);
			BEGIN(TripleComment);
                      }
   "'"	              {
       			yyextra->stringContext=YY_START;
			yyextra->current->initializer << "'";
			yyextra->copyString=&yyextra->current->initializer;
                        BEGIN( SingleQuoteString );
                      }
   "\""	              {
       			yyextra->stringContext=YY_START;
			yyextra->current->initializer << "\"";
			yyextra->copyString=&yyextra->current->initializer;
                        BEGIN( DoubleQuoteString );
                      }
   {IDENTIFIER}       {
      		        yyextra->current->initializer << yytext;
	              }
   .		      {
      		        yyextra->current->initializer << *yytext;
                      }
   \n		      {
      		        yyextra->current->initializer << *yytext;
     			incLineNr(yyscanner);
     		      }

}

<VariableEnd>{
    \n                {
                        incLineNr(yyscanner);
			newVariable(yyscanner);
                        BEGIN(Search);
                      }
    .		      { 
                        unput(*yytext);
			newVariable(yyscanner);
                        BEGIN(Search);
      		      }
    <<EOF>>           { yyterminate();
                      }
}

<TripleComment>{
    {ENDTRIDOUBLEQUOTE}   | 
    {ENDTRISINGLEQUOTE}   {
			  // printf("Expected module block %d special=%d\n",yyextra->expectModuleDocs,yyextra->specialBlock);
			  if (yyextra->doubleQuote==(yytext[0]=='"')) 
			  {
			    if (yyextra->specialBlock) // expecting a docstring
			    {
			      QCString actualDoc=yyextra->docBlock;
			      if (!yyextra->docBlockSpecial) // legacy unformatted docstring
			      {
                                if (!actualDoc.isEmpty())
                                {
                                  stripIndentation(actualDoc,yyextra->commentIndent);
			          actualDoc.prepend("\\verbatim\n");
			          actualDoc.append("\\endverbatim ");
			        }
			      }
			      //printf("-------> yyextra->current=%p yyextra->bodyEntry=%p\n",yyextra->current,yyextra->bodyEntry);
			      handleCommentBlock(yyscanner, actualDoc, FALSE);
			    }
			    else if (yyextra->packageCommentAllowed) // expecting module docs
			    {
			      QCString actualDoc=yyextra->docBlock;
			      if (!yyextra->docBlockSpecial) // legacy unformatted docstring
			      {
                                if (!actualDoc.isEmpty())
                                {
                                  stripIndentation(actualDoc,yyextra->commentIndent);
			          actualDoc.prepend("\\verbatim\n");
			          actualDoc.append("\\endverbatim ");
			        }
			      }
			      actualDoc.prepend("\\namespace "+yyextra->moduleScope+" ");
			      handleCommentBlock(yyscanner, actualDoc, FALSE);
			    }
			    if ((yyextra->docBlockContext==ClassBody /*&& !yyextra->hideClassDocs*/) ||
				yyextra->docBlockContext==FunctionBody)
			    {
			      yyextra->current->program << yyextra->docBlock;
			      yyextra->current->program << yytext;
			    }
                            //if (yyextra->hideClassDocs)
			    //{
                            //  yyextra->current->startLine = yyextra->yyLineNr;
			    //}
			    //yyextra->hideClassDocs=FALSE;
			    BEGIN(yyextra->docBlockContext);
			  }
                          else 
			  {
                            yyextra->docBlock += yytext;
                          }
			  yyextra->packageCommentAllowed = FALSE;
                        }


    ^{BB}		{ // leading whitespace
			  yyextra->docBlock += yytext;
      			}
    [^"'\n \t\\]+       {
			  yyextra->docBlock += yytext;
                        }
    \n			{
      			  incLineNr(yyscanner);
			  yyextra->docBlock += yytext;
      			}
    \\.		        { // escaped char
			  yyextra->docBlock += yytext;
      			}
    .			{
			  yyextra->docBlock += yytext;
      			}
}

<SpecialComment>{
    ^{B}"#"("#")*	{ // skip leading hashes
      			}
    \n/{B}"#"		{ // continuation of the comment on the next line
      			  yyextra->docBlock+='\n';
			  yyextra->docBrief = FALSE;
			  incLineNr(yyscanner);
      			}
    [^#\n]+             { // any other stuff
      			  yyextra->docBlock+=yytext;
      			}
    \n			{ // new line that ends the comment
			  handleCommentBlock(yyscanner, yyextra->docBlock, yyextra->docBrief);
      			  incLineNr(yyscanner);
			  BEGIN(yyextra->docBlockContext);
      			}
    .			{ // anything we missed
      			  yyextra->docBlock+=*yytext;
      			}
}

<SingleQuoteString>{
    \\{B}\n                    { // line continuation
      			         addToString(yyscanner,yytext);
      				 incLineNr(yyscanner);
                               }
    \\.			       { // escaped char
      			         addToString(yyscanner,yytext);
                               }
    "\"\"\""		       { // triple double quotes
      			         addToString(yyscanner,yytext);
      			       }
    "'"			       { // end of the string
      			         addToString(yyscanner,yytext);
      		                 BEGIN(yyextra->stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
      			         addToString(yyscanner,yytext);
                               }
    .			       { // normal char
      			         addToString(yyscanner,yytext);
                               }
}

<DoubleQuoteString>{
    \\{B}\n                    { // line continuation
      			         addToString(yyscanner,yytext);
      				 incLineNr(yyscanner);
                               }
    \\.			       { // escaped char
      			         addToString(yyscanner,yytext);
                               }
    "'''"		       { // triple single quotes
      			         addToString(yyscanner,yytext);
      			       }
    "\""		       { // end of the string
      			         addToString(yyscanner,yytext);
      		                 BEGIN(yyextra->stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
      			         addToString(yyscanner,yytext);
                               }
    .			       { // normal char
      			         addToString(yyscanner,yytext);
                               }
}

<TripleString>{
    {ENDTRIDOUBLEQUOTE}    | 
    {ENDTRISINGLEQUOTE}    {
                          *yyextra->copyString << yytext;
			  if (yyextra->doubleQuote==(yytext[0]=='"')) 
			  {
			    BEGIN(yyextra->stringContext);
			  }
                        }


    ({LONGSTRINGBLOCK}) {
			  lineCount(yyscanner);
                          *yyextra->copyString << yytext;
                        }
    \n			{
      			  incLineNr(yyscanner);
                          *yyextra->copyString << yytext;
      			}
    .			{
                          *yyextra->copyString << *yytext;
      			}
}

  /* ------------ End rules -------------- */

  /*
<*>({NONEMPTY}|{EXPCHAR}|{BB})           { // This should go one character at a time.
				 // printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyextra->yyLineNr);

                               }
  */

<*>{NEWLINE}                   {
				 //printf("[pyscanner] %d NEWLINE [line %d] no match\n",
				 //       YY_START, yyextra->yyLineNr);

                                 lineCount(yyscanner);
                               }

<*>"'"                         {
                                 //fprintf(stderr,"Quote: %d\n",YY_START);
                               }

<*>.                           {
				 //printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyextra->yyLineNr);

                               }


%%

//----------------------------------------------------------------------------

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t c=0;
  const char *p = yyextra->inputString + yyextra->inputPosition;
  while ( c < max_size && *p ) { *buf++ = *p++; c++; }
  yyextra->inputPosition+=c;
  return c;
}

static void initParser(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->protection = Public;
  yyextra->mtype = Method;
  yyextra->stat = FALSE;
  yyextra->virt = Normal;
  yyextra->previous = 0;
  yyextra->packageCommentAllowed = TRUE;
}

static void initEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //yyextra->current->python = TRUE;
  yyextra->current->protection = yyextra->protection ;
  yyextra->current->mtype      = yyextra->mtype;
  yyextra->current->virt       = yyextra->virt;
  yyextra->current->stat       = yyextra->stat;
  yyextra->current->lang       = SrcLangExt_Python; 
  yyextra->commentScanner.initGroupInfo(yyextra->current.get());
  yyextra->stat = FALSE;
}

static void newEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->previous = yyextra->current;
  yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  initEntry(yyscanner);
}

static void newVariable(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->current->name.isEmpty() && yyextra->current->name.at(0)=='_') // mark as private
  {
    yyextra->current->protection=Private;
  }
  if (yyextra->current_root->section&Entry::COMPOUND_MASK) // mark as class variable
  {
    yyextra->current->stat = TRUE;
  }
  newEntry(yyscanner);
}

static void newFunction(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current->name.left(2)=="__" && yyextra->current->name.right(2)=="__")
  {
    // special method name, see
    // http://docs.python.org/ref/specialnames.html
    yyextra->current->protection=Public;
  }
  else if (yyextra->current->name.at(0)=='_')
  {
    yyextra->current->protection=Private;
  }
}

static inline int computeIndent(const char *s)
{
  int col=0;
  int tabSize=Config_getInt(TAB_SIZE);
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

static QCString findPackageScopeFromPath(yyscan_t yyscanner,const QCString &path)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  auto it = yyextra->packageNameCache.find(path.str());
  if (it!=yyextra->packageNameCache.end())
  {
    return it->second;
  }
  FileInfo pf(path.str()+"/__init__.py"); // found package initialization file
  if (pf.exists())
  {
    int i=path.findRev('/');
    if (i!=-1)
    {
      QCString scope = findPackageScopeFromPath(yyscanner,path.left(i));
      if (!scope.isEmpty())
      {
	scope+="::";
      }
      scope+=path.mid(i+1);
      yyextra->packageNameCache.insert(std::make_pair(path.str(),scope.str()));
      return scope;
    }
  }
  return "";
}
  
static QCString findPackageScope(yyscan_t yyscanner,const char *fileName)
{
  if (fileName==0) return "";
  FileInfo fi(fileName);
  return findPackageScopeFromPath(yyscanner,fi.dirPath(true).c_str());
}

static void addFrom(yyscan_t yyscanner,bool all)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString item=all ? yyextra->packageName : yyextra->packageName+"."+yytext;
  yyextra->current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
  yyextra->current->fileName = yyextra->yyFileName; 
  //printf("Adding using declaration: found:%s:%d name=%s\n",yyextra->yyFileName.data(),yyextra->yyLineNr,yyextra->current->name.data());
  yyextra->current->section=all ? Entry::USINGDIR_SEC : Entry::USINGDECL_SEC;
  yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  initEntry(yyscanner);
}
//-----------------------------------------------------------------------------

static void lineCount(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"yyextra->yyLineNr=%d\n",yyextra->yyLineNr));
  for (const char *p = yytext; *p; ++p)
  {
    yyextra->yyLineNr += (*p == '\n') ;
  }
}

static void incLineNr(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"yyextra->yyLineNr=%d\n",yyextra->yyLineNr));
  yyextra->yyLineNr++;
}

//-----------------------------------------------------------------
static void startCommentBlock(yyscan_t yyscanner,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (brief)
  {
    yyextra->current->briefFile = yyextra->yyFileName;
    yyextra->current->briefLine = yyextra->yyLineNr;
  }
  else
  {
    yyextra->current->docFile = yyextra->yyFileName;
    yyextra->current->docLine = yyextra->yyLineNr;
  }
}

static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("handleCommentBlock(doc=[%s] brief=%d yyextra->docBlockInBody=%d yyextra->docBlockJavaStyle=%d\n",
  //    doc.data(),brief,yyextra->docBlockInBody,yyextra->docBlockJavaStyle);

  // TODO: Fix me
  yyextra->docBlockInBody=FALSE;
  
  if (!yyextra->current->doc.isEmpty())
  {
    yyextra->current->doc=yyextra->current->doc.stripWhiteSpace()+"\n\n";
  }
  if (yyextra->docBlockInBody && yyextra->previous && !yyextra->previous->doc.isEmpty())
  {
    yyextra->previous->doc=yyextra->previous->doc.stripWhiteSpace()+"\n\n";
  }

  int position = 0;
  bool needsEntry = false;
  int lineNr = brief ? yyextra->current->briefLine : yyextra->current->docLine;
  Markdown markdown(yyextra->yyFileName,lineNr);
  QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(doc,lineNr) : doc;
  while (yyextra->commentScanner.parseCommentBlock(
        yyextra->thisParser,
        (yyextra->docBlockInBody && yyextra->previous) ? yyextra->previous.get() : yyextra->current.get(),
        processedDoc, // text
        yyextra->yyFileName,   // file
        lineNr,
        yyextra->docBlockInBody ? FALSE : brief, 
        yyextra->docBlockJavaStyle, // javadoc style // or FALSE,
        yyextra->docBlockInBody,
        yyextra->protection,
        position,
        needsEntry,
        Config_getBool(MARKDOWN_SUPPORT))
     ) // need to start a new entry
  {
    if (needsEntry)
    {
      newEntry(yyscanner);
    }
  }
  if (needsEntry)
  {
    newEntry(yyscanner);
  }

}

static void endOfDef(yyscan_t yyscanner,int correction)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("endOfDef at=%d\n",yyextra->yyLineNr);
  if (yyextra->bodyEntry)
  {
    yyextra->bodyEntry->endBodyLine  = yyextra->yyLineNr-correction;
    yyextra->bodyEntry = 0;
  }
  newEntry(yyscanner);
  //yyextra->insideConstructor = FALSE;
}

static inline void addToString(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->copyString) (*yyextra->copyString) << s;
}

static void initTriDoubleQuoteBlock(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->docBlockContext   = YY_START;
  yyextra->docBlockInBody    = FALSE;
  yyextra->docBlockJavaStyle = TRUE;
  yyextra->docBlockSpecial   = yytext[strlen(yytext) - 1]=='!' || !Config_getBool(PYTHON_DOCSTRING);
  yyextra->docBlock.resize(0);
  yyextra->commentIndent = yyextra->curIndent;
  yyextra->doubleQuote = TRUE;
  startCommentBlock(yyscanner,FALSE);
}

static void initTriSingleQuoteBlock(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->docBlockContext   = YY_START;
  yyextra->docBlockInBody    = FALSE;
  yyextra->docBlockJavaStyle = TRUE;
  yyextra->docBlockSpecial   = yytext[strlen(yytext) - 1]=='!' || !Config_getBool(PYTHON_DOCSTRING);
  yyextra->docBlock.resize(0);
  yyextra->commentIndent = yyextra->curIndent;
  yyextra->doubleQuote = FALSE;
  startCommentBlock(yyscanner,FALSE);
}

static void initSpecialBlock(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->docBlockContext   = YY_START;
  yyextra->docBlockInBody    = FALSE;
  yyextra->docBlockJavaStyle = TRUE;
  yyextra->docBrief = TRUE;
  yyextra->docBlock.resize(0);
  yyextra->commentIndent = yyextra->curIndent;
  startCommentBlock(yyscanner,FALSE);
}

static void searchFoundDef(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->fileName  = yyextra->yyFileName;
  yyextra->current->startLine = yyextra->yyLineNr;
  yyextra->current->bodyLine  = yyextra->yyLineNr;
  yyextra->current->section = Entry::FUNCTION_SEC;
  yyextra->current->lang = SrcLangExt_Python;
  yyextra->current->virt = Normal;
  yyextra->current->stat = yyextra->stat;
  yyextra->current->mtype = yyextra->mtype = Method;
  yyextra->current->type.resize(0);
  yyextra->current->name.resize(0);
  yyextra->current->args.resize(0);
  yyextra->current->argList.clear();
  yyextra->packageCommentAllowed = FALSE;
  yyextra->stat=FALSE;
  //printf("searchFoundDef at=%d\n",yyextra->yyLineNr);
}

static void searchFoundClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->section = Entry::CLASS_SEC;
  yyextra->current->argList.clear();
  yyextra->current->type += "class" ;
  yyextra->current->fileName  = yyextra->yyFileName;
  yyextra->current->startLine  = yyextra->yyLineNr;
  yyextra->current->bodyLine  = yyextra->yyLineNr;
  yyextra->packageCommentAllowed = FALSE;
}

//----------------------------------------------------------------------------

static void parseCompounds(yyscan_t yyscanner,std::shared_ptr<Entry> rt)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("parseCompounds(%s)\n",rt->name.data());
  for (size_t i=0; i<rt->children().size(); ++i)
  {
    std::shared_ptr<Entry> ce = rt->children()[i];
    if (ce->program.tellp() != std::streampos(0))
    {
      //fprintf(stderr,"parseCompounds: -- %s (line %d) ---------\n%s\n---------------\n",
      //  ce->name.data(), ce->bodyLine, ce->program.data());
      // init scanner state
      yyextra->inputString = ce->program.str();
      yyextra->inputPosition = 0;
      pyscannerYYrestart( 0, yyscanner );
      if (ce->section&Entry::COMPOUND_MASK)
      {
        yyextra->current_root = ce;
        BEGIN( Search );
      }
      else if (ce->parent())
      {
        yyextra->current_root = rt;
	//printf("Searching for member variables in %s parent=%s\n",
	//    ce->name.data(),ce->parent->name.data());
	BEGIN( SearchMemVars );
      }
      yyextra->yyFileName = ce->fileName;
      yyextra->yyLineNr   = ce->bodyLine ;
      yyextra->current = std::make_shared<Entry>();
      initEntry(yyscanner);

      QCString name = ce->name;
      yyextra->commentScanner.enterCompound(yyextra->yyFileName,yyextra->yyLineNr,name);

      pyscannerYYlex(yyscanner) ;
      yyextra->lexInit=TRUE;
      ce->program.str(std::string());

      yyextra->commentScanner.leaveCompound(yyextra->yyFileName,yyextra->yyLineNr,name);

    }
    parseCompounds(yyscanner,ce);
  }
}

//----------------------------------------------------------------------------


static void parseMain(yyscan_t yyscanner, const char *fileName,const char *fileBuf,const std::shared_ptr<Entry> &rt)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  initParser(yyscanner);

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;

  yyextra->protection    = Public;
  yyextra->mtype         = Method;
  yyextra->stat         = FALSE;
  yyextra->virt          = Normal;
  yyextra->current_root  = rt;
  yyextra->specialBlock = FALSE;


  yyextra->yyLineNr= 1 ; 
  yyextra->yyFileName = fileName;
  //setContext();
  msg("Parsing file %s...\n",yyextra->yyFileName.data());

  FileInfo fi(fileName);
  yyextra->moduleScope = findPackageScope(yyscanner,fileName);
  QCString baseName=fi.baseName();
  if (baseName!="__init__") // package initializer file is not a package itself
  {
    if (!yyextra->moduleScope.isEmpty())
    {
      yyextra->moduleScope+="::";
    }
    yyextra->moduleScope+=baseName;
  }

  yyextra->current            = std::make_shared<Entry>();
  initEntry(yyscanner);
  yyextra->current->name      = yyextra->moduleScope;
  yyextra->current->section   = Entry::NAMESPACE_SEC;
  yyextra->current->type      = "namespace";
  yyextra->current->fileName  = yyextra->yyFileName;
  yyextra->current->startLine = yyextra->yyLineNr;
  yyextra->current->bodyLine  = yyextra->yyLineNr;

  yyextra->current_root  = yyextra->current;

  rt->moveToSubEntryAndRefresh(yyextra->current);

  initParser(yyscanner);

  yyextra->commentScanner.enterFile(yyextra->yyFileName,yyextra->yyLineNr);

  yyextra->current->reset();
  initEntry(yyscanner);
  pyscannerYYrestart(0,yyscanner);
  BEGIN( Search );
  pyscannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->commentScanner.leaveFile(yyextra->yyFileName,yyextra->yyLineNr);

  yyextra->current_root->program.str(std::string());

  parseCompounds(yyscanner, yyextra->current_root);
}

//----------------------------------------------------------------------------

static void parsePrototype(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("**** parsePrototype(%s) begin\n",text.data());
  if (text.isEmpty()) 
  {
    warn(yyextra->yyFileName,yyextra->yyLineNr,"Empty prototype found!");
    return;
  }

  yyextra->specialBlock = FALSE;
  yyextra->packageCommentAllowed = FALSE;

  const char *orgInputString;
  yy_size_t orgInputPosition;
  YY_BUFFER_STATE orgState;
  
  // save scanner state
  orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner), yyscanner);
  orgInputString = yyextra->inputString; 
  orgInputPosition = yyextra->inputPosition;

  // set new string
  yyextra->inputString = text;
  yyextra->inputPosition = 0;
  pyscannerYYrestart( 0, yyscanner );

  BEGIN( FunctionDec );

  pyscannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->current->name = yyextra->current->name.stripWhiteSpace();
  if (yyextra->current->section == Entry::MEMBERDOC_SEC && yyextra->current->args.isEmpty())
    yyextra->current->section = Entry::VARIABLEDOC_SEC;

  // restore original scanner state

  YY_BUFFER_STATE tmpBuf = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(orgState, yyscanner);
  yy_delete_buffer(tmpBuf, yyscanner);

  yyextra->inputString = orgInputString; 
  yyextra->inputPosition = orgInputPosition;

  //printf("**** parsePrototype end\n");
}

//----------------------------------------------------------------------------

struct PythonOutlineParser::Private
{
  yyscan_t yyscanner;
  pyscannerYY_state state;
};

PythonOutlineParser::PythonOutlineParser() : p(std::make_unique<PythonOutlineParser::Private>())
{
  pyscannerYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  pyscannerYYset_debug(1,p->yyscanner);
#endif
}

PythonOutlineParser::~PythonOutlineParser()
{
  pyscannerYYlex_destroy(p->yyscanner);
}


void PythonOutlineParser::parseInput(const char *fileName,
                                       const char *fileBuf,
                                       const std::shared_ptr<Entry> &root,
                                       ClangTUParser * /*clangParser*/)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->thisParser = this;
  printlex(yy_flex_debug, TRUE, __FILE__, fileName);
  ::parseMain(p->yyscanner, fileName,fileBuf,root);
  printlex(yy_flex_debug, FALSE, __FILE__, fileName);

  // May print the AST for debugging purposes
  // printAST(global_root);
}

bool PythonOutlineParser::needsPreprocessing(const QCString &) const
{
  return FALSE;
}

void PythonOutlineParser::parsePrototype(const char *text)
{
  ::parsePrototype(p->yyscanner,text);

}

//----------------------------------------------------------------------------

#if USE_STATE2STRING
#include "pyscanner.l.h"
#endif
