/******************************************************************************
 *
 * Parser for syntax hightlighting and references for Fortran90 F subset
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

/**
 @todo - continutation lines not always recognized
       - merging of use-statements with same module name and different only-names
       - rename part of use-statement
       - links to interface functions 
       - references to variables
**/

%{

/*
 *	includes
 */
#include "qtbc.h"
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>
#include <qdir.h>
#include <qstringlist.h>
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "defargs.h"

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_TOP_STATE 1
   
//--------------------------------------------------------------------------------

/**
  data of an use-statement
*/
class UseEntry 
{
 public: 
   QCString module; // just for debug
   QStringList onlyNames;   /* entries of the ONLY-part */
};

/**
  module name -> list of ONLY/remote entries
  (module name = name of the module, which can be accessed via use-directive)
*/
class UseSDict : public SDict<UseEntry> 
{
  public:
    UseSDict() : SDict<UseEntry>(17) {}
};

/*===================================================================*/
/* 
 *	statics
 */
  
static QCString  docBlock;                   //!< contents of all lines of a documentation block
static QCString  currentModule=0;            //!< name of the current enclosing module
static UseSDict  *useMembers= new UseSDict;  //!< info about used modules
static UseEntry  *useEntry = 0;              //!< current use statement info
static QStack<QStringList> useStack;         //!< contains names of used modules for current program tree
static QStringList *currentUseNames= new QStringList; //! contains names of used modules of current program unit
static QCString str="";         //!> contents of fortran string

static CodeOutputInterface * g_code;

// TODO: is this still needed? if so, make it work
static QCString      g_parmType;
static QCString      g_parmName;

static const char *  g_inputString;     //!< the code fragment as text
static int	     g_inputPosition;   //!< read offset during parsing 
static int           g_inputLines;      //!< number of line in the code fragment
static int	     g_yyLineNr;        //!< current line number
static bool          g_needsTermination;

static bool          g_insideBody;      //!< inside subprog/program body? => create links
static const char *  g_currentFontClass;

static bool          g_exampleBlock;
static QCString      g_exampleName;
static QCString      g_exampleFile;

static FileDef *     g_sourceFileDef;
static Definition *  g_currentDefinition;
static MemberDef *   g_currentMemberDef;
static bool          g_includeCodeFragment;

static char          stringStartSymbol; // single or double quote

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

static void setCurrentDoc(const QCString &name,const QCString &base,const QCString &anchor="")
{
  static bool searchEngineEnabled=Config_getBool("SEARCHENGINE");
  if (searchEngineEnabled)
  {
    Doxygen::searchIndex->setCurrentDoc(name,base,anchor);
  }
}

static void addToSearchIndex(const char *text)
{
  static bool searchEngineEnabled=Config_getBool("SEARCHENGINE");
  if (searchEngineEnabled)
  {
    Doxygen::searchIndex->addWord(text,FALSE);
  }
}

/*! start a new line of code, inserting a line number if g_sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine()
{
  if (g_sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",g_yyLineNr);
    //lineAnchor.sprintf("l%05d",g_yyLineNr);
   
    Definition *d   = g_sourceFileDef->getSourceDefinition(g_yyLineNr);
    //printf("startCodeLine %d d=%s\n", g_yyLineNr,d ? d->name().data() : "<null>");
    if (!g_includeCodeFragment && d)
    {
      g_currentDefinition = d;
      g_currentMemberDef = g_sourceFileDef->getSourceMember(g_yyLineNr);
      g_insideBody = FALSE;
      g_parmType.resize(0);
      g_parmName.resize(0);
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",g_yyLineNr);
      if (g_currentMemberDef)
      {
        g_code->writeLineNumber(g_currentMemberDef->getReference(),
	                        g_currentMemberDef->getOutputFileBase(),
	                        g_currentMemberDef->anchor(),g_yyLineNr);
        setCurrentDoc(
                                g_currentMemberDef->qualifiedName(),
	                        g_sourceFileDef->getSourceFileBase(),
	                        lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        g_code->writeLineNumber(d->getReference(),
	                        d->getOutputFileBase(),
	                        0,g_yyLineNr);
        setCurrentDoc(
                                d->qualifiedName(),
	                        g_sourceFileDef->getSourceFileBase(),
	                        lineAnchor);
      }
    }
    else
    {
      g_code->writeLineNumber(0,0,0,g_yyLineNr);
    }
  }
  g_code->startCodeLine(); 
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
}


static void endFontClass();
static void endCodeLine()
{
  if (g_currentFontClass) { g_code->endFontClass(); }
  g_code->endCodeLine();
}

/*! write a code fragment `text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(char *text)
{
  //printf("codifyLines(%d,\"%s\")\n",g_yyLineNr,text);
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
      endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine();
      }
    }
    else
    {
      g_code->codify(sp);
      done=TRUE;
    }
  }
}

static void codifyLines(QCString str)
{
  char *tmp= (char *) malloc(str.length()+1);
  strcpy(tmp, str);
  codifyLines(tmp);
  free(tmp);
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be 
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(CodeOutputInterface &ol,
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
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,0);
      endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine();
      }
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,0);
      done=TRUE;
    }
  }
}

/**
  generates dictionay entries that are used if REFERENCED_BY_RELATION ... options are set
  (e.g. the "referenced by ..." list after the function documentation) 
*/

static void addDocCrossReference(MemberDef *src, MemberDef *dst)
{
  if (dst->isTypedef() || dst->isEnumerate()) return; // don't add types
 //printf("======= addDocCrossReference src=%s,dst=%s\n",src->name().data(),dst->name().data());
  if ((Config_getBool("REFERENCED_BY_RELATION") || Config_getBool("CALLER_GRAPH")) && 
      (src->isFunction()))
  {
    dst->addSourceReferencedBy(src);
  }
  if ((Config_getBool("REFERENCES_RELATION") || Config_getBool("CALL_GRAPH")) && (src->isFunction()))
  {
    src->addSourceReferences(dst);
  }
}

//-------------------------------------------------------------------------------
/**
  searches for definition of a type
  @param memberName the name of the type
  @param moduleName name of enclosing module or null, if global entry
  @param cd the entry, if found or null
  @param useList array of data of USE-statement
  @returns true, if type is found 
*/
static bool getFortranTypeDefs(const QCString &tname, const QCString &moduleName, 
                               ClassDef *&cd, UseSDict *usedict=0)
{
  if (tname.isEmpty()) return FALSE; /* empty name => nothing to link */

  //cout << "=== search for type: " << tname << endl;

  // search for type  
  if ((cd=Doxygen::classSDict->find(tname))) 
  {
    //cout << "=== type found in global module" << endl;
    return TRUE;
  }
  else if (moduleName && (cd= Doxygen::classSDict->find(moduleName+"::"+tname))) 
  {
    //cout << "=== type found in local module" << endl;
    return TRUE;
  }
  else 
  {
    UseEntry *use;
    for (UseSDict::Iterator di(*usedict); (use=di.current()); ++di)
      if ((cd= Doxygen::classSDict->find(use->module+"::"+tname))) 
      {
 	//cout << "===  type found in used module" << endl;
        return TRUE;
      }
  }

  return FALSE;
}

/**
  searches for definition of function memberName
  @param memberName the name of the function/variable
  @param moduleName name of enclosing module or null, if global entry
  @param memberDef the entry, if found or null
  @param useList array of data of USE-statement
  @returns true, if found 
*/
static bool getFortranDefs(const QCString &memberName, const QCString &moduleName, 
                           MemberDef *&md, UseSDict *usedict=0)
{
  if (memberName.isEmpty()) return FALSE; /* empty name => nothing to link */

  // search for function
  MemberName *mn = Doxygen::functionNameSDict->find(memberName);

  if (mn) // name is known
  {
      MemberListIterator mli(*mn);
      for (mli.toFirst();(md=mli.current());++mli) // all found functions with given name
      {
        FileDef  *fd=md->getFileDef();
        GroupDef *gd=md->getGroupDef();

 //cout << "found link with same name: " << fd->fileName() << "  " <<  memberName;
 //if (md->getNamespaceDef() != 0) cout << " in namespace " << md->getNamespaceDef()->name();cout << endl;

        if ((gd && gd->isLinkable()) || (fd && fd->isLinkable()))
        {
           NamespaceDef *nspace= md->getNamespaceDef();

           if (nspace == 0) 
	   { // found function in global scope
             return TRUE;
           }
           else if (moduleName == nspace->name()) 
	   { // found in local scope
             return TRUE;
           }
           else 
	   { // else search in used modules
	     QCString moduleName= nspace->name();
	     UseEntry *ue= usedict->find(moduleName);
	     if (ue) 
	     {
               // check if only-list exists and if current entry exists is this list
	       QStringList &only= ue->onlyNames;
	       if (only.isEmpty()) 
	       {
               //cout << " found in module " << moduleName << " entry " << memberName <<  endl;
                 return TRUE; // whole module used
               }
               else
	       {
	         for ( QStringList::Iterator it = only.begin(); it != only.end(); ++it)
                 {
                   //cout << " search in only: " << moduleName << ":: " << memberName << "==" << (*it)<<  endl;
		   if (memberName == (QCString)(*it))
                     return TRUE; // found in ONLY-part of use list
	         }
	       }
             }
           }
        } // if linkable
      } // for
  }
  return FALSE;
}

/**
 gets the link to a generic procedure which depends not on the name, but on the parameter list
 @todo implementation
*/
static bool getGenericProcedureLink(const ClassDef *cd, 
                                    const char *memberText, 
				    CodeOutputInterface &ol) 
{
  (void)cd;
  (void)memberText;
  (void)ol;
  return FALSE;
}

static bool getLink(UseSDict *usedict, // dictonary with used modules
                    const char *memberText,  // exact member text
		    CodeOutputInterface &ol,
		    const char *text)
{
  MemberDef *md;
  QCString memberName= removeRedundantWhiteSpace(memberText);
  //printf("Trying `%s'::`%s'\n",c.data(),m.data());

  if (getFortranDefs(memberName, currentModule, md, usedict) && md->isLinkable())
  { 
    //if (md->isVariable()) return FALSE; // variables aren't handled yet	

    Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	            md->getBodyDef() : md->getOuterScope();
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      if (g_currentDefinition && g_currentMemberDef && md!=g_currentMemberDef && g_insideBody)
      { 
	addDocCrossReference(g_currentMemberDef,md); 
      }     
      ol.linkableSymbol(g_yyLineNr,md->name(),md,
	                g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
      writeMultiLineCodeLink(ol,md->getReference(),
	                        md->getOutputFileBase(),
	                        md->anchor(),
				text ? text : memberText);
      addToSearchIndex(text ? text : memberText);
      return TRUE;
    } 
  }
  return FALSE;
}


static void generateLink(CodeOutputInterface &ol, char *lname)
{
  ClassDef *cd=0;
 
  // check if lname is a linkable type or interface
  if ( (getFortranTypeDefs(lname, currentModule, cd, useMembers)) && cd->isLinkable() )
  {
    if ( (cd->compoundType() == ClassDef::Class) && // was  Entry::INTERFACE_SEC) &&
         (getGenericProcedureLink(cd, lname, ol)) ) 
    {
      //cout << "=== generic procedure resolved" << endl; 
    } 
    else 
    { // write type or interface link
      ol.linkableSymbol(g_yyLineNr, lname, cd, g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
      writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),0,lname);
      addToSearchIndex(lname);
    }
  }
  // check for function/variable
  else if (getLink(useMembers, lname, ol, lname)) 
  {
    //cout << "=== found link for " << lname << endl;
  }
  else 
  {
    // nothing found, just write out the word
    ol.linkableSymbol(g_yyLineNr, lname, 0, g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
    //startFontClass("charliteral"); //test
    codifyLines(lname);
    //endFontClass(); //test
    addToSearchIndex(lname);
  }
}

/*! counts the number of lines in the input */
static int countLines()
{
  const char *p=g_inputString;
  char c;
  int count=1;
  while ((c=*p)) 
  { 
    p++ ; 
    if (c=='\n') count++;  
  }
  if (p>g_inputString && *(p-1)!='\n') 
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++, 
    g_needsTermination=TRUE; 
  } 
  return count;
}

//----------------------------------------------------------------------------
/** start use list */
void startUseScope() 
{
  //fprintf("===> startUse %s",yytext);
  useStack.push(currentUseNames);
  currentUseNames= new QStringList;
}

/** end use list */
void endUseScope() 
{
  //fprintf(stderr,"===> endUse %s",yytext);
  //UseSDict::Iterator di(*useMembers); UseEntry *ue;
  //for (di.toFirst(); ue=di.current(); ++di){cout << ue->module ;} cout << endl;

  for ( QStringList::Iterator it = currentUseNames->begin(); it != currentUseNames->end(); ++it) 
  {
    useMembers->remove(*it);
  }
  delete currentUseNames;
  currentUseNames= useStack.pop(); 
  if (currentUseNames == 0) 
  {
     fprintf(stderr,"fortrancode.l: stack empty!"); 
     //exit(-1);
  }
}

void addUse(QString moduleName) 
{
  currentUseNames->append(moduleName);
}
//----------------------------------------------------------------------------

/* -----------------------------------------------------------------*/
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

IDSYM	  [a-z_A-Z0-9]
ID        [a-z_A-Z]+{IDSYM}*
SUBPROG   (subroutine|function)
B         [ \t]
BS        [ \t]*
BS_       [ \t]+
COMMA     {BS},{BS}
ARGS      {BS}("("[^)]*")"){BS}

NUM_TYPE  (complex|integer|logical|real)
KIND      {ARGS}
CHAR      (CHARACTER{ARGS}?|CHARACTER{BS}"*"([0-9]+|{ARGS}))
TYPE_SPEC (({NUM_TYPE}("*"[0-9]+)?)|({NUM_TYPE}{KIND})|DOUBLE{BS}PRECISION|{CHAR})

INTENT_SPEC intent{BS}"("{BS}(in|out|in{BS}out){BS}")"
ATTR_SPEC (ALLOCATABLE|DIMENSION{ARGS}|EXTERNAL|{INTENT_SPEC}|INTRINSIC|OPTIONAL|PARAMETER|POINTER|PRIVATE|PUBLIC|SAVE|TARGET)
ACCESS_SPEC (PRIVATE|PUBLIC)
/* Assume that attribute statements are almost the same as attributes. */
ATTR_STMT {ATTR_SPEC}|DIMENSION
COMMANDS  (BLOCK{BS}DATA|DO|SELECT|CASE|WHERE|IF|THEN|ELSE|MODULE{BS_}PROCEDURE)
IGNORE (IMPLICIT{BS}NONE|CONTAINS|WRITE|READ|ALLOCATE|DEALLOCATE|SIZE)

/* |  */

%option noyywrap
%option stack
%option caseless
/*%option debug*/

%x Start
%x SubCall
%x FuncDef
%x ClassName
%x ClassVar
%x Subprog
%x DocBlock
%x Use
%x UseOnly
%x TypeDecl
%x Declaration
%x DeclContLine
%x Parameterlist
%x String

%%
 /*==================================================================*/

 /*-------- ignore ------------------------------------------------------------*/

<Start>{IGNORE}/{BS}"("?                { // do not search keywords, intrinsics... TODO: complete list
  					  codifyLines(yytext);
                                        }
 /*-------- inner construct ---------------------------------------------------*/
 
<Start>{COMMANDS}/[,( \t\n].*           { // hightlight rest of fortran statements
   					  /* font class is defined e.g. in doxygen.css */
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					}
<Start>"end"{BS}{COMMANDS}/[ \t\n].*    { 
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					}

 /*-------- use statement -------------------------------------------*/
<Start>"use"{BS_}                       { 
  					  codifyLines(yytext);
                                          yy_push_state(YY_START);
					  BEGIN(Use);     
                                        }
<Use>{ID}                               {
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();

					  /* append module name to use dict */
                                          useEntry = new UseEntry();
					  useEntry->module = yytext;
                                          useMembers->append(yytext, useEntry);
					  addUse(yytext);
                                        }           
<Use>,{BS}"ONLY"                        { // TODO: rename
 					  codifyLines(yytext);
                                          yy_push_state(YY_START);
					  BEGIN(UseOnly);     
                                        }           
<UseOnly>{BS},{BS}                      { codifyLines(yytext); }
<UseOnly>{ID}                           {
 					  codifyLines(yytext);
                                          useEntry->onlyNames.append(yytext);
                                        }   
<Use,UseOnly>"\n"                       {
                                          unput(*yytext);
                                          yy_pop_state();
                                        }
       
 /*-------- fortran module  -----------------------------------------*/
<Start>("program"|"module"|"type"|"interface")/{BS_}|({COMMA}{ACCESS_SPEC})|\n {  //
					  startUseScope();
  					  startFontClass("keyword"); 
  					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
					  BEGIN(ClassName); 
	                                  if (!strcmp(yytext,"module")) currentModule="module";
					}
<ClassName>{ID}               	        {
	                                  if (currentModule == "module") currentModule=yytext;
					  generateLink(*g_code,yytext);
                                          yy_pop_state();
 					}
<ClassName>\n				{ // interface may be without name
                                          yy_pop_state();
					  REJECT;
					}
<Start>"end"{BS}"module".*              { // just reset currentModule, rest is done in following rule
                                          currentModule=0;
					  REJECT;
                                        }
<Start>"end"{BS}("program"|"module"|"type"|"interface") {  //
                                          endUseScope();
 					  startFontClass("keyword"); 
  					  codifyLines(yytext);
					  endFontClass();
                                        }

 /*-------- subprog definition -------------------------------------*/
<Start>{TYPE_SPEC}{BS}/{SUBPROG}{BS_}  {   // TYPE_SPEC is for old function style function result
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                       }              
<Start>{SUBPROG}{BS_}                  {  // Fortran subroutine or function found
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
                                          BEGIN(Subprog);
                                        }
<Subprog>{ID}                           { // subroutine/function name
                                          //cout << "===> start procedure " << yytext << endl;
					  startUseScope();
					  generateLink(*g_code,yytext);
                                        }
<Subprog>"(".*                          { // ignore rest of line 
 					  codifyLines(yytext);
                                        }
<Subprog>"\n"                           { codifyLines(yytext);
                                          yy_pop_state();
                                        }
<Start>"end"{BS}{SUBPROG}.*             {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endUseScope();
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                        }
 /*-------- variable declaration ----------------------------------*/
<Start>"TYPE"{BS}"("                    {
                                          yy_push_state(YY_START);
					  BEGIN(TypeDecl);
   					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
                                        }
<TypeDecl>{ID}                          { // link type
					  g_insideBody=TRUE;
					  generateLink(*g_code,yytext);
					  g_insideBody=FALSE;
                                        }
<TypeDecl>")"                           { 
					  BEGIN(Declaration);
   					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
                                        }
<Start>{TYPE_SPEC}/[,:( ]               { 
                                          yy_push_state(YY_START);
					  BEGIN(Declaration);
   					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
                                       }
<Start>{ATTR_SPEC}		       { 
   					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
                                       }
<Declaration>({TYPE_SPEC}|{ATTR_SPEC})/[,:( ] { //| variable deklaration
  					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
  					}
<Declaration>"&"                        { // continuation line
                                          yy_push_state(YY_START);
					  BEGIN(DeclContLine);					  
 					}
<DeclContLine>"\n"                      { // declaration not yet finished
                                          codifyLines(yytext);
                                          yy_pop_state();
 				 	}
<Declaration>"\n"                       { // end declaration line
					  codifyLines(yytext);
                                          yy_pop_state();
 					}

 /*-------- subprog calls  -----------------------------------------*/

<Start>"call"{BS_}                      {
  					  codifyLines(yytext);
                                          yy_push_state(YY_START);
					  BEGIN(SubCall);
                                        }
<SubCall>{ID}                           { // subroutine call
					  g_insideBody=TRUE;
                                          generateLink(*g_code, yytext);
					  g_insideBody=FALSE;
	                                  yy_pop_state();
                                        }
<Start>{ID}{BS}/"("                     { // function call
					  g_insideBody=TRUE;
                                          generateLink(*g_code, yytext);
					  g_insideBody=FALSE;
                                        }

 /*-------- comments ---------------------------------------------------*/
<Start>\n?{BS}"!>"                      { // start comment line or comment block
                                          yy_push_state(YY_START);
					  BEGIN(DocBlock);
                                          docBlock=yytext;
					}

<DocBlock>.*    			{ // contents of current comment line
                                          docBlock+=yytext;
  					}
<DocBlock>"\n"{BS}("!>"|"!"+)		{ //| comment block (next line is also comment line)
					  docBlock+=yytext; 
   					}
<DocBlock>"\n"        			{ // comment block ends at the end of this line
 					  docBlock+=yytext; 
                                          // remove special comment (default config)
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr+=((QCString)docBlock).contains('\n');
					    endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine();
					    }
					  }
					  else // do not remove comment
					  {
					    startFontClass("comment");
					    codifyLines(docBlock);
					    endFontClass();
					  }
                                         yy_pop_state();
					}

<*>"!"[^>\n].*|"!"$ 			{ // normal comment
					  if(YY_START == String) REJECT; // ignore in strings
  					  startFontClass("comment");
  					  codifyLines(yytext);
					  endFontClass();
					}
 /*------ preprocessor  --------------------------------------------*/ 
<Start>"#".*\n                          { startFontClass("preprocessor");
  					  codifyLines(yytext);
					  endFontClass();
                                        }
 /*------ variable references?  -------------------------------------*/ 
<Start>{ID}                             {   
  					    g_insideBody=TRUE;
                                            generateLink(*g_code, yytext);
					    g_insideBody=FALSE;
                                        }
 /*------ strings --------------------------------------------------*/ 
<*>"\\\\"                               { str+=yytext; /* ignore \\  */}
<*>"\\\""|\\\'                          { str+=yytext; /* ignore \"  */}

<String>\"|\'                           { // string ends with next quote without previous backspace 
                                          if(yytext[0]!=stringStartSymbol) REJECT; // single vs double quote
                                          str+=yytext;
  					  startFontClass("stringliteral");
  					  codifyLines(str);
					  endFontClass();
                                          yy_pop_state();
                                        }           
<*>\"|\'                                { /* string starts */
					  /* if(YY_START == StrIgnore) REJECT; // ignore in simple comments */
                                          yy_push_state(YY_START);
                                          stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String);
					  str=yytext;
                                        }
<String>.                               {str+=yytext;}
 /*-----------------------------------------------------------------------------*/

<*>\n					{
  					  codifyLines(yytext); 
  					}
<*>.					{ 
  					  g_code->codify(yytext);
					}
%%

/*@ ----------------------------------------------------------------------------
 */

/*===================================================================*/

void resetFortranCodeParserState() {}

void parseFortranCode(CodeOutputInterface &od,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
		  int startLine,int endLine,bool inlineFragment,
		  MemberDef *memberDef)
{
  //printf("***parseCode() exBlock=%d exName=%s fd=%p\n",exBlock,exName,fd);

  // used parameters
  (void)memberDef;
  (void)className;

  if (s.isEmpty()) return;
  g_code = &od;
  g_inputString   = s;
  g_inputPosition = 0;
  g_currentFontClass = 0;
  g_needsTermination = FALSE;
  if (endLine!=-1)
    g_inputLines  = endLine+1;
  else
    g_inputLines  = countLines();

  if (startLine!=-1)
    g_yyLineNr    = startLine;
  else
    g_yyLineNr    = 1;

  g_exampleBlock  = exBlock; 
  g_exampleName   = exName;
  g_sourceFileDef = fd;
  if (exBlock && fd==0)
  {
    // create a dummy filedef for the example
    g_sourceFileDef = new FileDef("",exName);
  }
  if (g_sourceFileDef) 
  {
    setCurrentDoc(g_sourceFileDef->name(),g_sourceFileDef->getSourceFileBase());
  }
  g_currentDefinition = 0;
  g_currentMemberDef = 0;
  if (!g_exampleName.isEmpty())
  {
    g_exampleFile = convertNameToFile(g_exampleName+"-example");
  }
  g_includeCodeFragment = inlineFragment;
  startCodeLine();
  g_parmName.resize(0);
  g_parmType.resize(0);
  fcodeYYrestart( fcodeYYin );
  BEGIN( Start );
  fcodeYYlex();
  if (g_needsTermination)
  {
    endFontClass();
    g_code->endCodeLine();
  }
  if (exBlock && g_sourceFileDef)
  {
    // delete the temporary file definition used for this example
    delete g_sourceFileDef;
    g_sourceFileDef=0;
  }
  return;
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
  void fcodeYYdummy() { yy_flex_realloc(0,0); } 
}
#elif YY_FLEX_SUBMINOR_VERSION<33
#error "You seem to be using a version of flex newer than 2.5.4 but older than 2.5.33. These versions do NOT work with doxygen! Please use version <=2.5.4 or >=2.5.33 or expect things to be parsed wrongly!"
#else
extern "C" { // some bogus code to keep the compiler happy
  void fcodeYYdummy() { yy_top_state(); } 
}
#endif

