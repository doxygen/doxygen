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
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cerrno>

#include "qcstring.h"
#include "containers.h"
#include "pre.h"
#include "constexp.h"
#include "define.h"
#include "message.h"
#include "util.h"
#include "defargs.h"
#include "debug.h"
#include "portable.h"
#include "arguments.h"
#include "entry.h"
#include "condparser.h"
#include "config.h"
#include "filedef.h"
#include "regex.h"
#include "fileinfo.h"
#include "trace.h"
#include "debug.h"
#include "stringutil.h"

#define YY_NO_UNISTD_H 1

[[maybe_unused]] static const char *stateToString(int state);

struct preYY_CondCtx
{
  preYY_CondCtx(const QCString &file,int line,const QCString &id,bool b)
    : fileName(file), lineNr(line), sectionId(id), skip(b) {}
  QCString fileName;
  int lineNr;
  QCString sectionId;
  bool skip;
};

struct FileState
{
  int lineNr = 1;
  int curlyCount = 0;
  std::string fileBuf;
  const std::string *oldFileBuf = nullptr;
  int oldFileBufPos = 0;
  YY_BUFFER_STATE bufState = 0;
  QCString fileName;
  bool lexRulesPart = false;
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
        void addInclude(const std::string &fileName)
        {
          m_includedFiles.insert(fileName);
        }
        void store(const DefineMap &fromMap)
        {
          for (auto &[name,define] : fromMap)
          {
            m_defines.emplace(name,define);
          }
          //printf("  m_defines.size()=%zu\n",m_defines.size());
          m_stored=true;
        }
        void retrieve(DefineMap &toMap)
        {
          StringUnorderedSet includeStack;
          retrieveRec(toMap,includeStack);
        }
        void retrieveRec(DefineMap &toMap,StringUnorderedSet &includeStack)
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
          for (auto &[name,define] : m_defines)
          {
            toMap.emplace(name,define);
          }
        }
        bool stored() const { return m_stored; }
      private:
        DefineManager *m_parent;
        DefineMap m_defines;
        StringUnorderedSet m_includedFiles;
        bool m_stored = false;
    };

    friend class DefinesPerFile;
  public:

    void addInclude(const std::string &fromFileName,const std::string &toFileName)
    {
      //printf("DefineManager::addInclude('%s'->'%s')\n",qPrint(fromFileName),qPrint(toFileName));
      auto it = m_fileMap.find(fromFileName);
      if (it==m_fileMap.end())
      {
        it = m_fileMap.emplace(fromFileName,std::make_unique<DefinesPerFile>(this)).first;
      }
      auto &dpf = it->second;
      dpf->addInclude(toFileName);
    }

    void store(const std::string &fileName,const DefineMap &fromMap)
    {
      //printf("DefineManager::store(%s,#=%zu)\n",qPrint(fileName),fromMap.size());
      auto it = m_fileMap.find(fileName);
      if (it==m_fileMap.end())
      {
        it = m_fileMap.emplace(fileName,std::make_unique<DefinesPerFile>(this)).first;
      }
      it->second->store(fromMap);
    }

    void retrieve(const std::string &fileName,DefineMap &toMap)
    {
      auto it = m_fileMap.find(fileName);
      if (it!=m_fileMap.end())
      {
        auto &dpf = it->second;
        dpf->retrieve(toMap);
      }
      //printf("DefineManager::retrieve(%s,#=%zu)\n",qPrint(fileName),toMap.size());
    }

    bool alreadyProcessed(const std::string &fileName) const
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
    DefinesPerFile *find(const std::string &fileName) const
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
  QCString           fileName;
  FileDef           *yyFileDef      = nullptr;
  FileDef           *inputFileDef   = nullptr;
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
  const std::string *inputBuf       = nullptr;
  int                inputBufPos    = 0;
  std::string       *outputBuf      = nullptr;
  int                roundCount     = 0;
  bool               quoteArg       = false;
  bool               idStart        = false;
  int                findDefArgContext = 0;
  bool               expectGuard    = false;
  QCString           guardName;
  QCString           lastGuardName;
  QCString           incName;
  QCString           guardExpr;
  int                curlyCount     = 0;
  bool               nospaces       = false; // add extra spaces during macro expansion
  int                javaBlock      = 0;

  bool               macroExpansion = false; // from the configuration
  bool               expandOnlyPredef = false; // from the configuration
  QCString           potentialDefine;
  int                commentCount   = 0;
  bool               insideComment  = false;
  bool               isImported     = false;
  QCString           blockName;
  int                condCtx        = 0;
  bool               skip           = false;
  bool               insideIDL      = false;
  bool               insideCS       = false; // C# has simpler preprocessor
  bool               insideFtn      = false;
  bool               isSource       = false;

  yy_size_t          fenceSize      = 0;
  char               fenceChar      = ' ';
  bool               ccomment       = false;
  QCString           delimiter;
  bool               isSpecialComment = false;
  StringVector                             pathList;
  IntMap                                   argMap;
  BoolStack                                levelGuard;
  std::stack< std::unique_ptr<preYY_CondCtx> >   condStack;
  std::deque< std::unique_ptr<FileState> > includeStack;
  std::unordered_map<std::string,Define*>  expandedDict;
  StringUnorderedSet                       expanded;
  ConstExpressionParser                    constExpParser;
  DefineMap                                contextDefines; // macros imported from other files
  DefineMap                                localDefines;   // macros defined in this file
  DefineList                               macroDefinitions;
  LinkedMap<PreIncludeInfo>                includeRelations;
  StringUnorderedSet                       pragmaSet;

  int                lastContext = 0;
  bool               lexRulesPart = false;
  char               prevChar=0;
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
static int               yyread(yyscan_t yyscanner,char *buf,int max_size);
static Define *       isDefined(yyscan_t yyscanner,const QCString &name);
static void  determineBlockName(yyscan_t yyscanner);
static yy_size_t   getFenceSize(char *txt, yy_size_t leng);

/* ----------------------------------------------------------------- */

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

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

CMD            [\\@]
FORMULA_START  {CMD}("f{"|"f$"|"f["|"f(")
FORMULA_END    {CMD}("f}"|"f$"|"f]"|"f)")
VERBATIM_START {CMD}("verbatim"|"iliteral"|"latexonly"|"htmlonly"|"xmlonly"|"docbookonly"|"rtfonly"|"manonly"|"dot"|"msc"|"startuml"|"code"("{"[^}]*"}")?){BN}+
VERBATIM_END   {CMD}("endverbatim"|"endiliteral"|"endlatexonly"|"endhtmlonly"|"endxmlonly"|"enddocbookonly"|"endrtfonly"|"endmanonly"|"enddot"|"endmsc"|"enduml"|"endcode")
VERBATIM_LINE  {CMD}"noop"{B}+
LITERAL_BLOCK  {FORMULA_START}|{VERBATIM_START}
LITERAL_BLOCK_END {FORMULA_END}|{VERBATIM_END}

  // some rule pattern information for rules to handle lex files
nl              (\r\n|\r|\n)
RulesDelim      "%%"{nl}
RulesSharp      "<"[^>\n]*">"
RulesCurly      "{"[^{}\n]*"}"
StartSquare     "["
StartDouble     "\""
StartRound      "("
StartRoundQuest "(?"
EscapeRulesCharOpen  "\\["|"\\<"|"\\{"|"\\("|"\\\""|"\\ "|"\\\\"
EscapeRulesCharClose "\\]"|"\\>"|"\\}"|"\\)"
EscapeRulesChar      {EscapeRulesCharOpen}|{EscapeRulesCharClose}
CHARCE    "[:"[^:]*":]"

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

  //- begin: NUMBER
  // Note same defines in commentcnv.l: keep in sync
DECIMAL_INTEGER  [1-9][0-9']*[0-9]?[uU]?[lL]?[lL]?
HEXADECIMAL_INTEGER  "0"[xX][0-9a-zA-Z']+[0-9a-zA-Z]?
OCTAL_INTEGER  "0"[0-7][0-7']+[0-7]?
BINARY_INTEGER  "0"[bB][01][01']*[01]?
INTEGER_NUMBER {DECIMAL_INTEGER}|{HEXADECIMAL_INTEGER}|{OCTAL_INTEGER}|{BINARY_INTEGER}

FP_SUF [fFlL]

DIGIT_SEQ [0-9][0-9']*[0-9]?
FRAC_CONST {DIGIT_SEQ}"."|{DIGIT_SEQ}?"."{DIGIT_SEQ}
FP_EXP [eE][+-]?{DIGIT_SEQ}
DEC_FP1 {FRAC_CONST}{FP_EXP}?{FP_SUF}?
DEC_FP2 {DIGIT_SEQ}{FP_EXP}{FP_SUF}

HEX_DIGIT_SEQ [0-9a-fA-F][0-9a-fA-F']*[0-9a-fA-F]?
HEX_FRAC_CONST {HEX_DIGIT_SEQ}"."|{HEX_DIGIT_SEQ}?"."{HEX_DIGIT_SEQ}
BIN_EXP [pP][+-]?{DIGIT_SEQ}
HEX_FP1 "0"[xX]{HEX_FRAC_CONST}{BIN_EXP}{FP_SUF}?
HEX_FP2 "0"[xX]{HEX_DIGIT_SEQ}{BIN_EXP}{FP_SUF}?

FLOAT_DECIMAL {DEC_FP1}|{DEC_FP2}
FLOAT_HEXADECIMAL {HEX_FP1}|{HEX_FP2}
FLOAT_NUMBER {FLOAT_DECIMAL}|{FLOAT_HEXADECIMAL}
NUMBER {INTEGER_NUMBER}|{FLOAT_NUMBER}
  //- end: NUMBER ---------------------------------------------------------------------------


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
%x      CmakeDefName01
%x      SkipCPPBlock
%x      SkipCComment
%x      ArgCopyCComment
%x      ArgCopyCppComment
%x      CopyCComment
%x      SkipVerbatim
%x      SkipCondVerbatim
%x      SkipCPPComment
%x      JavaDocVerbatimCode
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
%x      IDLquote
%x      RulesPattern
%x      RulesDouble
%x      RulesRoundDouble
%x      RulesSquare
%x      RulesRoundSquare
%x      RulesRound
%x      RulesRoundQuest
%x      PragmaOnce

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
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::Lex) REJECT
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(LexCopyLine);
                                        }
<Start>^{Bopt}"cpp_quote"{Bopt}"("{Bopt}\" {
                                          if (yyextra->insideIDL)
                                          {
                                            BEGIN(IDLquote);
                                          }
                                          else
                                          {
                                            REJECT;
                                          }
                                        }
<IDLquote>"\\\\"                        {
                                          outputArray(yyscanner,"\\",1);
                                        }
<IDLquote>"\\\""                        {
                                          outputArray(yyscanner,"\"",1);
                                        }
<IDLquote>"\""{Bopt}")"                 {
                                          BEGIN(Start);
                                        }
<IDLquote>\n                            {
                                          outputChar(yyscanner,'\n');
                                          yyextra->yyLineNr++;
                                        }
<IDLquote>.                             {
                                          outputArray(yyscanner,yytext,yyleng);
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
<Start>^{B}*[_A-Z][_A-Z0-9]+{B}*"("[^\)\n]*")"{B}*\n | // function like macro
<Start>^{B}*[_A-Z][_A-Z0-9]+{B}*"("[^\(\)\n]*"("[^\)\n]*")"[^\)\n]*")"/{B}*("//"|"/\*") | // function list macro with one (...) argument followed by comment
<Start>^{B}*[_A-Z][_A-Z0-9]+{B}*"("[^\)\n]*")"/{B}*("//"|"/\*") { // function like macro followed by comment
                                          bool skipFuncMacros = Config_getBool(SKIP_FUNCTION_MACROS);
                                          QCString name(yytext);
                                          int pos = name.find('(');
                                          if (pos<0) pos=0; // should never happen
                                          name=name.left(pos).stripWhiteSpace();

                                          Define *def=nullptr;
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
                                            // Only when ends on \n
                                            if (yytext[yyleng-1] == '\n')
                                            {
                                              outputChar(yyscanner,'\n');
                                              yyextra->yyLineNr++;
                                            }
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
<CopyLine,LexCopyLine>"extern"{BN}*"\""[^\"]+"\""{BN}*("{")?  {
                                          QCString text=yytext;
                                          yyextra->yyLineNr+=text.contains('\n');
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyLine,LexCopyLine>{RAWBEGIN}        {
                                          yyextra->delimiter = extractBeginRawStringDelimiter(yytext);
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
<CopyLine,LexCopyLine>[$]?@\"           {
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::CSharp) REJECT;
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN( CopyStringCs );
                                        }
<CopyLine,LexCopyLine>\"                {
                                          outputChar(yyscanner,*yytext);
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::Fortran)
                                          {
                                            BEGIN( CopyString );
                                          }
                                          else
                                          {
                                            BEGIN( CopyStringFtnDouble );
                                          }
                                        }
<CopyLine,LexCopyLine>\'                {
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::Fortran) REJECT;
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyStringFtn );
                                        }
<CopyString>[^\"\\\r\n]{1,1000}         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringCs>[^\"\r\n]{1,1000}         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringCs>\"\"                      {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyString>\\.                         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyString,CopyStringCs>\"             {
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyLine );
                                        }
<CopyStringFtnDouble>[^\"\\\r\n]{1,1000} {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringFtnDouble>\\.                {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyStringFtnDouble>\"                 {
                                          outputChar(yyscanner,*yytext);
                                          BEGIN( CopyLine );
                                        }
<CopyStringFtn>[^\'\\\r\n]{1,1000}      {
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
                                          if (extractEndRawStringDelimiter(yytext)==yyextra->delimiter)
                                          {
                                            BEGIN( CopyLine );
                                          }
                                        }
<CopyRawString>[^)]{1,1000}             {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<CopyRawString>.                        {
                                          outputChar(yyscanner,*yytext);
                                        }
<CopyLine,LexCopyLine>{ID}/{BN}{0,80}"("        {
                                          yyextra->expectGuard = FALSE;
                                          Define *def=nullptr;
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
                                              (!yyextra->expandOnlyPredef || def->isPredefined)
                                             )
                                          {
                                            //printf("Found it! #args=%d\n",def->nargs);
                                            yyextra->roundCount=0;
                                            yyextra->defArgsStr=yytext;
                                            if (def->nargs==-1) // no function macro
                                            {
                                              QCString result = def->isPredefined && !def->expandAsDefined ?
                                                def->definition :
                                                expandMacro(yyscanner,yyextra->defArgsStr);
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
<CopyLine>{RulesDelim}                  {
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::Lex) REJECT;
                                          yyextra->lexRulesPart = !yyextra->lexRulesPart;
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
  /* start lex rule handling */
<CopyLine>{RulesSharp}                  {
                                          if (!yyextra->lexRulesPart) REJECT;
                                          if (yyextra->curlyCount) REJECT;
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(RulesPattern);
                                        }
<RulesPattern>{EscapeRulesChar}         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesPattern>{RulesCurly}              {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesPattern>{StartDouble}             {
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->lastContext = YY_START;
                                          BEGIN(RulesDouble);
                                        }
<RulesDouble,RulesRoundDouble>"\\\\"    {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesDouble,RulesRoundDouble>"\\\""    {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesDouble>"\""                       {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN( yyextra->lastContext ) ;
                                        }
<RulesRoundDouble>"\""                  {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(RulesRound) ;
                                        }
<RulesDouble,RulesRoundDouble>.         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesPattern>{StartSquare}             {
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->lastContext = YY_START;
                                          BEGIN(RulesSquare);
                                        }
<RulesSquare,RulesRoundSquare>{CHARCE}  {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesSquare,RulesRoundSquare>"\\[" |
<RulesSquare,RulesRoundSquare>"\\]"     {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesSquare>"]"                        {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(RulesPattern);
                                        }
<RulesRoundSquare>"]"                   {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(RulesRound) ;
                                        }
<RulesSquare,RulesRoundSquare>"\\\\" {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesSquare,RulesRoundSquare>.         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesPattern>{StartRoundQuest}         {
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->lastContext = YY_START;
                                          BEGIN(RulesRoundQuest);
                                        }
<RulesRoundQuest>{nl}                   {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRoundQuest>[^)]                   {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRoundQuest>")"                    {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(yyextra->lastContext);
                                        }
<RulesPattern>{StartRound}              {
                                          yyextra->roundCount++;
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->lastContext = YY_START;
                                          BEGIN(RulesRound);
                                        }
<RulesRound>{RulesCurly}                {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRound>{StartSquare}               {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(RulesRoundSquare);
                                        }
<RulesRound>{StartDouble}               {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(RulesRoundDouble);
                                        }
<RulesRound>{EscapeRulesChar}           {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRound>"("                         {
                                          yyextra->roundCount++;
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRound>")"                         {
                                          yyextra->roundCount--;
                                          outputArray(yyscanner,yytext,yyleng);
                                          if (!yyextra->roundCount) BEGIN( yyextra->lastContext ) ;
                                        }
<RulesRound>{nl}                        {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRound>{B}                         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesRound>.                           {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<RulesPattern>{B}                       {
                                          outputArray(yyscanner,yytext,yyleng);
                                          BEGIN(CopyLine);
                                        }
<RulesPattern>.                         {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
  /* end lex rule handling */
<CopyLine,LexCopyLine>{ID}              {
                                          Define *def=nullptr;
                                          if ((yyextra->includeStack.empty() || yyextra->curlyCount>0) &&
                                              yyextra->macroExpansion &&
                                              (def=isDefined(yyscanner,yytext)) &&
                                              def->nargs==-1 &&
                                              (!yyextra->expandOnlyPredef || def->isPredefined)
                                             )
                                          {
                                            QCString result=def->isPredefined && !def->expandAsDefined ?
                                              def->definition :
                                              expandMacro(yyscanner,yytext);
                                            outputString(yyscanner,result);
                                          }
                                          else
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                          }
                                        }
<CopyLine,LexCopyLine>"\\"\r?/\n        { // strip line continuation characters
                                          if (getLanguageFromFileName(yyextra->fileName)==SrcLangExt::Fortran) outputChar(yyscanner,*yytext);
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
<FindDefineArgs>{CCS}[*!]?              {
                                          yyextra->defArgsStr+=yytext;
                                          BEGIN(ArgCopyCComment);
                                        }
<FindDefineArgs>{CPPC}[/!].*\n/{B}*{CPPC}[/!] { // replace multi line C++ style comment by C style comment
                                          if (Config_getBool(MULTILINE_CPP_IS_BRIEF) && !Config_getBool(QT_AUTOBRIEF))
                                          {
                                            if (yytext[3]=='<') // preserve < before @brief
                                            {
                                              yyextra->defArgsStr+=QCString("/**< @brief ")+&yytext[4];
                                            }
                                            else
                                            {
                                              yyextra->defArgsStr+=QCString("/** @brief ")+&yytext[3];
                                            }
                                          }
                                          else
                                          {
                                            yyextra->defArgsStr+=QCString("/**")+&yytext[3];
                                          }
                                          BEGIN(ArgCopyCppComment);
                                        }
<FindDefineArgs>{CPPC}[/!].*\n          { // replace C++ single line style comment by C style comment
                                          if (Config_getBool(QT_AUTOBRIEF))
                                          {
                                            yyextra->defArgsStr+=QCString("/**")+&yytext[3]+" */";
                                          }
                                          else // add brief command explicitly when translating C++ to C comment style
                                          {
                                            if (yytext[3]=='<') // preserve < before @brief
                                            {
                                              yyextra->defArgsStr+=QCString("/**< @brief ")+&yytext[4]+" */";
                                            }
                                            else
                                            {
                                              yyextra->defArgsStr+=QCString("/** @brief ")+&yytext[3]+" */";
                                            }
                                          }
                                        }
<FindDefineArgs>{CPPC}.*\n              { // replace C++ single line style comment by C style comment
                                          yyextra->defArgsStr+=QCString("/*")+&yytext[2]+" */";
                                        }
<FindDefineArgs>\"                      {
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(ReadString);
                                        }
<FindDefineArgs>'                       {
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::Fortran) REJECT;
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
                                          yyextra->defArgsStr+=yytext;
                                          yyextra->yyLineNr++;
                                        }
<ArgCopyCComment>.                      {
                                          yyextra->defArgsStr+=yytext;
                                        }
<ArgCopyCppComment>^{B}*
<ArgCopyCppComment>{CPPC}[/!].*\n/{B}*{CPPC}[/!] { // replace multi line C++ style comment by C style comment
                                          const char *startContent = &yytext[3];
                                          if (startContent[0]=='<') startContent++;
                                          yyextra->defArgsStr+=startContent;
                                        }
<ArgCopyCppComment>{CPPC}[/!].*\n       { // replace C++ multie line style comment by C style comment
                                          const char *startContent = &yytext[3];
                                          if (startContent[0]=='<') startContent++;
                                          yyextra->defArgsStr+=QCString(startContent)+" */";
                                          BEGIN(FindDefineArgs);
                                        }
<ArgCopyCppComment>.                    { // unexpected character
                                          unput(*yytext);
                                          yyextra->defArgsStr+=" */";
                                          BEGIN(FindDefineArgs);
                                        }
<ReadString>"\""                        {
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(FindDefineArgs);
                                        }
<ReadString>"'"                         {
                                          if (getLanguageFromFileName(yyextra->fileName)!=SrcLangExt::Fortran) REJECT;
                                          yyextra->defArgsStr+=*yytext;
                                          BEGIN(FindDefineArgs);
                                        }

<ReadString>{CPPC}|{CCS}                {
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
<Command>"cmakedefine01"{B}+            {
                                          yyextra->potentialDefine += substitute(yytext,"cmakedefine01","     define  ");
                                          //printf("!!!DefName\n");
                                          yyextra->yyColNr+=(int)yyleng;
                                          BEGIN(CmakeDefName01);
                                        }
<Command>"ifdef"/{B}*"("                {
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr.clear();
                                          BEGIN(DefinedExpr2);
                                        }
<Command>"ifdef"/{B}+                   {
                                          //printf("Pre.l: ifdef\n");
                                          incrLevel(yyscanner);
                                          yyextra->guardExpr.clear();
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
                                          yyextra->guardExpr.clear();
                                          BEGIN(Guard);
                                        }
<Command>("elif"|"else"{B}*"if")/[ \t(!]        {
                                          if (!otherCaseDone(yyscanner))
                                          {
                                            yyextra->guardExpr.clear();
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
                                            yyextra->guardExpr.clear();
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
                                          if (yyextra->pragmaSet.find(yyextra->fileName.str())!=yyextra->pragmaSet.end())
                                          {
                                            outputChar(yyscanner,'\n');
                                            BEGIN(PragmaOnce);
                                          }
                                          else
                                          {
                                            yyextra->pragmaSet.insert(yyextra->fileName.data());
                                          }
                                        }
<PragmaOnce>.                           {}
<PragmaOnce>\n                          {}
<PragmaOnce><<EOF>>                     {
                                          yyextra->expectGuard = FALSE;
                                          BEGIN(Start);
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
<Guard>"true"/{B}|{B}*[\r]?\n           { yyextra->guardExpr+="1L"; }
<Guard>"false"/{B}|{B}*[\r]?\n          { yyextra->guardExpr+="0L"; }
<Guard>"not"/{B}                        { yyextra->guardExpr+='!'; }
<Guard>"not_eq"/{B}                     { yyextra->guardExpr+="!="; }
<Guard>"and"/{B}                        { yyextra->guardExpr+="&&"; }
<Guard>"or"/{B}                         { yyextra->guardExpr+="||"; }
<Guard>"bitand"/{B}                     { yyextra->guardExpr+="&"; }
<Guard>"bitor"/{B}                      { yyextra->guardExpr+="|"; }
<Guard>"xor"/{B}                        { yyextra->guardExpr+="^"; }
<Guard>"compl"/{B}                      { yyextra->guardExpr+="~"; }
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
                                              yyextra->guardExpr.clear();
                                              yyextra->lastGuardName.clear();
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
                                          if (yyextra->isImported)
                                          {
                                            BEGIN(EndImport);
                                          }
                                          else
                                          {
                                            readIncludeFile(yyscanner,yyextra->incName);
                                            BEGIN(Start);
                                          }
                                        }
<EndImport>{ENDIMPORTopt}/\n            {
                                          readIncludeFile(yyscanner,yyextra->incName);
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
                                          yyextra->defArgsStr.clear();
                                          yyextra->defText.clear();
                                          yyextra->defLitText.clear();
                                          yyextra->defName = yytext;
                                          yyextra->defVarArgs = FALSE;
                                          yyextra->defExtraSpacing.clear();
                                          yyextra->defContinue = false;
                                          BEGIN(DefineArg);
                                        }
<DefName>{ID}{B}+"1"/[ \r\t\n]          { // special case: define with 1 -> can be "guard"
                                          //printf("Define '%s'\n",yytext);
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = -1;
                                          yyextra->defArgsStr.clear();
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
                                            yyextra->lastGuardName.clear();
                                            yyextra->defText="1";
                                            yyextra->defLitText="1";
                                            BEGIN(DefineText);
                                          }
                                          else // define is a guard => hide
                                          {
                                            //printf("Found a guard %s\n",yytext);
                                            yyextra->defText.clear();
                                            yyextra->defLitText.clear();
                                            BEGIN(Start);
                                          }
                                          yyextra->expectGuard=FALSE;
                                        }
<DefName,CmakeDefName01>{ID}/{B}*"\n"   { // empty define
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = -1;
                                          yyextra->defName = yytext;
                                          yyextra->defArgsStr.clear();
                                          yyextra->defText.clear();
                                          yyextra->defLitText.clear();
                                          yyextra->defVarArgs = FALSE;
                                          //printf("Guard check: %s!=%s || %d\n",
                                          //    qPrint(yyextra->defName),qPrint(yyextra->lastGuardName),yyextra->expectGuard);
                                          if (yyextra->curlyCount>0 || yyextra->defName!=yyextra->lastGuardName || !yyextra->expectGuard)
                                          { // define may appear in the output
                                            QCString def = yyextra->potentialDefine + yyextra->defName;
                                            outputString(yyscanner,def);
                                            yyextra->quoteArg=FALSE;
                                            yyextra->insideComment=FALSE;
                                            if (YY_START == CmakeDefName01) yyextra->defText = "0";
                                            else if (yyextra->insideCS) yyextra->defText="1"; // for C#, use "1" as define text
                                            BEGIN(DefineText);
                                          }
                                          else // define is a guard => hide
                                          {
                                            //printf("Found a guard %s\n",yytext);
                                            yyextra->guardName = yytext;
                                            yyextra->lastGuardName.clear();
                                            BEGIN(Start);
                                          }
                                          yyextra->expectGuard=FALSE;
                                        }
<DefName>{ID}/{B}*                      { // define with content
                                          //printf("Define '%s'\n",yytext);
                                          yyextra->argMap.clear();
                                          yyextra->defArgs = -1;
                                          yyextra->defArgsStr.clear();
                                          yyextra->defText.clear();
                                          yyextra->defLitText.clear();
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
<SkipCond>{CMD}{CMD}                    { }
<SkipCond>^({B}*"*"+)?{B}{0,3}"~~~"[~]* {
                                          bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport || !yyextra->isSpecialComment)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->fenceChar='~';
                                            yyextra->fenceSize=(int)getFenceSize(yytext,yyleng);
                                            BEGIN(SkipCondVerbatim);
                                          }
                                        }
<SkipCond>^({B}*"*"+)?{B}{0,3}"```"[`]* {
                                          bool markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
                                          if (!markdownSupport || !yyextra->isSpecialComment)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->fenceChar='`';
                                            yyextra->fenceSize=(int)getFenceSize(yytext,yyleng);
                                            BEGIN(SkipCondVerbatim);
                                          }
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
                                            yyextra->fenceChar='~';
                                            yyextra->fenceSize=(int)getFenceSize(yytext,yyleng);
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
                                            yyextra->fenceChar='`';
                                            yyextra->fenceSize=(int)getFenceSize(yytext,yyleng);
                                            BEGIN(SkipVerbatim);
                                          }
                                        }
<SkipCComment>{CMD}{VERBATIM_LINE}      |
<SkipCComment>{CMD}{LITERAL_BLOCK}      { // escaped command
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                        }
<SkipCComment>{VERBATIM_LINE}.*/\n      { // normal command
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCComment>{LITERAL_BLOCK}           { // normal block command
                                          outputArray(yyscanner,yytext,yyleng);
                                          yyextra->yyLineNr+=QCString(yytext).contains('\n');
                                          determineBlockName(yyscanner);
                                          BEGIN(SkipVerbatim);
                                        }
<SkipCond>{CMD}{CMD}"cond"[ \t]+        {}// escaped cond command
<SkipCond>{CMD}"cond"/\n                |  
<SkipCond>{CMD}"cond"[ \t]+             { // cond command in a skipped cond section, this section has to be skipped as well
                                          // but has to be recorded to match the endcond command
                                          startCondSection(yyscanner," ");
                                        }
<SkipCComment>"{"[ \t]*"@code"/[ \t\n]  {
                                          outputArray(yyscanner,"@iliteral{code}",15);
                                          yyextra->javaBlock=1;
                                          BEGIN(JavaDocVerbatimCode);
                                        }
<SkipCComment>"{"[ \t]*"@literal"/[ \t\n]  {
                                          outputArray(yyscanner,"@iliteral",9);
                                          yyextra->javaBlock=1;
                                          BEGIN(JavaDocVerbatimCode);
                                        }
<SkipCComment,SkipCPPComment>{CMD}{CMD}"cond"[ \t\n]+ { // escaped cond command
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCPPComment>{CMD}"cond"[ \t]+       { // conditional section
                                          yyextra->ccomment=TRUE;
                                          yyextra->condCtx=YY_START;
                                          BEGIN(CondLineCpp);
                                        }
<SkipCComment>{CMD}"cond"[ \t]+ { // conditional section
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
<SkipCComment,SkipCPPComment>{CMD}"cond"{WSopt}/\n { // no guard
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
<SkipCond>{VERBATIM_LINE}.*/\n          { }
<SkipCond>{LITERAL_BLOCK}               {
                                          auto numNLs = QCString(yytext).contains('\n');
                                          yyextra->yyLineNr+=numNLs;
                                          for (int i = 0; i < numNLs; i++) outputChar(yyscanner,'\n');
                                          determineBlockName(yyscanner);
                                          BEGIN(SkipCondVerbatim);
                                        }

<SkipCond>.                             { }
<SkipCond>[^\/\!*\\@\n]+                { }
<SkipCond>{CPPC}[/!]                    { yyextra->ccomment=FALSE; }
<SkipCond>{CCS}[*!]                     { yyextra->ccomment=TRUE; }
<SkipCond,SkipCComment,SkipCPPComment>{CMD}{CMD}"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          if (!yyextra->skip)
                                          {
                                            outputArray(yyscanner,yytext,yyleng);
                                          }
                                        }
<SkipCond>{CMD}"endcond"/[^a-z_A-Z0-9\x80-\xFF]  {
                                          bool oldSkip = yyextra->skip;
                                          endCondSection(yyscanner);
                                          if (oldSkip && !yyextra->skip)
                                          {
                                            if (yyextra->ccomment)
                                            {
                                              outputArray(yyscanner,"/** ",4); // */
                                            }
                                            BEGIN(yyextra->condCtx);
                                          }
                                        }
<SkipCComment,SkipCPPComment>{CMD}"endcond"/[^a-z_A-Z0-9\x80-\xFF] {
                                          bool oldSkip = yyextra->skip;
                                          endCondSection(yyscanner);
                                          if (oldSkip && !yyextra->skip)
                                          {
                                            BEGIN(yyextra->condCtx);
                                          }
                                        }
<SkipCondVerbatim>{LITERAL_BLOCK_END}   { /* end of verbatim block */
                                          if (yytext[1]=='f' && yyextra->blockName==&yytext[2])
                                          {
                                            BEGIN(SkipCond);
                                          }
                                          else if (&yytext[4]==yyextra->blockName)
                                          {
                                            BEGIN(SkipCond);
                                          }
                                        }
<SkipVerbatim>{LITERAL_BLOCK_END}       { /* end of verbatim block */
                                          outputArray(yyscanner,yytext,yyleng);
                                          if (yytext[1]=='f' && yyextra->blockName==&yytext[2])
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                          else if (&yytext[4]==yyextra->blockName)
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipCondVerbatim>^({B}*"*"+)?{B}{0,3}"~~~"[~]*                 {
                                          if (yyextra->fenceSize==getFenceSize(yytext,yyleng) &&  yyextra->fenceChar=='~') 
                                          {
                                            BEGIN(SkipCond);
                                          }
                                        }
<SkipCondVerbatim>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
                                          if (yyextra->fenceSize==getFenceSize(yytext,yyleng) &&  yyextra->fenceChar=='`')
                                          {
                                            BEGIN(SkipCond);
                                          }
                                        }
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"~~~"[~]*                 {
                                          outputArray(yyscanner,yytext,yyleng);
                                          if (yyextra->fenceSize==getFenceSize(yytext,yyleng) &&  yyextra->fenceChar=='~')
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipVerbatim>^({B}*"*"+)?{B}{0,3}"```"[`]*                 {
                                          outputArray(yyscanner,yytext,yyleng);
                                          if (yyextra->fenceSize==getFenceSize(yytext,yyleng) &&  yyextra->fenceChar=='`')
                                          {
                                            BEGIN(SkipCComment);
                                          }
                                        }
<SkipCondVerbatim>{CCE}|{CCS}           { }
<SkipVerbatim>{CCE}|{CCS}               {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<JavaDocVerbatimCode>"{"                {
                                          if (yyextra->javaBlock==0)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->javaBlock++;
                                            outputArray(yyscanner,yytext,(int)yyleng);
                                          }
                                        }
<JavaDocVerbatimCode>"}"                {
                                          if (yyextra->javaBlock==0)
                                          {
                                            REJECT;
                                          }
                                          else
                                          {
                                            yyextra->javaBlock--;
                                            if (yyextra->javaBlock==0)
                                            {
                                              outputArray(yyscanner," @endiliteral ",14);
                                              BEGIN(SkipCComment);
                                            }
                                            else
                                            {
                                              outputArray(yyscanner,yytext,(int)yyleng);
                                            }
                                          }
                                        }
<JavaDocVerbatimCode>\n                 { /* new line in verbatim block */
                                          outputArray(yyscanner,yytext,(int)yyleng);
                                        }
<JavaDocVerbatimCode>.                  { /* any other character */
                                          outputArray(yyscanner,yytext,(int)yyleng);
                                        }
<SkipCondVerbatim>[^{*\\@\x06~`\n\/]+   { }
<SkipCComment,SkipVerbatim>[^{*\\@\x06~`\n\/]+ {
                                          outputArray(yyscanner,yytext,yyleng);
                                        }
<SkipCComment,SkipVerbatim,SkipCondVerbatim>\n           {
                                          yyextra->yyLineNr++;
                                          outputChar(yyscanner,'\n');
                                        }
<SkipCondVerbatim>.                     { }
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
<DefineText>"__VA_OPT__("{B}*"##"       {
                                          warn(yyextra->fileName,yyextra->yyLineNr,
                                               "'##' may not appear at the beginning of a __VA_OPT__()",
                                               yyextra->defName,yyextra->defLitText.stripWhiteSpace());
                                          yyextra->defText+="__VA_OPT__(";
                                          yyextra->defLitText+="__VA_OPT__(";
                                        }
<DefineText>"#"/"__VA_OPT__"            {
                                          yyextra->defText+=yytext;
                                          yyextra->defLitText+=yytext;
                                        }
<DefineText>"#"/{IDSTART}               {
                                          outputChar(yyscanner,' ');
                                          yyextra->quoteArg=TRUE;
                                          yyextra->idStart=true;
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
                                              if (yyextra->idStart)
                                              {
                                                warn(yyextra->fileName,yyextra->yyLineNr,
                                                  "'#' is not followed by a macro parameter '{}': '{}'",
                                                  yyextra->defName,yyextra->defLitText.stripWhiteSpace());
                                              }
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
                                          yyextra->idStart=false;
                                        }
<CopyCComment>.                         {
                                          yyextra->defLitText+=yytext;
                                          yyextra->defText+=yytext;
                                        }
<DefineText>\\[\r]?\n                   {
                                          yyextra->defLitText+=yytext;
                                          outputChar(yyscanner,'\\');
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
                                            warn(yyextra->fileName,yyextra->yyLineNr,
                                                 "'##' cannot occur at the beginning of a macro definition '{}': '{}'",
                                                 yyextra->defName,yyextra->defLitText.stripWhiteSpace());
                                          }
                                          else if (yyextra->defText.endsWith("##"))
                                          {
                                            warn(yyextra->fileName,yyextra->yyLineNr,
                                                 "'##' cannot occur at the end of a macro definition '{}': '{}'",
                                                 yyextra->defName,yyextra->defLitText.stripWhiteSpace());
                                          }
                                          else if (yyextra->defText.endsWith("#"))
                                          {
                                            warn(yyextra->fileName,yyextra->yyLineNr,
                                                 "expected formal parameter after # in macro definition '{}': '{}'",
                                                 yyextra->defName,yyextra->defLitText.stripWhiteSpace());
                                          }
                                          if (!comment.isEmpty())
                                          {
                                            outputString(yyscanner,comment);
                                            yyextra->defLitText=yyextra->defLitText.left(yyextra->defLitText.length()-comment.length()-1);
                                          }
                                          outputChar(yyscanner,'\n');
                                          yyextra->defLitText+=yytext;
                                          Define *def=nullptr;
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
                                              def->fileName = yyextra->fileName;
                                              def->lineNr = yyextra->yyLineNr-yyextra->yyMLines;
                                              def->columnNr = yyextra->yyColNr;
                                            }
                                            else
                                            {
                                              if (def->fileName != yyextra->fileName && !yyextra->expandOnlyPredef) addDefine(yyscanner);
                                              //printf("error: define %s is defined more than once!\n",qPrint(yyextra->defName));
                                            }
                                          }
                                          yyextra->argMap.clear();
                                          yyextra->yyLineNr++;
                                          yyextra->yyColNr=1;
                                          yyextra->lastGuardName.clear();
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
<DefineText>{NUMBER}                    {
                                          outputSpaces(yyscanner,yytext);
                                          yyextra->defText += yytext;
                                          yyextra->defLitText+=yytext;
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
                                          TRACE("End of include file");
                                          //printf("Include stack depth=%d\n",yyextra->includeStack.size());
                                          if (yyextra->includeStack.empty())
                                          {
                                            TRACE("Terminating scanner");
                                            yyterminate();
                                          }
                                          else
                                          {
                                            QCString toFileName = yyextra->fileName;
                                            const std::unique_ptr<FileState> &fs=yyextra->includeStack.back();
                                            //fileDefineCache->merge(yyextra->fileName,fs->fileName);
                                            YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
                                            yy_switch_to_buffer( fs->bufState, yyscanner );
                                            yy_delete_buffer( oldBuf, yyscanner );
                                            yyextra->yyLineNr    = fs->lineNr;
                                            //preYYin = fs->oldYYin;
                                            yyextra->inputBuf    = fs->oldFileBuf;
                                            yyextra->inputBufPos = fs->oldFileBufPos;
                                            yyextra->curlyCount  = fs->curlyCount;
                                            setFileName(yyscanner,fs->fileName);
                                            TRACE("switching to {}",yyextra->fileName);

                                            // Deal with file changes due to
                                            // #include's within { .. } blocks
                                            QCString lineStr(15+yyextra->fileName.length(), QCString::ExplicitSize);
                                            lineStr.sprintf("# %d \"%s\" 2",yyextra->yyLineNr,qPrint(yyextra->fileName));
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
                                                g_defineManager.addInclude(yyextra->fileName.str(),toFileName.str());
                                                g_defineManager.store(toFileName.str(),yyextra->localDefines);
                                              }
                                              else
                                              {
                                                if (Debug::isFlagSet(Debug::Preprocessor))
                                                {
                                                  Debug::print(Debug::Preprocessor,0,"#include {}: was already processed by another thread! not storing data...\n",toFileName);
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
                                          if (YY_START==SkipVerbatim || YY_START == SkipCondVerbatim || YY_START==SkipCond || YY_START==IDLquote || YY_START == PragmaOnce)
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
                                              yyextra->lastGuardName.clear(); // reset guard in case the #define is documented!
                                            }
                                            else
                                            {
                                              yyextra->isSpecialComment = false;
                                            }
                                            BEGIN(SkipCComment);
                                          }
                                        }
<*>{CPPC}[/!]?                          {
                                          if (YY_START==SkipVerbatim || YY_START == SkipCondVerbatim || YY_START==SkipCond || getLanguageFromFileName(yyextra->fileName)==SrcLangExt::Fortran || YY_START==IDLquote || YY_START == PragmaOnce)
                                          {
                                            REJECT;
                                          }
                                          else if (YY_START==RulesRoundDouble)
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
                                              yyextra->lastGuardName.clear(); // reset guard in case the #define is documented!
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

static int yyread(yyscan_t yyscanner,char *buf,int max_size)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  int bytesInBuf = static_cast<int>(state->inputBuf->size())-state->inputBufPos;
  int bytesToCopy = std::min(max_size,bytesInBuf);
  memcpy(buf,state->inputBuf->data()+state->inputBufPos,bytesToCopy);
  state->inputBufPos+=bytesToCopy;
  return bytesToCopy;
}

static yy_size_t getFenceSize(char *txt, yy_size_t leng)
{
  yy_size_t fenceSize = 0;
  for (size_t i = 0; i < leng; i++)
  {
     if (txt[i] != ' ' && txt[i] != '*' && txt[i] != '\t') break;
     fenceSize++;
  }
  return leng-fenceSize;
}

static void setFileName(yyscan_t yyscanner,const QCString &name)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  bool ambig = false;
  FileInfo fi(name.str());
  state->fileName=fi.absFilePath();
  state->yyFileDef=findFileDef(Doxygen::inputNameLinkedMap,state->fileName,ambig);
  if (state->yyFileDef==nullptr) // if this is not an input file check if it is an include file
  {
    state->yyFileDef=findFileDef(Doxygen::includeNameLinkedMap,state->fileName,ambig);
  }
  //printf("setFileName(%s) state->fileName=%s state->yyFileDef=%p\n",
  //    name,qPrint(state->fileName),state->yyFileDef);
  if (state->yyFileDef && state->yyFileDef->isReference()) state->yyFileDef=nullptr;
  state->insideIDL = getLanguageFromFileName(state->fileName)==SrcLangExt::IDL;
  state->insideCS = getLanguageFromFileName(state->fileName)==SrcLangExt::CSharp;
  state->insideFtn = getLanguageFromFileName(state->fileName)==SrcLangExt::Fortran;
  EntryType section = guessSection(state->fileName);
  state->isSource = section.isHeader() || section.isSource();
}

static void incrLevel(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  state->levelGuard.push(false);
  //printf("%s line %d: incrLevel %d\n",qPrint(yyextra->fileName),yyextra->yyLineNr,yyextra->levelGuard.size());
}

static void decrLevel(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("%s line %d: decrLevel %d\n",qPrint(state->fileName),state->yyLineNr,state->levelGuard.size());
  if (!state->levelGuard.empty())
  {
    state->levelGuard.pop();
  }
  else
  {
    warn(state->fileName,state->yyLineNr,"More #endif's than #if's found.");
  }
}

static bool otherCaseDone(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->levelGuard.empty())
  {
    warn(state->fileName,state->yyLineNr,"Found an #else without a preceding #if.");
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


static std::unique_ptr<FileState> checkAndOpenFile(yyscan_t yyscanner,const QCString &fileName,bool &alreadyProcessed)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  alreadyProcessed = FALSE;
  std::unique_ptr<FileState> fs;
  //printf("checkAndOpenFile(%s)\n",qPrint(fileName));
  FileInfo fi(fileName.str());
  if (fi.exists() && fi.isFile())
  {
    const StringVector &exclPatterns = Config_getList(EXCLUDE_PATTERNS);
    if (patternMatch(fi,exclPatterns)) return nullptr;

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
      return nullptr;
    }
    //printf("#include %s\n",qPrint(absName));

    fs = std::make_unique<FileState>();
    if (!readInputFile(absName,fs->fileBuf))
    { // error
      //printf("  error reading\n");
      fs.reset();
    }
    else
    {
      addTerminalCharIfMissing(fs->fileBuf,'\n');
      fs->oldFileBuf    = state->inputBuf;
      fs->oldFileBufPos = state->inputBufPos;
    }
  }
  return fs;
}

static std::unique_ptr<FileState> findFile(yyscan_t yyscanner, const QCString &fileName,bool localInclude,bool &alreadyProcessed)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  //printf("** findFile(%s,%d) state->fileName=%s\n",qPrint(fileName),localInclude,qPrint(state->fileName));
  if (Portable::isAbsolutePath(fileName))
  {
    auto fs = checkAndOpenFile(yyscanner,fileName,alreadyProcessed);
    if (fs)
    {
      setFileName(yyscanner,fileName);
      state->yyLineNr=1;
      return fs;
    }
    else if (alreadyProcessed)
    {
      return nullptr;
    }
  }
  if (localInclude && !state->fileName.isEmpty())
  {
    FileInfo fi(state->fileName.str());
    if (fi.exists())
    {
      QCString absName = QCString(fi.dirPath(TRUE))+"/"+fileName;
      auto fs = checkAndOpenFile(yyscanner,absName,alreadyProcessed);
      if (fs)
      {
        setFileName(yyscanner,absName);
        state->yyLineNr=1;
        return fs;
      }
      else if (alreadyProcessed)
      {
        return nullptr;
      }
    }
  }
  if (state->pathList.empty())
  {
    return nullptr;
  }
  for (auto path : state->pathList)
  {
    QCString absName = path+"/"+fileName;
    //printf("  Looking for %s in %s\n",fileName,qPrint(path));
    auto fs = checkAndOpenFile(yyscanner,absName,alreadyProcessed);
    if (fs)
    {
      setFileName(yyscanner,absName);
      state->yyLineNr=1;
      //printf("  -> found it\n");
      return fs;
    }
    else if (alreadyProcessed)
    {
      return nullptr;
    }
  }
  bool ambig = false;
  FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,fileName,ambig);
  if (fd && !ambig) // fallback in case the file is uniquely named in the input, use that one
  {
    auto fs = checkAndOpenFile(yyscanner,fd->absFilePath(),alreadyProcessed);
    if (fs)
    {
      setFileName(yyscanner,fd->absFilePath());
      state->yyLineNr=1;
      //printf("  -> found it\n");
      return fs;
    }
  }
  return nullptr;
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
            // only /*!< ... */ or /**< ... */ are treated as a comment for the macro name,
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

static int getNextChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint32_t &pos);
static int getCurrentChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint32_t pos);
static void unputChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint32_t &pos,char c);
static bool expandExpression(yyscan_t yyscanner,QCString &expr,QCString *rest,int pos,int level);

static QCString stringize(const QCString &s)
{
  QCString result;
  uint32_t i=0;
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
                                       uint32_t &pos,char term,QCString &arg)
{
  int cc;
  while ((cc=getNextChar(yyscanner,expr,rest,pos))!=EOF && cc!=0)
  {
    if (cc=='\\')
    {
      arg+=(char)cc;
      cc=getNextChar(yyscanner,expr,rest,pos);
    }
    else if (cc==term)
    {
      return;
    }
    arg+=(char)cc;
  }
}

static void skipCommentMacroName(yyscan_t yyscanner, const QCString &expr, QCString *rest,
                                 int &cc, uint32_t &j, int &len)
{
  bool changed = false;

  do
  {
    changed = false;
    while ((cc=getCurrentChar(yyscanner,expr,rest,j))!=EOF && cc!='\n' && isspace(cc))
    {
      len++;
      getNextChar(yyscanner,expr,rest,j);
    }

    if (cc=='/') // possible start of a comment
    {
      int prevChar = '\0';
      getNextChar(yyscanner,expr,rest,j);
      if ((cc=getCurrentChar(yyscanner,expr,rest,j))!=EOF && cc == '*') // we have a comment
      {
        while ((cc=getNextChar(yyscanner,expr,rest,j))!=EOF && cc!=0)
        {
          if (cc == '/' && prevChar == '*') break; // we have an end of comment
          prevChar = cc;
        }
        if (cc != EOF) changed = true;
      }
    }
  } while (changed);
}

// Expand C++20's __VA_OPT__(x) to either x if hasOptionalArgs==true or to the empty string if false
static QCString expandVAOpt(const QCString &vaStr,bool hasOptionalArgs)
{
  //printf("expandVAOpt(vaStr=%s,hasOptionalArgs=%d)\n",qPrint(vaStr),hasOptionalArgs);
  QCString result;
  int vo=0, vp=0;
  result.clear();
  int vl = static_cast<int>(vaStr.length());
  while ((vo = vaStr.find("__VA_OPT__(",vp))!=-1)
  {
    bool hasHash = vo>0 && vaStr.at(vo-1)=='#';
    if (hasHash)
    {
      result+=vaStr.mid(vp,vo-vp-1); // don't copy #
      result+="\"";
    }
    else
    {
      result+=vaStr.mid(vp,vo-vp);
    }
    int ve=vo+11; // skip over '__VA_OPT__(' part
    int bc=1;
    while (bc>0 && ve<vl)
    {
      if (vaStr[ve]==')') bc--;
      else if (vaStr[ve]=='(') bc++;
      ve++;
    }
    // ve points to end of __VA_OPT__(....)
    if (bc==0 && hasOptionalArgs)
    {
      QCString voStr = vaStr.mid(vo+11,ve-vo-12);
      //printf("vo=%d ve=%d voStr=%s\n",vo,ve,qPrint(voStr));
      result+=voStr; // take 'x' from __VA_OPT__(x)
    }
    if (hasHash)
    {
      result+="\"";
    }
    vp=ve;
  }
  result+=vaStr.mid(vp);
  //printf("vaStr='%s'\n   -> '%s'\n",qPrint(vaStr),qPrint(result));
  return result;
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
  //printf(">replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s') level=%zu\n",qPrint(expr),rest ? qPrint(*rest) : 0,pos,qPrint(def->name),preYYget_extra(yyscanner)->levelGuard.size());
  uint32_t j=pos;
  len=0;
  result.clear();
  int cc;

  skipCommentMacroName(yyscanner, expr, rest, cc, j, len);

  if (cc!='(')
  {
    if (cc!=':') // don't add spaces for colons
    {
      unputChar(yyscanner,expr,rest,j,' ');
    }
    return FALSE;
  }
  getNextChar(yyscanner,expr,rest,j); // eat the '(' character

  std::map<std::string,std::string> argTable;  // list of arguments
  QCString arg;
  int argCount=0;
  int argCountNonEmpty=0;
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
          {
            arg+=c;
          }
        }
      }
      else if (c==')' || c==',') // last or next argument found
      {
        if (c==',' && argCount==def->nargs-1 && def->varArgs)
        {
          expandExpression(yyscanner,arg,nullptr,0,level+1);
          arg=arg.stripWhiteSpace();
          arg+=',';
        }
        else
        {
          expandExpression(yyscanner,arg,nullptr,0,level+1);
          arg=arg.stripWhiteSpace();
          QCString argKey;
          argKey.sprintf("@%d",argCount++); // key name
          if (c==',' || !arg.isEmpty()) argCountNonEmpty++;
          // add argument to the lookup table
          argTable.emplace(toStdString(argKey), toStdString(arg));
          arg.clear();
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
    uint32_t k=0;
    // substitution of all formal arguments
    QCString resExpr;
    QCString d=def->definition.stripWhiteSpace();
    //printf("varArgs=%d argCount=%d def->nargs=%d d=%s\n",def->varArgs,argCount,def->nargs,qPrint(d));
    if (def->varArgs) d = expandVAOpt(d,argCountNonEmpty!=def->nargs-1);
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
            QCString substArg = it->second;
            //printf("substArg='%s'\n",qPrint(substArg));
            // only if no ## operator is before or after the argument
            // marker we do macro expansion.
            if (!hash)
            {
              expandExpression(yyscanner,substArg,nullptr,0,level+1);
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
        else if (k>2 && inString && d.at(k)=='\"' && (d.at(k-1)!='\\' || d.at(k-2)=='\\'))
        {
          inString=FALSE; // leaving a literal string
        }
        resExpr+=d.at(k++);
      }
    }
    len=j-pos;
    result=resExpr;
    //printf("<replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s',result='%s') level=%zu return=TRUE\n",qPrint(expr),rest ? qPrint(*rest) : 0,pos,qPrint(def->name),qPrint(result),preYYget_extra(yyscanner)->levelGuard.size());
    return TRUE;
  }
  //printf("<replaceFunctionMacro(expr='%s',rest='%s',pos=%d,def='%s',result='%s') level=%zu return=FALSE\n",qPrint(expr),rest ? qPrint(*rest) : 0,pos,qPrint(def->name),qPrint(result),preYYget_extra(yyscanner)->levelGuard.size());
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

static void addSeparatorsIfNeeded(yyscan_t yyscanner,const QCString &expr,QCString &resultExpr,QCString &restExpr,int pos)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (!state->nospaces)
  {
    // peek back in the stream, for a colon character
    char ccPrev = pos==0 || (int)expr.length()<pos ? state->prevChar : expr.at(pos-1);
    QCString leftSpace = ccPrev!=':' && ccPrev!=' ' ? " " : "";
    int ccNext = 0;
    restExpr=restExpr.stripWhiteSpace();
    if (restExpr.isEmpty()) // peek ahead in the stream for non-whitespace
    {
      uint32_t j=(uint32_t)resultExpr.length();
      while ((ccNext=getNextChar(yyscanner,resultExpr,nullptr,j))!=EOF && ccNext==' ') { }
      if (ccNext != EOF) unputChar(yyscanner,resultExpr,nullptr,j,(char)ccNext);
    }
    else // take first char from remainder
    {
      ccNext=restExpr.at(0);
    }
    // don't add whitespace before a colon
    QCString rightSpace = ccNext!=':' && ccNext!=' ' ? " " : "";
    //printf("ccPrev='%c' ccNext='%c' p=%d expr=%zu restExpr='%s' left='%s' right='%s'\n",
    //    ccPrev,ccNext,pos,expr.length(),qPrint(restExpr),qPrint(leftSpace),qPrint(rightSpace));
    resultExpr=leftSpace+resultExpr+rightSpace;
  }
}

/*! performs recursive macro expansion on the string \a expr
 *  starting at position \a pos.
 *  May read additional characters from the input while re-scanning!
 */
static bool expandExpression(yyscan_t yyscanner,QCString &expr,QCString *rest,int pos,int level)
{
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
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
  int i=pos, l=0, p=0, len=0;
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
        bool expanded=false;
        Define *def=isDefined(yyscanner,macroName);
        // In case EXPAND_ONLY_PREDEF is enabled prevent expansion unless the macro was explicitly
        // predefined
        if (yyextra->expandOnlyPredef && def && !def->isPredefined) def=nullptr;
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
          expMacro=def->definition.stripWhiteSpace();
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

        if (replaced) // expand the macro and rescan the expression
        {
          //printf(" replacing '%s'->'%s'\n",qPrint(expr.mid(p,len)),qPrint(expMacro));
          QCString resultExpr=expMacro;
          QCString restExpr=expr.right(expr.length()-len-p);
          addSeparatorsIfNeeded(yyscanner,expr,resultExpr,restExpr,p);
          processConcatOperators(resultExpr);
          //printf(" macroName=%s restExpr='%s' def->nonRecursive=%d\n",qPrint(macroName),qPrint(restExpr),def ? def->nonRecursive : false);
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
            //printf("expanded '%s' + '%s' + '%s'\n",qPrint(expr.left(p)),qPrint(resultExpr),qPrint(restExpr));
            expr=expr.left(p)+resultExpr+restExpr;
            i=p;
          }
          else
          {
            //printf("not expanded '%s' + @- '%s'\n",qPrint(expr.left(p)),qPrint(expr.right(expr.length()-p)));
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
  if (inputStr==nullptr) return inputStr;
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
  static const std::vector<std::string> signs = { "signed", "unsigned" };
  struct TypeInfo { std::string name; size_t size; };
  static const std::vector<TypeInfo> types = {
    { "short int",     sizeof(short int)     },
    { "long long int", sizeof(long long int) },
    { "long int",      sizeof(long int)      },
    { "long long",     sizeof(long long)     },
    { "long double",   sizeof(long double)   },
    { "int",           sizeof(int)           },
    { "short",         sizeof(short)         },
    { "bool",          sizeof(bool)          },
    { "long",          sizeof(long)          },
    { "char",          sizeof(char)          },
    { "float",         sizeof(float)         },
    { "double",        sizeof(double)        },
  };

  // Check if string p starts with basic types ending with a ')', such as 'signed long)' or ' float )'
  // and return the pointer just past the ')' and the size of the type as a tuple.
  // If the pattern is not found the tuple (nullptr,0) is returned.
  auto process_cast_or_sizeof = [](const char *p) -> std::pair<const char *,size_t>
  {
    const char *q = p;
    while (*q==' ' || *q=='\t') q++;
    bool found=false;
    size_t size = sizeof(int); // '(signed)' or '(unsigned)' is an int type
    for (const auto &sgn : signs)
    {
      if (qstrncmp(q,sgn.c_str(),sgn.length())==0) { q+=sgn.length(); found=true; }
    }
    if (!found || *q==' ' || *q=='\t' || *q==')') // continue searching
    {
      while (*q==' ' || *q=='\t') q++;
      for (const auto &t : types)
      {
        if (qstrncmp(q,t.name.c_str(),t.name.length())==0)
        {
          q += t.name.length();
          size = t.size;
          break;
        }
      }
      while (*q==' ' || *q=='\t') q++;
      if (*q==')') return std::make_pair(++q,size);
    }
    return std::make_pair(nullptr,0);
  };

  //printf("removeIdsAndMarkers(%s)\n",qPrint(s));
  if (s.isEmpty()) return s;
  const char *p=s.data();
  bool inNum=FALSE;
  QCString result;
  if (p)
  {
    char c = 0;
    while ((c=*p))
    {
      if (c=='(') // potential cast, ignore it
      {
        const char *q = process_cast_or_sizeof(p+1).first;
        //printf("potential cast:\nin:  %s\nout: %s\n",p,q);
        if (q)
        {
          p=q;
          continue;
        }
      }
      else if (c=='s' && literal_at(p,"sizeof")) // sizeof(...)
      {
        const char *q = p+6;
        while (*q==' ' || *q=='\t') q++;
        if (*q=='(')
        {
          auto r = process_cast_or_sizeof(q+1);
          //printf("sizeof:\nin:  %s\nout: %zu%s\n--> sizeof=%zu\n",p,r.second,r.first,r.second);
          if (r.first)
          {
            result+=QCString().setNum(r.second);
            p=r.first;
            continue;
          }
        }
      }

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
        if (literal_at(p,"defined ") || literal_at(p,"defined("))
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
        while ((c=*p) && isspace((uint8_t)c)) p++;
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
  QCString result;
  if (p)
  {
    char c = 0;
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
                {
                  result+=c;
                  p++;
                }
                else
                {
                  result+=c;
                }
                pc=c;
                c=*++p;
              }
              if (*p)
              {
                result+=c;
                p++;
              }
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
  QCString ee=expr;
  ee = removeMarkers(ee);
  state->expanded.clear();
  expandExpression(yyscanner,e,nullptr,0,0);
  //printf("after expansion '%s'\n",qPrint(e));
  e = removeIdsAndMarkers(e);
  if (e.isEmpty()) return FALSE;
  //printf("parsing '%s'\n",qPrint(e));
  return state->constExpParser.parse(state->fileName.data(),state->yyLineNr,e.str(),ee.str());
}

/*! expands the macro definition in \a name
 *  If needed the function may read additional characters from the input
 */

static QCString expandMacro(yyscan_t yyscanner,const QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  state->prevChar = yyscanner->yytext_r > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf ? *(yyscanner->yytext_r-1) : 0;
  QCString n=name;
  state->expanded.clear();
  expandExpression(yyscanner,n,nullptr,0,0);
  n=removeMarkers(n);
  state->prevChar=0;
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
  def.fileName   = state->fileName;
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
    def.expandAsDefined=TRUE;
  }
  auto it = state->localDefines.find(def.name.str());
  if (it!=state->localDefines.end()) // redefine
  {
    state->localDefines.erase(it);
  }
  state->localDefines.emplace(def.name.str(),def);
}

static void addMacroDefinition(yyscan_t yyscanner)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->skip) return; // do not add this define as it is inside a
                      // conditional section (cond command) that is disabled.

  Define define;
  define.fileName = state->fileName;
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
    char c = 0;
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
  if (state->includeStack.empty() || state->curlyCount>0) (*state->outputBuf)+=c;
}

static inline void outputArray(yyscan_t yyscanner,const char *a,yy_size_t len)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.empty() || state->curlyCount>0) (*state->outputBuf)+=std::string_view(a,len);
}

static inline void outputString(yyscan_t yyscanner,const QCString &a)
{
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  if (state->includeStack.empty() || state->curlyCount>0) (*state->outputBuf)+=a.str();
}

static inline void outputSpace(yyscan_t yyscanner,char c)
{
  if (c=='\t') outputChar(yyscanner,'\t');
  else outputChar(yyscanner,' ');
}

static inline void outputSpaces(yyscan_t yyscanner,char *s)
{
  const char *p=s;
  char c = 0;
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

static void determineBlockName(yyscan_t yyscanner)
{
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
  yyextra->fenceSize=0;
  char c=0;
  if (yytext[1]=='f' && ((c=yytext[2])=='[' || c=='{' || c=='(' || c=='$'))
  {
    switch (c)
    {
      case '[': yyextra->blockName="]"; break;
      case '{': yyextra->blockName="}"; break;
      case '(': yyextra->blockName=")"; break;
      case '$': yyextra->blockName="$"; break;
      default: break;
    }
    yyextra->blockName=yyextra->blockName.stripWhiteSpace();
  }
  else
  {
    QCString bn=QCString(&yytext[1]).stripWhiteSpace();
    if (bn=="startuml")
    {
      yyextra->blockName="uml";
    }
    else
    {
      int i = bn.find('{'); // for \code{.c}
      if (i!=-1) bn=bn.left(i).stripWhiteSpace();
      yyextra->blockName=bn;
    }
  }
}

static void readIncludeFile(yyscan_t yyscanner,const QCString &inc)
{
  AUTO_TRACE("inc={}",inc);
  YY_EXTRA_TYPE state = preYYget_extra(yyscanner);
  uint32_t i=0;

  // find the start of the include file name
  while (i<inc.length() &&
         (inc.at(i)==' ' || inc.at(i)=='"' || inc.at(i)=='<')
        ) i++;
  uint32_t s=i;

  // was it a local include?
  bool localInclude = s>0 && inc.at(s-1)=='"';

  // find the end of the include file name
  while (i<inc.length() && inc.at(i)!='"' && inc.at(i)!='>') i++;

  if (s<inc.length() && i>s) // valid include file name found
  {
    // extract include path+name
    QCString incFileName=inc.mid(s,i-s).stripWhiteSpace();
    if (incFileName.endsWith(".exe") || incFileName.endsWith(".dll") || incFileName.endsWith(".tlb"))
    {
      // skip imported binary files (e.g. M$ type libraries)
      return;
    }

    QCString oldFileName = state->fileName;
    FileDef *oldFileDef  = state->yyFileDef;
    int oldLineNr        = state->yyLineNr;
    //printf("Searching for '%s'\n",qPrint(incFileName));

    QCString absIncFileName = determineAbsoluteIncludeName(state->fileName,incFileName);

    // findFile will overwrite state->yyFileDef if found
    std::unique_ptr<FileState> fs;
    bool alreadyProcessed = FALSE;
    //printf("calling findFile(%s)\n",qPrint(incFileName));
    fs=findFile(yyscanner,absIncFileName,localInclude,alreadyProcessed); // see if the absolute include file can be found
    if (fs)
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
        Debug::print(Debug::Preprocessor,0,"#include {}: parsing...\n",incFileName);
      }

      if (state->includeStack.empty() && oldFileDef)
      {
        PreIncludeInfo *ii = state->includeRelations.find(absIncFileName);
        if (ii==nullptr)
        {
          bool ambig = false;
          FileDef *incFd = findFileDef(Doxygen::inputNameLinkedMap,absIncFileName,ambig);
          state->includeRelations.add(
              absIncFileName,
              oldFileDef,
              ambig ? nullptr : incFd,
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
      //state->curlyCount = 0; // don't reset counter, see issue #10997
      fs->lexRulesPart = state->lexRulesPart;
      state->lexRulesPart = false;
      // push the state on the stack
      FileState *fs_ptr = fs.get();
      state->includeStack.push_back(std::move(fs));
      // set the scanner to the include file

      // Deal with file changes due to
      // #include's within { .. } blocks
      QCString lineStr(state->fileName.length()+20, QCString::ExplicitSize);
      lineStr.sprintf("# 1 \"%s\" 1\n",qPrint(state->fileName));
      outputString(yyscanner,lineStr);

      AUTO_TRACE_ADD("Switching to include file {}",incFileName);
      state->expectGuard=TRUE;
      state->inputBuf   = &fs_ptr->fileBuf;
      state->inputBufPos=0;
      yy_switch_to_buffer(yy_create_buffer(0, YY_BUF_SIZE, yyscanner),yyscanner);
    }
    else
    {
      if (alreadyProcessed) // if this header was already process we can just copy the stored macros
                           // in the local context
      {
        std::lock_guard<std::mutex> lock(g_globalDefineMutex);
        g_defineManager.addInclude(state->fileName.str(),absIncFileName.str());
        g_defineManager.retrieve(absIncFileName.str(),state->contextDefines);
      }

      if (state->includeStack.empty() && oldFileDef)
      {
        PreIncludeInfo *ii = state->includeRelations.find(absIncFileName);
        if (ii==nullptr)
        {
          bool ambig = false;
          FileDef *incFd = findFileDef(Doxygen::inputNameLinkedMap,absIncFileName,ambig);
          ii = state->includeRelations.add(absIncFileName,
              oldFileDef,
              ambig ? nullptr : incFd,
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
          Debug::print(Debug::Preprocessor,0,"#include {}: already processed! skipping...\n",incFileName);
        }
        else
        {
          Debug::print(Debug::Preprocessor,0,"#include {}: not found! skipping...\n",incFileName);
        }
        //printf("error: include file %s not found\n",yytext);
      }
      if (localInclude && !state->includeStack.empty() && state->curlyCount>0 && !alreadyProcessed) // failed to find #include inside { ... }
      {
        warn(state->fileName,state->yyLineNr,"include file {} not found, perhaps you forgot to add its directory to INCLUDE_PATH?",incFileName);
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
  bool expResult = prs.parse(state->fileName.data(),state->yyLineNr,sectId.data());
  state->condStack.emplace(std::make_unique<preYY_CondCtx>(state->fileName,state->yyLineNr,sectId,state->skip));
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
    warn(state->fileName,state->yyLineNr,"the \\endcond does not have a corresponding \\cond in this file");
    state->skip=FALSE;
  }
  else
  {
    const std::unique_ptr<preYY_CondCtx> &ctx = state->condStack.top();
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
    char c = 0;
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

static int getNextChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint32_t &pos)
{
  //printf("getNextChar(%s,%s,%d)\n",qPrint(expr),rest ? rest->data() : 0,pos);
  if (pos<expr.length())
  {
    //printf("  expr()='%c'\n",expr.at(pos));
    return expr.at(pos++);
  }
  else if (rest && !rest->isEmpty())
  {
    int cc=rest->at(0);
    *rest=rest->right(rest->length()-1);
    //printf("  rest='%c'\n",cc);
    return cc;
  }
  else
  {
    int cc=yyinput(yyscanner);
    //printf("  yyinput()='%c' %d\n",cc,EOF);
    return cc;
  }
}

static int getCurrentChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint32_t pos)
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

static void unputChar(yyscan_t yyscanner,const QCString &expr,QCString *rest,uint32_t &pos,char c)
{
  //printf("unputChar(%s,%s,%d,%c)\n",qPrint(expr),rest ? rest->data() : 0,pos,c);
  if (pos<expr.length())
  {
    pos++;
  }
  else if (rest)
  {
    //printf("  prepending '%c' to rest!\n",c);
    char cs[2];cs[0]=c;cs[1]='\0';
    rest->prepend(cs);
  }
  else
  {
    //printf("  yyunput()='%c'\n",c);
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
    Define *d=nullptr;
    auto it = map.find(name.str());
    if (it!=map.end())
    {
      d = &it->second;
      if (d->undef)
      {
        undef=true;
        d=nullptr;
      }
    }
    return d;
  };

  Define *def = findDefine(state->localDefines);
  if (def==nullptr && !undef)
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
      //printf("predefined function macro '%s'\n",qPrint(ds));
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
        state->contextDefines.emplace(def.name.str(),def);

        //printf("#define '%s' '%s' #nargs=%d hasVarArgs=%d\n",
        //  qPrint(def.name),qPrint(def.definition),def.nargs,def.varArgs);
      }
    }
    else if (!ds.empty()) // predefined non-function macro definition
    {
      //printf("predefined normal macro '%s'\n",qPrint(ds));
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
        state->contextDefines.emplace(def.name.str(),def);
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

void Preprocessor::processFile(const QCString &fileName,const std::string &input,std::string &output)
{
  AUTO_TRACE("fileName={}",fileName);
  yyscan_t yyscanner = p->yyscanner;
  YY_EXTRA_TYPE state = preYYget_extra(p->yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

#ifdef FLEX_DEBUG
  preYYset_debug(Debug::isFlagSet(Debug::Lex_pre)?1:0,yyscanner);
#endif

  DebugLex debugLex(Debug::Lex_pre, __FILE__, qPrint(fileName));
  //printf("##########################\n%s\n####################\n",
  //    qPrint(input));

  state->macroExpansion = Config_getBool(MACRO_EXPANSION);
  state->expandOnlyPredef = Config_getBool(EXPAND_ONLY_PREDEF);
  state->skip=FALSE;
  state->curlyCount=0;
  state->lexRulesPart=false;
  state->nospaces=FALSE;
  state->inputBuf=&input;
  state->inputBufPos=0;
  state->outputBuf=&output;
  state->includeStack.clear();
  state->expandedDict.clear();
  state->contextDefines.clear();
  state->pragmaSet.clear();
  while (!state->condStack.empty()) state->condStack.pop();

  setFileName(yyscanner,fileName);

  state->inputFileDef = state->yyFileDef;
  //yyextra->defineManager.startContext(state->fileName);

  initPredefined(yyscanner,fileName);

  state->yyLineNr = 1;
  state->yyColNr  = 1;
  state->ifcount  = 0;

  BEGIN( Start );

  state->expectGuard = guessSection(fileName).isHeader();
  state->guardName.clear();
  state->lastGuardName.clear();
  state->guardExpr.clear();

  preYYlex(yyscanner);

  while (!state->condStack.empty())
  {
    const std::unique_ptr<preYY_CondCtx> &ctx = state->condStack.top();
    QCString sectionInfo = " ";
    if (ctx->sectionId!=" ") sectionInfo.sprintf(" with label '%s' ",qPrint(ctx->sectionId.stripWhiteSpace()));
    warn(ctx->fileName,ctx->lineNr,"Conditional section{}does not have "
        "a corresponding \\endcond command within this file.",sectionInfo);
    state->condStack.pop();
  }
  // make sure we don't extend a \cond with missing \endcond over multiple files (see bug 624829)
  forceEndCondSection(yyscanner);

  if (!state->levelGuard.empty())
  {
    warn(state->fileName,state->yyLineNr,"More #if's than #endif's found (might be in an included file).");
  }

  if (Debug::isFlagSet(Debug::Preprocessor))
  {
    std::lock_guard<std::mutex> lock(g_debugMutex);
    Debug::print(Debug::Preprocessor,0,"Preprocessor output of {} (size: {} bytes):\n",fileName,output.size());
    std::string contents;
    if (Debug::isFlagSet(Debug::NoLineNo))
    {
      contents=output;
    }
    else // need to add line numbers
    {
      int line=1;
      bool startOfLine = true;
      size_t content_size = output.size() +
                            output.size()*6/40; // assuming 40 chars per line on average
                                                // and 6 chars extra for the line number
      contents.reserve(content_size);
      size_t pos=0;
      while (pos<output.size())
      {
        if (startOfLine)
        {
          char lineNrStr[15];
          snprintf(lineNrStr,15,"%05d ",line++);
          contents+=lineNrStr;
        }
        contents   += output[pos];
        startOfLine = output[pos]=='\n';
        pos++;
      }
    }
    char end[2]={0,0};
    if (!contents.empty() && contents[contents.length()-1]!='\n')
    {
      end[0]='\n';
    }
    Debug::print(Debug::Preprocessor,0,"---------\n{}{}---------\n",contents,end);
    if (yyextra->contextDefines.size()>0)
    {
      Debug::print(Debug::Preprocessor,0,"Macros accessible in this file ({}):\n", fileName);
      Debug::print(Debug::Preprocessor,0,"---------\n");
      for (auto &kv : yyextra->contextDefines)
      {
        Debug::print(Debug::Preprocessor,0,"{} ",kv.second.name);
      }
      for (auto &kv : yyextra->localDefines)
      {
        Debug::print(Debug::Preprocessor,0,"{} ",kv.second.name);
      }
      Debug::print(Debug::Preprocessor,0,"\n---------\n");
    }
    else
    {
      Debug::print(Debug::Preprocessor,0,"No macros accessible in this file ({}).\n", fileName);
    }
  }

  {
    std::lock_guard<std::mutex> lock(g_updateGlobals);
    for (const auto &inc : state->includeRelations)
    {
      auto toKind = [](bool local,bool imported) -> IncludeKind
      {
        if (local)
        {
          if (imported)
          {
            return IncludeKind::ImportLocalObjC;
          }
          return IncludeKind::IncludeLocal;
        }
        else if (imported)
        {
          return IncludeKind::ImportSystemObjC;
        }
        return IncludeKind::IncludeSystem;
      };
      if (inc->fromFileDef)
      {
        inc->fromFileDef->addIncludeDependency(inc->toFileDef,inc->includeName,toKind(inc->local,inc->imported));
      }
      if (inc->toFileDef && inc->fromFileDef)
      {
        inc->toFileDef->addIncludedByDependency(inc->fromFileDef,inc->fromFileDef->docName(),toKind(inc->local,inc->imported));
      }
    }
    // add the macro definition for this file to the global map
    Doxygen::macroDefinitions.emplace(state->fileName.str(),std::move(state->macroDefinitions));
  }

  //yyextra->defineManager.endContext();
}

#include "pre.l.h"
