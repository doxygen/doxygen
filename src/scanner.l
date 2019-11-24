/*****************************************************************************
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
%option prefix="scannerYY"

%{

/*
 *	includes
 */

#include <algorithm>
#include <vector>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include <qarray.h>
#include <qstack.h>
#include <qregexp.h>
#include <qfile.h>

#include "scanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "code.h"
#include "arguments.h"

#include "clangparser.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

/* -----------------------------------------------------------------
 *
 *	statics
 */
static ParserInterface *g_thisParser;
static const char *     g_inputString = 0;
static int		g_inputPosition = 0;
static int		g_lastContext = 0;
static int		g_lastCContext = 0;
static int              g_lastDocContext = 0;
static int              g_lastCPPContext = 0;
static int              g_lastSkipSharpContext = 0;
static int              g_lastSkipRoundContext = 0;
static int              g_lastStringContext = 0;
static int              g_lastCurlyContext = 0;
static int              g_lastRoundContext = 0;
static int              g_lastSquareContext = 0;
static int              g_lastInitializerContext = 0;
static int              g_lastClassTemplSpecContext = 0;
static int              g_lastPreLineCtrlContext = 0;
static int              g_lastSkipVerbStringContext = 0;
static int              g_lastCommentInArgContext = 0;
static int              g_lastRawStringContext = 0;
static int              g_lastCSConstraint = 0;
static int              g_lastHereDocContext = 0;
static int              g_lastDefineContext = 0;
static int              g_lastAlignAsContext = 0;
static int              g_lastC11AttributeContext = 0;
static int              g_lastModifierContext = 0;
static Protection	g_protection = Public;
static Protection	g_baseProt = Public;
static int		g_sharpCount   = 0 ;
static int		g_roundCount   = 0 ;
static int		g_curlyCount   = 0 ;
static int		g_squareCount  = 0 ;
static int              g_padCount     = 0 ;
static std::unique_ptr<Entry> g_current;
static Entry*		g_current_root = 0 ;
static Entry*		g_previous     = 0 ;
static std::unique_ptr<Entry> g_tempEntry;
static Entry*		g_firstTypedefEntry = 0 ;
static Entry*		g_memspecEntry = 0 ;
static int		g_yyLineNr     = 1 ;
static int		g_yyBegLineNr  = 1 ;
static int		g_yyColNr      = 1 ;
static int		g_yyBegColNr   = 1 ;
static int              g_anonCount    = 0 ;
static int              g_anonNSCount  = 0 ;
static QCString		g_yyFileName;
static MethodTypes 	g_mtype = Method;
static bool    		g_stat = false;
static Specifier 	g_virt = Normal;
static Specifier 	g_baseVirt = Normal;
static QCString         g_msType;
static QCString         g_msName;
static QCString         g_msArgs;
static bool             g_isTypedef = false;
static QCString         g_funcPtrType;
static QCString         g_templateStr;
static QCString         g_aliasName;
static QCString         g_baseName;
static QCString*        g_specName = 0;

static SrcLangExt       g_language = SrcLangExt_Unknown;
static bool             g_insideIDL   = false;		//!< processing IDL code?
static bool             g_insideJava  = false;		//!< processing Java code?
static bool             g_insideCS    = false;		//!< processing C# code?
static bool             g_insideD     = false;		//!< processing D code?
static bool             g_insidePHP   = false;		//!< processing PHP code?
static bool             g_insideObjC  = false;            //!< processing Objective C code?
static bool             g_insideCli   = false;            //!< processing C++/CLI code?
static bool             g_insideJS    = false;            //!< processing JavaScript code?
static bool             g_insideSlice = false;            //!< processing Slice code?
static bool             g_insideCpp   = true;             //!< processing C/C++ code

static bool             g_insideCppQuote = false;
static bool             g_insideProtocolList = false;

static int              g_argRoundCount = 0;
static int              g_argSharpCount = 0;
static int              g_currentArgumentContext = 0;
static int              g_lastCopyArgStringContext = 0;
static int              g_lastCopyArgContext = 0;
static QCString        *g_copyArgString = 0;
static QCString         g_fullArgString;
static QCString         g_dummyRawString;

static ArgumentList    *g_currentArgumentList = 0;
static char             g_lastCopyArgChar = '\0';

static QCString        *g_pCopyQuotedString = 0;
static QCString        *g_pCopyRoundString = 0;
static QCString        *g_pCopyCurlyString = 0;
static QCString        *g_pCopyRawString = 0;
static QGString        *g_pCopyCurlyGString = 0;
static QGString        *g_pCopyRoundGString = 0;
static QGString        *g_pCopySquareGString = 0;
static QGString        *g_pCopyQuotedGString = 0;
static QGString        *g_pCopyHereDocGString = 0;
static QGString        *g_pCopyRawGString = 0;
static QGString        *g_pSkipVerbString = 0;
static QStack<Grouping> g_autoGroupStack;

static bool             g_insideFormula = false;
static bool  	        g_insideTryBlock = false;
static bool             g_insideCode = false;
static bool             g_needsSemi = false;

static int  	        g_initBracketCount = 0;

static QCString         g_oldStyleArgType;
static QCString         g_docBackup;
static QCString         g_briefBackup;

static int              g_docBlockContext = 0;
static QGString         g_docBlock;
static QCString         g_docBlockName;
static bool             g_docBlockInBody = false;
static bool             g_docBlockAutoBrief = false;
static char             g_docBlockTerm = '\0';

static QCString         g_idlAttr;
static QCString         g_idlProp;
static bool             g_odlProp = false;

static bool             g_lexInit = false;
static bool             g_externC = false;

static QCString         g_delimiter;

static int              g_column = 0;

static int              g_fencedSize = 0;
static bool             g_nestedComment = 0;
static std::vector< std::pair<Entry*,std::unique_ptr<Entry> > > g_outerScopeEntries;
static CodeScanner      g_codeScanner;

static const char *stateToString(int state);
//-----------------------------------------------------------------------------

// forward declarations
//static void handleGroupStartCommand(const char *header);
//static void handleGroupEndCommand();

//-----------------------------------------------------------------------------

static void initParser()
{
  g_outerScopeEntries.clear();
  g_baseName.resize(0);
  g_protection = Public;
  g_baseProt = Public;
  g_sharpCount = 0;
  g_roundCount = 0;
  g_curlyCount = 0;
  g_mtype = Method;
  g_stat = FALSE;
  g_virt = Normal;
  g_baseVirt = Normal;
  g_isTypedef = FALSE;
  g_autoGroupStack.clear();
  g_insideTryBlock = FALSE;
  g_autoGroupStack.setAutoDelete(TRUE);
  g_insideFormula = FALSE;
  g_insideCode=FALSE;
  g_insideCli=Config_getBool(CPP_CLI_SUPPORT);
  g_previous = 0;
  g_firstTypedefEntry = 0;
  g_memspecEntry =0;
}

static void initEntry()
{
  if (g_insideJava) 
  {
    g_protection = (g_current_root->spec & (Entry::Interface|Entry::Enum)) ?  Public : Package;
  }
  g_current->protection = g_protection ;
  g_current->mtype      = g_mtype;
  g_current->virt       = g_virt;
  g_current->stat       = g_stat;
  g_current->lang       = g_language;
  //printf("*** initEntry() g_language=%d\n",g_language);
  Doxygen::docGroup.initGroupInfo(g_current.get());
  g_isTypedef=FALSE;
}


//-----------------------------------------------------------------------------

static void lineCount()
{
  int tabSize = Config_getInt(TAB_SIZE);
  const char *p;
  for (p = yytext ; *p ; ++p )
  {
    if (*p=='\n') 
    {
      g_yyLineNr++,g_column=0,g_yyColNr=1;
    }
    else if (*p=='\t') 
    {
      g_column+=tabSize - (g_column%tabSize);
    }
    else 
    {
      g_column++,g_yyColNr++;
    }
  }
  //printf("lineCount()=%d\n",g_column);
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

static void addType()
{
    uint tl=g_current->type.length();
    if( tl>0 && !g_current->name.isEmpty() && g_current->type.at(tl-1)!='.') 
    {
      g_current->type += ' ' ;
    }
    g_current->type += g_current->name ;
    g_current->name.resize(0) ;
    tl=g_current->type.length();
    if( tl>0 && !g_current->args.isEmpty() && g_current->type.at(tl-1)!='.') 
    {
      g_current->type += ' ' ;
    }
    g_current->type += g_current->args ;
    g_current->args.resize(0) ;
    g_current->argList.clear();
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

static void startCommentBlock(bool);
static void handleCommentBlock(const QCString &doc,bool brief);
static void handleParametersCommentBlocks(ArgumentList &al);

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

static void setContext()
{
  g_language    = getLanguageFromFileName(g_yyFileName);
  g_insideIDL   = g_language==SrcLangExt_IDL;
  g_insideJava  = g_language==SrcLangExt_Java;
  g_insideCS    = g_language==SrcLangExt_CSharp; 
  g_insideD     = g_language==SrcLangExt_D;
  g_insidePHP   = g_language==SrcLangExt_PHP;
  g_insideObjC  = g_language==SrcLangExt_ObjC;
  g_insideJS    = g_language==SrcLangExt_JS;
  g_insideSlice = g_language==SrcLangExt_Slice;
  g_insideCpp   = g_language==SrcLangExt_Cpp;
  //printf("setContext(%s) g_insideIDL=%d g_insideJava=%d g_insideCS=%d "
  //       "g_insideD=%d g_insidePHP=%d g_insideObjC=%d\n",
  //       g_yyFileName.data(),g_insideIDL,g_insideJava,g_insideCS,g_insideD,g_insidePHP,g_insideObjC
  //	);
}

//-----------------------------------------------------------------------------

static void prependScope()
{
  if (g_current_root->section & Entry::SCOPE_MASK)
  {
    //printf("--- prependScope %s to %s\n",g_current_root->name.data(),g_current->name.data());
    g_current->name.prepend(g_current_root->name+"::");
    //printf("prependScope #=%d #g_current=%d\n",g_current_root->tArgLists->count(),g_current->tArgLists->count());
    for (const ArgumentList &srcAl : g_current_root->tArgLists)
    {
      g_current->tArgLists.insert(g_current->tArgLists.begin(),srcAl);
    }
  }
}

//-----------------------------------------------------------------------------

/*! Returns TRUE iff the g_current entry could be a K&R style C function */
static bool checkForKnRstyleC()
{
  if (((QCString)g_yyFileName).right(2).lower()!=".c") return FALSE; // must be a C file
  if (g_current->argList.empty()) return FALSE; // must have arguments
  for (const Argument &a : g_current->argList)
  {
    // in K&R style argument do not have a type, but doxygen expects a type
    // so it will think the argument has no name
    if (a.type.isEmpty() || !a.name.isEmpty()) return FALSE;
  }
  return TRUE;
}

//-----------------------------------------------------------------------------

static void splitKnRArg(QCString &oldStyleArgPtr,QCString &oldStyleArgName)
{
  int si = g_current->args.length();
  if (g_oldStyleArgType.isEmpty()) // new argument
  {
    static QRegExp re("([^)]*)");
    int bi1 = g_current->args.findRev(re);
    int bi2 = bi1!=-1 ? g_current->args.findRev(re,bi1-1) : -1; 
    char c;
    if (bi1!=-1 && bi2!=-1) // found something like "int (*func)(int arg)"
    {
      int s=bi2+1;
      g_oldStyleArgType = g_current->args.left(s);
      int i=s;
      while (i<si && ((c=g_current->args.at(i))=='*' || isspace((uchar)c))) i++;
      g_oldStyleArgType += g_current->args.mid(s,i-s);
      s=i;
      while (i<si && isId(g_current->args.at(i))) i++;
      oldStyleArgName = g_current->args.mid(s,i-s);
      g_oldStyleArgType+=g_current->args.mid(i);
    }
    else if (bi1!=-1) // redundant braces like in "int (*var)"
    {
      int s=bi1;
      g_oldStyleArgType = g_current->args.left(s);
      s++;
      int i=s+1;
      while (i<si && ((c=g_current->args.at(i))=='*' || isspace((uchar)c))) i++;
      g_oldStyleArgType += g_current->args.mid(s,i-s);
      s=i;
      while (i<si && isId(g_current->args.at(i))) i++;
      oldStyleArgName = g_current->args.mid(s,i-s);
    }
    else // normal "int *var"
    {
      int l=si,i=l-1,j;
      char c;
      // look for start of name in "type *name"
      while (i>=0 && isId(g_current->args.at(i))) i--;
      j=i+1;
      // look for start of *'s
      while (i>=0 && ((c=g_current->args.at(i))=='*' || isspace((uchar)c))) i--;
      i++;
      if (i!=l)
      {
	g_oldStyleArgType=g_current->args.left(i);
	oldStyleArgPtr=g_current->args.mid(i,j-i);
	oldStyleArgName=g_current->args.mid(j).stripWhiteSpace();
      }
      else
      {
	oldStyleArgName=g_current->args.copy().stripWhiteSpace();
      }
    }
  }
  else // continuation like *arg2 in "int *args,*arg2"
  {
    int l=si,j=0;
    char c;
    while (j<l && ((c=g_current->args.at(j))=='*' || isspace((uchar)c))) j++;
    if (j>0)
    {
      oldStyleArgPtr=g_current->args.left(j);
      oldStyleArgName=g_current->args.mid(j).stripWhiteSpace();
    }
    else
    {
      oldStyleArgName=g_current->args.copy().stripWhiteSpace();
    }
  }
}

//-----------------------------------------------------------------------------

/*! Update the argument \a name with additional \a type info. For K&R style
 *  function the type is found \e after the argument list, so this routine
 *  in needed to fix up.
 */
static void addKnRArgInfo(const QCString &type,const QCString &name,
                          const QCString &brief,const QCString &docs)
{
  for (Argument &a : g_current->argList)
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

/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
    int c=0;
    while( c < max_size && g_inputString[g_inputPosition] )
    {
      *buf = g_inputString[g_inputPosition++] ;
      //printf("%d (%c)\n",*buf,*buf);
      c++; buf++;
    }
    return c;
}

%}

       /* start command character */
CMD	  ("\\"|"@")
BN        [ \t\n\r]
BL        [ \t\r]*"\n" 
B         [ \t]
ID        "$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
SCOPENAME "$"?(({ID}?{BN}*"::"{BN}*)*)(((~|!){BN}*)?{ID})
TSCOPE    {ID}("<"[a-z_A-Z0-9 \t\*\&,:]*">")?
CSSCOPENAME (({ID}?{BN}*"."{BN}*)*)((~{BN}*)?{ID})
PRE       [pP][rR][eE]
CODE      [cC][oO][dD][eE]
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))
PHPKW	  ("require"|"require_once"|"include"|"include_once"|"echo")[^a-zA-Z0-9_;]
PHPUSEKW  ("public"|"private"|"protected")
IDLATTR   ("["[^\]]*"]"){BN}*
TYPEDEFPREFIX (("typedef"{BN}+)?)((("volatile"|"const"){BN}+)?)
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"
ARITHOP   "+"|"-"|"/"|"*"|"%"|"--"|"++"
ASSIGNOP  "="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|="
LOGICOP   "=="|"!="|">"|"<"|">="|"<="|"&&"|"||"|"!"
BITOP     "&"|"|"|"^"|"<<"|">>"|"~"
OPERATOR  "operator"{B}*({ARITHOP}|{ASSIGNOP}|{LOGICOP}|{BITOP})

%option noyywrap

  /* language parsing states */

%x      AlignAs
%x      AlignAsEnd
%x	Define
%x	DefineEnd
%x	CompoundName
%x	ClassVar
%x	CSConstraintName
%x	CSConstraintType
%x 	CSIndexer
%x	ClassCategory
%x	ClassTemplSpec
%x      CliPropertyType
%x      CliPropertyIndex
%x      CliOverride
%x	Bases
%x	BasesProt
%x	NextSemi
%x	BitFields
%x	EnumBaseType
%x	FindMembers
%x	FindMembersPHP
%x	FindMemberName
%x      FindFields
%x      FindFieldArg
%x	Function
%x	FuncRound
%x	ExcpRound
%x	ExcpList
%x	FuncQual
%x      TrailingReturn
%x	Operator
%x	Array
%x	ReadBody
%x	ReadNSBody
%x	ReadBodyIntf
%x	Using
%x	UsingAlias
%x	UsingAliasEnd
%x	UsingDirective
%x	SkipCurly
%x	SkipCurlyCpp
%x	SkipCurlyEndDoc
%x      SkipString
%x      SkipPHPString
%x	SkipInits
%x	SkipC11Inits
%x      SkipC11Attribute
%x	SkipCPP
%x	SkipCPPBlock
%x	SkipComment
%x	SkipCxxComment
%x      SkipCurlyBlock
%x      SkipRoundBlock
%x	Sharp
%x	SkipRound
%x	SkipSquare
%x	SkipRemainder
%x      StaticAssert
%x      DeclType
%x	TypedefName
%x	TryFunctionBlock
%x	TryFunctionBlockEnd
%x	Comment
%x	PackageName
%x	JavaImport
%x	PHPUse
%x	PHPUseAs
%x	CSAccessorDecl
%x	CSGeneric
%x	PreLineCtrl
%x	DefinePHP
%x	DefinePHPEnd
%x	OldStyleArgs
%x	SkipVerbString
%x      ObjCMethod
%x      ObjCReturnType
%x      ObjCParams
%x      ObjCParamType
%x      ObjCProtocolList
%x 	ObjCPropAttr
%x 	ObjCSkipStatement
%x	QtPropType
%x	QtPropName
%x	QtPropAttr
%x	QtPropRead
%x	QtPropWrite
%x	ReadInitializer
%x	UNOIDLAttributeBlock
%x	GetCallType
%x	CppQuote
%x	EndCppQuote
%x      MemberSpec
%x      MemberSpecSkip
%x	EndTemplate
%x	FuncPtr
%x	FuncPtrOperator
%x	EndFuncPtr
%x	ReadFuncArgType
%x	ReadTempArgs
%x	IDLUnionCase
%x	NSAliasName
%x	NSAliasArg
%x	CopyString
%x	CopyPHPString
%x	CopyGString
%x	CopyPHPGString
%x	CopyRound
%x	CopyCurly
%x	GCopyRound
%x	GCopySquare
%x	GCopyCurly
%x	SkipUnionSwitch
%x	Specialization
%x	SpecializationSingleQuote
%x	SpecializationDoubleQuote
%x	FuncPtrInit
%x	FuncFunc
%x	FuncFuncEnd
%x	FuncFuncType
%x	FuncFuncArray
%x      CopyArgString
%x      CopyArgPHPString
%x	CopyArgRound
%x	CopyArgSharp
%x	CopyArgComment
%x	CopyArgCommentLine
%x	CopyArgVerbatim
%x	HereDoc
%x	HereDocEnd
%x	CopyHereDoc
%x	CopyHereDocEnd
%x	RawString
%x	RawGString
%x	CSString

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

%x	DocLine
%x	DocBlock
%x      DocCopyBlock

%%

<NextSemi>"{"				{
  					  g_curlyCount=0;
					  g_needsSemi = TRUE;
  					  BEGIN(SkipCurlyBlock); 
					}
<NextSemi>"("				{
  				 	  g_roundCount=0;
  					  BEGIN(SkipRoundBlock);
  					}
<SkipRoundBlock>"("			{
					  ++g_roundCount;
  					}
<SkipRoundBlock>")"			{
  					  if (g_roundCount )
					    --g_roundCount ;
					  else
					    BEGIN( NextSemi ) ;
  					}
<SkipCurlyBlock>"{"			{
  					  ++g_curlyCount ; 
					}
<SkipCurlyBlock>"}"			{ 
  				          if( g_curlyCount )
					  {
					    --g_curlyCount ;
					  }
					  else if (g_needsSemi)
					  {
					    BEGIN( NextSemi );
					  }
					  else
					  {
					    BEGIN( FindMembers );
					  }
					}
<NextSemi>\'				{
  					  if (g_insidePHP)
					  {
					    g_lastStringContext=NextSemi;
					    BEGIN(SkipPHPString);
					  }
  					}
<NextSemi>{CHARLIT}			{ if (g_insidePHP) REJECT; }
<NextSemi>\"				{
  					  g_lastStringContext=NextSemi;
					  BEGIN(SkipString);
  					}
<NextSemi>[;,]				{ 
  					  unput(*yytext);
  					  BEGIN( FindMembers ); 
					}
<BitFields>[;,]				{
  					  unput(*yytext);
					  BEGIN( FindMembers );
  					}
<EnumBaseType>[{;,]                     {
                                          g_current->args = g_current->args.simplifyWhiteSpace();
  					  unput(*yytext);
					  BEGIN( ClassVar );
                                        }
<FindMembers>"<?php"			{ // PHP code with unsupported extension?
                                          g_insidePHP = TRUE;
  					}
<FindMembersPHP>"<?"("php"?)            { // PHP code start
                                           BEGIN( FindMembers );
					}
<FindMembersPHP>"<script"{BN}+"language"{BN}*"="{BN}*['"]?"php"['"]?{BN}*">" { // PHP code start
					  lineCount() ;
                                          BEGIN( FindMembers );
					}
<FindMembers>"?>"|"</script>"           { // PHP code end
					  if (g_insidePHP)
					    BEGIN( FindMembersPHP );
					  else
					    REJECT;
                                        }
<FindMembersPHP>[^\n<]+                 { // Non-PHP code text, ignore
  					}
<FindMembersPHP>\n                      { // Non-PHP code text, ignore
                                          lineCount();
  					}
<FindMembersPHP>.                       { // Non-PHP code text, ignore
				        }
<FindMembers>{PHPKW}			{ if (g_insidePHP)
					    BEGIN( NextSemi );
					  else
					    REJECT;
					}
<FindMembers>"%{"[^\n]*			{ // Mozilla XPIDL lang-specific block
					  if (!g_insideIDL)
					    REJECT;
					}
<FindMembers>"%}"			{ // Mozilla XPIDL lang-specific block end
					  if (!g_insideIDL)
					    REJECT;
					}
<FindMembers>{B}*("properties"){BN}*":"{BN}*  { // IDL or Borland C++ builder property 
  					  g_current->mtype = g_mtype = Property;
					  g_current->protection = g_protection = Public ;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
					}

<FindMembers>{B}*"k_dcop"{BN}*":"{BN}*  { g_current->mtype = g_mtype = DCOP;
					  g_current->protection = g_protection = Public ;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
					}

<FindMembers>{B}*("signals"|"Q_SIGNALS"){BN}*":"{BN}* { g_current->mtype = g_mtype = Signal;
  
					  g_current->protection = g_protection = Public ;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
					}

<FindMembers>{B}*"public"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
					  g_current->protection = g_protection = Public ;
					  g_current->mtype = g_mtype = Slot;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount();
					}

<FindMembers>{B}*"protected"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
					  g_current->protection = g_protection = Protected ;
					  g_current->mtype = g_mtype = Slot;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount();
					}

<FindMembers>{B}*"private"{BN}*("slots"|"Q_SLOTS"){BN}*":"{BN}* {
					  g_current->protection = g_protection = Private ;
					  g_current->mtype = g_mtype = Slot;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount();
					}
<FindMembers>{B}*("public"|"methods"|"__published"){BN}*":"{BN}* { 
					  g_current->protection = g_protection = Public ;
					  g_current->mtype = g_mtype = Method;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
					}
<FindMembers>{B}*"internal"{BN}*":"{BN}* {  // for now treat C++/CLI's internal as package...
  					  if (g_insideCli)
					  {
					    g_current->protection = g_protection = Package ;
					    g_current->mtype = g_mtype = Method;
					    g_current->type.resize(0); 
					    g_current->name.resize(0); 
					    g_current->args.resize(0);
					    g_current->argList.clear();
					    lineCount() ;
					  }
					  else
					  {
					    REJECT;
					  }
  					}
<FindMembers>{B}*"protected"{BN}*":"{BN}* {  
  					  g_current->protection = g_protection = Protected ;
					  g_current->mtype = g_mtype = Method;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
					}
<FindMembers>{B}*"private"{BN}*":"{BN}*	{ 
  					  g_current->protection = g_protection = Private ;
					  g_current->mtype = g_mtype = Method;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
					}
<FindMembers>{B}*"event"{BN}+           { 
  					  if (g_insideCli)
					  {
					    // C++/CLI event
					    lineCount() ;
  					    g_current->mtype = g_mtype = Event;
					    g_current->bodyLine = g_yyLineNr;
					    g_curlyCount=0;
					    BEGIN( CliPropertyType );
					  }
					  else if (g_insideCS)
					  {
					    lineCount() ;
  					    g_current->mtype = Event;
					    g_current->bodyLine = g_yyLineNr;
					  }
					  else
					  {
					    REJECT;
					  }
                                        }
<FindMembers>{B}*"property"{BN}+	{
  					   if (g_insideCli)
					   {
					     // C++/CLI property
					     lineCount() ;
  					     g_current->mtype = g_mtype = Property;
					     g_current->bodyLine = g_yyLineNr;
					     g_curlyCount=0;
					     BEGIN( CliPropertyType );
					   }
					   else
					   {
					     REJECT;
					   }
					}
<CliPropertyType>{ID}			{
  					  addType();
  					  g_current->name = yytext;
  					}
<CliPropertyType>"["		        { // C++/CLI indexed property
  					  g_current->args = "[";
					  BEGIN( CliPropertyIndex );
  					}
<CliPropertyType>"{"			{
					  g_curlyCount=0;
					  //printf("event: '%s' '%s'\n",g_current->type.data(),g_current->name.data());
  					  BEGIN( CSAccessorDecl );
  					}
<CliPropertyType>";"			{
  					  unput(*yytext);
					  BEGIN( FindMembers );
  					}
<CliPropertyType>\n			{
                                          lineCount();
  					}
<CliPropertyType>{B}*			{
  					}
<CliPropertyType>.			{
  					  addType();
  					  g_current->type += yytext;
  					}
<CliPropertyIndex>"]"			{
                                          BEGIN( CliPropertyType );
  					  g_current->args+=yytext;
  					}
<CliPropertyIndex>.			{
  					  g_current->args+=yytext;
  					}
  /*
<FindMembers>{B}*"property"{BN}+        { 
                                          if (!g_current->type.isEmpty())
					  {
					    REJECT;
                                          }
					  else
					  { 
                                            g_current->mtype = g_mtype = Property;
                                            lineCount();
					  }
                                        }
  */
<FindMembers>{B}*"@private"{BN}+	{
  					  g_current->protection = g_protection = Private ;
					  g_current->mtype = g_mtype = Method;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
  					}
<FindMembers>{B}*"@protected"{BN}+	{
  					  g_current->protection = g_protection = Protected ;
					  g_current->mtype = g_mtype = Method;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
  					}
<FindMembers>{B}*"@public"{BN}+	{
  					  g_current->protection = g_protection = Public ;
					  g_current->mtype = g_mtype = Method;
					  g_current->type.resize(0); 
					  g_current->name.resize(0); 
					  g_current->args.resize(0);
					  g_current->argList.clear();
					  lineCount() ;
  					}
<FindMembers>[\-+]{BN}*			{
  					  if (!g_insideObjC) 
					  {
					    REJECT;
					  }
					  else
					  {
  					    lineCount();
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->bodyLine  = g_yyLineNr;
					    g_current->section = Entry::FUNCTION_SEC;
					    g_current->protection = g_protection = Public ;
					    g_language = g_current->lang = SrcLangExt_ObjC;
					    g_insideObjC = TRUE;
					    g_current->virt = Virtual;
					    g_current->stat=yytext[0]=='+';
					    g_current->mtype = g_mtype = Method;
					    g_current->type.resize(0); 
					    g_current->name.resize(0); 
					    g_current->args.resize(0);
					    g_current->argList.clear();
					    BEGIN( ObjCMethod );
					  }
  					}
<ObjCMethod>"("				{ // start of method's return type
  					  BEGIN( ObjCReturnType );
  					}
<ObjCMethod>{ID}			{ // found method name
  					  if (g_current->type.isEmpty())
					  {
					    g_current->type = "id";
					  }
					  g_current->name = yytext;
                                          if (g_insideCpp || g_insideObjC) 
                                          {
                                            g_current->id = ClangParser::instance()->lookup(g_yyLineNr,yytext);
                                          }
  					}
<ObjCMethod>":"{B}*			{ // start of parameter list
  					  g_current->name += ':';
  					  Argument a;
					  g_current->argList.push_back(a);
  					  BEGIN( ObjCParams );
  					}
<ObjCReturnType>[^)]*			{ // TODO: check if nested braches are possible.
  					  g_current->type = yytext;
  					}
<ObjCReturnType>")"			{
  					  BEGIN( ObjCMethod );
  					}
<ObjCParams>({ID})?{BN}*":"		{ // Keyword of parameter
					  QCString keyw = yytext;
					  keyw=keyw.left(keyw.length()-1).stripWhiteSpace(); // strip :
					  if (keyw.isEmpty())
					  {
					    g_current->name += " :";
					  }
					  else
					  {
					    g_current->name += keyw+":";
					  }
					  if (g_current->argList.back().type.isEmpty())
					  {
					    g_current->argList.back().type="id";
					  }
  					  Argument a;
  					  a.attrib=(QCString)"["+keyw+"]";
					  g_current->argList.push_back(a);
  					}
<ObjCParams>{ID}{BN}*			{ // name of parameter
  					  lineCount();
  					  g_current->argList.back().name=QCString(yytext).stripWhiteSpace();
  					}
<ObjCParams>","{BN}*"..."		{ // name of parameter
  					  lineCount();
					  // do we want the comma as part of the name?
  					  //g_current->name += ",";
  					  Argument a;
  					  a.attrib="[,]";
  					  a.type="...";
					  g_current->argList.push_back(a);
  					}
   /*
<ObjCParams>":"				{ 
					  g_current->name += ':';
					}
   */
<ObjCParams>"("				{
				 	  g_roundCount=0;
					  g_current->argList.back().type.resize(0);
					  BEGIN( ObjCParamType );
					}
<ObjCParamType>"("                      {
                                          g_roundCount++;
  					  g_current->argList.back().type+=yytext;
                                        }
<ObjCParamType>")"/{B}*			{
                                          if (g_roundCount<=0)
                                          {
                                            BEGIN( ObjCParams );
                                          }
                                          else
                                          {
                                            g_current->argList.back().type+=yytext;
                                            g_roundCount--;
                                          }
					}
<ObjCParamType>[^()]*			{
        				  g_current->argList.back().type+=QCString(yytext).stripWhiteSpace();
  					}
<ObjCMethod,ObjCParams>";"		{ // end of method declaration
					  if (!g_current->argList.empty() && g_current->argList.back().type.isEmpty())
					  {
					    g_current->argList.back().type="id";
					  }
                                          if (g_current->argList.empty()) // method without parameters
                                          {
                                            g_current->argList.noParameters = TRUE;
                                          }
  					  g_current->args = argListToString(g_current->argList);
					  //printf("argList=%s\n",g_current->args.data());
  					  unput(';');
  					  BEGIN( Function );
                                        }
<ObjCMethod,ObjCParams>(";"{BN}+)?"{"	{ // start of a method body
  					  lineCount();
                                          //printf("Type=%s Name=%s args=%s\n",
					  //    g_current->type.data(),g_current->name.data(),argListToString(g_current->argList).data()
					  //    );
					  if (!g_current->argList.empty() && g_current->argList.back().type.isEmpty())
					  {
					    g_current->argList.back().type="id";
					  }
                                          if (g_current->argList.empty()) // method without parameters
                                          {
                                            g_current->argList.noParameters = TRUE;
                                          }
  					  g_current->args = argListToString(g_current->argList);
                                          unput('{');
  					  BEGIN( Function );
  					}
<FindMembers>{B}*"sequence"{BN}*"<"{BN}* {
					  if (g_insideSlice)
					  {
                                            lineCount();
					    g_current->bodyLine = g_yyLineNr;
                                            g_current->fileName = g_yyFileName ;
					    g_current->startLine = g_yyLineNr ;
					    g_current->startColumn = g_yyColNr;
                                            g_current->args.resize(0);
					    g_current->section = Entry::TYPEDEF_SEC ;
                                            g_isTypedef = TRUE;
					    BEGIN( SliceSequence );
					  }
					  else
					    REJECT;
					}
<FindMembers>{B}*"dictionary"{BN}*"<"{BN}* {
					  if (g_insideSlice)
					  {
                                            lineCount();
					    g_current->bodyLine = g_yyLineNr;
                                            g_current->fileName = g_yyFileName ;
					    g_current->startLine = g_yyLineNr ;
					    g_current->startColumn = g_yyColNr;
                                            g_current->args.resize(0);
					    g_current->section = Entry::TYPEDEF_SEC ;
                                            g_isTypedef = TRUE;
					    BEGIN( SliceDictionary );
					  }
					  else
					    REJECT;
					}
<FindMembers>{BN}{1,80}		        {
  					  lineCount();
  					}
<FindMembers>"@"({ID}".")*{ID}{BN}*"("	{
  					  if (g_insideJava) // Java annotation
					  {
  					    lineCount();
					    g_lastSkipRoundContext = YY_START;
					    g_roundCount=0;
					    BEGIN( SkipRound );
					  }
					  else if (qstrncmp(yytext,"@property",9)==0) // ObjC 2.0 property
					  {
  					    g_current->mtype = g_mtype = Property;
					    g_current->spec|=Entry::Readable | Entry::Writable | Entry::Assign;
					    g_current->protection = Public ;
					    unput('(');
					    BEGIN( ObjCPropAttr );
					  }
					  else
					  {
					    REJECT;
					  }
                                        }
<ObjCPropAttr>"getter="{ID}		{ 
                                          g_current->read = yytext+7;
					}
<ObjCPropAttr>"setter="{ID}		{ 
                                          g_current->write = yytext+7;
					}
<ObjCPropAttr>"readonly"		{
					  g_current->spec&=~Entry::Writable;
  					}
<ObjCPropAttr>"readwrite"		{ // default
  					}
<ObjCPropAttr>"assign"			{ // default
  					}
<ObjCPropAttr>"unsafe_unretained"       {
					  g_current->spec&=~Entry::Assign;
					  g_current->spec|=Entry::Unretained;
                                        }
<ObjCPropAttr>"retain"			{
					  g_current->spec&=~Entry::Assign;
					  g_current->spec|=Entry::Retain;
  					}
<ObjCPropAttr>"copy"			{
					  g_current->spec&=~Entry::Assign;
					  g_current->spec|=Entry::Copy;
  					}
<ObjCPropAttr>"weak"                    {
					  g_current->spec&=~Entry::Assign;
					  g_current->spec|=Entry::Weak;
                                        }
<ObjCPropAttr>"strong"                  {
					  g_current->spec&=~Entry::Assign;
					  g_current->spec|=Entry::Strong;
                                        }
<ObjCPropAttr>"nonatomic"		{
					  g_current->spec|=Entry::NonAtomic;
  					}
<ObjCPropAttr>")"			{
  					  BEGIN(FindMembers);
  					}
<FindMembers>"@"{ID}			{
  					  if (g_insideJava) // Java annotation
					  {
					    // skip annotation
					  }
					  else if (qstrcmp(yytext,"@property")==0) // ObjC 2.0 property
					  {
  					    g_current->mtype = g_mtype = Property;
					    g_current->spec|=Entry::Writable | Entry::Readable;
					    g_current->protection = Public ;
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
<ObjCSkipStatement>";"			{
  					  BEGIN(FindMembers);
  					}
<PackageName>{ID}(("."|"\\"){ID})*	{
  					  g_isTypedef=FALSE;
					  //printf("Found namespace %s lang=%d\n",yytext,g_current->lang);
					  g_current->name = yytext;
					  g_current->name = substitute(g_current->name,".","::");
					  g_current->name = substitute(g_current->name,"\\","::");
					  g_current->section = Entry::NAMESPACE_SEC;
					  g_current->type = "namespace" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount();
  					}
<PackageName>";"			{
                                          Entry *tmp = g_current.get();
					  g_current_root->moveToSubEntryAndRefresh(g_current);
                                          g_current_root = tmp;
					  initEntry();
  					  BEGIN(FindMembers);
  					}
<PackageName>"{"			{
					  g_curlyCount=0;
					  BEGIN( ReadNSBody );
  					}
<FindMembers>{B}*"initonly"{BN}+	{
  					  g_current->type += " initonly ";
					  if (g_insideCli) g_current->spec |= Entry::Initonly;
					  lineCount();
  					}
<FindMembers>{B}*"static"{BN}+     	{ g_current->type += " static ";
  					  g_current->stat = TRUE;
					  lineCount();
					}
<FindMembers>{B}*"extern"{BN}+		{
  					  g_current->stat = FALSE;
					  g_current->explicitExternal = TRUE;
					  lineCount();
  					}
<FindMembers>{B}*"const"{BN}+    	{ g_current->type += " const ";
					  if (g_insideCS) g_current->stat = TRUE;
					  lineCount();
					}
<FindMembers>{B}*"virtual"{BN}+    	{ g_current->type += " virtual ";
					  g_current->virt = Virtual;
					  lineCount();
					}
<FindMembers>{B}*"constexpr"{BN}+    	{ 
                                          if (g_insideCpp)
                                          {
                                            g_current->type += " constexpr ";
                                            g_current->spec |= Entry::ConstExpr;
                                            lineCount();
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
					}
<FindMembers>{B}*"published"{BN}+	{ // UNO IDL published keyword
					  if (g_insideIDL)
					  {
					    lineCount();
					    g_current->spec |= Entry::Published;
					  }
					  else
					  {
					    REJECT;
					  }
					}
<FindMembers>{B}*"abstract"{BN}+    	{ 
                                          if (!g_insidePHP) 
					  {
					    g_current->type += " abstract ";
					    if (!g_insideJava)
					    {
                                              g_current->virt = Pure;
					    }
					    else
					    {
                                              g_current->spec|=Entry::Abstract;
					    }
					  }
					  else
					  {
					    g_current->spec|=Entry::Abstract;
					  }
					  lineCount();
					}
<FindMembers>{B}*"inline"{BN}+		{ g_current->spec|=Entry::Inline;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"mutable"{BN}+		{ g_current->spec|=Entry::Mutable;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"explicit"{BN}+	{ g_current->spec|=Entry::Explicit;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"local"{BN}+		{ g_current->spec|=Entry::Local;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"@required"{BN}+	{ // Objective C 2.0 protocol required section
                                          g_current->spec=(g_current->spec & ~Entry::Optional) | Entry::Required;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"@optional"{BN}+	{  // Objective C 2.0 protocol optional section
                                          g_current->spec=(g_current->spec & ~Entry::Required) | Entry::Optional;
                                          lineCount(); 
                                        }
  /*
<FindMembers>{B}*"import"{BN}+		{ // IDL import keyword
  					  BEGIN( NextSemi );
  					}
  */
<FindMembers>{B}*"typename"{BN}+	{ lineCount(); }
<FindMembers>{B}*"namespace"{BN}*/[^a-z_A-Z0-9]	{ 
  					  g_isTypedef=FALSE;
					  g_current->section = Entry::NAMESPACE_SEC;
					  g_current->type = "namespace" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount();
					  if (g_insidePHP)
					  {
					    BEGIN( PackageName );
					  }
					  else
					  {
  					    BEGIN( CompoundName ); 
					  }
					}
<FindMembers>{B}*"module"{BN}+		{ 
					  lineCount();
                                          if (g_insideIDL || g_insideSlice)
					  {
  					    g_isTypedef=FALSE;
					    g_current->section = Entry::NAMESPACE_SEC;
					    g_current->type = "module" ;
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->bodyLine  = g_yyLineNr;
  					    BEGIN( CompoundName ); 
					  }
					  else if (g_insideD)
					  {
					    lineCount();
					    BEGIN(PackageName);
					  }
					  else
					  {
					    addType();
					    g_current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{B}*"library"{BN}+		{ 
					  lineCount();
                                          if (g_insideIDL)
					  {
  					    g_isTypedef=FALSE;
					    g_current->section = Entry::NAMESPACE_SEC;
					    g_current->type = "library" ;
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->bodyLine  = g_yyLineNr;
  					    BEGIN( CompoundName ); 
					  }
					  else
					  {
					    addType();
					    g_current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{B}*"constants"{BN}+	{  // UNO IDL constant group
					  lineCount();
					  if (g_insideIDL)
					  {
					    g_isTypedef=FALSE;
					    g_current->section = Entry::NAMESPACE_SEC;
					    g_current->type = "constants";
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->bodyLine  = g_yyLineNr;
					    BEGIN( CompoundName );
					  }
					  else
					  {
					    addType();
					    g_current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{BN}*("service"){BN}+ 	{ // UNO IDL service
					  lineCount();
					  if (g_insideIDL)
					  {
					    g_isTypedef=FALSE;
					    g_current->section = Entry::CLASS_SEC;
					    g_current->spec = Entry::Service |
					      // preserve UNO IDL [optional] or published
					      (g_current->spec & (Entry::Optional|Entry::Published));
					    addType();
					    g_current->type += " service " ;
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->bodyLine  = g_yyLineNr;
					    BEGIN( CompoundName );
					  }
					  else // TODO is addType right? just copy/pasted
					  {
					    addType();
					    g_current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{BN}*("singleton"){BN}+ 	{ // UNO IDL singleton
					  lineCount();
					  if (g_insideIDL)
					  {
					    g_isTypedef=FALSE;
					    g_current->section = Entry::CLASS_SEC;
					    g_current->spec = Entry::Singleton |
					      (g_current->spec & Entry::Published); // preserve
					    addType();
					    g_current->type += " singleton " ;
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->bodyLine  = g_yyLineNr;
					    BEGIN( CompoundName );
					  }
					  else // TODO is addType right? just copy/pasted
					  {
					    addType();
					    g_current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{BN}*((("disp")?"interface")|"valuetype"){BN}+ 	{ // M$/Corba/UNO IDL/Java/Slice interface
					  lineCount();
                                          if (g_insideIDL || g_insideJava || g_insideCS || g_insideD || g_insidePHP || g_insideSlice)
					  {
  					    g_isTypedef=FALSE;
					    g_current->section = Entry::CLASS_SEC;
                        		    g_current->spec = Entry::Interface |
                                                // preserve UNO IDL [optional], published, Slice local
                                                (g_current->spec & (Entry::Optional|Entry::Published|Entry::Local));
					    addType();
					    g_current->type += " interface" ;
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->bodyLine  = g_yyLineNr;
					    BEGIN( CompoundName );
					  }
					  else
					  {
					    addType();
					    g_current->name = QCString(yytext).stripWhiteSpace();
					  }
  					}
<FindMembers>{B}*"@implementation"{BN}+	{ // Objective-C class implementation
					  lineCount();
  					  g_isTypedef=FALSE;
					  g_current->section = Entry::OBJCIMPL_SEC;
					  g_language = g_current->lang = SrcLangExt_ObjC;
					  g_insideObjC = TRUE;
					  g_current->protection = g_protection = Public ;
					  addType();
					  g_current->type += " implementation" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->bodyLine  = g_yyLineNr;
					  BEGIN( CompoundName );
  					}
<FindMembers>{B}*"@interface"{BN}+	{ // Objective-C class interface, or Java attribute
					  lineCount();
  					  g_isTypedef=FALSE;
					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec = Entry::Interface;
					  if (!g_insideJava)
					  {
					    g_language = g_current->lang = SrcLangExt_ObjC;
					    g_insideObjC = TRUE;
					  }
					  g_current->protection = g_protection = Public ;
					  addType();
					  g_current->type += " interface" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  BEGIN( CompoundName );
  					}
<FindMembers>{B}*"@protocol"{BN}+	{ // Objective-C protocol definition
					  lineCount();
  					  g_isTypedef=FALSE;
					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec = Entry::Protocol;
					  g_language = g_current->lang = SrcLangExt_ObjC;
					  g_insideObjC = TRUE;
					  g_current->protection = g_protection = Public ;
					  addType();
					  g_current->type += " protocol" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  BEGIN( CompoundName );
  					}
<FindMembers>{B}*"exception"{BN}+ 	{ // Corba IDL/Slice exception
  					  g_isTypedef=FALSE;
					  g_current->section = Entry::CLASS_SEC;
                                          // preserve UNO IDL, Slice local
					  g_current->spec    = Entry::Exception |
					    (g_current->spec & Entry::Published) |
					    (g_current->spec & Entry::Local);
					  addType();
					  g_current->type += " exception" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount();
					  BEGIN( CompoundName );
  					}
<FindMembers>"@class" | // for Objective C class declarations
<FindMembers>{B}*{TYPEDEFPREFIX}"class{" |
<FindMembers>{B}*{TYPEDEFPREFIX}"class"{BN}+ { 
                                          QCString decl = yytext;
					  g_isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
  					  g_current->section = Entry::CLASS_SEC;
					  addType();
                                          uint64 spec = g_current->spec;
					  if (g_insidePHP && g_current->spec&Entry::Abstract)
					  {
					    // convert Abstract to AbstractClass
					    g_current->spec=(g_current->spec&~Entry::Abstract)|Entry::AbstractClass;
					  }
                                          if (g_insideSlice && spec&Entry::Local)
                                          {
                                            g_current->spec|=Entry::Local;
                                          }
                                          if (isConst)
                                          {
                                            g_current->type += " const";
                                          }
                                          else if (isVolatile)
                                          {
                                            g_current->type += " volatile";
                                          }
					  g_current->type += " class" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  if (yytext[0]=='@')
					  {
					    g_language = g_current->lang = SrcLangExt_ObjC;
					    g_insideObjC = TRUE;
					  }
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<FindMembers>{B}*"value class{" |         // C++/CLI extension
<FindMembers>{B}*"value class"{BN}+     {
  					  g_isTypedef=FALSE;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec = Entry::Value;
					  addType();
					  g_current->type += " value class" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
  					}
<FindMembers>{B}*"ref class{" |          // C++/CLI extension
<FindMembers>{B}*"ref class"{BN}+       {
  					  g_isTypedef=FALSE;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec = Entry::Ref;
					  addType();
					  g_current->type += " ref class" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
  					}
<FindMembers>{B}*"interface class{" |     // C++/CLI extension
<FindMembers>{B}*"interface class"{BN}+ {
  					  g_isTypedef=FALSE;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec = Entry::Interface;
					  addType();
					  g_current->type += " interface class" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
  					}
<FindMembers>{B}*"coclass"{BN}+		{
  					  if (g_insideIDL)
					  {
					    g_isTypedef=FALSE;
					    g_current->section = Entry::CLASS_SEC;
					    addType();
					    g_current->type += " coclass" ;
					    g_current->fileName  = g_yyFileName;
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->bodyLine  = g_yyLineNr;
					    lineCount() ;
					    BEGIN( CompoundName ) ;
					  }
					  else
					  {
  					    addType();
					    g_current->name = yytext;
					    g_current->name = g_current->name.stripWhiteSpace();
					    lineCount();
					  }
  					}
<FindMembers>{B}*{TYPEDEFPREFIX}"struct{" | 
<FindMembers>{B}*{TYPEDEFPREFIX}"struct"/{BN}+ { 
                                          QCString decl = yytext;
					  g_isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
                                          uint64 spec = g_current->spec;
  					  g_current->section = Entry::CLASS_SEC ;
                                          // preserve UNO IDL & Inline attributes, Slice local
					  g_current->spec    = Entry::Struct |
					    (g_current->spec & Entry::Published) |
                                            (g_current->spec & Entry::Inline) |
                                            (g_current->spec & Entry::Local); 
					  // bug 582676: can be a struct nested in an interface so keep g_insideObjC state
					  //g_current->objc    = g_insideObjC = FALSE;
					  addType();
                                          if (isConst)
                                          {
                                            g_current->type += " const";
                                          }
                                          else if (isVolatile)
                                          {
                                            g_current->type += " volatile";
                                          }
					  g_current->type += " struct" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<FindMembers>{B}*"value struct{" |      // C++/CLI extension
<FindMembers>{B}*"value struct"{BN}+     {
  					  g_isTypedef=FALSE;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec    = Entry::Struct | Entry::Value;
					  addType();
					  g_current->type += " value struct" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
  					}
<FindMembers>{B}*"ref struct{" |       // C++/CLI extension
<FindMembers>{B}*"ref struct"{BN}+     {
  					  g_isTypedef=FALSE;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec    = Entry::Struct | Entry::Ref;
					  addType();
					  g_current->type += " ref struct" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
  					}
<FindMembers>{B}*"interface struct{" |     // C++/CLI extension
<FindMembers>{B}*"interface struct"{BN}+ {
  					  g_isTypedef=FALSE;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec    = Entry::Struct | Entry::Interface;
					  addType();
					  g_current->type += " interface struct";
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
  					}
<FindMembers>{B}*{TYPEDEFPREFIX}"union{" |
<FindMembers>{B}*{TYPEDEFPREFIX}"union"{BN}+ { 
                                          QCString decl=yytext;
					  g_isTypedef=decl.find("typedef")!=-1;
                                          bool isConst=decl.find("const")!=-1;
                                          bool isVolatile=decl.find("volatile")!=-1;
  					  g_current->section = Entry::CLASS_SEC;
                                          g_current->spec    = Entry::Union;
					  // bug 582676: can be a struct nested in an interface so keep g_insideObjC state
					  //g_current->objc    = g_insideObjC = FALSE;
					  addType();
                                          if (isConst)
                                          {
                                            g_current->type += " const";
                                          }
                                          else if (isVolatile)
                                          {
                                            g_current->type += " volatile";
                                          }
					  g_current->type += " union" ;
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<FindMembers>{B}*{TYPEDEFPREFIX}{IDLATTR}?"enum"({BN}+("class"|"struct"))?"{" |
<FindMembers>{B}*{TYPEDEFPREFIX}{IDLATTR}?"enum"({BN}+("class"|"struct"))?{BN}+ { // for IDL: typedef [something] enum
                                          QCString text=yytext;
					  g_isTypedef    = text.find("typedef")!=-1;
                                          bool isStrongEnum = text.find("struct")!=-1 || text.find("class")!=-1 || g_insideCS;
					  if (g_insideJava)
					  {
					    g_current->section = Entry::CLASS_SEC;
					    g_current->spec    = Entry::Enum;
					  }
					  else
					  {
  					    g_current->section = Entry::ENUM_SEC ;
					  }
					  addType();
					  g_current->type += " enum";
                                          if (isStrongEnum)
                                          {
                                            g_current->spec |= Entry::Strong;
                                          }
					  g_current->fileName  = g_yyFileName;
					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->bodyLine  = g_yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<Operator>"("{BN}*")"({BN}*"<"[^>]*">"){BN}*/"("  {  // A::operator()<int>(int arg)
  					  lineCount();
					  g_current->name += "()";
					  BEGIN( FindMembers );
					}
<Operator>"("{BN}*")"{BN}*/"("		{
  					  lineCount();
    					  g_current->name += yytext ;
					  g_current->name = g_current->name.simplifyWhiteSpace();
					  BEGIN( FindMembers ) ;
  					}
<Operator>";"				{ // can occur when importing members
  					  unput(';');
					  BEGIN( FindMembers ) ;
  					}
<Operator>[^(]				{ 
    					  lineCount();
    					  g_current->name += *yytext ;
					}
<Operator>"<>"				{ /* skip guided templ specifiers */ }
<Operator>"("				{
					  g_current->name = g_current->name.simplifyWhiteSpace();
					  unput(*yytext);
					  BEGIN( FindMembers ) ;
					}
<FindMembers>("template"|"generic")({BN}*)"<"/[>]?	{  // generic is a C++/CLI extension
  					  lineCount();
					  ArgumentList al;
					  //g_current->spec |= (yytext[0]=='g') ? Entry::Generic : Entry::Template;
					  g_current->tArgLists.push_back(al);
					  g_currentArgumentList = &g_current->tArgLists.back();
					  g_templateStr="<";
					  g_fullArgString = g_templateStr;
					  g_copyArgString = &g_templateStr;
					  g_currentArgumentContext = FindMembers;
					  BEGIN( ReadTempArgs );
  					}
<FindMembers>"namespace"{BN}+/{ID}{BN}*"=" { // namespace alias
                                          lineCount(); 
  					  BEGIN( NSAliasName );
					}
<NSAliasName>{ID}			{
  					  g_aliasName = yytext;
					  BEGIN( NSAliasArg );
  					}
<NSAliasArg>({ID}"::")*{ID}		{
  					  //printf("Inserting namespace alias %s::%s->%s\n",g_current_root->name.data(),g_aliasName.data(),yytext);
					  //if (g_current_root->name.isEmpty())
					  //{
                                          // TODO: namespace aliases are now treated as global entities
                                          // while they should be aware of the scope they are in
                                            Doxygen::namespaceAliasDict.insert(g_aliasName,new QCString(yytext));
					  //}
					  //else
					  //{
                                          //  Doxygen::namespaceAliasDict.insert(g_current_root->name+"::"+g_aliasName,
					  //	new QCString(g_current_root->name+"::"+yytext));
					  //}
  					}
<NSAliasArg>";"				{
  					  BEGIN( FindMembers );
  					}
<PHPUse>({ID}{BN}*"\\"{BN}*)*{ID}/{BN}+"as"  {
  					  lineCount();
					  g_aliasName=yytext;
					  BEGIN(PHPUseAs);
					}
<PHPUse>({ID}{BN}*"\\"{BN}*)*{ID}	{
  					  lineCount();
					  g_current->name=removeRedundantWhiteSpace(substitute(yytext,"\\","::"));
  					  //printf("PHP: adding use relation: %s\n",g_current->name.data());
                                          g_current->fileName = g_yyFileName; 
                                          // add a using declaration
  					  g_current->section=Entry::USINGDECL_SEC;
					  g_current_root->copyToSubEntry(g_current);
                                          // also add it as a using directive
					  g_current->section=Entry::USINGDIR_SEC;
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  g_aliasName.resize(0);
					}
<PHPUseAs>{BN}+"as"{BN}+		{
  					  lineCount();
  					}
<PHPUseAs>{PHPUSEKW}                    {
                                        }
<PHPUseAs>{ID}				{
  					  //printf("PHP: adding use as relation: %s->%s\n",yytext,g_aliasName.data());
                                          if (!g_aliasName.isEmpty())
                                          {
                                            Doxygen::namespaceAliasDict.insert(yytext,
					         new QCString(removeRedundantWhiteSpace(
					  	   substitute(g_aliasName,"\\","::"))));
                                          }
					  g_aliasName.resize(0);
  					}
<PHPUse,PHPUseAs>[,;]			{
					  if (*yytext==',')
					  {
					    BEGIN(PHPUse);
					  }
					  else
					  {
  					    BEGIN(FindMembers);
					  }
  					}
<JavaImport>({ID}{BN}*"."{BN}*)+"*"	{ // package import => add as a using directive
  					  lineCount();
  					  QCString scope=yytext;
					  g_current->name=removeRedundantWhiteSpace(substitute(scope.left(scope.length()-1),".","::"));
                                          g_current->fileName = g_yyFileName; 
  					  g_current->section=Entry::USINGDIR_SEC;
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  BEGIN(Using);
  					}
<JavaImport>({ID}{BN}*"."{BN}*)+{ID}	{ // class import => add as a using declaration
                                          lineCount();
  					  QCString scope=yytext;
					  g_current->name=removeRedundantWhiteSpace(substitute(scope,".","::"));
					  g_current->fileName = g_yyFileName; 
					  if (g_insideD) 
					  {
					    g_current->section=Entry::USINGDIR_SEC;
					  }
					  else
					  {
					    //printf("import name = %s -> %s\n",yytext,g_current->name.data());
					    g_current->section=Entry::USINGDECL_SEC;
					  }
                                          g_previous = g_current.get();
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  BEGIN(Using);
  					}
<FindMembers>"using"{BN}+		{ 
					  g_current->startLine=g_yyLineNr; 
					  g_current->startColumn = g_yyColNr;
                                          lineCount(); 
                                          BEGIN(Using); 
                                        }
<Using>"namespace"{BN}+			{ lineCount(); BEGIN(UsingDirective); }
<Using>({ID}{BN}*("::"|"."){BN}*)*({ID}|{OPERATOR}) {
                                          lineCount();
  					  g_current->name=yytext;
                                          g_current->fileName = g_yyFileName; 
  					  g_current->section=Entry::USINGDECL_SEC;
                                          g_current->startLine = g_yyLineNr;
                                          g_previous = g_current.get();
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  if (g_insideCS) /* Hack: in C# a using declaration and 
							   directive have the same syntax, so we
							   also add it as a using directive here
							 */
					  {
					    g_current->name=yytext;
					    g_current->fileName = g_yyFileName; 
					    g_current->startLine = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    g_current->section=Entry::USINGDIR_SEC;
					    g_current_root->moveToSubEntryAndRefresh(g_current);
					    initEntry();
					  }
					  BEGIN(Using);
  					}
<Using>"="                              { // C++11 style template alias?
                                          BEGIN(UsingAlias);
                                        }
<UsingAlias>";"                         { 
                                          g_previous->section=Entry::VARIABLE_SEC;
                                          g_previous->type = "typedef "+g_previous->args;
					  g_previous->type=g_previous->type.simplifyWhiteSpace();
                                          g_previous->args.resize(0);
					  g_previous->name=g_previous->name.stripWhiteSpace();
					  g_previous->bodyLine = g_yyLineNr;
                                          g_previous->spec |= Entry::Alias;
                                          BEGIN(FindMembers); 
                                        }
<UsingAlias>";"{BN}*("/**"|"//!"|"/*!"|"///")"<" {
					  g_docBlockContext   = UsingAliasEnd;
					  g_docBlockInBody    = FALSE;
					  g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                      ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;
  					  lineCount();

					  g_docBlockTerm = ';';
					  if (yytext[yyleng-3]=='/')
					  {
					    startCommentBlock(TRUE);
					    BEGIN( DocLine );
					  }
					  else
					  {
					    startCommentBlock(FALSE);
					    BEGIN( DocBlock );
					  }
  					}
<UsingAlias>">>"                        {
                                          g_previous->args+="> >"; // see bug769552
                                        }
<UsingAlias>.                           {
                                          g_previous->args+=yytext;
                                        }
<UsingAlias>\n                          {
                                          g_previous->args+=yytext;
                                          lineCount();
                                        }
<UsingAliasEnd>";"                      {
                                          g_previous->doc   = g_current->doc;
                                          g_previous->brief = g_current->brief;
                                          g_current->doc.resize(0);
                                          g_current->brief.resize(0);
                                          unput(';');
                                          BEGIN(UsingAlias);
                                        }
<UsingDirective>{SCOPENAME}		{ g_current->name=removeRedundantWhiteSpace(yytext);
                                          g_current->fileName = g_yyFileName; 
  					  g_current->section=Entry::USINGDIR_SEC;
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  BEGIN(Using);
  					}
<Using>";"				{ BEGIN(FindMembers); }
<FindMembers>{SCOPENAME}{BN}*"<>"	{ // guided template decl
					  QCString n=yytext;
					  addType();
					  g_current->name=n.left(n.length()-2);
					}
<FindMembers>{SCOPENAME}{BN}*/"<"	{ // Note: this could be a return type!
                                          g_roundCount=0;
  					  g_sharpCount=0;
					  lineCount();
					  addType();
  					  g_current->name=yytext;
					  g_current->name=g_current->name.stripWhiteSpace();
					  //g_current->scopeSpec.resize(0);
					  // g_currentTemplateSpec = &g_current->scopeSpec;
					  if (nameIsOperator(g_current->name))
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
					}
<FindMemberName>{SCOPENAME}{BN}*/"<"	{
  					  g_sharpCount=0;
					  g_roundCount=0;
					  lineCount();
  					  g_current->name+=((QCString)yytext).stripWhiteSpace();
					  //g_current->memberSpec.resize(0);
					  // g_currentTemplateSpec = &g_current->memberSpec;
					  if (nameIsOperator(g_current->name))
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
  					}
<EndTemplate>"<<<"	                {
  					  if (!g_insidePHP) 
					  {
					    REJECT;
					  }
					  else
					  {
                                            g_lastHereDocContext = YY_START;
					    BEGIN(HereDoc);
					  }
  					}
<ClassTemplSpec,EndTemplate>"<<"	{
  					  g_current->name+=yytext;
  					  // *g_currentTemplateSpec+=yytext; 
  					}
<EndTemplate>"<"			{ 
                                          if (g_roundCount==0)
                                          {
  					    // *g_currentTemplateSpec+='<'; 
					    g_sharpCount++; 
                                          }
  					  g_current->name+=yytext;
					}
<ClassTemplSpec,EndTemplate>">>"	{
  					  if (g_insideJava || g_insideCS || g_insideCli || g_roundCount==0)
					  {
					    unput('>');
					    unput(' ');
					    unput('>');
					  }
					  else
					  {
  					    g_current->name+=yytext;
					  }
  					  // *g_currentTemplateSpec+=yytext; 
  					}
<EndTemplate>">"			{
  					  g_current->name+='>';
					  // *g_currentTemplateSpec+='>';
					  if (g_roundCount==0 && --g_sharpCount<=0)
					  {  
					    //printf("Found %s\n",g_current->name.data());
					    BEGIN(FindMembers);
					  }
					}
<EndTemplate>">"{BN}*"("		{ 
  					  lineCount();
  					  g_current->name+='>';
					  // *g_currentTemplateSpec+='>';
					  if (g_roundCount==0 && --g_sharpCount<=0)
					  {
					    g_current->bodyLine = g_yyLineNr;
					    g_current->args = "(";
					    g_currentArgumentContext = FuncQual;
					    g_fullArgString = g_current->args.copy();
					    g_copyArgString = &g_current->args;
					    //printf("Found %s\n",g_current->name.data());
					    BEGIN( ReadFuncArgType ) ;
					  }
					}
<EndTemplate>">"{BN}*/"("({BN}*{ID}{BN}*"::")*({BN}*"*"{BN}*)+ { // function pointer returning a template instance
  					  lineCount();
  					  g_current->name+='>';
                                          if (g_roundCount==0)
                                          {
					    BEGIN(FindMembers);
                                          }
  					}
<EndTemplate>">"{BN}*/"::"		{
  					  lineCount();
  					  g_current->name+='>';
  					  // *g_currentTemplateSpec+='>';
					  if (g_roundCount==0 && --g_sharpCount<=0)
					  {
					    BEGIN(FindMemberName);
					  }
  					}
<ClassTemplSpec,EndTemplate>"("         { g_current->name+=*yytext;
                                          g_roundCount++; 
                                        }
<ClassTemplSpec,EndTemplate>")"         { g_current->name+=*yytext;
                                          if (g_roundCount>0) g_roundCount--; 
                                        }
<EndTemplate>.				{ 
  					  g_current->name+=*yytext;
  					  // *g_currentTemplateSpec+=*yytext; 
					}
<FindMembers>"define"{BN}*"("{BN}*["']	{
					  if (g_insidePHP)
					  {
					    g_current->bodyLine = g_yyLineNr;
					    BEGIN( DefinePHP );
					  }
					  else
					    REJECT;
					}
<CopyHereDoc>{ID}                       { // PHP heredoc
  					  g_delimiter = yytext;
                                          *g_pCopyHereDocGString += yytext;
					  BEGIN(CopyHereDocEnd);
                                        }
<CopyHereDoc>"'"{ID}/"'"		{ // PHP nowdoc
  					  g_delimiter = &yytext[1];
                                          *g_pCopyHereDocGString += yytext;
					  BEGIN(CopyHereDocEnd);
                                        }
<HereDoc>{ID}				{ // PHP heredoc
  					  g_delimiter = yytext;
					  BEGIN(HereDocEnd);
  					}
<HereDoc>"'"{ID}/"'"			{ // PHP nowdoc
  					  g_delimiter = &yytext[1];
					  BEGIN(HereDocEnd);
  					}
<HereDocEnd>^{ID}			{ // id at start of the line could mark the end of the block
  					  if (g_delimiter==yytext) // it is the end marker
					  {
  					    BEGIN(g_lastHereDocContext);
					  }
  					}
<HereDocEnd>.				{ }
<CopyHereDocEnd>^{ID}			{ // id at start of the line could mark the end of the block
                                          *g_pCopyHereDocGString += yytext;
  					  if (g_delimiter==yytext) // it is the end marker
					  {
  					    BEGIN(g_lastHereDocContext);
					  }
  					}
<CopyHereDocEnd>\n			{ 
                                          *g_pCopyHereDocGString += yytext;
                                        }
<CopyHereDocEnd>{ID}			{ 
                                          *g_pCopyHereDocGString += yytext;
                                        }
<CopyHereDocEnd>.			{ 
                                          *g_pCopyHereDocGString += yytext;
                                        }
<FindMembers>"Q_OBJECT"			{ // Qt object macro
  					}
<FindMembers>"Q_PROPERTY"		{ // Qt property declaration
					  g_current->protection = Public ; // see bug734245 & bug735462
  					  g_current->mtype = g_mtype = Property;
					  g_current->type.resize(0);
  					  BEGIN(QtPropType);
  					}
<QtPropType>"("				{ // start of property arguments
  					}
<QtPropAttr>")"				{ // end of property arguments
  					  unput(';');
					  BEGIN(FindMembers);
  					}
<QtPropType>"const"|"volatile"|"unsigned"|"signed"|"long"|"short" {
  				          g_current->type+=yytext;
  					}
<QtPropType>{B}+			{
  				          g_current->type+=yytext;
  					}
<QtPropType>({TSCOPE}"::")*{TSCOPE}	{
  					  g_current->type+=yytext;
                                          BEGIN(QtPropName);
  					}
<QtPropName>{ID}			{
  					  g_current->name=yytext;
  					  BEGIN(QtPropAttr);
  					}
<QtPropAttr>"READ"			{
  					  g_current->spec |= Entry::Readable;
					  BEGIN(QtPropRead);
  					}
<QtPropAttr>"WRITE"			{
  					  g_current->spec |= Entry::Writable;
					  BEGIN(QtPropWrite);
  					}
<QtPropAttr>"RESET"{B}+{ID}		{ // reset method => not supported yet
  					}
<QtPropAttr>"SCRIPTABLE"{B}+{ID}	{ // scriptable property => not supported yet
  					}
<QtPropAttr>"DESIGNABLE"{B}+{ID}	{ // designable property => not supported yet
  					}
<QtPropRead>{ID}			{
  					  g_current->read = yytext;
  					  BEGIN(QtPropAttr);
  					}
<QtPropWrite>{ID}			{
  					  g_current->write = yytext;
					  BEGIN(QtPropAttr);
  					}
<FindMembers>"friend"{BN}+("class"|"union"|"struct"){BN}+ {
  					  g_current->name=yytext;
				          BEGIN(FindMembers);
			 	        }
<FindMembers,FindMemberName>{SCOPENAME}	{
                                          if (g_insideCpp || g_insideObjC) 
                                          {
                                            g_current->id = ClangParser::instance()->lookup(g_yyLineNr,yytext);
                                          }
					  g_yyBegColNr=g_yyColNr;
					  g_yyBegLineNr=g_yyLineNr;
					  lineCount();
                                          if (g_insideIDL && yyleng==9 && qstrcmp(yytext,"cpp_quote")==0)
					  {
					    BEGIN(CppQuote);
					  }
					  else if ((g_insideIDL || g_insideJava || g_insideD) && yyleng==6 && qstrcmp(yytext,"import")==0)
					  {
					    if (g_insideIDL)
					      BEGIN(NextSemi);
					    else // g_insideJava or g_insideD
					      BEGIN(JavaImport);
					  }
					  else if (g_insidePHP && qstrcmp(yytext,"use")==0)
					  {
					    BEGIN(PHPUse);
					  }
					  else if (g_insideJava && qstrcmp(yytext,"package")==0)
					  {
  					    lineCount();
					    BEGIN(PackageName);
					  }
					  else if (g_insideIDL && qstrcmp(yytext,"case")==0)
					  {
					    BEGIN(IDLUnionCase);
					  }
					  else if (g_insideTryBlock && qstrcmp(yytext,"catch")==0)
					  {
					    g_insideTryBlock=FALSE;
					    BEGIN(TryFunctionBlock);
					  }
                                          else if (g_insideCpp && qstrcmp(yytext,"alignas")==0)
                                          {
                                            g_lastAlignAsContext = YY_START;
                                            BEGIN(AlignAs);
                                          }
					  else if (g_insideJS && qstrcmp(yytext,"var")==0)
					  { // javascript variable
					    g_current->type="var";
					  }
					  else if (g_insideJS && qstrcmp(yytext,"function")==0)
					  { // javascript function
					    g_current->type="function";
					  }
					  else if (g_insideCS && qstrcmp(yytext,"this")==0)
					  {
					    // C# indexer
					    addType();
					    g_current->name="this";
					    BEGIN(CSIndexer);
					  }
                                          else if (g_insideCpp && qstrcmp(yytext,"static_assert")==0)
                                          {
                                            // C++11 static_assert
                                            BEGIN(StaticAssert);
                                          }
                                          else if (g_insideCpp && qstrcmp(yytext,"decltype")==0)
                                          {
                                            // C++11 decltype(x)
					    g_current->type+=yytext;
                                            BEGIN(DeclType);
                                          }
                                          else if (g_insideSlice && qstrcmp(yytext,"optional")==0)
                                          {
                                            if (g_current->type.isEmpty())
                                            {
                                              g_current->type = "optional";
                                            }
                                            else
                                            {
                                              g_current->type += " optional";
                                            }
                                            g_lastModifierContext = YY_START;
                                            BEGIN(SliceOptional);
                                          }
					  else
					  {
					    if (YY_START==FindMembers)
					    {
					      addType();
					    }
					    bool javaLike = g_insideJava || g_insideCS || g_insideD || g_insidePHP || g_insideJS;
					    if (javaLike && qstrcmp(yytext,"public")==0)
					    {
					      g_current->protection = Public;
					    }
					    else if (javaLike && qstrcmp(yytext,"protected")==0)
					    {
					      g_current->protection = Protected;
					    }
					    else if ((g_insideCS || g_insideD || g_insidePHP || g_insideJS) && qstrcmp(yytext,"internal")==0)
					    {
					      g_current->protection = Package;
					    }
					    else if (javaLike && qstrcmp(yytext,"private")==0)
					    {
					      g_current->protection = Private;
					    }
					    else if (javaLike && qstrcmp(yytext,"static")==0)
					    {
					      if (YY_START==FindMembers)
					        g_current->name  = yytext;
					      else
						g_current->name += yytext;
			 		      g_current->stat = TRUE;
					    }
					    else
					    {
					      if (YY_START==FindMembers)
					        g_current->name  = yytext;
					      else
						g_current->name += yytext;
					      if (g_current->name.left(7)=="static ")
					      {
						g_current->stat = TRUE;
						g_current->name= g_current->name.mid(7);
					      }
					      else if (g_current->name.left(7)=="inline ")
					      {
						if (g_current->type.isEmpty())
						{
						  g_current->type="inline";
						}
						else
						{
						  g_current->type+="inline ";
						}
						g_current->name= g_current->name.mid(7);
					      }
					      else if (g_current->name.left(6)=="const ")
					      {
						if (g_current->type.isEmpty())
						{
						  g_current->type="const";
						}
						else
						{
						  g_current->type+="const ";
						}
						g_current->name=g_current->name.mid(6);
					      }
					    }
					    QCString tmp=yytext;
					    if (nameIsOperator(tmp))
                                            {
					      BEGIN( Operator );
                                            }
					    else
                                            {
					      g_externC=FALSE; // see bug759247
		 			      BEGIN(FindMembers);
                                            }
					  }
					}
<StaticAssert>"("                       {
					  g_lastSkipRoundContext = FindMembers;
					  g_roundCount=0;
                                          BEGIN(SkipRound);
                                        }
<StaticAssert>{BN}+                     { lineCount(); }
<StaticAssert>.                         { // variable with static_assert as name?
                                          unput(*yytext);
                                          BEGIN(FindMembers);
                                        }
<DeclType>"("                           {
					  g_current->type+=yytext;
  					  g_lastRoundContext=FindMembers;
  					  g_pCopyRoundString=&g_current->type;
					  g_roundCount=0;
                                          BEGIN(CopyRound);
                                        }
<DeclType>{BN}+                         { lineCount(); }
<DeclType>.                             {
                                          unput(*yytext);
                                          BEGIN(FindMembers);
                                        }
<CSIndexer>"["[^\n\]]*"]"		{
					  g_current->name+=removeRedundantWhiteSpace(yytext);
  					  BEGIN(FindMembers);
  					}
<FindMembers>[0-9]{ID}			{ // some number where we did not expect one
  					}
<FindMembers>"."			{
  					  if (g_insideJava || g_insideCS || g_insideD)
					  {
					    g_current->name+=".";
					  }
  					}
<FindMembers>"::"			{
					  g_current->name+=yytext;
  					}
<CppQuote>"("{B}*"\""			{
  					  g_insideCppQuote=TRUE;
  					  BEGIN(FindMembers);
  					}
<IDLUnionCase>"::"
<IDLUnionCase>":"			{ BEGIN(FindMembers); }
<IDLUnionCase>\n			{ lineCount(); }
<IDLUnionCase>.
<TryFunctionBlock>\n			{ lineCount(); }
<TryFunctionBlock>"{"			{ 
					  g_curlyCount=0;
					  g_lastCurlyContext = TryFunctionBlockEnd ;
  					  BEGIN( SkipCurly );
					}
<TryFunctionBlock>.
<TryFunctionBlockEnd>{BN}*"catch"	{ lineCount(); BEGIN(TryFunctionBlock); // {BN}* added to fix bug 611193
					}
<TryFunctionBlockEnd>\n			{ unput(*yytext); // rule added to fix bug id 601138
  					  BEGIN( FindMembers );
					}
<TryFunctionBlockEnd>.			{ unput(*yytext);
  					  BEGIN( FindMembers );
					}
<EndCppQuote>")"			{
  					  g_insideCppQuote=FALSE;
					  BEGIN(FindMembers);
  					}
<FindMembers,FindFields>{B}*"#"		{ if (g_insidePHP)
					    REJECT;
					  g_lastCPPContext = YY_START;
					  BEGIN( SkipCPP ) ;
					}
<FindMembers,FindFields>{B}*"#"{B}*("cmake")?"define"	{
  					  if (g_insidePHP)
					    REJECT;
  					  g_current->bodyLine = g_yyLineNr;
                                          g_lastDefineContext = YY_START;
  					  BEGIN( Define );
  					}
<FindMembers,ReadBody,ReadNSBody,ReadBodyIntf,SkipCurly,SkipCurlyCpp>{B}*"#"{B}+[0-9]+{B}+/"\""	{ /* line control directive */
                                          g_yyLineNr = atoi(&yytext[1]);
					  //printf("setting line number to %d\n",g_yyLineNr);
					  g_lastPreLineCtrlContext = YY_START;
					  if (YY_START==ReadBody || 
					      YY_START==ReadNSBody ||
					      YY_START==ReadBodyIntf)
					  {
  					    g_current->program+=yytext;
					  }
  					  BEGIN( PreLineCtrl );
  					}
<PreLineCtrl>"\""[^\n\"]*"\""		{
				          g_yyFileName = stripQuotes(yytext);
					  if (g_lastPreLineCtrlContext==ReadBody || 
					      g_lastPreLineCtrlContext==ReadNSBody ||
					      g_lastPreLineCtrlContext==ReadBodyIntf)
					  {
					    g_current->program+=yytext;
					  }
  					}
<PreLineCtrl>.				{
					  if (g_lastPreLineCtrlContext==ReadBody || 
					      g_lastPreLineCtrlContext==ReadNSBody ||
					      g_lastPreLineCtrlContext==ReadBodyIntf)
					  {
  					    g_current->program+=yytext;
					  }
                                        }
<PreLineCtrl>\n				{
					  if (g_lastPreLineCtrlContext==ReadBody || 
					      g_lastPreLineCtrlContext==ReadNSBody ||
					      g_lastPreLineCtrlContext==ReadBodyIntf)
					  {
  					    g_current->program+=yytext;
					  }
                                          lineCount();
  					  BEGIN( g_lastPreLineCtrlContext );
  					}
<SkipCPP>.
<SkipCPP>\\[\r]*"\n"[\r]*		{ lineCount(); }
<SkipCPP>[\r]*\n[\r]*			{ lineCount();
					  BEGIN( g_lastCPPContext) ;
					}
<Define>{ID}{B}*"("			{
  					  g_current->name = yytext;
					  g_current->name = g_current->name.left(g_current->name.length()-1).stripWhiteSpace();
					  g_current->args = "(";
  					  g_current->bodyLine = g_yyLineNr;
					  g_currentArgumentContext = DefineEnd;
					  g_fullArgString=g_current->args.copy();
					  g_copyArgString=&g_current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
 /*
<DefineArg>")"				{
  					  //printf("Define with args\n");
  					  g_current->args += ')';
  					  BEGIN( DefineEnd );
  					}
<DefineArg>.				{
  					  g_current->args += *yytext;
  					}
  */
<Define>{ID}				{
  					  //printf("Define '%s' without args\n",yytext);
                                          if (g_insideCpp || g_insideObjC) 
                                          {
                                            g_current->id = ClangParser::instance()->lookup(g_yyLineNr,yytext);
                                          }
  					  g_current->bodyLine = g_yyLineNr;
  					  g_current->name = yytext;
					  BEGIN(DefineEnd);
  					}
<DefineEnd>\n				{
  					  //printf("End define: doc=%s docFile=%s docLine=%d\n",g_current->doc.data(),g_current->docFile.data(),g_current->docLine);
                                          lineCount();
					  g_current->fileName   = g_yyFileName;
					  g_current->startLine  = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->type.resize(0);
  					  g_current->args       = g_current->args.simplifyWhiteSpace();
  					  g_current->name       = g_current->name.stripWhiteSpace();
					  g_current->section    = Entry::DEFINE_SEC;
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  BEGIN(g_lastDefineContext);
  					}
<DefinePHPEnd>";"			{
  					  //printf("End define\n");
					  g_current->fileName   = g_yyFileName;
					  g_current->startLine  = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  g_current->type.resize(0);
                                          g_current->type       = "const";
                                          QCString init = g_current->initializer.data();
  					  init = init.simplifyWhiteSpace();
  					  init = init.left(init.length()-1);
                                          g_current->initializer = init;
  					  g_current->name       = g_current->name.stripWhiteSpace();
					  g_current->section    = Entry::VARIABLE_SEC; 
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  BEGIN(FindMembers);
  					}
<DefinePHPEnd>.
<DefineEnd>\\[\r]?\n			{
                                          lineCount();
  					}
<DefineEnd>\"				{
					  if (g_insideIDL && g_insideCppQuote)
					  {
					    BEGIN(EndCppQuote);
					  }
					  else
					  {
					    g_lastStringContext=DefineEnd;
					    BEGIN(SkipString);
					  }
  					}
<DefineEnd>.				
<DefinePHP>{ID}["']{BN}*","{BN}*	{
  					  g_current->name = yytext;
					  g_current->name = g_current->name.stripWhiteSpace();
					  g_current->name = g_current->name.left(g_current->name.length()-1).stripWhiteSpace();
					  g_current->name = g_current->name.left(g_current->name.length()-1);
  					  g_current->bodyLine = g_yyLineNr;
  					  g_lastRoundContext = DefinePHPEnd;
  					  g_pCopyRoundGString = &g_current->initializer;
					  g_roundCount = 0;
  					  BEGIN( GCopyRound );
					}

<FindMembers>[\^%]			{  // ^ and % are C++/CLI extensions
  					  if (g_insideCli)
					  {
  					    addType();
                                            g_current->name = yytext ; 
					  }
					  else
					  {
					    REJECT;
					  }
  					}
<FindMembers>[*&]+			{ 
                                          g_current->name += yytext ;  
  					  addType();
					}
<FindMembers,MemberSpec,Function,NextSemi,EnumBaseType,BitFields,ReadInitializer,OldStyleArgs,DefinePHPEnd>";"{BN}*("/**"|"//!"|"/*!"|"///")"<" {
					  if (g_current->bodyLine==-1)
					  {
					    g_current->bodyLine=g_yyLineNr;
					  }
					  g_docBlockContext   = YY_START;
					  g_docBlockInBody    = FALSE;
					  g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                      ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;
                                          //printf("indent=%d\n",computeIndent(yytext+1,g_column));
  					  lineCount();

					  g_docBlockTerm = ';';
                                          if (YY_START==EnumBaseType && g_current->section==Entry::ENUM_SEC)
                                          {
                                            g_current->bitfields = ":"+g_current->args;
                                            g_current->args.resize(0);
                                            g_current->section=Entry::VARIABLE_SEC;
                                          }
					  if (yytext[yyleng-3]=='/')
					  {
					    startCommentBlock(TRUE);
					    BEGIN( DocLine );
					  }
					  else
					  {
					    startCommentBlock(FALSE);
					    BEGIN( DocBlock );
					  }
  					}
<MemberSpec,FindFields,FindMembers,NextSemi,EnumBaseType,BitFields,ReadInitializer,OldStyleArgs>","{BN}*("/**"|"//!"|"/*!"|"///")"<" {
					  g_docBlockContext   = YY_START;
					  g_docBlockInBody    = FALSE;
					  g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                      ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;
  					  lineCount();

					  g_docBlockTerm = ',';
                                          if (YY_START==EnumBaseType && g_current->section==Entry::ENUM_SEC)
                                          {
                                            g_current->bitfields = ":"+g_current->args;
                                            g_current->args.resize(0);
                                            g_current->section=Entry::VARIABLE_SEC;
                                          }
					  if (yytext[yyleng-3]=='/')
					  {
					    startCommentBlock(TRUE);
					    BEGIN( DocLine );
					  }
					  else
					  {
					    startCommentBlock(FALSE);
					    BEGIN( DocBlock );
					  }
  					}
<DefineEnd,FindFields,FindFieldArg,ReadInitializer,OldStyleArgs>{BN}*("/**"|"//!"|"/*!"|"///")"<" {
					  if (g_current->bodyLine==-1)
					  {
					    g_current->bodyLine=g_yyLineNr;
					  }
					  g_docBlockContext   = YY_START;
					  g_docBlockInBody    = FALSE;
					  g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                      ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;
  					  lineCount();

					  g_docBlockTerm = 0;
					  if (yytext[yyleng-3]=='/')
					  {
					    startCommentBlock(TRUE);
					    BEGIN( DocLine );
					  }
					  else
					  {
					    startCommentBlock(FALSE);
					    BEGIN( DocBlock );
					  }
  					}
   
<FindMembers,FindFields>("//"([!/]){B}*{CMD}"{")|("/*"([!*]){B}*{CMD}"{")	{
  					  //handleGroupStartCommand(g_current->name);
                                          if (g_previous && g_previous->section==Entry::GROUPDOC_SEC)
					  {
					    // link open command to the group defined in the g_previous entry
					    Doxygen::docGroup.open(g_previous,g_yyFileName,g_yyLineNr);
					  }
					  else
					  {
					    // link open command to the g_current entry
					    Doxygen::docGroup.open(g_current.get(),g_yyFileName,g_yyLineNr);
					  }
					  //g_current = tmp;
					  initEntry();
					  if (yytext[1]=='/')
					  {
					    if (yytext[2]=='!' || yytext[2]=='/')
					    {
					      g_docBlockContext   = YY_START;
					      g_docBlockInBody    = FALSE;
					      g_docBlockAutoBrief = FALSE;
					      g_docBlock.resize(0);
					      g_docBlockTerm = 0;
					      startCommentBlock(TRUE);
					      BEGIN(DocLine);
					    }
					    else
					    {
					      g_lastCContext=YY_START;
					      BEGIN(SkipCxxComment);
					    }
					  }
					  else
					  {
					    if (yytext[2]=='!' || yytext[2]=='*')
					    {
					      g_docBlockContext   = YY_START;
					      g_docBlockInBody    = FALSE;
					      g_docBlock.resize(0);
					      g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                          ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
					      g_docBlockTerm = 0;
					      startCommentBlock(FALSE);
					      BEGIN(DocBlock);
					    }
					    else
					    {
					      g_lastCContext=YY_START;
					      BEGIN(SkipComment);
					    }
					  }
  					}
<FindMembers,FindFields,ReadInitializer>"//"([!/]){B}*{CMD}"}".*|"/*"([!*]){B}*{CMD}"}"[^*]*"*/"	{
                                          bool g_insideEnum = YY_START==FindFields || (YY_START==ReadInitializer && g_lastInitializerContext==FindFields); // see bug746226
  					  Doxygen::docGroup.close(g_current.get(),g_yyFileName,g_yyLineNr,g_insideEnum);
  					  lineCount();
  					}
<FindMembers>"="			{ // in PHP code this could also be due to "<?="
  					  g_current->bodyLine = g_yyLineNr;
                                          g_current->initializer = yytext;
					  g_lastInitializerContext = YY_START;
					  g_initBracketCount=0;
					  BEGIN(ReadInitializer);
  					}
<UNOIDLAttributeBlock>{BN}*[gs]"et"{BN}+"raises"{BN}*"("{BN}*{SCOPENAME}{BN}*(","{BN}*{SCOPENAME}{BN}*)*")"{BN}*";"	{
					  lineCount();
					  g_current->exception += " ";
					  g_current->exception += removeRedundantWhiteSpace(yytext);
					}
<UNOIDLAttributeBlock>"}"		{
					  g_current->exception += " }";
					  BEGIN(FindMembers);
					}
  /* Read initializer rules */
<ReadInitializer>"("			{
  					  g_lastRoundContext=YY_START;
  					  g_pCopyRoundGString=&g_current->initializer;
					  g_roundCount=0;
  					  g_current->initializer+=*yytext; 
  					  BEGIN(GCopyRound);
  					}
<ReadInitializer>"["                    {
                                          if (!g_insidePHP) REJECT;
                                          g_lastSquareContext=YY_START;
                                          g_pCopySquareGString=&g_current->initializer;
                                          g_squareCount=0;
                                          g_current->initializer+=*yytext; 
                                          BEGIN(GCopySquare);
                                        }
<ReadInitializer>"{"			{
  					  g_lastCurlyContext=YY_START;
  					  g_pCopyCurlyGString=&g_current->initializer;
					  g_curlyCount=0;
  					  g_current->initializer+=*yytext; 
  					  BEGIN(GCopyCurly);
  					}
<ReadInitializer>[;,]			{
  					  //printf(">> initializer '%s' <<\n",g_current->initializer.data());
                                          if (*yytext==';' && (g_current_root->spec&Entry::Enum))
					  {
					    g_current->fileName   = g_yyFileName;
					    g_current->startLine  = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
  					    g_current->args       = g_current->args.simplifyWhiteSpace();
  					    g_current->name       = g_current->name.stripWhiteSpace();
					    g_current->section    = Entry::VARIABLE_SEC;
					    g_current_root->moveToSubEntryAndRefresh(g_current);
					    initEntry();
					    BEGIN(FindMembers);
					  }
					  else if (*yytext==';' || (g_lastInitializerContext==FindFields && g_initBracketCount==0)) // g_initBracketCount==0 was added for bug 665778
					  {
  					    unput(*yytext);
  					    BEGIN(g_lastInitializerContext);
					  }
					  else if (*yytext==',' && g_initBracketCount==0) // for "int a=0,b=0"
					  {
  					    unput(*yytext);
  					    BEGIN(g_lastInitializerContext);
					  }
					  else
					  {
  					    g_current->initializer+=*yytext; 
					  }
  					}
<ReadInitializer>{RAWBEGIN}             { // C++11 raw string
                                          if (!g_insideCpp)
                                          { 
                                            REJECT;
                                          }
                                          else
                                          {
                                            QCString text=yytext;
                                            g_current->initializer+=text;
                                            int i=text.find('"');
                                            g_delimiter = yytext+i+1;
                                            g_delimiter=g_delimiter.left(g_delimiter.length()-1);
                                            g_lastRawStringContext = YY_START;
                                            g_pCopyRawGString = &g_current->initializer;
                                            BEGIN(RawGString);
                                            //printf("RawGString delimiter='%s'\n",delimiter.data());
                                          }
                                        }
<RawGString>{RAWEND}                    {
                                          *g_pCopyRawGString+=yytext;
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==g_delimiter)
                                          {
                                            BEGIN(g_lastRawStringContext);
                                          }
                                        }
<RawGString>[^)\n]+                     {
                                          *g_pCopyRawGString+=yytext;
                                        }
<RawGString>.                           {
                                          *g_pCopyRawGString+=yytext;
                                        }
<RawGString>\n                          {
                                          *g_pCopyRawGString+=yytext;
                                          lineCount();
                                        }
<RawString>{RAWEND}                     {
                                          *g_pCopyRawString+=yytext;
  					  g_fullArgString+=yytext;
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==g_delimiter)
                                          {
                                            BEGIN(g_lastRawStringContext);
                                          }
                                        }
<RawString>[^)]+                        {
                                          *g_pCopyRawString+=yytext;
  					  g_fullArgString+=yytext;
                                        }
<RawString>.                            {
                                          *g_pCopyRawString+=yytext;
  					  g_fullArgString+=yytext;
                                        }
<RawString>\n                           {
                                          *g_pCopyRawString+=yytext;
  					  g_fullArgString+=yytext;
                                          lineCount();
                                        }
<ReadInitializer>\"			{
					  if (g_insideIDL && g_insideCppQuote)
  					  {
					    BEGIN(EndCppQuote);
					  }
					  else
					  {
                                            g_lastStringContext=YY_START;
  					    g_current->initializer+=yytext; 
  					    g_pCopyQuotedGString=&g_current->initializer;
					    BEGIN(CopyGString);
					  }
					}
<ReadInitializer>"->"			{
  					  g_current->initializer+=yytext; 
					}
<ReadInitializer>"<<"			{
  					  g_current->initializer+=yytext; 
					}
<ReadInitializer>">>"			{
  					  g_current->initializer+=yytext; 
  					}
<ReadInitializer>[<\[{(]		{
  					  g_initBracketCount++;
  					  g_current->initializer+=*yytext; 
  					}
<ReadInitializer>[>\]})]		{
  					  g_initBracketCount--;
  					  g_current->initializer+=*yytext; 
  					}
<ReadInitializer>\'			{	
  					  if (g_insidePHP)
					  {
					    g_current->initializer+=yytext; 
                                            g_pCopyQuotedGString = &g_current->initializer;
                                            g_lastStringContext=YY_START;
					    BEGIN(CopyPHPGString);
					  }
					  else
					  {
					    g_current->initializer+=yytext; 
					  }
  					}
<ReadInitializer>{CHARLIT}              { 
                                          if (g_insidePHP) 
					  {
					    REJECT;
					  }
					  else
					  {  
					    g_current->initializer+=yytext; 
					  }
                                        } 
<ReadInitializer>\n			{
  					  g_current->initializer+=*yytext;
                                          lineCount();
  					}
<ReadInitializer>"@\""			{ 
  					  //printf("g_insideCS=%d\n",g_insideCS);
  					  g_current->initializer+=yytext;
                                          if (!g_insideCS && !g_insideObjC) 
					  {
					    REJECT;
					  }
					  else
					  {
					    // C#/ObjC verbatim string
					    g_lastSkipVerbStringContext=YY_START;
					    g_pSkipVerbString=&g_current->initializer;
					    BEGIN(SkipVerbString);
					  }
					}
<SkipVerbString>[^\n"]+			{
					  *g_pSkipVerbString+=yytext;
					}
<SkipVerbString>"\"\""			{ // quote escape
					  *g_pSkipVerbString+=yytext;
					}
<SkipVerbString>"\""			{
					  *g_pSkipVerbString+=*yytext;
					  BEGIN(g_lastSkipVerbStringContext);
					}
<SkipVerbString>\n			{
					  *g_pSkipVerbString+=*yytext;
                                          lineCount();
  					}
<SkipVerbString>.			{
					  *g_pSkipVerbString+=*yytext;
  					}
<ReadInitializer>"?>"			{
					  if (g_insidePHP)
					    BEGIN( FindMembersPHP );
					  else
					    g_current->initializer+=yytext;
  					}
<ReadInitializer>.			{ 
  					  g_current->initializer+=*yytext; 
					}

  /* generic quoted string copy rules */
<CopyString,CopyPHPString>\\.		{
  					  *g_pCopyQuotedString+=yytext;
  					}
<CopyString>\"				{ 
  					  *g_pCopyQuotedString+=*yytext;
  					  BEGIN( g_lastStringContext ); 
					}
<CopyPHPString>\'			{ 
  					  *g_pCopyQuotedString+=*yytext;
  					  BEGIN( g_lastStringContext ); 
					}
<CopyString,CopyPHPString>"/*"|"*/"|"//" {
  					  *g_pCopyQuotedString+=yytext;
  					}
<CopyString,CopyPHPString>\n		{
  					  *g_pCopyQuotedString+=*yytext;
                                          lineCount();
  					}
<CopyString,CopyPHPString>.		{
  					  *g_pCopyQuotedString+=*yytext;
  					}

  /* generic quoted growable string copy rules */
<CopyGString,CopyPHPGString>\\.		{
  					  *g_pCopyQuotedGString+=yytext;
  					}
<CopyGString>\"				{ 
  					  *g_pCopyQuotedGString+=*yytext;
  					  BEGIN( g_lastStringContext ); 
					}
<CopyPHPGString>\'			{ 
  					  *g_pCopyQuotedGString+=*yytext;
  					  BEGIN( g_lastStringContext ); 
					}
<CopyGString,CopyPHPGString>"<?php"	{ // we had an odd number of quotes.
					  *g_pCopyQuotedGString += yytext;
  					  BEGIN( g_lastStringContext ); 
					}
<CopyGString,CopyPHPGString>"/*"|"*/"|"//" {
  					  *g_pCopyQuotedGString+=yytext;
  					}
<CopyGString,CopyPHPGString>\n		{
  					  *g_pCopyQuotedGString+=*yytext;
                                          lineCount();
  					}
<CopyGString,CopyPHPGString>.		{
  					  *g_pCopyQuotedGString+=*yytext;
  					}

  /* generic round bracket list copy rules */
<CopyRound>\"				{
					  *g_pCopyRoundString+=*yytext;
  					  g_pCopyQuotedString=g_pCopyRoundString;
					  g_lastStringContext=YY_START;
					  BEGIN(CopyString);
					}
<CopyRound>"("				{
  					  *g_pCopyRoundString+=*yytext;
  					  g_roundCount++;
  					}
<CopyRound>")"				{
  					  *g_pCopyRoundString+=*yytext;
					  if (--g_roundCount<0)
					    BEGIN(g_lastRoundContext);
  					}
<CopyRound>\n				{
                                          lineCount();
  					  *g_pCopyRoundString+=*yytext;
  					}
<CopyRound>\'				{
  					  if (g_insidePHP)
					  {
					    g_current->initializer+=yytext; 
                                            g_pCopyQuotedString = g_pCopyRoundString;
                                            g_lastStringContext=YY_START;
					    BEGIN(CopyPHPString);
					  }
					  else
					  {
					    *g_pCopyRoundString+=yytext;
					  }
  					}
<CopyRound>{CHARLIT}		        { 
                                          if (g_insidePHP)
					  {
					    REJECT;
					  }
					  else
					  {
                                            *g_pCopyRoundString+=yytext; 
					  }
                                        }
<CopyRound>[^"'()\n]+			{
  					  *g_pCopyRoundString+=yytext;
  					}
<CopyRound>.				{
  					  *g_pCopyRoundString+=*yytext;
  					}

  /* generic round bracket list copy rules for growable strings */
<GCopyRound>\"				{
					  *g_pCopyRoundGString+=*yytext;
  					  g_pCopyQuotedGString=g_pCopyRoundGString;
					  g_lastStringContext=YY_START;
					  BEGIN(CopyGString);
					}
<GCopyRound>"("				{
  					  *g_pCopyRoundGString+=*yytext;
  					  g_roundCount++;
  					}
<GCopyRound>")"				{
  					  *g_pCopyRoundGString+=*yytext;
					  if (--g_roundCount<0)
					    BEGIN(g_lastRoundContext);
  					}
<GCopyRound>\n				{
                                          lineCount();
  					  *g_pCopyRoundGString+=*yytext;
  					}
<GCopyRound>\'				{
  					  if (g_insidePHP)
					  {
					    g_current->initializer+=yytext; 
                                            g_pCopyQuotedGString = g_pCopyRoundGString;
                                            g_lastStringContext=YY_START;
					    BEGIN(CopyPHPGString);
					  }
					  else
					  {
					    *g_pCopyRoundGString+=yytext;
					  }
  					}
<GCopyRound>{CHARLIT}		        { 
                                          if (g_insidePHP)
					  {
					    REJECT;
					  }
					  else
					  {
                                            *g_pCopyRoundGString+=yytext; 
					  }
                                        }
<GCopyRound>[^"'()\n/]+			{
  					  *g_pCopyRoundGString+=yytext;
  					}
<GCopyRound>.				{
  					  *g_pCopyRoundGString+=*yytext;
  					}

  /* generic square bracket list copy rules for growable strings, we should only enter here in case of php, left the test part as in GCopyRound to keep it compatible with the round bracket version */
<GCopySquare>\"                         {
                                          *g_pCopySquareGString+=*yytext;
                                            g_pCopyQuotedGString=g_pCopySquareGString;
                                          g_lastStringContext=YY_START;
                                          BEGIN(CopyGString);
                                        }
<GCopySquare>"["                        {
                                          *g_pCopySquareGString+=*yytext;
                                          g_squareCount++;
                                        }
<GCopySquare>"]"                        {
                                          *g_pCopySquareGString+=*yytext;
                                          if (--g_squareCount<0)
                                          BEGIN(g_lastSquareContext);
                                        }
<GCopySquare>\n                         {
                                          lineCount();
                                          *g_pCopySquareGString+=*yytext;
                                        }
<GCopySquare>\'                         {
                                          if (g_insidePHP)
                                          {
                                            g_current->initializer+=yytext; 
                                            g_pCopyQuotedGString = g_pCopySquareGString;
                                            g_lastStringContext=YY_START;
                                            BEGIN(CopyPHPGString);
                                          }
                                          else
                                          {
                                            *g_pCopySquareGString+=yytext;
                                          }
                                        }
<GCopySquare>{CHARLIT}                  { 
                                          if (g_insidePHP)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            *g_pCopySquareGString+=yytext; 
                                          }
                                        }
<GCopySquare>[^"\[\]\n/]+               {
                                          *g_pCopySquareGString+=yytext;
                                        }
<GCopySquare>.                          {
                                          *g_pCopySquareGString+=*yytext;
                                        }

  /* generic curly bracket list copy rules */
<CopyCurly>\"				{
					  *g_pCopyCurlyString+=*yytext;
  					  g_pCopyQuotedString=g_pCopyCurlyString;
					  g_lastStringContext=YY_START;
					  BEGIN(CopyString);
					}
<CopyCurly>\'				{
					  *g_pCopyCurlyString+=*yytext;
  					  if (g_insidePHP)
					  {
					    g_pCopyQuotedString=g_pCopyCurlyString;
					    g_lastStringContext=YY_START;
					    BEGIN(CopyPHPString);
					  }
					}
<CopyCurly>"{"				{
  					  *g_pCopyCurlyString+=*yytext;
					  g_curlyCount++;
  					}
<CopyCurly>"}"				{
					  *g_pCopyCurlyString+=*yytext;
					  if (--g_curlyCount<0)
					    BEGIN(g_lastCurlyContext); 
  					}
<CopyCurly>{CHARLIT}                    { if (g_insidePHP) 
                                          { 
					    REJECT; 
					  } 
					  else 
					  {
					    *g_pCopyCurlyString+=yytext; 
					  }
                                        }
<CopyCurly>[^"'{}\/\n]+			{
  					  *g_pCopyCurlyString+=yytext;
  					}
<CopyCurly>"/"				{ *g_pCopyCurlyString+=yytext; }
<CopyCurly>\n				{
                                          lineCount();
					  *g_pCopyCurlyString+=*yytext;
  					}
<CopyCurly>.				{
					  *g_pCopyCurlyString+=*yytext;
  					}

  /* generic curly bracket list copy rules for growable strings */
<GCopyCurly>^"#"{B}+[0-9]+{B}+"\""[^\"\n]+"\""{B}+"1"{B}*\n? { // start of included file marker
                                        }
<GCopyCurly>^"#"{B}+[0-9]+{B}+"\""[^\"\n]+"\""{B}+"2"{B}*\n? { // end of included file marker
					  QCString line = QCString(yytext);
					  int s = line.find(' ');
					  int e = line.find('"',s);
					  g_yyLineNr = line.mid(s,e-s).toInt();
					  if (yytext[yyleng-1]=='\n')
					  {
                                            lineCount();
                                            g_column=0;
					  }
  					}
<GCopyCurly>\"				{
					  *g_pCopyCurlyGString+=*yytext;
  					  g_pCopyQuotedGString=g_pCopyCurlyGString;
					  g_lastStringContext=YY_START;
					  BEGIN(CopyGString);
					}
<GCopyCurly>\'				{
					  *g_pCopyCurlyGString+=*yytext;
  					  if (g_insidePHP)
					  {
					    g_pCopyQuotedGString=g_pCopyCurlyGString;
					    g_lastStringContext=YY_START;
					    BEGIN(CopyPHPGString);
					  }
					}
<GCopyCurly>"{"				{
  					  *g_pCopyCurlyGString+=*yytext;
					  g_curlyCount++;
  					}
<GCopyCurly>"}"				{
					  *g_pCopyCurlyGString+=*yytext;
					  if (--g_curlyCount<0)
					    BEGIN(g_lastCurlyContext); 
  					}
<GCopyCurly>{CHARLIT}                    { if (g_insidePHP) 
                                          { 
					    REJECT; 
					  } 
					  else 
					  {
					    *g_pCopyCurlyGString+=yytext; 
					  }
                                        }
<GCopyCurly>[^"'{}\/\n,]+		{
  					  *g_pCopyCurlyGString+=yytext;
  					}
<GCopyCurly>[,]+		        {
  					  *g_pCopyCurlyGString+=yytext;
  					}
<GCopyCurly>"/"				{ *g_pCopyCurlyGString+=yytext; }
<GCopyCurly>\n				{
                                          lineCount();
					  *g_pCopyCurlyGString+=*yytext;
  					}
<GCopyCurly>.				{
					  *g_pCopyCurlyGString+=*yytext;
  					}

  /* ---------------------- */


<FindMembers>":"			{
					  if (g_current->type.isEmpty() &&
                                              g_current->name=="enum") // see bug 69041, C++11 style anon enum: 'enum : unsigned int {...}'
					  {
                                            g_current->section=Entry::ENUM_SEC;
					    g_current->name.resize(0);
                                            g_current->args.resize(0);
  					    BEGIN(EnumBaseType);
					  }
                                          else 
                                          {
                                            if (g_current->type.isEmpty()) // anonymous padding field, e.g. "int :7;"
					    {
					      addType();
					      g_current->name.sprintf("__pad%d__",g_padCount++);
					    }
  					    BEGIN(BitFields);
					    g_current->bitfields+=":";
                                          }
  					}
<BitFields>.				{
  					  g_current->bitfields+=*yytext;
  					}
<EnumBaseType>.				{
  					  g_current->args+=*yytext;
  					}
<EnumBaseType>\n			{
                                          lineCount();
  					  g_current->args+=' ';
  					}
<FindMembers>[;,]			{ 
  					  QCString oldType = g_current->type;
					  if (g_current->bodyLine==-1)
					  {
					    g_current->bodyLine = g_yyLineNr;
					  }
                                          if ( g_insidePHP && g_current->type.left(3) == "var" )
                                          {
                                            g_current->type = g_current->type.mid(3);
                                          }
					  if (g_isTypedef && g_current->type.left(8)!="typedef ")
					  {
					    g_current->type.prepend("typedef ");
					  }
					  bool stat = g_current->stat;
					  if (!g_current->name.isEmpty() && g_current->section!=Entry::ENUM_SEC)
					  {
					    g_current->type=g_current->type.simplifyWhiteSpace();
					    g_current->args=removeRedundantWhiteSpace(g_current->args);
					    g_current->name=g_current->name.stripWhiteSpace();
					    if (g_current->section==Entry::CLASS_SEC) // remove spec for "struct Bla bla;"
					    {
					    	g_current->spec = 0;
					    }
					    g_current->section = Entry::VARIABLE_SEC ;
					    g_current->fileName = g_yyFileName;
					    g_current->startLine = g_yyBegLineNr;
					    g_current->startColumn = g_yyBegColNr;
					    g_current_root->moveToSubEntryAndRefresh( g_current ) ;
                                            initEntry();
					  }
					  if ( *yytext == ',')
					  {
					    g_current->stat = stat; // the static attribute holds for all variables
					    g_current->name.resize(0);
					    g_current->args.resize(0);
					    g_current->brief.resize(0);
					    g_current->doc.resize(0);
					    g_current->initializer.resize(0);
					    g_current->bitfields.resize(0);
					    int i=oldType.length(); 
					    while (i>0 && (oldType[i-1]=='*' || oldType[i-1]=='&' || oldType[i-1]==' ')) i--;
					    g_current->type = oldType.left(i);
					  }
					  else
					  {
                                            g_mtype = Method;
                                            g_virt = Normal;
					    g_current->groups.clear();
					    initEntry();
					  }
					}

<FindMembers>"["			{ 
                                          if (g_insideSlice)
                                          {
          				    g_squareCount=1;
                                            g_lastSquareContext = YY_START;
                                            g_current->metaData += "[";
                                            BEGIN( SliceMetadata );
                                          }
                                          else if (!g_insideCS &&  
					      (g_current->name.isEmpty() || 
					       g_current->name=="typedef"
					      )
					     ) // IDL function property
 					  {
          				    g_squareCount=1;
					    g_lastSquareContext = YY_START;
					    g_idlAttr.resize(0);
					    g_idlProp.resize(0);
					    g_current->mtype = g_mtype;

					    if (Config_getBool(IDL_PROPERTY_SUPPORT) &&
					         g_current->mtype == Property)
					    { // we are g_inside the properties section of a dispinterface
					      g_odlProp = true;
					      g_current->spec |= Entry::Gettable;
					      g_current->spec |= Entry::Settable;
					    }

					    BEGIN( IDLAttribute );
					  }
                                          else if (g_insideCS &&
                                                  g_current->name.isEmpty())
                                          {
                                            g_squareCount=1;
                                            g_lastSquareContext = YY_START;
                                            // Skip the C# attribute
                                            // for this member
                                            g_current->args.resize(0);
                                            BEGIN( SkipSquare );
                                          }
					  else
					  {
  					    g_current->args += yytext ;
					    g_squareCount=1;
					    g_externC=FALSE; // see bug759247
					    BEGIN( Array ) ;
					  }
					}
<SliceMetadata>"["                      { // Global metadata.
          				  g_squareCount++;
                                          g_current->metaData += "[";
                                        }
<SliceMetadata>{BN}*                    {
          				  lineCount();
                                        }
<SliceMetadata>\"[^\"]*\"               {
                                          g_current->metaData += yytext;
                                        }
<SliceMetadata>","                      {
                                          g_current->metaData += yytext;
                                        }
<SliceMetadata>"]"                      {
                                          g_current->metaData += yytext;
                                          if (--g_squareCount<=0)
                                          {
                                            BEGIN (g_lastSquareContext);
                                          }
                                        }
<SliceOptional>"("                      {
                                          g_current->type += "(";
                                          g_roundCount++;
                                        }
<SliceOptional>[0-9]+                   {
                                          g_current->type += yytext;
                                        }
<SliceOptional>")"                      {
                                          g_current->type += ")";
                                          if(--g_roundCount<=0)
                                          {
                                            BEGIN (g_lastModifierContext);
                                          }
                                        }
<IDLAttribute>"]"			{
  					  // end of IDL function attribute
					  if (--g_squareCount<=0)
					  {
					    lineCount();
					    if (g_current->mtype == Property)
					      BEGIN( IDLPropName );
					    else
					      BEGIN( g_lastSquareContext );
					  }
					}
<IDLAttribute>"propput"	                {
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT))
					  {
					    g_current->mtype = Property;
					  }
					  g_current->spec |= Entry::Settable;
					}
<IDLAttribute>"propget" 		{
                                          if (Config_getBool(IDL_PROPERTY_SUPPORT))
					  {
					    g_current->mtype = Property;
					  }
					  g_current->spec |= Entry::Gettable;
					}
<IDLAttribute>"property" { // UNO IDL property
					  g_current->spec |= Entry::Property;
					}
<IDLAttribute>"attribute" { // UNO IDL attribute
					  g_current->spec |= Entry::Attribute;
					}
<IDLAttribute>"optional" { // on UNO IDL interface/service/attribute/property
                           g_current->spec |= Entry::Optional;
                         }
<IDLAttribute>"readonly" { // on UNO IDL attribute or property
					  if (Config_getBool(IDL_PROPERTY_SUPPORT) && g_odlProp)
					  {
					    g_current->spec ^= Entry::Settable;
					  }
					  else
					  {
					    g_current->spec |= Entry::Readonly;
					  }
					}
<IDLAttribute>"bound" { // on UNO IDL attribute or property
					  g_current->spec |= Entry::Bound;
					}
<IDLAttribute>"removable" { // on UNO IDL property
					  g_current->spec |= Entry::Removable;
					}
<IDLAttribute>"constrained" { // on UNO IDL property
					  g_current->spec |= Entry::Constrained;
					}
<IDLAttribute>"transient" { // on UNO IDL property
					  g_current->spec |= Entry::Transient;
					}
<IDLAttribute>"maybevoid" { // on UNO IDL property
					  g_current->spec |= Entry::MaybeVoid;
					}
<IDLAttribute>"maybedefault" { // on UNO IDL property
					  g_current->spec |= Entry::MaybeDefault;
					}
<IDLAttribute>"maybeambiguous" { // on UNO IDL property
					  g_current->spec |= Entry::MaybeAmbiguous;
					}
<IDLAttribute>.				{
					}
<IDLPropName>{BN}*{ID}{BN}*		{
					  // return type (probably HRESULT) - skip it

					  if (g_odlProp)
					  { // property type
					    g_idlProp = yytext;
					  }
					}
<IDLPropName>{ID}{BN}*"("		{
  					  g_current->name = yytext;
					  g_current->name = g_current->name.left(g_current->name.length()-1).stripWhiteSpace();
  					  g_current->startLine = g_yyLineNr;
					  g_current->startColumn = g_yyColNr;
					  BEGIN( IDLProp );
					}
<IDLPropName>{BN}*"("{BN}*{ID}{BN}*")"{BN}*	{
					   if (g_odlProp)
					   {
					     g_idlProp += yytext;
					   }
					}
<IDLPropName>{ID}{BN}*/";"		{
					   if (g_odlProp)
					   {
					     g_current->name = yytext;
					     g_idlProp = g_idlProp.stripWhiteSpace();
					     g_odlProp = false;

					     BEGIN( IDLProp );
					   }
					}
<IDLProp>{BN}*"["[^\]]*"]"{BN}*		{  // attribute of a parameter
					   g_idlAttr = yytext;
					   g_idlAttr=g_idlAttr.stripWhiteSpace();
					}
<IDLProp>{ID}				{  // property type
					   g_idlProp = yytext;
					}
<IDLProp>{BN}*{ID}{BN}*"," 		{  // Rare: Another parameter ([propput] HRESULT Item(int index, [in] Type theRealProperty);)
					  if (!g_current->args)
					    g_current->args = "(";
					  else
					    g_current->args += ", ";
					  g_current->args += g_idlAttr;
					  g_current->args += " ";
					  g_current->args += g_idlProp;	// prop was actually type of extra parameter
					  g_current->args += " ";
					  g_current->args += yytext;
					  g_current->args = g_current->args.left(g_current->args.length() - 1);	// strip comma
					  g_idlProp.resize(0);
					  g_idlAttr.resize(0);
					  BEGIN( IDLProp );
					}
<IDLProp>{BN}*{ID}{BN}*")"{BN}*		{
					  // the parameter name for the property - just skip.
					}
<IDLProp>";"				{
					  g_current->fileName   = g_yyFileName;
					  g_current->type		= g_idlProp;
  					  g_current->args       = g_current->args.simplifyWhiteSpace();
  					  if (g_current->args)
  					    g_current->args += ")";
  					  g_current->name       = g_current->name.stripWhiteSpace();
					  g_current->section    = Entry::VARIABLE_SEC;
					  g_current_root->moveToSubEntryAndRefresh(g_current);
					  initEntry();
					  BEGIN( FindMembers );
					}
<IDLProp>.				{ // spaces, *, or other stuff
  					  //g_idlProp+=yytext;
 					}
<Array>"]"		                { g_current->args += *yytext ;
					  if (--g_squareCount<=0)
	                                     BEGIN( FindMembers ) ;
					}
<FuncFuncArray>"]"		        { g_current->args += *yytext ;
					  if (--g_squareCount<=0)
	                                     BEGIN( Function ) ;
					}
<Array,FuncFuncArray>"["		{ g_current->args += *yytext ;
					  g_squareCount++;	
					}
<Array,FuncFuncArray>.			{ g_current->args += *yytext ; }
<SkipSquare>"["				{ g_squareCount++; }
<SkipSquare>"]"				{
  					  if (--g_squareCount<=0)
					    BEGIN( g_lastSquareContext );
  					}
<SkipSquare>\"				{
  					  g_lastStringContext=YY_START;
  				          BEGIN( SkipString ); 
					}
<SkipSquare>[^\n\[\]\"]+
<FindMembers>"<"			{ addType();
					  g_current->type += yytext ;
					  BEGIN( Sharp ) ;
					}
<Sharp>">"				{ g_current->type += *yytext ;
					  if (--g_sharpCount<=0)
	                                     BEGIN( FindMembers ) ;
					}
<Sharp>"<"				{ g_current->type += *yytext ;
					  g_sharpCount++;	
					}
<Sharp>{BN}+				{
                                          g_current->type += ' ';
  					  lineCount();
					}
<Sharp>.				{ g_current->type += *yytext ; }
<FindFields>{ID}			{
                                          if (g_insideCpp || g_insideObjC) 
                                          {
                                            g_current->id = ClangParser::instance()->lookup(g_yyLineNr,yytext);
                                          }
  					  g_current->bodyLine = g_yyLineNr;
  					  g_current->name     = yytext;
					}
<FindFields>"("				{
  					  // Java enum initializer
  					  unput('(');
  					  g_lastInitializerContext = YY_START;
					  g_initBracketCount=0;
                                          g_current->initializer = "=";
  					  BEGIN(ReadInitializer);
  					}
<FindFields>"="				{
  					  g_lastInitializerContext = YY_START;
					  g_initBracketCount=0;
                                          g_current->initializer = yytext;
  					  BEGIN(ReadInitializer);
  					}
<FindFields>";"                         {
  					  if (g_insideJava)  // g_last enum field in Java class
					  {
					    if (!g_current->name.isEmpty())
					    {
					      g_current->fileName   = g_yyFileName;
					      g_current->startLine  = g_yyLineNr;
					      g_current->startColumn = g_yyColNr;
					      if (!(g_current_root->spec&Entry::Enum))
					      {
					        g_current->type       = "@"; // enum marker
					      }
					      g_current->args       = g_current->args.simplifyWhiteSpace();
					      g_current->name       = g_current->name.stripWhiteSpace();
					      g_current->section    = Entry::VARIABLE_SEC;
					      g_current_root->moveToSubEntryAndRefresh(g_current);
					      initEntry();
					    }

					    BEGIN( FindMembers );
					  }
					  else
					  {
					    REJECT;
					  }
  					}
<SkipRemainder>\n			{ 
                                          lineCount();
                                        }
<SkipRemainder>[^\n]*
<FindFields>","				{
					  //printf("adding '%s' '%s' '%s' to enum '%s' (mGrpId=%d)\n",
					  //     g_current->type.data(), g_current->name.data(),
					  //     g_current->args.data(), g_current_root->name.data(),g_current->mGrpId);
  					  if (!g_current->name.isEmpty())
					  {
					    g_current->fileName   = g_yyFileName;
					    g_current->startLine  = g_yyLineNr;
					    g_current->startColumn = g_yyColNr;
					    if (!(g_current_root->spec&Entry::Enum))
					    {
					      g_current->type       = "@"; // enum marker
					    }
  					    g_current->args       = g_current->args.simplifyWhiteSpace();
  					    g_current->name       = g_current->name.stripWhiteSpace();
					    g_current->section    = Entry::VARIABLE_SEC;
					    // add to the scope of the enum
					    if (!g_insideCS && !g_insideJava &&
                                                !(g_current_root->spec&Entry::Strong)) 
                                                // for C# and Java 1.5+ enum values always have to be explicitly qualified,
                                                // same for C++11 style enums (enum class Name {})
					    {
					      // add to the scope surrounding the enum (copy!)
                                              // we cannot during it directly as that would invalidate the iterator in parseCompounds.
                                              //printf("*** adding outer scope entry for %s\n",g_current->name.data());
                                              g_outerScopeEntries.emplace_back(g_current_root->parent(), std::make_unique<Entry>(*g_current));
					    }
					    g_current_root->moveToSubEntryAndRefresh(g_current);
					    initEntry();
					  }
					  else // probably a redundant , 
					  {
				     	    g_current->reset();
					    initEntry();
					  }
  					}
<FindFields>"["				{ // attribute list in IDL
					  g_squareCount=1;
					  g_lastSquareContext = YY_START;
					  BEGIN(SkipSquare);
  					}
  /*
<FindFieldArg>","			{ unput(*yytext); BEGIN(FindFields); }
  */
<ReadBody,ReadNSBody,ReadBodyIntf>[^\r\n\#{}"@'/<]*	{ g_current->program += yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>"//".*		{ g_current->program += yytext ; }
<ReadBody,ReadNSBody,ReadBodyIntf>"#".*	{ if (!g_insidePHP) 
					    REJECT;
					  // append PHP comment.
					  g_current->program += yytext ;
					}
<ReadBody,ReadNSBody,ReadBodyIntf>@\"	{ g_current->program += yytext ; 
                                          g_pSkipVerbString = &g_current->program;
                                          g_lastSkipVerbStringContext=YY_START;
                                          BEGIN( SkipVerbString );
					}
<ReadBody,ReadNSBody,ReadBodyIntf>"<<<"	{ if (g_insidePHP)
                                          {
                                            g_current->program += yytext ; 
                                            g_pCopyHereDocGString = &g_current->program;
                                            g_lastHereDocContext=YY_START;
                                            BEGIN( CopyHereDoc );
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
					}
<ReadBody,ReadNSBody,ReadBodyIntf>\"	{ g_current->program += yytext ; 
                                          g_pCopyQuotedGString = &g_current->program;
                                          g_lastStringContext=YY_START;
                                          BEGIN( CopyGString );
					}
<ReadBody,ReadNSBody,ReadBodyIntf>"/*"{B}*		{ g_current->program += yytext ;
					  g_lastContext = YY_START ;
					  BEGIN( Comment ) ;
					}
<ReadBody,ReadNSBody,ReadBodyIntf>"/*"{BL}		{ g_current->program += yytext ;
					  ++g_yyLineNr ;
					  g_lastContext = YY_START ;
					  BEGIN( Comment ) ;
					}
<ReadBody,ReadNSBody,ReadBodyIntf>"'"	{
  					  if (!g_insidePHP)
					  {
					    g_current->program += yytext;
					  }
					  else
					  { // begin of single quoted string
					    g_current->program += yytext;
                                            g_pCopyQuotedGString = &g_current->program;
                                            g_lastStringContext=YY_START;
					    BEGIN(CopyPHPGString);
					  }
  					}
<ReadBody,ReadNSBody,ReadBodyIntf>{CHARLIT} { 
                                              if (g_insidePHP) 
					      {
						REJECT; // for PHP code single quotes 
					                // are used for strings of arbitrary length
					      }
					      else
					      {
                                                g_current->program += yytext; 
					      }
                                            }
<ReadBody,ReadNSBody,ReadBodyIntf>"{"   { g_current->program += yytext ;
					  ++g_curlyCount ;
					}
<ReadBodyIntf>"}"			{
					  g_current->program += yytext ;
					  --g_curlyCount ;
  					}
<ReadBody,ReadNSBody>"}"		{ //err("ReadBody count=%d\n",g_curlyCount);
  					  if ( g_curlyCount>0 )
					  {
					    g_current->program += yytext ;
					    --g_curlyCount ;
					  }
					  else
					  {
					    g_current->endBodyLine = g_yyLineNr;
                                            Entry * original_root = g_current_root; // save root this namespace is in
                                            if (g_current->section == Entry::NAMESPACE_SEC && g_current->type == "namespace")
                                            {
                                              int split_point;
                                              // save documentation values
                                              QCString doc       = g_current->doc;
                                              int      docLine   = g_current->docLine;
                                              QCString docFile   = g_current->docFile;
                                              QCString brief     = g_current->brief;
                                              int      briefLine = g_current->briefLine;
                                              QCString briefFile = g_current->briefFile;
                                              // reset documentation values
                                              g_current->doc       = "";
                                              g_current->docLine   = 0;
                                              g_current->docFile   = "";
                                              g_current->brief     = "";
                                              g_current->briefLine = 0;
                                              g_current->briefFile = "";
                                              while ((split_point = g_current->name.find("::")) != -1)
                                              {
                                                std::unique_ptr<Entry> new_g_current = std::make_unique<Entry>(*g_current);
                                                g_current->program = "";
                                                new_g_current->name  = g_current->name.mid(split_point + 2);
                                                g_current->name  = g_current->name.left(split_point);
                                                if (!g_current_root->name.isEmpty()) g_current->name.prepend(g_current_root->name+"::");

                                                Entry *tmp = g_current.get();
                                                g_current_root->moveToSubEntryAndKeep(g_current);
                                                g_current_root = tmp;
                                                g_current.swap(new_g_current);
                                              }
                                              // restore documentation values
                                              g_current->doc       = doc;
                                              g_current->docLine   = docLine;
                                              g_current->docFile   = docFile;
                                              g_current->brief     = brief;
                                              g_current->briefLine = briefLine;
                                              g_current->briefFile = briefFile;
                                            }
					    QCString &cn = g_current->name;
					    QCString rn = g_current_root->name.copy();
					    //printf("cn='%s' rn='%s' g_isTypedef=%d\n",cn.data(),rn.data(),g_isTypedef);
					    if (!cn.isEmpty() && !rn.isEmpty())
					    {
					      prependScope();
					    }
					    if (g_isTypedef && cn.isEmpty())
					    {
					      //printf("Typedef Name\n");
					      BEGIN( TypedefName );
					    }
					    else
					    {
					      if ((g_current->section == Entry::ENUM_SEC) || (g_current->spec&Entry::Enum))
					      {
					        g_current->program+=','; // add field terminator
					      }
					      // add compound definition to the tree
					      g_current->args=removeRedundantWhiteSpace(g_current->args);
					                // was: g_current->args.simplifyWhiteSpace();
  					      g_current->type = g_current->type.simplifyWhiteSpace();
  					      g_current->name = g_current->name.stripWhiteSpace();
					      //printf("adding '%s' '%s' '%s' brief=%s g_insideObjC=%d %x\n",g_current->type.data(),g_current->name.data(),g_current->args.data(),g_current->brief.data(),g_insideObjC,g_current->section);
					      if (g_insideObjC && 
						  ((g_current->spec&Entry::Interface) || (g_current->spec==Entry::Category))
						 ) // method definition follows
					      {
				                BEGIN( ReadBodyIntf ) ;
					      }
					      else
					      {
					        g_memspecEntry = g_current.get();
					        g_current_root->copyToSubEntry( g_current ) ;
						if (g_current->section==Entry::NAMESPACE_SEC ||
						    (g_current->spec==Entry::Interface) ||
						    g_insideJava || g_insidePHP || g_insideCS || g_insideD || g_insideJS ||
                                                    g_insideSlice
						   )
						{ // namespaces and interfaces and java classes ends with a closing bracket without semicolon
                                                  g_current->reset();
                                                  g_current_root = original_root; // restore scope from before namespace descent
						  initEntry();
						  g_memspecEntry = 0;
						  BEGIN( FindMembers ) ;
						}
						else
						{
                                                  static QRegExp re("@[0-9]+$");
						  if (!g_isTypedef && g_memspecEntry &&
                                                      g_memspecEntry->name.find(re)==-1) // not typedef or anonymous type (see bug691071)
						  {
						    // enabled the next two lines for bug 623424
						    g_current->doc.resize(0);
						    g_current->brief.resize(0);
						  }
						  BEGIN( MemberSpec ) ;
						}
					      }
					    }
					  }
					}
<ReadBody>"}"{BN}+"typedef"{BN}+	{ //err("ReadBody count=%d\n",g_curlyCount);
					  lineCount();
  					  if ( g_curlyCount>0 )
					  {
					    g_current->program += yytext ;
					    --g_curlyCount ;
					  }
					  else
					  {
					    g_isTypedef = TRUE;
					    g_current->endBodyLine = g_yyLineNr;
					    QCString &cn = g_current->name;
					    QCString rn = g_current_root->name.copy();
					    if (!cn.isEmpty() && !rn.isEmpty())
					    {
					      prependScope();
					    }
					    BEGIN( TypedefName );
					  }
					}
<TypedefName>("const"|"volatile"){BN}	{ // late "const" or "volatile" keyword
					  lineCount();
  					  g_current->type.prepend(yytext);
  					}
<TypedefName>{ID}			{
					  if ((g_current->section == Entry::ENUM_SEC) || (g_current->spec&Entry::Enum))
					  {
					    g_current->program+=","; // add field terminator
					  }
  				          g_current->name=yytext;
					  prependScope();
  					  g_current->args = g_current->args.simplifyWhiteSpace();
  					  g_current->type = g_current->type.simplifyWhiteSpace();
					  //printf("Adding compound %s %s %s\n",g_current->type.data(),g_current->name.data(),g_current->args.data());
					  if (!g_firstTypedefEntry)
					  {
					    g_firstTypedefEntry = g_current.get();
					  }
					  g_current_root->moveToSubEntryAndRefresh( g_current ) ;
					  initEntry();
					  g_isTypedef=TRUE; // to undo reset by initEntry()
  					  BEGIN(MemberSpecSkip); 
  					}
<TypedefName>";"			{ /* typedef of anonymous type */
					  g_current->name.sprintf("@%d",g_anonCount++);
					  if ((g_current->section == Entry::ENUM_SEC) || (g_current->spec&Entry::Enum))
					  {
					    g_current->program+=','; // add field terminator
					  }
					  // add compound definition to the tree
  					  g_current->args = g_current->args.simplifyWhiteSpace();
  					  g_current->type = g_current->type.simplifyWhiteSpace();
					  g_memspecEntry = g_current.get();
					  g_current_root->moveToSubEntryAndRefresh( g_current ) ;
					  initEntry();
					  unput(';');
					  BEGIN( MemberSpec ) ;
  					}
<MemberSpec>([*&]*{BN}*)*{ID}{BN}*("["[^\]\n]*"]")* { // the [] part could be improved.
  					  lineCount();
  					  int i=0,l=(int)yyleng,j;
					  while (i<l && (!isId(yytext[i]))) i++;
					  g_msName = QCString(yytext).right(l-i).stripWhiteSpace();
					  j=g_msName.find("[");
					  if (j!=-1) 
					  {
					    g_msArgs=g_msName.right(g_msName.length()-j);
					    g_msName=g_msName.left(j);
					  }
					  g_msType=QCString(yytext).left(i);

					  // handle *pName in: typedef { ... } name, *pName;
					  if (g_firstTypedefEntry) 
					  {
					    if (g_firstTypedefEntry->spec&Entry::Struct)
					    {
					      g_msType.prepend("struct "+g_firstTypedefEntry->name);
					    }
					    else if (g_firstTypedefEntry->spec&Entry::Union)
					    {
					      g_msType.prepend("union "+g_firstTypedefEntry->name);
					    }
					    else if (g_firstTypedefEntry->section==Entry::ENUM_SEC)
					    {
					      g_msType.prepend("enum "+g_firstTypedefEntry->name);
					    }
					    else
					    {
					      g_msType.prepend(g_firstTypedefEntry->name);
					    }
					  }
					}
<MemberSpec>"("				{ // function with struct return type
  					  addType();
  					  g_current->name = g_msName;
					  g_current->spec = 0;
  					  unput('(');
					  BEGIN(FindMembers);
  					}
<MemberSpec>[,;]			{
  					  if (g_msName.isEmpty() && !g_current->name.isEmpty())
					  { 
					    // see if the compound does not have a name or is g_inside another
					    // anonymous compound. If so we insert a 
					    // special 'anonymous' variable.
					    //Entry *p=g_current_root;
					    const Entry *p=g_current.get();
					    while (p)
					    {
					      // only look for class scopes, not namespace scopes
					      if ((p->section & Entry::COMPOUND_MASK) && !p->name.isEmpty())
					      {
						//printf("Trying scope '%s'\n",p->name.data());
						int i=p->name.findRev("::");
						int pi = (i==-1) ? 0 : i+2;
						if (p->name.at(pi)=='@')
						{
						  // anonymous compound g_inside -> insert dummy variable name
						  //printf("Adding anonymous variable for scope %s\n",p->name.data());
						  g_msName.sprintf("@%d",g_anonCount++); 
						  break;
						}
					      }
					      //p=p->parent;
					      if (p==g_current.get()) p=g_current_root; else p=p->parent();
					    }
					  }
					  //printf("g_msName=%s g_current->name=%s\n",g_msName.data(),g_current->name.data());
					  if (!g_msName.isEmpty() 
					      /*&& g_msName!=g_current->name*/) // skip typedef T {} T;, removed due to bug608493
					  {
					    bool typedefHidesStruct = Config_getBool(TYPEDEF_HIDES_STRUCT);
					    // case 1: typedef struct _S { ... } S_t; 
					    // -> omit typedef and use S_t as the struct name
					    if (typedefHidesStruct && 
						g_isTypedef && 
						((g_current->spec&(Entry::Struct|Entry::Union)) ||
						 g_current->section==Entry::ENUM_SEC )&&
						g_msType.stripWhiteSpace().isEmpty() && 
						g_memspecEntry)
					    {
					      g_memspecEntry->name=g_msName;
					    }
					    else // case 2: create a typedef field
					    {
                                              std::unique_ptr<Entry> varEntry=std::make_unique<Entry>();
					      varEntry->lang = g_language;
					      varEntry->protection = g_current->protection ;
					      varEntry->mtype = g_current->mtype;
					      varEntry->virt = g_current->virt;
					      varEntry->stat = g_current->stat;
					      varEntry->section = Entry::VARIABLE_SEC;
					      varEntry->name = g_msName.stripWhiteSpace();
					      varEntry->type = g_current->type.simplifyWhiteSpace()+" ";
					      varEntry->args = g_msArgs; 
					      if (g_isTypedef)
					      {
						varEntry->type.prepend("typedef ");
						//  //printf("g_current->name = %s %s\n",g_current->name.data(),g_msName.data());
					      }
					      if (typedefHidesStruct &&
						  g_isTypedef &&
						  (g_current->spec&(Entry::Struct|Entry::Union)) &&
						  g_memspecEntry
						 ) // case 1: use S_t as type for pS_t in "typedef struct _S {} S_t, *pS_t;"
					      {
						varEntry->type+=g_memspecEntry->name+g_msType;
					      }
					      else // case 2: use _S as type for for pS_t
					      {
						varEntry->type+=g_current->name+g_msType;
					      }
					      varEntry->fileName = g_yyFileName;
					      varEntry->startLine = g_yyLineNr;
					      varEntry->startColumn = g_yyColNr;
					      varEntry->doc = g_current->doc.copy();
					      varEntry->brief = g_current->brief.copy();
					      varEntry->mGrpId = g_current->mGrpId;
                                              varEntry->initializer = g_current->initializer;
                                              varEntry->groups = g_current->groups;
                                              varEntry->sli = g_current->sli;

					      //printf("Add: type='%s',name='%s',args='%s' brief=%s doc=%s\n",
					      //      varEntry->type.data(),varEntry->name.data(),
					      //      varEntry->args.data(),varEntry->brief.data(),varEntry->doc.data());
					      g_current_root->moveToSubEntryAndKeep(varEntry);
					    }
					  }
					  if (*yytext==';') // end of a struct/class ...
					  {
					    if (!g_isTypedef && g_msName.isEmpty() && g_memspecEntry && (g_current->section&Entry::COMPOUND_MASK))
					    { // case where a class/struct has a doc block after it
					      if (!g_current->doc.isEmpty())
					      {
					        g_memspecEntry->doc += g_current->doc;
					      }
					      if (!g_current->brief.isEmpty())
					      {
					        g_memspecEntry->brief += g_current->brief;
					      }
				  	    }
					    g_msType.resize(0);
					    g_msName.resize(0);
					    g_msArgs.resize(0);
					    g_isTypedef=FALSE;
					    g_firstTypedefEntry=0;
					    g_memspecEntry=0;
					    g_current->reset();
					    initEntry();
					    BEGIN( FindMembers );
					  }
					  else
					  {
					    g_current->doc.resize(0);
					    g_current->brief.resize(0);
					  }

  					}
<MemberSpec>"="				{ 
  					  g_lastInitializerContext=YY_START;
					  g_initBracketCount=0;
                                          g_current->initializer = yytext;
  					  BEGIN(ReadInitializer);
  					  /* BEGIN(MemberSpecSkip); */
					}
  /*
<MemberSpecSkip>"{"			{
  					  g_curlyCount=0;
					  g_lastCurlyContext = MemberSpecSkip;
					  g_previous = g_current;
  					  BEGIN(SkipCurly);
  					}
  */
<MemberSpecSkip>","			{ BEGIN(MemberSpec); }
<MemberSpecSkip>";"		        { unput(';'); BEGIN(MemberSpec); }
<ReadBody,ReadNSBody,ReadBodyIntf>{BN}{1,80} { g_current->program += yytext ;
					  lineCount() ;
					}
<ReadBodyIntf>"@end"/[^a-z_A-Z0-9]	{ // end of Objective C block
					  g_current_root->moveToSubEntryAndRefresh( g_current ) ;
					  initEntry();
					  g_language = g_current->lang = SrcLangExt_Cpp; // see bug746361
					  g_insideObjC=FALSE;
					  BEGIN( FindMembers ); 
  					}
<ReadBody,ReadNSBody,ReadBodyIntf>.	{ g_current->program += yytext ; }

<FindMembers>"("/{BN}*"::"*{BN}*({TSCOPE}{BN}*"::")*{TSCOPE}{BN}*")"{BN}*"(" | /* typedef void (A<int>::func_t)(args...) */
<FindMembers>("("({BN}*"::"*{BN}*{TSCOPE}{BN}*"::")*({BN}*[*&\^]{BN}*)+)+ {   /* typedef void (A::*ptr_t)(args...) or int (*func(int))[], the ^ is for Obj-C blocks */
  					  if (g_insidePHP) // reference parameter
					  {
					    REJECT
					  }
					  else
					  {
					    g_current->bodyLine = g_yyLineNr;
					    lineCount();
					    addType();
					    g_funcPtrType=yytext;
					    g_roundCount=0;
					    //g_current->type += yytext;
					    BEGIN( FuncPtr );
					  }
  					}
<FuncPtr>{SCOPENAME}			{
  					  g_current->name = yytext;
					  if (nameIsOperator(g_current->name))
					  {
					    BEGIN( FuncPtrOperator );
					  }
					  else
					  {
					    if (g_current->name=="const" || g_current->name=="volatile")
					    {
					      g_funcPtrType += g_current->name;
					    }
					    else
					    {
					      BEGIN( EndFuncPtr );
					    }
					  }
  					}
<FuncPtr>.				{
  					  //printf("error: FuncPtr '%c' unexpected at line %d of %s\n",*yytext,g_yyLineNr,g_yyFileName);
  					}
<FuncPtrOperator>"("{BN}*")"{BN}*/"("	{
  					  g_current->name += yytext;
					  g_current->name = g_current->name.simplifyWhiteSpace();
					  lineCount();
  					}
<FuncPtrOperator>\n			{
                                          lineCount();
  					  g_current->name += *yytext;
  					}
<FuncPtrOperator>"("			{
  					  unput(*yytext);
					  BEGIN( EndFuncPtr );
                                        }
<FuncPtrOperator>.			{
  					  g_current->name += *yytext;
  					}
<EndFuncPtr>")"{BN}*/";"		{ // a variable with extra braces
 					  lineCount();
					  g_current->type+=g_funcPtrType.data()+1;
  					  BEGIN(FindMembers);
  					}
<EndFuncPtr>")"{BN}*/"("		{ // a function pointer
  					  lineCount();
					  g_current->type+=g_funcPtrType+")";
					  BEGIN(FindMembers);
  					}
<EndFuncPtr>")"{BN}*/"["		{ // an array of variables
  					  lineCount();
					  g_current->type+=g_funcPtrType.data();
					  g_current->args += ")";
					  BEGIN(FindMembers);
  					}
<EndFuncPtr>"("				{ // a function returning a function or 
                                          // a function returning a pointer to an array
  					  g_current->args += *yytext ;
					  //g_roundCount=0;
					  //BEGIN( FuncFunc );
					  g_current->bodyLine = g_yyLineNr;
					  g_currentArgumentContext = FuncFuncEnd;
					  g_fullArgString=g_current->args.copy();
					  g_copyArgString=&g_current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
<EndFuncPtr>"["[^\n\]]*"]"		{
  					  g_funcPtrType+=yytext;
  					}
<EndFuncPtr>")"				{
  					  BEGIN(FindMembers);
  					}
<FuncFunc>"("				{
  					  g_current->args += *yytext ;
  					  ++g_roundCount;
					}
<FuncFunc>")"				{
  					  g_current->args += *yytext ;
  					  if ( g_roundCount )
					    --g_roundCount;
					  else
					  {
					    BEGIN(FuncFuncEnd);
					  }
  					}
<FuncFuncEnd>")"{BN}*"("		{
  					  lineCount();
					  g_current->type+=g_funcPtrType+")(";
					  BEGIN(FuncFuncType);
  					}
<FuncFuncEnd>")"{BN}*/[;{]		{
  					  lineCount();
					  g_current->type+=g_funcPtrType.data()+1;
  					  BEGIN(Function);
  					}
<FuncFuncEnd>")"{BN}*/"["		{ // function returning a pointer to an array
                                          lineCount();
					  g_current->type+=g_funcPtrType;
					  g_current->args+=")";
					  BEGIN(FuncFuncArray);
  					}
<FuncFuncEnd>.				{
  					  g_current->args += *yytext;
  					}
<FuncFuncType>"("			{
  					  g_current->type += *yytext;
					  g_roundCount++;
  					}
<FuncFuncType>")"			{
  					  g_current->type += *yytext;
  					  if (g_roundCount)
					    --g_roundCount;
					  else
					    BEGIN(Function);
					}
<FuncFuncType>{BN}*","{BN}*		{ lineCount() ; g_current->type += ", " ; }
<FuncFuncType>{BN}+			{ lineCount() ; g_current->type += ' ' ; }
<FuncFuncType>.				{
  					  g_current->type += *yytext;
  					}
<FindMembers>"("/{BN}*{ID}{BN}*"*"{BN}*{ID}*")"{BN}*"(" { // for catching typedef void (__stdcall *f)() like definitions
                                          if (g_current->type.left(7)=="typedef" && g_current->bodyLine==-1) 
					    // the bodyLine check is to prevent this guard to be true more than once
					  {
  					    g_current->bodyLine = g_yyLineNr;
					    BEGIN( GetCallType );
					  }
					  else if (!g_current->name.isEmpty()) // normal function
					  {
					    g_current->args = yytext;
					    g_current->bodyLine = g_yyLineNr;
					    g_currentArgumentContext = FuncQual;
					    g_fullArgString=g_current->args.copy();
					    g_copyArgString=&g_current->args;
					    BEGIN( ReadFuncArgType ) ;
					    //printf(">>> Read function arguments!\n");
					  }
					}
<GetCallType>{BN}*{ID}{BN}*"*"		{
  					  lineCount();
  					  addType();
					  g_funcPtrType="(";
					  g_funcPtrType+=yytext;
					  g_roundCount=0;
					  BEGIN( FuncPtr );
  					}
<FindMembers>"("			{ 
                                          if (!g_current->name.isEmpty())
					  {
					    g_current->args = yytext;
					    g_current->bodyLine = g_yyLineNr;
					    g_currentArgumentContext = FuncQual;
					    g_fullArgString=g_current->args.copy();
					    g_copyArgString=&g_current->args;
					    BEGIN( ReadFuncArgType ) ;
					    //printf(">>> Read function arguments g_current->argList.size()=%d\n",g_current->argList.size());
					  }
					}
  /*
<FindMembers>"("{BN}*("void"{BN}*)?")"	{
  					  lineCount();
  					  g_current->args = "()"; 
  					  BEGIN( FuncQual );
  					}
  */

  /*- Function argument reading rules ---------------------------------------*/

<ReadFuncArgType>[^ \/\r\t\n\)\(\"\'#]+ { *g_copyArgString+=yytext; 
  					  g_fullArgString+=yytext;
  					}
<CopyArgString,CopyArgPHPString>[^\n\\\"\']+		{ *g_copyArgString+=yytext; 
					  g_fullArgString+=yytext;
					}
<CopyArgRound>[^\/\n\)\(\"\']+		{ 
  					  *g_copyArgString+=yytext; 
  					  g_fullArgString+=yytext;
  					}
<ReadFuncArgType,ReadTempArgs>{BN}*	{
  					  *g_copyArgString+=" ";
  					  g_fullArgString+=" ";
  					  lineCount();
  					}
<ReadFuncArgType,CopyArgRound,CopyArgSharp,ReadTempArgs>{RAWBEGIN}	{
                                          g_delimiter = yytext+2;
                                          g_delimiter=g_delimiter.left(g_delimiter.length()-1);
                                          g_lastRawStringContext = YY_START;
                                          g_pCopyRawString = g_copyArgString;
                                          *g_pCopyRawString+=yytext;
  					  g_fullArgString+=yytext;
                                          BEGIN(RawString);
                                        }
<ReadFuncArgType,CopyArgRound,CopyArgSharp,ReadTempArgs>\"	{
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
					  g_lastCopyArgStringContext = YY_START;
  					  BEGIN( CopyArgString );
  					}
<ReadFuncArgType,ReadTempArgs>"("	{
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
  					  g_argRoundCount=0; 
					  g_lastCopyArgContext = YY_START;
					  BEGIN( CopyArgRound ); 
  					}
<ReadFuncArgType>")"			{ 
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
					  stringToArgumentList(g_fullArgString,g_current->argList);
					  if (g_insideJS)
					  {
					    fixArgumentListForJavaScript(g_current->argList);
					  }
                                          handleParametersCommentBlocks(g_current->argList);

					  /* remember the g_current documentation block, since
					     we could overwrite it with the documentation of
					     a function argument, which we then have to correct later
					     on
					   */
					  g_docBackup = g_current->doc;
					  g_briefBackup = g_current->brief;

					  BEGIN( g_currentArgumentContext );
					}
	/* a special comment */
<ReadFuncArgType,ReadTempArgs>("/*"[*!]|"//"[/!])("<"?)	{ 
                                          if (g_currentArgumentContext==DefineEnd)
					  {
					    // for defines we interpret a comment
					    // as documentation for the define 
					    int i;for (i=(int)yyleng-1;i>=0;i--)
					    {
					      unput(yytext[i]);
					    }
					    stringToArgumentList(g_fullArgString,g_current->argList);
                                            handleParametersCommentBlocks(g_current->argList);
					    BEGIN( g_currentArgumentContext );
					  }
					  else // not a define
					  {
					    // for functions we interpret a comment
					    // as documentation for the argument
					    g_fullArgString+=yytext;
					    g_lastCopyArgChar=0;
					    g_lastCommentInArgContext=YY_START;
					    if (yytext[1]=='/')
					      BEGIN( CopyArgCommentLine );
					    else
					      BEGIN( CopyArgComment );
					  }
  					}
	/* a non-special comment */
<ReadFuncArgType,ReadTempArgs>"/**/"	{ /* empty comment */ }
<ReadFuncArgType,ReadTempArgs>"/*"	{
  					  g_lastCContext = YY_START;
					  BEGIN( SkipComment );
  					}
<ReadFuncArgType,ReadTempArgs>"//"	{
  					  g_lastCContext = YY_START;
					  BEGIN( SkipCxxComment );
  					}
  /*
<ReadFuncArgType,ReadTempArgs>"'#"	{ if (g_insidePHP)
                                            REJECT;
  					  *g_copyArgString+=yytext; 
  					  g_fullArgString+=yytext; 
					}
<ReadFuncArgType,ReadTempArgs>"#"	{
  					  if (!g_insidePHP)
  					    REJECT;
  					  g_lastCContext = YY_START;
					  BEGIN( SkipCxxComment );
  					}
  */
	/* ')' followed by a special comment */
<ReadFuncArgType>")"{BN}*("/*"[*!]|"//"[/!])"<"	{
  					  lineCount();
                                          if (g_currentArgumentContext==DefineEnd)
					  {
					    // for defines we interpret a comment
					    // as documentation for the define 
					    int i;for (i=(int)yyleng-1;i>0;i--)
					    {
					      unput(yytext[i]);
					    }
					    *g_copyArgString+=*yytext;
					    g_fullArgString+=*yytext;
					    stringToArgumentList(g_fullArgString,g_current->argList);
                                            handleParametersCommentBlocks(g_current->argList);
					    BEGIN( g_currentArgumentContext );
					  }
					  else
					  {
					    // for functions we interpret a comment
					    // as documentation for the g_last argument
					    g_lastCopyArgChar=*yytext;
					    QCString text=&yytext[1];
					    text=text.stripWhiteSpace();
					    g_lastCommentInArgContext=YY_START;
					    g_fullArgString+=text;
					    if (text.find("//")!=-1)
					      BEGIN( CopyArgCommentLine );
					    else
					      BEGIN( CopyArgComment );
					  }
  					}
<CopyArgComment>^{B}*"*"+/{BN}+		
<CopyArgComment>[^\n\\\@\*]+		{ g_fullArgString+=yytext; }
<CopyArgComment>"*/"			{ g_fullArgString+=yytext; 
  					  if (g_lastCopyArgChar!=0)
					    unput(g_lastCopyArgChar); 
                                          BEGIN( g_lastCommentInArgContext ); 
					}
<CopyArgCommentLine>\n			{ g_fullArgString+=yytext;
                                          lineCount();
  					  if (g_lastCopyArgChar!=0)
					    unput(g_lastCopyArgChar);
					  BEGIN( g_lastCommentInArgContext );
  					}
<CopyArgCommentLine>{CMD}("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"dot"|"code")/[^a-z_A-Z0-9\-]	{ // verbatim command (which could contain nested comments!)
				          g_docBlockName=&yytext[1];
  					  g_fullArgString+=yytext;
					  BEGIN(CopyArgVerbatim);
  					}
<CopyArgCommentLine>{CMD}("f$"|"f["|"f{")	        {
				          g_docBlockName=&yytext[1];
					  if (g_docBlockName.at(1)=='[')
					  {
				            g_docBlockName.at(1)='}';
					  }
					  if (g_docBlockName.at(1)=='{')
					  {
				            g_docBlockName.at(1)='}';
					  }
  					  g_fullArgString+=yytext;
  					  BEGIN(CopyArgVerbatim);
                                        }
<CopyArgVerbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"enddot"|"endcode"|"f$"|"f]"|"f}")/[^a-z_A-Z0-9\-] { // end of verbatim block
  					  g_fullArgString+=yytext;
				          if (yytext[1]=='f') // end of formula
				          {
				            BEGIN(CopyArgCommentLine);
				          }
					  if (&yytext[4]==g_docBlockName)
					  {
  					    BEGIN(CopyArgCommentLine);
					  }
  					}
<CopyArgCommentLine>[^\\\@\n]+		{ g_fullArgString+=yytext; }
<CopyArgCommentLine>.			{ g_fullArgString+=*yytext; }
<CopyArgComment,CopyArgVerbatim>\n	{ g_fullArgString+=*yytext; lineCount(); }
<CopyArgComment,CopyArgVerbatim>.	{ g_fullArgString+=*yytext; }
<CopyArgComment>{CMD}("brief"|"short"){B}+ {
  					  warn(g_yyFileName,g_yyLineNr,
					      "Ignoring %cbrief command g_inside argument documentation",*yytext
					     );
                                          g_fullArgString+=' ';
                                        }
<ReadTempArgs>"<"			{
					  *g_copyArgString+=*yytext;
					  g_fullArgString+=*yytext;
					  g_argSharpCount=1;
					  BEGIN( CopyArgSharp );
					}
<ReadTempArgs>">"			{
					  *g_copyArgString+=*yytext;
					  g_fullArgString+=*yytext;
					  //printf("end template list '%s'\n",g_copyArgString->data());
					  stringToArgumentList(g_fullArgString,*g_currentArgumentList);
					  BEGIN( g_currentArgumentContext );
					}
<CopyArgRound>"("			{
  					  g_argRoundCount++;
					  *g_copyArgString+=*yytext;
					  g_fullArgString+=*yytext;
  					}
<CopyArgRound>")"			{
					  *g_copyArgString+=*yytext;
					  g_fullArgString+=*yytext;
					  if (g_argRoundCount>0) 
					    g_argRoundCount--;
					  else 
					    BEGIN( g_lastCopyArgContext );
  					}
<CopyArgSharp>"("                       {
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
  					  g_argRoundCount=0; 
					  g_lastCopyArgContext = YY_START;
					  BEGIN( CopyArgRound ); 
                                        }
<CopyArgSharp>"<"			{
  					  g_argSharpCount++;
					  //printf("g_argSharpCount++=%d  copy\n",g_argSharpCount);
					  *g_copyArgString+=*yytext;
					  g_fullArgString+=*yytext;
  					}
<CopyArgSharp>">"			{
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
					  g_argSharpCount--;
					  if (g_argSharpCount>0)
					  {
					    //printf("g_argSharpCount--=%d copy\n",g_argSharpCount);
					  }
					  else
					  {
					    BEGIN( ReadTempArgs );
					    //printf("end of g_argSharpCount\n");
					  }
  					}
<CopyArgString,CopyArgPHPString>\\.	{
  					  *g_copyArgString+=yytext;
  					  g_fullArgString+=yytext;
  					}
<CopyArgString>\"			{
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
					  BEGIN( g_lastCopyArgStringContext );
  					}
<CopyArgPHPString>\'			{
  					  *g_copyArgString+=*yytext;
  					  g_fullArgString+=*yytext;
					  BEGIN( g_lastCopyArgStringContext );
  					}
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>{CHARLIT}     { 
                                          if (g_insidePHP)
					  {
					    REJECT;
					  }
					  else
					  {
  					    *g_copyArgString+=yytext; 
  					    g_fullArgString+=yytext; 
					  }
					}
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>\'     { 
  					  *g_copyArgString+=yytext; 
  					  g_fullArgString+=yytext; 
					  if (g_insidePHP)
					  {
					    g_lastCopyArgStringContext=YY_START;
					    BEGIN(CopyArgPHPString);
					  }
  					}
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSharp>\n  { 
                                          lineCount();
					  *g_copyArgString+=*yytext; 
					  g_fullArgString+=*yytext; 
					}
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgPHPString,CopyArgRound,CopyArgSharp>.	  { 
  					  *g_copyArgString+=*yytext; 
  					  g_fullArgString+=*yytext; 
					}



  /*------------------------------------------------------------------------*/


<FuncRound>"("				{ g_current->args += *yytext ;
					  ++g_roundCount ;
					}
<FuncRound>")"                          { g_current->args += *yytext ; 
					  if ( g_roundCount )
					    --g_roundCount ;
				          else
					    BEGIN( FuncQual ) ;
					}
  /*
<FuncQual>"#"				{ if (g_insidePHP)
  					    REJECT;
  					  g_lastCPPContext = YY_START;
  					  BEGIN(SkipCPP);
					}
  */
<FuncQual>[{:;,]                        {
                                          if ( qstrcmp(yytext,";")==0 && 
					       g_insidePHP && 
					       !containsWord(g_current->type,"function") )
                                          {
                                            g_current->reset();
                                            initEntry();
                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            unput(*yytext); BEGIN( Function );
                                          }
                                        }
<FuncQual>{BN}*"abstract"{BN}*          { // pure virtual member function
                                          lineCount() ; 
                                          g_current->virt = Pure;
                                          g_current->args += " override "; 
                                        }
<FuncQual,TrailingReturn>{BN}*"override"{BN}*          { // C++11 overridden virtual member function
                                          lineCount() ; 
                                          g_current->spec |= Entry::Override;
                                          g_current->args += " override "; 
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"final"{BN}*             { // C++11 final method
                                          lineCount() ; 
                                          g_current->spec |= Entry::Final;
                                          g_current->args += " final "; 
                                          BEGIN(FuncQual);
                                        }
<FuncQual>{BN}*"sealed"{BN}*            { // sealed member function
                                          lineCount() ; 
                                          g_current->spec |= Entry::Sealed;
                                          g_current->args += " sealed "; 
                                        }
<FuncQual>{BN}*"new"{BN}*               { // new member function
                                          lineCount() ; 
                                          g_current->spec |= Entry::New;
                                          g_current->args += " new "; 
                                        }
<FuncQual>{BN}*"const"{BN}*       	{ // const member function
  					  lineCount() ; 
  					  g_current->args += " const "; 
					  g_current->argList.constSpecifier=TRUE;
					}
<FuncQual>{BN}*"volatile"{BN}*    	{ // volatile member function
  					  lineCount() ; 
  					  g_current->args += " volatile "; 
					  g_current->argList.volatileSpecifier=TRUE;
					}
<FuncQual>{BN}*"noexcept"{BN}*    	{ // noexcept qualifier
  					  lineCount() ; 
  					  g_current->args += " noexcept "; 
					  g_current->spec |= Entry::NoExcept;
					}
<FuncQual>{BN}*"noexcept"{BN}*"("    	{ // noexcept expression
  					  lineCount() ; 
  					  g_current->args += " noexcept("; 
					  g_current->spec |= Entry::NoExcept;
  					  g_lastRoundContext=FuncQual;
  					  g_pCopyRoundString=&g_current->args;
					  g_roundCount=0;
                                          BEGIN(CopyRound);
                                        }
<FuncQual>{BN}*"&"                      {
                                          g_current->args += " &";
                                          g_current->argList.refQualifier=RefQualifierLValue;
                                        }
<FuncQual>{BN}*"&&"                     {
                                          g_current->args += " &&";
                                          g_current->argList.refQualifier=RefQualifierRValue;
                                        }

<FuncQual,TrailingReturn>{BN}*"="{BN}*"0"{BN}*  	{ // pure virtual member function
  					  lineCount() ; 
					  g_current->args += " = 0"; 
					  g_current->virt = Pure; 
					  g_current->argList.pureSpecifier=TRUE;
                                          BEGIN(FuncQual);
					}
<FuncQual,TrailingReturn>{BN}*"="{BN}*"delete"{BN}*     { // C++11 explicitly delete member
                                          lineCount();
                                          g_current->args += " = delete";
					  g_current->spec |= Entry::Delete;
					  g_current->argList.isDeleted=TRUE;
                                          BEGIN(FuncQual);
                                        }
<FuncQual,TrailingReturn>{BN}*"="{BN}*"default"{BN}*     { // C++11 explicitly defaulted constructor/assignment operator
                                          lineCount();
                                          g_current->args += " = default";
					  g_current->spec |= Entry::Default;
                                          BEGIN(FuncQual);
                                        }
<FuncQual>{BN}*"->"{BN}*                {
                                          lineCount();
                                          g_current->argList.trailingReturnType = " -> ";
                                          g_current->args += " -> ";
                                          BEGIN(TrailingReturn);
                                        }
<TrailingReturn>[{;]                    {
                                          unput(*yytext);
                                          BEGIN(FuncQual);
                                        }
<TrailingReturn>.                       {
                                          g_current->argList.trailingReturnType+=yytext;
                                          g_current->args+=yytext;
                                        }
<TrailingReturn>\n                      {
                                          lineCount();
                                          g_current->argList.trailingReturnType+=yytext;
                                          g_current->args+=' ';
                                        }
<FuncRound,FuncFunc>{BN}*","{BN}*	{ 
  					  lineCount() ; 
					  g_current->args += ", " ; 
					}
<FuncQual,FuncRound,FuncFunc>{BN}+   	{ 
  					  lineCount() ; 
					  g_current->args += ' ' ; 
					}
<Function,FuncQual,FuncRound,FuncFunc>"#" { if (g_insidePHP)
  					    REJECT;
  					  g_lastCPPContext = YY_START;
  					  BEGIN(SkipCPP);
					}	
<FuncQual>"="				{ 
                                          if (g_insideCli && 
					      (g_current_root->section&Entry::COMPOUND_MASK) 
					     )
					  {
					    BEGIN(CliOverride);
					  }
					  else
					  {
                                            // typically an initialized function pointer
  					    g_lastInitializerContext=YY_START;
					    g_initBracketCount=0;
                                            g_current->initializer = yytext;
  					    BEGIN(ReadInitializer);
					  }
  					}
<CliOverride>{ID}			{
  					}
<CliOverride>"{"			{ 
  					  unput(*yytext);
					  BEGIN(FuncQual);
  					}
<CliOverride>\n				{
                                          lineCount();
  					}
<CliOverride>.				{
  					}
<FuncPtrInit>[{;]			{
  					  unput(*yytext);
					  BEGIN(FuncQual);
  					}
<FuncPtrInit>\"				{
                                          g_current->args += *yytext; 
  					  g_pCopyQuotedString=&g_current->args;
					  g_lastStringContext=FuncPtrInit;
					  BEGIN(CopyString);
					}
<FuncPtrInit>\'				{
                                          g_current->args += *yytext; 
					  if (g_insidePHP)
					  {
					    g_pCopyQuotedString=&g_current->args;
					    g_lastStringContext=FuncPtrInit;
					    BEGIN(CopyPHPString);
					  }
					}
<FuncPtrInit>{CHARLIT}			{
                                          if (g_insidePHP)
					  {
					    REJECT;
					  }
					  else
					  {
                                            g_current->args += yytext; 
					  }
  					}
<FuncPtrInit>{ID}			{
                                          g_current->args += yytext; 
					}
<FuncPtrInit>.				{
                                          g_current->args += *yytext; 
  					}
<FuncPtrInit>\n				{
                                          g_current->args += *yytext; 
                                          lineCount();
  					}
<FuncQual>{ID}				{ // typically a K&R style C function
                                          if (g_insideCS && qstrcmp(yytext,"where")==0)
					  { 
					    // type constraint for a method
					    g_current->typeConstr.clear();
					    g_current->typeConstr.push_back(Argument());
					    g_lastCSConstraint = YY_START;
					    BEGIN( CSConstraintName );
					  }
					  else if (checkForKnRstyleC())
					  {
                                            g_current->args = yytext; 
					    g_oldStyleArgType.resize(0);
					    BEGIN(OldStyleArgs);
					  }
					  else
					  {
                                            g_current->args += yytext; 
					  }
  					}
<OldStyleArgs>[,;]			{
  					  QCString oldStyleArgPtr;
  					  QCString oldStyleArgName;
					  splitKnRArg(oldStyleArgPtr,oldStyleArgName);
					  QCString doc,brief;
					  if (g_current->doc!=g_docBackup)
					  {
                                            doc=g_current->doc.copy();
					    g_current->doc=g_docBackup;
					  }
					  if (g_current->brief!=g_briefBackup)
					  {
                                            brief=g_current->brief.copy();
					    g_current->brief=g_briefBackup;
					  }
					  addKnRArgInfo(g_oldStyleArgType+oldStyleArgPtr,
					                oldStyleArgName,brief,doc);
					  g_current->args.resize(0);
					  if (*yytext==';') g_oldStyleArgType.resize(0);
  					}
<OldStyleArgs>{ID} 			{ g_current->args += yytext; }
<OldStyleArgs>"{"			{
                                          if (g_current->argList.empty())
                                          {
                                            g_current->argList.noParameters=TRUE;
                                          }
  					  g_current->args = argListToString(g_current->argList);
  					  unput('{');
					  BEGIN(FuncQual);
  					}
<OldStyleArgs>.	 			{ g_current->args += *yytext; }
<FuncQual,FuncRound,FuncFunc>.		{ g_current->args += *yytext; }
<FuncQual>{BN}*"try:"			|
<FuncQual>{BN}*"try"{BN}+		{ /* try-function-block */ 
					  g_insideTryBlock=TRUE;
					  lineCount();
  					  if (yytext[yyleng-1]==':')
					  {
					    unput(':');
					    BEGIN( Function );
					  }
					}
<FuncQual>{BN}*"throw"{BN}*"("		{ // C++ style throw clause
  					  g_current->exception = " throw (" ;
					  g_roundCount=0;
					  lineCount() ;
					  BEGIN( ExcpRound ) ;
					}
<FuncQual>{BN}*"raises"{BN}*"("         {
  					  g_current->exception = " raises (" ;
					  lineCount() ;
					  g_roundCount=0;
					  BEGIN( ExcpRound ) ;
  					}
<FuncQual>{BN}*"throws"{BN}+		{ // Java style throw clause
  					  g_current->exception = " throws " ;
					  lineCount() ;
					  BEGIN( ExcpList );
  					}
<ExcpRound>"("				{ g_current->exception += *yytext ;
					  ++g_roundCount ;
					}
<ExcpRound>")"                          { g_current->exception += *yytext ; 
					  if ( g_roundCount )
					    --g_roundCount ;
				          else
					    BEGIN( FuncQual ) ;
					}
<ExcpRound>.				{
  					  g_current->exception += *yytext;
  					}
<ExcpList>"{"				{
  					  unput('{'); BEGIN( FuncQual );
  					}
<ExcpList>";"				{
  					  unput(';'); BEGIN( FuncQual );
  					}
<ExcpList>"\n"				{
  					  g_current->exception += ' ';
                                          lineCount();
  					}
<ExcpList>.				{
  					  g_current->exception += *yytext;
  					}
<Function>"("				{ g_current->type += g_current->name ;
					  g_current->name  = g_current->args ;
					  g_current->args  = yytext ;
					  g_roundCount=0;
					  BEGIN( FuncRound ) ;
					}
<Function>":"				{
  					  if (!g_insidePHP) BEGIN(SkipInits);
  					}
<Function>[;{,]				{ 
					  g_current->name=g_current->name.simplifyWhiteSpace();
  					  g_current->type=g_current->type.simplifyWhiteSpace();
					  g_current->args=removeRedundantWhiteSpace(g_current->args);
					                // was: g_current->args.simplifyWhiteSpace();
					  g_current->fileName = g_yyFileName;
					  g_current->startLine = g_yyBegLineNr;
					  g_current->startColumn = g_yyBegColNr;
					  static QRegExp re("([^)]*[*&][^)]*)"); // (...*...)
					  if (*yytext!=';' || (g_current_root->section&Entry::COMPOUND_MASK) )
					  {
					    int tempArg=g_current->name.find('<');
                                            int ts=g_current->type.find('<');
                                            int te=g_current->type.findRev('>');
                                            int ti=g_current->type.find(re,0);

                                            // bug677315: A<int(void *, char *)> get(); is not a function pointer
                                            bool isFunction = ti==-1 || // not a (...*...) pattern
                                                              (ts!=-1 && ts<te && ts<ti && ti<te); // (...*...) is part of a template argument list
                                                              
                                            //printf("type=%s ts=%d te=%d ti=%d isFunction=%d\n",
                                            //    g_current->type.data(),ts,te,ti,isFunction);
					    QCString tempName;
					    if (tempArg==-1) tempName=g_current->name; else tempName=g_current->name.left(tempArg);
					    if (!g_current->type.isEmpty() &&
						(!isFunction || g_current->type.left(8)=="typedef "))
					    {
					      //printf("Scanner.l: found in class variable: '%s' '%s' '%s'\n", g_current->type.data(),g_current->name.data(),g_current->args.data());
					      if (g_isTypedef && g_current->type.left(8)!="typedef ")
					      {
						g_current->type.prepend("typedef ");
					      }
					      g_current->section = Entry::VARIABLE_SEC ;
					    }
					    else	      
					    {
					      //printf("Scanner.l: found in class function: '%s' '%s' '%s'\n", g_current->type.data(),g_current->name.data(),g_current->args.data());
					      g_current->section = Entry::FUNCTION_SEC ;
			                      g_current->proto = *yytext==';';
					    }
					  }
					  else // a global function prototype or function variable
					  {
					    //printf("Scanner.l: prototype? type='%s' name='%s' args='%s'\n",g_current->type.data(),g_current->name.data(),g_current->args.data());
					    if (!g_current->type.isEmpty() && 
						(g_current->type.find(re,0)!=-1 || g_current->type.left(8)=="typedef "))
					    {
					      if (g_isTypedef && g_current->type.left(8)!="typedef ")
					      {
						g_current->type.prepend("typedef ");
					      }
					      //printf("Scanner.l: found function variable!\n");
					      g_current->section = Entry::VARIABLE_SEC;
					    }
					    else
					    {
					      //printf("Scanner.l: found prototype\n");
					      g_current->section = Entry::FUNCTION_SEC;
					      g_current->proto = TRUE;
					    }
					  }
					  //printf("Adding entry '%s'\n",g_current->name.data());
					  if ( g_insidePHP)
					  {
					    if (findAndRemoveWord(g_current->type,"final"))
					    {
					      g_current->spec |= Entry::Final;
					    }
					    if (findAndRemoveWord(g_current->type,"abstract"))
					    {
					      g_current->spec |= Entry::Abstract;
					    }
					  }
					  if ( g_insidePHP && !containsWord(g_current->type,"function"))
					  {
					    initEntry();
					    if ( *yytext == '{' )
					    {
					      g_lastCurlyContext = FindMembers;
					      g_curlyCount=0;
					      BEGIN( SkipCurly );
					    }
					    else
					    {
					      BEGIN( FindMembers );
					    }
					  }
					  else
					  {
					    if ( g_insidePHP)
					    {
					      findAndRemoveWord(g_current->type,"function");
					    }
					    g_previous = g_current.get();
					    g_current_root->moveToSubEntryAndRefresh(g_current);
					    initEntry();
					    // Objective C 2.0: Required/Optional section
					    if (g_previous->spec & (Entry::Optional | Entry::Required))
					    {
					      g_current->spec |= g_previous->spec & (Entry::Optional|Entry::Required);
					    }
					    g_lastCurlyContext = FindMembers;
					    if ( *yytext == ',' )
					    {
					      g_current->type = g_previous->type;
					      // we need to strip any trailing * and & (see bugs 623023 and 649103 for test cases)
					      int i=g_current->type.length(); 
					      while (i>0 && (g_current->type[i-1]=='*' || g_current->type[i-1]=='&' || g_current->type[i-1]==' ')) i--;
					      g_current->type = g_current->type.left(i);
					    }
					    if ( *yytext == '{' )
					    {
					      if ( !g_insidePHP && (g_current_root->section & Entry::COMPOUND_MASK) )
					      {
						g_previous->spec |= Entry::Inline;
					      }
					      //addToBody(yytext);
					      g_curlyCount=0;
					      BEGIN( SkipCurly ) ;
					    }
					    else
					    {
					      if (g_previous->section!=Entry::VARIABLE_SEC)
						g_previous->bodyLine=-1; // a function/member declaration
					      BEGIN( FindMembers ) ;
					    }
					  }
                                        }
<SkipInits>">"{BN}*"{"                  { // C++11 style initializer (see bug 790788)
                                          lineCount();
                                          g_curlyCount=1;
                                          BEGIN(SkipC11Inits);
                                        }
<SkipInits>{ID}{BN}*"{"                 { // C++11 style initializer (see bug 688647)
                                          lineCount();
                                          g_curlyCount=1;
                                          BEGIN(SkipC11Inits);
                                        }
<SkipC11Inits>"{"                       {
                                          ++g_curlyCount;
                                        }
<SkipC11Inits>"}"                       {
  					  if ( --g_curlyCount<=0 )
                                          {
                                            BEGIN(SkipInits);
                                          }
                                        }
<SkipC11Attribute>"]]"                 {
                                          BEGIN(g_lastC11AttributeContext);
                                        }
<SkipInits>"{"				{ // C++11 style initializer
  					  unput('{');
					  BEGIN( Function );
					}
<SkipCurly>"{"				{ 
  				          //addToBody(yytext);
  					  ++g_curlyCount ; 
					}
<SkipCurly>"}"/{BN}*("/*!"|"/**"|"//!"|"///")"<!--" | /* see bug710917 */
<SkipCurly>"}"				{ 
  				          //addToBody(yytext);
  					  if( g_curlyCount )
					  {
					    --g_curlyCount ;
					  }
					  else
					  {
					    if (!g_current->sli.empty() && g_previous) // copy special list items
					    {
                                              g_previous->sli = g_current->sli;
					      g_current->sli.clear();
					    }
					    if (g_previous) g_previous->endBodyLine=g_yyLineNr;
					    BEGIN( g_lastCurlyContext ) ;
					  }
					}
<SkipCurly>"}"{BN}*("/*!"|"/**"|"//!"|"///")"<" { 
					  lineCount();
  					  if ( g_curlyCount )
					  {
					    //addToBody(yytext);
					    --g_curlyCount ;
					  }
					  else
					  {
					    g_current->endBodyLine=g_yyLineNr;
                                            // take g_previous out of g_current_root and move it into g_current
					    g_current.swap(g_tempEntry); // remember g_current
                                            g_current_root->moveFromSubEntry(g_previous,g_current);
					    g_previous = 0;

					    g_docBlockContext   = SkipCurlyEndDoc;
					    g_docBlockInBody    = FALSE;
					    g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                        ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
					    g_docBlock.resize(0);
					    g_docBlockTerm = '}';
					    if (yytext[yyleng-3]=='/')
					    {
					      startCommentBlock(TRUE);
					      BEGIN( DocLine );
					    }
					    else
					    {
					      startCommentBlock(FALSE);
					      BEGIN( DocBlock );
					    }
					  }
					}
<SkipCurlyEndDoc>"}"{BN}*("/*!"|"/**"|"//!"|"///")"<" { // desc is followed by another one
					  g_docBlockContext   = SkipCurlyEndDoc;
					  g_docBlockInBody    = FALSE;
					  g_docBlockAutoBrief = ( yytext[yyleng-2]=='*' && Config_getBool(JAVADOC_AUTOBRIEF) ) ||
					                      ( yytext[yyleng-2]=='!' && Config_getBool(QT_AUTOBRIEF) );
					  g_docBlock.resize(0);
					  g_docBlockTerm = '}';
					  if (yytext[yyleng-3]=='/')
					  {
					    startCommentBlock(TRUE);
					    BEGIN( DocLine );
					  }
					  else
					  {
					    startCommentBlock(FALSE);
					    BEGIN( DocBlock );
					  }
  					}
<SkipCurlyEndDoc>"}"			{
  				          //addToBody("}");
                                          if (g_tempEntry) // we can only switch back to g_current if no new item was created
					  {
					    g_tempEntry.swap(g_current);
					    g_tempEntry.reset();
					  }
  					  BEGIN( g_lastCurlyContext );
  					}
<SkipCurly>\"			        { 
  				          //addToBody(yytext);
  					  g_lastStringContext=SkipCurly;
  				          BEGIN( SkipString ); 
					}
<SkipCurly>^{B}*"#"			{ 
  				          if (g_insidePHP)
  				            REJECT;
  				          //addToBody(yytext);
  					  BEGIN( SkipCurlyCpp );
					}
<SkipCurly,SkipC11Inits,SkipInits,SkipC11Attribute>\n	{
                                          lineCount();
  				          //addToBody(yytext);
  					}
<SkipCurly,SkipCurlyCpp,ReadInitializer>"<<<"	        {
  					  if (!g_insidePHP) 
					  {
					    REJECT;
					  }
					  else
					  {
                                            g_lastHereDocContext = YY_START;
					    BEGIN(HereDoc);
					  }
  					}
<SkipCurly,SkipCurlyCpp>{B}*{RAWBEGIN}  {
                                          QCString raw=QCString(yytext).stripWhiteSpace();
                                          g_delimiter = raw.data()+2;
                                          g_delimiter=g_delimiter.left(g_delimiter.length()-1);
                                          g_lastRawStringContext = YY_START;
                                          g_dummyRawString.resize(0);
                                          g_pCopyRawString = &g_dummyRawString;
                                          *g_pCopyRawString+=yytext;
                                          BEGIN(RawString);
                                        }
<SkipCurly,SkipCurlyCpp>[^\n#"'@\\/{}<]+ {
                                          lineCount();  // for g_column updates
  				          //addToBody(yytext);
  					}
<SkipCurlyCpp>\n			{ 
  				          //addToBody(yytext);
                                          lineCount();
  					  g_lastCurlyContext = FindMembers;
  					  BEGIN( SkipCurly ); 
					}
<SkipCurlyCpp>\\[\r]*"\n"[\r]*		{ 
  				          //addToBody(yytext);
                                          lineCount();
					}
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute>"/*"	{
  				          //addToBody(yytext);
  					  g_lastCContext = YY_START;
					  BEGIN(SkipComment);
  					}
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute>"//"  {
  				          //addToBody(yytext);
  					  g_lastCContext = YY_START;
					  BEGIN(SkipCxxComment);
  					}
<SkipInits,SkipC11Inits,SkipC11Attribute>"("             {
                                          g_roundCount=0;
                                          g_lastSkipRoundContext=YY_START;
                                          BEGIN(SkipRound);
                                        }
<SkipInits,SkipC11Inits,SkipC11Attribute>\"	        {
  					  g_lastStringContext=YY_START;
  				          BEGIN( SkipString ); 
					}
<SkipInits>;	                        {
  					  warn(g_yyFileName,g_yyLineNr,
					      "Found ';' while parsing initializer list! "
					      "(doxygen could be confused by a macro call without semicolon)"
					     );
					  BEGIN( FindMembers );
  					}
<SkipInits,SkipCurly,SkipCurlyCpp>"#"   {
  				          if (!g_insidePHP)
  				            REJECT;
  				          //addToBody(yytext);
  					  g_lastCContext = YY_START;
					  BEGIN(SkipCxxComment);
  					}
<SkipInits,SkipCurly,SkipCurlyCpp>@\"   {
  				          if (!g_insideCS) REJECT;
					  // C# verbatim string
					  g_lastSkipVerbStringContext=YY_START;
					  g_pSkipVerbString=&g_current->initializer;
					  BEGIN(SkipVerbString);
  					}
<SkipInits,SkipCurly,SkipCurlyCpp>{CHARLIT}	{
                                          if (g_insidePHP) REJECT;
                                        }
<SkipInits,SkipCurly,SkipCurlyCpp>\'	{
  					  if (g_insidePHP)
					  {
					    g_lastStringContext=YY_START;
					    BEGIN(SkipPHPString);
					  }
					}
<SkipInits,SkipC11Inits,SkipCurly,SkipCurlyCpp,SkipC11Attribute>.	{ }
<SkipString,SkipPHPString>\\.		{ }
<SkipString>\"				{ 
  					  BEGIN( g_lastStringContext ); 
					}
<SkipPHPString>\'			{ 
  					  BEGIN( g_lastStringContext ); 
					}
<SkipString,SkipPHPString>"/*"|"*/"|"//" { }
<SkipString,SkipPHPString>\n		{
                                          lineCount();
  					}
<SkipString,SkipPHPString>.		{ }
<CompoundName>":"			{ // for "class : public base {} var;" construct, see bug 608359
  					  unput(':');
					  BEGIN(ClassVar);
  					}
<CompoundName>";"			{
					  g_current->section = Entry::EMPTY_SEC ;
					  g_current->type.resize(0) ;
					  g_current->name.resize(0) ;
					  g_current->args.resize(0) ;
					  g_current->argList.clear();
					  BEGIN( FindMembers ) ;
					}
<Bases>";"			{
					  if (g_insideIDL && (g_current->spec & (Entry::Singleton |
					                                     Entry::Service)))
					  {
					    // in UNO IDL a service or singleton may be defined
					    // completely like this: "service Foo : XFoo;"
					    if (!g_current->name.isEmpty() && !g_current_root->name.isEmpty())
					    {
					      prependScope();
					    }
					    g_current->name = g_current->name.stripWhiteSpace();
					    // there can be only one base class here
					    if (!g_baseName.isEmpty())
					    {
					      g_current->extends.push_back(
					        BaseInfo(g_baseName,Public,Normal));
					      g_baseName.resize(0);
					    }
					    g_current_root->moveToSubEntryAndRefresh( g_current ) ;
                                            initEntry();
					  }
					  else
					  {
					    g_current->section = Entry::EMPTY_SEC ;
					    g_current->type.resize(0) ;
					    g_current->name.resize(0) ;
					    g_current->args.resize(0) ;
					    g_current->argList.clear();
					  }
					  BEGIN( FindMembers ) ;
					}
<CompoundName>{SCOPENAME}/{BN}*"<"	{
  					  g_sharpCount = 0;
  					  g_current->name = yytext ;
					  if (g_current->spec & Entry::Protocol)
					  {
					    g_current->name+="-p";
					  }
					  lineCount();
					  g_lastClassTemplSpecContext = ClassVar;
					  if (g_insideObjC) // protocol list
					  {
					    BEGIN( ObjCProtocolList );
					  }
					  else if (g_insideCS) // C# generic class
					  {
                                            //g_current->name+="-g";
					    BEGIN( CSGeneric );
					  }
					  else // C++ template specialization
					  {
					    g_roundCount=0;
					    BEGIN( ClassTemplSpec );
					  }
					}
<CSGeneric>"<"				{
					  ArgumentList al;
					  // check bug 612858 before enabling the next line
					  //g_current->spec |= Entry::Template;
					  g_current->tArgLists.push_back(al);
					  g_currentArgumentList = &g_current->tArgLists.back();
					  g_templateStr="<";
					  g_current->name += "<";
					  g_fullArgString = g_templateStr;
					  g_copyArgString = &g_current->name;
					  //g_copyArgString = &g_templateStr;
					  g_currentArgumentContext = ClassVar;
					  BEGIN( ReadTempArgs );
  					}
<ObjCProtocolList>"<"			{
  					  g_insideProtocolList=TRUE;
  					  BEGIN( Bases );
  					}
<ClassTemplSpec>">"({BN}*"::"{BN}*{SCOPENAME})?	{
					  g_current->name += yytext;
					  lineCount();
  					  if (g_roundCount==0 && --g_sharpCount<=0)
					  {
					    g_current->name = removeRedundantWhiteSpace(g_current->name);
					    if (g_current->spec & Entry::Protocol)
					    { // Objective-C protocol
					      unput('{'); // fake start of body
					      BEGIN( ClassVar );
					    }
					    else
					    {
					      BEGIN( g_lastClassTemplSpecContext );
					    }
					  }
					}
<ClassTemplSpec>"<"			{
					  g_current->name += yytext;
					  if (g_roundCount==0) g_sharpCount++;
  					}
<ClassTemplSpec>.			{
  					  g_current->name += yytext;
					}
<CompoundName>{SCOPENAME}{BN}*";"	{ // forward declaration
                                          if (!g_current->tArgLists.empty())
                                          {
                                            // found a forward template declaration, this has
                                            // a purpose of its own
                                            g_current->name = yytext;
                                            g_current->name=g_current->name.left(g_current->name.length()-1).stripWhiteSpace();
                                            //printf("template class declaration for %s!\n",g_current->name.data());
					    QCString rn = g_current_root->name.copy();
					    //printf("cn='%s' rn='%s' g_isTypedef=%d\n",cn.data(),rn.data(),g_isTypedef);
					    if (!g_current->name.isEmpty() && !rn.isEmpty())
					    {
					      prependScope();
					    }
                                            g_current->spec|=Entry::ForwardDecl;
                                            g_current_root->moveToSubEntryAndRefresh(g_current);
                                          }
					  else if (g_insideIDL &&
					           (((g_current_root->spec & (Entry::Interface |
					                                    Entry::Service)) &&
					             (g_current->spec & Entry::Interface)) ||
					            ((g_current_root->spec & (Entry::Service |
					                                    Entry::Singleton)) &&
					             (g_current->spec & Entry::Service))))
					  {
					    // interface g_inside of UNO IDL service or interface
					    // service g_inside of UNO IDL service or singleton
					    // there may be documentation on the member,
					    // so do not throw it away...
					    g_current->name = yytext;
					    g_current->name=g_current->name.left(g_current->name.length()-1).stripWhiteSpace();
					    g_current->section = (g_current->spec & Entry::Interface)
					        ? Entry::EXPORTED_INTERFACE_SEC
					        : Entry::INCLUDED_SERVICE_SEC;
//					    g_current->section = Entry::MEMBERDOC_SEC;
					    g_current->spec &= ~(Entry::Interface|Entry::Service); // FIXME: horrible: Interface == Gettable, so need to clear it - actually we're mixing values from different enums in this case... granted only Optional and Interface are actually valid in this context but urgh...
					    g_current_root->moveToSubEntryAndRefresh(g_current);
					  }

    					  unput(';');
					  g_current->reset();
					  initEntry();
                                          if (g_insideObjC) // see bug746361
                                          {
					    g_language = g_current->lang = SrcLangExt_Cpp;
                                            g_insideObjC = FALSE;
                                          }
					  if (g_isTypedef) // typedef of a class, put typedef keyword back
					  {
					    g_current->type.prepend("typedef");
					  }
					  BEGIN( FindMembers );
					}
<CompoundName>{SCOPENAME}/{BN}*"("	{ 
					  g_current->name = yytext ;
					  lineCount();
                                          if (g_insideCpp && g_current->name=="alignas") // C++11
                                          {
                                            g_lastAlignAsContext = YY_START;
                                            BEGIN( AlignAs );
                                          }
                                          else
                                          {
                                            if (g_current->spec & Entry::Protocol)
                                            {
                                              g_current->name += "-p";
                                            }
                                            BEGIN( ClassVar );
                                          }
					}
<AlignAs>"("                            { g_roundCount=0; 
                                          BEGIN( AlignAsEnd ); 
                                        }
<AlignAs>\n                             { lineCount(); }
<AlignAs>.
<AlignAsEnd>"("                         { g_roundCount++; }
<AlignAsEnd>")"                         { if (--g_roundCount<0) 
                                          {
                                            BEGIN( g_lastAlignAsContext ); 
                                          }
                                        }
<AlignAsEnd>\n                          { lineCount(); }
<AlignAsEnd>.
<CompoundName>{SCOPENAME}/{BN}*","	{  // multiple forward declarations on one line
                                           // e.g. @protocol A,B;
					  g_current->reset();
					  initEntry();
  					}
<CompoundName>{SCOPENAME}		{ 
					  g_current->name = yytext ;
                                          if (g_insideCpp || g_insideObjC) 
                                          {
                                            g_current->id = ClangParser::instance()->lookup(g_yyLineNr,yytext);
                                          }
					  lineCount();
					  if (g_current->spec & Entry::Protocol)
					  {
					    g_current->name += "-p";
					  }
					  if ((g_current->spec & Entry::Protocol) ||
					      g_current->section == Entry::OBJCIMPL_SEC)
					  {
					    unput('{'); // fake start of body
					  }
					  BEGIN( ClassVar );
					}
<CompoundName>{CSSCOPENAME}	        { // C# style scope
					  g_current->name = substitute(yytext,".","::");
					  lineCount();
					  BEGIN( ClassVar );
                                        }
<ClassVar>{SCOPENAME}{BN}*/"("		{
  					  if (g_insideIDL && qstrncmp(yytext,"switch",6)==0 && !isId(yytext[6]))
					  {
					    // Corba IDL style union
					    g_roundCount=0;
					    BEGIN(SkipUnionSwitch);
					  }
					  else
					  {
  					    addType();
					    g_current->name = yytext;
					    g_current->name = g_current->name.stripWhiteSpace();
					    lineCount();
  					    BEGIN( FindMembers );
					  }
  					}
<ClassVar>","				{
 					  if (g_isTypedef)
					  {
					    // multiple types in one typedef
					    unput(',');
					    g_current->type.prepend("typedef ");
					    BEGIN(FindMembers);
					  }
					  else
					  { 
  					    // Multiple class forward declaration
					  }
  					}
<ClassVar>("sealed"|"abstract")/{BN}*(":"|"{") {
  					  if (g_insideCli)
					  {
					    if (yytext[0]=='s') // sealed
					      g_current->spec |= Entry::SealedClass;
					    else // abstract
					      g_current->spec |= Entry::AbstractClass;
					    BEGIN( ClassVar ); 
					  }
					  else
					  {
					    REJECT;
					  }
					}
<ClassVar>{ID}				{
                                          if (g_insideCpp || g_insideObjC) 
                                          {
                                            g_current->id = ClangParser::instance()->lookup(g_yyLineNr,yytext);
                                          }
  					  if (g_insideIDL && qstrcmp(yytext,"switch")==0)
					  {
					    // Corba IDL style union
					    g_roundCount=0;
					    BEGIN(SkipUnionSwitch);
					  }
					  else if ((g_insideJava || g_insidePHP || g_insideJS || g_insideSlice) && (qstrcmp(yytext,"implements")==0 || qstrcmp(yytext,"extends")==0))
					  {
  					    g_current->type.resize(0);
					    g_baseProt=Public;
                                            g_baseVirt=Normal;
					    g_baseName.resize(0);
					    BEGIN( BasesProt ) ;
					  }
					  else if (g_insideCS && qstrcmp(yytext,"where")==0) // C# type constraint
					  {
                                            g_current->typeConstr.clear();
					    g_current->typeConstr.push_back(Argument());
					    g_lastCSConstraint = YY_START;
					    BEGIN( CSConstraintName );
					  }
					  else if (g_insideCli &&  qstrcmp(yytext,"abstract")==0)
					  {
					    g_current->spec|=Entry::Abstract;
					  }
					  else if (g_insideCli &&  qstrcmp(yytext,"sealed")==0)
					  {
					    g_current->spec|=Entry::Sealed;
					  }
                                          else if (qstrcmp(yytext,"final")==0)
                                          {
                                            g_current->spec|=Entry::Final;
                                          }
					  else
					  {
					    if (g_current->section == Entry::ENUM_SEC)
					    { // found "enum a b" -> variable
					      g_current->section = Entry::VARIABLE_SEC ;
					    }
					    g_current->type += ' ' ;
					    g_current->type += g_current->name ;
					    g_current->name = yytext ;
					    
					    if (nameIsOperator(g_current->name))
					    {
				              BEGIN( Operator );
					    }
					  }
  					}
<ClassVar>[(\[]				{
    					  if (g_insideObjC && *yytext=='(') // class category
					  {
					    g_current->name+='(';
                                            //if (g_current->section!=Entry::OBJCIMPL_SEC)
                                            //{
					      g_current->spec|=Entry::Category;
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
<CSConstraintType,CSConstraintName>"/**/" { /* empty comment */ }
<CSConstraintType,CSConstraintName>("/*"[*!]|"//"[/!])("<"?)	{ // special comment
                                          g_fullArgString.resize(0);
					  g_lastCopyArgChar='#'; // end marker
					  g_lastCommentInArgContext=YY_START;
					  if (yytext[1]=='/')
					    BEGIN( CopyArgCommentLine );
					  else
					    BEGIN( CopyArgComment );
					}
<CSConstraintType,CSConstraintName>"#"	{ // artificially inserted token to signal end of comment block
                                          g_current->typeConstr.back().docs = g_fullArgString;
  					}
<CSConstraintType>"{"			{ // end of type constraint reached
                                          // parse documentation of the constraints
                                          handleParametersCommentBlocks(g_current->typeConstr);
					  unput('{');
					  BEGIN( g_lastCSConstraint );
					}
<CSConstraintType,CSConstraintName>";"  {
                                          handleParametersCommentBlocks(g_current->typeConstr);
					  unput(';');
					  BEGIN( g_lastCSConstraint );
  					}
<CSConstraintName>":"                   {
                                          BEGIN( CSConstraintType );
                                        }
<CSConstraintName>{ID}                  {
                                          // parameter name
                                          g_current->typeConstr.back().name=yytext;
                                        }
<CSConstraintType>"where"		{ // another constraint for a different param
					  g_current->typeConstr.push_back(Argument());
                                          BEGIN( CSConstraintName );
  					}
<CSConstraintType>({ID}".")*{ID}("<"{ID}">")?("()")?  {
                                          if (g_current->typeConstr.back().type.isEmpty())
                                              // first type constraint for this parameter
                                          {
                                            g_current->typeConstr.back().type=yytext;
                                          }
                                          else // new type constraint for same parameter
                                          {
                                            QCString name = g_current->typeConstr.back().name;
                                            g_current->typeConstr.push_back(Argument());
                                            g_current->typeConstr.back().name=name;
                                            g_current->typeConstr.back().type=yytext;
                                          }
                                        }
<CSConstraintName,CSConstraintType>\n	{
                                          lineCount();
					}
<CSConstraintName,CSConstraintType>.	{
					}
<ClassCategory>{ID}			{
					  g_current->name+=yytext;
					}
<ClassCategory>")"/{BN}*"{"		{
					  g_current->name+=')';
					  BEGIN( ClassVar );
                                        }
<ClassCategory>")"/{BN}*"<"		{
					  g_current->name+=')';
					  BEGIN( ObjCProtocolList );
                                        }
<ClassCategory>")"			{
					  g_current->name+=')';
					  if ((g_current->section & Entry::Protocol) ||
					      g_current->section == Entry::OBJCIMPL_SEC)
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
<ClassVar>":"				{ 
                                          if (g_current->section==Entry::VARIABLE_SEC) // enum A B:2, see bug 748208
                                          {
                                            g_current->bitfields+=":";
                                            g_current->args.resize(0);
                                            BEGIN(BitFields);
                                          }
                                          else if (g_current->section==Entry::ENUM_SEC) // enum E:2, see bug 313527, 
                                                                                 // or C++11 style enum: 'E : unsigned int {...}'
					  {
					    g_current->args.resize(0);
  					    BEGIN(EnumBaseType);
					  }
					  else
					  {
					    g_current->type.resize(0);
					    if ((g_current->spec & Entry::Interface) || 
						(g_current->spec & Entry::Struct)    || 
						(g_current->spec & Entry::Ref)       || 
						(g_current->spec & Entry::Value)     || 
						g_insidePHP || g_insideCS || g_insideD || g_insideObjC || g_insideIDL
					       )
					      g_baseProt=Public;
					    else
					      g_baseProt=Private;
					    g_baseVirt=Normal;
					    g_baseName.resize(0);
					    BEGIN( BasesProt ) ;
					  }
					}
<ClassVar>[;=*&]			{
    					  unput(*yytext);
					  if (g_isTypedef) // typedef of a class, put typedef keyword back
					  {
					    g_current->type.prepend("typedef");
					  }
					  if ((yytext[0]=='*' || yytext[0]=='&') && 
					      g_current->section == Entry::ENUM_SEC)
					  { // found "enum a *b" -> variable
					    g_current->section = Entry::VARIABLE_SEC ;
					  }
					  BEGIN( FindMembers );
    					}
<Bases,ClassVar>"///"/[^/]              {
  					  if (!g_insideObjC)
					  {
					    REJECT;
					  }
					  else
					  {
					    lineCount();
					    g_current->program+=yytext;
                                            g_current->fileName = g_yyFileName ;
					    g_current->startLine = g_yyLineNr ;
					    g_current->startColumn = g_yyColNr;
					    g_curlyCount=0;
					    BEGIN( ReadBodyIntf );
					  }
  					}
<Bases,ClassVar>("//"{B}*)?"/**"/[^/*]  |
<Bases,ClassVar>("//"{B}*)?"/*!"        |
<Bases,ClassVar>"//!"                   |
<Bases,ClassVar>[\-+]{BN}*		{
  					  if (!g_insideObjC)
					  {
					    REJECT;
					  }
					  else
					  {
					    lineCount();
					    g_current->program+=yytext;
                                            g_current->fileName = g_yyFileName ;
					    g_current->startLine = g_yyLineNr ;
					    g_current->startColumn = g_yyColNr;
					    g_curlyCount=0;
					    BEGIN( ReadBodyIntf );
					  }
  					}
<CompoundName,ClassVar>{B}*"{"{B}*	{ 
                                          g_current->fileName = g_yyFileName ;
					  g_current->startLine = g_yyLineNr ;
					  g_current->startColumn = g_yyColNr;
					  g_current->name = removeRedundantWhiteSpace(g_current->name);
					  if (g_current->name.isEmpty() && !g_isTypedef) // anonymous compound
					  {
					    if (g_current->section==Entry::NAMESPACE_SEC) // allow reopening of anonymous namespaces
					    {
					      if (Config_getBool(EXTRACT_ANON_NSPACES)) // use visible name
					      {
					        g_current->name="anonymous_namespace{"+stripPath(g_current->fileName)+"}";
					      }
					      else // use invisible name
					      {
					        g_current->name.sprintf("@%d",g_anonNSCount);
					      }
					    }
					    else
					    {
					      g_current->name.sprintf("@%d",g_anonCount++);
					    }
					  }
					  g_curlyCount=0;
					  if (g_current_root && // not a nested struct g_inside an @interface section
					      !(g_current_root->spec & Entry::Interface) &&
					      ((g_current->spec & (Entry::Interface | Entry::Protocol | Entry::Category) ||
					        g_current->section==Entry::OBJCIMPL_SEC)
					      ) &&
					      g_insideObjC
					     )
					  { // ObjC body that ends with @end
					    BEGIN( ReadBodyIntf );
					  }
					  else if (g_current->section==Entry::NAMESPACE_SEC)
					  { // namespace body
					    BEGIN( ReadNSBody );
					  }
					  else
					  { // class body
					    BEGIN( ReadBody ) ;
					  }
					}
<BasesProt>"virtual"{BN}+               { lineCount(); g_baseVirt = Virtual; }
<BasesProt>"public"{BN}+                { lineCount(); g_baseProt = Public; }
<BasesProt>"protected"{BN}+             { lineCount(); g_baseProt = Protected; }
<BasesProt>"internal"{BN}+              { if (!g_insideCli) REJECT ; lineCount(); g_baseProt = Package; }
<BasesProt>"private"{BN}+               { lineCount(); g_baseProt = Private; }
<BasesProt>{BN}				{ lineCount(); }
<BasesProt>.				{ unput(*yytext); BEGIN(Bases); }
<Bases>("\\")?({ID}"\\")*{ID}		{ // PHP namespace token, not sure if interspacing is allowed but it gives problems (see bug 640847)
                                          if (!g_insidePHP)
					  {
					    REJECT;
					  }
					  else // PHP base class of the form \Ns\Cl or Ns\Cl
					  {
  					    lineCount();
					    QCString bn=yytext;
					    bn = substitute(bn,"\\","::");
					    g_baseName += bn;
					    g_current->args += ' ';
					    g_current->args += yytext;
					  }
                                        }
<Bases>("::")?{BN}*({ID}{BN}*"::"{BN}*)*{ID}	{ 
  					  lineCount();
                                          QCString baseScope = yytext;
                                          if (g_insideCS && baseScope.stripWhiteSpace()=="where")
					  { 
					    // type constraint for a class
                                            g_current->typeConstr.clear();
					    g_current->typeConstr.push_back(Argument());
					    g_lastCSConstraint = YY_START;
					    BEGIN( CSConstraintName );
					  }
					  else
					  {
  					    g_baseName+=yytext;
					    g_current->args += ' ';
					    g_current->args += yytext;
					  }
					}
<Bases>{BN}*{ID}("."{ID})*		{ // Java style class
    					  QCString name = substitute(yytext,".","::");
					  g_baseName += name;
					  g_current->args += ' ';
					  g_current->args += name;
    					}
<ClassVar,Bases>\n/{BN}*[^{, \t\n]	{
                                          if (!g_insideObjC) 
					  {
					    REJECT;
					  }
					  else
					  {
                                            lineCount();
					    unput('{');
					  }
					}
<ClassVar,Bases>"@end"			{ // empty ObjC interface
  					  unput('d'); // insert fake body: {}@end
  					  unput('n'); 
  					  unput('e'); 
  					  unput('@'); 
  					  unput('}'); 
  					  unput('{');
					}
<ClassVar>"<"   	                { g_current->name += *yytext;
  					  g_sharpCount=1; 
					  g_roundCount=0;
					  g_lastSkipSharpContext = YY_START;
					  g_specName = &g_current->name;
					  BEGIN ( Specialization );
					}
<Bases>{BN}*"<"                         {
                                          lineCount();
  					  g_sharpCount=1; 
					  g_roundCount=0;
					  g_lastSkipSharpContext = YY_START;
					  if (g_insideObjC) // start of protocol list
					  {
					    unput(',');
					  }
					  else // template specialization
					  {
					    //if (g_insideCS) // generic
					    //{
					    //  g_baseName+="-g";
					    //}
                                            g_templateStr = yytext;
					    g_specName = &g_templateStr;
					    BEGIN ( Specialization );
					  }
					}
<Specialization>"<"			{ *g_specName += *yytext;
  					  if (g_roundCount==0) g_sharpCount++;
  					}
<Specialization>">"			{
  					  *g_specName += *yytext;
  					  if (g_roundCount==0 && --g_sharpCount<=0)
					  {
					    g_baseName+=removeRedundantWhiteSpace(*g_specName);
					    BEGIN(g_lastSkipSharpContext);
					  }
  					}
<Specialization>{BN}+			{ lineCount(); *g_specName +=' '; }
<Specialization>"<<"			{ *g_specName += yytext; }
<Specialization>">>"/{B}*"::"		{ // M$ C++ extension to allow >> to close a template...
  					  unput('>');
  					  unput(' ');
  					  unput('>');
                                        }
<Specialization>">>"			{
                                          if (g_insideCS) // for C# >> ends a nested template
					  {
					    REJECT;
					  }
					  else // for C++ >> is a bitshift 
					       // operator and > > would end 
					       // a nested template.
					       // We require the bitshift to be enclosed in braces.
					       // See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1757.html
					  {
					    if (g_roundCount>0)
					    {
                                              *g_specName += yytext; 
					    }
					    else
					    {
  					      unput('>');
  					      unput(' ');
  					      unput('>');
					    }
					  }
                                        }
<Specialization>"typename"{BN}+		{ lineCount(); }
<Specialization>"("			{ *g_specName += *yytext; g_roundCount++; }
<Specialization>")"			{ *g_specName += *yytext; g_roundCount--; }

<Specialization>"\\\\"			{ *g_specName += *yytext;}
<Specialization>"\\'"			{ *g_specName += *yytext;}
<Specialization>"\\\""			{ *g_specName += *yytext;}
<Specialization>"'"			{ *g_specName += *yytext;BEGIN(SpecializationSingleQuote);}
<Specialization>"\""			{ *g_specName += *yytext;BEGIN(SpecializationDoubleQuote);}
<SpecializationSingleQuote,SpecializationDoubleQuote>"\\\\"       { *g_specName += *yytext;}
<SpecializationSingleQuote>"\\'"        { *g_specName += *yytext;}
<SpecializationSingleQuote>"'"          { *g_specName += *yytext; BEGIN(Specialization);}
<SpecializationDoubleQuote>"\\\""       { *g_specName += *yytext;}
<SpecializationDoubleQuote>"\""         { *g_specName += *yytext; BEGIN(Specialization);}
<SpecializationSingleQuote,SpecializationDoubleQuote>.          { *g_specName += *yytext;}

<Specialization>.			{
  					  *g_specName += *yytext;
  					}
<SkipRound>"("				{ ++g_roundCount; }
<SkipRound>")"				{ if (--g_roundCount<0)
					    BEGIN ( g_lastSkipRoundContext );
					}
<SkipRound>\"				{
  					  g_lastStringContext=SkipRound;
					  BEGIN(SkipString);
  					}
<Bases>","|(">"({BN}*"{")?)|({BN}+"implements"{BN}*)	{ lineCount();
                                          if (g_insideProtocolList)
					  {
					    g_baseName+="-p";
					  }
					  else
					  {
                                            g_current->args += ',' ; 
					  }
					  g_current->name = removeRedundantWhiteSpace(g_current->name);
  					  if (!g_baseName.isEmpty())
					  {
  					    g_current->extends.push_back(
					      BaseInfo(g_baseName,g_baseProt,g_baseVirt)
					    );
					  }
					  if ((g_current->spec & (Entry::Interface|Entry::Struct)) || 
					      g_insideJava || g_insidePHP || g_insideCS || 
					      g_insideD || g_insideObjC || g_insideIDL || g_insideSlice)
					  {
					    g_baseProt=Public;
					  }
					  else
					  {
					    g_baseProt=Private;
					  }
					  g_baseVirt=Normal;
					  g_baseName.resize(0);
                                          if (*yytext=='>')
					  { // end of a ObjC protocol list
  					    g_insideProtocolList=FALSE;
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
					    if (*yytext==',' && g_insideObjC) // Begin of protocol list
					    {
  					      g_insideProtocolList=TRUE;
					    }
					    BEGIN(BasesProt);
					  }
					}
<Bases>{B}*"{"{B}*			{ g_current->fileName = g_yyFileName ;
					  g_current->startLine = g_yyLineNr ;
					  g_current->startColumn = g_yyColNr;
					  g_current->name = removeRedundantWhiteSpace(g_current->name);
  					  if (!g_baseName.isEmpty())
  					    g_current->extends.push_back(
					       BaseInfo(g_baseName,g_baseProt,g_baseVirt)
					    );
					  g_curlyCount=0;
					  if (g_insideObjC)
					  {
					    BEGIN( ReadBodyIntf );
					  }
					  else
					  {
					    BEGIN( ReadBody ) ;
					  }
					}
<SkipUnionSwitch>{B}*"("		{
  					  g_roundCount++;
  					}
<SkipUnionSwitch>")"			{
  					  if (--g_roundCount==0)
					  {
					    BEGIN(ClassVar);
					  }
  					}
<SkipUnionSwitch>\n			{ lineCount(); }
<SkipUnionSwitch>.			
<Comment>{BN}+				{ g_current->program += yytext ;
					  lineCount() ;
					}
<Comment>"/*"				{ g_current->program += yytext ; } 
<Comment>"//"				{ g_current->program += yytext ; }
<Comment>{CMD}("code"|"verbatim")	{
                                          g_insideCode=TRUE;
  					  g_current->program += yytext ;
  					}
<Comment>{CMD}("endcode"|"endverbatim")	{
                                          g_insideCode=FALSE;
  					  g_current->program += yytext ;
  					}
<Comment>[^ \.\t\r\n\/\*]+		{ g_current->program += yytext ; }
<Comment>"*/"				{ g_current->program += yytext ;
					  if (!g_insideCode) BEGIN( g_lastContext ) ;
					}
<Comment>.				{ g_current->program += *yytext ; }

<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,SkipC11Inits,SkipC11Attribute,Bases,OldStyleArgs>("//"{B}*)?"/*!" { 
  					  //printf("Start doc block at %d\n",g_yyLineNr);
					  if (!g_current->doc.isEmpty())
					  {
					    g_current->doc+="\n\n";
					  }
					  else
					  {
					    g_current->docLine = g_yyLineNr;
					    g_current->docFile = g_yyFileName;
					  }

					  g_lastDocContext = YY_START;
					  if (g_current_root->section & Entry::SCOPE_MASK)
                                          {
					    g_current->inside = g_current_root->name+"::";
                                          }
					  g_docBlockContext   = YY_START;
					  g_docBlockInBody    = YY_START==SkipCurly;
					  g_docBlockAutoBrief = Config_getBool(QT_AUTOBRIEF);

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;

					  if (g_docBlockAutoBrief)
					  {
					    g_current->briefLine = g_yyLineNr;
					    g_current->briefFile = g_yyFileName;
					  }
					  startCommentBlock(FALSE);
					  BEGIN( DocBlock );
					}
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,Bases,OldStyleArgs>"/**"[*]+{BL} {
					  bool javadocBanner = Config_getBool(JAVADOC_BANNER);
                                          lineCount();

					  if( javadocBanner )
                                          {
                                            g_lastDocContext = YY_START;

                                            //printf("Found comment banner at %s:%d\n",g_yyFileName,g_yyLineNr);
                                            if (g_current_root->section & Entry::SCOPE_MASK)
                                            {
                                              g_current->inside = g_current_root->name+"::";
                                            }
                                            g_current->docLine = g_yyLineNr;
                                            g_current->docFile = g_yyFileName;
                                            g_docBlockContext = YY_START;
                                            g_docBlockInBody  = YY_START==SkipCurly;
                                            bool javadocAutoBrief = Config_getBool(JAVADOC_AUTOBRIEF);
                                            g_docBlockAutoBrief = javadocAutoBrief;

                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,g_column));
                                            g_docBlock=indent;

                                            if (g_docBlockAutoBrief)
                                            {
                                              g_current->briefLine = g_yyLineNr;
                                              g_current->briefFile = g_yyFileName;
                                            }
                                            startCommentBlock(FALSE);
                                            BEGIN( DocBlock );
                                          }
                                          else
                                          {
					    g_current->program += yytext ;
					    g_lastContext = YY_START ;
					    BEGIN( Comment ) ;
					  }
					}
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,Bases,OldStyleArgs>("//"{B}*)?"/**"/[^/*] {
					  g_lastDocContext = YY_START;

					  //printf("Found comment block at %s:%d\n",g_yyFileName,g_yyLineNr);
					  if (g_current_root->section & Entry::SCOPE_MASK)
                                          {
					    g_current->inside = g_current_root->name+"::";
                                          }
					  g_current->docLine = g_yyLineNr;
					  g_current->docFile = g_yyFileName;
					  g_docBlockContext = YY_START;
					  g_docBlockInBody  = YY_START==SkipCurly;
                                          bool javadocAutoBrief = Config_getBool(JAVADOC_AUTOBRIEF);
					  g_docBlockAutoBrief = javadocAutoBrief;

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;

					  if (g_docBlockAutoBrief)
					  {
					    g_current->briefLine = g_yyLineNr;
					    g_current->briefFile = g_yyFileName;
					  }
					  startCommentBlock(FALSE);
					  BEGIN( DocBlock );
  					}
<FindMembers,FindFields,MemberSpec,SkipCurly,FuncQual,Operator,ClassVar,Bases,OldStyleArgs>"//!" { 
					  g_lastDocContext = YY_START;
					  if (g_current_root->section & Entry::SCOPE_MASK)
                                          {
					    g_current->inside = g_current_root->name+"::";
                                          }
					  g_docBlockContext   = YY_START;
					  g_docBlockInBody    = YY_START==SkipCurly;
					  g_docBlockAutoBrief = FALSE;

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;

					  startCommentBlock(g_current->brief.isEmpty());
					  BEGIN( DocLine );
					}
<FindMembers,FindFields,MemberSpec,SkipCurly,FuncQual,Operator,ClassVar,Bases,OldStyleArgs>"///"/[^/] { 
					  g_lastDocContext = YY_START;
					  if (g_current_root->section & Entry::SCOPE_MASK)
                                          {
					    g_current->inside = g_current_root->name+"::";
                                          }
					  g_docBlockContext   = YY_START;
					  g_docBlockInBody    = YY_START==SkipCurly;
					  g_docBlockAutoBrief = FALSE;
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock=indent;
					  startCommentBlock(g_current->brief.isEmpty());
					  BEGIN( DocLine );
					}
<FindMembers>"extern"{BN}*"\"C"("++")?"\""{BN}*("{")?  {
					  lineCount();
					  g_externC=TRUE;
					}
<FindMembers>"{"			{
                                          if (g_externC) 
					  {
					    g_externC=FALSE;
					  }
                                          else if (g_insideCS &&    
					      !g_current->name.isEmpty() && 
					      !g_current->type.isEmpty())
					  {
					    if (containsWord(g_current->type,"event")) // event
					    {
  					      g_current->mtype = g_mtype = Event;
					    }
					    else // property
					    {				
  					      g_current->mtype = g_mtype = Property;
					    }
					    g_current->bodyLine = g_yyLineNr;
					    g_curlyCount=0;
					    BEGIN( CSAccessorDecl );
					  }
					  else if (g_insideIDL && (g_current->spec & Entry::Attribute))
					  {
					    // UNO IDL: attributes may have setter and getter
					    // exception specifications
					    g_current->exception = " {";
					    BEGIN(UNOIDLAttributeBlock);
					  }
					  else
					  {
					    if ((g_insideJava || g_insideCS || g_insideD) &&
						g_current->name.isEmpty()
					       )
					    {
					      // static Java initializer
					      g_needsSemi = FALSE;
					      if (g_current->stat)
					      {
						g_current->name="[static initializer]";
						g_current->type.resize(0);
					      }
					      else
					      {
						g_current->name="[instance initializer]";
					      }
                                              unput(*yytext); 
					      BEGIN( Function );
					    }
					    else
					    {
					      // pre C++11 code -> ignore the initializer
                                              //g_needsSemi = TRUE;
					      //g_current->type.resize(0);
					      //g_current->name.resize(0);
					      //g_current->args.resize(0);
					      //g_current->argList.clear();
					      //g_curlyCount=0;
					      //BEGIN( SkipCurlyBlock );

                                              // C++11 style initializer list
  					      g_current->bodyLine = g_yyLineNr;
                                              g_current->initializer = yytext;
					      g_lastInitializerContext = YY_START;
					      g_initBracketCount=1;
					      BEGIN(ReadInitializer);
					    }
					  }
  					}
<CSAccessorDecl>"{"			{ g_curlyCount++; }
<CSAccessorDecl>"}"{B}*"="		{
					  // fall back to next rule if it's not the right bracket
					  if (g_curlyCount != 0) REJECT;
					  g_current->initializer = "=";
					  g_current->endBodyLine=g_yyLineNr;
					  g_lastInitializerContext = FindMembers;
					  BEGIN(ReadInitializer);
					}
<CSAccessorDecl>"}"			{ 
                                          if (g_curlyCount) 
                                          {
                                            g_curlyCount--; 
					  }
					  else
					  {
  					    g_mtype = Method;
                                            g_virt = Normal;
					    // not really important, but while we are at it
					    g_current->endBodyLine=g_yyLineNr;
                                            unput(';');
					    BEGIN(FindMembers);
					  }
                                        }
<CSAccessorDecl>"private "{BN}*"set"	{ if (g_curlyCount==0) g_current->spec |= Entry::PrivateSettable;   }
<CSAccessorDecl>"protected "{BN}*"set"	{ if (g_curlyCount==0) g_current->spec |= Entry::ProtectedSettable; }
<CSAccessorDecl>"private "{BN}*"get"	{ if (g_curlyCount==0) g_current->spec |= Entry::PrivateGettable;  	}
<CSAccessorDecl>"protected "{BN}*"get"	{ if (g_curlyCount==0) g_current->spec |= Entry::ProtectedGettable; }
<CSAccessorDecl>"set"			{ if (g_curlyCount==0) g_current->spec |= Entry::Settable;  }
<CSAccessorDecl>"get"			{ if (g_curlyCount==0) g_current->spec |= Entry::Gettable;  }
<CSAccessorDecl>"add"			{ if (g_curlyCount==0) g_current->spec |= Entry::Addable;   }
<CSAccessorDecl>"remove"		{ if (g_curlyCount==0) g_current->spec |= Entry::Removable; }
<CSAccessorDecl>"raise"			{ if (g_curlyCount==0) g_current->spec |= Entry::Raisable;  }
<CSAccessorDecl>"\""			{ BEGIN(CSString);}
<CSAccessorDecl>"."			{}
<CSAccessorDecl>\n			{ lineCount(); }
<CSString>"\""				{ BEGIN(CSAccessorDecl);}
<CSString>"//"				{} // Otherwise the rule <*>"//" will kick in
<CSString>"/*"				{} // Otherwise the rule <*>"/*" will kick in
<CSString>\n				{ lineCount(); }
<CSString>"."				{}

 /* ---- Slice-specific rules ------ */

<SliceSequence>{SCOPENAME}              {
                                          if (g_current->spec&Entry::Local)
                                          {
                                            g_current->type = "local ";
                                          }
                                          g_current->type += "sequence<";
                                          g_current->type += yytext;
                                          g_current->type += ">";
                                        }

<SliceSequence>{BN}*">"{BN}*            {
                                          lineCount();
                                          BEGIN(SliceSequenceName);
                                        }

<SliceSequenceName>{ID}{BN}*            {
                                          lineCount();
                                          g_current->name = yytext ;
                                          g_current->name = g_current->name.stripWhiteSpace();
                                        }

<SliceSequenceName>";"                  {
                                          g_current->section = Entry::VARIABLE_SEC;
                                          g_current_root->moveToSubEntryAndRefresh(g_current);
                                          initEntry();
                                          BEGIN(FindMembers);
                                        }

<SliceDictionary>{SCOPENAME}{BN}*","{BN}*{SCOPENAME} {
                                          lineCount();
                                          if (g_current->spec&Entry::Local)
                                          {
                                            g_current->type = "local ";
                                          }
                                          g_current->type += "dictionary<";
                                          g_current->type += yytext;
                                          g_current->type += ">";
                                          g_current->type = g_current->type.simplifyWhiteSpace();
                                        }

<SliceDictionary>{BN}*">"{BN}*          {
                                          lineCount();
                                          BEGIN(SliceDictionaryName);
                                        }

<SliceDictionaryName>{ID}{BN}*          {
                                          lineCount();
                                          g_current->name = yytext ;
                                          g_current->name = g_current->name.stripWhiteSpace();
                                        }

<SliceDictionaryName>";"                {
                                          g_current->section = Entry::VARIABLE_SEC;
                                          g_current_root->moveToSubEntryAndRefresh(g_current);
                                          initEntry();
                                          BEGIN(FindMembers);
                                        }

 /**********************************************************************************/
 /******************** Documentation block related rules ***************************/
 /**********************************************************************************/

 /* ---- Single line comments ------ */
<DocLine>[^\n]*"\n"[ \t]*"//"[/!][<]?   { // continuation of multiline C++-style comment
				           g_docBlock+=yytext;
                                           int markerLen = yytext[yyleng-1]=='<' ? 4 : 3;
          		                   g_docBlock.resize(g_docBlock.length() - markerLen);
                                           lineCount();
                                        }
<DocLine>{B}*"///"[/]+{B}*/"\n"		{ // ignore marker line (see bug700345)
					  handleCommentBlock(g_docBlock.data(),g_current->brief.isEmpty());
					  BEGIN( g_docBlockContext );
  					}
<DocLine>[^\n]*/"\n"{B}*"//"[!/]{B}*{CMD}"}"	{ // next line is an end group marker, see bug 752712
  					  g_docBlock+=yytext;
					  handleCommentBlock(g_docBlock.data(),g_current->brief.isEmpty());
					  BEGIN( g_docBlockContext );
  					}
<DocLine>[^\n]*/"\n"			{ // whole line
  					  g_docBlock+=yytext;
					  handleCommentBlock(g_docBlock.data(),g_current->brief.isEmpty());
					  BEGIN( g_docBlockContext );
  					}

 /* ---- Comments blocks ------ */

<DocBlock>"*"*"*/"			{ // end of comment block
  				          handleCommentBlock(g_docBlock.data(),FALSE);
					  BEGIN(g_docBlockContext);
  					}
<DocBlock>^{B}*"*"+/[^/]		{ 

                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock+=indent;
  					}
<DocBlock>^{B}*("//")?{B}*"*"+/[^//a-z_A-Z0-9*]	{ // start of a comment line
                                          QCString indent;
                                          indent.fill(' ',computeIndent(yytext,g_column));
                                          g_docBlock+=indent;
  					}
<DocBlock>^{B}*("//"){B}*		{ // strip embedded C++ comments if at the start of a line
  					}
<DocBlock>"//"				{ // slashes in the middle of a comment block
  					  g_docBlock+=yytext;
  					}
<DocBlock>"/*"				{ // start of a new comment in the 
                                          // middle of a comment block
  					  g_docBlock+=yytext;
  					}
<DocBlock>({CMD}{CMD}){ID}/[^a-z_A-Z0-9] { // escaped command
  					  g_docBlock+=yytext;
  					}
<DocBlock>{CMD}("f$"|"f["|"f{")	        {
                                          g_docBlock+=yytext;
				          g_docBlockName=&yytext[1];
					  if (g_docBlockName.at(1)=='{')
					  {
				            g_docBlockName.at(1)='}';
					  }
                                          g_fencedSize=0;
                                          g_nestedComment=FALSE;
  					  BEGIN(DocCopyBlock);
                                        }
<DocBlock>{B}*"<"{PRE}">"		{
                                          g_docBlock+=yytext;
				          g_docBlockName="<pre>";
                                          g_fencedSize=0;
                                          g_nestedComment=FALSE;
  					  BEGIN(DocCopyBlock);
  					}
<DocBlock>{CMD}("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"manonly"|"dot"|"code")/[^a-z_A-Z0-9\-]	{ // verbatim command (which could contain nested comments!)
                                          g_docBlock+=yytext;
				          g_docBlockName=&yytext[1];
                                          g_fencedSize=0;
                                          g_nestedComment=FALSE;
  					  BEGIN(DocCopyBlock);
  					}
<DocBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]*                {
                                          g_docBlock+=substitute(yytext,"*"," ");
                                          g_docBlockName="~~~";
                                          g_fencedSize=yyleng;
                                          g_nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*                {
                                          g_docBlock+=substitute(yytext,"*"," ");
                                          g_docBlockName="```";
                                          g_fencedSize=yyleng;
                                          g_nestedComment=FALSE;
                                          BEGIN(DocCopyBlock);
                                        }
<DocBlock>{B}*"<code>"                  {
  					  if (g_insideCS)
					  {
					    g_docBlock+=yytext;
					    g_docBlockName="<code>";
                                            g_nestedComment=FALSE;
					    BEGIN(DocCopyBlock);
					  }
					  else
					  {
					    REJECT;
					  }
  					}
<DocBlock>[^@*~\/\\\n]+			{ // any character that isn't special
  					  g_docBlock+=yytext;
  					}
<DocBlock>\n				{ // newline
                                          lineCount();
					  g_docBlock+=*yytext;
  					}
<DocBlock>.				{ // command block
					  g_docBlock+=*yytext;
  					}

 /* ---- Copy verbatim sections ------ */

<DocCopyBlock>"</"{PRE}">"		{ // end of a <pre> block
  					  g_docBlock+=yytext;
					  if (g_docBlockName=="<pre>")
					  {
  					    BEGIN(DocBlock);
					  }
  					}
<DocCopyBlock>"</"{CODE}">"		{ // end of a <code> block
  					  g_docBlock+=yytext;
					  if (g_docBlockName=="<code>")
					  {
  					    BEGIN(DocBlock);
					  }
  					}
<DocCopyBlock>[\\@]("f$"|"f]"|"f}")     {
  					  g_docBlock+=yytext;
					  BEGIN(DocBlock);
  					}
<DocCopyBlock>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endmanonly"|"enddot"|"endcode")/[^a-z_A-Z0-9] { // end of verbatim block
  					  g_docBlock+=yytext;
					  if (&yytext[4]==g_docBlockName)
					  {
  					    BEGIN(DocBlock);
					  }
  					}
<DocCopyBlock>^{B}*"*"+/{BN}+		{ // start of a comment line
  					  if (g_docBlockName=="verbatim")
					  {
					    REJECT;
					  }
  					  else if (g_docBlockName=="code")
					  {
					    REJECT;
					  }
                                          else
                                          {
                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,0));
                                            g_docBlock+=indent;
                                          }
  					}
<DocCopyBlock>^{B}*"*"+/{B}+"*"{BN}*	{ // start of a comment line with two *'s
  					  if (g_docBlockName=="code")
                                          {
                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,0));
                                            g_docBlock+=indent;
                                          }
                                          else
					  {
					    REJECT;
					  }
  					}
<DocCopyBlock>^{B}*"*"+/({ID}|"(")	{ // Assume *var or *(... is part of source code (see bug723516)
					  if (g_docBlockName=="code")
                                          {
                                            QCString indent;
                                            indent.fill(' ',computeIndent(yytext,-1));
                                            g_docBlock+=indent+"*";
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<DocCopyBlock>^{B}*"*"+/{BN}*		{ // start of a comment line with one *
  					  if (g_docBlockName=="code")
                                          {
                                            QCString indent;
                                            if (g_nestedComment) // keep * it is part of the code
                                            {
                                              indent.fill(' ',computeIndent(yytext,-1));
                                              g_docBlock+=indent+"*";
                                            }
                                            else // remove * it is part of the comment block
                                            {
                                              indent.fill(' ',computeIndent(yytext,0));
                                              g_docBlock+=indent;
                                            }
                                          }
                                          else
					  {
					    REJECT;
					  }
  					}
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                                          g_docBlock+=substitute(yytext,"*"," ");
                                          if (g_fencedSize==yyleng)
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
                                          g_docBlock+=substitute(yytext,"*"," ");
                                          if (g_fencedSize==yyleng)
                                          {
                                            BEGIN(DocBlock);
                                          }
                                        }
<DocCopyBlock>[^\<@/*\]~\$\\\n]+	{ // any character that is not special
  					  g_docBlock+=yytext;
  					}
<DocCopyBlock>"/*"|"*/"|"//"		{
                                          if (yytext[1]=='*')
                                          {
                                            g_nestedComment=TRUE;
                                          }
                                          else if (yytext[0]=='*')
                                          {
                                            g_nestedComment=FALSE;
                                          }
  					  g_docBlock+=yytext;
					}
<DocCopyBlock>\n			{ // newline
  					  g_docBlock+=*yytext;
                                          lineCount();
  					}
<DocCopyBlock>.				{ // any other character
  					  g_docBlock+=*yytext;
  					}
<DocCopyBlock><<EOF>>			{
  					  warn(g_yyFileName,g_yyLineNr,
					      "reached end of file while g_inside a '%s' block!\n"
					      "The command that should end the block seems to be missing!\n",
					      g_docBlockName.data());
  					  yyterminate();
  					}


    /* ------------- Prototype parser -------------- */

<Prototype>"operator"{B}*"("{B}*")"	{
  					  g_current->name+=yytext;
  					}
<Prototype>"("			       {
  					  g_current->args+=*yytext;
					  g_currentArgumentContext = PrototypeQual;
					  g_fullArgString = g_current->args.copy();
					  g_copyArgString = &g_current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
<Prototype>"("({ID}"::")*({B}*[&*])+	{
  					  g_current->type+=g_current->name+yytext;
					  g_current->name.resize(0);
  					  BEGIN( PrototypePtr );
  					}
<PrototypePtr>{SCOPENAME}		{
  					  g_current->name+=yytext;
  					}
<PrototypePtr>"("			{
  					  g_current->args+=*yytext;
					  g_currentArgumentContext = PrototypeQual;
					  g_fullArgString = g_current->args.copy();
					  g_copyArgString = &g_current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
<PrototypePtr>")"			{
  					  g_current->type+=')';
  					  BEGIN( Prototype );
  					}
<PrototypePtr>.				{
  					  g_current->name+=yytext;
  					}
<PrototypeQual>"{"			{
  					  BEGIN( PrototypeSkipLine);
  					}
<PrototypeQual>{B}*"const"{B}*    	{ 
  					  g_current->args += " const "; 
					  g_current->argList.constSpecifier=TRUE;
					}
<PrototypeQual>{B}*"volatile"{B}* 	{ 
  					  g_current->args += " volatile "; 
					  g_current->argList.volatileSpecifier=TRUE;
					}
<PrototypeQual>{B}*"="{B}*"0"{B}*	{ 
					  g_current->args += " = 0"; 
					  g_current->virt = Pure; 
					  g_current->argList.pureSpecifier=TRUE;
					}
<PrototypeQual>"throw"{B}*"("	        {
  					  g_current->exception = "throw(";
					  BEGIN(PrototypeExc);
  					}
<PrototypeExc>")"			{
  					  g_current->exception += ')';
					  BEGIN(PrototypeQual);
  					}
<PrototypeExc>.			        {
  					  g_current->exception += *yytext;
  					}
<PrototypeQual>.			{
  					  g_current->args += *yytext;
  					}
<Prototype>.	                        {
  					  g_current->name += *yytext;
  					}
<PrototypeSkipLine>.			{
  					}




<SkipCxxComment>.*"\\\n"		{  // line continuation
  					  if (g_insideCS) 
					  {
					    REJECT;
					  }
					  else 
					  {
                                            lineCount();
					  }
  					}
<SkipCxxComment>.*/\n			{ 
					  BEGIN( g_lastCContext ) ;
					}
<SkipComment>[^\*\n]+

  /* ------------ Generic rules -------------- */

<*>"[["                                 { // C++11 attribute
                                          if (!g_insideCpp) REJECT;
                                          if (YY_START == CopyGString || YY_START == CopyGString) REJECT;
                                          g_lastC11AttributeContext = YY_START;
                                          BEGIN( SkipC11Attribute );
                                        }

<*>\n					{ lineCount(); }
<*>\"					{
					  if (g_insideIDL && g_insideCppQuote)
					  {
					    BEGIN(EndCppQuote);
					  }
					}
<*>"#"				        {	
  					  if (!g_insidePHP)
  					    REJECT;
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>\'					{
  					  if (g_insidePHP)
					  {
  					    g_lastStringContext=YY_START;
					    BEGIN(SkipPHPString);
					  }
  					}
<*>\"					{
  					  if (g_insidePHP)
					  {
  					    g_lastStringContext=YY_START;
					    BEGIN(SkipString);
					  }
  					}
<*>\?                                   {
  					  if (g_insideCS && (YY_START != SkipRound)) 
					  {
					    if (g_current->type.isEmpty())
					    {
					      if (g_current->name.isEmpty())
					        g_current->name="?";
					      else
					        g_current->name+="?";
					    }
					    else
					    {
					      g_current->type+="?";
					    }
					  }
  					}
<*>.
<SkipComment>"//"|"/*"
<*>"/*"					{ g_lastCContext = YY_START ;
					  BEGIN( SkipComment ) ;
					}
<SkipComment>{B}*"*/"			{ BEGIN( g_lastCContext ) ; }
<*>"//"				        {	
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
%%

//----------------------------------------------------------------------------

static void startCommentBlock(bool brief)
{
  if (brief)
  {
    g_current->briefFile = g_yyFileName;
    g_current->briefLine = g_yyLineNr;
  }
  else
  {
    g_current->docFile = g_yyFileName;
    g_current->docLine = g_yyLineNr;
  }
}
  
//----------------------------------------------------------------------------

static void newEntry()
{
  if (g_tempEntry==0) // if temp entry is not 0, it holds g_current, 
                    // and g_current is actually replaced by g_previous which was 
                    // already added to g_current_root, so we should not add it again 
                    // (see bug723314)
  {
    g_previous = g_current.get();
    g_current_root->moveToSubEntryAndRefresh(g_current);
  }
  else
  {
    g_previous = g_current.get();
    g_tempEntry.swap(g_current);
    g_tempEntry.reset();
  }
  initEntry();
}

static void handleCommentBlock(const QCString &doc,bool brief)
{
  bool hideInBodyDocs = Config_getBool(HIDE_IN_BODY_DOCS);
  if (g_docBlockInBody && hideInBodyDocs) return;
  //printf("parseCommentBlock [%s] brief=%d\n",doc.data(),brief);
  int lineNr = brief ? g_current->briefLine : g_current->docLine;   // line of block start

  // fill in inbodyFile && inbodyLine the first time, see bug 633891
  Entry *docEntry = g_docBlockInBody && g_previous ? g_previous : g_current.get();
  if (g_docBlockInBody && docEntry && docEntry->inbodyLine==-1)
  {
    docEntry->inbodyFile = g_yyFileName;
    docEntry->inbodyLine = lineNr;
  }

  int position=0;
  bool needsEntry=FALSE;
  QCString processedDoc = preprocessCommentBlock(stripIndentation(doc),g_yyFileName,lineNr);
  while (parseCommentBlock(
	g_thisParser,
	g_docBlockInBody && g_previous ? g_previous : g_current.get(),
	processedDoc, // text
	g_yyFileName,   // file
	lineNr,       // line of block start
	g_docBlockInBody ? FALSE : brief,               // isBrief
	g_docBlockInBody ? FALSE : g_docBlockAutoBrief,   // isJavaDocStyle
	g_docBlockInBody,                               // isInBody
	g_protection,
        position,
        needsEntry
        )
     )
  {
    //printf("parseCommentBlock position=%d [%s]\n",position,doc.data()+position);
    if (needsEntry) 
    {
      QCString docFile = g_current->docFile;
      newEntry();
      g_current->docFile = docFile;
      g_current->docLine = lineNr;
    }
  }
  if (needsEntry)
  {
    newEntry();
  }

  if (g_docBlockTerm)
  {
    unput(g_docBlockTerm);
    g_docBlockTerm=0;
  }
}

static void handleParametersCommentBlocks(ArgumentList &al)
{
  //printf(">>>>>>> handleParametersCommentBlocks()\n");
  for (Argument &a : al)
  {
    //printf("    Param %s docs=%s\n",a->name.data(),a->docs.data());
    if (!a.docs.isEmpty())
    {
      if  (!a.name && a.type == "...") a.name= "...";
      int position=0;
      bool needsEntry;

      // save context
      QCString orgDoc   = g_current->doc;
      QCString orgBrief = g_current->brief;
      int orgDocLine    = g_current->docLine;
      int orgBriefLine  = g_current->briefLine;

      g_current->doc.resize(0);
      g_current->brief.resize(0);

      //printf("handleParametersCommentBlock [%s]\n",doc.data());
      while (parseCommentBlock(
	     g_thisParser,
	     g_current.get(),
	     a.docs,             // text
	     g_yyFileName,         // file
	     g_current->docLine,   // line of block start
	     FALSE, 
	     FALSE,
	     FALSE,
	     g_protection,
	     position,
	     needsEntry
	    )
	  ) 
      {
	//printf("handleParametersCommentBlock position=%d [%s]\n",position,doc.data()+position);
	if (needsEntry) newEntry();
      }
      if (needsEntry)
      {
	newEntry();
      }
      a.docs = g_current->doc;

      // restore context
      g_current->doc       = orgDoc;
      g_current->brief     = orgBrief;
      g_current->docLine   = orgDocLine;
      g_current->briefLine = orgBriefLine;
    }
  }
}


//----------------------------------------------------------------------------

static void parseCompounds(const std::unique_ptr<Entry> &rt)
{
  //printf("parseCompounds(%s)\n",rt->name.data());
  for (const auto &ce : rt->children())
  {
    if (!ce->program.isEmpty())
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  ce->name.data(),ce->program.data());
      // init scanner state
      g_padCount=0;
      //depthIf = 0;
      g_column=0;
      g_inputString = ce->program;
      g_inputPosition = 0;
      scannerYYrestart( scannerYYin ) ;
      if (ce->section==Entry::ENUM_SEC || (ce->spec&Entry::Enum))
	BEGIN( FindFields ) ;
      else
	BEGIN( FindMembers ) ;
      g_current_root = ce.get() ;
      g_yyFileName = ce->fileName;
      //setContext();
      g_yyLineNr = ce->startLine ;
      g_yyColNr = ce->startColumn ;
      g_insideObjC = ce->lang==SrcLangExt_ObjC;
      //printf("---> Inner block starts at line %d objC=%d\n",g_yyLineNr,g_insideObjC);
      g_current = std::make_unique<Entry>();
      g_stat = FALSE;
      initEntry();

      // deep copy group list from parent (see bug 727732)
      bool autoGroupNested = Config_getBool(GROUP_NESTED_COMPOUNDS);
      if (autoGroupNested && ce->section!=Entry::ENUM_SEC && !(ce->spec&Entry::Enum))
      {
        ce->groups = rt->groups;
      }

      int ni=ce->name.findRev("::"); if (ni==-1) ni=0; else ni+=2;
      // set default protection based on the compound type
      if( ce->section==Entry::CLASS_SEC ) // class
      {
        if (g_insidePHP || g_insideD || g_insideJS || g_insideIDL || g_insideSlice)
	{
          g_current->protection = g_protection = Public ; 
	}
	else if (g_insideJava)
	{
          g_current->protection = g_protection = (ce->spec & (Entry::Interface|Entry::Enum)) ?  Public : Package;
	}
	else if (ce->spec&(Entry::Interface | Entry::Ref | Entry::Value | Entry::Struct | Entry::Union))
	{
	  if (ce->lang==SrcLangExt_ObjC)
	  {
	    g_current->protection = g_protection = Protected ;
	  }
	  else
	  {
	    g_current->protection = g_protection = Public ;
	  }
	}
	else 
	{
          g_current->protection = g_protection = Private ;
	}
      }
      else if (ce->section == Entry::ENUM_SEC ) // enum
      {
	g_current->protection = g_protection = ce->protection;
      }
      else if (!ce->name.isEmpty() && ce->name.at(ni)=='@') // unnamed union or namespace
      {
	if (ce->section == Entry::NAMESPACE_SEC ) // unnamed namespace
	{
          g_current->stat = g_stat = TRUE;
	}
	g_current->protection = g_protection = ce->protection;
      }
      else // named struct, union, protocol, category
      {
	g_current->protection = g_protection = Public ;
      }
      g_mtype = Method;
      g_virt = Normal;
      //printf("name=%s g_current->stat=%d g_stat=%d\n",ce->name.data(),g_current->stat,g_stat);

      //memberGroupId = DOX_NOGROUP;
      //memberGroupRelates.resize(0);
      //memberGroupInside.resize(0);
      QCString name = ce->name;
      Doxygen::docGroup.enterCompound(g_yyFileName,g_yyLineNr,name);
      
      scannerYYlex() ;
      g_lexInit=TRUE;
      //forceEndGroup();

      Doxygen::docGroup.leaveCompound(g_yyFileName,g_yyLineNr,name);
      
      ce->program.resize(0);


      //if (depthIf>0)
      //{
      //	warn(g_yyFileName,g_yyLineNr,"Documentation block ended in the middle of a conditional section!");
      //}
    }
    parseCompounds(ce);
  }
}

//----------------------------------------------------------------------------

static void parseMain(const char *fileName,
                      const char *fileBuf,
                      const std::unique_ptr<Entry> &rt,
                      bool sameTranslationUnit,
                      QStrList & filesInSameTranslationUnit)
{
  initParser();

  g_inputString = fileBuf;
  g_inputPosition = 0;
  g_column = 0;

  //g_anonCount     = 0;  // don't reset per file
  //depthIf       = 0;
  g_protection    = Public;
  g_mtype         = Method;
  g_stat         = FALSE;
  g_virt          = Normal;
  g_current_root  = rt.get();
  g_yyLineNr= 1 ; 
  g_yyFileName = fileName;
  setContext();
  bool processWithClang = g_insideCpp || g_insideObjC;
  if (processWithClang)
  {
    if (!sameTranslationUnit) // new file
    {
      ClangParser::instance()->start(fileName,filesInSameTranslationUnit);
    }
    else
    {
      ClangParser::instance()->switchToFile(fileName);
    }
  }
  rt->lang = g_language;
  msg("Parsing file %s...\n",g_yyFileName.data());

  g_current_root  = rt.get() ;
  initParser();
  Doxygen::docGroup.enterFile(g_yyFileName,g_yyLineNr);
  g_current = std::make_unique<Entry>();
  //printf("g_current=%p g_current_root=%p\n",g_current,g_current_root);
  int sec=guessSection(g_yyFileName);
  if (sec)
  {
    g_current->name    = g_yyFileName;
    g_current->section = sec;
    g_current_root->moveToSubEntryAndRefresh(g_current);
  }
  g_current->reset();
  initEntry();
  scannerYYrestart( scannerYYin );
  if ( g_insidePHP )
  {
    BEGIN( FindMembersPHP );
  }
  else
  {
    BEGIN( FindMembers );
  }

  scannerYYlex();
  g_lexInit=TRUE;

  if (YY_START==Comment)
  {
    warn(g_yyFileName,g_yyLineNr,"File ended in the middle of a comment block! Perhaps a missing \\endcode?");
  }

  //forceEndGroup();
  Doxygen::docGroup.leaveFile(g_yyFileName,g_yyLineNr);

  rt->program.resize(0);

  parseCompounds(rt);

  g_anonNSCount++;

  // add additional entries that were created during processing
  for (auto &kv: g_outerScopeEntries)
  {
    //printf(">>> adding '%s' to scope '%s'\n",kv.second->name.data(),kv.first->name.data());
    kv.first->moveToSubEntryAndKeep(kv.second);
  }
  g_outerScopeEntries.clear();

}

//----------------------------------------------------------------------------

static void parsePrototype(const QCString &text)
{
  //printf("**** parsePrototype(%s) begin\n",text.data());
  if (text.isEmpty()) 
  {
    warn(g_yyFileName,g_yyLineNr,"Empty prototype found!");
    return;
  }
  if (!g_current) // nothing to store (see bug683516)
  {
    return;
  }

  const char *orgInputString;
  int orgInputPosition;
  YY_BUFFER_STATE orgState;
  
  // save scanner state
  orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(scannerYYin, YY_BUF_SIZE));
  orgInputString = g_inputString; 
  orgInputPosition = g_inputPosition;

  // set new string
  g_inputString = text;
  g_inputPosition = 0;
  g_column = 0;
  scannerYYrestart( scannerYYin );
  BEGIN(Prototype);
  scannerYYlex();
  g_lexInit=TRUE;

  g_current->name = g_current->name.stripWhiteSpace();
  if (g_current->section == Entry::MEMBERDOC_SEC && g_current->args.isEmpty())
    g_current->section = Entry::VARIABLEDOC_SEC;

  // restore original scanner state
  YY_BUFFER_STATE tmpState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(orgState);
  yy_delete_buffer(tmpState);
  g_inputString = orgInputString; 
  g_inputPosition = orgInputPosition;


  //printf("**** parsePrototype end\n");
}

void scanFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION)
  if (g_lexInit)
  {
    scannerYYlex_destroy();
  }
#endif
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

void CLanguageScanner::startTranslationUnit(const char *)
{
}

void CLanguageScanner::finishTranslationUnit()
{
  bool processWithClang = g_insideCpp || g_insideObjC;
  if (processWithClang)
  {
    ClangParser::instance()->finish();
  }
}

void CLanguageScanner::parseInput(const char *fileName,
                                  const char *fileBuf,
                                  const std::unique_ptr<Entry> &root,
                                  bool sameTranslationUnit,
                                  QStrList & filesInSameTranslationUnit)
{
  g_thisParser = this;

  printlex(yy_flex_debug, TRUE, __FILE__, fileName);

  ::parseMain(fileName,fileBuf,root,
              sameTranslationUnit,filesInSameTranslationUnit);

  printlex(yy_flex_debug, FALSE, __FILE__, fileName);
}


void CLanguageScanner::parseCode(CodeOutputInterface & codeOutIntf,
                   const char * scopeName,
                   const QCString & input,
                   SrcLangExt lang,
                   bool isExampleBlock,
                   const char * exampleName,
                   FileDef * fileDef,
                   int startLine,
                   int endLine,
                   bool inlineFragment,
		   const MemberDef *memberDef,
		   bool showLineNumbers,
                   const Definition *searchCtx,
                   bool collectXRefs
                  )
{
  g_codeScanner.parseCCode(codeOutIntf,scopeName,input,lang,isExampleBlock,exampleName,
               fileDef,startLine,endLine,inlineFragment,memberDef,
	       showLineNumbers,searchCtx,collectXRefs);
}

bool CLanguageScanner::needsPreprocessing(const QCString &extension) const
{
  QCString fe=extension.lower();
  SrcLangExt lang = getLanguageFromFileName(extension);
  return (SrcLangExt_Cpp == lang) ||
   !( fe==".java" || fe==".as"  || fe==".d"    || fe==".php" || 
      fe==".php4" || fe==".inc" || fe==".phtml"|| fe==".php5"
    );
}

void CLanguageScanner::resetCodeParserState()
{
  g_codeScanner.reset();
}

void CLanguageScanner::parsePrototype(const char *text)
{
  ::parsePrototype(text);
}

//----------------------------------------------------------------------------

#include "scanner.l.h"
