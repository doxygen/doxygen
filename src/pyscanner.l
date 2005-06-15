/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada), executed
 *  as part of CS179e (Compiler design project) at the UC Riverside,
 *  under supervision of Peter H. Fröhlic.
 */

%{

/*
 *	includes
 */
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
  
#include "pyscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "pycode.h"

#define YY_NEVER_INTERACTIVE 1

/* -----------------------------------------------------------------
 *
 *	statics
 */
static ParserInterface *g_thisParser;
static const char *     inputString;
static int		inputPosition;
static QFile            inputFile;

static Protection	protection;
static Protection	baseProt;

int tabsize = 0;
static QStack<int> spaceStack;

static int		sharpCount   = 0 ;
static int		roundCount   = 0 ;
static int		curlyCount   = 0 ;
static int              padCount     = 0 ;
static QCString         slString;
static Entry*		current_root = 0 ;
static Entry*		global_root  = 0 ;
static Entry*		current      = 0 ;
static Entry*		previous     = 0 ;
static int		yyLineNr     = 1 ;
static int              anonCount    = 0 ;        
static QCString		yyFileName;
static MethodTypes 	mtype;
static bool    		gstat;
static Specifier 	virt;
static Specifier 	baseVirt;
static QCString         msType,msName,msArgs;
static int              memberGroupId = DOX_NOGROUP;
static QCString         memberGroupHeader;
static QCString         memberGroupDocs;
static bool             isTypedef;
//static char             afterDocTerminator;
static QCString         sectionLabel;
static QCString		sectionTitle;
//static SectionInfo::SectionType      
//                        sectionType;
static QCString         funcPtrType;
static QCString         templateStr;
static QCString         aliasName;
static QCString         baseName;
static QCString         formulaText;
static QCString         formulaEnd;

static QCString         fullArgString;

//static QCString         *currentTemplateSpec;
static QStack<Grouping> autoGroupStack;
static Grouping  lastDefGroup( "", Grouping::GROUPING_LOWEST );

static bool             insideFormula;
static bool  	        insideTryBlock=FALSE;
static bool             insideCode;

static int              depthIf;
static QCString         memberGroupRelates;
static QCString         memberGroupInside;
static QCString         xrefItemKey;
static QCString         xrefItemTitle;
static QCString         xrefListTitle;

static QCString         g_skipBlockName;
static QCString         oldStyleArgType;
static QCString         docBackup;
static QCString         briefBackup;

static int              docBlockContext;
static QCString         docBlock;
static QCString         docBlockName;
static bool             docBlockInBody;
static bool             docBlockJavaStyle;
static bool             docBrief;

static bool             g_doubleQuote;
static bool             g_specialBlock;

int g_indent = 0;
int class_indent = 0;
int classKeywordIndent = 0;

//-----------------------------------------------------------------------------


static void initParser()
{
  sectionLabel.resize(0);
  sectionTitle.resize(0);
  baseName.resize(0);
  formulaText.resize(0);
  protection = Public;
  baseProt = Public;
  sharpCount = 0;
  roundCount = 0;
  curlyCount = 0;
  memberGroupId = DOX_NOGROUP;
  memberGroupRelates.resize(0);
  memberGroupInside.resize(0);
  mtype = Method;
  gstat = FALSE;
  virt = Normal;
  baseVirt = Normal;
  isTypedef = FALSE;
  autoGroupStack.clear();
  insideTryBlock = FALSE;
  autoGroupStack.setAutoDelete(TRUE);
  lastDefGroup.groupname.resize(0);
  insideFormula = FALSE;
  insideCode=FALSE;
  previous = 0;
}

static void initEntry()
{
  //current->python = TRUE;
  current->protection = protection ;
  current->mtype      = mtype;
  current->virt       = virt;
  current->stat       = gstat;
  current->mGrpId     = memberGroupId;
  current->relates    = memberGroupRelates.copy();
  current->inside     = memberGroupInside.copy();
  current->objc       = FALSE; //insideObjC;
  current->parent     = current_root;
  if (!autoGroupStack.isEmpty())
  {
    //printf("Appending group %s\n",autoGroupStack.top()->groupname.data());
    current->groups->append(new Grouping(*autoGroupStack.top()));
  }
}


//-----------------------------------------------------------------------------

static void lineCount()
{
  for( const char* c = yytext ; *c ; ++c )
    yyLineNr += (*c == '\n') ;
}

#if 0
// Appends the current-name to current-type;
// Destroys current-name.
// Destroys current->args and current->argList
static void addType( Entry* current )
{
    uint tl=current->type.length();
    if ( tl>0 && !current->name.isEmpty() && current->type.at(tl-1)!='.') 
    {
      current->type += ' ' ;
    }
    current->type += current->name ;
    current->name.resize(0) ;
    tl=current->type.length();
    if ( tl>0 && !current->args.isEmpty() && current->type.at(tl-1)!='.') 
    {
      current->type += ' ' ;
    }
    current->type += current->args ;
    current->args.resize(0) ;
    current->argList->clear();
}

static QCString stripQuotes(const char *s)
{
  QCString name;
  if (s==0 || *s==0) return name;
  name=s;
  if (name.at(0)=='"' && name.at(name.length()-1)=='"')
  {
    name=name.mid(1,name.length()-2);
  }
  return name;
}
#endif
//-----------------------------------------------------------------

static void addMemberGroupDocs()
{
  memberGroupDocs=current->brief.stripWhiteSpace();
  current->doc = current->doc.stripWhiteSpace();
  if (!memberGroupDocs.isEmpty() && !current->doc.isEmpty())
  {
    memberGroupDocs+="\n\n";
  }
  memberGroupDocs+=current->doc;
  MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(memberGroupId);
  if (info) 
  {
    info->doc = memberGroupDocs;
    info->docFile = yyFileName;
  }
  current->doc.resize(0);
  current->brief.resize(0);
}

//-----------------------------------------------------------------
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

/*
static void appendDocBlock() {
  previous = current;
  current_root->addSubEntry(current);
  current = new Entry;
  initEntry();
}
*/

static void handleCommentBlock(const QCString &doc,bool brief)
{
  //printf("handleCommentBlock(doc=[%s] brief=%d docBlockInBody=%d\n",
  //    doc.data(),brief,docBlockInBody);

  // TODO: Fix me
  docBlockInBody=FALSE;
  
  if (docBlockInBody && previous && !previous->doc.isEmpty())
  {
    previous->doc=previous->doc.stripWhiteSpace()+"\n\n";
  }

  if (parseCommentBlock(
	g_thisParser,
	(docBlockInBody && previous) ? previous : current,
	doc,     // text
	yyFileName, // file
	brief ? current->briefLine : current->docLine,   // line of block start
	docBlockInBody ? FALSE : brief, 
	FALSE, // javadoc style
	protection)
     ) // need to start a new entry
  {
    // printf("adding node to nodelist...");
    if (current->section==Entry::MEMBERGRP_SEC)
    {
      addMemberGroupDocs();
    }
    current_root->addSubEntry(current);
    previous = current;
    current = new Entry ;
    initEntry();
  }

}

//-----------------------------------------------------------------------------
/* ----------------------------------------------------------------- */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  while ( c < max_size && inputString[inputPosition] )
  {
    *buf = inputString[inputPosition++] ;
    //printf("%d (%c)\n",*buf,*buf);
    c++; buf++;
  }
  return c;
}

%}

       /* start command character */



BB                [ \t]+
B                 [ \t]*
NEWLINE           \n
BN                [ \t\n]

DIGIT             [0-9]
LETTER            [A-Za-z]
NONEMPTY          [A-Za-z0-9_]
EXPCHAR           [#(){}\[\],:.%/\\=`*~|&<>!;+-]
NONEMPTYEXP       [^ \t\n:]
PARAMNONEMPTY     [^ \t\n():]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*  
BORDER            ([^A-Za-z0-9])

POUNDCOMMENT      "#".*

TRISINGLEQUOTE    "'''"
TRIDOUBLEQUOTE    "\"\"\""
LONGSTRINGCHAR    [^\\"']
ESCAPESEQ         ("\\")(.)
LONGSTRINGITEM    ({LONGSTRINGCHAR}|{ESCAPESEQ})
SMALLQUOTE        ("\"\""|"\""|"'"|"''")
LONGSTRINGBLOCK   ({LONGSTRINGITEM}+|{SMALLQUOTE})

SHORTSTRING       ("'"{SHORTSTRINGITEM}*"'"|'"'{SHORTSTRINGITEM}*'"')
SHORTSTRINGITEM   ({SHORTSTRINGCHAR}|{ESCAPESEQ})
SHORTSTRINGCHAR   [^\\\n"]
STRINGLITERAL     {STRINGPREFIX}?( {SHORTSTRING} | {LONGSTRING})  
STRINGPREFIX      ("r"|"u"|"ur"|"R"|"U"|"UR"|"Ur"|"uR")
KEYWORD           ("lambda"|"import"|"class"|"assert"|"as"|"from"|"global"|"def"|"True"|"False")
FLOWKW            ("or"|"and"|"is"|"not"|"print"|"for"|"in"|"if"|"try"|"except"|"yield"|"raise"|"break"|"continue"|"pass"|"if"|"return"|"while"|"elif"|"else"|"finally")
QUOTES            ("\""[^"]*"\"")
SINGLEQUOTES      ("'"[^']*"'")

STARTDOCSYMS      "##"

%option noyywrap
%option nounput

  /* Main start state */

%x Body

  /* Mid-comment states */

  /* %x FuncDoubleComment */
  /* %x ClassDoubleComment */
%x TryClassDocString
%x MultiDoubleComment
%x SpecialComment

  /* Function states */

%x FunctionDec
%x FunctionParams

  /* Class states */

%x ClassDec
%x ClassInheritance
%x ClassCaptureIndent


%%

  /* ------------ Function recognition rules -------------- */

<FunctionDec>{

    {IDENTIFIER}            {
			      //found function name
			      if (current->type.isEmpty()) 
			      {
				  current->type = "def";
			      }

			      current->name = yytext;
			      current->name = current->name.stripWhiteSpace();
			      current->fileName = yyFileName;
                            }

    {B}"("                  {
			       BEGIN( FunctionParams );
		            }
}

<FunctionParams>{
    ({BB}|",")          {
                        }

    {IDENTIFIER}        { // Name of parameter
			  lineCount();
			  Argument *a = new Argument;
			  current->argList->append(a);
			  current->argList->getLast()->name = QCString(yytext).stripWhiteSpace();
			  current->argList->getLast()->type = "";
                        }
    "="[^,)\n]+		{ // default value
                          // TODO: this rule is too simple, need to be able to
                          // match things like =")" as well!
      			  QCString defVal=&yytext[1];
			  if (current->argList->getLast())
			  {
			    current->argList->getLast()->defval=defVal.stripWhiteSpace();
			  }
      			}

     ")"                { // end of parameter list
                        }

     ":"{BN}*         {
			lineCount();

                        // Push the entry.
                        previous = current;
                        current_root->addSubEntry(current);
                        current = new Entry ;
			initEntry();

                        BEGIN( Body );
                      }

    {PARAMNONEMPTY}     { // Default rule inside arguments.
                        }

}

<Body>{

    "def"{BB}           {
			  lineCount();
			  current->fileName  = yyFileName;
			  current->startLine = yyLineNr;
			  current->bodyLine  = yyLineNr;
			  current->section = Entry::FUNCTION_SEC;
			  current->protection = protection = Public;
			  current->objc = FALSE;
			  current->virt = Normal;
			  current->stat = FALSE;
			  current->mtype = mtype = Method;
			  current->type.resize(0);
			  current->name.resize(0);
			  current->args.resize(0);
			  current->argList->clear();

			  // If this function has slipped out
			  // of the parent scope, jump out.
			  if ( g_indent == 0 || g_indent < class_indent ) 
			  {
			    // printf("Function has slipped out of scope! (%d < %d)", g_indent, class_indent);

			    class_indent = 0;

			    if (current_root->parent) 
			    {
			      current_root = current_root->parent;
			    }
			    else 
			    {
			      // This is bad!!!
			      // printf("Warning: using global root because pointer to parent was lost\n");
			      current_root = global_root;
			    }

			  }     

			  BEGIN( FunctionDec );
                       }


     "class"{BB}       {
			  lineCount() ;
			  current->section = Entry::CLASS_SEC;
			  current->argList->clear();
			  current->type += "class" ;
			  current->fileName  = yyFileName;
			  current->startLine = yyLineNr;
			  current->bodyLine  = yyLineNr;

                          // Reset scope - new class found.
                          // (nested classes not supported)
			  classKeywordIndent = g_indent;
                          current_root = global_root;

			  BEGIN( ClassDec ) ;
                       }

    ^{BB}             { // This is for capturing the current indentation
                        // of the current line.
                        g_indent = yyleng;
                      }

    [^\n]             {
                        // This is the major default
                        // that should catch everything
                        // else in Body.
                      }

    {NEWLINE}+        {
                        lineCount();
                        g_indent = 0;
                      }
}

<MultiDoubleComment>{
    {TRIDOUBLEQUOTE}    {
			  if (g_doubleQuote) 
			  {
			    if (g_specialBlock)
			    {
			      handleCommentBlock(docBlock, FALSE);
			    }
			    else
			    {
			      docBlock.resize(0);
			    }
			    BEGIN(docBlockContext);
			  }
                          else 
			  {
                            docBlock += yytext;
                          }
                        }

    {TRISINGLEQUOTE}    {
			  if (!g_doubleQuote) 
			  {
			    if (g_specialBlock)
			    {
			      handleCommentBlock(docBlock, FALSE);
			    }
			    else
			    {
			      docBlock.resize(0);
			    }
			    BEGIN(docBlockContext);
			  }
                          else 
			  {
                            docBlock += yytext;
                          }
                        }

    ({LONGSTRINGBLOCK}) {
			  lineCount();
			  docBlock += yytext;
                        }
}

<SpecialComment>{
    ^{B}"#"("#")*	{ // skip leading hashes
      			}
    \n/{B}"#"		{ // continuation of the comment on the next line
      			  docBlock+='\n';
			  docBrief = FALSE;
                          startCommentBlock(FALSE);
			  yyLineNr++;
      			}
    [^#\n]+             { // any other stuff
      			  docBlock+=yytext;
      			}
    \n			{ // new line that ends the comment
			  handleCommentBlock(docBlock, docBrief);
      			  yyLineNr++;
			  BEGIN(docBlockContext);
      			}
    .			{ // anything we missed
      			  docBlock+=*yytext;
      			}
}

  /* ------------ Class rules -------------- */

<ClassDec>{IDENTIFIER} {
			  if (current->type.isEmpty()) 
			  {
			      current->type = "class";
			  }

			  current->section = Entry::CLASS_SEC;
			  current->name = yytext;
			  current->name = current->name.stripWhiteSpace();
			  current->fileName = yyFileName;        
			  docBlockContext   = YY_START;
			  docBlockInBody    = FALSE;
			  docBlockJavaStyle = FALSE;
			  docBlock.resize(0);

			  // Setting indentation to 0; this totally
			  // totally disallows nested classes.
			  // This is okay for now.
			  class_indent = 0;

			  BEGIN(ClassInheritance);                 
                        }

<ClassInheritance>{
   ({BB}|[(,)])        {
                       }

    ":"                {
			 //BEGIN(TryClassDocString);
      			 BEGIN(ClassCaptureIndent);
                       }


    {IDENTIFIER}       {
                         current->extends->append(
					      new BaseInfo(yytext,Public,Normal)
					    );
                         //Has base class-do stuff
                       }
}


<ClassCaptureIndent>{
    "\n"|({BB}"\n")            {
                                 // Blankline - ignore, keep looking for indentation.
				 lineCount();
                               }

    {BB}/({NONEMPTY}|{EXPCHAR}) {
                                 // Indentation level found!
				 // Pushback the class, and
				 // try to take over as the current root.

                                 // Add to tree
				 current_root->addSubEntry(current);

				 if (yyleng >= classKeywordIndent) 
				 {
				   // Take over the parent if this indentation
				   // is greater than the indentation
				   // of where the class started.
				   current->parent = current_root;
				   current_root = current;
				   previous = 0;
				   class_indent = yyleng;

				   // printf("Found indent of %d on line %d, using it.\n", class_indent, yyLineNr);
				 }
				 else 
				 {
				   // Otherwise, don't push deeper;
				   // this class's scope never started
				   // properly.
				   previous = current;
				   current->endBodyLine = yyLineNr;
				   // printf("Found indent, but its too small (%d < %d)", yyleng, classKeywordIndent);
                                 }

				 // Re-initialize current
				 current = new Entry ;
				 initEntry();

                                 // Remember indentation level for later funcs
                                 g_indent = yyleng;
				 BEGIN( Body );
                               }

    ""/({NONEMPTY}|{EXPCHAR})  {
                                 // Default rule; this is a syntax error
				 // (no indentation defined by user).
				 class_indent = 0;
				 
				 // Just pushback an empty class, and
				 // resume parsing the body.
				 previous = current;
				 current_root->addSubEntry(current);
				 current = new Entry ;
				 initEntry();

				 // printf("Failed to find indent - skipping!");
				 BEGIN( Body );
                               }
}



  /* ------------ End rules -------------- */

<*>{TRIDOUBLEQUOTE}("!")?  { // start of a comment block
			 lineCount();
                         docBlockContext   = YY_START;
                         docBlockInBody    = FALSE;
			 docBlockJavaStyle = FALSE;
			 docBlock.resize(0);
			 g_doubleQuote = TRUE;
			 g_specialBlock = yytext[yyleng-1]=='!';
                         startCommentBlock(FALSE);
			 BEGIN(MultiDoubleComment);
                      }

<*>{TRISINGLEQUOTE}("!"?) {
			 lineCount();
                         docBlockContext   = YY_START;
                         docBlockInBody    = FALSE;
			 docBlockJavaStyle = FALSE;
			 docBlock.resize(0);
			 g_doubleQuote = FALSE;
			 g_specialBlock = yytext[yyleng-1]=='!';
                         startCommentBlock(FALSE);
			 BEGIN(MultiDoubleComment);
                      }

<*>{STARTDOCSYMS}    {
                         docBlockContext   = YY_START;
                         docBlockInBody    = FALSE;
			 docBlockJavaStyle = TRUE;
			 docBrief = TRUE;
			 docBlock.resize(0);
                         startCommentBlock(TRUE);
			 BEGIN(SpecialComment);
                      }


<*>({NONEMPTY}|{EXPCHAR}|{BB})           { // This should go one character at a time.
				 // printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyLineNr);

                               }

<*>{NEWLINE}                   {
				 //printf("[pyscanner] %d NEWLINE [line %d] no match\n",
				 //       YY_START, yyLineNr);

                                 lineCount();
				 BEGIN(Body);
                               }

<*>.                           {
				 //printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyLineNr);

				 BEGIN(Body);					
                               }


%%

//----------------------------------------------------------------------------

static void parseCompounds(Entry *rt)
{
  //printf("parseCompounds(%s)\n",rt->name.data());
  EntryListIterator eli(*rt->sublist);
  Entry *ce;
  for (;(ce=eli.current());++eli)
  {
    if (!ce->program.isEmpty())
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  ce->name.data(),ce->program.data());
      // init scanner state
      padCount=0;
      depthIf = 0;
      inputString = ce->program;
      lastDefGroup.groupname.resize(0);
      inputPosition = 0;
      pyscanYYrestart( pyscanYYin ) ;

      BEGIN( Body ) ;

      current_root = ce ;
      yyFileName = ce->fileName;
      //setContext();
      yyLineNr = ce->startLine ;
      //insideObjC = ce->objc;
      //printf("---> Inner block starts at line %d objC=%d\n",yyLineNr,insideObjC);
      //current->reset();
      if (current) delete current;
      current = new Entry;
      gstat = FALSE;
      int ni=ce->name.findRev("::"); if (ni==-1) ni=0; else ni+=2;
      // set default protection based on the compound type
      if ( ce->section==Entry::CLASS_SEC ) // class
      {
        current->protection = protection = Public; 
      }
      mtype = Method;
      virt = Normal;
      //printf("name=%s current->stat=%d gstat=%d\n",ce->name.data(),current->stat,gstat);

      memberGroupId = DOX_NOGROUP;
      memberGroupRelates.resize(0);
      memberGroupInside.resize(0);
      
      pyscanYYlex() ;
      delete current; current=0;
      ce->program.resize(0);

      if (depthIf>0)
      {
	warn(yyFileName,yyLineNr,"Documentation block ended in the middle of a conditional section!");
      }
    }
    parseCompounds(ce);
  }
}

//----------------------------------------------------------------------------

static void parseMain(const char *fileName,const char *fileBuf,Entry *rt)
{
  initParser();

  inputString = fileBuf;
  inputPosition = 0;

  anonCount     = 0;
  depthIf       = 0;
  protection    = Public;
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
    //setContext();
    msg("Parsing file %s...\n",yyFileName.data());

    current_root  = rt ;
    initParser();
    current       = new Entry;
    int sec=guessSection(yyFileName);
    if (sec)
    {
      current->name    = yyFileName;
      current->section = sec;
      current_root->addSubEntry(current);
      current          = new Entry;
    }



    // Set the python flags
    //current_root->python = TRUE;
    //current->python = TRUE;

    current->reset();
    pyscanYYrestart( pyscanYYin );
    BEGIN( Body );

    pyscanYYlex();
    //call ast visitor
    if (depthIf>0)
    {
      warn(yyFileName,yyLineNr,"Documentation block ended in the middle of a conditional section!");
    }

    rt->program.resize(0);
    delete current; current=0;

    parseCompounds(rt);

    inputFile.close();
  }
  
}

//----------------------------------------------------------------------------

static void parsePrototype(const QCString &text)
{
  //printf("**** parsePrototype(%s) begin\n",text.data());

  const char *orgInputString;
  int orgInputPosition;
  YY_BUFFER_STATE orgState;
  
  // save scanner state
  orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(pyscanYYin, YY_BUF_SIZE));
  orgInputString = inputString; 
  orgInputPosition = inputPosition;

  // set new string
  inputString = text;
  inputPosition = 0;
  pyscanYYrestart( pyscanYYin );

  BEGIN( Body );

  pyscanYYlex();

  current->name = current->name.stripWhiteSpace();
  if (current->section == Entry::MEMBERDOC_SEC && current->args.isEmpty())
    current->section = Entry::VARIABLEDOC_SEC;

  // restore original scanner state
  yy_switch_to_buffer(orgState);
  inputString = orgInputString; 
  inputPosition = orgInputPosition;

  //printf("**** parsePrototype end\n");
}

//----------------------------------------------------------------------------

void PythonLanguageScanner::parseInput(const char *fileName,const char *fileBuf,Entry *root)
{
  g_thisParser = this;
  ::parseMain(fileName,fileBuf,root);

  // May print the AST for debugging purposes
  // printAST(global_root);
}

bool PythonLanguageScanner::needsPreprocessing(const QCString &)
{
  return FALSE;
}

void PythonLanguageScanner::parseCode(CodeOutputInterface &codeOutIntf,
    const char *scopeName,
    const QCString &input,
    bool isExampleBlock,
    const char *exampleName,
    FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    MemberDef *memberDef
    )
{
  ::parsePythonCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                    fileDef,startLine,endLine,inlineFragment,memberDef);
}

void PythonLanguageScanner::parsePrototype(const char *text)
{
  ::parsePrototype(text);

}

void PythonLanguageScanner::resetCodeParserState()
{
  ::resetPythonCodeParserState();
}

void PythonLanguageScanner::handleGroupStartCommand(const char * /*header*/)
{

}

void PythonLanguageScanner::handleGroupEndCommand()
{

}


//----------------------------------------------------------------------------

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void pyscannerYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

