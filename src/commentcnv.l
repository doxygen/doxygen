/*****************************************************************************
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
%option prefix="commentcnvYY"
%option reentrant
%option extra-type="struct commentcnvYY_state *"
%top{
#include <stdint.h>
}

%{

  
#include <stdio.h>
#include <stdlib.h>
#include <stack>

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
#define YY_NO_UNISTD_H 1

#define ADDCHAR(c)    yyextra->outBuf->addChar(c)
#define ADDARRAY(a,s) yyextra->outBuf->addArray(a,s)

#define USE_STATE2STRING 0
  
struct CondCtx
{
  CondCtx(int line,QCString id,bool b) 
    : lineNr(line),sectionId(id), skip(b) {}
  int lineNr;
  QCString sectionId;
  bool skip;
};
  
struct CommentCtx
{
  CommentCtx(int line) 
    : lineNr(line) {}
  int lineNr;
};
  
struct commentcnvYY_state
{
  BufStr * inBuf = 0;
  BufStr * outBuf = 0;
  yy_size_t inBufPos = 0;
  int      col = 0;
  int      blockHeadCol = 0;
  bool     mlBrief = FALSE;
  int      readLineCtx = 0;
  bool     skip = FALSE;
  QCString fileName;
  int      lineNr = 0;
  int      condCtx = 0;
  std::stack<CondCtx> condStack;
  std::stack<int> commentStack;
  QCString blockName;
  int      lastCommentContext = 0;
  bool     inSpecialComment = FALSE;
  bool     inRoseComment= FALSE;
  int      stringContext = 0;
  int      charContext = 0;
  int      javaBlock = 0;
  bool     specialComment = FALSE;

  QCString aliasString;
  int      blockCount = 0;
  bool     lastEscaped = FALSE;
  int      lastBlockContext= 0;
  bool     pythonDocString = FALSE;
  int      nestingCount= 0;

  bool     vhdl = FALSE; // for VHDL old style --! comment

  SrcLangExt lang = SrcLangExt_Unknown;
  bool       isFixedForm = FALSE; // For Fortran
};

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif
static inline int computeIndent(const char *s);

static void replaceCommentMarker(yyscan_t yyscanner,const char *s,int len);
static inline void copyToOutput(yyscan_t yyscanner,const char *s,int len);
static void startCondSection(yyscan_t yyscanner,const char *sectId);
static void endCondSection(yyscan_t yyscanner);
static void handleCondSectionId(yyscan_t yyscanner,const char *expression);
static void replaceAliases(yyscan_t yyscanner,const char *s);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);
static void replaceComment(yyscan_t yyscanner,int offset);
static void clearCommentStack(yyscan_t yyscanner);




#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);


%}

MAILADR   ("mailto:")?[a-z_A-Z0-9.+-]+"@"[a-z_A-Z0-9-]+("."[a-z_A-Z0-9\-]+)+[a-z_A-Z0-9\-]+

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

  //- start: NUMBER -------------------------------------------------------------------------
  // Note same defines in code.l: keep in sync
DECIMAL_INTEGER  [1-9][0-9']*[0-9]?[uU]?[lL]?[lL]?
HEXADECIMAL_INTEGER  "0"[xX][0-9a-zA-Z']+[0-9a-zA-Z]?
OCTAL_INTEGER  "0"[0-7][0-7']+[0-7]?
BINARY_INTEGER  "0"[bB][01][01']*[01]?
INTEGER_NUMBER {DECIMAL_INTEGER}|{HEXADECIMAL_INTEGER}|{OCTAL_INTEGER}|{BINARY_INTEGER}

FP_SUF [fFlL]

DIGIT_SEQ [0-9][0-9']*[0-9]?
FRAC_CONST {DIGIT_SEQ}"."|{DIGIT_SEQ}?"."{DIGIT_SEQ}
FP_EXP [eE][+-]?{DIGIT_SEQ}
DEC_FP1 {FRAC_CONST}{FP_EXP}?{FP_SUF}?
DEC_FP2 {DIGIT_SEQ}{FP_EXP}{FP_SUF}

HEX_DIGIT_SEQ [0-9a-fA-F][0-9a-fA-F']*[0-9a-fA-F]?
HEX_FRAC_CONST {HEX_DIGIT_SEQ}"."|{HEX_DIGIT_SEQ}?"."{HEX_DIGIT_SEQ}
BIN_EXP [pP][+-]?{DIGIT_SEQ}
HEX_FP1 "0"[xX]{HEX_FRAC_CONST}{BIN_EXP}{FP_SUF}?
HEX_FP2 "0"[xX]{HEX_DIGIT_SEQ}{BIN_EXP}{FP_SUF}?

FLOAT_DECIMAL {DEC_FP1}|{DEC_FP2}
FLOAT_HEXADECIMAL {HEX_FP1}|{HEX_FP2}
FLOAT_NUMBER {FLOAT_DECIMAL}|{FLOAT_HEXADECIMAL}
NUMBER {INTEGER_NUMBER}|{FLOAT_NUMBER}
  //- end: NUMBER ---------------------------------------------------------------------------

  // C start comment 
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment 
CPPC  "/\/"

  // Optional any character
ANYopt .*

  // Optional white space
WSopt [ \t\r]*
  // readline non special
RLopt [^\\@\n\*\/]*
  // Optional slash
SLASHopt [/]*

%%

<Scan>{NUMBER}			    { //Note similar code in code.l
                                      if (yyextra->lang!=SrcLangExt_Cpp) REJECT;
                                      copyToOutput(yyscanner,yytext,(int)yyleng); 
                                    }
<Scan>[^"'!\/\n\\#,\-=; \t]*        { /* eat anything that is not " / , or \n */
                                       copyToOutput(yyscanner,yytext,(int)yyleng);
                                    }
<Scan>[,= ;\t]                      { /* eat , so we have a nice separator in long initialization lines */ 
                                       copyToOutput(yyscanner,yytext,(int)yyleng);
                                    }
<Scan>"\"\"\""!                     { /* start of python long comment */
                                     if (yyextra->lang!=SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->pythonDocString = TRUE;
                                       yyextra->nestingCount=1;
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       copyToOutput(yyscanner,yytext,(int)yyleng);
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
                                   }
<Scan>![><!]/.*\n	   {
                                     if (yyextra->lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
                                       yyextra->nestingCount=0; // Fortran doesn't have an end comment
                                       clearCommentStack(yyscanner); /*  to be on the save side */
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
  				   }
<Scan>[Cc\*][><!]/.*\n	   {
                                     if (yyextra->lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       /* check for fixed format; we might have some conditional as part of multiline if like C<5 .and. & */
                                       if (yyextra->isFixedForm && (yyextra->col == 0))
                                       {
                                         copyToOutput(yyscanner,yytext,(int)yyleng); 
                                         yyextra->nestingCount=0; // Fortran doesn't have an end comment
                                         clearCommentStack(yyscanner); /*  to be on the save side */
				         BEGIN(CComment);
                                         yyextra->commentStack.push(yyextra->lineNr);
				       }
				       else
				       {
				         REJECT;
				       }
				     }
  				   }
<Scan>!.*\n		   {
  			             if (yyextra->lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
				     }
                                   }
<Scan>[Cc\*].*\n		   {
  			             if (yyextra->lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       if (yyextra->col == 0)
                                       {
                                         copyToOutput(yyscanner,yytext,(int)yyleng); 
				       }
				       else
				       {
				         REJECT;
				       }
				     }
                                   }
<Scan>"\""                         { /* start of a string */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->stringContext = YY_START;
				     BEGIN(SkipString); 
                                   }
<Scan>'				   {
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->charContext = YY_START;
                                     if (yyextra->lang!=SrcLangExt_VHDL)
                                     {
				       BEGIN(SkipChar);
                                     }
  				   }
<Scan>\n                           { /* new line */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<Scan>{CPPC}"!"/.*\n[ \t]*{CPPC}[\/!][^\/] | /* start C++ style special comment block */
<Scan>({CPPC}"/"[/]*)/[^/].*\n[ \t]*{CPPC}[\/!][^\/] { /* start C++ style special comment block */
  				     if (yyextra->mlBrief) 
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
				       yyextra->blockHeadCol=yyextra->col;
				       copyToOutput(yyscanner,"/**",3); 
				       replaceAliases(yyscanner,yytext+i);
				       yyextra->inSpecialComment=TRUE;
				       //BEGIN(SComment); 
				       yyextra->readLineCtx=SComment;
				       BEGIN(ReadLine);
				     }
                                   }
<Scan>{CPPC}"##Documentation"{ANYopt}/\n	   { /* Start of Rational Rose ANSI C++ comment block */
                                     if (yyextra->mlBrief) REJECT;
                                     int i=17; //=strlen("//##Documentation");
				     yyextra->blockHeadCol=yyextra->col;
				     copyToOutput(yyscanner,"/**",3);
				     replaceAliases(yyscanner,yytext+i);
				     yyextra->inRoseComment=TRUE;
				     BEGIN(SComment);
  				   }
<Scan>{CPPC}[!\/]/.*\n[ \t]*{CPPC}[|\/][ \t]*[@\\]"}" { // next line contains an end marker, see bug 752712
				     yyextra->inSpecialComment=yytext[2]=='/' || yytext[2]=='!';
  				     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<Scan>{CPPC}/.*\n	                   { /* one line C++ comment */ 
				     yyextra->inSpecialComment=yytext[2]=='/' || yytext[2]=='!';
  				     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
				   }
<Scan>{CCS}{CCE}                       { /* avoid matching next rule for empty C comment, see bug 711723 */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
<Scan>{CCS}[*!]?			   { /* start of a C comment */
                                     if (yyextra->lang==SrcLangExt_Python)
				     {
				       REJECT;
                                     }
  			             yyextra->specialComment=(int)yyleng==3;
                                     yyextra->nestingCount=1;
                                     clearCommentStack(yyscanner); /*  to be on the save side */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     BEGIN(CComment); 
                                     yyextra->commentStack.push(yyextra->lineNr);
                                   }
<Scan>"#"("#")?		           {
                                     if (yyextra->lang!=SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
                                       yyextra->nestingCount=0; // Python doesn't have an end comment for #
                                       clearCommentStack(yyscanner); /*  to be on the save side */
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
  				   }
<Scan>"--"[^!][^\n]*		   {
                                     if (yyextra->lang!=SrcLangExt_VHDL)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
				     }
  				   }
<Scan>"--!"		           {
                                     if (yyextra->lang!=SrcLangExt_VHDL)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->vhdl = TRUE;
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
                                       yyextra->nestingCount=0;  // VHDL doesn't have an end comment
                                       clearCommentStack(yyscanner); /*  to be on the save side */
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
  				   }
<Scan>![><!]		           {
                                     if (yyextra->lang!=SrcLangExt_Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
                                       yyextra->nestingCount=0;  // Fortran doesn't have an end comment
                                       clearCommentStack(yyscanner); /*  to be on the save side */
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
  				   }
<CComment,ReadLine>{MAILADR}      |
<CComment,ReadLine>"<"{MAILADR}">" { // Mail address, to prevent seeing e.g x@code-factory.org as start of a code block
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
<CComment>"{@code"/[ \t\n]	   {
                                     copyToOutput(yyscanner,"@code",5); 
				     yyextra->lastCommentContext = YY_START;
				     yyextra->javaBlock=1;
				     yyextra->blockName=&yytext[1];
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine>^[ \t]*("```"[`]*|"~~~"[~]*) { /* start of markdown code block */
                                     if (!Config_getBool(MARKDOWN_SUPPORT))
                                     {
                                       REJECT;
                                     }
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                     yyextra->lastCommentContext = YY_START;
                                     yyextra->javaBlock=0;
                                     yyextra->blockName=QCString(yytext).stripWhiteSpace().left(3);
                                     BEGIN(VerbatimCode);
                                   }
<CComment,ReadLine>[\\@]("dot"|"code"|"msc"|"startuml")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->lastCommentContext = YY_START;
				     yyextra->javaBlock=0;
                                     if (qstrcmp(&yytext[1],"startuml")==0)
                                     {
                                       yyextra->blockName="uml";
                                     }
                                     else
                                     {
				       yyextra->blockName=&yytext[1];
                                     }
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine>[\\@]("f$"|"f["|"f{") {
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->blockName=&yytext[1];
				     if (yyextra->blockName.at(1)=='[')
				     {
				       yyextra->blockName.at(1)=']';
				     }
				     else if (yyextra->blockName.at(1)=='{')
				     {
				       yyextra->blockName.at(1)='}';
				     }
				     yyextra->lastCommentContext = YY_START;
				     BEGIN(Verbatim);
  			           }
<CComment,ReadLine>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->blockName=&yytext[1];
				     yyextra->lastCommentContext = YY_START;
                                     BEGIN(Verbatim);
                                   }
<Scan>"\\\""                       { /* escaped double quote */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
<Scan>"\\\\"                       { /* escaped backslash */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
<Scan>.                            { /* any other character */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
<Verbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"f$"|"f]"|"f}") { /* end of verbatim block */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
				     if (&yytext[1]==yyextra->blockName) // end of formula
				     {
				       BEGIN(yyextra->lastCommentContext);
				     }
				     else if (&yytext[4]==yyextra->blockName)
				     {
				       BEGIN(yyextra->lastCommentContext);
				     }
                                   }
<VerbatimCode>"{"		   {
                                     if (yyextra->javaBlock==0)
				     {
				       REJECT;
				     }
				     else
				     {
				       yyextra->javaBlock++;
                                       copyToOutput(yyscanner,yytext,(int)yyleng);
				     }
                                   }
<VerbatimCode>"}"		   {
                                     if (yyextra->javaBlock==0)
				     {
				       REJECT;
				     }
				     else
				     {
				       yyextra->javaBlock--;
				       if (yyextra->javaBlock==0)
				       {
                                         copyToOutput(yyscanner," @endcode ",10);
				         BEGIN(yyextra->lastCommentContext);
				       }
				       else
				       {
                                         copyToOutput(yyscanner,yytext,(int)yyleng);
				       }
				     }
  				   }
<VerbatimCode>("```"[`]*|"~~~"[~]*) { /* end of markdown code block */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                     if (yytext[0]==yyextra->blockName[0])
                                     {
                                       BEGIN(yyextra->lastCommentContext);
                                     }
                                   }
<VerbatimCode>[\\@]("enddot"|"endcode"|"endmsc"|"enduml") { /* end of verbatim block */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
				     if (&yytext[4]==yyextra->blockName)
				     {
				       BEGIN(yyextra->lastCommentContext);
				     }
                                   }
<VerbatimCode>^[ \t]*{CPPC}[\!\/]?   { /* skip leading comments */
  				     if (!yyextra->inSpecialComment)
				     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng); 
				     }
                                     else
                                     {
                                       int l=0;
                                       while (yytext[l]==' ' || yytext[l]=='\t')
                                       {
                                         l++;
                                       }
                                       copyToOutput(yyscanner,yytext,l);
                                       if (yyleng-l==3) // ends with //! or ///
                                       {
                                         copyToOutput(yyscanner," * ",3);
                                       }
                                       else // ends with //
                                       {
                                         copyToOutput(yyscanner,"//",2);
                                       }
                                     }
  				   }
<Verbatim,VerbatimCode>[^`~@\/\\\n{}]* { /* any character not a backslash or new line or } */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<Verbatim,VerbatimCode>\n	   { /* new line in verbatim block */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<Verbatim>^[ \t]*{CPPC}[/!]          {
  				     if (yyextra->blockName=="dot" || yyextra->blockName=="msc" || yyextra->blockName=="uml" || yyextra->blockName.at(0)=='f')
				     {
				       // see bug 487871, strip /// from dot images and formulas.
                                       int l=0;
                                       while (yytext[l]==' ' || yytext[l]=='\t')
                                       {
                                         l++;
                                       }
                                       copyToOutput(yyscanner,yytext,l);
				       copyToOutput(yyscanner,"   ",3);
				     }
				     else // even slashes are verbatim (e.g. \verbatim, \code)
				     {
				       REJECT;
				     }
  				   }
<Verbatim,VerbatimCode>.	   { /* any other character */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<SkipString>\\.                    { /* escaped character in string */
                                     if (yyextra->lang==SrcLangExt_Fortran || yyextra->lang==SrcLangExt_VHDL)
                                     {
                                       unput(yytext[1]);
                                       copyToOutput(yyscanner,yytext,1);
                                     }
                                     else
                                     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng);
                                     }
                                   }
<SkipString>"\""       	           { /* end of string */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     BEGIN(yyextra->stringContext); 
                                   }
<SkipString>.                      { /* any other string character */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<SkipString>\n                     { /* new line inside string (illegal for some compilers) */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<SkipChar>\\.		           { /* escaped character */
                                     if (yyextra->lang==SrcLangExt_Fortran || yyextra->lang==SrcLangExt_VHDL)
                                     {
                                       unput(yytext[1]);
                                       copyToOutput(yyscanner,yytext,1);
                                     }
                                     else
                                     {
                                       copyToOutput(yyscanner,yytext,(int)yyleng);
                                     }
                                   }
<SkipChar>'                        { /* end of character literal */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                     BEGIN(yyextra->charContext);
                                   }
<SkipChar>.                        { /* any other string character */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<SkipChar>\n                       { /* new line character */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }

<CComment>[^ `~<\\!@*\n{\"\/]*     { /* anything that is not a '*' or command */ 
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<CComment>"*"+[^*\/\\@\n{\"]*      { /* stars without slashes */
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
                                   }
<CComment>"\"\"\""                 { /* end of Python docstring */
                                     if (yyextra->lang!=SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->nestingCount--;
                                       yyextra->pythonDocString = FALSE;
				       copyToOutput(yyscanner,yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
  				   }
<CComment>\n                       { /* new line in comment */
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                     /* in case of Fortran always end of comment */
  				     if (yyextra->lang==SrcLangExt_Fortran)
				     {
				       BEGIN(Scan);
				     }
                                   }
<CComment>"/"+"*"                  { /* nested C comment */
                                     if (yyextra->lang==SrcLangExt_Python ||
                                         yyextra->lang==SrcLangExt_Markdown)
				     {
				       REJECT;
                                     }
                                     yyextra->nestingCount++;
                                     yyextra->commentStack.push(yyextra->lineNr);
                                     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
<CComment>"*"+"/"                  { /* end of C comment */
                                     if (yyextra->lang==SrcLangExt_Python ||
                                         yyextra->lang==SrcLangExt_Markdown)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yyscanner,yytext,(int)yyleng);
                                       yyextra->nestingCount--;
                                       if (yyextra->nestingCount<=0)
                                       {
				         BEGIN(Scan);
                                       }
                                       else
                                       {
                                         //yyextra->nestingCount--;
                                         yyextra->commentStack.pop();
                                       }
				     }
                                   }
  /* Python an VHDL share CComment, so special attention for ending comments is required */
<CComment>"\n"/[ \t]*"#" 	   {
                                     if (yyextra->lang!=SrcLangExt_VHDL)
                                     {
                                       REJECT;
                                     }
                                     else
                                     {
                                       if (yyextra->vhdl) // inside --! comment
                                       {
                                         yyextra->vhdl = FALSE;
				         copyToOutput(yyscanner,yytext,(int)yyleng);
				         BEGIN(Scan);
                                       }
                                       else // C-type comment
                                       {
                                         REJECT;
                                       }
                                     }
                                   }
<CComment>"\n"/[ \t]*"-" 	   {
                                     if (yyextra->lang!=SrcLangExt_Python || yyextra->pythonDocString)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yyscanner,yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
                                   }
<CComment>"\n"/[ \t]*[^ \t#\-] 	   {
                                     if (yyextra->lang==SrcLangExt_Python)
                                     {
                                       if (yyextra->pythonDocString)
                                       {
                                         REJECT;
                                       }
                                       else
                                       {
				         copyToOutput(yyscanner,yytext,(int)yyleng);
				         BEGIN(Scan);
                                       }
                                     }
                                     else if (yyextra->lang==SrcLangExt_VHDL)
                                     {
                                       if (yyextra->vhdl) // inside --! comment
                                       {
                                         yyextra->vhdl = FALSE;
				         copyToOutput(yyscanner,yytext,(int)yyleng);
				         BEGIN(Scan);
                                       }
                                       else // C-type comment
                                       {
                                         REJECT;
                                       }
                                     }
                                     else
                                     {
				       REJECT;
                                     }
                                   }
   /* removed for bug 674842 (bug was introduced in rev 768)
<CComment>"'"			   {
  			             yyextra->charContext = YY_START;
				     copyToOutput(yyscanner,yytext,(int)yyleng);
				     BEGIN(SkipChar);
  				   }
<CComment>"\""			   {
  			             yyextra->stringContext = YY_START;
				     copyToOutput(yyscanner,yytext,(int)yyleng);
				     BEGIN(SkipString);
  				   }
   */
<CComment>.			   {
                                     copyToOutput(yyscanner,yytext,(int)yyleng); 
  				   }
<SComment>^[ \t]*{CPPC}"/"{SLASHopt}/\n     {
  				     replaceComment(yyscanner,0);
  				   }
<SComment>\n[ \t]*{CPPC}"/"{SLASHopt}/\n    {
                                     replaceComment(yyscanner,1); 
                                   }
<SComment>^[ \t]*{CPPC}"/"[^\/\n]/.*\n { 
  				     replaceComment(yyscanner,0);
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
  				   }
<SComment>\n[ \t]*{CPPC}[\/!]("<")?[ \t]*[\\@]"}".*\n {   
                                     /* See Bug 752712: end the multiline comment when finding a @} or \} command */
                                     copyToOutput(yyscanner," */",3); 
				     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->inSpecialComment=FALSE;
				     yyextra->inRoseComment=FALSE;
				     BEGIN(Scan); 
                                   }
<SComment>\n[ \t]*{CPPC}"/"[^\/\n]/.*\n  { 
                                     replaceComment(yyscanner,1); 
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
  				   }
<SComment>^[ \t]*{CPPC}"!"             |    // just //!
<SComment>^[ \t]*{CPPC}"!<"/.*\n       |    // or   //!< something
<SComment>^[ \t]*{CPPC}"!"[^<]/.*\n    {    // or   //!something
  				     replaceComment(yyscanner,0);
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>\n[ \t]*{CPPC}"!"            |
<SComment>\n[ \t]*{CPPC}"!<"/.*\n      |
<SComment>\n[ \t]*{CPPC}"!"[^<\n]/.*\n { 
                                     replaceComment(yyscanner,1); 
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<SComment>^[ \t]*{CPPC}"##"/.*\n       {
                                     if (!yyextra->inRoseComment)
				     {
				       REJECT;
				     }
				     else
				     {
  				       replaceComment(yyscanner,0);
				       yyextra->readLineCtx=YY_START;
				       BEGIN(ReadLine);
				     }
                                   }
<SComment>\n[ \t]*{CPPC}"##"/.*\n      {
                                     if (!yyextra->inRoseComment)
				     {
				       REJECT;
				     }
				     else
				     {
                                       replaceComment(yyscanner,1); 
				       yyextra->readLineCtx=YY_START;
				       BEGIN(ReadLine);
				     }
                                   }
<SComment>\n			   { /* end of special comment */
                                     copyToOutput(yyscanner," */",3); 
				     copyToOutput(yyscanner,yytext,(int)yyleng); 
				     yyextra->inSpecialComment=FALSE;
				     yyextra->inRoseComment=FALSE;
				     BEGIN(Scan); 
                                   }
<ReadLine>{CCS}"*"                    {
				     copyToOutput(yyscanner,"/&zwj;**",8);
				   }
<ReadLine>{CCE}                     {
				     copyToOutput(yyscanner,"*&zwj;/",7);
				   }
<ReadLine>"*"                      {
				     copyToOutput(yyscanner,yytext,(int)yyleng);
				   }
<ReadLine>{RLopt}                  {
				     copyToOutput(yyscanner,yytext,(int)yyleng);
				   }
<ReadLine>{RLopt}/\n               {
				     copyToOutput(yyscanner,yytext,(int)yyleng);
				     BEGIN(yyextra->readLineCtx);
				   }
<CComment,ReadLine>[\\@][\\@][~a-z_A-Z][a-z_A-Z0-9]*[ \t]* { // escaped command
				     copyToOutput(yyscanner,yytext,(int)yyleng);
  				   }
<CComment,ReadLine>[\\@]"cond"/[^a-z_A-Z0-9]	   { // conditional section
  				     yyextra->condCtx = YY_START; 
  				     BEGIN(CondLine);
  				   }
<CComment,ReadLine>[\\@]"endcond"/[^a-z_A-Z0-9] { // end of conditional section
  				     bool oldSkip=yyextra->skip;
  				     endCondSection(yyscanner);
				     if (YY_START==CComment && oldSkip && !yyextra->skip) 
    			             {
				       //printf("** Adding start of comment!\n");
				       if (yyextra->lang!=SrcLangExt_Python &&
					   yyextra->lang!=SrcLangExt_VHDL &&
					   yyextra->lang!=SrcLangExt_Markdown &&
					   yyextra->lang!=SrcLangExt_Fortran)
				       {
 				         ADDCHAR('/');
     				         ADDCHAR('*');
					 if (yyextra->specialComment)
					 {
					   ADDCHAR('*');
					 }
				       }
    				     }
				    }
<CondLine>[!()&| \ta-z_A-Z0-9.\-]+ {
                                     handleCondSectionId(yyscanner,yytext);
  				   }
<CComment,ReadLine>[\\@]"cond"{WSopt}/\n {
  				     yyextra->condCtx=YY_START;
                                     handleCondSectionId(yyscanner," "); // fake section id causing the section to be hidden unconditionally
                                   }
<CondLine>.			   { // forgot section id?
                                     handleCondSectionId(yyscanner," "); // fake section id causing the section to be hidden unconditionally
				     if (*yytext=='\n') yyextra->lineNr++;
  				   }
<CComment,ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*  { // expand alias without arguments
				     replaceAliases(yyscanner,yytext);
  				   }
<CComment,ReadLine>[\\@][a-z_A-Z][a-z_A-Z0-9]*"{" { // expand alias with arguments
                                     yyextra->lastBlockContext=YY_START;
				     yyextra->blockCount=1;
				     yyextra->aliasString=yytext;
				     yyextra->lastEscaped=0;
				     BEGIN( ReadAliasArgs );
  				   }
<ReadAliasArgs>^[ \t]*{CPPC}[/!]/[^\n]+   { // skip leading special comments (see bug 618079)
  				   }
<ReadAliasArgs>{CCE}		   { // oops, end of comment in the middle of an alias?
                                     if (yyextra->lang==SrcLangExt_Python)
				     {
				       REJECT;
				     }
				     else // abort the alias, restart scanning
				     {
				       copyToOutput(yyscanner,yyextra->aliasString,yyextra->aliasString.length());
				       copyToOutput(yyscanner,yytext,(int)yyleng);
				       BEGIN(Scan);
				     }
  				   }
<ReadAliasArgs>[^{}\n\\\*]+	   {
                                     yyextra->aliasString+=yytext;
				     yyextra->lastEscaped=FALSE;
  				   }
<ReadAliasArgs>"\\"		   {
                                     if (yyextra->lastEscaped)  yyextra->lastEscaped=FALSE;
                                     else                yyextra->lastEscaped=TRUE;
                                     yyextra->aliasString+=yytext;
                                   }
<ReadAliasArgs>\n		   {
                                     yyextra->aliasString+=yytext;
                                     yyextra->lineNr++;
				     yyextra->lastEscaped=FALSE;
  				   }
<ReadAliasArgs>"{"		   {
                                     yyextra->aliasString+=yytext;
                                     if (!yyextra->lastEscaped) yyextra->blockCount++;
				     yyextra->lastEscaped=FALSE;
                                   }
<ReadAliasArgs>"}"		   {
                                     yyextra->aliasString+=yytext;
				     if (!yyextra->lastEscaped) yyextra->blockCount--;
				     if (yyextra->blockCount==0)
				     {
				       replaceAliases(yyscanner,yyextra->aliasString);
				       BEGIN( yyextra->lastBlockContext );
				     }
				     yyextra->lastEscaped=FALSE;
  			           }
<ReadAliasArgs>.		   {
                                     yyextra->aliasString+=yytext;
				     yyextra->lastEscaped=FALSE;
  				   }
<ReadLine>.			   {
  				     copyToOutput(yyscanner,yytext,(int)yyleng);
  				   }

<*>.                               {
  				     copyToOutput(yyscanner,yytext,(int)yyleng);
                                   }
%%

static void replaceCommentMarker(yyscan_t yyscanner,const char *s,int len)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=s;
  char c;
  // copy leading blanks
  while ((c=*p) && (c==' ' || c=='\t' || c=='\n')) 
  {
    ADDCHAR(c);
    yyextra->lineNr += c=='\n';
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
  static int tabSize=Config_getInt(TAB_SIZE);
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

static inline void copyToOutput(yyscan_t yyscanner,const char *s,int len)
{
  int tabSize=Config_getInt(TAB_SIZE);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int i;
  if (yyextra->skip) // only add newlines.
  {
    for (i=0;i<len;i++)
    {
      switch(s[i])
      {
        case '\n':
	  ADDCHAR('\n');
	  yyextra->lineNr++;
          yyextra->col=0;
          break;
        case '\t':
          yyextra->col+=tabSize-(yyextra->col%tabSize);
          break;
        default:
          yyextra->col++;
          break;
      }
    }
  }
  else if (len>0)
  {
    ADDARRAY(s,len);
    for (i=0;i<len;i++) 
    {
      switch (s[i])
      {
	case '\n': yyextra->col=0; 
	           //fprintf(stderr,"---> copy %d\n",g_lineNr);
		   yyextra->lineNr++; break;
	case '\t': yyextra->col+=tabSize-(yyextra->col%tabSize); break;
	default:   yyextra->col++; break;
      }
    }
  }
}

static void clearCommentStack(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  while (!yyextra->commentStack.empty()) yyextra->commentStack.pop();
}

static void startCondSection(yyscan_t yyscanner,const char *sectId)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("startCondSection: skip=%d stack=%d\n",g_skip,g_condStack.count());
  CondParser prs;
  bool expResult = prs.parse(yyextra->fileName,yyextra->lineNr,sectId);
  yyextra->condStack.push(CondCtx(yyextra->lineNr,sectId,yyextra->skip));
  if (!expResult) // not enabled
  {
    yyextra->skip=TRUE;
  }
}

static void endCondSection(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->condStack.empty())
  {
    warn(yyextra->fileName,yyextra->lineNr,"Found \\endcond command without matching \\cond");
    yyextra->skip=FALSE;
  }
  else
  {
    const CondCtx &ctx = yyextra->condStack.top();
    yyextra->skip=ctx.skip;
    yyextra->condStack.pop();
  }
  //printf("endCondSection: skip=%d stack=%d\n",g_skip,g_condStack.count());
}

static void handleCondSectionId(yyscan_t yyscanner,const char *expression)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool oldSkip=yyextra->skip;
  startCondSection(yyscanner,expression);
  if ((yyextra->condCtx==CComment || yyextra->readLineCtx==SComment) && 
      !oldSkip && yyextra->skip) 
  {
    if (yyextra->lang!=SrcLangExt_Python &&
        yyextra->lang!=SrcLangExt_VHDL &&
        yyextra->lang!=SrcLangExt_Markdown &&
        yyextra->lang!=SrcLangExt_Fortran)
    {
      ADDCHAR('*');
      ADDCHAR('/');
    }
  }
  if (yyextra->readLineCtx==SComment)
  {
    BEGIN(SComment);
  }
  else
  {
    BEGIN(yyextra->condCtx);
  }
}

/** copies string \a s with length \a len to the output, while 
 *  replacing any alias commands found in the string.
 */
static void replaceAliases(yyscan_t yyscanner,const char *s)
{
  QCString result = resolveAliasCmd(s);
  //printf("replaceAliases(%s)->'%s'\n",s,result.data());
  copyToOutput(yyscanner,result,result.length());
}


static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t bytesInBuf = yyextra->inBuf->curPos()-yyextra->inBufPos;
  yy_size_t bytesToCopy = QMIN(max_size,bytesInBuf);
  memcpy(buf,yyextra->inBuf->data()+yyextra->inBufPos,bytesToCopy);
  yyextra->inBufPos+=bytesToCopy;
  return bytesToCopy;
}

static void replaceComment(yyscan_t yyscanner,int offset)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->mlBrief || yyextra->skip)
  {
    copyToOutput(yyscanner,yytext,(int)yyleng);
  }
  else
  {
    //printf("replaceComment(%s)\n",yytext);
    int i=computeIndent(&yytext[offset]);
    if (i==yyextra->blockHeadCol)
    {
      replaceCommentMarker(yyscanner,yytext,(int)yyleng);
    }
    else
    {
      copyToOutput(yyscanner," */",3);
      for (i=(int)yyleng-1;i>=0;i--) unput(yytext[i]);
      yyextra->inSpecialComment=FALSE;
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
  yyscan_t yyscanner;
  commentcnvYY_state extra;
  commentcnvYYlex_init_extra(&extra,&yyscanner);
#ifdef FLEX_DEBUG
  commentcnvYYset_debug(1,yyscanner);
#endif
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("convertCppComments(%s)\n",fileName);
  yyextra->inBuf    = inBuf;
  yyextra->outBuf   = outBuf;
  yyextra->inBufPos = 0;
  yyextra->col      = 0;
  yyextra->mlBrief = Config_getBool(MULTILINE_CPP_IS_BRIEF);
  yyextra->skip     = FALSE;
  yyextra->fileName = fileName;
  yyextra->lang = getLanguageFromFileName(fileName);
  yyextra->pythonDocString = FALSE;
  yyextra->lineNr   = 1;
  while (!yyextra->condStack.empty()) yyextra->condStack.pop();
  clearCommentStack(yyscanner);
  yyextra->vhdl = FALSE;

  printlex(yy_flex_debug, TRUE, __FILE__, fileName);
  yyextra->isFixedForm = FALSE;
  if (yyextra->lang==SrcLangExt_Fortran)
  {
    FortranFormat fmt = convertFileNameFortranParserCode(fileName);
    yyextra->isFixedForm = recognizeFixedForm(inBuf->data(),fmt);
  }

  if (yyextra->lang==SrcLangExt_Markdown)
  {
    yyextra->nestingCount=0;
    BEGIN(CComment);
    yyextra->commentStack.push(yyextra->lineNr);
  }
  else
  {
    BEGIN(Scan);
  }
  yylex(yyscanner);
  while (!yyextra->condStack.empty())
  {
    const CondCtx &ctx = yyextra->condStack.top();
    QCString sectionInfo = " ";
    if (ctx.sectionId!=" ") sectionInfo.sprintf(" with label '%s' ",ctx.sectionId.stripWhiteSpace().data());
    warn(yyextra->fileName,ctx.lineNr,"Conditional section%sdoes not have "
	"a corresponding \\endcond command within this file.",sectionInfo.data());
    yyextra->condStack.pop();
  }
  if (yyextra->nestingCount>0 && yyextra->lang!=SrcLangExt_Markdown && yyextra->lang!=SrcLangExt_Fortran)
  {
    QCString tmp= "(probable line reference: ";
    bool first = TRUE;
    while (!yyextra->commentStack.empty())
    {
      int lineNr = yyextra->commentStack.top();
      if (!first) tmp += ", ";
      tmp += QCString().setNum(lineNr);
      first = FALSE;
      yyextra->commentStack.pop();
    }
    tmp += ")";
    warn(yyextra->fileName,yyextra->lineNr,"Reached end of file while still inside a (nested) comment. "
        "Nesting level %d %s",yyextra->nestingCount,tmp.data());
  }
  yyextra->nestingCount = 0;
  if (Debug::isFlagSet(Debug::CommentCnv))
  {
    yyextra->outBuf->at(yyextra->outBuf->curPos())='\0';
    Debug::print(Debug::CommentCnv,0,"-----------\nCommentCnv: %s\n"
                 "output=[\n%s]\n-----------\n",fileName,yyextra->outBuf->data()
                );
  }
  printlex(yy_flex_debug, FALSE, __FILE__, fileName);
  commentcnvYYlex_destroy(yyscanner);
}


//----------------------------------------------------------------------------

#if USE_STATE2STRING
#include "commentcnv.l.h"
#endif
