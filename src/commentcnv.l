/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

%{

#define YY_NEVER_INTERACTIVE 1
  
#include <stdio.h>
#include <stdlib.h>

#include "bufstr.h"
#include "debug.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"

static BufStr *g_inBuf;
static BufStr *g_outBuf;
static int     g_inBufPos;
static int     g_col;
static int     g_blockHeadCol;
static bool    g_mlBrief;
static int     g_readLineCtx;

static void replaceCommentMarker(const char *s,int len)
{
  const char *p=s;
  char c;
  // copy blanks
  while ((c=*p) && (c==' ' || c=='\t' || c=='\n')) 
  {
    g_outBuf->addChar(c);
    p++;
  }
  // replace start of comment marker by spaces
  while ((c=*p) && (c=='/' || c=='!' || c=='#')) 
  {
    g_outBuf->addChar(' ');
    p++;
    if (*p=='<') // comment-after-item marker 
    { 
      g_outBuf->addChar(' '); 
      p++; 
    }
    if (c=='!') // end after first !
    {
      break;
    }
  }
  // copy comment line to output
  g_outBuf->addArray(p,len-(p-s));
}

static inline int computeIndent(const char *s)
{
  int col=0;
  static int tabSize=Config_getInt("TAB_SIZE");
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    if (c==' ') col++;
    else if (c=='\t') col+=tabSize-(col%tabSize); 
    else break;
  }
  return col;
}

static inline void copyToOutput(const char *s,int len)
{
  g_outBuf->addArray(s,len);
  int i;
  static int tabSize=Config_getInt("TAB_SIZE");
  for (i=0;i<len;i++) 
  {
    switch (s[i])
    {
      case '\n': g_col=0; break;
      case '\t': g_col+=tabSize-(g_col%tabSize); break;
      default:   g_col++; break;
    }
  }
}

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int bytesInBuf = g_inBuf->curPos()-g_inBufPos;
  int bytesToCopy = QMIN(max_size,bytesInBuf);
  memcpy(buf,g_inBuf->data()+g_inBufPos,bytesToCopy);
  g_inBufPos+=bytesToCopy;
  return bytesToCopy;
}

void replaceComment(int offset);

%}

%option noyywrap

%x Scan
%x SkipString
%x SComment
%x CComment
%x Verbatim
%x ReadLine

%%

<Scan>[^\"\/\n\\]*                 { /* eat anything that is not " / or \n */ 
                                     copyToOutput(yytext,yyleng); 
				   }
<Scan>"\""                         { /* start of a string */ 
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(SkipString); 
                                   }
<Scan>\n                           { /* new line */ 
                                     copyToOutput(yytext,yyleng); 
                                   }
<Scan>("//!"|"///").*/\n[ \t]*"//"[\/!][^\/] { /* start C++ style special comment block */
  				     if (g_mlBrief) REJECT; // bail out if we do not need to convert
  				     int i=3;
				     if (yytext[2]=='/')
				     {
				       while (i<yyleng && yytext[i]=='/') i++;
				     }
				     g_blockHeadCol=g_col;
                                     copyToOutput("/**",3); 
				     copyToOutput(yytext+i,yyleng-i); 
				     BEGIN(SComment); 
                                   }
<Scan>"//##Documentation".*/\n	   { /* Start of Rational Rose ANSI C++ comment block */
                                     if (g_mlBrief) REJECT;
                                     int i=17; //=strlen("//##Documentation");
				     g_blockHeadCol=g_col;
				     copyToOutput("/**",3);
				     copyToOutput(yytext+i,yyleng-i);
				     BEGIN(SComment);
  				   }
<Scan>"//"/.*\n		           { /* one line C++ comment */ 
  				     copyToOutput(yytext,yyleng); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
				   }
<Scan>"/*"			   { /* start of a C comment */
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(CComment); 
                                   }
<Scan>"\\verbatim"		   { /* start of a verbatim block */
                                     copyToOutput(yytext,yyleng); 
                                     BEGIN(Verbatim);
                                   }
<Scan>.                            { /* any other character */
                                     copyToOutput(yytext,yyleng); 
                                   }
<Verbatim>"\\endverbatim"          { /* end of verbatim block */
                                     copyToOutput(yytext,yyleng);
				     BEGIN(Scan);
                                   }
<Verbatim>[^\\\n]*		   { /* any character not a backslash or new line */
                                     copyToOutput(yytext,yyleng); 
                                   }
<Verbatim>\n			   { /* new line in verbatim block */
                                     copyToOutput(yytext,yyleng); 
                                   }
<Verbatim>.			   { /* any other character */
                                     copyToOutput(yytext,yyleng); 
                                   }
<SkipString>\\.                    { /* escaped character in string */
                                     copyToOutput(yytext,yyleng); 
                                   }
<SkipString>"\""       	           { /* end of string */ 
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(Scan); 
                                   }
<SkipString>.                      { /* any other string character */ 
                                     copyToOutput(yytext,yyleng); 
                                   }
<SkipString>\n                     { /* new line inside string (illegal for some compilers) */ 
                                     copyToOutput(yytext,yyleng); 
                                   }
<CComment>[^\\@*\n]*	           { /* anything that is not a '*' */ 
                                     copyToOutput(yytext,yyleng); 
                                   }
<CComment>"*"+[^*/\\@\n]*          { /* stars without slashes */
                                     copyToOutput(yytext,yyleng); 
                                   }
<CComment>\n                       { /* new line in comment */
                                     copyToOutput(yytext,yyleng); 
                                   }
<CComment>"*"+"/"                  { /* end of C comment */
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(Scan); 
                                   }
<CComment>[\\@][a-z_A-Z][a-z_A-Z0-9]*   { // expand alias
  				      QCString *pValue=Doxygen::aliasDict[yytext+1];
				      if (pValue)
				      {    
					copyToOutput(pValue->data(),pValue->length());
				      }
				      else
				      {
					copyToOutput(yytext,yyleng);
				      }
  				   }
<CComment>.			   {
                                     copyToOutput(yytext,yyleng); 
  				   }
<SComment>^[ \t]*"///"[\/]*/\n     {
  				     replaceComment(0);
  				   }
<SComment>\n[ \t]*"///"[\/]*/\n    {
                                     replaceComment(1); 
                                   }
<SComment>^[ \t]*"///"[^\/\n]/.*\n { 
  				     replaceComment(0);
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
  				   }
<SComment>\n[ \t]*"///"[^\/\n]/.*\n  { 
                                     replaceComment(1); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
  				   }
<SComment>^[ \t]*"//!"/.*\n        { 
  				     replaceComment(0);
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>\n[ \t]*"//!"/.*\n       { 
                                     replaceComment(1); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>^[ \t]*"//##"/.*\n       {
  				     replaceComment(0);
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>\n[ \t]*"//##"/.*\n      {
                                     replaceComment(1); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>\n			   { /* end of special comment */
                                     copyToOutput(" */",3); 
				     copyToOutput(yytext,yyleng); 
				     BEGIN(Scan); 
                                   }
<ReadLine>[^\\@\n]*/\n		   {
  				     copyToOutput(yytext,yyleng);
                                     BEGIN(g_readLineCtx);
  				   }
<ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*   { // expand alias
  				      QCString *pValue=Doxygen::aliasDict[yytext+1];
				      if (pValue)
				      {    
					copyToOutput(pValue->data(),pValue->length());
				      }
				      else
				      {
					copyToOutput(yytext,yyleng);
				      }
  				   }
<ReadLine>.			   {
  				     copyToOutput(yytext,yyleng);
  				   }

%%

void replaceComment(int offset)                                
{
  if (g_mlBrief)                                              
  {                                                           
    copyToOutput(yytext,yyleng); 
  }                                                           
  else                                                        
  {                                                           
    int i=computeIndent(&yytext[offset]);                     
    if (i==g_blockHeadCol)                                    
    {                                                         
      replaceCommentMarker(yytext,yyleng);                    
    }                                                         
    else                                                      
    {                                                         
      copyToOutput(" */",3);                                  
      int i;for (i=yyleng-1;i>=0;i--) unput(yytext[i]);       
      BEGIN(Scan);                                            
    }                                                         
  }
}

/*! This function does two things:
 *  -# It converts multi-line C++ style comment blocks (that are aligned)
 *     to C style comment blocks (if MULTILINE_CPP_IS_BRIEF is set to NO).
 *  -# It replaces aliases with their definition (see ALIASES)
 */
void convertCppComments(BufStr *inBuf,BufStr *outBuf)
{
  g_inBuf    = inBuf;
  g_outBuf   = outBuf;
  g_inBufPos = 0;
  g_col      = 0;
  g_mlBrief = Config_getBool("MULTILINE_CPP_IS_BRIEF");
  BEGIN(Scan);
  yylex();
  if (Debug::isFlagSet(Debug::CommentCnv))
  {
    msg("-------------\n%s\n-------------\n",g_outBuf->data());
  }
}

//----------------------------------------------------------------------------
#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
    void commentcnvYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

