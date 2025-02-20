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
%option prefix="passcannerYY"
%option reentrant
%option extra-type="struct passcannerYY_state *"
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

#include "passcanner.h"
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

# define DBG_CTX(x) fprintf x

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define unput_string(yytext,yyleng) do { for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]); } while(0)

/* ----------------------------------------------------------------- */

struct passcannerYY_state
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

PAS1CS              "(*"
PAS1CE              "*)"
PAS2CS              \{
PAS2CE              \}
PAS3CC              "//"

CDEFINE             "define"
CIF                 "if"
CELSE               "else"
CENDIF              "endif"

WS                  [ \t]
NEWLINE             \n

DIGIT               [0-9]
LETTER              [A-Za-z\x80-\xFF]
IDENTIFIER          ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*

ASSIGN              \:\=
NOT_EQUAL           \<\>

SEMICOLON           \;
COMMA               \,

SUB                 \-
ADD                 \+
MUL                 \*
DIV                 \/

LW                  \<
EQ                  \=
GT                  \>
LW_EQ               \<\=
GT_EQ               \>\=

PAREN_OPEN          \(
PAREN_CLOSE         \)

A                   [a|A]
B                   [b|B]
C                   [c|C]
D                   [d|D]
E                   [e|E]
F                   [f|F]
G                   [g|G]
H                   [h|H]
I                   [i|I]
J                   [j|J]
K                   [k|K]
L                   [l|L]
M                   [m|M]
N                   [n|N]
O                   [o|O]
P                   [p|P]
Q                   [q|Q]
R                   [r|R]
S                   [s|S]
T                   [t|T]
U                   [u|U]
V                   [v|V]
W                   [w|W]
X                   [x|X]
Y                   [y|Y]
Z                   [z|Z]

UNIT                ({U}{N}{I}{T})
LIBRARY             ({L}{I}{B}{R}{A}{R}{Y})
PROGRAM             ({P}{R}{O}{G}{R}{A}{M})

PROCEDURE           ({P}{R}{O}{C}{E}{D}{U}{R}{E})
FUNCTION            ({F}{U}{N}{C}{T}{I}{O}{N})

BEGIN               ({B}{E}{G}{I}{N})
END                 ({E}{N}{D})
RESULT              ({R}{E}{S}{U}{L}{T})
NIL                 ({N}{I}{L})

IF                  ({I}{F})
ELSE                ({E}{L}{S}{E})
THEN                ({T}{H}{E}{N})

FOR                 ({F}{O}{R})
TO                  ({T}{O})
DOWNTO              ({D}{O}{W}{N}{T}{O})

WHILE               ({W}{H}{I}{L}{E})
DO                  ({D}{O})
BREAK               ({B}{R}{E}{A}{K})

REPEAT              ({R}{E}{P}{E}{A}{T})
UNTIL               ({U}{N}{T}{I}{L})

USES                ({U}{S}{E}{S})
INTERFACE           ({I}{N}{T}{E}{R}{F}{A}{C}{E})
IMPLEMENTATION      ({I}{M}{P}{L}{E}{M}{E}{N}{T}{A}{T}{I}{O}{N})

TYPE                ({T}{Y}{P}{E})
VAR                 ({V}{A}{R})
CONST               ({C}{O}{N}{S}{T})

CLASS               ({C}{L}{A}{S}{S})
PRIVATE             ({P}{R}{I}{V}{A}{T}{E})
PUBLIC              ({P}{U}{B}{L}{I}{C})
PUBLISHED           ({P}{U}{B}{L}{I}{S}{H}{E}{D})
PROTECTED           ({P}{R}{O}{T}{E}{C}{T}{E}{D})
RECORD              ({R}{E}{C}{O}{R}{D})

OVERLOAD            ({O}{V}{E}{R}{L}{O}{A}{D})
OVERRIDE            ({O}{V}{E}{R}{R}{I}{D}{E})
REINTRODUCE         ({R}{E}{I}{N}{T}{R}{O}{D}{U}{C}{E})
INHERITED           ({I}{N}{H}{E}{R}{I}{T}{E}{D})

INITIALIZATION      ({I}{N}{I}{T}{I}{A}{L}{I}{Z}{A}{T}{I}{O}{N})
FINALZATION         ({F}{I}{N}{A}{L}{Z}{A}{T}{I}{O}{N})

%option noyywrap

%x COMMENT

%x Body
%x BodyModuleIdent
%x BodyModuleIdentEnd

%x Interface
%x Implementation

%%

{NEWLINE}                           {   yyextra->yyLineNr += 1; }

<COMMENT>[^*]*                      ;   // all chars, except: *
<COMMENT>"*"[^)]                    ;   // * next with no )
<COMMENT>"*)"                       {
    BEGIN(Body);                        // switch to mode: INITIAL
}
<COMMENT>{NEWLINE}                  {
    yyextra->yyLineNr += 1;
}
<COMMENT><<EOF>>                    {
    std::cerr << "comment not terminated." <<
    std::endl;
    exit(1);
}

<Body>{PROGRAM}|{LIBRARY}|{UNIT}    {
    std::string ident = std::string(yytext);
    if (ident == std::string("program")
    ||  ident == std::string("library")
    ||  ident == std::string("unit")) {
        std::cerr
            << "line: "
            << yyextra->yyLineNr
            << ": is a: "
            << ident
            << ": ";
        BEGIN(BodyModuleIdent);
    }   else {
        std::cerr
            << "line: "
            << yyextra->yyLineNr
            << ": error: not a comform pascal syntax."
            <<
        std::endl;
        exit(1);
    }
}
<Body>"(*"                          {
    BEGIN(COMMENT); // switch to mode: COMMENT
}
<Body>\n                            { yyextra->yyLineNr += 1; }
<Body>.                             {
    std::cerr
        << "line: "
        << yyextra->yyLineNr
        << ": unknown character found."
        <<
    std::endl;
    exit(1);
}
<Body><<EOF>>                       {
    std::cerr
        << "expected: PROGRAM, LIBRARY or UNIT"
        << "file reached end."
        <<
    std::endl;
    exit(1);
}
<BodyModuleIdent>{IDENTIFIER}       {
    auto buffer = new char[255];
    strcpy(buffer, yytext);
    
    std::cerr << buffer << std::endl;
    delete [] buffer;
    
    BEGIN(BodyModuleIdentEnd);
}
<BodyModuleIdent>{WS}*              ;
<BodyModuleIdent>{NEWLINE}          { yyextra->yyLineNr += 1; }
<BodyModuleIdent><<EOF>>            {
    std::cerr << "error: module missing Semicolon (;) at end." <<
    std::endl;
    exit(1);
}
<BodyModuleIdentEnd>{SEMICOLON}     { BEGIN(INITIAL); }
<BodyModuleIdentEnd>{WS}*           ;
<BodyModuleIdentEnd>{NEWLINE}       { yyextra->yyLineNr += 1; }
<BodyModuleIdentEnd><<EOF>>         {
    std::cerr << "error: module missing semicolon (;) at end." <<
    std::endl;
    exit(1);
}

%%

//----------------------------------------------------------------------------

static int
yyread(
    yyscan_t yyscanner,
    char     *buf,
    int      max_size)
{
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    int c = 0;
    
    const char *p = yyextra->inputString + yyextra->inputPosition;
    while ( c < max_size && *p ) { *buf++ = *p++; c++; }
    yyextra->inputPosition+=c;
    
    return c;
}

static void
initParser(
    yyscan_t yyscanner)
{
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

    yyextra->protection   = Protection::Public;
    yyextra->mtype        = MethodTypes::Method;
    yyextra->isStatic     = FALSE;
    yyextra->virt         = Specifier::Normal;
    yyextra->previous     = 0;
    yyextra->packageCommentAllowed = TRUE;
}

//----------------------------------------------------------------------------

static void
parseMain(
    yyscan_t yyscanner,
    const    QCString &fileName,
    const    char     *fileBuf,
    const    std::shared_ptr<Entry> &rt)
{
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    initParser(yyscanner);

    if (fileBuf == nullptr || fileBuf[0] == '\0') return;

    yyextra->inputString   = fileBuf;
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

    std::cerr << "lex file {" << yyextra->fileName << "}..." <<
    std::endl;
    BEGIN( Body );
    

    passcannerYYlex(yyscanner);
    yyextra->lexInit = TRUE;

    yyextra->commentScanner.leaveFile(yyextra->fileName,yyextra->yyLineNr);

    yyextra->programStr.clear();
    yyextra->current_root->program.str(std::string());
    
    exit(1);
}

static void
parsePrototype(
    yyscan_t yyscanner,
    const QCString &text)
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
    passcannerYYrestart( nullptr, yyscanner );

    std::cerr << "lex proto file {" << yyextra->fileName << "}..." <<
    std::endl;
    BEGIN( Body );

    passcannerYYlex(yyscanner);
    yyextra->lexInit=TRUE;

    yyextra->current->name = yyextra->current->name.stripWhiteSpace();
    if (yyextra->current->section.isMemberDoc() && yyextra->current->args.isEmpty()) {
        yyextra->current->section = EntryType::makeVariableDoc();
    }

    // restore original scanner state

    yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);
    yy_switch_to_buffer(orgState, yyscanner);

    yyextra->inputString = orgInputString;
    yyextra->inputPosition = orgInputPosition;
}

//----------------------------------------------------------------------------

struct PascalOutlineParser::Private
{
    yyscan_t yyscanner;
    passcannerYY_state state;
};

PascalOutlineParser::PascalOutlineParser() : p(std::make_unique<PascalOutlineParser::Private>())
{
    passcannerYYlex_init_extra(&p->state,&p->yyscanner);
    #ifdef FLEX_DEBUG
    passcannerYYset_debug(Debug::isFlagSet(Debug::Lex_passcanner)?1:0,p->yyscanner);
    #endif
}

PascalOutlineParser::~PascalOutlineParser()
{
    passcannerYYlex_destroy(p->yyscanner);
}


void
PascalOutlineParser::parseInput(
    const QCString &fileName,
    const char *fileBuf,
    const std::shared_ptr<Entry> &root, ClangTUParser * /*clangParser*/)
{
    struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
    yyextra->thisParser  = this;
    
    DebugLex debugLex(Debug::Lex_passcanner, __FILE__, qPrint(fileName));
    ::parseMain(p->yyscanner, fileName,fileBuf,root);

    // May print the AST for debugging purposes
    // printAST(global_root);
}

bool
PascalOutlineParser::needsPreprocessing(
    const QCString &
    ) const
{
    return FALSE;
}

void
PascalOutlineParser::parsePrototype(
    const QCString &text)
{
    ::parsePrototype(p->yyscanner,text);
}

//----------------------------------------------------------------------------

#include "passcanner.l.h"
