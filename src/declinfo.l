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

#include "declinfo.h"
#include "util.h"
  
  
#define YY_NO_UNPUT
  
/* -----------------------------------------------------------------
 *
 *	statics
 */
  
static const char * inputString;
static int	    inputPosition;
static QString      scope;
static QString      className;
static QString      classTempList;
static QString      funcTempList;
static QString      type;
static QString      name;
static QString      args;
static QString      tmpType;
static int          sharpCount;
static bool         classTempListFound;
static bool         funcTempListFound;
static QString      exceptionString;

static void addType()
{
  //printf("addType() type=`%s' scope=`%s' name=`%s'\n",
  //       type.data(),scope.data(),name.data());
  if (name.isEmpty() && scope.isEmpty()) return;
  if (!type.isNull()) type+=' ';
  if (!scope.isEmpty()) type+=scope+"::";
  type+=name;
  scope.resize(0);
  name.resize(0);
}
  
static void addTypeName()
{
  //printf("addTypeName() type=`%s' scope=`%s' name=`%s'\n",
  //       type.data(),scope.data(),name.data());
  if (name.isEmpty()) return;
  if (!type.isNull()) type+=' ';
  type+=name;
  name.resize(0);
}
  
#define YY_NEVER_INTERACTIVE 1
  
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
%x	Template
%x	ReadArgs
%x	Operator
%x	FuncPtr
%x	EndTemplate
%x	StripTempArgs
%x	SkipSharp
%x      ReadExceptions

%%

<Start>"operator"		{ // operator rule must be before {ID} rule
  				  name += yytext;
  				  BEGIN(Operator);
  				}
<Start>(~{B}*)?{ID}		{
  				  addTypeName();
				  name += yytext;
  				}
<Start>{B}*"::"{B}*		{ // found a scope specifier
 				  if (!scope.isEmpty())
				  {
				    scope+="::"+name; // add name to scope
				  }
				  else
				  {
  				    scope = name.copy(); // scope becomes name
				  }
				  name.resize(0);
  				}
<Start>[*&]+			{
  				  addType();
  				  type+=yytext;
  				}
<Start>{B}+			{
  				  addType();
  				}
<Start>{B}*"("{B}*"*"		{
  				  addType();
				  type+="(*";
  				}
<Start>{B}*")"			{
  				  type+=")";
  				}
<Start>{B}*"("			{ // TODO: function pointers
  				  args+="(";
  				  BEGIN(ReadArgs);
  				}
<Start>{B}*"["			{
  				  args+="[";
				  BEGIN(ReadArgs);
  				}
<Start>{B}*"<"			{
  				  name+="<";
				  sharpCount=0;
  				  BEGIN(Template);
  				}
<Template>"<"			{
  				  name+="<";
  				  sharpCount++;
  				}
<Template>">"			{
  				  name+=">";
  				  if (sharpCount)
				    --sharpCount;
				  else
				  {
				    BEGIN(Start);
				  }
  				}
<Template>.			{
  				  name+=*yytext;
  				}
<Operator>{B}*"()"{B}*"<>"{B}*/"("	{
  				  name+="() <>";
				  BEGIN(ReadArgs);
  				}
<Operator>{B}*"()"{B}*/"("	{
  				  name+="()";
				  BEGIN(ReadArgs);
  				}
<Operator>[^(]*{B}*("<>"{B}*)?/"(" {
  				  name+=yytext;
				  BEGIN(ReadArgs);
  				}
<ReadArgs>"throw"{B}*"("	{
  				  exceptionString="throw(";
				  BEGIN(ReadExceptions);
  				}
<ReadArgs>.			{
  				  args+=*yytext;
  				}
<ReadExceptions>.		{
  				  exceptionString+=*yytext;
  				}
<*>.

%%

/*@ ----------------------------------------------------------------------------
 */

void parseFuncDecl(const QString &decl,QString &cl,QString &ctl,QString &t,
                   QString &n,QString &a,QString &ftl,QString &exc)
{
  inputString   = decl;
  //printf("Input=`%s'\n",inputString);
  if (inputString==0) return;
  inputPosition = 0;
  classTempListFound = FALSE;
  funcTempListFound = FALSE;
  scope.resize(0);
  className.resize(0);
  classTempList.resize(0);
  funcTempList.resize(0);
  name.resize(0);
  type.resize(0);
  args.resize(0);
  exceptionString.resize(0);
  // first we try to find the type, scope, name and arguments
  declinfoYYrestart( declinfoYYin );
  BEGIN( Start );
  declinfoYYlex();

  cl=scope.copy();
  int i;
  if ((i=cl.find('<'))!=-1) // split up scope and template arguments
  {
    ctl=removeRedundantWhiteSpace(cl.right(cl.length()-i));
    cl=cl.left(i);
  }
  n=removeRedundantWhiteSpace(name);
  if ((i=n.find('<'))!=-1 && n.left(8)!="operator")
  {
    ftl=removeRedundantWhiteSpace(n.right(n.length()-i));
    n=n.left(i);
  }
  
  //ctl=classTempList.copy();
  //ftl=funcTempList.copy();
  t=removeRedundantWhiteSpace(type);
  a=removeRedundantWhiteSpace(args);
  exc=removeRedundantWhiteSpace(exceptionString);
  
  if (t.length()>0 && t.at(t.length()-1)==')')
  {
    a.prepend(")");
    t=t.left(t.length()-1);
  }
  //printf("type=`%s' class=`%s' name=`%s' args=`%s'\n",
  //        t.data(),cl.data(),n.data(),a.data());

  return;
  
  
}

extern "C" { // some bogus code to keep the compiler happy
  int  declinfoYYwrap() { return 1 ; }
  void declinfoYYdummy() { yy_flex_realloc(0,0); } 
}

#if 0
void dumpDecl(const char *s)
{
  QString className;
  QString classTNames;
  QString type;
  QString name;
  QString args;
  QString funcTNames;
  printf("-----------------------------------------\n");
  parseFuncDecl(s,className,classTNames,type,name,args,funcTNames);
  printf("type=`%s' class=`%s' classTempl=`%s' name=`%s' "
         "funcTemplateNames=`%s' args=`%s'\n",
	    type.data(),className.data(),classTNames.data(),
	    name.data(),funcTNames.data(),args.data()
	);
}

// some test code
int main()
{
  dumpDecl("A < T > :: Value * A < T > :: getValue < S > ( const A < T > & a )");
  dumpDecl("const A<T>::Value* A<T>::getValue<S>(const A<T>&a)");
  dumpDecl("func()");
  dumpDecl("friend void bla<>()");
  dumpDecl("name< T > :: operator () (int bla)");
  dumpDecl("name< T > :: operator << (int bla)");
  dumpDecl("name< T > :: operator << <> (int bla)");
  dumpDecl("className::func()");
  dumpDecl("void ( * Name < T > :: bla ) ( int, char * )"); 
}
#endif
