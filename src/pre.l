/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
%option never-interactive
%option prefix="preYY"
%option reentrant
%option extra-type="struct preYY_state *"

%{

/*
 *	includes
 */

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

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
#include "condparser.h"
#include "config.h"
#include "filedef.h"
#include "memberdef.h"
#include "membername.h"

#define YY_NO_UNISTD_H 1

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

static const char *stateToString(int state);

struct CondCtx
{
  CondCtx(int line,QCString id,bool b) 
    : lineNr(line),sectionId(id), skip(b) {}
  int lineNr;
  QCString sectionId;
  bool skip;
};

struct FileState
{
  FileState(int size) : lineNr(1), curlyCount(0),fileBuf(size),
                        oldFileBuf(0), oldFileBufPos(0), bufState(0) {}
  int lineNr;
  int curlyCount;
  BufStr fileBuf;
  BufStr *oldFileBuf;
  int oldFileBufPos;
  YY_BUFFER_STATE bufState;
  QCString fileName;
};  

/** @brief Singleton that manages the defines available while 
 *  preprocessing files. 
 */
class DefineManager
{
  /** Local class used to hold the defines for a single file */
  class DefinesPerFile
  {
    public:
      /** Creates an empty container for defines */
      DefinesPerFile(DefineManager *parent) 
         : m_parent(parent), m_defines(257), m_includedFiles(17)
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
      DefineManager *m_parent;
      DefineDict m_defines;
      QDict<void> m_includedFiles;
  };

  public:
    friend class DefinesPerFile;

    /** Creates a new DefineManager object */
    DefineManager() : m_fileMap(1009), m_contextDefines(1009)
    {
      m_fileMap.setAutoDelete(TRUE);
    }

    /** Destroys the object */
    virtual ~DefineManager() 
    {
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
	dpf = new DefinesPerFile(this);
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
	dpf = new DefinesPerFile(this);
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
	dpf = new DefinesPerFile(this);
	m_fileMap.insert(fileName,dpf);
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
	dpf = new DefinesPerFile(this);
	m_fileMap.insert(fromFileName,dpf);
      }
      dpf->addInclude(toFileName);
    }
    /** Returns a Define object given its name or 0 if the Define does
     *  not exist.
     */
    Define *isDefined(const char *name) const
    {
      Define *d = m_contextDefines.find(name);
      if (d && d->undef) d=0;
      //printf("isDefined(%s)=%p\n",name,d);
      return d;
    }
    /** Returns a reference to the defines found in the current context. */
    const DefineDict &defineContext() const
    {
      return m_contextDefines;
    }

  private:
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

    QDict<DefinesPerFile> m_fileMap;
    DefineDict m_contextDefines;
};


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
      DefinesPerFile *dpf = m_parent->find(incFile);
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

struct preYY_state
{
  preYY_state() :    allIncludes(10009) {}
  int                yyLineNr   = 1;
  int                yyMLines   = 1;
  int                yyColNr   = 1;
  QCString           yyFileName;
  FileDef           *yyFileDef;
  FileDef           *inputFileDef;
  int                ifcount    = 0;
  QStrList          *pathList = 0;
  QStack<FileState>  includeStack;
  QDict<int>        *argDict = 0;
  int                defArgs = -1;
  QCString           defName;
  QCString           defText;
  QCString           defLitText;
  QCString           defArgsStr;
  QCString           defExtraSpacing;
  bool               defVarArgs;
  int                level;
  int                lastCContext;
  int                lastCPPContext;
  QArray<int>        levelGuard;
  BufStr            *inputBuf = 0;
  int                inputBufPos;
  BufStr            *outputBuf = 0;
  int                roundCount;
  bool               quoteArg;
  DefineDict        *expandedDict = 0;
  int                findDefArgContext;
  bool               expectGuard;
  QCString           guardName;
  QCString           lastGuardName;
  QCString           incName;
  QCString           guardExpr;
  int                curlyCount;
  bool               nospaces; // add extra spaces during macro expansion

  bool               macroExpansion; // from the configuration
  bool               expandOnlyPredef; // from the configuration
  int                commentCount;
  bool               insideComment;
  bool               isImported;
  QCString           blockName;
  int                condCtx;
  bool               skip;
  QStack<CondCtx>    condStack;
  bool               insideCS; // C# has simpler preprocessor
  bool               isSource;

  int                fenceSize = 0;
  bool               ccomment;
  QCString           delimiter;
  QDict<void>        allIncludes;
  QDict<void>        expansionDict;
  DefineManager      defineManager;
  ConstExpressionParser constExpParser;
};

// stateless functions
static QCString escapeAt(const char *text);
static QCString extractTrailingComment(const char *s);
static char resolveTrigraph(char c);

// statefull functions
static inline void outputArray(yyscan_t yyscanner,const char *a,int len);
static inline void  outputChar(yyscan_t yyscanner,char c);
static QCString    expandMacro(yyscan_t yyscanner,const QCString &name);
static void    readIncludeFile(yyscan_t yyscanner,const QCString &inc);
static void          incrLevel(yyscan_t yyscanner);
static void          decrLevel(yyscan_t yyscanner);
static void        setCaseDone(yyscan_t yyscanner,bool value);
static bool      otherCaseDone(yyscan_t yyscanner);
static bool  computeExpression(yyscan_t yyscanner,const QCString &expr);
static void   startCondSection(yyscan_t yyscanner,const char *sectId);
static void     endCondSection(yyscan_t yyscanner);
static void          addDefine(yyscan_t yyscanner);
static Define *      newDefine(yyscan_t yyscanner);
static void        setFileName(yyscan_t yyscanner,const char *name);
static int              yyread(yyscan_t yyscanner,char *buf,int max_size);

/* ----------------------------------------------------------------- */

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

/* ----------------------------------------------------------------- */

%}

ID	[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
B       [ \t]
BN	[ \t\r\n]
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))

%option noyywrap

%x      Start
%x	Command
%x	SkipCommand
%x	SkipLine
%x	SkipString
%x	CopyLine
%x	CopyString
%x	CopyStringCs
%x	CopyStringFtn
%x	CopyStringFtnDouble
%x      CopyRawString
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
%x	ArgCopyCComment
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
%x	CondLineC
%x	CondLineCpp
%x      SkipCond

%%

<*>\x06					
<*>\x00
<*>\r
<*>"??"[=/'()!<>-]			{ // Trigraph
  					  unput(resolveTrigraph(yytext[2]));
  					}
<Start>^{B}*"#"				{ BEGIN(Command); yyextra->yyColNr+=yyleng; yyextra->yyMLines=0;}
<Start>^{B}*/[^#]			{
 					  outputArray(yyscanner,yytext,(int)yyleng);
  					  BEGIN(CopyLine); 
					}
<Start>^{B}*[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]+{B}*"("[^\)\n]*")"/{BN}{1,10}*[:{] { // constructors?
					  int i;
					  for (i=(int)yyleng-1;i>=0;i--)
					  {
					    unput(yytext[i]);
					  }
					  BEGIN(CopyLine);
                                        }
<Start>^{B}*[_A-Z][_A-Z0-9]+{B}*"("[^\(\)\n]*"("[^\)\n]*")"[^\)\n]*")"{B}*\n | // function list macro with one (...) argument, e.g. for K_GLOBAL_STATIC_WITH_ARGS
<Start>^{B}*[_A-Z][_A-Z0-9]+{B}*"("[^\)\n]*")"{B}*\n { // function like macro
  					  bool skipFuncMacros = Config_getBool(SKIP_FUNCTION_MACROS);
					  QCString name(yytext);
					  name=name.left(name.find('(')).stripWhiteSpace();

					  Define *def=0;
					  if (skipFuncMacros && 
					      name!="Q_PROPERTY" &&
					      !(
					         (yyextra->includeStack.isEmpty() || yyextra->curlyCount>0) &&
					         yyextra->macroExpansion &&
					         (def=yyextra->defineManager.isDefined(name)) &&
						 /*macroIsAccessible(def) &&*/
					         (!yyextra->expandOnlyPredef || def->isPredefined)
					       )
					     )
					  {
					    outputChar(yyscanner,'\n');
					    yyextra->yyLineNr++;
					  }
					  else // don't skip
					  {
					    int i;
					    for (i=(int)yyleng-1;i>=0;i--)
					    {
					      unput(yytext[i]);
					    }
					    BEGIN(CopyLine);
					  }
  					}
<CopyLine>"extern"{BN}{0,80}"\"C\""*{BN}{0,80}"{"	{
                                          QCString text=yytext;
  					  yyextra->yyLineNr+=text.contains('\n');
					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<CopyLine>{RAWBEGIN}                    {
                                          yyextra->delimiter = yytext+2;
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
					  outputArray(yyscanner,yytext,(int)yyleng);
                                          BEGIN(CopyRawString);
                                        }
<CopyLine>"{"				{ // count brackets inside the main file
  					  if (yyextra->includeStack.isEmpty()) 
					  {
					    yyextra->curlyCount++;
					  }
					  outputChar(yyscanner,*yytext);
  					}
<CopyLine>"}"				{ // count brackets inside the main file
  					  if (yyextra->includeStack.isEmpty() && yyextra->curlyCount>0) 
					  {
					    yyextra->curlyCount--;
					  }
					  outputChar(yyscanner,*yytext);
  					}
<CopyLine>"'"\\[0-7]{1,3}"'"		{ 
  					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyLine>"'"\\."'"			{ 
  					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyLine>"'"."'"			{ 
  					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyLine>@\"				{
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_CSharp) REJECT;
					  outputArray(yyscanner,yytext,(int)yyleng);
					  BEGIN( CopyStringCs );
					}
<CopyLine>\"				{
					  outputChar(yyscanner,*yytext);
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran)
                                          {
					    BEGIN( CopyString );
                                          }
                                          else
                                          {
					    BEGIN( CopyStringFtnDouble );
                                          }
					}
<CopyLine>\'				{
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran) REJECT;
					  outputChar(yyscanner,*yytext);
					  BEGIN( CopyStringFtn );
					}
<CopyString>[^\"\\\r\n]+		{
					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyStringCs>[^\"\r\n]+		{
  					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyString>\\.				{
					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyString,CopyStringCs>\"		{
					  outputChar(yyscanner,*yytext);
					  BEGIN( CopyLine );
					}
<CopyStringFtnDouble>[^\"\\\r\n]+	{
					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyStringFtnDouble>\\.		{
					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyStringFtnDouble>\"			{
					  outputChar(yyscanner,*yytext);
					  BEGIN( CopyLine );
					}
<CopyStringFtn>[^\'\\\r\n]+		{
					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyStringFtn>\\.			{
					  outputArray(yyscanner,yytext,(int)yyleng);
					}
<CopyStringFtn>\'			{
					  outputChar(yyscanner,*yytext);
					  BEGIN( CopyLine );
					}
<CopyRawString>{RAWEND}                 {
					  outputArray(yyscanner,yytext,(int)yyleng);
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==yyextra->delimiter)
                                          {
                                            BEGIN( CopyLine );
                                          }
                                        }
<CopyRawString>[^)]+                    {
					  outputArray(yyscanner,yytext,(int)yyleng);
                                        }
<CopyRawString>.                        {
					  outputChar(yyscanner,*yytext);
                                        }
<CopyLine>{ID}/{BN}{0,80}"("		{
  					  yyextra->expectGuard = FALSE;
  					  Define *def=0;
					  //def=yyextra->globalDefineDict->find(yytext);
					  //def=yyextra->defineManager.isDefined(yytext);
					  //printf("Search for define %s found=%d yyextra->includeStack.isEmpty()=%d "
					  //       "yyextra->curlyCount=%d yyextra->macroExpansion=%d yyextra->expandOnlyPredef=%d "
					  //	 "isPreDefined=%d\n",yytext,def ? 1 : 0,
					  //	 yyextra->includeStack.isEmpty(),yyextra->curlyCount,yyextra->macroExpansion,yyextra->expandOnlyPredef,
					  //	 def ? def->isPredefined : -1
					  //	);
					  if ((yyextra->includeStack.isEmpty() || yyextra->curlyCount>0) &&
					      yyextra->macroExpansion &&
					      (def=yyextra->defineManager.isDefined(yytext)) &&
				              /*(def->isPredefined || macroIsAccessible(def)) && */
					      (!yyextra->expandOnlyPredef || def->isPredefined)
					     )
					  {
					    //printf("Found it! #args=%d\n",def->nargs);
					    yyextra->roundCount=0;
					    yyextra->defArgsStr=yytext;
					    if (def->nargs==-1) // no function macro
					    {
					      QCString result = def->isPredefined ? def->definition : expandMacro(yyscanner,yyextra->defArgsStr);
					      outputArray(yyscanner,result,result.length());
					    }
					    else // zero or more arguments
					    {
					      yyextra->findDefArgContext = CopyLine;
					      BEGIN(FindDefineArgs);
					    }
					  }
					  else
					  {
					    outputArray(yyscanner,yytext,(int)yyleng);
					  }
  					}
<CopyLine>{ID}				{
                                          Define *def=0;
  					  if ((yyextra->includeStack.isEmpty() || yyextra->curlyCount>0) && 
					      yyextra->macroExpansion &&
					      (def=yyextra->defineManager.isDefined(yytext)) &&
					      def->nargs==-1 &&
				              /*(def->isPredefined || macroIsAccessible(def)) &&*/
					      (!yyextra->expandOnlyPredef || def->isPredefined)
					     )
					  {
					    QCString result=def->isPredefined ? def->definition : expandMacro(yyscanner,yytext); 
					    outputArray(yyscanner,result,result.length());
					  }
					  else
					  {
					    outputArray(yyscanner,yytext,(int)yyleng);
					  }
  					}
<CopyLine>"\\"\r?/\n			{ // strip line continuation characters
                                          if (getLanguageFromFileName(yyextra->yyFileName)==SrcLangExt_Fortran) outputChar(yyscanner,*yytext);
  					}
<CopyLine>.				{
  					  outputChar(yyscanner,*yytext);
  					}
<CopyLine>\n				{
  					  outputChar(yyscanner,'\n');
					  BEGIN(Start);
					  yyextra->yyLineNr++;
					  yyextra->yyColNr=1;
  					}
<FindDefineArgs>"("			{
  					  yyextra->defArgsStr+='(';
  					  yyextra->roundCount++;
  					}
<FindDefineArgs>")"			{
  					  yyextra->defArgsStr+=')';
					  yyextra->roundCount--;
					  if (yyextra->roundCount==0)
					  {
					    QCString result=expandMacro(yyscanner,yyextra->defArgsStr);
					    //printf("yyextra->defArgsStr='%s'->'%s'\n",yyextra->defArgsStr.data(),result.data());
					    if (yyextra->findDefArgContext==CopyLine)
					    {
					      outputArray(yyscanner,result,result.length());
					      BEGIN(yyextra->findDefArgContext);
					    }
					    else // yyextra->findDefArgContext==IncludeID
					    {
					      readIncludeFile(yyscanner,result);
					      yyextra->nospaces=FALSE;
					      BEGIN(Start);
					    }
					  }
  					}
  /*
<FindDefineArgs>")"{B}*"("		{
  					  yyextra->defArgsStr+=yytext;
  					}
  */
<FindDefineArgs>{CHARLIT}		{
  					  yyextra->defArgsStr+=yytext;
  					}
<FindDefineArgs>"/*"[*]?                {
                                          yyextra->defArgsStr+=yytext;
                                          BEGIN(ArgCopyCComment);
                                        }
<FindDefineArgs>\"			{
  					  yyextra->defArgsStr+=*yytext;
  					  BEGIN(ReadString);
  					}
<FindDefineArgs>'                       {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(ReadString);
                                        }
<FindDefineArgs>\n			{
                                          yyextra->defArgsStr+=' ';
  					  yyextra->yyLineNr++;
					  outputChar(yyscanner,'\n');
  					}
<FindDefineArgs>"@"			{
  					  yyextra->defArgsStr+="@@";
  					}
<FindDefineArgs>.			{
  					  yyextra->defArgsStr+=*yytext;
  					}
<ArgCopyCComment>[^*\n]+		{
					  yyextra->defArgsStr+=yytext;
  					}
<ArgCopyCComment>"*/"			{
					  yyextra->defArgsStr+=yytext;
  					  BEGIN(FindDefineArgs);
  					}
<ArgCopyCComment>\n			{ 
                                          yyextra->defArgsStr+=' ';
  					  yyextra->yyLineNr++;
					  outputChar(yyscanner,'\n');
  					}
<ArgCopyCComment>.			{ 
                                          yyextra->defArgsStr+=yytext;
                                        }
<ReadString>"\""			{
  					  yyextra->defArgsStr+=*yytext;
					  BEGIN(FindDefineArgs);
  					}
<ReadString>"'"                         {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(FindDefineArgs);
                                        }

<ReadString>"//"|"/*"			{
  					  yyextra->defArgsStr+=yytext;
  					}
<ReadString>\\/\r?\n			{ // line continuation
					}
<ReadString>\\.				{
  					  yyextra->defArgsStr+=yytext;
  					}
<ReadString>.				{
  					  yyextra->defArgsStr+=*yytext;
  					}
<Command>("include"|"import"){B}+/{ID}	{
  					  yyextra->isImported = yytext[1]=='m';
  					  if (yyextra->macroExpansion) 
					    BEGIN(IncludeID);
  					}
<Command>("include"|"import"){B}*[<"]	{ 
  					  yyextra->isImported = yytext[1]=='m';
					  char c[2];
					  c[0]=yytext[yyleng-1];c[1]='\0';
					  yyextra->incName=c;
  					  BEGIN(Include); 
					}
<Command>("cmake")?"define"{B}+		{ 
  			                  //printf("!!!DefName\n"); 
					  yyextra->yyColNr+=yyleng;
  					  BEGIN(DefName); 
					}
<Command>"ifdef"/{B}*"("		{
  					  incrLevel(yyscanner);
					  yyextra->guardExpr.resize(0);
  					  BEGIN(DefinedExpr2);
  					}
<Command>"ifdef"/{B}+			{
  					  //printf("Pre.l: ifdef\n");
  					  incrLevel(yyscanner);
					  yyextra->guardExpr.resize(0);
  					  BEGIN(DefinedExpr1);
  					}
<Command>"ifndef"/{B}*"("		{
  					  incrLevel(yyscanner);
					  yyextra->guardExpr="! ";
  					  BEGIN(DefinedExpr2);
					}
<Command>"ifndef"/{B}+			{
  					  incrLevel(yyscanner);
					  yyextra->guardExpr="! ";
  					  BEGIN(DefinedExpr1);
  					}
<Command>"if"/[ \t(!]			{
  					  incrLevel(yyscanner);
					  yyextra->guardExpr.resize(0);
					  BEGIN(Guard);
					}
<Command>("elif"|"else"{B}*"if")/[ \t(!]	{
  					  if (!otherCaseDone(yyscanner))
					  {
					    yyextra->guardExpr.resize(0);
					    BEGIN(Guard);  
					  }
					  else
					  {
					    yyextra->ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<Command>"else"/[^a-z_A-Z0-9\x80-\xFF]		{
					  //printf("else yyextra->levelGuard[%d]=%d\n",yyextra->level-1,yyextra->levelGuard[yyextra->level-1]);
  					  if (otherCaseDone(yyscanner))
					  {
					    yyextra->ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
					  else
					  {
					    setCaseDone(yyscanner,TRUE);
					    //yyextra->levelGuard[yyextra->level-1]=TRUE;
					  } 
  					}
<Command>"undef"{B}+			{
  					  BEGIN(UndefName);
  					}
<Command>("elif"|"else"{B}*"if")/[ \t(!]	{
  					  if (!otherCaseDone(yyscanner))
					  {
					    yyextra->guardExpr.resize(0);
  					    BEGIN(Guard);
					  }
  					}
<Command>"endif"/[^a-z_A-Z0-9\x80-\xFF]		{
  					  //printf("Pre.l: #endif\n");
  					  decrLevel(yyscanner);
  					}
<Command,IgnoreLine>\n			{
  					  outputChar(yyscanner,'\n');
  					  BEGIN(Start);
					  yyextra->yyLineNr++;
  					}
<Command>"pragma"{B}+"once"             {
                                          yyextra->expectGuard = FALSE;
                                        }
<Command>{ID}				{ // unknown directive
					  BEGIN(IgnoreLine);
					}
<IgnoreLine>\\[\r]?\n			{
  					  outputChar(yyscanner,'\n');
					  yyextra->yyLineNr++;
					}
<IgnoreLine>.
<Command>. {yyextra->yyColNr+=yyleng;}
<UndefName>{ID}				{
  					  Define *def;
  					  if ((def=yyextra->defineManager.isDefined(yytext)) 
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
  					  outputChar(yyscanner,'\n');
  					  yyextra->guardExpr+=' ';
					  yyextra->yyLineNr++;
  					}
<Guard>"defined"/{B}*"("		{
    					  BEGIN(DefinedExpr2);
    					}
<Guard>"defined"/{B}+			{
    					  BEGIN(DefinedExpr1);
    					}
<Guard>{ID}				{ yyextra->guardExpr+=yytext; }
<Guard>"@"                              { yyextra->guardExpr+="@@"; }
<Guard>.				{ yyextra->guardExpr+=*yytext; }
<Guard>\n				{
  					  unput(*yytext);
  					  //printf("Guard: '%s'\n",
					  //    yyextra->guardExpr.data());
					  bool guard=computeExpression(yyscanner,yyextra->guardExpr);
					  setCaseDone(yyscanner,guard);
					  //printf("if yyextra->levelGuard[%d]=%d\n",yyextra->level-1,yyextra->levelGuard[yyextra->level-1]);
					  if (guard)
					  {
					    BEGIN(Start);
					  } 
					  else
					  {
					    yyextra->ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<DefinedExpr1,DefinedExpr2>\\\n		{ yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<DefinedExpr1>{ID}			{
  					  if (yyextra->defineManager.isDefined(yytext) || yyextra->guardName==yytext)
					    yyextra->guardExpr+=" 1L ";
					  else
					    yyextra->guardExpr+=" 0L ";
					  yyextra->lastGuardName=yytext;
					  BEGIN(Guard);
  					}
<DefinedExpr2>{ID}			{
  					  if (yyextra->defineManager.isDefined(yytext) || yyextra->guardName==yytext)
					    yyextra->guardExpr+=" 1L ";
					  else
					    yyextra->guardExpr+=" 0L ";
					  yyextra->lastGuardName=yytext;
  					}
<DefinedExpr1,DefinedExpr2>\n		{ // should not happen, handle anyway
                                          yyextra->yyLineNr++;
  					  yyextra->ifcount=0;
 					  BEGIN(SkipCPPBlock); 
					}
<DefinedExpr2>")"			{
  					  BEGIN(Guard);
  					}
<DefinedExpr1,DefinedExpr2>.
<SkipCPPBlock>^{B}*"#"			{ BEGIN(SkipCommand); }
<SkipCPPBlock>^{B}*/[^#]		{ BEGIN(SkipLine); }
<SkipCPPBlock>\n			{ yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<SkipCPPBlock>.
<SkipCommand>"if"(("n")?("def"))?/[ \t(!]	{ 
  					  incrLevel(yyscanner);
                                          yyextra->ifcount++; 
  					  //printf("#if... depth=%d\n",yyextra->ifcount);
					}
<SkipCommand>"else"			{
					  //printf("Else! yyextra->ifcount=%d otherCaseDone=%d\n",yyextra->ifcount,otherCaseDone());
  					  if (yyextra->ifcount==0 && !otherCaseDone(yyscanner))
					  {
					    setCaseDone(yyscanner,TRUE);
  					    //outputChar(yyscanner,'\n');
					    BEGIN(Start);
					  }
  					}
<SkipCommand>("elif"|"else"{B}*"if")/[ \t(!]		{
  					  if (yyextra->ifcount==0) 
					  {
  					    if (!otherCaseDone(yyscanner))
					    {
					      yyextra->guardExpr.resize(0);
					      yyextra->lastGuardName.resize(0);
  					      BEGIN(Guard);
					    }
					    else
					    {
					      BEGIN(SkipCPPBlock);
					    }
					  }
					}
<SkipCommand>"endif"			{ 
					  yyextra->expectGuard = FALSE;
  					  decrLevel(yyscanner);
  				          if (--yyextra->ifcount<0)
  					  {
  					    //outputChar(yyscanner,'\n');
					    BEGIN(Start);
					  }
					}
<SkipCommand>\n				{ 
  					  outputChar(yyscanner,'\n');
  					  yyextra->yyLineNr++; 
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
  					  yyextra->lastCPPContext=YY_START;
  					  BEGIN(RemoveCPPComment);
					}
<SkipString>"/*"/[^\n]*                 { 
                                        }
<SkipLine,SkipCommand,SkipCPPBlock>"/*"/[^\n]* {
					  yyextra->lastCContext=YY_START;
  					  BEGIN(RemoveCComment);
  					}
<SkipLine>\n				{
  					  outputChar(yyscanner,'\n');
					  yyextra->yyLineNr++;  
					  BEGIN(SkipCPPBlock);
					}
<SkipString>[^"\\\n]+			{ }
<SkipString>\\.				{ }
<SkipString>\"				{
  					  BEGIN(SkipLine);
  					}
<SkipString>.				{ }
<IncludeID>{ID}{B}*/"("			{
  					  yyextra->nospaces=TRUE;
				          yyextra->roundCount=0;
					  yyextra->defArgsStr=yytext;
					  yyextra->findDefArgContext = IncludeID;
					  BEGIN(FindDefineArgs);
					}
<IncludeID>{ID}				{
  					  yyextra->nospaces=TRUE;
                                          readIncludeFile(yyscanner,expandMacro(yyscanner,yytext));
					  BEGIN(Start);
  					}
<Include>[^\">\n]+[\">]			{ 
					  yyextra->incName+=yytext;
					  readIncludeFile(yyscanner,yyextra->incName);
					  if (yyextra->isImported)
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
					  outputChar(yyscanner,'\n');
					  yyextra->yyLineNr++;
					}
<EndImport>.				{
  					}
<DefName>{ID}/("\\\n")*"("		{ // define with argument
  					  //printf("Define() '%s'\n",yytext);
					  delete yyextra->argDict;
					  yyextra->argDict = new QDict<int>(31);
					  yyextra->argDict->setAutoDelete(TRUE);
					  yyextra->defArgs = 0; 
                                          yyextra->defArgsStr.resize(0);
					  yyextra->defText.resize(0);
					  yyextra->defLitText.resize(0);
					  yyextra->defName = yytext;
					  yyextra->defVarArgs = FALSE;
					  yyextra->defExtraSpacing.resize(0);
					  BEGIN(DefineArg);
  					}
<DefName>{ID}{B}+"1"/[ \r\t\n]		{ // special case: define with 1 -> can be "guard"
  					  //printf("Define '%s'\n",yytext);
					  delete yyextra->argDict; yyextra->argDict=0;
					  yyextra->defArgs = -1;
                                          yyextra->defArgsStr.resize(0);
					  yyextra->defName = yytext;
					  yyextra->defName = yyextra->defName.left(yyextra->defName.length()-1).stripWhiteSpace();
					  yyextra->defVarArgs = FALSE;
					  //printf("Guard check: %s!=%s || %d\n",
					  //    yyextra->defName.data(),yyextra->lastGuardName.data(),yyextra->expectGuard);
					  if (yyextra->curlyCount>0 || yyextra->defName!=yyextra->lastGuardName || !yyextra->expectGuard)
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+yyextra->defName;
					    outputArray(yyscanner,tmp.data(),tmp.length());
					    yyextra->quoteArg=FALSE;
					    yyextra->insideComment=FALSE;
					    yyextra->lastGuardName.resize(0);
				            yyextra->defText="1"; 
					    yyextra->defLitText="1"; 
					    BEGIN(DefineText); 
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
					    yyextra->defText.resize(0);
					    yyextra->defLitText.resize(0);
					    BEGIN(Start);
					  }
					  yyextra->expectGuard=FALSE;
  					}
<DefName>{ID}/{B}*"\n"			{ // empty define
					  delete yyextra->argDict; yyextra->argDict=0;
					  yyextra->defArgs = -1;
					  yyextra->defName = yytext;
                                          yyextra->defArgsStr.resize(0);
					  yyextra->defText.resize(0);
					  yyextra->defLitText.resize(0);
					  yyextra->defVarArgs = FALSE;
					  //printf("Guard check: %s!=%s || %d\n",
					  //    yyextra->defName.data(),yyextra->lastGuardName.data(),yyextra->expectGuard);
					  if (yyextra->curlyCount>0 || yyextra->defName!=yyextra->lastGuardName || !yyextra->expectGuard)
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+yyextra->defName;
					    outputArray(yyscanner,tmp.data(),tmp.length());
					    yyextra->quoteArg=FALSE;
					    yyextra->insideComment=FALSE;
					    if (yyextra->insideCS) yyextra->defText="1"; // for C#, use "1" as define text
					    BEGIN(DefineText);
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
					    yyextra->guardName = yytext;
					    yyextra->lastGuardName.resize(0);
					    BEGIN(Start);
					  }
					  yyextra->expectGuard=FALSE;
  					}
<DefName>{ID}/{B}*			{ // define with content
  					  //printf("Define '%s'\n",yytext);
					  delete yyextra->argDict; yyextra->argDict=0;
					  yyextra->defArgs = -1;
                                          yyextra->defArgsStr.resize(0);
					  yyextra->defText.resize(0);
					  yyextra->defLitText.resize(0);
					  yyextra->defName = yytext;
					  yyextra->defVarArgs = FALSE;
					  QCString tmp=(QCString)"#define "+yyextra->defName+yyextra->defArgsStr;
					  outputArray(yyscanner,tmp.data(),tmp.length());
					  yyextra->quoteArg=FALSE;
					  yyextra->insideComment=FALSE;
					  BEGIN(DefineText); 
  					}
<DefineArg>"\\\n"                       {
  					  yyextra->defExtraSpacing+="\n";
					  yyextra->yyLineNr++;
                                        }
<DefineArg>","{B}*			{ yyextra->defArgsStr+=yytext; }
<DefineArg>"("{B}*                      { yyextra->defArgsStr+=yytext; }
<DefineArg>{B}*")"{B}*			{
                                          yyextra->defArgsStr+=yytext; 
					  QCString tmp=(QCString)"#define "+yyextra->defName+yyextra->defArgsStr+yyextra->defExtraSpacing;
					  outputArray(yyscanner,tmp.data(),tmp.length());
					  yyextra->quoteArg=FALSE;
					  yyextra->insideComment=FALSE;
  					  BEGIN(DefineText);
  					}
<DefineArg>"..."			{ // Variadic macro
					  yyextra->defVarArgs = TRUE;
					  yyextra->defArgsStr+=yytext;
					  yyextra->argDict->insert("__VA_ARGS__",new int(yyextra->defArgs));
					  yyextra->defArgs++;
  					}
<DefineArg>{ID}{B}*("..."?)		{
  					  //printf("Define addArg(%s)\n",yytext);
  					  QCString argName=yytext;
  					  yyextra->defVarArgs = yytext[yyleng-1]=='.';
					  if (yyextra->defVarArgs) // strip ellipsis
					  {
					    argName=argName.left(argName.length()-3);
					  }
					  argName = argName.stripWhiteSpace();
                                          yyextra->defArgsStr+=yytext;
					  yyextra->argDict->insert(argName,new int(yyextra->defArgs)); 
					  yyextra->defArgs++;
  					}
  /*
<DefineText>"/ **"|"/ *!"			{
  					  yyextra->defText+=yytext;
					  yyextra->defLitText+=yytext;
					  yyextra->insideComment=TRUE;
  					}
<DefineText>"* /"			{
  					  yyextra->defText+=yytext;
					  yyextra->defLitText+=yytext;
					  yyextra->insideComment=FALSE;
  					}
  */
<DefineText>"/*"[!*]?			{
					  yyextra->defText+=yytext;
					  yyextra->defLitText+=yytext;
					  yyextra->lastCContext=YY_START;
					  yyextra->commentCount=1;
  					  BEGIN(CopyCComment);
  					}
<DefineText>"//"[!/]?			{
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					  yyextra->lastCPPContext=YY_START;
					  yyextra->defLitText+=' ';
  					  BEGIN(SkipCPPComment);
  					}
<SkipCComment>[/]?"*/"			{
  					  if (yytext[0]=='/') outputChar(yyscanner,'/');
  					  outputChar(yyscanner,'*');outputChar(yyscanner,'/');
					  if (--yyextra->commentCount<=0)
					  {
					    if (yyextra->lastCContext==Start) 
					      // small hack to make sure that ^... rule will
					      // match when going to Start... Example: "/*...*/ some stuff..."
					    {
					      YY_CURRENT_BUFFER->yy_at_bol=1;
					    }
  					    BEGIN(yyextra->lastCContext);  
					  }
  					}
<SkipCComment>"//"("/")*		{
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<SkipCComment>"/*"			{
  					  outputChar(yyscanner,'/');outputChar(yyscanner,'*');
					  //yyextra->commentCount++;
  					}
<SkipCComment>[\\@][\\@]("f{"|"f$"|"f[") {
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<SkipCComment>^({B}*"*"+)?{B}{0,3}"~~~"[~]*   {
                                          bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
  					    outputArray(yyscanner,yytext,(int)yyleng);
                                            yyextra->fenceSize=yyleng;
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>^({B}*"*"+)?{B}{0,3}"```"[`]*            {
                                          bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
  					    outputArray(yyscanner,yytext,(int)yyleng);
                                            yyextra->fenceSize=yyleng;
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>[\\@][\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"code"("{"[^}]*"}")?){BN}+ {
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					  yyextra->yyLineNr+=QCString(yytext).contains('\n');
  					}
<SkipCComment>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"code"("{"[^}]*"}")?){BN}+	{
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					  yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                          yyextra->fenceSize=0;
					  if (yytext[1]=='f')
					  {
					    yyextra->blockName="f";
					  }
					  else
					  {
                                            QCString bn=&yytext[1];
                                            int i = bn.find('{'); // for \code{.c}
                                            if (i!=-1) bn=bn.left(i);
					    yyextra->blockName=bn.stripWhiteSpace();
					  }
					  BEGIN(SkipVerbatim);
  					}
<SkipCComment,SkipCPPComment>[\\@][\\@]"cond"[ \t]+ { // escaped @cond
  					  outputArray(yyscanner,yytext,(int)yyleng);
                                        }
<SkipCPPComment>[\\@]"cond"[ \t]+	{ // conditional section
                                          yyextra->ccomment=TRUE;  
                                          yyextra->condCtx=YY_START;
  					  BEGIN(CondLineCpp);
  					}
<SkipCComment>[\\@]"cond"[ \t]+	{ // conditional section
                                          yyextra->ccomment=FALSE;  
                                          yyextra->condCtx=YY_START;
  					  BEGIN(CondLineC);
  					}
<CondLineC,CondLineCpp>[!()&| \ta-z_A-Z0-9\x80-\xFF.\-]+      {
  				          startCondSection(yyscanner,yytext);
                                          if (yyextra->skip)
                                          {
                                            if (YY_START==CondLineC)
                                            {
                                              // end C comment
  					      outputArray(yyscanner,"*/",2);
                                              yyextra->ccomment=TRUE;
                                            }
                                            else
                                            {
                                              yyextra->ccomment=FALSE;
                                            }
                                            BEGIN(SkipCond);
                                          }
                                          else
                                          {
  					    BEGIN(yyextra->condCtx);
                                          }
  					}
<CondLineC,CondLineCpp>.		{ // non-guard character
  					  unput(*yytext);
  					  startCondSection(yyscanner," ");
                                          if (yyextra->skip)
                                          {
                                            if (YY_START==CondLineC)
                                            {
                                              // end C comment
  					      outputArray(yyscanner,"*/",2);
                                              yyextra->ccomment=TRUE;
                                            }
                                            else
                                            {
                                              yyextra->ccomment=FALSE;
                                            }
                                            BEGIN(SkipCond);
                                          }
                                          else
                                          {
					    BEGIN(yyextra->condCtx);
                                          }
  					}
<SkipCComment,SkipCPPComment>[\\@]"cond"[ \t\r]*/\n { // no guard
                                          if (YY_START==SkipCComment)
                                          {
                                            yyextra->ccomment=TRUE;
                                            // end C comment
  					    outputArray(yyscanner,"*/",2);
                                          }
                                          else
                                          {
                                            yyextra->ccomment=FALSE;
                                          }
                                          yyextra->condCtx=YY_START;
                                          startCondSection(yyscanner," ");
                                          BEGIN(SkipCond);
  					}
<SkipCond>\n                            { yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<SkipCond>.                             { }
<SkipCond>[^\/\!*\\@\n]+                { }
<SkipCond>"//"[/!]                      { yyextra->ccomment=FALSE; }
<SkipCond>"/*"[*!]                      { yyextra->ccomment=TRUE; }
<SkipCond,SkipCComment,SkipCPPComment>[\\@][\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          if (!yyextra->skip)
                                          {
  					    outputArray(yyscanner,yytext,(int)yyleng);
                                          }
                                        }
<SkipCond>[\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF]  { 
                                          bool oldSkip = yyextra->skip;
                                          endCondSection(yyscanner);
                                          if (oldSkip && !yyextra->skip)
                                          {
                                            if (yyextra->ccomment)
                                            {
                                              outputArray(yyscanner,"/** ",4);
                                            }
                                            BEGIN(yyextra->condCtx);
                                          }
                                        }
<SkipCComment,SkipCPPComment>[\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          bool oldSkip = yyextra->skip;
  					  endCondSection(yyscanner);
                                          if (oldSkip && !yyextra->skip) 
                                          {
                                            BEGIN(yyextra->condCtx);
                                          }
  					}
<SkipVerbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"f$"|"f]"|"f}") { /* end of verbatim block */
  					  outputArray(yyscanner,yytext,(int)yyleng);
					  if (yytext[1]=='f' && yyextra->blockName=="f")
					  {
					    BEGIN(SkipCComment);
					  }
					  else if (&yytext[4]==yyextra->blockName)
					  {
					    BEGIN(SkipCComment);
					  }
  					}
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"~~~"[~]*                 {
  					  outputArray(yyscanner,yytext,(int)yyleng);
                                          if (yyextra->fenceSize==yyleng)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
  					  outputArray(yyscanner,yytext,(int)yyleng);
                                          if (yyextra->fenceSize==yyleng)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>"*/"|"/*"			{
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<SkipCComment,SkipVerbatim>[^*\\@\x06~`\n\/]+ {
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<SkipCComment,SkipVerbatim>\n		{ 
  					  yyextra->yyLineNr++;
  					  outputChar(yyscanner,'\n');
  					}
<SkipCComment,SkipVerbatim>.		{
  					  outputChar(yyscanner,*yytext);
  					}
<CopyCComment>[^*a-z_A-Z\x80-\xFF\n]*[^*a-z_A-Z\x80-\xFF\\\n] {
					  yyextra->defLitText+=yytext;
					  yyextra->defText+=escapeAt(yytext);
					}
<CopyCComment>\\[\r]?\n                 {
                                          yyextra->defLitText+=yytext;
                                          outputChar(yyscanner,'\n');
                                          yyextra->defText+=" ";
                                          yyextra->yyLineNr++;
                                          yyextra->yyMLines++;
                                        }
<CopyCComment>"*/"			{
					  yyextra->defLitText+=yytext;
					  yyextra->defText+=yytext;
  					  BEGIN(yyextra->lastCContext);
  					}
<CopyCComment>\n			{ 
  					  yyextra->yyLineNr++;
  					  outputChar(yyscanner,'\n');
					  yyextra->defLitText+=yytext;
					  yyextra->defText+=' ';
  					}
<RemoveCComment>"*/"{B}*"#"	        { // see bug 594021 for a usecase for this rule
                                          if (yyextra->lastCContext==SkipCPPBlock)
					  {
					    BEGIN(SkipCommand);
					  }
					  else
					  {
					    REJECT;
					  }
					}
<RemoveCComment>"*/"		        { BEGIN(yyextra->lastCContext); }
<RemoveCComment>"//"			
<RemoveCComment>"/*"
<RemoveCComment>[^*\x06\n]+
<RemoveCComment>\n			{ yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<RemoveCComment>.			
<SkipCPPComment>[^\n\/\\@]+		{
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<SkipCPPComment,RemoveCPPComment>\n	{
  					  unput(*yytext);
  					  BEGIN(yyextra->lastCPPContext);
  					}
<SkipCPPComment>"/*"			{
  					  outputChar(yyscanner,'/');outputChar(yyscanner,'*');
  					}
<SkipCPPComment>"//"			{
  					  outputChar(yyscanner,'/');outputChar(yyscanner,'/');
  					}
<SkipCPPComment>[^\x06\@\\\n]+		{
  					  outputArray(yyscanner,yytext,(int)yyleng);
  					}
<SkipCPPComment>.			{
  					  outputChar(yyscanner,*yytext);
  					}
<RemoveCPPComment>"/*"
<RemoveCPPComment>"//"
<RemoveCPPComment>[^\x06\n]+
<RemoveCPPComment>.
<DefineText>"#"				{
  					  yyextra->quoteArg=TRUE;
					  yyextra->defLitText+=yytext;
  					}
<DefineText,CopyCComment>{ID}		{
					  yyextra->defLitText+=yytext;
  					  if (yyextra->quoteArg)
					  {
					    yyextra->defText+="\"";
					  }
					  if (yyextra->defArgs>0)
					  {
					    int *n;
					    if ((n=(*yyextra->argDict)[yytext]))
					    {
					      //if (!yyextra->quoteArg) yyextra->defText+=' ';
					      yyextra->defText+='@';
					      QCString numStr;
					      numStr.sprintf("%d",*n);
					      yyextra->defText+=numStr;
					      //if (!yyextra->quoteArg) yyextra->defText+=' ';
					    }
					    else
					    {
					      yyextra->defText+=yytext;
					    }
					  }
					  else
					  {
					    yyextra->defText+=yytext;
					  }
					  if (yyextra->quoteArg)
					  {
					    yyextra->defText+="\"";
					  }
					  yyextra->quoteArg=FALSE;
  					}
<CopyCComment>.				{
					  yyextra->defLitText+=yytext;
					  yyextra->defText+=yytext;
  					}
<DefineText>\\[\r]?\n			{
					  yyextra->defLitText+=yytext;
					  outputChar(yyscanner,'\n');
					  yyextra->defText += ' ';
					  yyextra->yyLineNr++;
					  yyextra->yyMLines++;
					}
<DefineText>\n				{
					  QCString comment=extractTrailingComment(yyextra->defLitText);
					  yyextra->defLitText+=yytext;
					  if (!comment.isEmpty())
					  {
					    outputArray(yyscanner,comment,comment.length());
					    yyextra->defLitText=yyextra->defLitText.left(yyextra->defLitText.length()-comment.length()-1);
					  }
  					  outputChar(yyscanner,'\n');
  					  Define *def=0;
					  //printf("Define name='%s' text='%s' litTexti='%s'\n",yyextra->defName.data(),yyextra->defText.data(),yyextra->defLitText.data());
					  if (yyextra->includeStack.isEmpty() || yyextra->curlyCount>0) 
					  {
					    addDefine(yyscanner);
					  }
					  def=yyextra->defineManager.isDefined(yyextra->defName);
					  if (def==0) // new define
					  {
					    //printf("new define '%s'!\n",yyextra->defName.data());
					    Define *nd = newDefine(yyscanner);
					    yyextra->defineManager.addDefine(yyextra->yyFileName,nd);

					    // also add it to the local file list if it is a source file
					    //if (yyextra->isSource && yyextra->includeStack.isEmpty())
					    //{
					    //  yyextra->fileDefineDict->insert(yyextra->defName,nd);
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
					      def->name = yyextra->defName;
					      def->definition = yyextra->defText.stripWhiteSpace();
					      def->nargs = yyextra->defArgs;
					      def->fileName = yyextra->yyFileName.copy(); 
					      def->lineNr = yyextra->yyLineNr-yyextra->yyMLines;
					      def->columnNr = yyextra->yyColNr;
					    }
					    else
					    {
					      //printf("error: define %s is defined more than once!\n",yyextra->defName.data());
					    }
					  }
					  delete yyextra->argDict; yyextra->argDict=0;
					  yyextra->yyLineNr++;
					  yyextra->yyColNr=1;
					  yyextra->lastGuardName.resize(0);
					  BEGIN(Start);
  					}
<DefineText>{B}*			{ yyextra->defText += ' '; yyextra->defLitText+=yytext; }
<DefineText>{B}*"##"{B}*		{ yyextra->defText += "##"; yyextra->defLitText+=yytext; }
<DefineText>"@"				{ yyextra->defText += "@@"; yyextra->defLitText+=yytext; }
<DefineText>\"				{ 
                                          yyextra->defText += *yytext; 
  					  yyextra->defLitText+=yytext; 
					  if (!yyextra->insideComment)
					  {
					    BEGIN(SkipDoubleQuote);
					  }
  					}
<DefineText>\'				{ yyextra->defText += *yytext;
  					  yyextra->defLitText+=yytext; 
					  if (!yyextra->insideComment)
					  {
  					    BEGIN(SkipSingleQuote);
					  }
					}
<SkipDoubleQuote>"//"[/]?		{ yyextra->defText += yytext; yyextra->defLitText+=yytext; }
<SkipDoubleQuote>"/*"			{ yyextra->defText += yytext; yyextra->defLitText+=yytext; }
<SkipDoubleQuote>\"			{
  					  yyextra->defText += *yytext; yyextra->defLitText+=yytext; 
					  BEGIN(DefineText);
  					}
<SkipSingleQuote,SkipDoubleQuote>\\.	{
  					  yyextra->defText += yytext; yyextra->defLitText+=yytext;
					}
<SkipSingleQuote>\'			{
  					  yyextra->defText += *yytext; yyextra->defLitText+=yytext;
					  BEGIN(DefineText);
  					}
<SkipDoubleQuote>.			{ yyextra->defText += *yytext; yyextra->defLitText+=yytext; }
<SkipSingleQuote>.			{ yyextra->defText += *yytext; yyextra->defLitText+=yytext; }
<DefineText>.				{ yyextra->defText += *yytext; yyextra->defLitText+=yytext; }
<<EOF>>					{
                                          DBG_CTX((stderr,"End of include file\n"));
					  //printf("Include stack depth=%d\n",yyextra->includeStack.count());
  					  if (yyextra->includeStack.isEmpty())
					  {
					    DBG_CTX((stderr,"Terminating scanner!\n"));
					    yyterminate();
					  }
					  else
					  {
					    FileState *fs=yyextra->includeStack.pop();
					    //fileDefineCache->merge(yyextra->yyFileName,fs->fileName);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->bufState, yyscanner );
					    yy_delete_buffer( oldBuf, yyscanner );
					    yyextra->yyLineNr    = fs->lineNr;
                                            //preYYin = fs->oldYYin;
                                            yyextra->inputBuf    = fs->oldFileBuf;
					    yyextra->inputBufPos = fs->oldFileBufPos;
                                            yyextra->curlyCount = fs->curlyCount;
					    setFileName(yyscanner,fs->fileName);
					    DBG_CTX((stderr,"######## FileName %s\n",yyextra->yyFileName.data()));
					    
                                            // Deal with file changes due to 
                                            // #include's within { .. } blocks
                                            QCString lineStr(15+yyextra->yyFileName.length());
                                            lineStr.sprintf("# %d \"%s\" 2",yyextra->yyLineNr,yyextra->yyFileName.data());
                                            outputArray(yyscanner,lineStr.data(),lineStr.length());
					    
					    delete fs; fs=0;
					  }
  					}
<*>"/*"/"*/"				|
<*>"/*"[*]?				{
                                          if (YY_START==SkipVerbatim || YY_START==SkipCond)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
					    outputArray(yyscanner,yytext,(int)yyleng);
  					    yyextra->lastCContext=YY_START;
					    yyextra->commentCount=1;
					    if (yyleng==3) yyextra->lastGuardName.resize(0); // reset guard in case the #define is documented!
					    BEGIN(SkipCComment);
                                          }
  					}
<*>"//"[/]?				{
                                          if (YY_START==SkipVerbatim || YY_START==SkipCond || getLanguageFromFileName(yyextra->yyFileName)==SrcLangExt_Fortran)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
					    outputArray(yyscanner,yytext,(int)yyleng);
  					    yyextra->lastCPPContext=YY_START;
					    if (yyleng==3) yyextra->lastGuardName.resize(0); // reset guard in case the #define is documented!
					    BEGIN(SkipCPPComment);
                                          }
					}
<*>\n					{ 
  					  outputChar(yyscanner,'\n');
  					  yyextra->yyLineNr++; 
					}
<*>.				        {
  					  yyextra->expectGuard = FALSE;
  					  outputChar(yyscanner,*yytext);
  					}

%%

/////////////////////////////////////////////////////////////////////////////////////

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  int bytesInBuf = state->inputBuf->curPos()-state->inputBufPos;
  int bytesToCopy = QMIN(max_size,bytesInBuf);
  memcpy(buf,state->inputBuf->data()+state->inputBufPos,bytesToCopy);
  state->inputBufPos+=bytesToCopy;
  return bytesToCopy;
}

static void setFileName(yyscan_t yyscanner,const char *name)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  bool ambig;
  QFileInfo fi(name);
  state->yyFileName=fi.absFilePath().utf8();
  state->yyFileDef=findFileDef(Doxygen::inputNameDict,state->yyFileName,ambig);
  if (state->yyFileDef==0) // if this is not an input file check if it is an
                      // include file
  {
    state->yyFileDef=findFileDef(Doxygen::includeNameDict,state->yyFileName,ambig);
  }
  //printf("setFileName(%s) state->yyFileName=%s state->yyFileDef=%p\n",
  //    name,state->yyFileName.data(),state->yyFileDef);
  if (state->yyFileDef && state->yyFileDef->isReference()) state->yyFileDef=0;
  state->insideCS = getLanguageFromFileName(state->yyFileName)==SrcLangExt_CSharp;
  state->isSource = guessSection(state->yyFileName);
}

static void incrLevel(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  state->level++;
  state->levelGuard.resize(state->level);
  state->levelGuard[state->level-1]=FALSE;
  //printf("%s line %d: incrLevel %d\n",yyextra->yyFileName.data(),yyextra->yyLineNr,yyextra->level);
}

static void decrLevel(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("%s line %d: decrLevel %d\n",state->yyFileName.data(),state->yyLineNr,state->level);
  if (state->level > 0)
  {
    state->level--;
    state->levelGuard.resize(state->level);
  }
  else
  {
    warn(state->yyFileName,state->yyLineNr,"More #endif's than #if's found.\n");
  }
}

static bool otherCaseDone(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->level==0)
  {
    warn(state->yyFileName,state->yyLineNr,"Found an #else without a preceding #if.\n");
    return TRUE;
  }
  else
  {
    return state->levelGuard[state->level-1];
  }
}

static void setCaseDone(yyscan_t yyscanner,bool value)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  state->levelGuard[state->level-1]=value;
}


static FileState *checkAndOpenFile(yyscan_t yyscanner,const QCString &fileName,bool &alreadyIncluded)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  alreadyIncluded = FALSE;
  FileState *fs = 0;
  //printf("checkAndOpenFile(%s)\n",fileName.data());
  QFileInfo fi(fileName);
  if (fi.exists() && fi.isFile())
  {
    const QStrList &exclPatterns = Config_getList(EXCLUDE_PATTERNS);
    if (patternMatch(fi,&exclPatterns)) return 0;

    QCString absName = fi.absFilePath().utf8();

    // global guard
    if (state->curlyCount==0) // not #include inside { ... }
    {
      if (state->allIncludes.find(absName)!=0)
      {
        alreadyIncluded = TRUE;
        //printf("  already included 1\n");
        return 0; // already done
      }
      state->allIncludes.insert(absName,(void *)0x8);
    }
    // check include stack for absName

    QStack<FileState> tmpStack;
    state->includeStack.setAutoDelete(FALSE);
    while ((fs=state->includeStack.pop()))
    {
      if (fs->fileName==absName) alreadyIncluded=TRUE;
      tmpStack.push(fs);
    }
    while ((fs=tmpStack.pop()))
    {
      state->includeStack.push(fs);
    }
    state->includeStack.setAutoDelete(TRUE);

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
      fs->oldFileBuf    = state->inputBuf;
      fs->oldFileBufPos = state->inputBufPos;
    }
  }
  return fs;
}

static FileState *findFile(yyscan_t yyscanner, const char *fileName,bool localInclude,bool &alreadyIncluded)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("** findFile(%s,%d) state->yyFileName=%s\n",fileName,localInclude,state->yyFileName.data());
  if (portable_isAbsolutePath(fileName))
  {
    FileState *fs = checkAndOpenFile(yyscanner,fileName,alreadyIncluded);
    if (fs)
    {
      setFileName(yyscanner,fileName);
      state->yyLineNr=1;
      return fs;
    }
    else if (alreadyIncluded)
    {
      return 0;
    }
  }
  if (localInclude && !state->yyFileName.isEmpty())
  {
    QFileInfo fi(state->yyFileName);
    if (fi.exists())
    {
      QCString absName = QCString(fi.dirPath(TRUE).data())+"/"+fileName;
      FileState *fs = checkAndOpenFile(yyscanner,absName,alreadyIncluded);
      if (fs)
      {
	setFileName(yyscanner,absName);
	state->yyLineNr=1;
	return fs;
      }
      else if (alreadyIncluded)
      {
	return 0;
      }
    }
  }
  if (state->pathList==0) 
  {
    return 0;
  }
  char *s=state->pathList->first();
  while (s)
  {
    QCString absName = (QCString)s+"/"+fileName;
    //printf("  Looking for %s in %s\n",fileName,s);
    FileState *fs = checkAndOpenFile(yyscanner,absName,alreadyIncluded);
    if (fs)
    {
      setFileName(yyscanner,absName);
      state->yyLineNr=1;
      //printf("  -> found it\n");
      return fs;
    }
    else if (alreadyIncluded)
    {
      return 0;
    }

    s=state->pathList->next();
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

static int getNextChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint &pos);
static int getCurrentChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint pos);
static void unputChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint &pos,char c);
static void expandExpression(yyscan_t yyscanner,QCString &expr,QCString *rest,int pos,int level);

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
  //printf("stringize '%s'->'%s'\n",s.data(),result.data());
  return result;
}

/*! Execute all ## operators in expr. 
 * If the macro name before or after the operator contains a no-rescan 
 * marker (@-) then this is removed (before the concatenated macro name
 * may be expanded again.
 */
static void processConcatOperators(QCString &expr)
{
  //printf("processConcatOperators: in='%s'\n",expr.data());
  QRegExp r("[ \\t\\n]*##[ \\t\\n]*"); 
  int l,n,i=0;
  if (expr.isEmpty()) return;
  while ((n=r.match(expr,i,&l))!=-1)
  {
    //printf("Match: '%s'\n",expr.data()+i);
    if (n+l+1<(int)expr.length() && expr.at(n+l)=='@' && expr.at(n+l+1)=='-')
    {
      // remove no-rescan marker after ID
      l+=2;
    }
    //printf("found '%s'\n",expr.mid(n,l).data());
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
  //printf("processConcatOperators: out='%s'\n",expr.data());
}

static void returnCharToStream(yyscan_t yyscanner,char c)
{
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
  unput(c);
}

static inline void addTillEndOfString(yyscan_t yyscanner,const QCString &expr,QCString *rest,
                                       uint &pos,char term,QCString &arg)
{
  int cc;
  while ((cc=getNextChar(yyscanner,expr,rest,pos))!=EOF && cc!=0)
  {
    if (cc=='\\') arg+=(char)cc,cc=getNextChar(yyscanner,expr,rest,pos);
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
static bool replaceFunctionMacro(yyscan_t yyscanner,const QCString &expr,QCString *rest,int pos,int &len,const Define *def,QCString &result,int level)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf(">replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s') level=%d\n",expr.data(),rest ? rest->data() : 0,pos,def->name.data(),state->level);
  uint j=pos;
  len=0;
  result.resize(0);
  int cc;
  while ((cc=getCurrentChar(yyscanner,expr,rest,j))!=EOF && isspace(cc)) 
  { 
    len++; 
    getNextChar(yyscanner,expr,rest,j); 
  }
  if (cc!='(') 
  { 
    unputChar(yyscanner,expr,rest,j,' '); 
    return FALSE; 
  }
  getNextChar(yyscanner,expr,rest,j); // eat the '(' character

  QDict<QCString> argTable;  // list of arguments
  argTable.setAutoDelete(TRUE);
  QCString arg;
  int argCount=0;
  bool done=FALSE;
  
  // PHASE 1: read the macro arguments
  if (def->nargs==0)
  {
    while ((cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
    {
      char c = (char)cc;
      if (c==')') break;
    }
  }
  else
  {
    while (!done && (argCount<def->nargs || def->varArgs) && 
	((cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
	  )
    {
      char c=(char)cc;
      if (c=='(') // argument is a function => search for matching )
      {
	int level=1;
	arg+=c;
	//char term='\0';
	while ((cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
	{
	  char c=(char)cc;
	  //printf("processing %c: term=%c (%d)\n",c,term,term);
	  if (c=='\'' || c=='\"') // skip ('s and )'s inside strings
	  {
	    arg+=c;
	    addTillEndOfString(yyscanner,expr,rest,j,c,arg);
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
	while (!found && (cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
	{
	  found = cc=='"';
	  if (cc=='\\')
	  {
	    c=(char)cc;	  
	    arg+=c;
	    if ((cc=getNextChar(yyscanner,expr,rest,j))==EOF || cc==0) break;
	  }
	  c=(char)cc;	  
	  arg+=c;
	}
      }
      else if (c=='\'') // append literal characters
      {
	arg+=c;
	bool found=FALSE;
	while (!found && (cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
	{
	  found = cc=='\'';
	  if (cc=='\\')
	  {
	    c=(char)cc;	  
	    arg+=c;
	    if ((cc=getNextChar(yyscanner,expr,rest,j))==EOF || cc==0) break;
	  }
	  c=(char)cc;
	  arg+=c;
	}
      }	    
      else if (c=='/') // possible start of a comment
      {
        char prevChar = '\0';
        arg+=c;
        if ((cc=getCurrentChar(yyscanner,expr,rest,j)) == '*') // we have a comment
        {
          while ((cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
          {
            c=(char)cc;
            arg+=c;
            if (c == '/' && prevChar == '*') break; // we have an end of comment
            prevChar = c;
          }
        }
      }
      else // append other characters
      {
	arg+=c;
      }
    }
  }

  // PHASE 2: apply the macro function
  if (argCount==def->nargs || // same number of arguments
      (argCount>=def->nargs-1 && def->varArgs)) // variadic macro with at least as many
                                                // params as the non-variadic part (see bug731985)
  {
    uint k=0;
    // substitution of all formal arguments
    QCString resExpr;
    const QCString d=def->definition.stripWhiteSpace();
    //printf("Macro definition: '%s'\n",d.data());
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
	    //printf("substArg='%s'\n",substArg.data());
	    // only if no ## operator is before or after the argument
	    // marker we do macro expansion.
	    if (!hash)
            {
              expandExpression(yyscanner,substArg,0,0,level+1);
            }
	    if (inString)
	    {
	      //printf("'%s'=stringize('%s')\n",stringize(*subst).data(),subst->data());

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
	      else if (state->nospaces)
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
    //printf("<replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s',result='%s') level=%d return=TRUE\n",expr.data(),rest ? rest->data() : 0,pos,def->name.data(),result.data(),state->level);
    return TRUE;
  }
  //printf("<replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s',result='%s') level=%d return=FALSE\n",expr.data(),rest ? rest->data() : 0,pos,def->name.data(),result.data(),state->level);
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

/*! performs recursive macro expansion on the string \a expr
 *  starting at position \a pos.
 *  May read additional characters from the input while re-scanning!
 */
static void expandExpression(yyscan_t yyscanner,QCString &expr,QCString *rest,int pos,int level)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf(">expandExpression(expr='%s',rest='%s',pos=%d,level=%d)\n",expr.data(),rest ? rest->data() : 0, pos, level);
  if (expr.isEmpty())
  {
    return;
  }
  if (state->expansionDict.find(expr)!=0) // check for recursive expansions
  {
    return;
  }
  else
  {
    state->expansionDict.insert(expr,(void*)0x8);
  }
  QCString macroName;
  QCString expMacro;
  bool definedTest=FALSE;
  int i=pos,l,p,len;
  while ((p=getNextId(expr,i,&l))!=-1) // search for an macro name
  {
    bool replaced=FALSE;
    macroName=expr.mid(p,l);
    //printf(" p=%d macroName=%s\n",p,macroName.data());
    if (p<2 || !(expr.at(p-2)=='@' && expr.at(p-1)=='-')) // no-rescan marker?
    {
      if (state->expandedDict->find(macroName)==0) // expand macro
      {
	Define *def=state->defineManager.isDefined(macroName);
        if (macroName=="defined")
        {
  	  //printf("found defined inside macro definition '%s'\n",expr.right(expr.length()-p).data());
	  definedTest=TRUE;
        }
	else if (definedTest) // macro name was found after defined 
	{
	  if (def) expMacro = " 1 "; else expMacro = " 0 ";
	  replaced=TRUE;
	  len=l;
	  definedTest=FALSE;
	}
	else if (def && def->nargs==-1) // simple macro
	{
	  // substitute the definition of the macro
	  //printf("macro '%s'->'%s'\n",macroName.data(),def->definition.data());
	  if (state->nospaces)
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
	  //printf("simple macro expansion='%s'->'%s'\n",macroName.data(),expMacro.data());
	}
	else if (def && def->nargs>=0) // function macro
	{
          //printf(" >>>> call replaceFunctionMacro\n");
	  replaced=replaceFunctionMacro(yyscanner,expr,rest,p+l,len,def,expMacro,level);
          //printf(" <<<< call replaceFunctionMacro: replaced=%d\n",replaced);
	  len+=l;
	}
        //printf(" macroName='%s' expMacro='%s' replaced=%d\n",macroName.data(),expMacro.data(),replaced);

	if (replaced) // expand the macro and rescan the expression
	{
	  //printf(" replacing '%s'->'%s'\n",expr.mid(p,len).data(),expMacro.data());
	  QCString resultExpr=expMacro;
	  QCString restExpr=expr.right(expr.length()-len-p);
	  processConcatOperators(resultExpr);
          //printf(" macroName=%s def->nonRecursive=%d\n",macroName.data(),def->nonRecursive);
	  if (def && !def->nonRecursive)
	  {
	    state->expandedDict->insert(macroName,def);
	    expandExpression(yyscanner,resultExpr,&restExpr,0,level+1);
	    state->expandedDict->remove(macroName);
	  }
	  expr=expr.left(p)+resultExpr+restExpr;
	  //printf(" new expression: '%s' old i=%d new i=%d\n",expr.data(),i,p);
	  i=p;
	}
	else // move to the next macro name
	{
	  //printf(" moving to the next macro old i=%d new i=%d\n",i,p+l);
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
  //printf("<expandExpression(expr='%s',rest='%s',pos=%d,level=%d)\n",expr.data(),rest ? rest->data() : 0, pos,level);
}

/*! @brief Process string or character literal.
 *
 * \a inputStr should point to the start of a string or character literal.
 * the routine will return a pointer to just after the end of the literal
 * the character making up the literal will be added to \a result.
 */
static const char *processUntilMatchingTerminator(const char *inputStr,QCString &result)
{
  if (inputStr==0) return inputStr;
  char term = *inputStr; // capture start character of the literal
  if (term!='\'' && term!='"') return inputStr; // not a valid literal
  char c=term;
  // output start character
  result+=c;
  inputStr++;
  while ((c=*inputStr)) // while inside the literal
  {
    if (c==term) // found end marker of the literal
    {
      // output end character and stop
      result+=c;
      inputStr++;
      break;
    }
    else if (c=='\\') // escaped character, process next character
                      // as well without checking for end marker.
    {
      result+=c;
      inputStr++;
      c=*inputStr;
      if (c==0) break; // unexpected end of string after escape character
    }
    result+=c;
    inputStr++;
  }
  return inputStr;
}

/*! replaces all occurrences of @@@@ in \a s by @@
 *  and removes all occurrences of @@E.
 *  All identifiers found are replaced by 0L
 */
static QCString removeIdsAndMarkers(const char *s)
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
      else if (c=='\'') // quoted character
      {
        p = processUntilMatchingTerminator(p,result);
      }
      else if (c=='d' && !inNum) // identifier starting with a 'd'
      {
	if (qstrncmp(p,"defined ",8)==0 || qstrncmp(p,"defined(",8)==0) 
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
	while ((c=*p) && isspace((uchar)c)) p++;
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
static QCString removeMarkers(const char *s)
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
	case '\'': // skip char literals
          p = processUntilMatchingTerminator(p,result);
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

static bool computeExpression(yyscan_t yyscanner,const QCString &expr)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  QCString e=expr;
  state->expansionDict.clear();
  expandExpression(yyscanner,e,0,0,0);
  //printf("after expansion '%s'\n",e.data());
  e = removeIdsAndMarkers(e);
  if (e.isEmpty()) return FALSE;
  //printf("parsing '%s'\n",e.data());
  return state->constExpParser.parse(state->yyFileName,state->yyLineNr,e);
}

/*! expands the macro definition in \a name
 *  If needed the function may read additional characters from the input
 */

static QCString expandMacro(yyscan_t yyscanner,const QCString &name)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  QCString n=name;
  state->expansionDict.clear();
  expandExpression(yyscanner,n,0,0,0);
  n=removeMarkers(n);
  //printf("expandMacro '%s'->'%s'\n",name.data(),n.data());
  return n;
}

static Define *newDefine(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  Define *def=new Define;
  def->name       = state->defName;
  def->definition = state->defText.stripWhiteSpace();
  def->nargs      = state->defArgs;
  def->fileName   = state->yyFileName; 
  def->fileDef    = state->yyFileDef;
  def->lineNr     = state->yyLineNr-state->yyMLines;
  def->columnNr   = state->yyColNr;
  def->varArgs    = state->defVarArgs;
  //printf("newDefine: %s %s file: %s\n",def->name.data(),def->definition.data(),
  //    def->fileDef ? def->fileDef->name().data() : def->fileName.data());
  //printf("newDefine: '%s'->'%s'\n",def->name.data(),def->definition.data());
  if (!def->name.isEmpty() && Doxygen::expandAsDefinedDict[def->name])
  {
    def->isPredefined=TRUE;
  }
  return def;
}

static void addDefine(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->skip) return; // do not add this define as it is inside a 
                      // conditional section (cond command) that is disabled.
  if (!Doxygen::gatherDefines) return;

  //printf("addDefine '%s' '%s'\n",state->defName.data(),state->defArgsStr.data());
  //ArgumentList *al = new ArgumentList;
  //stringToArgumentList(state->defArgsStr,al);
  MemberDef *md=createMemberDef(
      state->yyFileName,state->yyLineNr-state->yyMLines,state->yyColNr,
      "#define",state->defName,state->defArgsStr,0,
      Public,Normal,FALSE,Member,MemberType_Define,ArgumentList(),ArgumentList(),"");
  if (!state->defArgsStr.isEmpty())
  {
    ArgumentList argList;
    //printf("addDefine() state->defName='%s' state->defArgsStr='%s'\n",state->defName.data(),state->defArgsStr.data());
    stringToArgumentList(state->defArgsStr,argList);
    md->setArgumentList(argList);
  }
  //printf("Setting initializer for '%s' to '%s'\n",state->defName.data(),state->defText.data());
  int l=state->defLitText.find('\n');
  if (l>0 && state->defLitText.left(l).stripWhiteSpace()=="\\")
  {
    // strip first line if it only contains a slash
    state->defLitText = state->defLitText.right(state->defLitText.length()-l-1);
  }
  else if (l>0)
  {
    // align the items on the first line with the items on the second line
    int k=l+1;
    const char *p=state->defLitText.data()+k;
    char c;
    while ((c=*p++) && (c==' ' || c=='\t')) k++;
    state->defLitText=state->defLitText.mid(l+1,k-l-1)+state->defLitText.stripWhiteSpace();
  }
  md->setInitializer(state->defLitText.stripWhiteSpace());

  //printf("pre.l: md->setFileDef(%p)\n",state->inputFileDef);
  md->setFileDef(state->inputFileDef);
  md->setDefinition("#define "+state->defName);

  MemberName *mn=Doxygen::functionNameSDict->find(state->defName);
  if (mn==0)
  {
    mn = new MemberName(state->defName);
    Doxygen::functionNameSDict->append(state->defName,mn);
  }
  mn->append(md);
  if (state->yyFileDef) 
  {
    state->yyFileDef->insertMember(md);
  }

  //Define *d;
  //if ((d=defineDict[state->defName])==0) defineDict.insert(state->defName,newDefine()); 
}

static inline void outputChar(yyscan_t yyscanner,char c)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.isEmpty() || state->curlyCount>0) state->outputBuf->addChar(c);
}

static inline void outputArray(yyscan_t yyscanner,const char *a,int len)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.isEmpty() || state->curlyCount>0) state->outputBuf->addArray(a,len);
}

static void readIncludeFile(yyscan_t yyscanner,const QCString &inc)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  static bool searchIncludes = Config_getBool(SEARCH_INCLUDES);
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

    QCString oldFileName = state->yyFileName;
    FileDef *oldFileDef  = state->yyFileDef;
    int oldLineNr        = state->yyLineNr;
    //printf("Searching for '%s'\n",incFileName.data());

    // absIncFileName avoids difficulties for incFileName starting with "../" (bug 641336)
    QCString absIncFileName = incFileName;
    {
      QFileInfo fi(state->yyFileName);
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
	  QStrList &includePath = Config_getList(INCLUDE_PATH);
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
    state->defineManager.addInclude(state->yyFileName,absIncFileName);
    state->defineManager.addFileToContext(absIncFileName);

    // findFile will overwrite state->yyFileDef if found
    FileState *fs;
    bool alreadyIncluded = FALSE;
    //printf("calling findFile(%s)\n",incFileName.data());
    if ((fs=findFile(yyscanner,incFileName,localInclude,alreadyIncluded))) // see if the include file can be found
    {
      //printf("Found include file!\n");
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        for (i=0;i<state->includeStack.count();i++) 
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
        oldFileDef->addIncludeDependency(ambig ? 0 : incFd,incFileName,localInclude,state->isImported,FALSE);
        // add included by dependency
        if (state->yyFileDef)
        {
          //printf("Adding include dependency %s->%s\n",oldFileDef->name().data(),incFileName.data());
          state->yyFileDef->addIncludedByDependency(oldFileDef,oldFileDef->docName(),localInclude,state->isImported);
        }
      }
      else if (state->inputFileDef)
      {
        state->inputFileDef->addIncludeDependency(0,absIncFileName,localInclude,state->isImported,TRUE);
      }
      struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
      fs->bufState = YY_CURRENT_BUFFER;
      fs->lineNr   = oldLineNr;
      fs->fileName = oldFileName;
      fs->curlyCount = state->curlyCount;
      state->curlyCount = 0;
      // push the state on the stack
      state->includeStack.push(fs);
      // set the scanner to the include file

      // Deal with file changes due to 
      // #include's within { .. } blocks
      QCString lineStr(state->yyFileName.length()+20);
      lineStr.sprintf("# 1 \"%s\" 1\n",state->yyFileName.data());
      outputArray(yyscanner,lineStr.data(),lineStr.length());

      DBG_CTX((stderr,"Switching to include file %s\n",incFileName.data()));
      state->expectGuard=TRUE;
      state->inputBuf   = &fs->fileBuf;
      state->inputBufPos=0;
      yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner),yyscanner);
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
	oldFileDef->addIncludeDependency(ambig ? 0 : fd,incFileName,localInclude,state->isImported,FALSE);
	// add included by dependency
        if (fd)
        {
          //printf("Adding include dependency (2) %s->%s ambig=%d\n",oldFileDef->name().data(),fd->name().data(),ambig);
          fd->addIncludedByDependency(oldFileDef,oldFileDef->docName(),localInclude,state->isImported);
        }
      }
      else if (state->inputFileDef)
      {
        state->inputFileDef->addIncludeDependency(0,absIncFileName,localInclude,state->isImported,TRUE);
      }
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
	if (alreadyIncluded)
	{
          Debug::print(Debug::Preprocessor,0,"#include %s: already included! skipping...\n",qPrint(incFileName));
	}
	else
	{
          Debug::print(Debug::Preprocessor,0,"#include %s: not found! skipping...\n",qPrint(incFileName));
	}
        //printf("error: include file %s not found\n",yytext);
      }
      if (state->curlyCount>0 && !alreadyIncluded) // failed to find #include inside { ... }
      {
	warn(state->yyFileName,state->yyLineNr,"include file %s not found, perhaps you forgot to add its directory to INCLUDE_PATH?",incFileName.data());
      }
    }
  }
}

/* ----------------------------------------------------------------- */

static void startCondSection(yyscan_t yyscanner,const char *sectId)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("startCondSection: skip=%d stack=%d\n",state->skip,state->condStack.count());
  CondParser prs;
  bool expResult = prs.parse(state->yyFileName,state->yyLineNr,sectId);
  state->condStack.push(new CondCtx(state->yyLineNr,sectId,state->skip));
  if (!expResult)
  {
    state->skip=TRUE;
  }
  //printf("  expResult=%d skip=%d\n",expResult,state->skip);
}

static void endCondSection(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->condStack.isEmpty())
  {
    state->skip=FALSE;
  }
  else
  {
    CondCtx *ctx = state->condStack.pop();
    state->skip=ctx->skip;
    delete ctx;
  }
  //printf("endCondSection: skip=%d stack=%d\n",state->skip,state->condStack.count());
}

static void forceEndCondSection(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  while (!state->condStack.isEmpty())
  {
    delete state->condStack.pop();
  }
  state->skip=FALSE;
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

/*@ ----------------------------------------------------------------------------
 */

static int getNextChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint &pos)
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
    int cc=yyinput(yyscanner);
    //printf("%d=yyinput() %d\n",cc,EOF);
    return cc;
  }
}
 
static int getCurrentChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint pos)
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
    int cc=yyinput(yyscanner);
    returnCharToStream(yyscanner,cc);
    //unput((char)cc);
    //printf("%c=yyinput()\n",cc);
    return cc;
  }
}

static void unputChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint &pos,char c)
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
    returnCharToStream(yyscanner,c);
  }
  //printf("result: unputChar(%s,%s,%d,%c)\n",expr.data(),rest ? rest->data() : 0,pos,c);
}

///////////////////////////////////////////////////////////////////////////////////////////////

struct Preprocessor::Private
{
  yyscan_t yyscanner;
  preYY_state state;
  bool firstTime = FALSE;
};

void Preprocessor::addSearchDir(const char *dir)
{
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  QFileInfo fi(dir);
  if (fi.isDir()) state->pathList->append(fi.absFilePath().utf8());
} 

Preprocessor::Preprocessor()
{
  p = new Private;
  preYYlex_init_extra(&p->state,&p->yyscanner);
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  state->pathList = new QStrList;
  addSearchDir(".");
  state->expandedDict = new DefineDict(17);
}

Preprocessor::~Preprocessor()
{
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  delete state->expandedDict; 
  state->expandedDict=0;
  delete state->pathList; 
  state->pathList=0;
  preYYlex_destroy(p->yyscanner);
  delete p;
}

void Preprocessor::processFile(const char *fileName,BufStr &input,BufStr &output)
{
  static bool warningsGiven = false; // Note: reset after all defines have been handled once!
  yyscan_t yyscanner = p->yyscanner;
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

#ifdef FLEX_DEBUG
  preYYset_debug(1,yyscanner);
#endif

  printlex(yy_flex_debug, TRUE, __FILE__, fileName);
  uint orgOffset=output.curPos();
  //printf("##########################\n%s\n####################\n",
  //    input.data());

  state->macroExpansion = Config_getBool(MACRO_EXPANSION);
  state->expandOnlyPredef = Config_getBool(EXPAND_ONLY_PREDEF);
  state->skip=FALSE;
  state->curlyCount=0;
  state->nospaces=FALSE;
  state->inputBuf=&input;
  state->inputBufPos=0;
  state->outputBuf=&output;
  state->includeStack.setAutoDelete(TRUE);
  state->includeStack.clear();
  state->expandedDict->setAutoDelete(FALSE);
  state->expandedDict->clear();
  state->condStack.setAutoDelete(TRUE);
  state->condStack.clear();
  //state->fileDefineDict->clear();

  setFileName(yyscanner,fileName);
  state->inputFileDef = state->yyFileDef;
  state->defineManager.startContext(state->yyFileName);
  
  p->firstTime=TRUE;
  if (p->firstTime)
  {
    // add predefined macros
    char *defStr;
    QStrList &predefList = Config_getList(PREDEFINED);
    QStrListIterator sli(predefList);
    for (sli.toFirst();(defStr=sli.current());++sli)
    {
      QCString ds = defStr;
      int i_equals=ds.find('=');
      int i_obrace=ds.find('(');
      int i_cbrace=ds.find(')');
      bool nonRecursive = i_equals>0 && ds.at(i_equals-1)==':';

      if ((i_obrace==0) || (i_equals==0) || (i_equals==1 && ds.at(i_equals-1)==':'))
      {
        if (!warningsGiven) config_warn("In PREDEFINED configuration no define name specified in '%s'\n",ds.data());
        // Note: reset wil be after lopp when all defines have been handled once!
        continue;
      }

      if (i_obrace<i_equals && i_cbrace<i_equals && 
	  i_obrace!=-1      && i_cbrace!=-1      && 
	  i_obrace<i_cbrace
	 ) // predefined function macro definition
      {
	//printf("predefined function macro '%s'\n",defStr);
	QRegExp reId("[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*"); // regexp matching an id
	QDict<int> argDict(17);
	argDict.setAutoDelete(TRUE);
	int i=i_obrace+1,p,l,count=0;
	// gather the formal arguments in a dictionary 
	while (i<i_cbrace && (p=reId.match(ds,i,&l)))
	{
          if (l>0) // see bug375037
          {
	    argDict.insert(ds.mid(p,l),new int(count++));
	    i=p+l;
          }
          else
          {
            i++;
          }
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
	  def->fileDef      = state->yyFileDef;
	  def->fileName     = fileName;
	  state->defineManager.addDefine(state->yyFileName,def);
	}

	//printf("#define '%s' '%s' #nargs=%d\n",
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
	  def->fileDef      = state->yyFileDef;
	  def->fileName     = fileName;
	  state->defineManager.addDefine(state->yyFileName,def);
	}
	else
	{
	  delete def;
	}

	//printf("#define '%s' '%s' #nargs=%d\n",
	//  def->name.data(),def->definition.data(),def->nargs);
      }
    }
    warningsGiven=true; // Note: reset after all defines have been handled once!
    //firstTime=FALSE;
  }
 
  state->yyLineNr = 1;
  state->yyColNr  = 1;
  state->level    = 0;
  state->ifcount  = 0;

  BEGIN( Start );
  
  state->expectGuard = guessSection(fileName)==Entry::HEADER_SEC;
  state->guardName.resize(0);
  state->lastGuardName.resize(0);
  state->guardExpr.resize(0);
  
  preYYlex(yyscanner);

  while (!state->condStack.isEmpty())
  {
    CondCtx *ctx = state->condStack.pop();
    QCString sectionInfo = " ";
    if (ctx->sectionId!=" ") sectionInfo.sprintf(" with label '%s' ",ctx->sectionId.stripWhiteSpace().data());
    warn(fileName,ctx->lineNr,"Conditional section%sdoes not have "
	"a corresponding \\endcond command within this file.",sectionInfo.data());
    delete ctx;
  }
  // make sure we don't extend a \cond with missing \endcond over multiple files (see bug 624829)
  forceEndCondSection(yyscanner);

  // remove locally defined macros so they can be redefined in another source file
  //if (state->fileDefineDict->count()>0)
  //{
  //  QDictIterator<Define> di(*state->fileDefineDict);
  //  Define *d;
  //  for (di.toFirst();(d=di.current());++di)
  //  {
  //    state->globalDefineDict->remove(di.currentKey());
  //  }
  //  state->fileDefineDict->clear();
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
    if (state->defineManager.defineContext().count()>0)
    {
      Debug::print(Debug::Preprocessor,0,"Macros accessible in this file:\n");
      Debug::print(Debug::Preprocessor,0,"---------\n");
      QDictIterator<Define> di(state->defineManager.defineContext());
      Define *def;
      for (di.toFirst();(def=di.current());++di)
      {
        Debug::print(Debug::Preprocessor,0,"%s ",qPrint(def->name));
      }
      Debug::print(Debug::Preprocessor,0,"\n---------\n");
    }
    else
    {
      Debug::print(Debug::Preprocessor,0,"No macros accessible in this file.\n");
    }
  }
  state->defineManager.endContext();
  printlex(yy_flex_debug, FALSE, __FILE__, fileName);
}


#include "pre.l.h"
