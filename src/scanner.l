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
}

%{

/*
 *      includes
 */

#include <algorithm>
#include <vector>
#include <utility>
#include <atomic>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

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

#include "clangparser.h"
#include "markdown.h"
#include "regex.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

static AtomicInt  anonCount;
static AtomicInt  anonNSCount;

struct scannerYY_state
{
  OutlineParserInterface *thisParser;
  CommentScanner   commentScanner;
  const char *     inputString = 0;
  int              inputPosition = 0;
  int              lastContext = 0;
  int              lastCContext = 0;
  int              lastDocContext = 0;
  int              lastCPPContext = 0;
  int              lastSkipSharpContext = 0;
  int              lastSkipRoundContext = 0;
  int              lastStringContext = 0;
  int              lastCurlyContext = 0;
  int              lastRoundContext = 0;
  int              lastSharpContext = 0;
  int              lastSquareContext = 0;
  int              lastInitializerContext = 0;
  int              lastClassTemplSpecContext = 0;
  int              lastPreLineCtrlContext = 0;
  int              lastSkipVerbStringContext = 0;
  int              lastCommentInArgContext = 0;
  int              lastRawStringContext = 0;
  int              lastCSConstraint = 0;
  int              lastHereDocContext = 0;
  int              lastDefineContext = 0;
  int              lastAlignAsContext = 0;
  int              lastC11AttributeContext = 0;
  int              lastModifierContext = 0;
  Protection       protection = Public;
  Protection       baseProt = Public;
  int              sharpCount   = 0 ;
  int              roundCount   = 0 ;
  int              curlyCount   = 0 ;
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
  QCString         yyFileName;
  MethodTypes      mtype = Method;
  bool             stat = false;
  Specifier        virt = Normal;
  Specifier        baseVirt = Normal;
  QCString         msType;
  QCString         msName;
  QCString         msArgs;
  bool             isTypedef = false;
  QCString         funcPtrType;
  QCString         templateStr;
  QCString         aliasName;
  QCString         baseName;
  QCString*        specName = 0;

  SrcLangExt       language = SrcLangExt_Unknown;
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

  bool             insideCppQuote = false;
  bool             insideProtocolList = false;

  int              argRoundCount = 0;
  int              argSharpCount = 0;
  int              currentArgumentContext = 0;
  int              lastCopyArgStringContext = 0;
  int              lastCopyArgContext = 0;
  int              requiresContext = 0;
  QCString        *copyArgString = 0;
  QCString         fullArgString;
  QCString         dummyRawString;

  ArgumentList    *currentArgumentList = 0;
  char             lastCopyArgChar = '\0';

  QCString *pCopyQuotedString = 0;
  QCString *pCopyRoundString = 0;
  QCString *pCopyCurlyString = 0;
  QCString *pCopySharpString = 0;
  QCString *pCopyRawString = 0;
  TextStream *pCopyCurlyGString = 0;
  TextStream *pCopyRoundGString = 0;
  TextStream *pCopySquareGString = 0;
  TextStream *pCopyQuotedGString = 0;
  TextStream *pCopyHereDocGString = 0;
  TextStream *pCopyRawGString = 0;
  TextStream *pSkipVerbString = 0;

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
  int              docBlockLine = 0;
  bool             docBlockInBody = false;
  bool             docBlockAutoBrief = false;
  char             docBlockTerm = '\0';

  QCString         idlAttr;
  QCString         idlProp;
  bool             odlProp = false;

  bool             lexInit = false;
  bool             externC = false;

  QCString         delimiter;

  int              column = 0;

  uint             fencedSize = 0;
  bool             nestedComment = false;
  std::vector< std::pair<Entry*,std::shared_ptr<Entry> > > outerScopeEntries;
  QCString         programStr;

  ClangTUParser * clangParser = 0;
};

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif
//-----------------------------------------------------------------------------

// forward declarations for stateless functions
static inline int computeIndent(const char *s,int startIndent);
static QCString stripQuotes(const char *s);
static bool nameIsOperator(QCString &name);
void fixArgumentListForJavaScript(ArgumentList &al);

// forward declarations for statefull functions
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
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);


/* ----------------------------------------------------------------- */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

%}

       /* start command character */
CMD       ("\\"|"@")
BN        [ \t\n\r]
BNopt     {BN}*
BL        [ \t\r]*"\n"
B         [ \t]
Bopt      {B}*
ID        "$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
SCOPENAME "$"?(({ID}?{BN}*"::"{BN}*)*)(((~|!){BN}*)?{ID})
TSCOPE    {ID}("<"[a-z_A-Z0-9 \t\*\&,:]*">")?
CSSCOPENAME (({ID}?{BN}*"."{BN}*)*)((~{BN}*)?{ID})
PRE       [pP][rR][eE]
CODE      [cC][oO][dD][eE]
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))
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

  /* no comment start / end signs inside square brackets */
NCOMM [^/\*]
  // C start comment 
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment 
CPPC  "/\/"
  // doxygen start comment
DCOMM ("/\*!"|"/\**"|"/\/!"|"/\/\/")

  // Optional any character
ANYopt .*
  // Optional all but newline
NONLopt [^\n]*

%option noyywrap

  /* language parsing states */

%x      AlignAs
%x      AlignAsEnd
%x      Define
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
%x      FindFieldArg
%x      Function
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
%x      Using
%x      UsingAlias
%x      UsingAliasEnd
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
%x      SkipCPPBlock
%x      SkipComment
%x      SkipCxxComment
%x      SkipCurlyBlock
%x      SkipRoundBlock
%x      Sharp
%x      SkipRound
%x      SkipSquare
%x      SkipRemainder
%x      StaticAssert
%x      DeclType
%x      TypedefName
%x      TryFunctionBlock
%x      TryFunctionBlockEnd
%x      Comment
%x      PackageName
%x      JavaImport
%x      PHPUse
%x      PHPUseAs
%x      CSAccessorDecl
%x      CSGeneric
%x      PreLineCtrl
%x      DefinePHP
%x      DefinePHPEnd
%x      OldStyleArgs
%x      SkipVerbString
%x      ObjCMethod
%x      ObjCReturnType
%x      ObjCParams
%x      ObjCParamType
%x      ObjCProtocolList
%x      ObjCPropAttr
%x      ObjCSkipStatement
%x      QtPropType
%x      QtPropName
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
%x      GCopyRound
%x      GCopySquare
%x      GCopyCurly
%x      SkipUnionSwitch
%x      Specialization
%x      SpecializationSingleQuote
%x      SpecializationDoubleQuote
%x      FuncPtrInit
%x      FuncFunc
%x      FuncFuncEnd
%x      FuncFuncType
%x      FuncFuncArray
%x      CopyArgString
%x      CopyArgPHPString
%x      CopyArgRound
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

%%

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
                                          yyextra->current->mtype = yyextra->mtype = Property;
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }

<FindMembers>{B}*"k_dcop"{BN}*":"{BN}*  { yyextra->current->mtype = yyextra->mtype = DCOP;
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }

<FindMembers>{B}*("signals"|"Q_SIGNALS"){BN}*":"{BN}* { yyextra->current->mtype = yyextra->mtype = Signal;

                                          yyextra->current->protection = yyextra->protection = Public ;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }

<FindMembers>{B}*"public"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          yyextra->current->mtype = yyextra->mtype = Slot;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner);
                                        }

<FindMembers>{B}*"protected"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
                                          yyextra->current->protection = yyextra->protection = Protected ;
                                          yyextra->current->mtype = yyextra->mtype = Slot;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner);
                                        }

<FindMembers>{B}*"private"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
                                          yyextra->current->protection = yyextra->protection = Private ;
                                          yyextra->current->mtype = yyextra->mtype = Slot;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*("public"|"methods"|"__published"){BN}*":"{BN}* {
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          yyextra->current->mtype = yyextra->mtype = Method;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }
<FindMembers>{B}*"internal"{BN}*":"{BN}* {  // for now treat C++/CLI's internal as package...
                                          if (yyextra->insideCli)
                                          {
                                            yyextra->current->protection = yyextra->protection = Package ;
                                            yyextra->current->mtype = yyextra->mtype = Method;
                                            yyextra->current->type.resize(0);
                                            yyextra->current->name.resize(0);
                                            yyextra->current->args.resize(0);
                                            yyextra->current->argList.clear();
                                            lineCount(yyscanner) ;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"protected"{BN}*":"{BN}* {
                                          yyextra->current->protection = yyextra->protection = Protected ;
                                          yyextra->current->mtype = yyextra->mtype = Method;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }
<FindMembers>{B}*"private"{BN}*":"{BN}* {
                                          yyextra->current->protection = yyextra->protection = Private ;
                                          yyextra->current->mtype = yyextra->mtype = Method;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }
<FindMembers>{B}*"event"{BN}+           {
                                          if (yyextra->insideCli)
                                          {
                                            // C++/CLI event
                                            lineCount(yyscanner) ;
                                            yyextra->current->mtype = yyextra->mtype = Event;
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( CliPropertyType );
                                          }
                                          else if (yyextra->insideCS)
                                          {
                                            lineCount(yyscanner) ;
                                            yyextra->current->mtype = Event;
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
                                             yyextra->current->mtype = yyextra->mtype = Property;
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
                                            yyextra->current->mtype = yyextra->mtype = Property;
                                            lineCount(yyscanner);
                                          }
                                        }
  */
<FindMembers>{B}*"@private"{BN}+        {
                                          yyextra->current->protection = yyextra->protection = Private ;
                                          yyextra->current->mtype = yyextra->mtype = Method;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }
<FindMembers>{B}*"@protected"{BN}+      {
                                          yyextra->current->protection = yyextra->protection = Protected ;
                                          yyextra->current->mtype = yyextra->mtype = Method;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }
<FindMembers>{B}*"@public"{BN}+ {
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          yyextra->current->mtype = yyextra->mtype = Method;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->name.resize(0);
                                          yyextra->current->args.resize(0);
                                          yyextra->current->argList.clear();
                                          lineCount(yyscanner) ;
                                        }
<FindMembers>[\-+]{BN}*                 {
                                          if (!yyextra->insideObjC)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->fileName  = yyextra->yyFileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->bodyLine  = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->current->section = Entry::FUNCTION_SEC;
                                            yyextra->current->protection = yyextra->protection = Public ;
                                            yyextra->language = yyextra->current->lang = SrcLangExt_ObjC;
                                            yyextra->insideObjC = TRUE;
                                            yyextra->current->virt = Virtual;
                                            yyextra->current->stat=yytext[0]=='+';
                                            yyextra->current->mtype = yyextra->mtype = Method;
                                            yyextra->current->type.resize(0);
                                            yyextra->current->name.resize(0);
                                            yyextra->current->args.resize(0);
                                            yyextra->current->argList.clear();
                                            BEGIN( ObjCMethod );
                                          }
                                        }
<ObjCMethod>"("                         { // start of method's return type
                                          BEGIN( ObjCReturnType );
                                        }
<ObjCMethod>{ID}                        { // found method name
                                          if (yyextra->current->type.isEmpty())
                                          {
                                            yyextra->current->type = "id";
                                          }
                                          yyextra->current->name = yytext;
                                          if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
                                          {
                                            yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,yytext);
                                          }
                                        }
<ObjCMethod>":"{B}*                     { // start of parameter list
                                          yyextra->current->name += ':';
                                          Argument a;
                                          yyextra->current->argList.push_back(a);
                                          BEGIN( ObjCParams );
                                        }
<ObjCReturnType>[^)]*                   { // TODO: check if nested braches are possible.
                                          yyextra->current->type = yytext;
                                        }
<ObjCReturnType>")"                     {
                                          BEGIN( ObjCMethod );
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
                                          yyextra->current->argList.back().type.resize(0);
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
                                          BEGIN( Function );
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
                                          BEGIN( Function );
                                        }
<FindMembers>{B}*"sequence"{BN}*"<"{BN}* {
                                          if (yyextra->insideSlice)
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->current->fileName = yyextra->yyFileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->args.resize(0);
                                            yyextra->current->section = Entry::TYPEDEF_SEC ;
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
                                            yyextra->current->fileName = yyextra->yyFileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->args.resize(0);
                                            yyextra->current->section = Entry::TYPEDEF_SEC ;
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
                                          else if (qstrncmp(yytext,"@property",9)==0) // ObjC 2.0 property
                                          {
                                            yyextra->current->mtype = yyextra->mtype = Property;
                                            yyextra->current->spec|=Entry::Readable | Entry::Writable | Entry::Assign;
                                            yyextra->current->protection = Public ;
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
                                          yyextra->current->spec&=~Entry::Writable;
                                        }
<ObjCPropAttr>"readwrite"               { // default
                                        }
<ObjCPropAttr>"assign"                  { // default
                                        }
<ObjCPropAttr>"unsafe_unretained"       {
                                          yyextra->current->spec&=~Entry::Assign;
                                          yyextra->current->spec|=Entry::Unretained;
                                        }
<ObjCPropAttr>"retain"                  {
                                          yyextra->current->spec&=~Entry::Assign;
                                          yyextra->current->spec|=Entry::Retain;
                                        }
<ObjCPropAttr>"copy"                    {
                                          yyextra->current->spec&=~Entry::Assign;
                                          yyextra->current->spec|=Entry::Copy;
                                        }
<ObjCPropAttr>"weak"                    {
                                          yyextra->current->spec&=~Entry::Assign;
                                          yyextra->current->spec|=Entry::Weak;
                                        }
<ObjCPropAttr>"strong"                  {
                                          yyextra->current->spec&=~Entry::Assign;
                                          yyextra->current->spec|=Entry::Strong;
                                        }
<ObjCPropAttr>"nonatomic"               {
                                          yyextra->current->spec|=Entry::NonAtomic;
                                        }
<ObjCPropAttr>")"                       {
                                          BEGIN(FindMembers);
                                        }
<FindMembers>"@"{ID}                    {
                                          if (yyextra->insideJava) // Java annotation
                                          {
                                            // skip annotation
                                          }
                                          else if (qstrcmp(yytext,"@property")==0) // ObjC 2.0 property
                                          {
                                            yyextra->current->mtype = yyextra->mtype = Property;
                                            yyextra->current->spec|=Entry::Writable | Entry::Readable;
                                            yyextra->current->protection = Public ;
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
                                          yyextra->current->section = Entry::NAMESPACE_SEC;
                                          yyextra->current->type = "namespace" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner);
                                        }
<PackageName>";"                        {
                                          std::shared_ptr<Entry> tmp = yyextra->current;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          yyextra->current_root = tmp;
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }
<PackageName>"{"                        {
                                          yyextra->curlyCount=0;
                                          BEGIN( ReadNSBody );
                                        }
<FindMembers>{B}*"initonly"{BN}+        {
                                          yyextra->current->type += " initonly ";
                                          if (yyextra->insideCli) yyextra->current->spec |= Entry::Initonly;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"static"{BN}+          { yyextra->current->type += " static ";
                                          yyextra->current->stat = TRUE;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"extern"{BN}+          {
                                          yyextra->current->stat = FALSE;
                                          yyextra->current->explicitExternal = TRUE;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"const"{BN}+           { if (yyextra->insideCS)
                                          {
                                            yyextra->current->type += " const ";
                                            if (yyextra->insideCS) yyextra->current->stat = TRUE;
                                            lineCount(yyscanner);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"virtual"{BN}+         { yyextra->current->type += " virtual ";
                                          yyextra->current->virt = Virtual;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"constexpr"{BN}+       {
                                          if (yyextra->insideCpp)
                                          {
                                            yyextra->current->type += " constexpr ";
                                            yyextra->current->spec |= Entry::ConstExpr;
                                            lineCount(yyscanner);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"published"{BN}+       { // UNO IDL published keyword
                                          if (yyextra->insideIDL)
                                          {
                                            lineCount(yyscanner);
                                            yyextra->current->spec |= Entry::Published;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<FindMembers>{B}*"abstract"{BN}+        {
                                          if (!yyextra->insidePHP)
                                          {
                                            yyextra->current->type += " abstract ";
                                            if (!yyextra->insideJava)
                                            {
                                              yyextra->current->virt = Pure;
                                            }
                                            else
                                            {
                                              yyextra->current->spec|=Entry::Abstract;
                                            }
                                          }
                                          else
                                          {
                                            yyextra->current->spec|=Entry::Abstract;
                                          }
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"inline"{BN}+          { yyextra->current->spec|=Entry::Inline;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"mutable"{BN}+         { yyextra->current->spec|=Entry::Mutable;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"explicit"{BN}+        { yyextra->current->spec|=Entry::Explicit;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"local"{BN}+           { yyextra->current->spec|=Entry::Local;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"@required"{BN}+       { // Objective C 2.0 protocol required section
                                          yyextra->current->spec=(yyextra->current->spec & ~Entry::Optional) | Entry::Required;
                                          lineCount(yyscanner);
                                        }
<FindMembers>{B}*"@optional"{BN}+       {  // Objective C 2.0 protocol optional section
                                          yyextra->current->spec=(yyextra->current->spec & ~Entry::Required) | Entry::Optional;
                                          lineCount(yyscanner);
                                        }
  /*
<FindMembers>{B}*"import"{BN}+          { // IDL import keyword
                                          BEGIN( NextSemi );
                                        }
  */
<FindMembers>{B}*"typename"{BN}+        { lineCount(yyscanner); }
<FindMembers>{B}*"namespace"{BNopt}/[^a-z_A-Z0-9] {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::NAMESPACE_SEC;
                                          yyextra->current->type = "namespace" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::NAMESPACE_SEC;
                                            yyextra->current->type = "module" ;
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::NAMESPACE_SEC;
                                            yyextra->current->type = "library" ;
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::NAMESPACE_SEC;
                                            yyextra->current->type = "constants";
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::CLASS_SEC;
                                            yyextra->current->spec = Entry::Service |
                                              // preserve UNO IDL [optional] or published
                                              (yyextra->current->spec & (Entry::Optional|Entry::Published));
                                            addType(yyscanner);
                                            yyextra->current->type += " service " ;
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::CLASS_SEC;
                                            yyextra->current->spec = Entry::Singleton |
                                              (yyextra->current->spec & Entry::Published); // preserve
                                            addType(yyscanner);
                                            yyextra->current->type += " singleton " ;
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::CLASS_SEC;
                                            yyextra->current->spec = Entry::Interface |
                                                // preserve UNO IDL [optional], published, Slice local
                                                (yyextra->current->spec & (Entry::Optional|Entry::Published|Entry::Local));
                                            addType(yyscanner);
                                            yyextra->current->type += " interface" ;
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
<FindMembers>{B}*"@implementation"{BN}+ { // Objective-C class implementation
                                          lineCount(yyscanner);
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::OBJCIMPL_SEC;
                                          yyextra->language = yyextra->current->lang = SrcLangExt_ObjC;
                                          yyextra->insideObjC = TRUE;
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          addType(yyscanner);
                                          yyextra->current->type += " implementation" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          BEGIN( CompoundName );
                                        }
<FindMembers>{B}*"@interface"{BN}+      { // Objective-C class interface, or Java attribute
                                          lineCount(yyscanner);
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec = Entry::Interface;
                                          if (!yyextra->insideJava)
                                          {
                                            yyextra->language = yyextra->current->lang = SrcLangExt_ObjC;
                                            yyextra->insideObjC = TRUE;
                                          }
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          addType(yyscanner);
                                          yyextra->current->type += " interface" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          BEGIN( CompoundName );
                                        }
<FindMembers>{B}*"@protocol"{BN}+       { // Objective-C protocol definition
                                          lineCount(yyscanner);
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec = Entry::Protocol;
                                          yyextra->language = yyextra->current->lang = SrcLangExt_ObjC;
                                          yyextra->insideObjC = TRUE;
                                          yyextra->current->protection = yyextra->protection = Public ;
                                          addType(yyscanner);
                                          yyextra->current->type += " protocol" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          BEGIN( CompoundName );
                                        }
<FindMembers>{B}*"exception"{BN}+       { // Corba IDL/Slice exception
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          // preserve UNO IDL, Slice local
                                          yyextra->current->spec    = Entry::Exception |
                                            (yyextra->current->spec & Entry::Published) |
                                            (yyextra->current->spec & Entry::Local);
                                          addType(yyscanner);
                                          yyextra->current->type += " exception" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          addType(yyscanner);
                                          uint64 spec = yyextra->current->spec;
                                          if (yyextra->insidePHP && yyextra->current->spec&Entry::Abstract)
                                          {
                                            // convert Abstract to AbstractClass
                                            yyextra->current->spec=(yyextra->current->spec&~Entry::Abstract)|Entry::AbstractClass;
                                          }
                                          if (yyextra->insideSlice && spec&Entry::Local)
                                          {
                                            yyextra->current->spec|=Entry::Local;
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
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          if (yytext[0]=='@')
                                          {
                                            yyextra->language = yyextra->current->lang = SrcLangExt_ObjC;
                                            yyextra->insideObjC = TRUE;
                                          }
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"value class{" |         // C++/CLI extension
<FindMembers>{B}*"value class"{BN}+     {
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec = Entry::Value;
                                          addType(yyscanner);
                                          yyextra->current->type += " value class" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec = Entry::Ref;
                                          addType(yyscanner);
                                          yyextra->current->type += " ref class" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec = Entry::Interface;
                                          addType(yyscanner);
                                          yyextra->current->type += " interface class" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::CLASS_SEC;
                                            addType(yyscanner);
                                            yyextra->current->type += " coclass" ;
                                            yyextra->current->fileName  = yyextra->yyFileName;
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
                                          QCString decl = yytext;
                                          yyextra->isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
                                          yyextra->current->section = Entry::CLASS_SEC ;
                                          // preserve UNO IDL & Inline attributes, Slice local
                                          yyextra->current->spec    = Entry::Struct |
                                            (yyextra->current->spec & Entry::Published) |
                                            (yyextra->current->spec & Entry::Inline) |
                                            (yyextra->current->spec & Entry::Local);
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
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec    = Entry::Struct | Entry::Value;
                                          addType(yyscanner);
                                          yyextra->current->type += " value struct" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec    = Entry::Struct | Entry::Ref;
                                          addType(yyscanner);
                                          yyextra->current->type += " ref struct" ;
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec    = Entry::Struct | Entry::Interface;
                                          addType(yyscanner);
                                          yyextra->current->type += " interface struct";
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                          QCString decl=yytext;
                                          yyextra->isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
                                          yyextra->current->section = Entry::CLASS_SEC;
                                          yyextra->current->spec    = Entry::Union;
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
                                          yyextra->current->fileName  = yyextra->yyFileName;
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
                                            yyextra->current->section = Entry::CLASS_SEC;
                                            yyextra->current->spec    = Entry::Enum;
                                          }
                                          else
                                          {
                                            yyextra->current->section = Entry::ENUM_SEC ;
                                          }
                                          addType(yyscanner);
                                          yyextra->current->type += " enum";
                                          if (isStrongEnum)
                                          {
                                            yyextra->current->spec |= Entry::Strong;
                                          }
                                          if (isEnumSytruct)
                                          {
                                            yyextra->current->spec |= Entry::Strong;
                                            yyextra->current->spec |= Entry::EnumStruct;
                                          }
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          lineCount(yyscanner) ;
                                          if (yytext[yyleng-1]=='{') unput('{');
                                          BEGIN( CompoundName ) ;
                                        }
<FindMembers>{B}*"concept"{BN}+         { // C++20 concept
                                          yyextra->isTypedef=FALSE;
                                          yyextra->current->section = Entry::CONCEPT_SEC;
                                          addType(yyscanner);
                                          yyextra->current->type += " concept";
                                          yyextra->current->fileName  = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
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
<Operator>"<>"                          { /* skip guided templ specifiers */ }
<Operator>"("                           {
                                          yyextra->current->name = yyextra->current->name.simplifyWhiteSpace();
                                          unput(*yytext);
                                          BEGIN( FindMembers ) ;
                                        }
<FindMembers>("template"|"generic")({BN}*)"<"/[>]?      {  // generic is a C++/CLI extension
                                          lineCount(yyscanner);
                                          ArgumentList al;
                                          //yyextra->current->spec |= (yytext[0]=='g') ? Entry::Generic : Entry::Template;
                                          yyextra->current->tArgLists.push_back(al);
                                          yyextra->currentArgumentList = &yyextra->current->tArgLists.back();
                                          yyextra->templateStr="<";
                                          yyextra->fullArgString = yyextra->templateStr;
                                          yyextra->copyArgString = &yyextra->templateStr;
                                          yyextra->currentArgumentContext = FindMembers;
                                          BEGIN( ReadTempArgs );
                                        }
<FindMembers>"namespace"{BN}+/{ID}{BN}*"=" { // namespace alias
                                          lineCount(yyscanner);
                                          BEGIN( NSAliasName );
                                        }
<NSAliasName>{ID}                       {
                                          yyextra->aliasName = yytext;
                                          BEGIN( NSAliasArg );
                                        }
<NSAliasArg>({ID}"::")*{ID}             {
                                          //printf("Inserting namespace alias %s::%s->%s\n",qPrint(yyextra->current_root->name),qPrint(yyextra->aliasName),yytext);
                                          // TODO: namespace aliases are now treated as global entities
                                          // while they should be aware of the scope they are in
                                          Doxygen::namespaceAliasMap.insert({yyextra->aliasName.str(),std::string(yytext)});
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
                                          yyextra->current->fileName = yyextra->yyFileName;
                                          // add a using declaration
                                          yyextra->current->section=Entry::USINGDECL_SEC;
                                          yyextra->current_root->copyToSubEntry(yyextra->current);
                                          // also add it as a using directive
                                          yyextra->current->section=Entry::USINGDIR_SEC;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          yyextra->aliasName.resize(0);
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
                                            Doxygen::namespaceAliasMap.insert({yytext,
                                                 std::string(removeRedundantWhiteSpace(
                                                   substitute(yyextra->aliasName,"\\","::")).str())});
                                          }
                                          yyextra->aliasName.resize(0);
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
                                          yyextra->current->fileName = yyextra->yyFileName;
                                          yyextra->current->section=Entry::USINGDIR_SEC;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(Using);
                                        }
<JavaImport>({ID}{BN}*"."{BN}*)+{ID}    { // class import => add as a using declaration
                                          lineCount(yyscanner);
                                          QCString scope=yytext;
                                          yyextra->current->name=removeRedundantWhiteSpace(substitute(scope,".","::"));
                                          yyextra->current->fileName = yyextra->yyFileName;
                                          if (yyextra->insideD)
                                          {
                                            yyextra->current->section=Entry::USINGDIR_SEC;
                                          }
                                          else
                                          {
                                            //printf("import name = %s -> %s\n",yytext,qPrint(yyextra->current->name));
                                            yyextra->current->section=Entry::USINGDECL_SEC;
                                          }
                                          yyextra->previous = yyextra->current;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(Using);
                                        }
<FindMembers>"using"{BN}+               {
                                          yyextra->current->startLine=yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          lineCount(yyscanner);
                                          BEGIN(Using);
                                        }
<Using>"namespace"{BN}+                 { lineCount(yyscanner); BEGIN(UsingDirective); }
<Using>({ID}{BN}*("::"|"."){BN}*)*({ID}|{OPERATOR}) {
                                          lineCount(yyscanner);
                                          yyextra->current->name=yytext;
                                          yyextra->current->fileName = yyextra->yyFileName;
                                          yyextra->current->section=Entry::USINGDECL_SEC;
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
                                            yyextra->current->fileName = yyextra->yyFileName;
                                            yyextra->current->startLine = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->section=Entry::USINGDIR_SEC;
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                            initEntry(yyscanner);
                                          }
                                          BEGIN(Using);
                                        }
<Using>"="                              { // C++11 style template alias?
                                          BEGIN(UsingAlias);
                                        }
<UsingAlias>";"                         {
                                          yyextra->previous->section=Entry::VARIABLE_SEC;
                                          yyextra->previous->type = "typedef "+yyextra->previous->args;
                                          yyextra->previous->type=yyextra->previous->type.simplifyWhiteSpace();
                                          yyextra->previous->args.resize(0);
                                          yyextra->previous->name=yyextra->previous->name.stripWhiteSpace();
                                          yyextra->previous->bodyLine = yyextra->yyLineNr;
                                          yyextra->previous->bodyColumn = yyextra->yyColNr;
                                          yyextra->previous->spec |= Entry::Alias;
                                          BEGIN(FindMembers);
                                        }
<UsingAlias>";"{BN}*{DCOMM}"<" {
                                          yyextra->docBlockContext   = UsingAliasEnd;
                                          yyextra->docBlockInBody    = FALSE;
                                          yyextra->docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
                                                              ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());
                                          lineCount(yyscanner);

                                          yyextra->docBlockTerm = ';';
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
<UsingAlias>">>"                        {
                                          yyextra->previous->args+="> >"; // see bug769552
                                        }
<UsingAlias>.                           {
                                          yyextra->previous->args+=yytext;
                                        }
<UsingAlias>\n                          {
                                          yyextra->previous->args+=yytext;
                                          lineCount(yyscanner);
                                        }
<UsingAliasEnd>";"                      {
                                          yyextra->previous->doc   = yyextra->current->doc;
                                          yyextra->previous->brief = yyextra->current->brief;
                                          yyextra->current->doc.resize(0);
                                          yyextra->current->brief.resize(0);
                                          unput(';');
                                          BEGIN(UsingAlias);
                                        }
<UsingDirective>{SCOPENAME}             { yyextra->current->name=removeRedundantWhiteSpace(yytext);
                                          yyextra->current->fileName = yyextra->yyFileName;
                                          yyextra->current->section=Entry::USINGDIR_SEC;
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
                                          yyextra->roundCount=0;
                                          yyextra->sharpCount=0;
                                          lineCount(yyscanner);
                                          addType(yyscanner);
                                          yyextra->current->name=yytext;
                                          yyextra->current->name=yyextra->current->name.stripWhiteSpace();
                                          //yyextra->current->scopeSpec.resize(0);
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
                                          //yyextra->current->memberSpec.resize(0);
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
<ClassTemplSpec,EndTemplate>"<<"        {
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
<ClassTemplSpec,EndTemplate>">>"        {
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
                                            //printf("Found %s\n",qPrint(yyextra->current->name));
                                            BEGIN(FindMembers);
                                          }
                                        }
<EndTemplate>">"{BN}*"("                {
                                          lineCount(yyscanner);
                                          yyextra->current->name+='>';
                                          // *yyextra->currentTemplateSpec+='>';
                                          --yyextra->sharpCount;
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
<EndTemplate>">"{BNopt}/"("({BN}*{ID}{BN}*"::")*({BN}*"*"{BN}*)+ { // function pointer returning a template instance
                                          lineCount(yyscanner);
                                          yyextra->current->name+='>';
                                          if (yyextra->roundCount==0)
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
<CopyHereDoc>"'"{ID}/"'"                { // PHP nowdoc
                                          yyextra->delimiter = &yytext[1];
                                          *yyextra->pCopyHereDocGString << yytext;
                                          BEGIN(CopyHereDocEnd);
                                        }
<HereDoc>{ID}                           { // PHP heredoc
                                          yyextra->delimiter = yytext;
                                          BEGIN(HereDocEnd);
                                        }
<HereDoc>"'"{ID}/"'"                    { // PHP nowdoc
                                          yyextra->delimiter = &yytext[1];
                                          BEGIN(HereDocEnd);
                                        }
<HereDocEnd>^{ID}                       { // id at start of the line could mark the end of the block
                                          if (yyextra->delimiter==yytext) // it is the end marker
                                          {
                                            BEGIN(yyextra->lastHereDocContext);
                                          }
                                        }
<HereDocEnd>.                           { }
<CopyHereDocEnd>^{ID}                   { // id at start of the line could mark the end of the block
                                          *yyextra->pCopyHereDocGString << yytext;
                                          if (yyextra->delimiter==yytext) // it is the end marker
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
<FindMembers>"Q_OBJECT"                 { // Qt object macro
                                        }
<FindMembers>"Q_PROPERTY"               { // Qt property declaration
                                          yyextra->current->protection = Public ; // see bug734245 & bug735462
                                          yyextra->current->mtype = yyextra->mtype = Property;
                                          yyextra->current->type.resize(0);
                                          BEGIN(QtPropType);
                                        }
<QtPropType>"("                         { // start of property arguments
                                        }
<QtPropAttr>")"                         { // end of property arguments
                                          unput(';');
                                          BEGIN(FindMembers);
                                        }
<QtPropType>{B}+                        {
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
<QtPropType,QtPropAttr>{B}+"READ"{B}+   {
                                          yyextra->current->spec |= Entry::Readable;
                                          BEGIN(QtPropRead);
                                        }
<QtPropType,QtPropAttr>{B}+"WRITE"{B}+  {
                                          yyextra->current->spec |= Entry::Writable;
                                          BEGIN(QtPropWrite);
                                        }
<QtPropType,QtPropAttr>{B}+"MEMBER"{B}+{ID}     | // member property => not supported yet
<QtPropType,QtPropAttr>{B}+"RESET"{B}+{ID}      | // reset method => not supported yet
<QtPropType,QtPropAttr>{B}+"SCRIPTABLE"{B}+{ID} | // scriptable property => not supported yet
<QtPropType,QtPropAttr>{B}+"DESIGNABLE"{B}+{ID} | // designable property => not supported yet
<QtPropType,QtPropAttr>{B}+"NOTIFY"{B}+{ID}     | // notify property => not supported yet
<QtPropType,QtPropAttr>{B}+"REVISION"{B}+{ID}   | // revision property => not supported yet
<QtPropType,QtPropAttr>{B}+"STORED"{B}+{ID}     | // stored property => not supported yet
<QtPropType,QtPropAttr>{B}+"USER"{B}+{ID}       | // user property => not supported yet
<QtPropType,QtPropAttr>{B}+"CONSTANT"{B}        | // constant property => not supported yet
<QtPropType,QtPropAttr>{B}+"FINAL"{B}           { // final property => not supported yet
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
                                          yyextra->current->req.resize(0);
                                          yyextra->requiresContext = YY_START;
                                          BEGIN(RequiresClause);
                                        }
<RequiresClause>"requires"{BN}*"("      { // requires requires(T x) { ... }
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
                                          lineCount(yyextra);
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
<RequiresClause>{ID}                    { // something like "requires true"
                                          if (yyextra->current->req.stripWhiteSpace().isEmpty())
                                          {
                                            yyextra->current->req=yytext;
                                            BEGIN(yyextra->requiresContext);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<RequiresClause>{SCOPENAME}{BNopt}"("   { // "requires func(x)"
                                          yyextra->current->req+=yytext;
                                          yyextra->lastRoundContext=RequiresClause;
                                          yyextra->pCopyRoundString=&yyextra->current->req;
                                          yyextra->roundCount=0;
                                          BEGIN( CopyRound );
                                        }
<RequiresClause>{SCOPENAME}{BNopt}"<"   { // "requires C<S,T>"
                                          yyextra->current->req+=yytext;
                                          yyextra->lastSharpContext=RequiresClause;
                                          yyextra->pCopySharpString=&yyextra->current->req;
                                          yyextra->sharpCount=0;
                                          BEGIN( CopySharp );
                                        }
<RequiresClause>"||"|"&&"             { // "requires A || B" or "requires A && B"
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
                                          if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
                                          {
                                            yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,yytext);
                                          }
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
                                              BEGIN(NextSemi);
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
                                          else if (yyextra->insideCpp && qstrcmp(yytext,"static_assert")==0)
                                          {
                                            // C++11 static_assert
                                            BEGIN(StaticAssert);
                                          }
                                          else if (yyextra->insideCpp && qstrcmp(yytext,"decltype")==0)
                                          {
                                            // C++11 decltype(x)
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
                                              yyextra->current->protection = Public;
                                            }
                                            else if (javaLike && qstrcmp(yytext,"protected")==0)
                                            {
                                              yyextra->current->protection = Protected;
                                            }
                                            else if ((yyextra->insideCS || yyextra->insideD || yyextra->insidePHP || yyextra->insideJS) && qstrcmp(yytext,"internal")==0)
                                            {
                                              yyextra->current->protection = Package;
                                            }
                                            else if (javaLike && qstrcmp(yytext,"private")==0)
                                            {
                                              yyextra->current->protection = Private;
                                            }
                                            else if (javaLike && qstrcmp(yytext,"static")==0)
                                            {
                                              if (YY_START==FindMembers)
                                                yyextra->current->name  = yytext;
                                              else
                                                yyextra->current->name += yytext;
                                              yyextra->current->stat = TRUE;
                                            }
                                            else
                                            {
                                              if (YY_START==FindMembers)
                                                yyextra->current->name  = yytext;
                                              else
                                                yyextra->current->name += yytext;
                                              if (yyextra->current->name.left(7)=="static ")
                                              {
                                                yyextra->current->stat = TRUE;
                                                yyextra->current->name= yyextra->current->name.mid(7);
                                              }
                                              else if (yyextra->current->name.left(7)=="inline ")
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
                                              else if (yyextra->current->name.left(6)=="const ")
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
                                              else if (yyextra->current->name.left(9)=="volatile ")
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
                                            }
                                            QCString tmp=yytext;
                                            if (nameIsOperator(tmp))
                                            {
                                              BEGIN( Operator );
                                            }
                                            else
                                            {
                                              yyextra->externC=FALSE; // see bug759247
                                              BEGIN(FindMembers);
                                            }
                                          }
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
<FindMembers,FindFields>{B}*"#"         { if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCPPContext = YY_START;
                                          BEGIN( SkipCPP ) ;
                                        }
<FindMembers,FindFields>{B}*"#"{B}*("cmake")?"define"   {
                                          if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->lastDefineContext = YY_START;
                                          BEGIN( Define );
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
                                          yyextra->yyFileName = stripQuotes(yytext);
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
<Define>{ID}{B}*"("                     {
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
<Define>{ID}                            {
                                          //printf("Define '%s' without args\n",yytext);
                                          if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
                                          {
                                            yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,yytext);
                                          }
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = yytext;
                                          BEGIN(DefineEnd);
                                        }
<DefineEnd>\n                           {
                                          //printf("End define: doc=%s docFile=%s docLine=%d\n",qPrint(yyextra->current->doc),qPrint(yyextra->current->docFile),yyextra->current->docLine);
                                          yyextra->current->fileName   = yyextra->yyFileName;
                                          yyextra->current->startLine  = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                          yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->section    = Entry::DEFINE_SEC;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          lineCount(yyscanner);
                                          initEntry(yyscanner);
                                          BEGIN(yyextra->lastDefineContext);
                                        }
<DefinePHPEnd>";"                       {
                                          //printf("End define\n");
                                          yyextra->current->fileName   = yyextra->yyFileName;
                                          yyextra->current->startLine  = yyextra->yyLineNr;
                                          yyextra->current->startColumn = yyextra->yyColNr;
                                          yyextra->current->type.resize(0);
                                          yyextra->current->type       = "const";
                                          QCString init = yyextra->current->initializer.str();
                                          init = init.simplifyWhiteSpace();
                                          init = init.left(init.length()-1);
                                          yyextra->current->initializer.str(init.str());
                                          yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->section    = Entry::VARIABLE_SEC;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }
<DefinePHPEnd>.
<DefineEnd>\\[\r]?\n                    {
                                          lineCount(yyscanner);
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
<FindMembers,MemberSpec,Function,NextSemi,EnumBaseType,BitFields,ReadInitializer,ReadInitializerPtr,OldStyleArgs,DefinePHPEnd>";"{BN}*{DCOMM}"<" {
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
                                          if (YY_START==EnumBaseType && yyextra->current->section==Entry::ENUM_SEC)
                                          {
                                            yyextra->current->bitfields = ":"+yyextra->current->args;
                                            yyextra->current->args.resize(0);
                                            yyextra->current->section=Entry::VARIABLE_SEC;
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
                                          if (YY_START==EnumBaseType && yyextra->current->section==Entry::ENUM_SEC)
                                          {
                                            yyextra->current->bitfields = ":"+yyextra->current->args;
                                            yyextra->current->args.resize(0);
                                            yyextra->current->section=Entry::VARIABLE_SEC;
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
<DefineEnd,FindFields,FindFieldArg,ReadInitializer,ReadInitializerPtr,OldStyleArgs>{BN}*{DCOMM}"<" {
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
                                          if (yyextra->previous && yyextra->previous->section==Entry::GROUPDOC_SEC)
                                          {
                                            // link open command to the group defined in the yyextra->previous entry
                                            yyextra->commentScanner.open(yyextra->previous.get(),yyextra->yyFileName,yyextra->yyLineNr);
                                          }
                                          else
                                          {
                                            // link open command to the yyextra->current entry
                                            yyextra->commentScanner.open(yyextra->current.get(),yyextra->yyFileName,yyextra->yyLineNr);
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
                                          yyextra->commentScanner.close(yyextra->current.get(),yyextra->yyFileName,yyextra->yyLineNr,insideEnum);
                                          lineCount(yyscanner);
                                        }
<FindMembers>"=>"                       {
                                          if (!yyextra->insideCS) REJECT;
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->initializer.str(yytext);
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->mtype = yyextra->mtype = Property;
                                          yyextra->current->spec |= Entry::Gettable;
                                          BEGIN(ReadInitializerPtr);
                                        }
<FindMembers>"="                        { // in PHP code this could also be due to "<?="
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->initializer.str(yytext);
                                          yyextra->lastInitializerContext = YY_START;
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
                                          if (*yytext==';' && (yyextra->current_root->spec&Entry::Enum))
                                          {
                                            yyextra->current->fileName   = yyextra->yyFileName;
                                            yyextra->current->startLine  = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                            yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                            yyextra->current->section    = Entry::VARIABLE_SEC;
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
                                            QCString text=yytext;
                                            yyextra->current->initializer << text;
                                            int i=text.find('"');
                                            yyextra->delimiter = yytext+i+1;
                                            yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                                            yyextra->lastRawStringContext = YY_START;
                                            yyextra->pCopyRawGString = &yyextra->current->initializer;
                                            BEGIN(RawGString);
                                            //printf("RawGString delimiter='%s'\n",qPrint(delimiter));
                                          }
                                        }
<RawGString>{RAWEND}                    {
                                          *yyextra->pCopyRawGString << yytext;
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==yyextra->delimiter)
                                          {
                                            BEGIN(yyextra->lastRawStringContext);
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
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==yyextra->delimiter)
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
<ReadInitializer,ReadInitializerPtr>\"                  {
                                          if (yyextra->insideIDL && yyextra->insideCppQuote)
                                          {
                                            BEGIN(EndCppQuote);
                                          }
                                          else
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            yyextra->current->initializer << yytext;
                                            yyextra->pCopyQuotedGString=&yyextra->current->initializer;
                                            BEGIN(CopyGString);
                                          }
                                        }
<ReadInitializer,ReadInitializerPtr>"->"                        {
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>"<<"                        {
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>">>"                        {
                                          yyextra->current->initializer << yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>[<\[{(]             {
                                          yyextra->initBracketCount++;
                                          yyextra->current->initializer << *yytext;
                                        }
<ReadInitializer,ReadInitializerPtr>[>\]})]             {
                                          yyextra->initBracketCount--;
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
<SkipVerbString>[^\n"]+                 {
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
                                          lineCount(yyscanner);
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
<GCopyRound>[^"'()\n\/,]+               {
                                          *yyextra->pCopyRoundGString << yytext;
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
<GCopySquare>[^"\[\]\n\/,]+              {
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
                                          BEGIN(CopyString);
                                        }
<CopyCurly>\'                           {
                                          *yyextra->pCopyCurlyString += *yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->pCopyQuotedString=yyextra->pCopyCurlyString;
                                            yyextra->lastStringContext=YY_START;
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
                                            BEGIN(yyextra->lastCurlyContext);
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

  /* generic curly bracket list copy rules for growable strings */
<GCopyCurly>^"#"{B}+[0-9]+{B}+"\""[^\"\n]+"\""{B}+"1"{B}*\n? { // start of included file marker
                                        }
<GCopyCurly>^"#"{B}+[0-9]+{B}+"\""[^\"\n]+"\""{B}+"2"{B}*\n? { // end of included file marker
                                          QCString line = QCString(yytext);
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
                                          BEGIN(CopyGString);
                                        }
<GCopyCurly>\'                          {
                                          *yyextra->pCopyCurlyGString << *yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->pCopyQuotedGString=yyextra->pCopyCurlyGString;
                                            yyextra->lastStringContext=YY_START;
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
                                            BEGIN(yyextra->lastCurlyContext);
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

  /* ---------------------- */


<FindMembers>":"                        {
                                          if (yyextra->current->type.isEmpty() &&
                                              yyextra->current->name=="enum") // see bug 69041, C++11 style anon enum: 'enum : unsigned int {...}'
                                          {
                                            yyextra->current->section=Entry::ENUM_SEC;
                                            yyextra->current->name.resize(0);
                                            yyextra->current->args.resize(0);
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
                                          if ( yyextra->insidePHP && yyextra->current->type.left(3) == "var" )
                                          {
                                            yyextra->current->type = yyextra->current->type.mid(3);
                                          }
                                          if (yyextra->isTypedef && yyextra->current->type.left(8)!="typedef ")
                                          {
                                            yyextra->current->type.prepend("typedef ");
                                          }
                                          bool stat = yyextra->current->stat;
                                          if (yyextra->current->section==Entry::CONCEPT_SEC) // C++20 concept
                                          {
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          else if (!yyextra->current->name.isEmpty() && yyextra->current->section!=Entry::ENUM_SEC)
                                          {
                                            yyextra->current->type=yyextra->current->type.simplifyWhiteSpace();
                                            yyextra->current->args=removeRedundantWhiteSpace(yyextra->current->args);
                                            yyextra->current->name=yyextra->current->name.stripWhiteSpace();
                                            if (yyextra->current->section==Entry::CLASS_SEC) // remove spec for "struct Bla bla;"
                                            {
                                                yyextra->current->spec = 0;
                                            }
                                            yyextra->current->section = Entry::VARIABLE_SEC ;
                                            yyextra->current->fileName = yyextra->yyFileName;
                                            yyextra->current->startLine = yyextra->yyBegLineNr;
                                            yyextra->current->startColumn = yyextra->yyBegColNr;
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          if ( *yytext == ',')
                                          {
                                            yyextra->current->stat = stat; // the static attribute holds for all variables
                                            yyextra->current->name.resize(0);
                                            yyextra->current->args.resize(0);
                                            yyextra->current->brief.resize(0);
                                            yyextra->current->doc.resize(0);
                                            yyextra->current->initializer.str(std::string());
                                            yyextra->current->bitfields.resize(0);
                                            int i=oldType.length();
                                            while (i>0 && (oldType[i-1]=='*' || oldType[i-1]=='&' || oldType[i-1]==' ')) i--;
                                            yyextra->current->type = oldType.left(i);
                                          }
                                          else
                                          {
                                            yyextra->mtype = Method;
                                            yyextra->virt = Normal;
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
                                            yyextra->idlAttr.resize(0);
                                            yyextra->idlProp.resize(0);
                                            yyextra->current->mtype = yyextra->mtype;

                                            if (Config_getBool(IDL_PROPERTY_SUPPORT) &&
                                                 yyextra->current->mtype == Property)
                                            { // we are yyextra->inside the properties section of a dispinterface
                                              yyextra->odlProp = true;
                                              yyextra->current->spec |= Entry::Gettable;
                                              yyextra->current->spec |= Entry::Settable;
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
                                            yyextra->current->args.resize(0);
                                            BEGIN( SkipSquare );
                                          }
                                          else
                                          {
                                            yyextra->current->args += yytext ;
                                            yyextra->squareCount=1;
                                            yyextra->externC=FALSE; // see bug759247
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
                                            if (yyextra->current->mtype == Property)
                                              BEGIN( IDLPropName );
                                            else
                                              BEGIN( yyextra->lastSquareContext );
                                          }
                                        }
<IDLAttribute>"propput"                 {
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT))
                                          {
                                            yyextra->current->mtype = Property;
                                          }
                                          yyextra->current->spec |= Entry::Settable;
                                        }
<IDLAttribute>"propget"                 {
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT))
                                          {
                                            yyextra->current->mtype = Property;
                                          }
                                          yyextra->current->spec |= Entry::Gettable;
                                        }
<IDLAttribute>"property" { // UNO IDL property
                                          yyextra->current->spec |= Entry::Property;
                                        }
<IDLAttribute>"attribute" { // UNO IDL attribute
                                          yyextra->current->spec |= Entry::Attribute;
                                        }
<IDLAttribute>"optional" { // on UNO IDL interface/service/attribute/property
                           yyextra->current->spec |= Entry::Optional;
                         }
<IDLAttribute>"readonly" { // on UNO IDL attribute or property
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT) && yyextra->odlProp)
                                          {
                                            yyextra->current->spec ^= Entry::Settable;
                                          }
                                          else
                                          {
                                            yyextra->current->spec |= Entry::Readonly;
                                          }
                                        }
<IDLAttribute>"bound" { // on UNO IDL attribute or property
                                          yyextra->current->spec |= Entry::Bound;
                                        }
<IDLAttribute>"removable" { // on UNO IDL property
                                          yyextra->current->spec |= Entry::Removable;
                                        }
<IDLAttribute>"constrained" { // on UNO IDL property
                                          yyextra->current->spec |= Entry::Constrained;
                                        }
<IDLAttribute>"transient" { // on UNO IDL property
                                          yyextra->current->spec |= Entry::Transient;
                                        }
<IDLAttribute>"maybevoid" { // on UNO IDL property
                                          yyextra->current->spec |= Entry::MaybeVoid;
                                        }
<IDLAttribute>"maybedefault" { // on UNO IDL property
                                          yyextra->current->spec |= Entry::MaybeDefault;
                                        }
<IDLAttribute>"maybeambiguous" { // on UNO IDL property
                                          yyextra->current->spec |= Entry::MaybeAmbiguous;
                                        }
<IDLAttribute>.                         {
                                        }
<IDLPropName>{BN}*{ID}{BN}*             {
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
                                          yyextra->idlProp.resize(0);
                                          yyextra->idlAttr.resize(0);
                                          BEGIN( IDLProp );
                                        }
<IDLProp>{BN}*{ID}{BN}*")"{BN}*         {
                                          // the parameter name for the property - just skip.
                                        }
<IDLProp>";"                            {
                                          yyextra->current->fileName   = yyextra->yyFileName;
                                          yyextra->current->type                = yyextra->idlProp;
                                          yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                          if (!yyextra->current->args.isEmpty())
                                            yyextra->current->args += ")";
                                          yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                          yyextra->current->section    = Entry::VARIABLE_SEC;
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
                                             BEGIN( Function ) ;
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
                                          if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
                                          {
                                            yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,yytext);
                                          }
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name     = yytext;
                                        }
<FindFields>"("                         {
                                          // Java enum initializer
                                          unput('(');
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->initializer.str("=");
                                          BEGIN(ReadInitializer);
                                        }
<FindFields>"="                         {
                                          yyextra->lastInitializerContext = YY_START;
                                          yyextra->initBracketCount=0;
                                          yyextra->current->initializer.str(yytext);
                                          BEGIN(ReadInitializer);
                                        }
<FindFields>";"                         {
                                          if (yyextra->insideJava)  // yyextra->last enum field in Java class
                                          {
                                            if (!yyextra->current->name.isEmpty())
                                            {
                                              yyextra->current->fileName   = yyextra->yyFileName;
                                              yyextra->current->startLine  = yyextra->yyLineNr;
                                              yyextra->current->startColumn = yyextra->yyColNr;
                                              if (!(yyextra->current_root->spec&Entry::Enum))
                                              {
                                                yyextra->current->type       = "@"; // enum marker
                                              }
                                              yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                              yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                              yyextra->current->section    = Entry::VARIABLE_SEC;
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
<SkipRemainder>\n                       {
                                          lineCount(yyscanner);
                                        }
<SkipRemainder>[^\n]*
<FindFields>","                         {
                                          //printf("adding '%s' '%s' '%s' to enum '%s' (mGrpId=%d)\n",
                                          //     qPrint(yyextra->current->type), qPrint(yyextra->current->name),
                                          //     qPrint(yyextra->current->args), qPrint(yyextra->current_root->name),yyextra->current->mGrpId);
                                          if (!yyextra->current->name.isEmpty())
                                          {
                                            yyextra->current->fileName   = yyextra->yyFileName;
                                            yyextra->current->startLine  = yyextra->yyLineNr;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            if (!(yyextra->current_root->spec&Entry::Enum))
                                            {
                                              yyextra->current->type       = "@"; // enum marker
                                            }
                                            yyextra->current->args       = yyextra->current->args.simplifyWhiteSpace();
                                            yyextra->current->name       = yyextra->current->name.stripWhiteSpace();
                                            yyextra->current->section    = Entry::VARIABLE_SEC;
                                            // add to the scope of the enum
                                            if (!yyextra->insideCS && !yyextra->insideJava &&
                                                !(yyextra->current_root->spec&Entry::Strong))
                                                // for C# and Java 1.5+ enum values always have to be explicitly qualified,
                                                // same for C++11 style enums (enum class Name {})
                                            {
                                              // add to the scope surrounding the enum (copy!)
                                              // we cannot during it directly as that would invalidate the iterator in parseCompounds.
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
  /*
<FindFieldArg>","                       { unput(*yytext); BEGIN(FindFields); }
  */
<ReadBody,ReadNSBody,ReadBodyIntf>[^\r\n\#{}"@'/<]*     { yyextra->current->program << yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>{CPPC}.*                { yyextra->current->program << yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>"#".* { if (!yyextra->insidePHP)
                                            REJECT;
                                          // append PHP comment.
                                          yyextra->current->program << yytext ;
                                        }
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
<ReadBody,ReadNSBody,ReadBodyIntf>\"    { yyextra->current->program << yytext ;
                                          yyextra->pCopyQuotedGString = &yyextra->current->program;
                                          yyextra->lastStringContext=YY_START;
                                          BEGIN( CopyGString );
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>{CCS}{B}*              { yyextra->current->program << yytext ;
                                          yyextra->lastContext = YY_START ;
                                          BEGIN( Comment ) ;
                                        }
<ReadBody,ReadNSBody,ReadBodyIntf>{CCS}{BL}              { yyextra->current->program << yytext ;
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
<ReadBody,ReadNSBody>"}"                { //err("ReadBody count=%d\n",yyextra->curlyCount);
                                          if ( yyextra->curlyCount>0 )
                                          {
                                            yyextra->current->program << yytext ;
                                            --yyextra->curlyCount ;
                                          }
                                          else
                                          {
                                            yyextra->current->endBodyLine = yyextra->yyLineNr;
                                            std::shared_ptr<Entry> original_root = yyextra->current_root; // save root this namespace is in
                                            if (yyextra->current->section == Entry::NAMESPACE_SEC && yyextra->current->type == "namespace")
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
                                              if ((yyextra->current->section == Entry::ENUM_SEC) || (yyextra->current->spec&Entry::Enum))
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
                                                  ((yyextra->current->spec&Entry::Interface) || (yyextra->current->spec==Entry::Category))
                                                 ) // method definition follows
                                              {
                                                BEGIN( ReadBodyIntf ) ;
                                              }
                                              else
                                              {
                                                yyextra->memspecEntry = yyextra->current;
                                                yyextra->current_root->moveToSubEntryAndKeep( yyextra->current ) ;
                                                yyextra->current = std::make_shared<Entry>(*yyextra->current);
                                                if (yyextra->current->section==Entry::NAMESPACE_SEC ||
                                                    (yyextra->current->spec==Entry::Interface) ||
                                                    yyextra->insideJava || yyextra->insidePHP || yyextra->insideCS || yyextra->insideD || yyextra->insideJS ||
                                                    yyextra->insideSlice
                                                   )
                                                { // namespaces and interfaces and java classes ends with a closing bracket without semicolon
                                                  yyextra->current->reset();
                                                  yyextra->current_root = original_root; // restore scope from before namespace descent
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
                                                    yyextra->current->doc.resize(0);
                                                    yyextra->current->brief.resize(0);
                                                  }
                                                  BEGIN( MemberSpec ) ;
                                                }
                                              }
                                            }
                                          }
                                        }
<ReadBody>"}"{BN}+"typedef"{BN}+        { //err("ReadBody count=%d\n",yyextra->curlyCount);
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
                                          if ((yyextra->current->section == Entry::ENUM_SEC) || (yyextra->current->spec&Entry::Enum))
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
                                          yyextra->current->name.sprintf("@%d",anonCount++);
                                          if ((yyextra->current->section == Entry::ENUM_SEC) || (yyextra->current->spec&Entry::Enum))
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
                                            if (yyextra->firstTypedefEntry->spec&Entry::Struct)
                                            {
                                              yyextra->msType.prepend("struct "+yyextra->firstTypedefEntry->name);
                                            }
                                            else if (yyextra->firstTypedefEntry->spec&Entry::Union)
                                            {
                                              yyextra->msType.prepend("union "+yyextra->firstTypedefEntry->name);
                                            }
                                            else if (yyextra->firstTypedefEntry->section==Entry::ENUM_SEC)
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
                                          yyextra->current->spec = 0;
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
                                              if ((p->section & Entry::COMPOUND_MASK) && !p->name.isEmpty())
                                              {
                                                //printf("Trying scope '%s'\n",qPrint(p->name));
                                                int i=p->name.findRev("::");
                                                int pi = (i==-1) ? 0 : i+2;
                                                if (p->name.at(pi)=='@')
                                                {
                                                  // anonymous compound yyextra->inside -> insert dummy variable name
                                                  //printf("Adding anonymous variable for scope %s\n",qPrint(p->name));
                                                  yyextra->msName.sprintf("@%d",anonCount++);
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
                                                ((yyextra->current->spec&(Entry::Struct|Entry::Union)) ||
                                                 yyextra->current->section==Entry::ENUM_SEC )&&
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
                                              varEntry->stat = yyextra->current->stat;
                                              varEntry->section = Entry::VARIABLE_SEC;
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
                                                  (yyextra->current->spec&(Entry::Struct|Entry::Union)) &&
                                                  yyextra->memspecEntry
                                                 ) // case 1: use S_t as type for pS_t in "typedef struct _S {} S_t, *pS_t;"
                                              {
                                                varEntry->type+=yyextra->memspecEntry->name+yyextra->msType;
                                              }
                                              else // case 2: use _S as type for for pS_t
                                              {
                                                varEntry->type+=yyextra->current->name+yyextra->msType;
                                              }
                                              varEntry->fileName = yyextra->yyFileName;
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
                                            if (!yyextra->isTypedef && yyextra->msName.isEmpty() && yyextra->memspecEntry && (yyextra->current->section&Entry::COMPOUND_MASK))
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
                                            yyextra->msType.resize(0);
                                            yyextra->msName.resize(0);
                                            yyextra->msArgs.resize(0);
                                            yyextra->isTypedef=FALSE;
                                            yyextra->firstTypedefEntry.reset();
                                            yyextra->memspecEntry.reset();
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            yyextra->current->doc.resize(0);
                                            yyextra->current->brief.resize(0);
                                          }

                                        }
<MemberSpec>"="                         {
                                          yyextra->lastInitializerContext=YY_START;
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
                                          yyextra->language = yyextra->current->lang = SrcLangExt_Cpp; // see bug746361
                                          yyextra->insideObjC=FALSE;
                                          BEGIN( FindMembers );
                                        }
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
                                          //printf("error: FuncPtr '%c' unexpected at line %d of %s\n",*yytext,yyextra->yyLineNr,yyextra->yyFileName);
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
                                          yyextra->current->type+=yyextra->funcPtrType+")";
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
                                          BEGIN(Function);
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
                                            BEGIN(Function);
                                        }
<FuncFuncType>{BN}*","{BN}*             { lineCount(yyscanner) ; yyextra->current->type += ", " ; }
<FuncFuncType>{BN}+                     { lineCount(yyscanner) ; yyextra->current->type += ' ' ; }
<FuncFuncType>.                         {
                                          yyextra->current->type += *yytext;
                                        }
<FindMembers>"("/{BN}*{ID}{BN}*"*"{BN}*{ID}*")"{BN}*"(" { // for catching typedef void (__stdcall *f)() like definitions
                                          if (yyextra->current->type.left(7)=="typedef" && yyextra->current->bodyLine==-1)
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

<ReadFuncArgType>[^ \/\r\t\n\)\(\"\'#]+ { *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<CopyArgString,CopyArgPHPString>[^\n\\\"\']+            { *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<CopyArgRound>[^\/\n\)\(\"\']+          {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                        }
<ReadFuncArgType,ReadTempArgs>{BN}*     {
                                          *yyextra->copyArgString+=" ";
                                          yyextra->fullArgString+=" ";
                                          lineCount(yyscanner);
                                        }
<ReadFuncArgType,CopyArgRound,CopyArgSharp,ReadTempArgs>{RAWBEGIN}      {
                                          yyextra->delimiter = yytext+2;
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                                          yyextra->lastRawStringContext = YY_START;
                                          yyextra->pCopyRawString = yyextra->copyArgString;
                                          *yyextra->pCopyRawString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(RawString);
                                        }
<ReadFuncArgType,CopyArgRound,CopyArgSharp,ReadTempArgs>\"      {
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                          yyextra->lastCopyArgStringContext = YY_START;
                                          BEGIN( CopyArgString );
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
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(CopyArgVerbatim);
                                        }
<CopyArgCommentLine>{CMD}("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc"|"code")/[^a-z_A-Z0-9\-]   { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlockName=&yytext[1];
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fullArgString+=yytext;
                                          BEGIN(CopyArgVerbatim);
                                        }
<CopyArgCommentLine>{CMD}("f$"|"f["|"f{"|"f(")               {
                                          yyextra->docBlockName=&yytext[1];
                                          yyextra->docBlockLine=yyextra->yyLineNr;
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
<CopyArgVerbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"ebdmsc"|"enduml"|"endcode"|"f$"|"f]"|"f}"|"f)")/[^a-z_A-Z0-9\-] { // end of verbatim block
                                          yyextra->fullArgString+=yytext;
					  if (yytext[1]=='f' && yyextra->docBlockName==&yytext[1])
                                          {
                                            BEGIN(CopyArgCommentLine);
                                          }
                                          if (&yytext[4]==yyextra->docBlockName)
                                          {
                                            BEGIN(CopyArgCommentLine);
                                          }
                                        }
<CopyArgCommentLine>[^\\\@\n]+          { yyextra->fullArgString+=yytext; }
<CopyArgCommentLine>.                   { yyextra->fullArgString+=*yytext; }
<CopyArgComment,CopyArgVerbatim>\n      { yyextra->fullArgString+=*yytext; lineCount(yyscanner); }
<CopyArgComment,CopyArgVerbatim>.       { yyextra->fullArgString+=*yytext; }
<CopyArgComment>{CMD}("brief"|"short"){B}+ {
                                          warn(yyextra->yyFileName,yyextra->yyLineNr,
                                              "Ignoring %cbrief command inside argument documentation",*yytext
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
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>{CHARLIT}     {
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
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>\'     {
                                          *yyextra->copyArgString+=yytext;
                                          yyextra->fullArgString+=yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastCopyArgStringContext=YY_START;
                                            BEGIN(CopyArgPHPString);
                                          }
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSharp>\n  {
                                          lineCount(yyscanner);
                                          *yyextra->copyArgString+=*yytext;
                                          yyextra->fullArgString+=*yytext;
                                        }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSharp>.          {
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
                                               yyextra->insidePHP &&
                                               !containsWord(yyextra->current->type,"function") )
                                          {
                                            yyextra->current->reset();
                                            initEntry(yyscanner);
                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            unput(*yytext); BEGIN( Function );
                                          }
                                        }
<FuncQual>{BN}*"abstract"{BN}*          { // pure virtual member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->virt = Pure;
                                          yyextra->current->args += " override ";
                                        }
<FuncQual,TrailingReturn>{BN}*"override"{BN}*          { // C++11 overridden virtual member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec |= Entry::Override;
                                          yyextra->current->args += " override ";
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"final"{BN}*             { // C++11 final method
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec |= Entry::Final;
                                          yyextra->current->args += " final ";
                                          BEGIN(FuncQual);
                                        }
<FuncQual>{BN}*"sealed"{BN}*            { // sealed member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec |= Entry::Sealed;
                                          yyextra->current->args += " sealed ";
                                        }
<FuncQual>{BN}*"new"{BN}*               { // new member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->spec |= Entry::New;
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
                                          yyextra->current->spec |= Entry::NoExcept;
                                        }
<FuncQual>{BN}*"noexcept"{BN}*"("       { // noexcept expression
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " noexcept(";
                                          yyextra->current->spec |= Entry::NoExcept;
                                          yyextra->lastRoundContext=FuncQual;
                                          yyextra->pCopyRoundString=&yyextra->current->args;
                                          yyextra->roundCount=0;
                                          BEGIN(CopyRound);
                                        }
<FuncQual>{BN}*"&"                      {
                                          yyextra->current->args += " &";
                                          yyextra->current->argList.setRefQualifier(RefQualifierLValue);
                                        }
<FuncQual>{BN}*"&&"                     {
                                          yyextra->current->args += " &&";
                                          yyextra->current->argList.setRefQualifier(RefQualifierRValue);
                                        }

<FuncQual,TrailingReturn>{BN}*"="{BN}*"0"{BN}*          { // pure virtual member function
                                          lineCount(yyscanner) ;
                                          yyextra->current->args += " = 0";
                                          yyextra->current->virt = Pure;
                                          yyextra->current->argList.setPureSpecifier(TRUE);
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"="{BN}*"delete"{BN}*     { // C++11 explicitly delete member
                                          lineCount(yyscanner);
                                          yyextra->current->args += " = delete";
                                          yyextra->current->spec |= Entry::Delete;
                                          yyextra->current->argList.setIsDeleted(TRUE);
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"="{BN}*"default"{BN}*     { // C++11 explicitly defaulted constructor/assignment operator
                                          lineCount(yyscanner);
                                          yyextra->current->args += " = default";
                                          yyextra->current->spec |= Entry::Default;
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
                                          BEGIN(FuncQual);
                                        }
<TrailingReturn>"requires"{BN}+         {
                                          yyextra->requiresContext = FuncQual;
                                          yyextra->current->req+=' ';
                                          BEGIN(RequiresClause);
                                        }
<TrailingReturn>"("                     {
                                          yyextra->roundCount++;
                                          yyextra->current->argList.setTrailingReturnType(yyextra->current->argList.trailingReturnType()+yytext);
                                          yyextra->current->args+=yytext;
                                        }
<TrailingReturn>")"                     {
                                          if (yyextra->roundCount>0)
                                          {
                                            yyextra->roundCount--;
                                          }
                                          else
                                          {
                                            warn(yyextra->yyFileName,yyextra->yyLineNr,
                                                 "Found ')' without opening '(' for trailing return type '%s)...'",
                                                 qPrint(yyextra->current->argList.trailingReturnType()));
                                          }
                                          yyextra->current->argList.setTrailingReturnType(yyextra->current->argList.trailingReturnType()+yytext);
                                          yyextra->current->args+=yytext;
                                        }
<TrailingReturn>.                       {
                                          yyextra->current->argList.setTrailingReturnType(yyextra->current->argList.trailingReturnType()+yytext);
                                          yyextra->current->args+=yytext;
                                        }
<TrailingReturn>\n                      {
                                          lineCount(yyscanner);
                                          yyextra->current->argList.setTrailingReturnType(yyextra->current->argList.trailingReturnType()+yytext);
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
<Function,FuncQual,FuncRound,FuncFunc>"#" { if (yyextra->insidePHP)
                                            REJECT;
                                          yyextra->lastCPPContext = YY_START;
                                          BEGIN(SkipCPP);
                                        }
<FuncQual>"="                           {
                                          if (yyextra->insideCli &&
                                              (yyextra->current_root->section&Entry::COMPOUND_MASK)
                                             )
                                          {
                                            BEGIN(CliOverride);
                                          }
                                          else
                                          {
                                            // typically an initialized function pointer
                                            yyextra->lastInitializerContext=YY_START;
                                            yyextra->initBracketCount=0;
                                            yyextra->current->initializer.str(yytext);
                                            BEGIN(ReadInitializer);
                                          }
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
<FuncPtrInit>[{;]                       {
                                          unput(*yytext);
                                          BEGIN(FuncQual);
                                        }
<FuncPtrInit>\"                         {
                                          yyextra->current->args += *yytext;
                                          yyextra->pCopyQuotedString=&yyextra->current->args;
                                          yyextra->lastStringContext=FuncPtrInit;
                                          BEGIN(CopyString);
                                        }
<FuncPtrInit>\'                         {
                                          yyextra->current->args += *yytext;
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->pCopyQuotedString=&yyextra->current->args;
                                            yyextra->lastStringContext=FuncPtrInit;
                                            BEGIN(CopyPHPString);
                                          }
                                        }
<FuncPtrInit>{CHARLIT}                  {
                                          if (yyextra->insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->current->args += yytext;
                                          }
                                        }
<FuncPtrInit>{ID}                       {
                                          yyextra->current->args += yytext;
                                        }
<FuncPtrInit>.                          {
                                          yyextra->current->args += *yytext;
                                        }
<FuncPtrInit>\n                         {
                                          yyextra->current->args += *yytext;
                                          lineCount(yyscanner);
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
                                            yyextra->oldStyleArgType.resize(0);
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
                                          yyextra->current->args.resize(0);
                                          if (*yytext==';') yyextra->oldStyleArgType.resize(0);
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
<FuncQual,FuncRound,FuncFunc>.          { yyextra->current->args += *yytext; }
<FuncQual>{BN}*"try:"                   |
<FuncQual>{BN}*"try"{BN}+               { /* try-function-block */
                                          yyextra->insideTryBlock=TRUE;
                                          lineCount(yyscanner);
                                          if (yytext[yyleng-1]==':')
                                          {
                                            unput(':');
                                            BEGIN( Function );
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
<Function>"("                           { yyextra->current->type += yyextra->current->name ;
                                          yyextra->current->name  = yyextra->current->args ;
                                          yyextra->current->args  = yytext ;
                                          yyextra->roundCount=0;
                                          BEGIN( FuncRound ) ;
                                        }
<Function>":"                           {
                                          if (!yyextra->insidePHP) BEGIN(SkipInits);
                                        }
<Function>[;{,]                         {
                                          yyextra->current->name=yyextra->current->name.simplifyWhiteSpace();
                                          yyextra->current->type=yyextra->current->type.simplifyWhiteSpace();
                                          yyextra->current->args=removeRedundantWhiteSpace(yyextra->current->args);
                                                        // was: yyextra->current->args.simplifyWhiteSpace();
                                          yyextra->current->fileName = yyextra->yyFileName;
                                          yyextra->current->startLine = yyextra->yyBegLineNr;
                                          yyextra->current->startColumn = yyextra->yyBegColNr;
                                          static const reg::Ex re(R"(\([^)]*[*&][^)]*\))");
                                          reg::Match match;
                                          std::string type = yyextra->current->type.str();
                                          int ti=-1;
                                          if (reg::search(type,match,re))
                                          {
                                            ti = (int)match.position();
                                          }
                                          int ts=yyextra->current->type.find('<');
                                          int te=yyextra->current->type.findRev('>');

                                          // bug677315: A<int(void *, char *)> get(); is not a function pointer
                                          bool isFunction = ti==-1 || // not a (...*...) pattern
                                                            (ts!=-1 && ts<te && ts<ti && ti<te); // (...*...) is part of a template argument list
                                          bool isVariable = (!yyextra->current->type.isEmpty() &&
                                                (!isFunction || yyextra->current->type.left(8)=="typedef "));

                                          //printf("type=%s ts=%d te=%d ti=%d isFunction=%d\n",
                                          //    qPrint(yyextra->current->type),ts,te,ti,isFunction);

                                          if (*yytext!=';' || (yyextra->current_root->section&Entry::COMPOUND_MASK) )
                                          {
                                            int tempArg=yyextra->current->name.find('<');
                                            QCString tempName;
                                            if (tempArg==-1) tempName=yyextra->current->name; else tempName=yyextra->current->name.left(tempArg);
                                            if (isVariable)
                                            {
                                              //printf("Scanner.l: found in class variable: '%s' '%s' '%s'\n", qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                              if (yyextra->isTypedef && yyextra->current->type.left(8)!="typedef ")
                                              {
                                                yyextra->current->type.prepend("typedef ");
                                              }
                                              yyextra->current->section = Entry::VARIABLE_SEC ;
                                            }
                                            else
                                            {
                                              //printf("Scanner.l: found in class function: '%s' '%s' '%s'\n", qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                              yyextra->current->section = Entry::FUNCTION_SEC ;
                                              yyextra->current->proto = *yytext==';';
                                            }
                                          }
                                          else // a global function prototype or function variable
                                          {
                                            //printf("Scanner.l: prototype? type='%s' name='%s' args='%s'\n",qPrint(yyextra->current->type),qPrint(yyextra->current->name),qPrint(yyextra->current->args));
                                            if (isVariable)
                                            {
                                              if (yyextra->isTypedef && yyextra->current->type.left(8)!="typedef ")
                                              {
                                                yyextra->current->type.prepend("typedef ");
                                              }
                                              //printf("Scanner.l: found function variable!\n");
                                              yyextra->current->section = Entry::VARIABLE_SEC;
                                            }
                                            else
                                            {
                                              //printf("Scanner.l: found prototype\n");
                                              yyextra->current->section = Entry::FUNCTION_SEC;
                                              yyextra->current->proto = TRUE;
                                            }
                                          }
                                          //printf("Adding entry '%s'\n",qPrint(yyextra->current->name));
                                          if ( yyextra->insidePHP)
                                          {
                                            if (findAndRemoveWord(yyextra->current->type,"final"))
                                            {
                                              yyextra->current->spec |= Entry::Final;
                                            }
                                            if (findAndRemoveWord(yyextra->current->type,"abstract"))
                                            {
                                              yyextra->current->spec |= Entry::Abstract;
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
                                            if (yyextra->previous->spec & (Entry::Optional | Entry::Required))
                                            {
                                              yyextra->current->spec |= yyextra->previous->spec & (Entry::Optional|Entry::Required);
                                            }
                                            yyextra->lastCurlyContext = FindMembers;
                                            if ( *yytext == ',' )
                                            {
                                              yyextra->current->type = yyextra->previous->type;
                                              // we need to strip any trailing * and & (see bugs 623023 and 649103 for test cases)
                                              int i=yyextra->current->type.length();
                                              while (i>0 && (yyextra->current->type[i-1]=='*' || yyextra->current->type[i-1]=='&' || yyextra->current->type[i-1]==' ')) i--;
                                              yyextra->current->type = yyextra->current->type.left(i);
                                            }
                                            if ( *yytext == '{' )
                                            {
                                              if ( !yyextra->insidePHP && (yyextra->current_root->section & Entry::COMPOUND_MASK) )
                                              {
                                                yyextra->previous->spec |= Entry::Inline;
                                              }
                                              //addToBody(yytext);
                                              yyextra->curlyCount=0;
                                              BEGIN( SkipCurly ) ;
                                            }
                                            else
                                            {
                                              if (yyextra->previous->section!=Entry::VARIABLE_SEC)
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
                                          BEGIN( Function );
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
<SkipCurly>\"                           {
                                          //addToBody(yytext);
                                          yyextra->lastStringContext=SkipCurly;
                                          BEGIN( SkipString );
                                        }
<SkipCurly>^{B}*"#"                     {
                                          if (yyextra->insidePHP)
                                            REJECT;
                                          //addToBody(yytext);
                                          BEGIN( SkipCurlyCpp );
                                        }
<SkipCurly,SkipC11Inits,SkipInits,SkipC11Attribute>\n   {
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
                                          QCString raw=QCString(yytext).stripWhiteSpace();
                                          yyextra->delimiter = raw.mid(2);
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                                          yyextra->lastRawStringContext = YY_START;
                                          yyextra->dummyRawString.resize(0);
                                          yyextra->pCopyRawString = &yyextra->dummyRawString;
                                          *yyextra->pCopyRawString += yytext;
                                          BEGIN(RawString);
                                        }
<SkipCurly,SkipCurlyCpp>[^\n#"'@\\/{}<]+ {
                                          lineCount(yyscanner);  // for yyextra->column updates
                                          //addToBody(yytext);
                                        }
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
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute>{CCS}    {
                                          //addToBody(yytext);
                                          yyextra->lastCContext = YY_START;
                                          BEGIN(SkipComment);
                                        }
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute>{CPPC}  {
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
                                          warn(yyextra->yyFileName,yyextra->yyLineNr,
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
<SkipInits,SkipCurly,SkipCurlyCpp>@\"   {
                                          if (!yyextra->insideCS) REJECT;
                                          // C# verbatim string
                                          yyextra->lastSkipVerbStringContext=YY_START;
                                          yyextra->pSkipVerbString=&yyextra->current->initializer;
                                          BEGIN(SkipVerbString);
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp>{CHARLIT}     {
                                          if (yyextra->insidePHP) REJECT;
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp>\'    {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(SkipPHPString);
                                          }
                                        }
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute>.       { }
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
<SkipString,SkipPHPString>.             { }
<CompoundName>":"                       { // for "class : public base {} var;" construct, see bug 608359
                                          unput(':');
                                          BEGIN(ClassVar);
                                        }
<CompoundName>";"                       {
                                          yyextra->current->section = Entry::EMPTY_SEC ;
                                          yyextra->current->type.resize(0) ;
                                          yyextra->current->name.resize(0) ;
                                          yyextra->current->args.resize(0) ;
                                          yyextra->current->argList.clear();
                                          BEGIN( FindMembers ) ;
                                        }
<Bases>";"                      {
                                          if (yyextra->insideIDL && (yyextra->current->spec & (Entry::Singleton |
                                                                             Entry::Service)))
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
                                              yyextra->current->extends.push_back(
                                                BaseInfo(yyextra->baseName,Public,Normal));
                                              yyextra->baseName.resize(0);
                                            }
                                            yyextra->current_root->moveToSubEntryAndRefresh( yyextra->current ) ;
                                            initEntry(yyscanner);
                                          }
                                          else
                                          {
                                            yyextra->current->section = Entry::EMPTY_SEC ;
                                            yyextra->current->type.resize(0) ;
                                            yyextra->current->name.resize(0) ;
                                            yyextra->current->args.resize(0) ;
                                            yyextra->current->argList.clear();
                                          }
                                          BEGIN( FindMembers ) ;
                                        }
<CompoundName>{SCOPENAME}/{BN}*"<"      {
                                          yyextra->sharpCount = 0;
                                          yyextra->current->name = yytext ;
                                          if (yyextra->current->spec & Entry::Protocol)
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
                                            if (yyextra->current->spec & Entry::Protocol)
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
<CompoundName>{SCOPENAME}{BN}*";"       { // forward declaration
                                          if (!yyextra->current->tArgLists.empty())
                                          {
                                            // found a forward template declaration, this has
                                            // a purpose of its own
                                            yyextra->current->name = yytext;
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                            //printf("template class declaration for %s!\n",qPrint(yyextra->current->name));
                                            QCString rn = yyextra->current_root->name;
                                            //printf("cn='%s' rn='%s' yyextra->isTypedef=%d\n",qPrint(cn),qPrint(rn),yyextra->isTypedef);
                                            if (!yyextra->current->name.isEmpty() && !rn.isEmpty())
                                            {
                                              prependScope(yyscanner);
                                            }
                                            yyextra->current->spec|=Entry::ForwardDecl;
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          }
                                          else if (yyextra->insideIDL &&
                                                   (((yyextra->current_root->spec & (Entry::Interface |
                                                                            Entry::Service)) &&
                                                     (yyextra->current->spec & Entry::Interface)) ||
                                                    ((yyextra->current_root->spec & (Entry::Service |
                                                                            Entry::Singleton)) &&
                                                     (yyextra->current->spec & Entry::Service))))
                                          {
                                            // interface yyextra->inside of UNO IDL service or interface
                                            // service yyextra->inside of UNO IDL service or singleton
                                            // there may be documentation on the member,
                                            // so do not throw it away...
                                            yyextra->current->name = yytext;
                                            yyextra->current->name=yyextra->current->name.left(yyextra->current->name.length()-1).stripWhiteSpace();
                                            yyextra->current->section = (yyextra->current->spec & Entry::Interface)
                                                ? Entry::EXPORTED_INTERFACE_SEC
                                                : Entry::INCLUDED_SERVICE_SEC;
//                                          yyextra->current->section = Entry::MEMBERDOC_SEC;
                                            yyextra->current->spec &= ~(Entry::Interface|Entry::Service); // FIXME: horrible: Interface == Gettable, so need to clear it - actually we're mixing values from different enums in this case... granted only Optional and Interface are actually valid in this context but urgh...
                                            yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          }

                                          unput(';');
                                          yyextra->current->reset();
                                          initEntry(yyscanner);
                                          if (yyextra->insideObjC) // see bug746361
                                          {
                                            yyextra->language = yyextra->current->lang = SrcLangExt_Cpp;
                                            yyextra->insideObjC = FALSE;
                                          }
                                          if (yyextra->isTypedef) // typedef of a class, put typedef keyword back
                                          {
                                            yyextra->current->type.prepend("typedef");
                                          }
                                          BEGIN( FindMembers );
                                        }
<CompoundName>{SCOPENAME}/{BN}*"("      {
                                          yyextra->current->name = yytext ;
                                          lineCount(yyscanner);
                                          if (yyextra->insideCpp && yyextra->current->name=="alignas") // C++11
                                          {
                                            yyextra->lastAlignAsContext = YY_START;
                                            BEGIN( AlignAs );
                                          }
                                          else
                                          {
                                            if (yyextra->current->spec & Entry::Protocol)
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
                                        }
<ConceptName>"="                        {
                                          yyextra->current->bodyLine = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->initializer.str(std::string());
                                          yyextra->lastInitializerContext = FindMembers;
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
                                          if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
                                          {
                                            yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,yytext);
                                          }
                                          lineCount(yyscanner);
                                          if (yyextra->current->spec & Entry::Protocol)
                                          {
                                            yyextra->current->name += "-p";
                                          }
                                          if ((yyextra->current->spec & Entry::Protocol) ||
                                              yyextra->current->section == Entry::OBJCIMPL_SEC)
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
                                          if (yyextra->insideIDL && qstrncmp(yytext,"switch",6)==0 && !isId(yytext[6]))
                                          {
                                            // Corba IDL style union
                                            yyextra->roundCount=0;
                                            BEGIN(SkipUnionSwitch);
                                          }
                                          else
                                          {
                                            addType(yyscanner);
                                            yyextra->current->name = yytext;
                                            yyextra->current->name = yyextra->current->name.stripWhiteSpace();
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
                                              yyextra->current->spec |= Entry::SealedClass;
                                            else // abstract
                                              yyextra->current->spec |= Entry::AbstractClass;
                                            BEGIN( ClassVar );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<ClassVar>{ID}                          {
                                          if (yyextra->clangParser && (yyextra->insideCpp || yyextra->insideObjC))
                                          {
                                            yyextra->current->id = yyextra->clangParser->lookup(yyextra->yyLineNr,yytext);
                                          }
                                          if (yyextra->insideIDL && qstrcmp(yytext,"switch")==0)
                                          {
                                            // Corba IDL style union
                                            yyextra->roundCount=0;
                                            BEGIN(SkipUnionSwitch);
                                          }
                                          else if ((yyextra->insideJava || yyextra->insidePHP || yyextra->insideJS || yyextra->insideSlice) && (qstrcmp(yytext,"implements")==0 || qstrcmp(yytext,"extends")==0))
                                          {
                                            yyextra->current->type.resize(0);
                                            yyextra->baseProt=Public;
                                            yyextra->baseVirt=Normal;
                                            yyextra->baseName.resize(0);
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
                                            yyextra->current->spec|=Entry::Abstract;
                                          }
                                          else if (yyextra->insideCli &&  qstrcmp(yytext,"sealed")==0)
                                          {
                                            yyextra->current->spec|=Entry::Sealed;
                                          }
                                          else if (qstrcmp(yytext,"final")==0)
                                          {
                                            yyextra->current->spec|=Entry::Final;
                                          }
                                          else
                                          {
                                            if (yyextra->current->section == Entry::ENUM_SEC)
                                            { // found "enum a b" -> variable
                                              yyextra->current->section = Entry::VARIABLE_SEC ;
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
                                            //if (yyextra->current->section!=Entry::OBJCIMPL_SEC)
                                            //{
                                              yyextra->current->spec|=Entry::Category;
                                            //}
                                            BEGIN( ClassCategory );
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
                                          yyextra->fullArgString.resize(0);
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
                                          if ((yyextra->current->section & Entry::Protocol) ||
                                              yyextra->current->section == Entry::OBJCIMPL_SEC)
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
                                          if (yyextra->current->section==Entry::VARIABLE_SEC) // enum A B:2, see bug 748208
                                          {
                                            yyextra->current->bitfields+=":";
                                            yyextra->current->args.resize(0);
                                            BEGIN(BitFields);
                                          }
                                          else if (yyextra->current->section==Entry::ENUM_SEC) // enum E:2, see bug 313527,
                                                                                 // or C++11 style enum: 'E : unsigned int {...}'
                                          {
                                            yyextra->current->args.resize(0);
                                            BEGIN(EnumBaseType);
                                          }
                                          else
                                          {
                                            yyextra->current->type.resize(0);
                                            if ((yyextra->current->spec & Entry::Interface) ||
                                                (yyextra->current->spec & Entry::Struct)    ||
                                                (yyextra->current->spec & Entry::Ref)       ||
                                                (yyextra->current->spec & Entry::Value)     ||
                                                yyextra->insidePHP || yyextra->insideCS || yyextra->insideD || yyextra->insideObjC || yyextra->insideIDL
                                               )
                                              yyextra->baseProt=Public;
                                            else
                                              yyextra->baseProt=Private;
                                            yyextra->baseVirt=Normal;
                                            yyextra->baseName.resize(0);
                                            BEGIN( BasesProt ) ;
                                          }
                                        }
<ClassVar>[;=*&]                        {
                                          unput(*yytext);
                                          if (yyextra->isTypedef) // typedef of a class, put typedef keyword back
                                          {
                                            yyextra->current->type.prepend("typedef");
                                          }
                                          if ((yytext[0]=='*' || yytext[0]=='&') &&
                                              yyextra->current->section == Entry::ENUM_SEC)
                                          { // found "enum a *b" -> variable
                                            yyextra->current->section = Entry::VARIABLE_SEC ;
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
                                            yyextra->current->fileName = yyextra->yyFileName ;
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
                                            yyextra->current->fileName = yyextra->yyFileName ;
                                            yyextra->current->startLine = yyextra->yyLineNr ;
                                            yyextra->current->startColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( ReadBodyIntf );
                                          }
                                        }
<CompoundName,ClassVar>{B}*"{"{B}*      {
                                          yyextra->current->program.str(std::string());
                                          yyextra->current->fileName = yyextra->yyFileName ;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = removeRedundantWhiteSpace(yyextra->current->name);
                                          if (yyextra->current->name.isEmpty() && !yyextra->isTypedef) // anonymous compound
                                          {
                                            if (yyextra->current->section==Entry::NAMESPACE_SEC) // allow reopening of anonymous namespaces
                                            {
                                              if (Config_getBool(EXTRACT_ANON_NSPACES)) // use visible name
                                              {
                                                yyextra->current->name="anonymous_namespace{"+stripPath(yyextra->current->fileName)+"}";
                                              }
                                              else // use invisible name
                                              {
                                                yyextra->current->name.sprintf("@%d",anonNSCount.load());
                                              }
                                            }
                                            else
                                            {
                                              yyextra->current->name.sprintf("@%d",anonCount++);
                                            }
                                          }
                                          yyextra->curlyCount=0;
                                          if (yyextra->current_root && // not a nested struct yyextra->inside an @interface section
                                              !(yyextra->current_root->spec & Entry::Interface) &&
                                              ((yyextra->current->spec & (Entry::Interface | Entry::Protocol | Entry::Category) ||
                                                yyextra->current->section==Entry::OBJCIMPL_SEC)
                                              ) &&
                                              yyextra->insideObjC
                                             )
                                          { // ObjC body that ends with @end
                                            BEGIN( ReadBodyIntf );
                                          }
                                          else if (yyextra->current->section==Entry::NAMESPACE_SEC)
                                          { // namespace body
                                            BEGIN( ReadNSBody );
                                          }
                                          else
                                          { // class body
                                            BEGIN( ReadBody ) ;
                                          }
                                        }
<BasesProt>"virtual"{BN}+               { lineCount(yyscanner); yyextra->baseVirt = Virtual; }
<BasesProt>"public"{BN}+                { lineCount(yyscanner); yyextra->baseProt = Public; }
<BasesProt>"protected"{BN}+             { lineCount(yyscanner); yyextra->baseProt = Protected; }
<BasesProt>"internal"{BN}+              { if (!yyextra->insideCli) REJECT ; lineCount(yyscanner); yyextra->baseProt = Package; }
<BasesProt>"private"{BN}+               { lineCount(yyscanner); yyextra->baseProt = Private; }
<BasesProt>{BN}                         { lineCount(yyscanner); }
<BasesProt>.                            { unput(*yytext); BEGIN(Bases); }
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
<Bases>("::")?{BN}*({ID}{BN}*"::"{BN}*)*{ID}    {
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
                                            yyextra->current->extends.push_back(
                                              BaseInfo(yyextra->baseName,yyextra->baseProt,yyextra->baseVirt)
                                            );
                                          }
                                          if ((yyextra->current->spec & (Entry::Interface|Entry::Struct)) ||
                                              yyextra->insideJava || yyextra->insidePHP || yyextra->insideCS ||
                                              yyextra->insideD || yyextra->insideObjC || yyextra->insideIDL || yyextra->insideSlice)
                                          {
                                            yyextra->baseProt=Public;
                                          }
                                          else
                                          {
                                            yyextra->baseProt=Private;
                                          }
                                          yyextra->baseVirt=Normal;
                                          yyextra->baseName.resize(0);
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
                                          yyextra->current->fileName = yyextra->yyFileName ;
                                          yyextra->current->bodyLine  = yyextra->yyLineNr;
                                          yyextra->current->bodyColumn = yyextra->yyColNr;
                                          yyextra->current->name = removeRedundantWhiteSpace(yyextra->current->name);
                                          if (!yyextra->baseName.isEmpty())
                                            yyextra->current->extends.push_back(
                                               BaseInfo(yyextra->baseName,yyextra->baseProt,yyextra->baseVirt)
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
<Comment>{CMD}("code"|"verbatim")       {
                                          yyextra->insideCode=TRUE;
                                          yyextra->current->program << yytext ;
                                        }
<Comment>{CMD}("endcode"|"endverbatim") {
                                          yyextra->insideCode=FALSE;
                                          yyextra->current->program << yytext ;
                                        }
<Comment>[^ \.\t\r\n\/\*]+              { yyextra->current->program << yytext ; }
<Comment>{CCE}                           { yyextra->current->program << yytext ;
                                          if (!yyextra->insideCode) BEGIN( yyextra->lastContext ) ;
                                        }
<Comment>.                              { yyextra->current->program << *yytext ; }

<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,SkipC11Inits,SkipC11Attribute,Bases,OldStyleArgs>({CPPC}{B}*)?{CCS}"!" {
                                          //printf("Start doc block at %d\n",yyextra->yyLineNr);
                                          if (!yyextra->current->doc.isEmpty())
                                          {
                                            yyextra->current->doc+="\n\n";
                                          }
                                          else
                                          {
                                            yyextra->current->docLine = yyextra->yyLineNr;
                                            yyextra->current->docFile = yyextra->yyFileName;
                                          }

                                          yyextra->lastDocContext = YY_START;
                                          if (yyextra->current_root->section & Entry::SCOPE_MASK)
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = YY_START==SkipCurly;
                                          yyextra->docBlockAutoBrief = Config_getBool(QT_AUTOBRIEF);

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());

                                          if (yyextra->docBlockAutoBrief)
                                          {
                                            yyextra->current->briefLine = yyextra->yyLineNr;
                                            yyextra->current->briefFile = yyextra->yyFileName;
                                          }
                                          startCommentBlock(yyscanner,FALSE);
                                          BEGIN( DocBlock );
                                        }
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,Bases,OldStyleArgs>{CCS}"*"[*]+{BL} {
                                          bool javadocBanner = Config_getBool(JAVADOC_BANNER);
                                          lineCount(yyscanner);

                                          if( javadocBanner )
                                          {
                                            yyextra->lastDocContext = YY_START;

                                            //printf("Found comment banner at %s:%d\n",yyextra->yyFileName,yyextra->yyLineNr);
                                            if (yyextra->current_root->section & Entry::SCOPE_MASK)
                                            {
                                              yyextra->current->inside = yyextra->current_root->name+"::";
                                            }
                                            yyextra->current->docLine = yyextra->yyLineNr;
                                            yyextra->current->docFile = yyextra->yyFileName;
                                            yyextra->docBlockContext = YY_START;
                                            yyextra->docBlockInBody  = YY_START==SkipCurly;
                                            bool javadocAutoBrief = Config_getBool(JAVADOC_AUTOBRIEF);
                                            yyextra->docBlockAutoBrief = javadocAutoBrief;

                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,yyextra->column));
                                            yyextra->docBlock.str(indent.str());

                                            if (yyextra->docBlockAutoBrief)
                                            {
                                              yyextra->current->briefLine = yyextra->yyLineNr;
                                              yyextra->current->briefFile = yyextra->yyFileName;
                                            }
                                            startCommentBlock(yyscanner,FALSE);
                                            BEGIN( DocBlock );
                                          }
                                          else
                                          {
                                            yyextra->current->program << yytext ;
                                            yyextra->lastContext = YY_START ;
                                            BEGIN( Comment ) ;
                                          }
                                        }
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,Bases,OldStyleArgs>({CPPC}{B}*)?{CCS}"*"/{NCOMM} {
                                          yyextra->lastDocContext = YY_START;

                                          //printf("Found comment block at %s:%d\n",yyextra->yyFileName,yyextra->yyLineNr);
                                          if (yyextra->current_root->section & Entry::SCOPE_MASK)
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->current->docLine = yyextra->yyLineNr;
                                          yyextra->current->docFile = yyextra->yyFileName;
                                          yyextra->docBlockContext = YY_START;
                                          yyextra->docBlockInBody  = YY_START==SkipCurly;
                                          bool javadocAutoBrief = Config_getBool(JAVADOC_AUTOBRIEF);
                                          yyextra->docBlockAutoBrief = javadocAutoBrief;

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());

                                          if (yyextra->docBlockAutoBrief)
                                          {
                                            yyextra->current->briefLine = yyextra->yyLineNr;
                                            yyextra->current->briefFile = yyextra->yyFileName;
                                          }
                                          startCommentBlock(yyscanner,FALSE);
                                          BEGIN( DocBlock );
                                        }
<FindMembers,FindFields,MemberSpec,SkipCurly,FuncQual,Operator,ClassVar,Bases,OldStyleArgs>{CPPC}"!" {
                                          yyextra->lastDocContext = YY_START;
                                          if (yyextra->current_root->section & Entry::SCOPE_MASK)
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = YY_START==SkipCurly;
                                          yyextra->docBlockAutoBrief = FALSE;

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());

                                          startCommentBlock(yyscanner,yyextra->current->brief.isEmpty());
                                          BEGIN( DocLine );
                                        }
<FindMembers,FindFields,MemberSpec,SkipCurly,FuncQual,Operator,ClassVar,Bases,OldStyleArgs>{CPPC}"/"/[^/] {
                                          yyextra->lastDocContext = YY_START;
                                          if (yyextra->current_root->section & Entry::SCOPE_MASK)
                                          {
                                            yyextra->current->inside = yyextra->current_root->name+"::";
                                          }
                                          yyextra->docBlockContext   = YY_START;
                                          yyextra->docBlockInBody    = YY_START==SkipCurly;
                                          yyextra->docBlockAutoBrief = FALSE;
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,yyextra->column));
                                          yyextra->docBlock.str(indent.str());
                                          startCommentBlock(yyscanner,yyextra->current->brief.isEmpty());
                                          BEGIN( DocLine );
                                        }
<FindMembers>"extern"{BN}*"\"C"("++")?"\""{BN}*("{")?  {
                                          lineCount(yyscanner);
                                          yyextra->externC=TRUE;
                                        }
<FindMembers>"{"                        {
                                          if (yyextra->externC)
                                          {
                                            yyextra->externC=FALSE;
                                          }
                                          else if (yyextra->insideCS &&
                                              !yyextra->current->name.isEmpty() &&
                                              !yyextra->current->type.isEmpty())
                                          {
                                            if (containsWord(yyextra->current->type,"event")) // event
                                            {
                                              yyextra->current->mtype = yyextra->mtype = Event;
                                            }
                                            else // property
                                            {
                                              yyextra->current->mtype = yyextra->mtype = Property;
                                            }
                                            yyextra->current->bodyLine = yyextra->yyLineNr;
                                            yyextra->current->bodyColumn = yyextra->yyColNr;
                                            yyextra->curlyCount=0;
                                            BEGIN( CSAccessorDecl );
                                          }
                                          else if (yyextra->insideIDL && (yyextra->current->spec & Entry::Attribute))
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
                                              if (yyextra->current->stat)
                                              {
                                                yyextra->current->name="[static initializer]";
                                                yyextra->current->type.resize(0);
                                              }
                                              else
                                              {
                                                yyextra->current->name="[instance initializer]";
                                              }
                                              unput(*yytext);
                                              BEGIN( Function );
                                            }
                                            else
                                            {
                                              // pre C++11 code -> ignore the initializer
                                              //yyextra->needsSemi = TRUE;
                                              //yyextra->current->type.resize(0);
                                              //yyextra->current->name.resize(0);
                                              //yyextra->current->args.resize(0);
                                              //yyextra->current->argList.clear();
                                              //yyextra->curlyCount=0;
                                              //BEGIN( SkipCurlyBlock );

                                              // C++11 style initializer list
                                              yyextra->current->bodyLine = yyextra->yyLineNr;
                                              yyextra->current->bodyColumn = yyextra->yyColNr;
                                              yyextra->current->initializer.str(yytext);
                                              yyextra->lastInitializerContext = YY_START;
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
                                            yyextra->mtype = Method;
                                            yyextra->virt = Normal;
                                            // not really important, but while we are at it
                                            yyextra->current->endBodyLine=yyextra->yyLineNr;
                                            unput(';');
                                            BEGIN(FindMembers);
                                          }
                                        }
<CSAccessorDecl>"private "{BN}*"set"    { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::PrivateSettable;   }
<CSAccessorDecl>"protected "{BN}*"set"  { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::ProtectedSettable; }
<CSAccessorDecl>"private "{BN}*"get"    { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::PrivateGettable;         }
<CSAccessorDecl>"protected "{BN}*"get"  { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::ProtectedGettable; }
<CSAccessorDecl>"set"                   { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::Settable;  }
<CSAccessorDecl>"get"                   { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::Gettable;  }
<CSAccessorDecl>"add"                   { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::Addable;   }
<CSAccessorDecl>"remove"                { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::Removable; }
<CSAccessorDecl>"raise"                 { if (yyextra->curlyCount==0) yyextra->current->spec |= Entry::Raisable;  }
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
                                          if (yyextra->current->spec&Entry::Local)
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
                                          yyextra->current->section = Entry::VARIABLE_SEC;
                                          yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
                                          initEntry(yyscanner);
                                          BEGIN(FindMembers);
                                        }

<SliceDictionary>{SCOPENAME}{BN}*","{BN}*{SCOPENAME} {
                                          lineCount(yyscanner);
                                          if (yyextra->current->spec&Entry::Local)
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
                                          yyextra->current->section = Entry::VARIABLE_SEC;
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
<DocBlock>{CMD}("f$"|"f["|"f{"|"f(")         {
                                          yyextra->docBlock << yytext;
                                          yyextra->docBlockName=&yytext[1];
                                          yyextra->docBlockLine=yyextra->yyLineNr;
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
                                          yyextra->nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{B}*"<"{PRE}">"               {
                                          yyextra->docBlock << yytext;
                                          yyextra->docBlockName="<pre>";
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fencedSize=0;
                                          yyextra->nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}"startuml"/[^a-z_A-Z0-9\-]     { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlock << yytext;
                                          yyextra->docBlockName="uml";
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fencedSize=0;
                                          yyextra->nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{CMD}("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"rtfonly"|"docbookonly"|"dot"|"msc"|"code")/[^a-z_A-Z0-9\-]     { // verbatim command (which could contain nested comments!)
                                          yyextra->docBlock << yytext;
                                          yyextra->docBlockName=&yytext[1];
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fencedSize=0;
                                          yyextra->nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]*                {

                                          QCString pat = substitute(yytext,"*"," ");
                                          yyextra->docBlock << pat;
                                          yyextra->docBlockName="~~~";
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fencedSize=pat.stripWhiteSpace().length();
                                          yyextra->nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*                {
                                          QCString pat = substitute(yytext,"*"," ");
                                          yyextra->docBlock << pat;
                                          yyextra->docBlockName="```";
                                          yyextra->docBlockLine=yyextra->yyLineNr;
                                          yyextra->fencedSize=pat.stripWhiteSpace().length();
                                          yyextra->nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{B}*"<code>"                  {
                                          if (yyextra->insideCS)
                                          {
                                            yyextra->docBlock << yytext;
                                            yyextra->docBlockName="<code>";
                                            yyextra->docBlockLine=yyextra->yyLineNr;
                                            yyextra->nestedComment=FALSE;
                                            BEGIN(DocCopyBlock);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<DocBlock>[^@*~\/\\\n]+                 { // any character that isn't special
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
                                          yyextra->docBlock << yytext;
                                          if (yyextra->docBlockName=="<pre>")
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>"</"{CODE}">"             { // end of a <code> block
                                          yyextra->docBlock << yytext;
                                          if (yyextra->docBlockName=="<code>")
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>[\\@]("f$"|"f]"|"f}"|"f)")     {
                                          yyextra->docBlock << yytext;
                                          if (yyextra->docBlockName==&yytext[1])
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"endrtfonly"|"enddot"|"endmsc"|"enduml"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
                                          yyextra->docBlock << yytext;
                                          if (&yytext[4]==yyextra->docBlockName)
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>^{B}*"*"+/{BN}+           { // start of a comment line
                                          if (yyextra->docBlockName=="verbatim")
                                          {
                                            REJECT;
                                          }
                                          else if (yyextra->docBlockName=="code")
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
                                          if (yyextra->docBlockName=="code")
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
                                          if (yyextra->docBlockName=="code")
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
                                          if (yyextra->docBlockName=="code")
                                          {
                                            QCString indent;
                                            if (yyextra->nestedComment) // keep * it is part of the code
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
                                          yyextra->docBlock << pat;
                                          if (yyextra->fencedSize==pat.stripWhiteSpace().length())
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
                                          QCString pat = substitute(yytext,"*"," ");
                                          yyextra->docBlock << pat;
                                          if (yyextra->fencedSize==pat.stripWhiteSpace().length())
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>[^\<@/\*\]~\$\\\n]+       { // any character that is not special
                                          yyextra->docBlock << yytext;
                                        }
<DocCopyBlock>{CCS}|{CCE}|{CPPC}           {
                                          if (yytext[1]=='*')
                                          {
                                            yyextra->nestedComment=TRUE;
                                          }
                                          else if (yytext[0]=='*')
                                          {
                                            yyextra->nestedComment=FALSE;
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
                                          warn(yyextra->yyFileName,yyextra->yyLineNr,
                                              "reached end of file while inside a '%s' block!\n"
                                              "The command that should end the block seems to be missing (probable line reference: %d)!\n",
                                              qPrint(yyextra->docBlockName), yyextra->docBlockLine);
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
                                            yyextra->current->name.resize(0);
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
                                          yyextra->current->virt = Pure;
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
<*>\"                                   {
                                          if (yyextra->insidePHP)
                                          {
                                            yyextra->lastStringContext=YY_START;
                                            BEGIN(SkipString);
                                          }
                                        }
<*>\?                                   {
                                          if (yyextra->insideCS && (YY_START != SkipRound))
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
<*>.
<SkipComment>{CPPC}|{CCS}
<*>{CCS}                                 { yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipComment ) ;
                                        }
<SkipComment>{B}*{CCE}                   { BEGIN( yyextra->lastCContext ) ; }
<*>{CPPC}                                 {
                                          yyextra->lastCContext = YY_START ;
                                          BEGIN( SkipCxxComment ) ;
                                        }
%%

//----------------------------------------------------------------------------
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t c=0;
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
  yyextra->baseName.resize(0);
  yyextra->protection = Public;
  yyextra->baseProt = Public;
  yyextra->sharpCount = 0;
  yyextra->roundCount = 0;
  yyextra->curlyCount = 0;
  yyextra->mtype = Method;
  yyextra->stat = FALSE;
  yyextra->virt = Normal;
  yyextra->baseVirt = Normal;
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
    yyextra->protection = (yyextra->current_root->spec & (Entry::Interface|Entry::Enum)) ?  Public : Package;
  }
  yyextra->current->protection = yyextra->protection ;
  yyextra->current->mtype      = yyextra->mtype;
  yyextra->current->virt       = yyextra->virt;
  yyextra->current->stat       = yyextra->stat;
  yyextra->current->lang       = yyextra->language;
  //printf("*** initEntry(yyscanner) yyextra->language=%d\n",yyextra->language);
  yyextra->commentScanner.initGroupInfo(yyextra->current.get());
  yyextra->isTypedef=FALSE;
}


//-----------------------------------------------------------------------------

static void lineCount(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int tabSize = Config_getInt(TAB_SIZE);
  const char *p;
  for (p = yytext ; *p ; ++p )
  {
    if (*p=='\n')
    {
      yyextra->yyLineNr++,yyextra->column=0,yyextra->yyColNr=1;
    }
    else if (*p=='\t')
    {
      yyextra->column+=tabSize - (yyextra->column%tabSize);
    }
    else
    {
      yyextra->column++,yyextra->yyColNr++;
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

static void addType(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  uint tl=yyextra->current->type.length();
  if( tl>0 && !yyextra->current->name.isEmpty() && yyextra->current->type.at(tl-1)!='.')
  {
    yyextra->current->type += ' ' ;
  }
  yyextra->current->type += yyextra->current->name ;
  yyextra->current->name.resize(0) ;
  tl=yyextra->current->type.length();
  if( tl>0 && !yyextra->current->args.isEmpty() && yyextra->current->type.at(tl-1)!='.')
  {
    yyextra->current->type += ' ' ;
  }
  yyextra->current->type += yyextra->current->args ;
  yyextra->current->args.resize(0) ;
  yyextra->current->argList.clear();
}


static QCString stripQuotes(const char *s)
{
  QCString name;
  if (s==0 || *s==0) return name;
  name=s;
  if (name.at(0)=='"' && name.at(name.length()-1)=='"')
  {
    name=name.mid(1,name.length()-2);
  }
  return name;
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
  yyextra->language    = getLanguageFromFileName(yyextra->yyFileName);
  yyextra->insideIDL   = yyextra->language==SrcLangExt_IDL;
  yyextra->insideJava  = yyextra->language==SrcLangExt_Java;
  yyextra->insideCS    = yyextra->language==SrcLangExt_CSharp;
  yyextra->insideD     = yyextra->language==SrcLangExt_D;
  yyextra->insidePHP   = yyextra->language==SrcLangExt_PHP;
  yyextra->insideObjC  = yyextra->language==SrcLangExt_ObjC;
  yyextra->insideJS    = yyextra->language==SrcLangExt_JS;
  yyextra->insideSlice = yyextra->language==SrcLangExt_Slice;
  yyextra->insideCpp   = (yyextra->language==SrcLangExt_Cpp ||
                          yyextra->language==SrcLangExt_Lex);
  //printf("setContext(%s) yyextra->insideIDL=%d yyextra->insideJava=%d yyextra->insideCS=%d "
  //       "yyextra->insideD=%d yyextra->insidePHP=%d yyextra->insideObjC=%d\n",
  //       qPrint(yyextra->yyFileName),yyextra->insideIDL,yyextra->insideJava,yyextra->insideCS,yyextra->insideD,yyextra->insidePHP,yyextra->insideObjC
  //    );
}

//-----------------------------------------------------------------------------

static void prependScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->current_root->section & Entry::SCOPE_MASK)
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
  if (((QCString)yyextra->yyFileName).right(2).lower()!=".c") return FALSE; // must be a C file
  if (yyextra->current->argList.empty()) return FALSE; // must have arguments
  for (const Argument &a : yyextra->current->argList)
  {
    // in K&R style argument do not have a type, but doxygen expects a type
    // so it will think the argument has no name
    if (a.type.isEmpty() || !a.name.isEmpty()) return FALSE;
  }
  return TRUE;
}

//-----------------------------------------------------------------------------

static void splitKnRArg(yyscan_t yyscanner,QCString &oldStyleArgPtr,QCString &oldStyleArgName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int si = yyextra->current->args.length();
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
      while (i<si && ((c=yyextra->current->args.at(i))=='*' || isspace((uchar)c))) i++;
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
      while (i<si && ((c=yyextra->current->args.at(i))=='*' || isspace((uchar)c))) i++;
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
      while (i>=0 && ((c=yyextra->current->args.at(i))=='*' || isspace((uchar)c))) i--;
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
    while (j<l && ((c=yyextra->current->args.at(j))=='*' || isspace((uchar)c))) j++;
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
      if (a.type.left(9)=="register ") // strip keyword
      {
        a.type=a.type.mid(9);
      }
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
      a.type.resize(0);
    }
  }
}


static void startCommentBlock(yyscan_t yyscanner,bool brief)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (brief)
  {
    yyextra->current->briefFile = yyextra->yyFileName;
    yyextra->current->briefLine = yyextra->yyLineNr;
  }
  else
  {
    yyextra->current->docFile = yyextra->yyFileName;
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
  bool hideInBodyDocs = Config_getBool(HIDE_IN_BODY_DOCS);
  if (yyextra->docBlockInBody && hideInBodyDocs) return;
  //printf("parseCommentBlock [%s] brief=%d\n",qPrint(doc),brief);
  int lineNr = brief ? yyextra->current->briefLine : yyextra->current->docLine;   // line of block start

  // fill in inbodyFile && inbodyLine the first time, see bug 633891
  std::shared_ptr<Entry> docEntry = yyextra->docBlockInBody && yyextra->previous ? yyextra->previous : yyextra->current;
  if (yyextra->docBlockInBody && docEntry && docEntry->inbodyLine==-1)
  {
    docEntry->inbodyFile = yyextra->yyFileName;
    docEntry->inbodyLine = lineNr;
  }

  int position=0;
  bool needsEntry=FALSE;
  Markdown markdown(yyextra->yyFileName,lineNr);
  QCString strippedDoc = stripIndentation(doc);
  QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,lineNr) : strippedDoc;
  while (yyextra->commentScanner.parseCommentBlock(
        yyextra->thisParser,
        yyextra->docBlockInBody && yyextra->previous ? yyextra->previous.get() : yyextra->current.get(),
        processedDoc, // text
        yyextra->yyFileName,   // file
        lineNr,       // line of block start
        yyextra->docBlockInBody ? FALSE : brief,               // isBrief
        yyextra->docBlockInBody ? FALSE : yyextra->docBlockAutoBrief,   // isJavaDocStyle
        yyextra->docBlockInBody,                               // isInBody
        yyextra->protection,
        position,
        needsEntry,
        Config_getBool(MARKDOWN_SUPPORT)
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
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf(">>>>>>> handleParametersCommentBlocks()\n");
  for (Argument &a : al)
  {
    //printf("    Param %s docs=%s\n",qPrint(a->name),qPrint(a->docs));
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

      yyextra->current->doc.resize(0);
      yyextra->current->brief.resize(0);

      //printf("handleParametersCommentBlock [%s]\n",qPrint(doc));
      while (yyextra->commentScanner.parseCommentBlock(
             yyextra->thisParser,
             yyextra->current.get(),
             a.docs,             // text
             yyextra->yyFileName,         // file
             yyextra->current->docLine,   // line of block start
             FALSE,
             FALSE,
             FALSE,
             yyextra->protection,
             position,
             needsEntry,
             Config_getBool(MARKDOWN_SUPPORT)
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
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("parseCompounds(%s)\n",qPrint(rt->name));
  for (const auto &ce : rt->children())
  {
    if (!ce->program.empty())
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  qPrint(ce->name),qPrint(ce->program));
      // init scanner state
      yyextra->padCount=0;
      //depthIf = 0;
      yyextra->column=0;
      yyextra->programStr = ce->program.str();
      yyextra->inputString = yyextra->programStr.data();
      yyextra->inputPosition = 0;
      if (ce->section==Entry::ENUM_SEC || (ce->spec&Entry::Enum))
        BEGIN( FindFields ) ;
      else
        BEGIN( FindMembers ) ;
      yyextra->current_root = ce;
      yyextra->yyFileName = ce->fileName;
      //setContext();
      yyextra->yyLineNr = ce->bodyLine;
      yyextra->yyColNr = ce->bodyColumn;
      yyextra->insideObjC = ce->lang==SrcLangExt_ObjC;
      //printf("---> Inner block starts at line %d objC=%d\n",yyextra->yyLineNr,yyextra->insideObjC);
      yyextra->current = std::make_shared<Entry>();
      yyextra->stat = FALSE;
      initEntry(yyscanner);

      // deep copy group list from parent (see bug 727732)
      bool autoGroupNested = Config_getBool(GROUP_NESTED_COMPOUNDS);
      if (autoGroupNested && !rt->groups.empty() && ce->section!=Entry::ENUM_SEC && !(ce->spec&Entry::Enum))
      {
        ce->groups = rt->groups;
      }

      int ni=ce->name.findRev("::"); if (ni==-1) ni=0; else ni+=2;
      // set default protection based on the compound type
      if( ce->section==Entry::CLASS_SEC ) // class
      {
        if (yyextra->insidePHP || yyextra->insideD || yyextra->insideJS || yyextra->insideIDL || yyextra->insideSlice)
        {
          yyextra->current->protection = yyextra->protection = Public ;
        }
        else if (yyextra->insideJava)
        {
          yyextra->current->protection = yyextra->protection = (ce->spec & (Entry::Interface|Entry::Enum)) ?  Public : Package;
        }
        else if (ce->spec&(Entry::Interface | Entry::Ref | Entry::Value | Entry::Struct | Entry::Union))
        {
          if (ce->lang==SrcLangExt_ObjC)
          {
            yyextra->current->protection = yyextra->protection = Protected ;
          }
          else
          {
            yyextra->current->protection = yyextra->protection = Public ;
          }
        }
        else
        {
          yyextra->current->protection = yyextra->protection = Private ;
        }
      }
      else if (ce->section == Entry::ENUM_SEC ) // enum
      {
        yyextra->current->protection = yyextra->protection = ce->protection;
      }
      else if (!ce->name.isEmpty() && ce->name.at(ni)=='@') // unnamed union or namespace
      {
        if (ce->section == Entry::NAMESPACE_SEC ) // unnamed namespace
        {
          yyextra->current->stat = yyextra->stat = TRUE;
        }
        yyextra->current->protection = yyextra->protection = ce->protection;
      }
      else // named struct, union, protocol, category
      {
        yyextra->current->protection = yyextra->protection = Public ;
      }
      yyextra->mtype = Method;
      yyextra->virt = Normal;
      //printf("name=%s yyextra->current->stat=%d yyextra->stat=%d\n",qPrint(ce->name),yyextra->current->stat,yyextra->stat);

      //memberGroupId = DOX_NOGROUP;
      //memberGroupRelates.resize(0);
      //memberGroupInside.resize(0);
      QCString name = ce->name;
      yyextra->commentScanner.enterCompound(yyextra->yyFileName,yyextra->yyLineNr,name);

      scannerYYlex(yyscanner);
      yyextra->lexInit=TRUE;
      //forceEndGroup();

      yyextra->commentScanner.leaveCompound(yyextra->yyFileName,yyextra->yyLineNr,name);

      yyextra->programStr.resize(0);
      ce->program.str(std::string());


      //if (depthIf>0)
      //{
      //        warn(yyextra->yyFileName,yyextra->yyLineNr,"Documentation block ended in the middle of a conditional section!");
      //}
    }
    parseCompounds(yyscanner,ce);
  }
}

//----------------------------------------------------------------------------

static void parseMain(yyscan_t yyscanner,
                      const QCString &fileName,
                      const char *fileBuf,
                      const std::shared_ptr<Entry> &rt,
                      ClangTUParser *clangParser)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  initParser(yyscanner);

  yyextra->inputString = fileBuf;
  yyextra->inputPosition = 0;
  yyextra->column = 0;
  scannerYYrestart(0,yyscanner);

  //depthIf       = 0;
  yyextra->protection    = Public;
  yyextra->mtype         = Method;
  yyextra->stat          = FALSE;
  yyextra->virt          = Normal;
  yyextra->current_root  = rt;
  yyextra->yyLineNr      = 1 ;
  yyextra->yyBegLineNr   = 1;
  yyextra->yyBegColNr    = 0;
  yyextra->yyFileName = fileName;
  yyextra->clangParser = clangParser;
  setContext(yyscanner);
  rt->lang = yyextra->language;
  msg("Parsing file %s...\n",qPrint(yyextra->yyFileName));

  yyextra->current_root  = rt;
  initParser(yyscanner);
  yyextra->commentScanner.enterFile(yyextra->yyFileName,yyextra->yyLineNr);
  yyextra->current = std::make_shared<Entry>();
  //printf("yyextra->current=%p yyextra->current_root=%p\n",yyextra->current,yyextra->current_root);
  int sec=guessSection(yyextra->yyFileName);
  if (sec)
  {
    yyextra->current->name    = yyextra->yyFileName;
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
    yyextra->current->fileName = yyextra->yyFileName;
    yyextra->current->section=Entry::USINGDIR_SEC;
    yyextra->current_root->moveToSubEntryAndRefresh(yyextra->current);
    initEntry(yyscanner);
    BEGIN( FindMembers );
  }
  else
  {
    BEGIN( FindMembers );
  }

  scannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  if (YY_START==Comment)
  {
    warn(yyextra->yyFileName,yyextra->yyLineNr,"File ended in the middle of a comment block! Perhaps a missing \\endcode?");
  }

  //forceEndGroup();
  yyextra->commentScanner.leaveFile(yyextra->yyFileName,yyextra->yyLineNr);

  yyextra->programStr.resize(0);
  rt->program.str(std::string());

  parseCompounds(yyscanner,rt);

  anonNSCount++;

  // add additional entries that were created during processing
  for (auto &kv: yyextra->outerScopeEntries)
  {
    //printf(">>> adding '%s' to scope '%s'\n",qPrint(kv.second->name),qPrint(kv.first->name));
    kv.first->moveToSubEntryAndKeep(kv.second);
  }
  yyextra->outerScopeEntries.clear();

}

//----------------------------------------------------------------------------

static void parsePrototype(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("**** parsePrototype(%s) begin\n",qPrint(text));
  if (text.isEmpty())
  {
    warn(yyextra->yyFileName,yyextra->yyLineNr,"Empty prototype found!");
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
  yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner), yyscanner);
  orgInputString = yyextra->inputString;
  orgInputPosition = yyextra->inputPosition;

  // set new string
  yyextra->inputString = text.data();
  yyextra->inputPosition = 0;
  yyextra->column = 0;
  scannerYYrestart(0, yyscanner);
  BEGIN(Prototype);
  scannerYYlex(yyscanner);
  yyextra->lexInit=TRUE;

  yyextra->current->name = yyextra->current->name.stripWhiteSpace();
  if (yyextra->current->section == Entry::MEMBERDOC_SEC && yyextra->current->args.isEmpty())
    yyextra->current->section = Entry::VARIABLEDOC_SEC;

  // restore original scanner state
  YY_BUFFER_STATE tmpState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(orgState, yyscanner);
  yy_delete_buffer(tmpState, yyscanner);
  yyextra->inputString = orgInputString;
  yyextra->inputPosition = orgInputPosition;


  //printf("**** parsePrototype end\n");
}

//static void handleGroupStartCommand(const char *header)
//{
//  memberGroupHeader=header;
//  startGroupInDoc();
//}
//
//static void handleGroupEndCommand()
//{
//  endGroup();
//  g_previous=0;
//}

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
  scannerYYset_debug(1,p->yyscanner);
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
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->thisParser = this;

  printlex(yy_flex_debug, TRUE, __FILE__, qPrint(fileName));

  ::parseMain(p->yyscanner,fileName,fileBuf,root,clangParser);

  printlex(yy_flex_debug, FALSE, __FILE__, qPrint(fileName));
}


bool COutlineParser::needsPreprocessing(const QCString &extension) const
{
  QCString fe=extension.lower();
  SrcLangExt lang = getLanguageFromFileName(extension);
  return (SrcLangExt_Cpp == lang) || (SrcLangExt_Lex == lang) ||
   !( fe==".java" || fe==".as"  || fe==".d"    || fe==".php" ||
      fe==".php4" || fe==".inc" || fe==".phtml"|| fe==".php5"
    );
}

void COutlineParser::parsePrototype(const QCString &text)
{
  ::parsePrototype(p->yyscanner,text);
}

//----------------------------------------------------------------------------

#if USE_STATE2STRING
#include "scanner.l.h"
#endif
