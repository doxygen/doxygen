/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

%{

/*
 *	includes
 */

#include <stdio.h>
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
#include <qdir.h>
  
#include "pre.h"
#include "constexp.h"
#include "define.h"
#include "doxygen.h"
#include "message.h"
#include "util.h"
#include "defargs.h"
#include "debug.h"
#include "bufstr.h"

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

#define YY_NEVER_INTERACTIVE 1
  

struct FileState
{
  int lineNr;
  FILE *filePtr;
  YY_BUFFER_STATE bufState;
  QCString fileName;
};  

/* -----------------------------------------------------------------
 *
 *	scanner's state
 */

static int                g_yyLineNr   = 1;
static QCString           g_yyFileName;
static FileDef           *g_yyFileDef;
static int                g_ifcount    = 0;
static QStrList          *g_pathList = 0;  
static QStack<FileState>  g_includeStack;
static QDict<int>        *g_argDict;
static int                g_defArgs = -1;
static QCString           g_defName;
static QCString           g_defText;
static QCString           g_defLitText;
static QCString           g_defArgsStr;
static bool               g_defVarArgs;
static int                g_level;
static int                g_lastCContext;
static int                g_lastCPPContext;
static QArray<int>        g_levelGuard;
static BufStr            *g_outputBuf;
static int                g_roundCount;
static bool               g_quoteArg;
static DefineDict        *g_fileDefineDict;
static DefineDict        *g_expandedDict;
static int                g_findDefArgContext;
static QCString           g_lastGuardName;
static QCString           g_incName;
static QCString           g_guardExpr;
static int                g_curlyCount;
static bool               g_nospaces; // add extra spaces during macro expansion

static bool               g_macroExpansion; // from the configuration
static bool               g_expandOnlyPredef; // from the configuration
static int                g_commentCount;
static bool               g_insideComment;


static void setFileName(const char *name)
{
  bool ambig;
  g_yyFileName=name;
  g_yyFileDef=findFileDef(Doxygen::inputNameDict,g_yyFileName,ambig);
  if (g_yyFileDef && g_yyFileDef->isReference()) g_yyFileDef=0;
}

static void incrLevel()
{
  g_level++;
  g_levelGuard.resize(g_level);
  g_levelGuard[g_level-1]=FALSE;
  //printf("%s line %d: incrLevel %d\n",g_yyFileName.data(),g_yyLineNr,g_level);
}

static void decrLevel()
{
  //printf("%s line %d: decrLevel %d\n",g_yyFileName.data(),g_yyLineNr,g_level);
  if (g_level > 0)
  {
    g_level--;
    g_levelGuard.resize(g_level);
  }
  else
  {
    err("%s:%d: Error: More #endif's than #if's found.\n",
	g_yyFileName.data(),g_yyLineNr);
  }
}

static bool otherCaseDone()
{
  if (g_level==0)
  {
    err("%s:%d: Error: Found an #else without a preceding #if.\n",
	g_yyFileName.data(),g_yyLineNr);
    return TRUE;
  }
  else
  {
    return g_levelGuard[g_level-1];
  }
}

static void setCaseDone(bool value)
{
  g_levelGuard[g_level-1]=value;
}

static Define *isDefined(const char *name)
{
  if (name)
  {
    Define *def;
    //if ((def=fileDefineCache->findDefine(g_yyFileName,name)) && !def->undef) 
    //	return def;
    if ((def=g_fileDefineDict->find(name)) && !def->undef) return def; 
  }
  return 0;
}

static FILE *checkAndOpenFile(const QCString &absName)
{
  FILE *f = 0;
  //printf("checkAndOpenFile(%s)\n",absName.data());
  QFileInfo fi(absName);
  if (fi.exists() && fi.isFile())
  {
    // check include stack for absFileName

    QStack<FileState> tmpStack;
    g_includeStack.setAutoDelete(FALSE);
    FileState *fs;
    bool alreadyIncluded=FALSE;
    while ((fs=g_includeStack.pop()))
    {
      if (fs->fileName==absName) alreadyIncluded=TRUE;
      tmpStack.push(fs);
    }
    while ((fs=tmpStack.pop()))
    {
      g_includeStack.push(fs);
    }
    g_includeStack.setAutoDelete(TRUE);

    if (alreadyIncluded) return 0; 

    
    QCString filterName = getFileFilter(absName);
    if (!filterName.isEmpty())
    {
      QCString cmd = filterName+" "+absName;
      f=popen(cmd,"r");
      if (!f) err("Error: could not execute filter %s\n",cmd.data());
    }
    else
    {
      f=fopen(absName,"r");
      if (!f) err("Error: could not open file %s for reading\n",absName.data());
    }
  }
  return f;
}

static FILE *findFile(const char *fileName,bool localInclude)
{
  if (localInclude && g_yyFileDef)
  {
    QCString absName = g_yyFileDef->getPath()+"/"+fileName;
    FILE *f = checkAndOpenFile(absName);
    if (f)
    {
      setFileName(absName);
      g_yyLineNr=1;
      return f;
    }
  }
  if (g_pathList==0) 
  {
    return 0;
  }
  char *s=g_pathList->first();
  while (s)
  {
    QCString absName = (QCString)s+"/"+fileName;
    FILE *f = checkAndOpenFile(absName);
    if (f)
    {
      setFileName(absName);
      g_yyLineNr=1;
      return f;
    }

    s=g_pathList->next();
  } 
  return 0;
}

static QCString extractTrailingComment(const char *s)
{
  if (s==0) return "";
  int i=strlen(s)-1;
  while (i>=0)
  {
    char c=s[i];
    switch (c)
    {
      case '/':
	{
	  i--;
	  if (i>=0 && s[i]=='*') // end of a comment block
	  {
	    i--;
	    while (i>0 && !(s[i-1]=='/' && s[i]=='*')) i--;
	    if (i==0) return s; else return &s[i-1];
	  }
	  else
	  {
	    return "";
	  }
	} 
	break;
	// whitespace or line-continuation
      case ' ':
      case '\t': 
      case '\r':
      case '\n':
      case '\\':
	break;
      default:
	return "";
    }
    i--;
  }
  return "";
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
  //printf("processConcatOperators: in=`%s'\n",expr.data());
  QRegExp r("[ \\t\\n]*##[ \\t\\n]*"); 
  int l,n,i=0;
  if (expr.isEmpty()) return;
  while ((n=r.match(expr,i,&l))!=-1)
  {
    //printf("Match: `%s'\n",expr.data()+i);
    if (n+l+1<(int)expr.length() && expr.at(n+l)=='@' && expr.at(n+l+1)=='-')
    {
      // remove no-rescan marker after ID
      l+=2;
    }
    //printf("found `%s'\n",expr.mid(n,l).data());
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
    i=n;
  }
  //printf("processConcatOperators: out=`%s'\n",expr.data());
}

static void yyunput (int c,char *buf_ptr  );
static void returnCharToStream(char c)
{
  unput(c);
}

static inline void addTillEndOfString(const QCString &expr,QCString *rest,
                                       uint &pos,char term,QCString &arg)
{
  char cc;
  while ((cc=getNextChar(expr,rest,pos))!=EOF)
  {
    if (cc=='\\') arg+=cc,cc=getNextChar(expr,rest,pos);
    else if (cc==term) return;
    arg+=cc;
  }
}

/*! replaces the function macro \a def whose argument list starts at
 * \a pos in expression \a expr. 
 * Notice that this routine may scan beyond the \a expr string if needed.
 * In that case the characters will be read from the input file.
 * The replacement string will be returned in \a result and the 
 * length of the (unexpanded) argument list is stored in \a len.
 */ 
static bool replaceFunctionMacro(const QCString &expr,QCString *rest,int pos,int &len,const Define *def,QCString &result)
{
  //printf("replaceFunctionMacro(expr=%s,rest=%s,pos=%d,def=%s) level=%d\n",expr.data(),rest ? rest->data() : 0,pos,def->name.data(),g_level);
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

  QDict<QCString> argTable;  // list of arguments
  argTable.setAutoDelete(TRUE);
  QCString arg;
  int argCount=0;
  bool done=FALSE;
  
  // PHASE 1: read the macro arguments
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
	//char term='\0';
	while ((cc=getNextChar(expr,rest,j))!=EOF)
	{
	  char c=(char)cc;
	  //printf("processing %c: term=%c (%d)\n",c,term,term);
	  if (c=='\'' || c=='\"') // skip ('s and )'s inside strings
	  {
	    arg+=c;
	    addTillEndOfString(expr,rest,j,c,arg);
	  }
	  if (c==')')
	  {
	    level--;
	    arg+=c;
	    if (level==0) break;
	  }
	  else if (c=='(')
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
	bool found=FALSE;
	while (!found && (cc=getNextChar(expr,rest,j))!=EOF)
	{
	  found = cc=='"';
	  if (cc=='\\')
	  {
	    c=(char)cc;	  
	    arg+=c;
	    if ((cc=getNextChar(expr,rest,j))==EOF) break;
	  }
	  c=(char)cc;	  
	  arg+=c;
	}
      }
      else if (c=='\'') // append literal characters
      {
	arg+=c;
	bool found=FALSE;
	while (!found && (cc=getNextChar(expr,rest,j))!=EOF)
	{
	  found = cc=='\'';
	  if (cc=='\\')
	  {
	    c=(char)cc;	  
	    arg+=c;
	    if ((cc=getNextChar(expr,rest,j))==EOF) break;
	  }
	  c=(char)cc;
	  arg+=c;
	}
      }	    
      else // append other characters
      {
	arg+=c;
      }
    }
  }

  // PHASE 2: apply the macro function
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
	  //printf("request key %s result %s\n",key.data(),argTable[key]->data());
	  if (key.length()>1 && (subst=argTable[key])) 
	  {
	    QCString substArg=*subst;
	    //printf("substArg=`%s'\n",substArg.data());
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
	      if (hash && substArg.isEmpty())
	      {
		resExpr+="@E"; // empty argument will be remove later on
	      }
	      else if (g_nospaces)
	      {
	        resExpr+=substArg;
	      }
	      else
	      {
	        resExpr+=" "+substArg+" ";
	      }
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
	else if (inString && d.at(k)=='\"' && (d.at(k-1)!='\\' || d.at(k-2)=='\\'))
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
      char ppc=0,pc=c;
      if (p<(int)expr.length()) c=expr.at(p);
      while (p<(int)expr.length() && (c!='"' || (pc=='\\' && ppc!='\\'))) 
	// continue as long as no " is found, but ignoring \", but not \\"
      {
	ppc=pc;
	pc=c;
	c=expr.at(p);
	p++;
      }
    }
    else if (c=='/') // skip C Comment
    {
      char pc=c;
      if (p<(int)expr.length()) 
      {
	c=expr.at(++p);
        if (c=='*')  // Start of C comment
        { 
  	  while (p<(int)expr.length() && !(pc=='*' && c=='/'))
	  {
	    pc=c;
	    c=expr.at(++p);
	  }
	  p++;
        }
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
      if (g_expandedDict->find(macroName)==0) // expand macro
      {
	Define *def=isDefined(macroName);
	if (def && def->nargs==-1) // simple macro
	{
	  // substitute the definition of the macro
	  //printf("macro `%s'->`%s'\n",macroName.data(),def->definition.data());
	  if (g_nospaces)
	  {
	    expMacro=def->definition.stripWhiteSpace();
	  }
	  else
	  {
	    expMacro=" "+def->definition.stripWhiteSpace()+" ";
	  }
	  //expMacro=def->definition.stripWhiteSpace();
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
	    g_expandedDict->insert(macroName,def);
	    expandExpression(resultExpr,&restExpr,0);
	    g_expandedDict->remove(macroName);
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

/*! replaces all occurrences of @@@@ in \a s by @@
 *  and removes all occurrences of @@E.
 *  All identifiers found are replaced by 0L
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
      if (c=='@') // replace @@ with @ and remove @E
      {
	if (*(p+1)=='@')
	{
	  result+=c; 
	}
	else if (*(p+1)=='E')
	{
	  // skip
	}
	p+=2;
      }
      else if (isdigit(c)) // number
      {
	result+=c;
	p++;
        inNum=TRUE;	
      }
      else if (c=='d' && !inNum) // identifier starting with a `d'
      {
	if (strncmp(p,"defined ",8)==0 || strncmp(p,"defined(",8)==0) 
	           // defined keyword
	{
	  p+=7; // skip defined
	}
	else
	{
	  result+="0L";
	  p++;
	  while ((c=*p) && isId(c)) p++;
	}
      }
      else if ((isalpha(c) || c=='_') && !inNum) // replace identifier with 0L
      {
	result+="0L";
	p++;
	while ((c=*p) && isId(c)) p++;
      }
      else if (c=='/') // skip C comments
      {
	char pc=c;
	c=*++p;
	if (c=='*') // start of C comment
	{ 
	  while (*p && !(pc=='*' && c=='/')) // search end of comment
	  {
	    pc=c;
	    c=*++p;
	  }
	  p++;
	}
      }
      else 
      {
	result+=c;
	char lc=tolower(c);
	if (!isId(lc) && lc!='.' && lc!='-' && lc!='+') inNum=FALSE;
	p++;
      }
    }
  }
  //printf("removeIdsAndMarkers(%s)=%s\n",s,result.data());
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
      else if (c=='/') // skip C comments
      {
	result+=c;
	char pc=c;
	c=*++p;
	if (c=='*') // start of C comment
	{ 
	  while (*p && !(pc=='*' && c=='/')) // search end of comment
	  {
	    result+=c;
	    pc=c;
	    c=*++p;
	  }
	  result+=c;
	  p++;
	}
      }
      else
      {
	result+=c;
	p++;
      }
    }
  }
  //printf("RemoveMarkers(%s)=%s\n",s,result.data());
  return result;
}

/*! compute the value of the expression in string \a expr.
 *  If needed the function may read additional characters from the input.
 */

bool computeExpression(const QCString &expr)
{
  QCString e=expr;
  expandExpression(e,0,0);
  //printf("after expansion `%s'\n",e.data());
  e = removeIdsAndMarkers(e);
  if (e.isEmpty()) return FALSE;
  //printf("parsing `%s'\n",e.data());
  return parseCppExpression(g_yyFileName,g_yyLineNr,e);
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
  def->name = g_defName;
  def->definition = g_defText.stripWhiteSpace();
  def->nargs = g_defArgs;
  def->fileName = g_yyFileName; 
  def->lineNr = g_yyLineNr;
  def->varArgs = g_defVarArgs;
  //printf("newDefine: `%s'->`%s'\n",def->name.data(),def->definition.data());
  if (!def->name.isEmpty() && Doxygen::expandAsDefinedDict[def->name])
  {
    def->isPredefined=TRUE;
  }
  return def;
}

void addDefine()
{
  //printf("addDefine %s %s\n",g_defName.data(),g_defArgsStr.data());
  //ArgumentList *al = new ArgumentList;
  //stringToArgumentList(g_defArgsStr,al);
  MemberDef *md=new MemberDef(
      g_yyFileName,g_yyLineNr,
      "#define",g_defName,g_defArgsStr,0,
      Public,Normal,FALSE,FALSE,MemberDef::Define,0,0);
  if (!g_defArgsStr.isEmpty())
  {
    ArgumentList *argList = new ArgumentList;
    //printf("addDefine() g_defName=`%s' g_defArgsStr=`%s'\n",g_defName.data(),g_defArgsStr.data());
    stringToArgumentList(g_defArgsStr,argList);
    md->setArgumentList(argList);
  }
  //printf("Setting initializer for `%s' to `%s'\n",g_defName.data(),g_defText.data());
  int l=g_defLitText.find('\n');
  if (l>0 && g_defLitText.left(l).stripWhiteSpace()=="\\")
  {
    // strip first line if it only contains a slash 
    g_defLitText = g_defLitText.right(g_defLitText.length()-l-1);
  }
  else if (l>0) 
  {
    // align the items on the first line with the items on the second line
    int k=l+1;
    const char *p=g_defLitText.data()+k;
    char c;
    while ((c=*p++) && (c==' ' || c=='\t')) k++;
    g_defLitText=g_defLitText.mid(l+1,k-l-1)+g_defLitText.stripWhiteSpace();
  }
  md->setInitializer(g_defLitText.stripWhiteSpace());
  
  //md->setDefFile(g_yyFileName);
  //md->setDefLine(g_yyLineNr);
  md->setFileDef(g_yyFileDef);
  md->setDefinition("#define "+g_defName);

  MemberName *mn=Doxygen::functionNameSDict[g_defName];
  if (mn==0)
  {
    mn = new MemberName(g_defName);
    //Doxygen::functionNameList.append(mn);
    //Doxygen::functionNameDict.insert(g_defName,mn);
    Doxygen::functionNameSDict.append(g_defName,mn);
  }
  mn->append(md);
  if (g_yyFileDef) g_yyFileDef->insertMember(md);
  
  //Define *d;
  //if ((d=defineDict[g_defName])==0) defineDict.insert(g_defName,newDefine()); 
}

static inline void outputChar(char c)
{
  if (g_includeStack.isEmpty() || g_curlyCount>0) g_outputBuf->addChar(c);
} 

static inline void outputArray(const char *a,int len)
{
  if (g_includeStack.isEmpty() || g_curlyCount>0) g_outputBuf->addArray(a,len);
}

static void readIncludeFile(const QCString &inc)
{
  if (!Config_getBool("SEARCH_INCLUDES")) return; // do not read include files
  uint i=0;
  
  // find the start of the include file name
  while (i<inc.length() && 
         (inc.at(i)==' ' || inc.at(i)=='"' || inc.at(i)=='<')
	) i++;
  uint s=i;
  
  // was it a local include?
  bool localInclude = s>0 && inc.at(s-1)=='"';
  
  // find the end of the include file name
  while (i<inc.length() && inc.at(i)!='"' && inc.at(i)!='>') i++; 

  if (s<inc.length() && i>s) // valid include file name found
  {
    // extract include path+name
    QCString incFileName=inc.mid(s,i-s).stripWhiteSpace(); 

    FILE *f;
    QCString oldFileName = g_yyFileName.copy();
    FileDef *oldFileDef  = g_yyFileDef;
    int oldLineNr        = g_yyLineNr;
    //printf("Searching for `%s'\n",incFileName.data());
    if ((f=findFile(incFileName,localInclude))) // see if the include file can be found
    {
      //printf("Found include file!\n");
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        for (i=0;i<g_includeStack.count();i++) msg("  ");
        msg("#include %s: parsing...\n",incFileName.data());
      }
      if (oldFileDef) 
      {
	// add include dependency to the file in which the #include was found
	oldFileDef->addIncludeDependency(g_yyFileDef,incFileName,localInclude);
	// add included by dependency
        if (g_yyFileDef)
        {
	  //printf("Adding include dependency %s->%s\n",oldFileDef->name().data(),incFileName.data());
  	  g_yyFileDef->addIncludedByDependency(oldFileDef,oldFileDef->name(),localInclude);
        }
      }
      FileState *fs=new FileState;
      fs->bufState=YY_CURRENT_BUFFER;
      fs->lineNr=oldLineNr;
      fs->fileName=oldFileName;
      fs->filePtr=f;
      // push the state on the stack
      g_includeStack.push(fs);
      // set the scanner to the include file

      // Deal with file changes due to 
      // #include's within { .. } blocks
      QCString lineStr;
      lineStr.sprintf("# 1 \"%s\" 1\n",g_yyFileName.data());
      outputArray(lineStr.data(),lineStr.length());
      
      //fprintf(stderr,"Switching to include file %s\n",incFileName.data());
      preYYin=f;
      yy_switch_to_buffer(yy_create_buffer(preYYin, YY_BUF_SIZE));
    } 
    else
    {
      if (oldFileDef) 
      {
	bool ambig;
	FileDef *fd = findFileDef(Doxygen::inputNameDict,incFileName,ambig);
	//printf("findFileDef(%s)=%p\n",incFileName.data(),fd);
	// add include dependency to the file in which the #include was found
	oldFileDef->addIncludeDependency(fd,incFileName,localInclude);
	// add included by dependency
        if (fd)
        {
          //printf("Adding include dependency (2) %s->%s ambig=%d\n",oldFileDef->name().data(),fd->name().data(),ambig);
  	  fd->addIncludedByDependency(oldFileDef,oldFileDef->name(),localInclude);
        }
      }
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        msg("#include %s: not found or already included! skipping...\n",incFileName.data());
        //printf("Error: include file %s not found\n",yytext);
      }
    }
  }
}

/* ----------------------------------------------------------------- */

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int len = fread( buf, 1, max_size, yyin );
  if (len==0 && ferror( yyin ))
  {
    yy_fatal_error( "input in flex scanner failed" );
    return len;
  }
  return filterCRLF(buf,len);
}

/* ----------------------------------------------------------------- */

%}

ID	[a-z_A-Z][a-z_A-Z0-9]*
B       [ \t]
BN	[ \t\r\n]
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))

%option noyywrap

%x      Start
%x	Command
%x	SkipCommand
%x	SkipLine
%x	CopyLine
%x	CopyString
%x      Include
%x      IncludeID
%x	DefName
%x	DefineArg
%x	DefineText
%x      SkipCPPBlock
%x      Ifdef
%x      Ifndef
%x	SkipCComment
%x	CopyCComment
%x	SkipVerbatim
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
<Start>^{B}*"#"				{ BEGIN(Command); }
<Start>^{B}*/[^#]			{
 					  outputArray(yytext,yyleng); 
  					  BEGIN(CopyLine); 
					}
<Start>^{B}*[_A-Z][_A-Z0-9]*"("[^\)\n]*")"/{BN}{1,10}*[:{] { // constructors?
					  int i;
					  for (i=yyleng-1;i>=0;i--)
					  {
					    unput(yytext[i]);
					  }
					  BEGIN(CopyLine);
                                        }
<Start>^{B}*[_A-Z][_A-Z0-9]*{B}*"("[^\)\n]*")"{B}*\n { // function like macro
  					  static bool skipFuncMacros = Config_getBool("SKIP_FUNCTION_MACROS");
					  QCString name(yytext);
					  name=name.left(name.find('(')).stripWhiteSpace();

					  Define *def=0;
					  if (skipFuncMacros && 
					      name!="Q_PROPERTY" &&
					      !(
					         (g_includeStack.isEmpty() || g_curlyCount>0) &&
					         g_macroExpansion &&
					         (def=g_fileDefineDict->find(name)) &&
					         (!g_expandOnlyPredef || def->isPredefined)
					       )
					     )
					  {
					    outputChar('\n');
					    g_yyLineNr++;
					  }
					  else // don't skip
					  {
					    int i;
					    for (i=yyleng-1;i>=0;i--)
					    {
					      unput(yytext[i]);
					    }
					    BEGIN(CopyLine);
					  }
  					}
<CopyLine>"extern"{BN}{0,80}"\"C\""*{BN}{0,80}"{"	{
                                          QCString text=yytext;
  					  g_yyLineNr+=text.contains('\n');
					  outputArray(yytext,yyleng);
  					}
<CopyLine>"{"				{ // count brackets inside the main file
  					  if (g_includeStack.isEmpty()) 
					  {
					    g_curlyCount++;
					  }
					  outputChar(*yytext);
  					}
<CopyLine>"}"				{ // count brackets inside the main file
  					  if (g_includeStack.isEmpty() && g_curlyCount>0) 
					  {
					    g_curlyCount--;
					  }
					  outputChar(*yytext);
  					}
<CopyLine>"'"\\[0-7]{1,3}"'"		{ 
  					  outputArray(yytext,yyleng);
					}
<CopyLine>"'"\\."'"			{ 
  					  outputArray(yytext,yyleng);
					}
<CopyLine>"'"."'"			{ 
  					  outputArray(yytext,yyleng);
					}
<CopyLine>\"				{
					  outputChar(*yytext);
					  BEGIN( CopyString );
					}
<CopyString>[^\"\\\r\n]+		{
  					  outputArray(yytext,yyleng);
					}
<CopyString>\\.				{
					  outputArray(yytext,yyleng);
					}
<CopyString>\"				{
					  outputChar(*yytext);
					  BEGIN( CopyLine );
					}
<CopyLine>{ID}/{BN}{0,80}"("			{
  					  Define *def=0;
					  //def=g_fileDefineDict->find(yytext);
					  //printf("Search for define %s found=%d g_includeStack.isEmpty()=%d "
					  //       "g_curlyCount=%d g_macroExpansion=%d g_expandOnlyPredef=%d "
					  //	 "isPreDefined=%d\n",yytext,def ? 1 : 0,
					  //	 g_includeStack.isEmpty(),g_curlyCount,g_macroExpansion,g_expandOnlyPredef,
					  //	 def ? def->isPredefined : -1
					  //	);
					  if ((g_includeStack.isEmpty() || g_curlyCount>0) &&
					      g_macroExpansion &&
					      (def=g_fileDefineDict->find(yytext)) &&
					      (!g_expandOnlyPredef || def->isPredefined)
					     )
					  {
					    //printf("Found it!\n");
					    g_roundCount=0;
					    g_defArgsStr=yytext;
					    if (def->nargs==-1) // no function macro
					    {
					      QCString result = expandMacro(g_defArgsStr);
					      //printf("result=`%s'\n",result.data());
					      outputArray(result,result.length());
					    }
					    else // zero or more arguments
					    {
					      g_findDefArgContext = CopyLine;
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
  					  if ((g_includeStack.isEmpty() || g_curlyCount>0) && 
					      g_macroExpansion &&
					      (def=g_fileDefineDict->find(yytext)) &&
					      def->nargs==-1 &&
					      (!g_expandOnlyPredef || def->isPredefined)
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
					  g_yyLineNr++;
  					}
<FindDefineArgs>"("			{
  					  g_defArgsStr+='(';
  					  g_roundCount++;
  					}
<FindDefineArgs>")"			{
  					  g_defArgsStr+=')';
					  g_roundCount--;
					  if (g_roundCount==0)
					  {
					    QCString result=expandMacro(g_defArgsStr);
					    //printf("g_defArgsStr=`%s'->`%s'\n",g_defArgsStr.data(),result.data());
					    if (g_findDefArgContext==CopyLine)
					    {
					      outputArray(result,result.length());
					      BEGIN(g_findDefArgContext);
					    }
					    else // g_findDefArgContext==IncludeID
					    {
					      readIncludeFile(result);
					      g_nospaces=FALSE;
					      BEGIN(Start);
					    }
					  }
  					}
  /*
<FindDefineArgs>")"{B}*"("		{
  					  g_defArgsStr+=yytext;
  					}
  */
<FindDefineArgs>{CHARLIT}		{
  					  g_defArgsStr+=yytext;
  					}
<FindDefineArgs>\"			{
  					  g_defArgsStr+=*yytext;
  					  BEGIN(ReadString);
  					}
<FindDefineArgs>\n			{
  					  g_yyLineNr++;
					  outputChar('\n');
  					}
<FindDefineArgs>"@"			{
  					  g_defArgsStr+="@@";
  					}
<FindDefineArgs>.			{
  					  g_defArgsStr+=*yytext;
  					}
<ReadString>"\""			{
  					  g_defArgsStr+=*yytext;
					  BEGIN(FindDefineArgs);
  					}
<ReadString>"//"|"/*"			{
  					  g_defArgsStr+=yytext;
  					}
<ReadString>\\.				{
  					  g_defArgsStr+=yytext;
  					}
<ReadString>.				{
  					  g_defArgsStr+=*yytext;
  					}
<Command>"include"{B}+/{ID}		{
  					  if (g_macroExpansion) 
					    BEGIN(IncludeID);
  					}
<Command>"include"{B}*[<"]		{ 
					  char c[2];
					  c[0]=yytext[yyleng-1];c[1]='\0';
					  g_incName=c;
  					  BEGIN(Include); 
					}
<Command>"define"{B}+			{ 
  			                  //printf("!!!DefName\n"); 
  					  BEGIN(DefName); 
					}
<Command>"ifdef"/{B}*"("		{
  					  incrLevel();
					  g_guardExpr.resize(0);
  					  BEGIN(DefinedExpr2);
  					}
<Command>"ifdef"/{B}+			{
  					  //printf("Pre.l: ifdef\n");
  					  incrLevel();
					  g_guardExpr.resize(0);
  					  BEGIN(DefinedExpr1);
  					}
<Command>"ifndef"/{B}*"("		{
  					  incrLevel();
					  g_guardExpr="! ";
  					  BEGIN(DefinedExpr2);
					}
<Command>"ifndef"/{B}+			{
  					  incrLevel();
					  g_guardExpr="! ";
  					  BEGIN(DefinedExpr1);
  					}
<Command>"if"/[ \t(!]			{
  					  incrLevel();
					  g_guardExpr.resize(0);
					  BEGIN(Guard);
					}
<Command>("elif"|"else"{B}*"if")/[ \t(!]	{
  					  if (!otherCaseDone())
					  {
					    g_guardExpr.resize(0);
					    BEGIN(Guard);  
					  }
					  else
					  {
					    g_ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<Command>"else"/[^a-z_A-Z0-9]		{
					  //printf("else g_levelGuard[%d]=%d\n",g_level-1,g_levelGuard[g_level-1]);
  					  if (otherCaseDone())
					  {
					    g_ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
					  else
					  {
					    setCaseDone(TRUE);
					    //g_levelGuard[g_level-1]=TRUE;
					  } 
  					}
<Command>"undef"{B}+			{
  					  BEGIN(UndefName);
  					}
<Command>("elif"|"else"{B}*"if")/[ \t(!]	{
  					  if (!otherCaseDone())
					  {
					    g_guardExpr.resize(0);
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
					  g_yyLineNr++;
  					}
<Command>{ID}				{ // unknown directive
					  BEGIN(IgnoreLine);
					}
<IgnoreLine>\\[\r]?\n			{
  					  outputChar('\n');
					  g_yyLineNr++;
					}
<IgnoreLine>.
<Command>.
<UndefName>{ID}				{
  					  Define *def;
  					  if ((def=isDefined(yytext)) 
					      /*&& !def->isPredefined*/
					     )
					  {
					    //printf("undefining %s\n",yytext);
					    def->undef=TRUE;
					  }
					  BEGIN(Start);
  					}
<Guard>\\[\r]?\n			{
  					  outputChar('\n');
  					  g_guardExpr+=' ';
					  g_yyLineNr++;
  					}
<Guard>"defined"/{B}*"("		{
    					  BEGIN(DefinedExpr2);
    					}
<Guard>"defined"/{B}+			{
    					  BEGIN(DefinedExpr1);
    					}
<Guard>.				{ g_guardExpr+=*yytext; }
<Guard>\n				{
  					  outputChar('\n');
					  g_yyLineNr++;
  					  //printf("Guard: `%s'\n",
					  //    g_guardExpr.data());
					  bool guard=computeExpression(g_guardExpr);
					  setCaseDone(guard);
					  //printf("if g_levelGuard[%d]=%d\n",g_level-1,g_levelGuard[g_level-1]);
					  if (guard)
					  {
					    BEGIN(Start);
					  } 
					  else
					  {
					    g_ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<DefinedExpr1,DefinedExpr2>\\\n		{ g_yyLineNr++; outputChar('\n'); }
<DefinedExpr1>{ID}			{
  					  if (isDefined(yytext))
					    g_guardExpr+=" 1L ";
					  else
					    g_guardExpr+=" 0L ";
					  g_lastGuardName=yytext;
					  BEGIN(Guard);
  					}
<DefinedExpr2>{ID}			{
  					  if (isDefined(yytext))
					    g_guardExpr+=" 1L ";
					  else
					    g_guardExpr+=" 0L ";
					  g_lastGuardName.resize(0);
  					}
<DefinedExpr1,DefinedExpr2>\n		{ // should not happen, handle anyway
  					  g_ifcount=0;
 					  BEGIN(SkipCPPBlock); 
					}
<DefinedExpr2>")"			{
  					  BEGIN(Guard);
  					}
<DefinedExpr1,DefinedExpr2>.
<SkipCPPBlock>^{B}*"#"			{ BEGIN(SkipCommand); }
<SkipCPPBlock>^{B}*/[^#]		{ BEGIN(SkipLine); }
<SkipCPPBlock>.
<SkipCommand>"if"(("n")?("def"))?/[ \t(!]	{ 
  					  incrLevel();
                                          g_ifcount++; 
  					  //printf("#if... depth=%d\n",g_ifcount);
					}
<SkipCommand>"else"			{
					  //printf("Else! g_ifcount=%d otherCaseDone=%d\n",g_ifcount,otherCaseDone());
  					  if (g_ifcount==0 && !otherCaseDone())
					  {
					    setCaseDone(TRUE);
  					    //outputChar('\n');
					    BEGIN(Start);
					  }
  					}
<SkipCommand>("elif"|"else"{B}*"if")/[ \t(!]		{
  					  if (g_ifcount==0) 
					  {
  					    if (!otherCaseDone())
					    {
					      g_guardExpr.resize(0);
					      g_lastGuardName.resize(0);
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
  				          if (--g_ifcount<0)
  					  {
  					    //outputChar('\n');
					    BEGIN(Start);
					  }
					}
<SkipCommand>\n				{ 
  					  outputChar('\n');
  					  g_yyLineNr++; 
					  BEGIN(SkipCPPBlock);
					}
<SkipCommand>{ID}			{ // unknown directive 
  					  BEGIN(SkipLine); 
					}
<SkipCommand>.
<SkipLine>[^/\n]+			
<SkipLine>.
<SkipLine,SkipCommand,SkipCPPBlock>"//"	{
  					  g_lastCPPContext=YY_START;
  					  BEGIN(RemoveCPPComment);
					}
<SkipLine,SkipCommand,SkipCPPBlock>"/*"	{
					  g_lastCContext=YY_START;
  					  BEGIN(RemoveCComment);
  					}
<SkipLine>\n				{
  					  outputChar('\n');
					  g_yyLineNr++;  
					  BEGIN(SkipCPPBlock);
					}
<IncludeID>{ID}{B}*/"("			{
  					  g_nospaces=TRUE;
				          g_roundCount=0;
					  g_defArgsStr=yytext;
					  g_findDefArgContext = IncludeID;
					  BEGIN(FindDefineArgs);
					}
<IncludeID>{ID}				{
  					  g_nospaces=TRUE;
                                          readIncludeFile(expandMacro(yytext));
					  BEGIN(Start);
  					}
<Include>[^\">\n]+[\">]			{ 
					  g_incName+=yytext;
					  readIncludeFile(g_incName);
					  
					  BEGIN(Start);
  					}
<DefName>{ID}/"("			{
  					  //printf("Define() `%s'\n",yytext);
					  g_argDict = new QDict<int>(31);
					  g_argDict->setAutoDelete(TRUE);
					  g_defArgs = 0; 
                                          g_defArgsStr.resize(0);
					  g_defText.resize(0);
					  g_defLitText.resize(0);
					  g_defName = yytext;
					  g_defVarArgs = FALSE;
					  BEGIN(DefineArg);
  					}
<DefName>{ID}/{B}*			{
  					  //printf("Define `%s'\n",yytext);
  					  g_argDict = 0;
					  g_defArgs = -1;
                                          g_defArgsStr.resize(0);
					  g_defText.resize(0);
					  g_defLitText.resize(0);
					  g_defName = yytext;
					  g_defVarArgs = FALSE;
					  QCString tmp=(QCString)"#define "+g_defName+g_defArgsStr;
					  outputArray(tmp.data(),tmp.length());
					  g_quoteArg=FALSE;
					  g_insideComment=FALSE;
					  BEGIN(DefineText); 
  					}
<DefName>{ID}/{B}*"\n"			{
  					  g_argDict = 0;
					  g_defArgs = -1;
					  g_defName = yytext;
                                          g_defArgsStr.resize(0);
					  g_defText.resize(0);
					  g_defLitText.resize(0);
					  g_defVarArgs = FALSE;
					  if ( g_defName!=g_lastGuardName )
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+g_defName;
					    outputArray(tmp.data(),tmp.length());
					    g_quoteArg=FALSE;
					    g_insideComment=FALSE;
					    BEGIN(DefineText);
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
  					    Define *def=g_fileDefineDict->find(g_defName);
					    //if (g_includeStack.isEmpty()) 
					    //{
					    //  addDefine();
					    //}
					    if (def==0) // new define name for this file
					    {
					      g_fileDefineDict->insert(g_defName,newDefine());
					    }
					    else // name already exists
					    {
					      if (def->undef) // undefined name
					      {
					        def->undef = FALSE;
					        def->name = g_defName;
					        def->definition = g_defText.stripWhiteSpace();
					        def->nargs = g_defArgs;
					        def->fileName = g_yyFileName.copy(); 
					        def->lineNr = g_yyLineNr;
					      }
					      else
					      {
					        //printf("Error: define %s is defined more than once!\n",g_defName.data());
					      }
					    }
					    g_lastGuardName.resize(0);
					    BEGIN(Start);
					  }
  					}
<DefineArg>","{B}*			{ g_defArgsStr+=yytext; }
<DefineArg>"("{B}*                      { g_defArgsStr+=yytext; }
<DefineArg>{B}*")"{B}*			{
                                          g_defArgsStr+=yytext; 
					  QCString tmp=(QCString)"#define "+g_defName+g_defArgsStr;
					  outputArray(tmp.data(),tmp.length());
					  g_quoteArg=FALSE;
					  g_insideComment=FALSE;
  					  BEGIN(DefineText);
  					}
<DefineArg>{ID}{B}*("..."?)		{
  					  //printf("Define addArg(%s)\n",yytext);
  					  QCString argName=yytext;
  					  g_defVarArgs = yytext[yyleng-1]=='.';
					  if (g_defVarArgs) // strip ellipsis
					  {
					    argName=argName.left(argName.length()-3);
					  }
					  argName = argName.stripWhiteSpace();
                                          g_defArgsStr+=yytext;
					  g_argDict->insert(argName,new int(g_defArgs)); 
					  g_defArgs++;
  					}
  /*
<DefineText>"/ **"|"/ *!"			{
  					  g_defText+=yytext;
					  g_defLitText+=yytext;
					  g_insideComment=TRUE;
  					}
<DefineText>"* /"			{
  					  g_defText+=yytext;
					  g_defLitText+=yytext;
					  g_insideComment=FALSE;
  					}
  */
<DefineText>"/*"			{
					  g_defText+=yytext;
					  g_defLitText+=yytext;
					  g_lastCContext=YY_START;
					  g_commentCount=1;
  					  BEGIN(CopyCComment);
  					}
<DefineText>"//"			{
  				          outputChar('/');outputChar('/');
  					  g_lastCPPContext=YY_START;
					  g_defLitText+=' ';
  					  BEGIN(SkipCPPComment);
  					}
<SkipCComment>[/]?"*/"			{
  					  if (yytext[0]=='/') outputChar('/');
  					  outputChar('*');outputChar('/');
					  if (--g_commentCount<=0)
					  {
  					    BEGIN(g_lastCContext);  
					  }
  					}
<SkipCComment>"//"			{
  					  outputChar('/');outputChar('/');
  					}
<SkipCComment>"/*"			{
  					  outputChar('/');outputChar('*');
					  //g_commentCount++;
  					}
<SkipCComment>[\\@]"verbatim"{BN}+	{
  					  outputArray(yytext,yyleng);
					  BEGIN(SkipVerbatim);
  					}
<SkipVerbatim>[\\@]"endverbatim"	{
  					  outputArray(yytext,yyleng);
					  BEGIN(SkipCComment);
  					}
<SkipCComment,SkipVerbatim>[^*\x06\n\/]+ {
  					  outputArray(yytext,yyleng);
  					}
<SkipCComment,SkipVerbatim>\n		{ 
  					  g_yyLineNr++;
  					  outputChar('\n');
  					}
<SkipCComment,SkipVerbatim>.		{
  					  outputChar(*yytext);
  					}
<CopyCComment>[^*]+			{
					  g_defLitText+=yytext;
					  g_defText+=yytext;
  					}
<CopyCComment>"*/"			{
					  g_defLitText+=yytext;
					  g_defText+=yytext;
  					  BEGIN(g_lastCContext);
  					}
<CopyCComment>\n			{ 
  					  g_yyLineNr++;
					  g_defLitText+=yytext;
					  g_defText+=yytext;
  					}
<CopyCComment>.				{
					  g_defLitText+=yytext;
					  g_defText+=yytext;
  					}
<RemoveCComment>"*/"		        { BEGIN(g_lastCContext); }
<RemoveCComment>"//"			
<RemoveCComment>"/*"
<RemoveCComment>[^*\x06\n]+
<RemoveCComment>\n			{ g_yyLineNr++; outputChar('\n'); }
<RemoveCComment>.			
<SkipCPPComment,RemoveCPPComment>\n	{
  					  unput(*yytext);
  					  BEGIN(g_lastCPPContext);
  					}
<SkipCPPComment>"/*"			{
  					  outputChar('/');outputChar('*');
  					}
<SkipCPPComment>"//"			{
  					  outputChar('/');outputChar('/');
  					}
<SkipCPPComment>[^\x06\n]+		{
  					  outputArray(yytext,yyleng);
  					}
<SkipCPPComment>.			{
  					  outputChar(*yytext);
  					}
<RemoveCPPComment>"/*"
<RemoveCPPComment>"//"
<RemoveCPPComment>[^\x06\n]+
<RemoveCPPComment>.
<DefineText>"#"				{
  					  g_quoteArg=TRUE;
					  g_defLitText+=yytext;
  					}
<DefineText>{ID}			{
					  g_defLitText+=yytext;
  					  if (g_quoteArg)
					  {
					    g_defText+="\"";
					  }
					  if (g_defArgs>0)
					  {
					    int *n;
					    if ((n=(*g_argDict)[yytext]))
					    {
					      //if (!g_quoteArg) g_defText+=' ';
					      g_defText+='@';
					      QCString numStr;
					      numStr.sprintf("%d",*n);
					      g_defText+=numStr;
					      //if (!g_quoteArg) g_defText+=' ';
					    }
					    else
					    {
					      g_defText+=yytext;
					    }
					  }
					  else
					  {
					    g_defText+=yytext;
					  }
					  if (g_quoteArg)
					  {
					    g_defText+="\"";
					  }
					  g_quoteArg=FALSE;
  					}
<DefineText>\\[\r]?\n			{ 
					  g_defLitText+=yytext;
					  outputChar('\n');
  					  g_defText += ' '; g_yyLineNr++; 
					}
<DefineText>\n				{
					  QCString comment=extractTrailingComment(g_defLitText);
					  g_defLitText+=yytext;
					  if (!comment.isEmpty())
					  {
					    outputArray(comment,comment.length());
					    g_defLitText=g_defLitText.left(g_defLitText.length()-comment.length()-1);
					  }
  					  outputChar('\n');
  					  Define *def=0;
					  //printf("Define name=`%s' text=`%s' litTexti=`%s'\n",g_defName.data(),g_defText.data(),g_defLitText.data());
					  if (g_includeStack.isEmpty() || g_curlyCount>0) 
					  {
					    addDefine();
					  }
					  def=g_fileDefineDict->find(g_defName);
					  if (def==0) // new define
					  {
					    //printf("new define!\n");
					    g_fileDefineDict->insert(g_defName,newDefine());
					  }
					  else if (def)// name already exists
					  {
					    //printf("existing define!\n");
					    //printf("define found\n");
					    if (def->undef) // undefined name
					    {
					      def->undef = FALSE;
					      def->name = g_defName;
					      def->definition = g_defText.stripWhiteSpace();
					      def->nargs = g_defArgs;
					      def->fileName = g_yyFileName.copy(); 
					      def->lineNr = g_yyLineNr;
					    }
					    else
					    {
					      //printf("Error: define %s is defined more than once!\n",g_defName.data());
					    }
					  }
					  delete g_argDict; g_argDict=0;
					  g_yyLineNr++;
					  g_lastGuardName.resize(0);
					  BEGIN(Start);
  					}
<DefineText>{B}*			{ g_defText += ' '; g_defLitText+=yytext; }
<DefineText>{B}*"##"{B}*		{ g_defText += "##"; g_defLitText+=yytext; }
<DefineText>"@"				{ g_defText += "@@"; g_defLitText+=yytext; }
<DefineText>\"				{ 
                                          g_defText += *yytext; 
  					  g_defLitText+=yytext; 
					  if (!g_insideComment)
					  {
					    BEGIN(SkipDoubleQuote);
					  }
  					}
<DefineText>\'				{ g_defText += *yytext;
  					  g_defLitText+=yytext; 
					  if (!g_insideComment)
					  {
  					    BEGIN(SkipSingleQuote);
					  }
					}
<SkipDoubleQuote>"//"			{ g_defText += yytext; g_defLitText+=yytext; }
<SkipDoubleQuote>"/*"			{ g_defText += yytext; g_defLitText+=yytext; }
<SkipDoubleQuote>\"			{
  					  g_defText += *yytext; g_defLitText+=yytext; 
					  BEGIN(DefineText);
  					}
<SkipSingleQuote,SkipDoubleQuote>\\.	{
  					  g_defText += yytext; g_defLitText+=yytext;
					}
<SkipSingleQuote>\'			{
  					  g_defText += *yytext; g_defLitText+=yytext;
					  BEGIN(DefineText);
  					}
<SkipDoubleQuote>.			{ g_defText += *yytext; g_defLitText+=yytext; }
<SkipSingleQuote>.			{ g_defText += *yytext; g_defLitText+=yytext; }
<DefineText>.				{ g_defText += *yytext; g_defLitText+=yytext; }
<<EOF>>					{
                                          //fprintf(stderr,"End of include file\n");
					  //printf("Include stack depth=%d\n",g_includeStack.count());
  					  if (g_includeStack.isEmpty())
					  {
					    //fprintf(stderr,"Terminating scanner!\n");
					    yyterminate();
					  }
					  else
					  {
					    FileState *fs=g_includeStack.pop();
					    //fileDefineCache->merge(g_yyFileName,fs->fileName);
					    if (getFileFilter(fs->fileName.data()).isEmpty())
					      fclose(fs->filePtr);
					    else
					      pclose(fs->filePtr);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->bufState );
					    yy_delete_buffer( oldBuf );
					    g_yyLineNr=fs->lineNr;
					    setFileName(fs->fileName.copy());
					    //fprintf(stderr,"######## FileName %s\n",g_yyFileName.data());
					    
                                            // Deal with file changes due to 
                                            // #include's within { .. } blocks
                                            QCString lineStr;
                                            lineStr.sprintf("# %d \"%s\" 2",g_yyLineNr,g_yyFileName.data());
                                            outputArray(lineStr.data(),lineStr.length());
					    
					    delete fs; fs=0;
					  }
  					}
<*>"/*"					{
					  outputChar('/');outputChar('*');
  					  g_lastCContext=YY_START;
					  g_commentCount=1;
					  BEGIN(SkipCComment);
  					}
<*>"//"					{
					  outputChar('/');outputChar('/');
  					  g_lastCPPContext=YY_START;
					  BEGIN(SkipCPPComment);
					}
<*>\n					{ 
  					  outputChar('\n');
  					  g_yyLineNr++; 
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
  else if (rest && !rest->isEmpty())
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
  else if (rest && !rest->isEmpty())
  {
    int cc=rest->at(0);
    //printf("%c=rest\n",cc);
    return cc;
  }
  else
  {
    int cc=yyinput();
    returnCharToStream(cc);
    //unput((char)cc);
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
    //unput(c);
    returnCharToStream(c);
  }
  //printf("result: unputChar(%s,%s,%d,%c)\n",expr.data(),rest ? rest->data() : 0,pos,c);
}

void addSearchDir(const char *dir)
{
  QFileInfo fi(dir);
  if (fi.isDir()) g_pathList->append(fi.absFilePath());
} 

void initPreprocessor()
{
  g_pathList = new QStrList;
  addSearchDir(".");
  //defineNameList.setAutoDelete(TRUE);
  //defineNameList.clear();
  //defineDict.clear();
  //fileDefineCache = new DefineCache(1009);
  g_expandedDict = new DefineDict(17);
  g_fileDefineDict = new DefineDict(1009);
}

void cleanUpPreprocessor()
{
  //delete fileDefineCache;
  delete g_fileDefineDict; g_fileDefineDict=0;
  delete g_expandedDict; g_expandedDict=0;
  delete g_pathList; g_pathList=0;
}


void preprocessFile(const char *fileName,BufStr &output)
{
  uint orgOffset=output.curPos();

  g_macroExpansion = Config_getBool("MACRO_EXPANSION");
  g_expandOnlyPredef = Config_getBool("EXPAND_ONLY_PREDEF");
  g_curlyCount=0;
  g_nospaces=FALSE;
  g_outputBuf=&output;
  g_includeStack.setAutoDelete(TRUE);
  g_includeStack.clear();
  g_fileDefineDict->setAutoDelete(TRUE);
  g_fileDefineDict->clear();
  g_expandedDict->setAutoDelete(FALSE);
  g_expandedDict->clear();
  
  // add predefined macros
  char *defStr;
  QStrList &predefList = Config_getList("PREDEFINED");
  QStrListIterator sli(predefList);
  for (sli.toFirst();(defStr=sli.current());++sli)
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
      g_fileDefineDict->insert(def->name,def);

      //printf("#define `%s' `%s' #nargs=%d\n",
      //  def->name.data(),def->definition.data(),def->nargs);
    }
    else if ((i_obrace==-1 || i_obrace>i_equals) &&
	     (i_cbrace==-1 || i_cbrace>i_equals) &&
	     !ds.isEmpty() && (int)ds.length()>i_equals
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
      g_fileDefineDict->insert(def->name,def);

      //printf("#define `%s' `%s' #nargs=%d\n",
      //  def->name.data(),def->definition.data(),def->nargs);
    }
    
  }
 
  QCString inputFilter = getFileFilter(fileName);
  if (inputFilter.isEmpty())
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
    QCString cmd = inputFilter+" \""+fileName+"\"";
    preYYin = popen(cmd,"r");
    if (!preYYin)
    {
      err("Error: could not execute filter %s\n",cmd.data());
      return;
    }
  }
  g_yyLineNr = 1;
  g_level    = 0;
  g_ifcount  = 0;
  setFileName(fileName);
  BEGIN( Start );
  
  g_lastGuardName.resize(0);
  g_guardExpr.resize(0);
  
  preYYlex();
  if (inputFilter.isEmpty())
    fclose(preYYin);
  else
    pclose(preYYin);
  
  if (Debug::isFlagSet(Debug::Preprocessor))
  {
    char *orgPos=output.data()+orgOffset;
    char *newPos=output.data()+output.curPos();
    msg("Preprocessor output (size: %d bytes):\n",newPos-orgPos);
    int line=1;
    msg("---------\n00001 ");
    while (orgPos<newPos) 
    {
      putchar(*orgPos);
      if (*orgPos=='\n') printf("%05d ",++line);
      orgPos++;
    }
    msg("\n---------\n");
  }
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
//  int  preYYwrap() { return 1 ; }
  void preYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

