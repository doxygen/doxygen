/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
#include "util.h"

#include "lang_cfg.h"
#include "configoptions.h"

#undef Config_getString
#undef Config_getInt
#undef Config_getList
#undef Config_getEnum
#undef Config_getBool

#define YY_NO_INPUT 1

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
    QCString tmp=s.stripWhiteSpace();
    char *p=tmp.data();
    char c;
    result+="#";
    if (*p && *p!='\n')
      result+=" ";
    while ((c=*p++))
    {
      if (c=='\n')
      {
        result+="\n#";
        if (*p && *p!='\n')
          result+=" ";
      }
      else result+=c;
    }
    result+='\n';
  }
  return result;
}

void ConfigOption::writeBoolValue(FTextStream &t,bool v)
{
  t << " ";
  if (v) t << "YES"; else t << "NO";
}

void ConfigOption::writeIntValue(FTextStream &t,int i)
{
  t << " " << i;
}

void ConfigOption::writeStringValue(FTextStream &t,QCString &s)
{
  char c;
  bool needsEscaping=FALSE;
  // convert the string back to it original encoding
  QCString se = configStringRecode(s,"UTF-8",m_encoding);
  const char *p=se.data();
  if (p)
  {
    t << " ";
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

void ConfigOption::writeStringList(FTextStream &t,QStrList &l)
{
  const char *p = l.first();
  bool first=TRUE;
  while (p)
  {
    QCString s=p;
    if (!first)
      t << "                        ";
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
      config_warn("warning: argument `%s' for option %s is not a valid number in the range [%d..%d]!\n"
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
    if (val=="yes" || val=="true" || val=="1" || val=="all") 
    {
      m_value=TRUE;
    }
    else if (val=="no" || val=="false" || val=="0" || val=="none")
    {
      m_value=FALSE;
    }
    else
    {
      config_warn("warning: argument `%s' for option %s is not a valid boolean value\n"
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
 */

void ConfigInt::writeXML(FTextStream& t)
{
  t << "    <option type='int' "
       "id='" << convertToXML(name()) << "' "
       "docs='\n" << convertToXML(docs()) << "' "
       "minval='" << m_minVal << "' "
       "maxval='" << m_maxVal << "' "
       "defval='" << m_defValue << "'";
  if (!m_dependency.isEmpty()) t << " depends='" << m_dependency << "'";
  t << "/>" << endl;
}

void ConfigBool::writeXML(FTextStream& t)
{
  t << "    <option type='bool' "
       "id='" << convertToXML(name()) << "' "
       "docs='\n" << convertToXML(docs()) << "' "
       "defval='" << m_defValue << "'";
  if (!m_dependency.isEmpty()) t << " depends='" << m_dependency << "'";
  t << "/>" << endl;
}

void ConfigString::writeXML(FTextStream& t)
{
  QString format;
  switch (m_widgetType)
  {
    case String: format="string"; break;
    case File:   format="file";   break;
    case Dir:    format="dir";    break;
  }
  t << "    <option type='string' "
       "id='" << convertToXML(name()) << "' "
       "format='" << format << "' "
       "docs='\n" << convertToXML(docs()) << "' "
       "defval='" << convertToXML(m_defValue) << "'";
  if (!m_dependency.isEmpty()) t << " depends='" << m_dependency << "'";
  t << "/>" << endl;
}

void ConfigEnum::writeXML(FTextStream &t)
{
  t << "    <option type='enum' "
       "id='" << convertToXML(name()) << "' "
       "defval='" << convertToXML(m_defValue) << "' "
       "docs='\n" << convertToXML(docs()) << "'";
  if (!m_dependency.isEmpty()) t << " depends='" << m_dependency << "'";
  t << ">" << endl;

  char *enumVal = m_valueRange.first();
  while (enumVal)
  {
    t << "      <value name='" << convertToXML(enumVal) << "'/>" << endl;
    enumVal = m_valueRange.next();
  }

  t << "    </option>" << endl;
}

void ConfigList::writeXML(FTextStream &t)
{
  QString format;
  switch (m_widgetType)
  {
    case String:     format="string";  break;
    case File:       format="file";    break;
    case Dir:        format="dir";     break;
    case FileAndDir: format="filedir"; break;
  }
  t << "    <option type='list' "
       "id='" << convertToXML(name()) << "' "
       "format='" << format << "' "
       "docs='\n" << convertToXML(docs()) << "'";
  if (!m_dependency.isEmpty()) t << " depends='" << m_dependency << "'";
  t << ">" << endl;
  char *enumVal = m_value.first();
  while (enumVal)
  {
    t << "      <value name='" << convertToXML(enumVal) << "'/>" << endl;
    enumVal = m_value.next();
  }

  t << "    </option>" << endl;
}

void ConfigObsolete::writeXML(FTextStream &t)
{
  t << "    <option type='obsolete' "
       "id='" << convertToXML(name()) << "'/>" << endl;
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
  int outputSize=inputSize*4+1;
  QCString output(outputSize);
  void *cd = portable_iconv_open(outputEncoding,inputEncoding);
  if (cd==(void *)(-1)) 
  {
    fprintf(stderr,"error: unsupported character conversion: '%s'->'%s'\n",
        inputEncoding.data(),outputEncoding.data());
    exit(1);
  }
  size_t iLeft=(size_t)inputSize;
  size_t oLeft=(size_t)outputSize;
  char *inputPtr  = str.data();
  char *outputPtr = output.data();
  if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
  {
    outputSize-=(int)oLeft;
    output.resize(outputSize+1);
    output.at(outputSize)='\0';
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,srcBuf.data());
  }
  else
  {
    fprintf(stderr,"error: failed to translate characters from %s to %s: %s\n",
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
    FILE *f=portable_fopen(absName,"r");
    if (!f) config_err("error: could not open file %s for reading\n",absName.data());
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
    config_err("error: maximum include depth (%d) reached, %s is not included. Aborting...\n",
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
    config_err("error: @INCLUDE = %s: not found!\n",inc.data());
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
					     config_err("warning: ignoring unsupported tag `%s' at line %d, file %s\n",
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
					         config_err("warning: Tag `%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please remove this line from your configuration "
							    "file or upgrade it using \"doxygen -u\"\n", cmd.data(),yyLineNr,yyFileName.data()); 
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
					    config_err("warning: ignoring unsupported tag `%s' at line %d, file %s\n",
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
					        config_err("warning: operator += not supported for `%s'. Ignoring line at line %d, file %s\n",
						    yytext,yyLineNr,yyFileName.data()); 
					        BEGIN(SkipInvalid);
						break;
					       case ConfigOption::O_Obsolete:
					         config_err("warning: Tag `%s' at line %d of file %s has become obsolete.\n"
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

<Start>[a-z_A-Z0-9]+			{ config_err("warning: ignoring unknown tag `%s' at line %d, file %s\n",yytext,yyLineNr,yyFileName.data()); }
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
					    config_err("warning: Missing end quote (\") on line %d, file %s\n",yyLineNr,yyFileName.data());
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
					    config_warn("warning: Invalid value `%s' for "
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

void Config::writeTemplate(FTextStream &t,bool sl,bool upd)
{
  t << "# Doxyfile " << versionString << endl << endl;
  if (!sl)
  {
    t << "# This file describes the settings to be used by the documentation system\n";
    t << "# doxygen (www.doxygen.org) for a project.\n";
    t << "#\n";
    t << "# All text after a hash (#) is considered a comment and will be ignored.\n";
    t << "# The format is:\n";
    t << "#       TAG = value [value, ...]\n";
    t << "# For lists items can also be appended using:\n";
    t << "#       TAG += value [value, ...]\n";
    t << "# Values that contain spaces should be placed between quotes (\" \").\n";
  }
  ConfigOption *option = m_options->first();
  while (option)
  {
    option->writeTemplate(t,sl,upd);
    option = m_options->next();
  }
}

void Config::writeXML(FTextStream &t)
{
  t << "<doxygenconfig>" << endl;
  bool first=TRUE;
  ConfigOption *option = m_options->first();
  while (option)
  {
    if (option->kind()==ConfigOption::O_Info)
    {
      if (!first) t << "  </group>" << endl;
      t << "  <group name='" << option->name() << "' "
	   "docs='"        << option->docs() << "'>" << endl;
      first=FALSE;
    }
    else
    {
      option->writeXML(t);
    }
    option = m_options->next();
  }
  option = m_obsolete->first();
  while (option)
  {
    option->writeXML(t);
    option = m_obsolete->next();
  }
  if (!first) t << "  </group>" << endl;
  t << "</doxygenconfig>" << endl;
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
  static QRegExp re("\\$\\([a-z_A-Z0-9.-]+\\)");
  static QRegExp re2("\\$\\([a-z_A-Z0-9.-]+\\([a-z_A-Z0-9.-]+\\)\\)"); // For e.g. PROGRAMFILES(X86)
  if (s.isEmpty()) return;
  int p=0;
  int i,l;
  //printf("substEnvVarInString(%s) start\n",s.data());
  while ((i=re.match(s,p,&l))!=-1 || (i=re2.match(s,p,&l))!=-1)
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
		       add to sl. If the original string already 
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
	  str.insert(i,fi.absFilePath().utf8()+"/");
	else
	  str.append(fi.absFilePath().utf8()+"/");
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
      config_err("warning: warning format does not contain a $file tag!\n");
    }
    if (warnFormat.find("$line")==-1)
    {
      config_err("warning: warning format does not contain a $line tag!\n");
    }
    if (warnFormat.find("$text")==-1)
    {
      config_err("warning: warning format foes not contain a $text tag!\n");
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
    paperType = "a4";
  }
  if (paperType!="a4" && paperType!="a4wide" && paperType!="letter" && 
      paperType!="legal" && paperType!="executive")
  {
    config_err("error: Unknown page type specified");
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
    stripFromPath.append(QDir::currentDirPath().utf8()+"/");
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
      config_err("error: tag HTML_HEADER: header file `%s' "
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
      config_err("error: tag HTML_FOOTER: footer file `%s' "
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
      config_err("error: tag LATEX_HEADER: header file `%s' "
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
    if (!fi.exists()) config_err("warning: tag INCLUDE_PATH: include path `%s' "
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

  // check if GENERATE_TREEVIEW and GENERATE_HTMLHELP are both enabled
  if (Config_getBool("GENERATE_TREEVIEW") && Config_getBool("GENERATE_HTMLHELP"))
  {
    config_err("When enabling GENERATE_HTMLHELP the tree view (GENERATE_TREEVIEW) should be disabled. I'll do it for you.\n");
    Config_getBool("GENERATE_TREEVIEW")=FALSE;
  }
  if (Config_getBool("SEARCHENGINE") && Config_getBool("GENERATE_HTMLHELP"))
  {
    config_err("When enabling GENERATE_HTMLHELP the search engine (SEARCHENGINE) should be disabled. I'll do it for you.\n");
    Config_getBool("SEARCHENGINE")=FALSE;
  }

  // check if SEPARATE_MEMBER_PAGES and INLINE_GROUPED_CLASSES are both enabled
  if (Config_getBool("SEPARATE_MEMBER_PAGES") && Config_getBool("INLINE_GROUPED_CLASSES"))
  {
    config_err("When enabling INLINE_GROUPED_CLASSES the SEPARATE_MEMBER_PAGES option should be disabled. I'll do it for you.\n");
    Config_getBool("SEPARATE_MEMBER_PAGES")=FALSE;
  }
    
  // check dot image format
  QCString &dotImageFormat=Config_getEnum("DOT_IMAGE_FORMAT");
  dotImageFormat=dotImageFormat.stripWhiteSpace();
  if (dotImageFormat.isEmpty())
  {
    dotImageFormat = "png";
  }
  //else if (dotImageFormat!="gif" && dotImageFormat!="png" && dotImageFormat!="jpg")
  //{
  //  config_err("Invalid value for DOT_IMAGE_FORMAT: `%s'. Using the default.\n",dotImageFormat.data());
  //  dotImageFormat = "png";
  //}
  
  
  // check dot path
  QCString &dotPath = Config_getString("DOT_PATH");
  if (!dotPath.isEmpty())
  {
    QFileInfo fi(dotPath);
    if (fi.exists() && fi.isFile()) // user specified path + exec
    {
      dotPath=fi.dirPath(TRUE).utf8()+"/";
    }
    else
    {
      QFileInfo dp(dotPath+"/dot"+portable_commandExtension());
      if (!dp.exists() || !dp.isFile())
      {
	config_err("warning: the dot tool could not be found at %s\n",dotPath.data());
	dotPath="";
      }
      else
      {
	dotPath=dp.dirPath(TRUE).utf8()+"/";
      }
    }
#if defined(_WIN32) // convert slashes
    uint i=0,l=dotPath.length();
    for (i=0;i<l;i++) if (dotPath.at(i)=='/') dotPath.at(i)='\\';
#endif
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
      config_err("warning: the mscgen tool could not be found at %s\n",mscgenPath.data());
      mscgenPath="";
    }
    else
    {
      mscgenPath=dp.dirPath(TRUE).utf8()+"/";
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
    inputSources.append(QDir::currentDirPath().utf8());
  }
  else
  {
    s=inputSources.first();
    while (s)
    {
      QFileInfo fi(s);
      if (!fi.exists())
      {
	config_err("warning: tag INPUT: input source `%s' does not exist\n",s);
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
    filePatternList.append("*.for");
    filePatternList.append("*.vhd");
    filePatternList.append("*.vhdl");
    filePatternList.append("*.tcl");
    filePatternList.append("*.md");
    filePatternList.append("*.markdown");
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
      filePatternList.append("*.TCL");
      filePatternList.append("*.MD");
      filePatternList.append("*.MARKDOWN");
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
    config_err("warning: No output formats selected! Set at least one of the main GENERATE_* options to YES.\n");
  }

  // check HTMLHELP creation requirements
  if (!Config_getBool("GENERATE_HTML") && 
      Config_getBool("GENERATE_HTMLHELP"))
  {
    config_err("warning: GENERATE_HTMLHELP=YES requires GENERATE_HTML=YES.\n");
  }

  // check QHP creation requirements
  if (Config_getBool("GENERATE_QHP"))
  {
    if (Config_getString("QHP_NAMESPACE").isEmpty())
    {
      config_err("error: GENERATE_QHP=YES requires QHP_NAMESPACE to be set. Using 'org.doxygen.doc' as default!.\n");
      Config_getString("QHP_NAMESPACE")="org.doxygen.doc";
    }

    if (Config_getString("QHP_VIRTUAL_FOLDER").isEmpty())
    {
      config_err("error: GENERATE_QHP=YES requires QHP_VIRTUAL_FOLDER to be set. Using 'doc' as default!\n");
      Config_getString("QHP_VIRTUAL_FOLDER")="doc";
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

  int &hue = Config_getInt("HTML_COLORSTYLE_HUE");
  if (hue<0)
  {
    hue=0;
  }
  else if (hue>=360)
  {
    hue=hue%360;
  }

  int &sat = Config_getInt("HTML_COLORSTYLE_SAT");
  if (sat<0)
  {
    sat=0;
  }
  else if (sat>255)
  {
    sat=255;
  }
  int &gamma = Config_getInt("HTML_COLORSTYLE_GAMMA");
  if (gamma<40)
  {
    gamma=40;
  }
  else if (gamma>240)
  {
    gamma=240;
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
       !Config_getBool("EXTRACT_PRIVATE") ||
       !Config_getBool("EXTRACT_PACKAGE")
      )
     )
  {
    bool b1 = Config_getBool("INLINE_INHERITED_MEMB");
    bool b2 = Config_getBool("INHERIT_DOCS");
    bool b3 = Config_getBool("HIDE_SCOPE_NAMES");
    bool b4 = Config_getBool("EXTRACT_PRIVATE");
    bool b5 = Config_getBool("SKIP_FUNCTION_MACROS");
    bool b6 = Config_getBool("EXTRACT_PACKAGE");
    const char *s1,*s2,*s3,*s4,*s5,*s6;
    if (b1)  s1="  INLINE_INHERITED_MEMB  = NO (was YES)\n"; else s1="";
    if (b2)  s2="  INHERIT_DOCS           = NO (was YES)\n"; else s2="";
    if (!b3) s3="  HIDE_SCOPE_NAMES       = YES (was NO)\n"; else s3="";
    if (!b4) s4="  EXTRACT_PRIVATE        = YES (was NO)\n"; else s4="";
    if (b5)  s5="  ENABLE_PREPROCESSING   = NO (was YES)\n"; else s5="";
    if (!b6) s6="  EXTRACT_PACKAGE        = YES (was NO)\n"; else s6="";


    config_err("warning: enabling OPTIMIZE_OUTPUT_VHDL assumes the following settings:\n"
	       "%s%s%s%s%s%s",s1,s2,s3,s4,s5,s6
	      );

    Config_getBool("INLINE_INHERITED_MEMB") = FALSE;
    Config_getBool("INHERIT_DOCS")          = FALSE;
    Config_getBool("HIDE_SCOPE_NAMES")      = TRUE;
    Config_getBool("EXTRACT_PRIVATE")       = TRUE;
    Config_getBool("ENABLE_PREPROCESSING")  = FALSE;
    Config_getBool("EXTRACT_PACKAGE")       = TRUE;
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
  addConfigOptions(this);
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
      config_err("error: file `%s' not found\n",name);
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
    config_err("error: cannot open file `%s' for reading\n",name);
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
