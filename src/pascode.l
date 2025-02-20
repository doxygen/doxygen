/******************************************************************************
 *
 * Copyright (C) 1997-2025 by Dimitri van Heesch.
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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada)
 *  in Spring 2005 as part of CS 179E: Compiler Design Project
 *  at the University of California, Riverside; the course was
 *  taught by Peter H. Froehlich <phf@acm.org>.
 */
 
%option never-interactive
%option prefix="pascodeYY"
%option reentrant
%option extra-type="struct pascodeYY_state *"
%option noyy_top_state
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

#include <vector>
#include <unordered_map>
#include <string>
#include <stack>

#include <stdio.h>

#include "pascode.h"
#include "message.h"
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "tooltip.h"
#include "scopedtypevariant.h"
#include "symbolresolver.h"
#include "debug.h"

# define DBG_CTX(x) fprintf x

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct pascodeYY_state
{
  std::unordered_map< std::string, ScopedTypeVariant > codeClassMap;
  QCString      curClassName;
  StringVector  curClassBases;

  OutputCodeList * code = nullptr;
  
  const char *  inputString = nullptr;     //!< the code fragment as text
  int           inputPosition = 0;   //!< read offset during parsing
  QCString      fileName;

  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number

  const char *  currentFontClass = nullptr;
  bool          insideCodeLine = FALSE;
  bool          insideSpecialComment = false;
  
  const FileDef *    sourceFileDef = nullptr;
  const Definition * currentDefinition = nullptr;
  const MemberDef *  currentMemberDef  = nullptr;
  
  bool          insideBody = false;
  bool          includeCodeFragment = FALSE;
  bool          searchingForBody = FALSE;
  
  int           bodyCurlyCount = 0;
  
  QCString      docBlock;     //!< contents of all lines of a documentation block
  QCString      realScope;
  
  QCString      type;
  QCString      name;
  
  bool          endComment = FALSE;
  
  VariableContext theVarContext;
  CallContext theCallContext;
  SymbolResolver symbolResolver;
  TooltipManager tooltipManager;
  std::vector<const Definition *> foldStack;
};

//-------------------------------------------------------------------
[[maybe_unused]] static const char *stateToString(int state);

static void startCodeLine(yyscan_t yyscanner);
static void codify(yyscan_t yyscanner,const QCString &text);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static void endFontClass(yyscan_t yyscanner,bool specialComment=false);

static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static inline void pop_state(yyscan_t yyscanner);

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

PAS1CS            \(\*
PAS1CE            \*\)
PAS2CS            \{
PAS2CE            \}
PAS3CP            \/\/

PASMC1            ({PAS1CS}\$("define"|"if"|"else"|"endif").*{PAS1CE})
PASMC2            ({PAS2CS}\$("define"|"if"|"else"|"endif").*{PAS2CE})
PASMC3            ({PAS3CP}\$("define"|"if"|"else"|"endif").*\n$)

PASMC             (PASMC1|PASMC2|PASMC3)

WS                [ \t]*|{PASMC}|(({PAS1CS}.*{PAS1CE})|({PAS2CS}.*{PAS2CE})|{PAS3CP})
NEWLINE           \n

DIGIT             [0-9]
LETTER            [A-Za-z\x80-\xFF]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*

%option noyywrap
%option stack

%x Body

%%

<Body>\n    {
    yyextra->yyLineNr += 1;
    std::cerr << "[3] newline" <<
    std::endl;
}
<Body>{WS}* {
    std::cerr << "[1] whitespace" <<
    std::endl;
}
<Body>{WS}{IDENTIFIER} {
    std::cerr << "[2] white with ident: " << yytext <<
    std::endl;
}
%%

//-------------------------------------------------------------------------------

static int yyread(yyscan_t yyscanner, char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  int c=0;
  while( c < max_size && *s )
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}

//-------------------------------------------------------------------------------

/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c=0;
  int count=1;
  while ((c=*p))
  {
    p++;
    if (c=='\n') count++;
  }
  if (p>yyextra->inputString && *(p-1)!='\n')
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++;
  }
  return count;
}

//-------------------------------------------------------------------------------
static void codeFolding(yyscan_t yyscanner,const Definition *d)
{
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    while (!yyextra->foldStack.empty())
    {
      const Definition *dd = yyextra->foldStack.back();
      if (dd->getEndBodyLine()+1==yyextra->yyLineNr) // +1 to close the section after the end of the body
      {
        yyextra->code->endFold();
        //printf("%d:   end codeFolding for %s [%d..%d]\n",yyextra->yyLineNr,qPrint(dd->name()),dd->getStartDefLine(),dd->getEndBodyLine());
        yyextra->foldStack.pop_back();
      }
      else
      {
        break;
      }
    }
  }
}

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //if (yyextra->currentFontClass) { yyextra->code->endFontClass(yyscanner); }
  if (yyextra->sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",yyextra->yyLineNr);
    //lineAnchor.sprintf("l%05d",yyextra->yyLineNr);

    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    //printf("%s:startCodeLine(%d)=%p\n",qPrint(yyextra->sourceFileDef->name()),yyextra->yyLineNr,(void*)d);
    DBG_CTX((stderr,"%s:startCodeLine(%d)=%p\n",qPrint(yyextra->sourceFileDef->name()),yyextra->yyLineNr,(void*)d));
    if (!yyextra->includeCodeFragment && d)
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->insideBody = FALSE;
      yyextra->searchingForBody = TRUE;
      yyextra->realScope = d!=Doxygen::globalScope ? d->name() : "";
      yyextra->type.clear();
      yyextra->name.clear();
      DBG_CTX((stderr,"Real scope: '%s'\n",qPrint(yyextra->realScope)));
      yyextra->bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
    }
    else
    {
      codeFolding(yyscanner,nullptr);
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr,
                                     !yyextra->includeCodeFragment);
    }
  }
  DBG_CTX((stderr,"startCodeLine(%d)\n",yyextra->yyLineNr));
  yyextra->code->startCodeLine(yyextra->yyLineNr);
  yyextra->insideCodeLine = true;  
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(QCString(yyextra->currentFontClass));
  }
}

//-------------------------------------------------------------------------------

static void codify(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->code->codify(text);
}

static void endFontClass(yyscan_t yyscanner,bool specialComment)
{
#if 0
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=nullptr;
  }
  if (specialComment && yyextra->insideSpecialComment)
  {
    yyextra->code->endSpecialComment();
    yyextra->insideSpecialComment=false;
  }
#endif
}

static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"endCodeLine(%d)\n",yyextra->yyLineNr));
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
  yyextra->insideCodeLine = false;
}

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char * fc = yyextra->currentFontClass;
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

//-------------------------------------------------------------------------------

static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty()) return;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  const char *p=text.data(),*sp=p;
  char c=0;
  bool done=FALSE;
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
      done=TRUE;
    }
  }
}

//-------------------------------------------------------------------------------

struct PascalCodeParser::Private
{
  yyscan_t yyscanner;
  pascodeYY_state state;
};

PascalCodeParser::PascalCodeParser() : p(std::make_unique<Private>())
{
  pascodeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  pascodeYYset_debug(Debug::isFlagSet(Debug::Lex_pascode)?1:0,p->yyscanner);
#endif
  resetCodeParserState();
}

PascalCodeParser::~PascalCodeParser()
{
  pascodeYYlex_destroy(p->yyscanner);
}

void PascalCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->codeClassMap.clear();
  
  std::cerr << "parse file {" << yyextra->fileName << "}..." <<
  std::endl;

  BEGIN( Body );
}

//-------------------------------------------------------------------------------

void PascalCodeParser::parseCode(OutputCodeList &codeOutIntf,
    const QCString &/* scopeName */,
    const QCString &input,
    SrcLangExt /*lang*/,
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
    bool collectXRefs
    )
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  //printf("***parseCode(%s)\n",qPrint(input));

  codeOutIntf.stripCodeComments(stripCodeComments);
  if (input.isEmpty()) return;
  DebugLex debugLex(Debug::Lex_pascode, __FILE__, fileDef ? qPrint(fileDef->fileName()): nullptr);
  yyextra->fileName      = fileDef ? fileDef->fileName():"";
  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
}

static inline void pop_state(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if ( yyg->yy_start_stack_ptr <= 0 )
    warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected statement '{}'",yytext );
  else
    yy_pop_state(yyscanner);
}

#include "pascode.l.h"
