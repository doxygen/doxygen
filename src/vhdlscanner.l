/******************************************************************************
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
/******************************************************************************
 * Parser for VHDL subset
 * written by M. Kreis
 * supports VHDL-87/93
 * does not support VHDL-AMS 
 ******************************************************************************/
%{

// global includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <qcstring.h>
#include <qfileinfo.h>
#include <qstringlist.h>

/* --------------------------------------------------------------- */

// local includes
#include "vhdlscanner.h"
#include "vhdlcode.h"
#include "vhdldocgen.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "language.h"
#include "commentscan.h"
#include "index.h"
#include "definition.h"
#include "searchindex.h"
#include "outputlist.h"

/* --------------------------------------------------------------- */

//#define theTranslator_vhdlType theTranslator->trVhdlType
#define theTranslator_vhdlType VhdlDocGen::getVhdlType

static QStringList      qrl;
static int              openGroups;
static ParserInterface *g_thisParser;
static const char *     inputString;
static int              inputPosition;
static int              inputLen;
static int              startComment  = 0;
static QFile            inputFile;
static QCString         inbuf;
static Entry*           global_root   = 0;
static Entry*           current_root  = 0;
static Entry*           current       = 0;
static Entry*           previous      = 0;
static Entry*           functionEntry = 0;
static Entry*           lastEntity    = 0;
static Entry*           lastCompound  = 0;
static int              genPort       = 0;
static QCString         yyFileName;
static int              iFuncLine     = 1;
static bool             g_inputFromFile ;
static bool             g_lexInit     = FALSE;
static int              isBody=0;
static int              isFunc=0;
static int              yyLineNr      = 1;
static char *           g_buf         = 0;
static uint             g_bufSize     = 0;
static int              iTextCounter  = 0;
static int              iCounter      = 0;
static int              bropen        = 0;
static int              scantype      = 0;
static int              g_lastCommentContext = 0;
static bool             docBlockAutoBrief;
static char             docBlockTerm;
static int              iDocLine      = -1;
static Entry            gBlock;
static int              num_chars;

//#define YY_A_INTERACTIVE 1
#define YY_NEVER_INTERACTIVE 1
//-----------------------------------------------------------------------------
#define YY_USER_ACTION num_chars += yyleng;
static void parserInit();
static void deleteSpecChars(char* str,char *buf);
static void handleCommentBlock(const QCString &doc,bool brief);
static void newEntry();
static void initEntry(Entry *e);

static int iCodeLen;

static void makeInline()
{
  int diff=num_chars-iCodeLen;
  assert(inputLen>iCodeLen+diff);
  QCString par(&inputString[iCodeLen],diff);
  int index=par.findRev("\\endcode");
  int tt=par.length()-par.find("\n",index);
  QCString qc(&inputString[iCodeLen-tt],diff);
  index=qc.findRev("--!");
  if (index<=0) return;
  par=qc.left(index);
  /*
     fprintf(stderr,"\n-------------------------------------------------------------------------------- ");
     fprintf(stderr,"\n bytes since %d %d \n %s",num_chars,iCodeLen,par.data());
     fprintf(stderr,"\n-------------------------------------------------------------------------------- ");
   */
  gBlock.doc=par;
  gBlock.section=Entry::VARIABLE_SEC;
  gBlock.spec=VhdlDocGen::MISCELLANEOUS;
  gBlock.fileName = yyFileName;
  gBlock.endBodyLine=yyLineNr-1;
  Entry *temp=new Entry(gBlock);
  if (lastCompound)
    lastCompound->addSubEntry(temp);
  else if (lastEntity)
    lastEntity->addSubEntry(temp);
  else
  {
    temp->type="misc"; // global code
    current_root->addSubEntry(temp);
  }

  gBlock.reset();

}// makeInline

static void addSubEntry(Entry* root, Entry* e)
{
  if (e==0 || root==0) return;
  //if (isPrevDoc) 
  //{
  //  e->brief=prevDocEntry.brief;
  //  e->briefLine=prevDocEntry.briefLine;
  //  prevDocEntry.reset();
  //  isPrevDoc=FALSE;
  //}
  root->addSubEntry(e);
} 

static void bufferClear()
{
  int j;
  for (j=0;j<iCounter+1;j++)
  {
    g_buf[j]=0;
  }

  iCounter=0;
}

static void  addText (char *word, int llen)
{
  if ((uint)(iCounter + llen) > g_bufSize)
  {
    char *pTmp = (char*)realloc(g_buf,iCounter+llen+2048);
    if (pTmp)
    {
      g_buf = pTmp;
    }
    else
    {
      fprintf(stderr,"\n not enough memory for realloc\n");
      return; 
    }
  }
  while (llen>0)
  {
    g_buf[iCounter]=*word++;
    iCounter++;
    llen--;
  }    
  g_buf[iCounter]='\0'; 
} 

static void getBufText(QCString& qc,int start)
{
  while (start < iCounter)
  {
    qc+=(g_buf[start]);
    start++;
  }
}

static void lineCount()
{
  for ( const char* c = yytext ; *c ; ++c )
  {
    yyLineNr += (*c == '\n') ;
  }
}

static void deleteSpecChars(char* str,char *buf)
{
  while (*str)
  {
    if ((*str == '\t') || (*str == '\n') || (*str == '\r') || (*str == ' ')) 
    {
      *str++;
    }
    else
    {
      *buf++ = *str++;
    }
  }
  *buf='\0';
}

static void getType(Entry* p,char* text)
{
  QCString name(text);
  name=name.stripWhiteSpace();           
  if (stricmp(name.data(),"signal" )==0)
  {
    p->spec=VhdlDocGen::SIGNAL;
  }
  else if (stricmp(name.data(),"type" )==0)
  {
    p->spec=VhdlDocGen::TYPE; 
  }
  else if (stricmp(name.data(),"subtype" )==0)
  {
    p->spec=VhdlDocGen::SUBTYPE;   
  }
  else if (stricmp(name.data(),"constant" )==0)
  {
    p->spec=VhdlDocGen::CONSTANT;    
  }
  else if (stricmp(name.data(),"attribute" )==0)
  {
    p->spec=VhdlDocGen::ATTRIBUTE;
  }
  else if (stricmp(name.data(),"function" )==0)
  {
    p->spec=VhdlDocGen::FUNCTION;    
  }
  else if (stricmp(name.data(),"procedure" )==0)
  {
    p->spec=VhdlDocGen::PROCEDURE;
  }
  else if (stricmp(name.data(),"units" )==0)
  {
    p->spec=VhdlDocGen::UNITS;
  }
  else if (name.contains("shared",false) && name.contains("variable",false))
  {
    p->spec=VhdlDocGen::SHAREDVARIABLE;
  }
  else if (stricmp(name.data(),"file" )==0) 
  {
    p->spec=VhdlDocGen::VFILE;
  }
  else if (stricmp(name.data(),"group" )==0) 
  {
    p->spec=VhdlDocGen::GROUP; 
  }
  else if (stricmp(name.data(),"alias" )==0) 
  {
    p->spec=VhdlDocGen::ALIAS; 
  }
  else 
  {
    err("wrong type");
  }
  p->section=Entry::VARIABLE_SEC;
}

//-------------------------------------------------------------------------

/*
 * adds signals found in entities|records|units
 */

static void addSignals(const char* str,int line, Entry *e,const char *comment=0)
{
  //printf("===> addSignals (%s) comment='%s'\n",str,comment);
  QList<QCString> ql;
  QCString bufio;
  ql.setAutoDelete(TRUE);

  VhdlDocGen::getSigName(ql,str,bufio);
  int count = ql.count();

  QCString brief  = current->brief;
  QCString doc    = current->doc;
  Entry *tmpEntry = current;
  current = new Entry;
  initEntry(current);
  handleCommentBlock(comment,TRUE);
  if (!current->brief.isEmpty())
  {
    if (doc.isEmpty())
    {
      doc = brief;
    }
    else if (!brief.isEmpty())
    {
      doc = brief + "<p>" + doc;
    }
    brief = current->brief;
  }
  delete current;
  current = tmpEntry;
  current->brief.resize(0);
  current->doc.resize(0);
 
  if (genPort!=3) // not a unit
  {
    for (int k=1;k<count;k++)
    {
      //printf("adding '%s' '%s'\n",ql.at(0)->data(),ql.at(k)->data());
      Entry *pTemp=new Entry;
      initEntry(pTemp);
      pTemp->startLine = line;
      pTemp->bodyLine  = line;
      pTemp->name      = ql.at(k)->data();
      pTemp->section   = Entry::VARIABLE_SEC;
      pTemp->brief     = brief;
      pTemp->doc       = doc;
      pTemp->mGrpId    = current->mGrpId; // copy member group id
      QCString stSpec  = ql.at(0)->data();
      if (genPort==1) // found port
      {
	pTemp->spec    = VhdlDocGen::PORT;
	stSpec.stripPrefix(bufio.data()); 
	stSpec=stSpec.stripWhiteSpace();
	pTemp->args    = stSpec;
	pTemp->type    = bufio;
	addSubEntry(e,pTemp);
      }
      else if (genPort==2) // found record
      {
	pTemp->spec    = VhdlDocGen::RECORD;
	pTemp->type    = stSpec;
	pTemp->name.prepend(VhdlDocGen::getRecordNumber());
	delete current;
	current = new Entry(*pTemp); // make a deep copy of pTemp
	newEntry();                  // add it to lastCompound and make a new current
	delete pTemp;
      }
      else 
      {
	pTemp->spec    = VhdlDocGen::GENERIC;
	pTemp->type    = stSpec;
	addSubEntry(e,pTemp);
      }
    }// for
  }
  else // found a unit 
  {
    Entry *pTemp=new Entry;
    initEntry(pTemp);
    QCString tt(str);
    QStringList ql=QStringList::split("=",tt,FALSE);
    pTemp->spec      = VhdlDocGen::UNITS;
    pTemp->section   = Entry::VARIABLE_SEC;
    pTemp->startLine = line;
    pTemp->bodyLine  = line;
    pTemp->brief     = brief; // adds brief description to the unit member
    pTemp->doc       = doc;   // adds doc to the unit member
    pTemp->type      = ql[1];
    pTemp->name      = ql[0].stripWhiteSpace();
    pTemp->name.prepend(VhdlDocGen::getRecordNumber());
    delete current;
    current = new Entry(*pTemp); // make a deep copy
    newEntry();                  // add it to lastCompound
    delete pTemp;
  }
}                                              

/*
 * this function parses a process prototype
 * and adds the signal to the process
 */

static void parseProcessProto()
{
  QStringList ql;
  QCString qcs;
  bool sem=FALSE;
  //Entry* ppEntry=new Entry;
  //ppEntry->fileName=yyFileName;
  //processEntry=ppEntry;
  QCString name; 
  scantype=0;
  getBufText(qcs,0);
  if (qcs.contains('(') != qcs.contains(')')) return; 
  VhdlDocGen::deleteAllChars(qcs,'\n');
  VhdlDocGen::parseProcessProto(qcs,name,ql);
  current->section=Entry::FUNCTION_SEC;
  //current->stat=TRUE;
  current->spec=VhdlDocGen::PROCESS;
  current->startLine=iFuncLine;
  current->bodyLine=iFuncLine;
  current->fileName=yyFileName;
  if (!name.isEmpty())
  {
    current->name=name.stripWhiteSpace();
  }
  else // found an anonymous process, so we add a generated name  
  {
    current->name=VhdlDocGen::getProcessNumber(); 
  }

  current->args+=" ( "; 
  if (!ql.isEmpty())
  {
    QValueList<QString>::Iterator iter = ql.begin();
    for ( ; iter != ql.end(); ++iter)
    {
      if (sem)
      {
        current->args+=',';
      }
      Argument *arg=new Argument;
      arg->name=((QCString)*iter).stripWhiteSpace();    
      current->argList->append(arg);
      current->args+=(QCString)*iter; 
      sem = TRUE;
    }    
  }
  current->args+=" ) ";
  bufferClear();
}//parseProcessProto


/*
 * parses a function|procedure protoype
 */

static void parseFunctionProto()
{
  QCString name,ret,qcs,temp;
  bool sem=FALSE;
  QList<Argument> ql;
  ql.setAutoDelete(TRUE);
  getBufText(qcs,0);
  if (qcs.contains('(') != qcs.contains(')')) 
    return; // function without a prototype 
  if (qcs.contains("function",FALSE)==0 && qcs.contains("procedure",FALSE)==0) 
    return; 
  qcs=qcs.stripWhiteSpace();
  temp=qcs.lower();
  if (temp.stripPrefix("impure"))
  {
    current->exception="impure";
    qcs=qcs.remove(0,6);
  }
  else if (temp.stripPrefix("pure")) 
  {
    current->exception="pure";
    qcs=qcs.remove(0,4);
  }

  VhdlDocGen::parseFuncProto(qcs.data(),ql,name,ret); 
  //printf("parseFuncProto(%s)=%s,%s\n",qcs.data(),name.data(),ret.data());
  VhdlDocGen::deleteAllChars(name,';');
  current->name=name;
  current->startLine=iFuncLine;
  current->bodyLine=iFuncLine;

  int count = ql.count(); 

  current->args+" ( "; 
  for (int k=0;k<count;k++)
  {
    if (sem)
    {
      current->args+=",";
    }
    Argument *arg=new Argument;
    Argument *hh=(Argument*)ql.at(k);
    arg->name=hh->name;
    arg->type=hh->type;
    arg->defval=hh->defval;
    arg->attrib=hh->attrib;
    current->argList->append(arg);
    current->args+=hh->name;
    sem=TRUE;
  }
  current->args+" )";

  if (!ret.isEmpty()) 
    current->spec=VhdlDocGen::FUNCTION;
  else
    current->spec=VhdlDocGen::PROCEDURE;

  current->section=Entry::FUNCTION_SEC;
  current->type=ret;
  //addSubEntry(ee,ppEntry); 
  if (lastCompound)
  {
    lastCompound->addSubEntry(current);
    current = new Entry;
    initEntry(current);
  }
  else
  {
    newEntry();
  }
  bufferClear();
}//parseFunctionProto

static Entry* getEntryAtLine(const Entry* ce,int line)
{
  EntryListIterator eli(*ce->children());
  Entry *found=0;
  Entry *rt;
  for (;(rt=eli.current());++eli)
  {
    if (rt->bodyLine==line)
    {
      found=rt;
    } // if
    if (!found) 
    {
      found=getEntryAtLine(rt,line);
    }
  }
  return found;
}// getEntryAtLine

//-------------------------------------------------------------------------

static void parserInit()
{
  iCounter=0; 
  iTextCounter=0;
  yyLineNr=1;
  current=0;
  previous=0;
  isFunc=0;
  isBody=0;
  scantype=0;
  lastCompound=0;
  lastEntity=0;
  bropen=0;
  openGroups=0;
  iDocLine=-1;
  qrl.clear();
  num_chars=0;

  if (!g_lexInit) 
  {
    VhdlDocGen::init();
  }

  g_bufSize=inputFile.size()+1024;
  if (g_buf==0) free(g_buf);
  g_buf=(char*)(calloc(g_bufSize,sizeof(char)));

  if (g_buf==0)
  {
    fprintf(stderr,"\n not enough memory");
    return;
  }
  g_buf[g_bufSize-1]='\0';
}

bool VHDLLanguageScanner::needsPreprocessing(const QCString &)
{
  return FALSE;
}


void VHDLLanguageScanner::resetCodeParserState()
{

}

#undef    YY_INPUT
#define    YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  if (g_inputFromFile)
  {
    c = inputFile.readBlock(buf,max_size);
    if (c==-1) yy_fatal_error("input in flex scanner failed");
  }
  else
  {
    while ( c < max_size && inputString[inputPosition] )
    {
      *buf = inputString[inputPosition++] ;
      c++; 
      buf++;
    }
  }
  return c;
}



%}


  /* start command character */
  /* -------------- VHDL SECTION -----------------------------------*/

B                       [ \t]
CR                      [\r\n]
BR                      [ \t\n\r]
DIGIT                   [0-9]
LOWER_CASE_LETTER       [a-z]
UPPER_CASE_LETTER       [A-Z]
LETTER                  [a-zA-Z_0-9]
SPACE_CHARACTER         [ \t]
SPECIAL_CHARACTER       [#&'()*+,\-\./:;<=>_|]
OTHER_SPECIAL_CHARACTER [~!$§%?@\[\\\]^{}]
BASIC_GRAPHIC_CHARACTER {UPPER_CASE_LETTER}|{DIGIT}|{SPECIAL_CHARACTER}|{SPACE_CHARACTER}
GRAPHIC_CHARACTER       {BASIC_GRAPHIC_CHARACTER}|{LOWER_CASE_LETTER}|{OTHER_SPECIAL_CHARACTER}
EXTENDED_CHARACTER      [\\]{GRAPHIC_CHARACTER}*[\\] 

NAME                    ({LETTER}[a-zA-Z0-9_.]*)|{EXTENDED_CHARACTER}
STRING_LITERAL          \"{GRAPHIC_CHARACTER}*\"
FUNCNAME                ([a-zA-Z"][*+\-_a-zA-Z0-9"\/=<>]*)|{EXTENDED_CHARACTER}
DIGITS                  [0-9]+|[0-9]+"."[0-9]+|[0-9]+"#"[0-9_a-fA-F\+\.]+"#"
COMMENT                 "--"[^\n]*
LABELID                 [a-z_A-Z][^\;]*";"({B}*{COMMENT})*
PROTO                   [ (]*
TEXTT                   "--"[^\/\@\*\#][^\n]*
PROC                    ("function"|"procedure")
ENDE                    ({BR}*("end"){BR}*{PROC}*{BR}*[;]{1})
ENDEFF                  ("if"|"case"|"loop"|"generate"){BR}*[;]
ENDE3                   ({BR}*("end"){BR}*{PROC}*{BR}*{FUNCNAME}{BR}*[;])|{ENDE}
ENDFUNC                 {B}*"end"{BR}*{PROC}*{BR}*{FUNCNAME}{BR}*[;]
FUNCIMPURE              "impure"|"pure"
FUNCPROC                ^{B}*{FUNCIMPURE}*{BR}*("function"|"procedure"){B}*
ARCHITECTURE            ("architecture"){BR}+{NAME}{BR}*("of")
   /* Removed due to bug 538239
      POST         "postponed"
      PROCESS      ({BR}*{FUNCNAME}{B}*[:]{BR}*({POST}{BR}+)?("process"){BR}*{PROTO})|("process"){BR}*("("){BR}*{PROTO}|[^a-zA-Z]("process"){CR}|[^a-zA-Z]("process"){BR}+("is")
   */
PROCESS                 ({B}*{FUNCNAME}{B}*:{BR}*)?({B}*("postponed"){BR}+)?{B}*("process"){BR}*{PROTO}

ENDPROCESS              ("end"){BR}*("postponed")*("process"){BR}*{FUNCNAME}*{BR}*[;]
LIBUSE                  ^{B}*("use"|"library"){BR}+
ENTITY                  ^{B}*("component"|"entity"|"package"){BR}+
PBODY                   ("package"){B}+("body"){BR}+{NAME}
SHARED                  ("shared"){BR}+("variable")
SIGTYPES                ^{B}*({SHARED}|"alias"|"file"|"group"|"subtype"|"type"|"constant"|"attribute"|"signal"|"units"){BR}+
CONFIG                  ("configuration"){BR}+{NAME}{BR}*("of"){BR}+{NAME}{BR}+"is"

ALLTYPESMAP             {B}*[_a-zA-ZA_Z0-9.() ]*{B}*
MAPCOMPONENT            ({ALLTYPESMAP}{BR}*[:]{BR}*("component"|"configuration")*{ALLTYPESMAP}{BR}*{TEXTT}*{BR}*("port"|"generic"){BR}*("map"){BR}*("("){1})
MAPCOMPONENT1           ({ALLTYPESMAP}{BR}*[:]{BR}*("entity"){BR}*{ALLTYPESMAP}{BR}*("port"|"generic"){BR}*("map"){BR}*("("){1})

BRACEOPEN               [(]{1}
BRACECLOSE              [)]{1}

ALLID                   [^;()\t ]

/* VHDL 2001 */
ENDPROTECTED            ("end"{BR}+"protected"{BR}+{NAME}{BR}*";")|("end"{BR}+"protected"{BR}*";")
ENDPROTECEDBODY         "end"{BR}+"protected"{BR}+"body"{BR}+{NAME}


%option noyywrap

  /* language parsing states */

%x Start
%x Comment
%x FindTypeName
%x ParseType 
%x ParseRecord
%x ParseUnits
%x ParseProcess
%x ParseFunc
%x FindName
%x FindEntityName
%x FindGenPort
%x FindTypes
%x FindSigName
%x FindFuncName
%x FindBegin

%%

<Start>{ENDPROTECTED}|{ENDPROTECEDBODY}  {
 lineCount();
}



<Start>{CONFIG} { // found configuration
 
  QCString qcs(vhdlscanYYtext);
  current->name=VhdlDocGen::getIndexWord(qcs,1);
  current->type=VhdlDocGen::getIndexWord(qcs,3);
  current->startLine=yyLineNr;
  current->bodyLine=yyLineNr;
  current->section=Entry::VARIABLE_SEC; 
  current->spec=VhdlDocGen::CONFIG;
  current->args="configuration";
  newEntry();
  BEGIN(Start);
}

<Start>{SIGTYPES}  { // found type constant|type|attribute and so on..
    bropen=0;
    lineCount();

    bufferClear();
    //pEntry=current;
    getType(current,yytext);
    current->bodyLine=yyLineNr;
    if (current->spec==VhdlDocGen::UNITS)
    {
      //addSubEntry(current,pEntry);
      current->startLine=yyLineNr;
      current->bodyLine=yyLineNr;
      newEntry(); // adds the unit to the lastCompound
      genPort=3;
      BEGIN(ParseRecord);
    }
    else    
    {
      BEGIN(FindTypeName);
    }
  }

<Start>{ARCHITECTURE} {     //found architecure
  lineCount();
  bropen=0;
  bufferClear();
  isBody=0;
  lastCompound = current;
  QCString curName=VhdlDocGen::getIndexWord(yytext,1);
  current->section=Entry::CLASS_SEC; //Entry::CLASS_SEC;
  current->spec=VhdlDocGen::ARCHITECTURE;
  current->protection=Private;
  current->name=curName;
  current->fileName=yyFileName;
  current->startLine=yyLineNr;
  current->bodyLine=yyLineNr;      
  //printf("-> Architecture at line %d\n",yyLineNr);
  BEGIN(FindName);
}


<Start>{PROCESS} {          //found process
  lineCount();
  iFuncLine=yyLineNr;
  bropen=0;
  //printf("--> Process: line=%d\n",yyLineNr);
  bufferClear();
  addText(yytext,yyleng);
  QCString qcs(yytext);
  if (qcs.contains('('))
  {
    bropen=1;
    scantype=2;
    BEGIN(ParseType);
  }
  else 
  {
    //                      iFuncLine--;
    parseProcessProto();  
    BEGIN(ParseProcess);
  }
}                                                                               

<Start>{LIBUSE}{BR}*       {      //  found library or package
  bropen=0;
  bufferClear();
  isBody=0;
  QCString qcs=QCString(yytext);
  // lowerString(qcs);
  qcs=qcs.stripWhiteSpace();
  if (stricmp(qcs.data(),"use")==0)
  {
    current->spec=VhdlDocGen::USE;
    current->type="package";
  }
  else 
  {
    current->spec=VhdlDocGen::LIBRARY;
    current->type="library";
  }
  current->section=Entry::VARIABLE_SEC;
  current->bodyLine=yyLineNr;
  lineCount();
  BEGIN(FindName);
}

<Start>{FUNCPROC}       {   // found a new function|procedure
  lineCount();
  iFuncLine=yyLineNr;
  bropen=0;
  bufferClear();
  isFunc=1;
  addText(yytext,yyleng);
  BEGIN(FindFuncName);
}

<Start>{ENTITY}  {     // found entity|component|package
  lineCount();
  //printf("--> Entity at line %d\n",yyLineNr);

  bropen=0;  
  bufferClear();
  QCString word(yytext);
  word=word.lower();  
  word=word.stripWhiteSpace();

  if (strcmp(word.data(),"entity")==0)
  {
    isBody=0;
    scantype=0;                                    
    lastCompound=0;
    current->section=Entry::CLASS_SEC;
    current->spec=VhdlDocGen::ENTITY;
    current->protection=Public;
    current->bodyLine=yyLineNr;
    current->fileName=yyFileName;
    lastEntity = current;
  }
  else if (strcmp(word.data(),"component")==0)
  {
    current->section=Entry::VARIABLE_SEC;
   // current->stat=TRUE;
    current->spec=VhdlDocGen::COMPONENT;
    current->bodyLine=yyLineNr;
    scantype=1;  
  }
  else if (strcmp(word,"package")==0)
  {
    isBody=0;
    scantype=0;
    lastCompound = current;
    current->section=Entry::CLASS_SEC;
    current->spec=VhdlDocGen::PACKAGE;
    current->protection=Package; //VhdlDocGen::PACKAGE;
    current->bodyLine=yyLineNr;
    current->fileName=yyFileName;
  }
  else
    err("\n found wrong component at line [%d]",yyLineNr); 

  BEGIN(FindEntityName);
}

<Start>{MAPCOMPONENT}|{MAPCOMPONENT1}  { // found component instantiation 

 // lineCount();
  QCString type;
  QCString tt(yytext);
  QRegExp regg("[\\s:.()-]");
  QStringList qsl=QStringList::split(regg,tt,false);

  // consider upper/lower-case letters
  QStringList qsltemp=QStringList::split(regg,tt.lower(),false);
  int index=qsltemp.findIndex(QCString("entity"))+1;
  index+=qsltemp.findIndex(QCString("component"))+1;
  index+=qsltemp.findIndex(QCString("configuration"))+1;
  int len=qsltemp.count();

  current->spec=VhdlDocGen::COMPONENT_INST;
  current->section=Entry::VARIABLE_SEC;
  current->startLine=yyLineNr;
  current->bodyLine=yyLineNr;

  if (index!=0 && tt.contains(')')==0)     // found component instantiation xxx: configuration/component/entity yyy
  {
    current->type=(QCString)qsl[len-3];
  }
  else if (index!=0 && tt.contains(')'))   // found component instantiation xxx: entity www.yyy(zzz)
  {
    current->type=(QCString)qsl[len-4];
  }
  else
  {
    current->type=(QCString)qsl[1];          // found component instantiation xxx:yyy
  }

  current->name=QCString(qsl[0]);
  if (lastCompound)
  {
    if (!VhdlDocGen::foundInsertedComponent(current->type,lastCompound))
    {   
      BaseInfo *bb=new BaseInfo(current->type,Public,Normal);
      lastCompound->extends->append(bb);
    }
    lastCompound->addSubEntry(current);
    current = new Entry;
    initEntry(current);
  }
  else
  {
    newEntry();
  }
 lineCount();

} 

<Start>{CR}*    {
  lineCount();
  addText(yytext,yyleng);
  BEGIN(Start); 
}

<ParseProcess>[^;()] {
  // eat process body
  lineCount();
  BEGIN(ParseProcess);
}

<ParseProcess,ParseType>{ENDPROCESS} {  // find end of process
  lineCount();
  current->endBodyLine=yyLineNr;
  //printf("Process: start=%d end=%d\n",current->bodyLine,current->endBodyLine);
  if (lastCompound)
  {
    lastCompound->addSubEntry(current);
    current = new Entry;
    initEntry(current);
  }
  else
  {
    newEntry();
  }
  BEGIN(Start);
}


<ParseUnits>{BR}* { 
  lineCount();
}

<ParseUnits>{B}*[a-z_][^\n;]* { // parse record|unit body
  lineCount();
  QCString zz(yytext);
  addSignals(zz.data(),yyLineNr,current);
  BEGIN(ParseUnits);
}

<FindName>{NAME} {  // found entity|architecture|component name
  lineCount();

  QCString qcs(yytext);
  qcs=qcs.stripWhiteSpace();
  if (current->spec==VhdlDocGen::USE || current->spec==VhdlDocGen::LIBRARY)
  {
    int j=qcs.length();
    int i=qcs.find(".");
    if (i>0)
      qcs=qcs.right(j-i-1);
    i=qcs.find(".");
    if (i>0)
      qcs=qcs.left(i);            
  /*  
    -- Consider the case we have more than one entity in one file.Each entity has its own package/library
    -- declaration. In this case package yyy will be added [with newEntry()] to architecture aaa !! instead to entity
    -- bbb. We must place these constructs to current_root and the function mapLibPackage() will finish the rest.
    
    -- package xxx;
    -- entity aaa
    --  ....
    -- end entity aaa;
    -- architecture aaa
    --     ...
    -- end architecture aaa;
    -- package yyy;
    -- entity bbb;
    */
      
    current->name=qcs;                 
    Entry *copy=new Entry(*current);
    current->reset();
    addSubEntry(current_root,copy);   //  insert into entry list  with mapLibPackage()           
  }
  else if (current->spec==VhdlDocGen::ARCHITECTURE)
  {
    //current->name+=qcs.lower();
    current->name.prepend(qcs+"::");

    //if (lastEntity)
    //{
      // inherit private inheritance relation between entity and architecture
      //if (!VhdlDocGen::foundInsertedComponent(current->name,lastEntity))
      //{
	//	BaseInfo *bb=new BaseInfo(current->name,Private,Normal);
	//	lastEntity->extends->append(bb);    
      //}
    //}

  }
  else if (current->spec==VhdlDocGen::PACKAGE_BODY)
  {
    current->name+=qcs;
  }
  else
  {
    current->name+=qcs;
  }
  if (!(current->spec==VhdlDocGen::USE || current->spec==VhdlDocGen::LIBRARY))
  newEntry();

  BEGIN(Start);
}

<FindFuncName>{FUNCNAME}    { // found name of a process|function|procedure
  lineCount();

  addText(yytext,yyleng);
  BEGIN(ParseType);
}

<FindTypeName>{NAME}{BR}* {
  lineCount();                                                            
  current->name=QCString(yytext);
  BEGIN(ParseType);
}


<ParseType>("is"){BR}+("protected"){BR}+("body") {lineCount(); BEGIN(Start); }

<ParseType>("is"){BR}+("protected"){BR}+ { 
 lineCount();
 current->section=Entry::VARIABLE_SEC;
 current->spec=VhdlDocGen::TYPE;
 current->type="protected"; 
 newEntry(); 
  BEGIN(Start);
}




<ParseType>("is"){BR}*("record") { // find record
  lineCount();
  if (isFunc)
  {
    BEGIN(Start);
  }

  genPort=2;
  current->section=Entry::VARIABLE_SEC;
  current->spec=VhdlDocGen::RECORD;
  addText(yytext,yyleng); 
  newEntry(); // adds the record to the last compound
  BEGIN(ParseRecord);
}

<ParseRecord>{BR}* {
  lineCount();
 }

<ParseRecord>("end"){BR}*("record"){BR}*{LETTER}*{BR}*[;]|("end"){BR}*("units"){BR}*[;]  {
  lineCount();
  genPort=0;
  bufferClear();
  BEGIN(Start);
}

<ParseRecord>[a-z_A-Z0-9][^\n;]*";"({B}*{COMMENT})* { // parse record body
  lineCount();
  QCString comment;
  QCString zz(yytext);
  VhdlDocGen::deleteAllChars(zz,';'); //delete ; in unit construct
  if (zz.contains("--!"))
  {
    QStringList ql=QStringList::split("--!",zz,FALSE);
    comment = ql[1];
    zz = ql[0];
  }
  else if (zz.contains("--"))
  {
    QStringList ql=QStringList::split("--",zz,FALSE);
    zz = ql[0];
  }
  initEntry(current);
  addSignals(zz,yyLineNr,current,comment);
  addText(yytext,yyleng); 
  BEGIN(ParseRecord);
}

<ParseType>{BR}+("is"){BR}+|{BR}+("is"){B}*"--" { // found a new function  in an architecture ?
  addText(yytext,yyleng);
  lineCount();
  QCString ttt;
  bool bb=TRUE;
  getBufText(ttt,0);
  if (ttt.contains("--"))
  { 
    unput('-');unput('-'); 
    VhdlDocGen::deleteCharRev(ttt,'-');
    VhdlDocGen::deleteCharRev(ttt,'-');
  }
  if (ttt.contains('(') != ttt.contains(')'))
  {
    bb=FALSE;
  }
  bool ss = VhdlDocGen::isFunctionProto(ttt);
  //printf("VhdlDocGen::isFunctionProto(%s)=%d\n",ttt.data(),ss);
  if (ss && bb)
  {
    bufferClear();
    addText(ttt.data(),ttt.length());
    functionEntry=0;
    //eFuncBody=new Entry;
    ::parseFunctionProto();
  }
  bufferClear();  
  BEGIN(ParseType);
}


<ParseType>[^;()\t ] {
  lineCount();      
  addText(yytext,yyleng); 
  BEGIN(ParseType);
}

<ParseType>{BRACEOPEN} {
  lineCount();
  bropen++;
  addText(yytext,yyleng);
  BEGIN(ParseType);
}

<ParseType>{BRACECLOSE} {
  lineCount();
  bropen--;
  addText(yytext,yyleng);
  if (bropen==0 && scantype==2) // process
  {
    ::parseProcessProto();
    BEGIN(ParseProcess);
  } // if
  else
  {
    BEGIN(ParseType);
  }
}


<ParseType>{ENDE}|{ENDFUNC} { // found end of function|process
  QRegExp regg("[\\s;]");
  lineCount();
  QCString tt(yytext);
  tt=tt.lower();
  QStringList ql=QStringList::split(regg,tt,FALSE);
  int index=ql.findIndex(QCString("if"))+1;
  index+=ql.findIndex(QCString("case"))+1;
  index+=ql.findIndex(QCString("loop"))+1;
  index+=ql.findIndex(QCString("generate"))+1;
  bufferClear();
  if (index==0)
  {
    if (isFunc)
    {
      Entry* pFunc=getEntryAtLine(current_root,iFuncLine);
      if (pFunc && pFunc->section==Entry::FUNCTION_SEC)
      {
	pFunc->endBodyLine=yyLineNr;
      }
      isFunc=0;
      BEGIN(Start);
    }
  }
}

<ParseFunc>[^;()] {
  // eat process body
  lineCount();
  BEGIN(ParseFunc);
 }

<ParseFunc>{ENDE3} {
  QRegExp regg("[\\s;]");
  lineCount();
  QCString tt(yytext);
  tt=tt.lower();
  QStringList ql=QStringList::split(regg,tt,FALSE);
  int index=ql.findIndex(QCString("if"))+1;
  index+=ql.findIndex(QCString("case"))+1;
  index+=ql.findIndex(QCString("loop"))+1;
  index+=ql.findIndex(QCString("generate"))+1;
  bufferClear();
  if (index==0 && isFunc)
  {
    Entry* pFunc=getEntryAtLine(current_root,iFuncLine);
    if (pFunc && pFunc->section==Entry::FUNCTION_SEC)
    {
      pFunc->endBodyLine=yyLineNr;
    }
    isFunc=0;
    BEGIN(Start);
  }
}

<ParseType>";" {
  lineCount();
  addText(yytext,yyleng);    
  if (bropen==0 && !(isFunc==1 && isBody==1) )
  {
    if (isFunc)
    {
      parseFunctionProto();
      bufferClear();
      if (lastCompound && lastCompound->spec==VhdlDocGen::PACKAGE) 
      {
        isFunc=0;
        BEGIN(Start);
      }
      else
      {
        BEGIN(ParseFunc); 
      }
    }//if
    else
    {
      QCString qcs;
      getBufText(qcs,0);
      qcs=qcs.stripWhiteSpace();
      current->section=Entry::VARIABLE_SEC;
      current->type+=qcs.data();  

      if ((current->spec==VhdlDocGen::SIGNAL   || 
	   current->spec==VhdlDocGen::CONSTANT || 
	   current->spec==VhdlDocGen::TYPE     || 
	   current->spec==VhdlDocGen::SUBTYPE  ||
	   current->spec==VhdlDocGen::SHAREDVARIABLE
	  ) &&  
	  qcs.stripPrefix(","))
      {
	QList<QCString> ql;
	ql.setAutoDelete(TRUE);
	QCString buffer;
	if (current->spec==VhdlDocGen::SUBTYPE || 
	    current->spec==VhdlDocGen::TYPE
	   )
	{
	  VhdlDocGen::getSigTypeName(ql,qcs.data(),buffer);
	}
	else
	{
	  VhdlDocGen::getSigName(ql,qcs.data(),buffer);
	}
	QCString doc = current->doc;
	QCString brief = current->brief;
	if (ql.count()>0) 
	{
	  for (uint j=1;j<ql.count();j++)
	  {
	    Entry *ppt = new Entry;
            initEntry(ppt);
	    ppt->type     += ql.at(0)->data();  
	    ppt->section   = Entry::VARIABLE_SEC;
	    ppt->spec      = current->spec;        
	    ppt->name     += ql.at(j)->data();
	    ppt->bodyLine  = yyLineNr;
	    ppt->startLine = yyLineNr;
	    ppt->brief     = brief;
	    ppt->doc       = doc;
	    if (lastCompound)
	    {
	      lastCompound->addSubEntry(ppt);
	    }
	    else
	    {
	      current->addSubEntry(ppt);
	    }
	  }
	  current->type=ql.at(0)->data(); 
	  ql.clear();   
	}
      }
      if (lastCompound)
      {
	lastCompound->addSubEntry(current);
	current = new Entry;
	initEntry(current);
      }
      else
      {
	newEntry();
      }
      isFunc=0;
      bufferClear();
      BEGIN(Start);
    }
  }
  else
  {
    BEGIN(ParseType);
  }
}

<ParseType>{BR}* {
  lineCount();
  addText(yytext,yyleng);
  BEGIN(ParseType);
}

<FindEntityName>{NAME} {    // found name of an entity/architecture/package
  lineCount();
  QCString qcs(yytext);
  qcs=qcs.stripWhiteSpace();
  qcs=qcs.lower();
  if (strcmp(qcs.data(),"body")==0) // found package body
  {
    current->spec=VhdlDocGen::PACKAGE_BODY;
    current->section=Entry::CLASS_SEC;
    current->protection=Protected;
    current->name+=QCString("_");
    isBody=1;
    BEGIN(FindName);
  }
  else if (scantype==1) // found a component
  {
    QCString qq(yytext);
    qq=qq.stripWhiteSpace();
    //qq=qq.lower();

    current->name=qq;
    qq=qq.lower();
    if (lastCompound)
    {
      if (lastCompound->spec==VhdlDocGen::PACKAGE)
      {
        if (!VhdlDocGen::foundInsertedComponent(qq,lastCompound))
        {  
	  BaseInfo *bb=new BaseInfo(qq,Private,Normal);
	  lastCompound->extends->append(bb);    
        }
      } 

      lastCompound->addSubEntry(current);
      current = new Entry;
      initEntry(current);
    }
    else
    {
      newEntry();
    }
    BEGIN(Start);
  } 
  else
  {
    QCString qq(yytext);
    qq=qq.stripWhiteSpace();
    current->name=qq;
    newEntry();
    //QCString qreal=QCString(yytext);
    BEGIN(Start);
  }
}

<Start>{B}*("generic"|"port"){BR}*[(]+ { // found generic|port in entity
    QCString genp(yyleng+1);
    deleteSpecChars(yytext,genp.data());
    VhdlDocGen::deleteCharRev(genp,'(');

    if (stricmp(genp.data(),"port" )==0)
    {
      genPort=1;
    }
    else 
    {
      genPort=0;
    }

    bropen=1;
    bufferClear();
    lineCount();
    BEGIN(FindSigName);
}

<FindSigName>{BRACECLOSE} { 
    lineCount();
    bropen--;
    addText(yytext,yyleng); 
    if (bropen==0)
    {
      bufferClear();
      BEGIN(Start);
    }
    else
    {
      BEGIN(FindSigName);
    }
}

<FindSigName>{LABELID} {  // found signals in entity
  QCString line(yytext);

  // note that line can be something like:
  // "var1, var2, var3 : in std_logic_vector(8 downto 0); --! Some comment"

  // but also
  // "var4 --! Some comment
  // );"
  // which marks the end of a port

  // and also
  // "-- Some comment
  // var1 : in std_logic;"

  //printf("--> labelid='%s'\n",line.data());
  QStringList ql;
  QCString comment;
  int openCount=line.contains('(');
  int closeCount=line.contains(')');
  int semi = line.find(';');
  int pos  = line.find("--");
  int pos1 = line.find("--!");
  if (pos!=-1 && pos<pos1) // strip normal comment before special one
  {
    line = line.remove(pos,pos1-pos);
  }
  //printf("=> signal: line='%s'\n",line.data());
  if (semi!=-1 && pos!=-1)
  {
    int eol = line.findRev('\n');
    //printf("pos=%d eol=%d\n",pos,eol);
    if (eol>=pos+2)
    {
      QRegExp re("\\n[\\s]*--!"); // comment continuation
      comment=line.mid(pos+2,eol-pos-2);
      //printf("Comment: '%s'\n",comment.data());
      int p,l;
      while ((p=re.match(comment,0,&l))!=-1)
      {
	comment.remove(p,l);
      }
      line=line.left(pos)+line.right(line.length()-eol);
    }
    else
    {
      comment=line.mid(pos+2);
      line=line.left(pos);
    }
    comment.stripWhiteSpace();
    // must subtract "(" and ")" in comments because they are used for determining the
    // end of a port/generic construct
    openCount-=comment.contains('(');
    closeCount-=comment.contains(')');
    if (!comment.stripPrefix("!")) // not a special comment
    {
      comment.resize(0);
    }
  }
  else
  {
    //printf("no ; or --: pos=%d semi=%d\n",pos,semi);
  }
  int diff=openCount-closeCount;
  if (diff<0)
  {
    VhdlDocGen::deleteCharRev(line,')');
  }

  if (scantype!=1) // not a component
  {  
    addText(yytext,yyleng);
    addSignals(line,yyLineNr,lastEntity,comment);
  } 

  lineCount();

  if ((bropen+openCount-closeCount)==0)
  {
    bufferClear();
    BEGIN(Start);
  }
}


<FindSigName>{BRACEOPEN} { 
  lineCount();
  bropen++;
  addText(yytext,yyleng);
}


<FindSigName>{CR}        { 
  lineCount();
  addText(yytext,yyleng);
  //BEGIN(FindSigName);
}


<*>^{B}*("for ")[^;]* {
  //printf("\n found for[%s] [%d]",yytext,yyLineNr);
  lineCount();
}

<*>{DIGITS}                {   // found digit
  addText(yytext,yyleng);
  lineCount();
}

<*>{STRING_LITERAL} {
  // Make sure string literals get transfered to the output
  // We have to match these because the comment characters (--)
  // can exist inside a string literal.
  // We shouldn't have to call lineCount because newlines
  // are not allowed inside string literals
  addText(yytext,yyleng);
}

   /*
<*>{BR}*"--!"{B}*"@}"    { // end group
  if (current) 
  {
    Entry *pg=new Entry;  
    addSubEntry(current,pg);
    pg->startLine=yyLineNr;
    pg->name="endgroup";
  }
  lineCount();
}

<*>{BR}*"--!"{B}*"@{"     {  // start group
  if (current) 
  {
    Entry *pg=new Entry;  
    addSubEntry(current,pg);
    pg->startLine=yyLineNr;
    pg->name="startgroup";
  }
  lineCount();
}
   */

<*>{BR}*"--!"[^{}\n][^\n]*\n/{B}*"--!" { // multi line comment
  if (iDocLine==-1) iDocLine=yyLineNr;
  // signal clk :in std_logic; --!@brief global clock
  // --!@brief  global reset
  // signal reset:in std_logic;
  // these two comments are detected as a multi line comment
  QCString qc(yytext);
  int len=qc.contains('\n')+yyLineNr-1;

  if (YY_START!=Comment) // Start of the comment block
  {
    bufferClear();
    iTextCounter=0;
    startComment=yyLineNr;
    g_lastCommentContext=YY_START;
  }

  Entry* pTemp=getEntryAtLine(current_root,len);
  if (pTemp)
  { // found one line comment, add it to the entry on this line
    pTemp->briefLine=yyLineNr;
    pTemp->brief+=yytext;
    VhdlDocGen::prepareComment(pTemp->brief);
  }
  else 
  {
    addText(yytext,yyleng);
  }
  lineCount();
  BEGIN(Comment);
}

<Comment>^{B}*"--!"[^\n]* {
  if (iDocLine==-1) iDocLine=yyLineNr;
  addText(yytext,yyleng);
  lineCount();
}

<Comment>.|\n {
  // found end of comment block
  QCString qcs;
  getBufText(qcs,iTextCounter);
  VhdlDocGen::prepareComment(qcs);
  int ii =qcs.find("\\code");
  if (ii>0)
  {
    iCodeLen=num_chars;

    gBlock.reset();
    int len=qcs.length();
    QCString name=qcs.right(len-ii);
    name=VhdlDocGen::getIndexWord(name.data(),1);
    if (!name)
      gBlock.name="misc"+ VhdlDocGen::getRecordNumber(); 
    else
      gBlock.name=name;
    qcs=qcs.left(ii);
    gBlock.startLine=yyLineNr+1;
    gBlock.bodyLine=yyLineNr+1;
    gBlock.brief+=qcs;
    iTextCounter=0;
  }

  if (ii==-1)
  {
    handleCommentBlock(qcs,FALSE);
  }
  bufferClear();
  unput(*yytext);
  BEGIN(g_lastCommentContext);
}

<*>"--!"[^\n]* { // one line comment
  if (iDocLine==-1) iDocLine=yyLineNr;
  QCString qcs(yytext);
  int j=qcs.find("--!");
  qcs=qcs.right(qcs.length()-3-j);
  bool isEndCode=qcs.contains("\\endcode");

  if (isEndCode)
    makeInline();
  //printf("--> handleCommentBlock line %d\n",yyLineNr);
  Entry* pTemp=getEntryAtLine(current_root,yyLineNr);

  if (!isEndCode)
  {
    if (pTemp)
    {
      pTemp->briefLine=yyLineNr;
      pTemp->brief+=qcs;
      iDocLine=-1;
    }
    else
    {
      handleCommentBlock(qcs,TRUE);
    }
  } 

  bufferClear();
}// one line


<*>{COMMENT} {
}

<*>\n                    {
  lineCount();
  addText(yytext,yyleng);
  //         printf("\n new-line [%d]",yyLineNr);
  BEGIN(Start);
}

<*>{NAME} {
  addText(yytext,yyleng);
  lineCount();
}

<*>{B}*	{
  addText(yytext,yyleng);
  lineCount();
}

<*>.                    {
  addText(yytext,yyleng);
  lineCount();
}


%%

static void initEntry(Entry *e)
{
  e->fileName = yyFileName;
  initGroupInfo(e);
}

static void newEntry()
{
  // Add only enties/architectures/packages to root 
  // and signals to  classes where they were found
  // ENTITY dlatch_93 IS -- VHDL'93-Syntax !!!
  //      PORT (d, clk : IN bit;
  //      q, qbar : OUT bit);
  //      GROUP path IS (SIGNAL, SIGNAL);
  //      GROUP d_to_q : path (d, q);
  //      ATTRIBUTE propagation : time;
  //  END dlatch_93;

  if (current->spec==VhdlDocGen::ENTITY || 
      current->spec==VhdlDocGen::PACKAGE || 
      current->spec==VhdlDocGen::ARCHITECTURE || 
      current->spec==VhdlDocGen::PACKAGE_BODY)
  {
    current_root->addSubEntry(current);
  }
  else
  {
    if (lastCompound) 
    {
      lastCompound->addSubEntry(current);
    }
    else
    {
      if (lastEntity)
      {
	lastEntity->addSubEntry(current);
      }
      else 
      {
	current_root->addSubEntry(current); // should not happen!
      }
    }
  }
  previous = current;
  current = new Entry ;
  initEntry(current);
}

static void handleCommentBlock(const QCString &doc,bool brief)
{
  int position=0;
  bool needsEntry=FALSE;
  Protection protection=Public;
  int lineNr = iDocLine;
  if (brief) 
    current->briefLine = iDocLine; 
  else 
    current->docLine = iDocLine;

  //printf("parseCommentBlock %p [%s]\n",current,doc.data());
  while (parseCommentBlock(
	g_thisParser,
	current,
	doc,        // text
	yyFileName, // file
	lineNr,     // line of block start
	brief, 
	docBlockAutoBrief,
	FALSE,
	protection,
        position,
        needsEntry
        )
     ) 
  {
    //printf("parseCommentBlock position=%d [%s]\n",position,doc.data()+position);
    if (needsEntry) newEntry();
  }
  if (needsEntry)
  {
    newEntry();
  }

  if (docBlockTerm)
  {
    unput(docBlockTerm);
    docBlockTerm=0;
  }
  iDocLine=-1;
}

#if 0
/*!
 * adds grouping to the entries
 */
static void mergeGrouping(const Entry* ce,int)
{
  EntryListIterator eli(*ce->children());
  Entry *rt;
  for (;(rt=eli.current());++eli)
  {
    if (rt->section==Entry::GROUPDOC_SEC)
    {
      if (openGroups)
      {
	QCString tt=(QCString)qrl.last();
	if (!tt.isEmpty()) 
	{
	  rt->groups->append(new Grouping(tt.data(),Grouping::GROUPING_LOWEST));
	}
      }
      qrl.append(rt->name);    
    }

    if ((strcmp(rt->name.data(),"endgroup")==0) && !qrl.isEmpty())
    {
      qrl.remove((QCString)qrl.last());
      openGroups--;
    }

    if ((strcmp(rt->name.data(),"startgroup")==0))
    {
      openGroups++;
    }

    if (rt->section!=Entry::GROUPDOC_SEC && openGroups && !qrl.isEmpty())
    {
      rt->groups->append(new Grouping(qrl.last().data(),Grouping::GROUPING_LOWEST));
    }

    mergeGrouping(rt,openGroups);
  }
}
#endif

/*
 * adds the library|use statements to the next class (entity|package|architecture|package body
 * library ieee
 * entity xxx
 * .....
 * library
 * package
 * enity zzz
 * .....
 * and so on..
 */

static void mapLibPackage(const Entry* ce)
{
  Entry *lastComp=0;
  while (TRUE)
  {
    bool found = FALSE;
    Entry *rt=0;
    //const QList<Entry> *epp=ce->children();
    EntryListIterator eli(*ce->children());
    EntryListIterator eli1=eli;
    for (;(rt=eli.current()),eli1=eli;++eli)
    {
      if (rt->spec==VhdlDocGen::LIBRARY || rt->spec==VhdlDocGen::USE)
        // top level library or use statement
      {
	Entry *temp=0;
	for (;(temp=eli1.current());++eli1) // find next entity
	{
	  if (temp->spec==VhdlDocGen::ENTITY || temp->spec==VhdlDocGen::PACKAGE || temp->spec==VhdlDocGen::ARCHITECTURE || temp->spec==VhdlDocGen::PACKAGE_BODY)
	  {
	    Entry *ee=new Entry(*rt); //append a copy to entries sublist
	    temp->addSubEntry(ee);
	    found=TRUE;
	    rt->spec=-1; //nullify entry
	    rt->section=0;
	    lastComp=temp;
	    break;
	  }
	}//for
	if (lastComp && rt->spec!=-1)
	{
	  Entry *ee=new Entry(*rt); //append a copy to entries sublist
	  lastComp->addSubEntry(ee);
	  found=TRUE;
	  rt->spec=-1; //nullify entry
	  rt->section=0;
	}
      }//if
    }//for
    if (!found) // nothing left to do
    {
      return; 
    }
  }//while
}//MapLib

#if 0
/*!
 * merges a brief descriptions to the next entry
 */
void mergeBrief(const Entry* ce)
{
  EntryListIterator eli(*ce->children());
  Entry *rt;
  for (;(rt=eli.current());++eli)
  {

    if (found && (!eMerge.brief.isEmpty() || !eMerge.doc.isEmpty()))
    {
      rt->doc+=eMerge.doc.data();
      rt->docLine=eMerge.docLine;
      rt->brief+=eMerge.brief.data();
      rt->briefLine=eMerge.briefLine;
      found=FALSE;
    }

    if ((strcmp(rt->name.data(),"string")==0))
    {
      eMerge.reset();
      eMerge.doc+=rt->doc.data();
      eMerge.docLine=rt->docLine;
      eMerge.brief+=rt->brief.data();
      eMerge.briefLine=rt->briefLine;

      found=TRUE;
    }
    MergeBrief(rt);
  }
}
#endif



void vhdlscanFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION)
  if (g_lexInit)
  {
    vhdlscanYYlex_destroy();
  }

  if (g_buf)
  {
    free(g_buf);
  }

  g_buf=0;
#endif

}

void VHDLLanguageScanner::parseInput(const char *fileName,const char *fileBuf,Entry *root)
{
  inputFile.setName(fileName);
  //uint jfile=inputFile.size();
  ::parserInit(); 
  yyFileName=QCString(fileName);
  groupEnterFile(fileName,yyLineNr);
  g_thisParser = this;
  g_inputFromFile = FALSE;
  inputPosition = 0;
  assert(root!=0);

  inputString=fileBuf;
  inputLen=strlen(fileBuf);
  current_root  = root;
  global_root   = root;
  current=new Entry;
  initEntry(current);
  //current_root->name=QCString("XXX"); // dummy name for root
  if (!inputFile.open(IO_ReadOnly))
  {
    err("\n\n could not open file: %s !!\n\n",yyFileName.data());
    return ;
  }

  if (g_lexInit)
  {
    vhdlscanYYrestart(vhdlscanYYin);     
    unput(' ');
    BEGIN(Start);
  }
  vhdlscanYYlex();
  g_lexInit=TRUE;

  free(g_buf);
  g_buf=0;

  delete current;
  current=0;

  groupLeaveFile(yyFileName,yyLineNr);
  inputFile.close();

  //mergeBrief(current_root);
  //mergeGrouping(current_root,0);
  mapLibPackage(current_root);
}


void VHDLLanguageScanner::parsePrototype(const char *text)
{
  // will be called when a \fn command is found in a comment block

  QCString ss,ret;
  bool sem=FALSE;
  bool func=FALSE;
  QList<Argument> qs;
  qs.setAutoDelete(TRUE);
  VhdlDocGen::parseFuncProto(text,qs,ss,ret,TRUE);
  int count=qs.count();
  if (stricmp(ret.data(),"function")==0)
  {
    func=TRUE;
  }
  if (count<1 && !func)
  {
    return;
  }
  Entry *pp = new Entry;
  initEntry(pp);
  pp->name=ss.stripWhiteSpace();
  pp->args+='(';
  for (int j=0;j<count;j++)
  {
    if (sem)
    {
      pp->args+=','; 
    }

    Argument *ars=(Argument*)(qs.at(j));
    Argument *arg=new Argument;
    arg->attrib = ars->attrib;
    arg->name = ars->name;
    arg->type = ars->type;
    pp->args+=ars->name.data();
    pp->args+=" ";
    pp->args+=ars->type.data();
    pp->argList->append(arg);
    sem=TRUE;
  }
  pp->args+=')';

  if (!ret.isEmpty()) 
    pp->spec=VhdlDocGen::FUNCTION;
  else
    pp->spec=VhdlDocGen::PROCEDURE;

  if (pp->section == Entry::MEMBERDOC_SEC && pp->args.isEmpty())
    pp->section = Entry::VARIABLEDOC_SEC;

  pp->type=ret;
  current_root->addSubEntry(pp);
}

void VHDLLanguageScanner::parseCode(CodeOutputInterface &codeOutIntf,
    const char *scopeName,
    const QCString &input,
    bool isExampleBlock,
    const char *exampleName,
    FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    MemberDef *memberDef,
    bool showLineNumbers
    )
{
  ::parseVhdlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                  fileDef,startLine,endLine,inlineFragment,memberDef,
                  showLineNumbers);
}

