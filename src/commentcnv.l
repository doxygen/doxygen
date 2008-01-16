/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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

#include "bufstr.h"
#include "debug.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"


#define ADDCHAR(c)    g_outBuf->addChar(c)
#define ADDARRAY(a,s) g_outBuf->addArray(a,s)
  
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

static QCString g_aliasString;
static int      g_blockCount;
static int      g_lastBlockContext;

static void replaceCommentMarker(const char *s,int len)
{
  const char *p=s;
  char c;
  // copy blanks
  while ((c=*p) && (c==' ' || c=='\t' || c=='\n')) 
  {
    ADDCHAR(c);
    g_lineNr += c=='\n';
    p++;
  }
  // replace start of comment marker by spaces
  while ((c=*p) && (c=='/' || c=='!' || c=='#')) 
  {
    ADDCHAR(' ');
    p++;
    if (*p=='<') // comment-after-item marker 
    { 
      ADDCHAR(' '); 
      p++; 
    }
    if (c=='!') // end after first !
    {
      break;
    }
  }
  // copy comment line to output
  ADDARRAY(p,len-(p-s));
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
	g_lineNr++;
      }
    }
  }
  else
  {
    ADDARRAY(s,len);
    static int tabSize=Config_getInt("TAB_SIZE");
    for (i=0;i<len;i++) 
    {
      switch (s[i])
      {
	case '\n': g_col=0; g_lineNr++; break;
	case '\t': g_col+=tabSize-(g_col%tabSize); break;
	default:   g_col++; break;
      }
    }
  }
}

static void startCondSection(const char *sectId)
{
  g_condStack.push(new CondCtx(g_lineNr,sectId,g_skip));
  if (Config_getList("ENABLED_SECTIONS").find(sectId)!=-1)
  {
    //printf("*** Section is enabled!\n");
  }
  else
  {
    //printf("*** Section is disabled!\n");
    g_skip=TRUE;
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

#if 0
/** remove and executes cond and endcond commands in \a s */
static QCString handleCondCmdInAliases(const QCString &s)
{
  QCString result;
  //printf("handleCondCmdInAliases(%s)\n",s.data());
  static QRegExp cmdPat("[\\\\@][a-z_A-Z][a-z_A-Z0-9]*");
  int p=0,i,l;
  while ((i=cmdPat.match(s,p,&l))!=-1)
  {
    result+=s.mid(p,i-p);
    QCString cmd = s.mid(i+1,l-1);
    //printf("Found command %s\n",cmd.data());
    if (cmd=="cond")
    {
      int sp=i+l,ep;
      const char *arg=s.data()+sp;
      char c;
      // skip spaces
      while ((c=*arg) && (c==' ' || c=='\t')) arg++,sp++;
      // read argument
      if (*arg=='\n') // no arg
      {
	startCondSection(" ");
	ep=sp;
      }
      else // get argument
      {
	ep=sp;
	while ((c=*arg) && isId(c)) arg++,ep++;
	if (ep>sp)
	{
	  QCString id = s.mid(sp,ep-sp);
	  //printf("Found conditional section id %s\n",id.data());
	  startCondSection(id);
	}
	else // invalid identifier
	{
	}
      }
      p=ep;
    }
    else if (cmd=="endcond")
    {
      endCondSection();
      p=i+l;
    }
    else
    {
      result+=s.mid(i,l);
      p=i+l;
    }
  }
  result+=s.right(s.length()-p);
  return result;
}
#endif

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

<Scan>[^"'\/\n\\]*                 { /* eat anything that is not " / or \n */ 
                                     copyToOutput(yytext,yyleng); 
				   }
<Scan>"\""                         { /* start of a string */ 
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(SkipString); 
                                   }
<Scan>'				   {
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(SkipChar);
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
				     replaceAliases(yytext+i);
				     g_inSpecialComment=TRUE;
				     BEGIN(SComment); 
                                   }
<Scan>"//##Documentation".*/\n	   { /* Start of Rational Rose ANSI C++ comment block */
                                     if (g_mlBrief) REJECT;
                                     int i=17; //=strlen("//##Documentation");
				     g_blockHeadCol=g_col;
				     copyToOutput("/**",3);
				     replaceAliases(yytext+i);
				     BEGIN(SComment);
  				   }
<Scan>"//"/.*\n	                   { /* one line C++ comment */ 
  				     copyToOutput(yytext,yyleng); 
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
				   }
<Scan>"/*"			   { /* start of a C comment */
                                     copyToOutput(yytext,yyleng); 
				     BEGIN(CComment); 
                                   }
<CComment,ReadLine>[\\@]("dot"|"code"|"msc")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yytext,yyleng); 
				     g_lastCommentContext = YY_START;
				     g_blockName=&yytext[1];
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine>[\\@]("f$"|"f["|"f{"[a-z]*) {
                                     copyToOutput(yytext,yyleng); 
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
<CComment,ReadLine>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"rtfonly"|"manonly")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yytext,yyleng); 
				     g_blockName=&yytext[1];
				     g_lastCommentContext = YY_START;
                                     BEGIN(Verbatim);
                                   }
<Scan>.                            { /* any other character */
                                     copyToOutput(yytext,yyleng); 
                                   }
<Verbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"endrtfonly"|"endmanonly"|"f$"|"f]"|"f}") { /* end of verbatim block */
                                     copyToOutput(yytext,yyleng);
				     if (yytext[1]=='f') // end of formula
				     {
				       BEGIN(g_lastCommentContext);
				     }
				     else if (&yytext[4]==g_blockName)
				     {
				       BEGIN(g_lastCommentContext);
				     }
                                   }
<VerbatimCode>[\\@]("enddot"|"endcode"|"endmsc") { /* end of verbatim block */
                                     copyToOutput(yytext,yyleng);
				     if (&yytext[4]==g_blockName)
				     {
				       BEGIN(g_lastCommentContext);
				     }
                                   }
<VerbatimCode>^[ \t]*"//"[\!\/]?   { /* skip leading comments */
  				     if (!g_inSpecialComment)
				     {
                                       copyToOutput(yytext,yyleng); 
				     }
  				   }
<Verbatim,VerbatimCode>[^@\/\\\n]* { /* any character not a backslash or new line */
                                     copyToOutput(yytext,yyleng); 
                                   }
<Verbatim,VerbatimCode>\n	   { /* new line in verbatim block */
                                     copyToOutput(yytext,yyleng); 
                                   }
<Verbatim,VerbatimCode>.	   { /* any other character */
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
<SkipChar>\\.		           { /* escaped character */
                                     copyToOutput(yytext,yyleng); 
                                   }
<SkipChar>'                        { /* end of character literal */ 
                                     copyToOutput(yytext,yyleng); 
                                     BEGIN(Scan);
                                   }
<SkipChar>.                        { /* any other string character */ 
                                     copyToOutput(yytext,yyleng); 
                                   }
<SkipChar>\n                       { /* new line character */
                                     copyToOutput(yytext,yyleng); 
                                   }

<CComment>[^\\@*\n]*	           { /* anything that is not a '*' or command */ 
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
<SComment>^[ \t]*"//!"             |    // just //!
<SComment>^[ \t]*"//!<"/.*\n       |    // or   //!< something
<SComment>^[ \t]*"//!"[^<]/.*\n    {    // or   //!something
  				     replaceComment(0);
				     g_readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>\n[ \t]*"//!"            |
<SComment>\n[ \t]*"//!<"/.*\n      |
<SComment>\n[ \t]*"//!"[^<\n]/.*\n   { 
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
				     g_inSpecialComment=FALSE;
				     BEGIN(Scan); 
                                   }
<ReadLine>[^\\@\n]*/\n		   {
  				     copyToOutput(yytext,yyleng);
                                     BEGIN(g_readLineCtx);
  				   }
<CComment,ReadLine>[\\@][\\@][~a-z_A-Z][a-z_A-Z0-9]*[ \t]* { // escaped command
				     copyToOutput(yytext,yyleng);
  				   }
<CComment,ReadLine>[\\@]"cond"[ \t]+	   { // conditional section
  				     g_condCtx = YY_START; 
  				     BEGIN(CondLine);
  				   }
<CComment,ReadLine>[\\@]"endcond"/[^a-z_A-Z0-9] { // end of conditional section
  				     bool oldSkip=g_skip;
  				     endCondSection();
				     if (YY_START==CComment && oldSkip && !g_skip) 
    			             {
				       //printf("** Adding start of comment!\n");
 				       ADDCHAR('/');
     				       ADDCHAR('*');
    				     }
				    }
<CondLine>[a-z_A-Z][a-z_A-Z0-9.\-]* {
  				     bool oldSkip=g_skip;
                                     startCondSection(yytext);
				     if (g_condCtx==CComment && !oldSkip && g_skip) 
    			             {
				       //printf("** Adding terminator for comment!\n");
 				       ADDCHAR('*');
     				       ADDCHAR('/');
    				     }
  				     BEGIN(g_condCtx);
  				   }
<CondLine>[ \t]*
<CComment,ReadLine>[\\@]"cond"[ \t\r]*\n |
<CondLine>.			   { // forgot section id?
  				     if (YY_START!=CondLine) g_condCtx=YY_START;
  				     bool oldSkip=g_skip;
  				     startCondSection(" "); // fake section id causing the section to be hidden unconditionally
				     if (g_condCtx==CComment && !oldSkip && g_skip) 
    			             {
				       //printf("** Adding terminator for comment!\n");
 				       ADDCHAR('*');
     				       ADDCHAR('/');
    				     }
				     if (*yytext=='\n') g_lineNr++;
				     BEGIN(g_condCtx);
  				   }
<CComment,ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*  { // expand alias without arguments
				     replaceAliases(yytext);
  				   }
<CComment,ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*"{" { // expand alias with arguments
                                     g_lastBlockContext=YY_START;
				     g_blockCount=1;
				     g_aliasString=yytext;
				     BEGIN( ReadAliasArgs );
  				   }
<ReadAliasArgs>[^{}\n\*]+	   {
                                     g_aliasString+=yytext;
  				   }
<ReadAliasArgs>\n		   {
                                     g_aliasString+=yytext;
                                     g_lineNr++;
  				   }
<ReadAliasArgs>"{"		   {
                                     g_aliasString+=yytext;
                                     g_blockCount++;
                                   }
<ReadAliasArgs>"}"		   {
                                     g_aliasString+=yytext;
                                     g_blockCount--;
				     if (g_blockCount==0)
				     {
				       replaceAliases(g_aliasString);
				       BEGIN( g_lastBlockContext );
				     }
  			           }
<ReadAliasArgs>.		   {
                                     g_aliasString+=yytext;
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
    //printf("replaceComment(%s)\n",yytext);
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
  g_lineNr   = 0;
  g_condStack.clear();
  g_condStack.setAutoDelete(TRUE);
  BEGIN(Scan);
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

