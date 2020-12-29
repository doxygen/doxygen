/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

%option never-interactive
%option prefix="config_doxywYY"
%top{
#include <stdint.h>
}

%{

/*
 *	includes
 */
#include "config.h"
#include "input.h"
#include "inputstring.h"
#include "config_msg.h"

#include <QString>
#include <QVariant>
#include <QStack>
#include <QTextCodec>
#include <QByteArray>
#include <QFileInfo>
#include <QStringList>
#include <QRegExp>
#include <QTextStream>
#include <QMessageBox>

#define YY_NO_UNISTD_H 1

#define MAX_INCLUDE_DEPTH 10


/* -----------------------------------------------------------------
 *
 *	static variables
 */

struct ConfigFileState
{
  int lineNr;
  FILE *file;
  YY_BUFFER_STATE oldState;
  YY_BUFFER_STATE newState;
  QString fileName;
};

static const QHash<QString,Input*>   *g_options;
static FILE                          *g_file;
static QString                        g_yyFileName;
static QString                        g_includeName;
static QVariant                       g_includePathList;
static QStack<ConfigFileState*>       g_includeStack;
static int                            g_includeDepth;
static QVariant                      *g_arg;
static Input                         *g_curOption=0;
static QByteArray                     g_str;
static QTextCodec                    *g_codec     = QTextCodec::codecForName("UTF-8");
static QString                        g_codecName = QString::fromLatin1("UTF-8");
static QString                        g_cmd;
static bool                           g_isEnum;

static const char *stateToString(int state);

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static yy_size_t yyread(char *buf,yy_size_t maxSize)
{
    // no file included
    if (g_includeStack.isEmpty())
    {
      return fread(buf,1,maxSize,g_file);
    }
    else
    {
      return fread(buf,1,maxSize,g_includeStack.top()->file);
    }
}

static void substEnvVarsInStrList(QStringList &sl);
static void substEnvVarsInString(QString &s);

static void checkEncoding()
{
  Input *option = g_options->value(QString::fromLatin1("DOXYFILE_ENCODING"));
  if (option && option->value().toString()!=g_codecName)
  {
    QTextCodec *newCodec = QTextCodec::codecForName(option->value().toString().toLatin1());
    if (newCodec)
    {
      g_codec = newCodec;
      g_codecName = option->value().toString();
    }
  }
}

static QByteArray stripComment(const QByteArray &s)
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
      return s.left(i).trimmed();
    }
  }
  return s;
}


static void processString()
{
  // strip leading and trailing whitespace
  QByteArray s = stripComment(g_str.trimmed());
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
  QByteArray result;
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
        std::string str = g_str.trimmed().data();
        config_warn("Invalid value for '%s' tag at line %d, file %s: Value '%s' is not properly quoted\n",
                    qPrintable(g_cmd),yylineno-1,qPrintable(g_yyFileName),str.c_str());
      }
      warned=true;
    }
    else // normal character
    {
      result+=c;
    }
  }

  // recode the string
  if (g_isEnum)
  {
    InputString *cur = dynamic_cast<InputString *>(g_curOption);
    *g_arg = cur->checkEnumVal(g_codec->toUnicode(result));
  }
  else
  {
    *g_arg = QVariant(g_codec->toUnicode(result));
  }

  // update encoding
  checkEncoding();

  //printf("Processed string '%s'\n",g_string->data());
}

static void processList()
{
  bool allowCommaAsSeparator = g_cmd!=QString::fromLatin1("PREDEFINED");

  const QByteArray s = stripComment(g_str.trimmed());
  int l = s.length();

  QByteArray elemStr;

  // helper to push elemStr to the list and clear it
  auto addElem = [&elemStr]()
  {
    if (!elemStr.isEmpty())
    {
      //printf("Processed list element '%s'\n",e.data());
      *g_arg = QVariant(g_arg->toStringList() << g_codec->toUnicode(elemStr));
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
          std::string str = g_str.trimmed().data();
          config_warn("Invalid value for '%s' tag at line %d, file %s: Values in list '%s' are not properly space %sseparated\n",
                    qPrintable(g_cmd),yylineno-1,qPrintable(g_yyFileName),str.c_str(),allowCommaAsSeparator?"or comma ":"");
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
    std::string str = g_str.trimmed().data();
    config_warn("Invalid value for '%s' tag at line %d, file %s: Values in list '%s' are not properly quoted\n",
                qPrintable(g_cmd),yylineno-1,qPrintable(g_yyFileName),str.c_str());
  }
}


static FILE *tryPath(const QString &path,const QString &fileName)
{
  QString absName=!path.isEmpty() ? path+QString::fromLatin1("/")+fileName : fileName;
  QFileInfo fi(absName);
  if (fi.exists() && fi.isFile())
  {
    FILE *f = fopen(absName.toLocal8Bit(),"r");
    if (f==NULL)
      config_err("could not open file %s for reading\n",qPrintable(absName));
    else
      return f;
  }
  return NULL;
}

static FILE *findFile(const QString &fileName)
{
  if (QFileInfo(fileName).isAbsolute()) // absolute path
  {
    return tryPath(QString(), fileName);
  }

  // relative path, try with include paths in the list
  QStringList sl = g_includePathList.toStringList();
  substEnvVarsInStrList(sl);
  foreach (QString s, sl)
  {
    FILE *f = tryPath(s,fileName);
    if (f) return f;
  }
  // try cwd if g_includePathList fails
  return tryPath(QString::fromLatin1("."),fileName);
}

static void readIncludeFile(const QString &incName)
{
  if (g_includeDepth==MAX_INCLUDE_DEPTH)
  {
    config_err("maximum include depth (%d) reached, %s is not included.",
	MAX_INCLUDE_DEPTH,qPrintable(incName));
  }

  QString inc = incName;
  substEnvVarsInString(inc);
  inc = inc.trimmed();
  uint incLen = inc.length();
  if (inc.at(0)==QChar::fromLatin1('"') &&
      inc.at(incLen-1)==QChar::fromLatin1('"')) // strip quotes
  {
    inc=inc.mid(1,incLen-2);
  }

  FILE *f = findFile(inc);
  if (f) // see if the include file can be found
  {
    // For debugging
#if SHOW_INCLUDES
    for (i=0;i<includeStack.count();i++) msg("  ");
    msg("@INCLUDE = %s: parsing...\n",qPrintable(inc));
#endif

    // store the state of the old file
    ConfigFileState *fs=new ConfigFileState;
    fs->oldState=YY_CURRENT_BUFFER;
    fs->fileName=g_yyFileName;
    fs->file=f;
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
    config_err("@INCLUDE = %s: not found!\n",qPrintable(inc));
  }
}


%}

%option nounput
%option noyywrap
%option yylineno

%x      Start
%x      SkipInvalid
%x      GetString
%x      GetEnum
%x      GetStrList
%x      GetQuotedString
%x      Include

%%

<*>\0x0d

   /*-------------- Comments ---------------*/

<Start>"#".*\n	                         { /* Skip comment */ }

   /*-------------- TAG start ---------------*/

<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"="	 { g_cmd = g_codec->toUnicode(yytext);
                                           g_cmd=g_cmd.left(g_cmd.length()-1).trimmed();
					   g_curOption = g_options->value(g_cmd);
					   if (g_curOption==0) // oops not known
					   {
					     config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
						 qPrintable(g_cmd),yylineno,qPrintable(g_yyFileName));
					     BEGIN(SkipInvalid);
					   }
					   else // known tag
					   {
					     g_arg = &g_curOption->value();
                                             g_str = QByteArray();
                                             g_isEnum = false;
					     switch(g_curOption->kind())
					     {
					       case Input::StrList:
						 *g_arg = QStringList();
					         BEGIN(GetStrList);
					         break;
					       case Input::String:
                                                 g_isEnum = dynamic_cast<InputString *>(g_curOption)->stringMode() == InputString::StringFixed;
                                                 BEGIN(GetString);
					         break;
					       case Input::Int:
					         BEGIN(GetString);
					         break;
					       case Input::Bool:
					         BEGIN(GetString);
						 break;
					       case Input::Obsolete:
					         config_warn("Tag '%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", qPrintable(g_cmd),
							    yylineno,qPrintable(g_yyFileName));
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"+="	{ g_cmd=g_codec->toUnicode(yytext);
                                          g_cmd=g_cmd.left(g_cmd.length()-2).trimmed();
					  g_curOption = g_options->value(g_cmd);
					  if (g_curOption==0) // oops not known
					  {
					    config_warn("ignoring unsupported tag '%s' at line %d, file %s\n",
						yytext,yylineno,qPrintable(g_yyFileName));
					    BEGIN(SkipInvalid);
					  }
					  else // known tag
					  {
					    switch(g_curOption->kind())
					    {
					      case Input::StrList:
						g_arg = &g_curOption->value();
						g_str=QByteArray();
					        BEGIN(GetStrList);
					        break;
					      case Input::String:
					      case Input::Int:
					      case Input::Bool:
					        config_warn("operator += not supported for '%s'. Ignoring line %d, file %s\n",
						    qPrintable(g_cmd),yylineno,qPrintable(g_yyFileName));
					        BEGIN(SkipInvalid);
						break;
					      case Input::Obsolete:
					         config_warn("Tag '%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n",
							    qPrintable(g_cmd),yylineno,qPrintable(g_yyFileName));
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}

   /*-------------- INCLUDE* ---------------*/

<Start>"@INCLUDE_PATH"[ \t]*"=" 	{ BEGIN(GetStrList); g_arg=&g_includePathList; *g_arg = QStringList(); g_str=QByteArray(); }
  /* include a config file */
<Start>"@INCLUDE"[ \t]*"="     		{ BEGIN(Include);}
<Include>([^ \"\t\r\n]+)|("\""[^\n\"]+"\"") {
  					  readIncludeFile(g_codec->toUnicode(yytext));
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
					    ConfigFileState *fs = g_includeStack.pop();
					    fclose(fs->file);
					    YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
					    yy_switch_to_buffer( fs->oldState );
					    yy_delete_buffer( oldBuf );
					    g_yyFileName=fs->fileName;
					    delete fs;
                                            g_includeDepth--;
					  }
  					}

<Start>[a-z_A-Z0-9]+			{ config_warn("ignoring unknown tag '%s' at line %d, file %s\n",yytext,yylineno,qPrintable(g_yyFileName)); }

   /*-------------- GetString ---------------*/

<GetString>\n                           { // end of string
                                          processString();
                                          BEGIN(Start);
                                        }
<GetString>\\[ \r\t]*\n                 { // line continuation
                                          g_str+=' ';
                                        }
<GetString>"\\"                         { // escape character
                                          g_str+=yytext;
                                        }
<GetString>[^\n\\]+                     { // string part without escape characters
                                          g_str+=yytext;
                                        }

   /*-------------- GetStrList ---------------*/

<GetStrList>\n                          { // end of list
                                          processList();
                                          BEGIN(Start);
                                        }
<GetStrList>\\[ \r\t]*\n                { // line continuation
                                          g_str+=' ';
                                        }
<GetStrList>"\\"                        { // escape character
                                          g_str+=yytext;
                                        }
<GetStrList>[^\n\\]+                    { // string part without escape characters
                                          g_str+=yytext;
                                        }

   /*-------------- SkipInvalid ---------------*/

<SkipInvalid>\n                         { // end of skipped part
                                          BEGIN(Start);
                                        }
<SkipInvalid>\\[ \r\t]*\n               { // line continuation
                                          g_str+=' ';
                                        }
<SkipInvalid>"\\"                       { // escape character
                                          g_str+=yytext;
                                        }
<SkipInvalid>[^\n\\]+                   { // string part without escape characters
                                          g_str+=yytext;
                                        }

   /*-------------- fall through -------------*/

<*>\\[ \r\t]*\n				{ }
<*>[ \r\t]				{ }
<*>\n
<*>.					{ config_warn("ignoring unknown character '%c' at line %d, file %s\n",yytext[0],yylineno,qPrintable(g_yyFileName)); }

%%

/*@ ----------------------------------------------------------------------------
 */

static void substEnvVarsInString(QString &s)
{
  static QRegExp re(QString::fromLatin1("\\$\\([a-z_A-Z0-9]+\\)"));
  if (s.isEmpty()) return;
  int p=0;
  int i,l;
  //printf("substEnvVarInString(%s) start\n",qPrintable(s));
  while ((i=re.indexIn(s,p))!=-1)
  {
    l = re.matchedLength();
    //printf("Found environment var s.mid(%d,%d)='%s'\n",i+2,l-3,qPrintable(s.mid(i+2,l-3)));
    QString env=g_codec->toUnicode(getenv(s.mid(i+2,l-3).toLatin1()));
    substEnvVarsInString(env); // recursively expand variables if needed.
    s = s.left(i)+env+s.right(s.length()-i-l);
    p=i+env.length(); // next time start at the end of the expanded string
  }
  s=s.trimmed(); // to strip the bogus space that was added when an argument
                         // has quotes
  //printf("substEnvVarInString(%s) end\n",qPrintable(s));
}

static void substEnvVarsInStrList(QStringList &sl)
{
  QStringList out;

  foreach (QString result, sl)
  {
    // an argument with quotes will have an extra space at the end, so wasQuoted will be TRUE.
    bool wasQuoted = (result.indexOf(QChar::fromLatin1(' '))!=-1) ||
                     (result.indexOf(QChar::fromLatin1('\t'))!=-1);
    // here we strip the quote again
    substEnvVarsInString(result);

    //printf("Result %s was quoted=%d\n",qPrintable(result),wasQuoted);

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
	QChar c=0;
	// skip until start of new word
	while (i<l && ((c=result.at(i))==QChar::fromLatin1(' ') || c==QChar::fromLatin1('\t'))) i++;
	p=i; // p marks the start index of the word
	// skip until end of a word
	while (i<l && ((c=result.at(i))!=QChar::fromLatin1(' ') &&
	              c!=QChar::fromLatin1('\t') &&
		      c!=QChar::fromLatin1('"'))) i++;
	if (i<l) // not at the end of the string
	{
	  if (c==QChar::fromLatin1('"')) // word within quotes
	  {
	    p=i+1;
	    for (i++;i<l;i++)
	    {
	      c=result.at(i);
	      if (c==QChar::fromLatin1('"')) // end quote
	      {
                out += result.mid(p,i-p);
		p=i+1;
		break;
	      }
	      else if (c==QChar::fromLatin1('\\')) // skip escaped stuff
	      {
		i++;
	      }
	    }
	  }
	  else if (c==QChar::fromLatin1(' ') || c==QChar::fromLatin1('\t')) // separator
	  {
            out += result.mid(p,i-p);
	    p=i+1;
	  }
	}
      }
      if (p!=l) // add the leftover as a string
      {
        out += result.right(l-p);
      }
    }
    else // just goto the next element in the list
    {
      out += result;
    }
  }
  sl = out;
}

//--------------------------------------------------------------------------

bool parseConfig(
      const QString &fileName,
      const QHash<QString,Input *> &options
    )
{
  yylineno = 1;
  config_open();
  QHashIterator<QString, Input*> i(options);
  g_file = fopen(fileName.toLocal8Bit(),"r");
  if (g_file==NULL) return false;

  // reset all values
  i.toFront();
  while (i.hasNext())
  {
    i.next();
    if (i.value())
    {
      i.value()->reset();
    }
  }

  // parse config file
  g_options       = &options;
  g_yyFileName    = fileName;
  g_includeStack.clear();
  g_includeDepth  = 0;
  config_doxywYYrestart( config_doxywYYin );
  BEGIN( Start );
  config_doxywYYlex();

  // update the values in the UI
  i.toFront();
  while (i.hasNext())
  {
    i.next();
    if (i.value())
    {
      //printf("Updating: %s\n",qPrintable(i.key()));
      i.value()->update();
    }
    else
    {
      printf("Invalid option: %s\n",qPrintable(i.key()));
    }
  }
  fclose(g_file);
  config_finish();
  return true;
}

void writeStringValue(QTextStream &t,QTextCodec *codec,const QString &s)
{
  QChar c;
  bool needsEscaping=false;
  bool needsHashEscaping=false;
  // convert the string back to it original encoding
  //QByteArray se = codec->fromUnicode(s);
  t.setCodec(codec);
  const QChar *p=s.data();
  if (!s.isEmpty() && !p->isNull())
  {
    if (*p != QChar::fromLatin1('"'))
    {
      while (!(c=*p++).isNull() && !needsEscaping)
      {
        needsEscaping = (c==QChar::fromLatin1(' ')  ||
	                 c==QChar::fromLatin1(',') ||
	                 c==QChar::fromLatin1('\n') ||
		         c==QChar::fromLatin1('\t') ||
		         c==QChar::fromLatin1('"'));
      }
      p=s.data();
      while (!(c=*p++).isNull() && !needsHashEscaping)
      {
        needsHashEscaping = (c==QChar::fromLatin1('#'));
      }
    }
    if (needsHashEscaping || needsEscaping)
    {
      t << "\"";
    }
    if (needsEscaping)
    {
      p=s.data();
      while (!p->isNull())
      {
	if (*p   ==QChar::fromLatin1(' ') &&
	   *(p+1)==QChar::fromLatin1('\0')) break; // skip inserted space at the end
	if (*p   ==QChar::fromLatin1('"')) t << "\\"; // escape quotes
	t << *p++;
      }
    }
    else
    {
      t << s;
    }
    if (needsHashEscaping || needsEscaping)
    {
      t << "\"";
    }
  }
}
#include "config_doxyw.l.h"
