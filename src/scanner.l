/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qarray.h>
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
#include "outputlist.h"

#ifndef WIN32
#include <unistd.h>
#endif
  
#define YY_NEVER_INTERACTIVE 1
  

MemberGroupDict memberGroupDict(1009); // dictionary of the member groups heading
  
/* -----------------------------------------------------------------
 *
 *	statics
 */
static bool             insideArgumentList;
static QCString         className;
static QCString         memberName;
static QCString         refName;
static OutputList *     outDoc;
static QCString         code;
static QCString         linkRef;
static QCString         linkText;
static QCString         codeBlock;
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
static int              lastMemberGroupContext;
static int              lastFormulaContext;
static int              lastAnchorContext;
static int              lastInitializerContext;
static int              lastClassTemplSpecContext;
static int              nextDefContext;
static int              overloadContext;
static Protection	protection;
static Protection	baseProt;
static int		sharpCount   = 0 ;
static int		roundCount   = 0 ;
static int		curlyCount   = 0 ;
static int		squareCount  = 0 ;
static int              ifCount      = 0 ;
static Entry*		current_root = 0 ;
static Entry*		global_root  = 0 ;
static Entry*		current      = 0 ;
static Entry*		previous     = 0 ;
static Entry*		tempEntry    = 0 ;
static int		yyLineNr     = 0 ;
static int              anonCount    = 0 ;        
static char		yyFileName[2048] ;
static int              lastMemberGroupLine;
static bool    		sig;
static bool    		slot;
static bool    		gstat;
static bool             removeSlashes;
static Specifier 	virt;
static Specifier 	baseVirt;
static bool             exampleDoc;
static QCString         exampleName;
static QCString         htmlUrl,htmlText;
static QCString         currentIncludeFile;
static QCString         msType,msName,msArgs;
static int              memberGroupId = -1;
static int              includeFileOffset = 0;
static int              includeFileLength = 0;
static bool             firstLine;
static bool             isTypedef;
static bool             inParamBlock;
static bool             inRetValBlock;
static bool             inExceptionBlock;
static bool             inSeeBlock;
static bool             inReturnBlock;
static bool             inAuthorBlock;
static bool             inDeprecatedBlock;
static bool             inVersionBlock;
static bool             inDateBlock;
static bool             inBugBlock;
static bool             inNoteBlock;
static bool             inPreBlock;
static bool             inPostBlock;
static bool             inInvarBlock;
static bool             inWarningBlock;
static bool             inParBlock;
static bool             firstSeeArg;
static char             afterDocTerminator;
static int              tmpDocType;
static QCString         sectionLabel;
static QCString		sectionTitle;
static SectionInfo::SectionType      
                        sectionType;
static QCString         funcPtrType;
static QCString         templateStr;
static QCString         baseName;
static QCString         *specName;
static QCString         formulaText;
static QCString         sectionRef;
static bool             insideIDL = FALSE;
static bool             insideCppQuote = FALSE;

// state variable for reading the argument list of a function
static int              argRoundCount;
static int              argSharpCount;
static int              currentArgumentContext;
static int              lastCopyArgStringContext;
static int              lastCopyArgContext;
static int              currentListIndentLevel;
static QCString         *copyArgString;
static QCString         fullArgString;

static ArgumentList     *currentArgumentList;
static QCString         *currentTemplateSpec;
static QCString         curImageName;
static char             lastCopyArgChar;
static QCString         *pCopyRoundString;
static QCString         *pCopyCurlyString;
static QCString         *pCopyQuotedString;

//-----------------------------------------------------------------------------

static void initParser()
{
  insideArgumentList=FALSE;
  className.resize(0);
  memberName.resize(0);
  refName="<unknown>";
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
  sharpCount = 0;
  roundCount = 0;
  curlyCount = 0;
  ifCount = 0;
  memberGroupId = -1;
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
  inRetValBlock = FALSE;
  inExceptionBlock = FALSE;
  inSeeBlock = FALSE;
  inReturnBlock = FALSE;
  inAuthorBlock = FALSE;
  inDeprecatedBlock = FALSE;
  inVersionBlock = FALSE;
  inDateBlock = FALSE;
  inBugBlock = FALSE;
  inNoteBlock = FALSE;
  inPreBlock = FALSE;
  inPostBlock = FALSE;
  inInvarBlock = FALSE;
  inWarningBlock = FALSE;
  inParBlock = FALSE;
  firstSeeArg = FALSE;
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
  delete ol; ol=0;
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
  delete elemList; elemList=0;
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

static QCString stripKnownExtensions(const char *text)
{
  QCString result=text;
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
    QCString s;
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
    QCString s;
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
  QCString s;
  char c;
  bool found=FALSE;
  while (!found)
  {
    while ( includeFileOffset<includeFileLength && 
	(c=currentIncludeFile[includeFileOffset++])!='\n' && c!=0
	  ) s+=c;
    if (!s.stripWhiteSpace().isEmpty() || 
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
    QCString s;
    char c;
    while ( includeFileOffset<includeFileLength && 
	    (c=currentIncludeFile[includeFileOffset++])!='\n' && c!=0
	  ) s+=c;
    if (!s.stripWhiteSpace().isEmpty())
    {
      ol.writeString("    ");
      parseCode(ol,className,s,exampleDoc,exampleName);
      ol.writeString("\n");
      if (s.find(key)!=-1) found=TRUE; 
    }
    if (includeFileOffset==includeFileLength) found=TRUE;
  }
}

static int newMemberGroupId()
{
  static int curGroupId=0;
  return curGroupId++;
}

static void newDocState();


//-----------------------------------------------------------------

static QStack<int> listIndentStack;
static bool insideItemList = FALSE;

static void addListItemMarker(const char *marker)
{
  // find the actual position at which the bullet was found
  int indent=0;
  const char *p=marker;
  char c;
  while ((c=*p++))
  {
    if (c=='\t')
    {
      indent+=Config::tabSize - (indent%Config::tabSize);
    }
    else
    {
      indent++;
    }
  }
  //printf("list marker found at column %d\n",indent);
  if (!insideItemList)
  {
    outDoc->startItemList();
    outDoc->writeListItem();
    listIndentStack.push(new int(indent));
    insideItemList=TRUE;
  } 
  else
  {
    int *pPrevIndent = listIndentStack.top(); 
    if (*pPrevIndent==indent) // new item at the same indent level
    {
      outDoc->writeListItem(); 
    }
    else if (*pPrevIndent<indent) // start sub item list
    {
      outDoc->startItemList();
      outDoc->writeListItem();
      listIndentStack.push(new int(indent));
    }
    else // end sub item list
    {
      listIndentStack.pop();
      delete pPrevIndent;
      outDoc->endItemList();
      // safe guard against wrong indenting
      if (listIndentStack.isEmpty()) 
      {
	insideItemList=FALSE;
	warn("Warning: list item with invalid indent found!\n");
      }
      else
      {
	outDoc->writeListItem();
      }
    }
  }
}

static void forceEndItemList()
{
  int *indent;
  while ((indent=listIndentStack.pop())!=0)
  {
    outDoc->endItemList();
    delete indent;
  }
  insideItemList=FALSE;
}

#if 0
static void tryEndItemList()
{
  if (listIndentStack.count()==1) // no subitems => end list
  {
    outDoc->endItemList();
    delete listIndentStack.pop();
    insideItemList=FALSE;
  } 
}
#endif

//-----------------------------------------------------------------

static bool inBlock()
{
  return inParamBlock || inRetValBlock || inSeeBlock || inReturnBlock || inAuthorBlock ||
         inVersionBlock || inDateBlock || inWarningBlock || inBugBlock || inNoteBlock ||
	 inParBlock || inExceptionBlock || inDeprecatedBlock || inPreBlock ||
	 inPostBlock || inInvarBlock;
}

static void endBlock()
{
    if (inParamBlock || inRetValBlock || inExceptionBlock) 
    {
      outDoc->endDescTableData();
      outDoc->endDescTable();
    }
    outDoc->endDescList();
    inParamBlock=inRetValBlock=inSeeBlock=inReturnBlock=inAuthorBlock=
    inVersionBlock=inDateBlock=inBugBlock=inNoteBlock=inWarningBlock=
    inParBlock=inExceptionBlock=inDeprecatedBlock=inPreBlock=inPostBlock=
    inInvarBlock=FALSE;
}

static void addSection()
{
  //printf("New section pageName=%s label=%s title=%s\n",
  //    current->name.data(),sectionLabel.data(),sectionTitle.data());
  if (sectionLabel.isEmpty()) return;
  if (sectionDict[sectionLabel]==0)
  {
    SectionInfo *si=new SectionInfo(sectionLabel,sectionTitle,sectionType);
    //printf("Adding section addr=%p label=`%s' sectionTitle=`%s' fileName=%s\n",si,sectionLabel.data(),sectionTitle.data(),si->fileName.data());
    sectionDict.insert(sectionLabel,si);
    current->anchors->append(new QCString(sectionLabel));
  }
  else
  {
    warn("Warning: Duplicate label %s found!\n",sectionLabel.data());
  }
}

// Adds a formula text to the list/dictionary of formulas if it was
// not already added. Returns the label of the formula.
static QCString addFormula()
{
  QCString formLabel;
  QCString fText=formulaText.simplifyWhiteSpace();
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

static bool nameIsOperator(QCString &name)
{
  return name.right(8)=="operator" &&
         (name.length()==8 || !isId(name.at(name.length()-9)));
}

static void checkDocs()
{
  if ((current->brief.length()>2 && 
       current->brief.at(0)=='<' && current->brief.at(1)==' ') ||
      (current->doc.length()>2 && 
       current->doc.at(0)=='<' && current->doc.at(1)==' ')
     )
  {
    warn("Warning: Found lonely '<' symbol at the start of the documentation "
	 "at line %d of %s\n",yyLineNr,yyFileName);
	 
  }
}

//static bool curLatexState;
//static bool curManState;
//static bool curHtmlState;
//
//static void storeOutputListState()
//{
//  curLatexState = outDoc->isEnabled(OutputGenerator::Latex);
//  curHtmlState  = outDoc->isEnabled(OutputGenerator::Html);
//  curManState   = outDoc->isEnabled(OutputGenerator::Man);
//}
//
//static void restoreOutputListState()
//{
//  if (curLatexState) 
//    outDoc->enable(OutputGenerator::Latex);
//  else
//    outDoc->disable(OutputGenerator::Latex);
//  if (curHtmlState) 
//    outDoc->enable(OutputGenerator::Html);
//  else
//    outDoc->disable(OutputGenerator::Html);
//  if (curManState)   
//    outDoc->enable(OutputGenerator::Man);
//  else
//    outDoc->disable(OutputGenerator::Man);
//}

enum ImageTypes
{
  IT_Html,
  IT_Latex
};

// search for an image in the imageNameDict and if found
// copies the image to the output directory (which is the
// html directory if type==0 or the latex directory if type==1)
static QCString findAndCopyImage(const char *fileName,ImageTypes type)
{
  QCString result;
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(&imageNameDict,fileName,ambig)))
  {
    QFile inImage(fd->absFilePath());
    if (inImage.open(IO_ReadOnly))
    {
      result = fileName;
      int i;
      if ((i=result.findRev('/'))!=-1 || (i=result.findRev('\\'))!=-1)
      {
	result.right(result.length()-i-1);
      }
      QCString outputDir;
      switch(type)
      {
	case IT_Html: 
	  outputDir = Config::htmlOutputDir;
	  break;
	case IT_Latex: 
	  outputDir = Config::latexOutputDir;
	  break;
      }
      QCString outputFile = outputDir+"/"+result;
      QFile outImage(outputFile);
      if (outImage.open(IO_WriteOnly)) // copy the image
      {
	char *buffer = new char[inImage.size()];
	inImage.readBlock(buffer,inImage.size());
	outImage.writeBlock(buffer,inImage.size());
	outImage.flush();
	delete buffer;
      }
      else
      {
	warn("Warning: could not write output image %s\n",outputFile.data());
      }
    }
    else
    {
      warn("Warning: could not open image %s\n",fileName);
    }
  }
  else if (ambig)
  {
    warn("Warning: image file name %s is ambigious.\n",fileName);
    warn("Possible candidates:\n");
    //includeFileList.writeMatches(fileName);
    showFileDefMatches(&imageNameDict,fileName);
  }
  else
  {
    result=fileName;
    if (result.left(5)!="http:")
    {
      warn("Warning: image file %s is not found in IMAGE_PATH: "  
	   "assuming external image. ",fileName);
    }
  }
  return result;
}

/* ----------------------------------------------------------------- */

//static void addToBody(const char *text);
//static void addToBodyCond(const char *text);
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

//ATTR      ((({BN}+[^\>]+)/">")?)
%}

CMD	  ("\\"|"@")
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
DOCPARAM  ([a-z_A-Z0-9:\.\-]+)|("\"".*"\"")

%option noyywrap

%x	Define
%x      DefineArg
%x	DefineEnd
%x	Include
%x	ClassName
%x	ClassVar
%x	ClassTemplSpec
%x	Bases
%x	BasesProt
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
%x	UsingDirective
%x	NameSpaceDocArg1
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
%x      ClassDocFuncSkipLine
%x      ClassDocFuncExc
%x	ClassDocDefine
%x      ClassDocRelates
%x      ClassDocBrief
%x      ClassDocOverload
%x	ClassDefineArgs
%x	CppQuote
%x	EndCppQuote
%x	GroupDocArg1
%x	GroupDocArg2
%x	GroupName
%x	GroupHeader
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
%x	Text
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
%x	DocRefArg
%x	DocRefArgStart
%x      DocRefItem
%x      DocRefItemName
%x	DocImage
%x	DocHtmlImageName
%x	DocLatexImageName
%x	DocLatexImageWidth
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
%x      DocSkipHtmlComment
%x      ReadFormulaShort
%x	ReadFormulaLong
%x	AnchorLabel
%x	ReadInitializer
%x	CopyString
%x	CopyRound
%x	CopyCurly
%x	IDLUnionCase

%%

<*>\x06[^\x06]*\x06			{ // new file
  					  if (memberGroupId!=-1)
					  {
					    warn("Warning: Missing \\endmgroup in file %s\n",yyFileName);
					    memberGroupId=-1;
					  }
  					  yyLineNr= 0 ; // there is always an extra newline at the start of the file
					  int i; 
					  for( i = 0 ; yytext[i+1] != 6 ; i++ )
					    yyFileName[i] = yytext[i+1] ;
					  yyFileName[i] = 0 ;
					  insideIDL = i>4 && strcmp(&yyFileName[i-4],".idl")==0;
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
<DocScan>^{B}*(("//"{B}*)?)"*"*{B}*"-"{B}+ { /* found list item marker */
  					  addListItemMarker(yytext);
  					}
<DocScan>"<!--"				{ BEGIN(DocSkipHtmlComment); }
<DocSkipHtmlComment>"--"[!]?">"		{ BEGIN(DocScan); }
<DocSkipHtmlComment>.
<DocScan,Text>"&copy;"			{ outDoc->writeCopyright(); }
<DocScan,Text>"&quot;"			{ outDoc->writeQuote(); }
<DocScan,Text>"&"[AEIOUYaeiouy]"uml;"	{ outDoc->writeUmlaut(yytext[1]); }
<DocScan,Text>"&"[AEIOUYaeiouy]"acute;"	{ outDoc->writeAcute(yytext[1]);  }
<DocScan,Text>"&"[AEIOUaeiou]"grave;"	{ outDoc->writeGrave(yytext[1]);  }
<DocScan,Text>"&"[AEIOUaeiou]"circ;"	{ outDoc->writeCirc(yytext[1]);   }
<DocScan,Text>"&"[ANOano]"tilde;"	{ outDoc->writeTilde(yytext[1]);  }
<DocScan,Text>"&szlig;"			{ outDoc->writeSharpS();          }
<DocScan,Text>"&[aA]ring;"		{ outDoc->writeRing(yytext[1]);   }
<DocScan,DocHtmlScan,DocLatexScan>"$("[a-z_A-Z]+")"	{ 
  					  QCString envvar=&yytext[2];
  					  envvar=envvar.left(envvar.length()-1);
					  outDoc->docify(getenv(envvar));
					}
<DocScan>{CMD}"htmlonly"/[^a-z_A-Z0-9]	{
  				          outDoc->pushGeneratorState(); /*storeOutputListState();*/
  					  outDoc->disableAllBut(OutputGenerator::Html); 
					  BEGIN(DocHtmlScan);
  					}
<DocHtmlScan>{CMD}"endhtmlonly"/[^a-z_A-Z0-9] {
  					  /*restoreOutputListState();*/
  					  outDoc->popGeneratorState();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"latexonly"/[^a-z_A-Z0-9]	{
  				          /*storeOutputListState();*/
  					  outDoc->pushGeneratorState();
  					  outDoc->disableAllBut(OutputGenerator::Latex);
					  BEGIN(DocLatexScan);
  					}
<DocLatexScan>{CMD}"endlatexonly"/[^a-z_A-Z0-9] {
  					  /*restoreOutputListState();*/
  					  outDoc->popGeneratorState();
					  BEGIN(DocScan);
  					}
<DocHtmlScan,DocLatexScan>"//"|"/*"|"*/"	{
  					  outDoc->writeString(yytext);
  					}
<DocHtmlScan,DocLatexScan>.		{
  					  char c[2];
					  c[0]=*yytext;c[1]='\0';
  					  outDoc->writeString(c);
  					}
<DocScan>"\\postheader"/{BN}
<DocScan>"\\functionindex"/{BN}		{ writeMemberList(*outDoc); }
<DocScan>"\\classhierarchy"/{BN}	{ writeClassHierarchy(*outDoc); }
<DocScan>"\\annotatedclasslist"/{BN}	{ writeAnnotatedClassList(*outDoc); }
<DocScan>"\\headerfilelist"/{BN}	{ /*TODO: fix this writeHeaderFileList(*outDoc); */ }
<DocScan>"\\header"/{BN}                { BEGIN( DocSkipWord ); }
<DocScan>"\\define"/{BN}	        { BEGIN( DocSkipWord ); }
<DocScan>{CMD}"verbinclude"/{BN}	{ BEGIN( DocVerbInc ); }
<DocVerbInc>{FILE}			{
  					  verbIncludeFile(*outDoc,stripQuotes(yytext));
					  BEGIN( DocScan ); 
  					}
<DocScan>{CMD}"verbatim"/[^a-z_A-Z0-9]	{
  					  outDoc->startCodeFragment();
					  BEGIN(DocVerbatim);
  					}
<DocVerbatim>{CMD}"endverbatim"/[^a-z_A-Z0-9]	{
  					  outDoc->endCodeFragment();
  					  BEGIN(DocScan);
  					}
<DocVerbatim>[^\n\\\@]*"\n"		{
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
<DocScan>{CMD}"internal"/{BN}	        { 
					  if (!Config::internalDocsFlag)
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
<DocScan>{CMD}"link"/{BN}	        { BEGIN( DocLink ); }
<DocSkipWord>[a-z_A-Z0-9.:()]+		{ BEGIN( DocScan ); }
<DocLink>[a-z_A-Z0-9:#.~/()\-\+]+       { 
  					  linkRef  = stripKnownExtensions(yytext);
					  linkText = "";
  					  BEGIN( DocLinkText ); 
					}
<DocLinkText>.				{ linkText += *yytext; }
<DocLinkText>"\n"			{ linkText += " "; }
<DocLink,DocLinkText>{CMD}"endlink" { // <- needed for things like \endlink. 
  					  //printf("GenerateLink className=`%s' linkRef=`%s' linkText=`%s'\n",
					  //        className.data(),linkRef.data(),linkText.data());
					  generateLink(*outDoc,className,linkRef,inSeeBlock,linkText.stripWhiteSpace());
  					  BEGIN( DocScan ); 
					}
  /*
<DocScan>"@ref"{B}+			{
  					  BEGIN(DocRef);
  					}
<DocRef>{SCOPENAME}			{
  					  generateLink(*outDoc,className,yytext,TRUE,0);
					  BEGIN( DocScan );
  					}
  */
<DocScan>{CMD}"endlink"/[^a-z_A-Z0-9]      { warn("Warning: \\endlink without \\link "
    					         "in documentation of %s\n", refName.data()); 
					}
<DocScan>{CMD}"addindex"/{BN}		{
  					  BEGIN(DocIndexWord);
  					}
<DocScan>"\\form#"[0-9]+		{
  					  Formula *formula=formulaNameDict[yytext];
					  if (formula)
					  {
					    QCString formName;
					    formName.sprintf("form-%d.gif",formula->getId());
					    outDoc->writeFormula(formName,formula->getFormulaText());
					  }
  					}
<DocIndexWord>[^\n]+			{
  					  //printf("Adding %s to index\n",yytext);
					  outDoc->addToIndex(yytext,0);
  					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"arg"/{BN}	        { 
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
<DocScan>{CMD}"par"{B}+			{
  					  BEGIN(DocPar);
  					}
<DocPar>[^\n]*{BN}			{
  				          endArgumentList();
					  if (inBlock()) endBlock();  
					  inParBlock=TRUE;
  					  outDoc->startDescList();
					  outDoc->startBold();
					  outDoc->docify(((QCString)yytext).stripWhiteSpace());
					  outDoc->endBold();
					  outDoc->endDescTitle();
					  outDoc->writeDescItem();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"warning"/{BN}	{ 
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
<DocScan>{CMD}"bug"[s]?/{BN}		{
  				          endArgumentList();
  					  if (!inBugBlock)
					  {
					    if (inBlock()) endBlock();
					    inBugBlock=TRUE;
  					    outDoc->startDescList();
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
<DocScan>{CMD}"note"[s]?/{BN}		{
  				          endArgumentList();
  					  if (!inNoteBlock)
					  {
					    if (inBlock()) endBlock();
					    inNoteBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trNote()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"pre"/{BN}		{
  				          endArgumentList();
  					  if (!inPreBlock)
					  {
					    if (inBlock()) endBlock();
					    inPreBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trPrecondition()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"post"/{BN}		{
  				          endArgumentList();
  					  if (!inPostBlock)
					  {
					    if (inBlock()) endBlock();
					    inPostBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trPostcondition()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"invariant"/{BN}		{
  				          endArgumentList();
  					  if (!inInvarBlock)
					  {
					    if (inBlock()) endBlock();
					    inInvarBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trInvariant()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"version"/{BN}		{
  				          endArgumentList();
  					  if (!inVersionBlock)
					  {
					    if (inBlock()) endBlock();
					    inVersionBlock=TRUE;
  					    outDoc->startDescList();
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
<DocScan>{CMD}"date"/{BN}		{
  				          endArgumentList();
  					  if (!inDateBlock)
					  {
					    if (inBlock()) endBlock();
					    inDateBlock=TRUE;
  					    outDoc->startDescList();
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
<DocScan>{CMD}"deprecated"/{BN}		{
  					  endArgumentList();
					  if (!inDeprecatedBlock)
					  {
					    if (inBlock()) endBlock();
					    inDeprecatedBlock=TRUE;
					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trDeprecated()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"author"/{BN}		{
  				          endArgumentList();
  					  if (!inAuthorBlock)
					  {
					    if (inBlock()) endBlock();
					    inAuthorBlock=TRUE;
  					    outDoc->startDescList();
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
<DocScan>{CMD}("return"([s])?|"result")/{BN}	{
  				          endArgumentList();
  					  if (!inReturnBlock)
					  {
					    if (inBlock()) endBlock();
					    inReturnBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trReturns()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}("sa"|"see")/{BN}       { 
  				          endArgumentList();
  					  if (!inSeeBlock)
					  {
					    if (inBlock()) endBlock();
					    inSeeBlock=TRUE;
					    outDoc->startDescList();
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
<DocScan>{CMD}"param"/{BN}		{
  				          endArgumentList();
  					  if (!inParamBlock)
					  {
					    if (inBlock()) endBlock();
					    inParamBlock=TRUE;
					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trParameters()+": ");
					    outDoc->endBold();
  					    outDoc->endDescTitle();
  					    outDoc->writeDescItem();
					    outDoc->startDescTable();
					  }
					  else
					  {
					    outDoc->endDescTableData();
					  }
  				          BEGIN(DocParam); 
  					}
<DocScan>{CMD}"retval"/{BN}		{
  				          endArgumentList();
  					  if (!inRetValBlock)
					  {
					    if (inBlock()) endBlock();
					    inRetValBlock=TRUE;
					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trReturnValues()+": ");
					    outDoc->endBold();
  					    outDoc->endDescTitle();
  					    outDoc->writeDescItem();
					    outDoc->startDescTable();
					  }
					  else
					  {
					    outDoc->endDescTableData();
					  }
  				          BEGIN(DocParam); 
  					}
<DocScan>{CMD}("exception"|"throw")s?/{BN}	{
  					  endArgumentList();
					  if (!inExceptionBlock)
					  {
					    if (inBlock()) endBlock();
					    inExceptionBlock=TRUE;
					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trExceptions()+": ");
					    outDoc->endBold();
  					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					    outDoc->startDescTable();
					  }
					  else
					  {
					    outDoc->endDescTableData();
					  }
					  BEGIN(DocException);
  					}
<DocScan>"\\capt".*
<DocParam>({DOCPARAM}{BN}*","{BN}*)*{DOCPARAM} {
					  outDoc->startDescTableTitle();
					  outDoc->startEmphasis();
  					  outDoc->docify(yytext); 
					  outDoc->endEmphasis();
					  outDoc->endDescTableTitle();
					  outDoc->startDescTableData();
					  BEGIN(DocScan);
  					}
<DocException>{SCOPENAME}		{
					  outDoc->startDescTableTitle();
					  outDoc->startEmphasis();
  					  outDoc->docify(yytext); 
					  outDoc->endEmphasis();
					  outDoc->endDescTableTitle();
					  outDoc->startDescTableData();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"section "{ID}"\n"		{
  					  QCString secName=&yytext[9];               // skip "\section "
					  secName=secName.left(secName.length()-1); // remove \n
					  //printf("SectionName %s found\n",secName.data());
					  SectionInfo *sec;
					  if ((sec=sectionDict[secName]))
					  {
					    //printf("Title %s\n",sec->title.data());
					    outDoc->writeSection(sec->label,sec->title,
						                 sec->type==SectionInfo::Subsection);
					  }
  					}
<DocScan>{CMD}"anchor "{ID}"\n"		{
  					  QCString secName=&yytext[8];
					  secName=secName.left(secName.length()-1);
					  SectionInfo *sec;
					  if ((sec=sectionDict[secName]))
					  {
					    outDoc->writeAnchor(sec->label);
					  }
  					}
<DocScan>{CMD}"ref"		{
  					  BEGIN(DocRefName);
  					}
<DocScan>{CMD}"refitem"			{
  					  BEGIN(DocRefItem);
  					}
<DocRefName>{SCOPENAME}			{
  					  QCString ref=yytext;
					  SectionInfo *sec;
					  if ((sec=sectionDict[ref]))
					  {
					    QCString text;
					    if (sec->title.isEmpty()) 
					      text=sec->label;
					    else
					      text=sec->title;
					    if (sec->type==SectionInfo::Anchor)
					    {
					      outDoc->writeSectionRefAnchor(sec->fileName,sec->label,text);
					    }
					    else
					    {
					      //printf("  ref sec=%p sec->fileName=%s\n",sec,sec->fileName.data());
					      outDoc->writeSectionRef(sec->fileName,sec->label,text);
					    }
					  }
					  else if (!generateLink(*outDoc,className,yytext,TRUE,0))
					  {
					    warn("Warning: reference to unknown section %s!\n",yytext);
					    outDoc->writeBoldString(" unknown reference!");
					  }
					  BEGIN(DocScan);
  					}
<DocRefName>{SCOPENAME}/{B}+"\""	{
  					  sectionRef=yytext;
					  BEGIN(DocRefArgStart);
  					}
<DocRefArgStart>"\""			{
  					  BEGIN(DocRefArg);
  					}
<DocRefArg>[^\"\n]+[\n\"]		{
  					  yytext[yyleng-1]='\0';
					  QCString text=substitute(yytext,"\\\\","\\");
					  SectionInfo *sec;
					  if ((sec=sectionDict[sectionRef]))
					  {
					    if (sec->type==SectionInfo::Anchor)
					    {
					      outDoc->writeSectionRefAnchor(sec->fileName,sec->label,text);
					    }
					    else
					    {
					      outDoc->writeSectionRef(sec->fileName,sec->label,text);
					    }
					  }
					  else if (!generateLink(*outDoc,className,sectionRef,TRUE,text))
					  {
					    warn("Warning: reference to unknown section %s!\n",sectionRef.data());
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
					  QCString text=yytext;
					  if ((sec=sectionDict[sectionRef]))
					  {
					    outDoc->writeSectionRefItem(sec->fileName,sec->label,text.stripWhiteSpace());
					  }
					  else
					  {
					    warn("Warning: reference to unknown section %s!\n",sectionRef.data());
					    outDoc->writeBoldString("unknown reference!");
					  }
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"image"{B}+		{
  					  BEGIN(DocImage); 
  					}
<DocImage>[hH][tT][mM][lL]		{
  					  BEGIN(DocHtmlImageName);
  					}
<DocImage>[lL][aA][tT][eE][xX]		{
  					  BEGIN(DocLatexImageName);
  					}
<DocHtmlImageName>{FILE}|{URLMASK}      {
  					  curImageName = findAndCopyImage(stripQuotes(yytext),IT_Html);
					  if (!curImageName.isEmpty())
					  {
					    /*storeOutputListState();*/
					    outDoc->pushGeneratorState();
  				            outDoc->disableAllBut(OutputGenerator::Html);
					    outDoc->writeImage(curImageName,0,0);
					    /*restoreOutputListState();*/
					    outDoc->popGeneratorState();
					  }
					  BEGIN(DocScan);
  					}
<DocLatexImageName>{FILE}		{
  					  curImageName = findAndCopyImage(stripQuotes(yytext),IT_Latex);
					  if (curImageName.isEmpty())
					    BEGIN(DocScan);
					  else
					    BEGIN(DocLatexImageWidth);
  					}
<DocLatexImageWidth>\n			{ // no width specified
					  /*storeOutputListState();*/
  					  outDoc->pushGeneratorState();
  				          outDoc->disableAllBut(OutputGenerator::Latex);
					  outDoc->writeImage(curImageName,0,0);
					  /*restoreOutputListState();*/
					  outDoc->popGeneratorState();
					  BEGIN(DocScan);
  					}
<DocLatexImageWidth>"width"{B}*"="{B}*[0-9\.]+({B}*{ID})?	{
					  /*storeOutputListState();*/
  					  outDoc->pushGeneratorState();
  				          outDoc->disableAllBut(OutputGenerator::Latex);
					  outDoc->writeImage(curImageName,yytext,0);
					  /*restoreOutputListState();*/
					  outDoc->popGeneratorState();
					  BEGIN(DocScan);
  					}
<DocLatexImageWidth>"height"{B}*"="{B}*[0-9\.]+({B}*{ID})?	{
					  /*storeOutputListState();*/
  					  outDoc->pushGeneratorState();
  				          outDoc->disableAllBut(OutputGenerator::Latex);
					  outDoc->writeImage(curImageName,0,yytext);
					  /*restoreOutputListState();*/
					  outDoc->popGeneratorState();
					  BEGIN(DocScan);
  					}
<DocImage>[a-z_A-Z0-9\.\-]+		{
					  warn("Warning: %s is an unsupported output format for \\image\n",yytext);
  					}
<DocImage,DocHtmlImageName,DocLatexImageName>\n {
  					  warn("Warning: invalid \\image command found!\n");
					  outDoc->enableAll();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"code"({BN}*"\n"|{B}*) 	{ 
					  outDoc->startCodeFragment();
  					  codeBlock.resize(0);
					  BEGIN( DocCodeBlock ); 
					}
<DocScan>{CMD}"endcode"/[^a-z_A-Z0-9] 	{ 
  					  warn("Warning: \\endcode without <PRE> or \\code "
    					       "in the documentation of %s\n",refName.data()); 
					}
<DocScan>{SCOPEMASK}"("[a-z_A-Z,:\<\> \t\*\&]+")"	{
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
  					}
<DocScan>{SCOPEMASK}(("()")?)          	{ 
					  generateRef(*outDoc,className,yytext,inSeeBlock);
					} 
<DocScan>({SCOPEMASK}"::")?"operator()("[a-z_A-Z,\<\> \t\*\&]*")"	{ 
				          QCString oName=yytext;
					  generateRef(*outDoc,className,
					              removeRedundantWhiteSpace(oName),inSeeBlock);
					}
<DocScan>({SCOPEMASK}"::")?"operator"[^(\r\n.,]*"("[a-z_A-Z,\<\> \t\*\&]*")"	{ 
    					  QCString oName=yytext;
					  generateRef(*outDoc,className,
					              removeRedundantWhiteSpace(oName),inSeeBlock);
					}
<DocScan>("http:"|"ftp:"|"file:"){URLMASK}	{ outDoc->writeHtmlLink(yytext,yytext); }
<DocScan>[a-zA-Z_0-9\.\-]+"@"[0-9a-z_A-Z\.\-]+	{ outDoc->writeMailLink(yytext); }
<DocScan>{FILEMASK}			{
					  generateFileRef(*outDoc,yytext);
					}
<DocCodeBlock>{BN}*{CMD}"endcode"/[^a-z_A-Z0-9]  { // needed to match things like \endcode. (note the dot)
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
<DocScan>{CMD}"e"{BN}+	                { BEGIN( DocEmphasis ); }
<DocScan>{CMD}"a"{BN}+                  { BEGIN( DocEmphasis ); }
<DocScan>{CMD}"b"{BN}+			{ BEGIN( DocBold ); }
<DocScan>{CMD}"c"{BN}+                  { BEGIN( DocCode ); }
<DocScan>{CMD}"l"{BN}+		
<DocScan>"\\n"/[^a-z_A-Z0-9]		{ outDoc->lineBreak(); }
<DocScan>{CMD}"include"{BN}+		{ BEGIN( DocInclude ); }
<DocScan>{CMD}"dontinclude"{BN}+	{ BEGIN( DocDontInclude ); }
<DocScan>{CMD}"skip"{BN}+	 	{ BEGIN( DocSkipKey ); }	
<DocScan>{CMD}"skipline"{BN}+		{ BEGIN( DocSkiplineKey ); firstLine=TRUE; }
<DocScan>{CMD}"line"{BN}+		{ BEGIN( DocLineKey ); firstLine=TRUE; }
<DocScan>{CMD}"until"{BN}+		{ BEGIN( DocUntilKey ); firstLine=TRUE; }
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
<DocKeyEnd>{CMD}"line"{BN}+		{ BEGIN(DocLineKey); }
<DocKeyEnd>{CMD}"until"{BN}+		{ BEGIN(DocUntilKey); }
<DocKeyEnd>{CMD}"skipline"{BN}+		{ BEGIN(DocSkiplineKey); }
<DocKeyEnd>\n
<DocKeyEnd><<EOF>>			{
  					  if (!firstLine) outDoc->endCodeFragment();
					  yyterminate();
  					}
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
<DocScan>"<"{IMG}{ATTR}">"		{ 
                                          /*storeOutputListState();*/
  					  outDoc->pushGeneratorState();
                                          outDoc->disableAllBut(OutputGenerator::Html);
					  outDoc->writeString(yytext); 
                                          /*restoreOutputListState();*/
					  outDoc->popGeneratorState();
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
<DocScan>"<"{OL}{ATTR}">"		{ outDoc->startEnumList(); 
  					  currentListIndentLevel++;
					}
<DocScan>"</"{OL}{ATTR}">"		{ outDoc->endEnumList(); 
  					  currentListIndentLevel--;
					}
<DocScan>"<"{UL}{ATTR}">"		{ outDoc->startItemList(); 
					  currentListIndentLevel++;
					}
<DocScan>"</"{UL}{ATTR}">"		{ outDoc->endItemList(); 
  					  currentListIndentLevel--;
					}
<DocScan>"<"{LI}{ATTR}">"		{ outDoc->writeListItem(); }
<DocScan>"</"{LI}{ATTR}">"		
<DocScan>"<"{TT}{ATTR}">"		{ outDoc->startTypewriter(); }
<DocScan>"</"{TT}{ATTR}">"		{ outDoc->endTypewriter(); }
<DocScan>"<"{EM}{ATTR}">"		{ outDoc->startEmphasis(); }
<DocScan>"</"{EM}{ATTR}">"		{ outDoc->endEmphasis(); }
<DocScan>"<"{HR}{ATTR}">"		{ outDoc->writeRuler(); }
<DocScan>"<"{DL}{ATTR}">"		{ outDoc->startDescription(); 
					  currentListIndentLevel++;
					}
<DocScan>"</"{DL}{ATTR}">"		{ outDoc->endDescription(); 
  					  currentListIndentLevel--;
					}
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
<DocHtmlHref1>{URLMASK}			{ 
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
<DocScan>{CMD}("\\"|"@"|"<"|">"|"&"|"$"|"#"|"%") {
  					  outDoc->docify(&yytext[1]);
  					}
<DocScan>"%"[a-zA-Z_0-9\-]+		{
  					  outDoc->docify(yytext+1);
  					}
<DocEmphasis>[a-z_A-Z][a-z_A-Z:0-9<>]*	{ 
  					  outDoc->startEmphasis();
  					  linkifyText(*outDoc,className,0,yytext);
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
					}
<DocEmphasis>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"()" {
  					  outDoc->startEmphasis();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
  					}
<DocBold>[a-z_A-Z][a-z_A-Z:0-9<>]*	{ 
  					  outDoc->startBold();
  					  linkifyText(*outDoc,className,0,yytext);
					  outDoc->endBold();
					  BEGIN( DocScan );
					}
<DocBold>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"()" {
  					  outDoc->startBold();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endBold();
					  BEGIN( DocScan );
  					}
<DocCode>[a-z_A-Z][a-z_A-Z:0-9<>]*	{ 
  					  outDoc->startTypewriter();
  					  linkifyText(*outDoc,className,0,yytext);
					  outDoc->endTypewriter();
					  BEGIN( DocScan );
					}
<DocCode>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"()" {
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
<DocScan>({B}*"\n"){2,}{B}*"*"*{B}*"-"{B}+	{ // new paragraph & start of a list
  					  if (insideArgumentList)
					  {
					    insideArgumentList=FALSE;
					    outDoc->endItemList();
					  }
					  else if (insideItemList)
					  {
					    forceEndItemList();
					  }
					  else
					  {
					    outDoc->newParagraph(); 
					  }
					  if (inBlock()) endBlock();
  					  addListItemMarker(strrchr(yytext,'\n')+1);
					}
<DocScan>({B}*"\n"){2,}{B}*		{ // new paragraph
  					  if (insideArgumentList)
					  {
					    insideArgumentList=FALSE;
					    outDoc->endItemList();
					  }
					  else if (insideItemList)
					  {
					    forceEndItemList();
					  }
					  else
					  {
					    outDoc->newParagraph(); 
					  }
					  if (inBlock()) endBlock();
					}
<DocScan>{BN}+/\n			{ 
  					  outDoc->writeChar(' '); 
					}
<DocScan>{B}+				{
  					  outDoc->writeChar(' '); 
  					}
<DocScan>\n				{
  					  outDoc->writeChar('\n'); 
  					}
<DocCode,DocEmphasis,DocBold,DocScan,Text>[a-z_A-Z0-9]+ { 
					  outDoc->docify(yytext);
					}
<DocCode,DocEmphasis,DocBold,DocScan,Text>.  { 
					  outDoc->writeChar(*yytext);
					}
<NextSemi>"{"				{
  					  curlyCount=0;
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
					  else
					    BEGIN( NextSemi ) ;
					}
<NextSemi>"'"\\[0-7]{1,3}"'"            
<NextSemi>"'"\\."'"                     
<NextSemi>"'"."'"                       
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
<FindMembers>{BN}+			{
  					  lineCount();
  					}
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
<FindMembers>{B}*"inline"{BN}+		{ current->inLine = TRUE;
                                          lineCount(); 
                                        }
<FindMembers>{B}*"import"{BN}+		{ // IDL import keyword
  					  BEGIN( NextSemi );
  					}
<FindMembers>{B}*"typename"{BN}+	{ lineCount(); }
<FindMembers>{B}*"namespace"{BN}+	{ 
  					  isTypedef=FALSE;
					  current->section = Entry::NAMESPACE_SEC;
					  current->type = "namespace" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount();
  					  BEGIN( ClassName ); 
					}
<FindMembers>{B}*"module"{BN}+		{ 
  					  isTypedef=FALSE;
					  current->section = Entry::NAMESPACE_SEC;
					  current->type = "module" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount();
  					  BEGIN( ClassName ); 
					}
<FindMembers>{B}*"interface"{BN}+ 	{ // M$/Corba IDL interface
  					  isTypedef=FALSE;
					  current->section = Entry::INTERFACE_SEC;
					  addType( current ) ;
					  current->type += " interface" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount();
					  BEGIN( ClassName );
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
					  BEGIN( ClassName );
  					}
<FindMembers>{B}*(("typedef"{BN}+)?)"class"{BN}+ { 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::CLASS_SEC;
					  addType( current ) ;
					  current->type += " class" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"struct"{BN}+ { 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::STRUCT_SEC ;
					  addType( current ) ;
					  current->type += " struct" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"union"{BN}+ { 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::UNION_SEC ;
					  addType( current ) ;
					  current->type += " union" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
					  lineCount() ;
					  BEGIN( ClassName ) ;
					}
<FindMembers>{B}*(("typedef"{BN}+)?)"enum"{BN}+	{ 
					  isTypedef=((QCString)yytext).find("typedef")!=-1;
  					  current->section = Entry::ENUM_SEC ;
					  addType( current ) ;
					  current->type += " enum" ;
					  current->fileName  = yyFileName;
					  current->startLine = yyLineNr;
					  current->bodyLine  = yyLineNr;
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
					  // class template specifier already found => member template specifier
					  // already inside class => member template specifier
					  if (current->tArgList || (current_root->section&Entry::COMPOUND_MASK))
					  {
					    //printf("-------> member template\n");
					    if (current->mtArgList)
					    {
					      current->mtArgList->clear();
                                            }
					    else
					    {
					      current->mtArgList = new ArgumentList;
					      current->mtArgList->setAutoDelete(TRUE);
					    }
					    currentArgumentList = current->mtArgList;
					  }
					  else // class template specifier
					  {
					    //printf("-------> class template\n");
					    if (current->tArgList)
					    {
					      current->tArgList->clear();
                                            }
					    else
					    {
					      current->tArgList = new ArgumentList;
					      current->tArgList->setAutoDelete(TRUE);
					    }
					    currentArgumentList = current->tArgList;
					  }
					  templateStr="<";
					  fullArgString = templateStr.copy();
					  copyArgString = &templateStr;
					  currentArgumentContext = FindMembers;
					  //printf("Start template list\n");
					  BEGIN( ReadTempArgs );
  					}
<FindMembers>"using"{BN}+		{ 
					  current->startLine=yyLineNr; 
                                          lineCount(); 
                                          BEGIN(Using); 
                                        }
<Using>"namespace"{BN}+			{ lineCount(); BEGIN(UsingDirective); }
<Using>";"				{ BEGIN(FindMembers); }
<UsingDirective>{SCOPENAME}		{ current->name=yytext;
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDIR_SEC;
					  //printf("Found using directive %s\n",yytext);
					  current_root->addSubEntry(current);
					  current             = new Entry ;
					  current->protection = protection ;
				          current->sig        = sig;
					  current->virt       = virt;
					  current->stat       = gstat;
					  current->slot       = slot;
					  BEGIN(Using);
  					}
<FindMembers>{SCOPENAME}{BN}*"<>"	{ // guided template decl
					  QCString n=yytext;
					  addType( current );
					  current->name=n.left(n.length()-2);
					}
<FindMembers>{SCOPENAME}{BN}*/"<"	{
  					  sharpCount=0;
					  lineCount();
					  addType( current );
  					  current->name=yytext;
					  current->name=current->name.stripWhiteSpace();
					  current->scopeSpec.resize(0);
					  currentTemplateSpec = &current->scopeSpec;
					  if (nameIsOperator(current->name))
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
					}
<FindMemberName>{SCOPENAME}{BN}*/"<"	{
  					  sharpCount=0;
					  lineCount();
  					  current->name+=((QCString)yytext).stripWhiteSpace();
					  current->memberSpec.resize(0);
					  currentTemplateSpec = &current->memberSpec;
					  if (nameIsOperator(current->name))
					    BEGIN( Operator );
					  else
					    BEGIN( EndTemplate );
  					}
  /*
<SkipTemplate>"<"			{
					  sharpCount++; 
  					}
<SkipTemplate>">"			{
  					  if (--sharpCount<=0)
					  {
					    BEGIN(FindMembers);
					  }
  					}
<SkipTemplate>.
  */

<EndTemplate>"<"			{ 
  					  current->name+='<';
  					  *currentTemplateSpec+='<'; 
					  sharpCount++; 
					}
<EndTemplate>">"			{
  					  current->name+='>';
					  *currentTemplateSpec+='>';
					  if (--sharpCount<=0)
					  {  
					    //printf("Found %s\n",current->name.data());
					    BEGIN(FindMembers);
					  }
					}
<EndTemplate>">"{BN}*"("		{ 
  					  lineCount();
  					  current->name+='>';
					  *currentTemplateSpec+='>';
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
<EndTemplate>">"{BN}*/"::"		{
  					  lineCount();
  					  current->name+='>';
  					  *currentTemplateSpec+='>';
					  if (--sharpCount<=0)
					  {
					    BEGIN(FindMemberName);
					  }
  					}
<EndTemplate>.				{ 
  					  current->name+=*yytext;
  					  *currentTemplateSpec+=*yytext; 
					}
<FindMembers,FindMemberName>{SCOPENAME}	{ 
                                          if (insideIDL && yyleng==9 && strcmp(yytext,"cpp_quote")==0)
					  {
					    BEGIN(CppQuote);
					  }
					  else if (insideIDL && strcmp(yytext,"case")==0)
					  {
					    BEGIN(IDLUnionCase);
					  }
					  else
					  {
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
					    QCString tmp=yytext;
					    if (nameIsOperator(tmp)) 
					      BEGIN( Operator );
					    else
					      BEGIN(FindMembers);
					  }
					}
<CppQuote>"("{B}*"\""			{
  					  insideCppQuote=TRUE;
  					  BEGIN(FindMembers);
  					}
<IDLUnionCase>"::"
<IDLUnionCase>":"			{ BEGIN(FindMembers); }
<IDLUnionCase>\n			{ yyLineNr++; }
<IDLUnionCase>.
<EndCppQuote>")"			{
  					  insideCppQuote=FALSE;
					  BEGIN(FindMembers);
  					}
<FindMembers>{B}*"#"			{ lastCPPContext = YY_START; 
					  BEGIN( SkipCPP ) ; 
					}
<FindMembers>{B}*"#"{B}*"define"	{
  					  current->bodyLine = yyLineNr;
  					  BEGIN( Define );
  					}
<SkipCPP>.
<SkipCPP>\\[\r]*"\n"[\r]*		{ yyLineNr++ ; }
<SkipCPP>[\r]*\n[\r]*			{ yyLineNr++ ;
					  BEGIN( lastCPPContext) ;
					}
<Define>{ID}/"("			{
  					  current->bodyLine = yyLineNr;
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
					  current->protection = protection ;
				          current->sig        = sig;
					  current->virt       = virt;
					  current->stat       = gstat;
					  current->slot       = slot;
					  BEGIN(FindMembers);
  					}
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
					
<FindMembers>[*&]+			{ current->name += yytext ; }
<FindMembers,MemberSpec,Function,NextSemi,ReadInitializer>";"{BN}*("/**"|"//!"|"/*!"|"///")"<" {
  					  lineCount();
					  if (current->bodyLine==-1)
					    current->bodyLine=yyLineNr;
					  lastAfterDocContext = YY_START;
					  afterDocTerminator = ';';
					  if (yytext[yyleng-3]=='/')
					  {
					    current->brief.resize(0);
					    BEGIN(AfterDocLine);
					  }
					  else if (yytext[yyleng-2]=='*' && Config::autoBriefFlag)
					  {
					    current->brief.resize(0);
					    BEGIN(AfterDocBrief);
					  }
					  else
					  {
					    current->doc.resize(0);
					    BEGIN(AfterDoc);
					  }
  					}
<MemberSpec,FindFields,FindMembers,NextSemi,ReadInitializer>","{BN}*("/**"|"//!"|"/*!"|"///")"<" {
  					  lineCount();
					  lastAfterDocContext = YY_START;
					  afterDocTerminator = ',';
					  if (yytext[yyleng-3]=='/')
					  {
					    current->brief.resize(0);
					    BEGIN(AfterDocLine);
					  }
					  else if (yytext[yyleng-2]=='*' && Config::autoBriefFlag)
					  {
					    current->brief.resize(0);
					    BEGIN(AfterDocBrief);
					  }
					  else
					  {
					    current->doc.resize(0);
					    BEGIN(AfterDoc);
					  }
  					}
<DefineEnd,FindFields,FindFieldArg,ReadInitializer>{BN}*("/**"|"//!"|"/*!"|"///")"<" {
  					  lineCount();
					  lastAfterDocContext = YY_START;
					  if (YY_START==DefineEnd)
					    afterDocTerminator = '\n';
					  else
					    afterDocTerminator = 0;
					  if (yytext[yyleng-3]=='/')
					  {
					    current->brief.resize(0);
					    BEGIN(AfterDocLine);
					  }
					  else if (yytext[yyleng-2]=='*' && Config::autoBriefFlag)
					  {
					    current->brief.resize(0);
					    BEGIN(AfterDocBrief);
					  }
					  else
					  {
					    current->doc.resize(0);
					    BEGIN(AfterDoc);
					  }
  					}
<FindMembers>"="			{
  					  current->bodyLine = yyLineNr;
					  lastInitializerContext = YY_START;
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
  					  unput(*yytext);
  					  BEGIN(lastInitializerContext);
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
<ReadInitializer>"'"\\[0-7]{1,3}"'"            
<ReadInitializer>"'"\\."'"                     
<ReadInitializer>"'"."'"                       
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
<CopyRound>"'"\\[0-7]{1,3}"'"           { *pCopyRoundString+=yytext; }
<CopyRound>"'"\\."'"                    { *pCopyRoundString+=yytext; }
<CopyRound>"'"."'"                      { *pCopyRoundString+=yytext; }
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
<CopyCurly>"'"\\[0-7]{1,3}"'"           { *pCopyCurlyString+=yytext; }
<CopyCurly>"'"\\."'"                    { *pCopyCurlyString+=yytext; }
<CopyCurly>"'"."'"                      { *pCopyCurlyString+=yytext; }
<CopyCurly>[^"'{}\n]+			{
  					  *pCopyCurlyString+=yytext;
  					}
<CopyCurly>\n				{
  					  yyLineNr++;
					  *pCopyCurlyString+=*yytext;
  					}

<FindMembers>[:;,]			{ 
  					  QCString oldType = current->type.copy();
					  QCString oldDocs = current->doc.copy();
  					  if ( *yytext != ':')
					  {
					    if (current->bodyLine==-1)
					    {
					      current->bodyLine = yyLineNr;
					    }
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
					  }
					  // skip expression or bitfield if needed
					  if ( *yytext == ':') 
					  {
					    BEGIN( NextSemi );
					  }
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

<FindMembers>"["			{ 
					  if (current->name.isEmpty()) // IDL function property
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
  					  BEGIN(ReadInitializer);
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
  /*
<FindFieldArg>","			{ unput(*yytext); BEGIN(FindFields); }
  */
<Curly>[^\r\n{}"'/]*			{ current->program += yytext ; }
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
<Curly>"'"\\[0-7]{1,3}"'"               { current->program += yytext; }
<Curly>"'"\\."'"                        { current->program += yytext; }
<Curly>"'"."'"                          { current->program += yytext; }
<Curly>"{"				{ current->program += yytext ;
					  ++curlyCount ;
					}
<Curly>"}"				{ //err("Curly count=%d\n",curlyCount);
  					  if ( curlyCount>0 )
					  {
					    current->program += yytext ;
					    --curlyCount ;
					  }
					  else
					  {
					    current->endBodyLine = yyLineNr;
					    QCString &cn = current->name;
					    //QCString rn = stripAnnonymousScope(current_root->name);
					    QCString rn = current_root->name.copy();
					    //printf("cn=`%s' rn=`%s'\n",cn.data(),rn.data());
					    if (!cn.isEmpty() && !rn.isEmpty() && 
					      (current_root->section & Entry::SCOPE_MASK))
					    {
					      cn.prepend(rn+"::");
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
						  current->section==Entry::INTERFACE_SEC
						 )
					      { // namespaces and interfaces ends with a closing bracket without semicolon
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
					    varEntry->sig = current->sig;
					    varEntry->virt = current->virt;
					    varEntry->stat = current->stat;
					    varEntry->slot = current->slot;
					    varEntry->section = Entry::VARIABLE_SEC;
					    varEntry->name = msName.stripWhiteSpace();
					    varEntry->type = current->type.simplifyWhiteSpace()+" ";
					    varEntry->args = msArgs; //current->args.simplifyWhiteSpace();
					    //if (!current->name.isEmpty() && current->name[0]!='@' && 
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
<MemberSpec>"="				{ 
  					  lastInitializerContext=YY_START;
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
<Curly>{BN}+				{ current->program += yytext ;
					  lineCount() ;
					}
<Curly>.				{ current->program += yytext ; }

<FindMembers>"("({ID}{BN}*"::"{BN}*)*("*"{BN}*)+ {
  					  current->bodyLine = yyLineNr;
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
<EndFuncPtr>")"{BN}*/"["		{
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
<FindMembers>"("			{ current->args = yytext;
  					  current->bodyLine = yyLineNr;
					  currentArgumentContext = FuncQual;
					  fullArgString=current->args.copy();
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

<ReadFuncArgType>[^ \/\r\t\n\)\(\"\']+	{ *copyArgString+=yytext; 
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
<ReadFuncArgType>("/*"[*!]|"//"[/!])("<"?)	{ 
  					  fullArgString+=yytext;
  					  lastCopyArgChar=0;
					  if (yytext[1]=='/')
					    BEGIN( CopyArgCommentLine );
					  else
  					    BEGIN( CopyArgComment );
  					}
	/* `)' followed by a special comment */
<ReadFuncArgType>")"{BN}*("/*"[*!]|"//"[/!])"<"	{
  					  lineCount();
					  lastCopyArgChar=*yytext;
					  QCString text=&yytext[1];
					  text=text.stripWhiteSpace();
					  fullArgString+=text;
					  if (text.find("//")!=-1)
					    BEGIN( CopyArgCommentLine );
					  else
					    BEGIN( CopyArgComment );
  					}
<CopyArgComment>[^\n\*]+		{ fullArgString+=yytext; }
<CopyArgComment>"*/"			{ fullArgString+=yytext; 
  					  if (lastCopyArgChar!=0)
					    unput(lastCopyArgChar); 
                                          BEGIN( ReadFuncArgType ); 
					}
<CopyArgCommentLine>\n			{ fullArgString+=yytext;
  					  if (lastCopyArgChar!=0)
					    unput(lastCopyArgChar);
					  BEGIN( ReadFuncArgType );
  					}
<CopyArgCommentLine>[^\n]+		{ fullArgString+=yytext; }
<CopyArgComment>\n			{ fullArgString+=*yytext; yyLineNr++; }
<CopyArgComment>.			{ fullArgString+=*yytext; }
<ReadTempArgs>"<"			{
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
					  argSharpCount=0;
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
					  *copyArgString+=*yytext;
					  fullArgString+=*yytext;
  					}
<CopyArgSharp>">"			{
  					  *copyArgString+=*yytext;
  					  fullArgString+=*yytext;
					  if (argRoundCount>0)
					    argRoundCount--;
					  else
					    BEGIN( ReadTempArgs );
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
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>"'"\\[0-7]{1,3}"'" { 
                                          *copyArgString+=yytext; 
                                          fullArgString+=yytext; 
					}
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>"'"\\."'"   { 
  					  *copyArgString+=yytext; 
  					  fullArgString+=yytext; 
					}
<ReadFuncArgType,ReadTempArgs,CopyArgRound,CopyArgSharp>"'"."'"     { 
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
  					  current->exception = " throw (" ;
					  lineCount() ;
					  BEGIN( ExcpRound ) ;
					}
<FuncQual>{BN}*"raises"{BN}*"("         {
  					  current->exception = " raises (" ;
					  lineCount() ;
					  BEGIN( ExcpRound ) ;
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
					  QCString &cn=current->name;
					  QCString &rn=current_root->name;
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
					    QCString tempName;
					    if (tempArg==-1) 
					      tempName=current->name;
					    else
					      tempName=current->name.left(tempArg);
					    if (current->type.isEmpty() && tempName.find("operator")==-1 && 
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
					    QRegExp re("([^)]*)");
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
					  //printf("Adding entry `%s' inLine`%d' bodyLine=`%d'\n",
					  //    current->name.data(),current->inLine,current->bodyLine);
					  previous = current;
					  current_root->addSubEntry(current);
					  current = new Entry ;
					  current->protection = protection;
				          current->sig = sig;
					  current->virt = virt;
					  current->stat = gstat;
					  current->slot = slot;
					  current->mGrpId = memberGroupId;
					  lastCurlyContext = FindMembers;
					  if( *yytext == '{' )
					  {
					    if (current_root->section & Entry::COMPOUND_MASK)
					      previous->inLine = TRUE;
					    //addToBody(yytext);
					    curlyCount=0;
					    BEGIN( SkipCurly ) ;
					  }
					  else if( *yytext == ':' )
					  {
					    //addToBody(yytext);
					    BEGIN( SkipInits ) ;
					  }
					  else
					  {
					    if (previous->section!=Entry::VARIABLE_SEC)
					      previous->bodyLine=-1; // a function/member declaration
					    BEGIN( FindMembers ) ;
					  }
					}

<SkipInits>"{"				{ 
  				          //addToBody(yytext);
  				          lastCurlyContext = FindMembers;
					  curlyCount=0;
  					  BEGIN( SkipCurly ) ; 
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
					      BEGIN(AfterDocLine);
					    else if (yytext[yyleng-2]=='*' && Config::autoBriefFlag)
					      BEGIN(AfterDocBrief);
					    else
					      BEGIN(AfterDoc);
					  }
					}
<SkipCurlyEndDoc>"}"			{
  				          //addToBody("}");
					  current = tempEntry;
  					  BEGIN( lastCurlyContext );
  					}
<SkipCurly>"'"\\[0-7]{1,3}"'"           {
  				          //addToBody(yytext);
  					}
<SkipCurly>"'"\\."'"                    {
  				          //addToBody(yytext);
  					}
<SkipCurly>"'"."'"                      {
  				          //addToBody(yytext);
  					}
<SkipCurly>\"			        { 
  				          //addToBody(yytext);
  					  lastStringContext=SkipCurly;
  				          BEGIN( SkipString ); 
					}
<SkipCurly>^{B}*"#"			{ 
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
<Bases,ClassName>";"			{ 
					  current->section = Entry::EMPTY_SEC ;
					  current->type.resize(0) ;
					  current->name.resize(0) ;
					  current->args.resize(0) ;
					  current->argList->clear();
					  BEGIN( FindMembers ) ;
					}
<ClassName>{SCOPENAME}{BN}*/"<"		{
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
<ClassName>{SCOPENAME}			{ 
					  current->name = yytext ;
					  lineCount();
					  BEGIN( ClassVar );
					}
<ClassVar>{ID}				{
  					  if (insideIDL && strcmp(yytext,"switch")==0)
					  {
					    // Corba IDL style union
					    roundCount=0;
					    BEGIN(SkipUnionSwitch);
					  }
					  else
					  {
					    if (isTypedef)
					    {
					      //  typedefDict.insert(yytext,new QCString(current->name));
					      //  current->type.prepend("typedef ");
					      //  current->extends
					      current->extends->append(
						  new BaseInfo(yytext,Public,Normal)
								      );
					    }
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
					  if (current->section == Entry::INTERFACE_SEC)
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
<ClassName,ClassVar>{B}*"{"{B}*		{ current->fileName = yyFileName ;
					  current->startLine = yyLineNr ;
					  current->name = removeRedundantWhiteSpace(current->name);
					  if (current->name.isEmpty() && !isTypedef) // anonymous compound
					    current->name.sprintf("@%d",anonCount++);
					  curlyCount=0;
					  BEGIN( Curly ) ;
					}
<BasesProt>"virtual"                    { baseVirt = Virtual; }
<BasesProt>"public"                     { baseProt = Public; }
<BasesProt>"protected"                  { baseProt = Protected; }
<BasesProt>"private"                    { baseProt = Private; }
<BasesProt>{BN}				{ lineCount(); }
<BasesProt>.				{ unput(*yytext); BEGIN(Bases); }
<Bases>("::")*{BN}*({ID}{BN}*"::"{BN}*)*{ID}	{ 
  					  //current->extends->append(
					  //  new BaseInfo(yytext,baseProt,baseVirt)
					  //) ;
  					  if (*yytext != ':')
  					    baseName += yytext;
					  else
					    baseName += (yytext+2);
					  current->args += ' ';
					  if (*yytext != ':')
					    current->args += yytext;
					  else
					    current->args += (yytext+2);
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
					  current->name = removeRedundantWhiteSpace(current->name);
  					  if (!baseName.isEmpty())
  					    current->extends->append(
					      new BaseInfo(baseName,baseProt,baseVirt)
					    );
					  if (current->section == Entry::INTERFACE_SEC)
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
					  BEGIN( Curly ) ;
					}
<SkipUnionSwitch>"("			{
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
<Comment>.*"*/"				{ current->program += yytext ;
					  BEGIN( lastContext ) ;
					}
<Comment>.				{ current->program += *yytext ; }

<FindMembers,FindFields,MemberSpec,FuncQual,SkipCurly,Operator>("//"{B}*)?"/*!" { 
  					  //printf("Start doc block at %d\n",yyLineNr);
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
					  lastDocContext = YY_START;
					  if (current_root->section & Entry::SCOPE_MASK)
					    current->inside = current_root->name+"::";
					  if (!Config::autoBriefFlag) // use the Qt style
					  {
					    tmpDocType=-1;
					    if (YY_START==Curly)
					      current->doc+="\n\n";
					    else
  					      current->doc.resize(0);
					    BEGIN( Doc );
					  }
					  else // Use the javadoc style
					  {
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
					      BEGIN( JavaDoc );
					    }
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
<FindMembers>"extern"{BN}+"\"C"("++")?"\""{BN}*("{")?
<FindMembers>"{"			{
  					  current->type.resize(0);
  					  current->name.resize(0);
  					  current->args.resize(0);
					  current->argList->clear();
					  curlyCount=0;
					  BEGIN( SkipCurlyBlock );
					  	
  					}
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
<JavaDoc>"@"				{
  					  unput(*yytext);
					  BEGIN(ClassDoc);
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
<JavaDoc>{CMD}("image"|"author"|"internal"|"version"|"date"|"param"|"exception"|"return"[s]?|"retval"|"bug"|"warning"|"par"|"sa"|"see"|"pre"|"post"|"invariant"|"note") { 
  					  current->doc+=yytext;
					  BEGIN( tmpDocType );
  					}
<Doc,JavaDoc>{B}*{CMD}("fn"|"var"|"typedef"){B}*	{
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
<Doc,JavaDoc>{B}*{CMD}"enum"{B}*	{
  					  current->section = Entry::ENUMDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( EnumDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"defgroup"{B}* {
  					  current->section = Entry::GROUPDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( GroupDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"namespace"{B}*	{
  					  current->section = Entry::NAMESPACEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
  					  BEGIN( NameSpaceDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"class"{B}*  	{
  					  current->section = Entry::CLASSDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"union"{B}*  	{
  					  current->section = Entry::UNIONDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"struct"{B}* 	{
  					  current->section = Entry::STRUCTDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"interface"{B}* 	{
  					  current->section = Entry::INTERFACEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"idlexcept"{B}* 	{
  					  current->section = Entry::EXCEPTIONDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( ClassDocArg1 ); 
					}
<Doc,JavaDoc>{B}*{CMD}"page"{B}*   	{
  					  current->section = Entry::PAGEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( PageDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"mainpage"{B}* 	{
  					  current->section = Entry::MAINPAGEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( PageDocArg2 );
  					}
<Doc,JavaDoc>{B}*{CMD}"file"{B}*	{
  					  current->section = Entry::FILEDOC_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
					  BEGIN( FileDocArg1 );
  					}
<Doc,JavaDoc>{B}*{CMD}"example"{B}* 	{
 					  current->section = Entry::EXAMPLE_SEC;
					  current->fileName = yyFileName;
					  current->startLine = yyLineNr;
 					  BEGIN( ExampleDocArg1 );
					}
<ExampleDocArg1>{FILE}			{
				          current->name = stripQuotes(yytext);	
					  BEGIN( ExampleDoc );
					}
<ClassDoc,Doc,JavaDoc>{B}*{CMD}"relate"[sd]{B}*    { 
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
<NameSpaceDocArg1>"\\"{B}*"\n"		{ yyLineNr++; }
<NameSpaceDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\namespace at line %d of %s.\n",yyLineNr,yyFileName);
  					  yyLineNr++;
  					}
<ClassDocArg1>{SCOPENAME}/"<"		{
					  current->name = yytext;
					  // prepend outer scope name 
					  if (current_root->section & Entry::SCOPE_MASK)
					  {
					    current->name.prepend(current_root->name+"::");
					  }
					  lastClassTemplSpecContext = ClassDocArg2;
					  BEGIN( ClassTemplSpec );
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
<ClassDocArg1>"\\"{B}*"\n"		{ yyLineNr++; }
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
<GroupDocArg1>"\\"{B}*"\n"		{ yyLineNr++; }
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
<GroupDocArg2>"\\"{B}*"\n"		{ yyLineNr++; }
<GroupDocArg2>"\n"			{
 					  newDocState();
  					}
<ClassDocArg2>{FILE}			{
  					  //printf("ClassDocArg2=%s\n",yytext);
					  current->includeFile = stripQuotes(yytext);
					  BEGIN( ClassDocArg3 );
					}
<ClassDocArg2>"\\"{B}*"\n"		{ yyLineNr++; }
<ClassDocArg2>"\n"			{ yyLineNr++; 
					  newDocState();
					}
<ClassDocArg3>[<]?{FILE}[>]?		{
  					  //printf("ClassDocArg3=%s\n",yytext);
 					  current->includeName = yytext;
  					  newDocState();
					}
<ClassDocArg3>"\\"{B}*"\n"		{ yyLineNr++; }
<ClassDocArg3>"\n"			{ yyLineNr++; 
  					  newDocState();
					}
<FileDocArg1>{FILE}			{
  					  current->name = stripQuotes(yytext);
					  newDocState();
  				        }
<FileDocArg1>"\\"{B}*"\n"		{ yyLineNr++; }
<FileDocArg1>"\n"			{
  					  current->name = yyFileName;
  					  yyLineNr++;
					  newDocState();
  					}
<PageDocArg1>{FILE}			{
					  current->name = stripQuotes(yytext);
					  BEGIN( PageDocArg2 ); 
					}
<PageDocArg1>"\\"{B}*"\n"		{ yyLineNr++; }
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
<EnumDocArg1>"\\"{B}*"\n"		{ yyLineNr++; }
<EnumDocArg1>"\n"			{
  					  warn("Warning: missing argument after "
					       "\\enum at line %d of %s.\n",yyLineNr,yyFileName);
  					  yyLineNr++;
					  BEGIN( Doc );
  					}
<PageDoc>{CMD}"refitem".*"\n"		{
  					  current->doc+=yytext;
  					}
<PageDoc>{CMD}"section"{B}+		{
  					  sectionType=SectionInfo::Section;
  					  BEGIN(SectionLabel);
  					}
<PageDoc>{CMD}"subsection"{B}+		{
  					  sectionType=SectionInfo::Subsection;
  					  BEGIN(SectionLabel);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"mgroup"{B}+ {
  					  //printf("--> mgroup found!\n");
  					  lastMemberGroupContext = YY_START;
					  if (memberGroupId!=-1)
					  {
  					    warn("Warning: ignoring nested mgroup command "
					         "at line %d of %s. Previous command was found at line %d\n",
						 yyLineNr,yyFileName,lastMemberGroupLine);
					  }
					  else
					  {
					    memberGroupId   = newMemberGroupId();
					    current->mGrpId = memberGroupId;
					    lastMemberGroupLine = yyLineNr;
					  }
					  BEGIN(GroupHeader);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"endmgroup"/[^a-z_A-Z0-9] {
  					  //printf("--> endmgroup found!\n");
  					  memberGroupId = -1;
					  current->mGrpId = -1;
  					}
<GroupHeader>[^\n]*/"\n"		{
					  QCString header = ((QCString)yytext).stripWhiteSpace();
  					  memberGroupDict.insert(memberGroupId,
					                         new MemberGroup(memberGroupId,header)
								);
  					  BEGIN(lastMemberGroupContext);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"anchor"{B}+ {
  					  lastAnchorContext = YY_START;
  					  sectionType=SectionInfo::Anchor;
					  BEGIN(AnchorLabel);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>("\\\\"|"@@")"verbatim"/[^a-z_A-Z0-9] {
					  current->doc+="\\\\verbatim";
					}
<Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"verbatim"/[^a-z_A-Z0-9] {
					  lastVerbState=YY_START;
					  current->doc+="\\verbatim";
  					  BEGIN(SkipVerbatim);
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"addindex"{B}+[^\n]+ {
  					  current->doc+=yytext;
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>("\\\\"|"@@")"code"/[^a-z_A-Z0-9] {
  					  current->doc+="\\\\code";
  					}
<Doc,PageDoc,JavaDoc,ClassDoc>{CMD}"code"/[^a-z_A-Z0-9] {
  					  lastCodeState=YY_START;
					  current->doc+="\\code";
					  BEGIN(SkipCode);
					}
<Doc,PageDoc,JavaDoc,ClassDoc>"<"{PRE}{ATTR}">" {
  					  lastCodeState=YY_START;
					  current->doc+="<PRE>";
					  BEGIN(SkipCode);
  					}
<SkipVerbatim>{CMD}"endverbatim"/[^a-z_A-Z0-9] {
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
<AnchorLabel>{ID}			{
  					  sectionLabel=yytext;
					  addSection();
					  current->doc += "\\anchor "+sectionLabel+"\n";
					  BEGIN(lastAnchorContext);
  					}
<SectionLabel>{ID}			{
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
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc>("\\\\"|"@@")"f"[$\[\]] {
							  current->doc += &yytext[1]; 
							}
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc,AfterDoc>{CMD}"f$"	{ 
  							  lastFormulaContext = YY_START;
  							  formulaText="$"; 
							  BEGIN(ReadFormulaShort); 
							}
<Doc,JavaDoc,LineDoc,ExampleDoc,ClassDocBrief,PageDoc,ClassDoc,AfterDoc>{CMD}"f["	{ 
  							  lastFormulaContext = YY_START;
  							  formulaText="\\["; 
							  BEGIN(ReadFormulaLong); 
							}
<ReadFormulaShort>{CMD}"f$"				{
  							  formulaText+="$";
							  if (lastFormulaContext==ClassDocBrief || 
							      lastFormulaContext==LineDoc || 
							      lastFormulaContext==JavaDoc
							     )
							    current->brief += addFormula();
							  else
							    current->doc += addFormula();
							  BEGIN(lastFormulaContext);
  							}
<ReadFormulaLong>{CMD}"f]"					{
							  formulaText+="\\]";
							  if (lastFormulaContext==ClassDocBrief ||
							      lastFormulaContext==LineDoc ||
							      lastFormulaContext==JavaDoc
							     )
							    current->brief += addFormula();
							  else
							    current->doc += addFormula();
							  BEGIN(lastFormulaContext);
  							}
<ReadFormulaLong,ReadFormulaShort>.                     { formulaText+=*yytext; }
<ExampleDoc,PageDoc,ClassDocBrief,SkipCode,ClassDoc>{B}*"*/"  	{
  					  checkDocs();
					  if (YY_START==SkipCode) // premature end of code block
					  {
					    err("Error: comment block ended inside \\code ... \\endcode block at line %d in %s!\n",
						yyLineNr,yyFileName);
					    current->doc += "\\endcode\n\n";
					    BEGIN( lastDocContext );
					  }
					  else if (YY_START==ClassDocBrief &&
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
<ClassDoc,Doc,JavaDoc>{CMD}"ingroup"{B}+   { 
  					  lastGroupContext = YY_START;
  					  lineCount();
  					  BEGIN( GroupName ); 
					}
<GroupName>{ID}				{
  					  current->groups->append(
					    new QCString(yytext)
					  );
  					}
<GroupName>\n				{
  					  yyLineNr++; BEGIN( lastGroupContext );
  					}
<ClassDoc,Doc>{B}*{CMD}("brief"|"short")	{ 
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
					  if (!current->doc.isEmpty()) current->doc+="<p>";
  					  yyLineNr++; 
					  BEGIN( lastBriefContext );
					}
<ClassDocBrief>"\n"			{ yyLineNr++ ; current->brief += " "; }
<ClassDocBrief>"<"{BR}{ATTR}">"
<ClassDocBrief>{BS}/{CMD}"ingroup"	{
  					  current->brief=current->brief.stripWhiteSpace();
					  BEGIN( lastBriefContext ); 
  					}
<ClassDocBrief>{BS}/{CMD}("image"|"author"|"internal"|"version"|"date"|"deprecated"|"param"|"exception"|"return"[s]?|"retval"|"bug"|"warning"|"par"|"sa"|"see") { 
                                          BEGIN( lastBriefContext ); 
                                        }
<ClassDocBrief>{BS}/{CMD}("brief"|"short"){BN}+ {
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
<Doc,AfterDoc,LineDoc,ClassDoc>("\\\\"|"@@")	{ current->doc += yytext; }
<Doc>.					{ current->doc += *yytext; }
<DefLineDoc,LineDoc>.			{ current->brief += *yytext; }
<Doc>\n					{ yyLineNr++; current->doc += *yytext; }
<LineDoc>[\n\r]+{B}*"//"[!/]		{ lineCount(); }
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
  					  yyLineNr++;
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
<LineDoc,AfterDocBrief,AfterDocLine>{CMD}"internal"	{ 
                                          current->brief+="\\internal"; 
					}
<Doc,AfterDoc>{CMD}"internal"		{ 
                                          current->doc+="\\internal"; 
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
<ClassDocRelates,ClassDocFunc,ClassDocDefine,GroupDocArg1,ClassDocArg1,SectionTitle,EnumDocArg1,FileDocArg1,PageDocArg1,ExampleDocArg1,ClassDefineArgs,GroupName>"*/" {
					  current->doc += "\n\n";
					  err("Warning: unexpected end of "
				              "documentation block found in "
					      "file %s at line %d\n",yyFileName,yyLineNr);
					  BEGIN( lastDocContext );
					}
<Doc>"*/"				{ 
  				          checkDocs();
  					  current->doc += "\n\n";
					  //printf("End of docs at line %d\n",yyLineNr);
  					  BEGIN( lastDocContext ); 
					}
<JavaDoc>"*/"				{ 
					  unput('/');unput('*');
					  BEGIN( tmpDocType );
					}
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,ClassDocBrief>^{B}*(("//"{B}*)?)"*"+[ \t]*"-"{B}+ { 
  				         current->doc += yytext; 
					}
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,ClassDocBrief>^{B}*(("//"{B}*)?)"*"+/[^/]
<Doc,JavaDoc,ClassDoc,PageDoc,ExampleDoc,ReadFormulaShort,ReadFormulaLong,ClassDocBrief>^{B}*(("//"{B}*)?)"*"+{B}+ { 
  					  current->doc+=' '; 
					}
<DefLineDoc,LineDoc,ClassDoc,Doc>"/*"   { current->doc += yytext; }
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

//static void addToBody(const char *text)
//{
//  if (Config::includeSourceFlag)
//    previous->body+=text;
//}

//static void addToBodyCond(const char *text)
//{
//  if (Config::includeSourceFlag && lastStringContext==SkipCurly)
//    previous->body+=text;
//}

//----------------------------------------------------------------------------

void scanString(const char *s)
{
  const char *oldInputString = inputString;
  int oldInputPosition = inputPosition;
  int oldRule = YY_START;
  YY_BUFFER_STATE oldBuffer = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(scanYYin, YY_BUF_SIZE));  
  inputString  = s;
  inputPosition = 0;
  BEGIN( Text );
  scanYYlex();
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(oldBuffer);
  inputString = oldInputString;
  inputPosition = oldInputPosition;
  BEGIN( oldRule );
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
    if (!ce->program.isEmpty())
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
      //printf("---> Inner block starts at line %d\n",yyLineNr);
      //current->reset();
      current = new Entry;
      // set default protection based on the compound type
      if( ce->section==Entry::CLASS_SEC ) // class
        current->protection = protection = Private ;
      else if (ce->section == Entry::ENUM_SEC ) // enum
	current->protection = protection = ce->protection;
      else if (!ce->name.isEmpty() && ce->name.at(0)=='@') // anonymous union
	current->protection = protection = ce->protection;
      else // named struct, union, or interface
	current->protection = protection = Public ;
      sig = FALSE;
      slot = FALSE;
      gstat = FALSE;
      virt = Normal;
      scanYYlex() ;
      delete current; current=0;
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
  delete current; current=0;
  parseCompounds(rt);
}

//----------------------------------------------------------------------------

void parseDocument(OutputList &ol,const QCString &docString)
{
  //inParamBlock=inSeeBlock=inReturnBlock=FALSE;
  curTable = 0;
  outDoc = new OutputList(&ol);
  currentIncludeFile.resize(0);
  includeFileOffset=0;
  includeFileLength=0;
  currentListIndentLevel=0;
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
  if (insideItemList) { forceEndItemList(); }
  if (inBlock()) endBlock();
  if (currentListIndentLevel!=0)
  {
    if (!refName.isEmpty())
    {
      warn("Warning: Documentation block of %s ended in the middle " 
	   "of a list (indent level %d)!\n",refName.data(),
	   currentListIndentLevel);
    }
    else if (!exampleName.isEmpty())
    {
      warn("Warning: Documentation block of %s ended in the middle " 
	   "of a list (indent level %d)!\n",exampleName.data(),
	   currentListIndentLevel);
    }
    else
    {
      warn("Warning: Documentation block ended in the middle " 
	   "of a list (indent level %d)!\n",currentListIndentLevel);
    }
  }
  ol+=*outDoc;
  delete outDoc; outDoc=0;
  return;
}

//----------------------------------------------------------------------------

void parseDoc(OutputList &ol,const char *clName,
              const char *memName,const QCString &docString)
{
  //printf("Doc---------\n%s\n---------\n",docString.data());
  initParser();
  initParseCodeContext();
  exampleDoc=FALSE;           // do not cross reference with member docs
  className=clName;
  memberName=memName;
  if (memName)
  {
    refName=className+"::"+memberName;
  }
  else if (clName)
  {
    refName=className;
  }
  else
  {
    refName="<unknown>";
  }
  parseDocument(ol,docString);
}

//----------------------------------------------------------------------------

void parseText(OutputList &ol,const QCString &txtString)
{
  if (txtString.isEmpty()) return;
  inputString = txtString;
  outDoc = new OutputList(&ol);
  inputPosition = 0;
  scanYYrestart( scanYYin ); 
  BEGIN( Text );
  scanYYlex();
  if (memberGroupId!=-1)
  {
    warn("Warning: Missing \\endmgroup in file %s\n",yyFileName);
    memberGroupId=-1;
  }
  ol+=*outDoc;
  delete outDoc; outDoc=0;
  return;
}

//----------------------------------------------------------------------------

void parseExample(OutputList &ol,const QCString &docString, 
                  const char *fileName)
{
  initParser();
  initParseCodeContext();
  exampleDoc=TRUE;            // cross reference with member docs
  exampleName=fileName;
  refName="example "+exampleName;
  parseDocument(ol,docString); 
}

//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void scannerYYdummy() { yy_flex_realloc(0,0); } 
}
