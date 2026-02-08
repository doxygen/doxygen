/*****************************************************************************
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
%option prefix="scannerYY"
%option reentrant
%option extra-type="struct scannerYY_state *"
%top{
#include <stdint.h>
// forward declare yyscan_t to improve typesafety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

/*
 *      includes
 */

#include <algorithm>
#include <vector>
#include <utility>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cctype>

#include "scanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "arguments.h"
#include "moduledef.h"
#include "stringutil.h"

#include "clangparser.h"
#include "markdown.h"
#include "regex.h"
#include "trace.h"
#include "debug.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

struct scannerYY_state
{
  OutlineParserInterface *thisParser;
  CommentScanner   commentScanner;
  const char *     inputString = nullptr;
  int              inputPosition = 0;
  int              lastContext = 0;
  int              lastCContext = 0;
  int              lastDocContext = 0;
  int              lastCPPContext = 0;
  int              lastSkipSharpContext = 0;
  int              lastSkipRoundContext = 0;
  int              lastStringContext = 0;
  int              lastDeprecatedContext = 0;
  int              lastCurlyContext = 0;
  int              lastRoundContext = 0;
  int              lastSharpContext = 0;
  int              lastSquareContext = 0;
  int              lastInitializerContext = 0;
  int              lastClassTemplSpecContext = 0;
  int              lastPreLineCtrlContext = 0;
  int              lastSkipVerbStringContext = 0;
  int              lastSkipInterpStringContext = 0;
  int              lastSkipInterpVerbStringContext = 0;
  int              lastCommentInArgContext = 0;
  int              lastRawStringContext = 0;
  int              lastCSConstraint = 0;
  int              lastHereDocContext = 0;
  int              lastDefineContext = 0;
  int              lastAlignAsContext = 0;
  int              lastC11AttributeContext = 0;
  int              lastModifierContext = 0;
  Protection       protection = Protection::Public;
  Protection       baseProt = Protection::Public;
  bool             exported = false;
  int              sharpCount   = 0 ;
  int              roundCount   = 0 ;
  int              curlyCount   = 0 ;
  int              strCurlyCount = 0 ;
  int              squareCount  = 0 ;
  int              padCount     = 0 ;
  std::shared_ptr<Entry> current;
  std::shared_ptr<Entry> current_root;
  std::shared_ptr<Entry> previous;
  std::shared_ptr<Entry> tempEntry;
  std::shared_ptr<Entry> firstTypedefEntry;
  std::shared_ptr<Entry> memspecEntry;
  int              yyLineNr     = 1 ;
  int              yyBegLineNr  = 1 ;
  int              yyColNr      = 1 ;
  int              yyBegColNr   = 1 ;
  QCString         fileName;
  MethodTypes      mtype = MethodTypes::Method;
  bool             isStatic = false;
  Specifier        virt = Specifier::Normal;
  Specifier        baseVirt = Specifier::Normal;
  QCString         msType;
  QCString         msName;
  QCString         msArgs;
  bool             isTypedef = false;
  QCString         funcPtrType;
  QCString         templateStr;
  QCString         aliasName;
  QCString         baseName;
  QCString*        specName = nullptr;

  SrcLangExt       language = SrcLangExt::Unknown;
  bool             insideIDL   = false;                //!< processing IDL code?
  bool             insideJava  = false;                //!< processing Java code?
  bool             insideCS    = false;                //!< processing C# code?
  bool             insideD     = false;                //!< processing D code?
  bool             insidePHP   = false;                //!< processing PHP code?
  bool             insideObjC  = false;            //!< processing Objective C code?
  bool             insideCli   = false;            //!< processing C++/CLI code?
  bool             insideJS    = false;            //!< processing JavaScript code?
  bool             insideSlice = false;            //!< processing Slice code?
  bool             insideCpp   = true;             //!< processing C/C++ code
  bool             stopAtInvalidString = false;

  bool             insideCppQuote = false;
  bool             insideProtocolList = false;
  bool             doxygenComment = false;

  int              argRoundCount = 0;
  int              argSquareCount = 0;
  int              argSharpCount = 0;
  int              currentArgumentContext = 0;
  int              lastCopyArgStringContext = 0;
  int              lastCopyArgContext = 0;
  int              requiresContext = 0;
  QCString        *copyArgString = nullptr;
  QCString         fullArgString;
  QCString         dummyRawString;

  ArgumentList    *currentArgumentList = nullptr;
  char             lastCopyArgChar = '\0';

  QCString *pCopyQuotedString = nullptr;
  QCString *pCopyRoundString = nullptr;
  QCString *pCopyCurlyString = nullptr;
  QCString *pCopySharpString = nullptr;
  QCString *pCopyRawString = nullptr;
  TextStream *pCopyCurlyGString = nullptr;
  TextStream *pCopyRoundGString = nullptr;
  TextStream *pCopySquareGString = nullptr;
  TextStream *pCopyQuotedGString = nullptr;
  TextStream *pCopyHereDocGString = nullptr;
  TextStream *pCopyRawGString = nullptr;
  TextStream *pSkipVerbString = nullptr;
  TextStream *pSkipInterpString = nullptr;
  TextStream *pSkipInterpVerbString = nullptr;

  bool             insideFormula = false;
  bool             insideTryBlock = false;
  bool             insideCode = false;
  bool             needsSemi = false;

  int              initBracketCount = 0;

  QCString         oldStyleArgType;
  QCString         docBackup;
  QCString         briefBackup;

  int              docBlockContext = 0;
  TextStream       docBlock;
  QCString         docBlockName;
  bool             isCodeBlock = false;
  bool             docBlockInBody = false;
  bool             docBlockAutoBrief = false;
  char             docBlockTerm = '\0';

  QCString         idlAttr;
  QCString         idlProp;
  bool             odlProp = false;

  bool             externLinkage = false;

  QCString         delimiter;

  int              column = 0;

  size_t           fencedSize = 0;
  int              nestedComment = 0;
  using EntryRelation  = std::pair< Entry*, std::shared_ptr<Entry> >;
  using EntryRelations = std::vector<EntryRelation>;
  EntryRelations   outerScopeEntries;
  QCString         programStr;

  ClangTUParser   *clangParser = nullptr;

  int              fakeNS  = 0; //<! number of file scoped namespaces in CSharp file
  TextStream       dummyTextStream;

  int              anonCount = 0;
  int              anonNSCount = 0;

  bool             keepComment = false;
};

[[maybe_unused]] static const char *stateToString(int state);
//-----------------------------------------------------------------------------

// forward declarations for stateless functions
static inline int computeIndent(const char *s,int startIndent);
static inline void initMethodProtection(yyscan_t yyscanner,Protection prot);
static QCString stripQuotes(const char *s);
static QCString stripFuncPtr(const QCString &type);
static bool nameIsOperator(QCString &name);
void fixArgumentListForJavaScript(ArgumentList &al);
static bool startOfRequiresExpression(const QCString &req);

// forward declarations for stateful functions
static void initParser(yyscan_t yyscanner);
static void initEntry(yyscan_t yyscanner);
static void lineCount(yyscan_t yyscanner);
static void addType(yyscan_t yyscanner);
static void setContext(yyscan_t yyscanner);
static void prependScope(yyscan_t yyscanner);
static void startCommentBlock(yyscan_t yyscanner,bool);
static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief);
static void handleParametersCommentBlocks(yyscan_t yyscanner,ArgumentList &al);
static bool checkForKnRstyleC(yyscan_t yyscanner);
static void splitKnRArg(yyscan_t yyscanner,QCString &oldStyleArgPtr,QCString &oldStyleArgName);
static void addKnRArgInfo(yyscan_t yyscanner,const QCString &type,const QCString &name,
                          const QCString &brief,const QCString &docs);
static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static void setJavaProtection(yyscan_t yyscanner);
static void storeClangId(yyscan_t yyscanner,const char *id);
static void startVerbatimBlock(yyscan_t yyscanner,const QCString &blockName,size_t fencedSize=0,bool codeBlock=false);
static bool endVerbatimBlock(yyscan_t yyscanner,const QCString &blockName,size_t fencedSize=0);


/* ----------------------------------------------------------------- */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

       /* start command character */
CMD       ("\\"|"@")
BN        [ \t\n\r]
BNopt     {BN}*
BL        [ \t\r]*"\n"
B         [ \t]
Bopt      {B}*
NOTopt    (("!"{BNopt})|("not"{BN}+))?
DIGIT     [0-9]
HEXDIGIT  ({DIGIT}|[a-f]|[A-F])
ID        [$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*
SCOPENAME "$"?(({ID}?{BN}*"::"{BN}*)*)(((~|!){BN}*)?{ID})
TSCOPE    {ID}("<"[a-z_A-Z0-9 \t\*\&,:]*">")?
CSSCOPENAME (({ID}?{BN}*"."{BN}*)*)((~{BN}*)?{ID})
PRE       [pP][rR][eE]
CODE      [cC][oO][dD][eE]
CHARLIT   (("'"\\x[0-9a-fA-F]{1,2}"'")|("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))
PHPKW     ("require"|"require_once"|"include"|"include_once"|"echo")[^a-zA-Z0-9_;]
PHPUSEKW  ("public"|"private"|"protected")
IDLATTR   ("["[^\]]*"]"){BN}*
TYPEDEFPREFIX (("typedef"{BN}+)?)((("volatile"|"const"){BN}+)?)
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"
ARITHOP   "+"|"-"|"/"|"*"|"%"|"--"|"++"
ASSIGNOP  "="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|="
LOGICOP   "=="|"!="|">"|"<"|">="|"<="|"&&"|"||"|"!"|"<=>"
BITOP     "&"|"|"|"^"|"<<"|">>"|"~"
OPERATOR  "operator"{B}*({ARITHOP}|{ASSIGNOP}|{LOGICOP}|{BITOP})
FUNCOP    "operator"("()"|"[]"|{B}+[^;\n]+)
MODULE_ID ({ID}".")*{ID}
LINENR    {B}*[1-9][0-9]*
FILEICHAR [a-z_A-Z0-9\x80-\xFF\\:\\\/\-\+=&#@~]
FILEECHAR [a-z_A-Z0-9\x80-\xFF\-\+=&#@~]
FILECHARS {FILEICHAR}*{FILEECHAR}+
HFILEMASK {FILEICHAR}*("."{FILEICHAR}+)+{FILECHARS}*
VFILEMASK {FILECHARS}("."{FILECHARS})*
FILEMASK  {VFILEMASK}|{HFILEMASK}

  /* no comment start / end signs inside square brackets */
NCOMM [^/\*]
  // C start comment
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment
CPPC  "/\/"
  // doxygen C start comment
DCOMMC ("/\*!"|"/\**")
  // doxygen Cpp start comment
DCOMMCPP ("/\/!"|"/\/\/")
  // doxygen start comment
DCOMM {DCOMMC}|{DCOMMCPP}

  // Optional any character
ANYopt .*
  // Optional all but newline
NONLopt [^\n]*

%option noyywrap

  /* language parsing states */

%x      AlignAs
%x      AlignAsEnd
%x      SDefine
%x      DefineEnd
%x      CompoundName
%x      ClassVar
%x      CSConstraintName
%x      CSConstraintType
%x      CSIndexer
%x      ClassCategory
%x      ClassTemplSpec
%x      CliPropertyType
%x      CliPropertyIndex
%x      CliOverride
%x      Bases
%x      BasesProt
%x      NextSemi
%x      BitFields
%x      EnumBaseType
%x      FindMembers
%x      FindMembersPHP
%x      FindMemberName
%x      FindFields
%x      FindConceptParts
%x      FindConceptDocs
%x      FindConceptCxxDocs
%x      SFunction
%x      FuncRound
%x      ExcpRound
%x      ExcpList
%x      FuncQual
%x      TrailingReturn
%x      Operator
%x      Array
%x      ReadBody
%x      ReadNSBody
%x      ReadBodyIntf
%x      ReadExpressionBody
%x      Using
%x      UsingAlias
%x      UsingDirective
%x      SkipCurly
%x      SkipCurlyCpp
%x      SkipCurlyEndDoc
%x      SkipString
%x      SkipPHPString
%x      SkipInits
%x      SkipC11Inits
%x      SkipC11Attribute
%x      SkipCPP
%x      SkipComment
%x      SkipCxxComment
%x      SkipCurlyBlock
%x      SkipRoundBlock
%x      Sharp
%x      SkipRound
%x      SkipSquare
%x      StaticAssert
%x      DeclType
%x      TypedefName
%x      TryFunctionBlock
%x      TryFunctionBlockEnd
%x      Comment
%x      PackageName
%x      JavaImport
%x      IDLImport
%x      PHPUse
%x      PHPUseAs
%x      CSAccessorDecl
%x      CSGeneric
%x      PreLineCtrl
%x      DefinePHP
%x      DefinePHPEnd
%x      OldStyleArgs
%x      SkipVerbString
%x      SkipInterpString
%x      SkipInterpVerbString
%x      ObjCMethod
%x      ObjCReturnType
%x      ObjCParams
%x      ObjCParamType
%x      ObjCProtocolList
%x      ObjCPropAttr
%x      ObjCSkipStatement
%x      QtPropType
%x      QtPropAttr
%x      QtPropRead
%x      QtPropWrite
%x      ReadInitializer
%x      ReadInitializerPtr
%x      UNOIDLAttributeBlock
%x      GetCallType
%x      CppQuote
%x      EndCppQuote
%x      MemberSpec
%x      MemberSpecSkip
%x      EndTemplate
%x      FuncPtr
%x      FuncPtrOperator
%x      EndFuncPtr
%x      ReadFuncArgType
%x      ReadTempArgs
%x      IDLUnionCase
%x      NSAliasName
%x      NSAliasArg
%x      CopyString
%x      CopyPHPString
%x      CopyGString
%x      CopyPHPGString
%x      CopyRound
%x      CopySharp
%x      CopyCurly
%x      CopyComment
%x      GCopyRound
%x      GCopySquare
%x      GCopyCurly
%x      GCopyComment
%x      SkipUnionSwitch
%x      Specialization
%x      SpecializationSingleQuote
%x      SpecializationDoubleQuote
%x      FuncFunc
%x      FuncFuncEnd
%x      FuncFuncType
%x      FuncFuncArray
%x      CopyArgString
%x      CopyArgPHPString
%x      CopyArgRound
%x      CopyArgSquare
%x      CopyArgSharp
%x      CopyArgComment
%x      CopyArgCommentLine
%x      CopyArgVerbatim
%x      HereDoc
%x      HereDocEnd
%x      CopyHereDoc
%x      CopyHereDocEnd
%x      RawString
%x      RawGString
%x      CSString
%x      CppProt

%x      IDLAttribute
%x      IDLProp
%x      IDLPropName

 /** Slice states */

%x      SliceOptional
%x      SliceMetadata
%x      SliceSequence
%x      SliceSequenceName
%x      SliceDictionary
%x      SliceDictionaryName

 /** Prototype scanner states */

%x      Prototype
%x      PrototypePtr
%x      PrototypeQual
%x      PrototypeExc
%x      PrototypeSkipLine

 /** comment parsing states */

%x      DocLine
%x      DocBlock
%x      DocCopyBlock

 /** C++20 concepts */

%x      RequiresClause
%x      RequiresExpression
%x      ConceptName

 /** Object-C Deprecated */
%x      Deprecated_round


  /** C++20 modules */
%x      ModuleName
%x      ModuleImport
%%

<*>"DEPRECATED_ATTRIBUTE"               { // Object-C attribute
                                          if (!yyextra->insideObjC) REJECT;
                                        }
<*>"DEPRECATED_MSG_ATTRIBUTE(\""        { // Object-C attribute
                                          if (!yyextra->insideObjC) REJECT;
                                          yyextra->lastDeprecatedContext=YY_START;
                                          yyextra->lastStringContext=Deprecated_round;
                                          BEGIN(SkipString);
                                        }
<Deprecated_round>")"                   {
                                          BEGIN(yyextra->lastDeprecatedContext);
                                        }
<Deprecated_round>{BNopt}               {
                                          lineCount(yyscanner);
                                        }
<Deprecated_round>.                     { }
<NextSemi>"{"                           {
                                          yyextra->curlyCount=0;
                                          yyextra->needsSemi = TRUE;
                                          BEGIN(SkipCurlyBlock);
                                        }
<NextSemi>"("                           {
                                          yyextra->roundCount=0;
                                          BEGIN(SkipRoundBlock);
                                        }
<SkipRoundBlock>"("                     {
                                          ++yyextra->roundCount;
                                        }
<SkipRoundBlock>")"                     {
                                          if (yyextra->roundCount )
                                            --yyextra->roundCount ;
                                          else
                                            BEGIN( NextSemi ) ;
                                        }
<SkipCurlyBlock>"{"                     {
                                          ++yyextra->curlyCount ;
                                        }
<SkipCurlyBlock>"}"                     {
                                          if( yyextra->curlyCount )
                                          {
                                            --yyextra->curlyCount ;
                                          }
                                          else if (yyextra->needsSemi)
                                          {
                                            BEGIN( NextSemi );
                                          }
                                          else
                                          {
                                            BEGIN( FindMembers );
                                          }
                                        }
<NextSemi>\'                            {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastStringContext=NextSemi;
                                            BEGIN(SkipPHPString);
                                          }
                                        }
<NextSemi>{CHARLIT}                     { if (yyextra->insidePHP) REJECT; }
<NextSemi>\"                            {
                                          yyextra->lastStringContext=NextSemi;
                                          BEGIN(SkipString);
                                        }
<NextSemi>[;,]                          {
                                          unput(*yytext);
                                          BEGIN( FindMembers );
                                        }
<BitFields>[;,]                         {
                                          unput(*yytext);
                                          BEGIN( FindMembers );
                                        }
<EnumBaseType>[{;,]                     {
                                          yyextra->current->args = yyextra->current->args.simplifyWhiteSpace();
                                          unput(*yytext);
                                          BEGIN( ClassVar );
                                        }
<FindMembers>"<?php"                    { // PHP code with unsupported extension?
                                          yyextra->insidePHP = TRUE;
                                        }
<FindMembersPHP>"<?"("php"?)            { // PHP code start
                                           BEGIN( FindMembers );
                                        }
<FindMembersPHP>"<script"{BN}+"language"{BN}*"="{BN}*['"]?"php"['"]?{BN}*">" { // PHP code start
                                          lineCount(yyscanner) ;
                                          BEGIN( FindMembers );
                                        }
<FindMembers>"?>"|"</script>"           { // PHP code end
                                          if (yyextra->insidePHP)
                                            BEGIN( FindMembersPHP );
                                          else
                                            REJECT;
                                        }
<FindMembersPHP>[^\n<]+                 { // Non-PHP code text, ignore
                                        }
<FindMembersPHP>\n                      { // Non-PHP code text, ignore
                                          lineCount(yyscanner);
                                        }
<FindMembersPHP>.                       { // Non-PHP code text, ignore
                                        }
<FindMembers>{PHPKW}                    { if (yyextra->insidePHP)
                                            BEGIN( NextSemi );
                                          else
                                            REJECT;
                                        }
<FindMembers>"%{"[^\n]*                 { // Mozilla XPIDL lang-specific block
                                          if (!yyextra->insideIDL)
                                            REJECT;
                                        }
<FindMembers>"%}"                       { // Mozilla XPIDL lang-specific block end
                                          if (!yyextra->insideIDL)
                                            REJECT;
                                        }
<FindMembers>{B}*("properties"){BN}*":"{BN}*  { // IDL or Borland C++ builder property
                                          initMethodProtection(yyscanner,Protection::Public);
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                        }

<FindMembers>{B}*"k_dcop"{BN}*":"{BN}*  {
                                          initMethodProtection(yyscanner,Protection::Public);
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::DCOP;
                                        }

<FindMembers>{B}*("signals"|"Q_SIGNALS"){BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Public);
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Signal;
                                        }

<FindMembers>{B}*"public"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Public);
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Slot;
                                        }

<FindMembers>{B}*"protected"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Protected);
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Slot;
                                        }

<FindMembers>{B}*"private"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Private);
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Slot;
                                        }
<FindMembers>{B}*("public"|"methods"|"__published"){BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Public);
                                        }
<FindMembers>{B}*"internal"{BN}*":"{BN}* {  // for now treat C++/CLI's internal as package...
                                          if (yyextra->insideCli)
                                          {
                                            initMethodProtection(yyscanner,Protection::Package);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"protected"{BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Protected);
                                        }
<FindMembers>{B}*"private"{BN}*":"{BN}* {
                                          initMethodProtection(yyscanner,Protection::Private);
                                        }
<FindMembers>{B}*"public"/({BN}|{CCS}|{CPPC}) {
                                          if (!yyextra->insideCpp) REJECT;
                                          initMethodProtection(yyscanner,Protection::Public);
                                          BEGIN(CppProt);
                                        }
<FindMembers>{B}*"protected"/({BN}|{CCS}|{CPPC}) {
                                          if (!yyextra->insideCpp) REJECT;
                                          initMethodProtection(yyscanner,Protection::Protected);
                                          BEGIN(CppProt);
                                        }
<FindMembers>{B}*"private"/({BN}|{CCS}|{CPPC}) {
                                          if (!yyextra->insideCpp) REJECT;
                                          initMethodProtection(yyscanner,Protection::Private);
                                          BEGIN(CppProt);
                                        }
<CppProt>":"                            {
                                          BEGIN(FindMembers);
                                        }
<CppProt>.                              {
                                          unput(*yytext);
                                          BEGIN(FindMembers);
                                        }
<CppProt>{BN}+                          { lineCount(yyscanner); }
<CppProt>{CPPC}.*\n                     { lineCount(yyscanner); }
<CppProt>{CCS}                          { yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipComment ) ;
                                        }
<CppProt>("slots"|"Q_SLOTS")            {
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Slot;
                                        }
<FindMembers>{B}*"event"{BN}+           {
                                          if (yyextra->insideCli)
                                          {
                                            // C++/CLI event
                                            lineCount(yyscanner) ;
                                            yyextra->current->mtype = yyextra->mtype = MethodTypes::Event;
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( CliPropertyType );
                                          }
                                          else if (yyextra->insideCS)
                                          {
                                            lineCount(yyscanner) ;
                                            yyextra->current->mtype = MethodTypes::Event;
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"property"{BN}+        {
                                           if (yyextra->insideCli)
                                           {
                                             // C++/CLI property
                                             lineCount(yyscanner) ;
                                             yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                             yyextra->current->bodyLine = yyextra->yyLineNr;
                                             yyextra->current->bodyColumn = yyextra->yyColNr;
                                             yyextra->curlyCount=0;
                                             BEGIN( CliPropertyType );
                                           }
                                           else
                                           {
                                             REJECT;
                                           }
                                        }
<CliPropertyType>{ID}                   {
                                          addType(yyscanner);
                                          yyextra->current->name = yytext;
                                        }
<CliPropertyType>"["                    { // C++/CLI indexed property
                                          yyextra->current->args = "[";
                                          BEGIN( CliPropertyIndex );
                                        }
<CliPropertyType>"{"                    {
                                          yyextra->curlyCount=0;
                                          //printf("event: '%s' '%s'\n",qPrint(yyextra->current->type),qPrint(yyextra->current->name));
                                          BEGIN( CSAccessorDecl );
                                        }
<CliPropertyType>";"                    {
                                          unput(*yytext);
                                          BEGIN( FindMembers );
                                        }
<CliPropertyType>\n                     {
                                          lineCount(yyscanner);
                                        }
<CliPropertyType>{B}*                   {
                                        }
<CliPropertyType>.                      {
                                          addType(yyscanner);
                                          yyextra->current->type += yytext;
                                        }
<CliPropertyIndex>"]"                   {
                                          BEGIN( CliPropertyType );
                                          yyextra->current->args+=yytext;
                                        }
<CliPropertyIndex>.                     {
                                          yyextra->current->args+=yytext;
                                        }
  /*
<FindMembers>{B}*"property"{BN}+        {
                                          if (!yyextra->current->type.isEmpty())
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                            lineCount(yyscanner);
                                          }
                                        }
  */
<FindMembers>{B}*"@private"{BN}+        {
                                          initMethodProtection(yyscanner,Protection::Private);
                                        }
<FindMembers>{B}*"@protected"{BN}+      {
                                          initMethodProtection(yyscanner,Protection::Protected);
                                        }
<FindMembers>{B}*"@public"{BN}+         {
                                          initMethodProtection(yyscanner,Protection::Public);
                                        }
<FindMembers>[\-+]{BN}*                 {
                                          if (!yyextra->insideObjC)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->current->section = EntryType::makeFunction();
                                            yyextra->language = yyextra->current->lang = SrcLangExt::ObjC;
                                            yyextra->insideObjC = TRUE;
                                            yyextra->yyBegColNr  = yyextra->yyColNr;
                                            yyextra->yyBegLineNr = yyextra->yyLineNr;
                                            yyextra->current->virt = Specifier::Virtual;

                                            yyextra->current->isStatic=yytext[0]=='+';
                                            initMethodProtection(yyscanner,Protection::Public);
                                            BEGIN( ObjCMethod );
                                          }
                                        }
<ObjCMethod>"("                         { // start of method's return type
                                          BEGIN( ObjCReturnType );
                                          yyextra->current->type.clear();
                                          yyextra->roundCount=0;
                                        }
<ObjCMethod>{ID}                        { // found method name
                                          if (yyextra->current->type.isEmpty())
                                          {
                                            yyextra->current->type += "id";
                                          }
                                          yyextra->current->name = yytext;
                                          storeClangId(yyscanner,yytext);
                                        }
<ObjCMethod>":"{B}*                     { // start of parameter list
                                          yyextra->current->name += ':';
                                          Argument a;
                                          yyextra->current->argList.push_back(a);
                                          BEGIN( ObjCParams );
                                        }
<ObjCReturnType>[^()]*                  {
                                          yyextra->current->type += yytext;
                                        }
<ObjCReturnType>"(^)("                  { // Block return type
                                          yyextra->current->type += yytext;
                                          yyextra->roundCount++;
                                        }
<ObjCReturnType>"("                     {
                                          yyextra->current->type += yytext;
                                          yyextra->roundCount++;
                                        }
<ObjCReturnType>")"                     {
                                          if (yyextra->roundCount<=0)
                                          {
                                            BEGIN( ObjCMethod );
                                          }
                                          else
                                          {
                                            yyextra->current->type += yytext;
                                            yyextra->roundCount--;
                                          }
                                        }
<ObjCParams>({ID})?{BN}*":"             { // Keyword of parameter
                                          QCString keyw = yytext;
                                          keyw=keyw.left(keyw.length()-1).stripWhiteSpace(); // strip :
                                          if (keyw.isEmpty())
                                          {
                                            yyextra->current->name += " :";
                                          }
                                          else
                                          {
                                            yyextra->current->name += keyw+":";
                                          }
                                          if (yyextra->current->argList.back().type.isEmpty())
                                          {
                                            yyextra->current->argList.back().type="id";
                                          }
                                          Argument a;
                                          a.attrib=(QCString)"["+keyw+"]";
                                          yyextra->current->argList.push_back(a);
                                        }
<ObjCParams>{ID}{BN}*                   { // name of parameter
                                          lineCount(yyscanner);
                                          yyextra->current->argList.back().name=QCString(yytext).stripWhiteSpace();
                                        }
<ObjCParams>","{BN}*"..."               { // name of parameter
                                          lineCount(yyscanner);
                                          // do we want the comma as part of the name?
                                          //yyextra->current->name += ",";
                                          Argument a;
                                          a.attrib="[,]";
                                          a.type="...";
                                          yyextra->current->argList.push_back(a);
                                        }
   /*
<ObjCParams>":"                         {
                                          yyextra->current->name += ':';
                                        }
   */
<ObjCParams>"("                         {
                                          yyextra->roundCount=0;
                                          yyextra->current->argList.back().type.clear();
                                          BEGIN( ObjCParamType );
                                        }
<ObjCParamType>"("                      {
                                          yyextra->roundCount++;
                                          yyextra->current->argList.back().type+=yytext;
                                        }
<ObjCParamType>")"/{B}*                 {
                                          if (yyextra->roundCount<=0)
                                          {
                                            BEGIN( ObjCParams );
                                          }
                                          else
                                          {
                                            yyextra->current->argList.back().type+=yytext;
                                            yyextra->roundCount--;
                                          }
                                        }
<ObjCParamType>[^()]*                   {
                                          yyextra->current->argList.back().type+=QCString(yytext).stripWhiteSpace();
                                        }
<ObjCMethod,ObjCParams>";"              { // end of method declaration
                                          if (!yyextra->current->argList.empty() && yyextra->current->argList.back().type.isEmpty())
                                          {
                                            yyextra->current->argList.back().type="id";
                                          }
                                          if (yyextra->current->argList.empty()) // method without parameters
                                          {
                                            yyextra->current->argList.setNoParameters(TRUE);
                                          }
                                          yyextra->current->args = argListToString(yyextra->current->argList);
                                          //printf("argList=%s\n",qPrint(yyextra->current->args));
                                          unput(';');
                                          BEGIN( SFunction );
                                        }
<ObjCMethod,ObjCParams>(";"{BN}+)?"{"   { // start of a method body
                                          lineCount(yyscanner);
                                          //printf("Type=%s Name=%s args=%s\n",
                                          //    qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(argListToString(yyextra->current->argList))
                                          //    );
                                          if (!yyextra->current->argList.empty() && yyextra->current->argList.back().type.isEmpty())
                                          {
                                            yyextra->current->argList.back().type="id";
                                          }
                                          if (yyextra->current->argList.empty()) // method without parameters
                                          {
                                            yyextra->current->argList.setNoParameters(TRUE);
                                          }
                                          yyextra->current->args = argListToString(yyextra->current->argList);
                                          unput('{');
                                          BEGIN( SFunction );
                                        }
<FindMembers>{B}*"sequence"{BN}*"<"{BN}* {
                                          if (yyextra->insideSlice)
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->current->fileName = yyextra->fileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->args.clear();
                                            yyextra->current->section = EntryType::makeTypedef();
                                            yyextra->isTypedef = TRUE;
                                            BEGIN( SliceSequence );
                                          }
                                          else
                                            REJECT;
                                        }
<FindMembers>{B}*"dictionary"{BN}*"<"{BN}* {
                                          if (yyextra->insideSlice)
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->current->fileName = yyextra->fileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->args.clear();
                                            yyextra->current->section = EntryType::makeTypedef() ;
                                            yyextra->isTypedef = TRUE;
                                            BEGIN( SliceDictionary );
                                          }
                                          else
                                            REJECT;
                                        }
<FindMembers>{BN}{1,80}                 {
                                          lineCount(yyscanner);
                                        }
<FindMembers>"@"({ID}".")*{ID}{BN}*"("  {
                                          if (yyextra->insideJava) // Java annotation
                                          {
                                            lineCount(yyscanner);
                                            yyextra->lastSkipRoundContext = YY_START;
                                            yyextra->roundCount=0;
                                            BEGIN( SkipRound );
                                          }
                                          else if (literal_at(yytext,"@property")) // ObjC 2.0 property
                                          {
                                            yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                            yyextra->current->spec.setReadable(true).setWritable(true).setAssign(true);
                                            yyextra->current->protection = Protection::Public ;
                                            unput('(');
                                            BEGIN( ObjCPropAttr );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<ObjCPropAttr>"getter="{ID}             {
                                          yyextra->current->read = yytext+7;
                                        }
<ObjCPropAttr>"setter="{ID}             {
                                          yyextra->current->write = yytext+7;
                                        }
<ObjCPropAttr>"readonly"                {
                                          yyextra->current->spec.setWritable(false);
                                        }
<ObjCPropAttr>"readwrite"               { // default
                                        }
<ObjCPropAttr>"assign"                  { // default
                                        }
<ObjCPropAttr>"unsafe_unretained"       {
                                          yyextra->current->spec.setAssign(false);
                                          yyextra->current->spec.setUnretained(true);
                                        }
<ObjCPropAttr>"retain"                  {
                                          yyextra->current->spec.setAssign(false);
                                          yyextra->current->spec.setRetain(true);
                                        }
<ObjCPropAttr>"copy"                    {
                                          yyextra->current->spec.setAssign(false);
                                          yyextra->current->spec.setCopy(true);
                                        }
<ObjCPropAttr>"weak"                    {
                                          yyextra->current->spec.setAssign(false);
                                          yyextra->current->spec.setWeak(true);
                                        }
<ObjCPropAttr>"strong"                  {
                                          yyextra->current->spec.setAssign(false);
                                          yyextra->current->spec.setStrong(true);
                                        }
<ObjCPropAttr>"nonatomic"               {
                                          yyextra->current->spec.setNonAtomic(true);
                                        }
<ObjCPropAttr>")"                       {
                                          BEGIN(FindMembers);
                                        }
<FindMembers>"@"{ID}("."{ID})+          {
                                          if (yyextra->insideJava) // Java annotation
                                          {
                                            // skip annotation
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>"@"{ID}                    {
                                          if (yyextra->insideJava) // Java annotation
                                          {
                                            // skip annotation
                                          }
                                          else if (qstrcmp(yytext,"@property")==0) // ObjC 2.0 property
                                          {
                                            yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                            yyextra->current->spec.setWritable(true).setReadable(true);
                                            yyextra->current->protection = Protection::Public ;
                                          }
                                          else if (qstrcmp(yytext,"@synthesize")==0)
                                          {
                                            BEGIN( ObjCSkipStatement );
                                          }
                                          else if (qstrcmp(yytext,"@dynamic")==0)
                                          {
                                            BEGIN( ObjCSkipStatement );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<ObjCSkipStatement>";"                  {
                                          BEGIN(FindMembers);
                                        }
<PackageName>{ID}(("."|"\\"){ID})*      {
                                          yyextra->isTypedef=FALSE;
                                          //printf("Found namespace %s lang=%d\n",yytext,yyextra->current->lang);
                                          yyextra->current->name = yytext;
                                          yyextra->current->name = substitute(yyextra->current->name,".","::");
                                          yyextra->current->name = substitute(yyextra->current->name,"\\","::");
                                          yyextra->current->section = EntryType::makeNamespace();
                                          yyextra->current->type = "namespace" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner);
                                        }
<PackageName>";"                        {
                                          std::shared_ptr<Entry> tmp = yyextra->current;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          yyextra->current_root = std::move(tmp);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }
<PackageName>"{"                        {
                                          yyextra->curlyCount=0;
                                          BEGIN( ReadNSBody );
                                        }
<FindMembers>{B}*"export"{BN}+"module"{BN}+ { // primary module interface unit
                                          if (!yyextra->insideCpp) REJECT;
                                          //printf("Interface module unit\n");
                                          yyextra->current->exported = true;
                                          lineCount(yyscanner);
                                          BEGIN( ModuleName );
                                        }
<FindMembers>{B}*"module"{BN}*";"       { // global module section
                                          if (!yyextra->insideCpp) REJECT;
                                          if (!yyextra->current->type.isEmpty() || !yyextra->current->name.isEmpty()) REJECT;
                                          //printf("Implementation module unit\n");
                                          lineCount(yyscanner);
                                          BEGIN( FindMembers );
                                        }
<FindMembers>{B}*"module"{BN}+          { // module implementation unit
                                          if (!yyextra->insideCpp) REJECT;
                                          //printf("Implementation module unit\n");
                                          yyextra->current->exported = false;
                                          lineCount(yyscanner);
                                          BEGIN( ModuleName );
                                        }
<FindMembers>{B}*"export"{BN}+"import"{BN}+  { // export an imported module
                                          if (!yyextra->insideCpp) REJECT;
                                          yyextra->current->exported = true;
                                          lineCount(yyscanner);
                                          BEGIN( ModuleImport );
                                        }
<FindMembers>{B}*"import"{BN}+          { // start of a module import
                                          if (!yyextra->insideCpp) REJECT;
                                          lineCount(yyscanner);
                                          BEGIN( ModuleImport );
                                        }
<ModuleName>{MODULE_ID}{BN}*":"{BN}*{MODULE_ID} { // module partition name, e.g. A.B:C.D'
                                          QCString name = yytext;
                                          int i = name.find(':');
                                          QCString partition = name.mid(i+1).stripWhiteSpace();
                                          name = name.left(i).stripWhiteSpace();
                                          ModuleManager::instance().createModuleDef(yyextra->fileName,
                                                                                    yyextra->yyLineNr,
                                                                                    yyextra->yyColNr,
                                                                                    yyextra->current->exported,
                                                                                    name,
                                                                                    partition);
                                          yyextra->current->section = EntryType::makeModuleDoc();
                                          yyextra->isTypedef=FALSE;
                                          addType(yyscanner);
                                          yyextra->current->type += " module";
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = name+":"+partition;
                                          lineCount(yyscanner);
                                        }
<ModuleName>{MODULE_ID}                 { // primary module name, e.g. A.B
                                          ModuleManager::instance().createModuleDef(yyextra->fileName,
                                                                                    yyextra->yyLineNr,
                                                                                    yyextra->yyColNr,
                                                                                    yyextra->current->exported,
                                                                                    yytext);
                                          yyextra->current->section = EntryType::makeModuleDoc();
                                          yyextra->isTypedef=FALSE;
                                          addType(yyscanner);
                                          yyextra->current->type += " module";
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = yytext;
                                          lineCount(yyscanner);
                                        }
<ModuleName>":"{BN}+"private"           { // start of private section of the module interface
                                          yyextra->current->exported = yyextra->exported = false;
                                          lineCount(yyscanner);
                                        }
<ModuleName>";"                         { unput(';');
                                          BEGIN(FindMembers);
                                        }
<ModuleName>\n                          { lineCount(yyscanner); }
<ModuleName>.                           {}
<ModuleImport>"\""[^"\n]*"\""           { // local header import
                                          ModuleManager::instance().addHeader(yyextra->fileName,
                                                                              yyextra->yyLineNr,
                                                                              QCString(yytext).mid(1,yyleng-2),
                                                                              false);
                                        }
<ModuleImport>"<"[^>\n]*">"             { // system header import
                                          ModuleManager::instance().addHeader(yyextra->fileName,
                                                                              yyextra->yyLineNr,
                                                                              QCString(yytext).mid(1,yyleng-2),
                                                                              true);
                                        }
<ModuleImport>{MODULE_ID}?{BN}*":"{BN}*{MODULE_ID} { // module partition import
                                          QCString name = yytext; // can be 'M:P' or ':P'
                                          int i = name.find(':');
                                          QCString partition = name.mid(i+1).stripWhiteSpace();
                                          name = name.left(i).stripWhiteSpace();
                                          ModuleManager::instance().addImport(yyextra->fileName,
                                                                              yyextra->yyLineNr,
                                                                              name,
                                                                              yyextra->current->exported,
                                                                              partition);
                                          lineCount(yyscanner);
                                        }
<ModuleImport>{MODULE_ID}               { // module import
                                          ModuleManager::instance().addImport(yyextra->fileName,
                                                                              yyextra->yyLineNr,
                                                                              yytext,
                                                                              yyextra->current->exported);
                                          lineCount(yyscanner);
                                        }
<ModuleImport>";"                       { BEGIN(FindMembers); }
<ModuleImport>\n                        { lineCount(yyscanner); }
<ModuleImport>.                         {}
<FindMembers>{B}*"export"{BN}+"{"       {
                                          yyextra->current->exported = yyextra->exported = true; // export block
                                        }
<FindMembers>{B}*"export"{BN}+          {
                                          if (!yyextra->insideCpp) REJECT;
                                          yyextra->current->exported=true;
                                        }
<FindMembers>{B}*"initonly"{BN}+        { if (yyextra->insideJava || yyextra->insideCpp) REJECT;
                                          yyextra->current->type += " initonly ";
                                          if (yyextra->insideCli) yyextra->current->spec.setInitonly(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"static"{BN}*/"{"      { yyextra->current->type += " static ";
                                          yyextra->current->isStatic = TRUE;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"static"{BN}+          { yyextra->current->type += " static ";
                                          yyextra->current->isStatic = TRUE;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"extern"{BN}+          { if (yyextra->insideJava) REJECT;
                                          yyextra->current->isStatic = FALSE;
                                          yyextra->current->explicitExternal = TRUE;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"const"{BN}+           { if (yyextra->insideCS)
                                          {
                                            yyextra->current->type += " const ";
                                            if (yyextra->insideCS) yyextra->current->isStatic = TRUE;
                                            lineCount(yyscanner);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"virtual"{BN}+         { if (yyextra->insideJava) REJECT;
                                          yyextra->current->type += " virtual ";
                                          yyextra->current->virt = Specifier::Virtual;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"constexpr"{BN}+       {
                                          if (yyextra->insideCpp)
                                          {
                                            yyextra->current->spec.setConstExpr(true);
                                          }
                                          REJECT;
                                        }
<FindMembers>{B}*"consteval"{BN}+       {
                                          if (yyextra->insideCpp)
                                          {
                                            yyextra->current->spec.setConstEval(true);
                                          }
                                          REJECT;
                                        }
<FindMembers>{B}*"constinit"{BN}+       {
                                          if (yyextra->insideCpp)
                                          {
                                            yyextra->current->spec.setConstInit(true);
                                          }
                                          REJECT;
                                        }
<FindMembers>{B}*"published"{BN}+       { // UNO IDL published keyword
                                          if (yyextra->insideIDL)
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->spec.setPublished(true);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"sealed"{BN}+          {
                                          if (yyextra->insideCS)
                                          {
                                            yyextra->current->spec.setSealed(true);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"abstract"{BN}+        {
                                          if (yyextra->insidePHP || yyextra->insideCS)
                                          {
                                            yyextra->current->spec.setAbstract(true);
                                          }
                                          else
                                          {
                                            if (yyextra->insideCpp) REJECT;
                                            yyextra->current->type += " abstract ";
                                            if (!yyextra->insideJava)
                                            {
                                              yyextra->current->virt = Specifier::Pure;
                                            }
                                            else
                                            {
                                              yyextra->current->spec.setAbstract(true);
                                            }
                                          }
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"inline"{BN}+          { if (yyextra->insideJava) REJECT;
                                          yyextra->current->spec.setInline(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"mutable"{BN}+         { if (yyextra->insideJava) REJECT;
                                          yyextra->current->spec.setMutable(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"thread_local"{BN}+    { if (yyextra->insideJava) REJECT;
                                          yyextra->current->spec.setThreadLocal(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"explicit"{BN}+        { if (yyextra->insideJava) REJECT;
                                          yyextra->current->spec.setExplicit(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"local"{BN}+           { if (yyextra->insideJava || yyextra->insideCpp) REJECT;
                                          yyextra->current->spec.setLocal(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"@required"{BN}+       { // Objective C 2.0 protocol required section
                                          yyextra->current->spec.setOptional(false).setRequired(true);
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"@optional"{BN}+       {  // Objective C 2.0 protocol optional section
                                          yyextra->current->spec.setRequired(false).setOptional(true);
                                          lineCount(yyscanner);
                                        }
  /*
<FindMembers>{B}*"import"{BN}+          { // IDL import keyword
                                          BEGIN( NextSemi );
                                        }
  */
<FindMembers>{B}*"typename"{BN}+        { lineCount(yyscanner); }
<FindMembers>{B}*"namespace"{BNopt}/[^a-z_A-Z0-9] { if (yyextra->insideJava) REJECT;
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeNamespace();
                                          yyextra->current->type = "namespace" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner);
                                          if (yyextra->insidePHP)
                                          {
                                            BEGIN( PackageName );
                                          }
                                          else
                                          {
                                            BEGIN( CompoundName );
                                          }
                                        }
<FindMembers>{B}*"module"{BN}+          {
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL || yyextra->insideSlice)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeNamespace();
                                            yyextra->current->type = "module" ;
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( CompoundName );
                                          }
                                          else if (yyextra->insideD)
                                          {
                                            lineCount(yyscanner);
                                            BEGIN(PackageName);
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = QCString(yytext).stripWhiteSpace();
                                          }
                                        }
<FindMembers>{B}*"library"{BN}+         {
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeNamespace();
                                            yyextra->current->type = "library" ;
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( CompoundName );
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = QCString(yytext).stripWhiteSpace();
                                          }
                                        }
<FindMembers>{B}*"constants"{BN}+       {  // UNO IDL constant group
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeNamespace();
                                            yyextra->current->type = "constants";
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( CompoundName );
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = QCString(yytext).stripWhiteSpace();
                                          }
                                        }
<FindMembers>{BN}*("service"){BN}+      { // UNO IDL service
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeClass();
                                            TypeSpecifier spec = yyextra->current->spec;
                                            yyextra->current->spec = TypeSpecifier().setService(true).
                                              // preserve UNO IDL [optional] or published
                                              setOptional(spec.isOptional()).setPublished(spec.isPublished());
                                            addType(yyscanner);
                                            yyextra->current->type += " service " ;
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( CompoundName );
                                          }
                                          else // TODO is addType right? just copy/pasted
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = QCString(yytext).stripWhiteSpace();
                                          }
                                        }
<FindMembers>{BN}*("singleton"){BN}+    { // UNO IDL singleton
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeClass();
                                            TypeSpecifier spec = yyextra->current->spec;
                                            yyextra->current->spec = TypeSpecifier().setSingleton(true).
                                              setPublished(spec.isPublished()); // preserve
                                            addType(yyscanner);
                                            yyextra->current->type += " singleton " ;
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( CompoundName );
                                          }
                                          else // TODO is addType right? just copy/pasted
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = QCString(yytext).stripWhiteSpace();
                                          }
                                        }
<FindMembers>{BN}*((("disp")?"interface")|"valuetype"){BN}+     { // M$/Corba/UNO IDL/Java/Slice interface
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL || yyextra->insideJava || yyextra->insideCS || yyextra->insideD || yyextra->insidePHP || yyextra->insideSlice)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeClass();
                                            TypeSpecifier spec = yyextra->current->spec;
                                            yyextra->current->spec = TypeSpecifier().setInterface(true).
                                                // preserve UNO IDL [optional], published, Slice local
                                                setOptional(spec.isOptional()).
                                                setPublished(spec.isPublished()).
                                                setLocal(spec.isLocal());
                                            addType(yyscanner);
                                            yyextra->current->type += " interface" ;
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            setJavaProtection(yyscanner);
                                            BEGIN( CompoundName );
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = QCString(yytext).stripWhiteSpace();
                                          }
                                        }
<FindMembers>{B}*"@implementation"{BN}+ { // Objective-C class implementation
                                          lineCount(yyscanner);
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeObjcImpl();
                                          yyextra->language = yyextra->current->lang = SrcLangExt::ObjC;
                                          yyextra->insideObjC = TRUE;
                                          yyextra->current->protection = yyextra->protection = Protection::Public ;
                                          addType(yyscanner);
                                          yyextra->current->type += " implementation" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          BEGIN( CompoundName );
                                        }
<FindMembers>{B}*"@interface"{BN}+      { // Objective-C class interface, or Java attribute
                                          lineCount(yyscanner);
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setInterface(true);
                                          if (!yyextra->insideJava)
                                          {
                                            yyextra->language = yyextra->current->lang = SrcLangExt::ObjC;
                                            yyextra->insideObjC = TRUE;
                                          }
                                          yyextra->current->protection = yyextra->protection = Protection::Public ;
                                          addType(yyscanner);
                                          yyextra->current->type += " interface" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          BEGIN( CompoundName );
                                        }
<FindMembers>{B}*"@protocol"{BN}+       { // Objective-C protocol definition
                                          lineCount(yyscanner);
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setProtocol(true);
                                          yyextra->language = yyextra->current->lang = SrcLangExt::ObjC;
                                          yyextra->insideObjC = TRUE;
                                          yyextra->current->protection = yyextra->protection = Protection::Public ;
                                          addType(yyscanner);
                                          yyextra->current->type += " protocol" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          BEGIN( CompoundName );
                                        }
<FindMembers>{B}*"exception"{BN}+       { // Corba IDL/Slice exception
                                          if (yyextra->insideJava || yyextra->insideCpp) REJECT;
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          TypeSpecifier spec = yyextra->current->spec;
                                          // preserve UNO IDL, Slice local
                                          yyextra->current->spec = TypeSpecifier().setException(true).
                                            setPublished(spec.isPublished()).setLocal(spec.isLocal());
                                          addType(yyscanner);
                                          yyextra->current->type += " exception" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner);
                                          BEGIN( CompoundName );
                                        }
<FindMembers>"@class" | // for Objective C class declarations
<FindMembers>{B}*{TYPEDEFPREFIX}"class{" |
<FindMembers>{B}*{TYPEDEFPREFIX}"class"{BN}+ {
                                          QCString decl = yytext;
                                          yyextra->isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
                                          yyextra->current->section = EntryType::makeClass();
                                          addType(yyscanner);
                                          if (yyextra->insidePHP && yyextra->current->spec.isAbstract())
                                          {
                                            // convert Abstract to AbstractClass
                                            yyextra->current->spec.setAbstract(false).setAbstractClass(true);
                                          }
                                          if (yyextra->insideSlice && yyextra->current->spec.isLocal())
                                          {
                                            yyextra->current->spec.setLocal(true);
                                          }
                                          if (isConst)
                                          {
                                            yyextra->current->type += " const";
                                          }
                                          else if (isVolatile)
                                          {
                                            yyextra->current->type += " volatile";
                                          }
                                          yyextra->current->type += " class" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          if (yytext[0]=='@')
                                          {
                                            yyextra->language = yyextra->current->lang = SrcLangExt::ObjC;
                                            yyextra->insideObjC = TRUE;
                                          }
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"value class{" |         // C++/CLI extension
<FindMembers>{B}*"value class"{BN}+     {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setValue(true);
                                          addType(yyscanner);
                                          yyextra->current->type += " value class" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"ref class{" |          // C++/CLI extension
<FindMembers>{B}*"ref class"{BN}+       {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setRef(true);
                                          addType(yyscanner);
                                          yyextra->current->type += " ref class" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"interface class{" |     // C++/CLI extension
<FindMembers>{B}*"interface class"{BN}+ {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setInterface(true);
                                          addType(yyscanner);
                                          yyextra->current->type += " interface class" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"coclass"{BN}+         {
                                          if (yyextra->insideIDL)
                                          {
                                            yyextra->isTypedef=FALSE;
                                            yyextra->current->section = EntryType::makeClass();
                                            addType(yyscanner);
                                            yyextra->current->type += " coclass" ;
                                            yyextra->current->fileName  = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            lineCount(yyscanner) ;
                                            BEGIN( CompoundName ) ;
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = yytext;
                                            yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                            lineCount(yyscanner);
                                          }
                                        }
<FindMembers>{B}*{TYPEDEFPREFIX}"struct{" |
<FindMembers>{B}*{TYPEDEFPREFIX}"struct"/{BN}+ {
                                          if (yyextra->insideJava) REJECT;
                                          QCString decl = yytext;
                                          yyextra->isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
                                          yyextra->current->section = EntryType::makeClass() ;
                                          TypeSpecifier spec = yyextra->current->spec;
                                          yyextra->current->spec = TypeSpecifier().setStruct(true).
                                          // preserve UNO IDL & Inline attributes, Slice local
                                              setPublished(spec.isPublished()).
                                              setInline(spec.isInline()).
                                              setLocal(spec.isLocal());
                                          // bug 582676: can be a struct nested in an interface so keep yyextra->insideObjC state
                                          //yyextra->current->objc    = yyextra->insideObjC = FALSE;
                                          addType(yyscanner);
                                          if (isConst)
                                          {
                                            yyextra->current->type += " const";
                                          }
                                          else if (isVolatile)
                                          {
                                            yyextra->current->type += " volatile";
                                          }
                                          yyextra->current->type += " struct" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"value struct{" |      // C++/CLI extension
<FindMembers>{B}*"value struct"{BN}+     {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setStruct(true).setValue(true);
                                          addType(yyscanner);
                                          yyextra->current->type += " value struct" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"ref struct{" |       // C++/CLI extension
<FindMembers>{B}*"ref struct"{BN}+     {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setStruct(true).setRef(true);
                                          addType(yyscanner);
                                          yyextra->current->type += " ref struct" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"interface struct{" |     // C++/CLI extension
<FindMembers>{B}*"interface struct"{BN}+ {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setStruct(true).setInterface(true);
                                          addType(yyscanner);
                                          yyextra->current->type += " interface struct";
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*{TYPEDEFPREFIX}"union{" |
<FindMembers>{B}*{TYPEDEFPREFIX}"union"{BN}+ {
                                          if (yyextra->insideJava) REJECT;
                                          QCString decl=yytext;
                                          yyextra->isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
                                          yyextra->current->section = EntryType::makeClass();
                                          yyextra->current->spec = TypeSpecifier().setUnion(true);
                                          // bug 582676: can be a struct nested in an interface so keep yyextra->insideObjC state
                                          //yyextra->current->objc    = yyextra->insideObjC = FALSE;
                                          addType(yyscanner);
                                          if (isConst)
                                          {
                                            yyextra->current->type += " const";
                                          }
                                          else if (isVolatile)
                                          {
                                            yyextra->current->type += " volatile";
                                          }
                                          yyextra->current->type += " union" ;
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*{TYPEDEFPREFIX}{IDLATTR}?"enum"({BN}+("class"|"struct"))?"{" |
<FindMembers>{B}*{TYPEDEFPREFIX}{IDLATTR}?"enum"({BN}+("class"|"struct"))?{BN}+ { // for IDL: typedef [something] enum
                                          QCString text=yytext;
                                          yyextra->isTypedef    = text.find("typedef")!=-1;
                                          bool isStrongEnum =  text.find("class")!=-1 || yyextra->insideCS;
                                          bool isEnumSytruct = text.find("struct")!=-1;
                                          if (yyextra->insideJava)
                                          {
                                            yyextra->current->section = EntryType::makeClass();
                                            setJavaProtection(yyscanner);
                                            yyextra->current->spec = TypeSpecifier().setEnum(true);
                                          }
                                          else
                                          {
                                            yyextra->current->section = EntryType::makeEnum() ;
                                          }
                                          addType(yyscanner);
                                          yyextra->current->type += " enum";
                                          if (isStrongEnum)
                                          {
                                            yyextra->current->spec.setStrong(true);
                                          }
                                          if (isEnumSytruct)
                                          {
                                            yyextra->current->spec.setStrong(true).setEnumStruct(true);
                                          }
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"concept"{BN}+         { // C++20 concept
                                          if (yyextra->insideJava) REJECT;
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = EntryType::makeConcept();
                                          addType(yyscanner);
                                          yyextra->current->type += " concept";
                                          yyextra->current->fileName  = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->args = yyextra->fullArgString;
                                          yyextra->keepComment = true;
                                          lineCount(yyscanner) ;
                                          BEGIN( ConceptName ) ;
                                        }
<Operator>"("{BN}*")"({BN}*"<"[^>]*">"){BNopt}/"("  {  // A::operator()<int>(int arg)
                                          lineCount(yyscanner);
                                          yyextra->current->name += "()";
                                          BEGIN( FindMembers );
                                        }
<Operator>"("{BN}*")"{BNopt}/"("        {
                                          lineCount(yyscanner);
                                          yyextra->current->name += yytext ;
                                          yyextra->current->name = yyextra->current->name.simplifyWhiteSpace();
                                          BEGIN( FindMembers ) ;
                                        }
<Operator>";"                           { // can occur when importing members
                                          unput(';');
                                          BEGIN( FindMembers ) ;
                                        }
<Operator>[^(]                          {
                                          lineCount(yyscanner);
                                          yyextra->current->name += *yytext ;
                                        }
<Operator>"<"({B}*{ID}{B}*(","{B}*{BN})*{B}*)?">" { /* skip guided templ specifiers */ 
                                          if (!yyextra->current->type.startsWith("friend "))
                                          {
                                            yyextra->current->name += yytext;
                                          }
                                        }
<Operator>"("                           {
                                          yyextra->current->name = yyextra->current->name.simplifyWhiteSpace();
                                          unput(*yytext);
                                          BEGIN( FindMembers ) ;
                                        }
<FindMembers>("template"|"generic")({BN}*)"<"/[>]?      {  // generic is a C++/CLI extension
                                          lineCount(yyscanner);
                                          ArgumentList al;
                                          yyextra->current->tArgLists.push_back(al);
                                          yyextra->currentArgumentList = &yyextra->current->tArgLists.back();
                                          yyextra->templateStr="<";
                                          yyextra->fullArgString = yyextra->templateStr;
                                          yyextra->copyArgString = &yyextra->templateStr;
                                          yyextra->currentArgumentContext = FindMembers;
                                          BEGIN( ReadTempArgs );
                                        }
<FindMembers>"namespace"{BN}+/{ID}{BN}*"=" { // namespace alias
                                          if (yyextra->insideJava) REJECT;
                                          lineCount(yyscanner);
                                          BEGIN( NSAliasName );
                                        }
<NSAliasName>{ID}                       {
                                          yyextra->aliasName = yytext;
                                          BEGIN( NSAliasArg );
                                        }
<NSAliasArg>({ID}"::")*{ID}             {
                                          //printf("Inserting namespace alias %s::%s->%s\n",qPrint(yyextra->current_root->name),qPrint(yyextra->aliasName),yytext);
                                          std::string ctx = yyextra->current_root->name.str();
                                          if (ctx.empty())
                                          {
                                            Doxygen::namespaceAliasMap.emplace(yyextra->aliasName.str(),NamespaceAliasInfo(std::string(yytext),std::string()));
                                          }
                                          else
                                          {
                                            Doxygen::namespaceAliasMap.emplace(ctx+"::"+yyextra->aliasName.str(),NamespaceAliasInfo(std::string(yytext),ctx));
                                          }
                                        }
<NSAliasArg>";"                         {
                                          BEGIN( FindMembers );
                                        }
<PHPUse>({ID}{BN}*"\\"{BN}*)*{ID}/{BN}+"as"  {
                                          lineCount(yyscanner);
                                          yyextra->aliasName=yytext;
                                          BEGIN(PHPUseAs);
                                        }
<PHPUse>({ID}{BN}*"\\"{BN}*)*{ID}       {
                                          lineCount(yyscanner);
                                          yyextra->current->name=removeRedundantWhiteSpace(substitute(yytext,"\\","::"));
                                          //printf("PHP: adding use relation: %s\n",qPrint(yyextra->current->name));
                                          yyextra->current->fileName = yyextra->fileName;
                                          // add a using declaration
                                          yyextra->current->section = EntryType::makeUsingDecl();
                                          yyextra->current_root->copyToSubEntry(yyextra->current);
                                          // also add it as a using directive
                                          yyextra->current->section = EntryType::makeUsingDir();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          yyextra->aliasName.clear();
                                        }
<PHPUseAs>{BN}+"as"{BN}+                {
                                          lineCount(yyscanner);
                                        }
<PHPUseAs>{PHPUSEKW}                    {
                                        }
<PHPUseAs>{ID}                          {
                                          //printf("PHP: adding use as relation: %s->%s\n",yytext,qPrint(yyextra->aliasName));
                                          if (!yyextra->aliasName.isEmpty())
                                          {
                                            std::string aliasValue = removeRedundantWhiteSpace(substitute(yyextra->aliasName,"\\","::")).str();
                                            Doxygen::namespaceAliasMap.emplace(yytext,NamespaceAliasInfo(aliasValue));
                                          }
                                          yyextra->aliasName.clear();
                                        }
<PHPUse,PHPUseAs>[,;]                   {
                                          if (*yytext==',')
                                          {
                                            BEGIN(PHPUse);
                                          }
                                          else
                                          {
                                            BEGIN(FindMembers);
                                          }
                                        }
<JavaImport>({ID}{BN}*"."{BN}*)+"*"     { // package import => add as a using directive
                                          lineCount(yyscanner);
                                          QCString scope=yytext;
                                          yyextra->current->name=removeRedundantWhiteSpace(substitute(scope.left(scope.length()-1),".","::"));
                                          yyextra->current->fileName = yyextra->fileName;
                                          bool ambig = false;
                                          FileDef *incFd = findFileDef(Doxygen::inputNameLinkedMap,yyextra->fileName,ambig);
                                          if (incFd)
                                          {
                                            incFd->addIncludeDependency(nullptr,scope,IncludeKind::ImportModule);
                                          }
                                          yyextra->current->section = EntryType::makeUsingDir();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(Using);
                                        }
<JavaImport>({ID}{BN}*"."{BN}*)+{ID}    { // class import => add as a using declaration
                                          lineCount(yyscanner);
                                          QCString scope=yytext;
                                          yyextra->current->name=removeRedundantWhiteSpace(substitute(scope,".","::"));
                                          yyextra->current->fileName = yyextra->fileName;
                                          bool ambig = false;
                                          FileDef *fromFd = findFileDef(Doxygen::inputNameLinkedMap,yyextra->fileName,ambig);
                                          FileDef *toFd = findFileDef(Doxygen::inputNameLinkedMap,scope,ambig);
                                          if (fromFd)
                                          {
                                            fromFd->addIncludeDependency(toFd,scope,IncludeKind::ImportModule);
                                          }
                                          if (toFd && fromFd)
                                          {
                                            toFd->addIncludedByDependency(fromFd,fromFd->docName(),IncludeKind::ImportModule);
                                          }
                                          if (yyextra->insideD)
                                          {
                                            yyextra->current->section = EntryType::makeUsingDir();
                                          }
                                          else
                                          {
                                            //printf("import name = %s -> %s\n",yytext,qPrint(yyextra->current->name));
                                            yyextra->current->section = EntryType::makeUsingDecl();
                                          }
                                          yyextra->previous = yyextra->current;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(Using);
                                        }
<IDLImport>"\""[^"]*"\""                {
                                          QCString fileName(&yytext[1],yyleng-2);
                                          bool ambig = false;
                                          FileDef *fromFd = findFileDef(Doxygen::inputNameLinkedMap,yyextra->fileName,ambig);
                                          FileDef *toFd = findFileDef(Doxygen::inputNameLinkedMap,fileName,ambig);
                                          if (fromFd)
                                          {
                                            fromFd->addIncludeDependency(toFd,fileName,IncludeKind::ImportModule);
                                          }
                                          if (toFd && fromFd)
                                          {
                                            toFd->addIncludedByDependency(fromFd,fromFd->docName(),IncludeKind::ImportModule);
                                          }
                                        }
<IDLImport>";"                          {
                                          BEGIN(FindMembers);
                                        }
<FindMembers>"using"{BN}+/("::"{ID}("::"{ID})*)? {
                                          if (yyextra->insideJava) REJECT;
                                          yyextra->current->startLine=yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          lineCount(yyscanner);
                                          BEGIN(Using);
                                        }
<Using>"namespace"{BN}+                 { lineCount(yyscanner); BEGIN(UsingDirective); }
<Using>("::")?({ID}{BN}*("::"|"."){BN}*)*({ID}|{OPERATOR}|{FUNCOP}) {
                                          lineCount(yyscanner);
                                          yyextra->current->name=yytext;
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->section = EntryType::makeUsingDecl();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->previous = yyextra->current;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          if (yyextra->insideCS) /* Hack: in C# a using declaration and
                                                                    directive have the same syntax, so we
                                                                    also add it as a using directive here
                                                                  */
                                          {
                                            yyextra->current->name=yytext;
                                            yyextra->current->fileName = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->section = EntryType::makeUsingDir();
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          BEGIN(Using);
                                        }
<Using>({ID}{BN}*("::"|"."){BN}*)*({ID}|{OPERATOR}){BN}*"=" { // C++11 style using alias
                                          yyextra->current->name=QCString(yytext).left(yyleng-1).stripWhiteSpace();
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->section = EntryType::makeUsingDecl();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->lastInitializerContext = UsingAlias;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          storeClangId(yyscanner,yyextra->current->name.data());
                                          BEGIN(ReadInitializer);
                                        }
<UsingAlias>";"                         {
                                          yyextra->current->section = EntryType::makeVariable();
                                          QCString init = yyextra->current->initializer.str();
                                          init.stripPrefix("class ");
                                          init.stripPrefix("struct ");
                                          init = init.stripWhiteSpace();
                                          yyextra->current->type = "typedef "+init;
                                          yyextra->current->args.clear();
                                          yyextra->current->spec.setAlias(true);
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }
<UsingAlias>.                           {
                                          yyextra->current->initializer << yytext;
                                        }
<UsingAlias>\n                          {
                                          yyextra->current->initializer << yytext;
                                          lineCount(yyscanner);
                                        }
<UsingDirective>{SCOPENAME}             { yyextra->current->name=removeRedundantWhiteSpace(yytext);
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->section = EntryType::makeUsingDir();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(Using);
                                        }
<Using>";"                              { BEGIN(FindMembers); }
<FindMembers>{SCOPENAME}{BN}*"<>"       { // guided template decl
                                          QCString n=yytext;
                                          addType(yyscanner);
                                          yyextra->current->name=n.left(n.length()-2);
                                        }
<FindMembers>{SCOPENAME}{BNopt}/"<"     { // Note: this could be a return type!
                                          QCString name = QCString(yytext).stripWhiteSpace();
                                          if (yyextra->insideCpp && name=="import") REJECT; // C++20 module header import
                                          yyextra->roundCount=0;
                                          yyextra->sharpCount=0;
                                          lineCount(yyscanner);
                                          addType(yyscanner);
                                          yyextra->current->name=name;
                                          //yyextra->current->scopeSpec.clear();
                                          // yyextra->currentTemplateSpec = &yyextra->current->scopeSpec;
                                          if (nameIsOperator(yyextra->current->name))
                                            BEGIN( Operator );
                                          else
                                            BEGIN( EndTemplate );
                                        }
<FindMemberName>{SCOPENAME}{BNopt}/"<"  {
                                          yyextra->sharpCount=0;
                                          yyextra->roundCount=0;
                                          lineCount(yyscanner);
                                          yyextra->current->name+=((QCString)yytext).stripWhiteSpace();
                                          //yyextra->current->memberSpec.clear();
                                          // yyextra->currentTemplateSpec = &yyextra->current->memberSpec;
                                          if (nameIsOperator(yyextra->current->name))
                                            BEGIN( Operator );
                                          else
                                            BEGIN( EndTemplate );
                                        }
<EndTemplate>"<<<"                      {
                                          if (!yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->lastHereDocContext = YY_START;
                                            BEGIN(HereDoc);
                                          }
                                        }
<ClassTemplSpec,EndTemplate>("<<"|"<=") {
                                          yyextra->current->name+=yytext;
                                          // *yyextra->currentTemplateSpec+=yytext;
                                        }
<EndTemplate>"<"                        {
                                          if (yyextra->roundCount==0)
                                          {
                                            // *yyextra->currentTemplateSpec+='<';
                                            yyextra->sharpCount++;
                                          }
                                          yyextra->current->name+=yytext;
                                        }
<ClassTemplSpec,EndTemplate>">="        {
                                          yyextra->current->name+=yytext;
                                        }
<ClassTemplSpec,EndTemplate>(">>")      {
                                          if (yyextra->insideJava || yyextra->insideCS || yyextra->insideCli || yyextra->roundCount==0)
                                          {
                                            unput('>');
                                            unput(' ');
                                            unput('>');
                                          }
                                          else
                                          {
                                            yyextra->current->name+=yytext;
                                          }
                                          // *yyextra->currentTemplateSpec+=yytext;
                                        }
<EndTemplate>">"                        {
                                          yyextra->current->name+='>';
                                          // *yyextra->currentTemplateSpec+='>';
                                          if (yyextra->roundCount==0 && --yyextra->sharpCount<=0)
                                          {
                                            yyextra->current->name = yyextra->current->name.simplifyWhiteSpace();
                                            //printf("Found %s\n",qPrint(yyextra->current->name));
                                            BEGIN(FindMembers);
                                          }
                                        }
<EndTemplate>">"{BN}*"("                {
                                          lineCount(yyscanner);
                                          yyextra->current->name+='>';
                                          // *yyextra->currentTemplateSpec+='>';
                                          if (yyextra->roundCount==0)
                                          {
                                            --yyextra->sharpCount;
                                          }
                                          if (yyextra->roundCount==0 && yyextra->sharpCount<=0)
                                          {
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->current->args = "(";
                                            yyextra->currentArgumentContext = FuncQual;
                                            yyextra->fullArgString = yyextra->current->args;
                                            yyextra->copyArgString = &yyextra->current->args;
                                            //printf("Found %s\n",qPrint(yyextra->current->name));
                                            BEGIN( ReadFuncArgType ) ;
                                          }
                                          else
                                          {
                                             yyextra->current->name+="(";
                                             yyextra->roundCount++;
                                          }
                                        }
<EndTemplate>">"{BNopt}/"("({BN}*{TSCOPE}{BN}*"::")*({BN}*"*"{BN}*)+ { // function pointer returning a template instance
                                          lineCount(yyscanner);
                                          yyextra->current->name+='>';
                                          if (yyextra->roundCount==0 && --yyextra->sharpCount<=0)
                                          {
                                            BEGIN(FindMembers);
                                          }
                                        }
<EndTemplate>">"{BNopt}/"::"              {
                                          lineCount(yyscanner);
                                          yyextra->current->name+='>';
                                          // *yyextra->currentTemplateSpec+='>';
                                          if (yyextra->roundCount==0 && --yyextra->sharpCount<=0)
                                          {
                                            BEGIN(FindMemberName);
                                          }
                                        }
<ClassTemplSpec,EndTemplate>"("         { yyextra->current->name+=*yytext;
                                          yyextra->roundCount++;
                                        }
<ClassTemplSpec,EndTemplate>")"         { yyextra->current->name+=*yytext;
                                          if (yyextra->roundCount>0) yyextra->roundCount--;
                                        }
<EndTemplate>.                          {
                                          yyextra->current->name+=*yytext;
                                          // *yyextra->currentTemplateSpec+=*yytext;
                                        }
<FindMembers>"define"{BN}*"("{BN}*["']  {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( DefinePHP );
                                          }
                                          else
                                            REJECT;
                                        }
<CopyHereDoc>{ID}                       { // PHP heredoc
                                          yyextra->delimiter = yytext;
                                          *yyextra->pCopyHereDocGString << yytext;
                                          BEGIN(CopyHereDocEnd);
                                        }
<CopyHereDoc>"\""{ID}/"\""              { // PHP quoted heredoc
                                          yyextra->delimiter = &yytext[1];
                                          *yyextra->pCopyHereDocGString << yytext;
                                          BEGIN(CopyHereDocEnd);
                                        }
<CopyHereDoc>"'"{ID}/"'"                { // PHP nowdoc
                                          yyextra->delimiter = &yytext[1];
                                          *yyextra->pCopyHereDocGString << yytext;
                                          BEGIN(CopyHereDocEnd);
                                        }
<HereDoc>{ID}                           { // PHP heredoc
                                          yyextra->delimiter = yytext;
                                          BEGIN(HereDocEnd);
                                        }
<HereDoc>"\""{ID}/"\""                  { // PHP quoted heredoc
                                          yyextra->delimiter = &yytext[1];
                                          BEGIN(HereDocEnd);
                                        }
<HereDoc>"'"{ID}/"'"                    { // PHP nowdoc
                                          yyextra->delimiter = &yytext[1];
                                          BEGIN(HereDocEnd);
                                        }
<HereDocEnd>^{Bopt}{ID}                 { // id at start of the line could mark the end of the block
                                          if (yyextra->delimiter==QCString(yytext).stripWhiteSpace()) // it is the end marker
                                          {
                                            BEGIN(yyextra->lastHereDocContext);
                                          }
                                        }
<HereDocEnd>.                           { }
<CopyHereDocEnd>^{Bopt}{ID}             { // id at start of the line could mark the end of the block
                                          *yyextra->pCopyHereDocGString << yytext;
                                          if (yyextra->delimiter==QCString(yytext).stripWhiteSpace()) // it is the end marker
                                          {
                                            BEGIN(yyextra->lastHereDocContext);
                                          }
                                        }
<CopyHereDocEnd>\n                      {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyHereDocGString <<  yytext;
                                        }
<CopyHereDocEnd>{ID}                    {
                                          *yyextra->pCopyHereDocGString <<  yytext;
                                        }
<CopyHereDocEnd>.                       {
                                          *yyextra->pCopyHereDocGString <<  yytext;
                                        }
<FindMembers>"Q_OBJECT"|"Q_GADGET"      { // Qt object / gadget macro
                                        }
<FindMembers>"Q_PROPERTY"               { // Qt property declaration
                                          yyextra->yyBegLineNr = yyextra->yyLineNr;
                                          yyextra->yyBegColNr  = yyextra->yyColNr;
                                          yyextra->current->protection = Protection::Public ; // see bug734245 & bug735462
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                          yyextra->current->type.clear();
                                          BEGIN(QtPropType);
                                        }
<QtPropType>"("                         { // start of property arguments
                                        }
<QtPropAttr>")"                         { // end of property arguments
                                          unput(';');
                                          BEGIN(FindMembers);
                                        }
<QtPropType>{BN}+                       {
                                          yyextra->current->name+=yytext;
                                        }
<QtPropType>"*"                         {
                                          yyextra->current->type+= yyextra->current->name;
                                          yyextra->current->type+= yytext;
                                          yyextra->current->name="";
                                        }
<QtPropType>({TSCOPE}"::")*{TSCOPE}     {
                                          yyextra->current->type+= yyextra->current->name;
                                          yyextra->current->name=yytext;
                                        }
<QtPropType,QtPropAttr>{BN}+"READ"{BN}+   {
                                          yyextra->current->spec.setReadable(true);
                                          BEGIN(QtPropRead);
                                        }
<QtPropType,QtPropAttr>{BN}+"WRITE"{BN}+  {
                                          yyextra->current->spec.setWritable(true);
                                          BEGIN(QtPropWrite);
                                        }
<QtPropType,QtPropAttr>{BN}+"MEMBER"{BN}+{ID}     | // member property => not supported yet
<QtPropType,QtPropAttr>{BN}+"RESET"{BN}+{ID}      | // reset method => not supported yet
<QtPropType,QtPropAttr>{BN}+"SCRIPTABLE"{BN}+{ID} | // scriptable property => not supported yet
<QtPropType,QtPropAttr>{BN}+"DESIGNABLE"{BN}+{ID} | // designable property => not supported yet
<QtPropType,QtPropAttr>{BN}+"NOTIFY"{BN}+{ID}     | // notify property => not supported yet
<QtPropType,QtPropAttr>{BN}+"REVISION"{BN}+{ID}   | // revision property => not supported yet
<QtPropType,QtPropAttr>{BN}+"STORED"{BN}+{ID}     | // stored property => not supported yet
<QtPropType,QtPropAttr>{BN}+"USER"{BN}+{ID}       | // user property => not supported yet
<QtPropType,QtPropAttr>{BN}+"CONSTANT"{BN}        | // constant property => not supported yet
<QtPropType,QtPropAttr>{BN}+"FINAL"{BN}           { // final property => not supported yet
                                          BEGIN(QtPropAttr);
                                        }
<QtPropRead>{ID}                        {
                                          yyextra->current->read = yytext;
                                          BEGIN(QtPropAttr);
                                        }
<QtPropWrite>{ID}                       {
                                          yyextra->current->write = yytext;
                                          BEGIN(QtPropAttr);
                                        }
<FindMembers>"friend"{BN}+("class"|"union"|"struct"){BN}+ {
                                          yyextra->current->name=yytext;
                                          lineCount(yyscanner) ;
                                          BEGIN(FindMembers);
                                        }
<FindMembers>"requires"                 { // C++20 requires clause
                                          if (yyextra->insideJava) REJECT;
                                          yyextra->current->req.clear();
                                          yyextra->requiresContext = YY_START;
                                          BEGIN(RequiresClause);
                                        }
<RequiresClause>"requires"{BN}*/"{"     { // requires requires { ... }
                                          if (yyextra->insideJava) REJECT;
                                          lineCount(yyscanner) ;
                                          yyextra->current->req+=yytext;
                                          BEGIN( RequiresExpression ) ;
                                        }
<RequiresClause>"requires"{BN}*"("      { // requires requires(T x) { ... }
                                          if (yyextra->insideJava) REJECT;
                                          lineCount(yyscanner) ;
                                          yyextra->current->req+=yytext;
                                          yyextra->lastRoundContext=RequiresExpression;
                                          yyextra->pCopyRoundString=&yyextra->current->req;
                                          yyextra->roundCount=0;
                                          BEGIN( CopyRound ) ;
                                        }
<RequiresExpression>"{"                 {
                                          yyextra->current->req+=yytext;
                                          yyextra->lastCurlyContext=RequiresClause;
                                          yyextra->pCopyCurlyString=&yyextra->current->req;
                                          yyextra->curlyCount=0;
                                          BEGIN( CopyCurly ) ;
                                        }
<RequiresExpression>\n                  {
                                          yyextra->current->req+=' ';
                                          lineCount(yyscanner);
                                        }
<RequiresExpression>.                   {
                                          yyextra->current->req+=yytext;
                                        }
<RequiresClause>"("                     { // requires "(A && B)"
                                          yyextra->current->req+=yytext;
                                          yyextra->lastRoundContext=RequiresClause;
                                          yyextra->pCopyRoundString=&yyextra->current->req;
                                          yyextra->roundCount=0;
                                          BEGIN( CopyRound ) ;
                                        }
<RequiresClause>{NOTopt}{SCOPENAME}{BNopt}"("   { // "requires func(x)"
                                          if (startOfRequiresExpression(yyextra->current->req))
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->req+=yytext;
                                            yyextra->lastRoundContext=RequiresClause;
                                            yyextra->pCopyRoundString=&yyextra->current->req;
                                            yyextra->roundCount=0;
                                            BEGIN( CopyRound );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<RequiresClause>{NOTopt}{SCOPENAME}{BNopt}"<"   { // "requires C<S,T>"
                                          if (startOfRequiresExpression(yyextra->current->req))
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->req+=yytext;
                                            yyextra->lastSharpContext=RequiresClause;
                                            yyextra->pCopySharpString=&yyextra->current->req;
                                            yyextra->sharpCount=0;
                                            BEGIN( CopySharp );
                                          }
                                          else
                                          {
                                            REJECT
                                          }
                                        }
<RequiresClause>{NOTopt}{SCOPENAME}     { // something like "requires true" or "requires !my::value"
                                          if (startOfRequiresExpression(yyextra->current->req))
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->req=yytext;
                                            BEGIN(yyextra->requiresContext);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<RequiresClause>{NOTopt}"::"{ID}        {
                                          lineCount(yyscanner);
                                          yyextra->current->req+=yytext;
                                        }
<RequiresClause>"||"|"&&"|"!"|("or"{BN}+)|("and"{BN}+)|("not"{BN}+)  { // "requires A || B" or "requires A && B"
                                          lineCount(yyscanner);
                                          yyextra->current->req+=yytext;
                                        }
<RequiresClause>{BN}+                   {
                                          yyextra->current->req+=' ';
                                          lineCount(yyscanner) ;
                                        }
<RequiresClause>.                       {
                                          unput(*yytext);
                                          yyextra->current->req=yyextra->current->req.simplifyWhiteSpace();
                                          BEGIN(yyextra->requiresContext);
                                        }
<FindMembers,FindMemberName>{SCOPENAME} {
                                          storeClangId(yyscanner,yytext);
                                          yyextra->yyBegColNr=yyextra->yyColNr;
                                          yyextra->yyBegLineNr=yyextra->yyLineNr;
                                          lineCount(yyscanner);
                                          if (yyextra->insideIDL && yyleng==9 && qstrcmp(yytext,"cpp_quote")==0)
                                          {
                                            BEGIN(CppQuote);
                                          }
                                          else if ((yyextra->insideIDL || yyextra->insideJava || yyextra->insideD) && yyleng==6 && qstrcmp(yytext,"import")==0)
                                          {
                                            if (yyextra->insideIDL)
                                              BEGIN(IDLImport);
                                            else // yyextra->insideJava or yyextra->insideD
                                              BEGIN(JavaImport);
                                          }
                                          else if (yyextra->insidePHP && qstrcmp(yytext,"use")==0)
                                          {
                                            BEGIN(PHPUse);
                                          }
                                          else if (yyextra->insideJava && qstrcmp(yytext,"package")==0)
                                          {
                                            lineCount(yyscanner);
                                            BEGIN(PackageName);
                                          }
                                          else if (yyextra->insideIDL && qstrcmp(yytext,"case")==0)
                                          {
                                            BEGIN(IDLUnionCase);
                                          }
                                          else if (yyextra->insideTryBlock && qstrcmp(yytext,"catch")==0)
                                          {
                                            yyextra->insideTryBlock=FALSE;
                                            BEGIN(TryFunctionBlock);
                                          }
                                          else if (yyextra->insideCpp && qstrcmp(yytext,"alignas")==0)
                                          {
                                            yyextra->lastAlignAsContext = YY_START;
                                            BEGIN(AlignAs);
                                          }
                                          else if (yyextra->insideJS && qstrcmp(yytext,"var")==0)
                                          { // javascript variable
                                            yyextra->current->type="var";
                                          }
                                          else if (yyextra->insideJS && qstrcmp(yytext,"function")==0)
                                          { // javascript function
                                            yyextra->current->type="function";
                                          }
                                          else if (yyextra->insideCS && qstrcmp(yytext,"this")==0)
                                          {
                                            // C# indexer
                                            addType(yyscanner);
                                            yyextra->current->name="this";
                                            BEGIN(CSIndexer);
                                          }
                                          else if (yyextra->insideCpp && (qstrcmp(yytext,"static_assert")==0 || qstrcmp(yytext,"_Static_assert")==0))
                                          {
                                            // C/C++11 static_assert
                                            BEGIN(StaticAssert);
                                          }
                                          else if (yyextra->insideCpp && qstrcmp(yytext,"decltype")==0)
                                          {
                                            // C++11 decltype(x)
                                            addType(yyscanner);
                                            if (!yyextra->current->type.isEmpty()) yyextra->current->type+=' ';
                                            yyextra->current->type+=yytext;
                                            BEGIN(DeclType);
                                          }
                                          else if (yyextra->insideSlice && qstrcmp(yytext,"optional")==0)
                                          {
                                            if (yyextra->current->type.isEmpty())
                                            {
                                              yyextra->current->type = "optional";
                                            }
                                            else
                                            {
                                              yyextra->current->type += " optional";
                                            }
                                            yyextra->lastModifierContext = YY_START;
                                            BEGIN(SliceOptional);
                                          }
                                          else
                                          {
                                            if (YY_START==FindMembers)
                                            {
                                              addType(yyscanner);
                                            }
                                            bool javaLike = yyextra->insideJava || yyextra->insideCS || yyextra->insideD || yyextra->insidePHP || yyextra->insideJS;
                                            if (javaLike && qstrcmp(yytext,"public")==0)
                                            {
                                              yyextra->current->protection = Protection::Public;
                                            }
                                            else if (javaLike && qstrcmp(yytext,"protected")==0)
                                            {
                                              yyextra->current->protection = Protection::Protected;
                                            }
                                            else if ((yyextra->insideCS || yyextra->insideD || yyextra->insidePHP || yyextra->insideJS) && qstrcmp(yytext,"internal")==0)
                                            {
                                              yyextra->current->protection = Protection::Package;
                                            }
                                            else if (javaLike && qstrcmp(yytext,"private")==0)
                                            {
                                              yyextra->current->protection = Protection::Private;
                                            }
                                            else if (javaLike && qstrcmp(yytext,"static")==0)
                                            {
                                              if (YY_START==FindMembers)
                                                yyextra->current->name  = yytext;
                                              else
                                                yyextra->current->name += yytext;
                                              yyextra->current->isStatic = TRUE;
                                            }
                                            else
                                            {
                                              if (YY_START==FindMembers)
                                                yyextra->current->name  = yytext;
                                              else
                                                yyextra->current->name += yytext;
                                              if (yyextra->current->name.startsWith("static "))
                                              {
                                                yyextra->current->isStatic = TRUE;
                                                yyextra->current->name= yyextra->current->name.mid(7);
                                              }
                                              else if (yyextra->current->name.startsWith("inline "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="inline";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="inline ";
                                                }
                                                yyextra->current->name= yyextra->current->name.mid(7);
                                              }
                                              else if (yyextra->current->name.startsWith("constexpr "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="constexpr";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="constexpr ";
                                                }
                                                yyextra->current->name=yyextra->current->name.mid(10);
                                              }
                                              else if (yyextra->current->name.startsWith("consteval "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="consteval";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="consteval ";
                                                }
                                                yyextra->current->name=yyextra->current->name.mid(10);
                                              }
                                              else if (yyextra->current->name.startsWith("constinit "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="constinit";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="constinit ";
                                                }
                                                yyextra->current->name=yyextra->current->name.mid(10);
                                              }
                                              else if (yyextra->current->name.startsWith("const "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="const";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="const ";
                                                }
                                                yyextra->current->name=yyextra->current->name.mid(6);
                                              }
                                              else if (yyextra->current->name.startsWith("volatile "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="volatile";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="volatile ";
                                                }
                                                yyextra->current->name=yyextra->current->name.mid(9);
                                              }
                                              else if (yyextra->current->name.startsWith("typedef "))
                                              {
                                                if (yyextra->current->type.isEmpty())
                                                {
                                                  yyextra->current->type="typedef";
                                                }
                                                else
                                                {
                                                  yyextra->current->type+="typedef ";
                                                }
                                                yyextra->current->name=yyextra->current->name.mid(8);
                                              }
                                            }
                                            QCString tmp=yytext;
                                            if (nameIsOperator(tmp))
                                            {
                                              BEGIN( Operator );
                                            }
                                            else
                                            {
                                              yyextra->externLinkage=FALSE; // see bug759247
                                              BEGIN(FindMembers);
                                            }
                                          }
                                          yyextra->current->name = yyextra->current->name.removeWhiteSpace();
                                        }
<StaticAssert>"("                       {
                                          yyextra->lastSkipRoundContext = FindMembers;
                                          yyextra->roundCount=0;
                                          BEGIN(SkipRound);
                                        }
<StaticAssert>{BN}+                     { lineCount(yyscanner); }
<StaticAssert>.                         { // variable with static_assert as name?
                                          unput(*yytext);
                                          BEGIN(FindMembers);
                                        }
<DeclType>"("                           {
                                          yyextra->current->type+=yytext;
                                          yyextra->lastRoundContext=FindMembers;
                                          yyextra->pCopyRoundString=&yyextra->current->type;
                                          yyextra->roundCount=0;
                                          BEGIN(CopyRound);
                                        }
<DeclType>{BN}+                         { lineCount(yyscanner); }
<DeclType>.                             {
                                          unput(*yytext);
                                          BEGIN(FindMembers);
                                        }
<CSIndexer>"["[^\n\]]*"]"               {
                                          yyextra->current->name+=removeRedundantWhiteSpace(yytext);
                                          BEGIN(FindMembers);
                                        }
<FindMembers>[0-9]{ID}                  { // some number where we did not expect one
                                        }
<FindMembers>"."                        {
                                          if (yyextra->insideJava || yyextra->insideCS || yyextra->insideD)
                                          {
                                            yyextra->current->name+=".";
                                          }
                                        }
<FindMembers>"::"                       {
                                          yyextra->current->name+=yytext;
                                        }
<CppQuote>"("{B}*"\""                   {
                                          yyextra->insideCppQuote=TRUE;
                                          BEGIN(FindMembers);
                                        }
<IDLUnionCase>"::"
<IDLUnionCase>":"                       { BEGIN(FindMembers); }
<IDLUnionCase>\n                        { lineCount(yyscanner); }
<IDLUnionCase>.
<TryFunctionBlock>\n                    { lineCount(yyscanner); }
<TryFunctionBlock>"{"                   {
                                          yyextra->curlyCount=0;
                                          yyextra->lastCurlyContext = TryFunctionBlockEnd ;
                                          BEGIN( SkipCurly );
                                        }
<TryFunctionBlock>.
<TryFunctionBlockEnd>{BN}*"catch"       { lineCount(yyscanner); BEGIN(TryFunctionBlock); // {BN}* added to fix bug 611193
                                        }
<TryFunctionBlockEnd>\n                 { unput(*yytext); // rule added to fix bug id 601138
                                          BEGIN( FindMembers );
                                        }
<TryFunctionBlockEnd>.                  { unput(*yytext);
                                          BEGIN( FindMembers );
                                        }
<EndCppQuote>")"                        {
                                          yyextra->insideCppQuote=FALSE;
                                          BEGIN(FindMembers);
                                        }
<FindConceptParts>[^\/\n]*              {
                                          yyextra->current->initializer << yytext;
                                        }
<FindConceptParts>.                     {
                                          yyextra->current->initializer << yytext;
                                        }
<FindConceptParts>\n                    {
                                          yyextra->current->initializer << yytext;
                                          yyextra->yyLineNr++;
                                        }
<FindConceptParts>{CCS}                 {
                                          yyextra->current->initializer << yytext;
                                        }
<FindConceptParts>{CPPC}                {
                                          yyextra->current->initializer << yytext;
                                        }
<FindConceptParts>^[ \t]*{CCS}[!*]      { // start of a special comment inside Concept definition
                                          QCString text(yytext);
                                          int indent = text.find('/');
                                          //printf("## startLine=%d startColumn=%d\n",yyextra->current->startLine,indent);
                                          if (!yyextra->current->initializer.empty())
                                          {
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          yyextra->current->section = EntryType::makeConceptDocPart();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = indent;
                                          yyextra->current->initializer.clear();
                                          BEGIN(FindConceptDocs);
                                        }
<FindConceptParts>^[ \t]*{CPPC}[!/].*\n {
                                          if (!yyextra->current->initializer.empty())
                                          {
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          QCString line(yytext);
                                          int indent = line.find('/');
                                          yyextra->current->section = EntryType::makeConceptDocPart();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = indent;
                                          yyextra->current->initializer.clear();
                                          yyextra->current->initializer << line.mid(indent+3);
                                          yyextra->yyLineNr++;
                                          BEGIN(FindConceptCxxDocs);
                                        }
<FindConceptCxxDocs>^[ \t]*{CPPC}.*\n   {
                                          QCString line(yytext);
                                          int p = line.find('/');
                                          if (line.at(p+2)=='!' || line.at(p+2)=='/') p++;
                                          yyextra->current->initializer << line.mid(p+2);
                                          yyextra->yyLineNr++;
                                        }
<FindConceptCxxDocs>{CCS}|{CCE}|{CPPC}  {
                                          yyextra->current->initializer << yytext;
                                        }
<FindConceptCxxDocs>.                   {
                                          if (!yyextra->current->initializer.empty())
                                          {
                                            //printf("*** doc=[[%s]\n",qPrint(yyextra->current->initializer.str()));
                                            handleCommentBlock(yyscanner,yyextra->current->initializer.str(),false);
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          unput(*yytext);
                                          yyextra->current->section = EntryType::makeConceptCodePart();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          BEGIN(FindConceptParts);
                                        }
<FindConceptDocs>^[ \t]*"*"+/[^/]       { // skip leading indentation
                                        }
<FindConceptDocs>[^\*\n]+               {
                                          yyextra->current->initializer << yytext;
                                        }
<FindConceptDocs>\n                     {
                                          yyextra->current->initializer << yytext;
                                          yyextra->yyLineNr++;
                                        }
<FindConceptDocs>"*"*{CCE}[ t]*\n?      {
                                          if (!yyextra->current->initializer.empty())
                                          {
                                            //printf("*** doc=[[%s]\n",qPrint(yyextra->current->initializer.str()));
                                            handleCommentBlock(yyscanner,yyextra->current->initializer.str(),false);
                                            yyextra->current->section = EntryType::makeConceptDocPart();
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          lineCount(yyscanner);
                                          yyextra->current->section = EntryType::makeConceptCodePart();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          BEGIN(FindConceptParts);
                                        }
<FindConceptDocs>"*"*{CCE}              {
                                          if (!yyextra->current->initializer.empty())
                                          {
                                            //printf("*** doc=[[%s]\n",qPrint(yyextra->current->initializer.str()));
                                            handleCommentBlock(yyscanner,yyextra->current->initializer.str(),false);
                                            yyextra->current->section = EntryType::makeConceptDocPart();
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          yyextra->current->section = EntryType::makeConceptCodePart();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          BEGIN(FindConceptParts);
                                        }
<FindConceptParts><<EOF>>               {
                                          if (!yyextra->current->initializer.empty())
                                          {
                                            if (!yyextra->current->section.isConceptCodePart())
                                            {
                                              yyextra->current->section = EntryType::makeConceptDocPart();
                                            }
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          yyterminate();
                                        }
<FindMembers,FindFields>{B}*"#"         { if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCPPContext = YY_START;
                                          BEGIN( SkipCPP ) ;
                                        }
<FindMembers,FindFields>{B}*"#"{B}*"cmakedefine01"      |
<FindMembers,FindFields>{B}*"#"{B}*("cmake")?"define"   {
                                          if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->fileName   = yyextra->fileName;
                                          yyextra->current->startLine  = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->type.clear();
                                          yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                          yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->section = EntryType::makeDefine();
                                          yyextra->lastDefineContext = YY_START;
                                          BEGIN( SDefine );
                                        }
<FindMembers,ReadBody,ReadNSBody,ReadBodyIntf,SkipCurly,SkipCurlyCpp>{B}*"#"{B}+[0-9]+{B}+/"\"" { /* line control directive */
                                          yyextra->yyLineNr = atoi(&yytext[1]);
                                          //printf("setting line number to %d\n",yyextra->yyLineNr);
                                          yyextra->lastPreLineCtrlContext = YY_START;
                                          if (YY_START==ReadBody ||
                                              YY_START==ReadNSBody ||
                                              YY_START==ReadBodyIntf)
                                          {
                                            yyextra->current->program << yytext;
                                          }
                                          BEGIN( PreLineCtrl );
                                        }
<PreLineCtrl>"\""[^\n\"]*"\""           {
                                          yyextra->fileName = stripQuotes(yytext);
                                          if (yyextra->lastPreLineCtrlContext==ReadBody ||
                                              yyextra->lastPreLineCtrlContext==ReadNSBody ||
                                              yyextra->lastPreLineCtrlContext==ReadBodyIntf)
                                          {
                                            yyextra->current->program << yytext;
                                          }
                                        }
<PreLineCtrl>.                          {
                                          if (yyextra->lastPreLineCtrlContext==ReadBody ||
                                              yyextra->lastPreLineCtrlContext==ReadNSBody ||
                                              yyextra->lastPreLineCtrlContext==ReadBodyIntf)
                                          {
                                            yyextra->current->program << yytext;
                                          }
                                        }
<PreLineCtrl>\n                         {
                                          if (yyextra->lastPreLineCtrlContext==ReadBody ||
                                              yyextra->lastPreLineCtrlContext==ReadNSBody ||
                                              yyextra->lastPreLineCtrlContext==ReadBodyIntf)
                                          {
                                            yyextra->current->program << yytext;
                                          }
                                          lineCount(yyscanner);
                                          BEGIN( yyextra->lastPreLineCtrlContext );
                                        }
<SkipCPP>.
<SkipCPP>\\[\r]*"\n"[\r]*               { lineCount(yyscanner); }
<SkipCPP>[\r]*\n[\r]*                   { lineCount(yyscanner);
                                          BEGIN( yyextra->lastCPPContext) ;
                                        }
<SDefine>{ID}{B}*"("                     {
                                          yyextra->current->name = yytext;
                                          yyextra->current->name = yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                          yyextra->current->args = "(";
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->currentArgumentContext = DefineEnd;
                                          yyextra->fullArgString=yyextra->current->args;
                                          yyextra->copyArgString=&yyextra->current->args;
                                          BEGIN( ReadFuncArgType ) ;
                                        }
 /*
<DefineArg>")"                          {
                                          //printf("Define with args\n");
                                          yyextra->current->args += ')';
                                          BEGIN( DefineEnd );
                                        }
<DefineArg>.                            {
                                          yyextra->current->args += *yytext;
                                        }
  */
<SDefine>{ID}                            {
                                          //printf("Define '%s' without args\n",yytext);
                                          storeClangId(yyscanner,yytext);
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = yytext;
                                          BEGIN(DefineEnd);
                                        }
<DefineEnd><<EOF>>                      |
<DefineEnd>\n                           {
                                          //printf("End define: doc=%s docFile=%s docLine=%d\n",qPrint(yyextra->current->doc),qPrint(yyextra->current->docFile),yyextra->current->docLine);
                                          lineCount(yyscanner);
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(yyextra->lastDefineContext);
                                        }
<DefinePHPEnd>";"                       {
                                          //printf("End define\n");
                                          yyextra->current->fileName   = yyextra->fileName;
                                          yyextra->current->startLine  = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->type.clear();
                                          yyextra->current->type       = "const";
                                          QCString init = yyextra->current->initializer.str();
                                          init = init.simplifyWhiteSpace();
                                          init = init.left(init.length()-1);
                                          yyextra->current->initializer.str(init.str());
                                          yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->section = EntryType::makeVariable();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }
<DefinePHPEnd>.
<DefineEnd>\\[\r]?\n                    {
                                          lineCount(yyscanner);
                                          yyextra->current->endBodyLine = yyextra->yyLineNr;
                                        }
<DefineEnd>\"                           {
                                          if (yyextra->insideIDL && yyextra->insideCppQuote)
                                          {
                                            BEGIN(EndCppQuote);
                                          }
                                          else
                                          {
                                            yyextra->lastStringContext=DefineEnd;
                                            BEGIN(SkipString);
                                          }
                                        }
<DefineEnd>.
<DefinePHP>{ID}["']{BN}*","{BN}*        {
                                          yyextra->current->name = yytext;
                                          yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->name = yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                          yyextra->current->name = yyextra->current->name.left(yyextra->current->name.length()-1);
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->lastRoundContext = DefinePHPEnd;
                                          yyextra->pCopyRoundGString = &yyextra->current->initializer;
                                          yyextra->roundCount = 0;
                                          BEGIN( GCopyRound );
                                        }

<FindMembers>[\^%]                      {  // ^ and % are C++/CLI extensions
                                          if (yyextra->insideCli)
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = yytext ;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>[*&]+                      {
                                          yyextra->current->name += yytext ;
                                          addType(yyscanner);
                                        }
<FindMembers,MemberSpec,SFunction,NextSemi,EnumBaseType,BitFields,ReadInitializer,ReadInitializerPtr,OldStyleArgs,DefinePHPEnd>";"{BN}*{DCOMM}"<" {
                                          if (yyextra->current->bodyLine==-1)
                                          {
                                            yyextra->current->bodyLine=yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = FALSE;
                                          yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                              ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());
                                          //printf("indent=%d\n",computeIndent(yytext+1,yyextra->column));
                                          lineCount(yyscanner);

                                          yyextra->docBlockTerm = ';';
                                          if (YY_START==EnumBaseType && yyextra->current->section.isEnum())
                                          {
                                            yyextra->current->bitfields = ":"+yyextra->current->args;
                                            yyextra->current->args.clear();
                                            yyextra->current->section = EntryType::makeVariable();
                                          }
                                          if (yytext[yyleng-3]=='/')
                                          {
                                            startCommentBlock(yyscanner,TRUE);
                                            BEGIN( DocLine );
                                          }
                                          else
                                          {
                                            startCommentBlock(yyscanner,FALSE);
                                            BEGIN( DocBlock );
                                          }
                                        }
<MemberSpec,FindFields,FindMembers,NextSemi,EnumBaseType,BitFields,ReadInitializer,ReadInitializerPtr,OldStyleArgs>","{BN}*{DCOMM}"<" {
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = FALSE;
                                          yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                              ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());
                                          lineCount(yyscanner);

                                          yyextra->docBlockTerm = ',';
                                          if (YY_START==EnumBaseType && yyextra->current->section.isEnum())
                                          {
                                            yyextra->current->bitfields = ":"+yyextra->current->args;
                                            yyextra->current->args.clear();
                                            yyextra->current->section = EntryType::makeVariable();
                                          }
                                          if (yytext[yyleng-3]=='/')
                                          {
                                            startCommentBlock(yyscanner,TRUE);
                                            BEGIN( DocLine );
                                          }
                                          else
                                          {
                                            startCommentBlock(yyscanner,FALSE);
                                            BEGIN( DocBlock );
                                          }
                                        }
<DefineEnd,FindFields,ReadInitializer,ReadInitializerPtr,OldStyleArgs>{BN}*{DCOMM}"<" {
                                          if (yyextra->current->bodyLine==-1)
                                          {
                                            yyextra->current->bodyLine=yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = FALSE;
                                          yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                              ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());
                                          lineCount(yyscanner);

                                          yyextra->docBlockTerm = 0;
                                          if (yytext[yyleng-3]=='/')
                                          {
                                            startCommentBlock(yyscanner,TRUE);
                                            BEGIN( DocLine );
                                          }
                                          else
                                          {
                                            startCommentBlock(yyscanner,FALSE);
                                            BEGIN( DocBlock );
                                          }
                                        }

<FindMembers,FindFields>({CPPC}([!/]){B}*{CMD}"{")|({CCS}([!*]){B}*{CMD}"{")       {
                                          //handleGroupStartCommand(yyextra->current->name);
                                          if (yyextra->previous && yyextra->previous->section.isGroupDoc())
                                          {
                                            // link open command to the group defined in the yyextra->previous entry
                                            yyextra->commentScanner.open(yyextra->previous.get(),yyextra->fileName,yyextra->yyLineNr);
                                          }
                                          else
                                          {
                                            // link open command to the yyextra->current entry
                                            yyextra->commentScanner.open(yyextra->current.get(),yyextra->fileName,yyextra->yyLineNr);
                                          }
                                          //yyextra->current = tmp;
                                          initEntry(yyscanner);
                                          if (yytext[1]=='/')
                                          {
                                            if (yytext[2]=='!' || yytext[2]=='/')
                                            {
                                              yyextra->docBlockContext   = YY_START;
                                              yyextra->docBlockInBody    = FALSE;
                                              yyextra->docBlockAutoBrief = FALSE;
                                              yyextra->docBlock.str(std::string());
                                              yyextra->docBlockTerm = 0;
                                              startCommentBlock(yyscanner,TRUE);
                                              BEGIN(DocLine);
                                            }
                                            else
                                            {
                                              yyextra->lastCContext=YY_START;
                                              BEGIN(SkipCxxComment);
                                            }
                                          }
                                          else
                                          {
                                            if (yytext[2]=='!' || yytext[2]=='*')
                                            {
                                              yyextra->docBlockContext   = YY_START;
                                              yyextra->docBlockInBody    = FALSE;
                                              yyextra->docBlock.str(std::string());
                                              yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                                  ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                              yyextra->docBlockTerm = 0;
                                              startCommentBlock(yyscanner,FALSE);
                                              BEGIN(DocBlock);
                                            }
                                            else
                                            {
                                              yyextra->lastCContext=YY_START;
                                              BEGIN(SkipComment);
                                            }
                                          }
                                        }
<FindMembers,FindFields,ReadInitializer,ReadInitializerPtr>{CPPC}([!/]){B}*{CMD}"}".*|{CCS}([!*]){B}*{CMD}"}"[^*]*{CCE}     {
                                          bool insideEnum = YY_START==FindFields || ((YY_START==ReadInitializer || YY_START==ReadInitializerPtr) && yyextra->lastInitializerContext==FindFields); // see bug746226
                                          yyextra->commentScanner.close(yyextra->current.get(),yyextra->fileName,yyextra->yyLineNr,insideEnum);
                                          lineCount(yyscanner);
                                        }
<FindMembers>"=>"                       {
                                          if (!yyextra->insideCS) REJECT;
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->initializer.str(yytext);
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                          yyextra->current->spec.setGettable(true);
                                          BEGIN(ReadInitializerPtr);
                                        }
<FindMembers>"="                        { // in PHP code this could also be due to "<?="
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->initializer.str(" ");
                                          for (int ii = 2 ; ii <  yyextra->yyColNr; ii++)
                                            yyextra->current->initializer << " ";
                                          yyextra->current->initializer << "=";
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          BEGIN(ReadInitializer);
                                        }
<UNOIDLAttributeBlock>{BN}*[gs]"et"{BN}+"raises"{BN}*"("{BN}*{SCOPENAME}{BN}*(","{BN}*{SCOPENAME}{BN}*)*")"{BN}*";"     {
                                          lineCount(yyscanner);
                                          yyextra->current->exception += " ";
                                          yyextra->current->exception += removeRedundantWhiteSpace(yytext);
                                        }
<UNOIDLAttributeBlock>"}"               {
                                          yyextra->current->exception += " }";
                                          BEGIN(FindMembers);
                                        }
  /* Read initializer rules */
<ReadInitializer,ReadInitializerPtr>"(" {
                                          yyextra->lastRoundContext=YY_START;
                                          yyextra->pCopyRoundGString=&yyextra->current->initializer;
                                          yyextra->roundCount=0;
                                          yyextra->current->initializer << *yytext;
                                          BEGIN(GCopyRound);
                                        }
<ReadInitializer,ReadInitializerPtr>"[" {
                                          if (!yyextra->insidePHP) REJECT;
                                          yyextra->lastSquareContext=YY_START;
                                          yyextra->pCopySquareGString=&yyextra->current->initializer;
                                          yyextra->squareCount=0;
                                          yyextra->current->initializer << *yytext;
                                          BEGIN(GCopySquare);
                                        }
<ReadInitializer,ReadInitializerPtr>"{" {
                                          yyextra->lastCurlyContext=YY_START;
                                          yyextra->pCopyCurlyGString=&yyextra->current->initializer;
                                          yyextra->curlyCount=0;
                                          yyextra->current->initializer << *yytext;
                                          BEGIN(GCopyCurly);
                                        }
<ReadInitializer,ReadInitializerPtr>[;,] {
                                          //printf(">> initializer '%s' <<\n",qPrint(yyextra->current->initializer));
                                          if (*yytext==';' && yyextra->current_root->spec.isEnum())
                                          {
                                            yyextra->current->fileName   = yyextra->fileName;
                                            yyextra->current->startLine  = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                            yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                            yyextra->current->section = EntryType::makeVariable();
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                            BEGIN(FindMembers);
                                          }
                                          else if (*yytext==';' || (yyextra->lastInitializerContext==FindFields && yyextra->initBracketCount==0)) // yyextra->initBracketCount==0 was added for bug 665778
                                          {
                                            unput(*yytext);
                                            if (YY_START == ReadInitializerPtr) yyextra->current->initializer.str(std::string());
                                            BEGIN(yyextra->lastInitializerContext);
                                          }
                                          else if (*yytext==',' && yyextra->initBracketCount==0) // for "int a=0,b=0"
                                          {
                                            unput(*yytext);
                                            if (YY_START == ReadInitializerPtr) yyextra->current->initializer.str(std::string());
                                            BEGIN(yyextra->lastInitializerContext);
                                          }
                                          else
                                          {
                                            yyextra->current->initializer << *yytext;
                                          }
                                        }
<ReadInitializer,ReadInitializerPtr>{RAWBEGIN} { // C++11 raw string
                                          if (!yyextra->insideCpp)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->current->initializer << yytext;
                                            yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
                                            yyextra->lastRawStringContext = YY_START;
                                            yyextra->pCopyRawGString = &yyextra->current->initializer;
                                            BEGIN(RawGString);
                                            //printf("RawGString delimiter='%s'\n",qPrint(delimiter));
                                          }
                                        }
<ReadInitializer>{CPPC}|{CCS}           {
                                          if (yyextra->keepComment)
                                          {
                                            yyextra->current->initializer << yytext;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<RawGString>{RAWEND}                    {
                                          if (extractEndRawStringDelimiter(yytext)==yyextra->delimiter)
                                          {
                                            *yyextra->pCopyRawGString << yytext;
                                            BEGIN(yyextra->lastRawStringContext);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<RawGString>[^)\n]+                     {
                                          *yyextra->pCopyRawGString << yytext;
                                        }
<RawGString>.                           {
                                          *yyextra->pCopyRawGString << yytext;
                                        }
<RawGString>\n                          {
                                          *yyextra->pCopyRawGString << yytext;
                                          lineCount(yyscanner);
                                        }
<RawString>{RAWEND}                     {
                                          *yyextra->pCopyRawString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                          if (extractEndRawStringDelimiter(yytext)==yyextra->delimiter)
                                          {
                                            BEGIN(yyextra->lastRawStringContext);
                                          }
                                        }
<RawString>[^)]+                        {
                                          *yyextra->pCopyRawString += yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<RawString>.                            {
                                          *yyextra->pCopyRawString += yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<RawString>\n                           {
                                          *yyextra->pCopyRawString += yytext;
                                          yyextra->fullArgString+=yytext;
                                          lineCount(yyscanner);
                                        }
<ReadInitializer,ReadInitializerPtr>\"  {
                                          if (yyextra->insideIDL && yyextra->insideCppQuote)
                                          {
                                            BEGIN(EndCppQuote);
                                          }
                                          else
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedGString=&yyextra->current->initializer;
                                            yyextra->stopAtInvalidString=false;
                                            BEGIN(CopyGString);
                                          }
                                        }
<ReadInitializer,ReadInitializerPtr>"->"        {
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>("<<"|"<=") {
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>(">>") {
                                          if (yyextra->initBracketCount<=yyextra->sharpCount && yyextra->sharpCount>=2)
                                          {
                                            // heuristic to detect '>>' in A<B<C>>::D as '> >', but not e.g. 'A>>B' or A<B<(C>>2)>::D>
                                            yyextra->initBracketCount-=2;
                                            yyextra->sharpCount-=2;
                                          }
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>(">=") {
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>[<\[{(]             {
                                          yyextra->initBracketCount++;
                                          yyextra->sharpCount++;
                                          yyextra->current->initializer << *yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>[>\]})]             {
                                          yyextra->initBracketCount--;
                                          yyextra->sharpCount--;
                                          if (*yytext=='}')
                                          {
                                            yyextra->current->endBodyLine=yyextra->yyLineNr;
                                          }
                                          yyextra->current->initializer << *yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>\'                  {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedGString = &yyextra->current->initializer;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPGString);
                                          }
                                          else
                                          {
                                            yyextra->current->initializer << yytext;
                                          }
                                        }
<ReadInitializer,ReadInitializerPtr>{CHARLIT}              {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->current->initializer << yytext;
                                          }
                                        }
<ReadInitializer,ReadInitializerPtr>\n                  {
                                          yyextra->current->initializer << *yytext;
                                          lineCount(yyscanner);
                                        }
<ReadInitializer,ReadInitializerPtr>"@\""                       {
                                          //printf("yyextra->insideCS=%d\n",yyextra->insideCS);
                                          yyextra->current->initializer << yytext;
                                          if (!yyextra->insideCS && !yyextra->insideObjC)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            // C#/ObjC verbatim string
                                            yyextra->lastSkipVerbStringContext=YY_START;
                                            yyextra->pSkipVerbString=&yyextra->current->initializer;
                                            BEGIN(SkipVerbString);
                                          }
                                        }
<SkipVerbString>[^\n"\\]+               {
                                          *yyextra->pSkipVerbString << yytext;
                                        }
<SkipVerbString>"\\\\"                  { // escaped backslash
                                          if (yyextra->insideCS) REJECT
                                          *yyextra->pSkipVerbString << yytext;
                                        }
<SkipVerbString>"\\\""                  { // backslash escaped quote
                                          if (yyextra->insideCS) REJECT
                                          *yyextra->pSkipVerbString << yytext;
                                        }
<SkipVerbString>"\"\""                  { // quote escape
                                          *yyextra->pSkipVerbString << yytext;
                                        }
<SkipVerbString>"\""                    {
                                          *yyextra->pSkipVerbString << *yytext;
                                          BEGIN(yyextra->lastSkipVerbStringContext);
                                        }
<SkipVerbString>\n                      {
                                          *yyextra->pSkipVerbString << *yytext;
                                          lineCount(yyscanner);
                                        }
<SkipVerbString>.                       {
                                          *yyextra->pSkipVerbString << *yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>"?>"                        {
                                          if (yyextra->insidePHP)
                                            BEGIN( FindMembersPHP );
                                          else
                                            yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>.                   {
                                          yyextra->current->initializer << *yytext;
                                        }

  /* generic quoted string copy rules */
<CopyString,CopyPHPString>\\.           {
                                          *yyextra->pCopyQuotedString+=yytext;
                                        }
<CopyString>\"                          {
                                          *yyextra->pCopyQuotedString+=*yytext;
                                          BEGIN( yyextra->lastStringContext );
                                        }
<CopyPHPString>\'                       {
                                          *yyextra->pCopyQuotedString+=*yytext;
                                          BEGIN( yyextra->lastStringContext );
                                        }
<CopyString,CopyPHPString>{CCS}|{CCE}|{CPPC} {
                                          *yyextra->pCopyQuotedString+=yytext;
                                        }
<CopyString,CopyPHPString>\n            {
                                          *yyextra->pCopyQuotedString+=*yytext;
                                          lineCount(yyscanner);
                                        }
<CopyString,CopyPHPString>.             {
                                          *yyextra->pCopyQuotedString+=*yytext;
                                        }

  /* generic quoted growable string copy rules */
<CopyGString,CopyPHPGString>\\.         {
                                          *yyextra->pCopyQuotedGString << yytext;
                                        }
<CopyGString>\"                         {
                                          *yyextra->pCopyQuotedGString << *yytext;
                                          BEGIN( yyextra->lastStringContext );
                                        }
<CopyPHPGString>\'                      {
                                          *yyextra->pCopyQuotedGString << *yytext;
                                          BEGIN( yyextra->lastStringContext );
                                        }
<CopyGString,CopyPHPGString>"<?php"     { // we had an odd number of quotes.
                                          *yyextra->pCopyQuotedGString <<  yytext;
                                          BEGIN( yyextra->lastStringContext );
                                        }
<CopyGString,CopyPHPGString>{CCS}|{CCE}|{CPPC} {
                                          *yyextra->pCopyQuotedGString << yytext;
                                        }
<CopyGString,CopyPHPGString>\n          {
                                          *yyextra->pCopyQuotedGString << *yytext;
                                          if (yyextra->stopAtInvalidString)
                                          {
                                            BEGIN( yyextra->lastStringContext );
                                          }
                                          else
                                          {
                                            lineCount(yyscanner);
                                          }
                                        }
<CopyGString,CopyPHPGString>.           {
                                          *yyextra->pCopyQuotedGString << *yytext;
                                        }

  /* generic round bracket list copy rules */
<CopyRound>\"                           {
                                          *yyextra->pCopyRoundString += *yytext;
                                          yyextra->pCopyQuotedString=yyextra->pCopyRoundString;
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN(CopyString);
                                        }
<CopyRound>"("                          {
                                          *yyextra->pCopyRoundString += *yytext;
                                          yyextra->roundCount++;
                                        }
<CopyRound>")"                          {
                                          *yyextra->pCopyRoundString += *yytext;
                                          if (--yyextra->roundCount<0)
                                            BEGIN(yyextra->lastRoundContext);
                                        }
<CopyRound>\n                           {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyRoundString += *yytext;
                                        }
<CopyRound>\'                           {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedString = yyextra->pCopyRoundString;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPString);
                                          }
                                          else
                                          {
                                            *yyextra->pCopyRoundString += yytext;
                                          }
                                        }
<CopyRound>{CHARLIT}                    {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->pCopyRoundString+=yytext;
                                          }
                                        }
<CopyRound>[^"'()\n,]+                  {
                                          *yyextra->pCopyRoundString+=yytext;
                                        }
<CopyRound>.                            {
                                          *yyextra->pCopyRoundString+=*yytext;
                                        }

  /* generic sharp bracket list copy rules */
<CopySharp>\"                           {
                                          *yyextra->pCopySharpString += *yytext;
                                          yyextra->pCopyQuotedString=yyextra->pCopySharpString;
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN(CopyString);
                                        }
<CopySharp>"<"                          {
                                          *yyextra->pCopySharpString += *yytext;
                                          yyextra->sharpCount++;
                                        }
<CopySharp>">"                          {
                                          *yyextra->pCopySharpString += *yytext;
                                          if (--yyextra->sharpCount<0)
                                          {
                                            BEGIN(yyextra->lastSharpContext);
                                          }
                                        }
<CopySharp>\n                           {
                                          lineCount(yyscanner);
                                          *yyextra->pCopySharpString += *yytext;
                                        }
<CopySharp>\'                           {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedString = yyextra->pCopySharpString;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPString);
                                          }
                                          else
                                          {
                                            *yyextra->pCopySharpString += yytext;
                                          }
                                        }
<CopySharp>{CHARLIT}                    {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->pCopySharpString+=yytext;
                                          }
                                        }
<CopySharp>[^"'<>\n,]+                  {
                                          *yyextra->pCopySharpString+=yytext;
                                        }
<CopySharp>.                            {
                                          *yyextra->pCopySharpString+=*yytext;
                                        }


  /* generic round bracket list copy rules for growable strings */
<GCopyRound>\"                          {
                                          *yyextra->pCopyRoundGString << *yytext;
                                          yyextra->pCopyQuotedGString=yyextra->pCopyRoundGString;
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN(CopyGString);
                                        }
<GCopyRound>"("                         {
                                          *yyextra->pCopyRoundGString << *yytext;
                                          yyextra->roundCount++;
                                        }
<GCopyRound>")"                         {
                                          *yyextra->pCopyRoundGString << *yytext;
                                          if (--yyextra->roundCount<0)
                                            BEGIN(yyextra->lastRoundContext);
                                        }
<GCopyRound>\n                          {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyRoundGString << *yytext;
                                        }
<GCopyRound>\'                          {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedGString = yyextra->pCopyRoundGString;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPGString);
                                          }
                                          else
                                          {
                                            *yyextra->pCopyRoundGString << yytext;
                                          }
                                        }
<GCopyRound>{CHARLIT}                   {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->pCopyRoundGString << yytext;
                                          }
                                        }
<GCopyRound>"@\""                       {
                                          if (!yyextra->insideCS) REJECT;
                                          *yyextra->pCopyRoundGString << yytext;
                                           yyextra->lastSkipVerbStringContext=YY_START;
                                           yyextra->pSkipVerbString=yyextra->pCopyRoundGString;
                                           BEGIN(SkipVerbString);
                                        }
<GCopyRound>[^"'()\n\/,R]+              { // R because of raw string start
                                          *yyextra->pCopyRoundGString << yytext;
                                        }
<GCopyRound>{RAWBEGIN}                  {
                                          *yyextra->pCopyRoundGString << yytext;
                                          yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
                                          yyextra->lastRawStringContext = YY_START;
                                          yyextra->pCopyRawGString = yyextra->pCopyRoundGString;
                                          BEGIN(RawGString);
                                        }
<GCopyRound>.                           {
                                          *yyextra->pCopyRoundGString << *yytext;
                                        }

  /* generic square bracket list copy rules for growable strings, we should only enter here in case of php, left the test part as in GCopyRound to keep it compatible with the round bracket version */
<GCopySquare>\"                         {
                                          *yyextra->pCopySquareGString << *yytext;
                                            yyextra->pCopyQuotedGString=yyextra->pCopySquareGString;
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN(CopyGString);
                                        }
<GCopySquare>\'                         {
                                          *yyextra->pCopySquareGString << *yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->pCopyQuotedGString=yyextra->pCopySquareGString;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPGString);
                                          }
                                        }
<GCopySquare>"["                        {
                                          *yyextra->pCopySquareGString << *yytext;
                                          yyextra->squareCount++;
                                        }
<GCopySquare>"]"                        {
                                          *yyextra->pCopySquareGString << *yytext;
                                          if (--yyextra->squareCount<0)
                                          BEGIN(yyextra->lastSquareContext);
                                        }
<GCopySquare>\n                         {
                                          lineCount(yyscanner);
                                          *yyextra->pCopySquareGString << *yytext;
                                        }
<GCopySquare>\'                         {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedGString = yyextra->pCopySquareGString;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPGString);
                                          }
                                          else
                                          {
                                            *yyextra->pCopySquareGString << yytext;
                                          }
                                        }
<GCopySquare>{CHARLIT}                  {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->pCopySquareGString << yytext;
                                          }
                                        }
<GCopySquare>[^"'\[\]\n\/,]+            {
                                          *yyextra->pCopySquareGString << yytext;
                                        }
<GCopySquare>.                          {
                                          *yyextra->pCopySquareGString << *yytext;
                                        }

  /* generic curly bracket list copy rules */
<CopyCurly>\"                           {
                                          *yyextra->pCopyCurlyString += *yytext;
                                          yyextra->pCopyQuotedString=yyextra->pCopyCurlyString;
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->keepComment=false;
                                          BEGIN(CopyString);
                                        }
<CopyCurly>\'                           {
                                          *yyextra->pCopyCurlyString += *yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->pCopyQuotedString=yyextra->pCopyCurlyString;
                                            yyextra->lastStringContext=YY_START;
                                            yyextra->keepComment=false;
                                            BEGIN(CopyPHPString);
                                          }
                                        }
<CopyCurly>"{"                          {
                                          *yyextra->pCopyCurlyString += *yytext;
                                          yyextra->curlyCount++;
                                        }
<CopyCurly>"}"                          {
                                          *yyextra->pCopyCurlyString += *yytext;
                                          if (--yyextra->curlyCount<0)
                                          {
                                            yyextra->keepComment=false;
                                            BEGIN(yyextra->lastCurlyContext);
                                          }
                                        }
<CopyCurly>{CHARLIT}                    { if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->pCopyCurlyString += yytext;
                                          }
                                        }
<CopyCurly>[^"'{}\/\n,]+                {
                                          *yyextra->pCopyCurlyString += yytext;
                                        }
<CopyCurly>"/"                          { *yyextra->pCopyCurlyString += yytext; }
<CopyCurly>\n                           {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyCurlyString += *yytext;
                                        }
<CopyCurly>.                            {
                                          *yyextra->pCopyCurlyString += *yytext;
                                        }
<CopyCurly>{CPPC}[^\n]*                 {
                                          if (yyextra->keepComment)
                                          {
                                            *yyextra->pCopyCurlyString += yytext;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<CopyCurly>{CCS}                        {
                                          if (yyextra->keepComment)
                                          {
                                            *yyextra->pCopyCurlyString += yytext;
                                            BEGIN(CopyComment);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<CopyComment>{CCE}                      {
                                          *yyextra->pCopyCurlyString += yytext;
                                          BEGIN(CopyCurly);
                                        }
<CopyComment>\n                         {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyCurlyString += *yytext;
                                        }
<CopyComment>.                          {
                                          *yyextra->pCopyCurlyString += *yytext;
                                        }
<CopyComment><<EOF>>                    {
                                          warn(yyextra->fileName,yyextra->yyLineNr,
                                              "reached end of file while inside a C style comment block!");
                                          yyterminate();
                                        }

  /* generic curly bracket list copy rules for growable strings */
<GCopyCurly>^"#"{B}+[0-9]+{B}+"\""[^\"\n]+"\""{B}+"1"{B}*\n? { // start of included file marker
                                        }
<GCopyCurly>^"#"{B}+[0-9]+{B}+"\""[^\"\n]+"\""{B}+"2"{B}*\n? { // end of included file marker
                                          QCString line = yytext;
                                          int s = line.find(' ');
                                          int e = line.find('"',s);
                                          yyextra->yyLineNr = line.mid(s,e-s).toInt();
                                          if (yytext[yyleng-1]=='\n')
                                          {
                                            lineCount(yyscanner);
                                            yyextra->column=0;
                                          }
                                        }
<GCopyCurly>\"                          {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                          yyextra->pCopyQuotedGString=yyextra->pCopyCurlyGString;
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->keepComment = false;
                                          BEGIN(CopyGString);
                                        }
<GCopyCurly>\'                          {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->pCopyQuotedGString=yyextra->pCopyCurlyGString;
                                            yyextra->lastStringContext=YY_START;
                                            yyextra->keepComment = false;
                                            BEGIN(CopyPHPGString);
                                          }
                                        }
<GCopyCurly>"{"                         {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                          yyextra->curlyCount++;
                                        }
<GCopyCurly>"}"                         {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                          if (--yyextra->curlyCount<0)
                                          {
                                            yyextra->current->endBodyLine = yyextra->yyLineNr;
                                            yyextra->keepComment = false;
                                            BEGIN(yyextra->lastCurlyContext);
                                          }
                                        }
<GCopyCurly>{CHARLIT}                    { if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->pCopyCurlyGString << yytext;
                                          }
                                        }
<GCopyCurly>[^"'{}\/\n,]+               {
                                          *yyextra->pCopyCurlyGString << yytext;
                                        }
<GCopyCurly>[,]+                        {
                                          *yyextra->pCopyCurlyGString << yytext;
                                        }
<GCopyCurly>"/"                         { *yyextra->pCopyCurlyGString << yytext; }
<GCopyCurly>\n                          {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyCurlyGString << *yytext;
                                        }
<GCopyCurly>.                           {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                        }
<GCopyCurly>{CPPC}[^\n]*                {
                                          if (yyextra->keepComment)
                                          {
                                            *yyextra->pCopyCurlyGString << yytext;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<GCopyCurly>{CCS}                       {
                                          if (yyextra->keepComment)
                                          {
                                            *yyextra->pCopyCurlyGString << yytext;
                                            BEGIN(GCopyComment);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<GCopyComment>{CCE}                     {
                                          *yyextra->pCopyCurlyGString << yytext;
                                          BEGIN(GCopyCurly);
                                        }
<GCopyComment>\n                        {
                                          lineCount(yyscanner);
                                          *yyextra->pCopyCurlyGString << *yytext;
                                        }
<GCopyComment>.                         {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                        }
<GCopyComment><<EOF>>                   {
                                          warn(yyextra->fileName,yyextra->yyLineNr,
                                              "reached end of file while inside a C style comment block!");
                                          yyterminate();
                                        }

  /* ---------------------- */


<FindMembers>":"                        {
                                          if (yyextra->current->type.isEmpty() &&
                                              yyextra->current->name=="enum") // see bug 69041, C++11 style anon enum: 'enum : unsigned int {...}'
                                          {
                                            yyextra->current->section = EntryType::makeEnum();
                                            yyextra->current->name.clear();
                                            yyextra->current->args.clear();
                                            BEGIN(EnumBaseType);
                                          }
                                          else
                                          {
                                            if (yyextra->current->type.isEmpty()) // anonymous padding field, e.g. "int :7;"
                                            {
                                              addType(yyscanner);
                                              yyextra->current->name.sprintf("__pad%d__",yyextra->padCount++);
                                            }
                                            BEGIN(BitFields);
                                            yyextra->current->bitfields+=":";
                                          }
                                        }
<BitFields>.                            {
                                          yyextra->current->bitfields+=*yytext;
                                        }
<EnumBaseType>.                         {
                                          yyextra->current->args+=*yytext;
                                        }
<EnumBaseType>\n                        {
                                          lineCount(yyscanner);
                                          yyextra->current->args+=' ';
                                        }
<FindMembers>[;,]                       {
                                          QCString oldType = yyextra->current->type;
                                          if (yyextra->current->bodyLine==-1)
                                          {
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                          }
                                          if ( yyextra->insidePHP && yyextra->current->type.startsWith("var"))
                                          {
                                            yyextra->current->type = yyextra->current->type.mid(3);
                                          }
                                          if (yyextra->isTypedef && !yyextra->current->type.startsWith("typedef "))
                                          {
                                            yyextra->current->type.prepend("typedef ");
                                          }
                                          bool isStatic = yyextra->current->isStatic;
                                          Protection prot = yyextra->current->protection;
                                          bool isConcept = yyextra->current->section.isConcept();
                                          bool isModule  = yyextra->current->section.isModuleDoc();
                                          if (isConcept) // C++20 concept
                                          {
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          else if (isModule) // C++20 module
                                          {
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          else if (!yyextra->current->name.isEmpty() && !yyextra->current->section.isEnum())
                                          {
                                            yyextra->current->type=yyextra->current->type.simplifyWhiteSpace();
                                            yyextra->current->args=removeRedundantWhiteSpace(yyextra->current->args);
                                            yyextra->current->name=yyextra->current->name.stripWhiteSpace();
                                            if (yyextra->current->section.isClass()) // remove spec for "struct Bla bla;"
                                            {
                                                yyextra->current->spec = TypeSpecifier();
                                            }
                                            yyextra->current->section = EntryType::makeVariable() ;
                                            yyextra->current->fileName = yyextra->fileName;
                                            yyextra->current->startLine = yyextra->yyBegLineNr;
                                            yyextra->current->startColumn = yyextra->yyBegColNr;
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          if ( *yytext == ',')
                                          {
                                            yyextra->current->isStatic = isStatic; // the static attribute holds for all variables
                                            yyextra->current->protection = prot;
                                            yyextra->current->name.clear();
                                            yyextra->current->args.clear();
                                            yyextra->current->brief.clear();
                                            yyextra->current->doc.clear();
                                            yyextra->current->initializer.str(std::string());
                                            yyextra->current->bitfields.clear();
                                            yyextra->current->type = stripFuncPtr(oldType);
                                          }
                                          else
                                          {
                                            yyextra->mtype = MethodTypes::Method;
                                            yyextra->virt = Specifier::Normal;
                                            yyextra->current->bodyLine = -1;
                                            yyextra->current->bodyColumn = 1;
                                            yyextra->current->groups.clear();
                                            initEntry(yyscanner);
                                          }
                                        }

<FindMembers>"["                        {
                                          if (yyextra->insideSlice)
                                          {
                                            yyextra->squareCount=1;
                                            yyextra->lastSquareContext = YY_START;
                                            yyextra->current->metaData += "[";
                                            BEGIN( SliceMetadata );
                                          }
                                          else if (!yyextra->insideCS &&
                                              (yyextra->current->name.isEmpty() ||
                                               yyextra->current->name=="typedef"
                                              )
                                             ) // IDL function property
                                          {
                                            yyextra->squareCount=1;
                                            yyextra->lastSquareContext = YY_START;
                                            yyextra->idlAttr.clear();
                                            yyextra->idlProp.clear();
                                            yyextra->current->mtype = yyextra->mtype;

                                            if (Config_getBool(IDL_PROPERTY_SUPPORT) &&
                                                 yyextra->current->mtype == MethodTypes::Property)
                                            { // we are yyextra->inside the properties section of a dispinterface
                                              yyextra->odlProp = true;
                                              yyextra->current->spec.setGettable(true).setSettable(true);
                                            }

                                            BEGIN( IDLAttribute );
                                          }
                                          else if (yyextra->insideCS &&
                                                  yyextra->current->name.isEmpty())
                                          {
                                            yyextra->squareCount=1;
                                            yyextra->lastSquareContext = YY_START;
                                            // Skip the C# attribute
                                            // for this member
                                            yyextra->current->args.clear();
                                            BEGIN( SkipSquare );
                                          }
                                          else
                                          {
                                            yyextra->current->args += yytext ;
                                            yyextra->squareCount=1;
                                            yyextra->externLinkage=FALSE; // see bug759247
                                            BEGIN( Array ) ;
                                          }
                                        }
<SliceMetadata>"["                      { // Global metadata.
                                          yyextra->squareCount++;
                                          yyextra->current->metaData += "[";
                                        }
<SliceMetadata>{BN}*                    {
                                          lineCount(yyscanner);
                                        }
<SliceMetadata>\"[^\"]*\"               {
                                          yyextra->current->metaData += yytext;
                                        }
<SliceMetadata>","                      {
                                          yyextra->current->metaData += yytext;
                                        }
<SliceMetadata>"]"                      {
                                          yyextra->current->metaData += yytext;
                                          if (--yyextra->squareCount<=0)
                                          {
                                            BEGIN (yyextra->lastSquareContext);
                                          }
                                        }
<SliceOptional>"("                      {
                                          yyextra->current->type += "(";
                                          yyextra->roundCount++;
                                        }
<SliceOptional>[0-9]+                   {
                                          yyextra->current->type += yytext;
                                        }
<SliceOptional>")"                      {
                                          yyextra->current->type += ")";
                                          if(--yyextra->roundCount<=0)
                                          {
                                            BEGIN (yyextra->lastModifierContext);
                                          }
                                        }
<IDLAttribute>"]"                       {
                                          // end of IDL function attribute
                                          if (--yyextra->squareCount<=0)
                                          {
                                            lineCount(yyscanner);
                                            if (yyextra->current->mtype == MethodTypes::Property)
                                              BEGIN( IDLPropName );
                                            else
                                              BEGIN( yyextra->lastSquareContext );
                                          }
                                        }
<IDLAttribute>"propput"                 {
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT))
                                          {
                                            yyextra->current->mtype = MethodTypes::Property;
                                          }
                                          yyextra->current->spec.setSettable(true);
                                        }
<IDLAttribute>"propget"                 {
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT))
                                          {
                                            yyextra->current->mtype = MethodTypes::Property;
                                          }
                                          yyextra->current->spec.setGettable(true);
                                        }
<IDLAttribute>"property" { // UNO IDL property
                                          yyextra->current->spec.setProperty(true);
                                        }
<IDLAttribute>"attribute" { // UNO IDL attribute
                                          yyextra->current->spec.setAttribute(true);
                                        }
<IDLAttribute>"optional" { // on UNO IDL interface/service/attribute/property
                                          yyextra->current->spec.setOptional(true);
                                        }
<IDLAttribute>"readonly" { // on UNO IDL attribute or property
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT) && yyextra->odlProp)
                                          {
                                            yyextra->current->spec.setSettable(false);
                                          }
                                          else
                                          {
                                            yyextra->current->spec.setReadonly(true);
                                          }
                                        }
<IDLAttribute>"bound" { // on UNO IDL attribute or property
                                          yyextra->current->spec.setBound(true);
                                        }
<IDLAttribute>"removable" { // on UNO IDL property
                                          yyextra->current->spec.setRemovable(true);
                                        }
<IDLAttribute>"constrained" { // on UNO IDL property
                                          yyextra->current->spec.setConstrained(true);
                                        }
<IDLAttribute>"transient" { // on UNO IDL property
                                          yyextra->current->spec.setTransient(true);
                                        }
<IDLAttribute>"maybevoid" { // on UNO IDL property
                                          yyextra->current->spec.setMaybeVoid(true);
                                        }
<IDLAttribute>"maybedefault" { // on UNO IDL property
                                          yyextra->current->spec.setMaybeDefault(true);
                                        }
<IDLAttribute>"maybeambiguous" { // on UNO IDL property
                                          yyextra->current->spec.setMaybeAmbiguous(true);
                                        }
<IDLAttribute>.                         {
                                        }
<IDLPropName>{BN}*{ID}({BN}*[*]*{BN}*)? {
                                          // return type (probably HRESULT) - skip it

                                          if (yyextra->odlProp)
                                          { // property type
                                            yyextra->idlProp = yytext;
                                          }
                                        }
<IDLPropName>{ID}{BN}*"("               {
                                          yyextra->current->name = yytext;
                                          yyextra->current->name = yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          BEGIN( IDLProp );
                                        }
<IDLPropName>{BN}*"("{BN}*{ID}{BN}*")"{BN}*     {
                                          if (yyextra->odlProp)
                                          {
                                            yyextra->idlProp += yytext;
                                          }
                                        }
<IDLPropName>{ID}{BNopt}/";"            {
                                          if (yyextra->odlProp)
                                          {
                                            yyextra->current->name = yytext;
                                            yyextra->idlProp = yyextra->idlProp.stripWhiteSpace();
                                            yyextra->odlProp = false;

                                            BEGIN( IDLProp );
                                          }
                                        }
<IDLProp>{BN}*"["[^\]]*"]"{BN}*         {  // attribute of a parameter
                                           yyextra->idlAttr = yytext;
                                           yyextra->idlAttr=yyextra->idlAttr.stripWhiteSpace();
                                        }
<IDLProp>{ID}                           {  // property type
                                           yyextra->idlProp = yytext;
                                        }
<IDLProp>{BN}*{ID}{BN}*","              {  // Rare: Another parameter ([propput] HRESULT Item(int index, [in] Type theRealProperty);)
                                          if (yyextra->current->args.isEmpty())
                                            yyextra->current->args = "(";
                                          else
                                            yyextra->current->args += ", ";
                                          yyextra->current->args += yyextra->idlAttr;
                                          yyextra->current->args += " ";
                                          yyextra->current->args += yyextra->idlProp;   // prop was actually type of extra parameter
                                          yyextra->current->args += " ";
                                          yyextra->current->args += yytext;
                                          yyextra->current->args = yyextra->current->args.left(yyextra->current->args.length() - 1);    // strip comma
                                          yyextra->idlProp.clear();
                                          yyextra->idlAttr.clear();
                                          BEGIN( IDLProp );
                                        }
<IDLProp>{BN}*{ID}{BN}*")"{BN}*         {
                                          // the parameter name for the property - just skip.
                                        }
<IDLProp>";"                            {
                                          yyextra->current->fileName   = yyextra->fileName;
                                          yyextra->current->type                = yyextra->idlProp;
                                          yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                          if (!yyextra->current->args.isEmpty())
                                            yyextra->current->args += ")";
                                          yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->section = EntryType::makeVariable();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN( FindMembers );
                                        }
<IDLProp>.                              { // spaces, *, or other stuff
                                          //yyextra->idlProp+=yytext;
                                        }
<Array>"]"                              { yyextra->current->args += *yytext ;
                                          if (--yyextra->squareCount<=0)
                                             BEGIN( FindMembers ) ;
                                        }
<FuncFuncArray>"]"                      { yyextra->current->args += *yytext ;
                                          if (--yyextra->squareCount<=0)
                                             BEGIN( SFunction ) ;
                                        }
<Array,FuncFuncArray>"["                { yyextra->current->args += *yytext ;
                                          yyextra->squareCount++;
                                        }
<Array,FuncFuncArray>.                  { yyextra->current->args += *yytext ; }
<SkipSquare>"["                         { yyextra->squareCount++; }
<SkipSquare>"]"                         {
                                          if (--yyextra->squareCount<=0)
                                            BEGIN( yyextra->lastSquareContext );
                                        }
<SkipSquare>\"                          {
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN( SkipString );
                                        }
<SkipSquare>[^\n\[\]\"]+
<FindMembers>"<"                        { addType(yyscanner);
                                          yyextra->current->type += yytext ;
                                          BEGIN( Sharp ) ;
                                        }
<Sharp>">"                              { yyextra->current->type += *yytext ;
                                          if (--yyextra->sharpCount<=0)
                                             BEGIN( FindMembers ) ;
                                        }
<Sharp>"<"                              { yyextra->current->type += *yytext ;
                                          yyextra->sharpCount++;
                                        }
<Sharp>{BN}+                            {
                                          yyextra->current->type += ' ';
                                          lineCount(yyscanner);
                                        }
<Sharp>.                                { yyextra->current->type += *yytext ; }
<FindFields>{ID}                        {
                                          storeClangId(yyscanner,yytext);
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name     = yytext;
                                        }
<FindFields>[({]                        {
                                          // Java enum initializer
                                          unput(*yytext);
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->initializer.str("=");
                                          BEGIN(ReadInitializer);
                                        }
<FindFields>"="                         {
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->initializer.str(yytext);
                                          BEGIN(ReadInitializer);
                                        }
<FindFields>";"                         {
                                          if (yyextra->insideJava)  // yyextra->last enum field in Java class
                                          {
                                            if (!yyextra->current->name.isEmpty())
                                            {
                                              yyextra->current->fileName   = yyextra->fileName;
                                              yyextra->current->startLine  = yyextra->yyLineNr;
                                              yyextra->current->startColumn = yyextra->yyColNr;
                                              if (!yyextra->current_root->spec.isEnum())
                                              {
                                                yyextra->current->type       = "@"; // enum marker
                                              }
                                              yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                              yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                              yyextra->current->section = EntryType::makeVariable();
                                              yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                              initEntry(yyscanner);
                                            }

                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindFields>","                         {
                                          //printf("adding '%s' '%s' '%s' to enum '%s' (mGrpId=%d)\n",
                                          //     qPrint(yyextra->current->type), qPrint(yyextra->current->name),
                                          //     qPrint(yyextra->current->args), qPrint(yyextra->current_root->name),yyextra->current->mGrpId);
                                          if (!yyextra->current->name.isEmpty())
                                          {
                                            yyextra->current->fileName   = yyextra->fileName;
                                            if (yyextra->current_root->section.isEnum() || yyextra->current_root->spec.isEnum())
                                            {
                                              yyextra->current->startLine  = yyextra->current->bodyLine;
                                              yyextra->current->startColumn = yyextra->current->bodyColumn;
                                            }
                                            else
                                            {
                                              yyextra->current->startLine  = yyextra->yyLineNr;
                                              yyextra->current->startColumn = yyextra->yyColNr;
                                            }
                                            if (!yyextra->current_root->spec.isEnum())
                                            {
                                              yyextra->current->type       = "@"; // enum marker
                                            }
                                            yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                            yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                            yyextra->current->section = EntryType::makeVariable();
                                            // add to the scope of the enum
                                            if (!yyextra->insideCS && !yyextra->insideJava &&
                                                !yyextra->current_root->spec.isStrong())
                                                // for C# and Java 1.5+ enum values always have to be explicitly qualified,
                                                // same for C++11 style enums (enum class Name {})
                                            {
                                              // add to the scope surrounding the enum (copy!)
                                              // we cannot add it directly as that would invalidate the iterator in parseCompounds.
                                              //printf("*** adding outer scope entry for %s\n",qPrint(yyextra->current->name));
                                              yyextra->outerScopeEntries.emplace_back(yyextra->current_root->parent(), std::make_shared<Entry>(*yyextra->current));
                                            }
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          else // probably a redundant ,
                                          {
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                          }
                                        }
<FindFields>"["                         { // attribute list in IDL
                                          yyextra->squareCount=1;
                                          yyextra->lastSquareContext = YY_START;
                                          BEGIN(SkipSquare);
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>[^\r\n\#{}"@'/<\\\$R]* { yyextra->current->program << yytext ; } // R because of raw string start
<ReadBody,ReadNSBody,ReadBodyIntf>{CPPC}.*               { yyextra->current->program << yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>"#".* { if (!yyextra->insidePHP)
                                            REJECT;
                                          // append PHP comment.
                                          yyextra->current->program << yytext ;
                                        }
  /* Interpolated string C# */
<SkipCurly,ReadBody,ReadNSBody,ReadBodyIntf,ReadExpressionBody,FindMembers,FindMemberName>$\"   { if (!yyextra->insideCS) REJECT
                                          yyextra->current->program << yytext ;
                                          yyextra->pSkipInterpString = &yyextra->current->program;
                                          yyextra->lastSkipInterpStringContext=YY_START;
                                          yyextra->strCurlyCount = 0;
                                          BEGIN( SkipInterpString );
                                        }
<SkipInterpString>([^"\\{}\x000D\x000A\x0085\x2028\x2029]|"{{"|"}}"|"\\'"|"\\\""|"\\\\"|"\\0"|"\\a"|"\\b"|"\\f"|"\\n"|"\\r"|"\\t"|"\\v"|"\\x"{HEXDIGIT}{HEXDIGIT}?{HEXDIGIT}?{HEXDIGIT}?|"\\"[uU]{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}{HEXDIGIT})* {
                                          *yyextra->pSkipInterpString << yytext;
                                        }
<SkipInterpString>"{"                   {
                                          *yyextra->pSkipInterpString << *yytext;
                                          yyextra->strCurlyCount++;
                                        }
<SkipInterpString>"}"                   {
                                          *yyextra->pSkipInterpString << *yytext;
                                          yyextra->strCurlyCount--;
                                        }
<SkipInterpString>\"                    {
                                          *yyextra->pSkipInterpString << *yytext;
                                          if (yyextra->strCurlyCount==0)
                                          {
                                            BEGIN( yyextra->lastSkipInterpStringContext );
                                          }
                                        }
<SkipInterpString>.                     {
                                          *yyextra->pSkipInterpString << *yytext;
                                        }
  /* Verbatim Interpolated string C# */
<SkipCurly,ReadBody,ReadNSBody,ReadBodyIntf,ReadExpressionBody,FindMembers,FindMemberName>$@\"   { if (!yyextra->insideCS) REJECT
                                          yyextra->current->program << yytext ;
                                          yyextra->pSkipInterpVerbString = &yyextra->current->program;
                                          yyextra->lastSkipInterpVerbStringContext=YY_START;
                                          yyextra->strCurlyCount = 0;
                                          BEGIN( SkipInterpVerbString );
                                        }
<SkipInterpVerbString>([^\"{}]|"{{"|"}}"|"\"\"")* {
                                          *yyextra->pSkipInterpVerbString << yytext;
                                        }
<SkipInterpVerbString>"{"               {
                                          *yyextra->pSkipInterpVerbString << *yytext;
                                          yyextra->strCurlyCount++;
                                        }
<SkipInterpVerbString>"}"               {
                                          *yyextra->pSkipInterpVerbString << *yytext;
                                          yyextra->strCurlyCount--;
                                        }
<SkipInterpVerbString>\"                {
                                          *yyextra->pSkipInterpVerbString << *yytext;
                                          if (yyextra->strCurlyCount==0)
                                          {
                                            BEGIN( yyextra->lastSkipInterpVerbStringContext );
                                          }
                                        }
<SkipInterpVerbString>.                 {
                                          *yyextra->pSkipInterpVerbString << *yytext;
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>"\$"  { yyextra->current->program << yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>@\"   { yyextra->current->program << yytext ;
                                          yyextra->pSkipVerbString = &yyextra->current->program;
                                          yyextra->lastSkipVerbStringContext=YY_START;
                                          BEGIN( SkipVerbString );
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>"<<<" { if (yyextra->insidePHP)
                                          {
                                            yyextra->current->program << yytext ;
                                            yyextra->pCopyHereDocGString = &yyextra->current->program;
                                            yyextra->lastHereDocContext=YY_START;
                                            BEGIN( CopyHereDoc );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>{RAWBEGIN} {
                                            yyextra->current->program << yytext;
                                            yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
                                            yyextra->lastRawStringContext = YY_START;
                                            yyextra->pCopyRawGString = &yyextra->current->program;
                                            BEGIN(RawGString);
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>\"    { yyextra->current->program << yytext ;
                                          yyextra->pCopyQuotedGString = &yyextra->current->program;
                                          yyextra->lastStringContext=YY_START;
                                          yyextra->stopAtInvalidString=false;
                                          BEGIN( CopyGString );
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>{DCOMMC}  { yyextra->doxygenComment=true; REJECT;}
<ReadBody,ReadNSBody,ReadBodyIntf>{CCS}{B}* { yyextra->current->program << yytext ;
                                          yyextra->lastContext = YY_START ;
                                          BEGIN( Comment ) ;
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>{CCS}{BL} { yyextra->current->program << yytext ;
                                          ++yyextra->yyLineNr ;
                                          yyextra->lastContext = YY_START ;
                                          BEGIN( Comment ) ;
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>"'"   {
                                          if (!yyextra->insidePHP)
                                          {
                                            yyextra->current->program << yytext;
                                          }
                                          else
                                          { // begin of single quoted string
                                            yyextra->current->program << yytext;
                                            yyextra->pCopyQuotedGString = &yyextra->current->program;
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(CopyPHPGString);
                                          }
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>{CHARLIT} {
                                              if (yyextra->insidePHP)
                                              {
                                                REJECT; // for PHP code single quotes
                                                        // are used for strings of arbitrary length
                                              }
                                              else
                                              {
                                                yyextra->current->program << yytext;
                                              }
                                            }
<ReadBody,ReadNSBody,ReadBodyIntf>"{"   { yyextra->current->program << yytext ;
                                          ++yyextra->curlyCount ;
                                        }
<ReadBodyIntf>"}"                       {
                                          yyextra->current->program << yytext ;
                                          --yyextra->curlyCount ;
                                        }
<ReadBody,ReadNSBody>"}"                {
                                          if ( yyextra->curlyCount>0 )
                                          {
                                            yyextra->current->program << yytext ;
                                            --yyextra->curlyCount ;
                                          }
                                          else
                                          {
                                            yyextra->current->endBodyLine = yyextra->yyLineNr;
                                            std::shared_ptr<Entry> original_root = yyextra->current_root; // save root this namespace is in
                                            if (yyextra->current->section.isNamespace() && yyextra->current->type == "namespace")
                                            {
                                              int split_point;
                                              // save documentation values
                                              QCString doc       = yyextra->current->doc;
                                              int      docLine   = yyextra->current->docLine;
                                              QCString docFile   = yyextra->current->docFile;
                                              QCString brief     = yyextra->current->brief;
                                              int      briefLine = yyextra->current->briefLine;
                                              QCString briefFile = yyextra->current->briefFile;
                                              // reset documentation values
                                              yyextra->current->doc       = "";
                                              yyextra->current->docLine   = 0;
                                              yyextra->current->docFile   = "";
                                              yyextra->current->brief     = "";
                                              yyextra->current->briefLine = 0;
                                              yyextra->current->briefFile = "";
                                              while ((split_point = yyextra->current->name.find("::")) != -1)
                                              {
                                                std::shared_ptr<Entry> new_current = std::make_shared<Entry>(*yyextra->current);
                                                yyextra->current->program.str(std::string());
                                                new_current->name  = yyextra->current->name.mid(split_point + 2);
                                                yyextra->current->name  = yyextra->current->name.left(split_point);
                                                if (!yyextra->current_root->name.isEmpty()) yyextra->current->name.prepend(yyextra->current_root->name+"::");

                                                yyextra->current_root->moveToSubEntryAndKeep(yyextra->current);
                                                yyextra->current_root = yyextra->current;
                                                yyextra->current = new_current;
                                              }
                                              // restore documentation values
                                              yyextra->current->doc       = doc;
                                              yyextra->current->docLine   = docLine;
                                              yyextra->current->docFile   = docFile;
                                              yyextra->current->brief     = brief;
                                              yyextra->current->briefLine = briefLine;
                                              yyextra->current->briefFile = briefFile;
                                            }
                                            QCString &cn = yyextra->current->name;
                                            QCString rn = yyextra->current_root->name;
                                            //printf("cn='%s' rn='%s' yyextra->isTypedef=%d\n",qPrint(cn),qPrint(rn),yyextra->isTypedef);
                                            if (!cn.isEmpty() && !rn.isEmpty())
                                            {
                                              prependScope(yyscanner);
                                            }
                                            if (yyextra->isTypedef && cn.isEmpty())
                                            {
                                              //printf("Typedef Name\n");
                                              BEGIN( TypedefName );
                                            }
                                            else
                                            {
                                              if (yyextra->current->section.isEnum() || yyextra->current->spec.isEnum())
                                              {
                                                yyextra->current->program << ','; // add field terminator
                                              }
                                              // add compound definition to the tree
                                              yyextra->current->args=removeRedundantWhiteSpace(yyextra->current->args);
                                                        // was: yyextra->current->args.simplifyWhiteSpace();
                                              yyextra->current->type = yyextra->current->type.simplifyWhiteSpace();
                                              yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                              //printf("adding '%s' '%s' '%s' brief=%s yyextra->insideObjC=%d %x\n",qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args),qPrint(yyextra->current->brief),yyextra->insideObjC,yyextra->current->section);
                                              if (yyextra->insideObjC &&
                                                  (yyextra->current->spec.isInterface() || yyextra->current->spec.isCategory())
                                                 ) // method definition follows
                                              {
                                                BEGIN( ReadBodyIntf ) ;
                                              }
                                              else
                                              {
                                                yyextra->memspecEntry = yyextra->current;
                                                yyextra->current_root->moveToSubEntryAndKeep( yyextra->current ) ;
                                                yyextra->current = std::make_shared<Entry>(*yyextra->current);
                                                if (yyextra->current->section.isNamespace() ||
                                                    yyextra->current->spec.isInterface() ||
                                                    yyextra->insideJava || yyextra->insidePHP || yyextra->insideCS || yyextra->insideD || yyextra->insideJS ||
                                                    yyextra->insideSlice
                                                   )
                                                { // namespaces and interfaces and java classes ends with a closing bracket without semicolon
                                                  yyextra->current->reset();
                                                  yyextra->current_root = std::move(original_root); // restore scope from before namespace descent
                                                  initEntry(yyscanner);
                                                  yyextra->memspecEntry.reset();
                                                  BEGIN( FindMembers ) ;
                                                }
                                                else
                                                {
                                                  static const reg::Ex re(R"(@\d+$)");
                                                  if (!yyextra->isTypedef && yyextra->memspecEntry &&
                                                      !reg::search(yyextra->memspecEntry->name.str(),re)) // not typedef or anonymous type (see bug691071)
                                                  {
                                                    // enabled the next two lines for bug 623424
                                                    yyextra->current->doc.clear();
                                                    yyextra->current->brief.clear();
                                                  }
                                                  BEGIN( MemberSpec ) ;
                                                }
                                              }
                                            }
                                          }
                                        }
<ReadBody>"}"{BN}+"typedef"{BN}+        { 
                                          lineCount(yyscanner);
                                          if ( yyextra->curlyCount>0 )
                                          {
                                            yyextra->current->program << yytext ;
                                            --yyextra->curlyCount ;
                                          }
                                          else
                                          {
                                            yyextra->isTypedef = TRUE;
                                            yyextra->current->endBodyLine = yyextra->yyLineNr;
                                            QCString &cn = yyextra->current->name;
                                            QCString rn = yyextra->current_root->name;
                                            if (!cn.isEmpty() && !rn.isEmpty())
                                            {
                                              prependScope(yyscanner);
                                            }
                                            BEGIN( TypedefName );
                                          }
                                        }
<TypedefName>("const"|"volatile"){BN}   { // late "const" or "volatile" keyword
                                          lineCount(yyscanner);
                                          yyextra->current->type.prepend(yytext);
                                        }
<TypedefName>{ID}                       {
                                          if (yyextra->current->section.isEnum() || yyextra->current->spec.isEnum())
                                          {
                                            yyextra->current->program << ","; // add field terminator
                                          }
                                          yyextra->current->name=yytext;
                                          prependScope(yyscanner);
                                          yyextra->current->args = yyextra->current->args.simplifyWhiteSpace();
                                          yyextra->current->type = yyextra->current->type.simplifyWhiteSpace();
                                          //printf("Adding compound %s %s %s\n",qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                          if (!yyextra->firstTypedefEntry)
                                          {
                                            yyextra->firstTypedefEntry = yyextra->current;
                                          }
                                          yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                          initEntry(yyscanner);
                                          yyextra->isTypedef=TRUE; // to undo reset by initEntry(yyscanner)
                                          BEGIN(MemberSpecSkip);
                                        }
<TypedefName>";"                        { /* typedef of anonymous type */
                                          yyextra->current->name = generateAnonymousAnchor(yyextra->fileName,yyextra->anonCount++);
                                          if (yyextra->current->section.isEnum() || yyextra->current->spec.isEnum())
                                          {
                                            yyextra->current->program << ','; // add field terminator
                                          }
                                          // add compound definition to the tree
                                          yyextra->current->args = yyextra->current->args.simplifyWhiteSpace();
                                          yyextra->current->type = yyextra->current->type.simplifyWhiteSpace();
                                          yyextra->memspecEntry = yyextra->current;
                                          yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                          initEntry(yyscanner);
                                          unput(';');
                                          BEGIN( MemberSpec ) ;
                                        }
<MemberSpec>([*&]*{BN}*)*{ID}{BN}*("["[^\]\n]*"]")* { // the [] part could be improved.
                                          lineCount(yyscanner);
                                          int i=0,l=(int)yyleng,j;
                                          while (i<l && (!isId(yytext[i]))) i++;
                                          yyextra->msName = QCString(yytext).right(l-i).stripWhiteSpace();
                                          j=yyextra->msName.find("[");
                                          if (j!=-1)
                                          {
                                            yyextra->msArgs=yyextra->msName.right(yyextra->msName.length()-j);
                                            yyextra->msName=yyextra->msName.left(j);
                                          }
                                          yyextra->msType=QCString(yytext).left(i);

                                          // handle *pName in: typedef { ... } name, *pName;
                                          if (yyextra->firstTypedefEntry)
                                          {
                                            if (yyextra->firstTypedefEntry->spec.isStruct())
                                            {
                                              yyextra->msType.prepend("struct "+yyextra->firstTypedefEntry->name);
                                            }
                                            else if (yyextra->firstTypedefEntry->spec.isUnion())
                                            {
                                              yyextra->msType.prepend("union "+yyextra->firstTypedefEntry->name);
                                            }
                                            else if (yyextra->firstTypedefEntry->section.isEnum())
                                            {
                                              yyextra->msType.prepend("enum "+yyextra->firstTypedefEntry->name);
                                            }
                                            else
                                            {
                                              yyextra->msType.prepend(yyextra->firstTypedefEntry->name);
                                            }
                                          }
                                        }
<MemberSpec>"("                         { // function with struct return type
                                          addType(yyscanner);
                                          yyextra->current->name = yyextra->msName;
                                          yyextra->current->spec = TypeSpecifier();
                                          unput('(');
                                          BEGIN(FindMembers);
                                        }
<MemberSpec>[,;]                        {
                                          if (yyextra->msName.isEmpty() && !yyextra->current->name.isEmpty())
                                          {
                                            // see if the compound does not have a name or is yyextra->inside another
                                            // anonymous compound. If so we insert a
                                            // special 'anonymous' variable.
                                            //Entry *p=yyextra->current_root;
                                            const Entry *p=yyextra->current.get();
                                            while (p)
                                            {
                                              // only look for class scopes, not namespace scopes
                                              if (p->section.isCompound() && !p->name.isEmpty())
                                              {
                                                //printf("Trying scope '%s'\n",qPrint(p->name));
                                                int i=p->name.findRev("::");
                                                int pi = (i==-1) ? 0 : i+2;
                                                if (p->name.at(pi)=='@')
                                                {
                                                  // anonymous compound yyextra->inside -> insert dummy variable name
                                                  //printf("Adding anonymous variable for scope %s\n",qPrint(p->name));
                                                  yyextra->msName = generateAnonymousAnchor(yyextra->fileName,yyextra->anonCount++);
                                                  break;
                                                }
                                              }
                                              //p=p->parent;
                                              if (p==yyextra->current.get()) p=yyextra->current_root.get(); else p=p->parent();
                                            }
                                          }
                                          //printf("yyextra->msName=%s yyextra->current->name=%s\n",qPrint(yyextra->msName),qPrint(yyextra->current->name));
                                          if (!yyextra->msName.isEmpty()
                                              /*&& yyextra->msName!=yyextra->current->name*/) // skip typedef T {} T;, removed due to bug608493
                                          {
                                            bool typedefHidesStruct = Config_getBool(TYPEDEF_HIDES_STRUCT);
                                            // case 1: typedef struct _S { ... } S_t;
                                            // -> omit typedef and use S_t as the struct name
                                            if (typedefHidesStruct &&
                                                yyextra->isTypedef &&
                                                ((yyextra->current->spec.isStruct() || yyextra->current->spec.isUnion()) || yyextra->current->section.isEnum()) &&
                                                yyextra->msType.stripWhiteSpace().isEmpty() &&
                                                yyextra->memspecEntry)
                                            {
                                              yyextra->memspecEntry->name=yyextra->msName;
                                            }
                                            else // case 2: create a typedef field
                                            {
                                              std::shared_ptr<Entry> varEntry=std::make_shared<Entry>();
                                              varEntry->lang = yyextra->language;
                                              varEntry->protection = yyextra->current->protection ;
                                              varEntry->mtype = yyextra->current->mtype;
                                              varEntry->virt = yyextra->current->virt;
                                              varEntry->isStatic = yyextra->current->isStatic;
                                              varEntry->section = EntryType::makeVariable();
                                              varEntry->name = yyextra->msName.stripWhiteSpace();
                                              varEntry->type = yyextra->current->type.simplifyWhiteSpace()+" ";
                                              varEntry->args = yyextra->msArgs;
                                              if (yyextra->isTypedef)
                                              {
                                                varEntry->type.prepend("typedef ");
                                                //  //printf("yyextra->current->name = %s %s\n",qPrint(yyextra->current->name),qPrint(yyextra->msName));
                                              }
                                              if (typedefHidesStruct &&
                                                  yyextra->isTypedef &&
                                                  (yyextra->current->spec.isStruct() || yyextra->current->spec.isUnion()) &&
                                                  yyextra->memspecEntry
                                                 ) // case 1: use S_t as type for pS_t in "typedef struct _S {} S_t, *pS_t;"
                                              {
                                                varEntry->type+=yyextra->memspecEntry->name+yyextra->msType;
                                              }
                                              else // case 2: use _S as type for for pS_t
                                              {
                                                varEntry->type+=yyextra->current->name+yyextra->msType;
                                              }
                                              varEntry->fileName = yyextra->fileName;
                                              varEntry->startLine = yyextra->yyLineNr;
                                              varEntry->startColumn = yyextra->yyColNr;
                                              varEntry->doc = yyextra->current->doc;
                                              varEntry->brief = yyextra->current->brief;
                                              varEntry->mGrpId = yyextra->current->mGrpId;
                                              varEntry->initializer.str(yyextra->current->initializer.str());
                                              varEntry->groups = yyextra->current->groups;
                                              varEntry->sli = yyextra->current->sli;

                                              //printf("Add: type='%s',name='%s',args='%s' brief=%s doc=%s\n",
                                              //      qPrint(varEntry->type),qPrint(varEntry->name),
                                              //      qPrint(varEntry->args),qPrint(varEntry->brief),qPrint(varEntry->doc));
                                              yyextra->current_root->moveToSubEntryAndKeep(varEntry);
                                            }
                                          }
                                          if (*yytext==';') // end of a struct/class ...
                                          {
                                            if (!yyextra->isTypedef && yyextra->msName.isEmpty() && yyextra->memspecEntry && yyextra->current->section.isCompound())
                                            { // case where a class/struct has a doc block after it
                                              if (!yyextra->current->doc.isEmpty())
                                              {
                                                yyextra->memspecEntry->doc += yyextra->current->doc;
                                              }
                                              if (!yyextra->current->brief.isEmpty())
                                              {
                                                yyextra->memspecEntry->brief += yyextra->current->brief;
                                              }
                                            }
                                            yyextra->msType.clear();
                                            yyextra->msName.clear();
                                            yyextra->msArgs.clear();
                                            yyextra->isTypedef=FALSE;
                                            yyextra->firstTypedefEntry.reset();
                                            yyextra->memspecEntry.reset();
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            yyextra->current->doc.clear();
                                            yyextra->current->brief.clear();
                                          }

                                        }
<MemberSpec>"="                         {
                                          yyextra->lastInitializerContext=YY_START;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->initializer.str(yytext);
                                          BEGIN(ReadInitializer);
                                          /* BEGIN(MemberSpecSkip); */
                                        }
  /*
<MemberSpecSkip>"{"                     {
                                          yyextra->curlyCount=0;
                                          yyextra->lastCurlyContext = MemberSpecSkip;
                                          yyextra->previous = yyextra->current;
                                          BEGIN(SkipCurly);
                                        }
  */
<MemberSpecSkip>","                     { BEGIN(MemberSpec); }
<MemberSpecSkip>";"                     { unput(';'); BEGIN(MemberSpec); }
<ReadBody,ReadNSBody,ReadBodyIntf>{BN}{1,80} { yyextra->current->program << yytext ;
                                          lineCount(yyscanner) ;
                                        }
<ReadBodyIntf>"@end"/[^a-z_A-Z0-9]      { // end of Objective C block
                                          yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                          initEntry(yyscanner);
                                          yyextra->language = yyextra->current->lang = SrcLangExt::Cpp; // see bug746361
                                          yyextra->insideObjC=FALSE;
                                          BEGIN( FindMembers );
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>\\.   { yyextra->current->program << yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>.     { yyextra->current->program << yytext ; }

<FindMembers>"("/{BN}*"::"*{BN}*({TSCOPE}{BN}*"::")*{TSCOPE}{BN}*")"{BN}*"(" | /* typedef void (A<int>::func_t)(args...) */
<FindMembers>("("({BN}*"::"*{BN}*{TSCOPE}{BN}*"::")*({BN}*[*&\^]{BN}*)+)+ {   /* typedef void (A::*ptr_t)(args...) or int (*func(int))[], the ^ is for Obj-C blocks */
                                          if (yyextra->insidePHP) // reference parameter
                                          {
                                            REJECT
                                          }
                                          else
                                          {
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            lineCount(yyscanner);
                                            addType(yyscanner);
                                            yyextra->funcPtrType=yytext;
                                            yyextra->roundCount=0;
                                            //yyextra->current->type += yytext;
                                            BEGIN( FuncPtr );
                                          }
                                        }
<FuncPtr>{SCOPENAME}                    {
                                          yyextra->current->name = yytext;
                                          if (nameIsOperator(yyextra->current->name))
                                          {
                                            BEGIN( FuncPtrOperator );
                                          }
                                          else
                                          {
                                            if (yyextra->current->name=="const" || yyextra->current->name=="volatile")
                                            {
                                              yyextra->funcPtrType += yyextra->current->name;
                                            }
                                            else
                                            {
                                              BEGIN( EndFuncPtr );
                                            }
                                          }
                                        }
<FuncPtr>.                              {
                                          //printf("error: FuncPtr '%c' unexpected at line %d of %s\n",*yytext,yyextra->yyLineNr,yyextra->fileName);
                                        }
<FuncPtrOperator>"("{BN}*")"{BNopt}/"(" {
                                          yyextra->current->name += yytext;
                                          yyextra->current->name = yyextra->current->name.simplifyWhiteSpace();
                                          lineCount(yyscanner);
                                        }
<FuncPtrOperator>\n                     {
                                          lineCount(yyscanner);
                                          yyextra->current->name += *yytext;
                                        }
<FuncPtrOperator>"("                    {
                                          unput(*yytext);
                                          BEGIN( EndFuncPtr );
                                        }
<FuncPtrOperator>.                      {
                                          yyextra->current->name += *yytext;
                                        }
<EndFuncPtr>")"{BNopt}/";"              { // a variable with extra braces
                                          lineCount(yyscanner);
                                          yyextra->current->type+=yyextra->funcPtrType.mid(1);
                                          BEGIN(FindMembers);
                                        }
<EndFuncPtr>")"{BNopt}/"("              { // a function pointer
                                          lineCount(yyscanner);
                                          if (yyextra->funcPtrType!="(") // not just redundant braces
                                          {
                                            yyextra->current->type+=yyextra->funcPtrType+")";
                                          }
                                          BEGIN(FindMembers);
                                        }
<EndFuncPtr>")"{BNopt}/"["              { // an array of variables
                                          lineCount(yyscanner);
                                          yyextra->current->type+=yyextra->funcPtrType;
                                          yyextra->current->args += ")";
                                          BEGIN(FindMembers);
                                        }
<EndFuncPtr>"("                         { // a function returning a function or
                                          // a function returning a pointer to an array
                                          yyextra->current->args += *yytext ;
                                          //yyextra->roundCount=0;
                                          //BEGIN( FuncFunc );
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->currentArgumentContext = FuncFuncEnd;
                                          yyextra->fullArgString=yyextra->current->args;
                                          yyextra->copyArgString=&yyextra->current->args;
                                          BEGIN( ReadFuncArgType ) ;
                                        }
<EndFuncPtr>"["[^\n\]]*"]"              {
                                          yyextra->funcPtrType+=yytext;
                                        }
<EndFuncPtr>")"                         {
                                          BEGIN(FindMembers);
                                        }
<FuncFunc>"("                           {
                                          yyextra->current->args += *yytext ;
                                          ++yyextra->roundCount;
                                        }
<FuncFunc>")"                           {
                                          yyextra->current->args += *yytext ;
                                          if ( yyextra->roundCount )
                                            --yyextra->roundCount;
                                          else
                                          {
                                            BEGIN(FuncFuncEnd);
                                          }
                                        }
<FuncFuncEnd>")"{BN}*"("                {
                                          lineCount(yyscanner);
                                          yyextra->current->type+=yyextra->funcPtrType+")(";
                                          BEGIN(FuncFuncType);
                                        }
<FuncFuncEnd>")"{BNopt}/[;{]            {
                                          lineCount(yyscanner);
                                          yyextra->current->type+=yyextra->funcPtrType.mid(1);
                                          BEGIN(SFunction);
                                        }
<FuncFuncEnd>")"{BNopt}/"["             { // function returning a pointer to an array
                                          lineCount(yyscanner);
                                          yyextra->current->type+=yyextra->funcPtrType;
                                          yyextra->current->args+=")";
                                          BEGIN(FuncFuncArray);
                                        }
<FuncFuncEnd>.                          {
                                          yyextra->current->args += *yytext;
                                        }
<FuncFuncType>"("                       {
                                          yyextra->current->type += *yytext;
                                          yyextra->roundCount++;
                                        }
<FuncFuncType>")"                       {
                                          yyextra->current->type += *yytext;
                                          if (yyextra->roundCount)
                                            --yyextra->roundCount;
                                          else
                                            BEGIN(SFunction);
                                        }
<FuncFuncType>{BN}*","{BN}*             { lineCount(yyscanner) ; yyextra->current->type += ", " ; }
<FuncFuncType>{BN}+                     { lineCount(yyscanner) ; yyextra->current->type += ' ' ; }
<FuncFuncType>.                         {
                                          yyextra->current->type += *yytext;
                                        }
<FindMembers>"("/{BN}*{ID}{BN}*"*"{BN}*{ID}*")"{BN}*"(" { // for catching typedef void (__stdcall *f)() like definitions
                                          if (yyextra->current->type.startsWith("typedef") &&
                                              yyextra->current->bodyLine==-1)
                                            // the bodyLine check is to prevent this guard to be true more than once
                                          {
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            BEGIN( GetCallType );
                                          }
                                          else if (!yyextra->current->name.isEmpty()) // normal function
                                          {
                                            yyextra->current->args = yytext;
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->currentArgumentContext = FuncQual;
                                            yyextra->fullArgString=yyextra->current->args;
                                            yyextra->copyArgString=&yyextra->current->args;
                                            BEGIN( ReadFuncArgType ) ;
                                            //printf(">>> Read function arguments!\n");
                                          }
                                        }
<GetCallType>{BN}*{ID}{BN}*"*"          {
                                          lineCount(yyscanner);
                                          addType(yyscanner);
                                          yyextra->funcPtrType="(";
                                          yyextra->funcPtrType+=yytext;
                                          yyextra->roundCount=0;
                                          BEGIN( FuncPtr );
                                        }
<FindMembers>"("                        {
                                          if (!yyextra->current->name.isEmpty())
                                          {
                                            yyextra->current->args = yytext;
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->currentArgumentContext = FuncQual;
                                            yyextra->current->req.clear(); // see issue #11895
                                            yyextra->fullArgString=yyextra->current->args;
                                            yyextra->copyArgString=&yyextra->current->args;
                                            BEGIN( ReadFuncArgType ) ;
                                            //printf(">>> Read function arguments yyextra->current->argList.size()=%d\n",yyextra->current->argList.size());
                                          }
                                        }
  /*
<FindMembers>"("{BN}*("void"{BN}*)?")"  {
                                          lineCount(yyscanner);
                                          yyextra->current->args = "()";
                                          BEGIN( FuncQual );
                                        }
  */

  /*- Function argument reading rules ---------------------------------------*/

<ReadFuncArgType>[^ \/\r\t\n\[\]\)\(\"\'#]+ { *yyextra->copyArgString+=yytext;
                                          if (yyextra->insideCS) yyextra->fullArgString+=substitute(yytext,".","::");
                                          else yyextra->fullArgString+=yytext;
                                        }
<CopyArgString,CopyArgPHPString>[^\n\\\"\']+            { *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<CopyArgRound>[^\/\n\)\(\"\']+          {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<CopyArgSquare>[^\/\n\]\[\"\']+          {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<ReadFuncArgType,ReadTempArgs>{BN}*     {
                                          *yyextra->copyArgString+=" ";
                                          yyextra->fullArgString+=" ";
                                          lineCount(yyscanner);
                                        }
<ReadFuncArgType,CopyArgRound,CopyArgSquare,CopyArgSharp,ReadTempArgs>{RAWBEGIN}      {
                                          yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
                                          yyextra->lastRawStringContext = YY_START;
                                          yyextra->pCopyRawString = yyextra->copyArgString;
                                          *yyextra->pCopyRawString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(RawString);
                                        }
<ReadFuncArgType,CopyArgRound,CopyArgSquare,CopyArgSharp,ReadTempArgs>\"      {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->lastCopyArgStringContext = YY_START;
                                          BEGIN( CopyArgString );
                                        }
<ReadFuncArgType>"["                    {
                                          if (!yyextra->insidePHP) REJECT;
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->argSquareCount=0;
                                          yyextra->lastCopyArgContext = YY_START;
                                          BEGIN( CopyArgSquare );
                                        }
<ReadFuncArgType,ReadTempArgs>"("       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->argRoundCount=0;
                                          yyextra->lastCopyArgContext = YY_START;
                                          BEGIN( CopyArgRound );
                                        }
<ReadFuncArgType>")"                    {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->current->argList = *stringToArgumentList(yyextra->language, yyextra->fullArgString);
                                          if (yyextra->insideJS)
                                          {
                                            fixArgumentListForJavaScript(yyextra->current->argList);
                                          }
                                          handleParametersCommentBlocks(yyscanner,yyextra->current->argList);

                                          /* remember the yyextra->current documentation block, since
                                             we could overwrite it with the documentation of
                                             a function argument, which we then have to correct later
                                             on
                                           */
                                          yyextra->docBackup = yyextra->current->doc;
                                          yyextra->briefBackup = yyextra->current->brief;

                                          BEGIN( yyextra->currentArgumentContext );
                                        }
        /* a special comment */
<ReadFuncArgType,ReadTempArgs>({CCS}[*!]|{CPPC}[/!])("<"?) {
                                          if (yyextra->currentArgumentContext==DefineEnd)
                                          {
                                            // for defines we interpret a comment
                                            // as documentation for the define
                                            int i;for (i=(int)yyleng-1;i>=0;i--)
                                            {
                                              unput(yytext[i]);
                                            }
                                            yyextra->current->argList = *stringToArgumentList(yyextra->language, yyextra->fullArgString);
                                            handleParametersCommentBlocks(yyscanner,yyextra->current->argList);
                                            BEGIN( yyextra->currentArgumentContext );
                                          }
                                          else // not a define
                                          {
                                            // for functions we interpret a comment
                                            // as documentation for the argument
                                            yyextra->fullArgString+=yytext;
                                            yyextra->lastCopyArgChar=0;
                                            yyextra->lastCommentInArgContext=YY_START;
                                            if (yytext[1]=='/')
                                              BEGIN( CopyArgCommentLine );
                                            else
                                              BEGIN( CopyArgComment );
                                          }
                                        }
        /* a non-special comment */
<ReadFuncArgType,ReadTempArgs>{CCS}{CCE}    { /* empty comment */ }
<ReadFuncArgType,ReadTempArgs>{CCS}      {
                                          yyextra->lastCContext = YY_START;
                                          BEGIN( SkipComment );
                                        }
<ReadFuncArgType,ReadTempArgs>{CPPC}      {
                                          yyextra->lastCContext = YY_START;
                                          BEGIN( SkipCxxComment );
                                        }
  /*
<ReadFuncArgType,ReadTempArgs>"'#"      { if (yyextra->insidePHP)
                                            REJECT;
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<ReadFuncArgType,ReadTempArgs>"#"       {
                                          if (!yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCContext = YY_START;
                                          BEGIN( SkipCxxComment );
                                        }
  */
        /* ')' followed by a special comment */
<ReadFuncArgType>")"{BN}*({CCS}[*!]|{CPPC}[/!])"<" {
                                          lineCount(yyscanner);
                                          if (yyextra->currentArgumentContext==DefineEnd)
                                          {
                                            // for defines we interpret a comment
                                            // as documentation for the define
                                            int i;for (i=(int)yyleng-1;i>0;i--)
                                            {
                                              unput(yytext[i]);
                                            }
                                            *yyextra->copyArgString+=*yytext;
                                            yyextra->fullArgString+=*yytext;
                                            yyextra->current->argList = *stringToArgumentList(yyextra->language, yyextra->fullArgString);
                                            handleParametersCommentBlocks(yyscanner,yyextra->current->argList);
                                            BEGIN( yyextra->currentArgumentContext );
                                          }
                                          else
                                          {
                                            // for functions we interpret a comment
                                            // as documentation for the yyextra->last argument
                                            yyextra->lastCopyArgChar=*yytext;
                                            QCString text=&yytext[1];
                                            text=text.stripWhiteSpace();
                                            yyextra->lastCommentInArgContext=YY_START;
                                            yyextra->fullArgString+=text;
                                            if (text.find("//")!=-1)
                                              BEGIN( CopyArgCommentLine );
                                            else
                                              BEGIN( CopyArgComment );
                                          }
                                        }
<CopyArgComment>^{B}*"*"+/{BN}+
<CopyArgComment>[^\n\\\@\*]+            { yyextra->fullArgString+=yytext; }
<CopyArgComment>{CCE}                    { yyextra->fullArgString+=yytext;
                                          if (yyextra->lastCopyArgChar!=0)
                                            unput(yyextra->lastCopyArgChar);
                                          BEGIN( yyextra->lastCommentInArgContext );
                                        }
<CopyArgCommentLine>\n                  { yyextra->fullArgString+=yytext;
                                          lineCount(yyscanner);
                                          if (yyextra->lastCopyArgChar!=0)
                                            unput(yyextra->lastCopyArgChar);
                                          BEGIN( yyextra->lastCommentInArgContext );
                                        }
<CopyArgCommentLine>{CMD}"startuml"/[^a-z_A-Z0-9\-]   { // verbatim type command (which could contain nested comments!)
                                          yyextra->docBlockName="uml";
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(CopyArgVerbatim);
                                        }
<CopyArgCommentLine>{CMD}("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc"|"code")/[^a-z_A-Z0-9\-]   { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlockName=&yytext[1];
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(CopyArgVerbatim);
                                        }
<CopyArgCommentLine>{CMD}("f$"|"f["|"f{"|"f(")               {
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
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(CopyArgVerbatim);
                                        }
<CopyArgVerbatim>{CMD}("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endmsc"|"enduml"|"endcode")/[^a-z_A-Z0-9\-] { // end of verbatim block
                                          yyextra->fullArgString+=yytext;
                                          if (&yytext[4]==yyextra->docBlockName)
                                          {
                                            yyextra->docBlockName="";
                                            BEGIN(CopyArgCommentLine);
                                          }
                                        }
<CopyArgVerbatim>{CMD}("f$"|"f]"|"f}"|"f)") { // end of verbatim block
                                          yyextra->fullArgString+=yytext;
                                          if (yyextra->docBlockName==&yytext[1])
                                          {
                                            yyextra->docBlockName="";
                                            BEGIN(CopyArgCommentLine);
                                          }
                                        }
<CopyArgCommentLine>[^\\\@\n]+          { yyextra->fullArgString+=yytext; }
<CopyArgCommentLine>.                   { yyextra->fullArgString+=*yytext; }
<CopyArgComment,CopyArgVerbatim>\n      { yyextra->fullArgString+=*yytext; lineCount(yyscanner); }
<CopyArgComment,CopyArgVerbatim>.       { yyextra->fullArgString+=*yytext; }
<CopyArgComment>{CMD}("brief"|"short"){B}+ {
                                          warn(yyextra->fileName,yyextra->yyLineNr,
                                              "Ignoring {:c}brief command inside argument documentation",*yytext
                                             );
                                          yyextra->fullArgString+=' ';
                                        }
<ReadTempArgs>"<"                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->argSharpCount=1;
                                          BEGIN( CopyArgSharp );
                                        }
<ReadTempArgs>">"                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          //printf("end template list '%s'\n",qPrint(*yyextra->copyArgString));
                                          *yyextra->currentArgumentList = *stringToArgumentList(yyextra->language, yyextra->fullArgString);
                                          handleParametersCommentBlocks(yyscanner,yyextra->current->tArgLists.back());
                                          BEGIN( yyextra->currentArgumentContext );
                                        }
<CopyArgRound>"("                       {
                                          yyextra->argRoundCount++;
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                        }
<CopyArgRound>")"                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          if (yyextra->argRoundCount>0)
                                            yyextra->argRoundCount--;
                                          else
                                            BEGIN( yyextra->lastCopyArgContext );
                                        }
<CopyArgSquare>"["                       {
                                          yyextra->argSquareCount++;
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                        }
<CopyArgSquare>"]"                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          if (yyextra->argSquareCount>0)
                                            yyextra->argSquareCount--;
                                          else
                                            BEGIN( yyextra->lastCopyArgContext );
                                        }
<CopyArgSharp>"("                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->argRoundCount=0;
                                          yyextra->lastCopyArgContext = YY_START;
                                          BEGIN( CopyArgRound );
                                        }
<CopyArgSharp>"<"                       {
                                          yyextra->argSharpCount++;
                                          //printf("yyextra->argSharpCount++=%d  copy\n",yyextra->argSharpCount);
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                        }
<CopyArgSharp>">"                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->argSharpCount--;
                                          if (yyextra->argSharpCount>0)
                                          {
                                            //printf("yyextra->argSharpCount--=%d copy\n",yyextra->argSharpCount);
                                          }
                                          else
                                          {
                                            BEGIN( ReadTempArgs );
                                            //printf("end of yyextra->argSharpCount\n");
                                          }
                                        }
<CopyArgString,CopyArgPHPString>\\.     {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<CopyArgString>\"                       {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          BEGIN( yyextra->lastCopyArgStringContext );
                                        }
<CopyArgPHPString>\'                    {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          BEGIN( yyextra->lastCopyArgStringContext );
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSquare,CopyArgSharp>{CHARLIT}     {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *yyextra->copyArgString+=yytext;
                                            yyextra->fullArgString+=yytext;
                                          }
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSquare,CopyArgSharp>\'     {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastCopyArgStringContext=YY_START;
                                            BEGIN(CopyArgPHPString);
                                          }
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSquare,CopyArgSharp>"<="|">="|"<=>" {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSquare,CopyArgSharp>\n  {
                                          lineCount(yyscanner);
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSquare,CopyArgSharp>{ID} {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSquare,CopyArgSharp>. {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                        }



  /*------------------------------------------------------------------------*/


<FuncRound>"("                          { yyextra->current->args += *yytext ;
                                          ++yyextra->roundCount ;
                                        }
<FuncRound>")"                          { yyextra->current->args += *yytext ;
                                          if ( yyextra->roundCount )
                                            --yyextra->roundCount ;
                                          else
                                            BEGIN( FuncQual ) ;
                                        }
  /*
<FuncQual>"#"                           { if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCPPContext = YY_START;
                                          BEGIN(SkipCPP);
                                        }
  */
<FuncQual>[{:;,]                        {
                                          if ( qstrcmp(yytext,";")==0 &&
                                               ((yyextra->insideJS || yyextra->insidePHP) &&
                                                !containsWord(yyextra->current->type,"function")) )
                                          {
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            unput(*yytext); BEGIN( SFunction );
                                          }
                                        }
<FuncQual>{BN}*"abstract"{BN}*          { // pure virtual member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->virt = Specifier::Pure;
                                          yyextra->current->args += " override ";
                                        }
<FuncQual,TrailingReturn>{BN}*"override"{BN}*          { // C++11 overridden virtual member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec.setOverride(true);
                                          yyextra->current->args += " override ";
                                          if (YY_START==TrailingReturn) yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"final"{BN}*             { // C++11 final method
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec.setFinal(true);
                                          yyextra->current->args += " final ";
                                          if (YY_START==TrailingReturn) yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(FuncQual);
                                        }
<FuncQual>{BN}*"sealed"{BN}*            { // sealed member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec.setSealed(true);
                                          yyextra->current->args += " sealed ";
                                        }
<FuncQual>{BN}*"new"{BN}*               { // new member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec.setNew(true);
                                          yyextra->current->args += " new ";
                                        }
<FuncQual>{BN}*"const"{BN}*             { // const member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " const ";
                                          yyextra->current->argList.setConstSpecifier(TRUE);
                                        }
<FuncQual>{BN}*"volatile"{BN}*          { // volatile member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " volatile ";
                                          yyextra->current->argList.setVolatileSpecifier(TRUE);
                                        }
<FuncQual>{BN}*"noexcept"{BN}*          { // noexcept qualifier
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " noexcept ";
                                          yyextra->current->spec.setNoExcept(true);
                                        }
<FuncQual>{BN}*"noexcept"{BN}*"("{B}*false{B}*")"{BN}* { // noexcept(false) expression
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " noexcept(false)";
                                        }
<FuncQual>{BN}*"noexcept"{BN}*"("       { // noexcept expression
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " noexcept(";
                                          yyextra->current->spec.setNoExcept(true);
                                          yyextra->lastRoundContext=FuncQual;
                                          yyextra->pCopyRoundString=&yyextra->current->args;
                                          yyextra->roundCount=0;
                                          BEGIN(CopyRound);
                                        }
<FuncQual>{BN}*"&"                      {
                                          yyextra->current->args += " &";
                                          yyextra->current->argList.setRefQualifier(RefQualifierType::LValue);
                                        }
<FuncQual>{BN}*"&&"                     {
                                          yyextra->current->args += " &&";
                                          yyextra->current->argList.setRefQualifier(RefQualifierType::RValue);
                                        }

<FuncQual,TrailingReturn>{BN}*"="{BN}*"0"{BN}*          { // pure virtual member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " = 0";
                                          yyextra->current->virt = Specifier::Pure;
                                          yyextra->current->argList.setPureSpecifier(TRUE);
                                          if (YY_START==TrailingReturn) yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"="{BN}*"delete"{BN}*     { // C++11 explicitly delete member
                                          lineCount(yyscanner);
                                          yyextra->current->args += " = delete";
                                          yyextra->current->spec.setDelete(true);
                                          yyextra->current->argList.setIsDeleted(TRUE);
                                          if (YY_START==TrailingReturn) yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"="{BN}*"default"{BN}*     { // C++11 explicitly defaulted constructor/assignment operator
                                          lineCount(yyscanner);
                                          yyextra->current->args += " = default";
                                          yyextra->current->spec.setDefault(true);
                                          if (YY_START==TrailingReturn) yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(FuncQual);
                                        }
<FuncQual>{BN}*"->"{BN}*                {
                                          lineCount(yyscanner);
                                          yyextra->current->argList.setTrailingReturnType(" -> ");
                                          yyextra->current->args += " -> ";
                                          yyextra->roundCount=0;
                                          BEGIN(TrailingReturn);
                                        }
<TrailingReturn>[{;]                    {
                                          if (yyextra->roundCount>0) REJECT;
                                          unput(*yytext);
                                          yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(FuncQual);
                                        }
<TrailingReturn>"requires"{BN}+         {
                                          if (yyextra->insideJava) REJECT;
                                          yyextra->requiresContext = FuncQual;
                                          yyextra->current->req+=' ';
                                          yyextra->current->argList.finishTrailingReturnType();
                                          BEGIN(RequiresClause);
                                        }
<TrailingReturn>"("                     {
                                          yyextra->roundCount++;
                                          yyextra->current->argList.appendTrailingReturnType(yytext);
                                          yyextra->current->args+=yytext;
                                        }
<TrailingReturn>")"                     {
                                          if (yyextra->roundCount>0)
                                          {
                                            yyextra->roundCount--;
                                          }
                                          else
                                          {
                                            warn(yyextra->fileName,yyextra->yyLineNr,
                                                 "Found ')' without opening '(' for trailing return type '{})...'",
                                                 yyextra->current->argList.trailingReturnType());
                                          }
                                          yyextra->current->argList.appendTrailingReturnType(yytext);
                                          yyextra->current->args+=yytext;
                                        }
<TrailingReturn>.                       {
                                          yyextra->current->argList.appendTrailingReturnType(yytext);
                                          yyextra->current->args+=yytext;
                                        }
<TrailingReturn>\n                      {
                                          lineCount(yyscanner);
                                          yyextra->current->argList.appendTrailingReturnType(yytext);
                                          yyextra->current->args+=' ';
                                        }
<FuncRound,FuncFunc>{BN}*","{BN}*       {
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += ", " ;
                                        }
<FuncQual,FuncRound,FuncFunc>{BN}+      {
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += ' ' ;
                                        }
<SFunction,FuncQual,FuncRound,FuncFunc>"#" { if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCPPContext = YY_START;
                                          BEGIN(SkipCPP);
                                        }
<FuncQual>"=>"                          {
                                          if (!yyextra->insideCS)
                                            REJECT;
                                          // Expression body function
                                          unput('=');
                                          BEGIN(SFunction);
                                        }
<FuncQual>"="                           {
                                          if (yyextra->insideCli && yyextra->current_root->section.isCompound())
                                          {
                                            BEGIN(CliOverride);
                                          }
                                          else
                                          {
                                            // typically an initialized function pointer
                                            yyextra->lastInitializerContext=YY_START;
                                            yyextra->sharpCount=0;
                                            yyextra->initBracketCount=0;
                                            yyextra->current->initializer.str(yytext);
                                            BEGIN(ReadInitializer);
                                          }
                                        }
<ReadExpressionBody>";"                 {
                                          if (!yyextra->current->sli.empty() && yyextra->previous) // copy special list items
                                          {
                                            yyextra->previous->sli = yyextra->current->sli;
                                            yyextra->current->sli.clear();
                                          }
                                          if (yyextra->previous) yyextra->previous->endBodyLine=yyextra->yyLineNr;
                                          BEGIN(FindMembers);
                                        }
<CliOverride>{ID}                       {
                                        }
<CliOverride>"{"                        {
                                          unput(*yytext);
                                          BEGIN(FuncQual);
                                        }
<CliOverride>\n                         {
                                          lineCount(yyscanner);
                                        }
<CliOverride>.                          {
                                        }
<FuncQual>{ID}                          {
                                          if (yyextra->insideCpp && qstrcmp(yytext,"requires")==0)
                                          {
                                            // c++20 trailing requires clause
                                            yyextra->requiresContext = YY_START;
                                            yyextra->current->req+=' ';
                                            BEGIN(RequiresClause);
                                          }
                                          else if (yyextra->insideCS && qstrcmp(yytext,"where")==0)
                                          {
                                            // type constraint for a method
                                            yyextra->current->typeConstr.clear();
                                            yyextra->current->typeConstr.push_back(Argument());
                                            yyextra->lastCSConstraint = YY_START;
                                            BEGIN( CSConstraintName );
                                          }
                                          else if (checkForKnRstyleC(yyscanner)) // K&R style C function
                                          {
                                            yyextra->current->args = yytext;
                                            yyextra->oldStyleArgType.clear();
                                            BEGIN(OldStyleArgs);
                                          }
                                          else
                                          {
                                            yyextra->current->args += yytext;
                                          }
                                        }
<OldStyleArgs>[,;]                      {
                                          QCString oldStyleArgPtr;
                                          QCString oldStyleArgName;
                                          splitKnRArg(yyscanner,oldStyleArgPtr,oldStyleArgName);
                                          QCString doc,brief;
                                          if (yyextra->current->doc!=yyextra->docBackup)
                                          {
                                            doc=yyextra->current->doc;
                                            yyextra->current->doc=yyextra->docBackup;
                                          }
                                          if (yyextra->current->brief!=yyextra->briefBackup)
                                          {
                                            brief=yyextra->current->brief;
                                            yyextra->current->brief=yyextra->briefBackup;
                                          }
                                          addKnRArgInfo(yyscanner,yyextra->oldStyleArgType+oldStyleArgPtr,
                                                        oldStyleArgName,brief,doc);
                                          yyextra->current->args.clear();
                                          if (*yytext==';') yyextra->oldStyleArgType.clear();
                                        }
<OldStyleArgs>{ID}                      { yyextra->current->args += yytext; }
<OldStyleArgs>"{"                       {
                                          if (yyextra->current->argList.empty())
                                          {
                                            yyextra->current->argList.setNoParameters(TRUE);
                                          }
                                          yyextra->current->args = argListToString(yyextra->current->argList);
                                          unput('{');
                                          BEGIN(FuncQual);
                                        }
<OldStyleArgs>.                         { yyextra->current->args += *yytext; }
<FuncQual,FuncRound,FuncFunc>\"         {
                                          if (yyextra->insideIDL && yyextra->insideCppQuote)
                                          {
                                            BEGIN(EndCppQuote);
                                          }
                                          else
                                          {
                                            yyextra->current->args += *yytext;
                                          }
                                        }
<FuncQual,FuncRound,FuncFunc>.          { yyextra->current->args += *yytext; }
<FuncQual>{BN}*"try:"                   |
<FuncQual>{BN}*"try"{BN}+               { /* try-function-block */
                                          yyextra->insideTryBlock=TRUE;
                                          lineCount(yyscanner);
                                          if (yytext[yyleng-1]==':')
                                          {
                                            unput(':');
                                            BEGIN( SFunction );
                                          }
                                        }
<FuncQual>{BN}*"throw"{BN}*"("          { // C++ style throw clause
                                          yyextra->current->exception = " throw (" ;
                                          yyextra->roundCount=0;
                                          lineCount(yyscanner) ;
                                          BEGIN( ExcpRound ) ;
                                        }
<FuncQual>{BN}*"raises"{BN}*"("         {
                                          yyextra->current->exception = " raises (" ;
                                          lineCount(yyscanner) ;
                                          yyextra->roundCount=0;
                                          BEGIN( ExcpRound ) ;
                                        }
<FuncQual>{BN}*"throws"{BN}+            { // Java style throw clause
                                          yyextra->current->exception = " throws " ;
                                          lineCount(yyscanner) ;
                                          BEGIN( ExcpList );
                                        }
<ExcpRound>"("                          { yyextra->current->exception += *yytext ;
                                          ++yyextra->roundCount ;
                                        }
<ExcpRound>")"                          { yyextra->current->exception += *yytext ;
                                          if ( yyextra->roundCount )
                                            --yyextra->roundCount ;
                                          else
                                            BEGIN( FuncQual ) ;
                                        }
<ExcpRound>.                            {
                                          yyextra->current->exception += *yytext;
                                        }
<ExcpList>"{"                           {
                                          unput('{'); BEGIN( FuncQual );
                                        }
<ExcpList>";"                           {
                                          unput(';'); BEGIN( FuncQual );
                                        }
<ExcpList>"\n"                          {
                                          yyextra->current->exception += ' ';
                                          lineCount(yyscanner);
                                        }
<ExcpList>.                             {
                                          yyextra->current->exception += *yytext;
                                        }
<SFunction>"("                           { yyextra->current->type += yyextra->current->name ;
                                          yyextra->current->name  = yyextra->current->args ;
                                          yyextra->current->args  = yytext ;
                                          yyextra->roundCount=0;
                                          BEGIN( FuncRound ) ;
                                        }
<SFunction>":"                           {
                                          if (!yyextra->insidePHP) BEGIN(SkipInits);
                                        }
<SFunction>[=;{,]                       {
                                          yyextra->current->name=removeRedundantWhiteSpace(yyextra->current->name);
                                          yyextra->current->type=removeRedundantWhiteSpace(yyextra->current->type);
                                          yyextra->current->args=removeRedundantWhiteSpace(yyextra->current->args);
                                          yyextra->current->fileName = yyextra->fileName;
                                          yyextra->current->startLine = yyextra->yyBegLineNr;
                                          yyextra->current->startColumn = yyextra->yyBegColNr;
                                          static const reg::Ex re(R"(\([^)]*[*&][^)]*\))"); // e.g. (...*...)
                                          reg::Match match;
                                          std::string type = yyextra->current->type.str();
                                          int ti=-1;
                                          if (reg::search(type,match,re))
                                          {
                                            ti = (int)match.position();
                                          }
                                          if (ti!=-1)
                                          {
                                           int di = yyextra->current->type.find("decltype(");
                                           if (di!=-1 && di<ti) // decltype(...(...*...) -> normal return type
                                           {
                                             ti=-1;
                                           }
                                          }
                                          int ts=yyextra->current->type.find('<');
                                          int te=yyextra->current->type.findRev('>');

                                          // bug677315: A<int(void *, char *)> get(); is not a function pointer
                                          bool startsWithTypedef = yyextra->current->type.startsWith("typedef ");
                                          bool isFunction = ti==-1 || // not a (...*...) pattern
                                                            (ts!=-1 && ts<te && ts<ti && ti<te); // (...*...) is part of a template argument list
                                          bool isVariable = !yyextra->current->type.isEmpty() &&
                                                (!isFunction || startsWithTypedef);

                                          //printf("type=%s ts=%d te=%d ti=%d isFunction=%d\n",
                                          //    qPrint(yyextra->current->type),ts,te,ti,isFunction);

                                          if (*yytext!=';' || yyextra->current_root->section.isCompound())
                                          {
                                            if (isVariable)
                                            {
                                              //printf("Scanner.l: found in class variable: '%s' '%s' '%s'\n", qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                              if (yyextra->isTypedef && !startsWithTypedef)
                                              {
                                                yyextra->current->type.prepend("typedef ");
                                              }
                                              yyextra->current->section = EntryType::makeVariable() ;
                                            }
                                            else
                                            {
                                              //printf("Scanner.l: found in class function: '%s' '%s' '%s'\n", qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                              yyextra->current->section = EntryType::makeFunction() ;
                                              yyextra->current->proto = *yytext==';';
                                            }
                                          }
                                          else // a global function prototype or function variable
                                          {
                                            //printf("Scanner.l: prototype? type='%s' name='%s' args='%s'\n",qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                            if (isVariable)
                                            {
                                              if (yyextra->isTypedef && !startsWithTypedef)
                                              {
                                                yyextra->current->type.prepend("typedef ");
                                              }
                                              //printf("Scanner.l: found function variable!\n");
                                              yyextra->current->section = EntryType::makeVariable();
                                            }
                                            else
                                            {
                                              //printf("Scanner.l: found prototype\n");
                                              yyextra->current->section = EntryType::makeFunction();
                                              yyextra->current->proto = TRUE;
                                            }
                                          }
                                          //printf("Adding entry '%s'\n",qPrint(yyextra->current->name));
                                          if ( yyextra->insidePHP)
                                          {
                                            if (findAndRemoveWord(yyextra->current->type,"final"))
                                            {
                                              yyextra->current->spec.setFinal(true);
                                            }
                                            if (findAndRemoveWord(yyextra->current->type,"abstract"))
                                            {
                                              yyextra->current->spec.setAbstract(true);
                                            }
                                          }
                                          if ( yyextra->insidePHP && !containsWord(yyextra->current->type,"function"))
                                          {
                                            initEntry(yyscanner);
                                            if ( *yytext == '{' )
                                            {
                                              yyextra->lastCurlyContext = FindMembers;
                                              yyextra->curlyCount=0;
                                              BEGIN( SkipCurly );
                                            }
                                            else
                                            {
                                              BEGIN( FindMembers );
                                            }
                                          }
                                          else
                                          {
                                            if ( yyextra->insidePHP)
                                            {
                                              findAndRemoveWord(yyextra->current->type,"function");
                                            }
                                            yyextra->previous = yyextra->current;
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                            // Objective C 2.0: Required/Optional section
                                            if (yyextra->previous->spec.isOptional() || yyextra->previous->spec.isRequired())
                                            {
                                              yyextra->current->spec.setOptional(true).setRequired(true);
                                            }
                                            yyextra->lastCurlyContext = FindMembers;
                                            if ( *yytext == ',' )
                                            {
                                              yyextra->current->type = stripFuncPtr(yyextra->previous->type);
                                            }
                                            if ( *yytext == '{' )
                                            {
                                              if ( !yyextra->insidePHP && yyextra->current_root->section.isCompound() )
                                              {
                                                yyextra->previous->spec.setInline(true);
                                              }
                                              yyextra->curlyCount=0;
                                              BEGIN( SkipCurly ) ;
                                            }
                                            else if ( *yytext == '=' ) // C# Expression Body
                                            {
                                              yyextra->previous->spec.setInline(true);
                                              yyextra->curlyCount=0;
                                              BEGIN( ReadExpressionBody );
                                            }
                                            else
                                            {
                                              if (!yyextra->previous->section.isVariable())
                                                yyextra->previous->bodyLine=-1; // a function/member declaration
                                              BEGIN( FindMembers ) ;
                                            }
                                          }
                                        }
<SkipInits>">"{BN}*"{"                  { // C++11 style initializer (see bug 790788)
                                          lineCount(yyscanner);
                                          yyextra->curlyCount=1;
                                          BEGIN(SkipC11Inits);
                                        }
<SkipInits>{ID}{BN}*"{"                 { // C++11 style initializer (see bug 688647)
                                          lineCount(yyscanner);
                                          yyextra->curlyCount=1;
                                          BEGIN(SkipC11Inits);
                                        }
<SkipC11Inits>"{"                       {
                                          ++yyextra->curlyCount;
                                        }
<SkipC11Inits>"}"                       {
                                          if ( --yyextra->curlyCount<=0 )
                                          {
                                            BEGIN(SkipInits);
                                          }
                                        }
<SkipC11Attribute>"]]"                 {
                                          BEGIN(yyextra->lastC11AttributeContext);
                                        }
<SkipInits>"{"                          { // C++11 style initializer
                                          unput('{');
                                          BEGIN( SFunction );
                                        }
<SkipCurly>"{"                          {
                                          //addToBody(yytext);
                                          ++yyextra->curlyCount ;
                                        }
<SkipCurly>"}"/{BN}*{DCOMM}"<!--" | /* see bug710917 */)
<SkipCurly>"}"                          {
                                          //addToBody(yytext);
                                          if( yyextra->curlyCount )
                                          {
                                            --yyextra->curlyCount ;
                                          }
                                          else
                                          {
                                            if (!yyextra->current->sli.empty() && yyextra->previous) // copy special list items
                                            {
                                              yyextra->previous->sli = yyextra->current->sli;
                                              yyextra->current->sli.clear();
                                            }
                                            if (yyextra->previous) yyextra->previous->endBodyLine=yyextra->yyLineNr;
                                            BEGIN( yyextra->lastCurlyContext ) ;
                                          }
                                        }
<SkipCurly>"}"{BN}*{DCOMM}"<" {
                                          lineCount(yyscanner);
                                          if ( yyextra->curlyCount )
                                          {
                                            //addToBody(yytext);
                                            --yyextra->curlyCount ;
                                          }
                                          else
                                          {
                                            yyextra->current->endBodyLine=yyextra->yyLineNr;
                                            yyextra->tempEntry = yyextra->current; // temporarily switch to the previous entry
                                            yyextra->current   = yyextra->previous;

                                            yyextra->docBlockContext   = SkipCurlyEndDoc;
                                            yyextra->docBlockInBody    = FALSE;
                                            yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                                ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                            yyextra->docBlock.str(std::string());
                                            yyextra->docBlockTerm = '}';
                                            if (yytext[yyleng-3]=='/')
                                            {
                                              startCommentBlock(yyscanner,TRUE);
                                              BEGIN( DocLine );
                                            }
                                            else
                                            {
                                              startCommentBlock(yyscanner,FALSE);
                                              BEGIN( DocBlock );
                                            }
                                          }
                                        }
<SkipCurlyEndDoc>"}"{BN}*{DCOMM}"<" { // desc is followed by another one
                                          yyextra->docBlockContext   = SkipCurlyEndDoc;
                                          yyextra->docBlockInBody    = FALSE;
                                          yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                              ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                          yyextra->docBlock.str(std::string());
                                          yyextra->docBlockTerm = '}';
                                          if (yytext[yyleng-3]=='/')
                                          {
                                            startCommentBlock(yyscanner,TRUE);
                                            BEGIN( DocLine );
                                          }
                                          else
                                          {
                                            startCommentBlock(yyscanner,FALSE);
                                            BEGIN( DocBlock );
                                          }
                                        }
<SkipCurlyEndDoc>"}"                    {
                                          //addToBody("}");
                                          if (yyextra->tempEntry) // we can only switch back to yyextra->current if no new item was created
                                          {
                                            yyextra->current = yyextra->tempEntry;
                                            yyextra->tempEntry.reset();
                                          }
                                          BEGIN( yyextra->lastCurlyContext );
                                        }
<SkipCurly,ReadExpressionBody>\"        {
                                          //addToBody(yytext);
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN( SkipString );
                                        }
<SkipCurly>^{B}*"#"                     {
                                          if (yyextra->insidePHP)
                                            REJECT;
                                          //addToBody(yytext);
                                          BEGIN( SkipCurlyCpp );
                                        }
<SkipCurly,SkipC11Inits,SkipInits,SkipC11Attribute,ReadExpressionBody>\n   {
                                          lineCount(yyscanner);
                                          //addToBody(yytext);
                                        }
<SkipCurly,SkipCurlyCpp,ReadInitializer,ReadInitializerPtr>"<<<"  {
                                          if (!yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->lastHereDocContext = YY_START;
                                            BEGIN(HereDoc);
                                          }
                                        }
<SkipCurly,SkipCurlyCpp>{B}*{RAWBEGIN}  {
                                          yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
                                          yyextra->lastRawStringContext = YY_START;
                                          yyextra->dummyRawString.clear();
                                          yyextra->pCopyRawString = &yyextra->dummyRawString;
                                          *yyextra->pCopyRawString += yytext;
                                          BEGIN(RawString);
                                        }
<SkipCurly,SkipCurlyCpp>[^\n#"R'@\\/{}<\$]+ {
                                          lineCount(yyscanner);  // for yyextra->column updates
                                          //addToBody(yytext);
                                        }
<SkipCurly,SkipCurlyCpp>"\$"            {}
<SkipCurlyCpp>\n                        {
                                          //addToBody(yytext);
                                          lineCount(yyscanner);
                                          yyextra->lastCurlyContext = FindMembers;
                                          BEGIN( SkipCurly );
                                        }
<SkipCurlyCpp>\\[\r]*"\n"[\r]*          {
                                          //addToBody(yytext);
                                          lineCount(yyscanner);
                                        }
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute,ReadExpressionBody>{CCS}    {
                                          //addToBody(yytext);
                                          yyextra->lastCContext = YY_START;
                                          BEGIN(SkipComment);
                                        }
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute,ReadExpressionBody>{CPPC}  {
                                          //addToBody(yytext);
                                          yyextra->lastCContext = YY_START;
                                          BEGIN(SkipCxxComment);
                                        }
<SkipInits,SkipC11Inits,SkipC11Attribute>"("             {
                                          yyextra->roundCount=0;
                                          yyextra->lastSkipRoundContext=YY_START;
                                          BEGIN(SkipRound);
                                        }
<SkipInits,SkipC11Inits,SkipC11Attribute>\"             {
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN( SkipString );
                                        }
<SkipInits>;                            {
                                          warn(yyextra->fileName,yyextra->yyLineNr,
                                              "Found ';' while parsing initializer list! "
                                              "(doxygen could be confused by a macro call without semicolon)"
                                             );
                                          BEGIN( FindMembers );
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp>"#"   {
                                          if (!yyextra->insidePHP)
                                            REJECT;
                                          //addToBody(yytext);
                                          yyextra->lastCContext = YY_START;
                                          BEGIN(SkipCxxComment);
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp,ReadExpressionBody>@\"   {
                                          if (!yyextra->insideCS) REJECT;
                                          // C# verbatim string
                                          // we want to discard the string, due to reuse of states we need a dummy stream
                                          yyextra->lastSkipVerbStringContext=YY_START;
                                          yyextra->pSkipVerbString=&yyextra->dummyTextStream;
                                          yyextra->dummyTextStream.clear(); // remove old data so it won't grow too much
                                          BEGIN(SkipVerbString);
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp,ReadExpressionBody>{CHARLIT}     {
                                          if (yyextra->insidePHP) REJECT;
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp>\'    {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(SkipPHPString);
                                          }
                                        }
<SkipC11Attribute>{ID}                  {
                                          if (QCString(yytext)=="nodiscard")
                                          {
                                            yyextra->current->spec.setNoDiscard(true);
                                          }
                                        }
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute,ReadExpressionBody>.       { }
<SkipString,SkipPHPString>\\.           { }
<SkipString>\"                          {
                                          BEGIN( yyextra->lastStringContext );
                                        }
<SkipPHPString>\'                       {
                                          BEGIN( yyextra->lastStringContext );
                                        }
<SkipString,SkipPHPString>{CCS}|{CCE}|{CPPC} { }
<SkipString,SkipPHPString>\n            {
                                          lineCount(yyscanner);
                                        }
<SkipString>"[["                        { }
<SkipString,SkipPHPString>.             { }
<CompoundName>":"                       { // for "class : public base {} var;" construct, see bug 608359
                                          unput(':');
                                          BEGIN(ClassVar);
                                        }
<CompoundName>";"                       {
                                          yyextra->current->section = EntryType::makeEmpty() ;
                                          yyextra->current->type.clear() ;
                                          yyextra->current->name.clear() ;
                                          yyextra->current->args.clear() ;
                                          yyextra->current->argList.clear();
                                          BEGIN( FindMembers ) ;
                                        }
<Bases>";"                      {
                                          if (yyextra->insideIDL && (yyextra->current->spec.isSingleton() || yyextra->current->spec.isService()))
                                          {
                                            // in UNO IDL a service or singleton may be defined
                                            // completely like this: "service Foo : XFoo;"
                                            if (!yyextra->current->name.isEmpty() && !yyextra->current_root->name.isEmpty())
                                            {
                                              prependScope(yyscanner);
                                            }
                                            yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                            // there can be only one base class here
                                            if (!yyextra->baseName.isEmpty())
                                            {
                                              yyextra->current->extends.emplace_back(
                                                yyextra->baseName,Protection::Public,Specifier::Normal);
                                              yyextra->baseName.clear();
                                            }
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          else
                                          {
                                            yyextra->current->section = EntryType::makeEmpty() ;
                                            yyextra->current->type.clear() ;
                                            yyextra->current->name.clear() ;
                                            yyextra->current->args.clear() ;
                                            yyextra->current->argList.clear();
                                          }
                                          BEGIN( FindMembers ) ;
                                        }
<CompoundName>{SCOPENAME}/{BN}*"<"      {
                                          yyextra->sharpCount = 0;
                                          yyextra->current->name = yytext ;
                                          storeClangId(yyscanner,yytext);
                                          if (yyextra->current->spec.isProtocol())
                                          {
                                            yyextra->current->name+="-p";
                                          }
                                          lineCount(yyscanner);
                                          yyextra->lastClassTemplSpecContext = ClassVar;
                                          if (yyextra->insideObjC) // protocol list
                                          {
                                            BEGIN( ObjCProtocolList );
                                          }
                                          else if (yyextra->insideCS) // C# generic class
                                          {
                                            //yyextra->current->name+="-g";
                                            BEGIN( CSGeneric );
                                          }
                                          else // C++ template specialization
                                          {
                                            yyextra->roundCount=0;
                                            BEGIN( ClassTemplSpec );
                                          }
                                        }
<CSGeneric>"<"                          {
                                          ArgumentList al;
                                          // check bug 612858 before enabling the next line
                                          //yyextra->current->spec |= Entry::Template;
                                          yyextra->current->tArgLists.push_back(al);
                                          yyextra->currentArgumentList = &yyextra->current->tArgLists.back();
                                          yyextra->templateStr="<";
                                          yyextra->current->name += "<";
                                          yyextra->fullArgString = yyextra->templateStr;
                                          yyextra->copyArgString = &yyextra->current->name;
                                          //yyextra->copyArgString = &yyextra->templateStr;
                                          yyextra->currentArgumentContext = ClassVar;
                                          BEGIN( ReadTempArgs );
                                        }
<ObjCProtocolList>"<"                   {
                                          yyextra->insideProtocolList=TRUE;
                                          BEGIN( Bases );
                                        }
<ClassTemplSpec>">"({BN}*"::"{BN}*{SCOPENAME})? {
                                          yyextra->current->name += yytext;
                                          lineCount(yyscanner);
                                          if (yyextra->roundCount==0 && --yyextra->sharpCount<=0)
                                          {
                                            yyextra->current->name = removeRedundantWhiteSpace(yyextra->current->name);
                                            if (yyextra->current->spec.isProtocol())
                                            { // Objective-C protocol
                                              unput('{'); // fake start of body
                                              BEGIN( ClassVar );
                                            }
                                            else
                                            {
                                              BEGIN( yyextra->lastClassTemplSpecContext );
                                            }
                                          }
                                        }
<ClassTemplSpec>"<"                     {
                                          yyextra->current->name += yytext;
                                          if (yyextra->roundCount==0) yyextra->sharpCount++;
                                        }
<ClassTemplSpec>.                       {
                                          yyextra->current->name += yytext;
                                        }
<CompoundName>({SCOPENAME}|{CSSCOPENAME}){BN}*";"       { // forward declaration?
                                          if (yyextra->insideCS && yyextra->current->type == "namespace")
                                          {
                                            // file scoped CSharp namespace
                                            lineCount(yyscanner);
                                            yyextra->current->name = substitute(yytext,".","::");
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                            yyextra->fakeNS++;
                                            unput('{'); // fake start of body
                                            BEGIN( ClassVar );
                                          }
                                          else if (!yyextra->current->tArgLists.empty())
                                          {
                                            // found a forward template declaration, this has
                                            // a purpose of its own
                                            yyextra->current->name = yytext;
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                            storeClangId(yyscanner,yyextra->current->name.data());
                                            //printf("template class declaration for %s!\n",qPrint(yyextra->current->name));
                                            QCString rn = yyextra->current_root->name;
                                            //printf("cn='%s' rn='%s' yyextra->isTypedef=%d\n",qPrint(cn),qPrint(rn),yyextra->isTypedef);
                                            if (!yyextra->current->name.isEmpty() && !rn.isEmpty())
                                            {
                                              prependScope(yyscanner);
                                            }
                                            yyextra->current->spec.setForwardDecl(true);
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          }
                                          else if (yyextra->insideIDL &&
                                                   (((yyextra->current_root->spec.isInterface() || yyextra->current_root->spec.isService()) &&
                                                      yyextra->current->spec.isInterface()) ||
                                                    ((yyextra->current_root->spec.isService() || yyextra->current_root->spec.isSingleton()) &&
                                                      yyextra->current->spec.isService())
                                                   )
                                                  )
                                          {
                                            // interface yyextra->inside of UNO IDL service or interface
                                            // service yyextra->inside of UNO IDL service or singleton
                                            // there may be documentation on the member,
                                            // so do not throw it away...
                                            yyextra->current->name = yytext;
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                            yyextra->current->section = yyextra->current->spec.isInterface() ? EntryType::makeExportedInterface()
                                                                                                             : EntryType::makeIncludedService();
//                                          yyextra->current->section = EntryType::makeMemberDoc();
                                            yyextra->current->spec.setInterface(false).setService(false);
                                                     // FIXME: horrible: Interface == Gettable, so need to clear it - actually we're mixing values from 
                                                     // different enums in this case...
                                                     // granted only Optional and Interface are actually valid in this context but urgh...
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          }

                                          if (!(yyextra->insideCS && yyextra->current->type == "namespace"))
                                          {
                                            unput(';');
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                            if (yyextra->insideObjC) // see bug746361
                                            {
                                              yyextra->language = yyextra->current->lang = SrcLangExt::Cpp;
                                              yyextra->insideObjC = FALSE;
                                              }
                                            if (yyextra->isTypedef) // typedef of a class, put typedef keyword back
                                            {
                                              yyextra->current->type.prepend("typedef");
                                            }
                                            BEGIN( FindMembers );
                                          }
                                        }
<CompoundName>{SCOPENAME}/{BN}*"("      {
                                          yyextra->current->name = yytext ;
                                          storeClangId(yyscanner,yytext);
                                          lineCount(yyscanner);
                                          if (yyextra->insideCpp && yyextra->current->name=="alignas") // C++11
                                          {
                                            yyextra->lastAlignAsContext = YY_START;
                                            BEGIN( AlignAs );
                                          }
                                          else
                                          {
                                            if (yyextra->current->spec.isProtocol())
                                            {
                                              yyextra->current->name += "-p";
                                            }
                                            BEGIN( ClassVar );
                                          }
                                        }
<AlignAs>"("                            { yyextra->roundCount=0;
                                          BEGIN( AlignAsEnd );
                                        }
<AlignAs>\n                             { lineCount(yyscanner); }
<AlignAs>.
<AlignAsEnd>"("                         { yyextra->roundCount++; }
<AlignAsEnd>")"                         { if (--yyextra->roundCount<0)
                                          {
                                            BEGIN( yyextra->lastAlignAsContext );
                                          }
                                        }
<AlignAsEnd>\n                          { lineCount(yyscanner); }
<AlignAsEnd>.
<ConceptName>{ID}                       {
                                          yyextra->current->name = yytext ;
                                          storeClangId(yyscanner,yytext);
                                        }
<ConceptName>"="                        {
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->initializer.str(std::string());
                                          yyextra->lastInitializerContext = FindMembers;
                                          yyextra->sharpCount=0;
                                          yyextra->initBracketCount=0;
                                          BEGIN(ReadInitializer);
                                        }
<CompoundName>{SCOPENAME}/{BN}*","      {  // multiple forward declarations on one line
                                           // e.g. @protocol A,B;
                                          yyextra->current->reset();
                                          initEntry(yyscanner);
                                        }
<CompoundName>{SCOPENAME}               {
                                          yyextra->current->name = yytext ;
                                          storeClangId(yyscanner,yytext);
                                          lineCount(yyscanner);
                                          if (yyextra->current->spec.isProtocol())
                                          {
                                            yyextra->current->name += "-p";
                                          }
                                          if (yyextra->current->spec.isProtocol() || yyextra->current->section.isObjcImpl())
                                          {
                                            unput('{'); // fake start of body
                                          }
                                          BEGIN( ClassVar );
                                        }
<CompoundName>{CSSCOPENAME}             { // C# style scope
                                          yyextra->current->name = substitute(yytext,".","::");
                                          lineCount(yyscanner);
                                          BEGIN( ClassVar );
                                        }
<ClassVar>{SCOPENAME}{BNopt}/"("        {
                                          if (yyextra->insideIDL && literal_at(yytext,"switch") && !isId(yytext[6]))
                                          {
                                            // Corba IDL style union
                                            yyextra->roundCount=0;
                                            BEGIN(SkipUnionSwitch);
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->yyBegColNr=yyextra->yyColNr;
                                            yyextra->yyBegLineNr=yyextra->yyLineNr;
                                            yyextra->current->name = yytext;
                                            yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                            storeClangId(yyscanner,yyextra->current->name.data());
                                            lineCount(yyscanner);
                                            BEGIN( FindMembers );
                                          }
                                        }
<ClassVar>","                           {
                                          if (yyextra->isTypedef)
                                          {
                                            // multiple types in one typedef
                                            unput(',');
                                            yyextra->current->type.prepend("typedef ");
                                            BEGIN(FindMembers);
                                          }
                                          else
                                          {
                                            // Multiple class forward declaration
                                          }
                                        }
<ClassVar>("sealed"|"abstract")/{BN}*(":"|"{") {
                                          if (yyextra->insideCli)
                                          {
                                            if (yytext[0]=='s') // sealed
                                              yyextra->current->spec.setSealedClass(true);
                                            else // abstract
                                              yyextra->current->spec.setAbstractClass(true);
                                            BEGIN( ClassVar );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<ClassVar>({ID}{BN}*"::"{BN}*)+{ID}     {
                                          yyextra->yyBegColNr=yyextra->yyColNr;
                                          yyextra->yyBegLineNr=yyextra->yyLineNr;
                                          storeClangId(yyscanner,yytext);
                                          lineCount(yyscanner);
                                          if (yyextra->current->section.isEnum())
                                          { // found "enum a N::b" -> variable
                                            yyextra->current->section = EntryType::makeVariable() ;
                                          }
                                          yyextra->current->type += ' ' ;
                                          yyextra->current->type += yyextra->current->name ;
                                          yyextra->current->name = QCString(yytext).simplifyWhiteSpace();

                                          if (nameIsOperator(yyextra->current->name))
                                          {
                                            BEGIN( Operator );
                                          }
                                        }
<ClassVar>{ID}                          {
                                          yyextra->yyBegColNr=yyextra->yyColNr;
                                          yyextra->yyBegLineNr=yyextra->yyLineNr;
                                          storeClangId(yyscanner,yytext);
                                          if (yyextra->insideIDL && qstrcmp(yytext,"switch")==0)
                                          {
                                            // Corba IDL style union
                                            yyextra->roundCount=0;
                                            BEGIN(SkipUnionSwitch);
                                          }
                                          else if ((yyextra->insideJava || yyextra->insidePHP || yyextra->insideJS || yyextra->insideSlice) && (qstrcmp(yytext,"implements")==0 || qstrcmp(yytext,"extends")==0))
                                          {
                                            yyextra->current->type.clear();
                                            yyextra->baseProt = Protection::Public;
                                            yyextra->baseVirt = Specifier::Normal;
                                            yyextra->baseName.clear();
                                            BEGIN( BasesProt ) ;
                                          }
                                          else if (yyextra->insideCS && qstrcmp(yytext,"where")==0) // C# type constraint
                                          {
                                            yyextra->current->typeConstr.clear();
                                            yyextra->current->typeConstr.push_back(Argument());
                                            yyextra->lastCSConstraint = YY_START;
                                            BEGIN( CSConstraintName );
                                          }
                                          else if (yyextra->insideCli &&  qstrcmp(yytext,"abstract")==0)
                                          {
                                            yyextra->current->spec.setAbstract(true);
                                          }
                                          else if (yyextra->insideCli &&  qstrcmp(yytext,"sealed")==0)
                                          {
                                            yyextra->current->spec.setSealed(true);
                                          }
                                          else if (qstrcmp(yytext,"final")==0)
                                          {
                                            yyextra->current->spec.setFinal(true);
                                          }
                                          else
                                          {
                                            if (yyextra->current->section.isEnum())
                                            { // found "enum a b" -> variable
                                              yyextra->current->section = EntryType::makeVariable() ;
                                            }
                                            yyextra->current->type += ' ' ;
                                            yyextra->current->type += yyextra->current->name ;
                                            yyextra->current->name = yytext ;

                                            if (nameIsOperator(yyextra->current->name))
                                            {
                                              BEGIN( Operator );
                                            }
                                          }
                                        }
<ClassVar>[(\[]                         {
                                          if (yyextra->insideObjC && *yytext=='(') // class category
                                          {
                                            yyextra->current->name+='(';
                                            yyextra->current->spec.setCategory(true);
                                            BEGIN( ClassCategory );
                                          }
                                          else if (yyextra->insideCS && *yytext=='(') // C# 12 primary constructor
                                          {
                                            yyextra->current->args=yytext;
                                            yyextra->currentArgumentContext = ClassVar;
                                            yyextra->fullArgString = yyextra->current->args;
                                            yyextra->copyArgString = &yyextra->current->args;
                                            BEGIN( ReadFuncArgType ) ;
                                          }
                                          else
                                          {
                                            // probably a function anyway
                                            unput(*yytext);
                                            BEGIN( FindMembers );
                                          }
                                        }
<CSConstraintType,CSConstraintName>{CCS}{CCE} { /* empty comment */ }
<CSConstraintType,CSConstraintName>({CCS}[*!]|{CPPC}[/!])("<"?)    { // special comment
                                          yyextra->fullArgString.clear();
                                          yyextra->lastCopyArgChar='#'; // end marker
                                          yyextra->lastCommentInArgContext=YY_START;
                                          if (yytext[1]=='/')
                                            BEGIN( CopyArgCommentLine );
                                          else
                                            BEGIN( CopyArgComment );
                                        }
<CSConstraintType,CSConstraintName>"#"  { // artificially inserted token to signal end of comment block
                                          yyextra->current->typeConstr.back().docs = yyextra->fullArgString;
                                        }
<CSConstraintType>"=>"                  { // end of type constraint reached
                                          // parse documentation of the constraints
                                          handleParametersCommentBlocks(yyscanner,yyextra->current->typeConstr);
                                          unput('>');
                                          unput('=');
                                          BEGIN( yyextra->lastCSConstraint );
                                        }
<CSConstraintType>"{"                   { // end of type constraint reached
                                          // parse documentation of the constraints
                                          handleParametersCommentBlocks(yyscanner,yyextra->current->typeConstr);
                                          unput('{');
                                          BEGIN( yyextra->lastCSConstraint );
                                        }
<CSConstraintType,CSConstraintName>";"  {
                                          handleParametersCommentBlocks(yyscanner,yyextra->current->typeConstr);
                                          unput(';');
                                          BEGIN( yyextra->lastCSConstraint );
                                        }
<CSConstraintName>":"                   {
                                          BEGIN( CSConstraintType );
                                        }
<CSConstraintName>{ID}                  {
                                          // parameter name
                                          yyextra->current->typeConstr.back().name=yytext;
                                        }
<CSConstraintType>"where"               { // another constraint for a different param
                                          yyextra->current->typeConstr.push_back(Argument());
                                          BEGIN( CSConstraintName );
                                        }
<CSConstraintType>({ID}".")*{ID}("<"{ID}">")?("()")?  {
                                          if (yyextra->current->typeConstr.back().type.isEmpty())
                                              // first type constraint for this parameter
                                          {
                                            yyextra->current->typeConstr.back().type=yytext;
                                          }
                                          else // new type constraint for same parameter
                                          {
                                            QCString name = yyextra->current->typeConstr.back().name;
                                            yyextra->current->typeConstr.push_back(Argument());
                                            yyextra->current->typeConstr.back().name=name;
                                            yyextra->current->typeConstr.back().type=yytext;
                                          }
                                        }
<CSConstraintName,CSConstraintType>\n   {
                                          lineCount(yyscanner);
                                        }
<CSConstraintName,CSConstraintType>.    {
                                        }
<ClassCategory>{ID}                     {
                                          yyextra->current->name+=yytext;
                                        }
<ClassCategory>")"/{BN}*"{"             {
                                          yyextra->current->name+=')';
                                          BEGIN( ClassVar );
                                        }
<ClassCategory>")"/{BN}*"<"             {
                                          yyextra->current->name+=')';
                                          BEGIN( ObjCProtocolList );
                                        }
<ClassCategory>")"                      {
                                          yyextra->current->name+=')';
                                          if (yyextra->current->spec.isProtocol() || yyextra->current->section.isObjcImpl())
                                          {
                                            unput('{'); // fake start of body
                                          }
                                          else // category has no variables so push back an empty body
                                          {
                                            unput('}');
                                            unput('{');
                                          }
                                          BEGIN( ClassVar );
                                        }
<ClassVar>":"                           {
                                          if (yyextra->current->section.isVariable()) // enum A B:2, see bug 748208
                                          {
                                            yyextra->current->bitfields+=":";
                                            yyextra->current->args.clear();
                                            BEGIN(BitFields);
                                          }
                                          else if (yyextra->current->section.isEnum()) // enum E:2, see bug 313527,
                                                                                 // or C++11 style enum: 'E : unsigned int {...}'
                                          {
                                            yyextra->current->args.clear();
                                            BEGIN(EnumBaseType);
                                          }
                                          else
                                          {
                                            yyextra->current->type.clear();
                                            if (yyextra->current->spec.isInterface() ||
                                                yyextra->current->spec.isStruct()    ||
                                                yyextra->current->spec.isRef()       ||
                                                yyextra->current->spec.isValue()     ||
                                                yyextra->insidePHP || yyextra->insideCS || yyextra->insideD || yyextra->insideObjC || yyextra->insideIDL
                                               )
                                            {
                                              yyextra->baseProt = Protection::Public;
                                            }
                                            else
                                            {
                                              yyextra->baseProt = Protection::Private;
                                            }
                                            yyextra->baseVirt = Specifier::Normal;
                                            yyextra->baseName.clear();
                                            BEGIN( BasesProt ) ;
                                          }
                                        }
<ClassVar>[;=*&]                        {
                                          if (yyextra->isTypedef) // typedef of a class, put typedef keyword back
                                          {
                                            yyextra->current->type.prepend("typedef");
                                          }
                                          if ((yytext[0]=='*' || yytext[0]=='&') && yyextra->current->section.isEnum())
                                          { // found "enum a *b" -> variable
                                            yyextra->current->section = EntryType::makeVariable() ;
                                          }
                                          if (yytext[0]==';' && yyextra->current->section.isEnum())
                                          {
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                          }
                                          else
                                          {
                                            unput(*yytext);
                                          }
                                          BEGIN( FindMembers );
                                        }
<Bases,ClassVar>{CPPC}"/"/[^/]              {
                                          if (!yyextra->insideObjC)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->program << yytext;
                                            yyextra->current->fileName = yyextra->fileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( ReadBodyIntf );
                                          }
                                        }
<Bases,ClassVar>({CPPC}{B}*)?{CCS}"*"/{NCOMM}  |
<Bases,ClassVar>({CPPC}{B}*)?{CCS}"!"        |
<Bases,ClassVar>{CPPC}"!"                   |
<Bases,ClassVar>[\-+]{BN}*              {
                                          if (!yyextra->insideObjC)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->program << yytext;
                                            yyextra->current->fileName = yyextra->fileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( ReadBodyIntf );
                                          }
                                        }
<CompoundName,ClassVar>{B}*"{"{B}*      {
                                          yyextra->current->program.str(std::string());
                                          yyextra->current->fileName = yyextra->fileName ;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = removeRedundantWhiteSpace(yyextra->current->name);
                                          if (yyextra->current->name.isEmpty() && !yyextra->isTypedef) // anonymous compound
                                          {
                                            if (yyextra->current->section.isNamespace()) // allow reopening of anonymous namespaces
                                            {
                                              if (Config_getBool(EXTRACT_ANON_NSPACES)) // use visible name
                                              {
                                                yyextra->current->name="anonymous_namespace{"+stripPath(yyextra->current->fileName)+"}";
                                              }
                                              else // use invisible name
                                              {
                                                yyextra->current->name = generateAnonymousAnchor(yyextra->fileName,yyextra->anonNSCount);
                                              }
                                            }
                                            else
                                            {
                                              yyextra->current->name = generateAnonymousAnchor(yyextra->fileName,yyextra->anonCount++);
                                            }
                                          }
                                          yyextra->curlyCount=0;
                                          if (yyextra->current_root && // not a nested struct yyextra->inside an @interface section
                                              !yyextra->current_root->spec.isInterface() &&
                                              (yyextra->current->spec.isInterface() ||
                                               yyextra->current->spec.isProtocol()  ||
                                               yyextra->current->spec.isCategory()  ||
                                               yyextra->current->section.isObjcImpl()
                                              ) &&
                                              yyextra->insideObjC
                                             )
                                          { // ObjC body that ends with @end
                                            BEGIN( ReadBodyIntf );
                                          }
                                          else if (yyextra->current->section.isNamespace())
                                          { // namespace body
                                            BEGIN( ReadNSBody );
                                          }
                                          else
                                          { // class body
                                            BEGIN( ReadBody ) ;
                                          }
                                        }
<BasesProt>"virtual"{BN}+               { lineCount(yyscanner); yyextra->baseVirt = Specifier::Virtual; }
<BasesProt>"public"{BN}+                { lineCount(yyscanner); yyextra->baseProt = Protection::Public; }
<BasesProt>"protected"{BN}+             { lineCount(yyscanner); yyextra->baseProt = Protection::Protected; }
<BasesProt>"internal"{BN}+              { if (!yyextra->insideCli) REJECT ; lineCount(yyscanner); yyextra->baseProt = Protection::Package; }
<BasesProt>"private"{BN}+               { lineCount(yyscanner); yyextra->baseProt = Protection::Private; }
<BasesProt>{BN}                         { lineCount(yyscanner); }
<BasesProt>.                            { unput(*yytext); BEGIN(Bases); }
<Bases>"decltype"{BN}*"("               {
                                          lineCount(yyscanner);
                                          yyextra->roundCount=0;
                                          yyextra->lastSkipRoundContext=YY_START;
                                          BEGIN(SkipRound);
                                        }
<Bases>("\\")?({ID}"\\")*{ID}           { // PHP namespace token, not sure if interspacing is allowed but it gives problems (see bug 640847)
                                          if (!yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else // PHP base class of the form \Ns\Cl or Ns\Cl
                                          {
                                            lineCount(yyscanner);
                                            QCString bn=yytext;
                                            bn = substitute(bn,"\\","::");
                                            yyextra->baseName += bn;
                                            yyextra->current->args += ' ';
                                            yyextra->current->args += yytext;
                                          }
                                        }
<Bases>("::")?{BN}*({ID}{BN}*"::"{BN}*)*{ID}("...")? {
                                          lineCount(yyscanner);
                                          QCString baseScope = yytext;
                                          if (yyextra->insideCS && baseScope.stripWhiteSpace()=="where")
                                          {
                                            // type constraint for a class
                                            yyextra->current->typeConstr.clear();
                                            yyextra->current->typeConstr.push_back(Argument());
                                            yyextra->lastCSConstraint = YY_START;
                                            BEGIN( CSConstraintName );
                                          }
                                          else
                                          {
                                            yyextra->baseName+=yytext;
                                            yyextra->current->args += ' ';
                                            yyextra->current->args += yytext;
                                          }
                                        }
<Bases>{BN}*{ID}("."{ID})*              { // Java style class
                                          QCString name = substitute(yytext,".","::");
                                          yyextra->baseName += name;
                                          yyextra->current->args += ' ';
                                          yyextra->current->args += name;
                                        }
<ClassVar,Bases>\n/{BN}*[^{, \t\n]      {
                                          if (!yyextra->insideObjC)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            lineCount(yyscanner);
                                            unput('{');
                                          }
                                        }
<ClassVar,Bases>"@end"                  { // empty ObjC interface
                                          unput('d'); // insert fake body: {}@end
                                          unput('n');
                                          unput('e');
                                          unput('@');
                                          unput('}');
                                          unput('{');
                                        }
<ClassVar>"<"                           { yyextra->current->name += *yytext;
                                          yyextra->sharpCount=1;
                                          yyextra->roundCount=0;
                                          yyextra->lastSkipSharpContext = YY_START;
                                          yyextra->specName = &yyextra->current->name;
                                          BEGIN ( Specialization );
                                        }
<Bases>{BN}*"<"                         {
                                          lineCount(yyscanner);
                                          yyextra->sharpCount=1;
                                          yyextra->roundCount=0;
                                          yyextra->lastSkipSharpContext = YY_START;
                                          if (yyextra->insideObjC) // start of protocol list
                                          {
                                            unput(',');
                                          }
                                          else // template specialization
                                          {
                                            //if (yyextra->insideCS) // generic
                                            //{
                                            //  yyextra->baseName+="-g";
                                            //}
                                            yyextra->templateStr = yytext;
                                            yyextra->specName = &yyextra->templateStr;
                                            BEGIN ( Specialization );
                                          }
                                        }
<Specialization>"<"                     { *yyextra->specName += *yytext;
                                          if (yyextra->roundCount==0) yyextra->sharpCount++;
                                        }
<Specialization>">"                     {
                                          *yyextra->specName += *yytext;
                                          if (yyextra->roundCount==0 && --yyextra->sharpCount<=0)
                                          {
                                            yyextra->baseName+=removeRedundantWhiteSpace(*yyextra->specName);
                                            BEGIN(yyextra->lastSkipSharpContext);
                                          }
                                        }
<Specialization>{BN}+                   { lineCount(yyscanner); *yyextra->specName +=' '; }
<Specialization>"<<"                    { *yyextra->specName += yytext; }
<Specialization>">>"/{B}*"::"           { // M$ C++ extension to allow >> to close a template...
                                          unput('>');
                                          unput(' ');
                                          unput('>');
                                        }
<Specialization>">>"                    {
                                          if (yyextra->insideCS) // for C# >> ends a nested template
                                          {
                                            REJECT;
                                          }
                                          else // for C++ >> is a bitshift
                                               // operator and > > would end
                                               // a nested template.
                                               // We require the bitshift to be enclosed in braces.
                                               // See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1757.html
                                          {
                                            if (yyextra->roundCount>0)
                                            {
                                              *yyextra->specName += yytext;
                                            }
                                            else
                                            {
                                              unput('>');
                                              unput(' ');
                                              unput('>');
                                            }
                                          }
                                        }
<Specialization>"typename"{BN}+         { lineCount(yyscanner); }
<Specialization>"("                     { *yyextra->specName += *yytext; yyextra->roundCount++; }
<Specialization>")"                     { *yyextra->specName += *yytext; yyextra->roundCount--; }

<Specialization>"\\\\"                  { *yyextra->specName += *yytext;}
<Specialization>"\\'"                   { *yyextra->specName += *yytext;}
<Specialization>"\\\""                  { *yyextra->specName += *yytext;}
<Specialization>"'"                     { *yyextra->specName += *yytext;BEGIN(SpecializationSingleQuote);}
<Specialization>"\""                    { *yyextra->specName += *yytext;BEGIN(SpecializationDoubleQuote);}
<SpecializationSingleQuote,SpecializationDoubleQuote>"\\\\"       { *yyextra->specName += *yytext;}
<SpecializationSingleQuote>"\\'"        { *yyextra->specName += *yytext;}
<SpecializationSingleQuote>"'"          { *yyextra->specName += *yytext; BEGIN(Specialization);}
<SpecializationDoubleQuote>"\\\""       { *yyextra->specName += *yytext;}
<SpecializationDoubleQuote>"\""         { *yyextra->specName += *yytext; BEGIN(Specialization);}
<SpecializationSingleQuote,SpecializationDoubleQuote>.          { *yyextra->specName += *yytext;}

<Specialization>.                       {
                                          *yyextra->specName += *yytext;
                                        }
<SkipRound>"("                          { ++yyextra->roundCount; }
<SkipRound>")"                          { if (--yyextra->roundCount<0)
                                            BEGIN ( yyextra->lastSkipRoundContext );
                                        }
<SkipRound>\"                           {
                                          yyextra->lastStringContext=SkipRound;
                                          BEGIN(SkipString);
                                        }
<Bases>","|(">"({BN}*"{")?)|({BN}+"implements"{BN}*)    { lineCount(yyscanner);
                                          if (yyextra->insideProtocolList)
                                          {
                                            yyextra->baseName+="-p";
                                          }
                                          else
                                          {
                                            yyextra->current->args += ',' ;
                                          }
                                          yyextra->current->name = removeRedundantWhiteSpace(yyextra->current->name);
                                          if (!yyextra->baseName.isEmpty())
                                          {
                                            yyextra->current->extends.emplace_back(
                                              yyextra->baseName,yyextra->baseProt,yyextra->baseVirt
                                            );
                                          }
                                          if (yyextra->current->spec.isInterface() || yyextra->current->spec.isStruct() ||
                                              yyextra->insideJava || yyextra->insidePHP  || yyextra->insideCS  ||
                                              yyextra->insideD    || yyextra->insideObjC || yyextra->insideIDL || yyextra->insideSlice)
                                          {
                                            yyextra->baseProt=Protection::Public;
                                          }
                                          else
                                          {
                                            yyextra->baseProt=Protection::Private;
                                          }
                                          yyextra->baseVirt=Specifier::Normal;
                                          yyextra->baseName.clear();
                                          if (*yytext=='>')
                                          { // end of a ObjC protocol list
                                            yyextra->insideProtocolList=FALSE;
                                            if (yyleng==1)
                                            {
                                              unput('{'); // dummy start body
                                            }
                                            else
                                            {
                                              yyless(1);
                                            }
                                          }
                                          else
                                          {
                                            if (*yytext==',' && yyextra->insideObjC) // Begin of protocol list
                                            {
                                              yyextra->insideProtocolList=TRUE;
                                            }
                                            BEGIN(BasesProt);
                                          }
                                        }
<Bases>{B}*"{"{B}*                      {
                                          yyextra->current->program.str(std::string());
                                          yyextra->current->fileName = yyextra->fileName ;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = removeRedundantWhiteSpace(yyextra->current->name);
                                          if (!yyextra->baseName.isEmpty())
                                            yyextra->current->extends.emplace_back(
                                               yyextra->baseName,yyextra->baseProt,yyextra->baseVirt
                                            );
                                          yyextra->curlyCount=0;
                                          if (yyextra->insideObjC)
                                          {
                                            BEGIN( ReadBodyIntf );
                                          }
                                          else
                                          {
                                            BEGIN( ReadBody ) ;
                                          }
                                        }
<SkipUnionSwitch>{B}*"("                {
                                          yyextra->roundCount++;
                                        }
<SkipUnionSwitch>")"                    {
                                          if (--yyextra->roundCount==0)
                                          {
                                            BEGIN(ClassVar);
                                          }
                                        }
<SkipUnionSwitch>\n                     { lineCount(yyscanner); }
<SkipUnionSwitch>.
<Comment>{BN}+                          { yyextra->current->program << yytext ;
                                          lineCount(yyscanner) ;
                                        }
<Comment>{CCS}                           { yyextra->current->program << yytext ; }
<Comment>{CPPC}                           { yyextra->current->program << yytext ; }
<Comment>{CMD}("code"|"verbatim"|"iliteral") {
                                          if (yyextra->doxygenComment) yyextra->insideCode=TRUE;
                                          yyextra->current->program << yytext ;
                                        }
<Comment>{CMD}("endcode"|"endverbatim"|"endiliteral") {
                                          if (yyextra->doxygenComment) yyextra->insideCode=FALSE;
                                          yyextra->current->program << yytext ;
                                        }
<Comment>[^ \.\t\r\n\/\*]+              { yyextra->current->program << yytext ; }
<Comment>{CCE}                          { yyextra->current->program << yytext ;
                                          if (!yyextra->insideCode)
                                          {
                                            yyextra->doxygenComment=false;
                                            BEGIN( yyextra->lastContext );
                                          }
                                        }
<Comment>.                              { yyextra->current->program << *yytext ; }

<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,SkipC11Inits,SkipC11Attribute,ReadExpressionBody,Bases,OldStyleArgs>({CPPC}{B}*)?{CCS}"!" {
                                          //printf("Start doc block at %d\n",yyextra->yyLineNr);
                                          if (!yyextra->current->doc.isEmpty())
                                          {
                                            yyextra->current->doc+="\n\n";
                                          }
                                          else
                                          {
                                            yyextra->current->docLine = yyextra->yyLineNr;
                                            yyextra->current->docFile = yyextra->fileName;
                                          }

                                          yyextra->lastDocContext = YY_START;
                                          if (yyextra->current_root->section.isScope())
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = YY_START==SkipCurly || YY_START==ReadExpressionBody;
                                          yyextra->docBlockAutoBrief = Config_getBool(QT_AUTOBRIEF);

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());

                                          if (yyextra->docBlockAutoBrief)
                                          {
                                            yyextra->current->briefLine = yyextra->yyLineNr;
                                            yyextra->current->briefFile = yyextra->fileName;
                                          }
                                          startCommentBlock(yyscanner,FALSE);
                                          BEGIN( DocBlock );
                                        }
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,ReadExpressionBody,Bases,OldStyleArgs>{CCS}"*"[*]+{BL} {
                                          bool javadocBanner = Config_getBool(JAVADOC_BANNER);
                                          lineCount(yyscanner);

                                          if( javadocBanner )
                                          {
                                            yyextra->lastDocContext = YY_START;

                                            //printf("Found comment banner at %s:%d\n",yyextra->fileName,yyextra->yyLineNr);
                                            if (yyextra->current_root->section.isScope())
                                            {
                                              yyextra->current->inside = yyextra->current_root->name+"::";
                                            }
                                            yyextra->current->docLine = yyextra->yyLineNr;
                                            yyextra->current->docFile = yyextra->fileName;
                                            yyextra->docBlockContext = YY_START;
                                            yyextra->docBlockInBody  = YY_START==SkipCurly || YY_START==ReadExpressionBody;
                                            bool javadocAutoBrief = Config_getBool(JAVADOC_AUTOBRIEF);
                                            yyextra->docBlockAutoBrief = javadocAutoBrief;

                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,yyextra->column));
                                            yyextra->docBlock.str(indent.str());

                                            if (yyextra->docBlockAutoBrief)
                                            {
                                              yyextra->current->briefLine = yyextra->yyLineNr;
                                              yyextra->current->briefFile = yyextra->fileName;
                                            }
                                            startCommentBlock(yyscanner,FALSE);
                                            BEGIN( DocBlock );
                                          }
                                          else
                                          {
                                            yyextra->current->program << yytext ;
                                            yyextra->lastContext = YY_START ;
                                            yyextra->doxygenComment=true;
                                            BEGIN( Comment ) ;
                                          }
                                        }
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,ReadExpressionBody,SkipInits,Bases,OldStyleArgs>^{B}+({CPPC}{B}*)?{CCS}"*"/{NCOMM} {
                                          lineCount(yyscanner);
                                          yyextra->yyColNr=1;
                                          REJECT;
                                        }
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,ReadExpressionBody,Operator,ClassVar,SkipInits,Bases,OldStyleArgs>({CPPC}{B}*)?{CCS}"*"/{NCOMM} {
                                          yyextra->lastDocContext = YY_START;

                                          //printf("Found comment block at %s:%d\n",yyextra->fileName,yyextra->yyLineNr);
                                          if (yyextra->current_root->section.isScope())
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->current->docLine = yyextra->yyLineNr;
                                          yyextra->current->docFile = yyextra->fileName;
                                          yyextra->docBlockContext = YY_START;
                                          yyextra->docBlockInBody  = YY_START==SkipCurly || YY_START==ReadExpressionBody;
                                          bool javadocAutoBrief = Config_getBool(JAVADOC_AUTOBRIEF);
                                          yyextra->docBlockAutoBrief = javadocAutoBrief;

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());

                                          if (yyextra->docBlockAutoBrief)
                                          {
                                            yyextra->current->briefLine = yyextra->yyLineNr;
                                            yyextra->current->briefFile = yyextra->fileName;
                                          }
                                          startCommentBlock(yyscanner,FALSE);
                                          BEGIN( DocBlock );
                                        }
<FindMembers,FindFields,MemberSpec,SkipCurly,ReadExpressionBody,FuncQual,Operator,ClassVar,Bases,OldStyleArgs>{CPPC}"!" {
                                          yyextra->lastDocContext = YY_START;
                                          if (yyextra->current_root->section.isScope())
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = YY_START==SkipCurly || YY_START==ReadExpressionBody;
                                          yyextra->docBlockAutoBrief = FALSE;

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());

                                          startCommentBlock(yyscanner,yyextra->current->brief.isEmpty());
                                          BEGIN( DocLine );
                                        }
<FindMembers,FindFields,MemberSpec,SkipCurly,ReadExpressionBody,FuncQual,Operator,ClassVar,Bases,OldStyleArgs>{CPPC}"/"/[^/] {
                                          yyextra->lastDocContext = YY_START;
                                          if (yyextra->current_root->section.isScope())
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = YY_START==SkipCurly || YY_START==ReadExpressionBody;
                                          yyextra->docBlockAutoBrief = FALSE;
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());
                                          startCommentBlock(yyscanner,yyextra->current->brief.isEmpty());
                                          BEGIN( DocLine );
                                        }
<FindMembers>"extern"{BN}*"\""[^\"]+"\""{BN}*("{")?  {
                                          lineCount(yyscanner);
                                          yyextra->externLinkage=TRUE;
                                        }
<FindMembers>"{"                        {
                                          if (yyextra->externLinkage)
                                          {
                                            yyextra->externLinkage=FALSE;
                                          }
                                          else if (yyextra->insideCS &&
                                              !yyextra->current->name.isEmpty() &&
                                              !yyextra->current->type.isEmpty())
                                          { 
                                            if (yyextra->current->mtype == MethodTypes::Event)
                                            {
                                              yyextra->mtype = MethodTypes::Event;
                                            }
                                            else if (containsWord(yyextra->current->type,"event")) // event
                                            {
                                              yyextra->current->mtype = yyextra->mtype = MethodTypes::Event;
                                            }
                                            else // property
                                            {
                                              yyextra->current->mtype = yyextra->mtype = MethodTypes::Property;
                                            }
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( CSAccessorDecl );
                                          }
                                          else if (yyextra->insideIDL && yyextra->current->spec.isAttribute())
                                          {
                                            // UNO IDL: attributes may have setter and getter
                                            // exception specifications
                                            yyextra->current->exception = " {";
                                            BEGIN(UNOIDLAttributeBlock);
                                          }
                                          else
                                          {
                                            if ((yyextra->insideJava || yyextra->insideCS || yyextra->insideD) &&
                                                yyextra->current->name.isEmpty()
                                               )
                                            {
                                              // static Java initializer
                                              yyextra->needsSemi = FALSE;
                                              if (yyextra->current->isStatic)
                                              {
                                                yyextra->current->name="[static initializer]";
                                                yyextra->current->type.clear();
                                              }
                                              else
                                              {
                                                yyextra->current->name="[instance initializer]";
                                              }
                                              unput(*yytext);
                                              BEGIN( SFunction );
                                            }
                                            else
                                            {
                                              // pre C++11 code -> ignore the initializer
                                              //yyextra->needsSemi = TRUE;
                                              //yyextra->current->type.clear();
                                              //yyextra->current->name.clear();
                                              //yyextra->current->args.clear();
                                              //yyextra->current->argList.clear();
                                              //yyextra->curlyCount=0;
                                              //BEGIN( SkipCurlyBlock );

                                              // C++11 style initializer list
                                              yyextra->current->bodyLine = yyextra->yyLineNr;
                                              yyextra->current->bodyColumn = yyextra->yyColNr;
                                              yyextra->current->initializer.str(yytext);
                                              yyextra->lastInitializerContext = YY_START;
                                              yyextra->sharpCount=0;
                                              yyextra->initBracketCount=1;
                                              BEGIN(ReadInitializer);
                                            }
                                          }
                                        }
<CSAccessorDecl>"{"                     { yyextra->curlyCount++; }
<CSAccessorDecl>"}"{B}*"="              {
                                          // fall back to next rule if it's not the right bracket
                                          if (yyextra->curlyCount != 0) REJECT;
                                          yyextra->current->initializer.str("=");
                                          yyextra->current->endBodyLine=yyextra->yyLineNr;
                                          yyextra->lastInitializerContext = FindMembers;
                                          BEGIN(ReadInitializer);
                                        }
<CSAccessorDecl>"}"                     {
                                          if (yyextra->curlyCount)
                                          {
                                            yyextra->curlyCount--;
                                          }
                                          else
                                          {
                                            yyextra->mtype = MethodTypes::Method;
                                            yyextra->virt = Specifier::Normal;
                                            // not really important, but while we are at it
                                            yyextra->current->endBodyLine=yyextra->yyLineNr;
                                            unput(';');
                                            BEGIN(FindMembers);
                                          }
                                        }
<CSAccessorDecl>"private "{BN}*"set"    { if (yyextra->curlyCount==0) yyextra->current->spec.setPrivateSettable(true);   }
<CSAccessorDecl>"protected "{BN}*"set"  { if (yyextra->curlyCount==0) yyextra->current->spec.setProtectedSettable(true); }
<CSAccessorDecl>"private "{BN}*"get"    { if (yyextra->curlyCount==0) yyextra->current->spec.setPrivateGettable(true);   }
<CSAccessorDecl>"protected "{BN}*"get"  { if (yyextra->curlyCount==0) yyextra->current->spec.setProtectedGettable(true); }
<CSAccessorDecl>"set"                   { if (yyextra->curlyCount==0) yyextra->current->spec.setSettable(true);          }
<CSAccessorDecl>"get"                   { if (yyextra->curlyCount==0) yyextra->current->spec.setGettable(true);          }
<CSAccessorDecl>"add"                   { if (yyextra->curlyCount==0) yyextra->current->spec.setAddable(true);           }
<CSAccessorDecl>"remove"                { if (yyextra->curlyCount==0) yyextra->current->spec.setRemovable(true);         }
<CSAccessorDecl>"raise"                 { if (yyextra->curlyCount==0) yyextra->current->spec.setRaisable(true);          }
<CSAccessorDecl>{CHARLIT}               {}
<CSAccessorDecl>"\""                    { BEGIN(CSString);}
<CSAccessorDecl>"."                     {}
<CSAccessorDecl>\n                      { lineCount(yyscanner); }
<CSString>"\""                          { BEGIN(CSAccessorDecl);}
<CSString>{CPPC}                          {} // Otherwise the rule <*>"//" will kick in
<CSString>{CCS}                          {} // Otherwise the rule <*>"/*" will kick in
<CSString>\n                            { lineCount(yyscanner); }
<CSString>"."                           {}

 /* ---- Slice-specific rules ------ */

<SliceSequence>{SCOPENAME}              {
                                          if (yyextra->current->spec.isLocal())
                                          {
                                            yyextra->current->type = "local ";
                                          }
                                          yyextra->current->type += "sequence<";
                                          yyextra->current->type += yytext;
                                          yyextra->current->type += ">";
                                        }

<SliceSequence>{BN}*">"{BN}*            {
                                          lineCount(yyscanner);
                                          BEGIN(SliceSequenceName);
                                        }

<SliceSequenceName>{ID}{BN}*            {
                                          lineCount(yyscanner);
                                          yyextra->current->name = yytext ;
                                          yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                        }

<SliceSequenceName>";"                  {
                                          yyextra->current->section = EntryType::makeVariable();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }

<SliceDictionary>{SCOPENAME}{BN}*","{BN}*{SCOPENAME} {
                                          lineCount(yyscanner);
                                          if (yyextra->current->spec.isLocal())
                                          {
                                            yyextra->current->type = "local ";
                                          }
                                          yyextra->current->type += "dictionary<";
                                          yyextra->current->type += yytext;
                                          yyextra->current->type += ">";
                                          yyextra->current->type = yyextra->current->type.simplifyWhiteSpace();
                                        }

<SliceDictionary>{BN}*">"{BN}*          {
                                          lineCount(yyscanner);
                                          BEGIN(SliceDictionaryName);
                                        }

<SliceDictionaryName>{ID}{BN}*          {
                                          lineCount(yyscanner);
                                          yyextra->current->name = yytext ;
                                          yyextra->current->name = yyextra->current->name.stripWhiteSpace();
                                        }

<SliceDictionaryName>";"                {
                                          yyextra->current->section = EntryType::makeVariable();
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }

 /**********************************************************************************/
 /******************** Documentation block related rules ***************************/
 /**********************************************************************************/

 /* ---- Single line comments ------ */
<DocLine>[^\n]*"\n"[ \t]*{CPPC}[/!][<]?   { // continuation of multiline C++-style comment
                                           int markerLen = yytext[yyleng-1]=='<' ? 4 : 3;
                                           yyextra->docBlock << std::string(yytext).substr(0,yyleng-markerLen);
                                           lineCount(yyscanner);
                                        }
<DocLine>{B}*{CPPC}"/"[/]+{Bopt}/"\n"         { // ignore marker line (see bug700345)
                                          handleCommentBlock(yyscanner,yyextra->docBlock.str(),yyextra->current->brief.isEmpty());
                                          BEGIN( yyextra->docBlockContext );
                                        }
<DocLine>{NONLopt}/"\n"{B}*{CPPC}[!/]{B}*{CMD}"}"    { // next line is an end group marker, see bug 752712
                                          yyextra->docBlock << yytext;
                                          handleCommentBlock(yyscanner,yyextra->docBlock.str(),yyextra->current->brief.isEmpty());
                                          BEGIN( yyextra->docBlockContext );
                                        }
<DocLine>{NONLopt}/"\n"                    { // whole line
                                          yyextra->docBlock << yytext;
                                          handleCommentBlock(yyscanner,yyextra->docBlock.str(),yyextra->current->brief.isEmpty());
                                          BEGIN( yyextra->docBlockContext );
                                        }

 /* ---- Comments blocks ------ */

<DocBlock>"*"*{CCE}                      { // end of comment block
                                          handleCommentBlock(yyscanner,yyextra->docBlock.str(),FALSE);
                                          BEGIN(yyextra->docBlockContext);
                                        }
<DocBlock>"\\ilinebr "{B}*"*"/[^/]      {
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext+8,yyextra->column));
                                          yyextra->docBlock << "\\ilinebr " << indent;
                                        }
<DocBlock>^{B}*"*"+/[^/]                {
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock << indent;
                                        }
<DocBlock>^{B}*({CPPC})?{B}*"*"+/[^/a-z_A-Z0-9*] { // start of a comment line
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock << indent;
                                        }
<DocBlock>^{B}*({CPPC}){B}*               { // strip embedded C++ comments if at the start of a line
                                        }
<DocBlock>{CPPC}                          { // slashes in the middle of a comment block
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>{CCS}                          { // start of a new comment in the
                                          // middle of a comment block
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>{CMD}("f$"|"f["|"f{"|"f(")    {
                                          yyextra->docBlock << yytext;
                                          char blockName[] = "f$";
                                          char c = yytext[2];
                                          if      (c=='[') blockName[1]=']';
                                          else if (c=='{') blockName[1]='}';
                                          else if (c=='(') blockName[1]=')';
                                          startVerbatimBlock(yyscanner,blockName);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}"ifile"{B}+"\""[^\n\"]+"\"" {
                                          yyextra->fileName = &yytext[6];
                                          yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                          yyextra->fileName = yyextra->fileName.mid(1,yyextra->fileName.length()-2);
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>{CMD}"ifile"{B}+{FILEMASK}    {
                                          yyextra->fileName = &yytext[6];
                                          yyextra->fileName = yyextra->fileName.stripWhiteSpace();
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>{CMD}"iline"{LINENR}{B}       {
                                          bool ok = false;
                                          int nr = QCString(&yytext[6]).toInt(&ok);
                                          if (!ok)
                                          {
                                            warn(yyextra->fileName,yyextra->yyLineNr,"Invalid line number '{}' for iline command",yytext);
                                          }
                                          else
                                          {
                                            yyextra->yyLineNr = nr;
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>{B}*"<"{PRE}">"               {
                                          yyextra->docBlock << yytext;
                                          startVerbatimBlock(yyscanner,"<pre>");
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}"startuml"/[^a-z_A-Z0-9\-]     { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlock << yytext;
                                          startVerbatimBlock(yyscanner,"uml");
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc")/[^a-z_A-Z0-9\-]     { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlock << yytext;
                                          startVerbatimBlock(yyscanner,&yytext[1]);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}("code")/[^a-z_A-Z0-9\-] { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlock << yytext;
                                          startVerbatimBlock(yyscanner,&yytext[1],0,true);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>"\\ilinebr "({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                                          QCString pat = substitute(yytext+9,"*"," "); // skip over "\ilinebr " part
                                          yyextra->docBlock << "\\ilinebr ";
                                          yyextra->docBlock << pat;
                                          startVerbatimBlock(yyscanner,"~~~",pat.stripWhiteSpace().length(),true);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                                          QCString pat = substitute(yytext,"*"," ");
                                          yyextra->docBlock << pat;
                                          startVerbatimBlock(yyscanner,"~~~",pat.stripWhiteSpace().length(),true);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>"\\ilinebr "({B}*"*"+)?{B}{0,3}"```"[`]*/(".")?[a-zA-Z0-9#_-]+ |
<DocBlock>"\\ilinebr "({B}*"*"+)?{B}{0,3}"```"[`]*/"{"[^}]+"}" |
<DocBlock>"\\ilinebr "({B}*"*"+)?{B}{0,3}"```"[`]* {
                                          QCString pat = substitute(yytext+9,"*"," "); // skip over "\ilinebr " part
                                          yyextra->docBlock << "\\ilinebr ";
                                          yyextra->docBlock << pat;
                                          startVerbatimBlock(yyscanner,"```",pat.stripWhiteSpace().length(),true);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*/(".")?[a-zA-Z0-9#_-]+ |
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*/"{"[^}]+"}" |
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]* {
                                          QCString pat = substitute(yytext,"*"," ");
                                          yyextra->docBlock << pat;
                                          startVerbatimBlock(yyscanner,"```",pat.stripWhiteSpace().length(),true);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>"```"                         { // skip ``` if not at the start of a line
                                          yyextra->docBlock << "```";
                                        }
<DocBlock>"\\`"                         { // skip escaped backtick
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>"`"{1,2}                      {
                                          yyextra->docBlock << yytext;
                                          startVerbatimBlock(yyscanner,yytext,yyleng,true);
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{B}*"<"{CODE}">"              {
                                          if (yyextra->insideCS)
                                          {
                                            yyextra->docBlock << yytext;
                                            startVerbatimBlock(yyscanner,"<code>",0,true);
                                            BEGIN(DocCopyBlock);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<DocBlock>[^@*~`'\/\\\n]+               { // any character that isn't special
                                          yyextra->docBlock << yytext;
                                        }
<DocBlock>\n                            { // newline
                                          lineCount(yyscanner);
                                          yyextra->docBlock << *yytext;
                                        }
<DocBlock>.                             { // command block
                                          yyextra->docBlock << *yytext;
                                        }

 /* ---- Copy verbatim sections ------ */

<DocCopyBlock>"</"{PRE}">"              { // end of a <pre> block
                                          if (endVerbatimBlock(yyscanner,"<pre>"))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>"</"{CODE}">"             { // end of a <code> block
                                          if (endVerbatimBlock(yyscanner,"<code>"))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>[\\@]("f$"|"f]"|"f}"|"f)")     {
                                          if (endVerbatimBlock(yyscanner,&yytext[1]))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>[\\@]("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endmsc"|"enduml"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
                                          if (endVerbatimBlock(yyscanner,&yytext[4]))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>^{B}*"*"+/{BN}+           { // start of a comment line
                                          if ((yyextra->docBlockName=="verbatim") || (yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,0));
                                            yyextra->docBlock << indent;
                                          }
                                        }
<DocCopyBlock>^{B}*"*"+/{B}+"*"{BN}*    { // start of a comment line with two *'s
                                          if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                                          {
                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,0));
                                            yyextra->docBlock << indent;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<DocCopyBlock>^{B}*"*"+/({ID}|"(")      { // Assume *var or *(... is part of source code (see bug723516)
                                          if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                                          {
                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,-1));
                                            yyextra->docBlock << indent+"*";
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<DocCopyBlock>^{B}*"*"+/{BN}*           { // start of a comment line with one *
                                          if ((yyextra->docBlockName=="code") || (yyextra->docBlockName=="iliteral"))
                                          {
                                            QCString indent;
                                            if (yyextra->nestedComment>0) // keep * it is part of the code
                                            {
                                              indent.fill(' ',computeIndent(yytext,-1));
                                              yyextra->docBlock << indent+"*";
                                            }
                                            else // remove * it is part of the comment block
                                            {
                                              indent.fill(' ',computeIndent(yytext,0));
                                              yyextra->docBlock << indent;
                                            }
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                                          QCString pat = substitute(yytext,"*"," ");
                                          if (endVerbatimBlock(yyscanner,"~~~",pat.stripWhiteSpace().length()))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << pat;
                                        }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
                                          QCString pat = substitute(yytext,"*"," ");
                                          if (endVerbatimBlock(yyscanner,"```",pat.stripWhiteSpace().length()))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << pat;
                                        }
<DocCopyBlock>"```"                     { // backtick + end marker
                                          if (yyextra->docBlockName=="``") 
                                          {
                                            yyextra->docBlock << "`";
                                            endVerbatimBlock(yyscanner,"``",2);
                                            yyextra->docBlock << "``";
                                            BEGIN(DocBlock);
                                          }
                                          else
                                          {
                                            yyextra->docBlock << yytext;
                                          }
                                        }
<DocCopyBlock>"''"/[^a-z_A-Z0-9-]       {
                                          if (!Config_getBool(MARKDOWN_STRICT) && endVerbatimBlock(yyscanner,"``",2))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>"'"/[^'a-z_A-Z0-9-]       {
                                          if (!Config_getBool(MARKDOWN_STRICT) && endVerbatimBlock(yyscanner,"`",1))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>"`"{1,2}                  {
                                          if (endVerbatimBlock(yyscanner,yytext,yyleng))
                                          {
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>[^\<@/\*\]`'~"\$\\\n]+    { // any character that is not special
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>\"                        {
                                          yyextra->docBlock << yytext;
                                          if (yyextra->docBlockName=="code" || yyextra->docBlockName=="iliteral") 
                                            // to support end of comment character sequences inside
                                            // a string literal of a code block, see #6737
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            yyextra->pCopyQuotedGString=&yyextra->docBlock;
                                            yyextra->stopAtInvalidString=true;
                                            BEGIN(CopyGString);
                                          }
                                        }
<DocCopyBlock>{CCS}|{CCE}|{CPPC}        {
                                          if (yytext[1]=='*') // start comment
                                          {
                                            yyextra->nestedComment++;
                                          }
                                          else if (yytext[0]=='*' && yyextra->nestedComment>0) // end comment
                                          {
                                            yyextra->nestedComment--;
                                          }
                                          else if (yytext[0]=='*' && yyextra->nestedComment==0 && yyextra->isCodeBlock) // end comment without start
                                          {
                                            warn(yyextra->fileName,yyextra->yyLineNr,
                                                "Found end of C comment inside a '{}' block without matching start of the comment!"
                                                " Maybe the end marker for the block is missing?",
                                                yyextra->docBlockName);
                                            BEGIN(DocBlock);
                                          }
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>\n                        { // newline
                                          yyextra->docBlock << *yytext;
                                          lineCount(yyscanner);
                                        }
<DocCopyBlock>.                         { // any other character
                                          yyextra->docBlock << *yytext;
                                        }
<DocCopyBlock><<EOF>>                   {
                                          warn(yyextra->fileName,yyextra->yyLineNr,
                                              "reached end of file while inside a '{}' block!"
                                              " The command that should end the block seems to be missing!",
                                              yyextra->docBlockName);
                                          yyterminate();
                                        }


    /* ------------- Prototype parser -------------- */

<Prototype>"operator"{B}*"("{B}*")"     {
                                          yyextra->current->name+=yytext;
                                        }
<Prototype>"("                         {
                                          yyextra->current->args+=*yytext;
                                          yyextra->currentArgumentContext = PrototypeQual;
                                          yyextra->fullArgString = yyextra->current->args;
                                          yyextra->copyArgString = &yyextra->current->args;
                                          BEGIN( ReadFuncArgType ) ;
                                        }
<Prototype>"("({ID}"::")*({B}*[&*])+    {
                                          if (yyextra->insidePHP) // reference parameter
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->current->type+=yyextra->current->name+yytext;
                                            yyextra->current->name.clear();
                                            BEGIN( PrototypePtr );
                                          }
                                        }
<PrototypePtr>{SCOPENAME}               {
                                          yyextra->current->name+=yytext;
                                        }
<PrototypePtr>"("                       {
                                          yyextra->current->args+=*yytext;
                                          yyextra->currentArgumentContext = PrototypeQual;
                                          yyextra->fullArgString = yyextra->current->args;
                                          yyextra->copyArgString = &yyextra->current->args;
                                          BEGIN( ReadFuncArgType ) ;
                                        }
<PrototypePtr>")"                       {
                                          yyextra->current->type+=')';
                                          BEGIN( Prototype );
                                        }
<PrototypePtr>.                         {
                                          yyextra->current->name+=yytext;
                                        }
<PrototypeQual>"{"                      {
                                          BEGIN( PrototypeSkipLine);
                                        }
<PrototypeQual>{B}*"const"{B}*          {
                                          yyextra->current->args += " const ";
                                          yyextra->current->argList.setConstSpecifier(TRUE);
                                        }
<PrototypeQual>{B}*"volatile"{B}*       {
                                          yyextra->current->args += " volatile ";
                                          yyextra->current->argList.setVolatileSpecifier(TRUE);
                                        }
<PrototypeQual>{B}*"="{B}*"0"{B}*       {
                                          yyextra->current->args += " = 0";
                                          yyextra->current->virt = Specifier::Pure;
                                          yyextra->current->argList.setPureSpecifier(TRUE);
                                        }
<PrototypeQual>"throw"{B}*"("           {
                                          yyextra->current->exception = "throw(";
                                          BEGIN(PrototypeExc);
                                        }
<PrototypeExc>")"                       {
                                          yyextra->current->exception += ')';
                                          BEGIN(PrototypeQual);
                                        }
<PrototypeExc>.                         {
                                          yyextra->current->exception += *yytext;
                                        }
<PrototypeQual>.                        {
                                          yyextra->current->args += *yytext;
                                        }
<Prototype>.                            {
                                          yyextra->current->name += *yytext;
                                        }
<PrototypeSkipLine>.                    {
                                        }




<SkipCxxComment>.*"\\\n"                {  // line continuation
                                          if (yyextra->insideCS)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            lineCount(yyscanner);
                                          }
                                        }
<SkipCxxComment>{ANYopt}/\n                   {
                                          BEGIN( yyextra->lastCContext ) ;
                                        }
<SkipComment>[^\*\n]+

  /* ------------ Generic rules -------------- */

<*>"[["                                 { // C++11 attribute
                                          if (!yyextra->insideCpp) REJECT;
                                          if (YY_START == CopyGString || YY_START == CopyGString) REJECT;
                                          yyextra->lastC11AttributeContext = YY_START;
                                          BEGIN( SkipC11Attribute );
                                        }

<*>\n                                   { lineCount(yyscanner); }
<*>\"                                   {
                                          if (yyextra->insideIDL && yyextra->insideCppQuote)
                                          {
                                            BEGIN(EndCppQuote);
                                          }
                                          else if (yyextra->insidePHP)
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(SkipString);
                                          }
                                        }
<*>^{B}*"#"                             {
                                          if (!yyextra->insidePHP)
                                          {
                                            yyextra->lastCPPContext = YY_START;
                                            BEGIN( SkipCPP ) ;
                                          }
                                          else
                                          {
                                            yyextra->lastCContext = YY_START ;
                                            BEGIN( SkipCxxComment ) ;
                                          }
                                        }
<*>"#"                                  {
                                          if (!yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipCxxComment ) ;
                                        }
<*>\'                                   {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(SkipPHPString);
                                          }
                                        }
<*>\?                                   {
                                          if (yyextra->insideCS && (YY_START != SkipRound) && (YY_START != CSAccessorDecl))
                                          {
                                            if (yyextra->current->type.isEmpty())
                                            {
                                              if (yyextra->current->name.isEmpty())
                                                yyextra->current->name="?";
                                              else
                                                yyextra->current->name+="?";
                                            }
                                            else
                                            {
                                              yyextra->current->type+="?";
                                            }
                                          }
                                        }
<*>"}"                                  { yyextra->exported=false; }
<*>.
<SkipComment>{CPPC}|{CCS}
<*>{CCS}                                { yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipComment ) ;
                                        }
<SkipComment>{B}*{CCE}                  { BEGIN( yyextra->lastCContext ) ; }
<*>{CPPC}                               {
                                          yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipCxxComment ) ;
                                        }
<<EOF>>                                 {
                                          if (yyextra->insideCS && yyextra->fakeNS)
                                          {
                                            yyextra->fakeNS--;
                                            unput('}');
                                            BEGIN ( ReadNSBody);
                                          }
                                          else
                                          {
                                            yyterminate();
                                          }
                                        }
%%

//----------------------------------------------------------------------------
static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  while( c < max_size && yyextra->inputString[yyextra->inputPosition] )
  {
    *buf = yyextra->inputString[yyextra->inputPosition++] ;
    //printf("%d (%c)\n",*buf,*buf);
    c++; buf++;
  }
  return c;
}


static void initParser(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->outerScopeEntries.clear();
  yyextra->baseName.clear();
  yyextra->protection = Protection::Public;
  yyextra->baseProt = Protection::Public;
  yyextra->sharpCount = 0;
  yyextra->roundCount = 0;
  yyextra->curlyCount = 0;
  yyextra->mtype = MethodTypes::Method;
  yyextra->isStatic = FALSE;
  yyextra->virt = Specifier::Normal;
  yyextra->baseVirt = Specifier::Normal;
  yyextra->isTypedef = FALSE;
  yyextra->insideTryBlock = FALSE;
  yyextra->insideFormula = FALSE;
  yyextra->insideCode=FALSE;
  yyextra->insideCli=Config_getBool(CPP_CLI_SUPPORT);
  yyextra->previous = 0;
  yyextra->firstTypedefEntry.reset();
  yyextra->memspecEntry.reset();
}

static void initEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->insideJava)
  {
    yyextra->protection = (yyextra->current_root->spec.isInterface() || yyextra->current_root->spec.isEnum()) ?  Protection::Public : Protection::Package;
  }
  yyextra->current->protection = yyextra->protection;
  yyextra->current->exported   = yyextra->exported ;
  yyextra->current->mtype      = yyextra->mtype;
  yyextra->current->virt       = yyextra->virt;
  yyextra->current->isStatic   = yyextra->isStatic;
  yyextra->current->lang       = yyextra->language;
  //printf("*** initEntry(yyscanner) yyextra->language=%d\n",yyextra->language);
  yyextra->commentScanner.initGroupInfo(yyextra->current.get());
  yyextra->isTypedef=FALSE;
}


//-----------------------------------------------------------------------------

static void storeClangId(yyscan_t yyscanner,const char *id)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
  {
    yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,id);
  }
}

static void lineCount(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int tabSize = Config_getInt(TAB_SIZE);
  const char *p;
  for (p = yytext ; *p ; ++p )
  {
    if (*p=='\n')
    {
      yyextra->yyLineNr++;
      yyextra->column=0;
      yyextra->yyColNr=1;
    }
    else if (*p=='\t')
    {
      yyextra->column+=tabSize - (yyextra->column%tabSize);
    }
    else
    {
      yyextra->column++;
      yyextra->yyColNr++;
    }
  }
  //printf("lineCount()=%d\n",yyextra->column);
}

static inline int computeIndent(const char *s,int startIndent)
{
  int col=startIndent;
  int tabSize=Config_getInt(TAB_SIZE);
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    if (c=='\t') col+=tabSize-(col%tabSize);
    else if (c=='\n') col=0;
    else col++;
  }
  return col;
}

static inline void initMethodProtection(yyscan_t yyscanner,Protection prot)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->current->protection = yyextra->protection = prot;
  yyextra->current->mtype = yyextra->mtype = MethodTypes::Method;
  yyextra->current->type.clear();
  yyextra->current->name.clear();
  yyextra->current->args.clear();
  yyextra->current->argList.clear();
  lineCount(yyscanner) ;
}

static void addType(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  size_t tl=yyextra->current->type.length();
  if( tl>0 && !yyextra->current->name.isEmpty() && yyextra->current->type.at(tl-1)!='.')
  {
    yyextra->current->type += ' ' ;
  }
  yyextra->current->type += yyextra->current->name;
  yyextra->current->name.clear() ;
  tl=yyextra->current->type.length();
  if( tl>0 && !yyextra->current->args.isEmpty() && yyextra->current->type.at(tl-1)!='.')
  {
    yyextra->current->type += ' ' ;
  }
  yyextra->current->type += yyextra->current->args ;
  yyextra->current->args.clear() ;
  yyextra->current->argList.clear();
}


static QCString stripQuotes(const char *s)
{
  QCString name;
  if (s==nullptr || *s==0) return name;
  name=s;
  if (name.at(0)=='"' && name.at(name.length()-1)=='"')
  {
    name=name.mid(1,name.length()-2);
  }
  return name;
}

static QCString stripFuncPtr(const QCString &type)
{
  // we need to strip any trailing * and & (see bugs 623023 and 649103 for test cases)
  // also needed to reset the type for 'arr' to 'int' in 'typedef int (&fp)(), arr[2]'
  size_t i=type.length();
  bool funcPtr = i>0 && type[i-1]==')';
  if (funcPtr) i--;
  while (i>0 && (type[i-1]=='*' || type[i-1]=='&' || type[i-1]==' ')) i--;
  if (funcPtr && i>0 && type[i-1]=='(') i--;
  return type.left(i);
}

//-----------------------------------------------------------------
static void startVerbatimBlock(yyscan_t yyscanner,const QCString &blockName,size_t fencedSize,bool codeBlock)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Config_getBool(MARKDOWN_SUPPORT))
  {
    yyextra->docBlock << "\\iskip";
  }
  yyextra->docBlockName=blockName;
  yyextra->fencedSize=fencedSize;
  yyextra->isCodeBlock=codeBlock;
  yyextra->nestedComment=0;
}

//-----------------------------------------------------------------
static bool endVerbatimBlock(yyscan_t yyscanner,const QCString &blockName,size_t fencedSize)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->docBlockName==blockName && (fencedSize==0 || fencedSize==yyextra->fencedSize))
  {
    if (Config_getBool(MARKDOWN_SUPPORT))
    {
      yyextra->docBlock << "\\endiskip";
    }
    yyextra->docBlockName="";
    return true;
  }
  return false;
}

//-----------------------------------------------------------------

// return TRUE iff req holds the start of a requires expression
// or sub-expression without parenthesis, i.e. req is empty or ends with || or &&
static bool startOfRequiresExpression(const QCString &req)
{
  QCString r = req.stripWhiteSpace();
  return r.isEmpty() || r.endsWith("&&") || r.endsWith("||") || r.endsWith("and") || r.endsWith("or");
}

//-----------------------------------------------------------------

static bool nameIsOperator(QCString &name)
{
  int i=name.find("operator");
  if (i==-1) return FALSE;
  if (i==0 && !isId(name.at(8))) return TRUE; // case operator ::X
  if (i>0 && !isId(name.at(i-1)) && !isId(name.at(i+8))) return TRUE; // case X::operator
  return FALSE; // case TEXToperatorTEXT
}

//-----------------------------------------------------------------------------

static void setContext(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->language    = getLanguageFromFileName(yyextra->fileName);
  yyextra->insideIDL   = yyextra->language==SrcLangExt::IDL;
  yyextra->insideJava  = yyextra->language==SrcLangExt::Java;
  yyextra->insideCS    = yyextra->language==SrcLangExt::CSharp;
  yyextra->insideD     = yyextra->language==SrcLangExt::D;
  yyextra->insidePHP   = yyextra->language==SrcLangExt::PHP;
  yyextra->insideObjC  = yyextra->language==SrcLangExt::ObjC;
  yyextra->insideJS    = yyextra->language==SrcLangExt::JS;
  yyextra->insideSlice = yyextra->language==SrcLangExt::Slice;
  yyextra->insideCpp   = (yyextra->language==SrcLangExt::Cpp ||
                          yyextra->language==SrcLangExt::Lex);
  //printf("setContext(%s) yyextra->insideIDL=%d yyextra->insideJava=%d yyextra->insideCS=%d "
  //       "yyextra->insideD=%d yyextra->insidePHP=%d yyextra->insideObjC=%d\n",
  //       qPrint(yyextra->fileName),yyextra->insideIDL,yyextra->insideJava,yyextra->insideCS,yyextra->insideD,yyextra->insidePHP,yyextra->insideObjC
  //    );
}

//-----------------------------------------------------------------------------

static void prependScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current_root->section.isScope())
  {
    //printf("--- prependScope %s to %s\n",qPrint(yyextra->current_root->name),qPrint(yyextra->current->name));
    yyextra->current->name.prepend(yyextra->current_root->name+"::");
    //printf("prependScope #=%d #yyextra->current=%d\n",yyextra->current_root->tArgLists->count(),yyextra->current->tArgLists->count());
    for (const ArgumentList &srcAl : yyextra->current_root->tArgLists)
    {
      yyextra->current->tArgLists.insert(yyextra->current->tArgLists.begin(),srcAl);
    }
  }
}

//-----------------------------------------------------------------------------

/*! Returns TRUE iff the yyextra->current entry could be a K&R style C function */
static bool checkForKnRstyleC(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->fileName.lower().endsWith(".c")) return FALSE; // must be a C file
  if (yyextra->current->argList.empty()) return FALSE; // must have arguments
  for (const Argument &a : yyextra->current->argList)
  {
    // in K&R style argument do not have a type, but doxygen expects a type
    // so it will think the argument has no name
    if (a.type.isEmpty() || !a.name.isEmpty()) return FALSE;
  }
  return TRUE;
}

static void setJavaProtection(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->insideJava)
  {
    QCString text=yytext;
    yyextra->current->protection = Protection::Public;
    if (text.find("protected")!=-1)
      yyextra->current->protection = Protection::Protected;
    else if (text.find("private")!=-1)
      yyextra->current->protection = Protection::Private;
    else if (text.find("package")!=-1)
      yyextra->current->protection = Protection::Package;
  }
}
//-----------------------------------------------------------------------------

static void splitKnRArg(yyscan_t yyscanner,QCString &oldStyleArgPtr,QCString &oldStyleArgName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int si = static_cast<int>(yyextra->current->args.length());
  if (yyextra->oldStyleArgType.isEmpty()) // new argument
  {
    std::string args = yyextra->current->args.str();
    static const reg::Ex re(R"(\([^)]*\).*)"); // find first (...)
    int bi1=-1;
    int bi2=-1;
    reg::Match match;
    if (reg::search(args,match,re))
    {
      bi1=(int)match.position();
      size_t secondMatchStart = match.position()+match.length(); // search again after first match
      if (reg::search(args,match,re,secondMatchStart))
      {
        bi2=(int)match.position();
      }
    }
    char c;
    if (bi1!=-1 && bi2!=-1) // found something like "int (*func)(int arg)"
    {
      int s=bi2+1; // keep opening (
      yyextra->oldStyleArgType = yyextra->current->args.left(s);
      int i=s;
      while (i<si && ((c=yyextra->current->args.at(i))=='*' || isspace((uint8_t)c))) i++;
      yyextra->oldStyleArgType += yyextra->current->args.mid(s,i-s);
      s=i;
      while (i<si && isId(yyextra->current->args.at(i))) i++;
      oldStyleArgName = yyextra->current->args.mid(s,i-s);
      yyextra->oldStyleArgType+=yyextra->current->args.mid(i);
    }
    else if (bi1!=-1) // redundant braces like in "int (*var)"
    {
      int s=bi1; // strip opening (
      yyextra->oldStyleArgType = yyextra->current->args.left(s);
      s++;
      int i=s+1;
      while (i<si && ((c=yyextra->current->args.at(i))=='*' || isspace((uint8_t)c))) i++;
      yyextra->oldStyleArgType += yyextra->current->args.mid(s,i-s);
      s=i;
      while (i<si && isId(yyextra->current->args.at(i))) i++;
      oldStyleArgName = yyextra->current->args.mid(s,i-s);
    }
    else // normal "int *var"
    {
      int l=si,i=l-1,j;
      // look for start of name in "type *name"
      while (i>=0 && isId(yyextra->current->args.at(i))) i--;
      j=i+1;
      // look for start of *'s
      while (i>=0 && ((c=yyextra->current->args.at(i))=='*' || isspace((uint8_t)c))) i--;
      i++;
      if (i!=l)
      {
        yyextra->oldStyleArgType=yyextra->current->args.left(i);
        oldStyleArgPtr=yyextra->current->args.mid(i,j-i);
        oldStyleArgName=yyextra->current->args.mid(j).stripWhiteSpace();
      }
      else
      {
        oldStyleArgName=yyextra->current->args.stripWhiteSpace();
      }
    }
  }
  else // continuation like *arg2 in "int *args,*arg2"
  {
    int l=si,j=0;
    char c;
    while (j<l && ((c=yyextra->current->args.at(j))=='*' || isspace((uint8_t)c))) j++;
    if (j>0)
    {
      oldStyleArgPtr=yyextra->current->args.left(j);
      oldStyleArgName=yyextra->current->args.mid(j).stripWhiteSpace();
    }
    else
    {
      oldStyleArgName=yyextra->current->args.stripWhiteSpace();
    }
  }
}

//-----------------------------------------------------------------------------

/*! Update the argument \a name with additional \a type info. For K&R style
 *  function the type is found \e after the argument list, so this routine
 *  in needed to fix up.
 */
static void addKnRArgInfo(yyscan_t yyscanner,const QCString &type,const QCString &name,
                          const QCString &brief,const QCString &docs)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (Argument &a : yyextra->current->argList)
  {
    if (a.type==name)
    {
      a.type=type.stripWhiteSpace();
      a.type.stripPrefix("register ");
      a.name=name.stripWhiteSpace();
      if (!brief.isEmpty() && !docs.isEmpty())
      {
        a.docs=brief+"\n\n"+docs;
      }
      else if (!brief.isEmpty())
      {
        a.docs=brief;
      }
      else
      {
        a.docs=docs;
      }
    }
  }
}

//-----------------------------------------------------------------------------

void fixArgumentListForJavaScript(ArgumentList &al)
{
  for (Argument &a : al)
  {
    if (!a.type.isEmpty() && a.name.isEmpty())
    { // a->type is actually the (typeless) parameter name, so move it
      a.name=a.type;
      a.type.clear();
    }
  }
}

//-----------------------------------------------------------------------------

static void startCommentBlock(yyscan_t yyscanner,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (brief)
  {
    yyextra->current->briefFile = yyextra->fileName;
    yyextra->current->briefLine = yyextra->yyLineNr;
  }
  else
  {
    yyextra->current->docFile = yyextra->fileName;
    yyextra->current->docLine = yyextra->yyLineNr;
  }
}

//----------------------------------------------------------------------------

static void newEntry(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->tempEntry==0) // if temp entry is not 0, it holds yyextra->current,
                    // and yyextra->current is actually replaced by yyextra->previous which was
                    // already added to yyextra->current_root, so we should not add it again
                    // (see bug723314)
  {
    yyextra->previous = yyextra->current;
    yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  }
  else
  {
    yyextra->previous = yyextra->current;
    yyextra->current  = yyextra->tempEntry;
    yyextra->tempEntry.reset();
  }
  initEntry(yyscanner);
}

static void handleCommentBlock(yyscan_t yyscanner,const QCString &doc,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  AUTO_TRACE("doc='{}' is_brief={}",Trace::trunc(doc),brief);
  bool hideInBodyDocs = Config_getBool(HIDE_IN_BODY_DOCS);
  if (yyextra->docBlockInBody && hideInBodyDocs) return;
  int lineNr = brief ? yyextra->current->briefLine : yyextra->current->docLine;   // line of block start

  // fill in inbodyFile && inbodyLine the first time, see bug 633891
  std::shared_ptr<Entry> docEntry = yyextra->docBlockInBody && yyextra->previous ? yyextra->previous : yyextra->current;
  if (yyextra->docBlockInBody && docEntry && docEntry->inbodyLine==-1)
  {
    docEntry->inbodyFile = yyextra->fileName;
    docEntry->inbodyLine = lineNr;
  }

  int position=0;
  bool needsEntry=FALSE;
  GuardedSectionStack guards;
  Markdown markdown(yyextra->fileName,lineNr);
  QCString strippedDoc = stripIndentation(doc);
  QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,lineNr) : strippedDoc;
  while (yyextra->commentScanner.parseCommentBlock(
        yyextra->thisParser,
        yyextra->docBlockInBody && yyextra->previous ? yyextra->previous.get() : yyextra->current.get(),
        processedDoc, // text
        yyextra->fileName,   // file
        lineNr,       // line of block start
        yyextra->docBlockInBody ? FALSE : brief,               // isBrief
        yyextra->docBlockInBody ? FALSE : yyextra->docBlockAutoBrief,   // isJavaDocStyle
        yyextra->docBlockInBody,                               // isInBody
        yyextra->protection,
        position,
        needsEntry,
        Config_getBool(MARKDOWN_SUPPORT),
        &guards
        )
     )
  {
    //printf("parseCommentBlock position=%d [%s]\n",position,qPrint(doc)+position);
    if (needsEntry)
    {
      QCString docFile = yyextra->current->docFile;
      newEntry(yyscanner);
      yyextra->current->docFile = docFile;
      yyextra->current->docLine = lineNr;
    }
  }
  if (needsEntry)
  {
    newEntry(yyscanner);
  }

  if (yyextra->docBlockTerm)
  {
    unput(yyextra->docBlockTerm);
    yyextra->docBlockTerm=0;
  }
}

static void handleParametersCommentBlocks(yyscan_t yyscanner,ArgumentList &al)
{
  AUTO_TRACE();
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (Argument &a : al)
  {
    AUTO_TRACE_ADD("Param '{}' docs='{}'",a.name,Trace::trunc(a.docs));
    if (!a.docs.isEmpty())
    {
      if  (a.name.isEmpty() && a.type == "...") a.name= "...";
      int position=0;
      bool needsEntry;

      // save context
      QCString orgDoc   = yyextra->current->doc;
      QCString orgBrief = yyextra->current->brief;
      int orgDocLine    = yyextra->current->docLine;
      int orgBriefLine  = yyextra->current->briefLine;

      yyextra->current->doc.clear();
      yyextra->current->brief.clear();

      //printf("handleParametersCommentBlock [%s]\n",qPrint(doc));
      int lineNr = orgDocLine;
      GuardedSectionStack guards;
      Markdown markdown(yyextra->fileName,lineNr);
      QCString strippedDoc = stripIndentation(a.docs);
      QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,lineNr) : strippedDoc;
      while (yyextra->commentScanner.parseCommentBlock(
             yyextra->thisParser,
             yyextra->current.get(),
             processedDoc,     // text
             yyextra->fileName,         // file
             lineNr,
             FALSE,
             FALSE,
             FALSE,
             yyextra->protection,
             position,
             needsEntry,
             Config_getBool(MARKDOWN_SUPPORT),
             &guards
            )
          )
      {
        //printf("handleParametersCommentBlock position=%d [%s]\n",position,qPrint(doc)+position);
        if (needsEntry) newEntry(yyscanner);
      }
      if (needsEntry)
      {
        newEntry(yyscanner);
      }
      a.docs = yyextra->current->doc;

      // restore context
      yyextra->current->doc       = orgDoc;
      yyextra->current->brief     = orgBrief;
      yyextra->current->docLine   = orgDocLine;
      yyextra->current->briefLine = orgBriefLine;
    }
  }
}


//----------------------------------------------------------------------------

static void parseCompounds(yyscan_t yyscanner,const std::shared_ptr<Entry> &rt)
{
  AUTO_TRACE("name={}",rt->name);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (const auto &ce : rt->children())
  {
    if (!ce->program.empty())
    {
      AUTO_TRACE_ADD("compound name='{}' program='{}'",Trace::trunc(ce->name),Trace::trunc(ce->program.str()));
      // init scanner state
      yyextra->padCount=0;
      yyextra->column=0;
      yyextra->programStr = ce->program.str();
      yyextra->inputString = yyextra->programStr.data();
      yyextra->inputPosition = 0;
      if (ce->section.isEnum() || ce->spec.isEnum())
        BEGIN( FindFields ) ;
      else
        BEGIN( FindMembers ) ;
      yyextra->current_root = ce;
      yyextra->fileName = ce->fileName;
      //setContext();
      yyextra->yyLineNr = ce->bodyLine;
      yyextra->yyColNr = ce->bodyColumn;
      yyextra->insideObjC = ce->lang==SrcLangExt::ObjC;
      //printf("---> Inner block starts at line %d objC=%d\n",yyextra->yyLineNr,yyextra->insideObjC);
      yyextra->current = std::make_shared<Entry>();
      yyextra->isStatic = FALSE;
      initEntry(yyscanner);

      // deep copy group list from parent (see bug 727732)
      bool autoGroupNested = Config_getBool(GROUP_NESTED_COMPOUNDS);
      if (autoGroupNested && !rt->groups.empty() && !ce->section.isEnum() && !ce->spec.isEnum())
      {
        ce->groups = rt->groups;
      }

      int ni=ce->name.findRev("::"); if (ni==-1) ni=0; else ni+=2;
      // set default protection based on the compound type
      if ( ce->section.isClass() ) // class
      {
        if (yyextra->insidePHP || yyextra->insideD || yyextra->insideJS || yyextra->insideIDL || yyextra->insideSlice)
        {
          yyextra->current->protection = yyextra->protection = Protection::Public ;
        }
        else if (yyextra->insideJava)
        {
          yyextra->current->protection = yyextra->protection = (ce->spec.isInterface() || ce->spec.isEnum()) ?  Protection::Public : Protection::Package;
        }
        else if (ce->spec.isInterface() || ce->spec.isRef() || ce->spec.isValue() || ce->spec.isStruct() || ce->spec.isUnion())
        {
          if (ce->lang==SrcLangExt::ObjC)
          {
            yyextra->current->protection = yyextra->protection = Protection::Protected ;
          }
          else
          {
            yyextra->current->protection = yyextra->protection = Protection::Public ;
          }
        }
        else
        {
          yyextra->current->protection = yyextra->protection = Protection::Private ;
        }
      }
      else if (ce->section.isEnum() ) // enum
      {
        yyextra->current->protection = yyextra->protection = ce->protection;
      }
      else if (!ce->name.isEmpty() && ce->name.at(ni)=='@') // unnamed union or namespace
      {
        if (ce->section.isNamespace() ) // unnamed namespace
        {
          yyextra->current->isStatic = yyextra->isStatic = TRUE;
        }
        yyextra->current->protection = yyextra->protection = ce->protection;
        yyextra->current->exported = yyextra->exported = false;
      }
      else if (ce->section.isNamespace() )
      {
        yyextra->current->protection = yyextra->protection = Protection::Public ;
        yyextra->current->exported = yyextra->exported = ce->exported;
      }
      else // named struct, union, protocol, category
      {
        yyextra->current->protection = yyextra->protection = Protection::Public ;
        yyextra->current->exported = yyextra->exported = false;
      }
      yyextra->mtype = MethodTypes::Method;
      yyextra->virt = Specifier::Normal;
      //printf("name=%s yyextra->current->isStatic=%d yyextra->isStatic=%d\n",qPrint(ce->name),yyextra->current->isStatic,yyextra->isStatic);

      //memberGroupId = DOX_NOGROUP;
      //memberGroupRelates.clear();
      //memberGroupInside.clear();
      QCString name = ce->name;
      yyextra->commentScanner.enterCompound(yyextra->fileName,yyextra->yyLineNr,name);

      scannerYYlex(yyscanner);
      //forceEndGroup();

      yyextra->commentScanner.leaveCompound(yyextra->fileName,yyextra->yyLineNr,name);

      yyextra->programStr.clear();
      ce->program.str(std::string());


      //if (depthIf>0)
      //{
      //        warn(yyextra->fileName,yyextra->yyLineNr,"Documentation block ended in the middle of a conditional section!");
      //}
    }
    parseCompounds(yyscanner,ce);
  }
}

static void parseConcepts(yyscan_t yyscanner,const std::shared_ptr<Entry> &rt)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  for (const auto &ce : rt->children())
  {
    if (ce->section.isConcept())
    {
      //printf("*** found concept '%s' value=[[\n%s\n]] line=%d,col=%d\n",
      //    qPrint(ce->name),qPrint(ce->initializer.str()),ce->startLine,ce->startColumn);
      yyextra->commentScanner.enterCompound(yyextra->fileName,yyextra->yyLineNr,ce->name);
      // init scanner state
      yyextra->padCount      =0;
      yyextra->column        =0;
      yyextra->programStr    = ce->initializer.str();
      yyextra->inputString   = yyextra->programStr.data();
      yyextra->inputPosition = 0;
      yyextra->current_root  = ce;
      yyextra->fileName      = ce->fileName;
      yyextra->yyLineNr      = ce->bodyLine;
      yyextra->yyColNr       = ce->bodyColumn;
      yyextra->insideObjC    = false;
      yyextra->current       = std::make_shared<Entry>();

      // hack to reconstruct what was in the code before the concept expression
      QCString indent;
      indent.fill(' ',std::max(0,ce->startColumn-1));
      QCString templArgs;
      if (!ce->args.isEmpty())
      {
        templArgs=indent+"template"+ce->args+"\n";
      }
      yyextra->current->initializer.str(QCString(templArgs+indent+"concept "+ce->name+ " =").str());

      yyextra->isStatic      = FALSE;
      initEntry(yyscanner);
      yyextra->current->section = EntryType::makeConceptCodePart();
      yyextra->current->startLine = yyextra->yyLineNr;
      BEGIN( FindConceptParts );
      scannerYYlex(yyscanner);
      yyextra->commentScanner.leaveCompound(yyextra->fileName,yyextra->yyLineNr,ce->name);

    }
    parseConcepts(yyscanner,ce);
  }
}

//----------------------------------------------------------------------------

static void parseMain(yyscan_t yyscanner,
                      const QCString &fileName,
                      const char *fileBuf,
                      const std::shared_ptr<Entry> &rt,
                      ClangTUParser *clangParser)
{
  AUTO_TRACE("fileName={}",fileName);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  initParser(yyscanner);

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;
  yyextra->column = 0;
  scannerYYrestart(nullptr,yyscanner);

  //depthIf       = 0;
  yyextra->protection    = Protection::Public;
  yyextra->mtype         = MethodTypes::Method;
  yyextra->isStatic      = FALSE;
  yyextra->exported      = false;
  yyextra->virt          = Specifier::Normal;
  yyextra->current_root  = rt;
  yyextra->yyLineNr      = 1 ;
  yyextra->yyBegLineNr   = 1;
  yyextra->yyBegColNr    = 0;
  yyextra->anonCount     = 0;
  yyextra->anonNSCount   = 0;
  yyextra->fileName = fileName;
  yyextra->clangParser = clangParser;
  setContext(yyscanner);
  rt->lang = yyextra->language;
  msg("Parsing file {}...\n",yyextra->fileName);

  yyextra->current_root  = rt;
  initParser(yyscanner);
  yyextra->commentScanner.enterFile(yyextra->fileName,yyextra->yyLineNr);
  yyextra->current = std::make_shared<Entry>();
  //printf("yyextra->current=%p yyextra->current_root=%p\n",yyextra->current,yyextra->current_root);
  EntryType sec=guessSection(yyextra->fileName);
  if (!sec.isEmpty())
  {
    yyextra->current->name    = yyextra->fileName;
    yyextra->current->section = sec;
    yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
  }
  yyextra->current->reset();
  initEntry(yyscanner);
  if ( yyextra->insidePHP )
  {
    BEGIN( FindMembersPHP );
  }
  else if ( yyextra->insideJava ) // add default java.lang package scope
  {
    yyextra->current->name="java::lang"; // '::' is used in doxygen's internal representation as a scope separator
    yyextra->current->fileName = yyextra->fileName;
    yyextra->current->section = EntryType::makeUsingDir();
    yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
    initEntry(yyscanner);
    BEGIN( FindMembers );
  }
  else
  {
    BEGIN( FindMembers );
  }

  scannerYYlex(yyscanner);

  if (YY_START==Comment)
  {
    warn(yyextra->fileName,yyextra->yyLineNr,"File ended in the middle of a comment block! Perhaps a missing \\endcode?");
  }

  //forceEndGroup();
  yyextra->commentScanner.leaveFile(yyextra->fileName,yyextra->yyLineNr);

  yyextra->programStr.clear();
  rt->program.str(std::string());

  parseCompounds(yyscanner,rt);
  parseConcepts(yyscanner,rt);

  yyextra->anonNSCount++;

  // add additional entries that were created during processing
  for (auto &[parent,child]: yyextra->outerScopeEntries)
  {
    //printf(">>> adding '%s' to scope '%s'\n",qPrint(child->name),qPrint(parent->name));
    parent->moveToSubEntryAndKeep(child);
  }
  yyextra->outerScopeEntries.clear();

}

//----------------------------------------------------------------------------

static void parsePrototype(yyscan_t yyscanner,const QCString &text)
{
  AUTO_TRACE("text='{}'",Trace::trunc(text));
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text.isEmpty())
  {
    warn(yyextra->fileName,yyextra->yyLineNr,"Empty prototype found!");
    return;
  }
  if (!yyextra->current) // nothing to store (see bug683516)
  {
    return;
  }

  const char *orgInputString;
  int orgInputPosition;
  YY_BUFFER_STATE orgState;

  // save scanner state
  orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(nullptr, YY_BUF_SIZE, yyscanner), yyscanner);
  orgInputString = yyextra->inputString;
  orgInputPosition = yyextra->inputPosition;

  // set new string
  yyextra->inputString = text.data();
  yyextra->inputPosition = 0;
  yyextra->column = 0;
  scannerYYrestart(nullptr, yyscanner);
  BEGIN(Prototype);
  scannerYYlex(yyscanner);

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


  //printf("**** parsePrototype end\n");
}

//----------------------------------------------------------------------------

struct COutlineParser::Private
{
  yyscan_t yyscanner;
  scannerYY_state state;
};

COutlineParser::COutlineParser() : p(std::make_unique<COutlineParser::Private>())
{
  scannerYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  scannerYYset_debug(Debug::isFlagSet(Debug::Lex_scanner)?1:0,p->yyscanner);
#endif
}

COutlineParser::~COutlineParser()
{
  scannerYYlex_destroy(p->yyscanner);
}

void COutlineParser::parseInput(const QCString &fileName,
                                const char *fileBuf,
                                const std::shared_ptr<Entry> &root,
                                ClangTUParser *clangParser)
{
  AUTO_TRACE();
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->thisParser = this;

  DebugLex debugLex(Debug::Lex_scanner, __FILE__, qPrint(fileName));

  ::parseMain(p->yyscanner,fileName,fileBuf,root,clangParser);
}


bool COutlineParser::needsPreprocessing(const QCString &extension) const
{
  QCString fe=extension.lower();
  SrcLangExt lang = getLanguageFromFileName(extension);
  return (SrcLangExt::Cpp == lang) || (SrcLangExt::Lex == lang) ||
   !( fe==".java" || fe==".as"  || fe==".d"    || fe==".php" ||
      fe==".php4" || fe==".inc" || fe==".phtml"|| fe==".php5"
    );
}

void COutlineParser::parsePrototype(const QCString &text)
{
  ::parsePrototype(p->yyscanner,text);
}

//----------------------------------------------------------------------------

#include "scanner.l.h"
