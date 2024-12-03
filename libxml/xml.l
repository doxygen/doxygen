/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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
/******************************************************************************
 * Minimal flex based parser for XML
 ******************************************************************************/

%option never-interactive
%option prefix="xmlYY"
%option reentrant
%option extra-type="struct xmlYY_state *"
%option 8bit noyywrap
%top{
#include <stdint.h>
}

%{

#include <ctype.h>
#include <vector>
#include <stdio.h>
#include "xml.h"

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct xmlYY_state
{
  std::string   fileName;
  int           lineNr = 1;
  const char *  inputString = nullptr; //!< the code fragment as text
  int           inputPosition = 0;     //!< read offset during parsing
  std::string   name;
  bool          isEnd = false;
  bool          selfClose = false;
  std::string   data;
  std::string   attrValue;
  std::string   attrName;
  XMLHandlers::Attributes attrs;
  XMLHandlers   handlers;
  int           cdataContext;
  int           commentContext;
  char          stringChar;
  std::string   encoding;
  std::vector<std::string> xpath;
  std::function<XMLParser::Transcode> transcodeFunc;
};

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void initElement(yyscan_t yyscanner);
static void addCharacters(yyscan_t yyscanner);
static void addElement(yyscan_t yyscanner);
static void addAttribute(yyscan_t yyscanner);
static void countLines(yyscan_t yyscanner, const char *txt,yy_size_t len);
static void reportError(yyscan_t yyscanner, const std::string &msg);
static std::string processData(yyscan_t yyscanner,const char *txt,yy_size_t len);

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

%}

NL		(\r\n|\r|\n)
SP		[ \t\r\n]+
OPEN		{SP}?"<"
OPENSPECIAL     {SP}?"<?"
CLOSE		">"{NL}?
CLOSESPECIAL    "?>"{NL}?
NAMESTART	[:A-Za-z\200-\377_]
NAMECHAR	[:A-Za-z\200-\377_0-9.-]
NAME		{NAMESTART}{NAMECHAR}*
ESC		"&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
COLON		":"
PCDATA		[^<]+
COMMENT		{OPEN}"!--"
COMMENTEND      "--"{CLOSE}
STRING		\"([^"&]|{ESC})*\"|\'([^'&]|{ESC})*\'
DOCTYPE         {SP}?"<!DOCTYPE"{SP}
CDATA           {SP}?"<![CDATA["
ENDCDATA        "]]>"

%option noyywrap

%s Initial
%s Content
%s CDataSection
%s Element
%s Attributes
%s AttributeValue
%s AttrValueStr
%s Prolog
%s Comment

%%

<Initial>{
  {SP}             { countLines(yyscanner,yytext,yyleng); }
  {DOCTYPE}        { countLines(yyscanner,yytext,yyleng); }
  {OPENSPECIAL}    { countLines(yyscanner,yytext,yyleng); BEGIN(Prolog); }
  {OPEN}           { countLines(yyscanner,yytext,yyleng);
                     initElement(yyscanner);
                     BEGIN(Element); }
  {COMMENT}        { yyextra->commentContext = YY_START;
                     BEGIN(Comment);
                   }
}
<Content>{
  {CDATA}          { countLines(yyscanner,yytext,yyleng);
                     yyextra->cdataContext = YY_START;
                     BEGIN(CDataSection);
                   }
  {PCDATA}         { yyextra->data += processData(yyscanner,yytext,yyleng); }
  {OPEN}           { countLines(yyscanner,yytext,yyleng);
                     addCharacters(yyscanner);
                     initElement(yyscanner);
                     BEGIN(Element);
                   }
  {COMMENT}        { yyextra->commentContext = YY_START;
                     countLines(yyscanner,yytext,yyleng);
                     BEGIN(Comment);
                   }
}
<Element>{
  "/"              { yyextra->isEnd = true; }
  {NAME}           { yyextra->name = yytext;
                     BEGIN(Attributes); }
  {CLOSE}          { addElement(yyscanner);
                     countLines(yyscanner,yytext,yyleng);
                     yyextra->data = "";
                     BEGIN(Content);
                   }
  {SP}             { countLines(yyscanner,yytext,yyleng); }
}
<Attributes>{
  "/"              { yyextra->selfClose = true; }
  {NAME}           { yyextra->attrName = yytext; }
  "="              { BEGIN(AttributeValue); }
  {CLOSE}          { addElement(yyscanner);
                     countLines(yyscanner,yytext,yyleng);
                     yyextra->data = "";
                     BEGIN(Content);
                   }
  {SP}             { countLines(yyscanner,yytext,yyleng); }
}
<AttributeValue>{
  {SP}             { countLines(yyscanner,yytext,yyleng); }
  ['"]             { yyextra->stringChar = *yytext;
                     yyextra->attrValue = "";
                     BEGIN(AttrValueStr);
                   }
  .                { std::string msg = std::string("Missing attribute value. Unexpected character `")+yytext+"` found";
                     reportError(yyscanner,msg);
                     unput(*yytext);
                     BEGIN(Attributes);
                   }
}
<AttrValueStr>{
  [^'"\n]+         { yyextra->attrValue += processData(yyscanner,yytext,yyleng); }
  ['"]             { if (*yytext==yyextra->stringChar)
                     {
                       addAttribute(yyscanner);
                       BEGIN(Attributes);
                     }
                     else
                     {
                       yyextra->attrValue += processData(yyscanner,yytext,yyleng);
                     }
                   }
  \n               { yyextra->lineNr++; yyextra->attrValue+=' '; }
}
<CDataSection>{
  {ENDCDATA}       { BEGIN(yyextra->cdataContext); }
  [^]\n]+          { yyextra->data += yytext; }
  \n               { yyextra->data += yytext;
                     yyextra->lineNr++;
                   }
  .                { yyextra->data += yytext; }
}
<Prolog>{
  "encoding"\s*=\s*\"[^\"]*\" {
                     std::string encoding=yytext;
                     size_t i=encoding.find('"');
                     encoding=encoding.substr(i+1,yyleng-i-2);
                     if (encoding!="UTF-8") // need to transcode to UTF-8
                     {
                       yyextra->encoding=encoding;
                     }
                   }
  {CLOSESPECIAL}   { countLines(yyscanner,yytext,yyleng);
                     BEGIN(Initial);
                   }
  \n               { yyextra->lineNr++; }
  .                { }
}
<Comment>{
  {COMMENTEND}     { countLines(yyscanner,yytext,yyleng);
                     BEGIN(yyextra->commentContext);
                   }
  [^\n-]+          { }
  \n               { yyextra->lineNr++; }
  .                { }
}
\n                 { yyextra->lineNr++; }
.                  { std::string msg = "Unexpected character `";
                     msg+=yytext;
                     msg+="` found";
                     reportError(yyscanner,msg);
                   }

%%

//----------------------------------------------------------------------------------------

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  int c=0;
  while( c < max_size && *s)
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}

static void countLines(yyscan_t yyscanner, const char *txt,yy_size_t len)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (yy_size_t i=0;i<len;i++)
  {
    if (txt[i]=='\n') yyextra->lineNr++;
  }
}

static void initElement(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->isEnd = false;     // true => </tag>
  yyextra->selfClose = false; // true => <tag/>
  yyextra->name = "";
  yyextra->attrs.clear();
}

static void checkAndUpdatePath(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->xpath.empty())
  {
    std::string msg = "found closing tag '"+yyextra->name+"' without matching opening tag";
    reportError(yyscanner,msg);
  }
  else
  {
    std::string expectedTagName = yyextra->xpath.back();
    if (expectedTagName!=yyextra->name)
    {
      std::string msg = "Found closing tag '"+yyextra->name+"' that does not match the opening tag '"+expectedTagName+"' at the same level";
      reportError(yyscanner,msg);
    }
    else // matching end tag
    {
      yyextra->xpath.pop_back();
    }
  }
}

static void addElement(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->isEnd)
  {
    yyextra->xpath.push_back(yyextra->name);
    if (yyextra->handlers.startElement)
    {
      yyextra->handlers.startElement(yyextra->name,yyextra->attrs);
    }
    if (yy_flex_debug)
    {
      fprintf(stderr,"%d: startElement(%s,attr=[",yyextra->lineNr,yyextra->name.data());
      for (auto attr : yyextra->attrs)
      {
        fprintf(stderr,"%s='%s' ",attr.first.c_str(),attr.second.c_str());
      }
      fprintf(stderr,"])\n");
    }
  }
  if (yyextra->isEnd || yyextra->selfClose)
  {
    if (yy_flex_debug)
    {
      fprintf(stderr,"%d: endElement(%s)\n",yyextra->lineNr,yyextra->name.data());
    }
    checkAndUpdatePath(yyscanner);
    if (yyextra->handlers.endElement)
    {
      yyextra->handlers.endElement(yyextra->name);
    }
  }
}

static std::string trimSpaces(const std::string &str)
{
  const int l = static_cast<int>(str.length());
  int s=0, e=l-1;
  while (s<l && isspace(str.at(s))) s++;
  while (e>s && isspace(str.at(e))) e--;
  return str.substr(s,1+e-s);
}

static void addCharacters(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  std::string data = trimSpaces(yyextra->data);
  if (!yyextra->encoding.empty() && !yyextra->transcodeFunc(data,yyextra->encoding.c_str()))
  {
    reportError(yyscanner,"failed to transcode string '"+data+"' from encoding '"+yyextra->encoding+"' to UTF-8");
  }
  if (yyextra->handlers.characters)
  {
    yyextra->handlers.characters(data);
  }
  if (!data.empty())
  {
    if (yy_flex_debug)
    {
      fprintf(stderr,"characters(%s)\n",data.c_str());
    }
  }
}

static void addAttribute(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  std::string val = yyextra->attrValue;
  if (!yyextra->encoding.empty() && !yyextra->transcodeFunc(val,yyextra->encoding.c_str()))
  {
    reportError(yyscanner,"failed to transcode string '"+val+"' from encoding '"+yyextra->encoding+"' to UTF-8");
  }
  yyextra->attrs.insert(std::make_pair(yyextra->attrName,val));
}

static void reportError(yyscan_t yyscanner,const std::string &msg)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yy_flex_debug)
  {
    fprintf(stderr,"%s:%d: Error '%s'\n",yyextra->fileName.c_str(),yyextra->lineNr,msg.c_str());
  }
  if (yyextra->handlers.error)
  {
    yyextra->handlers.error(yyextra->fileName,yyextra->lineNr,msg);
  }
}

static const char *entities_enc[] = { "amp", "quot", "gt", "lt", "apos" };
static const char  entities_dec[] = { '&',   '"',    '>',  '<',  '\''   };
static const int   num_entities = 5;

// replace character entities such as &amp; in txt and return the string where entities
// are replaced
static std::string processData(yyscan_t yyscanner,const char *txt,yy_size_t len)
{
  std::string result;
  result.reserve(len);
  for (yy_size_t i=0; i<len; i++)
  {
    char c = txt[i];
    if (c=='&')
    {
      const int maxEntityLen = 10;
      char entity[maxEntityLen+1];
      entity[maxEntityLen]='\0';
      for (yy_size_t j=0; j<maxEntityLen && i+j+1<len; j++)
      {
        if (txt[i+j+1]!=';')
        {
          entity[j]=txt[i+j+1];
        }
        else
        {
          entity[j]=0;
          break;
        }
      }
      bool found=false;
      for (int e=0; !found && e<num_entities; e++)
      {
        if (strcmp(entity,entities_enc[e])==0)
        {
          result+=entities_dec[e];
          i+=strlen(entities_enc[e])+1;
          found=true;
        }
      }
      if (!found)
      {
        std::string msg = std::string("Invalid character entity '&") + entity + ";' found\n";
        reportError(yyscanner,msg);
      }
    }
    else
    {
      result+=c;
    }
  }
  return result;
}

//--------------------------------------------------------------

struct XMLParser::Private
{
  yyscan_t yyscanner;
  struct xmlYY_state xmlYY_extra;
};

XMLParser::XMLParser(const XMLHandlers &handlers) : p(new Private)
{
  xmlYYlex_init_extra(&p->xmlYY_extra,&p->yyscanner);
  p->xmlYY_extra.handlers = handlers;
}

XMLParser::~XMLParser()
{
  xmlYYlex_destroy(p->yyscanner);
}

void XMLParser::parse(const char *fileName,
                      const char *inputStr,
                      bool debugEnabled,
                      std::function<void()> debugStart,
                      std::function<void()> debugEnd,
                      std::function<Transcode> transcodeFunc)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

#ifdef FLEX_DEBUG
  xmlYYset_debug(debugEnabled?1:0,p->yyscanner);
#endif

  if (inputStr==nullptr || inputStr[0]=='\0') return; // empty input

  debugStart();

  BEGIN(Initial);
  yyextra->fileName      = fileName;
  yyextra->lineNr        = 1;
  yyextra->inputString   = inputStr;
  yyextra->inputPosition = 0;
  yyextra->transcodeFunc = transcodeFunc;

  if (static_cast<unsigned char>(inputStr[0])==0xEF &&
      static_cast<unsigned char>(inputStr[1])==0xBB &&
      static_cast<unsigned char>(inputStr[2])==0xBF)
  {
    yyextra->inputPosition = 3; // remove UTF-8 BOM
  }

  xmlYYrestart( 0, yyscanner );

  if (yyextra->handlers.startDocument)
  {
    yyextra->handlers.startDocument();
  }
  xmlYYlex(yyscanner);
  if (yyextra->handlers.endDocument)
  {
    yyextra->handlers.endDocument();
  }

  if (!yyextra->xpath.empty())
  {
    std::string tagName = yyextra->xpath.back();
    std::string msg = "End of file reached while expecting closing tag '"+tagName+"'";
    reportError(yyscanner,msg);
  }

  debugEnd();
}

int XMLParser::lineNr() const
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  return yyextra->lineNr;
}

std::string XMLParser::fileName() const
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  return yyextra->fileName;
}

#if USE_STATE2STRING
#include "xml.l.h"
#endif
