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

%option never-interactive
%option prefix="lexcodeYY"
%option noyywrap
%option reentrant
%option extra-type="struct lexcodeYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

#include <memory>
#include <stdio.h>

#include "config.h"
#include "doxygen.h"
#include "outputgen.h"
#include "outputlist.h"
#include "code.h"
#include "lexcode.h"
#include "filedef.h"
#include "message.h"
#include "debug.h"
#include "searchindex.h"

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct lexcodeYY_state
{
     OutputCodeList *code;
     CCodeParser ccodeParser;
     const char   *inputString;     //!< the code fragment as text
     int           inputPosition;   //!< read offset during parsing
     int           inputLines;      //!< number of line in the code fragment
     QCString      fileName;
     int           yyLineNr;        //!< current line number
     bool          insideCodeLine = false;

     bool          lineNumbers = FALSE;
     const Definition   *searchCtx;
     bool          collectXRefs = FALSE;

     int           lastContext = 0;
     int           lastCContext = 0;
     int           lastStringContext = 0;
     int           docBlockContext  = 0;
     int           lastPreLineCtrlContext = 0;
     int           lastRawStringContext = 0;
     int           curlyCount = 0;

     QCString      rulesPatternBuffer;
     QCString      CCodeBuffer;
     int           startCCodeLine = -1;
     int           roundCount = 0;
     bool          insideCode = FALSE;
     QCString      delimiter;
     QCString      docBlockName;
     size_t        fencedSize = 0;
     bool          nestedComment = false;

     bool          stripCodeComments;
     bool          exampleBlock;
     QCString      exampleName;
     QCString      classScope;

     std::unique_ptr<FileDef> exampleFileDef;
     const FileDef    *sourceFileDef = nullptr;
     const Definition *currentDefinition = nullptr;
     const MemberDef  *currentMemberDef = nullptr;
     bool              includeCodeFragment;
     const char       *currentFontClass = nullptr;
};

[[maybe_unused]] static const char *stateToString(int state);

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void startCodeLine(yyscan_t yyscanner);
static void endFontClass(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static void startFontClass(yyscan_t yyscanner,const char *s);
static int countLines(yyscan_t yyscanner);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void lineCount(yyscan_t yyscanner);
static void handleCCode(yyscan_t yyscanner);

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

nl              (\r\n|\r|\n)
ws              [ \t]
nws             [^ \t\n]
TopStart        "%top{"{nl}
TopEnd          "}"{nl}
LiteralStart    "%{"{nl}
LiteralEnd      "%}"{nl}
RulesStart      "%%"{nl}
RulesEnd        "%%"{nl}
RulesSharp      "<"[^>\n]*">"
RulesCurly      "{"[^{}\n]*"}"
StartSquare     "["
StartDouble     "\""
StartRound      "("
StartRoundQuest "(?"
EscapeRulesCharOpen  "\\["|"\\<"|"\\{"|"\\("|"\\\""|"\\ "|"\\\\"
EscapeRulesCharClose "\\]"|"\\>"|"\\}"|"\\)"
EscapeRulesChar      {EscapeRulesCharOpen}|{EscapeRulesCharClose}

CMD       ("\\"|"@")
BN        [ \t\n\r]
BL        [ \t\r]*"\n"
B         [ \t]
Bopt      {B}*
ID        [$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*
PRE       [pP][rR][eE]
CODE      [cC][oO][dD][eE]
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))
CHARCE    "[:"[^:]*":]"

  /* no comment start / end signs inside square brackets */
NCOMM [^/\*]
  // C start comment
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment
CPPC  "/\/"
  // doxygen start comment
DCOMM ("/\*!"|"/\**"|"/\/!"|"/\/\/")

  // Optional any character
ANYopt .*
  // Optional all but newline
NONLopt [^\n]*

%x DefSection
%x DefSectionLine
%x RulesSectionInit
%x RulesPattern
%x RulesDouble
%x RulesRoundDouble
%x RulesSquare
%x RulesRoundSquare
%x RulesRound
%x RulesRoundQuest
%x UserSection

%x TopSection
%x LiteralSection

%x COMMENT

%x SkipCurly
%x SkipCurlyEndDoc
%x PreLineCtrl
%x DocLine
%x DocBlock
%x DocCopyBlock
%x SkipString
%x RawString
%x SkipComment
%x SkipCxxComment
%x Comment

%%

<*>\x0d
<DefSection>^{TopStart}  {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           yyextra->lastContext = YY_START;
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           BEGIN (TopSection);
                         }
<DefSection>^{LiteralStart}   {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           yyextra->lastContext = YY_START;
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           BEGIN (LiteralSection);
                         }
<TopSection>^{TopEnd}    {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           BEGIN( yyextra->lastContext ) ;
                         }
<TopSection>.*{nl}       {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->yyLineNr++;
                         }
<LiteralSection>^{LiteralEnd}     {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           BEGIN( yyextra->lastContext ) ;
                         }
<LiteralSection>.*{nl}   {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->yyLineNr++;
                         }
<DefSection>{CPPC}.*{nl}   {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->yyLineNr++;
                         }
<DefSection>^{ws}*{CCS}   {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(COMMENT);
                         }
<COMMENT>{CCE}{ws}*{nl}   {
                           yyextra->CCodeBuffer+=yytext;
                           yyextra->yyLineNr++;
                           handleCCode(yyscanner);
                           BEGIN(yyextra->lastContext);
                         }
<COMMENT>{CCE}            {
                           yyextra->CCodeBuffer+=yytext;
                           handleCCode(yyscanner);
                           BEGIN(yyextra->lastContext);
                         }
<COMMENT>[^*\n]+         {
                           yyextra->CCodeBuffer += yytext;
                         }
<COMMENT>{CPPC}|{CCS}       {
                           yyextra->CCodeBuffer += yytext;
                         }
<COMMENT>{nl}            {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->yyLineNr++;
                         }
<COMMENT>.               {
                           yyextra->CCodeBuffer += yytext;
                         }
<DefSection>^{nl}        {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                         }
<DefSection>^{ws}.*{nl}  {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->yyLineNr++;
                         }
<DefSection>^{RulesStart} {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           BEGIN (RulesSectionInit);
                         }
<DefSection>^{nws}       {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           BEGIN(DefSectionLine);
                         }
<DefSectionLine>.*{nl}   {
                           codifyLines(yyscanner,yytext);
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           BEGIN(DefSection);
                         }
<RulesSectionInit,RulesPattern>^{RulesEnd} {
                           handleCCode(yyscanner);
                           codifyLines(yyscanner,yytext);
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           BEGIN (UserSection);
                         }
<RulesSectionInit>^{nws} {
                           handleCCode(yyscanner);
                           unput(*yytext);
                           BEGIN(RulesPattern);
                         }
<RulesSectionInit>{nl}   {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                         }
<RulesSectionInit>^{ws}.*{nl} {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                         }
<RulesPattern>"<<EOF>>"  {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<RulesPattern>{EscapeRulesChar} {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<RulesPattern>{RulesSharp} {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<RulesPattern>{RulesCurly} {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<RulesPattern>{StartDouble} {
                           yyextra->rulesPatternBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesDouble);
                        }
<RulesDouble,RulesRoundDouble>"\\\\"      {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesDouble,RulesRoundDouble>"\\\""     {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesDouble>"\""       {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN( yyextra->lastContext ) ;
                        }
<RulesRoundDouble>"\""  {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN(RulesRound) ;
                        }
<RulesDouble,RulesRoundDouble>.          {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesPattern>{StartSquare} {
                           yyextra->rulesPatternBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesSquare);
                        }
<RulesSquare,RulesRoundSquare>{CHARCE} {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesSquare,RulesRoundSquare>"\\["      |
<RulesSquare,RulesRoundSquare>"\\]"      {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesSquare>"]"        {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN(RulesPattern) ;
                        }
<RulesRoundSquare>"]"        {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN(RulesRound) ;
                        }
<RulesSquare,RulesRoundSquare>"\\\\"          {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesSquare,RulesRoundSquare>.          {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesPattern>{StartRoundQuest} {
                           yyextra->rulesPatternBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesRoundQuest);
                         }
<RulesRoundQuest>{nl}    {
                           yyextra->rulesPatternBuffer += yytext;
                           if (!yyextra->rulesPatternBuffer.isEmpty())
                           {
                             startFontClass(yyscanner,"stringliteral");
                             codifyLines(yyscanner,yyextra->rulesPatternBuffer.data());
                             yyextra->rulesPatternBuffer.clear();
                             endFontClass(yyscanner);
                           }
                         }
<RulesRoundQuest>[^)]    {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<RulesRoundQuest>")"     {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN(yyextra->lastContext);
                         }
<RulesPattern>{StartRound} {
                           yyextra->roundCount++;
                           yyextra->rulesPatternBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesRound);
                        }
<RulesRound>{RulesCurly} {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesRound>{StartSquare} {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN(RulesRoundSquare);
                        }
<RulesRound>{StartDouble} {
                           yyextra->rulesPatternBuffer += yytext;
                           BEGIN(RulesRoundDouble);
                        }
<RulesRound>{EscapeRulesChar} {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesRound>"("         {
                           yyextra->roundCount++;
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesRound>")"         {
                           yyextra->roundCount--;
                           yyextra->rulesPatternBuffer += yytext;
                           if (!yyextra->roundCount) BEGIN( yyextra->lastContext ) ;
                        }
<RulesRound>{nl}        {
                           yyextra->rulesPatternBuffer += yytext;
                           yyextra->yyLineNr++;
                        }
<RulesRound>{ws}        {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesRound>.           {
                           yyextra->rulesPatternBuffer += yytext;
                        }
<RulesPattern>{ws}+"|"  {
                           if (!yyextra->rulesPatternBuffer.isEmpty())
                           {
                             startFontClass(yyscanner,"stringliteral");
                             codifyLines(yyscanner,yyextra->rulesPatternBuffer);
                             yyextra->rulesPatternBuffer.clear();
                             endFontClass(yyscanner);
                           }
                           codifyLines(yyscanner,yytext);
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           yyextra->curlyCount = 0;
                           BEGIN(SkipCurly);
                         }
<RulesPattern>^{ws}*{nl} {
                           codifyLines(yyscanner,"\n");
                         }
<RulesPattern>^{ws}+     {
                           codifyLines(yyscanner,yytext);
                         }
<RulesPattern>({ws}|{nl}) {
                           unput(*yytext);
                           if (!yyextra->rulesPatternBuffer.isEmpty())
                           {
                             startFontClass(yyscanner,"stringliteral");
                             codifyLines(yyscanner,yyextra->rulesPatternBuffer);
                             yyextra->rulesPatternBuffer.clear();
                             endFontClass(yyscanner);
                           }
                           yyextra->startCCodeLine=yyextra->yyLineNr;
                           yyextra->curlyCount = 0;
                           BEGIN(SkipCurly);
                         }
<RulesPattern>"\\\\"     {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<RulesPattern>{CCS}       {
                           if (!yyextra->rulesPatternBuffer.isEmpty())
                           {
                             startFontClass(yyscanner,"stringliteral");
                             codifyLines(yyscanner,yyextra->rulesPatternBuffer);
                             yyextra->rulesPatternBuffer.clear();
                             endFontClass(yyscanner);
                           }
                           yyextra->CCodeBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(COMMENT);
                         }
<RulesPattern>.          {
                           yyextra->rulesPatternBuffer += yytext;
                         }
<SkipCurly>{B}*"#"{B}+[0-9]+{B}+/"\"" { /* line control directive */
                            yyextra->CCodeBuffer += yytext;
                            yyextra->lastPreLineCtrlContext = YY_START;
                            BEGIN( PreLineCtrl );
                          }
<PreLineCtrl>"\""[^\n\"]*"\"" {
                            yyextra->CCodeBuffer += yytext;
                          }
<PreLineCtrl>.            {
                            yyextra->CCodeBuffer += yytext;
                          }
<PreLineCtrl>\n           {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                            BEGIN( yyextra->lastPreLineCtrlContext );
                          }
<SkipCurly>"{"            {
                            yyextra->CCodeBuffer += yytext;
                                          ++yyextra->curlyCount ;
                          }
<SkipCurly>"}"/{BN}*{DCOMM}"<!--" | /* see bug710917 */
<SkipCurly>"}"            {
                            yyextra->CCodeBuffer += yytext;
                            lineCount(yyscanner);
                            if( yyextra->curlyCount )
                            {
                              --yyextra->curlyCount ;
                            }
                          }
<SkipCurly>"}"{BN}*{DCOMM}"<" {
                            yyextra->CCodeBuffer += yytext;
                            lineCount(yyscanner);
                            if ( yyextra->curlyCount )
                            {
                              --yyextra->curlyCount ;
                            }
                            else
                            {
                              yyextra->docBlockContext   = SkipCurlyEndDoc;
                              if (yytext[yyleng-3]=='/')
                              {
                                BEGIN( DocLine );
                              }
                              else
                              {
                                BEGIN( DocBlock );
                              }
                            }
                          }
<SkipCurly>\"             {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->lastStringContext=SkipCurly;
                            BEGIN( SkipString );
                          }
<SkipCurly>^{B}*"#"       {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->lastPreLineCtrlContext = YY_START;
                            BEGIN( PreLineCtrl );
                          }
<SkipCurly>{B}*{RAWBEGIN} {
                            QCString raw=QCString(yytext).stripWhiteSpace();
                            yyextra->delimiter = raw.mid(2);
                            yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                            yyextra->lastRawStringContext = YY_START;
                            yyextra->CCodeBuffer += yytext;
                            BEGIN(RawString);
                          }
<SkipCurly>[^\n#"'@\\/{}<]+ {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCurly>{CCS}           {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->lastCContext = YY_START;
                            BEGIN(SkipComment);
                          }
<SkipCurly>{CPPC}           {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->lastCContext = YY_START;
                            BEGIN(SkipCxxComment);
                          }
<SkipCurly>{CHARLIT}      {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCurly>\'             {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCurly>.              {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCurly>({CPPC}{B}*)?{CCS}"!" {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocBlock );
                          }
<SkipCurly>{CCS}"*"[*]+{BL}  {
                            bool javadocBanner = Config_getBool(JAVADOC_BANNER);
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                            if( javadocBanner )
                            {
                              yyextra->docBlockContext   = YY_START;
                              BEGIN( DocBlock );
                            }
                            else
                            {
                              BEGIN( Comment ) ;
                            }
                          }
<SkipCurly>({CPPC}{B}*)?{CCS}"*"/{NCOMM} {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocBlock );
                          }
<SkipCurly>{CPPC}"!"          {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocLine );
                          }
<SkipCurly>{CPPC}"/"/[^/]     {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocLine );
                          }

<SkipCurly>\n             {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                            if (yyextra->curlyCount<=0)
                            {
                              handleCCode(yyscanner);
                              BEGIN(RulesPattern);
                            }
                          }
<SkipString>\\.           {
                             yyextra->CCodeBuffer += yytext;
                          }
<SkipString>\"            {
                             yyextra->CCodeBuffer += yytext;
                             BEGIN( yyextra->lastStringContext );
                          }
<SkipString>{CCS}|{CCE}|{CPPC} {
                             yyextra->CCodeBuffer += yytext;
                           }
<SkipString>\n            {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                          }
<SkipString>.             {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCxxComment>.*"\\\n"  {  // line continuation
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                          }
<SkipCxxComment>{ANYopt}/\n     {
                            yyextra->CCodeBuffer += yytext;
                            BEGIN( yyextra->lastCContext ) ;
                          }
<Comment>{BN}+            {
                            yyextra->CCodeBuffer += yytext ;
                            lineCount(yyscanner);
                          }
<Comment>{CCS}             { yyextra->CCodeBuffer += yytext ; }
<Comment>{CPPC}             { yyextra->CCodeBuffer += yytext ; }
<Comment>{CMD}("code"|"verbatim"|"iliteral")       {
                            yyextra->insideCode=TRUE;
                            yyextra->CCodeBuffer += yytext ;
                          }
<Comment>{CMD}("endcode"|"endverbatim"|"endiliteral") {
                            yyextra->insideCode=FALSE;
                            yyextra->CCodeBuffer += yytext ;
                          }
<Comment>[^ \.\t\r\n\/\*]+ { yyextra->CCodeBuffer += yytext ; }
<Comment>{CCE}             {
                            yyextra->CCodeBuffer += yytext ;
                            if (!yyextra->insideCode) BEGIN( yyextra->lastContext ) ;
                          }
<Comment>.                { yyextra->CCodeBuffer += *yytext ; }

<SkipComment>{CPPC}|{CCS}    {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipComment>[^\*\n]+     {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipComment>\n           {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                          }
<SkipComment>{B}*{CCE}     {
                            yyextra->CCodeBuffer += yytext;
                            BEGIN( yyextra->lastCContext );
                          }
<SkipComment>"*"          {
                            yyextra->CCodeBuffer += yytext;
                          }
<RawString>{RAWEND}       {
                            yyextra->CCodeBuffer += yytext;
                            QCString delimiter = yytext+1;
                            delimiter=delimiter.left(delimiter.length()-1);
                            if (delimiter==yyextra->delimiter)
                            {
                              BEGIN(yyextra->lastRawStringContext);
                            }
                          }
<RawString>[^)\n]+        {
                            yyextra->CCodeBuffer += yytext;
                          }
<RawString>.              {
                            yyextra->CCodeBuffer += yytext;
                          }
<RawString>\n             {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                          }


  /* ---- Single line comments ------ */
<DocLine>[^\n]*"\n"[ \t]*{CPPC}[/!][<]? { // continuation of multiline C++-style comment
                            yyextra->CCodeBuffer += yytext;
                            lineCount(yyscanner);
                          }
<DocLine>{B}*{CPPC}"/"[/]+{Bopt}/"\n" { // ignore marker line (see bug700345)
                            yyextra->CCodeBuffer += yytext;
                            BEGIN( yyextra->docBlockContext );
                          }
<DocLine>{NONLopt}/"\n"{B}*{CPPC}[!/]{B}*{CMD}"}" { // next line is an end group marker, see bug 752712
                            yyextra->CCodeBuffer += yytext;
                            BEGIN( yyextra->docBlockContext );
                          }
<DocLine>{NONLopt}/"\n"      { // whole line
                            yyextra->CCodeBuffer += yytext;
                            BEGIN( yyextra->docBlockContext );
                          }

 /* ---- Comments blocks ------ */

<DocBlock>"*"*{CCE}        { // end of comment block
                            yyextra->CCodeBuffer += yytext;
                          BEGIN(yyextra->docBlockContext);
                                        }
<DocBlock>^{B}*"*"+/[^/]  {
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>^{B}*({CPPC})?{B}*"*"+/[^/a-z_A-Z0-9*] { // start of a comment line
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>^{B}*({CPPC}){B}* { // strip embedded C++ comments if at the start of a line
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>{CPPC}            { // slashes in the middle of a comment block
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>{CCS}            { // start of a new comment in the
                            // middle of a comment block
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>{CMD}("f$"|"f["|"f{"|"f(") {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockName=&yytext[1];
                            if (yyextra->docBlockName.at(1)=='[')
                            {
                              yyextra->docBlockName.at(1)=']';
                            }
                            if (yyextra->docBlockName.at(1)=='{')
                            {
                              yyextra->docBlockName.at(1)='}';
                            }
                            if (yyextra->docBlockName.at(1)=='(')
                            {
                              yyextra->docBlockName.at(1)=')';
                            }
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>{B}*"<"{PRE}">" {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockName="<pre>";
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>{CMD}"startuml"/[^a-z_A-Z0-9\-] { // verbatim type command (which could contain nested comments!)
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockName="uml";
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>{CMD}("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc"|"code")/[^a-z_A-Z0-9\-] { // verbatim command (which could contain nested comments!)
                            yyextra->CCodeBuffer += yytext;
                            yyextra->docBlockName=&yytext[1];
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                            yyextra->CCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            yyextra->docBlockName="~~~";
                            yyextra->fencedSize=pat.stripWhiteSpace().length();
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]* {
                            yyextra->CCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            yyextra->docBlockName="```";
                            yyextra->fencedSize=pat.stripWhiteSpace().length();
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>{B}*"<code>"    {
                            REJECT;
                          }
<DocBlock>[^@*~\/\\\n]+   { // any character that isn't special
                            yyextra->CCodeBuffer += yytext;
                          }
<DocBlock>\n              { // newline
                            yyextra->CCodeBuffer += yytext;
                            lineCount(yyscanner);
                          }
<DocBlock>.               { // command block
                            yyextra->CCodeBuffer += yytext;
                          }
 /* ---- Copy verbatim sections ------ */

<DocCopyBlock>"</"{PRE}">" { // end of a <pre> block
                            yyextra->CCodeBuffer += yytext;
                            if (yyextra->docBlockName=="<pre>")
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>"</"{CODE}">" { // end of a <code> block
                            yyextra->CCodeBuffer += yytext;
                            if (yyextra->docBlockName=="<code>")
                            {
                                            BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>[\\@]("f$"|"f]"|"f}"|"f)") {
                            yyextra->CCodeBuffer += yytext;
                            if (yyextra->docBlockName==&yytext[1])
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>[\\@]("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endmsc"|"enduml"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
                            yyextra->CCodeBuffer += yytext;
                            if (&yytext[4]==yyextra->docBlockName)
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{BN}+ { // start of a comment line
                            yyextra->CCodeBuffer += yytext;
                            if ((yyextra->docBlockName=="verbatim") || (yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              REJECT;
                            }
                            else
                            {
                              yyextra->CCodeBuffer += yytext;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{B}+"*"{BN}* { // start of a comment line with two *'s
                            if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              yyextra->CCodeBuffer += yytext;
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/({ID}|"(") { // Assume *var or *(... is part of source code (see bug723516)
                            if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              yyextra->CCodeBuffer += yytext;
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{BN}* { // start of a comment line with one *
                            if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              if (yyextra->nestedComment) // keep * it is part of the code
                              {
                                yyextra->CCodeBuffer += yytext;
                              }
                              else // remove * it is part of the comment block
                              {
                            yyextra->CCodeBuffer += yytext;
                              }
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                            yyextra->CCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            if (yyextra->fencedSize==pat.stripWhiteSpace().length())
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"```"[`]* {
                            yyextra->CCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            if (yyextra->fencedSize==pat.stripWhiteSpace().length())
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>[^\<@/\*\]~\$\\\n]+ { // any character that is not special
                            yyextra->CCodeBuffer += yytext;
                          }
<DocCopyBlock>{CCS}|{CCE}|{CPPC} {
                            if (yytext[1]=='*')
                            {
                              yyextra->nestedComment=TRUE;
                            }
                            else if (yytext[0]=='*')
                            {
                              yyextra->nestedComment=FALSE;
                            }
                            yyextra->CCodeBuffer += yytext;
                          }
<DocCopyBlock>\n          { // newline
                            yyextra->CCodeBuffer += yytext;
                            lineCount(yyscanner);
                          }
<DocCopyBlock>.           { // any other character
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCurlyEndDoc>"}"{BN}*{DCOMM}"<" { // desc is followed by another one
                            yyextra->docBlockContext   = SkipCurlyEndDoc;
                            yyextra->CCodeBuffer += yytext;
                            if (yytext[yyleng-3]=='/')
                            {
                              BEGIN( DocLine );
                            }
                            else
                            {
                              BEGIN( DocBlock );
                            }
                          }
<SkipCurlyEndDoc>"}"      {
                            yyextra->CCodeBuffer += yytext;
                            BEGIN(SkipCurly);
                          }

<UserSection>.*{nl}       {
                            yyextra->CCodeBuffer += yytext;
                            yyextra->yyLineNr++;
                          }
 /*
<*>.  { fprintf(stderr,"Lex code scanner Def rule for %s: #%s#\n",stateToString(YY_START),yytext);}
<*>{nl}  { fprintf(stderr,"Lex code scanner Def rule for newline %s: #%s#\n",stateToString(YY_START),yytext); yyextra->yyLineNr++;}
 */
<*><<EOF>>                {
                            handleCCode(yyscanner);
                            yyterminate();
                          }
%%

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex.enabled())
  {
    if (yyextra->searchCtx)
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),false);
    }
    else
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->sourceFileDef,anchor,true);
    }
  }
}

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is true. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->sourceFileDef && yyextra->lineNumbers)
  {
    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);

    if (!yyextra->includeCodeFragment && d)
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->classScope = d->name();
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                            yyextra->currentMemberDef->getOutputFileBase(),
                            yyextra->currentMemberDef->anchor(),yyextra->yyLineNr,
                            !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else
      {
        yyextra->code->writeLineNumber(d->getReference(),
                            d->getOutputFileBase(),
                            QCString(),yyextra->yyLineNr,
                            !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr,
                                     !yyextra->includeCodeFragment);
    }
  }

  yyextra->code->startCodeLine(yyextra->yyLineNr);
  yyextra->insideCodeLine = true;


  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
}

static void endFontClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=0;
  }
}

static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
  yyextra->insideCodeLine = false;
}

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

static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty()) return;
  const char *p=text.data(),*sp=p;
  bool done=false;
  while (!done)
  {
    sp=p;
    char c = 0;
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
      done=true;
    }
  }
  yyextra->startCCodeLine = yyextra->yyLineNr;
}

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  if (!yyextra->currentFontClass || !s || strcmp(yyextra->currentFontClass,s))
  {
    endFontClass(yyscanner);
    yyextra->code->startFontClass(s);
    yyextra->currentFontClass=s;
  }
}

/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c = 0;
  int count=1;
  while ((c=*p))
  {
    p++ ;
    if (c=='\n') count++;
  }
  if (p>yyextra->inputString && *(p-1)!='\n')
  { // last line does not end with a \n, so we add an extra
    count++;
  }
  return count;
}

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
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

static void lineCount(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (const char *p = yytext ; *p ; ++p )
  {
    if (*p=='\n')
    {
      yyextra->yyLineNr++;
    }
  }
}

static void handleCCode(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->CCodeBuffer.isEmpty()) return;

  yyextra->ccodeParser.setInsideCodeLine(yyextra->insideCodeLine);
  yyextra->ccodeParser.parseCode(*yyextra->code,
               yyextra->classScope,
               yyextra->CCodeBuffer,
               SrcLangExt::Cpp,
               yyextra->stripCodeComments,
               yyextra->exampleBlock,
               yyextra->exampleName,
               yyextra->sourceFileDef,
               yyextra->startCCodeLine,
               -1, /* endLine will be calculated in called routine */
               yyextra->includeCodeFragment,
               yyextra->currentMemberDef,
               yyextra->lineNumbers,
               yyextra->searchCtx,
               yyextra->collectXRefs
              );
  yyextra->CCodeBuffer.clear();
  yyextra->insideCodeLine = yyextra->ccodeParser.insideCodeLine();
  yyextra->yyLineNr--;
  codifyLines(yyscanner,"\n");
}

// public interface -----------------------------------------------------------

struct LexCodeParser::Private
{
  yyscan_t yyscanner;
  lexcodeYY_state state;
};

LexCodeParser::LexCodeParser() : p(std::make_unique<Private>())
{
  lexcodeYYlex_init_extra(&p->state, &p->yyscanner);
#ifdef FLEX_DEBUG
  lexcodeYYset_debug(Debug::isFlagSet(Debug::Lex_lexcode)?1:0,p->yyscanner);
#endif
  resetCodeParserState();
}

LexCodeParser::~LexCodeParser()
{
  lexcodeYYlex_destroy(p->yyscanner);
}

void LexCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->currentDefinition = nullptr;
  yyextra->currentMemberDef = nullptr;
}

void LexCodeParser::parseCode(OutputCodeList &codeOutIntf,
               const QCString &scopeName,
               const QCString &input,
               SrcLangExt,
               bool stripCodeComments,
               bool isExampleBlock,
               const QCString &exampleName,
               const FileDef *fileDef,
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

  if (input.isEmpty()) return;

  DebugLex debugLex(Debug::Lex_lexcode, __FILE__, fileDef ? qPrint(fileDef->fileName()) : nullptr);

  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
  yyextra->fileName      = fileDef ? fileDef->fileName():"";
  yyextra->currentFontClass = nullptr;
  yyextra->insideCodeLine = false;

  yyextra->classScope=scopeName;
  yyextra->currentMemberDef=memberDef;
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

  yyextra->startCCodeLine = yyextra->yyLineNr;
  yyextra->stripCodeComments = stripCodeComments;
  yyextra->exampleBlock  = isExampleBlock;
  yyextra->exampleName   = exampleName;
  yyextra->sourceFileDef = fileDef;
  yyextra->lineNumbers   = fileDef!=0 && showLineNumbers;

  if (isExampleBlock && fileDef==0)
  {
    // create a dummy filedef for the example
    yyextra->exampleFileDef = createFileDef(QCString(),!exampleName.isEmpty() ? exampleName : QCString("generated"));
    yyextra->sourceFileDef = yyextra->exampleFileDef.get();
  }

  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }

  yyextra->includeCodeFragment = inlineFragment;
  // Starts line 1 on the output
  startCodeLine(yyscanner);

  lexcodeYYrestart( nullptr, yyscanner );
  BEGIN( DefSection );
  lexcodeYYlex(yyscanner);

  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
  if (yyextra->exampleFileDef)
  {
    // delete the temporary file definition used for this example
    yyextra->exampleFileDef.reset();
    yyextra->sourceFileDef=0;
  }
}

//---------------------------------------------------------------------------------

#include "lexcode.l.h"
