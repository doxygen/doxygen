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
#include <iostream.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>

#include "qtbc.h"
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"

#define YY_NO_UNPUT
#define YY_NEVER_INTERACTIVE 1
  
class CodeClassDef 
{
  public:
    CodeClassDef() {}
    CodeClassDef(const CodeClassDef &d)
    {
      name = d.name;
      bases = d.bases;
    }
   ~CodeClassDef() {}

    QCString name;
    QStrList bases;
};  
  
class CodeVarDef
{
  public:
    CodeVarDef() {}
    CodeVarDef(const CodeVarDef &d) 
    { 
      name = d.name; 
      type = d.type; 
      classScope = d.classScope;  
    }
   ~CodeVarDef() {} 
    
    QCString name;
    QCString type; 
    QCString classScope;
};

typedef QList<CodeClassDef> CodeClassList; 
typedef QDict<CodeClassDef> CodeClassDict;
typedef QList<CodeVarDef>   CodeVarList; 

/* -----------------------------------------------------------------
 *	statics
 */
  
static CodeClassList g_codeClassList;
static CodeClassDict g_codeClassDict(17);
static CodeVarList   g_codeVarList;
static CodeVarList   g_codeParmList;
static const char *  g_inputString;     //!< the code fragment as text
static int	     g_inputPosition;   //!< read offset during parsing 
static int           g_inputLines;      //!< number of line in the code fragment
static int	     g_yyLineNr;        //!< current line number
static int	     g_lastCContext;
static int	     g_lastSpecialCContext;
static int           g_lastStringContext;
static int	     g_bracketCount = 0;
static int	     g_curlyCount   = 0;
static int	     g_sharpCount   = 0;
static QCString      g_type;
static QCString      g_name;
static QCString      g_args;
static QCString      g_parmType;
static QCString      g_parmName;
static bool          g_inClass;
static QCString      g_classScope;
static OutputList *  g_code;
static CodeClassDef  g_ccd;
static CodeVarDef    g_cvd;
static bool          g_exampleBlock;
static QCString      g_exampleName;
static QCString      g_exampleFile;
static int           g_anchorCount;
static FileDef *     g_sourceFileDef;
static bool          g_includeCodeFragment;

// start a new line of code, inserting a line number if g_sourceFileDef
// is TRUE. If a definition starts at the current line, then the line
// number is linked to the documentation of that definition.
static void startCodeLine(OutputList &ol)
{
  if (g_sourceFileDef)
  {
    QCString lineNumber,lineAnchor;
    lineNumber.sprintf("%05d",g_yyLineNr);
    lineAnchor.sprintf("l%05d",g_yyLineNr);
    Definition *d   = g_sourceFileDef->getSourceDefinition(g_yyLineNr);
    QCString anchor = g_sourceFileDef->getSourceAnchor(g_yyLineNr);
    if (!g_includeCodeFragment && d && d->isLinkableInProject())
    {
      ol.startCodeAnchor(lineAnchor);
      ol.writeCodeLink(d->getReference(),d->getOutputFileBase(),
	               anchor,lineNumber);
      ol.endCodeAnchor();
      ol.codify(" ");
    }
    else
    {
      ol.codify(lineNumber);
      ol.codify(" ");
    }
  }
  ol.startCodeLine(); 
}

// write a code fragment `text' that may span multiple lines, inserting
// line numbers for each line.
static void codifyLines(char *text)
{
  char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n');
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      g_code->codify(sp);
      g_code->endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine(*g_code);
      }
    }
    else
    {
      g_code->codify(sp);
      done=TRUE;
    }
  }
}

// writes a link to a fragment `text' that may span multiple lines, inserting
// line numbers for each line. If `text' contains newlines, the link will be 
// split into multiple links with the same destination, one for each line.
static void writeMultiLineCodeLink(OutputList &ol,
                  const char *ref,const char *file,
                  const char *anchor,const char *text)
{
  bool done=FALSE;
  QCString ts = text;
  char *p=ts.data();
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n');
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      ol.writeCodeLink(ref,file,anchor,sp);
      ol.endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine(ol);
      }
    }
    else
    {
      ol.writeCodeLink(ref,file,anchor,sp);
      done=TRUE;
    }
  }
}

static void addType()
{
  if (g_name=="const") { g_name.resize(0); return; }
  if (g_type.length()) g_type += ' ' ;
  g_type += g_name ;
  g_name.resize(0) ;
  if (g_type.length()) g_type += ' ' ;
  g_type += g_args ;
  g_args.resize(0) ;
}

static void addParmType()
{
  if (g_parmName=="const") { g_parmName.resize(0); return; }
  if (g_parmType.length()) g_parmType += ' ' ;
  g_parmType += g_parmName ;
  g_parmName.resize(0) ;
}

static void setClassScope(const QCString &name)
{
  //printf("setClassScope(%s)\n",name.data());
  QCString n=name;
  n=n.simplifyWhiteSpace();
  int index;
  if ((index=n.find("::"))!=-1)
    g_classScope=n.left(index);
  else
    g_classScope.resize(0);
  //printf("--->New class scope `%s'\n",g_classScope.data());
}

static void addVariable()
{
  g_cvd.name=g_name.copy().simplifyWhiteSpace();
  g_cvd.type=g_type.copy().simplifyWhiteSpace();
  if (g_type.length()==0) 
    return;
  else if ((getClass(g_cvd.type)) || (g_codeClassDict[g_cvd.type])) 
  {
    g_cvd.classScope=g_classScope;
    g_codeVarList.append(new CodeVarDef(g_cvd)); // add it to a list
  }
}

static void addParameter()
{
  g_cvd.name=g_parmName.copy().simplifyWhiteSpace();
  g_cvd.type=g_parmType.copy().simplifyWhiteSpace();
  if (g_cvd.type.length()==0) 
    return;
  else if ((getClass(g_cvd.type)) || (g_codeClassDict[g_cvd.type])) 
  {
    g_cvd.classScope=g_classScope;
    g_codeParmList.append(new CodeVarDef(g_cvd)); // add it to a list
  }
}

static void generateClassLink(OutputList &ol,char *clName)
{
  QCString className=clName;
  if (className.length()==0) return;
  ClassDef *cd;
  if ((cd=getClass(className)) && cd->isLinkable())
  {
    if (g_exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("_a%d",g_anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
      //                                   g_exampleFile.data());
      if (cd->addExample(anchor,g_exampleName,g_exampleFile))
      {
	bool latexOn = ol.isEnabled(OutputGenerator::Latex);
	if (latexOn) ol.disable(OutputGenerator::Latex);
	ol.writeAnchor(anchor);
	if (latexOn) ol.enable(OutputGenerator::Latex);
	g_anchorCount++;
      }
    }
    //ol.writeCodeLink(cd->getReference(),cd->getOutputFileBase(),0,className);
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),0,className);
  }
  else
  {
    codifyLines(clName);
  }
}

static bool getLink(const char *className,
                    const char *memberName,OutputList &result,
		    const char *text=0)
{
  MemberDef    *md;
  ClassDef     *cd;
  FileDef      *fd;
  NamespaceDef *nd;
  QCString m=memberName;
  QCString c=className;
  //printf("Trying `%s'::`%s'\n",c.data(),m.data());
  if (getDefs(c,m,"()",md,cd,fd,nd) && md->isLinkable())
  {
    //printf("Found!\n");
    if (g_exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("a%d",g_anchorCount);
      //printf("addExampleFile(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
      //                                  g_exampleFile.data());
      if (md->addExample(anchor,g_exampleName,g_exampleFile))
      {
	bool latexEnabled = result.isEnabled(OutputGenerator::Latex);
	if (latexEnabled) result.disable(OutputGenerator::Latex);
	result.writeAnchor(anchor);
	if (latexEnabled) result.enable(OutputGenerator::Latex);
	g_anchorCount++;
      }
    }
    Definition *d=0;
    if (cd) d=cd; else if (cd) d=nd; else d=fd;

    if (d)
    {
      //printf("d->getOutputBase()=`%s' name=`%s'\n",d->getOutputFileBase().data(),md->name().data());
      //result.writeCodeLink(d->getReference(),d->getOutputFileBase(),
      //                   md->anchor(),text ? text : memberName);
      writeMultiLineCodeLink(result,d->getReference(),d->getOutputFileBase(),
	                       md->anchor(),text ? text : memberName);
      return TRUE;
    } 
  }
  return FALSE;
}

static ClassDef *stripClassName(const char *s)
{
  QCString tmp=s;
  QRegExp re("[a-z_A-Z][a-z_A-Z0-9]*");
  int p=0,i,l;
  while ((i=re.match(tmp,p,&l))!=-1)
  {
    ClassDef *cd=getClass(tmp.mid(i,l));
    if (cd) return cd;
    p=i+l;
  }
  return 0;
}

static void generateMemberLink(OutputList &ol,const char *varName,
            char *memName)
{
  //printf("generateMemberLink(object=%s,mem=%s) classScope=%s\n",
  //                          varName,memName,classScope.data());
  CodeVarDef *cvd=g_codeParmList.last();
  while (cvd && cvd->name!=varName) cvd=g_codeParmList.prev();
  if (!cvd)
  {
    cvd=g_codeVarList.last();
    while (cvd && cvd->name!=varName) cvd=g_codeVarList.prev();
  }
  if (cvd) // variable found
  {
    //printf("variable found type=%s!\n",cvd->type.data());
    CodeClassDef *ccd=g_codeClassDict[cvd->type];
    if (ccd)
    {
      //printf("Class found!\n");
      OutputList result(&ol);
      if (getLink(ccd->name,memName,result)) 
      {
        //printf("Found result!\n");
        ol+=result;
        return;
      }
      char *s=ccd->bases.first();
      while (s)
      {
	OutputList result(&ol);
	if (getLink(s,memName,result)) 
	{
	  //printf("Found result!\n");
	  ol+=result;
	  return;
	}
	s=ccd->bases.next();
      }
    }
    else 
    {
      OutputList result(&ol);
      if (getLink(cvd->type,memName,result))
      {
        ol+=result;
      }
      else
      {
	codifyLines(memName);
      }
      return;
    }
  }
  else
  {
    ClassDef *vcd = getClass(g_classScope);
    if (vcd)
    {
      //printf("Found class for variable `%s'\n",varName);
      MemberName *vmn=memberNameDict[varName];
      if (vmn)
      {
	MemberNameIterator vmni(*vmn);
	MemberDef *vmd;
	for (;(vmd=vmni.current());++vmni)
	{
	  if (vmd->isVariable() && vmd->memberClass()==vcd)
	  {
	    //printf("Found variable type=%s\n",vmd->typeString());
	    ClassDef *mcd=stripClassName(vmd->typeString());
	    if (mcd)
	    {
	      MemberName *mmn=memberNameDict[memName];
	      if (mmn)
	      {
		MemberNameIterator mmni(*mmn);
		MemberDef *mmd;
		for (;(mmd=mmni.current());++mmni)
		{
		  if (mmd->memberClass()==mcd)
		  {
		    //ol.writeCodeLink(mcd->getReference(),mcd->getOutputFileBase(),
		    //	mmd->anchor(),memName);
		    writeMultiLineCodeLink(ol,mcd->getReference(),
			mcd->getOutputFileBase(),mmd->anchor(),memName);
		    return;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  codifyLines(memName);
  return;
}

static QCString removeWhiteSpace(const char *s)
{
  QCString result;
  if (s)
  {
    const char *p=s;
    int c;
    while ((c=*p++))
    {
      if (c!=' ' && c!='\n' && c!='\r' && c!='\t') result+=c;
    }
  }
  return result;
}

static void generateFunctionLink(OutputList &ol,char *funcName)
{
  OutputList result(&ol);
  CodeClassDef *ccd=0;
  QCString locScope=g_classScope.copy();
  QCString locFunc=removeWhiteSpace(funcName);
  int i=locFunc.findRev("::");
  if (i>0)
  {
    locScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-2);
  }
  //printf("generateFunctionLink(%s) classScope=%s\n",locFunc.data(),locScope.data());
  if (locScope.length()>0 && (ccd=g_codeClassDict[locScope]))
  {
    //printf("using classScope %s\n",g_classScope.data());
    char *s=ccd->bases.first();
    while (s)
    {
      if (getLink(s,locFunc,result,funcName)) 
      {
	ol+=result;
	return;
      }
      s=ccd->bases.next();
    }
  }
  if (getLink(locScope,locFunc,result,funcName))
  {
    ol+=result;
  }
  else
  {
    codifyLines(funcName);
  }
  return;
}

/*! counts the number of lines in the input */
static int countLines()
{
  const char *p=g_inputString;
  char c;
  int count=1;
  while ((c=*p++)) if (c=='\n') count++; 
  return count;
}

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

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

B       [ \t]
BN      [ \t\n\r]
ID	[a-z_A-Z][a-z_A-Z0-9]*
SCOPENAME (({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})

%option noyywrap

%x      SkipString
%x	SkipCPP
%x	SkipComment
%x	SkipCxxComment
%x	RemoveSpecialCComment
%x	StripSpecialCComment
%x	Body
%x      FuncCall
%x      MemberCall
%x      MemberCall2
%x      SkipInits
%x      ClassName
%x      Bases
%x      SkipSharp
%x      ReadInclude

%%

<*>\x0d
<Body>^([ \t]*"#"[ \t]"include"[ \t]*)("<"|"\"") {
					  g_code->codify(yytext);
  					  BEGIN( ReadInclude ); 
					}
<Body>("class"|"struct"|"union")[ \t\n]+ { 
                                          codifyLines(yytext);
					  //g_code->codify(yytext);
  					  BEGIN( ClassName ); 
					}
<ReadInclude>[a-z_A-Z0-9.]+(("/"[a-z_A-Z0-9.]+)*)/(">"|"\"")  {
					  //FileInfo *f;
					  bool ambig;
                                          FileDef *fd;
					  if ((fd=findFileDef(&inputNameDict,yytext,ambig)) &&
					      fd->isLinkable())
					  {
					    g_code->writeCodeLink(fd->getReference(),fd->getOutputFileBase(),0,yytext);
					  }
					  else
					  {
					    g_code->codify(yytext);
					  }
					  char c=yyinput();
					  QCString text;
					  text+=c;
					  g_code->codify(text);
					  BEGIN( Body );
  					}
<Body>^[ \t]*"#"			{ 
  					  g_code->codify(yytext);
  					  BEGIN( SkipCPP ) ; 
					}
<SkipCPP>.				{ 
  					  g_code->codify(yytext);
					}
<SkipCPP>\\\n				{ 
  					  codifyLines(yytext);
					}
<SkipCPP>\n				{ 
  					  codifyLines(yytext);
					  BEGIN( Body ) ;
					}
<SkipCPP>"//"				{ 
  					  g_code->codify(yytext);
					}
<Body>"{"				{ 
  					  g_code->codify(yytext);
  					  g_curlyCount++;
  					  g_type.resize(0); 
					  g_name.resize(0);
					}
<Body>"}"				{ 
  					  g_code->codify(yytext);
  					  g_inClass=FALSE; 
					  if (--g_curlyCount<=0) 
					  {
					    g_classScope.resize(0);
					    g_codeParmList.clear();
					  }
					}
<ClassName>";"				{ 
  					  g_code->codify(yytext);
  					  BEGIN( Body ); 
					}
<ClassName>[*&]+          		{
					  addType();
					  g_code->codify(yytext);
					}
<ClassName>{ID}			        {
					  g_ccd.name=yytext;
					  addType();
					  generateClassLink(*g_code,yytext);
					}
<ClassName>[ \t\n]*":"[ \t\n]*		{
  					  codifyLines(yytext);
  					  BEGIN( Bases ); 
					}
<Bases,ClassName>[ \t]*"{"[ \t]*	{
  					  g_code->codify(yytext);
					  g_curlyCount++;
					  g_inClass=TRUE;
					  if (g_ccd.name.length()>0)
					  {
					    g_classScope=g_ccd.name.copy();
					    CodeClassDef *cd=new CodeClassDef(g_ccd);
					    g_codeClassList.append(cd);
					    g_codeClassDict.insert(cd->name,cd);
					  }
					  BEGIN( Body );
 					}
<Bases>"virtual"			{ 
  					  g_code->codify(yytext);
					}
<Bases>"public"				{ 
  					  g_code->codify(yytext);
					}
<Bases>"protected"			{ 
  					  g_code->codify(yytext);
					}
<Bases>"private"			{ 
  					  g_code->codify(yytext);
					}
<Bases>{ID}	                        { 
					  //printf("%s:addBase(%s)\n",g_ccd.name.data(),yytext);
  					  g_ccd.bases.inSort(yytext); 
					  generateClassLink(*g_code,yytext);
					}
<Bases>"<"                              { 
  					  g_code->codify(yytext);
  					  g_sharpCount=1;
					  BEGIN ( SkipSharp );
					}
<SkipSharp>"<"                          {
  					  g_code->codify(yytext);
  					  ++g_sharpCount; 
					}
<SkipSharp>">"                          { 
  					  g_code->codify(yytext);
  					  if (--g_sharpCount<=0)
					  BEGIN ( Bases );
					}
<Bases>","                              { 
  					  g_code->codify(yytext);
					}
  					

<Body>("asm"|"auto"|"break"|"case"|"catch"|"continue"|"default"|"delete"|"do"|"else"|"for"|"goto"|"if"|"new"|"return"|"switch"|"this"|"throw"|"try"|"typedef"|"while")([ \t\n]*)/"(" {
  					  codifyLines(yytext);
  				          g_name.resize(0);g_type.resize(0);
  					}
<Body>("asm"|"auto"|"break"|"case"|"catch"|"continue"|"default"|"delete"|"do"|"else"|"for"|"goto"|"if"|"new"|"return"|"switch"|"this"|"throw"|"try"|"typedef"|"while")([ \t\n]*) {
  					  codifyLines(yytext);
  				          g_name.resize(0);g_type.resize(0);
  					}
  /*
<Body>([a-z_A-Z~][a-z_A-Z0-9]*)/([ \t]*)	{ 
					  generateClassLink(*g_code,yytext);
					  addType();
  					  name+=yytext; 
					}
  */
<Body>{SCOPENAME}/[ \t\n]* { 
					  generateClassLink(*g_code,yytext);
  					  //codifyLines(yytext);
					  addType();
  					  g_name+=yytext; 
					}
<Body>{SCOPENAME}/{B}*"("		{
  					  addType();
					  //if (type.length()==0)
					  QCString tmp=yytext;
					  generateFunctionLink(*g_code,yytext);
					  //else
  					  //  g_code->codify(yytext);
  					  g_bracketCount=1;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<FuncCall,Body,MemberCall>\"		{
  					  g_code->codify(yytext);
  					  g_lastStringContext=YY_START;
  					  BEGIN( SkipString );
  					}
<SkipString>[^\"\\]*			{ 
  					  g_code->codify(yytext);
					}
<SkipString>"//"|"/*"			{
  					  g_code->codify(yytext);
  					}
<SkipString>\"				{
  					  g_code->codify(yytext);
  					  BEGIN( g_lastStringContext );
  					}
<SkipString>\\.				{
  					  g_code->codify(yytext);
					}
<Body>":"				{
  					  g_code->codify(yytext);
  					  g_name.resize(0);g_type.resize(0);
  					}
<Body,MemberCall,MemberCall2,FuncCall>"'"((\\.)|(.))"'"	{
  					  g_code->codify(yytext);
  					}
<Body>"."|"->"				{ 
  					  g_code->codify(yytext);
  					  BEGIN( MemberCall ); 
					}
<MemberCall>{SCOPENAME}/{B}*"(" 	{
  					  if (g_name.length()>0)
					    generateMemberLink(*g_code,g_name,yytext);
					  else
  					    g_code->codify(yytext);
  					  g_name.resize(0);g_type.resize(0);
					  g_bracketCount=0;
					  BEGIN(FuncCall);
  					}
<MemberCall>[^a-z_A-Z0-9(\n]		{ 
  					  g_code->codify(yytext);
    					  g_type.resize(0);
					  g_name.resize(0);
					  BEGIN(Body); 
					}
<Body>[,=;\[]				{
  					  g_code->codify(yytext);
  					  if (g_type.length()>0) 
					    addVariable();
					  g_name.resize(0);
					  if (*yytext!=',') g_type.resize(0);
					  g_args.resize(0);
  					}
<Body>[0-9]+				{
					  g_code->codify(yytext);
					}
<MemberCall2,FuncCall>[a-z_A-Z][:a-z_A-Z0-9]* {
					  addParmType();
					  g_parmName=yytext; 
					  generateClassLink(*g_code,yytext);
					}
<MemberCall2,FuncCall>,			{
  					  g_code->codify(yytext);
					  addParameter();
					  g_parmType.resize(0);g_parmName.resize(0);
					}
<MemberCall2,FuncCall>"("		{
  					  g_code->codify(yytext);
  					  g_bracketCount++; 
					}
<MemberCall2,FuncCall>")"		{ 
  					  g_code->codify(yytext);
  					  if (--g_bracketCount<=0) 
					  g_name.resize(0);g_args.resize(0);
					  g_parmType.resize(0);g_parmName.resize(0);
  					  BEGIN( Body ); 
					}
<MemberCall2,FuncCall>")"[ \t\n]*";"	{
  					  codifyLines(yytext);
  					  g_bracketCount=0;
					  if (!g_inClass && g_type.length()>0)
					    addVariable();
					  g_name.resize(0);g_type.resize(0);
					  g_parmType.resize(0);g_parmName.resize(0);
  					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>")"[ \t\n]*"{" 	{
					  addParameter();
					  g_parmType.resize(0);g_parmName.resize(0);
					  if (g_name.find("::")!=-1) setClassScope(g_name);
  					  codifyLines(yytext);
					  g_curlyCount++;
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>")"[ \t\n]*":" 	{
					  addParameter();
					  g_parmType.resize(0);g_parmName.resize(0);
					  if (g_name.find("::")!=-1) setClassScope(g_name);
  					  codifyLines(yytext);
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( SkipInits );
  					}
<SkipInits>"{"				{ 
  					  g_code->codify(yytext);
					  g_curlyCount++; 
					  BEGIN( Body ); 
					}
<SkipInits>{ID}				{
					  generateClassLink(*g_code,yytext);
  					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/"("	{
					  generateFunctionLink(*g_code,yytext);
					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/("."|"->") { 
  					  g_code->codify(yytext);
					  g_args=yytext; 
					  BEGIN( MemberCall2 ); 
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*"(") { 
  					  if (g_args.length()>0)
					    generateMemberLink(*g_code,g_args,yytext);
					  else
  					    g_code->codify(yytext);
					  g_args.resize(0);
					  BEGIN( FuncCall );
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*("."|"->")) {
  					  g_code->codify(yytext);
    					  g_args=yytext;
    					}
<SkipComment>"//"			{
  					  g_code->codify(yytext);
  					}
<SkipComment>[^*/\n]+			{
  					  g_code->codify(yytext);
  					}
<SkipComment>[ \t]*"*/"			{ 
  					  g_code->codify(yytext);
  					  BEGIN( g_lastCContext ) ; 
					}
<SkipCxxComment>.*/\n			{ 
  					  //codifyLines(yytext);
  					  g_code->codify(yytext);
					  BEGIN( g_lastCContext ) ;
					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)*{B}*"/*"[*!]/[^/*] {
  					  g_yyLineNr+=QCString(yytext).contains('\n');
					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)* {
  					  g_yyLineNr+=QCString(yytext).contains('\n');
                                          g_code->endCodeLine();
                                          if (g_yyLineNr<g_inputLines) 
                                          {
                                            startCodeLine(*g_code);
                                          }
  					  BEGIN(g_lastSpecialCContext);
  					}
<RemoveSpecialCComment>"*/"		{
  					  BEGIN(g_lastSpecialCContext);
  					}
<RemoveSpecialCComment>[^*\n]+
<RemoveSpecialCComment>"//"|"/*"
<RemoveSpecialCComment>\n  { g_yyLineNr++; }
<RemoveSpecialCComment>.
  /*
<SkipSpecialComment>"//"
<SkipSpecialComment>[ \t]*"* /"		{
  					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialComment>[ \t]*"* /"[ \t\n]*"\n"/"/ *"  {
  					  //g_code->codify("\n");
					  //QCString lineText=yytext;
  					  //g_yyLineNr+=lineText.contains('\n');
  					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialComment>.	
<SkipSpecialComment>\n			{ 
  					  codifyLines(yytext); 
					}
<SkipSpecialCxxComment>.* /\n		{
  					  codifyLines(yytext); 
					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialCxxComment>.	
<SkipSpecialCxxComment>\n		{ 
  					  codifyLines(yytext); 
					}
 */
<*>\n({B}*"//"[!/][^\n]*\n)*		{ // remove special one-line comment
  					  g_yyLineNr+=((QCString)yytext).contains('\n');
                                          g_code->endCodeLine();
                                          if (g_yyLineNr<g_inputLines) 
                                          {
                                            startCodeLine(*g_code);
                                          }
  					}
<*>^{B}*"//"[!/][^\n]*\n		{ // remove special one-line comment
  					  g_yyLineNr++;
                                          g_code->endCodeLine();
                                          if (g_yyLineNr<g_inputLines) 
                                          {
                                            startCodeLine(*g_code);
                                          }
  					}
<*>"//"[!/][^\n]*\n			{ // strip special one-line comment
  					  char c[2]; c[0]='\n'; c[1]=0;
  					  codifyLines(c);
  					}
<*>\n{B}*"/*"[!*]/[^/*] 		{
					  g_lastSpecialCContext = YY_START;
					  g_yyLineNr++;
					  BEGIN(RemoveSpecialCComment);
					}
<*>^{B}*"/*"[!*]/[^/*]			{ // special C comment block at a new line
					  g_lastSpecialCContext = YY_START;
					  BEGIN(RemoveSpecialCComment);
					}
<*>"/*"[!*]/[^/*]			{ // special C comment block half way a line
					  g_lastSpecialCContext = YY_START;
					  BEGIN(RemoveSpecialCComment);
					}
<*>"/*"("!"?)"*/"			{}
<*>"/*"					{ 
  					  g_code->codify(yytext);
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipComment ) ;
					}
<*>"//"					{ 
  					  g_code->codify(yytext);
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>\n					{
  					  codifyLines(yytext); 
  					}
<*>.					{
  					  g_code->codify(yytext);
					}
  /*
<*>([ \t\n]*"\n"){2,}			{ // combine multiple blank lines
  					  //QCString sepLine=yytext;
  					  //g_code->codify("\n\n");
  					  //g_yyLineNr+=sepLine.contains('\n'); 
  					  //char sepLine[3]="\n\n";
  					  codifyLines(yytext);
					}
  */

%%

/*@ ----------------------------------------------------------------------------
 */

void initParseCodeContext()
{
  g_codeClassList.setAutoDelete(TRUE);
  g_codeVarList.setAutoDelete(TRUE);
  g_codeParmList.setAutoDelete(TRUE);
  g_codeClassDict.clear();
  g_codeClassList.clear();
  g_codeVarList.clear();
  g_codeParmList.clear();
  g_ccd.bases.clear();
  g_anchorCount = 0;
}

void parseCode(OutputList &ol,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
		  int startLine,int endLine,bool inlineFragment)
{
  g_code = new OutputList(&ol);
  if (s.isEmpty()) return;
  g_inputString   = s;
  g_inputPosition = 0;
  if (endLine!=-1)
    g_inputLines  = endLine+1;
  else
    g_inputLines  = countLines();
  if (startLine!=-1)
    g_yyLineNr    = startLine;
  else
    g_yyLineNr    = 1;
  g_curlyCount    = 0;
  g_bracketCount  = 0;
  g_sharpCount    = 0;
  g_classScope    = className;
  g_exampleBlock  = exBlock; 
  g_exampleName   = exName;
  g_sourceFileDef = fd;
  g_exampleFile   = convertSlashes(g_exampleName,TRUE)+"-example";
  g_includeCodeFragment = inlineFragment;
  startCodeLine(*g_code);
  g_type.resize(0);
  g_name.resize(0);
  g_args.resize(0);
  g_parmName.resize(0);
  g_parmType.resize(0);
  codeYYrestart( codeYYin );
  BEGIN( Body );
  codeYYlex();
  //if (g_yyLineNr<=g_inputLines) code->endCodeLine();
  ol+=*g_code;
  delete g_code;
  return;
}

extern "C" { // some bogus code to keep the compiler happy
//  int  codeYYwrap() { return 1 ; }
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
