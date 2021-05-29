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
%option prefix="sqlcodeYY"
%option noyywrap
%option nounput
%option reentrant
%option extra-type="struct sqlcodeYY_state *"
%top{
#include <stdint.h>
}

%{

#include <stdio.h>

#include "sqlcode.h"

#include "entry.h"
#include "doxygen.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "config.h"
#include "filedef.h"
#include "tooltip.h"
#include "message.h"

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

struct sqlcodeYY_state
{
     CodeOutputInterface * code;
     const char   *inputString;     //!< the code fragment as text
     yy_size_t     inputPosition;   //!< read offset during parsing
     QCString      fileName;
     int           inputLines;      //!< number of line in the code fragment
     int           yyLineNr;        //!< current line number
     bool          needsTermination;
     const Definition   *searchCtx;

     bool          exampleBlock;
     QCString      exampleName;
     QCString      classScope;

     const FileDef    *sourceFileDef;
     const Definition *currentDefinition;
     const MemberDef  *currentMemberDef;
     bool          includeCodeFragment;
     const char   *currentFontClass;
};

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static void startCodeLine(yyscan_t yyscanner);
static void endFontClass(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void codifyLines(yyscan_t yyscanner,const char *text);
static void startFontClass(yyscan_t yyscanner,const char *s);
static int countLines(yyscan_t yyscanner);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#include "doxygen_lex.h"

%}

nl              (\r\n|\r|\n)
ws              [ \t]+
idchar          [A-Za-z0-9\-_]+
keywords1       ("ADD"|"ALL"|"ALLOCATE"|"ALTER"|"AND"|"ANY"|"ARE"|"AS"|"ASENSITIVE"|"ASYMMETRIC"|"AT"|"ATOMIC"|"AUTHORIZATION"|"BETWEEN"|"BOTH"|"BY"|"CALL"|"CALLED"|"CASCADED"|"CAST")
keywords2       ("CHECK"|"CLOSE"|"COLLATE"|"COLUMN"|"COMMIT"|"CONNECT"|"CONSTRAINT"|"CONTINUE"|"CORRESPONDING"|"CREATE"|"CROSS"|"CUBE"|"CURRENT"|"CURRENT_DATE"|"CURRENT_DEFAULT_TRANSFORM_GROUP")
keywords3       ("CURRENT_PATH"|"CURRENT_ROLE"|"CURRENT_TIME"|"CURRENT_TIMESTAMP"|"CURRENT_TRANSFORM_GROUP_FOR_TYPE"|"CURRENT_USER")
keywords4       ("CURSOR"|"CYCLE"|"DAY"|"DEALLOCATE"|"DECLARE"|"DEFAULT"|"DELETE"|"DEREF"|"DESCRIBE"|"DETERMINISTIC"|"DISCONNECT"|"DISTINCT"|"DROP"|"DYNAMIC")
keywords5       ("EACH"|"ELEMENT"|"END-EXEC"|"ESCAPE"|"EXCEPT"|"EXEC"|"EXECUTE"|"EXISTS"|"EXTERNAL"|"FETCH"|"FILTER"|"FOR"|"FOREIGN"|"FREE"|"FROM"|"FULL"|"FUNCTION")
keywords6       ("GET"|"GLOBAL"|"GRANT"|"GROUP"|"GROUPING"|"HAVING"|"HOLD"|"HOUR"|"IDENTITY"|"IMMEDIATE"|"IN"|"INDICATOR"|"INNER"|"INOUT"|"INPUT"|"INSENSITIVE"|"INSERT"|"INTERSECT")
keywords7       ("INTERVAL"|"INTO"|"IS"|"ISOLATION"|"JOIN"|"LANGUAGE"|"LARGE"|"LATERAL"|"LEADING"|"LEFT"|"LIKE"|"LOCAL"|"LOCALTIME"|"LOCALTIMESTAMP"|"MATCH"|"MEMBER"|"MERGE"|"METHOD"|"MINUTE")
keywords8       ("MODIFIES"|"MODULE"|"MONTH"|"MULTISET"|"NATIONAL"|"NATURAL"|"NEW"|"NO"|"NONE"|"NOT"|"OF"|"OLD"|"ON"|"ONLY"|"OPEN"|"OR"|"ORDER"|"OUT"|"OUTER"|"OUTPUT")
keywords9       ("OVER"|"OVERLAPS"|"PARAMETER"|"PARTITION"|"PRECISION"|"PREPARE"|"PRIMARY"|"PROCEDURE"|"RANGE"|"READS"|"RECURSIVE"|"REF"|"REFERENCES"|"REFERENCING"|"REGR_AVGX"|"REGR_AVGY")
keywords10      ("REGR_COUNT"|"REGR_INTERCEPT"|"REGR_R2"|"REGR_SLOPE"|"REGR_SXX"|"REGR_SXY"|"REGR_SYY"|"RELEASE"|"RESULT"|"RETURN"|"RETURNS"|"REVOKE"|"RIGHT"|"ROLLBACK"|"ROLLUP"|"ROW"|"ROWS"|"SAVEPOINT")
keywords11      ("SCROLL"|"SEARCH"|"SECOND"|"SELECT"|"SENSITIVE"|"SESSION_USER"|"SET"|"SIMILAR"|"SOME"|"SPECIFIC"|"SPECIFICTYPE"|"SQL"|"SQLEXCEPTION"|"SQLSTATE"|"SQLWARNING"|"START"|"STATIC")
keywords12      ("SUBMULTISET"|"SYMMETRIC"|"SYSTEM"|"SYSTEM_USER"|"TABLE"|"THEN"|"TIMEZONE_HOUR"|"TIMEZONE_MINUTE"|"TO"|"TRAILING"|"TRANSLATION"|"TREAT"|"TRIGGER"|"UESCAPE"|"UNION")
keywords13      ("UNIQUE"|"UNNEST"|"UPDATE"|"UPPER"|"USER"|"USING"|"VALUE"|"VALUES"|"VAR_POP"|"VAR_SAMP"|"VARYING"|"WHEN"|"WHENEVER"|"WHERE"|"WIDTH_BUCKET"|"WINDOW"|"WITH"|"WITHIN"|"WITHOUT"|"YEAR")

/* Need multiple keyword definitions due to max length */
keyword         (?i:{keywords1}|{keywords2}|{keywords3}|{keywords4}|{keywords5}|{keywords6}|{keywords7}|{keywords8}|{keywords9}|{keywords10}|{keywords11}|{keywords12}|{keywords13})

typekeyword     (?i:"ARRAY"|"BIGINT"|"BINARY"|"BLOB"|"BOOLEAN"|"CHAR"|"CHARACTER"|"CLOB"|"DATE"|"DEC"|"DECIMAL"|"DOUBLE"|"FLOAT"|"INT"|"INTEGER"|"NCHAR"|"NCLOB"|"NUMERIC"|"NVARCHAR"|"REAL"|"SMALLINT"|"TIME"|"TIMESTAMP"|"VARCHAR")

flowkeyword     (?i:"CASE"|"IF"|"ELSE"|"BEGIN"|"END"|"WHILE")

literalkeyword  (?i:"false"|"true"|"NULL"|"UNKNOWN")
stringliteral   (\"[^"]*\")|('[^']*')
number          [0-9]+
literals        ({literalkeyword}|{stringliteral}|{number})

variable        @{idchar}+

simplecomment   --.*
commentopen     "/\*"
commentclose    "\*\/"

%x COMMENT

%%

{literals}          {
                        startFontClass(yyscanner,"stringliteral");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }


{keyword}           {
                        startFontClass(yyscanner,"keyword");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }

{flowkeyword}       {
                        startFontClass(yyscanner,"keywordflow");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }

{typekeyword}       {
                        startFontClass(yyscanner,"keywordtype");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }

{variable}          {
                        startFontClass(yyscanner,"preprocessor");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }

{simplecomment}     {
                        startFontClass(yyscanner,"comment");
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                    }

{commentopen}       {
                        startFontClass(yyscanner,"comment");
                        codifyLines(yyscanner,yytext);
                        BEGIN(COMMENT);
                    }

<COMMENT>.          {
                        codifyLines(yyscanner,yytext);

                    }
<COMMENT>{nl}       {
                        codifyLines(yyscanner,yytext);
                    }

<COMMENT>{commentclose} {
                        codifyLines(yyscanner,yytext);
                        endFontClass(yyscanner);
                        BEGIN(INITIAL);
                    }

{idchar}            {
                        codifyLines(yyscanner,yytext);
                    }

{nl}                {
                        codifyLines(yyscanner,yytext);
                    }

[\x80-\xFF]*        { // keep utf8 characters together...
                        codifyLines(yyscanner,yytext);
                    }
.                   {
                        codifyLines(yyscanner,yytext);
                    }

%%


static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    if (yyextra->searchCtx)
    {
      yyextra->code->setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),false);
    }
    else
    {
      yyextra->code->setCurrentDoc(yyextra->sourceFileDef,anchor,true);
    }
  }
}

/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is true. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->sourceFileDef)
  {
    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);

    if (!yyextra->includeCodeFragment && d && d->isLinkableInProject())
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->classScope = d->name();
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                            yyextra->currentMemberDef->getOutputFileBase(),
                            yyextra->currentMemberDef->anchor(),yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else
      {
        yyextra->code->writeLineNumber(d->getReference(),
                            d->getOutputFileBase(),
                            QCString(),yyextra->yyLineNr);
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
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
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

static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
}

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *fc = yyextra->currentFontClass;
  endCodeLine(yyscanner);
  if (yyextra->yyLineNr<yyextra->inputLines)
  {
    yyextra->currentFontClass = fc;
    startCodeLine(yyscanner);
  }
}

static void codifyLines(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=text,*sp=p;
  char c;
  bool done=false;
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
      yyextra->code->codify(tmp);
      free(tmp);
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(sp);
      done=true;
    }
  }
}

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->startFontClass(s);
  yyextra->currentFontClass=s;
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
    yyextra->needsTermination=true;
  }
  return count;
}

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  yy_size_t c=0;
  while( c < max_size && *s )
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}


// public interface -----------------------------------------------------------

struct SQLCodeParser::Private
{
  yyscan_t yyscanner;
  sqlcodeYY_state state;
};

SQLCodeParser::SQLCodeParser() : p(std::make_unique<Private>())
{
  sqlcodeYYlex_init_extra(&p->state, &p->yyscanner);
#ifdef FLEX_DEBUG
  sqlcodeYYset_debug(1,p->yyscanner);
#endif
  resetCodeParserState();
}

SQLCodeParser::~SQLCodeParser()
{
  sqlcodeYYlex_destroy(p->yyscanner);
}

void SQLCodeParser::resetCodeParserState()
{
  struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
}

void SQLCodeParser::parseCode(CodeOutputInterface &codeOutIntf,
               const QCString &scopeName,
               const QCString &input,
               SrcLangExt,
               bool isExampleBlock,
               const QCString &exampleName,
               FileDef *fileDef,
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
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

  if (input.isEmpty()) return;

  printlex(yy_flex_debug, true, __FILE__, fileDef ? qPrint(fileDef->fileName()): NULL);
  yyextra->fileName      = fileDef ? fileDef->fileName():"";

  yyextra->code = &codeOutIntf;
  yyextra->inputString   = input.data();
  yyextra->inputPosition = 0;
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = false;
  yyextra->searchCtx=searchCtx;

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

  bool cleanupSourceDef = false;

  if (isExampleBlock && fileDef==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef(QCString(),!exampleName.isEmpty() ? exampleName : QCString("generated"));
    cleanupSourceDef = true;
  }

  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }

  yyextra->includeCodeFragment = inlineFragment;
  // Starts line 1 on the output
  startCodeLine(yyscanner);

  sqlcodeYYrestart( 0, yyscanner );

  sqlcodeYYlex(yyscanner);

  if (yyextra->needsTermination)
  {
    endCodeLine(yyscanner);
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete yyextra->sourceFileDef;
    yyextra->sourceFileDef=0;
  }

  printlex(yy_flex_debug, false, __FILE__, fileDef ? qPrint(fileDef->fileName()): NULL);
}

//---------------------------------------------------------------------------------

#if USE_STATE2STRING
#include "sqlcode.l.h"
#endif
