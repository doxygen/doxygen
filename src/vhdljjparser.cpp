/******************************************************************************
 *
 * Copyright (C) 2014 by M. Kreis
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include <qcstring.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include "vhdljjparser.h"
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
#include "arguments.h"
#include "types.h"
#include "VhdlParserIF.h"

using namespace vhdl::parser;
using namespace std;

static ParserInterface *g_thisParser;

static QCString         yyFileName;
static int              yyLineNr      = 1;
static int*             lineParse;
static int              iDocLine      = -1;
static QCString         inputString;
static Entry*           gBlock        = 0;
static Entry*           previous      = 0;
//-------------------------------------------------------

static Entry* oldEntry;
static bool varr=FALSE;
static QCString varName;

static QList<Entry> instFiles;
static QList<Entry> libUse;
static QList<Entry> lineEntry;

Entry*   VhdlParser::currentCompound=0;
Entry*   VhdlParser::tempEntry=0;
Entry*   VhdlParser::lastEntity=0  ;
Entry*   VhdlParser::lastCompound=0  ;
Entry*   VhdlParser::current=0;
Entry*   VhdlParser::current_root  = 0;
QCString VhdlParser::compSpec;
QCString VhdlParser::currName;
QCString VhdlParser::confName;
QCString VhdlParser::genLabels;
QCString VhdlParser::lab;
QCString VhdlParser::forL;

int VhdlParser::param_sec = 0;
int VhdlParser::parse_sec=0;
int VhdlParser::currP=0;
int VhdlParser::levelCounter;

static QList<VhdlConfNode> configL;

static struct
{
  QCString doc;
  bool brief;
  bool pending;
  int iDocLine;
} str_doc;

static bool doxComment=FALSE; // doxygen comment ?
static QCString strComment;
static int iCodeLen;

bool  checkMultiComment(QCString& qcs,int line);
QList<Entry>* getEntryAtLine(const Entry* ce,int line);

//-------------------------------------

QList<VhdlConfNode>& getVhdlConfiguration() { return  configL; }
QList<Entry>& getVhdlInstList() { return  instFiles; }

Entry* getVhdlCompound()
{
  if (VhdlParser::lastEntity) return VhdlParser::lastEntity;
  if (VhdlParser::lastCompound) return VhdlParser::lastCompound;
  return NULL;
}

void startCodeBlock(int index)
{
  int ll=strComment.length();
  if (!gBlock) gBlock = new Entry;
  iCodeLen=inputString.findRev(strComment.data())+ll;
  // fprintf(stderr,"\n startin code..%d %d %d\n",iCodeLen,num_chars,ll);
  gBlock->reset();
  int len=strComment.length();
  QCString name=strComment.right(len-index);//
  name=VhdlDocGen::getIndexWord(name.data(),1);
  if (!name)
    gBlock->name="misc"+ VhdlDocGen::getRecordNumber();
  else
    gBlock->name=name;

  gBlock->startLine=yyLineNr;
  gBlock->bodyLine=yyLineNr;

  strComment=strComment.left(index);
  VhdlDocGen::prepareComment(strComment);
  gBlock->brief+=strComment;
}

void makeInlineDoc(int endCode)
{
  int len=endCode-iCodeLen;
  if (!gBlock) gBlock = new Entry;
  QCString par=inputString.mid(iCodeLen,len);
  //fprintf(stderr,"\n inline code: \n<%s>",par.data());
  gBlock->doc=par;
  gBlock->inbodyDocs=par;
  gBlock->section=Entry::VARIABLE_SEC;
  gBlock->spec=VhdlDocGen::MISCELLANEOUS;
  gBlock->fileName = yyFileName;
  gBlock->endBodyLine=yyLineNr-1;
  gBlock->lang=SrcLangExt_VHDL;
  Entry *temp=new Entry(*gBlock);
  Entry* compound=getVhdlCompound();

  if (compound)
  {
    compound->addSubEntry(temp);
  }
  else
  {
    temp->type="misc"; // global code like library ieee...
    VhdlParser::current_root->addSubEntry(temp);
  }
  strComment.resize(0);
  gBlock->reset();
}// makeInlineDoc


bool isConstraintFile(const QCString &fileName,const QCString &ext)
{
  return fileName.right(ext.length())==ext;
}


void VHDLLanguageScanner::parseInput(const char *fileName,const char *fileBuf,Entry *root,
                          bool ,QStrList&)
{
  g_thisParser=this;
  bool inLine=false;
  inputString=fileBuf;

 // fprintf(stderr,"\n ============= %s\n ==========\n",fileBuf);

  if (strlen(fileName)==0)
  {
    inLine=true;
  }

  yyFileName+=fileName;

  bool xilinx_ucf=isConstraintFile(yyFileName,".ucf");
  bool altera_qsf=isConstraintFile(yyFileName,".qsf");

  // support XILINX(ucf) and ALTERA (qsf) file

  if (xilinx_ucf)
  {
    VhdlDocGen::parseUCF(fileBuf,root,yyFileName,FALSE);
    return;
  }
  if (altera_qsf)
  {
    VhdlDocGen::parseUCF(fileBuf,root,yyFileName,TRUE);
    return;
  }
  libUse.setAutoDelete(true);
  yyLineNr=1;
  VhdlParser::current_root=root;
  VhdlParser::lastCompound=0;
  VhdlParser::lastEntity=0;
  VhdlParser::currentCompound=0;
  VhdlParser::lastEntity=0;
  VhdlParser::current=new Entry();
  VhdlParser::initEntry(VhdlParser::current);
  groupEnterFile(fileName,yyLineNr);
  lineParse=new int[200]; // Dimitri: dangerous constant: should be bigger than largest token id in VhdlParserConstants.h
  VhdlParserIF::parseVhdlfile(fileBuf,inLine);

  delete VhdlParser::current;
  VhdlParser::current=0;

  if (!inLine)
  VhdlParser::mapLibPackage(root);

  delete[] lineParse;
  yyFileName.resize(0);
  libUse.clear();
  VhdlDocGen::resetCodeVhdlParserState();
}

void VhdlParser::lineCount()
{
  yyLineNr++;
}

void VhdlParser::lineCount(const char* text)
{
  for (const char* c=text ; *c ; ++c )
  {
    yyLineNr += (*c == '\n') ;
  }
}

void isVhdlDocPending()
{
  if (!str_doc.pending) return;

  str_doc.pending=FALSE;
  oldEntry=0; // prevents endless recursion
  iDocLine=str_doc.iDocLine;
  VhdlParser::handleCommentBlock(str_doc.doc,str_doc.brief);
  iDocLine=-1;
}

void VhdlParser::initEntry(Entry *e)
{
  e->fileName = yyFileName;
  e->lang     = SrcLangExt_VHDL;
  isVhdlDocPending();
  initGroupInfo(e);
}

void VhdlParser::newEntry()
{
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
	current_root->addSubEntry(current);
      }
    }
  }
  previous = current;
  current = new Entry ;
  initEntry(current);
}

bool checkInlineCode(QCString & doc)
{
  int index=doc.find("\\code");

  if (index>0)
  {
     strComment+=doc;
	 startCodeBlock(index);
	 doxComment=TRUE;
	 return true;
  }
  return false;
}

void VhdlParser::handleFlowComment(const char* doc)
{
	lineCount(doc);

  if (VhdlDocGen::getFlowMember())
  {
    QCString qcs(doc);
    qcs=qcs.stripWhiteSpace();
    qcs.stripPrefix("--#");
    FlowChart::addFlowChart(FlowChart::COMMENT_NO,0,0,qcs.data());
  }
}


void VhdlParser::handleCommentBlock(const char* doc1,bool brief)
{
  int position=0;
  static bool isIn;
  QCString doc;
  doc.append(doc1);
 // fprintf(stderr,"\n %s",doc.data());
  if (doc.isEmpty()) return;

  if (checkMultiComment(doc,yyLineNr))
  {
    strComment.resize(0);
    return;
  }

  isIn=checkInlineCode(doc);
  bool isEndCode=doc.contains("\\endcode");
  // empty comment  --!
  if (isEndCode)
  {
    int end=inputString.find(doc.data(),iCodeLen);
    makeInlineDoc(end);
    strComment.resize(0);
    isIn=false;
  }
  if (isIn)
  {
    isIn=false;
    return;
  }

  VhdlDocGen::prepareComment(doc);

  bool needsEntry=FALSE;
  Protection protection=Public;

  if (oldEntry==current)
  {
    //printf("\n find pending message  < %s > at line: %d \n ",doc.data(),iDocLine);
    str_doc.doc=doc;
    str_doc.iDocLine=iDocLine;
    str_doc.brief=brief;
    str_doc.pending=TRUE;
    return;
  }

  oldEntry=current;

  if (brief)
  {
    current->briefLine = yyLineNr;
  }
  else
  {
    current->docLine = yyLineNr;
  }
  //  printf("parseCommentBlock file<%s>\n [%s]\n at line [%d] \n ",yyFileName.data(),doc.data(),iDocLine);

  int j=doc.find("[plant]");
  if (j>=0)
  {
    doc=doc.remove(j,7);
    current->stat=true;
  }

  while (parseCommentBlock(
        g_thisParser,
        current,
        doc,        // text
        yyFileName, // file
        iDocLine,   // line of block start
        brief,
        0,
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
    if (varr)
    {
      varr=FALSE;
      current->name=varName;
      current->section=Entry::VARIABLEDOC_SEC;
      varName="";
    }
    newEntry();
  }
  iDocLine=-1;
  strComment.resize(0);
}

void VHDLLanguageScanner::parsePrototype(const char *text)
{
  varName=text;
  varr=TRUE;
}

void VhdlParser::addCompInst(const char *n, const char* instName, const char* comp,int iLine)
{
  current->spec=VhdlDocGen::INSTANTIATION;
  current->section=Entry::VARIABLE_SEC;
  current->startLine=iLine;
  current->bodyLine=iLine;
  current->type=instName;                       // foo:instname e.g proto or work. proto(ttt)
  current->exception=genLabels.lower();         // |arch|label1:label2...
  current->name=n;                              // foo
  if (lastCompound)
  {
    current->args=lastCompound->name;             // architecture name
  }
  current->includeName=comp;                    // component/enity/configuration
  int u=genLabels.find("|",1);
  if (u>0)
  {
    current->write=genLabels.right(genLabels.length()-u);
    current->read=genLabels.left(u);
  }
  //printf  (" \n genlable: [%s]  inst: [%s]  name: [%s] %d\n",n,instName,comp,iLine);

  if (lastCompound)
  {
    current->args=lastCompound->name;
    if (true) // !findInstant(current->type))
    {
      initEntry(current);
      instFiles.append(new Entry(*current));
    }

    Entry *temp=current;  // hold  current pointer  (temp=oldEntry)
    current=new Entry;     // (oldEntry != current)
    delete  temp;
  }
  else
  {
    newEntry();
  }
}

void VhdlParser::addVhdlType(const char *n,int startLine,int section,
    uint64 spec,const char* args,const char* type,Protection prot)
{
  QCString name(n);
  if (isFuncProcProced() || VhdlDocGen::getFlowMember())  return;

  if (parse_sec==GEN_SEC)
  {
    spec= VhdlDocGen::GENERIC;
  }

  QStringList ql=QStringList::split(",",name,FALSE);

  for (uint u=0;u<ql.count();u++)
  {
    current->name=ql[u].utf8();
    current->startLine=startLine;
    current->bodyLine=startLine;
    current->section=section;
    current->spec=spec;
    current->fileName=yyFileName;
    if (current->args.isEmpty())
    {
      current->args=args;
    }
    current->type=type;
    current->protection=prot;

    if (!lastCompound && (section==Entry::VARIABLE_SEC) &&  (spec == VhdlDocGen::USE || spec == VhdlDocGen::LIBRARY) )
    {
      libUse.append(new Entry(*current));
      current->reset();
    }
    newEntry();
  }
}

void VhdlParser::createFunction(const char *imp,uint64 spec,const char *fn)
{
  QCString impure(imp);
  QCString fname(fn);
  current->spec=spec;
  current->section=Entry::FUNCTION_SEC;

  if (impure=="impure" || impure=="pure")
  {
    current->exception=impure;
  }

  if (parse_sec==GEN_SEC)
  {
    current->spec= VhdlDocGen::GENERIC;
    current->section=Entry::FUNCTION_SEC;
  }

  if (currP==VhdlDocGen::PROCEDURE)
  {
    current->name=impure;
    current->exception="";
  }
  else
  {
    current->name=fname;
  }

  if (spec==VhdlDocGen::PROCESS)
  {
    current->args=fname;
    current->name=impure;
    VhdlDocGen::deleteAllChars(current->args,' ');
    if (!fname.isEmpty())
    {
      QStringList q1=QStringList::split(",",fname);
      for (uint ii=0;ii<q1.count();ii++)
      {
        Argument *arg=new Argument;
        arg->name=q1[ii].utf8();
        current->argList->append(arg);
      }
    }
    return;
  }
 }


bool VhdlParser::isFuncProcProced()
{
  if (currP==VhdlDocGen::FUNCTION  ||
      currP==VhdlDocGen::PROCEDURE ||
      currP==VhdlDocGen::PROCESS
     )
  {
    return TRUE;
  }
  return FALSE;
}

void VhdlParser::pushLabel( QCString &label,QCString & val)
{
  label+="|";
  label+=val;
}

 QCString  VhdlParser::popLabel(QCString & q)
{
  int i=q.findRev("|");
  if (i<0) return "";
  q = q.left(i);
  return q;
}

void VhdlParser::addConfigureNode(const char* a,const char*b, bool,bool isLeaf,bool inlineConf)
{
  VhdlConfNode* co=0;
  QCString ent;
  ent=a;

  if (b)
  {
    ent=b;
  }
  int level=0;

  if (!configL.isEmpty())
  {
    VhdlConfNode* vc=configL.getLast();
    level=vc->level;
    if (levelCounter==0)
    {
      pushLabel(forL,ent);
    }
    else if (level<levelCounter)
    {
      if (!isLeaf)
      {
        pushLabel(forL,ent);
      }
    }
    else if (level>levelCounter)
    {
      forL=popLabel(forL);
    }
  }
  else
  {
    pushLabel(forL,ent);
  }

  if (inlineConf)
  {
    confName=lastCompound->name;
  }

  //fprintf(stderr,"\n[%s %d %d]\n",forL.data(),levelCounter,level);
  co=new VhdlConfNode(a,b,confName.lower().data(),forL.lower().data(),isLeaf);

  if (inlineConf)
  {
    co->isInlineConf=TRUE;
  }

  configL.append(co);
}


void VhdlParser::addProto(const char *s1,const char *s2,const char *s3,
    const char *s4,const char *s5,const char *s6)
{
  (void)s5; // avoid unused warning
  QCString name=s2;
  QStringList ql=QStringList::split(",",name,FALSE);

  for (uint u=0;u<ql.count();u++)
  {
    Argument *arg=new Argument;
    arg->name=ql[u].utf8();
    if (s3)
    {
      arg->type=s3;
    }
    arg->type+=" ";
    arg->type+=s4;
    if (s6)
    {
      arg->type+=s6;
    }
    if (parse_sec==GEN_SEC && param_sec==0)
    {
      arg->defval="gen!";
    }

    if (parse_sec==PARAM_SEC)
    {
    //  assert(false);
    }

    arg->defval+=s1;
    arg->attrib="";//s6;

    current->argList->append(arg);
    current->args+=s2;
    current->args+=",";
  }
}


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
void VhdlParser::mapLibPackage( Entry* root)
{
  QList<Entry> epp=libUse;
  EntryListIterator eli(epp);
  Entry *rt;
  for (;(rt=eli.current());++eli)
  {
    if (addLibUseClause(rt->name))
    {
      Entry *current;
      EntryListIterator eLib(*root->children());
      bool bFound=FALSE;
      for (eLib.toFirst();(current=eLib.current());++eLib)
      {
        if (VhdlDocGen::isVhdlClass(current))
        {
          if (current->startLine > rt->startLine)
          {
            bFound=TRUE;
            current->addSubEntry(new Entry(*rt));
            break;
          }
        }
      }//for
      if (!bFound)
      {
        root->addSubEntry(new Entry(*rt));
      }
    } //if
  }// for
}//MapLib

bool VhdlParser::addLibUseClause(const QCString &type)
{
  static bool showIEEESTD=Config_getBool(FORCE_LOCAL_INCLUDES);

  if (showIEEESTD) // all standard packages and libraries will not be shown
  {
    if (type.lower().stripPrefix("ieee")) return FALSE;
    if (type.lower().stripPrefix("std")) return FALSE;
  }
  return TRUE;
}

int VhdlParser::getLine()
{
  return yyLineNr;
}

void VhdlParser::setLineParsed(int tok)
{
  lineParse[tok]=yyLineNr;
}

int VhdlParser::getLine(int tok)
{
  int val=lineParse[tok];
  if (val<0) val=0;
  //assert(val>=0 && val<=yyLineNr);
  return val;
}


void VhdlParser::createFlow()
{
  if (!VhdlDocGen::getFlowMember())
  {
    return;
  }
  QCString q,ret;

  if (currP==VhdlDocGen::FUNCTION)
  {
    q=":function( ";
    FlowChart::alignFuncProc(q,tempEntry->argList,true);
    q+=")";
  }
  else if (currP==VhdlDocGen::PROCEDURE)
  {
    q=":procedure (";
    FlowChart::alignFuncProc(q,tempEntry->argList,false);
    q+=")";
  }
  else
  {
    q=":process( "+tempEntry->args;
    q+=")";
  }

  q.prepend(VhdlDocGen::getFlowMember()->name().data());

  FlowChart::addFlowChart(FlowChart::START_NO,q,0);

  if (currP==VhdlDocGen::FUNCTION)
  {
    ret="end function ";
  }
  else if (currP==VhdlDocGen::PROCEDURE)
  {
    ret="end procedure";
  }
  else
  {
    ret="end process ";
  }

  FlowChart::addFlowChart(FlowChart::END_NO,ret,0);
  //  FlowChart::printFlowList();
  FlowChart::writeFlowChart();
  currP=0;
}

void VhdlParser::setMultCommentLine()
{
  iDocLine=yyLineNr;
}

void VhdlParser::oneLineComment(QCString qcs)
{
  bool isEndCode=qcs.contains("\\endcode");

  int index = qcs.find("\\code");
  if (isEndCode)
  {
    int end = inputString.find(qcs.data(),iCodeLen);
    makeInlineDoc(end);
  }
  else if (index > 0)
  {
    // assert(false);
    strComment=qcs;
    startCodeBlock(index);
    strComment.resize(0);
  }

  if (!isEndCode && index==-1)
  {
    int j=qcs.find("--!");
    qcs=qcs.right(qcs.length()-3-j);
    if (!checkMultiComment(qcs,iDocLine))
    {
      handleCommentBlock(qcs,TRUE);
    }
  }
}


bool  checkMultiComment(QCString& qcs,int line)
{
  QList<Entry> *pTemp=getEntryAtLine(VhdlParser::current_root,line);

  if (pTemp->isEmpty()) return false;

  VhdlDocGen::prepareComment(qcs);
  while (!pTemp->isEmpty())
  {
    Entry *e=(Entry*)pTemp->getFirst();
    e->briefLine=line;
    e->brief+=qcs;

    pTemp->removeFirst();
  }
  return true;
}

// returns the vhdl parsed types at line xxx
QList<Entry>* getEntryAtLine(const Entry* ce,int line)
{
  EntryListIterator eli(*ce->children());
  Entry *rt;
  for (;(rt=eli.current());++eli)
  {
    if (rt->bodyLine==line)
    {
      lineEntry.insert(0,rt);
    }

    getEntryAtLine(rt,line);
  }
  return &lineEntry;
}

