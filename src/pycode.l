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
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
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
#include "debug.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct pycodeYY_state
{
  std::unordered_map< std::string, ScopedTypeVariant > codeClassMap;
  QCString      curClassName;
  StringVector  curClassBases;

  OutputCodeList * code = nullptr;
  const char *  inputString = nullptr;     //!< the code fragment as text
  int           inputPosition = 0;   //!< read offset during parsing
  QCString      fileName;
  const char *  currentFontClass = nullptr;
  bool          insideCodeLine = FALSE;
  const Definition *searchCtx = nullptr;
  bool          collectXRefs = FALSE;
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  std::unique_ptr<FileDef> exampleFileDef;
  const FileDef *    sourceFileDef = nullptr;
  const Definition * currentDefinition = nullptr;
  const MemberDef *  currentMemberDef = nullptr;
  bool          includeCodeFragment = FALSE;
  QCString      realScope;
  int           bodyCurlyCount = 0;
  bool          searchingForBody = FALSE;
  QCString      classScope;
  int           paramParens = 0;

  bool          insideBody = false;
  bool          exampleBlock = FALSE;
  QCString      exampleName;

  QCString      type;
  QCString      name;

  bool          doubleStringIsDoc = FALSE;
  bool          doubleQuote = FALSE;
  bool          noSuiteFound = FALSE;
  int           stringContext = 0;
  bool          insideSpecialComment = false;

  std::stack<yy_size_t> indents;  //!< Tracks indentation levels for scoping in python

  QCString      docBlock;     //!< contents of all lines of a documentation block
  bool          endComment = FALSE;
  VariableContext theVarContext;
  CallContext theCallContext;
  SymbolResolver symbolResolver;
  TooltipManager tooltipManager;
  std::vector<const Definition *> foldStack;
};


[[maybe_unused]] static const char *stateToString(int state);

static void startCodeLine(yyscan_t yyscanner);
static int countLines(yyscan_t yyscanner);
static void setCurrentDoc(yyscan_t yyscanner, const QCString &anchor);
static void addToSearchIndex(yyscan_t yyscanner, const QCString &text);
static const ClassDef *stripClassName(yyscan_t yyscanner,const QCString &s,Definition *d);
static void codify(yyscan_t yyscanner,const QCString &text);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void writeMultiLineCodeLink(yyscan_t yyscanner, OutputCodeList &ol, const Definition *d, const QCString &text);
static void startFontClass(yyscan_t yyscanner,const char *s,bool specialComment=false);
static void endFontClass(yyscan_t yyscanner,bool specialComment=false);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static bool getLinkInScope(yyscan_t yyscanner, const QCString &c, const QCString &m,
                           const QCString &memberText, OutputCodeList &ol, const QCString &text);
static bool getLink(yyscan_t yyscanner, const QCString &className, const QCString &memberName,
                    OutputCodeList &ol, const QCString &text=QCString());
static void generateClassOrGlobalLink(yyscan_t yyscanner, OutputCodeList &ol,
                                      const QCString &clName, bool typeOnly=FALSE);
static void generateFunctionLink(yyscan_t yyscanner, OutputCodeList &ol,
                                const QCString &funcName);
static bool findMemberLink(yyscan_t yyscanner, OutputCodeList &ol,
                           const Definition *sym, const QCString &symName);
static void findMemberLink(yyscan_t yyscanner, OutputCodeList &ol,
                           const QCString &symName);
static void incrementFlowKeyWordCount(yyscan_t yyscanner);
static void adjustScopesAndSuites(yyscan_t yyscanner,unsigned indentLength);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static inline void pop_state(yyscan_t yyscanner);

#if 0 // TODO: call me to store local variables and get better syntax highlighting, see code.l
static void addVariable(yyscan_t yyscanner, QCString type, QCString name);
#endif

//-------------------------------------------------------------------

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

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
LONGSTRINGBLOCK   ({LONGSTRINGITEM}|{SMALLQUOTE})

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

                                      yyextra->curClassBases.emplace_back(yytext);
                                      yyextra->insideBody = true;
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                      yyextra->insideBody = false;
                                      // codify(yyscanner,yytext);
                                    }

    ":"                             {
                                      codify(yyscanner,yytext);

                                      // Assume this will
                                      // be a one-line suite;
                                      // found counter-example
                                      // in SuiteStart.

                                      // Push a class scope
                                      ScopedTypeVariant var(yyextra->curClassName);
                                      for (const auto &s : yyextra->curClassBases)
                                      {
                                        const ClassDef *baseDefToAdd = nullptr;
                                        // find class in the local scope
                                        auto it = yyextra->codeClassMap.find(s);
                                        if (it != yyextra->codeClassMap.end())
                                        {
                                          baseDefToAdd = toClassDef(it->second.globalDef());
                                        }
                                        // Try to find class in global scope
                                        if (baseDefToAdd==nullptr)
                                        {
                                          baseDefToAdd=yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,s.c_str(),true);
                                        }

                                        if (baseDefToAdd && baseDefToAdd->name()!=yyextra->curClassName)
                                        {
                                          var.localDef()->insertBaseClass(baseDefToAdd->name());
                                        }
                                      }
                                      yyextra->codeClassMap.emplace(yyextra->curClassName.str(),std::move(var));

                                      // Reset class-parsing variables.
                                      yyextra->curClassName.clear();
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
                                      incrementFlowKeyWordCount(yyscanner);
                                      startFontClass(yyscanner,"keywordflow");
                                      codify(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }
    ({IDENTIFIER}".")*{IDENTIFIER}/"("  {
                                      yyextra->insideBody = true;
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                      yyextra->insideBody = false;
                                    }
    ({IDENTIFIER}".")+{IDENTIFIER}  {
                                      yyextra->insideBody = true;
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,TRUE);
                                      yyextra->insideBody = false;
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
                                      incrementFlowKeyWordCount(yyscanner);
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
    [ \t]+                          {
                                      codify(yyscanner,yytext);
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
                                      startFontClass(yyscanner,"comment",true);
                                      codifyLines(yyscanner,yyextra->docBlock);
                                      endFontClass(yyscanner,true);
                                      unput(*yytext);
                                      pop_state(yyscanner);
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
                                    if (YY_START==DocBlock)
                                    {
                                      startFontClass(yyscanner,"comment",true);
                                      codifyLines(yyscanner,yyextra->docBlock);
                                      endFontClass(yyscanner,true);
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

static int yyread(yyscan_t yyscanner, char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  int c=0;
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

      yyextra->currentMemberDef=nullptr;
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
  char c=0;
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
  }
  return count;
}

//-------------------------------------------------------------------------------

static void setCurrentDoc(yyscan_t yyscanner, const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex.enabled())
  {
    if (yyextra->searchCtx)
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),FALSE);
    }
    else
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->sourceFileDef,anchor,TRUE);
    }
  }
}

//-------------------------------------------------------------------------------

static void addToSearchIndex(yyscan_t /* yyscanner */, const QCString &text)
{
  if (Doxygen::searchIndex.enabled())
  {
    Doxygen::searchIndex.addWord(text,FALSE);
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
    const ClassDef *cd=nullptr;
    if (!yyextra->classScope.isEmpty())
    {
      cd=yyextra->symbolResolver.resolveClass(d,yyextra->classScope+"::"+clName,true);
    }
    if (cd==nullptr)
    {
      cd=yyextra->symbolResolver.resolveClass(d,clName,true);
    }
    if (cd)
    {
      return cd;
    }
  }

  return nullptr;
}

//-------------------------------------------------------------------------------
static void codeFolding(yyscan_t yyscanner,const Definition *d)
{
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    while (!yyextra->foldStack.empty())
    {
      const Definition *dd = yyextra->foldStack.back();
      if (dd->getEndBodyLine()+1==yyextra->yyLineNr) // +1 to close the section after the end of the body
      {
        yyextra->code->endFold();
        //printf("%d:   end codeFolding for %s [%d..%d]\n",yyextra->yyLineNr,qPrint(dd->name()),dd->getStartDefLine(),dd->getEndBodyLine());
        yyextra->foldStack.pop_back();
      }
      else
      {
        break;
      }
    }
    if (d)
    {
      int startLine = d->getStartDefLine();
      int endLine   = d->getEndBodyLine();
      if (endLine!=-1 && startLine!=endLine &&
          // since the end of a section is closed after the last line, we need to avoid starting a
          // new section if the previous section ends at the same line, i.e. something like
          // struct X {
          // ...
          // }; struct S {  <- start of S and end of X at the same line
          // ...
          // };
          (yyextra->foldStack.empty() || yyextra->foldStack.back()->getEndBodyLine()!=startLine))
      {
        //printf("%d: start codeFolding for %s [%d..%d]\n",yyextra->yyLineNr,qPrint(d->name()),d->getStartDefLine(),d->getEndBodyLine());
        yyextra->code->startFold(yyextra->yyLineNr,"", "");
        yyextra->foldStack.push_back(d);
      }
    }
  }
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
      yyextra->insideBody = false;
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
        codeFolding(yyscanner,yyextra->currentMemberDef);
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),yyextra->yyLineNr,
                                !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        codeFolding(yyscanner,d);
        yyextra->code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                QCString(),yyextra->yyLineNr,
                                !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else
      {
        codeFolding(yyscanner,nullptr);
      }
    }
    else
    {
      codeFolding(yyscanner,nullptr);
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr,
                                     !yyextra->includeCodeFragment);
    }
  }
  yyextra->code->startCodeLine(yyextra->yyLineNr);
  yyextra->insideCodeLine=true;

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
  yyextra->insideCodeLine=false;
}

//-------------------------------------------------------------------------------

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *fc = yyextra->currentFontClass;
  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
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
                  OutputCodeList &ol,
                  const Definition *d,
                  const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty()) return;
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  yyextra->tooltipManager.addTooltip(d);
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
    char c=0;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,QCString(sp,p-sp-1),tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}

//-------------------------------------------------------------------------------

static void startFontClass(yyscan_t yyscanner,const char *s,bool specialComment)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (specialComment)
  {
    yyextra->code->startSpecialComment();
    yyextra->insideSpecialComment = true;
  }
  // if font class is already set don't stop and start it.
  if (qstrcmp(yyextra->currentFontClass,s)!=0)
  {
    endFontClass(yyscanner);
    yyextra->code->startFontClass(s);
    yyextra->currentFontClass=s;
  }
}

//-------------------------------------------------------------------------------

static void endFontClass(yyscan_t yyscanner,bool specialComment)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=nullptr;
  }
  if (specialComment && yyextra->insideSpecialComment)
  {
    yyextra->code->endSpecialComment();
    yyextra->insideSpecialComment=false;
  }
}

//-------------------------------------------------------------------------------

static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty()) return;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  const char *p=text.data(),*sp=p;
  char c=0;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      size_t l = static_cast<size_t>(p-sp-1);
      std::string tmp(sp,l);
      yyextra->code->codify(tmp.c_str());
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
                           OutputCodeList &ol,
                           const QCString &text
                          )
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  GetDefInput input(c,m,"()");
  input.currentFile = yyextra->sourceFileDef;
  input.insideCode = true;
  GetDefResult result = getDefs(input);
  //printf("Trying '%s'::'%s'\n",qPrint(c),qPrint(m));
  if (result.found && result.md->isLinkable())
  {
    const Definition *d = result.md->getOuterScope()==Doxygen::globalScope ?
                          result.md->getBodyDef() : result.md->getOuterScope();
    //printf("Found! d=%s\n",d?qPrint(d->name()):"<none>");
    if (result.md->getGroupDef()) d = result.md->getGroupDef();
    if (d && d->isLinkable())
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,result.md->typeString(),result.md->getOuterScope())));
      //printf("yyextra->currentDefinition=%p yyextra->currentMemberDef=%p\n",
      //        yyextra->currentDefinition,yyextra->currentMemberDef);

      if (yyextra->currentDefinition && yyextra->currentMemberDef && yyextra->collectXRefs && yyextra->insideBody)
      {
        addDocCrossReference(yyextra->currentMemberDef,result.md);
      }
      //printf("d->getReference()='%s' d->getOutputBase()='%s' name='%s' member name='%s'\n",qPrint(d->getReference()),qPrint(d->getOutputFileBase()),qPrint(d->name()),qPrint(md->name()));

      writeMultiLineCodeLink(yyscanner,ol,result.md, !text.isEmpty() ? text : memberText);
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
                    OutputCodeList &ol,
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
                                      OutputCodeList &ol,
                                      const QCString &clName,
                                      bool typeOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString className=clName;

  // Don't do anything for empty text
  if (className.isEmpty()) return;

  DBG_CTX((stderr,"generateClassOrGlobalLink(className=%s)\n",qPrint(className)));

  const ScopedTypeVariant *lcd = nullptr;
  const ClassDef *cd=nullptr;             // Class def that we may find 
  const MemberDef *md=nullptr;            // Member def that we may find 
  //bool isLocal=FALSE;

  if ((lcd=yyextra->theVarContext.findVariable(className))==nullptr) // not a local variable
  {
    const Definition *d = yyextra->currentDefinition;
    QCString scope = substitute(className,".","::");

    cd = yyextra->symbolResolver.resolveClass(d,substitute(className,".","::"),true);
    md = yyextra->symbolResolver.getTypedef();

    DBG_CTX((stderr,"d=%s yyextra->sourceFileDef=%s\n",
        d?qPrint(d->displayName()):"<null>",
        yyextra->currentDefinition?qPrint(yyextra->currentDefinition->displayName()):"<null>"));
    DBG_CTX((stderr,"is found as a type %s\n",cd?qPrint(cd->name()):"<null>"));

    if (cd==nullptr && md==nullptr) // also see if it is variable or enum or enum value
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
    if (!lcd->isDummy())
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
          yyextra->currentMemberDef && yyextra->collectXRefs && yyextra->insideBody)
      {
        addDocCrossReference(yyextra->currentMemberDef,md);
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
              yyextra->currentMemberDef && yyextra->collectXRefs && yyextra->insideBody)
          {
            addDocCrossReference(yyextra->currentMemberDef,mmd);
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
                yyextra->currentMemberDef && yyextra->collectXRefs && yyextra->insideBody)
            {
              addDocCrossReference(yyextra->currentMemberDef,mmd);
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
                                OutputCodeList &ol,
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
                           OutputCodeList &ol,
                           const Definition *sym,
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
        addDocCrossReference(yyextra->currentMemberDef,toMemberDef(sym));
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
                           OutputCodeList &ol,
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
    const auto &v = Doxygen::symbolMap->find(symName);
    for (const auto &p : v)
    {
      if (findMemberLink(yyscanner,ol,p,symName)) return;
    }
  }
  //printf("sym %s not found\n",&yytext[5]);
  codify(yyscanner,symName);
}

static void incrementFlowKeyWordCount(yyscan_t yyscanner)
{
  std::lock_guard<std::mutex> lock(Doxygen::countFlowKeywordsMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
  {
    MemberDefMutable *md = toMemberDefMutable(const_cast<MemberDef*>(yyextra->currentMemberDef));
    if (md)
    {
      md->incrementFlowKeyWordCount();
    }
  }
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
  pycodeYYset_debug(Debug::isFlagSet(Debug::Lex_pycode)?1:0,p->yyscanner);
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
  yyextra->currentDefinition = nullptr;
  yyextra->currentMemberDef = nullptr;
  yyextra->doubleStringIsDoc = FALSE;
  yyextra->paramParens = 0;
  while (!yyextra->indents.empty()) yyextra->indents.pop();
  BEGIN( Body );
}

void PythonCodeParser::parseCode(OutputCodeList &codeOutIntf,
    const QCString &/* scopeName */,
    const QCString &input,
    SrcLangExt /*lang*/,
    bool stripCodeComments,
    bool isExampleBlock,
    const QCString &exampleName,
    const FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    const MemberDef * /* memberDef */,
    bool /* showLineNumbers */,
    const Definition *searchCtx,
    bool collectXRefs
    )
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  //printf("***parseCode(%s)\n",qPrint(input));

  codeOutIntf.stripCodeComments(stripCodeComments);
  if (input.isEmpty()) return;
  DebugLex debugLex(Debug::Lex_pycode, __FILE__, fileDef ? qPrint(fileDef->fileName()): nullptr);
  yyextra->fileName      = fileDef ? fileDef->fileName():"";
  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
  yyextra->currentFontClass = nullptr;
  yyextra->insideCodeLine = FALSE;
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
  yyextra->foldStack.clear();
  yyextra->insideSpecialComment = false;

  if (yyextra->exampleBlock && fileDef==nullptr)
  {
    // create a dummy filedef for the example
    yyextra->exampleFileDef = createFileDef("",(!exampleName.isEmpty()?qPrint(exampleName):"generated"));
    yyextra->sourceFileDef = yyextra->exampleFileDef.get();
  }
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }

  yyextra->includeCodeFragment = inlineFragment;
  // Starts line 1 on the output
  startCodeLine(yyscanner);

  pycodeYYrestart(nullptr,yyscanner);

  pycodeYYlex(yyscanner);

  if (!yyextra->indents.empty())
  {
    // printf("Exited pysourceparser in inconsistent state!\n");
  }

  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    while (!yyextra->foldStack.empty())
    {
      yyextra->code->endFold();
      yyextra->foldStack.pop_back();
    }
  }
  if (yyextra->exampleFileDef)
  {
    // delete the temporary file definition used for this example
    yyextra->exampleFileDef.reset();
    yyextra->sourceFileDef=nullptr;
  }
  // write the tooltips
  yyextra->tooltipManager.writeTooltips(codeOutIntf);
}

static inline void pop_state(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if ( yyg->yy_start_stack_ptr <= 0 )
    warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected statement '%s'",yytext );
  else
    yy_pop_state(yyscanner);
}

#include "pycode.l.h"
