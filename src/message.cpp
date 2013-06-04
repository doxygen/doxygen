/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include <qdatetime.h>
#include "config.h"
#include "util.h"
#include "debug.h"
#include "doxygen.h"
#include "portable.h"
#include "filedef.h"

static QCString outputFormat;
static const char *warning_str = "warning: ";
static const char *error_str = "error: ";
//static int warnFormatOrder; // 1 = $file,$line,$text
//                            // 2 = $text,$line,$file
//                            // 3 = $line,$text,$file
//                            // 4 = $file,$text,$line
//                            // 5 = $text,$file,$line
//                            // 6 = $line,$file,$text

static FILE *warnFile = stderr;

void initWarningFormat()
{
//  int filePos = Config_getString("WARN_FORMAT").find("$file");
//  int linePos = Config_getString("WARN_FORMAT").find("$line");
//  int textPos = Config_getString("WARN_FORMAT").find("$text");
//
//  // sort items on position (there are 6 cases)
//  warnFormatOrder = 1;
//  if (filePos>linePos && filePos>textPos)
//  {
//    if (linePos>textPos) // $text,$line,$file
//    {
//      warnFormatOrder = 2;
//    }
//    else                 // $line,$text,$file
//    {
//      warnFormatOrder = 3;
//    }
//  }
//  else if (filePos<linePos && filePos<textPos)
//  {
//    if (linePos>textPos) // $file,$text,$line
//    {
//      warnFormatOrder = 4;
//    }
//  }
//  else if (filePos<linePos && filePos>textPos) // $text,$file,$line
//  {
//    warnFormatOrder = 5;
//  }
//  else // $line,$file,$text
//  {
//    warnFormatOrder = 6;
//  }
//  outputFormat = 
//      substitute(
//        substitute(
//          substitute( 
//            Config_getString("WARN_FORMAT"),
//           "$file","%s"
//          ),
//          "$text","%s"
//        ),
//        "$line","%d"
//      )+'\n';

  //    replace(QRegExp("\\$file"),"%s").
  //    replace(QRegExp("\\$text"),"%s").
  //    replace(QRegExp("\\$line"),"%d")+
  //    '\n';

  outputFormat = Config_getString("WARN_FORMAT");

  if (!Config_getString("WARN_LOGFILE").isEmpty())
  {
    warnFile = portable_fopen(Config_getString("WARN_LOGFILE"),"w");
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
    if (Debug::isFlagSet(Debug::Time))
    {
      printf("%.3f sec: ",((double)Doxygen::runningTime.elapsed())/1000.0);
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args); 
  }
}

static void format_warn(const char *file,int line,const char *text)
{
  QCString fileSubst = file==0 ? "<unknown>" : file;
  QCString lineSubst; lineSubst.setNum(line);
  QCString textSubst = text;
  QCString versionSubst;
  if (file) // get version from file name
  {
    bool ambig;
    FileDef *fd=findFileDef(Doxygen::inputNameDict,file,ambig);
    if (fd)
    {
      versionSubst = fd->getVersion();
    }
  }
  // substitute markers by actual values
  QCString msgText = 
      substitute(
        substitute(
          substitute(
            substitute( 
              outputFormat,
              "$file",fileSubst
            ),
            "$text",textSubst
          ),
          "$line",lineSubst
        ),
        "$version",versionSubst
      )+'\n';

  // print resulting message
  fwrite(msgText.data(),1,msgText.length(),warnFile);
}

static void do_warn(const char *tag, const char *file, int line, const char *prefix, const char *fmt, va_list args)
{
  if (!Config_getBool(tag)) return; // warning type disabled
  char text[4096];
  int l=0;
  if (prefix)
  {
    strcpy(text,prefix);
    l=strlen(prefix);
  }
  vsnprintf(text+l, 4096-l, fmt, args);
  text[4095]='\0';
  format_warn(file,line,text);
}

void warn(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn("WARNINGS", file, line, warning_str, fmt, args);
  va_end(args); 
}

void warn_simple(const char *file,int line,const char *text)
{
  if (!Config_getBool("WARNINGS")) return; // warning type disabled
  format_warn(file,line,QCString(warning_str) + text);
}

void warn_undoc(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn("WARN_IF_UNDOCUMENTED", file, line, warning_str, fmt, args);
  va_end(args);
}
  
void warn_doc_error(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn("WARN_IF_DOC_ERROR", file, line, warning_str, fmt, args);
  va_end(args);
}

void warn_uncond(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(warnFile, (QCString(warning_str) + fmt).data(), args);
  va_end(args); 
}

void err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(warnFile, (QCString(error_str) + fmt).data(), args);
  va_end(args); 
}
