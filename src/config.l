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

#include <qstring.h>  
#include <qfileinf.h>
#include <qdir.h>
#include <qtstream.h>
  
#include "doxygen.h"
#include "config.h"
#include "message.h"
#include "pre.h"
#include "version.h"
#include "language.h"

#define YY_NEVER_INTERACTIVE 1
  
/* -----------------------------------------------------------------
 *
 *	exported variables
 */
  
QString  projectName;          
QString  projectNumber;          
QString  outputDir;          
QString  htmlOutputDir;          
QString  latexOutputDir;          
QString  manOutputDir;          
QString  headerFile;          
QString  footerFile;          
QString  cgiName;          
QString  cgiURL;          
QString  docURL;          
QString  binAbsPath;          
QString  docAbsPath;          
QString  perlPath;          
QString  genTagFile;
QString  inputFilter;
QString  paperType;
QString  outputLanguage;
QString  stripFromPath;
QString  manExtension;
QStrList includePath;                
QStrList examplePath;
QStrList inputSources;               
QStrList excludeSources;
QStrList filePatternList;            
QStrList excludePatternList;
QStrList tagFileList;                
QStrList extDocPathList;             
QStrList predefined;
QStrList extraPackageList;
bool     quietFlag           = FALSE; 
bool     warningFlag         = FALSE; 
bool     recursiveFlag       = FALSE; 
bool     allExtFlag          = FALSE; 
bool     searchEngineFlag    = FALSE; 
bool     extractAllFlag      = FALSE; 
bool     extractPrivateFlag  = FALSE; 
bool     noIndexFlag         = FALSE;
bool     hideMemberFlag      = FALSE;
bool     hideClassFlag       = FALSE;
bool     macroExpansionFlag  = FALSE;
bool     onlyPredefinedFlag  = FALSE;
bool     fullPathNameFlag    = FALSE;
bool     compactLatexFlag    = FALSE;
bool     internalDocsFlag    = FALSE;
bool     caseSensitiveNames  = FALSE;
bool     generateHtml        = TRUE;
bool     generateLatex       = TRUE;
bool     generateMan         = TRUE;
bool     preprocessingFlag   = TRUE;
bool     briefMemDescFlag    = TRUE;
bool     searchIncludeFlag   = TRUE;
bool     classDiagramFlag    = TRUE;
bool     repeatBriefFlag     = TRUE;
bool     verbatimHeaderFlag  = TRUE;
bool     htmlAlignMemberFlag = TRUE;
  
/* -----------------------------------------------------------------
 *
 *	static variables
 */
  
static const char * inputString;
static int	    inputPosition;
static int          yyLineNr;
static QString      tmpString;
static QString *    s=0;
static bool    *    b=0;
static QStrList *   l=0;
static int          lastState;
static int          lastEnvState;
static QString      elemStr;

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
<Start>"PROJECT_NAME"[ \t]*"="		{ BEGIN(GetString);  s=&projectName;      }
<Start>"PROJECT_NUMBER"[ \t]*"="	{ BEGIN(GetString);  s=&projectNumber;    }
<Start>"OUTPUT_DIRECTORY"[ \t]*"="	{ BEGIN(GetString);  s=&outputDir;        }
<Start>"HTML_OUTPUT"[ \t]*"="		{ BEGIN(GetString);  s=&htmlOutputDir;    }
<Start>"MAN_OUTPUT"[ \t]*"="		{ BEGIN(GetString);  s=&manOutputDir;     }
<Start>"LATEX_OUTPUT"[ \t]*"="		{ BEGIN(GetString);  s=&latexOutputDir;   }
<Start>"HTML_HEADER"[ \t]*"="	        { BEGIN(GetString);  s=&headerFile;       }
<Start>"HTML_FOOTER"[ \t]*"="	        { BEGIN(GetString);  s=&footerFile;       }
<Start>"CGI_NAME"[ \t]*"="              { BEGIN(GetString);  s=&cgiName;          }
<Start>"CGI_URL"[ \t]*"="               { BEGIN(GetString);  s=&cgiURL;           }
<Start>"DOC_URL"[ \t]*"="		{ BEGIN(GetString);  s=&docURL;           }
<Start>"BIN_ABSPATH"[ \t]*"="		{ BEGIN(GetString);  s=&binAbsPath;       }
<Start>"DOC_ABSPATH"[ \t]*"="		{ BEGIN(GetString);  s=&docAbsPath;       }
<Start>"PERL_PATH"[ \t]*"="		{ BEGIN(GetString);  s=&perlPath;         }
<Start>"GENERATE_TAGFILE"[ \t]*"="	{ BEGIN(GetString);  s=&genTagFile;       }
<Start>"INPUT_FILTER"[ \t]*"="		{ BEGIN(GetString);  s=&inputFilter;      }
<Start>"PAPER_TYPE"[ \t]*"="		{ BEGIN(GetString);  s=&paperType;        }
<Start>"OUTPUT_LANGUAGE"[ \t]*"="	{ BEGIN(GetString);  s=&outputLanguage;   }
<Start>"STRIP_FROM_PATH"[ \t]*"="	{ BEGIN(GetString);  s=&stripFromPath;    }
<Start>"MAN_EXTENSION"[ \t]*"="		{ BEGIN(GetString);  s=&manExtension;     }
<Start>"INCLUDE_PATH"[ \t]*"="		{ BEGIN(GetStrList); l=&includePath;        elemStr=""; }
<Start>"EXAMPLE_PATH"[ \t]*"="	        { BEGIN(GetStrList); l=&examplePath;        elemStr=""; }
<Start>"INPUT"[ \t]*"="			{ BEGIN(GetStrList); l=&inputSources;       elemStr=""; }
<Start>"EXCLUDE"[ \t]*"="		{ BEGIN(GetStrList); l=&excludeSources;     elemStr=""; }
<Start>"FILE_PATTERNS"[ \t]*"="		{ BEGIN(GetStrList); l=&filePatternList;    elemStr=""; }
<Start>"EXCLUDE_PATTERNS"[ \t]*"="	{ BEGIN(GetStrList); l=&excludePatternList; elemStr=""; }
<Start>"TAGFILES"[ \t]*"="		{ BEGIN(GetStrList); l=&tagFileList;        elemStr=""; }
<Start>"EXT_DOC_PATHS"[ \t]*"="		{ BEGIN(GetStrList); l=&extDocPathList;     elemStr=""; }
<Start>"PREDEFINED"[ \t]*"="            { BEGIN(GetStrList); l=&predefined;         elemStr=""; }
<Start>"EXTRA_PACKAGES"[ \t]*"="	{ BEGIN(GetStrList); l=&extraPackageList;   elemStr=""; }
<Start>"QUIET"[ \t]*"="			{ BEGIN(GetBool);    b=&quietFlag;        }
<Start>"WARNINGS"[ \t]*"="              { BEGIN(GetBool);    b=&warningFlag;      }
<Start>"RECURSIVE"[ \t]*"="		{ BEGIN(GetBool);    b=&recursiveFlag;    }
<Start>"ALLEXTERNALS"[ \t]*"="		{ BEGIN(GetBool);    b=&allExtFlag;       }
<Start>"SEARCHENGINE"[ \t]*"="          { BEGIN(GetBool);    b=&searchEngineFlag; }
<Start>"EXTRACT_ALL"[ \t]*"="           { BEGIN(GetBool);    b=&extractAllFlag;   }
<Start>"EXTRACT_PRIVATE"[ \t]*"="       { BEGIN(GetBool);    b=&extractPrivateFlag;   }
<Start>"DISABLE_INDEX"[ \t]*"="	        { BEGIN(GetBool);    b=&noIndexFlag; }
<Start>"GENERATE_LATEX"[ \t]*"="	{ BEGIN(GetBool);    b=&generateLatex; }
<Start>"GENERATE_HTML"[ \t]*"="		{ BEGIN(GetBool);    b=&generateHtml; }
<Start>"GENERATE_MAN"[ \t]*"="		{ BEGIN(GetBool);    b=&generateMan; }
<Start>"ENABLE_PREPROCESSING"[ \t]*"="  { BEGIN(GetBool);    b=&preprocessingFlag; }
<Start>"MACRO_EXPANSION"[ \t]*"="	{ BEGIN(GetBool);    b=&macroExpansionFlag; }
<Start>"SEARCH_INCLUDES"[ \t]*"="	{ BEGIN(GetBool);    b=&searchIncludeFlag; }
<Start>"BRIEF_MEMBER_DESC"[ \t]*"="     { BEGIN(GetBool);    b=&briefMemDescFlag; }
<Start>"HIDE_UNDOC_MEMBERS"[ \t]*"="    { BEGIN(GetBool);    b=&hideMemberFlag; }
<Start>"HIDE_UNDOC_CLASSES"[ \t]*"="    { BEGIN(GetBool);    b=&hideClassFlag; }
<Start>"EXPAND_ONLY_PREDEF"[ \t]*"="    { BEGIN(GetBool);    b=&onlyPredefinedFlag; }
<Start>"FULL_PATH_NAMES"[ \t]*"="	{ BEGIN(GetBool);    b=&fullPathNameFlag; }
<Start>"CLASS_DIAGRAMS"[ \t]*"="	{ BEGIN(GetBool);    b=&classDiagramFlag; }
<Start>"COMPACT_LATEX"[ \t]*"="		{ BEGIN(GetBool);    b=&compactLatexFlag; }
<Start>"REPEAT_BRIEF"[ \t]*"="		{ BEGIN(GetBool);    b=&repeatBriefFlag; }
<Start>"INTERNAL_DOCS"[ \t]*"="		{ BEGIN(GetBool);    b=&internalDocsFlag; }
<Start>"CASE_SENSE_NAMES"[ \t]*"="      { BEGIN(GetBool);    b=&caseSensitiveNames; }
<Start>"VERBATIM_HEADERS"[ \t]*"="      { BEGIN(GetBool);    b=&verbatimHeaderFlag; }
<Start>"HTML_ALIGN_MEMBERS"[ \t]*"="    { BEGIN(GetBool);    b=&htmlAlignMemberFlag; }
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
  					  QString bs=yytext; 
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
  /*
<GetStrList>[a-z_A-Z.*:?~0-9/\\\-]+	{ 
  					  elemStr+=yytext;
  					}
  */
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
  printf("projectName=`%s'\n",projectName.data());
  printf("outputDir=`%s'\n",  outputDir.data());
  printf("headerFile=`%s'\n", headerFile.data());
  printf("footerFile=`%s'\n", footerFile.data());
  char *ip=includePath.first();
  while (ip)
  {
    printf("includePath=`%s'\n",ip);
    ip=includePath.next();
  }
  printf("quiet=`%d'\n",      quietFlag);
  printf("warnings=`%d'\n",   warningFlag);
  char *is=inputSources.first();
  while (is)
  {
    printf("inputSources=`%s'\n",is);
    is=inputSources.next();
  }
  char *fp=filePatternList.first();
  while (fp)
  {
    printf("filePattern=`%s'\n",fp);
    fp=filePatternList.next();
  }
  printf("recusive=`%d'\n",recursiveFlag);
  printf("inputFilter=`%s'\n",inputFilter.data());
  char *tf=tagFileList.first();
  while (tf)
  {
    printf("tagFile=`%s'\n",tf);
    tf=tagFileList.next();
  }
  printf("allExternals=`%d'\n",allExtFlag);
  printf("searchEngine=`%d'\n",searchEngineFlag);
  printf("cgiName=`%s'\n",cgiName.data());
  printf("cgiURL=`%s'\n",cgiURL.data());
  printf("docURL=`%s'\n",docURL.data());
  printf("binAbsPath=`%s'\n",binAbsPath.data());
  char *ed=extDocPathList.first();
  while (ed)
  {
    printf("binAbsPathFile=`%s'\n",ed);
    ed=extDocPathList.next();
  }
}

void writeTemplateConfig(QFile *f,bool sl)
{
  QTextStream t(f);
  t << "# Doxyfile " << versionString << endl;
  if (!sl)
  {
    t << "# This file describes the settings to be used by doxygen for a project\n";
    t << "#\n";
    t << "# All text after a hash (#) is considered a comment and will be ignored\n";
    t << "# The format is:\n";
    t << "#       TAG = value [value, ...]\n";
    t << "# Values that contain spaces should be placed between quotes (\" \")\n";
    t << "\n";
    t << "#---------------------------------------------------------------------------\n";
    t << "# General configuration options\n";
    t << "#---------------------------------------------------------------------------\n";
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
    t << "# The default language is English, other supported languages are: Dutch\n";
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
    t << "# If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend\n";
    t << "# the brief description of a member or function before the detailed description.\n";
    t << "# Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the \n";
    t << "# brief descriptions will be completely suppressed.\n";
    t << "\n";
  }
  t << "REPEAT_BRIEF         = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "# If the FULL_PATH_NAMES tag is set to YES Doxygen will prepend the full\n";
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
    t << "# only done if the specified string matches the left-hand part of the path.\n";
    t << "\n";
  }
  t << "STRIP_FROM_PATH      =\n";
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
    t << "# If the VERBATIM_HEADERS tag is set the YES (the default) then Doxygen\n";
    t << "# will generate a verbatim copy of the header file for each class for\n";
    t << "# which an include is specified. Set to NO to disable this.\n";
    t << "\n";
  }
  t << "VERBATIM_HEADERS     = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "#---------------------------------------------------------------------------\n";
    t << "# configuration options related to the input files\n";
    t << "#---------------------------------------------------------------------------\n";
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
    t << "#---------------------------------------------------------------------------\n";
    t << "# configuration options related to the HTML output\n";
    t << "#---------------------------------------------------------------------------\n";
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
    t << "# If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes,\n";
    t << "# files or namespaces will be aligned in HTML using tables. If set to\n";
    t << "# NO a bullet list will be used.\n";
    t << "\n";
  }
  t << "HTML_ALIGN_MEMBERS   = YES\n";
  if (!sl)
  {
    t << "\n";
    t << "#---------------------------------------------------------------------------\n";
    t << "# configuration options related to the LaTeX output\n";
    t << "#---------------------------------------------------------------------------\n";
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
    t << "#---------------------------------------------------------------------------\n";
    t << "# configuration options related to the man page output\n";
    t << "#---------------------------------------------------------------------------\n";
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
    t << "#---------------------------------------------------------------------------\n";
    t << "# Configuration options related to the preprocessor \n";
    t << "#---------------------------------------------------------------------------\n";
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
    t << "#---------------------------------------------------------------------------\n";
    t << "# Configuration options related to external references \n";
    t << "#---------------------------------------------------------------------------\n";
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
  t << "PERL_PATH            = /usr/local/bin/perl\n";
  if (!sl)
  {
    t << "\n";
    t << "#---------------------------------------------------------------------------\n";
    t << "# Configuration options related to the search engine \n";
    t << "#---------------------------------------------------------------------------\n";
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
  
  // set default man page extension if non is given by the user
  if (manExtension.isEmpty())
  {
    manExtension=".3";
  }
  
  paperType = paperType.lower().stripWhiteSpace(); 
  if (paperType.isEmpty())
  {
    paperType = "a4wide";
  }
  if (paperType!="a4" && paperType!="a4wide" && paperType!="letter" && 
      paperType!="legal" && paperType!="executive")
  {
    err("Error: Unknown page type specified");
  }
  
  outputLanguage=outputLanguage.stripWhiteSpace();
  if (outputLanguage.isEmpty())
  {
    setTranslator("English");
  }
  else
  {
    if (!setTranslator(outputLanguage))
    {
      err("Error: Output language %s not supported! Using english instead.\n",
	  outputLanguage.data());
    }
  }
  
  // Test to see if output directory is valid
  if (outputDir.isEmpty()) 
    outputDir=QDir::currentDirPath();
  else
  {
    QDir dir(outputDir);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDir))
      {
        err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
	    "exist and cannot be created\n",outputDir.data());
        exit(1);
      }
      else if (!quietFlag)
      {
	err("Notice: Output directory `%s' does not exist. "
	    "I have created it for you.\n", outputDir.data());
      }
      dir.cd(outputDir);
    }
    outputDir=dir.absPath();
  }

  if (htmlOutputDir.isEmpty() && generateHtml)
  {
    htmlOutputDir=outputDir+"/html";
  }
  else if (htmlOutputDir && htmlOutputDir[0]!='/')
  {
    htmlOutputDir.prepend(outputDir+'/');
  }
  QDir htmlDir(htmlOutputDir);
  if (!htmlDir.exists() && !htmlDir.mkdir(htmlOutputDir))
  {
    err("Could not create output directory %s\n",htmlOutputDir.data());
    exit(1);
  }
  
  if (latexOutputDir.isEmpty() && generateLatex)
  {
    latexOutputDir=outputDir+"/latex";
  }
  else if (latexOutputDir && latexOutputDir[0]!='/')
  {
    latexOutputDir.prepend(outputDir+'/');
  }
  QDir latexDir(latexOutputDir);
  if (!latexDir.exists() && !latexDir.mkdir(latexOutputDir))
  {
    err("Could not create output directory %s\n",latexOutputDir.data());
    exit(1);
  }
  
  if (manOutputDir.isEmpty() && generateMan)
  {
    manOutputDir=outputDir+"/man";
  }
  else if (manOutputDir && manOutputDir[0]!='/')
  {
    manOutputDir.prepend(outputDir+'/');
  }
  QDir manDir(manOutputDir);
  if (!manDir.exists() && !manDir.mkdir(manOutputDir))
  {
    err("Could not create output directory %s\n",manOutputDir.data());
    exit(1);
  }
  
  // Test to see if HTML header is valid
  if (headerFile.length()>0)
  {
    QFileInfo fi(headerFile);
    if (!fi.exists())
    {
      err("Error: tag HTML_HEADER: header file `%s' "
	  "does not exist\n",headerFile.data());
      exit(1);
    }
  }
  // Test to see if HTML footer is valid
  if (footerFile.length()>0)
  {
    QFileInfo fi(footerFile);
    if (!fi.exists())
    {
      err("Error: tag HTML_FOOTER: footer file `%s' "
	  "does not exist\n",footerFile.data());
      exit(1);
    }
  }
  // check include path
  char *s=includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    if (!fi.exists()) err("Warning: tag INCLUDE_PATH: include path `%s' "
	                  "does not exist\n",s);
    addSearchDir(fi.absFilePath());
    s=includePath.next();
  }
  // check input
  if (inputSources.count()==0)
  {
    err("Error: tag INPUT: no input files specified after the INPUT tag.\n");
    exit(1);
  }
  else
  {
    s=inputSources.first();
    while (s)
    {
      QFileInfo fi(s);
      if (!fi.exists())
      {
	err("Error: tag INPUT: input source `%s' does not exist\n",s);
	exit(1);
      }
      s=inputSources.next();
    }
  }
  // add default pattern if needed
  if (filePatternList.count()==0)
  {
    filePatternList.append("*");
  }
  
  // more checks needed if and only if the search engine is enabled.
  if (searchEngineFlag)
  {
    // check cgi name
    if (cgiName.length()==0)
    {
      err("Error: tag CGI_NAME: no cgi script name after the CGI_NAME tag.\n");
      exit(1);
    }
    // check cgi URL
    if (cgiURL.length()==0)
    {
      err("Error: tag CGI_URL: no URL to cgi directory specified.\n");
      exit(1);
    }
    else if (cgiURL.left(7)!="http://")
    {
      err("Error: tag CGI_URL: URL to cgi directory is invalid (must "
	  "start with http://).\n");
      exit(1);
    }
    // check documentation URL
    if (docURL.length()==0)
    {
      docURL = outputDir.copy().prepend("file://").append("html");
    }
    else if (docURL.left(7)!="http://" && docURL.left(7)!="file://")
    {
      err("Error: tag DOC_URL: URL to documentation is invalid or "
	  "not absolute.\n"); 
      exit(1);
    }
    // check absolute documentation path
    if (docAbsPath.length()==0)
    {
      docAbsPath = outputDir+"/html"; 
    }
    else if (docAbsPath && docAbsPath[0]!='/')
    {
      err("Error: tag DOC_ABSPATH: path is not absolute\n");
      exit(1);
    }
    // check path to doxysearch
    if (binAbsPath.length()==0)
    {
      err("Error: tag BIN_ABSPATH: no absolute path to doxysearch "
	  "specified.\n");
      exit(1);
    }
    else
    {
      if (binAbsPath && binAbsPath[0]!='/')
      {
	err("Error: tag BIN_ABSPATH: path is not absolute "
	    "(must start with /).\n");
	exit(1);
      }
    }
    // check perl path
    if (perlPath.length()==0)
    {
      perlPath="/usr/local/bin/perl";
    }
    QFileInfo fi(perlPath);
    if (!fi.exists())
    {
      err("Error: tag PERL_PATH: perl interpreter not found at `%s'\n",
	  perlPath.data());
      exit(1);
    }
  }
}

void parseConfig(const QString &s)
{
  inputString   = s;
  inputPosition = 0;
  yyLineNr      = 1;
  configYYrestart( configYYin );
  BEGIN( Start );
  configYYlex();
  checkConfig();
}

extern "C" { // some bogus code to keep the compiler happy
  int  configYYwrap() { return 1 ; }
}
