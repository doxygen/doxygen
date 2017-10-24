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
%option noyywrap
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
  FileState(int size) : lineNr(1), fileBuf(size),
                        oldFileBuf(0), oldFileBufPos(0), bufState(0) {}
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
	dpf = new DefinesPerFile;
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
struct preYY_state
{
  int                g_yyLineNr   = 1;
  int                g_yyMLines   = 1;
  int                g_yyColNr   = 1;
  QCString           g_yyFileName;
  FileDef           *g_yyFileDef;
  FileDef           *g_inputFileDef;
  int                g_ifcount    = 0;
  QStrList          *g_pathList = 0;  
  QStack<FileState>  g_includeStack;
  QDict<int>        *g_argDict;
  int                g_defArgs = -1;
  QCString           g_defName;
  QCString           g_defText;
  QCString           g_defLitText;
  QCString           g_defArgsStr;
  QCString           g_defExtraSpacing;
  bool               g_defVarArgs;
  int                g_level;
  int                g_lastCContext;
  int                g_lastCPPContext;
  QArray<int>        g_levelGuard;
  BufStr            *g_inputBuf;
  int                g_inputBufPos;
  BufStr            *g_outputBuf;
  int                g_roundCount;
  bool               g_quoteArg;
  DefineDict        *g_expandedDict;
  int                g_findDefArgContext;
  bool               g_expectGuard;
  QCString           g_guardName;
  QCString           g_lastGuardName;
  QCString           g_incName;
  QCString           g_guardExpr;
  int                g_curlyCount;
  bool               g_nospaces; // add extra spaces during macro expansion
  
  bool               g_macroExpansion; // from the configuration
  bool               g_expandOnlyPredef; // from the configuration
  int                g_commentCount;
  bool               g_insideComment;
  bool               g_isImported;
  QCString           g_blockName;
  int                g_condCtx;
  bool               g_skip;
  QStack<CondCtx>    g_condStack;
  bool               g_insideCS; // C# has simpler preprocessor
  bool               g_isSource;
  
  bool               g_lexInit = FALSE;
  int                g_fenceSize = 0;
  bool               g_ccomment;
  QDict<void>        g_allIncludes;
  Doxygen           *doxygen = NULL;
};

//DefineDict* getGlobalDefineDict() 
//{
//  return yyget_extra(yyscanner)->g_globalDefineDict;
//}

static void setFileName(const char *name, yyscan_t yyscanner);
static void incrLevel(yyscan_t yyscanner);
static void decrLevel(yyscan_t yyscanner);
static bool otherCaseDone(yyscan_t yyscanner);
static void setCaseDone(bool value, yyscan_t yyscanner);
static FileState *checkAndOpenFile(const QCString &fileName,bool &alreadyIncluded, yyscan_t yyscanner);
static FileState *findFile(const char *fileName,bool localInclude,bool &alreadyIncluded, yyscan_t yyscanner);
static QCString extractTrailingComment(const char *s);
static int getNextChar(const QCString &expr,QCString *rest,uint &pos,yyscan_t yyscanner);
static int getCurrentChar(const QCString &expr,QCString *rest,uint pos, yyscan_t yyscanner);
static void unputChar(const QCString &expr,QCString *rest,uint &pos,char c, yyscan_t yyscanner);
static void expandExpression(QCString &expr,QCString *rest,int pos, yyscan_t yyscanner);
static QCString stringize(const QCString &s);
static void processConcatOperators(QCString &expr);
static void yyunput (int c,char *buf_ptr, yyscan_t yyscanner);
static void returnCharToStream(char c, yyscan_t yyscanner);
static void addTillEndOfString(const QCString &expr,QCString *rest,
                                       uint &pos,char term,QCString &arg,
                                       yyscan_t yyscanner);
static bool replaceFunctionMacro(const QCString &expr,QCString *rest,int pos,int &len,const Define *def,QCString &result, yyscan_t yyscanner);
static int getNextId(const QCString &expr,int p,int *l);
static void expandExpression(QCString &expr,QCString *rest,int pos);
static QCString removeIdsAndMarkers(const char *s);
static QCString removeMarkers(const char *s);
static bool computeExpression(const QCString &expr, yyscan_t yyscanner);
static QCString expandMacro(const QCString &name, yyscan_t yyscanner);
static Define *newDefine(yyscan_t yyscanner);
static void addDefine(yyscan_t yyscanner);
static void outputChar(char c, yyscan_t yyscanner);
static void outputArray(const char *a,int len, yyscan_t yyscanner);
static void readIncludeFile(const QCString &inc, yyscan_t yyscanner);
static void startCondSection(const char *sectId, yyscan_t yyscanner);
static void endCondSection(yyscan_t yyscanner);
static void forceEndCondSection(yyscan_t yyscanner);
static QCString escapeAt(const char *text);
static char resolveTrigraph(char c);
static int yyread(char *buf,int max_size,yyscan_t yyscanner);

/* ----------------------------------------------------------------- */

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size,yyscanner);

/* ----------------------------------------------------------------- */

%}

ID	[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
B       [ \t]
BN	[ \t\r\n]
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))

%x      Start
%x	Command
%x	SkipCommand
%x	SkipLine
%x	SkipString
%x	CopyLine
%x	CopyString
%x	CopyStringFtn
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
<Start>^{B}*"#"				{ BEGIN(Command);
					  yyextra->g_yyColNr+=yyleng;
					  yyextra->g_yyMLines=0;}
<Start>^{B}*/[^#]			{
 					  outputArray(yytext,(int)yyleng, yyscanner);
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
  					  static bool skipFuncMacros = Config_getBool(SKIP_FUNCTION_MACROS);
					  QCString name(yytext);
					  name=name.left(name.find('(')).stripWhiteSpace();

					  Define *def=0;
					  if (skipFuncMacros && 
					      name!="Q_PROPERTY" &&
					      !(
					         (yyextra->g_includeStack.isEmpty() || yyextra->g_curlyCount>0) &&
					         yyextra->g_macroExpansion &&
					         (def=DefineManager::instance().isDefined(name)) &&
						 /*macroIsAccessible(def) &&*/
					         (!yyextra->g_expandOnlyPredef || def->isPredefined)
					       )
					     )
					  {
					    outputChar('\n', yyscanner);
					    yyextra->g_yyLineNr++;
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
  					  yyextra->g_yyLineNr+=text.contains('\n');
					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<CopyLine>"{"				{ // count brackets inside the main file
  					  if (yyextra->g_includeStack.isEmpty()) 
					  {
					    yyextra->g_curlyCount++;
					  }
					  outputChar(*yytext, yyscanner);
  					}
<CopyLine>"}"				{ // count brackets inside the main file
  					  if (yyextra->g_includeStack.isEmpty() && yyextra->g_curlyCount>0) 
					  {
					    yyextra->g_curlyCount--;
					  }
					  outputChar(*yytext, yyscanner);
  					}
<CopyLine>"'"\\[0-7]{1,3}"'"		{ 
  					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyLine>"'"\\."'"			{ 
  					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyLine>"'"."'"			{ 
  					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyLine>\"				{
					  outputChar(*yytext, yyscanner);
					  BEGIN( CopyString );
					}
<CopyLine>\'				{
                                          if (getLanguageFromFileName(yyextra->g_yyFileName)!=SrcLangExt_Fortran) REJECT;
					  outputChar(*yytext, yyscanner);
					  BEGIN( CopyStringFtn );
					}
<CopyString>[^\"\\\r\n]+		{
  					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyString>\\.				{
					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyString>\"				{
					  outputChar(*yytext, yyscanner);
					  BEGIN( CopyLine );
					}
<CopyStringFtn>[^\'\\\r\n]+		{
  					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyStringFtn>\\.			{
					  outputArray(yytext,(int)yyleng, yyscanner);
					}
<CopyStringFtn>\'			{
					  outputChar(*yytext, yyscanner);
					  BEGIN( CopyLine );
					}
<CopyLine>{ID}/{BN}{0,80}"("		{
  					  yyextra->g_expectGuard = FALSE;
  					  Define *def=0;
					  //def=yyextra->g_globalDefineDict->find(yytext);
					  //def=DefineManager::instance().isDefined(yytext);
					  //printf("Search for define %s found=%d yyextra->g_includeStack.isEmpty()=%d "
					  //       "yyextra->g_curlyCount=%d yyextra->g_macroExpansion=%d yyextra->g_expandOnlyPredef=%d "
					  //	 "isPreDefined=%d\n",yytext,def ? 1 : 0,
					  //	 yyextra->g_includeStack.isEmpty(),yyextra->g_curlyCount,yyextra->g_macroExpansion,yyextra->g_expandOnlyPredef,
					  //	 def ? def->isPredefined : -1
					  //	);
					  if ((yyextra->g_includeStack.isEmpty() || yyextra->g_curlyCount>0) &&
					      yyextra->g_macroExpansion &&
					      (def=DefineManager::instance().isDefined(yytext)) &&
				              /*(def->isPredefined || macroIsAccessible(def)) && */
					      (!yyextra->g_expandOnlyPredef || def->isPredefined)
					     )
					  {
					    //printf("Found it! #args=%d\n",def->nargs);
					    yyextra->g_roundCount=0;
					    yyextra->g_defArgsStr=yytext;
					    if (def->nargs==-1) // no function macro
					    {
					      QCString result = def->isPredefined ? def->definition : expandMacro(yyextra->g_defArgsStr, yyscanner);
					      outputArray(result,result.length(), yyscanner);
					    }
					    else // zero or more arguments
					    {
					      yyextra->g_findDefArgContext = CopyLine;
					      BEGIN(FindDefineArgs);
					    }
					  }
					  else
					  {
					    outputArray(yytext,(int)yyleng, yyscanner);
					  }
  					}
<CopyLine>{ID}				{
                                          Define *def=0;
  					  if ((yyextra->g_includeStack.isEmpty() || yyextra->g_curlyCount>0) && 
					      yyextra->g_macroExpansion &&
					      (def=DefineManager::instance().isDefined(yytext)) &&
					      def->nargs==-1 &&
				              /*(def->isPredefined || macroIsAccessible(def)) &&*/
					      (!yyextra->g_expandOnlyPredef || def->isPredefined)
					     )
					  {
					    QCString result=def->isPredefined ? def->definition : expandMacro(yytext, yyscanner); 
					    outputArray(result,result.length(), yyscanner);
					  }
					  else
					  {
					    outputArray(yytext,(int)yyleng, yyscanner);
					  }
  					}
<CopyLine>"\\"\r?/\n			{ // strip line continuation characters
  					}
<CopyLine>.				{
  					  outputChar(*yytext, yyscanner);
  					}
<CopyLine>\n				{
  					  outputChar('\n', yyscanner);
					  BEGIN(Start);
					  yyextra->g_yyLineNr++;
					  yyextra->g_yyColNr=1;
  					}
<FindDefineArgs>"("			{
  					  yyextra->g_defArgsStr+='(';
  					  yyextra->g_roundCount++;
  					}
<FindDefineArgs>")"			{
  					  yyextra->g_defArgsStr+=')';
					  yyextra->g_roundCount--;
					  if (yyextra->g_roundCount==0)
					  {
					    QCString result=expandMacro(yyextra->g_defArgsStr, yyscanner);
					    //printf("yyextra->g_defArgsStr=`%s'->`%s'\n",yyextra->g_defArgsStr.data(),result.data());
					    if (yyextra->g_findDefArgContext==CopyLine)
					    {
					      outputArray(result,result.length(), yyscanner);
					      BEGIN(yyextra->g_findDefArgContext);
					    }
					    else // yyextra->g_findDefArgContext==IncludeID
					    {
					      readIncludeFile(result, yyscanner);
					      yyextra->g_nospaces=FALSE;
					      BEGIN(Start);
					    }
					  }
  					}
  /*
<FindDefineArgs>")"{B}*"("		{
  					  yyextra->g_defArgsStr+=yytext;
  					}
  */
<FindDefineArgs>{CHARLIT}		{
  					  yyextra->g_defArgsStr+=yytext;
  					}
<FindDefineArgs>"/*"[*]?                {
                                          yyextra->g_defArgsStr+=yytext;
                                          BEGIN(ArgCopyCComment);
                                        }
<FindDefineArgs>\"			{
  					  yyextra->g_defArgsStr+=*yytext;
  					  BEGIN(ReadString);
  					}
<FindDefineArgs>'                       {
                                          if (getLanguageFromFileName(yyextra->g_yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          yyextra->g_defArgsStr+=*yytext;
                                          BEGIN(ReadString);
                                        }
<FindDefineArgs>\n			{
                                          yyextra->g_defArgsStr+=' ';
  					  yyextra->g_yyLineNr++;
					  outputChar('\n', yyscanner);
  					}
<FindDefineArgs>"@"			{
  					  yyextra->g_defArgsStr+="@@";
  					}
<FindDefineArgs>.			{
  					  yyextra->g_defArgsStr+=*yytext;
  					}
<ArgCopyCComment>[^*\n]+		{
					  yyextra->g_defArgsStr+=yytext;
  					}
<ArgCopyCComment>"*/"			{
					  yyextra->g_defArgsStr+=yytext;
  					  BEGIN(FindDefineArgs);
  					}
<ArgCopyCComment>\n			{ 
                                          yyextra->g_defArgsStr+=' ';
  					  yyextra->g_yyLineNr++;
					  outputChar('\n', yyscanner);
  					}
<ArgCopyCComment>.			{ 
                                          yyextra->g_defArgsStr+=yytext;
                                        }
<ReadString>"\""			{
  					  yyextra->g_defArgsStr+=*yytext;
					  BEGIN(FindDefineArgs);
  					}
<ReadString>"'"                         {
                                          if (getLanguageFromFileName(yyextra->g_yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          yyextra->g_defArgsStr+=*yytext;
                                          BEGIN(FindDefineArgs);
                                        }

<ReadString>"//"|"/*"			{
  					  yyextra->g_defArgsStr+=yytext;
  					}
<ReadString>\\.				{
  					  yyextra->g_defArgsStr+=yytext;
  					}
<ReadString>.				{
  					  yyextra->g_defArgsStr+=*yytext;
  					}
<Command>("include"|"import"){B}+/{ID}	{
  					  yyextra->g_isImported = yytext[1]=='m';
  					  if (yyextra->g_macroExpansion) 
					    BEGIN(IncludeID);
  					}
<Command>("include"|"import"){B}*[<"]	{ 
  					  yyextra->g_isImported = yytext[1]=='m';
					  char c[2];
					  c[0]=yytext[yyleng-1];c[1]='\0';
					  yyextra->g_incName=c;
  					  BEGIN(Include); 
					}
<Command>("cmake")?"define"{B}+		{ 
  			                  //printf("!!!DefName\n"); 
					  yyextra->g_yyColNr+=yyleng;
  					  BEGIN(DefName); 
					}
<Command>"ifdef"/{B}*"("		{
  					  incrLevel(yyscanner);
					  yyextra->g_guardExpr.resize(0);
  					  BEGIN(DefinedExpr2);
  					}
<Command>"ifdef"/{B}+			{
  					  //printf("Pre.l: ifdef\n");
  					  incrLevel(yyscanner);
					  yyextra->g_guardExpr.resize(0);
  					  BEGIN(DefinedExpr1);
  					}
<Command>"ifndef"/{B}*"("		{
  					  incrLevel(yyscanner);
					  yyextra->g_guardExpr="! ";
  					  BEGIN(DefinedExpr2);
					}
<Command>"ifndef"/{B}+			{
  					  incrLevel(yyscanner);
					  yyextra->g_guardExpr="! ";
  					  BEGIN(DefinedExpr1);
  					}
<Command>"if"/[ \t(!]			{
  					  incrLevel(yyscanner);
					  yyextra->g_guardExpr.resize(0);
					  BEGIN(Guard);
					}
<Command>("elif"|"else"{B}*"if")/[ \t(!]	{
  					  if (!otherCaseDone(yyscanner))
					  {
					    yyextra->g_guardExpr.resize(0);
					    BEGIN(Guard);  
					  }
					  else
					  {
					    yyextra->g_ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<Command>"else"/[^a-z_A-Z0-9\x80-\xFF]		{
					  //printf("else yyextra->g_levelGuard[%d]=%d\n",yyextra->g_level-1,yyextra->g_levelGuard[yyextra->g_level-1]);
  					  if (otherCaseDone(yyscanner))
					  {
					    yyextra->g_ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
					  else
					  {
					    setCaseDone(TRUE, yyscanner);
					    //yyextra->g_levelGuard[yyextra->g_level-1]=TRUE;
					  } 
  					}
<Command>"undef"{B}+			{
  					  BEGIN(UndefName);
  					}
<Command>("elif"|"else"{B}*"if")/[ \t(!]	{
  					  if (!otherCaseDone(yyscanner))
					  {
					    yyextra->g_guardExpr.resize(0);
  					    BEGIN(Guard);
					  }
  					}
<Command>"endif"/[^a-z_A-Z0-9\x80-\xFF]		{
  					  //printf("Pre.l: #endif\n");
  					  decrLevel(yyscanner);
  					}
<Command,IgnoreLine>\n			{
  					  outputChar('\n', yyscanner);
  					  BEGIN(Start);
					  yyextra->g_yyLineNr++;
  					}
<Command>"pragma"{B}+"once"             {
                                          yyextra->g_expectGuard = FALSE;
                                        }
<Command>{ID}				{ // unknown directive
					  BEGIN(IgnoreLine);
					}
<IgnoreLine>\\[\r]?\n			{
  					  outputChar('\n', yyscanner);
					  yyextra->g_yyLineNr++;
					}
<IgnoreLine>.
<Command>. {yyextra->g_yyColNr+=yyleng;}
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
  					  outputChar('\n', yyscanner);
  					  yyextra->g_guardExpr+=' ';
					  yyextra->g_yyLineNr++;
  					}
<Guard>"defined"/{B}*"("		{
    					  BEGIN(DefinedExpr2);
    					}
<Guard>"defined"/{B}+			{
    					  BEGIN(DefinedExpr1);
    					}
<Guard>{ID}				{ yyextra->g_guardExpr+=yytext; }
<Guard>.				{ yyextra->g_guardExpr+=*yytext; }
<Guard>\n				{
  					  unput(*yytext);
  					  //printf("Guard: `%s'\n",
					  //    yyextra->g_guardExpr.data());
					  bool guard=computeExpression(yyextra->g_guardExpr, yyscanner);
					  setCaseDone(guard, yyscanner);
					  //printf("if yyextra->g_levelGuard[%d]=%d\n",yyextra->g_level-1,yyextra->g_levelGuard[yyextra->g_level-1]);
					  if (guard)
					  {
					    BEGIN(Start);
					  } 
					  else
					  {
					    yyextra->g_ifcount=0;
					    BEGIN(SkipCPPBlock);
					  }
  					}
<DefinedExpr1,DefinedExpr2>\\\n		{ yyextra->g_yyLineNr++; outputChar('\n', yyscanner); }
<DefinedExpr1>{ID}			{
  					  if (DefineManager::instance().isDefined(yytext) || yyextra->g_guardName==yytext)
					    yyextra->g_guardExpr+=" 1L ";
					  else
					    yyextra->g_guardExpr+=" 0L ";
					  yyextra->g_lastGuardName=yytext;
					  BEGIN(Guard);
  					}
<DefinedExpr2>{ID}			{
  					  if (DefineManager::instance().isDefined(yytext) || yyextra->g_guardName==yytext)
					    yyextra->g_guardExpr+=" 1L ";
					  else
					    yyextra->g_guardExpr+=" 0L ";
					  yyextra->g_lastGuardName=yytext;
  					}
<DefinedExpr1,DefinedExpr2>\n		{ // should not happen, handle anyway
                                          yyextra->g_yyLineNr++;
  					  yyextra->g_ifcount=0;
 					  BEGIN(SkipCPPBlock); 
					}
<DefinedExpr2>")"			{
  					  BEGIN(Guard);
  					}
<DefinedExpr1,DefinedExpr2>.
<SkipCPPBlock>^{B}*"#"			{ BEGIN(SkipCommand); }
<SkipCPPBlock>^{B}*/[^#]		{ BEGIN(SkipLine); }
<SkipCPPBlock>\n			{ yyextra->g_yyLineNr++; outputChar('\n', yyscanner); }
<SkipCPPBlock>.
<SkipCommand>"if"(("n")?("def"))?/[ \t(!]	{ 
  					  incrLevel(yyscanner);
                                          yyextra->g_ifcount++; 
  					  //printf("#if... depth=%d\n",yyextra->g_ifcount);
					}
<SkipCommand>"else"			{
					  //printf("Else! yyextra->g_ifcount=%d otherCaseDone=%d\n",yyextra->g_ifcount,otherCaseDone());
  					  if (yyextra->g_ifcount==0 && !otherCaseDone(yyscanner))
					  {
					    setCaseDone(TRUE, yyscanner);
  					    //outputChar('\n', yyscanner);
					    BEGIN(Start);
					  }
  					}
<SkipCommand>("elif"|"else"{B}*"if")/[ \t(!]		{
  					  if (yyextra->g_ifcount==0) 
					  {
  					    if (!otherCaseDone(yyscanner))
					    {
					      yyextra->g_guardExpr.resize(0);
					      yyextra->g_lastGuardName.resize(0);
  					      BEGIN(Guard);
					    }
					    else
					    {
					      BEGIN(SkipCPPBlock);
					    }
					  }
					}
<SkipCommand>"endif"			{ 
					  yyextra->g_expectGuard = FALSE;
  					  decrLevel(yyscanner);
  				          if (--yyextra->g_ifcount<0)
  					  {
  					    //outputChar('\n', yyscanner);
					    BEGIN(Start);
					  }
					}
<SkipCommand>\n				{ 
  					  outputChar('\n', yyscanner);
  					  yyextra->g_yyLineNr++; 
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
  					  yyextra->g_lastCPPContext=YY_START;
  					  BEGIN(RemoveCPPComment);
					}
<SkipString>"/*"/[^\n]*                 { 
                                        }
<SkipLine,SkipCommand,SkipCPPBlock>"/*"/[^\n]* {
					  yyextra->g_lastCContext=YY_START;
  					  BEGIN(RemoveCComment);
  					}
<SkipLine>\n				{
  					  outputChar('\n', yyscanner);
					  yyextra->g_yyLineNr++;  
					  BEGIN(SkipCPPBlock);
					}
<SkipString>[^"\\\n]+			{ }
<SkipString>\\.				{ }
<SkipString>\"				{
  					  BEGIN(SkipLine);
  					}
<SkipString>.				{ }
<IncludeID>{ID}{B}*/"("			{
  					  yyextra->g_nospaces=TRUE;
				          yyextra->g_roundCount=0;
					  yyextra->g_defArgsStr=yytext;
					  yyextra->g_findDefArgContext = IncludeID;
					  BEGIN(FindDefineArgs);
					}
<IncludeID>{ID}				{
  					  yyextra->g_nospaces=TRUE;
                                          readIncludeFile(expandMacro(yytext, yyscanner), yyscanner);
					  BEGIN(Start);
  					}
<Include>[^\">\n]+[\">]			{ 
					  yyextra->g_incName+=yytext;
					  readIncludeFile(yyextra->g_incName, yyscanner);
					  if (yyextra->g_isImported)
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
					  outputChar('\n', yyscanner);
					  yyextra->g_yyLineNr++;
					}
<EndImport>.				{
  					}
<DefName>{ID}/("\\\n")*"("		{ // define with argument
  					  //printf("Define() `%s'\n",yytext);
					  delete yyextra->g_argDict;
					  yyextra->g_argDict = new QDict<int>(31);
					  yyextra->g_argDict->setAutoDelete(TRUE);
					  yyextra->g_defArgs = 0; 
                                          yyextra->g_defArgsStr.resize(0);
					  yyextra->g_defText.resize(0);
					  yyextra->g_defLitText.resize(0);
					  yyextra->g_defName = yytext;
					  yyextra->g_defVarArgs = FALSE;
					  yyextra->g_defExtraSpacing.resize(0);
					  BEGIN(DefineArg);
  					}
<DefName>{ID}{B}+"1"/[ \r\t\n]		{ // special case: define with 1 -> can be "guard"
  					  //printf("Define `%s'\n",yytext);
					  delete yyextra->g_argDict; yyextra->g_argDict=0;
					  yyextra->g_defArgs = -1;
                                          yyextra->g_defArgsStr.resize(0);
					  yyextra->g_defName = yytext;
					  yyextra->g_defName = yyextra->g_defName.left(yyextra->g_defName.length()-1).stripWhiteSpace();
					  yyextra->g_defVarArgs = FALSE;
					  //printf("Guard check: %s!=%s || %d\n",
					  //    yyextra->g_defName.data(),yyextra->g_lastGuardName.data(),yyextra->g_expectGuard);
					  if (yyextra->g_curlyCount>0 || yyextra->g_defName!=yyextra->g_lastGuardName || !yyextra->g_expectGuard)
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+yyextra->g_defName;
					    outputArray(tmp.data(),tmp.length(), yyscanner);
					    yyextra->g_quoteArg=FALSE;
					    yyextra->g_insideComment=FALSE;
					    yyextra->g_lastGuardName.resize(0);
				            yyextra->g_defText="1"; 
					    yyextra->g_defLitText="1"; 
					    BEGIN(DefineText); 
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
					    yyextra->g_defText.resize(0);
					    yyextra->g_defLitText.resize(0);
					    BEGIN(Start);
					  }
					  yyextra->g_expectGuard=FALSE;
  					}
<DefName>{ID}/{B}*"\n"			{ // empty define
					  delete yyextra->g_argDict; yyextra->g_argDict=0;
					  yyextra->g_defArgs = -1;
					  yyextra->g_defName = yytext;
                                          yyextra->g_defArgsStr.resize(0);
					  yyextra->g_defText.resize(0);
					  yyextra->g_defLitText.resize(0);
					  yyextra->g_defVarArgs = FALSE;
					  //printf("Guard check: %s!=%s || %d\n",
					  //    yyextra->g_defName.data(),yyextra->g_lastGuardName.data(),yyextra->g_expectGuard);
					  if (yyextra->g_curlyCount>0 || yyextra->g_defName!=yyextra->g_lastGuardName || !yyextra->g_expectGuard)
					  { // define may appear in the output
					    QCString tmp=(QCString)"#define "+yyextra->g_defName;
					    outputArray(tmp.data(),tmp.length(), yyscanner);
					    yyextra->g_quoteArg=FALSE;
					    yyextra->g_insideComment=FALSE;
					    if (yyextra->g_insideCS) yyextra->g_defText="1"; // for C#, use "1" as define text
					    BEGIN(DefineText);
					  }
					  else // define is a guard => hide
					  {
					    //printf("Found a guard %s\n",yytext);
					    yyextra->g_guardName = yytext;
					    yyextra->g_lastGuardName.resize(0);
					    BEGIN(Start);
					  }
					  yyextra->g_expectGuard=FALSE;
  					}
<DefName>{ID}/{B}*			{ // define with content
  					  //printf("Define `%s'\n",yytext);
					  delete yyextra->g_argDict; yyextra->g_argDict=0;
					  yyextra->g_defArgs = -1;
                                          yyextra->g_defArgsStr.resize(0);
					  yyextra->g_defText.resize(0);
					  yyextra->g_defLitText.resize(0);
					  yyextra->g_defName = yytext;
					  yyextra->g_defVarArgs = FALSE;
					  QCString tmp=(QCString)"#define "+yyextra->g_defName+yyextra->g_defArgsStr;
					  outputArray(tmp.data(),tmp.length(), yyscanner);
					  yyextra->g_quoteArg=FALSE;
					  yyextra->g_insideComment=FALSE;
					  BEGIN(DefineText); 
  					}
<DefineArg>"\\\n"                       {
  					  yyextra->g_defExtraSpacing+="\n";
					  yyextra->g_yyLineNr++;
                                        }
<DefineArg>","{B}*			{ yyextra->g_defArgsStr+=yytext; }
<DefineArg>"("{B}*                      { yyextra->g_defArgsStr+=yytext; }
<DefineArg>{B}*")"{B}*			{
                                          yyextra->g_defArgsStr+=yytext; 
					  QCString tmp=(QCString)"#define "+yyextra->g_defName+yyextra->g_defArgsStr+yyextra->g_defExtraSpacing;
					  outputArray(tmp.data(),tmp.length(), yyscanner);
					  yyextra->g_quoteArg=FALSE;
					  yyextra->g_insideComment=FALSE;
  					  BEGIN(DefineText);
  					}
<DefineArg>"..."			{ // Variadic macro
					  yyextra->g_defVarArgs = TRUE;
					  yyextra->g_defArgsStr+=yytext;
					  yyextra->g_argDict->insert("__VA_ARGS__",new int(yyextra->g_defArgs));
					  yyextra->g_defArgs++;
  					}
<DefineArg>{ID}{B}*("..."?)		{
  					  //printf("Define addArg(%s)\n",yytext);
  					  QCString argName=yytext;
  					  yyextra->g_defVarArgs = yytext[yyleng-1]=='.';
					  if (yyextra->g_defVarArgs) // strip ellipsis
					  {
					    argName=argName.left(argName.length()-3);
					  }
					  argName = argName.stripWhiteSpace();
                                          yyextra->g_defArgsStr+=yytext;
					  yyextra->g_argDict->insert(argName,new int(yyextra->g_defArgs)); 
					  yyextra->g_defArgs++;
  					}
  /*
<DefineText>"/ **"|"/ *!"			{
  					  yyextra->g_defText+=yytext;
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_insideComment=TRUE;
  					}
<DefineText>"* /"			{
  					  yyextra->g_defText+=yytext;
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_insideComment=FALSE;
  					}
  */
<DefineText>"/*"[!*]?			{
					  yyextra->g_defText+=yytext;
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_lastCContext=YY_START;
					  yyextra->g_commentCount=1;
  					  BEGIN(CopyCComment);
  					}
<DefineText>"//"[!/]?			{
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					  yyextra->g_lastCPPContext=YY_START;
					  yyextra->g_defLitText+=' ';
  					  BEGIN(SkipCPPComment);
  					}
<SkipCComment>[/]?"*/"			{
  					  if (yytext[0]=='/') outputChar('/', yyscanner);
  					  outputChar('*', yyscanner);outputChar('/', yyscanner);
					  if (--yyextra->g_commentCount<=0)
					  {
					    if (yyextra->g_lastCContext==Start) 
					      // small hack to make sure that ^... rule will
					      // match when going to Start... Example: "/*...*/ some stuff..."
					    {
					      YY_CURRENT_BUFFER->yy_at_bol=1;
					    }
  					    BEGIN(yyextra->g_lastCContext);  
					  }
  					}
<SkipCComment>"//"("/")*		{
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<SkipCComment>"/*"			{
  					  outputChar('/', yyscanner);outputChar('*', yyscanner);
					  //yyextra->g_commentCount++;
  					}
<SkipCComment>[\\@][\\@]("f{"|"f$"|"f[") {
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<SkipCComment>^({B}*"*"+)?{B}{0,3}"~~~"[~]*   {
                                          static bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
  					    outputArray(yytext,(int)yyleng, yyscanner);
                                            yyextra->g_fenceSize=yyleng;
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>^({B}*"*"+)?{B}{0,3}"```"[`]*            {
                                          static bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
  					    outputArray(yytext,(int)yyleng, yyscanner);
                                            yyextra->g_fenceSize=yyleng;
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>[\\@][\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"code"("{"[^}]*"}")?){BN}+ {
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					  yyextra->g_yyLineNr+=QCString(yytext).contains('\n');
  					}
<SkipCComment>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"code"("{"[^}]*"}")?){BN}+	{
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					  yyextra->g_yyLineNr+=QCString(yytext).contains('\n');
                                          yyextra->g_fenceSize=0;
					  if (yytext[1]=='f')
					  {
					    yyextra->g_blockName="f";
					  }
					  else
					  {
                                            QCString bn=&yytext[1];
                                            int i = bn.find('{'); // for \code{.c}
                                            if (i!=-1) bn=bn.left(i);
					    yyextra->g_blockName=bn.stripWhiteSpace();
					  }
					  BEGIN(SkipVerbatim);
  					}
<SkipCComment,SkipCPPComment>[\\@][\\@]"cond"[ \t]+ { // escaped @cond
  					  outputArray(yytext,(int)yyleng, yyscanner);
                                        }
<SkipCPPComment>[\\@]"cond"[ \t]+	{ // conditional section
                                          yyextra->g_ccomment=TRUE;  
                                          yyextra->g_condCtx=YY_START;
  					  BEGIN(CondLineCpp);
  					}
<SkipCComment>[\\@]"cond"[ \t]+	{ // conditional section
                                          yyextra->g_ccomment=FALSE;  
                                          yyextra->g_condCtx=YY_START;
  					  BEGIN(CondLineC);
  					}
<CondLineC,CondLineCpp>[!()&| \ta-z_A-Z0-9\x80-\xFF.\-]+      {
  				          startCondSection(yytext, yyscanner);
                                          if (yyextra->g_skip)
                                          {
                                            if (YY_START==CondLineC)
                                            {
                                              // end C comment
  					      outputArray("*/",2, yyscanner);
                                              yyextra->g_ccomment=TRUE;
                                            }
                                            else
                                            {
                                              yyextra->g_ccomment=FALSE;
                                            }
                                            BEGIN(SkipCond);
                                          }
                                          else
                                          {
  					    BEGIN(yyextra->g_condCtx);
                                          }
  					}
<CondLineC,CondLineCpp>.		{ // non-guard character
  					  unput(*yytext);
  					  startCondSection(" ", yyscanner);
                                          if (yyextra->g_skip)
                                          {
                                            if (YY_START==CondLineC)
                                            {
                                              // end C comment
  					      outputArray("*/",2, yyscanner);
                                              yyextra->g_ccomment=TRUE;
                                            }
                                            else
                                            {
                                              yyextra->g_ccomment=FALSE;
                                            }
                                            BEGIN(SkipCond);
                                          }
                                          else
                                          {
					    BEGIN(yyextra->g_condCtx);
                                          }
  					}
<SkipCComment,SkipCPPComment>[\\@]"cond"[ \t\r]*/\n { // no guard
                                          if (YY_START==SkipCComment)
                                          {
                                            yyextra->g_ccomment=TRUE;
                                            // end C comment
  					    outputArray("*/",2, yyscanner);
                                          }
                                          else
                                          {
                                            yyextra->g_ccomment=FALSE;
                                          }
                                          yyextra->g_condCtx=YY_START;
                                          startCondSection(" ", yyscanner);
                                          BEGIN(SkipCond);
  					}
<SkipCond>\n                            { yyextra->g_yyLineNr++; outputChar('\n', yyscanner); }
<SkipCond>.                             { }
<SkipCond>[^\/\!*\\@\n]+                { }
<SkipCond>"//"[/!]                      { yyextra->g_ccomment=FALSE; }
<SkipCond>"/*"[*!]                      { yyextra->g_ccomment=TRUE; }
<SkipCond,SkipCComment,SkipCPPComment>[\\@][\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          if (!yyextra->g_skip)
                                          {
  					    outputArray(yytext,(int)yyleng, yyscanner);
                                          }
                                        }
<SkipCond>[\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF]  { 
                                          bool oldSkip = yyextra->g_skip;
                                          endCondSection(yyscanner); 
                                          if (oldSkip && !yyextra->g_skip)
                                          {
                                            if (yyextra->g_ccomment)
                                            {
                                              outputArray("/** ",4, yyscanner);
                                            }
                                            BEGIN(yyextra->g_condCtx);
                                          }
                                        }
<SkipCComment,SkipCPPComment>[\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          bool oldSkip = yyextra->g_skip;
  					  endCondSection(yyscanner);
                                          if (oldSkip && !yyextra->g_skip) 
                                          {
                                            BEGIN(yyextra->g_condCtx);
                                          }
  					}
<SkipVerbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"f$"|"f]"|"f}") { /* end of verbatim block */
  					  outputArray(yytext,(int)yyleng, yyscanner);
					  if (yytext[1]=='f' && yyextra->g_blockName=="f")
					  {
					    BEGIN(SkipCComment);
					  }
					  else if (&yytext[4]==yyextra->g_blockName)
					  {
					    BEGIN(SkipCComment);
					  }
  					}
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"~~~"[~]*                 {
  					  outputArray(yytext,(int)yyleng, yyscanner);
                                          if (yyextra->g_fenceSize==yyleng)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
  					  outputArray(yytext,(int)yyleng, yyscanner);
                                          if (yyextra->g_fenceSize==yyleng)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>"*/"|"/*"			{
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<SkipCComment,SkipVerbatim>[^*\\@\x06~`\n\/]+ {
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<SkipCComment,SkipVerbatim>\n		{ 
  					  yyextra->g_yyLineNr++;
  					  outputChar('\n', yyscanner);
  					}
<SkipCComment,SkipVerbatim>.		{
  					  outputChar(*yytext, yyscanner);
  					}
<CopyCComment>[^*a-z_A-Z\x80-\xFF\n]*[^*a-z_A-Z\x80-\xFF\\\n] {
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_defText+=escapeAt(yytext);
					}
<CopyCComment>\\[\r]?\n                 {
                                          yyextra->g_defLitText+=yytext;
                                          outputChar('\n', yyscanner);
                                          yyextra->g_defText+=" ";
                                          yyextra->g_yyLineNr++;
                                          yyextra->g_yyMLines++;
                                        }
<CopyCComment>"*/"			{
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_defText+=yytext;
  					  BEGIN(yyextra->g_lastCContext);
  					}
<CopyCComment>\n			{ 
  					  yyextra->g_yyLineNr++;
  					  outputChar('\n', yyscanner);
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_defText+=' ';
  					}
<RemoveCComment>"*/"{B}*"#"	        { // see bug 594021 for a usecase for this rule
                                          if (yyextra->g_lastCContext==SkipCPPBlock)
					  {
					    BEGIN(SkipCommand);
					  }
					  else
					  {
					    REJECT;
					  }
					}
<RemoveCComment>"*/"		        { BEGIN(yyextra->g_lastCContext); }
<RemoveCComment>"//"			
<RemoveCComment>"/*"
<RemoveCComment>[^*\x06\n]+
<RemoveCComment>\n			{ yyextra->g_yyLineNr++; outputChar('\n', yyscanner); }
<RemoveCComment>.			
<SkipCPPComment>[^\n\/\\@]+		{
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<SkipCPPComment,RemoveCPPComment>\n	{
  					  unput(*yytext);
  					  BEGIN(yyextra->g_lastCPPContext);
  					}
<SkipCPPComment>"/*"			{
  					  outputChar('/', yyscanner);outputChar('*', yyscanner);
  					}
<SkipCPPComment>"//"			{
  					  outputChar('/', yyscanner);outputChar('/', yyscanner);
  					}
<SkipCPPComment>[^\x06\@\\\n]+		{
  					  outputArray(yytext,(int)yyleng, yyscanner);
  					}
<SkipCPPComment>.			{
  					  outputChar(*yytext, yyscanner);
  					}
<RemoveCPPComment>"/*"
<RemoveCPPComment>"//"
<RemoveCPPComment>[^\x06\n]+
<RemoveCPPComment>.
<DefineText>"#"				{
  					  yyextra->g_quoteArg=TRUE;
					  yyextra->g_defLitText+=yytext;
  					}
<DefineText,CopyCComment>{ID}		{
					  yyextra->g_defLitText+=yytext;
  					  if (yyextra->g_quoteArg)
					  {
					    yyextra->g_defText+="\"";
					  }
					  if (yyextra->g_defArgs>0)
					  {
					    int *n;
					    if ((n=(*yyextra->g_argDict)[yytext]))
					    {
					      //if (!yyextra->g_quoteArg) yyextra->g_defText+=' ';
					      yyextra->g_defText+='@';
					      QCString numStr;
					      numStr.sprintf("%d",*n);
					      yyextra->g_defText+=numStr;
					      //if (!yyextra->g_quoteArg) yyextra->g_defText+=' ';
					    }
					    else
					    {
					      yyextra->g_defText+=yytext;
					    }
					  }
					  else
					  {
					    yyextra->g_defText+=yytext;
					  }
					  if (yyextra->g_quoteArg)
					  {
					    yyextra->g_defText+="\"";
					  }
					  yyextra->g_quoteArg=FALSE;
  					}
<CopyCComment>.				{
					  yyextra->g_defLitText+=yytext;
					  yyextra->g_defText+=yytext;
  					}
<DefineText>\\[\r]?\n			{
					  yyextra->g_defLitText+=yytext;
					  outputChar('\n', yyscanner);
					  yyextra->g_defText += ' ';
					  yyextra->g_yyLineNr++;
					  yyextra->g_yyMLines++;
					}
<DefineText>\n				{
					  QCString comment=extractTrailingComment(yyextra->g_defLitText);
					  yyextra->g_defLitText+=yytext;
					  if (!comment.isEmpty())
					  {
					    outputArray(comment,comment.length(), yyscanner);
					    yyextra->g_defLitText=yyextra->g_defLitText.left(yyextra->g_defLitText.length()-comment.length()-1);
					  }
  					  outputChar('\n', yyscanner);
  					  Define *def=0;
					  //printf("Define name=`%s' text=`%s' litTexti=`%s'\n",yyextra->g_defName.data(),yyextra->g_defText.data(),yyextra->g_defLitText.data());
					  if (yyextra->g_includeStack.isEmpty() || yyextra->g_curlyCount>0) 
					  {
					    addDefine(yyscanner);
					  }
					  def=DefineManager::instance().isDefined(yyextra->g_defName);
					  if (def==0) // new define
					  {
					    //printf("new define '%s'!\n",yyextra->g_defName.data());
					    Define *nd = newDefine(yyscanner);
					    DefineManager::instance().addDefine(yyextra->g_yyFileName,nd);

					    // also add it to the local file list if it is a source file
					    //if (yyextra->g_isSource && yyextra->g_includeStack.isEmpty())
					    //{
					    //  yyextra->g_fileDefineDict->insert(yyextra->g_defName,nd);
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
					      def->name = yyextra->g_defName;
					      def->definition = yyextra->g_defText.stripWhiteSpace();
					      def->nargs = yyextra->g_defArgs;
					      def->fileName = yyextra->g_yyFileName.copy(); 
					      def->lineNr = yyextra->g_yyLineNr-yyextra->g_yyMLines;
					      def->columnNr = yyextra->g_yyColNr;
					    }
					    else
					    {
					      //printf("error: define %s is defined more than once!\n",yyextra->g_defName.data());
					    }
					  }
					  delete yyextra->g_argDict; yyextra->g_argDict=0;
					  yyextra->g_yyLineNr++;
					  yyextra->g_yyColNr=1;
					  yyextra->g_lastGuardName.resize(0);
					  BEGIN(Start);
  					}
<DefineText>{B}*			{ yyextra->g_defText += ' '; yyextra->g_defLitText+=yytext; }
<DefineText>{B}*"##"{B}*		{ yyextra->g_defText += "##"; yyextra->g_defLitText+=yytext; }
<DefineText>"@"				{ yyextra->g_defText += "@@"; yyextra->g_defLitText+=yytext; }
<DefineText>\"				{ 
                                          yyextra->g_defText += *yytext; 
  					  yyextra->g_defLitText+=yytext; 
					  if (!yyextra->g_insideComment)
					  {
					    BEGIN(SkipDoubleQuote);
					  }
  					}
<DefineText>\'				{ yyextra->g_defText += *yytext;
  					  yyextra->g_defLitText+=yytext; 
					  if (!yyextra->g_insideComment)
					  {
  					    BEGIN(SkipSingleQuote);
					  }
					}
<SkipDoubleQuote>"//"[/]?		{ yyextra->g_defText += yytext; yyextra->g_defLitText+=yytext; }
<SkipDoubleQuote>"/*"			{ yyextra->g_defText += yytext; yyextra->g_defLitText+=yytext; }
<SkipDoubleQuote>\"			{
  					  yyextra->g_defText += *yytext; yyextra->g_defLitText+=yytext; 
					  BEGIN(DefineText);
  					}
<SkipSingleQuote,SkipDoubleQuote>\\.	{
  					  yyextra->g_defText += yytext; yyextra->g_defLitText+=yytext;
					}
<SkipSingleQuote>\'			{
  					  yyextra->g_defText += *yytext; yyextra->g_defLitText+=yytext;
					  BEGIN(DefineText);
  					}
<SkipDoubleQuote>.			{ yyextra->g_defText += *yytext; yyextra->g_defLitText+=yytext; }
<SkipSingleQuote>.			{ yyextra->g_defText += *yytext; yyextra->g_defLitText+=yytext; }
<DefineText>.				{ yyextra->g_defText += *yytext; yyextra->g_defLitText+=yytext; }
<<EOF>>					{
                                          DBG_CTX((stderr,"End of include file\n"));
					  //printf("Include stack depth=%d\n",yyextra->g_includeStack.count());
  					  if (yyextra->g_includeStack.isEmpty())
					  {
					    DBG_CTX((stderr,"Terminating scanner!\n"));
					    yyterminate();
					  }
					  else
					  {
					    FileState *fs=yyextra->g_includeStack.pop();
					    //fileDefineCache->merge(yyextra->g_yyFileName,fs->fileName);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->bufState, yyscanner );
					    yy_delete_buffer( oldBuf, yyscanner );
					    yyextra->g_yyLineNr    = fs->lineNr;
                                            //preYYin = fs->oldYYin;
                                            yyextra->g_inputBuf    = fs->oldFileBuf;
					    yyextra->g_inputBufPos = fs->oldFileBufPos;
					    setFileName(fs->fileName, yyscanner);
					    DBG_CTX((stderr,"######## FileName %s\n",yyextra->g_yyFileName.data()));
					    
                                            // Deal with file changes due to 
                                            // #include's within { .. } blocks
                                            QCString lineStr(15+yyextra->g_yyFileName.length());
                                            lineStr.sprintf("# %d \"%s\" 2",yyextra->g_yyLineNr,yyextra->g_yyFileName.data());
                                            outputArray(lineStr.data(),lineStr.length(), yyscanner);
					    
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
					    outputArray(yytext,(int)yyleng, yyscanner);
  					    yyextra->g_lastCContext=YY_START;
					    yyextra->g_commentCount=1;
					    if (yyleng==3) yyextra->g_lastGuardName.resize(0); // reset guard in case the #define is documented!
					    BEGIN(SkipCComment);
                                          }
  					}
<*>"//"[/]?				{
                                          if (YY_START==SkipVerbatim || YY_START==SkipCond || getLanguageFromFileName(yyextra->g_yyFileName)==SrcLangExt_Fortran)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
					    outputArray(yytext,(int)yyleng, yyscanner);
  					    yyextra->g_lastCPPContext=YY_START;
					    if (yyleng==3) yyextra->g_lastGuardName.resize(0); // reset guard in case the #define is documented!
					    BEGIN(SkipCPPComment);
                                          }
					}
<*>\n					{ 
  					  outputChar('\n', yyscanner);
  					  yyextra->g_yyLineNr++; 
					}
<*>.				        {
  					  yyextra->g_expectGuard = FALSE;
  					  outputChar(*yytext, yyscanner);
  					}

%%

/*@ ----------------------------------------------------------------------------
 */

static void setFileName(const char *name, yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  bool ambig;
  QFileInfo fi(name);
  preYYstate->g_yyFileName=fi.absFilePath().utf8();
  preYYstate->g_yyFileDef=findFileDef(preYYstate->doxygen->inputNameDict,preYYstate->g_yyFileName,ambig);
  if (preYYstate->g_yyFileDef==0) // if this is not an input file check if it is an
                      // include file
  {
    preYYstate->g_yyFileDef=findFileDef(preYYstate->doxygen->includeNameDict,preYYstate->g_yyFileName,ambig);
  }
  //printf("setFileName(%s) preYYstate->g_yyFileName=%s preYYstate->g_yyFileDef=%p\n",
  //    name,preYYstate->g_yyFileName.data(),preYYstate->g_yyFileDef);
  if (preYYstate->g_yyFileDef && preYYstate->g_yyFileDef->isReference()) preYYstate->g_yyFileDef=0;
  preYYstate->g_insideCS = getLanguageFromFileName(preYYstate->g_yyFileName)==SrcLangExt_CSharp;
  preYYstate->g_isSource = guessSection(preYYstate->g_yyFileName);
}

static void incrLevel(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  preYYstate->g_level++;
  preYYstate->g_levelGuard.resize(preYYstate->g_level);
  preYYstate->g_levelGuard[preYYstate->g_level-1]=FALSE;
  //printf("%s line %d: incrLevel %d\n",preYYstate->g_yyFileName.data(),preYYstate->g_yyLineNr,preYYstate->g_level);
}

static void decrLevel(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  //printf("%s line %d: decrLevel %d\n",preYYstate->g_yyFileName.data(),preYYstate->g_yyLineNr,preYYstate->g_level);
  if (preYYstate->g_level > 0)
  {
    preYYstate->g_level--;
    preYYstate->g_levelGuard.resize(preYYstate->g_level);
  }
  else
  {
    warn(preYYstate->g_yyFileName,preYYstate->g_yyLineNr,"More #endif's than #if's found.\n");
  }
}

static bool otherCaseDone(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  if (preYYstate->g_level==0)
  {
    warn(preYYstate->g_yyFileName,preYYstate->g_yyLineNr,"Found an #else without a preceding #if.\n");
    return TRUE;
  }
  else
  {
    return preYYstate->g_levelGuard[preYYstate->g_level-1];
  }
}

static void setCaseDone(bool value, yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  preYYstate->g_levelGuard[preYYstate->g_level-1]=value;
}

static FileState *checkAndOpenFile(const QCString &fileName,bool &alreadyIncluded, yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  alreadyIncluded = FALSE;
  FileState *fs = 0;
  //printf("checkAndOpenFile(%s)\n",fileName.data());
  QFileInfo fi(fileName);
  if (fi.exists() && fi.isFile())
  {
    static QStrList &exclPatterns = Config_getList(EXCLUDE_PATTERNS);
    if (patternMatch(fi,&exclPatterns)) return 0;

    QCString absName = fi.absFilePath().utf8();

    // global guard
    if (preYYstate->g_curlyCount==0) // not #include inside { ... }
    {
      if (preYYstate->g_allIncludes.find(absName)!=0)
      {
        alreadyIncluded = TRUE;
        //printf("  already included 1\n");
        return 0; // already done
      }
      preYYstate->g_allIncludes.insert(absName,(void *)0x8);
    }
    // check include stack for absName

    QStack<FileState> tmpStack;
    preYYstate->g_includeStack.setAutoDelete(FALSE);
    while ((fs=preYYstate->g_includeStack.pop()))
    {
      if (fs->fileName==absName) alreadyIncluded=TRUE;
      tmpStack.push(fs);
    }
    while ((fs=tmpStack.pop()))
    {
      preYYstate->g_includeStack.push(fs);
    }
    preYYstate->g_includeStack.setAutoDelete(TRUE);

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
      fs->oldFileBuf    = preYYstate->g_inputBuf;
      fs->oldFileBufPos = preYYstate->g_inputBufPos;
    }
  }
  return fs;
}

static FileState *findFile(const char *fileName,bool localInclude,bool &alreadyIncluded, yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  //printf("** findFile(%s,%d) preYYstate->g_yyFileName=%s\n",fileName,localInclude,preYYstate->g_yyFileName.data());
  if (portable_isAbsolutePath(fileName))
  {
    FileState *fs = checkAndOpenFile(fileName,alreadyIncluded,yyscanner);
    if (fs)
    {
      setFileName(fileName,yyscanner);
      preYYstate->g_yyLineNr=1;
      return fs;
    }
    else if (alreadyIncluded)
    {
      return 0;
    }
  }
  if (localInclude && !preYYstate->g_yyFileName.isEmpty())
  {
    QFileInfo fi(preYYstate->g_yyFileName);
    if (fi.exists())
    {
      QCString absName = QCString(fi.dirPath(TRUE).data())+"/"+fileName;
      FileState *fs = checkAndOpenFile(absName,alreadyIncluded, yyscanner);
      if (fs)
      {
	setFileName(absName, yyscanner);
	preYYstate->g_yyLineNr=1;
	return fs;
      }
      else if (alreadyIncluded)
      {
	return 0;
      }
    }
  }
  if (preYYstate->g_pathList==0) 
  {
    return 0;
  }
  char *s=preYYstate->g_pathList->first();
  while (s)
  {
    QCString absName = (QCString)s+"/"+fileName;
    //printf("  Looking for %s in %s\n",fileName,s);
    FileState *fs = checkAndOpenFile(absName,alreadyIncluded, yyscanner);
    if (fs)
    {
      setFileName(absName, yyscanner);
      preYYstate->g_yyLineNr=1;
      //printf("  -> found it\n");
      return fs;
    }
    else if (alreadyIncluded)
    {
      return 0;
    }

    s=preYYstate->g_pathList->next();
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

static int getNextChar(const QCString &expr,QCString *rest,uint &pos,yyscan_t yyscanner)
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
 
static int getCurrentChar(const QCString &expr,QCString *rest,uint pos, yyscan_t yyscanner)
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
    returnCharToStream(cc, yyscanner);
    //unput((char)cc);
    //printf("%c=yyinput()\n",cc);
    return cc;
  }
}

static void unputChar(const QCString &expr,QCString *rest,uint &pos,char c, yyscan_t yyscanner)
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
    returnCharToStream(c, yyscanner);
  }
  //printf("result: unputChar(%s,%s,%d,%c)\n",expr.data(),rest ? rest->data() : 0,pos,c);
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

static void returnCharToStream(char c, yyscan_t yyscanner)
{
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
  yyunput( c, yyg->yytext_ptr , yyscanner );
}

static void addTillEndOfString(const QCString &expr,QCString *rest,
                                       uint &pos,char term,QCString &arg,yyscan_t yyscanner)
{
  int cc;
  while ((cc=getNextChar(expr,rest,pos, yyscanner))!=EOF && cc!=0)
  {
    if (cc=='\\') arg+=(char)cc,cc=getNextChar(expr,rest,pos,yyscanner);
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
static bool replaceFunctionMacro(const QCString &expr,QCString *rest,int pos,int &len,const Define *def,QCString &result, yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  //printf("replaceFunctionMacro(expr=%s,rest=%s,pos=%d,def=%s) level=%d\n",expr.data(),rest ? rest->data() : 0,pos,def->name.data(),preYYstate->g_level);
  uint j=pos;
  len=0;
  result.resize(0);
  int cc;
  while ((cc=getCurrentChar(expr,rest,j,yyscanner))!=EOF && isspace(cc)) 
  { 
    len++; 
    getNextChar(expr,rest,j,yyscanner); 
  }
  if (cc!='(') 
  { 
    unputChar(expr,rest,j,' ', yyscanner); 
    return FALSE; 
  }
  getNextChar(expr,rest,j,yyscanner); // eat the `(' character

  QDict<QCString> argTable;  // list of arguments
  argTable.setAutoDelete(TRUE);
  QCString arg;
  int argCount=0;
  bool done=FALSE;
  
  // PHASE 1: read the macro arguments
  if (def->nargs==0)
  {
    while ((cc=getNextChar(expr,rest,j,yyscanner))!=EOF && cc!=0)
    {
      char c = (char)cc;
      if (c==')') break;
    }
  }
  else
  {
    while (!done && (argCount<def->nargs || def->varArgs) && 
	((cc=getNextChar(expr,rest,j,yyscanner))!=EOF && cc!=0)
	  )
    {
      char c=(char)cc;
      if (c=='(') // argument is a function => search for matching )
      {
	int level=1;
	arg+=c;
	//char term='\0';
	while ((cc=getNextChar(expr,rest,j,yyscanner))!=EOF && cc!=0)
	{
	  char c=(char)cc;
	  //printf("processing %c: term=%c (%d)\n",c,term,term);
	  if (c=='\'' || c=='\"') // skip ('s and )'s inside strings
	  {
	    arg+=c;
	    addTillEndOfString(expr,rest,j,c,arg,yyscanner);
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
	while (!found && (cc=getNextChar(expr,rest,j,yyscanner))!=EOF && cc!=0)
	{
	  found = cc=='"';
	  if (cc=='\\')
	  {
	    c=(char)cc;	  
	    arg+=c;
	    if ((cc=getNextChar(expr,rest,j,yyscanner))==EOF || cc==0) break;
	  }
	  c=(char)cc;	  
	  arg+=c;
	}
      }
      else if (c=='\'') // append literal characters
      {
	arg+=c;
	bool found=FALSE;
	while (!found && (cc=getNextChar(expr,rest,j,yyscanner))!=EOF && cc!=0)
	{
	  found = cc=='\'';
	  if (cc=='\\')
	  {
	    c=(char)cc;	  
	    arg+=c;
	    if ((cc=getNextChar(expr,rest,j,yyscanner))==EOF || cc==0) break;
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
  if (argCount==def->nargs || // same number of arguments
      (argCount>=def->nargs-1 && def->varArgs)) // variadic macro with at least as many
                                                // params as the non-variadic part (see bug731985)
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
	    if (!hash) expandExpression(substArg,0,0,yyscanner);
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
	      else if (preYYstate->g_nospaces)
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

/*! performs recursive macro expansion on the string \a expr
 *  starting at position \a pos.
 *  May read additional characters from the input while re-scanning!
 *  If \a expandAll is \c TRUE then all macros in the expression are
 *  expanded, otherwise only the first is expanded.
 */
static void expandExpression(QCString &expr,QCString *rest,int pos,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
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
      if (preYYstate->g_expandedDict->find(macroName)==0) // expand macro
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
	  if (preYYstate->g_nospaces)
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
	  replaced=replaceFunctionMacro(expr,rest,p+l,len,def,expMacro, yyscanner);
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
	    preYYstate->g_expandedDict->insert(macroName,def);
	    expandExpression(resultExpr,&restExpr,0, yyscanner);
	    preYYstate->g_expandedDict->remove(macroName);
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
      else if (c=='d' && !inNum) // identifier starting with a `d'
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

static bool computeExpression(const QCString &expr,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  QCString e=expr;
  expandExpression(e,0,0,yyscanner);
  //printf("after expansion `%s'\n",e.data());
  e = removeIdsAndMarkers(e);
  if (e.isEmpty()) return FALSE;
  //printf("parsing `%s'\n",e.data());
  return parseconstexp(preYYstate->g_yyFileName,preYYstate->g_yyLineNr,e);
}

/*! expands the macro definition in \a name
 *  If needed the function may read additional characters from the input
 */

static QCString expandMacro(const QCString &name,yyscan_t yyscanner)
{
  QCString n=name;
  expandExpression(n,0,0,yyscanner);
  n=removeMarkers(n);
  //printf("expandMacro `%s'->`%s'\n",name.data(),n.data());
  return n;
}

static Define *newDefine(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  Define *def=new Define;
  def->name       = preYYstate->g_defName;
  def->definition = preYYstate->g_defText.stripWhiteSpace();
  def->nargs      = preYYstate->g_defArgs;
  def->fileName   = preYYstate->g_yyFileName; 
  def->fileDef    = preYYstate->g_yyFileDef;
  def->lineNr     = preYYstate->g_yyLineNr-preYYstate->g_yyMLines;
  def->columnNr   = preYYstate->g_yyColNr;
  def->varArgs    = preYYstate->g_defVarArgs;
  //printf("newDefine: %s %s file: %s\n",def->name.data(),def->definition.data(),
  //    def->fileDef ? def->fileDef->name().data() : def->fileName.data());
  //printf("newDefine: `%s'->`%s'\n",def->name.data(),def->definition.data());
  if (!def->name.isEmpty() && preYYstate->doxygen->expandAsDefinedDict[def->name])
  {
    def->isPredefined=TRUE;
  }
  return def;
}

static void addDefine(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  if (preYYstate->g_skip) return; // do not add this define as it is inside a 
                      // conditional section (cond command) that is disabled.
  if (!preYYstate->doxygen->gatherDefines) return;

  //printf("addDefine %s %s\n",preYYstate->g_defName.data(),preYYstate->g_defArgsStr.data());
  //ArgumentList *al = new ArgumentList;
  //stringToArgumentList(preYYstate->g_defArgsStr,al);
  MemberDef *md=new MemberDef(
      preYYstate->g_yyFileName,preYYstate->g_yyLineNr-preYYstate->g_yyMLines,preYYstate->g_yyColNr,
      "#define",preYYstate->g_defName,preYYstate->g_defArgsStr,0,
      Public,Normal,FALSE,Member,MemberType_Define,0,0);
  if (!preYYstate->g_defArgsStr.isEmpty())
  {
    ArgumentList *argList = new ArgumentList;
    //printf("addDefine() preYYstate->g_defName=`%s' preYYstate->g_defArgsStr=`%s'\n",preYYstate->g_defName.data(),preYYstate->g_defArgsStr.data());
    stringToArgumentList(preYYstate->g_defArgsStr,argList);
    md->setArgumentList(argList);
  }
  //printf("Setting initializer for `%s' to `%s'\n",preYYstate->g_defName.data(),preYYstate->g_defText.data());
  int l=preYYstate->g_defLitText.find('\n');
  if (l>0 && preYYstate->g_defLitText.left(l).stripWhiteSpace()=="\\")
  {
    // strip first line if it only contains a slash
    preYYstate->g_defLitText = preYYstate->g_defLitText.right(preYYstate->g_defLitText.length()-l-1);
  }
  else if (l>0)
  {
    // align the items on the first line with the items on the second line
    int k=l+1;
    const char *p=preYYstate->g_defLitText.data()+k;
    char c;
    while ((c=*p++) && (c==' ' || c=='\t')) k++;
    preYYstate->g_defLitText=preYYstate->g_defLitText.mid(l+1,k-l-1)+preYYstate->g_defLitText.stripWhiteSpace();
  }
  md->setInitializer(preYYstate->g_defLitText.stripWhiteSpace());

  //printf("pre.l: md->setFileDef(%p)\n",preYYstate->g_inputFileDef);
  md->setFileDef(preYYstate->g_inputFileDef);
  md->setDefinition("#define "+preYYstate->g_defName);

  MemberName *mn=preYYstate->doxygen->functionNameSDict->find(preYYstate->g_defName);
  if (mn==0)
  {
    mn = new MemberName(preYYstate->g_defName);
    preYYstate->doxygen->functionNameSDict->append(preYYstate->g_defName,mn);
  }
  mn->append(md);
  if (preYYstate->g_yyFileDef) 
  {
    preYYstate->g_yyFileDef->insertMember(md);
  }

  //Define *d;
  //if ((d=defineDict[preYYstate->g_defName])==0) defineDict.insert(preYYstate->g_defName,newDefine()); 
}

static inline void outputChar(char c,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  if (preYYstate->g_includeStack.isEmpty() || preYYstate->g_curlyCount>0) preYYstate->g_outputBuf->addChar(c);
}

static inline void outputArray(const char *a,int len,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  if (preYYstate->g_includeStack.isEmpty() || preYYstate->g_curlyCount>0) preYYstate->g_outputBuf->addArray(a,len);
}

static void readIncludeFile(const QCString &inc,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
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

    QCString oldFileName = preYYstate->g_yyFileName;
    FileDef *oldFileDef  = preYYstate->g_yyFileDef;
    int oldLineNr        = preYYstate->g_yyLineNr;
    //printf("Searching for `%s'\n",incFileName.data());

    // absIncFileName avoids difficulties for incFileName starting with "../" (bug 641336)
    QCString absIncFileName = incFileName;
    {
      QFileInfo fi(preYYstate->g_yyFileName);
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
    DefineManager::instance().addInclude(preYYstate->g_yyFileName,absIncFileName);
    DefineManager::instance().addFileToContext(absIncFileName);

    // findFile will overwrite preYYstate->g_yyFileDef if found
    FileState *fs;
    bool alreadyIncluded = FALSE;
    //printf("calling findFile(%s)\n",incFileName.data());
    if ((fs=findFile(incFileName,localInclude,alreadyIncluded,yyscanner))) // see if the include file can be found
    {
      //printf("Found include file!\n");
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        for (i=0;i<preYYstate->g_includeStack.count();i++) 
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
        FileDef *incFd = findFileDef(preYYstate->doxygen->inputNameDict,absIncFileName,ambig);
        oldFileDef->addIncludeDependency(ambig ? 0 : incFd,incFileName,localInclude,preYYstate->g_isImported,FALSE);
        // add included by dependency
        if (preYYstate->g_yyFileDef)
        {
          //printf("Adding include dependency %s->%s\n",oldFileDef->name().data(),incFileName.data());
          preYYstate->g_yyFileDef->addIncludedByDependency(oldFileDef,oldFileDef->docName(),localInclude,preYYstate->g_isImported);
        }
      }
      else if (preYYstate->g_inputFileDef)
      {
        preYYstate->g_inputFileDef->addIncludeDependency(0,absIncFileName,localInclude,preYYstate->g_isImported,TRUE);
      }
      fs->bufState = YY_CURRENT_BUFFER;
      fs->lineNr   = oldLineNr;
      fs->fileName = oldFileName;
      // push the state on the stack
      preYYstate->g_includeStack.push(fs);
      // set the scanner to the include file

      // Deal with file changes due to 
      // #include's within { .. } blocks
      QCString lineStr(preYYstate->g_yyFileName.length()+20);
      lineStr.sprintf("# 1 \"%s\" 1\n",preYYstate->g_yyFileName.data());
      outputArray(lineStr.data(),lineStr.length(), yyscanner);

      DBG_CTX((stderr,"Switching to include file %s\n",incFileName.data()));
      preYYstate->g_expectGuard=TRUE;
      preYYstate->g_inputBuf   = &fs->fileBuf;
      preYYstate->g_inputBufPos=0;
      yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner), yyscanner);
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
	FileDef *fd = findFileDef(preYYstate->doxygen->inputNameDict,absIncFileName,ambig);
	//printf("%s::findFileDef(%s)=%p\n",oldFileDef->name().data(),incFileName.data(),fd);
	// add include dependency to the file in which the #include was found
	oldFileDef->addIncludeDependency(ambig ? 0 : fd,incFileName,localInclude,preYYstate->g_isImported,FALSE);
	// add included by dependency
        if (fd)
        {
          //printf("Adding include dependency (2) %s->%s ambig=%d\n",oldFileDef->name().data(),fd->name().data(),ambig);
          fd->addIncludedByDependency(oldFileDef,oldFileDef->docName(),localInclude,preYYstate->g_isImported);
        }
      }
      else if (preYYstate->g_inputFileDef)
      {
        preYYstate->g_inputFileDef->addIncludeDependency(0,absIncFileName,localInclude,preYYstate->g_isImported,TRUE);
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
      if (preYYstate->g_curlyCount>0 && !alreadyIncluded) // failed to find #include inside { ... }
      {
	warn(preYYstate->g_yyFileName,preYYstate->g_yyLineNr,"include file %s not found, perhaps you forgot to add its directory to INCLUDE_PATH?",incFileName.data());
      }
    }
  }
}

/* ----------------------------------------------------------------- */

static void startCondSection(const char *sectId,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  //printf("startCondSection: skip=%d stack=%d\n",preYYstate->g_skip,preYYstate->g_condStack.count());
  CondParser prs;
  bool expResult = prs.parse(preYYstate->g_yyFileName,preYYstate->g_yyLineNr,sectId);
  preYYstate->g_condStack.push(new CondCtx(preYYstate->g_yyLineNr,sectId,preYYstate->g_skip));
  if (!expResult)
  {
    preYYstate->g_skip=TRUE;
  }
  //printf("  expResult=%d skip=%d\n",expResult,preYYstate->g_skip);
}

static void endCondSection(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  if (preYYstate->g_condStack.isEmpty())
  {
    preYYstate->g_skip=FALSE;
  }
  else
  {
    CondCtx *ctx = preYYstate->g_condStack.pop();
    preYYstate->g_skip=ctx->skip;
    delete ctx;
  }
  //printf("endCondSection: skip=%d stack=%d\n",preYYstate->g_skip,preYYstate->g_condStack.count());
}

static void forceEndCondSection(yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  while (!preYYstate->g_condStack.isEmpty())
  {
    delete preYYstate->g_condStack.pop();
  }
  preYYstate->g_skip=FALSE;
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

static int yyread(char *buf,int max_size,yyscan_t yyscanner)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  int bytesInBuf = preYYstate->g_inputBuf->curPos()-preYYstate->g_inputBufPos;
  int bytesToCopy = QMIN(max_size,bytesInBuf);
  memcpy(buf,preYYstate->g_inputBuf->data()+preYYstate->g_inputBufPos,bytesToCopy);
  preYYstate->g_inputBufPos+=bytesToCopy;
  return bytesToCopy;
}

/* ----------------------------------------------------------------- */

static yyscan_t yyscanner;
static struct preYY_state pre_extra;

void initPreprocessor()
{
  preYYlex_init_extra(&pre_extra, &yyscanner);
  pre_extra.g_pathList = new QStrList;
  addSearchDir(".");
  pre_extra.g_expandedDict = new DefineDict(17);
}

void cleanUpPreprocessor()
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  delete preYYstate->g_expandedDict;
  preYYstate->g_expandedDict=0;

  delete preYYstate->g_pathList;
  preYYstate->g_pathList=0;

  DefineManager::deleteInstance();
#if defined(YY_FLEX_SUBMINOR_VERSION) 
  if (preYYstate->g_lexInit)
  {
    preYYlex_destroy(yyscanner);
  }
#endif
}

void addSearchDir(const char *dir)
{
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  QFileInfo fi(dir);
  if (fi.isDir()) preYYstate->g_pathList->append(fi.absFilePath().utf8());
} 

void preprocessFile(const char *fileName,BufStr &input,BufStr &output)
{
  // needed by BEGIN
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  struct preYY_state *preYYstate = yyget_extra(yyscanner);
  printlex(yyget_debug(yyscanner), TRUE, __FILE__, fileName);
  uint orgOffset=output.curPos();
  //printf("##########################\n%s\n####################\n",
  //    input.data());

  preYYstate->g_macroExpansion = Config_getBool(MACRO_EXPANSION);
  preYYstate->g_expandOnlyPredef = Config_getBool(EXPAND_ONLY_PREDEF);
  preYYstate->g_skip=FALSE;
  preYYstate->g_curlyCount=0;
  preYYstate->g_nospaces=FALSE;
  preYYstate->g_inputBuf=&input;
  preYYstate->g_inputBufPos=0;
  preYYstate->g_outputBuf=&output;
  preYYstate->g_includeStack.setAutoDelete(TRUE);
  preYYstate->g_includeStack.clear();
  preYYstate->g_expandedDict->setAutoDelete(FALSE);
  preYYstate->g_expandedDict->clear();
  preYYstate->g_condStack.setAutoDelete(TRUE);
  preYYstate->g_condStack.clear();
  //preYYstate->g_fileDefineDict->clear();

  setFileName(fileName, yyscanner);
  preYYstate->g_inputFileDef = preYYstate->g_yyFileDef;
  DefineManager::instance().startContext(preYYstate->g_yyFileName);
  
  static bool firstTime=TRUE;
  if (firstTime)
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

      if (i_obrace==0) continue; // no define name

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
	  def->fileDef      = preYYstate->g_yyFileDef;
	  def->fileName     = fileName;
	  DefineManager::instance().addDefine(preYYstate->g_yyFileName,def);
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
	  def->fileDef      = preYYstate->g_yyFileDef;
	  def->fileName     = fileName;
	  DefineManager::instance().addDefine(preYYstate->g_yyFileName,def);
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
 
  preYYstate->g_yyLineNr = 1;
  preYYstate->g_yyColNr  = 1;
  preYYstate->g_level    = 0;
  preYYstate->g_ifcount  = 0;

  BEGIN( Start );
  
  preYYstate->g_expectGuard = guessSection(fileName)==Entry::HEADER_SEC;
  preYYstate->g_guardName.resize(0);
  preYYstate->g_lastGuardName.resize(0);
  preYYstate->g_guardExpr.resize(0);

  yylex (yyscanner);
  preYYstate->g_lexInit=TRUE;

  while (!preYYstate->g_condStack.isEmpty())
  {
    CondCtx *ctx = preYYstate->g_condStack.pop();
    QCString sectionInfo = " ";
    if (ctx->sectionId!=" ") sectionInfo.sprintf(" with label %s ",ctx->sectionId.data()); 
    warn(fileName,ctx->lineNr,"Conditional section%sdoes not have "
	"a corresponding \\endcond command within this file.",sectionInfo.data());
    delete ctx;
  }
  // make sure we don't extend a \cond with missing \endcond over multiple files (see bug 624829)
  forceEndCondSection(yyscanner);

  // remove locally defined macros so they can be redefined in another source file
  //if (preYYstate->g_fileDefineDict->count()>0)
  //{
  //  QDictIterator<Define> di(*preYYstate->g_fileDefineDict);
  //  Define *d;
  //  for (di.toFirst();(d=di.current());++di)
  //  {
  //    preYYstate->g_globalDefineDict->remove(di.currentKey());
  //  }
  //  preYYstate->g_fileDefineDict->clear();
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
        Debug::print(Debug::Preprocessor,0,"%s ",qPrint(def->name));
      }
      Debug::print(Debug::Preprocessor,0,"\n---------\n");
    }
    else
    {
      Debug::print(Debug::Preprocessor,0,"No macros accessible in this file.\n");
    }
  }
  DefineManager::instance().endContext();
  printlex(yyget_debug(yyscanner), FALSE, __FILE__, fileName);
}

void preFreeScanner()
{
}

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
//  int  preYYwrap() { return 1 ; }
  void preYYdummy() { yy_flex_realloc(0,0); } 
}
#endif

