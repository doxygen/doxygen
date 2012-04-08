/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
#include <errno.h>

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
#include "portable.h"
#include "bufstr.h"
#include "arguments.h"
#include "entry.h"

#define YY_NEVER_INTERACTIVE 1
  

struct FileState
{
  FileState(int size) : fileBuf(size), 
                        oldFileBuf(0), oldFileBufPos(0) {}
  int lineNr;
  BufStr fileBuf;
  BufStr *oldFileBuf;
  int oldFileBufPos;
  YY_BUFFER_STATE bufState;
  QCString fileName;
};  

/** @brief Singleton that manages the defines available while 
 *  proprocessing files. 
 */
class DefineManager
{
  /** Local class used to hold the defines for a single file */
  class DefinesPerFile
  {
    public:
      /** Creates an empty container for defines */
      DefinesPerFile() : m_defines(257), m_includedFiles(17)
      {
        m_defines.setAutoDelete(TRUE);
      }
      /** Destroys the object */
      virtual ~DefinesPerFile()
      {
      }
      /** Adds a define in the context of a file. Will replace 
       *  an existing define with the same name (redefinition)
       *  @param def The Define object to add.
       */
      void addDefine(Define *def)
      {
	Define *d = m_defines.find(def->name);
	if (d!=0) // redefine
	{
	  m_defines.remove(d->name);
	}
	m_defines.insert(def->name,def);
      }
      /** Adds an include file for this file 
       *  @param fileName The name of the include file
       */
      void addInclude(const char *fileName)
      {
	m_includedFiles.insert(fileName,(void*)0x8);
      }
      void collectDefines(DefineDict *dict,QDict<void> &includeStack);
    private:
      DefineDict m_defines;
      QDict<void> m_includedFiles;
  };

  public:
    friend class DefinesPerFile;
    /** Returns a reference to the singleton */
    static DefineManager &instance()
    {
      if (theInstance==0) theInstance = new DefineManager;
      return *theInstance;
    }
    /** Deletes the singleton */
    static void deleteInstance()
    {
      delete theInstance;
      theInstance = 0;
    }
    /** Starts a context in which defines are collected. 
     *  Called at the start of a new file that is preprocessed.
     *  @param fileName the name of the file to process.
     */
    void startContext(const char *fileName)
    {
      //printf("DefineManager::startContext()\n");
      m_contextDefines.clear();
      if (fileName==0) return;
      DefinesPerFile *dpf = m_fileMap.find(fileName);
      if (dpf==0)
      {
	//printf("New file!\n");
	dpf = new DefinesPerFile;
	m_fileMap.insert(fileName,dpf);
      }
    }
    /** Ends the context started with startContext() freeing any
     *  defines collected within in this context.
     */
    void endContext()
    {
      //printf("DefineManager::endContext()\n");
      m_contextDefines.clear();
    }
    /** Add an included file to the current context.
     *  If the file has been pre-processed already, all defines are added
     *  to the context.
     *  @param fileName The name of the include file to add to the context.
     */
    void addFileToContext(const char *fileName)
    {
      if (fileName==0) return;
      //printf("DefineManager::addFileToContext(%s)\n",fileName);
      DefinesPerFile *dpf = m_fileMap.find(fileName);
      if (dpf==0)
      {
	//printf("New file!\n");
	dpf = new DefinesPerFile;
	m_fileMap.insert(fileName,dpf);
      }
      else
      {
	//printf("existing file!\n");
	QDict<void> includeStack(17);
	dpf->collectDefines(&m_contextDefines,includeStack);
      }
    }

    /** Add a define to the manager object.
     *  @param fileName The file in which the define was found
     *  @param def The Define object to add.
     */
    void addDefine(const char *fileName,Define *def)
    {
      if (fileName==0) return;
      //printf("DefineManager::addDefine(%s,%s)\n",fileName,def->name.data());
      Define *d = m_contextDefines.find(def->name);
      if (d!=0) // redefine
      {
	m_contextDefines.remove(d->name);
      }
      m_contextDefines.insert(def->name,def);

      DefinesPerFile *dpf = m_fileMap.find(fileName);
      if (dpf==0)
      {
	dpf = new DefinesPerFile;
      }
      dpf->addDefine(def);
    }

    /** Add an include relation to the manager object.
     *  @param fromFileName file name in which the include was found.
     *  @param toFileName file name that is included.
     */
    void addInclude(const char *fromFileName,const char *toFileName)
    {
      //printf("DefineManager::addInclude(%s,%s)\n",fromFileName,toFileName);
      if (fromFileName==0 || toFileName==0) return;
      DefinesPerFile *dpf = m_fileMap.find(fromFileName);
      if (dpf==0)
      {
	dpf = new DefinesPerFile;
      }
      dpf->addInclude(toFileName);
    }
    /** Returns a Define object given its name or 0 if the Define does
     *  not exist.
     */
    Define *isDefined(const char *name) const
    {
      return m_contextDefines.find(name);
    }
    /** Returns a reference to the defines found in the current context. */
    const DefineDict &defineContext() const
    {
      return m_contextDefines;
    }
  private:
    static DefineManager *theInstance;

    /** Helper function to collect all define for a given file */
    void collectDefinesForFile(const char *fileName,DefineDict *dict)
    {
      if (fileName==0) return;
      DefinesPerFile *dpf = m_fileMap.find(fileName);
      if (dpf)
      {
	QDict<void> includeStack(17);
	dpf->collectDefines(dict,includeStack);
      }
    }

    /** Helper function to return the DefinesPerFile object for a given file name. */
    DefinesPerFile *find(const char *fileName) const
    {
      if (fileName==0) return 0;
      return m_fileMap.find(fileName);
    }

    /** Creates a new DefineManager object */
    DefineManager() : m_fileMap(1009), m_contextDefines(1009)
    {
      m_fileMap.setAutoDelete(TRUE);
    }

    /** Destroys the object */
    virtual ~DefineManager() 
    {
    }

    QDict<DefinesPerFile> m_fileMap;
    DefineDict m_contextDefines;
};

/** Singleton instance */
DefineManager *DefineManager::theInstance = 0;

/** Collects all defines for a file and all files that the file includes.
 *  This function will recursively call itself for each file.
 *  @param dict The dictionary to fill with the defines. A redefine will
 *         replace a previous definition.
 *  @param includeStack The stack of includes, used to stop recursion in
 *         case there is a cyclic include dependency.
 */
void DefineManager::DefinesPerFile::collectDefines(
                     DefineDict *dict,QDict<void> &includeStack)
{
  //printf("DefinesPerFile::collectDefines #defines=%d\n",m_defines.count());
  {
    QDictIterator<void> di(m_includedFiles);
    for (di.toFirst();(di.current());++di)
    {
      QCString incFile = di.currentKey();
      DefinesPerFile *dpf = DefineManager::instance().find(incFile);
      if (dpf && includeStack.find(incFile)==0) 
      {
        //printf("  processing include %s\n",incFile.data());
	includeStack.insert(incFile,(void*)0x8);
	dpf->collectDefines(dict,includeStack);
      }
    }
  }
  {
    QDictIterator<Define> di(m_defines);
    Define *def;
    for (di.toFirst();(def=di.current());++di)
    {
      Define *d = dict->find(def->name);
      if (d!=0) // redefine
      {
	dict->remove(d->name);
      }
      dict->insert(def->name,def);
      //printf("  adding define %s\n",def->name.data());
    }
  }
}

/* -----------------------------------------------------------------
 *
 *	scanner's state
 */

static int                g_yyLineNr   = 1;
static QCString           g_yyFileName;
static FileDef           *g_yyFileDef;
static FileDef           *g_inputFileDef;
static int                g_ifcount    = 0;
static QStrList          *g_pathList = 0;  
static QStack<FileState>  g_includeStack;
static QDict<int>        *g_argDict;
static int                g_defArgs = -1;
static QCString           g_defName;
static QCString           g_defText;
static QCString           g_defLitText;
static QCString           g_defArgsStr;
static QCString           g_defExtraSpacing;
static bool               g_defVarArgs;
static int                g_level;
static int                g_lastCContext;
static int                g_lastCPPContext;
static QArray<int>        g_levelGuard;
static BufStr            *g_inputBuf;
static int                g_inputBufPos;
static BufStr            *g_outputBuf;
static int                g_roundCount;
static bool               g_quoteArg;
static DefineDict        *g_expandedDict;
static int                g_findDefArgContext;
static bool               g_expectGuard;
static QCString           g_guardName;
static QCString           g_lastGuardName;
static QCString           g_incName;
static QCString           g_guardExpr;
static int                g_curlyCount;
static bool               g_nospaces; // add extra spaces during macro expansion

static bool               g_macroExpansion; // from the configuration
static bool               g_expandOnlyPredef; // from the configuration
static int                g_commentCount;
static bool               g_insideComment;
static bool               g_isImported;
static QCString           g_blockName;
static int                g_condCtx;
static bool               g_skip;
static QStack<bool>       g_condStack;
static bool               g_insideCS; // C# has simpler preprocessor
static bool               g_isSource;

static bool               g_lexInit = FALSE;

//DefineDict* getGlobalDefineDict() 
//{
//  return g_globalDefineDict;
//}

static void setFileName(const char *name)
{
  bool ambig;
  QFileInfo fi(name);
  g_yyFileName=convertToQCString(fi.absFilePath());
  g_yyFileDef=findFileDef(Doxygen::inputNameDict,g_yyFileName,ambig);
  if (g_yyFileDef==0) // if this is not an input file check if it is an
                      // include file
  {
    g_yyFileDef=findFileDef(Doxygen::includeNameDict,g_yyFileName,ambig);
  }
  //printf("setFileName(%s) g_yyFileName=%s g_yyFileDef=%p\n",
  //    name,g_yyFileName.data(),g_yyFileDef);
  if (g_yyFileDef && g_yyFileDef->isReference()) g_yyFileDef=0;
  g_insideCS = getLanguageFromFileName(g_yyFileName)==SrcLangExt_CSharp;
  g_isSource = guessSection(g_yyFileName);
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
    warn(g_yyFileName,g_yyLineNr,"warning: More #endif's than #if's found.\n");
  }
}

static bool otherCaseDone()
{
  if (g_level==0)
  {
    warn(g_yyFileName,g_yyLineNr,"warning: Found an #else without a preceding #if.\n");
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

#if 0
static bool macroIsAccessible(Define *def)
{
  //printf("macroIsAccessible(%s) input=%s def=%s\n",
  //    def->name.data(),g_inputFileDef?g_inputFileDef->name().data():"<none>",
  //    def->fileDef ? def->fileDef->name().data() : "<none>");
  if (def && def->isPredefined) // predefined macro -> globally accessible
  {
    //printf("%s: predefined macro %s\n",g_inputFileDef->name().data(),def->name.data());
    return TRUE;
  }
  if (def && def->fileDef==g_inputFileDef)
  {
    //printf("%s: macro %s defined in this file at line %d now at %d\n",
    //	g_inputFileDef->name().data(),def->name.data(),def->lineNr,g_yyLineNr);
    return def->lineNr<=g_yyLineNr;
  }
  if (g_inputFileDef && def && def->fileDef) // check if g_inputFileDef actually includes def->fileDef
  {
    QDict<FileDef> includedFiles(257);
    bool b = g_inputFileDef->includes(def->fileDef,&includedFiles);
    //printf("%s: Checking for accessibility of define '%s' (defined in %s): result=%d\n",
    //       g_inputFileDef->name().data(),def->name.data(),def->fileDef->name().data(),b);
    return b;
  }
  if (g_inputFileDef && def && !def->fileName.isEmpty())
  {
    bool b = g_inputFileDef->includesByName(def->fileName);
    //printf("%s: Checking for accessibility of define '%s' (defined in %s): result=%d\n",
    //       g_inputFileDef->name().data(),def->name.data(),def->fileName.data(),b);
    return b;
  }
  //printf("not accessible!\n");
  return FALSE;
}

static Define *isDefined(const char *name)
{
  Define *def=0;
  if (name)
  {
    def=g_globalDefineDict->find(name);
    if (def && def->undef) def=0;
    if (def && !macroIsAccessible(def)) def=0;
  }
  //printf("isDefined(%s)=%p\n",name,def);
  return def;
}
#endif


static QDict<void> g_allIncludes(10009);

static FileState *checkAndOpenFile(const QCString &fileName,bool &alreadyIncluded)
{
  alreadyIncluded = FALSE;
  FileState *fs = 0;
  //printf("checkAndOpenFile(%s)\n",fileName.data());
  QFileInfo fi(fileName);
  if (fi.exists() && fi.isFile())
  {
    static QStrList &exclPatterns = Config_getList("EXCLUDE_PATTERNS");
    if (patternMatch(fi,&exclPatterns)) return 0;

    QCString absName = convertToQCString(fi.absFilePath());

    // global guard
    if (g_curlyCount==0) // not #include inside { ... }
    {
      if (g_allIncludes.find(absName)!=0)
      {
        alreadyIncluded = TRUE;
        //printf("  already included 1\n");
        return 0; // already done
      }
      g_allIncludes.insert(absName,(void *)0x8);
    }
    // check include stack for absName

    QStack<FileState> tmpStack;
    g_includeStack.setAutoDelete(FALSE);
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

    if (alreadyIncluded)
    {
      //printf("  already included 2\n");
      return 0;
    }
    //printf("#include %s\n",absName.data());

    fs = new FileState(fi.size()+4096);
    alreadyIncluded = FALSE;
    if (!readInputFile(absName,fs->fileBuf))
    { // error
      //printf("  error reading\n");
      delete fs;
      fs=0;
    }
    else
    {
      fs->oldFileBuf    = g_inputBuf;
      fs->oldFileBufPos = g_inputBufPos;
    }
  }
  return fs;
}

static FileState *findFile(const char *fileName,bool localInclude,bool &alreadyIncluded)
{
  //printf("** findFile(%s,%d) g_yyFileName=%s\n",fileName,localInclude,g_yyFileName.data());
  if (localInclude && !g_yyFileName.isEmpty())
  {
    QFileInfo fi(g_yyFileName);
    if (fi.exists())
    {
      QCString absName = QCString(fi.dirPath(TRUE).data())+"/"+fileName;
      FileState *fs = checkAndOpenFile(absName,alreadyIncluded);
      if (fs)
      {
	setFileName(absName);
	g_yyLineNr=1;
	return fs;
      }
      else if (alreadyIncluded)
      {
	return 0;
      }
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
    //printf("  Looking for %s in %s\n",fileName,s);
    FileState *fs = checkAndOpenFile(absName,alreadyIncluded);
    if (fs)
    {
      setFileName(absName);
      g_yyLineNr=1;
      //printf("  -> found it\n");
      return fs;
    }
    else if (alreadyIncluded)
    {
      return 0;
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
	    if (i==0) 
	    {
	      i++;
	    }
	    // only /*!< or /**< are treated as a comment for the macro name,
	    // otherwise the comment is treated as part of the macro definition
	    return ((s[i+1]=='*' || s[i+1]=='!') && s[i+2]=='<') ? &s[i-1] : ""; 
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
  int cc;
  while ((cc=getNextChar(expr,rest,pos))!=EOF)
  {
    if (cc=='\\') arg+=(char)cc,cc=getNextChar(expr,rest,pos);
    else if (cc==term) return;
    arg+=(char)cc;
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
  while ((cc=getCurrentChar(expr,rest,j))!=EOF && isspace(cc)) 
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
    //printf("Macro definition: %s\n",d.data());
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
  return FALSE;
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
    if (isdigit(c)) // skip number
    {
      while (p<(int)expr.length() && isId(expr.at(p))) p++;
    }
    else if (isalpha(c) || c=='_') // read id
    {
      n=p-1;
      while (p<(int)expr.length() && isId(expr.at(p))) p++;
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
      if (p<(int)expr.length()) ++p; // skip closing quote
    }
    else if (c=='/') // skip C Comment
    {
      //printf("Found C comment at p=%d\n",p);
      char pc=c;
      if (p<(int)expr.length()) 
      {
	c=expr.at(p);
        if (c=='*')  // Start of C comment
        { 
	  p++;
  	  while (p<(int)expr.length() && !(pc=='*' && c=='/'))
	  {
	    pc=c;
	    c=expr.at(p++);
	  }
        }
      }
      //printf("Found end of C comment at p=%d\n",p);
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
  bool definedTest=FALSE;
  int i=pos,l,p,len;
  while ((p=getNextId(expr,i,&l))!=-1) // search for an macro name
  {
    bool replaced=FALSE;
    macroName=expr.mid(p,l);
    //printf("macroName=%s\n",macroName.data());
    if (p<2 || !(expr.at(p-2)=='@' && expr.at(p-1)=='-')) // no-rescan marker?
    {
      if (g_expandedDict->find(macroName)==0) // expand macro
      {
	Define *def=DefineManager::instance().isDefined(macroName);
	if (definedTest) // macro name was found after defined 
	{
	  if (def) expMacro = " 1 "; else expMacro = " 0 ";
	  replaced=TRUE;
	  len=l;
	  definedTest=FALSE;
	}
	else if (def && def->nargs==-1) // simple macro
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
        else if (macroName=="defined")
        {
  	  //printf("found defined inside macro definition '%s'\n",expr.right(expr.length()-p).data());
	  definedTest=TRUE;
        }

	if (replaced) // expand the macro and rescan the expression
	{
	    
	  //printf("replacing `%s'->`%s'\n",expr.mid(p,len).data(),expMacro.data());
	  QCString resultExpr=expMacro;
	  QCString restExpr=expr.right(expr.length()-len-p);
	  processConcatOperators(resultExpr);
	  if (def && !def->nonRecursive)
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
	if (*p=='(') // undefined function macro
	{
	  p++;
	  int count=1;
	  while ((c=*p++))
	  {
	    if (c=='(') count++;
	    else if (c==')')
	    {
	      count--;
	      if (count==0) break;
	    }
	    else if (c=='/')
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
	  }
	}
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
	else // oops, not comment but division
	{
	  result+=pc;
	  goto nextChar;
	}
      }
      else 
      {
nextChar:
	result+=c;
	char lc=tolower(c);
	if (!isId(lc) && lc!='.' /*&& lc!='-' && lc!='+'*/) inNum=FALSE;
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
      switch(c)
      {
	case '@': // replace @@ with @
	  {
	    if (*(p+1)=='@')
	    {
	      result+=c; 
	    }
	    p+=2;
	  }
	  break;
	case '/': // skip C comments
	  {
	    result+=c;
	    char pc=c;
	    c=*++p;
	    if (c=='*') // start of C comment
	    { 
	      while (*p && !(pc=='*' && c=='/')) // search end of comment
	      {
		if (*p=='@' && *(p+1)=='@') 
		  result+=c,p++;
		else 
		  result+=c;
		pc=c;
		c=*++p;
	      }
	      if (*p) result+=c,p++;
	    }
	  }
	  break;
	case '"': // skip string literals
	  {
	    result+=c;
	    char pc=c;
	    c=*++p;
	    while (*p && (c!='"' || pc=='\\')) // no end quote
	    {
	      result+=c;
	      c=*++p;
	    }
	    if (*p) result+=c,p++; 
	  }
	  break;
	case '\'': // skip char literals
	  {
	    result+=c;
	    char pc=c;
	    c=*++p;
	    while (*p && (c!='\'' || pc=='\\')) // no end quote
	    {
	      result+=c;
	      c=*++p;
	    }
	    if (*p) result+=c,p++; 
	  }
	  break;
	default:
	  {
	    result+=c;
	    p++;
	  }
	  break;
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
  def->name       = g_defName;
  def->definition = g_defText.stripWhiteSpace();
  def->nargs      = g_defArgs;
  def->fileName   = g_yyFileName; 
  def->fileDef    = g_yyFileDef;
  def->lineNr     = g_yyLineNr;
  def->varArgs    = g_defVarArgs;
  //printf("newDefine: %s %s file: %s\n",def->name.data(),def->definition.data(),
  //    def->fileDef ? def->fileDef->name().data() : def->fileName.data());
  //printf("newDefine: `%s'->`%s'\n",def->name.data(),def->definition.data());
  if (!def->name.isEmpty() && Doxygen::expandAsDefinedDict[def->name])
  {
    def->isPredefined=TRUE;
  }
  return def;
}

void addDefine()
{
  if (g_skip) return; // do not add this define as it is inside a 
                      // conditional section (cond command) that is disabled.
  if (!Doxygen::gatherDefines) return;

  //printf("addDefine %s %s\n",g_defName.data(),g_defArgsStr.data());
  //ArgumentList *al = new ArgumentList;
  //stringToArgumentList(g_defArgsStr,al);
  MemberDef *md=new MemberDef(
      g_yyFileName,g_yyLineNr,
      "#define",g_defName,g_defArgsStr,0,
      Public,Normal,FALSE,Member,MemberDef::Define,0,0);
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

  //printf("pre.l: md->setFileDef(%p)\n",g_inputFileDef);
  md->setFileDef(g_inputFileDef);
  md->setDefinition("#define "+g_defName);

  MemberName *mn=Doxygen::functionNameSDict->find(g_defName);
  if (mn==0)
  {
    mn = new MemberName(g_defName);
    Doxygen::functionNameSDict->append(g_defName,mn);
  }
  mn->append(md);
  if (g_yyFileDef) 
  {
    g_yyFileDef->insertMember(md);
  }

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
  static bool searchIncludes = Config_getBool("SEARCH_INCLUDES");
  if (!searchIncludes) return; // do not read include files
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

    QCString dosExt = incFileName.right(4);
    if (dosExt==".exe" || dosExt==".dll" || dosExt==".tlb")
    {
      // skip imported binary files (e.g. M$ type libraries)
      return;
    }

    QCString oldFileName = g_yyFileName;
    FileDef *oldFileDef  = g_yyFileDef;
    int oldLineNr        = g_yyLineNr;
    //printf("Searching for `%s'\n",incFileName.data());

    // absIncFileName avoids difficulties for incFileName starting with "../" (bug 641336)
    QCString absIncFileName = incFileName;
    {
      static bool searchIncludes = Config_getBool("SEARCH_INCLUDES");
      QFileInfo fi(g_yyFileName);
      if (fi.exists())
      {
	QCString absName = QCString(fi.dirPath(TRUE).data())+"/"+incFileName;
        QFileInfo fi2(absName);
        if (fi2.exists())
        {
	  absIncFileName=fi2.absFilePath().utf8();
	}
	else if (searchIncludes) // search in INCLUDE_PATH as well
	{
	  QStrList &includePath = Config_getList("INCLUDE_PATH");
	  char *s=includePath.first();
	  while (s)
	  {
	    QFileInfo fi(s);
	    if (fi.exists() && fi.isDir())
	    {
	      QCString absName = QCString(fi.absFilePath().utf8())+"/"+incFileName;
	      //printf("trying absName=%s\n",absName.data());
	      QFileInfo fi2(absName);
	      if (fi2.exists())
	      {
		absIncFileName=fi2.absFilePath().utf8();
		break;
	      }
	      //printf( "absIncFileName = %s\n", absIncFileName.data() );
	    }
	    s=includePath.next();
	  }
	}
	//printf( "absIncFileName = %s\n", absIncFileName.data() );
      }
    }
    DefineManager::instance().addInclude(g_yyFileName,absIncFileName);
    DefineManager::instance().addFileToContext(absIncFileName);

    // findFile will overwrite g_yyFileDef if found
    FileState *fs;
    bool alreadyIncluded = FALSE;
    //printf("calling findFile(%s)\n",incFileName.data());
    if ((fs=findFile(incFileName,localInclude,alreadyIncluded))) // see if the include file can be found
    {
      //printf("Found include file!\n");
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        for (i=0;i<g_includeStack.count();i++) 
        {
          Debug::print(Debug::Preprocessor,0,"  ");
        }
        //msg("#include %s: parsing...\n",incFileName.data());
      }
      if (oldFileDef)
      {
        // add include dependency to the file in which the #include was found
	bool ambig;
	// change to absolute name for bug 641336 
        FileDef *incFd = findFileDef(Doxygen::inputNameDict,absIncFileName,ambig);
        oldFileDef->addIncludeDependency(ambig ? 0 : incFd,incFileName,localInclude,g_isImported,FALSE);
        // add included by dependency
        if (g_yyFileDef)
        {
          //printf("Adding include dependency %s->%s\n",oldFileDef->name().data(),incFileName.data());
          g_yyFileDef->addIncludedByDependency(oldFileDef,oldFileDef->docName(),localInclude,g_isImported);
        }
      }
      else if (g_inputFileDef)
      {
        g_inputFileDef->addIncludeDependency(0,absIncFileName,localInclude,g_isImported,TRUE);
      }
      fs->bufState = YY_CURRENT_BUFFER;
      fs->lineNr   = oldLineNr;
      fs->fileName = oldFileName;
      // push the state on the stack
      g_includeStack.push(fs);
      // set the scanner to the include file

      // Deal with file changes due to 
      // #include's within { .. } blocks
      QCString lineStr(g_yyFileName.length()+20);
      lineStr.sprintf("# 1 \"%s\" 1\n",g_yyFileName.data());
      outputArray(lineStr.data(),lineStr.length());

      //fprintf(stderr,"Switching to include file %s\n",incFileName.data());
      g_expectGuard=TRUE;
      g_inputBuf   = &fs->fileBuf;
      g_inputBufPos=0;
      yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE));
    }
    else
    {
      //printf("  calling findFile(%s) alreadyInc=%d\n",incFileName.data(),alreadyIncluded);
      if (oldFileDef)
      {
	bool ambig;
	//QCString absPath = incFileName;
	//if (QDir::isRelativePath(incFileName))
	//{
	//  absPath = QDir::cleanDirPath(oldFileDef->getPath()+"/"+incFileName);
	//  //printf("%s + %s -> resolved path %s\n",oldFileDef->getPath().data(),incFileName.data(),absPath.data());
	//}

	// change to absolute name for bug 641336 
	FileDef *fd = findFileDef(Doxygen::inputNameDict,absIncFileName,ambig);
	//printf("%s::findFileDef(%s)=%p\n",oldFileDef->name().data(),incFileName.data(),fd);
	// add include dependency to the file in which the #include was found
	oldFileDef->addIncludeDependency(ambig ? 0 : fd,incFileName,localInclude,g_isImported,FALSE);
	// add included by dependency
        if (fd)
        {
          //printf("Adding include dependency (2) %s->%s ambig=%d\n",oldFileDef->name().data(),fd->name().data(),ambig);
          fd->addIncludedByDependency(oldFileDef,oldFileDef->docName(),localInclude,g_isImported);
        }
      }
      else if (g_inputFileDef)
      {
        g_inputFileDef->addIncludeDependency(0,absIncFileName,localInclude,g_isImported,TRUE);
      }
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
	if (alreadyIncluded)
	{
          Debug::print(Debug::Preprocessor,0,"#include %s: already included! skipping...\n",incFileName.data());
	}
	else
	{
          Debug::print(Debug::Preprocessor,0,"#include %s: not found! skipping...\n",incFileName.data());
	}
        //printf("error: include file %s not found\n",yytext);
      }
      if (g_curlyCount>0 && !alreadyIncluded) // failed to find #include inside { ... }
      {
	warn(g_yyFileName,g_yyLineNr,"Warning: include file %s not found, perhaps you forgot to add its directory to INCLUDE_PATH?",incFileName.data());
      }
    }
  }
}

/* ----------------------------------------------------------------- */

static void startCondSection(const char *sectId)
{
  g_condStack.push(new bool(g_skip));
  if (Config_getList("ENABLED_SECTIONS").find(sectId)==-1)
  {
    g_skip=TRUE;
  }
}

static void endCondSection()
{
  if (g_condStack.isEmpty())
  {
    g_skip=FALSE;
  }
  else
  {
    bool *ctx = g_condStack.pop();
    g_skip=*ctx;
  }
}

static void forceEndCondSection()
{
  while (!g_condStack.isEmpty())
  {
    g_condStack.pop();
  }
  g_skip=FALSE;
}

static QCString escapeAt(const char *text)
{
  QCString result;
  if (text)
  {
    char c;
    const char *p=text;
    while ((c=*p++))
    {
      if (c=='@') result+="@@"; else result+=c;
    }
  }
  return result;
}

static char resolveTrigraph(char c)
{
  switch (c)
  {
    case '=': return '#';
    case '/': return '\\';
    case '\'': return '^';
    case '(': return '[';
    case ')': return ']';
    case '!': return '|';
    case '<': return '{';
    case '>': return '}';
    case '-': return '~';
  }
  return '?';
}

/* ----------------------------------------------------------------- */

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int bytesInBuf = g_inputBuf->curPos()-g_inputBufPos;
  int bytesToCopy = QMIN(max_size,bytesInBuf);
  memcpy(buf,g_inputBuf->data()+g_inputBufPos,bytesToCopy);
  g_inputBufPos+=bytesToCopy;
  return bytesToCopy;
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
%x	SkipString
%x	CopyLine
%x	CopyString
%x      Include
%x      IncludeID
%x      EndImport
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
%x	CondLine

%%

<*>\x06					
<*>\x00
<*>\r
<*>"??"[=/'()!<>-]			{ // Trigraph
  					  unput(resolveTrigraph(yytext[2]));
  					}
<Start>^{B}*"#"				{ BEGIN(Command); }
<Start>^{B}*/[^#]			{
 					  outputArray(yytext,yyleng); 
  					  BEGIN(CopyLine); 
					}
<Start>^{B}*[_A-Z][_A-Z0-9]*{B}*"("[^\)\n]*")"/{BN}{1,10}*[:{] { // constructors?
					  int i;
					  for (i=yyleng-1;i>=0;i--)
					  {
					    unput(yytext[i]);
					  }
					  BEGIN(CopyLine);
                                        }
<Start>^{B}*[_A-Z][_A-Z0-9]*{B}*"("[^\(\)\n]*"("[^\)\n]*")"[^\)\n]*")"{B}*\n | // function list macro with one (...) argument, e.g. for K_GLOBAL_STATIC_WITH_ARGS
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
					         (def=DefineManager::instance().isDefined(name)) &&
						 /*macroIsAccessible(def) &&*/
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
<CopyLine>{ID}/{BN}{0,80}"("		{
  					  g_expectGuard = FALSE;
  					  Define *def=0;
					  //def=g_globalDefineDict->find(yytext);
					  //def=DefineManager::instance().isDefined(yytext);
					  //printf("Search for define %s found=%d g_includeStack.isEmpty()=%d "
					  //       "g_curlyCount=%d g_macroExpansion=%d g_expandOnlyPredef=%d "
					  //	 "isPreDefined=%d\n",yytext,def ? 1 : 0,
					  //	 g_includeStack.isEmpty(),g_curlyCount,g_macroExpansion,g_expandOnlyPredef,
					  //	 def ? def->isPredefined : -1
					  //	);
					  if ((g_includeStack.isEmpty() || g_curlyCount>0) &&
					      g_macroExpansion &&
					      (def=DefineManager::instance().isDefined(yytext)) &&
				              /*(def->isPredefined || macroIsAccessible(def)) && */
					      (!g_expandOnlyPredef || def->isPredefined)
					     )
					  {
					    //printf("Found it! #args=%d\n",def->nargs);
					    g_roundCount=0;
					    g_defArgsStr=yytext;
					    if (def->nargs==-1) // no function macro
					    {
					      QCString result = def->isPredefined ? def->definition : expandMacro(g_defArgsStr);
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
  					  if ((g_includeStack.isEmpty() || g_curlyCount>0) && 
					      g_macroExpansion &&
					      (def=DefineManager::instance().isDefined(yytext)) &&
					      def->nargs==-1 &&
				              /*(def->isPredefined || macroIsAccessible(def)) &&*/
					      (!g_expandOnlyPredef || def->isPredefined)
					     )
					  {
					    QCString result=def->isPredefined ? def->definition : expandMacro(yytext); 
					    outputArray(result,result.length());
					  }
					  else
					  {
					    outputArray(yytext,yyleng);
					  }
  					}
<CopyLine>"\\"\r?/\n			{ // strip line continuation characters
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
<Command>("include"|"import"){B}+/{ID}	{
  					  g_isImported = yytext[1]=='m';
  					  if (g_macroExpansion) 
					    BEGIN(IncludeID);
  					}
<Command>("include"|"import"){B}*[<"]	{ 
  					  g_isImported = yytext[1]=='m';
					  char c[2];
					  c[0]=yytext[yyleng-1];c[1]='\0';
					  g_incName=c;
  					  BEGIN(Include); 
					}
<Command>("cmake")?"define"{B}+		{ 
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
<Command>"pragma"{B}+"once"             {
                                          g_expectGuard = FALSE;
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
  					  if ((def=DefineManager::instance().isDefined(yytext)) 
					      /*&& !def->isPredefined*/
					      && !def->nonRecursive
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
<Guard>{ID}				{ g_guardExpr+=yytext; }
<Guard>.				{ g_guardExpr+=*yytext; }
<Guard>\n				{
  					  unput(*yytext);
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
  					  if (DefineManager::instance().isDefined(yytext) || g_guardName==yytext)
					    g_guardExpr+=" 1L ";
					  else
					    g_guardExpr+=" 0L ";
					  g_lastGuardName=yytext;
					  BEGIN(Guard);
  					}
<DefinedExpr2>{ID}			{
  					  if (DefineManager::instance().isDefined(yytext) || g_guardName==yytext)
					    g_guardExpr+=" 1L ";
					  else
					    g_guardExpr+=" 0L ";
					  g_lastGuardName=yytext;
  					}
<DefinedExpr1,DefinedExpr2>\n		{ // should not happen, handle anyway
                                          g_yyLineNr++;
  					  g_ifcount=0;
 					  BEGIN(SkipCPPBlock); 
					}
<DefinedExpr2>")"			{
  					  BEGIN(Guard);
  					}
<DefinedExpr1,DefinedExpr2>.
<SkipCPPBlock>^{B}*"#"			{ BEGIN(SkipCommand); }
<SkipCPPBlock>^{B}*/[^#]		{ BEGIN(SkipLine); }
<SkipCPPBlock>\n			{ g_yyLineNr++; outputChar('\n'); }
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
					  g_expectGuard = FALSE;
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
<SkipLine>[^'"/\n]+			
<SkipLine>{CHARLIT}			{ }
<SkipLine>\"				{
					  BEGIN(SkipString);
					}
<SkipLine>.
<SkipString>"//"/[^\n]*                 { 
                                        }
<SkipLine,SkipCommand,SkipCPPBlock>"//"[^\n]* {
  					  g_lastCPPContext=YY_START;
  					  BEGIN(RemoveCPPComment);
					}
<SkipString>"/*"/[^\n]*                 { 
                                        }
<SkipLine,SkipCommand,SkipCPPBlock>"/*"/[^\n]* {
					  g_lastCContext=YY_START;
  					  BEGIN(RemoveCComment);
  					}
<SkipLine>\n				{
  					  outputChar('\n');
					  g_yyLineNr++;  
					  BEGIN(SkipCPPBlock);
					}
<SkipString>[^"\\\n]+			{ }
<SkipString>\\.				{ }
<SkipString>\"				{
  					  BEGIN(SkipLine);
  					}
<SkipString>.				{ }
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
					  if (g_isImported)
					  {
					    BEGIN(EndImport);
					  }
					  else
					  {
					    BEGIN(Start);
					  }
  					}
<EndImport>[^\\\n]*/\n			{
  					  BEGIN(Start);
  					}
<EndImport>\\[\r]?"\n"			{ 
					  outputChar('\n');
					  g_yyLineNr++;
					}
<EndImport>.				{
  					}
<DefName>{ID}/("\\\n")*"("		{ // define with argument
  					  //printf("Define() `%s'\n",yytext);
					  g_argDict = new QDict<int>(31);
					  g_argDict->setAutoDelete(TRUE);
					  g_defArgs = 0; 
                                          g_defArgsStr.resize(0);
					  g_defText.resize(0);
					  g_defLitText.resize(0);
					  g_defName = yytext;
					  g_defVarArgs = FALSE;
					  g_defExtraSpacing.resize(0);
					  BEGIN(DefineArg);
  					}
<DefName>{ID}{B}+"1"/[ \r\t\n]		{ // special case: define with 1 -> can be "guard"
  					  //printf("Define `%s'\n",yytext);
  					  g_argDict = 0;
					  g_defArgs = -1;
                                          g_defArgsStr.resize(0);
					  g_defName = yytext;
					  g_defName = g_defName.left(g_defName.length()-1).stripWhiteSpace();
					  g_defVarArgs = FALSE;
					  //printf("Guard check: %s!=%s || %d\n",
					  //    g_defName.data(),g_lastGuardName.data(),g_expectGuard);
					  if ( g_defName!=g_lastGuardName || !g_expectGuard)
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+g_defName;
					    outputArray(tmp.data(),tmp.length());
					    g_quoteArg=FALSE;
					    g_insideComment=FALSE;
					    g_lastGuardName.resize(0);
				            g_defText="1"; 
					    g_defLitText="1"; 
					    BEGIN(DefineText); 
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
					    g_defText.resize(0);
					    g_defLitText.resize(0);
					    BEGIN(Start);
					  }
					  g_expectGuard=FALSE;
  					}
<DefName>{ID}/{B}*"\n"			{ // empty define
  					  g_argDict = 0;
					  g_defArgs = -1;
					  g_defName = yytext;
                                          g_defArgsStr.resize(0);
					  g_defText.resize(0);
					  g_defLitText.resize(0);
					  g_defVarArgs = FALSE;
					  //printf("Guard check: %s!=%s || %d\n",
					  //    g_defName.data(),g_lastGuardName.data(),g_expectGuard);
					  if ( g_defName!=g_lastGuardName || !g_expectGuard)
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+g_defName;
					    outputArray(tmp.data(),tmp.length());
					    g_quoteArg=FALSE;
					    g_insideComment=FALSE;
					    if (g_insideCS) g_defText="1"; // for C#, use "1" as define text
					    BEGIN(DefineText);
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
					    g_guardName = yytext;
					    g_lastGuardName.resize(0);
					    BEGIN(Start);
					  }
					  g_expectGuard=FALSE;
  					}
<DefName>{ID}/{B}*			{ // define with content
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
<DefineArg>"\\\n"                       {
  					  g_defExtraSpacing+="\n";
					  g_yyLineNr++;
                                        }
<DefineArg>","{B}*			{ g_defArgsStr+=yytext; }
<DefineArg>"("{B}*                      { g_defArgsStr+=yytext; }
<DefineArg>{B}*")"{B}*			{
                                          g_defArgsStr+=yytext; 
					  QCString tmp=(QCString)"#define "+g_defName+g_defArgsStr+g_defExtraSpacing;
					  outputArray(tmp.data(),tmp.length());
					  g_quoteArg=FALSE;
					  g_insideComment=FALSE;
  					  BEGIN(DefineText);
  					}
<DefineArg>"..."			{ // Variadic macro
					  g_defVarArgs = TRUE;
					  g_defArgsStr+=yytext;
					  g_argDict->insert("__VA_ARGS__",new int(g_defArgs));
					  g_defArgs++;
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
<DefineText>"/*"[!*]?			{
					  g_defText+=yytext;
					  g_defLitText+=yytext;
					  g_lastCContext=YY_START;
					  g_commentCount=1;
  					  BEGIN(CopyCComment);
  					}
<DefineText>"//"[!/]?			{
  					  outputArray(yytext,yyleng);
  					  g_lastCPPContext=YY_START;
					  g_defLitText+=' ';
  					  BEGIN(SkipCPPComment);
  					}
<SkipCComment>[/]?"*/"			{
  					  if (yytext[0]=='/') outputChar('/');
  					  outputChar('*');outputChar('/');
					  if (--g_commentCount<=0)
					  {
					    if (g_lastCContext==Start) 
					      // small hack to make sure that ^... rule will
					      // match when going to Start... Example: "/*...*/ some stuff..."
					    {
					      YY_CURRENT_BUFFER->yy_at_bol=1;
					    }
  					    BEGIN(g_lastCContext);  
					  }
  					}
<SkipCComment>"//"("/")*		{
  					  outputArray(yytext,yyleng);
  					}
<SkipCComment>"/*"			{
  					  outputChar('/');outputChar('*');
					  //g_commentCount++;
  					}
<SkipCComment>[\\@][\\@]("f{"|"f$"|"f[") {
  					  outputArray(yytext,yyleng);
  					}
<SkipCComment>[\\@][\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"rtfonly"|"manonly"|"dot"|"code"){BN}+ {
  					  outputArray(yytext,yyleng);
  					  g_yyLineNr+=QCString(yytext).contains('\n');
  					}
<SkipCComment>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"rtfonly"|"manonly"|"dot"|"code"){BN}+	{
  					  outputArray(yytext,yyleng);
  					  g_yyLineNr+=QCString(yytext).contains('\n');
					  if (yytext[1]=='f')
					  {
					    g_blockName="f";
					  }
					  else
					  {
					    g_blockName=QCString(&yytext[1]).stripWhiteSpace();
					  }
					  BEGIN(SkipVerbatim);
  					}
<SkipCComment,SkipCPPComment>[\\@]"cond"[ \t]+	{ // conditional section
  					  g_condCtx = YY_START;
  					  outputArray(yytext,yyleng);
  					  BEGIN(CondLine);
  					}
<CondLine>[a-z_A-Z][a-z_A-Z0-9.\-]*	{
  				          startCondSection(yytext);
  					  outputArray(yytext,yyleng);
  					  BEGIN(g_condCtx);
  					}
<SkipCComment,SkipCPPComment>[\\@]"cond"[ \t\r]*/\n {
  					  g_condCtx = YY_START;
  					  outputArray(yytext,yyleng);
  					}
<CondLine>.				{
  					  unput(*yytext);
  					  startCondSection(" ");
					  BEGIN(g_condCtx);
  					}
<SkipCComment,SkipCPPComment>[\\@]"endcond"/[^a-z_A-Z0-9] {
  					  outputArray(yytext,yyleng);
  					  endCondSection();
  					}
<SkipVerbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"f$"|"f]"|"f}") { /* end of verbatim block */
  					  outputArray(yytext,yyleng);
					  if (yytext[1]=='f' && g_blockName=="f")
					  {
					    BEGIN(SkipCComment);
					  }
					  else if (&yytext[4]==g_blockName)
					  {
					    BEGIN(SkipCComment);
					  }
  					}
<SkipVerbatim>"*/"|"/*"			{
  					  outputArray(yytext,yyleng);
  					}
<SkipCComment,SkipVerbatim>[^*\\@\x06\n\/]+ {
  					  outputArray(yytext,yyleng);
  					}
<SkipCComment,SkipVerbatim>\n		{ 
  					  g_yyLineNr++;
  					  outputChar('\n');
  					}
<SkipCComment,SkipVerbatim>.		{
  					  outputChar(*yytext);
  					}
<CopyCComment>[^*a-z_A-Z\n]+		{
					  g_defLitText+=yytext;
					  g_defText+=escapeAt(yytext);
  					}
<CopyCComment>"*/"			{
					  g_defLitText+=yytext;
					  g_defText+=yytext;
  					  BEGIN(g_lastCContext);
  					}
<CopyCComment>\n			{ 
  					  g_yyLineNr++;
					  g_defLitText+=yytext;
					  g_defText+=' ';
  					}
<RemoveCComment>"*/"{B}*"#"	        { // see bug 594021 for a usecase for this rule
                                          if (g_lastCContext==SkipCPPBlock)
					  {
					    BEGIN(SkipCommand);
					  }
					  else
					  {
					    REJECT;
					  }
					}
<RemoveCComment>"*/"		        { BEGIN(g_lastCContext); }
<RemoveCComment>"//"			
<RemoveCComment>"/*"
<RemoveCComment>[^*\x06\n]+
<RemoveCComment>\n			{ g_yyLineNr++; outputChar('\n'); }
<RemoveCComment>.			
<SkipCPPComment>[^\n\/\\@]+		{
  					  outputArray(yytext,yyleng);
  					}
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
<SkipCPPComment>[^\x06\@\\\n]+		{
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
<DefineText,CopyCComment>{ID}		{
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
<CopyCComment>.				{
					  g_defLitText+=yytext;
					  g_defText+=yytext;
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
					  def=DefineManager::instance().isDefined(g_defName);
					  if (def==0) // new define
					  {
					    //printf("new define '%s'!\n",g_defName.data());
					    Define *nd = newDefine();
					    DefineManager::instance().addDefine(g_yyFileName,nd);

					    // also add it to the local file list if it is a source file
					    //if (g_isSource && g_includeStack.isEmpty())
					    //{
					    //  g_fileDefineDict->insert(g_defName,nd);
					    //}
					  }
					  else if (def /*&& macroIsAccessible(def)*/)
					       // name already exists
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
					      //printf("error: define %s is defined more than once!\n",g_defName.data());
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
<SkipDoubleQuote>"//"[/]?		{ g_defText += yytext; g_defLitText+=yytext; }
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
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->bufState );
					    yy_delete_buffer( oldBuf );
					    g_yyLineNr    = fs->lineNr;
                                            //preYYin = fs->oldYYin;
                                            g_inputBuf    = fs->oldFileBuf;
					    g_inputBufPos = fs->oldFileBufPos;
					    setFileName(fs->fileName);
					    //fprintf(stderr,"######## FileName %s\n",g_yyFileName.data());
					    
                                            // Deal with file changes due to 
                                            // #include's within { .. } blocks
                                            QCString lineStr(15+g_yyFileName.length());
                                            lineStr.sprintf("# %d \"%s\" 2",g_yyLineNr,g_yyFileName.data());
                                            outputArray(lineStr.data(),lineStr.length());
					    
					    delete fs; fs=0;
					  }
  					}
<*>"/*"/"*/"				|
<*>"/*"[*]?				{
					  outputArray(yytext,yyleng);
  					  g_lastCContext=YY_START;
					  g_commentCount=1;
					  if (yyleng==3) g_lastGuardName.resize(0); // reset guard in case the #define is documented!
					  BEGIN(SkipCComment);
  					}
<*>"//"[/]?				{
					  outputArray(yytext,yyleng);
  					  g_lastCPPContext=YY_START;
					  if (yyleng==3) g_lastGuardName.resize(0); // reset guard in case the #define is documented!
					  BEGIN(SkipCPPComment);
					}
<*>\n					{ 
  					  outputChar('\n');
  					  g_yyLineNr++; 
					}
<*>.				        {
  					  g_expectGuard = FALSE;
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
  if (fi.isDir()) g_pathList->append(fi.absFilePath().utf8());
} 

void initPreprocessor()
{
  g_pathList = new QStrList;
  addSearchDir(".");
  g_expandedDict = new DefineDict(17);
}

void cleanUpPreprocessor()
{
  delete g_expandedDict; g_expandedDict=0;
  delete g_pathList; g_pathList=0;
  DefineManager::deleteInstance();
}


void preprocessFile(const char *fileName,BufStr &input,BufStr &output)
{
  uint orgOffset=output.curPos();
  //printf("##########################\n%s\n####################\n",
  //    input.data());

  g_macroExpansion = Config_getBool("MACRO_EXPANSION");
  g_expandOnlyPredef = Config_getBool("EXPAND_ONLY_PREDEF");
  g_curlyCount=0;
  g_nospaces=FALSE;
  g_inputBuf=&input;
  g_inputBufPos=0;
  g_outputBuf=&output;
  g_includeStack.setAutoDelete(TRUE);
  g_includeStack.clear();
  g_expandedDict->setAutoDelete(FALSE);
  g_expandedDict->clear();
  g_condStack.clear();
  g_condStack.setAutoDelete(TRUE);
  //g_fileDefineDict->clear();

  setFileName(fileName);
  g_inputFileDef = g_yyFileDef;
  DefineManager::instance().startContext(g_yyFileName);
  
  static bool firstTime=TRUE;
  if (firstTime)
  {
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
	//printf("predefined function macro '%s'\n",defStr);
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
	QCString dname = ds.left(i_obrace);
	if (!dname.isEmpty())
	{
	  Define *def = new Define;
	  def->name         = dname;
	  def->definition   = definition; 
	  def->nargs        = count;
	  def->isPredefined = TRUE;
	  def->nonRecursive = nonRecursive;
	  def->fileDef      = g_yyFileDef;
	  def->fileName     = fileName;
	  DefineManager::instance().addDefine(g_yyFileName,def);
	}

	//printf("#define `%s' `%s' #nargs=%d\n",
	//  def->name.data(),def->definition.data(),def->nargs);
      }
      else if ((i_obrace==-1 || i_obrace>i_equals) &&
	  (i_cbrace==-1 || i_cbrace>i_equals) &&
	  !ds.isEmpty() && (int)ds.length()>i_equals
	  ) // predefined non-function macro definition
      {
	//printf("predefined normal macro '%s'\n",defStr);
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
	if (!def->name.isEmpty())
	{
	  def->nargs = -1;
	  def->isPredefined = TRUE;
	  def->nonRecursive = nonRecursive;
	  def->fileDef      = g_yyFileDef;
	  def->fileName     = fileName;
	  DefineManager::instance().addDefine(g_yyFileName,def);
	}
	else
	{
	  delete def;
	}

	//printf("#define `%s' `%s' #nargs=%d\n",
	//  def->name.data(),def->definition.data(),def->nargs);
      }
    }
    //firstTime=FALSE;
  }
 
  g_yyLineNr = 1;
  g_level    = 0;
  g_ifcount  = 0;

  BEGIN( Start );
  
  g_expectGuard = guessSection(fileName)==Entry::HEADER_SEC;
  g_guardName.resize(0);
  g_lastGuardName.resize(0);
  g_guardExpr.resize(0);
  
  preYYlex();
  g_lexInit=TRUE;

  // make sure we don't extend a \cond with missing \endcond over multiple files (see bug 624829)
  forceEndCondSection();

  // remove locally defined macros so they can be redefined in another source file
  //if (g_fileDefineDict->count()>0)
  //{
  //  QDictIterator<Define> di(*g_fileDefineDict);
  //  Define *d;
  //  for (di.toFirst();(d=di.current());++di)
  //  {
  //    g_globalDefineDict->remove(di.currentKey());
  //  }
  //  g_fileDefineDict->clear();
  //}

  if (Debug::isFlagSet(Debug::Preprocessor))
  {
    char *orgPos=output.data()+orgOffset;
    char *newPos=output.data()+output.curPos();
    Debug::print(Debug::Preprocessor,0,"Preprocessor output (size: %d bytes):\n",newPos-orgPos);
    int line=1;
    Debug::print(Debug::Preprocessor,0,"---------\n00001 ");
    while (orgPos<newPos) 
    {
      putchar(*orgPos);
      if (*orgPos=='\n') Debug::print(Debug::Preprocessor,0,"%05d ",++line);
      orgPos++;
    }
    Debug::print(Debug::Preprocessor,0,"\n---------\n");
    if (DefineManager::instance().defineContext().count()>0)
    {
      Debug::print(Debug::Preprocessor,0,"Macros accessible in this file:\n");
      Debug::print(Debug::Preprocessor,0,"---------\n");
      QDictIterator<Define> di(DefineManager::instance().defineContext());
      Define *def;
      for (di.toFirst();(def=di.current());++di)
      {
        Debug::print(Debug::Preprocessor,0,"%s ",def->name.data());
      }
      Debug::print(Debug::Preprocessor,0,"\n---------\n");
    }
    else
    {
      Debug::print(Debug::Preprocessor,0,"No macros accessible in this file.\n");
    }
  }
  DefineManager::instance().endContext();
}

void preFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION) 
  if (g_lexInit)
  {
    preYYlex_destroy();
  }
#endif
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
//  int  preYYwrap() { return 1 ; }
  void preYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

