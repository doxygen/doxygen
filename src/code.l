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

static CodeClassList codeClassList;
static CodeClassDict codeClassDict(17);
static CodeVarList codeVarList;
static CodeVarList codeParmList;

/* -----------------------------------------------------------------
 *
 *	statics
 */
  
static const char * inputString;     //!< the code fragment as text
static int	    inputPosition;   //!< read offset during parsing 
static int          inputLines;      //!< number of line in the code fragment
static int	    yyLineNr;        //!< current line number
static int	    lastCContext;
static int	    lastSpecialCContext;
static int          lastStringContext;
static int	    bracketCount = 0;
static int	    curlyCount   = 0;
static int	    sharpCount   = 0;
static QCString     type;
static QCString     name;
static QCString     args;
static QCString     parmType;
static QCString     parmName;
static bool         inClass;
static QCString     classScope;
static OutputList   *code;
static CodeClassDef ccd;
static CodeVarDef   cvd;
static bool         exampleBlock;
static QCString     exampleName;
static QCString     exampleFile;
static int          anchorCount;
static FileDef     *sourceFileDef;
static QCString     lastVariable;

// start a new line of code, inserting a line number if sourceFileDef
// is TRUE. If a definition starts at the current line, then the line
// number is linked to the documentation of that definition.
static void startCodeLine(OutputList &ol)
{
  if (sourceFileDef)
  {
    QCString lineNumber,lineAnchor;
    lineNumber.sprintf("%05d ",yyLineNr);
    lineAnchor.sprintf("l%05d",yyLineNr);
    Definition *d   = sourceFileDef->getSourceDefinition(yyLineNr);
    QCString anchor = sourceFileDef->getSourceAnchor(yyLineNr);
    if (d && d->isLinkableInProject())
    {
      ol.startCodeAnchor(lineAnchor);
      ol.writeCodeLink(d->getReference(),d->getOutputFileBase(),
	                   anchor,lineNumber);
      ol.endCodeAnchor();
    }
    else
    {
      ol.codify(lineNumber);
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
      yyLineNr++;
      *(p-1)='\0';
      code->codify(sp);
      code->endCodeLine();
      if (yyLineNr<inputLines) 
      {
	startCodeLine(*code);
      }
    }
    else
    {
      code->codify(sp);
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
      yyLineNr++;
      *(p-1)='\0';
      ol.writeCodeLink(ref,file,anchor,sp);
      ol.endCodeLine();
      if (yyLineNr<inputLines) 
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
  if (name=="const") { name.resize(0); return; }
  if (type.length()) type += ' ' ;
  type += name ;
  name.resize(0) ;
  if (type.length()) type += ' ' ;
  type += args ;
  args.resize(0) ;
}

static void addParmType()
{
  if (parmName=="const") { parmName.resize(0); return; }
  if (parmType.length()) parmType += ' ' ;
  parmType += parmName ;
  parmName.resize(0) ;
}

static void setClassScope(const QCString &name)
{
  QCString n=name;
  n=n.simplifyWhiteSpace();
  int index;
  if ((index=n.find("::"))!=-1)
    classScope=n.left(index);
  else
    classScope.resize(0);
  //printf("--->New class scope `%s'\n",classScope.data());
}

static void addVariable()
{
  cvd.name=name.copy().simplifyWhiteSpace();
  cvd.type=type.copy().simplifyWhiteSpace();
  if (type.length()==0) 
    return;
  else if ((getClass(cvd.type)) || (codeClassDict[cvd.type])) 
  {
    cvd.classScope=classScope;
    codeVarList.append(new CodeVarDef(cvd)); // add it to a list
  }
}

static void addParameter()
{
  cvd.name=parmName.copy().simplifyWhiteSpace();
  cvd.type=parmType.copy().simplifyWhiteSpace();
  if (cvd.type.length()==0) 
    return;
  else if ((getClass(cvd.type)) || (codeClassDict[cvd.type])) 
  {
    cvd.classScope=classScope;
    codeParmList.append(new CodeVarDef(cvd)); // add it to a list
  }
}

static void generateClassLink(OutputList &ol,char *clName)
{
  QCString className=clName;
  if (className.length()==0) return;
  ClassDef *cd;
  if ((cd=getClass(className)) && cd->isLinkable())
  {
    if (exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("_a%d",anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),exampleName.data(),
      //                                   exampleFile.data());
      if (cd->addExample(anchor,exampleName,exampleFile))
      {
	bool latexOn = ol.isEnabled(OutputGenerator::Latex);
	if (latexOn) ol.disable(OutputGenerator::Latex);
	ol.writeAnchor(anchor);
	if (latexOn) ol.enable(OutputGenerator::Latex);
	anchorCount++;
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
    if (exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("a%d",anchorCount);
      //printf("addExampleFile(%s,%s,%s)\n",anchor.data(),exampleName.data(),
      //                                  exampleFile.data());
      if (md->addExample(anchor,exampleName,exampleFile))
      {
	bool latexEnabled = result.isEnabled(OutputGenerator::Latex);
	if (latexEnabled) result.disable(OutputGenerator::Latex);
	result.writeAnchor(anchor);
	if (latexEnabled) result.enable(OutputGenerator::Latex);
	anchorCount++;
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
  CodeVarDef *cvd=codeParmList.last();
  while (cvd && cvd->name!=varName) cvd=codeParmList.prev();
  if (!cvd)
  {
    cvd=codeVarList.last();
    while (cvd && cvd->name!=varName) cvd=codeVarList.prev();
  }
  if (cvd) // variable found
  {
    //printf("variable found type=%s!\n",cvd->type.data());
    CodeClassDef *ccd=codeClassDict[cvd->type];
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
    ClassDef *vcd = getClass(classScope);
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
  QCString locScope=classScope.copy();
  QCString locFunc=removeWhiteSpace(funcName);
  int i=locFunc.findRev("::");
  if (i>0)
  {
    locScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-2);
  }
  //printf("generateFunctionLink(%s) classScope=%s\n",locFunc.data(),locScope.data());
  if (locScope.length()>0 && (ccd=codeClassDict[locScope]))
  {
    //printf("using classScope %s\n",classScope.data());
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
  const char *p=inputString;
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
    while( c < max_size && inputString[inputPosition] )
    {
	*buf = inputString[inputPosition++] ;
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
					  code->codify(yytext);
  					  BEGIN( ReadInclude ); 
					}
<Body>("class"|"struct"|"union")[ \t\n]+ { 
                                          codifyLines(yytext);
					  //code->codify(yytext);
  					  BEGIN( ClassName ); 
					}
<ReadInclude>[a-z_A-Z0-9.]+(("/"[a-z_A-Z0-9.]+)*)/(">"|"\"")  {
					  //FileInfo *f;
					  bool ambig;
                                          FileDef *fd;
					  if ((fd=findFileDef(&inputNameDict,yytext,ambig)) &&
					      fd->isLinkable())
					  {
					    code->writeCodeLink(fd->getReference(),fd->getOutputFileBase(),0,yytext);
					  }
					  else
					  {
					    code->codify(yytext);
					  }
					  char c=yyinput();
					  QCString text;
					  text+=c;
					  code->codify(text);
					  BEGIN( Body );
  					}
<Body>^[ \t]*"#"			{ 
  					  code->codify(yytext);
  					  BEGIN( SkipCPP ) ; 
					}
<SkipCPP>.				{ 
  					  code->codify(yytext);
					}
<SkipCPP>\\\n				{ 
  					  codifyLines(yytext);
					}
<SkipCPP>\n				{ 
  					  codifyLines(yytext);
					  BEGIN( Body ) ;
					}
<SkipCPP>"//"				{ 
  					  code->codify(yytext);
					}
<Body>"{"				{ 
  					  code->codify(yytext);
  					  curlyCount++;
  					  type.resize(0); name.resize(0);
					}
<Body>"}"				{ 
  					  code->codify(yytext);
  					  inClass=FALSE; 
					  if (--curlyCount<=0) 
					  {
					    classScope.resize(0);
					    codeParmList.clear();
					  }
					}
<ClassName>";"				{ 
  					  code->codify(yytext);
  					  BEGIN( Body ); 
					}
<ClassName>[*&]+          		{
					  addType();
					  code->codify(yytext);
					}
<ClassName>{ID}			        {
					  ccd.name=yytext;
					  addType();
					  generateClassLink(*code,yytext);
					}
<ClassName>[ \t\n]*":"[ \t\n]*		{
  					  codifyLines(yytext);
  					  BEGIN( Bases ); 
					}
<Bases,ClassName>[ \t]*"{"[ \t]*	{
  					  code->codify(yytext);
					  curlyCount++;
					  inClass=TRUE;
					  if (ccd.name.length()>0)
					  {
					    classScope=ccd.name.copy();
					    CodeClassDef *cd=new CodeClassDef(ccd);
					    codeClassList.append(cd);
					    codeClassDict.insert(cd->name,cd);
					  }
					  BEGIN( Body );
 					}
<Bases>"virtual"			{ 
  					  code->codify(yytext);
					}
<Bases>"public"				{ 
  					  code->codify(yytext);
					}
<Bases>"protected"			{ 
  					  code->codify(yytext);
					}
<Bases>"private"			{ 
  					  code->codify(yytext);
					}
<Bases>{ID}	                        { 
					  //printf("%s:addBase(%s)\n",ccd.name.data(),yytext);
  					  ccd.bases.inSort(yytext); 
					  generateClassLink(*code,yytext);
					}
<Bases>"<"                              { 
  					  code->codify(yytext);
  					  sharpCount=1;
					  BEGIN ( SkipSharp );
					}
<SkipSharp>"<"                          {
  					  code->codify(yytext);
  					  ++sharpCount; 
					}
<SkipSharp>">"                          { 
  					  code->codify(yytext);
  					  if (--sharpCount<=0)
					  BEGIN ( Bases );
					}
<Bases>","                              { 
  					  code->codify(yytext);
					}
  					

<Body>("asm"|"auto"|"break"|"case"|"catch"|"continue"|"default"|"delete"|"do"|"else"|"for"|"goto"|"if"|"new"|"return"|"switch"|"this"|"throw"|"try"|"typedef"|"while")([ \t\n]*)/"(" {
  					  codifyLines(yytext);
  				          name.resize(0);type.resize(0);
  					}
<Body>("asm"|"auto"|"break"|"case"|"catch"|"continue"|"default"|"delete"|"do"|"else"|"for"|"goto"|"if"|"new"|"return"|"switch"|"this"|"throw"|"try"|"typedef"|"while")([ \t\n]*) {
  					  codifyLines(yytext);
  				          name.resize(0);type.resize(0);
  					}
  /*
<Body>([a-z_A-Z~][a-z_A-Z0-9]*)/([ \t]*)	{ 
					  generateClassLink(*code,yytext);
					  addType();
  					  name+=yytext; 
					}
  */
<Body>{SCOPENAME}/[ \t\n]* { 
					  generateClassLink(*code,yytext);
  					  //codifyLines(yytext);
					  addType();
  					  name+=yytext; 
					}
<Body>{SCOPENAME}/{B}*"("		{
  					  addType();
					  //if (type.length()==0)
					  QCString tmp=yytext;
					  generateFunctionLink(*code,yytext);
					  //else
  					  //  code->codify(yytext);
  					  bracketCount=1;
					  args.resize(0);
  					  name+=yytext; 
  					  BEGIN( FuncCall );
					}
<FuncCall,Body,MemberCall>\"		{
  					  code->codify(yytext);
  					  lastStringContext=YY_START;
  					  BEGIN( SkipString );
  					}
<SkipString>[^\"\\]*			{ 
  					  code->codify(yytext);
					}
<SkipString>"//"|"/*"			{
  					  code->codify(yytext);
  					}
<SkipString>\"				{
  					  code->codify(yytext);
  					  BEGIN( lastStringContext );
  					}
<SkipString>\\.				{
  					  code->codify(yytext);
					}
<Body>":"				{
  					  code->codify(yytext);
  					  name.resize(0);type.resize(0);
  					}
<Body>"'"."'"				{
  					  code->codify(yytext);
  					}
<Body>"."|"->"				{ 
  					  code->codify(yytext);
  					  BEGIN( MemberCall ); 
					}
<MemberCall>{SCOPENAME}/{B}*"(" 	{
  					  if (name.length()>0)
					    generateMemberLink(*code,name,yytext);
					  else
  					    code->codify(yytext);
  					  name.resize(0);type.resize(0);
					  bracketCount=0;
					  BEGIN(FuncCall);
  					}
<MemberCall>[^a-z_A-Z0-9(\n]		{ 
  					  code->codify(yytext);
    					  type.resize(0);
					  name.resize(0);
					  BEGIN(Body); 
					}
<Body>[,=;\[]				{
  					  code->codify(yytext);
  					  if (type.length()>0) 
					    addVariable();
					  name.resize(0);
					  if (*yytext!=',') type.resize(0);
					  args.resize(0);
  					}
<Body>[0-9]+				{
					  code->codify(yytext);
					}
<MemberCall2,FuncCall>[a-z_A-Z][:a-z_A-Z0-9]* {
					  addParmType();
					  parmName=yytext; 
					  generateClassLink(*code,yytext);
					}
<MemberCall2,FuncCall>,			{
  					  code->codify(yytext);
					  addParameter();
					  parmType.resize(0);parmName.resize(0);
					}
<MemberCall2,FuncCall>"("		{
  					  code->codify(yytext);
  					  bracketCount++; 
					}
<MemberCall2,FuncCall>")"		{ 
  					  code->codify(yytext);
  					  if (--bracketCount<=0) 
					  name.resize(0);args.resize(0);
					  parmType.resize(0);parmName.resize(0);
  					  BEGIN( Body ); 
					}
<MemberCall2,FuncCall>")"[ \t\n]*";"	{
  					  codifyLines(yytext);
  					  bracketCount=0;
					  if (!inClass && type.length()>0)
					    addVariable();
					  name.resize(0);type.resize(0);
					  parmType.resize(0);parmName.resize(0);
  					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>")"[ \t\n]*"{" 	{
					  addParameter();
					  parmType.resize(0);parmName.resize(0);
					  if (name.length()>0) setClassScope(name);
  					  codifyLines(yytext);
					  curlyCount++;
  					  type.resize(0); name.resize(0);
					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>")"[ \t\n]*":" 	{
					  addParameter();
					  parmType.resize(0);parmName.resize(0);
					  if (name.length()>0) setClassScope(name);
  					  codifyLines(yytext);
  					  type.resize(0); name.resize(0);
					  BEGIN( SkipInits );
  					}
<SkipInits>"{"				{ 
  					  code->codify(yytext);
					  curlyCount++; 
					  BEGIN( Body ); 
					}
<SkipInits>{ID}				{
					  generateClassLink(*code,yytext);
  					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/"("	{
					  generateFunctionLink(*code,yytext);
					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/("."|"->") { 
  					  code->codify(yytext);
					  args=yytext; 
					  BEGIN( MemberCall2 ); 
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*"(") { 
  					  if (args.length()>0)
					    generateMemberLink(*code,args,yytext);
					  else
  					    code->codify(yytext);
					  args.resize(0);
					  BEGIN( FuncCall );
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*("."|"->")) {
  					  code->codify(yytext);
    					  args=yytext;
    					}
<SkipComment>"//"			{
  					  code->codify(yytext);
  					}
<SkipComment>[^*/\n]+			{
  					  code->codify(yytext);
  					}
<SkipComment>[ \t]*"*/"			{ 
  					  code->codify(yytext);
  					  BEGIN( lastCContext ) ; 
					}
<SkipCxxComment>.*\n			{ 
  					  codifyLines(yytext);
					  BEGIN( lastCContext ) ;
					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)*{B}*"/*"[*!]/[^/*] {
  					  yyLineNr+=QCString(yytext).contains('\n');
					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)* {
  					  yyLineNr+=QCString(yytext).contains('\n');
                                          code->endCodeLine();
                                          if (yyLineNr<inputLines) 
                                          {
                                            startCodeLine(*code);
                                          }
  					  BEGIN(lastSpecialCContext);
  					}
<RemoveSpecialCComment>"*/"		{
  					  BEGIN(lastSpecialCContext);
  					}
<RemoveSpecialCComment>[^*\n]+
<RemoveSpecialCComment>"//"|"/*"
<RemoveSpecialCComment>\n  { yyLineNr++; }
<RemoveSpecialCComment>.
  /*
<SkipSpecialComment>"//"
<SkipSpecialComment>[ \t]*"* /"		{
  					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialComment>[ \t]*"* /"[ \t\n]*"\n"/"/ *"  {
  					  //code->codify("\n");
					  //QCString lineText=yytext;
  					  //yyLineNr+=lineText.contains('\n');
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
  					  yyLineNr+=((QCString)yytext).contains('\n');
                                          code->endCodeLine();
                                          if (yyLineNr<inputLines) 
                                          {
                                            startCodeLine(*code);
                                          }
  					}
<*>^{B}*"//"[!/][^\n]*\n		{ // remove special one-line comment
  					  yyLineNr++;
                                          code->endCodeLine();
                                          if (yyLineNr<inputLines) 
                                          {
                                            startCodeLine(*code);
                                          }
  					}
<*>"//"[!/][^\n]*\n			{ // strip special one-line comment
  					  char c[2]; c[0]='\n'; c[1]=0;
  					  codifyLines(c);
  					}
<*>\n{B}*"/*"[!*]/[^/*]			{
					  lastSpecialCContext = YY_START;
					  yyLineNr++;
					  BEGIN(RemoveSpecialCComment);
					}
<*>^{B}*"/*"[!*]/[^/*]			{ // special C comment block at a new line
					  lastSpecialCContext = YY_START;
					  BEGIN(RemoveSpecialCComment);
					}
<*>"/*"[!*]/[^/*]			{ // special C comment block half way a line
					  lastSpecialCContext = YY_START;
					  BEGIN(RemoveSpecialCComment);
					}
<*>"/*"					{ 
  					  code->codify(yytext);
  					  lastCContext = YY_START ;
					  BEGIN( SkipComment ) ;
					}
<*>"//"					{ 
  					  code->codify(yytext);
  					  lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>\n					{
  					  codifyLines(yytext); 
  					}
<*>.					{
  					  code->codify(yytext);
					}
  /*
<*>([ \t\n]*"\n"){2,}			{ // combine multiple blank lines
  					  //QCString sepLine=yytext;
  					  //code->codify("\n\n");
  					  //yyLineNr+=sepLine.contains('\n'); 
  					  //char sepLine[3]="\n\n";
  					  codifyLines(yytext);
					}
  */

%%

/*@ ----------------------------------------------------------------------------
 */

void initParseCodeContext()
{
  codeClassList.setAutoDelete(TRUE);
  codeVarList.setAutoDelete(TRUE);
  codeParmList.setAutoDelete(TRUE);
  codeClassDict.clear();
  codeClassList.clear();
  codeVarList.clear();
  codeParmList.clear();
  ccd.bases.clear();
  anchorCount = 0;
}

void parseCode(OutputList &ol,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd)
{
  code = new OutputList(&ol);
  if (s.isEmpty()) return;
  inputString   = s;
  inputPosition = 0;
  inputLines    = countLines();
  yyLineNr      = 1;
  curlyCount    = 0;
  bracketCount  = 0;
  sharpCount    = 0;
  classScope    = className;
  exampleBlock  = exBlock; 
  exampleName   = exName;
  sourceFileDef = fd;
  exampleFile   = convertSlashes(exampleName,TRUE)+"-example";
  startCodeLine(*code);
  type.resize(0);
  name.resize(0);
  args.resize(0);
  parmName.resize(0);
  parmType.resize(0);
  codeYYrestart( codeYYin );
  BEGIN( Body );
  codeYYlex();
  //if (yyLineNr<=inputLines) code->endCodeLine();
  ol+=*code;
  delete code;
  return;
}

extern "C" { // some bogus code to keep the compiler happy
//  int  codeYYwrap() { return 1 ; }
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
