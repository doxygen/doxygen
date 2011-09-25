/******************************************************************************
 *
 * $Id: config_templ.l,v 1.8 2001/01/01 10:15:16 root Exp $
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
#include "config.h"
#include "input.h"
#include <QtCore>

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
static QString                        g_elemStr;
static QTextCodec                    *g_codec     = QTextCodec::codecForName("UTF-8");
static QString                        g_codecName = QString::fromAscii("UTF-8");
static int                            g_lastState;
static QByteArray                     g_tmpString;

/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int maxSize)
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

static void substEnvVarsInStrList(QStringList &sl);
static void substEnvVarsInString(QString &s);

static void checkEncoding()
{
  Input *option = g_options->value(QString::fromAscii("DOXYFILE_ENCODING"));
  if (option && option->value().toString()!=g_codecName)
  {
    QTextCodec *newCodec = QTextCodec::codecForName(option->value().toString().toAscii());
    if (newCodec)
    {
      g_codec = newCodec;
      g_codecName = option->value().toString();
    }
  }
}

static FILE *tryPath(const QString &path,const QString &fileName)
{
  QString absName=!path.isEmpty() ? path+QString::fromAscii("/")+fileName : fileName;
  QFileInfo fi(absName);
  if (fi.exists() && fi.isFile())
  {
    FILE *f = fopen(absName.toLocal8Bit(),"r");
    if (f==NULL)
      config_err("Error: could not open file %s for reading\n",absName.toLatin1().data());
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
  return tryPath(QString::fromAscii("."),fileName);
}

static void readIncludeFile(const QString &incName)
{
  if (g_includeDepth==MAX_INCLUDE_DEPTH) 
  {
    config_err("Error: maximum include depth (%d) reached, %s is not included. Aborting...\n",
	MAX_INCLUDE_DEPTH,qPrintable(incName));
    exit(1);
  } 

  QString inc = incName;
  substEnvVarsInString(inc);
  inc = inc.trimmed();
  uint incLen = inc.length();
  if (inc.at(0)==QChar::fromAscii('"') && 
      inc.at(incLen-1)==QChar::fromAscii('"')) // strip quotes
  {
    inc=inc.mid(1,incLen-2);
  }

  FILE *f = findFile(inc);
  if (f) // see if the include file can be found
  {
    // For debugging
#if SHOW_INCLUDES
    for (i=0;i<includeStack.count();i++) msg("  ");
    msg("@INCLUDE = %s: parsing...\n",inc.toLatin1().data());
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
    config_err("Error: @INCLUDE = %s: not found!\n",inc.toLatin1().data());
    exit(1);
  }
}


%}

%option nounput
%option noyywrap
%option yylineno

%x      Start
%x	SkipComment
%x      SkipInvalid
%x      GetString
%x      GetStrList
%x      GetQuotedString
%x      GetEnvVar
%x      Include

%%

<*>\0x0d
<Start,GetString,GetStrList,SkipInvalid>"#"	 { BEGIN(SkipComment); }
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"="	 { QString cmd = g_codec->toUnicode(yytext);
                                           cmd=cmd.left(cmd.length()-1).trimmed(); 
					   g_curOption = g_options->value(cmd);
					   if (g_curOption==0) // oops not known
					   {
					     config_err("Warning: ignoring unsupported tag `%s' at line %d, file %s\n",
						 qPrintable(cmd),yylineno,qPrintable(g_yyFileName)); 
					     BEGIN(SkipInvalid);
					   }
					   else // known tag
					   {
					     //option->setEncoding(encoding);
					     g_arg = &g_curOption->value();
					     switch(g_curOption->kind())
					     {
					       case Input::StrList:
						 g_elemStr = QString();
						 *g_arg = QStringList();
					         BEGIN(GetStrList);
					         break;
					       case Input::String:
					         BEGIN(GetString);
					         break;
					       case Input::Int:
					         BEGIN(GetString);
					         break;
					       case Input::Bool:
					         BEGIN(GetString);
						 break;
					       case Input::Obsolete:
					         config_err("Warning: Tag `%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", qPrintable(cmd),
							    yylineno,qPrintable(g_yyFileName)); 
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>[a-z_A-Z][a-z_A-Z0-9]*[ \t]*"+="	{ QString cmd=g_codec->toUnicode(yytext);
                                          cmd=cmd.left(cmd.length()-2).trimmed(); 
					  g_curOption = g_options->value(cmd);
					  if (g_curOption==0) // oops not known
					  {
					    config_err("Warning: ignoring unsupported tag `%s' at line %d, file %s\n",
						yytext,yylineno,qPrintable(g_yyFileName)); 
					    BEGIN(SkipInvalid);
					  }
					  else // known tag
					  {
					    switch(g_curOption->kind())
					    {
					      case Input::StrList:
						g_arg = &g_curOption->value();
						g_elemStr=QString();
					        BEGIN(GetStrList);
					        break;
					      case Input::String:
					      case Input::Int:
					      case Input::Bool:
					        config_err("Warning: operator += not supported for `%s'. Ignoring line at line %d, file %s\n",
						    yytext,yylineno,qPrintable(g_yyFileName)); 
					        BEGIN(SkipInvalid);
						break;
					      case Input::Obsolete:
					         config_err("Warning: Tag `%s' at line %d of file %s has become obsolete.\n"
						            "To avoid this warning please update your configuration "
							    "file using \"doxygen -u\"\n", 
							    qPrintable(cmd),yylineno,qPrintable(g_yyFileName)); 
					         BEGIN(SkipInvalid);
						 break;
					     }
					   }
					}
<Start>"@INCLUDE_PATH"[ \t]*"=" 	{ BEGIN(GetStrList); g_arg=&g_includePathList; *g_arg = QStringList(); g_elemStr=QString(); }
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

<Start>[a-z_A-Z0-9]+			{ config_err("Warning: ignoring unknown tag `%s' at line %d, file %s\n",yytext,yylineno,qPrintable(g_yyFileName)); }
<GetString,SkipInvalid>\n	        { BEGIN(Start); }
<GetStrList>\n				{ 
					  if (!g_elemStr.isEmpty())
					  {
					    //printf("elemStr1=`%s'\n",elemStr.toLatin1().data());
					    *g_arg = QVariant(g_arg->toStringList() << g_elemStr);
					  }
					  BEGIN(Start); 
					}
<GetStrList>[ \t]+			{
  				          if (!g_elemStr.isEmpty())
					  {
					    //printf("elemStr2=`%s'\n",elemStr.toLatin1().data());
  					    *g_arg = QVariant(g_arg->toStringList() << g_elemStr);
					  }
					  g_elemStr = QString();
  					}
<GetString>[^ \"\t\r\n]+		{ 
                                          *g_arg = QVariant(g_codec->toUnicode(yytext)); 
                                          checkEncoding();
                                        }
<GetString,GetStrList,SkipInvalid>"\""	{ g_lastState=YY_START;
  					  BEGIN(GetQuotedString); 
                                          g_tmpString="";
					}
<GetQuotedString>"\""|"\n" 		{ 
                                          // we add a bogus space to signal that the string was quoted. This space will be stripped later on.
                                          g_tmpString+=" ";
  					  //printf("Quoted String = `%s'\n",tmpString.toLatin1().data());
  					  if (g_lastState==GetString)
					  {
					    *g_arg = g_codec->toUnicode(g_tmpString);
                                            checkEncoding();
					  }
					  else
					  {
					    g_elemStr+=g_codec->toUnicode(g_tmpString);
					  }
					  if (*yytext=='\n')
					  {
					    config_err("Warning: Missing end quote (\") on line %d, file %s\n",yylineno,
                                                qPrintable(g_yyFileName));
					  }
					  BEGIN(g_lastState);
  					}
<GetQuotedString>"\\\""			{
  					  g_tmpString+='"';
  					}
<GetQuotedString>.			{ g_tmpString+=*yytext; }
<GetStrList>[^ \#\"\t\r\n]+		{
  					  g_elemStr+=g_codec->toUnicode(yytext);
  					}
<SkipComment>\n				{ BEGIN(Start); }
<SkipComment>\\[ \r\t]*\n		{ BEGIN(Start); }
<*>\\[ \r\t]*\n				{ }
<*>\n
<*>.					

%%

/*@ ----------------------------------------------------------------------------
 */

static void substEnvVarsInString(QString &s)
{
  static QRegExp re(QString::fromAscii("\\$\\([a-z_A-Z0-9]+\\)"));
  if (s.isEmpty()) return;
  int p=0;
  int i,l;
  //printf("substEnvVarInString(%s) start\n",s.toLatin1().data());
  while ((i=re.indexIn(s,p))!=-1)
  {
    l = re.matchedLength();
    //printf("Found environment var s.mid(%d,%d)=`%s'\n",i+2,l-3,s.mid(i+2,l-3).toLatin1().data());
    QString env=g_codec->toUnicode(getenv(s.mid(i+2,l-3).toLatin1()));
    substEnvVarsInString(env); // recursively expand variables if needed.
    s = s.left(i)+env+s.right(s.length()-i-l);
    p=i+env.length(); // next time start at the end of the expanded string
  }
  s=s.trimmed(); // to strip the bogus space that was added when an argument
                         // has quotes
  //printf("substEnvVarInString(%s) end\n",s.toLatin1().data());
}

static void substEnvVarsInStrList(QStringList &sl)
{
  QStringList out;

  foreach (QString result, sl)
  {
    // an argument with quotes will have an extra space at the end, so wasQuoted will be TRUE.
    bool wasQuoted = (result.indexOf(QChar::fromAscii(' '))!=-1) || 
                     (result.indexOf(QChar::fromAscii('\t'))!=-1);
    // here we strip the quote again
    substEnvVarsInString(result);

    //printf("Result %s was quoted=%d\n",result.toLatin1().data(),wasQuoted);

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
	QChar c=0;
	// skip until start of new word
	while (i<l && ((c=result.at(i))==QChar::fromAscii(' ') || c==QChar::fromAscii('\t'))) i++; 
	p=i; // p marks the start index of the word
	// skip until end of a word
	while (i<l && ((c=result.at(i))!=QChar::fromAscii(' ') && 
	              c!=QChar::fromAscii('\t') && 
		      c!=QChar::fromAscii('"'))) i++;
	if (i<l) // not at the end of the string
	{
	  if (c==QChar::fromAscii('"')) // word within quotes
	  {
	    p=i+1;
	    for (i++;i<l;i++)
	    {
	      c=result.at(i);
	      if (c==QChar::fromAscii('"')) // end quote
	      {
                out += result.mid(p,i-p);
		p=i+1;
		break; 
	      }
	      else if (c==QChar::fromAscii('\\')) // skip escaped stuff
	      {
		i++;
	      }
	    }
	  }
	  else if (c==QChar::fromAscii(' ') || c==QChar::fromAscii('\t')) // separator
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
  configrestart( configin );
  BEGIN( Start );
  configlex();

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
  return true;
}

void writeStringValue(QTextStream &t,QTextCodec *codec,const QString &s)
{
  QChar c;
  bool needsEscaping=FALSE;
  // convert the string back to it original encoding
  //QByteArray se = codec->fromUnicode(s);
  t.setCodec(codec);
  const QChar *p=s.data();
  if (!s.isEmpty() && !p->isNull())
  {
    while (!(c=*p++).isNull() && !needsEscaping) 
    {
      needsEscaping = (c==QChar::fromAscii(' ')  || 
	               c==QChar::fromAscii('\n') || 
		       c==QChar::fromAscii('\t') || 
		       c==QChar::fromAscii('"'));
    }
    if (needsEscaping)
    { 
      t << "\"";
      p=s.data();
      while (!p->isNull())
      {
	if (*p   ==QChar::fromAscii(' ') && 
	   *(p+1)==QChar::fromAscii('\0')) break; // skip inserted space at the end
	if (*p   ==QChar::fromAscii('"')) t << "\\"; // escape quotes
	t << *p++;
      }
      t << "\"";
    }
    else
    {
      t << s;
    }
  }
}

