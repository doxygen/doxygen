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

#include <qfileinfo.h>
#include <qdir.h>
#include <qtextstream.h>
  
#include "config.h"
#include "version.h"
  
#ifdef DOXYWIZARD  
#include <stdarg.h>
void err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args); 
}
void warn(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}
#else
#include "doxygen.h"
#include "message.h"
#include "pre.h"
#include "version.h"
#include "language.h"
#endif

#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 *	exported variables
 */
  

QCString Config::projectName;          
QCString Config::projectNumber;          
QCString Config::outputDir;          
QCString Config::htmlOutputDir;          
QCString Config::latexOutputDir;          
QCString Config::manOutputDir;          
QCString Config::outputLanguage;
QCString Config::headerFile;          
QCString Config::latexHeaderFile;          
QCString Config::footerFile;          
QCString Config::cgiName;          
QCString Config::cgiURL;          
QCString Config::docURL;          
QCString Config::binAbsPath;          
QCString Config::docAbsPath;          
QCString Config::perlPath;          
QCString Config::genTagFile;
QCString Config::inputFilter;
QCString Config::paperType;
QCString Config::manExtension;
QCString Config::htmlStyleSheet;
QStrList Config::includePath;                
QStrList Config::examplePath;
QStrList Config::imagePath;
QStrList Config::inputSources;               
QStrList Config::excludeSources;
QStrList Config::filePatternList;            
QStrList Config::excludePatternList;
QStrList Config::examplePatternList;
QStrList Config::imagePatternList;
QStrList Config::tagFileList;                
QStrList Config::extDocPathList;             
QStrList Config::predefined;
QStrList Config::extraPackageList;
QStrList Config::stripFromPath;
bool     Config::quietFlag           = FALSE; 
bool     Config::recursiveFlag       = FALSE; 
bool     Config::allExtFlag          = FALSE; 
bool     Config::searchEngineFlag    = FALSE; 
bool     Config::extractAllFlag      = FALSE; 
bool     Config::extractPrivateFlag  = FALSE; 
bool     Config::noIndexFlag         = FALSE;
bool     Config::hideMemberFlag      = FALSE;
bool     Config::hideClassFlag       = FALSE;
bool     Config::macroExpansionFlag  = FALSE;
bool     Config::onlyPredefinedFlag  = FALSE;
bool     Config::fullPathNameFlag    = FALSE;
bool     Config::compactLatexFlag    = FALSE;
bool     Config::internalDocsFlag    = FALSE;
bool     Config::caseSensitiveNames  = FALSE;
bool     Config::sourceBrowseFlag    = FALSE;
bool     Config::htmlHelpFlag        = FALSE;
bool     Config::alphaIndexFlag      = FALSE;
bool     Config::pdfHyperFlag        = FALSE;
bool     Config::alwaysDetailsFlag   = FALSE;
bool     Config::autoBriefFlag       = TRUE;
bool     Config::warningFlag         = TRUE; 
bool     Config::generateHtml        = TRUE;
bool     Config::generateLatex       = TRUE;
bool     Config::generateMan         = TRUE;
bool     Config::preprocessingFlag   = TRUE;
bool     Config::briefMemDescFlag    = TRUE;
bool     Config::searchIncludeFlag   = TRUE;
bool     Config::classDiagramFlag    = TRUE;
bool     Config::repeatBriefFlag     = TRUE;
bool     Config::verbatimHeaderFlag  = TRUE;
bool     Config::htmlAlignMemberFlag = TRUE;
bool     Config::inheritDocsFlag     = TRUE;
bool     Config::inlineInfoFlag      = TRUE;
int      Config::tabSize             = 8;
int      Config::colsInAlphaIndex    = 5;
  
/* -----------------------------------------------------------------
 *
 *	static variables
 */
  
static const char * inputString;
static int	    inputPosition;
static int          yyLineNr;
static QCString     tmpString;
static QCString *   s=0;
static bool    *    b=0;
static QStrList *   l=0;
static int          lastState;
static int          lastEnvState;
static QCString     elemStr;
static QCString     tabSizeString;
static QCString     colsInAlphaIndexString;

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
    int c=0;
    while( c < max_size && inputString[inputPosition] )
    {
	*buf = inputString[inputPosition++] ;
	c++; buf++;
    }
    return c;
}

%}

%option noyywrap

%x      Start
%x	SkipComment
%x      GetString
%x      GetBool
%x      GetStrList
%x      GetQuotedString
%x      GetEnvVar

%%

<*>\0x0d
<Start,GetString,GetStrList,GetBool>"#"	{ BEGIN(SkipComment); }
<Start>"PROJECT_NAME"[ \t]*"="		{ BEGIN(GetString);  s=&Config::projectName;      }
<Start>"PROJECT_NUMBER"[ \t]*"="	{ BEGIN(GetString);  s=&Config::projectNumber;    }
<Start>"OUTPUT_DIRECTORY"[ \t]*"="	{ BEGIN(GetString);  s=&Config::outputDir;        }
<Start>"HTML_OUTPUT"[ \t]*"="		{ BEGIN(GetString);  s=&Config::htmlOutputDir;    }
<Start>"MAN_OUTPUT"[ \t]*"="		{ BEGIN(GetString);  s=&Config::manOutputDir;     }
<Start>"LATEX_OUTPUT"[ \t]*"="		{ BEGIN(GetString);  s=&Config::latexOutputDir;   }
<Start>"HTML_HEADER"[ \t]*"="	        { BEGIN(GetString);  s=&Config::headerFile;       }
<Start>"HTML_FOOTER"[ \t]*"="	        { BEGIN(GetString);  s=&Config::footerFile;       }
<Start>"LATEX_HEADER"[ \t]*"="	        { BEGIN(GetString);  s=&Config::latexHeaderFile;  }
<Start>"CGI_NAME"[ \t]*"="              { BEGIN(GetString);  s=&Config::cgiName;          }
<Start>"CGI_URL"[ \t]*"="               { BEGIN(GetString);  s=&Config::cgiURL;           }
<Start>"DOC_URL"[ \t]*"="		{ BEGIN(GetString);  s=&Config::docURL;           }
<Start>"BIN_ABSPATH"[ \t]*"="		{ BEGIN(GetString);  s=&Config::binAbsPath;       }
<Start>"DOC_ABSPATH"[ \t]*"="		{ BEGIN(GetString);  s=&Config::docAbsPath;       }
<Start>"PERL_PATH"[ \t]*"="		{ BEGIN(GetString);  s=&Config::perlPath;         }
<Start>"GENERATE_TAGFILE"[ \t]*"="	{ BEGIN(GetString);  s=&Config::genTagFile;       }
<Start>"INPUT_FILTER"[ \t]*"="		{ BEGIN(GetString);  s=&Config::inputFilter;      }
<Start>"PAPER_TYPE"[ \t]*"="		{ BEGIN(GetString);  s=&Config::paperType;        }
<Start>"OUTPUT_LANGUAGE"[ \t]*"="	{ BEGIN(GetString);  s=&Config::outputLanguage;   }
<Start>"MAN_EXTENSION"[ \t]*"="		{ BEGIN(GetString);  s=&Config::manExtension;     }
<Start>"TAB_SIZE"[ \t]*"="		{ BEGIN(GetString);  s=&tabSizeString;            }
<Start>"HTML_STYLESHEET"[ \t]*"="	{ BEGIN(GetString);  s=&Config::htmlStyleSheet;   }
<Start>"COLS_IN_ALPHA_INDEX"[ \t]*"="   { BEGIN(GetString);  s=&colsInAlphaIndexString;   }
<Start>"INCLUDE_PATH"[ \t]*"="		{ BEGIN(GetStrList); l=&Config::includePath;        elemStr=""; }
<Start>"EXAMPLE_PATH"[ \t]*"="	        { BEGIN(GetStrList); l=&Config::examplePath;        elemStr=""; }
<Start>"IMAGE_PATH"[ \t]*"="	        { BEGIN(GetStrList); l=&Config::imagePath;          elemStr=""; }
<Start>"INPUT"[ \t]*"="			{ BEGIN(GetStrList); l=&Config::inputSources;       elemStr=""; }
<Start>"EXCLUDE"[ \t]*"="		{ BEGIN(GetStrList); l=&Config::excludeSources;     elemStr=""; }
<Start>"FILE_PATTERNS"[ \t]*"="		{ BEGIN(GetStrList); l=&Config::filePatternList;    elemStr=""; }
<Start>"EXCLUDE_PATTERNS"[ \t]*"="	{ BEGIN(GetStrList); l=&Config::excludePatternList; elemStr=""; }
<Start>"EXAMPLE_PATTERNS"[ \t]*"="	{ BEGIN(GetStrList); l=&Config::examplePatternList; elemStr=""; }
<Start>"IMAGE_PATTERNS"[ \t]*"="	{ BEGIN(GetStrList); l=&Config::imagePatternList;   elemStr=""; }
<Start>"TAGFILES"[ \t]*"="		{ BEGIN(GetStrList); l=&Config::tagFileList;        elemStr=""; }
<Start>"EXT_DOC_PATHS"[ \t]*"="		{ BEGIN(GetStrList); l=&Config::extDocPathList;     elemStr=""; }
<Start>"PREDEFINED"[ \t]*"="            { BEGIN(GetStrList); l=&Config::predefined;         elemStr=""; }
<Start>"EXTRA_PACKAGES"[ \t]*"="	{ BEGIN(GetStrList); l=&Config::extraPackageList;   elemStr=""; }
<Start>"STRIP_FROM_PATH"[ \t]*"="	{ BEGIN(GetStrList); l=&Config::stripFromPath;      elemStr=""; }
<Start>"QUIET"[ \t]*"="			{ BEGIN(GetBool);    b=&Config::quietFlag;        }
<Start>"WARNINGS"[ \t]*"="              { BEGIN(GetBool);    b=&Config::warningFlag;      }
<Start>"RECURSIVE"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::recursiveFlag;    }
<Start>"ALLEXTERNALS"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::allExtFlag;       }
<Start>"SEARCHENGINE"[ \t]*"="          { BEGIN(GetBool);    b=&Config::searchEngineFlag; }
<Start>"EXTRACT_ALL"[ \t]*"="           { BEGIN(GetBool);    b=&Config::extractAllFlag;   }
<Start>"EXTRACT_PRIVATE"[ \t]*"="       { BEGIN(GetBool);    b=&Config::extractPrivateFlag;   }
<Start>"DISABLE_INDEX"[ \t]*"="	        { BEGIN(GetBool);    b=&Config::noIndexFlag; }
<Start>"GENERATE_LATEX"[ \t]*"="	{ BEGIN(GetBool);    b=&Config::generateLatex; }
<Start>"GENERATE_HTML"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::generateHtml; }
<Start>"GENERATE_MAN"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::generateMan; }
<Start>"ENABLE_PREPROCESSING"[ \t]*"="  { BEGIN(GetBool);    b=&Config::preprocessingFlag; }
<Start>"MACRO_EXPANSION"[ \t]*"="	{ BEGIN(GetBool);    b=&Config::macroExpansionFlag; }
<Start>"SEARCH_INCLUDES"[ \t]*"="	{ BEGIN(GetBool);    b=&Config::searchIncludeFlag; }
<Start>"BRIEF_MEMBER_DESC"[ \t]*"="     { BEGIN(GetBool);    b=&Config::briefMemDescFlag; }
<Start>"ALWAYS_DETAILED_SEC"[ \t]*"="   { BEGIN(GetBool);    b=&Config::alwaysDetailsFlag; }
<Start>"HIDE_UNDOC_MEMBERS"[ \t]*"="    { BEGIN(GetBool);    b=&Config::hideMemberFlag; }
<Start>"HIDE_UNDOC_CLASSES"[ \t]*"="    { BEGIN(GetBool);    b=&Config::hideClassFlag; }
<Start>"EXPAND_ONLY_PREDEF"[ \t]*"="    { BEGIN(GetBool);    b=&Config::onlyPredefinedFlag; }
<Start>"FULL_PATH_NAMES"[ \t]*"="	{ BEGIN(GetBool);    b=&Config::fullPathNameFlag; }
<Start>"CLASS_DIAGRAMS"[ \t]*"="	{ BEGIN(GetBool);    b=&Config::classDiagramFlag; }
<Start>"COMPACT_LATEX"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::compactLatexFlag; }
<Start>"REPEAT_BRIEF"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::repeatBriefFlag; }
<Start>"INTERNAL_DOCS"[ \t]*"="		{ BEGIN(GetBool);    b=&Config::internalDocsFlag; }
<Start>"CASE_SENSE_NAMES"[ \t]*"="      { BEGIN(GetBool);    b=&Config::caseSensitiveNames; }
<Start>"VERBATIM_HEADERS"[ \t]*"="      { BEGIN(GetBool);    b=&Config::verbatimHeaderFlag; }
<Start>"HTML_ALIGN_MEMBERS"[ \t]*"="    { BEGIN(GetBool);    b=&Config::htmlAlignMemberFlag; }
<Start>"SOURCE_BROWSER"[ \t]*"="        { BEGIN(GetBool);    b=&Config::sourceBrowseFlag; }
<Start>"JAVADOC_AUTOBRIEF"[ \t]*"="     { BEGIN(GetBool);    b=&Config::autoBriefFlag; }
<Start>"GENERATE_HTMLHELP"[ \t]*"="     { BEGIN(GetBool);    b=&Config::htmlHelpFlag; }
<Start>"ALPHABETICAL_INDEX"[ \t]*"="    { BEGIN(GetBool);    b=&Config::alphaIndexFlag; }
<Start>"PDF_HYPERLINKS"[ \t]*"="	{ BEGIN(GetBool);    b=&Config::pdfHyperFlag; }
<Start>"INHERIT_DOCS"[ \t]*"="	        { BEGIN(GetBool);    b=&Config::inheritDocsFlag; }
<Start>"INLINE_INFO"[ \t]*"="	        { BEGIN(GetBool);    b=&Config::inlineInfoFlag; }
<Start>[a-z_A-Z0-9]+			{ err("Warning: ignoring unknown tag `%s' at line %d\n",yytext,yyLineNr); }
<GetString,GetBool>\n			{ yyLineNr++; BEGIN(Start); }
<GetStrList>\n				{ 
  					  yyLineNr++; 
					  if (elemStr.length()>0)
					  {
					    //printf("elemStr1=`%s'\n",elemStr.data());
					    l->append(elemStr);
					  }
					  BEGIN(Start); 
					}
<GetStrList>[ \t]+			{
  				          if (elemStr.length()>0)
					  {
					    //printf("elemStr2=`%s'\n",elemStr.data());
  					    l->append(elemStr);
					  }
					  elemStr.resize(0);
  					}
<GetString>[^ \"\$\t\r\n]+		{ (*s)+=yytext; }
<GetString,GetStrList>"\""		{ lastState=YY_START;
  					  BEGIN(GetQuotedString); 
                                          tmpString.resize(0); 
					}
<GetString,GetStrList,GetQuotedString>"\$\("	{
  				          //printf(">> Enter env\n"); 
  					  lastEnvState=YY_START;
  					  BEGIN(GetEnvVar);
  					}
<GetEnvVar>[a-z_A-Z0-9]+")"		{
  					  yytext[yyleng-1]='\0';
					  const char *env=getenv(yytext);
					  int i;
					  int l=strlen(env);
					  //printf("env name=`%s' text=`%s'\n",yytext,env);
					  for (i=l-1;i>=0;i--) unput(env[i]);
					  BEGIN(lastEnvState);
  					}
<GetQuotedString>"\""|"\n" 		{ 
  					  //printf("Quoted String = `%s'\n",tmpString.data());
  					  if (lastState==GetString)
					    (*s)+=tmpString;
					  else
					    elemStr+=tmpString;
					  if (*yytext=='\n')
					  {
					    err("Warning: Missing end quote (\") on line %d\n",yyLineNr);
					    yyLineNr++;
					  }
					  BEGIN(lastState);
  					}
<GetQuotedString>"\\\""			{
  					  tmpString+='"';
  					}
<GetQuotedString>.			{ tmpString+=*yytext; }
<GetBool>[a-zA-Z]+			{ 
  					  QCString bs=yytext; 
  					  bs=bs.upper();
  					  if (bs=="YES")
					    *b=TRUE;
					  else if (bs=="NO")
					    *b=FALSE;
					  else 
					  {
					    *b=FALSE; 
					    warn("Warning: Invalid value `%s' for "
						 "boolean tag in line %d; use YES or NO\n",
						 bs.data(),yyLineNr);
					  }
					}
<GetStrList>[^ \#\"\$\t\r\n]+		{
  					  elemStr+=yytext;
  					}
<SkipComment>\n				{ yyLineNr++; BEGIN(Start); }
<SkipComment>\\[ \r\t]*\n		{ yyLineNr++; BEGIN(Start); }
<*>\\[ \r\t]*\n				{ yyLineNr++; }
<*>.					
<*>\n					{ yyLineNr++ ; }

%%

/*@ ----------------------------------------------------------------------------
 */


void dumpConfig()
{
  printf("projectName=`%s'\n",Config::projectName.data());
  printf("outputDir=`%s'\n",  Config::outputDir.data());
  printf("headerFile=`%s'\n", Config::headerFile.data());
  printf("footerFile=`%s'\n", Config::footerFile.data());
  char *ip=Config::includePath.first();
  while (ip)
  {
    printf("includePath=`%s'\n",ip);
    ip=Config::includePath.next();
  }
  printf("quiet=`%d'\n",      Config::quietFlag);
  printf("warnings=`%d'\n",   Config::warningFlag);
  char *is=Config::inputSources.first();
  while (is)
  {
    printf("inputSources=`%s'\n",is);
    is=Config::inputSources.next();
  }
  char *fp=Config::filePatternList.first();
  while (fp)
  {
    printf("filePattern=`%s'\n",fp);
    fp=Config::filePatternList.next();
  }
  printf("recusive=`%d'\n",Config::recursiveFlag);
  printf("inputFilter=`%s'\n",Config::inputFilter.data());
  char *tf=Config::tagFileList.first();
  while (tf)
  {
    printf("tagFile=`%s'\n",tf);
    tf=Config::tagFileList.next();
  }
  printf("allExternals=`%d'\n",Config::allExtFlag);
  printf("searchEngine=`%d'\n",Config::searchEngineFlag);
  printf("cgiName=`%s'\n",Config::cgiName.data());
  printf("cgiURL=`%s'\n",Config::cgiURL.data());
  printf("docURL=`%s'\n",Config::docURL.data());
  printf("binAbsPath=`%s'\n",Config::binAbsPath.data());
  char *ed=Config::extDocPathList.first();
  while (ed)
  {
    printf("binAbsPathFile=`%s'\n",ed);
    ed=Config::extDocPathList.next();
  }
}

void Config::init()
{
  Config::projectName.resize(0);          
  Config::projectNumber.resize(0);          
  Config::outputDir.resize(0);
  Config::htmlOutputDir = "html";          
  Config::latexOutputDir ="latex";          
  Config::manOutputDir ="man";          
  Config::outputLanguage = "English";
  Config::headerFile.resize(0);          
  Config::latexHeaderFile.resize(0);          
  Config::footerFile.resize(0);          
  Config::cgiName = "search.cgi";          
  Config::cgiURL.resize(0);          
  Config::docURL.resize(0);          
  Config::binAbsPath = "/usr/local/bin/";          
  Config::docAbsPath.resize(0);          
  Config::perlPath = "/usr/bin/perl";          
  Config::genTagFile.resize(0);
  Config::inputFilter.resize(0);
  Config::paperType = "a4wide";
  Config::manExtension = ".3";
  Config::htmlStyleSheet.resize(0);
  Config::includePath.clear();                
  Config::examplePath.clear();
  Config::imagePath.clear();
  Config::inputSources.clear();               
  Config::excludeSources.clear();
  Config::filePatternList.clear();            
  Config::examplePatternList.clear();            
  Config::imagePatternList.clear();            
  Config::excludePatternList.clear();
  Config::tagFileList.clear();                
  Config::extDocPathList.clear();             
  Config::predefined.clear();
  Config::extraPackageList.clear();
  Config::stripFromPath.clear();
  Config::tabSize=8;
  Config::colsInAlphaIndex=5;
  Config::quietFlag           = FALSE; 
  Config::recursiveFlag       = FALSE; 
  Config::allExtFlag          = FALSE; 
  Config::searchEngineFlag    = FALSE; 
  Config::extractAllFlag      = FALSE; 
  Config::extractPrivateFlag  = FALSE; 
  Config::noIndexFlag         = FALSE;
  Config::hideMemberFlag      = FALSE;
  Config::hideClassFlag       = FALSE;
  Config::macroExpansionFlag  = FALSE;
  Config::onlyPredefinedFlag  = FALSE;
  Config::fullPathNameFlag    = FALSE;
  Config::compactLatexFlag    = FALSE;
  Config::internalDocsFlag    = FALSE;
  Config::caseSensitiveNames  = FALSE;
  Config::sourceBrowseFlag    = FALSE;
  Config::htmlHelpFlag        = FALSE;
  Config::alphaIndexFlag      = FALSE;
  Config::pdfHyperFlag        = FALSE;
  Config::alwaysDetailsFlag   = FALSE;
  Config::warningFlag         = TRUE; 
  Config::generateHtml        = TRUE;
  Config::generateLatex       = TRUE;
  Config::generateMan         = TRUE;
  Config::preprocessingFlag   = TRUE;
  Config::briefMemDescFlag    = TRUE;
  Config::searchIncludeFlag   = TRUE;
  Config::classDiagramFlag    = TRUE;
  Config::repeatBriefFlag     = TRUE;
  Config::verbatimHeaderFlag  = TRUE;
  Config::htmlAlignMemberFlag = TRUE;
  Config::autoBriefFlag       = TRUE;
  Config::inheritDocsFlag     = TRUE;
  Config::inlineInfoFlag      = TRUE;
}

void writeTemplateConfig(QFile *f,bool sl)
{
  QTextStream t(f);
#ifdef DOXYWIZARD
  t << "# Doxygen configuration generated by Doxywizard version " << versionString << endl;
#else
  t << "# Doxyfile " << versionString << endl << endl;
#endif
  if (!sl)
  {
    t << "# This file describes the settings to be used by doxygen for a project\n";
    t << "#\n";
    t << "# All text after a hash (#) is considered a comment and will be ignored\n";
    t << "# The format is:\n";
    t << "#       TAG = value [value, ...]\n";
    t << "# Values that contain spaces should be placed between quotes (\" \")\n";
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# General configuration options\n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# The PROJECT_NAME tag is a single word (or a sequence of word surrounded\n";
    t << "# by quotes) that should identify the project. \n";
    t << "\n";
  }
  t << "PROJECT_NAME         =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The PROJECT_NUMBER tag can be used to enter a project or revision number.\n" ;
    t << "# This could be handy for archiving the generated documentation or \n";
    t << "# if some version control system is used.\n";
    t << "\n";
  }
  t << "PROJECT_NUMBER       =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The OUTPUT_DIRECTORY tag is used to specify the (relative or absolute) \n";
    t << "# base path where the generated documentation will be put. \n";
    t << "# If a relative path is entered, it will be relative to the location \n";
    t << "# where doxygen was started. If left blank the current directory will be used.\n";
    t << "\n";
  }
  t << "OUTPUT_DIRECTORY     =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The OUTPUT_LANGUAGE tag is used to specify the language in which all\n";
    t << "# documentation generated by doxygen is written. Doxygen will use this\n";
    t << "# information to generate all constant output in the proper language.\n";
    t << "# The default language is English, other supported languages are: \n";
    t << "# Dutch, French, Italian, Czech, Swedish, German and Japanese\n";
    t << "\n";
  }
  t << "OUTPUT_LANGUAGE      = English\n";
  if (!sl)
  {
    t << "\n";
    t << "# The QUIET tag can be used to turn on/off the messages that are generated\n";
    t << "# by doxygen. Possible values are YES and NO. If left blank NO is used.\n";
    t << "\n";
  }
  t << "QUIET                = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# The WARNINGS tag can be used to turn on/off the warning messages that are\n";
    t << "# generated by doxygen. Possible values are YES and NO. If left blank\n";
    t << "# NO is used.\n";
    t << "\n";
  }
  t << "WARNINGS             = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# The DISABLE_INDEX tag can be used to turn on/off the condensed index at\n";
    t << "# top of each HTML page. The value NO (the default) enables the index and\n";
    t << "# the value YES disables it.\n";
    t << "\n";
  }
  t << "DISABLE_INDEX        = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the EXTRACT_ALL tag is set to YES all classes and functions will be\n";
    t << "# included in the documentation, even if no documentation was available.\n";
    t << "\n";
  }
  t << "EXTRACT_ALL          = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the EXTRACT_PRIVATE tag is set to YES all private members of a class\n";
    t << "# will be included in the documentation.\n";
    t << "\n";
  }
  t << "EXTRACT_PRIVATE      = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the HIDE_UNDOC_MEMBERS tag is set to YES, Doxygen will hide all\n";
    t << "# undocumented members inside documented classes or files.\n";
    t << "\n";
  }
  t << "HIDE_UNDOC_MEMBERS   = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the HIDE_UNDOC_CLASSESS tag is set to YES, Doxygen will hide all\n";
    t << "# undocumented classes.\n";
    t << "\n";
  }
  t << "HIDE_UNDOC_CLASSES   = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the BRIEF_MEMBER_DESC tag is set to YES (the default) Doxygen will\n";
    t << "# include brief member descriptions after the members that are listed in \n";
    t << "# the file and class documentation (similar to JavaDoc).\n";
    t << "# Set to NO to disable this.\n";
    t << "\n";
  }
  t << "BRIEF_MEMBER_DESC    = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend\n";
    t << "# the brief description of a member or function before the detailed description.\n";
    t << "# Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the \n";
    t << "# brief descriptions will be completely suppressed.\n";
    t << "\n";
  }
  t << "REPEAT_BRIEF         = YES\n";
  if (!sl)
  {
    t <<"\n";
    t << "# If the ALWAYS_DETAILS_SEC and REPEAT_BRIEF tags are both set to YES then\n";
    t << "# Doxygen will generate a detailed section even if there is only a brief\n";
    t << "# description.\n"; 
    t <<"\n";
  }
  t << "ALWAYS_DETAILED_SEC  = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the FULL_PATH_NAMES tag is set to YES then Doxygen will prepend the full\n";
    t << "# path before files name in the file list and in the header files. If set\n" ;
    t << "# to NO the shortest path that makes the file name unique will be used.\n";
    t << "\n";
  }
  t << "FULL_PATH_NAMES      = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the FULL_PATH_NAMES tag is set to YES then the STRIP_FROM_PATH tag\n";
    t << "# can be used to strip a user defined part of the path. Stripping is\n" ;
    t << "# only done if one of the specified strings matches the left-hand part of\n";
    t << "# the path.\n";
    t << "\n";
  }
  t << "STRIP_FROM_PATH      =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The INTERNAL_DOCS tag determines if documentation\n";
    t << "# that is typed after a \\internal command is included. If the tag is set \n";
    t << "# to NO (the default) then the documentation will be excluded.\n";
    t << "# Set it to YES to include the internal documentation.\n";
    t << "\n";
  }
  t << "INTERNAL_DOCS        = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will\n";
    t << "# generate a class diagram (in Html and LaTeX) for classes with base or\n";
    t << "# super classes. Setting the tag to NO turns the diagrams off.\n";
    t << "\n";
  }
  t << "CLASS_DIAGRAMS       = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the SOURCE_BROWSER tag is set to YES than the body of a member or\n";
    t << "# function will be appended as a block of code to the documentation of.\n";
    t << "# that member or function.\n";
    t << "\n";
  }
  t << "SOURCE_BROWSER       = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the CASE_SENSE_NAMES tag is set to NO (the default) then Doxygen\n";
    t << "# will only generate file names in lower case letters. If set to\n";
    t << "# YES upper case letters are also allowed. This is useful if you have\n";
    t << "# classes or files whose names only differ in case and if your file system\n";
    t << "# supports case sensitive file names.\n";
    t << "\n";
  }
  t << "CASE_SENSE_NAMES     = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the VERBATIM_HEADERS tag is set to YES (the default) then Doxygen\n";
    t << "# will generate a verbatim copy of the header file for each class for\n";
    t << "# which an include is specified. Set to NO to disable this.\n";
    t << "\n";
  }
  t << "VERBATIM_HEADERS     = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the JAVADOC_AUTOBRIEF tag is set to YES (the default) then Doxygen\n";
    t << "# will interpret the first line (until the first dot) of a JavaDoc-style\n";
    t << "# comment as the brief description. If set to NO, the Javadoc-style will\n";
    t << "# behave just like the Qt-style comments.\n";
    t << "\n";
  }
  t << "JAVADOC_AUTOBRIEF    = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# if the INHERIT_DOCS tag is set to YES (the default) then an undocumented\n";
    t << "# member inherits the documentation from any documented member that it\n";
    t << "# reimplements.\n";
    t << "\n";
  }
  t << "INHERIT_DOCS         = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# if the INLINE_INFO tag is set to YES (the default) then a tag [inline]\n";
    t << "# is inserted in the documentation for inline members.\n";
    t << "\n";
  }
  t << "INLINE_INFO          = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# the TAB_SIZE tag can be used to set the number of spaces in a tab\n";
    t << "# Doxygen uses this value to replace tabs by spaces in code fragments.\n";
    t << "\n";
  }
  t << "TAB_SIZE             = 8\n";
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# configuration options related to the input files\n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# The INPUT tag can be used to specify the files and/or directories that contain \n";
    t << "# documented source files. You may enter file names like \"myfile.cpp\" or \n";
    t << "# directories like \"/usr/src/myproject\". Separate the files or directories \n";
    t << "# with spaces.\n";
    t << "\n";
  }
  t << "INPUT                =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the value of the INPUT tag contains directories, you can use the \n";
    t << "# FILE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n";
    t << "# and *.h) to filter out the source-files in the directories. If left \n";
    t << "# blank all files are included.\n";
    t << "\n";
  }
  t << "FILE_PATTERNS        =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The RECURSIVE tag can be used to turn specify whether or not subdirectories\n";
    t << "# should be searched for input files as well. Possible values are YES and NO.\n";
    t << "# If left blank NO is used.\n";
    t << "\n";
  }
  t << "RECURSIVE            = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# The EXCLUDE tag can be used to specify files and/or directories that should\n";
    t << "# excluded from the INPUT source files. This way you can easily exclude a \n";
    t << "# subdirectory from a directory tree whose root is specified with the INPUT tag.\n";
    t << "\n";
  }
  t << "EXCLUDE              =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the value of the INPUT tag contains directories, you can use the\n";
    t << "# EXCLUDE_PATTERNS tag to specify one or more wildcard patterns to exclude\n";
    t << "# certain files from those directories.\n";
    t << "\n";
  }
  t << "EXCLUDE_PATTERNS     =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The EXAMPLE_PATH tag can be used to specify one or more files or \n";
    t << "# directories that contain example code fragments that are included (see \n";
    t << "# the \\include command).\n";
    t << "\n";
  }
  t << "EXAMPLE_PATH         =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the value of the EXAMPLE_PATH tag contains directories, you can use the\n";
    t << "# EXAMPLE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n";
    t << "# and *.h) to filter out the source-files in the directories. If left \n";
    t << "# blank all files are included.\n";
    t << "\n";
  }
  t << "EXAMPLE_PATTERNS     =\n";

  if (!sl)
  {
    t << "\n";
    t << "# The IMAGE_PATH tag can be used to specify one or more files or \n";
    t << "# directories that contain image that are included in the documentation (see \n";
    t << "# the \\image command).\n";
    t << "\n";
  }
  t << "IMAGE_PATH           =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the value of the IMAGE_PATH tag contains directories, you can use the\n";
    t << "# IMAGE_PATTERNS tag to specify one or more wildcard pattern (like *.gif \n";
    t << "# and *.eps) to filter out the image files in the directories. If left \n";
    t << "# blank all files are included.\n";
    t << "\n";
  }
  t << "IMAGE_PATTERNS       =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The INPUT_FILTER tag can be used to specify a program that doxygen should\n";
    t << "# invoke to filter for each input file. Doxygen will invoke the filter program \n";
    t << "# by executing (via popen()) the command <filter> <input-file>, where <filter>\n";
    t << "# is the value of the INPUT_FILTER tag, and <input-file> is the name of an\n";
    t << "# input file. Doxygen will then use the output that the filter program writes\n";
    t << "# to standard output.\n";
    t << "\n";
  }
  t << "INPUT_FILTER         =\n";
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# configuration options related to the HTML output\n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the GENERATE_HTML tag is set to YES (the default) Doxygen will\n";
    t << "# generate HTML output\n";
    t << "\n";
  }
  t << "GENERATE_HTML        = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# The HTML_OUTPUT tag is used to specify where the HTML docs will be put.\n";
    t << "# If a relative path is entered the value of OUTPUT_DIRECTORY will be\n";
    t << "# put in front of it. If left blank `html' will be used as the default path.\n";
    t << "\n";
  }
  t << "HTML_OUTPUT          =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The HTML_HEADER tag can be used to specify a personal HTML header for \n";
    t << "# each generated HTML page. If it is left blank doxygen will generate a \n";
    t << "# standard header.\n";
    t << "\n";
  }
  t << "HTML_HEADER          =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The HTML_FOOTER tag can be used to specify a personal HTML footer for \n";
    t << "# each generated HTML page. If it is left blank doxygen will generate a \n";
    t << "# standard footer.\n";
    t << "\n";
  }
  t << "HTML_FOOTER          =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The HTML_STYLESHEET tag can be used to specify a user defined cascading\n";
    t << "# style sheet that is used by each HTML page. It can be used to \n";
    t << "# fine-tune the look of the HTML output. If the tag is left blank doxygen\n"; 
    t << "# will generate a default style sheet\n";
    t << "\n";
  }
  t << "HTML_STYLESHEET      =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes,\n";
    t << "# files or namespaces will be aligned in HTML using tables. If set to\n";
    t << "# NO a bullet list will be used.\n";
    t << "\n";
  }
  t << "HTML_ALIGN_MEMBERS   = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the GENERATE_HTMLHELP tag is set to YES, additional index files\n";
    t << "# will be generated that can be used as input for tools like the\n";
    t << "# Microsoft HTML help workshop to generate a compressed HTML help file (.chm)\n";
    t << "# of the generated HTML documentation.\n";
    t << "\n";
  }
  t << "GENERATE_HTMLHELP    = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index\n";
    t << "# of all compounds will be generated. Enable this if the project\n";
    t << "# contains a lot of classes, structs, unions or interfaces.\n";
    t << "\n";
  }
  t << "ALPHABETICAL_INDEX   = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the alphabetical index is enabled (see ALPHABETICAL_INDEX) then\n";
    t << "# the COLS_IN_ALPHA_INDEX tag can be used to specify the number of columns\n";
    t << "# in which this list will be split (can be a number in the range [1..20])\n";
    t << "\n";
  }
  t << "COLS_IN_ALPHA_INDEX  = 5\n";
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# configuration options related to the LaTeX output\n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the GENERATE_LATEX tag is set to YES (the default) Doxygen will\n";
    t << "# generate Latex output.\n";
    t << "\n";
  }
  t << "GENERATE_LATEX       = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# The LATEX_OUTPUT tag is used to specify where the LaTeX docs will be put.\n";
    t << "# If a relative path is entered the value of OUTPUT_DIRECTORY will be\n";
    t << "# put in front of it. If left blank `latex' will be used as the default path.\n";
    t << "\n";
  }
  t << "LATEX_OUTPUT         =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the COMPACT_LATEX tag is set to YES Doxygen generates more compact\n";
    t << "# LaTeX documents. This may be useful for small projects and may help to\n";
    t << "# save some trees in general.\n"; 
    t << "\n";
  }
  t << "COMPACT_LATEX        = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# The PAPER_TYPE tag can be used to set the paper type that is used\n";
    t << "# by the printer. Possible values are: a4, a4wide, letter, legal and \n";
    t << "# executive. If left blank a4wide will be used.\n";
    t << "\n";
  }
  t << "PAPER_TYPE           = a4wide\n";
  if (!sl)
  {
    t << "\n";
    t << "# The EXTRA_PACKAGES tag can be to specify one or more names of LaTeX\n";
    t << "# packages that should be included in the LaTeX output.\n";
    t << "\n";
  }
  t << "EXTRA_PACKAGES       =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The LATEX_HEADER tag can be used to specify a personal LaTeX header for \n";
    t << "# the generated latex document. The header should contain everything until\n";
    t << "# the first chapter. If it is left blank doxygen will generate a \n";
    t << "# standard header. Notice: only use this tag if you know what you are doing!\n";
    t << "\n";
  }
  t << "LATEX_HEADER          =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the PDF_HYPERLINKS tag is set to YES, the LaTeX that is generated\n";
    t << "# is prepared for conversion to pdf (using ps2pdf). The pdf file will\n";
    t << "# contain links (just like the HTML output) instead of page references\n";
    t << "# This makes the output suitable for online browsing using a pdf viewer.\n";
    t << "\n";
  }
  t << "PDF_HYPERLINKS       = NO\n";
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# configuration options related to the man page output\n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the GENERATE_MAN tag is set to YES (the default) Doxygen will\n";
    t << "# generate man pages\n";
    t << "\n";
  }
  t << "GENERATE_MAN         = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# The MAN_OUTPUT tag is used to specify where the man pages will be put.\n";
    t << "# If a relative path is entered the value of OUTPUT_DIRECTORY will be\n";
    t << "# put in front of it. If left blank `man' will be used as the default path.\n";
    t << "\n";
  }
  t << "MAN_OUTPUT           =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The MAN_EXTENSION tag determines the extension that is added to\n";
    t << "# the generated man pages (default is the subroutine's section .3)\n";
    t << "\n";
  }
  t << "MAN_EXTENSION        = .3\n";
 
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# Configuration options related to the preprocessor \n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will\n";
    t << "# evaluate all C-preprocessor directives found in the sources and include\n";
    t << "# files.\n";
    t << "\n";
  }
  t << "ENABLE_PREPROCESSING = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the MACRO_EXPANSION tag is set to YES Doxygen will expand all macro\n";
    t << "# names in the source code. If set to NO (the default) only conditional \n";
    t << "# compilation will be performed.\n";
    t << "\n";
  }
  t << "MACRO_EXPANSION      = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the SEARCH_INCLUDES tag is set to YES (the default) the includes files\n";
    t << "# in the INCLUDE_PATH (see below) will be search if a #include is found.\n";
    t << "\n";
  }
  t << "SEARCH_INCLUDES      = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# The INCLUDE_PATH tag can be used to specify one or more directories that\n";
    t << "# contain include files that are not input files but should be processed by\n";
    t << "# the preprocessor.\n" ;
    t << "\n";
  }
  t << "INCLUDE_PATH         =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The PREDEFINED tag can be used to specify one or more macro names that\n";
    t << "# are defined before the preprocessor is started (similar to the -D option of\n";
    t << "# gcc). The argument of the tag is a list of macros of the form: name\n";
    t << "# or name=definition (no spaces). If the definition and the = are \n";
    t << "# omitted =1 is assumed.\n";
    t << "\n";
  }
  t << "PREDEFINED           =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the EXPAND_ONLY_PREDEF and MACRO_EXPANSION tags are both set to YES\n";
    t << "# then the macro expansion is limited to the macros specified with the\n";
    t << "# PREDEFINED tag.\n";
    t << "\n";
  }
  t << "EXPAND_ONLY_PREDEF   = NO\n";
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# Configuration options related to external references \n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# The TAGFILES tag can be used to specify one or more tagfiles. \n";
    t << "\n";
  }
  t << "TAGFILES             =\n";
  if (!sl)
  {
    t << "\n";
    t << "# When a file name is specified after GENERATE_TAGFILE, doxygen will create\n";
    t << "# a tag file that is based on the input files it reads.\n";
    t << "\n";
  }
  t << "GENERATE_TAGFILE     =\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the ALLEXTERNALS tag is set to YES all external classes will be listed\n";
    t << "# in the class index. If set to NO only the inherited external classes\n";
    t << "# will be listed.\n";
    t << "\n";
  }
  t << "ALLEXTERNALS         = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# The PERL_PATH should be the absolute path and name of the perl script\n";
    t << "# interpreter (i.e. the result of `which perl').\n";
    t << "\n";
  }
  t << "PERL_PATH            = /usr/bin/perl\n";
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# Configuration options related to the search engine \n";
  t << "#---------------------------------------------------------------------------\n";
  if (!sl)
  {
    t << "\n";
    t << "# The SEARCHENGINE tag specifies whether or not a search engine should be \n";
    t << "# used. If set to NO the values of all tags below this one will be ignored.\n";
    t << "\n";
  }
  t << "SEARCHENGINE         = NO\n";
  if (!sl)
  {
    t << "\n";
    t << "# The CGI_NAME tag should be the name of the CGI script that\n";
    t << "# starts the search engine (doxysearch) with the correct parameters.\n";
    t << "# A script with this name will be generated by doxygen.\n";
    t << "\n";
  }
  t << "CGI_NAME             = search.cgi\n";
  if (!sl)
  {
    t << "\n";
    t << "# The CGI_URL tag should be the absolute URL to the directory where the\n";
    t << "# cgi binaries are located. See the documentation of your http daemon for \n";
    t << "# details.\n";
    t << "\n";
  }
  t << "CGI_URL              =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The DOC_URL tag should be the absolute URL to the directory where the\n";
    t << "# documentation is located. If left blank the absolute path to the \n";
    t << "# documentation, with file:// prepended to it, will be used.\n";
    t << "\n";
  }
  t << "DOC_URL              =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The DOC_ABSPATH tag should be the absolute path to the directory where the\n";
    t << "# documentation is located. If left blank the directory on the local machine\n";
    t << "# will be used.\n";
    t << "\n";
  }
  t << "DOC_ABSPATH          =\n";
  if (!sl)
  {
    t << "\n";
    t << "# The BIN_ABSPATH tag must point to the directory where the doxysearch binary\n";
    t << "# is installed.\n";
    t << "\n";
  }
  t << "BIN_ABSPATH          = /usr/local/bin/\n";
  if (!sl)
  {
    t << "\n";
    t << "# The EXT_DOC_PATHS tag can be used to specify one or more paths to \n";
    t << "# documentation generated for other projects. This allows doxysearch to search\n";
    t << "# the documentation for these projects as well.\n";
    t << "\n";
  }
  t << "EXT_DOC_PATHS        =\n";
}

void checkConfig()
{
  //if (projectName.length()>0)
  //{
  //  projectName[0]=toupper(projectName[0]);
  //}

  if (tabSizeString.isEmpty())
  {
    Config::tabSize=8;
  }
  else
  {
    bool ok;
    int ts = tabSizeString.toInt(&ok);
    if (!ok || ts<1 || ts>16)
    {
      warn("Warning: argument of TAB_SIZE is not a valid number, using tab size of 8 spaces!\n");
      ts=8;
    }
    Config::tabSize = ts;
  }
  
  if (colsInAlphaIndexString.isEmpty())
  {
    Config::colsInAlphaIndex=5;
  }
  else
  {
    bool ok;
    int cols = colsInAlphaIndexString.toInt(&ok);
    if (!ok || cols<1 || cols>20)
    {
      warn("Warning: argument of COLS_IN_ALPHA_INDEX is not a valid number in the range [1..20]!\n"
	   "Using the default of 5 columns!\n");
      cols = 5;
    }
    Config::colsInAlphaIndex=cols;
  }
  
  // set default man page extension if non is given by the user
  if (Config::manExtension.isEmpty())
  {
    Config::manExtension=".3";
  }
  
  Config::paperType = Config::paperType.lower().stripWhiteSpace(); 
  if (Config::paperType.isEmpty())
  {
    Config::paperType = "a4wide";
  }
  if (Config::paperType!="a4" && Config::paperType!="a4wide" && Config::paperType!="letter" && 
      Config::paperType!="legal" && Config::paperType!="executive")
  {
    err("Error: Unknown page type specified");
  }
  
  Config::outputLanguage=Config::outputLanguage.stripWhiteSpace();
  if (Config::outputLanguage.isEmpty())
  {
    Config::outputLanguage = "English";
#ifndef DOXYWIZARD
    setTranslator("English");
#endif
  }
  else
  {
#ifndef DOXYWIZARD
    if (!setTranslator(Config::outputLanguage))
    {
      err("Error: Output language %s not supported! Using English instead.\n",
	  Config::outputLanguage.data());
    }
#endif
  }
  
  // Test to see if output directory is valid
  if (Config::outputDir.isEmpty()) 
    Config::outputDir=QDir::currentDirPath();
  else
  {
    QDir dir(Config::outputDir);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(Config::outputDir))
      {
        err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
	    "exist and cannot be created\n",Config::outputDir.data());
        exit(1);
      }
      else if (!Config::quietFlag)
      {
	err("Notice: Output directory `%s' does not exist. "
	    "I have created it for you.\n", Config::outputDir.data());
      }
      dir.cd(Config::outputDir);
    }
    Config::outputDir=dir.absPath();
  }

  if (Config::htmlOutputDir.isEmpty() && Config::generateHtml)
  {
    Config::htmlOutputDir=Config::outputDir+"/html";
  }
  else if (Config::htmlOutputDir && Config::htmlOutputDir[0]!='/')
  {
    Config::htmlOutputDir.prepend(Config::outputDir+'/');
  }
  QDir htmlDir(Config::htmlOutputDir);
  if (!htmlDir.exists() && !htmlDir.mkdir(Config::htmlOutputDir))
  {
    err("Could not create output directory %s\n",Config::htmlOutputDir.data());
    exit(1);
  }
  
  if (Config::latexOutputDir.isEmpty() && Config::generateLatex)
  {
    Config::latexOutputDir=Config::outputDir+"/latex";
  }
  else if (Config::latexOutputDir && Config::latexOutputDir[0]!='/')
  {
    Config::latexOutputDir.prepend(Config::outputDir+'/');
  }
  QDir latexDir(Config::latexOutputDir);
  if (!latexDir.exists() && !latexDir.mkdir(Config::latexOutputDir))
  {
    err("Could not create output directory %s\n",Config::latexOutputDir.data());
    exit(1);
  }
  
  if (Config::manOutputDir.isEmpty() && Config::generateMan)
  {
    Config::manOutputDir=Config::outputDir+"/man";
  }
  else if (Config::manOutputDir && Config::manOutputDir[0]!='/')
  {
    Config::manOutputDir.prepend(Config::outputDir+'/');
  }
  QDir manDir(Config::manOutputDir);
  if (!manDir.exists() && !manDir.mkdir(Config::manOutputDir))
  {
    err("Could not create output directory %s\n",Config::manOutputDir.data());
    exit(1);
  }
  
  // Test to see if HTML header is valid
  if (Config::headerFile.length()>0)
  {
    QFileInfo fi(Config::headerFile);
    if (!fi.exists())
    {
      err("Error: tag HTML_HEADER: header file `%s' "
	  "does not exist\n",Config::headerFile.data());
      exit(1);
    }
  }
  // Test to see if HTML footer is valid
  if (Config::footerFile.length()>0)
  {
    QFileInfo fi(Config::footerFile);
    if (!fi.exists())
    {
      err("Error: tag HTML_FOOTER: footer file `%s' "
	  "does not exist\n",Config::footerFile.data());
      exit(1);
    }
  }
  // Test to see if LaTeX header is valid
  if (Config::latexHeaderFile.length()>0)
  {
    QFileInfo fi(Config::latexHeaderFile);
    if (!fi.exists())
    {
      err("Error: tag LATEX_HEADER: header file `%s' "
	  "does not exist\n",Config::latexHeaderFile.data());
      exit(1);
    }
  }
  // check include path
  char *s=Config::includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    if (!fi.exists()) err("Warning: tag INCLUDE_PATH: include path `%s' "
	                  "does not exist\n",s);
#ifndef DOXYWIZARD
    addSearchDir(fi.absFilePath());
#endif
    s=Config::includePath.next();
  }
  // check input
  if (Config::inputSources.count()==0)
  {
    err("Error: tag INPUT: no input files specified after the INPUT tag.\n");
    exit(1);
  }
  else
  {
    s=Config::inputSources.first();
    while (s)
    {
      QFileInfo fi(s);
      if (!fi.exists())
      {
	err("Error: tag INPUT: input source `%s' does not exist\n",s);
	exit(1);
      }
      s=Config::inputSources.next();
    }
  }

  // add default pattern if needed
  if (Config::filePatternList.count()==0)
  {
    Config::filePatternList.append("*");
  }

  // add default pattern if needed
  if (Config::examplePatternList.count()==0)
  {
    Config::examplePatternList.append("*");
  }

  // add default pattern if needed
  if (Config::imagePatternList.count()==0)
  {
    Config::imagePatternList.append("*");
  }
  
  // more checks needed if and only if the search engine is enabled.
  if (Config::searchEngineFlag)
  {
    // check cgi name
    if (Config::cgiName.length()==0)
    {
      err("Error: tag CGI_NAME: no cgi script name after the CGI_NAME tag.\n");
      exit(1);
    }
    // check cgi URL
    if (Config::cgiURL.length()==0)
    {
      err("Error: tag CGI_URL: no URL to cgi directory specified.\n");
      exit(1);
    }
    else if (Config::cgiURL.left(7)!="http://")
    {
      err("Error: tag CGI_URL: URL to cgi directory is invalid (must "
	  "start with http://).\n");
      exit(1);
    }
    // check documentation URL
    if (Config::docURL.length()==0)
    {
      Config::docURL = Config::outputDir.copy().prepend("file://").append("html");
    }
    else if (Config::docURL.left(7)!="http://" && Config::docURL.left(7)!="file://")
    {
      err("Error: tag DOC_URL: URL to documentation is invalid or "
	  "not absolute.\n"); 
      exit(1);
    }
    // check absolute documentation path
    if (Config::docAbsPath.length()==0)
    {
      Config::docAbsPath = Config::outputDir+"/html"; 
    }
    else if (Config::docAbsPath[0]!='/' && Config::docAbsPath[1]!=':')
    {
      err("Error: tag DOC_ABSPATH: path is not absolute!\n");
      exit(1);
    }
    // check path to doxysearch
    if (Config::binAbsPath.length()==0)
    {
      err("Error: tag BIN_ABSPATH: no absolute path to doxysearch "
	  "specified.\n");
      exit(1);
    }
    else if (Config::binAbsPath[0]!='/' && Config::binAbsPath[1]!=':')
    {
      err("Error: tag BIN_ABSPATH: path is not absolute!\n");
      exit(1);
    }

    // check perl path
    bool found=FALSE;
    if (Config::perlPath.length()==0)
    {
      QFileInfo fi;
      fi.setFile("/usr/bin/perl");
      if (fi.exists()) 
      {
	Config::perlPath="/usr/bin/perl";
        found=TRUE;
      }
      else
      {
	fi.setFile("/usr/local/bin/perl");
	if (fi.exists())
        {
  	  Config::perlPath="/usr/local/bin/perl";
          found=TRUE;
        }
      }
    }
    if (!found)
    {
      QFileInfo fi(Config::perlPath);
      if (!fi.exists())
      {
        warn("Warning: tag PERL_PATH: perl interpreter not found at default or"
            "user specified (%s) location\n",
        Config::perlPath.data());
      }
    }
  }
}

void parseConfig(const QCString &s)
{
  inputString   = s;
  inputPosition = 0;
  yyLineNr      = 1;
  configYYrestart( configYYin );
  BEGIN( Start );
  configYYlex();
}

//extern "C" { // some bogus code to keep the compiler happy
//  int  configYYwrap() { return 1 ; }
//}
