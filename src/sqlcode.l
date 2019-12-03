/******************************************************************************
 *
 * Copyright (C) 1997-2014 by Dimitri van Heesch.
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

struct sqlcodeYY_state
{
     CodeOutputInterface * code;
     const char   *inputString;     //!< the code fragment as text
     int           inputPosition;   //!< read offset during parsing
     int           inputLines;      //!< number of line in the code fragment
     int           yyLineNr;        //!< current line number
     bool          needsTermination;
     const Definition   *searchCtx;

     bool          exampleBlock;
     QCString      exampleName;
     QCString      classScope;

     FileDef      *sourceFileDef;
     Definition   *currentDefinition;
     MemberDef    *currentMemberDef;
     bool          includeCodeFragment;
     const char   *currentFontClass;
};

static void codify(const char* text);
static const char *stateToString(int state);
static void setCurrentDoc(const QCString &anchor,yyscan_t yyscanner);
static void startCodeLine(yyscan_t yyscanner);
static void endFontClass(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void codifyLines(char *text,yyscan_t yyscanner);
static void startFontClass(const char *s,yyscan_t yyscanner);
static int countLines(yyscan_t yyscanner);
static int yyread(char *buf,int max_size,yyscan_t yyscanner);

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size,yyscanner);

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

literalkeyword  (?i:"FALSE"|"TRUE"|"NULL"|"UNKNOWN")
stringliteral   (\"[^"]*\")|('[^']*')
number          [0-9]+
literals        ({literalkeyword}|{stringliteral}|{number})

variable        @{idchar}+

simplecomment   --.*
commentopen     "/\*"
commentclose    "\*/"

%x COMMENT

%%

{literals}          {
                        startFontClass("stringliteral",yyscanner);
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                    }
                

{keyword}           {
                        startFontClass("keyword",yyscanner);
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                    }

{flowkeyword}       {
                        startFontClass("keywordflow",yyscanner);
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                    }

{typekeyword}       {
                        startFontClass("keywordtype",yyscanner);
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                    }

{variable}          {
                        startFontClass("preprocessor",yyscanner);
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                    }

{simplecomment}     {
                        startFontClass("comment",yyscanner);
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                    }

{commentopen}       {
                        startFontClass("comment",yyscanner);
                        codifyLines(yytext,yyscanner);
                        BEGIN(COMMENT);
                    }

<COMMENT>.          {
                        codifyLines(yytext,yyscanner);
 
                    }
<COMMENT>{nl}       {
                        codifyLines(yytext,yyscanner);
                    }

<COMMENT>{commentclose} {
                        codifyLines(yytext,yyscanner);
                        endFontClass(yyscanner);
                        BEGIN(INITIAL);
                    }
                    
{idchar}            {
                        codifyLines(yytext,yyscanner);
                    }

{nl}                {
                        codifyLines(yytext,yyscanner);
                    }

.                   {
                        codifyLines(yytext,yyscanner);
                    }

%%


static void codify(const char* text, yyscan_t yyscanner)
{ 
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->code->codify(text);
}

static void setCurrentDoc(const QCString &anchor, yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    if (yyextra->searchCtx)
    {
      Doxygen::searchIndex->setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),FALSE);
    }
    else
    {
      Doxygen::searchIndex->setCurrentDoc(yyextra->sourceFileDef,anchor,TRUE);
    }
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
    Definition *d   = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    
    if (!yyextra->includeCodeFragment && d && d->isLinkableInProject())
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      yyextra->classScope = d->name().copy();
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                            yyextra->currentMemberDef->getOutputFileBase(),
                            yyextra->currentMemberDef->anchor(),yyextra->yyLineNr);
        setCurrentDoc(lineAnchor,yyscanner);
      }
      else
      {
        yyextra->code->writeLineNumber(d->getReference(),
                            d->getOutputFileBase(),
                            0,yyextra->yyLineNr);
        setCurrentDoc(lineAnchor,yyscanner);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(0,0,0,yyextra->yyLineNr);
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

static void codifyLines(char *text,yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  
  while (!done)
  {
    sp=p;
    
    while ((c=*p++) && c!='\n') { }
    
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      *(p-1)='\0';
      yyextra->code->codify(sp);
      nextCodeLine(yyscanner);
    }
    else
    {
      yyextra->code->codify(sp);
      done=TRUE;
    }
  }
}

static void startFontClass(const char *s,yyscan_t yyscanner)
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
    yyextra->needsTermination=TRUE; 
  } 
  return count;
}

static int yyread(char *buf,int max_size,yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  int c=0;
  while( c < max_size && yyextra->inputString[yyextra->inputPosition] )
  {
    *buf = yyextra->inputString[yyextra->inputPosition++] ;
    c++; buf++;
  }
  return c;
}


// public interface -----------------------------------------------------------
static yyscan_t yyscanner;
static struct sqlcodeYY_state sqlcode_extra;

void parseSqlCode(
    CodeOutputInterface &od,
    const char * /*className*/,
    const QCString &s,
    bool exBlock,
    const char *exName,
    FileDef *fd,
    int startLine,
    int endLine,
    bool inlineFragment,
    const MemberDef *,
    bool,const Definition *searchCtx,
    bool /*collectXRefs*/
    ) 
{  
  if (s.isEmpty()) return;

  sqlcodeYYlex_init_extra(&sqlcode_extra, &yyscanner);
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;

#ifdef FLEX_DEBUG
  yyset_debug(1,yyscanner);
#endif

  printlex(yy_flex_debug, TRUE, __FILE__, fd ? fd->fileName().data(): NULL);
  
  yyextra->code = &od;
  yyextra->inputString   = s;
  yyextra->inputPosition = 0;
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = FALSE;
  yyextra->searchCtx=searchCtx;
  
  if (startLine!=-1)
    yyextra->yyLineNr    = startLine;
  else
    yyextra->yyLineNr    = 1;
  
  if (endLine!=-1)
    yyextra->inputLines  = endLine+1;
  else
    yyextra->inputLines  = yyextra->yyLineNr + countLines(yyscanner) - 1;
  
  yyextra->exampleBlock  = exBlock; 
  yyextra->exampleName   = exName;
  yyextra->sourceFileDef = fd;

  bool cleanupSourceDef = FALSE;
  
  if (exBlock && fd==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef("",(exName?exName:"generated"));
    cleanupSourceDef = TRUE;
  }
  
  if (yyextra->sourceFileDef) 
  {
    setCurrentDoc("l00001",yyscanner);
  }

  yyextra->includeCodeFragment = inlineFragment;
  // Starts line 1 on the output  
  startCodeLine(yyscanner);

  sqlcodeYYrestart( yyin,yyscanner );

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
  
  printlex(yy_flex_debug, FALSE, __FILE__, fd ? fd->fileName().data(): NULL);
  sqlcodeYYlex_destroy(yyscanner);
  return;
}

void resetSqlCodeParserState() 
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
}

//---------------------------------------------------------------------------------

void SQLCodeParser::parseCode(CodeOutputInterface &codeOutIntf,
               const char *scopeName,
               const QCString &input,
               SrcLangExt,
               bool isExampleBlock,
               const char *exampleName,
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
  parseSqlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                fileDef,startLine,endLine,inlineFragment,memberDef,
                showLineNumbers,searchCtx,collectXRefs);
}

void SQLCodeParser::resetCodeParserState()
{
  resetSqlCodeParserState();
}

//---------------------------------------------------------------------------------

#include "sqlcode.l.h"
