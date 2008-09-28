/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

#include <qfileinfo.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qstack.h>
#include <qglobal.h>
  
#include "config.h"
#include "version.h"
#include "portable.h"

#include "lang_cfg.h"

#undef Config_getString
#undef Config_getInt
#undef Config_getList
#undef Config_getEnum
#undef Config_getBool

// use in-class definitions
#define Config_getString(val)  getString(__FILE__,__LINE__,val)
#define Config_getInt(val)     getInt(__FILE__,__LINE__,val)
#define Config_getList(val)    getList(__FILE__,__LINE__,val)
#define Config_getEnum(val)    getEnum(__FILE__,__LINE__,val)
#define Config_getBool(val)    getBool(__FILE__,__LINE__,val)
  
void config_err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args); 
}
void config_warn(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

static QCString configStringRecode(
    const QCString &str,
    const char *fromEncoding,
    const char *toEncoding);

#define MAX_INCLUDE_DEPTH 10
#define YY_NEVER_INTERACTIVE 1

/* -----------------------------------------------------------------
 */

QCString ConfigOption::convertToComment(const QCString &s)
{
  QCString result;
  if (s.isEmpty()) return result;
  else
  {
    result+="# ";
    QCString tmp=s.stripWhiteSpace();
    char *p=tmp.data();
    char c;
    while ((c=*p++))
    {
      if (c=='\n') result+="\n# ";
      else result+=c;
    }
    result+='\n';
  }
  return result;
}

void ConfigOption::writeBoolValue(QTextStream &t,bool v)
{
  if (v) t << "YES"; else t << "NO";
}

void ConfigOption::writeIntValue(QTextStream &t,int i)
{
  t << i;
}

void ConfigOption::writeStringValue(QTextStream &t,QCString &s)
{
  char c;
  bool needsEscaping=FALSE;
  // convert the string back to it original encoding
  QCString se = configStringRecode(s,"UTF-8",m_encoding);
  const char *p=se.data();
  if (p)
  {
    while ((c=*p++)!=0 && !needsEscaping) 
      needsEscaping = (c==' ' || c=='\n' || c=='\t' || c=='"' || c=='#');
    if (needsEscaping)
    { 
      t << "\"";
      p=se.data();
      while (*p)
      {
	if (*p==' ' && *(p+1)=='\0') break; // skip inserted space at the end
	if (*p=='"') t << "\\"; // escape quotes
	t << *p++;
      }
      t << "\"";
    }
    else
    {
      t << se;
    }
  }
}

void ConfigOption::writeStringList(QTextStream &t,QStrList &l)
{
  const char *p = l.first();
  bool first=TRUE;
  while (p)
  {
    QCString s=p;
    if (!first) t << "                         ";
    first=FALSE;
    writeStringValue(t,s);
    p = l.next();
    if (p) t << " \\" << endl;
  }
}

/* -----------------------------------------------------------------
 */

Config *Config::m_instance = 0;

void ConfigInt::convertStrToVal() 
{
  if (!m_valueString.isEmpty())
  {
    bool ok;
    int val = m_valueString.toInt(&ok);
    if (!ok || val<m_minVal || val>m_maxVal)
    {
      config_warn("Warning: argument `%s' for option %s is not a valid number in the range [%d..%d]!\n"
                "Using the default: %d!\n",m_valueString.data(),m_name.data(),m_minVal,m_maxVal,m_value);
    }
    m_value=val;
  }
}

void ConfigBool::convertStrToVal()
{
  QCString val = m_valueString.stripWhiteSpace().lower();
  if (!val.isEmpty())
  {
    if (val=="yes" || val=="true" || val=="1") 
    {
      m_value=TRUE;
    }
    else if (val=="no" || val=="false" || val=="0")
    {
      m_value=FALSE;
    }
    else
    {
      config_warn("Warning: argument `%s' for option %s is not a valid boolean value\n"
                "Using the default: %s!\n",m_valueString.data(),m_name.data(),m_value?"YES":"NO");
    }
  }
}

QCString &Config::getString(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_String)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of string type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigString *)opt)->valueRef();
}

QStrList &Config::getList(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_List)
  {
    config_err("%d<%d>: Internal error: Requested option %s not of list type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigList *)opt)->valueRef();
}

QCString &Config::getEnum(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Enum)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of enum type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigEnum *)opt)->valueRef();
}

int &Config::getInt(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Int)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of integer type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigInt *)opt)->valueRef();
}

bool &Config::getBool(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Bool)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of integer type!\n",fileName,num,name);
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
static QCString     encoding;

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
	if (inputString==0) return c;
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
	return (int)fread(buf,1,max_size,includeStack.current()->filePtr);
    }
}


static QCString configStringRecode(
    const QCString &str,
    const char *fromEncoding,
    const char *toEncoding)
{
  QCString inputEncoding = fromEncoding;
  QCString outputEncoding = toEncoding;
  if (inputEncoding.isEmpty() || outputEncoding.isEmpty() || inputEncoding==outputEncoding) return str;
  int inputSize=str.length();
  size_t outputSize=inputSize*4+1;
  QCString output(outputSize);
  void *cd = portable_iconv_open(outputEncoding,inputEncoding);
  if (cd==(void *)(-1)) 
  {
    fprintf(stderr,"Error: unsupported character conversion: '%s'->'%s'\n",
        inputEncoding.data(),outputEncoding.data());
    exit(1);
  }
  size_t iLeft=inputSize;
  size_t oLeft=outputSize;
  const char *inputPtr  = str.data();
  char       *outputPtr = output.data();
  if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
  {
    outputSize-=oLeft;
    output.resize(outputSize+1);
    output.at(outputSize)='\0';
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,srcBuf.data());
  }
  else
  {
    fprintf(stderr,"Error: failed to translate characters from %s to %s: %s\n",
        inputEncoding.data(),outputEncoding.data(),strerror(errno));
    exit(1);
  }
  portable_iconv_close(cd);
  return output;
}

static void checkEncoding()
{
  ConfigString *option = (ConfigString*)config->get("DOXYFILE_ENCODING");
  encoding = *option->valueRef();
}

static FILE *tryPath(const char *path,const char *fileName)
{
  QCString absName=(path ? (QCString)path+"/"+fileName : (QCString)fileName);
  QFileInfo fi(absName);
  if (fi.exists() && fi.isFile())
  {
    FILE *f=fopen(absName,"r");
    if (!f) config_err("Error: could not open file %s for reading\n",absName.data());
    return f;
  }
  return 0;
}

static void substEnvVarsInStrList(QStrList &sl);
static void substEnvVarsInString(QCString &s);

static bool isAbsolute(const char * fileName)
{
# ifdef _WIN32
  if (isalpha (fileName [0]) && fileName[1] == ':')
    fileName += 2;
# endif
  char const fst = fileName [0];
  if (fst == '/')  {
    return true;
  }
# ifdef _WIN32
  if (fst == '\\')
    return true;
# endif
  return false;
}

static FILE *findFile(const char *fileName)
{
  if(isAbsolute(fileName))
    return tryPath(NULL, fileName);
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
    config_err("Error: maximum include depth (%d) reached, %s is not included. Aborting...\n",
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
    config_err("Error: @INCLUDE = %s: not found!\n",inc.data());
    exit(1);
  }
}


%}

%option nounput
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
					     config_err("Warning: ignoring unsupported tag `%s' at line %d, file %s\n",
						 yytext,yyLineNr,yyFileName.data()); 
					     BEGIN(SkipInvalid);
					   }
					   else // known tag
					   {
					     option->setEncoding(encoding);
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
						 s = ((ConfigBool *)option)->valueStringRef();
					         s->resize(0);
					         BEGIN(GetString);
						 break;
					       case ConfigOption::O_Obsolete:
					         config_err("Warning: Tag `%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", cmd.data(),yyLineNr,yyFileName.data()); 
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"+="	{ QCString cmd=yytext;
                                          cmd=cmd.left(cmd.length()-2).stripWhiteSpace(); 
					  ConfigOption *option = config->get(cmd);
					  if (option==0) // oops not known
					  {
					    config_err("Warning: ignoring unsupported tag `%s' at line %d, file %s\n",
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
						elemStr="";
					        BEGIN(GetStrList);
					        break;
					      case ConfigOption::O_Enum:
					      case ConfigOption::O_String:
					      case ConfigOption::O_Int:
					      case ConfigOption::O_Bool:
					        config_err("Warning: operator += not supported for `%s'. Ignoring line at line %d, file %s\n",
						    yytext,yyLineNr,yyFileName.data()); 
					        BEGIN(SkipInvalid);
						break;
					       case ConfigOption::O_Obsolete:
					         config_err("Warning: Tag `%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", cmd.data(),yyLineNr,yyFileName.data()); 
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>"@INCLUDE_PATH"[ \t]*"=" 	{ BEGIN(GetStrList); l=&includePathList; l->clear(); elemStr=""; }
  /* include a config file */
<Start>"@INCLUDE"[ \t]*"="     		{ BEGIN(Include);}
<Include>([^ \"\t\r\n]+)|("\""[^\n\"]+"\"") { 
  					  readIncludeFile(configStringRecode(yytext,encoding,"UTF-8")); 
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

<Start>[a-z_A-Z0-9]+			{ config_err("Warning: ignoring unknown tag `%s' at line %d, file %s\n",yytext,yyLineNr,yyFileName.data()); }
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
<GetString>[^ \"\t\r\n]+		{ (*s)+=configStringRecode(yytext,encoding,"UTF-8"); 
                                          checkEncoding();
                                        }
<GetString,GetStrList,SkipInvalid>"\""	{ lastState=YY_START;
  					  BEGIN(GetQuotedString); 
                                          tmpString.resize(0); 
					}
<GetQuotedString>"\""|"\n" 		{ 
                                          // we add a bogus space to signal that the string was quoted. This space will be stripped later on.
                                          tmpString+=" ";
  					  //printf("Quoted String = `%s'\n",tmpString.data());
  					  if (lastState==GetString)
					  {
					    (*s)+=configStringRecode(tmpString,encoding,"UTF-8");
                                            checkEncoding();
					  }
					  else
					  {
					    elemStr+=configStringRecode(tmpString,encoding,"UTF-8");
					  }
					  if (*yytext=='\n')
					  {
					    config_err("Warning: Missing end quote (\") on line %d, file %s\n",yyLineNr,yyFileName.data());
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
					    config_warn("Warning: Invalid value `%s' for "
						 "boolean tag in line %d, file %s; use YES or NO\n",
						 bs.data(),yyLineNr,yyFileName.data());
					  }
					}
<GetStrList>[^ \#\"\t\r\n]+		{
  					  elemStr+=configStringRecode(yytext,encoding,"UTF-8");
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

void Config::writeTemplate(QTextStream &t,bool sl,bool upd)
{
  t << "# Doxyfile " << versionString << endl << endl;
  if (!sl)
  {
    t << "# This file describes the settings to be used by the documentation system\n";
    t << "# doxygen (www.doxygen.org) for a project\n";
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
    option->writeTemplate(t,sl,upd);
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
    QCString env=portable_getenv(s.mid(i+2,l-3));
    substEnvVarsInString(env); // recursively expand variables if needed.
    s = s.left(i)+env+s.right(s.length()-i-l);
    p=i+env.length(); // next time start at the end of the expanded string
  }
  s=s.stripWhiteSpace(); // to strip the bogus space that was added when an argument
                         // has quotes
  //printf("substEnvVarInString(%s) end\n",s.data());
}

static void substEnvVarsInStrList(QStrList &sl)
{
  char *s = sl.first();
  while (s)
  {
    QCString result(s);
    // an argument with quotes will have an extra space at the end, so wasQuoted will be TRUE.
    bool wasQuoted = (result.find(' ')!=-1) || (result.find('\t')!=-1);
    // here we strip the quote again
    substEnvVarsInString(result);

    //printf("Result %s was quoted=%d\n",result.data(),wasQuoted);

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
	char c=0;
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
    }
    else // just goto the next element in the list
    {
      sl.insert(sl.at(),result);
      sl.next();
    }
    // remove the old unexpanded string from the list
    int i=sl.at();
    sl.remove(); // current item index changes if the last element is removed.
    if (sl.at()==i)     // not last item
	s = sl.current();
    else                // just removed last item
	s = 0;
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

void ConfigBool::substEnvVars()
{
  substEnvVarsInString(m_valueString);
}

void ConfigInt::substEnvVars()
{
  substEnvVarsInString(m_valueString);
}

void ConfigEnum::substEnvVars()
{
  substEnvVarsInString(m_value);
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

static void cleanUpPaths(QStrList &str)
{
  char *sfp = str.first();
  while (sfp)
  {
    register char *p = sfp;
    if (p)
    {
      char c;
      while ((c=*p))
      {
	if (c=='\\') *p='/';
	p++;
      }
    }
    QCString path = sfp;
    if ((path.at(0)!='/' && (path.length()<=2 || path.at(1)!=':')) ||
	path.at(path.length()-1)!='/'
       )
    {
      QFileInfo fi(path);
      if (fi.exists() && fi.isDir())
      {
	int i = str.at();
	str.remove();
	if (str.at()==i) // did not remove last item
	  str.insert(i,fi.absFilePath()+"/");
	else
	  str.append(fi.absFilePath()+"/");
      }
    }
    sfp = str.next();
  }
}

void Config::check()
{
  //if (!projectName.isEmpty())
  //{
  //  projectName[0]=toupper(projectName[0]);
  //}

  QCString &warnFormat = Config_getString("WARN_FORMAT");
  if (warnFormat.stripWhiteSpace().isEmpty())
  {
    warnFormat="$file:$line $text";
  }
  else
  {
    if (warnFormat.find("$file")==-1)
    {
      config_err("Warning: warning format does not contain a $file tag!\n");
    }
    if (warnFormat.find("$line")==-1)
    {
      config_err("Warning: warning format does not contain a $line tag!\n");
    }
    if (warnFormat.find("$text")==-1)
    {
      config_err("Warning: warning format foes not contain a $text tag!\n");
    }
  }

  QCString &manExtension = Config_getString("MAN_EXTENSION");
  
  // set default man page extension if non is given by the user
  if (manExtension.isEmpty())
  {
    manExtension=".3";
  }
  
  QCString &paperType = Config_getEnum("PAPER_TYPE");
  paperType=paperType.lower().stripWhiteSpace(); 
  if (paperType.isEmpty())
  {
    paperType = "a4wide";
  }
  if (paperType!="a4" && paperType!="a4wide" && paperType!="letter" && 
      paperType!="legal" && paperType!="executive")
  {
    config_err("Error: Unknown page type specified");
  }
  
  QCString &outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
  outputLanguage=outputLanguage.stripWhiteSpace();
  if (outputLanguage.isEmpty())
  {
    outputLanguage = "English";
  }

  QCString &htmlFileExtension=Config_getString("HTML_FILE_EXTENSION");
  htmlFileExtension=htmlFileExtension.stripWhiteSpace();
  if (htmlFileExtension.isEmpty())
  {
    htmlFileExtension = ".html";
  }
  
  // expand the relative stripFromPath values
  QStrList &stripFromPath = Config_getList("STRIP_FROM_PATH");
  char *sfp = stripFromPath.first();
  if (sfp==0) // by default use the current path
  {
    stripFromPath.append(QDir::currentDirPath()+"/");
  }
  else
  {
    cleanUpPaths(stripFromPath);
  }

  // expand the relative stripFromPath values
  QStrList &stripFromIncPath = Config_getList("STRIP_FROM_INC_PATH");
  cleanUpPaths(stripFromIncPath);
  
  // Test to see if HTML header is valid
  QCString &headerFile = Config_getString("HTML_HEADER");
  if (!headerFile.isEmpty())
  {
    QFileInfo fi(headerFile);
    if (!fi.exists())
    {
      config_err("Error: tag HTML_HEADER: header file `%s' "
	  "does not exist\n",headerFile.data());
      exit(1);
    }
  }
  // Test to see if HTML footer is valid
  QCString &footerFile = Config_getString("HTML_FOOTER");
  if (!footerFile.isEmpty())
  {
    QFileInfo fi(footerFile);
    if (!fi.exists())
    {
      config_err("Error: tag HTML_FOOTER: footer file `%s' "
	  "does not exist\n",footerFile.data());
      exit(1);
    }
  }
  // Test to see if LaTeX header is valid
  QCString &latexHeaderFile = Config_getString("LATEX_HEADER");
  if (!latexHeaderFile.isEmpty())
  {
    QFileInfo fi(latexHeaderFile);
    if (!fi.exists())
    {
      config_err("Error: tag LATEX_HEADER: header file `%s' "
	  "does not exist\n",latexHeaderFile.data());
      exit(1);
    }
  }
  // check include path
  QStrList &includePath = Config_getList("INCLUDE_PATH");
  char *s=includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    if (!fi.exists()) config_err("Warning: tag INCLUDE_PATH: include path `%s' "
	                  "does not exist\n",s);
    s=includePath.next();
  }

  // check aliases
  QStrList &aliasList = Config_getList("ALIASES");
  s=aliasList.first();
  while (s)
  {
    QRegExp re1("[a-z_A-Z][a-z_A-Z0-9]*[ \t]*=");         // alias without argument
    QRegExp re2("[a-z_A-Z][a-z_A-Z0-9]*{[0-9]*}[ \t]*="); // alias with argument
    QCString alias=s;
    alias=alias.stripWhiteSpace();
    if (alias.find(re1)!=0 && alias.find(re2)!=0)
    {
      config_err("Illegal alias format `%s'. Use \"name=value\" or \"name(n)=value\", where n is the number of arguments\n",
	  alias.data());
    }
    s=aliasList.next();
  }

  // check dot image format
  QCString &dotImageFormat=Config_getEnum("DOT_IMAGE_FORMAT");
  dotImageFormat=dotImageFormat.stripWhiteSpace();
  if (dotImageFormat.isEmpty())
  {
    dotImageFormat = "png";
  }
  else if (dotImageFormat!="gif" && dotImageFormat!="png" && dotImageFormat!="jpg")
  {
    config_err("Invalid value for DOT_IMAGE_FORMAT: `%s'. Using the default.\n",dotImageFormat.data());
    dotImageFormat = "png";
  }
  
  
  // check dot path
  QCString &dotPath = Config_getString("DOT_PATH");
  if (!dotPath.isEmpty())
  {
    QFileInfo dp(dotPath+"/dot"+portable_commandExtension());
    if (!dp.exists() || !dp.isFile())
    {
      config_err("Warning: the dot tool could not be found at %s\n",dotPath.data());
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

  // check mscgen path
  QCString &mscgenPath = Config_getString("MSCGEN_PATH");
  if (!mscgenPath.isEmpty())
  {
    QFileInfo dp(mscgenPath+"/mscgen"+portable_commandExtension());
    if (!dp.exists() || !dp.isFile())
    {
      config_err("Warning: the mscgen tool could not be found at %s\n",mscgenPath.data());
      mscgenPath="";
    }
    else
    {
      mscgenPath=dp.dirPath(TRUE)+"/";
#if defined(_WIN32) // convert slashes
      uint i=0,l=mscgenPath.length();
      for (i=0;i<l;i++) if (mscgenPath.at(i)=='/') mscgenPath.at(i)='\\';
#endif
    }
  }
  else // make sure the string is empty but not null!
  {
    mscgenPath="";
  }

  
  // check input
  QStrList &inputSources=Config_getList("INPUT");
  if (inputSources.count()==0)
  {
    // use current dir as the default
    inputSources.append(QDir::currentDirPath());
  }
  else
  {
    s=inputSources.first();
    while (s)
    {
      QFileInfo fi(s);
      if (!fi.exists())
      {
	config_err("Warning: tag INPUT: input source `%s' does not exist\n",s);
      }
      s=inputSources.next();
    }
  }

  // add default pattern if needed
  QStrList &filePatternList = Config_getList("FILE_PATTERNS");
  if (filePatternList.isEmpty())
  {
    filePatternList.append("*.c");
    filePatternList.append("*.cc"); 
    filePatternList.append("*.cxx");
    filePatternList.append("*.cpp");
    filePatternList.append("*.c++");
    filePatternList.append("*.d");
    filePatternList.append("*.java");
    filePatternList.append("*.ii");
    filePatternList.append("*.ixx");
    filePatternList.append("*.ipp");
    filePatternList.append("*.i++");
    filePatternList.append("*.inl");
    filePatternList.append("*.h");
    filePatternList.append("*.hh");
    filePatternList.append("*.hxx");
    filePatternList.append("*.hpp");
    filePatternList.append("*.h++");
    filePatternList.append("*.idl");
    filePatternList.append("*.odl");
    filePatternList.append("*.cs");
    filePatternList.append("*.php");
    filePatternList.append("*.php3");
    filePatternList.append("*.inc");
    filePatternList.append("*.m");
    filePatternList.append("*.mm");
    filePatternList.append("*.dox");
    filePatternList.append("*.py");
    filePatternList.append("*.f90");
    filePatternList.append("*.f");
    filePatternList.append("*.vhd");
    filePatternList.append("*.vhdl");
    if (portable_fileSystemIsCaseSensitive())
    {
      // unix => case sensitive match => also include useful uppercase versions
      filePatternList.append("*.C");
      filePatternList.append("*.CC"); 
      filePatternList.append("*.C++");
      filePatternList.append("*.II");
      filePatternList.append("*.I++");
      filePatternList.append("*.H");
      filePatternList.append("*.HH");
      filePatternList.append("*.H++");
      filePatternList.append("*.CS");
      filePatternList.append("*.PHP");
      filePatternList.append("*.PHP3");
      filePatternList.append("*.M");
      filePatternList.append("*.MM");
      filePatternList.append("*.PY");
      filePatternList.append("*.F90");
      filePatternList.append("*.F");
      filePatternList.append("*.VHD");
      filePatternList.append("*.VHDL");
    }
  }

  // add default pattern if needed
  QStrList &examplePatternList = Config_getList("EXAMPLE_PATTERNS");
  if (examplePatternList.isEmpty())
  {
    examplePatternList.append("*");
  }

  // if no output format is enabled, warn the user
  if (!Config_getBool("GENERATE_HTML")    && 
      !Config_getBool("GENERATE_LATEX")   &&
      !Config_getBool("GENERATE_MAN")     && 
      !Config_getBool("GENERATE_RTF")     &&
      !Config_getBool("GENERATE_XML")     &&
      !Config_getBool("GENERATE_PERLMOD") &&
      !Config_getBool("GENERATE_RTF")     &&
      !Config_getBool("GENERATE_AUTOGEN_DEF") &&
      Config_getString("GENERATE_TAGFILE").isEmpty()
     )
  {
    config_err("Warning: No output formats selected! Set at least one of the main GENERATE_* options to YES.\n");
  }

  // check HTMLHELP creation requirements
  if (!Config_getBool("GENERATE_HTML") && 
      Config_getBool("GENERATE_HTMLHELP"))
  {
    config_err("Warning: GENERATE_HTMLHELP=YES requires GENERATE_HTML=YES.\n");
  }

  // check QHP creation requirements
  if (Config_getBool("GENERATE_QHP"))
  {
    if (!Config_getBool("GENERATE_HTML"))
    {
      config_err("Warning: GENERATE_QHP=YES requires GENERATE_HTML=YES.\n");
    }

    if (Config_getString("QHP_NAMESPACE").isEmpty())
    {
      config_err("Warning: GENERATE_QHP=YES requires QHP_NAMESPACE to be set.\n");
    }

    if (Config_getString("QHP_VIRTUAL_FOLDER").isEmpty())
    {
      config_err("Warning: GENERATE_QHP=YES requires QHP_VIRTUAL_FOLDER to be set.\n");
    }
  }

  // check QCH creation requirements
  if (!Config_getString("QHG_LOCATION").isEmpty() &&
      !Config_getBool("GENERATE_QHP"))
  {
    config_err("Warning: Specifying QHG_LOCATION requires GENERATE_QHP=YES.\n");
  }
  if (!Config_getString("QCH_FILE").isEmpty() &&
      Config_getString("QHG_LOCATION").isEmpty())
  {
    config_err("Warning: Specifying QCH_FILE requires QHG_LOCATION to be set.\n");
  }

  // check INDEXLOG creation requirements
  if (!Config_getBool("GENERATE_HTML") && 
      Config_getBool("GENERATE_INDEXLOG"))
  {
    config_err("Warning: GENERATE_INDEXLOG=YES requires GENERATE_HTML=YES.\n");
  }

  if (Config_getBool("HAVE_DOT")) 
  {
    QCString curFontPath = Config_getString("DOT_FONTPATH");
    if (curFontPath.isEmpty())
    {  
      portable_getenv("DOTFONTPATH");
      QCString newFontPath = ".";
      if (!curFontPath.isEmpty())
      {
        newFontPath+=portable_pathListSeparator();
        newFontPath+=curFontPath;
      }
      portable_setenv("DOTFONTPATH",newFontPath);
    }
    else
    {
      portable_setenv("DOTFONTPATH",curFontPath);
    }
  }

  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA") && Config_getBool("INLINE_INFO"))
  {
    // don't show inline info for Java output, since Java has no inline 
    // concept.
    Config_getBool("INLINE_INFO")=FALSE;
  }

  int &depth = Config_getInt("MAX_DOT_GRAPH_DEPTH");
  if (depth==0)
  {
    depth=1000;
  }

  
  // add default words if needed
  QStrList &annotationFromBrief = Config_getList("ABBREVIATE_BRIEF");
  if (annotationFromBrief.isEmpty())
  {
    annotationFromBrief.append("The $name class");
    annotationFromBrief.append("The $name widget");
    annotationFromBrief.append("The $name file");
    annotationFromBrief.append("is");
    annotationFromBrief.append("provides");
    annotationFromBrief.append("specifies");
    annotationFromBrief.append("contains");
    annotationFromBrief.append("represents");
    annotationFromBrief.append("a");
    annotationFromBrief.append("an");
    annotationFromBrief.append("the");
  }

  // some default settings for vhdl
  if (Config_getBool("OPTIMIZE_OUTPUT_VHDL") && 
      (Config_getBool("INLINE_INHERITED_MEMB") || 
       Config_getBool("INHERIT_DOCS") || 
       !Config_getBool("HIDE_SCOPE_NAMES") ||
       !Config_getBool("EXTRACT_PRIVATE")
      )
     )
  {
    bool b1 = Config_getBool("INLINE_INHERITED_MEMB");
    bool b2 = Config_getBool("INHERIT_DOCS");
    bool b3 = Config_getBool("HIDE_SCOPE_NAMES");
    bool b4 = Config_getBool("EXTRACT_PRIVATE");
    const char *s1,*s2,*s3,*s4;
    if (b1)  s1="  INLINDE_INHERITED_MEMB = NO (was YES)\n"; else s1="";
    if (b2)  s2="  INHERIT_DOCS           = NO (was YES)\n"; else s2="";
    if (!b3) s3="  HIDE_SCOPE_NAMES       = YES (was NO)\n"; else s3="";
    if (!b4) s4="  EXTRACT_PRIVATE        = YES (was NO)\n"; else s4="";

    config_err("Warning: enabling OPTIMIZE_OUTPUT_VHDL assumes the following settings:\n"
	       "%s%s%s%s",s1,s2,s3,s4
	      );

    Config_getBool("INLINE_INHERITED_MEMB") = FALSE;
    Config_getBool("INHERIT_DOCS")          = FALSE;
    Config_getBool("HIDE_SCOPE_NAMES")      = TRUE;
    Config_getBool("EXTRACT_PRIVATE")       = TRUE;
  }

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
  addInfo("Project","Project related configuration options");
  //-----------------------------------------------------------------------------------------------
  
  cs = addString(
                  "DOXYFILE_ENCODING",
		  "This tag specifies the encoding used for all characters in the config file \n"
		  "that follow. The default is UTF-8 which is also the encoding used for all \n"
		  "text before the first occurrence of this tag. Doxygen uses libiconv (or the \n"
		  "iconv built into libc) for the transcoding. See \n"
		  "http://www.gnu.org/software/libiconv for the list of possible encodings.\n"
                );
  cs->setDefaultValue("UTF-8");
  cs = addString(
                 "PROJECT_NAME",
                 "The PROJECT_NAME tag is a single word (or a sequence of words surrounded \n"
                 "by quotes) that should identify the project. "
                );
  cs = addString(
                 "PROJECT_NUMBER",
                 "The PROJECT_NUMBER tag can be used to enter a project or revision number. \n"
                 "This could be handy for archiving the generated documentation or \n"
                 "if some version control system is used.\n"
                );
  cs = addString(
                 "OUTPUT_DIRECTORY",
                 "The OUTPUT_DIRECTORY tag is used to specify the (relative or absolute) \n"
                 "base path where the generated documentation will be put. \n"
                 "If a relative path is entered, it will be relative to the location \n"
                 "where doxygen was started. If left blank the current directory will be used.\n"
		);
  cs->setWidgetType(ConfigString::Dir);
  cb = addBool(
                 "CREATE_SUBDIRS",
                 "If the CREATE_SUBDIRS tag is set to YES, then doxygen will create \n"
                 "4096 sub-directories (in 2 levels) under the output directory of each output \n"
		 "format and will distribute the generated files over these directories. \n"
		 "Enabling this option can be useful when feeding doxygen a huge amount of \n"
		 "source files, where putting all generated files in the same directory would \n"
		 "otherwise cause performance problems for the file system. \n",
		 FALSE
                );
  ce = addEnum(
                 "OUTPUT_LANGUAGE",
                 "The OUTPUT_LANGUAGE tag is used to specify the language in which all \n"
                 "documentation generated by doxygen is written. Doxygen will use this \n"
                 "information to generate all constant output in the proper language. \n"
                 "The default language is English, other supported languages are: \n"
		 "Afrikaans, Arabic, Brazilian, Catalan, Chinese, Chinese-Traditional, \n"
		 "Croatian, Czech, Danish, Dutch, Farsi, Finnish, French, German, Greek, \n"
		 "Hungarian, Italian, Japanese, Japanese-en (Japanese with English messages), \n"
		 "Korean, Korean-en, Lithuanian, Norwegian, Macedonian, Persian, Polish, \n"
		 "Portuguese, Romanian, Russian, Serbian, Serbian-Cyrilic, Slovak, Slovene, \n"
		 "Spanish, Swedish, and Ukrainian.\n", 
		 "English"
                );
#ifdef LANG_ZA
  ce->addValue("Afrikaans");
#endif
#ifdef LANG_AR
  ce->addValue("Arabic");
#endif
#ifdef LANG_BR
  ce->addValue("Brazilian");
#endif
#ifdef LANG_CA
  ce->addValue("Catalan");
#endif
#ifdef LANG_CN
  ce->addValue("Chinese");
#endif
#ifdef LANG_TW
  ce->addValue("Chinese-Traditional");
#endif
#ifdef LANG_HR
  ce->addValue("Croatian");
#endif
#ifdef LANG_CZ
  ce->addValue("Czech");
#endif
#ifdef LANG_DK
  ce->addValue("Danish");
#endif
#ifdef LANG_NL
  ce->addValue("Dutch");
#endif
  ce->addValue("English");
#ifdef LANG_FI
  ce->addValue("Finnish");
#endif
#ifdef LANG_FR
  ce->addValue("French");
#endif
#ifdef LANG_DE
  ce->addValue("German");
#endif
#ifdef LANG_GR
  ce->addValue("Greek");
#endif
#ifdef LANG_HU
  ce->addValue("Hungarian");
#endif
#ifdef LANG_IT
  ce->addValue("Italian");
#endif
#ifdef LANG_JP
  ce->addValue("Japanese");
  ce->addValue("Japanese-en");
#endif
#ifdef LANG_KR
  ce->addValue("Korean");
  ce->addValue("Korean-en");
#endif
#ifdef LANG_LI
  ce->addValue("Lithuanian");
#endif
#ifdef LANG_NO
  ce->addValue("Norwegian");
#endif
#ifdef LANG_MK
  ce->addValue("Macedonian");
#endif
#ifdef LANG_FA
  ce->addValue("Farsi");
  ce->addValue("Persian");
#endif
#ifdef LANG_PL
  ce->addValue("Polish");
#endif
#ifdef LANG_PT
  ce->addValue("Portuguese");
#endif
#ifdef LANG_RO
  ce->addValue("Romanian");
#endif
#ifdef LANG_RU
  ce->addValue("Russian");
#endif
#ifdef LANG_SR
  ce->addValue("Serbian");
#endif
#ifdef LANG_SC
  ce->addValue("Serbian-Cyrilic");
#endif
#ifdef LANG_SK
  ce->addValue("Slovak");
#endif
#ifdef LANG_SI
  ce->addValue("Slovene");
#endif
#ifdef LANG_ES
  ce->addValue("Spanish");
#endif
#ifdef LANG_SE
  ce->addValue("Swedish");
#endif
#ifdef LANG_UA
  ce->addValue("Ukrainian");
#endif

#if 0
  cb = addBool(
                    "USE_WINDOWS_ENCODING",
		    "This tag can be used to specify the encoding used in the generated output. \n"
		    "The encoding is not always determined by the language that is chosen, \n"
		    "but also whether or not the output is meant for Windows or non-Windows users. \n"
		    "In case there is a difference, setting the USE_WINDOWS_ENCODING tag to YES \n"
		    "forces the Windows encoding (this is the default for the Windows binary), \n"
		    "whereas setting the tag to NO uses a Unix-style encoding (the default for \n"
		    "all platforms other than Windows).\n",
#if defined(_WIN32) || defined(__CYGWIN__)
		    TRUE
#else
		    FALSE
#endif
		 );
#endif
  addObsolete("USE_WINDOWS_ENCODING");

  cb = addBool(
                    "BRIEF_MEMBER_DESC",
                    "If the BRIEF_MEMBER_DESC tag is set to YES (the default) Doxygen will \n"
                    "include brief member descriptions after the members that are listed in \n"
                    "the file and class documentation (similar to JavaDoc). \n"
                    "Set to NO to disable this. \n",
                    TRUE
                 );
  cb = addBool(
                    "REPEAT_BRIEF",
                    "If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend \n"
                    "the brief description of a member or function before the detailed description. \n"
                    "Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the \n"
                    "brief descriptions will be completely suppressed. \n",
                    TRUE
                 );
  cl = addList(
                    "ABBREVIATE_BRIEF",
                    "This tag implements a quasi-intelligent brief description abbreviator \n"
                    "that is used to form the text in various listings. Each string \n"
                    "in this list, if found as the leading text of the brief description, will be \n"
                    "stripped from the text and the result after processing the whole list, is \n"
		    "used as the annotated text. Otherwise, the brief description is used as-is. \n"
		    "If left blank, the following values are used (\"$name\" is automatically \n"
		    "replaced with the name of the entity): \"The $name class\" \"The $name widget\" \n"
		    "\"The $name file\" \"is\" \"provides\" \"specifies\" \"contains\" \n"
		    "\"represents\" \"a\" \"an\" \"the\"\n"
                 );
  cb = addBool(
                    "ALWAYS_DETAILED_SEC",
                    "If the ALWAYS_DETAILED_SEC and REPEAT_BRIEF tags are both set to YES then \n"
                    "Doxygen will generate a detailed section even if there is only a brief \n"
                    "description. \n",
                    FALSE
                 );
  cb = addBool(
                    "INLINE_INHERITED_MEMB",
                    "If the INLINE_INHERITED_MEMB tag is set to YES, doxygen will show all \n"
		    "inherited members of a class in the documentation of that class as if those \n"
		    "members were ordinary class members. Constructors, destructors and assignment \n"
		    "operators of the base classes will not be shown. \n",
		    FALSE
                 );
  cb = addBool(
                    "FULL_PATH_NAMES",
                    "If the FULL_PATH_NAMES tag is set to YES then Doxygen will prepend the full \n"
                    "path before files name in the file list and in the header files. If set \n"
                    "to NO the shortest path that makes the file name unique will be used. \n",
                    TRUE
                 );
  cl = addList(
                    "STRIP_FROM_PATH",
                    "If the FULL_PATH_NAMES tag is set to YES then the STRIP_FROM_PATH tag \n"
                    "can be used to strip a user-defined part of the path. Stripping is \n"
                    "only done if one of the specified strings matches the left-hand part of \n"
                    "the path. The tag can be used to show relative paths in the file list. \n"
		    "If left blank the directory from which doxygen is run is used as the \n"
		    "path to strip. \n"
                 );
  cl->addDependency("FULL_PATH_NAMES");
  cl = addList(
                    "STRIP_FROM_INC_PATH",
                    "The STRIP_FROM_INC_PATH tag can be used to strip a user-defined part of \n"
		    "the path mentioned in the documentation of a class, which tells \n"
		    "the reader which header file to include in order to use a class. \n"
		    "If left blank only the name of the header file containing the class \n"
		    "definition is used. Otherwise one should specify the include paths that \n"
		    "are normally passed to the compiler using the -I flag.\n"
                 );
  cb = addBool(
                    "SHORT_NAMES",
		    "If the SHORT_NAMES tag is set to YES, doxygen will generate much shorter \n"
		    "(but less readable) file names. This can be useful is your file systems \n"
		    "doesn't support long names like on DOS, Mac, or CD-ROM. \n",
		    FALSE
                 );
  cb = addBool(
                    "JAVADOC_AUTOBRIEF",
                    "If the JAVADOC_AUTOBRIEF tag is set to YES then Doxygen \n"
                    "will interpret the first line (until the first dot) of a JavaDoc-style \n"
                    "comment as the brief description. If set to NO, the JavaDoc \n"
                    "comments will behave just like regular Qt-style comments \n"
                    "(thus requiring an explicit @brief command for a brief description.) \n",
                    FALSE
                 );
  cb = addBool(
                    "QT_AUTOBRIEF",
                    "If the QT_AUTOBRIEF tag is set to YES then Doxygen will \n"
                    "interpret the first line (until the first dot) of a Qt-style \n"
                    "comment as the brief description. If set to NO, the comments \n"
                    "will behave just like regular Qt-style comments (thus requiring \n"
                    "an explicit \\brief command for a brief description.) \n",
                    FALSE
                 );
  cb = addBool(
                    "MULTILINE_CPP_IS_BRIEF",
                    "The MULTILINE_CPP_IS_BRIEF tag can be set to YES to make Doxygen \n"
                    "treat a multi-line C++ special comment block (i.e. a block of //! or /// \n"
		    "comments) as a brief description. This used to be the default behaviour. \n"
		    "The new default is to treat a multi-line C++ comment block as a detailed \n"
		    "description. Set this tag to YES if you prefer the old behaviour instead. \n",
                    FALSE
                 );
  //cb = addBool(
  //                  "DETAILS_AT_TOP",
  //                  "If the DETAILS_AT_TOP tag is set to YES then Doxygen \n"
  //                  "will output the detailed description near the top, like JavaDoc.\n"
  //                  "If set to NO, the detailed description appears after the member \n"
  //                  "documentation. \n",
  //                  FALSE
  //               );
  cb = addBool(
                    "INHERIT_DOCS",
                    "If the INHERIT_DOCS tag is set to YES (the default) then an undocumented \n"
                    "member inherits the documentation from any documented member that it \n"
                    "re-implements. \n",
                    TRUE
                 );
  cb = addBool(    
                    "SEPARATE_MEMBER_PAGES",
		    "If the SEPARATE_MEMBER_PAGES tag is set to YES, then doxygen will produce \n"
		    "a new page for each member. If set to NO, the documentation of a member will \n"
		    "be part of the file/class/namespace that contains it.\n",
		    FALSE
		 );
  ci = addInt(
                    "TAB_SIZE",
                    "The TAB_SIZE tag can be used to set the number of spaces in a tab. \n"
                    "Doxygen uses this value to replace tabs by spaces in code fragments. \n",
                    1,16,8
                 );
  cl = addList(
                    "ALIASES",
                    "This tag can be used to specify a number of aliases that acts \n"
                    "as commands in the documentation. An alias has the form \"name=value\". \n"
                    "For example adding \"sideeffect=\\par Side Effects:\\n\" will allow you to \n"
                    "put the command \\sideeffect (or @sideeffect) in the documentation, which \n"
                    "will result in a user-defined paragraph with heading \"Side Effects:\". \n"
                    "You can put \\n's in the value part of an alias to insert newlines. \n" 
                 );
  cb = addBool(
                    "OPTIMIZE_OUTPUT_FOR_C",
                    "Set the OPTIMIZE_OUTPUT_FOR_C tag to YES if your project consists of C \n"
		    "sources only. Doxygen will then generate output that is more tailored for C. \n"
                    "For instance, some of the names that are used will be different. The list \n"
                    "of all members will be omitted, etc. \n",
                    FALSE
                 );
  cb = addBool(
                    "OPTIMIZE_OUTPUT_JAVA",
                    "Set the OPTIMIZE_OUTPUT_JAVA tag to YES if your project consists of Java \n"
		    "sources only. Doxygen will then generate output that is more tailored for \n"
		    "Java. For instance, namespaces will be presented as packages, qualified \n"
		    "scopes will look different, etc. \n",
                    FALSE
                 );
  cb = addBool(
                    "OPTIMIZE_FOR_FORTRAN",
                    "Set the OPTIMIZE_FOR_FORTRAN tag to YES if your project consists of Fortran \n"
                    "sources only. Doxygen will then generate output that is more tailored for \n"
		    "Fortran. \n",
                    FALSE
                 );
  cb = addBool(
                    "OPTIMIZE_OUTPUT_VHDL",
                    "Set the OPTIMIZE_OUTPUT_VHDL tag to YES if your project consists of VHDL \n"
                    "sources. Doxygen will then generate output that is tailored for \n"
		    "VHDL. \n",
                    FALSE
                 );
  cb = addBool(  
                    "BUILTIN_STL_SUPPORT",
		    "If you use STL classes (i.e. std::string, std::vector, etc.) but do not want \n"
		    "to include (a tag file for) the STL sources as input, then you should \n"
		    "set this tag to YES in order to let doxygen match functions declarations and \n"
		    "definitions whose arguments contain STL classes (e.g. func(std::string); v.s. \n"
		    "func(std::string) {}). This also make the inheritance and collaboration \n"
		    "diagrams that involve STL classes more complete and accurate. \n",
		    FALSE
		 );
  cb = addBool(
                    "CPP_CLI_SUPPORT",
		    "If you use Microsoft's C++/CLI language, you should set this option to YES to\n"
		    "enable parsing support.\n",
		    FALSE
      	         );
  cb = addBool(
                    "SIP_SUPPORT",
                    "Set the SIP_SUPPORT tag to YES if your project consists of sip sources only. \n"
		    "Doxygen will parse them like normal C++ but will assume all classes use public \n"
		    "instead of private inheritance when no explicit protection keyword is present. \n",
                    FALSE
                 );
  cb = addBool(    "IDL_PROPERTY_SUPPORT",
                   "For Microsoft's IDL there are propget and propput attributes to indicate getter \n"
		   "and setter methods for a property. Setting this option to YES (the default) \n"
		   "will make doxygen to replace the get and set methods by a property in the \n"
		   "documentation. This will only work if the methods are indeed getting or \n"
		   "setting a simple type. If this is not the case, or you want to show the \n"
		   "methods anyway, you should set this option to NO. \n",
		   TRUE
		  );
  cb = addBool(
                    "DISTRIBUTE_GROUP_DOC",
                    "If member grouping is used in the documentation and the DISTRIBUTE_GROUP_DOC \n"
                    "tag is set to YES, then doxygen will reuse the documentation of the first \n"
                    "member in the group (if any) for the other members of the group. By default \n"
                    "all members of a group must be documented explicitly.\n",
                    FALSE
                 );
  cb = addBool(    
                    "SUBGROUPING",
		    "Set the SUBGROUPING tag to YES (the default) to allow class member groups of \n"
		    "the same type (for instance a group of public functions) to be put as a \n"
		    "subgroup of that type (e.g. under the Public Functions section). Set it to \n"
		    "NO to prevent subgrouping. Alternatively, this can be done per class using \n"
		    "the \\nosubgrouping command. \n",
		    TRUE
                );
  cb = addBool(     "TYPEDEF_HIDES_STRUCT",
                    "When TYPEDEF_HIDES_STRUCT is enabled, a typedef of a struct, union, or enum \n"
		    "is documented as struct, union, or enum with the name of the typedef. So \n"
		    "typedef struct TypeS {} TypeT, will appear in the documentation as a struct \n"
		    "with name TypeT. When disabled the typedef will appear as a member of a file, \n"
		    "namespace, or class. And the struct will be named TypeS. This can typically \n"
		    "be useful for C code in case the coding convention dictates that all compound \n"
		    "types are typedef'ed and only the typedef is referenced, never the tag name.\n",
		    FALSE
                );
  ci = addInt(
                    "SYMBOL_CACHE_SIZE",
                    "The SYMBOL_CACHE_SIZE determines the size of the internal cache use to \n"
		    "determine which symbols to keep in memory and which to flush to disk.\n"
		    "When the cache is full, less often used symbols will be written to disk.\n"
		    "For small to medium size projects (<1000 input files) the default value is \n"
		    "probably good enough. For larger projects a too small cache size can cause \n"
		    "doxygen to be busy swapping symbols to and from disk most of the time \n"
		    "causing a significant performance penality. \n"
		    "If the system has enough physical memory increasing the cache will improve the \n"
		    "performance by keeping more symbols in memory. Note that the value works on \n"
		    "a logarithmic scale so increasing the size by one will rougly double the \n"
		    "memory usage. The cache size is given by this formula: \n"
		    "2^(16+SYMBOL_CACHE_SIZE). The valid range is 0..9, the default is 0, \n"
		    "corresponding to a cache size of 2^16 = 65536 symbols\n",
                    0,9,0
             );

  //-----------------------------------------------------------------------------------------------
  addInfo("Build","Build related configuration options");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "EXTRACT_ALL",
                    "If the EXTRACT_ALL tag is set to YES doxygen will assume all entities in \n"
                    "documentation are documented, even if no documentation was available. \n"
                    "Private class members and static file members will be hidden unless \n"
                    "the EXTRACT_PRIVATE and EXTRACT_STATIC tags are set to YES \n",
                    FALSE
                 );
  cb = addBool(
                    "EXTRACT_PRIVATE",
                    "If the EXTRACT_PRIVATE tag is set to YES all private members of a class \n"
                    "will be included in the documentation. \n",
                    FALSE
                 );
  cb = addBool(
                    "EXTRACT_STATIC",
                    "If the EXTRACT_STATIC tag is set to YES all static members of a file \n"
                    "will be included in the documentation. \n",
                    FALSE
                 );
  cb = addBool(
                    "EXTRACT_LOCAL_CLASSES",
                    "If the EXTRACT_LOCAL_CLASSES tag is set to YES classes (and structs) \n"
		    "defined locally in source files will be included in the documentation. \n"
		    "If set to NO only classes defined in header files are included. \n",
                    TRUE
                 );
  cb = addBool(
                    "EXTRACT_LOCAL_METHODS",
                    "This flag is only useful for Objective-C code. When set to YES local \n"
		    "methods, which are defined in the implementation section but not in \n"
		    "the interface are included in the documentation. \n"
		    "If set to NO (the default) only methods in the interface are included. \n",
                    FALSE
                 );
  cb = addBool(
                    "EXTRACT_ANON_NSPACES",
                    "If this flag is set to YES, the members of anonymous namespaces will be \n"
		    "extracted and appear in the documentation as a namespace called \n"
		    "'anonymous_namespace{file}', where file will be replaced with the base \n"
		    "name of the file that contains the anonymous namespace. By default \n"
		    "anonymous namespace are hidden. \n",
                   FALSE 
                 );
  cb = addBool(
                    "HIDE_UNDOC_MEMBERS",
                    "If the HIDE_UNDOC_MEMBERS tag is set to YES, Doxygen will hide all \n"
                    "undocumented members of documented classes, files or namespaces. \n"
                    "If set to NO (the default) these members will be included in the \n"
                    "various overviews, but no documentation section is generated. \n"
                    "This option has no effect if EXTRACT_ALL is enabled. \n",
                    FALSE
                 );
  cb = addBool(
                    "HIDE_UNDOC_CLASSES",
                    "If the HIDE_UNDOC_CLASSES tag is set to YES, Doxygen will hide all \n"
                    "undocumented classes that are normally visible in the class hierarchy. \n"
                    "If set to NO (the default) these classes will be included in the various \n"
                    "overviews. This option has no effect if EXTRACT_ALL is enabled. \n",
                    FALSE
                 );
  cb = addBool(
                    "HIDE_FRIEND_COMPOUNDS",
                    "If the HIDE_FRIEND_COMPOUNDS tag is set to YES, Doxygen will hide all \n"
                    "friend (class|struct|union) declarations. \n"
                    "If set to NO (the default) these declarations will be included in the \n"
                    "documentation. \n",
                    FALSE
                 );
  cb = addBool(
                    "HIDE_IN_BODY_DOCS",
                    "If the HIDE_IN_BODY_DOCS tag is set to YES, Doxygen will hide any \n"
                    "documentation blocks found inside the body of a function. \n"
                    "If set to NO (the default) these blocks will be appended to the \n"
                    "function's detailed documentation block. \n",
                    FALSE
                 );
  cb = addBool(
                    "INTERNAL_DOCS",
                    "The INTERNAL_DOCS tag determines if documentation \n"
                    "that is typed after a \\internal command is included. If the tag is set \n"
                    "to NO (the default) then the documentation will be excluded. \n"
                    "Set it to YES to include the internal documentation. \n",
                    FALSE
                 );
  cb = addBool(
                    "CASE_SENSE_NAMES",
                    "If the CASE_SENSE_NAMES tag is set to NO then Doxygen will only generate \n"
                    "file names in lower-case letters. If set to YES upper-case letters are also \n"
                    "allowed. This is useful if you have classes or files whose names only differ \n"
                    "in case and if your file system supports case sensitive file names. Windows \n"
                    "and Mac users are advised to set this option to NO.\n",
		    portable_fileSystemIsCaseSensitive()
                 );
  cb = addBool(
                    "HIDE_SCOPE_NAMES",
                    "If the HIDE_SCOPE_NAMES tag is set to NO (the default) then Doxygen \n"
                    "will show members with their full class and namespace scopes in the \n"
                    "documentation. If set to YES the scope will be hidden. \n",
                    FALSE
                 );
  cb = addBool(
                    "SHOW_INCLUDE_FILES",
                    "If the SHOW_INCLUDE_FILES tag is set to YES (the default) then Doxygen \n"
                    "will put a list of the files that are included by a file in the documentation \n"
                    "of that file. \n",
                    TRUE
                 );
  cb = addBool(
                    "INLINE_INFO",
                    "If the INLINE_INFO tag is set to YES (the default) then a tag [inline] \n"
                    "is inserted in the documentation for inline members. \n",
                    TRUE
                 );
  cb = addBool(
                    "SORT_MEMBER_DOCS",
                    "If the SORT_MEMBER_DOCS tag is set to YES (the default) then doxygen \n"
                    "will sort the (detailed) documentation of file and class members \n"
                    "alphabetically by member name. If set to NO the members will appear in \n"
                    "declaration order. \n",
                    TRUE
                 );
  cb = addBool(
                    "SORT_BRIEF_DOCS",
                    "If the SORT_BRIEF_DOCS tag is set to YES then doxygen will sort the \n"
                    "brief documentation of file, namespace and class members alphabetically \n"
                    "by member name. If set to NO (the default) the members will appear in \n"
                    "declaration order. \n",
                    FALSE
                 );
  cb = addBool(
                    "SORT_GROUP_NAMES",
                    "If the SORT_GROUP_NAMES tag is set to YES then doxygen will sort the \n"
                    "hierarchy of group names into alphabetical order. If set to NO (the default) \n"
                    "the group names will appear in their defined order. \n",
                    FALSE
                 );
  cb = addBool(
                    "SORT_BY_SCOPE_NAME",
                    "If the SORT_BY_SCOPE_NAME tag is set to YES, the class list will be \n"
                    "sorted by fully-qualified names, including namespaces. If set to \n"
		    "NO (the default), the class list will be sorted only by class name, \n"
		    "not including the namespace part. \n"
                    "Note: This option is not very useful if HIDE_SCOPE_NAMES is set to YES.\n"
                    "Note: This option applies only to the class list, not to the \n"
                    "alphabetical list.\n",
                    FALSE
                 );

  cb = addBool(
                    "GENERATE_TODOLIST",
                    "The GENERATE_TODOLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the todo list. This list is created by putting \\todo \n"
                    "commands in the documentation.\n",
                    TRUE
                 );
  cb = addBool(
                    "GENERATE_TESTLIST",
                    "The GENERATE_TESTLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the test list. This list is created by putting \\test \n"
                    "commands in the documentation.\n",
                    TRUE
                 );
  cb = addBool(
                    "GENERATE_BUGLIST",
                    "The GENERATE_BUGLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the bug list. This list is created by putting \\bug \n"
                    "commands in the documentation.\n",
                    TRUE
                 );
  cb = addBool(
                    "GENERATE_DEPRECATEDLIST",
                    "The GENERATE_DEPRECATEDLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the deprecated list. This list is created by putting \n"
		    "\\deprecated commands in the documentation.\n",
                    TRUE
	         );
  cl = addList(
                    "ENABLED_SECTIONS",
                    "The ENABLED_SECTIONS tag can be used to enable conditional \n"
                    "documentation sections, marked by \\if sectionname ... \\endif. \n"
                 );
  ci = addInt(
                    "MAX_INITIALIZER_LINES",
                    "The MAX_INITIALIZER_LINES tag determines the maximum number of lines \n"
                    "the initial value of a variable or define consists of for it to appear in \n"
                    "the documentation. If the initializer consists of more lines than specified \n"
                    "here it will be hidden. Use a value of 0 to hide initializers completely. \n"
                    "The appearance of the initializer of individual variables and defines in the \n"
                    "documentation can be controlled using \\showinitializer or \\hideinitializer \n"
                    "command in the documentation regardless of this setting. \n",
                    0,10000,30
                 );
  cb = addBool(
                    "SHOW_USED_FILES",
                    "Set the SHOW_USED_FILES tag to NO to disable the list of files generated \n"
                    "at the bottom of the documentation of classes and structs. If set to YES the \n"
                    "list will mention the files that were used to generate the documentation. \n",
                    TRUE
                );
  cb = addBool(
                    "SHOW_DIRECTORIES",
		    "If the sources in your project are distributed over multiple directories \n"
		    "then setting the SHOW_DIRECTORIES tag to YES will show the directory hierarchy \n"
		    "in the documentation. The default is NO.\n",
		    FALSE
              );
  cb = addBool(
                    "SHOW_FILES",
                    "Set the SHOW_FILES tag to NO to disable the generation of the Files page.\n"
                    "This will remove the Files entry from the Quick Index and from the \n"
                    "Folder Tree View (if specified). The default is YES.\n",
                    TRUE
                );
  cb = addBool(
                    "SHOW_NAMESPACES",
                    "Set the SHOW_NAMESPACES tag to NO to disable the generation of the \n"
                    "Namespaces page.  This will remove the Namespaces entry from the Quick Index\n"
                    "and from the Folder Tree View (if specified). The default is YES.\n",
                    TRUE
                );
  cs = addString(  "FILE_VERSION_FILTER",
                   "The FILE_VERSION_FILTER tag can be used to specify a program or script that \n"
		   "doxygen should invoke to get the current version for each file (typically from \n"
		   "the version control system). Doxygen will invoke the program by executing (via \n"
		   "popen()) the command <command> <input-file>, where <command> is the value of \n"
		   "the FILE_VERSION_FILTER tag, and <input-file> is the name of an input file \n"
		   "provided by doxygen. Whatever the program writes to standard output \n"
		   "is used as the file version. See the manual for examples. \n"
              );
  cs->setWidgetType(ConfigString::File);
  cs = addString(  "LAYOUT_FILE",
                   "The LAYOUT_FILE tag can be used to specify a layout file which will be parsed by \n"
		   "doxygen. The layout file controls the global structure of the generated output files \n"
		   "in an output format independent way. The create the layout file that represents \n"
		   "doxygen's defaults, run doxygen with the -l option. You can optionally specify a \n"
		   "file name after the option, if omitted DoxygenLayout.xml will be used as the name \n"
		   "of the layout file.\n"
	      );
  cs->setWidgetType(ConfigString::File);
  addObsolete("DETAILS_AT_TOP");
  addObsolete("ALPHABETICAL_INDEX");

  
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Messages","configuration options related to warning and progress messages");
  //-----------------------------------------------------------------------------------------------

  cb = addBool(
                    "QUIET",
                    "The QUIET tag can be used to turn on/off the messages that are generated \n"
                    "by doxygen. Possible values are YES and NO. If left blank NO is used. \n",
                    FALSE
                 );
  cb = addBool(
                    "WARNINGS",
                    "The WARNINGS tag can be used to turn on/off the warning messages that are \n"
                    "generated by doxygen. Possible values are YES and NO. If left blank \n"
                    "NO is used. \n",
                    TRUE
                 );
  cb = addBool(
                    "WARN_IF_UNDOCUMENTED",
                    "If WARN_IF_UNDOCUMENTED is set to YES, then doxygen will generate warnings \n"
                    "for undocumented members. If EXTRACT_ALL is set to YES then this flag will \n"
                    "automatically be disabled. \n",
                    TRUE
                 );
  cb = addBool(
                    "WARN_IF_DOC_ERROR",
                    "If WARN_IF_DOC_ERROR is set to YES, doxygen will generate warnings for \n"
                    "potential errors in the documentation, such as not documenting some \n"
		    "parameters in a documented function, or documenting parameters that \n"
		    "don't exist or using markup commands wrongly. \n",
                    TRUE
                 );
  cb = addBool(     "WARN_NO_PARAMDOC",
                    "This WARN_NO_PARAMDOC option can be abled to get warnings for \n"
		    "functions that are documented, but have no documentation for their parameters \n"
		    "or return value. If set to NO (the default) doxygen will only warn about \n"
		    "wrong or incomplete parameter documentation, but not about the absence of \n"
		    "documentation.\n",
                    FALSE
                 );
  cs = addString(
                    "WARN_FORMAT",
                    "The WARN_FORMAT tag determines the format of the warning messages that \n"
                    "doxygen can produce. The string should contain the $file, $line, and $text \n"
                    "tags, which will be replaced by the file and line number from which the \n"
                    "warning originated and the warning text. Optionally the format may contain \n"
		    "$version, which will be replaced by the version of the file (if it could \n"
		    "be obtained via FILE_VERSION_FILTER)\n"
                   ); 
  cs->setDefaultValue("$file:$line: $text");
  cs = addString(
                    "WARN_LOGFILE",
                    "The WARN_LOGFILE tag can be used to specify a file to which warning \n"
                    "and error messages should be written. If left blank the output is written \n"
                    "to stderr. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Input","configuration options related to the input files");
  //-----------------------------------------------------------------------------------------------
  cl = addList(
                    "INPUT",
                    "The INPUT tag can be used to specify the files and/or directories that contain \n"
                    "documented source files. You may enter file names like \"myfile.cpp\" or \n"
                    "directories like \"/usr/src/myproject\". Separate the files or directories \n"
                    "with spaces. \n"
                 );
  cl->setWidgetType(ConfigList::FileAndDir);
  cs = addString(   "INPUT_ENCODING",
                    "This tag can be used to specify the character encoding of the source files \n"
		    "that doxygen parses. Internally doxygen uses the UTF-8 encoding, which is \n"
		    "also the default input encoding. Doxygen uses libiconv (or the iconv built \n"
		    "into libc) for the transcoding. See http://www.gnu.org/software/libiconv for \n"
		    "the list of possible encodings.\n"
                );
  cs->setDefaultValue("UTF-8");
  cl = addList(
                    "FILE_PATTERNS",
                    "If the value of the INPUT tag contains directories, you can use the \n"
                    "FILE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n"
                    "and *.h) to filter out the source-files in the directories. If left \n"
                    "blank the following patterns are tested: \n"
		    "*.c *.cc *.cxx *.cpp *.c++ *.java *.ii *.ixx *.ipp *.i++ *.inl *.h *.hh *.hxx \n"
		    "*.hpp *.h++ *.idl *.odl *.cs *.php *.php3 *.inc *.m *.mm *.py *.f90\n"
                 );
  cb = addBool(
                    "RECURSIVE",
                    "The RECURSIVE tag can be used to turn specify whether or not subdirectories \n"
                    "should be searched for input files as well. Possible values are YES and NO. \n"
                    "If left blank NO is used. \n",
                    FALSE
                 );
  cl = addList(
                    "EXCLUDE",
                    "The EXCLUDE tag can be used to specify files and/or directories that should \n"
                    "excluded from the INPUT source files. This way you can easily exclude a \n"
                    "subdirectory from a directory tree whose root is specified with the INPUT tag. \n"
                 );
  cb = addBool(
                    "EXCLUDE_SYMLINKS",
                    "The EXCLUDE_SYMLINKS tag can be used select whether or not files or \n"
		    "directories that are symbolic links (a Unix filesystem feature) are excluded \n"
		    "from the input. \n",
                    FALSE
                 );
  cl->setWidgetType(ConfigList::FileAndDir);
  cl = addList(
                    "EXCLUDE_PATTERNS",
                    "If the value of the INPUT tag contains directories, you can use the \n"
                    "EXCLUDE_PATTERNS tag to specify one or more wildcard patterns to exclude \n"
                    "certain files from those directories. Note that the wildcards are matched \n"
		    "against the file with absolute path, so to exclude all test directories \n"
		    "for example use the pattern */test/* \n"
                 );
  cl = addList(
                    "EXCLUDE_SYMBOLS",
                    "The EXCLUDE_SYMBOLS tag can be used to specify one or more symbol names \n"
		    "(namespaces, classes, functions, etc.) that should be excluded from the \n"
		    "output. The symbol name can be a fully qualified name, a word, or if the \n"
		    "wildcard * is used, a substring. Examples: ANamespace, AClass, \n"
		    "AClass::ANamespace, ANamespace::*Test \n"
                 );
  cl = addList(
                    "EXAMPLE_PATH",
                    "The EXAMPLE_PATH tag can be used to specify one or more files or \n"
                    "directories that contain example code fragments that are included (see \n"
                    "the \\include command). \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cl = addList(
                    "EXAMPLE_PATTERNS",
                    "If the value of the EXAMPLE_PATH tag contains directories, you can use the \n"
                    "EXAMPLE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n"
                    "and *.h) to filter out the source-files in the directories. If left \n"
                    "blank all files are included. \n"
                 );
  cb = addBool(
                    "EXAMPLE_RECURSIVE",
                    "If the EXAMPLE_RECURSIVE tag is set to YES then subdirectories will be \n"
                    "searched for input files to be used with the \\include or \\dontinclude \n"
                    "commands irrespective of the value of the RECURSIVE tag. \n"
                    "Possible values are YES and NO. If left blank NO is used. \n",
                    FALSE
                 );
  cl = addList(
                    "IMAGE_PATH",
                    "The IMAGE_PATH tag can be used to specify one or more files or \n"
                    "directories that contain image that are included in the documentation (see \n"
                    "the \\image command). \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cs = addString(
                    "INPUT_FILTER",
                    "The INPUT_FILTER tag can be used to specify a program that doxygen should \n"
                    "invoke to filter for each input file. Doxygen will invoke the filter program \n"
                    "by executing (via popen()) the command <filter> <input-file>, where <filter> \n"
                    "is the value of the INPUT_FILTER tag, and <input-file> is the name of an \n"
                    "input file. Doxygen will then use the output that the filter program writes \n"
                    "to standard output.  If FILTER_PATTERNS is specified, this tag will be \n"
                    "ignored. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cl = addList(
                    "FILTER_PATTERNS",
                    "The FILTER_PATTERNS tag can be used to specify filters on a per file pattern \n"
                    "basis.  Doxygen will compare the file name with each pattern and apply the \n"
                    "filter if there is a match.  The filters are a list of the form: \n"
                    "pattern=filter (like *.cpp=my_cpp_filter). See INPUT_FILTER for further \n"
                    "info on how filters are used. If FILTER_PATTERNS is empty, INPUT_FILTER \n"
                    "is applied to all files. \n"
                   );
  cl->setWidgetType(ConfigList::File);
  cb = addBool(
                    "FILTER_SOURCE_FILES",
                    "If the FILTER_SOURCE_FILES tag is set to YES, the input filter (if set using \n"
                    "INPUT_FILTER) will be used to filter the input files when producing source \n"
                    "files to browse (i.e. when SOURCE_BROWSER is set to YES). \n",
                    FALSE
                );    
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Source Browser","configuration options related to source browsing");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "SOURCE_BROWSER",
                    "If the SOURCE_BROWSER tag is set to YES then a list of source files will \n"
                    "be generated. Documented entities will be cross-referenced with these sources. \n"
		    "Note: To get rid of all source code in the generated output, make sure also \n"
		    "VERBATIM_HEADERS is set to NO. \n",
                    FALSE
                 );
  cb = addBool(
                    "INLINE_SOURCES",
                    "Setting the INLINE_SOURCES tag to YES will include the body \n"
                    "of functions and classes directly in the documentation. \n",
                    FALSE
                 );
  cb = addBool(
                    "STRIP_CODE_COMMENTS",
                    "Setting the STRIP_CODE_COMMENTS tag to YES (the default) will instruct \n"
                    "doxygen to hide any special comment blocks from generated source code \n"
                    "fragments. Normal C and C++ comments will always remain visible. \n",
                    TRUE
                 );
  cb = addBool(     "REFERENCED_BY_RELATION",
                    "If the REFERENCED_BY_RELATION tag is set to YES \n"
		    "then for each documented function all documented \n"
		    "functions referencing it will be listed. \n",
                    FALSE
              );
  cb = addBool(     "REFERENCES_RELATION",
                    "If the REFERENCES_RELATION tag is set to YES \n"
		    "then for each documented function all documented entities \n"
		    "called/used by that function will be listed. \n",
                    FALSE
              );
  cb = addBool(     "REFERENCES_LINK_SOURCE",
                    "If the REFERENCES_LINK_SOURCE tag is set to YES (the default)\n"
                    "and SOURCE_BROWSER tag is set to YES, then the hyperlinks from\n"
                    "functions in REFERENCES_RELATION and REFERENCED_BY_RELATION lists will\n"
                    "link to the source code.  Otherwise they will link to the documentstion.\n",
                    TRUE
              );
  cb = addBool(
                    "USE_HTAGS",
		    "If the USE_HTAGS tag is set to YES then the references to source code \n"
		    "will point to the HTML generated by the htags(1) tool instead of doxygen \n"
		    "built-in source browser. The htags tool is part of GNU's global source \n"
		    "tagging system (see http://www.gnu.org/software/global/global.html). You \n"
		    "will need version 4.8.6 or higher. \n",
		    FALSE
              );
  cb->addDependency("SOURCE_BROWSER");
  cb = addBool(
                    "VERBATIM_HEADERS",
                    "If the VERBATIM_HEADERS tag is set to YES (the default) then Doxygen \n"
                    "will generate a verbatim copy of the header file for each class for \n"
                    "which an include is specified. Set to NO to disable this. \n",
                    TRUE
              );
  
  //-----------------------------------------------------------------------------------------------
  addInfo(  "Index","configuration options related to the alphabetical class index");
  //-----------------------------------------------------------------------------------------------
  
  //cb = addBool(
  //                  "ALPHABETICAL_INDEX",
  //                  "If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index \n"
  //                  "of all compounds will be generated. Enable this if the project \n"
  //                  "contains a lot of classes, structs, unions or interfaces. \n",
  //                  FALSE
  //               );
  ci = addInt(
                    "COLS_IN_ALPHA_INDEX",
                    "If the alphabetical index is enabled (see ALPHABETICAL_INDEX) then \n"
                    "the COLS_IN_ALPHA_INDEX tag can be used to specify the number of columns \n"
                    "in which this list will be split (can be a number in the range [1..20]) \n",
                    1,20,5
                );
  cl = addList(
                    "IGNORE_PREFIX",
                    "In case all classes in a project start with a common prefix, all \n"
                    "classes will be put under the same header in the alphabetical index. \n"
                    "The IGNORE_PREFIX tag can be used to specify one or more prefixes that \n"
                    "should be ignored while generating the index headers. \n"
                 );
  //-----------------------------------------------------------------------------------------------
  addInfo(  "HTML","configuration options related to the HTML output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "GENERATE_HTML",
                    "If the GENERATE_HTML tag is set to YES (the default) Doxygen will \n"
                    "generate HTML output. \n",
                    TRUE
                 );
  cs = addString(
                    "HTML_OUTPUT",
                    "The HTML_OUTPUT tag is used to specify where the HTML docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `html' will be used as the default path. \n"
                   );
  cs->setDefaultValue("html");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "HTML_FILE_EXTENSION",
                    "The HTML_FILE_EXTENSION tag can be used to specify the file extension for \n"
                    "each generated HTML page (for example: .htm,.php,.asp). If it is left blank \n"
                    "doxygen will generate files with .html extension.\n"
                   );
  cs->setDefaultValue(".html");
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "HTML_HEADER",
                    "The HTML_HEADER tag can be used to specify a personal HTML header for \n"
                    "each generated HTML page. If it is left blank doxygen will generate a \n"
                    "standard header.\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "HTML_FOOTER",
                    "The HTML_FOOTER tag can be used to specify a personal HTML footer for \n"
                    "each generated HTML page. If it is left blank doxygen will generate a \n"
                    "standard footer.\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "HTML_STYLESHEET",
                    "The HTML_STYLESHEET tag can be used to specify a user-defined cascading \n"
                    "style sheet that is used by each HTML page. It can be used to \n"
                    "fine-tune the look of the HTML output. If the tag is left blank doxygen \n"
                    "will generate a default style sheet. Note that doxygen will try to copy \n"
		    "the style sheet file to the HTML output directory, so don't put your own \n"
		    "stylesheet in the HTML output directory as well, or it will be erased! \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cb = addBool( 
                    "HTML_ALIGN_MEMBERS",
                    "If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes, \n"
                    "files or namespaces will be aligned in HTML using tables. If set to \n"
                    "NO a bullet list will be used. \n",
                    TRUE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(
                    "HTML_DYNAMIC_SECTIONS",
                    "If the HTML_DYNAMIC_SECTIONS tag is set to YES then the generated HTML \n"
		    "documentation will contain sections that can be hidden and shown after the \n"
		    "page has loaded. For this to work a browser that supports \n"
                    "JavaScript and DHTML is required (for instance Mozilla 1.0+, Firefox \n"
		    "Netscape 6.0+, Internet explorer 5.0+, Konqueror, or Safari). \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");

  /////////////////////////////////////////////////////////
  // Docsets //////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  cb = addBool( "GENERATE_DOCSET",
                    "If the GENERATE_DOCSET tag is set to YES, additional index files \n"
                    "will be generated that can be used as input for Apple's Xcode 3 \n"
                    "integrated development environment, introduced with OSX 10.5 (Leopard). \n"
		    "To create a documentation set, doxygen will generate a Makefile in the \n"
		    "HTML output directory. Running make will produce the docset in that \n"
		    "directory and running \"make install\" will install the docset in \n"
		    "~/Library/Developer/Shared/Documentation/DocSets so that Xcode will find \n"
		    "it at startup. \n"
		    "See http://developer.apple.com/tools/creatingdocsetswithdoxygen.html "
		    "for more information. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cs = addString(
                    "DOCSET_FEEDNAME",
                    "When GENERATE_DOCSET tag is set to YES, this tag determines the name of the \n"
                    "feed. A documentation feed provides an umbrella under which multiple \n"
		    "documentation sets from a single provider (such as a company or product suite) \n"
		    "can be grouped. \n"
                );
  cs->setDefaultValue("Doxygen generated docs");
  cs->addDependency("GENERATE_DOCSET");
  cs = addString(
                    "DOCSET_BUNDLE_ID",
                    "When GENERATE_DOCSET tag is set to YES, this tag specifies a string that \n"
		    "should uniquely identify the documentation set bundle. This should be a \n"
		    "reverse domain-name style string, e.g. com.mycompany.MyDocSet. Doxygen \n"
		    "will append .docset to the name. \n"
		);
  cs->setDefaultValue("org.doxygen.Project");
  cs->addDependency("GENERATE_DOCSET");

  /////////////////////////////////////////////////////////
  // HTMLHELP /////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  cb = addBool(
                    "GENERATE_HTMLHELP",
                    "If the GENERATE_HTMLHELP tag is set to YES, additional index files \n"
                    "will be generated that can be used as input for tools like the \n"
                    "Microsoft HTML help workshop to generate a compiled HTML help file (.chm) \n"
                    "of the generated HTML documentation. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cs = addString(
                    "CHM_FILE",
                    "If the GENERATE_HTMLHELP tag is set to YES, the CHM_FILE tag can \n"
                    "be used to specify the file name of the resulting .chm file. You \n"
		    "can add a path in front of the file if the result should not be \n"
		    "written to the html output directory. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "HHC_LOCATION",
                    "If the GENERATE_HTMLHELP tag is set to YES, the HHC_LOCATION tag can \n"
                    "be used to specify the location (absolute path including file name) of \n"
		    "the HTML help compiler (hhc.exe). If non-empty doxygen will try to run \n"
		    "the HTML help compiler on the generated index.hhp.\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
#if 0
  cs = addString(
                    "QTHELP_FILE",
                    "If the GENERATE_HTMLHELP tag is set to YES, the QTHELP_FILE tag can \n"
                    "be used to specify the file name of the resulting .(qch|qhp) file. \n"
                    "You can add a path in front of the file if the result should not be \n"
                    "written to the html output directory. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "QTHELP_CONFIG",
                    "If DOXYGEN2QTHELP_LOC is set, QTHELP_CONFIG must specify the file name \n"
                    "of a config file to pass to doxygen2qthelp. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
  cs = addString(
                    "DOXYGEN2QTHELP_LOC",
                    "If the GENERATE_HTMLHELP tag is set to YES, the DOXYGEN2QTHELP_LOC tag \n"
                    "can be used to specify the location (absolute path including file name) of \n"
                    "the doxygen2qthelp tool. If non-empty doxygen will try to run doxygen2qthelp \n"
                    "on the generated index.hhp.\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_HTML");
#endif
  addObsolete("QTHELP_FILE");
  addObsolete("QTHELP_CONFIG");
  addObsolete("DOXYGEN2QTHELP_LOC");
  cb = addBool(
                    "GENERATE_CHI",
                    "If the GENERATE_HTMLHELP tag is set to YES, the GENERATE_CHI flag \n"
                    "controls if a separate .chi index file is generated (YES) or that \n"
                    "it should be included in the master .chm file (NO).\n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cs = addString(
                    "CHM_INDEX_ENCODING",
                    "If the GENERATE_HTMLHELP tag is set to YES, the CHM_INDEX_ENCODING\n"
                    "is used to encode HtmlHelp index (hhk), content (hhc) and project file\n"
                    "content.\n" 
                 );
  cs->addDependency("GENERATE_HTML");
  cb = addBool(
                    "BINARY_TOC",
                    "If the GENERATE_HTMLHELP tag is set to YES, the BINARY_TOC flag \n"
                    "controls whether a binary table of contents is generated (YES) or a \n"
                    "normal table of contents (NO) in the .chm file.\n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cb = addBool(
                    "TOC_EXPAND",
                    "The TOC_EXPAND flag can be set to YES to add extra items for group members \n"
                    "to the contents of the HTML help documentation and to the tree view. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");

  /////////////////////////////////////////////////////////
  // QT HELP //////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  cb = addBool(
                    "GENERATE_QHP",
                    "If the GENERATE_QHP tag is set to YES and both QHP_NAMESPACE and QHP_VIRTUAL_FOLDER \n"
                    "are set, an additional index file will be generated that can be used as input for \n"
                    "Qt's qhelpgenerator to generate a Qt Compressed Help (.qch) of the generated \n"
                    "HTML documentation. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  cs = addString(
                    "QCH_FILE",
                    "If the QHG_LOCATION tag is specified, the QCH_FILE tag can \n"
                    "be used to specify the file name of the resulting .qch file. \n"
                    "The path specified is relative to the HTML output folder. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_QHP");
  cs = addString(
                    "QHP_NAMESPACE",
                    "The QHP_NAMESPACE tag specifies the namespace to use when generating \n"
                    "Qt Help Project output. For more information please see \n"
                    "<a href=\"http://doc.trolltech.com/qthelpproject.html#namespace\">Qt Help Project / Namespace</a>. \n"
                   );
  cs->setDefaultValue("org.doxygen.Project");
  cs->addDependency("GENERATE_QHP");
  cs = addString(
                    "QHP_VIRTUAL_FOLDER",
                    "The QHP_VIRTUAL_FOLDER tag specifies the namespace to use when generating \n"
                    "Qt Help Project output. For more information please see \n"
                    "<a href=\"http://doc.trolltech.com/qthelpproject.html#virtual-folders\">Qt Help Project / Virtual Folders</a>. \n"
                   );
  cs->setDefaultValue("doc");
  cs->addDependency("GENERATE_QHP");
  cs = addString(
                    "QHG_LOCATION",
                    "If the GENERATE_QHP tag is set to YES, the QHG_LOCATION tag can \n"
                    "be used to specify the location of Qt's qhelpgenerator. \n"
                    "If non-empty doxygen will try to run qhelpgenerator on the generated \n"
                    ".qhp file .\n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_QHP");

  /////////////////////////////////////////////////////////
  // MISC /////////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  cb = addBool(
                    "DISABLE_INDEX",
                    "The DISABLE_INDEX tag can be used to turn on/off the condensed index at \n"
                    "top of each HTML page. The value NO (the default) enables the index and \n"
                    "the value YES disables it. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
  ci = addInt(
                    "ENUM_VALUES_PER_LINE",
                    "This tag can be used to set the number of enum values (range [1..20]) \n"
                    "that doxygen will group on one line in the generated HTML documentation. \n",
                    1,20,4
                );
  ci->addDependency("GENERATE_HTML");
  ce = addEnum(
                    "GENERATE_TREEVIEW",
                    "The GENERATE_TREEVIEW tag is used to specify whether a tree-like index\n"
                    "structure should be generated to display hierarchical information.\n"
                    "If the tag value is set to FRAME, a side panel will be generated\n"
                    "containing a tree-like index structure (just like the one that \n"
                    "is generated for HTML Help). For this to work a browser that supports \n"
                    "JavaScript, DHTML, CSS and frames is required (for instance Mozilla 1.0+, \n"
		    "Netscape 6.0+, Internet explorer 5.0+, or Konqueror). Windows users are \n"
		    "probably better off using the HTML help feature. Other possible values \n"
		    "for this tag are: HIERARCHIES, which will generate the Groups, Directories,\n"
                    "and Class Hierarchy pages using a tree view instead of an ordered list;\n"
                    "ALL, which combines the behavior of FRAME and HIERARCHIES; and NONE, which\n"
                    "disables this behavior completely. For backwards compatibility with previous\n"
                    "releases of Doxygen, the values YES and NO are equivalent to FRAME and NONE\n"
                    "respectively.\n",
                    "NONE"
                 );
  ce->addValue("NONE");
  ce->addValue("FRAME");
  ce->addValue("HIERARCHIES");
  ce->addValue("ALL");
  cb->addDependency("GENERATE_HTML");
  ci = addInt(
                    "TREEVIEW_WIDTH",
                    "If the treeview is enabled (see GENERATE_TREEVIEW) then this tag can be \n"
                    "used to set the initial width (in pixels) of the frame in which the tree \n"
                    "is shown. \n",
                    0,1500,250
                 );
  ci->addDependency("GENERATE_HTML");
  ci = addInt(
                    "FORMULA_FONTSIZE",
                    "Use this tag to change the font size of Latex formulas included \n"
                    "as images in the HTML documentation. The default is 10. Note that \n"
		    "when you change the font size after a successful doxygen run you need \n"
		    "to manually remove any form_*.png images from the HTML output directory \n"
		    "to force them to be regenerated. \n",
                    8,50,10
                 );
  ci->addDependency("GENERATE_HTML");
#if 0
  cb = addBool(
                    "GENERATE_INDEXLOG",
                    "If the GENERATE_INDEXLOG tag is set to YES, an additional log file \n"
                    "will be generated that can be used to create new index formats using XSLT \n"
                    "instead of writing C++ code. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_HTML");
#endif

  //-----------------------------------------------------------------------------------------------
  addInfo(  "LaTeX","configuration options related to the LaTeX output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "GENERATE_LATEX",
                    "If the GENERATE_LATEX tag is set to YES (the default) Doxygen will \n"
                    "generate Latex output. \n",
                    TRUE
                 );
  cs = addString(
                    "LATEX_OUTPUT",
                    "The LATEX_OUTPUT tag is used to specify where the LaTeX docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `latex' will be used as the default path. \n"
                   );
  cs->setDefaultValue("latex");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_LATEX");
#if 0
  cs = addString(   "LATEX_OUTPUT_ENCODING",
                    "The LATEX_OUTPUT_ENCODING specifies the character encoding of the LaTeX output.\n"
                    "produced by doxygen. If left blank ISO-8859-1 will be used. Doxygen uses \n"
		    "libiconv for the transcoding. See http://www.gnu.org/software/libiconv for \n"
		    "the list of possible encodings.\n"
                );
  cs->setDefaultValue("ISO-8859-1");
  cs->addDependency("GENERATE_LATEX");
#endif
  cs = addString(
                    "LATEX_CMD_NAME",
                    "The LATEX_CMD_NAME tag can be used to specify the LaTeX command name to be \n"
                    "invoked. If left blank `latex' will be used as the default command name. \n"
                   );
  cs->setDefaultValue("latex");
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_LATEX");
  cs = addString(
                    "MAKEINDEX_CMD_NAME",
                    "The MAKEINDEX_CMD_NAME tag can be used to specify the command name to \n"
		    "generate index for LaTeX. If left blank `makeindex' will be used as the \n"
		    "default command name. \n"
                   );
  cs->setDefaultValue("makeindex");
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_LATEX");
  cb = addBool(
                    "COMPACT_LATEX",
                    "If the COMPACT_LATEX tag is set to YES Doxygen generates more compact \n"
                    "LaTeX documents. This may be useful for small projects and may help to \n"
                    "save some trees in general. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_LATEX");
  ce = addEnum(
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
  cl = addList(
                    "EXTRA_PACKAGES",
                    "The EXTRA_PACKAGES tag can be to specify one or more names of LaTeX \n"
                    "packages that should be included in the LaTeX output. \n"
                 );
  cl->addDependency("GENERATE_LATEX");
  cs = addString(
                    "LATEX_HEADER",
                    "The LATEX_HEADER tag can be used to specify a personal LaTeX header for \n"
                    "the generated latex document. The header should contain everything until \n"
                    "the first chapter. If it is left blank doxygen will generate a \n"
                    "standard header. Notice: only use this tag if you know what you are doing! \n"
                 );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_LATEX");
  cb = addBool(
                    "PDF_HYPERLINKS",
                    "If the PDF_HYPERLINKS tag is set to YES, the LaTeX that is generated \n"
                    "is prepared for conversion to pdf (using ps2pdf). The pdf file will \n"
                    "contain links (just like the HTML output) instead of page references \n"
                    "This makes the output suitable for online browsing using a pdf viewer. \n",
                    TRUE
                 );
  cb->addDependency("GENERATE_LATEX");
  cb = addBool(
                    "USE_PDFLATEX",
                    "If the USE_PDFLATEX tag is set to YES, pdflatex will be used instead of \n"
                    "plain latex in the generated Makefile. Set this option to YES to get a \n"
                    "higher quality PDF documentation. \n",
                    TRUE
                 );   
  cb->addDependency("GENERATE_LATEX");
  cb = addBool(
                    "LATEX_BATCHMODE",
                    "If the LATEX_BATCHMODE tag is set to YES, doxygen will add the \\\\batchmode. \n"
                    "command to the generated LaTeX files. This will instruct LaTeX to keep \n"
                    "running if errors occur, instead of asking the user for help. \n"
                    "This option is also used when generating formulas in HTML. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_LATEX");
  cb = addBool(
                    "LATEX_HIDE_INDICES",
                    "If LATEX_HIDE_INDICES is set to YES then doxygen will not \n"
                    "include the index chapters (such as File Index, Compound Index, etc.) \n"
                    "in the output. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_LATEX");
  //-----------------------------------------------------------------------------------------------
  addInfo(  "RTF","configuration options related to the RTF output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "GENERATE_RTF",
                    "If the GENERATE_RTF tag is set to YES Doxygen will generate RTF output \n"
                    "The RTF output is optimized for Word 97 and may not look very pretty with \n"
                    "other RTF readers or editors.\n",
                    FALSE
                 );
  cs = addString(
                    "RTF_OUTPUT",
                    "The RTF_OUTPUT tag is used to specify where the RTF docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `rtf' will be used as the default path. \n"
                 );
  cs->setDefaultValue("rtf");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_RTF");
#if 0
  cs = addString(   "RTF_OUTPUT_ENCODING",
                    "The RTF_OUTPUT_ENCODING specifies the character encoding of the RTF output.\n"
                    "produced by doxygen. If left blank ISO-8859-1 will be used. Doxygen uses \n"
		    "libiconv for the transcoding. See http://www.gnu.org/software/libiconv for \n"
		    "the list of possible encodings.\n"
                );
  cs->setDefaultValue("ISO-8859-1");
  cs->addDependency("GENERATE_RTF");
#endif
  cb = addBool(
                    "COMPACT_RTF",
                    "If the COMPACT_RTF tag is set to YES Doxygen generates more compact \n"
                    "RTF documents. This may be useful for small projects and may help to \n"
                    "save some trees in general. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_RTF");
  cb = addBool(
                    "RTF_HYPERLINKS",
                    "If the RTF_HYPERLINKS tag is set to YES, the RTF that is generated \n"
                    "will contain hyperlink fields. The RTF file will \n"
                    "contain links (just like the HTML output) instead of page references. \n"
                    "This makes the output suitable for online browsing using WORD or other \n"
                    "programs which support those fields. \n"
                    "Note: wordpad (write) and others do not support links. \n",
                    FALSE
                 );
  cb->addDependency("GENERATE_RTF");
  cs = addString(
                    "RTF_STYLESHEET_FILE",
                    "Load stylesheet definitions from file. Syntax is similar to doxygen's \n"
                    "config file, i.e. a series of assignments. You only have to provide \n"
                    "replacements, missing definitions are set to their default value. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_RTF");
  cs = addString(
                    "RTF_EXTENSIONS_FILE",
                    "Set optional variables used in the generation of an rtf document. \n"
                    "Syntax is similar to doxygen's config file.\n"
                );                  
  cs->setWidgetType(ConfigString::File);
  cs->addDependency("GENERATE_RTF");

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Man","configuration options related to the man page output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "GENERATE_MAN",
                    "If the GENERATE_MAN tag is set to YES (the default) Doxygen will \n"
                    "generate man pages \n",
                    FALSE
                   );
  cs = addString(
                    "MAN_OUTPUT",
                    "The MAN_OUTPUT tag is used to specify where the man pages will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `man' will be used as the default path. \n"
                   );
  cs->setDefaultValue("man");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_MAN");
  cs = addString(
                    "MAN_EXTENSION",
                    "The MAN_EXTENSION tag determines the extension that is added to \n"
                    "the generated man pages (default is the subroutine's section .3) \n"
                   );
  cs->setDefaultValue(".3");
  cs->addDependency("GENERATE_MAN");
  cb = addBool(
                    "MAN_LINKS",
                    "If the MAN_LINKS tag is set to YES and Doxygen generates man output, \n"
                    "then it will generate one additional man file for each entity \n"
                    "documented in the real man page(s). These additional files \n"
                    "only source the real man page, but without them the man command \n"
                    "would be unable to find the correct page. The default is NO. \n",
                    FALSE
                   );
  cb->addDependency("GENERATE_MAN");
  //-----------------------------------------------------------------------------------------------
  addInfo(  "XML","configuration options related to the XML output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  
                    "GENERATE_XML",
                    "If the GENERATE_XML tag is set to YES Doxygen will \n"
                    "generate an XML file that captures the structure of \n"
                    "the code including all documentation. \n",
		    FALSE
                 );
  cs = addString(
                    "XML_OUTPUT",
                    "The XML_OUTPUT tag is used to specify where the XML pages will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `xml' will be used as the default path. \n"
                   );
  cs->setDefaultValue("xml");
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("GENERATE_XML");
  cs = addString(
                    "XML_SCHEMA",
		    "The XML_SCHEMA tag can be used to specify an XML schema, \n"
		    "which can be used by a validating XML parser to check the \n"
		    "syntax of the XML files. \n"
                 );
  cs->addDependency("GENERATE_XML");
  cs = addString(
                    "XML_DTD",
		    "The XML_DTD tag can be used to specify an XML DTD, \n"
		    "which can be used by a validating XML parser to check the \n"
		    "syntax of the XML files. \n"
                 );
  cs->addDependency("GENERATE_XML");
  cb = addBool(
                    "XML_PROGRAMLISTING",
                    "If the XML_PROGRAMLISTING tag is set to YES Doxygen will \n"
                    "dump the program listings (including syntax highlighting \n"
		    "and cross-referencing information) to the XML output. Note that \n"
		    "enabling this will significantly increase the size of the XML output. \n",
                    TRUE
                 );
  cb->addDependency("GENERATE_XML");

  //--------------------------------------------------------------------------
  addInfo(  "DEF","configuration options for the AutoGen Definitions output");
  //--------------------------------------------------------------------------
  cb = addBool("GENERATE_AUTOGEN_DEF",

               "If the GENERATE_AUTOGEN_DEF tag is set to YES Doxygen will \n"
               "generate an AutoGen Definitions (see autogen.sf.net) file \n"
	       "that captures the structure of the code including all \n"
	       "documentation. Note that this feature is still experimental \n"
	       "and incomplete at the moment. \n",

               FALSE );
  //-----------------------------------------------------------------------------------------------
  addInfo(  "PerlMod","configuration options related to the Perl module output");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(  
                    "GENERATE_PERLMOD",
                    "If the GENERATE_PERLMOD tag is set to YES Doxygen will \n"
                    "generate a Perl module file that captures the structure of \n"
                    "the code including all documentation. Note that this \n"
		    "feature is still experimental and incomplete at the \n"
		    "moment. \n",
		    FALSE
                 );
  cb = addBool(  
                    "PERLMOD_LATEX",
                    "If the PERLMOD_LATEX tag is set to YES Doxygen will generate \n"
                    "the necessary Makefile rules, Perl scripts and LaTeX code to be able \n"
		    "to generate PDF and DVI output from the Perl module output. \n",
		    FALSE
                 );
  cb->addDependency("GENERATE_PERLMOD");
  cb = addBool(  
                    "PERLMOD_PRETTY",
                    "If the PERLMOD_PRETTY tag is set to YES the Perl module output will be \n"
		    "nicely formatted so it can be parsed by a human reader.  This is useful \n"
		    "if you want to understand what is going on.  On the other hand, if this \n"
		    "tag is set to NO the size of the Perl module output will be much smaller \n"
		    "and Perl will parse it just the same. \n",
		    TRUE
                 );
  cb->addDependency("GENERATE_PERLMOD");
  cs = addString(  
                    "PERLMOD_MAKEVAR_PREFIX",
                    "The names of the make variables in the generated doxyrules.make file \n"
                    "are prefixed with the string contained in PERLMOD_MAKEVAR_PREFIX. \n"
		    "This is useful so different doxyrules.make files included by the same \n"
		    "Makefile don't overwrite each other's variables."
                 );
  cs->addDependency("GENERATE_PERLMOD");

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Preprocessor","Configuration options related to the preprocessor   ");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "ENABLE_PREPROCESSING",
                    "If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will \n"
                    "evaluate all C-preprocessor directives found in the sources and include \n"
                    "files. \n",
                    TRUE
                 );
  cb = addBool(
                    "MACRO_EXPANSION",
                    "If the MACRO_EXPANSION tag is set to YES Doxygen will expand all macro \n"
                    "names in the source code. If set to NO (the default) only conditional \n"
                    "compilation will be performed. Macro expansion can be done in a controlled \n"
                    "way by setting EXPAND_ONLY_PREDEF to YES. \n",
                    FALSE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  cb = addBool(
                    "EXPAND_ONLY_PREDEF",
                    "If the EXPAND_ONLY_PREDEF and MACRO_EXPANSION tags are both set to YES \n"
                    "then the macro expansion is limited to the macros specified with the \n"
                    "PREDEFINED and EXPAND_AS_DEFINED tags. \n",
                    FALSE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  cb = addBool(
                    "SEARCH_INCLUDES",
                    "If the SEARCH_INCLUDES tag is set to YES (the default) the includes files \n"
                    "in the INCLUDE_PATH (see below) will be search if a #include is found. \n",
                    TRUE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  cl = addList(
                    "INCLUDE_PATH",
                    "The INCLUDE_PATH tag can be used to specify one or more directories that \n"
                    "contain include files that are not input files but should be processed by \n"
                    "the preprocessor.\n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cl->addDependency("ENABLE_PREPROCESSING");
  cl = addList(
                    "INCLUDE_FILE_PATTERNS",
                    "You can use the INCLUDE_FILE_PATTERNS tag to specify one or more wildcard \n"
                    "patterns (like *.h and *.hpp) to filter out the header-files in the \n"
                    "directories. If left blank, the patterns specified with FILE_PATTERNS will \n"
                    "be used. \n"
                 );
  cl->addDependency("ENABLE_PREPROCESSING");
  cl = addList(
                    "PREDEFINED",
                    "The PREDEFINED tag can be used to specify one or more macro names that \n"
                    "are defined before the preprocessor is started (similar to the -D option of \n"
                    "gcc). The argument of the tag is a list of macros of the form: name \n"
                    "or name=definition (no spaces). If the definition and the = are \n"
                    "omitted =1 is assumed. To prevent a macro definition from being \n"
		    "undefined via #undef or recursively expanded use the := operator \n"
		    "instead of the = operator.\n"
                 );
  cl->addDependency("ENABLE_PREPROCESSING");
  cl = addList(
                    "EXPAND_AS_DEFINED",
                    "If the MACRO_EXPANSION and EXPAND_ONLY_PREDEF tags are set to YES then \n"
                    "this tag can be used to specify a list of macro names that should be expanded. \n"
                    "The macro definition that is found in the sources will be used. \n"
                    "Use the PREDEFINED tag if you want to use a different macro definition. \n" 
                 );
  cl->addDependency("ENABLE_PREPROCESSING");
  cb = addBool(
                    "SKIP_FUNCTION_MACROS",
                    "If the SKIP_FUNCTION_MACROS tag is set to YES (the default) then \n"
		    "doxygen's preprocessor will remove all function-like macros that are alone \n"
		    "on a line, have an all uppercase name, and do not end with a semicolon. Such \n"
		    "function macros are typically used for boiler-plate code, and will confuse \n"
		    "the parser if not removed. \n",
                    TRUE
                 );
  cb->addDependency("ENABLE_PREPROCESSING");
  //-----------------------------------------------------------------------------------------------
  addInfo(  "External","Configuration::additions related to external references   ");
  //-----------------------------------------------------------------------------------------------
  cl = addList(
                    "TAGFILES",
                    "The TAGFILES option can be used to specify one or more tagfiles. \n"
		    "Optionally an initial location of the external documentation \n"
		    "can be added for each tagfile. The format of a tag file without \n"
		    "this location is as follows: \n"
		    "  TAGFILES = file1 file2 ... \n" 
		    "Adding location for the tag files is done as follows: \n"
		    "  TAGFILES = file1=loc1 \"file2 = loc2\" ... \n"
	            "where \"loc1\" and \"loc2\" can be relative or absolute paths or \n"
		    "URLs. If a location is present for each tag, the installdox tool \n"
		    "does not have to be run to correct the links.\n"
		    "Note that each tag file must have a unique name\n"
		    "(where the name does NOT include the path)\n"
	            "If a tag file is not located in the directory in which doxygen \n"
	            "is run, you must also specify the path to the tagfile here. \n"
               );
  cl->setWidgetType(ConfigList::File);
  cs = addString(
                    "GENERATE_TAGFILE",
                    "When a file name is specified after GENERATE_TAGFILE, doxygen will create \n"
                    "a tag file that is based on the input files it reads. \n"
                   );
  cs->setWidgetType(ConfigString::File);
  cb = addBool(
                    "ALLEXTERNALS",
                    "If the ALLEXTERNALS tag is set to YES all external classes will be listed \n"
                    "in the class index. If set to NO only the inherited external classes \n"
                    "will be listed. \n",
                    FALSE
                 );
  cb = addBool(
                    "EXTERNAL_GROUPS",
                    "If the EXTERNAL_GROUPS tag is set to YES all external groups will be listed \n"
                    "in the modules index. If set to NO, only the current project's groups will \n"
                    "be listed. \n",
                    TRUE
                 );
  cs = addString(
                    "PERL_PATH",
                    "The PERL_PATH should be the absolute path and name of the perl script \n"
                    "interpreter (i.e. the result of `which perl'). \n"
                   );
  cs->setDefaultValue("/usr/bin/perl");
  cs->setWidgetType(ConfigString::Dir);

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Dot","Configuration options related to the dot tool   ");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "CLASS_DIAGRAMS",
                    "If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will \n"
                    "generate a inheritance diagram (in HTML, RTF and LaTeX) for classes with base \n"
		    "or super classes. Setting the tag to NO turns the diagrams off. Note that \n"
		    "this option is superseded by the HAVE_DOT option below. This is only a \n"
		    "fallback. It is recommended to install and use dot, since it yields more \n"
		    "powerful graphs. \n",
                    TRUE
                 );
  cs = addString(   "MSCGEN_PATH",
                    "You can define message sequence charts within doxygen comments using the \\msc \n"
		    "command. Doxygen will then run the mscgen tool (see \n"
		    "http://www.mcternan.me.uk/mscgen/) to produce the chart and insert it in the \n"
		    "documentation. The MSCGEN_PATH tag allows you to specify the directory where \n"
		    "the mscgen tool resides. If left empty the tool is assumed to be found in the \n"
		    "default search path. \n"
                );
  cb = addBool(
                    "HIDE_UNDOC_RELATIONS",
		    "If set to YES, the inheritance and collaboration graphs will hide \n"
		    "inheritance and usage relations if the target is undocumented \n"
		    "or is not a class. \n",
                    TRUE
                 );
  cb = addBool(
                    "HAVE_DOT",
                    "If you set the HAVE_DOT tag to YES then doxygen will assume the dot tool is \n"
                    "available from the path. This tool is part of Graphviz, a graph visualization \n"
                    "toolkit from AT&T and Lucent Bell Labs. The other options in this section \n"
                    "have no effect if this option is set to NO (the default) \n",
                    FALSE
                 );
  cs = addString(   "DOT_FONTNAME",
                    "By default doxygen will write a font called FreeSans.ttf to the output \n"
		    "directory and reference it in all dot files that doxygen generates. This \n"
		    "font does not include all possible unicode characters however, so when you need \n"
		    "these (or just want a differently looking font) you can specify the font name \n"
		    "using DOT_FONTNAME. You need need to make sure dot is able to find the font, \n"
		    "which can be done by putting it in a standard location or by setting the \n"
		    "DOTFONTPATH environment variable or by setting DOT_FONTPATH to the directory \n"
		    "containing the font. \n"
                 );
  cs->setDefaultValue("FreeSans");
  cs = addString(   "DOT_FONTPATH",
      		    "By default doxygen will tell dot to use the output directory to look for the \n"
		    "FreeSans.ttf font (which doxygen will put there itself). If you specify a \n"
		    "different font using DOT_FONTNAME you can set the path where dot \n"
		    "can find it using this tag. \n"
                );
  cb = addBool(
                    "CLASS_GRAPH",
                    "If the CLASS_GRAPH and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for each documented class showing the direct and \n"
                    "indirect inheritance relations. Setting this tag to YES will force the \n"
                    "the CLASS_DIAGRAMS tag to NO.\n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "COLLABORATION_GRAPH",
                    "If the COLLABORATION_GRAPH and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for each documented class showing the direct and \n"
                    "indirect implementation dependencies (inheritance, containment, and \n"
                    "class references variables) of the class with other documented classes. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "GROUP_GRAPHS",
                    "If the GROUP_GRAPHS and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for groups, showing the direct groups dependencies\n",
                    TRUE
                 );                 
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "UML_LOOK",
                    "If the UML_LOOK tag is set to YES doxygen will generate inheritance and \n"
	            "collaboration diagrams in a style similar to the OMG's Unified Modeling \n"
		    "Language. \n",
	            FALSE
	      );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "TEMPLATE_RELATIONS",
		    "If set to YES, the inheritance and collaboration graphs will show the \n"
		    "relations between templates and their instances. \n",
                    FALSE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "INCLUDE_GRAPH",
                    "If the ENABLE_PREPROCESSING, SEARCH_INCLUDES, INCLUDE_GRAPH, and HAVE_DOT \n"
                    "tags are set to YES then doxygen will generate a graph for each documented \n"
                    "file showing the direct and indirect include dependencies of the file with \n"
                    "other documented files. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "INCLUDED_BY_GRAPH",
                    "If the ENABLE_PREPROCESSING, SEARCH_INCLUDES, INCLUDED_BY_GRAPH, and \n"
                    "HAVE_DOT tags are set to YES then doxygen will generate a graph for each \n"
                    "documented header file showing the documented files that directly or \n"
                    "indirectly include this file. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "CALL_GRAPH",
                    "If the CALL_GRAPH and HAVE_DOT options are set to YES then \n"
		    "doxygen will generate a call dependency graph for every global function \n"
		    "or class method. Note that enabling this option will significantly increase \n"
		    "the time of a run. So in most cases it will be better to enable call graphs \n"
		    "for selected functions only using the \\callgraph command.\n",
                    FALSE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "CALLER_GRAPH",
                    "If the CALLER_GRAPH and HAVE_DOT tags are set to YES then \n"
		    "doxygen will generate a caller dependency graph for every global function \n"
		    "or class method. Note that enabling this option will significantly increase \n"
		    "the time of a run. So in most cases it will be better to enable caller \n"
		    "graphs for selected functions only using the \\callergraph command.\n",
                    FALSE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "GRAPHICAL_HIERARCHY",
                    "If the GRAPHICAL_HIERARCHY and HAVE_DOT tags are set to YES then doxygen \n"
                    "will graphical hierarchy of all classes instead of a textual one. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(    
                  "DIRECTORY_GRAPH",
  		  "If the DIRECTORY_GRAPH, SHOW_DIRECTORIES and HAVE_DOT tags are set to YES \n"
  	 	  "then doxygen will show the dependencies a directory has on other directories \n"
       	          "in a graphical way. The dependency relations are determined by the #include\n"
  		  "relations between the files in the directories.\n",
                  TRUE
               );
  cb->addDependency("HAVE_DOT");
  ce = addEnum(
                    "DOT_IMAGE_FORMAT",
                    "The DOT_IMAGE_FORMAT tag can be used to set the image format of the images \n"
		    "generated by dot. Possible values are png, jpg, or gif\n"
                    "If left blank png will be used. \n",
                    "png"
                   );
  ce->addValue("png");
  ce->addValue("jpg");
  ce->addValue("gif");
  ce->addDependency("HAVE_DOT");
  cs = addString(
                    "DOT_PATH",
                    "The tag DOT_PATH can be used to specify the path where the dot tool can be \n"
                    "found. If left blank, it is assumed the dot tool can be found in the path. \n"
                   );
  cs->setWidgetType(ConfigString::Dir);
  cs->addDependency("HAVE_DOT");
  cl = addList(
                    "DOTFILE_DIRS",
                    "The DOTFILE_DIRS tag can be used to specify one or more directories that \n"
		    "contain dot files that are included in the documentation (see the \n"
                    "\\dotfile command). \n"
                 );
  cl->setWidgetType(ConfigList::Dir);
  cl->addDependency("HAVE_DOT");
  ci = addInt(
                    "DOT_GRAPH_MAX_NODES",
                    "The DOT_GRAPH_MAX_NODES tag can be used to set the maximum number of \n"
                    "nodes that will be shown in the graph. If the number of nodes in a graph \n"
                    "becomes larger than this value, doxygen will truncate the graph, which is \n"
                    "visualized by representing a node as a red box. Note that doxygen if the \n"
		    "number of direct children of the root node in a graph is already larger than \n"
		    "DOT_GRAPH_MAX_NODES then the graph will not be shown at all. Also note \n"
                    "that the size of a graph can be further restricted by MAX_DOT_GRAPH_DEPTH. \n",
                    0,10000, 50
                );
  ci->addDependency("HAVE_DOT");
  ci = addInt(
                    "MAX_DOT_GRAPH_DEPTH",
                    "The MAX_DOT_GRAPH_DEPTH tag can be used to set the maximum depth of the \n"
                    "graphs generated by dot. A depth value of 3 means that only nodes reachable \n"
                    "from the root by following a path via at most 3 edges will be shown. Nodes \n"
		    "that lay further from the root node will be omitted. Note that setting this \n"
		    "option to 1 or 2 may greatly reduce the computation time needed for large \n"
		    "code bases. Also note that the size of a graph can be further restricted by \n"
                    "DOT_GRAPH_MAX_NODES. Using a depth of 0 means no depth restriction.\n",
                    0,1000,0
                );
  ci->addDependency("HAVE_DOT");
  addObsolete("MAX_DOT_GRAPH_WIDTH");
  addObsolete("MAX_DOT_GRAPH_HEIGHT");
  cb = addBool(
                    "DOT_TRANSPARENT",
		    "Set the DOT_TRANSPARENT tag to YES to generate images with a transparent \n"
		    "background. This is disabled by default, because dot on Windows does not \n"
		    "seem to support this out of the box. Warning: Depending on the platform used, \n"
		    "enabling this option may lead to badly anti-aliased labels on the edges of \n"
		    "a graph (i.e. they become hard to read). \n",
		    FALSE
              );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "DOT_MULTI_TARGETS",
		    "Set the DOT_MULTI_TARGETS tag to YES allow dot to generate multiple output \n"
		    "files in one run (i.e. multiple -o and -T options on the command line). This \n"
		    "makes dot run faster, but since only newer versions of dot (>1.8.10) \n"
		    "support this, this feature is disabled by default. \n",
		    FALSE
              );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "GENERATE_LEGEND",
                    "If the GENERATE_LEGEND tag is set to YES (the default) Doxygen will \n"
                    "generate a legend page explaining the meaning of the various boxes and \n"
                    "arrows in the dot generated graphs. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");
  cb = addBool(
                    "DOT_CLEANUP",
                    "If the DOT_CLEANUP tag is set to YES (the default) Doxygen will \n"
                    "remove the intermediate dot files that are used to generate \n"
                    "the various graphs. \n",
                    TRUE
                 );
  cb->addDependency("HAVE_DOT");

  //-----------------------------------------------------------------------------------------------
  addInfo(  "Search","Configuration::additions related to the search engine   ");
  //-----------------------------------------------------------------------------------------------
  cb = addBool(
                    "SEARCHENGINE",
                    "The SEARCHENGINE tag specifies whether or not a search engine should be \n"
                    "used. If set to NO the values of all tags below this one will be ignored. \n",
                    FALSE
                 );
  addObsolete("CGI_NAME");
  addObsolete("CGI_URL");
  addObsolete("DOC_URL");
  addObsolete("DOC_ABSPATH");
  addObsolete("BIN_ABSPATH");
  addObsolete("EXT_DOC_PATHS");

  // The IMAGE_PATTERNS tag is now officially obsolete.
}

static QCString configFileToString(const char *name)
{
  if (name==0 || name[0]==0) return 0;
  QFile f;

  bool fileOpened=FALSE;
  if (name[0]=='-' && name[1]==0) // read from stdin
  {
    fileOpened=f.open(IO_ReadOnly,stdin);
    if (fileOpened)
    {
      const int bSize=4096;
      QCString contents(bSize);
      int totalSize=0;
      int size;
      while ((size=f.readBlock(contents.data()+totalSize,bSize))==bSize)
      {
        totalSize+=bSize;
        contents.resize(totalSize+bSize); 
      }
      totalSize+=size+2;
      contents.resize(totalSize);
      contents.at(totalSize-2)='\n'; // to help the scanner
      contents.at(totalSize-1)='\0';
      return contents;
    }
  }
  else // read from file
  {
    QFileInfo fi(name);
    if (!fi.exists() || !fi.isFile())
    {
      config_err("Error: file `%s' not found\n",name);
      return "";
    }
    f.setName(name);
    fileOpened=f.open(IO_ReadOnly);
    if (fileOpened)
    {
      int fsize=f.size();
      QCString contents(fsize+2);
      f.readBlock(contents.data(),fsize);
      f.close();
      if (fsize==0 || contents[fsize-1]=='\n') 
	contents[fsize]='\0';
      else
	contents[fsize]='\n'; // to help the scanner
      contents[fsize+1]='\0';
      return contents;
    }
  }
  if (!fileOpened)  
  {
    config_err("Error: cannot open file `%s' for reading\n",name);
  }
  return "";
}

bool Config::parseString(const char *fn,const char *str)
{
  config = Config::instance();
  inputString   = str;
  inputPosition = 0;
  yyFileName    = fn;
  yyLineNr      = 1;
  includeStack.setAutoDelete(TRUE);
  includeStack.clear();
  includeDepth  = 0;
  configYYrestart( configYYin );
  BEGIN( Start );
  configYYlex();
  inputString = 0;
  return TRUE;
}

bool Config::parse(const char *fn)
{
  encoding = "UTF-8";
  return parseString(fn,configFileToString(fn)); 
}

extern "C" { // some bogus code to keep the compiler happy
  //int  configYYwrap() { return 1 ; }
}
