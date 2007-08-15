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

// {{{ ----- Helper structs -----
//! Holds modifiers (ie attributes) for one symbol (variable, function, etc)
struct SymbolModifiers {
  enum Protection {NONE_P, PUBLIC, PRIVATE};
  enum Direction {NONE_D, IN, OUT, INOUT};

  //!< This is only used with function return value.
  QString type, returnName;
  Protection protection;
  Direction direction;
  bool optional;
  QString dimension;
  bool allocatable;
  bool external;
  bool intrinsic;
  bool parameter;
  bool pointer;
  bool target;
  bool save;

  SymbolModifiers() : type(), returnName(), protection(NONE_P), direction(NONE_D),
    optional(FALSE), dimension(), allocatable(FALSE),
    external(FALSE), intrinsic(FALSE), parameter(FALSE),
    pointer(FALSE), target(FALSE), save(FALSE) {}

  SymbolModifiers& operator|=(const SymbolModifiers &mdfs);
  SymbolModifiers& operator|=(QString mdfrString);
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
static QFile            inputFile;
static QCString		yyFileName;
static int		yyLineNr     = 1 ;
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

static QString           debugStr;
static QCString          result; // function result
static Argument          *parameter; // element of parameter list
static QCString          argType;  // fortran type of an argument of a parameter list
static QCString          argName;  // last identifier name in variable list
static QCString          initializer;  // initial value of a variable
static QCString          useModuleName;  // name of module in the use statement
static Protection        defaultProtection;

static char              stringStartSymbol; // single or double quote

//! Accumulated modifiers of current statement, eg variable declaration.
static SymbolModifiers currentModifiers;
//! Holds program scope->symbol name->symbol modifiers.
static QMap<Entry*,QMap<QString,SymbolModifiers> > modifiers;

//-----------------------------------------------------------------------------

static int yyread(char *buf,int max_size);
static void startCommentBlock(bool);
static void handleCommentBlock(const QCString &doc,bool brief);
static void addCurrentEntry();
static void addInterface(QString name);
static Argument *addFortranParameter(const QCString &type,const QCString &name, const QString docs);
static void scanner_abort();

static void startScope(Entry *scope);
static bool endScope(Entry *scope);
static QString getFullName(Entry *e);
static bool isTypeName(QString name);
static void resolveModuleProcedures(QList<Entry> &moduleProcedures, Entry *current_root);

//-----------------------------------------------------------------------------
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);
//-----------------------------------------------------------------------------

%}

 //-----------------------------------------------------------------------------
 //-----------------------------------------------------------------------------
IDSYM	  [a-z_A-Z0-9]
NOTIDSYM  [^a-z_A-Z0-9]
SEPARATE  [:, \t]
ID        [a-z_A-Z]+{IDSYM}*
PP_ID     {ID}
LABELID   [a-z_A-Z]+[a-z_A-Z0-9\-]*
SUBPROG   (subroutine|function)
B         [ \t]
BS        [ \t]*
BS_       [ \t]+
COMMA     {BS},{BS}
ARGS      {BS}("("[^)]*")"){BS}
NOARGS    {BS}"\n"

NUM_TYPE  (complex|integer|logical|real)
KIND      {ARGS}
CHAR      (CHARACTER{ARGS}?|CHARACTER{BS}"*"([0-9]+|{ARGS}))
TYPE_SPEC (({NUM_TYPE}("*"[0-9]+)?)|({NUM_TYPE}{KIND})|DOUBLE{BS}PRECISION|{CHAR}|TYPE{ARGS})

INTENT_SPEC intent{BS}"("{BS}(in|out|in{BS}out){BS}")"
ATTR_SPEC (ALLOCATABLE|DIMENSION{ARGS}|EXTERNAL|{INTENT_SPEC}|INTRINSIC|OPTIONAL|PARAMETER|POINTER|PRIVATE|PUBLIC|SAVE|TARGET)
ACCESS_SPEC (PRIVATE|PUBLIC)
/* Assume that attribute statements are almost the same as attributes. */
ATTR_STMT {ATTR_SPEC}|DIMENSION

%option noyywrap
%option stack
%option caseless
/*%option debug */

 //---------------------------------------------------------------------------------

 /** fortran parsing states */
%x	Subprog
%x	Parameterlist
%x	SubprogBody
%x	Start
%x	Comment
%x      Module
%x      ModuleBody
%x	AttributeList
%x      Variable
%x      Initialization
%x      ArrayInitializer
%x      Typedef
%x      TypedefBody
%x      InterfaceBody
%x      StrIgnore
%x      String
%x      Use
%x      UseOnly
%x      ModuleProcedure

 /** comment parsing states */
%x	DocBlock
%x	DocBackLine
%x	EndDoc

%%

 /*-----------------------------------------------------------------------------------*/

<*>"&".*"\n"                            { if (YY_START == String) REJECT; // "&" is ignored in strings
                                          yyLineNr++;}  /* line not finished -> read next line (text after "&" may be 
                                                           comment and has to be ignored */

 /*------ ignore strings */ 
<*>"\\\\"                               { /* ignore \\  */}
<*>"\\\""|\\\'                          { /* ignore \" and \'  */}

<String>\"|\'                           { // string ends with next quote without previous backspace
                                          if(yytext[0]!=stringStartSymbol) REJECT; // single vs double quote
                                          // cout << "string end: " << debugStr << endl;
                                          yy_pop_state();
                                        }           

<String>.                               {debugStr+=yytext;} // ignore String contents (especially '!') 

<*>\"|\'                                { /* string starts */
					  if(YY_START == StrIgnore) REJECT; // ignore in simple comments
                                          // cout << "string start: " << yytext[0] << yyLineNr << endl;
                                          yy_push_state(YY_START);
                                          stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String); debugStr="!^!";
                                        }

 /*------ ignore simple comment (not documentation comments) */

<*>"!"/[^<>\n]                         {  if (YY_START == String) REJECT; // "!" is ignored in strings
                                          // skip comment line (without docu comments "!>" "!<" ) 
                                          /* ignore further "!" and ignore comments in Strings */
                                          if ((YY_START != StrIgnore) && (YY_START != String)) {
                                            yy_push_state(YY_START);
                                            BEGIN(StrIgnore); 
                                            debugStr="*!";
                                            //cout << "start comment "<< yyLineNr << endl;
                                           }      
                                        }
<StrIgnore>.?/\n                        { yy_pop_state(); // comment ends with endline character
                                          //cout << "end comment " << yyLineNr <<" "<< debugStr << endl;
                                        } // comment line ends
<StrIgnore>.                            { debugStr+=yytext; } 


 /*------ use handling ------------------------------------------------------------*/

<Start,ModuleBody,TypedefBody,SubprogBody>"use"{BS_} { 
                                          yy_push_state(YY_START);
					  BEGIN(Use);     
                                        }
<Use>{ID}                               { 
                                          //cout << "using dir "<< yytext << endl;
                                          current->name=yytext;
                                          current->fileName = yyFileName; 
  					  current->section=Entry::USINGDIR_SEC;
					  current_root->addSubEntry(current);
					  current = new Entry;
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
  					}
<Use,UseOnly>"\n"                       {
                                          unput(*yytext);
                                          yy_pop_state();
                                        }

 /*------ ignore special fortran statements */
<Start,ModuleBody,SubprogBody>(^|[ \t])interface({BS_}{ID})?/[ \t\n] { // handle interface block
					  QString name = yytext;
					  int index = name.find("interface", 0, FALSE);
					  index = name.find(QRegExp("[^ \\t]"), index+9);
					  //cout<<name<<", "<<index<<endl;
					  if(index!=-1)
					    name = name.right(name.length()-index);
					  else // interface without name, must be inside subprog
					    name = "interface"; 
					  addInterface(name);
					  yy_push_state(InterfaceBody);
 					  startScope(last_entry);
                                        }
<InterfaceBody>"end"{BS}"interface".*   {
					  if(!endScope(current_root))
					    yyterminate();
					  yy_pop_state();
                                          //cout << "end interface " << yyLineNr
					  //  <<", "<<Interface<<endl;
                                        }
<InterfaceBody>module{BS}procedure      { yy_push_state(YY_START);
                                          BEGIN(ModuleProcedure);
                                        }
<ModuleProcedure>{ID}                   {                       			
                                          current->section = Entry::FUNCTION_SEC ;
                                          current->name = yytext; 
 	                                  moduleProcedures.append(current);
                                          addCurrentEntry();
                                        }
<ModuleProcedure>"\n"                   { unput(*yytext); 
                                          yy_pop_state();
                                        }
<InterfaceBody>.                        {}
                 
 /*------ module handling ------------------------------------------------------------*/ 
<Start>module|program{BS_}             {  //
					    BEGIN(Module);
                                            defaultProtection = Public;
                                       }
<Start,ModuleBody>"end"{BS}(module|program).* { // end module	
					    resolveModuleProcedures(moduleProcedures, current_root);
					    if(!endScope(current_root))
					      yyterminate();
                                            defaultProtection = Public;
					    BEGIN(Start);
                                       }
<Module>{ID}                           {  
                                            //cout << "0=========> got module " << yytext  << endl; 
                                            current->section = Entry::NAMESPACE_SEC;
  					    current->name = yytext;
			                    current->type = "module";
 					    current->fileName  = yyFileName;
					    current->bodyLine  = yyLineNr; // used for source reference
					    current->protection = Public ;

                                            addCurrentEntry();
					    startScope(last_entry);

					    BEGIN(ModuleBody);
                                       }

  /*------- access specification --------------------------------------------------------------------------*/

<ModuleBody>private/{BS}(\n|"!")         { defaultProtection = Private; }
<ModuleBody>public/{BS}(\n|"!")          { defaultProtection = Public; }

 /*------- type definition  -------------------------------------------------------------------------------*/

<Start,ModuleBody>"type"({BS_}|({COMMA}{ACCESS_SPEC})) { /* type definition found : TYPE , access-spec::type-name |*/
 					    yy_push_state(YY_START);
                                            BEGIN(Typedef);
					    current->protection = defaultProtection;
                                        }
<Typedef>{ACCESS_SPEC}          	{ 	
                                            QString type= yytext;				    
                                        }
<Typedef>{ID}                        	{ /* type name found */
                                            //cout << "=========> got typedef " << yytext << ": " << yyLineNr << endl; 
                                            current->section = Entry::CLASS_SEC; // was Entry::STRUCT_SEC;
   					    current->spec = Entry::Struct;        	
  					    current->name = yytext;

                                            /* if type is part of a module, mod name is necessary for output */
                                            if ((current_root) && 
                                               (current_root->section ==  Entry::CLASS_SEC ||
					        current_root->section ==  Entry::NAMESPACE_SEC))
						//current_root->section ==  Entry::INTERFACE_SEC)) 
					    {
                                              current->name= current_root->name+"::"+current->name;
                                            }
					    current->fileName = yyFileName;
					    current->bodyLine  = yyLineNr; 
                                            addCurrentEntry();
					    startScope(last_entry); 
                                            BEGIN(TypedefBody);
                                        }
<TypedefBody>"end"{BS}"type".*          { /* end type definition */
                                           //cout << "=========> got typedef end "<< endl; 
					   if(!endScope(current_root))
					     yyterminate();
                                           yy_pop_state();
                                        } 

 /*------- module/global/typedef variable ---------------------------------------------------*/

<Start,ModuleBody,TypedefBody,SubprogBody>{
{TYPE_SPEC}/{SEPARATE}			{
                                          /* variable declaration starts */
                                          //cout << "4=========> got variable type: " << yytext << endl;
 					  QString help=yytext;
					  help= help.simplifyWhiteSpace();
					  argType= help.latin1();
					  yy_push_state(AttributeList);
                                        }
^{BS}{PP_ID}{KIND}?			{ /* check for preprocessor symbol expand to type */
					  QString str = yytext;
					  str = str.stripWhiteSpace();
					  DefineDict* defines = getFileDefineDict();
					  QString name;
					  int index = str.find("(");
					  if(index != -1)
					    name = str.left(index).stripWhiteSpace();
					  else
					    name = str;

					  Define *define = (*defines)[name];
					  if(define != NULL && isTypeName(define->definition)) {
					    argType = str;
					    yy_push_state(AttributeList);
					  } else {
					    REJECT;
					  }
					}
{ATTR_STMT}{BS}/{ID}	         	{ 
                                          /* attribute statement starts */
                                          //cout << "5=========> Attribute statement: "<< yytext << endl;
                                          QString tmp = yytext;
                                          currentModifiers |= tmp.stripWhiteSpace();
					  argType="";
					  yy_push_state(YY_START);
					  /* goto attribute parsing, however there must not be one,
					     just catch "::" if it is there. */
					  BEGIN( AttributeList ) ;
                                        }
}
<AttributeList>{
{COMMA}					{}
{BS}					{}
{ATTR_SPEC}				{ /* update current modifiers */
                                          QString tmp = yytext;
                                          currentModifiers |= (tmp);
					}
"::"					{ /* end attribute list */
					  BEGIN( Variable );
					}
.					{ /* unknown attribute, consider variable name */
					  //cout<<"start variables, unput "<<*yytext<<endl;
					  unput(*yytext);
					  BEGIN( Variable );
					}
}

<Variable>{BS}				{}
<Variable>{ID}				{ /* parse variable declaration */
                                          //cout << "5=========> got variable: " << argType << "::" << yytext << endl;
 					  /* work around for bug in QCString.replace (QString works) */
					  QString name=yytext;
					  /* remember attributes for the symbol */
					  modifiers[current_root][name.lower()] |= currentModifiers;
					  argName= name.latin1();
					  int last= yy_top_state();

					  v_type= V_IGNORE;
  					  if (!argType.isEmpty() && last != SubprogBody) { // new variable entry
					    v_type = V_VARIABLE;
                                            current->section = Entry::VARIABLE_SEC;
 					    current->name = argName;
					    current->type = argType;
					    current->fileName = yyFileName;
					    current->bodyLine  = yyLineNr; // used for source reference
                                            addCurrentEntry();
                                          } else if(!argType.isEmpty()){ // deklaration of parameter list: add type for corr. parameter 
					    parameter= addFortranParameter(argType,argName,docBlock);
					    if (parameter) v_type= V_PARAMETER;
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
					  QString name(argName);
					  QString attr("dimension");
					  attr += yytext;
					  modifiers[current_root][name] |= attr;
					}
<Variable>{COMMA}	                {}
<Variable>{BS}"="                       { yy_push_state(YY_START);
                                          initializer="";
					  BEGIN(Initialization);
                                        }
<Variable>"\n"	                        { currentModifiers = SymbolModifiers();
                                          yy_pop_state(); // end variable deklaration list
                                          yyLineNr++;
                                          docBlock.resize(0);
                                        }

<Initialization>"(/"                    { initializer+=yytext; 
                                          BEGIN(ArrayInitializer); // initializer may contain comma
                                        }
<ArrayInitializer>.                     { initializer+=yytext; }
<ArrayInitializer>"/)"                  { initializer+=yytext;
                                          yy_pop_state(); // end initialization
                                          if (v_type == V_VARIABLE) last_entry->initializer= initializer;
                                        }
<Initialization>{COMMA}                 { yy_pop_state(); // end initialization
                                          if (v_type == V_VARIABLE) last_entry->initializer= initializer;
                                        }
<Initialization>"\n"|"!"                { //| 
                                          yy_pop_state(); // end initialization
                                          if (v_type == V_VARIABLE) last_entry->initializer= initializer;
 					  unput(*yytext);
                                        }
<Initialization>.                       { initializer+=yytext; }
 
 /*------ fortran subroutine/function handling ------------------------------------------------------------*/
 /*       Start is initial condition                                                                       */
 
<Start,ModuleBody,InterfaceBody>{TYPE_SPEC}{BS}/{SUBPROG}{BS_} {   
		                           // TYPE_SPEC is for old function style function result
                                           result= yytext; 
                                           result= result.stripWhiteSpace();
  					   current->type = result;
                                       }              
<Start,ModuleBody,SubprogBody,InterfaceBody>{BS}{SUBPROG}{BS_} {   // Fortran subroutine or function found
 					   //cout << "1=========> got subprog, type:" << yytext  <<endl; 
                                           current->section = Entry::FUNCTION_SEC ;
                                           QCString subtype = yytext; subtype=subtype.lower().stripWhiteSpace();
	                                   if (!current->type) current->type = subtype;
					   current->fileName  = yyFileName;
					   current->bodyLine  = yyLineNr; // used for source reference
		                           current->startLine = -1; // ??? what is startLine for?
					   current->args.resize(0);
					   current->argList->clear();
                                           yy_push_state(Subprog);
                                           docBlock.resize(0);
                                       }
<Subprog>{BS}                          {   /* ignore white space */   }
<Subprog>{ID}                          {   current->name = yytext; 
					   //cout << "1a==========> got " << current->type << " " << yytext << " " << yyLineNr << endl;
					   modifiers[current_root][current->name.lower()].returnName = current->name;
					   BEGIN(Parameterlist);
                                       } 
<Parameterlist>{ARGS}                  {   
				           //current->type not yet available
                                           QString arglist= yytext;
					   //cout << "3=========> got parameterlist " << yytext << endl;
                                           yyLineNr+= arglist.contains('\n');
                                           arglist = arglist.replace(QRegExp("&[^\n]*\n"),"");
					   //cout << "3=========> got parameterlist " << arglist << endl;
					   current->args = arglist;
					   current->args = removeRedundantWhiteSpace(current->args);
					   stringToArgumentList(current->args, current->argList);
                                           addCurrentEntry();
					   startScope(last_entry);
					   BEGIN(SubprogBody);
 				       } 
<Parameterlist>{NOARGS}                {   
                                           yyLineNr++;
					   //cout << "3=========> without parameterlist " <<endl;
 					   stringToArgumentList("", current->argList);
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
<SubprogBody>"end"{BS}{SUBPROG}.*      {  
                                           //cout << "1e=========> got end subprog: " << yytext << endl;

                                           /* args is used for parameters in list of functions, argList for 
                                             parameters in detailed function descripttion */
                                           //current->args = argListToString(current->argList); 
					   //current->endBodyLine  = yyLineNr; // ??? what ist endBodyLine for
					   if(!endScope(current_root))
					     yyterminate();
					   yy_pop_state() ;
				       } 

 /*---- documentation comments --------------------------------------------------------------------*/

<Variable>"!<"                           { /* backward docu comment (only one line) */
                                          if (v_type != V_IGNORE) {
                                           yy_push_state(YY_START);
					   current->docLine  = yyLineNr;
 					   docBlockJavaStyle = FALSE;
					   docBlock.resize(0);
					   docBlockJavaStyle = Config_getBool("JAVADOC_AUTOBRIEF");
					   startCommentBlock(TRUE);
					   BEGIN(DocBackLine);
                                          }
  					 }
<DocBackLine>.*    			 { // contents of current comment line
                                          docBlock=yytext;
					  if (v_type == V_VARIABLE) {
					    Entry *tmp_entry = current; 
					    current = last_entry; // temporarily switch to the previous entry
  					    handleCommentBlock(docBlock,TRUE);
 					    current=tmp_entry;
                                          }
					  else if (v_type == V_PARAMETER) {
					    parameter->docs=docBlock;
                                          }
                                          yy_pop_state();
 					 }
   
<Start,SubprogBody,ModuleBody,TypedefBody,InterfaceBody>"!>"  {
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
                                          yyLineNr++;
  					}
<DocBlock>"\n"        			{ // comment block ends at the end of this line
                                          //cout <<"3=========> comment block : "<< docBlock << endl;
					  unput(*yytext);                                        
 					  handleCommentBlock(docBlock,TRUE);
                                          yy_pop_state();                                          
  					}

 /*------------------------------------------------------------------------------------------------*/

<*>"\n"                                 {yyLineNr++;
                                         //if (debugStr.stripWhiteSpace().length() > 0) cout << "ignored text: " << debugStr << " state: " <<YY_START << endl;
                                         debugStr="";
                                        }

 /*---- error: EOF in wrong state --------------------------------------------------------------------*/
 <SubprogBody,ModuleBody,String,StrIgnore,InterfaceBody><<EOF>> {
                                          fprintf(stderr,"==== Error: EOF reached in wrong state (end missing)");
					  scanner_abort();
					  yyterminate();
                                        }
 <*>.                 	                {debugStr+=yytext;} // ignore remaining text 

 /**********************************************************************************/
 /**********************************************************************************/
 /**********************************************************************************/
%%
//----------------------------------------------------------------------------

/** used to copy entry to an interface module procedure */
static void copyEntry(Entry *dest, Entry *src) 
{
   dest->type     = src->type;
   dest->fileName = src->fileName;
   dest->bodyLine = src->bodyLine;
   dest->args     = src->args;
   dest->argList  = new ArgumentList(*src->argList);
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

static bool isTypeName(QString name)
{
  name = name.lower();
  return name=="integer" || name == "real" || 
    name=="complex" || name == "logical";
}

/*! Extracts string which resides within parentheses of provided string. */
static QString extractFromParens(const QString name)
{
  QString extracted = name;
  int start = extracted.find("(");
  if(start != -1) 
  {
    extracted.remove(0, start+1);
  }
  int end = extracted.findRev(")");
  if(end != -1) 
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
  if(mdfs.protection!=NONE_P) protection = mdfs.protection;
  if(mdfs.direction!=NONE_D) direction = mdfs.direction;
  optional |= mdfs.optional;
  if(!mdfs.dimension.isNull()) dimension = mdfs.dimension;
  allocatable |= mdfs.allocatable;
  external |= mdfs.external;
  intrinsic |= mdfs.intrinsic;
  parameter |= mdfs.parameter;
  pointer |= mdfs.pointer;
  target |= mdfs.target;
  save |= mdfs.save;
  return *this;
}

/*! Extracts  and adds passed modifier to these modifiers.*/
SymbolModifiers& SymbolModifiers::operator|=(QString mdfString)
{
  mdfString = mdfString.lower();
  SymbolModifiers newMdf;

  if (mdfString.startsWith("dimension")) 
  {
    newMdf.dimension=mdfString;
  }
  else if (mdfString.contains("intent")) 
  {
    QString tmp = extractFromParens(mdfString);
    bool isin = tmp.contains("in");
    bool isout = tmp.contains("out");
    if(isin && isout) newMdf.direction = SymbolModifiers::INOUT;
    else if(isin) newMdf.direction = SymbolModifiers::IN;
    else if(isout) newMdf.direction = SymbolModifiers::OUT;
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
  else if(mdfString=="intrinsic") 
  {
    newMdf.intrinsic = TRUE;
  }
  else if(mdfString=="parameter") 
  {
    newMdf.parameter = TRUE;
  }
  else if(mdfString=="pointer") 
  {
    newMdf.pointer = TRUE;
  }
  else if(mdfString=="target") 
  {
    newMdf.target = TRUE;
  }
  else if(mdfString=="save") 
  {
    newMdf.save = TRUE;
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
static Argument *findArgument(Entry* subprog, QString name, bool byTypeName = FALSE)
{
  QCString cname(name.lower());
  for (unsigned int i=0; i<subprog->argList->count(); i++) 
  {
    Argument *arg = subprog->argList->at(i);
    if(!byTypeName && arg->name.lower() == cname ||
	byTypeName && arg->type.lower() == cname)
      return arg;
  }

  return NULL;
}

/*! Find function with given name in \a entry. */
#if 0
static Entry *findFunction(Entry* entry, QString name)
{
  QCString cname(name.lower());

  EntryListIterator eli(*entry->children());
  Entry *ce;
  for (;(ce=eli.current());++eli) 
  {
    if(ce->section != Entry::FUNCTION_SEC)
      continue;

    if(ce->name.lower() == cname)
      return ce;
  }

  return NULL;
}
#endif

/*! Apply modifiers stored in \a mdfs to the \a typeName string. */
static QString applyModifiers(QString typeName, SymbolModifiers& mdfs) 
{
  if(!mdfs.dimension.isNull()) 
  {
    typeName += ",";
    typeName += mdfs.dimension;
  }
  if(mdfs.direction!=SymbolModifiers::NONE_D) 
  {
    typeName += ",";
    typeName += directionStrs[mdfs.direction];
  }
  if(mdfs.optional) 
  {
    typeName += ",";
    typeName += "optional";
  }
  if(mdfs.allocatable) 
  {
    typeName += ",";
    typeName += "allocatable";
  }
  if(mdfs.external) 
  {
    typeName += ",";
    typeName += "external";
  }
  if(mdfs.intrinsic) 
  {
    typeName += ",";
    typeName += "intrinsic";
  }
  if(mdfs.parameter) 
  {
    typeName += ",";
    typeName += "parameter";
  }
  if(mdfs.pointer) 
  {
    typeName += ",";
    typeName += "pointer";
  }
  if(mdfs.target) 
  {
    typeName += ",";
    typeName += "target";
  }
  if(mdfs.save) 
  {
    typeName += ",";
    typeName += "save";
  }

  return typeName;
}

/*! Apply modifiers stored in \a mdfs to the \a arg argument. */
static void applyModifiers(Argument *arg, SymbolModifiers& mdfs)
{
  QString tmp = arg->type;
  arg->type = applyModifiers(tmp, mdfs);
}

/*! Apply modifiers stored in \a mdfs to the \a ent entry. */
static void applyModifiers(Entry *ent, SymbolModifiers& mdfs)
{
  QString tmp = ent->type;
  ent->type = applyModifiers(tmp, mdfs);

  if(mdfs.protection == SymbolModifiers::PUBLIC)
    ent->protection = Public;
  else if(mdfs.protection == SymbolModifiers::PRIVATE)
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

  QMap<QString,SymbolModifiers> mdfMap;
  modifiers.insert(scope, mdfMap);
}

/*! Ends scope in fortran program: may update subprogram arguments or module variable attributes.
 * \see startScope()
 */
static bool endScope(Entry *scope) 
{
  //cout<<"end scope: "<<scope->name<<endl;
  if (current_root->parent())
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
  QMap<QString,SymbolModifiers>& mdfsMap = modifiers[scope];

  if(scope->section == Entry::FUNCTION_SEC) 
  {
    // iterate all symbol modifiers of the scope
    for(QMap<QString,SymbolModifiers>::Iterator it=mdfsMap.begin(); it!=mdfsMap.end(); it++) {
      //cout<<it.key()<<": "<<it.data()<<endl;
      Argument *arg = findArgument(scope, it.key());

      if(arg)
        applyModifiers(arg, it.data());
    }

    // find return type for function
    //cout<<"RETURN NAME "<<modifiers[current_root][scope->name.lower()].returnName<<endl;
    QString returnName = modifiers[current_root][scope->name.lower()].returnName.lower();
    if(modifiers[scope].contains(returnName)) 
    {
      scope->type = modifiers[scope][returnName].type; // returning type works
      applyModifiers(scope, modifiers[scope][returnName]); // returning array works
    }

  } 
  else if(scope->section == Entry::CLASS_SEC) 
  { // was INTERFACE_SEC
    if(scope->parent()->section == Entry::FUNCTION_SEC) 
    { // interface within function
      // iterate functions of interface and 
      // try to find types for dummy(ie. argument) procedures.
      //cout<<"Search in "<<scope->name<<endl;
      EntryListIterator eli(*scope->children());
      Entry *ce;
      for (;(ce=eli.current());++eli) 
      {
        if(ce->section != Entry::FUNCTION_SEC)
          continue;

        Argument *arg = findArgument(scope->parent(), ce->name, TRUE);
        if(arg != NULL) 
	{
          // set type of dummy procedure argument to interface
	  arg->name = arg->type;
          arg->type = scope->name;
        }
      }
    }

  } 
  else 
  { // not function section or interface
    // iterate variables: get and apply modifiers
    EntryListIterator eli(*scope->children());
    Entry *ce;
    for (;(ce=eli.current());++eli) 
    {
      if(ce->section != Entry::VARIABLE_SEC && ce->section != Entry::FUNCTION_SEC)
        continue;

      //cout<<ce->name<<", "<<mdfsMap.contains(ce->name.lower())<<mdfsMap.count()<<endl;
      if(mdfsMap.contains(ce->name.lower()))
        applyModifiers(ce, mdfsMap[ce->name.lower()]);
    }
  }

  // clear all modifiers of the scope
  modifiers.remove(scope);

  return TRUE;
}

//! Return full name of the entry. Sometimes we must combine several names recursively.
static QString getFullName(Entry *e) 
{
  QString name = e->name;
  if(e->section == Entry::CLASS_SEC //  || e->section == Entry::INTERFACE_SEC
     || !e->parent() || e->parent()->name.isEmpty())
    return name;

  return getFullName(e->parent())+"::"+name;
}

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
  initGroupInfo(current);
}

/**
  adds current entry to current_root and creates new current
*/
static void addCurrentEntry()
{
  //cout << "Adding entry " <<current->name.data() << endl;
  current_root->addSubEntry(current);
  last_entry = current;
  current = new Entry ;
  initEntry();
}

/*! Adds interface to the root entry.
 * \note Code was brought to this procedure from the parser,
 * because there was/is idea to use it in several parts of the parser.
 */ 
static void addInterface(QString name) 
{
  current->section = Entry::CLASS_SEC; // was Entry::INTERFACE_SEC;
  current->spec = Entry::Interface;
  current->name = name;

  /* if type is part of a module, mod name is necessary for output */
  if ((current_root) && 
      (current_root->section ==  Entry::CLASS_SEC ||
       current_root->section ==  Entry::NAMESPACE_SEC)) 
  {
    current->name= current_root->name+"::"+current->name;
  }
  if ((current_root) &&
      (current_root->section ==  Entry::FUNCTION_SEC)) 
  {
    current->name = getFullName(current_root) + "__" + QString(current->name);
  }

  current->fileName = yyFileName;
  current->bodyLine  = yyLineNr; 
  addCurrentEntry();
}


//-----------------------------------------------------------------------------

/*! Update the argument \a name with additional \a type info. 
 */
static Argument *addFortranParameter(const QCString &type,const QCString &name, const QString docs)
{
  //cout<<"addFortranParameter(): "<<name<<" DOCS:"<<(docs.isNull()?QString("null"):docs)<<endl;
  Argument *ret = 0;
  if (current_root->argList==0) return FALSE;
  ArgumentListIterator ali(*current_root->argList);
  Argument *a;
  for (ali.toFirst();(a=ali.current());++ali)
  {
    if (a->type.lower()==name.lower())
    {
      ret=a;
//cout << "addParameter found:   " << type << " , " << name << endl;
      a->type=type.stripWhiteSpace();
      a->name=name.stripWhiteSpace();
      if(!docs.isNull())
        a->docs = docs;
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
  while (parseCommentBlock(
	g_thisParser,
	docBlockInBody ? last_entry : current,
	doc,        // text
	yyFileName, // file
	brief ? current->briefLine : current->docLine,   // line of block start
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

  //anonCount     = 0;  // don't reset per file
  mtype         = Method;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  global_root   = rt;
  inputFile.setName(fileName);
  if (inputFile.open(IO_ReadOnly))
  {
    yyLineNr= 1 ; 
    yyFileName = fileName;
    msg("Parsing file %s...\n",yyFileName.data());

    current_root  = rt ;
    initParser();
    groupEnterFile(yyFileName,yyLineNr);

    current          = new Entry;
    current->name    = yyFileName;
    current->section = Entry::SOURCE_SEC;
    current_root->addSubEntry(current);
    file_root        = current;
    current          = new Entry;

    fscanYYrestart( fscanYYin );
    {
      BEGIN( Start );
    }

    fscanYYlex();
    groupLeaveFile(yyFileName,yyLineNr);

    //debugCompounds(rt); //debug 

    rt->program.resize(0);
    delete current; current=0;
    moduleProcedures.clear();

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
		   MemberDef *memberDef
                  )
{
  ::parseFortranCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                     fileDef,startLine,endLine,inlineFragment,memberDef);
}

bool FortranLanguageScanner::needsPreprocessing(const QCString &extension)
{
  (void)extension;
  return TRUE;
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

