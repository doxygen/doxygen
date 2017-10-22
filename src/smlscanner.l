%option never-interactive
%option prefix="smlscannerYY"

%{

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include <qarray.h>
#include <qregexp.h>
#include <unistd.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "smlscanner.h"
#include "entry.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "defargs.h"
#include "language.h"
#include "commentscan.h"
#include "smlcode.h"
#include "arguments.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1

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
static MethodTypes      mtype;
static bool             gstat;
static Specifier        virt;

static int              docBlockContext;
static QCString         docBlock;
static QCString         docBlockName;
static bool             docBlockInBody;
static bool             docBlockJavaStyle;
static bool             docBrief;
static bool             docBlockSpecial;

/* used to handle tuples */
static char             g_atomStart;
static char             g_atomEnd;
static int              g_atomCount;

static bool             g_doubleQuote;
static bool             g_inComment;
static bool             g_specialBlock;
static int              g_stringContext;
static QGString *       g_copyString;

static QDict<QCString>  g_packageNameCache(257);
static QCString         g_packageScope;

static QCString         g_moduleScope;
static QCString         g_packageName;

static bool             g_inSignature;
static QCString         g_valDoc;
static QCString         g_andKeyword;
static QCString         g_documentingStructure;
static QCString         g_defVal;
static int              g_braceCount;

static bool             g_lexInit = FALSE;
static bool             g_packageCommentAllowed;

/* used for detecting what the 'and' keyword is declaring. */
typedef QCString stackElement;

/* a stack for the 'and' keyword. */
typedef struct {stackElement contents[1000]; int maxSize; int top; } stackType ;

/* pushes a declaritive keyword onto the stack. */
stackType stack_push(stackType stack, stackElement element)
{
  stack.contents[++stack.top] = element;
  return stack;
}

/* tests whether a stack is empty. */
int stack_isEmpty(stackType stack)
{
  return stack.top < 0;
}

/* pops an element from the stack. */
stackType stack_pop(stackType stack)
{
  if (stack_isEmpty(stack)) {
    fprintf(stderr, "Error: attempted to pop element from empty stack!.\n");
    exit(1);
  }
  stack.top--;
  return stack;
}

/* gets the top element of the stack. */
stackElement stack_top(stackType stack) {
  if (stack_isEmpty(stack)) {
    fprintf(stderr, "Error: attempted to get top element of an empty stack!.\n");
    exit(1);
  }
  return stack.contents[stack.top];
}

/* declare a new stack, myStack. Used for handling the 'and' keyword */
static stackType myStack;

static void initParser()
{
  protection = Public;
  mtype = Method;
  gstat = FALSE;
  virt = Normal;
  previous = 0;
  g_packageCommentAllowed = TRUE;
  g_packageNameCache.setAutoDelete(TRUE);
}

static void initEntry()
{
  current->protection = protection ;
  current->mtype      = mtype;
  current->virt       = virt;
  current->stat       = gstat;
  current->lang       = SrcLangExt_Sml;
  current->setParent(current_root);
  initGroupInfo(current);
  gstat = FALSE;
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
  if (current_root->section&Entry::COMPOUND_MASK) // mark as class variable
  {
    current->stat = TRUE;
  }
  newEntry();
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
  DBG_CTX((stderr,"yyLineNr=%d\n",yyLineNr));
  for (const char *p = yytext; *p; ++p)
  {
    yyLineNr += (*p == '\n') ;
  }
}

static void incLineNr()
{
  DBG_CTX((stderr,"yyLineNr=%d\n",yyLineNr));
  yyLineNr++;
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
  int lineNr = brief ? current->briefLine : current->docLine;
  while (parseCommentBlock(
        g_thisParser,
        (docBlockInBody && previous) ? previous : current,
        doc,     // text
        yyFileName, // file
        lineNr,
        docBlockInBody ? FALSE : brief,
    docBlockJavaStyle, // javadoc style // or FALSE,
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

static void endOfDef(int correction=0)
{
  if (bodyEntry)
  {
    bodyEntry->endBodyLine  = yyLineNr-correction;
    bodyEntry = 0;
  }
  newEntry();
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

static void searchFoundDef()
{
  current->fileName  = yyFileName;
  current->startLine = yyLineNr;
  current->bodyLine  = yyLineNr;
  current->section = Entry::FUNCTION_SEC;
  current->protection = protection = Public;
  current->lang = SrcLangExt_Sml;
  current->virt = Normal;
  current->stat = gstat;
  current->mtype = mtype = Method;
  current->type.resize(0);
  current->name.resize(0);
  current->args.resize(0);
  current->argList->clear();
  g_packageCommentAllowed = FALSE;
  gstat=FALSE;
}

static void searchFoundClass()
{
  current->section = Entry::CLASS_SEC;
  current->argList->clear();
  current->type += "structure" ;
  current->fileName  = yyFileName;
  current->startLine  = yyLineNr;
  current->bodyLine  = yyLineNr;
  g_packageCommentAllowed = FALSE;
}

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

IDENTIFIER        ({LETTER}|"'")({LETTER}|{DIGIT}|"_"|"'")*

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
DECKW             ("(** "|"val"|"type"|"datatype"|"abstype"|"exception"|"local"|"open"|"infix"|"infixr"|"nofix"|"eqtype"|"include"|"end")

STARTDOCSYMS      "(** "

%option noyywrap

  /* Main start state */

%x Search
%x SearchMemVars

  /* Mid-comment states */

  /* %x FuncDoubleComment */
  /* %x ClassDoubleComment */
%x TryClassDocString
%x TripleComment
%x StandardComment
%x LinkToStructure
%x SpecialComment
%x Test


  /* Function states */

%x FunctionDec
%x FunctionParams
%x SmlPattern
%x SmlVal
%x SmlType
%x SmlEqtype
%x SmlAbstype
%x SmlDatatype
%x SmlException
%x SmlInclude
%x SmlQuoteString
%x FunctionParamDefVal
%x StructureWrapper

  /* Class states */

%x ClassDec
%x ClassInheritance
%x ClassBody

/* Variable states */
%x VariableAtom


/* String states */

%x SingleQuoteString
%x DoubleQuoteString
%x TripleString

%%

  /* ------------ Function recognition rules -------------- */

<Search>{

     "fun"{BB} {
                 g_andKeyword = "fun";
                 searchFoundDef();
                 BEGIN( FunctionDec );
               }
     "structure"{BB}{IDENTIFIER}{B}"="{B}"struct" {yyless(0); BEGIN( StructureWrapper); }
     "structure"{BB}{IDENTIFIER}{B}"="{B}{IDENTIFIER}   	{}
     "structure"{BB}{IDENTIFIER}{B}":"{B}{IDENTIFIER}{B}^"="	{}
     "structure"{BB}{IDENTIFIER}{B}":"{B}{IDENTIFIER}{B}\n	{}
     "structure"{BB}	{
                          g_andKeyword = "structure";
                          g_inSignature = false;
                          DBG_CTX((stderr,"Found class at %d\n",yyLineNr));
                          searchFoundClass();
                          BEGIN( ClassDec ) ;
                        }
     "functor"{BB}      {
                          g_inSignature = false;
                          searchFoundClass();
                          BEGIN( ClassDec ) ;
                       }
     "signature"{BB}   {
                          g_andKeyword = "signature";
                          g_inSignature = true;
                          DBG_CTX((stderr,"Found class at %d\n",yyLineNr));
                          searchFoundClass();
                          BEGIN( ClassDec ) ;
                       }
    "let"             { if (!g_inComment) {myStack = stack_push(myStack,g_andKeyword);} }
    "local"           { if (!g_inComment) {myStack = stack_push(myStack,g_andKeyword);} }
    "abstype"         { if (!g_inComment) {myStack = stack_push(myStack,g_andKeyword);} }
    "end"             {
                        if (!g_inComment) {
                          // ending a structure, signature, or functor
                          if (stack_isEmpty(myStack)) {
			    endOfDef();
			  }
			  else {
                            g_andKeyword = stack_top(myStack);
                            myStack = stack_pop(myStack);
			  }

			}
                      }

     "and"{BB}    	{   // continuation of some definition, unput a space and then the keyword
                            unput(' ');
                            int i;
                            for ( i = g_andKeyword.length() - 1; i >= 0; --i )
                                unput( g_andKeyword[i] );
                        }
     "val"{BB}"_"{B}"="      {}
     "val"{BB}      { g_andKeyword = "val"; BEGIN( SmlVal ); }
     "exception"{B} { g_andKeyword = "exception"; BEGIN( SmlException ); }
     "include"{B}   { BEGIN( SmlInclude );   }
     "type"{BB}     { g_andKeyword = "type"; BEGIN( SmlType );      }
     "eqtype"{B}    { BEGIN( SmlEqtype );    }
     "datatype"{B}  {
                      g_andKeyword = "datatype";
                      g_defVal.resize(0);
                      BEGIN( SmlDatatype );
                    }
     "abstype"{B}   { BEGIN( SmlAbstype ); }


     "'"              { // start of a single quoted string
                        g_stringContext=YY_START;
                        g_copyString=0;
                        g_packageCommentAllowed = FALSE;
                        BEGIN( SingleQuoteString );
                      }
     "\""             { // start of a double quoted string
                        if (g_inComment == 0) {
                          g_stringContext=YY_START;
                          g_copyString=0;
                          g_packageCommentAllowed = FALSE;
                          BEGIN( DoubleQuoteString );
                        }
                      }
    "@staticmethod"  {
                        gstat=TRUE;
                      }
    {IDENTIFIER}      { // some other identifier
                        g_packageCommentAllowed = FALSE;
                      }
    ^{BB}             {}

    {NEWLINE}+        { lineCount(); }

    {TRIDOUBLEQUOTE}  { // start of a comment block
                        initTriDoubleQuoteBlock();
                        BEGIN(TripleComment);
                      }

    {TRISINGLEQUOTE}  { // start of a comment block
                        initTriSingleQuoteBlock();
                        BEGIN(TripleComment);
                      }
    {STARTDOCSYMS}/[^*]    {  // start of a special comment
                        g_inComment = TRUE;
                        g_packageCommentAllowed = FALSE;
                        initSpecialBlock();
                        BEGIN(SpecialComment);
                      }
    "(*"              { g_inComment = 1; BEGIN(StandardComment); }
    [^\n]             { // any other character...
                        // This is the major default
                        // that should catch everything
                        // else in Body.
                      }
}

<StructureWrapper>{
     "structure"{BB}	{ // start of structure declaration
                          g_andKeyword = "structure";
                          g_inSignature = false;
                          DBG_CTX((stderr,"Found class at %d\n",yyLineNr));
                          searchFoundClass();
                          BEGIN( ClassDec ) ;
                        }
}

<SmlVal>{

"("          {
                if (current->mtype!=Property) {
		  current->type = "tuple";
		}
                g_atomStart='(';
		g_atomEnd=')';
		g_atomCount=1;
		BEGIN( VariableAtom );
             }

{IDENTIFIER} {
               if (g_inSignature) {
                 g_valDoc = "View documentation in ";
                 g_valDoc += g_documentingStructure;
                 g_valDoc += ".";
                 g_valDoc += yytext;
                 g_valDoc += ".";
                 handleCommentBlock(g_valDoc, true);
               }

               current->section   = Entry::VARIABLE_SEC;
               current->name      = QCString(yytext).stripWhiteSpace();
	       current->type      = "val";
               current->fileName  = yyFileName;
               current->startLine = yyLineNr;
               current->bodyLine  = yyLineNr;
               g_packageCommentAllowed = FALSE;
               newVariable();
               BEGIN(Search);
             }

}

<VariableAtom>{
    [\(\[\{]	      {
                        if (g_atomStart==*yytext)
			  g_atomCount++;
                      }
    [\)\]\}]	      {
                        if (g_atomEnd==*yytext)
			  g_atomCount--;
			if (g_atomCount==0)
			{

                        current->section   = Entry::VARIABLE_SEC;
                        current->name      = current->initializer;
	                current->type      = "val";
                        current->fileName  = yyFileName;
                        current->startLine = yyLineNr;
                        current->bodyLine  = yyLineNr;
                        g_packageCommentAllowed = FALSE;

			  newVariable();
                          BEGIN(Search);
			}
                      }
   ,                  {
                        current->section   = Entry::VARIABLE_SEC;
                        current->name      = current->initializer;
                        current->type      = "val";
                        current->fileName  = yyFileName;
                        current->startLine = yyLineNr;
                        current->bodyLine  = yyLineNr;
                        g_packageCommentAllowed = FALSE;
			newVariable();
		      }
   .		      { current->initializer+=*yytext; }
   \n		      {
      		        current->initializer+=*yytext;
     			incLineNr();
     		      }

}

<SmlType>{
{IDENTIFIER}/{B} {
               if (g_inSignature) {
                 g_valDoc = "View documentation in ";
                 g_valDoc += g_documentingStructure;
                 g_valDoc += ".";
                 g_valDoc += yytext;
                 g_valDoc += ".";
                 handleCommentBlock(g_valDoc, true);
               }
                          current->section   = Entry::VARIABLE_SEC;
                          current->name      = QCString(yytext).stripWhiteSpace();
			  current->type      = "type";
                          current->protection = Protected;
                          current->fileName  = yyFileName;
                          current->startLine = yyLineNr;
                          current->bodyLine  = yyLineNr;
                          g_packageCommentAllowed = FALSE;
                          newVariable();
                          BEGIN(Search);
                        }
}

<SmlDatatype>{
"'"{IDENTIFIER}{B}{IDENTIFIER} {
               current->section   = Entry::VARIABLE_SEC;
               current->name      = QCString(yytext).stripWhiteSpace();
	       current->type      = "datatype";
               current->protection = Protected;
               current->fileName  = yyFileName;
               current->startLine = yyLineNr;
               current->bodyLine  = yyLineNr;
               g_packageCommentAllowed = FALSE;
               newVariable();
               yyless(0);
               BEGIN(Search);
             }
"(".*")"{B}{IDENTIFIER} {
               current->section   = Entry::VARIABLE_SEC;
               current->name      = QCString(yytext).stripWhiteSpace();
	       current->type      = "datatype";
               current->protection = Protected;
               current->fileName  = yyFileName;
               current->startLine = yyLineNr;
               current->bodyLine  = yyLineNr;
               g_packageCommentAllowed = FALSE;
               newVariable();
               yyless(0);
               BEGIN(Search);
             }
{IDENTIFIER} {
               current->section   = Entry::VARIABLE_SEC;
               current->name      = QCString(yytext).stripWhiteSpace();
	       current->type      = "datatype";
               current->protection = Protected;
               current->fileName  = yyFileName;
               current->startLine = yyLineNr;
               current->bodyLine  = yyLineNr;
               g_packageCommentAllowed = FALSE;
               newVariable();
               yyless(0);
               BEGIN(Search);
             }
}

<SmlEqtype>{
{IDENTIFIER}/{B} {
                          current->section   = Entry::VARIABLE_SEC;
                          current->name      = QCString(yytext).stripWhiteSpace();
			  current->type      = "eqtype";
                          current->protection = Protected;
                          current->fileName  = yyFileName;
                          current->startLine = yyLineNr;
                          current->bodyLine  = yyLineNr;
                          g_packageCommentAllowed = FALSE;
                          newVariable();
                          BEGIN(Search);
                        }
}

<SmlAbstype>{
{IDENTIFIER}/{B} {
                          current->section   = Entry::VARIABLE_SEC;
                          current->name      = QCString(yytext).stripWhiteSpace();
			  current->type      = "abstype";
                          current->protection = Protected;
                          current->fileName  = yyFileName;
                          current->startLine = yyLineNr;
                          current->bodyLine  = yyLineNr;
                          g_packageCommentAllowed = FALSE;
                          newVariable();
                          BEGIN(Search);
                        }
}

<SmlException>{
{IDENTIFIER}/{B} {
               if (g_inSignature) {
                 g_valDoc = "View documentation in ";
                 g_valDoc += g_documentingStructure;
                 g_valDoc += ".";
                 g_valDoc += yytext;
                 g_valDoc += ".";
                 handleCommentBlock(g_valDoc, true);
               }

                   current->section   = Entry::VARIABLE_SEC;
                   current->name      = QCString(yytext).stripWhiteSpace();
		   current->type      = "exn";
                   current->fileName  = yyFileName;
                   current->startLine = yyLineNr;
                   current->bodyLine  = yyLineNr;
                   g_packageCommentAllowed = FALSE;
                   newVariable();
                   BEGIN(Search);
                 }
}

<SmlInclude>{
{IDENTIFIER}/{B} {
                   current->section   = Entry::VARIABLE_SEC;
                   current->name      = QCString(yytext).stripWhiteSpace();
		   current->type      = "include";
                   current->fileName  = yyFileName;
                   current->startLine = yyLineNr;
                   current->bodyLine  = yyLineNr;
                   g_packageCommentAllowed = FALSE;
                   newVariable();
                   BEGIN(Search);
                 }
}

<SearchMemVars>{
    "self."{IDENTIFIER}/{B}"=" {
                        DBG_CTX((stderr,"Found member variable %s in %s at %d\n",&yytext[5],current_root->name.data(),yyLineNr));
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

    {STARTDOCSYMS}/[^*]    {  // start of a special comment
                        initSpecialBlock();
                        BEGIN(SpecialComment);
                      }
    "'"               { // start of a single quoted string
                        g_stringContext=YY_START;
                        g_copyString=0;
                        BEGIN( SingleQuoteString );
                      }
    "\""              { // start of a double quoted string
                        g_stringContext=YY_START;
                        g_copyString=0;
                        BEGIN( DoubleQuoteString );
                      }
    \n                { incLineNr(); }
    {IDENTIFIER}      // identifiers
    [^'"\.#a-z_A-Z\n]+  // other uninteresting stuff
    .                 // anything else
}

<FunctionDec>{

    {IDENTIFIER}            {
                              //found function name
                              if (current->type.isEmpty())
                              {
                                  current->type = "fun";
                              }
                              current->name = yytext;
                              current->name = current->name.stripWhiteSpace();
                              BEGIN( FunctionParams );
                            }
}

<FunctionParams>{
    {IDENTIFIER}        { // Name of parameter
                          lineCount();
                          Argument *a = new Argument;
                          current->argList->append(a);
                          current->argList->getLast()->name = QCString(yytext).stripWhiteSpace();
                          current->argList->getLast()->type = "";
                        }
   "_"                  { // Name of parameter
                          lineCount();
                          Argument *a = new Argument;
                          current->argList->append(a);
                          current->argList->getLast()->name = QCString(yytext).stripWhiteSpace();
                          current->argList->getLast()->type = "";
                        }
    "("                 { // Name of parameter
                          unput('(');
                          g_defVal.resize(0);
                          g_braceCount=0;
                          BEGIN( SmlPattern );
                        }
    "["                 { // Name of parameter
                          unput('[');
                          g_defVal.resize(0);
                          g_braceCount=0;
                          BEGIN( SmlPattern );
                        }
    "{"                 { // Name of parameter
                          unput('(');
                          g_defVal.resize(0);
                          g_braceCount=0;
                          BEGIN( SmlPattern );
                        }
    "="                 {
                          g_specialBlock = TRUE; // expecting a docstring
                          bodyEntry = current;
                          current->bodyLine  = yyLineNr;
			  endOfDef();
                          BEGIN( Search );
                        }
}

<SmlPattern>{
     [\(\[\{]           {
                          g_braceCount++;
                          g_defVal+=*yytext;
                        }
     [\)\]\}]           {
                          g_braceCount--;
                          if (g_braceCount==0) {
                            g_defVal+=*yytext;
                            lineCount();
                            Argument *a = new Argument;
                            current->argList->append(a);
                            current->argList->getLast()->name = QCString(g_defVal).stripWhiteSpace();
                            current->argList->getLast()->type = "";
                            BEGIN( FunctionParams );
                          }
                          else { g_defVal+=*yytext; }
                        }
    "\""                {
                          g_defVal+=*yytext;
                          BEGIN( SmlQuoteString );
                        }
    .                   { g_defVal+=*yytext; }

}


<ClassBody>{
    ^{BB}/\n          { // skip empty line
                        current->program+=yytext;
                      }
    <<EOF>>           {
                        endOfDef();
                        yyterminate();
                      }
    [^ \t\n#'"]+      { // non-special stuff
                        current->program+=yytext;
                      }
    {NEWLINE}         {
                        current->program+=*yytext;
                        incLineNr();
                      }
    .                 { // any character
                        current->program+=*yytext;
                      }
}

<ClassDec>{
    {IDENTIFIER}{B}"(".*")" {
                   if (current->type.isEmpty())
                   {
                     current->type = "structure";
                   }

                   current->section = Entry::CLASS_SEC;
                   current->name = yytext;

                   // prepend scope in case of nested classes
                   if (current_root->section&Entry::SCOPE_MASK)
                     current->name.prepend(current_root->name+"::");

                   current->name = current->name.stripWhiteSpace();
                   current->fileName = yyFileName;
                   docBlockContext   = YY_START;
                   docBlockInBody    = FALSE;
                   docBlockJavaStyle = FALSE;
                   docBlock.resize(0);

                   BEGIN(ClassInheritance);
                }

    {IDENTIFIER} {
                   if (current->type.isEmpty())
                   {
                     current->type = "structure";
                   }

                   current->section = Entry::CLASS_SEC;
                   current->name = yytext;

                   // prepend scope in case of nested classes
                   if (current_root->section&Entry::SCOPE_MASK)
                     current->name.prepend(current_root->name+"::");

                   current->name = current->name.stripWhiteSpace();
                   current->fileName = yyFileName;
                   docBlockContext   = YY_START;
                   docBlockInBody    = TRUE;
                   docBlockJavaStyle = FALSE;
                   docBlock.resize(0);

                   BEGIN(ClassInheritance);
                }
}

<ClassInheritance>{
   ({BB}|[\(,\)])      {}

    "="                { // begin of the class definition
                         g_specialBlock = TRUE; // expecting a docstring
                         current->bodyLine  = yyLineNr;
                         current->program.resize(0);
                         BEGIN(ClassBody);
                       }

    (":>")*{SCOPE}            {
                         current->extends->append(
                                              new BaseInfo(substitute(yytext,".","::"),Public,Normal)
                                            );
                         //Has base class-do stuff
                       }
}

<TripleComment>{
    {TRIDOUBLEQUOTE}   |
    {TRISINGLEQUOTE}   {
                          // printf("Expected module block %d special=%d\n",g_expectModuleDocs,g_specialBlock);
                          if (g_doubleQuote==(yytext[0]=='"'))
                          {
                            if (g_specialBlock) // expecting a docstring
                            {
                              QCString actualDoc=docBlock;
                              if (!docBlockSpecial) // legacy unformatted docstring
                              {
                                actualDoc.prepend("\\verbatim ");
                                actualDoc.append("\\endverbatim ");
                              }
                              //printf("-------> current=%p bodyEntry=%p\n",current,bodyEntry);
                              handleCommentBlock(actualDoc, FALSE);
                            }
                            else if (g_packageCommentAllowed) // expecting module docs
                            {
                              QCString actualDoc=docBlock;
                              if (!docBlockSpecial) // legacy unformatted docstring
                              {
                                actualDoc.prepend("\\verbatim ");
                                actualDoc.append("\\endverbatim ");
                              }
                              actualDoc.prepend("\\namespace "+g_moduleScope+"\\_linebr ");
                              handleCommentBlock(actualDoc, FALSE);
                            }
                            if ((docBlockContext==ClassBody /*&& !g_hideClassDocs*/))
                            {
                              current->program+=docBlock;
                              current->program+=yytext;
                            }
                            //if (g_hideClassDocs)
                            //{
                            //  current->startLine = yyLineNr;
                            //}
                            //g_hideClassDocs=FALSE;
                            BEGIN(docBlockContext);
                          }
                          else
                          {
                            docBlock += yytext;
                          }
                          g_packageCommentAllowed = FALSE;
                        }


    ^{BB}		{ // leading whitespace
                            docBlock += yytext;
                        }
    [^"'\n \t]+          {
                          docBlock += yytext;
                        }
    \n			{
                          incLineNr();
                          docBlock += yytext;
                        }
    .			{
                          docBlock += yytext;
                        }
}

<StandardComment>{
    "*)"		{ g_inComment = 0; BEGIN(Search); }
    \n/{B}"*)"		{ g_inComment = 0; BEGIN(Search); }
    \n/{B}"*"		{ incLineNr(); }
    [^*\n*)]+           { current->program+=yytext; }
    \n			{ incLineNr(); }
    .			{ current->program+=yytext; }

}

<SpecialComment>{
    "refstruct{"        { BEGIN(LinkToStructure); }
    "*)"		{
                          handleCommentBlock(docBlock, docBrief);
                          g_inComment = FALSE;
                          BEGIN(docBlockContext);
                        }
    "*"                 {}
    ^{B}"*"("*")*	{ // skip leading stars
                        }
    \n/{B}"*)"		{
                          handleCommentBlock(docBlock, docBrief);
                        g_inComment = FALSE;
                          BEGIN(docBlockContext);
                        }
    \n/{B}"*"		{ // continuation of the comment on the next line
                          docBlock+='\n';
                          docBrief = FALSE;
                          startCommentBlock(FALSE);
                          incLineNr();
                        }
    \n			{ // new line that ends the comment
                          //handleCommentBlock(docBlock, docBrief);
                          incLineNr();
                        }
    .			{ // anything we missed
                          docBlock+=*yytext;
                        }
}

<LinkToStructure>{
    {IDENTIFIER}        {
                          docBlock += yytext;
                          g_documentingStructure = yytext;
                        }
    "}"                 { BEGIN(SpecialComment); }
}

<SingleQuoteString>{
    \\{B}\n                    { // line continuation
                                 addToString(yytext);
                                 incLineNr();
                               }
    \\.                        { // espaced char
                                 addToString(yytext);
                               }
    "\"\"\""                   { // tripple double quotes
                                 addToString(yytext);
                               }
    "'"                        { // end of the string
                                 addToString(yytext);
                                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+                 { // normal chars
                                 addToString(yytext);
                               }
    .                          { // normal char
                                 addToString(yytext);
                               }
}

<DoubleQuoteString>{
    \\{B}\n                    { // line continuation
                                 addToString(yytext);
                                 incLineNr();
                               }
    \\.                        { // espaced char
                                 addToString(yytext);
                               }
    "'''"                      { // tripple single quotes
                                 addToString(yytext);
                               }
    "\""                       { // end of the string
                                 addToString(yytext);
                                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+                 { // normal chars
                                 addToString(yytext);
                               }
    .                          { // normal char
                                 addToString(yytext);
                               }
}

<SmlQuoteString>{
    \\{B}\n                    { // line continuation
                                 g_defVal+=*yytext;
                                 incLineNr();
                               }
    \\.                        { // espaced char
                                 g_defVal+=*yytext;
                               }
    "'''"                      { // tripple single quotes
                                 g_defVal+=*yytext;
                               }
    "\""                       { // end of the string
                                 g_defVal+=*yytext;
                                 BEGIN(SmlPattern);
                               }
    .                          { // normal char
                                 g_defVal+=*yytext;
                               }
}

<TripleString>{
    {TRIDOUBLEQUOTE}    |
    {TRISINGLEQUOTE}    {
                          *g_copyString += yytext;
                          if (g_doubleQuote==(yytext[0]=='"'))
                          {
                            BEGIN(g_stringContext);
                          }
                        }


    ({LONGSTRINGBLOCK}) {
                          lineCount();
                          *g_copyString += yytext;
                        }
    \n			{
                          incLineNr();
                          *g_copyString += yytext;
                        }
    .			{
                          *g_copyString += *yytext;
                        }
}

  /* ------------ End rules -------------- */

  /*
<*>({NONEMPTY}|{EXPCHAR}|{BB}) {}
  */

<*>{NEWLINE}                   { lineCount(); }
<*>.                           {}
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
      smlscannerYYrestart( smlscannerYYin ) ;
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
      yyLineNr   = ce->bodyLine ;
      if (current) delete current;
      current = new Entry;
      initEntry();

      groupEnterCompound(yyFileName,yyLineNr,ce->name);

      smlscannerYYlex() ;
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
  myStack.top = -1;

  inputString = fileBuf;
  inputPosition = 0;

  protection    = Public;
  mtype         = Method;
  gstat         = FALSE;
  virt          = Normal;
  current_root  = rt;
  g_specialBlock = FALSE;


  inputFile.setName(fileName);
  if (inputFile.open(IO_ReadOnly))
  {
    yyLineNr= 1 ;
    yyFileName = fileName;
    //setContext();
    msg("Parsing file %s...\n",yyFileName.data());

    QFileInfo fi(fileName);
    g_moduleScope = findPackageScope(fileName);
    QCString baseName=fi.baseName().utf8();
    if (baseName!="__init__") // package initializer file is not a package itself
    {
      if (!g_moduleScope.isEmpty())
      {
        g_moduleScope+="::";
      }
      g_moduleScope+=baseName;
    }

    current            = new Entry;
    initEntry();
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
    initEntry();
    smlscannerYYrestart( smlscannerYYin );
    BEGIN( Search );
    smlscannerYYlex();
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

  g_specialBlock = FALSE;
  g_packageCommentAllowed = FALSE;

  const char *orgInputString;
  int orgInputPosition;
  YY_BUFFER_STATE orgState;

  // save scanner state
  orgState = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(smlscannerYYin, YY_BUF_SIZE));
  orgInputString = inputString;
  orgInputPosition = inputPosition;

  // set new string
  inputString = text;
  inputPosition = 0;
  smlscannerYYrestart( smlscannerYYin );

  BEGIN( FunctionDec );

  smlscannerYYlex();
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

void smlscanFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION)
  if (g_lexInit)
  {
    smlscannerYYlex_destroy();
  }
#endif
}

//----------------------------------------------------------------------------

void SmlLanguageScanner::parseInput(const char *fileName,
                                       const char *fileBuf,
                                       Entry *root,
                                       bool /*sameTranslationUnit*/,
                                       QStrList & /*filesInSameTranslationUnit*/)
{
  g_thisParser = this;
  ::parseMain(fileName,fileBuf,root);

  // May print the AST for debugging purposes
  // printAST(global_root);
}

bool SmlLanguageScanner::needsPreprocessing(const QCString &)
{
  return FALSE;
}

void SmlLanguageScanner::parseCode(CodeOutputInterface &codeOutIntf,
    const char *scopeName,
    const QCString &input,
    SrcLangExt /*lang*/,
    bool isExampleBlock,
    const char *exampleName,
    FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    MemberDef *memberDef,
    bool showLineNumbers,
    Definition *searchCtx,
    bool collectXRefs
    )
{
  ::parseSmlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                    fileDef,startLine,endLine,inlineFragment,memberDef,
                    showLineNumbers,searchCtx,collectXRefs);
}

void SmlLanguageScanner::parsePrototype(const char *text)
{
  ::parsePrototype(text);

}

void SmlLanguageScanner::resetCodeParserState()
{
  ::resetSmlCodeParserState();
}

//----------------------------------------------------------------------------

#if !defined(YY_FLEX_SUBMINOR_VERSION)
//----------------------------------------------------------------------------
extern "C" { // some bogus code to keep the compiler happy
  void smlscannerYYdummy() { yy_flex_realloc(0,0); }
}
#endif
