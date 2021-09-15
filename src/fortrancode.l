/******************************************************************************
 *
 * Parser for syntax highlighting and references for Fortran90 F subset
 *
 * Copyright (C) by Anke Visser
 * based on the work of Dimitri van Heesch.
 * Copyright (C) 2020 by Dimitri van Heesch.
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

/**
 @todo - continuation lines not always recognized
       - merging of use-statements with same module name and different only-names
       - rename part of use-statement
       - links to interface functions
       - references to variables
**/
%option never-interactive
%option case-insensitive
%option reentrant
%option prefix="fortrancodeYY"
%option extra-type="struct fortrancodeYY_state *"
%option noyy_top_state
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
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "defargs.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "tooltip.h"
#include "fortrancode.h"
#include "fortranscanner.h"
#include "containers.h"

const int fixedCommentAfter = 72;

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NO_TOP_STATE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

/*
 * For fixed formatted code position 6 is of importance (continuation character).
 * The following variables and macros keep track of the column number
 * YY_USER_ACTION is always called for each scan action
 * YY_FTN_RESET   is used to handle end of lines and reset the column counter
 * YY_FTN_REJECT  resets the column counters when a pattern is rejected and thus rescanned.
 */
int yy_old_start = 0;
int yy_my_start  = 0;
int yy_end       = 1;
#define YY_USER_ACTION {yy_old_start = yy_my_start; yy_my_start = yy_end; yy_end += static_cast<int>(yyleng);}
#define YY_FTN_RESET   {yy_old_start = 0; yy_my_start = 0; yy_end = 1;}
#define YY_FTN_REJECT  {yy_end = yy_my_start; yy_my_start = yy_old_start; REJECT;}

//--------------------------------------------------------------------------------

/**
  data of an use-statement
*/
class UseEntry
{
 public:
   QCString module; // just for debug
   std::vector<QCString> onlyNames;   /* entries of the ONLY-part */
};

/**
  module name -> list of ONLY/remote entries
  (module name = name of the module, which can be accessed via use-directive)
*/
class UseMap : public std::map<std::string,UseEntry>
{
};

/**
  Contains names of used modules and names of local variables.
*/
class Scope
{
  public:
    std::vector<QCString> useNames; //!< contains names of used modules
    StringUnorderedSet localVars; //!< contains names of local variables
    StringUnorderedSet externalVars; //!< contains names of external entities
};

/*===================================================================*/
/*
 *      statics
 */

struct fortrancodeYY_state
{
  QCString              docBlock;                   //!< contents of all lines of a documentation block
  QCString              currentModule=QCString();   //!< name of the current enclosing module
  UseMap                useMembers;                 //!< info about used modules
  UseEntry              useEntry;                   //!< current use statement info
  std::vector<Scope>    scopeStack;
  bool                  isExternal = false;
  QCString              str=QCString();             //!< contents of fortran string

  CodeOutputInterface * code = 0;

  const char *  inputString = 0;     //!< the code fragment as text
  yy_size_t     inputPosition = 0;   //!< read offset during parsing
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  int           contLineNr = 0;      //!< current, local, line number for continuation determination
  int          *hasContLine = 0;     //!< signals whether or not a line has a continuation line (fixed source form)
  bool          needsTermination = false;
  const Definition *searchCtx = 0;
  bool          collectXRefs = false;
  bool          isFixedForm = false;

  bool          insideBody = false;      //!< inside subprog/program body? => create links
  const char *  currentFontClass = 0;

  bool          exampleBlock = false;
  QCString      exampleName;
  QCString      exampleFile;

  const FileDef *    sourceFileDef = 0;
  const Definition * currentDefinition = 0;
  const MemberDef *  currentMemberDef = 0;
  bool          includeCodeFragment = false;

  char          stringStartSymbol = '\0'; // single or double quote
// count in variable declaration to filter out
//  declared from referenced names
  int           bracketCount = 0;

// signal when in type / class /procedure declaration
  int           inTypeDecl = 0;

  bool          endComment = false;
  TooltipManager tooltipManager;
};

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static bool getFortranNamespaceDefs(const QCString &mname, NamespaceDef *&cd);
static bool getFortranTypeDefs(const QCString &tname, const QCString &moduleName, ClassDef *&cd, const UseMap &useMap);

//----------------------------------------------------------------------------

static void endFontClass(yyscan_t yyscanner);
static void startFontClass(yyscan_t yyscanner,const char *s);
static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void addToSearchIndex(yyscan_t yyscanner,const QCString &text);
static void startCodeLine(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const QCString &text);
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol, Definition *d,const QCString &text);
static bool getGenericProcedureLink(yyscan_t yyscanner,const ClassDef *cd, const QCString &memberText, CodeOutputInterface &ol);
static bool getLink(yyscan_t yyscanner,const UseMap &useMap, // map with used modules
                    const QCString &memberText,  // exact member text
                    CodeOutputInterface &ol,
                    const QCString &text);
static void generateLink(yyscan_t yyscanner,CodeOutputInterface &ol, const QCString &lname);
static void generateLink(yyscan_t yyscanner,CodeOutputInterface &ol, const char *lname);
static int countLines(yyscan_t yyscanner);
static void startScope(yyscan_t yyscanner);
static void endScope(yyscan_t yyscanner);
static void addUse(yyscan_t yyscanner,const QCString &moduleName);
static void addLocalVar(yyscan_t yyscanner,const QCString &varName);
static MemberDef *getFortranDefs(yyscan_t yyscanner,const QCString &memberName, const QCString &moduleName, const UseMap &useMap);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);


//-------------------------------------------------------------------

static std::mutex g_docCrossReferenceMutex;
static std::mutex g_countFlowKeywordsMutex;

/* -----------------------------------------------------------------*/
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

%}

IDSYM     [a-z_A-Z0-9]
ID        [a-z_A-Z]+{IDSYM}*
B         [ \t]
BS        [ \t]*
BS_       [ \t]+
COMMA     {BS},{BS}
ARGS_L0   ("("[^)]*")")
ARGS_L1a  [^()]*"("[^)]*")"[^)]*
ARGS_L1   ("("{ARGS_L1a}*")")
ARGS_L2   "("({ARGS_L0}|[^()]|{ARGS_L1a}|{ARGS_L1})*")"
ARGS      {BS}({ARGS_L0}|{ARGS_L1}|{ARGS_L2})

/* The following appear in submodules as (subroutine|function|procedure) */
SUBPROG   (subroutine|function|procedure)

/* REAL and LOGICAL are treated separately as it can also be a function */
NUM_TYPE            (COMPLEX|INTEGER)
OPERATOR_LOGICAL    (\.and\.|\.eq\.|\.eqv\.|\.ge\.|\.gt\.|\.le\.|\.lt\.|\.ne\.|\.neqv\.|\.or\.|\.not\.)
OPERATOR_SYMBOLS    [\+\-\*/%::><=&]
KIND      {ARGS}

/* The pattern CHARACTER{ARGS}? is inconsistent with Fortran syntax and as such, removed from CHAR */
CHAR      (CHARACTER|CHARACTER{BS}"*"({BS}[0-9]+|{ARGS}))

/* The patterns ({NUM_TYPE}{KIND}) and ({NUM_TYPE}({BS}"*"{BS}[0-9]+)?) are inconsistent with Fortran syntax and as such, was removed from TYPE_SPEC */
TYPE_SPEC ({NUM_TYPE}|DOUBLE{BS}COMPLEX|DOUBLE{BS}PRECISION|{CHAR}|TYPE|CLASS|PROCEDURE|ENUMERATOR)

ACCESS_SPEC         (PROTECTED|PRIVATE|PUBLIC)
INTENT_SPEC         INTENT{BS}"("{BS}(IN|OUT|IN{BS}OUT){BS}")"

/* Fortran attributes except BIND and C which are treated separately */
ATTR_SPEC           (ALLOCATABLE|ASSIGNMENT|ASYNCHRONOUS|ABSTRACT|BIND|CODIMENSION|CONTIGUOUS|DEFERRED|DIMENSION|ELEMENTAL|EXTENDS|EXTERNAL|IMPURE|INTENT{BS}"("{BS}(IN|OUT|IN{BS}OUT){BS}")"|INTRINSIC|LOCAL|LOCAL_INIT|NON_INTRINSIC|NON_OVERRIDABLE|NON_RECURSIVE|NOPASS|OPERATOR|OPTIONAL|OVERRIDABLE|PARAMETER|PASS|POINTER|PRIVATE|PROTECTED|PUBLIC|PURE|RECURSIVE|SAVE|SEQUENCE|SHARED|TARGET|VALUE|VOLATILE)

/* attributes that take arguments */
ATTR_SPEC_WARG      (CODIMENSION|DIMENSION|EXTENDS|OPERATOR|PASS|BIND)

/* Assume that attribute statements are almost the same as attributes. */
ATTR_STMT           {ATTR_SPEC}

/* Intel directive statement and attributes. */
DIR_INTEL_STATEMENT (ALIAS|ASSUME|ASSUME_ALIGNED|ATTRIBUTES|DLLEXPORT|BLOCK_LOOP|DECLARE|DEFINE|DISTRIBUTE{BS_}POINT|FIXEDFORMLINESIZE|FMA|FORCEINLINE|FREEFORM|IDENT|IF|IF{BS_}DEFINED|INLINE|INTEGER|IVDEP|LOOP{BS_}COUNT|MESSAGE|NOBLOCK_LOOP|NODECLARE|NOFMA|NOFREEFORM|NOFUSION|NOINLINE|NOOPTIMIZE|NOPARALLEL|NOPREFETCH|NOSTRICT|NOUNROLL|NOUNROLL_AND_JAM|NOVECTOR|OBJCOMMENT|OPTIMIZE|OPTIONS|PACK|PARALLEL|PREFETCH|PSECT|REAL|SIMD|STRICT|UNDEFINE|UNROLL|UNROLL_AND_JAM|VECTOR|ALIGN|ALLOCATABLE|ALLOW_NULL|C|CODE_ALIGN|CONCURRENCY_SAFE|CVF|DECORATE|DEFAULT|DLLIMPORT|EXTERN|FASTMEM|IGNORE_LOC|INLINE|MIXED_STR_LEN_ARG|NO_ARG_CHECK|NOCLONE|OPTIMIZATION_PARAMETER|REFERENCE|STDCALL|VALUE|VARYING)

/* ("ATTRIBUTE"{BS}[,]?{BS} */
/* |ALLOCATABLE|C|DEFAULT|DLLEXPORT|DLLIMPORT|EXTERN|FASTMEM|INLINE|FORCEINLINE|IGNORE_LOC|MIXED_STR_LEN_ARG|NO_ARG_CHECK|NOCLONE|NOINLINE|OPTIMIZATION_PARAMETER|REFERENCE|STDCALL|VALUE|VARYING|VECTOR */

/* IEEE constants can be merged with NAMED_CONST in the future.
Currently, including IEEE objects increases the number of flex rules to beyond what GNU flex can handle.
The flex "flex: input rules are too complicated (>= 32000 NFA states)" error can be resolved by increasing
the definitions in flexdef.h of flex source code for:
    #define JAMSTATE -32766
    #define MAXIMUM_MNS 31999
    #define BAD_SUBSCRIPT -32767
before compiling th flex library. However, the default distributed versions of flex cannot handle this.
*/

NAMED_CONST_IEEE                (IEEE_ALL|IEEE_ALL|IEEE_AWAY|IEEE_DATATYPE|IEEE_DENORMAL|IEEE_DIVIDE|IEEE_DIVIDE_BY_ZERO|IEEE_DOWN|IEEE_HALTING|IEEE_INEXACT|IEEE_INEXACT_FLAG|IEEE_INF|IEEE_INVALID|IEEE_INVALID_FLAG|IEEE_NAN|IEEE_NEAREST|IEEE_NEGATIVE_DENORMAL|IEEE_NEGATIVE_INF|IEEE_NEGATIVE_NORMAL|IEEE_NEGATIVE_SUBNORMAL|IEEE_NEGATIVE_ZERO|IEEE_OTHER|IEEE_OTHER_VALUE|IEEE_OVERFLOW|IEEE_POSITIVE_DENORMAL|IEEE_POSITIVE_INF|IEEE_POSITIVE_NORMAL|IEEE_POSITIVE_SUBNORMAL|IEEE_POSITIVE_ZERO|IEEE_ROUNDING|IEEE_SIGNALING_NAN|IEEE_SQRT|IEEE_SUBNORMAL|IEEE_TO_ZERO|IEEE_UNDERFLOW|IEEE_UNDERFLOW_FLAG|IEEE_UP|IEEE_USUAL)

NAMED_CONST                     ({NAMED_CONST_IEEE}|C_ALERT|C_BACKSPACE|C_BOOL|C_CARRIAGE_RETURN|C_CHAR|C_DOUBLE|C_DOUBLE_COMPLEX|C_FLOAT|C_FLOAT_COMPLEX|C_FORM_FEED|C_HORIZONTAL_TAB|C_INT|C_INT16_T|C_INT32_T|C_INT64_T|C_INT8_T|C_INT_FAST16_T|C_INT_FAST32_T|C_INT_FAST64_T|C_INT_FAST8_T|C_INT_LEAST16_T|C_INT_LEAST32_T|C_INT_LEAST64_T|C_INT_LEAST8_T|C_INTMAX_T|C_INTPTR_T|C_LONG|C_LONG_DOUBLE|C_LONG_DOUBLE_COMPLEX|C_LONG_LONG|C_NEW_LINE|C_NULL_CHAR|C_NULL_FUNPTR|C_NULL_PTR|C_SHORT|C_SIGNED_CHAR|C_SIZE_T|C_VERTICAL_TAB|CHARACTER_KINDS|CHARACTER_STORAGE_SIZE|CURRENT_TEAM|ERROR_UNIT|FILE_STORAGE_SIZE|INITIAL_TEAM|INPUT_UNIT|INT16|INT32|INT64|INT8|INTEGER_KINDS|IOSTAT_END|IOSTAT_EOR|IOSTAT_INQUIRE_INTERNAL_UNIT|LOGICAL_KINDS|NUMERIC_STORAGE_SIZE|OUTPUT_UNIT|PARENT_TEAM|REAL128|REAL64|REAL32|REAL_KINDS|STAT_FAILED_IMAGE|STAT_LOCKED|STAT_LOCKED_OTHER_IMAGE|STAT_STOPPED_IMAGE|STAT_UNLOCKED|STAT_UNLOCKED_FAILED_IMAGE|\.FALSE\.|\.TRUE\.)

FLOW                            (DO|DO{BS}CONCURRENT|SELECT{BS}(CASE|TYPE|RANK)|CASE|IF|THEN|ELSE{BS}IF|ELSE|DO{BS}WHILE|WHERE|ELSEWHERE|FORALL|CRITICAL|BLOCK|ASSOCIATE)

FLOW_END                        (DO|SELECT|WHERE|IF|WHILE|FORALL|CRITICAL|BLOCK|ASSOCIATE)

PREFIX                          ((NON_)?RECURSIVE{BS_}|IMPURE{BS_}|PURE{BS_}|ELEMENTAL{BS_}){0,4}((NON_)?RECURSIVE|IMPURE|PURE|ELEMENTAL)?0
SUFFIX                          (RESULT{ARGS})
/* The specified syntax is too generic and does not respect NAME and C as a specifiers. Also "bind" as an attribute takes precedence over its statement format.
LANGUAGE_BIND_SPEC              BIND{BS}"("{BS}C{BS}(,{BS}NAME{BS}"="{BS}"\""(.*)"\""{BS})?")"
*/

/* INTRINSIC_FUNC_ELEMENTAL_IEEE can be merged with INTRINSIC_FUNC_ELEMENTAL when the flex bug gets resolved in the future */
INTRINSIC_FUNC_ELEMENTAL_IEEE   (IEEE_CLASS|IEEE_COPY_SIGN|IEEE_FMA|IEEE_GET_FLAG|IEEE_GET_HALTING_MODE|IEEE_INT|IEEE_IS_FINITE|IEEE_IS_NAN|IEEE_IS_NEGATIVE|IEEE_IS_NORMAL|IEEE_LOGB|IEEE_MAX_NUM|IEEE_MAX_NUM_MAG|IEEE_MIN_NUM|IEEE_MIN_NUM_MAG|IEEE_NEXT_AFTER|IEEE_NEXT_DOWN|IEEE_NEXT_UP|IEEE_QUIET_EQ|IEEE_QUIET_GE|IEEE_QUIET_GT|IEEE_QUIET_LE|IEEE_QUIET_LT|IEEE_QUIET_NAN|IEEE_QUIET_NE|IEEE_REAL|IEEE_REM|IEEE_RINT|IEEE_SCALB|IEEE_SIGNALING_EQ|IEEE_SIGNALING_GE|IEEE_SIGNALING_GT|IEEE_SIGNALING_LE|IEEE_SIGNALING_LT|IEEE_SIGNALING_NE|IEEE_SIGNBIT|IEEE_UNORDERED|IEEE_VALUE)

INTRINSIC_FUNC_ELEMENTAL        (ACHAR|ACOS|ACOSH|ADJUSTL|ADJUSTR|AIMAG|AINT|ANINT|ASIN|ASINH|ATAN|ATAN2|ATANH|BESSEL_J0|BESSEL_J1|BESSEL_JN|BESSEL_Y0|BESSEL_Y1|BESSEL_YN|BGE|BGT|BLE|BLT|BTEST|CEILING|CHAR|CMPLX|CONJG|COS|COSH|DBLE|DIM|DPROD|DSHIFTL|DSHIFTR|ERF|ERFC|ERFC_SCALED|EXP|EXPONENT|FLOOR|FRACTION|GAMMA|HYPOT|IACHAR|IAND|IBCLR|IBITS|IBSET|ICHAR|IEOR|IMAGE_STATUS|INT|IOR|ISHFT|ISHFTC|LEADZ|LEN_TRIM|LGE|LGT|LLE|LLT|LOG|LOG10|LOG_GAMMA|LOGICAL|MASKL|MASKR|MAX|MIN|MOD|MODULO|MVBITS|NEAREST|NINT|NOT|OUT_OF_RANGE|POPCNT|POPPAR|REAL|SCALE|SCAN|SET_EXPONENT|SHAPE|SHIFTA|SHIFTL|SHIFTR|SIGN|SIN|SINH|SPACING|SQRT|TAN|TANH|TRAILZ|TRIM|VERIFY)

/* INTRINSIC_FUNC_INQUIRY_IEEE can be merged with INTRINSIC_FUNC_INQUIRY when the flex bug gets resolved in the future */
INTRINSIC_FUNC_INQUIRY_IEEE     (IEEE_SUPPORT_DATATYPE|IEEE_SUPPORT_DENORMAL|IEEE_SUPPORT_DIVIDE|IEEE_SUPPORT_INF|IEEE_SUPPORT_IO|IEEE_SUPPORT_NAN|IEEE_SUPPORT_SQRT|IEEE_SUPPORT_STANDARD|IEEE_SUPPORT_SUBNORMAL|IEEE_SUPPORT_UNDERFLOW_CONTROL)

INTRINSIC_FUNC_INQUIRY          ({INTRINSIC_FUNC_INQUIRY_IEEE}|ALLOCATED|ASSOCIATED|BIT_SIZE|C_SIZEOF|COSHAPE|DIGITS|EPSILON|EXTENDS_TYPE_OF|FAILED_IMAGES|HUGE|IS_CONTIGUOUS|IS_IOSTAT_END|IS_IOSTAT_EOR|KIND|LBOUND|LCOBOUND|LEN|MAXEXPONENT|MINEXPONENT|NEW_LINE|PRECISION|PRESENT|RADIX|RANGE|RANK|SAME_TYPE_AS|SIZE|STORAGE_SIZE|TINY|UBOUND|UCOBOUND)

INTRINSIC_FUNC_TRANSFORMATIONAL_IEEE (IEEE_SELECTED_REAL_KIND|IEEE_SUPPORT_FLAG|IEEE_SUPPORT_HALTING|IEEE_SUPPORT_ROUNDING)

INTRINSIC_FUNC_TRANSFORMATIONAL ({INTRINSIC_FUNC_TRANSFORMATIONAL_IEEE}|ALL|ANY|BESSEL_JN|BESSEL_YN|C_ASSOCIATED|C_FUNLOC|C_LOC|COMMAND_ARGUMENT_COUNT|COMPILER_OPTIONS|COMPILER_VERSION|COUNT|CSHIFT|DOT_PRODUCT|EOSHIFT|FINDLOC|GET_TEAM|IALL|IANY|IMAGE_INDEX|INDEX|IPARITY|MATMUL|MAXLOC|MAXVAL|MERGE|MERGE_BITS|MINLOC|MINVAL|NORM2|NULL|NUM_IMAGES|PACK|PARITY|PRODUCT|REDUCE|REPEAT|RESHAPE|SELECTED_CHAR_KIND|SELECTED_INT_KIND|SELECTED_REAL_KIND|SPREAD|STOPPED_IMAGES|SUM|TEAM_NUMBER|THIS_IMAGE|TRANSPOSE|UNPACK)

INTRINSIC_FUNC_VOID             (CFI_address|CFI_allocate|CFI_deallocate|CFI_establish|CFI_is_contiguous|CFI_section|CFI_select_part|CFI_setpointer)

INTRINSIC_FUNC                  ({INTRINSIC_FUNC_ELEMENTAL}|{INTRINSIC_FUNC_INQUIRY}|{INTRINSIC_FUNC_TRANSFORMATIONAL})

/* intrinsic subroutines */

INTRINSIC_SUB_ATOMIC            (ATOMIC_ADD|ATOMIC_AND|ATOMIC_CAS|ATOMIC_DEFINE|ATOMIC_FETCH_ADD|ATOMIC_FETCH_AND|ATOMIC_FETCH_OR|ATOMIC_FETCH_XOR|ATOMIC_INT_KIND|ATOMIC_LOGICAL_KIND|ATOMIC_OR|ATOMIC_REF|ATOMIC_XOR)

INTRINSIC_SUB_COLLECTIVE        (CO_BROADCAST|CO_MAX|CO_MIN|CO_REDUCE|CO_SUM)

/* INTRINSIC_SUB_IMPURE_IEEE can be merged with INTRINSIC_SUB_IMPURE when the flex bug gets resolved in the future */
INTRINSIC_SUB_IMPURE_IEEE       (IEEE_GET_MODES|IEEE_GET_ROUNDING_MODE|IEEE_GET_STATUS|IEEE_GET_UNDERFLOW_MODE|IEEE_SET_MODES|IEEE_SET_ROUNDING_MODE|IEEE_SET_STATUS|IEEE_SET_UNDERFLOW_MODE)
INTRINSIC_SUB_IMPURE            ({INTRINSIC_SUB_IMPURE_IEEE}|C_F_POINTER|CPU_TIME|DATE_AND_TIME|EVENT_QUERY|EXECUTE_COMMAND_LINE|GET_COMMAND|GET_COMMAND_ARGUMENT|GET_ENVIRONMENT_VARIABLE|RANDOM_INIT|RANDOM_NUMBER|RANDOM_SEED|SYSTEM_CLOCK)

/* INTRINSIC_SUB_PURE_IEEE can be merged with INTRINSIC_SUB_PURE when the flex bug gets resolved in the future */
INTRINSIC_SUB_PURE_IEEE         (IEEE_SET_FLAG|IEEE_SET_HALTING_MODE)
INTRINSIC_SUB_PURE              ({INTRINSIC_SUB_PURE_IEEE}|C_F_PROCPOINTER|MOVE_ALLOC)

INTRINSIC_SUB                   ({INTRINSIC_SUB_ATOMIC}|{INTRINSIC_SUB_COLLECTIVE}|{INTRINSIC_SUB_IMPURE}|{INTRINSIC_SUB_PURE})

INTRINSIC_PROC                  ({INTRINSIC_SUB}|{INTRINSIC_FUNC})

/* INTRINSIC_MODULE_IEEE can be merged with INTRINSIC_MODULE when the flex bug gets resolved in the future */
INTRINSIC_MODULE_IEEE           (IEEE_ARITHMETIC|IEEE_EXCEPTIONS|IEEE_FEATURES)
INTRINSIC_MODULE                ({INTRINSIC_MODULE_IEEE}|ISO_C_BINDING|ISO_FORTRAN_ENV)

SPECIFIER                       (ACCESS|ACQUIRED_LOCK|ACTION|ADVANCE|ASYNCHRONOUS|BLANK|DECIMAL|DELIM|DIM|DIRECT|ENCODING|EOR|ERR|ERRMSG|EXIST|FILE|FMT|FORM|FORMATTED|ID|IOLENGTH|IOMSG|IOSTAT|KIND|LEN|MOLD|NAME|NAMED|NEW_INDEX|NEWUNIT|NEXTREC|NML|NUMBER|OPENED|PAD|PENDING|POS|POSITION|QUIET|READ|READWRITE|REC|RECL|ROUND|SEQUENTIAL|SIGN|SIZE|SOURCE|STAT|STATUS|STREAM|TEAM|TEAM_NUMBER|UNFORMATTED|UNIT|UNTIL_COUNT|WRITE)

STATEMENT                       (ALLOCATE|ASSIGN|BACKSPACE|BLOCK{BS}DATA|CALL|CHANGE{BS}TEAM|CLOSE|COMMON|CONTAINS|CONTINUE|CYCLE|DATA|DEALLOCATE|ENTRY|ERROR{BS}STOP|EQUIVALENCE|EVENT{BS_}POST|EVENT{BS_}WAIT|EXIT|FAIL{BS_}IMAGE|FINAL|FLUSH|FORM{BS_}TEAM|FORMAT|FORMATTED|FUNCTION|GENERIC|GO{BS}TO|INCLUDE|INQUIRE|LOCK|MODULE{BS_}PROCEDURE|NAMELIST|NULLIFY|OPEN|PAUSE|PRINT|PROGRAM|READ|RESULT|RETURN|REWIND|STOP|END{BS}SUBMODULE|SUBROUTINE|SYNC{BS_}(ALL|MEMORY|IMAGES|TEAM)|UNFORMATTED|UNLOCK|WAIT|WRITE)

/* INTRINSIC_DERIVED_TYPE_IEEE and INTRINSIC_DERIVED_TYPE_CFI can be merged with INTRINSIC_DERIVED_TYPE when the flex bug gets resolved in the future */
INTRINSIC_DERIVED_TYPE_IEEE     (IEEE_CLASS_TYPE|IEEE_FEATURES_TYPE|IEEE_FLAG_TYPE|IEEE_MODES_TYPE|IEEE_ROUND_TYPE|IEEE_STATUS_TYPE)
INTRINSIC_DERIVED_TYPE_CFI      (CFI_cdesc_t)
INTRINSIC_DERIVED_TYPE          (C_FUNPTR|C_PTR|EVENT_TYPE|LOCK_TYPE|TEAM_TYPE)

/* |  */

%option noyywrap
%option stack
%option caseless
/*%option debug*/

%x Start
%x SubCall
%x FuncDef
%x ClassName
%x ClassVar
%x Subprog
%x DocBlock
%x Use
%x UseOnly
%x Import
%x Declaration
%x DeclarationBinding
%x DeclContLine
%x Parameterlist
%x String
%x Subprogend

%%
 /*==================================================================*/

 /*-------- inner construct ---------------------------------------------------*/

<Start>{BS}"%"/{BS}[a-zA-Z]+.*                                  {   // highlight rule for the percent-symbol separator of Fortran standard.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_operator_symbols");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}[.]*{BS}("+"|"-"|"*"|"/"|"<"|">"|"="|"&"){BS}            {   // highlight rule for the operators of Fortran standard.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_operator_symbols");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}(0|[1-9]\d*)(\.\d*)?(e[-\+]?(0|[1-9]\d*))?(_([a-zA-Z])+)?    {   // highlight rule for the numeric values of Fortran.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_numeric_literal");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}{NAMED_CONST}{BS}                                        {   // highlight rule for the named constants of Fortran standard.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_named_const");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>{BS}"bind"/{BS}"("[.]*                       {   // highlight rule for the bind attribute of Fortran standard.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_attribute");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>{BS}"C"/{BS}(","{BS}"name"{BS}"="{BS}(.)+)("&"|")"){BS}  {   // highlight rule for the bind C attribute of Fortran standard.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_attribute");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Declaration>{BS}{ATTR_SPEC}/({BS},|{BS_}([a-zA-Z]|"::")){BS}   {   // highlight rule for the attributes of Fortran standard.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    if (QCString(yytext) == "external") yyextra->isExternal = true;
                                                                    startFontClass(yyscanner,"fortran_attribute");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Declaration>{BS}{ATTR_SPEC_WARG}/("("[.*]")")?({BS},|{BS_}([a-zA-Z]|"::")){BS}   {   // highlight rule for the attributes that take argument: DIMENSION(:,:).
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_attribute");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}"("{BS}{INTRINSIC_DERIVED_TYPE}{BS}")"{BS}               {   // highlight rule for Fortran intrinsic derived types.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_inrinsic_dtype");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}{INTRINSIC_DERIVED_TYPE}{BS}                             {   // highlight rule for Fortran intrinsic module names.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_inrinsic_dtype");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}{INTRINSIC_MODULE}{BS}                                   {   // highlight rule for Fortran intrinsic module names.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_inrinsic_module");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start,Declaration>{BS}[^%]{SPECIFIER}/{BS}={BS}                {   // highlight rule for Fortran intrinsic specifier (intrinsic procedure argument) names.
                                                                    // \todo: A specifier must be enclosed with parentheses. This is not currently enforced.
                                                                    startFontClass(yyscanner,"fortran_spcifier");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }

 /* Fortran intrinsic procedures */

<Start>^{BS}"real"/[,:( ].*                                     {   // real is a data type but also an elemental function, which makes it a bit tricky.
                                                                    yy_push_state(YY_START,yyscanner);
                                                                    BEGIN(Declaration);
                                                                    startFontClass(yyscanner,"fortran_statement");
                                                                    yyextra->code->codify(QCString(yytext));
                                                                    endFontClass(yyscanner);
                                                                }
<Start>^{BS}"logical"/[,:( ].*                                  {   // logical is a data type but also an elemental function, which makes it a bit tricky.
                                                                    yy_push_state(YY_START,yyscanner);
                                                                    BEGIN(Declaration);
                                                                    startFontClass(yyscanner,"fortran_statement");
                                                                    yyextra->code->codify(QCString(yytext));
                                                                    endFontClass(yyscanner);
                                                                }
<Start,Declaration>{BS}{INTRINSIC_FUNC}/{BS}[(]                 {   // highlight rule for Fortran intrinsic functions.
                                                                    // \todo This must be improved when function name appears in an intrinsic module USE statement.
                                                                    // \todo The current rule only captures function names when followed by "(".
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_function");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>{BS}{INTRINSIC_SUB}/{BS}[(]                              {   // highlight rule for Fortran intrinsic subroutines.
                                                                    // \todo This must be improved when subroutine name appears in an intrinsic module USE statement.
                                                                    // \todo The current rule only captures subroutine names when followed by "(".
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_subroutine");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>{BS}"MPI_"{BS}[a-zA-Z_]+/{BS}                            {   // highlight rule for MPI module intrinsic entities.
                                                                    // \todo The current search pattern is too generic. In the future, all MPI entities from
                                                                    // \todo the standard MPI module must be listed here and explicitly searched for.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_mpi");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }

 /* Fortran intrinsic statements */

<Start>{BS}{STATEMENT}/{BS}[;( \t\n]                            {   // highlight rule for Fortran intrinsic statements.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_statement");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>{FLOW}/{BS}[;( \t\n]                                     {   // highlight rule for Fortran flow-control statements.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    if (yyextra->isFixedForm)
                                                                    {
                                                                        if ((yy_my_start == 1) && ((yytext[0] == 'c') || (yytext[0] == 'C'))) YY_FTN_REJECT;
                                                                    }
                                                                    if (yyextra->currentMemberDef && yyextra->currentMemberDef->isFunction())
                                                                    {
                                                                        std::lock_guard<std::mutex> lock(g_countFlowKeywordsMutex);
                                                                        MemberDefMutable *mdm = toMemberDefMutable(yyextra->currentMemberDef);
                                                                        if (mdm)
                                                                        {
                                                                            mdm->incrementFlowKeyWordCount();
                                                                        }
                                                                    }
                                                                    /* font class is defined e.g. in doxygen.css */
                                                                    startFontClass(yyscanner,"fortran_keywordflow");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>(CASE|CLASS|TYPE|RANK){BS_}(IS|DEFAULT)                  {
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_keywordflow");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<*>{BS}"end"{BS}{FLOW_END}/[ ;\t\n]                             {   // list is a bit long as not all have possible end. <*> needed since the FLOW could be all in one line.
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_keywordflow");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
<Start>"implicit"{BS}("none"|{TYPE_SPEC})                       {
                                                                    if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                                    startFontClass(yyscanner,"fortran_statement");
                                                                    codifyLines(yyscanner,yytext);
                                                                    endFontClass(yyscanner);
                                                                }
 /*-------- use statement -------------------------------------------*/
<Start>"use"{BS}                        {
                                          if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Use);
                                        }
<Use>("intrinsic"|"non_intrinsic")      { // TODO: rename
                                          startFontClass(yyscanner,"fortran_attribute");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Use);
                                        }
<Use>"ONLY"                             { // TODO: rename
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(UseOnly);
                                        }
<Use>{ID}                               {
                                          QCString tmp(yytext);
                                          // tmp = tmp.lower();
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;

                                          /* append module name to use dict */
                                          yyextra->useEntry = UseEntry();
                                          yyextra->useEntry.module = tmp;
                                          yyextra->useMembers.insert(std::make_pair(tmp.str(), yyextra->useEntry));
                                          addUse(yyscanner,tmp);
                                        }
<Use,UseOnly,Import>{BS},{BS}           { codifyLines(yyscanner,yytext); }
<UseOnly,Import>{BS}&{BS}"\n"           { codifyLines(yyscanner,yytext);
                                          yyextra->contLineNr++;
                                          YY_FTN_RESET}
<UseOnly>{ID}                           {
                                          QCString tmp(yytext);
                                          // tmp = tmp.lower();
                                          yyextra->useEntry.onlyNames.push_back(tmp);
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;
                                        }
<Use,UseOnly,Import>"\n"                {
                                          unput(*yytext);
                                          yy_pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<*>"import"{BS}/"\n"                    |
<*>"import"{BS_}                        {
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Import);
                                        }
<Import>{ID}                            {
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;
                                        }
<Import>("ONLY"|"NONE"|"ALL")           {
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
 /*-------- fortran module  -----------------------------------------*/
<Start>("block"{BS}"data"|"program"|"module"|"interface")/{BS_}|({COMMA}{ACCESS_SPEC})|\n { //
                                            startScope(yyscanner);
                                            startFontClass(yyscanner,"fortran_statement");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(ClassName);
                                            if (!qstricmp(yytext,"module")) yyextra->currentModule="module";
                                        }
<Start>^{BS}"enum"/{BS}("&"|",")        {   // rule for enum statement
                                            startScope(yyscanner);
                                            startFontClass(yyscanner,"fortran_statement");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(ClassName);
                                        }
<Start>("type")/{BS}[:,]{BS}            {   // rule for type-definition construct. Do NOT put "(" in [:,] as it leads to confusion with variable declaration.
                                            startScope(yyscanner);
                                            startFontClass(yyscanner,"fortran_statement");
                                            codifyLines(yyscanner,yytext);
                                            endFontClass(yyscanner);
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(ClassName);
                                        }
<ClassName>{ID}                         {
                                            if (yyextra->currentModule == "module")
                                            {
                                                yyextra->currentModule=yytext;
                                                yyextra->currentModule = yyextra->currentModule;//.lower();
                                            }
                                            generateLink(yyscanner,*yyextra->code,yytext);
                                            yy_pop_state(yyscanner);
                                        }
<ClassName>{BS}({ATTR_SPEC})/{BS}[,:( ]{BS} { // variable declaration
                                            if (QCString(yytext) == "external") yyextra->isExternal = true;
                                            startFontClass(yyscanner,"fortran_attribute");
                                            yyextra->code->codify(QCString(yytext));
                                            endFontClass(yyscanner);
                                        }
<Start>^{BS}{ACCESS_SPEC}/{BS}[,: ]     {   // access attribute declaration. Highlighting must be "fortran_attribute" as attribute has precedence over statement.
                                            startFontClass(yyscanner,"fortran_attribute");
                                            yyextra->code->codify(QCString(yytext));
                                            endFontClass(yyscanner);
                                        }
<ClassName>\n                           {   // interface may be without name
                                            yy_pop_state(yyscanner);
                                            YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"enum").*        {
                                            YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"type").*        {
                                            YY_FTN_REJECT;
                                        }
<Start>^{BS}"end"({BS_}"module").*      {   // just reset yyextra->currentModule, rest is done in following rule
                                            yyextra->currentModule=0;
                                            YY_FTN_REJECT;
                                        }
 /*-------- subprog definition -------------------------------------*/
 /*
<Start>({PREFIX}{BS_})?{TYPE_SPEC}{BS_}({PREFIX}{BS_})?{BS}/{SUBPROG}{BS_}  {   // TYPE_SPEC is for old function style function result
                                          startFontClass(yyscanner,"fortran_prefix");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
 */
<Start>("module"{BS_})?{SUBPROG}{BS_}   {  // Fortran subroutine or function found
                                          startFontClass(yyscanner,"fortran_prefix");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Subprog);
                                        }
<Subprog>{ID}                           { // subroutine/function name
                                          DBG_CTX((stderr, "===> start subprogram %s\n", yytext));
                                          startScope(yyscanner);
                                          generateLink(yyscanner,*yyextra->code,yytext);
                                        }
<Subprog>"result"/{BS}"("[^)]*")"       {
                                          startFontClass(yyscanner,"fortran_suffix");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<Subprog>"("[^)]*")"                    { // ignore rest of line
                                          codifyLines(yyscanner,yytext);
                                        }
<Subprog,Subprogend>"\n"                { codifyLines(yyscanner,yytext);
                                          yyextra->contLineNr++;
                                          yy_pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<Start>"end"{BS}("block"{BS}"data"|{SUBPROG}|"module"|"program"|"enum"|"type"|"interface")?{BS}     {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endScope(yyscanner);
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Subprogend);
                                        }
<Subprogend>{ID}/{BS}(\n|!|;)           {
                                          generateLink(yyscanner,*yyextra->code,yytext);
                                          yy_pop_state(yyscanner);
                                        }
<Start>"end"{BS}("block"{BS}"data"|{SUBPROG}|"module"|"program"|"enum"|"type"|"interface"){BS}/(\n|!|;) {  // Fortran subroutine or function ends
                                          //cout << "===> end function " << yytext << endl;
                                          endScope(yyscanner);
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
 /*-------- variable declaration ----------------------------------*/
<Start>{TYPE_SPEC}/{BS}[,:( ]           {
                                          QCString typ(yytext);
                                          typ = removeRedundantWhiteSpace(typ.lower());
                                          if (typ.startsWith("real")) YY_FTN_REJECT;
                                          if (typ.startsWith("logical")) YY_FTN_REJECT;
                                          if (typ == "type" || typ == "class" || typ == "procedure") yyextra->inTypeDecl = 1;
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(Declaration);
                                          startFontClass(yyscanner,"fortran_statement");
                                          yyextra->code->codify(QCString(yytext));
                                          endFontClass(yyscanner);
                                        }
<Start>^{BS}{ATTR_SPEC}                 { // declare attribute statement. Highlighting must be "fortran_attribute" as attribute has precedence over statement.
                                          if (QCString(yytext) == "external")
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(Declaration);
                                            yyextra->isExternal = true;
                                          }
                                          startFontClass(yyscanner,"fortran_attribute");
                                          yyextra->code->codify(QCString(yytext));
                                          endFontClass(yyscanner);
                                        }
<Declaration>({TYPE_SPEC})/{BS}[(,: ]   { //| variable declaration
                                          if (QCString(yytext) == "external") yyextra->isExternal = true;
                                          startFontClass(yyscanner,"fortran_statement");
                                          yyextra->code->codify(QCString(yytext));
                                          endFontClass(yyscanner);
                                        }
<Declaration>{ID}                       { // local var
                                          if (yyextra->isFixedForm && yy_my_start == 1)
                                          {
                                            startFontClass(yyscanner,"fortran_comment");
                                            yyextra->code->codify(QCString(yytext));
                                            endFontClass(yyscanner);
                                          }
                                          else if (yyextra->currentMemberDef &&
                                                   ((yyextra->currentMemberDef->isFunction() && (yyextra->currentMemberDef->typeString()!=QCString("subroutine") || yyextra->inTypeDecl)) ||
                                                     yyextra->currentMemberDef->isVariable() || yyextra->currentMemberDef->isEnumValue()
                                                    )
                                                   )
                                          {
                                            generateLink(yyscanner,*yyextra->code, yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(QCString(yytext));
                                            addLocalVar(yyscanner,QCString(yytext));
                                          }
                                        }
<Declaration>{BS}("=>"|"="){BS}         { // Procedure binding
                                          BEGIN(DeclarationBinding);
                                          yyextra->code->codify(QCString(yytext));
                                        }
<DeclarationBinding>{ID}                { // Type bound procedure link
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yy_pop_state(yyscanner);
                                        }
<Declaration>[(]                        { // start of array or type / class specification
                                          yyextra->bracketCount++;
                                          yyextra->code->codify(QCString(yytext));
                                        }

<Declaration>[)]                        { // end array specification
                                          yyextra->bracketCount--;
                                          if (!yyextra->bracketCount) yyextra->inTypeDecl = 0;
                                          yyextra->code->codify(QCString(yytext));
                                        }

<Declaration,DeclarationBinding>"&"     { // continuation line
                                          yyextra->code->codify(QCString(yytext));
                                          if (!yyextra->isFixedForm)
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(DeclContLine);
                                          }
                                        }
<DeclContLine>"\n"                      { // declaration not yet finished
                                          yyextra->contLineNr++;
                                          codifyLines(yyscanner,yytext);
                                          yyextra->bracketCount = 0;
                                          yy_pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<Declaration,DeclarationBinding>"\n"    { // end declaration line (?)
                                          if (yyextra->endComment)
                                          {
                                            yyextra->endComment=FALSE;
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext);
                                          }
                                          yyextra->bracketCount = 0;
                                          yyextra->contLineNr++;
                                          if (!(yyextra->hasContLine && yyextra->hasContLine[yyextra->contLineNr - 1]))
                                          {
                                            yyextra->isExternal = false;
                                            yy_pop_state(yyscanner);
                                          }
                                          YY_FTN_RESET
                                        }

 /*-------- subprog calls  -----------------------------------------*/

<Start>"call"{BS_}                      {
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(SubCall);
                                        }
<SubCall>{ID}                           { // subroutine call
                                          yyextra->insideBody=TRUE;
                                          generateLink(yyscanner,*yyextra->code, yytext);
                                          yyextra->insideBody=FALSE;
                                          yy_pop_state(yyscanner);
                                        }
<Start>{ID}{BS}/"("                     { // function call
                                          if (yyextra->isFixedForm && yy_my_start == 6)
                                          {
                                            // fixed form continuation line
                                            YY_FTN_REJECT;
                                          }
                                          else if (QCString(yytext).stripWhiteSpace().lower() == "type")
                                          {
                                            yy_push_state(YY_START,yyscanner);
                                            BEGIN(Declaration);
                                            startFontClass(yyscanner,"fortran_statement");
                                            yyextra->code->codify(QCString(yytext).stripWhiteSpace());
                                            endFontClass(yyscanner);
                                            yyextra->code->codify(QCString(yytext + 4));
                                          }
                                          else
                                          {
                                            yyextra->insideBody=TRUE;
                                            generateLink(yyscanner,*yyextra->code,yytext);
                                            yyextra->insideBody=FALSE;
                                          }
                                        }

 /*-------- comments ---------------------------------------------------*/
<Start,Declaration,DeclarationBinding>\n?{BS}"!>"|"!<"                 { // start comment line or comment block
                                          if (yytext[0] == '\n')
                                          {
                                            yyextra->contLineNr++;
                                            yy_old_start = 0;
                                            yy_my_start = 1;
                                            yy_end = static_cast<int>(yyleng);
                                          }
                                          // Actually we should see if ! on position 6, can be continuation
                                          // but the chance is very unlikely, so no effort to solve it here
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(DocBlock);
                                          yyextra->docBlock=yytext;
                                        }
<Declaration,DeclarationBinding>{BS}"!<"                   { // start comment line or comment block
                                          yy_push_state(YY_START,yyscanner);
                                          BEGIN(DocBlock);
                                          yyextra->docBlock=yytext;
                                        }

<DocBlock>.*                            { // contents of current comment line
                                          yyextra->docBlock+=yytext;
                                        }
<DocBlock>"\n"{BS}("!>"|"!<"|"!!")      { // comment block (next line is also comment line)
                                          yyextra->contLineNr++;
                                          yy_old_start = 0;
                                          yy_my_start = 1;
                                          yy_end = static_cast<int>(yyleng);
                                          // Actually we should see if ! on position 6, can be continuation
                                          // but the chance is very unlikely, so no effort to solve it here
                                          yyextra->docBlock+=yytext;
                                        }
<DocBlock>"\n"                          { // comment block ends at the end of this line
                                          // remove special comment (default config)
                                          yyextra->contLineNr++;
                                          if (Config_getBool(STRIP_CODE_COMMENTS))
                                          {
                                            yyextra->yyLineNr+=((QCString)yyextra->docBlock).contains('\n');
                                            yyextra->yyLineNr+=1;
                                            nextCodeLine(yyscanner);
                                            yyextra->endComment=TRUE;
                                          }
                                          else // do not remove comment
                                          {
                                            startFontClass(yyscanner,"fortran_comment");
                                            codifyLines(yyscanner,yyextra->docBlock);
                                            endFontClass(yyscanner);
                                          }
                                          unput(*yytext);
                                          yyextra->contLineNr--;
                                          yy_pop_state(yyscanner);
                                          YY_FTN_RESET
                                        }
<Start>^{BS}[!C]("DEC"|"DIR")"$"{BS}"ATTRIBUTES".*/"::" {   // ATTRIBUTES directive is treated separately as its argument (procedure name) needs to be recognized by Doxygen).
                                                            startFontClass(yyscanner,"fortran_directive_intel");
                                                            codifyLines(yyscanner,yytext);
                                                            endFontClass(yyscanner);
                                                        }
<Start>^{BS}[!C]("DEC"|"DIR")"$".*                      {   // highlight rule for Intel Fortran Compiler directives (except ATTRIBUTES).
                                                            QCString directive(yytext);
                                                            directive = removeRedundantWhiteSpace(directive.lower());
                                                            if (directive.contains("attributes",true)) YY_FTN_REJECT;
                                                            startFontClass(yyscanner,"fortran_directive_intel");
                                                            codifyLines(yyscanner,yytext);
                                                            endFontClass(yyscanner);
                                                        }
<Start>^{BS}[\!c]"$OMP".*                               {   // OpenMP directives
                                                            yy_push_state(YY_START,yyscanner);
                                                            BEGIN(Declaration);
                                                            startFontClass(yyscanner,"fortran_directive_omp");
                                                            yyextra->code->codify(QCString(yytext));
                                                            endFontClass(yyscanner);
                                                        }
<*>"!"[^><\n].*|"!"$                                    {   // normal comment
                                                            if(YY_START == String) YY_FTN_REJECT; // ignore in strings
                                                            if (yyextra->isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                                            startFontClass(yyscanner,"fortran_comment");
                                                            codifyLines(yyscanner,yytext);
                                                            endFontClass(yyscanner);
                                                        }

<*>^[Cc*].*                                             {   // normal comment
                                                            if(! yyextra->isFixedForm) YY_FTN_REJECT;
                                                            startFontClass(yyscanner,"fortran_comment");
                                                            codifyLines(yyscanner,yytext);
                                                            endFontClass(yyscanner);
                                                        }

<*>"assignment"/{BS}"("{BS}"="{BS}")"   {
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }
<*>"operator"/{BS}"("[^)]*")"           {
                                          startFontClass(yyscanner,"fortran_statement");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                        }

 /*------ preprocessor  --------------------------------------------*/
<Start>"#".*\n                          {
                                          if (yyextra->isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          yyextra->contLineNr++;
                                          startFontClass(yyscanner,"preprocessor");
                                          codifyLines(yyscanner,yytext);
                                          endFontClass(yyscanner);
                                          YY_FTN_RESET
                                        }
 /*------ variable references?  -------------------------------------*/

<Start>"%"{BS}{ID}                      { // ignore references to elements
                                          yyextra->code->codify(QCString(yytext));
                                        }
<Start>{ID}                             {
                                            yyextra->insideBody=TRUE;
                                            generateLink(yyscanner,*yyextra->code, yytext);
                                            yyextra->insideBody=FALSE;
                                        }
 /*------ strings --------------------------------------------------*/
<String>\n                              { // string with \n inside
                                          yyextra->contLineNr++;
                                          yyextra->str+=yytext;
                                          startFontClass(yyscanner,"fortran_string");
                                          codifyLines(yyscanner,yyextra->str);
                                          endFontClass(yyscanner);
                                          yyextra->str = "";
                                          YY_FTN_RESET
                                        }
<String>\"|\'                           { // string ends with next quote without previous backspace
                                          if(yytext[0]!=yyextra->stringStartSymbol) YY_FTN_REJECT; // single vs double quote
                                          yyextra->str+=yytext;
                                          startFontClass(yyscanner,"fortran_string");
                                          codifyLines(yyscanner,yyextra->str);
                                          endFontClass(yyscanner);
                                          yy_pop_state(yyscanner);
                                        }
<String>.                               {yyextra->str+=yytext;}

<*>\"|\'                                { /* string starts */
                                          /* if(YY_START == StrIgnore) YY_FTN_REJECT; // ignore in simple comments */
                                          if (yyextra->isFixedForm && yy_my_start == 6) YY_FTN_REJECT;
                                          yy_push_state(YY_START,yyscanner);
                                          yyextra->stringStartSymbol=yytext[0]; // single or double quote
                                          BEGIN(String);
                                          yyextra->str=yytext;
                                        }
 /*-----------------------------------------------------------------------------*/

<*>\n                                   {
                                          if (yyextra->endComment)
                                          {
                                            yyextra->endComment=FALSE;
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext);
                                            // comment cannot extend over the end of a line so should always be terminated at the end of the line.
                                            if (yyextra->currentFontClass && !strcmp(yyextra->currentFontClass,"fortran_comment")) endFontClass(yyscanner);
                                          }
                                          yyextra->contLineNr++;
                                          YY_FTN_RESET
                                        }
<*>^{BS}"type"{BS}"="                   { yyextra->code->codify(QCString(yytext)); }

<*>[\x80-\xFF]*                         { // keep utf8 characters together...
                                          if (yyextra->isFixedForm && yy_my_start > fixedCommentAfter)
                                          {
                                            startFontClass(yyscanner,"fortran_comment");
                                            codifyLines(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(QCString(yytext));
                                          }
                                        }
<*>.                                    {
                                          if (yyextra->isFixedForm && yy_my_start > fixedCommentAfter)
                                          {
                                            //yy_push_state(YY_START,yyscanner);
                                            //BEGIN(DocBlock);
                                            //yyextra->docBlock=yytext;
                                            startFontClass(yyscanner,"fortran_comment");
                                            codifyLines(yyscanner,yytext);
                                          }
                                          else
                                          {
                                            yyextra->code->codify(QCString(yytext));
                                          }
                                        }
<*>{OPERATOR_LOGICAL}                   {   // Fortran logical comparison keywords
                                            startFontClass(yyscanner,"fortran_operator_logical");
                                            //codifyLines(yyscanner,yyextra->docBlock);
                                            yyextra->code->codify(QCString(yytext));
                                            endFontClass(yyscanner);
                                        }
<*>{OPERATOR_SYMBOLS}                   { // Fortran operator symbols
                                            startFontClass(yyscanner,"fortran_operator_symbols");
                                            //codifyLines(yyscanner,yyextra->docBlock);
                                            yyextra->code->codify(QCString(yytext));
                                            endFontClass(yyscanner);
                                        }
<*><<EOF>>                              {
                                            if (YY_START == DocBlock) {
                                                if  (!Config_getBool(STRIP_CODE_COMMENTS))
                                                {
                                                    startFontClass(yyscanner,"fortran_comment");
                                                    codifyLines(yyscanner,yyextra->docBlock);
                                                    endFontClass(yyscanner);
                                                }
                                            }
                                          yyterminate();
                                        }
%%

/*@ ----------------------------------------------------------------------------
 */

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  yy_size_t c=0;
  while( c < max_size && *s)
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}

static void endFontClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=0;
  }
}

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  // if font class is already set don't stop and start it.
  // strcmp does not like null pointers as input.
  if (!yyextra->currentFontClass || !s || strcmp(yyextra->currentFontClass,s))
  {
    endFontClass(yyscanner);
    yyextra->code->startFontClass(QCString(s));
    yyextra->currentFontClass=s;
  }
}

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    if (yyextra->searchCtx)
    {
      yyextra->code->setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),FALSE);
    }
    else
    {
      yyextra->code->setCurrentDoc(yyextra->sourceFileDef,anchor,TRUE);
    }
  }
}

static void addToSearchIndex(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    yyextra->code->addWord(text,FALSE);
  }
}

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",yyextra->yyLineNr);
    //lineAnchor.sprintf("l%05d",yyextra->yyLineNr);

    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    //printf("startCodeLine %d d=%s\n", yyextra->yyLineNr,d ? qPrint(d->name()) : "<null>");
    if (!yyextra->includeCodeFragment && d)
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->insideBody = FALSE;
      yyextra->endComment = FALSE;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        yyextra->code->writeLineNumber( d->getReference(),
                                        d->getOutputFileBase(),
                                        QCString(),yyextra->yyLineNr
                                        );
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(QCString(),QCString(),QCString(),yyextra->yyLineNr);
    }
  }
  yyextra->code->startCodeLine(yyextra->sourceFileDef);
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(QCString(yyextra->currentFontClass));
  }
}


static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
}

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char * fc = yyextra->currentFontClass;
  endCodeLine(yyscanner);
  if (yyextra->yyLineNr<yyextra->inputLines)
  {
    yyextra->currentFontClass = fc;
    startCodeLine(yyscanner);
  }
}

/*! write a code fragment 'text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(yyscan_t yyscanner,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  if (text.isEmpty()) return;
  const char *p=text.data(),*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      int l = (int)(p-sp-1);
      char *tmp = (char*)malloc(l+1);
      memcpy(tmp,sp,l);
      tmp[l]='\0';
      yyextra->code->codify(QCString(tmp));
      free(tmp);
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(QCString(sp));
      done=TRUE;
    }
  }
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                  Definition *d,const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  static bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  yyextra->tooltipManager.addTooltip(ol,d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip;
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
    tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=FALSE;
  const char *p=text.data();
  while (!done)
  {
    const char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,QCString(sp,p-sp-1),tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}
//-------------------------------------------------------------------------------
/**
  searches for definition of a module (Namespace)
  @param mname the name of the module
  @param cd the entry, if found or null
  @returns true, if module is found
*/
static bool getFortranNamespaceDefs(const QCString &mname,
                               NamespaceDef *&cd)
{
  if (mname.isEmpty()) return FALSE; /* empty name => nothing to link */

  // search for module
  if ((cd=Doxygen::namespaceLinkedMap->find(mname))) return TRUE;

  return FALSE;
}
//-------------------------------------------------------------------------------
/**
  searches for definition of a type
  @param tname the name of the type
  @param moduleName name of enclosing module or null, if global entry
  @param cd the entry, if found or null
  @param useMap map of data of USE-statement
  @returns true, if type is found
*/
static bool getFortranTypeDefs(const QCString &tname, const QCString &moduleName,
                               ClassDef *&cd, const UseMap &useMap)
{
  if (tname.isEmpty()) return FALSE; /* empty name => nothing to link */

  //cout << "=== search for type: " << tname << endl;

  // search for type
  if ((cd=Doxygen::classLinkedMap->find(tname)))
  {
    //cout << "=== type found in global module" << endl;
    return TRUE;
  }
  else if (!moduleName.isEmpty() && (cd= Doxygen::classLinkedMap->find(moduleName+"::"+tname)))
  {
    //cout << "=== type found in local module" << endl;
    return TRUE;
  }
  else
  {
    for (const auto &kv : useMap)
    {
      if ((cd= Doxygen::classLinkedMap->find(kv.second.module+"::"+tname)))
      {
        //cout << "===  type found in used module" << endl;
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  searches for definition of function memberName
  @param yyscanner the scanner data to be used
  @param memberName the name of the function/variable
  @param moduleName name of enclosing module or null, if global entry
  @param useMap map of data of USE-statement
  @returns MemberDef pointer, if found, or nullptr otherwise
*/
static MemberDef *getFortranDefs(yyscan_t yyscanner,const QCString &memberName, const QCString &moduleName,
                                 const UseMap &useMap)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (memberName.isEmpty()) return nullptr; /* empty name => nothing to link */

  // look in local variables
  for (auto it = yyextra->scopeStack.rbegin(); it!=yyextra->scopeStack.rend(); ++it)
  {
    const Scope &scope = *it;
    //std::string lowMemName = memberName.lower().str();
    std::string lowMemName = memberName.str();
    if (scope.localVars   .find(lowMemName)!=std::end(scope.localVars)     &&  // local var
        scope.externalVars.find(lowMemName)==std::end(scope.externalVars))     // and not external
    {
      return nullptr;
    }
  }

  // search for function
  MemberName *mn = Doxygen::functionNameLinkedMap->find(memberName);
  if (!mn)
  {
    mn = Doxygen::memberNameLinkedMap->find(memberName);
  }

  if (mn) // name is known
  {
    // all found functions with given name
    for (const auto &md : *mn)
    {
      const FileDef  *fd=md->getFileDef();
      const GroupDef *gd=md->getGroupDef();
      const ClassDef *cd=md->getClassDef();

      //cout << "found link with same name: " << fd->fileName() << "  " <<  memberName;
      //if (md->getNamespaceDef() != 0) cout << " in namespace " << md->getNamespaceDef()->name();cout << endl;

      if ((gd && gd->isLinkable()) || (fd && fd->isLinkable()))
      {
        const NamespaceDef *nspace= md->getNamespaceDef();

        if (nspace == 0)
        { // found function in global scope
          if(cd == 0)
          { // Skip if bound to type
            return md.get();
          }
        }
        else if (moduleName == nspace->name())
        { // found in local scope
          return md.get();
        }
        else
        { // else search in used modules
          QCString usedModuleName= nspace->name();
          auto use_it = useMap.find(usedModuleName.str());
          if (use_it!=useMap.end())
          {
            const UseEntry &ue = use_it->second;
            // check if only-list exists and if current entry exists is this list
            if (ue.onlyNames.empty())
            {
              //cout << " found in module " << usedModuleName << " entry " << memberName <<  endl;
              return md.get(); // whole module used
            }
            else
            {
              for ( const auto &name : ue.onlyNames)
              {
                //cout << " search in only: " << usedModuleName << ":: " << memberName << "==" << (*it)<<  endl;
                if (memberName == name)
                {
                  return md.get(); // found in ONLY-part of use list
                }
              }
            }
          }
        }
      } // if linkable
    } // for
  }
  return nullptr;
}

/**
 gets the link to a generic procedure which depends not on the name, but on the parameter list
 @todo implementation
*/
static bool getGenericProcedureLink(yyscan_t yyscanner,const ClassDef *cd,
                                    const QCString &memberText,
                                    CodeOutputInterface &ol)
{
  (void)cd;
  (void)memberText;
  (void)ol;
  return FALSE;
}

static bool getLink(yyscan_t yyscanner,const UseMap &useMap, // dictionary with used modules
                    const QCString &memberText,  // exact member text
                    CodeOutputInterface &ol,
                    const QCString &text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  MemberDef *md=0;
  QCString memberName= removeRedundantWhiteSpace(memberText);

  if ((md=getFortranDefs(yyscanner,memberName, yyextra->currentModule, useMap)) && md->isLinkable())
  {
    if (md->isVariable() && (md->getLanguage()!=SrcLangExt_Fortran)) return FALSE; // Non Fortran variables aren't handled yet,
                                                                                   // see also linkifyText in util.cpp

    const Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                          md->getBodyDef() : md->getOuterScope();
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      if (yyextra->currentDefinition && yyextra->currentMemberDef &&
          yyextra->insideBody && yyextra->collectXRefs)
      {
        std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
        addDocCrossReference(toMemberDefMutable(yyextra->currentMemberDef),toMemberDefMutable(md));
      }
      writeMultiLineCodeLink(yyscanner,ol,md,!text.isEmpty() ? text : memberText);
      addToSearchIndex(yyscanner, !text.isEmpty() ? text : memberText);
      return TRUE;
    }
  }
  return FALSE;
}


static void generateLink(yyscan_t yyscanner,CodeOutputInterface &ol, const QCString &lname)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  ClassDef *cd=0;
  NamespaceDef *nsd=0;
  QCString name = lname;
  // name = removeRedundantWhiteSpace(name.lower());
  name = removeRedundantWhiteSpace(name);

  // check if lowercase lname is a linkable type or interface
  if ( (getFortranTypeDefs(name, yyextra->currentModule, cd, yyextra->useMembers)) && cd->isLinkable() )
  {
    if ( (cd->compoundType() == ClassDef::Class) && // was  Entry::INTERFACE_SEC) &&
         (getGenericProcedureLink(yyscanner, cd, name, ol)) )
    {
      //cout << "=== generic procedure resolved" << endl;
    }
    else
    { // write type or interface link
      writeMultiLineCodeLink(yyscanner, ol,cd,name);
      addToSearchIndex(yyscanner, name);
    }
  }
  // check for module
  else if ( (getFortranNamespaceDefs(name, nsd)) && nsd->isLinkable() )
  { // write module link
    writeMultiLineCodeLink(yyscanner,ol,nsd,name);
    addToSearchIndex(yyscanner,name);
  }
  // check for function/variable
  else if (getLink(yyscanner,yyextra->useMembers, name, ol, name))
  {
    //cout << "=== found link for lowercase " << lname << endl;
  }
  else
  {
    // nothing found, just write out the word
    //startFontClass("charliteral"); //test
    codifyLines(yyscanner,name);
    //endFontClass(yyscanner); //test
    addToSearchIndex(yyscanner,name);
  }
}

static void generateLink(yyscan_t yyscanner,CodeOutputInterface &ol, const char *lname)
{
  generateLink(yyscanner,ol,QCString(lname));
}

/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c;
  int count=1;
  while ((c=*p))
  {
    p++ ;
    if (c=='\n') count++;
  }
  if (p>yyextra->inputString && *(p-1)!='\n')
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++,
    yyextra->needsTermination=TRUE;
  }
  return count;
}

//----------------------------------------------------------------------------
/** start scope */
static void startScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr, "===> startScope %s",yytext));
  yyextra->scopeStack.push_back(Scope());
}

/** end scope */
static void endScope(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  DBG_CTX((stderr,"===> endScope %s",yytext));
  if (yyextra->scopeStack.empty())
  {
    DBG_CTX((stderr,"WARNING: fortrancode.l: stack empty!\n"));
    return;
  }

  Scope &scope = yyextra->scopeStack.back();
  for ( const auto &name : scope.useNames)
  {
    yyextra->useMembers.erase(name.str());
  }
  yyextra->scopeStack.pop_back();
}

static void addUse(yyscan_t yyscanner,const QCString &moduleName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->scopeStack.empty())
    yyextra->scopeStack.back().useNames.push_back(moduleName);
}

static void addLocalVar(yyscan_t yyscanner,const QCString &varName)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (!yyextra->scopeStack.empty())
  {
    // std::string lowVarName = varName.lower().str();
    std::string lowVarName = varName.str();
    yyextra->scopeStack.back().localVars.insert(lowVarName);
    if (yyextra->isExternal) yyextra->scopeStack.back().externalVars.insert(lowVarName);
  }
}

/*===================================================================*/


static void checkContLines(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int numLines = 0;
  int i = 0;
  const char *p = s;

  numLines = 2; // one for element 0, one in case no \n at end
  while (*p)
  {
    if (*p == '\n') numLines++;
    p++;
  }

  yyextra->hasContLine = (int *) malloc((numLines) * sizeof(int));
  for (i = 0; i < numLines; i++)
    yyextra->hasContLine[i] = 0;
  p = prepassFixedForm(s, yyextra->hasContLine);
  yyextra->hasContLine[0] = 0;
}

void parseFortranCode(CodeOutputInterface &od,const char *,const QCString &s,
                  bool exBlock, const char *exName,const FileDef *fd,
                  int startLine,int endLine,bool inlineFragment,
                  const MemberDef *,bool,const Definition *searchCtx,
                  bool collectXRefs, FortranFormat format)
{
  //printf("***parseCode() exBlock=%d exName=%s fd=%p\n",exBlock,exName,fd);

  return;
}

//---------------------------------------------------------

struct FortranCodeParser::Private
{
  yyscan_t yyscanner;
  fortrancodeYY_state state;
  FortranFormat format;
};

FortranCodeParser::FortranCodeParser(FortranFormat format) : p(std::make_unique<Private>())
{
  p->format = format;
  fortrancodeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  fortrancodeYYset_debug(1,p->yyscanner);
#endif
  resetCodeParserState();
}

FortranCodeParser::~FortranCodeParser()
{
  fortrancodeYYlex_destroy(p->yyscanner);
}

void FortranCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = FALSE;
  BEGIN( Start );
}

void FortranCodeParser::parseCode(CodeOutputInterface & codeOutIntf,
                   const QCString & scopeName,
                   const QCString & input,
                   SrcLangExt /*lang*/,
                   bool isExampleBlock,
                   const QCString & exampleName,
                   const FileDef * fileDef,
                   int startLine,
                   int endLine,
                   bool inlineFragment,
                   const MemberDef *memberDef,
                   bool showLineNumbers,
                   const Definition *searchCtx,
                   bool collectXRefs
                  )
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  //::parseFortranCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
  //                   fileDef,startLine,endLine,inlineFragment,memberDef,
  //                   showLineNumbers,searchCtx,collectXRefs,m_format);
  //  parseFortranCode(CodeOutputInterface &od,const char *,const QCString &s,
  //                bool exBlock, const char *exName,FileDef *fd,
  //                int startLine,int endLine,bool inlineFragment,
  //                const MemberDef *,bool,const Definition *searchCtx,
  //                bool collectXRefs, FortranFormat format)
  if (input.isEmpty()) return;
  printlex(yy_flex_debug, TRUE, __FILE__, fileDef ? qPrint(fileDef->fileName()): NULL);
  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
  yyextra->isFixedForm = recognizeFixedForm(input,p->format);
  yyextra->contLineNr = 1;
  yyextra->hasContLine = NULL;
  if (yyextra->isFixedForm)
  {
    checkContLines(yyscanner,yyextra->inputString);
  }
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = FALSE;
  yyextra->searchCtx = searchCtx;
  yyextra->collectXRefs = collectXRefs;
  if (startLine!=-1)
    yyextra->yyLineNr    = startLine;
  else
    yyextra->yyLineNr    = 1;

  if (endLine!=-1)
    yyextra->inputLines  = endLine+1;
  else
    yyextra->inputLines  = yyextra->yyLineNr + countLines(yyscanner) - 1;

  yyextra->exampleBlock  = isExampleBlock;
  yyextra->exampleName   = exampleName;
  yyextra->sourceFileDef = fileDef;
  if (isExampleBlock && fileDef==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef(QCString(),exampleName);
  }
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,QCString("l00001"));
  }
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
  if (!yyextra->exampleName.isEmpty())
  {
    yyextra->exampleFile = convertNameToFile(yyextra->exampleName+"-example");
  }
  yyextra->includeCodeFragment = inlineFragment;
  startCodeLine(yyscanner);
  fortrancodeYYrestart(0, yyscanner);
  BEGIN( Start );
  fortrancodeYYlex(yyscanner);
  if (yyextra->needsTermination)
  {
    endFontClass(yyscanner);
    yyextra->code->endCodeLine();
  }
  if (isExampleBlock && yyextra->sourceFileDef)
  {
    // delete the temporary file definition used for this example
    delete yyextra->sourceFileDef;
    yyextra->sourceFileDef=0;
  }
  if (yyextra->hasContLine) free(yyextra->hasContLine);
  yyextra->hasContLine = NULL;

  // write the tooltips
  yyextra->tooltipManager.writeTooltips(codeOutIntf);

  printlex(yy_flex_debug, FALSE, __FILE__, fileDef ? qPrint(fileDef->fileName()): NULL);
}

//---------------------------------------------------------

#if USE_STATE2STRING
#include "fortrancode.l.h"
#endif
