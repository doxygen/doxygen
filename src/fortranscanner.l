/* -*- mode: fundamental; indent-tabs-mode: 1; -*- */
/*****************************************************************************
 * Parser for Fortran90 F subset
 *
 * Copyright (C) by Anke Visser
 * based on the work of Dimitri van Heesch.
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

/* Developer notes.
 *
 * - Consider using startScope(), endScope() functions with  module, program, 
 * subroutine or any other scope in fortran program.
 *
 * - Symbol modifiers (attributes) are collected using SymbolModifiers |= operator during
 * substructure parsing. When substructure ends all modifiers are applied to actual
 * entries in applyModifiers() functions.
 * 
 * - How case insensitiveness should be handled in code?
 * On one side we have arg->name and entry->name, on another side modifierMap[name].
 * In entries and arguments case is the same as in code, in modifier map case is lowered and
 * then it is compared to lowered entry/argument names.
 *
 * - Do not like constructs like aa{BS} or {BS}bb. Should try to handle blank space
 * with separate rule?: It seems it is often necessary, because we may parse something like 
 * "functionA" or "MyInterface". So constructs like `(^|[ \t])interface({BS_}{ID})?/[ \t\n]'
 * are desired.
 *
 * - Must track yyLineNr when using REJECT, unput() or similar commands.
 */

%{

#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "qtbc.h"
#include <qarray.h>
#include <qstack.h>
#include <qregexp.h>
#include <unistd.h> 
#include <qfile.h>
#include <qmap.h>
  
#include "fortranscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h" 
#include "fortrancode.h"
#include "pre.h"

#define YY_NEVER_INTERACTIVE 1

enum ScanVar { V_IGNORE, V_VARIABLE, V_PARAMETER};
enum InterfaceType { IF_NONE, IF_SPECIFIC, IF_GENERIC, IF_ABSTRACT };

// {{{ ----- Helper structs -----
//! Holds modifiers (ie attributes) for one symbol (variable, function, etc)
struct SymbolModifiers {
  enum Protection {NONE_P, PUBLIC, PRIVATE};
  enum Direction {NONE_D, IN, OUT, INOUT};

  //!< This is only used with function return value.
  QCString type, returnName;
  Protection protection;
  Direction direction;
  bool optional;
  QCString dimension;
  bool allocatable;
  bool external;
  bool intrinsic;
  bool parameter;
  bool pointer;
  bool target;
  bool save;
  bool deferred;
  bool nonoverridable;
  bool nopass;
  bool pass;
  QCString passVar;

  SymbolModifiers() : type(), returnName(), protection(NONE_P), direction(NONE_D),
    optional(FALSE), dimension(), allocatable(FALSE),
    external(FALSE), intrinsic(FALSE), parameter(FALSE),
    pointer(FALSE), target(FALSE), save(FALSE), deferred(FALSE), nonoverridable(FALSE),
    nopass(FALSE), pass(FALSE), passVar() {}

  SymbolModifiers& operator|=(const SymbolModifiers &mdfs);
  SymbolModifiers& operator|=(QCString mdfrString);
};

//ostream& operator<<(ostream& out, const SymbolModifiers& mdfs);

static const char *directionStrs[] = 
{
   "", "intent(in)", "intent(out)", "intent(inout)"
};

// }}}

/* -----------------------------------------------------------------
 *
 *	statics
 */
static ParserInterface *g_thisParser; 
static const char *     inputString;
static int		inputPosition;
static bool             isFixedForm;
static QCString         inputStringPrepass; ///< Input string for prepass of line cont. '&'
static QCString         inputStringSemi; ///< Input string after command separetor ';'
static unsigned int     inputPositionPrepass;
static int              lineCountPrepass = 0;

struct CommentInPrepass {
  int column;
  QCString str;
  CommentInPrepass(int column, QCString str) : column(column), str(str) {}
};
static QList<CommentInPrepass>  comments;

#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;

static QFile            inputFile;
static QCString		yyFileName;
static int		yyLineNr     = 1 ;
static int		yyColNr     = 0 ;
static Entry*		current_root = 0 ;
static Entry*		global_root  = 0 ;
static Entry*		file_root    = 0 ;
static Entry*		current      = 0 ;
static Entry*		last_entry   = 0 ;
static ScanVar          v_type       = V_IGNORE; // type of parsed variable
static QList<Entry>     moduleProcedures; // list of all interfaces which contain unresolved 
                                          // module procedures
static QCString         docBlock;
static QCString         docBlockName;
static bool             docBlockInBody;
static bool             docBlockJavaStyle;

static MethodTypes 	mtype;
static bool    		gstat;
static Specifier 	virt;

static QCString          debugStr;
static QCString          result; // function result
static Argument          *parameter; // element of parameter list
static QCString          argType;  // fortran type of an argument of a parameter list
static QCString          argName;  // last identifier name in variable list
static QCString          initializer;  // initial value of a variable
static int               initializerArrayScope;  // number if nested array scopes in initializer
static int               initializerScope;  // number if nested function calls in initializer
static QCString          useModuleName;  // name of module in the use statement
static Protection        defaultProtection;
static InterfaceType     ifType = IF_NONE;

static char              stringStartSymbol; // single or double quote

//! Accumulated modifiers of current statement, eg variable declaration.
static SymbolModifiers currentModifiers;
//! Holds program scope->symbol name->symbol modifiers.
static QMap<Entry*,QMap<QCString,SymbolModifiers> > modifiers;

//-----------------------------------------------------------------------------

static int yyread(char *buf,int max_size);
static void startCommentBlock(bool);
static void handleCommentBlock(const QCString &doc,bool brief);
static void addCurrentEntry();
static void addModule(const char *name, bool isModule=FALSE);
static void addSubprogram(const char *text);
static void addInterface(QCString name, InterfaceType type);
static Argument *getParameter(const QCString &name);
static void scanner_abort();

static void startScope(Entry *scope);
static bool endScope(Entry *scope, bool isGlobalRoot=FALSE);
static QCString getFullName(Entry *e);
static bool isTypeName(QCString name);
static void resolveModuleProcedures(QList<Entry> &moduleProcedures, Entry *current_root);
static int getAmpersandAtTheStart(const char *buf, int length);
static int getAmpOrExclAtTheEnd(const char *buf, int length);
static void truncatePrepass(int index);
static void pushBuffer(QCString &buffer);
static void popBuffer();
//static void extractPrefix(QCString& text);
static QCString extractFromParens(const QCString name);
static CommentInPrepass* locatePrepassComment(int from, int to);
static void updateVariablePrepassComment(int from, int to);
static void newLine();

//-----------------------------------------------------------------------------
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);
#define YY_USER_ACTION yyColNr+=yyleng;
//-----------------------------------------------------------------------------

%}

 //-----------------------------------------------------------------------------
 //-----------------------------------------------------------------------------
IDSYM	  [a-z_A-Z0-9]
NOTIDSYM  [^a-z_A-Z0-9]
SEPARATE  [:, \t]
ID        [a-z_A-Z%]+{IDSYM}*
PP_ID     {ID}
LABELID   [a-z_A-Z]+[a-z_A-Z0-9\-]*
SUBPROG   (subroutine|function)
B         [ \t]
BS        [ \t]*
BS_       [ \t]+
COMMA     {BS},{BS}
ARGS_L0   ("("[^)]*")")
ARGS_L1a  [^()]*"("[^)]*")"[^)]*
ARGS_L1   ("("{ARGS_L1a}*")")
ARGS_L2   "("({ARGS_L0}|[^()]|{ARGS_L1a}|{ARGS_L1})*")"
ARGS      {BS}({ARGS_L0}|{ARGS_L1}|{ARGS_L2})
NOARGS    {BS}"\n"

NUM_TYPE  (complex|integer|logical|real)
KIND      {ARGS}
CHAR      (CHARACTER{ARGS}?|CHARACTER{BS}"*"({BS}[0-9]+|{ARGS}))
TYPE_SPEC (({NUM_TYPE}({BS}"*"{BS}[0-9]+)?)|({NUM_TYPE}{KIND})|DOUBLE{BS_}PRECISION|{CHAR}|TYPE{ARGS}|CLASS{ARGS}|PROCEDURE{ARGS}?)

INTENT_SPEC intent{BS}"("{BS}(in|out|in{BS}out){BS}")"
ATTR_SPEC (ALLOCATABLE|DIMENSION{ARGS}|EXTERNAL|{INTENT_SPEC}|INTRINSIC|OPTIONAL|PARAMETER|POINTER|PRIVATE|PUBLIC|SAVE|TARGET|NOPASS|PASS{ARGS}?|DEFERRED|NON_OVERRIDABLE)
ACCESS_SPEC (PRIVATE|PUBLIC)
LANGUAGE_BIND_SPEC BIND{BS}"("{BS}C{BS}(,{BS}NAME{BS}"="{BS}"\""(.*)"\""{BS})?")"
/* Assume that attribute statements are almost the same as attributes. */
ATTR_STMT {ATTR_SPEC}|DIMENSION|{ACCESS_SPEC}

CONTAINS  CONTAINS
PREFIX    (RECURSIVE{BS_}|PURE{BS_}|ELEMENTAL{BS_}){0,2}(RECURSIVE|PURE|ELEMENTAL)?

%option noyywrap
%option stack
%option caseless
/*%option debug */

 //---------------------------------------------------------------------------------

 /** fortran parsing states */
%x	Subprog
%x	SubprogPrefix
%x	Parameterlist
%x	SubprogBody
%x	SubprogBodyContains
%x	Start
%x	Comment
%x      Module
%x      Program
%x      ModuleBody
%x	ModuleBodyContains
%x	AttributeList
%x      Variable
%x      Initialization
%x      ArrayInitializer
%x      Typedef
%x      TypedefBody
%x      TypedefBodyContains
%x      InterfaceBody
%x      StrIgnore
%x      String
%x      Use
%x      UseOnly
%x      ModuleProcedure

%x      Prepass

 /** comment parsing states */
%x	DocBlock
%x	DocBackLine
%x	EndDoc

%%

 /*-----------------------------------------------------------------------------------*/

<*>^.*\n                                { // prepass: look for line continuations

                                          //fprintf(stderr, "---%s", yytext);

                                            int indexStart = getAmpersandAtTheStart(yytext, yyleng);                              
                                            int indexEnd = getAmpOrExclAtTheEnd(yytext, yyleng);
					    if (indexEnd>=0 && yytext[indexEnd]!='&') //we are only interested in amp
					      indexEnd=-1;

                                            if(indexEnd<0){ // ----- no ampersand as line continuation
                                               if(YY_START == Prepass) { // last line in "continuation"

                                                 // Only take input after initial ampersand
                                                 inputStringPrepass+=(const char*)(yytext+(indexStart+1));
   
						 //printf("BUFFER:%s\n", (const char*)inputStringPrepass);
                                                 pushBuffer(inputStringPrepass);
						 yyColNr = 0;						 
                                                 yy_pop_state();
                                               } else { // simple line
					         yyColNr = 0;
                                                 REJECT;
                                               }

                                            } else { // ----- line with continuation
                                              if(YY_START != Prepass) {
                                                comments.setAutoDelete(TRUE);
						comments.clear();
                                                yy_push_state(Prepass);
                                              }

                                              int length = inputStringPrepass.length();

                                              // Only take input after initial ampersand
                                              inputStringPrepass+=(const char*)(yytext+(indexStart+1));
                                              lineCountPrepass ++;

                                              // cut off & and remove following comment if present
					      truncatePrepass(length+indexEnd-(indexStart+1));
                                            }

                                        }


 /*------ ignore strings that are not initialization strings */ 
<*>"\\\\"				{ if (yy_top_state() == Initialization
					      || yy_top_state() == ArrayInitializer)
					    initializer+=yytext;
					}
<*>"\\\""|\\\'                          { if (yy_top_state() == Initialization
					      || yy_top_state() == ArrayInitializer)
					    initializer+=yytext;
					}
<String>\"|\'                           { // string ends with next quote without previous backspace
                                          if (yytext[0]!=stringStartSymbol) { yyColNr -= yyleng; REJECT; } // single vs double quote
					  if (yy_top_state() == Initialization
					      || yy_top_state() == ArrayInitializer)
					    initializer+=yytext;
                                          yy_pop_state();
                                        }           
<String>.                               { if (yy_top_state() == Initialization
					      || yy_top_state() == ArrayInitializer)
					    initializer+=yytext;
					} 
<*>\"|\'                                { /* string starts */
					  if (YY_START == StrIgnore) { yyColNr -= yyleng; REJECT; }; // ignore in simple comments
                                          yy_push_state(YY_START);
                                          if (yy_top_state() == Initialization
					      || yy_top_state() == ArrayInitializer)
					    initializer+=yytext;
                                          stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String);
                                        }

 /*------ ignore simple comment (not documentation comments) */

<*>"!"/[^<>\n]                         {  if (YY_START == String) { yyColNr -= yyleng; REJECT; } // "!" is ignored in strings
                                          // skip comment line (without docu comments "!>" "!<" ) 
                                          /* ignore further "!" and ignore comments in Strings */
                                          if ((YY_START != StrIgnore) && (YY_START != String)) 
					  {
                                            yy_push_state(YY_START);
                                            BEGIN(StrIgnore); 
                                            debugStr="*!";
                                            //fprintf(stderr,"start comment %d\n",yyLineNr);
                                           }      
                                        }
<StrIgnore>.?/\n                        { yy_pop_state(); // comment ends with endline character
                                          //fprintf(stderr,"end comment %d %s\n",yyLineNr,debugStr.data());
                                        } // comment line ends
<StrIgnore>.                            { debugStr+=yytext; } 


 /*------ use handling ------------------------------------------------------------*/

<Start,ModuleBody,SubprogBody>"use"{BS_} {
                                          if(YY_START == Start)
                                          {
                                            addModule(NULL); 
                                            yy_push_state(ModuleBody); //anon program
                                          }
                                          yy_push_state(Use);
                                        }
<Use>{ID}                               { 
                                          //fprintf(stderr,"using dir %s\n",yytext);
                                          current->name=yytext;
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDIR_SEC;
					  current_root->addSubEntry(current);
					  current = new Entry;
                                          current->lang    = SrcLangExt_F90; 
                                          yy_pop_state();
                                        }
<Use>{ID}/,                             { 
                                          useModuleName=yytext;
                                        }
<Use>,{BS}"ONLY"                        { BEGIN(UseOnly); 
                                        }           
<UseOnly>{BS},{BS}                      {}
<UseOnly>{ID}                           {
  					  current->name= useModuleName+"::"+yytext;
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDECL_SEC;
					  current_root->addSubEntry(current);
					  current = new Entry ;
                                          current->lang    = SrcLangExt_F90; 
  					}
<Use,UseOnly>"\n"                       {
					  yyColNr -= 1;
                                          unput(*yytext);
                                          yy_pop_state();
                                        }

 /* INTERFACE definitions */
<Start,ModuleBody,SubprogBody>{
^{BS}interface                          { ifType = IF_SPECIFIC;
                                          yy_push_state(InterfaceBody);
                                          // do not start a scope here, every
                                          // interface body is a scope of its own
                                        }

^{BS}abstract{BS_}interface             { ifType = IF_ABSTRACT;
                                          yy_push_state(InterfaceBody);
                                          // do not start a scope here, every
                                          // interface body is a scope of its own
                                        }

^{BS}interface{BS_}{ID}{ARGS}?          { ifType = IF_GENERIC;
                                          yy_push_state(InterfaceBody);

                                          // extract generic name
                                          QCString name = QCString(yytext).stripWhiteSpace();
                                          name = name.right(name.length() - 9).stripWhiteSpace();
                                          addInterface(name, ifType);

                                          startScope(last_entry);
                                        }
}

<InterfaceBody>^{BS}end{BS}interface({BS_}{ID})? {
                                          // end scope only if GENERIC interface
                                          if (ifType == IF_GENERIC && !endScope(current_root))
                                            yyterminate();

                                          ifType = IF_NONE;
                                          yy_pop_state();
                                        }
<InterfaceBody>module{BS}procedure      { yy_push_state(YY_START);
                                          BEGIN(ModuleProcedure);
                                        }
<ModuleProcedure>{ID}                   { if (ifType == IF_ABSTRACT || ifType == IF_SPECIFIC)
                                          {
                                            addInterface(yytext, ifType);
                                            startScope(last_entry);
                                          }

                                          current->section = Entry::FUNCTION_SEC ;
                                          current->name = yytext; 
                                          moduleProcedures.append(current);
                                          addCurrentEntry();
                                        }
<ModuleProcedure>"\n"                   { yyColNr -= 1;
					  unput(*yytext); 
                                          yy_pop_state();
                                        }
<InterfaceBody>.                        {}

 /*-- Contains handling --*/
<Start>^{BS}{CONTAINS}/({BS}|\n|!)      {
                                          if(YY_START == Start)
                                          {
                                            addModule(NULL); 
                                            yy_push_state(ModuleBodyContains); //anon program
                                          }                                            
                                        }
<ModuleBody>^{BS}{CONTAINS}/({BS}|\n|!)   { BEGIN(ModuleBodyContains); }
<SubprogBody>^{BS}{CONTAINS}/({BS}|\n|!)  { BEGIN(SubprogBodyContains); }
<TypedefBody>^{BS}{CONTAINS}/({BS}|\n|!)  { BEGIN(TypedefBodyContains); }

 /*------ module handling ------------------------------------------------------------*/ 
<Start>module|program{BS_}             {  //
					    if(yytext[0]=='m' || yytext[0]=='M')
					      yy_push_state(Module);
					    else
					      yy_push_state(Program);
                                            defaultProtection = Public;
                                       }
<Start,ModuleBody,ModuleBodyContains>^{BS}"end"({BS}(module|program)({BS_}{ID})?)?{BS}/(\n|!) { // end module	
					    resolveModuleProcedures(moduleProcedures, current_root);
					    if (!endScope(current_root))
					      yyterminate();
                                            defaultProtection = Public;
					    yy_pop_state();
                                       }
<Module>{ID}                           {  
                                            addModule(yytext, TRUE);
					    BEGIN(ModuleBody);
                                       }

<Program>{ID}                           {  
                                            addModule(yytext, FALSE);
					    BEGIN(ModuleBody);
                                       }

  /*------- access specification --------------------------------------------------------------------------*/

<ModuleBody>private/{BS}(\n|"!")         { defaultProtection = Private; }
<ModuleBody>public/{BS}(\n|"!")          { defaultProtection = Public; }

 /*------- type definition  -------------------------------------------------------------------------------*/

<Start,ModuleBody>^{BS}type             {
                                          if(YY_START == Start)
                                          {
                                            addModule(NULL); 
                                            yy_push_state(ModuleBody); //anon program
                                          }

                                          yy_push_state(Typedef);
                                          current->protection = defaultProtection;
                                        }
<Typedef>{
{COMMA}                                 {}

{BS}"::"{BS}                            {}

abstract                                {
                                          current->spec |= Entry::AbstractClass;
                                        }
extends{ARGS}                           {
                                          QCString basename = extractFromParens(yytext);
                                          current->extends->append(new BaseInfo(basename, Public, Normal));
                                        }
public                                  {
                                          current->protection = Public;
                                        }
private                                 {
                                          current->protection = Private;
                                        }
{LANGUAGE_BIND_SPEC}                    {
                                          /* ignored for now */
                                        }
{ID}                                    { /* type name found */
                                          current->section = Entry::CLASS_SEC;
                                          current->spec |= Entry::Struct;
                                          current->name = yytext;
                                          current->fileName = yyFileName;
                                          current->bodyLine  = yyLineNr; 

                                          /* if type is part of a module, mod name is necessary for output */
                                          if ((current_root) && 
                                              (current_root->section == Entry::CLASS_SEC
                                               || current_root->section == Entry::NAMESPACE_SEC))
                                          {
                                            current->name = current_root->name + "::" + current->name;
                                          }

                                          addCurrentEntry();
                                          startScope(last_entry); 
                                          BEGIN(TypedefBody);
                                        }
}

<TypedefBodyContains>{                  /* Type Bound Procedures */
^{BS}PROCEDURE{ARGS}?                   {
                                          current->type = QCString(yytext).simplifyWhiteSpace();
                                        }
^{BS}final                              {
                                          current->spec |= Entry::Final;
                                          current->type = QCString(yytext).simplifyWhiteSpace();
                                        }
^{BS}generic                            {
                                          current->type = QCString(yytext).simplifyWhiteSpace();
                                        }
{COMMA}                                 {
                                        }
{ATTR_SPEC}                             {
                                          currentModifiers |= QCString(yytext);
                                        }
{BS}"::"{BS}                            {
                                        }
{ID}                                    {
                                          QCString name = yytext;
                                          modifiers[current_root][name.lower()] |= currentModifiers;
                                          current->section  = Entry::FUNCTION_SEC;
                                          current->name     = name;
                                          current->fileName = yyFileName;
                                          current->bodyLine = yyLineNr;
                                          addCurrentEntry();
                                        }
{BS}"=>"[^(\n|\!)]*                     { /* Specific bindings come after the ID. */
                                          last_entry->args = yytext;
                                        }
"\n"                                    {
                                          currentModifiers = SymbolModifiers();
					  newLine();
                                          docBlock.resize(0);
                                        }
}


<TypedefBody,TypedefBodyContains>{
^{BS}"end"{BS}"type"({BS_}{ID})?{BS}/(\n|!) { /* end type definition */
                                          if (!endScope(current_root))
                                            yyterminate();
                                          yy_pop_state();
                                        }
}

 /*------- module/global/typedef variable ---------------------------------------------------*/

<SubprogBody,SubprogBodyContains>^{BS}[0-9]*{BS}"end"({BS}{SUBPROG}({BS_}{ID})?)?{BS}/(\n|!) {  
                                           //
                                           // ABSTRACT and specific interfaces are stored
                                           // in a scope of their own, even if multiple
                                           // are group in one INTERFACE/END INTERFACE block.
                                           //
                                           if (ifType == IF_ABSTRACT || ifType == IF_SPECIFIC)
                                             endScope(current_root);

					   if (!endScope(current_root))
					     yyterminate();
					   yy_pop_state() ;
				        }
<Start,ModuleBody,TypedefBody,SubprogBody>{
^{BS}{TYPE_SPEC}/{SEPARATE}             {
                                          /* variable declaration starts */
                                          if(YY_START == Start)
                                          {
                                            addModule(NULL); 
                                            yy_push_state(ModuleBody); //anon program
                                          }
					  argType = QCString(yytext).simplifyWhiteSpace();
					  yy_push_state(AttributeList);
                                        }
^{BS}{PP_ID}{KIND}?			{ /* check for preprocessor symbol expand to type */
					  QCString str = yytext;
					  str = str.stripWhiteSpace();
					  DefineDict* defines = getGlobalDefineDict();
					  QCString name;
					  int index = str.find("(");
					  if (index != -1)
					    name = str.left(index).stripWhiteSpace();
					  else
					    name = str;

					  Define *define = (*defines)[name];
					  if (define != 0 && isTypeName(define->definition)) 
					  {
					    argType = str;
					    yy_push_state(AttributeList);
					  } 
					  else 
					  {
					    yyColNr -= yyleng;
					    REJECT;
					  }
					}
{ATTR_STMT}/{BS_}{ID}		       |
{ATTR_STMT}/{BS}"::"                   { 
                                          /* attribute statement starts */
                                          //fprintf(stderr,"5=========> Attribute statement: %s\n", yytext); 
                                          QCString tmp = yytext;
                                          currentModifiers |= tmp.stripWhiteSpace();
					  argType="";
					  yy_push_state(YY_START);
					  BEGIN( AttributeList ) ;
  				       }
{ID}	                               {
                                       }  
}
<AttributeList>{
{COMMA}					{}
{BS}					{}
{ATTR_SPEC}.				{ /* update current modifierswhen it is an ATTR_SPEC and not a variable name */
            				  /* bug_625519 */
                                          QChar chr = yytext[yyleng-1];
                                          if (chr.isLetter() || chr.isDigit() || (chr == '_'))
                                          {
					    yyColNr -= yyleng;
                                            REJECT;
                                          }
                                          else
                                          {
                                            QCString tmp = yytext;
                                            tmp = tmp.left(tmp.length() - 1);
					    yyColNr -= 1;
                                            unput(yytext[yyleng-1]);
                                            currentModifiers |= (tmp);
                                          }
					}
"::"					{ /* end attribute list */
					  BEGIN( Variable );
					}
.					{ /* unknown attribute, consider variable name */
					  //cout<<"start variables, unput "<<*yytext<<endl;
					  yyColNr -= 1;
					  unput(*yytext);
					  BEGIN( Variable );
					}
}

<Variable>{BS}				{ }
<Variable>{ID}				{ /* parse variable declaration */
                                          //cout << "5=========> got variable: " << argType << "::" << yytext << endl;
 					  /* work around for bug in QCString.replace (QCString works) */
					  QCString name=yytext;
					  /* remember attributes for the symbol */
					  modifiers[current_root][name.lower()] |= currentModifiers;
					  argName= name;

					  v_type= V_IGNORE;
  					  if (!argType.isEmpty() && current_root->section!=Entry::FUNCTION_SEC) 
					  { // new variable entry
					    v_type = V_VARIABLE;
                                            current->section = Entry::VARIABLE_SEC;
 					    current->name = argName;
					    current->type = argType;
					    current->fileName = yyFileName;
					    current->bodyLine  = yyLineNr; // used for source reference
                                            addCurrentEntry();
                                          } 
					  else if (!argType.isEmpty())
					  { // deklaration of parameter list: add type for corr. parameter 
					    parameter = getParameter(argName);
					    if (parameter) {
					      v_type= V_PARAMETER;
      					      if (!argType.isNull())
        				        parameter->type=argType.stripWhiteSpace();
					      if (!docBlock.isNull()) {
					        parameter->docs += "\n";
					        parameter->docs += docBlock;
      					      }					    
					    }
					    // save, it may be function return type
					    modifiers[current_root][name.lower()].type = argType;
					    // any accumulated doc for argument should be emptied,
					    // because it is handled other way and this doc can be
					    // unexpectedly passed to the next member.
					    current->doc.resize(0);
					    current->brief.resize(0);
                                          } 
  					}
<Variable>{ARGS}			{ /* dimension of the previous entry. */
					  QCString name(argName);
					  QCString attr("dimension");
					  attr += yytext;
					  modifiers[current_root][name] |= attr;
					}
<Variable>{COMMA}	                { //printf("COMMA: %d<=..<=%d\n", yyColNr-yyleng, yyColNr);
					  // locate !< comment
					  updateVariablePrepassComment(yyColNr-yyleng, yyColNr);
					}
<Variable>{BS}"="                       { yy_push_state(YY_START);
                                          initializer="";
                                          initializerScope = initializerArrayScope = 0;
					  BEGIN(Initialization);
                                        }
<Variable>"\n"	                        { currentModifiers = SymbolModifiers();
                                          yy_pop_state(); // end variable deklaration list
					  newLine();
                                          docBlock.resize(0);
                                        }
<Variable>";".*"\n"                     { currentModifiers = SymbolModifiers();
                                          yy_pop_state(); // end variable deklaration list
                                          docBlock.resize(0);
                                          inputStringSemi =(const char*)(yytext+1);
                                          pushBuffer(inputStringSemi);
                                        }

<Initialization,ArrayInitializer>"(/"   { initializer+=yytext;
                                           initializerArrayScope++;
                                           BEGIN(ArrayInitializer); // initializer may contain comma
                                        }
<ArrayInitializer>"/)"                   { initializer+=yytext;
                                           initializerArrayScope--;
                                           if(initializerArrayScope<=0)
                                           {
                                              initializerArrayScope = 0; // just in case
                                              BEGIN(Initialization);
                                           }
                                        }
<ArrayInitializer>.                     { initializer+=yytext; }
<Initialization>"("			{ initializerScope++;
					  initializer+=yytext;
					}
<Initialization>")"			{ initializerScope--;
					  initializer+=yytext;
					}
<Initialization>{COMMA}                 { if (initializerScope == 0)
					  {
					    updateVariablePrepassComment(yyColNr-yyleng, yyColNr);
					    yy_pop_state(); // end initialization
                                            if (v_type == V_VARIABLE) last_entry->initializer= initializer;
					  }
					  else
					    initializer+=", ";
					}
<Initialization>"\n"|"!"                { //|
                                          yy_pop_state(); // end initialization
                                          if (v_type == V_VARIABLE) last_entry->initializer= initializer;
					  yyColNr -= 1;
 					  unput(*yytext);
                                        }
<Initialization>.                       { initializer+=yytext; }
 
 /*------ fortran subroutine/function handling ------------------------------------------------------------*/
 /*       Start is initial condition                                                                       */
 
<Start,ModuleBody,SubprogBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains>^{BS}({PREFIX}{BS_})?{TYPE_SPEC}{BS}/{SUBPROG}{BS_} {
                                         if (ifType == IF_ABSTRACT || ifType == IF_SPECIFIC)
                                         {
                                           addInterface("$interface$", ifType);
                                           startScope(last_entry);
                                         }

                                         // TYPE_SPEC is for old function style function result
                                         result = QCString(yytext).stripWhiteSpace();
                                         current->type = result;
                                         yy_push_state(SubprogPrefix);
                                       }

<SubprogPrefix>{BS}{SUBPROG}{BS_}     {
                                         // Fortran subroutine or function found
                                         addSubprogram(yytext);
                                         BEGIN(Subprog);
                                       }

<Start,ModuleBody,SubprogBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains>^{BS}({PREFIX}{BS_})?{SUBPROG}{BS_} {
                                         // Fortran subroutine or function found
                                         if (ifType == IF_ABSTRACT || ifType == IF_SPECIFIC)
                                         {
                                           addInterface("$interface$", ifType);
                                           startScope(last_entry);
                                         }

                                         result = QCString(yytext).stripWhiteSpace();
                                         addSubprogram(result);
                                         yy_push_state(Subprog);
                                       }

<Subprog>{BS}                          {   /* ignore white space */   }
<Subprog>{ID}                          { current->name = yytext;
                                         //cout << "1a==========> got " << current->type << " " << yytext << " " << yyLineNr << endl;
                                         modifiers[current_root][current->name.lower()].returnName = current->name;

                                         if (ifType == IF_ABSTRACT || ifType == IF_SPECIFIC)
                                         {
                                           current_root->name.replace(QRegExp("\\$interface\\$"), yytext);
                                         }

                                         BEGIN(Parameterlist);
                                       }
<Parameterlist>"("                     { current->args = "("; }
<Parameterlist>")"                     {
				         current->args += ")";
				         current->args = removeRedundantWhiteSpace(current->args);
                                         addCurrentEntry();
					 startScope(last_entry);
					 BEGIN(SubprogBody);
				       }
<Parameterlist>{COMMA}|{BS}            { current->args += yytext;
					 CommentInPrepass *c = locatePrepassComment(yyColNr-yyleng, yyColNr);
					 if (c!=NULL) {
                                           if(current->argList->count()>0) {
					     current->argList->at(current->argList->count()-1)->docs = c->str;
					   }
					 }		         
				       }
<Parameterlist>{ID}                    {
				           //current->type not yet available
                                           QCString param = yytext;
					   // std::cout << "3=========> got parameter " << param << std::endl;
					   current->args += param;
					   Argument *arg = new Argument;
					   arg->name = param;
					   arg->type = "";
					   current->argList->append(arg);
 				       } 
<Parameterlist>{NOARGS}                {   
				           newLine();
					   //printf("3=========> without parameterlist \n");
 					   //current->argList = ;
                                           addCurrentEntry();
					   startScope(last_entry);
					   BEGIN(SubprogBody);				           
}
<SubprogBody>result{BS}\({BS}{ID}      {  
                                           result= yytext;
                                           result= result.right(result.length()-result.find("(")-1);
                                           result= result.stripWhiteSpace();
 					   modifiers[current_root->parent()][current_root->name.lower()].returnName = result;
                                           //cout << "=====> got result " <<  result << endl;
 				       } 

 /*---- documentation comments --------------------------------------------------------------------*/

<Variable,SubprogBody,ModuleBody,TypedefBody>"!<"  { /* backward docu comment (only one line) */
                                          if (v_type != V_IGNORE) {
					   current->docLine  = yyLineNr;
 					   docBlockJavaStyle = FALSE;
					   docBlock.resize(0);
					   docBlockJavaStyle = Config_getBool("JAVADOC_AUTOBRIEF");
					   startCommentBlock(TRUE);
                                           yy_push_state(DocBackLine);
                                          }
  					 }
<DocBackLine>.*    			 { // contents of current comment line
                                          docBlock=yytext;
					  if (v_type == V_VARIABLE) 
					  {
					    Entry *tmp_entry = current; 
					    current = last_entry; // temporarily switch to the previous entry
  					    handleCommentBlock(docBlock,TRUE);
 					    current=tmp_entry;
                                          }
					  else if (v_type == V_PARAMETER) 
					  {
					    parameter->docs+=docBlock;
                                          }
                                          yy_pop_state();
 					 }
   
<Start,SubprogBody,ModuleBody,TypedefBody,InterfaceBody,ModuleBodyContains,SubprogBodyContains,TypedefBodyContains>"!>"  {
                                          yy_push_state(YY_START);
					  current->docLine  = yyLineNr;
 					  docBlockJavaStyle = FALSE;
					  docBlock.resize(0);
					  docBlockJavaStyle = Config_getBool("JAVADOC_AUTOBRIEF");
					  startCommentBlock(TRUE);
					  BEGIN(DocBlock);
                                          //cout << "start DocBlock " << endl;
					}

<DocBlock>.*    			{ // contents of current comment line
                                          docBlock+=yytext;
  					}
<DocBlock>"\n"{BS}"!"(">"|"!"+)		{ // comment block (next line is also comment line)
					  docBlock+="\n"; // \n is necessary for lists
					  newLine();
  					}
<DocBlock>"\n"        			{ // comment block ends at the end of this line
                                          //cout <<"3=========> comment block : "<< docBlock << endl;
					  yyColNr -= 1;
					  unput(*yytext);                                        
 					  handleCommentBlock(docBlock,TRUE);
                                          yy_pop_state();                                          
  					}

 /*------------------------------------------------------------------------------------------------*/

<*>"\n"                                 {
					  newLine();
                                          //if (debugStr.stripWhiteSpace().length() > 0) cout << "ignored text: " << debugStr << " state: " <<YY_START << endl;
                                          debugStr="";
                                        }


 /*---- error: EOF in wrong state --------------------------------------------------------------------*/

<*><<EOF>>                              {
                                          if ( include_stack_ptr <= 0 ) {
                                            if (YY_START!=INITIAL && YY_START!=Start) {
                                              //fprintf(stderr,"==== Error: EOF reached in wrong state (end missing)");
                                              scanner_abort();
                                            }
                                            yyterminate();
                                          } else {
                                            popBuffer();
                                          }
                                        }
 <*>.                 	                {
                                          //debugStr+=yytext;
					  //printf("I:%c\n", *yytext);
                                        } // ignore remaining text 

 /**********************************************************************************/
 /**********************************************************************************/
 /**********************************************************************************/
%%
//----------------------------------------------------------------------------

#if 0
static void extractPrefix(QCString &text) 
{
  int prefixIndex = 0;
  int curIndex = 0;
  bool cont = TRUE;
  const char* pre[] = {"RECURSIVE","PURE","ELEMENTAL"};
  while(cont)
  {
    cont = FALSE;
    for(unsigned int i=0; i<3; i++)
    {
      if((prefixIndex=text.find(pre[i], curIndex, FALSE))==0)
      {
        text.remove(0,strlen(pre[i]));
        text.stripWhiteSpace();
        cont = TRUE;
      }
    }
  }
}
#endif

static void newLine() {
  yyLineNr++;
  yyLineNr+=lineCountPrepass;
  lineCountPrepass=0;
  comments.clear();
}

static CommentInPrepass* locatePrepassComment(int from, int to) {
  //printf("Locate %d-%d\n", from, to);
  for(uint i=0; i<comments.count(); i++) { // todo: optimize
    int c = comments.at(i)->column;
    //printf("Candidate %d\n", c);
    if (c>=from && c<=to) {
      // comment for previous variable or parameter
      return comments.at(i);
    }
  }
  return NULL;
}

static void updateVariablePrepassComment(int from, int to) {
  CommentInPrepass *c = locatePrepassComment(from, to);
  if (c!=NULL && v_type == V_VARIABLE) {
    last_entry->brief = c->str;
  } else if (c!=NULL && v_type == V_PARAMETER) {
    Argument *parameter = getParameter(argName);
    if (parameter) parameter->docs = c->str;
  }
}

static int getAmpersandAtTheStart(const char *buf, int length)
{
  for(int i=0; i<length; i++) {
    switch(buf[i]) {
      case ' ':
      case '\t':
        break;
      case '&':
        return i;
      default:
        return -1;
    }
  }
  return -1;
}

/* Returns ampersand index, comment start index or -1 if neither exist.*/ 
static int getAmpOrExclAtTheEnd(const char *buf, int length)
{
  // Avoid ampersands in string and comments
  int parseState = Start;
  char quoteSymbol = 0;
  int ampIndex = -1;
  int commentIndex = -1;

  for(int i=0; i<length && parseState!=Comment; i++)
  {
    // When in string, skip backslashes
    // Legacy code, not sure whether this is correct?
    if(parseState==String)
    {
      if(buf[i]=='\\') i++;
    }

    switch(buf[i])
    {
        case '\'':
        case '"':
          // Close string, if quote symbol matches.
          // Quote symbol is set iff parseState==String
          if(buf[i]==quoteSymbol)
          {
             parseState = Start;
             quoteSymbol = 0;
          }
          // Start new string, if not already in string or comment
          else if(parseState==Start)
          {
            parseState = String;
            quoteSymbol = buf[i];
          }
          ampIndex = -1; // invalidate prev ampersand
          break;
        case '!':
          // When in string or comment, ignore exclamation mark
          if(parseState==Start)
          {
            parseState = Comment;
            commentIndex = i;
          }
          break;
        case ' ':  // ignore whitespace
        case '\t':
        case '\n': // this may be at the end of line
          break;
        case '&':
          ampIndex = i;
          break;
        default:
          ampIndex = -1; // invalidate prev ampersand
    }
  }

  if (ampIndex>=0)
    return ampIndex;
  else
   return commentIndex;
}

/* Although comments at the end of continuation line are grabbed by this function,
* we still do not know how to use them later in parsing.
*/
void truncatePrepass(int index)
{
  int length = inputStringPrepass.length();
  for (int i=index+1; i<length; i++) {
    if (inputStringPrepass[i]=='!' && i<length-1 && inputStringPrepass[i+1]=='<') { // save comment 
      struct CommentInPrepass *c=new CommentInPrepass(index, inputStringPrepass.right(length-i-2));
      comments.append(c);
    }
  }
  inputStringPrepass.truncate(index);
}

// simplified way to know if this is fixed form
// duplicate in fortrancode.l
static bool recognizeFixedForm(const char* contents)
{
  int column=0;
  bool skipLine=FALSE;

  for(int i=0;;i++) {
    column++;

    switch(contents[i]) {
      case '\n':
        column=0;
        skipLine=FALSE;
        break;
      case ' ':
        break;
      case '\000':
        return FALSE;
      case 'C':
      case 'c':
      case '*':
        if(column==1) return TRUE;
        if(skipLine) break;
        return FALSE;
      case '!':
        if(column>1 && column<7) return FALSE;
        skipLine=TRUE;
        break;
      default:
        if(skipLine) break;
        if(column==7) return TRUE;
        return FALSE;
    }
  }
  return FALSE;
}

/* This function assumes that contents has at least size=length+1 */
static void insertCharacter(char *contents, int length, int pos, char c)
{
  // shift tail by one character
  for(int i=length; i>pos; i--)
    contents[i]=contents[i-1];
  // set the character
  contents[pos] = c;
}

/* change comments and bring line continuation character to previous line */
static const char* prepassFixedForm(const char* contents)
{
  int column=0;
  int prevLineLength=0;
  int prevLineAmpOrExclIndex=-1;
  bool emptyLabel=TRUE;
  int newContentsSize = strlen(contents)+2; // \000 and one spare character (to avoid reallocation)
  char* newContents = (char*)malloc(newContentsSize);

  for(int i=0, j=0;;i++,j++) {
    if(j>=newContentsSize-1) { // check for one spare character, which may be eventually used below (by &)
      newContents = (char*)realloc(newContents, newContentsSize+1000);
      newContentsSize = newContentsSize+1000;
    }

    column++;
    char c = contents[i];
    switch(c) {
      case '\n':
        prevLineLength=column;
        prevLineAmpOrExclIndex=getAmpOrExclAtTheEnd(&contents[i-prevLineLength+1], prevLineLength);
        column=0;
	emptyLabel=TRUE;
        newContents[j]=c;
        break;
      case ' ':
        newContents[j]=c;
        break;
      case '\000':
        newContents[j]='\000';
        return newContents;
      case 'C':
      case 'c':
      case '*':
	emptyLabel=FALSE;
        if(column==1)
	  newContents[j]='!';
        else
          newContents[j]=c;
        break;
      default:
        if(column==6 && emptyLabel) { // continuation
          newContents[j]=' ';

          if(prevLineAmpOrExclIndex==-1) { // add & just before end of previous line
            insertCharacter(newContents, j+1, (j+1)-6-1, '&');
            j++;
          } else { // add & just before end of previous line comment
            insertCharacter(newContents, j+1, (j+1)-6-prevLineLength+prevLineAmpOrExclIndex, '&');
            j++;
          }
	} else {
	  newContents[j]=c;
	  emptyLabel=FALSE;
	}
        break;
    }
  }
  return newContents;
}

static void pushBuffer(QCString& buffer)
{
  if ( include_stack_ptr >= MAX_INCLUDE_DEPTH )
  {
    fprintf( stderr, "Stack buffers nested too deeply" );
    exit( 1 );
  }
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(buffer));

  //fprintf(stderr, "--PUSH--%s", (const char *)buffer);
  buffer = NULL;
}

static void popBuffer() {
  //fprintf(stderr, "--POP--");
  include_stack_ptr --;
  yy_delete_buffer( YY_CURRENT_BUFFER );
  yy_switch_to_buffer( include_stack[include_stack_ptr] );
}

/** used to copy entry to an interface module procedure */
static void copyEntry(Entry *dest, Entry *src) 
{
   dest->type     = src->type;
   dest->fileName = src->fileName;
   dest->bodyLine = src->bodyLine;
   dest->args     = src->args;
   dest->argList  = new ArgumentList(*src->argList);
   dest->doc      = src->doc;
   dest->brief    = src->brief;
}

/** fill empty interface module procedures with info from 
    corresponding module subprogs 
    @TODO: handle procedures in used modules
*/
void resolveModuleProcedures(QList<Entry> &moduleProcedures, Entry *current_root)
{
  if (moduleProcedures.isEmpty()) return;
 
  EntryListIterator eli1(moduleProcedures);
  // for all module procedures
  for (Entry *ce1; (ce1=eli1.current()); ++eli1) 
  {
    // check all entries in this module
    EntryListIterator eli2(*current_root->children());
    for (Entry *ce2; (ce2=eli2.current()); ++eli2) 
    {
      if (ce1->name == ce2->name) 
      {
        copyEntry(ce1, ce2);
      }
    } // for procedures in current module
  } // for all interface module procedures
  moduleProcedures.clear();
}

static bool isTypeName(QCString name)
{
  name = name.lower();
  return name=="integer" || name == "real" || 
    name=="complex" || name == "logical";
}

/*! Extracts string which resides within parentheses of provided string. */
static QCString extractFromParens(const QCString name)
{
  QCString extracted = name;
  int start = extracted.find("(");
  if (start != -1) 
  {
    extracted.remove(0, start+1);
  }
  int end = extracted.findRev(")");
  if (end != -1) 
  {
    int length = extracted.length();
    extracted.remove(end, length);
  }
  extracted = extracted.stripWhiteSpace();

  return extracted;
}

/*! Adds passed modifiers to these modifiers.*/
SymbolModifiers& SymbolModifiers::operator|=(const SymbolModifiers &mdfs)
{
  if (mdfs.protection!=NONE_P) protection = mdfs.protection;
  if (mdfs.direction!=NONE_D) direction = mdfs.direction;
  optional |= mdfs.optional;
  if (!mdfs.dimension.isNull()) dimension = mdfs.dimension;
  allocatable |= mdfs.allocatable;
  external |= mdfs.external;
  intrinsic |= mdfs.intrinsic;
  parameter |= mdfs.parameter;
  pointer |= mdfs.pointer;
  target |= mdfs.target;
  save |= mdfs.save;
  deferred |= mdfs.deferred;
  nonoverridable |= mdfs.nonoverridable;
  nopass |= mdfs.nopass;
  pass |= mdfs.pass;
  passVar = mdfs.passVar;
  return *this;
}

/*! Extracts  and adds passed modifier to these modifiers.*/
SymbolModifiers& SymbolModifiers::operator|=(QCString mdfString)
{
  mdfString = mdfString.lower();
  SymbolModifiers newMdf;

  if (mdfString.find("dimension")==0) 
  {
    newMdf.dimension=mdfString;
  }
  else if (mdfString.contains("intent")) 
  {
    QCString tmp = extractFromParens(mdfString);
    bool isin = tmp.contains("in");
    bool isout = tmp.contains("out");
    if (isin && isout) newMdf.direction = SymbolModifiers::INOUT;
    else if (isin) newMdf.direction = SymbolModifiers::IN;
    else if (isout) newMdf.direction = SymbolModifiers::OUT;
  }
  else if (mdfString=="public") 
  {
    newMdf.protection = SymbolModifiers::PUBLIC;
  }
  else if (mdfString=="private") 
  {
    newMdf.protection = SymbolModifiers::PRIVATE;
  }
  else if (mdfString=="optional") 
  {
    newMdf.optional = TRUE;
  }
  else if (mdfString=="allocatable") 
  {
    newMdf.allocatable = TRUE;
  }
  else if (mdfString=="external") 
  {
    newMdf.external = TRUE;
  }
  else if (mdfString=="intrinsic") 
  {
    newMdf.intrinsic = TRUE;
  }
  else if (mdfString=="parameter") 
  {
    newMdf.parameter = TRUE;
  }
  else if (mdfString=="pointer") 
  {
    newMdf.pointer = TRUE;
  }
  else if (mdfString=="target") 
  {
    newMdf.target = TRUE;
  }
  else if (mdfString=="save") 
  {
    newMdf.save = TRUE;
  }
  else if (mdfString=="nopass")
  {
    newMdf.nopass = TRUE;
  }
  else if (mdfString=="deferred")
  {
    newMdf.deferred = TRUE;
  }
  else if (mdfString=="non_overridable")
  {
    newMdf.nonoverridable = TRUE;
  }
  else if (mdfString.contains("pass"))
  {
    newMdf.pass = TRUE;
    if (mdfString.contains("("))
      newMdf.passVar = extractFromParens(mdfString);
    else
      newMdf.passVar = "";
  }

  (*this) |= newMdf;
  return *this;
}

/*! For debugging purposes. */
//ostream& operator<<(ostream& out, const SymbolModifiers& mdfs)
//{
//  out<<mdfs.protection<<", "<<mdfs.direction<<", "<<mdfs.optional<<
//    ", "<<(mdfs.dimension.isNull() ? "" : mdfs.dimension.latin1())<<
//    ", "<<mdfs.allocatable<<", "<<mdfs.external<<", "<<mdfs.intrinsic;
//
//  return out;
//}

/*! Find argument with given name in \a subprog entry. */
static Argument *findArgument(Entry* subprog, QCString name, bool byTypeName = FALSE)
{
  QCString cname(name.lower());
  for (unsigned int i=0; i<subprog->argList->count(); i++) 
  {
    Argument *arg = subprog->argList->at(i);
    if ((!byTypeName && arg->name.lower() == cname) ||
	(byTypeName && arg->type.lower() == cname)
       )
    {
      return arg;
    }
  }
  return 0;
}

/*! Find function with given name in \a entry. */
#if 0
static Entry *findFunction(Entry* entry, QCString name)
{
  QCString cname(name.lower());

  EntryListIterator eli(*entry->children());
  Entry *ce;
  for (;(ce=eli.current());++eli) 
  {
    if (ce->section != Entry::FUNCTION_SEC)
      continue;

    if (ce->name.lower() == cname)
      return ce;
  }

  return 0;
}
#endif

/*! Apply modifiers stored in \a mdfs to the \a typeName string. */
static QCString applyModifiers(QCString typeName, SymbolModifiers& mdfs) 
{
  if (!mdfs.dimension.isNull()) 
  {
    typeName += ",";
    typeName += mdfs.dimension;
  }
  if (mdfs.direction!=SymbolModifiers::NONE_D) 
  {
    typeName += ",";
    typeName += directionStrs[mdfs.direction];
  }
  if (mdfs.optional) 
  {
    typeName += ",";
    typeName += "optional";
  }
  if (mdfs.allocatable) 
  {
    typeName += ",";
    typeName += "allocatable";
  }
  if (mdfs.external) 
  {
    typeName += ",";
    typeName += "external";
  }
  if (mdfs.intrinsic) 
  {
    typeName += ",";
    typeName += "intrinsic";
  }
  if (mdfs.parameter) 
  {
    typeName += ",";
    typeName += "parameter";
  }
  if (mdfs.pointer) 
  {
    typeName += ",";
    typeName += "pointer";
  }
  if (mdfs.target) 
  {
    typeName += ",";
    typeName += "target";
  }
  if (mdfs.save) 
  {
    typeName += ",";
    typeName += "save";
  }
  if (mdfs.deferred) 
  {
    typeName += ",";
    typeName += "deferred";
  }
  if (mdfs.nonoverridable) 
  {
    typeName += ",";
    typeName += "non_overridable";
  }
  if (mdfs.nopass) 
  {
    typeName += ",";
    typeName += "nopass";
  }
  if (mdfs.pass) 
  {
    typeName += ",";
    typeName += "pass";
    if (!mdfs.passVar.isEmpty())
      typeName += "(" + mdfs.passVar + ")";
  }
  if (mdfs.protection == SymbolModifiers::PUBLIC)
  {
    typeName += ",";
    typeName += "public";
  }
  else if (mdfs.protection == SymbolModifiers::PRIVATE)
  {
    typeName += ",";
    typeName += "private";
  }

  return typeName;
}

/*! Apply modifiers stored in \a mdfs to the \a arg argument. */
static void applyModifiers(Argument *arg, SymbolModifiers& mdfs)
{
  QCString tmp = arg->type;
  arg->type = applyModifiers(tmp, mdfs);
}

/*! Apply modifiers stored in \a mdfs to the \a ent entry. */
static void applyModifiers(Entry *ent, SymbolModifiers& mdfs)
{
  QCString tmp = ent->type;
  ent->type = applyModifiers(tmp, mdfs);

  if (mdfs.protection == SymbolModifiers::PUBLIC)
    ent->protection = Public;
  else if (mdfs.protection == SymbolModifiers::PRIVATE)
    ent->protection = Private;
}

/*! Starts the new scope in fortran program. Consider using this function when
 * starting module, interface, function or other program block.
 * \see endScope()
 */
static void startScope(Entry *scope) 
{
  //cout<<"start scope: "<<scope->name<<endl;
  current_root= scope; /* start substructure */

  QMap<QCString,SymbolModifiers> mdfMap;
  modifiers.insert(scope, mdfMap);
}

/*! Ends scope in fortran program: may update subprogram arguments or module variable attributes.
 * \see startScope()
 */
static bool endScope(Entry *scope, bool isGlobalRoot)
{
  //cout<<"end scope: "<<scope->name<<endl;
  if (current_root->parent() || isGlobalRoot)
  {
    current_root= current_root->parent(); /* end substructure */
  }
  else 
  {
    fprintf(stderr,"parse error in end <scopename>");
    scanner_abort();
    return FALSE;
  }

  // update variables or subprogram arguments with modifiers
  QMap<QCString,SymbolModifiers>& mdfsMap = modifiers[scope];

  if (scope->section == Entry::FUNCTION_SEC) 
  {
    // iterate all symbol modifiers of the scope
    for (QMap<QCString,SymbolModifiers>::Iterator it=mdfsMap.begin(); it!=mdfsMap.end(); it++) 
    {
      //cout<<it.key()<<": "<<it.data()<<endl;
      Argument *arg = findArgument(scope, it.key());

      if (arg)
        applyModifiers(arg, it.data());
    }

    // find return type for function
    //cout<<"RETURN NAME "<<modifiers[current_root][scope->name.lower()].returnName<<endl;
    QCString returnName = modifiers[current_root][scope->name.lower()].returnName.lower();
    if (modifiers[scope].contains(returnName)) 
    {
      scope->type = modifiers[scope][returnName].type; // returning type works
      applyModifiers(scope, modifiers[scope][returnName]); // returning array works
    }

  } 
  if (scope->section == Entry::CLASS_SEC) 
  { // was INTERFACE_SEC
    if (scope->parent()->section == Entry::FUNCTION_SEC) 
    { // interface within function
      // iterate functions of interface and 
      // try to find types for dummy(ie. argument) procedures.
      //cout<<"Search in "<<scope->name<<endl;
      EntryListIterator eli(*scope->children());
      Entry *ce;
      for (;(ce=eli.current());++eli) 
      {
        if (ce->section != Entry::FUNCTION_SEC)
          continue;

        Argument *arg = findArgument(scope->parent(), ce->name, TRUE);
        if (arg != 0) 
	{
          // set type of dummy procedure argument to interface
	  arg->name = arg->type;
          arg->type = scope->name;
        }
      }
    }
  } 
  if (scope->section!=Entry::FUNCTION_SEC) 
  { // not function section 
    // iterate variables: get and apply modifiers
    EntryListIterator eli(*scope->children());
    Entry *ce;
    for (;(ce=eli.current());++eli) 
    {
      if (ce->section != Entry::VARIABLE_SEC && ce->section != Entry::FUNCTION_SEC)
        continue;

      //cout<<ce->name<<", "<<mdfsMap.contains(ce->name.lower())<<mdfsMap.count()<<endl;
      if (mdfsMap.contains(ce->name.lower()))
        applyModifiers(ce, mdfsMap[ce->name.lower()]);
    }
  }

  // clear all modifiers of the scope
  modifiers.remove(scope);

  return TRUE;
}

//! Return full name of the entry. Sometimes we must combine several names recursively.
static QCString getFullName(Entry *e) 
{
  QCString name = e->name;
  if (e->section == Entry::CLASS_SEC //  || e->section == Entry::INTERFACE_SEC
     || !e->parent() || e->parent()->name.isEmpty())
    return name;

  return getFullName(e->parent())+"::"+name;
}

static int yyread(char *buf,int max_size)
{
  int c=0;

  while ( c < max_size && inputString[inputPosition] )
  {
    *buf = inputString[inputPosition++] ;
    c++; buf++;
  }
  return c;
}

static void initParser()
{
  last_entry = 0;
}

static void initEntry()
{
  current->protection = defaultProtection ;
  current->mtype      = mtype;
  current->virt       = virt;
  current->stat       = gstat;
  current->lang       = SrcLangExt_F90; 
  initGroupInfo(current);
}

/**
  adds current entry to current_root and creates new current
*/
static void addCurrentEntry()
{
  //printf("===Adding entry %s to %s\n", current->name.data(), current_root->name.data());
  current_root->addSubEntry(current);
  last_entry = current;
  current = new Entry ;
  initEntry();
}

static int max(int a, int b) {return a>b?a:b;}

static void addModule(const char *name, bool isModule)
{
  //fprintf(stderr, "0=========> got module %s\n", name);

  if (isModule)
    current->section = Entry::NAMESPACE_SEC;
  else
    current->section = Entry::FUNCTION_SEC;

  if (name!=NULL)
  {
    current->name = name;
  } 
  else
  {
    QCString fname = yyFileName;
    int index = max(fname.findRev('/'), fname.findRev('\\'));
    fname = fname.right(fname.length()-index-1);
    fname = fname.prepend("__").append("__");
    current->name = fname;
  }
  current->type = "program";
  current->fileName  = yyFileName;
  current->bodyLine  = yyLineNr; // used for source reference
  current->protection = Public ;
  addCurrentEntry();
  startScope(last_entry);
}


static void addSubprogram(const char *text)
{
  //fprintf(stderr,"1=========> got subprog, type: %s\n",text); 
  current->section = Entry::FUNCTION_SEC ;
  QCString subtype = text; subtype=subtype.lower().stripWhiteSpace();
  if (!current->type) current->type = subtype;
  current->fileName  = yyFileName;
  current->bodyLine  = yyLineNr; // used for source reference
  current->startLine = -1; // ??? what is startLine for?
  current->args.resize(0);
  current->argList->clear();
  docBlock.resize(0);
}

/*! Adds interface to the root entry.
 * \note Code was brought to this procedure from the parser,
 * because there was/is idea to use it in several parts of the parser.
 */ 
static void addInterface(QCString name, InterfaceType type)
{
  if (YY_START == Start)
  {
    addModule(NULL);
    yy_push_state(ModuleBody); //anon program
  }

  current->section = Entry::CLASS_SEC; // was Entry::INTERFACE_SEC;
  current->spec = Entry::Interface;
  current->name = name;

  switch (type)
  {
    case IF_ABSTRACT:
      current->type = "abstract";
      break;

    case IF_GENERIC:
      current->type = "generic";
      break;

    case IF_SPECIFIC:
    case IF_NONE:
    default:
      current->type = "";
  }

  /* if type is part of a module, mod name is necessary for output */
  if ((current_root) && 
      (current_root->section ==  Entry::CLASS_SEC ||
       current_root->section ==  Entry::NAMESPACE_SEC)) 
  {
    current->name= current_root->name + "::" + current->name;
  }

  current->fileName = yyFileName;
  current->bodyLine  = yyLineNr; 
  addCurrentEntry();
}


//-----------------------------------------------------------------------------

/*! Get the argument \a name.
 */
static Argument* getParameter(const QCString &name)
{
  // std::cout<<"addFortranParameter(): "<<name<<" DOCS:"<<(docs.isNull()?QCString("null"):docs)<<std::endl;
  Argument *ret = 0;
  if (current_root->argList==0) return 0;
  ArgumentListIterator ali(*current_root->argList);
  Argument *a;
  for (ali.toFirst();(a=ali.current());++ali)
  {
    if (a->name.lower()==name.lower())
    {
      ret=a;
      //printf("parameter found: %s\n",(const char*)name);
      break;
    }
  } // for
  return ret;
}

  //----------------------------------------------------------------------------
static void startCommentBlock(bool brief)
{
  if (brief)
  {
    current->briefFile = yyFileName;
    current->briefLine = yyLineNr;
  }
  else
  {
    current->docFile = yyFileName;
    current->docLine = yyLineNr;
  }
}

  //----------------------------------------------------------------------------
static void handleCommentBlock(const QCString &doc,bool brief)
{
  docBlockInBody = FALSE;
  bool needsEntry = FALSE;
  static bool hideInBodyDocs = Config_getBool("HIDE_IN_BODY_DOCS");
  int position=0;
  if (docBlockInBody && hideInBodyDocs) return;
  //fprintf(stderr,"call parseCommentBlock [%s]\n",doc.data());
  int lineNr = brief ? current->briefLine : current->docLine;
  while (parseCommentBlock(
	g_thisParser,
	docBlockInBody ? last_entry : current,
	doc,        // text
	yyFileName, // file
	lineNr,
	docBlockInBody ? FALSE : brief, 
	docBlockInBody ? FALSE : docBlockJavaStyle,
	docBlockInBody,
	defaultProtection,
        position,
        needsEntry
        )) 
  {
	   //fprintf(stderr,"parseCommentBlock position=%d [%s]  needsEntry=%d\n",position,doc.data()+position,needsEntry);
   if (needsEntry) addCurrentEntry();
  }
  //fprintf(stderr,"parseCommentBlock position=%d [%s]  needsEntry=%d\n",position,doc.data()+position,needsEntry);

  if (needsEntry) addCurrentEntry();
}

//----------------------------------------------------------------------------
static int level=0;
static void debugCompounds(Entry *rt)  // print Entry structure (for debugging)
{
 level++;
  printf("%d) debugCompounds(%s) line %d\n",level, rt->name.data(), rt->bodyLine);
  EntryListIterator eli(*rt->children());
  Entry *ce;
  for (;(ce=eli.current());++eli)
  {
     debugCompounds(ce); 
  } 
level--;
}


static void parseMain(const char *fileName,const char *fileBuf,Entry *rt)
{
  initParser();

  defaultProtection = Public;
  inputString = fileBuf;
  inputPosition = 0;
  inputStringPrepass = NULL;
  inputPositionPrepass = 0;

  //anonCount     = 0;  // don't reset per file
  mtype         = Method;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  global_root   = rt;
  inputFile.setName(fileName);
  if (inputFile.open(IO_ReadOnly))
  {
    isFixedForm = recognizeFixedForm(fileBuf);

    if (isFixedForm) {
      printf("Prepassing fixed form of %s\n", fileName);
      //printf("---strlen=%d\n", strlen(fileBuf));
      //clock_t start=clock();

      inputString = prepassFixedForm(fileBuf);

      //clock_t end=clock();
      //printf("CPU time used=%f\n", ((double) (end-start))/CLOCKS_PER_SEC);
    }

    yyLineNr= 1 ; 
    yyFileName = fileName;
    msg("Parsing file %s...\n",yyFileName.data());

    startScope(rt); // implies current_root = rt
    initParser();
    groupEnterFile(yyFileName,yyLineNr);

    current          = new Entry;
    current->lang    = SrcLangExt_F90; 
    current->name    = yyFileName;
    current->section = Entry::SOURCE_SEC;
    current_root->addSubEntry(current);
    file_root        = current;
    current          = new Entry;
    current->lang    = SrcLangExt_F90; 

    fscanYYrestart( fscanYYin );
    {
      BEGIN( Start );
    }

    fscanYYlex();
    groupLeaveFile(yyFileName,yyLineNr);

    endScope(current_root, TRUE); // TRUE - global root

    //debugCompounds(rt); //debug 

    rt->program.resize(0);
    delete current; current=0;
    moduleProcedures.clear();
    if (isFixedForm) {
      free((char*)inputString);
      inputString=NULL;
    }

    inputFile.close();
  }
}

//----------------------------------------------------------------------------

void FortranLanguageScanner::parseInput(const char *fileName,const char *fileBuf,Entry *root)
{
  g_thisParser = this;
  ::parseMain(fileName,fileBuf,root);
}

void FortranLanguageScanner::parseCode(CodeOutputInterface & codeOutIntf,
                   const char * scopeName,
                   const QCString & input,
                   bool isExampleBlock,
                   const char * exampleName,
                   FileDef * fileDef,
                   int startLine,
                   int endLine,
                   bool inlineFragment,
		   MemberDef *memberDef,
                   bool showLineNumbers
                  )
{
  ::parseFortranCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                     fileDef,startLine,endLine,inlineFragment,memberDef,
                     showLineNumbers);
}

bool FortranLanguageScanner::needsPreprocessing(const QCString &extension)
{
  return !(extension==".f"||extension==".f90"||
           extension==".f95"||extension==".f03");
}
void FortranLanguageScanner::resetCodeParserState()
{
  ::resetFortranCodeParserState();
}

void FortranLanguageScanner::parsePrototype(const char *text)
{
  (void)text;
}

static void scanner_abort() 
{
  fprintf(stderr,"********************************************************************\n");
  fprintf(stderr,"Error in file %s line: %d, state: %d\n",yyFileName.data(),yyLineNr,YY_START);
  fprintf(stderr,"********************************************************************\n");
   
  EntryListIterator eli(*global_root->children());
  Entry *ce;
  bool start=FALSE;

  for (;(ce=eli.current());++eli)
  {
     if (ce == file_root) start=TRUE;
     if (start) ce->reset(); 
  } 

  // dummy call to avoid compiler warning
  (void)yy_top_state();
  
  return;
  //exit(-1);
}

//----------------------------------------------------------------------------

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void fscannerYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

