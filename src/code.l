/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
static CodeClassDict g_codeClassDict(1009);
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
static const char *  g_currentFontClass;

// start a new line of code, inserting a line number if g_sourceFileDef
// is TRUE. If a definition starts at the current line, then the line
// number is linked to the documentation of that definition.
static void startCodeLine(OutputList &ol)
{
  if (g_currentFontClass)
  {
    g_code->endFontClass();
  }
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
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
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
  char *p=(char *)text;
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
  if (!g_type.isEmpty()) g_type += ' ' ;
  g_type += g_name ;
  g_name.resize(0) ;
  if (!g_type.isEmpty()) g_type += ' ' ;
  g_type += g_args ;
  g_args.resize(0) ;
}

static void addParmType()
{
  if (g_parmName=="const") { g_parmName.resize(0); return; }
  if (!g_parmType.isEmpty()) g_parmType += ' ' ;
  g_parmType += g_parmName ;
  g_parmName.resize(0) ;
}

static void setClassScope(const QCString &name)
{
  //printf("setClassScope(%s)\n",name.data());
  QCString n=name;
  n=n.simplifyWhiteSpace();
  int index;
  if ((index=n.findRev("::"))!=-1)
    g_classScope=n.left(index);
  else
    g_classScope.resize(0);
  //printf("--->New class scope `%s'\n",g_classScope.data());
}

static void addVariable()
{
  g_cvd.name=g_name.copy().simplifyWhiteSpace();
  g_cvd.type=g_type.copy().simplifyWhiteSpace();
  if (g_type.isEmpty()) 
  {
    return;
  }
  else 
  {
    //printf("adding variable `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
    if ((getClass(g_cvd.type)) || (g_codeClassDict[g_cvd.type])) 
    {
      g_cvd.classScope=g_classScope;
      g_codeVarList.append(new CodeVarDef(g_cvd)); // add it to a list
    }
  }
}

static void addParameter()
{
  g_cvd.name=g_parmName.copy().simplifyWhiteSpace();
  g_cvd.type=g_parmType.copy().simplifyWhiteSpace();
  if (g_cvd.type.isEmpty()) 
  {
    return;
  }
  else 
  {
    //printf("adding parameter `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
    if ((getClass(g_cvd.type)) || (g_codeClassDict[g_cvd.type])) 
    {
      g_cvd.classScope=g_classScope;
      g_codeParmList.append(new CodeVarDef(g_cvd)); // add it to a list
    }
  }
}

static void generateClassLink(OutputList &ol,char *clName,int *clNameLen=0)
{
  int i=0;
  QCString className=clName;
  if (clNameLen) *clNameLen=0;
  if (className.isEmpty()) return;
  ClassDef *cd=getClass(className);
  if (cd==0 && (i=className.find('<'))!=-1)
  {
    cd=getClass(className.left(i));
  }
  if (cd && cd->isLinkable())
  {
    if (g_exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("_a%d",g_anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
      //                                   g_exampleFile.data());
      if (cd->addExample(anchor,g_exampleName,g_exampleFile))
      {
	ol.pushGeneratorState();
	//bool latexOn = ol.isEnabled(OutputGenerator::Latex);
	//if (latexOn) ol.disable(OutputGenerator::Latex);
	ol.disable(OutputGenerator::Latex);
	ol.disable(OutputGenerator::RTF);
	ol.writeAnchor(anchor);
	//if (latexOn) ol.enable(OutputGenerator::Latex);
	ol.popGeneratorState();
	g_anchorCount++;
      }
    }
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),0,className);
    if (clNameLen) *clNameLen=className.length()-i-1;
  }
  else
  {
    codifyLines(clName);
    if (clNameLen) *clNameLen=className.length()-1;
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
  GroupDef     *gd;
  QCString m=memberName;
  QCString c=className;
  //printf("Trying `%s'::`%s'\n",c.data(),m.data());
  if (getDefs(c,m,"()",md,cd,fd,nd,gd) && md->isLinkable())
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
	//bool latexEnabled = result.isEnabled(OutputGenerator::Latex);
	result.pushGeneratorState();
	//if (latexEnabled) result.disable(OutputGenerator::Latex);
	result.disable(OutputGenerator::Latex);
	result.writeAnchor(anchor);
	result.popGeneratorState();
	//if (latexEnabled) result.enable(OutputGenerator::Latex);
	g_anchorCount++;
      }
    }
    Definition *d=0;
    if (cd) d=cd; else if (cd) d=nd; else if (fd) d=fd; else d=gd;

    if (d && d->isLinkable())
    {
      //printf("d->getOutputBase()=`%s' name=`%s'\n",d->getOutputFileBase().data(),md->name().data());
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
  static const QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
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
    if (vcd && vcd->isLinkable())
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
	    if (mcd && mcd->isLinkable())
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
  if (!locScope.isEmpty() && (ccd=g_codeClassDict[locScope]))
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
  endFontClass();
  g_code->startFontClass(s);
  g_currentFontClass=s;
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
KEYWORD ("asm"|"auto"|"break"|"case"|"catch"|"class"|"const_cast"|"continue"|"default"|"delete"|"do"|"dynamic_cast"|"else"|"enum"|"explicit"|"extern"|"false"|"for"|"friend"|"goto"|"if"|"inline"|"mutable"|"namespace"|"new"|"operator"|"private"|"protected"|"public"|"register"|"reinterpret_cast"|"return"|"sizeof"|"static"|"static_cast"|"struct"|"switch"|"template"|"this"|"throw"|"true"|"try"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"while")
TYPEKW  ("bool"|"char"|"const"|"double"|"float"|"int"|"long"|"short"|"signed"|"unsigned"|"void"|"wchar_t")

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
  					  startFontClass("preprocessor");
					  g_code->codify(yytext);
  					  BEGIN( ReadInclude ); 
					}
<Body>("class"|"struct"|"union")[ \t\n]+ { 
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
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
					  endFontClass();
					  BEGIN( Body );
  					}
<Body>^[ \t]*"#"			{ 
  					  startFontClass("preprocessor");
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
					  endFontClass();
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
					  if (!g_ccd.name.isEmpty())
					  {
					    g_classScope=g_ccd.name.copy();
					    CodeClassDef *cd=new CodeClassDef(g_ccd);
					    g_codeClassList.append(cd);
					    g_codeClassDict.insert(cd->name,cd);
					  }
					  BEGIN( Body );
 					}
<Bases>"virtual"|"public"|"protected"|"private" { 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
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
  					

<Body>{KEYWORD}/([^a-z_A-Z0-9]) {
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{KEYWORD}/{B}* {
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{KEYWORD}/{B}*"(" {
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  				          g_name.resize(0);g_type.resize(0);
  					}
<Body>[\\|\)\+\-\/\%\~\!]		{
  					  g_code->codify(yytext);
  				          g_name.resize(0);g_type.resize(0);
  					}
  /*
<Body>([a-z_A-Z~][a-z_A-Z0-9]*)/([ \t]*)	{ 
					  generateClassLink(*g_code,yytext);
					  addType();
  					  name+=yytext; 
					}
  */
<Body>{TYPEKW}/{B}* {
  					  startFontClass("keyword");
					  g_code->codify(yytext);
					  endFontClass();
					  addType();
  					  g_name+=yytext; 
  					}
<Body>{SCOPENAME}{B}*"<"[^\}\{\(\)\/\n\>]*">"/{B}*	{
  					  int i;
					  generateClassLink(*g_code,yytext,&i);
					  addType();
					  QCString text=yytext;
  					  g_name+=text.left(i); 
  					}
<Body>{SCOPENAME}/{B}* 			{ 
					  generateClassLink(*g_code,yytext);
  					  //codifyLines(yytext);
					  addType();
  					  g_name+=yytext; 
					}
<Body>{SCOPENAME}/{B}*"("		{
  					  addType();
					  //if (type.isEmpty())
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
<SkipString>[^\"\\\n]*			{ 
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
<Body>"this->"				{ g_code->codify(yytext); }
<Body>"."|"->"				{ 
  					  g_code->codify(yytext);
  					  BEGIN( MemberCall ); 
					}
<MemberCall>{SCOPENAME}/{B}*"(" 	{
  					  if (!g_name.isEmpty())
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
  					  if (!g_type.isEmpty()) 
					    addVariable();
					  g_name.resize(0);
					  if (*yytext!=',') g_type.resize(0);
					  g_args.resize(0);
  					}
<Body>[0-9]+				{
					  g_code->codify(yytext);
					}
<MemberCall2,FuncCall>({KEYWORD}|{TYPEKW})/([^a-z_A-Z0-9]) {
					  addParmType();
					  g_parmName=yytext; 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
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
					  if (!g_inClass && !g_type.isEmpty())
					    addVariable();
					  g_name.resize(0);g_type.resize(0);
					  g_parmType.resize(0);g_parmName.resize(0);
  					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>")"({BN}"const"|"volatile")*{BN}*"{" 	{
					  addParameter();
					  g_parmType.resize(0);g_parmName.resize(0);
					  if (g_name.find("::")!=-1) setClassScope(g_name);
					  g_code->codify(")");
					  startFontClass("keyword");
					  yytext[yyleng-1]='\0';
  					  codifyLines(yytext+1);
					  endFontClass();
					  g_code->codify("{");
					  g_curlyCount++;
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( Body );
  					}
					/*
<MemberCall2,FuncCall>")"[ \t\n]*":" 	{
					  addParameter();
					  g_parmType.resize(0);g_parmName.resize(0);
					  if (g_name.find("::")!=-1) setClassScope(g_name);
  					  codifyLines(yytext);
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( SkipInits );
  					}
					*/
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
  					  if (!g_args.isEmpty())
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
					  endFontClass();
  					  BEGIN( g_lastCContext ) ; 
					}
<SkipCxxComment>[^\r\n]*		{ 
  					  g_code->codify(yytext);
					}
<SkipCxxComment>\r			
<SkipCxxComment>\n			{
  					  unput('\n');
					  endFontClass();
					  BEGIN( g_lastCContext ) ;
  					}
<SkipCxxComment>.			{
  					  g_code->codify(yytext);
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
					  if (g_lastSpecialCContext==SkipCxxComment)
					  { // force end of C++ comment here
					    endFontClass();
					    BEGIN( g_lastCContext ) ;
					  }
					  else
					  {
  					    BEGIN(g_lastSpecialCContext);
					  }
  					}
<RemoveSpecialCComment>"*/"		{
  					  BEGIN(g_lastSpecialCContext);
  					}
<RemoveSpecialCComment>[^*\n]+
<RemoveSpecialCComment>"//"|"/*"
<RemoveSpecialCComment>\n  { g_yyLineNr++; }
<RemoveSpecialCComment>.
<*>\n({B}*"//"[!/][^\n]*\n)+		{ // remove special one-line comment
  					  if (Config::stripCommentsFlag)
					  {
					    g_yyLineNr+=((QCString)yytext).contains('\n');
					    g_code->endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine(*g_code);
					    }
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>^{B}*"//"[!/][^\n]*\n		{ // remove special one-line comment
  					  if (Config::stripCommentsFlag)
					  {
					    g_yyLineNr++;
					    g_code->endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine(*g_code);
					    }
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>"//"[!/][^\n]*\n			{ // strip special one-line comment
  					  if (Config::stripCommentsFlag)
					  {
					    char c[2]; c[0]='\n'; c[1]=0;
					    codifyLines(c);
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>\n{B}*"/*"[!*]/[^/*] 		{
					  if (Config::stripCommentsFlag)
  					  {
					    g_lastSpecialCContext = YY_START;
					    g_yyLineNr++;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
  					    g_lastCContext = YY_START ;
					    startFontClass("comment");
					    codifyLines(yytext);
					    BEGIN(SkipComment);
  					  }
					}
<*>^{B}*"/*"[!*]/[^/*]			{ // special C comment block at a new line
					  if (Config::stripCommentsFlag)
					  {
					    g_lastSpecialCContext = YY_START;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
  					    g_lastCContext = YY_START ;
					    startFontClass("comment");
					    g_code->codify(yytext);
					    BEGIN(SkipComment);
 					  }
					}
<*>"/*"[!*]/[^/*]			{ // special C comment block half way a line
					  if (Config::stripCommentsFlag)
					  {
					    g_lastSpecialCContext = YY_START;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
  					    g_lastCContext = YY_START ;
					    startFontClass("comment");
					    g_code->codify(yytext);
					    BEGIN(SkipComment);
					  }
					}
<*>"/*"("!"?)"*/"			{ if (!Config::stripCommentsFlag)
  					  {
					    startFontClass("comment");
					    g_code->codify(yytext);
					    endFontClass();
					  }
					}
<*>"/*"					{ 
					  startFontClass("comment");
  					  g_code->codify(yytext);
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipComment ) ;
					}
<*>"//"					{ 
  					  startFontClass("comment");
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
  g_currentFontClass = 0;
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
  endFontClass();
  //if (g_yyLineNr<=g_inputLines) code->endCodeLine();
  ol+=*g_code;
  delete g_code;
  return;
}

extern "C" { // some bogus code to keep the compiler happy
//  int  codeYYwrap() { return 1 ; }
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
