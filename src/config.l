/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

%{

/*
 *	includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <assert.h>
#include <ctype.h>

#include <qfileinfo.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qstack.h>
  
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
void warn_cont(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}
void initWarningFormat()
{
}
#else
#include "doxygen.h"
#include "message.h"
#include "pre.h"
#include "version.h"
#include "language.h"
#endif

#define MAX_INCLUDE_DEPTH 10
#define YY_NEVER_INTERACTIVE 1
#define YY_NO_UNPUT
  
Config *Config::m_instance = 0;

void ConfigInt::convertStrToVal() 
{
  if (!m_valueString.isEmpty())
  {
    bool ok;
    int val = m_valueString.toInt(&ok);
    if (!ok || val<m_minVal || val>m_maxVal)
    {
      warn_cont("Warning: argument of %s is not a valid number in the range [%d..%d]!\n"
                "Using the default of %d!\n",m_name.data(),m_minVal,m_maxVal,m_value);
    }
    m_value=val;
  }
}

QCString &Config::getString(const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    err("ConfigOption::getString: Requested unknown option %s!\n",name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_String)
  {
    err("ConfigOption::getString: Requested option %s not of string type!\n",name);
    exit(1);
  }
  return *((ConfigString *)opt)->valueRef();
}

QStrList &Config::getList(const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    err("ConfigOption::getList: Requested unknown option %s!\n",name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_List)
  {
    err("ConfigOption::getList: Requested option %s not of list type!\n",name);
    exit(1);
  }
  return *((ConfigList *)opt)->valueRef();
}

QCString &Config::getEnum(const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    err("ConfigOption::getEnum: Requested unknown option %s!\n",name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Enum)
  {
    err("ConfigOption::getEnum: Requested option %s not of enum type!\n",name);
    exit(1);
  }
  return *((ConfigEnum *)opt)->valueRef();
}

int &Config::getInt(const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    err("ConfigOption::getInt: Requested unknown option %s!\n",name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Int)
  {
    err("ConfigOption::getInt: Requested option %s not of integer type!\n",name);
    exit(1);
  }
  return *((ConfigInt *)opt)->valueRef();
}

bool &Config::getBool(const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    err("ConfigOption::getInt: Requested unknown option %s!\n",name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Bool)
  {
    err("ConfigOption::getInt: Requested option %s not of integer type!\n",name);
    exit(1);
  }
  return *((ConfigBool *)opt)->valueRef();
}

/* -----------------------------------------------------------------
 *
 *	static variables
 */

struct ConfigFileState
{
  int lineNr;
  FILE *filePtr;
  YY_BUFFER_STATE oldState;
  YY_BUFFER_STATE newState;
  QCString fileName;
};  

static const char       *inputString;
static int	         inputPosition;
static int               yyLineNr;
static QCString          yyFileName;
static QCString          tmpString;
static QCString         *s=0;
static bool             *b=0;
static QStrList         *l=0;
static int               lastState;
static QCString          elemStr;
static QCString          includeName;
static QStrList          includePathList;
static QStack<ConfigFileState> includeStack;  
static int               includeDepth;

static QCString     tabSizeString;
static QCString     maxInitLinesString;
static QCString     colsInAlphaIndexString;
static QCString     enumValuesPerLineString;
static QCString     treeViewWidthString;
static QCString     maxDotGraphWidthString;
static QCString     maxDotGraphHeightString;

static Config      *config;

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
    // no file included
    if (includeStack.isEmpty()) 
    {
        int c=0;
	while( c < max_size && inputString[inputPosition] )
	{
	      *buf = inputString[inputPosition++] ;
	      c++; buf++;
  	}
	return c;
    } 
    else 
    {
        //assert(includeStack.current()->newState==YY_CURRENT_BUFFER);
	return fread(buf,1,max_size,includeStack.current()->filePtr);
    }
}


static FILE *tryPath(const char *path,const char *fileName)
{
  QCString absName=(QCString)path+"/"+fileName;
  QFileInfo fi(absName);
  if (fi.exists() && fi.isFile())
  {
    FILE *f=fopen(absName,"r");
    if (!f) err("Error: could not open file %s for reading\n",absName.data());
    return f;
  }
  return 0;
}

static void substEnvVarsInStrList(QStrList &sl);
static void substEnvVarsInString(QCString &s);

static FILE *findFile(const char *fileName)
{
  substEnvVarsInStrList(includePathList);
  char *s=includePathList.first();
  while (s) // try each of the include paths
  {
    FILE *f = tryPath(s,fileName);
    if (f) return f;
    s=includePathList.next();
  } 
  // try cwd if includePathList fails
  return tryPath(".",fileName);
}

static void readIncludeFile(const char *incName)
{
  if (includeDepth==MAX_INCLUDE_DEPTH) {
    err("Error: maximum include depth (%d) reached, %s is not included. Aborting...\n",
	MAX_INCLUDE_DEPTH,incName);
    exit(1);
  } 

  QCString inc = incName;
  substEnvVarsInString(inc);
  inc = inc.stripWhiteSpace();
  uint incLen = inc.length();
  if (inc.at(0)=='"' && inc.at(incLen-1)=='"') // strip quotes
  {
    inc=inc.mid(1,incLen-2);
  }

  FILE *f;

  //printf("Searching for `%s'\n",incFileName.data());
  if ((f=findFile(inc))) // see if the include file can be found
  {
    // For debugging
#if SHOW_INCLUDES
    for (i=0;i<includeStack.count();i++) msg("  ");
    msg("@INCLUDE = %s: parsing...\n",inc.data());
#endif

    // store the state of the old file 
    ConfigFileState *fs=new ConfigFileState;
    fs->oldState=YY_CURRENT_BUFFER;
    fs->lineNr=yyLineNr;
    fs->fileName=yyFileName;
    fs->filePtr=f;
    // push the state on the stack
    includeStack.push(fs);
    // set the scanner to the include file
    yy_switch_to_buffer(yy_create_buffer(f, YY_BUF_SIZE));
    fs->newState=YY_CURRENT_BUFFER;
    yyFileName=inc;
    includeDepth++;
  } 
  else
  {
    err("Error: @INCLUDE = %s: not found!\n",inc.data());
    exit(1);
  }
}


%}

%option noyywrap

%x      Start
%x	SkipComment
%x      SkipInvalid
%x      GetString
%x      GetBool
%x      GetStrList
%x      GetQuotedString
%x      GetEnvVar
%x      Include

%%

<*>\0x0d
<Start,GetString,GetStrList,GetBool,SkipInvalid>"#"	 { BEGIN(SkipComment); }
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"="	 { QCString cmd=yytext;
                                           cmd=cmd.left(cmd.length()-1).stripWhiteSpace(); 
					   ConfigOption *option = config->get(cmd);
					   if (option==0) // oops not known
					   {
					     err("Warning: ignoring unsupported tag `%s' at line %d, file %s\n",
						 yytext,yyLineNr,yyFileName.data()); 
					     BEGIN(SkipInvalid);
					   }
					   else // known tag
					   {
					     switch(option->kind())
					     {
					       case ConfigOption::O_Info:
						 // shouldn't get here!
					         BEGIN(SkipInvalid);
						 break;
					       case ConfigOption::O_List:
						 l = ((ConfigList *)option)->valueRef();
					         l->clear();
						 elemStr="";
					         BEGIN(GetStrList);
					         break;
					       case ConfigOption::O_Enum:
						 s = ((ConfigEnum *)option)->valueRef();
					         s->resize(0);
					         BEGIN(GetString);
					         break;
					       case ConfigOption::O_String:
						 s = ((ConfigString *)option)->valueRef();
					         s->resize(0);
					         BEGIN(GetString);
					         break;
					       case ConfigOption::O_Int:
						 s = ((ConfigInt *)option)->valueStringRef();
					         s->resize(0);
					         BEGIN(GetString);
					         break;
					       case ConfigOption::O_Bool:
						 b = ((ConfigBool *)option)->valueRef();
					         BEGIN(GetBool);
						 break;
					     }
					   }
					 }
<Start>"@INCLUDE_PATH"[ \t]*"=" 	{ BEGIN(GetStrList); l=&includePathList; l->clear(); elemStr=""; }
  /* include a config file */
<Start>"@INCLUDE"[ \t]*"="     		{ BEGIN(Include);}
<Include>([^ \"\t\r\n]+)|("\""[^\n\"]+"\"") { 
  					  readIncludeFile(yytext); 
  					  BEGIN(Start);
					}
<<EOF>>					{
                                          //printf("End of include file\n");
					  //printf("Include stack depth=%d\n",g_includeStack.count());
                                          if (includeStack.isEmpty())
					  {
					    //printf("Terminating scanner!\n");
					    yyterminate();
					  }
					  else
					  {
					    ConfigFileState *fs=includeStack.pop();
					    fclose(fs->filePtr);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->oldState );
					    yy_delete_buffer( oldBuf );
					    yyLineNr=fs->lineNr;
					    yyFileName=fs->fileName;
					    delete fs; fs=0;
                                            includeDepth--;
					  }
  					}

<Start>[a-z_A-Z0-9]+			{ err("Warning: ignoring unknown tag `%s' at line %d, file %s\n",yytext,yyLineNr,yyFileName.data()); }
<GetString,GetBool,SkipInvalid>\n	{ yyLineNr++; BEGIN(Start); }
<GetStrList>\n				{ 
  					  yyLineNr++; 
					  if (!elemStr.isEmpty())
					  {
					    //printf("elemStr1=`%s'\n",elemStr.data());
					    l->append(elemStr);
					  }
					  BEGIN(Start); 
					}
<GetStrList>[ \t]+			{
  				          if (!elemStr.isEmpty())
					  {
					    //printf("elemStr2=`%s'\n",elemStr.data());
  					    l->append(elemStr);
					  }
					  elemStr.resize(0);
  					}
<GetString>[^ \"\t\r\n]+		{ (*s)+=yytext; }
<GetString,GetStrList,SkipInvalid>"\""	{ lastState=YY_START;
  					  BEGIN(GetQuotedString); 
                                          tmpString.resize(0); 
					}
<GetQuotedString>"\""|"\n" 		{ 
  					  //printf("Quoted String = `%s'\n",tmpString.data());
  					  if (lastState==GetString)
					    (*s)+=tmpString;
					  else
					    elemStr+=tmpString;
					  if (*yytext=='\n')
					  {
					    err("Warning: Missing end quote (\") on line %d, file %s\n",yyLineNr,yyFileName.data());
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
  					  if (bs=="YES" || bs=="1")
					    *b=TRUE;
					  else if (bs=="NO" || bs=="0")
					    *b=FALSE;
					  else 
					  {
					    *b=FALSE; 
					    warn_cont("Warning: Invalid value `%s' for "
						 "boolean tag in line %d, file %s; use YES or NO\n",
						 bs.data(),yyLineNr,yyFileName.data());
					  }
					}
<GetStrList>[^ \#\"\t\r\n]+		{
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

#if 0
static void writeBoolValue(QTextStream &t,bool v)
{
  if (v) t << "YES"; else t << "NO";
}

static void writeIntValue(QTextStream &t,int i)
{
  t << i;
}

static void writeStringValue(QTextStream &t,QCString &s)
{
  const char *p=s.data();
  char c;
  bool hasBlanks=FALSE;
  if (p)
  {
    while ((c=*p++)!=0 && !hasBlanks) hasBlanks = (c==' ' || c=='\n' || c=='\t');
    if (hasBlanks) 
      t << "\"" << s << "\"";
    else
      t << s;
  }
}

static void writeStringList(QTextStream &t,QStrList &l)
{
  const char *p = l.first();
  bool first=TRUE;
  while (p)
  {
    char c;
    const char *s=p;
    bool hasBlanks=FALSE;
    while ((c=*p++)!=0 && !hasBlanks) hasBlanks = (c==' ' || c=='\n' || c=='\t');
    if (!first) t << "                         ";
    first=FALSE;
    if (hasBlanks) t << "\"" << s << "\""; else t << s;
    p = l.next();
    if (p) t << " \\" << endl;
  }
}
#endif

void Config::writeTemplate(QFile *f,bool sl)
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
    t << "# For lists items can also be appended using:\n";
    t << "#       TAG += value [value, ...]\n";
    t << "# Values that contain spaces should be placed between quotes (\" \")\n";
  }
  ConfigOption *option = m_options->first();
  while (option)
  {
    option->writeTemplate(t,sl);
    option = m_options->next();
  }
}

void Config::convertStrToVal()
{
  ConfigOption *option = m_options->first();
  while (option)
  {
    option->convertStrToVal();
    option = m_options->next();
  }
}

static void substEnvVarsInString(QCString &s)
{
  static QRegExp re("\\$\\([a-z_A-Z0-9]+\\)");
  if (s.isEmpty()) return;
  int p=0;
  int i,l;
  //printf("substEnvVarInString(%s) start\n",s.data());
  while ((i=re.match(s,p,&l))!=-1)
  {
    //printf("Found environment var s.mid(%d,%d)=`%s'\n",i+2,l-3,s.mid(i+2,l-3).data());
    QCString env=getenv(s.mid(i+2,l-3));
    substEnvVarsInString(env); // recursively expand variables if needed.
    s = s.left(i)+env+s.right(s.length()-i-l);
    p=i+env.length(); // next time start at the end of the expanded string
  }
  //printf("substEnvVarInString(%s) end\n",s.data());
}

static void substEnvVarsInStrList(QStrList &sl)
{
  char *s = sl.first();
  while (s)
  {
    QCString result(s);
    bool wasQuoted = (result.find(' ')!=-1) || (result.find('\t')!=-1);
    substEnvVarsInString(result);

    if (!wasQuoted) /* as a result of the expansion, a single string
		       may have expanded into a list, which we'll
		       add to sl. If the orginal string already 
		       contained multiple elements no further 
		       splitting is done to allow quoted items with spaces! */
    {
      int l=result.length();
      int i,p=0;
      // skip spaces
      // search for a "word"
      for (i=0;i<l;i++)
      {
	char c;
	// skip until start of new word
	while (i<l && ((c=result.at(i))==' ' || c=='\t')) i++; 
	p=i; // p marks the start index of the word
	// skip until end of a word
	while (i<l && ((c=result.at(i))!=' ' && c!='\t' && c!='"')) i++;
	if (i<l) // not at the end of the string
	{
	  if (c=='"') // word within quotes
	  {
	    p=i+1;
	    for (i++;i<l;i++)
	    {
	      c=result.at(i);
	      if (c=='"') // end quote
	      {
		// replace the string in the list and go to the next item.
		sl.insert(sl.at(),result.mid(p,i-p)); // insert new item before current item.
		sl.next();                 // current item is now the old item
		p=i+1;
		break; 
	      }
	      else if (c=='\\') // skip escaped stuff
	      {
		i++;
	      }
	    }
	  }
	  else if (c==' ' || c=='\t') // separator
	  {
	    // replace the string in the list and go to the next item.
	    sl.insert(sl.at(),result.mid(p,i-p)); // insert new item before current item.
	    sl.next();                 // current item is now the old item
	    p=i+1;
	  }
	}
      }
      if (p!=l) // add the leftover as a string
      {
	// replace the string in the list and go to the next item.
	sl.insert(sl.at(),result.right(l-p)); // insert new item before current item.
	sl.next();                 // current item is now the old item
      }

      // remove the old unexpanded string from the list
      i=sl.at();
      sl.remove(); // current item index changes if the last element is removed.
      if (sl.at()==i)     // not last item
	s = sl.current();
      else                // just removed last item
	s = 0;
    }
    else // just goto the next element in the list
    {
      s=sl.next();
    }
  }
}

void ConfigString::substEnvVars()
{
  substEnvVarsInString(m_value);
}

void ConfigList::substEnvVars()
{
  substEnvVarsInStrList(m_value);
}


void Config::substituteEnvironmentVars()
{
  ConfigOption *option = m_options->first();
  while (option)
  {
    option->substEnvVars();
    option = m_options->next();
  }
}

void Config::check()
{
  //if (!projectName.isEmpty())
  //{
  //  projectName[0]=toupper(projectName[0]);
  //}

  QCString &warnFormat = getString("WARN_FORMAT");
  if (warnFormat.isEmpty())
  {
    warnFormat="$file:$line $text";
  }
  else
  {
    if (warnFormat.find("$file")==-1)
    {
      err("Error: warning format does not contain a $file tag!\n");
      exit(1);
    }
    if (warnFormat.find("$line")==-1)
    {
      err("Error: warning format does not contain a $line tag!\n");
      exit(1);
    }
    if (warnFormat.find("$text")==-1)
    {
      err("Error: wanring format foes not contain a $text tag!\n");
      exit(1);
    }
  }
  initWarningFormat();

  QCString &manExtension = getString("MAN_EXTENSION");
  // set default man page extension if non is given by the user
  if (manExtension.isEmpty())
  {
    manExtension=".3";
  }
  
  QCString &paperType = getEnum("PAPER_TYPE");
  paperType=paperType.lower().stripWhiteSpace(); 
  if (paperType.isEmpty())
  {
    paperType = "a4wide";
  }
  if (paperType!="a4" && paperType!="a4wide" && paperType!="letter" && 
      paperType!="legal" && paperType!="executive")
  {
    err("Error: Unknown page type specified");
  }
  
  QCString &outputLanguage=getEnum("OUTPUT_LANGUAGE");
  outputLanguage=outputLanguage.stripWhiteSpace();
  if (outputLanguage.isEmpty())
  {
    outputLanguage = "English";
#ifndef DOXYWIZARD
    setTranslator("English");
#endif
  }
  else
  {
#ifndef DOXYWIZARD
    if (!setTranslator(outputLanguage))
    {
      err("Error: Output language %s not supported! Using English instead.\n",
	  outputLanguage.data());
    }
#endif
  }
  
  // expand the relative stripFromPath values
  QStrList &stripFromPath = getList("STRIP_FROM_PATH");
  char *sfp = stripFromPath.first();
  while (sfp)
  {
    QCString path = sfp;
    if (path.at(0)!='/' && (path.length()<=2 || path.at(1)!=':'))
    {
      QFileInfo fi(path);
      if (fi.exists() && fi.isDir())
      {
	int i = stripFromPath.at();
	stripFromPath.remove();
	if (stripFromPath.at()==i) // did not remove last item
	  stripFromPath.insert(i,fi.absFilePath()+"/");
	else
	  stripFromPath.append(fi.absFilePath()+"/");
      }
    }
    sfp = stripFromPath.next();
  }
  
  
  // Test to see if HTML header is valid
  QCString &headerFile = getString("HTML_HEADER");
  if (!headerFile.isEmpty())
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
  QCString &footerFile = getString("HTML_FOOTER");
  if (!footerFile.isEmpty())
  {
    QFileInfo fi(footerFile);
    if (!fi.exists())
    {
      err("Error: tag HTML_FOOTER: footer file `%s' "
	  "does not exist\n",footerFile.data());
      exit(1);
    }
  }
  // Test to see if LaTeX header is valid
  QCString &latexHeaderFile = getString("LATEX_HEADER");
  if (!latexHeaderFile.isEmpty())
  {
    QFileInfo fi(latexHeaderFile);
    if (!fi.exists())
    {
      err("Error: tag LATEX_HEADER: header file `%s' "
	  "does not exist\n",latexHeaderFile.data());
      exit(1);
    }
  }
  // check include path
  QStrList &includePath = getList("INCLUDE_PATH");
  char *s=includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    if (!fi.exists()) err("Warning: tag INCLUDE_PATH: include path `%s' "
	                  "does not exist\n",s);
#ifndef DOXYWIZARD
    addSearchDir(fi.absFilePath());
#endif
    s=includePath.next();
  }

  // check aliases
  QStrList &aliasList = getList("ALIASES");
  s=aliasList.first();
  while (s)
  {
    QRegExp re("[a-z_A-Z][a-z_A-Z0-9]*[ \t]*=");
    QCString alias=s;
    alias=alias.stripWhiteSpace();
    if (alias.find(re)!=0)
    {
      err("Illegal alias format `%s'. Use \"name=value\"\n",alias.data());
    }
    s=aliasList.next();
  }
  
  // check dot path
  QCString &dotPath = getString("DOT_PATH");
  if (!dotPath.isEmpty())
  {
    if (dotPath.find('\\')!=-1)
    {
      if (dotPath.at(dotPath.length()-1)!='\\')
      {
	dotPath+='\\';
      } 
    } 
    else if (dotPath.find('/')!=-1)
    {
      if (dotPath.at(dotPath.length()-1)!='/')
      {
	dotPath+='/';
      } 
    } 
#if defined(_WIN32)
    QFileInfo dp(dotPath+"dot.exe");
#else
    QFileInfo dp(dotPath+"dot");
#endif
    if (!dp.exists() || !dp.isFile())
    {
      err("Warning: the dot tool could not be found at %s\n",dotPath.data());
      dotPath="";
    }
    else
    {
      dotPath=dp.dirPath(TRUE)+"/";
#if defined(_WIN32) // convert slashes
      uint i=0,l=dotPath.length();
      for (i=0;i<l;i++) if (dotPath.at(i)=='/') dotPath.at(i)='\\';
#endif
    }
  }
  else // make sure the string is empty but not null!
  {
    dotPath="";
  }
  
  // check input
  QStrList &inputSources=getList("INPUT");
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
  QStrList &filePatternList = getList("FILE_PATTERNS");
  if (filePatternList.isEmpty())
  {
    filePatternList.append("*");
  }

  // add default pattern if needed
  QStrList &examplePatternList = getList("EXAMPLE_PATTERNS");
  if (examplePatternList.isEmpty())
  {
    examplePatternList.append("*");
  }

  // add default pattern if needed
  //QStrList &imagePatternList = getList("IMAGE_PATTERNS");
  //if (imagePatternList.isEmpty())
  //{
  //  imagePatternList.append("*");
  //}
  
  // more checks needed if and only if the search engine is enabled.
  if (getBool("SEARCHENGINE"))
  {
    // check cgi name
    QCString &cgiName = getString("CGI_NAME");
    if (cgiName.isEmpty())
    {
      err("Error: tag CGI_NAME: no cgi script name after the CGI_NAME tag.\n");
      exit(1);
    }
    // check cgi URL
    QCString &cgiURL = getString("CGI_URL");
    if (cgiURL.isEmpty())
    {
      err("Error: tag CGI_URL: no URL to cgi directory specified.\n");
      exit(1);
    }
    else if (cgiURL.left(7)!="http://" && 
	     cgiURL.left(8)!="https://"
	    )
    {
      err("Error: tag CGI_URL: URL to cgi directory is invalid (must "
	  "start with http:// or https://).\n");
      exit(1);
    }
    // check documentation URL
    QCString &docURL = getString("DOC_URL");
    if (docURL.isEmpty())
    {
      docURL = getString("OUTPUT_DIRECTORY").copy().prepend("file://").append("html");
    }
    else if (docURL.left(7)!="http://" && 
	     docURL.left(8)!="https://" &&
	     docURL.left(7)!="file://"
	    )
    {
      err("Error: tag DOC_URL: URL to documentation is invalid or "
	  "not absolute.\n"); 
      exit(1);
    }
    // check absolute documentation path
    QCString &docAbsPath = getString("DOC_ABSPATH");
    if (docAbsPath.isEmpty())
    {
      docAbsPath = getString("OUTPUT_DIRECTORY")+"/html"; 
    }
    else if (docAbsPath[0]!='/' && docAbsPath[1]!=':')
    {
      err("Error: tag DOC_ABSPATH: path is not absolute!\n");
      exit(1);
    }
    // check path to doxysearch
    QCString &binAbsPath = getString("BIN_ABSPATH");
    if (binAbsPath.isEmpty())
    {
      err("Error: tag BIN_ABSPATH: no absolute path to doxysearch "
	  "specified.\n");
      exit(1);
    }
    else if (binAbsPath[0]!='/' && binAbsPath[1]!=':')
    {
      err("Error: tag BIN_ABSPATH: path is not absolute!\n");
      exit(1);
    }

    // check perl path
    bool found=FALSE;
    QCString &perlPath = getString("PERL_PATH");
    if (perlPath.isEmpty())
    {
      QFileInfo fi;
      fi.setFile("/usr/bin/perl");
      if (fi.exists()) 
      {
	perlPath="/usr/bin/perl";
        found=TRUE;
      }
      else
      {
	fi.setFile("/usr/local/bin/perl");
	if (fi.exists())
        {
  	  perlPath="/usr/local/bin/perl";
          found=TRUE;
        }
      }
    }
    if (!found)
    {
      QFileInfo fi(perlPath);
      if (!fi.exists())
      {
        warn_cont("Warning: tag PERL_PATH: perl interpreter not found at default or"
            "user specified (%s) location\n",
        perlPath.data());
      }
    }
  }

#undef PUTENV
#if defined(_WIN32) && !defined(__GNUC__)
#define PUTENV _putenv
#else
#define PUTENV putenv
#endif
  if (getBool("HAVE_DOT")) PUTENV("DOTFONTPATH=.");
  
}

void Config::init()
{
  ConfigOption *option = m_options->first();
  while (option)
  {
    option->init();
    option = m_options->next();
  }
}

void Config::create()
{
  if (m_initialized) return; 
  m_initialized = TRUE;

  ConfigString *cs;
  ConfigEnum   *ce;
  ConfigList   *cl;
  ConfigInt    *ci;
  ConfigBool   *cb;
  
  // option definitions
  //-----------------------------------------------------------------------------------------------
  addInfo("General","General configuration options");
  //-----------------------------------------------------------------------------------------------
  
  
  cs = addString(//"projectName",
                 "PROJECT_NAME",
                 "The PROJECT_NAME tag is a single word (or a sequence of words surrounded \n"
                 "by quotes) that should identify the project. "
                );
  cs = addString(//"projectNumber",
                 "PROJECT_NUMBER",
                 "The PROJECT_NUMBER tag can be used to enter a project or revision number. \n"
                 "This could be handy for archiving the generated documentation or \n"
                 "if some version control system is used.\n"
                );
  cs = addString(//"outputDir",
                 "OUTPUT_DIRECTORY",
                 "The OUTPUT_DIRECTORY tag is used to specify the (relative or absolute) \n"
                 "base path where the generated documentation will be put. \n"
                 "If a relative path is entered, it will be relative to the location \n"
                 "where doxygen was started. If left blank the current directory will be used.\n"
		);
  cs->setWidgetType(ConfigString::Dir);
  ce = addEnum(//"outputLanguage",
                    "OUTPUT_LANGUAGE",
                    "The OUTPUT_LANGUAGE tag is used to specify the language in which all \n"
                    "documentation generated by doxygen is written. Doxygen will use this \n"
                    "information to generate all constant output in the proper language. \n"
                    "The default language is English, other supported languages are: \n"
                    "Dutch, French, Italian, Czech, Swedish, German, Finnish, Japanese, \n"
                    "Korean, Hungarian, Norwegian, Spanish, Romanian, Russian, Croatian, \n"
                    "Polish, Portuguese, Brazilian and Slovene.\n",
                    "English"
                   );
  ce->addValue("Brazilian");
  ce->addValue("Croatian");
  ce->addValue("Czech");
  ce->addValue("Dutch");
  ce->addValue("English");
  ce->addValue("French");
  ce->addValue("Finnish");
  ce->addValue("German");
  ce->addValue("Hungarian");
  ce->addValue("Italian");
  ce->addValue("Japanese");
  ce->addValue("Korean");
  ce->addValue("Norwegian");
  ce->addValue("Romanian");
  ce->addValue("Russian");
  ce->addValue("Polish");
  ce->addValue("Portuguese");
  ce->addValue("Slovene");
  ce->addValue("Spanish");
  ce->addValue("Swedish");
  cb = addBool(  //"extractAllFlag",
                    "EXTRACT_ALL",
                    "If the EXTRACT_ALL tag is set to YES doxygen will assume all entities in \n"
                    "documentation are documented, even if no documentation was available. \n"
                    "Private class members and static file members will be hidden unless \n"
                    "the EXTRACT_PRIVATE and EXTRACT_STATIC tags are set to YES \n",
                    FALSE
                 );
  cb = addBool(  //"extractPrivateFlag",
                    "EXTRACT_PRIVATE",
                    "If the EXTRACT_PRIVATE tag is set to YES all private members of a class \n"
                    "will be included in the documentation. \n",
                    FALSE
                 );
  cb = addBool(  //"extractStaticFlag",
                    "EXTRACT_STATIC",
                    "If the EXTRACT_STATIC tag is set to YES all static members of a file \n"
                    "will be included in the documentation. \n",
                    FALSE
                 );
  cb = addBool(  //"hideMemberFlag",
                    "HIDE_UNDOC_MEMBERS",
                    "If the HIDE_UNDOC_MEMBERS tag is set to YES, Doxygen will hide all \n"
                    "undocumented members of documented classes, files or namespaces. \n"
                    "If set to NO (the default) these members will be included in the \n"
                    "various overviews, but no documentation section is generated. \n"
                    "This option has no effect if EXTRACT_ALL is enabled. \n",
                    FALSE
                 );
  cb = addBool(  //"hideClassFlag",
                    "HIDE_UNDOC_CLASSES",
                    "If the HIDE_UNDOC_CLASSES tag is set to YES, Doxygen will hide all \n"
                    "undocumented classes that are normally visible in the class hierarchy. \n"
                    "If set to NO (the default) these class will be included in the various \n"
                    "overviews. This option has no effect if EXTRACT_ALL is enabled. \n",
                    FALSE
                 );
  cb = addBool(  //"briefMemDescFlag",
                    "BRIEF_MEMBER_DESC",
                    "If the BRIEF_MEMBER_DESC tag is set to YES (the default) Doxygen will \n"
                    "include brief member descriptions after the members that are listed in \n"
                    "the file and class documentation (similar to JavaDoc). \n"
                    "Set to NO to disable this. \n",
                    TRUE
                 );
  cb = addBool(  //"repeatBriefFlag",
                    "REPEAT_BRIEF",
                    "If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend \n"
                    "the brief description of a member or function before the detailed description. \n"
                    "Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the \n"
                    "brief descriptions will be completely suppressed. \n",
                    TRUE
                 );
  cb = addBool(  //"alwaysDetailsFlag",
                    "ALWAYS_DETAILED_SEC",
                    "If the ALWAYS_DETAILED_SEC and REPEAT_BRIEF tags are both set to YES then \n"
                    "Doxygen will generate a detailed section even if there is only a brief \n"
                    "description. \n",
                    FALSE
                 );
  cb = addBool(  //"fullPathNameFlag",
                    "FULL_PATH_NAMES",
                    "If the FULL_PATH_NAMES tag is set to YES then Doxygen will prepend the full \n"
                    "path before files name in the file list and in the header files. If set \n"
                    "to NO the shortest path that makes the file name unique will be used. \n",
                    FALSE
                 );
  cl = addList(  //"stripFromPath",
                    "STRIP_FROM_PATH",
                    "If the FULL_PATH_NAMES tag is set to YES then the STRIP_FROM_PATH tag \n"
                    "can be used to strip a user defined part of the path. Stripping is \n"
                    "only done if one of the specified strings matches the left-hand part of \n"
                    "the path. It is allowed to use relative paths in the argument list.\n"
                 );
  cl->addDependency("FULL_PATH_NAMES");
  cb = addBool(  //"internalDocsFlag",
                    "INTERNAL_DOCS",
                    "The INTERNAL_DOCS tag determines if documentation \n"
                    "that is typed after a \\internal command is included. If the tag is set \n"
                    "to NO (the default) then the documentation will be excluded. \n"
                    "Set it to YES to include the internal documentation. \n",
                    FALSE
                 );
  cb = addBool(  //"classDiagramFlag",
                    "CLASS_DIAGRAMS",
                    "If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will \n"
                    "generate a class diagram (in Html and LaTeX) for classes with base or \n"
                    "super classes. Setting the tag to NO turns the diagrams off. \n",
                    TRUE
                 );
  cb = addBool(  //"sourceBrowseFlag",
                    "SOURCE_BROWSER",
                    "If the SOURCE_BROWSER tag is set to YES then a list of source files will \n"
                    "be generated. Documented entities will be cross-referenced with these sources. \n",
                    FALSE
                 );
  cb = addBool(  //"inlineSourceFlag",
                    "INLINE_SOURCES",
                    "Setting the INLINE_SOURCES tag to YES will include the body \n"
                    "of functions and classes directly in the documentation. \n",
                    FALSE
                 );
  cb = addBool(  //"stripCommentsFlag",
                    "STRIP_CODE_COMMENTS",
                    "Setting the STRIP_CODE_COMMENTS tag to YES (the default) will instruct \n"
                    "doxygen to hide any special comment blocks from generated source code \n"
                    "fragments. Normal C and C++ comments will always remain visible. \n",
                    TRUE
                 );
  cb = addBool(  //"caseSensitiveNames",
                    "CASE_SENSE_NAMES",
                    "If the CASE_SENSE_NAMES tag is set to NO then Doxygen will only generate \n"
                    "file names in lower case letters. If set to YES upper case letters are also \n"
                    "allowed. This is useful if you have classes or files whose names only differ \n"
                    "in case and if your file system supports case sensitive file names. Windows \n"
                    "users are adviced to set this option to NO.\n",
                    TRUE
                 );
  cb = addBool(  //"hideScopeNames",
                    "HIDE_SCOPE_NAMES",
                    "If the HIDE_SCOPE_NAMES tag is set to NO (the default) then Doxygen \n"
                    "will show members with their full class and namespace scopes in the \n"
                    "documentation. If set to YES the scope will be hidden. \n",
                    FALSE
                 );
  cb = addBool(  //"verbatimHeaderFlag",
                    "VERBATIM_HEADERS",
                    "If the VERBATIM_HEADERS tag is set to YES (the default) then Doxygen \n"
                    "will generate a verbatim copy of the header file for each class for \n"
                    "which an include is specified. Set to NO to disable this. \n",
                    TRUE
                 );
  cb = addBool(  //"showIncFileFlag",
                    "SHOW_INCLUDE_FILES",
                    "If the SHOW_INCLUDE_FILES tag is set to YES (the default) then Doxygen \n"
                    "will put list of the files that are included by a file in the documentation \n"
                    "of that file. \n",
                    TRUE
                 );
  cb = addBool(  //"autoBriefFlag",
                    "JAVADOC_AUTOBRIEF",
                    "If the JAVADOC_AUTOBRIEF tag is set to YES then Doxygen \n"
                    "will interpret the first line (until the first dot) of a JavaDoc-style \n"
                    "comment as the brief description. If set to NO, the JavaDoc \n"
                    "comments  will behave just like the Qt-style comments (thus requiring an \n"
                    "explict @brief command for a brief description. \n",
                    FALSE
                 );
  cb = addBool(  //"inheritDocsFlag",
                    "INHERIT_DOCS",
                    "If the INHERIT_DOCS tag is set to YES (the default) then an undocumented \n"
                    "member inherits the documentation from any documented member that it \n"
                    "reimplements. \n",
                    TRUE
                 );
  cb = addBool(  //"inlineInfoFlag",
                    "INLINE_INFO",
                    "If the INLINE_INFO tag is set to YES (the default) then a tag [inline] \n"
                    "is inserted in the documentation for inline members. \n",
                    TRUE
                 );
  cb = addBool(  //"sortMembersFlag",
                    "SORT_MEMBER_DOCS",
                    "If the SORT_MEMBER_DOCS tag is set to YES (the default) then doxygen \n"
                    "will sort the (detailed) documentation of file and class members \n"
                    "alphabetically by member name. If set to NO the members will appear in \n"
                    "declaration order. \n",
                    TRUE
                 );
  cb = addBool(  //"distributeDocFlag",
                    "DISTRIBUTE_GROUP_DOC",
                    "If member grouping is used in the documentation and the DISTRIBUTE_GROUP_DOC \n"
                    "tag is set to YES, then doxygen will reuse the documentation of the first \n"
                    "member in the group (if any) for the other members of the group. By default \n"
                    "all members of a group must be documented explicitly.\n",
                    FALSE
                 );
  ci = addInt(   //"tabSize",
                    "TAB_SIZE",
                    "The TAB_SIZE tag can be used to set the number of spaces in a tab. \n"
                    "Doxygen uses this value to replace tabs by spaces in code fragments. \n",
                    1,16,8
                );
  cl = addList(  //"sectionFilterList",
                    "ENABLED_SECTIONS",
                    "The ENABLE_SECTIONS tag can be used to enable conditional \n"
                    "documentation sections, marked by \\if sectionname ... \\endif. \n"
                 );
  cb = addBool(  //"generateTodoList",
                    "GENERATE_TODOLIST",
                    "The GENERATE_TODOLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the todo list. This list is created by putting \\todo \n"
                    "commands in the documentation.\n",
                    TRUE
                 );
  cb = addBool(  //"generateTestList",
                    "GENERATE_TESTLIST",
                    "The GENERATE_TESTLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the test list. This list is created by putting \\test \n"
                    "commands in the documentation.\n",
                    TRUE
                 );
  cb = addBool(  //"generateBugList",
                    "GENERATE_BUGLIST",
                    "The GENERATE_BUGLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the bug list. This list is created by putting \\bug \n"
                    "commands in the documentation.\n",
                    TRUE
                 );
  cl = addList(  //"aliasList",
                    "ALIASES",
                    "This tag can be used to specify a number of aliases that acts \n"
                    "as commands in the documentation. An alias has the form \"name=value\". \n"
                    "For example adding \"sideeffect=\\par Side Effects:\\n\" will allow you to \n"
                    "put the command \\sideeffect (or @sideeffect) in the documentation, which \n"
                    "will result in a user defined paragraph with heading \"Side Effects:\". \n"
                    "You can put \\n's in the value part of an alias to insert newlines. \n" 
                 );
  ci = addInt(   //"maxInitLines",
                    "MAX_INITIALIZER_LINES",
                    "The MAX_INITIALIZER_LINES tag determines the maximum number of lines \n"
                    "the initial value of a variable or define consist of for it to appear in \n"
                    "the documentation. If the initializer consists of more lines than specified \n"
                    "here it will be hidden. Use a value of 0 to hide initializers completely. \n"
                    "The appearance of the initializer of individual variables and defines in the \n"
                    "documentation can be controlled using \\showinitializer or \\hideinitializer \n"
                    "command in the documentation regardless of this setting. \n",
                    0,10000,30
                 );
  cb = addBool(  //"optimizeForCFlag",
                    "OPTIMIZE_OUTPUT_FOR_C",
                    "Set the OPTIMIZE_OUTPUT_FOR_C tag to YES if your project consists of C sources \n"
                    "only. Doxygen will then generate output that is more tailored for C. \n"
                    "For instance some of the names that are used will be different. The list \n"
                    "of all members will be omitted, etc. \n",
                    FALSE
                 );
  cb = addBool(  //"showUsedFilesFlag",
                    "SHOW_USED_FILES",
                    "Set the SHOW_USED_FILES tag to NO to disable the list of files generated \n"
                    "at the bottom of the documentation of classes and structs. If set to YES the \n"
                    "list will mention the files that were used to generate the documentation. \n",
                    TRUE
                );
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Messages","configuration options related to warning and progress messages");
  //-----------------------------------------------------------------------------------------------

  cb = addBool(  //"quietFlag",
                    "QUIET",
                    "The QUIET tag can be used to turn on/off the messages that are generated \n"
                    "by doxygen. Possible values are YES and NO. If left blank NO is used. \n",
                    FALSE
                 );
  cb = addBool(  //"warningFlag",
                    "WARNINGS",
                    "The WARNINGS tag can be used to turn on/off the warning messages that are \n"
                    "generated by doxygen. Possible values are YES and NO. If left blank \n"
                    "NO is used. \n",
                    TRUE
                 );
  cb = addBool(  //"warningUndocFlag",
                    "WARN_IF_UNDOCUMENTED",
                    "If WARN_IF_UNDOCUMENTED is set to YES, then doxygen will generate warnings \n"
                    "for undocumented members. If EXTRACT_ALL is set to YES then this flag will \n"
                    "automatically be disabled. \n",
                    TRUE
                 );
  cs = addString(//"warnFormat",
                    "WARN_FORMAT",
                    "The WARN_FORMAT tag determines the format of the warning messages that \n"
                    "doxygen can produce. The string should contain the $file, $line, and $text \n"
                    "tags, which will be replaced by the file and line number from which the \n"
                    "warning originated and the warning text. \n"
                   ); 
  cs->setDefaultValue("$file:$line: $text");
  cs = addString(//"warnLogFile",
                    "WARN_LOGFILE",
                    "The WARN_LOGFILE tag can be used to specify a file to which warning \n"
                    "and error messages should be written. If left blank the output is written \n"
                    "to stderr. \n"
                   );
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Input","configuration options related to the input files");
  //-----------------------------------------------------------------------------------------------
  cl = addList(  //"inputSources",
                    "INPUT",
                    "The INPUT tag can be used to specify the files and/or directories that contain \n"
                    "documented source files. You may enter file names like \"myfile.cpp\" or \n"
                    "directories like \"/usr/src/myproject\". Separate the files or directories \n"
                    "with spaces. \n"
                 );
  cl->setWidgetType(ConfigList::FileAndDir);
  cl = addList(  //"filePatternList",
                    "FILE_PATTERNS",
                    "If the value of the INPUT tag contains directories, you can use the \n"
                    "FILE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n"
                    "and *.h) to filter out the source-files in the directories. If left \n"
                    "blank all files are included. \n"
                 );
  cb = addBool(  //"recursiveFlag",
                    "RECURSIVE",
                    "The RECURSIVE tag can be used to turn specify whether or not subdirectories \n"
                    "should be searched for input files as well. Possible values are YES and NO. \n"
                    "If left blank NO is used. \n",
                    FALSE
                 );
  cl = addList(  //"excludeSources",
                    "EXCLUDE",
                    "The EXCLUDE tag can be used to specify files and/or directories that should \n"
                    "excluded from the INPUT source files. This way you can easily exclude a \n"
                    "subdirectory from a directory tree whose root is specified with the INPUT tag. \n"
                 );
  cl->setWidgetType(ConfigList::FileAndDir);
  cl = addList(  //"excludePatternList",
                    "EXCLUDE_PATTERNS",
                    "If the value of the INPUT tag contains directories, you can use the \n"
                    "EXCLUDE_PATTERNS tag to specify one or more wildcard patterns to exclude \n"
                    "certain files from those directories. \n"
                 );
  cl = addList(  //"examplePath",
                    "EXAMPLE_PATH",
                    "The EXAMPLE_PATH tag can be used to specify one or more files or \n"
                    "directories that contain example code fragments that are included (see \n"
                    "the \\include command). \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cl = addList(  //"examplePatternList",
                    "EXAMPLE_PATTERNS",
                    "If the value of the EXAMPLE_PATH tag contains directories, you can use the \n"
                    "EXAMPLE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n"
                    "and *.h) to filter out the source-files in the directories. If left \n"
                    "blank all files are included. \n"
                 );
  cl = addList(  //"imagePath",
                    "IMAGE_PATH",
                    "The IMAGE_PATH tag can be used to specify one or more files or \n"
                    "directories that contain image that are included in the documentation (see \n"
                    "the \\image command). \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cs = addString(//"inputFilter",
                    "INPUT_FILTER",
                    "The INPUT_FILTER tag can be used to specify a program that doxygen should \n"
                    "invoke to filter for each input file. Doxygen will invoke the filter program \n"
                    "by executing (via popen()) the command <filter> <input-file>, where <filter> \n"
                    "is the value of the INPUT_FILTER tag, and <input-file> is the name of an \n"
                    "input file. Doxygen will then use the output that the filter program writes \n"
                    "to standard output. \n"
                   );
  cl->setWidgetType(ConfigList::File);
  cb = addBool(  //"filterForSourceFlag",
                    "FILTER_SOURCE_FILES",
                    "If the FILTER_SOURCE_FILES tag is set to YES, the input filter (if set using \n"
                    "INPUT_FILTER) will be used to filter the input files when producing source \n"
                    "files to browse. \n",
                    FALSE
                );    
  
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Index","configuration options related to the alphabetical class index");
  //-----------------------------------------------------------------------------------------------
  
  cb = addBool(  //"alphaIndexFlag",
                    "ALPHABETICAL_INDEX",
                    "If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index \n"
                    "of all compounds will be generated. Enable this if the project \n"
                    "contains a lot of classes, structs, unions or interfaces. \n",
                    FALSE
                 );
  ci = addInt(   //"colsInAlphaIndex",
                    "COLS_IN_ALPHA_INDEX",
                    "If the alphabetical index is enabled (see ALPHABETICAL_INDEX) then \n"
                    "the COLS_IN_ALPHA_INDEX tag can be used to specify the number of columns \n"
                    "in which this list will be split (can be a number in the range [1..20]) \n",
                    1,20,5
                );
  cl = addList(  //"ignorePrefixList",
                    "IGNORE_PREFIX",
                    "In case all classes in a project start with a common prefix, all \n"
                    "classes will be put under the same header in the alphabetical index. \n"
                    "The IGNORE_PREFIX tag can be used to specify one or more prefixes that \n"
                    "should be ignored while generating the index headers. \n"
                 );
  //-----------------------------------------------------------------------------------------------
  addInfo(  "HTML","configuration options related to the HTML output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"generateHtml",
                    "GENERATE_HTML",
                    "If the GENERATE_HTML tag is set to YES (the default) Doxygen will \n"
                    "generate HTML output. \n",
                    TRUE
                 );
  cs = addString(//"htmlOutputDir",
                    "HTML_OUTPUT",
                    "The HTML_OUTPUT tag is used to specify where the HTML docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `html' will be used as the default path. \n"
                   );
  cs->setDefaultValue("html");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_HTML");
  cs = addString(//"headerFile",
                    "HTML_HEADER",
                    "The HTML_HEADER tag can be used to specify a personal HTML header for \n"
                    "each generated HTML page. If it is left blank doxygen will generate a \n"
                    "standard header.\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(//"footerFile",
                    "HTML_FOOTER",
                    "The HTML_FOOTER tag can be used to specify a personal HTML footer for \n"
                    "each generated HTML page. If it is left blank doxygen will generate a \n"
                    "standard footer.\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(//"htmlStyleSheet",
                    "HTML_STYLESHEET",
                    "The HTML_STYLESHEET tag can be used to specify a user defined cascading \n"
                    "style sheet that is used by each HTML page. It can be used to \n"
                    "fine-tune the look of the HTML output. If the tag is left blank doxygen \n"
                    "will generate a default style sheet \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cb = addBool(  //"htmlAlignMemberFlag",
                    "HTML_ALIGN_MEMBERS",
                    "If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes, \n"
                    "files or namespaces will be aligned in HTML using tables. If set to \n"
                    "NO a bullet list will be used. \n",
                    TRUE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(  //"htmlHelpFlag",
                    "GENERATE_HTMLHELP",
                    "If the GENERATE_HTMLHELP tag is set to YES, additional index files \n"
                    "will be generated that can be used as input for tools like the \n"
                    "Microsoft HTML help workshop to generate a compressed HTML help file (.chm) \n"
                    "of the generated HTML documentation. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(  //"htmlHelpChiFlag",
                    "GENERATE_CHI",
                    "If the GENERATE_HTMLHELP tag is set to YES, the GENERATE_CHI flag \n"
                    "controls if a separate .chi index file is generated (YES) or that \n"
                    "it should be included in the master .chm file (NO).\n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(  //"htmlHelpTocFlag",
                    "BINARY_TOC",
                    "If the GENERATE_HTMLHELP tag is set to YES, the BINARY_TOC flag \n"
                    "controls whether a binary table of contents is generated (YES) or a \n"
                    "normal table of contents (NO) in the .chm file.\n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(  //"htmlHelpTocExpandFlag",
                    "TOC_EXPAND",
                    "The TOC_EXPAND flag can be set YES to add extra items for group members \n"
                    "to the contents of the Html help documentation and to the tree view. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(  //"noIndexFlag",
                    "DISABLE_INDEX",
                    "The DISABLE_INDEX tag can be used to turn on/off the condensed index at \n"
                    "top of each HTML page. The value NO (the default) enables the index and \n"
                    "the value YES disables it. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  ci = addInt(   //"enumValuesPerLine",
                    "ENUM_VALUES_PER_LINE",
                    "This tag can be used to set the number of enum values (range [1..20]) \n"
                    "that doxygen will group on one line in the generated HTML documentation. \n",
                    1,20,4
                );
  ci->addDependency("GENERATE_HTML");
  cb = addBool(  //"ftvHelpFlag",
                    "GENERATE_TREEVIEW",
                    "If the GENERATE_TREEVIEW tag is set to YES, a side panel will be\n"
                    "generated containing a tree-like index structure (just like the one that \n"
                    "is generated for HTML Help). For this to work a browser that supports \n"
                    "JavaScript and frames is required (for instance Netscape 4.0+ \n"
                    "or Internet explorer 4.0+). ",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  ci = addInt(   //"treeViewWidth",
                    "TREEVIEW_WIDTH",
                    "If the treeview is enabled (see GENERATE_TREEVIEW) then this tag can be \n"
                    "used to set the initial width (in pixels) of the frame in which the tree \n"
                    "is shown. \n",
                    0,1500,250
                 );
  ci->addDependency("GENERATE_HTML");

  //-----------------------------------------------------------------------------------------------
  addInfo(  "LaTeX","configuration options related to the LaTeX output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"generateLatex",
                    "GENERATE_LATEX",
                    "If the GENERATE_LATEX tag is set to YES (the default) Doxygen will \n"
                    "generate Latex output. \n",
                    TRUE
                 );
  cs = addString(//"latexOutputDir",
                    "LATEX_OUTPUT",
                    "The LATEX_OUTPUT tag is used to specify where the LaTeX docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `latex' will be used as the default path. \n"
                   );
  cs->setDefaultValue("latex");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_LATEX");
  cb = addBool(  //"compactLatexFlag",
                    "COMPACT_LATEX",
                    "If the COMPACT_LATEX tag is set to YES Doxygen generates more compact \n"
                    "LaTeX documents. This may be useful for small projects and may help to \n"
                    "save some trees in general. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_LATEX");
  ce = addEnum(//"paperType",
                    "PAPER_TYPE",
                    "The PAPER_TYPE tag can be used to set the paper type that is used \n"
                    "by the printer. Possible values are: a4, a4wide, letter, legal and \n"
                    "executive. If left blank a4wide will be used. \n",
                    "a4wide"
                   );
  ce->addValue("a4");
  ce->addValue("a4wide");
  ce->addValue("letter");
  ce->addValue("legal");
  ce->addValue("executive");
  ce->addDependency("GENERATE_LATEX");
  cl = addList(  //"extraPackageList",
                    "EXTRA_PACKAGES",
                    "The EXTRA_PACKAGES tag can be to specify one or more names of LaTeX \n"
                    "packages that should be included in the LaTeX output. \n"
                 );
  cl->addDependency("GENERATE_LATEX");
  cs = addString(//"latexHeaderFile",
                    "LATEX_HEADER",
                    "The LATEX_HEADER tag can be used to specify a personal LaTeX header for \n"
                    "the generated latex document. The header should contain everything until \n"
                    "the first chapter. If it is left blank doxygen will generate a \n"
                    "standard header. Notice: only use this tag if you know what you are doing! \n"
                 );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_LATEX");
  cb = addBool(  //"pdfHyperFlag",
                    "PDF_HYPERLINKS",
                    "If the PDF_HYPERLINKS tag is set to YES, the LaTeX that is generated \n"
                    "is prepared for conversion to pdf (using ps2pdf). The pdf file will \n"
                    "contain links (just like the HTML output) instead of page references \n"
                    "This makes the output suitable for online browsing using a pdf viewer. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_LATEX");
  cb = addBool(  //"usePDFLatexFlag",
                    "USE_PDFLATEX",
                    "If the USE_PDFLATEX tag is set to YES, pdflatex will be used instead of \n"
                    "plain latex in the generated Makefile. Set this option to YES to get a \n"
                    "higher quality PDF documentation. \n",
                    FALSE
                 );   
  cb->addDependency("GENERATE_LATEX");
  cb = addBool(  //"latexBatchModeFlag",
                    "LATEX_BATCHMODE",
                    "If the LATEX_BATCHMODE tag is set to YES, doxygen will add the \\\\batchmode. \n"
                    "command to the generated LaTeX files. This will instruct LaTeX to keep \n"
                    "running if errors occur, instead of asking the user for help. \n"
                    "This option is also used when generating formulas in HTML. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_LATEX");
  //-----------------------------------------------------------------------------------------------
  addInfo(  "RTF","configuration options related to the RTF output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"generateRTF",
                    "GENERATE_RTF",
                    "If the GENERATE_RTF tag is set to YES Doxygen will generate RTF output \n"
                    "The RTF output is optimised for Word 97 and may not look very pretty with \n"
                    "other RTF readers or editors.\n",
                    TRUE
                 );
  cs = addString(//"rtfOutputDir",
                    "RTF_OUTPUT",
                    "The RTF_OUTPUT tag is used to specify where the RTF docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `rtf' will be used as the default path. \n"
                 );
  cs->setDefaultValue("rtf");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_RTF");
  cb = addBool(  //"compactRTFFlag",
                    "COMPACT_RTF",
                    "If the COMPACT_RTF tag is set to YES Doxygen generates more compact \n"
                    "RTF documents. This may be useful for small projects and may help to \n"
                    "save some trees in general. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_RTF");
  cb = addBool(  //"rtfHyperFlag",
                    "RTF_HYPERLINKS",
                    "If the RTF_HYPERLINKS tag is set to YES, the RTF that is generated \n"
                    "will contain hyperlink fields. The RTF file will \n"
                    "contain links (just like the HTML output) instead of page references. \n"
                    "This makes the output suitable for online browsing using a WORD or other. \n"
                    "programs which support those fields. \n"
                    "Note: wordpad (write) and others do not support links. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_RTF");
  cs = addString(//"rtfStylesheetFile",
                    "RTF_STYLESHEET_FILE",
                    "Load stylesheet definitions from file. Syntax is similar to doxygen's \n"
                    "config file, i.e. a series of assigments. You only have to provide \n"
                    "replacements, missing definitions are set to their default value. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_RTF");
  cs = addString(//"rtfExtentionsFile", 
                    "RTF_EXTENSIONS_FILE",
                    "Set optional variables used in the generation of an rtf document. \n"
                    "Syntax is similar to doxygen's config file.\n"
                );                  
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_RTF");

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Man","configuration options related to the man page output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"generateMan",
                    "GENERATE_MAN",
                    "If the GENERATE_MAN tag is set to YES (the default) Doxygen will \n"
                    "generate man pages \n",
                    TRUE
                   );
  cs = addString(//"manOutputDir",
                    "MAN_OUTPUT",
                    "The MAN_OUTPUT tag is used to specify where the man pages will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `man' will be used as the default path. \n"
                   );
  cs->setDefaultValue("man");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_MAN");
  cs = addString(//"manExtension",
                    "MAN_EXTENSION",
                    "The MAN_EXTENSION tag determines the extension that is added to \n"
                    "the generated man pages (default is the subroutine's section .3) \n"
                   );
  cs->setDefaultValue(".3");
  cs->addDependency("GENERATE_MAN");
  //-----------------------------------------------------------------------------------------------
  //addInfo(  "XML","configuration options related to the XML output");
  //-----------------------------------------------------------------------------------------------
  //addBool(  "generateXML",
  //                  "GENERATE_XML",
  //                  "FALSE",
  //                  "generate XML output",
  //                  "If the GENERATE_XML tag is set to YES Doxygen will \n"
  //                  "generate an XML file that captures the structure of \n"
  //                  "the code including all documentation. Warning: This feature \n"
  //                  "is still experimental and very incomplete.\n"
  //               );
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Preprocessor","Configuration options related to the preprocessor   ");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"preprocessingFlag",
                    "ENABLE_PREPROCESSING",
                    "If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will \n"
                    "evaluate all C-preprocessor directives found in the sources and include \n"
                    "files. \n",
                    TRUE
                 );
  cb = addBool(  //"macroExpansionFlag",
                    "MACRO_EXPANSION",
                    "If the MACRO_EXPANSION tag is set to YES Doxygen will expand all macro \n"
                    "names in the source code. If set to NO (the default) only conditional \n"
                    "compilation will be performed. Macro expansion can be done in a controlled \n"
                    "way by setting EXPAND_ONLY_PREDEF to YES. \n",
                    FALSE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  cb = addBool(  //"onlyPredefinedFlag",
                    "EXPAND_ONLY_PREDEF",
                    "If the EXPAND_ONLY_PREDEF and MACRO_EXPANSION tags are both set to YES \n"
                    "then the macro expansion is limited to the macros specified with the \n"
                    "PREDEFINED and EXPAND_AS_PREDEFINED tags. \n",
                    FALSE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  cb = addBool(  //"searchIncludeFlag",
                    "SEARCH_INCLUDES",
                    "If the SEARCH_INCLUDES tag is set to YES (the default) the includes files \n"
                    "in the INCLUDE_PATH (see below) will be search if a #include is found. \n",
                    TRUE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  cl = addList(  //"includePath",
                    "INCLUDE_PATH",
                    "The INCLUDE_PATH tag can be used to specify one or more directories that \n"
                    "contain include files that are not input files but should be processed by \n"
                    "the preprocessor. \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cl->addDependency("ENABLE_PREPROCESSING");
  cl = addList(  //"includeFilePatternList",
                    "INCLUDE_FILE_PATTERNS",
                    "You can use the INCLUDE_FILE_PATTERNS tag to specify one or more wildcard \n"
                    "patterns (like *.h and *.hpp) to filter out the header-files in the \n"
                    "directories. If left blank, the patterns specified with FILE_PATTERNS will \n"
                    "be used. \n"
                 );
  cl->addDependency("ENABLE_PREPROCESSING");
  cl = addList(  //"predefined",
                    "PREDEFINED",
                    "The PREDEFINED tag can be used to specify one or more macro names that \n"
                    "are defined before the preprocessor is started (similar to the -D option of \n"
                    "gcc). The argument of the tag is a list of macros of the form: name \n"
                    "or name=definition (no spaces). If the definition and the = are \n"
                    "omitted =1 is assumed. \n"
                 );
  cl->addDependency("ENABLE_PREPROCESSING");
  cl = addList(  //"expandAsDefinedList",
                    "EXPAND_AS_DEFINED",
                    "If the MACRO_EXPANSION and EXPAND_PREDEF_ONLY tags are set to YES then \n"
                    "this tag can be used to specify a list of macro names that should be expanded. \n"
                    "The macro definition that is found in the sources will be used. \n"
                    "Use the PREDEFINED tag if you want to use a different macro definition. \n" 
                 );
  cl->addDependency("ENABLE_PREPROCESSING");
  //-----------------------------------------------------------------------------------------------
  addInfo(  "External","Configuration::addtions related to external references   ");
  //-----------------------------------------------------------------------------------------------
  cl = addList(  //"tagFileList",
                    "TAGFILES",
                    "The TAGFILES tag can be used to specify one or more tagfiles. \n"
                 );
  cl->setWidgetType(ConfigList::File);
  cs = addString(//"genTagFile",
                    "GENERATE_TAGFILE",
                    "When a file name is specified after GENERATE_TAGFILE, doxygen will create \n"
                    "a tag file that is based on the input files it reads. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cb = addBool(  //"allExtFlag",
                    "ALLEXTERNALS",
                    "If the ALLEXTERNALS tag is set to YES all external classes will be listed \n"
                    "in the class index. If set to NO only the inherited external classes \n"
                    "will be listed. \n",
                    FALSE
                 );
  cs = addString(//"perlPath",
                    "PERL_PATH",
                    "The PERL_PATH should be the absolute path and name of the perl script \n"
                    "interpreter (i.e. the result of `which perl'). \n"
                   );
  cs->setDefaultValue("/usr/bin/perl");
  cs->setWidgetType(ConfigString::Dir);

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Dot","Configuration options related to the dot tool   ");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"haveDotFlag",
                    "HAVE_DOT",
                    "If you set the HAVE_DOT tag to YES then doxygen will assume the dot tool is \n"
                    "available from the path. This tool is part of Graphviz, a graph visualization \n"
                    "toolkit from AT&T and Lucent Bell Labs. The other options in this section \n"
                    "have no effect if this option is set to NO (the default) \n",
                    FALSE
                 );
  cb = addBool(  //"classGraphFlag",
                    "CLASS_GRAPH",
                    "If the CLASS_GRAPH and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for each documented class showing the direct and \n"
                    "indirect inheritance relations. Setting this tag to YES will force the \n"
                    "the CLASS_DIAGRAMS tag to NO.\n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(  //"collGraphFlag",
                    "COLLABORATION_GRAPH",
                    "If the COLLABORATION_GRAPH and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for each documented class showing the direct and \n"
                    "indirect implementation dependencies (inheritance, containment, and \n"
                    "class references variables) of the class with other documented classes. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(  //"includeGraphFlag",
                    "INCLUDE_GRAPH",
                    "If the ENABLE_PREPROCESSING, INCLUDE_GRAPH, and HAVE_DOT tags are set to \n"
                    "YES then doxygen will generate a graph for each documented file showing \n"
                    "the direct and indirect include dependencies of the file with other \n"
                    "documented files. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(  //"includedByGraphFlag",
                    "INCLUDED_BY_GRAPH",
                    "If the ENABLE_PREPROCESSING, INCLUDED_BY_GRAPH, and HAVE_DOT tags are set to \n"
                    "YES then doxygen will generate a graph for each documented header file showing \n"
                    "the documented files that directly or indirectly include this file \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(  //"gfxHierarchyFlag",
                    "GRAPHICAL_HIERARCHY",
                    "If the GRAPHICAL_HIERARCHY and HAVE_DOT tags are set to YES then doxygen \n"
                    "will graphical hierarchy of all classes instead of a textual one. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cs = addString(//"dotPath",
                    "DOT_PATH",
                    "The tag DOT_PATH can be used to specify the path where the dot tool can be \n"
                    "found. If left blank, it is assumed the dot tool can be found on the path. \n"
                   );
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("HAVE_DOT");
  ci = addInt(   //"maxDotGraphWidth",
                    "MAX_DOT_GRAPH_WIDTH",
                    "The MAX_DOT_GRAPH_WIDTH tag can be used to set the maximum allowed width \n"
                    "(in pixels) of the graphs generated by dot. If a graph becomes larger than \n"
                    "this value, doxygen will try to truncate the graph, so that it fits within \n"
                    "the specified constraint. Beware that most browsers cannot cope with very \n"
                    "large images. \n",
                    100,30000, 1024
                );
  ci->addDependency("HAVE_DOT");
  ci = addInt(   //"maxDotGraphHeight",
                    "MAX_DOT_GRAPH_HEIGHT",
                    "The MAX_DOT_GRAPH_HEIGHT tag can be used to set the maximum allows height \n"
                    "(in pixels) of the graphs generated by dot. If a graph becomes larger than \n"
                    "this value, doxygen will try to truncate the graph, so that it fits within \n"
                    "the specified constraint. Beware that most browsers cannot cope with very \n"
                    "large images. \n",
                    100,30000,1024
                );
  ci->addDependency("HAVE_DOT");
  cb = addBool(  //"generateLegend",
                    "GENERATE_LEGEND",
                    "If the GENERATE_LEGEND tag is set to YES (the default) Doxygen will \n"
                    "generate a legend page explaining the meaning of the various boxes and \n"
                    "arrows in the dot generated graphs. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(  //"dotCleanUp",
                    "DOT_CLEANUP",
                    "If the DOT_CLEANUP tag is set to YES (the default) Doxygen will \n"
                    "remove the intermedate dot files that are used to generate \n"
                    "the various graphs. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Search","Configuration::addtions related to the search engine   ");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  //"searchEngineFlag",
                    "SEARCHENGINE",
                    "The SEARCHENGINE tag specifies whether or not a search engine should be \n"
                    "used. If set to NO the values of all tags below this one will be ignored. \n",
                    FALSE
                 );
  cs = addString(//"cgiName",
                    "CGI_NAME",
                    "The CGI_NAME tag should be the name of the CGI script that \n"
                    "starts the search engine (doxysearch) with the correct parameters. \n"
                    "A script with this name will be generated by doxygen. \n"
                   );
  cs->setDefaultValue("search.cgi");
  cs->addDependency("SEARCHENGINE");
  cs = addString(//"cgiURL",
                    "CGI_URL",
                    "The CGI_URL tag should be the absolute URL to the directory where the \n"
                    "cgi binaries are located. See the documentation of your http daemon for \n"
                    "details. \n"
                   );
  cs->addDependency("SEARCHENGINE");
  cs = addString(//"docURL",
                    "DOC_URL",
                    "The DOC_URL tag should be the absolute URL to the directory where the \n"
                    "documentation is located. If left blank the absolute path to the \n"
                    "documentation, with file:// prepended to it, will be used. \n"
                   );
  cs->addDependency("SEARCHENGINE");
  cs = addString(//"docAbsPath",
                    "DOC_ABSPATH",
                    "The DOC_ABSPATH tag should be the absolute path to the directory where the \n"
                    "documentation is located. If left blank the directory on the local machine \n"
                    "will be used. \n"
                   );
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("SEARCHENGINE");
  cs = addString(//"binAbsPath",
                    "BIN_ABSPATH",
                    "The BIN_ABSPATH tag must point to the directory where the doxysearch binary \n"
                    "is installed. \n"
                   );
  cs->setDefaultValue("/usr/local/bin/");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("SEARCHENGINE");
  cl = addList(  //"extDocPathList",
                    "EXT_DOC_PATHS",
                    "The EXT_DOC_PATHS tag can be used to specify one or more paths to \n"
                    "documentation generated for other projects. This allows doxysearch to search \n"
                    "the documentation for these projects as well. \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cl->addDependency("SEARCHENGINE");

  // The IMAGE_PATTERNS tag is now officially obsolete.
}


void Config::parse(const QCString &s,const char *fn)
{
  config = Config::instance();
  inputString   = s;
  inputPosition = 0;
  yyLineNr      = 1;
  yyFileName=fn;
  includeStack.setAutoDelete(TRUE);
  includeStack.clear();
  includeDepth  = 0;
  configYYrestart( configYYin );
  BEGIN( Start );
  configYYlex();
}

//extern "C" { // some bogus code to keep the compiler happy
//  int  configYYwrap() { return 1 ; }
//}
