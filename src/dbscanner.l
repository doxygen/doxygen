/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
%option prefix="dbscannerYY"
%option reentrant
%option extra-type="struct dbscannerYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

/*
 *      includes
 */

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "dbscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "arguments.h"
#include "markdown.h"
#include "fileinfo.h"
#include "debug.h"
#include "stringutil.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define unput_string(yytext,yyleng) do { for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]); } while(0)

/* ----------------------------------------------------------------- */

struct dbscannerYY_state
{
  CommentScanner          commentScanner;
  OutlineParserInterface *thisParser = nullptr;
  const char *            inputString = nullptr;
  int                     inputPosition = 0;
  Protection              protection = Protection::Public;
  std::shared_ptr<Entry>  current_root;
  std::shared_ptr<Entry>  current;
  std::shared_ptr<Entry>  previous;
  std::shared_ptr<Entry>  bodyEntry;
  int                     yyLineNr = 1 ;
  QCString                fileName;
  MethodTypes             mtype = MethodTypes::Method;
  bool                    isStatic = FALSE;
  Specifier               virt = Specifier::Normal;
  int                     docBlockContext = 0;
  QCString                docBlock;
  bool                    docBlockInBody = FALSE;
  bool                    docBlockJavaStyle = FALSE;
  bool                    docBrief = FALSE;
  bool                    docBlockSpecial = FALSE;
  bool                    doubleQuote = FALSE;
  bool                    specialBlock = FALSE;
  int                     stringContext = 0;
  TextStream *            copyString = nullptr;
  int                     indent = 0;
  int                     curIndent = 0;
  int                     commentIndent = 0;
  bool                    importTuple = FALSE;
  StringUnorderedMap      packageNameCache;
  char                    atomStart = 0;
  char                    atomEnd = 0;
  int                     atomCount = 0;
  int                     atomContext = 0;
  QCString                moduleScope;
  QCString                packageName;
  TextStream              defVal;
  int                     braceCount = 0;
  bool                    lexInit = FALSE;
  bool                    packageCommentAllowed = FALSE;
  bool                    startInit = FALSE;
  int                     searchCount = 0;
  QCString                argType;
  bool                    funcParamsEnd = FALSE;
  std::vector<QCString>   decorators;
  QCString                programStr;
  TextStream              decoratorCommentStr;
  int                     decoratorRound = 0;
  bool                    checkDupEntry = false;
  bool                    firstPass = true;
};

//-----------------------------------------------------------------------------
[[maybe_unused]] static const char *stateToString(int state);

static int yyread(yyscan_t yyscanner,char *buf,int max_size);

//-----------------------------------------------------------------------------
/* ----------------------------------------------------------------- */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}


WS                [ \t]*
NEWLINE           \n

DIGIT             [0-9]
LETTER            [A-Za-z\x80-\xFF]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*

%option noyywrap

%x Search

%%

<Search>{
    {WS}{IDENTIFIER}
}

%%

//----------------------------------------------------------------------------

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  const char *p = yyextra->inputString + yyextra->inputPosition;
  while ( c < max_size && *p ) { *buf++ = *p++; c++; }
  yyextra->inputPosition+=c;
  return c;
}

static void initParser(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->protection = Protection::Public;
  yyextra->mtype = MethodTypes::Method;
  yyextra->isStatic = FALSE;
  yyextra->virt = Specifier::Normal;
  yyextra->previous = 0;
  yyextra->packageCommentAllowed = TRUE;
}

//----------------------------------------------------------------------------

static void parseMain(yyscan_t yyscanner, const QCString &fileName,const char *fileBuf,const std::shared_ptr<Entry> &rt)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  initParser(yyscanner);

  if (fileBuf==nullptr || fileBuf[0]=='\0') return;

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;

  yyextra->protection    = Protection::Public;
  yyextra->mtype         = MethodTypes::Method;
  yyextra->isStatic      = false;
  yyextra->virt          = Specifier::Normal;
  yyextra->current_root  = rt;
  yyextra->specialBlock  = false;

  yyextra->yyLineNr      = 1 ;
  yyextra->fileName      = fileName;
  yyextra->checkDupEntry = false;
  yyextra->firstPass     = true;
  //setContext();
  msg("Parsing file {}...\n",yyextra->fileName);

  BEGIN( Search );

  dbscannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->commentScanner.leaveFile(yyextra->fileName,yyextra->yyLineNr);

  yyextra->programStr.clear();
  yyextra->current_root->program.str(std::string());
}

static void parsePrototype(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("**** parsePrototype(%s) begin\n",qPrint(text));
  if (text.isEmpty())
  {
    warn(yyextra->fileName,yyextra->yyLineNr,"Empty prototype found!");
    return;
  }

  // save scanner state
  YY_BUFFER_STATE orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(nullptr, YY_BUF_SIZE, yyscanner), yyscanner);

  const char *orgInputString = yyextra->inputString;
  int orgInputPosition = yyextra->inputPosition;

  // set new string
  yyextra->inputString = text.data();
  yyextra->inputPosition = 0;
  dbscannerYYrestart( nullptr, yyscanner );
  
  BEGIN( Search );
  
  dbscannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->current->name = yyextra->current->name.stripWhiteSpace();
  if (yyextra->current->section.isMemberDoc() && yyextra->current->args.isEmpty())
  {
    yyextra->current->section = EntryType::makeVariableDoc();
  }

  // restore original scanner state

  yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);
  yy_switch_to_buffer(orgState, yyscanner);

  yyextra->inputString = orgInputString;
  yyextra->inputPosition = orgInputPosition;
}

//----------------------------------------------------------------------------

struct dBaseOutlineParser::Private
{
  yyscan_t yyscanner;
  dbscannerYY_state state;
};

dBaseOutlineParser::dBaseOutlineParser() : p(std::make_unique<dBaseOutlineParser::Private>())
{
  dbscannerYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  dbscannerYYset_debug(Debug::isFlagSet(Debug::Lex_dbscanner)?1:0,p->yyscanner);
#endif
}

dBaseOutlineParser::~dBaseOutlineParser()
{
  dbscannerYYlex_destroy(p->yyscanner);
}


void dBaseOutlineParser::parseInput(const QCString &fileName,
                                     const char *fileBuf,
                                     const std::shared_ptr<Entry> &root,
                                     ClangTUParser * /*clangParser*/)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->thisParser = this;
  DebugLex debugLex(Debug::Lex_dbscanner, __FILE__, qPrint(fileName));
  ::parseMain(p->yyscanner, fileName,fileBuf,root);

  // May print the AST for debugging purposes
  // printAST(global_root);
}

bool dBaseOutlineParser::needsPreprocessing(const QCString &) const
{
  return FALSE;
}

void dBaseOutlineParser::parsePrototype(const QCString &text)
{
  ::parsePrototype(p->yyscanner,text);
}

//----------------------------------------------------------------------------

#include "dbscanner.l.h"
