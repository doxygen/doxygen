/******************************************************************************
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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
 * supports VHDL-87
 * does not support all keywords of VHDL '93 (impure function/shared variables ..)
 * and VHDL-AMS 
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
static int              iTextCounter  = 0;
static int              iCounter      = 0;
static int              bropen        = 0;
static int              scantype      = 0;
static int              g_lastCommentContext = 0;
static bool             docBlockAutoBrief;
static char             docBlockTerm;
static int              iDocLine      = -1;

//#define YY_A_INTERACTIVE 1
#define YY_NEVER_INTERACTIVE 1
//-----------------------------------------------------------------------------

static void parserInit();
static void deleteSpecChars(char* str,char *buf);
static void handleCommentBlock(const QCString &doc,bool brief);
static void newEntry();
static void initEntry(Entry *e);

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
  for ( const char* c = vhdlscanYYtext ; *c ; ++c )
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

void getType(Entry* p,char* text)
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

void addSignals(const char* str,int line, Entry *e,const char *comment=0)
{
  //printf("===> addSignals (%s) comment='%s'\n",str,comment);
  QList<QCString> ql;
  QCString bufio;
  ql.setAutoDelete(TRUE);

  VhdlDocGen::getSigName(ql,str,bufio);
  int count = ql.count();

  QCString brief = current->brief;
  QCString doc   = current->doc;
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
    QCString stSpec  = ql.at(0)->data();
    if (genPort==1)
    {
      pTemp->spec    = VhdlDocGen::PORT;
      stSpec.stripPrefix(bufio.data()); 
      stSpec=stSpec.stripWhiteSpace();
      pTemp->args    = stSpec;
      pTemp->type    = bufio;
    }
    else if (genPort==2) // found record
    {
      pTemp->spec    = VhdlDocGen::RECORD;
      pTemp->type    = stSpec;
      pTemp->name.prepend(VhdlDocGen::getRecordNumber());
    }
    else 
    {
      pTemp->spec    = VhdlDocGen::GENERIC;
      pTemp->type    = stSpec;
    }
    addSubEntry(e,pTemp);
  }// for

  /*  found a unit */
  if (genPort==3)
  {
    Entry *pTemp=new Entry;
    initEntry(pTemp);
    QCString tt(str);
    QStringList ql=QStringList::split("=",tt,FALSE);
    pTemp->spec=VhdlDocGen::UNITS;
    pTemp->section=Entry::VARIABLE_SEC;
    pTemp->startLine=line;
    pTemp->bodyLine=line;
    pTemp->type=(QCString)ql[1];
    pTemp->name=(QCString)ql[0].stripWhiteSpace();
    pTemp->name.prepend(VhdlDocGen::getRecordNumber());
    addSubEntry(e,pTemp);                           
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
  VhdlDocGen::parseProcessProto(qcs.data(),name,ql);
  current->section=Entry::FUNCTION_SEC;
  current->stat=TRUE;
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
  QCString name,ret,qcs;
  bool sem=FALSE;
  QList<Argument> ql;
  ql.setAutoDelete(TRUE);
  getBufText(qcs,0);
  if (qcs.contains('(') != qcs.contains(')')) 
    return; // function without a prototype 
  if (qcs.contains("function",FALSE)==0 && qcs.contains("procedure",FALSE)==0) 
    return; 
  //Entry* ppEntry=new Entry;

  VhdlDocGen::parseFuncProto(qcs.data(),ql,name,ret); 
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

  if (ret.data()) 
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


//-------------------------------------------------------------------------


void parserInit()
{
  iCounter=0; 
  iTextCounter=0;
  yyLineNr=1;
  current=0;
  previous=0;
  isFunc=0;
  isBody=0;
  scantype=0;
  //pEntry=0;
  //pp=0; 
  lastCompound=0;
  lastEntity=0;
  bropen=0;
  openGroups=0;
  iDocLine=-1;
  //isPrevDoc=FALSE;
  //prevDocEntry.reset();
  qrl.clear();

  if (!g_lexInit) 
  {
    VhdlDocGen::init();
  }

  uint SSIZE=inputFile.size()+1024;
  if (g_buf==0) free(g_buf);
  g_buf=(char*)(calloc(SSIZE,sizeof(char)));

  if (g_buf==0)
  {
    fprintf(stderr,"\n no enough memory");
    return;
  }
  g_buf[SSIZE-1]='\0';
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
      c++; buf++;
    }
  }
  return c;
}

#if 0
/*
 * adds a  text line description [--#] to the the previous type  
 */

static void addOneTextLine(QCString& ss )
{
  Entry* pTemp=0;
  if (current && current->bodyLine==yyLineNr)
    pTemp=current;
  //else if (pEntry && pEntry->bodyLine==yyLineNr)
  //  pTemp=pEntry;
  else 
    pTemp=VhdlDocGen::getEntryAtLine(current_root,yyLineNr) ;

  if (pTemp)
  {
    ss=ss.stripWhiteSpace();
    ss.stripPrefix("--!");
    pTemp->brief=ss;
    pTemp->briefLine=yyLineNr;
  }        
}
#endif

%}


  /* start command character */
  /* -------------- VHDL SECTION -----------------------------------*/

B            [ \t]
CR           [\r\n]
BR           [ \t\n\r]
LETTER       [a-zA-Z_0-9]
NAME         {LETTER}[a-zA-Z0-9_.]*
FUNCNAME     [a-zA-Z"][*+\-_a-zA-Z0-9"\/]*
DIGITS       [0-9]+|[0-9]+"."[0-9]+|[0-9]+"#"[0-9_a-fA-F\+\.]+"#"
COMMENT      "--"[^\n]*
LABELID      [a-z_A-Z][^\;]*";"({B}*{COMMENT})*
PROTO        [ (]*
TEXTT        "--"[^\/\@\*\#][^\n]*

ENDE         ({BR}*("end"){BR}*[;]{1})
ENDEFF       ("if"|"case"|"loop"|"generate"){BR}*[;]

ENDE3        {BR}*[^a-zA-Z]("end"){BR}+{FUNCNAME}{BR}*[;]
ENDFUNC      {B}*"end"{BR}+"function"{BR}+{FUNCNAME}{BR}*[;]

FUNCPROC     ^{B}*("function"|"procedure"){B}*
ARCHITECTURE ("architecture"){BR}+{NAME}{BR}*("of")
PROCESS      ({BR}*{FUNCNAME}{B}*[:]{BR}*("process"){BR}*{PROTO})|("process"){BR}*("("){BR}*{PROTO}|[^a-zA-Z]("process"){CR}|[^a-zA-Z]("process"){BR}+("is")

ENDPROCESS   ("end"){BR}*("process")
LIBUSE       ^{B}*("use"|"library"){BR}+
ENTITY       ^{B}*("component"|"entity"|"package"){BR}+
PBODY        ("package"){B}+("body"){BR}+{NAME}
SIGTYPES     ^{B}*("subtype"|"type"|"constant"|"attribute"|"signal"|"units"){BR}+

ALLTYPESMAP  {B}*[_a-zA-ZA_Z0-9. ]*{B}*

MAPCOMPONENT ({ALLTYPESMAP}{BR}*[:]{BR}*{ALLTYPESMAP}{BR}*("port"|"generic"){BR}+("map"){BR}*("("){1})

BRACEOPEN    [(]{1}
BRACECLOSE   [)]{1}

ALLID        [^;()\t ]

%option noyywrap

  /* language parsing states */

%x Start
%x Comment
%x FindTypeName
%x ParseType 
%x ParseRecord
%x ParseUnits
%x ParseProcess
%x FindName
%x FindEntityName
%x FindGenPort
%x FindTypes
%x FindSigName
%x FindFuncName
%x FindBegin

%%

<Start>{SIGTYPES}  { // found type constant|type|attribute and so on..
    bropen=0;
    lineCount();

    bufferClear();
    //pEntry=current;
    getType(current,vhdlscanYYtext);
    current->bodyLine=yyLineNr;
    if (current->spec==VhdlDocGen::UNITS)
    {
      //addSubEntry(current,pEntry);
      current->startLine=yyLineNr;
      current->bodyLine=yyLineNr;
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
  QCString curName=VhdlDocGen::getIndexWord(vhdlscanYYtext,1);
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
  QCString qcs=QCString(vhdlscanYYtext);
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
  addText(vhdlscanYYtext,vhdlscanYYleng);
  BEGIN(FindFuncName);
}

<Start>{ENTITY}  {     // found entity|component|package
  lineCount();
  //printf("--> Entity at line %d\n",yyLineNr);

  bropen=0;  
  bufferClear();
  QCString word(vhdlscanYYtext);
  word=word.lower();  
  word=word.stripWhiteSpace();

  if (strcmp(word.data(),"entity")==0)
  {
    isBody=0;
    scantype=0;                                    
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
    current->stat=TRUE;
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

<Start>{MAPCOMPONENT}  { // found new mapped component aaa: bbb port map 

  lineCount();
  QCString type;
  QCString tt(vhdlscanYYtext);
  tt = tt.stripWhiteSpace();
  //printf("  tt=%s\n",tt.data());
  //VhdlDocGen::deleteAllChars(tt,'\n');
  QCString name = VhdlDocGen::getIndexWord(tt,0);
  int i=name.find(':');
  if (i!=-1) 
  {
    name = name.left(i);
    tt = tt.right(tt.length()-i-1);
  }
  int j = tt.find('.');
  if (j!=-1)
  {
    int k=tt.find(" ",j,FALSE);
    if (k>(j+1))
      type=tt.mid(j+1,k-j-1);
  }
  else
  {
    type=VhdlDocGen::getIndexWord(tt,1);
    if (type==":") type=VhdlDocGen::getIndexWord(tt,2);
    if (type=="component") type=VhdlDocGen::getIndexWord(tt,3);
  }
  //printf("Add mapping %s->%s\n",type.data(),name.data());

  current->spec=VhdlDocGen::PORTMAP;
  current->section=Entry::VARIABLE_SEC;
  current->startLine=yyLineNr;
  current->bodyLine=yyLineNr;
  current->type=type;
  current->name=name;
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

#if 0    
  if (current && current->spec==VhdlDocGen::ARCHITECTURE)
  {
    if (!VhdlDocGen::foundInsertedComponent(name,current) && !name.isEmpty())
    {
      BaseInfo *bb=new BaseInfo(name,Private,Normal);
      current->extends->append(bb);    
    }
  }                         
#endif
} 

<Start>{CR}*    {
  lineCount();
  addText(vhdlscanYYtext,vhdlscanYYleng);
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
  QCString zz(vhdlscanYYtext);
  addSignals(zz.data(),yyLineNr,current);
  BEGIN(ParseUnits);
}

<FindName>{NAME} {  // found entity|architecture|component name
  lineCount();

  QCString qcs(vhdlscanYYtext);
  QCString qreal=QCString(vhdlscanYYtext);
  qcs=qcs.stripWhiteSpace();
  if (current->spec==VhdlDocGen::USE)
  {
    int j=qcs.length();
    int i=qcs.find(".");
    if (i>0)
      qcs=qcs.right(j-i-1);
    j=qcs.length();
    i=qcs.find(".");
    if (i>0)
      qcs=qcs.left(i);            
    current->name=qcs;                 
  }
  else if (current->spec==VhdlDocGen::ARCHITECTURE)
  {
    //current->name+=qcs.lower();
    current->name.prepend(qcs+"::");

    if (lastEntity)
    {
      // inherit private inheritance relation between entity and architecture
      if (!VhdlDocGen::foundInsertedComponent(current->name,lastEntity))
      {
	BaseInfo *bb=new BaseInfo(current->name,Private,Normal);
	lastEntity->extends->append(bb);    
      }
    }

  }
  else if (current->spec==VhdlDocGen::PACKAGE_BODY)
  {
    current->name+=qcs;
  }
  else
  {
    current->name+=qcs;
  }
  newEntry();

  BEGIN(Start);
}

<FindFuncName>{FUNCNAME}    { // found name of a process|function|procedure
  lineCount();

  addText(vhdlscanYYtext,vhdlscanYYleng);
  BEGIN(ParseType);
}

<FindTypeName>{NAME}{BR}* {
  lineCount();                                                            
  current->name=QCString(vhdlscanYYtext);
  BEGIN(ParseType);
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
  addText(vhdlscanYYtext,vhdlscanYYleng); 
  addSubEntry(current_root,current);
  current = new Entry;
  initEntry(current);
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

<ParseRecord>[a-z_][^\n;]* { // parse record body
  lineCount();
  QCString zz(vhdlscanYYtext);
  addSignals(zz.data(),yyLineNr,current);
  addText(vhdlscanYYtext,vhdlscanYYleng); 
  BEGIN(ParseRecord);
}

<ParseType>{B}+("is"){B}*{CR}|{B}+("is"){B}*"--" { // found a new function  in an architecture ?
  addText(vhdlscanYYtext,vhdlscanYYleng);
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
  if (ss && bb)
  {
    bufferClear();
    addText(ttt.data(),ttt.length());
    functionEntry=0;
    //eFuncBody=new Entry;
    ::parseFunctionProto();
#if 0

    EntryListIterator eli(*eFuncBody->children());
    Entry *rrt=eli.current();

    if (current && (current->spec==VhdlDocGen::ARCHITECTURE && rrt))
    {
      Entry *ep=new Entry(*rrt);
      addSubEntry(current,ep);
      isBody=1;
    }
    if (rrt) 
    {
      Entry *ef=VhdlDocGen::findFunction(current_root,rrt);
      if (ef) 
      { 
	ef->bodyLine=iFuncLine;
	functionEntry=ef;
      }
      else if ((current->spec==VhdlDocGen::PACKAGE_BODY))//VhdlDocGen::Package_Body))
      {
	Entry *ep=new Entry(*rrt);
	addSubEntry(current,ep);
	ep->bodyLine=iFuncLine;
	functionEntry = ep;
      }
    }
    delete eFuncBody;
    eFuncBody=0;
#endif
  }
  bufferClear();  
  BEGIN(ParseType);
}


<ParseType>[^;()\t ] {
  lineCount();      
  addText(vhdlscanYYtext,vhdlscanYYleng); 
  BEGIN(ParseType);
}

<ParseType>{BRACEOPEN} {
  lineCount();
  bropen++;
  addText(vhdlscanYYtext,vhdlscanYYleng);
  BEGIN(ParseType);
}

<ParseType>{BRACECLOSE} {
  lineCount();
  bropen--;
  addText(vhdlscanYYtext,vhdlscanYYleng);
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
  lineCount();
  if (functionEntry)
  {
    functionEntry->endBodyLine=yyLineNr;
    functionEntry=0;
  }

  if (isBody==1 && isFunc==1 && bropen==0)
  {
    isFunc=0;
  }

  bufferClear();
  BEGIN(Start);

}


<ParseType>{ENDE3} {
  lineCount();
  QCString tt(vhdlscanYYtext);
  tt=tt.lower();
  VhdlDocGen::deleteAllChars(tt,';');
  tt.stripWhiteSpace();
  QStringList ql=QStringList::split(" ",tt,FALSE);
  int index=ql.findIndex(QCString("if"))+1;
  index+=ql.findIndex(QCString("case"))+1;
  index+=ql.findIndex(QCString("loop"))+1;
  index+=ql.findIndex(QCString("generate"))+1;
  bufferClear();
  if (functionEntry && index==0)
  {
    functionEntry->endBodyLine=yyLineNr;
    functionEntry=0;
    isFunc=0;
    BEGIN(Start);
  }
  else if (index==0)
  {
    BEGIN(Start);
  }
  else 
  {
    BEGIN(ParseType);
  }
}

<ParseType>";" {
  lineCount();
  addText(vhdlscanYYtext,vhdlscanYYleng);    
  if (bropen==0 && !(isFunc==1 && isBody==1) )
  {
    if (isFunc)
    {
      parseFunctionProto();
    }//if
    else
    {
      QCString qcs;
      getBufText(qcs,0);
      qcs=qcs.stripWhiteSpace();
      current->section=Entry::VARIABLE_SEC;
      current->type+=qcs.data();  

      if ((current->spec==VhdlDocGen::SIGNAL || 
	   current->spec==VhdlDocGen::CONSTANT || 
	   current->spec==VhdlDocGen::TYPE || 
	   current->spec==VhdlDocGen::SUBTYPE
	  ) &&  
	  qcs.stripPrefix(","))
      {
	QList<QCString> ql;
	ql.setAutoDelete(TRUE);
	QCString buffer;
	if (current->spec==VhdlDocGen::SUBTYPE || 
	    current->spec==VhdlDocGen::TYPE)
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
    }
    isFunc=0;
    //bufferClear();
    BEGIN(Start);
  }
  else
  {
    BEGIN(ParseType);
  }
}

<ParseType>{TEXTT} {
  lineCount();
  BEGIN(ParseType);
}

<ParseType>{BR}* {
  lineCount();
  addText(vhdlscanYYtext,vhdlscanYYleng);
  BEGIN(ParseType);
}

<FindEntityName>{NAME} {    // found name of an entity/architecture/package
  lineCount();
  QCString qcs(vhdlscanYYtext);
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
    QCString qq(vhdlscanYYtext);
    qq=qq.stripWhiteSpace();
    //qq=qq.lower();

    current->name=qq;
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
    QCString qq(vhdlscanYYtext);
    qq=qq.stripWhiteSpace();
    current->name=qq;
    newEntry();
    //QCString qreal=QCString(vhdlscanYYtext);
    BEGIN(Start);
  }
}

<Start>{B}*("generic"|"port"){BR}*[(]+ { // found generic|port in entity
    QCString genp(vhdlscanYYleng+1);
    deleteSpecChars(vhdlscanYYtext,genp.data());
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
    addText(vhdlscanYYtext,vhdlscanYYleng); 
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
  QCString line(vhdlscanYYtext);

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
  int pos;
  //printf("=> signal: line='%s'\n",line.data());
  if (semi!=-1 && (pos=line.find("--"))!=-1)
  {
    int eol = line.findRev('\n');
    //printf("pos=%d eol=%d\n",pos,eol);
    if (eol>=pos+2)
    {
      QRegExp re("\\n[ \\t]*--!"); // comment continuation
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
    addText(vhdlscanYYtext,vhdlscanYYleng);
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
  addText(vhdlscanYYtext,vhdlscanYYleng);
}


<FindSigName>{CR}        { 
  lineCount();
  addText(vhdlscanYYtext,vhdlscanYYleng);
  //BEGIN(FindSigName);
}


<*>^{B}*("for ")[^;]* {
  //printf("\n found for[%s] [%d]",vhdlscanYYtext,yyLineNr);
  lineCount();
}

<*>{DIGITS}                {   // found digit
  addText(vhdlscanYYtext,vhdlscanYYleng);
  lineCount();
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
  if (YY_START!=Comment) // Start of the comment block
  {
    iTextCounter=iCounter;
    startComment=yyLineNr;
    g_lastCommentContext=YY_START;
  }
  addText(vhdlscanYYtext,vhdlscanYYleng);
  lineCount();
  BEGIN(Comment);
}

<Comment>^{B}*"--!"[^\n]* {
  if (iDocLine==-1) iDocLine=yyLineNr;
  addText(vhdlscanYYtext,vhdlscanYYleng);
  lineCount();
}

<Comment>.|\n {
  // found end of comment block
  QCString qcs;
  getBufText(qcs,iTextCounter);
  VhdlDocGen::prepareComment(qcs);
  handleCommentBlock(qcs,FALSE);
  bufferClear();
  unput(*vhdlscanYYtext);
  BEGIN(g_lastCommentContext);
}

<*>^{B}*"--!"[^\n]* { // one line comment
  if (iDocLine==-1) iDocLine=yyLineNr;
  QCString qcs(yytext);
  int j=qcs.find("--!");
  qcs=qcs.right(qcs.length()-3-j);
  //printf("--> handleCommentBlock line %d\n",yyLineNr);
  handleCommentBlock(qcs,TRUE);
  //printf("--> end: handleCommentBlock line %d\n",yyLineNr);
  bufferClear();
} 

<*>{COMMENT} {
}

<*>\n                    {
  lineCount();
  addText(vhdlscanYYtext,vhdlscanYYleng);
  //         printf("\n new-line [%d]",yyLineNr);
  BEGIN(Start);
}

<*>{NAME} {
  addText(vhdlscanYYtext,vhdlscanYYleng);
  lineCount();
}

<*>{B}*	{
  addText(vhdlscanYYtext,vhdlscanYYleng);
  lineCount();
}

<*>.                    {
  addText(vhdlscanYYtext,vhdlscanYYleng);
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
  current_root->addSubEntry(current);
  previous = current;
  current = new Entry ;
  initEntry(current);
}

static void handleCommentBlock(const QCString &doc,bool brief)
{
  int position=0;
  bool needsEntry=FALSE;
  Protection protection=Public;
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
	iDocLine,   // line of block start
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

void mapLibPackage(const Entry* ce)
{
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
	    break;
	  }
	}//for
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
    MemberDef *memberDef
    )
{
  ::parseVhdlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,fileDef,startLine,endLine,inlineFragment,memberDef);
}

