/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include "config.h"
#include "util.h"

static QCString outputFormat;
static int warnFormatOrder; // 1 = $file,$line,$text
                            // 2 = $text,$line,$file
                            // 3 = $line,$text,$file
                            // 4 = $file,$text,$line
                            // 5 = $text,$file,$line
                            // 6 = $line,$file,$text

static FILE *warnFile = stderr;

void initWarningFormat()
{
  int filePos = Config_getString("WARN_FORMAT").find("$file");
  int linePos = Config_getString("WARN_FORMAT").find("$line");
  int textPos = Config_getString("WARN_FORMAT").find("$text");

  // sort items on position (there are 6 cases)
  warnFormatOrder = 1;
  if (filePos>linePos && filePos>textPos)
  {
    if (linePos>textPos) // $text,$line,$file
    {
      warnFormatOrder = 2;
    }
    else                 // $line,$text,$file
    {
      warnFormatOrder = 3;
    }
  }
  else if (filePos<linePos && filePos<textPos)
  {
    if (linePos>textPos) // $file,$text,$line
    {
      warnFormatOrder = 4;
    }
  }
  else if (filePos<linePos && filePos>textPos) // $text,$file,$line
  {
    warnFormatOrder = 5;
  }
  else // $line,$file,$text
  {
    warnFormatOrder = 6;
  }
  outputFormat = 
      substitute(
        substitute(
          substitute( 
            Config_getString("WARN_FORMAT"),
           "$file","%s"
          ),
          "$text","%s"
        ),
        "$line","%d"
      )+'\n';

  //    replace(QRegExp("\\$file"),"%s").
  //    replace(QRegExp("\\$text"),"%s").
  //    replace(QRegExp("\\$line"),"%d")+
  //    '\n';

  if (!Config_getString("WARN_LOGFILE").isEmpty())
  {
    warnFile = fopen(Config_getString("WARN_LOGFILE"),"w");
  }
  if (!warnFile) // point it to something valid, because warn() relies on it
  {
    warnFile = stderr;
  }
}


void msg(const char *fmt, ...)
{
  if (!Config_getBool("QUIET"))
  {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args); 
  }
}

static void do_warn(const char *tag, const char *file, int line, const char *fmt, va_list args)
{
  if (!Config_getBool(tag)) return; // warning type disabled
  char text[4096];
  vsprintf(text, fmt, args);
  if (file==0) file="<unknown>";
  switch(warnFormatOrder)
  {
    case 1: fprintf(warnFile,outputFormat,file,line,text); break;
    case 2: fprintf(warnFile,outputFormat,text,line,file); break;
    case 3: fprintf(warnFile,outputFormat,line,text,file); break;
    case 4: fprintf(warnFile,outputFormat,file,text,line); break;
    case 5: fprintf(warnFile,outputFormat,text,file,line); break;
    case 6: fprintf(warnFile,outputFormat,line,file,text); break;
    default:
      printf("Error: warning format has not been initialized!\n");
  }
}

void warn(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn("WARNINGS", file, line, fmt, args);
  va_end(args); 
}

void warn_cont(const char *fmt, ...)
{
  if (!Config_getBool("WARNINGS"))
    return;
  va_list args;
  va_start(args, fmt);
  vfprintf(warnFile, fmt, args);
  va_end(args); 
}
  
void warn_undoc(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn("WARN_IF_UNDOCUMENTED", file, line, fmt, args);
  va_end(args);
}
  
void warn_doc_error(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn("WARN_IF_DOC_ERROR", file, line, fmt, args);
  va_end(args);
}

void err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(warnFile, fmt, args);
  va_end(args); 
}
