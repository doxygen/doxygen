/****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
  
// new experimental parser
#include "debug.h"
  
#include "doc.h"
#include "code.h"
#include "message.h"
#include "doxygen.h"
#include "config.h"
#include "util.h"
#include "language.h"
#include "outputlist.h"
#include "reflist.h"
#include "page.h"

#ifndef WIN32
#include <unistd.h>
#endif
  
#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 * scanner's state variables
 */
static MemberDef          * memberDef;
static bool                 hasParamCommand;
static QDict<void>          paramsFound;

static OutputDocInterface * outDoc;
static bool             insideArgumentList;
static QCString         className;
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
static QCString         caption;
static QCString         refItemText;
static QCString         addIndexWord;
static QStack<char>	  currentListIndent; // indent stack of all list items 
static bool insideItemList = FALSE;

struct DocLexerContext
{
  int rule;
  int position;
  const char *inputString;
  YY_BUFFER_STATE lexerState;
};

static QStack<DocLexerContext> lexerStack;

static void pushContext()
{
  DocLexerContext *ctx = new DocLexerContext;
  ctx->rule = YY_START;
  ctx->position = inputPosition;
  ctx->inputString = inputString;
  ctx->lexerState = YY_CURRENT_BUFFER;
  lexerStack.push(ctx);
  yy_switch_to_buffer(yy_create_buffer(docYYin, YY_BUF_SIZE));  
}

static bool popContext()
{
  if (lexerStack.isEmpty()) return TRUE; 
  DocLexerContext *ctx = lexerStack.pop();
  inputPosition = ctx->position;
  inputString = ctx->inputString;
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(ctx->lexerState);
  BEGIN(ctx->rule);
  delete ctx;
  return TRUE;
}
  
static QCString         copyDocString;
static QCString         copyDocScope;
static QList<void>      copyDocDefList;

//-----------------------------------------------------------------------------

static void initParser()
{
  insideArgumentList=FALSE;
  className.resize(0);
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
void scanDoc(const char *s);
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
    void setCaption(const char *s);
    
  private:
    OutputDocInterface *m_parentDoc;
    QList<TableElem> *m_elemList;
    QCString m_caption;
    int m_curRow;
    int m_curCol;
    int m_rows;
    int m_cols;
};  

Table::Table() 
{
  m_parentDoc=outDoc;
  m_elemList=new QList<TableElem>;
  m_elemList->setAutoDelete(TRUE);
  m_curRow=m_curCol=m_rows=m_cols=0;
}

Table::~Table()
{
  //printf("Table::~Table()\n");
  // use elemList & cols & rows
  if (m_cols>0 && m_rows>0)
  {
    m_parentDoc->startTable(!m_caption.isEmpty(),m_cols);
    TableElem *e=m_elemList->first();
    while (e)
    {
      if (e->getRow()>0)
      {
	if (e->getCol()==0) 
	{
	  if (e->getRow()>1) m_parentDoc->endTableRow();
	  m_parentDoc->nextTableRow();
	}
	else
	{
	  m_parentDoc->nextTableColumn();
	}
	m_parentDoc->append(e->outputDocInterface());
	m_parentDoc->endTableColumn();
      }
      e=m_elemList->next();
    }
    if (!m_caption.isEmpty())
    {
      m_parentDoc->startCaption();
      m_parentDoc->docify(m_caption);
      m_parentDoc->endCaption();
    }
    m_parentDoc->endTable(!m_caption.isEmpty());
  }
  delete m_elemList; m_elemList=0;
  outDoc=m_parentDoc;
}

void Table::setCaption(const char *s)
{
  m_caption=s;
}

void Table::newRow()
{
  //printf("Table::newRow()\n");
  m_curRow++;
  if (m_curRow>m_rows) m_rows=m_curRow;
  m_curCol=0;
}

void Table::newElem()
{
  //printf("Table::newElem(%d,%d)\n",curRow,curCol);
  TableElem *te = new TableElem(m_curRow,m_curCol);
  m_elemList->append(te);

  m_curCol++;
  if (m_curCol>m_cols) m_cols=m_curCol;
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
    currentIncludeFile=fileToString(fd->absFilePath(),Config_getBool("FILTER_SOURCE_FILES"));
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
    od.codify(fileToString(fd->absFilePath(),Config_getBool("FILTER_SOURCE_FILES"))+"\n");
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
  else if (result.right(Doxygen::htmlFileExtension.length())==Doxygen::htmlFileExtension) 
  {
    result=result.left(result.length()-Doxygen::htmlFileExtension.length());
  }
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
      //od.writeString("\n");
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
    //od.writeString("\n");
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
      //od.writeString("\n");
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
    outDoc->endSimpleSect();
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

static int computeIndent(const char *str,int length)
{
  int i;
  int indent=0;
  int tabSize=Config_getInt("TAB_SIZE");
  for (i=0;i<length;i++)
  {
    //printf("Parsed[%d]=%d\n",i,marker[i]);
    if (str[i]=='\t')
    {
      indent+=tabSize - (indent%tabSize);
    }
    else if (str[i]=='\n')
    {
      indent=0;
    }
    else
    {
      indent++;
    }
  }
  return indent;
}

static void addListItemMarker(const char *marker,int dashPos,bool enumerated)
{
  // find the actual position at which the bullet was found
  int indent=computeIndent(marker,dashPos);
  //printf("list marker found at column %d enumerated %d\n",indent,enumerated);
  if (!insideItemList)
  {
    //printf("startListMarker indent=%d\n",indent);
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
      //printf("startListMarker indent=%d\n",indent);
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

static void endListMarker(const char *marker,int dotPos)
{
  int indent=computeIndent(marker,dotPos);
  //printf("endListMarker indent=%d "
  //       "insideItemList=%d listIndentStack.count()=%d\n",
  //	 indent,insideItemList,listIndentStack.count());
  if (insideItemList && !listIndentStack.isEmpty())
  {
    IndentInfo *ii = listIndentStack.top(); 
    while (ii && indent<=ii->indent)
    {
      ii->endList();
      listIndentStack.pop();
      currentListIndent.pop();
      delete ii;
      ii = listIndentStack.top();
      //printf("ending list new indent=%d\n",ii ? ii->indent : -1);
    }
    if (listIndentStack.isEmpty())
    {
      insideItemList=FALSE;
      //printf("ending last list\n");
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
       case 'D': outDoc->endDescription(); break;
       case 'P': if (inBlock()) endBlock(); break; 
       default:
	  err("Unexpected list indent token `%c'\n",c);
    }
  }
  insideItemList=FALSE;
}

static void endArgumentList()
{
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
	  if (!Config_getBool("GENERATE_HTML")) return result;
	  outputDir = Config_getString("HTML_OUTPUT");
	  break;
	case IT_Latex: 
	  if (!Config_getBool("GENERATE_LATEX")) return result;
	  outputDir = Config_getString("LATEX_OUTPUT");
	  break;
	case IT_RTF:
	  if (!Config_getBool("GENERATE_RTF")) return result;
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

    if (type==IT_Latex && Config_getBool("USE_PDFLATEX") && 
	fd->name().right(4)==".eps"
       )
    { // we have an .eps image in pdflatex mode => convert it to a pdf.
      QCString outputDir = Config_getString("LATEX_OUTPUT");
      QCString baseName  = fd->name().left(fd->name().length()-4);
      QCString epstopdfArgs(4096);
      epstopdfArgs.sprintf("\"%s/%s.eps\" --outfile=\"%s/%s.pdf\"",
                           outputDir.data(), baseName.data(),
			   outputDir.data(), baseName.data());
      if (iSystem("epstopdf",epstopdfArgs,TRUE)!=0)
      {
	err("Error: Problems running epstopdf. Check your TeX installation!\n");
      }
      return baseName;
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

static void checkArgName(const QCString &name,bool isParam)
{
  hasParamCommand=TRUE;
  if (memberDef==0) return; // not a member
  ArgumentList *al=memberDef->isDocsForDefinition() ? 
		   memberDef->argumentList() :
                   memberDef->declArgumentList();
  if (al==0) return; // no argument list
  if (!Config_getBool("WARN_IF_UNDOCUMENTED")) return;

  //printf("name=%s\n",name.data());
  static QRegExp re("[a-zA-Z0-9_]+\\.*");
  int p=0,i=0,l;
  while ((i=re.match(name,p,&l))!=-1)
  {
    QCString aName=name.mid(i,l);
    //printf("aName=%s\n",aName.data());
    ArgumentListIterator ali(*al);
    Argument *a;
    bool found=FALSE;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      QCString argName = memberDef->isDefine() ? a->type : a->name;
      if (argName.right(3)=="...") argName=argName.left(argName.length()-3);
      if (aName==argName) 
      {
	//printf("adding `%s'\n",aName.data());
	paramsFound.insert(aName,(void *)(0x8));
	found=TRUE;
	break;
      }
    }
    if (!found && isParam)
    {
      //printf("member type=%d\n",memberDef->memberType());
      QCString scope=memberDef->getScopeString();
      if (!scope.isEmpty()) scope+="::"; else scope="";
      warn(memberDef->docFile(),memberDef->docLine(),
	  "Warning: argument `%s' of command @param "
	  "is not found in the argument list of %s%s%s",
	  aName.data(),scope.data(),memberDef->name().data(),
	  argListToString(al).data()
	  );
    }
    p=i+l;
  }
}

/* ----------------------------------------------------------------- */

/*! Looks for a documentation block with name commandName in the current
 *  context (copyDocScope). The resulting documentation string is
 *  put in pDoc, the definition in which the documentation was found is
 *  put in pDef.
 *  @retval TRUE if name was found.
 *  @retval FALSE if name was not found.
 */
static bool findDocsForMemberOrCompound(const char *commandName,
                                 QCString *pDoc,
                                 Definition **pDef)
{
  pDoc->resize(0);
  *pDef=0;
  QCString cmdArg=commandName;
  int l=cmdArg.length();
  if (l==0) return FALSE;

  int funcStart=cmdArg.find('(');
  if (funcStart==-1) funcStart=l;
  //int lastScopeStart=cmdArg.findRev("::",funcStart);
  //int lastScopeEnd = lastScopeStart==-1 ? 0 : lastScopeStart+2;
  //QCString scope=cmdArg.left(QMAX(lastScopeStart,0));
  //QCString name=cmdArg.mid(lastScopeEnd,funcStart-lastScopeEnd);
  QCString name=cmdArg.left(funcStart);
  QCString args=cmdArg.right(l-funcStart);

  // try if the link is to a member
  MemberDef    *md=0;
  ClassDef     *cd=0;
  FileDef      *fd=0;
  NamespaceDef *nd=0;
  GroupDef     *gd=0;
  PageInfo     *pi=0;
  bool found = getDefs(copyDocScope,name,args,md,cd,fd,nd,gd,FALSE,0,TRUE);
  if (found && md)
  {
    *pDoc=md->documentation();
    *pDef=md;
    return TRUE;
  }


  int scopeOffset=copyDocScope.length();
  do // for each scope
  {
    QCString fullName=cmdArg;
    if (scopeOffset>0)
    {
      fullName.prepend(copyDocScope.left(scopeOffset)+"::");
    }
    //printf("Trying fullName=`%s'\n",fullName.data());

    // try class, namespace, group, page, file reference
    cd = Doxygen::classSDict[fullName];
    if (cd) // class 
    {
      *pDoc=cd->documentation();
      *pDef=cd;
      return TRUE;
    }
    nd = Doxygen::namespaceSDict[fullName];
    if (nd) // namespace
    {
      *pDoc=nd->documentation();
      *pDef=nd;
      return TRUE;
    }
    gd = Doxygen::groupSDict[cmdArg];
    if (gd) // group
    {
      *pDoc=gd->documentation();
      *pDef=gd;
      return TRUE;
    }
    pi = Doxygen::pageSDict->find(cmdArg);
    if (pi) // page
    {
      *pDoc=pi->doc;
      *pDef=(Definition *)pi;
      return TRUE;
    }
    bool ambig;
    fd = findFileDef(Doxygen::inputNameDict,cmdArg,ambig);
    if (fd && !ambig) // file
    {
      *pDoc=fd->documentation();
      *pDef=fd;
      return TRUE;
    }

    if (scopeOffset==0)
    {
      scopeOffset=-1;
    }
    else
    {
      scopeOffset = copyDocScope.findRev("::",scopeOffset-1);
      if (scopeOffset==-1) scopeOffset=0;
    }
  } while (scopeOffset>=0);

  
  return FALSE;
}

/* ----------------------------------------------------------------- */

static void writeSpecialItem(const char *commandString)
{
  QCString cmd = commandString; // format "\test 20" or "\todo 4"
  int sepPos=cmd.find(' ');
  QCString listName = cmd.mid(1,cmd.length()-sepPos-1); // i.e. "test" or "todo"
  RefList *refList = Doxygen::specialLists->find(cmd.mid(1,sepPos-1));
  ASSERT(refList!=0);
  if (Config_getBool(refList->optionName()))
  {
    QCString numStr=cmd.right(cmd.length()-sepPos-1); // i.e. "20" or "4"
    bool ok; int num = numStr.toUInt(&ok);
    RefItem *item = refList->getRefItem(num);
    ASSERT(item!=0);
    if (insideItemList)
    {
      forceEndItemList();
    }
    endArgumentList();
    if (inBlock()) endBlock();
    currentListIndent.push("P");
    outDoc->startSimpleSect(refList->sectionType(),
                            refList->listName(),
			    item->listAnchor,
			    refList->sectionTitle()+": "
			   );
    outDoc->writeDescItem();
    internalParseDocument(item->text);
    outDoc->endSimpleSect();
    currentListIndent.pop();
  }
}

/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
    int c=0;
    while ( c < max_size && inputString[inputPosition] )
    {
	*buf = inputString[inputPosition++] ;
	//printf("%d (%c)\n",*buf,*buf);
	c++; buf++;
    }
    return c;
}

 //LINKMASK  [a-z_A-Z0-9:#.,~&*/\[\]<>()\-\+]+({B}*("const"|"volatile"))?
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
URLCHAR   [a-z_A-Z0-9\!\~\:\;\'\$\?\@\&\%\#\.\-\+\/\=]
URLMASK   (([a-z_A-Z][^\>\"\n]*{URLCHAR})|({URLCHAR}+))([({]{URLCHAR}*[)}])?
NONTERM   [\{\}\[\]\`\~\@\|\-\+\#\$\/\\\!\%\^\&\*()a-z_A-Z<>0-9\x80-\xff]
WORD      ({NONTERM}+([^\n\t ]*{NONTERM}+)?)|("\""[^\n\"]*"\"")
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
CAPTION   [cC][aA][pP][tT][iI][oO][nN]
TITLE     [tT][iI][tT][lL][eE]
TD	  [tT][dD]
TR	  [tT][rR]
TT	  [tT][tT]
UL	  [uU][lL]
VAR	  [vV][aA][rR]
BLOCKQUOTE [bB][lL][oO][cC][kK][qQ][uU][oO][tT][eE]
DOCPARAM  ("#")?([a-z_A-Z0-9:\!\<\~\>\^\&\=\.\-]+)|("\"".*"\"")
OPNEW     {B}+"new"({B}*"[]")?
OPDEL     {B}+"delete"({B}*"[]")?
OPARG     "("[a-z_A-Z0-9,\<\> \t\*\&]*")"
OPNORM    {OPNEW}|{OPDEL}|"+"|"-"|"*"|"/"|"%"|"^"|"&"|"|"|"~"|"!"|"="|"<"|">"|"+="|"-="|"*="|"/="|"%="|"^="|"&="|"|="|"<<"|">>"|"<<="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|","|"->*"|"->"|"[]"|"()"
OPCAST    {B}+[^(\r\n.,]+
OPMASK    ({B}*{OPNORM}({OPARG}?))|({OPCAST}{OPARG})
LINKMASK  [^ \t\n\r\\@<&$]+("("[^\n)]*")")?({B}*("const"|"volatile"))? 

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
%x	DocCaption
%x	DocHtmlImageName
%x	DocHtmlImageOpt
%x	DocLatexImageName
%x	DocLatexImageOpt
%x	DocRtfImageName
%x	DocRtfImageOpt
%x	DocDotFile
%x	DocDotFileOpt
%x	DocSkipLanguage
%x	DocCopyFind
%x	DocCopyArg
%x	DocCopySkipVerb

%%

<*>\x0d
<DocScan>^{B}*(("//"{B}*)?)"*"*{B}*"-"("#")?{B}+ { /* found list item marker */
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  //printf("dashPos=%d char='%c'\n",dashPos,text.at(dashPos+1));
					  bool isEnumerated = text.at(dashPos+1)=='#';
  					  addListItemMarker(yytext,dashPos+1,isEnumerated);
  					}
<DocScan>^{B}*(("//"{B}*)?)"*"*{B}*"."{B}*/\n { /* found end list marker */
  					  QCString text=yytext;
                                          int dotPos = text.findRev('.');
                                          endListMarker(yytext,dotPos+1);
                                        }
<DocScan>\n{B}*(("//"{B}*)?)"*"*{B}*"-"("#")?{B}+ { /* found list item marker */
  					  QCString text=yytext;
					  int dashPos = text.findRev('-');
					  //printf("dashPos=%d char='%c'\n",dashPos,text.at(dashPos+1));
					  bool isEnumerated = text.at(dashPos+1)=='#';
  					  addListItemMarker(yytext+1,dashPos,isEnumerated);
  					}
<DocScan>\n{B}*(("//"{B}*)?)"*"*{B}*"."{B}*/\n { /* found end list marker */
  					  QCString text=yytext;
                                          int dotPos = text.findRev('.');
                                          endListMarker(yytext+1,dotPos);
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
  					  outDoc->startHtmlOnly();
  				          outDoc->pushGeneratorState(); 
  					  outDoc->disableAllBut(OutputGenerator::Html); 
					  BEGIN(DocHtmlScan);
  					}
<DocHtmlScan>{CMD}"endhtmlonly"/[^a-z_A-Z0-9] {
  					  outDoc->popGeneratorState();
					  outDoc->endHtmlOnly();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"latexonly"/[^a-z_A-Z0-9]	{
  					  outDoc->startLatexOnly();
  					  outDoc->pushGeneratorState();
  					  outDoc->disableAllBut(OutputGenerator::Latex);
					  BEGIN(DocLatexScan);
  					}
<DocLatexScan>{CMD}"endlatexonly"/[^a-z_A-Z0-9] {
  					  outDoc->popGeneratorState();
					  outDoc->endLatexOnly();
					  BEGIN(DocScan);
  					}
<DocHtmlScan,DocLatexScan>"//"|"/*"|"*/"	{
  					  outDoc->writeString(yytext);
  					}
<DocHtmlScan,DocLatexScan>.|\n		{
  					  outDoc->writeString(yytext);
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
  					  outDoc->startVerbatimFragment();
					  insideVerbatim=TRUE;
					  BEGIN(DocVerbatim);
  					}
<DocVerbatim>{CMD}"endverbatim"/[^a-z_A-Z0-9]	{
  					  outDoc->endVerbatimFragment();
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
<DocScan>{CMD}"~"[a-z_A-Z0-9]*/{BN}	{
  					  if (yytext[2])
					  {
					    if (theTranslator->idLanguage()!=&yytext[2]) // not current language
					    {
					      BEGIN(DocSkipLanguage);
					    }
					  }
  					}
<DocSkipLanguage>{CMD}"~"[a-z_A-Z0-9]*/{BN} {
  					  if (yytext[2])
					  {
					    if (theTranslator->idLanguage()==&yytext[2])
					    {
					      BEGIN( DocScan ); // current language => include
					    }
					  }
					  else // back to "all language" processing.
					  {
					    BEGIN( DocScan );
					  }
  					}
<DocSkipLanguage>[^\\\@\~\n]+
<DocScan>{CMD}"internal"/{BN}	        { 
					  outDoc->newParagraph();
					  outDoc->startBold();
					  scanString(theTranslator->trForInternalUseOnly()+"\n");
					  outDoc->endBold();
					  outDoc->newParagraph();
					}
<DocScan>"\\reimp"/{BN}			{
  					  outDoc->newParagraph();
					  scanString(theTranslator->trReimplementedForInternalReasons()+"\n");
  					}
<DocScan>{CMD}"link"/{BN}	        { BEGIN( DocLink ); }
<DocScan>"{"{CMD}"link"{BN}+	        { BEGIN( DocJavaLink ); }
<DocSkipWord>[a-z_A-Z0-9.:()]+		{ BEGIN( DocScan ); }
<DocLink>{LINKMASK}                     { // TODO: support operators as well!
  					  linkRef  = stripKnownExtensions(yytext);
					  linkText = "";
  					  BEGIN( DocLinkText ); 
					}
<DocJavaLink>{LINKMASK}                 { // TODO: support operators as well!
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
                                          addIndexWord.resize(0);
  					  BEGIN(DocIndexWord);
  					}
<DocScan>"\\form#"[0-9]+		{
  					  Formula *formula=Doxygen::formulaNameDict[yytext];
					  if (formula)
					  {
					    QCString formName;
					    formName.sprintf("form_%d",formula->getId());
					    outDoc->writeFormula(formName,formula->getFormulaText());
					  }
  					}
<DocIndexWord>"\\&"			{ addIndexWord+='&'; }
<DocIndexWord>"\\>"			{ addIndexWord+='>'; }
<DocIndexWord>"\\<"			{ addIndexWord+='<'; }
<DocIndexWord>"\\@"			{ addIndexWord+='@'; }
<DocIndexWord>"\\$"			{ addIndexWord+='$'; }
<DocIndexWord>"\\#"			{ addIndexWord+='#'; }
<DocIndexWord>"\\\\"			{ addIndexWord+='\\'; }
<DocIndexWord>.			        { addIndexWord+=*yytext; }
<DocIndexWord>\n		        {
  					  //printf("Adding %s to index\n",yytext);
					  outDoc->addIndexItem(addIndexWord,0);
  					  BEGIN(DocScan);
  					}
<DocScan>{CMD}("arg"|"li")/{BN}	        { 
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
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
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"par"{B}*	{
 					  QCString t=yytext;
  					  if (/*t.contains('\n')>1 &&*/ insideItemList)
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
  					    if (insideItemList)
					    {
					      forceEndItemList();
					    }
  				            endArgumentList();
					    if (b) endBlock();  
					    inParBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Par,0,0,title);
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->newParagraph();
					  }
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"warning"{BSEP}		{ 
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inWarningBlock)
					  {
					    if (inBlock()) endBlock();
					    inWarningBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Warning,0,0,theTranslator->trWarning()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"remark"[s]?{BSEP}	{ 
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inRemarkBlock)
					  {
					    if (inBlock()) endBlock();
					    inRemarkBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Remark,0,0,theTranslator->trRemarks()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"attention"{BSEP}		{ 
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inAttentionBlock)
					  {
					    if (inBlock()) endBlock();
					    inAttentionBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Attention,0,0,theTranslator->trAttention()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
				        }
<DocScan>{CMD}"note"[s]?{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inNoteBlock)
					  {
					    if (inBlock()) endBlock();
					    inNoteBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Note,0,0,theTranslator->trNote()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"pre"{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inPreBlock)
					  {
					    if (inBlock()) endBlock();
					    inPreBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Pre,0,0,theTranslator->trPrecondition()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"post"{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inPostBlock)
					  {
					    if (inBlock()) endBlock();
					    inPostBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Post,0,0,theTranslator->trPostcondition()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"invariant"{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inInvarBlock)
					  {
					    if (inBlock()) endBlock();
					    inInvarBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Invar,0,0,theTranslator->trInvariant()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"version"{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inVersionBlock)
					  {
					    if (inBlock()) endBlock();
					    inVersionBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Version,0,0,theTranslator->trVersion()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"since"{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inSinceBlock)
					  {
					    if (inBlock()) endBlock();
					    inSinceBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Since,0,0,theTranslator->trSince()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}"date"{BSEP}		{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inDateBlock)
					  {
					    if (inBlock()) endBlock();
					    inDateBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Date,0,0,theTranslator->trDate()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
  					    outDoc->writeDescItem();
					  }
  					}
<DocScan>"\\todo "[0-9]+		| /* generated labels */
<DocScan>"\\test "[0-9]+		|
<DocScan>"\\bug "[0-9]+			|
<DocScan>"\\deprecated "[0-9]+		{ 
                                          writeSpecialItem(yytext);
  					}
 /*
<DocScan>{CMD}"deprecated"{BSEP}	{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  					  endArgumentList();
					  if (!inDeprecatedBlock)
					  {
					    if (inBlock()) endBlock();
					    inDeprecatedBlock=TRUE;
					    currentListIndent.push("P");
					    outDoc->startSimpleSect(BaseOutputDocInterface::Deprecated,0,0,theTranslator->trDeprecated()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->writeDescItem();
					  }
                                        }
  */
<DocScan>"$"[a-zA-Z_0-9]+":"[^\n\$]+"$"	{ // RCS tag
  					  QCString tagName(&yytext[1]);
					  int i=tagName.find(':');
					  tagName=tagName.left(i);
					  QCString tagText=&yytext[i+2];
					  tagText=tagText.left(tagText.length()-1);
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
					  if (inBlock()) endBlock();
					  currentListIndent.push("P");
  					  outDoc->startSimpleSect(BaseOutputDocInterface::RCS,0,0,tagName+": ");
					  outDoc->writeDescItem();
					  scanString(tagText);
					  outDoc->endSimpleSect();
					  currentListIndent.pop();
  					}
<DocScan>{CMD}"author"[s]?{BSEP}	{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inAuthorBlock)
					  {
					    if (inBlock()) endBlock();
					    inAuthorBlock=TRUE;
					    currentListIndent.push("P");
					    bool singular = ((QString)yytext).find('s')==-1;
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Author,
						                    0,0,theTranslator->trAuthor(TRUE,singular)+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->docify(", ");
					  }
  					}
<DocScan>{CMD}("return"([s])?|"result"){BSEP}	{
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inReturnBlock)
					  {
					    if (inBlock()) endBlock();
					    inReturnBlock=TRUE;
					    currentListIndent.push("P");
  					    outDoc->startSimpleSect(BaseOutputDocInterface::Return,0,0,theTranslator->trReturns()+": ");
					    outDoc->writeDescItem();
					  }
  					}
<DocScan>{CMD}("sa"|"see"){BSEP}        { 
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inSeeBlock)
					  {
					    if (inBlock()) endBlock();
					    inSeeBlock=TRUE;
					    currentListIndent.push("P");
					    outDoc->startSimpleSect(BaseOutputDocInterface::See,0,0,theTranslator->trSeeAlso()+": ");
					    outDoc->writeDescItem();
					  }
					  else
					  {
					    outDoc->docify(", ");
					  }
					}
<DocScan>(({B}*"\n"){2,}{B}*)?{CMD}"param"{BSEP}	{
					  QCString t=yytext;
  					  if (insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inParamBlock)
					  {
					    if (inBlock()) endBlock();
					    inParamBlock=TRUE;
					    currentListIndent.push("P");
					    outDoc->startParamList(BaseOutputDocInterface::Param,theTranslator->trParameters()+": ");
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
  					  if (/*t.contains('\n')>1 &&*/ insideItemList)
					  {
					    forceEndItemList();
					  }
  				          endArgumentList();
  					  if (!inRetValBlock)
					  {
					    if (inBlock()) endBlock();
					    inRetValBlock=TRUE;
					    currentListIndent.push("P");
					    outDoc->startParamList(BaseOutputDocInterface::RetVal,theTranslator->trReturnValues()+": ");
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
  					  if (/*t.contains('\n')>1 &&*/ insideItemList)
					  {
					    forceEndItemList();
					  }
  					  endArgumentList();
					  if (!inExceptionBlock)
					  {
					    if (inBlock()) endBlock();
					    inExceptionBlock=TRUE;
					    currentListIndent.push("P");
					    outDoc->startParamList(BaseOutputDocInterface::Exception,theTranslator->trExceptions()+": ");
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
  					  QCString argName = substitute(yytext,"\"","").stripWhiteSpace();
					  if (inParamBlock) 
					  {
					    checkArgName(argName,TRUE);
					  }
					  else if (inRetValBlock)
					  {
					    checkArgName(argName,FALSE);
					  }
					  outDoc->startDescTableTitle();
					  scanDoc(argName);
					  outDoc->endDescTableTitle();
					  outDoc->startDescTableData();
					  BEGIN(DocScan);
  					}
<DocException>{SCOPENAME}		{
					  outDoc->startDescTableTitle();
  					  generateRef(*outDoc,className,yytext,FALSE);
					  outDoc->endDescTableTitle();
					  outDoc->startDescTableData();
					  BEGIN(DocScan);
  					}
<DocScan>{CMD}"section "{ID}" "	{
  					  QCString secName=&yytext[9];               // skip "\section "
					  secName=secName.left(secName.length()-1); // remove \n
					  //printf("SectionName %s found\n",secName.data());
					  SectionInfo *sec;
					  if ((sec=Doxygen::sectionDict[secName]))
					  {
					    //printf("Title %s\n",sec->title.data());
					    outDoc->startSection(sec->label,sec->title,sec->type);
					    scanString(sec->title);
					    outDoc->endSection(sec->label,sec->type);
					  }
					  else
					  {
					    warn(yyFileName,yyLineNr,"Warning: reference to unknown section %s in the documentation of this entity!",yytext);
					  }
  					}
<DocScan>{CMD}"anchor "{ID}" "	        {
  					  QCString secName=&yytext[8];
					  secName=secName.left(secName.length()-1);
					  SectionInfo *sec;
					  //printf("secName=%s\n",secName.data());
					  if ((sec=Doxygen::sectionDict[secName]))
					  {
					    //printf("writeAnchor %s_%s\n",sec->fileName.data(),sec->label.data());
					    outDoc->writeAnchor(sec->fileName,sec->label);
					  }
					  else
					  {
					    //printf("Section %s not found!\n",secName.data());
					  }
  					}
<DocScan>"\\_internalref"{B}+		{ // for internal use only!
					  internalRefFile.resize(0);
					  internalRefAnchor.resize(0);
  					  BEGIN(DocInternalRef);
  					}
<DocInternalRef>[A-Z_a-z0-9.:#\-\+]+	{
  					  internalRefFile=yytext;
					  int i = internalRefFile.find('#');
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
<DocScan>{CMD}"secreflist"/{BN}		{ 
  				          outDoc->startSectionRefList();
					}
<DocScan>{CMD}"endsecreflist"/{BN}	{
  					  outDoc->endSectionRefList();
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
					  refItemText.resize(0);
					  BEGIN(DocRefItemName);
  					}
<DocRefItemName>"\\&"			{ refItemText+='&'; }
<DocRefItemName>"\\>"			{ refItemText+='>'; }
<DocRefItemName>"\\<"			{ refItemText+='<'; }
<DocRefItemName>"\\@"			{ refItemText+='@'; }
<DocRefItemName>"\\$"			{ refItemText+='$'; }
<DocRefItemName>"\\#"			{ refItemText+='#'; }
<DocRefItemName>"\\\\"			{ refItemText+='\\'; }
<DocRefItemName>.			{ refItemText+=*yytext; }
<DocRefItemName>\n			{
					  SectionInfo *sec;
					  if ((sec=Doxygen::sectionDict[sectionRef]))
					  {
					    outDoc->writeSectionRefItem(sec->fileName,sec->label,refItemText.stripWhiteSpace());
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
<DocHtmlImageName>[^ \t\n]+             {
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
					  BEGIN(DocScan);
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
<DocScan,DocRefName>{SCOPEMASK}"("[a-z_A-Z0-9,:\<\> \t\*\&]+")"({B}*("const"|"volatile"))? {
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
<DocScan,DocRefName>{SCOPEMASK}("()"({B}*("const"|"volatile"))?)? {
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
  					  codeBlock+="\n";
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
<DocSkipKey>[^\r\n]+			{ 
  					  if (includeFileLength>0) 
					  {
					    QCString pattern=yytext;
					    skipUntil(pattern.stripWhiteSpace()); 
					  }
  					  BEGIN( DocScan );
					}
<DocLineKey>[^\r\n]+			{ 
  					  if (includeFileLength>0) 
					  {
					    QCString pattern=yytext;
					    if (firstLine) outDoc->startCodeFragment();
					    firstLine=FALSE;
					    showLine(*outDoc,pattern.stripWhiteSpace()); 
					    BEGIN( DocKeyEnd );
					  }
					  else
					  {
  					    BEGIN( DocScan );
					  }
					}
<DocSkiplineKey>[^\r\n]+		{ 
  					  if (includeFileLength>0) 
					  {
					    QCString pattern=yytext;
					    if (firstLine) outDoc->startCodeFragment();
					    firstLine=FALSE;
					    skipLine(*outDoc,pattern.stripWhiteSpace()); 
					    BEGIN( DocKeyEnd );
					  }
					  else
					  {
  					    BEGIN( DocScan );
					  }
					}
<DocUntilKey>[^\r\n]+			{ 
  					  if (includeFileLength>0) 
					  {
					    QCString pattern=yytext;
					    if (firstLine) outDoc->startCodeFragment();
					    firstLine=FALSE;
					    showUntil(*outDoc,pattern.stripWhiteSpace()); 
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
<DocScan>"<"{CAPTION}{ATTR}">"		{ caption.resize(0); 
                                          BEGIN( DocCaption ); 
                                        }
<DocCaption>[^\n\<\>\/]+		{
  					  caption+=yytext;
  					}
<DocCaption>\n				{ caption+=" "; }
<DocCaption>.				{ caption+=*yytext; }
<DocCaption>"</"{CAPTION}{ATTR}">"	{ if (curTable) 
                                          { 
					    curTable->setCaption(caption); 
					  }
                                          BEGIN( DocScan ); 
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
<DocScan,DocEmphasis,DocBold,DocCode>"%"[a-z_A-Z][a-z_A-Z0-9:\-]* {
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
  					    linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,TRUE);
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
  					    linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,TRUE);
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
  					    linkifyText(TextGeneratorOLImpl(*outDoc),className,0,yytext,FALSE,TRUE);
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
					    if (ib) endBlock();
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
					    if (ib && *currentListIndent.top()=='P') 
					    { // inside paragraph block
					      endBlock();
					    }
					  }
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
<DocCopyFind>{CMD}"copydoc"{B}+		{ // found copydoc command
  					  BEGIN(DocCopyArg);
  					}
<DocCopyFind>{CMD}"verbatim"/[^a-z_A-Z0-9] { // skip verbatim sections
                                          copyDocString+=yytext; 
                                          BEGIN(DocCopySkipVerb);
                                        }
<DocCopyFind>[^@\\\n]+			{ copyDocString+=yytext; }
<DocCopyFind>"\\\\"|"@@"		{ copyDocString+=yytext; /* skip escaped commands */ }
<DocCopySkipVerb>[^@\\\n]+		{ copyDocString+=yytext; }
<DocCopySkipVerb>{CMD}"endverbatim"	{ copyDocString+=yytext; 
                                          BEGIN(DocCopyFind);
                                        }
<DocCopyFind,DocCopySkipVerb>\n		{ copyDocString+=yytext; }
<DocCopyFind,DocCopySkipVerb>.		{ copyDocString+=yytext; }
<DocCopyArg>{LINKMASK}			{ //printf("found @copydoc with arg `%s'\n",yytext);
                                          QCString doc;
					  Definition *def;
                                          if (findDocsForMemberOrCompound(yytext,&doc,&def))
					  {
					    //printf("found docs `%s'\n",doc.data());
					    if (copyDocDefList.findRef(def)!=-1)
					    {
					      warn(yyFileName,yyLineNr,"Recursive @copydoc relation!");
					    }
					    else
					    {
					      copyDocDefList.append(def);
					      pushContext();
					      inputString = doc;
					      inputPosition = 0;
					      BEGIN(DocCopyFind);
					      docYYlex();
					      popContext();
					      copyDocDefList.remove(def);
					      BEGIN(DocCopyFind);
					    }
					  }
					  else // reference not found!
					  {
					    warn(yyFileName,yyLineNr,"Argument `%s' of @copydoc command "
						 "could not be resolved!",yytext);
					  }
                                          BEGIN(DocCopyFind);
					}
<*>\n					{ yyLineNr++ ; }
<*>.

%%

//----------------------------------------------------------------------------

void scanString(const char *s)
{
  pushContext();
  inputString  = s;
  inputPosition = 0;
  BEGIN( Text );
  docYYlex();
  popContext();
}

void scanDoc(const char *s)
{
  pushContext();
  inputString  = s;
  inputPosition = 0;
  BEGIN( DocScan );
  docYYlex();
  popContext();
}

void internalParseDocument(const char *s)
{
  if (s==0) return;
  pushContext();
  inputString  = s;
  inputPosition = 0;
  BEGIN( DocScan );
  docYYlex();
  popContext();
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

// in this pass all @copydoc commands are resolved.
void resolveCopyDocCommands(const char *scope,QCString &docString)
{
  copyDocString.resize(0);
  copyDocScope = scope;
  inputString = docString;
  inputPosition = 0;
  docYYrestart( docYYin ); 
  BEGIN( DocCopyFind );
  docYYlex();
  docString = copyDocString;
}

//----------------------------------------------------------------------------

void parseDoc(OutputDocInterface &od,const char *fileName,int startLine,
              const char *clName,MemberDef *md,const QCString &docStr)
{

  //printf("parseDoc doc=`%s'\n",docStr.data());
  
  if (Debug::isFlagSet(Debug::Validate))
  {
    od.parseDoc(fileName,startLine,clName,md,docStr,FALSE);
  }
  else
  {
    strcpy(yyFileName,fileName);
    yyLineNr = startLine;

    QCString docString=docStr;
    resolveCopyDocCommands(clName,docString);

    yyLineNr = startLine;
    //printf("parseDoc(file=`%s',line=%d)\n",fileName,startLine);
    initParser();
    initParseCodeContext();
    exampleDoc=FALSE;           // do not cross reference with member docs
    className=clName;
    memberDef = md;
    hasParamCommand = FALSE;
    paramsFound.setAutoDelete(FALSE);
    paramsFound.clear();
    parseDocument(od,docString);

    if (md && hasParamCommand && Config_getBool("WARN_IF_UNDOCUMENTED"))
    {
      ArgumentList *al=memberDef->isDocsForDefinition() ? 
	memberDef->argumentList() :
	memberDef->declArgumentList();
      if (al)
      {
	ArgumentListIterator ali(*al);
	Argument *a;
	bool found=FALSE;
	for (ali.toFirst();(a=ali.current());++ali)
	{
	  QCString argName = memberDef->isDefine() ? a->type : a->name;
	  if (argName.right(3)=="...") argName=argName.left(argName.length()-3);
	  if (!argName.isEmpty() && paramsFound.find(argName)==0) 
	  {
	    found = TRUE;
	    break;
	  }
	}
	if (found)
	{
	  QCString scope=memberDef->getScopeString();
	  if (!scope.isEmpty()) scope+="::"; else scope="";
	  warn(memberDef->docFile(),memberDef->docLine(),
	      "Warning: The following parameters of "
	      "%s%s%s are not documented:",
	      scope.data(),memberDef->name().data(),
	      argListToString(al).data());
	  for (ali.toFirst();(a=ali.current());++ali)
	  {
	    QCString argName = memberDef->isDefine() ? a->type : a->name;
	    if (!argName.isEmpty() && paramsFound.find(argName)==0) 
	    {
	      warn_cont( "  parameter %s\n",argName.data());
	    }
	  }
	}
      }
    }
  }
}

//----------------------------------------------------------------------------

void parseText(OutputDocInterface &od,const QCString &txtString)
{
  if (txtString.isEmpty()) return;
  if (Debug::isFlagSet(Debug::Validate))
  {
    od.parseText(txtString);
  }
  else
  {
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
}

//----------------------------------------------------------------------------

void parseExample(OutputDocInterface &od,const QCString &docString, 
                  const char *fileName)
{
  if (Debug::isFlagSet(Debug::Validate))
  {
    od.parseDoc(fileName,/*startLine*/1,/*clName*/0,/*md*/0,docString,TRUE);
  }
  else
  {
    initParser();
    initParseCodeContext();
    exampleDoc=TRUE;            // cross reference with member docs
    exampleName=fileName;
    strcpy(yyFileName,fileName);
    parseDocument(od,docString); 
  }
}

//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void docYYdummy() { yy_flex_realloc(0,0); } 
}
