/*****************************************************************************
 *
 *
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

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

#define repeatChar(chr, cnt) std::string(cnt, chr).c_str()

struct lexscannerYY_state
{
  COutlineParser   cOutlineParser;
  const char *     inputString = 0;
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
  uint             fencedSize = 0;
  bool             nestedComment = false;

  QCString         prefix = "yy";
  QCString         CCodeBuffer;
  int              roundCount = 0;
  int              squareCount = 0;

  QCString         yyFileName;
  ClangTUParser   *clangParser = 0;

  std::shared_ptr<Entry> current;
  std::shared_ptr<Entry> current_root;
  SrcLangExt language;
};

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif
//-----------------------------------------------------------------------------

// forward declarations for statefull functions
static void handleCCode(yyscan_t yyscanner);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);

/* ----------------------------------------------------------------- */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

%}

nl              (\r\n|\r|\n)
ws              [ \t]
nws             [^ \t\n]
TopStart        "%top{"{nl}
TopEnd          "}"{nl}
LiteralStart    "%{"{nl}
LiteralEnd      "%}"{nl}
OptPrefix       "%option"{ws}+"prefix"{ws}*"="{ws}*
RulesStart      "%%"{nl}
RulesEnd        "%%"{nl}
RulesSharp      "<"[^>]*">"
RulesCurly      "{"[^{}\n]*"}"
StartSquare     "["
StartDouble     "\""
StartRound      "("
EscapeRulesCharOpen  "\\["|"\\<"|"\\{"|"\\("|"\\\""|"\\{"|"\\ "
EscapeRulesCharClose "\\]"|"\\>"|"\\}"|"\\)"
EscapeRulesChar      {EscapeRulesCharOpen}|{EscapeRulesCharClose}

CMD       ("\\"|"@")
BN        [ \t\n\r]
BL        [ \t\r]*"\n"
B         [ \t]
Bopt      {B}*
ID        "$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
PRE       [pP][rR][eE]
CODE      [cC][oO][dD][eE]
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))

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
%x OptPrefix
%x DefSectionLine
%x RulesSectionInit
%x RulesPattern
%x RulesDouble
%x RulesRoundDouble
%x RulesSquare
%x RulesRoundSquare
%x RulesRound
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
<DefSection>{OptPrefix}  {
                           BEGIN (OptPrefix);
                         }
<OptPrefix>"\""[^\"]*"\"" {
                           yyextra->prefix = yytext;
                           yyextra->prefix = yyextra->prefix.mid(1,yyleng-2);
                         }
<OptPrefix>{nl}          {
                           yyextra->CCodeBuffer += yytext;
                           BEGIN (DefSection);
                         }
<DefSection>^{RulesStart} {
                           yyextra->CCodeBuffer += "int " + yyextra->prefix + "lex (yyscan_t yyscanner) {\n";
                           BEGIN (RulesSectionInit);
                         }
<DefSection>^{TopStart}  {
                           yyextra->CCodeBuffer += "\n";
                           yyextra->lastContext = YY_START;
                           BEGIN (TopSection);
                         }
<DefSection>^{LiteralStart}   {
                           yyextra->CCodeBuffer += "\n";
                           yyextra->lastContext = YY_START;
                           BEGIN (LiteralSection);
                         }
<TopSection>^{TopEnd}    {
                           yyextra->CCodeBuffer += "\n";
                           BEGIN( yyextra->lastContext ) ;
                         }
<TopSection>.*{nl}       {
                           yyextra->CCodeBuffer += yytext;
                         }
<LiteralSection>^{LiteralEnd}     {
                           yyextra->CCodeBuffer += "\n";
                           BEGIN( yyextra->lastContext ) ;
                         }
<LiteralSection>.*{nl}   {
                           yyextra->CCodeBuffer += yytext;
                         }
<DefSection>^{nws}       {
                           BEGIN(DefSectionLine);
                         }
<DefSection>{CPPC}.*{nl}   {
                           yyextra->CCodeBuffer += yytext;
                         }
<DefSection>^{ws}*{CCS}   {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(COMMENT);
                         }
<COMMENT>{CCE}{ws}*{nl}   {
                           yyextra->CCodeBuffer+=yytext;
                           BEGIN(yyextra->lastContext);
                         }
<COMMENT>{CCE}            {
                           yyextra->CCodeBuffer+=yytext;
                           BEGIN(yyextra->lastContext);
                         }
<COMMENT>[^*\n]+         {
                           yyextra->CCodeBuffer += yytext;
                         }
<COMMENT>{CPPC}|{CCS}       {
                           yyextra->CCodeBuffer += yytext;
                         }
<COMMENT>{nl}              {
                           yyextra->CCodeBuffer += yytext;
                         }
<COMMENT>.               {
                           yyextra->CCodeBuffer += yytext;
                         }
<DefSection>^{nl}        {
                           yyextra->CCodeBuffer += "\n";
                         }
<DefSection>^{ws}.*{nl}  {
                           yyextra->CCodeBuffer += yytext;
                         }
<DefSectionLine>.*{nl}   {
                           yyextra->CCodeBuffer += "\n";
                           BEGIN(DefSection);
                         }
<RulesSectionInit,RulesPattern>^{RulesEnd} {
                           yyextra->CCodeBuffer += "}\n";
                           BEGIN (UserSection);
                         }
<RulesSectionInit>^{nws}     {
                           unput(*yytext);
                           BEGIN(RulesPattern);
                         }
<RulesSectionInit>^{ws}.*{nl} {
                           yyextra->CCodeBuffer += yytext;
                         }
<RulesSectionInit>^{nl}  {
                           yyextra->CCodeBuffer += yytext;
                         }
<RulesPattern>"<<EOF>>"  {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{EscapeRulesChar} {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{RulesSharp} {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{RulesCurly} {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{StartDouble} {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesDouble);
                        }
<RulesDouble,RulesRoundDouble>"\\\\" {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesDouble,RulesRoundDouble>"\\\"" {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesDouble>"\""       {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN( yyextra->lastContext ) ;
                        }
<RulesRoundDouble>"\""  {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRound) ;
                        }
<RulesDouble,RulesRoundDouble>. {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesPattern>{StartSquare} {
                           yyextra->squareCount++;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesSquare);
                        }
<RulesSquare,RulesRoundSquare>"\\[" |
<RulesSquare,RulesRoundSquare>"\\]" {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesSquare,RulesRoundSquare>"[" {
                           yyextra->squareCount++;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesSquare>"]"        {
                           yyextra->squareCount--;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           if (!yyextra->squareCount) BEGIN(RulesPattern);
                        }
<RulesRoundSquare>"]"   {
                           yyextra->squareCount--;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           if (!yyextra->squareCount) BEGIN(RulesRound) ;
                        }
<RulesSquare,RulesRoundSquare>"\\\\" {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesSquare,RulesRoundSquare>. {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesPattern>{StartRound} {
                           yyextra->roundCount++;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->lastContext = YY_START;
                           BEGIN(RulesRound);
                        }
<RulesRound>{RulesCurly} {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>{StartSquare} {
                           yyextra->squareCount++;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRoundSquare);
                        }
<RulesRound>{StartDouble} {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           BEGIN(RulesRoundDouble);
                        }
<RulesRound>"\\("       |
<RulesRound>"\\)"       {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>"("         {
                           yyextra->roundCount++;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesRound>")"         {
                           yyextra->roundCount--;
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           if (!yyextra->roundCount) BEGIN( yyextra->lastContext ) ;
                        }
<RulesRound>.           {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                        }
<RulesPattern>{ws}+"|"  {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                           yyextra->curlyCount = 0;
                           BEGIN(SkipCurly);
                         }
<RulesPattern>^{ws}*{nl} {
                           yyextra->CCodeBuffer += "\n";
                         }
<RulesPattern>^{ws}+     {
                         }

<RulesPattern>({ws}|{nl}) {
                           unput(*yytext);
                           yyextra->curlyCount = 0;
                           BEGIN(SkipCurly);
                         }
<RulesPattern>"\\\\"     {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
                         }
<RulesPattern>{CCS}       {
                           yyextra->CCodeBuffer += yytext;
                           yyextra->lastContext = YY_START;
                           BEGIN(COMMENT);
                         }
<RulesPattern>.          {
                           yyextra->CCodeBuffer += repeatChar(' ', yyleng);
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
                            BEGIN( yyextra->lastPreLineCtrlContext );
                          }
<SkipCurly>"{"            {
                            yyextra->CCodeBuffer += yytext;
                                          ++yyextra->curlyCount ;
                          }
<SkipCurly>"}"/{BN}*{DCOMM}"<!--" | /* see bug710917 */
<SkipCurly>"}"            {
                            yyextra->CCodeBuffer += yytext;
                            if( yyextra->curlyCount )
                            {
                              --yyextra->curlyCount ;
                            }
                          }
<SkipCurly>"}"{BN}*{DCOMM}"<" {
                            yyextra->CCodeBuffer += yytext;
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
<SkipCurly>{B}*{RAWBEGIN}  {
                            QCString raw=QCString(yytext).stripWhiteSpace();
                            yyextra->delimiter = raw.data()+2;
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
                            if (yyextra->curlyCount<=0)
                            {
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
                          }
<SkipString>.             {
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCxxComment>.*"\\\n"  {  // line continuation
                            yyextra->CCodeBuffer += yytext;
                          }
<SkipCxxComment>{ANYopt}/\n     {
                            yyextra->CCodeBuffer += yytext;
                            BEGIN( yyextra->lastCContext ) ;
                          }
<Comment>{BN}+            {
                            yyextra->CCodeBuffer += yytext ;
                          }
<Comment>{CCS}             { yyextra->CCodeBuffer += yytext ; }
<Comment>{CPPC}             { yyextra->CCodeBuffer += yytext ; }
<Comment>{CMD}("code"|"verbatim") {
                            yyextra->insideCode=TRUE;
                            yyextra->CCodeBuffer += yytext ;
                          }
<Comment>{CMD}("endcode"|"endverbatim") {
                            yyextra->insideCode=FALSE;
                            yyextra->CCodeBuffer += yytext ;
                          }
<Comment>[^ \.\t\r\n\/\*]+ { yyextra->CCodeBuffer += yytext ; }
<Comment>{CCE}             { yyextra->CCodeBuffer += yytext ;
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
                          }


  /* ---- Single line comments ------ */
<DocLine>[^\n]*"\n"[ \t]*{CPPC}[/!][<]? { // continuation of multiline C++-style comment
                            yyextra->CCodeBuffer += yytext;
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
                            if (yyextra->docBlockName.at(1)=='{')
                            {
                              yyextra->docBlockName.at(1)='}';
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
<DocBlock>{CMD}("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"code")/[^a-z_A-Z0-9\-] { // verbatim command (which could contain nested comments!)
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
                            BEGIN(DocBlock);
                          }
<DocCopyBlock>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
                            yyextra->CCodeBuffer += yytext;
                            if (&yytext[4]==yyextra->docBlockName)
                            {
                              BEGIN(DocBlock);
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{BN}+ { // start of a comment line
                            yyextra->CCodeBuffer += yytext;
                            if (yyextra->docBlockName=="verbatim")
                            {
                              REJECT;
                            }
                            else if (yyextra->docBlockName=="code")
                            {
                              REJECT;
                            }
                            else
                            {
                              yyextra->CCodeBuffer += yytext;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{B}+"*"{BN}* { // start of a comment line with two *'s
                            if (yyextra->docBlockName=="code")
                            {
                              yyextra->CCodeBuffer += yytext;
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/({ID}|"(") { // Assume *var or *(... is part of source code (see bug723516)
                            if (yyextra->docBlockName=="code")
                            {
                              yyextra->CCodeBuffer += yytext;
                            }
                            else
                            {
                              REJECT;
                            }
                          }
<DocCopyBlock>^{B}*"*"+/{BN}* { // start of a comment line with one *
                            if (yyextra->docBlockName=="code")
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
                          }

  /*
<*>.  { fprintf(stderr,"Lex scanner Def rule for %s: #%s#\n",stateToString(YY_START),yytext);}
  */
<*><<EOF>>                {
                            handleCCode(yyscanner);
                            yyterminate();
                          }
%%

//----------------------------------------------------------------------------
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t c=0;
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
                      const char *fileName,
                      const char *fileBuf,
                      const std::shared_ptr<Entry> &rt,
                      ClangTUParser *clangParser)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;
  lexscannerYYrestart(0,yyscanner);

  yyextra->current_root  = rt;
  yyextra->yyFileName = fileName;
  yyextra->clangParser = clangParser;
  yyextra->language    = getLanguageFromFileName(yyextra->yyFileName);
  rt->lang = yyextra->language;
  msg("Parsing file %s...\n",yyextra->yyFileName.data());

  yyextra->current_root  = rt;
  yyextra->current = std::make_shared<Entry>();
  int sec=guessSection(yyextra->yyFileName);
  if (sec)
  {
    yyextra->current->name    = yyextra->yyFileName;
    yyextra->current->section = sec;
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

  if (yyextra->CCodeBuffer.isEmpty()) return;
  yyextra->cOutlineParser.parseInput(yyextra->yyFileName,
                                     yyextra->CCodeBuffer,
                                     yyextra->current_root,
                                     yyextra->clangParser);
               // SrcLangExt_Cpp,
  yyextra->CCodeBuffer.resize(0);
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
  lexscannerYYset_debug(1,p->yyscanner);
#endif
}

LexOutlineParser::~LexOutlineParser()
{
  lexscannerYYlex_destroy(p->yyscanner);
}

void LexOutlineParser::parseInput(const char *fileName,
                                  const char *fileBuf,
                                  const std::shared_ptr<Entry> &root,
                                  ClangTUParser *clangParser)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

  printlex(yy_flex_debug, TRUE, __FILE__, fileName);

  ::parseMain(p->yyscanner,fileName,fileBuf,root,clangParser);

  printlex(yy_flex_debug, FALSE, __FILE__, fileName);
}


//----------------------------------------------------------------------------

#if USE_STATE2STRING
#include "lexscanner.l.h"
#endif
