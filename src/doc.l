/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
//#include <iostream.h>
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
static OutputDocInterface * outDoc;
static bool             insideArgumentList;
static QCString         className;
static QCString         memberName;
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
static bool             insideHtmlLink;
static QCString         sectionRef;
static bool             insideVerbatim = FALSE;
static bool             insidePre = FALSE;
static int              depthIf;
static QCString         curImageName;
static QCString         curImageCaption;
static QCString         curDotFileName;
static QCString         curDotFileCaption;
static QCString         internalRefFile;
static QCString         internalRefAnchor;
static QStack<char>	  currentListIndent; // indent stack of all list items 
static bool insideItemList = FALSE;

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
  insideHtmlLink = FALSE;
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
    OutputDocInterface *outputDocInterface() { return od; }
   
  private:
    OutputDocInterface *od;
    int row;
    int col;
};

TableElem::TableElem(int r,int c)
{
  //printf("TableElem::TableElem(%d,%d)\n",r,c);
  od=outDoc->clone();
  outDoc=od;
  row=r;
  col=c;
}

TableElem::~TableElem()
{
  //printf("TableElem::~TableElem(%d,%d)\n",row,col);
  delete od; od=0;
}

class Table
{
  public:
    Table();
   ~Table();
    void newRow();
    void newElem();
    
  private:
    OutputDocInterface *parentDoc;
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
	parentDoc->append(e->outputDocInterface());
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

static void includeFile(OutputDocInterface &od,const char *fileName,bool quiet)
{
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(Doxygen::exampleNameDict,fileName,ambig)))
  {
    currentIncludeFile=fileToString(fd->absFilePath());
    includeFileOffset=0;
    includeFileLength=currentIncludeFile.length();
    OutputDocInterface *codeFrag = od.clone();
    parseCode(*codeFrag,0,currentIncludeFile,exampleDoc,exampleName);
    if (!quiet)
    {
      od.startCodeFragment();
      od.append(codeFrag);
      od.endCodeFragment();
    }
    delete codeFrag;
  }
  else if (ambig)
  {
    QCString text;
    text.sprintf("Include file name %s is ambigious.\n",fileName);
    text+="Possible candidates:\n";
    text+=showFileDefMatches(Doxygen::exampleNameDict,fileName);
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

static void verbIncludeFile(OutputDocInterface &od,const char *name)
{
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(Doxygen::exampleNameDict,name,ambig)))
  {
    od.startCodeFragment();
    od.codify(fileToString(fd->absFilePath()));
    od.endCodeFragment();
  }
  else if (ambig)
  {
    QCString text;
    text.sprintf("Include file name %s is ambigious.\n",name);
    text+=("Possible candidates:\n");
    text+=showFileDefMatches(Doxygen::exampleNameDict,name);
    warn(yyFileName,yyLineNr,text);
  }
  else
  {
    warn(yyFileName,yyLineNr,
         "Warning: example file %s is not found. "
         "Check your EXAMPLE_PATH",name);
  }
}

static void rawIncludeFile(OutputDocInterface &od,const char *name)
{
  bool ambig;
  FileDef *fd;
  if ((fd=findFileDef(Doxygen::exampleNameDict,name,ambig)))
  {
    od.writeString(fileToString(fd->absFilePath()));
  }
  else if (ambig)
  {
    QCString text;
    text.sprintf("Include file name %s is ambigious.\n",name);
    text+=("Possible candidates:\n");
    text+=showFileDefMatches(Doxygen::exampleNameDict,name);
    warn(yyFileName,yyLineNr,text);
  }
  else
  {
    warn(yyFileName,yyLineNr,
	"Warning: include file %s is not found. "
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

static void skipLine(OutputDocInterface &od,const char *key)
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
      od.writeString("    ");
      parseCode(od,className,s,exampleDoc,exampleName);
      od.writeString("\n");
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

static void showLine(OutputDocInterface &od,const char *key)
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
    od.writeString("    ");
    parseCode(od,className,s,exampleDoc,exampleName);
    od.writeString("\n");
  }
}

static void showUntil(OutputDocInterface &od,const char *key)
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
      od.writeString("    ");
      parseCode(od,className,s,exampleDoc,exampleName);
      od.writeString("\n");
      if (s.find(key)!=-1) found=TRUE; 
    }
    if (includeFileOffset==includeFileLength) found=TRUE;
  }
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
      outDoc->endParamList();
    }
    else
    {
      outDoc->endDescList();
    }
    currentListIndent.pop();
    inParamBlock=inRetValBlock=inSeeBlock=inReturnBlock=inAuthorBlock=
    inVersionBlock=inSinceBlock=inDateBlock=inBugBlock=inNoteBlock=inWarningBlock=
    inParBlock=inExceptionBlock=inDeprecatedBlock=inPreBlock=inPostBlock=
    inInvarBlock=inRemarkBlock=inAttentionBlock=FALSE;
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

static QStack<IndentInfo> listIndentStack;   // indent stack of - items

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
      indent+=Config_getInt("TAB_SIZE") - (indent%Config_getInt("TAB_SIZE"));
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
    currentListIndent.push(enumerated ? "O" : "U");
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
      currentListIndent.push(enumerated ? "O" : "U");
      listIndentStack.push(new IndentInfo(indent,enumerated));
      listIndentStack.top()->startList();
      listIndentStack.top()->writeItem();
    }
    else // end sub item list
    {
      while (pPrevInfo && pPrevInfo->indent>indent)
      {
	pPrevInfo->endList();
	listIndentStack.pop();
	currentListIndent.pop();
	delete pPrevInfo;
	pPrevInfo = listIndentStack.top();
      }
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
    delete info;
  }
  while (!currentListIndent.isEmpty())
  {
    char c=*currentListIndent.pop();
    switch(c)
    {
       case 'O': outDoc->endEnumList(); break;
       case 'U': outDoc->endItemList(); break;
       case 'D':
	if (inBlock()) 
	{
	  currentListIndent.push("D"); // hack!
	  endBlock(); 
	}
	else 
	  outDoc->endDescription(); 
	break;
    }
  }
  insideItemList=FALSE;
}

static void endArgumentList()
{
#if 0
  IndentInfo *info;
  while ((info=listIndentStack.pop())!=0)
  {
    delete info;
  }
  while (!currentListIndent.isEmpty())
  {
    char c=*currentListIndent.pop();
    switch(c)
    {
       case 'O': outDoc->endEnumList(); break;
       case 'U': outDoc->endItemList(); break;
       case 'D':
	if (!inBlock()) outDoc->endDescription(); 
	break;
    }
  }
  insideItemList=FALSE;
#endif
  if (insideArgumentList)
  {
    insideArgumentList=FALSE;
    outDoc->endItemList();
  }
}

//-----------------------------------------------------------------

enum ImageTypes
{
  IT_Html,
  IT_Latex,
  IT_RTF
};

/*! search for an image in the imageNameDict and if found
 * copies the image to the output directory (which is the
 * html directory if type==0 or the latex directory if type==1)
 */
static QCString findAndCopyImage(const char *fileName,ImageTypes type)
{
  QCString result;
  bool ambig;
  FileDef *fd;
  //printf("Search for %s\n",fileName);
  if ((fd=findFileDef(Doxygen::imageNameDict,fileName,ambig)))
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
	  outputDir = Config_getString("HTML_OUTPUT");
	  break;
	case IT_Latex: 
	  outputDir = Config_getString("LATEX_OUTPUT");
	  break;
	case IT_RTF:
	  outputDir = Config_getString("RTF_OUTPUT");
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
    text+=showFileDefMatches(Doxygen::imageNameDict,fileName);
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
    case IT_RTF:
      {
	outDoc->disableAllBut(OutputGenerator::RTF);
	outDoc->startImage(curImageName,0,hasCaption);
	if (hasCaption)
	{
	  scanString(curImageCaption);
	}
	outDoc->endImage(hasCaption);
      }
  }
  outDoc->popGeneratorState();
}

// search for a dot file in the dotFileNameDict, and if found
// generates the graph in the output directories.
static void writeDotFile(const char *fileName, const char *captionText)
{
  bool ambig;
  FileDef *fd;
  bool hasCaption = captionText!=0;

  if ((fd=findFileDef(Doxygen::dotFileNameDict,fileName,ambig)))
  {
    outDoc->startDotFile(fd->absFilePath(),hasCaption);
    if (hasCaption)
    {
       scanString(captionText);
    }
    outDoc->endDotFile(hasCaption);
  }
  else if (ambig)
  {
    QCString text;
    text.sprintf("Warning: dot file name %s is ambigious.\n",fileName);
    text+="Possible candidates:\n";
    text+=showFileDefMatches(Doxygen::dotFileNameDict,fileName);
    warn(yyFileName,yyLineNr,text);
  }
  else
  {
    warn(yyFileName,yyLineNr,
         "Warning: dot file %s is not found in DOTFILE_DIRS! ",fileName
        );
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

//ATTR      ((({BN}+[^\>]+)/">")?)
%}

CMD	  ("\\"|"@")
BN        [ \t\n\r]
BL        [ \t\r]*"\n"
BSEP      [ \t\r]*([ \t\r]|"\n")({BL}{0,100})
B         [ \t]
BS        ^(({B}*"//")?)(({B}*"*"+)?){B}*
FILESCHAR [a-z_A-Z0-9\\:\\\/\-\+]
FILEECHAR [a-z_A-Z0-9\-\+]
FILEMASK  {FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)+
FILE      ({FILESCHAR}*{FILEECHAR}+("."{FILESCHAR}*{FILEECHAR}+)*)|("\""[^\n\"]+"\"")
ID        [a-z_A-Z][a-z_A-Z0-9]*
SCOPENAME (({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
SCOPEMASK {ID}?(("::"|"#")?(~)?{ID})+
URLMASK   [a-z_A-Z0-9\~\:\?\@\&\%\#\.\-\+\/\=]+
NONTERM   [\{\}\[\]\`\~\@\|\-\+\#\$\/\\\!\%\^\&\*()a-z_A-Z<>0-9\x80-\xff]
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
KBD	  [kK][bB][dD]
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
BLOCKQUOTE [bB][lL][oO][cC][kK][qQ][uU][oO][tT][eE]
DOCPARAM  ([a-z_A-Z0-9:\<\>\=\.\-]+)|("\"".*"\"")
OPNEW     {B}+"new"({B}*"[]")?
OPDEL     {B}+"delete"({B}*"[]")?
OPARG     "("[a-z_A-Z0-9,\<\> \t\*\&]*")"
OPNORM    {OPNEW}|{OPDEL}|"+"|"-"|"*"|"/"|"%"|"^"|"&"|"|"|"~"|"!"|"="|"<"|">"|"+="|"-="|"*="|"/="|"%="|"^="|"&="|"|="|"<<"|">>"|"<<="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|","|"->*"|"->"|"[]"|"()"
OPCAST    {B}+[^(\r\n.,]+
OPMASK    ({B}*{OPNORM}({OPARG}?))|({OPCAST}{OPARG})

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
%x      DocCodeBlock
%x      DocInternal
%x      DocLink
%x      DocJavaLink
%x      DocLinkText
%x      DocJavaLinkText
%x      DocSkipWord
%x      DocInclude
%x      DocDontInclude
%x      DocHtmlLink
%x      DocHtmlAnchor
%x      DocHtmlHref
%x	DocSkiplineKey
%x	DocSkipKey
%x	DocLineKey
%x	DocUntilKey
%x	DocKeyEnd
%x	DocPar
%x	DocRefName
%x	DocVerbatim
%x	DocVerbInc
%x	DocHtmlInc
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
%x	DocRtfImageName
%x	DocRtfImageOpt
%x	DocDotFile
%x	DocDotFileOpt

%%

<*>\x0d
<DocScan>^{B}*(("//"{B}*)?)"*"*{B}*"-"("#")?{B}+ { /* found list item marker */
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  //printf("dashPos=%d char='%c'\n",dashPos,text.at(dashPos+1));
					  bool isEnumerated = text.at(dashPos+1)=='#';
  					  addListItemMarker(yytext,dashPos+1,isEnumerated);
  					}
<DocScan>\n{B}*(("//"{B}*)?)"*"*{B}*"-"("#")?{B}+ { /* found list item marker */
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  //printf("dashPos=%d char='%c'\n",dashPos,text.at(dashPos+1));
					  bool isEnumerated = text.at(dashPos+1)=='#';
  					  addListItemMarker(yytext+1,dashPos,isEnumerated);
  					}
<DocScan,Text>"&copy;"			{ outDoc->writeCopyright(); }
<DocScan,Text>"&lt;"			{ outDoc->docify("<"); }
<DocScan,Text>"&gt;"			{ outDoc->docify(">"); }
<DocScan,Text>"&amp;"			{ outDoc->docify("&"); }
<DocScan,Text>"&apos;"			{ outDoc->docify("'"); }
<DocScan,Text>"&quot;"			{ outDoc->docify("\""); }
<DocScan,Text>"&"[AEIOUYaeiouy]"uml;"	{ outDoc->writeUmlaut(yytext[1]); }
<DocScan,Text>"&"[AEIOUYaeiouy]"acute;"	{ outDoc->writeAcute(yytext[1]);  }
<DocScan,Text>"&"[AEIOUaeiou]"grave;"	{ outDoc->writeGrave(yytext[1]);  }
<DocScan,Text>"&"[AEIOUaeiou]"circ;"	{ outDoc->writeCirc(yytext[1]);   }
<DocScan,Text>"&"[ANOano]"tilde;"	{ outDoc->writeTilde(yytext[1]);  }
<DocScan,Text>"&szlig;"			{ outDoc->writeSharpS();          }
<DocScan,Text>"&"[cC]"cedil;"		{ outDoc->writeCCedil(yytext[1]); }
<DocScan,Text>"&"[aA]"ring;"		{ outDoc->writeRing(yytext[1]);   }
<DocScan,Text>"&nbsp;"			{ outDoc->writeNonBreakableSpace(1); }
<DocScan,DocHtmlScan,DocLatexScan>"$("[a-z_A-Z]+")"	{ 
  					  QCString envvar=&yytext[2];
  					  envvar=envvar.left(envvar.length()-1);
					  outDoc->docify(getenv(envvar));
					}
<DocScan>{CMD}"htmlonly"/[^a-z_A-Z0-9]	{
  				          outDoc->pushGeneratorState(); 
  					  outDoc->disableAllBut(OutputGenerator::Html); 
					  BEGIN(DocHtmlScan);
  					}
<DocHtmlScan>{CMD}"endhtmlonly"/[^a-z_A-Z0-9] {
  					  outDoc->popGeneratorState();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"latexonly"/[^a-z_A-Z0-9]	{
  					  outDoc->pushGeneratorState();
  					  outDoc->disableAllBut(OutputGenerator::Latex);
					  BEGIN(DocLatexScan);
  					}
<DocLatexScan>{CMD}"endlatexonly"/[^a-z_A-Z0-9] {
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
<DocScan>"\\functionindex"/{BN}		{ /* writeMemberList(*outDoc,FALSE);*/ }
<DocScan>"\\classhierarchy"/{BN}	{ /* writeClassHierarchy(*outDoc); */ }
<DocScan>"\\annotatedclasslist"/{BN}	{ /* writeAnnotatedClassList(*outDoc); */ }
<DocScan>"\\headerfilelist"/{BN}	{ /* writeHeaderFileList(*outDoc); */ }
<DocScan>"\\header"/{BN}                { BEGIN( DocSkipWord ); }
<DocScan>"\\define"/{BN}	        { BEGIN( DocSkipWord ); }
<DocScan>{CMD}"verbinclude"/{BN}	{ BEGIN( DocVerbInc ); }
<DocVerbInc>{FILE}			{
  					  verbIncludeFile(*outDoc,stripQuotes(yytext));
					  BEGIN( DocScan ); 
					}
<DocScan>{CMD}"htmlinclude"/{BN}	{ BEGIN( DocHtmlInc ); }
<DocHtmlInc>{FILE}			{
    					  outDoc->pushGeneratorState();
    					  outDoc->disableAllBut(OutputGenerator::Html);
  					  rawIncludeFile(*outDoc,stripQuotes(yytext));
    					  outDoc->popGeneratorState();
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
					  if (!Config_getBool("INTERNAL_DOCS"))
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
<DocScan>"{"{CMD}"link"{BN}+	        { BEGIN( DocJavaLink ); }
<DocSkipWord>[a-z_A-Z0-9.:()]+		{ BEGIN( DocScan ); }
<DocLink>[a-z_A-Z0-9:#.,~&*/\[\]<>()\-\+]+  {  // TODO: support operators as well!
  					  linkRef  = stripKnownExtensions(yytext);
					  linkText = "";
  					  BEGIN( DocLinkText ); 
					}
<DocJavaLink>([a-z_A-Z0-9]+".")+	{ /* Skip scope prefix (TODO: fix) */ }
<DocJavaLink>([a-z_A-Z0-9]*"#")?[a-z_A-Z0-9]+("("[a-z_A-Z0-9.,:~&*()\[\]]*")")? {  // TODO: support operators as well!
  					  linkRef  = yytext;
					  linkText = "";
					  BEGIN( DocJavaLinkText );
					}
<DocJavaLinkText>"}"			{
  					  //printf("Trying to link `%s'\n",linkRef.data());
					  if (!generateLink(*outDoc,className,linkRef,inSeeBlock,linkText.stripWhiteSpace()))
					  {
					    warn(yyFileName,yyLineNr,"Warning: link to unknown entity `%s' in the documentation of this entity!",linkRef.data());
					  }
  					  BEGIN( DocScan ); 
  					}
<DocJavaLinkText,DocLinkText>.		{ linkText += *yytext; }
<DocJavaLinkText,DocLinkText>"\n"	{ linkText += " "; }
<DocLink,DocLinkText>{CMD}"endlink" 	{ // <- needed for things like \endlink. 
  					  //printf("GenerateLink className=`%s' linkRef=`%s' linkText=`%s'\n",
					  //        className.data(),linkRef.data(),linkText.data());
					  if (!generateLink(*outDoc,className,linkRef,inSeeBlock,linkText.stripWhiteSpace()))
					  {
					    warn(yyFileName,yyLineNr,"Warning: link to unknown entity `%s' in the documentation of this entity!",linkRef.data());
					  }
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
  					  Formula *formula=Doxygen::formulaNameDict[yytext];
					  if (formula)
					  {
					    QCString formName;
					    formName.sprintf("form-%d.gif",formula->getId());
					    outDoc->writeFormula(formName,formula->getFormulaText());
					  }
  					}
<DocIndexWord>[^\n]+			{
  					  //printf("Adding %s to index\n",yytext);
					  outDoc->addIndexItem(yytext,0);
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
 					  QCString t=yytext;
  					  if (t.contains('\n')>1 && insideItemList)
					  {
					    forceEndItemList();
					  }
  					  BEGIN(DocPar);
  					}
<DocPar>[^\n]*{BSEP}			{
  					  QCString title=QCString(yytext).stripWhiteSpace();
					  bool b = inBlock();
					  if (!title.isEmpty())
					  {
  				            endArgumentList();
					    if (b) endBlock();  
					    inParBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Par);
					    outDoc->docify(title);
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->newParagraph();
					  }
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"warning"{BSEP}		{ 
  				          endArgumentList();
  					  if (!inWarningBlock)
					  {
					    if (inBlock()) endBlock();
					    inWarningBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Warning);
					    scanString(theTranslator->trWarning()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"remark"[s]?{BSEP}	{ 
  				          endArgumentList();
  					  if (!inRemarkBlock)
					  {
					    if (inBlock()) endBlock();
					    inRemarkBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Remark);
					    scanString(theTranslator->trRemarks()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"attention"{BSEP}		{ 
  				          endArgumentList();
  					  if (!inAttentionBlock)
					  {
					    if (inBlock()) endBlock();
					    inAttentionBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Attention);
					    scanString(theTranslator->trAttention()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"note"[s]?{BSEP}		{
  				          endArgumentList();
  					  if (!inNoteBlock)
					  {
					    if (inBlock()) endBlock();
					    inNoteBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Note);
					    scanString(theTranslator->trNote()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"pre"{BSEP}		{
  				          endArgumentList();
  					  if (!inPreBlock)
					  {
					    if (inBlock()) endBlock();
					    inPreBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Pre);
					    scanString(theTranslator->trPrecondition()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"post"{BSEP}		{
  				          endArgumentList();
  					  if (!inPostBlock)
					  {
					    if (inBlock()) endBlock();
					    inPostBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Post);
					    scanString(theTranslator->trPostcondition()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"invariant"{BSEP}		{
  				          endArgumentList();
  					  if (!inInvarBlock)
					  {
					    if (inBlock()) endBlock();
					    inInvarBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Invar);
					    scanString(theTranslator->trInvariant()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"version"{BSEP}		{
  				          endArgumentList();
  					  if (!inVersionBlock)
					  {
					    if (inBlock()) endBlock();
					    inVersionBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Version);
					    scanString(theTranslator->trVersion()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"since"{BSEP}		{
  				          endArgumentList();
  					  if (!inSinceBlock)
					  {
					    if (inBlock()) endBlock();
					    inSinceBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Since);
					    scanString(theTranslator->trSince()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"date"{BSEP}		{
  				          endArgumentList();
  					  if (!inDateBlock)
					  {
					    if (inBlock()) endBlock();
					    inDateBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Date);
					    scanString(theTranslator->trDate()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>"\\todo "[0-9]+		{ // this tag is generated in an earlier pass
  					  if (Config_getBool("GENERATE_TODOLIST"))
					  {
					    QCString numStr=yytext;
					    numStr=numStr.right(numStr.length()-6);
					    bool ok; int num = numStr.toUInt(&ok);
					    RefItem *item = todoList.getRefItem(num);
					    ASSERT(item!=0);
					    endArgumentList();
					    if (inBlock()) endBlock();
					    currentListIndent.push("D");
					    outDoc->startDescList(BaseOutputDocInterface::Todo);
					    outDoc->writeObjectLink(0,"todo",item->listAnchor,theTranslator->trTodo()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					    internalParseDocument(item->text);
					    outDoc->endDescList();
					    currentListIndent.pop();
					  }
  					}
<DocScan>"\\test "[0-9]+		{ // this tag is generated in an earlier pass
  				          if (Config_getBool("GENERATE_TESTLIST"))
					  {
  					    QCString numStr=yytext;
					    numStr=numStr.right(numStr.length()-6);
					    bool ok; int num = numStr.toUInt(&ok);
					    RefItem *item = testList.getRefItem(num);
					    ASSERT(item!=0);
  					    endArgumentList();
					    if (inBlock()) endBlock();
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Test);
					    outDoc->writeObjectLink(0,"test",item->listAnchor,theTranslator->trTest()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					    internalParseDocument(item->text);
					    outDoc->endDescList();
					    currentListIndent.pop();
					  }
  					}
<DocScan>"\\bug "[0-9]+			{ // this tag is generated in an earlier pass
  				          if (Config_getBool("GENERATE_BUGLIST"))
					  {
  					    QCString numStr=yytext;
					    numStr=numStr.right(numStr.length()-5);
					    bool ok; int num = numStr.toUInt(&ok);
					    RefItem *item = bugList.getRefItem(num);
					    ASSERT(item!=0);
  					    endArgumentList();
					    if (inBlock()) endBlock();
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Bug);
					    outDoc->writeObjectLink(0,"bug",item->listAnchor,theTranslator->trBug()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					    internalParseDocument(item->text);
					    outDoc->endDescList();
					    currentListIndent.pop();
					  }
  					}
<DocScan>{CMD}"deprecated"{BSEP}	{
  					  endArgumentList();
					  if (!inDeprecatedBlock)
					  {
					    if (inBlock()) endBlock();
					    inDeprecatedBlock=TRUE;
					    currentListIndent.push("D");
					    outDoc->startDescList(BaseOutputDocInterface::Deprecated);
					    scanString(theTranslator->trDeprecated()+": ");
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
					  currentListIndent.push("D");
  					  outDoc->startDescList(BaseOutputDocInterface::RCS);
					  scanString(tagName+": ");
					  outDoc->endDescTitle();
					  outDoc->writeDescItem();
					  scanString(tagText);
					  outDoc->endDescList();
					  currentListIndent.pop();
  					}
<DocScan>{CMD}"author"[s]?{BSEP}	{
  				          endArgumentList();
  					  if (!inAuthorBlock)
					  {
					    if (inBlock()) endBlock();
					    inAuthorBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Author);
					    bool singular = ((QString)yytext).find('s')==-1;
					    scanString(theTranslator->trAuthor(TRUE,singular)+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->docify(", ");
					  }
  					}
<DocScan>{CMD}("return"([s])?|"result"){BSEP}	{
  				          endArgumentList();
  					  if (!inReturnBlock)
					  {
					    if (inBlock()) endBlock();
					    inReturnBlock=TRUE;
					    currentListIndent.push("D");
  					    outDoc->startDescList(BaseOutputDocInterface::Return);
					    scanString(theTranslator->trReturns()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}("sa"|"see"){BSEP}        { 
  				          endArgumentList();
  					  if (!inSeeBlock)
					  {
					    if (inBlock()) endBlock();
					    inSeeBlock=TRUE;
					    currentListIndent.push("D");
					    outDoc->startDescList(BaseOutputDocInterface::See);
					    scanString(theTranslator->trSeeAlso()+": ");
					    outDoc->endDescTitle();
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->docify(", ");
					  }
					}
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"param"{BSEP}	{
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
					    currentListIndent.push("D");
					    outDoc->startParamList(BaseOutputDocInterface::Param);
					    scanString(theTranslator->trParameters()+": ");
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"retval"{BSEP}	{
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
					    currentListIndent.push("D");
					    outDoc->startParamList(BaseOutputDocInterface::RetVal);
					    scanString(theTranslator->trReturnValues()+": ");
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}("exception"|"throw")s?{BSEP}	{
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
					    currentListIndent.push("D");
					    outDoc->startParamList(BaseOutputDocInterface::Exception);
					    scanString(theTranslator->trExceptions()+": ");
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
<DocParam>({DOCPARAM}{BN}*","{BN}*)*{DOCPARAM}{BSEP}* {
					  outDoc->startDescTableTitle();
  					  outDoc->docify(substitute(yytext,"\"","").stripWhiteSpace()); 
					  outDoc->endDescTableTitle();
					  outDoc->startDescTableData();
					  BEGIN(DocScan);
  					}
<DocException>{SCOPENAME}		{
					  outDoc->startDescTableTitle();
  					  outDoc->docify(yytext); 
					  outDoc->endDescTableTitle();
					  outDoc->startDescTableData();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"section "{ID}"\n"	{
  					  QCString secName=&yytext[9];               // skip "\section "
					  secName=secName.left(secName.length()-1); // remove \n
					  //printf("SectionName %s found\n",secName.data());
					  SectionInfo *sec;
					  if ((sec=Doxygen::sectionDict[secName]))
					  {
					    //printf("Title %s\n",sec->title.data());
					    outDoc->startSection(sec->label,sec->title,
						                 sec->type==SectionInfo::Subsection);
					    scanString(sec->title);
					    outDoc->endSection(sec->label,
						               sec->type==SectionInfo::Subsection);
					  }
					  else
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",yytext);
					  }
  					}
<DocScan>{CMD}"anchor "{ID}"\n"		{
  					  QCString secName=&yytext[8];
					  secName=secName.left(secName.length()-1);
					  SectionInfo *sec;
					  if ((sec=Doxygen::sectionDict[secName]))
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
  /*
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
  					  if (Config_getList("ENABLED_SECTIONS").find(yytext)==-1)
					  {
					    outDoc->disableAll();
					  }
					  BEGIN(DocScan);
  					}
  */
<DocRefName>{SCOPENAME}|{FILE}		{
  					  QCString ref=yytext;
					  SectionInfo *sec;
					  //printf(">>> ref `%s'\n",yytext);
					  if ((sec=Doxygen::sectionDict[ref]))
					  {
					    //printf("Is a section!\n");
					    QCString text;
					    if (sec->title.isEmpty()) 
					      text=sec->label;
					    else
					      text=sec->title;
					    if (sec->type==SectionInfo::Anchor)
					    {
					      outDoc->writeObjectLink(sec->ref,sec->fileName,sec->label,text);
					      if (sec->ref.isEmpty())
					      {
					        writePageRef(*outDoc,sec->label,0);
					      }
					    }
					    else
					    {
					      //printf("  ref sec=%p sec->fileName=%s text=%s\n",sec,sec->fileName.data(),text.data());
					      outDoc->writeSectionRef(sec->ref,sec->fileName,sec->label,text);
					    }
					  }
					  else if (!generateLink(*outDoc,className,yytext,TRUE,0))
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",yytext);
					  }
					  BEGIN(DocScan);
  					}
<DocRefName>({SCOPENAME}|{FILE}){B}+/"\""	{
					  //printf(">>> ref `%s'\n",yytext);
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
					  if ((sec=Doxygen::sectionDict[sectionRef]))
					  {
					    //printf("Is a section!\n");
					    if (sec->type==SectionInfo::Anchor)
					    {
					      outDoc->writeObjectLink(sec->ref,sec->fileName,sec->label,text);
					      if (sec->ref.isEmpty())
					      {
					        writePageRef(*outDoc,sec->label,0);
					      }
					    }
					    else
					    {
					      outDoc->writeSectionRef(sec->ref,sec->fileName,sec->label,text);
					    }
					  }
					  else if (!generateLink(*outDoc,className,sectionRef,TRUE,text))
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",sectionRef.data());
					    outDoc->startBold();
					    outDoc->writeString(" unknown reference! ");
					    outDoc->endBold();
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
					  if ((sec=Doxygen::sectionDict[sectionRef]))
					  {
					    outDoc->writeSectionRefItem(sec->fileName,sec->label,text.stripWhiteSpace());
					  }
					  else
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",sectionRef.data());
					    outDoc->startBold();
					    outDoc->writeString(" unknown reference! ");
					    outDoc->endBold();
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
<DocImage>[rR][tT][fF]			{
  					  BEGIN(DocRtfImageName);
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
<DocRtfImageName>{FILE}			{
					  curImageName = findAndCopyImage(stripQuotes(yytext),IT_RTF);
					  curImageCaption.resize(0);
					  if (curImageName.isEmpty())
					  {
					    BEGIN(DocScan);
					  }
					  else
					  {
					    BEGIN(DocRtfImageOpt);
					  }
					}
<DocRtfImageOpt>\n			{
  					  writeImage(IT_RTF,0);
  					}
<DocRtfImageOpt>\"[^\n"]*\"		{
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
<DocScan>{CMD}"dotfile"{B}*		{
  					  BEGIN(DocDotFile);
  					}
<DocDotFile>{FILE}			{
  					  curDotFileName = stripQuotes(yytext);
					  curDotFileCaption.resize(0);
					  if (curDotFileName.isEmpty())
					  {
					    BEGIN(DocScan);
					  }
					  else
					  {
					    BEGIN(DocDotFileOpt);
					  }
  					}
<DocDotFileOpt>\n			{
  					  writeDotFile(curDotFileName,curDotFileCaption);
					  BEGIN(DocScan);
  					}
<DocDotFileOpt>\"[^\n"]*\"		{
					  curDotFileCaption = stripQuotes(yytext);
  					  writeDotFile(curDotFileName,curDotFileCaption);
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
  				          if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  BEGIN(DocScan);
  					}
<DocScan,DocRefName>{SCOPEMASK}"("[a-z_A-Z0-9,:\<\> \t\*\&]+")" {
  					  if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  BEGIN(DocScan);
  					}
<DocScan,DocRefName>{SCOPEMASK}("()")?	{
  					  if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  BEGIN(DocScan);
  					}
<DocScan,DocRefName>({SCOPEMASK}"::")?"operator"{OPMASK} { 
				          QCString oName=yytext;
					  if (!insideHtmlLink)
					  {
					    generateRef(*outDoc,className,
					                removeRedundantWhiteSpace(oName),inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  BEGIN(DocScan);
					}
<DocScan>("http:"|"https:"|"ftp:"|"file:"){URLMASK}	{ outDoc->startHtmlLink(yytext); 
                                                          outDoc->docify(yytext); 
							  outDoc->endHtmlLink(); 
                                                        }
<DocScan>[a-zA-Z_0-9\.\-]+"@"[0-9a-z_A-Z\.\-]+	{ outDoc->writeMailLink(yytext); }
<DocScan>{FILESCHAR}*{FILEECHAR}+/".\\n" { // special exception that is otherwise matches by FILEMASK
  					  if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
  					}
<DocScan>{FILEMASK}			{
  					  if (!insideHtmlLink)
					  {
					    generateFileRef(*outDoc,yytext);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
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
<DocScan>"<"{BLOCKQUOTE}{ATTR}">"
<DocScan>"</"{BLOCKQUOTE}{ATTR}">"
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
<DocScan>"<"{OL}{ATTR}">"{BN}*		{ outDoc->startEnumList(); 
  					  currentListIndent.push("O");
					}
<DocScan>"</"{OL}{ATTR}">"{BN}*		{ 
					  if (currentListIndent.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
                                                 "Warning in the documentation of this entity:\nMore </ol> tags than <ol> tags in the documentation of this entity."
                                                );
					  }
					  else if (*currentListIndent.top()!='O')
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
<DocScan>"<"{UL}{ATTR}">"{BN}*		{ outDoc->startItemList(); 
					  currentListIndent.push("U");
					}
<DocScan>"</"{UL}{ATTR}">"{BN}*		{ 
					  if (currentListIndent.isEmpty())
					  {
					    warn(yyFileName,yyLineNr,
                                                 "Warning in the documentation of this entity:\nMore </ul> tags than <ul> tags."
                                                );
					  }
					  else if (*currentListIndent.top()!='U')
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
					      !currentListIndent.isEmpty() && *currentListIndent.top()=='D')
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
<DocScan>"<"{KBD}{ATTR}">"		{ outDoc->startTypewriter(); }
<DocScan>"</"{KBD}{ATTR}">"		{ outDoc->endTypewriter(); }
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
					  else if (*currentListIndent.top()!='D')
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
					      *currentListIndent.top()!='D')
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
					      *currentListIndent.top()!='D')
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
<DocScan>"</"{A}{ATTR}">"		{ if (insideHtmlLink) 
                                          {
                                            outDoc->endHtmlLink(); 
                                            insideHtmlLink=FALSE;
                                          }
                                        }
<DocScan>"<"{A}{BN}+			{ BEGIN(DocHtmlLink); }
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
  					  BEGIN(DocHtmlHref); 
				        }
<DocHtmlHref>{URLMASK}			{ 
  					  htmlUrl=yytext; 
					}
<DocHtmlHref>">"			{ 
  					  outDoc->startHtmlLink(htmlUrl);
                                          insideHtmlLink=TRUE;
  					  BEGIN(DocScan); 
					}
<DocHtmlLink,DocHtmlAnchor>">"		{ BEGIN(DocScan); }
<DocScan>{CMD}("\\"|"@"|"<"|">"|"&"|"$"|"#"|"%") {
  					  outDoc->docify(&yytext[1]);
  					}
<DocScan,DocEmphasis,DocBold,DocCode>"%"[a-zA-Z_0-9\-]+		{
  					  outDoc->docify(yytext+1);
  					}
<DocEmphasis>{FILEMASK}			{
  					  outDoc->startEmphasis();
  					  if (!insideHtmlLink)
					  {
					    generateFileRef(*outDoc,yytext);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
					}
<DocEmphasis>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"("[a-z_A-Z0-9,:\<\> \t\*\&]*")" {
  					  outDoc->startEmphasis();
					  if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
  					}
<DocEmphasis>{WORD}			{ 
  					  outDoc->startEmphasis();
					  if (!insideHtmlLink)
					  {
  					    linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,FALSE);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endEmphasis();
					  BEGIN( DocScan );
					}
<DocBold>{FILEMASK}			{
  					  outDoc->startBold();
					  if (!insideHtmlLink)
					  {
					    generateFileRef(*outDoc,yytext);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endBold();
					  BEGIN( DocScan );
					}
<DocBold>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()|\*/]*"("[a-z_A-Z0-9,:\<\> \t\*\&]*")" {
  					  outDoc->startBold();
					  if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endBold();
					  BEGIN( DocScan );
  					}
<DocBold>{WORD}				{ 
  					  outDoc->startBold();
					  if (!insideHtmlLink)
					  {
  					    linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,FALSE);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endBold();
					  BEGIN( DocScan );
					}
<DocCode>{FILEMASK}			{
  					  outDoc->startTypewriter();
					  if (!insideHtmlLink)
					  {
					    generateFileRef(*outDoc,yytext);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endTypewriter();
					  BEGIN( DocScan );
					}
<DocCode>[a-z_A-Z][a-z_A-Z:0-9<>&\-=^%~!\[\]()!\*/]*"("[a-z_A-Z0-9,:\<\> \t\*\&]*")" {
  					  outDoc->startTypewriter();
					  if (!insideHtmlLink)
					  {
  					    generateRef(*outDoc,className,yytext,inSeeBlock);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
					  outDoc->endTypewriter();
					  BEGIN( DocScan );
  					}
<DocCode>{WORD}				{ 
  					  outDoc->startTypewriter();
					  if (!insideHtmlLink)
					  {
  					    linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,FALSE);
					  }
					  else
					  {
					    outDoc->docify(yytext);
					  }
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
                                          bool ib = inBlock();
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
					    else if (!ib)
					    {
					      outDoc->newParagraph(); 
					    }
					  }
					  if (ib) endBlock();
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

void parseDocument(OutputDocInterface &od,const QCString &docString)
{
  //inParamBlock=inSeeBlock=inReturnBlock=FALSE;
  curTable = 0;
  depthIf = 0;
  outDoc = od.clone();
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
  od.append(outDoc);
  delete outDoc; outDoc=0;
  return;
}

//----------------------------------------------------------------------------

void parseDoc(OutputDocInterface &od,const char *fileName,int startLine,
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
  parseDocument(od,docString);

}

//----------------------------------------------------------------------------

void parseText(OutputDocInterface &od,const QCString &txtString)
{
  if (txtString.isEmpty()) return;
  inputString = txtString;
  outDoc = od.clone();
  inputPosition = 0;
  docYYrestart( docYYin ); 
  BEGIN( Text );
  docYYlex();
  od.append(outDoc);
  delete outDoc; outDoc=0;
  return;
}

//----------------------------------------------------------------------------

void parseExample(OutputDocInterface &od,const QCString &docString, 
                  const char *fileName)
{
  initParser();
  initParseCodeContext();
  exampleDoc=TRUE;            // cross reference with member docs
  exampleName=fileName;
  strcpy(yyFileName,fileName);
  parseDocument(od,docString); 
}

//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void docYYdummy() { yy_flex_realloc(0,0); } 
}
