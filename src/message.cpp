/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

void initWarningFormat()
{
  int filePos = Config::warnFormat.find("$file");
  int linePos = Config::warnFormat.find("$line");
  int textPos = Config::warnFormat.find("$text");

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
            Config::warnFormat,
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
}


void msg(const char *fmt, ...)
{
  if (!Config::quietFlag)
  {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args); 
  }
}

void warn(const char *file,int line,const char *fmt, ...)
{
  if (Config::warningFlag)
  {
    if (file==0) file="<unknwon>";
    char text[4096];
    va_list args;
    va_start(args, fmt);
    vsprintf(text, fmt, args);
    va_end(args); 
    switch(warnFormatOrder)
    {
      case 1: fprintf(stderr,outputFormat,file,line,text); break;
      case 2: fprintf(stderr,outputFormat,text,line,file); break;
      case 3: fprintf(stderr,outputFormat,line,text,file); break;
      case 4: fprintf(stderr,outputFormat,file,text,line); break;
      case 5: fprintf(stderr,outputFormat,text,file,line); break;
      case 6: fprintf(stderr,outputFormat,line,file,text); break;
      default:
        printf("Error: warning format has not been initialized!\n");
    }
  }
}

void warn_cont(const char *fmt, ...)
{
  if (Config::warningFlag)
  {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args); 
  }
}
  
void warn_undoc(const char *file,int line,const char *fmt, ...)
{
  if (Config::warningUndocFlag)
  {
    if (file==0) file="<unknwon>";
    char text[4096];
    va_list args;
    va_start(args, fmt);
    vsprintf(text, fmt, args);
    va_end(args); 
    switch(warnFormatOrder)
    {
      case 1: fprintf(stderr,outputFormat,file,line,text); break;
      case 2: fprintf(stderr,outputFormat,text,line,file); break;
      case 3: fprintf(stderr,outputFormat,line,text,file); break;
      case 4: fprintf(stderr,outputFormat,file,text,line); break;
      case 5: fprintf(stderr,outputFormat,text,file,line); break;
      case 6: fprintf(stderr,outputFormat,line,file,text); break;
      default:
        printf("Error: warning format has not been initialized!\n");
    }
  }
}
  
void err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args); 
}
