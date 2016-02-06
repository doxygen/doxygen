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
%option never-interactive
%{

/*
 *	includes
 */
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
#include "memberlist.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "tooltip.h"
#include "fortrancode.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_TOP_STATE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

/*
 * For fixed formatted code position 6 is of importance (continuation character).
 * The following variables and macros keep track of the column number
 * YY_USER_ACTION is always called for each scan action
 * YY_FTN_REST    is used to handle end of lines and reset the column counter
 * YY_FTN_REJECT  resets the column counters when a pattern is rejected and thus rescanned.
 */
int yy_old_start = 0;
int yy_my_start  = 0;
int yy_end       = 1;
#define YY_USER_ACTION {yy_old_start = yy_my_start; yy_my_start = yy_end; yy_end += yyleng;}
#define YY_FTN_RESET   {yy_old_start = 0; yy_my_start = 0; yy_end = 1;}
#define YY_FTN_REJECT  {yy_end = yy_my_start; yy_my_start = yy_old_start; REJECT;}
   
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

/**
  Contains names of used modules and names of local variables.
*/
class Scope 
{
  public:
    QStringList useNames; //!< contains names of used modules
    QDict<void> localVars; //!< contains names of local variables

    Scope() : localVars(7, FALSE /*caseSensitive*/) {}
};

/*===================================================================*/
/* 
 *	statics
 */
  
static QCString  docBlock;                   //!< contents of all lines of a documentation block
static QCString  currentModule=0;            //!< name of the current enclosing module
static QCString  currentClass=0;             //!< name of the current enclosing class
static UseSDict  *useMembers= new UseSDict;  //!< info about used modules
static UseEntry  *useEntry = 0;              //!< current use statement info
static QList<Scope> scopeStack;
// static QStringList *currentUseNames= new QStringList; //! contains names of used modules of current program unit
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
static Definition   *g_searchCtx;
static bool          g_collectXRefs;
static bool          g_isFixedForm;

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
// count in variable declaration to filter out
//  declared from referenced names
static int 	     bracketCount = 0;

static bool      g_endComment;

// simplified way to know if this is fixed form
// duplicate in fortranscanner.l
static bool recognizeFixedForm(const char* contents, FortranFormat format)
{
  int column=0;
  bool skipLine=FALSE;

  if (format == FortranFormat_Fixed) return TRUE;
  if (format == FortranFormat_Free)  return FALSE;
  for (int i=0;;i++)
  {
    column++;

    switch(contents[i]) 
    {
      case '\n':
        column=0;
        skipLine=FALSE;
        break;
      case ' ':
        break;
      case '#':
        skipLine=TRUE;
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
  // if font class is already set don't stop and start it.
  // strcmp does not like null pointers as input.
  if (!g_currentFontClass || !s || strcmp(g_currentFontClass,s))
  {
    endFontClass();
    g_code->startFontClass(s);
    g_currentFontClass=s;
  }
}

static void setCurrentDoc(const QCString &anchor)
{
  if (Doxygen::searchIndex)
  {
    if (g_searchCtx)
    {
      Doxygen::searchIndex->setCurrentDoc(g_searchCtx,g_searchCtx->anchor(),FALSE);
    }
    else
    {
      Doxygen::searchIndex->setCurrentDoc(g_sourceFileDef,anchor,TRUE);
    }
  }
}

static void addToSearchIndex(const char *text)
{
  if (Doxygen::searchIndex)
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
      g_endComment = FALSE;
      g_parmType.resize(0);
      g_parmName.resize(0);
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",g_yyLineNr);
      if (g_currentMemberDef)
      {
        g_code->writeLineNumber(g_currentMemberDef->getReference(),
	                        g_currentMemberDef->getOutputFileBase(),
	                        g_currentMemberDef->anchor(),g_yyLineNr);
        setCurrentDoc(lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        g_code->writeLineNumber(d->getReference(),
	                        d->getOutputFileBase(),
	                        0,g_yyLineNr);
        setCurrentDoc(lineAnchor);
      }
    }
    else
    {
      g_code->writeLineNumber(0,0,0,g_yyLineNr);
    }
  }
  g_code->startCodeLine(g_sourceFileDef); 
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
}


static void endFontClass();
static void endCodeLine()
{
  endFontClass();
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
  const char *  tmp_currentFontClass = g_currentFontClass;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
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
      if (tmp_currentFontClass)
      {
        startFontClass(tmp_currentFontClass);
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
                  Definition *d,const char *text)
{
  static bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  TooltipManager::instance()->addTooltip(d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip; 
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
    tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=FALSE;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine();
      }
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}
//-------------------------------------------------------------------------------
/**
  searches for definition of a module (Namespace)
  @param mname the name of the module
  @param cd the entry, if found or null
  @returns true, if module is found
*/
static bool getFortranNamespaceDefs(const QCString &mname,
                               NamespaceDef *&cd)
{
  if (mname.isEmpty()) return FALSE; /* empty name => nothing to link */

  // search for module
  if ((cd=Doxygen::namespaceSDict->find(mname))) return TRUE;

  return FALSE;
}
//-------------------------------------------------------------------------------
/**
  searches for definition of a type
  @param tname the name of the type
  @param moduleName name of enclosing module or null, if global entry
  @param cd the entry, if found or null
  @param useDict dictionary of data of USE-statement
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
    {
      if ((cd= Doxygen::classSDict->find(use->module+"::"+tname)))
      {
 	//cout << "===  type found in used module" << endl;
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  searches for definition of function memberName
  @param memberName the name of the function/variable
  @param moduleName name of enclosing module or null, if global entry
  @param md the entry, if found or null
  @param usedict array of data of USE-statement
  @returns true, if found 
*/
static bool getFortranDefs(const QCString &memberName, const QCString &moduleName, 
                           MemberDef *&md, UseSDict *usedict=0)
{
  if (memberName.isEmpty()) return FALSE; /* empty name => nothing to link */

  // look in local variables
  QListIterator<Scope> it(scopeStack);
  Scope *scope;
  for (it.toLast();(scope=it.current());--it)
  {
    if (scope->localVars.find(memberName))
      return FALSE;
  }

  // search for function
  MemberName *mn = Doxygen::functionNameSDict->find(memberName);
  if (!mn)
  {
    mn = Doxygen::memberNameSDict->find(memberName);
  }

  if (mn) // name is known
  {
      MemberListIterator mli(*mn);
      for (mli.toFirst();(md=mli.current());++mli) // all found functions with given name
      {
        FileDef  *fd=md->getFileDef();
        GroupDef *gd=md->getGroupDef();
        ClassDef *cd=md->getClassDef();

 //cout << "found link with same name: " << fd->fileName() << "  " <<  memberName;
 //if (md->getNamespaceDef() != 0) cout << " in namespace " << md->getNamespaceDef()->name();cout << endl;

        if ((gd && gd->isLinkable()) || (fd && fd->isLinkable()))
        {
           NamespaceDef *nspace= md->getNamespaceDef();

           if (nspace == 0) 
	   { // found function in global scope
             if(cd == 0) { // Skip if bound to type
                return TRUE;
              }
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
		   if (memberName == (*it).utf8())
	           {
                     return TRUE; // found in ONLY-part of use list
	           }
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
  MemberDef *md=0;
  QCString memberName= removeRedundantWhiteSpace(memberText);

  if (getFortranDefs(memberName, currentModule, md, usedict) && md->isLinkable())
  { 
    //if (md->isVariable()) return FALSE; // variables aren't handled yet	

    Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	            md->getBodyDef() : md->getOuterScope();
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      if (g_currentDefinition && g_currentMemberDef && 
          md!=g_currentMemberDef && g_insideBody && g_collectXRefs)
      { 
	addDocCrossReference(g_currentMemberDef,md); 
      }     
      writeMultiLineCodeLink(ol,md,text ? text : memberText);
      addToSearchIndex(text ? text : memberText);
      return TRUE;
    } 
  }
  return FALSE;
}


static void generateLink(CodeOutputInterface &ol, char *lname)
{
  ClassDef *cd=0;
  NamespaceDef *nsd=0;
  QCString tmp = lname;
  tmp = removeRedundantWhiteSpace(tmp.lower());
 
  // check if lowercase lname is a linkable type or interface
  if ( (getFortranTypeDefs(tmp, currentModule, cd, useMembers)) && cd->isLinkable() )
  {
    if ( (cd->compoundType() == ClassDef::Class) && // was  Entry::INTERFACE_SEC) &&
         (getGenericProcedureLink(cd, tmp, ol)) ) 
    {
      //cout << "=== generic procedure resolved" << endl; 
    } 
    else 
    { // write type or interface link
      writeMultiLineCodeLink(ol,cd,tmp);
      addToSearchIndex(tmp.data());
    }
  }
  // check for module
  else if ( (getFortranNamespaceDefs(tmp, nsd)) && nsd->isLinkable() )
  { // write module link
    writeMultiLineCodeLink(ol,nsd,tmp);
    addToSearchIndex(tmp.data());
  }
  // check for function/variable
  else if (getLink(useMembers, tmp, ol, tmp)) 
  {
    //cout << "=== found link for lowercase " << lname << endl;
  }
  else 
  {
    // nothing found, just write out the word
    //startFontClass("charliteral"); //test
    codifyLines(tmp);
    //endFontClass(); //test
    addToSearchIndex(tmp.data());
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
/** start scope */
static void startScope() 
{
  DBG_CTX((stderr, "===> startScope %s",yytext));
  Scope *scope = new Scope;
  scopeStack.append(scope);
}

/** end scope */
static void endScope() 
{
  DBG_CTX((stderr,"===> endScope %s",yytext));
  if (scopeStack.isEmpty()) 
  {
    DBG_CTX((stderr,"WARNING: fortrancode.l: stack empty!\n")); 
    return;
  }

  Scope *scope = scopeStack.getLast();
  scopeStack.removeLast();
  for ( QStringList::Iterator it = scope->useNames.begin(); it != scope->useNames.end(); ++it) 
  {
    useMembers->remove((*it).utf8());
  }
  delete scope;
}

static void addUse(const QCString &moduleName) 
{
  if (!scopeStack.isEmpty())
    scopeStack.getLast()->useNames.append(moduleName);
}

static void addLocalVar(const QCString &varName) 
{
  if (!scopeStack.isEmpty())
    scopeStack.getLast()->localVars.insert(varName, (void*)1);
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
ARGS_L0   ("("[^)]*")")
ARGS_L1a  [^()]*"("[^)]*")"[^)]*
ARGS_L1   ("("{ARGS_L1a}*")")
ARGS_L2   "("({ARGS_L0}|[^()]|{ARGS_L1a}|{ARGS_L1})*")"
ARGS      {BS}({ARGS_L0}|{ARGS_L1}|{ARGS_L2})

NUM_TYPE  (complex|integer|logical|real)
LOG_OPER  (\.and\.|\.eq\.|\.eqv\.|\.ge\.|\.gt\.|\.le\.|\.lt\.|\.ne\.|\.neqv\.|\.or\.|\.not\.)
KIND      {ARGS}
CHAR      (CHARACTER{ARGS}?|CHARACTER{BS}"*"({BS}[0-9]+|{ARGS}))
TYPE_SPEC (({NUM_TYPE}({BS}"*"{BS}[0-9]+)?)|({NUM_TYPE}{KIND})|DOUBLE{BS}COMPLEX|DOUBLE{BS}PRECISION|{CHAR}|TYPE|CLASS|PROCEDURE)

INTENT_SPEC intent{BS}"("{BS}(in|out|in{BS}out){BS}")"
ATTR_SPEC (IMPLICIT|ALLOCATABLE|DIMENSION{ARGS}|EXTERNAL|{INTENT_SPEC}|INTRINSIC|OPTIONAL|PARAMETER|POINTER|PROTECTED|PRIVATE|PUBLIC|SAVE|TARGET|RECURSIVE|PURE|IMPURE|ELEMENTAL|VALUE|NOPASS|DEFERRED|CONTIGUOUS|VOLATILE)
ACCESS_SPEC (PROTECTED|PRIVATE|PUBLIC)
/* Assume that attribute statements are almost the same as attributes. */
ATTR_STMT {ATTR_SPEC}|DIMENSION
FLOW      (DO|SELECT|CASE|SELECT{BS}(CASE|TYPE)|WHERE|IF|THEN|ELSE|WHILE|FORALL|ELSEWHERE|ELSEIF|RETURN|CONTINUE|EXIT|GO{BS}TO)
COMMANDS  (FORMAT|CONTAINS|MODULE{BS_}PROCEDURE|WRITE|READ|ALLOCATE|ALLOCATED|ASSOCIATED|PRESENT|DEALLOCATE|NULLIFY|SIZE|INQUIRE|OPEN|CLOSE|FLUSH|DATA|COMMON)
IGNORE    (CALL)
PREFIX    (RECURSIVE{BS_}|IMPURE{BS_}|PURE{BS_}|ELEMENTAL{BS_}){0,3}(RECURSIVE|IMPURE|PURE|ELEMENTAL)?

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
%x Import
%x Declaration
%x DeclarationBinding
%x DeclContLine
%x Parameterlist
%x String
%x Subprogend

%%
 /*==================================================================*/

 /*-------- ignore ------------------------------------------------------------*/

<Start>{IGNORE}/{BS}"("                 { // do not search keywords, intrinsics... TODO: complete list
                                          codifyLines(yytext);
                                        }
 /*-------- inner construct ---------------------------------------------------*/
 
<Start>{COMMANDS}/{BS}[,( \t\n]         {  // highlight
   					  /* font class is defined e.g. in doxygen.css */
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					}
<Start>{FLOW}/{BS}[,( \t\n]               {
                                          if (g_isFixedForm)
                                          {
                                            if ((yy_my_start == 1) && ((yytext[0] == 'c') || (yytext[0] == 'C'))) YY_FTN_REJECT;
                                          }
   					  /* font class is defined e.g. in doxygen.css */
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
					}
<Start>{BS}(CASE|CLASS|TYPE){BS_}(IS|DEFAULT) {
                                          startFontClass("keywordflow");
                                          codifyLines(yytext);
                                          endFontClass();
                                        }
<Start>{BS}"end"({BS}{FLOW})/[ \t\n]       { // list is a bit long as not all have possible end
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
					}
<Start>"implicit"{BS}("none"|{TYPE_SPEC})  { 
  					  startFontClass("keywordtype"); 
  					  codifyLines(yytext);
					  endFontClass();
                                        }
<Start>^{BS}"namelist"/[//]             {  // Namelist specification
                                          startFontClass("keywordtype");
                                          codifyLines(yytext);
                                          endFontClass();
                                        }
 /*-------- use statement -------------------------------------------*/
<Start>"use"{BS_}                       { 
  					  startFontClass("keywordtype"); 
  					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
					  BEGIN(Use);     
                                        }
<Use>{ID}                               {
                                          QCString tmp = yytext;
                                          tmp = tmp.lower();
					  g_insideBody=TRUE;
                                          generateLink(*g_code, yytext);
					  g_insideBody=FALSE;

					  /* append module name to use dict */
                                          useEntry = new UseEntry();
					  //useEntry->module = yytext;
                                          //useMembers->append(yytext, useEntry);
					  //addUse(yytext);
					  useEntry->module = tmp;
                                          useMembers->append(tmp, useEntry);
					  addUse(tmp);
                                        }           
<Use>,{BS}"ONLY"                        { // TODO: rename
  					  startFontClass("keywordtype"); 
 					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
					  BEGIN(UseOnly);     
                                        }           
<UseOnly,Import>{BS},{BS}               { codifyLines(yytext); }
<UseOnly,Import>{BS}&{BS}"\n"           { codifyLines(yytext); YY_FTN_RESET}
<UseOnly>{ID}                           {
                                          QCString tmp = yytext;
                                          tmp = tmp.lower();
                                          useEntry->onlyNames.append(tmp);
                                          g_insideBody=TRUE;
                                          generateLink(*g_code, yytext);
                                          g_insideBody=FALSE;
                                        }
<Use,UseOnly,Import>"\n"                {
                                          unput(*yytext);
                                          yy_pop_state();YY_FTN_RESET
                                        }
<Start>"import"{BS_}                    {
                                          startFontClass("keywordtype");
                                          codifyLines(yytext);
                                          endFontClass();
                                          yy_push_state(YY_START);
                                          BEGIN(Import);
                                        }
<Import>{ID}                            {
                                          g_insideBody=TRUE;
                                          generateLink(*g_code, yytext);
                                          g_insideBody=FALSE;
                                        }
 /*-------- fortran module  -----------------------------------------*/
<Start>("block"{BS}"data"|"program"|"module"|"interface")/{BS_}|({COMMA}{ACCESS_SPEC})|\n {  //
					  startScope();
  					  startFontClass("keyword"); 
  					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
					  BEGIN(ClassName); 
	                                  if (!qstricmp(yytext,"module")) currentModule="module";
					}
<Start>("type")/{BS_}|({COMMA}({ACCESS_SPEC}|ABSTRACT|EXTENDS))|\n {  //
            startScope();
              startFontClass("keyword");
              codifyLines(yytext);
            endFontClass();
                                          yy_push_state(YY_START);
            BEGIN(ClassName);
            currentClass="class";
          }
<ClassName>{ID}               	        {
	                                  if (currentModule == "module")
                                          {
                                            currentModule=yytext;
                                            currentModule = currentModule.lower();
                                          }
					  generateLink(*g_code,yytext);
                                          yy_pop_state();
 					}
<ClassName>({ACCESS_SPEC}|ABSTRACT|EXTENDS)/[,:( ] { //| variable deklaration
              startFontClass("keyword");
            g_code->codify(yytext);
            endFontClass();
            }
<ClassName>\n				{ // interface may be without name
                                          yy_pop_state();
					  YY_FTN_REJECT;
					}
<Start>^{BS}"end"({BS_}"type").*        { // just reset currentClass, rest is done in following rule
                                          currentClass=0;
            YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"module").*      { // just reset currentModule, rest is done in following rule
                                          currentModule=0;
					  YY_FTN_REJECT;
                                        }
 /*-------- subprog definition -------------------------------------*/
<Start>({PREFIX}{BS_})?{TYPE_SPEC}{BS_}({PREFIX}{BS_})?{BS}/{SUBPROG}{BS_}  {   // TYPE_SPEC is for old function style function result
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                       }              
<Start>({PREFIX}{BS_})?{SUBPROG}{BS_}                  {  // Fortran subroutine or function found
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
                                          BEGIN(Subprog);
                                        }
<Subprog>{ID}                           { // subroutine/function name
                                          DBG_CTX((stderr, "===> start subprogram %s\n", yytext));
					  startScope();
					  generateLink(*g_code,yytext);
                                        }
<Subprog>"(".*                          { // ignore rest of line 
 					  codifyLines(yytext);
                                        }
<Subprog,Subprogend>"\n"                { codifyLines(yytext);
                                          yy_pop_state();
                                          YY_FTN_RESET
                                        }
<Start>^{BS}"end"{BS}("block"{BS}"data"|{SUBPROG}|"module"|"program"|"type"|"interface")?{BS}     {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endScope();
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                          yy_push_state(YY_START);
                                          BEGIN(Subprogend);
                                        }
<Subprogend>{ID}/{BS}(\n|!)             {
					  generateLink(*g_code,yytext);
                                          yy_pop_state();
                                        }
<Start>^{BS}"end"{BS}("block"{BS}"data"|{SUBPROG}|"module"|"program"|"type"|"interface"){BS}/(\n|!) {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endScope();
   					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                        }
 /*-------- variable declaration ----------------------------------*/
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
<Declaration>{ID}                       { // local var
                                          if (g_currentMemberDef && g_currentMemberDef->isFunction() && bracketCount==0)
                                          {
                                            g_code->codify(yytext);
                                            addLocalVar(yytext);
                                          }
                                           else
                                          {
                                            generateLink(*g_code, yytext);
                                          }
					}
<Declaration>{BS}("=>"|"="){BS}                        { // Procedure binding
            BEGIN(DeclarationBinding);
            g_code->codify(yytext);
          }
<DeclarationBinding>{ID}                       { // Type bound procedure link
                                          generateLink(*g_code, yytext);
                                          yy_pop_state();
          }
<Declaration>[(]			{ // start of array specification
					  bracketCount++;
					  g_code->codify(yytext);
					}

<Declaration>[)]			{ // end array specification
					  bracketCount--;
					  g_code->codify(yytext);
					}

<Declaration,DeclarationBinding>"&"                        { // continuation line
					  g_code->codify(yytext);
                                          yy_push_state(YY_START);
					  BEGIN(DeclContLine);					  
 					}
<DeclContLine>"\n"                      { // declaration not yet finished
                                          codifyLines(yytext);
					  bracketCount = 0;
                                          yy_pop_state();
                                          YY_FTN_RESET
 				 	}
<Declaration,DeclarationBinding>"\n"                       { // end declaration line
					  if (g_endComment)
            {
            g_endComment=FALSE;
            }
            else
            {
            codifyLines(yytext);
            }
					  bracketCount = 0;
                                          yy_pop_state();
                                          YY_FTN_RESET
 					}

 /*-------- subprog calls  -----------------------------------------*/

<Start>"call"{BS_}                      {
                                          startFontClass("keyword");
                                          codifyLines(yytext);
                                          endFontClass();
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
                                          if (g_isFixedForm && yy_my_start == 6)
                                          {
                                            // fixed form continuation line
                                            YY_FTN_REJECT;
                                          }
                                          else
                                          {
					    g_insideBody=TRUE;
                                            generateLink(*g_code, yytext);
					    g_insideBody=FALSE;
                                          }
                                        }

 /*-------- comments ---------------------------------------------------*/
<Start,Declaration,DeclarationBinding>\n?{BS}"!>"|"!<"                 { // start comment line or comment block
                                          if (yytext[0] == '\n')
                                          {
                                            yy_old_start = 0;
                                            yy_my_start = 1;
                                            yy_end = yyleng;
                                          }
                                          // Actually we should see if ! on position 6, can be continuation
                                          // but the chance is very unlikely, so no effort to solve it here
                                          yy_push_state(YY_START);
					  BEGIN(DocBlock);
                                          docBlock=yytext;
					}
<Declaration,DeclarationBinding>{BS}"!<"                   { // start comment line or comment block
                                          yy_push_state(YY_START);
					  BEGIN(DocBlock);
                                          docBlock=yytext;
					}

<DocBlock>.*    			{ // contents of current comment line
                                          docBlock+=yytext;
  					}
<DocBlock>"\n"{BS}("!>"|"!<"|"!!")	{ // comment block (next line is also comment line)
                                          yy_old_start = 0;
                                          yy_my_start = 1;
                                          yy_end = yyleng;
                                          // Actually we should see if ! on position 6, can be continuation
                                          // but the chance is very unlikely, so no effort to solve it here
					  docBlock+=yytext; 
   					}
<DocBlock>"\n"        			{ // comment block ends at the end of this line
                                          // remove special comment (default config)
  					  if (Config_getBool(STRIP_CODE_COMMENTS))
					  {
					    g_yyLineNr+=((QCString)docBlock).contains('\n');
              g_yyLineNr+=1;
					    endCodeLine();
					    if (g_yyLineNr<g_inputLines)
					    {
					      startCodeLine();
					    }
              g_endComment=TRUE;
					  }
					  else // do not remove comment
					  {
					    startFontClass("comment");
					    codifyLines(docBlock);
					    endFontClass();
					  }
            unput(*yytext);
                                         yy_pop_state();
                                          YY_FTN_RESET
					}

<*>"!"[^><\n].*|"!"$ 			{ // normal comment
					  if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          if (g_isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
  					  startFontClass("comment");
  					  codifyLines(yytext);
					  endFontClass();
					}

<*>^[Cc*].*              		{ // normal comment
                                          if(! g_isFixedForm) YY_FTN_REJECT;

  					  startFontClass("comment");
  					  codifyLines(yytext);
					  endFontClass();
					}

 /*------ preprocessor  --------------------------------------------*/ 
<Start>"#".*\n                          {
                                          if (g_isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          startFontClass("preprocessor");
  					  codifyLines(yytext);
					  endFontClass();
                                          YY_FTN_RESET
                                        }
 /*------ variable references?  -------------------------------------*/ 

<Start>"%"{BS}{ID}	 		{ // ignore references to elements 
					  g_code->codify(yytext);
					}
<Start>{ID}                             {   
  					    g_insideBody=TRUE;
                                            generateLink(*g_code, yytext);
					    g_insideBody=FALSE;
                                        }
 /*------ strings --------------------------------------------------*/ 
<*>"\\\\"                               { str+=yytext; /* ignore \\  */}
<*>"\\\""|\\\'                          { str+=yytext; /* ignore \"  */}

<String>\n                              { // string with \n inside
                                          str+=yytext;
  					  startFontClass("stringliteral");
  					  codifyLines(str);
					  endFontClass();
                                          str = "";
                                          YY_FTN_RESET
                                        }           
<String>\"|\'                           { // string ends with next quote without previous backspace 
                                          if(yytext[0]!=stringStartSymbol) YY_FTN_REJECT; // single vs double quote
                                          str+=yytext;
  					  startFontClass("stringliteral");
  					  codifyLines(str);
					  endFontClass();
                                          yy_pop_state();
                                        }           
<String>.                               {str+=yytext;}

<*>\"|\'                                { /* string starts */
					  /* if(YY_START == StrIgnore) YY_FTN_REJECT; // ignore in simple comments */
                                          if (g_isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          yy_push_state(YY_START);
                                          stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String);
					  str=yytext;
                                        }
 /*-----------------------------------------------------------------------------*/

<*>\n					{
  					if (g_endComment)
            {
            g_endComment=FALSE;
            }
            else
            {
            codifyLines(yytext);
            }
                                          YY_FTN_RESET
  					}
<*>^{BS}"type"{BS}"="                   { g_code->codify(yytext); }

<*>.                                    { 
                                          if (g_isFixedForm && yy_my_start > fixedCommentAfter)
                                          {
                                            //yy_push_state(YY_START);
                                            //BEGIN(DocBlock);
                                            //docBlock=yytext;
                                            startFontClass("comment");
                                            codifyLines(yytext);
                                          }
                                          else
                                          {
                                            g_code->codify(yytext);
                                          }
                                        }
<*>{LOG_OPER}                           { // Fortran logical comparison keywords
                                          g_code->codify(yytext);
                                        }
<*><<EOF>>                              {
                                          if (YY_START == DocBlock) {
                                            if (!Config_getBool(STRIP_CODE_COMMENTS))
                                            {
                                              startFontClass("comment");
                                              codifyLines(docBlock);
                                              endFontClass();
                                            }
                                          }
                                          yyterminate();
                                        }
%%

/*@ ----------------------------------------------------------------------------
 */

/*===================================================================*/


void resetFortranCodeParserState() {}

void parseFortranCode(CodeOutputInterface &od,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
		  int startLine,int endLine,bool inlineFragment,
		  MemberDef *memberDef,bool,Definition *searchCtx,
                  bool collectXRefs, FortranFormat format)
{
  //printf("***parseCode() exBlock=%d exName=%s fd=%p\n",exBlock,exName,fd);

  // used parameters
  (void)memberDef;
  (void)className;

  if (s.isEmpty()) return;
  printlex(yy_flex_debug, TRUE, __FILE__, fd ? fd->fileName().data(): NULL);
  TooltipManager::instance()->clearTooltips();
  g_code = &od;
  g_inputString   = s;
  g_inputPosition = 0;
  g_isFixedForm = recognizeFixedForm((const char*)s,format);
  g_currentFontClass = 0;
  g_needsTermination = FALSE;
  g_searchCtx = searchCtx;
  g_collectXRefs = collectXRefs;
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
    setCurrentDoc("l00001");
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
  fortrancodeYYrestart( fortrancodeYYin );
  BEGIN( Start );
  fortrancodeYYlex();
  if (g_needsTermination)
  {
    endFontClass();
    g_code->endCodeLine();
  }
  if (fd)
  {
    TooltipManager::instance()->writeTooltips(*g_code);
  }
  if (exBlock && g_sourceFileDef)
  {
    // delete the temporary file definition used for this example
    delete g_sourceFileDef;
    g_sourceFileDef=0;
  }
  printlex(yy_flex_debug, FALSE, __FILE__, fd ? fd->fileName().data(): NULL);
  return;
}

#if !defined(YY_FLEX_SUBMINOR_VERSION)
extern "C" { // some bogus code to keep the compiler happy
  void fortrancodeYYdummy() { yy_flex_realloc(0,0); }
}
#elif YY_FLEX_MAJOR_VERSION<=2 && YY_FLEX_MINOR_VERSION<=5 && YY_FLEX_SUBMINOR_VERSION<33
#error "You seem to be using a version of flex newer than 2.5.4 but older than 2.5.33. These versions do NOT work with doxygen! Please use version <=2.5.4 or >=2.5.33 or expect things to be parsed wrongly!"
#else
extern "C" { // some bogus code to keep the compiler happy
  void fortrancodeYYdummy() { yy_top_state(); } 
}
#endif

