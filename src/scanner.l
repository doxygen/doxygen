/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
  
%{

/*
 *	includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "qtbc.h"
#include <qarray.h>
#include <qstack.h>
#include <qregexp.h>
#include <unistd.h>
  
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "index.h"
#include "defargs.h"
#include "language.h"
#include "outputlist.h"
#include "membergroup.h"
#include "reflist.h"
#include "code.h"

  
#define YY_NEVER_INTERACTIVE 1

/* -----------------------------------------------------------------
 *
 *	statics
 */
static const char *     inputString;
static int		inputPosition;
static int		lastContext;
static int		lastCContext;
static int              lastDocContext;
static int              lastDocRelContext;
static int              lastCPPContext;
static int              lastSkipSharpContext;
static int              lastSkipRoundContext;
static int              lastBriefContext;
static int              lastVerbState;
static int              lastStringContext;
static int              lastCurlyContext;
static int              lastRoundContext;
static int              lastSquareContext;
static int              lastCodeState;
static int              lastAfterDocContext;
static int              lastGroupContext;
static int              lastFormulaContext;
static int              lastAnchorContext;
static int              lastInitializerContext;
static int              lastClassTemplSpecContext;
static int              lastSkipHtmlCommentContext;
static int              lastIfContext;
static int              lastInternalDocContext;
static int              lastPreLineCtrlContext;
static int              nextDefContext;
static int              overloadContext;
static Protection	protection;
static Protection	baseProt;
static int		sharpCount   = 0 ;
static int		roundCount   = 0 ;
static int		curlyCount   = 0 ;
static int		squareCount  = 0 ;
static int              padCount     = 0 ;
static int              slStartContext = 0;
static QCString         slString;
static Entry*		current_root = 0 ;
static Entry*		global_root  = 0 ;
static Entry*		current      = 0 ;
static Entry*		previous     = 0 ;
static Entry*		tempEntry    = 0 ;
static int		yyLineNr     = 1 ;
static int              anonCount    = 0 ;        
static char		yyFileName[4096] ;
static int              lastMemberGroupLine;
static MethodTypes 	mtype;
static bool    		gstat;
static bool             removeSlashes;
static Specifier 	virt;
static Specifier 	baseVirt;
static QCString         msType,msName,msArgs;
static int              memberGroupId = NOGROUP;
static QCString         memberGroupHeader;
static QCString         memberGroupDocs;
static bool             isTypedef;
static char             afterDocTerminator;
static int              tmpDocType;
static QCString         sectionLabel;
static QCString		sectionTitle;
//static SectionInfo::SectionType      
//                        sectionType;
static QCString         funcPtrType;
static QCString         templateStr;
static QCString         aliasName;
static QCString         baseName;
static QCString*        specName;
static QCString         formulaText;
static bool             useOverrideCommands = FALSE;
static bool             insideIDL = FALSE;		//!< processing IDL code?
static bool             insideJava = FALSE;		//!< processing Java code?
static bool             insideCS = FALSE;		//!< processing C# code?
static bool             insidePHP = FALSE;		//!< processing PHP code?
static bool             insideCppQuote = FALSE;

static int              argRoundCount;
static int              argSharpCount;
static int              currentArgumentContext;
static int              lastCopyArgStringContext;
static int              lastCopyArgContext;
static QCString         *copyArgString;
static QCString         fullArgString;

static ArgumentList     *currentArgumentList;
//static QCString         *currentTemplateSpec;
static char             lastCopyArgChar;
static QCString         *pCopyRoundString;
static QCString         *pCopyCurlyString;
static QCString         *pCopyQuotedString;
static QCString         *pSkipDoc;
static QStack<Grouping> autoGroupStack;
static Grouping  lastDefGroup( "", Grouping::GROUPING_LOWEST );

static bool             insideFormula;
static bool  	        insideTryBlock=FALSE;
static bool             needsSemi;

static int              depthIf;
static int  	        initializerSharpCount;
static QCString         memberGroupRelates;
static QCString         memberGroupInside;
static QCString         xrefItemKey;
static QCString         xrefItemTitle;
static QCString         xrefListTitle;


//-----------------------------------------------------------------------------

static void initParser()
{
  sectionLabel.resize(0);
  sectionTitle.resize(0);
  baseName.resize(0);
  formulaText.resize(0);
  protection = Public;
  baseProt = Public;
  sharpCount = 0;
  roundCount = 0;
  curlyCount = 0;
  memberGroupId = NOGROUP;
  memberGroupRelates.resize(0);
  memberGroupInside.resize(0);
  mtype = Method;
  gstat = FALSE;
  virt = Normal;
  baseVirt = Normal;
  isTypedef = FALSE;
  autoGroupStack.clear();
  insideTryBlock = FALSE;
  autoGroupStack.setAutoDelete(TRUE);
  lastDefGroup.groupname.resize(0);
  insideFormula = FALSE;
}

static void initEntry()
{
  if (insideJava) protection = Package;
  current->protection = protection ;
  current->mtype      = mtype;
  current->virt       = virt;
  current->stat       = gstat;
  current->mGrpId     = memberGroupId;
  current->relates    = memberGroupRelates.copy();
  current->inside     = memberGroupInside.copy();
  if (!autoGroupStack.isEmpty())
  {
    //printf("Appending group %s\n",autoGroupStack.top()->groupname.data());
    current->groups->append(new Grouping(*autoGroupStack.top()));
  }
}


//-----------------------------------------------------------------------------

/// remove any automatic grouping and add new one (if given)
static void setCurrentGroup( QCString *newgroup, Grouping::GroupPri_t pri )
{
   /* remove auto group name from current entry and discard it */
   Grouping *g = current->groups->first();
   int i=0; 
   while (g)
   {
     if (g->pri <= Grouping::GROUPING_AUTO_DEF)
     {
       current->groups->remove(i);
       i--;
     }
     g=current->groups->next();
     i++;
   }

   /* use new group name instead? */
   if ( newgroup )
   {
      current->groups->append(new Grouping(*newgroup, pri));
   } 
}

static int newMemberGroupId()
{
  static int curGroupId=0;
  return curGroupId++;
}

// forward declarations
static void startGroup();
static void startGroupInDoc();
static void endGroup();

//-----------------------------------------------------------------------------

static void lineCount()
{
  for( const char* c = yytext ; *c ; ++c )
    yyLineNr += (*c == '\n') ;
}

static void addType( Entry* current )
{
    uint tl=current->type.length();
    if( tl>0 && !current->name.isEmpty() && current->type.at(tl-1)!='.') 
    {
      current->type += ' ' ;
    }
    current->type += current->name ;
    current->name.resize(0) ;
    tl=current->type.length();
    if( tl>0 && !current->args.isEmpty() && current->type.at(tl-1)!='.') 
    {
      current->type += ' ' ;
    }
    current->type += current->args ;
    current->args.resize(0) ;
    current->argList->clear();
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

static void newDocState();

//-----------------------------------------------------------------

static void addXRefItem(const char *listName,const char *itemTitle,const char *listTitle)
{
  if (listName==0) return;

  ListItemInfo *lii=0;
  RefList *refList = Doxygen::xrefLists->find(listName);
  if (refList==0) // new list
  {
    refList = new RefList(listName,listTitle,itemTitle);
    Doxygen::xrefLists->insert(listName,refList);
  }
  if (current->sli)
  {
    QListIterator<ListItemInfo> slii(*current->sli);
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      if (strcmp(lii->type,listName)==0) break;
    }
  }
  if (lii) // already found item of same type before
  {
    RefItem *item = refList->getRefItem(lii->itemId);
    ASSERT(item!=0);
    item->text += " <p>";
    item->text += current->brief;
    //printf("%s: text +=%s\n",listName,item->text.data());
  }
  else // new item
  {
    int itemId  = refList->addRefItem();
    char anchorLabel[1024];
    sprintf(anchorLabel,"_%s%06d",listName,itemId);
    RefItem *item = refList->getRefItem(itemId);
    ASSERT(item!=0);
    item->text = current->brief.copy();
    item->listAnchor = anchorLabel;
    current->addSpecialListItem(listName,itemId);
    QCString cmdString;
    cmdString.sprintf("\\xrefitem %s %d\n",listName,itemId);
    current->doc += cmdString;
    SectionInfo *si=new SectionInfo(listName,anchorLabel,
	                            sectionTitle,SectionInfo::Anchor);
    Doxygen::sectionDict.insert(anchorLabel,si);
    current->anchors->append(new QCString(anchorLabel));
  }
  current->brief  = slString.copy(); // restore orginial brief desc.
}

//-----------------------------------------------------------------------------

// Adds a formula text to the list/dictionary of formulas if it was
// not already added. Returns the label of the formula.
static QCString addFormula()
{
  QCString formLabel;
  QCString fText=formulaText.simplifyWhiteSpace();
  Formula *f=0;
  if ((f=Doxygen::formulaDict[fText])==0)
  {
    f = new Formula(fText);
    Doxygen::formulaList.append(f);
    Doxygen::formulaDict.insert(fText,f);
    formLabel.sprintf("\\form#%d",f->getId());
    Doxygen::formulaNameDict.insert(formLabel,f);
  }
  else
  {
    formLabel.sprintf("\\form#%d",f->getId());
  }
  return formLabel;
}

static bool nameIsOperator(QCString &name)
{
  int i=name.find("operator");
  if (i==-1) return FALSE;
  if (i==0 && !isId(name.at(8))) return TRUE; // case operator ::X
  if (i>0 && !isId(name.at(i-1)) && !isId(name.at(i+8))) return TRUE; // case X::operator
  return FALSE; // case TEXToperatorTEXT
}

static void checkFormula()
{
  if (insideFormula)
  {
    warn(yyFileName,yyLineNr,"Warning: End of comment block while inside formula.");
  }
}

static void checkDocs()
{
  checkFormula();
  if ((current->brief.length()>2 && 
       current->brief.at(0)=='<' && current->brief.at(1)==' ') ||
      (current->doc.length()>2 && 
       current->doc.at(0)=='<' && current->doc.at(1)==' ')
     )
  {
    warn(yyFileName,yyLineNr,"Warning: Found lonely '<' symbol at the start of the documentation.");
	 
  }
}

#if 0
static QCString extractName(const QCString &s)
{
  //static const QRegExp id("[a-z_A-Z][a-z_A-Z0-9]*");
  //int i,p=0,l;
  //while ((i=id.match(s,p,&l))!=-1)
  //{
  //  QCString idstr=s.mid(i,l);
  //  if (idstr!="struct" && idstr!="class" && idstr!="union") 
  //  {
  //    
  //    return idstr;
  //  }
  //  p=i+l;
  //}
  //return "";
  QCString result=s;
  if (result.left(7)=="struct ") result=result.right(result.length()-7);
  if (result.left(6)=="class " ) result=result.right(result.length()-6);
  if (result.left(6)=="union " ) result=result.right(result.length()-6);
  int l=result.length()-1;
  while (l>=0 && 
         (result.at(l)=='*' || result.at(l)==' ' || isspace(result.at(l)))
        ) l--;
  return removeRedundantWhiteSpace(result.left(l+1));
}
#endif

static void setContext()
{
  QCString fileName = yyFileName;
  insideIDL  = fileName.right(4)==".idl" || fileName.right(4)==".odl";
  insideJava = fileName.right(5)==".java";
  insideCS   = fileName.right(3)==".cs";
  insidePHP  = fileName.right(4)==".php" || fileName.right(5)==".php4" || 
               fileName.right(4)==".inc" || fileName.right(6)==".phtml";
  if ( insidePHP )
  {
    useOverrideCommands = TRUE;
  }
  //printf("setContext(%s) insideIDL=%d\n",yyFileName,insideIDL);
}

static void prependScope()
{
  if (current_root->section & Entry::SCOPE_MASK)
  {
    //printf("--- prependScope %s to %s\n",current_root->name.data(),current->name.data());
    current->name.prepend(current_root->name+"::");
    if (current_root->tArgLists)
    {
      if (current->tArgLists==0)
      {
	current->tArgLists = new QList<ArgumentList>;
	current->tArgLists->setAutoDelete(TRUE);
      }
      //printf("prependScope #=%d #current=%d\n",current_root->tArgLists->count(),current->tArgLists->count());
      QListIterator<ArgumentList> talsi(*current_root->tArgLists);
      ArgumentList *srcAl=0;
      for (talsi.toLast();(srcAl=talsi.current());--talsi)
      {
        ArgumentList *dstAl = new ArgumentList;
	dstAl->setAutoDelete(TRUE);
	QListIterator<Argument> tali(*srcAl);
        Argument *a;
        for (;(a=tali.current());++tali)
        {
          dstAl->append(new Argument(*a));
        //printf("appending argument %s %s\n",a->type.data(),a->name.data());
        }	  
        current->tArgLists->insert(0,dstAl);	
      }
    }
  }
}

/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
    int c=0;
    while( c < max_size && inputString[inputPosition] )
    {
	*buf = inputString[inputPosition++] ;
	//printf("%d (%c)\n",*buf,*buf);
	c++; buf++;
    }
    return c;
}

%}

       /* start command character */
CMD	  ("\\"|"@")
SECTIONCMD {CMD}("image"|"author"|"internal"|"version"|"date"|"deprecated"|"param"|"exception"|"return"[s]?|"retval"|"bug"|"warning"|"par"|"sa"|"see"|"pre"|"post"|"invariant"|"note"|"remark"[s]?|"todo"|"test"|"xrefitem"|"ingroup"|"latexonly"|"htmlonly"|"{"|"verbatim"|"dotfile"|"defgroup"|"addtogroup"|"weakgroup"|"class"|"namespace"|"union"|"struct"|"fn"|"var"|"typedef"|"def"|"overload")|("<"{PRE}">")
BN        [ \t\n\r]
BL        [ \t\r]*"\n" 
B         [ \t]
BS        ^(({B}*"//")?)(({B}*"*"+)?){B}*
FILESCHAR [a-z_A-Z0-9\\:\\\/\-\+]
FILEECHAR [a-z_A-Z0-9\-\+]
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]+"\"")
ID        [a-z_A-Z][a-z_A-Z0-9]*
LABELID   [a-z_A-Z][a-z_A-Z0-9\-]*
SCOPEID   {ID}({ID}*{BN}*"::"{BN}*)*({ID}?)
SCOPENAME (({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
CSSCOPENAME (({ID}?{BN}*"."{BN}*)*)((~{BN}*)?{ID})
ATTR      ({B}+[^>\n]*)?
A         [aA]
BR	  [bB][rR]
PRE       [pP][rR][eE]
TABLE	  [tT][aA][bB][lL][eE]
P	  [pP]
UL        [uU][lL]
OL	  [oO][lL]
DL	  [dD][lL]
TITLE     [tT][iI][tT][lL][eE]
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^' \\\n]{1,4}"'"))
PHPKW	  ("require"|"require_once"|"include"|"include_once"|"echo")[^a-zA-Z0-9_;]

%option noyywrap

%x	Define
%x      DefineArg
%x	DefineEnd
%x	CompoundName
%x	ClassVar
%x	ClassTemplSpec
%x	Bases
%x	BasesProt
%x	NextSemi
%x	BitFields
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
%x	Operator
%x	Array
%x	ReadBody
%x	ReadNSBody
%x	Using
%x	UsingDirective
%x	NameSpaceDocArg1
%x	PackageDocArg1
%x	SkipCurly
%x	SkipCurlyCpp
%x	SkipCurlyEndDoc
%x      SkipString
%x	SkipInits
%x	SkipCPP
%x	SkipCPPBlock
%x	SkipComment
%x	SkipCxxComment
%x      SkipCurlyBlock
%x      SkipRoundBlock
%x	SkipCode
%x	Sharp
%x      SkipSharp
%x	SkipRound
%x	SkipSquare
%x	SkipSection
%x	IfGuard
%x	IfNotGuard
%x	TypedefName
%x	TryFunctionBlock
%x	TryFunctionBlockEnd
%x	Comment
%x      Doc
%x	JavaDoc
%x      ClassDoc
%x      LineDoc
%x      DefLineDoc
%x      ClassDocArg1
%x      ClassDocArg2
%x      ClassDocArg3
%x      ClassDocFunc
%x      ClassDocFuncPtr
%x      ClassDocFuncQual
%x      ClassDocFuncSkipLine
%x      ClassDocFuncExc
%x	ClassDocDefine
%x      ClassDocRelates
%x      ClassDocBrief
%x      ClassDocOverload
%x	ClassDefineArgs
%x	DocInternal
%x	DocInternalLine
%x	DocBaseClass
%x	CppQuote
%x	EndCppQuote
%x	GroupDocArg1
%x	GroupDocArg2
%x	GroupName
%x	GroupHeader
%x	StoreGroupDocs
%x	AfterDoc
%x	AfterDocBrief
%x	AfterDocLine
%x      PageDoc
%x      PageDocTitle
%x      PageDocArg1
%x      PageDocArg2
%x      FileDocArg1
%x      FileDocArg2
%x      ExampleDoc
%x	ExampleDocArg1
%x	EnumDocArg1
%x	FuncPtr
%x	EndFuncPtr
%x	FuncFunc
%x	FuncFuncEnd
%x	FuncFuncType
%x      MemberSpec
%x      MemberSpecSkip
%x	SkipVerbatim
%x      TodoParam
%x      TestParam
%x      BugParam
%x      DeprecatedParam
%x      XRefItemParam1
%x      XRefItemParam2
%x      XRefItemParam3
%x      XRefItemParam4
%x	SectionLabel
%x	SectionTitle
%x	SkipTemplate
%x	EndTemplate
%x      CopyArgString
%x	CopyArgRound
%x	CopyArgSharp
%x	CopyArgComment
%x	CopyArgCommentLine
%x	SkipUnionSwitch
%x	ReadFuncArgType
%x	ReadTempArgs
%x	Specialization
%x      SkipHtmlComment
%x      ReadFormulaShort
%x	ReadFormulaLong
%x	AnchorLabel
%x	ReadInitializer
%x	CopyString
%x	CopyRound
%x	CopyCurly
%x	IDLUnionCase
%x	NSAliasName
%x	NSAliasArg
%x	PackageName
%x	GetCallType
%x	JavaImport
%x	CSAccessorDecl
%x	PreLineCtrl
%x	DefinePHP
%x	DefinePHPEnd

%%

<*>\x06[^\x06]*\x06			{ // new file
  					  if (memberGroupId!=NOGROUP)
					  {
					    warn(yyFileName,yyLineNr,"Warning: Missing //@}");
					    memberGroupId=NOGROUP;
					  }
  					  yyLineNr= 0 ; // there is always an extra newline at the start of the file
					  int i; 
					  for( i = 0 ; yytext[i+1] != 6 ; i++ )
					    yyFileName[i] = yytext[i+1] ;
					  yyFileName[i] = 0 ;
					  setContext();
					  msg("Parsing file %s...\n",yyFileName);
					  current_root  = global_root ;
					  initParser();
					  current->reset();
					  int sec=guessSection(yyFileName);
					  if (sec)
					  {
					    current->name    = yyFileName;
					    current->section = sec;
					    current_root->addSubEntry(current);
					    current          = new Entry;
					  }
					  if ( insidePHP )
					  {
					    BEGIN( FindMembersPHP );
					  }
					  else
					  {
					    BEGIN( FindMembers );
					  }
                                        }
<NextSemi>"{"				{
  					  curlyCount=0;
					  needsSemi = TRUE;
  					  BEGIN(SkipCurlyBlock); 
					}
<NextSemi>"("				{
  				 	  roundCount=0;
  					  BEGIN(SkipRoundBlock);
  					}
<SkipRoundBlock>"("			{
					  ++roundCount;
  					}
<SkipRoundBlock>")"			{
  					  if (roundCount )
					    --roundCount ;
					  else
					    BEGIN( NextSemi ) ;
  					}
<SkipCurlyBlock>"{"			{
  					  ++curlyCount ; 
					}
<SkipCurlyBlock>"}"			{ 
  				          if( curlyCount )
					  {
					    --curlyCount ;
					  }
					  else if (needsSemi)
					  {
					    BEGIN( NextSemi );
					  }
					  else
					  {
					    BEGIN( FindMembers );
					  }
					}
<NextSemi>{CHARLIT}			
<NextSemi>\"				{
  					  lastStringContext=NextSemi;
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
<FindMembersPHP>"<?"("php"?)            { // PHP code start
                                           BEGIN( FindMembers );
					}
<FindMembersPHP>.                       { // Non-PHP code text, ignore
				        }
<FindMembers>"?>"                       { // PHP code end
					  if (insidePHP)
					    BEGIN( FindMembersPHP );
					  else
					    REJECT;
                                        }

<FindMembers>{PHPKW}			{ if (insidePHP)
					    BEGIN( NextSemi );
					  else
					    REJECT;
					}
<FindMembers>{B}*("properties"|"__property"){BN}*":"{BN}*  { // IDL or Borland C++ builder property 
  					  current->mtype = mtype = Property;
					  current->protection = protection = Public ;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}

<FindMembers>{B}*"k_dcop"{BN}*":"{BN}*  { current->mtype = mtype = DCOP;
					  current->protection = protection = Public ;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}

<FindMembers>{B}*"signals"{BN}*":"{BN}* { current->mtype = mtype = Signal;
					  current->protection = protection = Public ;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}

<FindMembers>{B}*"public"{BN}*"slots"{BN}*":"{BN}* {
					  current->protection = protection = Public ;
					  current->mtype = mtype = Slot;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount();
					}

<FindMembers>{B}*"protected"{BN}*"slots"{BN}*":"{BN}* {
					  current->protection = protection = Protected ;
					  current->mtype = mtype = Slot;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount();
					}

<FindMembers>{B}*"private"{BN}*"slots"{BN}*":"{BN}* {
					  current->protection = protection = Private ;
					  current->mtype = mtype = Slot;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount();
					}
<FindMembers>{B}*("public"|"methods"|"__published"){BN}*":"{BN}* { 
					  current->protection = protection = Public ;
					  current->mtype = mtype = Method;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}
<FindMembers>{B}*"protected"{BN}*":"{BN}* { 
  					  current->protection = protection = Protected ;
					  current->mtype = mtype = Method;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}
<FindMembers>{B}*"private"{BN}*":"{BN}*	{ 
  					  current->protection = protection = Private ;
					  current->mtype = mtype = Method;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}
<FindMembers>{BN}{1,80}			{
  					  lineCount();
  					}
<PackageName>{ID}("."{ID})*		{
  					  //current->name = yytext;
                                          //current->fileName = yyFileName; 
                                          //current->startLine = yyLineNr; 
  					  //current->section=Entry::PACKAGE_SEC;
					  //current_root->addSubEntry(current);
					  //current = new Entry ;
					  //initEntry();

  					  isTypedef=FALSE;
					  current->name = yytext;
					  current->name = substitute(current->name,".","::");
					  current->section = Entry::NAMESPACE_SEC;
					  current->type = "namespace" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount();
					  curlyCount=0;
					  current_root->addSubEntry(current);
                                          current_root = current ;
					  current             = new Entry ;
					  initEntry();
					  BEGIN( FindMembers ) ;
  					}
<PackageName>";"			{
  					  BEGIN(FindMembers);
  					}
<FindMembers>{B}*"static"{BN}+     	{ //current->type += " static ";
  					  current->stat = TRUE;
					  lineCount();
					}
<FindMembers>{B}*"extern"{BN}+		{
  					  current->stat = FALSE;
					  current->explicitExternal = TRUE;
					  lineCount();
  					}
<FindMembers>{B}*"virtual"{BN}+    	{ current->type += " virtual ";
					  current->virt = Virtual;
					  lineCount();
					}
<FindMembers>{B}*"abstract"{BN}+    	{ current->type += " abstract ";
					  current->virt = Pure;
					  lineCount();
					}
<FindMembers>{B}*"inline"{BN}+		{ current->memSpec|=Entry::Inline;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"mutable"{BN}+		{ current->memSpec|=Entry::Mutable;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"explicit"{BN}+	{ current->memSpec|=Entry::Explicit;
                                          lineCount(); 
                                        }
  /*
<FindMembers>{B}*"import"{BN}+		{ // IDL import keyword
  					  BEGIN( NextSemi );
  					}
  */
<FindMembers>{B}*"typename"{BN}+	{ lineCount(); }
<FindMembers>{B}*"namespace"{BN}*/[^a-z_A-Z0-9]	{ 
  					  isTypedef=FALSE;
					  current->section = Entry::NAMESPACE_SEC;
					  current->type = "namespace" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount();
  					  BEGIN( CompoundName ); 
					}
<FindMembers>{B}*"module"{BN}+		{ 
					  lineCount();
                                          if (insideIDL)
					  {
  					    isTypedef=FALSE;
					    current->section = Entry::NAMESPACE_SEC;
					    current->type = "module" ;
					    current->fileName  = yyFileName;
					    current->startLine = yyLineNr;
					    current->bodyLine  = yyLineNr;
  					    BEGIN( CompoundName ); 
					  }
					  else
					  {
					    addType( current ) ;
					    current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{B}*"library"{BN}+		{ 
					  lineCount();
                                          if (insideIDL)
					  {
  					    isTypedef=FALSE;
					    current->section = Entry::NAMESPACE_SEC;
					    current->type = "library" ;
					    current->fileName  = yyFileName;
					    current->startLine = yyLineNr;
					    current->bodyLine  = yyLineNr;
  					    BEGIN( CompoundName ); 
					  }
					  else
					  {
					    addType( current ) ;
					    current->name = QCString(yytext).stripWhiteSpace();
					  }
					}
<FindMembers>{B}*((("disp")?"interface")|"valuetype"){BN}+ 	{ // M$/Corba IDL interface
					  lineCount();
                                          if (insideIDL || insideJava || insideCS)
					  {
  					    isTypedef=FALSE;
					    current->section = Entry::INTERFACE_SEC;
					    addType( current ) ;
					    current->type += " interface" ;
					    current->fileName  = yyFileName;
					    current->startLine = yyLineNr;
					    current->bodyLine  = yyLineNr;
					    BEGIN( CompoundName );
					  }
					  else
					  {
					    addType( current ) ;
					    current->name = QCString(yytext).stripWhiteSpace();
					  }
  					}
<FindMembers>{B}*"exception"{BN}+ 	{ // Corba IDL exception
  					  isTypedef=FALSE;
					  current->section = Entry::EXCEPTION_SEC;
					  addType( current ) ;
					  current->type += " exception" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount();
					  BEGIN( CompoundName );
  					}
<FindMembers>{B}*(("typedef"{BN}+)?)("volatile"{BN}+)?"class{" |
<FindMembers>{B}*(("typedef"{BN}+)?)("volatile"{BN}+)?"class"{BN}+ { 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::CLASS_SEC;
					  addType( current ) ;
					  current->type += " class" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<FindMembers>{B}*"coclass"{BN}+		{
  					  if (insideIDL)
					  {
					    isTypedef=FALSE;
					    current->section = Entry::CLASS_SEC;
					    addType( current ) ;
					    current->type += " coclass" ;
					    current->fileName  = yyFileName;
					    current->startLine = yyLineNr;
					    current->bodyLine  = yyLineNr;
					    lineCount() ;
					    BEGIN( CompoundName ) ;
					  }
					  else
					  {
  					    addType(current);
					    current->name = yytext;
					    current->name = current->name.stripWhiteSpace();
					    lineCount();
					  }
  					}
<FindMembers>{B}*(("typedef"{BN}+)?)("volatile"{BN}+)?"struct{" | 
<FindMembers>{B}*(("typedef"{BN}+)?)("volatile"{BN}+)?"struct"/{BN}+ { 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::STRUCT_SEC ;
					  addType( current ) ;
					  current->type += " struct" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  //if (current->mtArgList) // transfer template arguments
					  //{
					  //  if (current->tArgList)
					  //  {
					  //    delete current->tArgList;
					  //  }
					  //  current->tArgList = current->mtArgList;
					  //  current->mtArgList = 0;
					  //}
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)("volatile"{BN}+)?"union{" |
<FindMembers>{B}*(("typedef"{BN}+)?)("volatile"{BN}+)?"union"{BN}+ { 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::UNION_SEC ;
					  addType( current ) ;
					  current->type += " union" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"enum{" | 
<FindMembers>{B}*(("typedef"{BN}+)?)"enum"{BN}+	{ 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::ENUM_SEC ;
					  addType( current ) ;
					  current->type += " enum" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount() ;
					  if (yytext[yyleng-1]=='{') unput('{');
					  BEGIN( CompoundName ) ;
					}
<Operator>"("{BN}*")"{BN}*/"("		{
  					  lineCount();
    					  current->name += yytext ;
					  current->name = current->name.simplifyWhiteSpace();
					  BEGIN( FindMembers ) ;
  					}
<Operator>";"				{ // can occur when importing members
  					  unput(';');
					  BEGIN( FindMembers ) ;
  					}
<Operator>[^(]				{ 
    					  lineCount();
    					  current->name += *yytext ;
					}
<Operator>"<>"				{ /* skip guided templ specifiers */ }
<Operator>"("				{
					  current->name = current->name.simplifyWhiteSpace();
					  unput(*yytext);
					  BEGIN( FindMembers ) ;
					}
<FindMembers>"template"({BN}*)"<"/[>]?	{ 
  					  lineCount();
					  if (current->tArgLists==0)
					  {
					    current->tArgLists = new QList<ArgumentList>;
					    current->tArgLists->setAutoDelete(TRUE);
					  }
					  ArgumentList *al = new ArgumentList;
					  al->setAutoDelete(TRUE);
					  current->tArgLists->append(al);
					  currentArgumentList = al;
					  templateStr="<";
					  fullArgString = templateStr.copy();
					  copyArgString = &templateStr;
					  currentArgumentContext = FindMembers;
					  BEGIN( ReadTempArgs );
  					}
<FindMembers>"namespace"{BN}+/{ID}{BN}*"=" { // namespace alias
                                          lineCount(); 
  					  BEGIN( NSAliasName );
					}
<NSAliasName>{ID}			{
  					  aliasName = yytext;
					  BEGIN( NSAliasArg );
  					}
<NSAliasArg>({ID}"::")*{ID}		{
  					  //printf("Inserting namespace alias %s::%s->%s\n",current_root->name.data(),aliasName.data(),yytext);
					  //if (current_root->name.isEmpty())
					  //{
                                          // TODO: namespace aliases are now treated as global entities
                                          // while they should be aware of the scope they are in
                                            Doxygen::namespaceAliasDict.insert(aliasName,new QCString(yytext));
					  //}
					  //else
					  //{
                                          //  Doxygen::namespaceAliasDict.insert(current_root->name+"::"+aliasName,
					  //	new QCString(current_root->name+"::"+yytext));
					  //}
  					}
<NSAliasArg>";"				{
  					  BEGIN( FindMembers );
  					}
<JavaImport>({ID}{BN}*"."{BN}*)+"*"	{ // package import => add as a using directive
  					  lineCount();
  					  QCString scope=yytext;
					  current->name=removeRedundantWhiteSpace(substitute(scope.left(scope.length()-2),".","::"));
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDIR_SEC;
					  current_root->addSubEntry(current);
					  current = new Entry ;
					  initEntry();
					  BEGIN(Using);
  					}
<JavaImport>({ID}{BN}*"."{BN}*)+{ID}	{ // class import => add as a using declaration
                                          lineCount();
  					  QCString scope=yytext;
					  current->name=removeRedundantWhiteSpace(substitute(scope,".","::"));
					  //printf("import name = %s -> %s\n",yytext,current->name.data());
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDECL_SEC;
					  current_root->addSubEntry(current);
					  current = new Entry ;
					  initEntry();
					  BEGIN(Using);
  					}
<FindMembers>"using"{BN}+		{ 
					  current->startLine=yyLineNr; 
                                          lineCount(); 
                                          BEGIN(Using); 
                                        }
<Using>"namespace"{BN}+			{ lineCount(); BEGIN(UsingDirective); }
<Using>{ID}{BN}*"::"{BN}*{ID}({BN}*"::"{BN}*{ID})*	{
                                          lineCount();
  					  current->name=yytext;
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDECL_SEC;
					  //printf("Found using declaration %s\n",yytext);
					  current_root->addSubEntry(current);
					  current             = new Entry ;
					  initEntry();
					  BEGIN(Using);
  					}
<UsingDirective>{SCOPENAME}		{ current->name=yytext;
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDIR_SEC;
					  //printf("Found using directive %s\n",yytext);
					  current_root->addSubEntry(current);
					  current             = new Entry ;
					  initEntry();
					  BEGIN(Using);
  					}
<Using>";"				{ BEGIN(FindMembers); }
<FindMembers>{SCOPENAME}{BN}*"<>"	{ // guided template decl
					  QCString n=yytext;
					  addType( current );
					  current->name=n.left(n.length()-2);
					}
<FindMembers>{SCOPENAME}{BN}*/"<"	{ // Note: this could be a return type!
  					  sharpCount=0;
					  lineCount();
					  addType( current );
  					  current->name=yytext;
					  current->name=current->name.stripWhiteSpace();
					  //current->scopeSpec.resize(0);
					  // currentTemplateSpec = &current->scopeSpec;
					  if (nameIsOperator(current->name))
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
					}
<FindMemberName>{SCOPENAME}{BN}*/"<"	{
  					  sharpCount=0;
					  lineCount();
  					  current->name+=((QCString)yytext).stripWhiteSpace();
					  //current->memberSpec.resize(0);
					  // currentTemplateSpec = &current->memberSpec;
					  if (nameIsOperator(current->name))
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
  					}
<EndTemplate>"<<"			{
  					  current->name+=yytext;
  					  // *currentTemplateSpec+=yytext; 
  					}
<EndTemplate>"<"			{ 
  					  current->name+='<';
  					  // *currentTemplateSpec+='<'; 
					  sharpCount++; 
					}
<EndTemplate>">>"			{
  					  current->name+=yytext;
  					  // *currentTemplateSpec+=yytext; 
  					}
<EndTemplate>">"			{
  					  current->name+='>';
					  // *currentTemplateSpec+='>';
					  if (--sharpCount<=0)
					  {  
					    //printf("Found %s\n",current->name.data());
					    BEGIN(FindMembers);
					  }
					}
<EndTemplate>">"{BN}*"("		{ 
  					  lineCount();
  					  current->name+='>';
					  // *currentTemplateSpec+='>';
					  if (--sharpCount<=0)
					  {
					    current->args = "(";
					    currentArgumentContext = FuncQual;
					    fullArgString = current->args.copy();
					    copyArgString = &current->args;
					    //printf("Found %s\n",current->name.data());
					    BEGIN( ReadFuncArgType ) ;
					  }
					}
<EndTemplate>">"{BN}*/"("({BN}*{ID}{BN}*"::")*({BN}*"*"{BN}*)+ { // function pointer returning a template instance
  					  lineCount();
  					  current->name+='>';
					  BEGIN(FindMembers);
  					}
<EndTemplate>">"{BN}*/"::"		{
  					  lineCount();
  					  current->name+='>';
  					  // *currentTemplateSpec+='>';
					  if (--sharpCount<=0)
					  {
					    BEGIN(FindMemberName);
					  }
  					}
<EndTemplate>.				{ 
  					  current->name+=*yytext;
  					  // *currentTemplateSpec+=*yytext; 
					}
<FindMembers>"define"{BN}*"("{BN}*["']	{
					  if (insidePHP)
					  {
					    current->bodyLine = yyLineNr;
					    BEGIN( DefinePHP );
					  }
					  else
					    REJECT;
					}
<FindMembers,FindMemberName>{SCOPENAME}	{
					  lineCount();
                                          if (insideIDL && yyleng==9 && strcmp(yytext,"cpp_quote")==0)
					  {
					    BEGIN(CppQuote);
					  }
					  else if ((insideIDL || insideJava) && yyleng==6 && strcmp(yytext,"import")==0)
					  {
					    if (insideIDL)
					      BEGIN(NextSemi);
					    else // insideJava
					      BEGIN(JavaImport);
					  }
					  else if (insideJava && strcmp(yytext,"package")==0)
					  {
  					    lineCount();
					    BEGIN(PackageName);
					  }
					  else if (insideIDL && strcmp(yytext,"case")==0)
					  {
					    BEGIN(IDLUnionCase);
					  }
					  else if (insideTryBlock && strcmp(yytext,"catch")==0)
					  {
					    insideTryBlock=FALSE;
					    BEGIN(TryFunctionBlock);
					  }
					  else
					  {
					    if (YY_START==FindMembers)
					    {
					      addType( current ) ;
					    }
					    if ((insideJava || insideCS) && strcmp(yytext,"public")==0)
					    {
					      current->protection = Public;
					    }
					    else if ((insideJava || insideCS) && strcmp(yytext,"protected")==0)
					    {
					      current->protection = Protected;
					    }
					    else if ((insideJava || insideCS) && strcmp(yytext,"private")==0)
					    {
					      current->protection = Private;
					    }
					    else
					    {
					      if (YY_START==FindMembers)
					        current->name  = yytext;
					      else
						current->name += yytext;
					    }
					    QCString tmp=yytext;
					    if (nameIsOperator(tmp))
                                            {
					      BEGIN( Operator );
                                            }
					    else
                                            {
		 			      BEGIN(FindMembers);
                                            }
					  }
					}
<FindMembers>"."			{
  					  if (insideJava || insideCS)
					  {
					    current->name+=".";
					  }
  					}
<FindMembers>"::"			{
					  current->name+=yytext;
  					}
<CppQuote>"("{B}*"\""			{
  					  insideCppQuote=TRUE;
  					  BEGIN(FindMembers);
  					}
<IDLUnionCase>"::"
<IDLUnionCase>":"			{ BEGIN(FindMembers); }
<IDLUnionCase>\n			{ yyLineNr++; }
<IDLUnionCase>.
<TryFunctionBlock>\n			{ yyLineNr++; }
<TryFunctionBlock>"{"			{ 
					  curlyCount=0;
					  lastCurlyContext = TryFunctionBlockEnd ;
  					  BEGIN( SkipCurly );
					}
<TryFunctionBlock>.
<TryFunctionBlockEnd>"catch"		{ BEGIN(TryFunctionBlock); }
<TryFunctionBlockEnd>.			{ unput(*yytext); 
  					  BEGIN( FindMembers );
					}
<EndCppQuote>")"			{
  					  insideCppQuote=FALSE;
					  BEGIN(FindMembers);
  					}
<FindMembers>{B}*"#"			{ if (insidePHP)
					    REJECT;
					  lastCPPContext = YY_START;
					  BEGIN( SkipCPP ) ;
					}
<FindMembers>{B}*"#"{B}*"define"	{ if (insidePHP)
					    REJECT;
  					  current->bodyLine = yyLineNr;
  					  BEGIN( Define );
  					}
<FindMembers,ReadBody,ReadNSBody>{B}*"#"{B}+[0-9]+{B}+/"\""	{ /* line control directive */
                                          yyLineNr = atoi(&yytext[1]);
					  //printf("setting line number to %d\n",yyLineNr);
					  lastPreLineCtrlContext = YY_START;
  					  BEGIN( PreLineCtrl );
  					}
<PreLineCtrl>"\""[^\n\"]*"\""		{
				          strncpy(yyFileName,stripQuotes(yytext),4096);	
  					}
<PreLineCtrl>.				{}
<PreLineCtrl>\n				{
                                          yyLineNr++;
  					  BEGIN( lastPreLineCtrlContext );
  					}
<SkipCPP>.
<SkipCPP>\\[\r]*"\n"[\r]*		{ yyLineNr++ ; }
<SkipCPP>[\r]*\n[\r]*			{ yyLineNr++ ;
					  BEGIN( lastCPPContext) ;
					}
<Define>{ID}{B}*"("			{
  					  current->name = yytext;
					  current->name = current->name.left(current->name.length()-1).stripWhiteSpace();
					  current->args = "(";
  					  current->bodyLine = yyLineNr;
					  currentArgumentContext = DefineEnd;
					  fullArgString=current->args.copy();
					  copyArgString=&current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
 /*
<DefineArg>")"				{
  					  //printf("Define with args\n");
  					  current->args += ')';
  					  BEGIN( DefineEnd );
  					}
<DefineArg>.				{
  					  current->args += *yytext;
  					}
  */
<Define>{ID}				{
  					  //printf("Define `%s' without args\n",yytext);
  					  current->bodyLine = yyLineNr;
  					  current->name = yytext;
					  BEGIN(DefineEnd);
  					}
<DefineEnd>\n				{
  					  //printf("End define\n");
					  yyLineNr++;
					  current->fileName   = yyFileName;
					  current->startLine  = yyLineNr;
					  current->type.resize(0);
  					  current->args       = current->args.simplifyWhiteSpace();
  					  current->name       = current->name.stripWhiteSpace();
					  current->section    = Entry::DEFINE_SEC;
					  current_root->addSubEntry(current);
					  current             = new Entry ;
					  initEntry();
					  BEGIN(FindMembers);
  					}
<DefinePHPEnd>";"			{
  					  //printf("End define\n");
					  current->fileName   = yyFileName;
					  current->startLine  = yyLineNr;
					  current->type.resize(0);
  					  current->args       = current->args.simplifyWhiteSpace();
  					  current->name       = current->name.stripWhiteSpace();
					  current->section    = Entry::ENUM_SEC;  //HACK!
					  current_root->addSubEntry(current);
					  current             = new Entry ;
					  initEntry();
					  BEGIN(FindMembers);
  					}
<DefinePHPEnd>.
<DefineEnd>\\[\r]?\n			{
  					  yyLineNr++;
  					}
<DefineEnd>\"				{
					  if (insideIDL && insideCppQuote)
					  {
					    BEGIN(EndCppQuote);
					  }
					  else
					  {
					    lastStringContext=DefineEnd;
					    BEGIN(SkipString);
					  }
  					}
<DefineEnd>.				
<DefinePHP>{ID}["']{BN}*","{BN}*	{
  					  current->name = yytext;
					  current->name = current->name.stripWhiteSpace();
					  current->name = current->name.left(current->name.length()-1).stripWhiteSpace();
					  current->name = current->name.left(current->name.length()-1);
					  current->args = "(";
  					  current->bodyLine = yyLineNr;
  					  lastRoundContext = DefinePHPEnd;
  					  pCopyRoundString = &current->args;
					  roundCount = 0;
  					  BEGIN( CopyRound );
					}

<FindMembers>[*&]+			{ current->name += yytext ; 
  					  addType( current );
					}
<FindMembers,MemberSpec,Function,NextSemi,BitFields,ReadInitializer>";"{BN}*("/**"|"//!"|"/*!"|"///")"<" {
  					  lineCount();
					  if (current->bodyLine==-1)
					    current->bodyLine=yyLineNr;
					  lastAfterDocContext = YY_START;
					  afterDocTerminator = ';';
					  if (yytext[yyleng-3]=='/')
					  {
					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyFileName;
					    BEGIN(AfterDocLine);
					  }
					  else if (yytext[yyleng-2]=='*' && Config_getBool("JAVADOC_AUTOBRIEF"))
					  {
					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyLineNr;
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					    BEGIN(AfterDocBrief);
					  }
					  else
					  {
					    current->doc.resize(0);
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					    BEGIN(AfterDoc);
					  }
  					}
<MemberSpec,FindFields,FindMembers,NextSemi,BitFields,ReadInitializer>","{BN}*("/**"|"//!"|"/*!"|"///")"<" {
  					  lineCount();
					  lastAfterDocContext = YY_START;
					  afterDocTerminator = ',';
					  if (yytext[yyleng-3]=='/')
					  {
					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyLineNr;
					    BEGIN(AfterDocLine);
					  }
					  else if (yytext[yyleng-2]=='*' && Config_getBool("JAVADOC_AUTOBRIEF"))
					  {
					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyLineNr;
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					    BEGIN(AfterDocBrief);
					  }
					  else
					  {
					    current->doc.resize(0);
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					    BEGIN(AfterDoc);
					  }
  					}
<DefineEnd,FindFields,FindFieldArg,ReadInitializer>{BN}*("/**"|"//!"|"/*!"|"///")"<" {
  					  lineCount();
					  lastAfterDocContext = YY_START;
					  if (YY_START==DefineEnd)
					  {
					    afterDocTerminator = '\n';
					    yyLineNr--;
					  }
					  else
					    afterDocTerminator = 0;
					  if (yytext[yyleng-3]=='/')
					  {
					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyFileName;
					    BEGIN(AfterDocLine);
					  }
					  else if (yytext[yyleng-2]=='*' && Config_getBool("JAVADOC_AUTOBRIEF"))
					  {
					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyFileName;
					    BEGIN(AfterDocBrief);
					  }
					  else
					  {
					    current->doc.resize(0);
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					    BEGIN(AfterDoc);
					  }
  					}
<FindMembers,FindFields>("//"([!/]?){B}*{CMD}"{")|("/*"([!*]?){B}*{CMD}"{")	{
  					  startGroup();
					  tmpDocType=-1;
					  if (current_root->section & Entry::SCOPE_MASK)
                                          {
					      current->inside = current_root->name+"::";
                                              if (current->mGrpId!=NOGROUP)
                                              {
                                                memberGroupInside = current->inside.copy();
                                              }
                                          }
					  if (yytext[1]=='/') // C++ style comment
					  {
  					    current->brief.resize(0);
					    current->briefLine = yyLineNr;
					    current->briefFile = yyFileName;
					    lastDocContext = YY_START;
					    BEGIN( LineDoc );
					  }
					  else // C style comment 
					  {
					    current->doc.resize(0);
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					    lastDocContext = YY_START;
					    removeSlashes=FALSE;
					    BEGIN( Doc );
					  }
  					}
<FindMembers,FindFields,ReadInitializer>"//"([!/]?){B}*{CMD}"}".*|"/*"([!*]?){B}*{CMD}"}".*"*/"	{
                                          if (memberGroupId==NOGROUP && autoGroupStack.isEmpty())
                                          {
                                            warn(yyFileName,yyLineNr,
	                                        "Warning: end of group without matching begin.");
                                          }
                                          //printf("end of member group marker ends group %d\n",memberGroupId);
  					  endGroup();
                                          memberGroupHeader.resize(0);
  					}
<FindMembers>"="			{
  					  current->bodyLine = yyLineNr;
					  lastInitializerContext = YY_START;
					  initializerSharpCount=0;
					  BEGIN(ReadInitializer);
  					}
  /* Read initializer rules */
<ReadInitializer>"("			{
  					  lastRoundContext=YY_START;
  					  pCopyRoundString=&current->initializer;
					  roundCount=0;
  					  current->initializer+=*yytext; 
  					  BEGIN(CopyRound);
  					}
<ReadInitializer>"{"			{
  					  lastCurlyContext=YY_START;
  					  pCopyCurlyString=&current->initializer;
					  curlyCount=0;
  					  current->initializer+=*yytext; 
  					  BEGIN(CopyCurly);
  					}
<ReadInitializer>[;,]			{
  					  //printf(">> initializer `%s' <<\n",current->initializer.data());
  					  if (initializerSharpCount==0)
					  {
  					    unput(*yytext);
  					    BEGIN(lastInitializerContext);
					  }
					  else
					  {
  					    current->initializer+=*yytext; 
					  }
  					}
<ReadInitializer>\"			{
					  if (insideIDL && insideCppQuote)
  					  {
					    BEGIN(EndCppQuote);
					  }
					  else
					  {
                                            lastStringContext=YY_START;
  					    current->initializer+=*yytext; 
  					    pCopyQuotedString=&current->initializer;
					    BEGIN(CopyString);
					  }
					}
<ReadInitializer>"<<"			{
  					  current->initializer+=yytext; 
					}
<ReadInitializer>">>"			{
  					  current->initializer+=yytext; 
  					}
<ReadInitializer>\<			{
  					  initializerSharpCount++;
  					  current->initializer+=*yytext; 
  					}
<ReadInitializer>\>			{
  					  initializerSharpCount--;
  					  current->initializer+=*yytext; 
  					}
<ReadInitializer>{CHARLIT}              { current->initializer+=yytext; } 
<ReadInitializer>\n			{
  					  current->initializer+=*yytext;
					  yyLineNr++;
  					}
<ReadInitializer>.			{ 
  					  current->initializer+=*yytext; 
					}

  /* generic quoted string copy rules */
<CopyString>\\.				{
  					  *pCopyQuotedString+=yytext;
  					}
<CopyString>\"				{ 
  					  *pCopyQuotedString+=*yytext;
  					  BEGIN( lastStringContext ); 
					}
<CopyString>"/*"|"*/"|"//"		{
  					  *pCopyQuotedString+=yytext;
  					}
<CopyString>\n				{
  					  *pCopyQuotedString+=*yytext;
  					  yyLineNr++;
  					}
<CopyString>.				{
  					  *pCopyQuotedString+=*yytext;
  					}

  /* generic round bracket list copy rules */
<CopyRound>\"				{
					  *pCopyRoundString+=*yytext;
  					  pCopyQuotedString=pCopyRoundString;
					  lastStringContext=YY_START;
					  BEGIN(CopyString);
					}
<CopyRound>"("				{
  					  *pCopyRoundString+=*yytext;
  					  roundCount++;
  					}
<CopyRound>")"				{
  					  *pCopyRoundString+=*yytext;
					  if (--roundCount<0)
					    BEGIN(lastRoundContext);
  					}
<CopyRound>\n				{
  					  yyLineNr++;
  					  *pCopyRoundString+=*yytext;
  					}
<CopyRound>{CHARLIT}		        { *pCopyRoundString+=yytext; }
<CopyRound>[^"'()\n]+			{
  					  *pCopyRoundString+=yytext;
  					}

  /* generic curly bracket list copy rules */
<CopyCurly>\"				{
					  *pCopyCurlyString+=*yytext;
  					  pCopyQuotedString=pCopyCurlyString;
					  lastStringContext=YY_START;
					  BEGIN(CopyString);
					}
<CopyCurly>"{"				{
  					  *pCopyCurlyString+=*yytext;
					  curlyCount++;
  					}
<CopyCurly>"}"				{
					  *pCopyCurlyString+=*yytext;
					  if (--curlyCount<0)
					    BEGIN(lastCurlyContext); 
  					}
<CopyCurly>{CHARLIT}                    { *pCopyCurlyString+=yytext; }
<CopyCurly>[^"'{}\/\n]+			{
  					  *pCopyCurlyString+=yytext;
  					}
<CopyCurly>\n				{
  					  yyLineNr++;
					  *pCopyCurlyString+=*yytext;
  					}
<FindMembers>":"			{
  					  if (current->type.isEmpty()) // bit pad field
					  {
					    addType(current);
					    current->name.sprintf("__pad%d__",padCount++);
					  }
  					  BEGIN(BitFields);
					  current->bitfields+=":";
  					}
<BitFields>.				{
  					  current->bitfields+=*yytext;
  					}
<FindMembers>[;,]			{ 
  					  QCString oldType = current->type.copy();
					  if (current->bodyLine==-1)
					  {
					    current->bodyLine = yyLineNr;
					  }
                                          if ( insidePHP && current->type.left(3) == "var" )
                                          {
                                            current->type = current->type.mid(3);
                                          }
					  current->type=current->type.simplifyWhiteSpace();
					  current->args=current->args.simplifyWhiteSpace();
					  current->name=current->name.stripWhiteSpace();
					  //if (!current->name.isEmpty() && current->type.left(8)=="typedef ")
					  //{
					  //  // add typedef to dictionary
					  //  QCString dest = extractName(current->type.right(current->type.length()-8));
					  //  if (Doxygen::typedefDict[current->name]==0 && !dest.isEmpty())
					  //  {		
					  //    //printf("1>>>>>>>>>> adding %s->%s\n",current->name.data(),dest.data());
					  //    QCString scope;
					  //    if (current_root->section & Entry::SCOPE_MASK) scope=current_root->name;
                                          //    Doxygen::typedefDict.insert(current->name, new TypedefInfo(dest,scope));
					  //  }
					  //}
					  current->section = Entry::VARIABLE_SEC ;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  //printf("New variable type=`%s' name=`%s' groupId=%d\n",current->type.data(),current->name.data(),current->mGrpId);
					  current_root->addSubEntry( current ) ;
					  if ( *yytext == ',')
					  {
					    current = new Entry(*current);
					    current->name.resize(0);
					    current->args.resize(0);
					    current->initializer.resize(0);
					    current->bitfields.resize(0);
					    int i=oldType.length(); 
					    while (i>0 && (oldType[i-1]=='*' || oldType[i-1]=='&' || oldType[i-1]==' ')) i--;
					    current->type = oldType.left(i);
					  }
					  else
					  {
					    current = new Entry ;
					    initEntry();
					  }
					  BEGIN( FindMembers ) ;
					}

<FindMembers>"["			{ 
					  if (current->name.isEmpty() || current->name=="typedef") // IDL function property
					  {
					    squareCount=1;
					    lastSquareContext = YY_START;
					    BEGIN(SkipSquare);
					  }
					  else
					  {
  					    current->args += yytext ;
					    squareCount=1;
					    BEGIN( Array ) ;
					  }
					}
<Array>"]"				{ current->args += *yytext ;
					  if (--squareCount<=0)
	                                     BEGIN( FindMembers ) ;
					}
<Array>"["				{ current->args += *yytext ;
					  squareCount++;	
					}
<Array>.				{ current->args += *yytext ; }
<SkipSquare>"["				{ squareCount++; }
<SkipSquare>"]"				{
  					  if (--squareCount<=0)
					    BEGIN( lastSquareContext );
  					}
<SkipSquare>\"				{
  					  lastStringContext=YY_START;
  				          BEGIN( SkipString ); 
					}
<SkipSquare>[^\n\[\]\"]+
<FindMembers>"<"			{ addType( current ) ;
					  current->type += yytext ;
					  BEGIN( Sharp ) ;
					}
<Sharp>">"				{ current->type += *yytext ;
					  if (--sharpCount<=0)
	                                     BEGIN( FindMembers ) ;
					}
<Sharp>"<"				{ current->type += *yytext ;
					  sharpCount++;	
					}
<Sharp>{BN}+				{
  					  lineCount();
					}
<Sharp>.				{ current->type += *yytext ; }
<FindFields>{ID}			{
  					  current->name = yytext;
					}
<FindFields>"="				{
  					  lastInitializerContext = YY_START;
					  initializerSharpCount=0;
  					  BEGIN(ReadInitializer);
  					}
<FindFields>","				{
					  //printf("adding `%s' `%s' `%s' to enum `%s' (mGrpId=%d)\n",
					  //     current->type.data(), current->name.data(),
					  //     current->args.data(), current_root->name.data(),current->mGrpId);
  					  if (!current->name.isEmpty())
					  {
					    current->fileName   = yyFileName;
					    current->startLine  = yyLineNr;
					    current->type       = "@"; // enum marker
  					    current->args       = current->args.simplifyWhiteSpace();
  					    current->name       = current->name.stripWhiteSpace();
					    current->section    = Entry::VARIABLE_SEC;
					    // add to the scope of the enum
					    current_root->addSubEntry(current);
					    current             = new Entry(*current);
					    // add to the scope surrounding the enum (copy!)
					    current_root->parent->addSubEntry(current);
					    current             = new Entry ;
					    initEntry();
					  }
					  else // probably a redundant , 
					  {
				     	    current->reset();
					  }
  					}
<FindFields>"["				{ // attribute list in IDL
					  squareCount=1;
					  lastSquareContext = YY_START;
					  BEGIN(SkipSquare);
  					}
  /*
<FindFieldArg>","			{ unput(*yytext); BEGIN(FindFields); }
  */
<ReadBody,ReadNSBody>[^\r\n\#{}"'/]*	{ current->program += yytext ; }
<ReadBody,ReadNSBody>"//".*		{ current->program += yytext ; }
<ReadBody,ReadNSBody>"#".*		{ if (! insidePHP)
					    REJECT;
					  current->program += yytext ;
					}
<ReadBody,ReadNSBody>\"				{ current->program += yytext ; 
                                          pCopyQuotedString = &current->program;
                                          lastStringContext=YY_START;
                                          BEGIN( CopyString );
					}
<ReadBody,ReadNSBody>"/*"{B}*		{ current->program += yytext ;
					  lastContext = ReadBody ;
					  BEGIN( Comment ) ;
					}
<ReadBody,ReadNSBody>"/*"{BL}		{ current->program += yytext ;
					  ++yyLineNr ;
					  lastContext = ReadBody ;
					  BEGIN( Comment ) ;
					}
<ReadBody,ReadNSBody>{CHARLIT}                     { current->program += yytext; }
<ReadBody,ReadNSBody>"{"				{ current->program += yytext ;
					  ++curlyCount ;
					}
<ReadBody,ReadNSBody>"}"		{ //err("ReadBody count=%d\n",curlyCount);
  					  if ( curlyCount>0 )
					  {
					    current->program += yytext ;
					    --curlyCount ;
					  }
					  else
					  {
					    current->endBodyLine = yyLineNr;
					    QCString &cn = current->name;
					    QCString rn = current_root->name.copy();
					    //printf("cn=`%s' rn=`%s'\n",cn.data(),rn.data());
					    if (!cn.isEmpty() && !rn.isEmpty())
					    {
					      prependScope();
					      //cn.prepend(rn+"::");
					    }
					    if (isTypedef && cn.isEmpty())
					    {
					      //printf("Typedef Name\n");
					      BEGIN( TypedefName );
					    }
					    else
					    {
					      if (current->section == Entry::ENUM_SEC)
					      {
					        current->program+=','; // add field terminator
					      }
					      // add compound definition to the tree
  					      current->args = current->args.simplifyWhiteSpace();
  					      current->type = current->type.simplifyWhiteSpace();
  					      current->name = current->name.stripWhiteSpace();
					      //printf("adding `%s' `%s' `%s' brief=%s\n",current->type.data(),current->name.data(),current->args.data(),current->brief.data());
					      current_root->addSubEntry( current ) ;
					      current = new Entry(*current);
					      if (current->section==Entry::NAMESPACE_SEC || 
						  current->section==Entry::INTERFACE_SEC ||
						  insideJava || insidePHP || insideCS
						 )
					      { // namespaces and interfaces and java classes ends with a closing bracket without semicolon
						current->reset();
						initEntry();
						BEGIN( FindMembers ) ;
					      }
					      else
					      {
						BEGIN( MemberSpec ) ;
					      }
					    }
					  }
					}
<ReadBody>"}"{BN}+"typedef"{BN}+	{ //err("ReadBody count=%d\n",curlyCount);
  					  if ( curlyCount>0 )
					  {
					    current->program += yytext ;
					    --curlyCount ;
					  }
					  else
					  {
					    lineCount();
					    isTypedef = TRUE;
					    current->endBodyLine = yyLineNr;
					    QCString &cn = current->name;
					    QCString rn = current_root->name.copy();
					    if (!cn.isEmpty() && !rn.isEmpty())
					    {
					      prependScope();
					    }
					    BEGIN( TypedefName );
					  }
					}
<TypedefName>{ID}			{
  					  if (current->section == Entry::ENUM_SEC)
					  {
					    current->program+=","; // add field terminator
					  }
  				          current->name=yytext;
					  prependScope();
  					  current->args = current->args.simplifyWhiteSpace();
  					  current->type = current->type.simplifyWhiteSpace();
					  //printf("Adding compound %s %s %s\n",current->type.data(),current->name.data(),current->args.data());
					  current_root->addSubEntry( current ) ;
					  current = new Entry;
					  initEntry();
  					  BEGIN(MemberSpecSkip); 
  					}
<TypedefName>";"			{ /* typedef of anonymous type */
					  current->name.sprintf("@%d",anonCount++);
					  if (current->section == Entry::ENUM_SEC)
					  {
					    current->program+=','; // add field terminator
					  }
					  // add compound definition to the tree
  					  current->args = current->args.simplifyWhiteSpace();
  					  current->type = current->type.simplifyWhiteSpace();
					  current_root->addSubEntry( current ) ;
					  current = new Entry(*current);
					  unput(';');
					  BEGIN( MemberSpec ) ;
  					}
<MemberSpec>([*&]*{BN}*)*{ID}("["[a-z_A-Z0-9]*"]")* { // the [] part could be improved.
  					  lineCount();
  					  int i=0,l=yyleng,j;
					  while (i<l && (!isId(yytext[i]))) i++;
					  msName = yytext; 
					  msName = msName.right(msName.length()-i);
					  j=msName.find("[");
					  if (j!=-1) 
					  {
					    msArgs=msName.right(msName.length()-j);
					    msName=msName.left(j);
					  }
					  msType = yytext; msType=msType.left(i);
					}
<MemberSpec>[,;]			{
                                          //printf("current->name=`%s' msName=`%s'\n",current->name.data(),msName.data());
  					  if (msName.isEmpty() && !current->name.isEmpty())
					   /* && (current->section & Entry::COMPOUND_MASK)) */
					  { 
					    // see if the compound does not have a name or is inside another
					    // annonymous compound. If so we insert a 
					    // special `annonymous' variable.
					    Entry *p=current_root;
					    while (p)
					    {
					      // only look for class scopes, not namespace scopes
					      if ((p->section & Entry::COMPOUND_MASK) && !p->name.isEmpty())
					      {
						//printf("Trying scope `%s'\n",p->name.data());
						int i=p->name.findRev("::");
						int pi = (i==-1) ? 0 : i+2;
						if (p->name.at(pi)=='@')
						{
						  // annonymous compound inside -> insert dummy variable name
						  //printf("Adding annonymous variable for scope %s\n",p->name.data());
						  msName.sprintf("@%d",anonCount++); 
						  break;
						}
					      }
					      p=p->parent;
					    }
					  }
					  if (!msName.isEmpty())
					  {
  					    Entry *varEntry=new Entry;
					    varEntry->protection = current->protection ;
                                            varEntry->mtype = current->mtype;
					    varEntry->virt = current->virt;
					    varEntry->stat = current->stat;
					    varEntry->section = Entry::VARIABLE_SEC;
					    varEntry->name = msName.stripWhiteSpace();
					    varEntry->type = current->type.simplifyWhiteSpace()+" ";
					    varEntry->args = msArgs; //current->args.simplifyWhiteSpace();
					    //if (!current->name.isEmpty() && current->name[0]!='@' && 
					    //    current->parent->section & Entry::COMPOUND_MASK)
					    //  varEntry->type+=current->parent->name+"::";
					    if (isTypedef)
					    {
					      varEntry->type.prepend("typedef ");
					    //  //printf("current->name = %s %s\n",current->name.data(),msName.data());
					    //  if (!current->name.isEmpty() && current->name.at(0)!='@')
					    //  {
					    //	//printf("2>>>>>>>>>> adding %s->%s\n",msName.data(),current->name.data());
					    //    QCString scope;
					    //    if (current_root->section & Entry::SCOPE_MASK) scope=current_root->name;
                                            //    Doxygen::typedefDict.insert(msName,new TypedefInfo(current->name,scope));
					    //  }
					    }
					    varEntry->type+=current->name+msType;
					    varEntry->fileName = yyFileName;
					    varEntry->startLine = yyLineNr;
					    varEntry->doc = current->doc.copy();
					    varEntry->brief = current->brief.copy();
					    varEntry->mGrpId = current->mGrpId;

					    // deep copy group list
					    QListIterator<Grouping> gli(*current->groups);
					    Grouping *g;
					    for (;(g=gli.current());++gli)
					    {
					      varEntry->groups->append(new Grouping(*g));
					    }

					    //printf("Add: type=`%s',name=`%s',args=`%s'\n",
					    //      varEntry->type.data(),varEntry->name.data(),varEntry->args.data());
					    current_root->addSubEntry(varEntry);
					  }
					  if (*yytext==';')
					  {
					    msType.resize(0);
					    msName.resize(0);
					    msArgs.resize(0);
					    isTypedef=FALSE;
					    current->reset();
					    initEntry();
					    BEGIN( FindMembers );
					  }
  					}
<MemberSpec>"="				{ 
  					  lastInitializerContext=YY_START;
					  initializerSharpCount=0;
  					  BEGIN(ReadInitializer);
  					  /* BEGIN(MemberSpecSkip); */
					}
  /*
<MemberSpecSkip>"{"			{
  					  curlyCount=0;
					  lastCurlyContext = MemberSpecSkip;
					  previous = current;
  					  BEGIN(SkipCurly);
  					}
  */
<MemberSpecSkip>","			{ BEGIN(MemberSpec); }
<MemberSpecSkip>";"		        { unput(';'); BEGIN(MemberSpec); }
<ReadBody,ReadNSBody>{BN}+				{ current->program += yytext ;
					  lineCount() ;
					}
<ReadBody,ReadNSBody>.				{ current->program += yytext ; }

<FindMembers>"("/({BN}*{ID}{BN}*"::")*{ID}{BN}*")"{BN}*"(" | /* typedef void (A::func_t)(args...) */
<FindMembers>("("({BN}*{ID}{BN}*"::")*({BN}*"*"{BN}*)+)+ {   /* typedef void (A::*ptr_t)(args...) */
  					  current->bodyLine = yyLineNr;
  					  lineCount();
  					  addType(current);
					  funcPtrType=yytext;
					  roundCount=0;
					  //current->type += yytext;
					  BEGIN( FuncPtr );
  					}
<FuncPtr>{SCOPENAME}			{
  					  current->name = yytext;
					  if (current->name=="const" || current->name=="volatile")
					  {
					    funcPtrType += current->name;
					  }
					  else
					  {
					    BEGIN( EndFuncPtr );
					  }
  					}
<FuncPtr>.				{
  					  //printf("Error: FuncPtr `%c' unexpected at line %d of %s\n",*yytext,yyLineNr,yyFileName);
  					}
<EndFuncPtr>")"{BN}*/";"		{ // a variable with extra braces
 					  lineCount();
					  current->type+=funcPtrType.data()+1;
  					  BEGIN(FindMembers);
  					}
<EndFuncPtr>")"{BN}*/"("		{ // a function pointer
  					  lineCount();
					  current->type+=funcPtrType+")";
					  BEGIN(FindMembers);
  					}
<EndFuncPtr>")"{BN}*/"["		{ // an array of variables
  					  lineCount();
					  current->type+=funcPtrType.data();
					  current->args += ")";
					  BEGIN(FindMembers);
  					}
<EndFuncPtr>"("				{ // a function returning a function
  					  current->args += *yytext ;
					  roundCount=0;
					  BEGIN( FuncFunc );
  					}
<EndFuncPtr>"["[^\n\]]*"]"		{
  					  funcPtrType+=yytext;
  					}
<EndFuncPtr>")"				{
  					  BEGIN(FindMembers);
  					}
<FuncFunc>"("				{
  					  current->args += *yytext ;
  					  ++roundCount;
					}
<FuncFunc>")"				{
  					  current->args += *yytext ;
  					  if ( roundCount )
					    --roundCount;
					  else
					  {
					    BEGIN(FuncFuncEnd);
					  }
  					}
<FuncFuncEnd>")"{BN}*"("		{
  					  lineCount();
					  current->type+=funcPtrType+")(";
					  BEGIN(FuncFuncType);
  					}
<FuncFuncEnd>")"{BN}*/[;{]		{
  					  lineCount();
					  current->type+=funcPtrType.data()+1;
  					  BEGIN(Function);
  					}
<FuncFuncEnd>.				{
  					  current->args += *yytext;
  					}
<FuncFuncType>"("			{
  					  current->type += *yytext;
					  roundCount++;
  					}
<FuncFuncType>")"			{
  					  current->type += *yytext;
  					  if (roundCount)
					    --roundCount;
					  else
					    BEGIN(Function);
					}
<FuncFuncType>{BN}*","{BN}*		{ lineCount() ; current->type += ", " ; }
<FuncFuncType>{BN}+			{ lineCount() ; current->type += ' ' ; }
<FuncFuncType>.				{
  					  current->type += *yytext;
  					}
<FindMembers>"("/{BN}*{ID}{BN}*"*"{BN}*{ID}*")(" { // for catching typedef void (__stdcall *f)() like definitions
                                          if (current->type.left(7)=="typedef" && current->bodyLine==-1) 
					    // the bodyLine check is to prevent this guard to be true more than once
					  {
  					    current->bodyLine = yyLineNr;
					    BEGIN( GetCallType );
					  }
					  else if (!current->name.isEmpty()) // normal function
					  {
					    current->args = yytext;
					    current->bodyLine = yyLineNr;
					    currentArgumentContext = FuncQual;
					    fullArgString=current->args.copy();
					    copyArgString=&current->args;
					    BEGIN( ReadFuncArgType ) ;
					    //printf(">>> Read function arguments!\n");
					  }
					}
<GetCallType>{BN}*{ID}{BN}*"*"		{
  					  lineCount();
  					  addType(current);
					  funcPtrType="(";
					  funcPtrType+=yytext;
					  roundCount=0;
					  BEGIN( FuncPtr );
  					}
<FindMembers>"("			{ 
                                          if (!current->name.isEmpty())
					  {
					    current->args = yytext;
					    current->bodyLine = yyLineNr;
					    currentArgumentContext = FuncQual;
					    fullArgString=current->args.copy();
					    copyArgString=&current->args;
					    BEGIN( ReadFuncArgType ) ;
					    //printf(">>> Read function arguments!\n");
					  }
					}
  /*
<FindMembers>"("{BN}*("void"{BN}*)?")"	{
  					  lineCount();
  					  current->args = "()"; 
  					  BEGIN( FuncQual );
  					}
  */

  /*- Function argument reading rules ---------------------------------------*/

<ReadFuncArgType>[^ \/\r\t\n\)\(\"\'#]+ { *copyArgString+=yytext; 
  					  fullArgString+=yytext;
  					}
<CopyArgString>[^\n\\\"\']+		{ *copyArgString+=yytext; 
					  fullArgString+=yytext;
					}
<CopyArgRound>[^\/\n\)\(\"\']+		{ 
  					  *copyArgString+=yytext; 
  					  fullArgString+=yytext;
  					}
<ReadFuncArgType,ReadTempArgs>{BN}*	{
  					  *copyArgString+=" ";
  					  fullArgString+=" ";
  					  lineCount();
  					}
<ReadFuncArgType,CopyArgRound,CopyArgSharp,ReadTempArgs>\"	{
  					  *copyArgString+=*yytext;
  					  fullArgString+=*yytext;
					  lastCopyArgStringContext = YY_START;
  					  BEGIN( CopyArgString );
  					}
<ReadFuncArgType,ReadTempArgs>"("	{
  					  *copyArgString+=*yytext;
  					  fullArgString+=*yytext;
  					  argRoundCount=0; 
					  lastCopyArgContext = YY_START;
					  BEGIN( CopyArgRound ); 
  					}
<ReadFuncArgType>")"			{ 
  					  *copyArgString+=*yytext;
  					  fullArgString+=*yytext;
					  stringToArgumentList(fullArgString,current->argList);
					  BEGIN( currentArgumentContext );
					}
	/* a special comment */
<ReadFuncArgType,ReadTempArgs>("/*"[*!]|"//"[/!])("<"?)	{ 
                                          if (currentArgumentContext==DefineEnd)
					  {
					    // for defines we interpret a comment
					    // as documentation for the define 
					    int i;for (i=yyleng-1;i>=0;i--)
					    {
					      unput(yytext[i]);
					    }
					    stringToArgumentList(fullArgString,current->argList);
					    BEGIN( currentArgumentContext );
					  }
					  else
					  {
					    // for functions we interpret a comment
					    // as documentation for the argument
					    fullArgString+=yytext;
					    lastCopyArgChar=0;
					    if (yytext[1]=='/')
					      BEGIN( CopyArgCommentLine );
					    else
					      BEGIN( CopyArgComment );
					  }
  					}
	/* a non-special comment */
<ReadFuncArgType,ReadTempArgs>"/*"	{
  					  lastCContext = YY_START;
					  BEGIN( SkipComment );
  					}
<ReadFuncArgType,ReadTempArgs>"//"	{
  					  lastCContext = YY_START;
					  BEGIN( SkipCxxComment );
  					}
<ReadFuncArgType,ReadTempArgs>"#"	{
  					  if (! insidePHP)
  					    REJECT;
  					  lastCContext = YY_START;
					  BEGIN( SkipCxxComment );
  					}
	/* `)' followed by a special comment */
<ReadFuncArgType>")"{BN}*("/*"[*!]|"//"[/!])"<"	{
  					  lineCount();
                                          if (currentArgumentContext==DefineEnd)
					  {
					    // for defines we interpret a comment
					    // as documentation for the define 
					    int i;for (i=yyleng-1;i>0;i--)
					    {
					      unput(yytext[i]);
					    }
					    *copyArgString+=*yytext;
					    fullArgString+=*yytext;
					    stringToArgumentList(fullArgString,current->argList);
					    BEGIN( currentArgumentContext );
					  }
					  else
					  {
					    // for functions we interpret a comment
					    // as documentation for the last argument
					    lastCopyArgChar=*yytext;
					    QCString text=&yytext[1];
					    text=text.stripWhiteSpace();
					    fullArgString+=text;
					    if (text.find("//")!=-1)
					      BEGIN( CopyArgCommentLine );
					    else
					      BEGIN( CopyArgComment );
					  }
  					}
<CopyArgComment>^{B}*"*"+/{BN}+		
<CopyArgComment>[^\n\\\@\*]+		{ fullArgString+=yytext; }
<CopyArgComment>"*/"			{ fullArgString+=yytext; 
  					  if (lastCopyArgChar!=0)
					    unput(lastCopyArgChar); 
                                          BEGIN( ReadFuncArgType ); 
					}
<CopyArgCommentLine>\n			{ fullArgString+=yytext;
  					  yyLineNr++;
  					  if (lastCopyArgChar!=0)
					    unput(lastCopyArgChar);
					  BEGIN( ReadFuncArgType );
  					}
<CopyArgCommentLine>[^\\\@\n]+		{ fullArgString+=yytext; }
<CopyArgCommentLine>.			{ fullArgString+=*yytext; }
<CopyArgComment>\n			{ fullArgString+=*yytext; yyLineNr++; }
<CopyArgComment>.			{ fullArgString+=*yytext; }
<ReadTempArgs>"<"			{
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
					  argSharpCount=1;
					  BEGIN( CopyArgSharp );
					}
<ReadTempArgs>">"			{
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
					  //printf("end template list %s\n",copyArgString->data());
					  stringToArgumentList(fullArgString,currentArgumentList);
					  BEGIN( currentArgumentContext );
					}
<CopyArgRound>"("			{
  					  argRoundCount++;
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
  					}
<CopyArgRound>")"			{
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
					  if (argRoundCount>0) 
					    argRoundCount--;
					  else 
					    BEGIN( lastCopyArgContext );
  					}
<CopyArgSharp>"<"			{
  					  argSharpCount++;
					  //printf("argSharpCount++=%d  copy\n",argSharpCount);
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
  					}
<CopyArgSharp>">"			{
  					  *copyArgString+=*yytext;
  					  fullArgString+=*yytext;
					  argSharpCount--;
					  if (argSharpCount>0)
					  {
					    //printf("argSharpCount--=%d copy\n",argSharpCount);
					  }
					  else
					  {
					    BEGIN( ReadTempArgs );
					    //printf("end of argSharpCount\n");
					  }
  					}
<CopyArgString>\\.			{
  					  *copyArgString+=yytext;
  					  fullArgString+=yytext;
  					}
<CopyArgString>\"			{
  					  *copyArgString+=*yytext;
  					  fullArgString+=*yytext;
					  BEGIN( lastCopyArgStringContext );
  					}
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>{CHARLIT}     { 
  					  *copyArgString+=yytext; 
  					  fullArgString+=yytext; 
					}
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgRound,CopyArgSharp>\n  { 
  					  yyLineNr++; 
					  *copyArgString+=*yytext; 
					  fullArgString+=*yytext; 
					}
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgRound,CopyArgSharp>.	  { 
  					  *copyArgString+=*yytext; 
  					  fullArgString+=*yytext; 
					}



  /*------------------------------------------------------------------------*/


<FuncRound>"("				{ current->args += *yytext ;
					  ++roundCount ;
					}
<FuncRound>")"                          { current->args += *yytext ; 
					  if ( roundCount )
					    --roundCount ;
				          else
					    BEGIN( FuncQual ) ;
					}
  /*
<FuncQual>"#"				{ if (insidePHP)
  					    REJECT;
  					  lastCPPContext = YY_START;
  					  BEGIN(SkipCPP);
					}
  */
<FuncQual>[{:;,]                        {
                                          if ( strcmp(yytext,";")==0 && insidePHP && current->type.left(8) != "function" )
                                          {
                                            current->reset();
                                            initEntry();
                                            BEGIN( FindMembers );
                                          }
                                          else
                                          {
                                            unput(*yytext); BEGIN( Function );
                                          }
                                        }
<FuncQual>{BN}*"const"{BN}*       	{ 
  					  lineCount() ; 
  					  current->args += " const "; 
					  current->argList->constSpecifier=TRUE;
					}
<FuncQual>{BN}*"volatile"{BN}*    	{ 
  					  lineCount() ; 
  					  current->args += " volatile "; 
					  current->argList->volatileSpecifier=TRUE;
					}
<FuncQual>{BN}*"="{BN}*"0"{BN}*  	{ 
  					  lineCount() ; 
					  current->args += " = 0"; 
					  current->virt = Pure; 
					  current->argList->pureSpecifier=TRUE;
					}
<FuncRound,FuncFunc>{BN}*","{BN}*	{ 
  					  lineCount() ; 
					  current->args += ", " ; 
					}
<FuncQual,FuncRound,FuncFunc>{BN}+   	{ 
  					  lineCount() ; 
					  current->args += ' ' ; 
					}
<FuncQual,FuncRound,FuncFunc>.		{ current->args += *yytext; }
<FuncQual>{BN}*"try"{BN}+		{ /* try-function-block */ 
					  insideTryBlock=TRUE;
					  lineCount();
					}
<FuncQual>{BN}*"throw"{BN}*"("		{ // C++ style throw clause
  					  current->exception = " throw (" ;
					  roundCount=0;
					  lineCount() ;
					  BEGIN( ExcpRound ) ;
					}
<FuncQual>{BN}*"raises"{BN}*"("         {
  					  current->exception = " raises (" ;
					  lineCount() ;
					  roundCount=0;
					  BEGIN( ExcpRound ) ;
  					}
<FuncQual>{BN}*"throws"{BN}+		{ // Java style throw clause
  					  current->exception = " throws " ;
					  lineCount() ;
					  BEGIN( ExcpList );
  					}
<ExcpRound>"("				{ current->exception += *yytext ;
					  ++roundCount ;
					}
<ExcpRound>")"                          { current->exception += *yytext ; 
					  if ( roundCount )
					    --roundCount ;
				          else
					    BEGIN( FuncQual ) ;
					}
<ExcpRound>.				{
  					  current->exception += *yytext;
  					}
<ExcpList>"{"				{
  					  unput('{'); BEGIN( FuncQual );
  					}
<ExcpList>";"				{
  					  unput(';'); BEGIN( FuncQual );
  					}
<ExcpList>"\n"				{
  					  current->exception += ' ';
					  yyLineNr++;
  					}
<ExcpList>.				{
  					  current->exception += *yytext;
  					}
<Function>"("				{ current->type += current->name ;
					  current->name  = current->args ;
					  current->args  = yytext ;
					  roundCount=0;
					  BEGIN( FuncRound ) ;
					}
<Function>"#"				{ if (insidePHP)
  					    REJECT;
  					  lastCPPContext = YY_START;
  					  BEGIN(SkipCPP);
					}	
<Function>":"				{
  					  if (!insidePHP) BEGIN(SkipInits);
  					}
<Function>[;{,]				{ 
					  current->name=current->name.simplifyWhiteSpace();
  					  current->type=current->type.simplifyWhiteSpace();
  					  current->args=current->args.simplifyWhiteSpace();
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  if (*yytext!=';' || (current_root->section&Entry::COMPOUND_MASK) )
					  {
					    int tempArg=current->name.find('<');
					    QCString tempName;
					    static QRegExp re("operator[^a-z_A-Z0-9]");
					    if (tempArg==-1) tempName=current->name; else tempName=current->name.left(tempArg);
					    if (/*(current->type.isEmpty() && tempName.find(re)==-1) || */
						 current->type.left(8)=="typedef "
					       )
					    {
					      //printf("Scanner.l: found in class variable: `%s' `%s' `%s'\n",
					      //   current->type.data(),current->name.data(),current->args.data());
					      current->section = Entry::VARIABLE_SEC ;
					    }
					    else	      
					    {
					      //printf("Scanner.l: found in class function: `%s' `%s' `%s'\n",
					      //   current->type.data(),current->name.data(),current->args.data());
					      current->section = Entry::FUNCTION_SEC ;
			                      current->proto = *yytext==';';
					    }
					  }
					  else // a global function prototype or function variable
					  {
					    static QRegExp re("([^)]*)");
					    //printf("Scanner.l: prototype? type=`%s' name=`%s' args=`%s'\n",current->type.data(),current->name.data(),current->args.data());
					    if (!current->type.isEmpty() && 
						(current->type.find(re,0)!=-1 || current->type.left(8)=="typedef "))
					    {
					      //printf("Scanner.l: found function variable!\n");
					      current->section = Entry::VARIABLE_SEC;
					    }
					    else
					    {
					      //printf("Scanner.l: found prototype\n");
					      current->section = Entry::FUNCTION_SEC;
					      current->proto = TRUE;
					    }
					  }
					  //printf("Adding entry `%s'\n",current->name.data());
					  if ( insidePHP && current->type.left(8) != "function" )
					  {
					    initEntry();
					  }
					  else
					  {
					    if ( insidePHP && current->type.left(8) == "function" )
					    {
					      current->type = current->type.mid(8);
					    }
					    previous = current;
					    current_root->addSubEntry(current);
					    current = new Entry ;
					    initEntry();
					    lastCurlyContext = FindMembers;
					    if ( *yytext == ',' )
					    {
					      current->type = previous->type.data();
					    }
					    if ( *yytext == '{' )
					    {
					      if ( !insidePHP && (current_root->section & Entry::COMPOUND_MASK) )
					      {
						previous->memSpec |= Entry::Inline;
					      }
					      //addToBody(yytext);
					      curlyCount=0;
					      BEGIN( SkipCurly ) ;
					    }
					    else
					    {
					      if (previous->section!=Entry::VARIABLE_SEC)
						previous->bodyLine=-1; // a function/member declaration
					      BEGIN( FindMembers ) ;
					    }
					  }
                                        }
<SkipInits>"{"				{ 
  				          //addToBody(yytext);
  				          //lastCurlyContext = FindMembers;
					  //curlyCount=0;
  					  //BEGIN( SkipCurly ) ; 
  					  unput('{');
					  BEGIN( Function );
					}
<SkipCurly>"{"				{ 
  				          //addToBody(yytext);
  					  ++curlyCount ; 
					}
<SkipCurly>"}"				{ 
  				          //addToBody(yytext);
  					  if( curlyCount )
					  {
					    --curlyCount ;
					  }
					  else
					  {
					    if (!Config_getBool("HIDE_IN_BODY_DOCS") && 
						!current->doc.isEmpty())
					    {
					      // copy documentation found inside the body
					      // to the previous item
					      previous->doc += current->doc;
					      current->doc.resize(0);
					    }
					    if (current->sli) // copy special list items
					    {
					      QListIterator<ListItemInfo> li(*current->sli);
					      ListItemInfo *lii;
					      for (li.toFirst();(lii=li.current());++li)
					      {
						previous->addSpecialListItem(lii->type,lii->itemId);
					      }
					      delete current->sli;
					      current->sli = 0;
					    }
					    previous->endBodyLine=yyLineNr;
					    BEGIN( lastCurlyContext ) ;
					  }
					}
<SkipCurly>"}"{BN}*("/*!"|"/**"|"//!"|"///")"<" { 
  					  if ( curlyCount )
					  {
					    //addToBody(yytext);
					    --curlyCount ;
					  }
					  else
					  {
					    current->endBodyLine=yyLineNr;
					    lineCount();
					    tempEntry = current; // temporarily switch to the previous entry
					    current = previous;
					    current->doc.resize(0);
					    current->brief.resize(0);
					    lastAfterDocContext = SkipCurlyEndDoc;
					    afterDocTerminator = '}';
					    if (yytext[yyleng-3]=='/')
					    {
					      current->briefLine = yyLineNr;
					      current->briefFile = yyFileName;
					      BEGIN(AfterDocLine);
					    }
					    else if (yytext[yyleng-2]=='*' && Config_getBool("JAVADOC_AUTOBRIEF"))
					    {
					      current->briefLine = yyLineNr;
					      current->briefFile = yyFileName;
					      current->docLine = yyLineNr;
					      current->docFile = yyFileName;
					      BEGIN(AfterDocBrief);
					    }
					    else
					    {
					      current->docLine = yyLineNr;
					      current->docFile = yyFileName;
					      BEGIN(AfterDoc);
					    }
					  }
					}
<SkipCurlyEndDoc>"}"			{
  				          //addToBody("}");
					  current = tempEntry;
  					  BEGIN( lastCurlyContext );
  					}
<SkipCurly>{CHARLIT}                    {
  				          //addToBody(yytext);
  					}
<SkipCurly>\"			        { 
  				          //addToBody(yytext);
  					  lastStringContext=SkipCurly;
  				          BEGIN( SkipString ); 
					}
<SkipCurly>^{B}*"#"			{ 
  				          if (insidePHP)
  				            REJECT;
  				          //addToBody(yytext);
  					  BEGIN( SkipCurlyCpp );
					}
<SkipCurly,SkipInits>\n			{
  					  yyLineNr++;
  				          //addToBody(yytext);
  					}
<SkipCurly,SkipCurlyCpp>[^\n"'\\/{}]+	{
  				          //addToBody(yytext);
  					}
<SkipCurlyCpp>\n			{ 
  				          //addToBody(yytext);
  					  yyLineNr++; 
  					  lastCurlyContext = FindMembers;
  					  BEGIN( SkipCurly ); 
					}
<SkipCurlyCpp>\\[\r]*"\n"[\r]*		{ 
  				          //addToBody(yytext);
  					  yyLineNr++; 
					}
<SkipInits,SkipCurly,SkipCurlyCpp>"/*"	{
  				          //addToBody(yytext);
  					  lastCContext = YY_START;
					  BEGIN(SkipComment);
  					}
<SkipInits,SkipCurly,SkipCurlyCpp>"//"  {
  				          //addToBody(yytext);
  					  lastCContext = YY_START;
					  BEGIN(SkipCxxComment);
  					}
<SkipInits>;	                        {
  					  warn(yyFileName,yyLineNr,
					      "Warning: Found ';' while parsing initializer list! "
					      "(doxygen could be confused by a macro call without semicolon)"
					     );
					  BEGIN( FindMembers );
  					}
<SkipInits,SkipCurly,SkipCurlyCpp>"#"   {
  				          if (! insidePHP)
  				            REJECT;
  				          //addToBody(yytext);
  					  lastCContext = YY_START;
					  BEGIN(SkipCxxComment);
  					}
<SkipInits,SkipCurly,SkipCurlyCpp>.	{
  				          //addToBody(yytext);
  					}
<SkipString>\\.				{
  				          //addToBodyCond(yytext);
  					}
<SkipString>\"				{ 
  				          //addToBodyCond(yytext);
  					  BEGIN( lastStringContext ); 
					}
<SkipString>"/*"|"*/"|"//"		{
  				          //addToBodyCond(yytext);
  					}
<SkipString>\n				{
  					  yyLineNr++;
  				          //addToBodyCond(yytext);
  					}
<SkipString>.				{
  				          //addToBodyCond(yytext);
  					}
<Bases,CompoundName>";"			{ 
					  current->section = Entry::EMPTY_SEC ;
					  current->type.resize(0) ;
					  current->name.resize(0) ;
					  current->args.resize(0) ;
					  current->argList->clear();
					  BEGIN( FindMembers ) ;
					}
<CompoundName>{SCOPENAME}{BN}*/"<"	{
  					  sharpCount = 0;
  					  current->name = yytext ;
					  lineCount();
					  lastClassTemplSpecContext = ClassVar;
					  BEGIN( ClassTemplSpec );
					}
<ClassTemplSpec>">"({BN}*"::"{BN}*{SCOPENAME})?	{
					  current->name += yytext;
					  lineCount();
  					  if (--sharpCount<=0)
					  {
					    current->name = removeRedundantWhiteSpace(current->name);
					    BEGIN( lastClassTemplSpecContext );
					  }
					}
<ClassTemplSpec>"<"			{
  					  current->name += yytext;
  					  sharpCount++;
  					}
<ClassTemplSpec>.			{
  					  current->name += yytext;
					}
<CompoundName>{SCOPENAME}		{ 
					  current->name = yytext ;
					  lineCount();
					  BEGIN( ClassVar );
					}
<CompoundName>{CSSCOPENAME}	        { // C# style scope
					  current->name = substitute(yytext,".","::");
					  lineCount();
					  BEGIN( ClassVar );
                                        }
<ClassVar>{SCOPENAME}{BN}*/"("		{
  					  if (insideIDL && strncmp(yytext,"switch",6)==0 && !isId(yytext[6]))
					  {
					    // Corba IDL style union
					    roundCount=0;
					    BEGIN(SkipUnionSwitch);
					  }
					  else
					  {
  					    addType(current);
					    current->name = yytext;
					    current->name = current->name.stripWhiteSpace();
					    lineCount();
  					    BEGIN( FindMembers );
					  }
  					}
<ClassVar>{ID}				{
  					  if (insideIDL && strcmp(yytext,"switch")==0)
					  {
					    // Corba IDL style union
					    roundCount=0;
					    BEGIN(SkipUnionSwitch);
					  }
					  else if (insideJava && (strcmp(yytext,"implements")==0 || strcmp(yytext,"extends")==0))
					  {
  					    current->type.resize(0);
					    baseProt=Public;
                                            baseVirt=Normal;
					    baseName.resize(0);
					    BEGIN( BasesProt ) ;
					  }
					  else if (insidePHP && (strcmp(yytext,"extends")==0))
					  {
  					    current->type.resize(0);
					    baseProt=Public;
                                            baseVirt=Normal;
					    baseName.resize(0);
					    BEGIN( BasesProt ) ;
					  }
					  else
					  {
					    //if (isTypedef)
					    //{
					    //  //QCString dest = extractName(current->name);
				   	    //  //printf("3>>>>>>>>>> adding %s->%s\n",yytext,current->name.data());
					    //  QCString scope;
					    //  if (current_root->section & Entry::SCOPE_MASK) scope=current_root->name;
                                            //  Doxygen::typedefDict.insert(yytext,new TypedefInfo(current->name,scope));
					    //  //current->extends->append(
					    //  //   new BaseInfo(yytext,Public,Normal)
					    //  //		      );
					    //}
					    current->type += ' ' ;
					    current->type += current->name ;
					    current->name = yytext ;
					    //BEGIN( FindMembers );
					  }
  					}
<ClassVar>[(\[]				{
                                          // probably a function anyway
                                          unput(*yytext);
					  BEGIN( FindMembers );
					}
<ClassVar>":"				{ 
  					  current->type.resize(0);
					  if (current->section == Entry::INTERFACE_SEC || 
					      current->section == Entry::STRUCT_SEC || 
					      insidePHP || insideCS
					     )
					    baseProt=Public;
					  else
					    baseProt=Private;
                                          baseVirt=Normal;
					  baseName.resize(0);
					  BEGIN( BasesProt ) ;
					}
<ClassVar>[;=*&]			{
    					  unput(*yytext);
					  if (isTypedef) // typedef of a class, put typedef keyword back
					  {
					    current->type.prepend("typedef");
					  }
					  BEGIN( FindMembers );
    					}
<CompoundName,ClassVar>{B}*"{"{B}*	{ 
                                          current->fileName = yyFileName ;
					  current->startLine = yyLineNr ;
					  current->name = removeRedundantWhiteSpace(current->name);
					  if (current->name.isEmpty() && !isTypedef) // anonymous compound
					  {
					    current->name.sprintf("@%d",anonCount++);
					  }
					  curlyCount=0;
					  if (current->section==Entry::NAMESPACE_SEC)
					  {
					    BEGIN( ReadNSBody );
					  }
					  else
					  {
					    BEGIN( ReadBody ) ;
					  }
					}
<BasesProt>"virtual"                    { baseVirt = Virtual; }
<BasesProt>"public"                     { baseProt = Public; }
<BasesProt>"protected"                  { baseProt = Protected; }
<BasesProt>"private"                    { baseProt = Private; }
<BasesProt>{BN}				{ lineCount(); }
<BasesProt>.				{ unput(*yytext); BEGIN(Bases); }
<Bases>("::")?{BN}*({ID}{BN}*"::"{BN}*)*{ID}	{ 
  					  //QCString bName = yytext;
					  //bName = bName.stripWhiteSpace();
  					  //bool globalScope = bName.at(0)==':' && baseName.isEmpty();
  					  //if (!globalScope)
  					  //  baseName += bName;
					  //else
					  //  baseName += (bName.data()+2);
  					  baseName+=yytext;
					  current->args += ' ';
					  //if (!globalScope)
					  //  current->args += bName;
					  //else
					  //  current->args += (bName.data()+2);
					  current->args += yytext;
					}
<Bases>{BN}*{ID}("."{ID})*		{ // Java style class
    					  QCString name = substitute(yytext,".","::");
					  baseName += name;
					  current->args += ' ';
					  current->args += name;
    					}
<ClassVar>"<"   	                { current->name += *yytext;
  					  sharpCount=1; 
					  lastSkipSharpContext = YY_START;
					  specName = &current->name;
					  BEGIN ( Specialization );
					}
<Bases>"<"   	       	                { baseName += *yytext;
  					  sharpCount=1; 
					  lastSkipSharpContext = YY_START;
					  specName = &baseName;
					  BEGIN ( Specialization );
					}
<Specialization>"<"			{ *specName += *yytext;
  					  sharpCount++;
  					}
<Specialization>">"			{
  					  *specName += *yytext;
  					  if (--sharpCount<=0)
					    BEGIN(lastSkipSharpContext);
  					}
<Specialization>{BN}+			{ lineCount(); *specName +=' '; }
<Specialization>"<<"			{ *specName += yytext; }
<Specialization>">>"			{ *specName += yytext; }
<Specialization>"typename"{BN}+		{ lineCount(); }
<Specialization>.			{
  					  *specName += *yytext;
  					}
<SkipSharp>"<"				{ ++sharpCount; }
<SkipSharp>">"				{ if (--sharpCount<=0)
					    BEGIN ( lastSkipSharpContext );
					}
<SkipRound>"("				{ ++roundCount; }
<SkipRound>")"				{ if (--roundCount<=0)
					    BEGIN ( lastSkipRoundContext );
					}
<Bases>","|({BN}+"implements"{BN}*)	{ lineCount();
                                          current->args += ',' ; 
					  current->name = removeRedundantWhiteSpace(current->name);
  					  if (!baseName.isEmpty())
  					    current->extends->append(
					      new BaseInfo(baseName,baseProt,baseVirt)
					    );
					  if (current->section == Entry::INTERFACE_SEC || insideJava || insidePHP || insideCS)
					    baseProt=Public;
					  else
					    baseProt=Private;
					  baseVirt=Normal;
					  baseName.resize(0);
					  BEGIN(BasesProt);
					}
<Bases>{B}*"{"{B}*			{ current->fileName = yyFileName ;
					  current->startLine = yyLineNr ;
					  current->name = removeRedundantWhiteSpace(current->name);
  					  if (!baseName.isEmpty())
  					    current->extends->append(
					      new BaseInfo(baseName,baseProt,baseVirt)
					    );
					  curlyCount=0;
					  BEGIN( ReadBody ) ;
					}
<SkipUnionSwitch>{B}*"("		{
  					  roundCount++;
  					}
<SkipUnionSwitch>")"			{
  					  if (--roundCount==0)
					  {
					    BEGIN(ClassVar);
					  }
  					}
<SkipUnionSwitch>\n			{ yyLineNr++; }
<SkipUnionSwitch>.			
<Comment>{BN}+				{ current->program += yytext ;
					  lineCount() ;
					}
<Comment>"/*"				{ current->program += yytext ; } 
<Comment>"//"				{ current->program += yytext ; }
<Comment>[^\n\*]+			{ current->program += yytext ; }
<Comment>"*/"				{ current->program += yytext ;
					  BEGIN( lastContext ) ;
					}
<Comment>.				{ current->program += *yytext ; }

<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,Bases>("//"{B}*)?"/*!" { 
  					  //printf("Start doc block at %d\n",yyLineNr);
					  removeSlashes=(yytext[1]=='/');
					  tmpDocType=-1;
  					  if (YY_START!=SkipCurly)
					  {
					    current->doc.resize(0);
					    current->docLine = yyLineNr;
					    current->docFile = yyFileName;
					  }
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
                                          {
					    current->inside = current_root->name+"::";
                                            if (current->mGrpId!=NOGROUP)
                                            {
                                              memberGroupInside = current->inside.copy();
                                            }
                                          }
					  BEGIN( Doc );
					}
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator,ClassVar,SkipInits,Bases>("//"{B}*)?"/**"/[^/*] {
  					  removeSlashes=(yytext[1]=='/');
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
                                          {
					    current->inside = current_root->name+"::";
                                            if (current->mGrpId!=NOGROUP)
                                            {
                                              memberGroupInside = current->inside.copy();
                                            }
                                          }
					  if (!Config_getBool("JAVADOC_AUTOBRIEF")) // use the Qt style
					  {
					    tmpDocType=-1;
					    if (!Config_getBool("HIDE_IN_BODY_DOCS") && 
						YY_START==SkipCurly) // inside body
					    {
					      current->doc+="\n\n";
					    }
					    else
					    {
  					      current->doc.resize(0);
					      current->docLine = yyLineNr;
					      current->docFile = yyFileName;
					    }
					    BEGIN( Doc );
					  }
					  else // Use the javadoc style
					  {
					    if (!Config_getBool("HIDE_IN_BODY_DOCS") &&
						YY_START==SkipCurly) // inside body
					    {
					      tmpDocType=-1;
					      current->doc+="\n\n";
					      lastDocContext = SkipCurly;
					      BEGIN( Doc );
					    }
					    else
					    {  
					      tmpDocType=Doc;
					      current->doc.resize(0);
					      current->brief.resize(0);
					      current->docLine = yyLineNr;
					      current->docFile = yyFileName;
					      current->briefLine = yyLineNr;
					      current->briefFile = yyFileName;
					      BEGIN( JavaDoc );
					    }
					  }
  					}
<FindMembers,FindFields,MemberSpec,SkipCurly,FuncQual,Operator,ClassVar,Bases>"//!" { 
                                          if (YY_START!=SkipCurly)
					  {
  					    current->brief.resize(0);
					    current->briefFile=yyFileName;
					    current->briefLine=yyLineNr;
					  }
					  tmpDocType=-1;
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
                                          {
					    current->inside = current_root->name+"::";
                                            if (current->mGrpId!=NOGROUP)
                                            {
                                              memberGroupInside = current->inside.copy();
                                            }
                                          }
					  BEGIN( LineDoc );
					}
<FindMembers,FindFields,MemberSpec,SkipCurly,FuncQual,Operator,ClassVar,Bases>"///"/[^/] { 
                                          if (YY_START!=SkipCurly)
					  {
  					    current->brief.resize(0);
					    current->briefFile=yyFileName;
					    current->briefLine=yyLineNr;
					  }
					  tmpDocType=-1;
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
                                          {
					    current->inside = current_root->name+"::";
                                            if (current->mGrpId!=NOGROUP)
                                            {
                                              memberGroupInside = current->inside.copy();
                                            }
                                          }
					  BEGIN( LineDoc );
					}
<FindMembers>"extern"{BN}+"\"C"("++")?"\""{BN}*("{")?  {
					  lineCount();
					}
<FindMembers>"{"			{
                                          if (insideCS && !current->name.isEmpty() && !current->type.isEmpty())
					  {
					    if (current->type.left(6)=="event " || current->type.find("event")!=-1) // event
					    {
  					      current->mtype = mtype = Event;
					    }
					    else // property
					    {				
  					      current->mtype = mtype = Property;
					    }
					    current->bodyLine = yyLineNr;
					    curlyCount=0;
					    BEGIN(CSAccessorDecl);
					  }
					  else
					  {
					    if ((insideJava || insideCS) && 
						/*current->stat && */
						current->name.isEmpty() && 
						current->type.isEmpty()
					       )
					    {
					      // static Java initializer
					      needsSemi = FALSE;
					      if (current->stat)
					      {
						current->name="[static initializer]";
					      }
					      else
					      {
						current->name="[instance initializer]";
					      }
                                              unput(*yytext); 
					      BEGIN( Function );
					    }
					    else
					    {
					      needsSemi = TRUE;
					      current->type.resize(0);
					      current->name.resize(0);
					      current->args.resize(0);
					      current->argList->clear();
					      curlyCount=0;
					      BEGIN( SkipCurlyBlock );
					    }
					  }
  					}
<CSAccessorDecl>"{"			{ curlyCount++; }
<CSAccessorDecl>"}"			{ 
                                          if (curlyCount) 
                                          {
                                            curlyCount--; 
					  }
					  else
					  {
                                            unput(';');
					    BEGIN(FindMembers);
					  }
                                        }
<CSAccessorDecl>"set"			{ if (curlyCount==0) current->memSpec |= Entry::Settable; }
<CSAccessorDecl>"get"			{ if (curlyCount==0) current->memSpec |= Entry::Gettable; }
<CSAccessorDecl>.			{}
<CSAccessorDecl>\n			{ yyLineNr++; }
<JavaDoc>{CMD}("brief"|"short"){B}+	{
  					  lastBriefContext=tmpDocType;
  					  BEGIN( ClassDocBrief ); 
					}
<JavaDoc>^(({B}*"*"+)?){BL}		{
					  lineCount();
					  if (!current->brief.stripWhiteSpace().isEmpty())
					  {
					    BEGIN( tmpDocType );
					  }
 					} 
  /*
<JavaDoc>"@"				{
  					  unput(*yytext);
					  BEGIN(ClassDoc);
  					}
  */
<JavaDoc>^{B}*"*"+/[^/]			{
  					  //printf("---> removing %s\n",yytext);
  					}
  /*
<JavaDoc>[^\n\@\*\.\\]+			{
  					  current->brief+=yytext;
  					}
  */
<JavaDoc>.				{
  				          //printf("---> copy %c\n",*yytext);
  					  current->brief+=*yytext;
  					}
<JavaDoc>\n				{
  					  current->brief+=' ';
					  lineCount();
  					}
<JavaDoc,AfterDocBrief>".\\"/[ \t\r\n]		{
					  current->brief+=".";
					}
<JavaDoc>"."[ \t\r\n]			{
  					  lineCount();
					  current->brief+=".";
					  BEGIN( tmpDocType );
					}
<JavaDoc>{B}*/{SECTIONCMD}		{ 
  					  current->doc+=yytext;
					  BEGIN( tmpDocType );
  					}
<JavaDoc>"<"({TABLE}|{UL}|{OL}|{DL}|{P}){ATTR}">"	{  // end brief upon encountering any of these
					  int i;
					  for (i=yyleng-1;i>=0;i--)
					  {
					    unput(yytext[i]);
					  }
					  BEGIN( tmpDocType );
  					}
<Doc,JavaDoc>{B}*{CMD}("fn"|"var"|"typedef"){B}+	{
					  current->section = Entry::MEMBERDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( ClassDocFunc ); 
					}
<Doc,JavaDoc>{B}*{CMD}"def"{B}+	{
  					  nextDefContext = YY_START==LineDoc ? DefLineDoc : ClassDoc;
  					  current->section = Entry::DEFINEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( ClassDocDefine );
  					}
<LineDoc,Doc,JavaDoc>{B}*{CMD}"overload"{B}* { 
  					  overloadContext = YY_START;
  					  BEGIN( ClassDocOverload ); 
					}
<ClassDocOverload>{B}*/"\n"	        {
					  QCString orgDoc = current->doc;
					  current->doc = getOverloadDocs();
					  current->doc += "\n\n";
					  current->doc += orgDoc;
  					  BEGIN( overloadContext  );
  				        }
<ClassDocOverload>{B}*/"*/"		{
					  QCString orgDoc = current->doc;
					  current->doc = getOverloadDocs();
					  current->doc += "\n\n";
					  current->doc += orgDoc;
  					  BEGIN( overloadContext );
  					}
<ClassDocOverload>.                     { unput(*yytext);
  					  current->section = Entry::OVERLOADDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocFunc ); 
					}
<Doc,JavaDoc>{B}*{CMD}"enum"{B}+	{
  					  current->section = Entry::ENUMDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( EnumDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"defgroup"{B}+ {
  					  current->section = Entry::GROUPDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  current->groupDocType = Entry::GROUPDOC_NORMAL;
					  BEGIN( GroupDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"addtogroup"{B}+ {
  					  current->section = Entry::GROUPDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  current->groupDocType = Entry::GROUPDOC_ADD;
					  BEGIN( GroupDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"weakgroup"{B}+ {
  					  current->section = Entry::GROUPDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  current->groupDocType = Entry::GROUPDOC_WEAK;
					  BEGIN( GroupDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"namespace"{B}+	{
  					  current->section = Entry::NAMESPACEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( NameSpaceDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"package"{B}+	{
  					  current->section = Entry::PACKAGEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( PackageDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"class"{B}+  	{
  					  current->section = Entry::CLASSDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"union"{B}+  	{
  					  current->section = Entry::UNIONDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"struct"{B}+ 	{
  					  current->section = Entry::STRUCTDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"interface"{B}+ 	{
  					  current->section = Entry::INTERFACEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"idlexcept"{B}+ 	{
  					  current->section = Entry::EXCEPTIONDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"page"{B}+   	{
  					  current->section = Entry::PAGEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( PageDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"mainpage"{B}* 	{
  					  current->section = Entry::MAINPAGEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  current->name = "mainpage";
					  BEGIN( PageDocArg2 );
  					}
<Doc,JavaDoc>{B}*{CMD}"file"{B}*	{
  					  current->section = Entry::FILEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( FileDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"example"{B}+ 	{
 					  current->section = Entry::EXAMPLE_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
 					  BEGIN( ExampleDocArg1 );
					}
<LineDoc>{CMD}"name"[^\n]*\n		{
  					  lastDefGroup.groupname.resize(0);
  					  memberGroupHeader=&yytext[5];
					  memberGroupHeader=memberGroupHeader.stripWhiteSpace(); 
					  current->section = Entry::MEMBERGRP_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  yyLineNr++;  
					  startGroupInDoc();
					  BEGIN( lastDocContext );
  					}
<Doc,JavaDoc>{CMD}"name"{B}+ 		{
  					  lastDefGroup.groupname.resize(0);
					  current->section = Entry::MEMBERGRP_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  memberGroupHeader.resize(0);
					  memberGroupDocs.resize(0);
					  BEGIN(GroupHeader);
  					}
<LineDoc,Doc,JavaDoc,ClassDoc,PageDoc>"<!--"	{ 
					  lastSkipHtmlCommentContext = YY_START;
					  BEGIN(SkipHtmlComment); 
					}
<SkipHtmlComment>"--"[!]?">"		{ BEGIN(lastSkipHtmlCommentContext); }
<SkipHtmlComment>.
<AfterDoc,Doc,ClassDoc,PageDoc>("\\\\"|"@@")("todo"|"test"|"bug"|"deprecated"|"xrefitem")/[^a-z_A-Z0-9] {
  					  current->doc+=yytext;
  					}
<AfterDocLine,LineDoc,JavaDoc>("\\\\"|"@@")("todo"|"test"|"bug"|"deprecated"|"xrefitem")/[^a-z_A-Z0-9]	{
  					  current->brief+=yytext;
  					}
<AfterDoc,AfterDocLine,LineDoc,Doc,JavaDoc,ClassDoc,PageDoc>{CMD}"todo"/[^a-z_A-Z0-9]	{
  					  slStartContext = YY_START;
					  lastBriefContext = TodoParam; // this is where we will continue at the end of the argument
					  slString = current->brief.copy(); // these will be swapped later on.
					  current->brief.resize(0);
					  BEGIN(ClassDocBrief);
  					}
<AfterDoc,AfterDocLine,LineDoc,Doc,JavaDoc,ClassDoc,PageDoc>{CMD}"test"/[^a-z_A-Z0-9]	{
  					  slStartContext = YY_START;
					  lastBriefContext = TestParam; // this is where we will continue at the end of the argument
					  slString = current->brief.copy(); // these will be swapped later on.
					  current->brief.resize(0);
					  BEGIN(ClassDocBrief);
  					}
<AfterDoc,AfterDocLine,LineDoc,Doc,JavaDoc,ClassDoc,PageDoc>{CMD}"bug"/[^a-z_A-Z0-9]	{
  					  slStartContext = YY_START;
					  lastBriefContext = BugParam; // this is where we will continue at the end of the argument
					  slString = current->brief.copy(); // these will be swapped later on.
					  current->brief.resize(0);
					  BEGIN(ClassDocBrief);
  					}
<AfterDoc,AfterDocLine,LineDoc,Doc,JavaDoc,ClassDoc,PageDoc>{CMD}"deprecated"/[^a-z_A-Z0-9]	{
  					  slStartContext = YY_START;
					  lastBriefContext = DeprecatedParam; // this is where we will continue at the end of the argument
					  slString = current->brief.copy(); // these will be swapped later on.
					  current->brief.resize(0);
					  BEGIN(ClassDocBrief);
  					}
<AfterDoc,AfterDocLine,LineDoc,Doc,JavaDoc,ClassDoc,PageDoc>{CMD}"xrefitem"/[^a-z_A-Z0-9]	{
  					  slStartContext = YY_START;
					  lastBriefContext = XRefItemParam4; // this is where we will continue at the end of the argument
					  slString = current->brief.copy(); // these will be swapped later on.
					  current->brief.resize(0);
					  BEGIN(XRefItemParam1);
  					}
<TodoParam>\n                           |
<TodoParam>"//"                         |
<TodoParam>"/*"                         |
<TodoParam>.				{
                                          addXRefItem("todo",theTranslator->trTodo(),theTranslator->trTodoList());
					  int i;for (i=yyleng-1;i>=0;i--) unput(yytext[i]);
					  BEGIN(slStartContext);
  					}
<TestParam>\n                           |
<TestParam>"//"                         |
<TestParam>"/*"                         |
<TestParam>.				{
                                          addXRefItem("test",theTranslator->trTest(),theTranslator->trTestList());
					  int i;for (i=yyleng-1;i>=0;i--) unput(yytext[i]);
					  BEGIN(slStartContext);
  					}
<BugParam>\n                            |
<BugParam>"//"                          |
<BugParam>"/*"                          |
<BugParam>.				{
                                          addXRefItem("bug",theTranslator->trBug(),theTranslator->trBugList());
					  int i;for (i=yyleng-1;i>=0;i--) unput(yytext[i]);
					  BEGIN(slStartContext);
  					}
<DeprecatedParam>\n                     |
<DeprecatedParam>"//"                   |
<DeprecatedParam>"/*"                   |
<DeprecatedParam>.		        {
                                          addXRefItem("deprecated",theTranslator->trDeprecated(),theTranslator->trDeprecatedList());
					  int i;for (i=yyleng-1;i>=0;i--) unput(yytext[i]);
					  BEGIN(slStartContext);
  					}
<XRefItemParam1>{ID}			{
  					  xrefItemKey=yytext;
					  BEGIN(XRefItemParam2);
					}
<XRefItemParam1>{B}*	                
<XRefItemParam1>.			{
  					  warn(yyFileName,yyLineNr,"Found unexpected character %s while parsing the first argument of \\xrefitem\n",yytext);
					  unput(*yytext);
					  BEGIN(slStartContext);
  					}
<XRefItemParam2>"\""[^\n\"]*"\""	{
  					  xrefItemTitle = stripQuotes(yytext);
					  BEGIN(XRefItemParam3);
  					}
<XRefItemParam2>{B}*
<XRefItemParam2>.			{
  					  warn(yyFileName,yyLineNr,"Found unexpected character %s while parsing the second argument of \\xrefitem\n",yytext);
					  unput(*yytext);
					  BEGIN(slStartContext);
  					}
<XRefItemParam3>{B}*
<XRefItemParam3>"\""[^\n\"]*"\""	{
  					  xrefListTitle = stripQuotes(yytext);
					  BEGIN(ClassDocBrief);
  					}
<XRefItemParam3>.			{
  					  warn(yyFileName,yyLineNr,"Found unexpected character %s while parsing the third argument of \\xrefitem\n",yytext);
					  unput(*yytext);
					  BEGIN(slStartContext);
  					}
<XRefItemParam4>{B}*
<XRefItemParam4>\n                      |
<XRefItemParam4>"//"                    |
<XRefItemParam4>"/*"                    |
<XRefItemParam4>.		        {
                                          addXRefItem(xrefItemKey,xrefItemTitle,xrefListTitle);
					  int i;for (i=yyleng-1;i>=0;i--) unput(yytext[i]);
					  BEGIN(slStartContext);
  					}
<ExampleDocArg1>{FILE}			{
				          current->name = stripQuotes(yytext);	
					  BEGIN( ExampleDoc );
					}
<ClassDoc,Doc,JavaDoc>{B}*{CMD}"relate"[sd]{B}*    { 
  					  lastDocRelContext = YY_START;
  					  BEGIN( ClassDocRelates ); 
					}
<ClassDocRelates>({ID}"::")*{ID}	{ 
  					  current->relates = yytext;
                                          if (current->mGrpId!=NOGROUP) 
                                          {
                                            memberGroupRelates = yytext;
                                          }
					  BEGIN( lastDocRelContext );
					}
<NameSpaceDocArg1>{SCOPENAME}		{
  					  current->name = yytext;
					  newDocState();
  					}
<NameSpaceDocArg1>"\\"{B}*"\n"		{ 
                                          yyLineNr++; 
                                        }
<NameSpaceDocArg1>"\n"			{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after "
					       "\\namespace."
                                              );
  					  yyLineNr++;
  					}
<PackageDocArg1>{ID}("."{ID})*		{
  					  current->name = yytext;
					  newDocState();
  					}
<PackageDocArg1>"\\"{B}*"\n"		{ 
                                          yyLineNr++; 
                                        }
<PackageDocArg1>"\n"			{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after "
					       "\\package."
                                              );
  					  yyLineNr++;
  					}
<ClassDocArg1>{SCOPENAME}/"<"		{
					  current->name = yytext;
					  // prepend outer scope name 
					  prependScope();
					  lastClassTemplSpecContext = ClassDocArg2;
					  BEGIN( ClassTemplSpec );
  					}
<ClassDocArg1>{SCOPENAME}		{
					  current->name = yytext;
					  // prepend outer scope name 
					  prependScope();
					  BEGIN( ClassDocArg2 );
					}
<ClassDocArg1>"\\"{B}*"\n"		{ 
                                          yyLineNr++; 
                                        }
<ClassDocArg1>"\n"			{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after "
					       "\\class."
                                              );
  					  yyLineNr++;
					  
  					}
<GroupDocArg1>{ID}(".html"?)		{ 
  					  current->name = yytext;
					  lastDefGroup.groupname = yytext;
					  lastDefGroup.pri = current->groupingPri();
  					  // the .html stuff is for Qt compatibility
					  if (current->name.right(5)==".html") 
					    current->name=current->name.left(current->name.length()-5);
					  BEGIN(GroupDocArg2);
  					}
<GroupDocArg1>"\\"{B}*"\n"		{ yyLineNr++; 
                                        }
<GroupDocArg1>"\n"			{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing group name after %s",
					       current->groupDocCmd()
                                              );
					  yyLineNr++;
					  BEGIN( Doc );
  					}
<GroupDocArg2>{B}*"*/"			{
                                          // fake input for end of title rule
                                          yyLineNr--;
  					  unput('/');unput('*');unput('\n');
  					}
<GroupDocArg2>"\\"{B}*"\n"		{ yyLineNr++; 
                                        }
<GroupDocArg2>[^\n\*]+			{
					  current->type += yytext;
					  current->type = current->type.stripWhiteSpace();
  					}
<GroupDocArg2>"\n"			{
                                          if( current->groupDocType == Entry::GROUPDOC_NORMAL &&
                                              current->type.length() == 0 )
  					    warn(yyFileName,yyLineNr,
                                                 "Warning: missing title after "
					         "\\defgroup %s", current->name.data()
                                                );
  					  yyLineNr++; 
 					  newDocState();
  					}
<ClassDocArg2>{FILE}			{
  					  //printf("ClassDocArg2=%s\n",yytext);
					  current->includeFile = stripQuotes(yytext);
					  BEGIN( ClassDocArg3 );
					}
<ClassDocArg2>"\\"{B}*"\n"		{ yyLineNr++; 
                                        }
<ClassDocArg2>"\n"			{ yyLineNr++; 
					  newDocState();
					}
<ClassDocArg2>{B}*"*/"			{
					  newDocState();
  					  unput('/');unput('*');
  					}
<ClassDocArg3>[<]?{FILE}[>]?		{
  					  //printf("ClassDocArg3=%s\n",yytext);
 					  current->includeName = yytext;
  					  newDocState();
					}
<ClassDocArg3>"\\"{B}*"\n"		{ yyLineNr++;
                                        }
<ClassDocArg3>"\n"			{ yyLineNr++; 
  					  newDocState();
					}
<ClassDocArg3>{B}*"*/"			{
					  newDocState();
  					  unput('/');unput('*');
  					}
<FileDocArg1>{FILE}			{
  					  current->name = stripQuotes(yytext);
					  newDocState();
  				        }
<FileDocArg1>"\\"{B}*"\n"		{ yyLineNr++;
                                        }
<FileDocArg1>"\n"			{
  					  current->name = yyFileName;
  					  yyLineNr++;
					  newDocState();
  					}
<PageDocArg1>{FILE}			{
					  current->name = stripQuotes(yytext);
					  BEGIN( PageDocArg2 ); 
					}
<PageDocArg1>"\\"{B}*"\n"		{ yyLineNr++; 
					  current->doc+="\n";
                                        }
<PageDocArg1>"\n"			{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after "
					       "\\page."
                                              );
					  current->doc+="\n";
  					  yyLineNr++;
					  BEGIN( Doc );
  					}
<PageDocArg2>.*"\n"			{
  				          yyLineNr++;
					  current->args = yytext;
					  current->doc+="\n";
					  BEGIN( PageDoc );
					}
<EnumDocArg1>{SCOPEID}			{
  					  current->name = yytext;
					  prependScope();
  					  newDocState();
  					}
<EnumDocArg1>"\\"{B}*"\n"		{ yyLineNr++;
					  current->doc+="\n";
                                        }
<EnumDocArg1>"\n"			{
  					  warn(yyFileName,yyLineNr,
                                               "Warning: missing argument after \\enum."
                                              );
					  current->doc+="\n";
  					  yyLineNr++;
					  BEGIN( Doc );
  					}
<PageDoc>{CMD}"refitem".*"\n"		{
  					  current->doc+=yytext;
  					}
<PageDoc>{CMD}"section"{B}+		{
  					  //sectionType=SectionInfo::Section;
					  current->doc+=yytext;
  					  BEGIN(SectionLabel);
  					}
<PageDoc>{CMD}"subsection"{B}+		{
  					  //sectionType=SectionInfo::Subsection;
					  current->doc+=yytext;
  					  BEGIN(SectionLabel);
  					}
<PageDoc>{CMD}"subsubsection"{B}+	{
  					  //sectionType=SectionInfo::Subsubsection;
					  current->doc+=yytext;
  					  BEGIN(SectionLabel);
  					}
<PageDoc>{CMD}"paragraph"{B}+		{
  					  //sectionType=SectionInfo::Paragraph;
					  current->doc+=yytext;
  					  BEGIN(SectionLabel);
  					}
<GroupHeader>.				{ memberGroupHeader+=*yytext; }
<GroupHeader>"*/"			{
  					  unput('/');unput('*');
					  //printf("Found memberGroup=`%s'\n",memberGroupHeader.data());
					  startGroupInDoc();
					  newDocState();
  					}
<GroupHeader>\n				{
  					  yyLineNr++;
					  current->doc+="\n";
					  //printf("Found memberGroup=`%s'\n",memberGroupHeader.data());
					  startGroupInDoc();
					  newDocState();
  					}
<StoreGroupDocs>"$"			{
  					  //printf("StoreGroupDocs memberGroupId=%d brief=`%s' doc=`%s'!\n",memberGroupId,current->brief.data(),current->doc.data());
					  memberGroupDocs=current->brief.stripWhiteSpace();
					  current->doc = current->doc.stripWhiteSpace();
					  if (!memberGroupDocs.isEmpty() && !current->doc.isEmpty())
					  {
					    memberGroupDocs+="\n\n";
					  }
					  memberGroupDocs+=current->doc;
					  //Doxygen::memberDocDict.insert(memberGroupId,
					  //    new QCString(memberGroupDocs)
					  //    );
					  MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(memberGroupId);
					  if (info) 
					  {
					    info->doc = memberGroupDocs;
					    info->docFile = yyFileName;
					  }
					  current->doc.resize(0);
					  current->brief.resize(0);
					  BEGIN(lastDocContext);
  					}
<ExampleDoc,Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"anchor"{B}+ {
  					  lastAnchorContext = YY_START;
  					  //sectionType=SectionInfo::Anchor;
					  current->doc+=yytext;
					  BEGIN(AnchorLabel);
  					}
<Doc,PageDoc,ClassDoc>("\\\\"|"@@")("verbatim"|"latexonly"|"htmlonly")/[^a-z_A-Z0-9] {
					  current->doc+="\\\\verbatim";
					}
<JavaDoc>("\\\\"|"@@")("verbatim"|"latexonly"|"htmlonly")/[^a-z_A-Z0-9] {
					  current->brief+="\\\\verbatim";
					}
<Doc,PageDoc,ClassDoc>{CMD}"verbatim"/[^a-z_A-Z0-9] {
					  lastVerbState=YY_START;
					  current->doc+="\\verbatim";
  					  BEGIN(SkipVerbatim);
  					}
<JavaDoc>{CMD}"verbatim"/[^a-z_A-Z0-9] 	{
					  lastVerbState=YY_START;
					  current->brief+="\\verbatim";
  					  BEGIN(SkipVerbatim);
  					}
<Doc,PageDoc,ClassDoc>{CMD}"latexonly"/[^a-z_A-Z0-9] {
					  lastVerbState=YY_START;
					  current->doc+="\\latexonly";
  					  BEGIN(SkipVerbatim);
  					}
<JavaDoc>{CMD}"latexonly"/[^a-z_A-Z0-9]	{
					  lastVerbState=YY_START;
					  current->brief+="\\latexonly";
  					  BEGIN(SkipVerbatim);
  					}
<Doc,PageDoc,ClassDoc>{CMD}"htmlonly"/[^a-z_A-Z0-9] {
					  lastVerbState=YY_START;
					  current->doc+="\\htmlonly";
  					  BEGIN(SkipVerbatim);
  					}
<JavaDoc>{CMD}"htmlonly"/[^a-z_A-Z0-9] 	{
					  lastVerbState=YY_START;
					  current->brief+="\\htmlonly";
  					  BEGIN(SkipVerbatim);
  					}
<Doc,PageDoc,ClassDoc>{CMD}"addindex"{B}+[^\n]+ {
  					  current->doc+=yytext;
  					}
<JavaDoc>{CMD}"addindex"{B}+[^\n]+ 	{
  					  current->brief+=yytext;
  					}
<Doc,PageDoc,ClassDoc>("\\\\"|"@@")"code"/[^a-z_A-Z0-9] {
  					  current->doc+="\\\\code";
  					}
<JavaDoc>("\\\\"|"@@")"code"/[^a-z_A-Z0-9] {
  					  current->brief+="\\\\code";
  					}
<Doc,PageDoc,ClassDoc>{CMD}"code"/[^a-z_A-Z0-9] {
  					  lastCodeState=YY_START;
					  current->doc+="\\code";
					  pSkipDoc=&current->doc;
					  BEGIN(SkipCode);
					}
<JavaDoc>{CMD}"code"/[^a-z_A-Z0-9] 	{
  					  lastCodeState=YY_START;
					  current->brief+="\\code";
					  pSkipDoc=&current->brief;
					  BEGIN(SkipCode);
					}
<Doc,PageDoc,ClassDoc>"<"{PRE}{ATTR}">" {
  					  lastCodeState=YY_START;
					  current->doc+="<PRE>";
					  pSkipDoc=&current->doc;
					  BEGIN(SkipCode);
  					}
<JavaDoc>"<"{PRE}{ATTR}">" 		{
  					  lastCodeState=YY_START;
					  current->brief+="<PRE>";
					  pSkipDoc=&current->brief;
					  BEGIN(SkipCode);
  					}
<SkipVerbatim>{CMD}("endverbatim"|"endlatexonly"|"endhtmlonly")/[^a-z_A-Z0-9] {
  					  current->doc+=yytext;
  					  BEGIN(lastVerbState);
  					}
<SkipVerbatim>[^ \t\/\@\\\n]*		{
  					  current->doc+=yytext;
  					}
<SkipVerbatim>^"//"			{
  					  if (!removeSlashes)
					    current->doc+=yytext;
  					}
  /*
<SkipVerbatim>^"//"({B}*"*"+)?		{
  					  if (!removeSlashes)
					    current->doc+=yytext;
  					}
<SkipVerbatim>^{B}*"*"+			
  */
<SkipVerbatim>"//"|"/*"			{ 
  					  current->doc+=yytext; 
  					}
<SkipVerbatim>"\n"			{
  					  yyLineNr++;
					  current->doc+=*yytext;
  					}
<SkipVerbatim>.				{
  					  current->doc+=*yytext;
  					}
<SkipCode>{CMD}"endcode"		{
  					  *pSkipDoc+="\\endcode";
					  BEGIN(lastCodeState);
  					}
<SkipCode>"</"{PRE}{ATTR}">"		{
  					  *pSkipDoc+="</PRE>";
					  BEGIN(lastCodeState);
  					}
<SkipCode>^"//"({B}*"*"+)?		{
  					  if (!removeSlashes)
					    *pSkipDoc+=yytext;
  					}
<SkipCode>^{B}*"*"+/{BN}+		
<SkipCode>"//"				{
  					  *pSkipDoc+=yytext;
  					}
<SkipCode>"/*"|"*/"			{
  					  *pSkipDoc+=yytext;
  					}
<SkipCode>[^ \<\*\t\/\\\n]+		{
  					  *pSkipDoc+=yytext;
  					}
<SkipCode>\n				{
  					  yyLineNr++;
					  *pSkipDoc+=*yytext;
  					}
<SkipCode>.				{
  					  *pSkipDoc+=*yytext;
  					}
<AnchorLabel>{LABELID}			{
  					  //sectionLabel=yytext;
					  //addSection();
					  //current->doc += "\\anchor "+sectionLabel+" ";
                                          current->doc+=yytext;
					  BEGIN(lastAnchorContext);
  					}
<SectionLabel>{LABELID}			{
  					  //sectionLabel=yytext;
					  sectionTitle.resize(0);
                                          current->doc+=yytext;
					  BEGIN(SectionTitle);
  					}
<SectionTitle>[^\n*]*/"\n"		{
					  sectionTitle+=yytext;
					  sectionTitle=sectionTitle.stripWhiteSpace();
					  //current->doc += "\\section "+sectionLabel+" ";
					  //addSection();
                                          current->doc+=yytext;
					  BEGIN(PageDoc);
  					}
<SectionTitle>[^\n*]*			{
  					  sectionTitle+=yytext;
                                          current->doc+=yytext;
  					}
<SectionTitle>"*"			{
  					  sectionTitle+=yytext;
                                          current->doc+=yytext;
  					}
<ExampleDoc,PageDoc,ClassDoc>"\n"			{ yyLineNr++ ; current->doc+=yytext; }
<ExampleDoc,PageDoc,ClassDoc>[a-z_A-Z0-9 \t]+		{ current->doc += yytext; }
<ExampleDoc,PageDoc>{CMD}"ingroup"{B}+			{
  							  lastGroupContext = YY_START;
							  lineCount();
							  BEGIN( GroupName );
  							}
<ClassDoc,Doc,JavaDoc>{CMD}"{"				{
                                                          if (memberGroupId==NOGROUP && current->section==Entry::GROUPDOC_SEC)
							  {
							    startGroupInDoc();
							  }
  							}
<ClassDoc,Doc,JavaDoc>{CMD}"}"				{
                                                          if (memberGroupId==NOGROUP && autoGroupStack.isEmpty())
                                                          {
                                                             warn(yyFileName,yyLineNr,
	                                                          "Warning: end of group without matching begin.");
                                                          }
                                                          //printf("end of member group marker ends group %d\n",memberGroupId);
  							  endGroup();
                                                          memberGroupHeader.resize(0);
  							}
<ExampleDoc,PageDoc,ClassDoc>.    	 	        { current->doc += yytext; }
<Doc,JavaDoc,LineDoc,ExampleDoc,PageDoc,ClassDoc>^{B}*"//" 
<Doc,ExampleDoc,PageDoc,ClassDoc>"//"  			{ current->doc += yytext; }
<LineDoc,JavaDoc,ClassDocBrief>"//"			{ current->brief += yytext; }
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc,AfterDoc,AfterDocLine,AfterDocBrief>("\\\\"|"@@")"f"[$\[\]] {
							  current->doc += yytext; 
							}
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc,AfterDoc,AfterDocLine,AfterDocBrief>{CMD}"f$"	{ 
  							  lastFormulaContext = YY_START;
  							  formulaText="$"; 
							  insideFormula=TRUE;
							  BEGIN(ReadFormulaShort); 
							}
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc,AfterDoc,AfterDocLine,AfterDocBrief>{CMD}"f["	{ 
  							  lastFormulaContext = YY_START;
  							  formulaText="\\["; 
							  insideFormula=TRUE;
							  BEGIN(ReadFormulaLong); 
							}
<ReadFormulaShort>{CMD}"f$"				{
  							  formulaText+="$";
							  if (lastFormulaContext==ClassDocBrief || 
							      lastFormulaContext==LineDoc || 
							      lastFormulaContext==JavaDoc || 
							      lastFormulaContext==AfterDocBrief ||
							      lastFormulaContext==AfterDocLine
							     )
							    current->brief += addFormula();
							  else
							    current->doc += addFormula();
							  insideFormula=FALSE;
							  BEGIN(lastFormulaContext);
  							}
<ReadFormulaShort>\n					{
  							  formulaText+=" ";
							  yyLineNr++;
							  if (lastFormulaContext==LineDoc || 
							      lastFormulaContext==AfterDocLine
							     )
							  {
							    checkFormula();
							    insideFormula=FALSE;
							    BEGIN(lastFormulaContext);
							  }
  							}
<ReadFormulaLong>{CMD}"f]"					{
							  formulaText+="\\]";
							  if (lastFormulaContext==ClassDocBrief ||
							      lastFormulaContext==LineDoc ||
							      lastFormulaContext==JavaDoc ||
							      lastFormulaContext==AfterDocBrief ||
							      lastFormulaContext==AfterDocLine
							     )
							    current->brief += addFormula();
							  else
							    current->doc += addFormula();
							  insideFormula=FALSE;
							  BEGIN(lastFormulaContext);
  							}
<ReadFormulaLong>\n					{ formulaText+=*yytext; yyLineNr++; }
<ReadFormulaLong,ReadFormulaShort>.                     { formulaText+=*yytext; }
<ExampleDoc,PageDoc,ClassDocBrief,ClassDoc,ReadFormulaShort,ReadFormulaLong>{B}*"*/"  	{
  					  checkDocs();
					  //printf("current->section=%x\n",current->section);
					  if (YY_START==SkipCode) // premature end of code block
					  {
					    err("Error: comment block ended inside \\code ... \\endcode block at line %d in %s!\n",
						yyLineNr,yyFileName);
					    *pSkipDoc += "\\endcode\n\n";
					    BEGIN( lastDocContext );
					  }
					  else if (YY_START==ClassDocBrief &&
					      lastBriefContext==TodoParam)
					  {
					    unput('/');unput('*'); // make sure we have something to read
					    BEGIN( TodoParam );
					  }
					  else if (YY_START==ClassDocBrief &&
					      lastBriefContext==TestParam)
					  {
					    unput('/');unput('*'); // make sure we have something to read
					    BEGIN( TestParam );
					  }
					  else if (YY_START==ClassDocBrief &&
					      lastBriefContext==BugParam)
					  {
					    unput('/');unput('*'); // make sure we have something to read
					    BEGIN( BugParam );
					  }
					  else if (YY_START==ClassDocBrief &&
					      lastBriefContext==DeprecatedParam)
					  {
					    unput('/');unput('*'); // make sure we have something to read
					    BEGIN( DeprecatedParam );
					  }
					  else if (YY_START==ClassDocBrief &&
					      lastBriefContext==XRefItemParam4)
					  {
					    unput('/');unput('*'); // make sure we have something to read
					    BEGIN( XRefItemParam4 );
					  }
					  else if (YY_START==ClassDocBrief &&
					      lastBriefContext==Doc)
					  {
  					    current->doc += "\n\n";
  					    BEGIN( lastDocContext ); 
					  }
					  else if (current->section==Entry::MEMBERGRP_SEC)
					  {
					    unput('$');
					    BEGIN( StoreGroupDocs );
					  }
					  else
					  {
  					    current->doc += "\n\n";
					    //printf("Add docs for class %s\n",current->name.data());
					    current_root->addSubEntry(current);
					    current = new Entry ;
					    initEntry();
					    BEGIN( FindMembers );
					  }
					}
<PageDoc>"<"{TITLE}">"			{ 
  					  current->args.resize(0); 
					  current->argList->clear();
  					  BEGIN( PageDocTitle); 
					}
<PageDocTitle>\n			{ yyLineNr++; current->args+=" "; }
<PageDocTitle>[^\n\<]			{ current->args+=yytext; }
<PageDocTitle>"</"{TITLE}">"		{ BEGIN( PageDoc ); }

  /* escaped versions of the conditional commands (for putting them in the docs) */
<ClassDoc,Doc,AfterDoc,PageDoc,ExampleDoc>{CMD}{CMD}"if"/[^a-z_A-Z0-9]     { current->doc+=yytext; }
<ClassDoc,Doc,AfterDoc,PageDoc,ExampleDoc>{CMD}{CMD}"ifnot"/[^a-z_A-Z0-9]  { current->doc+=yytext; }
<ClassDoc,Doc,AfterDoc,PageDoc,ExampleDoc>{CMD}{CMD}"elseif"/[^a-z_A-Z0-9] { current->doc+=yytext; }
<ClassDoc,Doc,AfterDoc,PageDoc,ExampleDoc>{CMD}{CMD}"else"/[^a-z_A-Z0-9]   { current->doc+=yytext; }
<ClassDoc,Doc,AfterDoc,PageDoc,ExampleDoc>{CMD}{CMD}"endif"/[^a-z_A-Z0-9]  { current->doc+=yytext; }
<LineDoc,JavaDoc>{CMD}{CMD}"if"/[^a-z_A-Z0-9]     { current->brief+=yytext; }
<LineDoc,JavaDoc>{CMD}{CMD}"ifnot"/[^a-z_A-Z0-9]  { current->brief+=yytext; }
<LineDoc,JavaDoc>{CMD}{CMD}"elseif"/[^a-z_A-Z0-9] { current->brief+=yytext; }
<LineDoc,JavaDoc>{CMD}{CMD}"else"/[^a-z_A-Z0-9]   { current->brief+=yytext; }
<LineDoc,JavaDoc>{CMD}{CMD}"endif"/[^a-z_A-Z0-9]  { current->brief+=yytext; }

  /* conditional commands */
<ClassDoc,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc,PageDoc,ExampleDoc>{CMD}"if"{B}+ {
                                          lastIfContext = YY_START;
					  BEGIN(IfGuard);
  					}
<ClassDoc,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc,PageDoc,ExampleDoc>{CMD}"ifnot"{B}+ {
                                          lastIfContext = YY_START;
					  BEGIN(IfNotGuard);
  					}
<ClassDoc,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc,PageDoc,ExampleDoc>{CMD}"if"(\r?)\n |
<IfGuard>\n				{
  					  warn(yyFileName,yyLineNr,"Missing guard for if statement!");
					  yyLineNr++;
					}  
<IfGuard>[^\n\t ]+			{
  					  if (Config_getList("ENABLED_SECTIONS").find(yytext)==-1) // not enabled
					  {
					    BEGIN(SkipSection);
  					    depthIf=1;
					  }
					  else // section enabled
					  {
					    BEGIN(lastIfContext);
					  }
  					}
<IfNotGuard>\n				{
  					  warn(yyFileName,yyLineNr,"Missing guard for ifnot statement!");
					  yyLineNr++;
					}  
<IfNotGuard>[^\n\t ]+			{
  					  if (Config_getList("ENABLED_SECTIONS").find(yytext)==-1) // not enabled
					  {
					    BEGIN(lastIfContext);
					  }
					  else // section enabled
					  {
  					    depthIf=1;
					    BEGIN(SkipSection);
					  }
  					}
<SkipSection>{CMD}"if"/[^a-z_A-Z0-9]	{
  					  depthIf++;
  					}
<SkipSection>{CMD}"endif"/[^a-z_A-Z0-9]	{
  					  if (--depthIf<=0)
					  {
					    BEGIN(lastIfContext);
					  }
  					}
<SkipSection>{CMD}"else"/[^a-z_A-Z0-9]	{
  					  if (depthIf==1)
					  {
					    depthIf=0;
					    BEGIN(lastIfContext);
					  }
  					}
<SkipSection>{CMD}"elseif"/[^a-z_A-Z0-9] {
  					  if (depthIf==1)
					  {
					    BEGIN(IfGuard);
					  }
  					}
<SkipSection>"*/"			{
  					  unput('/');unput('*');
					  BEGIN( lastIfContext );
  					}
<SkipSection>\n				{
  					  yyLineNr++;
  					}
<SkipSection>"//"|"*/"			  					
<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc,PageDoc,ExampleDoc>{CMD}"elseif"/[^a-z_A-Z0-9] {
                                          // previous section enabled => skip now
  					  depthIf=1;
  					  BEGIN(SkipSection);
  					}
<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc,PageDoc,ExampleDoc>{CMD}"else"/[^a-z_A-Z0-9] {
                                          // section was enabled => skip now
  					  depthIf=1;
  					  BEGIN(SkipSection);
  					}
<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc,PageDoc,ExampleDoc>{CMD}"endif"/[^a-z_A-Z0-9] {
                                          // section enabled => absorb endif
  					}


<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc>{CMD}"ingroup"{B}+   { 
  					  lastGroupContext = YY_START;
  					  lineCount();
  					  BEGIN( GroupName ); 
					}
<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc>{CMD}"nosubgrouping"/[^a-z_A-Z0-9] {
                                          current->subGrouping = FALSE; 
                                        }
<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc>{CMD}"showinitializer"/[^a-z_A-Z0-9] {
					  current->initLines = 100000; // ON
  					}
<ClassDoc,ClassDocBrief,LineDoc,AfterDocLine,AfterDocBrief,Doc,JavaDoc,AfterDoc>{CMD}"hideinitializer"/[^a-z_A-Z0-9] {
					  current->initLines = 0; // OFF
  					}
<GroupName>{ID}				{
  					  current->groups->append(
					    new Grouping(yytext, Grouping::GROUPING_INGROUP)
					  );
  					}
<GroupName>\n				{
  					  yyLineNr++; 
					  BEGIN( lastGroupContext );
  					}
<GroupName>"*/"				{
  					  unput('/');unput('*');
					  BEGIN( lastGroupContext );
  					}
<ClassDoc,Doc>{B}*{CMD}("brief"|"short")	{ 
  					  lastBriefContext=YY_START;
					  current->briefFile = yyFileName;
					  current->briefLine = yyLineNr;
  					  BEGIN( ClassDocBrief ); 
					}
<ClassDoc>{B}*"\\inherit"{B}+	        { BEGIN( DocBaseClass ); }
<DocBaseClass>{ID}			{
					  //printf("Adding base class %s\n",yytext);
					  current->extends->append(
					    new BaseInfo(removeRedundantWhiteSpace(yytext),Public,Normal)
					  );
					}
<DocBaseClass>\n			{ yyLineNr++; BEGIN( ClassDoc ); }
<ClassDocBrief>{BS}({BL}|"\\n\\n")	{ 
					  //if (!current->doc.isEmpty()) current->doc+=" <p>";
					  if (lastBriefContext==TodoParam || 
					      lastBriefContext==TestParam || 
					      lastBriefContext==BugParam ||
					      lastBriefContext==DeprecatedParam ||
					      lastBriefContext==XRefItemParam4
					     )
					  {
					    unput('\n');
					  }
					  else
					  {
  					    current->brief=current->brief.stripWhiteSpace();
					    if (yytext[yyleng-1]=='\n') yyLineNr++;
					  }
					  BEGIN( lastBriefContext );
					}
<ClassDocBrief>"\n"			{ 
					  // allow \todo in brief description
					  if (lastBriefContext==TodoParam &&
					      (slStartContext==LineDoc ||
					       slStartContext==AfterDocLine
					      )
					     )
					  {
					    unput('\n'); // make sure we have something to read
					    BEGIN( TodoParam );
					  }
					  else if
					     (lastBriefContext==TestParam &&
					      (slStartContext==LineDoc ||
					       slStartContext==AfterDocLine
					      )
					     )
					  {
					    unput('\n'); // make sure we have something to read
					    BEGIN( TestParam );
					  }
					  else if
					     (lastBriefContext==BugParam &&
					      (slStartContext==LineDoc ||
					       slStartContext==AfterDocLine
					      )
					     )
					  {
					    unput('\n'); // make sure we have something to read
					    BEGIN( BugParam );
					  }
					  else if
					     (lastBriefContext==DeprecatedParam &&
					      (slStartContext==LineDoc ||
					       slStartContext==AfterDocLine
					      )
					     )
					  {
					    unput('\n'); // make sure we have something to read
					    BEGIN( DeprecatedParam );
					  }
					  else if
					     (lastBriefContext==XRefItemParam4 &&
					      (slStartContext==LineDoc ||
					       slStartContext==AfterDocLine
					      )
					     )
					  {
					    unput('\n'); // make sure we have something to read
					    BEGIN( XRefItemParam4 );
					  }
					  else
					  {
					    current->brief += "\n"; 
					    if (!current->doc.isEmpty())
					    {
					      current->doc += "<p>";
					    }
  					    yyLineNr++ ; 
					  }
					}
<ClassDocBrief>"<"{BR}{ATTR}">"
<ClassDocBrief>{BS}/{SECTIONCMD}	|
<ClassDocBrief>{B}*/{SECTIONCMD}	{ 
  					  current->brief=current->brief.stripWhiteSpace();
                                          BEGIN( lastBriefContext ); 
                                        }
<ClassDocBrief>{BS}{CMD}("brief"|"short"){BN}+ {
  					  //lastBriefContext=YY_START;
					}
<ClassDocBrief>.			{ current->brief += *yytext; }
<ClassDocDefine>{ID}/"("		{
  					  current->name = yytext;
					  BEGIN( ClassDefineArgs );
  					}
<ClassDocDefine>{ID}			{
  					  current->name = yytext;
					  if (nextDefContext==ClassDoc)
					    newDocState();
					  else 
					    BEGIN( nextDefContext );
  					}
<ClassDefineArgs>")"			{
  					  current->args+=")";
					  if (nextDefContext==ClassDoc)
					    newDocState();
					  else
					    BEGIN( nextDefContext );
  					}
<ClassDefineArgs>.			{
  					  current->args+= yytext;
  					}
<ClassDocFunc>"\\"{B}*"\n"		{ yyLineNr++; }
<ClassDocFunc>"\n"			{ 
  					  yyLineNr++; 
					  current->name = current->name.stripWhiteSpace();
					  if (current->section == Entry::MEMBERDOC_SEC && current->args.isEmpty())
					    current->section = Entry::VARIABLEDOC_SEC;
					  newDocState();
					}
<ClassDocFunc>"operator"{B}*"("{B}*")"	{
  					  current->name+=yytext;
  					}
<ClassDocFunc>"("			{
  					  current->args+=*yytext;
					  currentArgumentContext = ClassDocFuncQual;
					  fullArgString = current->args.copy();
					  copyArgString = &current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
<ClassDocFunc>"("({ID}"::")*({B}*"*")+	{
  					  current->type+=current->name+yytext;
					  current->name.resize(0);
  					  BEGIN( ClassDocFuncPtr );
  					}
<ClassDocFuncPtr>{SCOPENAME}		{
  					  current->name+=yytext;
  					}
<ClassDocFuncPtr>")"			{
  					  current->type+=')';
  					  BEGIN( ClassDocFunc );
  					}
<ClassDocFuncQual>"{"			{
  					  BEGIN( ClassDocFuncSkipLine);
  					}
<ClassDocFuncQual>{B}*"const"{B}*    	{ 
  					  current->args += " const "; 
					  current->argList->constSpecifier=TRUE;
					}
<ClassDocFuncQual>{B}*"volatile"{B}* 	{ 
  					  current->args += " volatile "; 
					  current->argList->volatileSpecifier=TRUE;
					}
<ClassDocFuncQual>{B}*"="{B}*"0"{B}*	{ 
					  current->args += " = 0"; 
					  current->virt = Pure; 
					  current->argList->pureSpecifier=TRUE;
					}
<ClassDocFuncQual>"throw"{B}*"("	{
  					  current->exception = "throw(";
					  BEGIN(ClassDocFuncExc);
  					}
<ClassDocFuncExc>")"			{
  					  current->exception += ')';
					  BEGIN(ClassDocFuncQual);
  					}
<ClassDocFuncExc>.			{
  					  current->exception += *yytext;
  					}
<ClassDocFunc,ClassDocFuncQual>.	{
  					  current->name += *yytext;
  					}
<ClassDocFuncQual,ClassDocFuncSkipLine>"\n"	{
  					  yyLineNr++; 
					  current->name = current->name.stripWhiteSpace();
					  newDocState();
  					}
<Doc>[a-z_A-Z0-9]+			{ current->doc += yytext; }
<Doc,PageDoc,AfterDoc,LineDoc,ClassDoc>("\\\\"|"@@")	{ current->doc += yytext; }
<Doc>.					{ current->doc += *yytext; }
<DefLineDoc,LineDoc>.			{ current->brief += *yytext; }
<Doc>\n					{ yyLineNr++; current->doc += *yytext; }
<LineDoc>[\n\r]+{B}*"//"[!/]		{ lineCount(); }
<LineDoc>\n				{ 
					  yyLineNr++;  
					  if (lastDocContext==SkipCurly) // inside body
					  {
					    if (!Config_getBool("HIDE_IN_BODY_DOCS"))
					    {
					      if (!previous->doc.isEmpty())
					      {
						previous->doc+="<p>";
					      }
					      previous->doc += current->brief;
					    }
					    current->brief.resize(0);
					  }
					  BEGIN( lastDocContext );
					}
<DefLineDoc>\n				{
  					  yyLineNr++; 
					  unput('/');unput('*');
					  BEGIN( ClassDoc );
  					}

<AfterDocLine>"/*"|"//"			{ current->brief+=yytext; }
<AfterDocLine>\n			{
  					  yyLineNr++;
                                          if (afterDocTerminator!=0)
					    unput(afterDocTerminator);
					  BEGIN(lastAfterDocContext);
  					}
<AfterDocLine>\n{B}*("//!<"|"///<") 	{ 
  					  yyLineNr++;
  					  BEGIN(AfterDocLine);
  					}
<AfterDocLine>\n{B}*("/*!<"|"/**<") 	{ 
  					  yyLineNr++;
  					  BEGIN(AfterDoc);
  					}
<AfterDocLine>.				{ current->brief+=yytext; }
<AfterDocBrief>{BS}({BL}|"\\n\\n")	{ 
  					  current->brief=current->brief.stripWhiteSpace();
					  yyLineNr++;
					  BEGIN( AfterDoc );
					}
<AfterDocBrief>"/*"|"//"		{ current->brief+=yytext; }
<AfterDocBrief>{B}*/{SECTIONCMD}	{
  					  current->brief=current->brief.stripWhiteSpace();
  					  BEGIN( AfterDoc );
  					}
<AfterDocBrief>\n			{ current->brief+=yytext; yyLineNr++; }
<AfterDocBrief>.			{ current->brief+=*yytext; }

  /*
<AfterDocBrief>"<"{BR}{ATTR}">"
<AfterDocBrief>{BS}/{CMD}"ingroup"	{
  					  current->brief=current->brief.stripWhiteSpace();
					  BEGIN( lastBriefContext ); 
  					}
<AfterDocBrief>{BS}/{SECTIONCMD}	{ 
                                          BEGIN( lastBriefContext ); 
                                        }
<AfterDocBrief>{BS}/[^/\n]{BL}		{ yyLineNr++; 
  					  if (!current->brief.stripWhiteSpace().isEmpty())
					    BEGIN(AfterDoc); 
					}
  */
<AfterDocBrief>"*/"			{
                                          if (afterDocTerminator!=0)
					    unput(afterDocTerminator);
					  BEGIN(lastAfterDocContext);
  					}
<AfterDocBrief>"."/{BN}			{ BEGIN(AfterDoc); }
<LineDoc,AfterDocLine>{CMD}"internal"	{ 
                                          if (!Config_getBool("INTERNAL_DOCS"))
                                          {
					    lastInternalDocContext = YY_START;
                                            BEGIN( DocInternalLine );
                                          }
                                          else
                                          {
                                            current->doc+="\\internal"; 
					  }
					}
<Doc,JavaDoc,ExampleDoc,PageDoc,ClassDoc,AfterDoc>{CMD}"internal"		{ 
                                          if (!Config_getBool("INTERNAL_DOCS"))
                                          {
					    lastInternalDocContext = YY_START;
                                            BEGIN( DocInternal );
                                          }
                                          else
                                          { 
					    current->doc+="\\internal"; 
					  }
					}
<DocInternal>.				
<DocInternal>\n				{ yyLineNr++; }
<DocInternal>"/*"|"//"			
<DocInternal>"*/"			{
					  unput('/');
					  unput('*');
  					  BEGIN( lastInternalDocContext );
  					}
<DocInternalLine>.				
<DocInternalLine>\n			{
  					  yyLineNr++;
					  unput('\n');
  					  BEGIN( lastInternalDocContext );
  					}
<AfterDoc>{CMD}"brief"			{ BEGIN(AfterDocBrief); }
<AfterDoc>"/*"|"//"			{ current->doc+=yytext; }
<AfterDoc>^{B}*"*"+/[^/]			
<AfterDoc>\n				{ current->doc+=yytext; yyLineNr++; }
<AfterDoc>.				{ current->doc+=*yytext; }
<AfterDoc>"*/"				{
                                          if (afterDocTerminator!=0)
					    unput(afterDocTerminator);
					  BEGIN(lastAfterDocContext);
  					}
<ClassDocRelates,ClassDocFunc,ClassDocDefine,GroupDocArg1,ClassDocArg1,SectionTitle,EnumDocArg1,PageDocArg1,ExampleDocArg1,ClassDefineArgs>"*/" {
                                          // defer "*/" to a later time
  					  unput('/');
  					  unput('*');
					  // insert \n and decrement the line number to compensate for the artifical newline
					  unput('\n');
					  yyLineNr--;
					  BEGIN( Doc );
					}
<FileDocArg1>"*/"			{
  					  current->name = yyFileName;
  					  current->doc += "\n\n";
					  current_root->addSubEntry(current);
					  current = new Entry ;
					  initEntry();
					  BEGIN( FindMembers );
  					}
<Doc>"*/"				{ 
  				          checkDocs();
  					  current->doc += "\n\n";
					  //printf("End of docs at line %d\n",yyLineNr);
					  if (lastDocContext==SkipCurly && Config_getBool("HIDE_IN_BODY_DOCS"))
					  {
					    current->doc.resize(0);
					  }
  					  BEGIN( lastDocContext ); 
					}
<JavaDoc>"*/"				{ 
					  unput('/');unput('*');
					  BEGIN( tmpDocType );
					}
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,AfterDoc>^{B}*(("//"{B}*)?)"*"+[ \t]*"-"("#")?{B}+ { 
  				         current->doc += yytext; 
					}
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,AfterDoc>^{B}*(("//"{B}*)?)"*"+[ \t]*"."{B}*\n { 
  				         current->doc += yytext; 
					 yyLineNr++;
					}
<ClassDocBrief,AfterDocBrief>^{B}*(("//"{B}*)?)"*"+[ \t]*"-"("#")?{B}+ { 
  				         current->brief += "-"; 
					}
<ClassDocBrief,AfterDocBrief>^{B}*(("//"{B}*)?)"*"+[ \t]*"."{B}*\n { 
  				         current->brief += "."; yyLineNr++; 
					}
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,ClassDocBrief,AfterDoc,AfterDocBrief>^{B}*(("//"{B}*)?)"*"+/[^/]
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,ClassDocBrief,AfterDoc,AfterDocBrief>^{B}*(("//"{B}*)?)"*"+{B}+ { 
					  current->doc+=' '; 
					}

<Doc,ClassDoc,PageDoc,ExampleDoc,AfterDoc>"\\"[a-z_A-Z][a-z_A-Z0-9]*[\\] { // directory type of text
  					  current->doc+=yytext;
  					}
<SkipSection>{CMD}[a-z_A-Z][a-z_A-Z0-9]*  {
  					    QCString *pValue=Doxygen::aliasDict[yytext+1];
					    if (pValue)
					    {
					      int i,l=pValue->length();
					      for (i=l-1;i>=0;i--)
					      {
					        unput(pValue->at(i));
					      }
					    }
                                          }
<Doc,ClassDoc,PageDoc,ExampleDoc,AfterDoc,CopyArgComment>{CMD}{CMD}[a-z_A-Z][a-z_A-Z0-9]* 	{ /* escaped command */
					    if (YY_START==CopyArgComment)
					      fullArgString+=yytext;
					    else
  					      current->doc+=yytext;
                                          }
<Doc,ClassDoc,PageDoc,ExampleDoc,AfterDoc,CopyArgComment>{CMD}[a-z_A-Z][a-z_A-Z0-9]* 	{
                                          bool handled=FALSE;
                                          if ( useOverrideCommands)
                                          {
					      if ( strcmp(yytext+1,"static")==0 )
					      {
  					        current->stat = TRUE; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"pure")==0 )
					      {
						current->virt = Pure; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"private")==0 )
					      {
						current->protection = Private; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"privatesection")==0 )
					      {
						current->protection = protection = Private; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"protected")==0 )
					      {
						current->protection = Protected; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"protectedsection")==0 )
					      {
						current->protection = protection = Protected ; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"public")==0 )
					      {
						current->protection = Public; handled = TRUE;
					      }
					      else if ( strcmp(yytext+1,"publicsection")==0 )
					      {
						current->protection = protection = Public; handled = TRUE;
					      }
					  }
                                          if (!handled)
                                          {
  					    QCString *pValue=Doxygen::aliasDict[yytext+1];
					    if (pValue)
					    {
					      int i,l=pValue->length();
					      char c;
					      for (i=l-1;i>=0;i--)
					      {
						c=pValue->at(i);
					        unput(c);
						if (c=='\n') yyLineNr--;
					      }
					    }
					    else
					    {
					      if (YY_START==CopyArgComment)
					        fullArgString+=yytext;
					     else
  					        current->doc+=yytext;
					    }
                                          }
  					}
<JavaDoc,LineDoc,ClassDocBrief,AfterDocBrief,AfterDocLine>"\\"[a-z_A-Z][a-z_A-Z0-9]*[\\] { // directory type of text
  					  current->brief+=yytext;
  					}
<LineDoc,AfterDocLine,CopyArgCommentLine>{CMD}("brief"|"short") {}
<JavaDoc,LineDoc,ClassDocBrief,AfterDocBrief,AfterDocLine,CopyArgCommentLine>{CMD}{CMD}[a-z_A-Z][a-z_A-Z0-9]* 	{ /* escaped command */
					  if (YY_START==CopyArgCommentLine)
					    fullArgString+=yytext;
					  else
  					    current->brief+=yytext;
                                        }
<JavaDoc,LineDoc,ClassDocBrief,AfterDocBrief,AfterDocLine,CopyArgCommentLine>{CMD}[a-z_A-Z][a-z_A-Z0-9]* 	{
  					  QCString *pValue=Doxygen::aliasDict[yytext+1];
					  if (pValue)
					  {
					    int i,l=pValue->length();
					    for (i=l-1;i>=0;i--)
					    {
					      unput(pValue->at(i));
					    }
					  }
					  else
					  {
					    if (YY_START==CopyArgCommentLine)
					      fullArgString+=yytext;
					    else
  					      current->brief+=yytext;
					  }
  					}
<DefLineDoc,LineDoc,ClassDoc,PageDoc,ExampleDoc,Doc>"/*"|"//"   { current->doc += yytext; }
<SkipCxxComment>.*/\n			{ 
					  BEGIN( lastCContext ) ;
					}
<SkipComment>[^\*\n]+
<*>\n					{ yyLineNr++ ; }
<*>\"					{
					  if (insideIDL && insideCppQuote)
					  {
					    BEGIN(EndCppQuote);
					  }
					}
<*>"#"				        {	
  					  if (! insidePHP)
  					    REJECT;
  					  lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>.
<SkipComment>"//"|"/*"
<*>"/*"					{ lastCContext = YY_START ;
					  BEGIN( SkipComment ) ;
					}
<SkipComment>{B}*"*/"			{ BEGIN( lastCContext ) ; }
<*>"//"				        {	
  					  lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
%%

//----------------------------------------------------------------------------

static void startGroup()
{
  if (!lastDefGroup.groupname.isEmpty())
  {
    setCurrentGroup( &lastDefGroup.groupname, lastDefGroup.pri );   
    autoGroupStack.push(new Grouping(lastDefGroup));
    lastDefGroup.groupname.resize(0);
  }
  else 
  {
    //if (memberGroupId!=NOGROUP)
    //{
    //  //warn(yyFileName,yyLineNr,"Warning: ignoring nested member group. "
    //  //	"Previous command was found at line %d.",lastMemberGroupLine);
    //  printf("startGroup ends group %d\n",memberGroupId);
    //  endGroup();
    //}
    if (memberGroupHeader.isEmpty())
    {
      // warn( yyFileName, yyLineNr, "Warning: member group does not have a header" );
      memberGroupHeader="[NOHEADER]";
    }
    memberGroupId = newMemberGroupId();
    MemberGroupInfo *info = new MemberGroupInfo;
    info->header = memberGroupHeader.stripWhiteSpace();
    Doxygen::memGrpInfoDict.insert(memberGroupId,info);
    //Doxygen::memberHeaderDict.insert(memberGroupId,
    //	new QCString(memberGroupHeader.stripWhiteSpace())
    //			   );
    
    memberGroupRelates = current->relates.copy();
    memberGroupInside = current->inside.copy();
    current->mGrpId = memberGroupId;
    lastMemberGroupLine = yyLineNr;
  }
}

static void startGroupInDoc()
{
  if (current->section==Entry::GROUPDOC_SEC ) /* scope for a non-member group: @defgroup */
  {
    autoGroupStack.push(new Grouping(current->name,
	  current->groupingPri()
	  ));
  }
  else if (current->section == Entry::MEMBERGRP_SEC) /* scope for a member group: @name */
  {
    //if (memberGroupId!=NOGROUP)
    //{
    //  printf("startGroupInDoc ends group %d\n",memberGroupId);
    //  endGroup();
    //}
    memberGroupId = newMemberGroupId();
    MemberGroupInfo *info = new MemberGroupInfo;
    info->header = memberGroupHeader.stripWhiteSpace();
    Doxygen::memGrpInfoDict.insert(memberGroupId,info);
    //Doxygen::memberHeaderDict.insert(memberGroupId,
    //	new QCString(memberGroupHeader.stripWhiteSpace())
    //			   );
    memberGroupRelates = current->relates.copy();
    memberGroupInside = current->inside.copy();
    current->mGrpId = memberGroupId;
    lastMemberGroupLine = yyLineNr;
  }
  else
  {
    warn(yyFileName,yyLineNr,"Warning: @{ may only be used in a group block!\n");
  }
}

static void endGroup()
{
  if (memberGroupId!=NOGROUP) // end of member group
  {
    //Doxygen::memberDocDict.insert(memberGroupId,
    //	new QCString(memberGroupDocs)
    //			);
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(memberGroupId);
    if (info) 
    {
      info->doc = memberGroupDocs;
      info->docFile = yyFileName;
    }
    memberGroupId=NOGROUP;
    memberGroupRelates.resize(0);
    memberGroupInside.resize(0);
    if (YY_START!=ReadInitializer) 
    {
      current->mGrpId=NOGROUP;
      current->relates.resize(0);
    }
    memberGroupDocs.resize(0);
  }
  else if (!autoGroupStack.isEmpty()) // end of group
  {
    Grouping *current = autoGroupStack.pop();
    Grouping *parent = autoGroupStack.top();
    if( parent ) {
      setCurrentGroup( &parent->groupname, parent->pri );
    } else {
      setCurrentGroup( 0, Grouping::GROUPING_LOWEST );
    }
    delete current;
  }
}

static void forceEndGroup()
{
  while (memberGroupId!=NOGROUP || !autoGroupStack.isEmpty()) 
  {
    //printf("forceEndGroup ends group %d\n",memberGroupId);
    endGroup();
  }
}

//----------------------------------------------------------------------------

static void newDocState()
{
  if (tmpDocType!=-1)
  {
    tmpDocType=ClassDoc;
    BEGIN(JavaDoc);
  }
  else
  {
    BEGIN(ClassDoc);
  }
}

//----------------------------------------------------------------------------

static void parseCompounds(Entry *rt)
{
  //printf("parseCompounds(%s)\n",rt->name.data());
  EntryListIterator eli(*rt->sublist);
  Entry *ce;
  for (;(ce=eli.current());++eli)
  {
    if (!ce->program.isEmpty())
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  ce->name.data(),ce->program.data());
      // init scanner state
      padCount=0;
      depthIf = 0;
      inputString = ce->program;
      lastDefGroup.groupname.resize(0);
      inputPosition = 0;
      scanYYrestart( scanYYin ) ;
      if (ce->section==Entry::ENUM_SEC)
	BEGIN( FindFields ) ;
      else
	BEGIN( FindMembers ) ;
      current_root = ce ;
      strcpy( yyFileName, ce->fileName ) ;
      setContext();
      yyLineNr = ce->startLine ;
      //printf("---> Inner block starts at line %d\n",yyLineNr);
      //current->reset();
      current = new Entry;
      gstat = FALSE;
      int ni=ce->name.findRev("::"); if (ni==-1) ni=0; else ni+=2;
      // set default protection based on the compound type
      if( ce->section==Entry::CLASS_SEC ) // class
      {
	if (
	    ce->fileName.right(4)==".php" ||
	    ce->fileName.right(4)==".inc"
	   )
          current->protection = protection = Public ; 
	else if (ce->fileName.right(5)==".java")
          current->protection = protection = Package ; 
	else
          current->protection = protection = Private ;
      }
      else if (ce->section == Entry::ENUM_SEC ) // enum
      {
	current->protection = protection = ce->protection;
      }
      else if (!ce->name.isEmpty() && ce->name.at(ni)=='@') // unnamed union or namespace
      {
	if (ce->section == Entry::NAMESPACE_SEC ) // unnamed namespace
	{
          current->stat = gstat = TRUE;
	}
	current->protection = protection = ce->protection;
      }
      else // named struct, union, or interface
      {
	current->protection = protection = Public ;
      }
      mtype = Method;
      virt = Normal;
      //printf("name=%s current->stat=%d gstat=%d\n",ce->name.data(),current->stat,gstat);

      memberGroupId = NOGROUP;
      memberGroupRelates.resize(0);
      memberGroupInside.resize(0);
      
      scanYYlex() ;
      forceEndGroup();
      delete current; current=0;
      ce->program.resize(0);


      if (depthIf>0)
      {
	warn(yyFileName,yyLineNr,"Documentation block ended in the middle of a conditional section!");
      }
    }
    parseCompounds(ce);
  }
}

//----------------------------------------------------------------------------

void parseMain(Entry *rt)
{
  initParser();
  anonCount     = 0;
  depthIf       = 0;
  protection    = Public;
  mtype         = Method;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  global_root   = rt;
  current       = new Entry;
  inputString   = rt->program;
  inputPosition = 0;
  scanYYrestart( scanYYin );
  BEGIN( FindMembers );
  scanYYlex();

  forceEndGroup();

  if (depthIf>0)
  {
    warn(yyFileName,yyLineNr,"Documentation block ended in the middle of a conditional section!");
  }

  rt->program.resize(0);
  delete current; current=0;
  parseCompounds(rt);
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void scannerYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

