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

%{

#include "constexp.h"  
#include "cppvalue.h"
#include "ce_parse.hpp" // generated header file
#include "message.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1


static int yyread(char *buf,int max_size,yyscan_t yyscanner);

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size,yyscanner);

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
                                     yyextra->g_strToken=yytext;  
				     return TOK_CHARACTER; 
				   }
0[0-7]*{CONSTSUFFIX}?              { yyextra->g_strToken=yytext; 
  				     return TOK_OCTALINT; 
				   }
[1-9][0-9]*{CONSTSUFFIX}?          { yyextra->g_strToken=yytext; 
  				     return TOK_DECIMALINT; 
				   }
(0x|0X)[0-9a-fA-F]+{CONSTSUFFIX}?  { yyextra->g_strToken=yytext+2; 
                                     return TOK_HEXADECIMALINT; 
                                   }
(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE]([\-\+])?[0-9]+)?([fFlL])? { 
                                     yyextra->g_strToken=yytext; return TOK_FLOAT; 
                                   }
([0-9]+[eE])([\-\+])?[0-9]+([fFlL])? { 
                                     yyextra->g_strToken=yytext; return TOK_FLOAT; 
			           }
.				   
\n

%%

static int yyread(char *buf,int max_size,yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  while( c < max_size && yyextra->g_inputString[yyextra->g_inputPosition] )
  {
    *buf = yyextra->g_inputString[yyextra->g_inputPosition++] ;
    c++; buf++;
  }
  return c;
}


static yyscan_t yyscanner;
static struct constexpYY_state constexpYY_extra;

bool parseconstexp(const char *fileName,int lineNr,const QCString &s)
{
  constexpYYlex_init_extra(&constexpYY_extra, &yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  yyextra->g_constExpFileName = fileName;
  yyextra->g_constExpLineNr = lineNr;
  yyextra->g_inputString = s;
  yyextra->g_inputPosition = 0;
  constexpYYrestart( yyin, yyscanner );

  printlex(yy_flex_debug, TRUE, __FILE__, fileName);
  //printf("Expression: `%s'\n",s.data());

  constexpYYparse(yyscanner);

  //printf("Result: %ld\n",(long)g_resultValue);
  printlex(yy_flex_debug, FALSE, __FILE__, fileName);
  return (long)yyextra->g_resultValue!=0;
}

extern "C" {
  int constexpYYwrap(yyscan_t yyscanner) { return 1; }
}
