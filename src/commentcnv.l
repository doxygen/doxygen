/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include <qstack.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qglobal.h>

#include "bufstr.h"
#include "debug.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "condparser.h"

#include <assert.h>

#define YY_NO_INPUT 1

#define ADDCHAR(c)    g_outBuf->addChar(c)
#define ADDARRAY(a,s) g_outBuf->addArray(a,s)
  
enum GuardType
{
  Guard_Cond,
  Guard_CondNot
}; 

struct CondCtx
{
  CondCtx(int line,QCString id,bool b) 
    : lineNr(line),sectionId(id), skip(b) {}
  int lineNr;
  QCString sectionId;
  bool skip;
};
  
static BufStr * g_inBuf;
static BufStr * g_outBuf;
static int      g_inBufPos;
static int      g_col;
static int      g_blockHeadCol;
static bool     g_mlBrief;
static int      g_readLineCtx;
static bool     g_skip;
static QCString g_fileName;
static int      g_lineNr;
static int      g_condCtx;
static QStack<CondCtx> g_condStack;
static QCString g_blockName;
static int      g_lastCommentContext;
static bool     g_inSpecialComment;
static bool     g_inRoseComment;
static int      g_stringContext;
static int      g_charContext;
static int      g_javaBlock;
static bool     g_specialComment;

static QCString g_aliasString;
static int      g_blockCount;
static bool     g_lastEscaped;
static int      g_lastBlockContext;
static bool     g_pythonDocString;

static GuardType        guardType;           // kind of guard for conditional section

static SrcLangExt g_lang;
static bool       isFixedForm; // For Fortran

static void replaceCommentMarker(const char *s,int len)
{
  const char *p=s;
  char c;
  // copy leading blanks
  while ((c=*p) && (c==' ' || c=='\t' || c=='\n')) 
  {
    ADDCHAR(c);
    g_lineNr += c=='\n';
    p++;
  }
  // replace start of comment marker by blanks and the last character by a *
  int blanks=0;
  while ((c=*p) && (c=='/' || c=='!' || c=='#')) 
  {
    blanks++;
    p++;
    if (*p=='<') // comment-after-item marker 
    { 
      blanks++;
      p++; 
    }
    if (c=='!') // end after first !
    {
      break;
    }
  }
  if (blanks>0)
  {
    while (blanks>2)
    {
      ADDCHAR(' ');
      blanks--;
    }
    if (blanks>1) ADDCHAR('*');
    ADDCHAR(' ');
  }
  // copy comment line to output
  ADDARRAY(p,len-(int)(p-s));
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
  int i;
  if (g_skip) // only add newlines.
  {
    for (i=0;i<len;i++) 
    {
      if (s[i]=='\n') 
      {
	ADDCHAR('\n');
	//fprintf(stderr,"---> skip %d\n",g_lineNr);
	g_lineNr++;
      }
    }
  }
  else if (len>0)
  {
    ADDARRAY(s,len);
    static int tabSize=Config_getInt("TAB_SIZE");
    for (i=0;i<len;i++) 
    {
      switch (s[i])
      {
	case '\n': g_col=0; 
	           //fprintf(stderr,"---> copy %d\n",g_lineNr);
		   g_lineNr++; break;
	case '\t': g_col+=tabSize-(g_col%tabSize); break;
	default:   g_col++; break;
      }
    }
  }
}

static void startCondSection(const char *sectId)
{
  CondParser prs;
  bool expResult = prs.parse(g_fileName,g_lineNr,sectId);
  g_condStack.push(new CondCtx(g_lineNr,sectId,g_skip));
  if (guardType == Guard_Cond) // found @cond
  {
    if (!expResult) // not enabled
    {
      g_skip=TRUE;
    }
  }
  else if (guardType == Guard_CondNot) // found @notcond
  {
    if (expResult) // enabled
    {
      g_skip=TRUE;
    }
  }
}

static void endCondSection()
{
  if (g_condStack.isEmpty())
  {
    warn(g_fileName,g_lineNr,"Found \\endcond command without matching \\cond");
    g_skip=FALSE;
  }
  else
  {
    CondCtx *ctx = g_condStack.pop();
    g_skip=ctx->skip;
  }
}

/** copies string \a s with length \a len to the output, while 
 *  replacing any alias commands found in the string.
 */
static void replaceAliases(const char *s)
{
  QCString result = resolveAliasCmd(s);
  //printf("replaceAliases(%s)->'%s'\n",s,result.data());
  copyToOutput(result,result.length());
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
%x SkipChar
%x SComment
%x CComment
%x Verbatim
%x VerbatimCode
%x ReadLine
%x CondLine
%x ReadAliasArgs

%%

<Scan>[^"'!\/\n\\#\\-]*           { /* eat anything that is not " / or \n */ 
                                     copyToOutput(yytext,(int)yyleng); 
				   }
<Scan>"\"\"\""!                     { /* start of python long comment */
                                     if (g_lang!=SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       g_pythonDocString = TRUE;
                                       copyToOutput(yytext,(int)yyleng); 
				       BEGIN(CComment);
				     }
                                   }
<Scan>![><!]/.*\n	   {
                                     if (g_lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yytext,(int)yyleng); 
				       BEGIN(CComment);
				     }
  				   }
<Scan>[Cc\*][><!]/.*\n	   {
                                     if (g_lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       /* check for fixed format; we might have some conditional as part of multilene if like C<5 .and. & */
                                       if (isFixedForm && (g_col == 0))
                                       {
                                         copyToOutput(yytext,(int)yyleng); 
				         BEGIN(CComment);
				       }
				       else
				       {
				         REJECT;
				       }
				     }
  				   }
<Scan>!.*\n		   {
  			             if (g_lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yytext,(int)yyleng); 
				     }
                                   }
<Scan>[Cc\*].*\n		   {
  			             if (g_lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       if (g_col == 0)
                                       {
                                         copyToOutput(yytext,(int)yyleng); 
				       }
				       else
				       {
				         REJECT;
				       }
				     }
                                   }
<Scan>"\""                         { /* start of a string */ 
                                     copyToOutput(yytext,(int)yyleng); 
				     g_stringContext = YY_START;
				     BEGIN(SkipString); 
                                   }
<Scan>'				   {
                                     copyToOutput(yytext,(int)yyleng); 
				     g_charContext = YY_START;
				     BEGIN(SkipChar);
  				   }
<Scan>\n                           { /* new line */ 
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<Scan>"//!"/.*\n[ \t]*"//"[\/!][^\/] | /* start C++ style special comment block */
<Scan>("///"[/]*)/[^/].*\n[ \t]*"//"[\/!][^\/] { /* start C++ style special comment block */
  				     if (g_mlBrief) 
				     {
				       REJECT; // bail out if we do not need to convert
				     }
				     else
				     {
				       int i=3;
				       if (yytext[2]=='/')
				       {
					 while (i<(int)yyleng && yytext[i]=='/') i++;
				       }
				       g_blockHeadCol=g_col;
				       copyToOutput("/**",3); 
				       replaceAliases(yytext+i);
				       g_inSpecialComment=TRUE;
				       //BEGIN(SComment); 
				       g_readLineCtx=SComment;
				       BEGIN(ReadLine);
				     }
                                   }
<Scan>"//##Documentation".*/\n	   { /* Start of Rational Rose ANSI C++ comment block */
                                     if (g_mlBrief) REJECT;
                                     int i=17; //=strlen("//##Documentation");
				     g_blockHeadCol=g_col;
				     copyToOutput("/**",3);
				     replaceAliases(yytext+i);
				     g_inRoseComment=TRUE;
				     BEGIN(SComment);
  				   }
<Scan>"//"/.*\n	                   { /* one line C++ comment */ 
				     g_inSpecialComment=yytext[2]=='/' || yytext[2]=='!';
  				     copyToOutput(yytext,(int)yyleng); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
				   }
<Scan>"/*"[*!]?			   { /* start of a C comment */
  			             g_specialComment=(int)yyleng==3;
                                     copyToOutput(yytext,(int)yyleng); 
				     BEGIN(CComment); 
                                   }
<Scan>"#"("#")?		           {
                                     if (g_lang!=SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yytext,(int)yyleng); 
				       BEGIN(CComment);
				     }
  				   }
<Scan>"--!"		           {
                                     if (g_lang!=SrcLangExt_VHDL)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yytext,(int)yyleng); 
				       BEGIN(CComment);
				     }
  				   }
<Scan>![><!]		           {
                                     if (g_lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yytext,(int)yyleng); 
				       BEGIN(CComment);
				     }
  				   }
<CComment>"{@code"/[ \t\n]	   {
                                     copyToOutput("@code",5); 
				     g_lastCommentContext = YY_START;
				     g_javaBlock=1;
				     g_blockName=&yytext[1];
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine>[\\@]("dot"|"code"|"msc")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yytext,(int)yyleng); 
				     g_lastCommentContext = YY_START;
				     g_javaBlock=0;
				     g_blockName=&yytext[1];
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine>[\\@]("f$"|"f["|"f{"[a-z]*) {
                                     copyToOutput(yytext,(int)yyleng); 
				     g_blockName=&yytext[1];
				     if (g_blockName.at(1)=='[')
				     {
				       g_blockName.at(1)=']';
				     }
				     else if (g_blockName.at(1)=='{')
				     {
				       g_blockName.at(1)='}';
				     }
				     g_lastCommentContext = YY_START;
				     BEGIN(Verbatim);
  			           }
<CComment,ReadLine>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yytext,(int)yyleng); 
				     g_blockName=&yytext[1];
				     g_lastCommentContext = YY_START;
                                     BEGIN(Verbatim);
                                   }
<Scan>.                            { /* any ather character */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<Verbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"docbookonly"|"endrtfonly"|"endmanonly"|"f$"|"f]"|"f}") { /* end of verbatim block */
                                     copyToOutput(yytext,(int)yyleng);
				     if (yytext[1]=='f') // end of formula
				     {
				       BEGIN(g_lastCommentContext);
				     }
				     else if (&yytext[4]==g_blockName)
				     {
				       BEGIN(g_lastCommentContext);
				     }
                                   }
<VerbatimCode>"{"		   {
                                     if (g_javaBlock==0)
				     {
				       REJECT;
				     }
				     else
				     {
				       g_javaBlock++;
                                       copyToOutput(yytext,(int)yyleng);
				     }
                                   }
<VerbatimCode>"}"		   {
                                     if (g_javaBlock==0)
				     {
				       REJECT;
				     }
				     else
				     {
				       g_javaBlock--;
				       if (g_javaBlock==0)
				       {
                                         copyToOutput(" @endcode ",10);
				         BEGIN(g_lastCommentContext);
				       }
				       else
				       {
                                         copyToOutput(yytext,(int)yyleng);
				       }
				     }
  				   }
<VerbatimCode>[\\@]("enddot"|"endcode"|"endmsc") { /* end of verbatim block */
                                     copyToOutput(yytext,(int)yyleng);
				     if (&yytext[4]==g_blockName)
				     {
				       BEGIN(g_lastCommentContext);
				     }
                                   }
<VerbatimCode>^[ \t]*"//"[\!\/]?   { /* skip leading comments */
  				     if (!g_inSpecialComment)
				     {
                                       copyToOutput(yytext,(int)yyleng); 
				     }
                                     else
                                     {
                                       int l=0;
                                       while (yytext[l]==' ' || yytext[l]=='\t')
                                       {
                                         l++;
                                       }
                                       copyToOutput(yytext,l);
                                       if (yyleng-l==3) // ends with //! or ///
                                       {
                                         copyToOutput(" * ",3);
                                       }
                                       else // ends with //
                                       {
                                         copyToOutput("//",2);
                                       }
                                     }
  				   }
<Verbatim,VerbatimCode>[^@\/\\\n{}]* { /* any character not a backslash or new line or } */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<Verbatim,VerbatimCode>\n	   { /* new line in verbatim block */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<Verbatim>^[ \t]*"///"             {
  				     if (g_blockName=="dot" || g_blockName=="msc" || g_blockName.at(0)=='f')
				     {
				       // see bug 487871, strip /// from dot images and formulas.
                                       int l=0;
                                       while (yytext[l]==' ' || yytext[l]=='\t')
                                       {
                                         l++;
                                       }
                                       copyToOutput(yytext,l);
				       copyToOutput("   ",3);
				     }
				     else // even slashes are verbatim (e.g. \verbatim, \code)
				     {
				       REJECT;
				     }
  				   }
<Verbatim,VerbatimCode>.	   { /* any other character */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<SkipString>\\.                    { /* escaped character in string */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<SkipString>"\""       	           { /* end of string */ 
                                     copyToOutput(yytext,(int)yyleng); 
				     BEGIN(g_stringContext); 
                                   }
<SkipString>.                      { /* any other string character */ 
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<SkipString>\n                     { /* new line inside string (illegal for some compilers) */ 
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<SkipChar>\\.		           { /* escaped character */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<SkipChar>'                        { /* end of character literal */ 
                                     copyToOutput(yytext,(int)yyleng); 
                                     BEGIN(g_charContext);
                                   }
<SkipChar>.                        { /* any other string character */ 
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<SkipChar>\n                       { /* new line character */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }

<CComment>[^\\!@*\n{\"]*           { /* anything that is not a '*' or command */ 
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<CComment>"*"+[^*/\\@\n{\"]*       { /* stars without slashes */
                                     copyToOutput(yytext,(int)yyleng); 
                                   }
<CComment>"\"\"\""                 { /* end of Python docstring */
                                     if (g_lang!=SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       g_pythonDocString = FALSE;
				       copyToOutput(yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
  				   }
<CComment>\n                       { /* new line in comment */
                                     copyToOutput(yytext,(int)yyleng); 
                                     /* in case of Fortran always end of comment */
  				     if (g_lang==SrcLangExt_Fortran)
				     {
				       BEGIN(Scan);
				     }
                                   }
<CComment>"*"+"/"                  { /* end of C comment */
                                     if (g_lang==SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
                                   }
<CComment>"\n"/[ \t]*[^#]	   { /* end of Python comment */
                                     if (g_lang!=SrcLangExt_Python || g_pythonDocString)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
  				   }
<CComment>"\n"/[ \t]*[^\-]	   { /* end of VHDL comment */
                                     if (g_lang!=SrcLangExt_VHDL)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
  				   }
   /* removed for bug 674842 (bug was introduced in rev 768)
<CComment>"'"			   {
  			             g_charContext = YY_START;
				     copyToOutput(yytext,(int)yyleng);
				     BEGIN(SkipChar);
  				   }
<CComment>"\""			   {
  			             g_stringContext = YY_START;
				     copyToOutput(yytext,(int)yyleng);
				     BEGIN(SkipString);
  				   }
   */
<CComment>.			   {
                                     copyToOutput(yytext,(int)yyleng); 
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
<SComment>^[ \t]*"//!"             |    // just //!
<SComment>^[ \t]*"//!<"/.*\n       |    // or   //!< something
<SComment>^[ \t]*"//!"[^<]/.*\n    {    // or   //!something
  				     replaceComment(0);
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>\n[ \t]*"//!"            |
<SComment>\n[ \t]*"//!<"/.*\n      |
<SComment>\n[ \t]*"//!"[^<\n]/.*\n { 
                                     replaceComment(1); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>^[ \t]*"//##"/.*\n       {
                                     if (!g_inRoseComment)
				     {
				       REJECT;
				     }
				     else
				     {
  				       replaceComment(0);
				       g_readLineCtx=YY_START;
				       BEGIN(ReadLine);
				     }
                                   }
<SComment>\n[ \t]*"//##"/.*\n      {
                                     if (!g_inRoseComment)
				     {
				       REJECT;
				     }
				     else
				     {
                                       replaceComment(1); 
				       g_readLineCtx=YY_START;
				       BEGIN(ReadLine);
				     }
                                   }
<SComment>\n			   { /* end of special comment */
                                     copyToOutput(" */",3); 
				     copyToOutput(yytext,(int)yyleng); 
				     g_inSpecialComment=FALSE;
				     g_inRoseComment=FALSE;
				     BEGIN(Scan); 
                                   }
<ReadLine>[^\\@\n]*/\n		   {
  				     copyToOutput(yytext,(int)yyleng);
                                     BEGIN(g_readLineCtx);
  				   }
<CComment,ReadLine>[\\@][\\@][~a-z_A-Z][a-z_A-Z0-9]*[ \t]* { // escaped command
				     copyToOutput(yytext,(int)yyleng);
  				   }
<CComment,ReadLine>[\\@]"cond"[ \t]+	   { // conditional section
  				     g_condCtx = YY_START; 
                                     guardType = Guard_Cond;
  				     BEGIN(CondLine);
  				   }
<CComment,ReadLine>[\\@]"endcond"/[^a-z_A-Z0-9] { // end of conditional section
  				     bool oldSkip=g_skip;
  				     endCondSection();
				     if (YY_START==CComment && oldSkip && !g_skip) 
    			             {
				       //printf("** Adding start of comment!\n");
				       if (g_lang!=SrcLangExt_Python &&
					   g_lang!=SrcLangExt_VHDL &&
					   g_lang!=SrcLangExt_Fortran)
				       {
 				         ADDCHAR('/');
     				         ADDCHAR('*');
					 if (g_specialComment)
					 {
					   ADDCHAR('*');
					 }
				       }
    				     }
				    }
<CondLine>[!()&| \ta-z_A-Z0-9.\-]+ {
  				     bool oldSkip=g_skip;
                                     startCondSection(yytext);
				     if ((g_condCtx==CComment || g_readLineCtx==SComment) && 
                                         !oldSkip && g_skip) 
    			             {
				       if (g_lang!=SrcLangExt_Python &&
					   g_lang!=SrcLangExt_VHDL &&
					   g_lang!=SrcLangExt_Fortran)
				       {
 				         ADDCHAR('*');
     				         ADDCHAR('/');
				       }
    				     }
                                     if (g_readLineCtx==SComment)
                                     {
                                       BEGIN(SComment);
                                     }
                                     else
                                     {
  				       BEGIN(g_condCtx);
                                     }
  				   }
<CondLine>[ \t]*
<CComment,ReadLine>[\\@]"cond"[ \t\r]*/\n |
<CondLine>.			   { // forgot section id?
                                     guardType = Guard_Cond;
  				     if (YY_START!=CondLine) g_condCtx=YY_START;
  				     bool oldSkip=g_skip;
  				     startCondSection(" "); // fake section id causing the section to be hidden unconditionally
				     if ((g_condCtx==CComment || g_readLineCtx==SComment) && 
                                         !oldSkip && g_skip) 
    			             {
				       //printf("** Adding terminator for comment!\n");
				       if (g_lang!=SrcLangExt_Python &&
					   g_lang!=SrcLangExt_VHDL)
				       {
 				         ADDCHAR('*');
     				         ADDCHAR('/');
				       }
    				     }
				     if (*yytext=='\n') g_lineNr++;
                                     if (g_readLineCtx==SComment)
                                     {
                                       BEGIN(SComment);
                                     }
                                     else
                                     {
  				       BEGIN(g_condCtx);
                                     }
  				   }
<CComment,ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*  { // expand alias without arguments
				     replaceAliases(yytext);
  				   }
<CComment,ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*"{" { // expand alias with arguments
                                     g_lastBlockContext=YY_START;
				     g_blockCount=1;
				     g_aliasString=yytext;
				     g_lastEscaped=0;
				     BEGIN( ReadAliasArgs );
  				   }
<ReadAliasArgs>^[ \t]*"//"[/!]/[^\n]+   { // skip leading special comments (see bug 618079)
  				   }
<ReadAliasArgs>"*/"		   { // oops, end of comment in the middle of an alias?
                                     if (g_lang==SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else // abort the alias, restart scanning
				     {
				       copyToOutput(g_aliasString,g_aliasString.length());
				       copyToOutput(yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
  				   }
<ReadAliasArgs>[^{}\n\\\*]+	   {
                                     g_aliasString+=yytext;
				     g_lastEscaped=FALSE;
  				   }
<ReadAliasArgs>"\\"		   {
                                     if (g_lastEscaped)  g_lastEscaped=FALSE;
                                     else                g_lastEscaped=TRUE;
                                     g_aliasString+=yytext;
                                   }
<ReadAliasArgs>\n		   {
                                     g_aliasString+=yytext;
                                     g_lineNr++;
				     g_lastEscaped=FALSE;
  				   }
<ReadAliasArgs>"{"		   {
                                     g_aliasString+=yytext;
                                     if (!g_lastEscaped) g_blockCount++;
				     g_lastEscaped=FALSE;
                                   }
<ReadAliasArgs>"}"		   {
                                     g_aliasString+=yytext;
				     if (!g_lastEscaped) g_blockCount--;
				     if (g_blockCount==0)
				     {
				       replaceAliases(g_aliasString);
				       BEGIN( g_lastBlockContext );
				     }
				     g_lastEscaped=FALSE;
  			           }
<ReadAliasArgs>.		   {
                                     g_aliasString+=yytext;
				     g_lastEscaped=FALSE;
  				   }
<ReadLine>.			   {
  				     copyToOutput(yytext,(int)yyleng);
  				   }

%%

void replaceComment(int offset)
{
  if (g_mlBrief || g_skip)
  {
    copyToOutput(yytext,(int)yyleng);
  }
  else
  {
    //printf("replaceComment(%s)\n",yytext);
    int i=computeIndent(&yytext[offset]);
    if (i==g_blockHeadCol)
    {
      replaceCommentMarker(yytext,(int)yyleng);
    }
    else
    {
      copyToOutput(" */",3);
      int i;for (i=(int)yyleng-1;i>=0;i--) unput(yytext[i]);
      g_inSpecialComment=FALSE;
      BEGIN(Scan);                                            
    }                                                         
  }
}

// simplified way to know if this is fixed form
// duplicate in fortrancode.l
static bool recognizeFixedForm(const char* contents)
{
  int column=0;
  bool skipLine=FALSE;

  for(int i=0;;i++) {
    column++;

    switch(contents[i]) {
      case '\n':
        column=0;
        skipLine=FALSE;
        break;
      case ' ':
        break;
      case '\000':
        return FALSE;
      case 'C':
      case 'c':
      case '*':
        if(column==1) return TRUE;
        if(skipLine) break;
        return FALSE;
      case '!':
        if(column>1 && column<7) return FALSE;
        skipLine=TRUE;
        break;
      default:
        if(skipLine) break;
        if(column==7) return TRUE;
        return FALSE;
    }
  }
  return FALSE;
}


/*! This function does three things:
 *  -# It converts multi-line C++ style comment blocks (that are aligned)
 *     to C style comment blocks (if MULTILINE_CPP_IS_BRIEF is set to NO).
 *  -# It replaces aliases with their definition (see ALIASES)
 *  -# It handles conditional sections (cond...endcond blocks)
 */
void convertCppComments(BufStr *inBuf,BufStr *outBuf,const char *fileName)
{
  //printf("convertCppComments(%s)\n",fileName);
  g_inBuf    = inBuf;
  g_outBuf   = outBuf;
  g_inBufPos = 0;
  g_col      = 0;
  g_mlBrief = Config_getBool("MULTILINE_CPP_IS_BRIEF");
  g_skip     = FALSE;
  g_fileName = fileName;
  g_lang = getLanguageFromFileName(fileName);
  g_pythonDocString = FALSE;
  g_lineNr   = 1;
  g_condStack.clear();
  g_condStack.setAutoDelete(TRUE);

  isFixedForm = FALSE;
  if (g_lang==SrcLangExt_Fortran)
  {
    isFixedForm = recognizeFixedForm(inBuf->data());
  }

  if (g_lang==SrcLangExt_Markdown)
  {
    BEGIN(CComment);
  }
  else
  {
    BEGIN(Scan);
  }
  yylex();
  while (!g_condStack.isEmpty())
  {
    CondCtx *ctx = g_condStack.pop();
    QCString sectionInfo = " ";
    if (ctx->sectionId!=" ") sectionInfo.sprintf(" with label %s ",ctx->sectionId.data()); 
    warn(g_fileName,ctx->lineNr,"Conditional section%sdoes not have "
	"a corresponding \\endcond command within this file.",sectionInfo.data());
  }
  if (Debug::isFlagSet(Debug::CommentCnv))
  {
    g_outBuf->at(g_outBuf->curPos())='\0';
    msg("-------------\n%s\n-------------\n",g_outBuf->data());
  }
}


//----------------------------------------------------------------------------
#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
    void commentcnvYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

