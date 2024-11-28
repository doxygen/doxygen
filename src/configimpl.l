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
#include <iostream>
#include <iomanip>
#include <cstdint>

#include "config.h"
#include "regex.h"
#include "configimpl.h"
#include "version.h"
#include "portable.h"
#include "message.h"
#include "language.h"
#include "configoptions.h"
#include "fileinfo.h"
#include "dir.h"
#include "textstream.h"
#include "dotattributes.h"
#include "debug.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

// For debugging
#define SHOW_INCLUDES 0

[[maybe_unused]] static const char *stateToString(int state);

static const char *warning_str = "warning: ";
static const char *error_str = "error: ";

void ConfigImpl::config_err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, qPrint(QCString(error_str) + fmt), args);
  va_end(args);
}

void ConfigImpl::config_term(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, qPrint(QCString(error_str) + fmt), args);
  va_end(args);
  fprintf(stderr, "%s\n", "Exiting...");
  exit(1);
}

void ConfigImpl::config_warn(const char *fmt, ...)
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

static bool containsEnvVar(QCString &str);

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
    char c = 0;
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

void ConfigOption::writeStringValue(TextStream &t,const QCString &s,bool initSpace, bool wasQuoted)
{
  char c = 0;
  bool needsEscaping=wasQuoted;
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
    bool wasQuoted = ((s.at(0)=='"') && (s.at(s.length()-1)=='"'));
    if (wasQuoted)
    {
      s = s.mid(1,s.length()-2);
    }
    writeStringValue(t,s,true,wasQuoted);
    first=FALSE;
  }
}

/* -----------------------------------------------------------------
 */

std::unique_ptr<ConfigImpl> ConfigImpl::m_instance;

void ConfigInt::convertStrToVal(Config::CompareMode compareMode)
{
  if (!m_valueString.isEmpty())
  {
    if (compareMode == Config::CompareMode::CompressedNoEnv)
    {
       if (containsEnvVar(m_valueString)) return;
    }
    bool ok = false;
    int val = m_valueString.toInt(&ok);
    if (!ok || val<m_minVal || val>m_maxVal)
    {
      ConfigImpl::config_warn("argument '%s' for option %s is not a valid number in the range [%d..%d]!\n"
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

void ConfigBool::convertStrToVal(Config::CompareMode compareMode)
{
  if (!m_valueString.stripWhiteSpace().isEmpty())
  {
    if (compareMode == Config::CompareMode::CompressedNoEnv)
    {
       if (containsEnvVar(m_valueString)) return;
    }
    bool isValid=false;
    bool b = convertStringToBool(m_valueString,isValid);
    if (isValid)
    {
      m_value=b;
    }
    else
    {
      ConfigImpl::config_warn("argument '%s' for option %s is not a valid boolean value\n"
          "Using the default: %s!\n",qPrint(m_valueString),qPrint(m_name),m_value?"YES":"NO");
    }
  }
}

void ConfigEnum::convertStrToVal(Config::CompareMode compareMode)
{
  if (m_value.isEmpty())
  {
    m_value = m_defValue;
    return;
  }
  if (compareMode == Config::CompareMode::CompressedNoEnv)
  {
     if (containsEnvVar(m_value)) return;
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

  ConfigImpl::config_warn("argument '%s' for option %s is not a valid enum value\n"
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
  return *(dynamic_cast<ConfigString *>(it->second))->valueRef();
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
  return *(dynamic_cast<ConfigList *>(it->second))->valueRef();
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
  return *(dynamic_cast<ConfigEnum *>(it->second))->valueRef();
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
  return *(dynamic_cast<ConfigInt *>(it->second))->valueRef();
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
  return *(dynamic_cast<ConfigBool *>(it->second))->valueRef();
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
  auto get_stripped = [](const std::string &s)             { return QCString(s.c_str()).stripWhiteSpace(); };
  auto is_not_empty = [get_stripped](const std::string &s) { return !get_stripped(s).isEmpty();            };
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

void ConfigList::compareDoxyfile(TextStream &t, Config::CompareMode)
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

void ConfigList::writeXSDDoxyfile(TextStream &t)
{
  t << "      <xsd:enumeration value=\"" << m_name << "\"/>\n";
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

void ConfigEnum::compareDoxyfile(TextStream &t, Config::CompareMode)
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

void ConfigEnum::writeXSDDoxyfile(TextStream &t)
{
  t << "      <xsd:enumeration value=\"" << m_name << "\"/>\n";
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

void ConfigString::compareDoxyfile(TextStream &t, Config::CompareMode)
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

void ConfigString::writeXSDDoxyfile(TextStream &t)
{
  t << "      <xsd:enumeration value=\"" << m_name << "\"/>\n";
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

void ConfigInt::compareDoxyfile(TextStream &t,Config::CompareMode compareMode)
{
  if (!isDefault() || ((compareMode == Config::CompareMode::CompressedNoEnv) && containsEnvVar(m_valueString))) writeTemplate(t,TRUE,TRUE);
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

void ConfigInt::writeXSDDoxyfile(TextStream &t)
{
  t << "      <xsd:enumeration value=\"" << m_name << "\"/>\n";
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

void ConfigBool::compareDoxyfile(TextStream &t,Config::CompareMode compareMode)
{
  if (!isDefault() || ((compareMode == Config::CompareMode::CompressedNoEnv) && containsEnvVar(m_valueString))) writeTemplate(t,TRUE,TRUE);
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

void ConfigBool::writeXSDDoxyfile(TextStream &t)
{
  t << "      <xsd:enumeration value=\"" << m_name << "\"/>\n";
}

void ConfigObsolete::writeTemplate(TextStream &,bool,bool) {}

void ConfigDisabled::writeTemplate(TextStream &,bool,bool) {}
void ConfigDisabled::writeXSDDoxyfile(TextStream &t)
{
  t << "      <xsd:enumeration value=\"" << m_name << "\"/>\n";
}

/* -----------------------------------------------------------------
 *
 *      static variables
 */

struct ConfigFileState
{
  int lineNr = 1;
  FILE *filePtr = nullptr;
  YY_BUFFER_STATE oldState;
  YY_BUFFER_STATE newState;
  QCString fileName;
};

static const char       *g_inputString = nullptr;
static int               g_inputPosition = 0;
static int               g_yyLineNr = 1;
static QCString          g_yyFileName;
static QCString          g_cmd;
static QCString         *g_string = nullptr;
static StringVector     *g_list = nullptr;
static QCString          g_listStr;
static StringVector      g_includePathList;
static std::vector< std::unique_ptr<ConfigFileState> > g_includeStack;
static bool              g_configUpdate = FALSE;
static QCString          g_encoding;
static ConfigImpl       *g_config = nullptr;
static Config::CompareMode g_compareMode = Config::CompareMode::Full;
static QCString          g_localStoreRepl;

#define unput_string(yytext,yyleng) do { for (int i=(int)yyleng-1;i>=0;i--) unput(yytext[i]); } while(0)
/* -----------------------------------------------------------------
 */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

// otherwise the filename would be the name of the converted file (*.cpp instead of *.l)
static inline const char *getLexerFILE() {return __FILE__;}
#define LEX_NO_REENTRANT
#include "doxygen_lex.h"

static int yyread(char *buf,int max_size)
{
  // no file included
  if (g_includeStack.empty())
  {
    int c=0;
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
    return static_cast<int>(fread(buf,1,max_size,g_includeStack.back()->filePtr));
  }
}


static QCString configStringRecode(
    const QCString &str,
    const QCString &inputEncoding,
    const QCString &outputEncoding)
{
  if (inputEncoding.isEmpty() || outputEncoding.isEmpty() || inputEncoding==outputEncoding) return str;
  size_t inputSize=str.length();
  size_t outputSize=inputSize*4;
  QCString output(outputSize, QCString::ExplicitSize);
  void *cd = portable_iconv_open(outputEncoding.data(),inputEncoding.data());
  if (cd==reinterpret_cast<void *>(-1))
  {
    ConfigImpl::config_term("Error: unsupported character conversion: '%s'->'%s'\n"
                "Check the 'DOXYFILE_ENCODING' setting in the config file!\n",
        qPrint(inputEncoding),qPrint(outputEncoding));
  }
  size_t iLeft=inputSize;
  size_t oLeft=outputSize;
  const char *inputPtr  = str.data();
  char *outputPtr = output.rawData();
  if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
  {
    outputSize-=oLeft;
    output.resize(outputSize);
    output.at(outputSize)='\0';
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,qPrint(srcBuf));
  }
  else
  {
    ConfigImpl::config_term("Error: failed to translate characters from %s to %s: %s\n",
        qPrint(inputEncoding),qPrint(outputEncoding),strerror(errno));
  }
  portable_iconv_close(cd);
  return output;
}

static void checkEncoding()
{
  ConfigString *option = dynamic_cast<ConfigString*>(g_config->get("DOXYFILE_ENCODING"));
  g_encoding = *option->valueRef();
}

static QCString stripComment(const QCString &s)
{
  // check if there is a comment at the end of the string
  bool insideQuote=false;
  size_t l = s.length();
  for (size_t i=0;i<l;i++)
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

static void processStoreRepl(QCString &storeReplStr)
{
  // strip leading and trailing whitespace
  QCString s = stripComment(storeReplStr.stripWhiteSpace());
  // recode the string
  storeReplStr=configStringRecode(s,g_encoding,"UTF-8");
}

static void processString()
{
  // strip leading and trailing whitespace
  QCString s = stripComment(g_string->stripWhiteSpace());
  size_t l = s.length();

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
  for (size_t i=0;i<l;i++)
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
        ConfigImpl::config_warn("Invalid value for '%s' tag at line %d, file %s: Value '%s' is not properly quoted\n",
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
  size_t l = s.length();

  QCString elemStr;
  bool wasQuote=false;

  // helper to push elemStr to the list and clear it
  auto addElem = [&elemStr,&wasQuote]()
  {
    if (!elemStr.isEmpty())
    {
      QCString e = configStringRecode(elemStr,g_encoding,"UTF-8");
      //printf("Processed list element '%s'\n",qPrint(e));
      if (wasQuote) e = "\""+e+"\"";
      wasQuote = false;
      g_list->push_back(e.str());
      elemStr="";
    }
  };

  bool needsSeparator=false;
  bool insideQuote=false;
  bool warned=false;
  for (size_t i=0;i<l;i++)
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
        wasQuote=true;
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
          ConfigImpl::config_warn("Invalid value for '%s' tag at line %d, file %s: Values in list '%s' are not properly space %sseparated\n",
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
    ConfigImpl::config_warn("Invalid value for '%s' tag at line %d, file %s: Values in list '%s' are not properly quoted\n",
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
    if (!f) ConfigImpl::config_err("could not open file %s for reading\n",qPrint(absName));
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
    ConfigImpl::config_term("maximum include depth (%d) reached, %s is not included. Aborting...\n",
        MAX_INCLUDE_DEPTH,qPrint(incName));
  }

  QCString inc = incName;
  substEnvVarsInString(inc);
  inc = inc.stripWhiteSpace();
  size_t incLen = inc.length();
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
    ConfigImpl::config_term("@INCLUDE = %s: not found!\n",qPrint(inc));
  }
}


%}

%option noyywrap

REGEX_a [a-z_A-Z\x80-\xFF]
REGEX_w [a-z_A-Z0-9\x80-\xFF]

%x      Start
%x      SkipInvalid
%x      GetString
%x      GetStrList
%x      Include
%x      StoreRepl

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
                                             ConfigImpl::config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
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
                                                 g_list = dynamic_cast<ConfigList *>(option)->valueRef();
                                                 g_list->clear();
                                                 g_listStr="";
                                                 BEGIN(GetStrList);
                                                 break;
                                               case ConfigOption::O_Enum:
                                                 g_string = dynamic_cast<ConfigEnum *>(option)->valueRef();
                                                 g_string->clear();
                                                 BEGIN(GetString);
                                                 break;
                                               case ConfigOption::O_String:
                                                 g_string = dynamic_cast<ConfigString *>(option)->valueRef();
                                                 g_string->clear();
                                                 BEGIN(GetString);
                                                 break;
                                               case ConfigOption::O_Int:
                                                 g_string = dynamic_cast<ConfigInt *>(option)->valueStringRef();
                                                 g_string->clear();
                                                 BEGIN(GetString);
                                                 break;
                                               case ConfigOption::O_Bool:
                                                 g_string = dynamic_cast<ConfigBool *>(option)->valueStringRef();
                                                 g_string->clear();
                                                 BEGIN(GetString);
                                                 break;
                                               case ConfigOption::O_Obsolete:
                                                 if (g_configUpdate)
                                                 {
                                                   ConfigImpl::config_warn("Tag '%s' at line %d of file '%s' has become obsolete.\n"
                                                              "         This tag has been removed.\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 else
                                                 {
                                                   ConfigImpl::config_warn("Tag '%s' at line %d of file '%s' has become obsolete.\n"
                                                              "         To avoid this warning please remove this line from your configuration "
                                                              "file or upgrade it using \"doxygen -u\"\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 dynamic_cast<ConfigObsolete*>(option)->markAsPresent();
                                                 if (dynamic_cast<ConfigObsolete*>(option)->orgType()==ConfigOption::O_List)
                                                 {
                                                   g_list = dynamic_cast<ConfigObsolete*>(option)->valueListRef();
                                                   g_list->clear();
                                                   g_listStr="";
                                                   BEGIN(GetStrList);
                                                 }
                                                 else
                                                 {
                                                   g_string = dynamic_cast<ConfigObsolete*>(option)->valueStringRef();
                                                   g_string->clear();
                                                   BEGIN(GetString);
                                                 }
                                                 break;
                                               case ConfigOption::O_Disabled:
                                                 if (g_configUpdate)
                                                 {
                                                   ConfigImpl::config_warn("Tag '%s' at line %d of file '%s' belongs to an option that was not enabled at compile time.\n"
                                                              "         This tag has been removed.\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 }
                                                 else
                                                 {
                                                   ConfigImpl::config_warn("Tag '%s' at line %d of file '%s' belongs to an option that was not enabled at compile time.\n"
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
                                            ConfigImpl::config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
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
                                                g_list = dynamic_cast<ConfigList *>(option)->valueRef();
                                                g_listStr="";
                                                BEGIN(GetStrList);
                                                break;
                                              case ConfigOption::O_Enum:
                                              case ConfigOption::O_String:
                                              case ConfigOption::O_Int:
                                              case ConfigOption::O_Bool:
                                                ConfigImpl::config_warn("operator += not supported for '%s'. Ignoring line at line %d, file %s\n",
                                                    yytext,g_yyLineNr,qPrint(g_yyFileName));
                                                BEGIN(SkipInvalid);
                                                break;
                                               case ConfigOption::O_Obsolete:
                                                ConfigImpl::config_warn("Tag '%s' at line %d of file %s has become obsolete.\n"
                                                            "To avoid this warning please update your configuration "
                                                            "file using \"doxygen -u\"\n", qPrint(g_cmd),g_yyLineNr,qPrint(g_yyFileName));
                                                 if (dynamic_cast<ConfigObsolete*>(option)->orgType()==ConfigOption::O_List)
                                                 {
                                                   g_list = dynamic_cast<ConfigObsolete*>(option)->valueListRef();
                                                   g_listStr="";
                                                   BEGIN(GetStrList);
                                                 }
                                                 else
                                                 {
                                                   BEGIN(SkipInvalid);
                                                 }
                                                 break;
                                               case ConfigOption::O_Disabled:
                                                 ConfigImpl::config_warn("Tag '%s' at line %d of file %s belongs to an option that was not enabled at compile time.\n"
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
<Start>"$("{REGEX_a}({REGEX_w}|[.-])*")" | // e.g. $(HOME)
<Start>"$("{REGEX_a}({REGEX_w}|[.-])*"("{REGEX_a}({REGEX_w}|[.-])*"))" { // e.g. $(PROGRAMFILES(X86))
                                          g_localStoreRepl = yytext;
                                          if (g_compareMode == Config::CompareMode::CompressedNoEnv) // the x_noenv
                                          {
                                            BEGIN(StoreRepl);
                                          }
                                          else
                                          {
                                            substEnvVarsInString(g_localStoreRepl);
                                            unput_string(g_localStoreRepl.data(),g_localStoreRepl.length());
                                          }
                                        }
<Start>"@"{REGEX_a}{REGEX_w}*"@"        {
                                          if (g_compareMode == Config::CompareMode::CompressedNoEnv) // the x_noenv
                                          {
                                            g_localStoreRepl = yytext;
                                            BEGIN(StoreRepl);
                                          }
                                          else
                                          {
                                            ConfigImpl::config_warn("ignoring unknown '%s' at line %d, file %s\n",
                                                        yytext,g_yyLineNr,qPrint(g_yyFileName));
                                          }
                                        }
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

<Start>[a-z_A-Z0-9]+                    { ConfigImpl::config_warn("ignoring unknown tag '%s' at line %d, file %s\n",yytext,g_yyLineNr,qPrint(g_yyFileName)); }
   /*-------------- GetString ---------------*/

<StoreRepl>\n                           {
                                          g_localStoreRepl += yytext;
                                          processStoreRepl(g_localStoreRepl);
                                          g_config->appendStoreRepl(g_localStoreRepl + "\n");
                                          g_localStoreRepl.clear();
                                          g_yyLineNr++; // end of string
                                          BEGIN(Start);
                                        }
<StoreRepl>\\[ \r\t]*\n                 { g_yyLineNr++; // line continuation
                                          g_localStoreRepl += yytext;
                                        }
<StoreRepl>"\\"                         { // escape character
                                          g_localStoreRepl += yytext;
                                        }
<StoreRepl>[^\n\\]+                     { // string part without escape characters
                                          g_localStoreRepl += yytext;
                                        }
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
<*>.                                    { ConfigImpl::config_warn("ignoring unknown character '%c' at line %d, file %s\n",yytext[0],g_yyLineNr,qPrint(g_yyFileName)); }

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

void ConfigImpl::compareDoxyfile(TextStream &t,Config::CompareMode compareMode)
{
  t << "# Difference with default Doxyfile " << getFullVersion();
  t << "\n";
  for (const auto &option : m_options)
  {
    option->m_userComment = "";
    option->compareDoxyfile(t,compareMode);
  }
  if (!m_storeRepl.isEmpty())
  {
    t << "\n";
    t << takeStoreRepl() << "\n";
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

void ConfigImpl::writeXSDDoxyfile(TextStream &t)
{
  for (const auto &option : m_options)
  {
    option->writeXSDDoxyfile(t);
  }
  for (const auto &option : m_disabled)
  {
    option->writeXSDDoxyfile(t);
  }
}

void ConfigImpl::convertStrToVal(Config::CompareMode compareMode)
{
  for (const auto &option : m_options)
  {
    option->convertStrToVal(compareMode);
  }
}
void ConfigImpl::emptyValueToDefault()
{
  for (const auto &option : m_options)
  {
    option->emptyValueToDefault();
  }
}

static const reg::Ex reEnvVar(R"(\$\((\a[\w.-]*)\))");                 // e.g. $(HOME)
static const reg::Ex reEnvVarExt(R"(\$\((\a[\w.-]*\(\a[\w.-]*\))\))"); // e.g. $(PROGRAMFILES(X86))
static const reg::Ex reEnvVarCMake(R"(@\a\w*@)");                      // CMake type replacement

static bool containsEnvVar(QCString &str)
{
  reg::Match m;
  std::string s = str.str();
  return reg::search(s,m,reEnvVar) || reg::search(s,m,reEnvVarExt) || reg::search(s,m,reEnvVarCMake);
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

  str = QCString(replace(replace(str.str(),reEnvVar),reEnvVarExt)).stripWhiteSpace();
}

static void substEnvVarsInStrList(StringVector &sl)
{
  StringVector results;
  for (const auto &s : sl)
  {
    QCString result = s.c_str();
    bool wasQuoted = ((result.at(0)=='"') && (result.at(result.length()-1)=='"'));
    if (wasQuoted)
    {
      result = result.mid(1,result.length()-2);
    }
    else
    {
      wasQuoted = (result.find(' ')!=-1) || (result.find('\t')!=-1) || (result.find('"')!=-1);
    }
    // here we strip the quote again
    substEnvVarsInString(result);

    //printf("Result %s was quoted=%d\n",qPrint(result),wasQuoted);

    if (!wasQuoted) /* as a result of the expansion, a single string
                       may have expanded into a list, which we'll
                       add to sl. If the original string already
                       contained multiple elements no further
                       splitting is done to allow quoted items with spaces! */
    {
      int l = static_cast<int>(result.length());
      int p = 0;
      // skip spaces
      // search for a "word"
      for (int i=0;i<l;i++)
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

ConfigImpl::ConfigImpl()
{
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
    ret.append(buffer, static_cast<uint32_t>(in.gcount()));
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
    std::ifstream f = Portable::openInputStream(name);
    if (!f.is_open())
    {
      ConfigImpl::config_term("file '%s' not found or could not be opened\n",qPrint(name));
      return "";
    }
    return stream2string(f);
  }
}

bool ConfigImpl::parseString(const QCString &fn,const QCString &str,bool update)
{
#ifdef FLEX_DEBUG
  configimplYYset_debug(Debug::isFlagSet(Debug::Lex_configimpl)?1:0);
#endif
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
  g_encoding = "UTF-8";
  DebugLex debugLex(Debug::Lex_configimpl, __FILE__, qPrint(fn));
  bool retval =  parseString(fn,configFileToString(fn), update);
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

static void adjustBoolSetting(const char *depOption, const char *optionName,bool expectedValue)
{
  // lookup option by name
  const ConfigValues::Info *option = ConfigValues::instance().get(optionName);
  if (option && option->type==ConfigValues::Info::Bool) // safety check
  {
    if (ConfigValues::instance().*(option->value.b)!=expectedValue) // current value differs from expectation
    {
      err("When enabling %s the %s option should be %s. I'll adjust it for you.\n",depOption,optionName,expectedValue? "enabled" : "disabled");
      ConfigValues::instance().*(option->value.b)=expectedValue; // adjust option
    }
  }
}

static void adjustStringSetting(const char *depOption, const char *optionName,const QCString &expectedValue)
{
  // lookup option by name
  const ConfigValues::Info *option = ConfigValues::instance().get(optionName);
  if (option && option->type==ConfigValues::Info::String) // safety check
  {
    if (ConfigValues::instance().*(option->value.s)!=expectedValue) // current value differs from expectation
    {
      err("When enabling %s the %s option should have value '%s'. I'll adjust it for you.\n",depOption,optionName,qPrint(expectedValue));
      ConfigValues::instance().*(option->value.s)=expectedValue; // adjust option
    }
  }
}

static void adjustColorStyleSetting(const char *depOption)
{
  auto updateColorStyle = [&depOption](HTML_COLORSTYLE_t curStyle,HTML_COLORSTYLE_t newStyle)
  {
    err("When enabling '%s' the 'HTML_COLORSTYLE' option should be either 'LIGHT' or 'DARK' but has value '%s'. I'll adjust it for you to '%s'.\n",
        depOption,
        qPrint(HTML_COLORSTYLE_enum2str(curStyle)),
        qPrint(HTML_COLORSTYLE_enum2str(newStyle)));
    Config_updateEnum(HTML_COLORSTYLE,newStyle);
  };
  auto colorStyle = Config_getEnum(HTML_COLORSTYLE);
  switch (colorStyle)
  {
    case HTML_COLORSTYLE_t::LIGHT:
    case HTML_COLORSTYLE_t::DARK:
      // no adjustment needed
      break;
    case HTML_COLORSTYLE_t::AUTO_LIGHT:
    case HTML_COLORSTYLE_t::TOGGLE:
      updateColorStyle(colorStyle,HTML_COLORSTYLE_t::LIGHT);
      break;
    case HTML_COLORSTYLE_t::AUTO_DARK:
      updateColorStyle(colorStyle,HTML_COLORSTYLE_t::DARK);
      break;
  }
}


void Config::checkAndCorrect(bool quiet, const bool check)
{
  ConfigValues::instance().init();

  Config_updateBool(QUIET,quiet || Config_getBool(QUIET));
  //------------------------
  // check WARN_FORMAT
  QCString warnFormat = Config_getString(WARN_FORMAT);
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
    warn_uncond("warning format does not contain a $text tag!\n");
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
  if (check && !headerFile.isEmpty())
  {
    FileInfo fi(headerFile.str());
    if (!fi.exists())
    {
      ConfigImpl::config_term("tag HTML_HEADER: header file '%s' "
          "does not exist\n",qPrint(headerFile));
    }
  }

  //------------------------
  // Test to see if HTML footer is valid
  QCString footerFile = Config_getString(HTML_FOOTER);
  if (check && !footerFile.isEmpty())
  {
    FileInfo fi(footerFile.str());
    if (!fi.exists())
    {
      ConfigImpl::config_term("tag HTML_FOOTER: footer file '%s' "
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
    if (check && !mathJaxCodefile.isEmpty())
    {
      FileInfo fi(mathJaxCodefile.str());
      if (!fi.exists())
      {
        ConfigImpl::config_term("tag MATHJAX_CODEFILE file '%s' "
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
  if (check && !latexHeaderFile.isEmpty())
  {
    FileInfo fi(latexHeaderFile.str());
    if (!fi.exists())
    {
      ConfigImpl::config_term("tag LATEX_HEADER: header file '%s' "
          "does not exist\n",qPrint(latexHeaderFile));
    }
  }

  //------------------------
  // Test to see if LaTeX footer is valid
  QCString latexFooterFile = Config_getString(LATEX_FOOTER);
  if (check && !latexFooterFile.isEmpty())
  {
    FileInfo fi(latexFooterFile.str());
    if (!fi.exists())
    {
      ConfigImpl::config_term("tag LATEX_FOOTER: footer file '%s' "
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
  // check EXTENSION_MAPPING
  checkList(Config_getList(EXTENSION_MAPPING),"EXTENSION_MAPPING",TRUE,TRUE);

  //------------------------
  // check FILTER_PATTERNS
  checkList(Config_getList(FILTER_PATTERNS),"FILTER_PATTERNS",TRUE,TRUE);

  //------------------------
  // check FILTER_SOURCE_PATTERNS
  checkList(Config_getList(FILTER_SOURCE_PATTERNS),"FILTER_SOURCE_PATTERNS",FALSE,FALSE);

  //------------------------
  // check INPUT_FILE_ENCODING
  checkList(Config_getList(INPUT_FILE_ENCODING),"INPUT_FILE_ENCODING",TRUE,TRUE);

  //------------------------
  // check TAGFILES
  checkList(Config_getList(TAGFILES),"TAGFILES",FALSE,TRUE);

  //------------------------
  // check EXTRA_SEARCH_MAPPINGS
  if (Config_getBool(SEARCHENGINE) && Config_getBool(GENERATE_HTML))
  {
    checkList(Config_getList(EXTRA_SEARCH_MAPPINGS),"EXTRA_SEARCH_MAPPING",TRUE,TRUE);
  }

  int numThreads = Config_getInt(NUM_PROC_THREADS);
  if (numThreads==0)
  {
     numThreads = static_cast<int>(std::thread::hardware_concurrency());
     Config_updateInt(NUM_PROC_THREADS,numThreads);
  }

  //------------------------

  // check for settings that are inconsistent with having GENERATE_HTMLHELP enabled
  if (Config_getBool(GENERATE_HTMLHELP))
  {
    const char *depOption = "GENERATE_HTMLHELP";
    adjustBoolSetting(  depOption, "GENERATE_TREEVIEW",    false  );
    adjustBoolSetting(  depOption, "SEARCHENGINE",         false  );
    adjustBoolSetting(  depOption, "HTML_DYNAMIC_MENUS",   false  );
    adjustBoolSetting(  depOption, "HTML_DYNAMIC_SECTIONS",false  );
    adjustBoolSetting(  depOption, "HTML_COPY_CLIPBOARD",  false  );
    adjustStringSetting(depOption, "HTML_FILE_EXTENSION", ".html");
    adjustColorStyleSetting(depOption);
    const StringVector &tagFileList = Config_getList(TAGFILES);
    StringVector filteredTagFileList;
    for (const auto &s : tagFileList)
    {
      bool validUrl = false;
      size_t eqPos = s.find('=');
      if (eqPos!=std::string::npos) // tag command contains a destination
      {
        QCString url = QCString(s.substr(eqPos+1)).stripWhiteSpace().lower();
        validUrl = url.startsWith("http:") || url.startsWith("https:");
      }
      if (validUrl)
      {
        filteredTagFileList.push_back(s);
      }
      else
      {
        err("When enabling GENERATE_HTMLHELP the TAGFILES option should only contain destinations "
            "with https / http addresses (not: %s). I'll adjust it for you.\n",s.c_str());
      }
    }
    Config_updateList(TAGFILES,filteredTagFileList);
  }

  // check for settings that are inconsistent with having INLINE_GROUPED_CLASSES enabled
  if (Config_getBool(INLINE_GROUPED_CLASSES))
  {
    const char *depOption = "INLINE_GROUPED_CLASSES";
    adjustBoolSetting(depOption, "SEPARATE_MEMBER_PAGES", false);
  }

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
        plantumlJarPath = plantumlJar;
      }
      else
      {
        err("Jar file 'plantuml.jar' not found at location "
                   "specified via PLANTUML_JAR_PATH: '%s'\n",qPrint(plantumlJarPath));
        plantumlJarPath="";
      }
    }
    else if (pu.exists() && pu.isFile()) // PLANTUML_JAR_PATH is file
    {
      // Nothing to be done
    }
    else
    {
      err("PLANTUML_JAR_PATH is not a directory with a 'plantuml.jar' file or is not an existing file: %s\n",
                 qPrint(plantumlJarPath));
      plantumlJarPath="";
    }
    Config_updateString(PLANTUML_JAR_PATH,plantumlJarPath);
  }

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
      size_t i=0,l=diaPath.length();
      for (i=0;i<l;i++) if (diaPath.at(i)=='/') diaPath.at(i)='\\';
#endif
    }
    Config_updateString(DIA_PATH,diaPath);
  }

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
  // check sitemap creation requirements
  if (!Config_getBool(GENERATE_HTML) &&
      !Config_getString(SITEMAP_URL).isEmpty())
  {
    warn_uncond("Setting SITEMAP_URL requires GENERATE_HTML=YES.\n");
  }

  //------------------------
  // check QHP creation requirements
  if (Config_getBool(GENERATE_QHP))
  {
    if (!Config_getBool(GENERATE_HTML))
    {
      warn_uncond("GENERATE_QHP=YES requires GENERATE_HTML=YES.\n");
    }
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
    const StringVector &tagFileList = Config_getList(TAGFILES);
    if (!tagFileList.empty())
    {
      err("When enabling GENERATE_QHP the TAGFILES option should be empty. I'll adjust it for you.\n");
      Config_updateList(TAGFILES,StringVector());
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
  // check for settings that are inconsistent with having OPTIMIZED_OUTPUT_VHDL enabled
  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
  {
    const char *depOption = "OPTIMIZE_OUTPUT_VHDL";
    adjustBoolSetting(depOption,"INLINE_INHERITED_MEMB",false);
    adjustBoolSetting(depOption,"INHERIT_DOCS",         false);
    adjustBoolSetting(depOption,"HIDE_SCOPE_NAMES",     true );
    adjustBoolSetting(depOption,"EXTRACT_PRIVATE",      true );
    adjustBoolSetting(depOption,"ENABLE_PREPROCESSING", false);
    adjustBoolSetting(depOption,"EXTRACT_PACKAGE",      true );
  }

  if (!checkFileName(Config_getString(GENERATE_TAGFILE),"GENERATE_TAGFILE"))
  {
    Config_updateString(GENERATE_TAGFILE,"");
  }

#if 0 // TODO: this breaks test 25; SOURCEBROWSER = NO and SOURCE_TOOLTIPS = YES.
      // So this and other regressions should be analyzed and fixed before this can be enabled
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

static void updateAttribute(DotAttributes& attr, QCString name, ConfigObsolete* value)
{
  attr.updateValue(name,*value->valueStringRef());
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
    if (classDiagramsOpt_ && haveDotOpt_ && classGraphOpt_ &&
        classDiagramsOpt_->isPresent() && classDiagramsOpt_->orgType()==ConfigOption::O_Bool)
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

  // update TIMESTAMP based on HTML_TIMESTAMP and LATEX_TIMESTAMP
  ConfigOption *HtmlTimestamp = ConfigImpl::instance()->get("HTML_TIMESTAMP");
  ConfigOption *timestampOpt    = ConfigImpl::instance()->get("TIMESTAMP");
  bool reset = false;
  if (HtmlTimestamp && HtmlTimestamp->kind()==ConfigOption::O_Obsolete && timestampOpt)
  {
    ConfigObsolete *htmlTimestamp_ = dynamic_cast<ConfigObsolete*>(HtmlTimestamp);
    ConfigEnum     *timestampOpt_ = dynamic_cast<ConfigEnum*>(timestampOpt);
    if (htmlTimestamp_ && timestampOpt_ &&
        htmlTimestamp_->isPresent() && htmlTimestamp_->orgType()==ConfigOption::O_Bool)
    {
      QCString &timestampValue  = *timestampOpt_->valueRef();
      QCString htmlTimestampValue = *htmlTimestamp_->valueStringRef();
      bool isValid=true;
      bool bTimestamp = convertStringToBool(htmlTimestampValue,isValid);
      if (isValid && bTimestamp)
      {
        reset = true;
        timestampValue = "YES";
      }
    }
  }
  ConfigOption *LatexTimestamp = ConfigImpl::instance()->get("LATEX_TIMESTAMP");
  if (!reset && LatexTimestamp && LatexTimestamp->kind()==ConfigOption::O_Obsolete && timestampOpt)
  {
    ConfigObsolete *latexTimestamp_ = dynamic_cast<ConfigObsolete*>(LatexTimestamp);
    ConfigEnum     *timestampOpt_ = dynamic_cast<ConfigEnum*>(timestampOpt);
    if (latexTimestamp_ && timestampOpt_ &&
        latexTimestamp_->isPresent() && latexTimestamp_->orgType()==ConfigOption::O_Bool)
    {
      QCString &timestampValue  = *timestampOpt_->valueRef();
      QCString latexTimestampValue = *latexTimestamp_->valueStringRef();
      bool isValid=true;
      bool bTimestamp = convertStringToBool(latexTimestampValue,isValid);
      if (isValid && bTimestamp) timestampValue = "YES";
    }
  }

  auto fontname = dynamic_cast<ConfigObsolete*>(ConfigImpl::instance()->get("DOT_FONTNAME"));
  auto fontsize = dynamic_cast<ConfigObsolete*>(ConfigImpl::instance()->get("DOT_FONTSIZE"));

  // correct DOT_FONTNAME if needed
  if (fontname &&
      (*fontname->valueStringRef() == "FreeSans"
      || *fontname->valueStringRef() == "FreeSans.ttf"))
    warn_uncond("doxygen no longer ships with the FreeSans font.\n"
               "  You may want to clear or change DOT_FONTNAME.\n"
               "  Otherwise you run the risk that the wrong font is being used for dot generated graphs.\n");

  auto commonAttrOpt = dynamic_cast<ConfigString*>(ConfigImpl::instance()->get("DOT_COMMON_ATTR"));
  if (commonAttrOpt)
  {
    QCString& commonAttrStr = *commonAttrOpt->valueRef();
    DotAttributes commonAttr(commonAttrStr);
    updateAttribute(commonAttr, "fontname", fontname);
    updateAttribute(commonAttr, "fontsize", fontsize);
    commonAttrStr = commonAttr.str();
  }

  auto edgeAttrOpt = dynamic_cast<ConfigString*>(ConfigImpl::instance()->get("DOT_EDGE_ATTR"));
  if (edgeAttrOpt)
  {
    QCString& edgeAttrStr = *edgeAttrOpt->valueRef();
    DotAttributes edgeAttr(edgeAttrStr);
    updateAttribute(edgeAttr, "labelfontname", fontname);
    updateAttribute(edgeAttr, "labelfontsize", fontsize);
    edgeAttrStr = edgeAttr.str();
  }
}

void Config::writeTemplate(TextStream &t,bool shortList,bool update)
{
  ConfigImpl::instance()->writeTemplate(t,shortList,update);
}

void Config::compareDoxyfile(TextStream &t,Config::CompareMode compareMode)
{
  postProcess(FALSE, compareMode);
  ConfigImpl::instance()->compareDoxyfile(t, compareMode);
}

void Config::writeXMLDoxyfile(TextStream &t)
{
  ConfigImpl::instance()->writeXMLDoxyfile(t);
}

void Config::writeXSDDoxyfile(TextStream &t)
{
  ConfigImpl::instance()->writeXSDDoxyfile(t);
}

bool Config::parse(const QCString &fileName,bool update, Config::CompareMode compareMode)
{
  g_compareMode = compareMode;
  bool parseRes = ConfigImpl::instance()->parse(fileName,update);
  if (!parseRes) return parseRes;

  // Internally we use the default format UTF-8 and
  // when updating etc. the output is in this format as well and not in the read format
  ConfigString *option = dynamic_cast<ConfigString*>(g_config->get("DOXYFILE_ENCODING"));
  option->init();

  return parseRes;
}

void Config::postProcess(bool clearHeaderAndFooter, Config::CompareMode compareMode)
{
  auto configInst = ConfigImpl::instance();
  if (compareMode != CompareMode::CompressedNoEnv) configInst->substituteEnvironmentVars();
  if (compareMode == CompareMode::Full)            configInst->emptyValueToDefault();
  configInst->convertStrToVal(compareMode);

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

#include "configimpl.l.h"
