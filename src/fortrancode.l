/******************************************************************************
 *
 * Parser for syntax highlighting and references for Fortran90 F subset
 *
 * Copyright (C) by Anke Visser
 * based on the work of Dimitri van Heesch.
 * Copyright (C) 2020 by Dimitri van Heesch.
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

/**
 @todo
       - continuation lines not always recognized
       - merging of use-statements with same module name and different only-names
       - rename part of use-statement
       - links to interface functions
       - references to variables
**/
%option never-interactive
%option case-insensitive
%option reentrant
%option prefix="fortrancodeYY"
%option extra-type="struct fortrancodeYY_state *"
%option noyy_top_state
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
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "defargs.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "tooltip.h"
#include "fortrancode.h"
#include "fortranscanner.h"
#include "containers.h"
#include "debug.h"
#include "searchindex.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_TOP_STATE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

/*
 * For fixed formatted code position 6 is of importance (continuation character).
 * The following variables and macros keep track of the column number
 * YY_USER_ACTION is always called for each scan action
 * YY_FTN_RESET   is used to handle end of lines and reset the column counter
 * YY_FTN_REJECT  resets the column counters when a pattern is rejected and thus rescanned.
 */
int yy_old_start = 0;
int yy_my_start  = 0;
int yy_end       = 1;
#define YY_USER_ACTION {yy_old_start = yy_my_start; yy_my_start = yy_end; yy_end += static_cast<int>(yyleng);}
#define YY_FTN_RESET   {yy_old_start = 0; yy_my_start = 0; yy_end = 1;}
#define YY_FTN_REJECT  {yy_end = yy_my_start; yy_my_start = yy_old_start; REJECT;}

//--------------------------------------------------------------------------------

/**
  data of an use-statement
*/
class UseEntry
{
 public:
   QCString module; // just for debug
   std::vector<QCString> onlyNames;   /* entries of the ONLY-part */
};

/**
  module name -> list of ONLY/remote entries
  (module name = name of the module, which can be accessed via use-directive)
*/
class UseMap : public std::map<std::string,UseEntry>
{
};

/**
  Contains names of used modules and names of local variables.
*/
class Scope
{
  public:
    std::vector<QCString> useNames; //!< contains names of used modules
    StringUnorderedSet localVars; //!< contains names of local variables
    StringUnorderedSet externalVars; //!< contains names of external entities
};

/*===================================================================*/
/*
 *      statics
 */

struct fortrancodeYY_state
{
  QCString      docBlock;                   //!< contents of all lines of a documentation block
  QCString      currentModule=QCString();   //!< name of the current enclosing module
  UseMap      useMembers;                   //!< info about used modules
  UseEntry      useEntry;                   //!< current use statement info
  std::vector<Scope>  scopeStack;
  bool          isExternal = false;
  QCString      str=QCString();             //!< contents of fortran string

  OutputCodeList * code = nullptr;

  const char *  inputString = nullptr;     //!< the code fragment as text
  int           inputPosition = 0;   //!< read offset during parsing
  int           inputLines = 0;      //!< number of line in the code fragment
  QCString      fileName;
  int           yyLineNr = 0;        //!< current line number
  int           contLineNr = 0;      //!< current, local, line number for continuation determination
  int          *hasContLine = nullptr;     //!< signals whether or not a line has a continuation line (fixed source form)
  bool          insideCodeLine = false;
  const Definition *searchCtx = nullptr;
  bool          collectXRefs = false;
  bool          isFixedForm = false;

  bool          insideBody = false;      //!< inside subprog/program body? => create links
  const char *  currentFontClass = nullptr;
  bool          insideSpecialComment = false;

  bool          exampleBlock = false;
  QCString      exampleName;
  QCString      exampleFile;

  std::unique_ptr<FileDef> exampleFileDef;
  const FileDef *    sourceFileDef = nullptr;
  const Definition * currentDefinition = nullptr;
  const MemberDef *  currentMemberDef = nullptr;
  bool          includeCodeFragment = false;

  char          stringStartSymbol = '\0'; // single or double quote
// count in variable declaration to filter out
//  declared from referenced names
  int           bracketCount = 0;

// signal when in type / class /procedure declaration
  int           inTypeDecl = 0;

  bool          endComment = false;
  TooltipManager tooltipManager;
  std::vector<const Definition *> foldStack;

   int fixedCommentAfter = 72;
};

[[maybe_unused]] static const char *stateToString(int state);

static bool getFortranNamespaceDefs(const QCString &mname,
                               NamespaceDef *&cd);
static bool getFortranTypeDefs(const QCString &tname, const QCString &moduleName,
                               ClassDef *&cd, const UseMap &useMap);

//----------------------------------------------------------------------------

static void startFontClass(yyscan_t yyscanner,const char *s,bool specialComment=false);
static void endFontClass(yyscan_t yyscanner,bool specialComment=false);
static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void addToSearchIndex(yyscan_t yyscanner,const QCString &text);
static void startCodeLine(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static void writeMultiLineCodeLink(yyscan_t yyscanner,OutputCodeList &ol,
                  Definition *d,const QCString &text);
static bool getGenericProcedureLink(yyscan_t yyscanner,const ClassDef *cd,
                                    const QCString &memberText,
                                    OutputCodeList &ol);
static bool getLink(yyscan_t yyscanner,const UseMap &useMap, // map with used modules
                    const QCString &memberText,  // exact member text
                    OutputCodeList &ol,
                    const QCString &text);
static void generateLink(yyscan_t yyscanner,OutputCodeList &ol, const QCString &lname);
static int countLines(yyscan_t yyscanner);
static void startScope(yyscan_t yyscanner);
static void endScope(yyscan_t yyscanner);
static void addUse(yyscan_t yyscanner,const QCString &moduleName);
static void addLocalVar(yyscan_t yyscanner,const QCString &varName);
static MemberDef *getFortranDefs(yyscan_t yyscanner,const QCString &memberName, const QCString &moduleName,
                                 const UseMap &useMap);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static inline void pop_state(yyscan_t yyscanner);

/* -----------------------------------------------------------------*/
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

IDSYM     [a-z_A-Z0-9]
ID        [a-z_A-Z]+{IDSYM}*
SUBPROG   (subroutine|function)
BS        [ \t]*
BS_       [ \t]+
COMMA     {BS},{BS}
ARGS_L0   ("("[^)]*")")
ARGS_L1a  [^()]*"("[^)]*")"[^)]*
ARGS_L1   ("("{ARGS_L1a}*")")
ARGS_L2   "("({ARGS_L0}|[^()]|{ARGS_L1a}|{ARGS_L1})*")"
ARGS      {BS}({ARGS_L0}|{ARGS_L1}|{ARGS_L2})

NUM_TYPE  (complex|integer|logical|real)
LOG_OPER  (\.and\.|\.eq\.|\.eqv\.|\.ge\.|\.gt\.|\.le\.|\.lt\.|\.ne\.|\.neqv\.|\.or\.|\.not\.)
KIND      {ARGS}
CHAR      (CHARACTER{ARGS}?|CHARACTER{BS}"*"({BS}[0-9]+|{ARGS}))
TYPE_SPEC (({NUM_TYPE}({BS}"*"{BS}[0-9]+)?)|({NUM_TYPE}{KIND})|DOUBLE{BS}COMPLEX|DOUBLE{BS}PRECISION|{CHAR}|TYPE|CLASS|PROCEDURE|ENUMERATOR)

INTENT_SPEC intent{BS}"("{BS}(in|out|in{BS}out){BS}")"
ATTR_SPEC (IMPLICIT|ALLOCATABLE|DIMENSION{ARGS}|EXTERNAL|{INTENT_SPEC}|INTRINSIC|OPTIONAL|PARAMETER|POINTER|PROTECTED|PRIVATE|PUBLIC|SAVE|TARGET|(NON_)?RECURSIVE|PURE|IMPURE|ELEMENTAL|VALUE|NOPASS|DEFERRED|CONTIGUOUS|VOLATILE)
ACCESS_SPEC (PROTECTED|PRIVATE|PUBLIC)
/* Assume that attribute statements are almost the same as attributes. */
FLOW      (DO|SELECT|CASE|SELECT{BS}(CASE|TYPE|RANK)|WHERE|IF|THEN|ELSE|WHILE|FORALL|ELSEWHERE|ELSEIF|RETURN|CONTINUE|EXIT|GO{BS}TO)
COMMANDS  (FORMAT|CONTAINS|MODULE{BS_}PROCEDURE|WRITE|READ|ALLOCATE|ALLOCATED|ASSOCIATED|PRESENT|DEALLOCATE|NULLIFY|SIZE|INQUIRE|OPEN|CLOSE|FLUSH|DATA|COMMON)
IGNORE    (CALL)
PREFIX    ((NON_)?RECURSIVE{BS_}|IMPURE{BS_}|PURE{BS_}|ELEMENTAL{BS_}){0,4}((NON_)?RECURSIVE|IMPURE|PURE|ELEMENTAL)?0
LANGUAGE_BIND_SPEC BIND{BS}"("{BS}C{BS}(,{BS}NAME{BS}"="{BS}"\""(.*)"\""{BS})?")"

/* |  */

%option noyywrap
%option stack
%option caseless
/*%option debug*/

%x Start
%x SubCall
%x ClassName
%x Subprog
%x DocBlock
%x Use
%x UseOnly
%x Import
%x Declaration
%x DeclarationBinding
%x DeclContLine
%x String
%x Subprogend

%%
 /*==================================================================*/

 /*-------- ignore ------------------------------------------------------------*/

<Start>{IGNORE}/{BS}"("                   { // do not search keywords, intrinsics... TODO: complete list
                                            codifyLines(yyscanner,yytext);
                                          }
 /*-------- inner construct ---------------------------------------------------*/

<Start>{COMMANDS}/{BS}[,( \t\n]           {  // highlight
                                            /* font class is defined e.g. in doxygen.css */
                                            startFontClass(yyscanner,"keyword");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
<Start>{FLOW}/{BS}[,( \t\n]               {
                                          if (yyextra->isFixedForm)
                                          {
                                            if ((yy_my_start == 1) && ((yytext[0] == 'c') || (yytext[0] == 'C'))) YY_FTN_REJECT;
                                          }
                                          if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                          {
                                            std::lock_guard<std::mutex> lock(Doxygen::countFlowKeywordsMutex);
                                            MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef*>(yyextra->currentMemberDef));
                                            if (mdm)
                                            {
                                              mdm->incrementFlowKeyWordCount();
                                            }
                                          }
                                          /* font class is defined e.g. in doxygen.css */
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>{BS}(RANK){BS_}(DEFAULT) |
<Start>{BS}(RANK)/{BS}"("{BS}([0-9]+|"*"){BS}")" |
<Start>{BS}(CASE|CLASS|TYPE){BS_}(IS|DEFAULT) {
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>{BS}"end"({BS}{FLOW})/[ \t\n]       { // list is a bit long as not all have possible end
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>"implicit"{BS}("none"|{TYPE_SPEC})  {
                                          startFontClass(yyscanner,"keywordtype");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>^{BS}"namelist"/[/]              {  // Namelist specification
                                          startFontClass(yyscanner,"keywordtype");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
 /*-------- use statement -------------------------------------------*/
<Start>"use"{BS_}                       {
                                          startFontClass(yyscanner,"keywordtype");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Use);
                                        }
<Use>"ONLY"                             { // TODO: rename
                                          startFontClass(yyscanner,"keywordtype");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(UseOnly);
                                        }
<Use>{ID}                               {
                                          QCString tmp(yytext);
                                          tmp = tmp.lower();
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;

                                          /* append module name to use dict */
                                          yyextra->useEntry = UseEntry();
                                          yyextra->useEntry.module = tmp;
                                          yyextra->useMembers.emplace(tmp.str(), yyextra->useEntry);
                                          addUse(yyscanner,tmp);
                                        }
<Use,UseOnly,Import>{BS},{BS}           { codifyLines(yyscanner,yytext); }
<UseOnly,Import>{BS}&{BS}"\n"           { codifyLines(yyscanner,yytext);
                                          yyextra->contLineNr++;
                                          YY_FTN_RESET}
<UseOnly>{ID}                           {
                                          QCString tmp(yytext);
                                          tmp = tmp.lower();
                                          yyextra->useEntry.onlyNames.push_back(tmp);
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;
                                        }
<Use,UseOnly,Import>"\n"                {
                                          unput(*yytext);
                                          pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<*>"import"{BS}/"\n"                    |
<*>"import"{BS_}                        {
                                          if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          startFontClass(yyscanner,"keywordtype");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Import);
                                        }
<Import>{ID}                            {
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;
                                        }
<Import>("ONLY"|"NONE"|"ALL")           {
                                          startFontClass(yyscanner,"keywordtype");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
 /*-------- fortran module  -----------------------------------------*/
<Start>("block"{BS}"data"|"program"|"module"|"interface")/{BS_}|({COMMA}{ACCESS_SPEC})|\n {  //
                                          startScope(yyscanner);
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(ClassName);
                                          if (!qstricmp(yytext,"module")) yyextra->currentModule="module";
                                        }
<Start>("enum")/{BS_}|{BS}{COMMA}{BS}{LANGUAGE_BIND_SPEC}|\n {  //
                                          startScope(yyscanner);
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(ClassName);
                                        }
<*>{LANGUAGE_BIND_SPEC}                 {  //
                                          if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>("type")/{BS_}|({COMMA}({ACCESS_SPEC}|ABSTRACT|EXTENDS))|\n {  //
                                          startScope(yyscanner);
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(ClassName);
                                        }
<ClassName>{ID}                         {
                                          if (yyextra->currentModule == "module")
                                          {
                                            yyextra->currentModule=yytext;
                                            yyextra->currentModule = yyextra->currentModule.lower();
                                          }
                                          generateLink(yyscanner,*yyextra->code,yytext);
                                          pop_state(yyscanner);
                                        }
<ClassName>({ACCESS_SPEC}|ABSTRACT|EXTENDS)/[,:( ] { //| variable declaration
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<ClassName>\n                           { // interface may be without name
                                          pop_state(yyscanner);
                                          YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"enum").*        {
                                          YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"type").*        {
                                          YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"module").*      { // just reset yyextra->currentModule, rest is done in following rule
                                          yyextra->currentModule=nullptr;
                                          YY_FTN_REJECT;
                                        }
 /*-------- subprog definition -------------------------------------*/
<Start>({PREFIX}{BS_})?{TYPE_SPEC}{BS_}({PREFIX}{BS_})?{BS}/{SUBPROG}{BS_}  {   // TYPE_SPEC is for old function style function result
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>({PREFIX}{BS_})?{SUBPROG}{BS_}   {  // Fortran subroutine or function found
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Subprog);
                                        }
<Subprog>{ID}                           { // subroutine/function name
                                          DBG_CTX((stderr, "===> start subprogram %s\n", yytext));
                                          startScope(yyscanner);
                                          generateLink(yyscanner,*yyextra->code,yytext);
                                        }
<Subprog>"result"/{BS}"("[^)]*")"       {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Subprog>"("[^)]*")"                    { // ignore rest of line
                                          codifyLines(yyscanner,yytext);
                                        }
<Subprog,Subprogend>"\n"                { codifyLines(yyscanner,yytext);
                                          yyextra->contLineNr++;
                                          pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<Start>"end"{BS}("block"{BS}"data"|{SUBPROG}|"module"|"program"|"enum"|"type"|"interface")?{BS}     {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endScope(yyscanner);
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Subprogend);
                                        }
<Subprogend>{ID}/{BS}(\n|!|;)           {
                                          generateLink(yyscanner,*yyextra->code,yytext);
                                          pop_state(yyscanner);
                                        }
<Start>"end"{BS}("block"{BS}"data"|{SUBPROG}|"module"|"program"|"enum"|"type"|"interface"){BS}/(\n|!|;) {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endScope(yyscanner);
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
 /*-------- variable declaration ----------------------------------*/
<Start>^{BS}"real"/[,:( ]               { // real is a bit tricky as it is a data type but also a function.
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Declaration);
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>{TYPE_SPEC}/[,:( ]               {
                                          QCString typ(yytext);
                                          typ = removeRedundantWhiteSpace(typ.lower());
                                          if (typ.startsWith("real")) YY_FTN_REJECT;
                                          if (typ == "type" || typ == "class" || typ == "procedure") yyextra->inTypeDecl = 1;
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Declaration);
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<Start>{ATTR_SPEC}                      {
                                          if (QCString(yytext) == "external")
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(Declaration);
                                            yyextra->isExternal = true;
                                          }
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<Declaration>({TYPE_SPEC}|{ATTR_SPEC})/[,:( ] { //| variable declaration
                                          if (QCString(yytext) == "external") yyextra->isExternal = true;
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<Declaration>{ID}                       { // local var
                                          if (yyextra->isFixedForm && yy_my_start == 1)
                                          {
                                            startFontClass(yyscanner,"comment");
                                            yyextra->code->codify(yytext);
                                            endFontClass(yyscanner);
                                          }
                                          else if (yyextra->currentMemberDef &&
                                                   ((yyextra->currentMemberDef->isFunction() && (yyextra->currentMemberDef->typeString()!=QCString("subroutine") || yyextra->inTypeDecl)) ||
                                                     yyextra->currentMemberDef->isVariable() || yyextra->currentMemberDef->isEnumValue()
                                                    )
                                                   )
                                          {
                                            generateLink(yyscanner,*yyextra->code, yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(yytext);
                                            addLocalVar(yyscanner,yytext);
                                          }
                                        }
<Declaration>{BS}("=>"|"="){BS}         { // Procedure binding
                                          BEGIN(DeclarationBinding);
                                          yyextra->code->codify(yytext);
                                        }
<DeclarationBinding>{ID}                { // Type bound procedure link
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          pop_state(yyscanner);
                                        }
<Declaration>[(]                        { // start of array or type / class specification
                                          yyextra->bracketCount++;
                                          yyextra->code->codify(yytext);
                                        }

<Declaration>[)]                        { // end array specification
                                          yyextra->bracketCount--;
                                          if (!yyextra->bracketCount) yyextra->inTypeDecl = 0;
                                          yyextra->code->codify(yytext);
                                        }

<Declaration,DeclarationBinding>"&"     { // continuation line
                                          yyextra->code->codify(yytext);
                                          if (!yyextra->isFixedForm)
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(DeclContLine);
                                          }
                                        }
<DeclContLine>"\n"                      { // declaration not yet finished
                                          yyextra->contLineNr++;
                                          codifyLines(yyscanner,yytext);
                                          yyextra->bracketCount = 0;
                                          pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<Declaration,DeclarationBinding>"\n"    { // end declaration line (?)
                                          if (yyextra->endComment)
                                          {
                                            yyextra->endComment=FALSE;
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext);
                                          }
                                          yyextra->bracketCount = 0;
                                          yyextra->contLineNr++;
                                          if (!(yyextra->hasContLine && yyextra->hasContLine[yyextra->contLineNr - 1]))
                                          {
                                            yyextra->isExternal = false;
                                            pop_state(yyscanner);
                                          }
                                          YY_FTN_RESET
                                        }

 /*-------- subprog calls  -----------------------------------------*/

<Start>"call"{BS_}                      {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(SubCall);
                                        }
<SubCall>{ID}                           { // subroutine call
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;
                                          pop_state(yyscanner);
                                        }
<Start>{ID}{BS}/"("                     { // function call
                                          if (yyextra->isFixedForm && yy_my_start == 6)
                                          {
                                            // fixed form continuation line
                                            YY_FTN_REJECT;
                                          }
                                          else if (QCString(yytext).stripWhiteSpace().lower() == "type")
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(Declaration);
                                            startFontClass(yyscanner,"keywordtype");
                                            yyextra->code->codify(QCString(yytext).stripWhiteSpace());
                                            endFontClass(yyscanner);
                                            yyextra->code->codify(QCString(yytext + 4));
                                          }
                                          else
                                          {
                                            yyextra->insideBody=TRUE;
                                            generateLink(yyscanner,*yyextra->code,yytext);
                                            yyextra->insideBody=FALSE;
                                          }
                                        }

 /*-------- comments ---------------------------------------------------*/
<Start,Declaration,DeclarationBinding>\n?{BS}"!>"|"!<"                 { // start comment line or comment block
                                          if (yytext[0] == '\n')
                                          {
                                            yyextra->contLineNr++;
                                            yy_old_start = 0;
                                            yy_my_start = 1;
                                            yy_end = static_cast<int>(yyleng);
                                          }
                                          // Actually we should see if ! on position 6, can be continuation
                                          // but the chance is very unlikely, so no effort to solve it here
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(DocBlock);
                                          yyextra->docBlock=yytext;
                                        }
<Declaration,DeclarationBinding>{BS}"!<"                   { // start comment line or comment block
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(DocBlock);
                                          yyextra->docBlock=yytext;
                                        }

<DocBlock>.*                            { // contents of current comment line
                                          yyextra->docBlock+=yytext;
                                        }
<DocBlock>"\n"{BS}("!>"|"!<"|"!!")      { // comment block (next line is also comment line)
                                          yyextra->contLineNr++;
                                          yy_old_start = 0;
                                          yy_my_start = 1;
                                          yy_end = static_cast<int>(yyleng);
                                          // Actually we should see if ! on position 6, can be continuation
                                          // but the chance is very unlikely, so no effort to solve it here
                                          yyextra->docBlock+=yytext;
                                        }
<DocBlock>"\n"                          { // comment block ends at the end of this line
                                          // remove special comment (default config)
                                          yyextra->contLineNr++;
                                          startFontClass(yyscanner,"comment");
                                          codifyLines(yyscanner,yyextra->docBlock);
                                          endFontClass(yyscanner);
                                          unput(*yytext);
                                          yyextra->contLineNr--;
                                          pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }

<*>"!"[^><\n].*|"!"$                    { // normal comment
                                          if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          if (yyextra->isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          startFontClass(yyscanner,"comment");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }

<*>^[Cc*].*                             { // normal comment
                                          if(! yyextra->isFixedForm) YY_FTN_REJECT;

                                          startFontClass(yyscanner,"comment");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<*>"assignment"/{BS}"("{BS}"="{BS}")"   {
                                          if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<*>"operator"/{BS}"("[^)]*")"           {
                                          if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }

 /*------ preprocessor  --------------------------------------------*/
<Start>"#".*\n                          {
                                          if (yyextra->isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          yyextra->contLineNr++;
                                          startFontClass(yyscanner,"preprocessor");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          YY_FTN_RESET
                                        }
 /*------ variable references?  -------------------------------------*/

<Start>"%"{BS}{ID}                      { // ignore references to elements
                                          yyextra->code->codify(yytext);
                                        }
<Start>{ID}                             {
                                            yyextra->insideBody=TRUE;
                                            generateLink(yyscanner,*yyextra->code, yytext);
                                            yyextra->insideBody=FALSE;
                                        }
 /*------ strings --------------------------------------------------*/
<String>\n                              { // string with \n inside
                                          yyextra->contLineNr++;
                                          yyextra->str+=yytext;
                                          startFontClass(yyscanner,"stringliteral");
                                          codifyLines(yyscanner,yyextra->str);
                                          endFontClass(yyscanner);
                                          yyextra->str = "";
                                          YY_FTN_RESET
                                        }
<String>\"|\'                           { // string ends with next quote without previous backspace
                                          if(yytext[0]!=yyextra->stringStartSymbol) YY_FTN_REJECT; // single vs double quote
                                          yyextra->str+=yytext;
                                          startFontClass(yyscanner,"stringliteral");
                                          codifyLines(yyscanner,yyextra->str);
                                          endFontClass(yyscanner);
                                          pop_state(yyscanner);
                                        }
<String>[\x80-\xFF]*                    |
<String>.                               {yyextra->str+=yytext;}

<*>\"|\'                                { /* string starts */
                                          /* if(YY_START == StrIgnore) YY_FTN_REJECT; // ignore in simple comments */
                                          if (yyextra->isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          yy_push_state(YY_START,yyscanner);
                                          yyextra->stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String);
                                          yyextra->str=yytext;
                                        }
 /*-----------------------------------------------------------------------------*/

<*>\n                                   {
                                          if (yyextra->endComment)
                                          {
                                            yyextra->endComment=FALSE;
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext);
                                            // comment cannot extend over the end of a line so should always be terminated at the end of the line.
                                            if (yyextra->currentFontClass && !strcmp(yyextra->currentFontClass,"comment")) endFontClass(yyscanner);
                                          }
                                          yyextra->contLineNr++;
                                          YY_FTN_RESET
                                        }
<*>^{BS}"type"{BS}"="                   { yyextra->code->codify(yytext); }

<*>[\x80-\xFF]*                         { // keep utf8 characters together...
                                          if (yyextra->isFixedForm && yy_my_start > yyextra->fixedCommentAfter)
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(yytext);
                                          }
                                        }
<*>.                                    {
                                          if (yyextra->isFixedForm && yy_my_start > yyextra->fixedCommentAfter)
                                          {
                                            //yy_push_state(YY_START,yyscanner);
                                            //BEGIN(DocBlock);
                                            //yyextra->docBlock=yytext;
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(yytext);
                                          }
                                        }
<*>{LOG_OPER}                           { // Fortran logical comparison keywords
                                          yyextra->code->codify(yytext);
                                        }
<*><<EOF>>                              {
                                          if (YY_START == DocBlock)
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yyextra->docBlock);
                                            endFontClass(yyscanner);
                                          }
                                          yyterminate();
                                        }
%%

/*@ ----------------------------------------------------------------------------
 */

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

static void endFontClass(yyscan_t yyscanner,bool specialComment)
{
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
}

static void startFontClass(yyscan_t yyscanner,const char *s,bool specialComment)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (specialComment)
  {
    yyextra->code->startSpecialComment();
    yyextra->insideSpecialComment = true;
  }
  // if font class is already set don't stop and start it.
  if (qstrcmp(yyextra->currentFontClass,s)!=0)
  {
    endFontClass(yyscanner);
    yyextra->code->startFontClass(s);
    yyextra->currentFontClass=s;
  }
}

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex.enabled())
  {
    if (yyextra->searchCtx)
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),FALSE);
    }
    else
    {
      Doxygen::searchIndex.setCurrentDoc(yyextra->sourceFileDef,anchor,TRUE);
    }
  }
}

static void addToSearchIndex(yyscan_t /*yyscanner*/,const QCString &text)
{
  if (Doxygen::searchIndex.enabled())
  {
    Doxygen::searchIndex.addWord(text,FALSE);
  }
}

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
    if (d)
    {
      int startLine = d->getStartDefLine();
      int endLine   = d->getEndBodyLine();
      if (endLine!=-1 && startLine!=endLine &&
          // since the end of a section is closed after the last line, we need to avoid starting a
          // new section if the previous section ends at the same line, i.e. something like
          // struct X {
          // ...
          // }; struct S {  <- start of S and end of X at the same line
          // ...
          // };
          (yyextra->foldStack.empty() || yyextra->foldStack.back()->getEndBodyLine()!=startLine))
      {
        //printf("%d: start codeFolding for %s [%d..%d]\n",yyextra->yyLineNr,qPrint(d->name()),d->getStartDefLine(),d->getEndBodyLine());
        yyextra->code->startFold(yyextra->yyLineNr,"","");
        yyextra->foldStack.push_back(d);
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
  if (yyextra->sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",yyextra->yyLineNr);
    //lineAnchor.sprintf("l%05d",yyextra->yyLineNr);

    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    //printf("startCodeLine %d d=%s\n", yyextra->yyLineNr,d ? qPrint(d->name()) : "<null>");
    if (!yyextra->includeCodeFragment && d)
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->insideBody = FALSE;
      yyextra->endComment = FALSE;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        codeFolding(yyscanner,yyextra->currentMemberDef);
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),yyextra->yyLineNr,
                                !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        codeFolding(yyscanner,d);
        yyextra->code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                QCString(),yyextra->yyLineNr,
                                !yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else
      {
        codeFolding(yyscanner,nullptr);
      }
    }
    else
    {
      codeFolding(yyscanner,nullptr);
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr,
                                     !yyextra->includeCodeFragment);
    }
  }
  yyextra->code->startCodeLine(yyextra->yyLineNr);
  yyextra->insideCodeLine=true;
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
}


static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
  yyextra->insideCodeLine=false;
}

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char * fc = yyextra->currentFontClass;
  endCodeLine(yyscanner);
  if (yyextra->yyLineNr<yyextra->inputLines)
  {
    yyextra->currentFontClass = fc;
    startCodeLine(yyscanner);
  }
}

/*! write a code fragment 'text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  if (text.isEmpty()) return;
  const char *p=text.data(),*sp=p;
  char c = 0;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      size_t l = static_cast<size_t>(p-sp-1);
      yyextra->code->codify(QCString(sp,l));
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(sp);
      done=TRUE;
    }
  }
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(yyscan_t yyscanner,OutputCodeList &ol,
                  Definition *d,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  yyextra->tooltipManager.addTooltip(d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip;
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
    tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=FALSE;
  const char *p=text.data();
  while (!done)
  {
    const char *sp=p;
    char c = 0;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,QCString(sp,p-sp-1),tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}
//-------------------------------------------------------------------------------
/**
  searches for definition of a module (Namespace)
  @param mname the name of the module
  @param cd the entry, if found or null
  @returns true, if module is found
*/
static bool getFortranNamespaceDefs(const QCString &mname,
                               NamespaceDef *&cd)
{
  if (mname.isEmpty()) return FALSE; /* empty name => nothing to link */

  // search for module
  if ((cd=Doxygen::namespaceLinkedMap->find(mname))) return TRUE;

  return FALSE;
}
//-------------------------------------------------------------------------------
/**
  searches for definition of a type
  @param tname the name of the type
  @param moduleName name of enclosing module or null, if global entry
  @param cd the entry, if found or null
  @param useMap map of data of USE-statement
  @returns true, if type is found
*/
static bool getFortranTypeDefs(const QCString &tname, const QCString &moduleName,
                               ClassDef *&cd, const UseMap &useMap)
{
  if (tname.isEmpty()) return FALSE; /* empty name => nothing to link */

  //cout << "=== search for type: " << tname << endl;

  // search for type
  if ((cd=Doxygen::classLinkedMap->find(tname)))
  {
    //cout << "=== type found in global module" << endl;
    return TRUE;
  }
  else if (!moduleName.isEmpty() && (cd=Doxygen::classLinkedMap->find(moduleName+"::"+tname)))
  {
    //cout << "=== type found in local module" << endl;
    return TRUE;
  }
  else
  {
    for (const auto &[name,useEntry] : useMap)
    {
      if ((cd=Doxygen::classLinkedMap->find(useEntry.module+"::"+tname)))
      {
        //cout << "===  type found in used module" << endl;
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  searches for definition of function memberName
  @param yyscanner the scanner data to be used
  @param memberName the name of the function/variable
  @param moduleName name of enclosing module or null, if global entry
  @param useMap map of data of USE-statement
  @returns MemberDef pointer, if found, or nullptr otherwise
*/
static MemberDef *getFortranDefs(yyscan_t yyscanner,const QCString &memberName, const QCString &moduleName,
                                 const UseMap &useMap)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  MemberDef *potentialMd = nullptr;
  if (memberName.isEmpty()) return nullptr; /* empty name => nothing to link */

  // look in local variables
  for (auto it = yyextra->scopeStack.rbegin(); it!=yyextra->scopeStack.rend(); ++it)
  {
    const Scope &scope = *it;
    std::string lowMemName = memberName.lower().str();
    if (scope.localVars   .find(lowMemName)!=std::end(scope.localVars)     &&  // local var
        scope.externalVars.find(lowMemName)==std::end(scope.externalVars))     // and not external
    {
      return nullptr;
    }
  }

  // search for function
  MemberName *mn = Doxygen::functionNameLinkedMap->find(memberName);
  if (!mn)
  {
    mn = Doxygen::memberNameLinkedMap->find(memberName);
  }

  if (mn) // name is known
  {
    // all found functions with given name
    for (const auto &md : *mn)
    {
      const FileDef  *fd=md->getFileDef();
      const GroupDef *gd=md->getGroupDef();
      const ClassDef *cd=md->getClassDef();

      //cout << "found link with same name: " << fd->fileName() << "  " <<  memberName;
      //if (md->getNamespaceDef() != 0) cout << " in namespace " << md->getNamespaceDef()->name();cout << endl;

      if ((gd && gd->isLinkable()) || (fd && fd->isLinkable()))
      {
        const NamespaceDef *nspace= md->getNamespaceDef();

        if (nspace == nullptr)
        { // found function in global scope
          if (cd == nullptr)
          { // Skip if bound to type
            if (md.get()->getFileDef() == yyextra->sourceFileDef) return md.get();
            else if (!potentialMd) potentialMd = md.get();
          }
        }
        else if (moduleName == nspace->name())
        { // found in local scope
          return md.get();
        }
        else
        { // else search in used modules
          QCString usedModuleName= nspace->name();
          auto use_it = useMap.find(usedModuleName.str());
          if (use_it!=useMap.end())
          {
            const UseEntry &ue = use_it->second;
            // check if only-list exists and if current entry exists is this list
            if (ue.onlyNames.empty())
            {
              //cout << " found in module " << usedModuleName << " entry " << memberName <<  endl;
              return md.get(); // whole module used
            }
            else
            {
              for ( const auto &name : ue.onlyNames)
              {
                //cout << " search in only: " << usedModuleName << ":: " << memberName << "==" << (*it)<<  endl;
                if (memberName == name)
                {
                  return md.get(); // found in ONLY-part of use list
                }
              }
            }
          }
        }
      } // if linkable
    } // for
  }
  return potentialMd;
}

/**
 gets the link to a generic procedure which depends not on the name, but on the parameter list
 @todo implementation
*/
static bool getGenericProcedureLink(yyscan_t ,const ClassDef *,
                                    const QCString &,
                                    OutputCodeList &)
{
  return FALSE;
}

static bool getLink(yyscan_t yyscanner,const UseMap &useMap, // dictionary with used modules
                    const QCString &memberText,  // exact member text
                    OutputCodeList &ol,
                    const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  MemberDef *md=nullptr;
  QCString memberName= removeRedundantWhiteSpace(memberText);

  if ((md=getFortranDefs(yyscanner,memberName, yyextra->currentModule, useMap)) && md->isLinkable())
  {
    if (md->isVariable() && (md->getLanguage()!=SrcLangExt::Fortran)) return FALSE; // Non Fortran variables aren't handled yet,
                                                                                   // see also linkifyText in util.cpp

    const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                          md->getBodyDef() : md->getOuterScope();
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      if (yyextra->currentDefinition && yyextra->currentMemberDef &&
          yyextra->insideBody && yyextra->collectXRefs)
      {
        addDocCrossReference(yyextra->currentMemberDef,md);
      }
      writeMultiLineCodeLink(yyscanner,ol,md,!text.isEmpty() ? text : memberText);
      addToSearchIndex(yyscanner, !text.isEmpty() ? text : memberText);
      return TRUE;
    }
  }
  return FALSE;
}


static void generateLink(yyscan_t yyscanner,OutputCodeList &ol, const QCString &lname)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  ClassDef *cd=nullptr;
  NamespaceDef *nsd=nullptr;
  QCString name = lname;
  name = removeRedundantWhiteSpace(name.lower());

  // check if lowercase lname is a linkable type or interface
  if ( (getFortranTypeDefs(name, yyextra->currentModule, cd, yyextra->useMembers)) && cd->isLinkable() )
  {
    if ( (cd->compoundType() == ClassDef::Class) && // was  Entry::INTERFACE_SEC) &&
         (getGenericProcedureLink(yyscanner, cd, name, ol)) )
    {
      //cout << "=== generic procedure resolved" << endl;
    }
    else
    { // write type or interface link
      writeMultiLineCodeLink(yyscanner, ol,cd,name);
      addToSearchIndex(yyscanner, name);
    }
  }
  // check for module
  else if ( (getFortranNamespaceDefs(name, nsd)) && nsd->isLinkable() )
  { // write module link
    writeMultiLineCodeLink(yyscanner,ol,nsd,name);
    addToSearchIndex(yyscanner,name);
  }
  // check for function/variable
  else if (getLink(yyscanner,yyextra->useMembers, name, ol, name))
  {
    //cout << "=== found link for lowercase " << lname << endl;
  }
  else
  {
    // nothing found, just write out the word
    //startFontClass("charliteral"); //test
    codifyLines(yyscanner,name);
    //endFontClass(yyscanner); //test
    addToSearchIndex(yyscanner,name);
  }
}

/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c = 0;
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

//----------------------------------------------------------------------------
/** start scope */
static void startScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr, "===> startScope %s",yytext));
  yyextra->scopeStack.emplace_back();
}

/** end scope */
static void endScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"===> endScope %s",yytext));
  if (yyextra->scopeStack.empty())
  {
    DBG_CTX((stderr,"WARNING: fortrancode.l: stack empty!\n"));
    return;
  }

  Scope &scope = yyextra->scopeStack.back();
  for ( const auto &name : scope.useNames)
  {
    yyextra->useMembers.erase(name.str());
  }
  yyextra->scopeStack.pop_back();
}

static void addUse(yyscan_t yyscanner,const QCString &moduleName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->scopeStack.empty())
    yyextra->scopeStack.back().useNames.push_back(moduleName);
}

static void addLocalVar(yyscan_t yyscanner,const QCString &varName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->scopeStack.empty())
  {
    std::string lowVarName = varName.lower().str();
    yyextra->scopeStack.back().localVars.insert(lowVarName);
    if (yyextra->isExternal) yyextra->scopeStack.back().externalVars.insert(lowVarName);
  }
}

/*===================================================================*/


static void checkContLines(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int numLines = 0;
  int i = 0;
  const char *p = s;

  numLines = 2; // one for element 0, one in case no \n at end
  while (*p)
  {
    if (*p == '\n') numLines++;
    p++;
  }

  yyextra->hasContLine = (int *) malloc((numLines) * sizeof(int));
  for (i = 0; i < numLines; i++)
  {
    yyextra->hasContLine[i] = 0;
  }
  p = prepassFixedForm(s, yyextra->hasContLine,yyextra->fixedCommentAfter);
  yyextra->hasContLine[0] = 0;
}

void parseFortranCode(OutputCodeList &,const char *,const QCString &,
                  bool , const char *,const FileDef *,
                  int ,int ,bool ,
                  const MemberDef *,bool,const Definition *,
                  bool , FortranFormat )
{
  //printf("***parseCode() exBlock=%d exName=%s fd=%p\n",exBlock,exName,fd);

  return;
}

//---------------------------------------------------------

struct FortranCodeParser::Private
{
  yyscan_t yyscanner;
  fortrancodeYY_state state;
  FortranFormat format;
};

FortranCodeParser::FortranCodeParser(FortranFormat format) : p(std::make_unique<Private>())
{
  p->format = format;
  fortrancodeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  fortrancodeYYset_debug(Debug::isFlagSet(Debug::Lex_fortrancode)?1:0,p->yyscanner);
#endif
  resetCodeParserState();
}

FortranCodeParser::~FortranCodeParser()
{
  fortrancodeYYlex_destroy(p->yyscanner);
}

void FortranCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->currentDefinition = nullptr;
  yyextra->currentMemberDef = nullptr;
  yyextra->currentFontClass = nullptr;
  yyextra->insideCodeLine = FALSE;
  BEGIN( Start );
}

void FortranCodeParser::parseCode(OutputCodeList & codeOutIntf,
                                  const QCString & /* scopeName */,
                                  const QCString & input,
                                  SrcLangExt /*lang*/,
                                  bool stripCodeComments,
                                  const CodeParserOptions &options
                                 )
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  if (input.isEmpty()) return;
  DebugLex debugLex(Debug::Lex_fortrancode, __FILE__, options.fileDef() ? qPrint(options.fileDef()->fileName()): nullptr);
  codeOutIntf.stripCodeComments(stripCodeComments);
  yyextra->code               = &codeOutIntf;
  yyextra->inputString        = input.data();
  yyextra->inputPosition      = 0;
  yyextra->fileName           = options.fileDef() ? options.fileDef()->fileName():"";
  yyextra->isFixedForm        = recognizeFixedForm(input,p->format);
  yyextra->contLineNr         = 1;
  yyextra->hasContLine        = nullptr;
  if (yyextra->isFixedForm)
  {
    checkContLines(yyscanner,yyextra->inputString);
    yyextra->fixedCommentAfter = Config_getInt(FORTRAN_COMMENT_AFTER);
  }
  yyextra->currentFontClass   = nullptr;
  yyextra->insideCodeLine     = FALSE;
  yyextra->searchCtx          = options.searchCtx();
  yyextra->collectXRefs       = options.collectXRefs();
  yyextra->yyLineNr           = options.startLine()!=-1 ? options.startLine() : 1;
  yyextra->inputLines         = options.endLine()!=-1   ? options.endLine()+1 : yyextra->yyLineNr + countLines(yyscanner) - 1;
  yyextra->exampleBlock       = options.isExample();
  yyextra->exampleName        = options.exampleName();
  yyextra->sourceFileDef      = options.fileDef();
  yyextra->foldStack.clear();
  yyextra->insideSpecialComment = false;
  if (options.isExample() && options.fileDef()==nullptr)
  {
    // create a dummy filedef for the example
    yyextra->exampleFileDef = createFileDef(QCString(),options.exampleName());
    yyextra->sourceFileDef = yyextra->exampleFileDef.get();
  }
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,QCString("l00001"));
  }
  yyextra->currentDefinition = nullptr;
  yyextra->currentMemberDef = nullptr;
  if (!yyextra->exampleName.isEmpty())
  {
    yyextra->exampleFile = convertNameToFile(yyextra->exampleName+"-example");
  }
  yyextra->includeCodeFragment = options.inlineFragment();
  startCodeLine(yyscanner);
  fortrancodeYYrestart(nullptr, yyscanner);
  BEGIN( Start );
  fortrancodeYYlex(yyscanner);
  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    while (!yyextra->foldStack.empty())
    {
      yyextra->code->endFold();
      yyextra->foldStack.pop_back();
    }
  }
  if (yyextra->exampleFileDef)
  {
    // delete the temporary file definition used for this example
    yyextra->exampleFileDef.reset();
    yyextra->sourceFileDef=nullptr;
  }
  if (yyextra->hasContLine) free(yyextra->hasContLine);
  yyextra->hasContLine = nullptr;

  // write the tooltips
  yyextra->tooltipManager.writeTooltips(codeOutIntf);
}

static inline void pop_state(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if ( yyg->yy_start_stack_ptr <= 0 )
    warn(yyextra->fileName,yyextra->yyLineNr,"Unexpected statement '{}'",yytext );
  else
    yy_pop_state(yyscanner);
}
//---------------------------------------------------------

#include "fortrancode.l.h"
