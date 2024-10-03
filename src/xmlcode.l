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
 * Parser for syntax highlighting and references for XML
 * written by Weston Thayer
 ******************************************************************************/

%option never-interactive
%option prefix="xmlcodeYY"
%option reentrant
%option extra-type="struct xmlcodeYY_state *"
%option noyy_top_state
%option nounput
%option noyywrap
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

#include <stdio.h>

#include "xmlcode.h"

#include "entry.h"
#include "doxygen.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "config.h"
#include "filedef.h"
#include "tooltip.h"
#include "message.h"
#include "debug.h"

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct xmlcodeYY_state
{
  OutputCodeList * code;
  QCString      curClassName;
  QCString      parmType;
  QCString      parmName;
  const char *  inputString = nullptr;     //!< the code fragment as text
  int           inputPosition = 0;   //!< read offset during parsing
  QCString      fileName;
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  bool          insideCodeLine = false;
  const Definition   *searchCtx = nullptr;

  bool          stripCodeComments = true;
  bool          exampleBlock = false;
  QCString      exampleName;
  QCString      exampleFile;

  QCString      type;
  QCString      name;
  QCString      args;
  QCString      classScope;

  QCString      CurrScope;

  std::unique_ptr<FileDef> exampleFileDef;
  const FileDef *     sourceFileDef = nullptr;
  const Definition *  currentDefinition = nullptr;
  const MemberDef *   currentMemberDef = nullptr;
  bool          includeCodeFragment = false;
  const char *  currentFontClass = nullptr;
};

[[maybe_unused]] static const char *stateToString(int state);

static void codify(yyscan_t yyscanner,const char* text);
static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void startCodeLine(yyscan_t yyscanner);
static void endFontClass(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const char *text);
static void startFontClass(yyscan_t yyscanner,const char *s);
static int  countLines(yyscan_t yyscanner);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

nl          (\r\n|\r|\n)
ws          [ \t]+
open        "<"
close       ">"
namestart   [A-Za-z\200-\377_]
namechar    [:A-Za-z\200-\377_0-9.-]
esc         "&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
name        {namestart}{namechar}*
comment     {open}"!--"([^-]|"-"[^-])*"--"{close}
cdata       {open}"![CDATA["([^\]]|"\]"[^\]])*"]]"{close}
data        "random string"
string      \"([^\"&]|{esc})*\"|\'([^'&]|{esc})*\'

%option noyywrap
%option nounput

%%

<INITIAL>{ws}       {
                        codifyLines(yyscanner,yytext);
                    }
<INITIAL>"/"        {
                        endFontClass(yyscanner);
                        codify(yyscanner,yytext);
                    }
<INITIAL>"="        {
                        endFontClass(yyscanner);
                        codify(yyscanner,yytext);
                    }
<INITIAL>{close}    {
                        endFontClass(yyscanner);
                        codify(yyscanner,yytext);
                    }
<INITIAL>{name}     {
                        startFontClass(yyscanner,"keyword");
                        codify(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }
<INITIAL>{string}   {
                        startFontClass(yyscanner,"stringliteral");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }
{cdata}             {
                        startFontClass(yyscanner,"xmlcdata");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }

{open}{ws}?{name}   {
                        // Write the < in a different color
                        char openBracket[] = { yytext[0], '\0' };
                        codify(yyscanner,openBracket);

                        // Then write the rest
                        yytext++;
                        startFontClass(yyscanner,"keywordtype");
                        codify(yyscanner,yytext);
                        endFontClass(yyscanner);

                        BEGIN(INITIAL);
                    }
{open}{ws}?"/"{name} {
                        // Write the "</" in a different color
                        char closeBracket[] = { yytext[0], yytext[1], '\0' };
                        endFontClass(yyscanner);
                        codify(yyscanner,closeBracket);

                        // Then write the rest
                        yytext++; // skip the '<'
                        yytext++; // skip the '/'
                        startFontClass(yyscanner,"keywordtype");
                        codify(yyscanner,yytext);
                        endFontClass(yyscanner);

                        BEGIN(INITIAL);
                    }
{comment}           {
                        // Strip off the extra '!'
                        // yytext++; // <
                        // *yytext = '<'; // replace '!' with '<'

                        startFontClass(yyscanner,"comment");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }
{nl}                {
                        codifyLines(yyscanner,yytext);
                    }

.                   {
                        //printf("!ERROR(%c)\n", *yytext);
                        codifyLines(yyscanner,yytext);
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

static void codify(yyscan_t yyscanner,const char* text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->code->codify(text);
}

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex.enabled())
  {
    if (yyextra->searchCtx)
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),false);
    }
    else
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->sourceFileDef,anchor,true);
    }
  }
}

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is true. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->sourceFileDef)
  {
    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);

    if (!yyextra->includeCodeFragment && d && d->isLinkableInProject())
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      //yyextra->insideBody = false;
      yyextra->classScope = d->name();
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                            yyextra->currentMemberDef->getOutputFileBase(),
                            yyextra->currentMemberDef->anchor(),yyextra->yyLineNr,
                            !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else
      {
        yyextra->code->writeLineNumber(d->getReference(),
                            d->getOutputFileBase(),
                            QCString(),yyextra->yyLineNr,
                            !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr,
                                     !yyextra->includeCodeFragment);
    }
  }

  yyextra->code->startCodeLine(yyextra->yyLineNr);
  yyextra->insideCodeLine = true;

  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
}

static void endFontClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=0;
  }
}

static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
  yyextra->insideCodeLine = false;
}

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *fc = yyextra->currentFontClass;
  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
  if (yyextra->yyLineNr<yyextra->inputLines)
  {
    yyextra->currentFontClass = fc;
    startCodeLine(yyscanner);
  }
}

static void codifyLines(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=text,*sp=p;
  char c;
  bool done=false;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      size_t l = static_cast<size_t>(p-sp-1);
      std::string tmp(sp,l);
      yyextra->code->codify(tmp.c_str());
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(sp);
      done=true;
    }
  }
}

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->startFontClass(s);
  yyextra->currentFontClass=s;
}

/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c;
  int count=1;
  while ((c=*p))
  {
    p++ ;
    if (c=='\n') count++;
  }
  if (p>yyextra->inputString && *(p-1)!='\n')
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++;
  }
  return count;
}

//----------------------------------------------------------------------------------------

struct XMLCodeParser::Private
{
  yyscan_t yyscanner;
  xmlcodeYY_state state;
};

XMLCodeParser::XMLCodeParser() : p(std::make_unique<Private>())
{
  xmlcodeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  xmlcodeYYset_debug(Debug::isFlagSet(Debug::Lex_xmlcode)?1:0,p->yyscanner);
#endif
  resetCodeParserState();
}

XMLCodeParser::~XMLCodeParser()
{
  xmlcodeYYlex_destroy(p->yyscanner);
}

void XMLCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->currentDefinition = nullptr;
  yyextra->currentMemberDef = nullptr;
}

void XMLCodeParser::parseCode(OutputCodeList &codeOutIntf,
               const QCString &/* scopeName */,
               const QCString &input,
               SrcLangExt,
               bool stripCodeComments,
               bool isExampleBlock,
               const QCString &exampleName,
               const FileDef *fileDef,
               int startLine,
               int endLine,
               bool inlineFragment,
               const MemberDef * /* memberDef */,
               bool /* showLineNumbers */,
               const Definition *searchCtx,
               bool/* collectXRefs */ 
              )
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  if (input.isEmpty()) return;

  DebugLex debugLex(Debug::Lex_xmlcode, __FILE__, fileDef ? qPrint(fileDef->fileName()): nullptr);
  yyextra->fileName      = fileDef ? fileDef->fileName():"";

  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
  yyextra->currentFontClass = nullptr;
  yyextra->insideCodeLine = false;
  yyextra->searchCtx = searchCtx;

  if (startLine!=-1)
    yyextra->yyLineNr    = startLine;
  else
    yyextra->yyLineNr    = 1;

  if (endLine!=-1)
    yyextra->inputLines  = endLine+1;
  else
    yyextra->inputLines  = yyextra->yyLineNr + countLines(yyscanner) - 1;

  yyextra->stripCodeComments = stripCodeComments;
  yyextra->exampleBlock  = isExampleBlock;
  yyextra->exampleName   = exampleName;
  yyextra->sourceFileDef = fileDef;

  if (isExampleBlock && fileDef==0)
  {
    // create a dummy filedef for the example
    yyextra->exampleFileDef = createFileDef("",(!exampleName.isEmpty()?exampleName:QCString("generated")));
    yyextra->sourceFileDef = yyextra->exampleFileDef.get();
  }

  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }

  yyextra->includeCodeFragment = inlineFragment;
  // Starts line 1 on the output
  startCodeLine(yyscanner);

  xmlcodeYYrestart( nullptr, yyscanner );

  xmlcodeYYlex(yyscanner);

  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
  if (yyextra->exampleFileDef)
  {
    // delete the temporary file definition used for this example
    yyextra->exampleFileDef.reset();
    yyextra->sourceFileDef=nullptr;
  }
}


#include "xmlcode.l.h"
