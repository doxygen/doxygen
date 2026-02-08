/* -*- mode: fundamental; indent-tabs-mode: 1; -*- */
/*****************************************************************************
 * Parser for Fortran90 F subset
 *
 * Copyright (C) by Anke Visser
 * based on the work of Dimitri van Heesch.
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

/* Developer notes.
 *
 * - Consider using startScope(), endScope() functions with  module, program,
 * subroutine or any other scope in fortran program.
 *
 * - Symbol yyextra->modifiers (attributes) are collected using SymbolModifiers |= operator during
 * substructure parsing. When substructure ends all yyextra->modifiers are applied to actual
 * entries in applyModifiers() functions.
 *
 * - How case insensitiveness should be handled in code?
 * On one side we have arg->name and entry->name, on another side modifierMap[name].
 * In entries and arguments case is the same as in code, in modifier map case is lowered and
 * then it is compared to lowered entry/argument names.
 *
 * - Do not like constructs like aa{BS} or {BS}bb. Should try to handle blank space
 * with separate rule?: It seems it is often necessary, because we may parse something like
 * "functionA" or "MyInterface". So constructs like '(^|[ \t])interface({BS_}{ID})?/[ \t\n]'
 * are desired.
 *
 * - Must track yyextra->lineNr when using REJECT, unput() or similar commands.
 */
%option never-interactive
%option case-insensitive
%option prefix="fortranscannerYY"
%option reentrant
%option extra-type="struct fortranscannerYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

#include <map>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "fortranscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "pre.h"
#include "arguments.h"
#include "debug.h"
#include "markdown.h"


// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

enum ScanVar { V_IGNORE, V_VARIABLE, V_PARAMETER, V_RESULT};
enum InterfaceType { IF_NONE, IF_SPECIFIC, IF_GENERIC, IF_ABSTRACT };

// {{{ ----- Helper structs -----
//! Holds yyextra->modifiers (ie attributes) for one symbol (variable, function, etc)
struct SymbolModifiers
{
  enum Protection {NONE_P, PUBLIC, PRIVATE};
  enum Direction {NONE_D, IN, OUT, INOUT};

  //! This is only used with function return value.
  QCString type, returnName;
  Protection protection;
  Direction direction;
  bool optional;
  bool protect;
  QCString dimension;
  bool allocatable;
  bool external;
  bool intrinsic;
  bool parameter;
  bool pointer;
  bool target;
  bool save;
  bool deferred;
  bool nonoverridable;
  bool nopass;
  bool pass;
  bool contiguous;
  bool volat; /* volatile is a reserved name */
  bool value; /* volatile is a reserved name */
  QCString passVar;
  QCString bindVar;

  SymbolModifiers() : type(), returnName(), protection(NONE_P), direction(NONE_D),
    optional(FALSE), protect(FALSE), dimension(), allocatable(FALSE),
    external(FALSE), intrinsic(FALSE), parameter(FALSE),
    pointer(FALSE), target(FALSE), save(FALSE), deferred(FALSE), nonoverridable(FALSE),
    nopass(FALSE), pass(FALSE), contiguous(FALSE), volat(FALSE), value(FALSE), passVar(),
    bindVar() {}

  SymbolModifiers& operator|=(const SymbolModifiers &mdfs);
  SymbolModifiers& operator|=(QCString mdfrString);
};

//ostream& operator<<(ostream& out, const SymbolModifiers& mdfs);

static const char *directionStrs[] =
{
   "", "intent(in)", "intent(out)", "intent(inout)"
};
static const char *directionParam[] =
{
   "", "[in]", "[out]", "[in,out]"
};

// }}}

struct CommentInPrepass
{
  CommentInPrepass(int col, const QCString &s) : column(col), str(s) {}
  int column;
  QCString str;
};

/* -----------------------------------------------------------------
 *
 *      statics
 */

struct fortranscannerYY_state
{
  OutlineParserInterface * thisParser;
  CommentScanner           commentScanner;
  const char *             inputString;
  int                      inputPosition;
  bool                     isFixedForm;
  QCString                 inputStringPrepass; ///< Input string for prepass of line cont. '&'
  QCString                 inputStringSemi; ///< Input string after command separator ';'
  unsigned int             inputPositionPrepass;
  int                      lineCountPrepass = 0;
  EntryList                subrCurrent;
  std::vector<CommentInPrepass>  comments;
  YY_BUFFER_STATE *        includeStack = nullptr;
  int                      includeStackPtr = 0;
  int                      includeStackCnt = 0;
  QCString                 fileName;
  int                      lineNr = 1 ;
  int                      colNr  = 0 ;
  Entry                   *current_root = nullptr;
  Entry                   *global_scope = nullptr;
  std::shared_ptr<Entry>   global_root;
  std::shared_ptr<Entry>   file_root;
  std::shared_ptr<Entry>   last_entry;
  std::shared_ptr<Entry>   last_enum;
  std::shared_ptr<Entry>   current;
  ScanVar                  vtype       = V_IGNORE; // type of parsed variable
  EntryList                moduleProcedures; // list of all interfaces which contain unresolved module procedures
  QCString                 docBlock;
  bool                     docBlockInBody = false;
  bool                     docBlockJavaStyle;
  QCString                 docBlockName;
  QCString                 blockString;
  int                      blockLineNr=-1;
  QCString                 debugStr;
  size_t                   fencedSize = 0;
//  Argument                *parameter; // element of parameter list
  QCString                 argType;  // fortran type of an argument of a parameter list
  QCString                 argName;  // last identifier name in variable list
  QCString                 initializer;  // initial value of a variable
  int                      initializerArrayScope;  // number if nested array scopes in initializer
  int                      initializerScope;  // number if nested function calls in initializer
  QCString                 useModuleName;  // name of module in the use statement
  Protection               defaultProtection;
  Protection               typeProtection;
  bool                     typeMode = false;
  InterfaceType            ifType = IF_NONE;
  bool                     functionLine = false;
  char                     stringStartSymbol; // single or double quote
  bool                     parsingPrototype = false; // see parsePrototype()

//! Accumulated modifiers of current statement, eg variable declaration.
  SymbolModifiers          currentModifiers;
//! Holds program scope->symbol name->symbol modifiers.
  std::map<Entry*,std::map<std::string,SymbolModifiers> > modifiers;
  int                      anonCount    = 0 ;

  int fixedCommentAfter = 72;
  //! counter for the number of main programs in this file
  int mainPrograms      = 0; 
  int curIndent         = 0;
};

//-----------------------------------------------------------------------------
static int getAmpersandAtTheStart(const char *buf, int length);
static int getAmpOrExclAtTheEnd(const char *buf, int length, char ch);
static QCString extractFromParens(const QCString &name);
static QCString extractBind(const QCString &name);


static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void startCommentBlock(yyscan_t yyscanner,bool);
static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief);
static void subrHandleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief);
static void subrHandleCommentBlockResult(yyscan_t yyscanner,const QCString &doc,bool brief);
static void addCurrentEntry(yyscan_t yyscanner,bool case_insens);
static void addModule(yyscan_t yyscanner,const QCString &name=QCString(), bool isModule=FALSE);
static void addSubprogram(yyscan_t yyscanner,const QCString &text);
static void addInterface(yyscan_t yyscanner,QCString name, InterfaceType type);
static Argument *getParameter(yyscan_t yyscanner,const QCString &name);
static void scanner_abort(yyscan_t yyscanner);
static inline void pop_state(yyscan_t yyscanner);

static void startScope(yyscan_t yyscanner,Entry *scope);
static bool endScope(yyscan_t yyscanner,Entry *scope, bool isGlobalRoot=FALSE);
static void copyEntry(std::shared_ptr<Entry> dest, const std::shared_ptr<Entry> &src);
static void resolveModuleProcedures(yyscan_t yyscanner,Entry *current_root);
static void resolveTypeBoundProcedures(Entry *scope);
static void truncatePrepass(yyscan_t yyscanner,int index);
static void pushBuffer(yyscan_t yyscanner,const QCString &buffer);
static void popBuffer(yyscan_t yyscanner);
static const CommentInPrepass* locatePrepassComment(yyscan_t yyscanner,int from, int to);
static void updateVariablePrepassComment(yyscan_t yyscanner,int from, int to);
static void newLine(yyscan_t yyscanner);
static void initEntry(yyscan_t yyscanner);

static const char *stateToString(int state);
static inline int computeIndent(const char *s);


//-----------------------------------------------------------------------------
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"
#define YY_USER_ACTION yyextra->colNr+=(int)yyleng;
#define INVALID_ENTRY ((Entry*)0x8)


//-----------------------------------------------------------------------------

%}

 //-----------------------------------------------------------------------------
 //-----------------------------------------------------------------------------
CMD       ("\\"|"@")
IDSYM     [a-z_A-Z0-9]
SEPARATE  [:, \t]
ID        [a-z_A-Z%]+{IDSYM}*
ID_       [a-z_A-Z%]*{IDSYM}*
OPERATOR_ID (operator{BS}"("{BS}(\.[a-z_A-Z]+\.|"="|"/="|"//"|"=="|"<"|"<="|">"|">="|"+"|"*"|"**"|"/"|"-"){BS}")")
SUBPROG   (subroutine|function)
B         [ \t]
BS        [ \t]*
BS_       [ \t]+
BT_       ([ \t]+|[ \t]*"(")
COMMA     {BS},{BS}
ARGS_L0   ("("[^)]*")")
ARGS_L1a  [^()]*"("[^)]*")"[^)]*
ARGS_L1   ("("{ARGS_L1a}*")")
ARGS_L2   "("({ARGS_L0}|[^()]|{ARGS_L1a}|{ARGS_L1})*")"
ARGS      {BS}({ARGS_L0}|{ARGS_L1}|{ARGS_L2})
NOARGS    {BS}"\n"

PRE       [pP][rR][eE]
CODE      [cC][oO][dD][eE]

COMM      "!"[!<>]
NUM_TYPE  (complex|integer|logical|real)
LOG_OPER  (\.and\.|\.eq\.|\.eqv\.|\.ge\.|\.gt\.|\.le\.|\.lt\.|\.ne\.|\.neqv\.|\.or\.|\.not\.)
KIND      {ARGS}
CHAR      (CHARACTER{ARGS}?|CHARACTER{BS}"*"({BS}[0-9]+|{ARGS}))
TYPE_SPEC (({NUM_TYPE}({BS}"*"{BS}[0-9]+)?)|({NUM_TYPE}{KIND})|DOUBLE{BS}COMPLEX|DOUBLE{BS}PRECISION|ENUMERATOR|{CHAR}|TYPE{ARGS}|CLASS{ARGS}|PROCEDURE{ARGS}?)

INTENT_SPEC intent{BS}"("{BS}(in|out|in{BS}out){BS}")"
ATTR_SPEC (EXTERNAL|ALLOCATABLE|DIMENSION{ARGS}|{INTENT_SPEC}|INTRINSIC|OPTIONAL|PARAMETER|POINTER|PROTECTED|PRIVATE|PUBLIC|SAVE|TARGET|NOPASS|PASS{ARGS}?|DEFERRED|NON_OVERRIDABLE|CONTIGUOUS|VOLATILE|VALUE)
LANGUAGE_BIND_SPEC BIND{BS}"("{BS}C{BS}((,{BS}NAME{BS}"="{BS}"\""(.*)"\""{BS})|(,{BS}NAME{BS}"="{BS}"'"(.*)"'"{BS}))?")"
/* Assume that attribute statements are almost the same as attributes. */
ATTR_STMT {ATTR_SPEC}|DIMENSION
EXTERNAL_STMT (EXTERNAL)

CONTAINS  CONTAINS
PREFIX    ((NON_)?RECURSIVE{BS_}|IMPURE{BS_}|PURE{BS_}|ELEMENTAL{BS_}){0,4}((NON_)?RECURSIVE|IMPURE|PURE|ELEMENTAL)?
SCOPENAME ({ID}{BS}"::"{BS})*

LINENR    {B}*[1-9][0-9]*
FILEICHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+=&#@~]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+=&#@~]
FILECHARS {FILEICHAR}*{FILEECHAR}+
HFILEMASK {FILEICHAR}*("."{FILEICHAR}+)+{FILECHARS}*
VFILEMASK {FILECHARS}("."{FILECHARS})*
FILEMASK  {VFILEMASK}|{HFILEMASK}

%option noyywrap
%option stack
%option caseless
/*%option debug */

 //---------------------------------------------------------------------------------

 /** fortran parsing states */
%x      Subprog
%x      SubprogPrefix
%x      Parameterlist
%x      SubprogBody
%x      SubprogBodyContains
%x      Start
%x      Comment
%x      Module
%x      Program
%x      ModuleBody
%x      ModuleBodyContains
%x      AttributeList
%x      FVariable
%x      Initialization
%x      ArrayInitializer
%x      FEnum
%x      Typedef
%x      TypedefBody
%x      TypedefBodyContains
%x      InterfaceBody
%x      StrIgnore
%x      String
%x      Use
%x      UseOnly
%x      ModuleProcedure

%x      Prepass

 /** comment parsing states */
%x      DocBlock
%x      DocBackLine
%x      DocCopyBlock

%x      BlockData

/** prototype parsing */
%x      Prototype
%x      PrototypeSubprog
%x      PrototypeArgs

%%

 /*-----------------------------------------------------------------------------------*/

<Prepass>^{BS}[&]*{BS}!.*\n             { /* skip lines with just comment. Note code was in free format or has been converted to it */
                                          yyextra->lineCountPrepass ++;
                                        }
<Prepass>^{BS}\n                        { /* skip empty lines */
                                          yyextra->lineCountPrepass ++;
                                        }
<*>^.*\n                                { // prepass: look for line continuations
                                          yyextra->functionLine = FALSE;

                                          DBG_CTX((stderr, "---%s", yytext));

                                          int indexStart = getAmpersandAtTheStart(yytext, (int)yyleng);
                                          int indexEnd = getAmpOrExclAtTheEnd(yytext, (int)yyleng, '\0');
                                          if (indexEnd>=0 && yytext[indexEnd]!='&') //we are only interested in amp
                                          {
                                            indexEnd=-1;
                                          }

                                          if (indexEnd<0)
                                          { // ----- no ampersand as line continuation
                                             if (YY_START == Prepass)
                                             { // last line in "continuation"

                                               // Only take input after initial ampersand
                                               yyextra->inputStringPrepass+=(const char*)(yytext+(indexStart+1));

                                               //printf("BUFFER:%s\n", (const char*)yyextra->inputStringPrepass);
                                               pushBuffer(yyscanner,yyextra->inputStringPrepass);
                                               yyextra->colNr = 0;
                                               pop_state(yyscanner);
                                             }
                                             else
                                             { // simple line
                                               yyextra->colNr = 0;
                                               REJECT;
                                             }
                                          }
                                          else
                                          { // ----- line with continuation
                                            if (YY_START != Prepass)
                                            {
                                              yyextra->comments.clear();
                                              yyextra->inputStringPrepass=QCString();
                                              yy_push_state(Prepass,yyscanner);
                                            }

                                            size_t length = yyextra->inputStringPrepass.length();

                                            // Only take input after initial ampersand
                                            yyextra->inputStringPrepass+=(const char*)(yytext+(indexStart+1));
                                            yyextra->lineCountPrepass ++;

                                            // cut off & and remove following comment if present
                                            truncatePrepass(yyscanner,static_cast<int>(length) + indexEnd - indexStart - 1);
                                          }
                                        }


 /*------ ignore strings that are not initialization strings */
<String>\"|\'                           { // string ends with next quote without previous backspace
                                          if (yytext[0]!=yyextra->stringStartSymbol)
                                          {
                                            yyextra->colNr -= (int)yyleng;
                                            REJECT;
                                          } // single vs double quote
                                          if (yy_top_state(yyscanner) == Initialization ||
                                              yy_top_state(yyscanner) == ArrayInitializer)
                                          {
                                            yyextra->initializer+=yytext;
                                          }
                                          pop_state(yyscanner);
                                        }
<String>[\x80-\xFF]*                    |
<String>.                               { if (yy_top_state(yyscanner) == Initialization ||
                                              yy_top_state(yyscanner) == ArrayInitializer)
                                          {
                                            yyextra->initializer+=yytext;
                                          }
                                        }
<*>\"|\'                                { /* string starts */
                                          if (YY_START == StrIgnore)
                                          { yyextra->colNr -= (int)yyleng;
                                            REJECT;
                                          }; // ignore in simple yyextra->comments
                                          yy_push_state(YY_START,yyscanner);
                                          if (yy_top_state(yyscanner) == Initialization ||
                                              yy_top_state(yyscanner) == ArrayInitializer)
                                          {
                                            yyextra->initializer+=yytext;
                                          }
                                          yyextra->stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String);
                                        }

 /*------ ignore simple comment (not documentation yyextra->comments) */

<*>"!"/([^<>\n]|"<ff>"|"<FF>")          {  if (YY_START == String || YY_START == DocCopyBlock)
                                          { yyextra->colNr -= (int)yyleng;
                                            REJECT;
                                          } // "!" is ignored in strings
                                          // skip comment line (without docu yyextra->comments "!>" "!<" )
                                          /* ignore further "!" and ignore yyextra->comments in Strings */
                                          if ((YY_START != StrIgnore) && (YY_START != String))
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(StrIgnore);
                                            yyextra->debugStr="*!";
                                            DBG_CTX((stderr,"start comment %d\n",yyextra->lineNr));
                                           }
                                        }
<StrIgnore>.?/\n                        { pop_state(yyscanner); // comment ends with endline character
                                          DBG_CTX((stderr,"end comment %d %s\n",yyextra->lineNr,qPrint(yyextra->debugStr)));
                                        } // comment line ends
<StrIgnore>[\x80-\xFF]*                 |
<StrIgnore>.                            { yyextra->debugStr+=yytext; }


 /*------ use handling ------------------------------------------------------------*/

<Start,ModuleBody,SubprogBody>"use"{BS_} {
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }
                                          yy_push_state(Use,yyscanner);
                                        }
<Use>{ID}                               {
                                          DBG_CTX((stderr,"using dir %s\n",yytext));
                                          yyextra->current->name=yytext;
                                          yyextra->current->name=yyextra->current->name.lower();
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->section=EntryType::makeUsingDir();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          yyextra->current->lang = SrcLangExt::Fortran;
                                          pop_state(yyscanner);
                                        }
<Use>{ID}/,                             {
                                          yyextra->useModuleName=yytext;
                                          yyextra->useModuleName=yyextra->useModuleName.lower();
                                        }
<Use>,{BS}"ONLY"                        { BEGIN(UseOnly);
                                        }
<UseOnly>{BS},{BS}                      {}
<UseOnly>{ID}                           {
                                          yyextra->current->name= yyextra->useModuleName+"::"+yytext;
                                          yyextra->current->name=yyextra->current->name.lower();
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->section=EntryType::makeUsingDecl();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          yyextra->current->lang = SrcLangExt::Fortran;
                                        }
<Use,UseOnly>"\n"                       {
                                          yyextra->colNr -= 1;
                                          unput(*yytext);
                                          pop_state(yyscanner);
                                        }

 /* INTERFACE definitions */
<Start,ModuleBody,SubprogBody>{
^{BS}interface{IDSYM}+                  { /* variable with interface prefix */ }
^{BS}interface                          { yyextra->ifType = IF_SPECIFIC;
                                          yy_push_state(InterfaceBody,yyscanner);
                                          // do not start a scope here, every
                                          // interface body is a scope of its own
                                        }

^{BS}abstract{BS_}interface             { yyextra->ifType = IF_ABSTRACT;
                                          yy_push_state(InterfaceBody,yyscanner);
                                          // do not start a scope here, every
                                          // interface body is a scope of its own
                                        }

^{BS}interface{BS_}{ID}{ARGS}?          { yyextra->ifType = IF_GENERIC;
                                          yyextra->current->bodyLine = yyextra->lineNr + yyextra->lineCountPrepass + 1; // we have to be at the line after the definition and we have to take continuation lines into account.
                                          yy_push_state(InterfaceBody,yyscanner);

                                          // extract generic name
                                          QCString name = QCString(yytext).stripWhiteSpace();
                                          name = name.right(name.length() - 9).stripWhiteSpace().lower();
                                          addInterface(yyscanner,name, yyextra->ifType);
                                          startScope(yyscanner,yyextra->last_entry.get());
                                        }
}

<InterfaceBody>^{BS}end{BS}interface({BS_}{ID})? {
                                          // end scope only if GENERIC interface
                                          if (yyextra->ifType == IF_GENERIC)
                                          {
                                            yyextra->last_entry->parent()->endBodyLine = yyextra->lineNr - 1;
                                          }
                                          if (yyextra->ifType == IF_GENERIC && !endScope(yyscanner,yyextra->current_root))
                                          {
                                            yyterminate();
                                          }
                                          yyextra->ifType = IF_NONE;
                                          pop_state(yyscanner);
                                        }
<InterfaceBody>module{BS}procedure      { yy_push_state(YY_START,yyscanner);
                                          BEGIN(ModuleProcedure);
                                        }
<ModuleProcedure>{ID}                   { QCString name = QCString(yytext).lower();
                                          if (yyextra->ifType == IF_ABSTRACT || yyextra->ifType == IF_SPECIFIC)
                                          {
                                            addInterface(yyscanner,name, yyextra->ifType);
                                            startScope(yyscanner,yyextra->last_entry.get());
                                          }

                                          yyextra->current->section = EntryType::makeFunction();
                                          yyextra->current->name = name; 
                                          yyextra->moduleProcedures.push_back(yyextra->current);
                                          addCurrentEntry(yyscanner,true);
                                        }
<ModuleProcedure>"\n"                   { yyextra->colNr -= 1;
                                          unput(*yytext);
                                          pop_state(yyscanner);
                                        }
<InterfaceBody>.                        {}

 /*-- Contains handling --*/
<Start>^{BS}{CONTAINS}/({BS}|\n|!|;)    {
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBodyContains,yyscanner); //anon program
                                          }
                                        }
<ModuleBody>^{BS}{CONTAINS}/({BS}|\n|!|;)   { BEGIN(ModuleBodyContains); }
<SubprogBody>^{BS}{CONTAINS}/({BS}|\n|!|;)  { BEGIN(SubprogBodyContains); }
<TypedefBody>^{BS}{CONTAINS}/({BS}|\n|!|;)  { BEGIN(TypedefBodyContains); }

 /*------ module handling ------------------------------------------------------------*/
<Start>block{BS}data{BS}{ID_}           {  //
                                          yyextra->vtype = V_IGNORE;
                                          yy_push_state(BlockData,yyscanner);
                                          yyextra->defaultProtection = Protection::Public;
                                        }
<Start>module|program{BS_}              {  //
                                          yyextra->vtype = V_IGNORE;
                                          if (yytext[0]=='m' || yytext[0]=='M')
                                          {
                                            yy_push_state(Module,yyscanner);
                                          }
                                          else
                                          {
                                            yy_push_state(Program,yyscanner);
                                          }
                                          yyextra->defaultProtection = Protection::Public;
                                        }
<BlockData>^{BS}"end"({BS}(block{BS}data)({BS_}{ID})?)?{BS}/(\n|!|;) { // end block data
                                          //if (!endScope(yyscanner,yyextra->current_root))
                                          //  yyterminate();
                                          yyextra->defaultProtection = Protection::Public;
                                          pop_state(yyscanner);
                                        }
<Start,ModuleBody,ModuleBodyContains>"end"({BS}(module|program)({BS_}{ID})?)?{BS}/(\n|!|;) { // end module
                                          resolveModuleProcedures(yyscanner,yyextra->current_root);
                                          if (!endScope(yyscanner,yyextra->current_root))
                                          {
                                            yyterminate();
                                          }
                                          yyextra->defaultProtection = Protection::Public;
                                          if (yyextra->global_scope)
                                          {
                                            if (yyextra->global_scope != INVALID_ENTRY)
                                            {
                                              yy_push_state(Start,yyscanner);
                                            }
                                            else
                                            {
                                              pop_state(yyscanner); // cannot pop artrificial entry
                                            }
                                          }
                                          else
                                          {
                                            yy_push_state(Start,yyscanner);
                                            yyextra->global_scope = INVALID_ENTRY; // signal that the yyextra->global_scope has already been used.
                                          }
                                        }
<Module>{ID}                            {
                                          addModule(yyscanner, yytext, TRUE);
                                          BEGIN(ModuleBody);
                                        }
<Program>{ID}                           {
                                            addModule(yyscanner, yytext, FALSE);
                                            BEGIN(ModuleBody);
                                        }

  /*------- access specification --------------------------------------------------------------------------*/

<ModuleBody,TypedefBody,TypedefBodyContains>private/{BS}(\n|"!") {
                                           yyextra->defaultProtection = Protection::Private;
                                           yyextra->current->protection = yyextra->defaultProtection ;
                                         }
<ModuleBody,TypedefBody,TypedefBodyContains>public/{BS}(\n|"!")  {
                                           yyextra->defaultProtection = Protection::Public;
                                           yyextra->current->protection = yyextra->defaultProtection ;
                                         }

 /*------- type definition  -------------------------------------------------------------------------------*/

<ModuleBody>^{BS}type{BS}"="            {}
<Start,ModuleBody>^{BS}type/[^a-z0-9_]  {
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner,QCString());
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }

                                          yy_push_state(Typedef,yyscanner);
                                          yyextra->current->protection = Protection::Package; // invalid in Fortran, replaced below
                                          yyextra->typeProtection = Protection::Public;
                                          yyextra->typeMode = true;
                                        }
<Typedef>{
{COMMA}                                 {}

{BS}"::"{BS}                            {}

abstract                                {
                                          yyextra->current->spec.setAbstractClass(true);
                                        }
extends{ARGS}                           {
                                          QCString basename = extractFromParens(yytext).lower();
                                          yyextra->current->extends.emplace_back(basename, Protection::Public, Specifier::Normal);
                                        }
public                                  {
                                          yyextra->current->protection = Protection::Public;
                                        }
private                                 {
                                          yyextra->current->protection = Protection::Private;
                                        }
{LANGUAGE_BIND_SPEC}                    {
                                          /* ignored for now */
                                        }
{ID}                                    { /* type name found */
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec.setStruct(true);
                                          yyextra->current->name = yytext;
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->bodyLine  = yyextra->lineNr;
                                          yyextra->current->startLine  = yyextra->lineNr;

                                          /* if type is part of a module, mod name is necessary for output */
                                          if (yyextra->current_root &&
                                              (yyextra->current_root->section.isClass() ||
                                               yyextra->current_root->section.isNamespace()))
                                          {
                                            yyextra->current->name = yyextra->current_root->name + "::" + yyextra->current->name;
                                          }

                                          // set modifiers to allow adjusting public/private in surrounding module scope
                                          if( yyextra->current->protection == Protection::Package )
                                          {
                                            yyextra->current->protection = yyextra->defaultProtection;
                                          }
                                          else if( yyextra->current->protection == Protection::Public )
                                          {
                                            yyextra->modifiers[yyextra->current_root][yyextra->current->name.lower().str()] |= QCString("public");
                                          }
                                          else if( yyextra->current->protection == Protection::Private )
                                          {
                                            yyextra->modifiers[yyextra->current_root][yyextra->current->name.lower().str()] |= QCString("private");
                                          }

                                          addCurrentEntry(yyscanner,true);
                                          startScope(yyscanner,yyextra->last_entry.get());
                                          BEGIN(TypedefBody);
                                        }
}

<TypedefBodyContains>{                  /* Type Bound Procedures */
^{BS}PROCEDURE{ARGS}?                   {
                                          yyextra->current->type = QCString(yytext).simplifyWhiteSpace().lower();
                                        }
^{BS}final                              {
                                          yyextra->current->spec.setFinal(true);
                                          yyextra->current->type = QCString(yytext).simplifyWhiteSpace();
                                        }
^{BS}generic                            {
                                          yyextra->current->type = QCString(yytext).simplifyWhiteSpace();
                                        }
{COMMA}                                 {
                                        }
{ATTR_SPEC}                             {
                                          yyextra->currentModifiers |= QCString(yytext).stripWhiteSpace();
                                        }
{BS}"::"{BS}                            {
                                        }
{ID}                                    {
                                          QCString name = yytext;
                                          yyextra->modifiers[yyextra->current_root][name.lower().str()] |= yyextra->currentModifiers;
                                          yyextra->current->section  = EntryType::makeFunction();
                                          yyextra->current->name     = name;
                                          // check for procedure(name)
                                          if (yyextra->current->type.find('(') != -1)
                                          {
                                            yyextra->current->args = extractFromParens(yyextra->current->type).stripWhiteSpace();
                                          }
                                          else
                                          {
                                            yyextra->current->args = name.lower(); // target procedure name if no => is given
                                          }
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->bodyLine = yyextra->lineNr;
                                          yyextra->current->startLine  = yyextra->lineNr;
                                          addCurrentEntry(yyscanner,true);
                                        }
{BS}"=>"[^(\n|\!)]*                     { /* Specific bindings come after the ID. */
                                          QCString tmp = yytext;
                                          int i = tmp.find("=>");
                                          if (i!=-1)
                                          {
                                            tmp.remove(0, i+2);
                                          }
                                          tmp = tmp.simplifyWhiteSpace().lower();
                                          if (yyextra->last_entry->type == "generic")
                                          {
                                            // duplicate entries for each overloaded variant
                                            // (parse through medhod1,method2, methodN, ...
                                            //printf("Parsing through %s for generic method %s.\n", tmp.data(), last_entry->name.data());
                                            i = tmp.find(",");
                                            while (i>0)
                                            {
                                              copyEntry(yyextra->current, yyextra->last_entry);
                                              yyextra->current->name = yyextra->last_entry->name;
                                              yyextra->current->section = EntryType::makeFunction();
                                              yyextra->last_entry->args = tmp.left(i).stripWhiteSpace();
                                              //printf("Found %s.\n", last_entry->args.data());
                                              addCurrentEntry(yyscanner,true);
                                              tmp = tmp.remove(0,i+1).stripWhiteSpace();
                                              i = tmp.find(",");
                                            }
                                          }
                                          //printf("Target function: %s\n", tmp.data());
                                          yyextra->last_entry->args = tmp;
                                        }
"\n"                                    {
                                          yyextra->currentModifiers = SymbolModifiers();
                                          newLine(yyscanner);
                                          yyextra->docBlock.clear();
                                        }
}


<TypedefBody,TypedefBodyContains>{
^{BS}"end"{BS}"type"({BS_}{ID})?{BS}/(\n|!|;) { /* end type definition */
                                          yyextra->last_entry->parent()->endBodyLine = yyextra->lineNr;
                                          if (!endScope(yyscanner,yyextra->current_root))
                                          {
                                            yyterminate();
                                          }
                                          yyextra->typeMode = false;
                                          pop_state(yyscanner);
                                        }
^{BS}"end"{BS}/(\n|!|;) { /* incorrect end type definition */
                                          warn(yyextra->fileName,yyextra->lineNr, "Found 'END' instead of 'END TYPE'");
                                          yyextra->last_entry->parent()->endBodyLine = yyextra->lineNr;
                                          if (!endScope(yyscanner,yyextra->current_root))
                                          {
                                            yyterminate();
                                          }
                                          yyextra->typeMode = false;
                                          pop_state(yyscanner);
                                        }
}

 /*------- module/global/typedef variable ---------------------------------------------------*/

<SubprogBody,SubprogBodyContains>^{BS}[0-9]*{BS}"end"({BS}{SUBPROG}({BS_}{ID})?)?{BS}/(\n|!|;) {
                                           //
                                           // ABSTRACT and specific interfaces are stored
                                           // in a scope of their own, even if multiple
                                           // are group in one INTERFACE/END INTERFACE block.
                                           //
                                           if (yyextra->ifType == IF_ABSTRACT || yyextra->ifType == IF_SPECIFIC)
                                           {
                                             endScope(yyscanner,yyextra->current_root);
                                             yyextra->last_entry->endBodyLine = yyextra->lineNr - 1;
                                           }
                                           yyextra->current_root->endBodyLine = yyextra->lineNr - 1;

                                           if (!endScope(yyscanner,yyextra->current_root))
                                           {
                                             yyterminate();
                                           }
                                           yyextra->subrCurrent.pop_back();
                                           yyextra->vtype = V_IGNORE;
                                           pop_state(yyscanner) ;
                                        }
<BlockData>{
{ID}                                    {
                                        }
}
<Start,ModuleBody,TypedefBody,SubprogBody,FEnum>{
^{BS}{TYPE_SPEC}/{SEPARATE}             {
                                          yyextra->last_enum.reset();
                                          if (YY_START == FEnum)
                                          {
                                            yyextra->argType = "@"; // enum marker
                                          }
                                          else
                                          {
                                            yyextra->argType = QCString(yytext).simplifyWhiteSpace().lower();
                                          }
                                          yyextra->current->bodyLine = yyextra->lineNr + 1;
                                          yyextra->current->endBodyLine = yyextra->lineNr + yyextra->lineCountPrepass;
                                          /* variable declaration starts */
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }
                                          yy_push_state(AttributeList,yyscanner);
                                        }
{EXTERNAL_STMT}/({BS}"::"|{BS_}{ID})    {
                                          /* external can be a "type" or an attribute */
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }
                                          QCString tmp = yytext;
                                          yyextra->currentModifiers |= tmp.stripWhiteSpace();
                                          yyextra->argType = QCString(yytext).simplifyWhiteSpace().lower();
                                          yy_push_state(AttributeList,yyscanner);
                                        }
{ATTR_STMT}/{BS_}{ID}                   |
{ATTR_STMT}/{BS}"::"                    {
                                          /* attribute statement starts */
                                          DBG_CTX((stderr,"5=========> Attribute statement: %s\n", yytext));
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }
                                          QCString tmp = yytext;
                                          yyextra->currentModifiers |= tmp.stripWhiteSpace();
                                          yyextra->argType="";
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN( AttributeList ) ;
                                        }
"common"                                {
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }
                                        }
{ID}                                    {
                                          if (YY_START == Start && QCString(yytext).stripWhiteSpace().lower()!="include")
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }
                                        }
^{BS}"type"{BS_}"is"/{BT_}              {}
^{BS}"type"{BS}"="                      {}
^{BS}"class"{BS_}"is"/{BT_}             {}
^{BS}"class"{BS_}"default"              {}
}
<AttributeList>{
{COMMA}                                 {}
{BS}                                    {}
{LANGUAGE_BIND_SPEC}                    {
                                          yyextra->currentModifiers |= yytext;
                                        }
{ATTR_SPEC}.                            { /* update yyextra->current yyextra->modifiers when it is an ATTR_SPEC and not a variable name */
                                          /* buyyextra->625519 */
                                          char chr = yytext[(int)yyleng-1];
                                          if (isId(chr))
                                          {
                                            yyextra->colNr -= (int)yyleng;
                                            REJECT;
                                          }
                                          else
                                          {
                                            QCString tmp = yytext;
                                            tmp = tmp.left(tmp.length() - 1);
                                            yyextra->colNr -= 1;
                                            unput(yytext[(int)yyleng-1]);
                                            yyextra->currentModifiers |= (tmp);
                                          }
                                        }
"::"                                    { /* end attribute list */
                                          BEGIN( FVariable );
                                        }
.                                       { /* unknown attribute, consider variable name */
                                          //cout<<"start variables, unput "<<*yytext<<endl;
                                          yyextra->colNr -= 1;
                                          unput(*yytext);
                                          BEGIN( FVariable );
                                        }
}

<FVariable>{BS}                          {}
<FVariable>{OPERATOR_ID}                 { /* parse operator access statements "public :: operator(==)" */
                                          QCString name = QCString(yytext).stripWhiteSpace().lower();
                                          /* if variable/type/etc is part of a module, mod name is necessary for output */
                                          // get surrounding state
                                          int currentState = YY_START;
                                          pop_state(yyscanner);
                                          int outerState = YY_START;
                                          yy_push_state(currentState,yyscanner);
                                          if( outerState == Start || outerState == ModuleBody )
                                          {
                                            if ((yyextra->current_root) &&
                                                (yyextra->current_root->section.isClass() ||
                                                 yyextra->current_root->section.isNamespace()))
                                            {
                                              name = yyextra->current_root->name + "::" + name;
                                            }
                                          }
                                          /* remember attributes for the symbol */
                                          yyextra->modifiers[yyextra->current_root][name.str()] |= yyextra->currentModifiers;
                                        }
<FVariable>{ID}                          { /* parse variable declaration */
                                          //cout << "5=========> got variable: " << yyextra->argType << "::" << yytext << endl;
                                          /* work around for bug in QCString.replace (QCString works) */
                                          QCString name=yytext;
                                          name = name.lower();
                                          /* if variable/type/etc is part of a module, mod name is necessary for output */
                                          if ((yyextra->current_root) && yyextra->current_root->section.isNamespace())
                                          {
                                            name = yyextra->current_root->name + "::" + name;
                                          }
                                          /* remember attributes for the symbol */
                                          yyextra->modifiers[yyextra->current_root][name.lower().str()] |= yyextra->currentModifiers;
                                          yyextra->argName= name;

                                          yyextra->vtype= V_IGNORE;
                                          if (!yyextra->argType.isEmpty() && !yyextra->current_root->section.isFunction())
                                          { // new variable entry
                                            yyextra->vtype = V_VARIABLE;
                                            yyextra->current->section = EntryType::makeVariable();
                                            yyextra->current->name = yyextra->argName;
                                            yyextra->current->type = yyextra->argType;
                                            yyextra->current->fileName = yyextra->fileName;
                                            yyextra->current->bodyLine  = yyextra->lineNr; // used for source reference
                                            yyextra->current->startLine  = yyextra->lineNr;
                                            if (yyextra->argType == "@")
                                            {
                                              yyextra->current_root->copyToSubEntry(yyextra->current);
                                              // add to the scope surrounding the enum (copy!)
                                              yyextra->last_enum = yyextra->current;
                                              yyextra->current_root->parent()->moveToSubEntryAndRefresh(yyextra->current);
                                              initEntry(yyscanner);
                                            }
                                            else
                                            {
                                              addCurrentEntry(yyscanner,true);
                                            }
                                          }
                                          else if (!yyextra->argType.isEmpty())
                                          { // declaration of parameter list: add type for corr. parameter
                                            Argument *parameter = getParameter(yyscanner,yyextra->argName);
                                            if (parameter)
                                            {
                                              yyextra->vtype= V_PARAMETER;
                                              if (!yyextra->argType.isEmpty()) parameter->type=yyextra->argType.stripWhiteSpace();
                                              if (!yyextra->docBlock.isEmpty())
                                              {
                                                subrHandleCommentBlock(yyscanner,yyextra->docBlock,TRUE);
                                              }
                                            }
                                            // save, it may be function return type
                                            if (parameter)
                                            {
                                              yyextra->modifiers[yyextra->current_root][name.lower().str()].type = yyextra->argType;
                                            }
                                            else
                                            {
                                              if ((yyextra->current_root->name.lower() == yyextra->argName.lower()) ||
                                                  (yyextra->modifiers[yyextra->current_root->parent()][yyextra->current_root->name.lower().str()].returnName.lower() == yyextra->argName.lower()))
                                              {
                                                int strt = yyextra->current_root->type.find("function");
                                                QCString lft;
                                                QCString rght;
                                                if (strt != -1)
                                                {
                                                  yyextra->vtype = V_RESULT;
                                                  lft = "";
                                                  rght = "";
                                                  if (strt != 0) lft = yyextra->current_root->type.left(strt).stripWhiteSpace();
                                                  if ((yyextra->current_root->type.length() - strt - strlen("function"))!= 0)
                                                  {
                                                    rght = yyextra->current_root->type.right(yyextra->current_root->type.length() - strt - (int)strlen("function")).stripWhiteSpace();
                                                  }
                                                  yyextra->current_root->type = lft;
                                                  if (rght.length() > 0)
                                                  {
                                                    if (yyextra->current_root->type.length() > 0) yyextra->current_root->type += " ";
                                                    yyextra->current_root->type += rght;
                                                  }
                                                  if (yyextra->argType.stripWhiteSpace().length() > 0)
                                                  {
                                                    if (yyextra->current_root->type.length() > 0) yyextra->current_root->type += " ";
                                                    yyextra->current_root->type += yyextra->argType.stripWhiteSpace();
                                                  }
                                                  if (yyextra->current_root->type.length() > 0) yyextra->current_root->type += " ";
                                                  yyextra->current_root->type += "function";
                                                  if (!yyextra->docBlock.isEmpty())
                                                  {
                                                    subrHandleCommentBlockResult(yyscanner,yyextra->docBlock,TRUE);
                                                  }
                                                }
                                                else
                                                {
                                                  yyextra->current_root->type += " " + yyextra->argType.stripWhiteSpace();
                                                }
                                                yyextra->current_root->type = yyextra->current_root->type.stripWhiteSpace();
                                                yyextra->modifiers[yyextra->current_root][name.lower().str()].type = yyextra->current_root->type;
                                              }
                                              else
                                              {
                                                yyextra->modifiers[yyextra->current_root][name.lower().str()].type = yyextra->argType;
                                              }
                                            }
                                            // any accumulated doc for argument should be emptied,
                                            // because it is handled other way and this doc can be
                                            // unexpectedly passed to the next member.
                                            yyextra->current->doc.clear();
                                            yyextra->current->brief.clear();
                                          }
                                        }
<FVariable>{ARGS}                        { /* dimension of the previous entry. */
                                          QCString name(yyextra->argName);
                                          QCString attr("dimension");
                                          attr += yytext;
                                          yyextra->modifiers[yyextra->current_root][name.lower().str()] |= attr;
                                        }
<FVariable>{COMMA}                       { //printf("COMMA: %d<=..<=%d\n", yyextra->colNr-(int)yyleng, yyextra->colNr);
                                          // locate !< comment
                                          updateVariablePrepassComment(yyscanner,yyextra->colNr-(int)yyleng, yyextra->colNr);
                                        }
<FVariable>{BS}"="                       {
                                          yy_push_state(YY_START,yyscanner);
                                          yyextra->initializer="=";
                                          yyextra->initializerScope = yyextra->initializerArrayScope = 0;
                                          BEGIN(Initialization);
                                        }
<FVariable>"\n"                          { yyextra->currentModifiers = SymbolModifiers();
                                          pop_state(yyscanner); // end variable declaration list
                                          newLine(yyscanner);
                                          yyextra->docBlock.clear();
                                        }
<FVariable>";".*"\n"                     { yyextra->currentModifiers = SymbolModifiers();
                                          pop_state(yyscanner); // end variable declaration list
                                          yyextra->docBlock.clear();
                                          yyextra->inputStringSemi = " \n"+QCString(yytext+1);
                                          yyextra->lineNr--;
                                          pushBuffer(yyscanner,yyextra->inputStringSemi);
                                        }
<*>";".*"\n"                            {
                                          if (YY_START == FVariable) REJECT; // Just be on the safe side
                                          if (YY_START == String) REJECT; // ";" ignored in strings
                                          if (YY_START == StrIgnore) REJECT; // ";" ignored in regular yyextra->comments
                                          if (YY_START == DocBlock) REJECT; // ";" ignored in documentation blocks
                                          yyextra->inputStringSemi = " \n"+QCString(yytext+1);
                                          yyextra->lineNr--;
                                          pushBuffer(yyscanner,yyextra->inputStringSemi);
                                        }

<Initialization,ArrayInitializer>"["    |
<Initialization,ArrayInitializer>"(/"   { yyextra->initializer+=yytext;
                                          yyextra->initializerArrayScope++;
                                          BEGIN(ArrayInitializer); // initializer may contain comma
                                        }
<ArrayInitializer>"]"                   |
<ArrayInitializer>"/)"                  { yyextra->initializer+=yytext;
                                          yyextra->initializerArrayScope--;
                                          if (yyextra->initializerArrayScope<=0)
                                          {
                                            yyextra->initializerArrayScope = 0; // just in case
                                            BEGIN(Initialization);
                                          }
                                        }
<ArrayInitializer>.                     { yyextra->initializer+=yytext; }
<Initialization>"("                     { yyextra->initializerScope++;
                                          yyextra->initializer+=yytext;
                                        }
<Initialization>")"                     { yyextra->initializerScope--;
                                          yyextra->initializer+=yytext;
                                        }
<Initialization>{COMMA}                 { if (yyextra->initializerScope == 0)
                                          {
                                            updateVariablePrepassComment(yyscanner,yyextra->colNr-(int)yyleng, yyextra->colNr);
                                            pop_state(yyscanner); // end initialization
                                            if (yyextra->last_enum)
                                            {
                                              yyextra->last_enum->initializer.str(yyextra->initializer.str());
                                            }
                                            else
                                            {
                                              if (yyextra->vtype == V_VARIABLE) yyextra->last_entry->initializer.str(yyextra->initializer.str());
                                            }
                                          }
                                          else
                                          {
                                            yyextra->initializer+=", ";
                                          }
                                        }
<Initialization>"\n"|"!"                { //|
                                          pop_state(yyscanner); // end initialization
                                          if (yyextra->last_enum)
                                          {
                                            yyextra->last_enum->initializer.str(yyextra->initializer.str());
                                          }
                                          else
                                          {
                                            if (yyextra->vtype == V_VARIABLE) yyextra->last_entry->initializer.str(yyextra->initializer.str());
                                          }
                                          yyextra->colNr -= 1;
                                          unput(*yytext);
                                        }
<Initialization>.                       { yyextra->initializer+=yytext; }

<*>{BS}"enum"{BS}","{BS}"bind"{BS}"("{BS}"c"{BS}")"{BS} {
                                          if (YY_START == Start)
                                          {
                                            addModule(yyscanner);
                                            yy_push_state(ModuleBody,yyscanner); //anon program
                                          }

                                          yy_push_state(FEnum,yyscanner);
                                          yyextra->current->protection = yyextra->defaultProtection;
                                          yyextra->typeProtection = yyextra->defaultProtection;
                                          yyextra->typeMode = true;

                                          yyextra->current->spec.setStruct(true);
                                          yyextra->current->name.clear();
                                          yyextra->current->args.clear();
                                          yyextra->current->name.sprintf("@%d",yyextra->anonCount++);

                                          yyextra->current->section = EntryType::makeEnum();
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->lineNr;
                                          yyextra->current->bodyLine  = yyextra->lineNr;
                                          if ((yyextra->current_root) &&
                                              (yyextra->current_root->section.isClass() ||
                                               yyextra->current_root->section.isNamespace()))
                                          {
                                            yyextra->current->name = yyextra->current_root->name + "::" + yyextra->current->name;
                                          }

                                          addCurrentEntry(yyscanner,true);
                                          startScope(yyscanner,yyextra->last_entry.get());
                                          BEGIN( FEnum ) ;
                                        }
<FEnum>"end"{BS}"enum"                   {
                                          yyextra->last_entry->parent()->endBodyLine = yyextra->lineNr;
                                          if (!endScope(yyscanner,yyextra->current_root))
                                          {
                                            yyterminate();
                                          }
                                          yyextra->typeMode = false;
                                          pop_state(yyscanner);
                                        }
 /*------ fortran subroutine/function handling ------------------------------------------------------------*/
 /*       Start is initial condition                                                                       */

<Start,ModuleBody,SubprogBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains>^{BS}({PREFIX}{BS_})?{TYPE_SPEC}{BS}({PREFIX}{BS_})?/{SUBPROG}{BS_} {
                                          if (yyextra->ifType == IF_ABSTRACT || yyextra->ifType == IF_SPECIFIC)
                                          {
                                            addInterface(yyscanner,"$interface$", yyextra->ifType);
                                            startScope(yyscanner,yyextra->last_entry.get());
                                          }

                                          // TYPE_SPEC is for old function style function result
                                          yyextra->current->type = QCString(yytext).stripWhiteSpace().lower();
                                          yy_push_state(SubprogPrefix,yyscanner);
                                        }

<SubprogPrefix>{BS}{SUBPROG}{BS_}       {
                                          // Fortran subroutine or function found
                                          yyextra->vtype = V_IGNORE;
                                          QCString result=yytext;
                                          result=result.stripWhiteSpace();
                                          addSubprogram(yyscanner,result);
                                          BEGIN(Subprog);
                                          yyextra->current->bodyLine = yyextra->lineNr + yyextra->lineCountPrepass + 1; // we have to be at the line after the definition and we have to take continuation lines into account.
                                          yyextra->current->startLine = yyextra->lineNr;
                                        }

<Start,ModuleBody,SubprogBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains>^{BS}({PREFIX}{BS_})?{SUBPROG}{BS_} {
                                          // Fortran subroutine or function found
                                          yyextra->vtype = V_IGNORE;
                                          if (yyextra->ifType == IF_ABSTRACT || yyextra->ifType == IF_SPECIFIC)
                                          {
                                            addInterface(yyscanner,"$interface$", yyextra->ifType);
                                            startScope(yyscanner,yyextra->last_entry.get());
                                          }

                                          QCString result = QCString(yytext).stripWhiteSpace();
                                          addSubprogram(yyscanner,result);
                                          yy_push_state(Subprog,yyscanner);
                                          yyextra->current->bodyLine = yyextra->lineNr + yyextra->lineCountPrepass + 1; // we have to be at the line after the definition and we have to take continuation lines into account.
                                          yyextra->current->startLine = yyextra->lineNr;
                                        }

<Subprog>{BS}                           {   /* ignore white space */   }
<Subprog>{ID}                           { yyextra->current->name = yytext;
                                          //cout << "1a==========> got " << yyextra->current->type << " " << yytext << " " << yyextra->lineNr << endl;
                                          QCString returnName = yyextra->current->name.lower();
                                          /* if type is part of a module, mod name is necessary for output */
                                          if ((yyextra->current_root) &&
                                              (yyextra->current_root->section.isClass() ||
                                               yyextra->current_root->section.isNamespace()))
                                          {
                                            yyextra->current->name= yyextra->current_root->name + "::" + yyextra->current->name;
                                          }
                                          yyextra->modifiers[yyextra->current_root][yyextra->current->name.lower().str()].returnName = std::move(returnName);

                                          if (yyextra->ifType == IF_ABSTRACT || yyextra->ifType == IF_SPECIFIC)
                                          {
                                            yyextra->current_root->name = substitute(
                                                yyextra->current_root->name, "$interface$", QCString(yytext).lower());
                                          }

                                          BEGIN(Parameterlist);
                                        }
<Parameterlist>"("                      { yyextra->current->args = "("; }
<Parameterlist>")"                      {
                                          yyextra->current->args += ")";
                                          yyextra->current->args = removeRedundantWhiteSpace(yyextra->current->args);
                                          addCurrentEntry(yyscanner,true);
                                          startScope(yyscanner,yyextra->last_entry.get());
                                          BEGIN(SubprogBody);
                                        }
<Parameterlist>{COMMA}|{BS}             { yyextra->current->args += yytext;
                                          const CommentInPrepass *c = locatePrepassComment(yyscanner,yyextra->colNr-(int)yyleng, yyextra->colNr);
                                          if (c)
                                          {
                                            if (!yyextra->current->argList.empty())
                                            {
                                              yyextra->current->argList.back().docs = c->str;
                                            }
                                          }
                                        }
<Parameterlist>{ID}                     {
                                          //yyextra->current->type not yet available
                                          QCString param = QCString(yytext).lower();
                                          // std::cout << "3=========> got parameter " << param << "\n";
                                          yyextra->current->args += param;
                                          Argument arg;
                                          arg.name = param;
                                          arg.type = "";
                                          yyextra->current->argList.push_back(arg);
                                        }
<Parameterlist>{NOARGS}                 {
                                          newLine(yyscanner);
                                          //printf("3=========> without parameterlist \n");
                                          addCurrentEntry(yyscanner,true);
                                          startScope(yyscanner,yyextra->last_entry.get());
                                          BEGIN(SubprogBody);
                                        }
<SubprogBody>result{BS}\({BS}{ID}       {
                                          if (yyextra->functionLine)
                                          {
                                            QCString result= yytext;
                                            result= result.right(result.length()-result.find("(")-1);
                                            result= result.stripWhiteSpace();
                                            yyextra->modifiers[yyextra->current_root->parent()][yyextra->current_root->name.lower().str()].returnName = result;
                                          }
                                          //cout << "=====> got result " <<  result << endl;
                                         }

 /*---- documentation yyextra->comments --------------------------------------------------------------------*/

<FVariable,SubprogBody,ModuleBody,TypedefBody,TypedefBodyContains>"!<"  { /* backward docu comment */
                                          if (yyextra->vtype != V_IGNORE)
                                          {
                                            yyextra->current->docLine  = yyextra->lineNr;
                                            yyextra->docBlockJavaStyle = FALSE;
                                            yyextra->docBlock.clear();
                                            yyextra->docBlockJavaStyle = Config_getBool(JAVADOC_AUTOBRIEF);
                                            startCommentBlock(yyscanner,FALSE);
                                            yy_push_state(DocBackLine,yyscanner);
                                          }
                                          else
                                          {
                                            /* handle out of place !< comment as a normal comment */
                                            if (YY_START == String)
                                            {
                                              yyextra->colNr -= (int)yyleng;
                                              REJECT;
                                            } // "!" is ignored in strings
                                            // skip comment line (without docu yyextra->comments "!>" "!<" )
                                            /* ignore further "!" and ignore yyextra->comments in Strings */
                                            if ((YY_START != StrIgnore) && (YY_START != String))
                                            {
                                              yy_push_state(YY_START,yyscanner);
                                              BEGIN(StrIgnore);
                                              yyextra->debugStr="*!";
                                            }
                                          }
                                        }
<DocBackLine>.*                         { // contents of yyextra->current comment line
                                          yyextra->docBlock+=yytext;
                                        }
<DocBackLine>"\n"{BS}"!"("<"|"!"+)      { // comment block (next line is also comment line)
                                          yyextra->docBlock+="\n"; // \n is necessary for lists
                                          newLine(yyscanner);
                                        }
<DocBackLine>"\n"                       { // comment block ends at the end of this line
                                          //cout <<"3=========> comment block : "<< yyextra->docBlock << endl;
                                          yyextra->colNr -= 1;
                                          unput(*yytext);
                                          if (yyextra->vtype == V_VARIABLE)
                                          {
                                            std::shared_ptr<Entry> tmp_entry = yyextra->current;
                                            // temporarily switch to the previous entry
                                            if (yyextra->last_enum)
                                            {
                                              yyextra->current = yyextra->last_enum;
                                            }
                                            else
                                            {
                                              yyextra->current = yyextra->last_entry;
                                            }
                                            handleCommentBlock(yyscanner,stripIndentation(yyextra->docBlock),TRUE);
                                            // switch back
                                            yyextra->current = std::move(tmp_entry);
                                          }
                                          else if (yyextra->vtype == V_PARAMETER)
                                          {
                                            subrHandleCommentBlock(yyscanner,yyextra->docBlock,TRUE);
                                          }
                                          else if (yyextra->vtype == V_RESULT)
                                          {
                                            subrHandleCommentBlockResult(yyscanner,yyextra->docBlock,TRUE);
                                          }
                                          pop_state(yyscanner);
                                          yyextra->docBlock.clear();
                                        }

<Start,SubprogBody,ModuleBody,TypedefBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains,TypedefBodyContains,FEnum>^{BS}  {
                                          yyextra->curIndent = computeIndent(yytext);
                                        }
<Start,SubprogBody,ModuleBody,TypedefBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains,TypedefBodyContains,FEnum>"!>"  {
                                          yy_push_state(YY_START,yyscanner);
                                          yyextra->current->docLine  = yyextra->lineNr;
                                          yyextra->docBlockJavaStyle = FALSE;
                                          if (YY_START==SubprogBody) yyextra->docBlockInBody = TRUE;
                                          yyextra->docBlock.clear();
                                          yyextra->docBlockJavaStyle = Config_getBool(JAVADOC_AUTOBRIEF);
                                          startCommentBlock(yyscanner,FALSE);
                                          BEGIN(DocBlock);
                                          //cout << "start DocBlock " << endl;
                                        }


<DocBlock>({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
                                          yyextra->docBlock += yytext;
                                        }
<DocBlock>{CMD}("f$"|"f["|"f{"|"f(")    {
                                          yyextra->docBlock += yytext;
                                          yyextra->docBlockName=&yytext[1];
                                          if (yyextra->docBlockName.at(1)=='[')
                                          {
                                            yyextra->docBlockName.at(1)=']';
                                          }
                                          if (yyextra->docBlockName.at(1)=='{')
                                          {
                                            yyextra->docBlockName.at(1)='}';
                                          }
                                          if (yyextra->docBlockName.at(1)=='(')
                                          {
                                            yyextra->docBlockName.at(1)=')';
                                          }
                                          yyextra->fencedSize=0;
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}"ifile"{B}+"\""[^\n\"]+"\"" {
                                          yyextra->fileName = &yytext[6];
                                          yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                          yyextra->fileName = yyextra->fileName.mid(1,yyextra->fileName.length()-2);
                                          yyextra->docBlock += yytext;
                                        }
<DocBlock>{CMD}"ifile"{B}+{FILEMASK}    {
                                          yyextra->fileName = &yytext[6];
                                          yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                          yyextra->docBlock += yytext;
                                        }
<DocBlock>{CMD}"iline"{LINENR}/[\n\.]   |
<DocBlock>{CMD}"iline"{LINENR}{B}       {
                                          bool ok = false;
                                          int nr = QCString(&yytext[6]).toInt(&ok);
                                          if (!ok)
                                          {
                                            warn(yyextra->fileName,yyextra->lineNr,"Invalid line number '{}' for iline command",yytext);
                                          }
                                          else
                                          {
                                            yyextra->lineNr = nr;
                                          }
                                          yyextra->docBlock += yytext;
                                        }
<DocBlock>{B}*"<"{PRE}">"               {
                                          yyextra->docBlock += yytext;
                                          yyextra->docBlockName="<pre>";
                                          yyextra->fencedSize=0;
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{B}*"<"<CODE>">"              {
                                          yyextra->docBlock += yytext;
                                          yyextra->docBlockName="<code>";
                                          yyextra->fencedSize=0;
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}"startuml"/[^a-z_A-Z0-9\-]     { // verbatim command
                                          yyextra->docBlock += yytext;
                                          yyextra->docBlockName="uml";
                                          yyextra->fencedSize=0;
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc"|"code")/[^a-z_A-Z0-9\-]     { // verbatim command
                                          yyextra->docBlock += yytext;
                                          yyextra->docBlockName=&yytext[1];
                                          yyextra->fencedSize=0;
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>"~~~"[~]*                     {
                                          QCString pat = yytext;
                                          yyextra->docBlock += pat;
                                          yyextra->docBlockName="~~~";
                                          yyextra->fencedSize=pat.length();
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>"```"[`]*/(".")?[a-zA-Z0-9#_-]+ |
<DocBlock>"```"[`]*/"{"[^}]+"}" |
<DocBlock>"```"[`]*                     {
                                          QCString pat = yytext;
                                          yyextra->docBlock += pat;
                                          yyextra->docBlockName="```";
                                          yyextra->fencedSize=pat.length();
                                          yyextra->blockString=yytext;
                                          yyextra->blockLineNr=yyextra->lineNr;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>"\\ilinebr "{BS}              {
                                          QCString indent;
                                          int extraSpaces = std::max(0,static_cast<int>(yyleng-9-yyextra->curIndent-2));
                                          indent.fill(' ',extraSpaces);
                                          //printf("extraSpaces=%d\n",extraSpaces);
                                          yyextra->docBlock += "\\ilinebr ";
                                          yyextra->docBlock += indent;
                                        }

<DocBlock>[^@*`~\/\\\n]+                { // any character that isn't special
                                          yyextra->docBlock += yytext;
                                        }
<DocBlock>"\n"{BS}"!"(">"|"!"+)         { // comment block (next line is also comment line)
                                          yyextra->docBlock+="\n"; // \n is necessary for lists
                                          newLine(yyscanner);
                                        }
<DocBlock>"\n"                          { // comment block ends at the end of this line
                                          //cout <<"3=========> comment block : "<< yyextra->docBlock << endl;
                                          yyextra->colNr -= 1;
                                          unput(*yytext);
                                          handleCommentBlock(yyscanner,stripIndentation(yyextra->docBlock),TRUE);
                                          pop_state(yyscanner);
                                        }
<DocBlock>.                             { // command block
                                          yyextra->docBlock += *yytext;
                                        }

 /* ---- Copy verbatim sections ------ */

<DocCopyBlock>"</"{PRE}">"              { // end of a <pre> block
                                          yyextra->docBlock += yytext;
                                          if (yyextra->docBlockName=="<pre>")
                                          {
                                            yyextra->docBlockName="";
                                            yyextra->blockString.clear();
                                            yyextra->blockLineNr=-1;
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>"</"{CODE}">"             { // end of a <code> block
                                          yyextra->docBlock += yytext;
                                          if (yyextra->docBlockName=="<code>")
                                          {
                                            yyextra->docBlockName="";
                                            yyextra->blockString.clear();
                                            yyextra->blockLineNr=-1;
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>{CMD}("f$"|"f]"|"f}"|"f)")     {
                                          yyextra->docBlock += yytext;
                                          if (yyextra->docBlockName==&yytext[1])
                                          {
                                            yyextra->docBlockName="";
                                            yyextra->blockString.clear();
                                            yyextra->blockLineNr=-1;
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>{CMD}("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endmsc"|"enduml"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
                                          yyextra->docBlock += yytext;
                                          if (&yytext[4]==yyextra->docBlockName)
                                          {
                                            yyextra->docBlockName="";
                                            yyextra->blockString.clear();
                                            yyextra->blockLineNr=-1;
                                            BEGIN(DocBlock);
                                          }
                                        }

<DocCopyBlock>^{B}*{COMM}               { // start of a comment line
                                          if (yyextra->docBlockName=="verbatim")
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            QCString indent; 
                                            indent.fill(' ',computeIndent(yytext) + 2);
                                            yyextra->docBlock += indent;
                                          }
                                        }
<DocCopyBlock>"~~~"[~]*                 {
                                          QCString pat = yytext;
                                          yyextra->docBlock += pat;
                                          if (yyextra->docBlockName == "~~~" && yyextra->fencedSize==pat.length())
                                          {
                                            yyextra->blockString.clear();
                                            yyextra->blockLineNr=-1;
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>"```"[`]*                 {
                                          QCString pat = yytext;
                                          yyextra->docBlock += pat;
                                          if (yyextra->docBlockName == "```" && yyextra->fencedSize==pat.length())
                                          {
                                            yyextra->blockString.clear();
                                            yyextra->blockLineNr=-1;
                                            BEGIN(DocBlock);
                                          }
                                        }

<DocCopyBlock>[^\<@/\*\]!`~"\$\\\n]+      { // any character that is not special
                                          yyextra->docBlock += yytext;
                                        }
<DocCopyBlock>\n                        { // newline
                                          yyextra->docBlock += *yytext;
                                          newLine(yyscanner);
                                        }
<DocCopyBlock>.                         { // any other character
                                          yyextra->docBlock += *yytext;
                                        }

 /*-----Prototype parsing -------------------------------------------------------------------------*/
<Prototype>{BS}{SUBPROG}{BS_}           {
                                          BEGIN(PrototypeSubprog);
                                        }
<Prototype,PrototypeSubprog>{BS}{SCOPENAME}?{BS}{ID} {
                                          yyextra->current->name = QCString(yytext).lower();
                                          yyextra->current->name.stripWhiteSpace();
                                          BEGIN(PrototypeArgs);
                                        }
<PrototypeArgs>{
"("|")"|","|{BS_}                       { yyextra->current->args += yytext; }
{ID}                                    { yyextra->current->args += yytext;
                                          Argument a;
                                          a.name = QCString(yytext).lower();
                                          yyextra->current->argList.push_back(a);
                                        }
}

 /*------------------------------------------------------------------------------------------------*/

<*>"\n"                                 {
                                          newLine(yyscanner);
                                          //if (yyextra->debugStr.stripWhiteSpace().length() > 0) cout << "ignored text: " << yyextra->debugStr << " state: " <<YY_START << endl;
                                          yyextra->debugStr="";
                                        }


 /*---- error: EOF in wrong state --------------------------------------------------------------------*/

<*><<EOF>>                              {
                                          if (yyextra->parsingPrototype)
                                          {
                                            yyterminate();
                                          }
                                          else if ( yyextra->includeStackPtr <= 0 )
                                          {
                                            if (YY_START!=INITIAL && YY_START!=Start)
                                            {
                                              DBG_CTX((stderr,"==== Error: EOF reached in wrong state (end missing)"));
                                              scanner_abort(yyscanner);
                                            }
                                            yyterminate();
                                          }
                                          else
                                          {
                                            popBuffer(yyscanner);
                                          }
                                        }
<*>{LOG_OPER}                           { // Fortran logical comparison keywords
                                        }
<*>.                                    {
                                          //yyextra->debugStr+=yytext;
                                          //printf("I:%c\n", *yytext);
                                        } // ignore remaining text

 /**********************************************************************************/
 /**********************************************************************************/
 /**********************************************************************************/
%%
//----------------------------------------------------------------------------

static void newLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->lineNr++;
  yyextra->lineNr+=yyextra->lineCountPrepass;
  yyextra->lineCountPrepass=0;
  yyextra->comments.clear();
}

static inline int computeIndent(const char *s)
{
  int col=0;
  int tabSize=Config_getInt(TAB_SIZE);
  const char *p=s;
  char c = 0;
  while ((c=*p++))
  {
    if (c=='\t') col+=tabSize-(col%tabSize);
    else if (c=='\n') col=0;
    else col++;
  }
  return col;
}

static const CommentInPrepass *locatePrepassComment(yyscan_t yyscanner,int from, int to)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("Locate %d-%d\n", from, to);
  for (const auto &cip : yyextra->comments)
  { // todo: optimize
    int c = cip.column;
    //printf("Candidate %d\n", c);
    if (c>=from && c<=to)
    {
      // comment for previous variable or parameter
      return &cip;
    }
  }
  return nullptr;
}

static void updateVariablePrepassComment(yyscan_t yyscanner,int from, int to)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const CommentInPrepass *c = locatePrepassComment(yyscanner,from, to);
  if (c && yyextra->vtype == V_VARIABLE)
  {
    yyextra->last_entry->brief = c->str;
  }
  else if (c && yyextra->vtype == V_PARAMETER)
  {
    Argument *parameter = getParameter(yyscanner,yyextra->argName);
    if (parameter) parameter->docs = c->str;
  }
}

static int getAmpersandAtTheStart(const char *buf, int length)
{
  for(int i=0; i<length; i++)
  {
    switch(buf[i])
    {
      case ' ':
      case '\t':
        break;
      case '&':
        return i;
      default:
        return -1;
    }
  }
  return -1;
}

/* Returns ampersand index, comment start index or -1 if neither exist.*/
static int getAmpOrExclAtTheEnd(const char *buf, int length, char ch)
{
  // Avoid ampersands in string and yyextra->comments
  int parseState = Start;
  char quoteSymbol = 0;
  int ampIndex = -1;
  int commentIndex = -1;
  quoteSymbol = ch;
  if (ch != '\0') parseState = String;

  for(int i=0; i<length && parseState!=Comment; i++)
  {
    // When in string, skip backslashes
    // Legacy code, not sure whether this is correct?
    if (parseState==String)
    {
      if (buf[i]=='\\') i++;
    }

    switch(buf[i])
    {
        case '\'':
        case '"':
          // Close string, if quote symbol matches.
          // Quote symbol is set iff parseState==String
          if (buf[i]==quoteSymbol)
          {
             parseState = Start;
             quoteSymbol = 0;
          }
          // Start new string, if not already in string or comment
          else if (parseState==Start)
          {
            parseState = String;
            quoteSymbol = buf[i];
          }
          ampIndex = -1; // invalidate prev ampersand
          break;
        case '!':
          // When in string or comment, ignore exclamation mark
          if (parseState==Start)
          {
            parseState = Comment;
            commentIndex = i;
          }
          break;
        case ' ':  // ignore whitespace
        case '\t':
        case '\n': // this may be at the end of line
          break;
        case '&':
          ampIndex = i;
          break;
        default:
          ampIndex = -1; // invalidate prev ampersand
    }
  }

  if (ampIndex>=0)
    return ampIndex;
  else
   return commentIndex;
}

/* Although yyextra->comments at the end of continuation line are grabbed by this function,
* we still do not know how to use them later in parsing.
*/
void truncatePrepass(yyscan_t yyscanner,int index)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  size_t length = yyextra->inputStringPrepass.length();
  for (size_t i=index+1; i<length; i++) {
    if (yyextra->inputStringPrepass[i]=='!' && i<length-1 && yyextra->inputStringPrepass[i+1]=='<') { // save comment
      yyextra->comments.emplace_back(index, yyextra->inputStringPrepass.right(length-i-2));
    }
  }
  yyextra->inputStringPrepass.resize(index);
}

/* This function assumes that contents has at least size=length+1 */
static void insertCharacter(char *contents, int length, int pos, char c)
{
  // shift tail by one character
  for(int i=length; i>pos; i--)
    contents[i]=contents[i-1];
  // set the character
  contents[pos] = c;
}

/* change yyextra->comments and bring line continuation character to previous line */
/* also used to set continuation marks in case of fortran code usage, done here as it is quite complicated code */
const char* prepassFixedForm(const char* contents, int *hasContLine,int fixedCommentAfter)
{
  int column=0;
  int prevLineLength=0;
  int prevLineAmpOrExclIndex=-1;
  int skipped = 0;
  char prevQuote = '\0';
  char thisQuote = '\0';
  bool emptyLabel=TRUE;
  bool commented=FALSE;
  bool inSingle=FALSE;
  bool inDouble=FALSE;
  bool inBackslash=FALSE;
  bool fullCommentLine=TRUE;
  bool artificialComment=FALSE;
  bool spaces=TRUE;
  int newContentsSize = (int)strlen(contents)+3; // \000, \n (when necessary) and one spare character (to avoid reallocation)
  char* newContents = (char*)malloc(newContentsSize);
  int curLine = 1;
  size_t sizCont;

  int j = -1;
  sizCont = strlen(contents);
  for(size_t i=0;i<sizCont;i++) {
    column++;
    char c = contents[i];
    if (artificialComment && c != '\n')
    {
      if (c == '!' && spaces)
      {
        newContents[j++] = c;
        artificialComment = FALSE;
        spaces = FALSE;
        skipped = 0;
        continue;
      }
      else if (c == ' ' || c == '\t') continue;
      else
      {
        spaces = FALSE;
        skipped++;
        continue;
      }
    }

    j++;
    if (j>=newContentsSize-3) { // check for spare characters, which may be eventually used below (by & and '! ')
      newContents = (char*)realloc(newContents, newContentsSize+1000);
      newContentsSize = newContentsSize+1000;
    }

    switch(c) {
      case '\n':
        if (!fullCommentLine)
        {
          prevLineLength=column;
          prevLineAmpOrExclIndex=getAmpOrExclAtTheEnd(&contents[i-prevLineLength+1], prevLineLength,prevQuote);
          if (prevLineAmpOrExclIndex == -1) prevLineAmpOrExclIndex = column - 1;
          if (skipped)
          {
            prevLineAmpOrExclIndex = -1;
            skipped = 0;
          }
        }
        else
        {
          prevLineLength+=column;
          /* Even though a full comment line is not really a comment line it can be seen as one. An empty line is also seen as a comment line (small bonus) */
          if (hasContLine)
          {
            hasContLine[curLine - 1] = 1;
          }
        }
        artificialComment=FALSE;
        spaces=TRUE;
        fullCommentLine=TRUE;
        column=0;
        emptyLabel=TRUE;
        commented=FALSE;
        newContents[j]=c;
        prevQuote = thisQuote;
        curLine++;
        break;
      case ' ':
      case '\t':
        newContents[j]=c;
        break;
      case '\000':
        if (hasContLine)
        {
           free(newContents);
           return nullptr;
        }
        newContents[j]='\000';
        newContentsSize = (int)strlen(newContents);
        if (newContents[newContentsSize - 1] != '\n')
        {
          // to be on the safe side
          newContents = (char*)realloc(newContents, newContentsSize+2);
          newContents[newContentsSize] = '\n';
          newContents[newContentsSize + 1] = '\000';
        }
        return newContents;
      case '"':
      case '\'':
      case '\\':
        if ((column <= fixedCommentAfter) && (column!=6) && !commented)
        {
          // we have some special cases in respect to strings and escaped string characters
          fullCommentLine=FALSE;
          newContents[j]=c;
          if (c == '\\')
          {
            inBackslash = !inBackslash;
            break;
          }
          else if (c == '\'')
          {
            if (!inDouble)
            {
              inSingle = !inSingle;
              if (inSingle) thisQuote = c;
              else thisQuote = '\0';
            }
            break;
          }
          else if (c == '"')
          {
            if (!inSingle)
            {
              inDouble = !inDouble;
              if (inDouble) thisQuote = c;
              else thisQuote = '\0';
            }
            break;
          }
        }
        inBackslash = FALSE;
        // fallthrough
      case '#':
      case 'C':
      case 'c':
      case '*':
      case '!':
        if ((column <= fixedCommentAfter) && (column!=6))
        {
          emptyLabel=FALSE;
          if (column==1)
          {
            newContents[j]='!';
            commented = TRUE;
          }
          else if ((c == '!') && !inDouble && !inSingle)
          {
            newContents[j]=c;
            commented = TRUE;
          }
          else
          {
            if (!commented) fullCommentLine=FALSE;
            newContents[j]=c;
          }
          break;
        }
        // fallthrough
      default:
        if (!commented && (column < 6) && ((c - '0') >= 0) && ((c - '0') <= 9))
        { // remove numbers, i.e. labels from first 5 positions.
            newContents[j]=' ';
        }
        else if (column==6 && emptyLabel)
        { // continuation
          if (!commented) fullCommentLine=FALSE;
          if (c != '0')
          { // 0 not allowed as continuation character, see f95 standard paragraph 3.3.2.3
            newContents[j]=' ';

            if (prevLineAmpOrExclIndex==-1)
            { // add & just before end of previous line
              /* first line is not a continuation line in code, just in snippets etc. */
              if (curLine != 1) insertCharacter(newContents, j+1, (j+1)-6-1, '&');
              j++;
            }
            else
            { // add & just before end of previous line comment
              /* first line is not a continuation line in code, just in snippets etc. */
              if (curLine != 1) insertCharacter(newContents, j+1, (j+1)-6-prevLineLength+prevLineAmpOrExclIndex+skipped, '&');
              skipped = 0;
              j++;
            }
            if (hasContLine)
            {
              hasContLine[curLine - 1] = 1;
            }
          }
          else
          {
            newContents[j]=c; // , just handle like space
          }
          prevLineLength=0;
        }
        else if ((column > fixedCommentAfter) && !commented)
        {
          // first non commented non blank character after position fixedCommentAfter
          if (c == '&')
          {
            newContents[j]=' ';
          }
          else if (c != '!')
          {
            // I'm not a possible start of doxygen comment
            newContents[j]=' ';
            artificialComment = TRUE;
            spaces=TRUE;
            skipped = 0;
          }
          else
          {
            newContents[j]=c;
            commented = TRUE;
          }
        }
        else
        {
          if (!commented) fullCommentLine=FALSE;
          newContents[j]=c;
          emptyLabel=FALSE;
        }
        break;
    }
  }

  if (hasContLine)
  {
    free(newContents);
    return nullptr;
  }

  if (j==-1) // contents was empty
  {
    newContents = (char*)realloc(newContents, 2);
    newContents[0] = '\n';
    newContents[1] = '\000';
  }
  else if (newContents[j] == '\n') // content ended with newline
  {
    newContents = (char*)realloc(newContents, j+2);
    newContents[j + 1] = '\000';
  }
  else // content did not end with a newline
  {
    newContents = (char*)realloc(newContents, j+3);
    newContents[j + 1] = '\n';
    newContents[j + 2] = '\000';
  }
  return newContents;
}

static void pushBuffer(yyscan_t yyscanner,const QCString &buffer)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->includeStackCnt <= yyextra->includeStackPtr)
  {
     yyextra->includeStackCnt++;
     yyextra->includeStack = (YY_BUFFER_STATE *)realloc(yyextra->includeStack, yyextra->includeStackCnt * sizeof(YY_BUFFER_STATE));
  }
  yyextra->includeStack[yyextra->includeStackPtr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(buffer.data(),yyscanner),yyscanner);

  DBG_CTX((stderr, "--PUSH--%s", qPrint(buffer)));
}

static void popBuffer(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr, "--POP--"));
  yyextra->includeStackPtr --;
  yy_delete_buffer( YY_CURRENT_BUFFER, yyscanner );
  yy_switch_to_buffer( yyextra->includeStack[yyextra->includeStackPtr], yyscanner );
}

/** used to copy entry to an interface module procedure */
static void copyEntry(std::shared_ptr<Entry> dest, const std::shared_ptr<Entry> &src)
{
   dest->type        = src->type;
   dest->fileName    = src->fileName;
   dest->startLine   = src->startLine;
   dest->bodyLine    = src->bodyLine;
   dest->endBodyLine = src->endBodyLine;
   dest->args        = src->args;
   dest->argList     = src->argList;
   dest->doc         = src->doc;
   dest->docLine  = src->docLine;
   dest->docFile  = src->docFile;
   dest->brief       = src->brief;
   dest->briefLine= src->briefLine;
   dest->briefFile= src->briefFile;
}

/** fill empty interface module procedures with info from
    corresponding module subprogs

    TODO: handle procedures in used modules
*/
void resolveModuleProcedures(yyscan_t yyscanner,Entry *current_root)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->moduleProcedures.empty()) return;
 
  // build up map of available functions
  std::map<std::string,std::shared_ptr<Entry>> procMap;
  {
    for (const auto& cf: current_root->children())
    {
      if (!cf->section.isFunction())
        continue;

      // remove scope from name
      QCString name = cf->name;
      {
        int end = name.findRev(":");
        if (end != -1) 
          name.remove(0, end+1);
      }

      procMap.emplace(name.str(), cf);
    }
  }


  // for all module procedures
  for (const auto& ce1: yyextra->moduleProcedures)
  {
    if (procMap.find(ce1->name.str())!=procMap.end())
    {
      std::shared_ptr<Entry> proc = procMap[ce1->name.str()];
      copyEntry(ce1, proc);
    }
  } // for all interface module procedures
  yyextra->moduleProcedures.clear();
}

/*! Extracts string which resides within parentheses of provided string. */
static QCString extractFromParens(const QCString &name)
{
  QCString extracted = name;
  int start = extracted.find("(");
  if (start != -1)
  {
    extracted.remove(0, start+1);
  }
  int end = extracted.findRev(")");
  if (end != -1)
  {
    size_t length = extracted.length();
    extracted.remove(end, length);
  }
  extracted = extracted.stripWhiteSpace();

  return extracted;
}

/*! remove useless spaces from bind statement */
static QCString extractBind(const QCString &name)
{
  QCString parensPart = extractFromParens(name);
  if (parensPart.length() == 1)
  {
    return "bind(C)";
  }
  else
  {
    //strip 'c'
    parensPart = parensPart.mid(1).stripWhiteSpace();
    // strip ','
    parensPart = parensPart.mid(1).stripWhiteSpace();
    // name part
    parensPart = parensPart.mid(4).stripWhiteSpace();
    // = part
    parensPart = parensPart.mid(1).stripWhiteSpace();

    return "bind(C, name=" + parensPart + ")";
  }
}

/*! Adds passed yyextra->modifiers to these yyextra->modifiers.*/
SymbolModifiers& SymbolModifiers::operator|=(const SymbolModifiers &mdfs)
{
  if (mdfs.protection!=NONE_P) protection = mdfs.protection;
  if (mdfs.direction!=NONE_D) direction = mdfs.direction;
  optional |= mdfs.optional;
  if (!mdfs.dimension.isEmpty()) dimension = mdfs.dimension;
  allocatable |= mdfs.allocatable;
  external |= mdfs.external;
  intrinsic |= mdfs.intrinsic;
  protect |= mdfs.protect;
  parameter |= mdfs.parameter;
  pointer |= mdfs.pointer;
  target |= mdfs.target;
  save |= mdfs.save;
  deferred |= mdfs.deferred;
  nonoverridable |= mdfs.nonoverridable;
  nopass |= mdfs.nopass;
  pass |= mdfs.pass;
  passVar = mdfs.passVar;
  bindVar = mdfs.bindVar;
  contiguous |= mdfs.contiguous;
  volat |= mdfs.volat;
  value |= mdfs.value;
  return *this;
}

/*! Extracts and adds passed modifier to these yyextra->modifiers.*/
SymbolModifiers& SymbolModifiers::operator|=(QCString mdfStringArg)
{
  QCString mdfString = mdfStringArg.lower();
  SymbolModifiers newMdf;

  if (mdfString.find("dimension")==0)
  {
    newMdf.dimension=mdfString;
  }
  else if (mdfString.contains("intent"))
  {
    QCString tmp = extractFromParens(mdfString);
    bool isin = tmp.contains("in")!=0;
    bool isout = tmp.contains("out")!=0;
    if (isin && isout) newMdf.direction = SymbolModifiers::INOUT;
    else if (isin) newMdf.direction = SymbolModifiers::IN;
    else if (isout) newMdf.direction = SymbolModifiers::OUT;
  }
  else if (mdfString=="public")
  {
    newMdf.protection = SymbolModifiers::PUBLIC;
  }
  else if (mdfString=="private")
  {
    newMdf.protection = SymbolModifiers::PRIVATE;
  }
  else if (mdfString=="protected")
  {
    newMdf.protect = TRUE;
  }
  else if (mdfString=="optional")
  {
    newMdf.optional = TRUE;
  }
  else if (mdfString=="allocatable")
  {
    newMdf.allocatable = TRUE;
  }
  else if (mdfString=="external")
  {
    newMdf.external = TRUE;
  }
  else if (mdfString=="intrinsic")
  {
    newMdf.intrinsic = TRUE;
  }
  else if (mdfString=="parameter")
  {
    newMdf.parameter = TRUE;
  }
  else if (mdfString=="pointer")
  {
    newMdf.pointer = TRUE;
  }
  else if (mdfString=="target")
  {
    newMdf.target = TRUE;
  }
  else if (mdfString=="save")
  {
    newMdf.save = TRUE;
  }
  else if (mdfString=="nopass")
  {
    newMdf.nopass = TRUE;
  }
  else if (mdfString=="deferred")
  {
    newMdf.deferred = TRUE;
  }
  else if (mdfString=="non_overridable")
  {
    newMdf.nonoverridable = TRUE;
  }
  else if (mdfString=="contiguous")
  {
    newMdf.contiguous = TRUE;
  }
  else if (mdfString=="volatile")
  {
    newMdf.volat = TRUE;
  }
  else if (mdfString=="value")
  {
    newMdf.value = TRUE;
  }
  else if (mdfString.contains("pass"))
  {
    newMdf.pass = TRUE;
    if (mdfString.contains("("))
      newMdf.passVar = extractFromParens(mdfString);
    else
      newMdf.passVar = "";
  }
  else if (mdfString.startsWith("bind"))
  {
    // we need here the original string as we want to don't want to have the lowercase name between the quotes of the name= part
    newMdf.bindVar = extractBind(mdfStringArg);
  }

  (*this) |= newMdf;
  return *this;
}

/*! For debugging purposes. */
//ostream& operator<<(ostream& out, const SymbolModifiers& mdfs)
//{
//  out<<mdfs.protection<<", "<<mdfs.direction<<", "<<mdfs.optional<<
//    ", "<<(mdfs.dimension.isEmpty() ? "" : mdfs.dimension.latin1())<<
//    ", "<<mdfs.allocatable<<", "<<mdfs.external<<", "<<mdfs.intrinsic;
//
//  return out;
//}

/*! Find argument with given name in \a subprog entry. */
static Argument *findArgument(Entry* subprog, QCString name, bool byTypeName = FALSE)
{
  QCString cname(name.lower());
  for (Argument &arg : subprog->argList)
  {
    if ((!byTypeName && arg.name.lower() == cname) ||
         (byTypeName && arg.type.lower() == cname)
       )
    {
      return &arg;
    }
  }
  return nullptr;
}


/*! Apply yyextra->modifiers stored in \a mdfs to the \a typeName string. */
static QCString applyModifiers(QCString typeName, const SymbolModifiers& mdfs)
{
  if (!mdfs.dimension.isEmpty())
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += mdfs.dimension;
  }
  if (mdfs.direction!=SymbolModifiers::NONE_D)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += directionStrs[mdfs.direction];
  }
  if (mdfs.optional)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "optional";
  }
  if (mdfs.allocatable)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "allocatable";
  }
  if (mdfs.external)
  {
    if (!typeName.contains("external"))
    {
      if (!typeName.isEmpty()) typeName += ", ";
      typeName += "external";
    }
  }
  if (mdfs.intrinsic)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "intrinsic";
  }
  if (mdfs.parameter)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "parameter";
  }
  if (mdfs.pointer)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "pointer";
  }
  if (mdfs.target)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "target";
  }
  if (mdfs.save)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "save";
  }
  if (mdfs.deferred)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "deferred";
  }
  if (mdfs.nonoverridable)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "non_overridable";
  }
  if (mdfs.nopass)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "nopass";
  }
  if (mdfs.pass)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "pass";
    if (!mdfs.passVar.isEmpty())
      typeName += "(" + mdfs.passVar + ")";
  }
  if (!mdfs.bindVar.isEmpty())
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += mdfs.bindVar;
  }
  if (mdfs.protection == SymbolModifiers::PUBLIC)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "public";
  }
  else if (mdfs.protection == SymbolModifiers::PRIVATE)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "private";
  }
  if (mdfs.protect)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "protected";
  }
  if (mdfs.contiguous)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "contiguous";
  }
  if (mdfs.volat)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "volatile";
  }
  if (mdfs.value)
  {
    if (!typeName.isEmpty()) typeName += ", ";
    typeName += "value";
  }

  return typeName;
}

/*! Apply yyextra->modifiers stored in \a mdfs to the \a arg argument. */
static void applyModifiers(Argument *arg, const SymbolModifiers& mdfs)
{
  arg->type = applyModifiers(arg->type, mdfs);
}

/*! Apply yyextra->modifiers stored in \a mdfs to the \a ent entry. */
static void applyModifiers(Entry *ent, const SymbolModifiers& mdfs)
{
  ent->type = applyModifiers(ent->type, mdfs);

  if (mdfs.protection == SymbolModifiers::PUBLIC)
    ent->protection = Protection::Public;
  else if (mdfs.protection == SymbolModifiers::PRIVATE)
    ent->protection = Protection::Private;

  if (mdfs.nonoverridable)
    ent->spec.setFinal(true);
  if (mdfs.nopass)
    ent->isStatic = TRUE;
  if (mdfs.deferred)
    ent->virt = Specifier::Pure;
}

/*! Starts the new scope in fortran program. Consider using this function when
 * starting module, interface, function or other program block.
 * \see endScope()
 */
static void startScope(yyscan_t yyscanner,Entry *scope)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //cout<<"start scope: "<<scope->name<<endl;
  yyextra->current_root= scope; /* start substructure */

  yyextra->modifiers.emplace(scope, std::map<std::string,SymbolModifiers>());

  // create new current with possibly different defaults...
  yyextra->current = std::make_shared<Entry>();
  initEntry(yyscanner);
}

/*! Ends scope in fortran program: may update subprogram arguments or module variable attributes.
 * \see startScope()
 */
static bool endScope(yyscan_t yyscanner,Entry *scope, bool isGlobalRoot)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->global_scope == scope)
  {
    yyextra->global_scope = nullptr;
    return TRUE;
  }
  if (yyextra->global_scope == INVALID_ENTRY)
  {
    return TRUE;
  }
  //cout<<"end scope: "<<scope->name<<endl;
  if (yyextra->current_root->parent() || isGlobalRoot)
  {
    yyextra->current_root= yyextra->current_root->parent(); /* end substructure */
  }
  else // if (yyextra->current_root != scope)
  {
    fprintf(stderr,"parse error in end <scopename>\n");
    scanner_abort(yyscanner);
    return FALSE;
  }

  // create new current with possibly different defaults...
  yyextra->current = std::make_shared<Entry>();
  initEntry(yyscanner);

  // update variables or subprogram arguments with yyextra->modifiers
  std::map<std::string,SymbolModifiers>& mdfsMap = yyextra->modifiers[scope];

  if (scope->section.isFunction())
  {
    // iterate all symbol yyextra->modifiers of the scope
    for (const auto &kv : mdfsMap)
    {
      //cout<<it.key()<<": "<<qPrint(it)<<endl;
      Argument *arg = findArgument(scope, kv.first);

      if (arg)
      {
        applyModifiers(arg, kv.second);
      }
    }

    // find return type for function
    //cout<<"RETURN NAME "<<yyextra->modifiers[yyextra->current_root][scope->name.lower()].returnName<<endl;
    QCString returnName = yyextra->modifiers[yyextra->current_root][scope->name.lower().str()].returnName.lower();
    if (yyextra->modifiers[scope].find(returnName.str())!=yyextra->modifiers[scope].end())
    {
      scope->type = yyextra->modifiers[scope][returnName.str()].type; // returning type works
      applyModifiers(scope, yyextra->modifiers[scope][returnName.str()]); // returning array works
    }

  }
  if (scope->section.isClass() && scope->spec.isInterface())
  { // was INTERFACE_SEC
    if (scope->parent() && scope->parent()->section.isFunction())
    { // interface within function
      // iterate functions of interface and
      // try to find types for dummy(ie. argument) procedures.
      //cout<<"Search in "<<scope->name<<endl;
      for (const auto &ce : scope->children())
      {
        if (!ce->section.isFunction())
          continue;

        // remove prefix
        QCString name = ce->name.lower();
        int ii = name.findRev(":");
        if (ii != -1)
        {
          name.remove(0, ii+1);
        }
        Argument *arg = findArgument(scope->parent(), name);
        if (arg)
        {
          // set type of dummy procedure argument to interface
          arg->type = "external " + ce->type + "(";
          for (unsigned int i=0; i<ce->argList.size(); i++)
          {
            if (i > 0)
            {
              arg->type = arg->type + ", ";
            }
            const Argument &subarg = ce->argList.at(i);
            arg->type = arg->type + subarg.type + " " + subarg.name;
          }
          arg->type = arg->type + ")";
          arg->name = name;
        }
      }
      // clear all yyextra->modifiers of the scope
      yyextra->modifiers.erase(scope);
      scope->parent()->removeSubEntry(scope);
      scope = nullptr;
      return TRUE;
    }
  }
  if (!scope->section.isFunction())
  { // not function section
    // iterate variables: get and apply yyextra->modifiers
    for (const auto &ce : scope->children())
    {
      if (!ce->section.isVariable() && !ce->section.isFunction() && !ce->section.isClass())
        continue;

      //cout<<ce->name<<", "<<mdfsMap.contains(ce->name.lower())<<mdfsMap.count()<<endl;
      if (mdfsMap.find(ce->name.lower().str())!=mdfsMap.end())
        applyModifiers(ce.get(), mdfsMap[ce->name.lower().str()]);

      // remove prefix for variable names
      if (ce->section.isVariable() || ce->section.isFunction())
      {
        int end = ce->name.findRev(":");
        if (end != -1) 
          ce->name.remove(0, end+1);
      }
    }
  }

  // clear all yyextra->modifiers of the scope
  yyextra->modifiers.erase(scope);

  // resolve procedures in types
  resolveTypeBoundProcedures(scope);

  return TRUE;
}

/*! search for types with type bound procedures (e.g. methods)
 *  and try to resolve their arguments
 */
static void resolveTypeBoundProcedures(Entry *scope)
{
  // map of all subroutines/functions
  bool procMapCreated = false;
  std::unordered_map<std::string,std::shared_ptr<Entry>> procMap;

  // map of all abstract interfaces
  bool interfMapCreated = false;
  std::unordered_map<std::string,std::shared_ptr<Entry>> interfMap;

  // iterate over all types
  for (const auto &ce: scope->children())
  {
    if (!ce->section.isClass())
      continue;

    // handle non-"generic" non-"deferred" methods, copying the arguments from the implementation
    std::unordered_map<std::string,std::shared_ptr<Entry>> methodMap;
    for (auto &ct: ce->children())
    {
      if (!ct->section.isFunction())
        continue;

      if (ct->type=="generic")
        continue;

      if (ct->virt==Specifier::Pure)
        continue;

      // set up the procMap
      if (!procMapCreated)
      {
        for (const auto &cf: scope->children())
        {
          if (cf->section.isFunction())
          {
            procMap.emplace(cf->name.str(), cf);
          }
        }
        procMapCreated = true;
      }

      // found a (non-generic) method
      QCString implName = ct->args;
      if (procMap.find(implName.str())!=procMap.end())
      {
        std::shared_ptr<Entry> proc = procMap[implName.str()];
        ct->args = proc->args;
        ct->argList = ArgumentList(proc->argList);
        if (ct->brief.isEmpty())
        {
          ct->brief = proc->brief;
          ct->briefLine = proc->briefLine;
          ct->briefFile = proc->briefFile;
        }
        if (ct->doc.isEmpty())
        {
          ct->doc = proc->doc;
          ct->docLine = proc->docLine;
          ct->docFile = proc->docFile;
        }
        methodMap.emplace(ct->name.str(), ct);
      }
    }

    // handle "deferred" methods (pure virtual functions), duplicating with arguments from the target abstract interface
    for (auto &ct: ce->children())
    {
      if (!ct->section.isFunction())
        continue;

      if (ct->virt != Specifier::Pure)
        continue;

      // set up the procMap
      if (!interfMapCreated)
      {
        for(const auto &cf: scope->children())
        {
          if (cf->section.isClass() && cf->spec.isInterface() && cf->type=="abstract")
          {
            std::shared_ptr<Entry> ci = cf->children().front();
            interfMap.emplace(ci->name.str(), ci);
          }
        }
        interfMapCreated = true;
      }

      // found a (non-generic) method
      QCString implName = ct->args;
      if (interfMap.find(implName.str())!= interfMap.end() )
      {
        std::shared_ptr<Entry> proc = interfMap[implName.str()];
        ct->args = proc->args;
        ct->argList = ArgumentList(proc->argList);
        if (ct->brief.isEmpty())
        {
          ct->brief = proc->brief;
          ct->briefLine = proc->briefLine;
          ct->briefFile = proc->briefFile;
        }
        if (ct->doc.isEmpty())
        {
          ct->doc = proc->doc;
          ct->docLine = proc->docLine;
          ct->docFile = proc->docFile;
        }

        methodMap.emplace(ct->name.str(), ct);
      }
    }

    // handle "generic" methods (that is function overloading!), duplicating with arguments from the target method of the type
    {
      for (auto &ct: ce->children())
      {
        if (!ct->section.isFunction())
          continue;

        if (ct->type!="generic")
          continue;

        // found a generic method (already duplicated for each entry by the parser)
        QCString methodName = ct->args;
        if (methodMap.find(methodName.str()) != methodMap.end())
        {
          std::shared_ptr<Entry> method = methodMap[methodName.str()];
          ct->args = method->args;
          ct->argList = ArgumentList(method->argList);
          if (ct->brief.isEmpty())
          {
            ct->brief = method->brief;
            ct->briefLine = method->briefLine;
            ct->briefFile = method->briefFile;
          }
          if (ct->doc.isEmpty())
          {
            ct->doc = method->doc;
            ct->docLine = method->docLine;
            ct->docFile = method->docFile;
          }
        }
      }
    }
  }
}

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  while ( c < max_size && yyextra->inputString[yyextra->inputPosition] )
  {
    *buf = yyextra->inputString[yyextra->inputPosition++] ;
    c++; buf++;
  }
  return c;
}

static void initParser(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->last_entry.reset();
}

static void initEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->typeMode)
  {
    yyextra->current->protection = yyextra->typeProtection;
  }
  else if (yyextra->current_root && yyextra->current_root->section.isClass() && yyextra->current_root->spec.isInterface())
  {
    yyextra->current->protection = Protection::Public;
  }
  else if (yyextra->current_root && yyextra->current_root->section.isFunction())
  {
    yyextra->current->protection = Protection::Private;
  }
  else
  {
    yyextra->current->protection = yyextra->defaultProtection;
  }
  yyextra->current->mtype      = MethodTypes::Method;
  yyextra->current->virt       = Specifier::Normal;
  yyextra->current->isStatic   = false;
  yyextra->current->lang       = SrcLangExt::Fortran;
  yyextra->commentScanner.initGroupInfo(yyextra->current.get());
}

/**
  adds yyextra->current entry to yyextra->current_root and creates new yyextra->current
*/
static void addCurrentEntry(yyscan_t yyscanner,bool case_insens)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (case_insens) yyextra->current->name = yyextra->current->name.lower();
  //printf("===Adding entry %s to %s\n", qPrint(yyextra->current->name), qPrint(yyextra->current_root->name));
  yyextra->last_entry = yyextra->current;
  yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  initEntry(yyscanner);
}

static void addModule(yyscan_t yyscanner,const QCString &name, bool isModule)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr, "0=========> got module %s\n", qPrint(name)));

  if (isModule)
    yyextra->current->section = EntryType::makeNamespace();
  else
    yyextra->current->section = EntryType::makeFunction();

  if (!name.isEmpty())
  {
    yyextra->current->name = name;
  }
  else
  {
    QCString fname = yyextra->fileName;
    int index = std::max(fname.findRev('/'), fname.findRev('\\'));
    fname = fname.right(fname.length()-index-1);
    if (yyextra->mainPrograms) fname += "__" + QCString().setNum(yyextra->mainPrograms);
    yyextra->mainPrograms++;
    fname = fname.prepend("__").append("__");
    yyextra->current->name = substitute(fname, ".", "_");
  }
  yyextra->current->type = "program";
  yyextra->current->fileName  = yyextra->fileName;
  yyextra->current->bodyLine  = yyextra->lineNr; // used for source reference
  yyextra->current->startLine  = yyextra->lineNr;
  yyextra->current->protection = Protection::Public ;
  addCurrentEntry(yyscanner,true);
  startScope(yyscanner,yyextra->last_entry.get());
}


static void addSubprogram(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"1=========> got subprog, type: %s\n",qPrint(text)));
  yyextra->subrCurrent.push_back(yyextra->current);
  yyextra->current->section = EntryType::makeFunction();
  QCString subtype = text; subtype=subtype.lower().stripWhiteSpace();
  yyextra->functionLine = (subtype.find("function") != -1);
  yyextra->current->type += " " + subtype;
  yyextra->current->type = yyextra->current->type.stripWhiteSpace();
  if (yyextra->ifType == IF_ABSTRACT)
  {
    yyextra->current->virt = Specifier::Virtual;
  }
  yyextra->current->fileName  = yyextra->fileName;
  yyextra->current->bodyLine  = yyextra->lineNr; // used for source reference start of body of routine
  yyextra->current->startLine  = yyextra->lineNr; // used for source reference start of definition
  yyextra->current->args.clear();
  yyextra->current->argList.clear();
  yyextra->docBlock.clear();
}

/*! Adds interface to the root entry.
 * \note Code was brought to this procedure from the parser,
 * because there was/is idea to use it in several parts of the parser.
 */
static void addInterface(yyscan_t yyscanner,QCString name, InterfaceType type)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (YY_START == Start)
  {
    addModule(yyscanner);
    yy_push_state(ModuleBody,yyscanner); //anon program
  }

  yyextra->current->section = EntryType::makeClass(); // was EntryType::Interface;
  yyextra->current->spec = TypeSpecifier().setInterface(true);
  yyextra->current->name = name;

  switch (type)
  {
    case IF_ABSTRACT:
      yyextra->current->type = "abstract";
      break;

    case IF_GENERIC:
      yyextra->current->type = "generic";
      break;

    case IF_SPECIFIC:
    case IF_NONE:
    default:
      yyextra->current->type = "";
  }

  /* if type is part of a module, mod name is necessary for output */
  if ((yyextra->current_root) &&
      (yyextra->current_root->section.isClass() ||
       yyextra->current_root->section.isNamespace()))
  {
    yyextra->current->name= yyextra->current_root->name + "::" + yyextra->current->name;
  }

  yyextra->current->fileName = yyextra->fileName;
  yyextra->current->bodyLine  = yyextra->lineNr;
  yyextra->current->startLine  = yyextra->lineNr;
  addCurrentEntry(yyscanner,true);
}


//-----------------------------------------------------------------------------

/*! Get the argument \a name.
 */
static Argument *getParameter(yyscan_t yyscanner,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // std::cout<<"addFortranParameter(): "<<name<<" DOCS:"<<(docs.isEmpty()?QCString("null"):docs)<<"\n";
  Argument *ret = nullptr;
  for (Argument &a:yyextra->current_root->argList)
  {
    if (a.name.lower()==name.lower())
    {
      ret=&a;
      //printf("parameter found: %s\n",(const char*)name);
      break;
    }
  } // for
  return ret;
}

  //----------------------------------------------------------------------------
static void startCommentBlock(yyscan_t yyscanner,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (brief)
  {
    yyextra->current->briefFile = yyextra->fileName;
    yyextra->current->briefLine = yyextra->lineNr;
  }
  else
  {
    yyextra->current->docFile = yyextra->fileName;
    yyextra->current->docLine = yyextra->lineNr;
  }
}

//----------------------------------------------------------------------------

static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool hideInBodyDocs = Config_getBool(HIDE_IN_BODY_DOCS);
  if (yyextra->docBlockInBody && hideInBodyDocs)
  {
    yyextra->docBlockInBody = FALSE;
    return;
  }
  DBG_CTX((stderr,"call parseCommentBlock [%s]\n",qPrint(doc)));
  int lineNr = brief ? yyextra->current->briefLine : yyextra->current->docLine;
  int position=0;
  bool needsEntry = FALSE;
  Markdown markdown(yyextra->fileName,lineNr);
  GuardedSectionStack guards;
  QCString strippedDoc = stripIndentation(doc);
  QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,lineNr) : strippedDoc;
  while (yyextra->commentScanner.parseCommentBlock(
        yyextra->thisParser,
        yyextra->docBlockInBody ? yyextra->subrCurrent.back().get() : yyextra->current.get(),
        processedDoc, // text
        yyextra->fileName, // file
        lineNr,
        yyextra->docBlockInBody ? FALSE : brief,
        yyextra->docBlockInBody ? FALSE : yyextra->docBlockJavaStyle,
        yyextra->docBlockInBody,
        yyextra->defaultProtection,
        position,
        needsEntry,
        Config_getBool(MARKDOWN_SUPPORT),
        &guards
        ))
  {
    DBG_CTX((stderr,"parseCommentBlock position=%d [%s]  needsEntry=%d\n",position,doc.data()+position,needsEntry));
    if (needsEntry) addCurrentEntry(yyscanner,false);
  }
  DBG_CTX((stderr,"parseCommentBlock position=%d [%s]  needsEntry=%d\n",position,doc.data()+position,needsEntry));

  if (needsEntry) addCurrentEntry(yyscanner,false);
  yyextra->docBlockInBody = FALSE;
}

//----------------------------------------------------------------------------
/// Handle parameter description as defined after the declaration of the parameter
static void subrHandleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString loc_doc;
  loc_doc = doc.stripWhiteSpace();

  std::shared_ptr<Entry> tmp_entry = yyextra->current;
  yyextra->current = yyextra->subrCurrent.back(); // temporarily switch to the entry of the subroutine / function

  // Still in the specification section so no inbodyDocs yet, but parameter documentation
  yyextra->current->inbodyDocs = "";

  // strip \\param or @param, so we can do some extra checking. We will add it later on again.
  if (!loc_doc.stripPrefix("\\param") &&
      !loc_doc.stripPrefix("@param")
     ) (void)loc_doc; // Do nothing work has been done by stripPrefix; (void)loc_doc: to overcome 'empty controlled statement' warning
  loc_doc.stripWhiteSpace();

  // direction as defined with the declaration of the parameter
  int dir1 = yyextra->modifiers[yyextra->current_root][yyextra->argName.lower().str()].direction;
  // in description [in] is specified
  if (loc_doc.lower().find(directionParam[SymbolModifiers::IN]) == 0)
  {
    // check if with the declaration intent(in) or nothing has been specified
    if ((directionParam[dir1] == directionParam[SymbolModifiers::NONE_D]) ||
        (directionParam[dir1] == directionParam[SymbolModifiers::IN]))
    {
      // strip direction
      loc_doc = loc_doc.right(loc_doc.length()-(int)strlen(directionParam[SymbolModifiers::IN]));
      loc_doc.stripWhiteSpace();
      // in case of empty documentation or (now) just name, consider it as no documentation
      if (!loc_doc.isEmpty() && (loc_doc.lower() != yyextra->argName.lower()))
      {
        handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[SymbolModifiers::IN] + " " +
                         yyextra->argName + " " + loc_doc,brief);
      }
    }
    else
    {
      // something different specified, give warning and leave error.
      warn(yyextra->fileName,yyextra->lineNr, "Routine: {}{} inconsistency between intent attribute and documentation for parameter {}:",
             yyextra->current->name,yyextra->current->args,yyextra->argName);
      handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[dir1] + " " +
                         yyextra->argName + " " + loc_doc,brief);
    }
  }
  // analogous to the [in] case, here [out] direction specified
  else if (loc_doc.lower().find(directionParam[SymbolModifiers::OUT]) == 0)
  {
    if ((directionParam[dir1] == directionParam[SymbolModifiers::NONE_D]) ||
        (directionParam[dir1] == directionParam[SymbolModifiers::OUT]))
    {
      loc_doc = loc_doc.right(loc_doc.length()-(int)strlen(directionParam[SymbolModifiers::OUT]));
      loc_doc.stripWhiteSpace();
      if (loc_doc.isEmpty() || (loc_doc.lower() == yyextra->argName.lower()))
      {
        yyextra->current = tmp_entry;
        return;
      }
      handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[SymbolModifiers::OUT] + " " +
                         yyextra->argName + " " + loc_doc,brief);
    }
    else
    {
      warn(yyextra->fileName,yyextra->lineNr, "Routine: {}{} inconsistency between intent attribute and documentation for parameter {}:",
             yyextra->current->name,yyextra->current->args,yyextra->argName);
      handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[dir1] + " " +
                         yyextra->argName + " " + loc_doc,brief);
    }
  }
  // analogous to the [in] case, here [in,out] direction specified
  else if (loc_doc.lower().find(directionParam[SymbolModifiers::INOUT]) == 0)
  {
    if ((directionParam[dir1] == directionParam[SymbolModifiers::NONE_D]) ||
        (directionParam[dir1] == directionParam[SymbolModifiers::INOUT]))
    {
      loc_doc = loc_doc.right(loc_doc.length()-(int)strlen(directionParam[SymbolModifiers::INOUT]));
      loc_doc.stripWhiteSpace();
      if (!loc_doc.isEmpty() && (loc_doc.lower() != yyextra->argName.lower()))
      {
        handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[SymbolModifiers::INOUT] + " " +
                           yyextra->argName + " " + loc_doc,brief);
      }
    }
    else
    {
      warn(yyextra->fileName,yyextra->lineNr, "Routine: {}{} inconsistency between intent attribute and documentation for parameter {}:",
             yyextra->current->name,yyextra->current->args,yyextra->argName);
      handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[dir1] + " " +
                         yyextra->argName + " " + loc_doc,brief);
    }
  }
  // analogous to the [in] case; here no direction specified
  else if (!loc_doc.isEmpty() && (loc_doc.lower() != yyextra->argName.lower()))
  {
    handleCommentBlock(yyscanner,QCString("\n\n@param ") + directionParam[dir1] + " " +
                       yyextra->argName + " " + loc_doc,brief);
  }

  // reset yyextra->current back to the part inside the routine
  yyextra->current = tmp_entry;
}
//----------------------------------------------------------------------------
/// Handle result description as defined after the declaration of the parameter
static void subrHandleCommentBlockResult(yyscan_t yyscanner,const QCString &doc,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString loc_doc;
  loc_doc = doc.stripWhiteSpace();

  std::shared_ptr<Entry> tmp_entry = yyextra->current;
  yyextra->current = yyextra->subrCurrent.back(); // temporarily switch to the entry of the subroutine / function

  // Still in the specification section so no inbodyDocs yet, but parameter documentation
  yyextra->current->inbodyDocs = "";

  // strip \\returns or @returns. We will add it later on again.
  if (!loc_doc.stripPrefix("\\returns") &&
      !loc_doc.stripPrefix("\\return") &&
      !loc_doc.stripPrefix("@returns") &&
      !loc_doc.stripPrefix("@return")
     ) (void)loc_doc; // Do nothing work has been done by stripPrefix; (void)loc_doc: to overcome 'empty controlled statement' warning
  loc_doc.stripWhiteSpace();

  if (!loc_doc.isEmpty() && (loc_doc.lower() != yyextra->argName.lower()))
  {
    handleCommentBlock(yyscanner,QCString("\n\n@returns ") + loc_doc,brief);
  }

  // reset yyextra->current back to the part inside the routine
  yyextra->current = std::move(tmp_entry);
}

//----------------------------------------------------------------------------

static void parseMain(yyscan_t yyscanner, const QCString &fileName,const char *fileBuf,
                      const std::shared_ptr<Entry> &rt, FortranFormat format)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  char *tmpBuf = nullptr;
  initParser(yyscanner);

  if (fileBuf==nullptr || fileBuf[0]=='\0') return;

  yyextra->defaultProtection = Protection::Public;
  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;
  yyextra->inputStringPrepass = nullptr;
  yyextra->inputPositionPrepass = 0;

  //yyextra->anonCount     = 0;  // don't reset per file
  yyextra->current_root  = rt.get();
  yyextra->global_root   = rt;

  yyextra->isFixedForm = recognizeFixedForm(fileBuf,format);

  if (yyextra->isFixedForm)
  {
    yyextra->fixedCommentAfter = Config_getInt(FORTRAN_COMMENT_AFTER);
    msg("Prepassing fixed form of {}\n", fileName);
    //printf("---strlen=%d\n", strlen(fileBuf));
    //clock_t start=clock();

    //printf("Input fixed form string:\n%s\n", fileBuf);
    //printf("===========================\n");
    yyextra->inputString = prepassFixedForm(fileBuf, nullptr,yyextra->fixedCommentAfter);
    Debug::print(Debug::FortranFixed2Free,0,"======== Fixed to Free format  =========\n---- Input fixed form string ------- \n{}\n", fileBuf);
    Debug::print(Debug::FortranFixed2Free,0,"---- Resulting free form string ------- \n{}\n", yyextra->inputString);
    //printf("Resulting free form string:\n%s\n", yyextra->inputString);
    //printf("===========================\n");

    //clock_t end=clock();
    //printf("CPU time used=%f\n", ((double) (end-start))/CLOCKS_PER_SEC);
  }
  else if (yyextra->inputString[strlen(fileBuf)-1] != '\n')
  {
    tmpBuf = (char *)malloc(strlen(fileBuf)+2);
    strcpy(tmpBuf,fileBuf);
    tmpBuf[strlen(fileBuf)]= '\n';
    tmpBuf[strlen(fileBuf)+1]= '\000';
    yyextra->inputString = tmpBuf;
  }

  yyextra->lineNr= 1 ;
  yyextra->fileName = fileName;
  msg("Parsing file {}...\n",yyextra->fileName);

  yyextra->global_scope = rt.get();
  startScope(yyscanner,rt.get()); // implies yyextra->current_root = rt
  initParser(yyscanner);
  yyextra->commentScanner.enterFile(yyextra->fileName,yyextra->lineNr);

  // add entry for the file
  yyextra->current          = std::make_shared<Entry>();
  yyextra->current->lang    = SrcLangExt::Fortran;
  yyextra->current->name    = yyextra->fileName;
  yyextra->current->section = EntryType::makeSource();
  yyextra->file_root        = yyextra->current;
  yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  yyextra->current->lang    = SrcLangExt::Fortran;

  fortranscannerYYrestart( nullptr, yyscanner );
  {
    BEGIN( Start );
  }

  fortranscannerYYlex(yyscanner);
  yyextra->commentScanner.leaveFile(yyextra->fileName,yyextra->lineNr);

  if (yyextra->global_scope && yyextra->global_scope != INVALID_ENTRY)
  {
    endScope(yyscanner,yyextra->current_root, TRUE); // TRUE - global root
  }

  //debugCompounds(rt); //debug

  rt->program.str(std::string());
  //delete yyextra->current; yyextra->current=0;
  yyextra->moduleProcedures.clear();
  if (tmpBuf)
  {
    free((char*)tmpBuf);
    yyextra->inputString=nullptr;
  }
  if (yyextra->isFixedForm)
  {
    free((char*)yyextra->inputString);
    yyextra->inputString=nullptr;
  }

}

//----------------------------------------------------------------------------

struct FortranOutlineParser::Private
{
  yyscan_t yyscanner;
  fortranscannerYY_state extra;
  FortranFormat format;
  Private(FortranFormat fmt) : format(fmt)
  {
    fortranscannerYYlex_init_extra(&extra,&yyscanner);
#ifdef FLEX_DEBUG
    fortranscannerYYset_debug(Debug::isFlagSet(Debug::Lex_fortranscanner) ? 1 : 0,yyscanner);
#endif
  }
};

FortranOutlineParser::FortranOutlineParser(FortranFormat format)
   : p(std::make_unique<Private>(format))
{
}

FortranOutlineParser::~FortranOutlineParser()
{
  fortranscannerYYlex_destroy(p->yyscanner);
}

void FortranOutlineParser::parseInput(const QCString &fileName,
                                      const char *fileBuf,
                                      const std::shared_ptr<Entry> &root,
                                      ClangTUParser * /*clangParser*/)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->thisParser = this;

  DebugLex debugLex(Debug::Lex_fortranscanner, __FILE__, qPrint(fileName));

  ::parseMain(p->yyscanner,fileName,fileBuf,root,p->format);
}

bool FortranOutlineParser::needsPreprocessing(const QCString &extension) const
{
  return extension!=extension.lower(); // use preprocessor only for upper case extensions
}

void FortranOutlineParser::parsePrototype(const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  pushBuffer(p->yyscanner,text);
  yyextra->parsingPrototype = TRUE;
  BEGIN(Prototype);
  fortranscannerYYlex(p->yyscanner);
  yyextra->parsingPrototype = FALSE;
  popBuffer(p->yyscanner);
}

//----------------------------------------------------------------------------

static void scanner_abort(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  fprintf(stderr,"********************************************************************\n");
  if (yyextra->blockLineNr == -1)
  {
    fprintf(stderr,"Error in file %s line: %d, state: %d(%s)\n",
      qPrint(yyextra->fileName),yyextra->lineNr,YY_START,stateToString(YY_START));
  }
  else
  {
    fprintf(stderr,"Error in file %s line: %d, state: %d(%s), starting command: '%s' probable line reference: %d\n",
      qPrint(yyextra->fileName),yyextra->lineNr,YY_START,stateToString(YY_START),qPrint(yyextra->blockString),yyextra->blockLineNr);
  }
  fprintf(stderr,"********************************************************************\n");

  bool start=FALSE;

  for (const auto &ce : yyextra->global_root->children())
  {
     if (ce == yyextra->file_root) start=TRUE;
     if (start) ce->reset();
  }

  // dummy call to avoid compiler warning
  (void)yy_top_state(yyscanner);

  return;
  //exit(-1);
}

static inline void pop_state(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if ( yyg->yy_start_stack_ptr <= 0 )
    warn(yyextra->fileName,yyextra->lineNr,"Unexpected statement '{}'",yytext );
  else
    yy_pop_state(yyscanner);
}
//----------------------------------------------------------------------------

#include "fortranscanner.l.h"
