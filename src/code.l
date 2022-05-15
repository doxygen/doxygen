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
%option never-interactive
%option prefix="codeYY"
%option reentrant
%option extra-type="struct codeYY_state *"
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

#include <utility>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <vector>
#include <string>
#include <mutex>
#include <sstream>

#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "code.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "arguments.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "filename.h"
#include "namespacedef.h"
#include "tooltip.h"
#include "scopedtypevariant.h"
#include "symbolresolver.h"
#include "dir.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_UNISTD_H 1

#define CLASSBLOCK 1
#define SCOPEBLOCK 2
#define INNERBLOCK 3

#define USE_STATE2STRING 0

// context for an Objective-C method call
struct ObjCCallCtx
{
  int id;
  QCString methodName;
  QCString objectTypeOrName;
  TextStream comment;
  const ClassDef *objectType;
  const MemberDef *objectVar;
  const MemberDef *method;
  QCString format;
  int lexState;
  int braceCount;
};

struct codeYY_state
{
  CodeOutputInterface * code = 0;

  std::unordered_map< std::string, ScopedTypeVariant > codeClassMap;
  QCString      curClassName;
  StringVector  curClassBases;

  QCString      parmType;
  QCString      parmName;

  const char *  inputString = 0;     //!< the code fragment as text
  yy_size_t     inputPosition = 0;   //!< read offset during parsing
  QCString      fileName;
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  yy_size_t     yyColNr = 0;         //!< current column number
  bool          insideCodeLine = FALSE;
  bool          skipCodify = FALSE;  //!< for CSharp files scoped namespace {

  bool          exampleBlock = FALSE;
  QCString      exampleName;
  QCString      exampleFile;

  bool          insideTemplate = FALSE;
  QCString      type;
  QCString      name;
  QCString      args;
  QCString      classScope;
  QCString      realScope;
  std::stack<int> scopeStack;      //!< 1 if bracket starts a scope,
                                   //   2 for internal blocks
  int           anchorCount = 0;
  const FileDef *     sourceFileDef = 0;
  bool          lineNumbers = FALSE;
  const Definition *  currentDefinition = 0;
  const MemberDef *   currentMemberDef = 0;
  bool          includeCodeFragment = FALSE;
  const char *  currentFontClass = 0;
  bool          searchingForBody = FALSE;
  bool          insideBody = FALSE;
  int           bodyCurlyCount = 0;
  QCString      saveName;
  QCString      saveType;
  QCString      delimiter;

  int           bracketCount = 0;
  int           curlyCount   = 0;
  int           sharpCount   = 0;
  bool          inFunctionTryBlock = FALSE;
  bool          inForEachExpression = FALSE;

  int           lastTemplCastContext = 0;
  int           lastSpecialCContext = 0;
  int           lastStringContext = 0;
  int           lastSkipCppContext = 0;
  int           lastVerbStringContext = 0;
  int           lastObjCCallContext = 0;
  int           memCallContext = 0;
  int           lastCContext = 0;
  int           skipInlineInitContext = 0;

  SrcLangExt    lang = SrcLangExt_Unknown;
  bool          insideObjC = FALSE;
  bool          insideProtocolList = FALSE;

  bool          lexInit = FALSE;

  std::stack<int>  classScopeLengthStack;

  int           isPrefixedWithThis = FALSE;
  const Definition *searchCtx = 0;
  bool          collectXRefs = FALSE;

  ObjCCallCtx * currentCtx=0;
  int           currentCtxId=0;
  int           currentNameId=0;
  int           currentObjId=0;
  int           currentWordId=0;
  int           currentCommentId=0;
  std::stack<ObjCCallCtx*> contextStack;
  std::unordered_map< int,std::unique_ptr<ObjCCallCtx> > contextMap;
  std::unordered_map< int, QCString>  nameMap;
  std::unordered_map< int, QCString>  objectMap;
  std::unordered_map< int, QCString>  wordMap;
  std::unordered_map< int, QCString>  commentMap;
  int           braceCount=0;

  using UsingContext = std::map<std::string,const NamespaceDef*>;

  VariableContext theVarContext;
  UsingContext    theUsingContext;
  CallContext     theCallContext;
  SymbolResolver  symbolResolver;
  TooltipManager  tooltipManager;
};

static bool isCastKeyword(const char *s);

//-------------------------------------------------------------------
#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static void saveObjCContext(yyscan_t yyscanner);
static void restoreObjCContext(yyscan_t yyscanner);
static void pushScope(yyscan_t yyscanner,const QCString &s);
static void popScope(yyscan_t yyscanner);
static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void addToSearchIndex(yyscan_t yyscanner,const QCString &text);
static void addToSearchIndex(yyscan_t yyscanner,const char *text);
static void setClassScope(yyscan_t yyscanner,const QCString &name);
static void startCodeLine(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void startFontClass(yyscan_t yyscanner,const char *s);
static void endFontClass(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static void codifyLines(yyscan_t yyscanner,const char *text);
static void incrementFlowKeyWordCount(yyscan_t yyscanner);
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                                   const Definition *d,
                                   const QCString &text);
static void addType(yyscan_t yyscanner);
static void addParmType(yyscan_t yyscanner);
static void addUsingDirective(yyscan_t yyscanner,const QCString &name);
static void setParameterList(yyscan_t yyscanner,const MemberDef *md);
static const ClassDef *stripClassName(yyscan_t yyscanner,const QCString &s,const Definition *d);
static const MemberDef *setCallContextForVar(yyscan_t yyscanner,const QCString &name);
static void updateCallContextForSmartPointer(yyscan_t yyscanner);
static bool getLinkInScope(yyscan_t yyscanner,const QCString &c,  // scope
                           const QCString &m,  // member
                           const QCString &memberText, // exact text
                           CodeOutputInterface &ol,
                           const QCString &text,
                           bool varOnly=FALSE
                          );
static bool getLink(yyscan_t yyscanner,const QCString &className,
                    const QCString &memberName,
                    CodeOutputInterface &ol,
                    const QCString &text=QCString(),
                    bool varOnly=FALSE);
static void generateClassOrGlobalLink(yyscan_t yyscanner,CodeOutputInterface &ol,const QCString &clName,
                                      bool typeOnly=FALSE,bool varOnly=FALSE);
static void generateClassOrGlobalLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *clName,
                                      bool typeOnly=FALSE,bool varOnly=FALSE);
static bool generateClassMemberLink(yyscan_t yyscanner,CodeOutputInterface &ol,const MemberDef *xmd,const QCString &memName);
static bool generateClassMemberLink(yyscan_t yyscanner,CodeOutputInterface &ol,const Definition *def,const QCString &memName);
static void generateMemberLink(yyscan_t yyscanner,CodeOutputInterface &ol,const QCString &varName,
            const QCString &memName);
static void generatePHPVariableLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *varName);
static void generateFunctionLink(yyscan_t yyscanner,CodeOutputInterface &ol,const QCString &funcName);
static void generateFunctionLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *funcName);
static int countLines(yyscan_t yyscanner);
static void writeObjCMethodCall(yyscan_t yyscanner,ObjCCallCtx *ctx);
static QCString escapeName(yyscan_t yyscanner,const char *s);
static QCString escapeObject(yyscan_t yyscanner,const char *s);
static QCString escapeWord(yyscan_t yyscanner,const char *s);
static QCString escapeComment(yyscan_t yyscanner,const char *s);
static bool skipLanguageSpecificKeyword(yyscan_t yyscanner,const char *kw);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);
static void addVariable(yyscan_t yyscanner,QCString type,QCString name);
static bool startsWithKeyword(const QCString &str,const QCString &kw);

//-------------------------------------------------------------------

static std::mutex g_searchIndexMutex;
static std::mutex g_docCrossReferenceMutex;
static std::mutex g_addExampleMutex;
static std::mutex g_countFlowKeywordsMutex;

/* -----------------------------------------------------------------
 */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

B       [ \t]
Bopt    {B}*
BN      [ \t\n\r]
ID      [$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*
SEP     ("::"|"\\")
SCOPENAME ({SEP}{BN}*)?({ID}{BN}*{SEP}{BN}*)*("~"{BN}*)?{ID}
TEMPLIST "<"[^\"\}\{\(\)\/\n\>]*">"
SCOPETNAME (((({ID}{TEMPLIST}?){BN}*)?{SEP}{BN}*)*)((~{BN}*)?{ID})
SCOPEPREFIX ({ID}{TEMPLIST}?{BN}*{SEP}{BN}*)+
KEYWORD_OBJC ("@public"|"@private"|"@protected"|"@class"|"@implementation"|"@interface"|"@end"|"@selector"|"@protocol"|"@optional"|"@required"|"@throw"|"@synthesize"|"@property")
  /* please also pay attention to skipLanguageSpecificKeyword when changing the list of keywords. */
KEYWORD ("asm"|"__assume"|"auto"|"class"|"const"|"delete"|"enum"|"explicit"|"extern"|"false"|"friend"|"gcnew"|"gcroot"|"set"|"get"|"inline"|"internal"|"mutable"|"namespace"|"new"|"null"|"nullptr"|"override"|"operator"|"pin_ptr"|"private"|"protected"|"public"|"raise"|"register"|"remove"|"self"|"sizeof"|"static"|"struct"|"__super"|"function"|"template"|"generic"|"this"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"abstract"|"final"|"import"|"synchronized"|"transient"|"alignas"|"alignof"|"concept"|"requires"|"decltype"|{KEYWORD_OBJC}|"constexpr"|"consteval"|"constinit"|"co_await"|"co_return"|"co_yield"|"static_assert"|"noexcept"|"thread_local"|"enum"{B}+("class"|"struct"))
FLOWKW  ("break"|"catch"|"continue"|"default"|"do"|"else"|"finally"|"return"|"switch"|"throw"|"throws"|"@catch"|"@finally")
FLOWCONDITION  ("case"|"for"|"foreach"|"for each"|"goto"|"if"|"try"|"while"|"@try")
TYPEKW  ("bool"|"byte"|"char"|"char8_t"|"char16_t"|"char32_t"|"double"|"float"|"int"|"long"|"object"|"short"|"signed"|"unsigned"|"void"|"wchar_t"|"size_t"|"boolean"|"id"|"SEL"|"string"|"nullptr")
TYPEKWSL ("LocalObject"|"Object"|"Value")
CASTKW ("const_cast"|"dynamic_cast"|"reinterpret_cast"|"static_cast")
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^' \\\n]{1,4}"'"))
ARITHOP "+"|"-"|"/"|"*"|"%"|"--"|"++"
ASSIGNOP "="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|="
LOGICOP "=="|"!="|">"|"<"|">="|"<="|"&&"|"||"|"!"|"<=>"
BITOP   "&"|"|"|"^"|"<<"|">>"|"~"
OPERATOR {ARITHOP}|{ASSIGNOP}|{LOGICOP}|{BITOP}
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"

  /* no comment start / end signs inside square brackets */
NCOMM [^/\*]
  //- start: NUMBER -------------------------------------------------------------------------
  // Note same defines in commentcnv.l: keep in sync
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
  //- end: NUMBER ---------------------------------------------------------------------------

  // C start comment 
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment 
CPPC  "/\/"

  // ENDIDopt
ENDIDopt ("::"{ID})*
  // Optional end qualifiers
ENDQopt ("const"|"volatile"|"sealed"|"override")({BN}+("const"|"volatile"|"sealed"|"override"))*

%option noyywrap

%x      SkipString
%x      SkipStringS
%x      SkipVerbString
%x      SkipCPP
%x      SkipComment
%x      SkipCxxComment
%x      RemoveSpecialCComment
%x      Body
%x      FuncCall
%x      MemberCall
%x      MemberCall2
%x      SkipInits
%x      ClassName
%x      AlignAs
%x      AlignAsEnd
%x      PackageName
%x      ClassVar
%x      CppCliTypeModifierFollowup
%x      Bases
%x      SkipSharp
%x      ReadInclude
%x      TemplDecl
%x      TemplCast
%x      CallEnd
%x      ObjCMethod
%x      ObjCParams
%x      ObjCParamType
%x      ObjCCall
%x      ObjCMName
%x      ObjCSkipStr
%x      ObjCCallComment
%x      OldStyleArgs
%x      ConceptName
%x      UsingName
%x      RawString
%x      InlineInit

%%

<*>\x0d
<Body>^([ \t]*"#"[ \t]*("include"|"import")[ \t]*)("<"|"\"") {
                                          startFontClass(yyscanner,"preprocessor");
                                          yyextra->code->codify(yytext);
                                          BEGIN( ReadInclude );
                                        }
<Body>("@interface"|"@implementation"|"@protocol")[ \t\n]+ {
                                          yyextra->insideObjC=TRUE;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          if (!yyextra->insideTemplate)
                                            BEGIN( ClassName );
                                        }
<Body>(("public"|"private"){B}+)?("ref"|"value"|"interface"|"enum"){B}+("class"|"struct") {
                                          if (yyextra->insideTemplate) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( ClassName );
                                        }
<Body>"property"|"event"/{BN}*                  {
                                          if (yyextra->insideTemplate) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Body>("partial"{B}+)?("class"|"struct"|"union"|"namespace"|"interface"){B}+ {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          if (!yyextra->insideTemplate)
                                            BEGIN( ClassName );
                                        }
<Body>("package")[ \t\n]+               {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( PackageName );
                                        }
<ClassVar>\n                            {
                                          if (!yyextra->insideObjC) REJECT;
                                          codifyLines(yyscanner,yytext);
                                          BEGIN(Body);
                                        }
<Body,ClassVar,Bases>"-"|"+"            {
                                          if (!yyextra->insideObjC || yyextra->insideBody)
                                          {
                                            yyextra->code->codify(yytext);
                                          }
                                          else // Start of Objective-C method
                                          {
                                            DBG_CTX((stderr,"Start of Objective-C method!\n"));
                                            yyextra->code->codify(yytext);
                                            BEGIN(ObjCMethod);
                                          }
                                        }
<ObjCMethod>":"                         {
                                          yyextra->code->codify(yytext);
                                          BEGIN(ObjCParams);
                                        }
<ObjCParams>"("                         {
                                          yyextra->code->codify(yytext);
                                          BEGIN(ObjCParamType);
                                        }
<ObjCParams,ObjCMethod>";"|"{"          {
                                          yyextra->code->codify(yytext);
                                          if (*yytext=='{')
                                          {
                                            if (yyextra->searchingForBody)
                                            {
                                              yyextra->searchingForBody=FALSE;
                                              yyextra->insideBody=TRUE;
                                            }
                                            if (yyextra->insideBody) yyextra->bodyCurlyCount++;
                                            if (!yyextra->curClassName.isEmpty()) // valid class name
                                            {
                                              pushScope(yyscanner,yyextra->curClassName);
                                              DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
                                              yyextra->scopeStack.push(SCOPEBLOCK);
                                            }
                                          }
                                          yyextra->type.resize(0);
                                          yyextra->name.resize(0);
                                          BEGIN(Body);
                                        }
<ObjCParams>{ID}{B}*":"                 {
                                          yyextra->code->codify(yytext);
                                        }
<ObjCParamType>{TYPEKW}                 {
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          yyextra->parmType=yytext;
                                        }
<ObjCParamType>{ID}                     {
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->parmType=yytext;
                                        }
<ObjCParamType>")"                      {
                                          yyextra->code->codify(yytext);
                                          BEGIN(ObjCParams);
                                        }
<ObjCParams>{ID}                        {
                                          yyextra->code->codify(yytext);
                                          yyextra->parmName=yytext;
                                          addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                        }
<ObjCMethod,ObjCParams,ObjCParamType>{ID} {
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                        }
<ObjCMethod,ObjCParams,ObjCParamType>.  {
                                          yyextra->code->codify(yytext);
                                        }
<ObjCMethod,ObjCParams,ObjCParamType>\n {
                                          codifyLines(yyscanner,yytext);
                                        }
<ReadInclude>[^\n\"\>]+/(">"|"\"")      {
                                          //FileInfo *f;
                                          bool ambig;
                                          bool found=FALSE;

                                          const FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,yytext,ambig);
                                          //printf("looking for include %s -> %s fd=%p\n",yytext,qPrint(absPath),fd);
                                          if (fd && fd->isLinkable())
                                          {
                                            if (ambig) // multiple input files match the name
                                            {
                                              DBG_CTX((stderr,"===== yes %s is ambiguous\n",yytext));
                                              QCString name(Dir::cleanDirPath(yytext));
                                              if (!name.isEmpty() && yyextra->sourceFileDef)
                                              {
                                                const FileName *fn = Doxygen::inputNameLinkedMap->find(name);
                                                if (fn)
                                                {
                                                  // see if this source file actually includes the file
                                                  auto it = std::find_if(fn->begin(),
                                                                         fn->end(),
                                                                         [&sfd=yyextra->sourceFileDef]
                                                                         (const auto &lfd)
                                                                         { return sfd->isIncluded(lfd->absFilePath()); });
                                                  found = it!=fn->end();
                                                }
                                              }
                                            }
                                            else // not ambiguous
                                            {
                                              found = TRUE;
                                            }
                                          }
                                          DBG_CTX((stderr,"      include file %s found=%d\n",fd ? qPrint(fd->absFilePath()) : "<none>",found));
                                          if (found)
                                          {
                                            writeMultiLineCodeLink(yyscanner,*yyextra->code,fd,yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(yytext);
                                          }
                                          char c=(char)yyinput(yyscanner);
                                          QCString text;
                                          text+=c;
                                          yyextra->code->codify(text);
                                          endFontClass(yyscanner);
                                          BEGIN( Body );
                                        }
<Body,Bases>^[ \t]*"#"                  {
                                          startFontClass(yyscanner,"preprocessor");
                                          yyextra->lastSkipCppContext = YY_START;
                                          yyextra->code->codify(yytext);
                                          BEGIN( SkipCPP ) ;
                                        }
<SkipCPP>\"                             {
                                          yyextra->code->codify(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN( SkipString ) ;
                                        }
<SkipCPP>.                              {
                                          yyextra->code->codify(yytext);
                                        }
<SkipCPP>[^\n\/\\\"]+                   {
                                          yyextra->code->codify(yytext);
                                        }
<SkipCPP>\\[\r]?\n                      {
                                          codifyLines(yyscanner,yytext);
                                        }
<SkipCPP>{CPPC}/[^/!]                     {
                                          REJECT;
                                        }
<Body,FuncCall>"{"                      {
                                          yyextra->theVarContext.pushScope();

                                          DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                          yyextra->scopeStack.push(INNERBLOCK);

                                          if (yyextra->searchingForBody)
                                          {
                                            yyextra->searchingForBody=FALSE;
                                            yyextra->insideBody=TRUE;
                                          }
                                          yyextra->code->codify(yytext);
                                          if (yyextra->insideBody)
                                          {
                                            yyextra->bodyCurlyCount++;
                                          }
                                          yyextra->type.resize(0);
                                          yyextra->name.resize(0);
                                          BEGIN( Body );
                                        }
<Body,FuncCall,MemberCall,MemberCall2>"}"  {
                                          yyextra->theVarContext.popScope();
                                          yyextra->type.resize(0);
                                          yyextra->name.resize(0);

                                          if (!yyextra->scopeStack.empty())
                                          {
                                            int scope = yyextra->scopeStack.top();
                                            yyextra->scopeStack.pop();
                                            DBG_CTX((stderr,"** scope stack pop SCOPEBLOCK=%d\n",scope==SCOPEBLOCK));
                                            if (scope==SCOPEBLOCK || scope==CLASSBLOCK)
                                            {
                                              popScope(yyscanner);
                                            }
                                          }

                                          yyextra->code->codify(yytext);

                                          DBG_CTX((stderr,"yyextra->bodyCurlyCount=%d\n",yyextra->bodyCurlyCount));
                                          if (--yyextra->bodyCurlyCount<=0)
                                          {
                                            yyextra->insideBody=FALSE;
                                            yyextra->currentMemberDef=0;
                                            if (yyextra->currentDefinition)
                                              yyextra->currentDefinition=yyextra->currentDefinition->getOuterScope();
                                          }
                                          BEGIN(Body);
                                        }
<Body,ClassVar>"@end"                   {
                                          DBG_CTX((stderr,"End of objc scope fd=%s\n",qPrint(yyextra->sourceFileDef->name())));
                                          if (yyextra->sourceFileDef)
                                          {
                                            const FileDef *fd=yyextra->sourceFileDef;
                                            yyextra->insideObjC = fd->name().lower().right(2)==".m" ||
                                                           fd->name().lower().right(3)==".mm";
                                            DBG_CTX((stderr,"insideObjC=%d\n",yyextra->insideObjC));
                                          }
                                          else
                                          {
                                            yyextra->insideObjC = FALSE;
                                          }
                                          if (yyextra->insideBody)
                                          {
                                            yyextra->theVarContext.popScope();

                                            if (!yyextra->scopeStack.empty())
                                            {
                                              int scope = yyextra->scopeStack.top();
                                              yyextra->scopeStack.pop();
                                              DBG_CTX((stderr,"** scope stack pop SCOPEBLOCK=%d\n",scope==SCOPEBLOCK));
                                              if (scope==SCOPEBLOCK || scope==CLASSBLOCK)
                                              {
                                                popScope(yyscanner);
                                              }
                                            }
                                            yyextra->insideBody=FALSE;
                                          }

                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);

                                          yyextra->currentMemberDef=0;
                                          if (yyextra->currentDefinition)
                                            yyextra->currentDefinition=yyextra->currentDefinition->getOuterScope();
                                          BEGIN(Body);
                                        }
<ClassName,ClassVar>";"                 {
                                          if (yyextra->lang==SrcLangExt_CSharp)
                                          {
                                            yyextra->code->codify(yytext);
                                            yyextra->skipCodify = true;
                                            unput('{');
                                          }
                                          else
                                          {
                                            yyextra->code->codify(yytext);
                                            yyextra->searchingForBody=FALSE;
                                            BEGIN( Body );
                                          }
                                        }
<ClassName,ClassVar>[*&^%]+             {
                                          yyextra->type=yyextra->curClassName;
                                          yyextra->name.resize(0);
                                          yyextra->code->codify(yytext);
                                          BEGIN( Body ); // variable of type struct *
                                        }
<ClassName>"__declspec"{B}*"("{B}*{ID}{B}*")"   {
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<ClassName>{ID}("."{ID})*               |
<ClassName>{ID}("::"{ID})*              {
                                          if (yyextra->lang==SrcLangExt_CSharp)
                                            yyextra->curClassName=substitute(yytext,".","::");
                                          else
                                            yyextra->curClassName=yytext;
                                          addType(yyscanner);
                                          if (yyextra->curClassName=="alignas")
                                          {
                                            startFontClass(yyscanner,"keyword");
                                            yyextra->code->codify(yytext);
                                            endFontClass(yyscanner);
                                            BEGIN( AlignAs );
                                          }
                                          else
                                          {
                                            generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                            BEGIN( ClassVar );
                                          }
                                        }
<AlignAs>"("                            {
                                          yyextra->bracketCount=1;
                                          yyextra->code->codify(yytext);
                                          BEGIN( AlignAsEnd );
                                        }
<AlignAs>\n                             { yyextra->yyLineNr++;
                                          codifyLines(yyscanner,yytext);
                                        }
<AlignAs>.                              { yyextra->code->codify(yytext); }
<AlignAsEnd>"("                         { yyextra->code->codify(yytext);
                                          yyextra->bracketCount++;
                                        }
<AlignAsEnd>")"                         {
                                          yyextra->code->codify(yytext);
                                          if (--yyextra->bracketCount<=0)
                                          {
                                            BEGIN(ClassName);
                                          }
                                        }
<AlignAsEnd>\n                          { yyextra->yyLineNr++;
                                          codifyLines(yyscanner,yytext);
                                        }
<AlignAsEnd>.                           { yyextra->code->codify(yytext); }
<ClassName>{ID}("\\"{ID})*              { // PHP namespace
                                          yyextra->curClassName=substitute(yytext,"\\","::");
                                          yyextra->scopeStack.push(CLASSBLOCK);
                                          pushScope(yyscanner,yyextra->curClassName);
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          BEGIN( ClassVar );
                                        }
<ClassName>{ID}{B}*"("{ID}")"           { // Obj-C category
                                          yyextra->curClassName=removeRedundantWhiteSpace(yytext);
                                          yyextra->scopeStack.push(CLASSBLOCK);
                                          pushScope(yyscanner,yyextra->curClassName);
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          BEGIN( ClassVar );
                                        }
<PackageName>{ID}("."{ID})*             {
                                          yyextra->curClassName=substitute(yytext,".","::");
                                          DBG_CTX((stderr,"found package: %s\n",qPrint(yyextra->curClassName)));
                                          addType(yyscanner);
                                          codifyLines(yyscanner,yytext);
                                        }
<ClassVar>"="                           {
                                          unput(*yytext);
                                          BEGIN( Body );
                                        }
<ClassVar>("extends"|"implements")      { // Java, Slice
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->curClassBases.clear();
                                          BEGIN( Bases );
                                        }
<ClassVar>("sealed"|"abstract")/{BN}*(":"|"{") {
                                          DBG_CTX((stderr,"***** C++/CLI modifier %s on yyextra->curClassName=%s\n",yytext,qPrint(yyextra->curClassName)));
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( CppCliTypeModifierFollowup );
                                        }
<ClassVar>{ID}                          {
                                          yyextra->type = yyextra->curClassName;
                                          yyextra->name = yytext;
                                          if (yyextra->insideBody)
                                          {
                                            addVariable(yyscanner,yyextra->type,yyextra->name);
                                          }
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                        }
<ClassName,ClassVar,CppCliTypeModifierFollowup>{B}*":"{B}*      {
                                          codifyLines(yyscanner,yytext);
                                          yyextra->curClassBases.clear();
                                          BEGIN( Bases );
                                        }
<PackageName>[ \t]*";"                  |
<Bases>^{Bopt}/"@"{ID}                    | // Objective-C interface
<Bases,ClassName,ClassVar,CppCliTypeModifierFollowup>{B}*"{"{B}* {
                                          yyextra->theVarContext.pushScope();
                                          if (!yyextra->skipCodify) yyextra->code->codify(yytext);
                                          yyextra->skipCodify = false;
                                          if (YY_START==ClassVar && yyextra->curClassName.isEmpty())
                                          {
                                            yyextra->curClassName = yyextra->name;
                                          }
                                          if (yyextra->searchingForBody)
                                          {
                                            yyextra->searchingForBody=FALSE;
                                            yyextra->insideBody=TRUE;
                                          }
                                          if (yyextra->insideBody) yyextra->bodyCurlyCount++;
                                          if (!yyextra->curClassName.isEmpty()) // valid class name
                                          {
                                            DBG_CTX((stderr,"** scope stack push CLASSBLOCK\n"));
                                            yyextra->scopeStack.push(CLASSBLOCK);
                                            pushScope(yyscanner,yyextra->curClassName);
                                            DBG_CTX((stderr,"***** yyextra->curClassName=%s\n",qPrint(yyextra->curClassName)));
                                            if (yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,yyextra->curClassName)==0)
                                            {
                                              DBG_CTX((stderr,"Adding new class %s\n",qPrint(yyextra->curClassName)));
                                              ScopedTypeVariant var(yyextra->curClassName);
                                              // insert base classes.
                                              for (const auto &s : yyextra->curClassBases)
                                              {
                                                const ClassDef *bcd=0;
                                                auto it = yyextra->codeClassMap.find(s);
                                                if (it!=yyextra->codeClassMap.end())
                                                {
                                                  bcd = toClassDef(it->second.globalDef());
                                                }
                                                if (bcd==0) bcd=yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,QCString(s));
                                                if (bcd && bcd->name()!=yyextra->curClassName)
                                                {
                                                  var.localDef()->insertBaseClass(bcd->name());
                                                }
                                              }
                                              yyextra->codeClassMap.emplace(std::make_pair(yyextra->curClassName.str(),std::move(var)));
                                            }
                                            //printf("yyextra->codeClassList.count()=%d\n",yyextra->codeClassList.count());
                                          }
                                          else // not a class name -> assume inner block
                                          {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                            yyextra->scopeStack.push(INNERBLOCK);
                                          }
                                          yyextra->curClassName.resize(0);
                                          yyextra->curClassBases.clear();
                                          BEGIN( Body );
                                        }
<Bases>"virtual"|"public"|"protected"|"private"|"@public"|"@private"|"@protected" {
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<Bases>{SEP}?({ID}{SEP})*{ID}           {
                                          DBG_CTX((stderr,"%s:addBase(%s)\n",qPrint(yyextra->curClassName),yytext));
                                          yyextra->curClassBases.push_back(yytext);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                        }
<Bases>"<"                              {
                                          yyextra->code->codify(yytext);
                                          if (!yyextra->insideObjC)
                                          {
                                            yyextra->sharpCount=1;
                                            BEGIN ( SkipSharp );
                                          }
                                          else
                                          {
                                            yyextra->insideProtocolList=TRUE;
                                          }
                                        }
<Bases>">"                              {
                                          yyextra->code->codify(yytext);
                                          yyextra->insideProtocolList=FALSE;
                                        }
<SkipSharp>"<"                          {
                                          yyextra->code->codify(yytext);
                                          ++yyextra->sharpCount;
                                        }
<SkipSharp>">"                          {
                                          yyextra->code->codify(yytext);
                                          if (--yyextra->sharpCount<=0)
                                          BEGIN ( Bases );
                                        }
<SkipSharp>"\""                         {
                                          yyextra->code->codify(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN(SkipString);
                                        }
<SkipSharp>"\'"                         {
                                          yyextra->code->codify(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN(SkipStringS);
                                        }
<Bases>"("                              {
                                          yyextra->code->codify(yytext);
                                          yyextra->sharpCount=1;
                                          BEGIN ( SkipSharp );
                                        }
<SkipSharp>"("                          {
                                          yyextra->code->codify(yytext);
                                          ++yyextra->sharpCount;
                                        }
<SkipSharp>")"                          {
                                          yyextra->code->codify(yytext);
                                          if (--yyextra->sharpCount<=0)
                                            BEGIN ( Bases );
                                        }


<Bases>","                              {
                                          yyextra->code->codify(yytext);
                                        }


<Body>{SCOPEPREFIX}?"operator"{B}*"()"{Bopt}/"(" {
                                          addType(yyscanner);
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->bracketCount=0;
                                          yyextra->args.resize(0);
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<Body>{SCOPEPREFIX}?"operator"/"("      {
                                          addType(yyscanner);
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->bracketCount=0;
                                          yyextra->args.resize(0);
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<Body>{SCOPEPREFIX}?"operator"[^a-z_A-Z0-9\(\n]+/"(" {
                                          addType(yyscanner);
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->bracketCount=0;
                                          yyextra->args.resize(0);
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<Body,TemplDecl>("template"|"generic")/([^a-zA-Z0-9])           {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->insideTemplate=TRUE;
                                          yyextra->sharpCount=0;
                                        }
<Body>"concept"{BN}+                    {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN(ConceptName);
                                        }
<Body>"using"{BN}+"namespace"{BN}+      {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN(UsingName);
                                        }
<Body>"using"{BN}+                      {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN(UsingName);
                                        }
<ConceptName>{ID}("::"{ID})*              {
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                        }
<ConceptName>"="                        { codifyLines(yyscanner,yytext); BEGIN(Body); }
<UsingName>{ID}(("::"|"."){ID})*        {
                                          addUsingDirective(yyscanner,substitute(yytext,".","::"));
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          BEGIN(Body);
                                        }
<UsingName>\n                           { codifyLines(yyscanner,yytext); BEGIN(Body); }
<UsingName>.                            { codifyLines(yyscanner,yytext); BEGIN(Body); }
<Body,FuncCall>"$"?"this"("->"|".")     { yyextra->code->codify(yytext); // this-> for C++, this. for C#
                                          yyextra->isPrefixedWithThis = TRUE;
                                        }
<Body>{KEYWORD}/([^a-z_A-Z0-9])         {
                                          if (yyextra->lang==SrcLangExt_Java && qstrcmp("internal",yytext) ==0) REJECT;
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          if (QCString(yytext)=="typedef")
                                          {
                                            addType(yyscanner);
                                            yyextra->name+=yytext;
                                          }
                                          endFontClass(yyscanner);
                                        }
<Body>{KEYWORD}/{B}*                    {
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Body>{KEYWORD}/{BN}*"("                {
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                        }
<FuncCall>"in"/{BN}*                    {
                                          if (!yyextra->inForEachExpression) REJECT;
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          // insert the variable in the parent scope, see bug 546158
                                          yyextra->theVarContext.popScope();
                                          addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          yyextra->theVarContext.pushScope();
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                        }
<Body>{FLOWKW}/{BN}*"("                         {
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                          yyextra->inForEachExpression = (qstrcmp(yytext,"for each")==0 || qstrcmp(yytext, "foreach")==0);
                                          BEGIN(FuncCall);
                                        }
<Body>{FLOWCONDITION}/{BN}*"("          {
                                          incrementFlowKeyWordCount(yyscanner);
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                          yyextra->inForEachExpression = (strcmp(yytext,"for each")==0 || strcmp(yytext, "foreach")==0);
                                          BEGIN(FuncCall);
                                        }
<Body>{FLOWKW}/([^a-z_A-Z0-9])          {
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          if (yyextra->inFunctionTryBlock && (qstrcmp(yytext,"catch")==0 || qstrcmp(yytext,"finally")==0))
                                          {
                                            yyextra->inFunctionTryBlock=FALSE;
                                          }
                                        }
<Body>{FLOWCONDITION}/([^a-z_A-Z0-9])   {
                                          incrementFlowKeyWordCount(yyscanner);
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          if (yyextra->inFunctionTryBlock && (strcmp(yytext,"catch")==0 || strcmp(yytext,"finally")==0))
                                          {
                                            yyextra->inFunctionTryBlock=FALSE;
                                          }
                                        }
<Body>{FLOWKW}/{B}*                     {
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Body>{FLOWCONDITION}/{B}*              {
                                          incrementFlowKeyWordCount(yyscanner);
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Body>"*"{B}*")"                        { // end of cast?
                                          yyextra->code->codify(yytext);
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
                                          yyextra->bracketCount--;
                                          yyextra->parmType = yyextra->name;
                                          BEGIN(FuncCall);
                                        }
<Body>"\\)"|"\\("                       {
                                          yyextra->code->codify(yytext);
                                        }
<Body>[\\|\)\+\-\/\%\~\!]               {
                                          yyextra->code->codify(yytext);
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                          if (*yytext==')')
                                          {
                                            yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
                                            yyextra->bracketCount--;
                                            BEGIN(FuncCall);
                                          }
                                        }
<Body,TemplDecl,ObjCMethod>{TYPEKW}/{B}* {
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          addType(yyscanner);
                                          yyextra->name+=yytext;
                                        }
<Body,TemplDecl,ObjCMethod>{TYPEKWSL}/{B}* {
                                          if (yyextra->lang!=SrcLangExt_Slice)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            startFontClass(yyscanner,"keywordtype");
                                            yyextra->code->codify(yytext);
                                            endFontClass(yyscanner);
                                            addType(yyscanner);
                                            yyextra->name+=yytext;
                                          }
                                        }
<Body>"generic"/{B}*"<"[^\n\/\-\.\{\"\>]*">"{B}* {
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          yyextra->sharpCount=0;
                                          BEGIN(TemplDecl);
                                        }
<Body>"template"/{B}*"<"[^\n\/\-\.\{\"\>]*">"{B}* { // template<...>
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          yyextra->sharpCount=0;
                                          BEGIN(TemplDecl);
                                        }
<TemplDecl>"class"|"typename"           {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<TemplDecl>"<"                          {
                                          yyextra->code->codify(yytext);
                                          yyextra->sharpCount++;
                                        }
<TemplDecl>">"                          {
                                          yyextra->code->codify(yytext);
                                          yyextra->sharpCount--;
                                          if (yyextra->sharpCount<=0)
                                          {
                                            BEGIN(Body);
                                          }
                                        }
<TemplCast>">"                          {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastTemplCastContext );
                                        }
<TemplCast>{ID}("::"{ID})*              {
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                        }
<TemplCast>("const"|"volatile"){B}*     {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<TemplCast>[*^]*                        {
                                          codifyLines(yyscanner,yytext);
                                        }
<Body,MemberCall2,FuncCall>{CASTKW}{B}*"<"  { // static_cast<T>(
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->lastTemplCastContext = YY_START;
                                          BEGIN(TemplCast);
                                        }
<Body>"$this->"{SCOPENAME}/{BN}*[;,)\]] { // PHP member variable
                                          addType(yyscanner);
                                          generatePHPVariableLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext+7;
                                        }
<Body,TemplCast>{SCOPENAME}{B}*"<"[^\n\/\-\.\{\"\>\(]*">"{ENDIDopt}/{B}* { // A<T> *pt;
                                          if (isCastKeyword(yytext) && YY_START==Body)
                                          {
                                            REJECT;
                                          }
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext;
                                        }
<Body>{SCOPENAME}/{BN}*[:;,)\]]         { // "int var;" or "var, var2" or "debug(f) macro" , or int var : 5;
                                          if (startsWithKeyword(yytext,"typedef")) REJECT;
                                          addType(yyscanner);
                                          // changed this to generateFunctionLink, see bug 624514
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext;
                                        }
<Body>{ID}("."{ID})+/{BN}+              { // CSharp/Java scope
                                          if (yyextra->lang==SrcLangExt_CSharp || yyextra->lang==SrcLangExt_Java)
                                          {
                                            addType(yyscanner);
                                            generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                            yyextra->name+=yytext;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<Body>{SCOPENAME}/{B}*                  { // p->func()
                                          if (startsWithKeyword(yytext,"typedef")) REJECT;
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext;
                                        }
<Body>"("{B}*("*"{B}*)+{SCOPENAME}+{B}*")"/{B}* {  // (*p)->func() but not "if (p) ..."
                                          yyextra->code->codify(yytext);
                                          uint s=0;while (s<(uint)yyleng && !isId(yytext[s])) s++;
                                          uint e=(uint)yyleng-1;while (e>1 && !isId(yytext[e])) e--;
                                          QCString varname = ((QCString)yytext).mid(s,e-s+1);
                                          addType(yyscanner);
                                          yyextra->name=varname;
                                        }
<Body>{SCOPETNAME}{B}*"<"[^\n\/\-\.\{\"\>]*">"/{BN}*"(" |
<Body>{SCOPETNAME}/{BN}*"("             { // a() or c::a() or t<A,B>::a() or A\B\foo()
                                          if (isCastKeyword(yytext))
                                          {
                                            REJECT;
                                          }
                                          addType(yyscanner);
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->bracketCount=0;
                                          yyextra->args.resize(0);
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>{RAWBEGIN}   {
                                          QCString text(yytext);
                                          uint i=(uint)text.find('R');
                                          yyextra->code->codify(text.left(i+1));
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(QCString(yytext+i+1));
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->inForEachExpression = FALSE;
                                          yyextra->delimiter = yytext+i+2;
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                                          BEGIN( RawString );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>\"   {
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->inForEachExpression = FALSE;
                                          BEGIN( SkipString );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>{NUMBER} { //Note similar code in commentcnv.l
                                          if (yyextra->lang!=SrcLangExt_Cpp) REJECT;
                                          yyextra->code->codify(yytext);
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>\'   {
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->inForEachExpression = FALSE;
                                          BEGIN( SkipStringS );
                                        }
<SkipString>[^\"\\\r\n]*                {
                                          yyextra->code->codify(yytext);
                                        }
<SkipStringS>[^\'\\\r\n]*               {
                                          yyextra->code->codify(yytext);
                                        }
<SkipString,SkipStringS>{CPPC}|{CCS}       {
                                          yyextra->code->codify(yytext);
                                        }
<SkipString>@?\"                        {
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastStringContext );
                                        }
<SkipStringS>\'                         {
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastStringContext );
                                        }
<SkipString,SkipStringS>\\.             {
                                          yyextra->code->codify(yytext);
                                        }
<RawString>{RAWEND}                     {
                                          yyextra->code->codify(yytext);
                                          QCString delimiter(yytext+1);
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==yyextra->delimiter)
                                          {
                                            BEGIN( yyextra->lastStringContext );
                                          }
                                        }
<RawString>[^)\n]+                      { yyextra->code->codify(yytext); }
<RawString>.                            { yyextra->code->codify(yytext); }
<RawString>\n                           { codifyLines(yyscanner,yytext); }
<SkipVerbString>[^"\n]+                 {
                                          yyextra->code->codify(yytext);
                                        }
<SkipVerbString>\"\"                    { // escaped quote
                                          yyextra->code->codify(yytext);
                                        }
<SkipVerbString>\"                      { // end of string
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastVerbStringContext );
                                        }
<SkipVerbString>.                       {
                                          yyextra->code->codify(yytext);
                                        }
<SkipVerbString>\n                      {
                                          codifyLines(yyscanner,yytext);
                                        }
<Body>":"                               {
                                          yyextra->code->codify(yytext);
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                        }
<Body>"<"                               {
                                          if (yyextra->insideTemplate)
                                          {
                                            yyextra->sharpCount++;
                                          }
                                          yyextra->code->codify(yytext);
                                        }
<Body>">"                               {
                                          if (yyextra->insideTemplate)
                                          {
                                            if (--yyextra->sharpCount<=0)
                                            {
                                              yyextra->insideTemplate=FALSE;
                                            }
                                          }
                                          yyextra->code->codify(yytext);
                                        }
<Body,MemberCall,MemberCall2,FuncCall>"'"((\\0[Xx0-9]+)|(\\.)|(.))"'"   {
                                          startFontClass(yyscanner,"charliteral");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<Body>"."|"->"                          {
                                          if (yytext[0]=='-') // -> could be overloaded
                                          {
                                            updateCallContextForSmartPointer(yyscanner);
                                          }
                                          yyextra->code->codify(yytext);
                                          yyextra->memCallContext = YY_START;
                                          BEGIN( MemberCall );
                                        }
<MemberCall>{SCOPETNAME}/{BN}*"("       {
                                          if (yyextra->theCallContext.getScope().globalDef())
                                          {
                                            if (!generateClassMemberLink(yyscanner,*yyextra->code,yyextra->theCallContext.getScope().globalDef(),yytext))
                                            {
                                              codifyLines(yyscanner,yytext);
                                              addToSearchIndex(yyscanner,yytext);
                                            }
                                            yyextra->name.resize(0);
                                          }
                                          else
                                          {
                                              codifyLines(yyscanner,yytext);
                                            addToSearchIndex(yyscanner,yytext);
                                            yyextra->name.resize(0);
                                          }
                                          yyextra->type.resize(0);
                                          if (yyextra->memCallContext==Body)
                                          {
                                            BEGIN(FuncCall);
                                          }
                                          else
                                          {
                                            BEGIN(yyextra->memCallContext);
                                          }
                                        }
<MemberCall>{SCOPENAME}/{B}*            {
                                          if (yyextra->theCallContext.getScope().globalDef())
                                          {
                                            DBG_CTX((stderr,"yyextra->theCallContext.getClass()=%p\n",(void*)yyextra->theCallContext.getScope().globalDef()));
                                            if (!generateClassMemberLink(yyscanner,*yyextra->code,yyextra->theCallContext.getScope().globalDef(),yytext))
                                            {
                                              codifyLines(yyscanner,yytext);
                                              addToSearchIndex(yyscanner,yytext);
                                            }
                                            yyextra->name.resize(0);
                                          }
                                          else
                                          {
                                            DBG_CTX((stderr,"no class context!\n"));
                                            codifyLines(yyscanner,yytext);
                                            addToSearchIndex(yyscanner,yytext);
                                            yyextra->name.resize(0);
                                          }
                                          yyextra->type.resize(0);
                                          BEGIN(yyextra->memCallContext);
                                        }
<Body>[,=;\[]                           {
                                          if (yyextra->insideObjC && *yytext=='[')
                                          {
                                            DBG_CTX((stderr,"Found start of ObjC call!\n"));
                                            // start of a method call
                                            yyextra->contextMap.clear();
                                            yyextra->nameMap.clear();
                                            yyextra->objectMap.clear();
                                            yyextra->wordMap.clear();
                                            yyextra->commentMap.clear();
                                            yyextra->currentCtxId  = 0;
                                            yyextra->currentNameId  = 0;
                                            yyextra->currentObjId  = 0;
                                            yyextra->currentCtx = 0;
                                            yyextra->braceCount = 0;
                                            unput('[');
                                            BEGIN(ObjCCall);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(yytext);
                                            yyextra->saveName = yyextra->name;
                                            yyextra->saveType = yyextra->type;
                                            if (*yytext!='[' && !yyextra->type.isEmpty())
                                            {
                                              //printf("yyextra->scopeStack.bottom()=%p\n",yyextra->scopeStack.bottom());
                                              //if (yyextra->scopeStack.top()!=CLASSBLOCK) // commented out for bug731363
                                              {
                                                //printf("AddVariable: '%s' '%s' context=%d\n",
                                                //    qPrint(yyextra->type),qPrint(yyextra->name),yyextra->theVarContext.count());
                                                addVariable(yyscanner,yyextra->type,yyextra->name);
                                              }
                                              yyextra->name.resize(0);
                                            }
                                            if (*yytext==';' || *yytext=='=')
                                            {
                                              yyextra->type.resize(0);
                                              yyextra->name.resize(0);
                                            }
                                            else if (*yytext=='[')
                                            {
                                              yyextra->theCallContext.pushScope(yyextra->name, yyextra->type);
                                            }
                                            yyextra->args.resize(0);
                                            yyextra->parmType.resize(0);
                                            yyextra->parmName.resize(0);
                                          }
                                        }
<ObjCCall,ObjCMName>"["|"{"       {
                                    saveObjCContext(yyscanner);
                                    yyextra->currentCtx->format+=*yytext;
                                    BEGIN(ObjCCall);
                                    DBG_CTX((stderr,"open\n"));
                                  }
<ObjCCall,ObjCMName>"]"|"}"       {
                                    yyextra->currentCtx->format+=*yytext;
                                    restoreObjCContext(yyscanner);
                                    BEGIN(ObjCMName);
                                    if (yyextra->currentCtx==0)
                                    {
                                      // end of call
                                      ObjCCallCtx *ctx = 0;
                                      auto it = yyextra->contextMap.find(0);
                                      if (it!=yyextra->contextMap.end())
                                      {
                                        ctx = it->second.get();
                                      }
                                      writeObjCMethodCall(yyscanner,ctx);
                                      BEGIN(Body);
                                    }
                                    DBG_CTX((stderr,"close\n"));
                                  }
<ObjCCall,ObjCMName>{CPPC}.*        {
                                    yyextra->currentCtx->format+=escapeComment(yyscanner,yytext);
                                  }
<ObjCCall,ObjCMName>{CCS}          {
                                    yyextra->lastObjCCallContext = YY_START;
                                    yyextra->currentCtx->comment.str(yytext);
                                    BEGIN(ObjCCallComment);
                                  }
<ObjCCallComment>{CCE}             {
                                    yyextra->currentCtx->comment << yytext;
                                    std::string commentStr = yyextra->currentCtx->comment.str();
                                    yyextra->currentCtx->format+=escapeComment(yyscanner,commentStr.c_str());
                                    BEGIN(yyextra->lastObjCCallContext);
                                  }
<ObjCCallComment>[^*\n]+          { yyextra->currentCtx->comment << yytext; }
<ObjCCallComment>{CPPC}|{CCS}        { yyextra->currentCtx->comment << yytext; }
<ObjCCallComment>\n               { yyextra->currentCtx->comment << *yytext; }
<ObjCCallComment>.                { yyextra->currentCtx->comment << *yytext; }
<ObjCCall>{ID}                    {
                                    yyextra->currentCtx->format+=escapeObject(yyscanner,yytext);
                                    if (yyextra->braceCount==0)
                                    {
                                      yyextra->currentCtx->objectTypeOrName=yytext;
                                      DBG_CTX((stderr,"new type=%s\n",qPrint(yyextra->currentCtx->objectTypeOrName)));
                                      BEGIN(ObjCMName);
                                    }
                                  }
<ObjCMName>{ID}/{BN}*"]"          {
                                    if (yyextra->braceCount==0 &&
                                        yyextra->currentCtx->methodName.isEmpty())
                                    {
                                      yyextra->currentCtx->methodName=yytext;
                                      yyextra->currentCtx->format+=escapeName(yyscanner,yytext);
                                    }
                                    else
                                    {
                                      yyextra->currentCtx->format+=escapeWord(yyscanner,yytext);
                                    }
                                  }
<ObjCMName>{ID}/{BN}*":"           {
                                     if (yyextra->braceCount==0)
                                     {
                                       yyextra->currentCtx->methodName+=yytext;
                                       yyextra->currentCtx->methodName+=":";
                                     }
                                     yyextra->currentCtx->format+=escapeName(yyscanner,yytext);
                                   }
<ObjCSkipStr>[^\n\"$\\]*           { yyextra->currentCtx->format+=yytext; }
<ObjCSkipStr>\\.                   { yyextra->currentCtx->format+=yytext; }
<ObjCSkipStr>"\""                  { yyextra->currentCtx->format+=yytext;
                                      BEGIN(yyextra->lastStringContext);
                                   }
<ObjCCall,ObjCMName>{CHARLIT}      { yyextra->currentCtx->format+=yytext; }
<ObjCCall,ObjCMName>"@"?"\""       { yyextra->currentCtx->format+=yytext;
                                      yyextra->lastStringContext=YY_START;
                                      BEGIN(ObjCSkipStr);
                                   }
<ObjCCall,ObjCMName,ObjCSkipStr>"$" { yyextra->currentCtx->format+="$$"; }
<ObjCCall,ObjCMName>"("            { yyextra->currentCtx->format+=*yytext; yyextra->braceCount++; }
<ObjCCall,ObjCMName>")"            { yyextra->currentCtx->format+=*yytext; yyextra->braceCount--; }
<ObjCSkipStr>"@"/"\""              { // needed to prevent matching the global rule (for C#)
                                     yyextra->currentCtx->format+=yytext;
                                   }
<ObjCCall,ObjCMName,ObjCSkipStr>{ID} { yyextra->currentCtx->format+=escapeWord(yyscanner,yytext); }
<ObjCCall,ObjCMName,ObjCSkipStr>.  { yyextra->currentCtx->format+=*yytext; }
<ObjCCall,ObjCMName,ObjCSkipStr>\n { yyextra->currentCtx->format+=*yytext; }

<Body>"]"                               {
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
                                          yyextra->code->codify(yytext);
                                          // TODO: nested arrays like: a[b[0]->func()]->func()
                                          yyextra->name = yyextra->saveName;
                                          yyextra->type = yyextra->saveType;
                                        }
<Body>[0-9]+                            {
                                          yyextra->code->codify(yytext);
                                        }
<Body>[0-9]+[xX][0-9A-Fa-f]+            {
                                          yyextra->code->codify(yytext);
                                        }
<MemberCall2,FuncCall>{KEYWORD}/([^a-z_A-Z0-9]) {
                                          //addParmType(yyscanner);
                                          //yyextra->parmName=yytext;
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<MemberCall2,FuncCall,OldStyleArgs,TemplCast>{TYPEKW}/([^a-z_A-Z0-9]) {
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          startFontClass(yyscanner,"keywordtype");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<MemberCall2,FuncCall,OldStyleArgs,TemplCast>{TYPEKWSL}/([^a-z_A-Z0-9]) {
                                          if (yyextra->lang!=SrcLangExt_Slice)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            addParmType(yyscanner);
                                            yyextra->parmName=yytext;
                                            startFontClass(yyscanner,"keywordtype");
                                            yyextra->code->codify(yytext);
                                            endFontClass(yyscanner);
                                          }
                                        }
<MemberCall2,FuncCall>{FLOWKW}/([^a-z_A-Z0-9]) {
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          startFontClass(yyscanner,"keywordflow");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<MemberCall2,FuncCall>{FLOWCONDITION}/([^a-z_A-Z0-9]) {
                                          incrementFlowKeyWordCount(yyscanner);
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          startFontClass(yyscanner,"keywordflow");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<MemberCall2,FuncCall>("::")?{ID}(({B}*"<"[^\n\[\](){}<>]*">")?({B}*"::"{B}*{ID})?)* {
                                          if (isCastKeyword(yytext))
                                          {
                                            REJECT;
                                          }
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,!yyextra->insideBody);
                                        }
<FuncCall>";"                           { // probably a cast, not a function call
                                          yyextra->code->codify(yytext);
                                          yyextra->inForEachExpression = FALSE;
                                          BEGIN( Body );
                                        }
<MemberCall2,FuncCall>,                 {
                                          yyextra->code->codify(yytext);
                                          addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                        }
<MemberCall2,FuncCall>"{"               {
                                          if (yyextra->bracketCount>0)
                                          {
                                            yyextra->code->codify(yytext);
                                            yyextra->skipInlineInitContext=YY_START;
                                            yyextra->curlyCount=0;
                                            BEGIN(InlineInit);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<InlineInit>"{"                         { yyextra->curlyCount++;
                                          yyextra->code->codify(yytext);
                                        }
<InlineInit>"}"                         {
                                          yyextra->code->codify(yytext);
                                          if (--yyextra->curlyCount<=0)
                                          {
                                            BEGIN(yyextra->skipInlineInitContext);
                                          }
                                        }
<InlineInit>\n                          {
                                          codifyLines(yyscanner,yytext);
                                        }
<InlineInit>.                           {
                                          yyextra->code->codify(yytext);
                                        }
<MemberCall2,FuncCall>"("               {
                                          yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                          yyextra->code->codify(yytext);
                                          yyextra->bracketCount++;
                                          yyextra->theCallContext.pushScope(yyextra->name, yyextra->type);
                                          if (YY_START==FuncCall && !yyextra->insideBody)
                                          {
                                            yyextra->theVarContext.pushScope();
                                          }
                                        }
<MemberCall2,FuncCall>{OPERATOR}        { // operator
                                          if (qstrcmp(yytext,"*") &&
                                              qstrcmp(yytext,"&") &&
                                              qstrcmp(yytext,"^") &&
                                              qstrcmp(yytext,"%")) // typically a pointer or reference
                                          {
                                            // not a * or &, or C++/CLI's ^ or %
                                            yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                          }
                                          yyextra->code->codify(yytext);
                                        }
<MemberCall,MemberCall2,FuncCall>("*"{B}*)?")"  {
                                          if (yytext[0]==')') // no a pointer cast
                                          {
                                            DBG_CTX((stderr,"addVariable(%s,%s)\n",qPrint(yyextra->parmType),qPrint(yyextra->parmName)));
                                            if (yyextra->parmType.isEmpty())
                                            {
                                              yyextra->parmType=yyextra->parmName;
                                              yyextra->parmName.resize(0);
                                            }
                                            addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          }
                                          else
                                          {
                                            yyextra->parmType = yyextra->parmName;
                                            yyextra->parmName.resize(0);
                                            addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          }
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
                                          yyextra->inForEachExpression = FALSE;
                                          //yyextra->theCallContext.setClass(0); // commented out, otherwise a()->b() does not work for b().
                                          yyextra->code->codify(yytext);
                                          if (--yyextra->bracketCount<=0)
                                          {
                                            if (yyextra->name.isEmpty())
                                            {
                                              BEGIN( Body );
                                            }
                                            else
                                            {
                                              BEGIN( CallEnd );
                                            }
                                          }
                                        }
<CallEnd>[ \t\n]*                       { codifyLines(yyscanner,yytext); }
  /*
<MemberCall2,FuncCall>")"[ \t\n]*[;:]   {
  */
<CallEnd>[;:]                           {
                                          codifyLines(yyscanner,yytext);
                                          yyextra->bracketCount=0;
                                          if (*yytext==';') yyextra->searchingForBody=FALSE;
                                          if (!yyextra->type.isEmpty())
                                          {
                                            DBG_CTX((stderr,"add variable yyextra->type=%s yyextra->name=%s)\n",qPrint(yyextra->type),qPrint(yyextra->name)));
                                            addVariable(yyscanner,yyextra->type,yyextra->name);
                                          }
                                          yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                          yyextra->theCallContext.setScope(ScopedTypeVariant());
                                          if (*yytext==';' || yyextra->insideBody)
                                          {
                                            if (!yyextra->insideBody)
                                            {
                                              yyextra->theVarContext.popScope();
                                            }
                                            yyextra->name.resize(0);yyextra->type.resize(0);
                                            BEGIN( Body );
                                          }
                                          else
                                          {
                                            yyextra->bracketCount=0;
                                            BEGIN( SkipInits );
                                          }
                                        }
<CallEnd>{ENDQopt}/{BN}*(";"|"="|"throw"{BN}*"(") {
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<CallEnd,OldStyleArgs>("const"|"volatile"|"sealed"|"override")*({BN}+("const"|"volatile"|"sealed"|"override"))*{BN}*"{" {
                                          if (yyextra->insideBody)
                                          {
                                            yyextra->theVarContext.pushScope();
                                          }
                                          addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          //yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
                                          yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                          int index = yyextra->name.findRev("::");
                                          DBG_CTX((stderr,"yyextra->name=%s\n",qPrint(yyextra->name)));
                                          if (index!=-1)
                                          {
                                            QCString scope = yyextra->name.left((uint)index);
                                            if (!yyextra->classScope.isEmpty()) scope.prepend((yyextra->classScope+"::"));
                                            const ClassDef *cd=yyextra->symbolResolver.resolveClass(Doxygen::globalScope,scope);
                                            if (cd)
                                            {
                                              setClassScope(yyscanner,cd->name());
                                              yyextra->scopeStack.push(SCOPEBLOCK);
                                              DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
                                            }
                                            else
                                            {
                                              //setClassScope(yyscanner,yyextra->realScope);
                                              yyextra->scopeStack.push(INNERBLOCK);
                                              DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                            }
                                          }
                                          else
                                          {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                            yyextra->scopeStack.push(INNERBLOCK);
                                          }
                                          yytext[yyleng-1]='\0';
                                          QCString cv(yytext);
                                          if (!cv.stripWhiteSpace().isEmpty())
                                          {
                                            startFontClass(yyscanner,"keyword");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
                                          else // just whitespace
                                          {
                                            codifyLines(yyscanner,yytext);
                                          }
                                          yyextra->code->codify("{");
                                          if (yyextra->searchingForBody)
                                          {
                                            yyextra->searchingForBody=FALSE;
                                            yyextra->insideBody=TRUE;
                                          }
                                          if (yyextra->insideBody) yyextra->bodyCurlyCount++;
                                          yyextra->type.resize(0); yyextra->name.resize(0);
                                          BEGIN( Body );
                                        }
<CallEnd>"try"                          { // function-try-block
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          yyextra->inFunctionTryBlock=TRUE;
                                        }
<CallEnd>"requires"                     { // function-try-block
                                          startFontClass(yyscanner,"keyword");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<CallEnd>{ID}                           {
                                          if (yyextra->insideBody || !yyextra->parmType.isEmpty())
                                          {
                                            REJECT;
                                          }
                                          // could be K&R style definition
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,!yyextra->insideBody);
                                          BEGIN(OldStyleArgs);
                                        }
<OldStyleArgs>{ID}                      {
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,!yyextra->insideBody);
                                        }
<OldStyleArgs>[,;]                      {
                                          yyextra->code->codify(yytext);
                                          addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          if (*yytext==';') yyextra->parmType.resize(0);
                                          yyextra->parmName.resize(0);
                                        }
<CallEnd,OldStyleArgs>"#"               {
                                          startFontClass(yyscanner,"preprocessor");
                                          yyextra->lastSkipCppContext = Body;
                                          yyextra->code->codify(yytext);
                                          BEGIN( SkipCPP );
                                        }
<CallEnd>.                              {
                                          unput(*yytext);
                                          if (!yyextra->insideBody)
                                          {
                                            yyextra->theVarContext.popScope();
                                          }
                                          yyextra->name.resize(0);yyextra->args.resize(0);
                                          yyextra->parmType.resize(0);yyextra->parmName.resize(0);
                                          BEGIN( Body );
                                        }
<SkipInits>";"                          {
                                          yyextra->code->codify(yytext);
                                          yyextra->type.resize(0); yyextra->name.resize(0);
                                          BEGIN( Body );
                                        }
<SkipInits>"{"                          {
                                          yyextra->code->codify(yytext);
                                          if (yyextra->searchingForBody)
                                          {
                                            yyextra->searchingForBody=FALSE;
                                            yyextra->insideBody=TRUE;
                                          }
                                          if (yyextra->insideBody) yyextra->bodyCurlyCount++;
                                          if (yyextra->name.find("::")!=-1)
                                          {
                                            DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
                                            yyextra->scopeStack.push(SCOPEBLOCK);
                                            setClassScope(yyscanner,yyextra->realScope);
                                          }
                                          else
                                          {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                            yyextra->scopeStack.push(INNERBLOCK);
                                          }
                                          yyextra->type.resize(0); yyextra->name.resize(0);
                                          BEGIN( Body );
                                        }
<SkipInits>{ID}{B}*"{"                  {
                                          QCString text(yytext);
                                          int bracketPos = text.find('{');
                                          int spacePos = text.find(' ');
                                          int len = spacePos==-1 ? bracketPos : spacePos;
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,text.left(len));
                                          yyextra->code->codify(QCString(yytext+len));
                                        }
<SkipInits>{ID}                         {
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                        }
<FuncCall>{ID}/"("                      {
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                        }
<FuncCall>{ID}/("."|"->")               {
                                          yyextra->name=yytext;
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          BEGIN( MemberCall2 );
                                        }
<FuncCall,MemberCall2>("("{B}*("*"{B}*)+{ID}+{B}*")"{B}*)/("."|"->") {
                                          yyextra->code->codify(yytext);
                                          uint s=0;while (!isId(yytext[s])) s++;
                                          uint e=(uint)yyleng-1;while (e>1 && !isId(yytext[e])) e--;
                                          yyextra->name=((QCString)yytext).mid(s,e-s+1);
                                          BEGIN( MemberCall2 );
                                        }
<MemberCall2>{ID}/([ \t\n]*"(")         {
                                          if (!yyextra->args.isEmpty())
                                            generateMemberLink(yyscanner,*yyextra->code,yyextra->args,yytext);
                                          else
                                            generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->args.resize(0);
                                          BEGIN( FuncCall );
                                        }
<MemberCall2>{ID}/([ \t\n]*("."|"->"))  {
                                          //yyextra->code->codify(yytext);
                                          yyextra->name=yytext;
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          BEGIN( MemberCall2 );
                                        }
<MemberCall2>"->"|"."                   {
                                          if (yytext[0]=='-') // -> could be overloaded
                                          {
                                            updateCallContextForSmartPointer(yyscanner);
                                          }
                                          yyextra->code->codify(yytext);
                                          yyextra->memCallContext = YY_START;
                                          BEGIN( MemberCall );
                                        }
<SkipComment>{CCS}("!"?){CCE}           {
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastCContext ) ;
                                        }
<SkipComment>{CPPC}|{CCS}               {
                                          yyextra->code->codify(yytext);
                                        }
<SkipComment>[^*\/\n]+                  {
                                          yyextra->code->codify(yytext);
                                        }
<SkipComment>[ \t]*{CCE}                {
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                          if (yyextra->lastCContext==SkipCPP)
                                          {
                                            startFontClass(yyscanner,"preprocessor");
                                          }
                                          BEGIN( yyextra->lastCContext ) ;
                                        }
<SkipCxxComment>[^\r\n]*"\\"[\r]?\n     { // line continuation
                                          codifyLines(yyscanner,yytext);
                                        }
<SkipCxxComment>[^\r\n]+                {
                                          yyextra->code->codify(yytext);
                                        }
<SkipCxxComment>\r
<SkipCxxComment>\n                      {
                                          unput('\n');
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastCContext ) ;
                                        }
<SkipCxxComment>.                       {
                                          yyextra->code->codify(yytext);
                                        }
<RemoveSpecialCComment>{CCE}{B}*\n({B}*\n)*({B}*(({CPPC}"@"[{}])|({CCS}"@"[{}]{CCE})){B}*\n)?{B}*{CCS}[*!]/{NCOMM} {
                                          yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                        }
<RemoveSpecialCComment>{CCE}{B}*\n({B}*\n)*({B}*(({CPPC}"@"[{}])|({CCS}"@"[{}]{CCE})){B}*\n)? {
                                          if (yyextra->lastSpecialCContext==SkipCxxComment)
                                          { // force end of C++ comment here
                                            yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                            nextCodeLine(yyscanner);
                                            endFontClass(yyscanner);
                                            BEGIN( yyextra->lastCContext ) ;
                                          }
                                          else
                                          {
                                            yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                            if (yytext[yyleng-1]=='\n')
                                            {
                                              yyextra->yyLineNr--;
                                              unput('\n');
                                            }
                                            else
                                            {
                                              nextCodeLine(yyscanner);
                                            }
                                            BEGIN(yyextra->lastSpecialCContext);
                                          }
                                        }
<RemoveSpecialCComment>{CCE}             {
                                          BEGIN(yyextra->lastSpecialCContext);
                                        }
<RemoveSpecialCComment>[^*\n]+
<RemoveSpecialCComment>{CPPC}|{CCS}
<RemoveSpecialCComment>\n  { yyextra->yyLineNr++; }
<RemoveSpecialCComment>.
<MemberCall>[^a-z_A-Z0-9(\n]            {
                                          yyextra->code->codify(yytext);
                                          yyextra->type.resize(0);
                                          yyextra->name.resize(0);
                                          BEGIN(yyextra->memCallContext);
                                        }
<*>\n({B}*{CPPC}[!/][^\n]*\n)+            { // remove special one-line comment
                                          if (YY_START==SkipCPP) REJECT;
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                            nextCodeLine(yyscanner);
                                          }
                                          else
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
                                          if (YY_START==SkipCxxComment)
                                          {
                                            endFontClass(yyscanner);
                                            BEGIN( yyextra->lastCContext ) ;
                                          }
                                        }
<SkipCPP>\n/(.|\n)                      {
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastSkipCppContext ) ;
                                          unput('\n');
                                        }
<*>\n{B}*{CPPC}"@"[{}].*\n                  { // remove one-line group marker
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            yyextra->yyLineNr+=2;
                                            nextCodeLine(yyscanner);
                                          }
                                          else
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
                                          if (YY_START==SkipCxxComment)
                                          {
                                            endFontClass(yyscanner);
                                            BEGIN( yyextra->lastCContext ) ;
                                          }
                                        }
<*>\n{B}*{CCS}"@"[{}]                      { // remove one-line group marker
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            if (YY_START != RemoveSpecialCComment) yyextra->lastSpecialCContext = YY_START;
                                            yyextra->yyLineNr++;
                                            BEGIN(RemoveSpecialCComment);
                                          }
                                          else
                                          {
                                            // check is to prevent getting stuck in skipping C++ comments
                                            if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                            {
                                              yyextra->lastCContext = YY_START ;
                                            }
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            BEGIN(SkipComment);
                                          }
                                        }
<*>^{B}*{CPPC}"@"[{}].*\n                   { // remove one-line group marker
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            yyextra->yyLineNr++;
                                            nextCodeLine(yyscanner);
                                          }
                                          else
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
                                        }
<*>^{B}*{CCS}"@"[{}]                       { // remove multi-line group marker
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            if (YY_START != RemoveSpecialCComment) yyextra->lastSpecialCContext = YY_START;
                                            BEGIN(RemoveSpecialCComment);
                                          }
                                          else
                                          {
                                            // check is to prevent getting stuck in skipping C++ comments
                                            if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                            {
                                              yyextra->lastCContext = YY_START ;
                                            }
                                            startFontClass(yyscanner,"comment");
                                            yyextra->code->codify(yytext);
                                            BEGIN(SkipComment);
                                          }
                                        }
<*>^{B}*{CPPC}[!/][^\n]*                  { // remove special one-line comment
                                          if (!Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
                                        }
<*>{CPPC}[!/][^\n]*                       { // strip special one-line comment
                                          if (YY_START==SkipComment || YY_START==SkipString) REJECT;
                                          if (!Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                          }
                                        }
<*>\n{B}*{CCS}[!*]/{NCOMM}                 {
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            if (YY_START != RemoveSpecialCComment) yyextra->lastSpecialCContext = YY_START;
                                            yyextra->yyLineNr++;
                                            BEGIN(RemoveSpecialCComment);
                                          }
                                          else
                                          {
                                            // check is to prevent getting stuck in skipping C++ comments
                                            if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                            {
                                              yyextra->lastCContext = YY_START ;
                                            }
                                            startFontClass(yyscanner,"comment");
                                            codifyLines(yyscanner,yytext);
                                            BEGIN(SkipComment);
                                          }
                                        }
<*>^{B}*{CCS}"*"[*]+/[^/]                  { // special C "banner" comment block at a new line
                                          if (Config_getBool(JAVADOC_BANNER) && Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            if (YY_START != RemoveSpecialCComment) yyextra->lastSpecialCContext = YY_START;
                                            BEGIN(RemoveSpecialCComment);
                                          }
                                          else
                                          {
                                            // check is to prevent getting stuck in skipping C++ comments
                                            if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                            {
                                              yyextra->lastCContext = YY_START ;
                                            }
                                            startFontClass(yyscanner,"comment");
                                            yyextra->code->codify(yytext);
                                            BEGIN(SkipComment);
                                          }
                                        }
<*>^{B}*{CCS}[!*]/{NCOMM}                  { // special C comment block at a new line
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            if (YY_START != RemoveSpecialCComment) yyextra->lastSpecialCContext = YY_START;
                                            BEGIN(RemoveSpecialCComment);
                                          }
                                          else
                                          {
                                            // check is to prevent getting stuck in skipping C++ comments
                                            if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                            {
                                              yyextra->lastCContext = YY_START ;
                                            }
                                            startFontClass(yyscanner,"comment");
                                            yyextra->code->codify(yytext);
                                            BEGIN(SkipComment);
                                          }
                                        }
<*>{CCS}[!*]/{NCOMM}                       { // special C comment block half way a line
                                          if (YY_START==SkipString) REJECT;
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            if (YY_START != RemoveSpecialCComment) yyextra->lastSpecialCContext = YY_START;
                                            BEGIN(RemoveSpecialCComment);
                                          }
                                          else
                                          {
                                            // check is to prevent getting stuck in skipping C++ comments
                                            if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                            {
                                              yyextra->lastCContext = YY_START ;
                                            }
                                            startFontClass(yyscanner,"comment");
                                            yyextra->code->codify(yytext);
                                            BEGIN(SkipComment);
                                          }
                                        }
<*>{CCS}("!"?){CCE}                       {
                                          if (YY_START==SkipString) REJECT;
                                          if (!Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            startFontClass(yyscanner,"comment");
                                            yyextra->code->codify(yytext);
                                            endFontClass(yyscanner);
                                          }
                                        }
<SkipComment>[^\*\n]+                   {
                                          yyextra->code->codify(yytext);
                                        }
<*>{CCS}                                 {
                                          startFontClass(yyscanner,"comment");
                                          yyextra->code->codify(yytext);
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          BEGIN( SkipComment ) ;
                                        }
<*>@\"                                  { // C# verbatim string
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastVerbStringContext=YY_START;
                                          BEGIN(SkipVerbString);
                                        }
<*>{CPPC}                                 {
                                          startFontClass(yyscanner,"comment");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipCxxComment ) ;
                                        }
<*>"("|"["                                      {
                                          yyextra->code->codify(yytext);
                                          yyextra->theCallContext.pushScope(yyextra->name, yyextra->type);
                                        }
<*>")"|"]"                                      {
                                          yyextra->code->codify(yytext);
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
                                        }
<*>\n                                   {
                                          yyextra->yyColNr++;
                                          codifyLines(yyscanner,yytext);
                                        }
<*>[\x80-\xFF]*                         { // keep utf8 characters together...
                                          yyextra->yyColNr+=yyleng;
                                          yyextra->code->codify(yytext);
                                        }
<*>.                                    {
                                          yyextra->yyColNr++;
                                          yyextra->code->codify(yytext);
                                        }
  /*
<*>([ \t\n]*"\n"){2,}                   { // combine multiple blank lines
                                          //QCString sepLine=yytext;
                                          //yyextra->code->codify("\n\n");
                                          //yyextra->yyLineNr+=sepLine.contains('\n');
                                          //char sepLine[3]="\n\n";
                                          codifyLines(yyscanner,yytext);
                                        }
  */

%%

/*@ ----------------------------------------------------------------------------
 */

static bool startsWithKeyword(const QCString &str,const QCString &kw)
{
  if (str.length()<kw.length()) return false;                // string too short to match
  return str==kw ||                                          // exact match
         (str.startsWith(kw) && !isId(str.at(kw.length()))); // match that is not a substring
}

static void addVariable(yyscan_t yyscanner,QCString type,QCString name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"VariableContext::addVariable(%s,%s)\n",qPrint(type),qPrint(name)));
  QCString ltype = type.simplifyWhiteSpace();
  QCString lname = name.simplifyWhiteSpace();
  if (ltype.left(7)=="struct ")
  {
    ltype = ltype.right(ltype.length()-7);
  }
  else if (ltype.left(6)=="union ")
  {
    ltype = ltype.right(ltype.length()-6);
  }
  if (ltype.isEmpty() || lname.isEmpty()) return;
  ltype = substitute(ltype,".","::");
  DBG_CTX((stderr,"** addVariable trying: type='%s' name='%s' currentDefinition=%s\n",
        qPrint(ltype),qPrint(lname),yyextra->currentDefinition?qPrint(yyextra->currentDefinition->name()):"<none>"));
  auto it = yyextra->codeClassMap.find(ltype.str());
  if (it!=yyextra->codeClassMap.end()) // look for class definitions inside the code block
  {
    DBG_CTX((stderr,"** addVariable type='%s' name='%s'\n",qPrint(ltype),qPrint(lname)));
    yyextra->theVarContext.addVariable(lname,std::move(it->second)); // add it to a list
  }
  else
  {
    auto findVariableType = [&yyscanner,&yyg,&ltype,&lname,&name](const Definition *d) -> const ClassDef *
    {
      const ClassDef *varDef = yyextra->symbolResolver.resolveClass(d,ltype);
      int i=0;
      if (varDef)
      {
        DBG_CTX((stderr,"** addVariable type='%s' name='%s'\n",qPrint(ltype),qPrint(lname)));
        yyextra->theVarContext.addVariable(lname,ScopedTypeVariant(varDef)); // add it to a list
      }
      else if ((i=ltype.find('<'))!=-1)
      {
        // probably a template class
        QCString typeName(ltype.left(i));
        addVariable(yyscanner,typeName,name);
      }
      return varDef;
    };
    const ClassDef *varDef = findVariableType(yyextra->currentDefinition);
    if (varDef==0) // also check via using directive
    {
      for (const auto &kv : yyextra->theUsingContext)
      {
        varDef = findVariableType(kv.second);
        if (varDef!=0) break;
      }
    }
    if (varDef==0)
    {
      if (!yyextra->theVarContext.atGlobalScope()) // for local variables add a dummy entry so the name
        // is hidden to avoid false links to global variables with the same name
        // TODO: make this work for namespaces as well!
      {
        DBG_CTX((stderr,"** addVariable: dummy context for '%s'\n",qPrint(lname)));
        yyextra->theVarContext.addVariable(lname,ScopedTypeVariant());
      }
      else
      {
        DBG_CTX((stderr,"** addVariable: not adding variable!\n"));
      }
    }
  }
}

//-------------------------------------------------------------------

/*! add class/namespace name s to the scope */
static void pushScope(yyscan_t yyscanner,const QCString &s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->classScopeLengthStack.push(int(yyextra->classScope.length()));
  if (yyextra->classScope.isEmpty() || leftScopeMatch(s,yyextra->classScope))
  {
    yyextra->classScope = s;
  }
  else
  {
    yyextra->classScope += "::";
    yyextra->classScope += s;
  }
  DBG_CTX((stderr,"pushScope(%s) result: '%s'\n",qPrint(s),qPrint(yyextra->classScope)));
}


/*! remove the top class/namespace name from the scope */
static void popScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->classScopeLengthStack.empty())
  {
    int length = yyextra->classScopeLengthStack.top();
    yyextra->classScopeLengthStack.pop();
    yyextra->classScope.truncate(length);
  }
  else
  {
    //err("Too many end of scopes found!\n");
  }
  DBG_CTX((stderr,"popScope() result: '%s'\n",qPrint(yyextra->classScope)));
}

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    std::lock_guard<std::mutex> lock(g_searchIndexMutex);
    if (yyextra->searchCtx)
    {
      yyextra->code->setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),FALSE);
    }
    else
    {
      yyextra->code->setCurrentDoc(yyextra->sourceFileDef,anchor,TRUE);
    }
  }
}

static void addToSearchIndex(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    std::lock_guard<std::mutex> lock(g_searchIndexMutex);
    yyextra->code->addWord(text,FALSE);
  }
}

static void addToSearchIndex(yyscan_t yyscanner,const char *text)
{
  addToSearchIndex(yyscanner,QCString(text));
}


static void setClassScope(yyscan_t yyscanner,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"setClassScope(%s)\n",qPrint(name)));
  QCString n=name;
  n=n.simplifyWhiteSpace();
  int ts=n.find('<'); // start of template
  int te=n.findRev('>'); // end of template
  DBG_CTX((stderr,"ts=%d te=%d\n",ts,te));
  if (ts!=-1 && te!=-1 && te>ts)
  {
    // remove template from scope
    n=n.left(ts)+n.right(n.length()-te-1);
  }
  while (!yyextra->classScopeLengthStack.empty())
  {
    popScope(yyscanner);
  }
  yyextra->classScope.resize(0);
  int i;
  while ((i=n.find("::"))!=-1)
  {
    pushScope(yyscanner,n.left(i));
    n = n.mid(i+2);
  }
  pushScope(yyscanner,n);
  DBG_CTX((stderr,"--->New class scope '%s'\n",qPrint(yyextra->classScope)));
}

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //if (yyextra->currentFontClass) { yyextra->code->endFontClass(yyscanner); }
  if (yyextra->sourceFileDef && yyextra->lineNumbers)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",yyextra->yyLineNr);
    //lineAnchor.sprintf("l%05d",yyextra->yyLineNr);

    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    DBG_CTX((stderr,"%s:startCodeLine(%d)=%p\n",qPrint(yyextra->sourceFileDef->name()),yyextra->yyLineNr,(void*)d));
    if (!yyextra->includeCodeFragment && d)
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->insideBody = FALSE;
      yyextra->searchingForBody = TRUE;
      yyextra->realScope = d->name();
      //yyextra->classScope = "";
      yyextra->type.resize(0);
      yyextra->name.resize(0);
      yyextra->args.resize(0);
      yyextra->parmType.resize(0);
      yyextra->parmName.resize(0);
      DBG_CTX((stderr,"Real scope: '%s'\n",qPrint(yyextra->realScope)));
      yyextra->bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),
                                yyextra->yyLineNr,!yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        yyextra->code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                QCString(),yyextra->yyLineNr,!yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr,
                                     !yyextra->includeCodeFragment);
    }
  }
  DBG_CTX((stderr,"startCodeLine(%d)\n",yyextra->yyLineNr));
  yyextra->code->startCodeLine(yyextra->sourceFileDef && yyextra->lineNumbers);
  yyextra->insideCodeLine = true;
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(QCString(yyextra->currentFontClass));
  }
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

/*! write a code fragment 'text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,qPrint(text)));
  if (text.isEmpty()) return;
  const char *p=text.data(),*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { yyextra->yyColNr++; }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      yyextra->yyColNr=1;
      int l = (int)(p-sp-1);
      char *tmp = (char*)malloc(l+1);
      memcpy(tmp,sp,l);
      tmp[l]='\0';
      yyextra->code->codify(QCString(tmp));
      free(tmp);
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(QCString(sp));
      done=TRUE;
    }
  }
}

static void codifyLines(yyscan_t yyscanner,const char *text)
{
  codifyLines(yyscanner,QCString(text));
}

static void incrementFlowKeyWordCount(yyscan_t yyscanner)
{
  std::lock_guard<std::mutex> lock(g_countFlowKeywordsMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
  {
    MemberDefMutable *md = toMemberDefMutable(yyextra->currentMemberDef);
    if (md)
    {
      md->incrementFlowKeyWordCount();
    }
  }
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                                   const Definition *d,
                                   const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  yyextra->tooltipManager.addTooltip(ol,d);
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
    char c;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      DBG_CTX((stderr,"writeCodeLink(%s,%s,%s,%s)\n",qPrint(ref),qPrint(file),qPrint(anchor),qPrint(QCString(sp,p-sp-1))));
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,QCString(sp,p-sp-1),tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      DBG_CTX((stderr,"writeCodeLink(%s,%s,%s,%s)\n",qPrint(ref),qPrint(file),qPrint(anchor),sp));
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,QCString(sp),tooltip);
      done=TRUE;
    }
  }
}

static void addType(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->name=="const") { yyextra->name.resize(0); return; }
  if (!yyextra->type.isEmpty()) yyextra->type += ' ' ;
  yyextra->type += yyextra->name ;
  yyextra->name.resize(0) ;
  if (!yyextra->type.isEmpty()) yyextra->type += ' ' ;
  yyextra->type += yyextra->args ;
  yyextra->args.resize(0) ;
}

static void addParmType(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->parmName=="const") { yyextra->parmName.resize(0); return; }
  if (!yyextra->parmType.isEmpty()) yyextra->parmType += ' ' ;
  yyextra->parmType += yyextra->parmName ;
  yyextra->parmName.resize(0) ;
}

static void addUsingDirective(yyscan_t yyscanner,const QCString &name)
{
  //printf("AddUsingDirective(%s)\n",qPrint(name));
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->sourceFileDef && !name.isEmpty())
  {
    const NamespaceDef *nd = Doxygen::namespaceLinkedMap->find(name);
    if (nd)
    {
      yyextra->theUsingContext.insert(std::make_pair(name.str(),nd));
    }
  }
}

static void setParameterList(yyscan_t yyscanner,const MemberDef *md)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->classScope = md->getClassDef() ? md->getClassDef()->name() : QCString();
  for (const Argument &a : md->argumentList())
  {
    yyextra->parmName = a.name;
    yyextra->parmType = a.type;
    int i = yyextra->parmType.find('*');
    if (i!=-1) yyextra->parmType = yyextra->parmType.left(i);
    i = yyextra->parmType.find('&');
    if (i!=-1) yyextra->parmType = yyextra->parmType.left(i);
    yyextra->parmType.stripPrefix("const ");
    yyextra->parmType=yyextra->parmType.stripWhiteSpace();
    addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
  }
}

static const ClassDef *stripClassName(yyscan_t yyscanner,const QCString &s,const Definition *d)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int pos=0;
  QCString type = s;
  QCString className;
  QCString templSpec;
  while (extractClassNameFromType(type,pos,className,templSpec)!=-1)
  {
    QCString clName=className+templSpec;
    const ClassDef *cd=0;
    if (!yyextra->classScope.isEmpty())
    {
      cd=yyextra->symbolResolver.resolveClass(d,yyextra->classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=yyextra->symbolResolver.resolveClass(d,clName);
    }
    DBG_CTX((stderr,"stripClass trying '%s' = %p\n",qPrint(clName),(void*)cd));
    if (cd)
    {
      return cd;
    }
  }

  return 0;
}

static const MemberDef *setCallContextForVar(yyscan_t yyscanner,const QCString &name)
{
  if (name.isEmpty()) return 0;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"setCallContextForVar(%s) yyextra->classScope=%s\n",qPrint(name),qPrint(yyextra->classScope)));

  int scopeEnd = name.findRev("::");
  if (scopeEnd!=-1) // name with explicit scope
  {
    QCString scope   = name.left(scopeEnd);
    QCString locName = name.right(name.length()-scopeEnd-2);
    DBG_CTX((stderr,"explicit scope: name=%s scope=%s\n",qPrint(locName),qPrint(scope)));
    const ClassDef *mcd = getClass(scope);
    if (mcd && !locName.isEmpty())
    {
      const MemberDef *md=mcd->getMemberByName(locName);
      if (md)
      {
        DBG_CTX((stderr,"name=%s scope=%s\n",qPrint(locName),qPrint(scope)));
        yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
        return md;
      }
    }
    else // check namespace as well
    {
      const NamespaceDef *mnd = getResolvedNamespace(scope);
      if (mnd && !locName.isEmpty())
      {
        const MemberDef *md=mnd->getMemberByName(locName);
        if (md)
        {
          DBG_CTX((stderr,"name=%s scope=%s\n",qPrint(locName),qPrint(scope)));
          yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
          return md;
        }
      }
    }
  }

  const MemberName *mn;
  const ScopedTypeVariant *mcv = yyextra->theVarContext.findVariable(name);
  if (mcv)
  {
    DBG_CTX((stderr,"local variable?\n"));
    if (mcv->type()!=ScopedTypeVariant::Dummy) // locally found variable
    {
      DBG_CTX((stderr,"local var '%s' mcd=%s\n",qPrint(name),qPrint(mcv->name())));
      yyextra->theCallContext.setScope(*mcv);
    }
  }
  else
  {
    DBG_CTX((stderr,"class member? scope=%s\n",qPrint(yyextra->classScope)));
    // look for a class member
    const ClassDef *mcd = getClass(yyextra->classScope);
    if (mcd)
    {
      DBG_CTX((stderr,"Inside class %s\n",qPrint(mcd->name())));
      const MemberDef *md=mcd->getMemberByName(name);
      if (md)
      {
        DBG_CTX((stderr,"Found member %s\n",qPrint(md->name())));
        if (yyextra->scopeStack.empty() || yyextra->scopeStack.top()!=CLASSBLOCK)
        {
          DBG_CTX((stderr,"class member '%s' mcd=%s\n",qPrint(name),qPrint(mcd->name())));
          yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
        }
        return md;
      }
    }
  }

  // look for a global member
  if ((mn=Doxygen::functionNameLinkedMap->find(name)))
  {
    DBG_CTX((stderr,"global var '%s'\n",qPrint(name)));
    if (mn->size()==1) // global defined only once
    {
      const std::unique_ptr<MemberDef> &md=mn->front();
      if (!md->isStatic() || md->getBodyDef()==yyextra->sourceFileDef)
      {
        yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
        return md.get();
      }
      return 0;
    }
    else if (mn->size()>1) // global defined more than once
    {
      for (const auto &md : *mn)
      {
        //printf("mn=%p md=%p md->getBodyDef()=%p yyextra->sourceFileDef=%p\n",
        //    mn,md,
        //    md->getBodyDef(),yyextra->sourceFileDef);

        // in case there are multiple members we could link to, we
        // only link to members if defined in the same file or
        // defined as external.
        if (!md->isStatic() || md->getBodyDef()==yyextra->sourceFileDef)
        {
          yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
          DBG_CTX((stderr,"returning member %s in source file %s\n",qPrint(md->name()),qPrint(yyextra->sourceFileDef->name())));
          return md.get();
        }
      }
      return 0;
    }
  }
  return 0;
}

static void updateCallContextForSmartPointer(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const Definition *d = yyextra->theCallContext.getScope().globalDef();
  //printf("updateCallContextForSmartPointer() cd=%s\n",cd ? qPrint(d->name()) : "<none>");
  const MemberDef *md;
  if (d && d->definitionType()==Definition::TypeClass && (md=(toClassDef(d))->isSmartPointer()))
  {
    const ClassDef *ncd = stripClassName(yyscanner,md->typeString(),md->getOuterScope());
    if (ncd)
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(ncd));
      //printf("Found smart pointer call %s->%s!\n",qPrint(cd->name()),qPrint(ncd->name()));
    }
  }
}

static bool getLinkInScope(yyscan_t yyscanner,
                           const QCString &c,  // scope
                           const QCString &m,  // member
                           const QCString &memberText, // exact text
                           CodeOutputInterface &ol,
                           const QCString &text,
                           bool varOnly
                          )
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const MemberDef    *md = 0;
  const ClassDef     *cd = 0;
  const FileDef      *fd = 0;
  const NamespaceDef *nd = 0;
  const GroupDef     *gd = 0;
  DBG_CTX((stderr,"getLinkInScope: trying '%s'::'%s' varOnly=%d\n",qPrint(c),qPrint(m),varOnly));
  if (getDefs(c,m,"()",md,cd,fd,nd,gd,FALSE,yyextra->sourceFileDef,FALSE) &&
      (!varOnly || md->isVariable()))
  {
    if (md->isLinkable())
    {
      DBG_CTX((stderr,"found it %s!\n",qPrint(md->qualifiedName())));
      if (yyextra->exampleBlock)
      {
        std::lock_guard<std::mutex> lock(g_addExampleMutex);
        QCString anchor;
        anchor.sprintf("a%d",yyextra->anchorCount);
        DBG_CTX((stderr,"addExampleFile(%s,%s,%s)\n",qPrint(anchor),qPrint(yyextra->exampleName),
                                                     qPrint(yyextra->exampleFile)));
        MemberDefMutable *mdm = toMemberDefMutable(md);
        if (mdm && mdm->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
        {
          ol.writeCodeAnchor(anchor);
          yyextra->anchorCount++;
        }
      }

      const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                            md->resolveAlias()->getFileDef() : md->getOuterScope();
      if (md->resolveAlias()->getGroupDef()) d = md->resolveAlias()->getGroupDef();
      if (d && d->isLinkable())
      {
        yyextra->theCallContext.setScope(ScopedTypeVariant(stripClassName(yyscanner,md->typeString(),md->getOuterScope())));
        DBG_CTX((stderr,"yyextra->currentDefinition=%p yyextra->currentMemberDef=%p yyextra->insideBody=%d\n",
                 (void*)yyextra->currentDefinition,(void*)yyextra->currentMemberDef,yyextra->insideBody));

        if (yyextra->currentDefinition && yyextra->currentMemberDef &&
            yyextra->insideBody && yyextra->collectXRefs)
        {
          std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
          addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(md));
        }
        DBG_CTX((stderr,"d->getReference()='%s' d->getOutputBase()='%s' name='%s' member name='%s'\n",qPrint(d->getReference()),qPrint(d->getOutputFileBase()),qPrint(d->name()),qPrint(md->name())));

        writeMultiLineCodeLink(yyscanner,ol,md, !text.isEmpty() ? text : memberText);
        addToSearchIndex(yyscanner,!text.isEmpty() ? text : memberText);
        return TRUE;
      }
    }
    else // found member, but it is not linkable, so make sure content inside is not assigned
         // to the previous member, see bug762760
    {
      DBG_CTX((stderr,"unlinkable member %s\n",qPrint(md->name())));
      yyextra->currentMemberDef = 0;
    }
  }
  return FALSE;
}

static bool getLink(yyscan_t yyscanner,
                    const QCString &className,
                    const QCString &memberName,
                    CodeOutputInterface &ol,
                    const QCString &text,
                    bool varOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"getLink(%s,%s) yyextra->curClassName=%s\n",
        qPrint(className),qPrint(memberName),qPrint(yyextra->curClassName)));
  QCString m=removeRedundantWhiteSpace(memberName);
  QCString c=className;
  if (!getLinkInScope(yyscanner,c,m,memberName,ol,text,varOnly))
  {
    if (!yyextra->curClassName.isEmpty())
    {
      if (!c.isEmpty()) c.prepend("::");
      c.prepend(yyextra->curClassName);
      return getLinkInScope(yyscanner,c,m,memberName,ol,text,varOnly);
    }
    return FALSE;
  }
  return TRUE;
}

static void generateClassOrGlobalLink(yyscan_t yyscanner,
                                      CodeOutputInterface &ol,
                                      const QCString &clName,
                                      bool typeOnly,
                                      bool varOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString className=clName;
  if (!className.isEmpty() && className[0]=='~') // correct for matching negated values i.s.o. destructors.
  {
    className=className.mid(1);
  }
  if (className.isEmpty())
  {
    yyextra->code->codify("~");
    return;
  }
  if (yyextra->insideProtocolList) // for Obj-C
  {
    className+="-p";
  }
  if (yyextra->lang==SrcLangExt_PHP)
  {
    className = substitute(className,"\\","::"); // for PHP namespaces
  }
  else if (yyextra->lang==SrcLangExt_CSharp || yyextra->lang==SrcLangExt_Java)
  {
    className = substitute(className,".","::"); // for C#/Java namespaces
  }
  const ScopedTypeVariant *lcd=0;
  const ClassDef *cd=0;
  const MemberDef *md=0;
  bool isLocal=FALSE;

  DBG_CTX((stderr,"generateClassOrGlobalLink(className=%s)\n",qPrint(className)));
  if (!yyextra->isPrefixedWithThis || (lcd=yyextra->theVarContext.findVariable(className))==0) // not a local variable
  {
    int i=className.find('<');
    QCString bareName = className;
    if (i!=-1) bareName = bareName.left(i);

    auto checkForClass = [&yyg,&bareName,&className](const Definition *d,
                                                     const ClassDef *&cd_,const MemberDef *&md_)
    {
      cd_ = yyextra->symbolResolver.resolveClass(d,className);
      md_ = yyextra->symbolResolver.getTypedef();
      DBG_CTX((stderr,"non-local variable name=%s cd=%s md=%s!\n",
            qPrint(className),cd_?qPrint(cd_->name()):"<none>",
            md_?qPrint(md_->name()):"<none>"));
      if (cd_==0 && md_==0 && !bareName.isEmpty())
      {
        DBG_CTX((stderr,"bareName=%s\n",qPrint(bareName)));
        if (bareName!=className)
        {
          cd_ = yyextra->symbolResolver.resolveClass(d,bareName); // try unspecialized version
          md_ = yyextra->symbolResolver.getTypedef();
        }
      }
    };
    const Definition *d = yyextra->currentDefinition;
    DBG_CTX((stderr,"d=%s yyextra->sourceFileDef=%s\n",d?qPrint(d->name()):"<none>",yyextra->sourceFileDef?qPrint(yyextra->sourceFileDef->name()):"<none>"));
    checkForClass(d,cd,md);
    if (cd==0 && md==0 && d && d->definitionType()==Definition::TypeClass)
    {
      const FileDef *fd = toClassDef(d)->getFileDef();
      if (fd)
      {
        // also check for using directive in the file that defines this class
        for (const auto &nd : fd->getUsedNamespaces())
        {
          checkForClass(nd,cd,md);
          if (cd!=0 || md!=0) break;
        }
      }
    }
    if (cd==0 && md==0)
    {
      // also check via using directive
      for (const auto &kv : yyextra->theUsingContext)
      {
        checkForClass(kv.second,cd,md);
        if (cd!=0 || md!=0) break;
      }
    }

    const NamespaceDef *nd = getResolvedNamespace(className);
    if (nd && nd->isLinkable())
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(nd));
      addToSearchIndex(yyscanner,className);
      writeMultiLineCodeLink(yyscanner,*yyextra->code,nd,clName);
      return;
    }
    const ConceptDef *conceptDef = getResolvedConcept(d,bareName);
    if (conceptDef && conceptDef->isLinkable())
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(conceptDef));
      addToSearchIndex(yyscanner,className);
      writeMultiLineCodeLink(yyscanner,*yyextra->code,conceptDef,clName);
      return;
    }
    DBG_CTX((stderr,"md=%s\n",md?qPrint(md->name()):"<none>"));
    DBG_CTX((stderr,"is found as a type cd=%s nd=%s\n",
          cd?qPrint(cd->name()):"<null>",
          nd?qPrint(nd->name()):"<null>"));
    if (cd==0 && md==0) // also see if it is variable or enum or enum value
    {
      if (getLink(yyscanner,yyextra->classScope,clName,ol,clName,varOnly))
      {
        return;
      }
    }
  }
  else
  {
    DBG_CTX((stderr,"local variable!\n"));
    if (lcd->type()!=ScopedTypeVariant::Dummy)
    {
      DBG_CTX((stderr,"non-dummy context lcd=%s!\n",qPrint(lcd->name())));
      yyextra->theCallContext.setScope(*lcd);

      // to following is needed for links to a global variable, but is
      // no good for a link to a local variable that is also a global symbol.

      //if (getLink(yyscanner,yyextra->classScope,clName,ol,clName))
      //{
        //return;
      //}
    }
    isLocal=TRUE;
    DBG_CTX((stderr,"is a local variable cd=%p!\n",(void*)cd));
  }
  yyextra->isPrefixedWithThis = FALSE; // discard the "this" prefix for the next calls

  if (cd && cd->isLinkable()) // is it a linkable class
  {
    DBG_CTX((stderr,"is linkable class %s\n",qPrint(clName)));
    if (yyextra->exampleBlock)
    {
      std::lock_guard<std::mutex> lock(g_addExampleMutex);
      QCString anchor;
      anchor.sprintf("_a%d",yyextra->anchorCount);
      DBG_CTX((stderr,"addExampleClass(%s,%s,%s)\n",qPrint(anchor),qPrint(yyextra->exampleName),
                                                    qPrint(yyextra->exampleFile)));
      ClassDefMutable *cdm = toClassDefMutable(const_cast<ClassDef*>(cd));
      if (cdm && cdm->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
      {
        ol.writeCodeAnchor(anchor);
        yyextra->anchorCount++;
      }
    }
    writeMultiLineCodeLink(yyscanner,ol,cd,clName);
    addToSearchIndex(yyscanner,className);
    yyextra->theCallContext.setScope(ScopedTypeVariant(cd));
    if (md)
    {
      const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                      md->getFileDef() : md->getOuterScope();
      if (md->getGroupDef()) d = md->getGroupDef();
      if (d && d->isLinkable() && md->isLinkable() &&
          yyextra->currentMemberDef && yyextra->collectXRefs)
      {
        std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
        addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(md));
      }
    }
  }
  else // not a class, maybe a global member
  {
    DBG_CTX((stderr,"class %s not linkable! cd=%p md=%p typeOnly=%d\n",qPrint(clName),(void*)cd,(void*)md,typeOnly));
    if (!isLocal && (md!=0 || (cd==0 && !typeOnly))) // not a class, see if it is a global enum/variable/typedef.
    {
      if (md==0) // not found as a typedef
      {
        md = setCallContextForVar(yyscanner,clName);
        DBG_CTX((stderr,"setCallContextForVar(%s) md=%p yyextra->currentDefinition=%s\n",qPrint(clName),(void*)md,yyextra->currentDefinition ? qPrint(yyextra->currentDefinition->name()) : "<none>"));
        if (md && yyextra->currentDefinition)
        {
          DBG_CTX((stderr,"%s accessible from %s? %d md->getOuterScope=%s\n",
              qPrint(md->name()),qPrint(yyextra->currentDefinition->name()),
              yyextra->symbolResolver.isAccessibleFrom(yyextra->currentDefinition,md),
              qPrint(md->getOuterScope()->name())));
        }

        if (md && yyextra->currentDefinition &&
            yyextra->symbolResolver.isAccessibleFrom(yyextra->currentDefinition,md)==-1)
        {
          md=0; // variable not accessible
        }
      }
      if (md && (!varOnly || md->isVariable()))
      {
        DBG_CTX((stderr,"is a global md=%p yyextra->currentDefinition=%s linkable=%d\n",(void*)md,yyextra->currentDefinition?qPrint(yyextra->currentDefinition->name()):"<none>",md->isLinkable()));
        if (md->isLinkable())
        {
          writeMultiLineCodeLink(yyscanner,ol,md,clName);
          addToSearchIndex(yyscanner,clName);
          if (yyextra->currentMemberDef && yyextra->collectXRefs)
          {
            std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
            addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(md));
          }
          return;
        }
      }
    }

    // nothing found, just write out the word
    DBG_CTX((stderr,"not found!\n"));
    codifyLines(yyscanner,clName);
    addToSearchIndex(yyscanner,clName);
  }
}

static void generateClassOrGlobalLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *clName,
                                      bool typeOnly,bool varOnly)
{
  generateClassOrGlobalLink(yyscanner,ol,QCString(clName),typeOnly,varOnly);
}

static bool generateClassMemberLink(yyscan_t yyscanner,
                                    CodeOutputInterface &ol,
                                    const MemberDef *xmd,
                                    const QCString &memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // extract class definition of the return type in order to resolve
  // a->b()->c() like call chains

  DBG_CTX((stderr,"type='%s' args='%s' class=%s\n",
           qPrint(xmd->typeString()),qPrint(xmd->argsString()),
           qPrint(xmd->getClassDef()->name())));

  if (yyextra->exampleBlock)
  {
    std::lock_guard<std::mutex> lock(g_addExampleMutex);
    QCString anchor;
    anchor.sprintf("a%d",yyextra->anchorCount);
    DBG_CTX((stderr,"addExampleFile(%s,%s,%s)\n",qPrint(anchor),qPrint(yyextra->exampleName),
                                                 qPrint(yyextra->exampleFile)));
    MemberDefMutable *mdm = toMemberDefMutable(xmd);
    if (mdm && mdm->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
    {
      ol.writeCodeAnchor(anchor);
      yyextra->anchorCount++;
    }
  }

  const ClassDef *typeClass = stripClassName(yyscanner,removeAnonymousScopes(xmd->typeString()),xmd->getOuterScope());
  DBG_CTX((stderr,"%s -> typeName=%p\n",qPrint(xmd->typeString()),(void*)typeClass));
  yyextra->theCallContext.setScope(ScopedTypeVariant(typeClass));

  const Definition *xd = xmd->getOuterScope()==Doxygen::globalScope ?
                   xmd->getFileDef() : xmd->getOuterScope();
  if (xmd->getGroupDef()) xd = xmd->getGroupDef();
  if (xd && xd->isLinkable())
  {

    DBG_CTX((stderr,"yyextra->currentDefinition=%p yyextra->currentMemberDef=%p xmd=%p yyextra->insideBody=%d\n",
          (void*)yyextra->currentDefinition,(void*)yyextra->currentMemberDef,(void*)xmd,yyextra->insideBody));

    if (xmd->templateMaster()) xmd = xmd->templateMaster();

    if (xmd->isLinkable())
    {
      // add usage reference
      if (yyextra->currentDefinition && yyextra->currentMemberDef &&
          yyextra->insideBody && yyextra->collectXRefs)
      {
        std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
        addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(xmd));
      }

      // write the actual link
      writeMultiLineCodeLink(yyscanner,ol,xmd,memName);
      addToSearchIndex(yyscanner,memName);
      return TRUE;
    }
  }

  return FALSE;
}

static bool generateClassMemberLink(yyscan_t yyscanner,
                                    CodeOutputInterface &ol,
                                    const Definition *def,
                                    const QCString &memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (def && def->definitionType()==Definition::TypeClass)
  {
    const ClassDef *cd = toClassDef(def);
    const MemberDef *xmd = cd->getMemberByName(memName);
    DBG_CTX((stderr,"generateClassMemberLink(class=%s,member=%s)=%p\n",qPrint(def->name()),qPrint(memName),(void*)xmd));
    if (xmd)
    {
      return generateClassMemberLink(yyscanner,ol,xmd,memName);
    }
    else
    {
      const Definition *innerDef = cd->findInnerCompound(memName);
      if (innerDef)
      {
        yyextra->theCallContext.setScope(ScopedTypeVariant(innerDef));
        addToSearchIndex(yyscanner,memName);
        writeMultiLineCodeLink(yyscanner,*yyextra->code,innerDef,memName);
        return TRUE;
      }
    }
  }
  else if (def && def->definitionType()==Definition::TypeNamespace)
  {
    const NamespaceDef *nd = toNamespaceDef(def);
    DBG_CTX((stderr,"Looking for %s inside namespace %s\n",qPrint(memName),qPrint(nd->name())));
    const Definition *innerDef = nd->findInnerCompound(memName);
    if (innerDef)
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(innerDef));
      addToSearchIndex(yyscanner,memName);
      writeMultiLineCodeLink(yyscanner,*yyextra->code,innerDef,memName);
      return TRUE;
    }
  }
  return FALSE;
}

static void generateMemberLink(yyscan_t yyscanner,
                               CodeOutputInterface &ol,
                               const QCString &varName,
                               const QCString &memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"generateMemberLink(object=%s,mem=%s) classScope=%s\n",
           qPrint(varName),qPrint(memName),qPrint(yyextra->classScope)));

  if (varName.isEmpty()) return;

  // look for the variable in the current context
  const ScopedTypeVariant *stv = yyextra->theVarContext.findVariable(varName);
  if (stv)
  {
    if (stv->type()!=ScopedTypeVariant::Dummy)
    {
      DBG_CTX((stderr,"Class found!\n"));
      if (getLink(yyscanner,stv->name(),memName,ol))
      {
        DBG_CTX((stderr,"Found result!\n"));
        return;
      }
      if (stv->localDef() && !stv->localDef()->baseClasses().empty())
      {
        for (const auto &bcName : stv->localDef()->baseClasses())
        {
          if (getLink(yyscanner,bcName,memName,ol))
          {
            DBG_CTX((stderr,"Found result!\n"));
            return;
          }
        }
      }
    }
  }
  else // variable not in current context, maybe it is in a parent context
  {
    const ClassDef *vcd = yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,yyextra->classScope);
    if (vcd && vcd->isLinkable())
    {
      DBG_CTX((stderr,"Found class %s for variable '%s'\n",qPrint(yyextra->classScope),qPrint(varName)));
      MemberName *vmn=Doxygen::memberNameLinkedMap->find(varName);
      if (vmn==0)
      {
        int vi;
        QCString vn=varName;
        if ((vi=vn.findRev("::"))!=-1 || (vi=vn.findRev('.'))!=-1)  // explicit scope A::b(), probably static member
        {
          const ClassDef *jcd = getClass(vn.left(vi));
          vn=vn.right(vn.length()-vi-2);
          vmn=Doxygen::memberNameLinkedMap->find(vn);
          //printf("Trying name '%s' scope=%s\n",qPrint(vn),qPrint(scope));
          if (vmn)
          {
            for (const auto &vmd : *vmn)
            {
              if (vmd->getClassDef()==jcd)
              {
                DBG_CTX((stderr,"Found variable type=%s\n",qPrint(vmd->typeString())));
                const ClassDef *mcd=stripClassName(yyscanner,vmd->typeString(),vmd->getOuterScope());
                if (mcd && mcd->isLinkable())
                {
                  if (generateClassMemberLink(yyscanner,ol,mcd,memName)) return;
                }
              }
            }
          }
        }
      }
      if (vmn)
      {
        DBG_CTX((stderr,"There is a variable with name '%s'\n",qPrint(varName)));
        for (const auto &vmd : *vmn)
        {
          if (vmd->getClassDef()==vcd)
          {
            DBG_CTX((stderr,"Found variable type=%s\n",qPrint(vmd->typeString())));
            const ClassDef *mcd=stripClassName(yyscanner,vmd->typeString(),vmd->getOuterScope());
            if (mcd && mcd->isLinkable())
            {
              if (generateClassMemberLink(yyscanner,ol,mcd,memName)) return;
            }
          }
        }
      }
    }
  }
  // nothing found -> write result as is
  codifyLines(yyscanner,memName);
  addToSearchIndex(yyscanner,memName);
  return;
}

static void generatePHPVariableLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *varName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString name(varName+7); // strip $this->
  name.prepend("$");
  DBG_CTX((stderr,"generatePHPVariableLink(%s) name=%s scope=%s\n",varName,qPrint(name),qPrint(yyextra->classScope)));
  if (!getLink(yyscanner,yyextra->classScope,name,ol,QCString(varName)))
  {
    codifyLines(yyscanner,varName);
  }
}

static void generateFunctionLink(yyscan_t yyscanner,CodeOutputInterface &ol,const QCString &funcName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //CodeClassDef *ccd=0;
  QCString locScope=yyextra->classScope;
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  if (yyextra->lang==SrcLangExt_PHP && locFunc.startsWith("self::")) locFunc=locFunc.mid(4);
  QCString funcScope;
  QCString funcWithScope=locFunc;
  QCString funcWithFullScope=locFunc;
  QCString fullScope=locScope;
  DBG_CTX((stderr,"*** locScope=%s locFunc=%s\n",qPrint(locScope),qPrint(locFunc)));
  int len=2;
  int i=locFunc.findRev("::");
  if (yyextra->currentMemberDef && yyextra->currentMemberDef->resolveAlias()->getClassDef() &&
      funcName==yyextra->currentMemberDef->localName() &&
      yyextra->currentMemberDef->getDefLine()==yyextra->yyLineNr &&
      generateClassMemberLink(yyscanner,ol,yyextra->currentMemberDef,funcName)
     )
  {
    // special case where funcName is the name of a method that is also
    // defined on this line. In this case we can directly link to
    // yyextra->currentMemberDef, which is not only faster, but
    // in case of overloaded methods, this will make sure that we link to
    // the correct method, and thereby get the correct reimplemented relations.
    // See also bug 549022.
    goto exit;
  }
  if (i==-1) i=locFunc.findRev("."),len=1;
  if (i==-1) i=locFunc.findRev("\\"),len=1; // for PHP
  if (i>0)
  {
    funcScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-len).stripWhiteSpace();
    int ts=locScope.find('<'); // start of template
    int te=locScope.findRev('>'); // end of template
    DBG_CTX((stderr,"ts=%d te=%d\n",ts,te));
    if (ts!=-1 && te!=-1 && te>ts)
    {
      // remove template from scope
      locScope=locScope.left(ts)+locScope.right(locScope.length()-te-1);
    }
    ts=funcScope.find('<'); // start of template
    te=funcScope.findRev('>'); // end of template
    DBG_CTX((stderr,"ts=%d te=%d\n",ts,te));
    if (ts!=-1 && te!=-1 && te>ts)
    {
      // remove template from scope
      funcScope=funcScope.left(ts)+funcScope.right(funcScope.length()-te-1);
    }
    if (!funcScope.isEmpty())
    {
      funcWithScope = funcScope+"::"+locFunc;
      if (!locScope.isEmpty())
      {
        fullScope=locScope+"::"+funcScope;
      }
    }
    if (!locScope.isEmpty())
    {
      funcWithFullScope = locScope+"::"+funcWithScope;
    }
  }

  if (!fullScope.isEmpty())
  {
    auto it = yyextra->codeClassMap.find(fullScope.str());
    if (it!=yyextra->codeClassMap.end())
    {
      ScopedTypeVariant ccd = it->second;
      if (ccd.localDef() && !ccd.localDef()->baseClasses().empty())
      {
        for (const auto &bcName : ccd.localDef()->baseClasses())
        {
          if (getLink(yyscanner,bcName,locFunc,ol,funcName))
          {
            goto exit;
          }
        }
      }
    }
  }

  if (!locScope.isEmpty() && fullScope!=locScope)
  {
    auto it = yyextra->codeClassMap.find(locScope.str());
    if (it!=yyextra->codeClassMap.end())
    {
      ScopedTypeVariant ccd = it->second;
      if (ccd.localDef() && !ccd.localDef()->baseClasses().empty())
      {
        for (const auto &bcName : ccd.localDef()->baseClasses())
        {
          if (getLink(yyscanner,bcName,funcWithScope,ol,funcName))
          {
            goto exit;
          }
        }
      }
    }
  }
  if (!getLink(yyscanner,locScope,funcWithScope,ol,funcName))
  {
    generateClassOrGlobalLink(yyscanner,ol,funcName);
  }
exit:
  return;
}

static void generateFunctionLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *funcName)
{
  generateFunctionLink(yyscanner,ol,QCString(funcName));
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
    count++;
  }
  return count;
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

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->startFontClass(QCString(s));
  yyextra->currentFontClass=s;
}

//----------------------------------------------------------------------------

// recursively writes a linkified Objective-C method call
static void writeObjCMethodCall(yyscan_t yyscanner,ObjCCallCtx *ctx)
{
  if (ctx==0) return;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  char c;
  if (!ctx->methodName.isEmpty())
  {
    DBG_CTX((stderr,"writeObjCMethodCall(%s) obj=%s method=%s\n",
             qPrint(ctx->format),qPrint(ctx->objectTypeOrName),qPrint(ctx->methodName)));
    if (!ctx->objectTypeOrName.isEmpty() && ctx->objectTypeOrName.at(0)!='$')
    {
      DBG_CTX((stderr,"Looking for object=%s method=%s\n",qPrint(ctx->objectTypeOrName),
               qPrint(ctx->methodName)));
      const ScopedTypeVariant *stv = yyextra->theVarContext.findVariable(ctx->objectTypeOrName);
      if (stv==0) // not a local variable
      {
        if (ctx->objectTypeOrName=="self")
        {
          if (yyextra->currentDefinition &&
              yyextra->currentDefinition->definitionType()==Definition::TypeClass)
          {
            ctx->objectType = toClassDef(yyextra->currentDefinition);
          }
        }
        else
        {
          ctx->objectType = yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,ctx->objectTypeOrName);
          ctx->method = yyextra->symbolResolver.getTypedef();
        }
        DBG_CTX((stderr,"  object is class? %p\n",(void*)ctx->objectType));
        if (ctx->objectType) // found class
        {
          ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
          DBG_CTX((stderr,"    yes->method=%s\n",ctx->method?qPrint(ctx->method->name()):"<none>"));
        }
        else if (ctx->method==0) // search for class variable with the same name
        {
          DBG_CTX((stderr,"    no\n"));
          DBG_CTX((stderr,"yyextra->currentDefinition=%p\n",(void*)yyextra->currentDefinition));
          if (yyextra->currentDefinition &&
              yyextra->currentDefinition->definitionType()==Definition::TypeClass)
          {
            ctx->objectVar = (toClassDef(yyextra->currentDefinition))->getMemberByName(ctx->objectTypeOrName);
            DBG_CTX((stderr,"      ctx->objectVar=%p\n",(void*)ctx->objectVar));
            if (ctx->objectVar)
            {
              ctx->objectType = stripClassName(yyscanner,ctx->objectVar->typeString(),yyextra->currentDefinition);
              DBG_CTX((stderr,"        ctx->objectType=%p\n",(void*)ctx->objectType));
              if (ctx->objectType && !ctx->methodName.isEmpty())
              {
                ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
                DBG_CTX((stderr,"          ctx->method=%p\n",(void*)ctx->method));
              }
            }
          }
        }
      }
      else // local variable
      {
        DBG_CTX((stderr,"  object is local variable\n"));
        if (stv->globalDef() && !ctx->methodName.isEmpty())
        {
          const ClassDef *cd = toClassDef(stv->globalDef());
          if (cd)
          {
            ctx->method = cd->getMemberByName(ctx->methodName);
          }
          DBG_CTX((stderr,"   class=%p method=%p\n",(void*)cd,(void*)ctx->method));
        }
      }
    }
  }

  DBG_CTX((stderr,"["));
  if (!ctx->format.isEmpty())
  {
    const char *p = ctx->format.data();
    while ((c=*p++)) // for each character in ctx->format
    {
      if (c=='$')
      {
        char nc=*p++;
        if (nc=='$') // escaped $
        {
          yyextra->code->codify("$");
        }
        else // name fragment or reference to a nested call
        {
          if (nc=='n') // name fragment
          {
            nc=*p++;
            QCString refIdStr;
            while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
            p--;
            int refId=refIdStr.toInt();
            auto it = yyextra->nameMap.find(refId);
            if (it!=yyextra->nameMap.end())
            {
              QCString name = it->second;
              if (ctx->method && ctx->method->isLinkable())
              {
                writeMultiLineCodeLink(yyscanner,*yyextra->code,ctx->method,name);
                if (yyextra->currentMemberDef && yyextra->collectXRefs)
                {
                  std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
                  addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(ctx->method));
                }
              }
              else
              {
                codifyLines(yyscanner,name);
              }
            }
            else
            {
              DBG_CTX((stderr,"Invalid name: id=%d\n",refId));
            }
          }
          else if (nc=='o') // reference to potential object name
          {
            nc=*p++;
            QCString refIdStr;
            while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
            p--;
            int refId=refIdStr.toInt();
            auto it = yyextra->objectMap.find(refId);
            if (it!=yyextra->objectMap.end())
            {
              QCString object = it->second;
              if (object=="self")
              {
                if (yyextra->currentDefinition &&
                    yyextra->currentDefinition->definitionType()==Definition::TypeClass)
                {
                  ctx->objectType = toClassDef(yyextra->currentDefinition);
                  if (ctx->objectType->categoryOf())
                  {
                    ctx->objectType = ctx->objectType->categoryOf();
                  }
                  if (ctx->objectType && !ctx->methodName.isEmpty())
                  {
                    ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
                  }
                }
                startFontClass(yyscanner,"keyword");
                codifyLines(yyscanner,object);
                endFontClass(yyscanner);
              }
              else if (object=="super")
              {
                if (yyextra->currentDefinition &&
                    yyextra->currentDefinition->definitionType()==Definition::TypeClass)
                {
                  const ClassDef *cd = toClassDef(yyextra->currentDefinition);
                  if (cd->categoryOf())
                  {
                    cd = cd->categoryOf();
                  }
                  for (const auto &bclass : cd->baseClasses())
                  {
                    if (bclass.classDef->compoundType()!=ClassDef::Protocol)
                    {
                      ctx->objectType = bclass.classDef;
                      if (ctx->objectType && !ctx->methodName.isEmpty())
                      {
                        ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
                      }
                    }
                  }
                }
                startFontClass(yyscanner,"keyword");
                codifyLines(yyscanner,object);
                endFontClass(yyscanner);
              }
              else if (ctx->objectVar && ctx->objectVar->isLinkable()) // object is class variable
              {
                writeMultiLineCodeLink(yyscanner,*yyextra->code,ctx->objectVar,object);
                if (yyextra->currentMemberDef && yyextra->collectXRefs)
                {
                  std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
                  addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(ctx->objectVar));
                }
              }
              else if (ctx->objectType &&
                  ctx->objectType->isLinkable()
                  ) // object is class name
              {
                const ClassDef *cd = ctx->objectType;
                writeMultiLineCodeLink(yyscanner,*yyextra->code,cd,object);
              }
              else // object still needs to be resolved
              {
                const ClassDef *cd = yyextra->symbolResolver.resolveClass(yyextra->currentDefinition, object);
                if (cd && cd->isLinkable())
                {
                  if (ctx->objectType==0) ctx->objectType=cd;
                  writeMultiLineCodeLink(yyscanner,*yyextra->code,cd,object);
                }
                else
                {
                  codifyLines(yyscanner,object);
                }
              }
            }
            else
            {
              DBG_CTX((stderr,"Invalid object: id=%d\n",refId));
            }
          }
          else if (nc=='c') // reference to nested call
          {
            nc=*p++;
            QCString refIdStr;
            while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
            p--;
            int refId=refIdStr.toInt();
            auto it = yyextra->contextMap.find(refId);
            if (it!=yyextra->contextMap.end()) // recurse into nested call
            {
              ObjCCallCtx *ictx = it->second.get();
              writeObjCMethodCall(yyscanner,ictx);
              if (ictx->method) // link to nested call successfully
              {
                // get the ClassDef representing the method's return type
                if (QCString(ictx->method->typeString())=="id")
                {
                  // see if the method name is unique, if so we link to it
                  MemberName *mn=Doxygen::memberNameLinkedMap->find(ctx->methodName);
                  //printf("mn->count=%d ictx->method=%s ctx->methodName=%s\n",
                  //    mn==0?-1:(int)mn->count(),
                  //    qPrint(ictx->method->name()),
                  //    qPrint(ctx->methodName));
                  if (mn && mn->size()==1) // member name unique
                  {
                    ctx->method = mn->front().get();
                  }
                }
                else
                {
                  ctx->objectType = stripClassName(yyscanner,ictx->method->typeString(),yyextra->currentDefinition);
                  if (ctx->objectType && !ctx->methodName.isEmpty())
                  {
                    ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
                  }
                }
                DBG_CTX((stderr,"  ***** method=%s -> object=%p\n",qPrint(ictx->method->name()),(void*)ctx->objectType));
              }
            }
            else
            {
              DBG_CTX((stderr,"Invalid context: id=%d\n",refId));
            }
          }
          else if (nc=='w') // some word
          {
            nc=*p++;
            QCString refIdStr;
            while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
            p--;
            int refId=refIdStr.toInt();
            auto it = yyextra->wordMap.find(refId);
            if (it!=yyextra->wordMap.end())
            {
              QCString word = it->second;
              codifyLines(yyscanner,word);
            }
          }
          else if (nc=='d') // comment block
          {
            nc=*p++;
            QCString refIdStr;
            while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
            p--;
            int refId=refIdStr.toInt();
            auto it = yyextra->commentMap.find(refId);
            if (it!=yyextra->commentMap.end())
            {
              QCString comment = it->second;
              startFontClass(yyscanner,"comment");
              codifyLines(yyscanner,comment);
              endFontClass(yyscanner);
            }
          }
          else // illegal marker
          {
            ASSERT("invalid escape sequence"==0);
          }
        }
      }
      else // normal non-marker character
      {
        char s[2];
        s[0]=c;s[1]=0;
        codifyLines(yyscanner,s);
      }
    }
  }
  DBG_CTX((stderr,"%s %s]\n",qPrint(ctx->objectTypeOrName),qPrint(ctx->methodName)));
  DBG_CTX((stderr,"}=(type='%s',name='%s')",
           qPrint(ctx->objectTypeOrName),
           qPrint(ctx->methodName)));
}

// Replaces an Objective-C method name fragment s by a marker of the form
// $n12, the number (12) can later be used as a key for obtaining the name
// fragment, from yyextra->nameMap
static QCString escapeName(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$n%d",yyextra->currentNameId);
  yyextra->nameMap.emplace(std::make_pair(yyextra->currentNameId,s));
  yyextra->currentNameId++;
  return result;
}

static QCString escapeObject(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$o%d",yyextra->currentObjId);
  yyextra->objectMap.emplace(std::make_pair(yyextra->currentObjId,s));
  yyextra->currentObjId++;
  return result;
}

static QCString escapeWord(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$w%d",yyextra->currentWordId);
  yyextra->wordMap.emplace(std::make_pair(yyextra->currentWordId,s));
  yyextra->currentWordId++;
  return result;
}

static QCString escapeComment(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$d%d",yyextra->currentCommentId);
  yyextra->commentMap.emplace(std::make_pair(yyextra->currentCommentId,s));
  yyextra->currentCommentId++;
  return result;
}

static bool skipLanguageSpecificKeyword(yyscan_t yyscanner,const char *keyword)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  static const std::unordered_set<std::string> non_cpp_keywords = {
    "__assume", "__super", "abstract", "function",
    "gcnew", "gcroot", "generic", "get",
    "internal", "null", "pin_ptr", "raise",
    "remove", "self", "set", "transient"};
  static const std::unordered_set<std::string> non_java_keywords = {
    "alignas", "alignof", "and", "and_eq", "asm",
    "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto", "bitand",
    "bitor", "bool", "char8_t", "char16_t", "char32_t",
    "compl", "concept", "consteval", "constexpr", "constinit",
    "const_cast", "co_await", "co_return", "co_yield", "decltype",
    "delete", "dynamic_cast", "explicit", "export", "extern",
    "friend", "inline", "mutable", "namespace", "noexcept",
    "not", "not_eq", "nullptr", "operator", "or",
    "or_eq", "reflexpr", "register", "reinterpret_cast", "requires",
    "signed", "sizeof", "static_assert", "static_cast", "struct",
    "template", "thread_local", "typedef", "typeid", "typename",
    "union", "unsigned", "using", "virtual", "wchar_t",
    "xor", "xor_eq",
    "override"
  };
  bool retval;
  switch (yyextra->lang)
  {
    case SrcLangExt_Cpp:
      retval = (non_cpp_keywords.find(keyword) != non_cpp_keywords.end());
      break;
    case SrcLangExt_Java:
      retval = (non_java_keywords.find(keyword) != non_java_keywords.end());
      break;
    default: 
      retval = false;
      break;
  }
  return retval;
}

static bool isCastKeyword(const char *keyword)
{
  QCString s(keyword);
  int i=s.find('<');
  if (i==-1) return FALSE;
  QCString kw = s.left(i).stripWhiteSpace();
  return kw=="const_cast" || kw=="static_cast" || kw=="dynamic_cast" || kw=="reinterpret_cast";
}

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  yy_size_t c=0;
  while( c < max_size && *s )
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}


static void saveObjCContext(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentCtx)
  {
    yyextra->currentCtx->format+=QCString().sprintf("$c%d",yyextra->currentCtxId);
    if (yyextra->braceCount==0 && YY_START==ObjCCall)
    {
      yyextra->currentCtx->objectTypeOrName=yyextra->currentCtx->format.mid(1);
      DBG_CTX((stderr,"new type=%s\n",qPrint(yyextra->currentCtx->objectTypeOrName)));
    }
    yyextra->contextStack.push(yyextra->currentCtx);
  }
  else
  {
    DBG_CTX((stderr,"Trying to save NULL context!\n"));
  }
  auto newCtx = std::make_unique<ObjCCallCtx>();
  newCtx->id = yyextra->currentCtxId;
  newCtx->lexState = YY_START;
  newCtx->braceCount = yyextra->braceCount;
  newCtx->objectType = 0;
  newCtx->objectVar = 0;
  newCtx->method = 0;
  DBG_CTX((stderr,"save state=%d\n",YY_START));
  yyextra->currentCtx = newCtx.get();
  yyextra->contextMap.emplace(std::make_pair(yyextra->currentCtxId,std::move(newCtx)));
  yyextra->braceCount = 0;
  yyextra->currentCtxId++;
}

static void restoreObjCContext(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"restore state=%d->%d\n",YY_START,yyextra->currentCtx->lexState));
  BEGIN(yyextra->currentCtx->lexState);
  yyextra->braceCount = yyextra->currentCtx->braceCount;
  if (!yyextra->contextStack.empty())
  {
    yyextra->currentCtx = yyextra->contextStack.top();
    yyextra->contextStack.pop();
  }
  else
  {
    yyextra->currentCtx = 0;
    DBG_CTX((stderr,"Trying to pop context while yyextra->contextStack is empty!\n"));
  }
}

struct CCodeParser::Private
{
  yyscan_t yyscanner;
  codeYY_state state;
};

CCodeParser::CCodeParser() : p(std::make_unique<CCodeParser::Private>())
{
  codeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  codeYYset_debug(1,p->yyscanner);
#endif
  resetCodeParserState();
}

CCodeParser::~CCodeParser()
{
  codeYYlex_destroy(p->yyscanner);
}

void CCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  DBG_CTX((stderr,"***CodeParser::reset()\n"));
  yyextra->theVarContext.clear();
  while (!yyextra->classScopeLengthStack.empty()) yyextra->classScopeLengthStack.pop();
  yyextra->codeClassMap.clear();
  yyextra->curClassBases.clear();
  yyextra->anchorCount = 0;
  yyextra->insideCodeLine = false;
}

void CCodeParser::setInsideCodeLine(bool inp)
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->insideCodeLine = inp;
}

bool CCodeParser::insideCodeLine() const
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  return yyextra->insideCodeLine;
}

void CCodeParser::parseCode(CodeOutputInterface &od,const QCString &className,const QCString &s,
                SrcLangExt lang,bool exBlock, const QCString &exName,const FileDef *fd,
                int startLine,int endLine,bool inlineFragment,
                const MemberDef *memberDef,bool showLineNumbers,const Definition *searchCtx,
                bool collectXRefs)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"***parseCode() exBlock=%d exName=%s fd=%p className=%s searchCtx=%s\n",
           exBlock,qPrint(exName),(void*)fd,qPrint(className),searchCtx?qPrint(searchCtx->name()):"<none>"));

  if (s.isEmpty()) return;

  printlex(yy_flex_debug, TRUE, __FILE__, fd ? qPrint(fd->fileName()): !exName.isEmpty() ? qPrint(exName) : NULL);

  yyextra->code = &od;
  yyextra->inputString   = s.data();
  yyextra->fileName      = fd ? fd->fileName():"";
  yyextra->inputPosition = 0;
  codeYYrestart(0,yyscanner);
  yyextra->currentFontClass = 0;
  yyextra->searchCtx = searchCtx;
  yyextra->collectXRefs = collectXRefs;
  yyextra->inFunctionTryBlock = FALSE;
  yyextra->symbolResolver.setFileScope(fd);

  if (startLine!=-1)
    yyextra->yyLineNr    = startLine;
  else
    yyextra->yyLineNr    = 1;

  if (endLine!=-1)
    yyextra->inputLines  = endLine+1;
  else
    yyextra->inputLines  = yyextra->yyLineNr + countLines(yyscanner) - 1;

  yyextra->curlyCount    = 0;
  yyextra->bodyCurlyCount    = 0;
  yyextra->bracketCount  = 0;
  yyextra->sharpCount    = 0;
  yyextra->insideTemplate = FALSE;
  yyextra->theCallContext.clear();
  while (!yyextra->scopeStack.empty()) yyextra->scopeStack.pop();
  yyextra->classScope    = className;
  DBG_CTX((stderr,"parseCCode %s\n",qPrint(className)));
  yyextra->exampleBlock  = exBlock;
  yyextra->exampleName   = exName;
  yyextra->sourceFileDef = fd;
  yyextra->lineNumbers   = fd!=0 && showLineNumbers;
  bool cleanupSourceDef = FALSE;
  if (fd==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef(QCString(),(!exName.isEmpty()?exName:"generated"));
    cleanupSourceDef = TRUE;
  }
  yyextra->lang        = lang;
  yyextra->insideObjC  = lang==SrcLangExt_ObjC;
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }
  yyextra->currentDefinition = searchCtx ? searchCtx : getResolvedNamespace(className);
  yyextra->currentMemberDef = 0;
  yyextra->searchingForBody = exBlock;
  yyextra->insideBody = FALSE;
  yyextra->bracketCount = 0;
  if (!yyextra->exampleName.isEmpty())
  {
    yyextra->exampleFile = convertNameToFile(yyextra->exampleName+"-example",FALSE,TRUE);
    DBG_CTX((stderr,"yyextra->exampleFile=%s\n",qPrint(yyextra->exampleFile)));
  }
  yyextra->includeCodeFragment = inlineFragment;
  DBG_CTX((stderr,"** exBlock=%d exName=%s include=%d\n",exBlock,qPrint(exName),inlineFragment));
  if (!yyextra->insideCodeLine)
  {
    startCodeLine(yyscanner);
  }
  yyextra->type.resize(0);
  yyextra->name.resize(0);
  yyextra->args.resize(0);
  yyextra->parmName.resize(0);
  yyextra->parmType.resize(0);
  if (memberDef) setParameterList(yyscanner,memberDef);
  BEGIN( Body );
  codeYYlex(yyscanner);
  yyextra->lexInit=TRUE;
  if (yyextra->insideCodeLine)
  {
    endCodeLine(yyscanner);
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete yyextra->sourceFileDef;
    yyextra->sourceFileDef=0;
  }
  // write the tooltips
  yyextra->tooltipManager.writeTooltips(od);

  printlex(yy_flex_debug, FALSE, __FILE__, fd ? qPrint(fd->fileName()): NULL);
}

#if USE_STATE2STRING
#include "code.l.h"
#endif
