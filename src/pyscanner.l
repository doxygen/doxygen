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
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

/*
 *      includes
 */

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

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
#include "debug.h"
#include "stringutil.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define unput_string(yytext,yyleng) do { for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]); } while(0)

/* ----------------------------------------------------------------- */

struct pyscannerYY_state
{
  CommentScanner          commentScanner;
  OutlineParserInterface *thisParser = nullptr;
  const char *            inputString = nullptr;
  int                     inputPosition = 0;
  Protection              protection = Protection::Public;
  std::shared_ptr<Entry>  current_root;
  std::shared_ptr<Entry>  current;
  std::shared_ptr<Entry>  previous;
  std::shared_ptr<Entry>  bodyEntry;
  int                     yyLineNr = 1 ;
  QCString                fileName;
  MethodTypes             mtype = MethodTypes::Method;
  bool                    isStatic = FALSE;
  Specifier               virt = Specifier::Normal;
  int                     docBlockContext = 0;
  QCString                docBlock;
  bool                    docBlockInBody = FALSE;
  bool                    docBlockJavaStyle = FALSE;
  bool                    docBrief = FALSE;
  bool                    docBlockSpecial = FALSE;
  bool                    doubleQuote = FALSE;
  bool                    specialBlock = FALSE;
  int                     stringContext = 0;
  TextStream *            copyString = nullptr;
  int                     indent = 0;
  int                     curIndent = 0;
  int                     commentIndent = 0;
  bool                    importTuple = FALSE;
  StringUnorderedMap      packageNameCache;
  char                    atomStart = 0;
  char                    atomEnd = 0;
  int                     atomCount = 0;
  int                     atomContext = 0;
  QCString                moduleScope;
  QCString                packageName;
  TextStream              defVal;
  int                     braceCount = 0;
  bool                    lexInit = FALSE;
  bool                    packageCommentAllowed = FALSE;
  bool                    startInit = FALSE;
  int                     searchCount = 0;
  QCString                argType;
  bool                    funcParamsEnd = FALSE;
  std::vector<QCString>   decorators;
  QCString                programStr;
  TextStream              decoratorCommentStr;
  int                     decoratorRound = 0;
  bool                    checkDupEntry = false;
};

//-----------------------------------------------------------------------------
[[maybe_unused]] static const char *stateToString(int state);

static inline int computeIndent(const char *s);

static void initParser(yyscan_t yyscanner);
static void initEntry(yyscan_t yyscanner);
static void newEntry(yyscan_t yyscanner);
static void addEntry(yyscan_t yyscanner);
static void docVariable(yyscan_t yyscanner,const char *s);
static void newVariable(yyscan_t yyscanner);
static void addVariable(yyscan_t yyscanner);
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
static QCString findPackageScope(yyscan_t yyscanner,const QCString &fileName);
static void setProtection(yyscan_t yyscanner);

static int yyread(yyscan_t yyscanner,char *buf,int max_size);

//-----------------------------------------------------------------------------
/* ----------------------------------------------------------------- */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

       /* start command character */

CMD               ("\\"|"@")
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
LONGSTRINGBLOCK   ({LONGSTRINGITEM}|{SMALLQUOTE})

SHORTSTRING       ("'"{SHORTSTRINGITEM}*"'"|'"'{SHORTSTRINGITEM}*'"')
SHORTSTRINGITEM   ({SHORTSTRINGCHAR}|{ESCAPESEQ})
SHORTSTRINGCHAR   [^\\\n"]
STRINGLITERAL     {STRINGPREFIX}?( {SHORTSTRING} | {LONGSTRING})
STRINGPREFIX      ("r"|"u"|"ur"|"R"|"U"|"UR"|"Ur"|"uR")
FLOWKW            ("or"|"and"|"is"|"not"|"print"|"for"|"in"|"try"|"except"|"yield"|"raise"|"break"|"continue"|"pass"|"if"|"return"|"while"|"elif"|"else"|"finally")
POUNDCOMMENT      "#"[^#\n][^\n]*
SCRIPTCOMMENT      "#!".*

STARTDOCSYMS      "##"

LINENR            {B}*[1-9][0-9]*
FILEICHAR         [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+=&#@]
FILEECHAR         [a-z_A-Z0-9\x80-\xFF\-\+=&#@]
FILECHARS         {FILEICHAR}*{FILEECHAR}+
HFILEMASK         {FILEICHAR}*("."{FILEICHAR}+)+{FILECHARS}*
VFILEMASK         {FILECHARS}("."{FILECHARS})*
FILEMASK          {VFILEMASK}|{HFILEMASK}

IDSYM     [a-z_A-Z0-9]
ID        [a-z_A-Z%]+{IDSYM}*

%option noyywrap

  /* Main start state */

%x Search
%x SearchMemVars
%x SearchSkipValue

  /* Mid-comment states */

  /* %x FuncDoubleComment */
  /* %x ClassDoubleComment */
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

  /* TypeHints */
%x TypeHint

  /* Decorator */
%x Decorator

%%

  /* ------------ Function recognition rules -------------- */

<Search>{

    ^{B}"def"{BB}       { // start of a function/method definition with indent
                          DBG_CTX((stderr,"Found def at %d\n",yyextra->yyLineNr));
                          yyextra->indent=computeIndent(yytext);
                          searchFoundDef(yyscanner);
                          BEGIN( FunctionDec );
                        }
    ^{B}"async"{BB}"def"{BB} { // start of an async function/method definition with indent
                          DBG_CTX((stderr,"Found async def at %d\n",yyextra->yyLineNr));
                          yyextra->indent=computeIndent(yytext);
                          searchFoundDef(yyscanner);
                          BEGIN( FunctionDec );
                        }
    "def"{BB}           { // start of a function/method definition
                          searchFoundDef(yyscanner);
                          BEGIN( FunctionDec );
                        }
    "async"{BB}"def"{BB} { // start of a function/method definition
                          searchFoundDef(yyscanner);
                          BEGIN( FunctionDec );
                        }

     ^{B}"class"{BB}    { // start of a class definition with indent
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
     "from"{BB}        { // start of an from import
                          yyextra->packageCommentAllowed = FALSE;
                          BEGIN( FromMod );
                       }

     ^{B}"import"{BB}  |
     "import"{BB}      { // start of an import statement
                          yyextra->packageCommentAllowed = FALSE;
                          BEGIN( Import );
                       }
     ^{B}{IDENTIFIER}/{B}"="{B}"property" { // property
                        yyextra->current->section   = EntryType::makeVariable();
                        yyextra->current->mtype     = MethodTypes::Property;
                        yyextra->current->name      = QCString(yytext).stripWhiteSpace();
                        yyextra->current->fileName  = yyextra->fileName;
                        yyextra->current->startLine = yyextra->yyLineNr;
                        yyextra->current->bodyLine  = yyextra->yyLineNr;
                        yyextra->packageCommentAllowed = FALSE;
                        BEGIN(VariableDec);
                      }
     ^{B}{IDENTIFIER}/{B}"="[^=] { // variable
                        if (yyextra->searchCount>0) REJECT;
                        yyextra->indent=computeIndent(yytext);
                        yyextra->current->section   = EntryType::makeVariable();
                        yyextra->current->name      = QCString(yytext).stripWhiteSpace();
                        yyextra->current->fileName  = yyextra->fileName;
                        yyextra->current->startLine = yyextra->yyLineNr;
                        yyextra->current->bodyLine  = yyextra->yyLineNr;
                        yyextra->packageCommentAllowed = FALSE;
                        BEGIN(VariableDec);
                      }
     ^{B}{IDENTIFIER}/{B}":" { // variable
                        if (yyextra->searchCount>0) REJECT;
                        QCString id = QCString(yytext).stripWhiteSpace();
                        if (id =="try" || id == "else" || id == "except" || id == "finally") REJECT;
                        yyextra->indent=computeIndent(yytext);
                        yyextra->current->section   = EntryType::makeVariable();
                        yyextra->current->name      = id;
                        yyextra->current->fileName  = yyextra->fileName;
                        yyextra->current->startLine = yyextra->yyLineNr;
                        yyextra->current->bodyLine  = yyextra->yyLineNr;
                        yyextra->packageCommentAllowed = FALSE;
                        BEGIN(VariableDec);
                      }
     {B}{IDENTIFIER}/({B},{B}{IDENTIFIER})*{B}")"*{B}"="[^=] { // list of variables, we cannot place the default value
                                                               // so we will skip it later on in a general rule
                                                               // Also note ")" this is to catch also (a,b). the "("
                                                               // is caught in the rule: [(], the ")" will be handled in [)]
                        if (yyextra->searchCount>1) REJECT;
                        yyextra->indent=computeIndent(yytext);
                        yyextra->current->section   = EntryType::makeVariable();
                        yyextra->current->name      = QCString(yytext).stripWhiteSpace();
                        yyextra->current->fileName  = yyextra->fileName;
                        yyextra->current->startLine = yyextra->yyLineNr;
                        yyextra->current->bodyLine  = yyextra->yyLineNr;
                        yyextra->packageCommentAllowed = FALSE;
                        addVariable(yyscanner);
                      }
     "'"              { // start of a single quoted string
                        yyextra->stringContext=YY_START;
                        yyextra->copyString=nullptr;
                        yyextra->packageCommentAllowed = FALSE;
                        BEGIN( SingleQuoteString );
                      }
     "\""             { // start of a double quoted string
                        yyextra->stringContext=YY_START;
                        yyextra->copyString=nullptr;
                        yyextra->packageCommentAllowed = FALSE;
                        BEGIN( DoubleQuoteString );
                      }
    "@staticmethod"   {
                        yyextra->isStatic=TRUE;
                      }
    "@"{SCOPE}"("     { // decorator
                        lineCount(yyscanner);
                        yyextra->decoratorRound = 1;
                        yyextra->copyString=nullptr;
                        BEGIN( Decorator );
                      }
    "@"{SCOPE}        { // decorator
                        lineCount(yyscanner);
                      }
    {SCRIPTCOMMENT}   { // Unix type script comment
                        if (yyextra->yyLineNr != 1) REJECT;
                      }
    {POUNDCOMMENT}    { // normal comment
                        // issue 9672
                        //yyextra->packageCommentAllowed = FALSE;
                      }
    {IDENTIFIER}      { // some other identifier
                        yyextra->packageCommentAllowed = FALSE;
                      }
    ^{BB}             {
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

    {B}{STARTDOCSYMS}/[^#]    {  // start of a special comment
                        yyextra->curIndent=computeIndent(yytext);
                        yyextra->packageCommentAllowed = FALSE;
                        initSpecialBlock(yyscanner);
                        BEGIN(SpecialComment);
                      }
    [(]               { // we have to do something with (
                        yyextra->searchCount++;
                      }
    [)]               { // we have to do something with )
                        if (yyextra->searchCount>0)
                        {
                          yyextra->searchCount--;
                        }
                      }
    "="               {
                        yyextra->current->doc.clear();
                        yyextra->current->brief.clear();
                      }
    {IDENTIFIER}      {
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
  "import"{B}         {
                        BEGIN(FromModItem);
                      }
  \n                  {
                        incLineNr(yyscanner);
                        BEGIN(Search);
                      }
  {B}                 {
                      }
  .                   {
                        unput(*yytext);
                        BEGIN(Search);
                      }
}

<FromModItem>{
  "*"           { // import all
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
  \n            {
                  incLineNr(yyscanner);
                  if (!yyextra->importTuple)
                  {
                    BEGIN(Search);
                  }
                }
  {B}           {
                }
  "("           {
                  yyextra->importTuple=TRUE;
                }
  ")"           {
                  yyextra->importTuple=FALSE;
                  BEGIN(Search);
                }
  ","           {
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
                        yyextra->current->fileName = yyextra->fileName;
                        //printf("Adding using declaration: found:%s:%d name=%s\n",qPrint(yyextra->fileName),yyextra->yyLineNr,qPrint(yyextra->current->name));
                        yyextra->current->section=EntryType::makeUsingDecl();
                        yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                        initEntry(yyscanner);
                        BEGIN(Search);
                      }
  \n            {
                  incLineNr(yyscanner);
                  BEGIN(Search);
                }
  {B}           {
                }
  .             {
                  unput(*yytext);
                  BEGIN(Search);
                }
}

<SearchMemVars>{
    ("cls"|"self")"."{IDENTIFIER}/{B}[,)] {
                        const char *s = strchr(yytext,'.'); s++;
                        DBG_CTX((stderr,"Found instance method variable %s in %s at %d\n",s,qPrint(yyextra->current_root->name.data(),yyextra->yyLineNr)));
                        docVariable(yyscanner,s);
                        addEntry(yyscanner);
                      }
    ("cls"|"self")"."{IDENTIFIER}/{B}"=" {
                        const char *s = strchr(yytext,'.'); s++;
                        DBG_CTX((stderr,"Found instance method variable %s in %s at %d\n",s,qPrint(yyextra->current_root->name.data(),yyextra->yyLineNr)));
                        docVariable(yyscanner,s);
                        BEGIN( SearchSkipValue  );
                      }
    ("cls"|"self")"."{IDENTIFIER}/{B}":" { // type hint
                        const char *s = strchr(yytext,'.'); s++;
                        DBG_CTX((stderr,"Found instance method variable %s in %s at %d\n",s,qPrint(yyextra->current_root->name.data(),yyextra->yyLineNr)));
                        docVariable(yyscanner,s);
                        BEGIN(TypeHint);
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
    "'"               { // start of a single quoted string
                        yyextra->stringContext=YY_START;
                        yyextra->copyString=nullptr;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
                        yyextra->stringContext=YY_START;
                        yyextra->copyString=nullptr;
                        BEGIN( DoubleQuoteString );
                      }
    \n                { incLineNr(yyscanner); }
    "="               {
                        yyextra->current->doc.clear();
                        yyextra->current->brief.clear();
                        unput(*yytext);
                        BEGIN( SearchSkipValue  );
                      }
    {IDENTIFIER}      // identifiers
    [^'"\.#a-z_A-Z=\n]+  // other uninteresting stuff
    .                 // anything else
}

<TypeHint>{
    ":"                 { // skip over start of type hint
                          yyextra->braceCount=0;
                        }
    "("|"["|"{"         {
                          yyextra->current->type+=*yytext;
                          yyextra->braceCount++;
                        }
    ")"|"]"|"}"         {
                          yyextra->current->type+=*yytext;
                          yyextra->braceCount--;
                        }
    "'"                 { // start of a single quoted string
                          yyextra->stringContext=YY_START;
                          yyextra->copyString=nullptr;
                          BEGIN( SingleQuoteString );
                        }
    "\""                { // start of a double quoted string
                          yyextra->stringContext=YY_START;
                          yyextra->copyString=nullptr;
                          BEGIN( DoubleQuoteString );
                        }
    "="                 {
                          if (yyextra->braceCount==0)
                          { // end of the type hint
                            yyextra->current->type = yyextra->current->type.stripWhiteSpace();
                            unput(*yytext);
                            BEGIN(SearchSkipValue);
                          }
                          else
                          {
                            yyextra->current->type+=*yytext;
                          }
                        }
    \n                  { // end of the type hint
                          yyextra->current->type = yyextra->current->type.stripWhiteSpace();
                          incLineNr(yyscanner);
                          newEntry(yyscanner);
                          BEGIN(SearchMemVars);
                        }
   "\\\n"               {
                          yyextra->current->type+=' ';
                          incLineNr(yyscanner);
                        }
    .                   {
                          yyextra->current->type+=*yytext;
                        }
}

<SearchSkipValue,VariableDec>{
   "="                { // the assignment operator
                        //printf("====== VariableDec at line %d\n",yyextra->yyLineNr);
                        yyextra->startInit = TRUE;
                        yyextra->current->initializer.str(yytext);
                        yyextra->current->initializer << " ";
                      }
   {B}                { // spaces
                        yyextra->current->initializer << yytext;
                      }

   {INTNUMBER}        { // integer value
                        if (yyextra->current->type.isEmpty()) yyextra->current->type = "int";
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
                        if (yyextra->current->type.isEmpty()) yyextra->current->type = "str";
                        yyextra->current->initializer << yytext;
                        yyextra->copyString=&yyextra->current->initializer;
                        yyextra->stringContext=YY_START;
                        BEGIN( SingleQuoteString );
                      }
   {STRINGPREFIX}?"\"" { // string
                        if (yyextra->current->type.isEmpty()) yyextra->current->type = "str";
                        yyextra->current->initializer << yytext;
                        yyextra->copyString=&yyextra->current->initializer;
                        yyextra->stringContext=YY_START;
                        BEGIN( DoubleQuoteString );
                      }
   {TRIDOUBLEQUOTE}   { // start of a comment block
                        if (yyextra->current->type.isEmpty()) yyextra->current->type = "str";
                        yyextra->current->initializer << yytext;
                        yyextra->doubleQuote=TRUE;
                        yyextra->copyString=&yyextra->current->initializer;
                        yyextra->stringContext=YY_START;
                        BEGIN(TripleString);
                      }

   {TRISINGLEQUOTE}   { // start of a comment block
                        if (yyextra->current->type.isEmpty()) yyextra->current->type = "str";
                        yyextra->current->initializer << yytext;
                        yyextra->doubleQuote=FALSE;
                        yyextra->copyString=&yyextra->current->initializer;
                        yyextra->stringContext=YY_START;
                        BEGIN(TripleString);
                      }
   "("                { // tuple, only when direct after =
                        if (yyextra->current->mtype!=MethodTypes::Property && yyextra->startInit)
                        {
                          yyextra->current->type = "tuple";
                        }
                        yyextra->current->initializer << *yytext;
                        yyextra->atomStart='(';
                        yyextra->atomEnd=')';
                        yyextra->atomCount=1;
                        yyextra->atomContext=YY_START;
                        BEGIN( VariableAtom );
                      }
   "["                { // list
                        if (yyextra->startInit) yyextra->current->type = "list";
                        yyextra->current->initializer << *yytext;
                        yyextra->atomStart='[';
                        yyextra->atomEnd=']';
                        yyextra->atomCount=1;
                        yyextra->atomContext=YY_START;
                        BEGIN( VariableAtom );
                      }
   "{"                { // dictionary
                        if (yyextra->startInit) yyextra->current->type = "dict";
                        yyextra->current->initializer << *yytext;
                        yyextra->atomStart='{';
                        yyextra->atomEnd='}';
                        yyextra->atomCount=1;
                        yyextra->atomContext=YY_START;
                        BEGIN( VariableAtom );
                      }
   "\\\n"             {
                        yyextra->current->initializer << yytext;
                        incLineNr(yyscanner);
                      }
   {IDENTIFIER}       {
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
                        yyextra->startInit = FALSE;
                        yyextra->current->initializer << yytext;
                      }
   .                  {
                        yyextra->startInit = FALSE;
                        yyextra->current->initializer << *yytext;
                      }
}
<SearchSkipValue>{
    {STARTDOCSYMS}/[^#]    {  // start of a special comment
                        initSpecialBlock(yyscanner);
                        BEGIN(SpecialComment);
                      }
    {POUNDCOMMENT}    { // #
                      }
    \n                { incLineNr(yyscanner);
                        newEntry(yyscanner);
                        BEGIN(SearchMemVars);
                      }
}

<FunctionBody>{
    \n{B}/{IDENTIFIER}[^{LETTER}{DIGIT}_]  {
                        DBG_CTX((stderr,"indent %d<=%d\n",computeIndent(&yytext[1]),yyextra->indent));
                        if (computeIndent(&yytext[1])<=yyextra->indent)
                        {
                          unput_string(yytext,yyleng);
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
    \n{B}/"##"        {
                        if (computeIndent(&yytext[1])<=yyextra->indent)
                        {
                          unput_string(yytext,yyleng);
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
    <<EOF>>           {
                        endOfDef(yyscanner);
                        yyterminate();
                      }
    ^{BB}/\n          { // skip empty line
                        yyextra->current->program << yytext;
                      }
    ^{BB}             { // something at indent >0
                        yyextra->current->program << yytext;
                        yyextra->curIndent = computeIndent(yytext);
                        if (yyextra->curIndent<=yyextra->indent)
                          // jumped out of the function
                        {
                          endOfDef(yyscanner,1);
                          BEGIN(Search);
                        }
                      }
    "'"               { // start of a single quoted string
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
    {NEWLINE}         {
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

}

<FunctionDec>{
    {IDENTIFIER}            {
                              //found function name
                              yyextra->current->name = yytext;
                              yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                              newFunction(yyscanner);
                            }
    {B}":"{B}               { // function without arguments
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
    {BB}                {
                        }

    ","                 {
                          if (!yyextra->argType.isEmpty())
                          {
                            Argument a;
                            a.name = "";
                            a.type = yyextra->argType;
                            yyextra->current->argList.push_back(a);
                            yyextra->argType = "";
                          }
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
    "="                 { // default value
                          // TODO: this rule is too simple, need to be able to
                          // match things like =")" as well!
                          yyextra->defVal.str(std::string());
                          yyextra->braceCount = 0;
                          BEGIN(FunctionParamDefVal);
                        }
     ")"                {
                          if (!yyextra->argType.isEmpty())
                          {
                            Argument a;
                            a.name = "";
                            a.type = yyextra->argType;
                            yyextra->current->argList.push_back(a);
                            yyextra->argType = "";
                          }
                          unput(*yytext);
                          BEGIN(FunctionDec);
                        }
     ":"{B}             {
                          yyextra->defVal.str(std::string());
                          yyextra->braceCount = 0;
                          BEGIN(FunctionAnnotation);
                        }
    {POUNDCOMMENT}      { // a comment
                        }
    {PARAMNONEMPTY}     { // Default rule inside arguments.
                        }

}

<FunctionTypeAnnotation>{
     "{"                |
     "["                |
     "("                {
                          ++yyextra->braceCount;
                          yyextra->defVal << *yytext;
                        }
     "}"                |
     "]"                |
     ")"                {
                          --yyextra->braceCount;
                          yyextra->defVal << *yytext;
                        }
     ":"                {
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
     .                  {
                          yyextra->defVal << *yytext;
                        }
}

<FunctionAnnotation>{
     "{"                |
     "["                |
     "("                {
                          ++yyextra->braceCount;
                          yyextra->defVal << *yytext;
                        }
     "}"                |
     "]"                {
                          --yyextra->braceCount;
                          yyextra->defVal << *yytext;
                        }
     ")"                |
     "="                |
     ","                {
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
     .                  {
                          yyextra->defVal << *yytext;
                        }
}

<FunctionParamDefVal>{
     "{"                |
     "["                |
     "("                { // internal opening brace, assumption is that we have correct code so braces do match
                          ++yyextra->braceCount;
                          yyextra->defVal << *yytext;
                        }
     "}"                |
     "]"                {
                          --yyextra->braceCount;
                          yyextra->defVal << *yytext;
                        }
     ")"                |
     ","                {
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
     .                  {
                            yyextra->defVal << *yytext;
                        }
}


<ClassBody>{
    \n/{IDENTIFIER}{BB}  { // new def at indent 0
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
    \n/"##"[^#]       {  // start of a special comment at indent 0
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
    ^{BB}/\n          { // skip empty line
                        yyextra->current->program << yytext;
                      }
    <<EOF>>           {
                        endOfDef(yyscanner);
                        yyterminate();
                      }
    ^{BB}             { // something at indent >0
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
    "'"               { // start of a single quoted string
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
    {NEWLINE}         {
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

                          yyextra->current->section = EntryType::makeClass();
                          yyextra->current->name = yytext;
                          // we need to set the protectiion based on the "local" class name
                          setProtection(yyscanner);

                          // prepend scope in case of nested classes
                          if (yyextra->current_root->section.isScope())
                          {
                            //printf("*** Prepending scope %s to class %s\n",qPrint(yyextra->current_root->name),qPrint(yyextra->current->name));
                            yyextra->current->name.prepend(yyextra->current_root->name+"::");
                          }

                          yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                          yyextra->current->fileName = yyextra->fileName;
                          yyextra->docBlockContext   = YY_START;
                          yyextra->docBlockInBody    = FALSE;
                          yyextra->docBlockJavaStyle = FALSE;
                          yyextra->docBlock.clear();

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
                         yyextra->current->extends.emplace_back(
                                              substitute(yytext,".","::"),Protection::Public,Specifier::Normal
                                            );
                         //Has base class-do stuff
                       }
    "'"                { // start of a single quoted string
                         yyextra->stringContext=YY_START;
                         BEGIN( SingleQuoteStringIgnore );
                       }
    "\""               { // start of a double quoted string
                         yyextra->stringContext=YY_START;
                         BEGIN( DoubleQuoteStringIgnore );
                       }
}

<SingleQuoteStringIgnore>{
    "'"                { // end of a single quoted string
                         BEGIN(yyextra->stringContext);
                       }
    .                  { }
}
<DoubleQuoteStringIgnore>{
    "\""               { // end of a double quoted string
                         BEGIN(yyextra->stringContext);
                       }
    .                  { }
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
    ^{BB}                      {
                                 yyextra->current->program << yytext;
                                 //yyextra->current->startLine = yyextra->yyLineNr;
                                 yyextra->curIndent=computeIndent(yytext);
                                 yyextra->bodyEntry = yyextra->current;
                                 DBG_CTX((stderr,"setting indent %d\n",yyextra->curIndent));
                                 //printf("yyextra->current->program=[%s]\n",qPrint(yyextra->current->program));
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
   ":"{B}{IDENTIFIER} { //typing
                        yyextra->startInit = FALSE;
                        yyextra->current->type = substitute(yytext,":","");
                      }
    {STARTDOCSYMS}"<"/.* {  // start of a special comment
                        yyextra->curIndent=computeIndent(yytext);
                        yyextra->packageCommentAllowed = FALSE;
                        initSpecialBlock(yyscanner);
                        yyextra->docBlockContext = VariableEnd;
                        BEGIN(SpecialComment);
                      }
   "#".*              { // comment
                        BEGIN( VariableEnd );
                      }
   \n                 {
                        unput('\n');
                        BEGIN( VariableEnd );
                      }
}

<VariableAtom>{
    [\(\[\{]          {
                        yyextra->current->initializer << *yytext;
                        if (yyextra->atomStart==*yytext)
                        {
                          yyextra->atomCount++;
                        }
                      }
    [\)\]\}]          {
                        yyextra->current->initializer << *yytext;
                        if (yyextra->atomEnd==*yytext)
                        {
                          yyextra->atomCount--;
                        }
                        if (yyextra->atomCount==0)
                        {
                          yyextra->startInit = FALSE;
                          BEGIN(yyextra->atomContext);
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
   "'"                {
                        yyextra->stringContext=YY_START;
                        yyextra->current->initializer << "'";
                        yyextra->copyString=&yyextra->current->initializer;
                        BEGIN( SingleQuoteString );
                      }
   "\""               {
                        yyextra->stringContext=YY_START;
                        yyextra->current->initializer << "\"";
                        yyextra->copyString=&yyextra->current->initializer;
                        BEGIN( DoubleQuoteString );
                      }
   {IDENTIFIER}       {
                        yyextra->current->initializer << yytext;
                      }
   .                  {
                        yyextra->current->initializer << *yytext;
                      }
   \n                 {
                        yyextra->current->initializer << *yytext;
                        incLineNr(yyscanner);
                      }

}

<VariableEnd>{
    \n                {
                        incLineNr(yyscanner);
                        if (!stripWhiteSpace(yyextra->current->initializer.str()).empty())
                        {
                          newVariable(yyscanner);
                        }
                        BEGIN(Search);
                      }
    .                 {
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
                                  stripIndentationVerbatim(actualDoc,yyextra->commentIndent);
                                  actualDoc.prepend("@iverbatim\n");
                                  actualDoc.append("@endiverbatim ");
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
                                  stripIndentationVerbatim(actualDoc,yyextra->commentIndent);
                                  actualDoc.prepend("@iverbatim\n");
                                  actualDoc.append("@endiverbatim ");
                                }
                              }
                              if (yyextra->moduleScope.startsWith("__") &&  yyextra->moduleScope.endsWith("__"))
                              {
                                actualDoc.prepend("\\namespace \\"+yyextra->moduleScope+" ");
                              }
                              else
                              {
                                actualDoc.prepend("\\namespace "+yyextra->moduleScope+" ");
                              }
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


    ^{BB}               { // leading whitespace, compensate for """! / '''!
                          if (yyextra->docBlockSpecial && yyleng >= yyextra->curIndent)
                            yyextra->docBlock += yytext + yyextra->curIndent;
                          else
                            yyextra->docBlock += yytext;
                        }
    [^"'\n \t\\@]+      {
                          yyextra->docBlock += yytext;
                        }
    \n                  {
                          incLineNr(yyscanner);
                          yyextra->docBlock += yytext;
                        }
    {CMD}"ifile"{B}+"\""[^\n\"]+"\"" {
                                    yyextra->fileName = &yytext[6];
                                    yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                    yyextra->fileName = yyextra->fileName.mid(1,yyextra->fileName.length()-2);
                                    yyextra->docBlock+=yytext;
                                  }
    {CMD}"ifile"{B}+{FILEMASK}    {
                                    yyextra->fileName = &yytext[6];
                                    yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                    yyextra->docBlock+=yytext;
                                  }
    {CMD}"iline"{LINENR}/[\n\.]   |
    {CMD}"iline"{LINENR}{B}       {
                                    bool ok = false;
                                    int nr = QCString(&yytext[6]).toInt(&ok);
                                    if (!ok)
                                    {
                                      warn(yyextra->fileName,yyextra->yyLineNr,"Invalid line number '%s' for iline command",yytext);
                                    }
                                    else
                                    {
                                      yyextra->yyLineNr = nr;
                                    }
                                    yyextra->docBlock+=yytext;
                                  }
    ({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
                                     yyextra->docBlock+=yytext;
                                  }
    \\.                 { // escaped char TO be extended
                          yyextra->docBlock += yytext;
                        }
    .                   {
                          yyextra->docBlock += yytext;
                        }
}

<SpecialComment>{
    ^{B}"#"("#")*       { // skip leading hashes
                        }
    \n/{B}"#"           { // continuation of the comment on the next line
                          yyextra->docBlock+='\n';
                          yyextra->docBrief = FALSE;
                          incLineNr(yyscanner);
                        }
    {CMD}"ifile"{B}+"\""[^\n\"]+"\"" {
                                    yyextra->fileName = &yytext[6];
                                    yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                    yyextra->fileName = yyextra->fileName.mid(1,yyextra->fileName.length()-2);
                                    yyextra->docBlock+=yytext;
                                  }
    {CMD}"ifile"{B}+{FILEMASK}    {
                                    yyextra->fileName = &yytext[6];
                                    yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                    yyextra->docBlock+=yytext;
                                  }
    {CMD}"iline"{LINENR}/[\n\.]   |
    {CMD}"iline"{LINENR}{B}       {
                                    bool ok = false;
                                    int nr = QCString(&yytext[6]).toInt(&ok);
                                    if (!ok)
                                    {
                                      warn(yyextra->fileName,yyextra->yyLineNr,"Invalid line number '%s' for iline command",yytext);
                                    }
                                    else
                                    {
                                      yyextra->yyLineNr = nr;
                                    }
                                    yyextra->docBlock+=yytext;
                                  }
    ({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
                                     yyextra->docBlock+=yytext;
                                  }
    "\\ilinebr "{B}*    {
                          QCString indent;
                          int extraSpaces = std::max(0,static_cast<int>(yyleng-9-yyextra->curIndent-2));
                          indent.fill(' ',extraSpaces);
                          //printf("extraSpaces=%d\n",extraSpaces);
                          yyextra->docBlock += "\\ilinebr ";
                          yyextra->docBlock += indent;
                        }
    [^#\\@\n]+          { // any other stuff
                          yyextra->docBlock+=yytext;
                        }
    \n                  { // new line that ends the comment
                          handleCommentBlock(yyscanner, yyextra->docBlock, yyextra->docBrief);
                          if (yyextra->docBlockContext == VariableEnd)
                          {
                            unput(*yytext);
                          }
                          else
                          {
                            incLineNr(yyscanner);
                          }
                          BEGIN(yyextra->docBlockContext);
                        }
    .                   { // anything we missed
                          yyextra->docBlock+=*yytext;
                        }
}

<SingleQuoteString>{
    \\{B}\n                    { // line continuation
                                 addToString(yyscanner,yytext);
                                 incLineNr(yyscanner);
                               }
    \\.                        { // escaped char
                                 addToString(yyscanner,yytext);
                               }
    "\"\"\""                   { // triple double quotes
                                 addToString(yyscanner,yytext);
                               }
    "'"                        { // end of the string
                                 addToString(yyscanner,yytext);
                                 BEGIN(yyextra->stringContext);
                               }
    [^"'\n\\]+                 { // normal chars
                                 addToString(yyscanner,yytext);
                               }
    .                          { // normal char
                                 addToString(yyscanner,yytext);
                               }
}

<DoubleQuoteString>{
    \\{B}\n                    { // line continuation
                                 addToString(yyscanner,yytext);
                                 incLineNr(yyscanner);
                               }
    \\.                        { // escaped char
                                 addToString(yyscanner,yytext);
                               }
    "'''"                      { // triple single quotes
                                 addToString(yyscanner,yytext);
                               }
    "\""                       { // end of the string
                                 addToString(yyscanner,yytext);
                                 BEGIN(yyextra->stringContext);
                               }
    [^"'\n\\]+                 { // normal chars
                                 addToString(yyscanner,yytext);
                               }
    .                          { // normal char
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
    \n                  {
                          incLineNr(yyscanner);
                          *yyextra->copyString << yytext;
                        }
    .                   {
                          *yyextra->copyString << *yytext;
                        }
}

<Decorator>{
   {TRIDOUBLEQUOTE}     { // start of a comment block
                          yyextra->doubleQuote=TRUE;
                          yyextra->decoratorCommentStr.str(std::string());
                          yyextra->copyString=&yyextra->decoratorCommentStr;
                          yyextra->stringContext=YY_START;
                          BEGIN(TripleString);
                        }

   {TRISINGLEQUOTE}     { // start of a comment block
                          yyextra->doubleQuote=FALSE;
                          yyextra->decoratorCommentStr.str(std::string());
                          yyextra->copyString=&yyextra->decoratorCommentStr;
                          yyextra->stringContext=YY_START;
                          BEGIN(TripleString);
                        }
   "'"                  {
                          yyextra->stringContext=YY_START;
                          yyextra->decoratorCommentStr.str(std::string());
                          yyextra->copyString=&yyextra->decoratorCommentStr;
                          BEGIN( SingleQuoteString );
                        }
   "\""                 {
                          yyextra->stringContext=YY_START;
                          yyextra->decoratorCommentStr.str(std::string());
                          yyextra->copyString=&yyextra->decoratorCommentStr;
                          BEGIN( DoubleQuoteString );
                        }
    "("                 {
                          yyextra->decoratorRound++;
                        }
    ")"                 {
                          yyextra->decoratorRound--;
                          if (!yyextra->decoratorRound) BEGIN( Search );
                        }
    \n                  {
                          incLineNr(yyscanner);
                        }
    .                   { }
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

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  const char *p = yyextra->inputString + yyextra->inputPosition;
  while ( c < max_size && *p ) { *buf++ = *p++; c++; }
  yyextra->inputPosition+=c;
  return c;
}

static void initParser(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->protection = Protection::Public;
  yyextra->mtype = MethodTypes::Method;
  yyextra->isStatic = FALSE;
  yyextra->virt = Specifier::Normal;
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
  yyextra->current->isStatic   = yyextra->isStatic;
  yyextra->current->lang       = SrcLangExt::Python;
  yyextra->commentScanner.initGroupInfo(yyextra->current.get());
  yyextra->isStatic = FALSE;
}

static void newEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool found = false;
  if (yyextra->checkDupEntry)
  {
    for (auto &v : yyextra->current_root->children())
    {
      //printf("candidate %s section=%s!\n",qPrint(v->name),v->section.to_string().c_str());
      if (v->name==yyextra->current->name && v->section==yyextra->current->section) // name is already added, don't add it again
      {
        if (v->doc.isEmpty() && !yyextra->current->doc.isEmpty())
        {
          v->doc       = yyextra->current->doc;
          v->docLine   = yyextra->current->docLine;
          v->docFile   = yyextra->current->docFile;
        }
        if (v->brief.isEmpty() && !yyextra->current->brief.isEmpty())
        {
          v->brief     = yyextra->current->brief;
          v->briefLine = yyextra->current->briefLine;
          v->briefFile = yyextra->current->briefFile;
        }
        if (v->type.isEmpty() && !yyextra->current->type.isEmpty())
        {
          v->type      = yyextra->current->type;
        }
        found=true;
      }
    }
    yyextra->checkDupEntry=false;
  }
  if (!found)
  {
    yyextra->previous = yyextra->current;
    yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  }
  initEntry(yyscanner);
}

static void addEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  auto doc        = yyextra->current->doc;
  auto docLine    = yyextra->current->docLine;
  auto docFile    = yyextra->current->docFile;
  auto brief      = yyextra->current->brief;
  auto briefLine  = yyextra->current->briefLine;
  auto briefFile  = yyextra->current->briefFile;

  yyextra->previous = yyextra->current;
  yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  initEntry(yyscanner);

  yyextra->current->doc        = doc;
  yyextra->current->docLine    = docLine;
  yyextra->current->docFile    = docFile;
  yyextra->current->brief      = brief;
  yyextra->current->briefLine  = briefLine;
  yyextra->current->briefFile  = briefFile;
}

static void setProtection(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->current->name.isEmpty() && yyextra->current->name.at(0)=='_')
  {
    if (yyextra->current->name.at(1)=='_') // mark as private
    {
      yyextra->current->protection=Protection::Private;
    }
    else // mark as protected
    {
      yyextra->current->protection=Protection::Protected;
    }
  }
}

static void docVariable(yyscan_t yyscanner,const char *name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->name = name;
  yyextra->current->section=EntryType::makeVariable();
  yyextra->current->fileName  = yyextra->fileName;
  yyextra->current->startLine = yyextra->yyLineNr;
  yyextra->current->bodyLine  = yyextra->yyLineNr;
  yyextra->current->type.clear();
  setProtection(yyscanner);
  yyextra->checkDupEntry = true;
}

static void newVariable(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  setProtection(yyscanner);
  if (yyextra->current_root->section.isCompound()) // mark as class variable
  {
    yyextra->current->isStatic = TRUE;
  }
  newEntry(yyscanner);
}

static void addVariable(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  setProtection(yyscanner);
  if (yyextra->current_root->section.isCompound()) // mark as class variable
  {
    yyextra->current->isStatic = TRUE;
  }
  addEntry(yyscanner);
}

static void newFunction(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current->name.startsWith("__") && yyextra->current->name.endsWith("__"))
  {
    // special method name, see
    // http://docs.python.org/ref/specialnames.html
    yyextra->current->protection=Protection::Public;
  }
  else
  {
    setProtection(yyscanner);
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
    return QCString(it->second);
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
      yyextra->packageNameCache.emplace(path.str(),scope.str());
      return scope;
    }
  }
  return "";
}

static QCString findPackageScope(yyscan_t yyscanner,const QCString &fileName)
{
  if (fileName.isEmpty()) return fileName;
  FileInfo fi(fileName.str());
  return findPackageScopeFromPath(yyscanner,fi.dirPath(true).c_str());
}

static void addFrom(yyscan_t yyscanner,bool all)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString item=all ? yyextra->packageName : yyextra->packageName+"."+yytext;
  yyextra->current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
  yyextra->current->fileName = yyextra->fileName;
  //printf("Adding using declaration: found:%s:%d name=%s\n",qPrint(yyextra->fileName),yyextra->yyLineNr,qPrint(yyextra->current->name));
  yyextra->current->section=all ? EntryType::makeUsingDir() : EntryType::makeUsingDecl();
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
    yyextra->current->briefFile = yyextra->fileName;
    yyextra->current->briefLine = yyextra->yyLineNr;
  }
  else
  {
    yyextra->current->docFile = yyextra->fileName;
    yyextra->current->docLine = yyextra->yyLineNr;
  }
}

static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("handleCommentBlock(doc=[%s] brief=%d yyextra->docBlockInBody=%d yyextra->docBlockJavaStyle=%d\n",
  //    qPrint(doc),brief,yyextra->docBlockInBody,yyextra->docBlockJavaStyle);

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
  Markdown markdown(yyextra->fileName,lineNr);
  GuardedSectionStack guards;
  QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(doc,lineNr) : doc;
  while (yyextra->commentScanner.parseCommentBlock(
        yyextra->thisParser,
        (yyextra->docBlockInBody && yyextra->previous) ? yyextra->previous.get() : yyextra->current.get(),
        processedDoc, // text
        yyextra->fileName,   // file
        lineNr,
        yyextra->docBlockInBody ? FALSE : brief,
        yyextra->docBlockJavaStyle, // javadoc style // or FALSE,
        yyextra->docBlockInBody,
        yyextra->protection,
        position,
        needsEntry,
        Config_getBool(MARKDOWN_SUPPORT),
        &guards
       )
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
  yyextra->docBlock.clear();
  yyextra->commentIndent = yyextra->curIndent;
  yyextra->doubleQuote = TRUE;
  if (yyextra->docBlockSpecial)
  {
    yyextra->docBlock.fill(' ',yyextra->indent);
  }
  startCommentBlock(yyscanner,FALSE);
}

static void initTriSingleQuoteBlock(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->docBlockContext   = YY_START;
  yyextra->docBlockInBody    = FALSE;
  yyextra->docBlockJavaStyle = TRUE;
  yyextra->docBlockSpecial   = yytext[strlen(yytext) - 1]=='!' || !Config_getBool(PYTHON_DOCSTRING);
  yyextra->docBlock.clear();
  yyextra->commentIndent = yyextra->curIndent;
  yyextra->doubleQuote = FALSE;
  if (yyextra->docBlockSpecial)
  {
    yyextra->docBlock.fill(' ',yyextra->indent);
  }
  startCommentBlock(yyscanner,FALSE);
}

static void initSpecialBlock(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->docBlockContext   = YY_START;
  yyextra->docBlockInBody    = FALSE;
  yyextra->docBlockJavaStyle = TRUE;
  yyextra->docBrief = TRUE;
  yyextra->docBlock.clear();
  yyextra->commentIndent = yyextra->curIndent;
  startCommentBlock(yyscanner,FALSE);
}

static void searchFoundDef(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->fileName  = yyextra->fileName;
  yyextra->current->startLine = yyextra->yyLineNr;
  yyextra->current->bodyLine  = yyextra->yyLineNr;
  yyextra->current->section = EntryType::makeFunction();
  yyextra->current->lang = SrcLangExt::Python;
  yyextra->current->virt = Specifier::Normal;
  yyextra->current->isStatic = yyextra->isStatic;
  yyextra->current->mtype = yyextra->mtype = MethodTypes::Method;
  yyextra->current->type.clear();
  yyextra->current->name.clear();
  yyextra->current->args.clear();
  yyextra->current->argList.clear();
  yyextra->packageCommentAllowed = FALSE;
  yyextra->isStatic=FALSE;
  //printf("searchFoundDef at=%d\n",yyextra->yyLineNr);
}

static void searchFoundClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->section = EntryType::makeClass();
  yyextra->current->argList.clear();
  yyextra->current->type += "class" ;
  yyextra->current->fileName  = yyextra->fileName;
  yyextra->current->startLine  = yyextra->yyLineNr;
  yyextra->current->bodyLine  = yyextra->yyLineNr;
  yyextra->packageCommentAllowed = FALSE;
}

//----------------------------------------------------------------------------

static void parseCompounds(yyscan_t yyscanner,std::shared_ptr<Entry> rt)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("parseCompounds(%s)\n",qPrint(rt->name));
  for (size_t i=0; i<rt->children().size(); ++i)
  {
    std::shared_ptr<Entry> ce = rt->children()[i];
    if (!ce->program.empty())
    {
      //fprintf(stderr,"parseCompounds: -- %s (line %d) ---------\n%s\n---------------\n",
      //  qPrint(ce->name), ce->bodyLine, qPrint(ce->program));
      // init scanner state
      yyextra->programStr = ce->program.str();
      yyextra->inputString = yyextra->programStr.data();
      yyextra->inputPosition = 0;
      pyscannerYYrestart( nullptr, yyscanner );
      if (ce->section.isCompound())
      {
        yyextra->specialBlock = false;
        yyextra->current_root = ce;
        BEGIN( Search );
      }
      else if (ce->parent())
      {
        yyextra->current_root = rt;
        //printf("Searching for member variables in %s parent=%s\n",
        //    qPrint(ce->name),qPrint(ce->parent->name));
        BEGIN( SearchMemVars );
      }
      yyextra->fileName = ce->fileName;
      yyextra->yyLineNr   = ce->bodyLine ;
      yyextra->current = std::make_shared<Entry>();
      initEntry(yyscanner);

      QCString name = ce->name;
      yyextra->commentScanner.enterCompound(yyextra->fileName,yyextra->yyLineNr,name);

      pyscannerYYlex(yyscanner) ;
      yyextra->lexInit=TRUE;

      yyextra->programStr.clear();
      ce->program.str(std::string());

      yyextra->commentScanner.leaveCompound(yyextra->fileName,yyextra->yyLineNr,name);

    }
    parseCompounds(yyscanner,ce);
  }
}

//----------------------------------------------------------------------------


static void parseMain(yyscan_t yyscanner, const QCString &fileName,const char *fileBuf,const std::shared_ptr<Entry> &rt)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  initParser(yyscanner);

  if (fileBuf==nullptr || fileBuf[0]=='\0') return;

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;

  yyextra->protection    = Protection::Public;
  yyextra->mtype         = MethodTypes::Method;
  yyextra->isStatic      = false;
  yyextra->virt          = Specifier::Normal;
  yyextra->current_root  = rt;
  yyextra->specialBlock  = false;

  yyextra->yyLineNr= 1 ;
  yyextra->fileName = fileName;
  //setContext();
  msg("Parsing file %s...\n",qPrint(yyextra->fileName));

  FileInfo fi(fileName.str());
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

  // add namespaces for each scope
  QCString scope = yyextra->moduleScope;
  int startPos = 0;
  int pos = 0;
  do
  {
    pos = scope.find("::",startPos);
    startPos=pos+2;
    if (pos==-1) pos=(int)scope.length();
    yyextra->current            = std::make_shared<Entry>();
    initEntry(yyscanner);
    yyextra->current->name      = scope.left(pos);
    yyextra->current->section   = EntryType::makeNamespace();
    yyextra->current->type      = "namespace";
    yyextra->current->fileName  = yyextra->fileName;
    yyextra->current->startLine = yyextra->yyLineNr;
    yyextra->current->bodyLine  = yyextra->yyLineNr;
    yyextra->current_root       = yyextra->current;
    rt->moveToSubEntryAndRefresh(yyextra->current);
  } while (pos<(int)scope.length());

  initParser(yyscanner);

  yyextra->commentScanner.enterFile(yyextra->fileName,yyextra->yyLineNr);

  yyextra->current->reset();
  initEntry(yyscanner);
  pyscannerYYrestart(nullptr,yyscanner);
  BEGIN( Search );
  pyscannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->commentScanner.leaveFile(yyextra->fileName,yyextra->yyLineNr);

  yyextra->programStr.clear();
  yyextra->current_root->program.str(std::string());

  parseCompounds(yyscanner, yyextra->current_root);
}

//----------------------------------------------------------------------------

static void parsePrototype(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("**** parsePrototype(%s) begin\n",qPrint(text));
  if (text.isEmpty())
  {
    warn(yyextra->fileName,yyextra->yyLineNr,"Empty prototype found!");
    return;
  }

  yyextra->specialBlock = FALSE;
  yyextra->packageCommentAllowed = FALSE;

  // save scanner state
  YY_BUFFER_STATE orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(nullptr, YY_BUF_SIZE, yyscanner), yyscanner);
  const char *orgInputString = yyextra->inputString;
  int orgInputPosition = yyextra->inputPosition;

  // set new string
  yyextra->inputString = text.data();
  yyextra->inputPosition = 0;
  pyscannerYYrestart( nullptr, yyscanner );

  BEGIN( FunctionDec );

  pyscannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->current->name = yyextra->current->name.stripWhiteSpace();
  if (yyextra->current->section.isMemberDoc() && yyextra->current->args.isEmpty())
  {
    yyextra->current->section = EntryType::makeVariableDoc();
  }

  // restore original scanner state

  yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);
  yy_switch_to_buffer(orgState, yyscanner);

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
  pyscannerYYset_debug(Debug::isFlagSet(Debug::Lex_pyscanner)?1:0,p->yyscanner);
#endif
}

PythonOutlineParser::~PythonOutlineParser()
{
  pyscannerYYlex_destroy(p->yyscanner);
}


void PythonOutlineParser::parseInput(const QCString &fileName,
                                     const char *fileBuf,
                                     const std::shared_ptr<Entry> &root,
                                     ClangTUParser * /*clangParser*/)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->thisParser = this;
  DebugLex debugLex(Debug::Lex_pyscanner, __FILE__, qPrint(fileName));
  ::parseMain(p->yyscanner, fileName,fileBuf,root);

  // May print the AST for debugging purposes
  // printAST(global_root);
}

bool PythonOutlineParser::needsPreprocessing(const QCString &) const
{
  return FALSE;
}

void PythonOutlineParser::parsePrototype(const QCString &text)
{
  ::parsePrototype(p->yyscanner,text);
}

//----------------------------------------------------------------------------

#include "pyscanner.l.h"
