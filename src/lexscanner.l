/*****************************************************************************
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
%option prefix="lexscannerYY"
%option reentrant
%option extra-type="struct lexscannerYY_state *"
%option noyywrap

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
#include <vector>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "config.h"
#include "lexscanner.h"
#include "entry.h"
#include "message.h"
#include "util.h"
#include "scanner.h"
#include "debug.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define repeatChar(chr, cnt) QCString().fill(chr,cnt)

struct lexscannerYY_state
{
  COutlineParser   cOutlineParser;
  const char *     inputString = nullptr;
  int              inputPosition = 0;

  int              lastContext = 0;
  int              lastCContext = 0;
  int              lastStringContext = 0;
  int              docBlockContext  = 0;
  int              lastPreLineCtrlContext = 0;
  int              lastRawStringContext = 0;
  int              curlyCount = 0;

  bool             insideCode = FALSE;
  QCString         delimiter;
  QCString         docBlockName;
  size_t           fencedSize = 0;
  bool             nestedComment = false;

  QCString         prefix = "yy";
  bool             reentrant = false;
  bool             bison_bridge = false;
  bool             bison_locations = false;
  QCString         cCodeBuffer;
  int              roundCount = 0;

  QCString         fileName;
  ClangTUParser   *clangParser = nullptr;

  std::shared_ptr<Entry> current;
  std::shared_ptr<Entry> current_root;
  SrcLangExt language;
};

[[maybe_unused]] static const char *stateToString(int state);
//-----------------------------------------------------------------------------

// forward declarations for stateful functions
static void handleCCode(yyscan_t yyscanner);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);

/* ----------------------------------------------------------------- */
#undef  YY_INPUT
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
Option          "%option"
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
%x Option
%x OptPrefix
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
<DefSection>{Option}     {
                           BEGIN (Option);
                         }
<Option>"prefix"{ws}*"="{ws}* {
                           BEGIN (OptPrefix);
                         }
<OptPrefix>"\""[^\"]*"\"" {
                           yyextra->prefix = yytext;
                           yyextra->prefix = yyextra->prefix.mid(1,yyleng-2);
                           BEGIN (Option);
                         }
<Option>"reentrant"      {
                           yyextra-> reentrant = true;
                         }
<Option>"bison-bridge"   {
                           yyextra-> bison_bridge = true;
                         }
<Option>"bison-locations" {
                           yyextra-> bison_bridge = true;
                           yyextra-> bison_locations = true;
                         }
<Option>{nws}+
<Option>{ws}+
<Option>{nl}             {
                           yyextra->cCodeBuffer += yytext;
                           BEGIN (DefSection);
                         }
<DefSection>^{RulesStart} {
                           {
                             bool fill = false;
                             yyextra->cCodeBuffer += "int " + yyextra->prefix + "lex (";
                             if (yyextra->bison_bridge )
                             {
                               if (fill) yyextra->cCodeBuffer += ",";
                               yyextra->cCodeBuffer += "YYSTYPE * yylval_param";
                               fill = true;
                             }
                             if (yyextra->bison_locations)
                             {
                               if (fill) yyextra->cCodeBuffer += ",";
                               yyextra->cCodeBuffer += "YYLTYPE * yylloc_param";
                               fill = true;
                             }
                             if (yyextra->reentrant)
                             {
                               if (fill) yyextra->cCodeBuffer += ",";
                               yyextra->cCodeBuffer += "yyscan_t yyscanner";
                               fill = true;
                             }
                             if (!yyextra->bison_bridge && !yyextra->bison_locations && !yyextra->reentrant)
                             {
                               yyextra->cCodeBuffer += "void";
                             }
                             yyextra->cCodeBuffer += ") {\n";
                           }
                           BEGIN (RulesSectionInit);
                         }
<DefSection>^{TopStart}  {
                           yyextra->cCodeBuffer += "\n";
                           yyextra->lastContext = YY_START;
                           BEGIN (TopSection);
                         }
<DefSection>^{LiteralStart}   {
                           yyextra->cCodeBuffer += "\n";
                           yyextra->lastContext = YY_START;
                           BEGIN (LiteralSection);
                         }
<TopSection>^{TopEnd}    {
                           yyextra->cCodeBuffer += "\n";
                           BEGIN( yyextra->lastContext ) ;
                         }
<TopSection>.*{nl}       {
                           yyextra->cCodeBuffer += yytext;
                         }
<LiteralSection>^{LiteralEnd}     {
                           yyextra->cCodeBuffer += "\n";
                           BEGIN( yyextra->lastContext ) ;
                         }
<LiteralSection>.*{nl}   {
                           yyextra->cCodeBuffer += yytext;
                         }
<DefSection>^{nws}       {
                           BEGIN(DefSectionLine);
                         }
<DefSection>{CPPC}.*{nl}   {
                           yyextra->cCodeBuffer += yytext;
                         }
<DefSection>^{ws}*{CCS}   {
                           yyextra->cCodeBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(COMMENT);
                         }
<COMMENT>{CCE}{ws}*{nl}   {
                           yyextra->cCodeBuffer+=yytext;
                           BEGIN(yyextra->lastContext);
                         }
<COMMENT>{CCE}            {
                           yyextra->cCodeBuffer+=yytext;
                           BEGIN(yyextra->lastContext);
                         }
<COMMENT>[^*\n]+         {
                           yyextra->cCodeBuffer += yytext;
                         }
<COMMENT>{CPPC}|{CCS}       {
                           yyextra->cCodeBuffer += yytext;
                         }
<COMMENT>{nl}              {
                           yyextra->cCodeBuffer += yytext;
                         }
<COMMENT>.               {
                           yyextra->cCodeBuffer += yytext;
                         }
<DefSection>^{nl}        {
                           yyextra->cCodeBuffer += "\n";
                         }
<DefSection>^{ws}.*{nl}  {
                           yyextra->cCodeBuffer += yytext;
                         }
<DefSectionLine>.*{nl}   {
                           yyextra->cCodeBuffer += "\n";
                           BEGIN(DefSection);
                         }
<RulesSectionInit,RulesPattern>^{RulesEnd} {
                           yyextra->cCodeBuffer += "}\n";
                           BEGIN (UserSection);
                         }
<RulesSectionInit>^{nws}     {
                           unput(*yytext);
                           BEGIN(RulesPattern);
                         }
<RulesSectionInit>^{ws}.*{nl} {
                           yyextra->cCodeBuffer += yytext;
                         }
<RulesSectionInit>^{nl}  {
                           yyextra->cCodeBuffer += yytext;
                         }
<RulesPattern>"<<EOF>>"  {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{EscapeRulesChar} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{RulesSharp} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{RulesCurly} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{StartDouble} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesDouble);
                        }
<RulesDouble,RulesRoundDouble>"\\\\" {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesDouble,RulesRoundDouble>"\\\"" {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesDouble>"\""       {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN( yyextra->lastContext ) ;
                        }
<RulesRoundDouble>"\""  {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRound) ;
                        }
<RulesDouble,RulesRoundDouble>. {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesPattern>{StartSquare} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesSquare);
                        }
<RulesSquare,RulesRoundSquare>{CHARCE} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesSquare,RulesRoundSquare>"\\[" |
<RulesSquare,RulesRoundSquare>"\\]" {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesSquare>"]"        {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesPattern);
                        }
<RulesRoundSquare>"]"   {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRound) ;
                        }
<RulesSquare,RulesRoundSquare>"\\\\" {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesSquare,RulesRoundSquare>. {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesPattern>{StartRoundQuest} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesRoundQuest);
                         }
<RulesRoundQuest>{nl}    {
                           yyextra->cCodeBuffer += "\n";
                         }
<RulesRoundQuest>[^)]    {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesRoundQuest>")"     {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(yyextra->lastContext);
                         }
<RulesPattern>{StartRound} {
                           yyextra->roundCount++;
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesRound);
                        }
<RulesRound>{RulesCurly} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>{StartSquare} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRoundSquare);
                        }
<RulesRound>{StartDouble} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRoundDouble);
                        }
<RulesRound>{EscapeRulesChar} {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>"("         {
                           yyextra->roundCount++;
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>")"         {
                           yyextra->roundCount--;
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           if (!yyextra->roundCount) BEGIN( yyextra->lastContext ) ;
                        }
<RulesRound>{nl}        {
                           yyextra->cCodeBuffer += "\n";
                        }
<RulesRound>{ws}        {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>.           {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesPattern>{ws}+"|"  {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->curlyCount = 0;
                           BEGIN(SkipCurly);
                         }
<RulesPattern>^{ws}*{nl} {
                           yyextra->cCodeBuffer += "\n";
                         }
<RulesPattern>^{ws}+     {
                         }

<RulesPattern>({ws}|{nl}) {
                           unput(*yytext);
                           yyextra->curlyCount = 0;
                           BEGIN(SkipCurly);
                         }
<RulesPattern>"\\\\"     {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{CCS}      {
                           yyextra->cCodeBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(COMMENT);
                         }
<RulesPattern>.          {
                           yyextra->cCodeBuffer += repeatChar(' ', yyleng);
                         }
<SkipCurly>{B}*"#"{B}+[0-9]+{B}+/"\"" { /* line control directive */
                            yyextra->cCodeBuffer += yytext;
                            yyextra->lastPreLineCtrlContext = YY_START;
                            BEGIN( PreLineCtrl );
                          }
<PreLineCtrl>"\""[^\n\"]*"\"" {
                            yyextra->cCodeBuffer += yytext;
                          }
<PreLineCtrl>.            {
                            yyextra->cCodeBuffer += yytext;
                          }
<PreLineCtrl>\n           {
                            yyextra->cCodeBuffer += yytext;
                            BEGIN( yyextra->lastPreLineCtrlContext );
                          }
<SkipCurly>"{"            {
                            yyextra->cCodeBuffer += yytext;
                                          ++yyextra->curlyCount ;
                          }
<SkipCurly>"}"/{BN}*{DCOMM}"<!--" | /* see bug710917 */
<SkipCurly>"}"            {
                            yyextra->cCodeBuffer += yytext;
                            if( yyextra->curlyCount )
                            {
                              --yyextra->curlyCount ;
                            }
                          }
<SkipCurly>"}"{BN}*{DCOMM}"<" {
                            yyextra->cCodeBuffer += yytext;
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
                            yyextra->cCodeBuffer += yytext;
                            yyextra->lastStringContext=SkipCurly;
                            BEGIN( SkipString );
                          }
<SkipCurly>^{B}*"#"       {
                            yyextra->cCodeBuffer += yytext;
                            yyextra->lastPreLineCtrlContext = YY_START;
                            BEGIN( PreLineCtrl );
                          }
<SkipCurly>{B}*{RAWBEGIN}  {
                            char *p=yytext;
                            while (*p && qisspace(*p)) p++;
                            yyextra->delimiter = extractBeginRawStringDelimiter(p);
                            yyextra->lastRawStringContext = YY_START;
                            yyextra->cCodeBuffer += yytext;
                            BEGIN(RawString);
                          }
<SkipCurly>[^\n#"'@\\/{}<]+ {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCurly>{CCS}           {
                            yyextra->cCodeBuffer += yytext;
                                          yyextra->lastCContext = YY_START;
                                          BEGIN(SkipComment);
                          }
<SkipCurly>{CPPC}           {
                            yyextra->cCodeBuffer += yytext;
                                          yyextra->lastCContext = YY_START;
                                          BEGIN(SkipCxxComment);
                          }
<SkipCurly>{CHARLIT}      {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCurly>\'             {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCurly>.              {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCurly>({CPPC}{B}*)?{CCS}"!" {
                            yyextra->cCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocBlock );
                          }
<SkipCurly>{CCS}"*"[*]+{BL}  {
                            bool javadocBanner = Config_getBool(JAVADOC_BANNER);
                            yyextra->cCodeBuffer += yytext;
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
                             yyextra->cCodeBuffer += yytext;
                             yyextra->docBlockContext   = YY_START;
                             BEGIN( DocBlock );
                          }
<SkipCurly>{CPPC}"!"          {
                            yyextra->cCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocLine );
                          }
<SkipCurly>{CPPC}"/"/[^/]     {
                            yyextra->cCodeBuffer += yytext;
                            yyextra->docBlockContext   = YY_START;
                            BEGIN( DocLine );
                          }

<SkipCurly>\n             {
                            yyextra->cCodeBuffer += yytext;
                            if (yyextra->curlyCount<=0)
                            {
                              BEGIN(RulesPattern);
                            }
                          }
<SkipString>\\.           {
                             yyextra->cCodeBuffer += yytext;
                          }
<SkipString>\"            {
                             yyextra->cCodeBuffer += yytext;
                             BEGIN( yyextra->lastStringContext );
                          }
<SkipString>{CCS}|{CCE}|{CPPC} {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipString>\n            {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipString>.             {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCxxComment>.*"\\\n"  {  // line continuation
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCxxComment>{ANYopt}/\n     {
                            yyextra->cCodeBuffer += yytext;
                            BEGIN( yyextra->lastCContext ) ;
                          }
<Comment>{BN}+            {
                            yyextra->cCodeBuffer += yytext ;
                          }
<Comment>{CCS}             { yyextra->cCodeBuffer += yytext ; }
<Comment>{CPPC}             { yyextra->cCodeBuffer += yytext ; }
<Comment>{CMD}("code"|"verbatim"|"iliteral") {
                            yyextra->insideCode=TRUE;
                            yyextra->cCodeBuffer += yytext ;
                          }
<Comment>{CMD}("endcode"|"endverbatim"|"endiliteral") {
                            yyextra->insideCode=FALSE;
                            yyextra->cCodeBuffer += yytext ;
                          }
<Comment>[^ \.\t\r\n\/\*]+ { yyextra->cCodeBuffer += yytext ; }
<Comment>{CCE}             { yyextra->cCodeBuffer += yytext ;
                                          if (!yyextra->insideCode) BEGIN( yyextra->lastContext ) ;
                          }
<Comment>.                { yyextra->cCodeBuffer += *yytext ; }

<SkipComment>{CPPC}|{CCS}    {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipComment>[^\*\n]+     {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipComment>\n           {
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipComment>{B}*{CCE}     {
                            yyextra->cCodeBuffer += yytext;
                            BEGIN( yyextra->lastCContext );
                          }
<SkipComment>"*"          {
                            yyextra->cCodeBuffer += yytext;
                          }
<RawString>{RAWEND}       {
                            yyextra->cCodeBuffer += yytext;
                            if (extractEndRawStringDelimiter(yytext)==yyextra->delimiter)
                            {
                              BEGIN(yyextra->lastRawStringContext);
                            }
                          }
<RawString>[^)\n]+        {
                            yyextra->cCodeBuffer += yytext;
                          }
<RawString>.              {
                            yyextra->cCodeBuffer += yytext;
                          }
<RawString>\n             {
                            yyextra->cCodeBuffer += yytext;
                          }


  /* ---- Single line comments ------ */
<DocLine>[^\n]*"\n"[ \t]*{CPPC}[/!][<]? { // continuation of multiline C++-style comment
                            yyextra->cCodeBuffer += yytext;
                          }
<DocLine>{B}*{CPPC}"/"[/]+{Bopt}/"\n" { // ignore marker line (see bug700345)
                            yyextra->cCodeBuffer += yytext;
                            BEGIN( yyextra->docBlockContext );
                          }
<DocLine>{NONLopt}/"\n"{B}*{CPPC}[!/]{B}*{CMD}"}" { // next line is an end group marker, see bug 752712
                            yyextra->cCodeBuffer += yytext;
                            BEGIN( yyextra->docBlockContext );
                          }
<DocLine>{NONLopt}/"\n"      { // whole line
                            yyextra->cCodeBuffer += yytext;
                            BEGIN( yyextra->docBlockContext );
                          }

 /* ---- Comments blocks ------ */

<DocBlock>"*"*{CCE}        { // end of comment block
                            yyextra->cCodeBuffer += yytext;
                            BEGIN(yyextra->docBlockContext);
                          }
<DocBlock>^{B}*"*"+/[^/]  {
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>^{B}*({CPPC})?{B}*"*"+/[^/a-z_A-Z0-9*] { // start of a comment line
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>^{B}*({CPPC}){B}* { // strip embedded C++ comments if at the start of a line
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>{CPPC}            { // slashes in the middle of a comment block
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>{CCS}            { // start of a new comment in the
                            // middle of a comment block
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>{CMD}("f$"|"f["|"f{"|"f(") {
                            yyextra->cCodeBuffer += yytext;
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
                            yyextra->cCodeBuffer += yytext;
                            yyextra->docBlockName="<pre>";
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>{CMD}"startuml"/[^a-z_A-Z0-9\-] { // verbatim type command (which could contain nested comments!)
                            yyextra->cCodeBuffer += yytext;
                            yyextra->docBlockName="uml";
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>{CMD}("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc"|"code")/[^a-z_A-Z0-9\-] { // verbatim command (which could contain nested comments!)
                            yyextra->cCodeBuffer += yytext;
                            yyextra->docBlockName=&yytext[1];
                            yyextra->fencedSize=0;
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                            yyextra->cCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            yyextra->docBlockName="~~~";
                            yyextra->fencedSize=pat.stripWhiteSpace().length();
                            yyextra->nestedComment=FALSE;
                            BEGIN(DocCopyBlock);
                          }
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]* {
                            yyextra->cCodeBuffer += yytext;
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
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>\n              { // newline
                            yyextra->cCodeBuffer += yytext;
                          }
<DocBlock>.               { // command block
                            yyextra->cCodeBuffer += yytext;
                          }
 /* ---- Copy verbatim sections ------ */

<DocCopyBlock>"</"{PRE}">" { // end of a <pre> block
                            yyextra->cCodeBuffer += yytext;
                            if (yyextra->docBlockName=="<pre>")
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>"</"{CODE}">" { // end of a <code> block
                            yyextra->cCodeBuffer += yytext;
                            if (yyextra->docBlockName=="<code>")
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>{CMD}("f$"|"f]"|"f}"|"f)") {
                            yyextra->cCodeBuffer += yytext;
                            if (yyextra->docBlockName==&yytext[1])
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>{CMD}("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endmsc"|"enduml"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
                            yyextra->cCodeBuffer += yytext;
                            if (yyextra->docBlockName==&yytext[4])
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{BN}+ { // start of a comment line
                            yyextra->cCodeBuffer += yytext;
                            if ((yyextra->docBlockName=="verbatim") || (yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              REJECT;
                            }
                            else
                            {
                              yyextra->cCodeBuffer += yytext;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{B}+"*"{BN}* { // start of a comment line with two *'s
                            if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              yyextra->cCodeBuffer += yytext;
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/({ID}|"(") { // Assume *var or *(... is part of source code (see bug723516)
                            if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                            {
                              yyextra->cCodeBuffer += yytext;
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
                                yyextra->cCodeBuffer += yytext;
                              }
                              else // remove * it is part of the comment block
                              {
                                yyextra->cCodeBuffer += yytext;
                              }
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                            yyextra->cCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            if (yyextra->docBlockName == "~~~" && yyextra->fencedSize==pat.stripWhiteSpace().length())
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"```"[`]* {
                            yyextra->cCodeBuffer += yytext;
                            QCString pat = substitute(yytext,"*"," ");
                            if (yyextra->docBlockName == "```" && yyextra->fencedSize==pat.stripWhiteSpace().length())
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>[^\<@/\*\]~\$\\\n]+ { // any character that is not special
                            yyextra->cCodeBuffer += yytext;
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
                            yyextra->cCodeBuffer += yytext;
                          }
<DocCopyBlock>\n          { // newline
                            yyextra->cCodeBuffer += yytext;
                          }
<DocCopyBlock>.           { // any other character
                            yyextra->cCodeBuffer += yytext;
                          }
<SkipCurlyEndDoc>"}"{BN}*{DCOMM}"<" { // desc is followed by another one
                            yyextra->docBlockContext   = SkipCurlyEndDoc;
                            yyextra->cCodeBuffer += yytext;
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
                            yyextra->cCodeBuffer += yytext;
                            BEGIN(SkipCurly);
                          }

<UserSection>.*{nl}       {
                            yyextra->cCodeBuffer += yytext;
                          }


 /*
<*>.  { fprintf(stderr,"Lex scanner Def rule for %s: #%s#\n",stateToString(YY_START),yytext);}
<*>{nl}  { fprintf(stderr,"Lex scanner Def rule for newline %s: #%s#\n",stateToString(YY_START),yytext);}
 */
<*><<EOF>>                {
                            handleCCode(yyscanner);
                            yyterminate();
                          }
%%

//----------------------------------------------------------------------------
static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  while( c < max_size && yyextra->inputString[yyextra->inputPosition] )
  {
    *buf = yyextra->inputString[yyextra->inputPosition++] ;
    //printf("%d (%c)\n",*buf,*buf);
    c++; buf++;
  }
  return c;
}

//-----------------------------------------------------------------------------

static void parseMain(yyscan_t yyscanner,
                      const QCString &fileName,
                      const char *fileBuf,
                      const std::shared_ptr<Entry> &rt,
                      ClangTUParser *clangParser)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;
  lexscannerYYrestart(0,yyscanner);

  yyextra->current_root  = rt;
  yyextra->fileName = fileName;
  yyextra->clangParser = clangParser;
  yyextra->language    = getLanguageFromFileName(yyextra->fileName);
  rt->lang = yyextra->language;
  msg("Parsing file {}...\n",yyextra->fileName);

  yyextra->current_root  = rt;
  yyextra->current = std::make_shared<Entry>();
  EntryType sec=guessSection(yyextra->fileName);
  if (!sec.isEmpty())
  {
    yyextra->current->name    = yyextra->fileName;
    yyextra->current->section = EntryType(sec);
    yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  }
  yyextra->current->reset();
  BEGIN( DefSection );

  lexscannerYYlex(yyscanner);

  rt->program.str(std::string());
}

//----------------------------------------------------------------------------


static void handleCCode(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  if (yyextra->cCodeBuffer.isEmpty()) return;
  yyextra->cOutlineParser.parseInput(yyextra->fileName,
                                     yyextra->cCodeBuffer.data(),
                                     yyextra->current_root,
                                     yyextra->clangParser);
  yyextra->cCodeBuffer.clear();
  return;
}
//----------------------------------------------------------------------------

struct LexOutlineParser::Private
{
  yyscan_t yyscanner;
  lexscannerYY_state state;
};

LexOutlineParser::LexOutlineParser() : p(std::make_unique<LexOutlineParser::Private>())
{
  lexscannerYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  lexscannerYYset_debug(Debug::isFlagSet(Debug::Lex_lexscanner)?1:0,p->yyscanner);
#endif
}

LexOutlineParser::~LexOutlineParser()
{
  lexscannerYYlex_destroy(p->yyscanner);
}

void LexOutlineParser::parseInput(const QCString &fileName,
                                  const char *fileBuf,
                                  const std::shared_ptr<Entry> &root,
                                  ClangTUParser *clangParser)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

  yyextra->fileName      = fileName;
  DebugLex debugLex(Debug::Lex_lexscanner, __FILE__, qPrint(fileName));

  ::parseMain(p->yyscanner,fileName,fileBuf,root,clangParser);
}


//----------------------------------------------------------------------------

#include "lexscanner.l.h"
