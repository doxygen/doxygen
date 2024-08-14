/*****************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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
  // redefine buffer size (defaults to 16Kb) to avoid a 'flex scanner push-back overflow' error
  #undef YY_BUF_SIZE
  #define YY_BUF_SIZE 10*1024*1024  // 10Mb should be sufficient for a source file
%}

%option never-interactive
%option prefix="commentcnvYY"
%option reentrant
%option extra-type="struct commentcnvYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{


#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <algorithm>
#include <deque>
#include <string_view>

#include "debug.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "aliases.h"
#include "condparser.h"
#include "fileinfo.h"
#include "stringutil.h"
#include "regex.h"
#include "section.h"

#include <assert.h>

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct commentcnvYY_CondCtx
{
  commentcnvYY_CondCtx(int line,const QCString &id,bool b)
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

struct commentcnv_FileState
{
  int oldLineNr = 1;
  YY_BUFFER_STATE bufState = 0;
  QCString fileName;
  QCString oldFileName;
  QCString oldIncPrefix;
  QCString blockId;
  int oldState = 0;
  std::string fileBuf;
  const std::string *oldFileBuf = nullptr;
  int oldFileBufPos = 0;
  int oldIncludeCtx = 0;
  int oldRaiseLvl = 0;
  QCString oldRaiseLbl;
};

struct commentcnvYY_state
{
  commentcnvYY_state(const std::string *i,std::string &o) : inBuf(i), outBuf(o) {}
  const std::string *inBuf;
  std::string &outBuf;
  int      inBufPos = 0;
  int      col = 0;
  int      blockHeadCol = 0;        // column at which the start of a special comment block was found
  int      insertCommentCol = 0;    // column at which an include or snippet command was found
  bool     mlBrief = FALSE;
  int      readLineCtx = 0;
  int      includeCtx = 0;
  int      raiseLevel = 0;
  QCString raiseLabel;
  int      raiseIncrement = 0;
  QCString raisePrefix;
  bool     skip = FALSE;
  QCString fileName;
  int      lineNr = 0;
  int      condCtx = 0;
  std::stack<commentcnvYY_CondCtx> condStack;
  std::stack<int> commentStack;
  QCString blockName;
  int      lastCommentContext = 0;
  bool     inSpecialComment = FALSE;
  bool     inRoseComment= FALSE;
  int      stringContext = 0;
  int      charContext = 0;
  int      javaBlock = 0;
  bool     specialComment = FALSE;
  bool     inVerbatim = false;
  bool     inHtmlDoxygenCommand = false;
  bool     firstIncludeLine = false;
  bool     insertCppCommentMarker = false;
  QCString aliasCmd;
  QCString aliasString;
  int      blockCount = 0;
  bool     lastEscaped = FALSE;
  int      lastBlockContext= 0;
  bool     pythonDocString = FALSE;
  char     pythonDocStringChar = '\0';
  int      nestingCount= 0;
  bool     vhdl = FALSE; // for VHDL old style --! comment
  SrcLangExt lang = SrcLangExt::Unknown;
  bool       isFixedForm = FALSE; // For Fortran
  std::deque<std::unique_ptr<commentcnv_FileState>> includeStack;
  std::vector<std::string> expandedAliases;
  QCString snippetFileName;
  QCString snippetName;
};

[[maybe_unused]] static const char *stateToString(int state);
static inline int computeIndent(const char *s);

static void replaceCommentMarker(yyscan_t yyscanner,std::string_view s);
static inline void copyToOutput(yyscan_t yyscanner,std::string_view s);
static inline void copyToOutput(yyscan_t yyscanner,const char *s,int len);
static void startCondSection(yyscan_t yyscanner,const QCString &sectId);
static void endCondSection(yyscan_t yyscanner);
static void handleCondSectionId(yyscan_t yyscanner,const char *expression);
static void replaceAliases(yyscan_t yyscanner,std::string_view s,bool replaceComment=false);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void replaceComment(yyscan_t yyscanner,int offset);
static void clearCommentStack(yyscan_t yyscanner);
static bool readIncludeFile(yyscan_t yyscanner,const QCString &inc,const QCString &blockId);
static void insertCommentStart(yyscan_t yyscanner);
static bool parseIncludeOptions(yyscan_t yyscanner,std::string_view s);

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

MAILADDR   ("mailto:")?[a-z_A-Z0-9\x80-\xff.+-]+"@"[a-z_A-Z0-9\x80-\xff-]+("."[a-z_A-Z0-9\x80-\xff\-]+)+[a-z_A-Z0-9\x80-\xff\-]+

%option noyywrap

%x Scan
%x SkipString
%x SkipVerbString
%x SkipChar
%x SComment
%x CComment
%x CNComment
%x Verbatim
%x VerbatimCode
%x ReadLine
%x CopyLine
%x CondLine
%x ReadAliasArgs
%x IncludeDoc
%x SnippetDoc
%x SnippetDocTag
%x IncludeFile

CMD ("\\"|"@")
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

FILEICHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+=&#@]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+=&#@]
FILECHARS {FILEICHAR}*{FILEECHAR}+
HFILEMASK {FILEICHAR}*("."{FILEICHAR}+)+{FILECHARS}*
VFILEMASK {FILECHARS}("."{FILECHARS})*
FILEMASK  {VFILEMASK}|{HFILEMASK}

B         [ \t]
ID        [$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*

OPTS      "{"[^}]*"}"{B}*

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
RLopt [^\\@<\n\*\/`]*
RL    [^\\@<\n\*\/`]+
  // Optional slash
SLASHopt [/]*

%%

<Scan>{NUMBER}			    { //Note similar code in code.l
                                      if (yyextra->lang!=SrcLangExt::Cpp) REJECT;
                                      copyToOutput(yyscanner,yytext,yyleng);
                                    }
<Scan>[^"'!\/\n\\#,\-=; \t@$]*      { /* eat anything that is not " / , or \n */
                                       copyToOutput(yyscanner,yytext,yyleng);
                                    }
<Scan>[,= ;\t]                      { /* eat , so we have a nice separator in long initialization lines */
                                       copyToOutput(yyscanner,yytext,yyleng);
                                    }
<Scan>"'''"!                        |
<Scan>"\"\"\""!                     { /* start of python long comment */
                                     if (yyextra->lang!=SrcLangExt::Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->pythonDocString = TRUE;
                                       yyextra->pythonDocStringChar = yytext[0];
                                       yyextra->nestingCount=1;
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       copyToOutput(yyscanner,yytext,yyleng);
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
                                   }
<Scan>"'''"                        |
<Scan>"\"\"\""                     { /* start of python long comment */
                                     if (yyextra->lang!=SrcLangExt::Python)
				     {
				       REJECT;
				     }
                                     else if (Config_getBool(PYTHON_DOCSTRING))
                                     {
				       REJECT;
                                     }
				     else
				     { /* handle as if """! */
                                       yyextra->pythonDocString = TRUE;
                                       yyextra->pythonDocStringChar = yytext[0];
                                       yyextra->nestingCount=1;
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       copyToOutput(yyscanner,yytext,yyleng);
				       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
                                   }
<Scan>{B}*![><!]/.*\n              {
                                     if (yyextra->lang!=SrcLangExt::Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->nestingCount=0; // Fortran doesn't have an end comment
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       yyextra->specialComment=true;
                                       copyToOutput(yyscanner,yytext,yyleng);
				       yyextra->blockHeadCol=yyextra->col-2;
                                       BEGIN(CComment);
                                       yyextra->commentStack.push(yyextra->lineNr);
				     }
  				   }
<Scan>[Cc\*][><!]/.*\n	   {
                                     if (yyextra->lang!=SrcLangExt::Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       /* check for fixed format; we might have some conditional as part of multiline if like C<5 .and. & */
                                       if (yyextra->isFixedForm && (yyextra->col == 0))
                                       {
                                         yyextra->nestingCount=0; // Fortran doesn't have an end comment
                                         clearCommentStack(yyscanner); /*  to be on the save side */
                                         yyextra->specialComment=true;
                                         copyToOutput(yyscanner,yytext,yyleng);
				         yyextra->blockHeadCol=yyextra->col-1;
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
  			             if (yyextra->lang!=SrcLangExt::Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,yyleng);
				     }
                                   }
<Scan>[Cc\*].*\n		   {
  			             if (yyextra->lang!=SrcLangExt::Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       if (yyextra->col == 0)
                                       {
                                         copyToOutput(yyscanner,yytext,yyleng);
				       }
				       else
				       {
				         REJECT;
				       }
				     }
                                   }
<Scan>[$]?"@\""                    { /* start of an interpolated verbatim C# string */
                                     if (yyextra->lang!=SrcLangExt::CSharp) REJECT
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->stringContext = YY_START;
				     BEGIN(SkipVerbString);
                                   }
<Scan>"\""                         { /* start of a string */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->stringContext = YY_START;
				     BEGIN(SkipString);
                                   }
<Scan>'				   {
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->charContext = YY_START;
                                     if (yyextra->lang!=SrcLangExt::VHDL)
                                     {
				       BEGIN(SkipChar);
                                     }
  				   }
<Scan>\n                           { /* new line */
                                     copyToOutput(yyscanner,yytext,yyleng);
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
				       yyextra->blockHeadCol=yyextra->col+1;
				       copyToOutput(yyscanner,"/**");
				       if (i<yyleng) replaceAliases(yyscanner,yytext+i);
				       yyextra->inSpecialComment=TRUE;
				       //BEGIN(SComment);
				       yyextra->readLineCtx=SComment;
				       BEGIN(ReadLine);
				     }
                                   }
<Scan>{CPPC}"##Documentation"{ANYopt}/\n	   { /* Start of Rational Rose ANSI C++ comment block */
                                     if (yyextra->mlBrief) REJECT;
                                     int i=17; //=strlen("//##Documentation");
				     yyextra->blockHeadCol=yyextra->col+1;
				     copyToOutput(yyscanner,"/**");
				     if (i<yyleng) replaceAliases(yyscanner,yytext+i);
				     yyextra->inRoseComment=TRUE;
				     BEGIN(SComment);
  				   }
<Scan>{CPPC}[!\/]/.*\n[ \t]*{CPPC}[|\/][ \t]*[@\\]"}" { // next line contains an end marker, see bug 752712
				     yyextra->inSpecialComment=yytext[2]=='/' || yytext[2]=='!';
                                     if (yyextra->inSpecialComment)
                                     {
				       yyextra->blockHeadCol=yyextra->col+1;
                                     }
  				     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<Scan>{CPPC}[!/]/.*\n              { /* one line special C++ comment */
				     yyextra->inSpecialComment=true;
				     yyextra->blockHeadCol=yyextra->col+1;
                                     yyextra->insertCppCommentMarker=true;
  				     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->readLineCtx=YY_START;
				     BEGIN(ReadLine);
                                   }
<Scan>{CPPC}/.*\n                  { /* one line normal C++ comment */
				     yyextra->inSpecialComment=false;
  				     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->readLineCtx=YY_START;
				     BEGIN(CopyLine);
				   }
<Scan>{CCS}{CCE}                   { /* avoid matching next rule for empty C comment, see bug 711723 */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<Scan>{CCS}[*!]?                   { /* start of a C comment */
                                     if (yyextra->lang==SrcLangExt::Python)
				     {
				       REJECT;
                                     }
  			             yyextra->specialComment=(int)yyleng==3;
                                     yyextra->nestingCount=1;
                                     clearCommentStack(yyscanner); /*  to be on the save side */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                     if (yyextra->specialComment)
                                     {
				       yyextra->blockHeadCol=0;
				       BEGIN(CComment);
                                     }
                                     else
                                     {
				       BEGIN(CNComment);
                                     }
                                     yyextra->commentStack.push(yyextra->lineNr);
                                   }
<Scan>"#"[^\n]*\n                  {
                                     if (yyextra->lang!=SrcLangExt::PHP)
				     {
				       REJECT;
				     }
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<Scan>"#"("#")?		           {
                                     if (yyextra->lang!=SrcLangExt::Python)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->nestingCount=0; // Python doesn't have an end comment for #
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       yyextra->specialComment=(int)yyleng==2;
                                       if (yyextra->specialComment)
                                       {
				         yyextra->blockHeadCol=yyextra->col;
                                       }
                                       yyextra->commentStack.push(yyextra->lineNr);
                                       copyToOutput(yyscanner,yytext,yyleng);
                                       BEGIN(CComment);
				     }
  				   }
<Scan>"--"[^!][^\n]*		   {
                                     if (yyextra->lang!=SrcLangExt::VHDL)
				     {
				       REJECT;
				     }
				     else
				     {
                                       copyToOutput(yyscanner,yytext,yyleng);
				     }
  				   }
<Scan>"--!"		           {
                                     if (yyextra->lang!=SrcLangExt::VHDL)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->specialComment=true;
				       yyextra->blockHeadCol=yyextra->col;
                                       yyextra->vhdl = TRUE;
                                       yyextra->nestingCount=0;  // VHDL doesn't have an end comment
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       yyextra->commentStack.push(yyextra->lineNr);
                                       copyToOutput(yyscanner,yytext,yyleng);
                                       BEGIN(CComment);
				     }
  				   }
<Scan>{B}*![><!]	           {
                                     if (yyextra->lang!=SrcLangExt::Fortran)
				     {
				       REJECT;
				     }
				     else
				     {
                                       yyextra->nestingCount=0;  // Fortran doesn't have an end comment
                                       clearCommentStack(yyscanner); /*  to be on the save side */
                                       yyextra->specialComment=true;
				       yyextra->blockHeadCol=yyextra->col;
                                       yyextra->commentStack.push(yyextra->lineNr);
                                       copyToOutput(yyscanner,yytext,yyleng);
                                       BEGIN(CComment);
				     }
  				   }
<CComment,CNComment,ReadLine,IncludeFile>{MAILADDR}       |
<CComment,CNComment,ReadLine,IncludeFile>"<"{MAILADDR}">" { // Mail address, to prevent seeing e.g x@code-factory.org as start of a code block
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<CComment,IncludeFile>"{"[ \t]*"@code"/[ \t\n] {
                                     copyToOutput(yyscanner,"@iliteral{code}");
				     yyextra->lastCommentContext = YY_START;
				     yyextra->javaBlock=1;
				     yyextra->blockName=QCString("end")+&yytext[1];
                                     yyextra->inVerbatim=true;
                                     BEGIN(VerbatimCode);
  				   }
<CComment,IncludeFile>"{"[ \t]*"@literal"/[ \t\n] {
                                     copyToOutput(yyscanner,"@iliteral");
				     yyextra->lastCommentContext = YY_START;
				     yyextra->javaBlock=1;
				     yyextra->blockName=QCString("end")+&yytext[1];
                                     yyextra->inVerbatim=true;
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine,IncludeFile>"\\ilinebr"[ \t]+("```"[`]*|"~~~"[~]*) { /* start of markdown code block */
                                     if (!Config_getBool(MARKDOWN_SUPPORT))
                                     {
                                       REJECT;
                                     }
                                     copyToOutput(yyscanner,yytext,yyleng);
                                     yyextra->lastCommentContext = YY_START;
                                     yyextra->javaBlock=0;
                                     yyextra->blockName=QCString(yytext).stripWhiteSpace().right(3); // take the ``` or ~~~ part
                                     yyextra->inVerbatim=true;
                                     BEGIN(VerbatimCode);
                                     }
<CComment,ReadLine,IncludeFile>^[ \t]*("```"[`]*|"~~~"[~]*) { /* start of markdown code block */
                                     if (!Config_getBool(MARKDOWN_SUPPORT))
                                     {
                                       REJECT;
                                     }
                                     copyToOutput(yyscanner,yytext,yyleng);
                                     yyextra->lastCommentContext = YY_START;
                                     yyextra->javaBlock=0;
                                     yyextra->blockName=QCString(yytext).stripWhiteSpace().left(3); // take the ``` or ~~~ part
                                     yyextra->inVerbatim=true;
                                     BEGIN(VerbatimCode);
                                   }
<CComment,ReadLine,IncludeFile>[\\@]("dot"|"code"|"msc"|"startuml")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->lastCommentContext = YY_START;
				     yyextra->javaBlock=0;
                                     if (qstrcmp(&yytext[1],"startuml")==0)
                                     {
                                       yyextra->blockName="enduml";
                                     }
                                     else
                                     {
				       yyextra->blockName=QCString("end")+&yytext[1];
                                     }
                                     yyextra->inVerbatim=true;
                                     BEGIN(VerbatimCode);
  				   }
<CComment,ReadLine,IncludeFile>[\\@]("f$"|"f["|"f{"|"f(") {
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->blockName=&yytext[1];
				     if (yyextra->blockName.at(1)=='[')
				     {
				       yyextra->blockName.at(1)=']';
				     }
				     else if (yyextra->blockName.at(1)=='{')
				     {
				       yyextra->blockName.at(1)='}';
				     }
				     else if (yyextra->blockName.at(1)=='(')
				     {
				       yyextra->blockName.at(1)=')';
				     }
				     yyextra->lastCommentContext = YY_START;
                                     yyextra->inVerbatim=true;
				     BEGIN(Verbatim);
  			           }
<CComment,ReadLine,IncludeFile>"<!--!" { /* HTML comment doxygen command*/
                                     if (yyextra->inVerbatim) REJECT;
                                     //copyToOutput(yyscanner,"     ",5);
                                     yyextra->inHtmlDoxygenCommand=true;
                                   }
<CComment,ReadLine,IncludeFile>"-->" { /* potential end HTML comment doxygen command*/
                                     if (yyextra->inHtmlDoxygenCommand)
                                     {
                                       yyextra->inHtmlDoxygenCommand=false;
                                     }
                                     else
                                     {
                                       copyToOutput(yyscanner,yytext,yyleng);
                                     }
                                   }
<CComment,ReadLine,IncludeFile>"<!--" { /* HTML comment */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->blockName="-->";
				     yyextra->lastCommentContext = YY_START;
                                     yyextra->inVerbatim=true;
                                     BEGIN(Verbatim);
                                   }
<CComment,ReadLine,IncludeFile>[\\@]("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly")/[^a-z_A-Z0-9] { /* start of a verbatim block */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->blockName=QCString("end")+&yytext[1];
				     yyextra->lastCommentContext = YY_START;
                                     yyextra->inVerbatim=true;
                                     BEGIN(Verbatim);
                                   }
<Scan>"\\\""                       { /* escaped double quote */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<Scan>"\\\\"                       { /* escaped backslash */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<Scan>.                            { /* any other character */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<Verbatim>[\\@]("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"f$"|"f]"|"f}"|"f)") { /* end of verbatim block */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     if (&yytext[1]==yyextra->blockName) // end of command or formula
				     {
                                       yyextra->inVerbatim=false;
				       BEGIN(yyextra->lastCommentContext);
				     }
                                   }
<Verbatim>"-->"                    {
                                     copyToOutput(yyscanner,yytext,yyleng);
				     if (yytext==yyextra->blockName) 
				     {
                                       yyextra->inVerbatim=false;
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
                                       copyToOutput(yyscanner,yytext,yyleng);
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
                                         copyToOutput(yyscanner," @endiliteral ");
                                         yyextra->inVerbatim=false;
				         BEGIN(yyextra->lastCommentContext);
				       }
				       else
				       {
                                         copyToOutput(yyscanner,yytext,yyleng);
				       }
				     }
  				   }
<VerbatimCode>("```"[`]*|"~~~"[~]*) { /* end of markdown code block */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                     if (yytext[0]==yyextra->blockName[0])
                                     {
                                       yyextra->inVerbatim=false;
                                       BEGIN(yyextra->lastCommentContext);
                                     }
                                   }
<VerbatimCode>[\\@]("enddot"|"endcode"|"endmsc"|"enduml") { /* end of verbatim block */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     if (&yytext[1]==yyextra->blockName)
				     {
                                       yyextra->inVerbatim=false;
				       BEGIN(yyextra->lastCommentContext);
				     }
                                   }
<VerbatimCode>^[ \t]*{CPPC}[\!\/]?   { /* skip leading comments */
  				     if (!yyextra->inSpecialComment || yyextra->mlBrief)
				     {
                                       copyToOutput(yyscanner,yytext,yyleng);
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
                                         copyToOutput(yyscanner," * ");
                                       }
                                       else // ends with //
                                       {
                                         copyToOutput(yyscanner,"//");
                                       }
                                     }
  				   }
<Verbatim,VerbatimCode>[^`~@\/\-\\\n{}]* { /* any character not a backslash or new line or } */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<Verbatim,VerbatimCode>\n	   { /* new line in verbatim block */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     if (yyextra->lastCommentContext == IncludeFile)
                                     {
                                       insertCommentStart(yyscanner);
                                     }
                                   }
<Verbatim>^[ \t]*{CPPC}[/!]          {
  				     if (yyextra->blockName=="enddot" || yyextra->blockName=="endmsc" || yyextra->blockName=="enduml" || yyextra->blockName.at(0)=='f')
				     {
				       // see bug 487871, strip /// from dot images and formulas.
                                       int l=0;
                                       while (yytext[l]==' ' || yytext[l]=='\t')
                                       {
                                         l++;
                                       }
                                       copyToOutput(yyscanner,yytext,l);
				       copyToOutput(yyscanner,"   ");
				     }
				     else // even slashes are verbatim (e.g. \verbatim, \code)
				     {
				       REJECT;
				     }
  				   }
<Verbatim,VerbatimCode>.	   { /* any other character */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipString>\\.                    { /* escaped character in string */
                                     if (yyextra->lang==SrcLangExt::Fortran || yyextra->lang==SrcLangExt::VHDL)
                                     {
                                       unput(yytext[1]);
                                       copyToOutput(yyscanner,yytext,1);
                                     }
                                     else
                                     {
                                       copyToOutput(yyscanner,yytext,yyleng);
                                     }
                                   }
<SkipString>"\""       	           { /* end of string */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     BEGIN(yyextra->stringContext);
                                   }
<SkipString>.                      { /* any other string character */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipString>\n                     { /* new line inside string (illegal for some compilers) */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipVerbString>[^"\n]+            {
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipVerbString>\"\"               { // escaped quote
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipVerbString>"\""       	   { /* end of string */
                                     copyToOutput(yyscanner,yytext,yyleng);
				     BEGIN(yyextra->stringContext);
                                   }
<SkipVerbString>.                  {
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipVerbString>\n                 {
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipChar>\\.		           { /* escaped character */
                                     if (yyextra->lang==SrcLangExt::Fortran || yyextra->lang==SrcLangExt::VHDL)
                                     {
                                       unput(yytext[1]);
                                       copyToOutput(yyscanner,yytext,1);
                                     }
                                     else
                                     {
                                       copyToOutput(yyscanner,yytext,yyleng);
                                     }
                                   }
<SkipChar>'                        { /* end of character literal */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                     BEGIN(yyextra->charContext);
                                   }
<SkipChar>.                        { /* any other string character */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<SkipChar>\n                       { /* new line character */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }

<CComment,CNComment>[^ `~<\\!@*\n{\"'\/-]*    { /* anything that is not a '*' or command */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<CComment,CNComment>"*"+[^*\/<\\@\n{\"]*      { /* stars without slashes */
                                     if (yyextra->col>yyextra->blockHeadCol)
                                     {
                                       //printf("new blockHeadCol=%d\n",yyextra->blockHeadCol);
                                       yyextra->blockHeadCol=yyextra->col;
                                     }
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<CComment>"'''"                    |
<CComment>"\"\"\""                 { /* end of Python docstring */
                                     if (yyextra->lang!=SrcLangExt::Python)
				     {
				       REJECT;
				     }
                                     else if (yyextra->pythonDocStringChar != yytext[0])
                                     {
				       copyToOutput(yyscanner,yytext,yyleng);
                                     }
				     else
				     {
                                       yyextra->nestingCount--;
                                       yyextra->pythonDocString = FALSE;
                                       yyextra->pythonDocStringChar = '\0';
				       copyToOutput(yyscanner,yytext,yyleng);
				       BEGIN(Scan);
				     }
  				   }
<CComment,CNComment>\n             { /* new line in comment */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                     /* in case of Fortran always end of comment */
  				     if (yyextra->lang==SrcLangExt::Fortran)
				     {
				       BEGIN(Scan);
				     }
                                   }
<CComment,CNComment>"/""/"+/"*/"   { /* we are already in C-comment so not a start of a nested comment but
                                      * just the end of the comment (the end part is handled later). */
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<CComment,CNComment>"/"+"*"                  { /* nested C comment */
                                     if (yyextra->lang==SrcLangExt::Python ||
                                         yyextra->lang==SrcLangExt::Markdown)
				     {
				       REJECT;
                                     }
                                     yyextra->nestingCount++;
                                     yyextra->commentStack.push(yyextra->lineNr);
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<CComment,CNComment>"*"+"/"                  { /* end of C comment */
                                     if (yyextra->lang==SrcLangExt::Python ||
                                         yyextra->lang==SrcLangExt::Markdown)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yyscanner,yytext,yyleng);
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
  /* Python an VHDL share CComment,CNComment, so special attention for ending comments is required */
<CComment,CNComment>"\n"/[ \t]*"#" 	   {
                                     if (yyextra->lang!=SrcLangExt::VHDL)
                                     {
                                       REJECT;
                                     }
                                     else
                                     {
                                       if (yyextra->vhdl) // inside --! comment
                                       {
                                         yyextra->vhdl = FALSE;
				         copyToOutput(yyscanner,yytext,yyleng);
				         BEGIN(Scan);
                                       }
                                       else // C-type comment
                                       {
                                         REJECT;
                                       }
                                     }
                                   }
<CComment,CNComment>"\n"/[ \t]*"-" 	   {
                                     if (yyextra->lang!=SrcLangExt::Python || yyextra->pythonDocString)
				     {
				       REJECT;
				     }
				     else
				     {
				       copyToOutput(yyscanner,yytext,yyleng);
				       BEGIN(Scan);
				     }
                                   }
<CComment,CNComment>"\n"/[ \t]*[^ \t#\-] 	   {
                                     if (yyextra->lang==SrcLangExt::Python)
                                     {
                                       if (yyextra->pythonDocString)
                                       {
                                         REJECT;
                                       }
                                       else
                                       {
				         copyToOutput(yyscanner,yytext,yyleng);
				         BEGIN(Scan);
                                       }
                                     }
                                     else if (yyextra->lang==SrcLangExt::VHDL)
                                     {
                                       if (yyextra->vhdl) // inside --! comment
                                       {
                                         yyextra->vhdl = FALSE;
				         copyToOutput(yyscanner,yytext,yyleng);
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
<CComment,CNComment>"'"			   {
  			             yyextra->charContext = YY_START;
				     copyToOutput(yyscanner,yytext,yyleng);
				     BEGIN(SkipChar);
  				   }
<CComment,CNComment>"\""			   {
  			             yyextra->stringContext = YY_START;
				     copyToOutput(yyscanner,yytext,yyleng);
				     BEGIN(SkipString);
  				   }
   */
<CComment,CNComment>{CMD}{CMD}     |
<CComment,CNComment>.	           {
                                     copyToOutput(yyscanner,yytext,yyleng);
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
                                     copyToOutput(yyscanner," */");
				     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->inSpecialComment=false;
				     yyextra->inRoseComment=false;
				     BEGIN(Scan);
                                   }
<SComment>\n[ \t]*{CPPC}"/"[^\\@\/\n]/.*\n  {
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
                                     copyToOutput(yyscanner," */");
				     copyToOutput(yyscanner,yytext,yyleng);
				     yyextra->inSpecialComment=FALSE;
				     yyextra->inRoseComment=FALSE;
                                     yyextra->insertCppCommentMarker=false;
                                     yyextra->readLineCtx = Scan; // reset, otherwise there will be problems with:
                                                                  //   static void handleCondSectionId
				     BEGIN(Scan);
                                   }
<ReadLine>{CCS}"*"                 {
				     copyToOutput(yyscanner,"/&zwj;**");
				   }
<ReadLine>{CCE}                    {
				     copyToOutput(yyscanner,"*&zwj;/");
				   }
<ReadLine,CopyLine>"*"             {
				     copyToOutput(yyscanner,yytext,yyleng);
				   }
<ReadLine,CopyLine>{RL}            {
				     copyToOutput(yyscanner,yytext,yyleng);
				   }
<ReadLine,CopyLine>{RL}/{B}"\\ilinebr"{B} {
                                     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<ReadLine,CopyLine>{RLopt}/\n      {
				     copyToOutput(yyscanner,yytext,yyleng);
                                     yyextra->insertCppCommentMarker=false;
				     BEGIN(yyextra->readLineCtx);
				   }
<CComment,CNComment,ReadLine>"\\<" { /* escaped html comment */
				     copyToOutput(yyscanner,yytext,yyleng);
  				   }
<CComment,CNComment,ReadLine>[\\@][\\@][~a-z_A-Z][a-z_A-Z0-9]*[ \t]* { // escaped command
				     copyToOutput(yyscanner,yytext,yyleng);
  				   }

<CComment,ReadLine,IncludeFile>[\\@]("include"{OPTS}|"includedoc"{OPTS}*) {
                                     if (!parseIncludeOptions(yyscanner,std::string_view{yytext,static_cast<size_t>(yyleng)})) REJECT;
                                     yyextra->includeCtx = YY_START;
                                     yyextra->firstIncludeLine = true;
                                     yyextra->insertCommentCol = yyextra->col;
                                     if (!yyextra->insertCppCommentMarker && (yyextra->includeCtx==ReadLine || yyextra->includeCtx==IncludeFile))
                                     {
                                       yyextra->insertCppCommentMarker = yyextra->mlBrief;
                                     }
                                     //printf("blockHeadCol=%d insertCommentCol=%d\n",yyextra->blockHeadCol, yyextra->insertCommentCol);
                                     BEGIN(IncludeDoc);
                                   }
<CComment,ReadLine,IncludeFile>[\\@]("snippet"{OPTS}|"snippetdoc"{OPTS}*) {
                                     if (!parseIncludeOptions(yyscanner,std::string_view{yytext,static_cast<size_t>(yyleng)})) REJECT;
                                     yyextra->includeCtx = YY_START;
                                     yyextra->firstIncludeLine = true;
                                     yyextra->insertCommentCol = yyextra->col;
                                     if (!yyextra->insertCppCommentMarker && (yyextra->includeCtx==ReadLine || yyextra->includeCtx==IncludeFile))
                                     {
                                       yyextra->insertCppCommentMarker = yyextra->mlBrief;
                                     }
                                     //printf("blockHeadCol=%d insertCommentCol=%d\n",yyextra->blockHeadCol, yyextra->insertCommentCol);
                                     BEGIN(SnippetDoc);
                                   }
<IncludeDoc,SnippetDoc>{B}*
<IncludeDoc>{FILEMASK}|"\""[^\n\"]+"\"" {
                                     QCString fileName=yytext;
                                     if (yytext[0]=='"')
                                     {
                                       fileName=fileName.mid(1,fileName.length()-2); // strip quotes
                                     }
                                     if (readIncludeFile(yyscanner,fileName,""))
                                     {
                                       BEGIN(IncludeFile);
                                     }
                                     else
                                     {
                                       BEGIN(yyextra->includeCtx);
                                     }
                                   }
<SnippetDoc>({FILEMASK}|"\""[^\n\"]+"\""){B}+ {
                                     yyextra->snippetFileName=yytext;
                                     yyextra->snippetFileName=yyextra->snippetFileName.stripWhiteSpace();
                                     if (yyextra->snippetFileName == "this") yyextra->snippetFileName=yyextra->fileName;
                                     yyextra->snippetName = "";
                                     BEGIN(SnippetDocTag);
                                   }
<SnippetDocTag>[^\\\n]+            {
                                     yyextra->snippetName += yytext;
                                   }
<SnippetDocTag>"\\"                {
                                     yyextra->snippetName += yytext;
                                   }
<SnippetDocTag>(\n|"\\ilinebr")    {
                                     for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]);
                                     yyextra->snippetName = yyextra->snippetName.stripWhiteSpace();
                                     QCString blockId = "["+yyextra->snippetName+"]";
                                     if (readIncludeFile(yyscanner,yyextra->snippetFileName,blockId))
                                     {
                                       BEGIN(IncludeFile);
                                     }
                                     else
                                     {
                                       BEGIN(yyextra->includeCtx);
                                     }
                                   }

<IncludeDoc,SnippetDoc>\n          {
				     copyToOutput(yyscanner,yytext,yyleng);
                                     insertCommentStart(yyscanner);
                                     // missing file name
                                     //warn(yyextra->fileName,yyextra->lineNr,"Found \\include{doc} command without valid file name argument");
                                     BEGIN(yyextra->includeCtx);
                                   }
<IncludeDoc,SnippetDoc>.           { // invalid character
				     copyToOutput(yyscanner,yytext,yyleng);
                                     BEGIN(yyextra->includeCtx);
                                   }
<CComment,ReadLine,IncludeFile>[\\@]"cond"/[^a-z_A-Z0-9]	   { // conditional section
  				     yyextra->condCtx = YY_START;
  				     BEGIN(CondLine);
  				   }
<CComment,ReadLine,IncludeFile>[\\@]"endcond"/[^a-z_A-Z0-9] { // end of conditional section
  				     bool oldSkip=yyextra->skip;
  				     endCondSection(yyscanner);
				     if (YY_START==CComment && oldSkip && !yyextra->skip)
    			             {
				       //printf("** Adding start of comment!\n");
				       if (yyextra->lang!=SrcLangExt::Python &&
					   yyextra->lang!=SrcLangExt::VHDL &&
					   yyextra->lang!=SrcLangExt::Markdown &&
					   yyextra->lang!=SrcLangExt::Fortran)
				       {
 				         yyextra->outBuf+='/';
     				         yyextra->outBuf+='*';
                                         yyextra->col+=2;
					 if (yyextra->specialComment)
					 {
					   yyextra->outBuf+='*';
                                           yyextra->col++;
					 }
				       }
    				     }
				    }
<CondLine>[!()&| \ta-z_A-Z0-9.\-]+ {
                                     handleCondSectionId(yyscanner,yytext);
  				   }
<CComment,ReadLine,IncludeFile>[\\@]"cond"{WSopt}/\n {
  				     yyextra->condCtx=YY_START;
                                     handleCondSectionId(yyscanner," "); // fake section id causing the section to be hidden unconditionally
                                   }
<CondLine>\n			   |
<CondLine>.			   { // forgot section id?
                                     handleCondSectionId(yyscanner," "); // fake section id causing the section to be hidden unconditionally
				     if (*yytext=='\n') { copyToOutput(yyscanner,"\n");}
  				   }
<CComment,ReadLine,IncludeFile,Verbatim,VerbatimCode>[\\@][a-z_A-Z][a-z_A-Z0-9-]*  { // expand alias without arguments
				     replaceAliases(yyscanner,yytext,YY_START==ReadLine && yyextra->readLineCtx==SComment);
  				   }
<CComment,ReadLine,IncludeFile,Verbatim,VerbatimCode>{B}?"\\ilinebr"{B}[\\@]"ialias{" { // expand alias with arguments
				     yyextra->lastBlockContext=YY_START;
				     yyextra->blockCount=1;
                                     int extraSpace = (yytext[0]==' '? 1:0);
				     yyextra->aliasString=yytext+9+extraSpace;
				     yyextra->aliasCmd=yytext+9+extraSpace;
				     yyextra->lastEscaped=0;
				     BEGIN( ReadAliasArgs );
				   }
<CComment,ReadLine,IncludeFile,Verbatim,VerbatimCode>[\\@][a-z_A-Z][a-z_A-Z0-9-]*"{" { // expand alias with arguments
                                     yyextra->lastBlockContext=YY_START;
				     yyextra->blockCount=1;
				     yyextra->aliasString=yytext;
				     yyextra->aliasCmd=yytext;
				     yyextra->lastEscaped=0;
				     BEGIN( ReadAliasArgs );
  				   }
<ReadAliasArgs>^[ \t]*"*"          { // skip leading *
                                   }
<ReadAliasArgs>^[ \t]*{CPPC}[/!]/[^\n]*   { // skip leading special comments (see bug 618079)
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
<ReadAliasArgs>[\\@]("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"f$"|"f]"|"f}"|"f)") { /* end of verbatim block */
                                     yyextra->aliasString+=yytext;
                                     if (yyextra->inVerbatim && &yytext[1]==yyextra->blockName)
                                                              // For verbatim sections we do not support matching end block markers inside
                                                              // alias arguments. Instead this will end the verbatim block.
                                                              // This is needed to support an alias definition
                                                              // like startalign=" \latexonly\noalign{\endlatexonly" were otherwise
                                                              // the scanner would try to find a matching closing brace for noalign and then
                                                              // skip over the \endlatexonly command.
                                     {
                                       copyToOutput(yyscanner,yyextra->aliasString.view());
                                       yyextra->inVerbatim=false;
				       BEGIN(yyextra->lastCommentContext);
                                     }
                                   }
<ReadAliasArgs>\n		   {
                                     yyextra->aliasString+=yytext;
				     yyextra->lastEscaped=FALSE;
                                     if (yyextra->inVerbatim) // for verbatim sections we do not support multi-line
                                                              // alias arguments.
                                     {
                                       copyToOutput(yyscanner,yyextra->aliasString.view());
				       BEGIN( yyextra->lastBlockContext );
                                     }
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
				       replaceAliases(yyscanner,yyextra->aliasString.view(),
                                           yyextra->lastBlockContext==ReadLine && yyextra->readLineCtx==SComment);
				       BEGIN( yyextra->lastBlockContext );
				     }
				     yyextra->lastEscaped=FALSE;
  			           }
<ReadAliasArgs>.		   {
                                     yyextra->aliasString+=yytext;
				     yyextra->lastEscaped=FALSE;
  				   }
<CopyLine>.                        {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<CopyLine>\n                       {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                     yyextra->insertCppCommentMarker=false;
                                     BEGIN(yyextra->readLineCtx);
                                   }
<ReadLine>```                      {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<ReadLine>`[^`]+`                  {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<ReadLine>{CMD}{CMD}               |
<ReadLine>.			   {
  				     copyToOutput(yyscanner,yytext,yyleng);
  				   }
<IncludeFile>.                     {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<IncludeFile>\n                    {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                     insertCommentStart(yyscanner);
                                   }
<*>.                               {
  				     copyToOutput(yyscanner,yytext,yyleng);
                                   }
<<EOF>>                            {
                                     if (YY_START == ReadAliasArgs)
                                     {
                                       warn(yyextra->fileName,yyextra->lineNr,
                                           "Reached end of file while still searching closing '}' of an alias argument (probable start: '%s')",
                                           qPrint(yyextra->aliasCmd));
                                     }
                                     if (yyextra->includeStack.empty())
                                     {
                                       yyextra->insertCppCommentMarker=false;
                                       yyterminate();
                                     }
                                     else // switch back to parent file
                                     {
                                       std::unique_ptr<commentcnv_FileState> &fs = yyextra->includeStack.back();
                                       YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
                                       yy_switch_to_buffer(fs->bufState, yyscanner);
                                       yy_delete_buffer(oldBuf, yyscanner);
                                       BEGIN(fs->oldState);
                                       yyextra->fileName      = fs->oldFileName;
                                       yyextra->lineNr        = fs->oldLineNr;
                                       yyextra->inBuf         = fs->oldFileBuf;
                                       yyextra->inBufPos      = fs->oldFileBufPos;
                                       yyextra->includeCtx    = fs->oldIncludeCtx;
                                       QCString lineStr= " \\ifile \""+yyextra->fileName+"\" \\iline "+QCString().setNum(yyextra->lineNr)+" ";
                                       if (fs->oldRaiseLvl!=yyextra->raiseLevel)
                                       {
                                          lineStr+="\\iraise " + std::to_string(fs->oldRaiseLvl)+ " ";
                                       }
                                       if (fs->oldRaiseLbl!=yyextra->raiseLabel)
                                       {
                                          lineStr+="\\iprefix \"" + fs->oldRaiseLbl + "\" ";
                                       }
                                       lineStr+="\\ilinebr ";
                                       yyextra->raiseLevel    = fs->oldRaiseLvl;
                                       yyextra->raiseLabel    = fs->oldRaiseLbl;
                                       copyToOutput(yyscanner,lineStr.view());
                                       yyextra->includeStack.pop_back();
                                       //printf("<<EOF>> switch back to %s line %d inbufPos=%d outbufPos=%d\n",
                                       //    qPrint(yyextra->fileName),yyextra->lineNr,yyextra->inBufPos,yyextra->outBuf.curPos());
                                     }
                                   }
  /*
<*>\n  { fprintf(stderr,"Lex scanner %s (%s) default rule newline for state %s.\n", __FILE__, qPrint(yyextra->fileName),stateToString(YY_START));}
  */
%%

static bool parseIncludeOptions(yyscan_t yyscanner,std::string_view s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  //printf("parseIncludeOptions=%s\n",qPrint(QCString(s)));
  size_t optIdxStart = s.find('{');
  size_t optIdxEnd   = optIdxStart!=std::string::npos ? s.find("}",optIdxStart+1) : std::string::npos;
  std::string cmdName;
  StringVector optList;
  if (optIdxStart == std::string::npos) // no options
  {
    cmdName = stripWhiteSpace(s.substr(1)); // to remove {CMD}
  }
  else // options present
  {
    cmdName = stripWhiteSpace(s.substr(1,optIdxStart-1)); // to remove {CMD}
    optList = split(std::string{s.substr(optIdxStart+1,optIdxEnd-optIdxStart-1)},",");
  }
  bool isDoc = cmdName=="includedoc" || cmdName=="snippetdoc";
  for (const auto &opt : optList)
  {
    if (stripWhiteSpace(opt)==std::string_view{"doc"})
    {
      isDoc=true;
      break;
    }
  }

  if (isDoc)
  {
    for (const auto &opt : optList)
    {
      std::string_view locOpt = stripWhiteSpace(opt);
      size_t posEqual = locOpt.find('=');
      std::string_view option = posEqual!=std::string::npos ? stripWhiteSpace(locOpt.substr(0,posEqual)) : locOpt;
      std::string_view value  = posEqual!=std::string::npos ? stripWhiteSpace(locOpt.substr(posEqual+1)) : std::string_view();

      if (option==std::string_view{"doc"} && value.empty())
      {
      }
      else if (option==std::string_view{"raise"} && !value.empty())
      {
        yyextra->raiseIncrement = atoi(value.data());
        if (yyextra->raiseLevel+yyextra->raiseIncrement>=SectionType::MaxLevel) // check range
        {
          warn(yyextra->fileName,yyextra->lineNr,"Raising section level from %d to %d, exceeds allowed range [0-%d], adjusting",
              yyextra->raiseLevel,yyextra->raiseLevel+yyextra->raiseIncrement,SectionType::MaxLevel-1);
          yyextra->raiseIncrement = std::max(0,SectionType::MaxLevel-1-yyextra->raiseLevel);
        }
      }
      else if (option==std::string_view{"prefix"} && !value.empty())
      {
        yyextra->raisePrefix = value;
      }
      else
      {
        warn(yyextra->fileName,yyextra->lineNr,"Unsupported option '%s' for %s command",qPrint(option), qPrint(cmdName));
      }
    }
  }

  return isDoc;
}


static void replaceCommentMarker(yyscan_t yyscanner,std::string_view s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (s.empty()) return;
  size_t p   = 0;
  size_t len = s.length();
  char c = 0;
  // copy leading blanks
  while (p<len && (c=s[p]) && (c==' ' || c=='\t' || c=='\n'))
  {
    yyextra->outBuf+=c;
    if (c=='\n') { yyextra->lineNr++; yyextra->col=0; } else { yyextra->col++; }
    p++;
  }
  // replace start of comment marker by blanks and the last character by a *
  int blanks=0;
  while (p<len && (c=s[p]) && (c=='/' || c=='!' || c=='#'))
  {
    blanks++;
    p++;
    if (p<len && s[p]=='<') // comment-after-item marker
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
      yyextra->outBuf+=' ';
      yyextra->col++;
      blanks--;
    }
    if (blanks>1) { yyextra->outBuf+='*'; yyextra->col++; }
    yyextra->outBuf+=' ';
    yyextra->col++;
  }
  // copy comment line to output
  yyextra->outBuf+=s.substr(p);
  yyextra->col+=s.substr(p).length();
}

static inline int computeIndent(const char *s)
{
  int col=0;
  int tabSize=Config_getInt(TAB_SIZE);
  const char *p=s;
  char c = 0;
  while ((c=*p++))
  {
    if (c==' ') col++;
    else if (c=='\t') col+=tabSize-(col%tabSize);
    else break;
  }
  return col;
}

static inline void copyToOutput(yyscan_t yyscanner,std::string_view s)
{
  int tabSize=Config_getInt(TAB_SIZE);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  size_t len = s.length();
  if (yyextra->skip) // only add newlines.
  {
    for (size_t i=0;i<len;i++)
    {
      switch(s[i])
      {
        case '\n':
	  yyextra->outBuf+='\n';
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
    yyextra->outBuf+=s;
    for (size_t i=0;i<len;i++)
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

static inline void copyToOutput(yyscan_t yyscanner,const char *s,int len)
{
  copyToOutput(yyscanner,std::string_view{s,(size_t)len});
}

static void clearCommentStack(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  while (!yyextra->commentStack.empty()) yyextra->commentStack.pop();
}

static void startCondSection(yyscan_t yyscanner,const QCString &sectId)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("startCondSection: skip=%d stack=%d\n",g_skip,g_condStack.count());
  CondParser prs;
  bool expResult = prs.parse(yyextra->fileName,yyextra->lineNr,sectId);
  yyextra->condStack.emplace(yyextra->lineNr,sectId,yyextra->skip);
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
    const commentcnvYY_CondCtx &ctx = yyextra->condStack.top();
    yyextra->skip=ctx.skip;
    yyextra->condStack.pop();
  }
  //printf("endCondSection: skip=%d stack=%d\n",g_skip,g_condStack.count());
}

static void handleCondSectionId(yyscan_t yyscanner,const char *expression)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool oldSkip=yyextra->skip;
  startCondSection(yyscanner,QCString(expression));
  if ((yyextra->condCtx==CComment || yyextra->readLineCtx==SComment) &&
      !oldSkip && yyextra->skip)
  {
    if (yyextra->lang!=SrcLangExt::Python &&
        yyextra->lang!=SrcLangExt::VHDL &&
        yyextra->lang!=SrcLangExt::Markdown &&
        yyextra->lang!=SrcLangExt::Fortran)
    {
      yyextra->outBuf+='*';
      yyextra->outBuf+='/';
      yyextra->col+=2;
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

/** Returns the section of text, in between a pair of markers.
 *  Full lines are returned, excluding the lines on which the markers appear.
 *  \sa routine lineBlock
 */
static QCString extractBlock(const QCString &text,const QCString &marker,int &blockPos)
{
  QCString result;
  int p=0,i=-1;
  bool found=FALSE;

  // find the character positions of the markers
  int m1 = text.find(marker);
  if (m1==-1) return result;
  int m2 = text.find(marker,m1+static_cast<int>(marker.length()));
  if (m2==-1) return result;

  // find start and end line positions for the markers
  int l1=-1,l2=-1;
  while (!found && (i=text.find('\n',p))!=-1)
  {
    found = (p<=m1 && m1<i); // found the line with the start marker
    p=i+1;
  }
  l1=p;
  blockPos=p;
  int lp=i;
  if (found)
  {
    while ((i=text.find('\n',p))!=-1)
    {
      if (p<=m2 && m2<i) // found the line with the end marker
      {
        l2=p;
        break;
      }
      p=i+1;
      lp=i;
    }
  }
  if (l2==-1) // marker at last line without newline (see bug706874)
  {
    l2=lp;
  }
  return l2>l1 ? text.mid(l1,l2-l1) : QCString();
}

static void insertCommentStart(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int startCol=yyextra->blockHeadCol;
  int contentCol=yyextra->insertCommentCol;
  int markerSpace=contentCol-startCol;
  //printf("insertCommentStart startCol=%d contentCol=%d mlBrief=%d insertCppCommentMarker=%d\n",
  //    yyextra->blockHeadCol,yyextra->insertCommentCol,yyextra->mlBrief,yyextra->insertCppCommentMarker);
  std::string marker;
  if (yyextra->lang==SrcLangExt::Python) // need to insert # or space
  {
    if (yyextra->pythonDocString) // """! style comment
    {
      marker="    ";
    }
    else
    {
      marker="# ";
    }
  }
  else if (yyextra->lang==SrcLangExt::Fortran) // need to insert !!
  {
    marker="!! ";
  }
  else if (yyextra->lang==SrcLangExt::Markdown)
  {
    marker="  ";
  }
  else if (yyextra->insertCppCommentMarker) // need to insert ///
  {
    marker="/// ";
    if (startCol>0)
    {
      // insert `///` instead of '* '
      startCol--;
      markerSpace++;
    }
  }
  else // need to insert *
  {
    marker="* ";
  }
  int i=0;
  for (;i<startCol;i++)
  {
    copyToOutput(yyscanner," ");
  }
  if (static_cast<int>(marker.length())<=markerSpace && !yyextra->firstIncludeLine)
  {
    copyToOutput(yyscanner,marker);
    i+=marker.length();
  }
  for (;i<contentCol;i++)
  {
    copyToOutput(yyscanner," ");
  }
  yyextra->firstIncludeLine = false;
}

static bool readIncludeFile(yyscan_t yyscanner,const QCString &inc,const QCString &blockId)
{
  //printf("readIncludeFile(inc=%s,blockId=%s)\n",qPrint(inc),qPrint(blockId));
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool ambig = false;
  QCString absFileName = findFilePath(inc,ambig);
  FileInfo fi(absFileName.str());
  if (!absFileName.isEmpty() && fi.exists() && fi.isFile())
  {
    if (ambig)
    {
      warn_doc_error(yyextra->fileName,yyextra->lineNr,"included file name '%s' is ambiguous"
	  "Possible candidates:\n%s",qPrint(inc),
	  qPrint(showFileDefMatches(Doxygen::exampleNameLinkedMap,inc))
	  );
    }
    bool alreadyProcessed = std::any_of(
      yyextra->includeStack.begin(),
      yyextra->includeStack.end(),
      [&absFileName,&blockId](const auto &lfs)
        { return lfs->fileName==absFileName && lfs->blockId==blockId; }
    );

    if (alreadyProcessed)
    {
      if (!blockId.isEmpty())
      {
        warn_doc_error(yyextra->fileName,yyextra->lineNr,"recursive usage of '\\snippet{doc}' block with name '%s' and file name '%s', skipping",
             qPrint(blockId),qPrint(absFileName));
      }
      else
      {
        warn_doc_error(yyextra->fileName,yyextra->lineNr,"recursive usage of '\\include{doc}' with file name '%s', skipping", qPrint(absFileName));
      }
      return false;
    }

    auto fs = std::make_unique<commentcnv_FileState>();
    if (!readInputFile(absFileName,fs->fileBuf,false))
    {
      warn_doc_error(yyextra->fileName,yyextra->lineNr,"\\%s{doc} file '%s' could not be read",blockId.isEmpty()?"include":"snippet",qPrint(absFileName));
      fs.reset();
      return false;
    }
    int lineNr=1;
    if (!blockId.isEmpty())
    {
      QCString incText { fs->fileBuf };
      int count = incText.contains(blockId.data());
      if (count!=2)
      {
        warn_doc_error(yyextra->fileName,yyextra->lineNr,"block marked with [%s] for \\snippet{doc} should appear twice in file %s, found it %d times, skipping",
            qPrint(blockId),qPrint(absFileName),count);
        return false;
      }
      lineNr = lineBlock(incText, blockId);
      int blockPos = 0;
      incText = extractBlock(incText, blockId, blockPos);
      fs->fileBuf.clear();
      if (!incText.isEmpty())
      {
        fs->fileBuf.append(incText.str());
      }
    }
    int oldRaiseLevel = yyextra->raiseLevel;
    QCString oldRaiseLabel = yyextra->raiseLabel;
    yyextra->raiseLevel+=yyextra->raiseIncrement;
    yyextra->raiseLabel+=yyextra->raisePrefix;
    QCString lineStr=" \\ifile \""+absFileName+"\" \\iline " + std::to_string(lineNr)+" ";
    if (yyextra->raiseLevel>0)
    {
      lineStr+="\\iraise " + std::to_string(yyextra->raiseLevel)+" ";
    }
    if (!yyextra->raiseLabel.isEmpty())
    {
      lineStr+="\\iprefix \"" + yyextra->raiseLabel + "\" ";
    }
    lineStr+="\\ilinebr ";
    copyToOutput(yyscanner,lineStr.view());

    fs->fileName      = absFileName;
    fs->bufState      = YY_CURRENT_BUFFER;
    fs->oldLineNr     = yyextra->lineNr;
    fs->oldFileName   = yyextra->fileName;
    fs->oldState      = yyextra->includeCtx;
    fs->oldFileBuf    = yyextra->inBuf;
    fs->oldFileBufPos = yyextra->inBufPos;
    fs->oldIncludeCtx = yyextra->includeCtx;
    fs->oldRaiseLvl   = oldRaiseLevel;
    fs->oldRaiseLbl   = oldRaiseLabel;
    fs->blockId       = blockId;
    yy_switch_to_buffer(yy_create_buffer(nullptr, YY_BUF_SIZE, yyscanner),yyscanner);
    yyextra->fileName = absFileName;
    yyextra->lineNr   = lineNr;
    yyextra->inBuf    = &fs->fileBuf;
    yyextra->inBufPos = 0;
    yyextra->includeStack.push_back(std::move(fs));
    insertCommentStart(yyscanner);
    //printf("switched to %s\n",qPrint(fileName));
  }
  else
  {
    warn_doc_error(yyextra->fileName,yyextra->lineNr,"\\%s{doc} file '%s' not found",blockId.isEmpty()?"include":"snippet",qPrint(inc));
    return false;
  }
  return true;
}

/** copies string \a s with length \a len to the output, while
 *  replacing any alias commands found in the string.
 */
static void replaceAliases(yyscan_t yyscanner,std::string_view s,bool replaceComment)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (s.empty()) return;
  size_t pos = s.find('{');
  std::string cmd { s.substr(1, pos!=std::string::npos ? pos-1 : s.length()-1) };
  auto &expAlias = yyextra->expandedAliases;
  if (std::find(expAlias.begin(),expAlias.end(),cmd)!=std::end(expAlias))
  {
    copyToOutput(yyscanner,s);
    return; // prevent recursive expansion
  }
  else if (cmd=="ialias")
  {
    if (s.length()>cmd.length()+3) // \cmd{value}
    {
      std::string value { s.substr(cmd.length()+2,s.length()-cmd.length()-3) };
      //printf("removing value '%s'\n",qPrint(value));
      expAlias.erase(std::remove(expAlias.begin(),expAlias.end(),value),expAlias.end());
    }
    return;
  }
  std::string result = resolveAliasCmd(s);
  if (yyextra->inVerbatim) // inside verbatim blocks (like @code...@endcode) only expand aliases if
                           // their expansion contains the matching end block marker.
  {
    std::string blk = yyextra->blockName.str();
    assert(!blk.empty());
    bool isNamedCommand=isId(blk[0]); // true for e.g. @endcode, false for e.g. ~~~
    size_t i=0,p=0;
    bool found=false;
    while ((i=result.find(blk,p))!=std::string::npos && !found) // for each match of blk in result
    {
      found = !isNamedCommand ||                                                                  // e.g. '~~~' or '-->'
              (i>0 && (result[i-1]=='\\' || result[i-1]=='@') && !isId(result[i+blk.length()]));  // e.g. '@endcode' but not ~endcode or @endcodex
      p = i+yyextra->blockName.length();
    }
    //printf("blk=%s result=%s found=%d\n",qPrint(blk),qPrint(result),found);
    if (!found) // treat alias as part of the verbatim block
    {
      copyToOutput(yyscanner,s);
      return;
    }
  }
  //printf("replaceAliases(%s)->'%s' replaceComment=%d\n",qPrint(s),qPrint(result),replaceComment);
  if (result!=s)
  {
    if (replaceComment) // In case we are replacing a multiline /// comment by a C style comment
                        // and we have new lines in the alias argument, we need to place back a /// for each new line
                        // to prevent breaking the multiline comment into multiple C style comments
    {
      result = substituteStringView(result,"\n","\n///");
    }
    expAlias.push_back(cmd);
    // add a ialias command to allow expansion of cmd again
    result += " \\ilinebr \\ialias{";
    result += cmd;
    result += "}";
    for (int i=(int)result.length()-1; i>=0; i--)
    {
      unput(result[i]);
    }
  }
  else
  {
    copyToOutput(yyscanner,result);
  }
}


static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int bytesInBuf = static_cast<int>(yyextra->inBuf->size())-yyextra->inBufPos;
  int bytesToCopy = std::min(max_size,bytesInBuf);
  memcpy(buf,yyextra->inBuf->data()+yyextra->inBufPos,bytesToCopy);
  yyextra->inBufPos+=bytesToCopy;
  return bytesToCopy;
}

static void replaceComment(yyscan_t yyscanner,int offset)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->mlBrief || yyextra->skip)
  {
    copyToOutput(yyscanner,yytext,yyleng);
  }
  else
  {
    int i=computeIndent(&yytext[offset]);
    //printf("i=%d blockHeadCol=%d\n",i,yyextra->blockHeadCol);
    if (i==yyextra->blockHeadCol || i+1==yyextra->blockHeadCol)
    {
      replaceCommentMarker(yyscanner,std::string_view(yytext,yyleng));
    }
    else
    {
      copyToOutput(yyscanner," */");
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
void convertCppComments(const std::string &inBuf,std::string &outBuf,const std::string &fn)
{
  QCString fileName { fn };
  yyscan_t yyscanner;
  commentcnvYY_state extra(&inBuf,outBuf);
  commentcnvYYlex_init_extra(&extra,&yyscanner);
#ifdef FLEX_DEBUG
  commentcnvYYset_debug(Debug::isFlagSet(Debug::Lex_commentcnv)?1:0,yyscanner);
#endif
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("convertCppComments(%s)\n",qPrint(fileName));
  yyextra->inBufPos = 0;
  yyextra->col      = 0;
  yyextra->mlBrief = Config_getBool(MULTILINE_CPP_IS_BRIEF);
  yyextra->skip     = FALSE;
  yyextra->fileName = fileName;
  yyextra->lang = getLanguageFromFileName(fileName);
  yyextra->pythonDocString = FALSE;
  yyextra->lineNr   = 1;
  yyextra->raiseLevel = 0;
  yyextra->raiseLabel = "";
  yyextra->raiseIncrement = 0;
  yyextra->raisePrefix = "";
  yyextra->insertCppCommentMarker=false;
  yyextra->expandedAliases.clear();
  while (!yyextra->condStack.empty()) yyextra->condStack.pop();
  clearCommentStack(yyscanner);
  yyextra->vhdl = FALSE;

  DebugLex debugLex(Debug::Lex_commentcnv,__FILE__, qPrint(fileName));
  yyextra->isFixedForm = FALSE;
  if (yyextra->lang==SrcLangExt::Fortran)
  {
    FortranFormat fmt = convertFileNameFortranParserCode(fileName);
    yyextra->isFixedForm = recognizeFixedForm(QCString(inBuf),fmt);
  }

  if (yyextra->lang==SrcLangExt::Markdown)
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
    const commentcnvYY_CondCtx &ctx = yyextra->condStack.top();
    QCString sectionInfo(" ");
    if (ctx.sectionId!=" ") sectionInfo.sprintf(" with label '%s' ",ctx.sectionId.stripWhiteSpace().data());
    warn(yyextra->fileName,ctx.lineNr,"Conditional section%sdoes not have "
	"a corresponding \\endcond command within this file.",sectionInfo.data());
    yyextra->condStack.pop();
  }
  if (yyextra->nestingCount>0 && yyextra->lang!=SrcLangExt::Markdown && yyextra->lang!=SrcLangExt::Fortran)
  {
    QCString tmp("(probable line reference: ");
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
    Debug::print(Debug::CommentCnv,0,"-----------\nCommentCnv: %s\n"
                 "output=[\n%s]\n-----------\n",qPrint(fileName),qPrint(yyextra->outBuf)
                );
  }
  commentcnvYYlex_destroy(yyscanner);
}


//----------------------------------------------------------------------------

#include "commentcnv.l.h"
