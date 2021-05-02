/******************************************************************************
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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada)
 *  in Spring 2005 as part of CS 179E: Compiler Design Project
 *  at the University of California, Riverside; the course was
 *  taught by Peter H. Froehlich <phf@acm.org>.
 */

%option never-interactive
%option prefix="pycodeYY"
%option reentrant
%option extra-type="struct pycodeYY_state *"
%option noyy_top_state
%top{
#include <stdint.h>
}

%{

#include <vector>
#include <unordered_map>
#include <string>
#include <stack>

#include <stdio.h>

#include "pycode.h"
#include "message.h"
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "tooltip.h"
#include "scopedtypevariant.h"
#include "symbolresolver.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0


struct pycodeYY_state
{
  std::unordered_map< std::string, ScopedTypeVariant > codeClassMap;
  QCString      curClassName;
  StringVector  curClassBases;

  CodeOutputInterface * code = 0;
  const char *  inputString = 0;     //!< the code fragment as text
  yy_size_t     inputPosition = 0;   //!< read offset during parsing
  const char *  currentFontClass = 0;
  bool          needsTermination = FALSE;
  const Definition *searchCtx = 0;
  bool          collectXRefs = FALSE;
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  FileDef *     sourceFileDef = 0;
  const Definition * currentDefinition = 0;
  const MemberDef *  currentMemberDef = 0;
  bool          includeCodeFragment = FALSE;
  QCString      realScope;
  int           bodyCurlyCount = 0;
  bool          searchingForBody = FALSE;
  QCString      classScope;
  int           paramParens = 0;

  bool          exampleBlock = FALSE;
  QCString      exampleName;

  QCString      type;
  QCString      name;

  bool          doubleStringIsDoc = FALSE;
  bool          doubleQuote = FALSE;
  bool          noSuiteFound = FALSE;
  int           stringContext = 0;

  std::stack<yy_size_t> indents;  //!< Tracks indentation levels for scoping in python

  QCString      docBlock;     //!< contents of all lines of a documentation block
  bool          endComment = FALSE;
  VariableContext theVarContext;
  CallContext theCallContext;
  SymbolResolver symbolResolver;
};


#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static void startCodeLine(yyscan_t yyscanner);
static int countLines(yyscan_t yyscanner);
static void setCurrentDoc(yyscan_t yyscanner, const QCString &anchor);
static void addToSearchIndex(yyscan_t yyscanner, const QCString &text);
static const ClassDef *stripClassName(yyscan_t yyscanner,const QCString &s,Definition *d);
static void codify(yyscan_t yyscanner,const QCString &text);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void writeMultiLineCodeLink(yyscan_t yyscanner, CodeOutputInterface &ol, const Definition *d, const QCString &text);
static void startFontClass(yyscan_t yyscanner,const char *s);
static void endFontClass(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static bool getLinkInScope(yyscan_t yyscanner, const QCString &c, const QCString &m,
                           const QCString &memberText, CodeOutputInterface &ol, const QCString &text);
static bool getLink(yyscan_t yyscanner, const QCString &className, const QCString &memberName,
                    CodeOutputInterface &ol, const QCString &text=QCString());
static void generateClassOrGlobalLink(yyscan_t yyscanner, CodeOutputInterface &ol,
                                      const QCString &clName, bool typeOnly=FALSE);
static void generateFunctionLink(yyscan_t yyscanner, CodeOutputInterface &ol,
                                const QCString &funcName);
static bool findMemberLink(yyscan_t yyscanner, CodeOutputInterface &ol,
                           Definition *sym, const QCString &symName);
static void findMemberLink(yyscan_t yyscanner, CodeOutputInterface &ol,
                           const QCString &symName);
static void adjustScopesAndSuites(yyscan_t yyscanner,unsigned indentLength);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);

#if 0 // TODO: call me to store local variables and get better syntax highlighting, see code.l
static void addVariable(yyscan_t yyscanner, QCString type, QCString name);
#endif

//-------------------------------------------------------------------

static std::mutex g_searchIndexMutex;
static std::mutex g_docCrossReferenceMutex;
static std::mutex g_countFlowKeywordsMutex;

//-------------------------------------------------------------------

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

%}


BB                [ \t]+
B                 [ \t]*
NEWLINE           \n

DIGIT             [0-9]
LETTER            [A-Za-z\x80-\xFF]
NONEMPTY          [A-Za-z0-9_\x80-\xFF]
EXPCHAR           [#(){}\[\],:.%/\\=`*~|&<>!;+-]
NONEMPTYEXP       [^ \t\n:]
PARAMNONEMPTY     [^ \t\n():]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*
SCOPE             {IDENTIFIER}("."{IDENTIFIER})*
CALLANY           "("[^)\n]*")"
BORDER            ([^A-Za-z0-9])

POUNDCOMMENT      "##"

TRISINGLEQUOTE    "'''"
TRIDOUBLEQUOTE    "\"\"\""
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
KEYWORD           ("lambda"|"import"|"class"|"assert"|"with"|"as"|"from"|"global"|"async"|"def"|"True"|"False")
FLOWKW            ("or"|"and"|"is"|"not"|"print"|"for"|"in"|"if"|"try"|"except"|"yield"|"raise"|"break"|"continue"|"pass"|"if"|"return"|"while"|"elif"|"else"|"finally")
QUOTES            ("\""[^"]*"\"")
SINGLEQUOTES      ("'"[^']*"'")

LONGINTEGER       {INTEGER}("l"|"L")
INTEGER           ({DECIMALINTEGER}|{OCTINTEGER}|{HEXINTEGER})
DECIMALINTEGER    ({NONZERODIGIT}{DIGIT}*|"0")
OCTINTEGER        "0"{OCTDIGIT}+
HEXINTEGER        "0"("x"|"X"){HEXDIGIT}+
NONZERODIGIT      [1-9]
OCTDIGIT          [0-7]
HEXDIGIT          ({DIGIT}|[a-f]|[A-F])
FLOATNUMBER       ({POINTFLOAT}|{EXPONENTFLOAT})
POINTFLOAT        ({INTPART}?{FRACTION}|{INTPART}".")
EXPONENTFLOAT     ({INTPART}|{POINTFLOAT}){EXPONENT}
INTPART           {DIGIT}+
FRACTION          "."{DIGIT}+
EXPONENT          ("e"|"E")("+"|"-")?{DIGIT}+
IMAGNUMBER        ({FLOATNUMBER}|{INTPART})("j"|"J")
ATOM              ({IDENTIFIER}|{LITERAL}|{ENCLOSURE})
ENCLOSURE         ({PARENTH_FORM}|{LIST_DISPLAY}|{DICT_DISPLAY}|{STRING_CONVERSION})
LITERAL           ({STRINGLITERAL}|{INTEGER}|{LONGINTEGER}|{FLOATNUMBER}|{IMAGNUMBER})
PARENTH_FORM      "("{EXPRESSION_LIST}?")"
TEST              ({AND_TEST}("or"{AND_TEST})*|{LAMBDA_FORM})
TESTLIST          {TEST}( ","{TEST})*","?
LIST_DISPLAY      "["{LISTMAKER}?"]"
LISTMAKER         {EXPRESSION}({LIST_FOR}|(","{EXPRESSION})*","?)
LIST_ITER         ({LIST_FOR}|{LIST_IF})
LIST_FOR          "for"{EXPRESSION_LIST}"in"{TESTLIST}{LIST_ITER}?
LIST_IF           "if"{TEST}{LIST_ITER}?
DICT_DISPLAY      "\{"{KEY_DATUM_LIST}?"\}"
KEY_DATUM_LIST    {KEY_DATUM}(","{KEY_DATUM})*","?
KEY_DATUM         {EXPRESSION}":"{EXPRESSION}
STRING_CONVERSION "`"{EXPRESSION_LIST}"`"
PRIMARY           ({ATOM}|{ATTRIBUTEREF}|{SUBSCRIPTION}|{SLICING}|{CALL})
ATTRIBUTEREF      {PRIMARY}"."{IDENTIFIER}
SUBSCRIPTION      {PRIMARY}"["{EXPRESSION_LIST}"]"
SLICING           ({SIMPLE_SLICING}|{EXTENDED_SLICING})
SIMPLE_SLICING    {PRIMARY}"["{SHORT_SLICE}"]"
EXTENDED_SLICING  {PRIMARY}"["{SLICE_LIST}"]"
SLICE_LIST        {SLICE_ITEM}(","{SLICE_ITEM})*","?
SLICE_ITEM        ({EXPRESSION}|{PROPER_SLICE}|{ELLIPSIS})
PROPER_SLICE      ({SHORT_SLICE}|{LONG_SLICE})
SHORT_SLICE       {LOWER_BOUND}?":"{UPPER_BOUND}?
LONG_SLICE        {SHORT_SLICE}":"{STRIDE}?
LOWER_BOUND       {EXPRESSION}
UPPER_BOUND       {EXPRESSION}
STRIDE            {EXPRESSION}
ELLIPSIS          "..."
CALL              {PRIMARY}"("({ARGUMENT_LIST}","?)?")"
ARGUMENT_LIST     ({POSITIONAL_ARGUMENTS}(","{KEYWORD_ARGUMENTS})?(",""*"{EXPRESSION})?(",""**"{EXPRESSION})?|{KEYWORD_ARGUMENTS}(",""*"{EXPRESSION})?(",""**"{EXPRESSION})?|"*"{EXPRESSION}(",""**"{EXPRESSION})?|"**"{EXPRESSION})
POSITIONAL_ARGUMENTS  {EXPRESSION}(","{EXPRESSION})*
KEYWORD_ARGUMENTS     {KEYWORD_ITEM}(","{KEYWORD_ITEM})*
KEYWORD_ITEM      {IDENTIFIER}"="{EXPRESSION}
POWER             {PRIMARY}("**"{U_EXPR})?
U_EXPR            ({POWER}|"-"{U_EXPR}|"+"{U_EXPR}|"\~"{U_EXPR})
M_EXPR            ({U_EXPR}|{M_EXPR}"*"{U_EXPR}|{M_EXPR}"/""/"{U_EXPR}|{M_EXPR}"/"{U_EXPR}|{M_EXPR}"\%"{U_EXPR})
A_EXPR            ({M_EXPR}|{A_EXPR}"+"{M_EXPR}|{A_EXPR}"-"{M_EXPR}
SHIFT_EXPR        ({A_EXPR}|{SHIFT_EXPR}("<<"|">>"){A_EXPR})
AND_EXPR          ({SHIFT_EXPR}|{AND_EXPR}"\;SPMamp;"{SHIFT_EXPR}
XOR_EXPR          ({AND_EXPR}|{XOR_EXPR}"\textasciicircum"{AND_EXPR})
OR_EXPR           ({XOR_EXPR}|{OR_EXPR}"|"{ XOR_EXPR})

COMPARISON        {OR_EXPR}({COMP_OPERATOR}{OR_EXPR})*
COMP_OPERATOR     ("<"|">"|"=="|">="|"<="|"<>"|"!="|"is""not"?|"not"?"in")
EXPRESSION        ({OR_TEST}|{LAMBDA_FORM})
OR_TEST           ({AND_TEST}|{OR_TEST}"or"{AND_TEST})
AND_TEST          ({NOT_TEST}|{AND_TEST}"and"{NOT_TEST})
NOT_TEST          ({COMPARISON}|"not"{NOT_TEST})
LAMBDA_FORM       "lambda"{PARAMETER_LIST}?":"{EXPRESSION}
EXPRESSION_LIST   {EXPRESSION}(","{EXPRESSION})*","?
SIMPLE_STMT       ({EXPRESSION_STMT}|{ASSERT_STMT}|{ASSIGNMENT_STMT}|{AUGMENTED_ASSIGNMENT_STMT}|{PASS_STMT}|{DEL_STMT}|{PRINT_STMT}|{RETURN_STMT}|{YIELD_STMT}|{RAISE_STMT}|{BREAK_STMT}|{CONTINUE_STMT}|{IMPORT_STMT}|{GLOBAL_STMT}|{EXEC_STMT})
EXPRESSION_STMT   {EXPRESSION_LIST}
ASSERT_STMT       "assert"{EXPRESSION}(","{EXPRESSION})?
ASSIGNMENT_STMT   ({TARGET_LIST}"=")+{EXPRESSION_LIST}
TARGET_LIST       {TARGET}(","{TARGET})*","?
TARGET            ({IDENTIFIER}|"("{TARGET_LIST}")"|"["{TARGET_LIST}"]"|{ATTRIBUTEREF}|{SUBSCRIPTION}|{SLICING})

%option noyywrap
%option stack

%x Body

%x FunctionDec
%x FunctionParams

%x ClassDec
%x ClassInheritance

%x Suite
%x SuiteCaptureIndent
%x SuiteStart
%x SuiteMaintain
%x SuiteContinuing

%x LongString

%x SingleQuoteString
%x DoubleQuoteString
%x TripleString

%x DocBlock
%%

<Body,Suite>{
      "def"{BB}                     {
                                      startFontClass(yyscanner,"keyword");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                      BEGIN( FunctionDec );
                                    }
      "async"{BB}"def"{BB}          {
                                      startFontClass(yyscanner,"keyword");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                      BEGIN( FunctionDec );
                                    }

      "class"{BB}                   {
                                      startFontClass(yyscanner,"keyword");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                      BEGIN( ClassDec );
                                    }
      "None"                        {
                                      startFontClass(yyscanner,"keywordtype");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }
      "self."{IDENTIFIER}/"."({IDENTIFIER}".")*{IDENTIFIER}"(" {
                                      codify(yyscanner,"self.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[5]);
                                    }
      "self."{IDENTIFIER}/"("       {
                                      codify(yyscanner,"self.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[5]);
                                    }
      "self."{IDENTIFIER}/"."({IDENTIFIER}".")*{IDENTIFIER} {
                                      codify(yyscanner,"self.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[5]);
                                    }
      "self."{IDENTIFIER}           {
                                      codify(yyscanner,"self.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[5]);
                                    }
      "cls."{IDENTIFIER}/"."({IDENTIFIER}".")*{IDENTIFIER}"(" {
                                      codify(yyscanner,"cls.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[4]);
                                    }
      "cls."{IDENTIFIER}/"("        {
                                      codify(yyscanner,"cls.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[4]);
                                    }
      "cls."{IDENTIFIER}/"."({IDENTIFIER}".")*{IDENTIFIER} {
                                      codify(yyscanner,"cls.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[4]);
                                    }
      "cls."{IDENTIFIER}            {
                                      codify(yyscanner,"cls.");
                                      findMemberLink(yyscanner,*yyextra->code,&yytext[4]);
                                    }
      "@"{SCOPE}{CALLANY}?          { // decorator
                                      startFontClass(yyscanner,"preprocessor");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }
}

<ClassDec>{IDENTIFIER}              {
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                      // codify(yyscanner,yytext);
                                      yyextra->curClassName = yytext;
                                      yyextra->curClassBases.clear();
                                      BEGIN( ClassInheritance );
                                    }

<ClassInheritance>{
   ({BB}|[(,)])                     {
                                      codify(yyscanner,yytext);
                                    }

   ({IDENTIFIER}".")*{IDENTIFIER}   {
                                      // The parser
                                      // is assuming
                                      // that ALL identifiers
                                      // in this state
                                      // are base classes;
                                      // it doesn't check to see
                                      // that the first parenthesis
                                      // has been seen.

                                      // This is bad - it should
                                      // probably be more strict
                                      // about what to accept.

                                      yyextra->curClassBases.push_back(yytext);
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                      // codify(yyscanner,yytext);
                                    }

    ":"                             {
                                      codify(yyscanner,yytext);

                                      // Assume this will
                                      // be a one-line suite;
                                      // found counter-example
                                      // in SuiteStart.

                                      // Push a class scope

                                      std::unique_ptr<ClassDef> classDefToAdd { createClassDef("<code>",1,1,yyextra->curClassName,ClassDef::Class,QCString(),QCString(),FALSE) };
                                      ScopedTypeVariant var(yyextra->curClassName);
                                      for (const auto &s : yyextra->curClassBases)
                                      {
                                        const ClassDef *baseDefToAdd = 0;
                                        // find class in the local scope
                                        auto it = yyextra->codeClassMap.find(s);
                                        if (it != yyextra->codeClassMap.end())
                                        {
                                          baseDefToAdd = toClassDef(it->second.globalDef());
                                        }
                                        // Try to find class in global scope
                                        if (baseDefToAdd==0)
                                        {
                                          baseDefToAdd=yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,s.c_str());
                                        }

                                        if (baseDefToAdd && baseDefToAdd->name()!=yyextra->curClassName)
                                        {
                                          var.localDef()->insertBaseClass(baseDefToAdd->name());
                                        }
                                      }
                                      yyextra->codeClassMap.emplace(std::make_pair(yyextra->curClassName.str(),std::move(var)));

                                      // Reset class-parsing variables.
                                      yyextra->curClassName.resize(0);
                                      yyextra->curClassBases.clear();

                                      yyextra->noSuiteFound = TRUE;
                                      BEGIN( SuiteStart );
                                    }
}


<FunctionDec>{
    {IDENTIFIER}                    {
                                      generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                    }

    {B}"("                          {
                                      codify(yyscanner,yytext);
                                      BEGIN( FunctionParams );
                                    }
}

<FunctionParams>{
    ({BB}|",")                      {
                                      // Parses delimiters
                                      codify(yyscanner,yytext);
                                    }

    ({IDENTIFIER}|{PARAMNONEMPTY}+) {
                                      codify(yyscanner,yytext);
                                    }

    ")"                             {
                                      codify(yyscanner,yytext);
                                    }

    "\n"                            {
                                      codifyLines(yyscanner,yytext);
                                    }

    ":"                             {
                                      codify(yyscanner,yytext);

                                      // Assume this will
                                      // be a one-line suite;
                                      // found counter-example
                                      // in SuiteStart.
                                      yyextra->noSuiteFound = TRUE;
                                      BEGIN( SuiteStart );
                                    }
}

<Body,Suite>{

  {KEYWORD}                         {
                                      // Position-sensitive rules!
                                      // Must come AFTER keyword-triggered rules
                                      // Must come BEFORE identifier NONEMPTY-like rules
                                      //   to syntax highlight.

                                      startFontClass(yyscanner,"keyword");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }

    {FLOWKW}                        {
                                      if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                      {
                                        std::lock_guard<std::mutex> lock(g_countFlowKeywordsMutex);
                                        MemberDefMutable *mdm = toMemberDefMutable(yyextra->currentMemberDef);
                                        if (mdm)
                                        {
                                          mdm->incrementFlowKeyWordCount();
                                        }
                                      }
                                      startFontClass(yyscanner,"keywordflow");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }
    ({IDENTIFIER}".")*{IDENTIFIER}/"("  {
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                    }
    ({IDENTIFIER}".")+{IDENTIFIER}  {
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,TRUE);
                                    }
    {IDENTIFIER}                    { codify(yyscanner,yytext); }

}



<SuiteStart>{

    {BB}                            {
                                      codify(yyscanner,yytext);
                                    }
    "pass"                          {
                                      startFontClass(yyscanner,"keyword");
                                      codifyLines(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                      BEGIN(Body);
                                    }
    {KEYWORD}                       {
                                      startFontClass(yyscanner,"keyword");
                                      codifyLines(yyscanner,yytext);
                                      endFontClass(yyscanner);

                                      // No indentation necessary
                                      yyextra->noSuiteFound = FALSE;
                                    }

    {FLOWKW}                        {
                                      if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                      {
                                        std::lock_guard<std::mutex> lock(g_countFlowKeywordsMutex);
                                        MemberDefMutable *mdm = toMemberDefMutable(yyextra->currentMemberDef);
                                        if (mdm)
                                        {
                                          mdm->incrementFlowKeyWordCount();
                                        }
                                      }
                                      startFontClass(yyscanner,"keywordflow");
                                      codifyLines(yyscanner,yytext);
                                      endFontClass(yyscanner);

                                      // No indentation necessary
                                      yyextra->noSuiteFound = FALSE;
                                    }
    {IDENTIFIER}                    {
                                      codify(yyscanner,yytext);
                                    }


    {POUNDCOMMENT}                  {
                                      if (YY_START==SingleQuoteString ||
                                          YY_START==DoubleQuoteString ||
                                          YY_START==TripleString
                                         )
                                      {
                                        REJECT;
                                      }
                                      yy_push_state(YY_START,yyscanner);
                                      BEGIN(DocBlock);
                                      yyextra->docBlock=yytext;
                                    }

    {NEWLINE}                       {
                                      codifyLines(yyscanner,yytext);
                                      if ( yyextra->noSuiteFound )
                                      {
                                        // printf("New suite to capture! [%d]\n", yyextra->yyLineNr);
                                        BEGIN ( SuiteCaptureIndent );
                                      }
                                    }
}

<SuiteCaptureIndent>{
    "\n"|({BB}"\n")                 {
                                      // Blankline - ignore, keep looking for indentation.
                                      codifyLines(yyscanner,yytext);
                                    }

    {BB}                            {
                                      // This state lasts momentarily,
                                      // to check the indentation
                                      // level that is about to be
                                      // used.
                                      codifyLines(yyscanner,yytext);
                                      yyextra->indents.push(yyleng);
                                      // printf("Captured indent of %d [line %d]\n", yyleng, yyextra->yyLineNr);
                                      BEGIN( Suite );
                                    }
}

<SuiteMaintain>{

    {BB}/({NONEMPTY}|{EXPCHAR})     {
                                      // This implements poor
                                      // indentation-tracking;
                                      // should be improved.
                                      // (translate tabs to space, etc)
                                      codifyLines(yyscanner,yytext);
                                      adjustScopesAndSuites(yyscanner,static_cast<int>(yyleng));
                                    }

    "\n"|({BB}"\n")                 {
                                      // If this ever succeeds,
                                      // it means that this is
                                      // a blank line, and
                                      // can be ignored.
                                      codifyLines(yyscanner,yytext);
                                    }

    ""/({NONEMPTY}|{EXPCHAR})       {
                                       // Default rule; matches
                                       // the empty string, assuming
                                       // real text starts here.
                                       // Just go straight to Body.
                                       adjustScopesAndSuites(yyscanner,0);
                                    }
}


<Suite>{NEWLINE}                    {
                                      codifyLines(yyscanner,yytext);
                                      BEGIN( SuiteMaintain );
                                    }
<Body>{IDENTIFIER}                  {
                                      codify(yyscanner,yytext);
                                    }
<Body>{NEWLINE}                     {
                                      codifyLines(yyscanner,yytext);
                                    }

<SingleQuoteString>{ // Single quoted string like 'That\'s a """nice""" string!'
    \\{B}\n                         { // line continuation
                                      codifyLines(yyscanner,yytext);
                                    }
    \\.                             { // escaped char
                                      codify(yyscanner,yytext);
                                    }
    {STRINGPREFIX}?{TRIDOUBLEQUOTE} { // triple double quotes
                                      codify(yyscanner,yytext);
                                    }
    "'"                             { // end of the string
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                      BEGIN(yyextra->stringContext);
                                    }
    [^"'\n\\]+                      { // normal chars
                                      codify(yyscanner,yytext);
                                    }
    .                               { // normal char
                                      codify(yyscanner,yytext);
                                    }
}

<DoubleQuoteString>{ // Double quoted string like "That's \"a '''nice'''\" string!"
    \\{B}\n                         { // line continuation
                                      codifyLines(yyscanner,yytext);
                                    }
    \\.                             { // escaped char
                                      codify(yyscanner,yytext);
                                    }
    {STRINGPREFIX}?{TRISINGLEQUOTE} { // triple single quotes
                                      codify(yyscanner,yytext);
                                    }
    "\""                            { // end of the string
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                      BEGIN(yyextra->stringContext);
                                    }
    [^"'\n\\]+                      { // normal chars
                                      codify(yyscanner,yytext);
                                    }
    .                               { // normal char
                                      codify(yyscanner,yytext);
                                    }
}

<TripleString>{
    {TRIDOUBLEQUOTE}                |
    {TRISINGLEQUOTE}                {
                                      codify(yyscanner,yytext);
                                      if (yyextra->doubleQuote==(yytext[0]=='"'))
                                      {
                                        endFontClass(yyscanner);
                                        BEGIN(yyextra->stringContext);
                                      }
                                    }
    {LONGSTRINGBLOCK}               {
                                      codifyLines(yyscanner,yytext);
                                    }
    \n                              {
                                      codifyLines(yyscanner,yytext);
                                    }
    .                               {
                                      codify(yyscanner,yytext);
                                    }
}


<*>{STRINGPREFIX}?{TRISINGLEQUOTE}  {
                                      if (YY_START==SingleQuoteString) REJECT;
                                      startFontClass(yyscanner,"stringliteral");
                                      yyextra->stringContext=YY_START;
                                      yyextra->doubleQuote=yytext[yyleng-1]=='"';
                                      codify(yyscanner,yytext);
                                      BEGIN(TripleString);
                                    }
<*>{STRINGPREFIX}?{TRIDOUBLEQUOTE}  {
                                      if (YY_START==DoubleQuoteString) REJECT;
                                      startFontClass(yyscanner,"stringliteral");
                                      yyextra->stringContext=YY_START;
                                      yyextra->doubleQuote=yytext[yyleng-1]=='"';
                                      codify(yyscanner,yytext);
                                      BEGIN(TripleString);
                                    }
<*>{STRINGPREFIX}?"'"               { // single quoted string
                                      if (YY_START==SingleQuoteString ||
                                          YY_START==DoubleQuoteString ||
                                          YY_START==TripleString)
                                      {
                                        REJECT;
                                      }
                                      startFontClass(yyscanner,"stringliteral");
                                      yyextra->stringContext=YY_START;
                                      codify(yyscanner,yytext);
                                      BEGIN(SingleQuoteString);
                                    }
<*>{STRINGPREFIX}?"\""              { // double quoted string
                                      if (YY_START==SingleQuoteString ||
                                          YY_START==DoubleQuoteString ||
                                          YY_START==TripleString)
                                      {
                                        REJECT;
                                      }
                                      startFontClass(yyscanner,"stringliteral");
                                      yyextra->stringContext=YY_START;
                                      codify(yyscanner,yytext);
                                      BEGIN(DoubleQuoteString);
                                    }
<DocBlock>.*                        { // contents of current comment line
                                      yyextra->docBlock+=yytext;
                                    }
<DocBlock>"\n"{B}("#")              { // comment block (next line is also comment line)
                                     yyextra->docBlock+=yytext;
                                    }
<DocBlock>{NEWLINE}                 { // comment block ends at the end of this line
                                      // remove special comment (default config)
                                      if (Config_getBool(STRIP_CODE_COMMENTS))
                                      {
                                        yyextra->yyLineNr+=((QCString)yyextra->docBlock).contains('\n');
                                        yyextra->endComment=TRUE;
                                      }
                                      else // do not remove comment
                                      {
                                        startFontClass(yyscanner,"comment");
                                        codifyLines(yyscanner,yyextra->docBlock);
                                        endFontClass(yyscanner);
                                      }
                                      unput(*yytext);
                                      yy_pop_state(yyscanner);
                                    }
<*>{POUNDCOMMENT}.*                 {
                                      if (YY_START==SingleQuoteString ||
                                         YY_START==DoubleQuoteString ||
                                         YY_START==TripleString)
                                      {
                                        REJECT;
                                      }
                                      yy_push_state(YY_START,yyscanner);
                                      BEGIN(DocBlock);
                                      yyextra->docBlock=yytext;
                                    }
<*>"#".*                            { // normal comment
                                      if (YY_START==SingleQuoteString ||
                                          YY_START==DoubleQuoteString ||
                                          YY_START==TripleString)
                                      {
                                        REJECT;
                                      }
                                      startFontClass(yyscanner,"comment");
                                      codifyLines(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }
<*>{NEWLINE}                      {
                                    if (yyextra->endComment)
                                    {
                                      yyextra->endComment=FALSE;
                                    }
                                    else
                                    {
                                      codifyLines(yyscanner,yytext);
                                    }
                                    //printf("[pycode] %d NEWLINE [line %d] no match\n",
                                    //       YY_START, yyextra->yyLineNr);

                                    BEGIN(Body);
                                  }

<*>[ \t]+                         {
                                    codify(yyscanner,yytext);
                                    BEGIN(Body);
                                  }
<*>.                              {
                                    codify(yyscanner,yytext);
                                    // printf("[pycode] '%s' [ state %d ]  [line %d] no match\n",
                                    //        yytext, YY_START, yyextra->yyLineNr);

                                    BEGIN(Body);
                                  }

<*><<EOF>>                        {
                                    if (YY_START==DocBlock && !Config_getBool(STRIP_CODE_COMMENTS))
                                    {
                                      startFontClass(yyscanner,"comment");
                                      codifyLines(yyscanner,yyextra->docBlock);
                                      endFontClass(yyscanner);
                                    }
                                    yyterminate();
                                  }
%%

/*@ ----------------------------------------------------------------------------
 */

#if 0 // TODO: call me to store local variables and get better syntax highlighting, see code.l
static void addVariable(yyscan_t yyscanner, QCString type, QCString name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("PyVariableContext::addVariable(%s,%s)\n",qPrint(type),qPrint(name));
  QCString ltype = type.simplifyWhiteSpace();
  QCString lname = name.simplifyWhiteSpace();

  auto it = yyextra->codeClassMap.find(ltype.str());
  if (it!=yyextra->codeClassMap.end())
  {
    yyextra->theVarContext.addVariable(lname,std::move(it->second));
  }
  else
  {
    const ClassDef *varType = getResolvedClass(yyextra->currentDefinition,yyextra->sourceFileDef,ltype); // look for global class definitions
    if (varType)
    {
      yyextra->theVarContext.addVariable(lname,ScopedTypeVariant(varType));
    }
    else
    {
      if (!yyextra->theVarContext.atGlobalScope()) // for local variable add a dummy entry to avoid linking to a global that is shadowed.
      {
        yyextra->theVarContext.addVariable(lname.str(),ScopedTypeVariant());
      }
    }
  }
}
#endif

//-------------------------------------------------------------------------------

static yy_size_t yyread(yyscan_t yyscanner, char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  yy_size_t c=0;
  while( c < max_size && *s )
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}

//-------------------------------------------------------------------------------

/*!
  Examines current stack of white-space indentations;
  re-syncs the parser with the correct scope.
*/
static void adjustScopesAndSuites(yyscan_t yyscanner,unsigned indentLength)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // States to pop
  if (!yyextra->indents.empty() && indentLength < yyextra->indents.top())
  {
    while (!yyextra->indents.empty() && indentLength < yyextra->indents.top())
    {
      // printf("Exited scope indent of [%d]\n", yyextra->indents.top());
      yyextra->indents.pop(); // Pop the old suite's indentation

      yyextra->currentMemberDef=0;
      if (yyextra->currentDefinition)
        yyextra->currentDefinition=yyextra->currentDefinition->getOuterScope();
    }
  }

  // Are there any remaining indentation levels for suites?
  if (!yyextra->indents.empty())
  {
    BEGIN( Suite );
  }
  else
  {
    BEGIN( Body );
  }
}

//-------------------------------------------------------------------------------

/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c;
  int count=1;
  while ((c=*p))
  {
    p++;
    if (c=='\n') count++;
  }
  if (p>yyextra->inputString && *(p-1)!='\n')
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++;
    yyextra->needsTermination=TRUE;
  }
  return count;
}

//-------------------------------------------------------------------------------

static void setCurrentDoc(yyscan_t yyscanner, const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    std::lock_guard<std::mutex> lock(g_searchIndexMutex);
    if (yyextra->searchCtx)
    {
      yyextra->code->setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),FALSE);
    }
    else
    {
      yyextra->code->setCurrentDoc(yyextra->sourceFileDef,anchor,TRUE);
    }
  }
}

//-------------------------------------------------------------------------------

static void addToSearchIndex(yyscan_t yyscanner, const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    std::lock_guard<std::mutex> lock(g_searchIndexMutex);
    yyextra->code->addWord(text,FALSE);
  }
}

//-------------------------------------------------------------------------------

static const ClassDef *stripClassName(yyscan_t yyscanner,const QCString &s,Definition *d)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int pos=0;
  QCString type = s;
  QCString className;
  QCString templSpec;
  while (extractClassNameFromType(type,pos,className,templSpec)!=-1)
  {
    QCString clName=className+templSpec;
    const ClassDef *cd=0;
    if (!yyextra->classScope.isEmpty())
    {
      cd=yyextra->symbolResolver.resolveClass(d,yyextra->classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=yyextra->symbolResolver.resolveClass(d,clName);
    }
    if (cd)
    {
      return cd;
    }
  }

  return 0;
}

//-------------------------------------------------------------------------------

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //if (yyextra->currentFontClass) { yyextra->code->endFontClass(yyscanner); }
  if (yyextra->sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",yyextra->yyLineNr);
    //lineAnchor.sprintf("l%05d",yyextra->yyLineNr);

    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    //printf("startCodeLine %d d=%p\n",yyextra->yyLineNr,d);
    //yyextra->code->startLineNumber();

    if (!yyextra->includeCodeFragment && d && d->isLinkableInProject())
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      //yyextra->insideBody = FALSE;
      yyextra->endComment = FALSE;
      yyextra->searchingForBody = TRUE;
      yyextra->realScope = d->name();
      yyextra->classScope = d->name();
      //printf("Real scope: '%s'\n",qPrint(yyextra->realScope));
      yyextra->bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else
      {
        yyextra->code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                QCString(),yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      //yyextra->code->codify(lineNumber);
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr);
    }
    //yyextra->code->endLineNumber();
  }
  yyextra->code->startCodeLine(yyextra->sourceFileDef);
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
}

//-------------------------------------------------------------------------------

static void codify(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->code->codify(text);
}

//-------------------------------------------------------------------------------

static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
}

//-------------------------------------------------------------------------------

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *fc = yyextra->currentFontClass;
  endCodeLine(yyscanner);
  if (yyextra->yyLineNr<yyextra->inputLines)
  {
    yyextra->currentFontClass = fc;
    startCodeLine(yyscanner);
  }
}

//-------------------------------------------------------------------------------

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(yyscan_t yyscanner,
                  CodeOutputInterface &ol,
                  const Definition *d,
                  const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty()) return;
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  TooltipManager::instance().addTooltip(ol,d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip;
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
    tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=FALSE;
  const char *p=text.data();
  while (!done)
  {
    const char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,QCString(sp,p-sp-1),tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}

//-------------------------------------------------------------------------------

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // if font class is already set don't stop and start it.
  // strcmp does not like null pointers as input.
  if (!yyextra->currentFontClass || !s || strcmp(yyextra->currentFontClass,s))
  {
    endFontClass(yyscanner);
    yyextra->code->startFontClass(s);
    yyextra->currentFontClass=s;
  }
}

//-------------------------------------------------------------------------------

static void endFontClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=0;
  }
}

//-------------------------------------------------------------------------------

static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty()) return;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  const char *p=text.data(),*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      int l = (int)(p-sp-1);
      char *tmp = (char*)malloc(l+1);
      memcpy(tmp,sp,l);
      tmp[l]='\0';
      yyextra->code->codify(tmp);
      free(tmp);
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(sp);
      done=TRUE;
    }
  }
}

//-------------------------------------------------------------------------------

static bool getLinkInScope(yyscan_t yyscanner,
                           const QCString &c,  // scope
                           const QCString &m,  // member
                           const QCString &memberText, // exact text
                           CodeOutputInterface &ol,
                           const QCString &text
                          )
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const MemberDef    *md = 0;
  const ClassDef     *cd = 0;
  const FileDef      *fd = 0;
  const NamespaceDef *nd = 0;
  const GroupDef     *gd = 0;
  //printf("Trying '%s'::'%s'\n",qPrint(c),qPrint(m));
  if (getDefs(c,m,"()",md,cd,fd,nd,gd,FALSE,yyextra->sourceFileDef) &&
      md->isLinkable())
  {
    //Definition *d=0;
    //if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;

    const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                          md->getBodyDef() : md->getOuterScope();
    //printf("Found! d=%s\n",d?qPrint(d->name()):"<none>");
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
      //printf("yyextra->currentDefinition=%p yyextra->currentMemberDef=%p\n",
      //        yyextra->currentDefinition,yyextra->currentMemberDef);

      if (yyextra->currentDefinition && yyextra->currentMemberDef &&
          md!=yyextra->currentMemberDef && yyextra->collectXRefs)
      {
        std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
        addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(md));
      }
      //printf("d->getReference()='%s' d->getOutputBase()='%s' name='%s' member name='%s'\n",qPrint(d->getReference()),qPrint(d->getOutputFileBase()),qPrint(d->name()),qPrint(md->name()));

      writeMultiLineCodeLink(yyscanner,ol,md, !text.isEmpty() ? text : memberText);
      addToSearchIndex(yyscanner,!text.isEmpty() ? text : memberText);
      return TRUE;
    }
  }
  return FALSE;
}

//-------------------------------------------------------------------------------

static bool getLink(yyscan_t yyscanner,
                    const QCString &className,
                    const QCString &memberName,
                    CodeOutputInterface &ol,
                    const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString m=removeRedundantWhiteSpace(memberName);
  QCString c=className;
  if (!getLinkInScope(yyscanner,c,m,memberName,ol,text))
  {
    if (!yyextra->curClassName.isEmpty())
    {
      if (!c.isEmpty()) c.prepend("::");
      c.prepend(yyextra->curClassName);
      return getLinkInScope(yyscanner,c,m,memberName,ol,text);
    }
    return FALSE;
  }
  return TRUE;
}

//-------------------------------------------------------------------------------

/*
  For a given string in the source code,
  finds its class or global id and links to it.
*/
static void generateClassOrGlobalLink(yyscan_t yyscanner,
                                      CodeOutputInterface &ol,
                                      const QCString &clName,
                                      bool typeOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString className=clName;

  // Don't do anything for empty text
  if (className.isEmpty()) return;

  DBG_CTX((stderr,"generateClassOrGlobalLink(className=%s)\n",qPrint(className)));

  const ScopedTypeVariant *lcd = 0;
  const ClassDef *cd=0;             // Class def that we may find 
  const MemberDef *md=0;            // Member def that we may find 
  //bool isLocal=FALSE;

  if ((lcd=yyextra->theVarContext.findVariable(className))==0) // not a local variable
  {
    const Definition *d = yyextra->currentDefinition;
    QCString scope = substitute(className,".","::");

    cd = yyextra->symbolResolver.resolveClass(d,substitute(className,".","::"));
    md = yyextra->symbolResolver.getTypedef();

    DBG_CTX((stderr,"d=%s yyextra->sourceFileDef=%s\n",
        d?qPrint(d->displayName()):"<null>",
        yyextra->currentDefinition?qPrint(yyextra->currentDefinition->displayName()):"<null>"));
    DBG_CTX((stderr,"is found as a type %s\n",cd?qPrint(cd->name()):"<null>"));

    if (cd==0 && md==0) // also see if it is variable or enum or enum value
    {
      const NamespaceDef *nd = getResolvedNamespace(scope);
      if (nd)
      {
        writeMultiLineCodeLink(yyscanner,ol,nd,clName);
        addToSearchIndex(yyscanner,className);
        return;
      }
      else if (getLink(yyscanner,yyextra->classScope,clName,ol,clName))
      {
        return;
      }
    }
  }
  else
  {
    if (lcd->type()!=ScopedTypeVariant::Dummy)
    {
      yyextra->theCallContext.setScope(*lcd);
    }
    //isLocal=TRUE;
    DBG_CTX((stderr,"is a local variable cd=%p!\n",cd));
  }

  if (cd && cd->isLinkable()) // is it a linkable class
  {
    writeMultiLineCodeLink(yyscanner,ol,cd,clName);
    addToSearchIndex(yyscanner,className);
    if (md)
    {
      const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                            md->getBodyDef() : md->getOuterScope();
      if (md->getGroupDef()) d = md->getGroupDef();
      if (d && d->isLinkable() && md->isLinkable() &&
          yyextra->currentMemberDef && yyextra->collectXRefs)
      {
        std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
        addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(md));
      }
    }
  }
  else // not a class, maybe a global member
  {
    int scopeEnd = className.findRev(".");
    if (scopeEnd!=-1 && !typeOnly) // name with explicit scope
    {
      QCString scope = substitute(className.left(scopeEnd),".","::");
      QCString locName = className.right(className.length()-scopeEnd-1);
      ClassDef *mcd = getClass(scope);
      DBG_CTX((stderr,"scope=%s locName=%s mcd=%p\n",qPrint(scope),qPrint(locName),mcd));
      if (mcd)
      {
        const MemberDef *mmd = mcd->getMemberByName(locName);
        if (mmd)
        {
          yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,mmd->typeString(),mmd->getOuterScope())));
          writeMultiLineCodeLink(yyscanner,ol,mmd,clName);
          addToSearchIndex(yyscanner,className);
          const Definition *d = mmd->getOuterScope()==Doxygen::globalScope ?
                                mmd->getBodyDef() : mmd->getOuterScope();
          if (mmd->getGroupDef()) d = mmd->getGroupDef();
          if (d && d->isLinkable() && mmd->isLinkable() &&
              yyextra->currentMemberDef && yyextra->collectXRefs)
          {
            std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
            addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(mmd));
          }
          return;
        }
      }
      else // check namespace as well
      {
        const NamespaceDef *mnd = getResolvedNamespace(scope);
        if (mnd)
        {
          const MemberDef *mmd=mnd->getMemberByName(locName);
          if (mmd)
          {
            //printf("name=%s scope=%s\n",qPrint(locName),qPrint(scope));
            yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,mmd->typeString(),mmd->getOuterScope())));
            writeMultiLineCodeLink(yyscanner,ol,mmd,clName);
            addToSearchIndex(yyscanner,className);
            const Definition *d = mmd->getOuterScope()==Doxygen::globalScope ?
                                  mmd->getBodyDef() : mmd->getOuterScope();
            if (mmd->getGroupDef()) d = mmd->getGroupDef();
            if (d && d->isLinkable() && mmd->isLinkable() &&
                yyextra->currentMemberDef && yyextra->collectXRefs)
            {
              std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
              addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(mmd));
            }
            return;
          }
        }
      }
    }

    // nothing found, just write out the word
    codifyLines(yyscanner,clName);
    addToSearchIndex(yyscanner,clName);
  }
}

//-------------------------------------------------------------------------------

/*
   As of June 1, this function seems to work
   for file members, but scopes are not
   being correctly tracked for classes
   so it doesn't work for classes yet.

*/
static void generateFunctionLink(yyscan_t yyscanner,
                                CodeOutputInterface &ol,
                                const QCString &funcName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString locScope=yyextra->classScope;
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  DBG_CTX((stdout,"*** locScope=%s locFunc=%s\n",qPrint(locScope),qPrint(locFunc)));
  int i=locFunc.findRev("::");
  if (i>0)
  {
    locScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-2).stripWhiteSpace();
  }
  //printf("generateFunctionLink(%s) classScope='%s'\n",qPrint(locFunc),qPrint(locScope));
  if (!locScope.isEmpty())
  {
    auto it = yyextra->codeClassMap.find(locScope.str());
    if (it!=yyextra->codeClassMap.end())
    {
      ScopedTypeVariant ccd = it->second;
      //printf("using classScope %s\n",qPrint(yyextra->classScope));
      if (ccd.localDef() && !ccd.localDef()->baseClasses().empty())
      {
        for (const auto &bcName : ccd.localDef()->baseClasses())
        {
          if (getLink(yyscanner,bcName,locFunc,ol,funcName))
          {
            return;
          }
        }
      }
    }
  }
  if (!getLink(yyscanner,locScope,locFunc,ol,funcName))
  {
    generateClassOrGlobalLink(yyscanner,ol,funcName);
  }
  return;
}

//-------------------------------------------------------------------------------

static bool findMemberLink(yyscan_t yyscanner,
                           CodeOutputInterface &ol,
                           Definition *sym,
                           const QCString &symName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("sym %s outerScope=%s equal=%d\n",
  //    qPrint(sym->name()),qPrint(sym->getOuterScope()->name()),
  //    sym->getOuterScope()==yyextra->currentDefinition);

  if (sym->getOuterScope() &&
      sym->getOuterScope()->definitionType()==Definition::TypeClass &&
      yyextra->currentDefinition->definitionType()==Definition::TypeClass)
  {
    const ClassDef *cd = toClassDef(sym->getOuterScope());
    const ClassDef *thisCd = toClassDef(yyextra->currentDefinition);
    if (sym->definitionType()==Definition::TypeMember)
    {
      if (yyextra->currentMemberDef && yyextra->collectXRefs)
      {
        std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
        addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(toMemberDef(sym)));
      }
    }
    DBG_CTX((stderr,"cd=%s thisCd=%s\n",cd?qPrint(cd->name()):"<none>",thisCd?qPrint(thisCd->name()):"<none>"));

    // TODO: find the nearest base class in case cd is a base class of
    // thisCd
    if (cd==thisCd || (thisCd && thisCd->isBaseClass(cd,TRUE)))
    {
      writeMultiLineCodeLink(yyscanner,ol,sym,symName);
      return TRUE;
    }
  }
  return FALSE;
}

//-------------------------------------------------------------------------------

static void findMemberLink(yyscan_t yyscanner,
                           CodeOutputInterface &ol,
                           const QCString &symName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("Member reference: %s scope=%s member=%s\n",
  //    yytext,
  //    yyextra->currentDefinition?qPrint(yyextra->currentDefinition->name()):"<none>",
  //    yyextra->currentMemberDef?qPrint(yyextra->currentMemberDef->name()):"<none>"
  //    );
  if (yyextra->currentDefinition)
  {
    auto range = Doxygen::symbolMap.find(symName);
    for (auto it = range.first; it!=range.second; ++it)
    {
      findMemberLink(yyscanner,ol,it->second,symName);
    }
  }
  //printf("sym %s not found\n",&yytext[5]);
  codify(yyscanner,symName);
}


//-------------------------------------------------------------------------------

struct PythonCodeParser::Private
{
  yyscan_t yyscanner;
  pycodeYY_state state;
};

PythonCodeParser::PythonCodeParser() : p(std::make_unique<Private>())
{
  pycodeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  pycodeYYset_debug(1,p->yyscanner);
#endif
  resetCodeParserState();
}

PythonCodeParser::~PythonCodeParser()
{
  pycodeYYlex_destroy(p->yyscanner);
}

void PythonCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->codeClassMap.clear();
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
  yyextra->doubleStringIsDoc = FALSE;
  yyextra->paramParens = 0;
  while (!yyextra->indents.empty()) yyextra->indents.pop();
  BEGIN( Body );
}

void PythonCodeParser::parseCode(CodeOutputInterface &codeOutIntf,
    const QCString &scopeName,
    const QCString &input,
    SrcLangExt /*lang*/,
    bool isExampleBlock,
    const QCString &exampleName,
    FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    const MemberDef *memberDef,
    bool showLineNumbers,
    const Definition *searchCtx,
    bool collectXRefs
    )
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  //printf("***parseCode()\n");

  if (input.isEmpty()) return;
  printlex(yy_flex_debug, TRUE, __FILE__, fileDef ? qPrint(fileDef->fileName()): NULL);
  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = FALSE;
  yyextra->searchCtx=searchCtx;
  yyextra->collectXRefs=collectXRefs;
  if (startLine!=-1)
    yyextra->yyLineNr    = startLine;
  else
    yyextra->yyLineNr    = 1;
  if (endLine!=-1)
    yyextra->inputLines  = endLine+1;
  else
    yyextra->inputLines  = yyextra->yyLineNr + countLines(yyscanner) - 1;


  yyextra->exampleBlock  = isExampleBlock;
  yyextra->exampleName   = exampleName;
  yyextra->sourceFileDef = fileDef;
  yyextra->symbolResolver.setFileScope(fileDef);

  bool cleanupSourceDef = FALSE;
  if (yyextra->exampleBlock && fileDef==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef("",(!exampleName.isEmpty()?qPrint(exampleName):"generated"));
    cleanupSourceDef = TRUE;
  }
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }

  yyextra->includeCodeFragment = inlineFragment;
  // Starts line 1 on the output
  startCodeLine(yyscanner);

  pycodeYYrestart(0,yyscanner);

  pycodeYYlex(yyscanner);

  if (!yyextra->indents.empty())
  {
    // printf("Exited pysourceparser in inconsistent state!\n");
  }

  if (yyextra->needsTermination)
  {
    endCodeLine(yyscanner);
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete yyextra->sourceFileDef;
    yyextra->sourceFileDef=0;
  }
  // write the tooltips
  TooltipManager::instance().writeTooltips(codeOutIntf);
  printlex(yy_flex_debug, FALSE, __FILE__, fileDef ? qPrint(fileDef->fileName()): NULL);
}

#if USE_STATE2STRING
#include "pycode.l.h"
#endif
