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
%{

#include "constexp.h"  
#include "cppvalue.h"
#include "ce_parse.h" // generated header file
#include "message.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1
  
QCString    g_strToken;  
CPPValue    g_resultValue;
int         g_constExpLineNr;
QCString    g_constExpFileName;

static const char *g_inputString;
static int         g_inputPosition;

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  while( c < max_size && g_inputString[g_inputPosition] )
  {
    *buf = g_inputString[g_inputPosition++] ;
    c++; buf++;
  }
  return c;
}

%}

CONSTSUFFIX ([uU][lL]?[lL]?)|([lL][lL]?[uU]?)

%option nounput

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
                                     g_strToken=yytext;  
				     return TOK_CHARACTER; 
				   }
0[0-7]*{CONSTSUFFIX}?              { g_strToken=yytext; 
  				     return TOK_OCTALINT; 
				   }
[1-9][0-9]*{CONSTSUFFIX}?          { g_strToken=yytext; 
  				     return TOK_DECIMALINT; 
				   }
(0x|0X)[0-9a-fA-F]+{CONSTSUFFIX}?  { g_strToken=yytext+2; 
                                     return TOK_HEXADECIMALINT; 
                                   }
(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE]([\-\+])?[0-9]+)?([fFlL])? { 
                                     g_strToken=yytext; return TOK_FLOAT; 
                                   }
([0-9]+[eE])([\-\+])?[0-9]+([fFlL])? { 
                                     g_strToken=yytext; return TOK_FLOAT; 
			           }
.				   
\n

%%

bool parseconstexp(const char *fileName,int lineNr,const QCString &s)
{
  printlex(yy_flex_debug, TRUE, __FILE__, fileName);
  //printf("Expression: `%s'\n",s.data());
  g_constExpFileName = fileName;
  g_constExpLineNr = lineNr;
  g_inputString = s;
  g_inputPosition = 0;
  constexpYYrestart( constexpYYin );
  constexpYYparse();
  //printf("Result: %ld\n",(long)g_resultValue);
  printlex(yy_flex_debug, FALSE, __FILE__, fileName);
  return (long)g_resultValue!=0;
}

extern "C" {
  int constexpYYwrap() { return 1; }
}
