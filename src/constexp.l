/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

%{

#include "constexp.h"  
#include "cppvalue.h"
#include "ce_parse.h" // generated header file

#define YY_NO_UNPUT
#define YY_NEVER_INTERACTIVE 1
  
QCString strToken;  

static const char *inputString;
static int         inputPosition;
CPPValue resultValue;

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  while( c < max_size && inputString[inputPosition] )
  {
    *buf = inputString[inputPosition++] ;
    c++; buf++;
  }
  return c;
}

%}

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
                                     strToken=yytext;  
				     return TOK_CHARACTER; 
				   }
0[0-7]*[uUlL]*                     { strToken=yytext; 
  				     return TOK_OCTALINT; 
				   }
[1-9][0-9]*[uUlL]*                 { strToken=yytext; 
  				     return TOK_DECIMALINT; 
				   }
(0x|0X)[0-9a-fA-F]+[uUlL]*         { strToken=yytext; return TOK_HEXADECIMALINT; }
(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE]([\-\+])?[0-9]+)?([fFlL])? { 
                                     strToken=yytext; return TOK_FLOAT; 
                                   }
([0-9]+[eE])([\-\+])?[0-9]+([fFlL])? { 
                                     strToken=yytext; return TOK_FLOAT; 
			           }
.				   
\n

%%

bool parseCppExpression(const QCString &s)
{
  //printf("Expression: `%s'\n",s.data());
  inputString = s;
  inputPosition = 0;
  cppExpYYrestart( cppExpYYin );
  cppExpYYparse();
  //printf("Result: %ld\n",(long)resultValue);
  return (long)resultValue!=0;
}

extern "C" {
  int cppExpYYwrap() { return 1; }
}
