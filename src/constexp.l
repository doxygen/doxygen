/******************************************************************************
 *
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
%option never-interactive
%option prefix="constexpYY"
%option nounput
%option reentrant bison-bridge
%option extra-type="struct constexpYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve typesafety
#include "constexp_p.h"  
}

%{

#include "constexp.h"  
#include "cppvalue.h"
#include "ce_parse.hpp" // generated header file
#include "message.h"
#include "debug.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

[[maybe_unused]] static const char *stateToString(int state);

static int yyread(char *buf,int max_size,yyscan_t yyscanner);

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size,yyscanner);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#define LEX_NO_INPUT_FILENAME
#include "doxygen_lex.h"

%}

CONSTSUFFIX ([uU][lL]?[lL]?)|([lL][lL]?[uU]?)


%%

"?"				   { return TOK_QUESTIONMARK; }
":"				   { return TOK_COLON; }
"||"				   { return TOK_OR; }
"&&"				   { return TOK_AND; }
"|"				   { return TOK_BITWISEOR; }
"^"				   { return TOK_BITWISEXOR; }
"&"				   { return TOK_AMPERSAND; }
"!="            		   { return TOK_NOTEQUAL; }
"=="            		   { return TOK_EQUAL; }
"<"             		   { return TOK_LESSTHAN; }
">"             		   { return TOK_GREATERTHAN; }
"<="            		   { return TOK_LESSTHANOREQUALTO; }
">="            		   { return TOK_GREATERTHANOREQUALTO; }
"<<"            		   { return TOK_SHIFTLEFT; }
">>"            		   { return TOK_SHIFTRIGHT; }
"+"             		   { return TOK_PLUS; }
"-"             		   { return TOK_MINUS; }
"*"             		   { return TOK_STAR; }
"/"             		   { return TOK_DIVIDE; }
"%"             		   { return TOK_MOD; }
"~"             		   { return TOK_TILDE; }
"!"             		   { return TOK_NOT; }
"("             		   { return TOK_LPAREN; }
")"             		   { return TOK_RPAREN; }
"'"(([^\'\n\r\\]+)|(\\(([ntvbrfa\\?'\"])|([0-9]+)|([xX][0-9a-fA-F]+))))"'"   { 
                                     yyextra->strToken=yytext;  
				     return TOK_CHARACTER; 
				   }
0[0-7]*{CONSTSUFFIX}?              { yyextra->strToken=yytext; 
  				     return TOK_OCTALINT; 
				   }
[1-9][0-9]*{CONSTSUFFIX}?          { yyextra->strToken=yytext; 
  				     return TOK_DECIMALINT; 
				   }
(0x|0X)[0-9a-fA-F]+{CONSTSUFFIX}?  { yyextra->strToken=yytext+2; 
                                     return TOK_HEXADECIMALINT; 
                                   }
(0b|0B)[01]+{CONSTSUFFIX}?         { yyextra->strToken=yytext+2; 
                                     return TOK_BINARYINT; 
                                   }
(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE]([\-\+])?[0-9]+)?([fFlL])? { 
                                     yyextra->strToken=yytext; return TOK_FLOAT; 
                                   }
([0-9]+[eE])([\-\+])?[0-9]+([fFlL])? { 
                                     yyextra->strToken=yytext; return TOK_FLOAT; 
			           }
.				   
\n

%%

static int yyread(char *buf,int max_size,yyscan_t yyscanner)
{
  struct yyguts_t *yyg = static_cast<struct yyguts_t*>(yyscanner);
  int c=0;
  while( c < max_size && yyextra->inputString[yyextra->inputPosition] )
  {
    *buf = yyextra->inputString[yyextra->inputPosition++] ;
    c++; buf++;
  }
  return c;
}

struct ConstExpressionParser::Private
{
  yyscan_t yyscanner;
  struct constexpYY_state constexpYY_extra;
};


ConstExpressionParser::ConstExpressionParser() : p(std::make_unique<Private>())
{
  constexpYYlex_init_extra(&p->constexpYY_extra, &p->yyscanner);
}

ConstExpressionParser::~ConstExpressionParser()
{
  constexpYYlex_destroy(p->yyscanner);
}

bool ConstExpressionParser::parse(const char *fileName,int lineNr,const std::string &s, const std::string &orgStr)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

#ifdef FLEX_DEBUG
  constexpYYset_debug(Debug::isFlagSet(Debug::Lex_constexp)?1:0,p->yyscanner);
#endif

  yyextra->constExpFileName = fileName;
  yyextra->constExpLineNr = lineNr;
  yyextra->orgString = orgStr;
  yyextra->inputString = s;
  yyextra->inputPosition = 0;
  constexpYYrestart( yyin, p->yyscanner );

  DebugLex debugLex(Debug::Lex_constexp, __FILE__, fileName);
  //printf("Expression: '%s'\n",qPrint(s));

  constexpYYparse(p->yyscanner);

  //printf("Result: %ld\n",(long)g_resultValue);
  bool result = static_cast<long>(yyextra->resultValue)!=0;

  return result;
}

extern "C" {
  int constexpYYwrap(yyscan_t /* yyscanner */) { return 1; }
}

#include "constexp.l.h"
