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
#include <cstdint>

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
#include "debug.h"
#include "moduledef.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_UNISTD_H 1

#define CLASSBLOCK 1
#define SCOPEBLOCK 2
#define INNERBLOCK 3

// context for an Objective-C method call
struct ObjCCallCtx
{
  int id = 0;
  QCString methodName;
  QCString objectTypeOrName;
  TextStream comment;
  const ClassDef *objectType = nullptr;
  const MemberDef *objectVar = nullptr;
  const MemberDef *method = nullptr;
  QCString format;
  int lexState = 0;
  int braceCount = 0;
};

struct codeYY_state
{
  OutputCodeList * code = nullptr;

  std::unordered_map< std::string, ScopedTypeVariant > codeClassMap;
  QCString      curClassName;
  StringVector  curClassBases;

  QCString      parmType;
  QCString      parmName;

  const char *  inputString = nullptr;     //!< the code fragment as text
  yy_size_t     inputPosition = 0;   //!< read offset during parsing
  QCString      fileName;
  QCString      absFileName;
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  bool          insideCodeLine = FALSE;
  bool          skipCodify = FALSE;  //!< for CSharp files scoped namespace {

  bool          exampleBlock = FALSE;
  QCString      exampleName;
  QCString      exampleFile;

  bool          insideTemplate = FALSE;
  QCString      type;
  QCString      name;
  QCString      args;
  QCString      scopeName;
  QCString      realScope;
  std::stack<int> scopeStack;      //!< 1 if bracket starts a scope,
                                   //   2 for internal blocks
  int           anchorCount = 0;
  std::unique_ptr<FileDef> exampleFileDef;
  const FileDef *     sourceFileDef = nullptr;
  bool          lineNumbers = FALSE;
  const Definition *  currentDefinition = nullptr;
  const MemberDef *   currentMemberDef = nullptr;
  bool          includeCodeFragment = FALSE;
  const char *  currentFontClass = nullptr;
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

  SrcLangExt    lang = SrcLangExt::Unknown;
  bool          insideObjC = FALSE;
  bool          insideProtocolList = FALSE;
  bool          insideSpecialComment = false;

  bool          lexInit = FALSE;

  std::vector<int>  scopeNameLengthStack;

  int           isPrefixedWithThis = FALSE;
  const Definition *searchCtx = nullptr;
  bool          collectXRefs = FALSE;

  ObjCCallCtx * currentCtx=nullptr;
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
  std::vector<const Definition *> foldStack;
};

static bool isCastKeyword(const char *s);

//-------------------------------------------------------------------
[[maybe_unused]] static const char *stateToString(int state);

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
static void startFontClass(yyscan_t yyscanner,const char *s,bool specialComment=false);
static void endFontClass(yyscan_t yyscanner,bool specialComment=false);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static void codifyLines(yyscan_t yyscanner,const char *text);
static void incrementFlowKeyWordCount(yyscan_t yyscanner);
static void writeMultiLineCodeLink(yyscan_t yyscanner,OutputCodeList &ol,
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
                           OutputCodeList &ol,
                           const QCString &text,
                           bool varOnly=FALSE
                          );
static bool getLink(yyscan_t yyscanner,const QCString &className,
                    const QCString &memberName,
                    OutputCodeList &ol,
                    const QCString &text=QCString(),
                    bool varOnly=FALSE);
static void generateClassOrGlobalLink(yyscan_t yyscanner,OutputCodeList &ol,const QCString &clName,
                                      bool typeOnly=FALSE,bool varOnly=FALSE);
static void generateClassOrGlobalLink(yyscan_t yyscanner,OutputCodeList &ol,const char *clName,
                                      bool typeOnly=FALSE,bool varOnly=FALSE);
static bool generateClassMemberLink(yyscan_t yyscanner,OutputCodeList &ol,const MemberDef *xmd,const QCString &memName);
static bool generateClassMemberLink(yyscan_t yyscanner,OutputCodeList &ol,const Definition *def,const QCString &memName);
static void generateMemberLink(yyscan_t yyscanner,OutputCodeList &ol,const QCString &varName,
            const QCString &memName);
static void generatePHPVariableLink(yyscan_t yyscanner,OutputCodeList &ol,const char *varName);
static void generateFunctionLink(yyscan_t yyscanner,OutputCodeList &ol,const QCString &funcName);
static void generateFunctionLink(yyscan_t yyscanner,OutputCodeList &ol,const char *funcName);
static int countLines(yyscan_t yyscanner);
static void writeObjCMethodCall(yyscan_t yyscanner,ObjCCallCtx *ctx);
static QCString escapeName(yyscan_t yyscanner,const char *s);
static QCString escapeObject(yyscan_t yyscanner,const char *s);
static QCString escapeWord(yyscan_t yyscanner,const char *s);
static QCString escapeComment(yyscan_t yyscanner,const char *s);
static bool skipLanguageSpecificKeyword(yyscan_t yyscanner,const char *kw);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void addVariable(yyscan_t yyscanner,QCString type,QCString name);
static bool startsWithKeyword(const QCString &str,const QCString &kw);
static void endCodeFold(yyscan_t yyscanner);

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
SCOPENAME ("::"{BN}*)?({ID}{BN}*{SEP}{BN}*)*("~"{BN}*)?{ID}
TEMPLIST "<"[^\"\}\{\(\)\/\n\>]*">"
SCOPETNAME (((({ID}{TEMPLIST}?){BN}*)?{SEP}{BN}*)*)((~{BN}*)?{ID})
SCOPEPREFIX ({ID}{TEMPLIST}?{BN}*{SEP}{BN}*)+
KEYWORD_OBJC ("@public"|"@private"|"@protected"|"@class"|"@implementation"|"@interface"|"@end"|"@selector"|"@protocol"|"@optional"|"@required"|"@throw"|"@synthesize"|"@property")
  /* please also pay attention to skipLanguageSpecificKeyword when changing the list of keywords. */
KEYWORD ("asm"|"__assume"|"auto"|"class"|"const"|"delete"|"enum"|"explicit"|"extern"|"false"|"friend"|"gcnew"|"gcroot"|"set"|"get"|"inline"|"internal"|"mutable"|"namespace"|"new"|"null"|"nullptr"|"override"|"operator"|"pin_ptr"|"private"|"protected"|"public"|"raise"|"register"|"remove"|"self"|"sizeof"|"static"|"struct"|"__super"|"function"|"template"|"generic"|"this"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"abstract"|"sealed"|"final"|"import"|"synchronized"|"transient"|"alignas"|"alignof"|"concept"|"requires"|"decltype"|{KEYWORD_OBJC}|"constexpr"|"consteval"|"constinit"|"co_await"|"co_return"|"co_yield"|"static_assert"|"_Static_assert"|"noexcept"|"thread_local"|"enum"{B}+("class"|"struct"))
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
MODULE_ID ({ID}".")*{ID}

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
%x      ModuleName
%x      ModuleImport

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
                                          yyextra->type.clear();
                                          yyextra->name.clear();
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
                                          yyextra->parmType.clear();yyextra->parmName.clear();
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
                                          bool found = false;
                                          bool ambig = false;
                                          QCString absIncFileName = determineAbsoluteIncludeName(yyextra->absFileName,yytext);
                                          const FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,absIncFileName,ambig);
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
                                          yyextra->theCallContext.pushScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                          yyextra->bracketCount = 0;

                                          DBG_CTX((stderr,"** type='%s' name='%s'\n",qPrint(yyextra->type),qPrint(yyextra->name)));
                                          if (yyextra->type.find("enum ")!=-1)
                                          { // for strong enums we need to start a scope
                                            DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
                                            pushScope(yyscanner,yyextra->name);
                                            yyextra->scopeStack.push(SCOPEBLOCK);
                                          }
                                          else
                                          {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                            yyextra->scopeStack.push(INNERBLOCK);
                                          }

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
                                          yyextra->type.clear();
                                          yyextra->name.clear();
                                          BEGIN( Body );
                                        }
<Body,FuncCall,MemberCall,MemberCall2>"}"  {
                                          yyextra->theVarContext.popScope();
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                          yyextra->type.clear();
                                          yyextra->name.clear();

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
                                            yyextra->currentMemberDef=nullptr;
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
                                            yyextra->insideObjC = fd->name().lower().endsWith(".m") ||
                                                                  fd->name().lower().endsWith(".mm");
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

                                          yyextra->currentMemberDef=nullptr;
                                          if (yyextra->currentDefinition)
                                            yyextra->currentDefinition=yyextra->currentDefinition->getOuterScope();
                                          BEGIN(Body);
                                        }
<ClassName,ClassVar>";"                 {
                                          if (yyextra->lang==SrcLangExt::CSharp)
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
                                          yyextra->name.clear();
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
                                          if (yyextra->lang==SrcLangExt::CSharp)
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
                                            if (yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,yyextra->curClassName,true)==nullptr)
                                            {
                                              DBG_CTX((stderr,"Adding new class %s\n",qPrint(yyextra->curClassName)));
                                              ScopedTypeVariant var(yyextra->curClassName);
                                              // insert base classes.
                                              for (const auto &s : yyextra->curClassBases)
                                              {
                                                const ClassDef *bcd=nullptr;
                                                auto it = yyextra->codeClassMap.find(s);
                                                if (it!=yyextra->codeClassMap.end())
                                                {
                                                  bcd = toClassDef(it->second.globalDef());
                                                }
                                                if (bcd==nullptr) bcd=yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,QCString(s),true);
                                                if (bcd && bcd->name()!=yyextra->curClassName)
                                                {
                                                  var.localDef()->insertBaseClass(bcd->name());
                                                }
                                              }
                                              yyextra->codeClassMap.emplace(yyextra->curClassName.str(),std::move(var));
                                            }
                                            //printf("yyextra->codeClassList.count()=%d\n",yyextra->codeClassList.count());
                                          }
                                          else // not a class name -> assume inner block
                                          {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
                                            yyextra->scopeStack.push(INNERBLOCK);
                                          }
                                          yyextra->curClassName.clear();
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
                                          yyextra->curClassBases.emplace_back(yytext);
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
                                          yyextra->args.clear();
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<Body>{SCOPEPREFIX}?"operator"/"("      {
                                          addType(yyscanner);
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->bracketCount=0;
                                          yyextra->args.clear();
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<Body>{SCOPEPREFIX}?"operator"[^a-z_A-Z0-9\(\n]+/"(" {
                                          addType(yyscanner);
                                          generateFunctionLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->bracketCount=0;
                                          yyextra->args.clear();
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
<Body>"module"/{B}*[:;]?                { // 'module X' or 'module : private' or 'module;'
                                          if (yyextra->lang!=SrcLangExt::Cpp) REJECT;
                                          if (!yyextra->type.isEmpty() || !yyextra->name.isEmpty()) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN(ModuleName);
                                        }
<Body>"import"/{B}*[<":]?               {
                                          if (yyextra->lang!=SrcLangExt::Cpp) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          BEGIN(ModuleImport);
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
                                          if (yyextra->lang==SrcLangExt::Java && qstrcmp("internal",yytext) ==0) REJECT;
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
                                          QCString text=yytext;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          if (text=="typedef" || text.find("enum ")!=-1) // typedef or strong enum
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
                                          yyextra->name.clear();yyextra->type.clear();
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
                                          yyextra->name.clear();yyextra->type.clear();
                                        }
<Body>{FLOWKW}/{BN}*"("                         {
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->name.clear();yyextra->type.clear();
                                          yyextra->inForEachExpression = (qstrcmp(yytext,"for each")==0 || qstrcmp(yytext, "foreach")==0);
                                          BEGIN(FuncCall);
                                        }
<Body>{FLOWCONDITION}/{BN}*"("          {
                                          incrementFlowKeyWordCount(yyscanner);
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->name.clear();yyextra->type.clear();
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
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                          yyextra->bracketCount--;
                                          yyextra->parmType = yyextra->name;
                                          BEGIN(FuncCall);
                                        }
<Body>"\\)"|"\\("                       {
                                          yyextra->code->codify(yytext);
                                        }
<Body>[\\|\)\+\-\/\%\~\!]               {
                                          yyextra->code->codify(yytext);
                                          yyextra->name.clear();yyextra->type.clear();
                                          if (*yytext==')')
                                          {
                                            yyextra->theCallContext.popScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                            yyextra->bracketCount--;
                                            if (yyextra->bracketCount<=0)
                                            {
                                              BEGIN(FuncCall);
                                            }
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
                                          if (yyextra->lang!=SrcLangExt::Slice)
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
<Body,TemplCast>{SCOPENAME}{B}*"<"[^\n\/\-\.\{\"\>\(']*">"{ENDIDopt}/{B}* { // A<T> *pt;
                                          if (isCastKeyword(yytext) && YY_START==Body)
                                          {
                                            REJECT;
                                          }
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext;
                                        }
<ModuleName,ModuleImport>{MODULE_ID}({BN}*":"{BN}*{MODULE_ID})?  {
                                          QCString name = yytext;
                                          int i = name.find(':');
                                          QCString partition;
                                          if (i!=-1)
                                          {
                                            partition = name.mid(i+1).stripWhiteSpace();
                                            name = name.left(i).stripWhiteSpace();
                                          }
                                          ModuleDef *mod = ModuleManager::instance().getPrimaryInterface(name);
                                          if (mod)
                                          {
                                            writeMultiLineCodeLink(yyscanner,*yyextra->code,mod,yytext);
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext);
                                          }
                                        }
<ModuleName>":"{BN}+"private"           {
                                          QCString text=yytext;
                                          int i=text.find('p');
                                          codifyLines(yyscanner,text.left(i));
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,text.mid(i));
                                          endFontClass(yyscanner);
                                        }
<ModuleName>";"                         { yyextra->code->codify(yytext); BEGIN(Body); }
<ModuleName>.                           { yyextra->code->codify(yytext); }
<ModuleName>\n                          { codifyLines(yyscanner,yytext); }
<ModuleImport>["<]                      { yyextra->code->codify(yytext); BEGIN(ReadInclude); }
<ModuleImport>";"                       { yyextra->code->codify(yytext); BEGIN(Body); }
<ModuleImport>.                         { yyextra->code->codify(yytext); }
<ModuleImport>\n                        { codifyLines(yyscanner,yytext); }

<Body>{SCOPENAME}/{BN}*[:;,)\]]         { // "int var;" or "var, var2" or "debug(f) macro" , or int var : 5;
                                          if (startsWithKeyword(yytext,"typedef")) REJECT;
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext;
                                        }
<Body>{ID}("."{ID})+/{BN}+              { // CSharp/Java scope
                                          if (yyextra->lang==SrcLangExt::CSharp || yyextra->lang==SrcLangExt::Java)
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
<Body>"export"/{B}*                     {
                                          if (yyextra->lang!=SrcLangExt::Cpp) REJECT;
                                          startFontClass(yyscanner,"keyword");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Body>{SCOPENAME}/{B}*                  { // p->func()
                                          if (startsWithKeyword(yytext,"typedef")) REJECT;
                                          addType(yyscanner);
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->name+=yytext;
                                        }
<Body>"("{B}*("*"{B}*)+{SCOPENAME}+{B}*")"/{B}* {  // (*p)->func() but not "if (p) ..."
                                          yyextra->code->codify(yytext);
                                          uint32_t s=0;while (s<(uint32_t)yyleng && !isId(yytext[s])) s++;
                                          uint32_t e=(uint32_t)yyleng-1;while (e>1 && !isId(yytext[e])) e--;
                                          QCString varname = ((QCString)yytext).mid(s,e-s+1);
                                          addType(yyscanner);
                                          yyextra->name=std::move(varname);
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
                                          yyextra->args.clear();
                                          yyextra->name+=yytext;
                                          BEGIN( FuncCall );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>{RAWBEGIN}   {
                                          QCString text(yytext);
                                          uint32_t i=(uint32_t)text.find('R');
                                          yyextra->code->codify(text.left(i+1));
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(QCString(yytext+i+1));
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->inForEachExpression = FALSE;
                                          yyextra->delimiter = yytext+i+2;
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                                          BEGIN( RawString );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit,ClassVar,OldStyleArgs>\"   {
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->inForEachExpression = FALSE;
                                          BEGIN( SkipString );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>{NUMBER} { //Note similar code in commentcnv.l
                                          if (yyextra->lang!=SrcLangExt::Cpp) REJECT;
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
                                          if (yyextra->lastStringContext!=SkipCPP)
                                          {
                                            endFontClass(yyscanner);
                                          }
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
                                          yyextra->name.clear();yyextra->type.clear();
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
<Body,MemberCall,MemberCall2,FuncCall,OldStyleArgs>"'"((\\0[Xx0-9]+)|(\\.)|(.))"'"   {
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
                                            yyextra->name.clear();
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext);
                                            addToSearchIndex(yyscanner,yytext);
                                            yyextra->name.clear();
                                          }
                                          yyextra->type.clear();
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
                                            yyextra->name.clear();
                                          }
                                          else
                                          {
                                            DBG_CTX((stderr,"no class context!\n"));
                                            codifyLines(yyscanner,yytext);
                                            addToSearchIndex(yyscanner,yytext);
                                            yyextra->name.clear();
                                          }
                                          yyextra->type.clear();
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
                                            yyextra->currentCtx = nullptr;
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
                                              yyextra->name.clear();
                                            }
                                            if (*yytext==';' || *yytext=='=')
                                            {
                                              yyextra->type.clear();
                                              yyextra->name.clear();
                                            }
                                            else if (*yytext=='[')
                                            {
                                              yyextra->theCallContext.pushScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                            }
                                            yyextra->args.clear();
                                            yyextra->parmType.clear();
                                            yyextra->parmName.clear();
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
                                    if (yyextra->currentCtx==nullptr)
                                    {
                                      // end of call
                                      ObjCCallCtx *ctx = nullptr;
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
<ObjCCall>{ID}({B}*"."{B}*{ID})*  {
                                    yyextra->currentCtx->format+=escapeObject(yyscanner,QCString(yytext).data());
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
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type, yyextra->bracketCount);
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
                                          if (yyextra->lang!=SrcLangExt::Slice)
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
<MemberCall2,FuncCall>("::")?{ID}(({B}*"<"[^\n\[\](){}<>']*">")?({B}*"::"{B}*{ID})?)* {
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
                                          yyextra->parmType.clear();yyextra->parmName.clear();
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
                                          yyextra->parmType.clear();yyextra->parmName.clear();
                                          yyextra->code->codify(yytext);
                                          yyextra->bracketCount++;
                                          yyextra->theCallContext.pushScope(yyextra->name, yyextra->type, yyextra->bracketCount);
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
                                            yyextra->parmType.clear();yyextra->parmName.clear();
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
                                              yyextra->parmName.clear();
                                            }
                                            addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          }
                                          else
                                          {
                                            yyextra->parmType = yyextra->parmName;
                                            yyextra->parmName.clear();
                                            addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          }
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type, yyextra->bracketCount);
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
<MemberCall,MemberCall2,FuncCall>[;:]   { // recover from unexpected end of call
                                          //if (yytext[0]==';' || yyextra->bracketCount<=0)
                                          if (yyextra->bracketCount<=0)
                                          {
                                            unput(*yytext);
                                            BEGIN(CallEnd);
                                          }
                                          else 
                                          {
                                            yyextra->code->codify(yytext);
                                          }
                                        }
<CallEnd>[ \t\n]*                       { codifyLines(yyscanner,yytext); }
<CallEnd>[;:]                           {
                                          codifyLines(yyscanner,yytext);
                                          yyextra->bracketCount=0;
                                          if (*yytext==';') yyextra->searchingForBody=FALSE;
                                          if (!yyextra->type.isEmpty())
                                          {
                                            DBG_CTX((stderr,"add variable yyextra->type=%s yyextra->name=%s)\n",qPrint(yyextra->type),qPrint(yyextra->name)));
                                            addVariable(yyscanner,yyextra->type,yyextra->name);
                                          }
                                          yyextra->parmType.clear();yyextra->parmName.clear();
                                          yyextra->theCallContext.setScope(ScopedTypeVariant());
                                          if (*yytext==';' || yyextra->insideBody)
                                          {
                                            if (!yyextra->insideBody)
                                            {
                                              yyextra->theVarContext.popScope();
                                            }
                                            yyextra->name.clear();yyextra->type.clear();
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
                                          yyextra->parmType.clear();yyextra->parmName.clear();
                                          int index = yyextra->name.findRev("::");
                                          DBG_CTX((stderr,"yyextra->name=%s\n",qPrint(yyextra->name)));
                                          if (index!=-1)
                                          {
                                            QCString scope = yyextra->name.left((uint32_t)index);
                                            if (!yyextra->scopeName.isEmpty()) scope.prepend((yyextra->scopeName+"::"));
                                            const ClassDef *cd=yyextra->symbolResolver.resolveClass(Doxygen::globalScope,scope,true);
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
                                          yyextra->type.clear(); yyextra->name.clear();
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
                                          if (*yytext==';') yyextra->parmType.clear();
                                          yyextra->parmName.clear();
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
                                          yyextra->name.clear();yyextra->args.clear();
                                          yyextra->parmType.clear();yyextra->parmName.clear();
                                          BEGIN( Body );
                                        }
<SkipInits>";"                          {
                                          yyextra->code->codify(yytext);
                                          yyextra->type.clear(); yyextra->name.clear();
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
                                          yyextra->type.clear(); yyextra->name.clear();
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
                                          uint32_t s=0;while (!isId(yytext[s])) s++;
                                          uint32_t e=(uint32_t)yyleng-1;while (e>1 && !isId(yytext[e])) e--;
                                          yyextra->name=((QCString)yytext).mid(s,e-s+1);
                                          BEGIN( MemberCall2 );
                                        }
<MemberCall2>{ID}/([ \t\n]*"(")         {
                                          if (!yyextra->args.isEmpty())
                                            generateMemberLink(yyscanner,*yyextra->code,yyextra->args,yytext);
                                          else
                                            generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          yyextra->args.clear();
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
                                          endFontClass(yyscanner,true);
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
                                          endFontClass(yyscanner,true);
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
<MemberCall>[^a-z_A-Z0-9(\n]            {
                                          yyextra->code->codify(yytext);
                                          yyextra->type.clear();
                                          yyextra->name.clear();
                                          BEGIN(yyextra->memCallContext);
                                        }
<*>\n({B}*{CPPC}[!/][^\n]*\n)+            { // remove special one-line comment
                                          if (YY_START==SkipCPP) REJECT;
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner,true);
                                          if (YY_START==SkipCxxComment)
                                          {
                                            BEGIN( yyextra->lastCContext ) ;
                                          }
                                        }
<SkipCPP>\n/(.|\n)                      {
                                          endFontClass(yyscanner);
                                          BEGIN( yyextra->lastSkipCppContext ) ;
                                          unput('\n');
                                        }
<*>\n{B}*{CPPC}"@"[{}].*\n              { // remove one-line group marker
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner,true);
                                          if (YY_START==SkipCxxComment)
                                          {
                                            BEGIN( yyextra->lastCContext ) ;
                                          }
                                        }
<*>\n{B}*{CCS}"@"[{}]                   { // remove one-line group marker
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {

                                            yyextra->lastCContext = YY_START ;
                                          }
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          BEGIN(SkipComment);
                                        }
<*>^{B}*{CPPC}"@"[{}].*\n               { // remove one-line group marker
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner,true);
                                        }
<*>^{B}*{CCS}"@"[{}]                    { // remove multi-line group marker
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          startFontClass(yyscanner,"comment",true);
                                          yyextra->code->codify(yytext);
                                          BEGIN(SkipComment);
                                        }
<*>^{B}*{CPPC}[!/][^\n]*                { // remove special one-line comment
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner,true);
                                        }
<*>{CPPC}[!/][^\n]*                       { // strip special one-line comment
                                          if (YY_START==SkipComment || YY_START==SkipString) REJECT;
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner,true);
                                        }
<*>\n{B}*{CCS}[!*]/{NCOMM}              {
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          startFontClass(yyscanner,"comment",true);
                                          codifyLines(yyscanner,yytext);
                                          BEGIN(SkipComment);
                                        }
<*>^{B}*{CCS}"*"[*]+/[^/]               { 
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          // special C "banner" comment block at a new line
                                          startFontClass(yyscanner,"comment",Config_getBool(JAVADOC_BANNER));
                                          yyextra->code->codify(yytext);
                                          BEGIN(SkipComment);
                                        }
<*>^{B}*{CCS}[!*]/{NCOMM}               { // special C comment block at a new line
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          startFontClass(yyscanner,"comment",true);
                                          yyextra->code->codify(yytext);
                                          BEGIN(SkipComment);
                                        }
<*>{CCS}[!*]/{NCOMM}                    { // special C comment block half way a line
                                          if (YY_START==SkipString) REJECT;
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          startFontClass(yyscanner,"comment",true);
                                          yyextra->code->codify(yytext);
                                          BEGIN(SkipComment);
                                        }
<*>{CCS}("!"?){CCE}                     {
                                          if (YY_START==SkipString) REJECT;
                                          bool specialComment = QCString(yytext).find('!')!=-1;
                                          startFontClass(yyscanner,"comment",specialComment);
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner,specialComment);
                                        }
<SkipComment>[^\*\n]+                   {
                                          yyextra->code->codify(yytext);
                                        }
<*>{CCS}                                {
                                          startFontClass(yyscanner,"comment");
                                          yyextra->code->codify(yytext);
                                          // check is to prevent getting stuck in skipping C++ comments
                                          if (YY_START != SkipComment && YY_START != SkipCxxComment)
                                          {
                                            yyextra->lastCContext = YY_START ;
                                          }
                                          BEGIN( SkipComment ) ;
                                        }
<*>[$]?@\"                                  { // C# (interpolated) verbatim string
                                          startFontClass(yyscanner,"stringliteral");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastVerbStringContext=YY_START;
                                          BEGIN(SkipVerbString);
                                        }
<*>{CPPC}                               {
                                          startFontClass(yyscanner,"comment");
                                          yyextra->code->codify(yytext);
                                          yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipCxxComment ) ;
                                        }
<*>"("|"["                              {
                                          if (yytext[0]=='(') yyextra->bracketCount++;
                                          yyextra->code->codify(yytext);
                                          yyextra->theCallContext.pushScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                        }
<*>")"|"]"                              {
                                          if (yytext[0]==')') yyextra->bracketCount--;
                                          yyextra->code->codify(yytext);
                                          yyextra->theCallContext.popScope(yyextra->name, yyextra->type, yyextra->bracketCount);
                                        }
<*>\n                                   {
                                          codifyLines(yyscanner,yytext);
                                        }
<*>[\x80-\xFF]*                         { // keep utf8 characters together...
                                          yyextra->code->codify(yytext);
                                        }
<*>.                                    {
                                          yyextra->code->codify(yytext);
                                        }

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
  ltype.stripPrefix("struct ");
  ltype.stripPrefix("union ");
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
      const ClassDef *varDef = yyextra->symbolResolver.resolveClass(d,ltype,true);
      int i=0;
      if (varDef)
      {
        DBG_CTX((stderr,"** addVariable type='%s' name='%s'\n",qPrint(ltype),qPrint(lname)));
        yyextra->theVarContext.addVariable(lname,ScopedTypeVariant(varDef)); // add it to a list
      }
      else if ((i=ltype.find('<'))!=-1)
      {
        // probably a template class
        addVariable(yyscanner,ltype.left(i),name);
      }
      return varDef;
    };
    const ClassDef *varDef = findVariableType(yyextra->currentDefinition);
    if (varDef==nullptr) // also check via using directive
    {
      for (const auto &[usingName,namespaceDef] : yyextra->theUsingContext)
      {
        varDef = findVariableType(namespaceDef);
        if (varDef) break;
      }
    }
    if (varDef==nullptr)
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
  yyextra->scopeNameLengthStack.push_back(int(yyextra->scopeName.length()));
  if (yyextra->scopeName.isEmpty() || leftScopeMatch(s,yyextra->scopeName))
  {
    yyextra->scopeName = s;
  }
  else
  {
    yyextra->scopeName += "::";
    yyextra->scopeName += s;
  }
  DBG_CTX((stderr,"pushScope(%s) result: '%s'\n",qPrint(s),qPrint(yyextra->scopeName)));
}


/*! remove the top class/namespace name from the scope */
static void popScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->scopeNameLengthStack.empty())
  {
    int length = yyextra->scopeNameLengthStack.back();
    yyextra->scopeNameLengthStack.pop_back();
    yyextra->scopeName.resize(length);
  }
  else
  {
    //err("Too many end of scopes found!\n");
  }
  DBG_CTX((stderr,"popScope() result: '%s'\n",qPrint(yyextra->scopeName)));
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
  while (!yyextra->scopeNameLengthStack.empty())
  {
    popScope(yyscanner);
  }
  yyextra->scopeName.clear();
  int i;
  while ((i=n.find("::"))!=-1)
  {
    pushScope(yyscanner,n.left(i));
    n = n.mid(i+2);
  }
  pushScope(yyscanner,n);
  DBG_CTX((stderr,"--->New class scope '%s'\n",qPrint(yyextra->scopeName)));
}

static void endCodeFold(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  while (!yyextra->foldStack.empty())
  {
    const Definition *dd = yyextra->foldStack.back();
    if (dd->getEndBodyLine()+1==yyextra->yyLineNr) // +1 to close the section after the end of the body
    {
      yyextra->code->endFold();
      //fprintf(stderr,"%d:   end codeFolding for %s [%d..%d]\n",yyextra->yyLineNr,qPrint(dd->name()),dd->getStartDefLine(),dd->getEndBodyLine());
      yyextra->foldStack.pop_back();
    }
    else
    {
      //fprintf(stderr,"no end of block dd=%s end=%d\n",qPrint(dd->qualifiedName()),dd->getEndBodyLine());
      break;
    }
  }
}


static void codeFolding(yyscan_t yyscanner,const Definition *d)
{
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    //fprintf(stderr,"codeFolding at %d\n",yyextra->yyLineNr);
    //if (d)
    //  fprintf(stderr,"%d: codeFolding: candidate=%s [%d..%d]\n",yyextra->yyLineNr,qPrint(d->qualifiedName()),d->getStartDefLine(),d->getEndBodyLine());
    endCodeFold(yyscanner);
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
        if (d->definitionType()==Definition::TypeMember)
        {
          const MemberDef *md = toMemberDef(d);
          if (md && md->isDefine())
          {
            yyextra->code->startFold(yyextra->yyLineNr,"",""); // #define X ...
          }
          else if (md && md->isCallable())
          {
            yyextra->code->startFold(yyextra->yyLineNr,"{","}"); // func() { ... }
          }
          else
          {
            yyextra->code->startFold(yyextra->yyLineNr,"{","};"); // enum X { ... }
          }
        }
        else if (d->definitionType()==Definition::TypeClass)
        {
          yyextra->code->startFold(yyextra->yyLineNr,"{","};"); // class X { ... };
        }
        else
        {
          yyextra->code->startFold(yyextra->yyLineNr,"{","}"); // namespace X {...}
        }
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
  //if (yyextra->currentFontClass) { yyextra->code->endFontClass(yyscanner); }
  if (yyextra->sourceFileDef && yyextra->lineNumbers)
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
      yyextra->args.clear();
      yyextra->parmType.clear();
      yyextra->parmName.clear();
      DBG_CTX((stderr,"Real scope: '%s'\n",qPrint(yyextra->realScope)));
      yyextra->bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        codeFolding(yyscanner,yyextra->currentMemberDef);
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),
                                yyextra->yyLineNr,!yyextra->includeCodeFragment);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        codeFolding(yyscanner,d);
        yyextra->code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                QCString(),yyextra->yyLineNr,!yyextra->includeCodeFragment);
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
  DBG_CTX((stderr,"startCodeLine(%d)\n",yyextra->yyLineNr));
  yyextra->code->startCodeLine(yyextra->yyLineNr);
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
    while ((c=*p++) && c!='\n');
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
  std::lock_guard<std::mutex> lock(Doxygen::countFlowKeywordsMutex);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
  {
    MemberDefMutable *md = toMemberDefMutable(const_cast<MemberDef*>(yyextra->currentMemberDef));
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
static void writeMultiLineCodeLink(yyscan_t yyscanner,OutputCodeList &ol,
                                   const Definition *d,
                                   const QCString &text)
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
  if (yyextra->name=="const") { yyextra->name.clear(); return; }
  if (!yyextra->type.isEmpty()) yyextra->type += ' ' ;
  yyextra->type += yyextra->name ;
  yyextra->name.clear() ;
  if (!yyextra->type.isEmpty()) yyextra->type += ' ' ;
  yyextra->type += yyextra->args ;
  yyextra->args.clear() ;
}

static void addParmType(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->parmName=="const") { yyextra->parmName.clear(); return; }
  if (!yyextra->parmType.isEmpty()) yyextra->parmType += ' ' ;
  yyextra->parmType += yyextra->parmName ;
  yyextra->parmName.clear() ;
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
      yyextra->theUsingContext.emplace(name.str(),nd);
    }
  }
}

static void setParameterList(yyscan_t yyscanner,const MemberDef *md)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
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
  DBG_CTX((stderr,"stripClassName(scope=%s,type=%s) scopeName=%s\n",
        qPrint(d?d->name():""),qPrint(s),qPrint(yyextra->scopeName)));
  int pos=0;
  QCString type = s;
  QCString className;
  QCString templSpec;
  while (extractClassNameFromType(type,pos,className,templSpec)!=-1)
  {
    QCString clName=className+templSpec;
    const ClassDef *cd=nullptr;
    if (!yyextra->scopeName.isEmpty())
    {
      cd=yyextra->symbolResolver.resolveClass(d,yyextra->scopeName+"::"+clName,true);
    }
    if (cd==nullptr)
    {
      cd=yyextra->symbolResolver.resolveClass(d,clName,true);
    }
    DBG_CTX((stderr,"stripClass trying '%s' = %p\n",qPrint(clName),(void*)cd));
    if (cd)
    {
      return cd;
    }
  }

  return nullptr;
}

static const MemberDef *setCallContextForVar(yyscan_t yyscanner,const QCString &name)
{
  if (name.isEmpty()) return nullptr;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"setCallContextForVar(%s) yyextra->scopeName=%s\n",qPrint(name),qPrint(yyextra->scopeName)));

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

  const ScopedTypeVariant *mcv = yyextra->theVarContext.findVariable(name);
  if (mcv)
  {
    DBG_CTX((stderr,"local variable?\n"));
    if (!mcv->isDummy()) // locally found variable
    {
      DBG_CTX((stderr,"local var '%s' mcd=%s\n",qPrint(name),qPrint(mcv->name())));
      yyextra->theCallContext.setScope(*mcv);
    }
  }
  else
  {
    DBG_CTX((stderr,"class member? scope=%s\n",qPrint(yyextra->scopeName)));
    // look for a class member
    const ClassDef *mcd = getClass(yyextra->scopeName);
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
  const MemberName *mn = Doxygen::functionNameLinkedMap->find(name);
  if (mn)
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
      return nullptr;
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
      return nullptr;
    }
  }
  return nullptr;
}

static void updateCallContextForSmartPointer(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const Definition *d = yyextra->theCallContext.getScope().globalDef();
  //printf("updateCallContextForSmartPointer() cd=%s\n",cd ? qPrint(d->name()) : "<none>");
  const MemberDef *md = nullptr;
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
                           OutputCodeList &ol,
                           const QCString &text,
                           bool varOnly
                          )
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"getLinkInScope: trying '%s'::'%s' varOnly=%d\n",qPrint(c),qPrint(m),varOnly));
  GetDefInput input(c,m,"()");
  input.currentFile = yyextra->sourceFileDef;
  input.insideCode = true;
  GetDefResult result = getDefs(input);
  DBG_CTX((stderr,"scopeNameLengthStack.size()=%zu\n",yyextra->scopeNameLengthStack.size()));
  if (!result.found && !yyextra->scopeNameLengthStack.empty() && c==yyextra->scopeName)
  {
    QCString localName = yyextra->scopeName.mid(yyextra->scopeNameLengthStack[0]+2); // try also without leading scope
    auto it = yyextra->codeClassMap.find(localName.str());
    if (it!=yyextra->codeClassMap.end())
    {
      ScopedTypeVariant ccd = it->second;
      if (ccd.localDef() && !ccd.localDef()->baseClasses().empty())
      {
        for (const auto &bcName : ccd.localDef()->baseClasses())
        {
          DBG_CTX((stderr,"trying lookup via base class %s\n",qPrint(bcName)));
          input.scopeName = bcName;
          result = getDefs(input);
          if (result.found) break;
        }
      }
    }
  }
  if (result.found && (!varOnly || result.md->isVariable()))
  {
    if (result.md->isLinkable())
    {
      DBG_CTX((stderr,"found it %s!\n",qPrint(result.md->qualifiedName())));
      if (yyextra->exampleBlock)
      {
        std::lock_guard<std::mutex> lock(Doxygen::addExampleMutex);
        QCString anchor;
        anchor.sprintf("a%d",yyextra->anchorCount);
        DBG_CTX((stderr,"addExampleFile(%s,%s,%s)\n",qPrint(anchor),qPrint(yyextra->exampleName),
                                                     qPrint(yyextra->exampleFile)));
        MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef*>(result.md));
        if (mdm && mdm->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
        {
          ol.writeCodeAnchor(anchor);
          yyextra->anchorCount++;
        }
      }

      const Definition *d = result.md->getOuterScope()==Doxygen::globalScope ?
                            result.md->resolveAlias()->getFileDef() : result.md->getOuterScope();
      if (result.md->resolveAlias()->getGroupDef()) d = result.md->resolveAlias()->getGroupDef();
      if (d && d->isLinkable())
      {
        const ClassDef *ncd = stripClassName(yyscanner,result.md->typeString(),result.md->getOuterScope());
        if (ncd)
        {
          yyextra->theCallContext.setScope(ScopedTypeVariant(ncd));
        }
        DBG_CTX((stderr,"yyextra->currentDefinition=%p yyextra->currentMemberDef=%p yyextra->insideBody=%d\n",
                 (void*)yyextra->currentDefinition,(void*)yyextra->currentMemberDef,yyextra->insideBody));

        if (yyextra->currentDefinition && yyextra->currentMemberDef &&
            yyextra->insideBody && yyextra->collectXRefs)
        {
          addDocCrossReference(yyextra->currentMemberDef,result.md);
        }
        DBG_CTX((stderr,"d->getReference()='%s' d->getOutputBase()='%s' name='%s' member name='%s'\n",qPrint(d->getReference()),qPrint(d->getOutputFileBase()),qPrint(d->name()),qPrint(result.md->name())));

        writeMultiLineCodeLink(yyscanner,ol,result.md, !text.isEmpty() ? text : memberText);
        addToSearchIndex(yyscanner,!text.isEmpty() ? text : memberText);
        return TRUE;
      }
    }
  }
  return FALSE;
}

static bool getLink(yyscan_t yyscanner,
                    const QCString &className,
                    const QCString &memberName,
                    OutputCodeList &ol,
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
                                      OutputCodeList &ol,
                                      const QCString &scName,
                                      bool typeOnly,
                                      bool varOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString scopeName=scName;
  if (!scopeName.isEmpty() && scopeName[0]=='~') // correct for matching negated values i.s.o. destructors.
  {
    scopeName=scopeName.mid(1);
  }
  if (scopeName.isEmpty())
  {
    yyextra->code->codify("~");
    return;
  }
  if (yyextra->insideProtocolList) // for Obj-C
  {
    scopeName+="-p";
  }
  if (yyextra->lang==SrcLangExt::PHP)
  {
    scopeName = substitute(scopeName,"\\","::"); // for PHP namespaces
  }
  else if (yyextra->lang==SrcLangExt::CSharp || yyextra->lang==SrcLangExt::Java)
  {
    scopeName = substitute(scopeName,".","::"); // for C#/Java namespaces
  }
  if (!yyextra->scopeName.isEmpty())
  {
    scopeName = yyextra->scopeName+"::"+scopeName;
  }
  const ScopedTypeVariant *lcd=nullptr;
  const Definition *sym=nullptr;
  bool isLocal=FALSE;

  DBG_CTX((stderr,"generateClassOrGlobalLink(name=%s) yyextra->scopeName=%s scopeName=%s\n",qPrint(scName),qPrint(yyextra->scopeName),qPrint(scopeName)));
  if (!yyextra->isPrefixedWithThis || (lcd=yyextra->theVarContext.findVariable(scopeName))==nullptr) // not a local variable
  {
    int i=scopeName.find('<');
    QCString bareName = scopeName;
    if (i!=-1) bareName = bareName.left(i);

    auto checkForSymbol = [&yyg,&bareName,&scopeName](const Definition *parent,
                                                      const Definition *&sym_)
    {
      sym_ = yyextra->symbolResolver.resolveSymbol(parent,scopeName,QCString(),false,true);
      DBG_CTX((stderr,"non-local variable name=%s sym=%s!\n",
            qPrint(scopeName),sym_?qPrint(sym_->name()):"<none>"));
      if (sym_==nullptr && !bareName.isEmpty())
      {
        DBG_CTX((stderr,"bareName=%s\n",qPrint(bareName)));
        if (bareName!=scopeName)
        {
          sym_ = yyextra->symbolResolver.resolveSymbol(parent,bareName,QCString(),false,true); // try unspecialized version
        }
      }
    };
    const Definition *d = yyextra->currentDefinition;
    DBG_CTX((stderr,"d=%s yyextra->sourceFileDef=%s\n",d?qPrint(d->name()):"<none>",yyextra->sourceFileDef?qPrint(yyextra->sourceFileDef->name()):"<none>"));
    checkForSymbol(d,sym);
    if (sym==nullptr && d && d->definitionType()==Definition::TypeClass)
    {
      const FileDef *fd = toClassDef(d)->getFileDef();
      if (fd)
      {
        // also check for using directive in the file that defines this class
        for (const auto &nd : fd->getUsedNamespaces())
        {
          checkForSymbol(nd,sym);
          if (sym) break;
        }
      }
    }
    if (sym==nullptr)
    {
      // also check via using directive
      for (const auto &[usingName,namespaceDef] : yyextra->theUsingContext)
      {
        checkForSymbol(namespaceDef,sym);
        if (sym) break;
      }
    }

    const NamespaceDef *nd = getResolvedNamespace(scopeName);
    if (nd && nd->isLinkable())
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(nd));
      addToSearchIndex(yyscanner,scopeName);
      writeMultiLineCodeLink(yyscanner,*yyextra->code,nd,scName);
      return;
    }
    const ConceptDef *conceptDef = getResolvedConcept(d,bareName);
    if (conceptDef && conceptDef->isLinkable())
    {
      yyextra->theCallContext.setScope(ScopedTypeVariant(conceptDef));
      addToSearchIndex(yyscanner,scopeName);
      writeMultiLineCodeLink(yyscanner,*yyextra->code,conceptDef,scName);
      return;
    }
    DBG_CTX((stderr,"sym=%s\n",sym?qPrint(sym->name()):"<none>"));
    DBG_CTX((stderr,"is found as a type sym=%s nd=%s\n",
          sym?qPrint(sym->name()):"<null>",
          nd?qPrint(nd->name()):"<null>"));
    if (sym==nullptr) // also see if it is variable or enum or enum value
    {
      if (getLink(yyscanner,yyextra->scopeName,scName,ol,scName,varOnly))
      {
        return;
      }
    }
  }
  else
  {
    DBG_CTX((stderr,"local variable!\n"));
    if (!lcd->isDummy())
    {
      DBG_CTX((stderr,"non-dummy context lcd=%s!\n",qPrint(lcd->name())));
      yyextra->theCallContext.setScope(*lcd);

      // to following is needed for links to a global variable, but is
      // no good for a link to a local variable that is also a global symbol.

      //if (getLink(yyscanner,yyextra->scopeName,scName,ol,scName))
      //{
        //return;
      //}
    }
    isLocal=TRUE;
    DBG_CTX((stderr,"is a local variable sym=%p!\n",(void*)sym));
  }
  yyextra->isPrefixedWithThis = FALSE; // discard the "this" prefix for the next calls

  if (sym && sym->isLinkable()) // is it a linkable class
  {
    DBG_CTX((stderr,"is linkable class %s\n",qPrint(scName)));
    if (yyextra->exampleBlock)
    {
      std::lock_guard<std::mutex> lock(Doxygen::addExampleMutex);
      QCString anchor;
      anchor.sprintf("_a%d",yyextra->anchorCount);
      DBG_CTX((stderr,"addExampleClass(%s,%s,%s)\n",qPrint(anchor),qPrint(yyextra->exampleName),
                                                    qPrint(yyextra->exampleFile)));
      if (sym->definitionType()==Definition::TypeClass)
      {
        ClassDefMutable *cdm = toClassDefMutable(const_cast<Definition*>(sym));
        if (cdm && cdm->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
        {
          ol.writeCodeAnchor(anchor);
          yyextra->anchorCount++;
        }
      }
    }
    writeMultiLineCodeLink(yyscanner,ol,sym,scName);
    addToSearchIndex(yyscanner,scopeName);
    yyextra->theCallContext.setScope(ScopedTypeVariant(sym));
    if (sym && sym->definitionType()==Definition::TypeMember)
    {
      const MemberDef *md = toMemberDef(sym);
      const Definition *d = sym->getOuterScope()==Doxygen::globalScope ?
                            md->getFileDef() : md->getOuterScope();
      if (md->getGroupDef()) d = md->getGroupDef();
      if (d && d->isLinkable() && md->isLinkable() &&
          yyextra->currentMemberDef && yyextra->collectXRefs)
      {
        addDocCrossReference(yyextra->currentMemberDef,md);
      }
    }
  }
  else // not a class, maybe a global member
  {
    const MemberDef *md = sym && sym->definitionType()==Definition::TypeMember ? toMemberDef(sym) : nullptr;
    DBG_CTX((stderr,"class %s not linkable! cd=%p typeOnly=%d\n",qPrint(scName),(void*)sym,typeOnly));
    if (!isLocal && (md || (!sym && !typeOnly))) // not a class, see if it is a global enum/variable/typedef.
    {
      if (md==nullptr) // not found as a typedef
      {
        md = setCallContextForVar(yyscanner,scName);
        DBG_CTX((stderr,"setCallContextForVar(%s) md=%p yyextra->currentDefinition=%s\n",qPrint(scName),(void*)md,yyextra->currentDefinition ? qPrint(yyextra->currentDefinition->name()) : "<none>"));
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
          md=nullptr; // variable not accessible
        }
      }
      if (md && (!varOnly || md->isVariable()))
      {
        DBG_CTX((stderr,"is a global md=%p yyextra->currentDefinition=%s linkable=%d\n",(void*)md,yyextra->currentDefinition?qPrint(yyextra->currentDefinition->name()):"<none>",md->isLinkable()));
        if (md->isLinkable())
        {
          writeMultiLineCodeLink(yyscanner,ol,md,scName);
          addToSearchIndex(yyscanner,scName);
          if (yyextra->currentMemberDef && yyextra->collectXRefs)
          {
            addDocCrossReference(yyextra->currentMemberDef,md);
          }
          return;
        }
      }
    }

    // nothing found, just write out the word
    DBG_CTX((stderr,"not found!\n"));
    codifyLines(yyscanner,scName);
    addToSearchIndex(yyscanner,scName);
  }
}

static void generateClassOrGlobalLink(yyscan_t yyscanner,OutputCodeList &ol,const char *clName,
                                      bool typeOnly,bool varOnly)
{
  generateClassOrGlobalLink(yyscanner,ol,QCString(clName),typeOnly,varOnly);
}

static bool generateClassMemberLink(yyscan_t yyscanner,
                                    OutputCodeList &ol,
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
    std::lock_guard<std::mutex> lock(Doxygen::addExampleMutex);
    QCString anchor;
    anchor.sprintf("a%d",yyextra->anchorCount);
    DBG_CTX((stderr,"addExampleFile(%s,%s,%s)\n",qPrint(anchor),qPrint(yyextra->exampleName),
                                                 qPrint(yyextra->exampleFile)));
    MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef*>(xmd));
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
        addDocCrossReference(yyextra->currentMemberDef,xmd);
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
                                    OutputCodeList &ol,
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
                               OutputCodeList &ol,
                               const QCString &varName,
                               const QCString &memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"generateMemberLink(object=%s,mem=%s) classScope=%s\n",
           qPrint(varName),qPrint(memName),qPrint(yyextra->scopeName)));

  if (varName.isEmpty()) return;

  // look for the variable in the current context
  const ScopedTypeVariant *stv = yyextra->theVarContext.findVariable(varName);
  if (stv)
  {
    if (!stv->isDummy())
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
    const ClassDef *vcd = yyextra->symbolResolver.resolveClass(yyextra->currentDefinition,yyextra->scopeName,true);
    if (vcd && vcd->isLinkable())
    {
      DBG_CTX((stderr,"Found class %s for variable '%s'\n",qPrint(yyextra->scopeName),qPrint(varName)));
      MemberName *vmn=Doxygen::memberNameLinkedMap->find(varName);
      if (vmn==nullptr)
      {
        int vi = 0;
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

static void generatePHPVariableLink(yyscan_t yyscanner,OutputCodeList &ol,const char *varName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString name(varName+7); // strip $this->
  name.prepend("$");
  DBG_CTX((stderr,"generatePHPVariableLink(%s) name=%s scope=%s\n",varName,qPrint(name),qPrint(yyextra->scopeName)));
  if (!getLink(yyscanner,yyextra->scopeName,name,ol,QCString(varName)))
  {
    codifyLines(yyscanner,varName);
  }
}

static void generateFunctionLink(yyscan_t yyscanner,OutputCodeList &ol,const QCString &funcName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //CodeClassDef *ccd=nullptr;
  QCString locScope=yyextra->scopeName;
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  if (yyextra->lang==SrcLangExt::PHP && locFunc.startsWith("self::")) locFunc=locFunc.mid(4);
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

static void generateFunctionLink(yyscan_t yyscanner,OutputCodeList &ol,const char *funcName)
{
  generateFunctionLink(yyscanner,ol,QCString(funcName));
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
    count++;
  }
  return count;
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
    yyextra->insideSpecialComment = false;
  }
}

static void startFontClass(yyscan_t yyscanner,const char *s,bool specialComment)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  if (specialComment)
  {
    yyextra->code->startSpecialComment();
    yyextra->insideSpecialComment = true;
  }
  yyextra->code->startFontClass(QCString(s));
  yyextra->currentFontClass=s;
}

//----------------------------------------------------------------------------

// recursively writes a linkified Objective-C method call
static void writeObjCMethodCall(yyscan_t yyscanner,ObjCCallCtx *ctx)
{
  if (ctx==nullptr) return;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  char c = 0;
  if (!ctx->methodName.isEmpty())
  {
    DBG_CTX((stderr,"writeObjCMethodCall(%s) obj=%s method=%s\n",
             qPrint(ctx->format),qPrint(ctx->objectTypeOrName),qPrint(ctx->methodName)));
    if (!ctx->objectTypeOrName.isEmpty() && ctx->objectTypeOrName.at(0)!='$')
    {
      DBG_CTX((stderr,"Looking for object=%s method=%s\n",qPrint(ctx->objectTypeOrName),
               qPrint(ctx->methodName)));
      const ScopedTypeVariant *stv = yyextra->theVarContext.findVariable(ctx->objectTypeOrName);
      if (stv!=nullptr) // local variable
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
    bool skip = false;
    char skipChar = ' ';
    while ((c=*p++)) // for each character in ctx->format
    {
      if (skip)
      {
        char s[2];
        s[0]=c;s[1]=0;
        codifyLines(yyscanner,s);
        if (c==skipChar)
        {
          skip = false;
          skipChar = ' ';
        }
        else if (c=='\\')
        {
          c = *p++;
          s[0]=c;
          codifyLines(yyscanner,s);
        }
      }
      else if (c=='$')
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
                  addDocCrossReference(yyextra->currentMemberDef,ctx->method);
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
          else if (nc=='o') // reference to potential object name, e.g. 'self', or 'self.var1' or 'self.var1.var2'
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
                  addDocCrossReference(yyextra->currentMemberDef,ctx->objectVar);
                }
              }
              else if (ctx->objectType && ctx->objectType->isLinkable()) // object is class name
              {
                const ClassDef *cd = ctx->objectType;
                writeMultiLineCodeLink(yyscanner,*yyextra->code,cd,object);
              }
              else // object still needs to be resolved
              {
                int pp=0;
                int dotPos=object.find('.',pp);
                int len = static_cast<int>(object.length());
                if (dotPos==-1) dotPos=len;
                const Definition *scope = yyextra->currentDefinition;
                //printf("initial scope=%s\n",scope?qPrint(scope->name()):"none");
                for (;;)
                {
                  QCString fragment = object.mid(pp,dotPos-pp);
                  QCString fragmentStripped = fragment.stripWhiteSpace();
                  //printf("fragment=%s\n",qPrint(fragment));
                  if (fragmentStripped=="self")
                  {
                    if (scope && scope->definitionType()==Definition::TypeClass)
                    {
                      scope = ctx->objectType = toClassDef(scope);
                    }
                    //printf("self scope=%s\n",scope?qPrint(scope->name()):"none");
                    startFontClass(yyscanner,"keyword");
                    codifyLines(yyscanner,fragment);
                    endFontClass(yyscanner);
                  }
                  else
                  {
                    const Definition *symbol = yyextra->symbolResolver.resolveSymbol(scope,fragmentStripped,QCString(),false,true);
                    //printf("scope=%s name=%s -> symbol=%s\n",scope?qPrint(scope->name()):"none",qPrint(fragment),
                    //                                         symbol?qPrint(symbol->name()):"none");
                    if (symbol && symbol->definitionType()==Definition::TypeClass)
                    {
                      scope = ctx->objectType = toClassDef(symbol);
                      ctx->method = yyextra->symbolResolver.getTypedef();
                    }
                    else if (symbol && symbol->definitionType()==Definition::TypeMember)
                    {
                      scope = ctx->objectType = stripClassName(yyscanner,toMemberDef(symbol)->typeString(),scope);
                    }
                    if (scope && scope!=Doxygen::globalScope)
                    {
                      writeMultiLineCodeLink(yyscanner,*yyextra->code,scope,fragment);
                    }
                    else
                    {
                      codifyLines(yyscanner,fragment);
                    }
                    //printf("new scope=%s\n",scope?qPrint(scope->name()):"none");
                  }
                  if (dotPos==len)
                  {
                    break;
                  }
                  else // find next fragment
                  {
                    codifyLines(yyscanner,".");
                    pp=dotPos+1;
                    dotPos=object.find('.',pp);
                    if (dotPos==-1) dotPos=len;
                  }
                }
                DBG_CTX((stderr,"  object is class? %p\n",(void*)ctx->objectType));
                if (ctx->objectType) // found class
                {
                  ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
                  DBG_CTX((stderr,"    yes->method=%s\n",ctx->method?qPrint(ctx->method->name()):"<none>"));
                }
                else if (ctx->method==nullptr) // search for class variable with the same name
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
              bool isKeyword = word=="self";
              if (isKeyword)
              {
                startFontClass(yyscanner,"keyword");
              }
              codifyLines(yyscanner,word);
              if (isKeyword)
              {
                endFontClass(yyscanner);
              }
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
            ASSERT(0); // "invalid escape sequence"
          }
        }
      }
      else if (c=='\'' || c == '"')
      {
        char s[2];
        s[0]=c;s[1]=0;
        codifyLines(yyscanner,s);
        skip = true;
        skipChar = c;
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
  yyextra->nameMap.emplace(yyextra->currentNameId,s);
  yyextra->currentNameId++;
  return result;
}

static QCString escapeObject(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$o%d",yyextra->currentObjId);
  yyextra->objectMap.emplace(yyextra->currentObjId,s);
  yyextra->currentObjId++;
  return result;
}

static QCString escapeWord(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$w%d",yyextra->currentWordId);
  yyextra->wordMap.emplace(yyextra->currentWordId,s);
  yyextra->currentWordId++;
  return result;
}

static QCString escapeComment(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$d%d",yyextra->currentCommentId);
  yyextra->commentMap.emplace(yyextra->currentCommentId,s);
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
    "remove", "self", "set", "transient",
    "sealed"
    };
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
    "signed", "sizeof", "static_assert", "_Static_assert", "static_cast", "struct",
    "template", "thread_local", "typedef", "typeid", "typename",
    "union", "unsigned", "using", "virtual", "wchar_t",
    "xor", "xor_eq", "override", "sealed"
  };
  bool retval = false;
  switch (yyextra->lang)
  {
    case SrcLangExt::Cpp:
      retval = (non_cpp_keywords.find(keyword) != non_cpp_keywords.end());
      break;
    case SrcLangExt::Java:
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

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t inputPosition = yyextra->inputPosition;
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
  newCtx->objectType = nullptr;
  newCtx->objectVar = nullptr;
  newCtx->method = nullptr;
  DBG_CTX((stderr,"save state=%d\n",YY_START));
  yyextra->currentCtx = newCtx.get();
  yyextra->contextMap.emplace(yyextra->currentCtxId,std::move(newCtx));
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
    yyextra->currentCtx = nullptr;
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
  codeYYset_debug(Debug::isFlagSet(Debug::Lex_code)?1:0,p->yyscanner);
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
  while (!yyextra->scopeNameLengthStack.empty()) yyextra->scopeNameLengthStack.pop_back();
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

void CCodeParser::parseCode(OutputCodeList &od,const QCString &scopeName,const QCString &s,
                SrcLangExt lang,bool stripCodeComments,bool exBlock,const QCString &exName,const FileDef *fd,
                int startLine,int endLine,bool inlineFragment,
                const MemberDef *memberDef,bool showLineNumbers,const Definition *searchCtx,
                bool collectXRefs)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"***parseCode() exBlock=%d exName=%s fd=%p scopeName=%s searchCtx=%s\n",
           exBlock,qPrint(exName),(void*)fd,qPrint(scopeName),searchCtx?qPrint(searchCtx->name()):"<none>"));

  if (s.isEmpty()) return;

  DebugLex debugLex(Debug::Lex_code, __FILE__, fd ? qPrint(fd->fileName()): !exName.isEmpty() ? qPrint(exName) : nullptr);

  od.stripCodeComments(stripCodeComments);
  yyextra->code = &od;
  yyextra->inputString   = s.data();
  yyextra->fileName      = fd ? fd->fileName():"";
  yyextra->absFileName   = fd ? fd->absFilePath():"";
  yyextra->inputPosition = 0;
  codeYYrestart(nullptr,yyscanner);
  yyextra->currentFontClass = nullptr;
  yyextra->searchCtx = searchCtx;
  yyextra->collectXRefs = collectXRefs;
  yyextra->inFunctionTryBlock = FALSE;
  yyextra->symbolResolver.setFileScope(fd);
  yyextra->foldStack.clear();
  yyextra->insideSpecialComment = false;

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
  yyextra->scopeName     = scopeName;
  DBG_CTX((stderr,"parseCCode %s\n",qPrint(scopeName)));
  yyextra->exampleBlock  = exBlock;
  yyextra->exampleName   = exName;
  yyextra->sourceFileDef = fd;
  yyextra->lineNumbers   = fd && showLineNumbers;
  if (fd==nullptr)
  {
    // create a dummy filedef for the example
    yyextra->exampleFileDef = createFileDef(QCString(),(!exName.isEmpty()?exName:"generated"));
    yyextra->sourceFileDef  = yyextra->exampleFileDef.get();
  }
  yyextra->lang        = lang;
  yyextra->insideObjC  = lang==SrcLangExt::ObjC;
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }
  yyextra->currentDefinition = searchCtx ? searchCtx : getResolvedNamespace(scopeName);
  yyextra->currentMemberDef = nullptr;
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
  yyextra->type.clear();
  yyextra->name.clear();
  yyextra->args.clear();
  yyextra->parmName.clear();
  yyextra->parmType.clear();
  if (memberDef) setParameterList(yyscanner,memberDef);
  BEGIN( Body );
  codeYYlex(yyscanner);
  yyextra->lexInit=TRUE;
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
  // write the tooltips
  yyextra->tooltipManager.writeTooltips(od);
}

#include "code.l.h"
