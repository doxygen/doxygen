/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */
  
%{

/*
 *	includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <assert.h>
#include <ctype.h>

#include <qarray.h>
#include <qstring.h>  
#include <qstack.h>
#include <qregexp.h>
  
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "index.h"
#include "defargs.h"
#include "language.h"

#ifndef WIN32
#include <unistd.h>
#endif
  
#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 *	statics
 */
static bool             insideArgumentList;
static QString          className;
static QString          memberName;
static QString          refName;
static OutputList *     outDoc;
static QString          code;
static QString          linkRef;
static QString          linkText;
static QString          codeBlock;
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
static int              lastCodeState;
static int              lastAfterDocContext;
static int              lastGroupContext;
static int              lastFormulaContext;
static int              nextDefContext;
static Protection	protection;
static Protection	baseProt;
static int		bracketCount = 0 ;
static int		sharpCount   = 0 ;
static int		roundCount   = 0 ;
static int              ifCount      = 0 ;
static Entry*		current_root = 0 ;
static Entry*		global_root  = 0 ;
static Entry*		current      = 0 ;
static int		yyLineNr     = 0 ;
static int              anonCount    = 0 ;        
static char		yyFileName[2048] ;
static bool    		sig;
static bool    		slot;
static bool    		gstat;
static bool             removeSlashes;
static Specifier 	virt;
static Specifier 	baseVirt;
static bool             exampleDoc;
static QString          exampleName;
static QString          htmlUrl,htmlText;
static QString          currentIncludeFile;
static QString          msType,msName,msArgs;
static int              includeFileOffset = 0;
static int              includeFileLength = 0;
static bool             firstLine;
static bool             isTypedef;
static bool             inParamBlock;
static bool             inExceptionBlock;
static bool             inSeeBlock;
static bool             inReturnBlock;
static bool             inAuthorBlock;
static bool             inVersionBlock;
static bool             inDateBlock;
static bool             inBugBlock;
static bool             inWarningBlock;
static bool             inParBlock;
static bool             firstSeeArg;
static bool             javaDocSee;
static char             afterDocTerminator;
static int              tmpDocType;
static QString          sectionLabel;
static QString		sectionTitle;
static bool             isSubsection;
static QString          funcPtrType;
static QString          templateStr;
static QString          baseName;
static QString         *specName;
static QString          formulaText;
static QString          sectionRef;

// state variable for reading the argument list of a function
static int              argRoundCount;
static int              argSharpCount;
static int              currentArgumentContext;
static int              lastCopyArgStringContext;
static int              lastCopyArgContext;
static QString         *copyArgString;



//-----------------------------------------------------------------------------

static void initParser()
{
  insideArgumentList=FALSE;
  className.resize(0);
  memberName.resize(0);
  refName.resize(0);
  code.resize(0);
  linkRef.resize(0);
  linkText.resize(0);
  codeBlock.resize(0);
  htmlUrl.resize(0);
  htmlText.resize(0);
  currentIncludeFile.resize(0);
  sectionLabel.resize(0);
  sectionTitle.resize(0);
  baseName.resize(0);
  formulaText.resize(0);
  protection = Public;
  baseProt = Public;
  bracketCount = 0;
  sharpCount = 0;
  roundCount = 0;
  ifCount = 0;
  sig = FALSE;
  slot = FALSE;
  gstat = FALSE;
  virt = Normal;
  baseVirt = Normal;
  includeFileOffset = 0;
  includeFileLength = 0;
  firstLine = TRUE;
  isTypedef = FALSE;
  inParamBlock = FALSE;
  inExceptionBlock = FALSE;
  inSeeBlock = FALSE;
  inReturnBlock = FALSE;
  inAuthorBlock = FALSE;
  inVersionBlock = FALSE;
  inDateBlock = FALSE;
  inBugBlock = FALSE;
  inWarningBlock = FALSE;
  inParBlock = FALSE;
  firstSeeArg = FALSE;
  javaDocSee = FALSE;
  isSubsection = FALSE;
}

//-----------------------------------------------------------------------------

void scanString(const char *s);

//-----------------------------------------------------------------------------

class TableElem
{
  public:
    TableElem(int r,int c);
   ~TableElem();
    int getRow() { return row; }
    int getCol() { return col; }
    OutputList *outputList() { return ol; }
   
  private:
    OutputList *ol;
    int row;
    int col;
};

TableElem::TableElem(int r,int c)
{
  //printf("TableElem::TableElem(%d,%d)\n",r,c);
  ol=new OutputList(outDoc);
  outDoc=ol;
  row=r;
  col=c;
}

TableElem::~TableElem()
{
  //printf("TableElem::~TableElem(%d,%d)\n",row,col);
  delete ol; 
}

class Table
{
  public:
    Table();
   ~Table();
    void newRow();
    void newElem();
    
  private:
    OutputList *parentDoc;
    QList<TableElem> *elemList;
    int curRow;
    int curCol;
    int rows;
    int cols;
};  

Table::Table() 
{
  parentDoc=outDoc;
  elemList=new QList<TableElem>;
  elemList->setAutoDelete(TRUE);
  curRow=curCol=rows=cols=0;
}

Table::~Table()
{
  //printf("Table::~Table()\n");
  // use elemList & cols & rows
  if (cols>0 && rows>0)
  {
    parentDoc->startTable(cols);
    TableElem *e=elemList->first();
    while (e)
    {
      if (e->getRow()>0)
      {
	if (e->getCol()==0) 
	{
	  if (e->getRow()>1) parentDoc->endTableRow();
	  parentDoc->nextTableRow();
	}
	else
	{
	  parentDoc->nextTableColumn();
	}
	*parentDoc+=*e->outputList();
	parentDoc->endTableColumn();
      }
      e=elemList->next();
    }
    parentDoc->endTable();
  }
  delete elemList;
  outDoc=parentDoc;
}

void Table::newRow()
{
  //printf("Table::newRow()\n");
  curRow++;
  if (curRow>rows) rows=curRow;
  curCol=0;
}

void Table::newElem()
{
  //printf("Table::newElem(%d,%d)\n",curRow,curCol);
  TableElem *te = new TableElem(curRow,curCol);
  elemList->append(te);

  curCol++;
  if (curCol>cols) cols=curCol;
}

static QStack<Table> tableStack;
static Table *curTable;

static void startTable()
{
  //printf("startTable()\n");
  curTable=new Table;
  tableStack.push(curTable);
}

static void endTable()
{
  //printf("endTable()\n");
  delete tableStack.pop(); // the destructor adds the table to the stream!
  curTable=tableStack.top();
}

//-----------------------------------------------------------------------------

static void lineCount()
{
  for( const char* c = yytext ; *c ; ++c )
    yyLineNr += (*c == '\n') ;
}

static void endArgumentList()
{
  if (insideArgumentList)
  {
    insideArgumentList=FALSE;
    outDoc->endItemList();
  }
}

static void addType( Entry* current )
{
    if( current->type.length() )
	current->type += ' ' ;
    current->type += current->name ;
    current->name.resize(0) ;
    if( current->type.length() )
	current->type += ' ' ;
    current->type += current->args ;
    current->args.resize(0) ;
    current->argList->clear();
}

static void includeFile(OutputList &ol,const char *fileName,bool quiet)
{
  //FileInfo *f;
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(&exampleNameDict,fileName,ambig)))
  {
    currentIncludeFile=fileToString(fd->absFilePath());
    includeFileOffset=0;
    includeFileLength=currentIncludeFile.length();
    OutputList codeFrag(&ol);
    parseCode(codeFrag,0,currentIncludeFile,exampleDoc,exampleName);
    if (!quiet)
    {
      ol.startCodeFragment();
      ol+=codeFrag;
      ol.endCodeFragment();
    }
  }
  else if (ambig)
  {
    warn("Include file name %s is ambigious.\n",fileName);
    warn("Possible candidates:\n");
    //includeFileList.writeMatches(fileName);
    showFileDefMatches(&exampleNameDict,fileName);
  }
  else
  {
    warn("Warning: example file %s is not found. ",fileName);
    warn("Check your EXAMPLE_PATH\n");
  }
}

static void verbIncludeFile(OutputList &ol,const char *name)
{
  //FileInfo *f;
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(&exampleNameDict,name,ambig)))
  {
    ol.startCodeFragment();
    ol.codify(fileToString(fd->absFilePath()));
    ol.endCodeFragment();
  }
  else if (ambig)
  {
    warn("Include file name %s is ambigious.\n",name);
    warn("Possible candidates:\n");
    showFileDefMatches(&exampleNameDict,name);
  }
  else
  {
    warn("Warning: example file %s is not found. ",name);
    warn("Check your EXAMPLE_PATH\n");
  }
}


static QString stripQuotes(const char *s)
{
  QString name;
  if (s==0 || *s==0) return name;
  name=s;
  if (name.at(0)=='"' && name.at(name.length()-1)=='"')
  {
    name=name.mid(1,name.length()-2);
  }
  return name;
}

static QString stripKnownExtensions(const char *text)
{
  QString result=text;
  if (result.right(4)==".tex")       result=result.left(result.length()-4);
  else if (result.right(5)==".html") result=result.left(result.length()-5);
  //printf("%s stripKnowExtensions(%s)\n",result.data(),text); 
  return result;
}

static void skipLine(OutputList &ol,const char *key)
{
  bool found=FALSE;
  while (!found)
  {
    QString s;
    char c;
    while ( includeFileOffset<includeFileLength && 
	    (c=currentIncludeFile[includeFileOffset++])!='\n' && c!=0
	  ) s+=c;
    if (s.find(key)!=-1)
    {
      found=TRUE;
      ol.writeString("    ");
      parseCode(ol,className,s,exampleDoc,exampleName);
      ol.writeString("\n");
    }
    else if (includeFileOffset==includeFileLength) found=TRUE;
  }
}

static void skipUntil(const char *key)
{
  bool found=FALSE;
  while (!found)
  {
    QString s;
    int i=includeFileOffset;
    char c;
    while ( i<includeFileLength &&
	    (c=currentIncludeFile[i++])!='\n' && c!=0
	  ) s+=c;
    if (s.find(key)!=-1 || i==includeFileLength)
    {
      found=TRUE;
    }
    else
    {
      includeFileOffset=i;
    }
  }
}

static void showLine(OutputList &ol,const char *key)
{
  QString s;
  char c;
  bool found=FALSE;
  while (!found)
  {
    while ( includeFileOffset<includeFileLength && 
	(c=currentIncludeFile[includeFileOffset++])!='\n' && c!=0
	  ) s+=c;
    if (s.stripWhiteSpace().length()>0 || 
	includeFileOffset==includeFileLength) found=TRUE;
  }
  if (s.find(key)!=-1)
  {
    ol.writeString("    ");
    parseCode(ol,className,s,exampleDoc,exampleName);
    ol.writeString("\n");
  }
}

static void showUntil(OutputList &ol,const char *key)
{
  bool found=FALSE;
  while (!found)
  {
    QString s;
    char c;
    while ( includeFileOffset<includeFileLength && 
	    (c=currentIncludeFile[includeFileOffset++])!='\n' && c!=0
	  ) s+=c;
    if (s.stripWhiteSpace().length()>0)
    {
      ol.writeString("    ");
      parseCode(ol,className,s,exampleDoc,exampleName);
      ol.writeString("\n");
      if (s.find(key)!=-1) found=TRUE; 
    }
    if (includeFileOffset==includeFileLength) found=TRUE;
  }
}

static void newDocState();

//-----------------------------------------------------------------

static bool inBlock()
{
  return inParamBlock || inSeeBlock || inReturnBlock || inAuthorBlock ||
         inVersionBlock || inDateBlock || inWarningBlock || inBugBlock ||
	 inParBlock || inExceptionBlock;
}

static void endBlock()
{
    outDoc->endDescList();
    inParamBlock=inSeeBlock=inReturnBlock=inAuthorBlock=
    inVersionBlock=inDateBlock=inBugBlock=inWarningBlock=
    inParBlock=inExceptionBlock=FALSE;
}

static void addSection()
{
  //printf("New section pageName=%s label=%s title=%s\n",
  //    current->name.data(),sectionLabel.data(),sectionTitle.data());
  SectionInfo *si=new SectionInfo(current->name,
                   sectionLabel,sectionTitle,isSubsection);
  sectionDict.insert(sectionLabel,si);
}


// Adds a formula text to the list/dictionary of formulas if it was
// not already added. Returns the label of the formula.
static QString addFormula()
{
  QString formLabel;
  QString fText=formulaText.simplifyWhiteSpace();
  Formula *f=0;
  if ((f=formulaDict[fText])==0)
  {
    f = new Formula(fText);
    formulaList.append(f);
    formulaDict.insert(fText,f);
    formLabel.sprintf("\\form#%d",f->getId());
    formulaNameDict.insert(formLabel,f);
  }
  else
  {
    formLabel.sprintf("\\form#%d",f->getId());
  }
  return formLabel;
}

/* -----------------------------------------------------------------
 */
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

//ATTR      ((({BN}+[^\>]+)/">")?)
%}

BN        [ \t\n\r]
BL        [ \t\r]*"\n"
B         [ \t]
BS        ^(({B}*"//")?)(({B}*"*"+)?){B}*
FILE      ([a-z_A-Z0-9\.\\:\/\-\+]+)|("\""[^\n\"]+"\"")
FILEMASK  [a-z_A-Z0-9\.\\:\/\-\+]+"."[a-z_A-Z0-9\.\-\+]*[a-z_A-Z0-9\-\+]
ID        [a-z_A-Z][a-z_A-Z0-9]*
SCOPEID   {ID}({ID}*{BN}*"::"{BN}*)*({ID}?)
SCOPENAME (({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
SCOPEMASK {ID}?(("::"|"#")?(~)?{ID})+
URLMASK   [a-z_A-Z0-9\~\:\@\#\.\-\+\/]+
ATTR      ({B}+[^>\n]*)?
A         [aA]
BOLD	  [bB]
BODY	  [bB][oO][dD][yY]
BR	  [bB][rR]
EM	  [eE][mM]
CENTER    [cC][eE][nN][tT][eE][rR]
CODE      [cC][oO][dD][eE]
DL	  [dD][lL]
DD	  [dD][dD]
DT	  [dD][tT]
DFN       [dD][fF][nN]
FORM	  [fF][oO][rR][mM]
H1	  [hH]1
H2	  [hH]2
H3	  [hH][3-6]
HEAD	  [hH][eE][aA][dD]
HR	  [hH][rR]
HREF      [hH][rR][eE][fF]
I         [iI]
IMG       [iI][mM][gG]
INPUT     [iI][nN][pP][uU][tT]
LI	  [lL][iI]
META	  [mM][eE][tT][aA]
MULTICOL  [mM][uU][lL][tT][iI][cC][oO][lL]
NAME      [nN][aA][mM][eE]
OL        [oO][lL]
P	  [pP]
PRE       [pP][rR][eE]
SMALL	  [sS][mM][aA][lL][lL]
STRONG    [sS][tT][rR][oO][nN][gG]
SUB	  [sS][uU][bB]
SUP	  [sS][uU][pP]
SRC       [sS][rR][cC]
TABLE	  [tT][aA][bB][lL][eE]
TITLE     [tT][iI][tT][lL][eE]
TD	  [tT][dD]
TR	  [tT][rR]
TT	  [tT][tT]
UL	  [uU][lL]
VAR	  [vV][aA][rR]

%x	Define
%x      DefineArg
%x	DefineEnd
%x	Include
%x	ClassName
%x	ClassVar
%x	Bases
%x	NextSemi
%x	FindMembers
%x	FindMemberName
%x      FindFields
%x      FindFieldArg
%x	Function
%x	FuncRound
%x	ExcpRound
%x	FuncQual
%x	Operator
%x	Array
%x	Curly
%x	Using
%x	NameSpaceDocArg1
%x	SkipCurly
%x	SkipCurlyCpp
%x      SkipString
%x	SkipInits
%x	SkipCPP
%x	SkipCPPBlock
%x	SkipComment
%x	SkipCxxComment
%x      SkipBlock
%x	SkipCode
%x	Sharp
%x      SkipSharp
%x	SkipRound
%x	TypedefName
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
%x      ClassDocFuncExc
%x	ClassDocDefine
%x      ClassDocRelates
%x      ClassDocBrief
%x      ClassDocOverload
%x	ClassDefineArgs
%x	GroupDocArg1
%x	GroupDocArg2
%x	GroupName
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
%x      EnumDoc
%x	EnumDocArg1
%x	FuncPtr
%x	EndFuncPtr
%x	FuncFunc
%x	FuncFuncEnd
%x	FuncFuncType
%x      MemberSpec
%x      MemberSpecSkip
%x	SkipVerbatim
%x      DocScan
%x	DocParam
%x	DocException
%x	DocHtmlScan
%x	DocLatexScan
%x      DocEmphasis
%x      DocBold
%x      DocCode
%x      DocCodeBlock
%x      DocInternal
%x      DocLink
%x      DocLinkText
%x      DocSkipWord
%x      DocInclude
%x      DocDontInclude
%x      DocDescItem
%x      DocHtmlLink
%x      DocHtmlAnchor
%x      DocHtmlHref1
%x      DocHtmlHref2
%x	DocBaseClass
%x	DocSkiplineKey
%x	DocSkipKey
%x	DocLineKey
%x	DocUntilKey
%x	DocKeyEnd
%x	DocPar
%x	DocRefName
%x	DocVerbatim
%x	DocVerbInc
%x	DocIndexWord
%x	DocRef
%x      DocRefItem
%x      DocRefItemName
%x	SectionLabel
%x	SectionTitle
%x	EndTemplate
%x      CopyArgString
%x	CopyArgRound
%x	CopyArgSharp
%x	ReadFuncArgType
%x	ReadTempArgs
%x	Specialization
%x      DocSkipHtmlComment
%x      ReadFormulaShort
%x	ReadFormulaLong

%%

<*>\x06[^\x06]*\x06			{ // new file
  					  yyLineNr= 1 ; 
					  int i; 
					  for( i = 0 ; yytext[i+1] != 6 ; i++ )
					    yyFileName[i] = yytext[i+1] ;
					  yyFileName[i] = 0 ;
					  msg("Parsing file %s...\n",yyFileName);
					  current_root  = global_root ;
					  initParser();
					  current->reset();
					  int sec=guessSection(yyFileName);
					  if (sec)
					  {
					    current->name    = yyFileName;
					    current->name    = current->name;
					    current->section = sec;
					    current_root->addSubEntry(current);
					    current          = new Entry;
					  }
					  BEGIN( FindMembers );
					}
<*>\x0d
  /*
<DocScan>^{BL} 	                        { 
  					  if (insideArgumentList)
					  {
					    insideArgumentList=FALSE;
					    outDoc->endItemList();
					  }
					  else
					  {
					    outDoc->newParagraph();
					  }
					  if (inBlock()) endBlock();
					}
  */
<DocScan>"<!--"				{ BEGIN(DocSkipHtmlComment); }
<DocSkipHtmlComment>"--"[!]?">"		{ BEGIN(DocScan); }
<DocSkipHtmlComment>.
<DocScan>"&copy;"			{ outDoc->writeCopyright(); }
<DocScan>"&quot;"			{ outDoc->writeQuote(); }
<DocScan>"&"[AEIOUYaeiouy]"uml;"	{ outDoc->writeUmlaut(yytext[1]); }
<DocScan>"&"[AEIOUYaeiouy]"acute;"	{ outDoc->writeAcute(yytext[1]);  }
<DocScan>"&"[AEIOUaeiou]"grave;"	{ outDoc->writeGrave(yytext[1]);  }
<DocScan>"&"[AEIOUaeiou]"circ;"		{ outDoc->writeCirc(yytext[1]);   }
<DocScan>"&"[ANOano]"tilde;"		{ outDoc->writeTilde(yytext[1]);  }
<DocScan,DocHtmlScan,DocLatexScan>"$("[a-z_A-Z]+")"	{ 
  					  QString envvar=&yytext[2];
  					  envvar=envvar.left(envvar.length()-1);
					  outDoc->docify(getenv(envvar));
					}
<DocScan>("\\"|"@")"htmlonly"/[^a-z_A-Z0-9]	{
  					  outDoc->disableAllBut(OutputGenerator::Html); 
					  BEGIN(DocHtmlScan);
  					}
<DocHtmlScan>("\\"|"@")"endhtmlonly"/[^a-z_A-Z0-9] {
  					  outDoc->enableAll();
					  BEGIN(DocScan);
  					}
<DocScan>("\\"|"@")"latexonly"/[^a-z_A-Z0-9]	{
  					  outDoc->disableAllBut(OutputGenerator::Latex);
					  BEGIN(DocLatexScan);
  					}
<DocHtmlScan,DocLatexScan>"//"|"/*"|"*/"	{
  					  outDoc->writeString(yytext);
  					}
<DocHtmlScan,DocLatexScan>.		{
  					  char c[2];
					  c[0]=*yytext;c[1]='\0';
  					  outDoc->writeString(c);
  					}
<DocLatexScan>("\\"|"@")"endlatexonly"/[^a-z_A-Z0-9] {
  					  outDoc->enableAll();
					  BEGIN(DocScan);
  					}
<DocScan>"\\postheader"/{BN}
<DocScan>"\\functionindex"/{BN}		{ writeMemberList(*outDoc); }
<DocScan>"\\classhierarchy"/{BN}	{ writeClassHierarchy(*outDoc); }
<DocScan>"\\annotatedclasslist"/{BN}	{ writeAnnotatedClassList(*outDoc); }
<DocScan>"\\headerfilelist"/{BN}	{ /*TODO: fix this writeHeaderFileList(*outDoc); */ }
<DocScan>"\\header"/{BN}                { BEGIN( DocSkipWord ); }
<DocScan>"\\define"/{BN}	        { BEGIN( DocSkipWord ); }
<DocScan>("\\"|"@")"verbinclude"/{BN}	{ BEGIN( DocVerbInc ); }
<DocVerbInc>{FILE}			{
  					  verbIncludeFile(*outDoc,stripQuotes(yytext));
					  BEGIN( DocScan ); 
  					}
<DocScan>("\\"|"@")"verbatim"/[^a-z_A-Z0-9]	{
  					  outDoc->startCodeFragment();
					  BEGIN(DocVerbatim);
  					}
<DocVerbatim>("\\"|"@")"endverbatim"/[^a-z_A-Z0-9]	{
  					  outDoc->endCodeFragment();
  					  BEGIN(DocScan);
  					}
<DocVerbatim>[^\n\\]*"\n"		{
  					  //printf("docifying: %s\n",yytext);
  					  outDoc->codify(yytext);
  					}
<DocVerbatim>"\n"|"//"|"/*"|"*/"	{
					  outDoc->codify(yytext);  
  					}
<DocVerbatim>.				{
  					  //printf("char %c\n",*yytext);
					  char c[2];c[0]=*yytext;c[1]='\0';
  					  outDoc->codify(c);
  					}
<DocScan>("\\"|"@")"internal"/{BN}      { 
					  if (!internalDocsFlag)
					  {
					    outDoc->newParagraph();
					    scanString(theTranslator->trForInternalUseOnly()+"\n");
					    //outDoc->writeString("For internal use only.\n");
  					    BEGIN( DocInternal ); 
					  }
					}
<DocScan>"\\reimp"/{BN}			{
  					  outDoc->newParagraph();
					  scanString(theTranslator->trReimplementedForInternalReasons()+"\n");
  					}
<DocScan>("\\"|"@")"link"/{BN}	        { BEGIN( DocLink ); }
<DocSkipWord>[a-z_A-Z0-9.:()]+		{ BEGIN( DocScan ); }
<DocLink>[a-z_A-Z0-9:#.~/()\-\+]+       { 
  					  linkRef  = stripKnownExtensions(yytext);
					  linkText = "";
  					  BEGIN( DocLinkText ); 
					}
<DocLinkText>.				{ linkText += *yytext; }
<DocLinkText>"\n"			{ linkText += " "; }
<DocLink,DocLinkText>("\\"|"@")"endlink"/[^a-z_A-Z0-9] { // <- needed for things like \endlink. 
					  generateLink(*outDoc,className,linkRef,inSeeBlock,linkText);
  					  BEGIN( DocScan ); 
					}
<DocScan>"@ref"{B}+			{
  					  BEGIN(DocRef);
  					}
<DocRef>{SCOPENAME}			{
  					  generateLink(*outDoc,className,yytext,TRUE,0);
					  BEGIN( DocScan );
  					}
<DocScan>("\\"|"@")"endlink"/[^a-z_A-Z0-9]      { warn("Warning: \\endlink without \\link "
    					         "in documentation of %s\n", refName.data()); 
					}
<DocScan>("\\"|"@")"addindex"/{BN}		{
  					  BEGIN(DocIndexWord);
  					}
<DocScan>"\\form#"[0-9]+		{
  					  Formula *formula=formulaNameDict[yytext];
					  if (formula)
					  {
					    QString formName;
					    formName.sprintf("form-%d.gif",formula->getId());
					    outDoc->writeFormula(formName,formula->getFormulaText());
					  }
  					}
<DocIndexWord>[^\n\t ]+			{
  					  //printf("Adding %s to index\n",yytext);
					  outDoc->addToIndex(yytext,0);
  					  BEGIN(DocScan);
  					}
<DocScan>("\\"|"@")"arg"{BN}+	        { 
  					  if (insideArgumentList)
					  {
					    outDoc->writeListItem();
					  }
					  else 
					  {
					    outDoc->startItemList();
					    outDoc->writeListItem();
					    insideArgumentList=TRUE;
					  }
					}
<DocScan>("\\"|"@")"par"{B}+			{
  					  BEGIN(DocPar);
  					}
<DocPar>[^\n]*{BN}+			{
  				          endArgumentList();
					  if (inBlock()) endBlock();  
					  inParBlock=TRUE;
  					  outDoc->startDescList();
					  outDoc->startBold();
					  outDoc->docify(((QString)yytext).stripWhiteSpace());
					  outDoc->endBold();
					  outDoc->endDescTitle();
					  outDoc->writeDescItem();
					  BEGIN(DocScan);
  					}
<DocScan>("\\"|"@")"warning"{BN}+	{ 
  				          endArgumentList();
  					  if (!inWarningBlock)
					  {
					    if (inBlock()) endBlock();
					    inWarningBlock=TRUE;
  					    outDoc->startDescList();
					    //outDoc->writeBoldString("Warning: ");
					    outDoc->startBold();
					    scanString(theTranslator->trWarning()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>("\\"|"@")"bug"{BN}+		{
  				          endArgumentList();
  					  if (!inBugBlock)
					  {
					    if (inBlock()) endBlock();
					    inBugBlock=TRUE;
  					    outDoc->startDescList();
					    //outDoc->writeBoldString("Bugs and limitations: ");
					    outDoc->startBold();
					    scanString(theTranslator->trBugsAndLimitations()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>("\\"|"@")"version"{BN}+	{
  				          endArgumentList();
  					  if (!inVersionBlock)
					  {
					    if (inBlock()) endBlock();
					    inVersionBlock=TRUE;
  					    outDoc->startDescList();
					    //outDoc->writeBoldString("Version: ");
					    outDoc->startBold();
					    scanString(theTranslator->trVersion()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>("\\"|"@")"date"{BN}+		{
  				          endArgumentList();
  					  if (!inDateBlock)
					  {
					    if (inBlock()) endBlock();
					    inDateBlock=TRUE;
  					    outDoc->startDescList();
					    //outDoc->writeBoldString("Date: ");
					    outDoc->startBold();
					    scanString(theTranslator->trDate()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>("\\"|"@")"author"{BN}+	{
  				          endArgumentList();
  					  if (!inAuthorBlock)
					  {
					    if (inBlock()) endBlock();
					    inAuthorBlock=TRUE;
  					    outDoc->startDescList();
					    //outDoc->writeBoldString("Author(s): ");
					    outDoc->startBold();
					    scanString(theTranslator->trAuthors()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->docify(", ");
					  }
  					}
<DocScan>("\\"|"@")"return"{BN}+	{
  				          endArgumentList();
  					  if (!inReturnBlock)
					  {
					    if (inBlock()) endBlock();
					    inReturnBlock=TRUE;
  					    outDoc->startDescList();
					    //outDoc->writeBoldString("Returns: ");
					    outDoc->startBold();
					    scanString(theTranslator->trReturns()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
  					}
<DocScan>("\\sa"|"@see"){BN}+           { 
  				          endArgumentList();
  					  if (!inSeeBlock)
					  {
					    if (inBlock()) endBlock();
					    inSeeBlock=TRUE;
					    //firstSeeArg=TRUE;
					    //javaDocSee=!strcmp(yytext,"@see");
					    outDoc->startDescList();
					    //outDoc->writeBoldString("See also: ");
					    outDoc->startBold();
					    scanString(theTranslator->trSeeAlso()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->docify(", ");
					  }
					}
<DocScan>("\\"|"@")"param"{BN}+		{
  				          endArgumentList();
  					  if (!inParamBlock)
					  {
					    if (inBlock()) endBlock();
					    inParamBlock=TRUE;
					    outDoc->startDescList();
					    //outDoc->writeBoldString("Parameters: ");
					    outDoc->startBold();
					    scanString(theTranslator->trParameters()+": ");
					    outDoc->endBold();
  					    outDoc->endDescTitle();
					  }
  				          BEGIN(DocParam); 
  					}
<DocScan>("\\"|"@")"exception"{BN}+	{
  					  endArgumentList();
					  if (!inExceptionBlock)
					  {
					    if (inBlock()) endBlock();
					    inExceptionBlock=TRUE;
					    outDoc->startDescList();
					    //outDoc->writeBoldString("Exceptions: ");
					    outDoc->startBold();
					    scanString(theTranslator->trExceptions()+": ");
					    outDoc->endBold();
  					    outDoc->endDescTitle();
					  }
					  BEGIN(DocException);
  					}
<DocScan>"\\capt".*
<DocParam>{SCOPENAME}			{
  					  outDoc->writeDescItem();
					  outDoc->startEmphasis();
  					  outDoc->docify(yytext); 
					  outDoc->endEmphasis();
					  outDoc->docify(" - ");
					  BEGIN(DocScan);
  					}
<DocException>{SCOPENAME}		{
  					  outDoc->writeDescItem();
					  outDoc->startEmphasis();
  					  outDoc->docify(yytext); 
					  outDoc->endEmphasis();
					  outDoc->docify(" - ");
					  BEGIN(DocScan);
  					}
<DocScan>("\\"|"@")"section "{ID}"\n"	{
  					  QString secName=&yytext[9];               // skip "\section "
					  secName=secName.left(secName.length()-1); // remove \n
					  //printf("SectionName %s found\n",secName.data());
					  SectionInfo *sec;
					  if ((sec=sectionDict[secName]))
					  {
					    //printf("Title %s\n",sec->title.data());
					    outDoc->writeSection(sec->label,sec->title,sec->isSubsection);
					  }
  					}
<DocScan>"\\ref"			{
  					  BEGIN(DocRefName);
  					}
<DocScan>"\\refitem"			{
  					  BEGIN(DocRefItem);
  					}
<DocRefName>{ID}			{
  					  QString ref=yytext;
					  SectionInfo *sec;
					  if ((sec=sectionDict[ref]))
					  {
					    outDoc->writeSectionRef(sec->pageName,sec->label,sec->title);
					  }
					  else
					  {
					    warn("Warning: reference to unknown section %s!\n",yytext);
					    outDoc->writeBoldString("unknown reference!");
					  }
					  BEGIN(DocScan);
  					}
<DocRefItem>{ID}			{
  					  sectionRef=yytext;
					  BEGIN(DocRefItemName);
  					}
<DocRefItemName>.*/"\n"			{
					  SectionInfo *sec;
					  if ((sec=sectionDict[sectionRef]))
					  {
					    outDoc->writeSectionRefItem(sec->pageName,sec->label,yytext);
					  }
					  else
					  {
					    warn("Warning: reference to unknown section %s!\n",sectionRef.data());
					    outDoc->writeBoldString("unknown reference!");
					  }
					  BEGIN(DocScan);
  					}
<DocScan>("\\"|"@")"code"/{BN}+	        { 
					  outDoc->startCodeFragment();
  					  codeBlock.resize(0);
					  BEGIN( DocCodeBlock ); 
					}
<DocScan>("\\"|"@")"endcode"/[^a-z_A-Z0-9] 	{ 
  					  warn("Warning: \\endcode without <PRE> or \\code "
    					       "in the documentation of %s\n",refName.data()); 
					}
<DocScan>{SCOPEMASK}"("[a-z_A-Z,\<\> \t\*\&]+")"	{
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
  					}
<DocScan>{SCOPEMASK}(("()")?)          	{ 
					  generateRef(*outDoc,className,yytext,inSeeBlock);
					} 
<DocScan>({SCOPEMASK}"::")?"operator"[^(\r\n.,]*"("[a-z_A-Z,\<\> \t\*\&]*")"	{ 
    					  QString oName=yytext;
					  generateRef(*outDoc,className,
					              removeRedundantWhiteSpace(oName),inSeeBlock);
					}
<DocScan>({SCOPEMASK}"::")?"operator()("[a-z_A-Z,\<\> \t\*\&]*")"	{ 
				          QString oName=yytext;
					  generateRef(*outDoc,className,
					              removeRedundantWhiteSpace(oName),inSeeBlock);
					}
<DocScan>{FILEMASK}			{
					  generateFileRef(*outDoc,yytext);
					}
<DocCodeBlock>{BN}*("\\"|"@")"endcode"/[^a-z_A-Z0-9]  { // needed to match things like \endcode. (note the dot)
				          parseCode(*outDoc,className,codeBlock,exampleDoc,exampleName);
					  //printf("Code block\n-------------\n%s\n--------------\n",codeBlock.data());
					  outDoc->endCodeFragment();
					  BEGIN( DocScan ); 
					}
<DocCodeBlock>"</"{PRE}{ATTR}">"	{ 
				          parseCode(*outDoc,className,codeBlock,exampleDoc,exampleName);
					  //printf("Code block\n-------------\n%s\n--------------\n",codeBlock.data());
					  outDoc->endCodeFragment();
					  BEGIN( DocScan ); 
					}
<DocScan>("\\"|"@")"e"{BN}+	                { BEGIN( DocEmphasis ); }
<DocScan>("\\"|"@")"a"{BN}+                     { BEGIN( DocEmphasis ); }
<DocScan>("\\"|"@")"b"{BN}+			{ BEGIN( DocBold ); }
<DocScan>("\\"|"@")"c"{BN}+                     { BEGIN( DocCode ); }
<DocScan>("\\"|"@")"l"{BN}+		
<DocScan>("\\"|"@")"include"{BN}+		{ BEGIN( DocInclude ); }
<DocScan>("\\"|"@")"dontinclude"{BN}+		{ BEGIN( DocDontInclude ); }
<DocScan>("\\"|"@")"skip"{BN}+		 	{ BEGIN( DocSkipKey ); }	
<DocScan>("\\"|"@")"skipline"{BN}+		{ BEGIN( DocSkiplineKey ); firstLine=TRUE; }
<DocScan>("\\"|"@")"line"{BN}+			{ BEGIN( DocLineKey ); firstLine=TRUE; }
<DocScan>("\\"|"@")"until"{BN}+			{ BEGIN( DocUntilKey ); firstLine=TRUE; }
<DocSkipKey>[^ \t\r\n]+			{ 
  					  if (includeFileLength>0) 
					    skipUntil(yytext); 
  					  BEGIN( DocScan );
					}
<DocLineKey>[^ \t\r\n]+			{ 
  					  if (includeFileLength>0) 
					  {
					    if (firstLine) outDoc->startCodeFragment();
					    firstLine=FALSE;
					    showLine(*outDoc,yytext); 
					    BEGIN( DocKeyEnd );
					  }
					  else
					  {
  					    BEGIN( DocScan );
					  }
					}
<DocSkiplineKey>[^ \t\r\n]+		{ 
  					  if (includeFileLength>0) 
					  {
					    if (firstLine) outDoc->startCodeFragment();
					    firstLine=FALSE;
					    skipLine(*outDoc,yytext); 
					    BEGIN( DocKeyEnd );
					  }
					  else
					  {
  					    BEGIN( DocScan );
					  }
					}
<DocUntilKey>[^ \t\r\n]+		{ 
  					  if (includeFileLength>0) 
					  {
					    if (firstLine) outDoc->startCodeFragment();
					    firstLine=FALSE;
					    showUntil(*outDoc,yytext); 
					    BEGIN( DocKeyEnd );
					  }
					  else
					  {
  					    BEGIN( DocScan );
					  }
					}
<DocKeyEnd>("\\"|"@")"line"{BN}+		{ BEGIN(DocLineKey); }
<DocKeyEnd>("\\"|"@")"until"{BN}+		{ BEGIN(DocUntilKey); }
<DocKeyEnd>("\\"|"@")"skipline"{BN}+		{ BEGIN(DocSkiplineKey); }
<DocKeyEnd>\n
<DocKeyEnd>.				{
  					  unput(*yytext);
  					  if (!firstLine) outDoc->endCodeFragment();
					  BEGIN( DocScan );
  					}
<DocScan>"<"{MULTICOL}{ATTR}">"		
<DocScan>"</"{MULTICOL}{ATTR}">"
<DocScan>"<"{STRONG}{ATTR}">"		{ outDoc->startBold(); }
<DocScan>"</"{STRONG}{ATTR}">"		{ outDoc->endBold(); }
<DocScan>"<"{CENTER}{ATTR}">"		{ outDoc->startCenter(); }
<DocScan>"</"{CENTER}{ATTR}">"		{ outDoc->endCenter(); }
<DocScan>"<"{TABLE}{ATTR}">"		{ 
  					  startTable();
  					  /*outDoc->startTable();*/ }
<DocScan>"</"{TABLE}{ATTR}">"		{
 					  endTable(); 
  					  /*outDoc->endTable();*/ 
					}
<DocScan>"<"{INPUT}{ATTR}">"
<DocScan>"<"{SMALL}{ATTR}">"		{ outDoc->startSmall(); }
<DocScan>"</"{SMALL}{ATTR}">"		{ outDoc->endSmall(); }
<DocScan>"<"{META}{ATTR}">"
<DocScan>"<"{FORM}{ATTR}">"
<DocScan>"</"{FORM}{ATTR}">"
<DocScan>"<"{HEAD}{ATTR}">"			
<DocScan>"</"{HEAD}{ATTR}">"			
<DocScan>"<"{BODY}{ATTR}">"
<DocScan>"</"{BODY}{ATTR}">"
<DocScan>"<"{CODE}{ATTR}">"		{ outDoc->startTypewriter(); }
<DocScan>"</"{CODE}{ATTR}">"		{ outDoc->endTypewriter(); }
<DocScan>"<"{DFN}{ATTR}">"		{ outDoc->startTypewriter(); }
<DocScan>"</"{DFN}{ATTR}">"		{ outDoc->endTypewriter(); }
<DocScan>"<"{VAR}{ATTR}">"		{ outDoc->startEmphasis(); }
<DocScan>"</"{VAR}{ATTR}">"		{ outDoc->endEmphasis(); }
<DocScan>"<"{IMG}{ATTR}">"		{ outDoc->disableAllBut(OutputGenerator::Html);
					  outDoc->writeString(yytext); 
					  outDoc->enableAll();
					}
<DocScan>"<"{PRE}{ATTR}">"			{ 
  					  outDoc->startCodeFragment();
  					  codeBlock.resize(0);
					  BEGIN( DocCodeBlock ); 
					}
<DocScan>"</"{PRE}{ATTR}">"		 	{ 
  					  warn("Warning: </PRE> without <PRE> or \\code"
    					         "in the documentation of %s\n",refName.data()); 
					}
<DocScan>"<"{SUB}{ATTR}">"		{ outDoc->startSubscript(); }
<DocScan>"</"{SUB}{ATTR}">"		{ outDoc->endSubscript(); }
<DocScan>"<"{SUP}{ATTR}">"		{ outDoc->startSuperscript(); }
<DocScan>"</"{SUP}{ATTR}">"		{ outDoc->endSuperscript(); }
<DocScan>"<"{TR}{ATTR}">"		{ if (curTable) curTable->newRow(); }
<DocScan>"</"{TR}{ATTR}">"			
<DocScan>"<"{TD}{ATTR}">"		{ if (curTable) curTable->newElem(); }
<DocScan>"</"{TD}{ATTR}">"
<DocScan>"<"{OL}{ATTR}">"		{ outDoc->startEnumList(); }
<DocScan>"</"{OL}{ATTR}">"		{ outDoc->endEnumList(); }
<DocScan>"<"{UL}{ATTR}">"		{ outDoc->startItemList(); }
<DocScan>"</"{UL}{ATTR}">"		{ outDoc->endItemList(); }
<DocScan>"<"{LI}{ATTR}">"		{ outDoc->writeListItem(); }
<DocScan>"</"{LI}{ATTR}">"		
<DocScan>"<"{TT}{ATTR}">"		{ outDoc->startTypewriter(); }
<DocScan>"</"{TT}{ATTR}">"		{ outDoc->endTypewriter(); }
<DocScan>"<"{EM}{ATTR}">"		{ outDoc->startEmphasis(); }
<DocScan>"</"{EM}{ATTR}">"		{ outDoc->endEmphasis(); }
<DocScan>"<"{HR}{ATTR}">"		{ outDoc->writeRuler(); }
<DocScan>"<"{DL}{ATTR}">"		{ outDoc->startDescription(); }
<DocScan>"</"{DL}{ATTR}">"		{ outDoc->endDescription(); }
<DocScan>"<"{DT}{ATTR}">"		{ outDoc->startDescItem(); }
<DocScan>"</"{DT}{ATTR}">"	
<DocScan>"<"{DD}{ATTR}">"		{ outDoc->endDescItem(); }
<DocScan>"</"{DD}{ATTR}">"
<DocScan>"<"{BR}{ATTR}">"		{ outDoc->lineBreak(); }
<DocScan>"<"{I}{ATTR}">"		{ outDoc->startEmphasis(); }
<DocScan>"</"{I}{ATTR}">"		{ outDoc->endEmphasis(); }
<DocScan>"</"{A}{ATTR}">"			
<DocScan>"<"{A}				{ BEGIN(DocHtmlLink); }
<DocScan>"<"{BOLD}{ATTR}">"		{ outDoc->startBold(); }
<DocScan>"</"{BOLD}{ATTR}">"		{ outDoc->endBold(); }
<DocScan>"<"{P}{ATTR}">"		{ 
  					  if (inBlock()) endBlock();
  					  outDoc->newParagraph(); }
<DocScan>"</"{P}{ATTR}">"
<DocScan>"<"{H1}{ATTR}">"		{ outDoc->startTitle(); }
<DocScan>"</"{H1}{ATTR}">"		{ outDoc->endTitle(); }
<DocScan>"<"{H2}{ATTR}">"		{ outDoc->startSubsection(); }
<DocScan>"</"{H2}{ATTR}">"		{ outDoc->endSubsection(); }
<DocScan>"<"{H3}{ATTR}">"		{ outDoc->startSubsubsection(); }
<DocScan>"</"{H3}{ATTR}">"		{ outDoc->endSubsubsection(); }
<DocHtmlLink>{NAME}{BN}*"="{BN}*("\""?)	{ BEGIN(DocHtmlAnchor); }
<DocHtmlAnchor>[a-z_A-Z0-9.\-\+\/]+	{ outDoc->writeAnchor(yytext); } 
<DocHtmlLink>{HREF}{BN}*"="{BN}*("\""?)	{ 
  					  htmlUrl.resize(0);
					  htmlText.resize(0);
  					  BEGIN(DocHtmlHref1); }
<DocHtmlHref1>{URLMASK}+		{ 
  					  htmlUrl=yytext; 
					}
<DocHtmlHref1>">"			{ BEGIN(DocHtmlHref2); }
<DocHtmlHref2>[^<]*			{ htmlText+=yytext; }
  /*
<DocHtmlHref2>\n			{ htmlText+='\n'; }
  */
<DocHtmlHref2>"<"			{ 
  					  outDoc->writeHtmlLink(htmlUrl,htmlText);
  					  unput(*yytext); 
					  BEGIN(DocScan); 
					}
<DocHtmlLink,DocHtmlAnchor>">"		{ BEGIN(DocScan); }

<DocScan>"\\\\"				{
  					  outDoc->docify("\\");
  					}
<DocScan>"\\@"				{
  					  outDoc->docify("@");
  					}
<DocScan>"\\<"			        { 
					  outDoc->docify("<");
					}
<DocScan>"\\>"				{ 
  					  outDoc->docify(">");
					}
<DocScan>"\\&"				{ 
  					  outDoc->docify("&");
					}
<DocScan>"\\$"				{
  					  outDoc->docify("$");
  					}
<DocScan>"\\#"				{
  					  outDoc->docify("#");
  					}
<DocScan>"\\%"				{
  					  outDoc->docify("%");
  					}
<DocScan>"%"[a-zA-Z_0-9\-]+		{
  					  outDoc->docify(yytext+1);
  					}
<DocEmphasis>[^ \t\n\r]+		{ 
  					  outDoc->startEmphasis();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
					  //addToIndex(yytext);
					}
<DocBold>[^ \t\n\r]+			{
  					  outDoc->startBold();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endBold();
					  BEGIN( DocScan );
  					}
<DocCode>[^ \t\n\r]+			{ 
					  outDoc->startTypewriter();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endTypewriter();
					  BEGIN( DocScan );
					}
<DocInclude>{FILE}			{
					  includeFile(*outDoc,stripQuotes(yytext),FALSE); 
					  BEGIN( DocScan );
  					}
<DocDontInclude>{FILE}			{
  					  includeFile(*outDoc,stripQuotes(yytext),TRUE);
					  BEGIN( DocScan );
  					}
<DocCodeBlock>"//"			{ codeBlock += yytext; }
<DocCodeBlock>"/*"			{ codeBlock += yytext; }
<DocCodeBlock>\n			{ codeBlock += '\n'; }
<DocCodeBlock>[^\/\\\<\n]*		{ codeBlock += yytext; }
<DocCodeBlock>.				{ codeBlock += *yytext; }
<DocCode,DocEmphasis,DocScan,DocBold>"//"       { 
					  outDoc->docify(yytext);
					}
<DocCode,DocEmphasis,DocScan,DocBold>"/*"       { 
  					  outDoc->docify(yytext);
					}
<DocCode,DocEmphasis,DocBold>"\n"	{ outDoc->writeChar('\n'); }
<DocScan>({B}*"\n"{B}*){2,}		{ 
  					  if (insideArgumentList)
					  {
					    insideArgumentList=FALSE;
					    outDoc->endItemList();
					  }
					  else
					  {
					    outDoc->newParagraph(); 
					  }
					  if (inBlock()) endBlock();
					}
<DocScan>{BN}+				{ 
  					  outDoc->writeChar(' '); 
					}
<DocCode,DocEmphasis,DocBold,DocScan>.  { 
					  outDoc->writeChar(*yytext);
					}
<NextSemi>"{"				{ BEGIN(SkipBlock); }
<SkipBlock>"{"				{ ++bracketCount ; }
<SkipBlock>"}"				{ if( bracketCount )
					    --bracketCount ;
					  else
					    BEGIN( NextSemi ) ;
					}
<NextSemi>\"				{
  					  lastStringContext=NextSemi;
					  BEGIN(SkipString);
  					}
<NextSemi>[;,]				{ 
  					  unput(*yytext);
  					  BEGIN( FindMembers ) ; 
					}
<FindMembers>{B}*"signals"{BN}*":"{BN}* { current->sig = sig = TRUE;
					  current->slot = slot = FALSE;
					  current->protection = protection = Public ;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}

<FindMembers>{B}*"public"{BN}*"slots"{BN}*":"{BN}* {
					  current->protection = protection = Public ;
					  current->slot = slot = TRUE;
					  current->sig = sig = FALSE;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount();
					}

<FindMembers>{B}*"protected"{BN}*"slots"{BN}*":"{BN}* {
					  current->protection = protection = Protected ;
					  current->slot = slot = TRUE;
					  current->sig = sig = FALSE;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount();
					}

<FindMembers>{B}*"private"{BN}*"slots"{BN}*":"{BN}* {
					  current->protection = protection = Private ;
					  current->slot = slot = TRUE;
					  current->sig = sig = FALSE;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount();
					}

<FindMembers>{B}*"public"{BN}*":"{BN}*	{ 
					  current->protection = protection = Public ;
					  current->slot = slot = FALSE;
					  current->sig = sig = FALSE;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}
<FindMembers>{B}*"protected"{BN}*":"{BN}* { 
  					  current->protection = protection = Protected ;
					  current->slot = slot = FALSE;
					  current->sig = sig = FALSE;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}
<FindMembers>{B}*"private"{BN}*":"{BN}*	{ 
  					  current->protection = protection = Private ;
					  current->slot = slot = FALSE;
					  current->sig = sig = FALSE;
					  current->type.resize(0); 
					  current->name.resize(0); 
					  current->args.resize(0);
					  current->argList->clear();
					  lineCount() ;
					}
  /*
<FindMembers>"inline"     		
  */
<FindMembers>{B}*"static"{BN}+     	{ //current->type += " static ";
  					  current->stat = TRUE;
					  lineCount();
					}
<FindMembers>{B}*"extern"{BN}+		{
  					  current->stat = FALSE;
					  lineCount();
  					}
<FindMembers>{B}*"virtual"{BN}+    	{ current->type += " virtual ";
					  current->virt = Virtual;
					  lineCount();
					}
<FindMembers>{B}*"inline"{BN}+		{ lineCount(); }
<FindMembers>{B}*"typename"{BN}+	{ lineCount(); }
<FindMembers>{B}*"namespace"{BN}+	{ 
  					  isTypedef=FALSE;
					  current->section = Entry::NAMESPACE_SEC;
					  current->type = "namespace" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  lineCount();
  					  BEGIN( ClassName ); 
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"class"{BN}+ { 
					  isTypedef=((QString)yytext).find("typedef")!=-1;
  					  current->section = Entry::CLASS_SEC ;
					  addType( current ) ;
					  current->type += " class" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"struct"{BN}+ { 
					  isTypedef=((QString)yytext).find("typedef")!=-1;
  					  current->section = Entry::STRUCT_SEC ;
					  addType( current ) ;
					  current->type += " struct" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"union"{BN}+ { 
					  isTypedef=((QString)yytext).find("typedef")!=-1;
  					  current->section = Entry::UNION_SEC ;
					  addType( current ) ;
					  current->type += " union" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"enum"{BN}+	{ 
					  isTypedef=((QString)yytext).find("typedef")!=-1;
  					  current->section = Entry::ENUM_SEC ;
					  addType( current ) ;
					  current->type += " enum" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<Operator>"("{BN}*")"{BN}*/"("		{
  					  lineCount();
    					  current->name += yytext ;
					  current->name = current->name.simplifyWhiteSpace();
					  BEGIN( FindMembers ) ;
  					}
<Operator>[^(]				{ 
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
					  if (current->tArgList)
					  {
					    //printf("scanner.l current->tArgList->clear() %p\n",current->tArgList);
					    current->tArgList->clear();
					  }
					  else
					  {
					    current->tArgList = new ArgumentList;
					    current->tArgList->setAutoDelete(TRUE);
					    //printf("scanner.l new tArgList %p\n",current->tArgList);
					  }
					  templateStr="<";
					  copyArgString=&templateStr;
					  currentArgumentContext = FindMembers;
					  //printf("Start template list\n");
					  BEGIN( ReadTempArgs );
  					}
  /*
<FindMembers>({ID}{BN}*"::")*{BN}*"operator"{BN}*[<=]+ {
					  lineCount();
					  addType( current );
					  current->name = yytext;
					}
<FindMembers>"operator"{BN}*"/"[=]?/[^/ *] {  space added!
					  lineCount();
					  addType( current );
					  current->name = yytext;
					}
<FindMembers>"operator"{BN}*"({B}*)"{BN}* /"(" {  space added!  
					  lineCount();
					  addType( current );
					  current->name = yytext;
					}
<FindMembers>"operator"{BN}*[^(/]+	{ 
  					  lineCount();
  				          addType( current ) ;
					  current->name  = yytext ;
					}
  */
<FindMembers>"using"{BN}+		{ lineCount(); BEGIN(Using); }
<Using>";"				{ BEGIN(FindMembers); }
<FindMembers>{SCOPENAME}{BN}*"<>"	{ // guided template decl
					  QString n=yytext;
					  addType( current );
					  current->name=n.left(n.length()-2);
					}
<FindMembers>{SCOPENAME}{BN}*/"<"	{
  					  sharpCount=0;
					  lineCount();
					  addType( current );
  					  current->name=yytext;
					  current->name=current->name.stripWhiteSpace();
					  if (current->name.right(8)=="operator") 
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
					}
<EndTemplate>"<"			{ 
  					  current->name+='<'; 
					  sharpCount++; 
					}
  /*
<EndTemplate>">"({BN}*"::"{SCOPENAME})?	{
  */
<EndTemplate>">"			{
					  current->name+=*yytext;
					  if (--sharpCount<=0)
					  {  
					    //printf("Found %s\n",current->name.data());
					    BEGIN(FindMembers);
					  }
					}
<EndTemplate>">"{BN}*"("		{ 
  					  lineCount();
					  current->name+='>';
					  if (--sharpCount<=0)
					  {
					    current->args = "(";
					    currentArgumentContext = FuncQual;
					    copyArgString=&current->args;
					    //printf("Found %s\n",current->name.data());
					    BEGIN( ReadFuncArgType ) ;
					  }
					}
<EndTemplate>">"{BN}*/"::"		{
  					  lineCount();
  					  current->name+='>';
					  if (--sharpCount<=0)
					  {
					    BEGIN(FindMemberName);
					  }
  					}
<EndTemplate>.				{ current->name+=*yytext; }
<FindMembers,FindMemberName>{SCOPENAME}	{ 
					  lineCount();
					  if (YY_START==FindMembers)
					  {
					    addType( current ) ;
					    current->name  = yytext;
					  }
					  else
					  {
					    current->name += yytext;
					  }
				          QString tmp=yytext;
					  if (tmp.right(8)=="operator") 
					    BEGIN( Operator );
					  else
					    BEGIN(FindMembers);
					}
<FindMembers>^{B}*"#"			{ lastCPPContext = YY_START; 
					  BEGIN( SkipCPP ) ; 
					}
<FindMembers>^{B}*"#define"		{
  					  BEGIN( Define );
  					}
<SkipCPP>.
<SkipCPP>\\[\r]*"\n"[\r]*		{ yyLineNr++ ; }
<SkipCPP>[\r]*\n[\r]*			{ yyLineNr++ ;
					  BEGIN( lastCPPContext) ;
					}
<Define>{ID}/"("			{
  					  current->name = yytext;
					  BEGIN( DefineArg );
  					}
<DefineArg>")"				{
  					  //printf("Define with args\n");
  					  current->args += ')';
  					  BEGIN( DefineEnd );
  					}
<DefineArg>.				{
  					  current->args += *yytext;
  					}
<Define>{ID}				{
  					  //printf("Define `%s' without args\n",yytext);
  					  current->name = yytext;
					  BEGIN(DefineEnd);
  					}
<DefineEnd>\n				{
  					  //printf("End define\n");
					  current->fileName   = yyFileName;
					  current->startLine  = yyLineNr;
					  current->type.resize(0);
  					  current->args       = current->args.simplifyWhiteSpace();
  					  current->name       = current->name.stripWhiteSpace();
					  current->section    = Entry::DEFINE_SEC;
					  current_root->addSubEntry(current);
					  current             = new Entry ;
					  current->protection = protection ;
				          current->sig        = sig;
					  current->virt       = virt;
					  current->stat       = gstat;
					  current->slot       = slot;
					  BEGIN(FindMembers);
  					}
<DefineEnd>\\\n
<DefineEnd>.				
					
<FindMembers>[*&]+			{ current->name += yytext ; }
<FindMembers,MemberSpec,Function,NextSemi>";"{BN}*("/**"|"//!"|"/*!")"<" {
  					  lineCount();
					  current->doc.resize(0);
					  current->brief.resize(0);
					  lastAfterDocContext = YY_START;
					  afterDocTerminator = ';';
					  if (yytext[yyleng-3]=='/')
					    BEGIN(AfterDocLine);
					  else if (yytext[yyleng-2]=='*')
					    BEGIN(AfterDocBrief);
					  else
					    BEGIN(AfterDoc);
  					}
<MemberSpec,FindFields,FindMembers,NextSemi>","{BN}*("/**"|"//!"|"/*!")"<" {
  					  lineCount();
					  current->doc.resize(0);
					  current->brief.resize(0);
					  lastAfterDocContext = YY_START;
					  afterDocTerminator = ',';
					  if (yytext[yyleng-3]=='/')
					    BEGIN(AfterDocLine);
					  else if (yytext[yyleng-2]=='*')
					    BEGIN(AfterDocBrief);
					  else
					    BEGIN(AfterDoc);
  					}
<DefineEnd,FindFields,FindFieldArg>{BN}*("/**"|"//!"|"/*!")"<" {
  					  lineCount();
					  current->doc.resize(0);
					  current->brief.resize(0);
					  lastAfterDocContext = YY_START;
					  if (YY_START==DefineEnd)
					    afterDocTerminator = '\n';
					  else
					    afterDocTerminator = 0;
					  if (yytext[yyleng-3]=='/')
					    BEGIN(AfterDocLine);
					  else if (yytext[yyleng-2]=='*')
					    BEGIN(AfterDocBrief);
					  else
					    BEGIN(AfterDoc);
  					}
<FindMembers>"="			{
  					  BEGIN(NextSemi);
  					}
<FindMembers>[:;,]			{ 
  					  QString oldType = current->type.copy();
					  QString oldDocs = current->doc.copy();
  					  current->type=current->type.simplifyWhiteSpace();
  					  current->args=current->args.simplifyWhiteSpace();
  					  current->name=current->name.stripWhiteSpace();
  					  current->section = Entry::VARIABLE_SEC ;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  current_root->addSubEntry( current ) ;
					  current = new Entry ;
					  // variable found 
					  current->section = Entry::EMPTY_SEC ;
  					  current->protection = protection;
					  current->slot = slot = FALSE;
					  current->sig = sig = FALSE;
					  current->virt = Normal; 
					  current->stat = gstat;
					  // skip expression or bitfield if needed
					  if ( *yytext == ':') 
					    BEGIN( NextSemi );
					  else 
					  {
					    if ( *yytext == ',' )
					    {
					      int i=oldType.length(); 
					      while (i>0 && (oldType[i-1]=='*' || oldType[i-1]==' ')) i--;
					      current->type = oldType.left(i);
					      current->doc  = oldDocs;
					    }
					    BEGIN( FindMembers ) ;
					  }
					}

<FindMembers>"["			{ current->args += yytext ;
					  sharpCount=1;
					  BEGIN( Array ) ;
					}
<Array>"]"				{ current->args += *yytext ;
					  if (--sharpCount<=0)
	                                     BEGIN( FindMembers ) ;
					}
<Array>"["				{ current->args += *yytext ;
					  sharpCount++;	
					}
<Array>.				{ current->args += *yytext ; }

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
  					  BEGIN(FindFieldArg);
  					}
<FindFields>","				{
					  //printf("adding `%s' `%s' `%s' to enum `%s'\n",
					  //     current->type.data(), current->name.data(),
					  //     current->args.data(), current_root->name.data());
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
					  current->protection = protection ;
				          current->sig        = sig;
					  current->virt       = virt;
					  current->stat       = gstat;
					  current->slot       = slot;
  					}
<FindFieldArg>","			{ unput(*yytext); BEGIN(FindFields); }
<Curly>[^\r\n{}"/]*			{ current->program += yytext ; }
<Curly>"//".*				{ current->program += yytext ; }
<Curly>\"[^\r\n"]*\"			{ current->program += yytext ; }
<Curly>"/*"{B}*				{ current->program += yytext ;
					  lastContext = Curly ;
					  BEGIN( Comment ) ;
					}
<Curly>"/*"{BL}				{ current->program += yytext ;
					  ++yyLineNr ;
					  lastContext = Curly ;
					  BEGIN( Comment ) ;
					}
<Curly>"{"				{ current->program += yytext ;
					  ++bracketCount ;
					}
<Curly>"}"				{ if ( bracketCount )
					  {
					    current->program += yytext ;
					    --bracketCount ;
					  }
					  else
					  {
					    QString &cn = current->name;
					    QString &rn = current_root->name;
					    //printf("current->name=`%s' current_root->name=`%s'\n",
					    //	   cn.data(),rn.data());
					    if (cn && cn[0]!='@' &&  
					      (current_root->section & Entry::SCOPE_MASK))
					    {
					      cn.prepend(rn+"::");
					    }
					    if (isTypedef && cn.length()==0)
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
					      if (current->section == Entry::NAMESPACE_SEC)
					      { // a namespace ends with a closing bracket
						current->reset();
						current->protection = protection ;
						current->sig = sig;
						current->virt = virt;
						current->stat = gstat;
						current->slot = slot;
						BEGIN( FindMembers ) ;
					      }
					      else
						BEGIN( MemberSpec ) ;
					    }
					  }
					}
<TypedefName>{ID}			{
  					  if (current->section == Entry::ENUM_SEC)
					  {
					    current->program+=","; // add field terminator
					  }
  				          current->name=yytext;
					  if (current_root->section & Entry::SCOPE_MASK)
					  {
					    current->name.prepend(current_root->name+"::");
					  }
  					  current->args = current->args.simplifyWhiteSpace();
  					  current->type = current->type.simplifyWhiteSpace();
					  //printf("Adding compound %s %s %s\n",current->type.data(),current->name.data(),current->args.data());
					  current_root->addSubEntry( current ) ;
					  current = new Entry;
					  current->protection = protection ;
					  current->sig = sig;
					  current->virt = virt;
					  current->stat = gstat;
					  current->slot = slot;
  					  BEGIN(MemberSpecSkip); 
  					}
<MemberSpec>([*&]*{BN}*)*{ID}("["[a-z_A-Z0-9]*"]")* { // the [] part could be improved.
  					  lineCount();
  					  int i=0,l=yyleng,j;
					  while (i<l && (!isalpha(yytext[i]))) i++;
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
					  if (msName.length()>0)
					  {
  					    Entry *varEntry=new Entry;
					    varEntry->protection = current->protection ;
					    varEntry->sig = current->sig;
					    varEntry->virt = current->virt;
					    varEntry->stat = current->stat;
					    varEntry->slot = current->slot;
					    varEntry->section = Entry::VARIABLE_SEC;
					    varEntry->name = msName.stripWhiteSpace();
					    varEntry->type = current->type.simplifyWhiteSpace()+" ";
					    varEntry->args = msArgs; //current->args.simplifyWhiteSpace();
					    //if (current->name.length()>0 && current->name[0]!='@' && 
					    //    current->parent->section & Entry::COMPOUND_MASK)
					    //  varEntry->type+=current->parent->name+"::";
					    if (isTypedef)
					      varEntry->type.prepend("typedef ");
					    varEntry->type+=current->name+msType;
					    varEntry->fileName = yyFileName;
					    varEntry->startLine = yyLineNr;
					    varEntry->doc = current->doc.copy();
					    varEntry->brief = current->brief.copy();
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
					    current->protection = protection ;
					    current->sig = sig;
					    current->virt = virt;
					    current->stat = gstat;
					    current->slot = slot;
					    BEGIN( FindMembers );
					  }
  					}					
<MemberSpec>"="				{ BEGIN(MemberSpecSkip); }
<MemberSpecSkip>"{"			{
  					  bracketCount=0;
					  lastCurlyContext = MemberSpecSkip;
  					  BEGIN(SkipCurly);
  					}
<MemberSpecSkip>","			{ BEGIN(MemberSpec); }
<MemberSpecSkip>";"		        { unput(';'); BEGIN(MemberSpec); }
<Curly>\n				{ current->program += yytext ;
					  yyLineNr++ ;
					}
<Curly>.				{ current->program += yytext ; }

<FindMembers>"("({ID}{BN}*"::"{BN}*)*("*"{BN}*)+ {
  					  lineCount();
  					  addType(current);
					  funcPtrType=yytext;
					  //current->type += yytext;
					  BEGIN( FuncPtr );
  					}
<FuncPtr>{SCOPENAME}			{
  					  current->name = yytext;
					  BEGIN( EndFuncPtr );
  					}
<FuncPtr>.				{
  					  //printf("Error: FuncPtr `%c' unexpected at line %d of %s\n",*yytext,yyLineNr,yyFileName);
  					}
<EndFuncPtr>")"{BN}*/";"		{ // a variable with extra braces
 					  lineCount();
					  current->type+=funcPtrType.data()+1;
  					  BEGIN(FindMembers);
  					}
<EndFuncPtr>")"{BN}*/"("		{ // a variable function
  					  lineCount();
					  current->type+=funcPtrType+")";
					  BEGIN(FindMembers);
  					}
<EndFuncPtr>"("				{ // a function returning a function
  					  current->args += *yytext ;
					  bracketCount=0;
					  BEGIN( FuncFunc );
  					}
<EndFuncPtr>")"				{
  					  BEGIN(FindMembers);
  					}
<FuncFunc>"("				{
  					  current->args += *yytext ;
  					  ++bracketCount;
					}
<FuncFunc>")"				{
  					  current->args += *yytext ;
  					  if ( bracketCount )
					    --bracketCount;
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
					  bracketCount++;
  					}
<FuncFuncType>")"			{
  					  current->type += *yytext;
  					  if (bracketCount)
					    --bracketCount;
					  else
					    BEGIN(Function);
					}
<FuncFuncType>{BN}*","{BN}*		{ lineCount() ; current->type += ", " ; }
<FuncFuncType>{BN}+			{ lineCount() ; current->type += ' ' ; }
<FuncFuncType>.				{
  					  current->type += *yytext;
  					}
<FindMembers>"("			{ current->args = yytext;
					  currentArgumentContext = FuncQual;
					  copyArgString=&current->args;
					  BEGIN( ReadFuncArgType ) ;
					  //printf(">>> Read function arguments!\n");
					}
  /*
<FindMembers>"("{BN}*("void"{BN}*)?")"	{
  					  lineCount();
  					  current->args = "()"; 
  					  BEGIN( FuncQual );
  					}
  */

  /*- Function argument reading rules ---------------------------------------*/

<ReadFuncArgType>[^ \/\r\t\n\)\(\"\']+	{ *copyArgString+=yytext; }
<CopyArgString>[^\n\\\"\']+		{ *copyArgString+=yytext; }
<CopyArgRound>[^\/\n\)\(\"\']+		{ *copyArgString+=yytext; }
<ReadFuncArgType,ReadTempArgs>{BN}*	{
  					  *copyArgString+=" ";
  					  lineCount();
  					}
<ReadFuncArgType,CopyArgRound,CopyArgSharp,ReadTempArgs>\"	{
  					  *copyArgString+=*yytext;
					  lastCopyArgStringContext = YY_START;
  					  BEGIN( CopyArgString );
  					}
<ReadFuncArgType,ReadTempArgs>"("	{
  					  *copyArgString+=*yytext;
  					  argRoundCount=0; 
					  lastCopyArgContext = YY_START;
					  BEGIN( CopyArgRound ); 
  					}
<ReadFuncArgType>")"			{ 
  					  *copyArgString+=*yytext;
					  stringToArgumentList(*copyArgString,current->argList);
					  BEGIN( currentArgumentContext );
					}
<ReadTempArgs>"<"			{
					  *copyArgString+=*yytext;
					  argSharpCount=0;
					  BEGIN( CopyArgSharp );
					}
<ReadTempArgs>">"			{
					  *copyArgString+=*yytext;
					  //printf("end template list %s\n",copyArgString->data());
					  stringToArgumentList(*copyArgString,current->tArgList);
					  BEGIN( currentArgumentContext );
					}
<CopyArgRound>"("			{
  					  argRoundCount++;
					  *copyArgString+=*yytext;
  					}
<CopyArgRound>")"			{
					  *copyArgString+=*yytext;
					  if (argRoundCount>0) 
					    argRoundCount--;
					  else 
					    BEGIN( lastCopyArgContext );
  					}
<CopyArgSharp>"<"			{
  					  argSharpCount++;
					  *copyArgString+=*yytext;
  					}
<CopyArgSharp>">"			{
  					  *copyArgString+=*yytext;
					  if (argRoundCount>0)
					    argRoundCount--;
					  else
					    BEGIN( ReadTempArgs );
  					}
<CopyArgString>\\.			{
  					  *copyArgString+=yytext;
  					}
<CopyArgString>\"			{
  					  *copyArgString+=*yytext;
					  BEGIN( lastCopyArgStringContext );
  					}
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>"'"\\[0-7]{1,3}"'" { *copyArgString+=yytext; }
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>"'"\\."'"   { *copyArgString+=yytext; }
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>"'"."'"     { *copyArgString+=yytext; }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgRound,CopyArgSharp>\n  { yyLineNr++; *copyArgString+=*yytext; }
<ReadFuncArgType,ReadTempArgs,CopyArgString,CopyArgRound,CopyArgSharp>.	  { *copyArgString+=*yytext; }



  /*------------------------------------------------------------------------*/


<FuncRound>"("				{ current->args += *yytext ;
					  ++bracketCount ;
					}
<FuncRound>")"                          { current->args += *yytext ; 
					  if ( bracketCount )
					    --bracketCount ;
				          else
					    BEGIN( FuncQual ) ;
					}
  /*
<FuncQual>"#"				{ lastCPPContext = YY_START;
  					  BEGIN(SkipCPP);
					}
  */
<FuncQual>[{:;]                         { unput(*yytext); BEGIN( Function ); }
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
<FuncQual>{BN}*"throw"{BN}*"("		{ 
  					  current->exception = " throw(" ;
					  lineCount() ;
					  BEGIN( ExcpRound ) ;
					}
<ExcpRound>"("				{ current->exception += *yytext ;
					  ++bracketCount ;
					}
<ExcpRound>")"                          { current->exception += *yytext ; 
					  if ( bracketCount )
					    --bracketCount ;
				          else
					    BEGIN( FuncQual ) ;
					}
<ExcpRound>.				{
  					  current->exception += yytext;
  					}
<Function>"("				{ current->type += current->name ;
					  current->name  = current->args ;
					  current->args  = yytext ;
					  BEGIN( FuncRound ) ;
					}
<Function>"#"				{ lastCPPContext = YY_START;
  					  BEGIN(SkipCPP);
					}	
<Function>[:;{]				{ 
					  current->name=current->name.simplifyWhiteSpace();
  					  current->type=current->type.simplifyWhiteSpace();
  					  current->args=current->args.simplifyWhiteSpace();
					  QString &cn=current->name;
					  QString &rn=current_root->name;
					  //printf("current_root->name=`%s'\n",rn.data());
  					  //printf("Function: `%s' `%s' `%s'\n",current->type.data(),cn.data(),current->args.data());
					  int i;
					  if ((i=cn.findRev("::"))!=-1) // name contains scope
					  {
					    if (cn.left(i)==rn.right(i)) // scope name is redundant
					    {
                                              cn=cn.right(cn.length()-i-2); // strip scope
					      //printf("new name=`%s'\n",cn.data());
					    }
					  }
					  //if (cname.left(current_root->name.length()+2)==current_root->name+"::") 
					  //{ // strip redundant scope
					  //  current->name=current->name.right(current->name.length()-current_root->name.length()-2);
					  //  printf("new name=`%s'\n",current->name.data());
					  //}
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  if (*yytext!=';' || (current_root->section&Entry::SCOPE_MASK) )
					  {
					    int tempArg=current->name.find('<');
					    QString tempName;
					    if (tempArg==-1) 
					      tempName=current->name;
					    else
					      tempName=current->name.left(tempArg);
					    if (current->type.isNull() && tempName.find("operator")==-1 && 
						(tempName.find('*')!=-1 || tempName.find('&')!=-1))
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
					    }
					  }
					  else // a global function prototype or function variable
					  {
					    //printf("Scanner.l: prototype? type=`%s' name=`%s' args=`%s'\n",current->type.data(),current->name.data(),current->args.data());
					    QRegExp re("([^)])");
					    if (!current->type.isNull() && current->type.find(re,0)!=-1)
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
					  current_root->addSubEntry(current);
					  current = new Entry ;
					  current->protection = protection;
				          current->sig = sig;
					  current->virt = virt;
					  current->stat = gstat;
					  current->slot = slot;
					  lastCurlyContext = FindMembers;
					  if( *yytext == '{' )
					      BEGIN( SkipCurly ) ;
					  else if( *yytext == ':' )
					      BEGIN( SkipInits ) ;
					  else
					      BEGIN( FindMembers ) ;
					}

<SkipInits>"{"				{ 
  				          lastCurlyContext = FindMembers;
  					  BEGIN( SkipCurly ) ; 
					}
<SkipCurly,SkipCurlyCpp>"{"		{ ++bracketCount ; }
<SkipCurly,SkipCurlyCpp>"}"		{ if( bracketCount )
					    --bracketCount ;
					  else
					    //BEGIN( FindMembers ) ;
					    BEGIN( lastCurlyContext ) ;
					}
<SkipCurly>\'.\'
<SkipCurly>\"			        { 
  					  lastStringContext=SkipCurly;
  				          BEGIN( SkipString ); 
					}
<SkipCurly>^{B}*"#"			{ BEGIN( SkipCurlyCpp ); }
<SkipCurlyCpp>\n			{ yyLineNr++; 
  					  lastCurlyContext = FindMembers;
  					  BEGIN( SkipCurly ); 
					}
<SkipCurlyCpp>\\[\r]*"\n"[\r]*		{ yyLineNr++; }
<SkipCurlyCpp>"/*"
<SkipCurlyCpp>"*/"
<SkipCurlyCpp>"//"                      
<SkipString>\\.
<SkipString>\"				{ BEGIN( lastStringContext ); }
<SkipString>"/*"
<SkipString>"*/"
<SkipString>"//"                        

<Bases,ClassName>";"			{ 
					  current->section = Entry::EMPTY_SEC ;
					  current->type.resize(0) ;
					  current->name.resize(0) ;
					  current->args.resize(0) ;
					  current->argList->clear();
					  BEGIN( FindMembers ) ;
					}
<ClassName>{ID}				{ 
					  current->name = yytext ;
					  BEGIN( ClassVar );
					}
  /*
<ClassVar>{ID}/{BN}*"{"			{ // we probably got some M$ extension
  					  current->name = yytext ;
  					}
<ClassVar>{ID}/{BN}*":"			{ // we probably got some M$ extension
  					  current->name = yytext ;
  					}
  */
<ClassVar>{ID}				{
					  if (isTypedef)
					  {
					    typedefDict.insert(yytext,new QString(current->name));
					    current->type.prepend("typedef ");
					  }
  					  current->type += ' ' ;
					  current->type += current->name ;
					  current->name = yytext ;
					  //BEGIN( FindMembers );
  					}
<ClassVar>"("				{
					  roundCount=1;
					  lastSkipRoundContext=YY_START;
					  BEGIN(SkipRound);
					}
<ClassVar>":"				{ 
  					  current->type.resize(0);
                                          baseProt=Private;
                                          baseVirt=Normal;
					  baseName.resize(0);
					  BEGIN( Bases ) ;
					}
<ClassVar>[;=*&]			{
    					  unput(*yytext);
					  BEGIN( FindMembers );
    					}
<ClassName,ClassVar>{B}*"{"{B}*		{ current->fileName = yyFileName ;
					  current->startLine = yyLineNr ;
					  if (current->name.length()==0 && !isTypedef) // anonymous compound
					    current->name.sprintf("@%d",anonCount++);
					  BEGIN( Curly ) ;
					}
<Bases>"virtual"                        { baseVirt = Virtual; }
<Bases>"public"                         { baseProt = Public; }
<Bases>"protected"                      { baseProt = Protected; }
<Bases>"private"                        { baseProt = Private; }
<Bases>({ID}{BN}*"::"{BN}*)*{ID}	{ 
  					  //current->extends->append(
					  //  new BaseInfo(yytext,baseProt,baseVirt)
					  //) ;
  					  baseName += yytext;
					  current->args += ' ' ;
					  current->args += yytext ;
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
<Bases>","				{ current->args += ',' ; 
  					  if (baseName.length()>0)
  					    current->extends->append(
					      new BaseInfo(baseName,baseProt,baseVirt)
					    );
					  baseProt=Private;
					  baseVirt=Normal;
					  baseName.resize(0);
					}
<Bases>{B}*"{"{B}*			{ current->fileName = yyFileName ;
					  current->startLine = yyLineNr ;
  					  if (baseName.length()>0)
  					    current->extends->append(
					      new BaseInfo(baseName,baseProt,baseVirt)
					    );
					  BEGIN( Curly ) ;
					}
<Comment>\n				{ current->program += yytext ;
					  yyLineNr++ ;
					}
<Comment>"/*"				{ current->program += yytext ; } 
<Comment>"//"				{ current->program += yytext ; }
<Comment>.				{ current->program += *yytext ; }
<Comment>.*"*/"				{ current->program += yytext ;
					  BEGIN( Curly ) ;
					}

<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator>("//"{B}*)?"/*!" { 
					  removeSlashes=(yytext[1]=='/');
					  tmpDocType=-1;
					  if (YY_START==Curly)
					    current->doc+="\n\n";
					  else
  					    current->doc.resize(0);
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
					    current->inside = current_root->name+"::";
					  BEGIN( Doc );
					}
<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator>("//"{B}*)?"/**"/[^/*] {
  					  removeSlashes=(yytext[1]=='/');
					  if (current_root->section & Entry::SCOPE_MASK)
					    current->inside = current_root->name+"::";
					  if (YY_START==Curly)
					  {
					    tmpDocType=-1;
					    current->doc+="\n\n";
					    lastDocContext = Curly;
					    BEGIN( Doc );
					  }
					  else
					  {  
					    tmpDocType=Doc;
					    current->doc.resize(0);
					    current->brief.resize(0);
					    lastDocContext = YY_START;
					    BEGIN( JavaDoc );
					  }
  					}
<FindMembers,FindFields,MemberSpec,FuncQual,Operator>"//!" { 
  					  current->brief.resize(0);
					  tmpDocType=-1;
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
					    current->inside = current_root->name+"::";
					  BEGIN( LineDoc );
					}
<FindMembers,FindFields,MemberSpec,FuncQual,Operator>"///"/[^/] { 
  					  current->brief.resize(0);
					  tmpDocType=-1;
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
					    current->inside = current_root->name+"::";
					  BEGIN( LineDoc );
					}
<FindMembers>"extern"{BN}+"\"C"("++")?"\""{BN}+("{")?
<FindMembers>"{"			{
  					  current->type.resize(0);
  					  current->name.resize(0);
  					  current->args.resize(0);
					  current->argList->clear();
					  bracketCount=0;
					  BEGIN( SkipBlock );
					  	
  					}
<JavaDoc>"@short"{B}+			{
  					  lastBriefContext=Doc;
  					  BEGIN( ClassDocBrief ); 
					}
<JavaDoc>^(({B}*"*"+)?){BL}		{
					  lineCount();
					  if (!current->brief.stripWhiteSpace().isEmpty())
					  {
					    BEGIN( tmpDocType );
					  }
 					} 
<JavaDoc>"@"				{
  					  unput(*yytext);
					  BEGIN(Doc);
  					}
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
<JavaDoc>"."[ \t\r\n]			{
  					  lineCount();
					  current->brief+=".";
					  BEGIN( tmpDocType );
					}
<JavaDoc>("\\"|"@")"internal"		{
  					  current->doc+="\\internal";
  					  BEGIN( tmpDocType );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")("fn"|"var"|"typedef"){B}*	{
					  current->section = Entry::MEMBERDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( ClassDocFunc ); 
					}
<LineDoc,Doc,JavaDoc>{B}*("\\"|"@")"def"{B}*	{
  					  nextDefContext = YY_START==LineDoc ? DefLineDoc : ClassDoc;
  					  current->section = Entry::DEFINEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( ClassDocDefine );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")"overload"{B}* { 
  					  BEGIN( ClassDocOverload ); 
					}
<ClassDocOverload>{B}*"\n"	        {
					  QString orgDoc = current->doc;
					  current->doc = getOverloadDocs();
					  current->doc += "\n\n";
					  current->doc += orgDoc;
  					  yyLineNr++;
  					  BEGIN( Doc );
  				        }
<ClassDocOverload>.                     { unput(*yytext);
  					  current->section = Entry::OVERLOADDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocFunc ); 
					}
<Doc,JavaDoc>{B}*("\\"|"@")"enum"{B}*	{
  					  current->section = Entry::ENUMDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( EnumDocArg1 );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")"defgroup"{B}* {
  					  current->section = Entry::GROUPDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( GroupDocArg1 );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")"namespace"{B}*	{
  					  current->section = Entry::NAMESPACEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( NameSpaceDocArg1 );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")"class"{B}*  {
  					  current->section = Entry::CLASSDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*("\\"|"@")"union"{B}*  {
  					  current->section = Entry::UNIONDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*("\\"|"@")"struct"{B}* {
  					  current->section = Entry::STRUCTDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*("\\"|"@")"page"{B}*   {
  					  current->section = Entry::PAGEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( PageDocArg1 );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")"file"{B}*	{
  					  current->section = Entry::FILEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( FileDocArg1 );
  					}
<Doc,JavaDoc>{B}*("\\"|"@")"example"{B}* {
 					  current->section = Entry::EXAMPLE_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
 					  BEGIN( ExampleDocArg1 );
					}
<ExampleDocArg1>{FILE}			{
				          current->name = stripQuotes(yytext);	
					  BEGIN( ExampleDoc );
					}
<ClassDoc,Doc,JavaDoc>{B}*("\\"|"@")"relate"[sd]{B}*    { 
  					  lastDocRelContext = YY_START;
  					  BEGIN( ClassDocRelates ); 
					}
<ClassDocRelates>{ID}			{ 
  					  current->relates = yytext;
					  BEGIN( lastDocRelContext );
					}
<NameSpaceDocArg1>{SCOPENAME}		{
  					  current->name = yytext;
					  newDocState();
  					}
<NameSpaceDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\namespace at line %d of %s.\n",yyLineNr,yyFileName);
  					  yyLineNr++;
  					}
<ClassDocArg1>{SCOPENAME}		{
					  current->name = yytext;
					  // prepend outer scope name 
					  if (current_root->section & Entry::SCOPE_MASK)
					  {
					    current->name.prepend(current_root->name+"::");
					  }
					  BEGIN( ClassDocArg2 );
					}
<ClassDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\class at line %d of %s.\n",yyLineNr,yyFileName);
  					  yyLineNr++;
					  
  					}
<GroupDocArg1>{ID}(".html"?)		{
  					  current->name = yytext;
					  if (current->name.right(5)==".html") 
					    current->name=current->name.left(current->name.length()-5);
					  BEGIN(GroupDocArg2);
  					}
<GroupDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\defgroup at line %d of %s.\n",yyLineNr,yyFileName);
					  yyLineNr++;
					  BEGIN( Doc );
  					}
<GroupDocArg2>.*			{
					  current->type = yytext;
					  current->type = current->type.stripWhiteSpace();
					  newDocState();
  					}
<GroupDocArg2>"\n"			{
 					  newDocState();
  					}
<ClassDocArg2>{FILE}			{
  					  //printf("ClassDocArg2=%s\n",yytext);
					  current->includeFile = stripQuotes(yytext);
					  BEGIN( ClassDocArg3 );
					}
<ClassDocArg2>{BL}			{ yyLineNr++; 
					  newDocState();
					}
<ClassDocArg3>{FILE}			{
  					  //printf("ClassDocArg3=%s\n",yytext);
 					  current->includeName = stripQuotes(yytext);
  					  newDocState();
					}
<ClassDocArg3>{BL}			{ yyLineNr++; 
  					  newDocState();
					}
<FileDocArg1>{FILE}			{
  					  current->name = stripQuotes(yytext);
					  newDocState();
  				        }
<FileDocArg1>"\n"			{
  					  //warn("Warning: missing argument after "
					  //     "\\file at line %d of %s.\n",yyLineNr,yyFileName);
  					  current->name = yyFileName;
  					  yyLineNr++;
					  newDocState();
  					}
<PageDocArg1>{FILE}			{
					  current->name = stripQuotes(yytext);
					  BEGIN( PageDocArg2 ); 
					}
<PageDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\page at line %d of %s.\n",yyLineNr,yyFileName);
  					  yyLineNr++;
					  BEGIN( Doc );
  					}
<PageDocArg2>.*"\n"			{
  				          yyLineNr++;
					  current->args = yytext;
					  BEGIN( PageDoc );
					}
<EnumDocArg1>{SCOPEID}			{
  					  current->name = yytext;
					  if (current_root->section & Entry::SCOPE_MASK)
					  {
					    current->name.prepend(current_root->name+"::");
					  }
  					  newDocState();
  					}
<EnumDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\enum at line %d of %s.\n",yyLineNr,yyFileName);
  					  yyLineNr++;
					  BEGIN( Doc );
  					}
<PageDoc>"\\refitem".*"\n"		{
  					  current->doc+=yytext;
  					}
<PageDoc>("\\"|"@")"section"{B}+	{
  					  isSubsection=FALSE;
  					  BEGIN(SectionLabel);
  					}
<PageDoc>("\\"|"@")"subsection"{B}+	{
  					  isSubsection=TRUE;
  					  BEGIN(SectionLabel);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>"\\\\verbatim"/[^a-z_A-Z0-9] {
					  current->doc+="\\\\verbatim";
					}
<Doc,PageDoc,JavaDoc,ClassDoc>("\\"|"@")"verbatim"/[^a-z_A-Z0-9] {
					  lastVerbState=YY_START;
					  current->doc+="\\verbatim";
  					  BEGIN(SkipVerbatim);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>"\\\\code"/[^a-z_A-Z0-9] {
  					  current->doc+="\\\\code";
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>("\\"|"@")"code"/[^a-z_A-Z0-9] {
  					  lastCodeState=YY_START;
					  current->doc+="\\code";
					  BEGIN(SkipCode);
					}
<Doc,PageDoc,JavaDoc,ClassDoc>"<"{PRE}{ATTR}">" {
  					  lastCodeState=YY_START;
					  current->doc+="<PRE>";
					  BEGIN(SkipCode);
  					}
<SkipVerbatim>("\\"|"@")"endverbatim"/[^a-z_A-Z0-9] {
  					  current->doc+=yytext;
  					  BEGIN(lastVerbState);
  					}
<SkipVerbatim>[^ \t\/\\\n]*		{
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
<SkipCode>("\\"|"@")"endcode"		{
  					  current->doc+="\\endcode";
					  BEGIN(lastCodeState);
  					}
<SkipCode>"</"{PRE}{ATTR}">"		{
  					  current->doc+="</PRE>";
					  BEGIN(lastCodeState);
  					}
<SkipCode>^"//"({B}*"*"+)?		{
  					  if (!removeSlashes)
					    current->doc+=yytext;
  					}
<SkipCode>^{B}*"*"+		
<SkipCode>"//"				{
  					  current->doc+=yytext;
  					}
<SkipCode>[^ \*\t\/\\\n]+		{
  					  current->doc+=yytext;
  					}
<SkipCode>\n				{
  					  yyLineNr++;
					  current->doc+=*yytext;
  					}
<SkipCode>.				{
  					  current->doc+=*yytext;
  					}
<SectionLabel>[a-z_A-Z0-9]+		{
  					  sectionLabel=yytext;
					  sectionTitle.resize(0);
					  BEGIN(SectionTitle);
  					}
<SectionTitle>[^\n*]*/"\n"		{
					  sectionTitle+=yytext;
					  sectionTitle=sectionTitle.stripWhiteSpace();
					  addSection();
					  current->doc += "\\section "+sectionLabel+"\n";
					  BEGIN(PageDoc);
  					}
<SectionTitle>"*"			{
  					  sectionTitle+=yytext;
  					}
<ExampleDoc,PageDoc,ClassDoc>"\n"			{ yyLineNr++ ; current->doc+=yytext; }
<ExampleDoc,PageDoc,ClassDoc>[a-z_A-Z0-9 \t]+		{ current->doc += yytext; }
<ExampleDoc,PageDoc,ClassDoc>.    	 	        { current->doc += yytext; }
<Doc,JavaDoc,LineDoc,ExampleDoc,PageDoc,ClassDoc>^{B}*"//" 
<Doc,JavaDoc,LineDoc,ExampleDoc,PageDoc,ClassDoc>"//"  	{ current->doc += yytext; }
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc>"\\\\f"[$\[\]] {
							  current->doc += &yytext[1]; 
							}
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc>"\\f$"	{ 
  							  lastFormulaContext = YY_START;
  							  formulaText="$"; 
							  BEGIN(ReadFormulaShort); 
							}
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc>"\\f["	{ 
  							  lastFormulaContext = YY_START;
  							  formulaText="\\["; 
							  BEGIN(ReadFormulaLong); 
							}
<ReadFormulaShort>"\\f$"				{
  							  formulaText+="$";
							  if (lastFormulaContext==ClassDocBrief)
							    current->brief += addFormula();
							  else
							    current->doc += addFormula();
							  BEGIN(lastFormulaContext);
  							}
<ReadFormulaLong>"\\f]"					{
							  formulaText+="\\]";
							  if (lastFormulaContext==ClassDocBrief)
							    current->brief += addFormula();
							  else
							    current->doc += addFormula();
							  BEGIN(lastFormulaContext);
  							}
<ReadFormulaLong,ReadFormulaShort>.                     { formulaText+=*yytext; }
<ExampleDoc,PageDoc,ClassDocBrief,ClassDoc>"*/"    	{
  					  if (YY_START==ClassDocBrief &&
					      lastBriefContext==Doc)
					  {
  					    current->doc += "\n\n";
  					    BEGIN( lastDocContext ); 
					  }
					  else
					  {
  					    current->doc += "\n\n";
					    //printf("Add docs for class %s\n",current->name.data());
					    current_root->addSubEntry(current);
					    current = new Entry ;
					    current->protection = protection ;
				            current->sig = sig;
					    current->virt = virt;
					    current->stat = gstat;
					    current->slot = slot;
					  }
					  BEGIN( FindMembers );
					}
<PageDoc>"<"{TITLE}">"			{ 
  					  current->args.resize(0); 
					  current->argList->clear();
  					  BEGIN( PageDocTitle); 
					}
<PageDocTitle>\n			{ yyLineNr++; current->args+=" "; }
<PageDocTitle>[^\n\<]			{ current->args+=yytext; }
<PageDocTitle>"</"{TITLE}">"		{ BEGIN( PageDoc ); }
<ClassDoc,Doc,JavaDoc>("\\"|"@")"ingroup"{B}+   { 
  					  lastGroupContext = YY_START;
  					  lineCount();
  					  BEGIN( GroupName ); 
					}
<GroupName>{ID}				{
  					  current->groups->append(
					    new QString(yytext)
					  );
  					}
<GroupName>\n				{
  					  yyLineNr++; BEGIN( lastGroupContext );
  					}
<ClassDoc,Doc>{B}*("\\brief"|"@short")	{ 
  					  lastBriefContext=YY_START;
  					  BEGIN( ClassDocBrief ); 
					}
<ClassDoc>{B}*"\\inherit"{B}+	        { BEGIN( DocBaseClass ); }
<DocBaseClass>{ID}			{
					  //printf("Adding base class %s\n",yytext);
					  current->extends->append(
					    new BaseInfo(yytext,Public,Normal)
					  );
					}
<DocBaseClass>\n			{ yyLineNr++; BEGIN( ClassDoc ); }
<ClassDocBrief>{BS}{BL}			{ 
  					  current->brief=current->brief.stripWhiteSpace();
  					  yyLineNr++; 
					  BEGIN( lastBriefContext );
					}
<ClassDocBrief>"\n"			{ yyLineNr++ ; current->brief += " "; }
<ClassDocBrief>"<"{BR}{ATTR}">"
<ClassDocBrief>{BS}/("\\"|"@")"ingroup"	{
  					  current->brief=current->brief.stripWhiteSpace();
					  BEGIN( lastBriefContext ); 
  					}
<ClassDocBrief>{BS}/("\\"|"@")"author"	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"internal" { BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"version"	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/"\\date"	        { BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"param"	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"exception" { BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"return"	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\sa"|"@see")	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"bug"	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"warning"	        { BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\"|"@")"par"{BN}+	{ BEGIN( lastBriefContext ); }
<ClassDocBrief>{BS}/("\\brief"|"@short"){BN}+ {
  					  lastBriefContext=YY_START;
  					  BEGIN( ClassDocBrief ); 
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
<ClassDocFunc>"\n"			{ 
  					  yyLineNr++; 
					  current->name = current->name.stripWhiteSpace();
					  if (current->section == Entry::MEMBERDOC_SEC && current->args.length()==0)
					    current->section = Entry::VARIABLEDOC_SEC;
					  newDocState();
					}
<ClassDocFunc>"("			{
  					  current->args+=*yytext;
					  currentArgumentContext = ClassDocFuncQual;
					  copyArgString=&current->args;
					  BEGIN( ReadFuncArgType ) ;
  					}
<ClassDocFunc>"("({B}*"*")+		{
  					  current->name+="(*";
  					  BEGIN( ClassDocFuncPtr );
  					}
<ClassDocFuncPtr>{SCOPENAME}		{
  					  current->name+=yytext;
  					}
<ClassDocFuncPtr>")"			{
  					  current->name+=')';
  					  BEGIN( ClassDocFunc );
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
<ClassDocFuncQual>"\n"			{
  					  yyLineNr++; 
					  current->name = current->name.stripWhiteSpace();
					  newDocState();
  					}
<Doc>.					{ current->doc += *yytext; }
<DefLineDoc,LineDoc>.			{ current->brief += *yytext; }
<Doc>\n					{ yyLineNr++; current->doc += *yytext; }
<LineDoc>\n				{ 
					  yyLineNr++;  
					  BEGIN( lastDocContext );
					}
<DefLineDoc>\n				{
  					  yyLineNr++; 
					  unput('/');unput('*');
					  BEGIN( ClassDoc );
  					}

<AfterDocLine>"/*"|"//"			{ current->brief+=yytext; }
<AfterDocLine>\n			{
                                          if (afterDocTerminator!=0)
					    unput(afterDocTerminator);
					  BEGIN(lastAfterDocContext);
  					}
<AfterDocLine>.				{ current->brief+=yytext; }
<AfterDocBrief>"/*"|"//"		{ current->brief+=yytext; }
<AfterDocBrief>^{B}*"*"+/[^/\n]            
<AfterDocBrief>\n			{ current->brief+=yytext; yyLineNr++; }
<AfterDocBrief>.			{ current->brief+=*yytext; }
<AfterDocBrief>^{B}*"*"/[^/\n]{BL}	{ yyLineNr++; 
  					  if (!current->brief.stripWhiteSpace().isEmpty())
					    BEGIN(AfterDoc); 
					}
<AfterDocBrief>"*/"			{
                                          if (afterDocTerminator!=0)
					    unput(afterDocTerminator);
					  BEGIN(lastAfterDocContext);
  					}
<AfterDocBrief>"."/{BN}			{ BEGIN(AfterDoc); }
<AfterDoc>("\\"|"@")"brief"		{ BEGIN(AfterDocBrief); }
<AfterDoc>"/*"|"//"			{ current->doc+=yytext; }
<AfterDoc>^{B}*"*"+/[^/]			
<AfterDoc>\n				{ current->doc+=yytext; yyLineNr++; }
<AfterDoc>.				{ current->doc+=*yytext; }
<AfterDoc>"*/"				{
                                          if (afterDocTerminator!=0)
					    unput(afterDocTerminator);
					  BEGIN(lastAfterDocContext);
  					}
<ClassDocRelates,ClassDocFunc,ClassDocDefine,GroupDocArg1,ClassDocArg1,SectionTitle,EnumDocArg1,FileDocArg1,PageDocArg1,ExampleDocArg1,ClassDefineArgs>"*/" {
					  current->doc += "\n\n";
					  err("Warning: unexpected end of "
				              "documentation block found in "
					      "file %s at line %d\n",yyFileName,yyLineNr);
					  BEGIN( lastDocContext );
					}
<Doc>"*/"				{ 
  					  current->doc += "\n\n";
  					  BEGIN( lastDocContext ); 
					}
<JavaDoc>"*/"				{ 
					  unput('/');unput('*');
					  BEGIN( tmpDocType );
					}
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,ClassDocBrief>^{B}*(("//"{B}*)?)"*"+/[^/]
<DefLineDoc,LineDoc,ClassDoc,Doc>"/*"              { current->doc += yytext; }

<SkipCxxComment>.*\n			{ yyLineNr++ ;
					  BEGIN( lastCContext ) ;
					}
<*>.
<*>\n					{ yyLineNr++ ; }
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

void scanString(const char *s)
{
  const char *oldInputString = inputString;
  int oldInputPosition = inputPosition;
  YY_BUFFER_STATE oldBuffer = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(scanYYin, YY_BUF_SIZE));  
  inputString  = s;
  inputPosition = 0;
  scanYYlex();
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(oldBuffer);
  inputString = oldInputString;
  inputPosition = oldInputPosition;
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

void parseCompounds(Entry *rt)
{
  //printf("parseCompounds(%s)\n",rt->name.data());
  EntryListIterator eli(*rt->sublist);
  Entry *ce;
  for (;(ce=eli.current());++eli)
  {
    if (ce->program.length()>0)
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  ce->name.data(),ce->program.data());
      // init scanner state
      inputString = ce->program;
      inputPosition = 0;
      scanYYrestart( scanYYin ) ;
      if (ce->section==Entry::ENUM_SEC)
	BEGIN( FindFields ) ;
      else
	BEGIN( FindMembers ) ;
      current_root = ce ;
      strcpy( yyFileName, ce->fileName ) ;
      yyLineNr = ce->startLine ;
      //current->reset();
      current = new Entry;
      // set default protection based on the compound type
      if( ce->section==Entry::CLASS_SEC ) // class
        current->protection = protection = Private ;
      else if (ce->section == Entry::ENUM_SEC ) // enum
	current->protection = protection = ce->protection;
      else if (ce->name.length()>0 && ce->name.at(0)=='@') // anonymous union
	current->protection = protection = ce->protection;
      else // named struct or union
	current->protection = protection = Public ;
      sig = FALSE;
      slot = FALSE;
      gstat = FALSE;
      virt = Normal;
      scanYYlex() ;
      delete current;
      ce->program.resize(0);
    }
    parseCompounds(ce);
  }
}

//----------------------------------------------------------------------------

void parseMain(Entry *rt)
{
  initParser();
  anonCount     = 0;
  protection    = Public;
  sig           = FALSE;
  slot          = FALSE;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  global_root   = rt;
  current       = new Entry;
  inputString   = rt->program;
  inputPosition = 0;
  ifCount=0;
  scanYYrestart( scanYYin );
  BEGIN( FindMembers );
  scanYYlex();
  rt->program.resize(0);
  delete current;
  parseCompounds(rt);
}

//----------------------------------------------------------------------------

void parseDocument(OutputList &ol,const QString &docString)
{
  //inParamBlock=inSeeBlock=inReturnBlock=FALSE;
  curTable = 0;
  outDoc = new OutputList(&ol);
  currentIncludeFile.resize(0);
  includeFileOffset=0;
  includeFileLength=0;
  if (!docString) return;
  linkRef  = "";
  linkText = "";
  inputString = docString;
  inputPosition = 0;
  scanYYrestart( scanYYin ); 
  BEGIN( DocScan );
  insideArgumentList = FALSE;
  scanYYlex();
  if (insideArgumentList) { insideArgumentList=FALSE; outDoc->endItemList(); }
  if (inBlock()) endBlock();
  ol+=*outDoc;
  delete outDoc;
  return;
}

//----------------------------------------------------------------------------

void parseDoc(OutputList &ol,const char *clName,
              const char *memName,const QString &docString)
{
  initParser();
  initParseCodeContext();
  exampleDoc=FALSE;           // do not cross reference with member docs
  className=clName;
  memberName=memName;
  if (memName)
  {
    refName=className+"::"+memberName;
  }
  else
  {
    refName=className;
  }
  parseDocument(ol,docString);
}

//----------------------------------------------------------------------------

void parseExample(OutputList &ol,const QString &docString, 
                  const char *fileName)
{
  initParser();
  initParseCodeContext();
  exampleDoc=TRUE;            // cross reference with member docs
  exampleName=fileName;
  parseDocument(ol,docString); 
}

//----------------------------------------------------------------------------

extern "C" { // some sillyness to keep the compiler happy
int	scanYYwrap()	{ return 1 ; }
void    bogus() { yy_flex_realloc(0,0); }
}

