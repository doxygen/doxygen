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
 *      includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <thread>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "regex.h"
#include "configimpl.h"
#include "version.h"
#include "portable.h"
#include "message.h"
#include "lang_cfg.h"
#include "language.h"
#include "configoptions.h"
#include "fileinfo.h"
#include "dir.h"
#include "textstream.h"

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
  vfprintf(stderr, qPrint(QCString(error_str) + fmt), args);
  va_end(args);
}
void config_term(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, qPrint(QCString(error_str) + fmt), args);
  va_end(args);
  fprintf(stderr, "%s\n", "Exiting...");
  exit(1);
}

void config_warn(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, qPrint(QCString(warning_str) + fmt), args);
  va_end(args);
}

static QCString configStringRecode(
    const QCString &str,
    const QCString &fromEncoding,
    const QCString &toEncoding);

#define MAX_INCLUDE_DEPTH 10
#define YY_NEVER_INTERACTIVE 1

/* -----------------------------------------------------------------
 */
static QCString convertToComment(const QCString &s, const QCString &u)
{
  //printf("convertToComment(%s)=%s\n",qPrint(s),qPrint(u));
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

void ConfigOption::writeBoolValue(TextStream &t,bool v,bool initSpace)
{
  if (initSpace) t << " ";
  if (v) t << "YES"; else t << "NO";
}

void ConfigOption::writeIntValue(TextStream &t,int i,bool initSpace)
{
  if (initSpace) t << " ";
  t << i;
}

void ConfigOption::writeStringValue(TextStream &t,const QCString &s,bool initSpace)
{
  char c;
  bool needsEscaping=FALSE;
  // convert the string back to it original g_encoding
  QCString se = configStringRecode(s,"UTF-8",m_encoding);
  if (se.isEmpty()) return;
  const char *p=se.data();
  if (p)
  {
    if (initSpace) t << " ";
    while ((c=*p++)!=0 && !needsEscaping)
      needsEscaping = (c==' ' || c== ',' || c=='\n' || c=='\t' || c=='"' || c=='#');
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

void ConfigOption::writeStringList(TextStream &t,const StringVector &l)
{
  bool first=TRUE;
  for (const auto &p : l)
  {
    if (!first) t << " \\\n";
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
                "Using the default: %d!\n",qPrint(m_valueString),qPrint(m_name),m_minVal,m_maxVal,m_value);
    }
    else
    {
      m_value=val;
    }
  }
}

static bool convertStringToBool(const QCString &str,bool &isValid)
{
  isValid=false;
  QCString val = str.stripWhiteSpace().lower();
  if (!val.isEmpty())
  {
    if (val=="yes" || val=="true" || val=="1" || val=="all")
    {
      isValid=true;
      return true;
    }
    else if (val=="no" || val=="false" || val=="0" || val=="none")
    {
      isValid=true;
      return false;
    }
  }
  return false;
}

void ConfigBool::convertStrToVal()
{
  if (!m_valueString.stripWhiteSpace().isEmpty())
  {
    bool isValid=false;
    bool b = convertStringToBool(m_valueString,isValid);
    if (isValid)
    {
      m_value=b;
    }
    else
    {
      config_warn("argument '%s' for option %s is not a valid boolean value\n"
          "Using the default: %s!\n",qPrint(m_valueString),qPrint(m_name),m_value?"YES":"NO");
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
  for (const auto &s : m_valueRange)
  {
    if (s.lower() == val)
    {
       m_value = s;
       return;
    }
  }

  config_warn("argument '%s' for option %s is not a valid enum value\n"
              "Using the default: %s!\n",qPrint(m_value),qPrint(m_name),qPrint(m_defValue));
  m_value = m_defValue;
}

QCString &ConfigImpl::getString(const char *fileName,int num,const char *name) const
{
  auto it = m_dict.find(name);
  if (it==m_dict.end())
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (it->second->kind()!=ConfigOption::O_String)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of string type!\n",fileName,num,name);
  }
  return *((ConfigString *)it->second)->valueRef();
}

StringVector &ConfigImpl::getList(const char *fileName,int num,const char *name) const
{
  auto it = m_dict.find(name);
  if (it==m_dict.end())
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (it->second->kind()!=ConfigOption::O_List)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of list type!\n",fileName,num,name);
  }
  return *((ConfigList *)it->second)->valueRef();
}

QCString &ConfigImpl::getEnum(const char *fileName,int num,const char *name) const
{
  auto it = m_dict.find(name);
  if (it==m_dict.end())
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (it->second->kind()!=ConfigOption::O_Enum)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of enum type!\n",fileName,num,name);
  }
  return *((ConfigEnum *)it->second)->valueRef();
}

int &ConfigImpl::getInt(const char *fileName,int num,const char *name) const
{
  auto it = m_dict.find(name);
  if (it==m_dict.end())
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (it->second->kind()!=ConfigOption::O_Int)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of integer type!\n",fileName,num,name);
  }
  return *((ConfigInt *)it->second)->valueRef();
}

bool &ConfigImpl::getBool(const char *fileName,int num,const char *name) const
{
  auto it = m_dict.find(name);
  if (it==m_dict.end())
  {
    config_term("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
  }
  else if (it->second->kind()!=ConfigOption::O_Bool)
  {
    config_term("%s<%d>: Internal error: Requested option %s not of boolean type!\n",fileName,num,name);
  }
  return *((ConfigBool *)it->second)->valueRef();
}

/* ------------------------------------------ */

void ConfigInfo::writeTemplate(TextStream &t, bool sl,bool)
{
  if (!sl)
  {
    t << "\n";
  }
  t << "#---------------------------------------------------------------------------\n";
  t << "# " << m_doc << "\n";
  t << "#---------------------------------------------------------------------------\n";
}

void ConfigList::writeTemplate(TextStream &t,bool sl,bool)
{
  if (!sl)
  {
    t << "\n";
    t << convertToComment(m_doc, m_userComment);
    t << "\n";
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  writeStringList(t,m_value);
  t << "\n";
}

bool ConfigList::isDefault()
{
  auto get_stripped = [](std::string s)             { return QCString(s.c_str()).stripWhiteSpace(); };
  auto is_not_empty = [get_stripped](std::string s) { return !get_stripped(s).isEmpty();            };
  size_t defCnt = std::count_if(       m_value.begin(),       m_value.end(),is_not_empty);
  size_t valCnt = std::count_if(m_defaultValue.begin(),m_defaultValue.end(),is_not_empty);
  if ( valCnt != defCnt)
  {
    return false;
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
        return false;
      }
      ++it1;
      ++it2;
    }
  }
  return true;
}

void ConfigList::compareDoxyfile(TextStream &t)
{
  if (!isDefault()) writeTemplate(t,TRUE,TRUE);
}

void ConfigList::writeXMLDoxyfile(TextStream &t)
{
  t << "  <option  id='" << m_name << "'";
  t << " default='" << (isDefault() ? "yes" : "no") << "'";
  t << " type='stringlist'";
  t << ">";
  t << "\n";
  for (const auto &p : m_value)
  {
    QCString s=p.c_str();
    t << "    <value>";
    t << "<![CDATA[";
    writeStringValue(t,s,false);
    t << "]]>";
    t << "</value>\n";
  }
  t << "  </option>\n";
}

void ConfigEnum::writeTemplate(TextStream &t,bool sl,bool)
{
  if (!sl)
  {
    t << "\n";
    t << convertToComment(m_doc, m_userComment);
    t << "\n";
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  writeStringValue(t,m_value);
  t << "\n";
}

void ConfigEnum::compareDoxyfile(TextStream &t)
{
  if (!isDefault()) writeTemplate(t,TRUE,TRUE);
}

void ConfigEnum::writeXMLDoxyfile(TextStream &t)
{
  t << "  <option  id='" << m_name << "'";
  t << " default='" << (isDefault() ? "yes" : "no") << "'";
  t << " type='string'";
  t << ">";
  t << "<value>";
  writeStringValue(t,m_value,false);
  t << "</value>";
  t << "</option>\n";
}

void ConfigString::writeTemplate(TextStream &t,bool sl,bool)
{
  if (!sl)
  {
    t << "\n";
    t << convertToComment(m_doc, m_userComment);
    t << "\n";
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "=";
  writeStringValue(t,m_value);
  t << "\n";
}

void ConfigString::compareDoxyfile(TextStream &t)
{
  if (!isDefault()) writeTemplate(t,TRUE,TRUE);
}

void ConfigString::writeXMLDoxyfile(TextStream &t)
{
  t << "  <option  id='" << m_name << "'";
  t << " default='" << (isDefault() ? "yes" : "no") << "'";
  t << " type='string'";
  t << ">";
  t << "<value>";
  t << "<![CDATA[";
  writeStringValue(t,m_value,false);
  t << "]]>";
  t << "</value>";
  t << "</option>\n";
}

void ConfigInt::writeTemplate(TextStream &t,bool sl,bool upd)
{
  if (!sl)
  {
    t << "\n";
    t << convertToComment(m_doc, m_userComment);
    t << "\n";
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

void ConfigInt::compareDoxyfile(TextStream &t)
{
  if (!isDefault()) writeTemplate(t,TRUE,TRUE);
}

void ConfigInt::writeXMLDoxyfile(TextStream &t)
{
  t << "  <option  id='" << m_name << "'";
  t << " default='" << (isDefault() ? "yes" : "no") << "'";
  t << " type='int'";
  t << ">";
  t << "<value>";
  writeIntValue(t,m_value,false);
  t << "</value>";
  t << "</option>\n";
}

void ConfigBool::writeTemplate(TextStream &t,bool sl,bool upd)
{
  if (!sl)
  {
    t << "\n";
    t << convertToComment(m_doc, m_userComment);
    t << "\n";
  }
  else if (!m_userComment.isEmpty())
  {
    t << convertToComment("", m_userComment);
  }
  QCString spaces = m_spaces.left(MAX_OPTION_LENGTH-m_name.length());
  t << m_name << spaces << "=";
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

void ConfigBool::compareDoxyfile(TextStream &t)
{
  if (!isDefault()) writeTemplate(t,TRUE,TRUE);
}

void ConfigBool::writeXMLDoxyfile(TextStream &t)
{
  t << "  <option  id='" << m_name << "'";
  t << " default='" << (isDefault() ? "yes" : "no") << "'";
  t << " type='bool'";
  t << ">";
  t << "<value>";
  writeBoolValue(t,m_value,false);
  t << "</value>";
  t << "</option>\n";
}

void ConfigObsolete::writeTemplate(TextStream &,bool,bool) {}
void ConfigDisabled::writeTemplate(TextStream &,bool,bool) {}

/* -----------------------------------------------------------------
 *
 *      static variables
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
static int               g_inputPosition;
static int               g_yyLineNr;
static QCString          g_yyFileName;
static QCString          g_cmd;
static QCString         *g_string=0;
static StringVector     *g_list=0;
static QCString          g_listStr;
static StringVector      g_includePathList;
static std::vector< std::unique_ptr<ConfigFileState> > g_includeStack;
static bool              g_configUpdate = FALSE;
static QCString          g_encoding;
static ConfigImpl       *g_config;

/* -----------------------------------------------------------------
 */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static yy_size_t yyread(char *buf,yy_size_t max_size)
{
  // no file included
  if (g_includeStack.empty())
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
    return (yy_size_t)fread(buf,1,max_size,g_includeStack.back()->filePtr);
  }
}


static QCString configStringRecode(
    const QCString &str,
    const QCString &inputEncoding,
    const QCString &outputEncoding)
{
  if (inputEncoding.isEmpty() || outputEncoding.isEmpty() || inputEncoding==outputEncoding) return str;
  int inputSize=str.length();
  int outputSize=inputSize*4+1;
  QCString output(outputSize);
  void *cd = portable_iconv_open(outputEncoding.data(),inputEncoding.data());
  if (cd==(void *)(-1))
  {
    config_term("Error: unsupported character conversion: '%s'->'%s'\n",
        qPrint(inputEncoding),qPrint(outputEncoding));
  }
  size_t iLeft=(size_t)inputSize;
  size_t oLeft=(size_t)outputSize;
  const char *inputPtr  = str.data();
  char *outputPtr = output.rawData();
  if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
  {
    outputSize-=(int)oLeft;
    output.resize(outputSize+1);
    output.at(outputSize)='\0';
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,qPrint(srcBuf));
  }
  else
  {
    config_term("Error: failed to translate characters from %s to %s: %s\n",
        qPrint(inputEncoding),qPrint(outputEncoding),strerror(errno));
  }
  portable_iconv_close(cd);
  return output;
}

static void checkEncoding()
{
  ConfigString *option = (ConfigString*)g_config->get("DOXYFILE_ENCODING");
  g_encoding = *option->valueRef();
}

static QCString stripComment(const QCString &s)
{
  // check if there is a comment at the end of the string
  bool insideQuote=false;
  int l = s.length();
  for (int i=0;i<l;i++)
  {
    char c = s.at(i);
    if (c=='\\') // skip over escaped characters
    {
      i++;
    }
    else if (c=='"') // toggle inside/outside quotation
    {
      insideQuote=!insideQuote;
    }
    else if (!insideQuote && c=='#') // found start of a comment
    {
      if (i<l-1 && s.at(i+1)=='#') // ## -> user comment
      {
        g_config->appendUserComment(s.mid(i)+"\n");
      }
      return s.left(i).stripWhiteSpace();
    }
  }
  return s;
}

static void processString()
{
  // strip leading and trailing whitespace
  QCString s = stripComment(g_string->stripWhiteSpace());
  int l = s.length();

  // remove surrounding quotes if present (and not escaped)
  if (l>=2 && s.at(0)=='"' && s.at(l-1)=='"' && // remove quotes
      (s.at(l-2)!='\\' || (s.at(l-2)=='\\' && s.at(l-3)=='\\')))
  {
    s=s.mid(1,s.length()-2);
    l=s.length();
  }

  // check for invalid and/or escaped quotes
  bool warned=false;
  QCString result;
  for (int i=0;i<l;i++)
  {
    char c = s.at(i);
    if (c=='\\') // escaped character
    {
      if (i<l-1 && s.at(i+1)=='"') // unescape the quote character
      {
        result+='"';
      }
      else // keep other escaped characters in escaped form
      {
        result+=c;
        if (i<l-1)
        {
          result+=s.at(i+1);
        }
      }
      i++; // skip over the escaped character
    }
    else if (c=='"') // unescaped quote
    {
      if (!warned)
      {
        config_warn("Invalid value for '%s' tag at line %d, file %s: Value '%s' is not properly quoted\n",
                    qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName),qPrint(g_string->stripWhiteSpace()));
      }
      warned=true;
    }
    else // normal character
    {
      result+=c;
    }
  }

  // recode the string
  *g_string=configStringRecode(result,g_encoding,"UTF-8");

  // update encoding
  checkEncoding();

  //printf("Processed string '%s'\n",qPrint(g_string));
}

static void processList()
{
  bool allowCommaAsSeparator = g_cmd!="PREDEFINED";

  QCString s = stripComment(g_listStr.stripWhiteSpace());
  int l = s.length();

  QCString elemStr;

  // helper to push elemStr to the list and clear it
  auto addElem = [&elemStr]()
  {
    if (!elemStr.isEmpty())
    {
      QCString e = configStringRecode(elemStr,g_encoding,"UTF-8");
      //printf("Processed list element '%s'\n",qPrint(e));
      g_list->push_back(e.str());
      elemStr="";
    }
  };

  bool needsSeparator=false;
  int insideQuote=false;
  bool warned=false;
  for (int i=0;i<l;i++)
  {
    char c = s.at(i);
    if (!needsSeparator && c=='\\') // escaped character
    {
      if (i<l-1 && s.at(i+1)=='"') // unescape the quote character
      {
        elemStr+='"';
      }
      else // keep other escaped characters in escaped form
      {
        elemStr+=c;
        if (i<l-1)
        {
          elemStr+=s.at(i+1);
        }
      }
      i++; // skip over the escaped character
    }
    else if (!needsSeparator && c=='"') // quote character
    {
      if (!insideQuote)
      {
        insideQuote=true;
      }
      else // this quote ends an element
      {
        insideQuote=false;
        needsSeparator=true;
      }
    }
    else if (!insideQuote && ((c==',' && allowCommaAsSeparator) || isspace(c))) // separator
    {
      needsSeparator=false;
      addElem();
    }
    else // normal content character
    {
      if (needsSeparator)
      {
        if (!warned)
        {
          config_warn("Invalid value for '%s' tag at line %d, file %s: Values in list '%s' are not properly space %sseparated\n",
                    qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName),qPrint(g_listStr.stripWhiteSpace()),allowCommaAsSeparator?"or comma ":"");
          warned=true;
        }
        needsSeparator=false;
        i--; // try the character again as part of a new element
        addElem();
      }
      else
      {
        elemStr+=c;
      }
    }
  }
  // add last part
  addElem();
  if (insideQuote)
  {
    config_warn("Invalid value for '%s' tag at line %d, file %s: Values in list '%s' are not properly quoted\n",
                qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName),qPrint(g_listStr.stripWhiteSpace()));
  }
}

static FILE *tryPath(const QCString &path,const QCString &fileName)
{
  QCString absName=(!path.isEmpty() ? path+"/"+fileName : fileName);
  FileInfo fi(absName.str());
  if (fi.exists() && fi.isFile())
  {
    FILE *f=Portable::fopen(absName,"r");
    if (!f) config_err("could not open file %s for reading\n",qPrint(absName));
    return f;
  }
  return 0;
}

static void substEnvVarsInStrList(StringVector &sl);
static void substEnvVarsInString(QCString &s);

static FILE *findFile(const QCString &fileName)
{
  if (fileName.isEmpty())
  {
    return 0;
  }
  if (Portable::isAbsolutePath(fileName))
  {
    return tryPath(QCString(), fileName);
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

static void readIncludeFile(const QCString &incName)
{
  if (g_includeStack.size()==MAX_INCLUDE_DEPTH) {
    config_term("maximum include depth (%d) reached, %s is not included. Aborting...\n",
        MAX_INCLUDE_DEPTH,qPrint(incName));
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
    for (size_t i=0;i<g_includeStack.size();i++) msg("  ");
    msg("@INCLUDE = %s: parsing...\n",qPrint(inc));
#endif

    // store the state of the old file
    ConfigFileState *fs=new ConfigFileState;
    fs->oldState=YY_CURRENT_BUFFER;
    fs->lineNr=g_yyLineNr;
    fs->fileName=g_yyFileName;
    fs->filePtr=f;
    // push the state on the stack
    g_includeStack.push_back(std::unique_ptr<ConfigFileState>(fs));
    // set the scanner to the include file
    yy_switch_to_buffer(yy_create_buffer(f, YY_BUF_SIZE));
    fs->newState=YY_CURRENT_BUFFER;
    g_yyFileName=inc;
  }
  else
  {
    config_term("@INCLUDE = %s: not found!\n",qPrint(inc));
  }
}


%}

%option noyywrap
%option nounput

%x      Start
%x      SkipInvalid
%x      GetString
%x      GetStrList
%x      Include

%%

<*>\0x0d

   /*-------------- Comments ---------------*/

<Start>"##".*"\n"                        {
                                           g_config->appendUserComment(yytext);
                                           g_yyLineNr++;
                                         }
<Start>"#".*"\n"                         { /* normal comment */
                                           g_yyLineNr++;
                                         }

   /*-------------- TAG start ---------------*/

<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"="   { g_cmd=yytext;
                                           g_cmd=g_cmd.left(g_cmd.length()-1).stripWhiteSpace();
                                           ConfigOption *option = g_config->get(g_cmd);
                                           if (option==0) // oops not known
                                           {
                                             config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
                                                 qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
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
                                                 g_listStr="";
                                                 BEGIN(GetStrList);
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
                                                              "         This tag has been removed.\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 else
                                                 {
                                                   config_warn("Tag '%s' at line %d of file '%s' has become obsolete.\n"
                                                              "         To avoid this warning please remove this line from your configuration "
                                                              "file or upgrade it using \"doxygen -u\"\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 ((ConfigObsolete*)option)->markAsPresent();
                                                 if (((ConfigObsolete*)option)->orgType()==ConfigOption::O_List)
                                                 {
                                                   g_list = ((ConfigObsolete*)option)->valueListRef();
                                                   g_list->clear();
                                                   g_listStr="";
                                                   BEGIN(GetStrList);
                                                 }
                                                 else
                                                 {
                                                   g_string = ((ConfigObsolete*)option)->valueStringRef();
                                                   g_string->resize(0);
                                                   BEGIN(GetString);
                                                 }
                                                 break;
                                               case ConfigOption::O_Disabled:
                                                 if (g_configUpdate)
                                                 {
                                                   config_warn("Tag '%s' at line %d of file '%s' belongs to an option that was not enabled at compile time.\n"
                                                              "         This tag has been removed.\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 else
                                                 {
                                                   config_warn("Tag '%s' at line %d of file '%s' belongs to an option that was not enabled at compile time.\n"
                                                              "         To avoid this warning please remove this line from your configuration "
                                                            "file or upgrade it using \"doxygen -u\", or recompile doxygen with this feature enabled.\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 BEGIN(SkipInvalid);
                                                 break;
                                             }
                                           }
                                        }
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"+=" { g_cmd=yytext;
                                          g_cmd=g_cmd.left(g_cmd.length()-2).stripWhiteSpace();
                                          ConfigOption *option = g_config->get(g_cmd);
                                          if (option==0) // oops not known
                                          {
                                            config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
                                                qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
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
                                                g_listStr="";
                                                BEGIN(GetStrList);
                                                break;
                                              case ConfigOption::O_Enum:
                                              case ConfigOption::O_String:
                                              case ConfigOption::O_Int:
                                              case ConfigOption::O_Bool:
                                                config_warn("operator += not supported for '%s'. Ignoring line at line %d, file %s\n",
                                                    yytext,g_yyLineNr,qPrint(g_yyFileName));
                                                BEGIN(SkipInvalid);
                                                break;
                                               case ConfigOption::O_Obsolete:
                                                 config_warn("Tag '%s' at line %d of file %s has become obsolete.\n"
                                                            "To avoid this warning please update your configuration "
                                                            "file using \"doxygen -u\"\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 if (((ConfigObsolete*)option)->orgType()==ConfigOption::O_List)
                                                 {
                                                   g_list = ((ConfigObsolete*)option)->valueListRef();
                                                   g_listStr="";
                                                   BEGIN(GetStrList);
                                                 }
                                                 else
                                                 {
                                                   BEGIN(SkipInvalid);
                                                 }
                                                 break;
                                               case ConfigOption::O_Disabled:
                                                 config_warn("Tag '%s' at line %d of file %s belongs to an option that was not enabled at compile time.\n"
                                                            "To avoid this warning please remove this line from your configuration "
                                                            "file, upgrade it using \"doxygen -u\", or recompile doxygen with this feature enabled.\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 BEGIN(SkipInvalid);
                                                 break;
                                             }
                                           }
                                        }

   /*-------------- INCLUDE* ---------------*/

<Start>"@INCLUDE_PATH"[ \t]*"="         { BEGIN(GetStrList); g_list=&g_includePathList; g_list->clear(); g_listStr=""; }
  /* include a g_config file */
<Start>"@INCLUDE"[ \t]*"="              { BEGIN(Include);}
<Include>([^ \"\t\r\n]+)|("\""[^\n\"]+"\"") {
                                          readIncludeFile(configStringRecode(yytext,g_encoding,"UTF-8"));
                                          BEGIN(Start);
                                        }
<<EOF>>                                 {
                                          //printf("End of include file\n");
                                          //printf("Include stack depth=%d\n",g_includeStack.count());
                                          if (g_includeStack.empty())
                                          {
                                            //printf("Terminating scanner!\n");
                                            yyterminate();
                                          }
                                          else
                                          {
                                            auto &fs=g_includeStack.back();
                                            fclose(fs->filePtr);
                                            YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
                                            yy_switch_to_buffer( fs->oldState );
                                            yy_delete_buffer( oldBuf );
                                            g_yyLineNr=fs->lineNr;
                                            g_yyFileName=fs->fileName;
                                            g_includeStack.pop_back();
                                          }
                                        }

<Start>[a-z_A-Z0-9]+                    { config_warn("ignoring unknown tag '%s' at line %d, file %s\n",yytext,g_yyLineNr,qPrint(g_yyFileName)); }
   /*-------------- GetString ---------------*/

<GetString>\n                           { processString();
                                          g_yyLineNr++; // end of string
                                          BEGIN(Start);
                                        }
<GetString>\\[ \r\t]*\n                 { g_yyLineNr++; // line continuation
                                          *g_string+=' ';
                                        }
<GetString>"\\"                         { // escape character
                                          *g_string+=yytext;
                                        }
<GetString>[^\n\\]+                     { // string part without escape characters
                                          *g_string+=yytext;
                                        }

   /*-------------- GetStrList --------------*/

<GetStrList>\n                          { processList();
                                          g_yyLineNr++; // end of list
                                          BEGIN(Start);
                                        }
<GetStrList>\\[ \r\t]*\n                { g_yyLineNr++; // line continuation
                                          g_listStr+=' ';
                                        }
<GetStrList>"\\"                        { // escape character
                                          g_listStr+=yytext;
                                        }
<GetStrList>[^\n\\]+                    { // string part without escape characters
                                          g_listStr+=yytext;
                                        }

   /*-------------- SkipInvalid --------------*/

<SkipInvalid>\n                         { g_yyLineNr++; // end of list
                                          BEGIN(Start);
                                        }
<SkipInvalid>\\[ \r\t]*\n               { g_yyLineNr++; // line continuation
                                        }
<SkipInvalid>"\\"                       { // escape character
                                        }
<SkipInvalid>[^\n\\]+                   { // string part without escape characters
                                        }

   /*-------------- fall through -------------*/

<*>\\[ \r\t]*\n                         { g_yyLineNr++; }
<*>[ \t\r]
<*>\n                                   { g_yyLineNr++ ; }
<*>.                                    { config_warn("ignoring unknown character '%c' at line %d, file %s\n",yytext[0],g_yyLineNr,qPrint(g_yyFileName)); }

%%

/*@ ----------------------------------------------------------------------------
 */

void ConfigImpl::writeTemplate(TextStream &t,bool sl,bool upd)
{
  /* print first lines of user comment that were at the beginning of the file, might have special meaning for editors */
  if (!m_startComment.isEmpty())
  {
    t << takeStartComment() << "\n";
  }
  t << "# Doxyfile " << getDoxygenVersion() << "\n\n";
  if (!sl)
  {
    t << convertToComment(m_header,"");
  }
  for (const auto &option : m_options)
  {
    option->writeTemplate(t,sl,upd);
  }
  /* print last lines of user comment that were at the end of the file */
  if (!m_userComment.isEmpty())
  {
    t << "\n";
    t << takeUserComment();
  }
}

void ConfigImpl::compareDoxyfile(TextStream &t)
{
  t << "# Difference with default Doxyfile " << getFullVersion();
  t << "\n";
  for (const auto &option : m_options)
  {
    option->m_userComment = "";
    option->compareDoxyfile(t);
  }
}

void ConfigImpl::writeXMLDoxyfile(TextStream &t)
{
  t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n";
  t << "<doxyfile xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"doxyfile.xsd\" version=\"" << getDoxygenVersion() << "\" xml:lang=\"" <<  theTranslator->trISOLang() << "\">\n";
  for (const auto &option : m_options)
  {
    option->writeXMLDoxyfile(t);
  }
  t << "</doxyfile>\n";
}

void ConfigImpl::convertStrToVal()
{
  for (const auto &option : m_options)
  {
    option->convertStrToVal();
  }
}
void ConfigImpl::emptyValueToDefault()
{
  for (const auto &option : m_options)
  {
    option->emptyValueToDefault();
  }
}

static void substEnvVarsInString(QCString &str)
{
  if (str.isEmpty()) return;
  auto replace = [](const std::string &s, const reg::Ex &re) -> std::string
  {
    reg::Iterator it(s,re);
    reg::Iterator end;
    std::string result;
    size_t p = 0;
    for (; it!=end ; ++it)
    {
      const auto &match = *it;
      size_t i = match.position();
      size_t l = match.length();
      result+=s.substr(p,i-p);
      std::string matchContents = match[1].str();
      QCString env=Portable::getenv(matchContents.c_str()); // get content of $(..) match
      substEnvVarsInString(env); // recursively expand variables if needed.
      result+=env.str();
      p=i+l;
    }
    result+=s.substr(p);
    return result;
  };

  // match e.g. re1=$(HOME) but also re2=$(PROGRAMFILES(X86))
  static const reg::Ex re1(R"(\$\((\a[\w.-]*)\))");
  static const reg::Ex re2(R"(\$\((\a[\w.-]*\(\a[\w.-]*\))\))");
  str = QCString(replace(replace(str.str(),re1),re2)).stripWhiteSpace();
}

static void substEnvVarsInStrList(StringVector &sl)
{
  StringVector results;
  for (const auto &s : sl)
  {
    QCString result = s.c_str();
    bool wasQuoted = (result.find(' ')!=-1) || (result.find('\t')!=-1) || (result.find('"')!=-1);
    // here we strip the quote again
    substEnvVarsInString(result);

    //printf("Result %s was quoted=%d\n",qPrint(result),wasQuoted);

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
                results.push_back(result.mid(p,i-p).str());
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
            if (i>p) results.push_back(result.mid(p,i-p).str());
            p=i+1;
          }
        }
      }
      if (p!=l) // add the leftover as a string
      {
        results.push_back(result.right(l-p).str());
      }
    }
    else // just goto the next element in the list
    {
      if (!result.isEmpty()) results.push_back(result.str());
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
  for (const auto &option : m_options)
  {
    option->substEnvVars();
  }
}

void ConfigImpl::init()
{
  for (const auto &option : m_options)
  {
    option->init();
  }

  // sanity check if all depends relations are valid
  for (const auto &option : m_options)
  {
    QCString depName = option->dependsOn();
    if (!depName.isEmpty())
    {
      ConfigOption * opt = ConfigImpl::instance()->get(depName);
      if (opt==0)
      {
        config_term("Config option '%s' has invalid depends relation on unknown option '%s'\n",
            qPrint(option->name()),qPrint(depName));
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

static QCString configFileToString(const QCString &name)
{
  if (name.isEmpty()) return QCString();

  auto stream2string = [](std::istream &in) -> std::string
  {
    std::string ret;
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer))) ret.append(buffer, sizeof(buffer));
    ret.append(buffer, static_cast<uint>(in.gcount()));
    if (!ret.empty() && ret[ret.length()-1]!='\n') ret+='\n'; // to help the scanner
    return ret;
  };

  if (name=="-") // read from stdin
  {
    // read contents from stdin into contents string
    return stream2string(std::cin);
  }
  else // read from file
  {
    std::ifstream f(name.str(),std::istream::in);
    if (!f.is_open())
    {
      config_term("file '%s' not found or could not be opened\n",qPrint(name));
      return "";
    }
    return stream2string(f);
  }
}

bool ConfigImpl::parseString(const QCString &fn,const QCString &str,bool update)
{
  g_config = ConfigImpl::instance();
  g_inputString   = str.data();
  g_inputPosition = 0;
  g_yyFileName    = fn;
  g_yyLineNr      = 1;
  g_includeStack.clear();
  configimplYYrestart( configimplYYin );
  BEGIN( Start );
  g_configUpdate = update;
  configimplYYlex();
  g_configUpdate = FALSE;
  g_inputString = 0;
  return TRUE;
}

bool ConfigImpl::parse(const QCString &fn,bool update)
{
  int retval;
  g_encoding = "UTF-8";
  printlex(yy_flex_debug, TRUE, __FILE__, qPrint(fn));
  retval =  parseString(fn,configFileToString(fn), update);
  printlex(yy_flex_debug, FALSE, __FILE__, qPrint(fn));
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
      FileInfo fi(path);
      if (fi.exists() && fi.isDir())
      {
        path = fi.absFilePath();
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
    err("file name expected for option %s, got %s instead. Ignoring...\n",optionName,qPrint(s));
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
      err("Illegal format for option %s, no equal sign ('=') specified for item '%s'\n",name,qPrint(item));
    }
    if (i!=-1)
    {
      QCString myName=item.left(i).stripWhiteSpace();
      if (myName.isEmpty())
      {
        err("Illegal format for option %s, no name specified for item '%s'\n",name,qPrint(item));
      }
      else if (valueRequired)
      {
        QCString myValue=item.right(item.length()-i-1).stripWhiteSpace();
        if (myValue.isEmpty())
        {
          err("Illegal format for option %s, no value specified for item '%s'\n",name,qPrint(item));
        }
      }
    }
  }
}

void Config::checkAndCorrect(bool quiet)
{
  ConfigValues::instance().init();

  Config_updateBool(QUIET,quiet || Config_getBool(QUIET));
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
  QCString paperType = Config_getEnumAsString(PAPER_TYPE);
  paperType=paperType.lower().stripWhiteSpace();
  if (paperType.isEmpty() || paperType=="a4wide")
  {
    // use a4
    Config_updateEnum(PAPER_TYPE,PAPER_TYPE_t::a4);
  }
  else if (paperType!="a4" && paperType!="letter" &&
           paperType!="legal" && paperType!="executive")
  {
    err("Unknown page type '%s' specified\n",qPrint(paperType));
    Config_updateEnum(PAPER_TYPE,PAPER_TYPE_t::a4);
  }


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
    std::string p = Dir::currentDirPath()+"/";
    stripFromPath.push_back(p);
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
    FileInfo fi(headerFile.str());
    if (!fi.exists())
    {
      config_term("tag HTML_HEADER: header file '%s' "
          "does not exist\n",qPrint(headerFile));
    }
  }

  //------------------------
  // Test to see if HTML footer is valid
  QCString footerFile = Config_getString(HTML_FOOTER);
  if (!footerFile.isEmpty())
  {
    FileInfo fi(footerFile.str());
    if (!fi.exists())
    {
      config_term("tag HTML_FOOTER: footer file '%s' "
          "does not exist\n",qPrint(footerFile));
    }
  }

  //------------------------
  // Test to see if MathJax code file is valid
  if (Config_getBool(USE_MATHJAX))
  {
    auto mathJaxFormat = Config_getEnum(MATHJAX_FORMAT);
    auto mathjaxVersion = Config_getEnum(MATHJAX_VERSION);
    if (mathjaxVersion == MATHJAX_VERSION_t::MathJax_2)
    {
      if (mathJaxFormat==MATHJAX_FORMAT_t::chtml)
      {
        Config_updateEnum(MATHJAX_FORMAT,MATHJAX_FORMAT_t::HTML_CSS);
      }
    }
    else
    {
      if (mathJaxFormat==MATHJAX_FORMAT_t::HTML_CSS || mathJaxFormat==MATHJAX_FORMAT_t::NativeMML)
      {
        Config_updateEnum(MATHJAX_FORMAT,MATHJAX_FORMAT_t::chtml);
      }
    }

    QCString mathJaxCodefile = Config_getString(MATHJAX_CODEFILE);
    if (!mathJaxCodefile.isEmpty())
    {
      FileInfo fi(mathJaxCodefile.str());
      if (!fi.exists())
      {
        config_term("tag MATHJAX_CODEFILE file '%s' "
            "does not exist\n",qPrint(mathJaxCodefile));
      }
    }
    QCString path = Config_getString(MATHJAX_RELPATH);
    if (path.isEmpty())
    {
      path = "https://cdn.jsdelivr.net/npm/mathjax@";
      switch (mathjaxVersion)
      {
        case MATHJAX_VERSION_t::MathJax_2: path += "2"; break;
        case MATHJAX_VERSION_t::MathJax_3: path += "3"; break;
      }
    }

    if (path.at(path.length()-1)!='/')
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
    FileInfo fi(latexHeaderFile.str());
    if (!fi.exists())
    {
      config_term("tag LATEX_HEADER: header file '%s' "
          "does not exist\n",qPrint(latexHeaderFile));
    }
  }

  //------------------------
  // Test to see if LaTeX footer is valid
  QCString latexFooterFile = Config_getString(LATEX_FOOTER);
  if (!latexFooterFile.isEmpty())
  {
    FileInfo fi(latexFooterFile.str());
    if (!fi.exists())
    {
      config_term("tag LATEX_FOOTER: footer file '%s' "
          "does not exist\n",qPrint(latexFooterFile));
    }
  }

  //------------------------
  // check include path
  const StringVector &includePath = Config_getList(INCLUDE_PATH);
  for (const auto &s : includePath)
  {
    FileInfo fi(s);
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
        err("Illegal PREDEFINED format '%s', no define name specified\n",qPrint(predef));
      }
    }
  }

  //------------------------
  // check ALIASES
  const StringVector &aliasList = Config_getList(ALIASES);
  for (const auto &alias : aliasList)
  {
    // match aliases of the form re1='name=' and re2='name{2} ='
    static const reg::Ex re1(R"(^\a\w*\s*=)");
    static const reg::Ex re2(R"(^\a\w*{\d+}\s*=)");
    if (!reg::search(alias,re1) && !reg::search(alias,re2))
    {
      err("Illegal ALIASES format '%s'. Use \"name=value\" or \"name{n}=value\", where n is the number of arguments\n",
          alias.c_str());
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
  // check if GENERATE_HTMLHELP and HTML_FILE_EXTENSION is not .html
  if (Config_getString(HTML_FILE_EXTENSION)!=".html" && Config_getBool(GENERATE_HTMLHELP))
  {
    err("When enabling GENERATE_HTMLHELP the HTML_FILE_EXTENSION should be \".html\". I'll do it for you.\n");
    Config_updateString(HTML_FILE_EXTENSION,".html");
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
  // clip number of threads
  int dotNumThreads = Config_getInt(DOT_NUM_THREADS);
  if (dotNumThreads>32)
  {
    dotNumThreads=32;
  }
  else if (dotNumThreads<=0)
  {
    dotNumThreads=std::max(2u,std::thread::hardware_concurrency()+1);
  }
  Config_updateInt(DOT_NUM_THREADS,dotNumThreads);

  //------------------------
  // check dot path
  QCString dotPath = Config_getString(DOT_PATH);
  if (!dotPath.isEmpty())
  {
    FileInfo fi(dotPath.str());
    if (fi.exists() && fi.isFile()) // user specified path + exec
    {
      dotPath=fi.dirPath(TRUE)+"/";
    }
    else
    {
      QCString dotExe = dotPath+"/dot"+Portable::commandExtension();
      FileInfo dp(dotExe.str());
      if (!dp.exists() || !dp.isFile())
      {
        warn_uncond("the dot tool could not be found at %s\n",qPrint(dotPath));
        dotPath="";
      }
      else
      {
        dotPath=dp.dirPath(TRUE)+"/";
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
    FileInfo pu(plantumlJarPath.str());
    if (pu.exists() && pu.isDir()) // PLANTUML_JAR_PATH is directory
    {
      QCString plantumlJar = plantumlJarPath+Portable::pathSeparator()+"plantuml.jar";
      FileInfo jar(plantumlJar.str());
      if (jar.exists() && jar.isFile())
      {
        plantumlJarPath = jar.dirPath(TRUE)+Portable::pathSeparator();
      }
      else
      {
        err("Jar file plantuml.jar not found at location "
                   "specified via PLANTUML_JAR_PATH: '%s'\n",qPrint(plantumlJarPath));
        plantumlJarPath="";
      }
    }
    else if (pu.exists() && pu.isFile() && plantumlJarPath.right(4)==".jar") // PLANTUML_JAR_PATH is file
    {
      plantumlJarPath = pu.dirPath(TRUE)+Portable::pathSeparator();
    }
    else
    {
      err("path specified via PLANTUML_JAR_PATH does not exist or not a directory: %s\n",
                 qPrint(plantumlJarPath));
      plantumlJarPath="";
    }
  }
  Config_updateString(PLANTUML_JAR_PATH,plantumlJarPath);

  //------------------------
  // check dia path
  QCString diaPath = Config_getString(DIA_PATH);
  if (!diaPath.isEmpty())
  {
    QCString diaExe = diaPath+"/dia"+Portable::commandExtension();
    FileInfo dp(diaExe.str());
    if (!dp.exists() || !dp.isFile())
    {
      warn_uncond("dia could not be found at %s\n",qPrint(diaPath));
      diaPath="";
    }
    else
    {
      diaPath=dp.dirPath(TRUE)+"/";
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
    inputSources.push_back(Dir::currentDirPath());
  }
  else
  {
    for (const auto &s : inputSources)
    {
      FileInfo fi(s.c_str());
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
  for (const auto &option : m_options)
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
          printf("disabling option %s\n",qPrint(option->name()));
          ConfigImpl_getBool("option->name("))=FALSE; // also disable this option
        }
      }
    }
  }
#endif

}

void Config::updateObsolete()
{
  //------------------------
  // check for presence of obsolete CLASS_DIAGRAM option and correct CLASS_GRAPH if needed
  ConfigOption *classDiagramsOpt = ConfigImpl::instance()->get("CLASS_DIAGRAMS");
  ConfigOption *haveDotOpt       = ConfigImpl::instance()->get("HAVE_DOT");
  ConfigOption *classGraphOpt    = ConfigImpl::instance()->get("CLASS_GRAPH");
  if (classDiagramsOpt && classDiagramsOpt->kind()==ConfigOption::O_Obsolete &&
      haveDotOpt && classGraphOpt)
  {
    ConfigObsolete *classDiagramsOpt_ = dynamic_cast<ConfigObsolete*>(classDiagramsOpt);
    ConfigBool     *haveDotOpt_       = dynamic_cast<ConfigBool*>(haveDotOpt);
    ConfigEnum     *classGraphOpt_    = dynamic_cast<ConfigEnum*>(classGraphOpt);
    if (classDiagramsOpt_->isPresent() && classDiagramsOpt_->orgType()==ConfigOption::O_Bool)
    {
      QCString classDiagramValue = *classDiagramsOpt_->valueStringRef();
      QCString haveDotValue      = *haveDotOpt_->valueStringRef();
      QCString &classGraphValue  = *classGraphOpt_->valueRef();
      bool isValid1=true, isValid2=true;
      bool bClassDiagrams = convertStringToBool(classDiagramValue,isValid1);
      bool bHaveDot       = haveDotValue.isEmpty() ? false : convertStringToBool(haveDotValue,     isValid2);
      if (isValid1 && isValid2 && !bClassDiagrams && !bHaveDot && classGraphValue.lower()=="yes")
      {
        warn_uncond("Changing CLASS_GRAPH option to TEXT because obsolete option CLASS_DIAGRAM was found and set to NO.\n");
        classGraphValue="TEXT";
      }
    }
  }
}

void Config::writeTemplate(TextStream &t,bool shortList,bool update)
{
  ConfigImpl::instance()->writeTemplate(t,shortList,update);
}

void Config::compareDoxyfile(TextStream &t)
{
  postProcess(FALSE, TRUE);
  ConfigImpl::instance()->compareDoxyfile(t);
}

void Config::writeXMLDoxyfile(TextStream &t)
{
  ConfigImpl::instance()->writeXMLDoxyfile(t);
}

bool Config::parse(const QCString &fileName,bool update)
{
  bool parseRes = ConfigImpl::instance()->parse(fileName,update);
  if (!parseRes) return parseRes;

  // Internally we use the default format UTF-8 and
  // when updating etc. the output is in this format as well and not in the read format
  ConfigString *option = (ConfigString*)g_config->get("DOXYFILE_ENCODING");
  option->init();

  return parseRes;
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
