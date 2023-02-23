%{
/***************************************************************************
 *
 * $Id: lexer.l 184 2011-02-28 21:38:28Z Michael.McTernan $
 *
 * Mscgen language lexer definition.
 * Copyright (C) 2009 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This file is part of msclib.
 *
 * Msc is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Msclib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with msclib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mscgen_config.h"
#include "mscgen_msc.h"
#include "mscgen_bool.h"
#include "mscgen_safe.h"
#include "mscgen_lexer.h"
#include "mscgen_language.h"  /* Token definitions from Yacc/Bison */

#define USE_STATE2STRING 0

/* Counter for error reporting */
static unsigned long  lex_linenum = 1;
static char          *lex_line = NULL;
static Boolean        lex_utf8 = FALSE;

/* Local function prototypes */
static void newline(const char *text, unsigned int n);
static char *trimQstring(char *s);

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif
%}

/* Not used, so prevent compiler warning */
%option never-interactive
%option noinput
%option noyywrap

%x IN_COMMENT
%x BODY
%%

<INITIAL>{
\xef\xbb\xbf                          lex_utf8 = TRUE; BEGIN(BODY);
(\r\n).*                              newline(yytext, 2); BEGIN(BODY);
(\r|\n).*                             newline(yytext, 1); BEGIN(BODY);
.                                     unput(yytext[0]); BEGIN(BODY);
}

<IN_COMMENT>{
"*/"                                  BEGIN(BODY);
[^*\n]+
"*"
(\r\n).*                              newline(yytext, 2);
(\r|\n).*                             newline(yytext, 1);
}

<BODY>{

"/*"                                  BEGIN(IN_COMMENT);

(\r\n).*                              newline(yytext, 2);
(\r|\n).*                             newline(yytext, 1);

#.*$                                  /* Ignore lines after '#' */
\/\/.*$                               /* Ignore lines after '//' */

msc                                   return TOK_MSC;
HSCALE|hscale                         yylval.optType = MSC_OPT_HSCALE;                return TOK_OPT_HSCALE;
WIDTH|width                           yylval.optType = MSC_OPT_WIDTH;                 return TOK_OPT_WIDTH;
ARCGRADIENT|arcgradient               yylval.optType = MSC_OPT_ARCGRADIENT;           return TOK_OPT_ARCGRADIENT;
WORDWRAPARCS|wordwraparcs             yylval.optType = MSC_OPT_WORDWRAPARCS;          return TOK_OPT_WORDWRAPARCS;
URL|url                               yylval.attribType = MSC_ATTR_URL;               return TOK_ATTR_URL;
LABEL|label                           yylval.attribType = MSC_ATTR_LABEL;             return TOK_ATTR_LABEL;
IDURL|idurl                           yylval.attribType = MSC_ATTR_IDURL;             return TOK_ATTR_IDURL;
ID|id                                 yylval.attribType = MSC_ATTR_ID;                return TOK_ATTR_ID;
LINECOLO(U?)R|linecolo(u?)r           yylval.attribType = MSC_ATTR_LINE_COLOUR;       return TOK_ATTR_LINE_COLOUR;
TEXTCOLO(U?)R|textcolo(u?)r           yylval.attribType = MSC_ATTR_TEXT_COLOUR;       return TOK_ATTR_TEXT_COLOUR;
TEXTBGCOLO(U?)R|textbgcolo(u?)r       yylval.attribType = MSC_ATTR_TEXT_BGCOLOUR;     return TOK_ATTR_TEXT_BGCOLOUR;
ARCLINECOLO(U?)R|arclinecolo(u?)r     yylval.attribType = MSC_ATTR_ARC_LINE_COLOUR;   return TOK_ATTR_ARC_LINE_COLOUR;
ARCTEXTCOLO(U?)R|arctextcolo(u?)r     yylval.attribType = MSC_ATTR_ARC_TEXT_COLOUR;   return TOK_ATTR_ARC_TEXT_COLOUR;
ARCTEXTBGCOLO(U?)R|arctextbgcolo(u?)r yylval.attribType = MSC_ATTR_ARC_TEXT_BGCOLOUR; return TOK_ATTR_ARC_TEXT_BGCOLOUR;
ARCSKIP|arcskip                       yylval.attribType = MSC_ATTR_ARC_SKIP;          return TOK_ATTR_ARC_SKIP;
\.\.\.                                yylval.arctype = MSC_ARC_DISCO;    return TOK_SPECIAL_ARC;        /* ... */
---                                   yylval.arctype = MSC_ARC_DIVIDER;  return TOK_SPECIAL_ARC;        /* --- */
\|\|\|                                yylval.arctype = MSC_ARC_SPACE;    return TOK_SPECIAL_ARC;        /* ||| */
\<-\>                                 yylval.arctype = MSC_ARC_SIGNAL;   return TOK_REL_SIG_BI;         /* <-> */
-\>                                   yylval.arctype = MSC_ARC_SIGNAL;   return TOK_REL_SIG_TO;         /* -> */
\<-                                   yylval.arctype = MSC_ARC_SIGNAL;   return TOK_REL_SIG_FROM;       /* <- */
--                                    yylval.arctype = MSC_ARC_SIGNAL;   return TOK_REL_SIG;            /* -- */
-[Xx]                                 yylval.arctype = MSC_ARC_LOSS;     return TOK_REL_LOSS_TO;        /* -x */
[Xx]-                                 yylval.arctype = MSC_ARC_LOSS;     return TOK_REL_LOSS_FROM;      /* x- */
\<=\>                                 yylval.arctype = MSC_ARC_METHOD;   return TOK_REL_METHOD_BI;      /* <=> */
=\>                                   yylval.arctype = MSC_ARC_METHOD;   return TOK_REL_METHOD_TO;      /* => */
\<=                                   yylval.arctype = MSC_ARC_METHOD;   return TOK_REL_METHOD_FROM;    /* <= */
==                                    yylval.arctype = MSC_ARC_METHOD;   return TOK_REL_METHOD;         /* == */
\<\<\>\>                              yylval.arctype = MSC_ARC_RETVAL;   return TOK_REL_RETVAL_BI;      /* <<>> */
\>\>                                  yylval.arctype = MSC_ARC_RETVAL;   return TOK_REL_RETVAL_TO;      /* >> */
\<\<                                  yylval.arctype = MSC_ARC_RETVAL;   return TOK_REL_RETVAL_FROM;    /* << */
\.\.                                  yylval.arctype = MSC_ARC_RETVAL;   return TOK_REL_RETVAL;         /* .. */
\<:\>                                 yylval.arctype = MSC_ARC_DOUBLE;   return TOK_REL_DOUBLE_BI;      /* <:> */
:\>                                   yylval.arctype = MSC_ARC_DOUBLE;   return TOK_REL_DOUBLE_TO;      /* :> */
\<:                                   yylval.arctype = MSC_ARC_DOUBLE;   return TOK_REL_DOUBLE_FROM;    /* <: */
::                                    yylval.arctype = MSC_ARC_DOUBLE;   return TOK_REL_DOUBLE;         /* :: */
\<\<=\>\>                             yylval.arctype = MSC_ARC_CALLBACK; return TOK_REL_CALLBACK_BI;    /* <<=>> */
=\>\>                                 yylval.arctype = MSC_ARC_CALLBACK; return TOK_REL_CALLBACK_TO;    /* =>> */
\<\<=                                 yylval.arctype = MSC_ARC_CALLBACK; return TOK_REL_CALLBACK_FROM;  /* <<= */
BOX|box                               yylval.arctype = MSC_ARC_BOX;      return TOK_REL_BOX;            /* box */
ABOX|abox                             yylval.arctype = MSC_ARC_ABOX;     return TOK_REL_ABOX;           /* abox */
RBOX|rbox                             yylval.arctype = MSC_ARC_RBOX;     return TOK_REL_RBOX;           /* rbox */
NOTE|note                             yylval.arctype = MSC_ARC_NOTE;     return TOK_REL_NOTE;           /* note */
[A-Za-z0-9_]+                         yylval.string = strdup_s(yytext);  return TOK_STRING;
\"(\\\"|[^\"])*\"                     yylval.string = trimQstring(strdup_s(yytext)); return TOK_QSTRING;
=                                     return TOK_EQUAL;
,                                     return TOK_COMMA;
\;                                    return TOK_SEMICOLON;
\{                                    return TOK_OCBRACKET;
\}                                    return TOK_CCBRACKET;
\[                                    return TOK_OSBRACKET;
\]                                    return TOK_CSBRACKET;
\*                                    return TOK_ASTERISK;
[ \t]+                                /* ignore whitespace */;

}


<*>.|\n|\r                            return TOK_UNKNOWN;

%%

/* Handle a new line of input.
 *  This counts the line number and duplicates the string in case we need
 *  it for error reporting.  The line is then returned back for parsing
 *  without the newline characters prefixed.
 */
static void newline(const char *text, unsigned int n)
{
    lex_linenum++;
    if(lex_line != NULL)
    {
        free(lex_line);
    }

    lex_line = strdup(text + n);
    yyless(n);
}


/* Trim a multi-line quoted string.
 *  This allows the parsed input quoted strings to span multiple lines of
 *  input but be condensed to only a single line of output e.g.
 *    a->b [label="line 1
 *                 line 1 too"];
 *  Will parse to a string such as"line1\n       line1 too".  This function
 *  will collapse the \n and whitespace into a single space.
 */
static char *trimQstring(char *const s)
{
    int i = 0, o = 0, skipmode = 0;

    /* Strip leading " */
    if(s[i] == '\"')
    {
        i++;
    }

    /* Copy body, compacting whitespace after newline sequences */
    while(s[i] != '\0')
    {
        if(s[i] == '\r' || s[i] == '\n' || s[i] == '\f')
        {
            skipmode = 1;
        }
        else if(!skipmode || !isspace(s[i]))
        {
            if(skipmode)
            {
                s[o] = ' ';
                o++;
            }

            skipmode = 0;
            s[o] = s[i];
            o++;
        }

        i++;
    }

    /* Null terminate */
    s[o] = '\0';

    /* Remove trailing " */
    if(o >= 1 && s[o - 1] == '\"')
        s[o-1] = '\0';

    return s;
}

unsigned long lex_getlinenum(void)
{
    return lex_linenum;
}

char *lex_getline(void)
{
    return lex_line;
}

void lex_destroy(void)
{
    if(lex_line != NULL)
    {
        free(lex_line);
        lex_line = NULL;
    }
}

Boolean lex_getutf8(void)
{
    return lex_utf8;
}

void lex_resetparser()
{
  lex_linenum = 1;
  lex_line = NULL;
  lex_utf8 = FALSE;
#ifdef FLEX_DEBUG
  // in case of a debug build stll disable debug information
  // as it disrupts the normal doxygen output
  yy_flex_debug = 0;
#endif
}

#if USE_STATE2STRING
#include "mscgen_lexer.l.h"
#endif
/* END OF FILE */
