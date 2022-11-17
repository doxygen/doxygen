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

#include <string>

#include "qcstring.h"
#include "containers.h"
#include "vhdljjparser.h"
#include "vhdldocgen.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "language.h"
#include "commentscan.h"
#include "definition.h"
#include "searchindex.h"
#include "outputlist.h"
#include "arguments.h"
#include "types.h"
#include "growbuf.h"
#include "markdown.h"
#include "VhdlParserTokenManager.h"
#include "VhdlParserErrorHandler.hpp"
#include "regex.h"

using namespace vhdl::parser;

struct VHDLDocInfo
{
  QCString doc;
  bool brief;
  bool pending = false;
  int iDocLine = 1;
};


static bool isConstraintFile(const QCString &fileName,const QCString &ext)
{
  return fileName.right(ext.length())==ext;
}


//-------------------------------------

static EntryList g_instFiles;

struct VHDLOutlineParser::Private
{
  void parseVhdlfile(const QCString &fileName,const char* inputBuffer,bool inLine);

  VHDLOutlineParser      *thisParser = 0;
  VhdlParser             *vhdlParser = 0;
  CommentScanner          commentScanner;

  QCString                yyFileName;
  int                     yyLineNr      = 1;
  IntVector               lineParse;
  int                     iDocLine      = -1;
  QCString                inputString;
  Entry*                  gBlock        = 0;
  Entry*                  previous      = 0;
//-------------------------------------------------------

  Entry*                  oldEntry = 0;
  bool                    varr = FALSE;
  QCString                varName;
  EntryList               libUse;
  EntryList               lineEntry;
  QCString                strComment;
  int                     iCodeLen;
  VHDLDocInfo             str_doc;
  VhdlParser::SharedState shared;
  QCString                forL;
  int code = 0;

};

void VHDLOutlineParser::Private::parseVhdlfile(const QCString &fileName,
                                               const char* inputBuffer,bool inLine)
{
  JAVACC_STRING_TYPE s =inputBuffer;
  CharStream *stream = new CharStream(s.c_str(), (int)s.size(), 1, 1);
  VhdlParserTokenManager *tokenManager = new VhdlParserTokenManager(stream);
  VhdlTokenManagerErrorHandler *tokErrHandler=new VhdlTokenManagerErrorHandler(fileName.data());
  vhdlParser=new VhdlParser(tokenManager);
  vhdlParser->setOutlineParser(thisParser);
  vhdlParser->setSharedState(&shared);
  tokenManager->setLexParser(vhdlParser);
  tokenManager->ReInit(stream,0);
  tokenManager->setErrorHandler(tokErrHandler);
  VhdlErrorHandler *parserErrHandler=new VhdlErrorHandler(fileName.data());
  vhdlParser->setErrorHandler(parserErrHandler);
  try
  {
    if(inLine)
    {
      vhdlParser->parseInline();
    }
    else
    {
      vhdlParser->design_file();
    }
  }
  catch( std::exception &){ /* fprintf(stderr,"\n[%s]",e.what()); */ }
  //  fprintf(stderr,"\n\nparsed lines: %d\n",yyLineNr);
  //  fprintf(stderr,"\n\nerrors : %d\n\n",myErr->getErrorCount());
  //
  delete vhdlParser;
  delete tokenManager;
  delete stream;
}

VHDLOutlineParser::VHDLOutlineParser() : p(std::make_unique<Private>())
{
}

VHDLOutlineParser::~VHDLOutlineParser()
{
}

void VHDLOutlineParser::parseInput(const QCString &fileName,const char *fileBuf,
                                   const std::shared_ptr<Entry> &root, ClangTUParser *)
{
  VhdlParser::SharedState *s = &p->shared;
  p->thisParser=this;
  p->inputString=fileBuf;

 // fprintf(stderr,"\n ============= %s\n ==========\n",fileBuf);

  bool inLine = fileName.isEmpty();

  p->yyFileName=fileName;

  bool xilinx_ucf=isConstraintFile(p->yyFileName,".ucf");
  bool altera_qsf=isConstraintFile(p->yyFileName,".qsf");

  // support XILINX(ucf) and ALTERA (qsf) file

  if (xilinx_ucf)
  {
    VhdlDocGen::parseUCF(fileBuf,root.get(),p->yyFileName,FALSE);
    return;
  }
  if (altera_qsf)
  {
    VhdlDocGen::parseUCF(fileBuf,root.get(),p->yyFileName,TRUE);
    return;
  }
  p->yyLineNr=1;
  s->current_root=root;
  s->lastCompound=0;
  s->lastEntity=0;
  s->lastEntity=0;
  p->oldEntry = 0;
  s->current=std::make_shared<Entry>();
  initEntry(s->current.get());
  p->commentScanner.enterFile(fileName,p->yyLineNr);
  p->lineParse.reserve(200);
  p->parseVhdlfile(fileName,fileBuf,inLine);
  p->commentScanner.leaveFile(fileName,p->yyLineNr);

  s->current.reset();

  if (!inLine)
  mapLibPackage(root.get());

  p->yyFileName.resize(0);
  p->libUse.clear();
}

void VHDLOutlineParser::lineCount()
{
  p->yyLineNr++;
}

void VHDLOutlineParser::lineCount(const char* text)
{
  for (const char* c=text ; *c ; ++c )
  {
    if (*c == '\n') p->yyLineNr++;
  }
}

void VHDLOutlineParser::initEntry(Entry *e)
{
  e->fileName = p->yyFileName;
  e->lang     = SrcLangExt_VHDL;
  if (p->str_doc.pending)
  {
    p->str_doc.pending=FALSE;
    p->oldEntry=0; // prevents endless recursion
    p->iDocLine=p->str_doc.iDocLine;
    handleCommentBlock(p->str_doc.doc,p->str_doc.brief);
    p->iDocLine=-1;
  }
  p->commentScanner.initGroupInfo(e);
}

void VHDLOutlineParser::newEntry()
{
  VhdlParser::SharedState *s = &p->shared;
  p->previous = s->current.get();
  if (s->current->spec==VhdlDocGen::ENTITY ||
      s->current->spec==VhdlDocGen::PACKAGE ||
      s->current->spec==VhdlDocGen::ARCHITECTURE ||
      s->current->spec==VhdlDocGen::PACKAGE_BODY)
  {
    s->current_root->moveToSubEntryAndRefresh(s->current);
  }
  else
  {
    if (s->lastCompound)
    {
      s->lastCompound->moveToSubEntryAndRefresh(s->current);
    }
    else
    {
      if (s->lastEntity)
      {
	s->lastEntity->moveToSubEntryAndRefresh(s->current);
      }
      else
      {
	s->current_root->moveToSubEntryAndRefresh(s->current);
      }
    }
  }
  initEntry(s->current.get());
}

static int idCounter;

/** returns a unique id for each record member.
*
*  type first_rec is record
*		RE: data_type;
*	end;
*
* type second_rec is record
*		RE: data_type;
*	end;
*/

QCString VHDLOutlineParser::getNameID()
{
  return QCString().setNum(idCounter++);
}

void VHDLOutlineParser::handleFlowComment(const char* doc)
{
  lineCount(doc);

  if (VhdlDocGen::getFlowMember())
  {
    QCString qcs(doc);
    qcs=qcs.stripWhiteSpace();
    qcs.stripPrefix("--#");
    FlowChart::addFlowChart(FlowChart::COMMENT_NO,QCString(),QCString(),qcs);
  }
}

int VHDLOutlineParser::checkInlineCode(QCString &doc)
{
  static const reg::Ex csRe(R"([\\@]code)");
  static const reg::Ex cendRe(R"(\s*[\\@]endcode)");
  static const reg::Ex cbriefRe(R"([\\@]brief)");

  // helper to simulate behavior of QString.find(const QRegExp &re,int pos)
  auto findRe = [](const QCString &str,const reg::Ex &re,int pos=0) -> int
  {
    if ((int)str.length()<pos) return -1;
    reg::Match match;
    const std::string s = str.str();
    if (reg::search(s,match,re,pos)) // match found
    {
      return (int)match.position();
    }
    else // not found
    {
      return -1;
    }
  };
  auto replaceRe = [](const QCString &str,const reg::Ex &re,const QCString &replacement) -> QCString
  {
    return reg::replace(str.str(), re, replacement.str());
  };

  int index = findRe(doc,csRe);

  if (findRe(doc,cendRe)!=-1)
    return 1;

  if (index < 0)
    return index;

  VhdlParser::SharedState *s = &p->shared;
  p->strComment += doc;
  p->code = findRe(p->inputString,csRe, p->code + 1);
  int com = p->inputString.find(p->strComment.data());
  int ref = findRe(p->inputString,cendRe, p->code + 1);
  int len = p->strComment.size();

  int ll = com + len;
  int diff = ref - ll - 3;
  QCString code = p->inputString.mid(ll, diff);
  int iLine = 0;
  code = stripLeadingAndTrailingEmptyLines(code, iLine);
  int val = code.contains('\n');
  VhdlDocGen::prepareComment(p->strComment);
  StringVector ql = split(p->strComment.str(),"\n");

  QCString co;
  QCString na;
  for (const auto &qcs_ : ql)
  {
    QCString qcs(qcs_);
    qcs = qcs.simplifyWhiteSpace();
    if (findRe(qcs,csRe)!=-1)
    {
      int i = qcs.find('{');
      int j = qcs.find('}');
      if (i > 0 && j > 0 && j > i)
      {
        na = qcs.mid(i + 1, (j - i - 1));
      }
      continue;
    }
    qcs = replaceRe(qcs,cbriefRe, "");
    co += qcs;
    co += '\n';
  }

  VhdlDocGen::prepareComment(co);

  Entry gBlock;
  if (!na.isEmpty())
    gBlock.name = na;
  else
    gBlock.name = "misc" + VhdlDocGen::getRecordNumber();
  gBlock.startLine = p->yyLineNr+iLine-1;
  gBlock.bodyLine = p->yyLineNr+iLine-1 ;
  gBlock.doc = code;
  gBlock.inbodyDocs = code;
  gBlock.brief = co;
  gBlock.section = Entry::VARIABLE_SEC;
  gBlock.spec = VhdlDocGen::MISCELLANEOUS;
  gBlock.fileName = p->yyFileName;
  gBlock.endBodyLine = p->yyLineNr + val +iLine;
  gBlock.lang = SrcLangExt_VHDL;
  std::shared_ptr<Entry> compound;

  if (s->lastEntity)
    compound = s->lastEntity;
  else if (s->lastCompound)
    compound = s->lastCompound;
  else
    compound = 0;

  if (compound)
  {
    compound->copyToSubEntry(&gBlock);
  }
  else
  {
    gBlock.type = "misc"; // global code like library ieee...
    s->current_root->copyToSubEntry(&gBlock);
  }
  p->strComment.resize(0);
  return 1;
}

void VHDLOutlineParser::handleCommentBlock(const QCString &doc1, bool brief)
{
  int position = 0;
  bool needsEntry = FALSE;
  VhdlParser::SharedState *s = &p->shared;
  QCString doc = doc1;

  if (doc.isEmpty())
    return;

  if (checkMultiComment(doc, p->yyLineNr))
  {
    p->strComment.resize(0);
    return;
  }

  if (checkInlineCode(doc) > 0)
  {
    return;
  }

  Protection protection = Public;
  VhdlDocGen::prepareComment(doc);

  if (p->oldEntry == s->current.get())
  {
    p->str_doc.doc = doc;
    p->str_doc.iDocLine = p->iDocLine;
    p->str_doc.brief = brief;
    p->str_doc.pending = TRUE;
    return;
  }

  p->oldEntry = s->current.get();

  if (brief)
  {
    s->current->briefLine = p->yyLineNr;
  }
  else
  {
    s->current->docLine = p->yyLineNr;
  }



  Markdown markdown(p->yyFileName,p->iDocLine);
  int lineNr = p->iDocLine;
  QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(doc,lineNr) : doc;

   while (p->commentScanner.parseCommentBlock(
      p->thisParser,
      s->current.get(),
      processedDoc,  // text
      p->yyFileName, // file
      lineNr,       // line of block start
      brief,
      0,
      FALSE,
      protection,
      position,
      needsEntry,
      Config_getBool(MARKDOWN_SUPPORT)))
  {
    if (needsEntry)
      newEntry();
  }
  if (needsEntry)
  {
    if (p->varr)
    {
      p->varr = FALSE;
      s->current->name = p->varName;
      s->current->section = Entry::VARIABLEDOC_SEC;
      p->varName = "";
    }
    newEntry();
  }
  p->iDocLine = -1;
  p->strComment.resize(0);
}

void VHDLOutlineParser::parsePrototype(const QCString &text)
{
  p->varName=text;
  p->varr=TRUE;
}

void VHDLOutlineParser::addCompInst(const char *n, const char* instName, const char* comp,int iLine)
{
  VhdlParser::SharedState *s = &p->shared;
  s->current->spec=VhdlDocGen::INSTANTIATION;
  s->current->section=Entry::VARIABLE_SEC;
  s->current->startLine=iLine;
  s->current->bodyLine=iLine;
  s->current->type=instName;                       // foo:instname e.g proto or work. proto(ttt)
  s->current->exception=s->genLabels.lower();         // |arch|label1:label2...
  s->current->name=n;                              // foo
  if (s->lastCompound)
  {
    s->current->args=s->lastCompound->name;             // architecture name
  }
  s->current->includeName=comp;                    // component/entity/configuration
  int u=s->genLabels.find("|",1);
  if (u>0)
  {
    s->current->write=s->genLabels.right(s->genLabels.length()-u);
    s->current->read=s->genLabels.left(u);
  }
  //printf  (" \n genlabel: [%s]  inst: [%s]  name: [%s] %d\n",n,instName,comp,iLine);

  if (s->lastCompound)
  {
    s->current->args=s->lastCompound->name;
    if (true) // !findInstant(current->type))
    {
      initEntry(s->current.get());
      // TODO: protect with mutex
      g_instFiles.emplace_back(std::make_shared<Entry>(*s->current));
      // TODO: end protect with mutex
    }

    s->current=std::make_shared<Entry>();
  }
  else
  {
    newEntry();
  }
}

void VHDLOutlineParser::addVhdlType(const char *n,int startLine,int section,
    uint64 spec,const char* args,const char* type,Protection prot)
{
  VhdlParser::SharedState *s = &p->shared;
  QCString name(n);
  if (isFuncProcProced() || VhdlDocGen::getFlowMember())  return;

  if (s->parse_sec==GEN_SEC)
  {
    spec= VhdlDocGen::GENERIC;
  }

  StringVector ql=split(name.str(),",");

  for (size_t u=0;u<ql.size();u++)
  {
    s->current->name=ql[u];
    s->current->startLine=startLine;
    s->current->bodyLine=startLine;
    s->current->section=section;
    s->current->spec=spec;
    s->current->fileName=p->yyFileName;
    if (s->current->args.isEmpty())
    {
      s->current->args=args;
    }
    s->current->type=type;
    s->current->protection=prot;

    if (!s->lastCompound && (section==Entry::VARIABLE_SEC) &&  (spec == VhdlDocGen::USE || spec == VhdlDocGen::LIBRARY) )
    {
      p->libUse.emplace_back(std::make_shared<Entry>(*s->current));
      s->current->reset();
    }
    newEntry();
  }
}

void VHDLOutlineParser::createFunction(const char *imp,uint64 spec,const char *fn)
{
  VhdlParser::SharedState *s = &p->shared;
  QCString impure(imp);
  QCString fname(fn);
  s->current->spec=spec;
  s->current->section=Entry::FUNCTION_SEC;

  if (impure=="impure" || impure=="pure")
  {
    s->current->exception=impure;
  }

  if (s->parse_sec==GEN_SEC)
  {
    s->current->spec= VhdlDocGen::GENERIC;
    s->current->section=Entry::FUNCTION_SEC;
  }

  if (s->currP==VhdlDocGen::PROCEDURE)
  {
    s->current->name=impure;
    s->current->exception="";
  }
  else
  {
    s->current->name=fname;
  }

  if (spec==VhdlDocGen::PROCESS)
  {
    s->current->args=fname;
    s->current->name=impure;
    VhdlDocGen::deleteAllChars(s->current->args,' ');
    if (!fname.isEmpty())
    {
      StringVector q1=split(fname.str(),",");
      for (const auto &name : q1)
      {
        Argument arg;
        arg.name=name;
        s->current->argList.push_back(arg);
      }
    }
  }
}


bool VHDLOutlineParser::isFuncProcProced()
{
  VhdlParser::SharedState *s = &p->shared;
  if (s->currP==VhdlDocGen::FUNCTION  ||
      s->currP==VhdlDocGen::PROCEDURE ||
      s->currP==VhdlDocGen::PROCESS
     )
  {
    return TRUE;
  }
  return FALSE;
}

void VHDLOutlineParser::pushLabel( QCString &label,QCString & val)
{
  label+="|";
  label+=val;
}

QCString VHDLOutlineParser::popLabel(QCString & q)
{
  int i=q.findRev("|");
  if (i<0) return QCString();
  q = q.left(i);
  return q;
}



void VHDLOutlineParser::addProto(const char *s1,const char *s2,const char *s3,
                          const char *s4,const char *s5,const char *s6)
{
  VhdlParser::SharedState *s = &p->shared;
  (void)s5; // avoid unused warning
  QCString name=s2;
  StringVector ql=split(name.str(),",");

  for (const auto &n : ql)
  {
    Argument arg;
    arg.name=n;
    if (s3)
    {
      arg.type=s3;
    }
    arg.type+=" ";
    arg.type+=s4;
    if (s6)
    {
      arg.type+=s6;
    }
    if (s->parse_sec==GEN_SEC && s->param_sec==0)
    {
      arg.defval="gen!";
    }

    if (s->parse_sec==PARAM_SEC)
    {
    //  assert(false);
    }

    arg.defval+=s1;
    arg.attrib="";//s6;

    s->current->argList.push_back(arg);
    s->current->args+=s2;
    s->current->args+=",";
  }
}


/*
 * adds the library|use statements to the next class (entity|package|architecture|package body
 * library ieee
 * entity xxx
 * .....
 * library
 * package
 * entity zzz
 * .....
 * and so on..
 */
void VHDLOutlineParser::mapLibPackage( Entry* root)
{
  for (const auto &rt : p->libUse)
  {
    if (addLibUseClause(rt->name))
    {
      bool bFound=FALSE;
      for (const auto &current : root->children())
      {
        if (VhdlDocGen::isVhdlClass(current.get()))
        {
          if (current->startLine > rt->startLine)
          {
            bFound=TRUE;
            current->copyToSubEntry(rt);
            break;
          }
        }
      }//for
      if (!bFound)
      {
        root->copyToSubEntry(rt);
      }
    } //if
  }// for
}//MapLib

bool VHDLOutlineParser::addLibUseClause(const QCString &type)
{
  bool showIEEESTD=Config_getBool(FORCE_LOCAL_INCLUDES);

  if (showIEEESTD) // all standard packages and libraries will not be shown
  {
    if (type.lower().stripPrefix("ieee")) return FALSE;
    if (type.lower().stripPrefix("std")) return FALSE;
  }
  return TRUE;
}

int VHDLOutlineParser::getLine()
{
  return p->yyLineNr;
}

void VHDLOutlineParser::setLineParsed(int tok)
{
  if ((int)p->lineParse.size()<=tok) p->lineParse.resize(tok+1);
  p->lineParse[tok]=p->yyLineNr;
}

int VHDLOutlineParser::getLine(int tok)
{
  int val=p->lineParse[tok];
  if (val<0) val=0;
  //assert(val>=0 && val<=yyLineNr);
  return val;
}


void VHDLOutlineParser::createFlow()
{
  VhdlParser::SharedState *s = &p->shared;
  if (!VhdlDocGen::getFlowMember())
  {
    return;
  }
  QCString q,ret;

  if (s->currP==VhdlDocGen::FUNCTION)
  {
    q=":function( ";
    FlowChart::alignFuncProc(q,s->tempEntry->argList,true);
    q+=")";
  }
  else if (s->currP==VhdlDocGen::PROCEDURE)
  {
    q=":procedure (";
    FlowChart::alignFuncProc(q,s->tempEntry->argList,false);
    q+=")";
  }
  else
  {
    q=":process( "+s->tempEntry->args;
    q+=")";
  }

  q.prepend(VhdlDocGen::getFlowMember()->name());

  FlowChart::addFlowChart(FlowChart::START_NO,q,QCString());

  if (s->currP==VhdlDocGen::FUNCTION)
  {
    ret="end function ";
  }
  else if (s->currP==VhdlDocGen::PROCEDURE)
  {
    ret="end procedure";
  }
  else
  {
    ret="end process ";
  }

  FlowChart::addFlowChart(FlowChart::END_NO,ret,QCString());
  //  FlowChart::printFlowList();
  FlowChart::writeFlowChart();
  s->currP=0;
}

void VHDLOutlineParser::setMultCommentLine()
{
  p->iDocLine=p->yyLineNr;
}

void VHDLOutlineParser::oneLineComment(QCString qcs)
{
    int j=qcs.find("--!");
    qcs=qcs.right(qcs.length()-3-j);
    if (!checkMultiComment(qcs,p->iDocLine))
    {
      handleCommentBlock(qcs,TRUE);
    }
}


bool VHDLOutlineParser::checkMultiComment(QCString& qcs,int line)
{
  VhdlParser::SharedState *s = &p->shared;
  insertEntryAtLine(s->current_root,line);

  if (p->lineEntry.empty()) return false;

  VhdlDocGen::prepareComment(qcs);
  while (!p->lineEntry.empty())
  {
    std::shared_ptr<Entry> e=p->lineEntry.back();
    e->briefLine=line;
    e->brief+=qcs;

    p->lineEntry.pop_back();
  }
  return true;
}

// returns the vhdl parsed types at line xxx
void VHDLOutlineParser::insertEntryAtLine(std::shared_ptr<Entry> ce,int line)
{
  for (const auto &rt : ce->children())
  {
    if (rt->bodyLine==line)
    {
      p->lineEntry.push_back(rt);
    }

    insertEntryAtLine(rt,line);
  }
}

const EntryList &getVhdlInstList()
{
  return g_instFiles;
}

void VHDLOutlineParser::error_skipto(int kind)
{
  Token *op;
  do
  {
    p->vhdlParser->getNextToken();  // step to next token
    op=p->vhdlParser->getToken(1);  // get first token
    if (op==0) break;
    //fprintf(stderr,"\n %s",qPrint(t->image));
  } while (op->kind != kind);
  p->vhdlParser->clearError();
  // The above loop consumes tokens all the way up to a token of
  // "kind".  We use a do-while loop rather than a while because the
  // current token is the one immediately before the erroneous token
  // (in our case the token immediately before what should have been
  // "if"/"while".
}

QCString filter2008VhdlComment(const char *s)
{
  GrowBuf growBuf;
  const char *p=s+3; // skip /*!
  char c='\0';
  while (*p == ' ' || *p == '\t') p++;
  while ((c=*p++))
  {
    growBuf.addChar(c);
    if (c == '\n')
    {
      // special handling of space followed by * at beginning of line
      while (*p == ' ' || *p == '\t') p++;
      while (*p == '*') p++;
      // special attention in case character at end is /
      if (*p == '/') p++;
    }
  }
  // special attention in case */ at end of last line
  size_t len = growBuf.getPos();
  if (growBuf.at(len-1) == '/' && growBuf.at(len-2) == '*')
  {
    len -= 2;
    while (growBuf.at(len-1) == '*') len--;
    c = growBuf.at(len-1);
    while ((c = growBuf.at(len-1)) == ' ' || c == '\t') len--;
    growBuf.setPos(len);
  }
  growBuf.addChar(0);
  return growBuf.get();
}
