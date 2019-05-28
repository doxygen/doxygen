/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
%option never-interactive
%option prefix="declinfoYY"
%option nounput
%option noyywrap
%option reentrant
%option extra-type="struct declinfoYY_state *"

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
#define YY_NO_UNISTD_H 1
#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 *	statics
 */
struct declinfoYY_state
{
     const char  *inputString;
     int          inputPosition;
     QCString     scope;
     QCString     className;
     QCString     classTempList;
     QCString     funcTempList;
     QCString     type;
     QCString     name;
     QCString     args;
     int          sharpCount;
     bool         classTempListFound;
     bool         funcTempListFound;
     QCString     exceptionString;
     bool         insideObjC;
};

static void addType(yyscan_t yyscanner);
static void addTypeName(yyscan_t yyscanner);
static int yyread(char *buf,int max_size, yyscan_t yyscanner);

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size,yyscanner);
%}

B       [ \t]
ID	"$"?([a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*)|(@[0-9]+)

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
  				  yyextra->name += yytext;
  				  BEGIN(Operator);
  				}
<Start>{ID}{B}*"("{B}*{ID}{B}*")" { // Objective-C class categories
  				  if (!yyextra->insideObjC) 
				  {
				    REJECT;
				  }
				  else 
				  {
				    yyextra->name += yytext;
				  }
  				}
<Start>([~!]{B}*)?{ID}/({B}*"["{B}*"]")* { // the []'s are for Java, 
                                        // the / was add to deal with multi-
                                        // dimensional C++ arrays like A[][15]
                                        // the leading ~ is for a destructor
                                        // the leading ! is for a C++/CLI finalizer (see bug 456475 and 635198)
  				  addTypeName(yyscanner);
				  yyextra->name += yytext;
  				}
<Start>{B}*"::"{B}*		{ // found a yyextra->scope specifier
 				  if (!yyextra->scope.isEmpty())
				  {
				    yyextra->scope+="::"+yyextra->name; // add yyextra->name to yyextra->scope
				  }
				  else
				  {
  				    yyextra->scope = yyextra->name.copy(); // yyextra->scope becomes yyextra->name
				  }
				  yyextra->name.resize(0);
  				}
<Start>{B}*":"			{ // Objective-C argument separator
  				  yyextra->name+=yytext;
  				}
<Start>[*&]+			{
  				  addType(yyscanner);
  				  yyextra->type+=yytext;
  				}
<Start>{B}+			{
  				  addType(yyscanner);
  				}
<Start>{B}*"("({ID}"::")*{B}*[&*]({B}*("const"|"volatile"){B}+)?	{
  				  addType(yyscanner);
				  QCString text=yytext;
				  yyextra->type+=text.stripWhiteSpace();
  				}
<Start>{B}*")"			{
  				  yyextra->type+=")";
  				}
<Start>{B}*"("			{ // TODO: function pointers
  				  yyextra->args+="(";
  				  BEGIN(ReadArgs);
  				}
<Start>{B}*"["			{
  				  yyextra->args+="[";
				  BEGIN(ReadArgs);
  				}
<Start>{B}*"<"			{
  				  yyextra->name+="<";
				  yyextra->sharpCount=0;
  				  BEGIN(Template);
  				}
<Template>"<<"			{ yyextra->name+="<<"; }
<Template>">>"			{ yyextra->name+=">>"; }
<Template>"<"			{
  				  yyextra->name+="<";
  				  yyextra->sharpCount++;
  				}
<Template>">"			{
  				  yyextra->name+=">";
  				  if (yyextra->sharpCount)
				    --yyextra->sharpCount;
				  else
				  {
				    BEGIN(Start);
				  }
  				}
<Template>.			{
  				  yyextra->name+=*yytext;
  				}
<Operator>{B}*"("{B}*")"{B}*"<>"{B}*/"("	{
  				  yyextra->name+="() <>";
				  BEGIN(ReadArgs);
  				}
<Operator>{B}*"("{B}*")"{B}*/"("	{
  				  yyextra->name+="()";
				  BEGIN(ReadArgs);
  				}
<Operator>[^(]*{B}*("<>"{B}*)?/"(" {
  				  yyextra->name+=yytext;
				  BEGIN(ReadArgs);
  				}
<ReadArgs>"throw"{B}*"("	{
  				  yyextra->exceptionString="throw(";
				  BEGIN(ReadExceptions);
  				}
<ReadArgs>.			{
  				  yyextra->args+=*yytext;
  				}
<ReadExceptions>.		{
  				  yyextra->exceptionString+=*yytext;
  				}
<*>.
<*>\n

%%

static void addType(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("addType() yyextra->type=`%s' yyextra->scope=`%s' yyextra->name=`%s'\n",
  //       yyextra->type.data(),yyextra->scope.data(),yyextra->name.data());
  if (yyextra->name.isEmpty() && yyextra->scope.isEmpty()) return;
  if (!yyextra->type.isEmpty()) yyextra->type+=" ";
  if (!yyextra->scope.isEmpty()) yyextra->type+=yyextra->scope+"::";
  yyextra->type+=yyextra->name;
  yyextra->scope.resize(0);
  yyextra->name.resize(0);
}

static void addTypeName(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("addTypeName() yyextra->type=`%s' yyextra->scope=`%s' yyextra->name=`%s'\n",
  //       yyextra->type.data(),yyextra->scope.data(),yyextra->name.data());
  if (yyextra->name.isEmpty() || 
      yyextra->name.at(yyextra->name.length()-1)==':')  // end of Objective-C keyword => append to yyextra->name not yyextra->type
  {
    return;
  }
  if (!yyextra->type.isEmpty()) yyextra->type+=' ';
  yyextra->type+=yyextra->name;
  yyextra->name.resize(0);
}

static int yyread(char *buf,int max_size, yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    int c=0;
    while( c < max_size && yyextra->inputString[yyextra->inputPosition] )
    {
	*buf = yyextra->inputString[yyextra->inputPosition++] ;
	c++; buf++;
    }
    return c;
}

/*@ public interface------------------------------------------------------------
 */
static yyscan_t g_yyscanner;
static struct declinfoYY_state g_declinfo_extra;

void parseFuncDecl(const QCString &decl,bool objC,QCString &cl,QCString &t,
                   QCString &n,QCString &a,QCString &ftl,QCString &exc)
{
  if (decl.isEmpty())
  {
    return;
  }
  declinfoYYlex_init_extra(&g_declinfo_extra, &g_yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)g_yyscanner;

#ifdef FLEX_DEBUG
  yyset_debug(1,g_yyscanner);
#endif

  printlex(yy_flex_debug, TRUE, __FILE__, NULL);
  yyextra->inputString   = decl;
  //printf("Input=`%s'\n",yyextra->inputString);
  yyextra->inputPosition      = 0;
  yyextra->classTempListFound = FALSE;
  yyextra->funcTempListFound  = FALSE;
  yyextra->insideObjC = objC;
  yyextra->scope.resize(0);
  yyextra->className.resize(0);
  yyextra->classTempList.resize(0);
  yyextra->funcTempList.resize(0);
  yyextra->name.resize(0);
  yyextra->type.resize(0);
  yyextra->args.resize(0);
  yyextra->exceptionString.resize(0);
  // first we try to find the yyextra->type, yyextra->scope, yyextra->name and arguments
  declinfoYYrestart( yyin, g_yyscanner );
  BEGIN( Start );
  declinfoYYlex(g_yyscanner);

  //printf("yyextra->type=`%s' class=`%s' yyextra->name=`%s' yyextra->args=`%s'\n",
  //        yyextra->type.data(),yyextra->scope.data(),yyextra->name.data(),yyextra->args.data());

  int nb = yyextra->name.findRev('[');
  if (nb!=-1 && yyextra->args.isEmpty()) // correct for [] in yyextra->name ambigity (due to Java return yyextra->type allowing [])
  {
    yyextra->args.prepend(yyextra->name.right(yyextra->name.length()-nb));
    yyextra->name=yyextra->name.left(nb);
  }

#if 0
  {
    int l=yyextra->scope.length();
    int i=0;
    int skipCount=0;
    cl.resize(0);
    ctl.resize(0);
    for (i=0;i<l;i++)
    {
      char c=yyextra->scope.at(i);
      if (c=='<') 
	skipCount++;
      else if (c=='>') 
	skipCount--;
      else if (skipCount==0) 
	cl+=c;
    }
  }
  cl=stripTemplateSpecifiersFromScope(removeRedundantWhiteSpace(yyextra->scope),FALSE); 
  ctl.resize(0);
#endif

  cl=yyextra->scope;
  n=removeRedundantWhiteSpace(yyextra->name);
  int il,ir;
  if ((il=n.find('<'))!=-1 && (ir=n.findRev('>'))!=-1)
    // TODO: handle cases like where n="operator<< <T>" 
  {
    ftl=removeRedundantWhiteSpace(n.right(n.length()-il));
    n=n.left(il);
  }
  
  //ctl=yyextra->classTempList.copy();
  //ftl=yyextra->funcTempList.copy();
  t=removeRedundantWhiteSpace(yyextra->type);
  a=removeRedundantWhiteSpace(yyextra->args);
  exc=removeRedundantWhiteSpace(yyextra->exceptionString);
  
  if (!t.isEmpty() && t.at(t.length()-1)==')') // for function pointers
  {
    a.prepend(")");
    t=t.left(t.length()-1);
  }
  //printf("yyextra->type=`%s' class=`%s' yyextra->name=`%s' yyextra->args=`%s'\n",
  //        t.data(),cl.data(),n.data(),a.data());

  printlex(yy_flex_debug, FALSE, __FILE__, NULL);
  declinfoYYlex_destroy(g_yyscanner);
  return;
}

//extern "C" { // some bogus code to keep the compiler happy
//  int  declinfoYYwrap() { return 1 ; }
//  void declinfoYYdummy() { yy_flex_realloc(0,0); } 
//}

#if 0
void dumpDecl(const char *s)
{
  QCString yyextra->className;
  QCString classTNames;
  QCString yyextra->type;
  QCString yyextra->name;
  QCString yyextra->args;
  QCString funcTNames;
  msg("-----------------------------------------\n");
  parseFuncDecl(s,yyextra->className,classTNames,yyextra->type,yyextra->name,yyextra->args,funcTNames);
  msg("yyextra->type=`%s' class=`%s' classTempl=`%s' yyextra->name=`%s' "
         "funcTemplateNames=`%s' yyextra->args=`%s'\n",
	    yyextra->type.data(),yyextra->className.data(),classTNames.data(),
	    yyextra->name.data(),funcTNames.data(),yyextra->args.data()
	);
}

// some test code
int main()
{
  dumpDecl("A < T > :: Value * A < T > :: getValue < S > ( const A < T > & a )");
  dumpDecl("const A<T>::Value* A<T>::getValue<S>(const A<T>&a)");
  dumpDecl("func()");
  dumpDecl("friend void bla<>()");
  dumpDecl("yyextra->name< T > :: operator () (int bla)");
  dumpDecl("yyextra->name< T > :: operator << (int bla)");
  dumpDecl("yyextra->name< T > :: operator << <> (int bla)");
  dumpDecl("yyextra->className::func()");
  dumpDecl("void ( * yyextra->Name < T > :: bla ) ( int, char * )"); 
}
#endif

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void declinfoYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

