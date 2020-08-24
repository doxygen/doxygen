/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
}

%{

/*
 *	includes
 */

#include <memory>
#include <algorithm>

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>
#include <qdir.h>
#include <qstack.h>

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

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_UNISTD_H 1

#define CLASSBLOCK (int *)4
#define SCOPEBLOCK (int *)8
#define INNERBLOCK (int *)12

#define USE_STATE2STRING 0

/* -----------------------------------------------------------------
 *	statics
 */
  
// context for an Objective-C method call
struct ObjCCallCtx
{
  int id;
  QCString methodName;
  QCString objectTypeOrName;
  QGString comment;
  const ClassDef *objectType;
  const MemberDef *objectVar;
  const MemberDef *method;
  QCString format;
  int lexState;
  int braceCount;
};

/*! Represents a stack of variable to class mappings as found in the
 *  code. Each scope is enclosed in pushScope() and popScope() calls.
 *  Variables are added by calling addVariables() and one can search
 *  for variable using findVariable().
 */
class VariableContext
{
  public:
    static const ClassDef *dummyContext;
    
    class Scope : public SDict<ClassDef>
    {
      public:
	Scope() : SDict<ClassDef>(17) {}
    };
    
    VariableContext() 
    {
      m_scopes.setAutoDelete(TRUE);
    }
    virtual ~VariableContext()
    {
    }
    
    void pushScope()
    {
      m_scopes.append(new Scope);
      DBG_CTX((stderr,"** Push var context %d\n",m_scopes.count()));
    }

    void popScope()
    {
      if (m_scopes.count()>0)
      {
        DBG_CTX((stderr,"** Pop var context %d\n",m_scopes.count()));
	m_scopes.remove(m_scopes.count()-1);
      }
      else
      {
        DBG_CTX((stderr,"** ILLEGAL: Pop var context\n"));
      }
    }

    void clear()
    {
      m_scopes.clear();
      m_globalScope.clear();
    }

    void clearExceptGlobal()
    {
      DBG_CTX((stderr,"** Clear var context\n"));
      m_scopes.clear();
    }

    void addVariable(yyscan_t yyscanner,const QCString &type,const QCString &name);
    ClassDef *findVariable(const QCString &name);

    uint count() const { return m_scopes.count(); }
    
  private:
    Scope        m_globalScope;
    QList<Scope> m_scopes;
};

//-------------------------------------------------------------------

class CallContext
{
  public:
    struct Ctx
    {
      Ctx(QCString _name, QCString _type) : name(_name), type(_type), d(0) {}
      QCString name;
      QCString type;
      const Definition *d;
    };

    CallContext()
    {
      m_defList.append(new Ctx("",""));
      m_defList.setAutoDelete(TRUE);
    }
    virtual ~CallContext() {}
    void setScope(const Definition *d)
    {
      Ctx *ctx = m_defList.getLast();
      if (ctx)
      {
	DBG_CTX((stderr,"** Set call context %s (%p)\n",d==0 ? "<null>" : d->name().data(),d));
        ctx->d=d;
      }
    }
    void pushScope(QCString _name, QCString _type)
    {
      m_defList.append(new Ctx(_name,_type));
      DBG_CTX((stderr,"** Push call context %d\n",m_defList.count()));
    }
    void popScope(QCString &_name, QCString &_type)
    {
      if (m_defList.count()>1)
      {
        DBG_CTX((stderr,"** Pop call context %d\n",m_defList.count()));
	Ctx *ctx = m_defList.getLast();
	if (ctx)
	{
	  _name = ctx->name;
	  _type = ctx->type;
	}
	m_defList.removeLast();
      }
      else
      {
        DBG_CTX((stderr,"** ILLEGAL: Pop call context\n"));
      }
    }
    void clear()
    {
      DBG_CTX((stderr,"** Clear call context\n"));
      m_defList.clear();
      m_defList.append(new Ctx("",""));
    }
    const Definition *getScope() const
    {
      Ctx *ctx = m_defList.getLast();
      if (ctx) return ctx->d; else return 0;
    }

  private:
    QList<Ctx> m_defList;
};


struct codeYY_state
{
  CodeOutputInterface * code = 0;

  ClassSDict    *codeClassSDict = 0;
  QCString      curClassName;
  QStrList      curClassBases;

  QCString      parmType;
  QCString      parmName;

  const char *  inputString = 0;     //!< the code fragment as text
  yy_size_t     inputPosition = 0;   //!< read offset during parsing 
  int           inputLines = 0;      //!< number of line in the code fragment
  int	        yyLineNr = 0;        //!< current line number
  int	        yyColNr = 0;         //!< current column number
  bool          needsTermination = FALSE;

  bool          exampleBlock = FALSE;
  QCString      exampleName;
  QCString      exampleFile;

  bool          insideTemplate = FALSE;
  QCString      type;
  QCString      name;
  QCString      args;
  QCString      classScope;
  QCString      realScope;
  QStack<int>   scopeStack;      //!< 1 if bracket starts a scope, 
                                 //   2 for internal blocks 
  int           anchorCount = 0;
  FileDef *     sourceFileDef = 0;
  bool          lineNumbers = FALSE;
  Definition *  currentDefinition = 0;
  MemberDef *   currentMemberDef = 0;
  bool          includeCodeFragment = FALSE;
  const char *  currentFontClass = 0;
  bool          searchingForBody = FALSE;
  bool          insideBody = FALSE;
  int           bodyCurlyCount = 0;
  QCString      saveName;
  QCString      saveType;
  QCString      delimiter;

  int	        bracketCount = 0;
  int	        curlyCount   = 0;
  int	        sharpCount   = 0;
  bool          inFunctionTryBlock = FALSE;
  bool          inForEachExpression = FALSE;

  int           lastTemplCastContext = 0;
  int	        lastSpecialCContext = 0;
  int           lastStringContext = 0;
  int           lastSkipCppContext = 0;
  int           lastVerbStringContext = 0;
  int           lastObjCCallContext = 0;
  int           memCallContext = 0;
  int	        lastCContext = 0;
  int           skipInlineInitContext = 0;

  SrcLangExt    lang = SrcLangExt_Unknown;
  bool          insideObjC = FALSE;
  bool          insideProtocolList = FALSE;

  bool          lexInit = FALSE;

  QStack<int>   classScopeLengthStack;

  int           prefixed_with_this_keyword = FALSE;
  const Definition *searchCtx = 0;
  bool          collectXRefs = FALSE;

  ObjCCallCtx * currentCtx=0;
  int           currentCtxId=0;
  int           currentNameId=0;
  int           currentObjId=0;
  int           currentWordId=0;
  int           currentCommentId=0;
  QStack<ObjCCallCtx>   contextStack;
  QIntDict<ObjCCallCtx> contextDict;
  QIntDict<QCString>    nameDict;
  QIntDict<QCString>    objectDict;
  QIntDict<QCString>    wordDict;
  QIntDict<QCString>    commentDict;
  int           braceCount=0;

  QCString        forceTagReference;
  VariableContext theVarContext;
  CallContext     theCallContext;
};

static bool isCastKeyword(const QCString &s);

//-------------------------------------------------------------------
#if USE_STATE2STRING
static const char *stateToString(yyscan_t yyscanner,int state);
#endif

static void saveObjCContext(yyscan_t yyscanner);
static void restoreObjCContext(yyscan_t yyscanner);
static void addUsingDirective(yyscan_t yyscanner,const char *name);
static void pushScope(yyscan_t yyscanner,const char *s);
static void popScope(yyscan_t yyscanner);
static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void addToSearchIndex(yyscan_t yyscanner,const char *text);
static void setClassScope(yyscan_t yyscanner,const QCString &name);
static void startCodeLine(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void startFontClass(yyscan_t yyscanner,const char *s);
static void endFontClass(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const char *text);
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                                   const Definition *d,
                                   const char *text);
static void addType(yyscan_t yyscanner);
static void addParmType(yyscan_t yyscanner);
static void addUsingDirective(yyscan_t yyscanner,const char *name);
static void setParameterList(yyscan_t yyscanner,const MemberDef *md);
static const ClassDef *stripClassName(yyscan_t yyscanner,const char *s,Definition *d);
static MemberDef *setCallContextForVar(yyscan_t yyscanner,const QCString &name);
static void updateCallContextForSmartPointer(yyscan_t yyscanner);
static bool getLinkInScope(yyscan_t yyscanner,const QCString &c,  // scope
                           const QCString &m,  // member
			   const char *memberText, // exact text
			   CodeOutputInterface &ol,
			   const char *text,
			   bool varOnly=FALSE
			  );
static bool getLink(yyscan_t yyscanner,const char *className,
                    const char *memberName,
		    CodeOutputInterface &ol,
		    const char *text=0,
		    bool varOnly=FALSE);
static void generateClassOrGlobalLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *clName,
                                      bool typeOnly=FALSE,bool varOnly=FALSE);
static bool generateClassMemberLink(yyscan_t yyscanner,CodeOutputInterface &ol,MemberDef *xmd,const char *memName);
static bool generateClassMemberLink(yyscan_t yyscanner,CodeOutputInterface &ol,const Definition *def,const char *memName);
static void generateMemberLink(yyscan_t yyscanner,CodeOutputInterface &ol,const QCString &varName,
            const char *memName);
static void generatePHPVariableLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *varName);
static void generateFunctionLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *funcName);
static int countLines(yyscan_t yyscanner);
static void writeObjCMethodCall(yyscan_t yyscanner,ObjCCallCtx *ctx);
static QCString escapeName(yyscan_t yyscanner,const char *s);
static QCString escapeObject(yyscan_t yyscanner,const char *s);
static QCString escapeWord(yyscan_t yyscanner,const char *s);
static QCString escapeComment(yyscan_t yyscanner,const char *s);
static bool skipLanguageSpecificKeyword(yyscan_t yyscanner,const QCString &kw);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);


/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);


%}

B       [ \t]
BN      [ \t\n\r]
ID	"$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
SEP     ("::"|"\\")
SCOPENAME ({SEP}{BN}*)?({ID}{BN}*{SEP}{BN}*)*("~"{BN}*)?{ID}
TEMPLIST "<"[^\"\}\{\(\)\/\n\>]*">"
SCOPETNAME (((({ID}{TEMPLIST}?){BN}*)?{SEP}{BN}*)*)((~{BN}*)?{ID})
SCOPEPREFIX ({ID}{TEMPLIST}?{BN}*{SEP}{BN}*)+
KEYWORD_OBJC ("@public"|"@private"|"@protected"|"@class"|"@implementation"|"@interface"|"@end"|"@selector"|"@protocol"|"@optional"|"@required"|"@throw"|"@synthesize"|"@property")
KEYWORD ("asm"|"__assume"|"auto"|"class"|"const"|"delete"|"enum"|"explicit"|"extern"|"false"|"friend"|"gcnew"|"gcroot"|"set"|"get"|"inline"|"internal"|"mutable"|"namespace"|"new"|"null"|"nullptr"|"override"|"operator"|"pin_ptr"|"private"|"protected"|"public"|"raise"|"register"|"remove"|"self"|"sizeof"|"static"|"struct"|"__super"|"function"|"template"|"generic"|"this"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"abstract"|"final"|"import"|"synchronized"|"transient"|"alignas"|"alignof"|{KEYWORD_OBJC})
FLOWKW  ("break"|"catch"|"continue"|"default"|"do"|"else"|"finally"|"return"|"switch"|"throw"|"throws"|"@catch"|"@finally")
FLOWCONDITION  ("case"|"for"|"foreach"|"for each"|"goto"|"if"|"try"|"while"|"@try")
TYPEKW  ("bool"|"byte"|"char"|"double"|"float"|"int"|"long"|"object"|"short"|"signed"|"unsigned"|"void"|"wchar_t"|"size_t"|"boolean"|"id"|"SEL"|"string"|"nullptr")
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

  //Note same defines in commentcnv.l
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

%option noyywrap

%x      SkipString
%x      SkipStringS
%x      SkipVerbString
%x	SkipCPP
%x	SkipComment
%x	SkipCxxComment
%x	RemoveSpecialCComment
%x	StripSpecialCComment
%x	Body
%x      FuncCall
%x      MemberCall
%x      MemberCall2
%x      SkipInits
%x      ClassName
%x      AlignAs
%x      AlignAsEnd
%x      PackageName
%x      ClassVar
%x	CppCliTypeModifierFollowup
%x      Bases
%x      SkipSharp
%x      ReadInclude
%x      TemplDecl
%x      TemplCast
%x	CallEnd
%x      ObjCMethod
%x	ObjCParams
%x	ObjCParamType
%x      ObjCCall
%x      ObjCMName
%x      ObjCSkipStr
%x      ObjCCallComment
%x      OldStyleArgs
%x	UsingName
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
<Body>"property"|"event"/{BN}*			{ 
  					  if (yyextra->insideTemplate) REJECT;
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					}
<Body>(KEYWORD_CPPCLI_DATATYPE|("partial"{B}+)?"class"|"struct"|"union"|"namespace"|"interface"){B}+ { 
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  					  if (!yyextra->insideTemplate) 
					    BEGIN( ClassName ); 
					}
<Body>("package")[ \t\n]+ 		{ 
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  BEGIN( PackageName ); 
					}
<ClassVar>\n				{
  					  if (!yyextra->insideObjC) REJECT;
  					  codifyLines(yyscanner,yytext);
					  BEGIN(Body);
  					}
<Body,ClassVar,Bases>"-"|"+"		{
					  if (!yyextra->insideObjC || yyextra->insideBody)
					  { 
  					    yyextra->code->codify(yytext);
					  }
					  else // Start of Objective-C method
					  {
					    //printf("Method!\n");
  					    yyextra->code->codify(yytext);
					    BEGIN(ObjCMethod);
					  }
  					}
<ObjCMethod>":"				{
  					  yyextra->code->codify(yytext);
					  BEGIN(ObjCParams);
  					}
<ObjCParams>"("				{
  					  yyextra->code->codify(yytext);
  					  BEGIN(ObjCParamType);
					}
<ObjCParams,ObjCMethod>";"|"{"		{
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
<ObjCParams>{ID}{B}*":"			{
  					  yyextra->code->codify(yytext);
  					}
<ObjCParamType>{TYPEKW} 		{
  					  startFontClass(yyscanner,"keywordtype");
					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
  					  yyextra->parmType=yytext;
  					}
<ObjCParamType>{ID}			{
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
  					  yyextra->parmType=yytext;
  					}
<ObjCParamType>")"			{
  					  yyextra->code->codify(yytext);
  					  BEGIN(ObjCParams);
  					}
<ObjCParams>{ID}			{
  					  yyextra->code->codify(yytext);
  					  yyextra->parmName=yytext;
					  yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
					  yyextra->parmType.resize(0);yyextra->parmName.resize(0);
  					}
<ObjCMethod,ObjCParams,ObjCParamType>{ID} {
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
  					}
<ObjCMethod,ObjCParams,ObjCParamType>.	{
  					  yyextra->code->codify(yytext);
  					}
<ObjCMethod,ObjCParams,ObjCParamType>\n	{
  					  codifyLines(yyscanner,yytext);
  					}
<ReadInclude>[^\n\"\>]+/(">"|"\"")  	{
					  //FileInfo *f;
					  bool ambig;
					  bool found=FALSE;
					  //QCString absPath = yytext;
					  //if (yyextra->sourceFileDef && QDir::isRelativePath(absPath))
					  //{
					  //  absPath = QDir::cleanDirPath(yyextra->sourceFileDef->getPath()+"/"+absPath);
					  //}

                                          const FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,yytext,ambig);
					  //printf("looking for include %s -> %s fd=%p\n",yytext,absPath.data(),fd);
					  if (fd && fd->isLinkable())
					  {
					    if (ambig) // multiple input files match the name
					    {
					      //printf("===== yes %s is ambiguous\n",yytext);
					      QCString name = QDir::cleanDirPath(yytext).utf8();
					      if (!name.isEmpty() && yyextra->sourceFileDef)
					      {
					        FileName *fn = Doxygen::inputNameLinkedMap->find(name);
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
					  //printf("      include file %s found=%d\n",fd ? fd->absFilePath().data() : "<none>",found);
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
<Body,Bases>^[ \t]*"#"			{ 
  					  startFontClass(yyscanner,"preprocessor");
					  yyextra->lastSkipCppContext = YY_START;
  					  yyextra->code->codify(yytext);
  					  BEGIN( SkipCPP ) ; 
					}
<SkipCPP>.				{ 
  					  yyextra->code->codify(yytext);
					}
<SkipCPP>[^\n\/\\]+			{
  					  yyextra->code->codify(yytext);
  					}
<SkipCPP>\\[\r]?\n			{ 
  					  codifyLines(yyscanner,yytext);
					}
<SkipCPP>"//"				{ 
  					  yyextra->code->codify(yytext);
					}
<Body,FuncCall>"{"			{ 
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

					  int *scope = yyextra->scopeStack.pop();
                                          DBG_CTX((stderr,"** scope stack pop SCOPEBLOCK=%d\n",scope==SCOPEBLOCK));
  					  if (scope==SCOPEBLOCK || scope==CLASSBLOCK) 
					  {
					    popScope(yyscanner);
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
<Body,ClassVar>"@end"			{ 
  					  //printf("End of objc scope fd=%s\n",yyextra->sourceFileDef->name().data());
                                          if (yyextra->sourceFileDef)
					  {
					    FileDef *fd=yyextra->sourceFileDef;
                                            yyextra->insideObjC = fd->name().lower().right(2)==".m" || 
                                                           fd->name().lower().right(3)==".mm"; 
					    //printf("insideObjC=%d\n",yyextra->insideObjC);
					  }
					  else
					  {
					    yyextra->insideObjC = FALSE;
					  }
					  if (yyextra->insideBody)
					  {
                                            yyextra->theVarContext.popScope();

					    int *scope = yyextra->scopeStack.pop();
                                            DBG_CTX((stderr,"** scope stack pop SCOPEBLOCK=%d\n",scope==SCOPEBLOCK));
  					    if (scope==SCOPEBLOCK || scope==CLASSBLOCK) 
					    {
					      popScope(yyscanner);
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
<ClassName,ClassVar>";"			{ 
  					  yyextra->code->codify(yytext);
					  yyextra->searchingForBody=FALSE; 
  					  BEGIN( Body ); 
					}
<ClassName,ClassVar>[*&^%]+       	{
  					  yyextra->type=yyextra->curClassName.copy();
  					  yyextra->name.resize(0);
					  yyextra->code->codify(yytext);
					  BEGIN( Body ); // variable of type struct *
					}
<ClassName>"__declspec"{B}*"("{B}*{ID}{B}*")"	{
					  startFontClass(yyscanner,"keyword");
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
					}
<ClassName>{ID}("."{ID})*	        |
<ClassName>{ID}("::"{ID})*	        {
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
<ClassName>{ID}("\\"{ID})*		{ // PHP namespace
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
<PackageName>{ID}("."{ID})*		{
					  yyextra->curClassName=substitute(yytext,".","::");
					  //printf("found package: %s\n",yyextra->curClassName.data());
					  addType(yyscanner);
					  codifyLines(yyscanner,yytext);
  					}
<ClassVar>"="				{
					  unput(*yytext);
					  BEGIN( Body );
  					}
<ClassVar>("extends"|"implements")	{ // Java, Slice
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  yyextra->curClassBases.clear();
  					  BEGIN( Bases ); 
					}
<ClassVar>("sealed"|"abstract")/{BN}*(":"|"{") {
					  DBG_CTX((stderr,"***** C++/CLI modifier %s on yyextra->curClassName=%s\n",yytext,yyextra->curClassName.data()));
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  					  BEGIN( CppCliTypeModifierFollowup ); 
					}
<ClassVar>{ID}				{
  					  yyextra->type = yyextra->curClassName.copy();
					  yyextra->name = yytext;
					  if (yyextra->insideBody)
					  {
					    yyextra->theVarContext.addVariable(yyscanner,yyextra->type,yyextra->name);
					  }
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
  					}
<ClassName,ClassVar,CppCliTypeModifierFollowup>{B}*":"{B}*	{
  					  codifyLines(yyscanner,yytext);
					  yyextra->curClassBases.clear();
  					  BEGIN( Bases ); 
					}
<PackageName>[ \t]*";"			|
<Bases>^{B}*/"@"{ID}                    | // Objective-C interface
<Bases,ClassName,ClassVar,CppCliTypeModifierFollowup>{B}*"{"{B}* {
                                          yyextra->theVarContext.pushScope();
  					  yyextra->code->codify(yytext);
					  if (YY_START==ClassVar && yyextra->curClassName.isEmpty())
					  {
					    yyextra->curClassName = yyextra->name.copy();
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
					    DBG_CTX((stderr,"***** yyextra->curClassName=%s\n",yyextra->curClassName.data()));
					    if (getResolvedClass(yyextra->currentDefinition,yyextra->sourceFileDef,yyextra->curClassName)==0)
					    {
					      DBG_CTX((stderr,"Adding new class %s\n",yyextra->curClassName.data()));
					      ClassDef *ncd=createClassDef("<code>",1,1,
				 		  yyextra->curClassName,ClassDef::Class,0,0,FALSE);
					      yyextra->codeClassSDict->append(yyextra->curClassName,ncd);
					      // insert base classes.
					      char *s=yyextra->curClassBases.first();
					      while (s)
					      {
						const ClassDef *bcd=yyextra->codeClassSDict->find(s);
						if (bcd==0) bcd=getResolvedClass(yyextra->currentDefinition,yyextra->sourceFileDef,s);
						if (bcd && bcd!=ncd)
						{
						  ncd->insertBaseClass(const_cast<ClassDef*>(bcd),s,Public,Normal);
						}
						s=yyextra->curClassBases.next();
					      }
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
					  DBG_CTX((stderr,"%s:addBase(%s)\n",yyextra->curClassName.data(),yytext));
  					  yyextra->curClassBases.inSort(yytext); 
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
<Bases>">"				{
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
  					

<Body>{SCOPEPREFIX}?"operator"{B}*"()"{B}*/"(" {
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
<Body,TemplDecl>("template"|"generic")/([^a-zA-Z0-9])		{
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  yyextra->insideTemplate=TRUE;
					  yyextra->sharpCount=0;
					}
<Body>"using"{BN}+"namespace"{BN}+	{
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  BEGIN(UsingName);
  					}
<UsingName>{ID}("::"{ID})*		{ addUsingDirective(yyscanner,yytext);
 					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                          DBG_CTX((stderr,"** scope stack push CLASSBLOCK\n"));
  					  yyextra->scopeStack.push(CLASSBLOCK);
					  pushScope(yyscanner,yytext);
					  BEGIN(Body);
                                        }
<UsingName>\n				{ codifyLines(yyscanner,yytext); BEGIN(Body); }
<UsingName>.				{ codifyLines(yyscanner,yytext); BEGIN(Body); }
<Body,FuncCall>"$"?"this"("->"|".")	{ yyextra->code->codify(yytext); // this-> for C++, this. for C#
					  yyextra->prefixed_with_this_keyword = TRUE;
                                        }
<Body>{KEYWORD}/([^a-z_A-Z0-9]) 	{
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
<Body>{KEYWORD}/{B}* 			{
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  					}
<Body>{KEYWORD}/{BN}*"(" 		{
                                          if (skipLanguageSpecificKeyword(yyscanner,yytext)) REJECT;
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  				          yyextra->name.resize(0);yyextra->type.resize(0);
  					}
<FuncCall>"in"/{BN}*			{
					  if (!yyextra->inForEachExpression) REJECT;
  					  startFontClass(yyscanner,"keywordflow");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  // insert the variable in the parent scope, see bug 546158
					  yyextra->theVarContext.popScope();
					  yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
					  yyextra->theVarContext.pushScope();
  				          yyextra->name.resize(0);yyextra->type.resize(0);
					}
<Body>{FLOWKW}/{BN}*"(" 			{
  					  startFontClass(yyscanner,"keywordflow");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  				          yyextra->name.resize(0);yyextra->type.resize(0);
					  yyextra->inForEachExpression = (qstrcmp(yytext,"for each")==0 || qstrcmp(yytext, "foreach")==0);
					  BEGIN(FuncCall);
  					}
<Body>{FLOWCONDITION}/{BN}*"("          {
                                          if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                          {
                                            yyextra->currentMemberDef->incrementFlowKeyWordCount();
                                          }
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yyextra->name.resize(0);yyextra->type.resize(0);
                                          yyextra->inForEachExpression = (strcmp(yytext,"for each")==0 || strcmp(yytext, "foreach")==0);
                                          BEGIN(FuncCall);
                                        }
<Body>{FLOWKW}/([^a-z_A-Z0-9]) 		{
  					  startFontClass(yyscanner,"keywordflow");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  if (yyextra->inFunctionTryBlock && (qstrcmp(yytext,"catch")==0 || qstrcmp(yytext,"finally")==0))
					  {
					    yyextra->inFunctionTryBlock=FALSE;
					  }
  					}
<Body>{FLOWCONDITION}/([^a-z_A-Z0-9]) 	{
                                          if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                          {
                                            yyextra->currentMemberDef->incrementFlowKeyWordCount();
                                          }
                                          startFontClass(yyscanner,"keywordflow");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          if (yyextra->inFunctionTryBlock && (strcmp(yytext,"catch")==0 || strcmp(yytext,"finally")==0))
                                          {
                                            yyextra->inFunctionTryBlock=FALSE;
                                          }
                                        }
<Body>{FLOWKW}/{B}* 			{
  					  startFontClass(yyscanner,"keywordflow");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  					}
<Body>{FLOWCONDITION}/{B}*              {
                                          if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                          {
                                            yyextra->currentMemberDef->incrementFlowKeyWordCount();
                                          }
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
<Body>[\\|\)\+\-\/\%\~\!]		{
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
<TemplDecl>"class"|"typename"		{
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  					}
<TemplDecl>"<"				{
                                          yyextra->code->codify(yytext);
                                          yyextra->sharpCount++;
  					}
<TemplDecl>">"				{
                                          yyextra->code->codify(yytext);
                                          yyextra->sharpCount--;
					  if (yyextra->sharpCount<=0)
					  {
					    BEGIN(Body);
					  }
  					}
<TemplCast>">"				{
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					  BEGIN( yyextra->lastTemplCastContext );
  					}
<TemplCast>{ID}("::"{ID})*		{
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
					}
<TemplCast>("const"|"volatile"){B}*	{
  					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
					}
<TemplCast>[*^]*			{
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
<Body,TemplCast>{SCOPENAME}{B}*"<"[^\n\/\-\.\{\"\>\(]*">"("::"{ID})*/{B}* { // A<T> *pt;
					  if (isCastKeyword(yytext) && YY_START==Body)
					  {
					    REJECT;
					  }
					  addType(yyscanner);
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
  					  yyextra->name+=yytext; 
					}
<Body>{SCOPENAME}/{BN}*[:;,)\]]		{ // "int var;" or "var, var2" or "debug(f) macro" , or int var : 5;
					  addType(yyscanner);
					  // changed this to generateFunctionLink, see bug 624514
					  //generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,FALSE,TRUE);
					  generateFunctionLink(yyscanner,*yyextra->code,yytext);
  					  yyextra->name+=yytext; 
					}
<Body>{SCOPENAME}/{B}* 			{ // p->func()
					  addType(yyscanner);
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
  					  yyextra->name+=yytext; 
					}
<Body>"("{B}*("*"{B}*)+{SCOPENAME}+{B}*")"/{B}*	{  // (*p)->func() but not "if (p) ..."
					  yyextra->code->codify(yytext);
					  uint s=0;while (s<(uint)yyleng && !isId(yytext[s])) s++;
                                          uint e=(uint)yyleng-1;while (e>1 && !isId(yytext[e])) e--;
					  QCString varname = ((QCString)yytext).mid(s,e-s+1); 
					  addType(yyscanner);
  					  yyextra->name=varname; 
					}
<Body>{SCOPETNAME}{B}*"<"[^\n\/\-\.\{\"\>]*">"/{BN}*"(" |
<Body>{SCOPETNAME}/{BN}*"("		{ // a() or c::a() or t<A,B>::a() or A\B\foo()
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
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>{RAWBEGIN}	{
                                          QCString text=yytext;
                                          uint i=(uint)text.find('R');
                                          yyextra->code->codify(text.left(i+1));
					  startFontClass(yyscanner,"stringliteral");
  					  yyextra->code->codify(yytext+i+1);
  					  yyextra->lastStringContext=YY_START;
					  yyextra->inForEachExpression = FALSE;
                                          yyextra->delimiter = yytext+i+2;
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
  					  BEGIN( RawString );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>\"	{
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
<FuncCall,Body,MemberCall,MemberCall2,SkipInits,InlineInit>\'	{
					  startFontClass(yyscanner,"stringliteral");
  					  yyextra->code->codify(yytext);
  					  yyextra->lastStringContext=YY_START;
					  yyextra->inForEachExpression = FALSE;
  					  BEGIN( SkipStringS );
  					}
<SkipString>[^\"\\\r\n]*		{ 
  					  yyextra->code->codify(yytext);
					}
<SkipStringS>[^\'\\\r\n]*		{
  					  yyextra->code->codify(yytext);
  					}
<SkipString,SkipStringS>"//"|"/*"	{
  					  yyextra->code->codify(yytext);
  					}
<SkipString>@?\"			{
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
  					  BEGIN( yyextra->lastStringContext );
  					}
<SkipStringS>\'				{
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
  					  BEGIN( yyextra->lastStringContext );
  					}
<SkipString,SkipStringS>\\.		{
  					  yyextra->code->codify(yytext);
					}
<RawString>{RAWEND}                     { 
                                          yyextra->code->codify(yytext);
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==yyextra->delimiter)
                                          {
					    BEGIN( yyextra->lastStringContext );
                                          }
                                        }
<RawString>[^)\n]+                      { yyextra->code->codify(yytext); }
<RawString>.                            { yyextra->code->codify(yytext); }
<RawString>\n                           { codifyLines(yyscanner,yytext); }
<SkipVerbString>[^"\n]+			{
  					  yyextra->code->codify(yytext);
					}
<SkipVerbString>\"\"			{ // escaped quote
  					  yyextra->code->codify(yytext);
					}
<SkipVerbString>\"			{ // end of string
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
					  BEGIN( yyextra->lastVerbStringContext );
					}
<SkipVerbString>.			{
  					  yyextra->code->codify(yytext);
  					}
<SkipVerbString>\n			{
  					  codifyLines(yyscanner,yytext);
  					}
<Body>":"				{
  					  yyextra->code->codify(yytext);
  					  yyextra->name.resize(0);yyextra->type.resize(0);
  					}
<Body>"<"				{
  					  if (yyextra->insideTemplate)
					  {
					    yyextra->sharpCount++;
					  }
  					  yyextra->code->codify(yytext);
  					}
<Body>">"				{
  					  if (yyextra->insideTemplate)
					  {
					    if (--yyextra->sharpCount<=0)
					    {
					      yyextra->insideTemplate=FALSE;
					    }
					  }
  					  yyextra->code->codify(yytext);
  					}
<Body,MemberCall,MemberCall2,FuncCall>"'"((\\0[Xx0-9]+)|(\\.)|(.))"'"	{
  					  startFontClass(yyscanner,"charliteral"); 
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
  					}
<Body>"."|"->"				{ 
  				          if (yytext[0]=='-') // -> could be overloaded
					  {
					    updateCallContextForSmartPointer(yyscanner);
					  }
  					  yyextra->code->codify(yytext);
					  yyextra->memCallContext = YY_START;
  					  BEGIN( MemberCall ); 
					}
<MemberCall>{SCOPETNAME}/{BN}*"(" 	{
					  if (yyextra->theCallContext.getScope())
					  {
					    if (!generateClassMemberLink(yyscanner,*yyextra->code,yyextra->theCallContext.getScope(),yytext))
					    {
					      yyextra->code->codify(yytext);
					      addToSearchIndex(yyscanner,yytext);
					    }
  					    yyextra->name.resize(0);
					  }
					  else
					  {
  					    yyextra->code->codify(yytext);
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
<MemberCall>{SCOPENAME}/{B}*		{
					  if (yyextra->theCallContext.getScope())
					  {
					    DBG_CTX((stderr,"yyextra->theCallContext.getClass()=%p\n",yyextra->theCallContext.getScope()));
					    if (!generateClassMemberLink(yyscanner,*yyextra->code,yyextra->theCallContext.getScope(),yytext))
					    {
					      yyextra->code->codify(yytext);
					      addToSearchIndex(yyscanner,yytext);
					    }
  					    yyextra->name.resize(0);
					  }
					  else
					  {
					    DBG_CTX((stderr,"no class context!\n"));
  					    yyextra->code->codify(yytext);
					    addToSearchIndex(yyscanner,yytext);
  					    yyextra->name.resize(0);
					  }
					  yyextra->type.resize(0);
  					  BEGIN(yyextra->memCallContext);
  					}
<Body>[,=;\[]				{
					  if (yyextra->insideObjC && *yytext=='[')
					  {
					    //printf("Found start of ObjC call!\n");
					    // start of a method call
					    yyextra->contextDict.setAutoDelete(TRUE);
					    yyextra->nameDict.setAutoDelete(TRUE);
					    yyextra->objectDict.setAutoDelete(TRUE);
					    yyextra->wordDict.setAutoDelete(TRUE);
                                            yyextra->commentDict.setAutoDelete(TRUE);
					    yyextra->contextDict.clear();
					    yyextra->nameDict.clear();
					    yyextra->objectDict.clear();
					    yyextra->wordDict.clear();
                                            yyextra->commentDict.clear();
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
					    yyextra->saveName = yyextra->name.copy();
					    yyextra->saveType = yyextra->type.copy();
					    if (*yytext!='[' && !yyextra->type.isEmpty()) 
					    {
					      //printf("yyextra->scopeStack.bottom()=%p\n",yyextra->scopeStack.bottom());
					      //if (yyextra->scopeStack.top()!=CLASSBLOCK) // commented out for bug731363
					      {
						//printf("AddVariable: '%s' '%s' context=%d\n",
						//    yyextra->type.data(),yyextra->name.data(),yyextra->theVarContext.count());
					        yyextra->theVarContext.addVariable(yyscanner,yyextra->type,yyextra->name);
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
  /*
<ObjCMemberCall>{ID}			{
  					  if (qstrcmp(yytext,"self")==0 || qstrcmp(yytext,"super")==0)
					  {
					    // TODO: get proper base class for "super"
					    yyextra->theCallContext.setClass(getClass(yyextra->curClassName));
					    startFontClass(yyscanner,"keyword");
					    yyextra->code->codify(yytext); 
					    endFontClass(yyscanner);
					  }
					  else
					  {
					    generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
					  }
					  yyextra->name.resize(0);
					  BEGIN(ObjCMemberCall2);
  					}
<ObjCMemberCall>"["			{
					    yyextra->code->codify(yytext);
					    yyextra->theCallContext.pushScope(yyscanner,yyextra->name, yyextra->type);
  					}
<ObjCMemberCall2>{ID}":"?		{
  					  yyextra->name+=yytext;
					  if (yyextra->theCallContext.getClass())
					  {
					    //printf("Calling method %s\n",yyextra->name.data());
					    if (!generateClassMemberLink(yyscanner,*yyextra->code,yyextra->theCallContext.getClass(),yyextra->name))
					    {
  					      yyextra->code->codify(yytext);
					      addToSearchIndex(yyscanner,yyextra->name);
					    }
					  }
					  else
					  {
  					    yyextra->code->codify(yytext);
					    addToSearchIndex(yyscanner,yyextra->name);
					  }
  					  yyextra->name.resize(0);
					  BEGIN(ObjCMemberCall3);
  					}
<ObjCMemberCall2,ObjCMemberCall3>"]"	{
					  yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
  					  yyextra->code->codify(yytext);
					  BEGIN(Body);
  					}
  */
<ObjCCall,ObjCMName>"["|"{"       {
                                    saveObjCContext(yyscanner);
			            yyextra->currentCtx->format+=*yytext;
			            BEGIN(ObjCCall);
		                    //printf("open\n");
                                  }
<ObjCCall,ObjCMName>"]"|"}"       {
			            yyextra->currentCtx->format+=*yytext;
                                    restoreObjCContext(yyscanner);
			            BEGIN(ObjCMName);
		             	    if (yyextra->currentCtx==0)
				    {
				      // end of call
				      writeObjCMethodCall(yyscanner,yyextra->contextDict.find(0));
				      BEGIN(Body);
				    }
			            //printf("close\n");
                                  }
<ObjCCall,ObjCMName>"//".*        {
                                    yyextra->currentCtx->format+=escapeComment(yyscanner,yytext);
                                  }
<ObjCCall,ObjCMName>"/*"          {
                                    yyextra->lastObjCCallContext = YY_START;
                                    yyextra->currentCtx->comment=yytext;
                                    BEGIN(ObjCCallComment);
                                  }
<ObjCCallComment>"*/"		  {
                                    yyextra->currentCtx->comment+=yytext;
                                    yyextra->currentCtx->format+=escapeComment(yyscanner,yyextra->currentCtx->comment);
  			  	    BEGIN(yyextra->lastObjCCallContext);
				  }
<ObjCCallComment>[^*\n]+          { yyextra->currentCtx->comment+=yytext; }
<ObjCCallComment>"//"|"/*"        { yyextra->currentCtx->comment+=yytext; }
<ObjCCallComment>\n               { yyextra->currentCtx->comment+=*yytext; }
<ObjCCallComment>.                { yyextra->currentCtx->comment+=*yytext; }
<ObjCCall>{ID}	                  {
                                    yyextra->currentCtx->format+=escapeObject(yyscanner,yytext);
			            if (yyextra->braceCount==0)
			            {
			              yyextra->currentCtx->objectTypeOrName=yytext;
                                      //printf("new type=%s\n",yyextra->currentCtx->objectTypeOrName.data());
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
<ObjCSkipStr>\\.	           { yyextra->currentCtx->format+=yytext; }
<ObjCSkipStr>"\""	           { yyextra->currentCtx->format+=yytext; 
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
<ObjCSkipStr>"@"/"\""		   { // needed to prevent matching the global rule (for C#)
                                     yyextra->currentCtx->format+=yytext;
                                   }
<ObjCCall,ObjCMName,ObjCSkipStr>{ID} { yyextra->currentCtx->format+=escapeWord(yyscanner,yytext); }
<ObjCCall,ObjCMName,ObjCSkipStr>.  { yyextra->currentCtx->format+=*yytext; }
<ObjCCall,ObjCMName,ObjCSkipStr>\n { yyextra->currentCtx->format+=*yytext; }

<Body>"]"				{
					  yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
  					  yyextra->code->codify(yytext);
					  // TODO: nested arrays like: a[b[0]->func()]->func()
					  yyextra->name = yyextra->saveName.copy();
					  yyextra->type = yyextra->saveType.copy();
					}
<Body>[0-9]+				{
					  yyextra->code->codify(yytext);
					}
<Body>[0-9]+[xX][0-9A-Fa-f]+		{
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
                                          if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                          {
                                            yyextra->currentMemberDef->incrementFlowKeyWordCount();
                                          }
                                          addParmType(yyscanner);
                                          yyextra->parmName=yytext;
                                          startFontClass(yyscanner,"keywordflow");
                                          yyextra->code->codify(yytext);
                                          endFontClass(yyscanner);
                                        }
<MemberCall2,FuncCall>{ID}(({B}*"<"[^\n\[\](){}<>]*">")?({B}*"::"{B}*{ID})?)* {
					  if (isCastKeyword(yytext))
					  {
					    REJECT;
					  }
					  addParmType(yyscanner);
					  yyextra->parmName=yytext; 
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,!yyextra->insideBody);
					}
<FuncCall>";"				{ // probably a cast, not a function call
  					  yyextra->code->codify(yytext);
					  yyextra->inForEachExpression = FALSE;
					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>,			{
  					  yyextra->code->codify(yytext);
					  yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
					  yyextra->parmType.resize(0);yyextra->parmName.resize(0);
					}
<MemberCall2,FuncCall>"{"		{
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
<MemberCall2,FuncCall>"("		{
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
<MemberCall,MemberCall2,FuncCall>("*"{B}*)?")"	{ 
                                          if (yytext[0]==')') // no a pointer cast
                                          {
                                            //printf("addVariable(%s,%s)\n",yyextra->parmType.data(),yyextra->parmName.data());
                                            if (yyextra->parmType.isEmpty())
                                            {
                                              yyextra->parmType=yyextra->parmName;
                                              yyextra->parmName.resize(0);
                                            }
					    yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
                                          }
                                          else
                                          {
                                            yyextra->parmType = yyextra->parmName;
                                            yyextra->parmName.resize(0);
					    yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
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
<CallEnd>[ \t\n]*			{ codifyLines(yyscanner,yytext); }
  /*
<MemberCall2,FuncCall>")"[ \t\n]*[;:]	{
  */
<CallEnd>[;:]				{
  					  codifyLines(yyscanner,yytext);
  					  yyextra->bracketCount=0;
					  if (*yytext==';') yyextra->searchingForBody=FALSE; 
					  if (!yyextra->type.isEmpty())
					  {
					    DBG_CTX((stderr,"add variable yyextra->type=%s yyextra->name=%s)\n",yyextra->type.data(),yyextra->name.data()));
					    yyextra->theVarContext.addVariable(yyscanner,yyextra->type,yyextra->name);
					  }
					  yyextra->parmType.resize(0);yyextra->parmName.resize(0);
					  yyextra->theCallContext.setScope(0);
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
<CallEnd>("const"|"volatile"|"sealed"|"override")({BN}+("const"|"volatile"|"sealed"|"override"))*/{BN}*(";"|"="|"throw"{BN}*"(") {
					  startFontClass(yyscanner,"keyword");
  					  codifyLines(yyscanner,yytext);
					  endFontClass(yyscanner);
  					}
<CallEnd,OldStyleArgs>("const"|"volatile"|"sealed"|"override")*({BN}+("const"|"volatile"|"sealed"|"override"))*{BN}*"{" {
                                          if (yyextra->insideBody)
					  {
					    yyextra->theVarContext.pushScope();
					  }
					  yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
					  //yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
					  yyextra->parmType.resize(0);yyextra->parmName.resize(0);
					  int index = yyextra->name.findRev("::");
					  DBG_CTX((stderr,"yyextra->name=%s\n",yyextra->name.data()));
					  if (index!=-1) 
					  {
					    QCString scope = yyextra->name.left((uint)index);
					    if (!yyextra->classScope.isEmpty()) scope.prepend(yyextra->classScope+"::");
					    const ClassDef *cd=getResolvedClass(Doxygen::globalScope,yyextra->sourceFileDef,scope);
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
<CallEnd>"try"				{ // function-try-block
					  startFontClass(yyscanner,"keyword");
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
					  yyextra->inFunctionTryBlock=TRUE;
                                        }
<CallEnd>{ID}				{
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
<OldStyleArgs>{ID}			{
					  addParmType(yyscanner);
					  yyextra->parmName=yytext; 
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext,!yyextra->insideBody);
  					}
<OldStyleArgs>[,;]			{
  					  yyextra->code->codify(yytext);
					  yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
					  if (*yytext==';') yyextra->parmType.resize(0);
					  yyextra->parmName.resize(0);
  					}
<CallEnd,OldStyleArgs>"#"		{
  					  startFontClass(yyscanner,"preprocessor");
					  yyextra->lastSkipCppContext = Body;
  					  yyextra->code->codify(yytext);
					  BEGIN( SkipCPP );
  					}
<CallEnd>.				{
  					  unput(*yytext);
                                          if (!yyextra->insideBody) 
					  {
					    yyextra->theVarContext.popScope();
					  }
					  yyextra->name.resize(0);yyextra->args.resize(0);
					  yyextra->parmType.resize(0);yyextra->parmName.resize(0);
					  BEGIN( Body ); 
  					}
<SkipInits>";"				{
  					  yyextra->code->codify(yytext);
  					  yyextra->type.resize(0); yyextra->name.resize(0);
  					  BEGIN( Body );
  					}
<SkipInits>"{"				{ 
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
<SkipInits>{ID}				{
					  generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
  					}
<FuncCall>{ID}/"("			{
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
<MemberCall2>"->"|"."			{
  				          if (yytext[0]=='-') // -> could be overloaded
					  {
					    updateCallContextForSmartPointer(yyscanner);
					  }
  					  yyextra->code->codify(yytext);
					  yyextra->memCallContext = YY_START;
  					  BEGIN( MemberCall ); 
  					}
<SkipComment>"/*"("!"?)"*/"		{ 
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
  					  BEGIN( yyextra->lastCContext ) ; 
					}
<SkipComment>"//"|"/*"			{
  					  yyextra->code->codify(yytext);
  					}
<SkipComment>[^*/\n]+			{
  					  yyextra->code->codify(yytext);
  					}
<SkipComment>[ \t]*"*/"			{ 
  					  yyextra->code->codify(yytext);
					  endFontClass(yyscanner);
  					  if (yyextra->lastCContext==SkipCPP)
                                          {
                                            startFontClass(yyscanner,"preprocessor");
                                          }
  					  BEGIN( yyextra->lastCContext ) ; 
					}
<SkipCxxComment>[^\r\n]*"\\"[\r]?\n	{ // line continuation
  					  codifyLines(yyscanner,yytext);
					}
<SkipCxxComment>[^\r\n]+		{ 
  					  yyextra->code->codify(yytext);
					}
<SkipCxxComment>\r			
<SkipCxxComment>\n			{
  					  unput('\n');
					  endFontClass(yyscanner);
					  BEGIN( yyextra->lastCContext ) ;
  					}
<SkipCxxComment>.			{
  					  yyextra->code->codify(yytext);
  					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)*({B}*(("//@"[{}])|("/*@"[{}]"*/")){B}*\n)?{B}*"/*"[*!]/[^/*] {
  					  yyextra->yyLineNr+=QCString(yytext).contains('\n');
					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)*({B}*(("//@"[{}])|("/*@"[{}]"*/")){B}*\n)? {
  					  yyextra->yyLineNr+=QCString(yytext).contains('\n');
					  nextCodeLine(yyscanner);
					  if (yyextra->lastSpecialCContext==SkipCxxComment)
					  { // force end of C++ comment here
					    endFontClass(yyscanner);
					    BEGIN( yyextra->lastCContext ) ;
					  }
					  else
					  {
  					    BEGIN(yyextra->lastSpecialCContext);
					  }
  					}
<RemoveSpecialCComment>"*/"		{
  					  BEGIN(yyextra->lastSpecialCContext);
  					}
<RemoveSpecialCComment>[^*\n]+
<RemoveSpecialCComment>"//"|"/*"
<RemoveSpecialCComment>\n  { yyextra->yyLineNr++; }
<RemoveSpecialCComment>.
<MemberCall>[^a-z_A-Z0-9(\n]		{ 
  					  yyextra->code->codify(yytext);
    					  yyextra->type.resize(0);
					  yyextra->name.resize(0);
					  BEGIN(yyextra->memCallContext); 
					}
<*>\n({B}*"//"[!/][^\n]*\n)+		{ // remove special one-line comment
					  if (YY_START==SkipCPP) REJECT;
  					  if (Config_getBool(STRIP_CODE_COMMENTS))
					  {
					    yyextra->yyLineNr+=((QCString)yytext).contains('\n');
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
<SkipCPP>\n/.*\n			{ 
					  endFontClass(yyscanner);
  					  codifyLines(yyscanner,yytext);
					  BEGIN( yyextra->lastSkipCppContext ) ;
					}
<*>\n{B}*"//@"[{}].*\n			{ // remove one-line group marker
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
<*>\n{B}*"/*@"[{}]			{ // remove one-line group marker
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
<*>^{B}*"//@"[{}].*\n			{ // remove one-line group marker
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
<*>^{B}*"/*@"[{}]			{ // remove multi-line group marker
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
<*>^{B}*"//"[!/][^\n]*\n		{ // remove special one-line comment
  					  if (Config_getBool(STRIP_CODE_COMMENTS))
					  {
					    yyextra->yyLineNr++;
					    //nextCodeLine(yyscanner);
					  }
					  else
					  {
					    startFontClass(yyscanner,"comment");
					    codifyLines(yyscanner,yytext);
					    endFontClass(yyscanner);
					  }
  					}
<*>"//"[!/][^\n]*\n			{ // strip special one-line comment
                                          if (YY_START==SkipComment || YY_START==SkipString) REJECT;
  					  if (Config_getBool(STRIP_CODE_COMMENTS))
					  {
					    char c[2]; c[0]='\n'; c[1]=0;
					    codifyLines(yyscanner,c);
					  }
					  else
					  {
					    startFontClass(yyscanner,"comment");
					    codifyLines(yyscanner,yytext);
					    endFontClass(yyscanner);
					  }
  					}
<*>"/*[tag:"[^\]\n]*"]*/"{B}*		{ // special pattern /*[tag:filename]*/ to force linking to a tag file
  					  yyextra->forceTagReference=yytext;
					  uint s=(uint)yyextra->forceTagReference.find(':');
					  uint e=(uint)yyextra->forceTagReference.findRev(']');
					  yyextra->forceTagReference = yyextra->forceTagReference.mid(s+1,e-s-1);
  					}
<*>\n{B}*"/*"[!*]/[^/*] 		{
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
<*>^{B}*"/**"[*]+/[^/]			{ // special C "banner" comment block at a new line
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
<*>^{B}*"/*"[!*]/[^/*]			{ // special C comment block at a new line
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
<*>"/*"[!*]/[^/*]			{ // special C comment block half way a line
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
<*>"/*"("!"?)"*/"			{ 
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
<*>"/*"					{ 
					  startFontClass(yyscanner,"comment");
  					  yyextra->code->codify(yytext);
					  // check is to prevent getting stuck in skipping C++ comments
					  if (YY_START != SkipComment && YY_START != SkipCxxComment)
					  {
  					    yyextra->lastCContext = YY_START ;
					  }
					  BEGIN( SkipComment ) ;
					}
<*>@\"					{ // C# verbatim string
					  startFontClass(yyscanner,"stringliteral");
  					  yyextra->code->codify(yytext);
					  yyextra->lastVerbStringContext=YY_START;
					  BEGIN(SkipVerbString);
					}
<*>"//"					{ 
  					  startFontClass(yyscanner,"comment");
  					  yyextra->code->codify(yytext);
  					  yyextra->lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>"("|"["					{
  					  yyextra->code->codify(yytext);
					  yyextra->theCallContext.pushScope(yyextra->name, yyextra->type);
  					}
<*>")"|"]"					{
  					  yyextra->code->codify(yytext);
					  yyextra->theCallContext.popScope(yyextra->name, yyextra->type);
  					}
<*>\n					{
					  yyextra->yyColNr++;
  					  codifyLines(yyscanner,yytext); 
  					}
<*>.					{
					  yyextra->yyColNr++;
  					  yyextra->code->codify(yytext);
					}
  /*
<*>([ \t\n]*"\n"){2,}			{ // combine multiple blank lines
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

void VariableContext::addVariable(yyscan_t yyscanner,const QCString &type,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("VariableContext::addVariable(%s,%s)\n",type.data(),name.data());
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
  DBG_CTX((stderr,"** addVariable trying: type='%s' name='%s' g_currentDefinition=%s\n",
	ltype.data(),lname.data(),g_currentDefinition?g_currentDefinition->name().data():"<none>"));
  Scope *scope = m_scopes.count()==0 ? &m_globalScope : m_scopes.getLast();
  const ClassDef *varType;
  int i=0;
  if (
      (varType=yyextra->codeClassSDict->find(ltype)) ||  // look for class definitions inside the code block
      (varType=getResolvedClass(yyextra->currentDefinition,yyextra->sourceFileDef,ltype)) // look for global class definitions
     ) 
  {
    DBG_CTX((stderr,"** addVariable type='%s' name='%s'\n",ltype.data(),lname.data()));
    scope->append(lname,varType); // add it to a list
  }
  else if ((i=ltype.find('<'))!=-1)
  {
    // probably a template class
    QCString typeName(ltype.left(i));
    ClassDef* newDef = 0;
    QCString templateArgs(ltype.right(ltype.length() - i));
    if ( !typeName.isEmpty() &&
         ( // look for class definitions inside the code block
	   (varType=yyextra->codeClassSDict->find(typeName)) ||
           // otherwise look for global class definitions
           (varType=getResolvedClass(yyextra->currentDefinition,yyextra->sourceFileDef,typeName,0,0,TRUE,TRUE))
	 ) && // and it must be a template
         !varType->templateArguments().empty())
    {
      newDef = varType->getVariableInstance( templateArgs );
    }
    if (newDef)
    {
      DBG_CTX((stderr,"** addVariable type='%s' templ='%s' name='%s'\n",typeName.data(),templateArgs.data(),lname.data()));
      scope->append(lname, newDef);
    }
    else
    {
      // Doesn't seem to be a template. Try just the base name.
      addVariable(yyscanner,typeName,name);
    }
  }
  else 
  {
    if (m_scopes.count()>0) // for local variables add a dummy entry so the name 
                            // is hidden to avoid false links to global variables with the same name
                            // TODO: make this work for namespaces as well!
    {
      DBG_CTX((stderr,"** addVariable: dummy context for '%s'\n",lname.data()));
      scope->append(lname,dummyContext);
    }
    else
    {
      DBG_CTX((stderr,"** addVariable: not adding variable!\n"));
    }
  }
}

ClassDef *VariableContext::findVariable(const QCString &name)
{
  if (name.isEmpty()) return 0;
  ClassDef *result = 0;
  QListIterator<Scope> sli(m_scopes);
  Scope *scope;
  QCString key = name;
  // search from inner to outer scope
  for (sli.toLast();(scope=sli.current());--sli)
  {
    result = scope->find(key);
    if (result)
    {
      DBG_CTX((stderr,"** findVariable(%s)=%p\n",name.data(),result));
      return result;
    }
  }
  // nothing found -> also try the global scope
  result=m_globalScope.find(name);
  DBG_CTX((stderr,"** findVariable(%s)=%p\n",name.data(),result));
  return result;
}

const ClassDef *VariableContext::dummyContext = (ClassDef*)0x8;

//-------------------------------------------------------------------

/*! add class/namespace name s to the scope */
static void pushScope(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->classScopeLengthStack.push(new int(yyextra->classScope.length()));
  if (yyextra->classScope.isEmpty() || leftScopeMatch(s,yyextra->classScope))
  {
    yyextra->classScope = s;
  }
  else
  {
    yyextra->classScope += "::";
    yyextra->classScope += s;
  }
  //printf("pushScope(%s) result: '%s'\n",s,yyextra->classScope.data());
}


/*! remove the top class/namespace name from the scope */
static void popScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->classScopeLengthStack.isEmpty())
  {
    int *pLength = yyextra->classScopeLengthStack.pop();
    yyextra->classScope.truncate(*pLength);
    delete pLength;
  }
  else
  {
    //err("Too many end of scopes found!\n");
  }
  //printf("popScope() result: '%s'\n",yyextra->classScope.data());
}

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
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

static void addToSearchIndex(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    yyextra->code->addWord(text,FALSE);
  }
}

static void setClassScope(yyscan_t yyscanner,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("setClassScope(%s)\n",name.data());
  QCString n=name;
  n=n.simplifyWhiteSpace();
  int ts=n.find('<'); // start of template
  int te=n.findRev('>'); // end of template
  //printf("ts=%d te=%d\n",ts,te);
  if (ts!=-1 && te!=-1 && te>ts)
  {
    // remove template from scope
    n=n.left(ts)+n.right(n.length()-te-1);
  }
  while (!yyextra->classScopeLengthStack.isEmpty())
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
  //printf("--->New class scope '%s'\n",yyextra->classScope.data());
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
   
    Definition *d   = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    //printf("%s:startCodeLine(%d)=%p\n",yyextra->sourceFileDef->name().data(),yyextra->yyLineNr,d);
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
      //printf("Real scope: '%s'\n",yyextra->realScope.data());
      yyextra->bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
	                        yyextra->currentMemberDef->getOutputFileBase(),
	                        yyextra->currentMemberDef->anchor(),yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        yyextra->code->writeLineNumber(d->getReference(),
	                        d->getOutputFileBase(),
	                        0,yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(0,0,0,yyextra->yyLineNr);
    }
  }
  DBG_CTX((stderr,"startCodeLine(%d)\n",yyextra->yyLineNr));
  yyextra->code->startCodeLine(yyextra->sourceFileDef && yyextra->lineNumbers); 
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
}



static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"endCodeLine(%d)\n",yyextra->yyLineNr));
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
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
static void codifyLines(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  const char *p=text,*sp=p;
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
      //*(p-1)='\0';
      int l = (int)(p-sp-1);
      char *tmp = (char*)malloc(l+1);
      memcpy(tmp,sp,l);
      tmp[l]='\0';
      yyextra->code->codify(tmp);
      free(tmp);
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
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                                   const Definition *d,
                                   const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  static bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  TooltipManager::instance()->addTooltip(d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip; 
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
    tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=FALSE;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      *(p-1)='\0';
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
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

static void addUsingDirective(yyscan_t yyscanner,const char *name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->sourceFileDef && name)
  {
    NamespaceDef *nd = Doxygen::namespaceSDict->find(name);
    if (nd)
    {
      yyextra->sourceFileDef->addUsingDirective(nd);
    }
  }
}

static void setParameterList(yyscan_t yyscanner,const MemberDef *md)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->classScope = md->getClassDef() ? md->getClassDef()->name().data() : "";
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
    yyextra->theVarContext.addVariable(yyscanner,yyextra->parmType,yyextra->parmName);
  }
}

static const ClassDef *stripClassName(yyscan_t yyscanner,const char *s,Definition *d)
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
      cd=getResolvedClass(d,yyextra->sourceFileDef,yyextra->classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=getResolvedClass(d,yyextra->sourceFileDef,clName);
    }
    //printf("stripClass trying '%s' = %p\n",clName.data(),cd);
    if (cd)
    {
      return cd;
    }
  }

  return 0;
}

static MemberDef *setCallContextForVar(yyscan_t yyscanner,const QCString &name)
{
  if (name.isEmpty()) return 0;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"setCallContextForVar(%s) yyextra->classScope=%s\n",name.data(),yyextra->classScope.data()));

  int scopeEnd = name.findRev("::");
  if (scopeEnd!=-1) // name with explicit scope
  {
    QCString scope   = name.left(scopeEnd);
    QCString locName = name.right(name.length()-scopeEnd-2);
    //printf("explicit scope: name=%s scope=%s\n",locName.data(),scope.data());
    ClassDef *mcd = getClass(scope); 
    if (mcd && !locName.isEmpty())
    {
      MemberDef *md=mcd->getMemberByName(locName);
      if (md)
      {
        //printf("name=%s scope=%s\n",locName.data(),scope.data());
        yyextra->theCallContext.setScope(stripClassName(yyscanner,md->typeString(),md->getOuterScope()));
        return md;
      }
    }
    else // check namespace as well
    {
      const NamespaceDef *mnd = getResolvedNamespace(scope);
      if (mnd && !locName.isEmpty())
      {
	MemberDef *md=mnd->getMemberByName(locName);
	if (md)
	{
	  //printf("name=%s scope=%s\n",locName.data(),scope.data());
	  yyextra->theCallContext.setScope(stripClassName(yyscanner,md->typeString(),md->getOuterScope()));
	  return md;
	}
      }
    }
  }
  
  MemberName *mn;
  ClassDef *mcd = yyextra->theVarContext.findVariable(name);
  if (mcd) // local variable
  {
    DBG_CTX((stderr,"local variable?\n"));
    if (mcd!=VariableContext::dummyContext)
    {
      DBG_CTX((stderr,"local var '%s' mcd=%s\n",name.data(),mcd->name().data()));
      yyextra->theCallContext.setScope(mcd);
    }
  }
  else
  {
    DBG_CTX((stderr,"class member? scope=%s\n",yyextra->classScope.data()));
    // look for a class member 
    mcd = getClass(yyextra->classScope);
    if (mcd)
    {
      DBG_CTX((stderr,"Inside class %s\n",mcd->name().data()));
      MemberDef *md=mcd->getMemberByName(name);
      if (md) 
      {
        DBG_CTX((stderr,"Found member %s\n",md->name().data()));
	if (yyextra->scopeStack.top()!=CLASSBLOCK)
	{
          DBG_CTX((stderr,"class member '%s' mcd=%s\n",name.data(),mcd->name().data()));
	  yyextra->theCallContext.setScope(stripClassName(yyscanner,md->typeString(),md->getOuterScope()));
	}
	return md;
      }
    }
  }

  // look for a global member
  if ((mn=Doxygen::functionNameLinkedMap->find(name)))
  {
    //printf("global var '%s'\n",name.data());
    if (mn->size()==1) // global defined only once
    {
      const std::unique_ptr<MemberDef> &md=mn->front();
      if (!md->isStatic() || md->getBodyDef()==yyextra->sourceFileDef)
      {
        yyextra->theCallContext.setScope(stripClassName(yyscanner,md->typeString(),md->getOuterScope()));
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
        if ((!md->isStatic() || md->getBodyDef()==yyextra->sourceFileDef) &&
	    (yyextra->forceTagReference.isEmpty() || yyextra->forceTagReference==md->getReference())
	   )
        {
          yyextra->theCallContext.setScope(stripClassName(yyscanner,md->typeString(),md->getOuterScope()));
	  //printf("returning member %s in source file %s\n",md->name().data(),yyextra->sourceFileDef->name().data());
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
  const Definition *d = yyextra->theCallContext.getScope();
  //printf("updateCallContextForSmartPointer() cd=%s\n",cd ? d->name().data() : "<none>");
  MemberDef *md;
  if (d && d->definitionType()==Definition::TypeClass && (md=(dynamic_cast<const ClassDef*>(d))->isSmartPointer()))
  {
    const ClassDef *ncd = stripClassName(yyscanner,md->typeString(),md->getOuterScope());
    if (ncd)
    {
      yyextra->theCallContext.setScope(ncd);
      //printf("Found smart pointer call %s->%s!\n",cd->name().data(),ncd->name().data());
    }
  }
}

static bool getLinkInScope(yyscan_t yyscanner,
                           const QCString &c,  // scope
                           const QCString &m,  // member
			   const char *memberText, // exact text
			   CodeOutputInterface &ol,
			   const char *text,
			   bool varOnly
			  )
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const MemberDef    *md = 0;
  const ClassDef     *cd = 0;
  const FileDef      *fd = 0;
  const NamespaceDef *nd = 0;
  const GroupDef     *gd = 0;
  DBG_CTX((stderr,"getLinkInScope: trying '%s'::'%s' varOnly=%d\n",c.data(),m.data(),varOnly));
  if (getDefs(c,m,"()",md,cd,fd,nd,gd,FALSE,yyextra->sourceFileDef,FALSE,yyextra->forceTagReference) && 
      (!varOnly || md->isVariable()))
  {
    if (md->isLinkable())
    {
      //printf("found it %s!\n",md->qualifiedName().data());
      if (yyextra->exampleBlock)
      {
        QCString anchor;
        anchor.sprintf("a%d",yyextra->anchorCount);
        //printf("addExampleFile(%s,%s,%s)\n",anchor.data(),yyextra->exampleName.data(),
        //                                  yyextra->exampleFile.data());
        if (const_cast<MemberDef*>(md)->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
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
        yyextra->theCallContext.setScope(stripClassName(yyscanner,md->typeString(),md->getOuterScope()));
        //printf("yyextra->currentDefinition=%p yyextra->currentMemberDef=%p yyextra->insideBody=%d\n",
        //        yyextra->currentDefinition,yyextra->currentMemberDef,yyextra->insideBody);

        if (yyextra->currentDefinition && yyextra->currentMemberDef &&
            md!=yyextra->currentMemberDef && yyextra->insideBody && yyextra->collectXRefs)
        {
          addDocCrossReference(yyextra->currentMemberDef,const_cast<MemberDef*>(md));
        }
        //printf("d->getReference()='%s' d->getOutputBase()='%s' name='%s' member name='%s'\n",d->getReference().data(),d->getOutputFileBase().data(),d->name().data(),md->name().data());

        writeMultiLineCodeLink(yyscanner,ol,md, text ? text : memberText);
        addToSearchIndex(yyscanner,text ? text : memberText);
        return TRUE;
      }
    }
    else // found member, but it is not linkable, so make sure content inside is not assigned
         // to the previous member, see bug762760
    {
      DBG_CTX((stderr,"unlinkable member %s\n",md->name().data()));
      yyextra->currentMemberDef = 0;
    }
  }
  return FALSE;
}

static bool getLink(yyscan_t yyscanner,
                    const char *className,
                    const char *memberName,
		    CodeOutputInterface &ol,
		    const char *text,
		    bool varOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("getLink(%s,%s) yyextra->curClassName=%s\n",className,memberName,yyextra->curClassName.data());
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
                                      const char *clName,
                                      bool typeOnly,
                                      bool varOnly)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int i=0;
  if (*clName=='~') // correct for matching negated values i.s.o. destructors.
  {
    yyextra->code->codify("~");
    clName++;
  }
  QCString className=clName;
  if (className.isEmpty()) return;
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
    className = substitute(className,".","::"); // for PHP namespaces
  }
  const ClassDef *cd=0,*lcd=0;
  const MemberDef *md=0;
  bool isLocal=FALSE;

  //printf("generateClassOrGlobalLink(className=%s)\n",className.data());
  if (!yyextra->prefixed_with_this_keyword || (lcd=yyextra->theVarContext.findVariable(className))==0) // not a local variable
  {
    Definition *d = yyextra->currentDefinition;
    //printf("d=%s yyextra->sourceFileDef=%s\n",d?d->name().data():"<none>",yyextra->sourceFileDef?yyextra->sourceFileDef->name().data():"<none>");
    cd = getResolvedClass(d,yyextra->sourceFileDef,className,&md);
    DBG_CTX((stderr,"non-local variable name=%s context=%d cd=%s md=%s!\n",
    className.data(),yyextra->theVarContext.count(),cd?cd->name().data():"<none>",
        md?md->name().data():"<none>"));
    if (cd==0 && md==0 && (i=className.find('<'))!=-1)
    {
      QCString bareName = className.left(i); //stripTemplateSpecifiersFromScope(className);
      DBG_CTX((stderr,"bareName=%s\n",bareName.data()));
      if (bareName!=className)
      {
	cd=getResolvedClass(d,yyextra->sourceFileDef,bareName,&md); // try unspecialized version
      }
    }
    const NamespaceDef *nd = getResolvedNamespace(className);
    if (nd && nd->isLinkable())
    {
      yyextra->theCallContext.setScope(nd);
      addToSearchIndex(yyscanner,className);
      writeMultiLineCodeLink(yyscanner,*yyextra->code,nd,clName);
      return;
    }
    //printf("md=%s\n",md?md->name().data():"<none>");
    DBG_CTX((stderr,"is found as a type cd=%s nd=%s\n",
          cd?cd->name().data():"<null>",
          nd?nd->name().data():"<null>"));
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
    //printf("local variable!\n");
    if (lcd!=VariableContext::dummyContext) 
    {
      //printf("non-dummy context lcd=%s!\n",lcd->name().data());
      yyextra->theCallContext.setScope(lcd);

      // to following is needed for links to a global variable, but is
      // no good for a link to a local variable that is also a global symbol.
       
      //if (getLink(yyscanner,yyextra->classScope,clName,ol,clName))
      //{
	//return;
      //}
    }
    isLocal=TRUE;
    DBG_CTX((stderr,"is a local variable cd=%p!\n",cd));
  }
  yyextra->prefixed_with_this_keyword = FALSE; // discard the "this" prefix for the next calls

  if (cd && cd->isLinkable()) // is it a linkable class
  {
    DBG_CTX((stderr,"is linkable class %s\n",clName));
    if (yyextra->exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("_a%d",yyextra->anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),yyextra->exampleName.data(),
      //                                   yyextra->exampleFile.data());
      if (const_cast<ClassDef*>(cd)->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
      {
	ol.writeCodeAnchor(anchor);
	yyextra->anchorCount++;
      }
    }
    writeMultiLineCodeLink(yyscanner,ol,cd,clName);
    addToSearchIndex(yyscanner,className);
    yyextra->theCallContext.setScope(cd);
    if (md)
    {
      const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                      md->getFileDef() : md->getOuterScope();
      if (md->getGroupDef()) d = md->getGroupDef();
      if (d && d->isLinkable() && md->isLinkable() && 
          yyextra->currentMemberDef && yyextra->collectXRefs)
      {
        addDocCrossReference(yyextra->currentMemberDef,const_cast<MemberDef*>(md));
      }
    }
  }
  else // not a class, maybe a global member
  {
    DBG_CTX((stderr,"class %s not linkable! cd=%p md=%p typeOnly=%d\n",clName,cd,md,typeOnly));
    if (!isLocal && (md!=0 || (cd==0 && !typeOnly))) // not a class, see if it is a global enum/variable/typedef.
    {
      if (md==0) // not found as a typedef
      {
	md = setCallContextForVar(yyscanner,clName);
	//printf("setCallContextForVar(%s) md=%p yyextra->currentDefinition=%p\n",clName,md,yyextra->currentDefinition);
	if (md && yyextra->currentDefinition)
	{
	  DBG_CTX((stderr,"%s accessible from %s? %d md->getOuterScope=%s\n",
	      md->name().data(),yyextra->currentDefinition->name().data(),
	      isAccessibleFrom(yyextra->currentDefinition,yyextra->sourceFileDef,md),
	      md->getOuterScope()->name().data()));
	}
	     
        if (md && yyextra->currentDefinition && 
	    isAccessibleFrom(yyextra->currentDefinition,yyextra->sourceFileDef,md)==-1)
	{
	  md=0; // variable not accessible
	}
      }
      if (md && (!varOnly || md->isVariable()))
      {
        DBG_CTX((stderr,"is a global md=%p yyextra->currentDefinition=%s linkable=%d\n",md,yyextra->currentDefinition?yyextra->currentDefinition->name().data():"<none>",md->isLinkable()));
	if (md->isLinkable())
	{
	  QCString text;
	  if (!yyextra->forceTagReference.isEmpty()) // explicit reference to symbol in tag file
	  {
	    text=yyextra->forceTagReference;
	    if (text.right(4)==".tag") // strip .tag if present
	    {
	      text=text.left(text.length()-4);
	    }
	    text+=getLanguageSpecificSeparator(md->getLanguage());
	    text+=clName;
	    const_cast<MemberDef*>(md)->setName(text);
            const_cast<MemberDef*>(md)->setLocalName(text);
	  }
	  else // normal reference
	  {
	    text=clName;
	  }
	  writeMultiLineCodeLink(yyscanner,ol,md,text);
          addToSearchIndex(yyscanner,clName);
	  if (yyextra->currentMemberDef && yyextra->collectXRefs)
	  {
	    addDocCrossReference(yyextra->currentMemberDef,const_cast<MemberDef*>(md));
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

static bool generateClassMemberLink(yyscan_t yyscanner,
                                    CodeOutputInterface &ol,
                                    MemberDef *xmd,
                                    const char *memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // extract class definition of the return type in order to resolve
  // a->b()->c() like call chains

  //printf("type='%s' args='%s' class=%s\n",
  //  xmd->typeString(),xmd->argsString(),
  //  xmd->getClassDef()->name().data());

  if (yyextra->exampleBlock)
  {
    QCString anchor;
    anchor.sprintf("a%d",yyextra->anchorCount);
    //printf("addExampleFile(%s,%s,%s)\n",anchor.data(),yyextra->exampleName.data(),
    //                                  yyextra->exampleFile.data());
    if (xmd->addExample(anchor,yyextra->exampleName,yyextra->exampleFile))
    {
      ol.writeCodeAnchor(anchor);
      yyextra->anchorCount++;
    }
  }

  const ClassDef *typeClass = stripClassName(yyscanner,removeAnonymousScopes(xmd->typeString()),xmd->getOuterScope());
  DBG_CTX((stderr,"%s -> typeName=%p\n",xmd->typeString(),typeClass));
  yyextra->theCallContext.setScope(typeClass);

  const Definition *xd = xmd->getOuterScope()==Doxygen::globalScope ?
                   xmd->getFileDef() : xmd->getOuterScope();
  if (xmd->getGroupDef()) xd = xmd->getGroupDef();
  if (xd && xd->isLinkable())
  {

    //printf("yyextra->currentDefinition=%p yyextra->currentMemberDef=%p xmd=%p yyextra->insideBody=%d\n",yyextra->currentDefinition,yyextra->currentMemberDef,xmd,yyextra->insideBody);

    if (xmd->templateMaster()) xmd = xmd->templateMaster();

    if (xmd->isLinkable())
    {
      // add usage reference
      if (yyextra->currentDefinition && yyextra->currentMemberDef &&
	  /*xmd!=yyextra->currentMemberDef &&*/ yyextra->insideBody && yyextra->collectXRefs)
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
                                    CodeOutputInterface &ol,
                                    const Definition *def,
                                    const char *memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (def && def->definitionType()==Definition::TypeClass)
  {
    const ClassDef *cd = dynamic_cast<const ClassDef*>(def);
    MemberDef *xmd = cd->getMemberByName(memName);
    //printf("generateClassMemberLink(class=%s,member=%s)=%p\n",def->name().data(),memName,xmd);
    if (xmd)
    {
      return generateClassMemberLink(yyscanner,ol,xmd,memName);
    }
    else
    {
      Definition *innerDef = cd->findInnerCompound(memName);
      if (innerDef)
      {
        yyextra->theCallContext.setScope(innerDef);
        addToSearchIndex(yyscanner,memName);
        writeMultiLineCodeLink(yyscanner,*yyextra->code,innerDef,memName);
        return TRUE;
      }
    }
  }
  else if (def && def->definitionType()==Definition::TypeNamespace)
  {
    const NamespaceDef *nd = dynamic_cast<const NamespaceDef*>(def);
    //printf("Looking for %s inside namespace %s\n",memName,nd->name().data());
    Definition *innerDef = nd->findInnerCompound(memName);
    if (innerDef)
    {
      yyextra->theCallContext.setScope(innerDef);
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
                               const char *memName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("generateMemberLink(object=%s,mem=%s) classScope=%s\n",
  //    varName.data(),memName,yyextra->classScope.data());

  if (varName.isEmpty()) return;

  // look for the variable in the current context
  const ClassDef *vcd = yyextra->theVarContext.findVariable(varName);
  if (vcd) 
  {
    if (vcd!=VariableContext::dummyContext)
    {
      //printf("Class found!\n");
      if (getLink(yyscanner,vcd->name(),memName,ol)) 
      {
	//printf("Found result!\n");
	return;
      }
      if (vcd->baseClasses())
      {
	BaseClassListIterator bcli(*vcd->baseClasses());
	for ( ; bcli.current() ; ++bcli)
	{
	  if (getLink(yyscanner,bcli.current()->classDef->name(),memName,ol)) 
	  {
	    //printf("Found result!\n");
	    return;
	  }
	}
      }
    }
  }
  else // variable not in current context, maybe it is in a parent context
  {
    vcd = getResolvedClass(yyextra->currentDefinition,yyextra->sourceFileDef,yyextra->classScope);
    if (vcd && vcd->isLinkable())
    {
      //printf("Found class %s for variable '%s'\n",yyextra->classScope.data(),varName.data());
      MemberName *vmn=Doxygen::memberNameLinkedMap->find(varName);
      if (vmn==0)
      {
	int vi;
	QCString vn=varName;
	if ((vi=vn.findRev("::"))!=-1 || (vi=vn.findRev('.'))!=-1)  // explicit scope A::b(), probably static member
	{
	  ClassDef *jcd = getClass(vn.left(vi));
	  vn=vn.right(vn.length()-vi-2);
	  vmn=Doxygen::memberNameLinkedMap->find(vn);
	  //printf("Trying name '%s' scope=%s\n",vn.data(),scope.data());
	  if (vmn)
	  {
            for (const auto &vmd : *vmn)
	    {
	      if (vmd->getClassDef()==jcd)
	      {
		//printf("Found variable type=%s\n",vmd->typeString());
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
	//printf("There is a variable with name '%s'\n",varName);
        for (const auto &vmd : *vmn)
	{
	  if (vmd->getClassDef()==vcd)
	  {
	    //printf("Found variable type=%s\n",vmd->typeString());
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
  QCString name = varName+7; // strip $this->
  name.prepend("$");
  //printf("generatePHPVariableLink(%s) name=%s scope=%s\n",varName,name.data(),yyextra->classScope.data());
  if (!getLink(yyscanner,yyextra->classScope,name,ol,varName))
  {
    codifyLines(yyscanner,varName);
  }
}

static void generateFunctionLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *funcName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //CodeClassDef *ccd=0;
  ClassDef *ccd=0;
  QCString locScope=yyextra->classScope;
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  if (yyextra->lang==SrcLangExt_PHP && locFunc.startsWith("self::")) locFunc=locFunc.mid(4);
  QCString funcScope;
  QCString funcWithScope=locFunc;
  QCString funcWithFullScope=locFunc;
  QCString fullScope=locScope;
  DBG_CTX((stdout,"*** locScope=%s locFunc=%s\n",locScope.data(),locFunc.data()));
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
    //printf("ts=%d te=%d\n",ts,te);
    if (ts!=-1 && te!=-1 && te>ts)
    {
      // remove template from scope
      locScope=locScope.left(ts)+locScope.right(locScope.length()-te-1);
    }
    ts=funcScope.find('<'); // start of template
    te=funcScope.findRev('>'); // end of template
    //printf("ts=%d te=%d\n",ts,te);
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
  if (!fullScope.isEmpty() && (ccd=yyextra->codeClassSDict->find(fullScope)))
  {
    //printf("using classScope %s\n",yyextra->classScope.data());
    if (ccd->baseClasses())
    {
      BaseClassListIterator bcli(*ccd->baseClasses());
      for ( ; bcli.current() ; ++bcli)
      {
	if (getLink(yyscanner,bcli.current()->classDef->name(),locFunc,ol,funcName)) 
	{
	  goto exit;
	}
      }
    }
  }
  if (!locScope.isEmpty() && fullScope!=locScope && (ccd=yyextra->codeClassSDict->find(locScope)))
  {
    //printf("using classScope %s\n",yyextra->classScope.data());
    if (ccd->baseClasses())
    {
      BaseClassListIterator bcli(*ccd->baseClasses());
      for ( ; bcli.current() ; ++bcli)
      {
	if (getLink(yyscanner,bcli.current()->classDef->name(),funcWithScope,ol,funcName)) 
	{
	  goto exit;
	}
      }
    }
  }
  if (!getLink(yyscanner,locScope,funcWithScope,ol,funcName))
  {
    generateClassOrGlobalLink(yyscanner,ol,funcName);
  }
exit:  
  yyextra->forceTagReference.resize(0);
  return;
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
    count++, 
    yyextra->needsTermination=TRUE; 
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
  yyextra->code->startFontClass(s);
  yyextra->currentFontClass=s;
}

//----------------------------------------------------------------------------

// recursively writes a linkified Objective-C method call
static void writeObjCMethodCall(yyscan_t yyscanner,ObjCCallCtx *ctx)
{
  if (ctx==0) return;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  char c;
  const char *p = ctx->format.data();
  if (!ctx->methodName.isEmpty())
  {
    //printf("writeObjCMethodCall(%s) obj=%s method=%s\n",
    //    ctx->format.data(),ctx->objectTypeOrName.data(),ctx->methodName.data());
    if (!ctx->objectTypeOrName.isEmpty() && ctx->objectTypeOrName.at(0)!='$')
    {
      //printf("Looking for object=%s method=%s\n",ctx->objectTypeOrName.data(),
      //	ctx->methodName.data());
      ClassDef *cd = yyextra->theVarContext.findVariable(ctx->objectTypeOrName);
      if (cd==0) // not a local variable
      {
	if (ctx->objectTypeOrName=="self")
	{
	  if (yyextra->currentDefinition && 
	      yyextra->currentDefinition->definitionType()==Definition::TypeClass)
	  {
	    ctx->objectType = dynamic_cast<ClassDef *>(yyextra->currentDefinition);
	  }
	}
	else
	{
	  ctx->objectType = getResolvedClass(
	      yyextra->currentDefinition,
	      yyextra->sourceFileDef,
	      ctx->objectTypeOrName,
	      &ctx->method);
	}
	//printf("  object is class? %p\n",ctx->objectType);
	if (ctx->objectType) // found class
	{
	  ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
	  //printf("    yes->method=%s\n",ctx->method?ctx->method->name().data():"<none>");
	}
	else if (ctx->method==0) // search for class variable with the same name
	{
	  //printf("    no\n");
	  //printf("yyextra->currentDefinition=%p\n",yyextra->currentDefinition);
	  if (yyextra->currentDefinition && 
	      yyextra->currentDefinition->definitionType()==Definition::TypeClass)
	  {
	    ctx->objectVar = (dynamic_cast<ClassDef *>(yyextra->currentDefinition))->getMemberByName(ctx->objectTypeOrName);
	    //printf("      ctx->objectVar=%p\n",ctx->objectVar);
	    if (ctx->objectVar)
	    {
	      ctx->objectType = stripClassName(yyscanner,ctx->objectVar->typeString(),yyextra->currentDefinition);
	      //printf("        ctx->objectType=%p\n",ctx->objectType);
	      if (ctx->objectType && !ctx->methodName.isEmpty())
	      {
		ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
		//printf("          ctx->method=%p\n",ctx->method);
	      }
	    }
	  }
	}
      }
      else // local variable
      {
	//printf("  object is local variable\n");
	if (cd!=VariableContext::dummyContext && !ctx->methodName.isEmpty())
	{
	  ctx->method = cd->getMemberByName(ctx->methodName);
	  //printf("   class=%p method=%p\n",cd,ctx->method);
	}
      }
    }
  }

  //printf("[");
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
	  QCString *pName = yyextra->nameDict.find(refId);
	  if (pName)
	  {
	    if (ctx->method && ctx->method->isLinkable())
	    {
              writeMultiLineCodeLink(yyscanner,*yyextra->code,ctx->method,pName->data());
	      if (yyextra->currentMemberDef && yyextra->collectXRefs)
	      {
	        addDocCrossReference(yyextra->currentMemberDef,const_cast<MemberDef*>(ctx->method));
	      }
	    }
	    else
	    {
   	      codifyLines(yyscanner,pName->data());
	    }
	  }
	  else
	  {
	    //printf("Invalid name: id=%d\n",refId);
	  }
	}
	else if (nc=='o') // reference to potential object name
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  QCString *pObject = yyextra->objectDict.find(refId);
	  if (pObject)
	  {
	    if (*pObject=="self")
	    {
	      if (yyextra->currentDefinition && 
		  yyextra->currentDefinition->definitionType()==Definition::TypeClass)
	      {
	        ctx->objectType = dynamic_cast<ClassDef *>(yyextra->currentDefinition);
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
              codifyLines(yyscanner,pObject->data());
	      endFontClass(yyscanner);
	    }
	    else if (*pObject=="super")
	    {
	      if (yyextra->currentDefinition &&
		  yyextra->currentDefinition->definitionType()==Definition::TypeClass)
	      {
		ClassDef *cd = dynamic_cast<ClassDef *>(yyextra->currentDefinition);
		if (cd->categoryOf()) 
		{
		  cd = cd->categoryOf();
		}
		BaseClassList *bcd = cd->baseClasses();
		if (bcd) // get direct base class (there should be only one)
		{
		  BaseClassListIterator bli(*bcd);
		  BaseClassDef *bclass;
		  for (bli.toFirst();(bclass=bli.current());++bli)
		  {
		    if (bclass->classDef->compoundType()!=ClassDef::Protocol)
		    {
		      ctx->objectType = bclass->classDef;
		      if (ctx->objectType && !ctx->methodName.isEmpty())
		      {
			ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
		      }
		    }
		  }
		}
	      }
 	      startFontClass(yyscanner,"keyword");
              codifyLines(yyscanner,pObject->data());
	      endFontClass(yyscanner);
	    }
	    else if (ctx->objectVar && ctx->objectVar->isLinkable()) // object is class variable
	    {
	      writeMultiLineCodeLink(yyscanner,*yyextra->code,ctx->objectVar,pObject->data());
	      if (yyextra->currentMemberDef && yyextra->collectXRefs)
	      {
	        addDocCrossReference(yyextra->currentMemberDef,const_cast<MemberDef*>(ctx->objectVar));
	      }
	    }
	    else if (ctx->objectType && 
		     ctx->objectType!=VariableContext::dummyContext && 
		     ctx->objectType->isLinkable()
		    ) // object is class name
	    {
	      const ClassDef *cd = ctx->objectType;
	      writeMultiLineCodeLink(yyscanner,*yyextra->code,cd,pObject->data());
	    }
	    else // object still needs to be resolved
	    {
	      const ClassDef *cd = getResolvedClass(yyextra->currentDefinition, 
		  yyextra->sourceFileDef, *pObject);
	      if (cd && cd->isLinkable())
	      {
		if (ctx->objectType==0) ctx->objectType=cd;
	   	writeMultiLineCodeLink(yyscanner,*yyextra->code,cd,pObject->data());
	      }
	      else
	      {
		codifyLines(yyscanner,pObject->data());
	      }
	    }
	  }
	  else
	  {
	    //printf("Invalid object: id=%d\n",refId);
	  }
	}
	else if (nc=='c') // reference to nested call
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  ObjCCallCtx *ictx = yyextra->contextDict.find(refId);
	  if (ictx) // recurse into nested call
	  {
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
		//    ictx->method->name().data(),
		//    ctx->methodName.data());
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
	      //printf("  ***** method=%s -> object=%p\n",ictx->method->name().data(),ctx->objectType);
	    }
	  }
	  else
	  {
	    //printf("Invalid context: id=%d\n",refId);
	  }
	}
	else if (nc=='w') // some word
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  QCString *pWord = yyextra->wordDict.find(refId);
	  if (pWord)
	  {
            codifyLines(yyscanner,pWord->data());
	  }
	}
        else if (nc=='d') // comment block
        {
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  QCString *pComment = yyextra->commentDict.find(refId);
          if (pComment)
          {
            startFontClass(yyscanner,"comment");
	    codifyLines(yyscanner,pComment->data());
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
  //printf("%s %s]\n",ctx->objectTypeOrName.data(),ctx->methodName.data());
  //printf("}=(type='%s',name='%s')",
  //    ctx->objectTypeOrName.data(),
  //    ctx->methodName.data());
}

// Replaces an Objective-C method name fragment s by a marker of the form
// $n12, the number (12) can later be used as a key for obtaining the name 
// fragment, from yyextra->nameDict
static QCString escapeName(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$n%d",yyextra->currentNameId);
  yyextra->nameDict.insert(yyextra->currentNameId,new QCString(s));
  yyextra->currentNameId++;
  return result;
}

static QCString escapeObject(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$o%d",yyextra->currentObjId);
  yyextra->objectDict.insert(yyextra->currentObjId,new QCString(s));
  yyextra->currentObjId++;
  return result;
}

static QCString escapeWord(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$w%d",yyextra->currentWordId);
  yyextra->wordDict.insert(yyextra->currentWordId,new QCString(s));
  yyextra->currentWordId++;
  return result;
}

static QCString escapeComment(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString result;
  result.sprintf("$d%d",yyextra->currentCommentId);
  yyextra->commentDict.insert(yyextra->currentCommentId,new QCString(s));
  yyextra->currentCommentId++;
  return result;
}

static bool skipLanguageSpecificKeyword(yyscan_t yyscanner,const QCString &kw)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  return yyextra->lang==SrcLangExt_Cpp && (kw == "remove" || kw == "set" || kw == "get");
}

static bool isCastKeyword(const QCString &s)
{
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
      //printf("new type=%s\n",yyextra->currentCtx->objectTypeOrName.data());
    }
    yyextra->contextStack.push(yyextra->currentCtx);
  }
  else
  {
    //printf("Trying to save NULL context!\n");
  }
  ObjCCallCtx *newCtx = new ObjCCallCtx;
  newCtx->id = yyextra->currentCtxId;
  newCtx->lexState = YY_START;
  newCtx->braceCount = yyextra->braceCount;
  newCtx->objectType = 0;
  newCtx->objectVar = 0;
  newCtx->method = 0;
  //printf("save state=%d\n",YY_START);
  yyextra->contextDict.insert(yyextra->currentCtxId,newCtx);
  yyextra->currentCtx = newCtx;
  yyextra->braceCount = 0;
  yyextra->currentCtxId++;
}

static void restoreObjCContext(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("restore state=%d->%d\n",YY_START,yyextra->currentCtx->lexState);
  BEGIN(yyextra->currentCtx->lexState);
  yyextra->braceCount = yyextra->currentCtx->braceCount;
  if (!yyextra->contextStack.isEmpty())
  {
    yyextra->currentCtx = yyextra->contextStack.pop();
  }
  else
  {
    yyextra->currentCtx = 0;
    //printf("Trying to pop context while yyextra->contextStack is empty!\n");
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
}

CCodeParser::~CCodeParser()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->classScopeLengthStack.clear();
  delete yyextra->codeClassSDict;
  yyextra->codeClassSDict=0;
  codeYYlex_destroy(p->yyscanner);
}

void CCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  //printf("***CodeParser::reset()\n");
  yyextra->forceTagReference.resize(0);
  yyextra->theVarContext.clear();
  yyextra->classScopeLengthStack.setAutoDelete(TRUE);
  yyextra->classScopeLengthStack.clear();
  delete yyextra->codeClassSDict;
  yyextra->codeClassSDict = new ClassSDict(17);
  yyextra->codeClassSDict->setAutoDelete(TRUE);
  yyextra->codeClassSDict->clear();
  yyextra->curClassBases.clear();
  yyextra->anchorCount = 0;
}

void CCodeParser::parseCode(CodeOutputInterface &od,const char *className,const QCString &s, 
                SrcLangExt lang,bool exBlock, const char *exName,FileDef *fd,
		int startLine,int endLine,bool inlineFragment,
		const MemberDef *memberDef,bool showLineNumbers,const Definition *searchCtx,
                bool collectXRefs)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("***parseCode() exBlock=%d exName=%s fd=%p className=%s searchCtx=%s\n",
  //      exBlock,exName,fd,className,searchCtx?searchCtx->name().data():"<none>");

  if (s.isEmpty()) return;

  printlex(yy_flex_debug, TRUE, __FILE__, fd ? fd->fileName().data(): NULL);

  if (yyextra->codeClassSDict==0)
  {
    resetCodeParserState();
  }
  yyextra->code = &od;
  yyextra->inputString   = s;
  yyextra->inputPosition = 0;
  codeYYrestart(0,yyscanner);
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = FALSE;
  yyextra->searchCtx = searchCtx;
  yyextra->collectXRefs = collectXRefs;
  yyextra->inFunctionTryBlock = FALSE;

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
  yyextra->scopeStack.clear();
  yyextra->classScope    = className;
  //printf("parseCCode %s\n",className);
  yyextra->exampleBlock  = exBlock; 
  yyextra->exampleName   = exName;
  yyextra->sourceFileDef = fd;
  yyextra->lineNumbers   = fd!=0 && showLineNumbers;
  bool cleanupSourceDef = FALSE;
  if (fd==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef("",(exName?exName:"generated"));
    cleanupSourceDef = TRUE;
  }
  yyextra->lang        = lang;
  yyextra->insideObjC  = lang==SrcLangExt_ObjC;
  if (yyextra->sourceFileDef) 
  {
    setCurrentDoc(yyscanner,"l00001");
  }
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
  yyextra->searchingForBody = exBlock;
  yyextra->insideBody = FALSE;
  yyextra->bracketCount = 0;
  if (!yyextra->exampleName.isEmpty())
  {
    yyextra->exampleFile = convertNameToFile(yyextra->exampleName+"-example",FALSE,TRUE);
    //printf("yyextra->exampleFile=%s\n",yyextra->exampleFile.data());
  }
  yyextra->includeCodeFragment = inlineFragment;
  //printf("** exBlock=%d exName=%s include=%d\n",exBlock,exName,inlineFragment);
  startCodeLine(yyscanner);
  yyextra->type.resize(0);
  yyextra->name.resize(0);
  yyextra->args.resize(0);
  yyextra->parmName.resize(0);
  yyextra->parmType.resize(0);
  if (memberDef) setParameterList(yyscanner,memberDef);
  BEGIN( Body );
  codeYYlex(yyscanner);
  yyextra->lexInit=TRUE;
  if (yyextra->needsTermination)
  {
    endFontClass(yyscanner);
    DBG_CTX((stderr,"endCodeLine(%d)\n",yyextra->yyLineNr));
    yyextra->code->endCodeLine();
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete yyextra->sourceFileDef;
    yyextra->sourceFileDef=0;
  }

  printlex(yy_flex_debug, FALSE, __FILE__, fd ? fd->fileName().data(): NULL);
  return;
}

#if USE_STATE2STRING
#include "code.l.h"
#endif
