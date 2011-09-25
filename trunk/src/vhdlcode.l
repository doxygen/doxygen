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
 * Parser for syntax hightlighting and references for vhdl subset
 * written by M. Kreis
 * supports VHDL-87
 * does not support VHDL-AMS 
 ******************************************************************************/

%{

/*
 *        includes
 */
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>
#include <qdir.h>
#include <qstringlist.h>

#include "qtbc.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "vhdldocgen.h"

#define YY_NEVER_INTERACTIVE 1
  
// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)
  

/* -----------------------------------------------------------------
 *        statics
 */

// ----------------- <vhdl> ----------------------------------

//static bool isPackBody=FALSE;
//static bool isStartMap;
static bool isFuncProto=FALSE;
static bool isComponent=FALSE;
static bool isPackageBody=FALSE;
static bool isProto = FALSE;

static QCString   g_PrevString;
static QCString   g_CurrClass;
static QDict<QCString>g_vhdlKeyDict;   
static QCString   g_tempClass;
static QCString   g_tempComp;
static QCString   g_PortMapComp;
static MemberDef *g_vhdlMember;
static QCString   g_FuncProto;

//-----------------------------------------------------------
  
static CodeOutputInterface * g_code;
static QCString      g_curClassName;
static QCString      g_parmType;
static QCString      g_parmName;
static const char *  g_inputString;     //!< the code fragment as text
static int           g_inputPosition;   //!< read offset during parsing 
static int           g_inputLines;      //!< number of line in the code fragment
static int           g_yyLineNr;        //!< current line number
static bool          g_needsTermination;

static QCString      g_exampleName;
static QCString      g_exampleFile;

static QCString      g_type;
static QCString      g_name;
static QCString      g_args;
static QCString      g_classScope;
   
static QCString      g_CurrScope;
   
static FileDef *     g_sourceFileDef;
static Definition *  g_currentDefinition;
static MemberDef *   g_currentMemberDef;
static bool          g_includeCodeFragment;
static const char *  g_currentFontClass;

static bool          g_lexInit = FALSE;
static int           g_braceCount=0;


static void writeFont(const char *s,const char* text);
static void generateMemLink(CodeOutputInterface &ol,QCString &clName,QCString& memberName);
static bool writeColoredWord(QCString& word );
static void generateClassOrGlobalLink(CodeOutputInterface &ol,const char *clName, bool typeOnly=FALSE);
static void endFontClass();
static void startFontClass(const char *s);
//-------------------------------------------------------------------


static void setCurrentDoc(const QCString &name,const QCString &base,const QCString &anchor="")
{
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(name,base,anchor);
  }
}

static bool checkString(QCString &name)
{
  if (name.isEmpty()) return FALSE;
  static QRegExp regg("[\\s\"]");

  int len=name.length();
  if (name.at(0)=='"' && name.at(len-1)=='"' && len > 2)
  {
    QStringList qrl=QStringList::split(regg,name,FALSE);
    if (VhdlDocGen::isNumber((QCString)qrl[0]))
    {
      g_code->codify("\""); 
      startFontClass("vhdllogic");
      QCString mid=name.mid(1,len-2); //" 1223 "
      g_code->codify(mid.data());
      endFontClass();
      g_code->codify("\""); 
    }
    else
    {
      startFontClass("keyword");
      g_code->codify(name.data());
      endFontClass();
    }
    return TRUE;
  }

  if (VhdlDocGen::isNumber(name))
  {
    startFontClass("vhdllogic");
    g_code->codify(name.data());
    endFontClass();
    return TRUE;
  }
  return FALSE;
}

static void addToSearchIndex(const char *text)
{
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->addWord(text,FALSE);
  }
}


/*! start a new line of code, inserting a line number if g_sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine()
{
  //if (g_currentFontClass) { g_code->endFontClass(); }
  if (g_sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",g_yyLineNr);
    //lineAnchor.sprintf("l%05d",g_yyLineNr);
    //  if ((g_yyLineNr % 500) == 0) 
    //         fprintf(stderr,"\n starting Line %d:",g_yyLineNr);
    Definition *d   = g_sourceFileDef->getSourceDefinition(g_yyLineNr);
    //printf("startCodeLine %d d=%s\n", g_yyLineNr,d ? d->name().data() : "<null>");
    if (!g_includeCodeFragment && d)
    {
      g_currentDefinition = d;
      g_currentMemberDef = g_sourceFileDef->getSourceMember(g_yyLineNr);
      if (!g_tempComp.isEmpty() && g_currentMemberDef )
      {
        //ClassDef *cf=VhdlDocGen::getClass(g_tempComp.data());
        QCString nn=g_currentMemberDef->name();
        MemberDef* mdeff=VhdlDocGen::findMember(g_tempComp,nn);
        if (mdeff)
        {
          g_currentMemberDef=mdeff;
        }
      }

      g_parmType.resize(0);
      g_parmName.resize(0);
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",g_yyLineNr);
      if (g_currentMemberDef)
      {
        g_code->writeLineNumber(g_currentMemberDef->getReference(),
                                g_currentMemberDef->getOutputFileBase(),
                                g_currentMemberDef->anchor(),g_yyLineNr);
        setCurrentDoc(g_currentMemberDef->qualifiedName(),
                      g_sourceFileDef->getSourceFileBase(),
                      lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        g_code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                0,g_yyLineNr);
        setCurrentDoc(d->qualifiedName(),
                      g_sourceFileDef->getSourceFileBase(),
                      lineAnchor);
      }
    }
    else
    {
      g_code->writeLineNumber(0,0,0,g_yyLineNr);
    }
  }
  g_code->startCodeLine(); 
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
}

static void endFontClass();
static void endCodeLine()
{
  endFontClass();
  g_code->endCodeLine();
}

static void nextCodeLine()
{
  const char *fc = g_currentFontClass;
  endCodeLine();
  if (g_yyLineNr<g_inputLines) 
  {
    g_currentFontClass = fc;
    startCodeLine();
  }
}

/*! writes a word to the output.
 *  If curr_class is defined, the word belongs to a class
 *  and will be linked.
 */

static void writeWord(const char *word,const char* curr_class=0,bool classLink=FALSE)
{
  bool found=FALSE;
  QCString temp; 
  QCString tclass(curr_class);
  QCString ttt(word);
  if (ttt.isEmpty()) return;
  for (unsigned int j=0;j<ttt.length();j++)
  {
    char c=ttt.at(j);
    if (c==' '|| c==',' || c==';' || c==':' || c=='(' || c==')' || c=='\r' || c=='\t' || c=='.')
    {
      if (found)
      {
        if (!writeColoredWord(temp)) // is it a keyword ?
        {
          //if (VhdlDocGen::findKeyWord(temp))
          // writeFont("vhdlkeyword",temp.data());
	  //printf("writeWord: %s\n",temp.data());
          if (!tclass.isEmpty())
          {
            if (!classLink)
	    {
              generateMemLink(*g_code,tclass,temp);
	    }
            else
            {
              generateClassOrGlobalLink(*g_code,temp);
            }
          }
          else                                                                          
	  {
          if (!checkString(temp))
            g_code->codify(temp.data());
	  }
        }
        temp.resize(0);
        found=FALSE;
      }

      char cc[2];
      cc[0]=c;
      cc[1]=0;
      g_code->codify(cc);
    }
    else
    {
      found=TRUE;
      temp+=c;
    }
  } // for

  if (!temp.isEmpty())
  {
    if (!writeColoredWord(temp))
    {
      if (!tclass.isEmpty())
      {
        if (!classLink)
        {
          generateMemLink(*g_code,tclass,temp); // generateMemLink(*g_code,g_CurrClass,left); 
        }
        else
        {
          generateClassOrGlobalLink(*g_code,temp);
        }
      }
      else                 
      {
         QCString qc(temp.data());
         if (VhdlDocGen::isNumber(qc)){
                         startFontClass("vhdllogic");
                         g_code->codify(temp.data());
                         endFontClass();
                              }
         else     
        g_code->codify(temp.data());
      }
    }
  }
}// writeWord


/*! write a code fragment `text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(const char *text,const char *cl=0,bool classlink=FALSE)
{
  if (text==0) return;
  //printf("codifyLines(%d,\"%s\")\n",g_yyLineNr,text);
  const char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') {}
    if (c=='\n')
    {
      g_yyLineNr++;
      QCString line = sp;
      line = line.left(p-sp-1);
      //*(p-1)='\0';
      //g_code->codify(sp);
      writeWord(line,cl,classlink);
      nextCodeLine();
    }
    else
    {
      //g_code->codify(sp);
      writeWord(sp,cl,classlink);
      done=TRUE;
    }
  }
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be 
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(CodeOutputInterface &ol,
                  const char *ref,const char *file,
                  const char *anchor,const char *text,
		  const char *tooltip)
{
  bool done=FALSE;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') {}
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      // printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      nextCodeLine();
    }
    else
    {
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}

static void setParameterList(MemberDef *md)
{
  g_classScope = md->getClassDef() ? md->getClassDef()->name().data() : "";
  LockingPtr<ArgumentList> al = md->argumentList();
  if (al==0) return; 
  Argument *a = al->first();
  while (a)
  {
    g_parmName = a->name.copy();
    g_parmType = a->type.copy();
    int i = g_parmType.find('*');
    if (i!=-1) g_parmType = g_parmType.left(i);
    i = g_parmType.find('&');
    if (i!=-1) g_parmType = g_parmType.left(i);
    g_parmType.stripPrefix("const ");
    g_parmType=g_parmType.stripWhiteSpace();
   // g_theVarContext.addVariable(g_parmType,g_parmName);
    a = al->next();
  }
}


/*! writes a link to a function or procedure
 */

static void generateFuncLink(CodeOutputInterface &ol,MemberDef* mdef)
{

  //printf("generateFuncLink(FuncName=%s)\n",mdef->name().data());
  QCString memberName=mdef->name();

  if (mdef && mdef->isLinkable()) // is it a linkable class
  {
    writeMultiLineCodeLink(ol,mdef->getReference(),
	                      mdef->getOutputFileBase(),
			      mdef->anchor(),
			      mdef->name(),
			      mdef->briefDescriptionAsTooltip());
    addToSearchIndex(memberName);
    return;
  }
  ol.linkableSymbol(g_yyLineNr,memberName,0,g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
  codifyLines(memberName.data());
  addToSearchIndex(memberName);
} // generateFuncLink


static void generateMemLink(CodeOutputInterface &ol,QCString &clName,QCString& memberName)
{
  if (clName.isEmpty() || memberName.isEmpty()) return; 
  QCString className=clName;

  MemberDef *md=0;
  //MemberDef *comp=0;
  //bool isLocal=FALSE;

  md=VhdlDocGen::findMember(className,memberName);
  ClassDef *po=VhdlDocGen::getClass(className.data());

  if (md==0 && po && (VhdlDocGen::VhdlClasses)po->protection()==VhdlDocGen::PACKBODYCLASS) 
  {
    QCString temp=className;//.stripPrefix("_");
    temp.stripPrefix("_");
    md=VhdlDocGen::findMember(temp,memberName);
  }

  if (md && md->isLinkable()) // is it a linkable class
  {
    writeMultiLineCodeLink(ol,md->getReference(),
	                      md->getOutputFileBase(),
			      md->anchor(),
			      memberName,
			      md->briefDescriptionAsTooltip());
    addToSearchIndex(memberName);
    return;
  }
  // nothing found, just write out the word
  ol.linkableSymbol(g_yyLineNr,memberName,0,g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
  codifyLines(memberName.data());
  addToSearchIndex(memberName);
}// generateMemLink


static void generateClassOrGlobalLink(CodeOutputInterface &ol,const char *clName, bool /*typeOnly*/)
{
  QCString className=clName;

  if (className.isEmpty()) return;

  ClassDef *cd=0;
  //MemberDef *md=0;
  //bool isLocal=FALSE;
  className.stripPrefix("_");
  cd = getClass(className.data()); 
  while (cd)
  {
    //className.stripPrefix("_");
    QCString temp(clName);
    temp.stripPrefix("_");
    if (cd && cd->isLinkable()) // is it a linkable class
    {
      //if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS)
      //{
      //  temp=VhdlDocGen::getClassName(cd);
      //}
      ol.linkableSymbol(g_yyLineNr,temp,cd,
                        g_currentMemberDef ? 
			g_currentMemberDef : 
			g_currentDefinition);
      writeMultiLineCodeLink(ol,cd->getReference(),
	                        cd->getOutputFileBase(),
				cd->anchor(),
				temp,
				cd->briefDescriptionAsTooltip());
      addToSearchIndex(className);
      return;
    }
    Definition *d = cd->getOuterScope();
    if (d && d->definitionType()==Definition::TypeClass)
    {
      cd = (ClassDef*)d;
    }
    else
    {
      cd = 0;
    }
  }

  // nothing found, just write out the word
  ol.linkableSymbol(g_yyLineNr,clName,0,g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
  codifyLines(clName);
  addToSearchIndex(clName);
}// generateClasss or global link


/*! counts the number of lines in the input */
static int countLines()
{
  const char *p=g_inputString;
  char c;
  int count=1;
  while ((c=*p)) 
  { 
    p++ ; 
    if (c=='\n') count++;  
  }
  if (p>g_inputString && *(p-1)!='\n') 
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++, 
    g_needsTermination=TRUE; 
  } 
  return count;
}

static void endFontClass()
{
  if (g_currentFontClass)
  {
    g_code->endFontClass();
    g_currentFontClass=0;
  }
}

static void startFontClass(const char *s)
{
  if (s==0) return;
  endFontClass();
  g_code->startFontClass(s);
  g_currentFontClass=s;
}

static void writeFont(const char *s,const char* text)
{
  if (s==0 || text==0) return;
  //printf("writeFont(%d,\"%s\")\n",g_yyLineNr,text);
  g_code->startFontClass(s);
  g_code->codify(text);
  g_code->endFontClass();
}

//----------------------------------------------------------------------------

static void appStringLower(QCString& qcs,const char* text)
{
  qcs.resize(0);
  qcs.append(text);
  //qcs=qcs.lower();
  qcs=qcs.stripWhiteSpace();
}

//static void appString(QCString& qcs,const char* text)
//{
//  qcs.resize(0);
//  qcs.append(text);
//}

static QCString g_temp;

/* writes and links a port map statement */
static void codifyMapLines(char *text)
{
  if (text==0) return;
  g_temp.resize(0);       
  //bool dot=FALSE;
  int wordCounter=0;
  QCString ctemp;
  //printf("codifyLines(%d,\"%s\")\n",g_yyLineNr,text);
  char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) &&  c!='\n' && c!=':' && c != ' ' && c != '(' && c!='\0' && c!='\t')
    { 
      if (c!=0x9)
	g_temp+=c; 
    }
    if (c=='\0') return;
    if (!g_temp.isEmpty()) wordCounter++;

    if (!g_temp.isEmpty())
    {
      // different kinds of component instantiations
      // xxx:yyy (generic/port) map(
      // xxx:(entity/component/configuration) yyy (generic/port) map(
      // xxx: entity yyy(zzz) (generic/port) map(
      if (wordCounter==2 || wordCounter==3)
      {
	QCString q=g_temp.lower(); // consider (upper/lower) cases
	if (q=="entity" || q=="component" || q=="configuration" || q=="port" || q=="generic")
	{
	  generateMemLink(*g_code,g_CurrClass,g_temp); 
	}
	else
	{
	  g_PortMapComp=g_temp;
	  generateClassOrGlobalLink(*g_code,g_temp);
	}
      }
      else 
      {
	generateMemLink(*g_code,g_CurrClass,g_temp); 
      }
    }
    ctemp.fill(c,1);
    codifyLines(ctemp.data()); 
    ctemp.resize(0);
    g_temp.resize(0); 
  }//while
}//codifymaplines

/*
* writes a function|procedure prototype  and links the function|procedure name 
*/

static void writeFuncProto()
{
  QList<Argument> ql;
  QCString name,ret;
  VhdlDocGen::parseFuncProto(g_FuncProto,ql,name,ret,FALSE);

  if (name.isEmpty())
  {
    codifyLines(g_FuncProto.data(),g_CurrClass.data());
    return;
  }
  QStringList qlist=QStringList::split(name,g_FuncProto,FALSE);
  QCString temp=(QCString)qlist[0];
  codifyLines(temp.data(),g_CurrClass.data());
  g_FuncProto.stripPrefix(temp.data());
  temp.resize(0);
  temp=g_CurrClass;
  if (isPackageBody) 
  {
    temp.stripPrefix("_");// _{package body name}
  }
  MemberDef *mdef=VhdlDocGen::findFunction(ql,name,temp,FALSE);

  if (mdef)
  {
    generateFuncLink(*g_code,mdef);
    g_FuncProto.stripPrefix(name.data());
    codifyLines(g_FuncProto.data(),g_CurrClass.data());
  }
  else
  {
    codifyLines(g_FuncProto.data(),g_CurrClass.data());
  }
}// writeFuncProto

/* writes a process prototype to the ouput */

 static void writeProcessProto(){
 codifyLines(g_FuncProto.data(),g_CurrClass.data());
 g_vhdlKeyDict.clear();
}// writeProcessProto

/* writes a keyword */

static bool writeColoredWord(QCString& word )
{
  QCString qcs=word.lower();
  QCString *ss=VhdlDocGen::findKeyWord(qcs);
  if (ss) 
  {
    writeFont(ss->data(),word.data());
    return TRUE;
  }
  return FALSE;
}

#undef        YY_INPUT
#define       YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  while( c < max_size && g_inputString[g_inputPosition] )
  {
    *buf = g_inputString[g_inputPosition++] ;
    c++; buf++;
  }
  return c;
}

%}


B             [ \t]
BN            [ \t\n\r]
STRING      ["][^"\n]*["]
NAME          [a-z_A-Z][ a-z_A-Z0-9]*
FUNCNAME      [a-z_A-Z"][a-z_A-Z0-9+*"/=<>-]*
ID            "$"?[a-z_A-Z][a-z_A-Z0-9]*
SPECSIGN      [:;, +*&\/=<>'\t]*
DIGITSS       [0-9]+|[0-9]+("#")*[0-9_a-fA-F\+\.\-]+("#")*
ALLTYPESMAP   {B}*[_a-zA-Z0-9. ]+{BN}*
ALLTYPESMAP1  {BN}*[_a-zA-Z0-9.() ]+{BN}*

ARCHITECTURE  ^{B}*("architecture"){BN}+{FUNCNAME}{BN}+("of"){BN}+{FUNCNAME}
PROCESS       ({BN}*{FUNCNAME}{BN}*[:]+{BN}*("process"){BN}*[(]*)|[^a-zA-Z]("process "|"process("){BN}*[ (]*|[^a-zA-Z]("process"){BN}+

END1          {B}*("end "){BN}+("if"|"case"|"loop"|"generate"|"for")
END2          [^a-zA-Z_]("end"){BN}*[;]
END3          {BN}*[^a-zA-Z]("end"){BN}+{FUNCNAME}{BN}*[;]
END4          {B}*("end"){BN}+"function"{BN}+{FUNCNAME}{BN}*[;]
ENDEFUNC      {END3}|{END4}|{END2}

KEYWORD       ("new"|"event"|"break"|"case"|"end"|"loop"|"else"|"for"|"goto"|"if"|"return"|"generate"|"is"|"while"|"in")
TYPEKW        ^{B}*("type"|"subtype"|"constant"|"attribute"|"signal"|"variable","alias","configuration")
FUNC          ^{B}*("function"|"procedure"){BN}*{FUNCNAME}{BN}*("(")

ARITHOP       "+"|"-"|"/"|"*"|"%"|"/="|":="
ASSIGNOP      "="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|="
LOGICOP       "=="|"!="|">"|"<"|">="|"<="|"&&"|"||"|"!"
BITOP         "&"|"|"|"^"|"<<"|">>"|"~"
OPERATOR      {ARITHOP}|{ASSIGNOP}|{LOGICOP}|{BITOP}

PORT          {B}*("port"){BN}*("(")
GENERIC       {B}*("generic"){BN}*("(")

BRACEOPEN     [(]{1}
BRACECLOSE    [)]{1}

TEXTT         {B}*"--"[^\n]*

MAPCOMPONENT1 ({ALLTYPESMAP}[:]{ALLTYPESMAP}{TEXTT}*{BN}+("port"|"generic"){BN}+("map"){BN}*("("){1})
MAPCOMPONENT2 {BN}*("port"|"generic"){BN}+("map"){BN}*("("){1}
MAPCOMPONENT3 ({ALLTYPESMAP}[:]{BN}*{ALLTYPESMAP1}{TEXTT}*{BN}+("port"|"generic"){BN}+("map"){BN}*("("){1})
MAPCOMPONENT4 ({ALLTYPESMAP}[:]{BN}*("entity"|"component"|"configuration"){BN}+{ALLTYPESMAP1}{TEXTT}*{BN}*("port"|"generic"){BN}*("map"){BN}*("("){1})

%option noyywrap
%option nounput

%x Bases
%x ParseType
%x ParseFuncProto
%x ParseComponent
%x ParsePackage
%x ParseProcessProto
%x ClassName
%x PackageName
%x ClassVar
%x ClassesName
%x Map
%x Body
        
%%

.                     { 
                        BEGIN(Bases); 
                      }

<Map>{BRACEOPEN}      {
                        g_braceCount++;
                        writeFont("vhdlchar",vhdlcodeYYtext);
                        BEGIN(Map);
                      }

<Map>[^()\n,--]*        { /* write and link a port map lines */
                        QCString tt(vhdlcodeYYtext);
                        VhdlDocGen::deleteAllChars(tt,',');
                        QRegExp r("=>");
			QStringList ql=QStringList::split(r,tt,FALSE);
			if (ql.count()>=2)
			{
			  unsigned int index=0;
			  QCString t1=(QCString)ql[0];
			  char cc=t1.at(index);
			  while (cc==' ' || cc=='\t') 
			  {
			    char c2[2];
			    c2[0]=cc;
			    c2[1]=0;
			    g_code->codify(c2);
			    index++;
			    if (index>=t1.size()) break;
			    cc=t1.at(index);
			  }        

			  QCString s1=(QCString)ql[0];
			  s1=s1.stripWhiteSpace();

			  //         if (!g_PortMapComp.isEmpty())
			  generateMemLink(*g_code,g_PortMapComp,s1); 
			  while (index++<t1.size()) 
			  { 
			    char cc=t1.at(index);
			    if (cc==' ' || cc=='\t')
			    {
			      char c2[2];
			      c2[0]=cc;
			      c2[1]=0;
			      g_code->codify(c2);
			    }
			  }        
			  codifyLines("=>");
			  index=0;
			  QCString s2=(QCString)ql[1];
			  t1=s2;
			  cc=t1.at(index);
			  while (cc==' ' || cc=='\t') 
			  {
			    char c2[2];
			    c2[0]=cc;
			    c2[1]=0;
			    g_code->codify(c2);
			    index++;
			    if (index>=t1.size()) break;
			    cc=t1.at(index);
			  }        
			  s2=s2.stripWhiteSpace();
			    if (!checkString(s2))
			  generateMemLink(*g_code,g_CurrClass,s2); 
			  while (index++<t1.size()) 
			  { 
			    if (t1.at(index)==' ')
			    {
			      g_code->codify(" "); 
			    }
			  }        
			}
			else
			{
			  codifyLines(vhdlcodeYYtext,g_CurrClass.data());
			}
			BEGIN(Map);
                      }

<Map>"\n"|","         {
                        codifyLines(vhdlcodeYYtext);
                        BEGIN(Map);
                      }

<Map>{BRACECLOSE}     {
                        g_braceCount--;
                        writeFont("vhdlchar",vhdlcodeYYtext);
                        if (g_braceCount==0)
			{
                          BEGIN(Bases);
			}
                      }

<ParseFuncProto>{NAME} {
                         QCString tmp(vhdlcodeYYtext);
                         tmp=tmp.stripWhiteSpace();
                         appStringLower(g_PrevString,vhdlcodeYYtext);
                         g_vhdlKeyDict.insert(g_PrevString,new QCString(g_PrevString.data()));
                         if (!writeColoredWord(tmp))
			 {
                           generateMemLink(*g_code,g_CurrClass,tmp); 
			 }
                         BEGIN(Bases);
                       }

<ParseType>{STRING} {
                       QCString qcs(vhdlcodeYYtext);
                       VhdlDocGen::deleteAllChars(qcs,'"');
                       VhdlDocGen::deleteAllChars(qcs,' ');
                        if (VhdlDocGen::isNumber(qcs))
                       writeFont("vhdllogic",vhdlcodeYYtext);
                       else
                       writeFont("keyword",vhdlcodeYYtext);
                }

<ParseType>"\n"       {
                        g_FuncProto.append(vhdlcodeYYtext);
                        if (isProto)
			{
                          codifyLines(vhdlcodeYYtext);
			}
                        BEGIN(ParseType);
                      }


<ParseType>{TEXTT}    {
                        g_FuncProto.append(vhdlcodeYYtext);                
                        if (isProto)                                                                 
		        {
                          writeFont("keyword",vhdlcodeYYtext);
		        }
                        BEGIN(ParseType); 
                       }

<ParseType>{ENDEFUNC} {
  			QRegExp regg("[\\s]");
                        QCString tt(vhdlcodeYYtext);
                        codifyLines(vhdlcodeYYtext,g_CurrClass.data());
                        tt=tt.lower();
                        VhdlDocGen::deleteAllChars(tt,';');
                        tt.stripWhiteSpace();
                        QStringList ql=QStringList::split(regg,tt,FALSE);
                        int index=ql.findIndex(QCString("if"))+1;
                        index+=ql.findIndex(QCString("case"))+1;
                        index+=ql.findIndex(QCString("loop"))+1;
                        index+=ql.findIndex(QCString("generate"))+1;                        
                        if (index==0)
		        {
                          BEGIN(Bases);
		        }
                        else
		        {
                          BEGIN(ParseType);
	        	}
                      }

<ParseType>{END1}     {
                        codifyLines(vhdlcodeYYtext,g_CurrClass.data());
                        g_vhdlKeyDict.clear();
                      }

<ParseType>^{B}*("begin "|"begin") {
                        codifyLines(vhdlcodeYYtext,g_CurrClass.data());
                        isFuncProto=FALSE;
                      }

<ParseType>{SPECSIGN} {
                        g_FuncProto.append(vhdlcodeYYtext);
                        if (isProto)
			{
                          codifyLines(vhdlcodeYYtext,g_CurrClass.data());
			}
                      }

<ParseType>["_a-zA-Z0-9]* {
                        QCString val(vhdlcodeYYtext);
                        g_FuncProto.append(vhdlcodeYYtext);
                        appStringLower(g_PrevString,vhdlcodeYYtext);
                                                        
                        if (isFuncProto && g_braceCount==0)
                        {
                          g_vhdlKeyDict.insert(g_PrevString,new QCString(g_PrevString.data()));
		        }
                                
                        if (isProto) 
		        { 
                          if (!writeColoredWord(val))
                          {
                            if (!isFuncProto && !g_vhdlKeyDict.find(g_PrevString))
                            {
                              val=val.stripWhiteSpace();
			      if (VhdlDocGen::isNumber(val))
			      {
				startFontClass("vhdllogic");
				codifyLines(vhdlcodeYYtext,g_CurrClass.data());
				endFontClass();
			      }
                             else
                              generateMemLink(*g_code,g_CurrClass,val); 
                            }
                            else
			    {
                              codifyLines(vhdlcodeYYtext,g_CurrClass.data());
			    }
                          }
                        }
                        BEGIN(ParseType); 
                      }

<ParseType>{BRACEOPEN} {
                        g_braceCount++;
                        g_FuncProto+='(';
                        if (isProto)
		        {
                          writeFont("vhdlchar",vhdlcodeYYtext);
		        }
                        BEGIN(ParseType);
                      }

<ParseType>{BRACECLOSE} {
                        g_braceCount--;
                        g_FuncProto+=')';
                        if (isProto)
			{
                          writeFont("vhdlchar",vhdlcodeYYtext);
			}
                        if (g_braceCount==0 && !isProto)// && !isPackageBody)
                        {
                          isProto=TRUE;
                          appStringLower(g_PrevString,vhdlcodeYYtext);
                          writeFuncProto();
                          BEGIN(Bases);
                        }
                        if (isPackageBody)
			{
                          BEGIN(ParseType);
			}
                      }


<ClassesName>{FUNCNAME} {
                         QDict<QCString> mem;
                         appStringLower(g_PrevString,vhdlcodeYYtext);
                         g_CurrClass.resize(0);
                         g_CurrClass.append(vhdlcodeYYtext);
                         g_CurrClass=g_CurrClass.stripWhiteSpace();
                                                  
                         if (!writeColoredWord(g_CurrScope))
			 {
                           generateClassOrGlobalLink(*g_code,vhdlcodeYYtext);
			 }
                         else
		         {
                           codifyLines(vhdlcodeYYtext,g_CurrClass.data());
		         }
                         BEGIN(Bases);
                       }


<ParseComponent>{BRACEOPEN} {
                         g_braceCount++;
                         g_code->codify(vhdlcodeYYtext);
                       }


<ParseComponent>{BRACECLOSE} {
                         g_braceCount--;
                         g_code->codify(vhdlcodeYYtext);  
                         if (g_braceCount==0 && !isComponent)
                         {
                           g_tempComp.resize(0);
                           BEGIN(Bases);
                         }
                         else
			 {
                           BEGIN(ParseComponent);
			 }
                       }

<ParseComponent>{B}*"-" {    
                         if (strlen(vhdlcodeYYtext)>=2) // found text ?
			 {
                           writeFont("keyword",vhdlcodeYYtext);
			 }
                         else
			 {
                           writeFont("vhdlchar",vhdlcodeYYtext); 
			 }
                       }

<ParseComponent>{SPECSIGN} {
                         codifyLines(vhdlcodeYYtext);
                       }



<ParseComponent>"\n"|" " {
                           codifyLines(vhdlcodeYYtext);
                         }

<ParseComponent>{DIGITSS} { 
                         startFontClass("vhdllogic");
                         codifyLines(vhdlcodeYYtext);
                         endFontClass();
                       }

<ParseComponent>{PORT} { 
                         codifyLines(vhdlcodeYYtext);
                         g_braceCount=1;
                         isComponent=FALSE;
                       }

<ParseComponent>{GENERIC} { 
                         codifyLines(vhdlcodeYYtext);
                         g_braceCount=1;
                       }

<ParseComponent>[_a-zA_Z][_a-zA-Z0-9]*  {
                         QCString temp(vhdlcodeYYtext);
                         appStringLower(g_PrevString,vhdlcodeYYtext);
                         if (!checkString(temp)){
                         if (!writeColoredWord(g_PrevString))
		         {
                           generateMemLink(*g_code,g_tempComp,temp); 
		         }
                       }
                       }

<ParseComponent>{STRING} { 
                            QCString temp(vhdlcodeYYtext);
                        if (!checkString(temp))
                         codifyLines(vhdlcodeYYtext);
                        }


<ParseProcessProto>[^()]* { 
                         g_FuncProto.append(vhdlcodeYYtext);
                       }



<ParseProcessProto>{BRACEOPEN} {
                         g_FuncProto.append(vhdlcodeYYtext);
                         g_braceCount++;
                       }

<ParseProcessProto>{BRACECLOSE} {
                         g_FuncProto.append(vhdlcodeYYtext);
                         g_braceCount--;
                         if (g_braceCount==0) 
			 {
                            writeProcessProto();
                            BEGIN(Bases);
                          }
                        }

<ParsePackage>[^:;]*    { //found package 
                          QCString temp(vhdlcodeYYtext);
                          QStringList strl=QStringList::split(".",temp,FALSE);
                                
                          if (strl.count()>2) 
			  {
                             QCString s1=(QCString)strl[0];  
                             QCString s2=(QCString)strl[1];  
                             QCString s3=(QCString)strl[2];  
                             s1.append(".");
                             s3.prepend(".");
                             codifyLines(s1.data(),g_CurrClass.data());  
                             ClassDef *cd=VhdlDocGen::getPackageName(s2);
                             if (cd)
			     {
                               generateClassOrGlobalLink(*g_code,s2.data());
			     }
                             else 
			     {
                               codifyLines(s2.data());
			     }
                             codifyLines(s3.data());
                           }
                           else
			   {
                             writeFont("keywordflow",vhdlcodeYYtext);
			   }
                           BEGIN(Bases);
                         }

<Bases>{MAPCOMPONENT1}|{MAPCOMPONENT2}|{MAPCOMPONENT3}|{MAPCOMPONENT4}  { // found port or generic map
                           QCString tt(vhdlcodeYYtext);
             /*
                           if (tt.contains(':',FALSE))
			   {
                             isStartMap=TRUE;
			   }
                           else
			   {
                             isStartMap=FALSE;                                        
			   }
			   */
			   int j=tt.find('.');
                                                    
                           if (j>0)
                           {
                             QCString left=tt.left(j+1); 
                             codifyLines(left.data());
                             tt=tt.right(tt.length()-j-1);
                             left=VhdlDocGen::getIndexWord(tt.data(),0);
                             if (!left.isEmpty())
                             {          
                               if (left.contains('('))
			       {
                                 j=left.find('(',FALSE);
                                 QCString name=left.left(j);
                                 generateClassOrGlobalLink(*g_code,name.data()); 
                                 g_PortMapComp=name;
                                 name=tt.right(tt.length()-name.length());
                                 codifyLines(name.data());
                               }
                               else
			       {
                                 generateClassOrGlobalLink(*g_code,left.data()); 
                                 tt=tt.right(tt.length()-left.length()-1);
                                 tt.prepend(" ");
                                 g_PortMapComp=left;
                                 codifyLines(tt.data());
                               }
                             }
                           }
                           else
			   {
                           if (tt.contains(':',FALSE)) 
                             codifyMapLines(tt.data());
                              else
                             codifyLines(tt.data()); 
			   }
                           g_braceCount=1;
                           BEGIN(Map);
                         }

<Bases>^{B}*("component"){BN}+{FUNCNAME}  { // found component
                           appStringLower(g_PrevString,vhdlcodeYYtext);
                           //  writeFont("keywordflow",VhdlDocGen::getIndexWord(vhdlcodeYYtext,0).data());
                           //  writeFont("vhdlkeyword"," ");
                           QCString temp=VhdlDocGen::getIndexWord(vhdlcodeYYtext,1);
                           temp=temp.stripWhiteSpace();
                           VhdlDocGen::deleteAllChars(temp,'\n');
                           g_tempComp=temp;
                           codifyLines(vhdlcodeYYtext,temp.data(),TRUE);
			   g_braceCount=0;
                         
                           //if (getClass(temp.data()))
                           //  generateClassOrGlobalLink(*g_code,temp.data());
                           //else
                           //  generateMemLink(*g_code,g_CurrClass,temp);
                         
                           isComponent=TRUE;
                           BEGIN(ParseComponent);
                         }
                                                


<Bases>{ARCHITECTURE}    { // found architecture
                           g_PortMapComp.resize(0);
                           //        writeFont("vhdlkeyword",VhdlDocGen::getIndexWord(vhdlcodeYYtext,0).data());
                           //        writeFont("vhdlkeyword"," ");
                           //        writeFont("vhdlchar",VhdlDocGen::getIndexWord(vhdlcodeYYtext,1).data());
                           //        writeFont("vhdlkeyword"," ");
                           //        writeFont("vhdlkeyword",VhdlDocGen::getIndexWord(vhdlcodeYYtext,2).data());
                           //        writeFont("vhdlkeyword"," ");
                           //QCString temp=VhdlDocGen::getIndexWord(vhdlcodeYYtext,1);
                           //temp=temp.stripWhiteSpace();
                           //temp+=("-");
                           //temp+=VhdlDocGen::getIndexWord(vhdlcodeYYtext,3);
			   QCString temp = VhdlDocGen::getIndexWord(vhdlcodeYYtext,3);
			   temp+="::";
			   temp+=VhdlDocGen::getIndexWord(vhdlcodeYYtext,1);
                           g_CurrClass=temp;
                           VhdlDocGen::deleteAllChars(temp,'\n');
                           codifyLines(vhdlcodeYYtext,temp.data(),TRUE);
                           //generateClassOrGlobalLink(*g_code,temp.data());
                           isPackageBody=FALSE;
                           BEGIN(ClassName);
                         }


<Bases>^{B}*("package "){BN}*("body"){BN}*{FUNCNAME}  { // found package body  
                           QCString ss(vhdlcodeYYtext);
                           QCString temp=VhdlDocGen::getIndexWord(vhdlcodeYYtext,2);
                           QStringList ql=QStringList::split(temp,ss,FALSE);
                           QCString ll=(QCString)ql[0];
                           codifyLines(ll.data(),g_CurrClass.data());
                           temp=temp.stripWhiteSpace();
                                     temp.prepend("_");
                           generateClassOrGlobalLink(*g_code,temp.data());
                           g_CurrClass.resize(0);
                           g_CurrClass=temp;
                           isProto=FALSE;
                           isPackageBody=TRUE;
                           // BEGIN(ClassesName);
                         }

<Bases>{PROCESS}         { // found process
                           isFuncProto=TRUE;
                           g_FuncProto.resize(0);
                           g_FuncProto.append(vhdlcodeYYtext);
                           g_vhdlKeyDict.clear();
                           appStringLower(g_PrevString,vhdlcodeYYtext);
                           if (g_PrevString.contains('(')) 
                           {
                             g_braceCount=1;
                             BEGIN(ParseProcessProto);
                           }
                           else
			   {
			     writeProcessProto();
			   }
                         }

<Bases>("end"){BN}+("process") { // end of process
                           isFuncProto=FALSE;
                           codifyLines(vhdlcodeYYtext);
                           BEGIN(Bases);
                         }


<Bases>^{B}*("begin "|"begin") { 
                           isFuncProto=FALSE;
                           writeFont("vhdlkeyword",vhdlcodeYYtext);
                         }

<Bases>^{B}*("use"|"library"){BN}+ { //found  package or library
                           writeFont("vhdlkeyword",vhdlcodeYYtext);
                           BEGIN(ParsePackage);
                         }


<Bases>^{B}*("use"){BN}+("configuration")[^\n]* { 
                           codifyLines(vhdlcodeYYtext);
                         }



<Bases>{FUNC}            {  // found function|procedure
                           g_vhdlKeyDict.clear();
                           g_FuncProto.resize(0);
                           isProto=FALSE;
                           g_FuncProto.append(vhdlcodeYYtext);
                           g_braceCount=1;
                           BEGIN(ParseType);
                         }
                                        


<Bases>^{B}*("entity"|"package"){BN}+ { 
                           appStringLower(g_PrevString,vhdlcodeYYtext);
                           writeFont("keywordflow",vhdlcodeYYtext);
                           isPackageBody=FALSE;
                           BEGIN(ClassesName);
                         }
                                        

<Bases>{KEYWORD}         { // found keyword
                           QCString qcs(vhdlcodeYYtext);
                           if (!writeColoredWord(qcs))
                           {
                             startFontClass("vhdlchar");
                             g_code->codify(vhdlcodeYYtext);
                             endFontClass();
                           }
                         }


<Bases>{ID}              {
                           appStringLower(g_PrevString,vhdlcodeYYtext);
                           QCString temp(vhdlcodeYYtext);
                           temp=temp.stripWhiteSpace();
                        
                           if (!writeColoredWord(temp))
                           {
                             startFontClass("vhdlchar");
                             generateMemLink(*g_code,g_CurrClass,temp);
                             endFontClass();
                           }
                         }

<Bases,ParseComponent>{DIGITSS} { 
                           startFontClass("vhdllogic");
                           codifyLines(vhdlcodeYYtext);
                           endFontClass();
                         }

<Bases>^{B}*("use"){BN}+("entity"|"component")[^\n]* { 
                            codifyLines(vhdlcodeYYtext,g_CurrClass.data(),TRUE);
                         }


<Bases>{TYPEKW}          { 
                           codifyLines(vhdlcodeYYtext);
                           if (isFuncProto)
			   {
                             BEGIN(ParseFuncProto);
			   }
                           else 
			   {
                             BEGIN(Bases);
			   }
                         }

<Bases>{OPERATOR}        {                            
                           startFontClass("vhdlchar");
                           g_code->codify(vhdlcodeYYtext);
                           endFontClass();
                         }
   
<Bases>","|"."|":"|"'"|"("|")" { 
                           startFontClass("vhdlchar");
                           g_code->codify(vhdlcodeYYtext);
                           endFontClass();
                         }
                                          
<Bases>{STRING} {
                       QCString qcs(vhdlcodeYYtext);
                       VhdlDocGen::deleteAllChars(qcs,'"');
                       VhdlDocGen::deleteAllChars(qcs,' ');
                    
                       if (VhdlDocGen::isNumber(qcs))
                       writeFont("vhdllogic",vhdlcodeYYtext);
                       else
                       writeFont("keyword",vhdlcodeYYtext);
                }

<*>\n                    {
			   codifyLines(vhdlcodeYYtext);
			   BEGIN(Bases);
                         }

<*>.                     {
                           g_code->codify(vhdlcodeYYtext);
                         }

<*>\n{TEXTT}             { // found normal or special comment on its own line
			   QCString text(vhdlcodeYYtext);
			   int i=text.find("--");
			   if (text.mid(i,3)=="--!" && // hide special comment
                               Config_getBool("STRIP_CODE_COMMENTS")) 
			   { 
			     g_yyLineNr++; // skip complete line
			   }  
			   else // normal comment
			   {
			     startFontClass("comment");
			     codifyLines(text);
			     endFontClass();
			   }
                         }
<*>{TEXTT}               { // found normal or special comment after something
			   QCString text(vhdlcodeYYtext);
			   int i=text.find("--");
			   if (text.mid(i,3)=="--!" &&
                               Config_getBool("STRIP_CODE_COMMENTS")) 
			   { 
			      // hide special comment
			   }  
			   else // normal comment
			   {
			     startFontClass("comment");
			     codifyLines(text);
			     endFontClass();
			   }
                         }


%%

/*@ ----------------------------------------------------------------------------
 */

void resetVhdlCodeParserState()
{
  g_vhdlKeyDict.setAutoDelete(TRUE);
  g_vhdlKeyDict.clear();
}

void parseVhdlCode(CodeOutputInterface &od,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
                  int startLine,int endLine,bool inlineFragment,
                  MemberDef *memberDef,bool)
{
  //printf("***parseCode() exBlock=%d exName=%s fd=%p\n",exBlock,exName,fd);
  if (s.isEmpty()) return;
  if (memberDef)
  {
    ClassDef *dd=memberDef->getClassDef();
    if (dd) g_CurrClass=dd->className();
    startLine--;
  }
  resetVhdlCodeParserState();
  g_code = &od;
  g_inputString   = s;
  g_inputPosition = 0;
  g_currentFontClass = 0;
  g_needsTermination = FALSE;

  if (endLine!=-1)
    g_inputLines  = endLine+1;
  else
    g_inputLines  = countLines();

  if (startLine!=-1)
    g_yyLineNr    = startLine;
  else
    g_yyLineNr    = 1;


  // g_theCallContext.clear();
  g_classScope    = className;
  g_exampleName   = exName;
  g_sourceFileDef = fd;
  if (exBlock && fd==0)
  {
    // create a dummy filedef for the example
    g_sourceFileDef = new FileDef("",exName);
  }
  if (g_sourceFileDef) 
  {
    setCurrentDoc(g_sourceFileDef->name(),g_sourceFileDef->getSourceFileBase());
  }
  g_currentDefinition = 0;
  g_currentMemberDef = 0;
  g_vhdlMember=0;
  if (!g_exampleName.isEmpty())
  {
    g_exampleFile = convertNameToFile(g_exampleName+"-example");
  }
  g_includeCodeFragment = inlineFragment;
  if (!memberDef)
  {
    startCodeLine();
  }
  // g_type.resize(0);
  // g_name.resize(0);
  // g_args.resize(0);
  g_parmName.resize(0);
  g_parmType.resize(0);
  if (memberDef) 
  {
    setParameterList(memberDef);
  }
  vhdlcodeYYrestart( vhdlcodeYYin );
  BEGIN( Bases );
  vhdlcodeYYlex();
  g_lexInit=TRUE;
  if (g_needsTermination)
  {
    endCodeLine();
  }
  if (exBlock && g_sourceFileDef)
  {
    // delete the temporary file definition used for this example
    delete g_sourceFileDef;
    g_sourceFileDef=0;
  }
  return;
}

void codeFreeVhdlScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION) 
  if (g_lexInit)
  {
    vhdlcodeYYlex_destroy();
  }
#endif
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
  void vhdlcodeYYdummy() { yy_flex_realloc(0,0); } 
}
#elif YY_FLEX_SUBMINOR_VERSION<33
#error "You seem to be using a version of flex newer than 2.5.4 but older than 2.5.33. These versions do NOT work with doxygen! Please use version <=2.5.4 or >=2.5.33 or expect things to be parsed wrongly!"
#endif




