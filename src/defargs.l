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

#include "defargs.h"
#include "entry.h"
#include "util.h"
  
#define YY_NO_UNPUT
#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 *	statics
 */
  
static const char      *inputString;
static int	        inputPosition;
static ArgumentList    *argList;
static QString         *copyArgValue;
static QString          curArgTypeName;
static QString          curArgDefValue;
static QString		curArgName;
static int              argRoundCount;
static int              argSharpCount;
static int              argCurlyCount;
static int              readArgContext;



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
ID	[a-z_A-Z][a-z_A-Z0-9]*

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


%%

<Start>[<(]				{ BEGIN(ReadFuncArgType); }

<ReadFuncArgType>{B}*			{
  					  curArgTypeName+=" ";
  					}
<ReadFuncArgDef>"'"\\[0-7]{1,3}"'"	{ curArgDefValue+=yytext; }
<ReadFuncArgDef>"'"\\."'"		{ curArgDefValue+=yytext; }
<ReadFuncArgDef>"'"."'"			{ curArgDefValue+=yytext; }
<ReadFuncArgDef>\"			{
  					  curArgDefValue+=*yytext;
  					  BEGIN( CopyArgString );
  					}
<ReadFuncArgType>"("([^:)]+{B}*"::")*{B}*[&*]+{B}*/{ID} { 
  					  // function pointer as argument
					  curArgTypeName+=yytext;
					  //curArgTypeName=curArgTypeName.simplifyWhiteSpace();
					  BEGIN( ReadFuncArgPtr );
  					}
<ReadFuncArgPtr>{ID}			{
					  curArgName=yytext;
  					}
<ReadFuncArgPtr>")"{B}*"("		{
					  curArgTypeName+=yytext;
					  //curArgTypeName=curArgTypeName.simplifyWhiteSpace();
					  readArgContext = ReadFuncArgType;
					  copyArgValue=&curArgTypeName;
					  argRoundCount=0;
					  BEGIN( CopyArgRound2 );
					}
<ReadFuncArgPtr>")"			{ // redundant braces detected / remove them
					  int i=curArgTypeName.findRev('('),l=curArgTypeName.length();
					  if (i!=-1)
					    curArgTypeName=curArgTypeName.left(i)+
					                   curArgTypeName.right(l-i-1);
					  curArgTypeName+=curArgName;
					  BEGIN( ReadFuncArgType );
					}
<ReadFuncArgType,ReadFuncArgDef>[({<]	{	 
					  if (YY_START==ReadFuncArgType)
					  {
					    curArgTypeName+=*yytext;
					    copyArgValue=&curArgTypeName;
					  }
					  else // YY_START==ReadFuncArgDef
					  {
					    curArgDefValue+=*yytext;
					    copyArgValue=&curArgDefValue;
					  }
  					  readArgContext = YY_START; 
					  if (*yytext=='(')
					  {
  					    argRoundCount=0; 
					    BEGIN( CopyArgRound ); 
					  }
					  else if (*yytext=='{')
					  {
					    argCurlyCount=0; 
					    BEGIN( CopyArgCurly ); 
					  }
					  else // yytext=='<'
					  {
					    argSharpCount=0; 
					    BEGIN( CopyArgSharp ); 
					  }
					}
<CopyArgRound,CopyArgRound2>"("		{
  					  argRoundCount++;
					  *copyArgValue += *yytext;
  					}
<CopyArgRound,CopyArgRound2>")"		{
					  *copyArgValue += *yytext;
					  if (argRoundCount>0) 
					  {
					    argRoundCount--;
					  }
					  else 
					  {
					    if (YY_START==CopyArgRound2)
					    {
					      *copyArgValue+=" "+curArgName;
					    }
					    BEGIN( readArgContext );
					  }
  					}
<CopyArgSharp>"<"			{
  					  argSharpCount++;
					  *copyArgValue += *yytext;
  					}
<CopyArgSharp>">"			{
					  *copyArgValue += *yytext;
					  if (argSharpCount>0) argSharpCount--;
					  else BEGIN( readArgContext );
  					}
<CopyArgCurly>"{"			{
  					  argCurlyCount++;
					  *copyArgValue += *yytext;
  					}
<CopyArgSharp>"}"			{
					  *copyArgValue += *yytext;
					  if (argCurlyCount>0) argCurlyCount--;
					  else BEGIN( readArgContext );
  					}
<CopyArgString>\\.			{
					  curArgDefValue+=yytext;
  					}
<CopyArgString>\"			{
					  curArgDefValue+=*yytext;
					  BEGIN( ReadFuncArgDef );
  					}
<ReadFuncArgType>"="			{
					  BEGIN( ReadFuncArgDef );
  					}
<ReadFuncArgType,ReadFuncArgDef>[,)>]	{
					  curArgTypeName=removeRedundantWhiteSpace(
					                   curArgTypeName.simplifyWhiteSpace());
					  curArgDefValue=curArgDefValue.stripWhiteSpace();
					  int l=curArgTypeName.length();
					  if (l>0)
					  {
					    int i=l-1;
					    while (i>=0 && 
						   (
						    isspace(curArgTypeName.at(i)) || 
						    isId(curArgTypeName.at(i))
						   )
						  ) i--;
					    Argument *a = new Argument; 
					    if (i>=0 && curArgTypeName.at(i)!=':')
					    { // type contains a name
					      a->type = curArgTypeName.left(i+1).stripWhiteSpace();
					      a->name = curArgTypeName.right(curArgTypeName.length()-i-1);
					    }
					    else // assume only the type was specified, try to determine name later 
					    {
					      a->type = curArgTypeName.stripWhiteSpace();  
					    }
					    a->defval = curArgDefValue.copy();
					    //printf("----> Adding argument `%s' `%s' `%s'\n",a->type.data(),a->name.data(),a->defval.data());
					    argList->append(a);
					  }
					  curArgTypeName.resize(0);
  					  curArgDefValue.resize(0);
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
<ReadFuncArgType,ReadFuncArgPtr>{ID}	{ 
  					  QString name=yytext; //resolveDefines(yytext);
					  //printf("resolveName `%s'->`%s'\n",yytext,name.data());
  					  curArgTypeName+=name;
					}
<ReadFuncArgType,ReadFuncArgPtr>.	{ 
  					  curArgTypeName+=*yytext;
					}
<ReadFuncArgDef,CopyArgString>.		{
					  curArgDefValue+=*yytext;
  					}
<CopyArgRound,CopyArgRound2,CopyArgSharp,CopyArgCurly>{ID}  {
  					  QString name=yytext; //resolveDefines(yytext);
					  *copyArgValue+=name;
					}
<CopyArgRound,CopyArgRound2,CopyArgSharp,CopyArgCurly>.  {
					  *copyArgValue += *yytext;
					}
<FuncQual>"const"		       	{ 
					  argList->constSpecifier=TRUE;
					}
<FuncQual>"volatile"		    	{ 
					  argList->volatileSpecifier=TRUE;
					}
<FuncQual>"="{B}*"0"		  	{ 
					  argList->pureSpecifier=TRUE;
					}
<*>.

%%

/* ----------------------------------------------------------------------------
 */

// converts an argument string into a list of Arguments.
// an Argument consists of a type, an optional name, and an optional 
// default initializer.

void stringToArgumentList(const char *argsString,ArgumentList* &al)
{
  //if (al==0) al=new ArgumentList; // allocate new list if needed.
  if (al==0) return; 
  if (!argsString) return;
  inputString   = argsString;
  inputPosition = 0;
  curArgTypeName.resize(0);
  curArgDefValue.resize(0);
  curArgName.resize(0);
  argList = al;
  defargsYYrestart( defargsYYin );
  BEGIN( Start );
  defargsYYlex();
}

extern "C" { // some bogus code to keep the compiler happy
  int  defargsYYwrap()  { return 1 ; }
  void defargsYYdummy() { yy_flex_realloc(0,0); } 
}
