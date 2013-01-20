/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
//#include <iostream.h>
#include <assert.h>
#include <ctype.h>

#include "declinfo.h"
#include "util.h"
#include "message.h"

#define YY_NO_INPUT 1
  
/* -----------------------------------------------------------------
 *
 *	statics
 */
  
static const char * inputString;
static int	    inputPosition;
static QCString      scope;
static QCString      className;
static QCString      classTempList;
static QCString      funcTempList;
static QCString      type;
static QCString      name;
static QCString      args;
static QCString      tmpType;
static int          sharpCount;
static bool         classTempListFound;
static bool         funcTempListFound;
static QCString      exceptionString;
static bool          insideObjC;

static void addType()
{
  //printf("addType() type=`%s' scope=`%s' name=`%s'\n",
  //       type.data(),scope.data(),name.data());
  if (name.isEmpty() && scope.isEmpty()) return;
  if (!type.isEmpty()) type+=" ";
  if (!scope.isEmpty()) type+=scope+"::";
  type+=name;
  scope.resize(0);
  name.resize(0);
}
  
static void addTypeName()
{
  //printf("addTypeName() type=`%s' scope=`%s' name=`%s'\n",
  //       type.data(),scope.data(),name.data());
  if (name.isEmpty() || 
      name.at(name.length()-1)==':')  // end of Objective-C keyword => append to name not type
  {
    return;
  }
  if (!type.isEmpty()) type+=' ';
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
ID	"$"?([a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*)|(@[0-9]+)

%option nounput
%option noyywrap

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

<Start>"operator"/({B}*"["{B}*"]")* 	{ // operator rule must be before {ID} rule
  				  name += yytext;
  				  BEGIN(Operator);
  				}
<Start>{ID}{B}*"("{B}*{ID}{B}*")" { // Objective-C class categories
  				  if (!insideObjC) 
				  {
				    REJECT;
				  }
				  else 
				  {
				    name += yytext;
				  }
  				}
<Start>([~!]{B}*)?{ID}/({B}*"["{B}*"]")* { // the []'s are for Java, 
                                        // the / was add to deal with multi-
                                        // dimensional C++ arrays like A[][15]
                                        // the leading ~ is for a destructor
                                        // the leading ! is for a C++/CLI finalizer (see bug 456475 and 635198)
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
<Start>{B}*":"			{ // Objective-C argument separator
  				  name+=yytext;
  				}
<Start>[*&]+			{
  				  addType();
  				  type+=yytext;
  				}
<Start>{B}+			{
  				  addType();
  				}
<Start>{B}*"("({ID}"::")*{B}*[&*]({B}*("const"|"volatile"){B}+)?	{
  				  addType();
				  QCString text=yytext;
				  type+=text.stripWhiteSpace();
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
<Template>"<<"			{ name+="<<"; }
<Template>">>"			{ name+=">>"; }
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
<Operator>{B}*"("{B}*")"{B}*"<>"{B}*/"("	{
  				  name+="() <>";
				  BEGIN(ReadArgs);
  				}
<Operator>{B}*"("{B}*")"{B}*/"("	{
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
<*>\n

%%

/*@ ----------------------------------------------------------------------------
 */

void parseFuncDecl(const QCString &decl,bool objC,QCString &cl,QCString &t,
                   QCString &n,QCString &a,QCString &ftl,QCString &exc)
{
  inputString   = decl;
  //printf("Input=`%s'\n",inputString);
  if (inputString==0) return;
  inputPosition      = 0;
  classTempListFound = FALSE;
  funcTempListFound  = FALSE;
  insideObjC = objC;
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

  //printf("type=`%s' class=`%s' name=`%s' args=`%s'\n",
  //        type.data(),scope.data(),name.data(),args.data());

  int nb = name.findRev('[');
  if (nb!=-1 && args.isEmpty()) // correct for [] in name ambigity (due to Java return type allowing [])
  {
    args.prepend(name.right(name.length()-nb));
    name=name.left(nb);
  }

#if 0
  {
    int l=scope.length();
    int i=0;
    int skipCount=0;
    cl.resize(0);
    ctl.resize(0);
    for (i=0;i<l;i++)
    {
      char c=scope.at(i);
      if (c=='<') 
	skipCount++;
      else if (c=='>') 
	skipCount--;
      else if (skipCount==0) 
	cl+=c;
    }
  }
  cl=stripTemplateSpecifiersFromScope(removeRedundantWhiteSpace(scope),FALSE); 
  ctl.resize(0);
#endif

  cl=scope;
  n=removeRedundantWhiteSpace(name);
  int il,ir;
  if ((il=n.find('<'))!=-1 && (ir=n.findRev('>'))!=-1)
    // TODO: handle cases like where n="operator<< <T>" 
  {
    ftl=removeRedundantWhiteSpace(n.right(n.length()-il));
    n=n.left(il);
  }
  
  //ctl=classTempList.copy();
  //ftl=funcTempList.copy();
  t=removeRedundantWhiteSpace(type);
  a=removeRedundantWhiteSpace(args);
  exc=removeRedundantWhiteSpace(exceptionString);
  
  if (!t.isEmpty() && t.at(t.length()-1)==')') // for function pointers
  {
    a.prepend(")");
    t=t.left(t.length()-1);
  }
  //printf("type=`%s' class=`%s' name=`%s' args=`%s'\n",
  //        t.data(),cl.data(),n.data(),a.data());

  return;
  
  
}

//extern "C" { // some bogus code to keep the compiler happy
//  int  declinfoYYwrap() { return 1 ; }
//  void declinfoYYdummy() { yy_flex_realloc(0,0); } 
//}

#if 0
void dumpDecl(const char *s)
{
  QCString className;
  QCString classTNames;
  QCString type;
  QCString name;
  QCString args;
  QCString funcTNames;
  msg("-----------------------------------------\n");
  parseFuncDecl(s,className,classTNames,type,name,args,funcTNames);
  msg("type=`%s' class=`%s' classTempl=`%s' name=`%s' "
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

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void declinfoYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

