/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

%{

/*
 *	includes
 */

#include <stdio.h>
#include <iostream.h>
#include <assert.h>
#include <ctype.h>

#include "qtbc.h"
#include <qarray.h>
#include <qstack.h>
#include <qfile.h>
#include <qstrlist.h>
#include <qdict.h>
#include <qregexp.h>
#include <qfileinfo.h>
  
#include "constexp.h"
#include "define.h"
#include "doxygen.h"
#include "message.h"
#include "util.h"
#include "defargs.h"

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

#define YY_NEVER_INTERACTIVE 1
  
#define DUMP_OUTPUT 0   // set this to one to see what the preprocessor 
                        // produces.
#define SHOW_INCLUDES 0 // set this to one to list all parsed include files

struct FileState
{
  int lineNr;
  FILE *filePtr;
  YY_BUFFER_STATE bufState;
  QCString fileName;
};  

/* -----------------------------------------------------------------
 *
 *	statics
 */

static int                yyLineNr   = 1;
static QCString           yyFileName;
static int                ifcount    = 0;
static QStrList          *pathList = 0;  
static QStack<FileState>  includeStack;
static QDict<int>        *argDict;
static int                defArgs = -1;
static QCString           defName;
static QCString           defText;
static QCString           defArgsStr;
static bool               defVarArgs;
static int                level;
static int                lastCContext;
static int                lastCPPContext;
static QArray<int>        levelGuard;
static QCString           guardExpr;
static BufStr            *outputBuf;
static int                roundCount;
static bool               quoteArg;
static DefineDict        *fileDefineDict;
static DefineDict        *expandedDict;
static int                findDefArgContext;

static QCString           lastGuardName;


static void incrLevel()
{
  level++;
  levelGuard.resize(level);
  levelGuard[level-1]=FALSE;
  //printf("%s line %d: incrLevel %d\n",yyFileName.data(),yyLineNr,level);
}

static void decrLevel()
{
  //printf("%s line %d: decrLevel %d\n",yyFileName.data(),yyLineNr,level);
  if (level > 0)
  {
    level--;
    levelGuard.resize(level);
  }
  else
  {
    err("Error: More #endif's than #if's found.\n");
  }
}

static bool otherCaseDone()
{
  return levelGuard[level-1];
}

static void setCaseDone(bool value)
{
  levelGuard[level-1]=value;
}

static Define *isDefined(const char *name)
{
  if (name)
  {
    Define *def;
    //if ((def=fileDefineCache->findDefine(yyFileName,name)) && !def->undef) 
    //	return def;
    if ((def=fileDefineDict->find(name)) && !def->undef) return def; 
  }
  return 0;
}

static FILE *findFile(const char *fileName)
{
  if (pathList==0) 
  {
    return 0;
  }
  char *s=pathList->first();
  while (s)
  {
    QCString absName=(QCString)s+"/"+fileName;
    QFileInfo fi(absName);
    if (fi.exists())
    {
      FILE *f;
      if (!Config::inputFilter.isEmpty())
      {
	QCString cmd = Config::inputFilter+" "+absName;
	f=popen(cmd,"r");
	if (!f) warn("Warning: could not execute filter %s\n",cmd.data());
      }
      else
      {
	f=fopen(absName,"r");
	if (!f) warn("Warning: could not open file %s for reading\n",absName.data());
      }
      if (f)
      {
	yyFileName=absName;
	yyLineNr=1;
	return f;
      }
    }
    s=pathList->next();
  } 
  return 0;
}


static int getNextChar(const QCString &expr,QCString *rest,uint &pos);
static int getCurrentChar(const QCString &expr,QCString *rest,uint pos);
static void unputChar(const QCString &expr,QCString *rest,uint &pos,char c);
static void expandExpression(QCString &expr,QCString *rest,int pos);

static QCString stringize(const QCString &s)
{
  QCString result;
  uint i=0;
  bool inString=FALSE;
  bool inChar=FALSE;
  char c,pc;
  while (i<s.length())
  {
    if (!inString && !inChar)
    {
      while (i<s.length() && !inString && !inChar)
      {
	c=s.at(i++);
	if (c=='"')
	{
	  result+="\\\"";
	  inString=TRUE;
	}
	else if (c=='\'')
	{
	  result+=c;
	  inChar=TRUE;
	}
	else
	{
	  result+=c;
	}
      }
    }
    else if (inChar)
    {
      while (i<s.length() && inChar)
      {
	c=s.at(i++);
	if (c=='\'')
	{
	  result+='\'';
	  inChar=FALSE;
	}
	else if (c=='\\')
	{
	  result+="\\\\";
	}
	else
	{
	  result+=c;
	}
      }
    }
    else
    {
      pc=0;
      while (i<s.length() && inString)
      {
	char c=s.at(i++);
	if (c=='"') 
	{
	  result+="\\\"";
	  inString= pc=='\\';
	}
	else if (c=='\\')
	  result+="\\\\";
	else
	  result+=c;
	pc=c;
      }
    }
  }
  //printf("stringize `%s'->`%s'\n",s.data(),result.data());
  return result;
}

/*! Execute all ## operators in expr. 
 * If the macro name before or after the operator contains a no-rescan 
 * marker (@-) then this is removed (before the concatenated macro name
 * may be expanded again.
 */
static void processConcatOperators(QCString &expr)
{
  QRegExp r("[ \\t\\n]*##[ \\t\\n]*"); 
  int l,n,i=0;
  while ((n=r.match(expr,i,&l))!=-1)
  {
    if (n+l+1<(int)expr.length() && expr.at(n+l)=='@' && expr.at(n+l+1)=='-')
    {
      // remove no-rescan marker after ID
      l+=2;
    }
    // remove the ## operator and the surrounding whitespace
    expr=expr.left(n)+expr.right(expr.length()-n-l);
    int k=n-1;
    while (k>=0 && isId(expr.at(k))) k--; 
    if (k>0 && expr.at(k)=='-' && expr.at(k-1)=='@')
    {
      // remove no-rescan marker before ID
      expr=expr.left(k-1)+expr.right(expr.length()-k-1);
      n-=2;
    }
    i=n+l;
  }
}

/*! replaces the function macro \a def whose argument list starts at
 * \a pos in expression \a expr. 
 * Notice that this routine may scan beyond the \a expr string if needed.
 * The characters from the input file will be read.
 * The replacement string will be returned in \a result and the 
 * length of the (unexpanded) argument list is stored in \a len.
 */ 
static bool replaceFunctionMacro(const QCString &expr,QCString *rest,int pos,int &len,const Define *def,QCString &result)
{
  //printf("replaceFunctionMacro(expr=%s,rest=%s,pos=%d,def=%s) level=%d\n",expr.data(),rest ? rest->data() : 0,pos,def->name.data(),level);
  //bool replaced=FALSE;
  uint j=pos;
  len=0;
  result.resize(0);
  int cc;
  while ((cc=getCurrentChar(expr,rest,j))!=EOF && cc==' ') 
  { 
    len++; 
    getNextChar(expr,rest,j); 
  }
  if (cc!='(') 
  { 
    unputChar(expr,rest,j,' '); 
    return FALSE; 
  }
  getNextChar(expr,rest,j); // eat the `(' character

  //while (j<expr.length() && expr.at(j)!='(') j++; 
  //j++; // skip opening paren

  QDict<QCString> argTable;  // list of arguments
  argTable.setAutoDelete(TRUE);
  QCString arg;
  int argCount=0;
  bool done=FALSE;
  
  // FASE 1: read the macro arguments
  if (def->nargs==0)
  {
    while ((cc=getNextChar(expr,rest,j))!=EOF)
    {
      char c = (char)cc;
      if (c==')') break;
    }
  }
  else
  {
    while (!done && (argCount<def->nargs || def->varArgs) && 
	((cc=getNextChar(expr,rest,j))!=EOF)
	  )
    {
      char c=(char)cc;
      if (c=='(') // argument is a function => search for matching )
      {
	int level=1;
	arg+=c;
	char term='\0';
	while ((cc=getNextChar(expr,rest,j))!=EOF)
	{
	  char c=(char)cc;
	  if (c=='\'' || c=='\"') // skip ('s and )'s inside strings
	  {
	    if (term!='\0') 
	    { 
	      if (c==term && expr.at(j-2)!='\\') term='\0'; 
	    } 
	    else 
	    { 
	      term=c; 
	    }
	  }
	  if (term=='\0' && c==')')
	  {
	    level--;
	    arg+=c;
	    if (level==0) break;
	  }
	  else if (term=='\0' && c=='(')
	  {
	    level++;
	    arg+=c;
	  }
	  else
	    arg+=c;
	}
      }
      else if (c==')' || c==',') // last or next argument found
      {
	if (c==',' && argCount==def->nargs-1 && def->varArgs)
	{
	  arg=arg.stripWhiteSpace();
	  arg+=',';
	}
	else
	{
	  QCString argKey;
	  argKey.sprintf("@%d",argCount++); // key name
	  arg=arg.stripWhiteSpace();
	  // add argument to the lookup table
	  argTable.insert(argKey, new QCString(arg));
	  arg.resize(0);
	  if (c==')') // end of the argument list
	  {
	    done=TRUE;
	  }
	}
      } 
      else if (c=='\"') // append literal strings
      {
	arg+=c; 
	char pc=c;
	bool found=FALSE;
	while (!found && (cc=getNextChar(expr,rest,j))!=EOF)
	{
	  found = pc!='\\' && cc=='"';
	  c=(char)cc;	  
	  pc=c;
	  arg+=c;
	}
      }
      else if (c=='\'') // append literal characters
      {
	arg+=c;
	char pc=c;
	bool found=FALSE;
	while (!found && (cc=getNextChar(expr,rest,j))!=EOF)
	{
	  found = pc!='\\' && cc=='\'';
	  c=(char)cc;
	  pc=c;
	  arg+=c;
	}
      }	    
      else // append other characters
      {
	arg+=c;
      }
    }
  }

  // FASE 2: apply the macro function
  if (argCount==def->nargs || 
      (argCount>def->nargs && def->varArgs)) // matching parameters lists
  {
    uint k=0;
    // substitution of all formal arguments
    QCString resExpr;
    const QCString d=def->definition.stripWhiteSpace();
    bool inString=FALSE;
    while (k<d.length())
    {
      if (d.at(k)=='@') // maybe a marker, otherwise an escaped @
      {
	if (d.at(k+1)=='@') // escaped @ => copy it (is unescaped later)
	{
	  k+=2;
	  resExpr+="@@"; // we unescape these later
	}
	else if (d.at(k+1)=='-') // no-rescan marker
	{
	  k+=2;
	  resExpr+="@-";
	}
	else // argument marker => read the argument number
	{
	  QCString key="@";
	  QCString *subst=0;
	  bool hash=FALSE;
	  int l=k-1;
	  // search for ## backward
	  if (l>=0 && d.at(l)=='"') l--;
	  while (l>=0 && d.at(l)==' ') l--;
	  if (l>0 && d.at(l)=='#' && d.at(l-1)=='#') hash=TRUE;
	  k++;
	  // scan the number
	  while (k<d.length() && d.at(k)>='0' && d.at(k)<='9') key+=d.at(k++);
	  if (!hash) 
	  {
	    // search for ## forward
	    l=k;
	    if (l<(int)d.length() && d.at(l)=='"') l++;
	    while (l<(int)d.length() && d.at(l)==' ') l++;
	    if (l<(int)d.length()-1 && d.at(l)=='#' && d.at(l+1)=='#') hash=TRUE;
	  }
	  //printf("request key %s result %s\n",key.data(),args[key]->data());
	  if (key.length()>1 && (subst=argTable[key])) 
	  {
	    QCString substArg=*subst;
	    // only if no ## operator is before or after the argument
	    // marker we do macro expansion.
	    if (!hash) expandExpression(substArg,0,0);
	    if (inString)
	    {
	      //printf("`%s'=stringize(`%s')\n",stringize(*subst).data(),subst->data());

	      // if the marker is inside a string (because a # was put 
	      // before the macro name) we must escape " and \ characters
	      resExpr+=stringize(substArg);
	    }
	    else
	    {
	      resExpr+=substArg;
	    }
	  }
	}
      }
      else // no marker, just copy
      {
	if (!inString && d.at(k)=='\"') 
	{
	  inString=TRUE; // entering a literal string
	}
	else if (inString && d.at(k)=='\"' && d.at(k-1)!='\\')
	{
	  inString=FALSE; // leaving a literal string
	}
	resExpr+=d.at(k++);
      }
    }
    len=j-pos;
    result=resExpr;
    //printf("result after substitution `%s' expr=`%s'\n",
    //       result.data(),expr.mid(pos,len).data());
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*! returns the next identifier in string \a expr by starting at position \a p.
 * The position of the identifier is returned (or -1 if nothing is found)
 * and \a l is its length. Any quoted strings are skipping during the search.
 */
static int getNextId(const QCString &expr,int p,int *l)
{
  int n;
  while (p<(int)expr.length())
  {
    char c=expr.at(p++);
    if (isalpha(c) || c=='_') // read id
    {
      n=p-1;
      while (p<(int)expr.length() && isId(expr.at(p))
	    ) p++;
      *l=p-n;
      return n; 
    }
    else if (c=='"') // skip string
    {
      char pc=c;
      if (p<(int)expr.length()) c=expr.at(p);
      while (p<(int)expr.length() && (c!='"' || pc=='\\')) 
      {
	pc=c;
	c=expr.at(p);
	p++;
      }
    }
  }
  return -1;
}

/*! preforms recursive macro expansion on the string \a expr
 *  starting at position \a pos.
 *  May read additional characters from the input while re-scanning!
 *  If \a expandAll is \c TRUE then all macros in the expression are
 *  expanded, otherwise only the first is expanded.
 */
static void expandExpression(QCString &expr,QCString *rest,int pos)
{
  //printf("expandExpression(%s,%s)\n",expr.data(),rest ? rest->data() : 0);
  QCString macroName;
  QCString expMacro;
  int i=pos,l,p,len;
  while ((p=getNextId(expr,i,&l))!=-1) // search for an macro name
  {
    bool replaced=FALSE;
    macroName=expr.mid(p,l);
    //printf("macroName %s found\n",macroName.data());
    if (p<2 || !(expr.at(p-2)=='@' && expr.at(p-1)=='-')) // no-rescan marker?
    {
      if (expandedDict->find(macroName)==0) // expand macro
      {
	Define *def=isDefined(macroName);
	//printf("name is not an expanded macro def=%s\n",def ? def->name.data() : 0);
	if (def && def->nargs==-1) // simple macro
	{
	  // substitute the definition of the macro
	  expMacro=def->definition.stripWhiteSpace();
	  replaced=TRUE;
	  len=l;
	  //printf("simple macro expansion=`%s'->`%s'\n",macroName.data(),expMacro.data());
	}
	else if (def && def->nargs>=0) // function macro
	{
	  replaced=replaceFunctionMacro(expr,rest,p+l,len,def,expMacro);
	  len+=l;
	}

	if (replaced) // expand the macro and rescan the expression
	{
	    
	  //printf("replacing `%s'->`%s'\n",expr.mid(p,len).data(),expMacro.data());
	  QCString resultExpr=expMacro;
	  QCString restExpr=expr.right(expr.length()-len-p);
	  processConcatOperators(resultExpr);
	  if (!def->nonRecursive)
	  {
	    expandedDict->insert(macroName,def);
	    expandExpression(resultExpr,&restExpr,0);
	    expandedDict->remove(macroName);
	  }
	  expr=expr.left(p)+resultExpr+restExpr;
	  i=p;
	  //printf("new expression: %s\n",expr.data());
	}
	else // move to the next macro name
	{
	  //printf("moving to the next macro old=%d new=%d\n",i,p+l);
	  i=p+l;
	}
      }
      else // move to the next macro name
      {
	expr=expr.left(p)+"@-"+expr.right(expr.length()-p);
	//printf("macro already expanded, moving to the next macro expr=%s\n",expr.data());
	i=p+l+2;
	//i=p+l;
      }
    }
    else // no re-scan marker found, skip the macro name
    {
      //printf("skipping marked macro\n");
      i=p+l;
    }
  }
}

/*! replaces all occurrences of @@ in \a s by @
 *  All identifiers found are replaced by 0L
 *  \par assumption: 
 *   \a s only contains pairs of @@'s.
 */
QCString removeIdsAndMarkers(const char *s)
{
  //printf("removeIdsAndMarkers(%s)\n",s);
  const char *p=s;
  char c;
  bool inNum=FALSE;
  QCString result;
  if (p)
  {
    while ((c=*p))
    {
      if (c=='@') // replace @@ with @
      {
	if (*(p+1)=='@')
	{
	  result+=c; 
	}
	p+=2;
      }
      else if (isdigit(c))
      {
	result+=c;
	p++;
        inNum=TRUE;	
      }
      else if ((isalpha(c) || c=='_') && !inNum) // replace identifier with 0L
      {
	result+="0L";
	p++;
	while ((c=*p) && isId(c)) p++;
      }
      else 
      {
	result+=c;
	char lc=tolower(c);
	if (lc!='l' && lc!='u') inNum=FALSE;
	p++;
      }
    }
  }
  return result;
}

/*! replaces all occurrences of @@ in \a s by @
 *  \par assumption: 
 *   \a s only contains pairs of @@'s
 */
QCString removeMarkers(const char *s)
{
  const char *p=s;
  char c;
  QCString result;
  if (p)
  {
    while ((c=*p))
    {
      if (c=='@') // replace @@ with @
      {
	if (*(p+1)=='@')
	{
	  result+=c; 
	}
	p+=2;
      }
      else
      {
	result+=c;
	p++;
      }
    }
  }
  return result;
}

/*! compute the value of the expression in string \a expr.
 *  If needed the function may read additional characters from the input.
 */

bool computeExpression(const QCString &expr)
{
  QCString e=expr;
  expandExpression(e,0,0);
  e = removeIdsAndMarkers(e);
  if (e.length()==0) return FALSE;
  return parseCppExpression(e);
}

/*! expands the macro definition in \a name
 *  If needed the function may read additional characters from the input
 */

QCString expandMacro(const QCString &name)
{
  QCString n=name;
  expandExpression(n,0,0);
  n=removeMarkers(n);
  //printf("expandMacro `%s'->`%s'\n",name.data(),n.data());
  return n;
}

Define *newDefine()
{
  Define *def=new Define;
  def->name = defName;
  def->definition = defText.stripWhiteSpace();
  def->nargs = defArgs;
  def->fileName = yyFileName; 
  def->lineNr = yyLineNr;
  def->varArgs = defVarArgs;
  return def;
}

void addDefine()
{
  bool ambig;
  FileDef   *fd=findFileDef(&inputNameDict,yyFileName,ambig);
  MemberDef *md=new MemberDef("#define",defName,defArgsStr,0,
      Public,Normal,FALSE,FALSE,MemberDef::Define,0,0);
  ArgumentList *argList = new ArgumentList;
  stringToArgumentList(defArgsStr,argList);
  md->setArgumentList(argList);
  md->setDefFile(yyFileName);
  md->setDefLine(yyLineNr);
  md->setFileDef(fd);
  md->setDefinition("#define "+defName);

  MemberName *mn=functionNameDict[defName];
  if (mn==0)
  {
    mn = new MemberName(defName);
    functionNameList.inSort(mn);
    functionNameDict.insert(defName,mn);
  }
  mn->append(md);
  if (fd) fd->insertMember(md);
  
  Define *d;
  if ((d=defineDict[defName])==0) defineDict.insert(defName,newDefine()); 
  
}

static void outputChar(char c)
{
  if (includeStack.isEmpty()) outputBuf->addChar(c);
} 

static void outputArray(const char *a,int len)
{
  if (includeStack.isEmpty()) outputBuf->addArray(a,len);
}

static void readIncludeFile(const QCString &inc)
{
  if (!Config::searchIncludeFlag) return; // do not read include files
  uint i=0;
  // find the start of the include file name
  while (i<inc.length() && 
         (inc.at(i)==' ' || inc.at(i)=='"' || inc.at(i)=='<')
	) i++;
  uint s=i;
  // find the end of the include file name
  while (i<inc.length() &&
      inc.at(i)!='"' && inc.at(i)!='>') i++; 
  if (s<inc.length() && i>s) // valid include file name found
  {
    QCString incFileName=inc.mid(s,i-s).stripWhiteSpace(); 

    FILE *f;
    QCString oldFileName=yyFileName.copy();
    if ((f=findFile(incFileName))) // see if the include file can be found
    {
#if SHOW_INCLUDES
      for (i=0;i<includeStack.count();i++) msg("  ");
      msg("#include %s: parsing...\n",incFileName.data());
#endif
      // store the state of the old file 
      FileState *fs=new FileState;
      fs->bufState=YY_CURRENT_BUFFER;
      fs->lineNr=yyLineNr;
      fs->fileName=oldFileName;
      fs->filePtr=f;
      // push the state on the stack
      includeStack.push(fs);
      // set the scanner to the include file
      preYYin=f;
      yy_switch_to_buffer(yy_create_buffer(preYYin, YY_BUF_SIZE));
    } 
    else
    {
#if SHOW_INCLUDES
      msg("#include %s: not found! skipping...\n",incFileName.data());
      //printf("Error: include file %s not found\n",yytext);
#endif
    }
  }
}

/* ----------------------------------------------------------------- */

%}

ID	[a-z_A-Z][a-z_A-Z0-9]*
B       [ \t]
BN	[ \t\r\n]

%option noyywrap

%x      Start
%x	Command
%x	SkipCommand
%x	SkipLine
%x	CopyLine
%x      Include
%x      IncludeID
%x	DefName
%x	DefineArg
%x	DefineText
%x      SkipCPPBlock
%x      Ifdef
%x      Ifndef
%x	SkipCComment
%x	SkipCPPComment
%x	RemoveCComment
%x	RemoveCPPComment
%x	Guard
%x	DefinedExpr1
%x	DefinedExpr2
%x	SkipDoubleQuote
%x	SkipSingleQuote
%x	UndefName
%x	IgnoreLine
%x	FindDefineArgs
%x	ReadString

%%

<*>\x06					
<*>\x00
<*>\r
  /*
<Start>^{B}*([^ \t#\n\/][^\n]*)?"\n"	{ 
  					  //printf("%s line %d: %s",yyFileName.data(),yyLineNr,yytext); 
					  if (includeStack.isEmpty())
					  {
					    //preprocessedFile+=yytext;
					    //char *s=yytext,c;
					    //if (s) while ((c=*s++)) *dataPtr++=c;
					    outputBuf->addArray(yytext,yyleng);
					  }
  					  yyLineNr++;
					}
  */
<Start>^{B}*"#"				{ BEGIN(Command); }
<Start>^{B}*/[^#]			{
 					  outputArray(yytext,yyleng); 
  					  BEGIN(CopyLine); 
					}
  /*
<CopyLine>[^\n/]+			{
  					  outputArray(yytext,yyleng);
  					}
  */
<CopyLine>{ID}/{BN}*"("			{
  					  Define *def=0;
					  //printf("Search for define %s\n",yytext);
					  if (includeStack.isEmpty() &&
					      Config::macroExpansionFlag &&
					      /* (expandDefine=fileDefineCache->findDefine(yyFileName,yytext)) */
					      (def=fileDefineDict->find(yytext)) &&
					      (!Config::onlyPredefinedFlag || def->isPredefined)
					     )
					  {
					    //printf("Found it!\n");
					    roundCount=0;
					    defArgsStr=yytext;
					    if (def->nargs==-1) // no function macro
					    {
					      QCString result = expandMacro(defArgsStr);
					      //printf("result=`%s'\n",result.data());
					      outputArray(result,result.length());
					    }
					    else // zero or more arguments
					    {
					      findDefArgContext = CopyLine;
					      BEGIN(FindDefineArgs);
					    }
					  }
					  else
					  {
					    outputArray(yytext,yyleng);
					  }
  					}
<CopyLine>{ID}				{
                                          Define *def=0;
					  //printf("Search for define %s\n",yytext);
  					  if (includeStack.isEmpty() && 
					      Config::macroExpansionFlag &&
					      (def=fileDefineDict->find(yytext)) &&
					      def->nargs==-1 &&
					      (!Config::onlyPredefinedFlag || def->isPredefined)
					     )
					  {
					    //printf("Found it!\n");
                                            QCString name=yytext;
					    QCString result=expandMacro(name); 
					    //printf("result=`%s'\n",result.data());
					    outputArray(result,result.length());
					  }
					  else
					  {
					    outputArray(yytext,yyleng);
					  }
  					}
<CopyLine>.				{
  					  outputChar(*yytext);
  					}
<CopyLine>\n				{
  					  outputChar('\n');
					  BEGIN(Start);
					  yyLineNr++;
  					}
<FindDefineArgs>"("			{
  					  defArgsStr+='(';
  					  roundCount++;
  					}
<FindDefineArgs>")"			{
  					  defArgsStr+=')';
					  roundCount--;
					  if (roundCount==0)
					  {
					    //printf("defArgsStr=`%s'\n",defArgsStr.data());
					    QCString result=expandMacro(defArgsStr);
					    if (findDefArgContext==CopyLine)
					    {
					      outputArray(result,result.length());
					      BEGIN(findDefArgContext);
					    }
					    else // findDefArgContext==IncludeID
					    {
					      readIncludeFile(result);
					      BEGIN(Start);
					    }
					  }
  					}
  /*
<FindDefineArgs>")"{B}*"("		{
  					  defArgsStr+=yytext;
  					}
  */
<FindDefineArgs>"\""			{
  					  defArgsStr+=*yytext;
  					  BEGIN(ReadString);
  					}
<FindDefineArgs>\n			{
  					  yyLineNr++;
					  outputChar('\n');
  					}
<FindDefineArgs>"@"			{
  					  defArgsStr+="@@";
  					}
<FindDefineArgs>.			{
  					  defArgsStr+=*yytext;
  					}
<ReadString>"\\\""			{
  					  defArgsStr+=yytext;
  					}
<ReadString>"\""			{
  					  defArgsStr+=*yytext;
					  BEGIN(FindDefineArgs);
  					}
<ReadString>"//"|"/*"			{
  					  defArgsStr+=yytext;
  					}
<ReadString>.				{
  					  defArgsStr+=*yytext;
  					}
<Command>"include"{B}*/{ID}		{
  					  if (Config::macroExpansionFlag) 
					    BEGIN(IncludeID);
  					}
<Command>"include"{B}*[<"]		{ 
  					  BEGIN(Include); 
					}
<Command>"define"{B}+			{ 
  			                  //printf("!!!DefName\n"); 
  					  BEGIN(DefName); 
					}
<Command>"ifdef"/{B}*"("		{
  					  incrLevel();
					  guardExpr.resize(0);
  					  BEGIN(DefinedExpr2);
  					}
<Command>"ifdef"/{B}+			{
  					  //printf("Pre.l: ifdef\n");
  					  incrLevel();
					  guardExpr.resize(0);
  					  BEGIN(DefinedExpr1);
  					}
<Command>"ifndef"/{B}*"("		{
  					  incrLevel();
					  guardExpr="! ";
  					  BEGIN(DefinedExpr2);
					}
<Command>"ifndef"/{B}+			{
  					  incrLevel();
					  guardExpr="! ";
  					  BEGIN(DefinedExpr1);
  					}
<Command>"if"/[ \t(]			{
  					  incrLevel();
					  guardExpr.resize(0);
					  BEGIN(Guard);
					}
<Command>"elif"/[ \t(]			{
  					  if (!otherCaseDone())
					  {
					    guardExpr.resize(0);
					    BEGIN(Guard);  
					  }
					  else
					  {
					    ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<Command>"else"/[^a-z_A-Z0-9]		{
					  //printf("else levelGuard[%d]=%d\n",level-1,levelGuard[level-1]);
  					  if (otherCaseDone())
					  {
					    ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
					  else
					  {
					    setCaseDone(TRUE);
					    //levelGuard[level-1]=TRUE;
					  } 
  					}
<Command>"undef"{B}+			{
  					  BEGIN(UndefName);
  					}
<Command>"elif"/[ \t(]			{
  					  if (!otherCaseDone())
					  {
					    guardExpr.resize(0);
  					    BEGIN(Guard);
					  }
  					}
<Command>"endif"/[^a-z_A-Z0-9]		{
  					  //printf("Pre.l: #endif\n");
  					  decrLevel();
  					}
<Command,IgnoreLine>\n			{
  					  outputChar('\n');
  					  BEGIN(Start);
					  yyLineNr++;
  					}
<Command>{ID}				{ // unknown directive
					  BEGIN(IgnoreLine);
					}
<IgnoreLine>.
<Command>.
<UndefName>{ID}				{
  					  Define *def;
  					  if ((def=isDefined(yytext)))
					  {
					    //printf("undefining %s\n",yytext);
					    def->undef=TRUE;
					  }
					  BEGIN(Start);
  					}
<Guard>\\\n				{
  					  outputChar('\n');
  					  guardExpr+=' ';
					  yyLineNr++;
  					}
<Guard>"defined"/{B}*"("		{
    					  BEGIN(DefinedExpr2);
    					}
<Guard>"defined"/{B}+			{
    					  BEGIN(DefinedExpr1);
    					}
<Guard>.				{ guardExpr+=*yytext; }
<Guard>\n				{
  					  outputChar('\n');
					  yyLineNr++;
  					  //printf("Guard: `%s'\n",
					  //    guardExpr.data());
					  bool guard=computeExpression(guardExpr);
					  setCaseDone(guard);
					  //printf("if levelGuard[%d]=%d\n",level-1,levelGuard[level-1]);
					  if (guard)
					  {
					    BEGIN(Start);
					  } 
					  else
					  {
					    ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<DefinedExpr1,DefinedExpr2>\\\n		{ yyLineNr++; outputChar('\n'); }
<DefinedExpr1>{ID}			{
  					  if (isDefined(yytext))
					    guardExpr+=" 1L ";
					  else
					    guardExpr+=" 0L ";
					  lastGuardName=yytext;
					  BEGIN(Guard);
  					}
<DefinedExpr2>{ID}			{
  					  if (isDefined(yytext))
					    guardExpr+=" 1L ";
					  else
					    guardExpr+=" 0L ";
					  lastGuardName.resize(0);
  					}
<DefinedExpr1,DefinedExpr2>\n		{ // should not happen, handle anyway
  					  ifcount=0;
 					  BEGIN(SkipCPPBlock); 
					}
<DefinedExpr2>")"			{
  					  BEGIN(Guard);
  					}
<DefinedExpr1,DefinedExpr2>.
<SkipCPPBlock>^{B}*"#"			{ BEGIN(SkipCommand); }
<SkipCPPBlock>^{B}*/[^#]		{ BEGIN(SkipLine); }
<SkipCPPBlock>.
<SkipCommand>"if"(("n")?("def"))?/[ \t(]	{ 
  					  incrLevel();
                                          ifcount++; 
  					  //printf("#if... depth=%d\n",ifcount);
					}
<SkipCommand>"else"			{
					  //printf("Else! ifcount=%d otherCaseDone=%d\n",ifcount,otherCaseDone());
  					  if (ifcount==0 && !otherCaseDone())
					  {
					    setCaseDone(TRUE);
  					    //outputChar('\n');
					    BEGIN(Start);
					  }
  					}
<SkipCommand>"elif"/[ \t(]		{
  					  if (ifcount==0) 
					  {
  					    if (!otherCaseDone())
					    {
					      guardExpr.resize(0);
					      lastGuardName.resize(0);
  					      BEGIN(Guard);
					    }
					    else
					    {
					      BEGIN(SkipCPPBlock);
					    }
					  }
					}
<SkipCommand>"endif"			{ 
  					  decrLevel();
  				          if (--ifcount<0)
  					  {
  					    //outputChar('\n');
					    BEGIN(Start);
					  }
					}
<SkipCommand>\n				{ 
  					  outputChar('\n');
  					  yyLineNr++; 
					  BEGIN(SkipCPPBlock);
					}
<SkipCommand>{ID}			{ // unknown directive 
  					  BEGIN(SkipLine); 
					}
<SkipCommand>.
<SkipLine>[^/\n]+			
<SkipLine>.
<SkipLine,SkipCommand,SkipCPPBlock>"//"	{
  					  lastCPPContext=YY_START;
  					  BEGIN(RemoveCPPComment);
					}
<SkipLine,SkipCommand,SkipCPPBlock>"/*"	{
					  lastCContext=YY_START;
  					  BEGIN(RemoveCComment);
  					}
<SkipLine>\n				{
  					  outputChar('\n');
					  yyLineNr++;  
					  BEGIN(SkipCPPBlock);
					}
<IncludeID>{ID}{B}*/"("			{
				          roundCount=0;
					  defArgsStr=yytext;
					  findDefArgContext = IncludeID;
					  BEGIN(FindDefineArgs);
					}
<IncludeID>{ID}				{
                                          readIncludeFile(expandMacro(yytext));
					  BEGIN(Start);
  					}
<Include>[^\">\n]+[\">]			{ 
					  QCString incName=yytext;
					  //int l=incName.length();
					  //QCString incFileName=incName.left(l-1);
					  //if (fileDefineCache->fileCached(incFileName))
					  //{
					  //  printf("file already cached!\n");
					  //  fileDefineCache->merge(incFileName,yyFileName);
					  //}
					  //else if ((f=findFile(incFileName)))
					  readIncludeFile(incName);
					  
					  BEGIN(Start);
  					}
<DefName>{ID}/"("			{
  					  //printf("Define() `%s'\n",yytext);
					  argDict = new QDict<int>(31);
					  argDict->setAutoDelete(TRUE);
					  defArgs = 0; 
                                          defArgsStr.resize(0);
					  defText.resize(0);
					  defName = yytext;
					  defVarArgs = FALSE;
					  BEGIN(DefineArg);
  					}
<DefName>{ID}/{B}*			{
  					  //printf("Define `%s'\n",yytext);
  					  argDict = 0;
					  defArgs = -1;
                                          defArgsStr.resize(0);
					  defText.resize(0);
					  defName = yytext;
					  defVarArgs = FALSE;
					  QCString tmp=(QCString)"#define "+defName+defArgsStr;
					  outputArray(tmp.data(),tmp.length());
					  quoteArg=FALSE;
					  BEGIN(DefineText); 
  					}
<DefName>{ID}/{B}*"\n"			{
  					  argDict = 0;
					  defArgs = -1;
					  defName = yytext;
                                          defArgsStr.resize(0);
					  defText.resize(0);
					  defVarArgs = FALSE;
					  if ( defName!=lastGuardName )
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+defName+defArgsStr;
					    outputArray(tmp.data(),tmp.length());
					    quoteArg=FALSE;
					    BEGIN(DefineText);
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
  					    Define *def=0;
					    //if (includeStack.isEmpty()) 
					    //{
					    //  addDefine();
					    //}
					    if (/*!Config::onlyPredefinedFlag &&*/ (def=fileDefineDict->find(defName))==0)
					    {
					      fileDefineDict->insert(defName,newDefine());
					    }
					    else if (def)// name already exists
					    {
					      if (def->undef) // undefined name
					      {
					        def->undef = FALSE;
					        def->name = defName;
					        def->definition = defText.stripWhiteSpace();
					        def->nargs = defArgs;
					        def->fileName = yyFileName; 
					        def->lineNr = yyLineNr;
					      }
					      else
					      {
					        //printf("Error: define %s is defined more than once!\n",defName.data());
					      }
					    }
  					    //outputChar('\n');
					    lastGuardName.resize(0);
					    BEGIN(Start);
					  }
  					}
<DefineArg>","{B}*			{ defArgsStr+=yytext; }
<DefineArg>"("{B}*                      { defArgsStr+=yytext; }
<DefineArg>{B}*")"{B}*			{
                                          defArgsStr+=yytext; 
					  QCString tmp=(QCString)"#define "+defName+defArgsStr;
					  outputArray(tmp.data(),tmp.length());
					  quoteArg=FALSE;
  					  BEGIN(DefineText);
  					}
<DefineArg>{ID}("..."?)			{
  					  //printf("Define addArg(%s)\n",yytext);
  					  QCString argName=yytext;
  					  defVarArgs = yytext[yyleng-1]=='.';
					  if (defVarArgs) // strip ellipsis
					    argName=argName.left(argName.length()-3);
                                          defArgsStr+=yytext;
					  argDict->insert(argName,new int(defArgs)); 
					  defArgs++;
  					}
<DefineText>"/*"			{
  					  outputChar('/');outputChar('*');
					  defText+=' ';
					  lastCContext=YY_START;
  					  BEGIN(SkipCComment);
  					}
<DefineText>"//"			{
  				          outputChar('/');outputChar('/');
  					  lastCPPContext=YY_START;
  					  BEGIN(SkipCPPComment);
  					}
<SkipCComment>"*/"			{
  					  outputChar('*');outputChar('/');
  					  BEGIN(lastCContext);  
  					}
<SkipCComment>"//"			{
  					  outputChar('/');outputChar('/');
  					}
<SkipCComment>"/*"			{
  					  outputChar('/');outputChar('*');
  					}
<SkipCComment>[^*\n]+			{
  					  outputArray(yytext,yyleng);
  					}
<SkipCComment>\n			{ 
  					  yyLineNr++;
  					  outputChar('\n');
  					}
<SkipCComment>.				{
  					  outputChar(*yytext);
  					}
<RemoveCComment>"*/"			{ BEGIN(lastCContext); }
<RemoveCComment>"//"			
<RemoveCComment>"/*"
<RemoveCComment>[^*\n]+
<RemoveCComment>\n			{ yyLineNr++; outputChar('\n'); }
<RemoveCComment>.			
<SkipCPPComment,RemoveCPPComment>\n	{
  					  unput(*yytext);
  					  BEGIN(lastCPPContext);
  					}
<SkipCPPComment>"/*"			{
  					  outputChar('/');outputChar('*');
  					}
<SkipCPPComment>"//"			{
  					  outputChar('/');outputChar('/');
  					}
<SkipCPPComment>[^\n]+			{
  					  outputArray(yytext,yyleng);
  					}
<SkipCPPComment>.			{
  					  outputChar(*yytext);
  					}
<RemoveCPPComment>"/*"
<RemoveCPPComment>"//"
<RemoveCPPComment>[^\n]+
<RemoveCPPComment>.
<DefineText>"#"				{
  					  quoteArg=TRUE;
  					}
<DefineText>{ID}			{
  					  if (quoteArg)
					  {
					    defText+="\"";
					  }
					  if (defArgs>0)
					  {
					    int *n;
					    if ((n=(*argDict)[yytext]))
					    {
					      if (!quoteArg) defText+=' ';
					      defText+='@';
					      QCString numStr;
					      numStr.setNum(*n);
					      defText+=numStr;
					      if (!quoteArg) defText+=' ';
					    }
					    else
					    {
					      defText+=yytext;
					    }
					  }
					  else
					  {
					    defText+=yytext;
					  }
					  if (quoteArg)
					  {
					    defText+="\"";
					  }
					  quoteArg=FALSE;
  					}
<DefineText>\\\n			{ 
  					  outputChar('\n');
  					  defText += ' '; yyLineNr++; 
					}
<DefineText>\n				{
  					  outputChar('\n');
  					  Define *def=0;
					  //printf("Define name=`%s' text=`%s'\n",defName.data(),defText.data());
					  if (includeStack.isEmpty()) 
					  {
					    addDefine();
					  }
					  if (/*!Config::onlyPredefinedFlag &&*/ (def=fileDefineDict->find(defName))==0)
					  {
					    fileDefineDict->insert(defName,newDefine());
					  }
					  else if (def)// name already exists
					  {
					    //printf("define found\n");
					    if (def->undef) // undefined name
					    {
					      def->undef = FALSE;
					      def->name = defName;
					      def->definition = defText.stripWhiteSpace();
					      def->nargs = defArgs;
					      def->fileName = yyFileName; 
					      def->lineNr = yyLineNr;
					    }
					    else
					    {
					      //printf("Error: define %s is defined more than once!\n",defName.data());
					    }
					  }
					  delete argDict;
					  yyLineNr++;
					  lastGuardName.resize(0);
					  BEGIN(Start);
  					}
<DefineText>{B}*			{ defText += ' '; }
<DefineText>{B}*"##"{B}*		{ defText += "##"; }
<DefineText>"@"				{ defText += "@@"; }
<DefineText>\"				{ defText += *yytext; 
  					  BEGIN(SkipDoubleQuote);
  					}
<DefineText>\'				{ defText += *yytext;
  					  BEGIN(SkipSingleQuote);
					}
<SkipDoubleQuote>"//"			{ defText += yytext; }
<SkipDoubleQuote>"/*"			{ defText += yytext; }
<SkipDoubleQuote>\"			{
  					  defText += *yytext;
					  BEGIN(DefineText);
  					}
<SkipSingleQuote,SkipDoubleQuote>\\.	{
  					  defText += yytext;
					}
<SkipSingleQuote>\'			{
  					  defText += *yytext;
					  BEGIN(DefineText);
  					}
<SkipDoubleQuote>.			{ defText += *yytext; }
<SkipSingleQuote>.			{ defText += *yytext; }
<DefineText>.				{ defText += *yytext; }
<<EOF>>					{
                                          //printf("End of include file\n");
					  //printf("Include stack depth=%d\n",includeStack.count());
  					  if (includeStack.isEmpty())
					  {
					    //printf("Terminating scanner!\n");
					    yyterminate();
					  }
					  else
					  {
					    FileState *fs=includeStack.pop();
					    //fileDefineCache->merge(yyFileName,fs->fileName);
					    if (Config::inputFilter.isEmpty())
					      fclose(fs->filePtr);
					    else
					      pclose(fs->filePtr);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->bufState );
					    yy_delete_buffer( oldBuf );
					    yyLineNr=fs->lineNr;
					    yyFileName=fs->fileName.copy();
					    //printf("######## FileName %s\n",yyFileName.data());
					    delete fs;
					  }
  					}
<*>"/*"					{
					  outputChar('/');outputChar('*');
  					  lastCContext=YY_START;
					  BEGIN(SkipCComment);
  					}
<*>"//"					{
					  outputChar('/');outputChar('/');
  					  lastCPPContext=YY_START;
					  BEGIN(SkipCPPComment);
					}
<*>\n					{ 
  					  outputChar('\n');
  					  yyLineNr++; 
					}
<*>.				        {
  					  outputChar(*yytext);
  					}

%%

/*@ ----------------------------------------------------------------------------
 */

static int getNextChar(const QCString &expr,QCString *rest,uint &pos)
{
  //printf("getNextChar(%s,%s,%d)\n",expr.data(),rest ? rest->data() : 0,pos);
  if (pos<expr.length())
  {
    //printf("%c=expr()\n",expr.at(pos));
    return expr.at(pos++);
  }
  else if (rest && rest->length()>0)
  {
    int cc=rest->at(0);
    *rest=rest->right(rest->length()-1);
    //printf("%c=rest\n",cc);
    return cc;
  }
  else
  {
    int cc=yyinput();
    //printf("%c=yyinput()\n",cc);
    return cc;
  }
}
 
static int getCurrentChar(const QCString &expr,QCString *rest,uint pos)
{
  //printf("getCurrentChar(%s,%s,%d)\n",expr.data(),rest ? rest->data() : 0,pos);
  if (pos<expr.length())
  {
    //printf("%c=expr()\n",expr.at(pos));
    return expr.at(pos);
  }
  else if (rest && rest->length()>0)
  {
    int cc=rest->at(0);
    //printf("%c=rest\n",cc);
    return cc;
  }
  else
  {
    int cc=yyinput();unput(cc);
    //printf("%c=yyinput()\n",cc);
    return cc;
  }
}

static void unputChar(const QCString &expr,QCString *rest,uint &pos,char c)
{
  //printf("unputChar(%s,%s,%d,%c)\n",expr.data(),rest ? rest->data() : 0,pos,c);
  if (pos<expr.length())
  {
    pos++;
  }
  else if (rest)
  {
    //printf("Prepending to rest!\n");
    char cs[2];cs[0]=c;cs[1]='\0';
    rest->prepend(cs);
  }
  else
  {
    unput(c);
  }
  //printf("result: unputChar(%s,%s,%d,%c)\n",expr.data(),rest ? rest->data() : 0,pos,c);
}

void addSearchDir(const char *dir)
{
  QFileInfo fi(dir);
  if (fi.isDir()) pathList->append(fi.absFilePath());
} 

void initPreprocessor()
{
  pathList = new QStrList;
  addSearchDir(".");
  //defineNameList.setAutoDelete(TRUE);
  //defineNameList.clear();
  defineDict.clear();
  //fileDefineCache = new DefineCache(1009);
  expandedDict = new DefineDict(17);
  fileDefineDict = new DefineDict(1009);
}

void cleanupPreprocessor()
{
  //delete fileDefineCache;
  delete fileDefineDict;
  delete expandedDict;
  delete pathList;
}


void preprocessFile(const char *fileName,BufStr &output)
{
#if DUMP_OUTPUT  
  uint orgOffset=output.curPos();
#endif

  outputBuf=&output;
  includeStack.setAutoDelete(TRUE);
  includeStack.clear();
  fileDefineDict->setAutoDelete(TRUE);
  fileDefineDict->clear();
  expandedDict->setAutoDelete(FALSE);
  expandedDict->clear();
  
  // add predefined macros
  char *defStr = Config::predefined.first();
  while (defStr)
  {
    QCString ds = defStr;
    int i_equals=ds.find('=');
    int i_obrace=ds.find('(');
    int i_cbrace=ds.find(')');
    bool nonRecursive = i_equals>0 && ds.at(i_equals-1)==':';
    
    if (i_obrace==0) continue; // no define name
    
    if (i_obrace<i_equals && i_cbrace<i_equals && 
	i_obrace!=-1      && i_cbrace!=-1      && 
	i_obrace<i_cbrace
       ) // predefined function macro definition
    {
      QRegExp reId("[a-z_A-Z][a-z_A-Z0-9]*"); // regexp matching an id
      QDict<int> argDict(17);
      argDict.setAutoDelete(TRUE);
      int i=i_obrace+1,p,l,count=0;
      // gather the formal arguments in a dictionary 
      while (i<i_cbrace && (p=reId.match(ds,i,&l)))
      {
        argDict.insert(ds.mid(p,l),new int(count++));
	i=p+l;
      }
      // strip definition part
      QCString tmp=ds.right(ds.length()-i_equals-1);
      QCString definition;
      i=0;
      // substitute all occurrences of formal arguments by their 
      // corresponding markers
      while ((p=reId.match(tmp,i,&l))!=-1)
      {
        if (p>i) definition+=tmp.mid(i,p-i);
	int *argIndex;
	if ((argIndex=argDict[tmp.mid(p,l)])!=0)
	{
	  QCString marker;
	  marker.sprintf(" @%d ",*argIndex);
	  definition+=marker;
	}
	else
	{
	  definition+=tmp.mid(p,l);
	}
	i=p+l;
      }
      if (i<(int)tmp.length()) definition+=tmp.mid(i,tmp.length()-i);
      
      // add define definition to the dictionary of defines for this file
      Define *def = new Define;
      def->name = ds.left(i_obrace);
      def->definition = definition; 
      def->nargs = count;
      def->isPredefined = TRUE;
      def->nonRecursive = nonRecursive;
      fileDefineDict->insert(def->name,def);

      //printf("#define `%s' `%s' #nargs=%d\n",
      //  def->name.data(),def->definition.data(),def->nargs);
    }
    else if ((i_obrace==-1 || i_obrace>i_equals) &&
	     (i_cbrace==-1 || i_cbrace>i_equals) &&
	     ds.length()>0 && (int)ds.length()>i_equals
	    ) // predefined non-function macro definition
    {
      Define *def = new Define;
      if (i_equals==-1) // simple define without argument
      {
        def->name = ds;
        def->definition = "1"; // substitute occurrences by 1 (true)
      }
      else // simple define with argument
      {
	int ine=i_equals - (nonRecursive ? 1 : 0);
	def->name = ds.left(ine);
	def->definition = ds.right(ds.length()-i_equals-1);
      }
      def->nargs = -1;
      def->isPredefined = TRUE;
      def->nonRecursive = nonRecursive;
      fileDefineDict->insert(def->name,def);

      //printf("#define `%s' `%s' #nargs=%d\n",
      //  def->name.data(),def->definition.data(),def->nargs);
    }
    
    defStr=Config::predefined.next();
  }
 
  if (Config::inputFilter.isEmpty())
  {
    preYYin = fopen(fileName,"r");
    if (!preYYin)
    {
      err("Error: could not open file %s\n",fileName);
      return;
    }
  }
  else
  {
    QCString cmd = Config::inputFilter+" "+fileName;
    preYYin = popen(cmd,"r");
    if (!preYYin)
    {
      err("Error: could not execute filter %s\n",cmd.data());
      return;
    }
  }
  yyLineNr = 1;
  level    = 0;
  ifcount  = 0;
  yyFileName = fileName;
  BEGIN( Start );
  
  lastGuardName.resize(0);
  guardExpr.resize(0);
  
  preYYlex();
  if (Config::inputFilter.isEmpty())
    fclose(preYYin);
  else
    pclose(preYYin);
  
#if DUMP_OUTPUT  
  char *orgPos=output.data()+orgOffset;
  char *newPos=output.data()+output.curPos();
  printf("Resulting size: %d bytes\n",newPos-orgPos);
  int line=1;
  printf("---------\n00001 ");
  while (orgPos<newPos) 
  {
    putchar(*orgPos);
    if (*orgPos=='\n') printf("%05d ",++line);
    orgPos++;
  }
  printf("\n---------\n");
#endif
}

extern "C" { // some bogus code to keep the compiler happy
//  int  preYYwrap() { return 1 ; }
  void preYYdummy() { yy_flex_realloc(0,0); } 
}
