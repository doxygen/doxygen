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
 *  \verbatim
 *    "(int a,int b) const"
 *    "(const char *s="hello world",int=5) = 0"
 *    "<class T,class N>"
 *    "(char c,const char)"
 *  \endverbatim
 *
 *  Note: It is not always possible to distinguish between the name and 
 *        type of an argument. In case of doubt the name is added to the
 *        type, and the matchArgumentList in util.cpp is be used to
 *        further determine the correct separation.
 */
%option never-interactive
%option prefix="defargsYY"
%option reentrant
%option extra-type="struct defargsYY_state *"
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

#include "defargs.h"
#include "entry.h"
#include "util.h"
#include "arguments.h"
#include "message.h"
#include "debug.h"
  
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

/* -----------------------------------------------------------------
 *	state variables
 */
struct defargsYY_state
{
  defargsYY_state(const char *inStr,std::unique_ptr<ArgumentList> &al,SrcLangExt l)
    : inputString(inStr), argList(al), lang(l) {}
  const char      *inputString;
  std::unique_ptr<ArgumentList> &argList;
  SrcLangExt       lang;
  int              inputPosition = 0;
  QCString        *copyArgValue = nullptr;
  QCString         curArgTypeName;
  QCString         curArgDefValue;
  QCString         curArgName;
  QCString         curArgDocs;
  QCString         curArgAttrib;
  QCString         curArgArray;
  QCString         curTypeConstraint;
  QCString         extraTypeChars;
  int              argRoundCount = 0;
  int              argSquareCount = 0;
  int              argSharpCount = 0;
  int              argCurlyCount = 0;
  int              readArgContext = 0;
  int              lastDocContext = 0;
  int              lastDocChar = 0;
  int              lastExtendsContext = 0;
  QCString         delimiter;
};

[[maybe_unused]] static const char *stateToString(int state);

static int yyread(yyscan_t yyscanner,char *buf,int max_size);
static bool nameIsActuallyPartOfType(QCString &name);

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#define LEX_NO_INPUT_FILENAME
#include "doxygen_lex.h"

%}

B       [ \t]
Bopt    {B}*
ID	[$a-z_A-Z\x80-\xFF][$a-z_A-Z0-9\x80-\xFF]*
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"

  // C start comment 
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment 
CPPC  "/\/"

%option noyywrap

%x      Start
%x      CopyArgString
%x      CopyRawString
%x	CopyArgRound
%x	CopyArgRound2
%x	CopyArgSquare
%x	CopyArgSharp
%x	CopyArgCurly
%x	ReadFuncArgType
%x	ReadFuncArgDef
%x	ReadFuncArgPtr
%x	FuncQual
%x	ReadDocBlock
%x	ReadDocLine
%x      ReadTypeConstraint
%x      TrailingReturn


%%

<Start>[<(]				{ BEGIN(ReadFuncArgType); }

<ReadFuncArgType>{B}*			{
  					  yyextra->curArgTypeName+=" ";
  					}
<ReadFuncArgType>"["[^\]]*"]"		{ 
					  if (yyextra->curArgTypeName.stripWhiteSpace().isEmpty())
					  {
					    yyextra->curArgAttrib=yytext; // for M$-IDL
					  }
					  else // array type
					  {
					    yyextra->curArgArray+=yytext;
					  }
					}
<ReadFuncArgDef>"'"\\[0-7]{1,3}"'"	{ yyextra->curArgDefValue+=yytext; }
<ReadFuncArgDef>"'"\\."'"		{ yyextra->curArgDefValue+=yytext; }
<ReadFuncArgDef>"'"."'"			{ yyextra->curArgDefValue+=yytext; }
<ReadFuncArgDef>{RAWBEGIN}              { yyextra->curArgDefValue+=yytext; 
                                          yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
                                          BEGIN( CopyRawString );
                                        }
<ReadFuncArgDef>\"			{
  					  yyextra->curArgDefValue+=*yytext;
  					  BEGIN( CopyArgString );
  					}
<ReadFuncArgType>"("([^:)]+{B}*"::")*{B}*[&*\^]+{Bopt}/{ID} { 
  					  // function pointer as argument
					  yyextra->curArgTypeName+=yytext;
					  //yyextra->curArgTypeName=yyextra->curArgTypeName.simplifyWhiteSpace();
					  BEGIN( ReadFuncArgPtr );
  					}
<ReadFuncArgPtr>{ID}			{
					  yyextra->curArgName=yytext;
  					}
<ReadFuncArgPtr>")"{B}*"("		{ // function pointer
					  yyextra->curArgTypeName+=yytext;
					  //yyextra->curArgTypeName=yyextra->curArgTypeName.simplifyWhiteSpace();
					  yyextra->readArgContext = ReadFuncArgType;
					  yyextra->copyArgValue=&yyextra->curArgTypeName;
					  yyextra->argRoundCount=0;
					  BEGIN( CopyArgRound2 );
					}
<ReadFuncArgPtr>")"/{B}*"["		{ // pointer to fixed size array
					  yyextra->curArgTypeName+=yytext;
					  yyextra->curArgTypeName+=yyextra->curArgName;
					  //yyextra->curArgTypeName=yyextra->curArgTypeName.simplifyWhiteSpace();
					  BEGIN( ReadFuncArgType );
					}
<ReadFuncArgPtr>")"			{ // redundant braces detected / remove them
					  int i=yyextra->curArgTypeName.findRev('(');
                                          int l=static_cast<int>(yyextra->curArgTypeName.length());
					  if (i!=-1)
                                          {
					    yyextra->curArgTypeName=yyextra->curArgTypeName.left(i)+
					                   yyextra->curArgTypeName.right(l-i-1);
                                          }
					  yyextra->curArgTypeName+=yyextra->curArgName;
					  BEGIN( ReadFuncArgType );
					}
<ReadFuncArgType>"<="|">="|"->"|">>"|"<<" { // handle operators in defargs
  					  yyextra->curArgTypeName+=yytext;
  					}
<ReadFuncArgType,ReadFuncArgDef>[({<\[]	{	 
					  if (YY_START==ReadFuncArgType)
					  {
					    yyextra->curArgTypeName+=*yytext;
					    yyextra->copyArgValue=&yyextra->curArgTypeName;
					  }
					  else // YY_START==ReadFuncArgDef
					  {
					    yyextra->curArgDefValue+=*yytext;
					    yyextra->copyArgValue=&yyextra->curArgDefValue;
					  }
					  yyextra->readArgContext = YY_START; 
					  if (*yytext=='(')
					  {
					    yyextra->argRoundCount=0; 
					    BEGIN( CopyArgRound ); 
					  }
					  else if (*yytext=='[')
					  {
					    yyextra->argSquareCount=0; 
					    BEGIN( CopyArgSquare ); 
					  }
					  else if (*yytext=='{')
					  {
					    yyextra->argCurlyCount=0; 
					    BEGIN( CopyArgCurly ); 
					  }
					  else // yytext=='<'
					  {
					    yyextra->argSharpCount=0; 
					    yyextra->argRoundCount=0; 
					    BEGIN( CopyArgSharp ); 
					  }
					}
<CopyArgRound,CopyArgRound2>"("		{
  					  yyextra->argRoundCount++;
					  *yyextra->copyArgValue += *yytext;
  					}
<CopyArgRound,CopyArgRound2>")"({B}*{ID})* {
					  *yyextra->copyArgValue += yytext;
					  if (yyextra->argRoundCount>0) 
					  {
					    yyextra->argRoundCount--;
					  }
					  else 
					  {
					    if (YY_START==CopyArgRound2)
					    {
					      *yyextra->copyArgValue+=" "+yyextra->curArgName;
					    }
					    BEGIN( yyextra->readArgContext );
					  }
  					}
<CopyArgRound>")"/{B}*                  {
					  *yyextra->copyArgValue += *yytext;
					  if (yyextra->argRoundCount>0) yyextra->argRoundCount--;
					  else BEGIN( yyextra->readArgContext );
                                        }
<CopyArgSquare>"["                      {
                                           yyextra->argSquareCount++;
                                           *yyextra->copyArgValue += *yytext;
                                        }
<CopyArgSquare>"]"({B}*{ID})* {
					  *yyextra->copyArgValue += yytext;
					  if (yyextra->argSquareCount>0) 
					  {
					    yyextra->argRoundCount--;
					  }
					  else 
					  {
					    BEGIN( yyextra->readArgContext );
					  }
  					}
<CopyArgSquare>"]"/{B}*                  {
					  *yyextra->copyArgValue += *yytext;
					  if (yyextra->argSquareCount>0) yyextra->argSquareCount--;
					  else BEGIN( yyextra->readArgContext );
                                        }
<CopyArgSharp>"<<"                      {
                                          if (yyextra->argRoundCount>0)
                                          {
                                            // for e.g. < typename A = (i<<3) >
					    *yyextra->copyArgValue += yytext;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<CopyArgSharp>">>"                      {
                                          if (yyextra->argRoundCount>0)
                                          {
                                            // for e.g. < typename A = (i>>3) >
					    *yyextra->copyArgValue += yytext;
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<CopyArgSharp>"<"			{
                                          // don't count < inside (, e.g. for things like: < typename A=(i<6) >
  					  if (yyextra->argRoundCount==0) yyextra->argSharpCount++;
					  *yyextra->copyArgValue += *yytext;
  					}
<CopyArgSharp>">"			{
					  *yyextra->copyArgValue += *yytext;
                                          if (yyextra->argRoundCount>0 && yyextra->argSharpCount==0)
                                          {
                                            // don't count > inside )
                                          }
                                          else
                                          {
                                            if (yyextra->argSharpCount>0)
                                            {
                                              yyextra->argSharpCount--;
                                            }
                                            else
                                            {
                                              BEGIN( yyextra->readArgContext );
                                            }
                                          }
  					}
<CopyArgSharp>"("                       {
                                          yyextra->argRoundCount++;
					  *yyextra->copyArgValue += *yytext;
                                        }
<CopyArgSharp>")"                       {
                                          yyextra->argRoundCount--;
					  *yyextra->copyArgValue += *yytext;
                                        }
<CopyArgCurly>"{"			{
  					  yyextra->argCurlyCount++;
					  *yyextra->copyArgValue += *yytext;
  					}
<CopyArgCurly>"}"			{
					  *yyextra->copyArgValue += *yytext;
					  if (yyextra->argCurlyCount>0) yyextra->argCurlyCount--;
					  else BEGIN( yyextra->readArgContext );
  					}
<CopyArgString>\\.			{
					  yyextra->curArgDefValue+=yytext;
  					}
<CopyRawString>{RAWEND}                 {
					  yyextra->curArgDefValue+=yytext;
                                          if (extractEndRawStringDelimiter(yytext)==yyextra->delimiter)
                                          {
					    BEGIN( ReadFuncArgDef );
                                          }
                                        }
<CopyArgString>\"			{
					  yyextra->curArgDefValue+=*yytext;
					  BEGIN( ReadFuncArgDef );
  					}
<ReadFuncArgType>"="			{
					  BEGIN( ReadFuncArgDef );
  					}
<ReadFuncArgType,ReadFuncArgDef>[,)>]{B}*({CCS}[*!]|{CPPC}[/!])"<" {
					  yyextra->lastDocContext=YY_START;
					  yyextra->lastDocChar=*yytext;  
					  QCString text(yytext);
					  if (text.find("//")!=-1)
					    BEGIN( ReadDocLine );
					  else
					    BEGIN( ReadDocBlock );
  					}
<ReadFuncArgType,ReadFuncArgDef>[,)>]	{
  					  if (*yytext==')' && yyextra->curArgTypeName.stripWhiteSpace().isEmpty())
					  {
					    yyextra->curArgTypeName+=*yytext;
					    BEGIN(FuncQual);
					  }
					  else
					  {
					    yyextra->curArgTypeName=removeRedundantWhiteSpace(yyextra->curArgTypeName);
					    yyextra->curArgDefValue=yyextra->curArgDefValue.stripWhiteSpace();
					    //printf("curArgType='%s' curArgDefVal='%s'\n",qPrint(yyextra->curArgTypeName),qPrint(yyextra->curArgDefValue));
					    int l = static_cast<int>(yyextra->curArgTypeName.length());
					    if (l>0)
					    {
					      int i=l-1;
					      while (i>=0 && (isspace((uint8_t)yyextra->curArgTypeName.at(i)) || yyextra->curArgTypeName.at(i)=='.')) i--;
					      while (i>=0 && (isId(yyextra->curArgTypeName.at(i)) || yyextra->curArgTypeName.at(i)=='$')) i--;
					      Argument a;
					      a.attrib  = yyextra->curArgAttrib;
                                              a.typeConstraint = yyextra->curTypeConstraint.stripWhiteSpace();
					      //printf("a->type=%s a->name=%s i=%d l=%d\n",
					      //        qPrint(a->type),qPrint(a->name),i,l);
					      a.array.clear();
					      if (i==l-1 && yyextra->curArgTypeName.at(i)==')') // function argument
					      {
						int bi=yyextra->curArgTypeName.find('(');
						int fi=bi-1;
						//printf("func arg fi=%d\n",fi);
						while (fi>=0 && (isId(yyextra->curArgTypeName.at(fi)) || yyextra->curArgTypeName.at(fi)==':')) fi--;
						if (fi>=0)
						{
						  a.type  = yyextra->curArgTypeName.left(fi+1);
						  a.name  = yyextra->curArgTypeName.mid(fi+1,bi-fi-1).stripWhiteSpace();
						  a.array = yyextra->curArgTypeName.right(l-bi);
						}
						else
						{
						  a.type = yyextra->curArgTypeName;
						}
					      }
					      else if (i>=0 && yyextra->curArgTypeName.at(i)!=':')
					      { // type contains a name
						a.type = removeRedundantWhiteSpace(yyextra->curArgTypeName.left(i+1)).stripWhiteSpace();
						a.name = yyextra->curArgTypeName.right(l-i-1).stripWhiteSpace();

						// if the type becomes a type specifier only then we make a mistake
						// and need to correct it to avoid seeing a nameless parameter
						// "struct A" as a parameter with type "struct" and name "A".
						int sv=0;
						if      (a.type.startsWith("const "))    sv=6;
						else if (a.type.startsWith("volatile ")) sv=9;

						if (a.type.mid(sv)=="struct"    ||
						    a.type.mid(sv)=="union"     ||
						    a.type.mid(sv)=="class"     ||
						    a.type.mid(sv)=="typename"  ||
						    a.type=="const"             ||
						    a.type=="volatile"          ||
						    nameIsActuallyPartOfType(a.name)
						   )
						{
						  a.type = a.type + " " + a.name;
						  a.name.clear();
						}
						//printf(" --> a->type='%s' a->name='%s'\n",qPrint(a->type),qPrint(a->name));
					      }
					      else // assume only the type was specified, try to determine name later 
					      {
						a.type = removeRedundantWhiteSpace(yyextra->curArgTypeName);  
					      }
                                              if (!a.type.isEmpty() && a.type.at(0)=='$') // typeless PHP name?
                                              {
                                                a.name = a.type;
                                                a.type = "";
                                              }
					      a.array  += removeRedundantWhiteSpace(yyextra->curArgArray);
					      //printf("array=%s\n",qPrint(a->array));
					      size_t alen = a.array.length();
					      if (alen>2 && a.array.at(0)=='(' && 
						            a.array.at(alen-1)==')') // fix-up for int *(a[10])
					      {
						i=a.array.find('[')-1;
						a.array = a.array.mid(1,alen-2);
						if (i>0 && a.name.isEmpty())
						{
						  a.name  = a.array.left(i).stripWhiteSpace();
						  a.array = a.array.mid(i);
						}
					      }
					      a.defval = yyextra->curArgDefValue;
					      //printf("a->type=%s a->name=%s a->defval=\"%s\"\n",qPrint(a->type),qPrint(a->name),qPrint(a->defval));
					      a.docs   = yyextra->curArgDocs.stripWhiteSpace();
					      //printf("Argument '%s' '%s' adding docs='%s'\n",qPrint(a->type),qPrint(a->name),qPrint(a->docs));
					      yyextra->argList->push_back(a);
					    }
					    yyextra->curArgAttrib.clear();
					    yyextra->curArgTypeName.clear();
					    yyextra->curArgDefValue.clear();
					    yyextra->curArgArray.clear();
					    yyextra->curArgDocs.clear();
                                            yyextra->curTypeConstraint.clear();
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
<ReadFuncArgType,ReadFuncArgPtr>"extends" {
                                           if (yyextra->lang!=SrcLangExt::Java)
                                           {
                                             REJECT;
                                           }
                                           else
                                           {
  					     yyextra->argSharpCount=0;
                                             yyextra->curTypeConstraint.clear();
                                             yyextra->lastExtendsContext=YY_START;
                                             BEGIN(ReadTypeConstraint);
                                           }
                                        }
<ReadFuncArgType,ReadFuncArgPtr>"$"?{ID} { 
  					  QCString name(yytext);
					  if (YY_START==ReadFuncArgType && yyextra->curArgArray=="[]") // Java style array
					  {
					    yyextra->curArgTypeName+=" []";
					    yyextra->curArgArray.clear();
					  }
					  //printf("resolveName '%s'->'%s'\n",yytext,qPrint(name));
  					  yyextra->curArgTypeName+=name;
					}
<ReadFuncArgType,ReadFuncArgPtr>.	{ 
  					  yyextra->curArgTypeName+=*yytext;
					}

<ReadFuncArgDef,CopyArgString>"<="|"->"|">="|">>"|"<<"	{
  					  yyextra->curArgDefValue+=yytext;
  					}
<ReadFuncArgDef,CopyArgString,CopyRawString>.		{
					  yyextra->curArgDefValue+=*yytext;
  					}
<CopyArgRound,CopyArgRound2,CopyArgSquare,CopyArgSharp,CopyArgCurly>{ID}  {
					  *yyextra->copyArgValue+=yytext;
					}
<CopyArgRound,CopyArgRound2,CopyArgSquare,CopyArgSharp,CopyArgCurly>.  {
					  *yyextra->copyArgValue += *yytext;
					}
<ReadTypeConstraint>[,)>]               {
                                          if (yytext[0]!='>' || yyextra->argSharpCount==0)
                                          {
                                            unput(*yytext);
                                            BEGIN(yyextra->lastExtendsContext);
                                          }
                                          else
                                          {
                                            yyextra->curTypeConstraint+=yytext;
  					    yyextra->argSharpCount--;
                                          }
                                        }
<ReadTypeConstraint>"<"                 {
                                          yyextra->curTypeConstraint+=yytext;
  					  yyextra->argSharpCount++;
                                        }
<ReadTypeConstraint>.                   {
                                          yyextra->curTypeConstraint+=yytext;
                                        }
<ReadTypeConstraint>\n                  {
                                          yyextra->curTypeConstraint+=' ';
                                        }
<FuncQual>"const"		       	{
					  yyextra->argList->setConstSpecifier(TRUE);
					}
<FuncQual>"volatile"		    	{
					  yyextra->argList->setVolatileSpecifier(TRUE);
					}
<FuncQual>"override"                    {
                                        }
<FuncQual>"&"		    	        {
                                          yyextra->argList->setRefQualifier(RefQualifierType::LValue);
					}
<FuncQual>"&&"		    	        {
                                          yyextra->argList->setRefQualifier(RefQualifierType::RValue);
					}
<FuncQual,TrailingReturn>"="{B}*"0"  	{
					  yyextra->argList->setPureSpecifier(TRUE);
                                          BEGIN(FuncQual);
					}
<FuncQual>"->"                          { // C++11 trailing return type
                                          yyextra->argList->setTrailingReturnType(QCString(" -> "));
                                          BEGIN(TrailingReturn);
                                        }
<TrailingReturn>{B}/("final"|"override"){B}*  {
                                          unput(*yytext);
                                          BEGIN(FuncQual);
                                        }
<TrailingReturn>.                       {
                                          yyextra->argList->setTrailingReturnType(yyextra->argList->trailingReturnType()+yytext);
                                        }
<TrailingReturn>\n                      {
                                          yyextra->argList->setTrailingReturnType(yyextra->argList->trailingReturnType()+yytext);
                                        }
<FuncQual>")"{B}*"["[^]]*"]"		{ // for functions returning a pointer to an array, 
                                          // i.e. ")[]" in "int (*f(int))[4]" with argsString="(int))[4]"
  					  yyextra->extraTypeChars=yytext;
  					}
<ReadDocBlock>[^\*\n]+			{
  					  yyextra->curArgDocs+=yytext;
  					}
<ReadDocLine>[^\n]+			{
  					  yyextra->curArgDocs+=yytext;
  					}
<ReadDocBlock>{CCE}			{ 
  					  if (yyextra->lastDocChar!=0)
					    unput(yyextra->lastDocChar);
  					  BEGIN(yyextra->lastDocContext); 
					}
<ReadDocLine>\n				{
  					  if (yyextra->lastDocChar!=0)
					    unput(yyextra->lastDocChar);
					  BEGIN(yyextra->lastDocContext);
  					}
<ReadDocBlock>\n			{
  					  yyextra->curArgDocs+=*yytext;
  					}
<ReadDocBlock>.				{
  					  yyextra->curArgDocs+=*yytext;
  					}
<*>({CCS}[*!]|{CPPC}[/!])("<"?)		{
  					  yyextra->lastDocContext=YY_START;
					  yyextra->lastDocChar=0;  
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

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
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

/*
The following code is generated using 'gperf keywords.txt'
where keywords.txt has the following content

---------------------------------
%define class-name KeywordHash
%define lookup-function-name find
%readonly-tables
%language=C++
%%
unsigned
signed
bool
char
char8_t
char16_t
char32_t
wchar_t
int
short
long
float
double
int8_t
int16_t
int32_t
int64_t
intmax_t
intptr_t
uint8_t
uint16_t
uint32_t
uint64_t
uintmax_t
uintptr_t
const
volatile
void
%%
---------------------------------
*/
//--- begin gperf generated code ----------------------------------------------------------

#define TOTAL_KEYWORDS 28
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 48
/* maximum key range = 46, duplicates = 0 */

class KeywordHash
{
  private:
    static inline size_t hash (const char *str, size_t len);
  public:
    static const char *find (const char *str, size_t len);
};

inline size_t KeywordHash::hash (const char *str, size_t len)
{
  static const unsigned char asso_values[] =
  {
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49,  5,
     5, 30,  0, 49, 25, 49, 10, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49,  0, 49,  0,  5, 49,
    15,  0, 49, 10, 49, 30, 49, 49,  0, 20,
     0, 49, 15, 49,  5, 10,  0, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49
  };
  size_t hval = len;

  switch (hval)
  {
    default:
      hval += asso_values[static_cast<unsigned char>(str[4])];
      /*FALLTHROUGH*/
    case 4:
      hval += asso_values[static_cast<unsigned char>(str[3])];
      /*FALLTHROUGH*/
    case 3:
      break;
  }
  return hval;
}

const char * KeywordHash::find (const char *str, size_t len)
{
  static const char * const wordlist[] =
  {
    "", "", "",
    "int",
    "bool",
    "float",
    "signed",
    "",
    "volatile",
    "char",
    "short",
    "double",
    "wchar_t",
    "uint16_t",
    "long",
    "const",
    "int8_t",
    "uint8_t",
    "char16_t",
    "void",
    "", "",
    "char8_t",
    "intptr_t",
    "uintptr_t",
    "", "", "",
    "intmax_t",
    "uintmax_t",
    "", "",
    "int64_t",
    "uint64_t",
    "", "", "",
    "int16_t",
    "uint32_t",
    "", "", "",
    "int32_t",
    "char32_t",
    "", "", "", "",
    "unsigned"
  };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
  {
    size_t key = hash (str, len);

    if (key <= MAX_HASH_VALUE)
    {
      const char *s = wordlist[key];

      if (*str == *s && !qstrcmp (str + 1, s + 1))
        return s;
    }
  }
  return nullptr;
}

//--- end gperf generated code ----------------------------------------------------------

/* bug_520975 */
static bool nameIsActuallyPartOfType(QCString &name)
{
  return KeywordHash::find(name.data(),name.length())!=nullptr;
}

/*! Converts an argument string into an ArgumentList.
 *  \param[in] lang language of the current argument list
 *  \param[in] argsString the list of Arguments.
 *  \param[out] extraTypeChars point to string to which trailing characters 
 *              for complex types are written to
 */
 
std::unique_ptr<ArgumentList> stringToArgumentList(SrcLangExt lang, const QCString &argsString,QCString *extraTypeChars)
{
  std::unique_ptr<ArgumentList> al = std::make_unique<ArgumentList>();
  if (argsString.isEmpty()) return al;

  yyscan_t yyscanner;
  defargsYY_state extra(argsString.data(),al,lang);
  defargsYYlex_init_extra(&extra,&yyscanner);
#ifdef FLEX_DEBUG
  defargsYYset_debug(Debug::isFlagSet(Debug::Lex_defargs)?1:0,yyscanner);
#endif
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DebugLex debugLex(Debug::Lex_defargs, __FILE__, nullptr);

  defargsYYrestart( nullptr, yyscanner );
  BEGIN( Start );
  defargsYYlex(yyscanner);
  if (yyextra->argList->empty())
  {
    yyextra->argList->setNoParameters(TRUE);
  }
  if (extraTypeChars) *extraTypeChars=yyextra->extraTypeChars;
  //printf("stringToArgumentList(%s) result=%s\n",argsString,qPrint(argListToString(*al)));
  defargsYYlex_destroy(yyscanner);
  return al;
}

#include "defargs.l.h"
