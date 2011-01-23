/******************************************************************************
 *
 * 
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

/*! \file
 *  This scanner is used to convert a string into a list of function or 
 *  template arguments. Each parsed argument results in a Argument struct,
 *  that is put into an ArgumentList in declaration order.
 *  Comment blocks for arguments can also be included in the string.
 *  The argument string does not contain new-lines (except inside any
 *  comment blocks).
 *  An Argument consists of the string fields: 
 *                 type,name,default value, and documentation
 *  The Argument list as a whole can be pure, constant or volatile.
 *
 *  Examples of input strings are:
 *  \code
 *    "(int a,int b) const"
 *    "(const char *s="hello world",int=5) = 0"
 *    "<class T,class N>"
 *    "(char c,const char)"
 *  \endcode
 *
 *  Note: It is not always possible to distinguish between the name and 
 *        type of an argument. In case of doubt the name is added to the
 *        type, and the matchArgumentList in util.cpp is be used to
 *        further determine the correct separation.
 */

%{

/*
 *	includes
 */
#include "qtbc.h"
#include <stdio.h>
//#include <iostream.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>

#include "defargs.h"
#include "entry.h"
#include "util.h"
  
#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *	state variables
 */
static const char      *g_inputString;
static int	        g_inputPosition;
static ArgumentList    *g_argList;
static QCString        *g_copyArgValue;
static QCString         g_curArgTypeName;
static QCString         g_curArgDefValue;
static QCString		g_curArgName;
static QCString		g_curArgDocs;
static QCString		g_curArgAttrib;
static QCString		g_curArgArray;
static QCString		g_extraTypeChars;
static int              g_argRoundCount;
static int              g_argSharpCount;
static int              g_argCurlyCount;
static int              g_readArgContext;
static int              g_lastDocContext;
static int              g_lastDocChar;

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
ID	[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*

%option noyywrap

%x      Start
%x      CopyArgString
%x	CopyArgRound
%x	CopyArgRound2
%x	CopyArgSharp
%x	CopyArgCurly
%x	ReadFuncArgType
%x	ReadFuncArgDef
%x	ReadFuncArgPtr
%x	FuncQual
%x	ReadDocBlock
%x	ReadDocLine


%%

<Start>[<(]				{ BEGIN(ReadFuncArgType); }

<ReadFuncArgType>{B}*			{
  					  g_curArgTypeName+=" ";
  					}
<ReadFuncArgType>"["[^\]]*"]"		{ 
					  if (g_curArgTypeName.stripWhiteSpace().isEmpty())
					  {
					    g_curArgAttrib=yytext; // for M$-IDL
					  }
					  else // array type
					  {
					    g_curArgArray+=yytext;
					  }
					}
<ReadFuncArgDef>"'"\\[0-7]{1,3}"'"	{ g_curArgDefValue+=yytext; }
<ReadFuncArgDef>"'"\\."'"		{ g_curArgDefValue+=yytext; }
<ReadFuncArgDef>"'"."'"			{ g_curArgDefValue+=yytext; }
<ReadFuncArgDef>\"			{
  					  g_curArgDefValue+=*yytext;
  					  BEGIN( CopyArgString );
  					}
<ReadFuncArgType>"("([^:)]+{B}*"::")*{B}*[&*\^]+{B}*/{ID} { 
  					  // function pointer as argument
					  g_curArgTypeName+=yytext;
					  //g_curArgTypeName=g_curArgTypeName.simplifyWhiteSpace();
					  BEGIN( ReadFuncArgPtr );
  					}
<ReadFuncArgPtr>{ID}			{
					  g_curArgName=yytext;
  					}
<ReadFuncArgPtr>")"{B}*"("		{ // function pointer
					  g_curArgTypeName+=yytext;
					  //g_curArgTypeName=g_curArgTypeName.simplifyWhiteSpace();
					  g_readArgContext = ReadFuncArgType;
					  g_copyArgValue=&g_curArgTypeName;
					  g_argRoundCount=0;
					  BEGIN( CopyArgRound2 );
					}
<ReadFuncArgPtr>")"/{B}*"["		{ // pointer to fixed size array
					  g_curArgTypeName+=yytext;
					  g_curArgTypeName+=g_curArgName;
					  //g_curArgTypeName=g_curArgTypeName.simplifyWhiteSpace();
					  BEGIN( ReadFuncArgType );
					}
<ReadFuncArgPtr>")"			{ // redundant braces detected / remove them
					  int i=g_curArgTypeName.findRev('('),l=g_curArgTypeName.length();
					  if (i!=-1)
					    g_curArgTypeName=g_curArgTypeName.left(i)+
					                   g_curArgTypeName.right(l-i-1);
					  g_curArgTypeName+=g_curArgName;
					  BEGIN( ReadFuncArgType );
					}
<ReadFuncArgType>"<="|">="|"->"|">>"|"<<" { // handle operators in defargs
  					  g_curArgTypeName+=yytext;
  					}
<ReadFuncArgType,ReadFuncArgDef>[({<]	{	 
					  if (YY_START==ReadFuncArgType)
					  {
					    g_curArgTypeName+=*yytext;
					    g_copyArgValue=&g_curArgTypeName;
					  }
					  else // YY_START==ReadFuncArgDef
					  {
					    g_curArgDefValue+=*yytext;
					    g_copyArgValue=&g_curArgDefValue;
					  }
					  g_readArgContext = YY_START; 
					  if (*yytext=='(')
					  {
					    g_argRoundCount=0; 
					    BEGIN( CopyArgRound ); 
					  }
					  else if (*yytext=='{')
					  {
					    g_argCurlyCount=0; 
					    BEGIN( CopyArgCurly ); 
					  }
					  else // yytext=='<'
					  {
					    g_argSharpCount=0; 
					    BEGIN( CopyArgSharp ); 
					  }
					}
<CopyArgRound,CopyArgRound2>"("		{
  					  g_argRoundCount++;
					  *g_copyArgValue += *yytext;
  					}
<CopyArgRound,CopyArgRound2>")"({B}*{ID})* {
					  *g_copyArgValue += yytext;
					  if (g_argRoundCount>0) 
					  {
					    g_argRoundCount--;
					  }
					  else 
					  {
					    if (YY_START==CopyArgRound2)
					    {
					      *g_copyArgValue+=" "+g_curArgName;
					    }
					    BEGIN( g_readArgContext );
					  }
  					}
<CopyArgSharp>"<"			{
  					  g_argSharpCount++;
					  *g_copyArgValue += *yytext;
  					}
<CopyArgSharp>">"			{
					  *g_copyArgValue += *yytext;
					  if (g_argSharpCount>0) g_argSharpCount--;
					  else BEGIN( g_readArgContext );
  					}
<CopyArgCurly>"{"			{
  					  g_argCurlyCount++;
					  *g_copyArgValue += *yytext;
  					}
<CopyArgSharp>"}"			{
					  *g_copyArgValue += *yytext;
					  if (g_argCurlyCount>0) g_argCurlyCount--;
					  else BEGIN( g_readArgContext );
  					}
<CopyArgString>\\.			{
					  g_curArgDefValue+=yytext;
  					}
<CopyArgString>\"			{
					  g_curArgDefValue+=*yytext;
					  BEGIN( ReadFuncArgDef );
  					}
<ReadFuncArgType>"="			{
					  BEGIN( ReadFuncArgDef );
  					}
<ReadFuncArgType,ReadFuncArgDef>[,)>]{B}*("/*"[*!]|"//"[/!])"<" {
					  g_lastDocContext=YY_START;
					  g_lastDocChar=*yytext;  
					  QCString text=yytext;
					  if (text.find("//")!=-1)
					    BEGIN( ReadDocLine );
					  else
					    BEGIN( ReadDocBlock );
  					}
<ReadFuncArgType,ReadFuncArgDef>[,)>]	{
  					  if (*yytext==')' && g_curArgTypeName.stripWhiteSpace().isEmpty())
					  {
					    g_curArgTypeName+=*yytext;
					    BEGIN(FuncQual);
					  }
					  else
					  {
					    g_curArgTypeName=removeRedundantWhiteSpace(g_curArgTypeName);
					    g_curArgDefValue=g_curArgDefValue.stripWhiteSpace();
					    //printf("curArgType=`%s' curArgDefVal=`%s'\n",g_curArgTypeName.data(),g_curArgDefValue.data());
					    int l=g_curArgTypeName.length();
					    if (l>0)
					    {
					      int i=l-1;
					      while (i>=0 && (isspace((uchar)g_curArgTypeName.at(i)) || g_curArgTypeName.at(i)=='.')) i--;
					      while (i>=0 && isId(g_curArgTypeName.at(i))) i--;
					      Argument *a = new Argument; 
					      a->attrib = g_curArgAttrib.copy();
					      //printf("a->type=%s a->name=%s i=%d l=%d\n",
					      //        a->type.data(),a->name.data(),i,l);
					      a->array.resize(0);
					      if (i==l-1 && g_curArgTypeName.at(i)==')') // function argument
					      {
						int bi=g_curArgTypeName.find('(');
						int fi=bi-1;
						//printf("func arg fi=%d\n",fi);
						while (fi>=0 && isId(g_curArgTypeName.at(fi))) fi--;
						if (fi>=0)
						{
						  a->type  = g_curArgTypeName.left(fi+1);
						  a->name  = g_curArgTypeName.mid(fi+1,bi-fi-1).stripWhiteSpace();
						  a->array = g_curArgTypeName.right(l-bi);
						}
						else
						{
						  a->type = g_curArgTypeName;
						}
					      }
					      else if (i>=0 && g_curArgTypeName.at(i)!=':')
					      { // type contains a name
						a->type = removeRedundantWhiteSpace(g_curArgTypeName.left(i+1)).stripWhiteSpace();
						a->name = g_curArgTypeName.right(l-i-1).stripWhiteSpace();

						// if the type becomes a type specifier only then we make a mistake
						// and need to correct it to avoid seeing a nameless parameter
						// "struct A" as a parameter with type "struct" and name "A".
						int sv=0;
						if      (a->type.left(6)=="const ")    sv=6;
						else if (a->type.left(9)=="volatile ") sv=9;

						if (a->type.mid(sv)=="struct"    ||
						    a->type.mid(sv)=="union"     ||
						    a->type.mid(sv)=="class"     ||
						    a->type.mid(sv)=="typename"  ||
						    a->type=="const"             ||
						    a->type=="volatile"
						   )
						{ 
						  a->type = a->type + " " + a->name;
						  a->name.resize(0);
						}
						//printf(" --> a->type='%s'\n",a->type.data());
					      }
					      else // assume only the type was specified, try to determine name later 
					      {
						a->type = removeRedundantWhiteSpace(g_curArgTypeName);  
					      }
					      a->array  += removeRedundantWhiteSpace(g_curArgArray);
					      //printf("array=%s\n",a->array.data());
					      int alen = a->array.length();
					      if (alen>2 && a->array.at(0)=='(' && 
						            a->array.at(alen-1)==')') // fix-up for int *(a[10])
					      {
						int i=a->array.find('[')-1;
						a->array = a->array.mid(1,alen-2);
						if (i>0 && a->name.isEmpty())
						{
						  a->name  = a->array.left(i).stripWhiteSpace();
						  a->array = a->array.mid(i);
						}
					      }
					      a->defval = g_curArgDefValue.copy();
					      //printf("a->type=%s a->name=%s a->defval=\"%s\"\n",a->type.data(),a->name.data(),a->defval.data());
					      a->docs   = g_curArgDocs.stripWhiteSpace();
					      //printf("Argument `%s' `%s' adding docs=`%s'\n",a->type.data(),a->name.data(),a->docs.data());
					      g_argList->append(a);
					    }
					    g_curArgAttrib.resize(0);
					    g_curArgTypeName.resize(0);
					    g_curArgDefValue.resize(0);
					    g_curArgArray.resize(0);
					    g_curArgDocs.resize(0);
					    if (*yytext==')')
					    {
					      BEGIN(FuncQual);
					      //printf(">>> end of argument list\n");
					    }
					    else
					    {
					      BEGIN( ReadFuncArgType );
					    }
					  }
  					}
<ReadFuncArgType,ReadFuncArgPtr>{ID}	{ 
  					  QCString name=yytext; //resolveDefines(yytext);
					  if (YY_START==ReadFuncArgType && g_curArgArray=="[]") // Java style array
					  {
					    g_curArgTypeName+=" []";
					    g_curArgArray.resize(0);
					  }
					  //printf("resolveName `%s'->`%s'\n",yytext,name.data());
  					  g_curArgTypeName+=name;
					}
<ReadFuncArgType,ReadFuncArgPtr>.	{ 
  					  g_curArgTypeName+=*yytext;
					}

<ReadFuncArgDef,CopyArgString>"->"|">="|">>"	{
  					  g_curArgDefValue+=yytext;
  					}
<ReadFuncArgDef,CopyArgString>.		{
					  g_curArgDefValue+=*yytext;
  					}
<CopyArgRound,CopyArgRound2,CopyArgSharp,CopyArgCurly>{ID}  {
  					  QCString name=yytext; //resolveDefines(yytext);
					  *g_copyArgValue+=name;
					}
<CopyArgRound,CopyArgRound2,CopyArgSharp,CopyArgCurly>.  {
					  *g_copyArgValue += *yytext;
					}
<FuncQual>"const"		       	{ 
					  g_argList->constSpecifier=TRUE;
					}
<FuncQual>"volatile"		    	{ 
					  g_argList->volatileSpecifier=TRUE;
					}
<FuncQual>"="{B}*"0"		  	{ 
					  g_argList->pureSpecifier=TRUE;
					}
<FuncQual>")"{B}*"["[^]]*"]"		{ // for functions returning a pointer to an array, 
                                          // i.e. ")[]" in "int (*f(int))[4]" with argsString="(int))[4]"
  					  g_extraTypeChars=yytext;
  					}
<ReadDocBlock>[^\*\n]+			{
  					  g_curArgDocs+=yytext;
  					}
<ReadDocLine>[^\n]+			{
  					  g_curArgDocs+=yytext;
  					}
<ReadDocBlock>"*/"			{ 
  					  if (g_lastDocChar!=0)
					    unput(g_lastDocChar);
  					  BEGIN(g_lastDocContext); 
					}
<ReadDocLine>\n				{
  					  if (g_lastDocChar!=0)
					    unput(g_lastDocChar);
					  BEGIN(g_lastDocContext);
  					}
<ReadDocBlock>\n			{
  					  g_curArgDocs+=*yytext;
  					}
<ReadDocBlock>.				{
  					  g_curArgDocs+=*yytext;
  					}
<*>("/*"[*!]|"//"[/!])("<"?)		{
  					  g_lastDocContext=YY_START;
					  g_lastDocChar=0;  
					  if (yytext[1]=='/')
					    BEGIN( ReadDocLine );
					  else
  					    BEGIN( ReadDocBlock );
  					}
<*>\n
<*>.

%%

/* ----------------------------------------------------------------------------
 */

/*! Converts an argument string into an ArgumentList.
 *  \param argsString the list of Arguments.
 *  \param al a reference to resulting argument list pointer.
 */
 
void stringToArgumentList(const char *argsString,ArgumentList* al,QCString *extraTypeChars)
{
  if (al==0) return; 
  if (argsString==0) return;

  g_copyArgValue=0;
  g_curArgDocs.resize(0);
  g_curArgAttrib.resize(0);
  g_curArgArray.resize(0);
  g_extraTypeChars.resize(0);
  g_argRoundCount = 0;
  g_argSharpCount = 0;
  g_argCurlyCount = 0;
  g_lastDocChar = 0;

  g_inputString   = argsString;
  g_inputPosition = 0;
  g_curArgTypeName.resize(0);
  g_curArgDefValue.resize(0);
  g_curArgName.resize(0);
  g_argList = al;
  defargsYYrestart( defargsYYin );
  BEGIN( Start );
  defargsYYlex();
  if (extraTypeChars) *extraTypeChars=g_extraTypeChars;
  //printf("stringToArgumentList(%s) result=%s\n",argsString,argListToString(al).data());
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
  void defargsYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

