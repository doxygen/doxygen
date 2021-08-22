/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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
%top{
#include <stdint.h>
// forward declare yyscan_t to improve type safety
#define YY_TYPEDEF_YY_SCANNER_T
struct yyguts_t;
typedef yyguts_t *yyscan_t;
}

%{

/*
 *      includes
 */

#include "doxygen.h"

#include <stack>
#include <deque>
#include <algorithm>
#include <utility>
#include <mutex>
#include <thread>
#include <algorithm>

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

#include "qcstring.h"
#include "containers.h"
#include "pre.h"
#include "constexp.h"
#include "define.h"
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
#include "regex.h"
#include "fileinfo.h"

#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

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
  FileState(uint size) : fileBuf(size) {}
  int lineNr = 1;
  int curlyCount = 0;
  BufStr fileBuf;
  BufStr *oldFileBuf = 0;
  yy_size_t oldFileBufPos = 0;
  YY_BUFFER_STATE bufState = 0;
  QCString fileName;
};

struct PreIncludeInfo
{
  PreIncludeInfo(const QCString &fn,FileDef *srcFd, FileDef *dstFd,const QCString &iName,bool loc, bool imp)
    : fileName(fn), fromFileDef(srcFd), toFileDef(dstFd), includeName(iName), local(loc), imported(imp)
  {
  }
  QCString fileName;    // file name in which the include statement was found
  FileDef *fromFileDef; // filedef in which the include statement was found
  FileDef *toFileDef;   // filedef to which the include is pointing
  QCString includeName; // name used in the #include statement
  bool local;           // is it a "local" or <global> include
  bool imported;        // include via "import" keyword (Objective-C)
};

/** A dictionary of managed Define objects. */
typedef std::map< std::string, Define > DefineMap;

/** @brief Class that manages the defines available while
 *  preprocessing files.
 */
class DefineManager
{
  private:
    /** Local class used to hold the defines for a single file */
    class DefinesPerFile
    {
      public:
        /** Creates an empty container for defines */
        DefinesPerFile(DefineManager *parent)
          : m_parent(parent)
        {
        }
        void addInclude(std::string fileName)
        {
          m_includedFiles.insert(fileName);
        }
        void store(const DefineMap &fromMap)
        {
          for (auto &kv : fromMap)
          {
            m_defines.emplace(kv.first,kv.second);
          }
          //printf("  m_defines.size()=%zu\n",m_defines.size());
          m_stored=true;
        }
        void retrieve(DefineMap &toMap)
        {
          StringSet includeStack;
          retrieveRec(toMap,includeStack);
        }
        void retrieveRec(DefineMap &toMap,StringSet &includeStack)
        {
          //printf("  retrieveRec #includedFiles=%zu\n",m_includedFiles.size());
          for (auto incFile : m_includedFiles)
          {
            DefinesPerFile *dpf = m_parent->find(incFile);
            if (dpf && includeStack.find(incFile)==includeStack.end())
            {
              includeStack.insert(incFile);
              dpf->retrieveRec(toMap,includeStack);
              //printf("  retrieveRec: processing include %s: #toMap=%zu\n",qPrint(incFile),toMap.size());
            }
          }
          for (auto &kv : m_defines)
          {
            toMap.emplace(kv.first,kv.second);
          }
        }
        bool stored() const { return m_stored; }
      private:
        DefineManager *m_parent;
        DefineMap m_defines;
        StringSet m_includedFiles;
        bool m_stored = false;
    };

    friend class DefinesPerFile;
  public:

    void addInclude(std::string fromFileName,std::string toFileName)
    {
      //printf("DefineManager::addInclude('%s'->'%s')\n",fromFileName.c_str(),toFileName.c_str());
      auto it = m_fileMap.find(fromFileName);
      if (it==m_fileMap.end())
      {
        it = m_fileMap.emplace(fromFileName,std::make_unique<DefinesPerFile>(this)).first;
      }
      auto &dpf = it->second;
      dpf->addInclude(toFileName);
    }

    void store(std::string fileName,const DefineMap &fromMap)
    {
      //printf("DefineManager::store(%s,#=%zu)\n",fileName.c_str(),fromMap.size());
      auto it = m_fileMap.find(fileName);
      if (it==m_fileMap.end())
      {
        it = m_fileMap.emplace(fileName,std::make_unique<DefinesPerFile>(this)).first;
      }
      it->second->store(fromMap);
    }

    void retrieve(std::string fileName,DefineMap &toMap)
    {
      auto it = m_fileMap.find(fileName);
      if (it!=m_fileMap.end())
      {
        auto &dpf = it->second;
        dpf->retrieve(toMap);
      }
      //printf("DefineManager::retrieve(%s,#=%zu)\n",fileName.c_str(),toMap.size());
    }

    bool alreadyProcessed(std::string fileName) const
    {
      auto it = m_fileMap.find(fileName);
      if (it!=m_fileMap.end())
      {
        return it->second->stored();
      }
      return false;
    }

  private:
    /** Helper function to return the DefinesPerFile object for a given file name. */
    DefinesPerFile *find(std::string fileName) const
    {
      auto it = m_fileMap.find(fileName);
      return it!=m_fileMap.end() ? it->second.get() : nullptr;
    }

    std::unordered_map< std::string, std::unique_ptr<DefinesPerFile> > m_fileMap;
};


/* -----------------------------------------------------------------
 *
 *      global state
 */
static std::mutex            g_debugMutex;
static std::mutex            g_globalDefineMutex;
static std::mutex            g_updateGlobals;
static DefineManager         g_defineManager;


/* -----------------------------------------------------------------
 *
 *      scanner's state
 */

struct preYY_state
{
  int                yyLineNr       = 1;
  int                yyMLines       = 1;
  int                yyColNr        = 1;
  QCString           yyFileName;
  FileDef           *yyFileDef      = 0;
  FileDef           *inputFileDef   = 0;
  int                ifcount        = 0;
  int                defArgs        = -1;
  QCString           defName;
  QCString           defText;
  QCString           defLitText;
  QCString           defArgsStr;
  QCString           defExtraSpacing;
  bool               defContinue = false;
  bool               defVarArgs     = false;
  int                lastCContext   = 0;
  int                lastCPPContext = 0;
  BufStr            *inputBuf       = 0;
  yy_size_t          inputBufPos    = 0;
  BufStr            *outputBuf      = 0;
  int                roundCount     = 0;
  bool               quoteArg       = false;
  int                findDefArgContext = 0;
  bool               expectGuard    = false;
  QCString           guardName;
  QCString           lastGuardName;
  QCString           incName;
  QCString           guardExpr;
  int                curlyCount     = 0;
  bool               nospaces       = false; // add extra spaces during macro expansion

  bool               macroExpansion = false; // from the configuration
  bool               expandOnlyPredef = false; // from the configuration
  QCString           potentialDefine;
  int                commentCount   = 0;
  bool               insideComment  = false;
  bool               isImported     = false;
  QCString           blockName;
  int                condCtx        = 0;
  bool               skip           = false;
  bool               insideCS       = false; // C# has simpler preprocessor
  bool               insideFtn      = false;
  bool               isSource       = false;

  yy_size_t          fenceSize      = 0;
  bool               ccomment       = false;
  QCString           delimiter;
  bool               isSpecialComment = false;
  StringVector                             pathList;
  IntMap                                   argMap;
  BoolStack                                levelGuard;
  std::stack< std::unique_ptr<CondCtx> >   condStack;
  std::deque< std::unique_ptr<FileState> > includeStack;
  std::unordered_map<std::string,Define*>  expandedDict;
  StringUnorderedSet                       expanded;
  ConstExpressionParser                    constExpParser;
  DefineMap                                contextDefines; // macros imported from other files
  DefineMap                                localDefines;   // macros defined in this file
  DefineList                               macroDefinitions;
  LinkedMap<PreIncludeInfo>                includeRelations;
};

// stateless functions
static QCString escapeAt(const QCString &text);
static QCString extractTrailingComment(const QCString &s);
static char resolveTrigraph(char c);

// stateful functions
static inline void  outputArray(yyscan_t yyscanner,const char *a,yy_size_t len);
static inline void outputString(yyscan_t yyscanner,const QCString &s);
static inline void   outputChar(yyscan_t yyscanner,char c);
static inline void outputSpaces(yyscan_t yyscanner,char *s);
static inline void  outputSpace(yyscan_t yyscanner,char c);
static inline void extraSpacing(yyscan_t yyscanner);
static QCString     expandMacro(yyscan_t yyscanner,const QCString &name);
static void     readIncludeFile(yyscan_t yyscanner,const QCString &inc);
static void           incrLevel(yyscan_t yyscanner);
static void           decrLevel(yyscan_t yyscanner);
static void         setCaseDone(yyscan_t yyscanner,bool value);
static bool       otherCaseDone(yyscan_t yyscanner);
static bool   computeExpression(yyscan_t yyscanner,const QCString &expr);
static void    startCondSection(yyscan_t yyscanner,const QCString &sectId);
static void      endCondSection(yyscan_t yyscanner);
static void  addMacroDefinition(yyscan_t yyscanner);
static void           addDefine(yyscan_t yyscanner);
static void         setFileName(yyscan_t yyscanner,const QCString &name);
static yy_size_t         yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);
static Define *       isDefined(yyscan_t yyscanner,const QCString &name);

/* ----------------------------------------------------------------- */

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

/* ----------------------------------------------------------------- */

%}

IDSTART [a-z_A-Z\x80-\xFF]
ID      {IDSTART}[a-z_A-Z0-9\x80-\xFF]*
B       [ \t]
Bopt    {B}*
BN      [ \t\r\n]
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^'\\\n]{1,4}"'"))

  // C start comment 
CCS   "/\*"
  // C end comment
CCE   "*\/"
  // Cpp comment 
CPPC  "/\/"
  // optional characters after import
ENDIMPORTopt [^\\\n]*
  // Optional white space
WSopt [ \t\r]*

%option noyywrap

%x      Start
%x      Command
%x      SkipCommand
%x      SkipLine
%x      SkipString
%x      CopyLine
%x      LexCopyLine
%x      CopyString
%x      CopyStringCs
%x      CopyStringFtn
%x      CopyStringFtnDouble
%x      CopyRawString
%x      Include
%x      IncludeID
%x      EndImport
%x      DefName
%x      DefineArg
%x      DefineText
%x      SkipCPPBlock
%x      Ifdef
%x      Ifndef
%x      SkipCComment
%x      ArgCopyCComment
%x      CopyCComment
%x      SkipVerbatim
%x      SkipCPPComment
%x      RemoveCComment
%x      RemoveCPPComment
%x      Guard
%x      DefinedExpr1
%x      DefinedExpr2
%x      SkipDoubleQuote
%x      SkipSingleQuote
%x      UndefName
%x      IgnoreLine
%x      FindDefineArgs
%x      ReadString
%x      CondLineC
%x      CondLineCpp
%x      SkipCond

%%

<*>\x06                                 
<*>\x00
<*>\r
<*>"??"[=/'()!<>-]                      { // Trigraph
                                          unput(resolveTrigraph(yytext[2]));
                                        }
<Start>^{B}*"#"                         {
                                          yyextra->yyColNr+=(int)yyleng;
                                          yyextra->yyMLines=0;
                                          yyextra->potentialDefine=yytext;
                                          BEGIN(Command); 
                                        }
<Start>^("%top{"|"%{")                  {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Lex) REJECT
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(LexCopyLine);
                                        }
<Start>^{Bopt}/[^#]                     {
                                          outputArray(yyscanner,yytext,yyleng);
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
                                          int pos = name.find('(');
                                          if (pos<0) pos=0; // should never happen
                                          name=name.left(pos).stripWhiteSpace();

                                          Define *def=0;
                                          if (skipFuncMacros && !yyextra->insideFtn &&
                                              name!="Q_PROPERTY" &&
                                              !(
                                                 (yyextra->includeStack.empty() || yyextra->curlyCount>0) &&
                                                 yyextra->macroExpansion &&
                                                 (def=isDefined(yyscanner,name)) &&
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
<CopyLine,LexCopyLine>"extern"{BN}{0,80}"\"C\""*{BN}{0,80}"{"   {
                                          QCString text=yytext;
                                          yyextra->yyLineNr+=text.contains('\n');
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyLine,LexCopyLine>{RAWBEGIN}        {
                                          yyextra->delimiter = yytext+2;
                                          yyextra->delimiter=yyextra->delimiter.left(yyextra->delimiter.length()-1);
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(CopyRawString);
                                        }
<CopyLine,LexCopyLine>"{"               { // count brackets inside the main file
                                          if (yyextra->includeStack.empty())
                                          {
                                            yyextra->curlyCount++;
                                          }
                                          outputChar(yyscanner,*yytext);
                                        }
<LexCopyLine>^"%}"                      {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyLine,LexCopyLine>"}"               { // count brackets inside the main file
                                          if (yyextra->includeStack.empty() && yyextra->curlyCount>0)
                                          {
                                            yyextra->curlyCount--;
                                          }
                                          outputChar(yyscanner,*yytext);
                                        }
<CopyLine,LexCopyLine>"'"\\[0-7]{1,3}"'" {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyLine,LexCopyLine>"'"\\."'"         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyLine,LexCopyLine>"'"."'"           {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyLine,LexCopyLine>@\"               {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_CSharp) REJECT;
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN( CopyStringCs );
                                        }
<CopyLine,LexCopyLine>\"                {
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
<CopyLine,LexCopyLine>\'                {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyStringFtn );
                                        }
<CopyString>[^\"\\\r\n]+                {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringCs>[^\"\r\n]+                {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyString>\\.                         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyString,CopyStringCs>\"             {
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyLine );
                                        }
<CopyStringFtnDouble>[^\"\\\r\n]+       {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringFtnDouble>\\.                {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringFtnDouble>\"                 {
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyLine );
                                        }
<CopyStringFtn>[^\'\\\r\n]+             {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringFtn>\\.                      {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringFtn>\'                       {
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyLine );
                                        }
<CopyRawString>{RAWEND}                 {
                                          outputArray(yyscanner,yytext,yyleng);
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==yyextra->delimiter)
                                          {
                                            BEGIN( CopyLine );
                                          }
                                        }
<CopyRawString>[^)]+                    {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyRawString>.                        {
                                          outputChar(yyscanner,*yytext);
                                        }
<CopyLine,LexCopyLine>{ID}/{BN}{0,80}"("        {
                                          yyextra->expectGuard = FALSE;
                                          Define *def=0;
                                          //def=yyextra->globalDefineDict->find(yytext);
                                          //def=isDefined(yyscanner,yytext);
                                          //printf("Search for define %s found=%d yyextra->includeStack.empty()=%d "
                                          //       "yyextra->curlyCount=%d yyextra->macroExpansion=%d yyextra->expandOnlyPredef=%d "
                                          //     "isPreDefined=%d\n",yytext,def ? 1 : 0,
                                          //     yyextra->includeStack.empty(),yyextra->curlyCount,yyextra->macroExpansion,yyextra->expandOnlyPredef,
                                          //     def ? def->isPredefined : -1
                                          //    );
                                          if ((yyextra->includeStack.empty() || yyextra->curlyCount>0) &&
                                              yyextra->macroExpansion &&
                                              (def=isDefined(yyscanner,yytext)) &&
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
                                              outputString(yyscanner,result);
                                            }
                                            else // zero or more arguments
                                            {
                                              yyextra->findDefArgContext = CopyLine;
                                              BEGIN(FindDefineArgs);
                                            }
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                          }
                                        }
<CopyLine,LexCopyLine>{ID}              {
                                          Define *def=0;
                                          if ((yyextra->includeStack.empty() || yyextra->curlyCount>0) &&
                                              yyextra->macroExpansion &&
                                              (def=isDefined(yyscanner,yytext)) &&
                                              def->nargs==-1 &&
                                              /*(def->isPredefined || macroIsAccessible(def)) &&*/
                                              (!yyextra->expandOnlyPredef || def->isPredefined)
                                             )
                                          {
                                            QCString result=def->isPredefined ? def->definition : expandMacro(yyscanner,yytext);
                                            outputString(yyscanner,result);
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                          }
                                        }
<CopyLine,LexCopyLine>"\\"\r?/\n        { // strip line continuation characters
                                          if (getLanguageFromFileName(yyextra->yyFileName)==SrcLangExt_Fortran) outputChar(yyscanner,*yytext);
                                        }
<CopyLine,LexCopyLine>\\.               {
                                          outputArray(yyscanner,yytext,(int)yyleng);
                                        }
<CopyLine,LexCopyLine>.                 {
                                          outputChar(yyscanner,*yytext);
                                        }
<CopyLine,LexCopyLine>\n                {
                                          outputChar(yyscanner,'\n');
                                          BEGIN(Start);
                                          yyextra->yyLineNr++;
                                          yyextra->yyColNr=1;
                                        }
<FindDefineArgs>"("                     {
                                          yyextra->defArgsStr+='(';
                                          yyextra->roundCount++;
                                        }
<FindDefineArgs>")"                     {
                                          yyextra->defArgsStr+=')';
                                          yyextra->roundCount--;
                                          if (yyextra->roundCount==0)
                                          {
                                            QCString result=expandMacro(yyscanner,yyextra->defArgsStr);
                                            //printf("yyextra->defArgsStr='%s'->'%s'\n",qPrint(yyextra->defArgsStr),qPrint(result));
                                            if (yyextra->findDefArgContext==CopyLine)
                                            {
                                              outputString(yyscanner,result);
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
<FindDefineArgs>")"{B}*"("              {
                                          yyextra->defArgsStr+=yytext;
                                        }
  */
<FindDefineArgs>{CHARLIT}               {
                                          yyextra->defArgsStr+=yytext;
                                        }
<FindDefineArgs>{CCS}[*]?                {
                                          yyextra->defArgsStr+=yytext;
                                          BEGIN(ArgCopyCComment);
                                        }
<FindDefineArgs>\"                      {
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(ReadString);
                                        }
<FindDefineArgs>'                       {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(ReadString);
                                        }
<FindDefineArgs>\n                      {
                                          yyextra->defArgsStr+=' ';
                                          yyextra->yyLineNr++;
                                          outputChar(yyscanner,'\n');
                                        }
<FindDefineArgs>"@"                     {
                                          yyextra->defArgsStr+="@@";
                                        }
<FindDefineArgs>.                       {
                                          yyextra->defArgsStr+=*yytext;
                                        }
<ArgCopyCComment>[^*\n]+                {
                                          yyextra->defArgsStr+=yytext;
                                        }
<ArgCopyCComment>{CCE}                  {
                                          yyextra->defArgsStr+=yytext;
                                          BEGIN(FindDefineArgs);
                                        }
<ArgCopyCComment>\n                     {
                                          yyextra->defArgsStr+=' ';
                                          yyextra->yyLineNr++;
                                          outputChar(yyscanner,'\n');
                                        }
<ArgCopyCComment>.                      {
                                          yyextra->defArgsStr+=yytext;
                                        }
<ReadString>"\""                        {
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(FindDefineArgs);
                                        }
<ReadString>"'"                         {
                                          if (getLanguageFromFileName(yyextra->yyFileName)!=SrcLangExt_Fortran) REJECT;
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(FindDefineArgs);
                                        }

<ReadString>{CPPC}|{CCS}                        {
                                          yyextra->defArgsStr+=yytext;
                                        }
<ReadString>\\/\r?\n                    { // line continuation
                                        }
<ReadString>\\.                         {
                                          yyextra->defArgsStr+=yytext;
                                        }
<ReadString>.                           {
                                          yyextra->defArgsStr+=*yytext;
                                        }
<Command>("include"|"import"){B}+/{ID}  {
                                          yyextra->isImported = yytext[1]=='m';
                                          if (yyextra->macroExpansion)
                                            BEGIN(IncludeID);
                                        }
<Command>("include"|"import"){B}*[<"]   {
                                          yyextra->isImported = yytext[1]=='m';
                                          char c[2];
                                          c[0]=yytext[yyleng-1];c[1]='\0';
                                          yyextra->incName=c;
                                          BEGIN(Include);
                                        }
<Command>("cmake")?"define"{B}+         {
                                          yyextra->potentialDefine += substitute(yytext,"cmake","     ");
                                          //printf("!!!DefName\n");
                                          yyextra->yyColNr+=(int)yyleng;
                                          BEGIN(DefName);
                                        }
<Command>"ifdef"/{B}*"("                {
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr.resize(0);
                                          BEGIN(DefinedExpr2);
                                        }
<Command>"ifdef"/{B}+                   {
                                          //printf("Pre.l: ifdef\n");
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr.resize(0);
                                          BEGIN(DefinedExpr1);
                                        }
<Command>"ifndef"/{B}*"("               {
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr="! ";
                                          BEGIN(DefinedExpr2);
                                        }
<Command>"ifndef"/{B}+                  {
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr="! ";
                                          BEGIN(DefinedExpr1);
                                        }
<Command>"if"/[ \t(!]                   {
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr.resize(0);
                                          BEGIN(Guard);
                                        }
<Command>("elif"|"else"{B}*"if")/[ \t(!]        {
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
<Command>"else"/[^a-z_A-Z0-9\x80-\xFF]          {
                                          if (otherCaseDone(yyscanner))
                                          {
                                            yyextra->ifcount=0;
                                            BEGIN(SkipCPPBlock);
                                          }
                                          else
                                          {
                                            setCaseDone(yyscanner,TRUE);
                                          }
                                        }
<Command>"undef"{B}+                    {
                                          BEGIN(UndefName);
                                        }
<Command>("elif"|"else"{B}*"if")/[ \t(!]        {
                                          if (!otherCaseDone(yyscanner))
                                          {
                                            yyextra->guardExpr.resize(0);
                                            BEGIN(Guard);
                                          }
                                        }
<Command>"endif"/[^a-z_A-Z0-9\x80-\xFF]         {
                                          //printf("Pre.l: #endif\n");
                                          decrLevel(yyscanner);
                                        }
<Command,IgnoreLine>\n                  {
                                          outputChar(yyscanner,'\n');
                                          BEGIN(Start);
                                          yyextra->yyLineNr++;
                                        }
<Command>"pragma"{B}+"once"             {
                                          yyextra->expectGuard = FALSE;
                                        }
<Command>{ID}                           { // unknown directive
                                          BEGIN(IgnoreLine);
                                        }
<IgnoreLine>\\[\r]?\n                   {
                                          outputChar(yyscanner,'\n');
                                          yyextra->yyLineNr++;
                                        }
<IgnoreLine>.
<Command>.                              { yyextra->potentialDefine += yytext[0]=='\t' ? '\t' : ' ';
                                          yyextra->yyColNr+=(int)yyleng;
                                        }
<UndefName>{ID}                         {
                                          Define *def;
                                          if ((def=isDefined(yyscanner,yytext))
                                              /*&& !def->isPredefined*/
                                              && !def->nonRecursive
                                             )
                                          {
                                            //printf("undefining %s\n",yytext);
                                            def->undef=TRUE;
                                          }
                                          BEGIN(Start);
                                        }
<Guard>\\[\r]?\n                        {
                                          outputChar(yyscanner,'\n');
                                          yyextra->guardExpr+=' ';
                                          yyextra->yyLineNr++;
                                        }
<Guard>"defined"/{B}*"("                {
                                          BEGIN(DefinedExpr2);
                                        }
<Guard>"defined"/{B}+                   {
                                          BEGIN(DefinedExpr1);
                                        }
<Guard>{ID}                             { yyextra->guardExpr+=yytext; }
<Guard>"@"                              { yyextra->guardExpr+="@@"; }
<Guard>.                                { yyextra->guardExpr+=*yytext; }
<Guard>\n                               {
                                          unput(*yytext);
                                          //printf("Guard: '%s'\n",
                                          //    qPrint(yyextra->guardExpr));
                                          bool guard=computeExpression(yyscanner,yyextra->guardExpr);
                                          setCaseDone(yyscanner,guard);
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
<DefinedExpr1,DefinedExpr2>\\\n         { yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<DefinedExpr1>{ID}                      {
                                          if (isDefined(yyscanner,yytext) || yyextra->guardName==yytext)
                                            yyextra->guardExpr+=" 1L ";
                                          else
                                            yyextra->guardExpr+=" 0L ";
                                          yyextra->lastGuardName=yytext;
                                          BEGIN(Guard);
                                        }
<DefinedExpr2>{ID}                      {
                                          if (isDefined(yyscanner,yytext) || yyextra->guardName==yytext)
                                            yyextra->guardExpr+=" 1L ";
                                          else
                                            yyextra->guardExpr+=" 0L ";
                                          yyextra->lastGuardName=yytext;
                                        }
<DefinedExpr1,DefinedExpr2>\n           { // should not happen, handle anyway
                                          yyextra->yyLineNr++;
                                          yyextra->ifcount=0;
                                          BEGIN(SkipCPPBlock);
                                        }
<DefinedExpr2>")"                       {
                                          BEGIN(Guard);
                                        }
<DefinedExpr1,DefinedExpr2>.
<SkipCPPBlock>^{B}*"#"                  { BEGIN(SkipCommand); }
<SkipCPPBlock>^{Bopt}/[^#]              { BEGIN(SkipLine); }
<SkipCPPBlock>\n                        { yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<SkipCPPBlock>.
<SkipCommand>"if"(("n")?("def"))?/[ \t(!] {
                                          incrLevel(yyscanner);
                                          yyextra->ifcount++;
                                          //printf("#if... depth=%d\n",yyextra->ifcount);
                                        }
<SkipCommand>"else"                     {
                                          //printf("Else! yyextra->ifcount=%d otherCaseDone=%d\n",yyextra->ifcount,otherCaseDone());
                                          if (yyextra->ifcount==0 && !otherCaseDone(yyscanner))
                                          {
                                            setCaseDone(yyscanner,TRUE);
                                            //outputChar(yyscanner,'\n');
                                            BEGIN(Start);
                                          }
                                        }
<SkipCommand>("elif"|"else"{B}*"if")/[ \t(!]            {
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
<SkipCommand>"endif"                    {
                                          yyextra->expectGuard = FALSE;
                                          decrLevel(yyscanner);
                                          if (--yyextra->ifcount<0)
                                          {
                                            //outputChar(yyscanner,'\n');
                                            BEGIN(Start);
                                          }
                                        }
<SkipCommand>\n                         {
                                          outputChar(yyscanner,'\n');
                                          yyextra->yyLineNr++;
                                          BEGIN(SkipCPPBlock);
                                        }
<SkipCommand>{ID}                       { // unknown directive
                                          BEGIN(SkipLine);
                                        }
<SkipCommand>.
<SkipLine>[^'"/\n]+                     
<SkipLine>{CHARLIT}                     { }
<SkipLine>\"                            {
                                          BEGIN(SkipString);
                                        }
<SkipLine>.
<SkipString>{CPPC}/[^\n]*                 {
                                        }
<SkipLine,SkipCommand,SkipCPPBlock>{CPPC}[^\n]* {
                                          yyextra->lastCPPContext=YY_START;
                                          BEGIN(RemoveCPPComment);
                                        }
<SkipString>{CCS}/[^\n]*                 {
                                        }
<SkipLine,SkipCommand,SkipCPPBlock>{CCS}/[^\n]* {
                                          yyextra->lastCContext=YY_START;
                                          BEGIN(RemoveCComment);
                                        }
<SkipLine>\n                            {
                                          outputChar(yyscanner,'\n');
                                          yyextra->yyLineNr++;
                                          BEGIN(SkipCPPBlock);
                                        }
<SkipString>[^"\\\n]+                   { }
<SkipString>\\.                         { }
<SkipString>\"                          {
                                          BEGIN(SkipLine);
                                        }
<SkipString>.                           { }
<IncludeID>{ID}{Bopt}/"("                       {
                                          yyextra->nospaces=TRUE;
                                          yyextra->roundCount=0;
                                          yyextra->defArgsStr=yytext;
                                          yyextra->findDefArgContext = IncludeID;
                                          BEGIN(FindDefineArgs);
                                        }
<IncludeID>{ID}                         {
                                          yyextra->nospaces=TRUE;
                                          readIncludeFile(yyscanner,expandMacro(yyscanner,yytext));
                                          BEGIN(Start);
                                        }
<Include>[^\">\n]+[\">]                 {
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
<EndImport>{ENDIMPORTopt}/\n                    {
                                          BEGIN(Start);
                                        }
<EndImport>\\[\r]?"\n"                  {
                                          outputChar(yyscanner,'\n');
                                          yyextra->yyLineNr++;
                                        }
<EndImport>.                            {
                                        }
<DefName>{ID}/("\\\n")*"("              { // define with argument
                                          //printf("Define() '%s'\n",yytext);
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = 0;
                                          yyextra->defArgsStr.resize(0);
                                          yyextra->defText.resize(0);
                                          yyextra->defLitText.resize(0);
                                          yyextra->defName = yytext;
                                          yyextra->defVarArgs = FALSE;
                                          yyextra->defExtraSpacing.resize(0);
                                          yyextra->defContinue = false;
                                          BEGIN(DefineArg);
                                        }
<DefName>{ID}{B}+"1"/[ \r\t\n]          { // special case: define with 1 -> can be "guard"
                                          //printf("Define '%s'\n",yytext);
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = -1;
                                          yyextra->defArgsStr.resize(0);
                                          yyextra->defName = QCString(yytext).left(yyleng-1).stripWhiteSpace();
                                          yyextra->defVarArgs = FALSE;
                                          //printf("Guard check: %s!=%s || %d\n",
                                          //    qPrint(yyextra->defName),qPrint(yyextra->lastGuardName),yyextra->expectGuard);
                                          if (yyextra->curlyCount>0 || yyextra->defName!=yyextra->lastGuardName || !yyextra->expectGuard)
                                          { // define may appear in the output
                                            QCString def = yyextra->potentialDefine +
                                                           yyextra->defName         ;
                                            outputString(yyscanner,def);
                                            outputSpaces(yyscanner,yytext+yyextra->defName.length());
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
<DefName>{ID}/{B}*"\n"                  { // empty define
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = -1;
                                          yyextra->defName = yytext;
                                          yyextra->defArgsStr.resize(0);
                                          yyextra->defText.resize(0);
                                          yyextra->defLitText.resize(0);
                                          yyextra->defVarArgs = FALSE;
                                          //printf("Guard check: %s!=%s || %d\n",
                                          //    qPrint(yyextra->defName),qPrint(yyextra->lastGuardName),yyextra->expectGuard);
                                          if (yyextra->curlyCount>0 || yyextra->defName!=yyextra->lastGuardName || !yyextra->expectGuard)
                                          { // define may appear in the output
                                            QCString def = yyextra->potentialDefine + yyextra->defName;
                                            outputString(yyscanner,def);
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
<DefName>{ID}/{B}*                      { // define with content
                                          //printf("Define '%s'\n",yytext);
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = -1;
                                          yyextra->defArgsStr.resize(0);
                                          yyextra->defText.resize(0);
                                          yyextra->defLitText.resize(0);
                                          yyextra->defName = yytext;
                                          yyextra->defVarArgs = FALSE;
                                          QCString def = yyextra->potentialDefine +
                                                         yyextra->defName         +
                                                         yyextra->defArgsStr      ;
                                          outputString(yyscanner,def);
                                          yyextra->quoteArg=FALSE;
                                          yyextra->insideComment=FALSE;
                                          BEGIN(DefineText);
                                        }
<DefineArg>"\\\n"                       {
                                          yyextra->defExtraSpacing+="\n";
                                          yyextra->defContinue = true;
                                          yyextra->yyLineNr++;
                                        }
<DefineArg>{B}*                         { yyextra->defExtraSpacing+=yytext; }
<DefineArg>","{B}*                      { yyextra->defArgsStr+=yytext; }
<DefineArg>"("{B}*                      { yyextra->defArgsStr+=yytext; }
<DefineArg>{B}*")"{B}*                  {
                                          extraSpacing(yyscanner);
                                          yyextra->defArgsStr+=yytext;
                                          QCString def = yyextra->potentialDefine +
                                                         yyextra->defName         +
                                                         yyextra->defArgsStr      +
                                                         yyextra->defExtraSpacing ;
                                          outputString(yyscanner,def);
                                          yyextra->quoteArg=FALSE;
                                          yyextra->insideComment=FALSE;
                                          BEGIN(DefineText);
                                        }
<DefineArg>"..."                        { // Variadic macro
                                          yyextra->defVarArgs = TRUE;
                                          yyextra->defArgsStr+=yytext;
                                          yyextra->argMap.emplace(std::string("__VA_ARGS__"),yyextra->defArgs);
                                          yyextra->defArgs++;
                                        }
<DefineArg>{ID}{B}*("..."?)             {
                                          //printf("Define addArg(%s)\n",yytext);
                                          QCString argName=yytext;
                                          yyextra->defVarArgs = yytext[yyleng-1]=='.';
                                          if (yyextra->defVarArgs) // strip ellipsis
                                          {
                                            argName=argName.left(argName.length()-3);
                                          }
                                          argName = argName.stripWhiteSpace();
                                          yyextra->defArgsStr+=yytext;
                                          yyextra->argMap.emplace(toStdString(argName),yyextra->defArgs);
                                          yyextra->defArgs++;
                                          extraSpacing(yyscanner);
                                        }
  /*
<DefineText>"/ **"|"/ *!"                       {
                                          yyextra->defText+=yytext;
                                          yyextra->defLitText+=yytext;
                                          yyextra->insideComment=TRUE;
                                        }
<DefineText>"* /"                       {
                                          yyextra->defText+=yytext;
                                          yyextra->defLitText+=yytext;
                                          yyextra->insideComment=FALSE;
                                        }
  */
<DefineText>{CCS}[!*]?                  {
                                          yyextra->defText+=yytext;
                                          yyextra->defLitText+=yytext;
                                          yyextra->lastCContext=YY_START;
                                          yyextra->commentCount=1;
                                          BEGIN(CopyCComment);
                                        }
<DefineText>{CPPC}[!/]?                 {
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->lastCPPContext=YY_START;
                                          yyextra->defLitText+=' ';
                                          BEGIN(SkipCPPComment);
                                        }
<SkipCComment>[/]?{CCE}                 {
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
<SkipCComment>{CPPC}("/")*              {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCComment>{CCS}                     {
                                          outputChar(yyscanner,'/');outputChar(yyscanner,'*');
                                          //yyextra->commentCount++;
                                        }
<SkipCComment>[\\@][\\@]("f{"|"f$"|"f[""f(") {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCComment>^({B}*"*"+)?{B}{0,3}"~~~"[~]*   {
                                          bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport || !yyextra->isSpecialComment)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                            yyextra->fenceSize=(int)yyleng;
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>^({B}*"*"+)?{B}{0,3}"```"[`]*            {
                                          bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport || !yyextra->isSpecialComment)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                            yyextra->fenceSize=(int)yyleng;
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>[\\@][\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"code"("{"[^}]*"}")?){BN}+ {
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                        }
<SkipCComment>[\\@]("verbatim"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"code"("{"[^}]*"}")?){BN}+      {
                                          outputArray(yyscanner,yytext,yyleng);
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
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCPPComment>[\\@]"cond"[ \t]+       { // conditional section
                                          yyextra->ccomment=TRUE;
                                          yyextra->condCtx=YY_START;
                                          BEGIN(CondLineCpp);
                                        }
<SkipCComment>[\\@]"cond"[ \t]+ { // conditional section
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
<CondLineC,CondLineCpp>.                { // non-guard character
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
<SkipCComment,SkipCPPComment>[\\@]"cond"{WSopt}/\n { // no guard
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
<SkipCond>{CPPC}[/!]                      { yyextra->ccomment=FALSE; }
<SkipCond>{CCS}[*!]                      { yyextra->ccomment=TRUE; }
<SkipCond,SkipCComment,SkipCPPComment>[\\@][\\@]"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          if (!yyextra->skip)
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
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
<SkipVerbatim>[\\@]("endverbatim"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"enddot"|"endcode"|"f$"|"f]"|"f}""f}") { /* end of verbatim block */
                                          outputArray(yyscanner,yytext,yyleng);
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
                                          outputArray(yyscanner,yytext,yyleng);
                                          if (yyextra->fenceSize==(yy_size_t)yyleng)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
                                          outputArray(yyscanner,yytext,yyleng);
                                          if (yyextra->fenceSize==(yy_size_t)yyleng)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>{CCE}|{CCS}                       {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCComment,SkipVerbatim>[^*\\@\x06~`\n\/]+ {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCComment,SkipVerbatim>\n           {
                                          yyextra->yyLineNr++;
                                          outputChar(yyscanner,'\n');
                                        }
<SkipCComment,SkipVerbatim>.            {
                                          outputChar(yyscanner,*yytext);
                                        }
<CopyCComment>[^*a-z_A-Z\x80-\xFF\n]*[^*a-z_A-Z\x80-\xFF\\\n] {
                                          yyextra->defLitText+=yytext;
                                          yyextra->defText+=escapeAt(yytext);
                                        }
<CopyCComment>\\[\r]?\n                 {
                                          yyextra->defLitText+=yytext;
                                          yyextra->defText+=" ";
                                          yyextra->yyLineNr++;
                                          yyextra->yyMLines++;
                                        }
<CopyCComment>{CCE}                     {
                                          yyextra->defLitText+=yytext;
                                          yyextra->defText+=yytext;
                                          BEGIN(yyextra->lastCContext);
                                        }
<CopyCComment>\n                        {
                                          yyextra->yyLineNr++;
                                          yyextra->defLitText+=yytext;
                                          yyextra->defText+=' ';
                                        }
<RemoveCComment>{CCE}{B}*"#"            { // see bug 594021 for a usecase for this rule
                                          if (yyextra->lastCContext==SkipCPPBlock)
                                          {
                                            BEGIN(SkipCommand);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<RemoveCComment>{CCE}                   { BEGIN(yyextra->lastCContext); }
<RemoveCComment>{CPPC}                  
<RemoveCComment>{CCS}
<RemoveCComment>[^*\x06\n]+
<RemoveCComment>\n                      { yyextra->yyLineNr++; outputChar(yyscanner,'\n'); }
<RemoveCComment>.                       
<SkipCPPComment>[^\n\/\\@]+             {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCPPComment,RemoveCPPComment>\n     {
                                          unput(*yytext);
                                          BEGIN(yyextra->lastCPPContext);
                                        }
<SkipCPPComment>{CCS}                   {
                                          outputChar(yyscanner,'/');outputChar(yyscanner,'*');
                                        }
<SkipCPPComment>{CPPC}                  {
                                          outputChar(yyscanner,'/');outputChar(yyscanner,'/');
                                        }
<SkipCPPComment>[^\x06\@\\\n]+          {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCPPComment>.                       {
                                          outputChar(yyscanner,*yytext);
                                        }
<RemoveCPPComment>{CCS}
<RemoveCPPComment>{CPPC}
<RemoveCPPComment>[^\x06\n]+
<RemoveCPPComment>.
<DefineText>"#"/{IDSTART}               {
                                          outputChar(yyscanner,' ');
                                          yyextra->quoteArg=TRUE;
                                          yyextra->defLitText+=yytext;
                                        }
<DefineText,CopyCComment>{ID}           {
                                          yyextra->defLitText+=yytext;
                                          if (YY_START == DefineText) outputSpaces(yyscanner,yytext);
                                          if (yyextra->quoteArg)
                                          {
                                            yyextra->defText+="\"";
                                          }
                                          if (yyextra->defArgs>0)
                                          {
                                            auto it = yyextra->argMap.find(yytext);
                                            if (it!=yyextra->argMap.end())
                                            {
                                              int n = it->second;
                                              yyextra->defText+='@';
                                              yyextra->defText+=QCString().setNum(n);
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
<CopyCComment>.                         {
                                          yyextra->defLitText+=yytext;
                                          yyextra->defText+=yytext;
                                        }
<DefineText>\\[\r]?\n                   {
                                          yyextra->defLitText+=yytext;
                                          outputChar(yyscanner,'\n');
                                          yyextra->defText += ' ';
                                          yyextra->yyLineNr++;
                                          yyextra->yyMLines++;
                                        }
<DefineText>\n                          {
                                          QCString comment=extractTrailingComment(yyextra->defLitText);
                                          yyextra->defText = yyextra->defText.stripWhiteSpace();
                                          if (yyextra->defText.startsWith("##"))
                                          {
                                            warn(yyextra->yyFileName,yyextra->yyLineNr,
                                                 "'##' cannot occur at the beginning of a macro definition '%s': '%s'",
                                                 qPrint(yyextra->defName),qPrint(yyextra->defLitText.stripWhiteSpace()));
                                          }
                                          else if (yyextra->defText.endsWith("##"))
                                          {
                                            warn(yyextra->yyFileName,yyextra->yyLineNr,
                                                 "'##' cannot occur at the end of a macro definition '%s': '%s'",
                                                 qPrint(yyextra->defName),qPrint(yyextra->defLitText.stripWhiteSpace()));
                                          }
                                          else if (yyextra->defText.endsWith("#"))
                                          {
                                            warn(yyextra->yyFileName,yyextra->yyLineNr,
                                                 "expected formal parameter after # in macro definition '%s': '%s'",
                                                 qPrint(yyextra->defName),qPrint(yyextra->defLitText.stripWhiteSpace()));
                                          }
                                          yyextra->defLitText+=yytext;
                                          if (!comment.isEmpty())
                                          {
                                            outputString(yyscanner,comment);
                                            yyextra->defLitText=yyextra->defLitText.left(yyextra->defLitText.length()-comment.length()-1);
                                          }
                                          outputChar(yyscanner,'\n');
                                          Define *def=0;
                                          //printf("Define name='%s' text='%s' litTexti='%s'\n",qPrint(yyextra->defName),qPrint(yyextra->defText),qPrint(yyextra->defLitText));
                                          if (yyextra->includeStack.empty() || yyextra->curlyCount>0)
                                          {
                                            addMacroDefinition(yyscanner);
                                          }
                                          def=isDefined(yyscanner,yyextra->defName);
                                          if (def==0) // new define
                                          {
                                            //printf("new define '%s'!\n",qPrint(yyextra->defName));
                                            addDefine(yyscanner);
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
                                              def->fileName = yyextra->yyFileName;
                                              def->lineNr = yyextra->yyLineNr-yyextra->yyMLines;
                                              def->columnNr = yyextra->yyColNr;
                                            }
                                            else
                                            {
                                              //printf("error: define %s is defined more than once!\n",qPrint(yyextra->defName));
                                            }
                                          }
                                          yyextra->argMap.clear();
                                          yyextra->yyLineNr++;
                                          yyextra->yyColNr=1;
                                          yyextra->lastGuardName.resize(0);
                                          BEGIN(Start);
                                        }
<DefineText>{B}*                        { outputString(yyscanner,yytext);
                                          yyextra->defText += ' ';
                                          yyextra->defLitText+=yytext;
                                        }
<DefineText>{B}*"##"{B}*                { outputString(yyscanner,substitute(yytext,"##","  "));
                                          yyextra->defText += "##";
                                          yyextra->defLitText+=yytext;
                                        }
<DefineText>"@"                         { outputString(yyscanner,substitute(yytext,"@@","  "));
                                          yyextra->defText += "@@";
                                          yyextra->defLitText+=yytext;
                                        }
<DefineText>\"                          {
                                          outputChar(yyscanner,' ');
                                          yyextra->defText += *yytext;
                                          yyextra->defLitText+=yytext;
                                          if (!yyextra->insideComment)
                                          {
                                            BEGIN(SkipDoubleQuote);
                                          }
                                        }
<DefineText>\'                          {
                                          outputChar(yyscanner,' ');
                                          yyextra->defText += *yytext;
                                          yyextra->defLitText+=yytext;
                                          if (!yyextra->insideComment)
                                          {
                                            BEGIN(SkipSingleQuote);
                                          }
                                        }
<SkipDoubleQuote>{CPPC}[/]?             { outputSpaces(yyscanner,yytext);
                                          yyextra->defText += yytext;
                                          yyextra->defLitText+=yytext;
                                        }
<SkipDoubleQuote>{CCS}[*]?              { outputSpaces(yyscanner,yytext);
                                          yyextra->defText += yytext;
                                          yyextra->defLitText+=yytext;
                                        }
<SkipDoubleQuote>\"                     {
                                          outputChar(yyscanner,' ');
                                          yyextra->defText += *yytext;
                                          yyextra->defLitText+=yytext;
                                          BEGIN(DefineText);
                                        }
<SkipSingleQuote,SkipDoubleQuote>\\.    {
                                          outputSpaces(yyscanner,yytext);
                                          yyextra->defText += yytext;
                                          yyextra->defLitText+=yytext;
                                        }
<SkipSingleQuote>\'                     {
                                          outputChar(yyscanner,' ');
                                          yyextra->defText += *yytext;
                                          yyextra->defLitText+=yytext;
                                          BEGIN(DefineText);
                                        }
<SkipDoubleQuote,SkipSingleQuote>.      { outputSpace(yyscanner,yytext[0]);
                                          yyextra->defText    += *yytext;
                                          yyextra->defLitText += *yytext;
                                        }
<DefineText>.                           { outputSpace(yyscanner,yytext[0]);
                                          yyextra->defText    += *yytext;
                                          yyextra->defLitText += *yytext;
                                        }
<<EOF>>                                 {
                                          DBG_CTX((stderr,"End of include file\n"));
                                          //printf("Include stack depth=%d\n",yyextra->includeStack.size());
                                          if (yyextra->includeStack.empty())
                                          {
                                            DBG_CTX((stderr,"Terminating scanner!\n"));
                                            yyterminate();
                                          }
                                          else
                                          {
                                            QCString toFileName = yyextra->yyFileName;
                                            const std::unique_ptr<FileState> &fs=yyextra->includeStack.back();
                                            //fileDefineCache->merge(yyextra->yyFileName,fs->fileName);
                                            YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
                                            yy_switch_to_buffer( fs->bufState, yyscanner );
                                            yy_delete_buffer( oldBuf, yyscanner );
                                            yyextra->yyLineNr    = fs->lineNr;
                                            //preYYin = fs->oldYYin;
                                            yyextra->inputBuf    = fs->oldFileBuf;
                                            yyextra->inputBufPos = fs->oldFileBufPos;
                                            yyextra->curlyCount  = fs->curlyCount;
                                            setFileName(yyscanner,fs->fileName);
                                            DBG_CTX((stderr,"######## FileName %s\n",qPrint(yyextra->yyFileName)));

                                            // Deal with file changes due to
                                            // #include's within { .. } blocks
                                            QCString lineStr(15+yyextra->yyFileName.length());
                                            lineStr.sprintf("# %d \"%s\" 2",yyextra->yyLineNr,qPrint(yyextra->yyFileName));
                                            outputString(yyscanner,lineStr);

                                            yyextra->includeStack.pop_back();

                                            {
                                              std::lock_guard<std::mutex> lock(g_globalDefineMutex);
                                              // to avoid deadlocks we allow multiple threads to process the same header file.
                                              // The first one to finish will store the results globally. After that the
                                              // next time the same file is encountered, the stored data is used and the file
                                              // is not processed again.
                                              if (!g_defineManager.alreadyProcessed(toFileName.str()))
                                              {
                                                // now that the file is completely processed, prevent it from processing it again
                                                g_defineManager.addInclude(yyextra->yyFileName.str(),toFileName.str());
                                                g_defineManager.store(toFileName.str(),yyextra->localDefines);
                                              }
                                              else
                                              {
                                                if (Debug::isFlagSet(Debug::Preprocessor))
                                                {
                                                  Debug::print(Debug::Preprocessor,0,"#include %s: was already processed by another thread! not storing data...\n",qPrint(toFileName));
                                                }
                                              }
                                            }
                                            // move the local macros definitions for in this file to the translation unit context
                                            for (const auto &kv : yyextra->localDefines)
                                            {
                                              auto pair = yyextra->contextDefines.insert(kv);
                                              if (!pair.second) // define already in context -> replace with local version
                                              {
                                                yyextra->contextDefines.erase(pair.first);
                                                yyextra->contextDefines.insert(kv);
                                              }
                                            }
                                            yyextra->localDefines.clear();
                                          }
                                        }
<*>{CCS}/{CCE}                          |
<*>{CCS}[*!]?                           {
                                          if (YY_START==SkipVerbatim || YY_START==SkipCond)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                            yyextra->lastCContext=YY_START;
                                            yyextra->commentCount=1;
                                            if (yyleng==3)
                                            {
                                              yyextra->isSpecialComment = true;
                                              yyextra->lastGuardName.resize(0); // reset guard in case the #define is documented!
                                            }
                                            else
                                            {
                                              yyextra->isSpecialComment = false;
                                            }
                                            BEGIN(SkipCComment);
                                          }
                                        }
<*>{CPPC}[/!]?                          {
                                          if (YY_START==SkipVerbatim || YY_START==SkipCond || getLanguageFromFileName(yyextra->yyFileName)==SrcLangExt_Fortran)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                            yyextra->lastCPPContext=YY_START;
                                            if (yyleng==3)
                                            {
                                              yyextra->isSpecialComment = true;
                                              yyextra->lastGuardName.resize(0); // reset guard in case the #define is documented!
                                            }
                                            else
                                            {
                                              yyextra->isSpecialComment = false;
                                            }
                                            BEGIN(SkipCPPComment);
                                          }
                                        }
<*>\n                                   {
                                          outputChar(yyscanner,'\n');
                                          yyextra->yyLineNr++;
                                        }
<*>.                                    {
                                          yyextra->expectGuard = FALSE;
                                          outputChar(yyscanner,*yytext);
                                        }

%%

/////////////////////////////////////////////////////////////////////////////////////

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  yy_size_t bytesInBuf = state->inputBuf->curPos()-state->inputBufPos;
  yy_size_t bytesToCopy = std::min(max_size,bytesInBuf);
  memcpy(buf,state->inputBuf->data()+state->inputBufPos,bytesToCopy);
  state->inputBufPos+=bytesToCopy;
  return bytesToCopy;
}

static void setFileName(yyscan_t yyscanner,const QCString &name)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  bool ambig;
  FileInfo fi(name.str());
  state->yyFileName=fi.absFilePath();
  state->yyFileDef=findFileDef(Doxygen::inputNameLinkedMap,state->yyFileName,ambig);
  if (state->yyFileDef==0) // if this is not an input file check if it is an
                      // include file
  {
    state->yyFileDef=findFileDef(Doxygen::includeNameLinkedMap,state->yyFileName,ambig);
  }
  //printf("setFileName(%s) state->yyFileName=%s state->yyFileDef=%p\n",
  //    name,qPrint(state->yyFileName),state->yyFileDef);
  if (state->yyFileDef && state->yyFileDef->isReference()) state->yyFileDef=0;
  state->insideCS = getLanguageFromFileName(state->yyFileName)==SrcLangExt_CSharp;
  state->insideFtn = getLanguageFromFileName(state->yyFileName)==SrcLangExt_Fortran;
  state->isSource = guessSection(state->yyFileName);
}

static void incrLevel(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  state->levelGuard.push(false);
  //printf("%s line %d: incrLevel %d\n",qPrint(yyextra->yyFileName),yyextra->yyLineNr,yyextra->levelGuard.size());
}

static void decrLevel(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("%s line %d: decrLevel %d\n",qPrint(state->yyFileName),state->yyLineNr,state->levelGuard.size());
  if (!state->levelGuard.empty())
  {
    state->levelGuard.pop();
  }
  else
  {
    warn(state->yyFileName,state->yyLineNr,"More #endif's than #if's found.\n");
  }
}

static bool otherCaseDone(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->levelGuard.empty())
  {
    warn(state->yyFileName,state->yyLineNr,"Found an #else without a preceding #if.\n");
    return TRUE;
  }
  else
  {
    return state->levelGuard.top();
  }
}

static void setCaseDone(yyscan_t yyscanner,bool value)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  state->levelGuard.top()=value;
}


static FileState *checkAndOpenFile(yyscan_t yyscanner,const QCString &fileName,bool &alreadyProcessed)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  alreadyProcessed = FALSE;
  FileState *fs = 0;
  //printf("checkAndOpenFile(%s)\n",qPrint(fileName));
  FileInfo fi(fileName.str());
  if (fi.exists() && fi.isFile())
  {
    const StringVector &exclPatterns = Config_getList(EXCLUDE_PATTERNS);
    if (patternMatch(fi,exclPatterns)) return 0;

    QCString absName = fi.absFilePath();

    // global guard
    if (state->curlyCount==0) // not #include inside { ... }
    {
      std::lock_guard<std::mutex> lock(g_globalDefineMutex);
      if (g_defineManager.alreadyProcessed(absName.str()))
      {
        alreadyProcessed = TRUE;
        //printf("  already included 1\n");
        return 0; // already done
      }
    }
    // check include stack for absName

    alreadyProcessed = std::any_of(
      state->includeStack.begin(),
      state->includeStack.end(),
      [absName](const std::unique_ptr<FileState> &lfs)
        { return lfs->fileName==absName; }
    );

    if (alreadyProcessed)
    {
      //printf("  already included 2\n");
      return 0;
    }
    //printf("#include %s\n",qPrint(absName));

    fs = new FileState(static_cast<uint>(fi.size())+4096);
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

static FileState *findFile(yyscan_t yyscanner, const QCString &fileName,bool localInclude,bool &alreadyProcessed)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("** findFile(%s,%d) state->yyFileName=%s\n",qPrint(fileName),localInclude,qPrint(state->yyFileName));
  if (Portable::isAbsolutePath(fileName))
  {
    FileState *fs = checkAndOpenFile(yyscanner,fileName,alreadyProcessed);
    if (fs)
    {
      setFileName(yyscanner,fileName);
      state->yyLineNr=1;
      return fs;
    }
    else if (alreadyProcessed)
    {
      return 0;
    }
  }
  if (localInclude && !state->yyFileName.isEmpty())
  {
    FileInfo fi(state->yyFileName.str());
    if (fi.exists())
    {
      QCString absName = QCString(fi.dirPath(TRUE))+"/"+fileName;
      FileState *fs = checkAndOpenFile(yyscanner,absName,alreadyProcessed);
      if (fs)
      {
        setFileName(yyscanner,absName);
        state->yyLineNr=1;
        return fs;
      }
      else if (alreadyProcessed)
      {
        return 0;
      }
    }
  }
  if (state->pathList.empty())
  {
    return 0;
  }
  for (auto path : state->pathList)
  {
    std::string absName = (path+"/"+fileName).str();
    //printf("  Looking for %s in %s\n",fileName,path.c_str());
    FileState *fs = checkAndOpenFile(yyscanner,absName.c_str(),alreadyProcessed);
    if (fs)
    {
      setFileName(yyscanner,absName.c_str());
      state->yyLineNr=1;
      //printf("  -> found it\n");
      return fs;
    }
    else if (alreadyProcessed)
    {
      return 0;
    }
  }
  return 0;
}

static QCString extractTrailingComment(const QCString &s)
{
  if (s.isEmpty()) return "";
  int i=(int)s.length()-1;
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
static bool expandExpression(yyscan_t yyscanner,QCString &expr,QCString *rest,int pos,int level);

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
        c=s.at(i++);
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
  //printf("stringize '%s'->'%s'\n",qPrint(s),qPrint(result));
  return result;
}

/*! Execute all ## operators in expr.
 * If the macro name before or after the operator contains a no-rescan
 * marker (@-) then this is removed (before the concatenated macro name
 * may be expanded again.
 */
static void processConcatOperators(QCString &expr)
{
  if (expr.isEmpty()) return;
  //printf("processConcatOperators: in='%s'\n",qPrint(expr));
  std::string e = expr.str();
  static const reg::Ex r(R"(\s*##\s*)");
  reg::Iterator end;

  size_t i=0;
  for (;;)
  {
    reg::Iterator it(e,r,i);
    if (it!=end)
    {
      const auto &match = *it;
      size_t n = match.position();
      size_t l = match.length();
      //printf("Match: '%s'\n",qPrint(expr.mid(i)));
      if (n+l+1<e.length() && e[static_cast<int>(n+l)]=='@' && expr[static_cast<int>(n+l+1)]=='-')
      {
        // remove no-rescan marker after ID
        l+=2;
      }
      //printf("found '%s'\n",qPrint(expr.mid(n,l)));
      // remove the ## operator and the surrounding whitespace
      e=e.substr(0,n)+e.substr(n+l);
      int k=static_cast<int>(n)-1;
      while (k>=0 && isId(e[k])) k--; 
      if (k>0 && e[k]=='-' && e[k-1]=='@')
      {
        // remove no-rescan marker before ID
        e=e.substr(0,k-1)+e.substr(k+1);
        n-=2;
      }
      i=n;
    }
    else
    {
      break;
    }
  }

  expr = e;

  //printf("processConcatOperators: out='%s'\n",qPrint(expr));
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
  //printf(">replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s') level=%d\n",qPrint(expr),rest ? qPrint(*rest) : 0,pos,qPrint(def->name),state->levelGuard.size());
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

  std::map<std::string,std::string> argTable;  // list of arguments
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
        int lvl=1;
        arg+=c;
        //char term='\0';
        while ((cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
        {
          c=(char)cc;
          //printf("processing %c: term=%c (%d)\n",c,term,term);
          if (c=='\'' || c=='\"') // skip ('s and )'s inside strings
          {
            arg+=c;
            addTillEndOfString(yyscanner,expr,rest,j,c,arg);
          }
          if (c==')')
          {
            lvl--;
            arg+=c;
            if (lvl==0) break;
          }
          else if (c=='(')
          {
            lvl++;
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
          argTable.emplace(toStdString(argKey), toStdString(arg));
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
    //printf("Macro definition: '%s'\n",qPrint(d));
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
          //printf("request key %s result %s\n",qPrint(key),argTable[key]->data());
          auto it = argTable.find(key.str());
          if (it!=argTable.end())
          {
            QCString substArg = it->second.c_str();
            //printf("substArg='%s'\n",qPrint(substArg));
            // only if no ## operator is before or after the argument
            // marker we do macro expansion.
            if (!hash)
            {
              expandExpression(yyscanner,substArg,0,0,level+1);
            }
            if (inString)
            {
              //printf("'%s'=stringize('%s')\n",qPrint(stringize(*subst)),subst->data());

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
    //printf("<replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s',result='%s') level=%d return=TRUE\n",qPrint(expr),rest ? qPrint(*rest) : 0,pos,qPrint(def->name),qPrint(result),state->levelGuard.size());
    return TRUE;
  }
  //printf("<replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s',result='%s') level=%d return=FALSE\n",qPrint(expr),rest ? qPrint(*rest) : 0,pos,qPrint(def->name),qPrint(result),state->levelGuard.size());
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

#define MAX_EXPANSION_DEPTH 50

/*! performs recursive macro expansion on the string \a expr
 *  starting at position \a pos.
 *  May read additional characters from the input while re-scanning!
 */
static bool expandExpression(yyscan_t yyscanner,QCString &expr,QCString *rest,int pos,int level)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf(">expandExpression(expr='%s',rest='%s',pos=%d,level=%d)\n",qPrint(expr),rest ? qPrint(*rest) : "", pos, level);
  if (expr.isEmpty())
  {
    //printf("<expandExpression: empty\n");
    return TRUE;
  }
  if (state->expanded.find(expr.str())!=state->expanded.end() &&
      level>MAX_EXPANSION_DEPTH) // check for too deep recursive expansions
  {
    //printf("<expandExpression: already expanded expr='%s'\n",qPrint(expr));
    return FALSE;
  }
  else
  {
    state->expanded.insert(expr.str());
  }
  QCString macroName;
  QCString expMacro;
  bool definedTest=FALSE;
  int i=pos,l,p,len;
  int startPos = pos;
  int samePosCount=0;
  while ((p=getNextId(expr,i,&l))!=-1) // search for an macro name
  {
    bool replaced=FALSE;
    macroName=expr.mid(p,l);
    //printf(" p=%d macroName=%s\n",p,qPrint(macroName));
    if (p<2 || !(expr.at(p-2)=='@' && expr.at(p-1)=='-')) // no-rescan marker?
    {
      if (state->expandedDict.find(macroName.str())==state->expandedDict.end()) // expand macro
      {
        Define *def=isDefined(yyscanner,macroName);
        if (macroName=="defined")
        {
          //printf("found defined inside macro definition '%s'\n",qPrint(expr.right(expr.length()-p)));
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
          //printf("macro '%s'->'%s'\n",qPrint(macroName),qPrint(def->definition));
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
          //printf("simple macro expansion='%s'->'%s'\n",qPrint(macroName),qPrint(expMacro));
        }
        else if (def && def->nargs>=0) // function macro
        {
          //printf(" >>>> call replaceFunctionMacro expr='%s'\n",qPrint(expr));
          replaced=replaceFunctionMacro(yyscanner,expr,rest,p+l,len,def,expMacro,level);
          //printf(" <<<< call replaceFunctionMacro: replaced=%d\n",replaced);
          len+=l;
        }
        //printf(" macroName='%s' expMacro='%s' replaced=%d\n",qPrint(macroName),qPrint(expMacro),replaced);

        if (replaced) // expand the macro and rescan the expression
        {
          //printf(" replacing '%s'->'%s'\n",expr.mid(p,qPrint(len)),qPrint(expMacro));
          QCString resultExpr=expMacro;
          QCString restExpr=expr.right(expr.length()-len-p);
          processConcatOperators(resultExpr);
          //printf(" macroName=%s restExpr='%s' def->nonRecursive=%d\n",qPrint(macroName),qPrint(restExpr),def->nonRecursive);
          bool expanded=false;
          if (def && !def->nonRecursive)
          {
            state->expandedDict.emplace(toStdString(macroName),def);
            expanded = expandExpression(yyscanner,resultExpr,&restExpr,0,level+1);
            state->expandedDict.erase(toStdString(macroName));
          }
    else if (def && def->nonRecursive)
    {
      expanded = true;
    }
          if (expanded)
          {
            expr=expr.left(p)+resultExpr+restExpr;
            //printf(" new expression: '%s' old i=%d new i=%d\n",qPrint(expr),i,p);
            i=p;
          }
          else
          {
            expr=expr.left(p)+"@-"+expr.right(expr.length()-p);
            i=p+l+2;
          }
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
        //printf("macro already expanded, moving to the next macro expr=%s\n",qPrint(expr));
        i=p+l+2;
        //i=p+l;
      }
      // check for too many inplace expansions without making progress
      if (i==startPos)
      {
        samePosCount++;
      }
      else
      {
        startPos=i;
        samePosCount=0;
      }
      if (samePosCount>MAX_EXPANSION_DEPTH)
      {
        break;
      }
    }
    else // no re-scan marker found, skip the macro name
    {
      //printf("skipping marked macro\n");
      i=p+l;
    }
  }
  //printf("<expandExpression(expr='%s',rest='%s',pos=%d,level=%d)\n",qPrint(expr),rest ? qPrint(*rest) : "", pos,level);
  return TRUE;
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
static QCString removeIdsAndMarkers(const QCString &s)
{
  //printf("removeIdsAndMarkers(%s)\n",s);
  if (s.isEmpty()) return s;
  const char *p=s.data();
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
        char lc=(char)tolower(c);
        if (!isId(lc) && lc!='.' /*&& lc!='-' && lc!='+'*/) inNum=FALSE;
        p++;
      }
    }
  }
  //printf("removeIdsAndMarkers(%s)=%s\n",s,qPrint(result));
  return result;
}

/*! replaces all occurrences of @@ in \a s by @
 *  \par assumption:
 *   \a s only contains pairs of @@'s
 */
static QCString removeMarkers(const QCString &s)
{
  if (s.isEmpty()) return s;
  const char *p=s.data();
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
  //printf("RemoveMarkers(%s)=%s\n",s,qPrint(result));
  return result;
}

/*! compute the value of the expression in string \a expr.
 *  If needed the function may read additional characters from the input.
 */

static bool computeExpression(yyscan_t yyscanner,const QCString &expr)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  QCString e=expr;
  state->expanded.clear();
  expandExpression(yyscanner,e,0,0,0);
  //printf("after expansion '%s'\n",qPrint(e));
  e = removeIdsAndMarkers(e);
  if (e.isEmpty()) return FALSE;
  //printf("parsing '%s'\n",qPrint(e));
  return state->constExpParser.parse(state->yyFileName.data(),state->yyLineNr,e.str());
}

/*! expands the macro definition in \a name
 *  If needed the function may read additional characters from the input
 */

static QCString expandMacro(yyscan_t yyscanner,const QCString &name)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  QCString n=name;
  state->expanded.clear();
  expandExpression(yyscanner,n,0,0,0);
  n=removeMarkers(n);
  //printf("expandMacro '%s'->'%s'\n",qPrint(name),qPrint(n));
  return n;
}

static void addDefine(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  Define def;
  def.name       = state->defName;
  def.definition = state->defText.stripWhiteSpace();
  def.nargs      = state->defArgs;
  def.fileName   = state->yyFileName;
  def.fileDef    = state->yyFileDef;
  def.lineNr     = state->yyLineNr-state->yyMLines;
  def.columnNr   = state->yyColNr;
  def.varArgs    = state->defVarArgs;
  //printf("newDefine: %s %s file: %s\n",qPrint(def.name),qPrint(def.definition),
  //    def.fileDef ? qPrint(def.fileDef->name()) : qPrint(def.fileName));
  //printf("newDefine: '%s'->'%s'\n",qPrint(def.name),qPrint(def.definition));
  if (!def.name.isEmpty() &&
      Doxygen::expandAsDefinedSet.find(def.name.str())!=Doxygen::expandAsDefinedSet.end())
  {
    def.isPredefined=TRUE;
  }
  auto it = state->localDefines.find(def.name.str());
  if (it!=state->localDefines.end()) // redefine
  {
    state->localDefines.erase(it);
  }
  state->localDefines.insert(std::make_pair(def.name.str(),def));
}

static void addMacroDefinition(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->skip) return; // do not add this define as it is inside a
                      // conditional section (cond command) that is disabled.

  Define define;
  define.fileName = state->yyFileName;
  define.lineNr   = state->yyLineNr - state->yyMLines;
  define.columnNr = state->yyColNr;
  define.name     = state->defName;
  define.args     = state->defArgsStr;
  define.fileDef  = state->inputFileDef;

  QCString litText = state->defLitText;
  int l=litText.find('\n');
  if (l>0 && litText.left(l).stripWhiteSpace()=="\\")
  {
    // strip first line if it only contains a slash
    litText = litText.right(litText.length()-l-1);
  }
  else if (l>0)
  {
    // align the items on the first line with the items on the second line
    int k=l+1;
    const char *p=litText.data()+k;
    char c;
    while ((c=*p++) && (c==' ' || c=='\t')) k++;
    litText=litText.mid(l+1,k-l-1)+litText.stripWhiteSpace();
  }
  QCString litTextStripped = state->defLitText.stripWhiteSpace();
  if (litTextStripped.contains('\n')>=1)
  {
    define.definition = litText;
  }
  else
  {
    define.definition = litTextStripped;
  }
  {
    state->macroDefinitions.push_back(define);
  }
}

static inline void outputChar(yyscan_t yyscanner,char c)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.empty() || state->curlyCount>0) state->outputBuf->addChar(c);
}

static inline void outputArray(yyscan_t yyscanner,const char *a,yy_size_t len)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.empty() || state->curlyCount>0) state->outputBuf->addArray(a,static_cast<uint>(len));
}

static inline void outputString(yyscan_t yyscanner,const QCString &a)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.empty() || state->curlyCount>0) state->outputBuf->addArray(a.data(),a.length());
}

static inline void outputSpace(yyscan_t yyscanner,char c)
{
  if (c=='\t') outputChar(yyscanner,'\t');
  else outputChar(yyscanner,' ');
}

static inline void outputSpaces(yyscan_t yyscanner,char *s)
{
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    if (c=='\t') outputChar(yyscanner,'\t');
    else outputChar(yyscanner,' ');
  }
}

static inline void extraSpacing(yyscan_t yyscanner)
{
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->defContinue) return;
  for (int i=0; i< (int)yyleng; i++)
  {
    if (yytext[i] == '\t')
      yyextra->defExtraSpacing+='\t';
    else
      yyextra->defExtraSpacing+=' ';
  }
}

static QCString determineAbsoluteIncludeName(const QCString &curFile,const QCString &incFileName)
{
  bool searchIncludes = Config_getBool(SEARCH_INCLUDES);
  QCString absIncFileName = incFileName;
  FileInfo fi(curFile.str());
  if (fi.exists())
  {
    QCString absName = QCString(fi.dirPath(TRUE))+"/"+incFileName;
    FileInfo fi2(absName.str());
    if (fi2.exists())
    {
      absIncFileName=fi2.absFilePath();
    }
    else if (searchIncludes) // search in INCLUDE_PATH as well
    {
      const StringVector &includePath = Config_getList(INCLUDE_PATH);
      for (const auto &incPath : includePath)
      {
        FileInfo fi3(incPath);
        if (fi3.exists() && fi3.isDir())
        {
          absName = QCString(fi3.absFilePath())+"/"+incFileName;
          //printf("trying absName=%s\n",qPrint(absName));
          FileInfo fi4(absName.str());
          if (fi4.exists())
          {
            absIncFileName=fi4.absFilePath();
            break;
          }
          //printf( "absIncFileName = %s\n", qPrint(absIncFileName) );
        }
      }
    }
    //printf( "absIncFileName = %s\n", qPrint(absIncFileName) );
  }
  return absIncFileName;
}

static void readIncludeFile(yyscan_t yyscanner,const QCString &inc)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
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
    //printf("Searching for '%s'\n",qPrint(incFileName));

    QCString absIncFileName = determineAbsoluteIncludeName(state->yyFileName,incFileName);

    // findFile will overwrite state->yyFileDef if found
    FileState *fs;
    bool alreadyProcessed = FALSE;
    //printf("calling findFile(%s)\n",qPrint(incFileName));
    if ((fs=findFile(yyscanner,incFileName,localInclude,alreadyProcessed))) // see if the include file can be found
    {
      {
        std::lock_guard<std::mutex> lock(g_globalDefineMutex);
        g_defineManager.addInclude(oldFileName.str(),absIncFileName.str());
      }

      //printf("Found include file!\n");
      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        for (i=0;i<state->includeStack.size();i++)
        {
          Debug::print(Debug::Preprocessor,0,"  ");
        }
        Debug::print(Debug::Preprocessor,0,"#include %s: parsing...\n",qPrint(incFileName));
      }

      if (state->includeStack.empty() && oldFileDef)
      {
        PreIncludeInfo *ii = state->includeRelations.find(absIncFileName);
        if (ii==0)
        {
          bool ambig;
          FileDef *incFd = findFileDef(Doxygen::inputNameLinkedMap,absIncFileName,ambig);
          state->includeRelations.add(
              absIncFileName,
              oldFileDef,
              ambig?nullptr:incFd,
              incFileName,
              localInclude,
              state->isImported
              );
        }
      }

      struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
      fs->bufState = YY_CURRENT_BUFFER;
      fs->lineNr   = oldLineNr;
      fs->fileName = oldFileName;
      fs->curlyCount = state->curlyCount;
      state->curlyCount = 0;
      // push the state on the stack
      state->includeStack.emplace_back(fs);
      // set the scanner to the include file

      // Deal with file changes due to
      // #include's within { .. } blocks
      QCString lineStr(state->yyFileName.length()+20);
      lineStr.sprintf("# 1 \"%s\" 1\n",qPrint(state->yyFileName));
      outputString(yyscanner,lineStr);

      DBG_CTX((stderr,"Switching to include file %s\n",qPrint(incFileName)));
      state->expectGuard=TRUE;
      state->inputBuf   = &fs->fileBuf;
      state->inputBufPos=0;
      yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner),yyscanner);
    }
    else
    {
      if (alreadyProcessed) // if this header was already process we can just copy the stored macros
                           // in the local context
      {
        std::lock_guard<std::mutex> lock(g_globalDefineMutex);
        g_defineManager.addInclude(state->yyFileName.str(),absIncFileName.str());
        g_defineManager.retrieve(absIncFileName.str(),state->contextDefines);
      }

      if (state->includeStack.empty() && oldFileDef)
      {
        PreIncludeInfo *ii = state->includeRelations.find(absIncFileName);
        if (ii==0)
        {
          bool ambig;
          FileDef *incFd = findFileDef(Doxygen::inputNameLinkedMap,absIncFileName,ambig);
          ii = state->includeRelations.add(absIncFileName,
              oldFileDef,
              ambig?0:incFd,
              incFileName,
              localInclude,
              state->isImported
              );
        }
      }

      if (Debug::isFlagSet(Debug::Preprocessor))
      {
        for (i=0;i<state->includeStack.size();i++)
        {
          Debug::print(Debug::Preprocessor,0,"  ");
        }
        if (alreadyProcessed)
        {
          Debug::print(Debug::Preprocessor,0,"#include %s: already processed! skipping...\n",qPrint(incFileName));
        }
        else
        {
          Debug::print(Debug::Preprocessor,0,"#include %s: not found! skipping...\n",qPrint(incFileName));
        }
        //printf("error: include file %s not found\n",yytext);
      }
      if (state->curlyCount>0 && !alreadyProcessed) // failed to find #include inside { ... }
      {
        warn(state->yyFileName,state->yyLineNr,"include file %s not found, perhaps you forgot to add its directory to INCLUDE_PATH?",qPrint(incFileName));
      }
    }
  }
}

/* ----------------------------------------------------------------- */

static void startCondSection(yyscan_t yyscanner,const QCString &sectId)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("startCondSection: skip=%d stack=%d\n",state->skip,state->condStack.size());
  CondParser prs;
  bool expResult = prs.parse(state->yyFileName.data(),state->yyLineNr,sectId.data());
  state->condStack.emplace(std::make_unique<CondCtx>(state->yyLineNr,sectId,state->skip));
  if (!expResult)
  {
    state->skip=TRUE;
  }
  //printf("  expResult=%d skip=%d\n",expResult,state->skip);
}

static void endCondSection(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->condStack.empty())
  {
    state->skip=FALSE;
  }
  else
  {
    const std::unique_ptr<CondCtx> &ctx = state->condStack.top();
    state->skip=ctx->skip;
    state->condStack.pop();
  }
  //printf("endCondSection: skip=%d stack=%d\n",state->skip,state->condStack.count());
}

static void forceEndCondSection(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  while (!state->condStack.empty())
  {
    state->condStack.pop();
  }
  state->skip=FALSE;
}

static QCString escapeAt(const QCString &text)
{
  QCString result;
  if (!text.isEmpty())
  {
    char c;
    const char *p=text.data();
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
  //printf("getNextChar(%s,%s,%d)\n",qPrint(expr),rest ? rest->data() : 0,pos);
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
  //printf("getCurrentChar(%s,%s,%d)\n",qPrint(expr),rest ? rest->data() : 0,pos);
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
    returnCharToStream(yyscanner,(char)cc);
    //printf("%c=yyinput()\n",cc);
    return cc;
  }
}

static void unputChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint &pos,char c)
{
  //printf("unputChar(%s,%s,%d,%c)\n",qPrint(expr),rest ? rest->data() : 0,pos,c);
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
  //printf("result: unputChar(%s,%s,%d,%c)\n",qPrint(expr),rest ? rest->data() : 0,pos,c);
}

/** Returns a reference to a Define object given its name or 0 if the Define does
 *  not exist.
 */
static Define *isDefined(yyscan_t yyscanner,const QCString &name)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);

  bool undef = false;
  auto findDefine = [&undef,&name](DefineMap &map)
  {
    Define *d=0;
    auto it = map.find(name.str());
    if (it!=map.end())
    {
      d = &it->second;
      if (d->undef)
      {
        undef=true;
        d=0;
      }
    }
    return d;
  };

  Define *def = findDefine(state->localDefines);
  if (def==0 && !undef)
  {
    def = findDefine(state->contextDefines);
  }
  return def;
}

static void initPredefined(yyscan_t yyscanner,const QCString &fileName)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);

  // add predefined macros
  const StringVector &predefList = Config_getList(PREDEFINED);
  for (const auto &ds : predefList)
  {
    size_t i_equals=ds.find('=');
    size_t i_obrace=ds.find('(');
    size_t i_cbrace=ds.find(')');
    bool nonRecursive = i_equals!=std::string::npos && i_equals>0 && ds[i_equals-1]==':';

    if ((i_obrace==0) || (i_equals==0) || (i_equals==1 && ds[i_equals-1]==':'))
    {
      continue; // no define name
    }

    if (i_obrace<i_equals && i_cbrace<i_equals && 
        i_obrace!=std::string::npos && i_cbrace!=std::string::npos && 
        i_obrace<i_cbrace
       ) // predefined function macro definition
    {
      static const reg::Ex reId(R"(\a\w*)");
      std::map<std::string,int> argMap;
      std::string args  = ds.substr(i_obrace+1,i_cbrace-i_obrace-1); // part between ( and )
      bool   hasVarArgs = args.find("...")!=std::string::npos;
      //printf("predefined function macro '%s'\n",ds.c_str());
      int count = 0;
      reg::Iterator arg_it(args,reId,0);
      reg::Iterator arg_end;
      // gather the formal arguments in a dictionary
      for (; arg_it!=arg_end; ++arg_it)
      {
        argMap.emplace(arg_it->str(),count++);
      }
      if (hasVarArgs) // add the variable argument if present
      {
        argMap.emplace("__VA_ARGS__",count++);
      }

      // strip definition part
      std::string definition;
      std::string in=ds.substr(i_equals+1);
      reg::Iterator re_it(in,reId);
      reg::Iterator re_end;
      size_t i=0;
      // substitute all occurrences of formal arguments by their 
      // corresponding markers
      for (; re_it!=re_end; ++re_it)
      {
        const auto &match = *re_it;
        size_t pi = match.position();
        size_t l  = match.length();
        if (pi>i) definition+=in.substr(i,pi-i);

        auto it = argMap.find(match.str());
        if (it!=argMap.end())
        {
          int argIndex = it->second;
          QCString marker;
          marker.sprintf(" @%d ",argIndex);
          definition+=marker.str();
        }
        else
        {
          definition+=match.str();
        }
        i=pi+l;
      }
      definition+=in.substr(i);

      // add define definition to the dictionary of defines for this file
      std::string dname = ds.substr(0,i_obrace);
      if (!dname.empty())
      {
        Define def;
        def.name         = dname;
        def.definition   = definition;
        def.nargs        = count;
        def.isPredefined = TRUE;
        def.nonRecursive = nonRecursive;
        def.fileDef      = state->yyFileDef;
        def.fileName     = fileName;
        def.varArgs      = hasVarArgs;
        state->contextDefines.insert(std::make_pair(def.name.str(),def));

        //printf("#define '%s' '%s' #nargs=%d hasVarArgs=%d\n",
        //  qPrint(def.name),qPrint(def.definition),def.nargs,def.varArgs);
      }
    }
    else if (!ds.empty()) // predefined non-function macro definition
    {
      //printf("predefined normal macro '%s'\n",ds.c_str());
      Define def;
      if (i_equals==std::string::npos) // simple define without argument
      {
        def.name = ds;
        def.definition = "1"; // substitute occurrences by 1 (true)
      }
      else // simple define with argument
      {
        int ine=static_cast<int>(i_equals) - (nonRecursive ? 1 : 0);
        def.name = ds.substr(0,ine);
        def.definition = ds.substr(i_equals+1);
      }
      if (!def.name.isEmpty())
      {
        def.nargs = -1;
        def.isPredefined = TRUE;
        def.nonRecursive = nonRecursive;
        def.fileDef      = state->yyFileDef;
        def.fileName     = fileName;
        state->contextDefines.insert(std::make_pair(def.name.str(),def));
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////

struct Preprocessor::Private
{
  yyscan_t yyscanner;
  preYY_state state;
};

void Preprocessor::addSearchDir(const QCString &dir)
{
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  FileInfo fi(dir.str());
  if (fi.isDir()) state->pathList.push_back(fi.absFilePath());
}

Preprocessor::Preprocessor() : p(std::make_unique<Private>())
{
  preYYlex_init_extra(&p->state,&p->yyscanner);
  addSearchDir(".");
}

Preprocessor::~Preprocessor()
{
  preYYlex_destroy(p->yyscanner);
}

void Preprocessor::processFile(const QCString &fileName,BufStr &input,BufStr &output)
{
//  printf("Preprocessor::processFile(%s)\n",fileName);
  yyscan_t yyscanner = p->yyscanner;
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

#ifdef FLEX_DEBUG
  preYYset_debug(1,yyscanner);
#endif

  printlex(yy_flex_debug, TRUE, __FILE__, qPrint(fileName));
  uint orgOffset=output.curPos();
  //printf("##########################\n%s\n####################\n",
  //    qPrint(input));

  state->macroExpansion = Config_getBool(MACRO_EXPANSION);
  state->expandOnlyPredef = Config_getBool(EXPAND_ONLY_PREDEF);
  state->skip=FALSE;
  state->curlyCount=0;
  state->nospaces=FALSE;
  state->inputBuf=&input;
  state->inputBufPos=0;
  state->outputBuf=&output;
  state->includeStack.clear();
  state->expandedDict.clear();
  state->contextDefines.clear();
  while (!state->condStack.empty()) state->condStack.pop();

  setFileName(yyscanner,fileName);

  state->inputFileDef = state->yyFileDef;
  //yyextra->defineManager.startContext(state->yyFileName);

  initPredefined(yyscanner,fileName);

  state->yyLineNr = 1;
  state->yyColNr  = 1;
  state->ifcount  = 0;

  BEGIN( Start );

  state->expectGuard = guessSection(fileName)==Entry::HEADER_SEC;
  state->guardName.resize(0);
  state->lastGuardName.resize(0);
  state->guardExpr.resize(0);

  preYYlex(yyscanner);

  while (!state->condStack.empty())
  {
    const std::unique_ptr<CondCtx> &ctx = state->condStack.top();
    QCString sectionInfo = " ";
    if (ctx->sectionId!=" ") sectionInfo.sprintf(" with label '%s' ",qPrint(ctx->sectionId.stripWhiteSpace()));
    warn(fileName,ctx->lineNr,"Conditional section%sdoes not have "
        "a corresponding \\endcond command within this file.",qPrint(sectionInfo));
    state->condStack.pop();
  }
  // make sure we don't extend a \cond with missing \endcond over multiple files (see bug 624829)
  forceEndCondSection(yyscanner);

  if (Debug::isFlagSet(Debug::Preprocessor))
  {
    std::lock_guard<std::mutex> lock(g_debugMutex);
    char *orgPos=output.data()+orgOffset;
    char *newPos=output.data()+output.curPos();
    Debug::print(Debug::Preprocessor,0,"Preprocessor output of %s (size: %d bytes):\n",qPrint(fileName),newPos-orgPos);
    int line=1;
    Debug::print(Debug::Preprocessor,0,"---------\n");
    if (!Debug::isFlagSet(Debug::NoLineNo)) Debug::print(Debug::Preprocessor,0,"00001 ");
    while (orgPos<newPos) 
    {
      putchar(*orgPos);
      if (*orgPos=='\n' && !Debug::isFlagSet(Debug::NoLineNo)) Debug::print(Debug::Preprocessor,0,"%05d ",++line);
      orgPos++;
    }
    Debug::print(Debug::Preprocessor,0,"\n---------\n");
    if (yyextra->contextDefines.size()>0)
    {
      Debug::print(Debug::Preprocessor,0,"Macros accessible in this file (%s):\n", qPrint(fileName));
      Debug::print(Debug::Preprocessor,0,"---------\n");
      for (auto &kv : yyextra->contextDefines)
      {
        Debug::print(Debug::Preprocessor,0,"%s ",qPrint(kv.second.name));
      }
      for (auto &kv : yyextra->localDefines)
      {
        Debug::print(Debug::Preprocessor,0,"%s ",qPrint(kv.second.name));
      }
      Debug::print(Debug::Preprocessor,0,"\n---------\n");
    }
    else
    {
      Debug::print(Debug::Preprocessor,0,"No macros accessible in this file (%s).\n", qPrint(fileName));
    }
  }

  {
    std::lock_guard<std::mutex> lock(g_updateGlobals);
    for (const auto &inc : state->includeRelations)
    {
      if (inc->fromFileDef)
      {
        inc->fromFileDef->addIncludeDependency(inc->toFileDef,inc->includeName,inc->local,inc->imported);
      }
      if (inc->toFileDef && inc->fromFileDef)
      {
        inc->toFileDef->addIncludedByDependency(inc->fromFileDef,inc->fromFileDef->docName(),inc->local,inc->imported);
      }
    }
    // add the macro definition for this file to the global map
    Doxygen::macroDefinitions.emplace(std::make_pair(state->yyFileName.str(),std::move(state->macroDefinitions)));
  }

  //yyextra->defineManager.endContext();
  printlex(yy_flex_debug, FALSE, __FILE__, qPrint(fileName));
//  printf("Preprocessor::processFile(%s) finished\n",fileName);
}

#if USE_STATE2STRING
#include "pre.l.h"
#endif
