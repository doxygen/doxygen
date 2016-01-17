/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */
%option never-interactive
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
  
#include "configimpl.h"
#include "version.h"
#include "portable.h"
#include "util.h"
#include "message.h"

#include "lang_cfg.h"
#include "configoptions.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

static const char *warning_str = "warning: ";
static const char *error_str = "error: ";

void config_err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, (QCString(error_str) + fmt).data(), args);
  va_end(args); 
}
void config_warn(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, (QCString(warning_str) + fmt).data(), args);
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
static QCString convertToComment(const QCString &s, const QCString &u)
{
  //printf("convertToComment(%s)=%s\n",s.data(),u.data());
  QCString result;
  if (!s.isEmpty())
  {
    QCString tmp=s.stripWhiteSpace();
    const char *p=tmp.data();
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
  if (!u.isEmpty())
  {
    if (!result.isEmpty()) result+='\n';
    result+= u;
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

ConfigImpl *ConfigImpl::m_instance = 0;

void ConfigInt::convertStrToVal() 
{
  if (!m_valueString.isEmpty())
  {
    bool ok;
    int val = m_valueString.toInt(&ok);
    if (!ok || val<m_minVal || val>m_maxVal)
    {
      config_warn("argument `%s' for option %s is not a valid number in the range [%d..%d]!\n"
                "Using the default: %d!\n",m_valueString.data(),m_name.data(),m_minVal,m_maxVal,m_value);
    }
    else
    {
      m_value=val;
    }
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
      config_warn("argument `%s' for option %s is not a valid boolean value\n"
                "Using the default: %s!\n",m_valueString.data(),m_name.data(),m_value?"YES":"NO");
    }
  }
}

QCString &ConfigImpl::getString(const char *fileName,int num,const char *name) const
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

QStrList &ConfigImpl::getList(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_List)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of list type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigList *)opt)->valueRef();
}

QCString &ConfigImpl::getEnum(const char *fileName,int num,const char *name) const
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

int &ConfigImpl::getInt(const char *fileName,int num,const char *name) const
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

bool &ConfigImpl::getBool(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Bool)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of boolean type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigBool *)opt)->valueRef();
}

/* ------------------------------------------ */

void ConfigInfo::writeTemplate(FTextStream &t, bool sl,bool)
{
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# " << m_doc << endl;
  t << "#---------------------------------------------------------------------------\n";
}

void ConfigList::writeTemplate(FTextStream &t,bool sl,bool)
{
  if (!sl)
  {
    t << endl;
    t << convertToComment(m_doc, m_userComment);
    t << endl;
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  writeStringList(t,m_value);
  t << "\n";
}

void ConfigEnum::writeTemplate(FTextStream &t,bool sl,bool)
{
  if (!sl)
  {
    t << endl;
    t << convertToComment(m_doc, m_userComment);
    t << endl;
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  writeStringValue(t,m_value);
  t << "\n";
}

void ConfigString::writeTemplate(FTextStream &t,bool sl,bool)
{
  if (!sl)
  {
    t << endl;
    t << convertToComment(m_doc, m_userComment);
    t << endl;
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  writeStringValue(t,m_value);
  t << "\n";
}

void ConfigInt::writeTemplate(FTextStream &t,bool sl,bool upd)
{
  if (!sl)
  {
    t << endl;
    t << convertToComment(m_doc, m_userComment);
    t << endl;
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  if (upd && !m_valueString.isEmpty())
  {
    writeStringValue(t,m_valueString);
  }
  else
  {
    writeIntValue(t,m_value);
  }
  t << "\n";
}

void ConfigBool::writeTemplate(FTextStream &t,bool sl,bool upd)
{
  if (!sl)
  {
    t << endl;
    t << convertToComment(m_doc, m_userComment);
    t << endl;
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  if (upd && !m_valueString.isEmpty())
  {
    writeStringValue(t,m_valueString);
  }
  else
  {
    writeBoolValue(t,m_value);
  }
  t << "\n";
}

void ConfigObsolete::writeTemplate(FTextStream &,bool,bool) {}
void ConfigDisabled::writeTemplate(FTextStream &,bool,bool) {}

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
static QStrList          includePathList;
static QStack<ConfigFileState> includeStack;  
static int               includeDepth;
static bool              config_upd = FALSE;
static QCString          encoding;
static ConfigImpl       *config;

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
    fprintf(stderr,"Error: unsupported character conversion: '%s'->'%s'\n",
        inputEncoding.data(),outputEncoding.data());
    exit(1);
  }
  size_t iLeft=(size_t)inputSize;
  size_t oLeft=(size_t)outputSize;
  char *inputPtr  = str.rawData();
  char *outputPtr = output.rawData();
  if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
  {
    outputSize-=(int)oLeft;
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
    FILE *f=portable_fopen(absName,"r");
    if (!f) config_err("could not open file %s for reading\n",absName.data());
    return f;
  }
  return 0;
}

static void substEnvVarsInStrList(QStrList &sl);
static void substEnvVarsInString(QCString &s);

static FILE *findFile(const char *fileName)
{
  if (fileName==0)
  {
    return 0;
  }
  if (portable_isAbsolutePath(fileName))
  {
    return tryPath(NULL, fileName);
  }
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
    config_err("maximum include depth (%d) reached, %s is not included. Aborting...\n",
	MAX_INCLUDE_DEPTH,incName);
    exit(1);
  } 

  QCString inc = incName;
  substEnvVarsInString(inc);
  inc = inc.stripWhiteSpace();
  uint incLen = inc.length();
  if (incLen>0 && inc.at(0)=='"' && inc.at(incLen-1)=='"') // strip quotes
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
    config_err("@INCLUDE = %s: not found!\n",inc.data());
    exit(1);
  }
}


%}

%option noyywrap

%x      PreStart
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
<PreStart>"##".*"\n" { config->appendStartComment(yytext);}
<PreStart>. {
              BEGIN(Start);
              unput(*yytext);
            }
<Start,GetString,GetStrList,GetBool,SkipInvalid>"##".*"\n" { config->appendUserComment(yytext);}
<Start,GetString,GetStrList,GetBool,SkipInvalid>"#"	   { BEGIN(SkipComment); }
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"="	 { QCString cmd=yytext;
                                           cmd=cmd.left(cmd.length()-1).stripWhiteSpace(); 
					   ConfigOption *option = config->get(cmd);
					   if (option==0) // oops not known
					   {
					     config_warn("ignoring unsupported tag `%s' at line %d, file %s\n",
						 yytext,yyLineNr,yyFileName.data()); 
					     BEGIN(SkipInvalid);
					   }
					   else // known tag
					   {
                                             option->setUserComment(config->takeUserComment());
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
                                                 if (config_upd)
                                                 {
					           config_warn("Tag `%s' at line %d of file `%s' has become obsolete.\n"
						              "         This tag has been removed.\n", cmd.data(),yyLineNr,yyFileName.data());
                                                 }
                                                 else
                                                 {
					           config_warn("Tag `%s' at line %d of file `%s' has become obsolete.\n"
						              "         To avoid this warning please remove this line from your configuration "
							      "file or upgrade it using \"doxygen -u\"\n", cmd.data(),yyLineNr,yyFileName.data()); 
                                                 }
					         BEGIN(SkipInvalid);
						 break;
					       case ConfigOption::O_Disabled:
                                                 if (config_upd)
                                                 {
					           config_warn("Tag `%s' at line %d of file `%s' belongs to an option that was not enabled at compile time.\n"
						              "         This tag has been removed.\n", cmd.data(),yyLineNr,yyFileName.data());
                                                 }
                                                 else
                                                 {
					           config_warn("Tag `%s' at line %d of file `%s' belongs to an option that was not enabled at compile time.\n"
						              "         To avoid this warning please remove this line from your configuration "
							    "file or upgrade it using \"doxygen -u\", or recompile doxygen with this feature enabled.\n", cmd.data(),yyLineNr,yyFileName.data()); 
                                                 }
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
					    config_warn("ignoring unsupported tag `%s' at line %d, file %s\n",
						yytext,yyLineNr,yyFileName.data()); 
					    BEGIN(SkipInvalid);
					  }
					  else // known tag
					  {
                                            option->setUserComment(config->takeUserComment());
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
					        config_warn("operator += not supported for `%s'. Ignoring line at line %d, file %s\n",
						    yytext,yyLineNr,yyFileName.data()); 
					        BEGIN(SkipInvalid);
						break;
					       case ConfigOption::O_Obsolete:
					         config_warn("Tag `%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", cmd.data(),yyLineNr,yyFileName.data()); 
					         BEGIN(SkipInvalid);
						 break;
					       case ConfigOption::O_Disabled:
					         config_warn("Tag `%s' at line %d of file %s belongs to an option that was not enabled at compile time.\n"
						            "To avoid this warning please remove this line from your configuration "
							    "file, upgrade it using \"doxygen -u\", or recompile doxygen with this feature enabled.\n", cmd.data(),yyLineNr,yyFileName.data()); 
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

<Start>[a-z_A-Z0-9]+			{ config_warn("ignoring unknown tag `%s' at line %d, file %s\n",yytext,yyLineNr,yyFileName.data()); }
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
					    config_warn("Missing end quote (\") on line %d, file %s\n",yyLineNr,yyFileName.data());
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
					    config_warn("Invalid value `%s' for "
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

void ConfigImpl::writeTemplate(FTextStream &t,bool sl,bool upd)
{
  /* print first lines of user comment that were at the beginning of the file, might have special meaning for editors */
  if (m_startComment)
  {
    t << takeStartComment() << endl; 
  }
  t << "# Doxyfile " << versionString << endl << endl;
  if (!sl)
  {
    t << convertToComment(m_header,"");
  }
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (;(option=it.current());++it)
  {
    option->writeTemplate(t,sl,upd);
  }
  /* print last lines of user comment that were at the end of the file */
  if (m_userComment)
  {
    t << "\n";
    t << takeUserComment();
  }
}

void ConfigImpl::convertStrToVal()
{
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (;(option=it.current());++it)
  {
    option->convertStrToVal();
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

//---------------------------------------------

void ConfigImpl::substituteEnvironmentVars()
{
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (;(option=it.current());++it)
  {
    option->substEnvVars();
  }
}

void ConfigImpl::init()
{
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (;(option=it.current());++it)
  {
    option->init();
  }

  // sanity check if all depends relations are valid
  for (it.toFirst();(option=it.current());++it)
  {
    QCString depName = option->dependsOn();
    if (!depName.isEmpty())
    {
      ConfigOption * opt = ConfigImpl::instance()->get(depName);
      if (opt==0)
      {
        config_warn("Config option '%s' has invalid depends relation on unknown option '%s'\n",
            option->name().data(),depName.data());
        exit(1);
      }
    }
  }
}

void ConfigImpl::create()
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
      while ((size=f.readBlock(contents.rawData()+totalSize,bSize))==bSize)
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
      config_err("file `%s' not found\n",name);
      return "";
    }
    f.setName(name);
    fileOpened=f.open(IO_ReadOnly);
    if (fileOpened)
    {
      int fsize=f.size();
      QCString contents(fsize+2);
      f.readBlock(contents.rawData(),fsize);
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
    config_err("cannot open file `%s' for reading\n",name);
    exit(1);
  }
  return "";
}

bool ConfigImpl::parseString(const char *fn,const char *str,bool update)
{
  config = ConfigImpl::instance();
  inputString   = str;
  inputPosition = 0;
  yyFileName    = fn;
  yyLineNr      = 1;
  includeStack.setAutoDelete(TRUE);
  includeStack.clear();
  includeDepth  = 0;
  configimplYYrestart( configimplYYin );
  BEGIN( PreStart );
  config_upd = update;
  configimplYYlex();
  config_upd = FALSE;
  inputString = 0;
  return TRUE;
}

bool ConfigImpl::parse(const char *fn,bool update)
{
  int retval;
  encoding = "UTF-8";
  printlex(yy_flex_debug, TRUE, __FILE__, fn);
  retval =  parseString(fn,configFileToString(fn), update); 
  printlex(yy_flex_debug, FALSE, __FILE__, fn);
  return retval;
}

//----------------------------------------------------------------------

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

static void checkFileName(QCString &s,const char *optionName)
{
  QCString val = s.stripWhiteSpace().lower();
  if ((val=="yes" || val=="true"  || val=="1" || val=="all") ||
      (val=="no"  || val=="false" || val=="0" || val=="none"))
  {
    err("file name expected for option %s, got %s instead. Ignoring...\n",optionName,s.data());
    s=""; // note the use of &s above: this will change the option value!
  }
}

static void initFilePattern(void)
{
  // add default pattern if needed
  QStrList &filePatternList = ConfigImpl_getList("FILE_PATTERNS");
  if (filePatternList.isEmpty())
  {
    QDictIterator<int> it( getExtensionLookup() );
    QCString pattern;
    bool caseSens =  portable_fileSystemIsCaseSensitive();
    for (;it.current();++it)
    {
      pattern = "*";
      pattern += it.currentKey();
      filePatternList.append(pattern.data());
      if (caseSens) filePatternList.append(pattern.upper().data());
    }
  }
}


#include "config.h"

void Config::init()
{
  ConfigImpl::instance()->init();
}

void Config::checkAndCorrect()
{
  QCString &warnFormat = ConfigImpl_getString("WARN_FORMAT");
  if (warnFormat.stripWhiteSpace().isEmpty())
  {
    warnFormat="$file:$line $text";
  }
  else
  {
    if (warnFormat.find("$file")==-1)
    {
      warn_uncond("warning format does not contain a $file tag!\n");

    }
    if (warnFormat.find("$line")==-1)
    {
      warn_uncond("warning format does not contain a $line tag!\n");
    }
    if (warnFormat.find("$text")==-1)
    {
      warn_uncond("warning format foes not contain a $text tag!\n");
    }
  }

  QCString &manExtension = ConfigImpl_getString("MAN_EXTENSION");

  // set default man page extension if non is given by the user
  if (manExtension.isEmpty())
  {
    manExtension=".3";
  }

  QCString &paperType = ConfigImpl_getEnum("PAPER_TYPE");
  paperType=paperType.lower().stripWhiteSpace();
  if (paperType.isEmpty() || paperType=="a4wide")
  {
    paperType = "a4";
  }
  if (paperType!="a4" && paperType!="letter" &&
      paperType!="legal" && paperType!="executive")
  {
    err("Unknown page type specified\n");
    paperType="a4";
  }

  QCString &outputLanguage=ConfigImpl_getEnum("OUTPUT_LANGUAGE");
  outputLanguage=outputLanguage.stripWhiteSpace();
  if (outputLanguage.isEmpty())
  {
    outputLanguage = "English";
  }

  QCString &htmlFileExtension=ConfigImpl_getString("HTML_FILE_EXTENSION");
  htmlFileExtension=htmlFileExtension.stripWhiteSpace();
  if (htmlFileExtension.isEmpty())
  {
    htmlFileExtension = ".html";
  }

  // expand the relative stripFromPath values
  QStrList &stripFromPath = ConfigImpl_getList("STRIP_FROM_PATH");
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
  QStrList &stripFromIncPath = ConfigImpl_getList("STRIP_FROM_INC_PATH");
  cleanUpPaths(stripFromIncPath);

  // Test to see if HTML header is valid
  QCString &headerFile = ConfigImpl_getString("HTML_HEADER");
  if (!headerFile.isEmpty())
  {
    QFileInfo fi(headerFile);
    if (!fi.exists())
    {
      err("tag HTML_HEADER: header file `%s' "
	  "does not exist\n",headerFile.data());
      exit(1);
    }
  }
  // Test to see if HTML footer is valid
  QCString &footerFile = ConfigImpl_getString("HTML_FOOTER");
  if (!footerFile.isEmpty())
  {
    QFileInfo fi(footerFile);
    if (!fi.exists())
    {
      err("tag HTML_FOOTER: footer file `%s' "
	  "does not exist\n",footerFile.data());
      exit(1);
    }
  }

  // Test to see if MathJax code file is valid
  if (ConfigImpl_getBool("USE_MATHJAX"))
  {
    QCString &MathJaxCodefile = ConfigImpl_getString("MATHJAX_CODEFILE");
    if (!MathJaxCodefile.isEmpty())
    {
      QFileInfo fi(MathJaxCodefile);
      if (!fi.exists())
      {
        err("tag MATHJAX_CODEFILE file `%s' "
	    "does not exist\n",MathJaxCodefile.data());
        exit(1);
      }
    }
    QCString &path = ConfigImpl_getString("MATHJAX_RELPATH");
    if (!path.isEmpty() && path.at(path.length()-1)!='/')
    {
      path+="/";
    }

  }

  // Test to see if LaTeX header is valid
  QCString &latexHeaderFile = ConfigImpl_getString("LATEX_HEADER");
  if (!latexHeaderFile.isEmpty())
  {
    QFileInfo fi(latexHeaderFile);
    if (!fi.exists())
    {
      err("tag LATEX_HEADER: header file `%s' "
	  "does not exist\n",latexHeaderFile.data());
      exit(1);
    }
  }
  // Test to see if LaTeX footer is valid
  QCString &latexFooterFile = ConfigImpl_getString("LATEX_FOOTER");
  if (!latexFooterFile.isEmpty())
  {
    QFileInfo fi(latexFooterFile);
    if (!fi.exists())
    {
      err("tag LATEX_FOOTER: footer file `%s' "
	  "does not exist\n",latexFooterFile.data());
      exit(1);
    }
  }

  // check include path
  QStrList &includePath = ConfigImpl_getList("INCLUDE_PATH");
  char *s=includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    if (!fi.exists()) warn_uncond("tag INCLUDE_PATH: include path `%s' "
	                  "does not exist\n",s);
    s=includePath.next();
  }

  // check aliases
  QStrList &aliasList = ConfigImpl_getList("ALIASES");
  s=aliasList.first();
  while (s)
  {
    QRegExp re1("[a-z_A-Z][a-z_A-Z0-9]*[ \t]*=");         // alias without argument
    QRegExp re2("[a-z_A-Z][a-z_A-Z0-9]*{[0-9]*}[ \t]*="); // alias with argument
    QCString alias=s;
    alias=alias.stripWhiteSpace();
    if (alias.find(re1)!=0 && alias.find(re2)!=0)
    {
      err("Illegal alias format `%s'. Use \"name=value\" or \"name(n)=value\", where n is the number of arguments\n",
	  alias.data());
    }
    s=aliasList.next();
  }

  // check if GENERATE_TREEVIEW and GENERATE_HTMLHELP are both enabled
  if (ConfigImpl_getBool("GENERATE_TREEVIEW") && ConfigImpl_getBool("GENERATE_HTMLHELP"))
  {
    err("When enabling GENERATE_HTMLHELP the tree view (GENERATE_TREEVIEW) should be disabled. I'll do it for you.\n");
    ConfigImpl_getBool("GENERATE_TREEVIEW")=FALSE;
  }
  if (ConfigImpl_getBool("SEARCHENGINE") && ConfigImpl_getBool("GENERATE_HTMLHELP"))
  {
    err("When enabling GENERATE_HTMLHELP the search engine (SEARCHENGINE) should be disabled. I'll do it for you.\n");
    ConfigImpl_getBool("SEARCHENGINE")=FALSE;
  }

  // check if SEPARATE_MEMBER_PAGES and INLINE_GROUPED_CLASSES are both enabled
  if (ConfigImpl_getBool("SEPARATE_MEMBER_PAGES") && ConfigImpl_getBool("INLINE_GROUPED_CLASSES"))
  {
    err("When enabling INLINE_GROUPED_CLASSES the SEPARATE_MEMBER_PAGES option should be disabled. I'll do it for you.\n");
    ConfigImpl_getBool("SEPARATE_MEMBER_PAGES")=FALSE;
  }

  // check dot image format
  QCString &dotImageFormat=ConfigImpl_getEnum("DOT_IMAGE_FORMAT");
  dotImageFormat=dotImageFormat.stripWhiteSpace();
  if (dotImageFormat.isEmpty())
  {
    dotImageFormat = "png";
  }
  //else if (dotImageFormat!="gif" && dotImageFormat!="png" && dotImageFormat!="jpg")
  //{
  //  err("Invalid value for DOT_IMAGE_FORMAT: `%s'. Using the default.\n",dotImageFormat.data());
  //  dotImageFormat = "png";
  //}

  QCString &dotFontName=ConfigImpl_getString("DOT_FONTNAME");
  if (dotFontName=="FreeSans" || dotFontName=="FreeSans.ttf")
  {
    warn_uncond("doxygen no longer ships with the FreeSans font.\n"
               "You may want to clear or change DOT_FONTNAME.\n"
               "Otherwise you run the risk that the wrong font is being used for dot generated graphs.\n");
  }


  // check dot path
  QCString &dotPath = ConfigImpl_getString("DOT_PATH");
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
	warn_uncond("the dot tool could not be found at %s\n",dotPath.data());
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
  QCString &mscgenPath = ConfigImpl_getString("MSCGEN_PATH");
  if (!mscgenPath.isEmpty())
  {
    QFileInfo dp(mscgenPath+"/mscgen"+portable_commandExtension());
    if (!dp.exists() || !dp.isFile())
    {
      warn_uncond("the mscgen tool could not be found at %s\n",mscgenPath.data());
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

  // check plantuml path
  QCString &plantumlJarPath = ConfigImpl_getString("PLANTUML_JAR_PATH");
  if (!plantumlJarPath.isEmpty())
  {
    QFileInfo pu(plantumlJarPath);
    if (pu.exists() && pu.isDir()) // PLANTUML_JAR_PATH is directory
    {
      QFileInfo jar(plantumlJarPath+portable_pathSeparator()+"plantuml.jar");
      if (jar.exists() && jar.isFile())
      {
        plantumlJarPath = jar.dirPath(TRUE).utf8()+portable_pathSeparator();
      }
      else
      {
        err("Jar file plantuml.jar not found at location "
                   "specified via PLANTUML_JAR_PATH: '%s'\n",plantumlJarPath.data());
        plantumlJarPath="";
      }
    }
    else if (pu.exists() && pu.isFile() && plantumlJarPath.right(4)==".jar") // PLANTUML_JAR_PATH is file
    {
      plantumlJarPath = pu.dirPath(TRUE).utf8()+portable_pathSeparator();
    }
    else
    {
      err("path specified via PLANTUML_JAR_PATH does not exist or not a directory: %s\n",
                 plantumlJarPath.data());
      plantumlJarPath="";
    }
  }

  // check dia path
  QCString &diaPath = ConfigImpl_getString("DIA_PATH");
  if (!diaPath.isEmpty())
  {
    QFileInfo dp(diaPath+"/dia"+portable_commandExtension());
    if (!dp.exists() || !dp.isFile())
    {
      warn_uncond("dia could not be found at %s\n",diaPath.data());
      diaPath="";
    }
    else
    {
      diaPath=dp.dirPath(TRUE).utf8()+"/";
#if defined(_WIN32) // convert slashes
      uint i=0,l=diaPath.length();
      for (i=0;i<l;i++) if (diaPath.at(i)=='/') diaPath.at(i)='\\';
#endif
    }
  }
  else // make sure the string is empty but not null!
  {
    diaPath="";
  }

  // check input
  QStrList &inputSources=ConfigImpl_getList("INPUT");
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
	warn_uncond("tag INPUT: input source `%s' does not exist\n",s);
      }
      s=inputSources.next();
    }
  }

  initFilePattern();

  // add default pattern if needed
  QStrList &examplePatternList = ConfigImpl_getList("EXAMPLE_PATTERNS");
  if (examplePatternList.isEmpty())
  {
    examplePatternList.append("*");
  }

  // if no output format is enabled, warn the user
  if (!ConfigImpl_getBool("GENERATE_HTML")    &&
      !ConfigImpl_getBool("GENERATE_LATEX")   &&
      !ConfigImpl_getBool("GENERATE_MAN")     &&
      !ConfigImpl_getBool("GENERATE_RTF")     &&
      !ConfigImpl_getBool("GENERATE_XML")     &&
      !ConfigImpl_getBool("GENERATE_PERLMOD") &&
      !ConfigImpl_getBool("GENERATE_RTF")     &&
      !ConfigImpl_getBool("GENERATE_DOCBOOK") &&
      !ConfigImpl_getBool("GENERATE_AUTOGEN_DEF") &&
      ConfigImpl_getString("GENERATE_TAGFILE").isEmpty()
     )
  {
    warn_uncond("No output formats selected! Set at least one of the main GENERATE_* options to YES.\n");
  }

  // check HTMLHELP creation requirements
  if (!ConfigImpl_getBool("GENERATE_HTML") &&
      ConfigImpl_getBool("GENERATE_HTMLHELP"))
  {
    warn_uncond("GENERATE_HTMLHELP=YES requires GENERATE_HTML=YES.\n");
  }

  // check QHP creation requirements
  if (ConfigImpl_getBool("GENERATE_QHP"))
  {
    if (ConfigImpl_getString("QHP_NAMESPACE").isEmpty())
    {
      err("GENERATE_QHP=YES requires QHP_NAMESPACE to be set. Using 'org.doxygen.doc' as default!.\n");
      ConfigImpl_getString("QHP_NAMESPACE")="org.doxygen.doc";
    }

    if (ConfigImpl_getString("QHP_VIRTUAL_FOLDER").isEmpty())
    {
      err("GENERATE_QHP=YES requires QHP_VIRTUAL_FOLDER to be set. Using 'doc' as default!\n");
      ConfigImpl_getString("QHP_VIRTUAL_FOLDER")="doc";
    }
  }

  if (ConfigImpl_getBool("OPTIMIZE_OUTPUT_JAVA") && ConfigImpl_getBool("INLINE_INFO"))
  {
    // don't show inline info for Java output, since Java has no inline
    // concept.
    ConfigImpl_getBool("INLINE_INFO")=FALSE;
  }

  int &depth = ConfigImpl_getInt("MAX_DOT_GRAPH_DEPTH");
  if (depth==0)
  {
    depth=1000;
  }

  int &hue = ConfigImpl_getInt("HTML_COLORSTYLE_HUE");
  if (hue<0)
  {
    hue=0;
  }
  else if (hue>=360)
  {
    hue=hue%360;
  }

  int &sat = ConfigImpl_getInt("HTML_COLORSTYLE_SAT");
  if (sat<0)
  {
    sat=0;
  }
  else if (sat>255)
  {
    sat=255;
  }
  int &gamma = ConfigImpl_getInt("HTML_COLORSTYLE_GAMMA");
  if (gamma<40)
  {
    gamma=40;
  }
  else if (gamma>240)
  {
    gamma=240;
  }

  QCString mathJaxFormat = ConfigImpl_getEnum("MATHJAX_FORMAT");
  if (!mathJaxFormat.isEmpty() && mathJaxFormat!="HTML-CSS" &&
       mathJaxFormat!="NativeMML" && mathJaxFormat!="SVG")
  {
    err("Unsupported value for MATHJAX_FORMAT: Should be one of HTML-CSS, NativeMML, or SVG\n");
    ConfigImpl_getEnum("MATHJAX_FORMAT")="HTML-CSS";
  }

  // add default words if needed
  QStrList &annotationFromBrief = ConfigImpl_getList("ABBREVIATE_BRIEF");
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
  if (ConfigImpl_getBool("OPTIMIZE_OUTPUT_VHDL") &&
      (ConfigImpl_getBool("INLINE_INHERITED_MEMB") ||
       ConfigImpl_getBool("INHERIT_DOCS") ||
       !ConfigImpl_getBool("HIDE_SCOPE_NAMES") ||
       !ConfigImpl_getBool("EXTRACT_PRIVATE") ||
       !ConfigImpl_getBool("EXTRACT_PACKAGE")
      )
     )
  {
    bool b1 = ConfigImpl_getBool("INLINE_INHERITED_MEMB");
    bool b2 = ConfigImpl_getBool("INHERIT_DOCS");
    bool b3 = ConfigImpl_getBool("HIDE_SCOPE_NAMES");
    bool b4 = ConfigImpl_getBool("EXTRACT_PRIVATE");
    bool b5 = ConfigImpl_getBool("SKIP_FUNCTION_MACROS");
    bool b6 = ConfigImpl_getBool("EXTRACT_PACKAGE");
    const char *s1,*s2,*s3,*s4,*s5,*s6;
    if (b1)  s1="  INLINE_INHERITED_MEMB  = NO (was YES)\n"; else s1="";
    if (b2)  s2="  INHERIT_DOCS           = NO (was YES)\n"; else s2="";
    if (!b3) s3="  HIDE_SCOPE_NAMES       = YES (was NO)\n"; else s3="";
    if (!b4) s4="  EXTRACT_PRIVATE        = YES (was NO)\n"; else s4="";
    if (b5)  s5="  ENABLE_PREPROCESSING   = NO (was YES)\n"; else s5="";
    if (!b6) s6="  EXTRACT_PACKAGE        = YES (was NO)\n"; else s6="";


    warn_uncond("enabling OPTIMIZE_OUTPUT_VHDL assumes the following settings:\n"
	       "%s%s%s%s%s%s",s1,s2,s3,s4,s5,s6
	      );

    ConfigImpl_getBool("INLINE_INHERITED_MEMB") = FALSE;
    ConfigImpl_getBool("INHERIT_DOCS")          = FALSE;
    ConfigImpl_getBool("HIDE_SCOPE_NAMES")      = TRUE;
    ConfigImpl_getBool("EXTRACT_PRIVATE")       = TRUE;
    ConfigImpl_getBool("ENABLE_PREPROCESSING")  = FALSE;
    ConfigImpl_getBool("EXTRACT_PACKAGE")       = TRUE;
  }

  checkFileName(ConfigImpl_getString("GENERATE_TAGFILE"),"GENERATE_TAGFILE");

#if 0 // TODO: this breaks test 25; SOURCEBROWSER = NO and SOURCE_TOOLTIPS = YES.
      // So this and other regressions should be analysed and fixed before this can be enabled
  // disable any boolean options that depend on disabled options
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (it.toFirst();(option=it.current());++it)
  {
    QCString depName = option->dependsOn(); // option has a dependency
    if (!depName.isEmpty())
    {
      ConfigOption * dep = Config::instance()->get(depName);
      if (dep->kind()==ConfigOption::O_Bool &&
          ConfigImpl_getBool("depName")==FALSE) // dependent option is disabled
      {
        if (option->kind()==ConfigOption::O_Bool)
        {
          printf("disabling option %s\n",option->name().data());
          ConfigImpl_getBool("option->name("))=FALSE; // also disable this option
        }
      }
    }
  }
#endif

  ConfigValues::instance().init();

}

void Config::writeTemplate(FTextStream &t,bool shortList,bool update)
{
  ConfigImpl::instance()->writeTemplate(t,shortList,update);
}

bool Config::parse(const char *fileName,bool update)
{
  return ConfigImpl::instance()->parse(fileName,update);
}

void Config::postProcess(bool clearHeaderAndFooter)
{
  ConfigImpl::instance()->substituteEnvironmentVars();
  ConfigImpl::instance()->convertStrToVal();

  // avoid bootstrapping issues when the config file already
  // refers to the files that we are supposed to parse.
  if (clearHeaderAndFooter)
  {
    Config_getString(HTML_HEADER)="";
    Config_getString(HTML_FOOTER)="";
    Config_getString(LATEX_HEADER)="";
    Config_getString(LATEX_FOOTER)="";
  }
}

void Config::deinit()
{
  ConfigImpl::instance()->deleteInstance();
}

