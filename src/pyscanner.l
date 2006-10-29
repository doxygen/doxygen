/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada)
 *  in Spring 2005 as part of CS 179E: Compiler Design Project
 *  at the University of California, Riverside; the course was
 *  taught by Peter H. Froehlich <phf@acm.org>.
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
#include <qfileinfo.h>
  
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

static Entry*		current_root = 0 ;
static Entry*		current      = 0 ;
static Entry*		previous     = 0 ;
static Entry*		bodyEntry    = 0 ;
static int		yyLineNr     = 1 ;
static QCString		yyFileName;
static MethodTypes 	mtype;
static bool    		gstat;
static Specifier 	virt;

static int              docBlockContext;
static QCString         docBlock;
static QCString         docBlockName;
static bool             docBlockInBody;
static bool             docBlockJavaStyle;
static bool             docBrief;
static bool             docBlockSpecial;

static bool             g_doubleQuote;
static bool             g_specialBlock;
//static bool             g_expectModuleDocs;
static int              g_stringContext;
static QGString *       g_copyString;
static int              g_indent = 0;
static int              g_curIndent = 0;

static QDict<QCString>  g_packageNameCache(257);
static QCString         g_packageScope;

static char             g_atomStart;
static char             g_atomEnd;
static int              g_atomCount;

//static bool             g_insideConstructor;

static QCString         g_moduleScope;
static QCString         g_packageName;

static bool             g_hideClassDocs;

static QCString         g_defVal;
static int              g_braceCount;

static bool             g_lexInit = FALSE;

//-----------------------------------------------------------------------------


static void initParser()
{
  protection = Public;
  mtype = Method;
  gstat = FALSE;
  virt = Normal;
  previous = 0;
  g_packageNameCache.setAutoDelete(TRUE);
}

static void initEntry()
{
  //current->python = TRUE;
  current->protection = protection ;
  current->mtype      = mtype;
  current->virt       = virt;
  current->stat       = gstat;
  current->objc       = FALSE; //insideObjC;
  current->setParent(current_root);
  initGroupInfo(current);
}

static void newEntry()
{
  previous = current;
  current_root->addSubEntry(current);
  current = new Entry ;
  initEntry();
}

static void newVariable()
{
  if (!current->name.isEmpty() && current->name.at(0)=='_') // mark as private
  {
    current->protection=Private;
  }
  if (current_root->section&Entry::SCOPE_MASK) // mark as class variable
  {
    current->stat = TRUE;
  }
  newEntry();
}

static void newFunction()
{
  if (current->name.left(2)=="__" && current->name.right(2)=="__")
  {
    // special method name, see
    // http://docs.python.org/ref/specialnames.html
    current->protection=Public;
  }
  else if (current->name.at(0)=='_')
  {
    current->protection=Private;
  }
}

static inline int computeIndent(const char *s)
{
  int col=0;
  static int tabSize=Config_getInt("TAB_SIZE");
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    if (c==' ') col++;
    else if (c=='\t') col+=tabSize-(col%tabSize);
    else break;
  }
  return col;
}

static QCString findPackageScopeFromPath(const QCString &path)
{
  QCString *pScope = g_packageNameCache.find(path);
  if (pScope)
  {
    return *pScope;
  }
  QFileInfo pf(path+"/__init__.py"); // found package initialization file
  if (pf.exists())
  {
    int i=path.findRev('/');
    if (i!=-1)
    {
      QCString scope = findPackageScopeFromPath(path.left(i));
      if (!scope.isEmpty())
      {
	scope+="::";
      }
      scope+=path.mid(i+1);
      g_packageNameCache.insert(path,new QCString(scope));
      return scope;
    }
  }
  return "";
}
  
static QCString findPackageScope(const char *fileName)
{
  if (fileName==0) return "";
  QFileInfo fi(fileName);
  return findPackageScopeFromPath(fi.dirPath(TRUE).data());
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
  //printf("handleCommentBlock(doc=[%s] brief=%d docBlockInBody=%d docBlockJavaStyle=%d\n",
  //    doc.data(),brief,docBlockInBody,docBlockJavaStyle);

  // TODO: Fix me
  docBlockInBody=FALSE;
  
  if (docBlockInBody && previous && !previous->doc.isEmpty())
  {
    previous->doc=previous->doc.stripWhiteSpace()+"\n\n";
  }

  int position = 0;
  bool needsEntry;
  while (parseCommentBlock(
	g_thisParser,
	(docBlockInBody && previous) ? previous : current,
	doc,     // text
	yyFileName, // file
	brief ? current->briefLine : current->docLine,   // line of block start
	docBlockInBody ? FALSE : brief, 
	FALSE, //docBlockJavaStyle, // javadoc style
	docBlockInBody,
	protection,
        position,
        needsEntry)
     ) // need to start a new entry
  {
    if (needsEntry)
    {
      newEntry();
    }
  }
  if (needsEntry)
  {
    newEntry();
  }

}

static void endOfDef()
{
  if (bodyEntry)
  {
    bodyEntry->endBodyLine  = yyLineNr;
    bodyEntry = 0;
  }
  newEntry();
  //g_insideConstructor = FALSE;
}

static inline void addToString(const char *s)
{
  if (g_copyString) (*g_copyString)+=s;
}

static void initTriDoubleQuoteBlock()
{
  docBlockContext   = YY_START;
  docBlockInBody    = FALSE;
  docBlockJavaStyle = TRUE;
  docBlockSpecial   = yytext[3]=='!';
  docBlock.resize(0);
  g_doubleQuote = TRUE;
  startCommentBlock(FALSE);
}

static void initTriSingleQuoteBlock()
{
  docBlockContext   = YY_START;
  docBlockInBody    = FALSE;
  docBlockJavaStyle = TRUE;
  docBlockSpecial   = yytext[3]=='!';
  docBlock.resize(0);
  g_doubleQuote = FALSE;
  startCommentBlock(FALSE);
}

static void initSpecialBlock()
{
  docBlockContext   = YY_START;
  docBlockInBody    = FALSE;
  docBlockJavaStyle = TRUE;
  docBrief = TRUE;
  docBlock.resize(0);
  startCommentBlock(TRUE);
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

HEXNUMBER         "0"[xX][0-9a-fA-F]+[lL]?
OCTNUMBER         "0"[0-7]+[lL]?
NUMBER            {DIGIT}+[lLjJ]?
INTNUMBER         {HEXNUMBER}|{OCTNUMBER}|{NUMBER}
FLOATNUMBER       {DIGIT}+"."{DIGIT}+([eE][+\-]?{DIGIT}+)?[jJ]?
LETTER            [A-Za-z]
NONEMPTY          [A-Za-z0-9_]
EXPCHAR           [#(){}\[\],:.%/\\=`*~|&<>!;+-]
NONEMPTYEXP       [^ \t\n:]
PARAMNONEMPTY     [^ \t\n():]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*  
SCOPE             {IDENTIFIER}("."{IDENTIFIER})*
BORDER            ([^A-Za-z0-9])

TRISINGLEQUOTE    "'''"(!)?
TRIDOUBLEQUOTE    "\"\"\""(!)?
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
POUNDCOMMENT      {B}"#"[^#\n][^\n]* 

STARTDOCSYMS      ^{B}"##"/[^#]

%option noyywrap

  /* Main start state */

%x Search
%x SearchMemVars

  /* Mid-comment states */

  /* %x FuncDoubleComment */
  /* %x ClassDoubleComment */
%x TryClassDocString
%x TripleComment
%x SpecialComment

  /* Function states */

%x FunctionDec
%x FunctionParams
%x FunctionBody
%x FunctionParamDefVal

  /* Class states */

%x ClassDec
%x ClassInheritance
%x ClassCaptureIndent
%x ClassBody

  /* Variable states */
%x VariableDec
%x VariableEnd
%x VariableAtom

  /* String states */

%x SingleQuoteString
%x DoubleQuoteString
%x TripleString

  /* import */
%x FromMod
%x FromModItem
%x Import

%%

  /* ------------ Function recognition rules -------------- */

<Search>{

    ^{B}"def"{BB}       |
    "def"{BB}           { // start of a function/method definition
      			  g_indent=computeIndent(yytext);
                          //g_expectModuleDocs = FALSE;
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
			  BEGIN( FunctionDec );
                       }

     ^{B}"class"{BB}	|
     "class"{BB}        {  // start of a class definition
      			  g_indent=computeIndent(yytext);
                          //g_expectModuleDocs = FALSE;
			  current->section = Entry::CLASS_SEC;
			  current->argList->clear();
			  current->type += "class" ;
			  current->fileName  = yyFileName;
			  current->bodyLine  = yyLineNr;

			  BEGIN( ClassDec ) ;
                       }
     ^{B}"from"{BB}    |
     "from"{BB}	       { // start of an from import
                          BEGIN( FromMod );
                       }

     ^{B}"import"{BB}  |
     "import"{BB}      { // start of an import statement
                          BEGIN( Import );
                       }
     ^{B}{IDENTIFIER}/{B}"="[^=] { // variable
      			g_indent=computeIndent(yytext);
      			current->section   = Entry::VARIABLE_SEC;
			current->name      = QCString(yytext).stripWhiteSpace();
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			BEGIN(VariableDec);
      		      }
     "'"	       { // start of a single quoted string
       			  g_stringContext=YY_START;
			  g_copyString=0;
                          BEGIN( SingleQuoteString );
                       }
     "\""	       { // start of a double quoted string
       			  g_stringContext=YY_START;
			  g_copyString=0;
                          BEGIN( DoubleQuoteString );
                       }

    {POUNDCOMMENT}    { // normal comment 
      		      }
    {IDENTIFIER}      { // some other identifier
		      }
    ^{BB}	      {
      			g_curIndent=computeIndent(yytext);
                      }
    [^\n]             { // any other character...
                        // This is the major default
                        // that should catch everything
                        // else in Body.
                      }

    {NEWLINE}+        { // new line
                        lineCount();
                      }

    {TRIDOUBLEQUOTE}  { // start of a comment block
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}    {  // start of a special comment
			initSpecialBlock();
			BEGIN(SpecialComment);
                      }
}

<FromMod>{
  {IDENTIFIER}({B}"."{B}{IDENTIFIER})* { // from package import 
                        g_packageName=yytext;
		      }
  "import"{B}	      {
    			BEGIN(FromModItem);
    		      }
  \n		      {
                        yyLineNr++;
                        BEGIN(Search);
                      }
  {B}		      {
		      }
  .                   {
                        unput(*yytext);
                        BEGIN(Search);
                      }
}

<FromModItem>{
  "*"		{ // import all
                  QCString item=g_packageName+"."+yytext;
		  current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
		  current->fileName = yyFileName; 
		  //printf("Adding using directive: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
		  current->section=Entry::USINGDIR_SEC;
		  current_root->addSubEntry(current);
		  current = new Entry ;
		  initEntry();
                  BEGIN(Search);
    		}
  {IDENTIFIER}  {
                  QCString item=g_packageName+"."+yytext;
		  current->name=removeRedundantWhiteSpace(substitute(item,".","::"));
		  current->fileName = yyFileName; 
		  //printf("Adding using declaration: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
		  current->section=Entry::USINGDECL_SEC;
		  current_root->addSubEntry(current);
		  current = new Entry ;
		  initEntry();
                  BEGIN(Search);
		}
  \n		{
                  yyLineNr++;
                  BEGIN(Search);
                }
  {B}		{
		}
  .             {
                  unput(*yytext);
                  BEGIN(Search);
                }
}

<Import>{
  {IDENTIFIER}({B}"."{B}{IDENTIFIER})* {
			current->name=removeRedundantWhiteSpace(substitute(yytext,".","::"));
			current->fileName = yyFileName; 
			//printf("Adding using declaration: found:%s:%d name=%s\n",yyFileName.data(),yyLineNr,current->name.data());
			current->section=Entry::USINGDECL_SEC;
			current_root->addSubEntry(current);
			current = new Entry ;
			initEntry();
			BEGIN(Search);
                      }
  \n		{
                  yyLineNr++;
                  BEGIN(Search);
                }
  {B}		{
		}
  .             {
                  unput(*yytext);
                  BEGIN(Search);
                }
}

<SearchMemVars>{
    "self."{IDENTIFIER}/{B}"=" {
                        //printf("Found member variable %s in %s\n",&yytext[5],current_root->name.data());
			current->name=&yytext[5];
			current->section=Entry::VARIABLE_SEC;
			current->fileName  = yyFileName;
			current->startLine = yyLineNr;
			current->bodyLine  = yyLineNr;
			current->type.resize(0);
			if (current->name.at(0)=='_') // mark as private
			{
			  current->protection=Private;
			}
			else
			{
			  current->protection=Public;
			}
			newEntry();
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}    {  // start of a special comment
			initSpecialBlock();
			BEGIN(SpecialComment);
                      }
    "'"	              { // start of a single quoted string
       			g_stringContext=YY_START;
			g_copyString=0;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
       			g_stringContext=YY_START;
			g_copyString=0;
                        BEGIN( DoubleQuoteString );
                      }
    .
    \n		      { yyLineNr++; }
}

<FunctionBody>{
    \n{B}/{IDENTIFIER}{BB}  {
                        //fprintf(stderr,"indent %d<=%d\n",computeIndent(&yytext[1]),g_indent);
                        if (computeIndent(&yytext[1])<=g_indent) 
			{
			  int i;
			  for (i=yyleng-1;i>=0;i--)
			  {
			    unput(yytext[i]);
			  }
			  endOfDef();
			  YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                          BEGIN(Search);
			}
			else
			{
                          yyLineNr++;
      		          current->program+=yytext;
			}
                      }
    \n{B}/"##"	      {
                        if (computeIndent(&yytext[1])<=g_indent)
			{
			  int i;
			  for (i=yyleng-1;i>=0;i--)
			  {
			    unput(yytext[i]);
			  }
			  endOfDef();
			  YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                          BEGIN(Search);
			}
			else
			{
			  yyLineNr++;
      		          current->program+=yytext;
			}
      		      }
    <<EOF>>	      {
			endOfDef();
			yyterminate();
      		      }
    ^{BB}/\n	      { // skip empty line
      		        current->program+=yytext;
      	              }
    ^{BB}	      { // something at indent >0
      		        current->program+=yytext;
			g_curIndent = computeIndent(yytext);
                        if (g_curIndent<=g_indent) 
			  // jumped out of the function
			{
			  endOfDef();
                          BEGIN(Search);
			}
      		      }
    "'"	              { // start of a single quoted string
      		        current->program+=yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
      		        current->program+=yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( DoubleQuoteString );
                      }
    [^ \t\n#'".]+     { // non-special stuff
      		        current->program+=yytext;
		        g_specialBlock = FALSE; 
                      }
    ^{POUNDCOMMENT}   { // normal comment 
      		        current->program+=yytext;
      		      }
    "#".*             { // comment half way
      		        current->program+=yytext;
                      }
    {NEWLINE}	      { yyLineNr++; 
      		        current->program+=yytext;
		      }
    .                 { // any character
      		        current->program+=*yytext;
		        g_specialBlock = FALSE; 
                      }

    {TRIDOUBLEQUOTE}  { // start of a comment block
			current->program+=yytext;
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
			current->program+=yytext;
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {STARTDOCSYMS}    {  // start of a special comment
			initSpecialBlock();
			BEGIN(SpecialComment);
                      }
    
}

<FunctionDec>{

    {IDENTIFIER}            {
			      //found function name
			      if (current->type.isEmpty()) 
			      {
				  current->type = "def";
			      }
			      current->name = yytext;
			      current->name = current->name.stripWhiteSpace();
			      newFunction();
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
    "="		        { // default value
                          // TODO: this rule is too simple, need to be able to
                          // match things like =")" as well!
      			  QCString defVal=&yytext[1];
			  g_defVal.resize(0);
			  g_braceCount=0;
			  BEGIN(FunctionParamDefVal);
      			}

     ")"                { // end of parameter list
                        }

     ":"{B}             {
			  g_specialBlock = TRUE; // expecting a docstring
			  bodyEntry = current;
                          BEGIN( FunctionBody );
                        }

    {PARAMNONEMPTY}     { // Default rule inside arguments.
                        }

}

<FunctionParamDefVal>{
     "("		{ // internal opening brace
       			  g_braceCount++;
			  g_defVal+=*yytext;
       			}
     ","		| 
     ")"		{
       			  if (g_braceCount==0)  // end of default argument
			  {
			    if (current->argList->getLast())
			    {
			      current->argList->getLast()->defval=g_defVal.stripWhiteSpace();
			    }
       			    BEGIN(FunctionParams);
			  }
			  else // continue
			  {
			    g_braceCount--;
			    g_defVal+=*yytext;
			  }
       			}
     .			{
			    g_defVal+=*yytext;
       			}
     \n                 {
       			    g_defVal+=*yytext;
			    yyLineNr++;
       			}
}


<ClassBody>{
    \n/{IDENTIFIER}{BB}  { // new def at indent 0
                        yyLineNr++;
			endOfDef();
			g_hideClassDocs = FALSE;
			YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                        BEGIN(Search);
                      }
    \n/"##"           {  // start of a special comment at indent 0
                        yyLineNr++;
			endOfDef();
			g_hideClassDocs = FALSE;
			YY_CURRENT_BUFFER->yy_at_bol=TRUE;
                        BEGIN(Search);
                      }
    ^{BB}/\n	      { // skip empty line
      		        current->program+=yytext;
      	              }
    <<EOF>>	      {
			endOfDef();
			yyterminate();
      		      }
    ^{BB}	      { // something at indent >0
                        g_curIndent=computeIndent(yytext);
			//fprintf(stderr,"g_curIndent=%d g_indent=%d\n",g_curIndent,g_indent);
                        if (g_curIndent<=g_indent) 
			  // jumped out of the class
			{
			  endOfDef();
			  g_indent=g_curIndent;
			  // make sure the next rule matches ^...
			  YY_CURRENT_BUFFER->yy_at_bol=TRUE;
			  g_hideClassDocs = FALSE;
                          BEGIN(Search);
			}
			else
			{
      		          current->program+=yytext;
			}
      		      }
    "'"	              { // start of a single quoted string
      		        current->program+=*yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
      		        current->program+=*yytext;
       			g_stringContext=YY_START;
		        g_specialBlock = FALSE; 
			g_copyString=&current->program;
                        BEGIN( DoubleQuoteString );
                      }
    [^ \t\n#'"]+      { // non-special stuff
      		        current->program+=yytext;
		        g_specialBlock = FALSE; 
			g_hideClassDocs = FALSE;
                      }
    {NEWLINE}	      { 
      		        current->program+=*yytext;
			yyLineNr++; 
		      }
    {POUNDCOMMENT}    { // normal comment
      		        current->program+=yytext;
                      }
    .                 { // any character
		        g_specialBlock = FALSE; 
      		        current->program+=*yytext;
                      }
    {TRIDOUBLEQUOTE}  { // start of a comment block
      		        if (!g_hideClassDocs) current->program+=yytext;
			initTriDoubleQuoteBlock();
			BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
      		        if (!g_hideClassDocs) current->program+=yytext;
			initTriSingleQuoteBlock();
			BEGIN(TripleComment);
                      }
}

<ClassDec>{IDENTIFIER} {
			  if (current->type.isEmpty()) 
			  {
			      current->type = "class";
			  }

			  current->section = Entry::CLASS_SEC;
			  current->name = yytext;

			  // prepend scope in case of nested classes
			  if (current_root->section&Entry::SCOPE_MASK)
			  {
			    current->name.prepend(current_root->name+"::");
			  }
			  
			  current->name = current->name.stripWhiteSpace();
			  current->fileName = yyFileName;        
			  docBlockContext   = YY_START;
			  docBlockInBody    = FALSE;
			  docBlockJavaStyle = FALSE;
			  docBlock.resize(0);

			  BEGIN(ClassInheritance);                 
                        }

<ClassInheritance>{
   ({BB}|[\(,\)])      { // syntactic sugar for the list
                       }

    ":"                { // begin of the class definition
			 g_specialBlock = TRUE; // expecting a docstring
      			 BEGIN(ClassCaptureIndent);
                       }

    {SCOPE}            {
                         current->extends->append(
					      new BaseInfo(substitute(yytext,".","::"),Public,Normal)
					    );
                         //Has base class-do stuff
                       }
}


<ClassCaptureIndent>{
    "\n"|({BB}"\n")            {
                                 // Blankline - ignore, keep looking for indentation.
				 lineCount();
                               }

    {TRIDOUBLEQUOTE}           { // start of a comment block
				 initTriDoubleQuoteBlock();
			 	 BEGIN(TripleComment);
                      	       }

    {TRISINGLEQUOTE}           { // start of a comment block
			         initTriSingleQuoteBlock();
			         BEGIN(TripleComment);
                               }

    ^{BB} 		       {
      				 current->program=yytext;
			         current->startLine = yyLineNr;
				 g_curIndent=computeIndent(yytext);
				 bodyEntry = current;
				 //fprintf(stderr,"setting indent %d\n",g_curIndent);
				 //printf("current->program=[%s]\n",current->program.data());
				 g_hideClassDocs = TRUE;
			         BEGIN(ClassBody);
                               }

    ""/({NONEMPTY}|{EXPCHAR})  {
				 
				 // Just pushback an empty class, and
				 // resume parsing the body.
                                 newEntry();

				 // printf("Failed to find indent - skipping!");
				 BEGIN( Search );
                               }
}


<VariableDec>{
   "="                { // the assignment operator
                        //printf("====== VariableDec at line %d\n",yyLineNr);
                      }
   {B}                { // spaces
	              }
   {INTNUMBER}	      { // integer value
			current->type = "int";
			current->initializer = yytext;
			BEGIN(VariableEnd);
		      }
   {FLOATNUMBER}      { // floating point value
			current->type = "float";
			current->initializer = yytext;
			BEGIN(VariableEnd);
		      }
   {STRINGPREFIX}?"'" { // string
			current->type = "string";
			current->initializer = yytext;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
                        BEGIN( SingleQuoteString );
     		      }
   {STRINGPREFIX}?"\"" { // string
			current->type = "string";
			current->initializer = yytext;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
                        BEGIN( DoubleQuoteString );
     		      }
   {TRIDOUBLEQUOTE}   { // start of a comment block
			current->type = "string";
			g_doubleQuote=TRUE;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
			BEGIN(TripleString);
                      }

   {TRISINGLEQUOTE}   { // start of a comment block
			current->type = "string";
			g_doubleQuote=FALSE;
			g_copyString=&current->initializer;
       			g_stringContext=VariableEnd;
			BEGIN(TripleString);
                      }
   "("		      { // typle
			current->type = "tuple";
      		        current->initializer+=*yytext;
                        g_atomStart='(';
			g_atomEnd=')';
			g_atomCount=1;
			BEGIN( VariableAtom );
                      }
   "["                { // list
			current->type = "list";
      		        current->initializer+=*yytext;
                        g_atomStart='[';
			g_atomEnd=']';
			g_atomCount=1;
			BEGIN( VariableAtom );
                      }
   "{"		      { // dictionary
			current->type = "dictionary";
      		        current->initializer+=*yytext;
                        g_atomStart='{';
			g_atomEnd='}';
			g_atomCount=1;
			BEGIN( VariableAtom );
                      }
   "#".*              { // comment
                        BEGIN( VariableEnd ); 
                      }
   {IDENTIFIER}	      {
      		        current->initializer+=yytext;
	              }
   .		      {
      		        current->initializer+=*yytext;
                      }
   \n		      {
     		        unput('\n');
     			BEGIN( VariableEnd );
     		      }
}

<VariableAtom>{
    [\(\[\{]	      {
      		        current->initializer+=*yytext;
                        if (g_atomStart==*yytext)
			{
			  g_atomCount++;
			}
                      }
    [\)\]\}]	      {
      		        current->initializer+=*yytext;
                        if (g_atomEnd==*yytext)
			{
			  g_atomCount--;
			}
			if (g_atomCount==0)
			{
			  BEGIN(VariableEnd);
			}
                      }
   {IDENTIFIER}       {
      		        current->initializer+=yytext;
	              }
   .		      {
      		        current->initializer+=*yytext;
                      }
   \n		      {
      		        current->initializer+=*yytext;
     			yyLineNr++;
     		      }

}

<VariableEnd>{
    \n                {
                        yyLineNr++;
			newVariable();
                        BEGIN(Search);
                      }
    .		      { 
                        unput(*yytext);
			newVariable();
                        BEGIN(Search);
      		      }
    <<EOF>>           { yyterminate();
                        newEntry();
                      }
}

<TripleComment>{
    {TRIDOUBLEQUOTE}   | 
    {TRISINGLEQUOTE}   {
			  // printf("Expected module block %d special=%d\n",g_expectModuleDocs,g_specialBlock);
			  if (g_doubleQuote==(yytext[0]=='"')) 
			  {
			    if (g_specialBlock)
			    {
			      QCString actualDoc=docBlock;
			      if (!docBlockSpecial) // legacy unformatted docstring
			      {
			        actualDoc.prepend("\\verbatim ");
			        actualDoc.append("\\endverbatim ");
			      }
			      //if (g_expectModuleDocs)
			      //{
			      //  actualDoc.prepend("\\namespace "+g_moduleScope+"\\_linebr ");
			      //}
			      //printf("-------> current=%p bodyEntry=%p\n",current,bodyEntry);
			      handleCommentBlock(actualDoc, FALSE);
			    }
		    	    //g_expectModuleDocs=FALSE;
			    if ((docBlockContext==ClassBody && !g_hideClassDocs) ||
				docBlockContext==FunctionBody)
			    {
			      current->program+=docBlock;
			      current->program+=yytext;
			    }
			    g_hideClassDocs=FALSE;
			    BEGIN(docBlockContext);
			  }
                          else 
			  {
                            docBlock += yytext;
                          }
                        }


    ^{BB}		{ // leading whitespace
      			  int indent = computeIndent(yytext);
			  if (indent>=g_curIndent)
			  { // strip g_curIndent amount of whitespace
			    int i;
			    for (i=0;i<indent-g_curIndent;i++) docBlock+=' ';
			    //fprintf(stderr,"stripping indent %d\n",g_curIndent);
			  }
			  else
			  {
			    //fprintf(stderr,"not stripping: %d<%d\n",indent,g_curIndent);
			    docBlock += yytext;
			  }
      			}
    [^"'\n \t]+          {
			  docBlock += yytext;
                        }
    \n			{
      			  yyLineNr++;
			  docBlock += yytext;
      			}
    .			{
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
			  //if (g_expectModuleDocs)
			  //{
			  //  docBlock.prepend("\\namespace "+g_moduleScope+"\\_linebr ");
			  //}
		    	  //g_expectModuleDocs=FALSE;
			  handleCommentBlock(docBlock, docBrief);
      			  yyLineNr++;
			  BEGIN(docBlockContext);
      			}
    .			{ // anything we missed
      			  docBlock+=*yytext;
      			}
}

<SingleQuoteString>{
    \\{B}\n                    { // line continuation
      			         addToString(yytext);
      				 yyLineNr++;
                               }
    \\.			       { // espaced char
      			         addToString(yytext);
                               }
    "\"\"\""		       { // tripple double quotes
      			         addToString(yytext);
      			       }
    "'"			       { // end of the string
      			         addToString(yytext);
      		                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
      			         addToString(yytext);
                               }
    .			       { // normal char
      			         addToString(yytext);
                               }
}

<DoubleQuoteString>{
    \\{B}\n                    { // line continuation
      			         addToString(yytext);
      				 yyLineNr++;
                               }
    \\.			       { // espaced char
      			         addToString(yytext);
                               }
    "'''"		       { // tripple single quotes
      			         addToString(yytext);
      			       }
    "\""		       { // end of the string
      			         addToString(yytext);
      		                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
      			         addToString(yytext);
                               }
    .			       { // normal char
      			         addToString(yytext);
                               }
}

<TripleString>{
    {TRIDOUBLEQUOTE}    | 
    {TRISINGLEQUOTE}    {
                          *g_copyString += yytext;
			  //printf("Expected module block %d special=%d\n",g_expectModuleDocs,g_specialBlock);
			  if (g_doubleQuote==(yytext[0]=='"')) 
			  {
			    BEGIN(docBlockContext);
			  }
                        }


    ({LONGSTRINGBLOCK}) {
			  lineCount();
                          *g_copyString += yytext;
                        }
    \n			{
      			  yyLineNr++;
                          *g_copyString += yytext;
      			}
    .			{
                          *g_copyString += *yytext;
      			}
}

  /* ------------ End rules -------------- */

  /*
<*>({NONEMPTY}|{EXPCHAR}|{BB})           { // This should go one character at a time.
				 // printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyLineNr);

                               }
  */

<*>{NEWLINE}                   {
				 //printf("[pyscanner] %d NEWLINE [line %d] no match\n",
				 //       YY_START, yyLineNr);

                                 lineCount();
                               }

<*>.                           {
				 //printf("[pyscanner] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, yyLineNr);

                               }


%%

//----------------------------------------------------------------------------

static void parseCompounds(Entry *rt)
{
  //printf("parseCompounds(%s)\n",rt->name.data());
  EntryListIterator eli(*rt->children());
  Entry *ce;
  for (;(ce=eli.current());++eli)
  {
    if (!ce->program.isEmpty())
    {
      //printf("-- %s ---------\n%s\n---------------\n",
      //  ce->name.data(),ce->program.data());
      // init scanner state
      inputString = ce->program;
      inputPosition = 0;
      pyscanYYrestart( pyscanYYin ) ;
      if (ce->section&Entry::COMPOUND_MASK)
      {
        current_root = ce ;
        BEGIN( Search );
      }
      else if (ce->parent())
      {
        current_root = ce->parent();
	//printf("Searching for member variables in %s parent=%s\n",
	//    ce->name.data(),ce->parent->name.data());
	BEGIN( SearchMemVars );
      }
      yyFileName = ce->fileName;
      yyLineNr = ce->startLine ;
      if (current) delete current;
      current = new Entry;

      groupEnterCompound(yyFileName,yyLineNr,ce->name);
      
      pyscanYYlex() ;
      g_lexInit=TRUE;
      delete current; current=0;
      ce->program.resize(0);

      groupLeaveCompound(yyFileName,yyLineNr,ce->name);

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

  protection    = Public;
  mtype         = Method;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  //g_expectModuleDocs = TRUE;
  g_specialBlock = FALSE;
  //g_insideConstructor = FALSE;


  inputFile.setName(fileName);
  if (inputFile.open(IO_ReadOnly))
  {
    yyLineNr= 1 ; 
    yyFileName = fileName;
    //setContext();
    msg("Parsing file %s...\n",yyFileName.data());

    QFileInfo fi(fileName);
    g_moduleScope = findPackageScope(fileName);
    if (!g_moduleScope.isEmpty())
    {
      g_moduleScope+="::";
    }
    g_moduleScope+=fi.baseName();

    current            = new Entry;
    current->name      = g_moduleScope;
    current->section   = Entry::NAMESPACE_SEC;
    current->type      = "namespace";
    current->fileName  = yyFileName;
    current->startLine = yyLineNr;
    current->bodyLine  = yyLineNr;

    rt->addSubEntry(current);

    current_root  = current ;
    initParser();
    current       = new Entry;

    groupEnterFile(yyFileName,yyLineNr);
    
    current->reset();
    pyscanYYrestart( pyscanYYin );
    BEGIN( Search );
    pyscanYYlex();
    g_lexInit=TRUE;

    groupLeaveFile(yyFileName,yyLineNr);

    current_root->program.resize(0);
    delete current; current=0;

    parseCompounds(current_root);

    inputFile.close();
  }
  
}

//----------------------------------------------------------------------------

static void parsePrototype(const QCString &text)
{
  //printf("**** parsePrototype(%s) begin\n",text.data());
  if (text.isEmpty()) 
  {
    warn(yyFileName,yyLineNr,"Empty prototype found!");
    return;
  }

  //g_expectModuleDocs = FALSE;
  g_specialBlock = FALSE;
  //g_insideConstructor = FALSE;

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

  BEGIN( FunctionDec );

  pyscanYYlex();
  g_lexInit=TRUE;

  current->name = current->name.stripWhiteSpace();
  if (current->section == Entry::MEMBERDOC_SEC && current->args.isEmpty())
    current->section = Entry::VARIABLEDOC_SEC;

  // restore original scanner state

  YY_BUFFER_STATE tmpBuf = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(orgState);
  yy_delete_buffer(tmpBuf);

  inputString = orgInputString; 
  inputPosition = orgInputPosition;

  //printf("**** parsePrototype end\n");
}

void pyscanFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION) 
  if (g_lexInit)
  {
    pyscanYYlex_destroy();
  }
#endif
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

//----------------------------------------------------------------------------

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void pyscannerYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

