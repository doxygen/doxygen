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

#include <qstring.h>  
  
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

    QString name;
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
    
    QString name;
    QString type; 
    QString classScope;
};

typedef QList<CodeClassDef> CodeClassList; 
typedef QDict<CodeClassDef> CodeClassDict;
typedef QList<CodeVarDef> CodeVarList; 

static CodeClassList codeClassList;
static CodeClassDict codeClassDict(17);
static CodeVarList codeVarList;
static CodeVarList codeParmList;

/* -----------------------------------------------------------------
 *
 *	statics
 */
  
static const char * inputString;
static int	    inputPosition;
static int	    lastCContext;
static int	    lastDContext;
static int          lastStringContext;
static int	    bracketCount = 0;
static int	    curlyCount   = 0;
static int	    sharpCount   = 0;
static int	    yyLineNr     = 0;
static QString      type;
static QString      name;
static QString      args;
static QString      parmType;
static QString      parmName;
static bool         inClass;
static QString      classScope;
static OutputList   *code;
static CodeClassDef ccd;
static CodeVarDef   cvd;
static bool         exampleBlock;
static QString      exampleName;
static QString      exampleFile;
static int          anchorCount;

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

static void setClassScope(const QString &name)
{
  QString n=name;
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

static void generateClassLink(OutputList &ol,const char *clName)
{
  QString className=clName;
  if (className.length()==0) return;
  ClassDef *cd;
  if ((cd=getClass(className)) && cd->isVisible())
  {
    if (exampleBlock)
    {
      QString anchor;
      anchor.sprintf("a%d",anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),exampleName.data(),
      //                                   exampleFile.data());
      if (cd->addExample(anchor,exampleName,exampleFile))
      {
	ol.writeAnchor(anchor);
	anchorCount++;
      }
    }
    ol.writeCodeLink(cd->getReference(),cd->classFile(),0,className);
  }
  else
  {
    ol.codify(clName);
  }
}

static bool getLink(const char *className,
                    const char *memberName,OutputList &result)
{
  MemberDef *md;
  ClassDef  *cd;
  FileDef   *fd;
  QString m=memberName;
  QString c=className;
  //printf("Trying `%s'::`%s'\n",c.data(),m.data());
  if (getDefs(m,c,"()",md,cd,fd) &&
      (md->hasDocumentation() || md->isReference()))
  {
    //printf("Found!\n");
    if (exampleBlock)
    {
      QString anchor;
      anchor.sprintf("a%d",anchorCount);
      //printf("addExampleFile(%s,%s,%s)\n",anchor.data(),exampleName.data(),
      //                                  exampleFile.data());
      if (md->addExample(anchor,exampleName,exampleFile))
      {
	result.writeAnchor(anchor);
	anchorCount++;
      }
    }
    if (cd)
    {
      result.writeCodeLink(cd->getReference(),cd->classFile(),
	                        md->anchor(),memberName);
      return TRUE;
    }
    else if (fd)
    {
      result.writeCodeLink(fd->getReference(),fd->diskName(),
	                        md->anchor(),memberName);
      return TRUE;
    }
  }
  
  return FALSE;
}

static void generateMemberLink(OutputList &ol,const char *varName,
            const char *memName)
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
    CodeClassDef *ccd=codeClassDict[cvd->type];
    if (ccd)
    {
      char *s=ccd->bases.first();
      while (s)
      {
	OutputList result(&ol);
	if (getLink(s,memName,result)) 
	{
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
	ol.codify(memName);
      }
      return;
    }
  }
  ol.codify(memName);
  return;
}

static void generateFunctionLink(OutputList &ol,const char *funcName)
{
  //printf("generateFunctionLink(%s) classScope=%s\n",funcName,classScope.data());
  OutputList result(&ol);
  CodeClassDef *ccd=0;
  if (classScope.length()>0 && (ccd=codeClassDict[classScope]))
  {
    //printf("using classScope %s\n",classScope.data());
    char *s=ccd->bases.first();
    while (s)
    {
      if (getLink(s,funcName,result)) 
      {
	ol+=result;
	return;
      }
      s=ccd->bases.next();
    }
  }
  if (getLink(classScope,funcName,result))
  {
    ol+=result;
  }
  else
  {
    ol.codify(funcName);
  }
  return;
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

ID	[a-z_A-Z][a-z_A-Z0-9]*

%x      SkipString
%x	SkipCPP
%x	SkipComment
%x	SkipSpecialComment
%x	SkipCxxComment
%x	SkipSpecialCxxComment
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
<Body>^([ \t]*"#include"[ \t]*)("<"|"\"") {
					  code->codify(yytext);
  					  BEGIN( ReadInclude ); 
					}
<Body>("class"|"struct"|"union")[ \t\n]+ { 
					  code->codify(yytext);
  					  BEGIN( ClassName ); 
					}
<ReadInclude>[a-z_A-Z0-9.]+(("/"[a-z_A-Z0-9.]+)*)/(">"|"\"")  {
					  //FileInfo *f;
					  bool ambig;
                                          FileDef *fd;
					  if ((fd=findFileDef(&inputNameDict,yytext,ambig)) &&
					      fd->hasDocumentation())
					  {
					    code->writeCodeLink(0,fd->diskName(),0,yytext);
					  }
					  else
					  {
					    code->codify(yytext);
					  }
					  char c=yyinput();
					  QString text;
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
  					  code->codify(yytext);
  					  yyLineNr++ ; 
					}
<SkipCPP>\n				{ 
					  yyLineNr++ ;
  					  code->codify(yytext);
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
  					  code->codify(yytext);
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
  					  code->codify(yytext);
  				          name.resize(0);type.resize(0);
  					}
<Body>("asm"|"auto"|"break"|"case"|"catch"|"continue"|"default"|"delete"|"do"|"else"|"for"|"goto"|"if"|"new"|"return"|"switch"|"this"|"throw"|"try"|"typedef"|"while")([ \t\n]*) {
  					  code->codify(yytext);
  				          name.resize(0);type.resize(0);
  					}
<Body>([a-z_A-Z~][a-z_A-Z0-9]*)/([ \t]*)	{ 
					  generateClassLink(*code,yytext);
					  addType();
  					  name+=yytext; 
					}
<Body>[a-z_A-Z~]([a-z_A-Z0-9]*[ \t\n]*"::")+[ \t\n]*[~a-z_A-Z][a-z_A-Z0-9]*[ \t\n]* { 
  					  code->codify(yytext);
					  addType();
  					  name+=yytext; 
					}
<Body>([a-z_A-Z~][a-z_A-Z0-9:]*)/([ \t]*"(")	{
  					  addType();
					  if (type.length()==0)
					    generateFunctionLink(*code,yytext);
					  else
  					    code->codify(yytext);
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
<SkipString>\"				{
  					  code->codify(yytext);
  					  BEGIN( lastStringContext );
  					}
<SkipString>\\\"			{
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
<MemberCall>([a-z_A-Z~][a-z_A-Z0-9]*)/([ \t\n]*"(") {
  					  if (name.length()>0)
					    generateMemberLink(*code,name,yytext);
					  else
  					    code->codify(yytext);
  					  name.resize(0);type.resize(0);
					  bracketCount=0;
					  BEGIN(FuncCall);
  					}
<MemberCall>[^a-z_A-Z0-9(]		{ 
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
  					  code->codify(yytext);
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
  					  code->codify(yytext);
					  curlyCount++;
  					  type.resize(0); name.resize(0);
					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>")"[ \t\n]*":" 	{
					  addParameter();
					  parmType.resize(0);parmName.resize(0);
					  if (name.length()>0) setClassScope(name);
  					  code->codify(yytext);
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
<SkipComment>[ \t]*"*/"			{ 
  					  code->codify(yytext);
  					  BEGIN( lastCContext ) ; 
					}
<SkipCxxComment>.*\n			{ 
  					  code->codify(yytext);
  					  yyLineNr++ ;
					  BEGIN( lastCContext ) ;
					}
<SkipSpecialComment>"//"
  /*!
<SkipSpecialComment>[ \t]*"* /"[ \t]* /"\n" {
  					  BEGIN( lastDContext ) ;
  					}
  */
<SkipSpecialComment>[ \t]*"*/"		{
  					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialComment>[ \t]*"*/"[ \t\n]*"\n"/"/*"  {
  					  //code->codify("\n");
					  QString lineText=yytext;
  					  yyLineNr+=lineText.contains('\n');
  					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialComment>.	
<SkipSpecialComment>\n			{ yyLineNr++; }
<SkipSpecialCxxComment>.*/\n		{
  					  yyLineNr++ ;
					  BEGIN( lastDContext ) ;
  					}
<SkipSpecialCxxComment>.	
<SkipSpecialCxxComment>\n		{ yyLineNr++; }
<*>[ \t]*"/*!"|"/**"			{
  					  lastDContext = YY_START ;
					  BEGIN( SkipSpecialComment );
					}
<*>"/*"					{ 
  					  code->codify(yytext);
  					  lastCContext = YY_START ;
					  BEGIN( SkipComment ) ;
					}
<*>[ \t]*"//!"				{
  					  lastDContext = YY_START ;
					  BEGIN( SkipSpecialCxxComment );
  					}
<*>"//"					{ 
  					  code->codify(yytext);
  					  lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>.					{
  					  code->codify(yytext);
					}
<*>\n					{
  					  code->codify(yytext);
  					}
<*>([ \t\n]*"\n"){2,}			{ // combine multiple blank lines
  					  QString sepLine=yytext;
  					  code->codify("\n\n");
  					  yyLineNr+=sepLine.contains('\n'); 
					}

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

void parseCode(OutputList &ol,const char *className,const QString &s, 
                  bool e, const char *exName)
{
  code = new OutputList(&ol);
  if (s.length()==0) return;
  inputString   = s;
  inputPosition = 0;
  yyLineNr      = 0;
  curlyCount    = 0;
  bracketCount  = 0;
  sharpCount    = 0;
  classScope    = className;
  exampleBlock  = e; 
  exampleName   = exName;
  exampleFile   = convertSlashes(exampleName,TRUE)+"-example";
  type.resize(0);
  name.resize(0);
  args.resize(0);
  parmName.resize(0);
  parmType.resize(0);
  codeYYrestart( codeYYin );
  BEGIN( Body );
  codeYYlex();
  ol+=*code;
  delete code;
  return;
}

extern "C" { // some bogus code to keep the compiler happy
  int  codeYYwrap() { return 1 ; }
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
