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
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

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
#include "types.h"
#include "debug.h"

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
     int          roundCount;
     bool         classTempListFound;
     bool         funcTempListFound;
     QCString     exceptionString;
     bool         insideObjC;
     bool         insidePHP;
     bool         insidePython;
};

[[maybe_unused]] static const char *stateToString(int state);

static void addType(yyscan_t yyscanner);
static void addTypeName(yyscan_t yyscanner);
static int yyread(char *buf,int max_size, yyscan_t yyscanner);

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size,yyscanner);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#define LEX_NO_INPUT_FILENAME
#include "doxygen_lex.h"
%}

B       [ \t]
Bopt    {B}*
ID	([$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*)|(@[0-9]+)

%x  Start
%x	Template
%x	ReadArgs
%x	Operator
%x	DeclType
%x  ReadExceptions

%%

<Start>"operator"/({B}*"["{B}*"]")* 	{ // operator rule must be before {ID} rule
  				  yyextra->name += yytext;
  				  BEGIN(Operator);
  				}
<Start>{ID}({B}*"*")?{B}*"("{B}*"^"{B}*")"{B}*"(" { // Objective-C 2.0 block return type
  				  if (!yyextra->insideObjC) 
				  {
				    REJECT;
				  }
				  else
				  {
  				    addType(yyscanner);
				    yyextra->type += yytext;
				  }
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
<Start>([~!]{B}*)?{ID}{B}*"["{B}*"]" { // PHP
  				  if (!yyextra->insidePHP)
				  {
				    REJECT;
				  }
  				  addTypeName(yyscanner);
				  yyextra->name += removeRedundantWhiteSpace(yytext);
  				}
<Start>"anonymous_namespace{"[^}]+"}" { // anonymous namespace
 				  if (!yyextra->scope.isEmpty())
				  {
				    yyextra->scope+=QCString("::")+yytext;
				  }
				  else
				  {
  				    yyextra->scope = yytext;
				  }
                                }
<Start>([~!]{B}*)?{ID}/({B}*"["{B}*"]")* { // the []'s are for Java, 
                                        // the / was add to deal with multi-
                                        // dimensional C++ arrays like A[][15]
                                        // the leading ~ is for a destructor
                                        // the leading ! is for a C++/CLI finalizer (see bug 456475 and 635198)
  				  addTypeName(yyscanner);
				  yyextra->name += removeRedundantWhiteSpace(yytext);
  				}
<Start>{B}*"::"{B}*		{ // found a yyextra->scope specifier
 				  if (!yyextra->scope.isEmpty() && !yyextra->scope.endsWith("::"))
				  {
                                    if (!yyextra->name.isEmpty()) yyextra->scope+="::"+yyextra->name; // add yyextra->name to yyextra->scope
				  }
				  else
				  {
  				    yyextra->scope = yyextra->name; // yyextra->scope becomes yyextra->name
				  }
				  yyextra->name.clear();
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
                                  if (yyextra->insidePHP) REJECT;
                                  if (yyextra->insidePython) REJECT;
  				  addType(yyscanner);
				  QCString text(yytext);
				  yyextra->type+=text.stripWhiteSpace();
  				}
<Start>{B}*")"			{
  				  yyextra->type+=")";
  				}
<Start>{B}*"decltype"/{B}*"("	{
					  yyextra->roundCount=0;
					  yyextra->type="decltype";
					  BEGIN(DeclType);
					}
<DeclType>{B}*"("	{
						++yyextra->roundCount;
						yyextra->type+="(";
					}
<DeclType>{B}*")"	{
						yyextra->type+=")";
						if (--yyextra->roundCount == 0) {
							BEGIN(Start);
						}
					}
<DeclType>.	{
						yyextra->type+=yytext;
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
                                  yyextra->roundCount=0;
  				  BEGIN(Template);
  				}
<Template>"<<"			{ yyextra->name+="<<"; }
<Template>">>"			{ yyextra->name+=">>"; }
<Template>"("                   { yyextra->name+="(";
                                  yyextra->roundCount++;
                                }
<Template>")"                   { yyextra->name+=")";
                                  if (yyextra->roundCount>0)
                                  {
                                    yyextra->roundCount--;
                                  }
                                }
<Template>"<"			{
  				  yyextra->name+="<";
                                  if (yyextra->roundCount==0)
                                  {
  				    yyextra->sharpCount++;
                                  }
  				}
<Template>">"			{
  				  yyextra->name+=">";
                                  if (yyextra->roundCount==0)
                                  {
                                    if (yyextra->sharpCount)
                                      --yyextra->sharpCount;
                                    else
                                    {
                                      BEGIN(Start);
                                    }
                                  }
  				}
<Template>.			{
  				  yyextra->name+=*yytext;
  				}
<Operator>{B}*"("{B}*")"{B}*"<>"{Bopt}/"("	{
  				  yyextra->name+="() <>";
				  BEGIN(ReadArgs);
  				}
<Operator>{B}*"("{B}*")"{Bopt}/"("	{
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
  //printf("addType() yyextra->type='%s' yyextra->scope='%s' yyextra->name='%s'\n",
  //       qPrint(yyextra->type),qPrint(yyextra->scope),qPrint(yyextra->name));
  if (yyextra->name.isEmpty() && yyextra->scope.isEmpty()) return;
  if (!yyextra->type.isEmpty()) yyextra->type+=" ";
  if (!yyextra->scope.isEmpty()) yyextra->type+=yyextra->scope+"::";
  yyextra->type+=yyextra->name;
  yyextra->scope.clear();
  yyextra->name.clear();
}

static void addTypeName(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("addTypeName() yyextra->type='%s' yyextra->scope='%s' yyextra->name='%s'\n",
  //       qPrint(yyextra->type),qPrint(yyextra->scope),qPrint(yyextra->name));
  if (yyextra->name.isEmpty() || 
      yyextra->name.at(yyextra->name.length()-1)==':')  // end of Objective-C keyword => append to yyextra->name not yyextra->type
  {
    return;
  }
  if (!yyextra->type.isEmpty()) yyextra->type+=' ';
  yyextra->type+=yyextra->name;
  yyextra->name.clear();
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

void parseFuncDecl(const QCString &decl,const SrcLangExt lang,QCString &cl,QCString &t,
                   QCString &n,QCString &a,QCString &ftl,QCString &exc)
{
  if (decl.isEmpty())
  {
    return;
  }
  declinfoYYlex_init_extra(&g_declinfo_extra, &g_yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)g_yyscanner;

#ifdef FLEX_DEBUG
  declinfoYYset_debug(Debug::isFlagSet(Debug::Lex_declinfo)?1:0,g_yyscanner);
#endif

  DebugLex debugLex(Debug::Lex_declinfo,__FILE__, NULL);
  yyextra->inputString   = decl.data();
  //printf("Input='%s'\n",yyextra->inputString);
  yyextra->inputPosition      = 0;
  yyextra->classTempListFound = FALSE;
  yyextra->funcTempListFound  = FALSE;
  yyextra->insideObjC = lang==SrcLangExt::ObjC;
  yyextra->insidePHP  = lang==SrcLangExt::PHP;
  yyextra->insidePython  = lang==SrcLangExt::Python;
  yyextra->scope.clear();
  yyextra->className.clear();
  yyextra->classTempList.clear();
  yyextra->funcTempList.clear();
  yyextra->name.clear();
  yyextra->type.clear();
  yyextra->args.clear();
  yyextra->exceptionString.clear();
  // first we try to find the yyextra->type, yyextra->scope, yyextra->name and arguments
  declinfoYYrestart( yyin, g_yyscanner );
  BEGIN( Start );
  declinfoYYlex(g_yyscanner);

  //printf("yyextra->type='%s' class='%s' yyextra->name='%s' yyextra->args='%s'\n",
  //        qPrint(yyextra->type),qPrint(yyextra->scope),qPrint(yyextra->name),qPrint(yyextra->args));

  int nb = yyextra->name.findRev('[');
  if (nb!=-1 && yyextra->args.isEmpty()) // correct for [] in yyextra->name ambiguity (due to Java return yyextra->type allowing [])
  {
    yyextra->args.prepend(yyextra->name.right(yyextra->name.length()-nb));
    yyextra->name=yyextra->name.left(nb);
  }

  cl=yyextra->scope;
  n=removeRedundantWhiteSpace(yyextra->name);
  int il=n.find('<'), ir=n.findRev('>');
  if (il!=-1 && ir!=-1 && n.at(il+1)!='=')
    // prevent <=>
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
  
  if (!t.isEmpty() && !t.startsWith("decltype") && t.at(t.length()-1)==')') // for function pointers
  {
    a.prepend(")");
    t=t.left(t.length()-1);
  }
  //printf("yyextra->type='%s' class='%s' yyextra->name='%s' yyextra->args='%s'\n",
  //        qPrint(t),qPrint(cl),qPrint(n),qPrint(a));

  declinfoYYlex_destroy(g_yyscanner);
  return;
}

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
  msg("yyextra->type='{}' class='{}' classTempl='{}' yyextra->name='{}' "
         "funcTemplateNames='{}' yyextra->args='{}'\n",
	    yyextra->type,yyextra->className,classTNames,
	    yyextra->name,funcTNames,yyextra->args
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

#include "declinfo.l.h"
