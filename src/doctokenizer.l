/******************************************************************************
 *
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

#include <ctype.h>

#include <qfile.h>
#include <qstring.h>
#include <qstack.h>
#include <qdict.h>
#include <qregexp.h>

#include "doctokenizer.h"
#include "cmdmapper.h"
#include "config.h"
#include "message.h"
#include "section.h"
#include "membergroup.h"
#include "definition.h"
#include "doxygen.h"
#include "portable.h"

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1
  
//--------------------------------------------------------------------------

// context for tokenizer phase
static int g_commentState;
TokenInfo *g_token = 0;
static int g_inputPos = 0;
static const char *g_inputString;
static QCString g_fileName;
static bool g_insidePre;

// context for section finding phase
static Definition  *g_definition;
static MemberGroup *g_memberGroup;
static QCString     g_secLabel;
static QCString     g_secTitle;
static SectionInfo::SectionType g_secType;
static QCString     g_endMarker;

struct DocLexerContext
{
  TokenInfo *token;
  int rule;
  int inputPos;
  const char *inputString;
  YY_BUFFER_STATE state;
};

static QStack<DocLexerContext> g_lexerStack;

//--------------------------------------------------------------------------

void doctokenizerYYpushContext()
{
  DocLexerContext *ctx = new DocLexerContext;
  ctx->rule = YY_START;
  ctx->token = g_token;
  ctx->inputPos = g_inputPos;
  ctx->inputString = g_inputString;
  ctx->state = YY_CURRENT_BUFFER;
  g_lexerStack.push(ctx);
  yy_switch_to_buffer(yy_create_buffer(doctokenizerYYin, YY_BUF_SIZE));
}

bool doctokenizerYYpopContext()
{
  if (g_lexerStack.isEmpty()) return FALSE;
  DocLexerContext *ctx = g_lexerStack.pop();
  g_inputPos = ctx->inputPos;
  g_inputString = ctx->inputString;
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(ctx->state);
  BEGIN(ctx->rule);
  delete ctx;
  return TRUE;
}


//--------------------------------------------------------------------------

const char *tokToString(int token)
{
  switch (token)
  {
    case 0:              return "TK_EOF";
    case TK_WORD:        return "TK_WORD";
    case TK_LNKWORD:     return "TK_LNKWORD";
    case TK_WHITESPACE:  return "TK_WHITESPACE";
    case TK_LISTITEM:    return "TK_LISTITEM";
    case TK_ENDLIST:     return "TK_ENDLIST";
    case TK_COMMAND:     return "TK_COMMAND";
    case TK_HTMLTAG:     return "TK_HTMLTAG";
    case TK_SYMBOL:      return "TK_SYMBOL";
    case TK_NEWPARA:     return "TK_NEWPARA";
    case TK_RCSTAG:      return "TK_RCSTAG";
    case TK_URL:         return "TK_URL";
  }
  return "ERROR";
}

static int computeIndent(const char *str,int length)
{
  int i;
  int indent=0;
  static int tabSize=Config_getInt("TAB_SIZE");
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
    else
    {
      indent++;
    }
  }
  return indent;
}

//--------------------------------------------------------------------------

static void processSection()
{
  //printf("%s: found section/anchor with name `%s'\n",g_fileName.data(),g_secLabel.data());
  QCString file;
  if (g_memberGroup)
  {
    file = g_memberGroup->parent()->getOutputFileBase();
  }
  else if (g_definition)
  {
    file = g_definition->getOutputFileBase();
  }
  else
  {
    warn(g_fileName,yylineno,"Found section/anchor %s without context\n",g_secLabel.data()); 
  }
  SectionInfo *si=0;
  if ((si=Doxygen::sectionDict->find(g_secLabel)))
  {
    si->fileName = file;
    //si = new SectionInfo(file,g_secLabel,g_secTitle,g_secType);
    //Doxygen::sectionDict.insert(g_secLabel,si);
  }
}

static void handleHtmlTag()
{
  QCString tagText=yytext;
  g_token->attribs.clear();
  g_token->endTag = FALSE;
  g_token->emptyTag = FALSE;
  
  // Check for end tag
  int startNamePos=1;
  if (tagText.at(1)=='/') 
  {
    g_token->endTag = TRUE;
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
  g_token->name = tagText.mid(startNamePos,i-startNamePos);

  // Parse the attributes. Each attribute is a name, value pair
  // The result is stored in g_token->attribs.
  int startName,endName,startAttrib,endAttrib;
  while (i<(int)yyleng)
  {
    char c=tagText.at(i);
    // skip spaces
    while (i<(int)yyleng && isspace(c)) { c=tagText.at(++i); }
    // check for end of the tag
    if (c == '>') break;
    // Check for XML style "empty" tag.
    if (c == '/') 
    {
      g_token->emptyTag = TRUE;
      break;
    }
    startName=i;
    // search for end of name
    while (i<(int)yyleng && !isspace(c) && c!='=') { c=tagText.at(++i); }
    endName=i;
    HtmlAttrib opt;
    opt.name  = tagText.mid(startName,endName-startName).lower(); 
    // skip spaces
    while (i<(int)yyleng && isspace(c)) { c=tagText.at(++i); } 
    if (tagText.at(i)=='=') // option has value
    {
      c=tagText.at(++i);
      // skip spaces
      while (i<(int)yyleng && isspace(c)) { c=tagText.at(++i); } 
      if (tagText.at(i)=='\'') // option '...'
      {
        c=tagText.at(++i);
	startAttrib=i;
	
	// search for matching quote 
        while (i<(int)yyleng && c!='\'') { c=tagText.at(++i); } 
	endAttrib=i;
        if (i<(int)yyleng) c=tagText.at(++i);
      }
      else if (tagText.at(i)=='"') // option "..."
      {
        c=tagText.at(++i);
	startAttrib=i;
	// search for matching quote 
        while (i<(int)yyleng && c!='"') { c=tagText.at(++i); } 
	endAttrib=i;
        if (i<(int)yyleng) c=tagText.at(++i);
      }
      else // value without any quotes
      {
	startAttrib=i;
	// search for separator or end symbol
        while (i<(int)yyleng && !isspace(c) && c!='>') { c=tagText.at(++i); } 
	endAttrib=i;
	if (i<(int)yyleng) c=tagText.at(++i);
      }
      opt.value  = tagText.mid(startAttrib,endAttrib-startAttrib); 
    }
    else // start next option
    {
    }
    //printf("=====> Adding option name=<%s> value=<%s>\n",
    //    opt.name.data(),opt.value.data());
    g_token->attribs.append(&opt);
  }
}
  
static QCString stripEmptyLines(const QCString &s)
{
  if (s.isEmpty()) return QCString();
  int end=s.length();
  int start=0,p=0;
  // skip leading empty lines
  for (;;)
  {
    int c;
    while ((c=s[p]) && (c==' ' || c=='\t')) p++;
    if (s[p]=='\n') 
    {
      start=++p; 
    }
    else 
    {
      break;
    }
  }
  // skip trailing empty lines
  p=end-1;
  if (p>=start && s.at(p)=='\n') p--;
  while (p>=start)
  {
    int c;
    while ((c=s[p]) && (c==' ' || c=='\t')) p--;
    if (s[p]=='\n') 
    {
      end=p;
    }
    else
    {
      break;
    }
    p--;
  }
  //printf("stripEmptyLines(%d-%d)\n",start,end);
  return s.mid(start,end-start);
}

//--------------------------------------------------------------------------

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  const char *src=g_inputString+g_inputPos;
  while ( c < max_size && *src ) *buf++ = *src++, c++;
  g_inputPos+=c;
  return c;
}

//--------------------------------------------------------------------------

%}

CMD       ("\\"|"@")
WS        [ \t\r\n]
NONWS     [^ \t\r\n]
BLANK     [ \t\r]
ID        "$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
LABELID   [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*
PHPTYPE   [\\:a-z_A-Z0-9\x80-\xFF\-]+
CITEID    [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-:/]*
MAILADR   ("mailto:")?[a-z_A-Z0-9.+-]+"@"[a-z_A-Z0-9-]+("."[a-z_A-Z0-9\-]+)+[a-z_A-Z0-9\-]+
OPTSTARS  ("//"{BLANK}*)?"*"*{BLANK}*
LISTITEM  {BLANK}*[-]("#")?{WS}
MLISTITEM {BLANK}*[+*]{WS}
OLISTITEM {BLANK}*[1-9][0-9]*"."{BLANK}
ENDLIST   {BLANK}*"."{BLANK}*\n
ATTRNAME  [a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*
ATTRIB    {ATTRNAME}{WS}*("="{WS}*(("\""[^\"]*"\"")|("'"[^\']*"'")|[^ \t\r\n'"><]+))?
URLCHAR   [a-z_A-Z0-9\!\~\,\:\;\'\$\?\@\&\%\#\.\-\+\/\=]
URLMASK   ({URLCHAR}+([({]{URLCHAR}*[)}])?)+
FILESCHAR [a-z_A-Z0-9\\:\\\/\-\+]
FILEECHAR [a-z_A-Z0-9\-\+]
HFILEMASK ("."{FILESCHAR}*{FILEECHAR}+)*
FILEMASK  ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|{HFILEMASK}
LINKMASK  [^ \t\n\r\\@<&${}]+("("[^\n)]*")")?({BLANK}*("const"|"volatile"){BLANK}+)? 
VERBATIM  "verbatim"{BLANK}*
SPCMD1    {CMD}([a-z_A-Z][a-z_A-Z0-9]*|{VERBATIM})
SPCMD2    {CMD}[\\@<>&$#%~".|]
SPCMD3    {CMD}form#[0-9]+
SPCMD4    {CMD}"::"
INOUT	  "inout"|"in"|"out"|("in"{BLANK}*","{BLANK}*"out")|("out"{BLANK}*","{BLANK}*"in")
PARAMIO   {CMD}param{BLANK}*"["{BLANK}*{INOUT}{BLANK}*"]"
TEMPCHAR  [a-z_A-Z0-9,: \t\*\&]
FUNCCHAR  [a-z_A-Z0-9,:\<\> \t\^\*\&\[\]]
SCOPESEP  "::"|"#"|"."
TEMPLPART "<"{TEMPCHAR}*">"
ANONNS    "anonymous_namespace{"[^}]*"}"
SCOPEPRE  (({ID}{TEMPLPART}?)|{ANONNS}){SCOPESEP}
SCOPEKEYS ":"({ID}":")*
SCOPECPP  {SCOPEPRE}*(~)?{ID}("<"{TEMPCHAR}*">")?
SCOPEOBJC {SCOPEPRE}?{ID}{SCOPEKEYS}?
SCOPEMASK {SCOPECPP}|{SCOPEOBJC}
FUNCARG   "("{FUNCCHAR}*")"({BLANK}*("volatile"|"const"){BLANK})?
OPNEW     {BLANK}+"new"({BLANK}*"[]")?
OPDEL     {BLANK}+"delete"({BLANK}*"[]")?
OPNORM    {OPNEW}|{OPDEL}|"+"|"-"|"*"|"/"|"%"|"^"|"&"|"|"|"~"|"!"|"="|"<"|">"|"+="|"-="|"*="|"/="|"%="|"^="|"&="|"|="|"<<"|">>"|"<<="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|","|"->*"|"->"|"[]"|"()"
OPCAST    {BLANK}+[^<(\r\n.,][^(\r\n.,]*
OPMASK    ({BLANK}*{OPNORM}{FUNCARG})
OPMASKOPT ({BLANK}*{OPNORM}{FUNCARG}?)|({OPCAST}{FUNCARG})
LNKWORD1  ("::"|"#")?{SCOPEMASK}
CVSPEC    {BLANK}*("const"|"volatile")
LNKWORD2  (({SCOPEPRE}*"operator"{OPMASK})|({SCOPEPRE}"operator"{OPMASKOPT})|(("::"|"#"){SCOPEPRE}*"operator"{OPMASKOPT})){CVSPEC}?
LNKWORD3  ([0-9a-z_A-Z\-]+("/"|"\\"))*[0-9a-z_A-Z\-]+("."[0-9a-z_A-Z]+)+
CHARWORDQ [^ \t\n\r\\@<>()\[\]:;\?{}&%$#,."=']
ESCWORD   ("%"{ID}(("::"|"."){ID})*)|("%'")
WORD1     {ESCWORD}|{CHARWORDQ}+|"{"|"}"|"'\"'"|("\""[^"\n]*\n?[^"\n]*"\"")
WORD2     "."|","|"("|")"|"["|"]"|":"|";"|"\?"|"="|"'"
WORD1NQ   {ESCWORD}|{CHARWORDQ}+|"{"|"}"
WORD2NQ   "."|","|"("|")"|"["|"]"|":"|";"|"\?"|"="|"'"
HTMLTAG   "<"(("/")?){ID}({WS}+{ATTRIB})*{WS}*(("/")?)">" 
HTMLKEYL  "strong"|"center"|"table"|"caption"|"small"|"code"|"dfn"|"var"|"img"|"pre"|"sub"|"sup"|"tr"|"td"|"th"|"ol"|"ul"|"li"|"tt"|"kbd"|"em"|"hr"|"dl"|"dt"|"dd"|"br"|"i"|"a"|"b"|"p"
HTMLKEYU  "STRONG"|"CENTER"|"TABLE"|"CAPTION"|"SMALL"|"CODE"|"DFN"|"VAR"|"IMG"|"PRE"|"SUB"|"SUP"|"TR"|"TD"|"TH"|"OL"|"UL"|"LI"|"TT"|"KBD"|"EM"|"HR"|"DL"|"DT"|"DD"|"BR"|"I"|"A"|"B"|"P"
HTMLKEYW  {HTMLKEYL}|{HTMLKEYU}
REFWORD2  ("#"|"::")?((({ID}{TEMPLPART}?)|{ANONNS})("."|"#"|"::"|"-"|"/"))*({ID}(":")?){FUNCARG}?
REFWORD3  ({ID}":")*{ID}":"?
REFWORD   {LABELID}|{REFWORD2}|{REFWORD3}

%option noyywrap
%option yylineno

%x St_Para
%x St_Comment
%x St_Title
%x St_TitleN
%x St_TitleQ
%x St_TitleA
%x St_TitleV
%x St_Code
%x St_CodeOpt
%x St_XmlCode
%x St_HtmlOnly
%x St_ManOnly
%x St_LatexOnly
%x St_RtfOnly
%x St_XmlOnly
%x St_DbOnly
%x St_Verbatim
%x St_Dot
%x St_Msc
%x St_Param
%x St_XRefItem
%x St_XRefItem2
%x St_File
%x St_Pattern
%x St_Link
%x St_Cite
%x St_Ref
%x St_Ref2
%x St_IntRef
%x St_Text
%x St_SkipTitle
%x St_Anchor
%x St_Snippet

%x St_Sections
%s St_SecLabel1
%s St_SecLabel2
%s St_SecTitle
%x St_SecSkip

%%
<St_Para>\r            /* skip carriage return */
<St_Para>^{LISTITEM}   { /* list item */ 
                         QCString text=yytext;
			 int dashPos = text.findRev('-');
			 g_token->isEnumList = text.at(dashPos+1)=='#';
                         g_token->id         = -1;
			 g_token->indent     = computeIndent(yytext,dashPos);
                         return TK_LISTITEM;
                       }
<St_Para>^{MLISTITEM}  { /* list item */ 
                         if (!Doxygen::markdownSupport || g_insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           QCString text=yytext;
                           static QRegExp re("[*+]");
                           int listPos = text.findRev(re);
                           g_token->isEnumList = FALSE;
                           g_token->id         = -1;
                           g_token->indent     = computeIndent(yytext,listPos);
                           return TK_LISTITEM;
                         }
                       }
<St_Para>^{OLISTITEM}  { /* numbered list item */ 
                         if (!Doxygen::markdownSupport || g_insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           QCString text=yytext;
                           static QRegExp re("[1-9]");
                           int digitPos = text.find(re);
                           int dotPos = text.find('.',digitPos);
                           g_token->isEnumList = TRUE;
                           g_token->id         = atoi(QCString(yytext).mid(digitPos,dotPos-digitPos));
                           g_token->indent     = computeIndent(yytext,digitPos);
                           return TK_LISTITEM;
                         }
                       }
<St_Para>{BLANK}*\n{LISTITEM}     { /* list item on next line */ 
                         QCString text=yytext;
			 text=text.right(text.length()-text.find('\n')-1);
			 int dashPos = text.findRev('-');
			 g_token->isEnumList = text.at(dashPos+1)=='#';
                         g_token->id         = -1;
			 g_token->indent     = computeIndent(text,dashPos);
                         return TK_LISTITEM;
                       }
<St_Para>{BLANK}*\n{MLISTITEM}     { /* list item on next line */ 
                         if (!Doxygen::markdownSupport || g_insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           QCString text=yytext;
                           static QRegExp re("[*+]");
                           text=text.right(text.length()-text.find('\n')-1);
                           int markPos = text.findRev(re);
                           g_token->isEnumList = FALSE;
                           g_token->id         = -1;
                           g_token->indent     = computeIndent(text,markPos);
                           return TK_LISTITEM;
                         }
                       }
<St_Para>{BLANK}*\n{OLISTITEM}     { /* list item on next line */ 
                         if (!Doxygen::markdownSupport || g_insidePre)
                         {
                           REJECT;
                         }
                         else
                         {
                           QCString text=yytext;
                           int nl=text.findRev('\n');
                           int len=text.length();
                           text=text.right(len-nl-1);
                           static QRegExp re("[1-9]");
                           int digitPos = text.find(re);
                           int dotPos = text.find('.',digitPos);
                           g_token->isEnumList = TRUE;
                           g_token->id         = atoi(QCString(text).mid(digitPos,dotPos-digitPos));
                           g_token->indent     = computeIndent(text,digitPos);
                           return TK_LISTITEM;
                         }
                       }
<St_Para>^{ENDLIST}       { /* end list */ 
                         int dotPos = QCString(yytext).findRev('.');
			 g_token->indent     = computeIndent(yytext,dotPos);
                         return TK_ENDLIST;
                       }
<St_Para>{BLANK}*\n{ENDLIST}      { /* end list on next line */ 
                         QCString text=yytext;
			 text=text.right(text.length()-text.find('\n')-1);
                         int dotPos = text.findRev('.');
			 g_token->indent     = computeIndent(text,dotPos);
                         return TK_ENDLIST;
                       }
<St_Para>"{"{BLANK}*"@link" {
  			 g_token->name = "javalink";
			 return TK_COMMAND;
  		       }
<St_Para>"{"{BLANK}*"@inheritDoc"{BLANK}*"}" {
  			 g_token->name = "inheritdoc";
			 return TK_COMMAND;
		       }
<St_Para>"@_fakenl"     { // artificial new line
                             yylineno++; 
  		          }
<St_Para>{SPCMD3}      {
  			 g_token->name = "form";
			 bool ok;
			 g_token->id = QCString(yytext).right((int)yyleng-6).toInt(&ok);
			 ASSERT(ok);
			 return TK_COMMAND;
  		       }
<St_Para>{SPCMD1}      |
<St_Para>{SPCMD2}      |
<St_Para>{SPCMD4}      { /* special command */
                         g_token->name = yytext+1;
			 g_token->name = g_token->name.stripWhiteSpace();
			 g_token->paramDir=TokenInfo::Unspecified;
                         return TK_COMMAND;
  		       }
<St_Para>{PARAMIO}     { /* param [in,out] command */
  			 g_token->name = "param";
			 QCString s(yytext);
			 bool isIn  = s.find("in")!=-1;
			 bool isOut = s.find("out")!=-1;
			 if (isIn)
			 {
			   if (isOut)
			   {
			     g_token->paramDir=TokenInfo::InOut;
			   }
			   else
			   {
			     g_token->paramDir=TokenInfo::In;
			   }
			 }
			 else if (isOut)
			 {
			   g_token->paramDir=TokenInfo::Out;
			 }
			 else
			 {
			   g_token->paramDir=TokenInfo::Unspecified;
			 }
			 return TK_COMMAND;
  		       }
<St_Para>("http:"|"https:"|"ftp:"|"file:"|"news:"){URLMASK}/\. { // URL.
                         g_token->name=yytext;
			 g_token->isEMailAddr=FALSE;
			 return TK_URL;
                       }
<St_Para>("http:"|"https:"|"ftp:"|"file:"|"news:"){URLMASK} { // URL
                         g_token->name=yytext;
			 g_token->isEMailAddr=FALSE;
			 return TK_URL;
                       }
<St_Para>"<"("http:"|"https:"|"ftp:"|"file:"|"news:"){URLMASK}">" { // URL
                         g_token->name=yytext;
                         g_token->name = g_token->name.mid(1,g_token->name.length()-2);
			 g_token->isEMailAddr=FALSE;
			 return TK_URL;
                       }
<St_Para>{MAILADR}     { // Mail address
                         g_token->name=yytext;
                         g_token->name.stripPrefix("mailto:");
			 g_token->isEMailAddr=TRUE;
			 return TK_URL;
                       }
<St_Para>"<"{MAILADR}">" { // Mail address
                         g_token->name=yytext;
                         g_token->name = g_token->name.mid(1,g_token->name.length()-2);
                         g_token->name.stripPrefix("mailto:");
			 g_token->isEMailAddr=TRUE;
			 return TK_URL;
                       }
<St_Para>"$"{ID}":"[^\n$]+"$" { /* RCS tag */
                         QCString tagName(yytext+1);
			 int index=tagName.find(':');
  			 g_token->name = tagName.left(index+1);
			 g_token->text = tagName.mid(index+2,tagName.length()-index-3);
			 return TK_RCSTAG;
  		       }
<St_Para,St_HtmlOnly>"$("{ID}")"   { /* environment variable */
                         QCString name = &yytext[2];
			 name = name.left(name.length()-1);
			 QCString value = portable_getenv(name);
			 for (int i=value.length()-1;i>=0;i--) unput(value.at(i));
                       }
<St_Para>{HTMLTAG}     { /* html tag */ 
                         handleHtmlTag();
                         return TK_HTMLTAG;
                       }
<St_Para,St_Text>"&"{ID}";" { /* special symbol */ 
                         g_token->name = yytext;
                         return TK_SYMBOL;
                       }

  /********* patterns for linkable words ******************/

<St_Para>{ID}/"<"{HTMLKEYW}">" { /* this rule is to prevent opening html 
				  * tag to be recognized as a templated classes 
				  */ 
                         g_token->name = yytext;
                         return TK_LNKWORD;
  			}
<St_Para>{LNKWORD1}/"<br>"           | // prevent <br> html tag to be parsed as template arguments
<St_Para>{LNKWORD1}                  |
<St_Para>{LNKWORD1}{FUNCARG}         |
<St_Para>{LNKWORD2}                  |
<St_Para>{LNKWORD3}    {
                         g_token->name = yytext;
                         return TK_LNKWORD;
  		       }
<St_Para>{LNKWORD1}{FUNCARG}{CVSPEC}[^a-z_A-Z0-9] {
                         g_token->name = yytext;
                         g_token->name = g_token->name.left(g_token->name.length()-1);
			 unput(yytext[(int)yyleng-1]);
                         return TK_LNKWORD;
                       }
  /********* patterns for normal words ******************/

<St_Para,St_Text>{WORD1} |
<St_Para,St_Text>{WORD2} { /* function call */ 
                         if (yytext[0]=='%') // strip % if present
			   g_token->name = &yytext[1];
			 else
                           g_token->name = yytext;
                         return TK_WORD;

			 /* the following is dummy code to please the 
			  * compiler, removing this results in a warning 
			  * on my machine 
			  */ 
			 goto find_rule;
                       }
<St_Text>({ID}".")+{ID} {
                          g_token->name = yytext;
                          return TK_WORD;
  			}
<St_Para,St_Text>"operator"/{BLANK}*"<"[a-zA-Z_0-9]+">" { // Special case: word "operator" followed by a HTML command
                                                          // avoid interpretation as "operator <"
                           g_token->name = yytext;
                           return TK_WORD;
                         }

  /*******************************************************/

<St_Para,St_Text>{BLANK}+      |
<St_Para,St_Text>{BLANK}*\n{BLANK}* { /* white space */ 
                         g_token->chars=yytext;
                         return TK_WHITESPACE;
                       }
<St_Text>[\\@<>&$#%~]  {
                         g_token->name = yytext;
                         return TK_COMMAND;
  		       }
<St_Para>({BLANK}*\n)+{BLANK}*\n{BLANK}* {
                         // g_insidePre was always FALSE, so the next section
                         // was never executed, now g_insidePre is set properly
                         // again, so the section is commented out to keep the
                         // old behavior.
                         //if (g_insidePre)
			 //{
			 //  /* Inside a <pre>..</pre> blank lines are treated
			 //   * as whitespace.
			 //   */
                         //  g_token->chars=yytext;
			 //  return TK_WHITESPACE;
			 //}
			 //else // found end of a paragraph
			 {
                           g_token->indent=computeIndent(yytext,(int)yyleng);
                           int i;
                           // put back the indentation (needed for list items)
                           for (i=0;i<g_token->indent;i++)
                           {
                             unput(' ');
                           }
                           // tell flex that after putting the last indent 
                           // back we are at the beginning of the line
                           YY_CURRENT_BUFFER->yy_at_bol=1;
                           // start of a new paragraph
  		           return TK_NEWPARA;
			 }
                       }
<St_CodeOpt>{BLANK}*"{"(".")?{LABELID}"}" {
                         g_token->name = yytext;
                         int i=g_token->name.find('{');
                         g_token->name = g_token->name.mid(i+1,g_token->name.length()-i-2);
                         BEGIN(St_Code);
                       }
<St_CodeOpt>\n         |
<St_CodeOpt>.          {
                         unput(*yytext);
                         BEGIN(St_Code);
                       }
<St_Code>{WS}*{CMD}"endcode" {
                         return RetVal_OK;
                       }
<St_XmlCode>{WS}*"</code>" {
                         return RetVal_OK;
                       }
<St_Code,St_XmlCode>[^\\@\n<]+  |
<St_Code,St_XmlCode>\n          |
<St_Code,St_XmlCode>.           {
  			 g_token->verb+=yytext;
  		       }
<St_HtmlOnly>{CMD}"endhtmlonly" {
                         return RetVal_OK;
                       }
<St_HtmlOnly>[^\\@\n$]+    |
<St_HtmlOnly>\n            |
<St_HtmlOnly>.             {
  			 g_token->verb+=yytext;
  		       }
<St_ManOnly>{CMD}"endmanonly" {
                         return RetVal_OK;
                       }
<St_ManOnly>[^\\@\n$]+    |
<St_ManOnly>\n            |
<St_ManOnly>.             {
  			 g_token->verb+=yytext;
  		       }
<St_RtfOnly>{CMD}"endrtfonly" {
                         return RetVal_OK;
                       }
<St_RtfOnly>[^\\@\n$]+    |
<St_RtfOnly>\n            |
<St_RtfOnly>.             {
  			 g_token->verb+=yytext;
  		       }
<St_LatexOnly>{CMD}"endlatexonly" {
                         return RetVal_OK;
                       }
<St_LatexOnly>[^\\@\n]+     |
<St_LatexOnly>\n            |
<St_LatexOnly>.             {
  			 g_token->verb+=yytext;
  		       }
<St_XmlOnly>{CMD}"endxmlonly" {
                         return RetVal_OK;
                       }
<St_XmlOnly>[^\\@\n]+  |
<St_XmlOnly>\n         |
<St_XmlOnly>.          {
  			 g_token->verb+=yytext;
  		       }
<St_DbOnly>{CMD}"enddocbookonly" {
                         return RetVal_OK;
                       }
<St_DbOnly>[^\\@\n]+  |
<St_DbOnly>\n         |
<St_DbOnly>.          {
  			 g_token->verb+=yytext;
  		       }
<St_Verbatim>{CMD}"endverbatim" {
  			 g_token->verb=stripEmptyLines(g_token->verb);
                         return RetVal_OK;
                       }
<St_Verbatim>[^\\@\n]+ |
<St_Verbatim>\n        |
<St_Verbatim>.         { /* Verbatim text */
  			 g_token->verb+=yytext;
  		       }
<St_Dot>{CMD}"enddot"  {
                         return RetVal_OK;
                       }
<St_Dot>[^\\@\n]+      |
<St_Dot>\n             |
<St_Dot>.              { /* dot text */
  			 g_token->verb+=yytext;
  		       }
<St_Msc>{CMD}("endmsc"|"endvhdlflow")  {
                         return RetVal_OK;
                       }
<St_Msc>[^\\@\n]+      |
<St_Msc>\n             |
<St_Msc>.              { /* msc text */
  			 g_token->verb+=yytext;
  		       }
<St_Title>"\""	       { // quoted title
  			 BEGIN(St_TitleQ);
  		       } 
<St_Title>[ \t]+       {
                         g_token->chars=yytext;
  			 return TK_WHITESPACE;
                       }
<St_Title>.	       { // non-quoted title
  			 unput(*yytext);
			 BEGIN(St_TitleN);
                       }
<St_Title>\n	       {
                         unput(*yytext);
  			 return 0;
  		       }
<St_TitleN>"&"{ID}";"  { /* symbol */
                         g_token->name = yytext;
  		         return TK_SYMBOL;
                       }
<St_TitleN>{HTMLTAG}   {
  		       }
<St_TitleN>{SPCMD1}    |   
<St_TitleN>{SPCMD2}    { /* special command */ 
                         g_token->name = yytext+1;
			 g_token->paramDir=TokenInfo::Unspecified;
                         return TK_COMMAND;
                       }
<St_TitleN>{WORD1}     |
<St_TitleN>{WORD2}     { /* word */
                         if (yytext[0]=='%') // strip % if present
			   g_token->name = &yytext[1];
			 else
                           g_token->name = yytext;
			 return TK_WORD;
                       }
<St_TitleN>[ \t]+      {
                         g_token->chars=yytext;
  			 return TK_WHITESPACE;
                       }
<St_TitleN>\n	       { /* new line => end of title */
                         unput(*yytext);
  			 return 0;
                       }
<St_TitleQ>"&"{ID}";"  { /* symbol */
                         g_token->name = yytext;
  		         return TK_SYMBOL;
                       }
<St_TitleQ>{SPCMD1}    |   
<St_TitleQ>{SPCMD2}    { /* special command */ 
                         g_token->name = yytext+1;
			 g_token->paramDir=TokenInfo::Unspecified;
                         return TK_COMMAND;
                       }
<St_TitleQ>{WORD1NQ}   |
<St_TitleQ>{WORD2NQ}   { /* word */
                         g_token->name = yytext;
			 return TK_WORD;
                       }
<St_TitleQ>[ \t]+      {
                         g_token->chars=yytext;
  			 return TK_WHITESPACE;
                       }
<St_TitleQ>"\""	       { /* closing quote => end of title */
  			 BEGIN(St_TitleA);
  			 return 0;
                       }
<St_TitleQ>\n	       { /* new line => end of title */
                         unput(*yytext);
  			 return 0;
                       }
<St_TitleA>{BLANK}*{ID}{BLANK}*"="{BLANK}* { // title attribute
  			 g_token->name = yytext;
			 g_token->name = g_token->name.left(
			       g_token->name.find('=')).stripWhiteSpace();
  			 BEGIN(St_TitleV);
  		       }
<St_TitleV>[^ \t\r\n]+ { // attribute value
  			 g_token->chars = yytext;
			 BEGIN(St_TitleN);
			 return TK_WORD;
  		       }
<St_TitleV,St_TitleA>. {
  			 unput(*yytext);
  			 return 0;
                       }
<St_TitleV,St_TitleA>\n	 {
  			 return 0;
                       }

<St_Anchor>{LABELID}{WS}? { // anchor
  			 g_token->name = QCString(yytext).stripWhiteSpace();
  			 return TK_WORD;
  		       }
<St_Anchor>.	       {
  			 unput(*yytext);
			 return 0;
  		       }
<St_Cite>{CITEID}      { // label to cite
  			 g_token->name=yytext;
			 return TK_WORD;
  		       }
<St_Cite>{BLANK}       { // white space
  			 unput(' ');
  			 return 0;
                       }
<St_Cite>\n	       { // new line
                         unput(*yytext);
  			 return 0;
  		       }
<St_Cite>.	       { // any other character
                         unput(*yytext);
  			 return 0;
  		       }
<St_Ref>{REFWORD}      { // label to refer to
  			 g_token->name=yytext;
			 return TK_WORD;
  		       }
<St_Ref>{BLANK}        { // white space
  			 unput(' ');
  			 return 0;
                       }
<St_Ref>{WS}+"\""{WS}* { // white space following by quoted string
  			 BEGIN(St_Ref2);
                       }
<St_Ref>\n	       { // new line
                         unput(*yytext);
  			 return 0;
  		       }
<St_Ref>.	       { // any other character
                         unput(*yytext);
  			 return 0;
  		       }
<St_IntRef>[A-Z_a-z0-9.:/#\-\+\(\)]+ {
                         g_token->name = yytext;
			 return TK_WORD;
  		       }
<St_IntRef>{BLANK}+"\"" {
                         BEGIN(St_Ref2);
                       }
<St_Ref2>"&"{ID}";"    { /* symbol */
                         g_token->name = yytext;
  		         return TK_SYMBOL;
                       }
<St_Ref2>{SPCMD1}      |   
<St_Ref2>{SPCMD2}      { /* special command */ 
                         g_token->name = yytext+1;
			 g_token->paramDir=TokenInfo::Unspecified;
                         return TK_COMMAND;
                       }
<St_Ref2>{WORD1NQ}     |
<St_Ref2>{WORD2NQ}     {
                         /* word */
                         g_token->name = yytext;
			 return TK_WORD;
                       }
<St_Ref2>[ \t]+        {
                         g_token->chars=yytext;
  			 return TK_WHITESPACE;
                       }
<St_Ref2>"\""|\n       { /* " or \n => end of title */
  			 return 0;
                       }
<St_XRefItem>{LABELID} {
                         g_token->name=yytext;
                       }
<St_XRefItem>" "       {
                         BEGIN(St_XRefItem2);
                       }
<St_XRefItem2>[0-9]+"." {
  			 QCString numStr=yytext;
			 numStr=numStr.left((int)yyleng-1);
			 g_token->id=numStr.toInt();
			 return RetVal_OK;
  		       }
<St_Para,St_Title,St_Ref2>"<!--"     { /* html style comment block */
                         g_commentState = YY_START;
                         BEGIN(St_Comment); 
                       }
<St_Param>"\""[^\n\"]+"\"" {
  			 g_token->name = yytext+1;
			 g_token->name = g_token->name.left((int)yyleng-2);
			 return TK_WORD;
                       }
<St_Param>({PHPTYPE}{BLANK}*"|"{BLANK}*)*{PHPTYPE}{WS}+("&")?"$"{LABELID} {
  			 QCString params = yytext;
			 int j = params.find('&');
			 int i = params.find('$');
			 if (j<i && j!=-1) i=j;
			 QCString types = params.left(i).stripWhiteSpace();
			 g_token->name = types+"#"+params.mid(i);
			 return TK_WORD;
  		       }
<St_Param>[^ \t\n,]+   {
  			 g_token->name = yytext;
			 return TK_WORD;
                       }
<St_Param>{WS}*","{WS}*  /* param separator */
<St_Param>{WS}	       {
                         g_token->chars=yytext;
                         return TK_WHITESPACE;
                       }
<St_File>{FILEMASK}    {
  			 g_token->name = yytext;
			 return TK_WORD;  
  		       }
<St_File>"\""[^\n\"]+"\"" {
  		         QCString text=yytext;
			 g_token->name = text.mid(1,text.length()-2);
			 return TK_WORD;
  		       }
<St_Pattern>[^\r\n]+   {
                         g_token->name = yytext;
                         g_token->name = g_token->name.stripWhiteSpace();
			 return TK_WORD;
  		       }
<St_Link>{LINKMASK}|{REFWORD}    {
                         g_token->name = yytext;
			 return TK_WORD;
                       }
<St_Comment>"-->"      { /* end of html comment */
                         BEGIN(g_commentState); 
                       }
<St_Comment>[^-\n]+       /* inside html comment */
<St_Comment>.             /* inside html comment */

     /* State for skipping title (all chars until the end of the line) */

<St_SkipTitle>.
<St_SkipTitle>\n       { return 0; }

     /* State for the pass used to find the anchors and sections */ 

<St_Sections>[^\n@\\]+
<St_Sections>"@@"|"\\\\"
<St_Sections>{CMD}"anchor"{BLANK}+  { 
                                      g_secType = SectionInfo::Anchor; 
				      BEGIN(St_SecLabel1); 
                                    }
<St_Sections>{CMD}"section"{BLANK}+ { 
                                      g_secType = SectionInfo::Section; 
                                      BEGIN(St_SecLabel2); 
                                    }
<St_Sections>{CMD}"subsection"{BLANK}+ { 
                                      g_secType = SectionInfo::Subsection; 
                                      BEGIN(St_SecLabel2); 
                                    }
<St_Sections>{CMD}"subsubsection"{BLANK}+ { 
                                      g_secType = SectionInfo::Subsubsection; 
                                      BEGIN(St_SecLabel2); 
                                    }
<St_Sections>{CMD}"paragraph"{BLANK}+ { 
                                      g_secType = SectionInfo::Paragraph; 
                                      BEGIN(St_SecLabel2); 
                                    }
<St_Sections>{CMD}"verbatim"/[^a-z_A-Z0-9]  {
                                      g_endMarker="endverbatim";
				      BEGIN(St_SecSkip);
  				    }
<St_Sections>{CMD}"dot"/[^a-z_A-Z0-9] {
                                      g_endMarker="enddot";
				      BEGIN(St_SecSkip);
  				    }
<St_Sections>{CMD}"msc"/[^a-z_A-Z0-9] {
                                      g_endMarker="endmsc";
				      BEGIN(St_SecSkip);
  				    }
<St_Sections>{CMD}"htmlonly"/[^a-z_A-Z0-9] {
                                      g_endMarker="endhtmlonly";
				      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"latexonly"/[^a-z_A-Z0-9] {
                                      g_endMarker="endlatexonly";
				      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"xmlonly"/[^a-z_A-Z0-9] {
                                      g_endMarker="endxmlonly";
				      BEGIN(St_SecSkip);
                                    }
<St_Sections>{CMD}"code"/[^a-z_A-Z0-9] {
                                      g_endMarker="endcode";
				      BEGIN(St_SecSkip);
  				    }
<St_Sections>"<!--"                 {
                                      g_endMarker="-->";
				      BEGIN(St_SecSkip);
                                    }
<St_SecSkip>{CMD}{ID}		    {
                                      if (qstrcmp(yytext+1,g_endMarker)==0)
				      {
					BEGIN(St_Sections);
				      }
                                    }
<St_SecSkip>"-->"		    {
                                      if (qstrcmp(yytext,g_endMarker)==0)
				      {
					BEGIN(St_Sections);
				      }
                                    }
<St_SecSkip>[^a-z_A-Z0-9\-\\\@]+
<St_SecSkip>.
<St_SecSkip>\n
<St_Sections>.
<St_Sections>\n        
<St_SecLabel1>{LABELID} { 
                         g_secLabel = yytext;
                         processSection();
                         BEGIN(St_Sections);
                       }
<St_SecLabel2>{LABELID}{BLANK}+ | 
<St_SecLabel2>{LABELID}         { 
                         g_secLabel = yytext;
			 g_secLabel = g_secLabel.stripWhiteSpace();
                         BEGIN(St_SecTitle);
                       }
<St_SecTitle>[^\n]+    |
<St_SecTitle>[^\n]*\n  {
                         g_secTitle = yytext;
			 g_secTitle = g_secTitle.stripWhiteSpace();
                         processSection();
                         BEGIN(St_Sections);
		       }
<St_SecTitle,St_SecLabel1,St_SecLabel2>. {
                         warn(g_fileName,yylineno,"warning: Unexpected character `%s' while looking for section label or title",yytext); 
                       }

<St_Snippet>[^\n]+     |
<St_Snippet>[^\n]*\n   {
  			 g_token->name = yytext;
			 g_token->name = g_token->name.stripWhiteSpace();
			 return TK_WORD;  
  		       }
   
     /* Generic rules that work for all states */ 
<*>\n                  { 
                         warn(g_fileName,yylineno,"warning: Unexpected new line character"); 
		       }
<*>[\\@<>&$#%~"=]      { /* unescaped special character */
                         //warn(g_fileName,yylineno,"warning: Unexpected character `%s', assuming command \\%s was meant.",yytext,yytext); 
			 g_token->name = yytext;
			 return TK_COMMAND;
                       }
<*>.                   { 
                         warn(g_fileName,yylineno,"warning: Unexpected character `%s'",yytext); 
		       }
%%

//--------------------------------------------------------------------------

void doctokenizerYYFindSections(const char *input,Definition *d,
                                MemberGroup *mg,const char *fileName)
{
  if (input==0) return;
  g_inputString = input;
  //printf("parsing --->`%s'<---\n",input);
  g_inputPos    = 0;
  g_definition  = d;
  g_memberGroup = mg;
  g_fileName    = fileName;
  BEGIN(St_Sections);
  doctokenizerYYlineno = 1;
  doctokenizerYYlex();
}

void doctokenizerYYinit(const char *input,const char *fileName)
{
  g_inputString = input;
  g_inputPos    = 0;
  g_fileName    = fileName;
  g_insidePre   = FALSE;
  BEGIN(St_Para);
}

void doctokenizerYYsetStatePara()
{
  BEGIN(St_Para);
}

void doctokenizerYYsetStateTitle()
{
  BEGIN(St_Title);
}

void doctokenizerYYsetStateTitleAttrValue()
{
  BEGIN(St_TitleV);
}

void doctokenizerYYsetStateCode()
{
  g_token->verb="";
  g_token->name="";
  BEGIN(St_CodeOpt);
}

void doctokenizerYYsetStateXmlCode()
{
  g_token->verb="";
  g_token->name="";
  BEGIN(St_XmlCode);
}

void doctokenizerYYsetStateHtmlOnly()
{
  g_token->verb="";
  BEGIN(St_HtmlOnly);
}

void doctokenizerYYsetStateManOnly()
{
  g_token->verb="";
  BEGIN(St_ManOnly);
}

void doctokenizerYYsetStateRtfOnly()
{
  g_token->verb="";
  BEGIN(St_RtfOnly);
}

void doctokenizerYYsetStateXmlOnly()
{
  g_token->verb="";
  BEGIN(St_XmlOnly);
}

void doctokenizerYYsetStateDbOnly()
{
  g_token->verb="";
  BEGIN(St_DbOnly);
}

void doctokenizerYYsetStateLatexOnly()
{
  g_token->verb="";
  BEGIN(St_LatexOnly);
}

void doctokenizerYYsetStateVerbatim()
{
  g_token->verb="";
  BEGIN(St_Verbatim);
}

void doctokenizerYYsetStateDot()
{
  g_token->verb="";
  BEGIN(St_Dot);
}

void doctokenizerYYsetStateMsc()
{
  g_token->verb="";
  BEGIN(St_Msc);
}

void doctokenizerYYsetStateParam()
{
  BEGIN(St_Param);
}

void doctokenizerYYsetStateXRefItem()
{
  BEGIN(St_XRefItem);
}

void doctokenizerYYsetStateFile()
{
  BEGIN(St_File);
}

void doctokenizerYYsetStatePattern()
{
  BEGIN(St_Pattern);
}

void doctokenizerYYsetStateLink()
{
  BEGIN(St_Link);
}

void doctokenizerYYsetStateCite()
{
  BEGIN(St_Cite);
}

void doctokenizerYYsetStateRef()
{
  BEGIN(St_Ref);
}

void doctokenizerYYsetStateInternalRef()
{
  BEGIN(St_IntRef);
}

void doctokenizerYYsetStateText()
{
  BEGIN(St_Text);
}

void doctokenizerYYsetStateSkipTitle()
{
  BEGIN(St_SkipTitle);
}

void doctokenizerYYsetStateAnchor()
{
  BEGIN(St_Anchor);
}

void doctokenizerYYsetStateSnippet()
{
  BEGIN(St_Snippet);
}

void doctokenizerYYcleanup()
{
  yy_delete_buffer( YY_CURRENT_BUFFER );
}

void doctokenizerYYsetInsidePre(bool b)
{
  g_insidePre = b;
}

void doctokenizerYYpushBackHtmlTag(const char *tag)
{
  QCString tagName = tag;
  int i,l = tagName.length();
  unput('>');
  for (i=l-1;i>=0;i--)
  {
    unput(tag[i]);
  }
  unput('<');
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
    void doctokenizerYYdummy() { yy_flex_realloc(0,0); }
}
#endif

