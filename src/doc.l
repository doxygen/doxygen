/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
#include <iostream.h>
#include <assert.h>
#include <ctype.h>

#include "qtbc.h"
#include <qarray.h>
#include <qstack.h>
#include <qregexp.h>
  
#include "doc.h"
#include "code.h"
#include "message.h"
#include "doxygen.h"
#include "config.h"
#include "util.h"
#include "language.h"
#include "outputlist.h"
#include "reflist.h"

#ifndef WIN32
#include <unistd.h>
#endif
  
#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 * scanner's state variables
 */
static bool             insideArgumentList;
static QCString         className;
static QCString         memberName;
static OutputList *     outDoc;
static QCString         linkRef;
static QCString         linkText;
static QCString         codeBlock;
static const char *     inputString;
static int		inputPosition;
static char		yyFileName[4096] ;
static int		yyLineNr     = 1 ;
static bool             exampleDoc;
static QCString         exampleName;
static QCString         htmlUrl,htmlText;
static QCString         currentIncludeFile;
static int              includeFileOffset = 0;
static int              includeFileLength = 0;
static bool             firstLine;
static bool             inParamBlock;
static bool             inRetValBlock;
static bool             inExceptionBlock;
static bool             inSeeBlock;
static bool             inReturnBlock;
static bool             inAuthorBlock;
static bool             inDeprecatedBlock;
static bool             inVersionBlock;
static bool             inSinceBlock;
static bool             inDateBlock;
static bool             inBugBlock;
static bool             inNoteBlock;
static bool             inPreBlock;
static bool             inPostBlock;
static bool             inInvarBlock;
static bool             inWarningBlock;
static bool             inRemarkBlock;
static bool             inAttentionBlock;
static bool             inParBlock;
static QCString         sectionRef;
static bool             insideVerbatim = FALSE;
static bool             insidePre = FALSE;
static int              depthIf;
//static int              currentListIndentLevel;
static QStack<char>	currentListIndent;
static QCString         curImageName;
static QCString         curImageCaption;
static QCString         internalRefFile;
static QCString         internalRefAnchor;

//-----------------------------------------------------------------------------

static void initParser()
{
  insideArgumentList=FALSE;
  className.resize(0);
  memberName.resize(0);
  linkRef.resize(0);
  linkText.resize(0);
  codeBlock.resize(0);
  htmlUrl.resize(0);
  htmlText.resize(0);
  currentIncludeFile.resize(0);
  includeFileOffset = 0;
  includeFileLength = 0;
  firstLine = TRUE;
  inParamBlock = FALSE;
  inRetValBlock = FALSE;
  inExceptionBlock = FALSE;
  inSeeBlock = FALSE;
  inReturnBlock = FALSE;
  inAuthorBlock = FALSE;
  inDeprecatedBlock = FALSE;
  inVersionBlock = FALSE;
  inSinceBlock = FALSE;
  inDateBlock = FALSE;
  inBugBlock = FALSE;
  inNoteBlock = FALSE;
  inPreBlock = FALSE;
  inPostBlock = FALSE;
  inInvarBlock = FALSE;
  inWarningBlock = FALSE;
  inRemarkBlock = FALSE;
  inAttentionBlock = FALSE;
  inParBlock = FALSE;
}

//-----------------------------------------------------------------------------

void scanString(const char *s);
void internalParseDocument(const char *s);

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

static void forceEndTable()
{
  err("Error: More <table> tags found than </table> "
      "tags in documentation block in file %s!\n",yyFileName); 
  while (!tableStack.isEmpty())
  {
    endTable();
  }
}

//-----------------------------------------------------------------------------

static void endArgumentList()
{
  if (insideArgumentList)
  {
    insideArgumentList=FALSE;
    outDoc->endItemList();
  }
}

static void includeFile(OutputList &ol,const char *fileName,bool quiet)
{
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(exampleNameDict,fileName,ambig)))
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
    QCString text;
    text.sprintf("Include file name %s is ambigious.\n",fileName);
    text+="Possible candidates:\n";
    text+=showFileDefMatches(exampleNameDict,fileName);
    warn(yyFileName,yyLineNr,text);
  }
  else
  {
    warn(yyFileName,yyLineNr,
         "Warning: example file %s is not found. "
         "Check your EXAMPLE_PATH",fileName
	);
  }
}

static void verbIncludeFile(OutputList &ol,const char *name)
{
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(exampleNameDict,name,ambig)))
  {
    ol.startCodeFragment();
    ol.codify(fileToString(fd->absFilePath()));
    ol.endCodeFragment();
  }
  else if (ambig)
  {
    QCString text;
    text.sprintf("Include file name %s is ambigious.\n",name);
    text+=("Possible candidates:\n");
    text+=showFileDefMatches(exampleNameDict,name);
    warn(yyFileName,yyLineNr,text);
  }
  else
  {
    warn(yyFileName,yyLineNr,
         "Warning: example file %s is not found. "
         "Check your EXAMPLE_PATH",name);
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

//-----------------------------------------------------------------

struct IndentInfo
{
  public:
    IndentInfo(int i,bool e) : indent(i), enumerated(e) {};
   ~IndentInfo() {}
    void startList() 
    { 
      if (enumerated) outDoc->startEnumList(); else outDoc->startItemList();
    }
    void endList()
    {
      if (enumerated) outDoc->endEnumList(); else outDoc->endItemList();
    }
    void writeItem()
    {
      outDoc->writeListItem();
    }
    int indent;
    bool enumerated;
};

static QStack<IndentInfo> listIndentStack;
static bool insideItemList = FALSE;

static void addListItemMarker(const char *marker,int dashPos,bool enumerated)
{
  // find the actual position at which the bullet was found
  int i;
  int indent=0;
  for (i=0;i<dashPos;i++)
  {
    //printf("Parsed[%d]=%d\n",i,marker[i]);
    if (marker[i]=='\t')
    {
      indent+=Config::tabSize - (indent%Config::tabSize);
    }
    else if (marker[i]=='\n')
    {
      indent=0;
    }
    else
    {
      indent++;
    }
  }
  //printf("list marker found at column %d enumerated %d\n",indent,enumerated);
  if (!insideItemList)
  {
    listIndentStack.push(new IndentInfo(indent,enumerated));
    listIndentStack.top()->startList();
    listIndentStack.top()->writeItem();
    insideItemList=TRUE;
  } 
  else
  {
    IndentInfo *pPrevInfo = listIndentStack.top(); 
    if (pPrevInfo->indent==indent && pPrevInfo->enumerated==enumerated) 
      // new item of same kind at the same indent level
    {
      pPrevInfo->writeItem(); 
    }
    else if (pPrevInfo->indent==indent)
      // new item of diffent kind at the same indent level
    {
      // switch to a diffent list type
      pPrevInfo->endList();
      pPrevInfo->enumerated=enumerated;
      pPrevInfo->startList();
      pPrevInfo->writeItem(); 
    }
    else if (pPrevInfo->indent<indent) // start sub item list
    {
      listIndentStack.push(new IndentInfo(indent,enumerated));
      listIndentStack.top()->startList();
      listIndentStack.top()->writeItem();
    }
    else // end sub item list
    {
      pPrevInfo->endList();
      listIndentStack.pop();
      delete pPrevInfo;
      // safe guard against wrong indenting
      if (listIndentStack.isEmpty()) 
      {
	insideItemList=FALSE;
	warn(yyFileName,yyLineNr,
             "Warning: list item with invalid indent found!");
      }
      else
      {
	listIndentStack.top()->writeItem();
      }
    }
  }
}

// end the current (nested) list regardless of the nesting level.
static void forceEndItemList()
{
  IndentInfo *info;
  while ((info=listIndentStack.pop())!=0)
  {
    info->endList();
    delete info;
  }
  insideItemList=FALSE;
}

//-----------------------------------------------------------------

static bool inBlock()
{
  return inParamBlock || inRetValBlock || inSeeBlock || inReturnBlock || inAuthorBlock ||
         inVersionBlock || inSinceBlock || inDateBlock || inWarningBlock || inRemarkBlock || 
	 inAttentionBlock || inBugBlock || inNoteBlock ||
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
    inVersionBlock=inSinceBlock=inDateBlock=inBugBlock=inNoteBlock=inWarningBlock=
    inParBlock=inExceptionBlock=inDeprecatedBlock=inPreBlock=inPostBlock=
    inInvarBlock=inRemarkBlock=inAttentionBlock=FALSE;
}

//-----------------------------------------------------------------

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
  //printf("Search for %s\n",fileName);
  if ((fd=findFileDef(imageNameDict,fileName,ambig)))
  {
    QFile inImage(QString(fd->absFilePath().data()));
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
      QFile outImage(QString(outputFile.data()));
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
	warn(yyFileName,yyLineNr,
             "Warning: could not write output image %s",outputFile.data());
      }
    }
    else
    {
      warn(yyFileName,yyLineNr,
           "Warning: could not open image %s",fileName);
    }
  }
  else if (ambig)
  {
    QCString text;
    text.sprintf("Warning: image file name %s is ambigious.\n",fileName);
    text+="Possible candidates:\n";
    text+=showFileDefMatches(imageNameDict,fileName);
    warn(yyFileName,yyLineNr,text);
  }
  else
  {
    result=fileName;
    if (result.left(5)!="http:" && result.left(6)!="https:")
    {
      warn(yyFileName,yyLineNr,
           "Warning: image file %s is not found in IMAGE_PATH: "  
	   "assuming external image.",fileName
          );
    }
  }
  return result;
}


void writeImage(ImageTypes it,const char *size)
{
  bool hasCaption=!curImageCaption.isEmpty();
  outDoc->pushGeneratorState();
  switch(it)
  {
    case IT_Latex:
      {
	outDoc->disableAllBut(OutputGenerator::Latex);
	outDoc->startImage(curImageName,size,hasCaption);
	if (hasCaption)
	{
	  scanString(curImageCaption);
	}
	outDoc->endImage(hasCaption);
      }
      break;
    case IT_Html:
      {
	outDoc->disableAllBut(OutputGenerator::Html);
	outDoc->startImage(curImageName,0,hasCaption);
	if (hasCaption)
	{
	  scanString(curImageCaption);
	}
	outDoc->endImage(hasCaption);
      }
      break;
  }
  outDoc->popGeneratorState();
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

//ATTR      ((({BN}+[^\>]+)/">")?)
%}

CMD	  ("\\"|"@")
BN        [ \t\n\r]
BL        [ \t\r]*"\n"
B         [ \t]
BS        ^(({B}*"//")?)(({B}*"*"+)?){B}*
FILESCHAR [a-z_A-Z0-9\\:\\\/\-\+]
FILEECHAR [a-z_A-Z0-9\-\+]
FILEMASK  {FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)+
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]+"\"")
ID        [a-z_A-Z][a-z_A-Z0-9]*
SCOPENAME (({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
SCOPEMASK {ID}?(("::"|"#")?(~)?{ID})+
URLMASK   [a-z_A-Z0-9\~\:\?\@\#\.\-\+\/\=]+
NONTERM   [\{\}\[\]\`\~\@\|\-\+\#\$\/\\\!\%\^\&\*()a-z_A-Z<>0-9]
WORD      ({NONTERM}+([^\n\t ]*{NONTERM}+)?)|("\""[^\n\"]"\"")
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
DOCPARAM  ([a-z_A-Z0-9:\<\>\=\.\-]+)|("\"".*"\"")

%option noyywrap

%x	Text
%x      DocScan
%x	DocParam
%x	DocException
%x	DocHtmlScan
%x	DocLatexScan
%x      DocEmphasis
%x      DocBold
%x      DocCode
%x	DocIf
%x      DocCodeBlock
%x      DocInternal
%x      DocLink
%x      DocLinkText
%x      DocSkipWord
%x      DocInclude
%x      DocDontInclude
%x      DocHtmlLink
%x      DocHtmlAnchor
%x      DocHtmlHref1
%x      DocHtmlHref2
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
%x	DocRefArg
%x	DocRefArgStart
%x      DocRefItem
%x      DocRefItemName
%x	DocInternalRef
%x	DocInternalRefText
%x	DocImage
%x	DocHtmlImageName
%x	DocHtmlImageOpt
%x	DocLatexImageName
%x	DocLatexImageOpt

%%

<*>\x0d
<DocScan>^{B}*(("//"{B}*)?)"*"*{B}*"-"("#")?{B}+ { /* found list item marker */
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  //printf("dashPos=%d char='%c'\n",dashPos,text.at(dashPos+1));
					  bool isEnumerated = text.at(dashPos+1)=='#';
  					  addListItemMarker(yytext,dashPos,isEnumerated);
  					}
<DocScan>\n{B}*(("//"{B}*)?)"*"*{B}*"-"("#")?{B}+ { /* found list item marker */
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  //printf("dashPos=%d char='%c'\n",dashPos,text.at(dashPos+1));
					  bool isEnumerated = text.at(dashPos+1)=='#';
  					  addListItemMarker(yytext+1,dashPos,isEnumerated);
  					}
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
<DocHtmlScan,DocLatexScan>.|\n		{
  					  char c[2];
					  c[0]=*yytext;c[1]='\0';
  					  outDoc->writeString(c);
  					}
<DocScan>"\\postheader"/{BN}
<DocScan>"\\functionindex"/{BN}		{ writeMemberList(*outDoc,FALSE); }
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
					  insideVerbatim=TRUE;
					  BEGIN(DocVerbatim);
  					}
<DocVerbatim>{CMD}"endverbatim"/[^a-z_A-Z0-9]	{
  					  outDoc->endCodeFragment();
					  insideVerbatim=FALSE;
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
<DocLink>[a-z_A-Z0-9:#.,~&*/<>()\-\+]+  {  // TODO: support operators as well!
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
<DocScan>{CMD}"endlink"/[^a-z_A-Z0-9]   { warn(yyFileName,yyLineNr,
                                               "Warning: \\endlink without \\link "
    					       "in documentation of this entity."
                                              ); 
					}
<DocScan>{CMD}"addindex"{B}+		{
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
<DocScan>{CMD}("arg"|"li")/{BN}	        { 
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"par"{B}*			{
  					  BEGIN(DocPar);
  					}
<DocPar>[^\n]*{BN}			{
  					  QCString title=QCString(yytext).stripWhiteSpace();
					  bool b = inBlock();
					  if (!title.isEmpty())
					  {
  				            endArgumentList();
					    if (b) endBlock();  
					    inParBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    outDoc->docify(title);
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->newParagraph();
					  }
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"warning"/{BN}	{ 
  				          endArgumentList();
  					  if (!inWarningBlock)
					  {
					    if (inBlock()) endBlock();
					    inWarningBlock=TRUE;
  					    outDoc->startDescList();
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
<DocScan>{CMD}"remark"[s]?/{BN}		{ 
  				          endArgumentList();
  					  if (!inRemarkBlock)
					  {
					    if (inBlock()) endBlock();
					    inRemarkBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trRemarks()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"attention"[s]?/{BN}	{ 
  				          endArgumentList();
  					  if (!inAttentionBlock)
					  {
					    if (inBlock()) endBlock();
					    inAttentionBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trAttention()+": ");
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
<DocScan>{CMD}"since"/{BN}		{
  				          endArgumentList();
  					  if (!inSinceBlock)
					  {
					    if (inBlock()) endBlock();
					    inSinceBlock=TRUE;
  					    outDoc->startDescList();
					    outDoc->startBold();
					    scanString(theTranslator->trSince()+": ");
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
<DocScan>"\\todo "[0-9]+		{ // this tag is generated in an earlier pass
  					  if (Config::generateTodoList)
					  {
					    QCString numStr=yytext;
					    numStr=numStr.right(numStr.length()-6);
					    bool ok; int num = numStr.toUInt(&ok);
					    RefItem *item = todoList.getRefItem(num);
					    ASSERT(item!=0);
					    endArgumentList();
					    if (inBlock()) endBlock();
					    outDoc->startDescList();
					    outDoc->startBold();
					    outDoc->writeObjectLink(0,"todo",item->listAnchor,theTranslator->trTodo()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					    internalParseDocument(item->text);
					    outDoc->endDescList();
					  }
  					}
<DocScan>"\\test "[0-9]+		{ // this tag is generated in an earlier pass
  				          if (Config::generateTestList)
					  {
  					    QCString numStr=yytext;
					    numStr=numStr.right(numStr.length()-6);
					    bool ok; int num = numStr.toUInt(&ok);
					    RefItem *item = testList.getRefItem(num);
					    ASSERT(item!=0);
  					    endArgumentList();
					    if (inBlock()) endBlock();
  					    outDoc->startDescList();
					    outDoc->startBold();
					    outDoc->writeObjectLink(0,"test",item->listAnchor,theTranslator->trTest()+": ");
					    outDoc->endBold();
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					    internalParseDocument(item->text);
					    outDoc->endDescList();
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
<DocScan>"$"[a-zA-Z_0-9]+":"[^\n\$]+"$"	{ // RCS tag
  					  QCString tagName(&yytext[1]);
					  int i=tagName.find(':');
					  tagName=tagName.left(i);
					  QCString tagText=&yytext[i+2];
					  tagText=tagText.left(tagText.length()-1);
  				          endArgumentList();
					  if (inBlock()) endBlock();
  					  outDoc->startDescList();
					  outDoc->startBold();
					  scanString(tagName+": ");
					  outDoc->endBold();
					  outDoc->endDescTitle();
					  outDoc->writeDescItem();
					  scanString(tagText);
					  outDoc->endDescList();
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"param"/{BN}	{
					  QCString t=yytext;
  					  if (t.contains('\n')>1 && insideItemList)
					  {
					    forceEndItemList();
					  }
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"retval"/{BN}		{
					  QCString t=yytext;
  					  if (t.contains('\n')>1 && insideItemList)
					  {
					    forceEndItemList();
					  }
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}("exception"|"throw")s?/{BN}	{
					  QCString t=yytext;
  					  if (t.contains('\n')>1 && insideItemList)
					  {
					    forceEndItemList();
					  }
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
  					  outDoc->docify(substitute(yytext,"\"","")); 
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
<DocScan>{CMD}"section "{ID}"\n"	{
  					  QCString secName=&yytext[9];               // skip "\section "
					  secName=secName.left(secName.length()-1); // remove \n
					  //printf("SectionName %s found\n",secName.data());
					  SectionInfo *sec;
					  if ((sec=sectionDict[secName]))
					  {
					    //printf("Title %s\n",sec->title.data());
					    outDoc->startSection(sec->label,sec->title,
						                 sec->type==SectionInfo::Subsection);
					    scanString(sec->title);
					    outDoc->endSection(sec->label,
						               sec->type==SectionInfo::Subsection);
					  }
  					}
<DocScan>{CMD}"anchor "{ID}"\n"		{
  					  QCString secName=&yytext[8];
					  secName=secName.left(secName.length()-1);
					  SectionInfo *sec;
					  if ((sec=sectionDict[secName]))
					  {
					    //printf("writeAnchor %s_%s\n",sec->fileName.data(),sec->label.data());
					    outDoc->writeAnchor(sec->fileName,sec->label);
					  }
  					}
<DocScan>"\\_internalref"{B}+		{ // for internal use only!
					  internalRefFile.resize(0);
					  internalRefAnchor.resize(0);
  					  BEGIN(DocInternalRef);
  					}
<DocInternalRef>[A-Z_a-z0-9.:\-\+]+	{
  					  internalRefFile=yytext;
					  int i = internalRefFile.find(':');
					  if (i!=-1)
					  {
					    internalRefAnchor=internalRefFile.right(internalRefFile.length()-i-1);
					    internalRefFile=internalRefFile.left(i);
					  }
					  //printf("InternalRef yytext=`%s' file=`%s' anchor=`%s'\n",yytext,internalRefFile.data(),internalRefAnchor.data());
  					  BEGIN(DocInternalRefText);
  					}
<DocInternalRefText>\"[^\n\"]*\" 	{
					  QCString text=substitute(yytext,"\"",""); 
  					  outDoc->writeObjectLink(0,internalRefFile,internalRefAnchor,text);
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"ref"/{BN}		{
  					  BEGIN(DocRefName);
  					}
<DocScan>{CMD}"refitem"/{BN}		{
  					  BEGIN(DocRefItem);
  					}
<DocScan>{CMD}"if"/{BN}			{
  					  outDoc->pushGeneratorState();
					  depthIf++;
					  BEGIN(DocIf);
  					}
<DocScan>{CMD}"endif"/[^a-z_A-Z0-9]	{
					  if (--depthIf<0)
					  {
					    warn(yyFileName,yyLineNr,
                                                 "Warning: documentation block contains \\endif without "
						 "matching \\if found in documentation of this entity."
                                                );
					  }
					  else
					  {
  					    outDoc->popGeneratorState();
					  }
  					}
<DocIf>[^\n\t ]+			{
  					  if (Config::sectionFilterList.find(yytext)==-1)
					  {
					    outDoc->disableAll();
					  }
					  BEGIN(DocScan);
  					}
<DocRefName>{SCOPENAME}|{FILE}		{
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
					      outDoc->writeObjectLink(0,sec->fileName,sec->label,text);
					      writePageRef(*outDoc,sec->label,0);
					    }
					    else
					    {
					      //printf("  ref sec=%p sec->fileName=%s text=%s\n",sec,sec->fileName.data(),text.data());
					      outDoc->writeSectionRef(sec->fileName,sec->label,text);
					    }
					  }
					  else if (!generateLink(*outDoc,className,yytext,TRUE,0))
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",yytext);
					    //outDoc->writeBoldString(" unknown reference! ");
					  }
					  BEGIN(DocScan);
  					}
<DocRefName>({SCOPENAME}|{FILE}){B}+/"\""	{
  					  sectionRef=yytext;
					  sectionRef=sectionRef.stripWhiteSpace();
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
					      //outDoc->writeSectionRefAnchor(sec->fileName,sec->label,text);
					      outDoc->writeObjectLink(0,sec->fileName,sec->label,text);
					      //printf("Writing page ref `%s'\n",sec->label.data());
					      writePageRef(*outDoc,sec->label,0);
					    }
					    else
					    {
					      outDoc->writeSectionRef(sec->fileName,sec->label,text);
					    }
					  }
					  else if (!generateLink(*outDoc,className,sectionRef,TRUE,text))
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",sectionRef.data());
					    outDoc->writeBoldString(" unknown reference! ");
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
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",sectionRef.data());
					    outDoc->writeBoldString(" unknown reference! ");
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
					  curImageCaption.resize(0);
					  if (curImageName.isEmpty())
					  {
					    BEGIN(DocScan);
					  }
					  else
					  {
					    BEGIN(DocHtmlImageOpt);
					  }
					}
<DocHtmlImageOpt>\n			{ 
  					  writeImage(IT_Html,0);
					  BEGIN(DocScan);
  					}
<DocHtmlImageOpt>\"[^\n"]*\"		{
					  curImageCaption=stripQuotes(yytext);
					}
<DocLatexImageName>{FILE}		{
  					  curImageName = findAndCopyImage(stripQuotes(yytext),IT_Latex);
					  curImageCaption.resize(0);
					  if (curImageName.isEmpty())
					    BEGIN(DocScan);
					  else
					    BEGIN(DocLatexImageOpt);
  					}
<DocLatexImageOpt>\n			{ // no width specified
  					  writeImage(IT_Latex,0);
					  BEGIN(DocScan);
  					}
<DocLatexImageOpt>\"[^\n"]*\"		{
					  curImageCaption=stripQuotes(yytext);
					}
<DocLatexImageOpt>("width"{B}*"="{B}*)(([0-9\.]+({B}*{ID})?)|("\\"{ID})) {
  					  writeImage(IT_Latex,yytext);
					  BEGIN(DocScan);
  					}
<DocLatexImageOpt>("height"{B}*"="{B}*)(([0-9\.]+({B}*{ID})?)|("\\"{ID}))	{
  					  writeImage(IT_Latex,yytext);
					  BEGIN(DocScan);
  					}
<DocImage>[a-z_A-Z0-9\.\-]+		{
					  warn(yyFileName,yyLineNr,"Warning: %s is an unsupported output format for \\image in the documentation of the entity",yytext);
  					}
<DocImage,DocHtmlImageName,DocLatexImageName>\n {
  					  warn(yyFileName,yyLineNr,"Warning: invalid \\image command found in the documentation of this entity!");
					  outDoc->enableAll();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"code"({BN}*"\n"|{B}*) 	{ 
					  outDoc->startCodeFragment();
  					  codeBlock.resize(0);
					  BEGIN( DocCodeBlock ); 
					}
<DocScan>{CMD}"endcode"/[^a-z_A-Z0-9] 	{ 
  					  warn(yyFileName,yyLineNr,"Warning: \\endcode without \\code "
    					       "in the documentation of this entity."); 
					}
					
<DocScan,DocRefName>{ID}"<"[^>\ \t\n]*">"("::"{ID})+"("?[a-z_A-Z0-9,:\<\> \t\*\&]*")"?	{
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  BEGIN(DocScan);
  					}
<DocScan,DocRefName>{SCOPEMASK}"("[a-z_A-Z0-9,:\<\> \t\*\&]+")" {
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  BEGIN(DocScan);
  					}
<DocScan,DocRefName>{SCOPEMASK}("()")?	{
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  BEGIN(DocScan);
  					}
<DocScan,DocRefName>({SCOPEMASK}"::")?"operator()("[a-z_A-Z0-9,\<\> \t\*\&]*")"	{ 
				          QCString oName=yytext;
					  generateRef(*outDoc,className,
					              removeRedundantWhiteSpace(oName),inSeeBlock);
					  BEGIN(DocScan);
					}
<DocScan,DocRefName>({SCOPEMASK}"::")?"operator"[^(\r\n.,]+"("[a-z_A-Z0-9,\<\> \t\*\&]*")"	{ 
    					  QCString oName=yytext;
					  generateRef(*outDoc,className,
					              removeRedundantWhiteSpace(oName),inSeeBlock);
					  BEGIN(DocScan);
					}
<DocScan>("http:"|"https:"|"ftp:"|"file:"){URLMASK}	{ outDoc->writeHtmlLink(yytext,yytext); }
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
<DocScan>{CMD}("e"|"em"|"a"){BN}+       { BEGIN( DocEmphasis ); }
<DocScan>{CMD}"b"{BN}+			{ BEGIN( DocBold ); }
<DocScan>{CMD}("c"|"p"){BN}+            { BEGIN( DocCode ); }
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
<DocScan>"<"{TABLE}{ATTR}">"		{ startTable(); }
<DocScan>"</"{TABLE}{ATTR}">"		{ endTable(); }
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
<DocScan>"<"{PRE}{ATTR}">"		{ 
  					  if (insidePre)
					  {
					    warn(yyFileName,yyLineNr,"Warning in the documentation of this entity:\nNested <pre> found in the documentation of this entity!");
					  }
  					  outDoc->startPreFragment(); 
					  insidePre=TRUE;
					}
<DocScan>"</"{PRE}{ATTR}">"	        { 
  					  outDoc->endPreFragment(); 
					  insidePre=FALSE;
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
  					  currentListIndent.push("O");
					}
<DocScan>"</"{OL}{ATTR}">"		{ 
					  if (currentListIndent.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
                                                 "Warning in the documentation of this entity:\nMore </ol> tags than <ol> tags in the documentation of this entity."
                                                );
					  }
					  else if (currentListIndent.top()!="O")
					  {
					    warn(yyFileName,yyLineNr,
						 "Warning in the documentation of this entity:\nThe </ol> tag does not end a <ol> tag."
						);
					  }
					  else
					  {
  				            outDoc->endEnumList(); 
  					    currentListIndent.pop();
					  }
					}
<DocScan>"<"{UL}{ATTR}">"		{ outDoc->startItemList(); 
					  currentListIndent.push("U");
					}
<DocScan>"</"{UL}{ATTR}">"		{ 
					  if (currentListIndent.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
                                                 "Warning in the documentation of this entity:\nMore </ul> tags than <ul> tags."
                                                );
					  }
					  else if (currentListIndent.top()!="U")
					  {
					    warn(yyFileName,yyLineNr,
						 "Warning in the documentation of this entity:\nThe </ul> tag does not end a <ul> tag."
						);
					  }
					  else
					  {
                                            outDoc->endItemList(); 
  					    currentListIndent.pop();
					  }
					}
<DocScan>"<"{LI}{ATTR}">"		{
  					  if (/*currentListIndent.isEmpty() ||*/  //DvH: I removed this check because I use this in the manual (the <ul> is in a \htmlonly block!)
					      currentListIndent.top()=="D")
					  {
 					    warn(yyFileName,yyLineNr,
					         "Warning in the documentation of this entity:\nThe <li> tag can only be used inside a <ul> ... </ul> or a <ol> ... </ol> block."
					        );
					  }
					  else
					  {
  					    outDoc->writeListItem(); 
					  }
					}
<DocScan>"</"{LI}{ATTR}">"		
<DocScan>"<"{TT}{ATTR}">"		{ outDoc->startTypewriter(); }
<DocScan>"</"{TT}{ATTR}">"		{ outDoc->endTypewriter(); }
<DocScan>"<"{EM}{ATTR}">"		{ outDoc->startEmphasis(); }
<DocScan>"</"{EM}{ATTR}">"		{ outDoc->endEmphasis(); }
<DocScan>"<"{HR}{ATTR}">"		{ outDoc->writeRuler(); }
<DocScan>"<"{DL}{ATTR}">"		{ outDoc->startDescription(); 
					  currentListIndent.push("D");
					}
<DocScan>"</"{DL}{ATTR}">"		{ 
					  if (currentListIndent.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
                                                 "Warning in the documentation of this entity:\nMore </dl> tags than <dl> tags in the documentation."
                                                );
					  }
					  else if (currentListIndent.top()!="D")
					  {
					    warn(yyFileName,yyLineNr,
						 "Warning in the documentation of this entity:\nThe </dl> tag does not end a <dl> tag in the documentation."
						);
					  }
					  else
					  {
                                            outDoc->endDescription(); 
  					    currentListIndent.pop();
					  }
					}
<DocScan>"<"{DT}{ATTR}">"		{ 
  					  if (currentListIndent.isEmpty() ||
					      currentListIndent.top()!="D")
					  {
 					    warn(yyFileName,yyLineNr,
					         "Warning in the documentation of this entity:\nThe <dt> tag can only be used inside a <dl> ... </dl> block."
					        );
					  }
					  else
					  {
  					    outDoc->startDescItem(); 
					  }
					}
<DocScan>"</"{DT}{ATTR}">"	
<DocScan>"<"{DD}{ATTR}">"		{ 
  					  if (currentListIndent.isEmpty() ||
					      currentListIndent.top()!="D")
					  {
 					    warn(yyFileName,yyLineNr,
					         "Warning in the documentation of this entity:\nThe <dd> tag can only be used inside a <dl> ... </dl> block."
					        );
					  }
					  else
					  {
  					    outDoc->endDescItem(); 
					  }
					}
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
<DocHtmlAnchor>[a-z_A-Z0-9.\-\+\/]+	{ outDoc->writeAnchor(0,yytext); } 
<DocHtmlLink>{HREF}{BN}*"="{BN}*("\""?)	{ 
  					  htmlUrl.resize(0);
					  htmlText.resize(0);
  					  BEGIN(DocHtmlHref1); }
<DocHtmlHref1>{URLMASK}			{ 
  					  htmlUrl=yytext; 
					}
<DocHtmlHref1>">"			{ BEGIN(DocHtmlHref2); }
<DocHtmlHref2>[^<]*			{ htmlText+=yytext; }
<DocHtmlHref2>"<"			{ 
  					  outDoc->writeHtmlLink(htmlUrl,htmlText);
  					  unput(*yytext); 
					  BEGIN(DocScan); 
					}
<DocHtmlLink,DocHtmlAnchor>">"		{ BEGIN(DocScan); }
<DocScan>{CMD}("\\"|"@"|"<"|">"|"&"|"$"|"#"|"%") {
  					  outDoc->docify(&yytext[1]);
  					}
<DocScan,DocEmphasis,DocBold,DocCode>"%"[a-zA-Z_0-9\-]+		{
  					  outDoc->docify(yytext+1);
  					}
<DocEmphasis>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"("[a-z_A-Z0-9,:\<\> \t\*\&]*")" {
  					  outDoc->startEmphasis();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
  					}
<DocEmphasis>{WORD}			{ 
  					  outDoc->startEmphasis();
  					  linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,FALSE);
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
					}
<DocBold>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"("[a-z_A-Z0-9,:\<\> \t\*\&]*")" {
  					  outDoc->startBold();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endBold();
					  BEGIN( DocScan );
  					}
<DocBold>{WORD}				{ 
  					  outDoc->startBold();
  					  linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,FALSE);
					  outDoc->endBold();
					  BEGIN( DocScan );
					}
<DocCode>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()!\*/]*"("[a-z_A-Z0-9,:\<\> \t\*\&]*")" {
  					  outDoc->startTypewriter();
  					  generateRef(*outDoc,className,yytext,inSeeBlock);
					  outDoc->endTypewriter();
					  BEGIN( DocScan );
  					}
<DocCode>{WORD}				{ 
  					  outDoc->startTypewriter();
  					  linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,FALSE);
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
<DocScan>({B}*"\n"){2,}{B}*"*"*{B}*"-"("#")?{B}+	{ // new paragraph & start of a list
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  bool isEnumerated = text.at(dashPos+1)=='#';
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
  					  addListItemMarker(yytext,dashPos+1,isEnumerated);
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
					    if (insidePre)
					    {
					      outDoc->docify(yytext);
					    }
					    else
					    {
					      outDoc->newParagraph(); 
					    }
					  }
					  if (inBlock()) endBlock();
					}
<DocScan>{BN}+/\n			{ 
  					  outDoc->writeChar(' '); 
					}
<DocScan>\n?{B}*			{
  					  if (insidePre)
					  {
					    outDoc->docify(yytext);
					  }
					  else
					  {
  					    outDoc->writeChar(' '); 
					  }
  					}
<DocScan>"\\"[a-z_A-Z][a-z_A-Z0-9]*[\\] { // directory type of text
  					  outDoc->docify(yytext);
  					}
<DocScan>{CMD}[a-z_A-Z][a-z_A-Z0-9]* 	{
  					  QCString *pValue=aliasDict[yytext+1];
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
  					    warn(yyFileName,yyLineNr,"Warning in the documentation of this entity:\nUnknown command `%s' found.",yytext);
					  }
  					}
<DocCode,DocEmphasis,DocBold,DocScan,Text>[a-z_A-Z0-9]+ { 
					  outDoc->docify(yytext);
					}
<DocCode,DocEmphasis,DocBold,DocScan,Text>.  { 
					  outDoc->writeChar(*yytext);
					}
<*>\n					{ yyLineNr++ ; }
<*>.

%%

//----------------------------------------------------------------------------

void scanString(const char *s)
{
  const char *oldInputString = inputString;
  int oldInputPosition = inputPosition;
  int oldRule = YY_START;
  YY_BUFFER_STATE oldBuffer = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(docYYin, YY_BUF_SIZE));  
  inputString  = s;
  inputPosition = 0;
  BEGIN( Text );
  docYYlex();
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(oldBuffer);
  inputString = oldInputString;
  inputPosition = oldInputPosition;
  BEGIN( oldRule );
}

void internalParseDocument(const char *s)
{
  const char *oldInputString = inputString;
  int oldInputPosition = inputPosition;
  int oldRule = YY_START;
  YY_BUFFER_STATE oldBuffer = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(docYYin, YY_BUF_SIZE));  
  inputString  = s;
  inputPosition = 0;
  BEGIN( DocScan );
  docYYlex();
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(oldBuffer);
  inputString = oldInputString;
  inputPosition = oldInputPosition;
  BEGIN( oldRule );
}

//----------------------------------------------------------------------------

void parseDocument(OutputList &ol,const QCString &docString)
{
  //inParamBlock=inSeeBlock=inReturnBlock=FALSE;
  curTable = 0;
  depthIf = 0;
  outDoc = new OutputList(&ol);
  currentIncludeFile.resize(0);
  includeFileOffset=0;
  includeFileLength=0;
  currentListIndent.clear();
  listIndentStack.clear();
  if (!docString) return;
  linkRef  = "";
  linkText = "";
  inputString = docString;
  inputPosition = 0;
  docYYrestart( docYYin ); 
  BEGIN( DocScan );
  insideArgumentList = FALSE;
  insideVerbatim = FALSE;
  docYYlex();
  if (insideArgumentList) { insideArgumentList=FALSE; outDoc->endItemList(); }
  if (insideItemList) { forceEndItemList(); }
  if (inBlock()) endBlock();
  if (!currentListIndent.isEmpty())
  {
    warn(yyFileName,yyLineNr,"Warning: Documentation ended in the middle " 
	   "of a list!");
    warn_cont("Missing: ");
    while (!currentListIndent.isEmpty())
    {
      char c;
      switch((c=*currentListIndent.pop()))
      {
	case 'O': warn_cont("</ol>"); break;
	case 'U': warn_cont("</ul>"); break;
	case 'D': warn_cont("</dl>"); break;
      }
    }
    warn_cont("\n");
  }
  if (depthIf!=0)
  {
    warn(yyFileName,yyLineNr,"Warning: Documentation block contains \\if "
         "without matching \\endif: nesting level is %d",depthIf);
  }
  if (!tableStack.isEmpty())
  {
    forceEndTable();
  }
  if (insideVerbatim)
  {
    warn(yyFileName,yyLineNr,
         "Warning: file ended inside a \\verbatim block!"
        );
  }
  ol+=*outDoc;
  delete outDoc; outDoc=0;
  return;
}

//----------------------------------------------------------------------------

void parseDoc(OutputList &ol,const char *fileName,int startLine,
              const char *clName,const char *memName,const QCString &docString)
{
  //printf("parseDoc(file=`%s',line=%d)\n",fileName,startLine);
  initParser();
  initParseCodeContext();
  exampleDoc=FALSE;           // do not cross reference with member docs
  className=clName;
  memberName=memName;
  strcpy(yyFileName,fileName);
  yyLineNr = startLine;
  parseDocument(ol,docString);

}

//----------------------------------------------------------------------------

void parseText(OutputList &ol,const QCString &txtString)
{
  if (txtString.isEmpty()) return;
  inputString = txtString;
  outDoc = new OutputList(&ol);
  inputPosition = 0;
  docYYrestart( docYYin ); 
  BEGIN( Text );
  docYYlex();
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
  strcpy(yyFileName,fileName);
  parseDocument(ol,docString); 
}

//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void docYYdummy() { yy_flex_realloc(0,0); } 
}
