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
 */
%option never-interactive
%option prefix="configimplYY"
%top{
#include <stdint.h>
}

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
#include <qregexp.h>
#include <qstack.h>
#include <qglobal.h>

#include <thread>
#include <algorithm>

#include "configimpl.h"
#include "version.h"
#include "portable.h"
#include "util.h"
#include "message.h"

#include "lang_cfg.h"
#include "configoptions.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif

static const char *warning_str = "warning: ";
static const char *error_str = "error: ";

void config_err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, (QCString(error_str) + fmt).data(), args);
  va_end(args);
}
void config_term(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, (QCString(error_str) + fmt).data(), args);
  va_end(args);
  fprintf(stderr, "%s\n", "Exiting...");
  exit(1);
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
    if (p)
    {
      result+="#";
      if (*p && *p!='\n')
      {
        result+=" ";
      }
      while ((c=*p++))
      {
        if (c=='\n')
        {
          result+="\n#";
          if (*p && *p!='\n')
          {
            result+=" ";
          }
        }
        else result+=c;
      }
      result+='\n';
    }
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

void ConfigOption::writeStringValue(FTextStream &t,const QCString &s)
{
  char c;
  bool needsEscaping=FALSE;
  // convert the string back to it original g_encoding
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

void ConfigOption::writeStringList(FTextStream &t,const StringVector &l)
{
  bool first=TRUE;
  for (const auto &p : l)
  {
    if (!first) t << " \\" << endl;
    QCString s=p.c_str();
    if (!first)
      t << "                        ";
    writeStringValue(t,s);
    first=FALSE;
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
      config_warn("argument '%s' for option %s is not a valid number in the range [%d..%d]!\n"
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
      config_warn("argument '%s' for option %s is not a valid boolean value\n"
                "Using the default: %s!\n",m_valueString.data(),m_name.data(),m_value?"YES":"NO");
    }
  }
}

void ConfigEnum::convertStrToVal()
{
  if (m_value.isEmpty())
  {
    m_value = m_defValue;
    return;
  }
  QCString val = m_value.stripWhiteSpace().lower();
  const char *s=m_valueRange.first();
  while (s)
  {
    if (QCString(s).lower() == val)
    {
       m_value = s;
       return;
    }
    s = m_valueRange.next();
  }

  config_warn("argument '%s' for option %s is not a valid enum value\n"
              "Using the default: %s!\n",m_value.data(),m_name.data(),m_defValue.data());
  m_value = m_defValue;
}

QCString &ConfigImpl::getString(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0)
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (opt->kind()!=ConfigOption::O_String)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of string type!\n",fileName,num,name);
  }
  return *((ConfigString *)opt)->valueRef();
}

StringVector &ConfigImpl::getList(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0)
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (opt->kind()!=ConfigOption::O_List)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of list type!\n",fileName,num,name);
  }
  return *((ConfigList *)opt)->valueRef();
}

QCString &ConfigImpl::getEnum(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0)
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (opt->kind()!=ConfigOption::O_Enum)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of enum type!\n",fileName,num,name);
  }
  return *((ConfigEnum *)opt)->valueRef();
}

int &ConfigImpl::getInt(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0)
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (opt->kind()!=ConfigOption::O_Int)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of integer type!\n",fileName,num,name);
  }
  return *((ConfigInt *)opt)->valueRef();
}

bool &ConfigImpl::getBool(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0)
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (opt->kind()!=ConfigOption::O_Bool)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of boolean type!\n",fileName,num,name);
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

void ConfigList::compareDoxyfile(FTextStream &t)
{
  auto get_stripped = [](std::string s)             { return QCString(s.c_str()).stripWhiteSpace(); };
  auto is_not_empty = [get_stripped](std::string s) { return !get_stripped(s).isEmpty();            };
  int defCnt = std::count_if(       m_value.begin(),       m_value.end(),is_not_empty);
  int valCnt = std::count_if(m_defaultValue.begin(),m_defaultValue.end(),is_not_empty);
  if ( valCnt != defCnt)
  {
    writeTemplate(t,TRUE,TRUE);
    return;
  }
  auto it1 = m_value.begin();
  auto it2 = m_defaultValue.begin();
  while (it1!=m_value.end() && it2!=m_defaultValue.end())
  {
    // skip over empty values
    while (it1!=m_value.end() && !is_not_empty(*it1))
    {
      ++it1;
    }
    if (it1!=m_value.end()) // non-empty value
    {
      if (get_stripped(*it1) != get_stripped(*it2)) // not the default, write as difference
      {
        writeTemplate(t,TRUE,TRUE);
        return;
      }
      ++it1;
      ++it2;
    }
  }
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

void ConfigEnum::compareDoxyfile(FTextStream &t)
{
  if (m_value != m_defValue) writeTemplate(t,TRUE,TRUE);
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

void ConfigString::compareDoxyfile(FTextStream &t)
{
  if (m_value.stripWhiteSpace() != m_defValue.stripWhiteSpace()) writeTemplate(t,TRUE,TRUE);
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

void ConfigInt::compareDoxyfile(FTextStream &t)
{
  if (m_value != m_defValue) writeTemplate(t,TRUE,TRUE);
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

void ConfigBool::compareDoxyfile(FTextStream &t)
{
  if (m_value != m_defValue) writeTemplate(t,TRUE,TRUE);
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

static const char       *g_inputString;
static int	         g_inputPosition;
static int               g_yyLineNr;
static QCString          g_yyFileName;
static QCString          g_tmpString;
static QCString         *g_string=0;
static bool             *g_bool=0;
static StringVector     *g_list=0;
static int               g_lastState;
static QCString          g_elemStr;
static StringVector      g_includePathList;
static QStack<ConfigFileState> g_includeStack;
static int               g_includeDepth;
static bool              g_configUpdate = FALSE;
static QCString          g_encoding;
static ConfigImpl       *g_config;

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static yy_size_t yyread(char *buf,yy_size_t max_size)
{
  // no file included
  if (g_includeStack.isEmpty())
  {
    yy_size_t c=0;
    if (g_inputString==0) return c;
    while( c < max_size && g_inputString[g_inputPosition] )
    {
      *buf = g_inputString[g_inputPosition++] ;
      c++; buf++;
    }
    return c;
  }
  else
  {
    //assert(g_includeStack.current()->newState==YY_CURRENT_BUFFER);
    return (yy_size_t)fread(buf,1,max_size,g_includeStack.current()->filePtr);
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
    config_term("Error: unsupported character conversion: '%s'->'%s'\n",
        inputEncoding.data(),outputEncoding.data());
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
    config_term("Error: failed to translate characters from %s to %s: %s\n",
        inputEncoding.data(),outputEncoding.data(),strerror(errno));
  }
  portable_iconv_close(cd);
  return output;
}

static void checkEncoding()
{
  ConfigString *option = (ConfigString*)g_config->get("DOXYFILE_ENCODING");
  g_encoding = *option->valueRef();
}

static FILE *tryPath(const char *path,const char *fileName)
{
  QCString absName=(path ? (QCString)path+"/"+fileName : (QCString)fileName);
  QFileInfo fi(absName);
  if (fi.exists() && fi.isFile())
  {
    FILE *f=Portable::fopen(absName,"r");
    if (!f) config_err("could not open file %s for reading\n",absName.data());
    return f;
  }
  return 0;
}

static void substEnvVarsInStrList(StringVector &sl);
static void substEnvVarsInString(QCString &s);

static FILE *findFile(const char *fileName)
{
  if (fileName==0)
  {
    return 0;
  }
  if (Portable::isAbsolutePath(fileName))
  {
    return tryPath(NULL, fileName);
  }
  substEnvVarsInStrList(g_includePathList);
  for (const auto &s : g_includePathList)
  {
    FILE *f = tryPath(s.c_str(),fileName);
    if (f) return f;
  }
  // try cwd if g_includePathList fails
  return tryPath(".",fileName);
}

static void readIncludeFile(const char *incName)
{
  if (g_includeDepth==MAX_INCLUDE_DEPTH) {
    config_term("maximum include depth (%d) reached, %s is not included. Aborting...\n",
	MAX_INCLUDE_DEPTH,incName);
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
    for (i=0;i<g_includeStack.count();i++) msg("  ");
    msg("@INCLUDE = %s: parsing...\n",inc.data());
#endif

    // store the state of the old file
    ConfigFileState *fs=new ConfigFileState;
    fs->oldState=YY_CURRENT_BUFFER;
    fs->lineNr=g_yyLineNr;
    fs->fileName=g_yyFileName;
    fs->filePtr=f;
    // push the state on the stack
    g_includeStack.push(fs);
    // set the scanner to the include file
    yy_switch_to_buffer(yy_create_buffer(f, YY_BUF_SIZE));
    fs->newState=YY_CURRENT_BUFFER;
    g_yyFileName=inc;
    g_includeDepth++;
  }
  else
  {
    config_term("@INCLUDE = %s: not found!\n",inc.data());
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
%x      GetStrList1
%x      GetQuotedString
%x      GetEnvVar
%x      Include

%%

<*>\0x0d
<PreStart>"##".*"\n" { g_config->appendStartComment(yytext);g_yyLineNr++;}
<PreStart>. {
              BEGIN(Start);
              unput(*yytext);
            }
<Start,GetString,GetStrList,GetStrList1,GetBool,SkipInvalid>"##".*"\n" { g_config->appendUserComment(yytext);g_yyLineNr++;}
<Start,GetString,GetStrList,GetStrList1,GetBool,SkipInvalid>"#"	   { BEGIN(SkipComment); }
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"="	 { QCString cmd=yytext;
                                           cmd=cmd.left(cmd.length()-1).stripWhiteSpace();
					   ConfigOption *option = g_config->get(cmd);
					   if (option==0) // oops not known
					   {
					     config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
						 cmd.data(),g_yyLineNr,g_yyFileName.data());
					     BEGIN(SkipInvalid);
					   }
					   else // known tag
					   {
                                             option->setUserComment(g_config->takeUserComment());
					     option->setEncoding(g_encoding);
					     switch(option->kind())
					     {
					       case ConfigOption::O_Info:
						 // shouldn't get here!
					         BEGIN(SkipInvalid);
						 break;
					       case ConfigOption::O_List:
						 g_list = ((ConfigList *)option)->valueRef();
					         g_list->clear();
						 g_elemStr="";
						 if (cmd == "PREDEFINED")
						 {
					           BEGIN(GetStrList1);
						 }
						 else
						 {
					           BEGIN(GetStrList);
						 }
					         break;
					       case ConfigOption::O_Enum:
						 g_string = ((ConfigEnum *)option)->valueRef();
					         g_string->resize(0);
					         BEGIN(GetString);
					         break;
					       case ConfigOption::O_String:
						 g_string = ((ConfigString *)option)->valueRef();
					         g_string->resize(0);
					         BEGIN(GetString);
					         break;
					       case ConfigOption::O_Int:
						 g_string = ((ConfigInt *)option)->valueStringRef();
					         g_string->resize(0);
					         BEGIN(GetString);
					         break;
					       case ConfigOption::O_Bool:
						 g_string = ((ConfigBool *)option)->valueStringRef();
					         g_string->resize(0);
					         BEGIN(GetString);
						 break;
					       case ConfigOption::O_Obsolete:
                                                 if (g_configUpdate)
                                                 {
					           config_warn("Tag '%s' at line %d of file '%s' has become obsolete.\n"
						              "         This tag has been removed.\n", cmd.data(),g_yyLineNr,g_yyFileName.data());
                                                 }
                                                 else
                                                 {
					           config_warn("Tag '%s' at line %d of file '%s' has become obsolete.\n"
						              "         To avoid this warning please remove this line from your configuration "
							      "file or upgrade it using \"doxygen -u\"\n", cmd.data(),g_yyLineNr,g_yyFileName.data());
                                                 }
					         BEGIN(SkipInvalid);
						 break;
					       case ConfigOption::O_Disabled:
                                                 if (g_configUpdate)
                                                 {
					           config_warn("Tag '%s' at line %d of file '%s' belongs to an option that was not enabled at compile time.\n"
						              "         This tag has been removed.\n", cmd.data(),g_yyLineNr,g_yyFileName.data());
                                                 }
                                                 else
                                                 {
					           config_warn("Tag '%s' at line %d of file '%s' belongs to an option that was not enabled at compile time.\n"
						              "         To avoid this warning please remove this line from your configuration "
							    "file or upgrade it using \"doxygen -u\", or recompile doxygen with this feature enabled.\n", cmd.data(),g_yyLineNr,g_yyFileName.data());
                                                 }
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"+="	{ QCString cmd=yytext;
                                          cmd=cmd.left(cmd.length()-2).stripWhiteSpace();
					  ConfigOption *option = g_config->get(cmd);
					  if (option==0) // oops not known
					  {
					    config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
						cmd.data(),g_yyLineNr,g_yyFileName.data());
					    BEGIN(SkipInvalid);
					  }
					  else // known tag
					  {
                                            option->setUserComment(g_config->takeUserComment());
					    switch(option->kind())
					    {
					      case ConfigOption::O_Info:
					        // shouldn't get here!
					        BEGIN(SkipInvalid);
						break;
					      case ConfigOption::O_List:
					        g_list = ((ConfigList *)option)->valueRef();
						g_elemStr="";
						if (cmd == "PREDEFINED")
						{
					          BEGIN(GetStrList1);
						}
						else
						{
					          BEGIN(GetStrList);
						}
					        break;
					      case ConfigOption::O_Enum:
					      case ConfigOption::O_String:
					      case ConfigOption::O_Int:
					      case ConfigOption::O_Bool:
					        config_warn("operator += not supported for '%s'. Ignoring line at line %d, file %s\n",
						    yytext,g_yyLineNr,g_yyFileName.data());
					        BEGIN(SkipInvalid);
						break;
					       case ConfigOption::O_Obsolete:
					         config_warn("Tag '%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", cmd.data(),g_yyLineNr,g_yyFileName.data());
					         BEGIN(SkipInvalid);
						 break;
					       case ConfigOption::O_Disabled:
					         config_warn("Tag '%s' at line %d of file %s belongs to an option that was not enabled at compile time.\n"
						            "To avoid this warning please remove this line from your configuration "
							    "file, upgrade it using \"doxygen -u\", or recompile doxygen with this feature enabled.\n", cmd.data(),g_yyLineNr,g_yyFileName.data());
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>"@INCLUDE_PATH"[ \t]*"=" 	{ BEGIN(GetStrList); g_list=&g_includePathList; g_list->clear(); g_elemStr=""; }
  /* include a g_config file */
<Start>"@INCLUDE"[ \t]*"="     		{ BEGIN(Include);}
<Include>([^ \"\t\r\n]+)|("\""[^\n\"]+"\"") {
  					  readIncludeFile(configStringRecode(yytext,g_encoding,"UTF-8"));
  					  BEGIN(Start);
					}
<<EOF>>					{
                                          //printf("End of include file\n");
					  //printf("Include stack depth=%d\n",g_includeStack.count());
                                          if (g_includeStack.isEmpty())
					  {
					    //printf("Terminating scanner!\n");
					    yyterminate();
					  }
					  else
					  {
					    ConfigFileState *fs=g_includeStack.pop();
					    fclose(fs->filePtr);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->oldState );
					    yy_delete_buffer( oldBuf );
					    g_yyLineNr=fs->lineNr;
					    g_yyFileName=fs->fileName;
					    delete fs; fs=0;
                                            g_includeDepth--;
					  }
  					}

<Start>[a-z_A-Z0-9]+			{ config_warn("ignoring unknown tag '%s' at line %d, file %s\n",yytext,g_yyLineNr,g_yyFileName.data()); }
<GetString,GetBool,SkipInvalid>\n	{ g_yyLineNr++; BEGIN(Start); }
<GetStrList,GetStrList1>\n				{
  					  g_yyLineNr++;
					  if (!g_elemStr.isEmpty())
					  {
					    //printf("elemStr1='%s'\n",g_elemStr.data());
					    g_list->push_back(g_elemStr.data());
					  }
					  BEGIN(Start);
					}
<GetStrList1>[ \t]+			{
					  if (!g_elemStr.isEmpty())
					  {
					    //printf("elemStr2='%s'\n",g_elemStr.data());
					    g_list->push_back(g_elemStr.data());
					  }
					  g_elemStr.resize(0);
					}
<GetStrList>[ \t,]+			{
  				          if (!g_elemStr.isEmpty())
					  {
					    //printf("elemStr2='%s'\n",g_elemStr.data());
  					    g_list->push_back(g_elemStr.data());
					  }
					  g_elemStr.resize(0);
  					}
<GetString>[^ \"\t\r\n]+		{ (*g_string)+=configStringRecode(yytext,g_encoding,"UTF-8");
                                          checkEncoding();
                                        }
<GetString,GetStrList,GetStrList1,SkipInvalid>"\""	{ g_lastState=YY_START;
  					  BEGIN(GetQuotedString);
                                          g_tmpString.resize(0);
					}
<GetQuotedString>"\""|"\n" 		{
                                          // we add a bogus space to signal that the string was quoted. This space will be stripped later on.
                                          g_tmpString+=" ";
  					  //printf("Quoted String = '%s'\n",g_tmpString.data());
  					  if (g_lastState==GetString)
					  {
					    (*g_string)+=configStringRecode(g_tmpString,g_encoding,"UTF-8");
                                            checkEncoding();
					  }
					  else
					  {
					    g_elemStr+=configStringRecode(g_tmpString,g_encoding,"UTF-8");
					  }
					  if (*yytext=='\n')
					  {
					    config_warn("Missing end quote (\") on line %d, file %s\n",g_yyLineNr,g_yyFileName.data());
					    g_yyLineNr++;
					  }
					  BEGIN(g_lastState);
  					}
<GetQuotedString>"\\\""			{
  					  g_tmpString+='"';
  					}
<GetQuotedString>.			{ g_tmpString+=*yytext; }
<GetBool>[a-zA-Z]+			{
  					  QCString bs=yytext;
  					  bs=bs.upper();
  					  if (bs=="YES" || bs=="1")
					    *g_bool=TRUE;
					  else if (bs=="NO" || bs=="0")
					    *g_bool=FALSE;
					  else
					  {
					    *g_bool=FALSE;
					    config_warn("Invalid value '%s' for "
						 "boolean tag in line %d, file %s; use YES or NO\n",
						 bs.data(),g_yyLineNr,g_yyFileName.data());
					  }
					}
<GetStrList1>[^ \#\"\t\r\n]+		{
					  g_elemStr+=configStringRecode(yytext,g_encoding,"UTF-8");
					}
<GetStrList>[^ \#\"\t\r\n,]+		{
  					  g_elemStr+=configStringRecode(yytext,g_encoding,"UTF-8");
  					}
<SkipComment>\n				{ g_yyLineNr++; BEGIN(Start); }
<SkipComment>\\[ \r\t]*\n		{ g_yyLineNr++; BEGIN(Start); }
<*>\\[ \r\t]*\n				{ g_yyLineNr++; }
<*>.
<*>\n					{ g_yyLineNr++ ; }

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
  t << "# Doxyfile " << getDoxygenVersion() << endl << endl;
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

void ConfigImpl::compareDoxyfile(FTextStream &t)
{
  t << "# Difference with default Doxyfile " << getFullVersion();
  t << endl;
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (;(option=it.current());++it)
  {
    option->m_userComment = "";
    option->compareDoxyfile(t);
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
void ConfigImpl::emptyValueToDefault()
{
  QListIterator<ConfigOption> it = iterator();
  ConfigOption *option;
  for (;(option=it.current());++it)
  {
    option->emptyValueToDefault();
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
    //printf("Found environment var s.mid(%d,%d)='%s'\n",i+2,l-3,s.mid(i+2,l-3).data());
    QCString env=Portable::getenv(s.mid(i+2,l-3));
    substEnvVarsInString(env); // recursively expand variables if needed.
    s = s.left(i)+env+s.right(s.length()-i-l);
    p=i+env.length(); // next time start at the end of the expanded string
  }
  s=s.stripWhiteSpace(); // to strip the bogus space that was added when an argument
                         // has quotes
  //printf("substEnvVarInString(%s) end\n",s.data());
}

static void substEnvVarsInStrList(StringVector &sl)
{
  StringVector results;
  for (const auto &s : sl)
  {
    QCString result = s.c_str();
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
                results.push_back(result.mid(p,i-p).data());
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
            results.push_back(result.mid(p,i-p).data());
	    p=i+1;
	  }
	}
      }
      if (p!=l) // add the leftover as a string
      {
        results.push_back(result.right(l-p).data());
      }
    }
    else // just goto the next element in the list
    {
      results.push_back(result.data());
    }
  }
  sl = results;
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
        config_term("Config option '%s' has invalid depends relation on unknown option '%s'\n",
            option->name().data(),depName.data());
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
      config_err("file '%s' not found\n",name);
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
    config_term("cannot open file '%s' for reading\n",name);
  }
  return "";
}

bool ConfigImpl::parseString(const char *fn,const char *str,bool update)
{
  g_config = ConfigImpl::instance();
  g_inputString   = str;
  g_inputPosition = 0;
  g_yyFileName    = fn;
  g_yyLineNr      = 1;
  g_includeStack.setAutoDelete(TRUE);
  g_includeStack.clear();
  g_includeDepth  = 0;
  configimplYYrestart( configimplYYin );
  BEGIN( PreStart );
  g_configUpdate = update;
  configimplYYlex();
  g_configUpdate = FALSE;
  g_inputString = 0;
  return TRUE;
}

bool ConfigImpl::parse(const char *fn,bool update)
{
  int retval;
  g_encoding = "UTF-8";
  printlex(yy_flex_debug, TRUE, __FILE__, fn);
  retval =  parseString(fn,configFileToString(fn), update);
  printlex(yy_flex_debug, FALSE, __FILE__, fn);
  return retval;
}

//----------------------------------------------------------------------

static void cleanUpPaths(StringVector &str)
{
  for (size_t i=0;i<str.size();i++)
  {
    std::string path = str[i];
    std::replace(path.begin(),path.end(),'\\','/');
    if ((path[0]!='/' && (path.size()<=2 || path[1]!=':')) || path[path.size()-1]!='/')
    {
      QFileInfo fi(path.c_str());
      if (fi.exists() && fi.isDir())
      {
        path = fi.absFilePath().utf8().data();
        if (path[path.size()-1]!='/') path+='/';
      }
    }
    str[i]=path;
  }
}

static bool checkFileName(const QCString &s,const char *optionName)
{
  QCString val = s.stripWhiteSpace().lower();
  if ((val=="yes" || val=="true"  || val=="1" || val=="all") ||
      (val=="no"  || val=="false" || val=="0" || val=="none"))
  {
    err("file name expected for option %s, got %s instead. Ignoring...\n",optionName,s.data());
    return false;
  }
  return true;
}

#include "config.h"

void Config::init()
{
  ConfigImpl::instance()->init();
}

static void checkList(const StringVector &list,const char *name, bool equalRequired,bool valueRequired)
{
  for (const auto &s: list)
  {
    QCString item=s.c_str();
    item=item.stripWhiteSpace();
    int i=item.find('=');
    if (i==-1 && equalRequired)
    {
      err("Illegal format for option %s, no equal sign ('=') specified for item '%s'\n",name,item.data());
    }
    if (i!=-1)
    {
      QCString myName=item.left(i).stripWhiteSpace();
      if (myName.isEmpty())
      {
        err("Illegal format for option %s, no name specified for item '%s'\n",name,item.data());
      }
      else if (valueRequired)
      {
        QCString myValue=item.right(item.length()-i-1).stripWhiteSpace();
        if (myValue.isEmpty())
        {
          err("Illegal format for option %s, no value specified for item '%s'\n",name,item.data());
        }
      }
    }
  }
}

void Config::checkAndCorrect()
{
  ConfigValues::instance().init();

  //------------------------
  // check WARN_FORMAT
  QCString warnFormat = Config_getString(WARN_FORMAT);
  if (warnFormat.stripWhiteSpace().isEmpty())
  {
    Config_updateString(WARN_FORMAT,"$file:$line $text");
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

  //------------------------
  // set default man page extension if non is given by the user
  QCString manExtension = Config_getString(MAN_EXTENSION);
  if (manExtension.isEmpty())
  {
    Config_updateString(MAN_EXTENSION,".3");
  }

  //------------------------
  // check and correct PAPER_TYPE
  QCString paperType = Config_getEnum(PAPER_TYPE);
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
  Config_updateEnum(PAPER_TYPE,paperType);

  //------------------------
  // check & correct OUTPUT_LANGUAGE
  QCString outputLanguage=Config_getEnum(OUTPUT_LANGUAGE);
  outputLanguage=outputLanguage.stripWhiteSpace();
  if (outputLanguage.isEmpty())
  {
    outputLanguage = "English";
  }
  Config_updateEnum(OUTPUT_LANGUAGE,outputLanguage);

  //------------------------
  // check & correct HTML_FILE_EXTENSION
  QCString htmlFileExtension=Config_getString(HTML_FILE_EXTENSION);
  htmlFileExtension=htmlFileExtension.stripWhiteSpace();
  if (htmlFileExtension.isEmpty())
  {
    htmlFileExtension = ".html";
  }
  Config_updateString(HTML_FILE_EXTENSION,htmlFileExtension);

  //------------------------
  // check & correct STRIP_FROM_PATH
  StringVector stripFromPath = Config_getList(STRIP_FROM_PATH);
  if (stripFromPath.empty()) // by default use the current path
  {
    QString p = QDir::currentDirPath();
    if (p.at(p.length()-1)!='/')
        p.append('/');
    stripFromPath.push_back(p.utf8().data());
  }
  else
  {
    cleanUpPaths(stripFromPath);
  }
  Config_updateList(STRIP_FROM_PATH,stripFromPath);

  //------------------------
  // check & correct STRIP_FROM_INC_PATH
  StringVector stripFromIncPath = Config_getList(STRIP_FROM_INC_PATH);
  cleanUpPaths(stripFromIncPath);
  Config_updateList(STRIP_FROM_INC_PATH,stripFromIncPath);

  //------------------------
  // Test to see if HTML header is valid
  QCString headerFile = Config_getString(HTML_HEADER);
  if (!headerFile.isEmpty())
  {
    QFileInfo fi(headerFile);
    if (!fi.exists())
    {
      config_term("tag HTML_HEADER: header file '%s' "
	  "does not exist\n",headerFile.data());
    }
  }

  //------------------------
  // Test to see if HTML footer is valid
  QCString footerFile = Config_getString(HTML_FOOTER);
  if (!footerFile.isEmpty())
  {
    QFileInfo fi(footerFile);
    if (!fi.exists())
    {
      config_term("tag HTML_FOOTER: footer file '%s' "
	  "does not exist\n",footerFile.data());
    }
  }

  //------------------------
  // Test to see if MathJax code file is valid
  if (Config_getBool(USE_MATHJAX))
  {
    QCString mathJaxCodefile = Config_getString(MATHJAX_CODEFILE);
    if (!mathJaxCodefile.isEmpty())
    {
      QFileInfo fi(mathJaxCodefile);
      if (!fi.exists())
      {
        config_term("tag MATHJAX_CODEFILE file '%s' "
	    "does not exist\n",mathJaxCodefile.data());
      }
    }
    QCString path = Config_getString(MATHJAX_RELPATH);
    if (!path.isEmpty() && path.at(path.length()-1)!='/')
    {
      path+="/";
    }
    Config_updateString(MATHJAX_RELPATH,path);
  }

  //------------------------
  // Test to see if LaTeX header is valid
  QCString latexHeaderFile = Config_getString(LATEX_HEADER);
  if (!latexHeaderFile.isEmpty())
  {
    QFileInfo fi(latexHeaderFile);
    if (!fi.exists())
    {
      config_term("tag LATEX_HEADER: header file '%s' "
	  "does not exist\n",latexHeaderFile.data());
    }
  }

  //------------------------
  // Test to see if LaTeX footer is valid
  QCString latexFooterFile = Config_getString(LATEX_FOOTER);
  if (!latexFooterFile.isEmpty())
  {
    QFileInfo fi(latexFooterFile);
    if (!fi.exists())
    {
      config_term("tag LATEX_FOOTER: footer file '%s' "
	  "does not exist\n",latexFooterFile.data());
    }
  }

  //------------------------
  // check include path
  const StringVector &includePath = Config_getList(INCLUDE_PATH);
  for (const auto &s : includePath)
  {
    QFileInfo fi(s.c_str());
    if (!fi.exists()) warn_uncond("tag INCLUDE_PATH: include path '%s' "
	                  "does not exist\n",s.c_str());
  }

  //------------------------
  // check PREDEFINED
  if (Config_getBool(ENABLE_PREPROCESSING))
  {
    const StringVector &predefList = Config_getList(PREDEFINED);
    for (const auto &s : predefList)
    {
      QCString predef=s.c_str();
      predef=predef.stripWhiteSpace();
      int i_equals=predef.find('=');
      int i_obrace=predef.find('(');
      if ((i_obrace==0) || (i_equals==0) || (i_equals==1 && predef.at(i_equals-1)==':'))
      {
        err("Illegal PREDEFINED format '%s', no define name specified\n",predef.data());
      }
    }
  }

  //------------------------
  // check ALIASES
  const StringVector &aliasList = Config_getList(ALIASES);
  for (const auto &s : aliasList)
  {
    QRegExp re1("[a-z_A-Z][a-z_A-Z0-9]*[ \t]*=");         // alias without argument
    QRegExp re2("[a-z_A-Z][a-z_A-Z0-9]*{[0-9]+}[ \t]*="); // alias with argument
    QCString alias=s.c_str();
    alias=alias.stripWhiteSpace();
    if (alias.find(re1)!=0 && alias.find(re2)!=0)
    {
      err("Illegal ALIASES format '%s'. Use \"name=value\" or \"name{n}=value\", where n is the number of arguments\n",
	  alias.data());
    }
  }

  //------------------------
  // check EXTENSION_MAPPING
  checkList(Config_getList(EXTENSION_MAPPING),"EXTENSION_MAPPING",TRUE,TRUE);

  //------------------------
  // check FILTER_PATTERNS
  checkList(Config_getList(FILTER_PATTERNS),"FILTER_PATTERNS",TRUE,TRUE);

  //------------------------
  // check FILTER_SOURCE_PATTERNS
  checkList(Config_getList(FILTER_SOURCE_PATTERNS),"FILTER_SOURCE_PATTERNS",FALSE,FALSE);

  //------------------------
  // check TAGFILES
  checkList(Config_getList(TAGFILES),"TAGFILES",FALSE,TRUE);

  //------------------------
  // check EXTRA_SEARCH_MAPPINGS
  if (Config_getBool(SEARCHENGINE) && Config_getBool(GENERATE_HTML))
  {
    checkList(Config_getList(EXTRA_SEARCH_MAPPINGS),"EXTRA_SEARCH_MAPPING",TRUE,TRUE);
  }

  //------------------------
  // check if GENERATE_TREEVIEW and GENERATE_HTMLHELP are both enabled
  if (Config_getBool(GENERATE_TREEVIEW) && Config_getBool(GENERATE_HTMLHELP))
  {
    err("When enabling GENERATE_HTMLHELP the tree view (GENERATE_TREEVIEW) should be disabled. I'll do it for you.\n");
    Config_updateBool(GENERATE_TREEVIEW,FALSE);
  }

  //------------------------
  // check if SEARCHENGINE and GENERATE_HTMLHELP are both enabled
  if (Config_getBool(SEARCHENGINE) && Config_getBool(GENERATE_HTMLHELP))
  {
    err("When enabling GENERATE_HTMLHELP the search engine (SEARCHENGINE) should be disabled. I'll do it for you.\n");
    Config_updateBool(SEARCHENGINE,FALSE);
  }

  //------------------------
  // check if SEPARATE_MEMBER_PAGES and INLINE_GROUPED_CLASSES are both enabled
  if (Config_getBool(SEPARATE_MEMBER_PAGES) && Config_getBool(INLINE_GROUPED_CLASSES))
  {
    err("When enabling INLINE_GROUPED_CLASSES the SEPARATE_MEMBER_PAGES option should be disabled. I'll do it for you.\n");
    Config_updateBool(SEPARATE_MEMBER_PAGES,FALSE);
  }

  //------------------------
  // check and correct DOT_IMAGE_FORMAT
  QCString dotImageFormat=Config_getEnum(DOT_IMAGE_FORMAT);
  dotImageFormat=dotImageFormat.stripWhiteSpace();
  if (dotImageFormat.isEmpty())
  {
    dotImageFormat = "png";
  }
  Config_updateEnum(DOT_IMAGE_FORMAT,dotImageFormat);

  //------------------------
  // correct DOT_FONTNAME if needed
  QCString dotFontName=Config_getString(DOT_FONTNAME);
  if (dotFontName=="FreeSans" || dotFontName=="FreeSans.ttf")
  {
    warn_uncond("doxygen no longer ships with the FreeSans font.\n"
               "You may want to clear or change DOT_FONTNAME.\n"
               "Otherwise you run the risk that the wrong font is being used for dot generated graphs.\n");
  }
  else if (dotFontName.isEmpty())
  {
    dotFontName = "Helvetica";
  }
  Config_updateString(DOT_FONTNAME,dotFontName);

  //------------------------
  // clip dotFontSize against the maximum bounds
  int dotFontSize = Config_getInt(DOT_FONTSIZE);
  if (dotFontSize<4)
  {
    dotFontSize=4;
  }
  else if (dotFontSize>24)
  {
    dotFontSize=24;
  }
  Config_updateInt(DOT_FONTSIZE,dotFontSize);

  //------------------------
  // clip number of threads
  int dotNumThreads = Config_getInt(DOT_NUM_THREADS);
  if (dotNumThreads>32)
  {
    dotNumThreads=32;
  }
  else if (dotNumThreads<=0)
  {
    dotNumThreads=QMAX(2,std::thread::hardware_concurrency()+1);
  }
  Config_updateInt(DOT_NUM_THREADS,dotNumThreads);

  //------------------------
  // check dot path
  QCString dotPath = Config_getString(DOT_PATH);
  if (!dotPath.isEmpty())
  {
    QFileInfo fi(dotPath);
    if (fi.exists() && fi.isFile()) // user specified path + exec
    {
      dotPath=fi.dirPath(TRUE).utf8()+"/";
    }
    else
    {
      QFileInfo dp(dotPath+"/dot"+Portable::commandExtension());
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
  Config_updateString(DOT_PATH,dotPath);

  //------------------------
  // check plantuml path
  QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  if (!plantumlJarPath.isEmpty())
  {
    QFileInfo pu(plantumlJarPath);
    if (pu.exists() && pu.isDir()) // PLANTUML_JAR_PATH is directory
    {
      QFileInfo jar(plantumlJarPath+Portable::pathSeparator()+"plantuml.jar");
      if (jar.exists() && jar.isFile())
      {
        plantumlJarPath = jar.dirPath(TRUE).utf8()+Portable::pathSeparator();
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
      plantumlJarPath = pu.dirPath(TRUE).utf8()+Portable::pathSeparator();
    }
    else
    {
      err("path specified via PLANTUML_JAR_PATH does not exist or not a directory: %s\n",
                 plantumlJarPath.data());
      plantumlJarPath="";
    }
  }
  Config_updateString(PLANTUML_JAR_PATH,plantumlJarPath);

  //------------------------
  // check dia path
  QCString diaPath = Config_getString(DIA_PATH);
  if (!diaPath.isEmpty())
  {
    QFileInfo dp(diaPath+"/dia"+Portable::commandExtension());
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
  Config_updateString(DIA_PATH,diaPath);

  //------------------------
  // check INPUT
  StringVector inputSources=Config_getList(INPUT);
  if (inputSources.empty())
  {
    // use current dir as the default
    inputSources.push_back(QDir::currentDirPath().utf8().data());
  }
  else
  {
    for (const auto &s : inputSources)
    {
      QFileInfo fi(s.c_str());
      if (!fi.exists())
      {
	warn_uncond("tag INPUT: input source '%s' does not exist\n",s.c_str());
      }
    }
  }
  Config_updateList(INPUT,inputSources);

  //------------------------
  // add default file patterns if needed
  StringVector filePatternList = Config_getList(FILE_PATTERNS);
  if (filePatternList.empty())
  {
    ConfigOption * opt = ConfigImpl::instance()->get("FILE_PATTERNS");
    if (opt->kind()==ConfigOption::O_List)
    {
      filePatternList = ((ConfigList*)opt)->getDefault();
    }
  }
  Config_updateList(FILE_PATTERNS,filePatternList);

  //------------------------
  // add default pattern if needed
  StringVector examplePatternList = Config_getList(EXAMPLE_PATTERNS);
  if (examplePatternList.empty())
  {
    examplePatternList.push_back("*");
    Config_updateList(EXAMPLE_PATTERNS,examplePatternList);
  }

  //------------------------
  // if no output format is enabled, warn the user
  if (!Config_getBool(GENERATE_HTML)    &&
      !Config_getBool(GENERATE_LATEX)   &&
      !Config_getBool(GENERATE_MAN)     &&
      !Config_getBool(GENERATE_RTF)     &&
      !Config_getBool(GENERATE_XML)     &&
      !Config_getBool(GENERATE_PERLMOD) &&
      !Config_getBool(GENERATE_RTF)     &&
      !Config_getBool(GENERATE_DOCBOOK) &&
      !Config_getBool(GENERATE_AUTOGEN_DEF) &&
      Config_getString(GENERATE_TAGFILE).isEmpty()
     )
  {
    warn_uncond("No output formats selected! Set at least one of the main GENERATE_* options to YES.\n");
  }

  //------------------------
  // check HTMLHELP creation requirements
  if (!Config_getBool(GENERATE_HTML) &&
      Config_getBool(GENERATE_HTMLHELP))
  {
    warn_uncond("GENERATE_HTMLHELP=YES requires GENERATE_HTML=YES.\n");
  }

  //------------------------
  // check QHP creation requirements
  if (Config_getBool(GENERATE_QHP))
  {
    if (Config_getString(QHP_NAMESPACE).isEmpty())
    {
      err("GENERATE_QHP=YES requires QHP_NAMESPACE to be set. Using 'org.doxygen.doc' as default!.\n");
      Config_updateString(QHP_NAMESPACE,"org.doxygen.doc");
    }

    if (Config_getString(QHP_VIRTUAL_FOLDER).isEmpty())
    {
      err("GENERATE_QHP=YES requires QHP_VIRTUAL_FOLDER to be set. Using 'doc' as default!\n");
      Config_updateString(QHP_VIRTUAL_FOLDER,"doc");
    }
  }

  //------------------------
  if (Config_getBool(OPTIMIZE_OUTPUT_JAVA) && Config_getBool(INLINE_INFO))
  {
    // don't show inline info for Java output, since Java has no inline
    // concept.
    Config_updateBool(INLINE_INFO,FALSE);
  }

  //------------------------
  int depth = Config_getInt(MAX_DOT_GRAPH_DEPTH);
  if (depth==0)
  {
    Config_updateInt(MAX_DOT_GRAPH_DEPTH,1000);
  }

  //------------------------
  int hue = Config_getInt(HTML_COLORSTYLE_HUE);
  if (hue<0)
  {
    hue=0;
  }
  else if (hue>=360)
  {
    hue=hue%360;
  }
  Config_updateInt(HTML_COLORSTYLE_HUE,hue);

  //------------------------
  int sat = Config_getInt(HTML_COLORSTYLE_SAT);
  if (sat<0)
  {
    sat=0;
  }
  else if (sat>255)
  {
    sat=255;
  }
  Config_updateInt(HTML_COLORSTYLE_SAT,sat);


  //------------------------
  int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  if (gamma<40)
  {
    gamma=40;
  }
  else if (gamma>240)
  {
    gamma=240;
  }
  Config_updateInt(HTML_COLORSTYLE_GAMMA,gamma);

  //------------------------
  QCString mathJaxFormat = Config_getEnum(MATHJAX_FORMAT);
  if (!mathJaxFormat.isEmpty() && mathJaxFormat!="HTML-CSS" &&
       mathJaxFormat!="NativeMML" && mathJaxFormat!="SVG")
  {
    err("Unsupported value for MATHJAX_FORMAT: Should be one of HTML-CSS, NativeMML, or SVG\n");
    Config_updateEnum(MATHJAX_FORMAT,"HTML-CSS");
  }

  //------------------------
  // add default words if needed
  const StringVector &annotationFromBrief = Config_getList(ABBREVIATE_BRIEF);
  if (annotationFromBrief.empty())
  {
    Config_updateList(ABBREVIATE_BRIEF,
                      { "The $name class", "The $name widget",
                        "The $name file", "is", "provides", "specifies",
                        "contains", "represents", "a", "an", "the"
                      });
  }

  //------------------------
  // some default settings for vhdl
  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL) &&
      (Config_getBool(INLINE_INHERITED_MEMB) ||
       Config_getBool(INHERIT_DOCS) ||
       !Config_getBool(HIDE_SCOPE_NAMES) ||
       !Config_getBool(EXTRACT_PRIVATE) ||
       !Config_getBool(EXTRACT_PACKAGE)
      )
     )
  {
    bool b1 = Config_getBool(INLINE_INHERITED_MEMB);
    bool b2 = Config_getBool(INHERIT_DOCS);
    bool b3 = Config_getBool(HIDE_SCOPE_NAMES);
    bool b4 = Config_getBool(EXTRACT_PRIVATE);
    bool b5 = Config_getBool(SKIP_FUNCTION_MACROS);
    bool b6 = Config_getBool(EXTRACT_PACKAGE);
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

    Config_updateBool(INLINE_INHERITED_MEMB, FALSE);
    Config_updateBool(INHERIT_DOCS,          FALSE);
    Config_updateBool(HIDE_SCOPE_NAMES,      TRUE);
    Config_updateBool(EXTRACT_PRIVATE,       TRUE);
    Config_updateBool(ENABLE_PREPROCESSING,  FALSE);
    Config_updateBool(EXTRACT_PACKAGE,       TRUE);
  }

  if (!checkFileName(Config_getString(GENERATE_TAGFILE),"GENERATE_TAGFILE"))
  {
    Config_updateString(GENERATE_TAGFILE,"");
  }

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


}

void Config::writeTemplate(FTextStream &t,bool shortList,bool update)
{
  ConfigImpl::instance()->writeTemplate(t,shortList,update);
}

void Config::compareDoxyfile(FTextStream &t)
{
  postProcess(FALSE, TRUE);
  ConfigImpl::instance()->compareDoxyfile(t);
}

bool Config::parse(const char *fileName,bool update)
{
  return ConfigImpl::instance()->parse(fileName,update);
}

void Config::postProcess(bool clearHeaderAndFooter, bool compare)
{
  ConfigImpl::instance()->substituteEnvironmentVars();
  if (!compare)ConfigImpl::instance()->emptyValueToDefault();
  ConfigImpl::instance()->convertStrToVal();

  // avoid bootstrapping issues when the g_config file already
  // refers to the files that we are supposed to parse.
  if (clearHeaderAndFooter)
  {
    Config_updateString(HTML_HEADER ,"");
    Config_updateString(HTML_FOOTER ,"");
    Config_updateString(LATEX_HEADER,"");
    Config_updateString(LATEX_FOOTER,"");
  }
}

void Config::deinit()
{
  ConfigImpl::instance()->deleteInstance();
}

#if USE_STATE2STRING
#include "configimpl.l.h"
#endif
