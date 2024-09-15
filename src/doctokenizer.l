/******************************************************************************
 *
 * $Id: $
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
%option prefix="doctokenizerYY"
%option reentrant
%option extra-type="struct doctokenizerYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

#include <ctype.h>
#include <stack>
#include <string>
#include <cassert>

#include "doctokenizer.h"
#include "cmdmapper.h"
#include "config.h"
#include "message.h"
#include "section.h"
#include "membergroup.h"
#include "definition.h"
#include "doxygen.h"
#include "portable.h"
#include "cite.h"
#include "regex.h"
#include "debug.h"
#include "docnode.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

//--------------------------------------------------------------------------

struct DocLexerContext
{
  DocLexerContext(const TokenInfo &tk,int r,int lvl,yy_size_t pos,const char *s,YY_BUFFER_STATE bs)
    : token(tk), rule(r), autoListLevel(lvl), inputPos(pos), inputString(s), state(bs) {}
  TokenInfo token;
  int rule;
  int autoListLevel;
  yy_size_t inputPos;
  const char *inputString;
  YY_BUFFER_STATE state;
};

struct doctokenizerYY_state
{

  // context for tokenizer phase
  int commentState;
  TokenInfo token;
  yy_size_t inputPos = 0;
  const char *inputString = nullptr;
  QCString fileName;
  bool insidePre = false;
  int sharpCount=0;
  bool markdownSupport=true;
  bool insideHtmlLink=false;

  // context for section finding phase
  const Definition  *definition = nullptr;
  QCString     secLabel;
  QCString     secTitle;
  SectionType  secType;
  QCString     endMarker;
  int          autoListLevel;
  std::stack< std::unique_ptr<DocLexerContext> > lexerStack;
  std::stack<int> stateStack;

  int yyLineNr = 0;
};

#define lineCount(s,len) do { for(int i=0;i<(int)len;i++) if (s[i]=='\n') yyextra->yyLineNr++; } while(0)


[[maybe_unused]] static const char *stateToString(int state);

static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void handleHtmlTag(yyscan_t yyscanner,const char *text);
static void processSection(yyscan_t yyscanner);

//--------------------------------------------------------------------------

QCString extractPartAfterNewLine(const QCString &text)
{
  int nl1 = text.find('\n');
  int nl2 = text.find("\\ilinebr");
  if (nl1!=-1 && nl1<nl2)
  {
    return text.mid(nl1+1);
  }
  if (nl2!=-1)
  {
    if (text.at(nl2+8)==' ') nl2++; // skip space after \\ilinebr
    return text.mid(nl2+8);
  }
  return text;
}

//--------------------------------------------------------------------------

static int computeIndent(const char *str,size_t length)
{
  if (str==0 || length==std::string::npos) return 0;
  size_t i;
  int indent=0;
  int tabSize=Config_getInt(TAB_SIZE);
  for (i=0;i<length;i++)
  {
    if (str[i]=='\t')
    {
      indent+=tabSize - (indent%tabSize);
    }
    else if (str[i]=='\n')
    {
      indent=0;
    }
    else if (str[i]=='\\' && qstrncmp(str+i+1,"ilinebr",7)==0)
    {
      indent=0;
      i+=7;
      if (str[i+1]==' ') i++; // also eat space after \\ilinebr if present
    }
    else
    {
      indent++;
    }
  }
  //printf("input('%s')=%d\n",str,indent);
  return indent;
}

//--------------------------------------------------------------------------

#define unput_string(yytext,yyleng) do { for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]); } while(0)
//--------------------------------------------------------------------------

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

//--------------------------------------------------------------------------
#define YY_DECL static Token doctokenizerYYlex(yyscan_t yyscanner)
//#define LOCAL_YY_DECL local_doctokenizer(yyscan_t yyscanner)
#ifndef yyterminate
#define yyterminate() return Token::make_TK_EOF()
#endif

%}

CMD       ("\\"|"@")
WS        [ \t\r\n]
NONWS     [^ \t\r\n]
BLANK     [ \t\r]
BLANKopt  {BLANK}*
ID        [$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*
LABELID   [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*
PHPTYPE   [?]?[\\:a-z_A-Z0-9\x80-\xFF\-]+
CITESCHAR [a-z_A-Z0-9\x80-\xFF\-\?]
CITEECHAR [a-z_A-Z0-9\x80-\xFF\-\+:\/\?]
CITEID    {CITESCHAR}{CITEECHAR}*("."{CITESCHAR}{CITEECHAR}*)*|"\""{CITESCHAR}{CITEECHAR}*("."{CITESCHAR}{CITEECHAR}*)*"\""
DOXYCFG   [A-Z][A-Z_0-9]*
MAILADDR  ("mailto:")?[a-z_A-Z0-9\x80-\xFF.+-]+"@"[a-z_A-Z0-9\x80-\xFf-]+("."[a-z_A-Z0-9\x80-\xFf\-]+)+[a-z_A-Z0-9\x80-\xFf\-]+
MAILWS    [\t a-z_A-Z0-9\x80-\xFF+-]
MAILADDR2 {MAILWS}+{BLANK}+("at"|"AT"|"_at_"|"_AT_"){BLANK}+{MAILWS}+("dot"|"DOT"|"_dot_"|"_DOT_"|"point"|"POINT"|"_point_"|"_POINT_"){BLANK}+{MAILWS}+
OPTSTARS  ("/""/"{BLANK}*)?"*"*{BLANK}*
LISTITEM  {BLANK}*[-]("#")?{WS}
MLISTITEM {BLANK}*[+*]{WS}
OLISTITEM {BLANK}*("0"|[1-9][0-9]*)"."{BLANK}
CLISTITEM {BLANK}*[-]{BLANK}*"\["[ xX]"\]"
ENDLIST   {BLANK}*"."{BLANK}*(\n|"\\ilinebr")
ATTRNAME  [a-z_A-Z\x80-\xFF][:a-z_A-Z0-9\x80-\xFF\-]*
ATTRIB    {ATTRNAME}{WS}*("="{WS}*(("\""[^\"]*"\"")|("'"[^\']*"'")|[^ \t\r\n'"><]+))?
URLCHAR   [a-z_A-Z0-9\!\~\,\:\;\'\$\?\@\&\%\#\.\-\+\/\=\x80-\xFF]
URLMASK   ({URLCHAR}+([({]{URLCHAR}*[)}])?)+
URLPROTOCOL ("http:"|"https:"|"ftp:"|"ftps:"|"sftp:"|"file:"|"news:"|"irc:"|"ircs:")
FILEICHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+=&#@]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+=&#@]
FILECHARS {FILEICHAR}*{FILEECHAR}+
HFILEMASK {FILEICHAR}*("."{FILEICHAR}+)+{FILECHARS}*
VFILEMASK {FILECHARS}("."{FILECHARS})*
FILEMASK  {VFILEMASK}|{HFILEMASK}
LINKMASK  [^ \t\n\r\\@<&${}]+("("[^\n)]*")")?({BLANK}*("const"|"volatile"){BLANK}+)?
VERBATIM  "verbatim"{BLANK}*
SPCMD1    {CMD}([a-z_A-Z][a-z_A-Z0-9]*|{VERBATIM}|"--"|"---")
SPCMD2    {CMD}[\\@<>&$#%~".+=|-]
SPCMD3    {CMD}_form#[0-9]+
SPCMD4    {CMD}"::"
SPCMD5    {CMD}":"
INOUT     "in"|"out"|("in"{BLANK}*","?{BLANK}*"out")|("out"{BLANK}*","?{BLANK}*"in")
PARAMIO   {CMD}param{BLANK}*"["{BLANK}*{INOUT}{BLANK}*"]"
VARARGS   "..."
TEMPCHAR  [a-z_A-Z0-9.,: \t\*\&\(\)\[\]]
FUNCCHAR  [a-z_A-Z0-9,:\<\> \t\n\^\*\&\[\]]|{VARARGS}|"\\ilinebr"
FUNCPART  {FUNCCHAR}*("("{FUNCCHAR}*")"{FUNCCHAR}*)?
SCOPESEP  "::"|"#"|"."
TEMPLPART "<"{TEMPCHAR}*("<"{TEMPCHAR}*("<"{TEMPCHAR}*">")?">")?">"
ANONNS    "anonymous_namespace{"[^}]*"}"
SCOPEPRE  (({ID}{TEMPLPART}?)|{ANONNS}){SCOPESEP}
SCOPEKEYS ":"({ID}":")*
SCOPECPP  {SCOPEPRE}*(~)?{ID}{TEMPLPART}?
SCOPEOBJC {SCOPEPRE}?{ID}{SCOPEKEYS}?
SCOPEMASK {SCOPECPP}|{SCOPEOBJC}
FUNCARG   "("{FUNCPART}")"({BLANK}*("volatile"|"const"){BLANK})?
FUNCARG2  "("{FUNCPART}")"({BLANK}*("volatile"|"const"))?
OPNEW     {BLANK}+"new"({BLANK}*"[]")?
OPDEL     {BLANK}+"delete"({BLANK}*"[]")?
OPNORM    {OPNEW}|{OPDEL}|"+"|"-"|"*"|"/"|"%"|"^"|"&"|"|"|"~"|"!"|"="|"<"|">"|"+="|"-="|"*="|"/="|"%="|"^="|"&="|"|="|"<<"|">>"|"<<="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|","|"->*"|"->"|"[]"|"()"|"<=>"
OPCAST    {BLANK}+[^<(\r\n.,][^(\r\n.,]*
OPMASK    ({BLANK}*{OPNORM}{FUNCARG})
OPMASKOPT ({BLANK}*{OPNORM}{FUNCARG}?)|({OPCAST}{FUNCARG})
OPMASKOP2 ({BLANK}*{OPNORM}{FUNCARG2}?)|({OPCAST}{FUNCARG2})
LNKWORD1  ("::"|"#")?{SCOPEMASK}
CVSPEC    {BLANK}*("const"|"volatile")
LNKWORD2  (({SCOPEPRE}*"operator"{OPMASK})|({SCOPEPRE}"operator"{OPMASKOPT})|(("::"|"#"){SCOPEPRE}*"operator"{OPMASKOPT})){CVSPEC}?
LNKWORD3  ([0-9a-z_A-Z\-]+("/"|"\\"))*[0-9a-z_A-Z\-]+("."[0-9a-z_A-Z]+)+
CHARWORDQ [^ \t\n\r\\@<>()\[\]:;\?{}&%$#,."=']
ESCWORD   ("%"{ID}(("::"|"."){ID})*)|("%'")
CHARWORDQ1 [^ \-+0-9\t\n\r\\@<>()\[\]:;\?{}&%$#,."=']
WORD1     {ESCWORD}|{CHARWORDQ1}{CHARWORDQ}*|"{"|"}"|"'\"'"|("\""([^"\n]*(\\\"|\n)?)*[^"\n]*"\"")
WORD2     "."|","|"("|")"|"["|"]"|"::"|":"|";"|"\?"|"="|"'"
WORD1NQ   {ESCWORD}|{CHARWORDQ}+|"{"|"}"
WORD2NQ   "."|","|"("|")"|"["|"]"|"::"|":"|";"|"\?"|"="|"'"
CAPTION   [cC][aA][pP][tT][iI][oO][nN]
HTMLTAG   "<"(("/")?){ID}({WS}+{ATTRIB})*{WS}*(("/")?)">"
HTMLKEYL  "strong"|"center"|"table"|"caption"|"small"|"code"|"dfn"|"var"|"img"|"pre"|"sub"|"sup"|"tr"|"td"|"th"|"ol"|"ul"|"li"|"tt"|"kbd"|"em"|"hr"|"dl"|"dt"|"dd"|"br"|"i"|"a"|"b"|"p"|"strike"|"u"|"del"|"ins"|"s"
HTMLKEYU  "STRONG"|"CENTER"|"TABLE"|"CAPTION"|"SMALL"|"CODE"|"DFN"|"VAR"|"IMG"|"PRE"|"SUB"|"SUP"|"TR"|"TD"|"TH"|"OL"|"UL"|"LI"|"TT"|"KBD"|"EM"|"HR"|"DL"|"DT"|"DD"|"BR"|"I"|"A"|"B"|"P"|"STRIKE"|"U"|"DEL"|"INS"|"S"
HTMLKEYW  {HTMLKEYL}|{HTMLKEYU}
HTMLTAG_STRICT  "<"(("/")?){HTMLKEYW}({WS}+{ATTRIB})*{WS}*(("/")?)">"
REFWORD2_PRE   ("#"|"::")?((({ID}{TEMPLPART}?)|{ANONNS})("."|"#"|"::"|"-"|"/"))*({ID}{TEMPLPART}?(":")?)
REFWORD2       {REFWORD2_PRE}{FUNCARG2}?
REFWORD2_NOCV  {REFWORD2_PRE}("("{FUNCPART}")")?
REFWORD3       ({ID}":")*{ID}":"?
REFWORD4_NOCV  (({SCOPEPRE}*"operator"{OPMASKOP2})|(("::"|"#"){SCOPEPRE}*"operator"{OPMASKOP2}))
REFWORD4       {REFWORD4_NOCV}{CVSPEC}?
REFWORD        {FILEMASK}|{LABELID}|{REFWORD2}|{REFWORD3}|{REFWORD4}
REFWORD_NOCV   {FILEMASK}|{LABELID}|{REFWORD2_NOCV}|{REFWORD3}|{REFWORD4_NOCV}
RCSID "$"("Author"|"Date"|"Header"|"Id"|"Locker"|"Log"|"Name"|"RCSfile"|"Revision"|"Source"|"State")":"[^:\n$][^\n$]*"$"
LINENR {BLANK}*([1-9][0-9]*|"0"|"-1")

SHOWDATE ([0-9]{4}"-"[0-9]{1,2}"-"[0-9]{1,2})?({WS}*[0-9]{1,2}":"[0-9]{1,2}(":"[0-9]{1,2})?)?

%option noyywrap

%x St_Para
%x St_Comment
%x St_Title
%x St_TitleN
%x St_TitleQ
%x St_TitleA
%x St_TitleV
%x St_Code
%x St_iCode
%x St_CodeOpt
%x St_iCodeOpt
%x St_XmlCode
%x St_HtmlOnly
%x St_HtmlOnlyOption
%x St_ManOnly
%x St_LatexOnly
%x St_RtfOnly
%x St_XmlOnly
%x St_DbOnly
%x St_Verbatim
%x St_iVerbatim
%x St_ILiteral
%x St_ILiteralOpt
%x St_Dot
%x St_Msc
%x St_PlantUMLOpt
%x St_PlantUML
%x St_Param
%x St_XRefItem
%x St_XRefItem2
%x St_File
%x St_IFile
%x St_Pattern
%x St_Link
%x St_Cite
%x St_DoxyConfig
%x St_Ref
%x St_Ref2
%x St_IntRef
%x St_Text
%x St_SkipTitle
%x St_Anchor
%x St_Prefix
%x St_Snippet
%x St_SetScope
%x St_SetScopeEnd
%x St_Options
%x St_Block
%x St_Emoji
%x St_ILine
%x St_ShowDate

%x St_Sections
%s St_SecLabel1
%s St_SecLabel2
%s St_SecTitle
%x St_SecSkip

%x St_QuotedString
%x St_QuotedContent

%%
<St_Para>\r            /* skip carriage return */
<St_Para>^{LISTITEM}   { /* list item */
                         if (yyextra->insideHtmlLink || yyextra->insidePre) REJECT;
                         lineCount(yytext,yyleng);
                         QCString text(yytext);
                         uint32_t dashPos = static_cast<uint32_t>(text.findRev('-'));
                         assert(dashPos!=static_cast<uint32_t>(-1));
                         yyextra->token.isEnumList = text.at(dashPos+1)=='#';
                         yyextra->token.isCheckedList = false;
                         yyextra->token.id         = -1;
                         yyextra->token.indent     = computeIndent(yytext,dashPos);
                         return Token::make_TK_LISTITEM();
                       }
<St_Para>^{CLISTITEM}  { /* checkbox item */
                         QCString text=yytext;
                         int dashPos = text.findRev('-');
                         yyextra->token.isEnumList = false;
                         yyextra->token.isCheckedList = true;
                         if (text.find('x') != -1) yyextra->token.id = DocAutoList::Checked_x;
                         else if (text.find('X') != -1) yyextra->token.id = DocAutoList::Checked_X;
                         else yyextra->token.id = DocAutoList::Unchecked;
                         yyextra->token.indent     = computeIndent(yytext,dashPos);
                         return Token::make_TK_LISTITEM();
                       }
<St_Para>^{MLISTITEM}  { /* list item */
                         if (yyextra->insideHtmlLink || !yyextra->markdownSupport || yyextra->insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           lineCount(yytext,yyleng);
                           std::string text(yytext);
                           static const reg::Ex re(R"([*+][^*+]*$)"); // find last + or *
                           reg::Match match;
                           reg::search(text,match,re);
                           size_t listPos = match.position();
                           assert(listPos!=std::string::npos);
                           yyextra->token.isEnumList = false;
                           yyextra->token.isCheckedList = false;
                           yyextra->token.id         = -1;
                           yyextra->token.indent     = computeIndent(yytext,listPos);
                           return Token::make_TK_LISTITEM();
                         }
                       }
<St_Para>^{OLISTITEM}  { /* numbered list item */
                         if (yyextra->insideHtmlLink || !yyextra->markdownSupport || yyextra->insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           std::string text(yytext);
                           static const reg::Ex re(R"(\d+)");
                           reg::Match match;
                           reg::search(text,match,re);
                           size_t markPos = match.position();
                           assert(markPos!=std::string::npos);
                           yyextra->token.isEnumList = true;
                           yyextra->token.isCheckedList = false;
                           bool ok = false;
                           int id = QCString(match.str()).toInt(&ok);
                           yyextra->token.id = ok ? id : -1;
                           if (!ok)
                           {
                             warn(yyextra->fileName,yyextra->yyLineNr,"Invalid number for list item '%s' ",match.str().c_str());
                           }
                           yyextra->token.indent     = computeIndent(yytext,markPos);
                           return Token::make_TK_LISTITEM();
                         }
                       }
<St_Para>{BLANK}*(\n|"\\ilinebr"){LISTITEM}     { /* list item on next line */
                         if (yyextra->insideHtmlLink || yyextra->insidePre) REJECT;
                         lineCount(yytext,yyleng);
                         QCString text=extractPartAfterNewLine(QCString(yytext));
                         uint32_t dashPos = static_cast<uint32_t>(text.findRev('-'));
                         assert(dashPos!=static_cast<uint32_t>(-1));
                         yyextra->token.isEnumList = text.at(dashPos+1)=='#';
                         yyextra->token.isCheckedList = false;
                         yyextra->token.id         = -1;
                         yyextra->token.indent     = computeIndent(text.data(),dashPos);
                         return Token::make_TK_LISTITEM();
                       }
<St_Para>{BLANK}*\n{CLISTITEM}   { /* checkbox item on next line */
                         QCString text=yytext;
                         text=text.right(text.length()-text.find('\n')-1);
                         int dashPos = text.findRev('-');
                         yyextra->token.isEnumList = false;
                         yyextra->token.isCheckedList = true;
                         if (text.find('x') != -1) yyextra->token.id = DocAutoList::Checked_x;
                         else if (text.find('X') != -1) yyextra->token.id = DocAutoList::Checked_X;
                         else yyextra->token.id = DocAutoList::Unchecked;
                         yyextra->token.indent     = computeIndent(text.data(),dashPos);
                         return Token::make_TK_LISTITEM();
                       }
<St_Para>{BLANK}*(\n|"\\ilinebr"){MLISTITEM}     { /* list item on next line */
                         if (yyextra->insideHtmlLink || !yyextra->markdownSupport || yyextra->insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           lineCount(yytext,yyleng);
                           std::string text=extractPartAfterNewLine(QCString(yytext)).str();
                           static const reg::Ex re(R"([*+][^*+]*$)"); // find last + or *
                           reg::Match match;
                           reg::search(text,match,re);
                           size_t markPos = match.position();
                           assert(markPos!=std::string::npos);
                           yyextra->token.isEnumList = FALSE;
                           yyextra->token.isCheckedList = false;
                           yyextra->token.id         = -1;
                           yyextra->token.indent     = computeIndent(text.c_str(),markPos);
                           return Token::make_TK_LISTITEM();
                         }
                       }
<St_Para>{BLANK}*(\n|"\\ilinebr"){OLISTITEM}     { /* list item on next line */
                         if (yyextra->insideHtmlLink || !yyextra->markdownSupport || yyextra->insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           lineCount(yytext,yyleng);
                           std::string text=extractPartAfterNewLine(QCString(yytext)).str();
                           static const reg::Ex re(R"(\d+)");
                           reg::Match match;
                           reg::search(text,match,re);
                           size_t markPos = match.position();
                           assert(markPos!=std::string::npos);
                           yyextra->token.isEnumList = true;
                           yyextra->token.isCheckedList = false;
                           bool ok = false;
                           int id = QCString(match.str()).toInt(&ok);
                           yyextra->token.id = ok ? id : -1;
                           if (!ok)
                           {
                             warn(yyextra->fileName,yyextra->yyLineNr,"Invalid number for list item '%s' ",match.str().c_str());
                           }
                           yyextra->token.indent     = computeIndent(text.c_str(),markPos);
                           return Token::make_TK_LISTITEM();
                         }
                       }
<St_Para>^{ENDLIST}       { /* end list */
                         if (yyextra->insideHtmlLink || yyextra->insidePre) REJECT;
                         lineCount(yytext,yyleng);
                         size_t dotPos = static_cast<size_t>(QCString(yytext).findRev('.'));
                         yyextra->token.indent     = computeIndent(yytext,dotPos);
                         return Token::make_TK_ENDLIST();
                       }
<St_Para>{BLANK}*(\n|"\\ilinebr"){ENDLIST}      { /* end list on next line */
                         if (yyextra->insideHtmlLink || yyextra->insidePre) REJECT;
                         lineCount(yytext,yyleng);
                         QCString text=extractPartAfterNewLine(QCString(yytext));
                         size_t dotPos = static_cast<size_t>(text.findRev('.'));
                         yyextra->token.indent = computeIndent(text.data(),dotPos);
                         return Token::make_TK_ENDLIST();
                       }
<St_Para>"{"{BLANK}*"@linkplain"/{WS}+ {
                         yyextra->token.name = "javalinkplain";
                         return Token::make_TK_COMMAND_AT();
                       }
<St_Para>"{"{BLANK}*"@link"/{WS}+ {
                         yyextra->token.name = "javalink";
                         return Token::make_TK_COMMAND_AT();
                       }
<St_Para>"{"{BLANK}*"@inheritDoc"{BLANK}*"}" {
                         yyextra->token.name = "inheritdoc";
                         return Token::make_TK_COMMAND_AT();
                       }
<St_Para>"@_fakenl"    { // artificial new line
                         //yyextra->yyLineNr++;
                       }
<St_Para>{SPCMD3}      {
                         yyextra->token.name = "_form";
                         bool ok;
                         yyextra->token.id = QCString(yytext).right((int)yyleng-7).toInt(&ok);
                         ASSERT(ok);
                         return Token::char_to_command(yytext[0]);
                       }
<St_Para>{CMD}"n"\n    { /* \n followed by real newline */
                         lineCount(yytext,yyleng);
                         //yyextra->yyLineNr++;
                         yyextra->token.name = yytext+1;
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         yyextra->token.paramDir=TokenInfo::Unspecified;
                         return Token::char_to_command(yytext[0]);
                       }
<St_Para>"\\ilinebr"   {
                       }
<St_Para>{SPCMD1}      |
<St_Para>{SPCMD2}      |
<St_Para>{SPCMD5}      |
<St_Para>{SPCMD4}      { /* special command */
                         yyextra->token.name = yytext+1;
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         yyextra->token.paramDir=TokenInfo::Unspecified;
                         return Token::char_to_command(yytext[0]);
                       }
<St_Para>{PARAMIO}     { /* param [in,out] command */
                         yyextra->token.name = "param";
                         QCString s(yytext);
                         bool isIn  = s.find("in")!=-1;
                         bool isOut = s.find("out")!=-1;
                         if (isIn)
                         {
                           if (isOut)
                           {
                             yyextra->token.paramDir=TokenInfo::InOut;
                           }
                           else
                           {
                             yyextra->token.paramDir=TokenInfo::In;
                           }
                         }
                         else if (isOut)
                         {
                           yyextra->token.paramDir=TokenInfo::Out;
                         }
                         else
                         {
                           yyextra->token.paramDir=TokenInfo::Unspecified;
                         }
                         return Token::char_to_command(yytext[0]);
                       }
<St_Para>{URLPROTOCOL}{URLMASK}/[,\.] { // URL, or URL.
                         yyextra->token.name=yytext;
                         yyextra->token.isEMailAddr=FALSE;
                         return Token::make_TK_URL();
                       }
<St_Para>{URLPROTOCOL}{URLMASK} { // URL
                         yyextra->token.name=yytext;
                         yyextra->token.isEMailAddr=FALSE;
                         return Token::make_TK_URL();
                       }
<St_Para>"<"{URLPROTOCOL}{URLMASK}">" { // URL
                         yyextra->token.name=yytext;
                         yyextra->token.name = yyextra->token.name.mid(1,yyextra->token.name.length()-2);
                         yyextra->token.isEMailAddr=FALSE;
                         return Token::make_TK_URL();
                       }
<St_Para>{MAILADDR}    { // Mail address
                         yyextra->token.name=yytext;
                         yyextra->token.name.stripPrefix("mailto:");
                         yyextra->token.isEMailAddr=TRUE;
                         return Token::make_TK_URL();
                       }
<St_Para>"<"{MAILADDR}">" { // Mail address
                         yyextra->token.name=yytext;
                         yyextra->token.name = yyextra->token.name.mid(1,yyextra->token.name.length()-2);
                         yyextra->token.name.stripPrefix("mailto:");
                         yyextra->token.isEMailAddr=TRUE;
                         return Token::make_TK_URL();
                       }
<St_Para>"<"{MAILADDR2}">" { // anti spam mail address
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_Para>{RCSID} { /* RCS tag */
                         QCString tagName(yytext+1);
                         int index=tagName.find(':');
                         if (index<0) index=0; // should never happen
                         yyextra->token.name = tagName.left(index);
                         int text_begin = index+2;
                         int text_end = static_cast<int>(tagName.length())-1;
                         if (tagName[text_begin-1]==':') /* check for Subversion fixed-length keyword */
                         {
                           ++text_begin;
                           if (tagName[text_end-1]=='#')
                           {
                             --text_end;
                           }
                         }
                         yyextra->token.text = tagName.mid(text_begin,text_end-text_begin);
                         return Token::make_TK_RCSTAG();
                       }
<St_Para,St_HtmlOnly,St_ManOnly,St_LatexOnly,St_RtfOnly,St_XmlOnly,St_DbOnly>"$("{ID}")"   | /* environment variable */
<St_Para,St_HtmlOnly,St_ManOnly,St_LatexOnly,St_RtfOnly,St_XmlOnly,St_DbOnly>"$("{ID}"("{ID}"))"   { /* environment variable */
                         QCString name(&yytext[2]);
                         name = name.left(static_cast<int>(name.length())-1);
                         QCString value = Portable::getenv(name);
                         for (int i=static_cast<int>(value.length())-1;i>=0;i--) unput(value.at(i));
                       }
<St_Para>{HTMLTAG}     { /* html tag */
                         lineCount(yytext,yyleng);
                         handleHtmlTag(yyscanner,yytext);
                         return Token::make_TK_HTMLTAG();
                       }
<St_Para,St_Text>"&"{ID}";" { /* special symbol */
                         yyextra->token.name = yytext;
                         return Token::make_TK_SYMBOL();
                       }

  /********* patterns for linkable words ******************/

<St_Para>{ID}/"<"{HTMLKEYW}">" { /* this rule is to prevent opening html
                                  * tag to be recognized as a templated classes
                                  */
                         yyextra->token.name = yytext;
                         return Token::make_TK_LNKWORD();
                        }
<St_Para>{LNKWORD1}/"<tt>" { // prevent <tt> html tag to be parsed as template arguments
                         yyextra->token.name = yytext;
                         return Token::make_TK_LNKWORD();
                       }
<St_Para>{LNKWORD1}/"<br>"           | // prevent <br> html tag to be parsed as template arguments
<St_Para>{LNKWORD1}                  |
<St_Para>{LNKWORD1}{FUNCARG}         |
<St_Para>{LNKWORD2}                  |
<St_Para>{LNKWORD3}    {
                         yyextra->token.name = yytext;
                         return Token::make_TK_LNKWORD();
                       }
<St_Para>{LNKWORD1}{FUNCARG}{CVSPEC}[^a-z_A-Z0-9] {
                         yyextra->token.name = yytext;
                         yyextra->token.name = yyextra->token.name.left(yyextra->token.name.length()-1);
                         unput(yytext[(int)yyleng-1]);
                         return Token::make_TK_LNKWORD();
                       }
  /********* patterns for normal words ******************/

<St_Para,St_Text>[\-+0-9] |
<St_Para,St_Text>{WORD1} |
<St_Para,St_Text>{WORD2} { /* function call */
                         if (QCString(yytext).find("\\ilinebr")!=-1) REJECT; // see issue #8311
                         lineCount(yytext,yyleng);
                         if (yytext[0]=='%') // strip % if present
                           yyextra->token.name = &yytext[1];
                         else
                           yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_Text>({ID}".")+{ID} {
                          yyextra->token.name = yytext;
                          return Token::make_TK_WORD();
                        }
<St_Para,St_Text>"operator"/{BLANK}*"<"[a-zA-Z_0-9]+">" { // Special case: word "operator" followed by a HTML command
                                                          // avoid interpretation as "operator <"
                           yyextra->token.name = yytext;
                           return Token::make_TK_WORD();
                         }

  /*******************************************************/

<St_Para,St_Text>{BLANK}+      |
<St_Para,St_Text>{BLANK}*\n{BLANK}* { /* white space */
                         lineCount(yytext,yyleng);
                         yyextra->token.chars=yytext;
                         return Token::make_TK_WHITESPACE();
                       }
<St_Text>[\\@<>&$#%~]  {
                         yyextra->token.name = yytext;
                         return Token::char_to_command(yytext[0]);
                       }
<St_Para>({BLANK}*\n)+{BLANK}*\n/{LISTITEM} { /* skip trailing paragraph followed by new list item */
                         if (yyextra->insidePre || yyextra->autoListLevel==0)
                         {
                           REJECT;
                         }
                         lineCount(yytext,yyleng);
                       }
<St_Para>({BLANK}*\n)+{BLANK}*\n/{CLISTITEM} { /* skip trailing paragraph followed by new checkbox item */
                         if (yyextra->insidePre || yyextra->autoListLevel==0)
                         {
                           REJECT;
                         }
                       }
<St_Para>({BLANK}*\n)+{BLANK}*\n/{MLISTITEM} { /* skip trailing paragraph followed by new list item */
                         if (!yyextra->markdownSupport || yyextra->insidePre || yyextra->autoListLevel==0)
                         {
                           REJECT;
                         }
                         lineCount(yytext,yyleng);
                       }
<St_Para>({BLANK}*\n)+{BLANK}*\n/{OLISTITEM} { /* skip trailing paragraph followed by new list item */
                         if (!yyextra->markdownSupport || yyextra->insidePre || yyextra->autoListLevel==0)
                         {
                           REJECT;
                         }
                         lineCount(yytext,yyleng);
                       }
<St_Para,St_Param>({BLANK}*(\n|"\\ilinebr"))+{BLANK}*(\n|"\\ilinebr"){BLANK}*/" \\ifile" | // we don't want to count the space before \ifile
<St_Para,St_Param>({BLANK}*(\n|"\\ilinebr"))+{BLANK}*(\n|"\\ilinebr"){BLANK}* {
                         lineCount(yytext,yyleng);
                         if (yyextra->insidePre)
                         {
                           yyextra->token.chars=yytext;
                           return Token::make_TK_WHITESPACE();
                         }
                         else
                         {
                           yyextra->token.indent=computeIndent(yytext,yyleng);
                           int i;
                           // put back the indentation (needed for list items)
                           //printf("token.indent=%d\n",yyextra->token.indent);
                           for (i=0;i<yyextra->token.indent;i++)
                           {
                             unput(' ');
                           }
                           // tell flex that after putting the last indent
                           // back we are at the beginning of the line
                           YY_CURRENT_BUFFER->yy_at_bol=1;
                           // start of a new paragraph
                           return Token::make_TK_NEWPARA();
                         }
                       }
<St_CodeOpt>{BLANK}*"{"(".")?{LABELID}"}" {
                         yyextra->token.name = yytext;
                         int i=yyextra->token.name.find('{'); /* } to keep vi happy */
                         yyextra->token.name = yyextra->token.name.mid(i+1,yyextra->token.name.length()-i-2);
                         BEGIN(St_Code);
                       }
<St_iCodeOpt>{BLANK}*"{"(".")?{LABELID}"}" {
                         yyextra->token.name = yytext;
                         int i=yyextra->token.name.find('{'); /* } to keep vi happy */
                         yyextra->token.name = yyextra->token.name.mid(i+1,yyextra->token.name.length()-i-2);
                         BEGIN(St_iCode);
                       }
<St_CodeOpt>"\\ilinebr" |
<St_CodeOpt>\n         |
<St_CodeOpt>.          {
                         unput_string(yytext,yyleng);
                         BEGIN(St_Code);
                       }
<St_iCodeOpt>"\\ilinebr" |
<St_iCodeOpt>\n         |
<St_iCodeOpt>.          {
                         unput_string(yytext,yyleng);
                         BEGIN(St_iCode);
                       }
<St_Code>{WS}*{CMD}"endcode" {
                         lineCount(yytext,yyleng);
                         return Token::make_RetVal_OK();
                       }
<St_iCode>{WS}*{CMD}"endicode" {
                         lineCount(yytext,yyleng);
                         return Token::make_RetVal_OK();
                       }
<St_XmlCode>{WS}*"</code>" {
                         lineCount(yytext,yyleng);
                         return Token::make_RetVal_OK();
                       }
<St_Code,St_iCode,St_XmlCode>[^\\@\n<]+  |
<St_Code,St_iCode,St_XmlCode>\n          |
<St_Code,St_iCode,St_XmlCode>.           {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_HtmlOnlyOption>" [block]" { // the space is added in commentscan.l
                         yyextra->token.name="block";
                         BEGIN(St_HtmlOnly);
                        }
<St_HtmlOnlyOption>.|\n {
                         unput(*yytext);
                         BEGIN(St_HtmlOnly);
                        }
<St_HtmlOnlyOption>"\\ilinebr" {
                         unput_string(yytext,yyleng);
                         BEGIN(St_HtmlOnly);
                        }
<St_HtmlOnly>{CMD}"endhtmlonly" {
                         return Token::make_RetVal_OK();
                       }
<St_HtmlOnly>[^\\@\n$]+    |
<St_HtmlOnly>\n            |
<St_HtmlOnly>.             {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_ManOnly>{CMD}"endmanonly" {
                         return Token::make_RetVal_OK();
                       }
<St_ManOnly>[^\\@\n$]+    |
<St_ManOnly>\n            |
<St_ManOnly>.             {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_RtfOnly>{CMD}"endrtfonly" {
                         return Token::make_RetVal_OK();
                       }
<St_RtfOnly>[^\\@\n$]+    |
<St_RtfOnly>\n            |
<St_RtfOnly>.             {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_LatexOnly>{CMD}"endlatexonly" {
                         return Token::make_RetVal_OK();
                       }
<St_LatexOnly>[^\\@\n]+     |
<St_LatexOnly>\n            |
<St_LatexOnly>.             {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_XmlOnly>{CMD}"endxmlonly" {
                         return Token::make_RetVal_OK();
                       }
<St_XmlOnly>[^\\@\n]+  |
<St_XmlOnly>\n         |
<St_XmlOnly>.          {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_DbOnly>{CMD}"enddocbookonly" {
                         return Token::make_RetVal_OK();
                       }
<St_DbOnly>[^\\@\n]+  |
<St_DbOnly>\n         |
<St_DbOnly>.          {
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_Verbatim>{CMD}"endverbatim" {
                         yyextra->token.verb = yyextra->token.verb.stripLeadingAndTrailingEmptyLines();
                         return Token::make_RetVal_OK();
                       }
<St_ILiteral>{CMD}"endiliteral " { // note extra space as this is artificially added
                         // remove spaces that have been added
                         yyextra->token.verb = yyextra->token.verb.mid(1,yyextra->token.verb.length()-2);
                         return Token::make_RetVal_OK();
                       }
<St_iVerbatim>{CMD}"endiverbatim" {
                         yyextra->token.verb = yyextra->token.verb.stripLeadingAndTrailingEmptyLines();
                         return Token::make_RetVal_OK();
                       }
<St_Verbatim,St_iVerbatim,St_ILiteral>[^\\@\n]+ |
<St_Verbatim,St_iVerbatim,St_ILiteral>\n        |
<St_Verbatim,St_iVerbatim,St_ILiteral>.         { /* Verbatim text */
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_ILiteralOpt>{BLANK}*"{"[a-zA-Z_,:0-9\. ]*"}" { // option(s) present
                         yyextra->token.verb = QCString(yytext).stripWhiteSpace();
                         return Token::make_RetVal_OK();
                       }
<St_ILiteralOpt>"\\ilinebr" |
<St_ILiteralOpt>"\n"   |
<St_ILiteralOpt>.      {
                         yyextra->token.sectionId = "";
                         unput_string(yytext,yyleng);
                         return Token::make_RetVal_OK();
                       }
<St_Dot>{CMD}"enddot"  {
                         return Token::make_RetVal_OK();
                       }
<St_Dot>[^\\@\n]+      |
<St_Dot>\n             |
<St_Dot>.              { /* dot text */
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_Msc>{CMD}("endmsc")  {
                         return Token::make_RetVal_OK();
                       }
<St_Msc>[^\\@\n]+      |
<St_Msc>\n             |
<St_Msc>.              { /* msc text */
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_PlantUMLOpt>{BLANK}*"{"[a-zA-Z_,:0-9\. ]*"}" { // case 1: options present
                         yyextra->token.sectionId = QCString(yytext).stripWhiteSpace();
                         return Token::make_RetVal_OK();
                       }
<St_PlantUMLOpt>{BLANK}*{FILEMASK}{BLANK}+/{ID}"=" { // case 2: plain file name specified followed by an attribute
                         yyextra->token.sectionId = QCString(yytext).stripWhiteSpace();
                         return Token::make_RetVal_OK();
                       }
<St_PlantUMLOpt>{BLANK}*{FILEMASK}{BLANK}+/"\"" { // case 3: plain file name specified followed by a quoted title
                         yyextra->token.sectionId = QCString(yytext).stripWhiteSpace();
                         return Token::make_RetVal_OK();
                       }
<St_PlantUMLOpt>{BLANK}*{FILEMASK}{BLANKopt}/\n { // case 4: plain file name specified without title or attributes
                         yyextra->token.sectionId = QCString(yytext).stripWhiteSpace();
                         return Token::make_RetVal_OK();
                       }
<St_PlantUMLOpt>{BLANK}*{FILEMASK}{BLANKopt}/"\\ilinebr" { // case 5: plain file name specified without title or attributes
                         yyextra->token.sectionId = QCString(yytext).stripWhiteSpace();
                         return Token::make_RetVal_OK();
                       }
<St_PlantUMLOpt>"\\ilinebr" |
<St_PlantUMLOpt>"\n"   |
<St_PlantUMLOpt>.      {
                         yyextra->token.sectionId = "";
                         unput_string(yytext,yyleng);
                         return Token::make_RetVal_OK();
                       }
<St_PlantUML>{CMD}"enduml"  {
                         return Token::make_RetVal_OK();
                       }
<St_PlantUML>[^\\@\n]+ |
<St_PlantUML>\n        |
<St_PlantUML>.         { /* plantuml text */
                         lineCount(yytext,yyleng);
                         yyextra->token.verb+=yytext;
                       }
<St_Title>"\""         { // quoted title
                         BEGIN(St_TitleQ);
                       }
<St_Title>[ \t]+       {
                         yyextra->token.chars=yytext;
                         return Token::make_TK_WHITESPACE();
                       }
<St_Title>.            { // non-quoted title
                         unput(*yytext);
                         BEGIN(St_TitleN);
                       }
<St_Title>\n           {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_Title>"\\ilinebr"  {
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_TitleN>"&"{ID}";"  { /* symbol */
                         yyextra->token.name = yytext;
                         return Token::make_TK_SYMBOL();
                       }
<St_TitleN>{HTMLTAG}   {
                         yyextra->token.name = yytext;
                         handleHtmlTag(yyscanner,yytext);
                         return Token::make_TK_HTMLTAG();
                       }
<St_TitleN>\n          { /* new line => end of title */
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_TitleN>"\\ilinebr" { /* new line => end of title */
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_TitleN>{SPCMD1}    |
<St_TitleN>{SPCMD2}    { /* special command */
                         yyextra->token.name = yytext+1;
                         yyextra->token.paramDir=TokenInfo::Unspecified;
                         return Token::char_to_command(yytext[0]);
                       }
<St_TitleN>{ID}"="     { /* attribute */
                         if (yytext[0]=='%') // strip % if present
                           yyextra->token.name = &yytext[1];
                         else
                           yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_TitleN>[\-+0-9]    |
<St_TitleN>{WORD1}     |
<St_TitleN>{WORD2}     { /* word */
                         if (QCString(yytext).find("\\ilinebr")!=-1) REJECT; // see issue #8311
                         lineCount(yytext,yyleng);
                         if (yytext[0]=='%') // strip % if present
                           yyextra->token.name = &yytext[1];
                         else
                           yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_TitleN>[ \t]+      {
                         yyextra->token.chars=yytext;
                         return Token::make_TK_WHITESPACE();
                       }
<St_TitleQ>"&"{ID}";"  { /* symbol */
                         yyextra->token.name = yytext;
                         return Token::make_TK_SYMBOL();
                       }
<St_TitleQ>(\n|"\\ilinebr") { /* new line => end of title */
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_TitleQ>{SPCMD1}    |
<St_TitleQ>{SPCMD2}    { /* special command */
                         yyextra->token.name = yytext+1;
                         yyextra->token.paramDir=TokenInfo::Unspecified;
                         return Token::char_to_command(yytext[0]);
                       }
<St_TitleQ>{WORD1NQ}   |
<St_TitleQ>{WORD2NQ}   { /* word */
                         yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_TitleQ>[ \t]+      {
                         yyextra->token.chars=yytext;
                         return Token::make_TK_WHITESPACE();
                       }
<St_TitleQ>"\""        { /* closing quote => end of title */
                         BEGIN(St_TitleA);
                         return Token::make_TK_NONE();
                       }
<St_TitleA>{BLANK}*{ID}{BLANK}*"="{BLANK}* { // title attribute
                         yyextra->token.name = yytext;
                         int pos = yyextra->token.name.find('=');
                         if (pos<0) pos=0; // should never happen
                         yyextra->token.name = yyextra->token.name.left(pos).stripWhiteSpace();
                         BEGIN(St_TitleV);
                       }
<St_TitleV>[^ \t\r\n]+ { // attribute value
                         lineCount(yytext,yyleng);
                         yyextra->token.chars = yytext;
                         BEGIN(St_TitleN);
                         return Token::make_TK_WORD();
                       }
<St_TitleV,St_TitleA>. {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_TitleV,St_TitleA>(\n|"\\ilinebr") {
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }

<St_Anchor>{LABELID}{WS}? { // anchor
                         lineCount(yytext,yyleng);
                         yyextra->token.name = QCString(yytext).stripWhiteSpace();
                         return Token::make_TK_WORD();
                       }
<St_Anchor>.           {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_Cite>{CITEID}      { // label to cite
                         if (yytext[0] =='"')
                         {
                           yyextra->token.name=yytext+1;
                           yyextra->token.name=yyextra->token.name.left(static_cast<uint32_t>(yyleng)-2);
                         }
                         else
                         {
                           yyextra->token.name=yytext;
                         }
                         return Token::make_TK_WORD();
                       }
<St_Cite>{BLANK}       { // white space
                         unput(' ');
                         return Token::make_TK_NONE();
                       }
<St_Cite>(\n|"\\ilinebr")  { // new line
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_Cite>.             { // any other character
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_DoxyConfig>{DOXYCFG} { // config option
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_DoxyConfig>{BLANK} { // white space
                         unput(' ');
                         return Token::make_TK_NONE();
                       }
<St_DoxyConfig>(\n|"\\ilinebr")  { // new line
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_DoxyConfig>.      { // any other character
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_Ref>{REFWORD_NOCV}/{BLANK}("const")[a-z_A-Z0-9] { // see bug776988
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_Ref>{REFWORD_NOCV}/{BLANK}("volatile")[a-z_A-Z0-9] { // see bug776988
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_Ref>{REFWORD}      { // label to refer to
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_Ref>{BLANK}        { // white space
                         unput(' ');
                         return Token::make_TK_NONE();
                       }
<St_Ref>{WS}+"\""{WS}* { // white space following by quoted string
                         lineCount(yytext,yyleng);
                         BEGIN(St_Ref2);
                       }
<St_Ref>(\n|"\\ilinebr") { // new line
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_Ref>"\""[^"\n]+"\"" { // quoted first argument -> return without quotes
                         yyextra->token.name=QCString(yytext).mid(1,yyleng-2);
                         return Token::make_TK_WORD();
                       }
<St_Ref>.              { // any other character
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_IntRef>[A-Z_a-z0-9.:/#\-\+\(\)]+ {
                         yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_IntRef>{BLANK}+"\"" {
                         BEGIN(St_Ref2);
                       }
<St_SetScope>({SCOPEMASK}|{ANONNS}){BLANK}|{FILEMASK} {
                         yyextra->token.name = yytext;
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         return Token::make_TK_WORD();
                       }
<St_SetScope>{SCOPEMASK}"<" {
                         yyextra->token.name = yytext;
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         yyextra->sharpCount=1;
                         BEGIN(St_SetScopeEnd);
                       }
<St_SetScope>{BLANK}   {
                       }
<St_SetScopeEnd>"<"    {
                         yyextra->token.name += yytext;
                         yyextra->sharpCount++;
                       }
<St_SetScopeEnd>">"    {
                         yyextra->token.name += yytext;
                         yyextra->sharpCount--;
                         if (yyextra->sharpCount<=0)
                         {
                           return Token::make_TK_WORD();
                         }
                       }
<St_SetScopeEnd>.      {
                         yyextra->token.name += yytext;
                       }
<St_Ref2>"&"{ID}";"    { /* symbol */
                         yyextra->token.name = yytext;
                         return Token::make_TK_SYMBOL();
                       }
<St_Ref2>"\""|\n|"\\ilinebr" { /* " or \n => end of title */
                         lineCount(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_Ref2>{HTMLTAG_STRICT}     { /* html tag */
                         lineCount(yytext,yyleng);
                         handleHtmlTag(yyscanner,yytext);
                         return Token::make_TK_HTMLTAG();
                       }
<St_Ref2>{SPCMD1}      |
<St_Ref2>{SPCMD2}      { /* special command */
                         yyextra->token.name = yytext+1;
                         yyextra->token.paramDir=TokenInfo::Unspecified;
                         return Token::char_to_command(yytext[0]);
                       }
<St_Ref2>{WORD1NQ}     |
<St_Ref2>{WORD2NQ}     {
                         /* word */
                         yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_Ref2>[ \t]+        {
                         yyextra->token.chars=yytext;
                         return Token::make_TK_WHITESPACE();
                       }
<St_XRefItem>{LABELID} {
                         yyextra->token.name=yytext;
                       }
<St_XRefItem>" "       {
                         BEGIN(St_XRefItem2);
                       }
<St_XRefItem2>[0-9]+"." {
                         QCString numStr(yytext);
                         numStr=numStr.left((int)yyleng-1);
                         yyextra->token.id=numStr.toInt();
                         return Token::make_RetVal_OK();
                       }
<St_Para,St_Title,St_Ref2>"<!--"     { /* html style comment block */
                         yyextra->commentState = YY_START;
                         BEGIN(St_Comment);
                       }
<St_Param>"\""[^\n\"]+"\"" {
                         yyextra->token.name = yytext+1;
                         yyextra->token.name = yyextra->token.name.left((int)yyleng-2);
                         return Token::make_TK_WORD();
                       }
<St_Param>({PHPTYPE}{BLANK}*("["{BLANK}*"]")*{BLANK}*"|"{BLANK}*)*{PHPTYPE}{BLANK}*("["{BLANK}*"]")*{WS}+("&")?"$"{LABELID} {
                         lineCount(yytext,yyleng);
                         QCString params(yytext);
                         int j = params.find('&');
                         int i = params.find('$');
                         if (i<0) i=0; // should never happen
                         if (j<i && j>=0) i=j;
                         QCString types = params.left(i).stripWhiteSpace();
                         yyextra->token.name = types+"#"+params.mid(i);
                         return Token::make_TK_WORD();
                       }
<St_Param>[^ \t\n,@\\]+  {
                         yyextra->token.name = yytext;
                         if (yyextra->token.name.at(static_cast<uint32_t>(yyleng)-1)==':')
                         {
                           yyextra->token.name=yyextra->token.name.left(static_cast<uint32_t>(yyleng)-1);
                         }
                         return Token::make_TK_WORD();
                       }
<St_Param>{WS}*","{WS}*  /* param separator */
<St_Param>{WS}         {
                         lineCount(yytext,yyleng);
                         yyextra->token.chars=yytext;
                         return Token::make_TK_WHITESPACE();
                       }
<St_Prefix>"\""[^\n\"]*"\"" {
                         yyextra->token.name = yytext+1;
                         yyextra->token.name = yyextra->token.name.left((int)yyleng-2);
                         return Token::make_TK_WORD();
                       }
<St_Prefix>.           {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_Options>{ID}       {
                         yyextra->token.name+=yytext;
                       }
<St_Options>{WS}*":"{WS}* {
                         lineCount(yytext,yyleng);
                         yyextra->token.name+=":";
                       }
<St_Options>{WS}*","{WS}* |
<St_Options>{WS}       { /* option separator */
                         lineCount(yytext,yyleng);
                         yyextra->token.name+=",";
                       }
<St_Options>"}"        {
                         return Token::make_TK_WORD();
                       }
<St_Block>{ID}         {
                         yyextra->token.name+=yytext;
                       }
<St_Block>"]"          {
                         return Token::make_TK_WORD();
                       }
<St_Emoji>[:0-9_a-z+-]+  {
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_Emoji>.            {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_QuotedString>"\""  {
                         yyextra->token.name="";
                         BEGIN(St_QuotedContent);
                       }
<St_QuotedString>(\n|"\\ilinebr") {
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_QuotedString>.     {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_QuotedContent>"\"" {
                         return Token::make_TK_WORD();
                       }
<St_QuotedContent>.    {
                         yyextra->token.name+=yytext;
                       }
<St_ShowDate>{WS}+{SHOWDATE} {
                         lineCount(yytext,yyleng);
                         yyextra->token.name=yytext;
                         return Token::make_TK_WORD();
                       }
<St_ShowDate>(\n|"\\ilinebr") {
                         unput_string(yytext,yyleng);
                         return Token::make_TK_NONE();
                       }
<St_ShowDate>.         {
                         unput(*yytext);
                         return Token::make_TK_NONE();
                       }
<St_ILine>{LINENR}/[\\@\n\.]  |
<St_ILine>{LINENR}{BLANK}     {
                         bool ok = false;
                         int nr = QCString(yytext).toInt(&ok);
                         if (!ok)
                         {
                           warn(yyextra->fileName,yyextra->yyLineNr,"Invalid line number '%s' for iline command",yytext);
                         }
                         else
                         {
                           yyextra->yyLineNr = nr;
                         }
                         return Token::make_TK_WORD();
                       }
<St_ILine>.            {
                         return Token::make_TK_NONE();
                       }
<St_IFile>{BLANK}*{FILEMASK} {
                         yyextra->fileName = QCString(yytext).stripWhiteSpace();
                         return Token::make_TK_WORD();
                       }
<St_IFile>{BLANK}*"\""[^\n\"]+"\"" {
                         QCString text(yytext);
                         text = text.stripWhiteSpace();
                         yyextra->fileName = text.mid(1,text.length()-2);
                         return Token::make_TK_WORD();
                       }
<St_File>{FILEMASK}    {
                         yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_File>"\""[^\n\"]+"\"" {
                         QCString text(yytext);
                         yyextra->token.name = text.mid(1,text.length()-2);
                         return Token::make_TK_WORD();
                       }
<St_Pattern>[^\\\r\n]+   {
                         yyextra->token.name += yytext;
                       }
<St_Pattern>"\\ilinebr" {
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         return Token::make_TK_WORD();
                       }
<St_Pattern>\n         {
                         lineCount(yytext,yyleng);
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         return Token::make_TK_WORD();
                       }
<St_Pattern>.          {
                         yyextra->token.name += yytext;
                       }
<St_Link>{LINKMASK}|{REFWORD}    {
                         yyextra->token.name = yytext;
                         return Token::make_TK_WORD();
                       }
<St_Comment>"-->"      { /* end of html comment */
                         BEGIN(yyextra->commentState);
                       }
<St_Comment>[^-]+      /* inside html comment */
<St_Comment>.          /* inside html comment */

     /* State for skipping title (all chars until the end of the line) */

<St_SkipTitle>.
<St_SkipTitle>(\n|"\\ilinebr") {
                         if (*yytext == '\n') unput('\n');
                         return Token::make_TK_NONE();
                         }

     /* State for the pass used to find the anchors and sections */

<St_Sections>[^\n@\\<]+
<St_Sections>{CMD}("<"|{CMD})
<St_Sections>"<"{CAPTION}({WS}+{ATTRIB})*">" {
                                      lineCount(yytext,yyleng);
                                      QCString tag(yytext);
                                      int s=tag.find("id=");
                                      if (s!=-1) // command has id attribute
                                      {
                                        char c=tag[s+3];
                                        if (c=='\'' || c=='"') // valid start
                                        {
                                          int e=tag.find(c,s+4);
                                          if (e!=-1) // found matching end
                                          {
                                            yyextra->secType = SectionType::Table;
                                            yyextra->secLabel=tag.mid(s+4,e-s-4); // extract id
                                            processSection(yyscanner);
                                          }
                                        }
                                      }
                                    }
<St_Sections>{CMD}"anchor"{BLANK}+  {
                                      yyextra->secType = SectionType::Anchor;
                                      BEGIN(St_SecLabel1);
                                    }
<St_Sections>{CMD}"ianchor"{BLANK}+  {
                                      yyextra->secType = SectionType::Anchor;
                                      BEGIN(St_SecLabel1);
                                    }
<St_Sections>{CMD}"section"{BLANK}+ {
                                      yyextra->secType = SectionType::Section;
                                      BEGIN(St_SecLabel2);
                                    }
<St_Sections>{CMD}"subsection"{BLANK}+ {
                                      yyextra->secType = SectionType::Subsection;
                                      BEGIN(St_SecLabel2);
                                    }
<St_Sections>{CMD}"subsubsection"{BLANK}+ {
                                      yyextra->secType = SectionType::Subsubsection;
                                      BEGIN(St_SecLabel2);
                                    }
<St_Sections>{CMD}"paragraph"{BLANK}+ {
                                      yyextra->secType = SectionType::Paragraph;
                                      BEGIN(St_SecLabel2);
                                    }
<St_Sections>{CMD}"subparagraph"{BLANK}+ {
                                      yyextra->secType = SectionType::Subparagraph;
                                      BEGIN(St_SecLabel2);
                                    }
<St_Sections>{CMD}"subsubparagraph"{BLANK}+ {
                                      yyextra->secType = SectionType::Subsubparagraph;
                                      BEGIN(St_SecLabel2);
                                    }
<St_Sections>{CMD}"verbatim"/[^a-z_A-Z0-9]  {
                                      yyextra->endMarker="endverbatim";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"iverbatim"/[^a-z_A-Z0-9]  {
                                      yyextra->endMarker="endiverbatim";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"iliteral"/[^a-z_A-Z0-9]  {
                                      yyextra->endMarker="endiliteral";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"dot"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="enddot";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"msc"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endmsc";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"startuml"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="enduml";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"htmlonly"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endhtmlonly";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"latexonly"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endlatexonly";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"manonly"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endmanonly";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"rtfonly"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endrtfonly";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"xmlonly"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endxmlonly";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"docbookonly"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="enddocbookonly";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"code"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endcode";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"icode"/[^a-z_A-Z0-9] {
                                      yyextra->endMarker="endicode";
                                      BEGIN(St_SecSkip);
                                    }
<St_Sections>"<!--"                 {
                                      yyextra->endMarker="-->";
                                      BEGIN(St_SecSkip);
                                    }
<St_SecSkip>{CMD}{ID}               {
                                      if (yyextra->endMarker==yytext+1)
                                      {
                                        BEGIN(St_Sections);
                                      }
                                    }
<St_SecSkip>"-->"                   {
                                      if (yyextra->endMarker==yytext)
                                      {
                                        BEGIN(St_Sections);
                                      }
                                    }
<St_SecSkip>[^a-z_A-Z0-9\-\\\@]+
<St_SecSkip>.
<St_SecSkip>(\n|"\\ilinebr")
<St_Sections>.
<St_Sections>(\n|"\\ilinebr")
<St_SecLabel1>{LABELID} {
                         lineCount(yytext,yyleng);
                         yyextra->secLabel = yytext;
                         processSection(yyscanner);
                         BEGIN(St_Sections);
                       }
<St_SecLabel2>{LABELID}{BLANK}+ |
<St_SecLabel2>{LABELID}         {
                         yyextra->secLabel = yytext;
                         yyextra->secLabel = yyextra->secLabel.stripWhiteSpace();
                         BEGIN(St_SecTitle);
                       }
<St_SecTitle>[^\n]+    |
<St_SecTitle>[^\n]*\n  {
                         lineCount(yytext,yyleng);
                         yyextra->secTitle = yytext;
                         yyextra->secTitle = yyextra->secTitle.stripWhiteSpace();
                         if (yyextra->secTitle.endsWith("\\ilinebr"))
                         {
                           yyextra->secTitle.left(yyextra->secTitle.length()-8);
                         }
                         processSection(yyscanner);
                         BEGIN(St_Sections);
                       }
<St_SecTitle,St_SecLabel1,St_SecLabel2>. {
                         warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected character '%s' while looking for section label or title",yytext);
                       }

<St_Snippet>[^\\\n]+   {
                         yyextra->token.name += yytext;
                       }
<St_Snippet>"\\"       {
                         yyextra->token.name += yytext;
                       }
<St_Snippet>(\n|"\\ilinebr")  {
                         unput_string(yytext,yyleng);
                         yyextra->token.name = yyextra->token.name.stripWhiteSpace();
                         return Token::make_TK_WORD();
                       }

     /* Generic rules that work for all states */
<*>\n                  {
                         lineCount(yytext,yyleng);
                         warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected new line character");
                       }
<*>"\\ilinebr"         {
                       }
<*>[\\@<>&$#%~"=]      { /* unescaped special character */
                         //warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected character '%s', assuming command \\%s was meant.",yytext,yytext);
                         yyextra->token.name = yytext;
                         return Token::char_to_command(yytext[0]);
                       }
<*>.                   {
                         warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected character '%s'",yytext);
                       }
%%

//--------------------------------------------------------------------------

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  const char *p = yyextra->inputString + yyextra->inputPos;
  while ( c < max_size && *p ) { *buf++ = *p++; c++; }
  yyextra->inputPos+=c;
  return c;
}

static void processSection(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("%s: found section/anchor with name '%s'\n",qPrint(g_fileName),qPrint(g_secLabel));
  QCString file;
  if (yyextra->definition)
  {
    file = yyextra->definition->getOutputFileBase();
  }
  else
  {
    warn(yyextra->fileName,yyextra->yyLineNr,"Found section/anchor %s without context",qPrint(yyextra->secLabel));
  }
  SectionInfo *si = SectionManager::instance().find(yyextra->secLabel);
  if (si)
  {
    si->setFileName(file);
    si->setType(yyextra->secType);
  }
}

static void handleHtmlTag(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  QCString tagText(text);
  yyextra->token.text = tagText;
  yyextra->token.attribs.clear();
  yyextra->token.endTag = FALSE;
  yyextra->token.emptyTag = FALSE;

  // Check for end tag
  int startNamePos=1;
  if (tagText.at(1)=='/')
  {
    yyextra->token.endTag = TRUE;
    startNamePos++;
  }

  // Parse the name portion
  int i = startNamePos;
  for (i=startNamePos; i < (int)yyleng; i++)
  {
    // Check for valid HTML/XML name chars (including namespaces)
    char c = tagText.at(i);
    if (!(isalnum(c) || c=='-' || c=='_' || c==':')) break;
  }
  yyextra->token.name = tagText.mid(startNamePos,i-startNamePos);

  // Parse the attributes. Each attribute is a name, value pair
  // The result is stored in yyextra->token.attribs.
  int startAttribList = i;
  while (i<(int)yyleng)
  {
    char c=tagText.at(i);
    // skip spaces
    while (i<(int)yyleng && isspace((uint8_t)c)) { c=tagText.at(++i); }
    // check for end of the tag
    if (c == '>') break;
    // Check for XML style "empty" tag.
    if (c == '/')
    {
      yyextra->token.emptyTag = TRUE;
      break;
    }
    int startName=i;
    // search for end of name
    while (i<(int)yyleng && !isspace((uint8_t)c) && c!='=' && c!= '>') { c=tagText.at(++i); }
    int endName=i;
    HtmlAttrib opt;
    opt.name  = tagText.mid(startName,endName-startName).lower();
    // skip spaces
    while (i<(int)yyleng && isspace((uint8_t)c)) { c=tagText.at(++i); }
    if (tagText.at(i)=='=') // option has value
    {
      int startAttrib=0, endAttrib=0;
      c=tagText.at(++i);
      // skip spaces
      while (i<(int)yyleng && isspace((uint8_t)c)) { c=tagText.at(++i); }
      if (tagText.at(i)=='\'') // option '...'
      {
        c=tagText.at(++i);
        startAttrib=i;

        // search for matching quote
        while (i<(int)yyleng && c!='\'') { c=tagText.at(++i); }
        endAttrib=i;
        if (i<(int)yyleng) { c=tagText.at(++i);}
      }
      else if (tagText.at(i)=='"') // option "..."
      {
        c=tagText.at(++i);
        startAttrib=i;
        // search for matching quote
        while (i<(int)yyleng && c!='"') { c=tagText.at(++i); }
        endAttrib=i;
        if (i<(int)yyleng) { c=tagText.at(++i);}
      }
      else // value without any quotes
      {
        startAttrib=i;
        // search for separator or end symbol
        while (i<(int)yyleng && !isspace((uint8_t)c) && c!='>') { c=tagText.at(++i); }
        endAttrib=i;
        if (i<(int)yyleng) { c=tagText.at(++i);}
      }
      opt.value  = tagText.mid(startAttrib,endAttrib-startAttrib);
      if (opt.name == "align") opt.value = opt.value.lower();
      else if (opt.name == "valign")
      {
        opt.value = opt.value.lower();
        if (opt.value == "center") opt.value="middle";
      }
    }
    else // start next option
    {
    }
    //printf("=====> Adding option name=<%s> value=<%s>\n",
    //    qPrint(opt.name),qPrint(opt.value));
    yyextra->token.attribs.push_back(opt);
  }
  yyextra->token.attribsStr = tagText.mid(startAttribList,i-startAttribList);
}

struct DocTokenizer::Private
{
  yyscan_t yyscanner;
  doctokenizerYY_state extra;
};


void DocTokenizer::pushContext()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("DocTokenizer::pushContext() stack=%zu\n",yyextra->lexerStack.size());
  yyextra->lexerStack.push(
      std::make_unique<DocLexerContext>(
        yyextra->token,YY_START,
        yyextra->autoListLevel,
        yyextra->inputPos,
        yyextra->inputString,
        YY_CURRENT_BUFFER));
  yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner), yyscanner);
}

bool DocTokenizer::popContext()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("DocTokenizer::popContext() stack=%zu\n",yyextra->lexerStack.size());
  if (yyextra->lexerStack.empty()) return FALSE;
  const auto &ctx = yyextra->lexerStack.top();
  yyextra->autoListLevel = ctx->autoListLevel;
  yyextra->inputPos      = ctx->inputPos;
  yyextra->inputString   = ctx->inputString;
  yyextra->token         = ctx->token;

  yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);
  yy_switch_to_buffer(ctx->state, yyscanner);

  BEGIN(ctx->rule);
  yyextra->lexerStack.pop();
  return TRUE;
}


DocTokenizer::DocTokenizer() : p(std::make_unique<Private>())
{
  //printf("%p:DocTokenizer::DocTokenizer()\n",(void*)this);
  doctokenizerYYlex_init_extra(&p->extra,&p->yyscanner);
#ifdef FLEX_DEBUG
  doctokenizerYYset_debug(Debug::isFlagSet(Debug::Lex_doctokenizer)?1:0,p->yyscanner);
#endif
}

DocTokenizer::~DocTokenizer()
{
  //printf("%p:DocTokenizer::~DocTokenizer()\n",(void*)this);
  doctokenizerYYlex_destroy(p->yyscanner);
}

Token DocTokenizer::lex()
{
  return doctokenizerYYlex(p->yyscanner);
}

void DocTokenizer::unputString(const QCString &tag)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  unput_string(tag.data(),tag.length());
}

void DocTokenizer::findSections(const QCString &input,const Definition *d,
                                const QCString &fileName)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  if (input.isEmpty()) return;
  DebugLex debugLex(Debug::Lex_doctokenizer, __FILE__, qPrint(fileName));
  yyextra->inputString = input.data();
  //printf("parsing --->'%s'<---\n",input);
  yyextra->inputPos    = 0;
  yyextra->definition  = d;
  yyextra->fileName    = fileName;
  BEGIN(St_Sections);
  yyextra->yyLineNr = 1;
  doctokenizerYYlex(yyscanner);
}

void DocTokenizer::init(const char *input,const QCString &fileName,bool markdownSupport, bool insideHtmlLink)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->autoListLevel = 0;
  yyextra->inputString = input;
  yyextra->inputPos    = 0;
  yyextra->fileName    = fileName;
  yyextra->insidePre   = FALSE;
  yyextra->markdownSupport = markdownSupport;
  yyextra->insideHtmlLink = insideHtmlLink;
  BEGIN(St_Para);
}

TokenInfo *DocTokenizer::token()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  return &yyextra->token;
}

TokenInfo *DocTokenizer::resetToken()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token = TokenInfo();
  return &yyextra->token;
}

void DocTokenizer::setStatePara()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->insideHtmlLink = false;
  BEGIN(St_Para);
}

void DocTokenizer::setStateTitle()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Title);
}

void DocTokenizer::setStateTitleAttrValue()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_TitleV);
}

void DocTokenizer::setStateCode()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  yyextra->token.name="";
  BEGIN(St_CodeOpt);
}

void DocTokenizer::setStateICode()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  yyextra->token.name="";
  BEGIN(St_iCodeOpt);
}

void DocTokenizer::setStateXmlCode()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  yyextra->token.name="";
  BEGIN(St_XmlCode);
}

void DocTokenizer::setStateHtmlOnly()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  yyextra->token.name="";
  BEGIN(St_HtmlOnlyOption);
}

void DocTokenizer::setStateManOnly()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_ManOnly);
}

void DocTokenizer::setStateRtfOnly()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_RtfOnly);
}

void DocTokenizer::setStateXmlOnly()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_XmlOnly);
}

void DocTokenizer::setStateDbOnly()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_DbOnly);
}

void DocTokenizer::setStateLatexOnly()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_LatexOnly);
}

void DocTokenizer::setStateILiteral()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_ILiteral);
}

void DocTokenizer::setStateILiteralOpt()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_ILiteralOpt);
}

void DocTokenizer::setStateVerbatim()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_Verbatim);
}

void DocTokenizer::setStateIVerbatim()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_iVerbatim);
}

void DocTokenizer::setStateDot()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_Dot);
}

void DocTokenizer::setStateMsc()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_Msc);
}

void DocTokenizer::setStatePlantUMLOpt()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  yyextra->token.sectionId="";
  BEGIN(St_PlantUMLOpt);
}

void DocTokenizer::setStatePlantUML()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.verb="";
  BEGIN(St_PlantUML);
}

void DocTokenizer::setStateParam()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Param);
}

void DocTokenizer::setStateXRefItem()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_XRefItem);
}

void DocTokenizer::setStateFile()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_File);
}

void DocTokenizer::setStateIFile()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_IFile);
}

void DocTokenizer::setStatePattern()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.name = "";
  BEGIN(St_Pattern);
}

void DocTokenizer::setStateLink()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Link);
}

void DocTokenizer::setStateCite()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Cite);
}

void DocTokenizer::setStateDoxyConfig()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_DoxyConfig);
}

void DocTokenizer::setStateRef()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Ref);
}

void DocTokenizer::setStateInternalRef()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_IntRef);
}

void DocTokenizer::setStateText()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Text);
}

void DocTokenizer::setStateSkipTitle()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_SkipTitle);
}

void DocTokenizer::setStateAnchor()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Anchor);
}

void DocTokenizer::setStatePrefix()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_Prefix);
}

void DocTokenizer::setStateSnippet()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.name="";
  BEGIN(St_Snippet);
}

void DocTokenizer::setStateSetScope()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_SetScope);
}

void DocTokenizer::setStateOptions()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.name="";
  BEGIN(St_Options);
}

void DocTokenizer::setStateBlock()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.name="";
  BEGIN(St_Block);
}

void DocTokenizer::setStateEmoji()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->token.name="";
  BEGIN(St_Emoji);
}

void DocTokenizer::setStateILine()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_ILine);
}

void DocTokenizer::setStateQuotedString()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_QuotedString);
}

void DocTokenizer::setStateShowDate()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  BEGIN(St_ShowDate);
}

void DocTokenizer::cleanup()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_delete_buffer( YY_CURRENT_BUFFER, yyscanner );
}

void DocTokenizer::setInsidePre(bool b)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->insidePre = b;
}

void DocTokenizer::pushBackHtmlTag(const QCString &tag)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString tagName = tag;
  int l = static_cast<int>(tagName.length());
  unput('>');
  for (int i=l-1;i>=0;i--)
  {
    unput(tag[i]);
  }
  unput('<');
}

void DocTokenizer::startAutoList()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->autoListLevel++;
}

void DocTokenizer::endAutoList()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->autoListLevel--;
}

void DocTokenizer::setLineNr(int lineno)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->yyLineNr = lineno;
}

int DocTokenizer::getLineNr(void)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  return yyextra->yyLineNr;
}

void DocTokenizer::pushState()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->stateStack.push(YYSTATE);
}

void DocTokenizer::popState()
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  assert(!yyextra->stateStack.empty());
  BEGIN(yyextra->stateStack.top());
  yyextra->stateStack.pop();
}

#include "doctokenizer.l.h"
