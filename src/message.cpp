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
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include <stdio.h>
#include "config.h"
#include "debug.h"
#include "portable.h"
#include "message.h"
#include "doxygen.h"

#include <mutex>

static QCString outputFormat;
static const char *warning_str = "warning: ";
static const char *error_str = "error: ";

static FILE *warnFile = stderr;

enum warn_as_error
{
   WARN_NO,
   WARN_YES,
   FAIL_ON_WARNINGS,
};
static warn_as_error warnBehavior = WARN_NO;
static bool warnStat = false;

static std::mutex g_mutex;

void initWarningFormat()
{
  outputFormat = Config_getString(WARN_FORMAT);

  if (!Config_getString(WARN_LOGFILE).isEmpty())
  {
    warnFile = Portable::fopen(Config_getString(WARN_LOGFILE),"w");
  }
  if (!warnFile) // point it to something valid, because warn() relies on it
  {
    warnFile = stderr;
  }

  QCString warnStr = Config_getEnum(WARN_AS_ERROR).upper();
  if (warnStr =="NO") warnBehavior=WARN_NO;
  else if (warnStr =="YES") warnBehavior=WARN_YES;
  else if (warnStr =="FAIL_ON_WARNINGS") warnBehavior=FAIL_ON_WARNINGS;
  if (warnBehavior == WARN_YES)
  {
    warning_str = error_str;
  }
}


void msg(const char *fmt, ...)
{
  if (!Config_getBool(QUIET))
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    if (Debug::isFlagSet(Debug::Time))
    {
      printf("%.3f sec: ",((double)Debug::elapsedTime()));
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
  // substitute markers by actual values
  QCString msgText =
      substitute(
        substitute(
          substitute(
            substitute(
              outputFormat,
              "$file",fileSubst
            ),
            "$line",lineSubst
          ),
          "$version",versionSubst
        ),
        "$text",textSubst
      );
  if (warnBehavior == WARN_YES)
  {
    msgText += " (warning treated as error, aborting now)";
  }
  msgText += '\n';

  {
    std::unique_lock<std::mutex> lock(g_mutex);
    // print resulting message
    fwrite(msgText.data(),1,msgText.length(),warnFile);
  }
  if (warnBehavior == WARN_YES)
  {
    exit(1);
  }
  warnStat = true;
}

static void handle_warn_as_error()
{
  if (warnBehavior == WARN_YES)
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    QCString msgText = " (warning treated as error, aborting now)\n";
    fwrite(msgText.data(),1,msgText.length(),warnFile);
    exit(1);
  }
  warnStat = true;
}

static void do_warn(bool enabled, const char *file, int line, const char *prefix, const char *fmt, va_list args)
{
  if (!enabled) return; // warning type disabled

  va_list argsCopy;
  va_copy(argsCopy, args);

  int l=0;
  if (prefix)
  {
    l=(int)strlen(prefix);
  }
  // determine needed buffersize based on:
  // format + arguments
  // prefix
  // 1 position for `\0`
  int bufSize = vsnprintf(NULL, 0, fmt, args) + l + 1;
  char *text = (char *)malloc(sizeof(char) * bufSize);
  if (prefix)
  {
    qstrncpy(text,prefix,bufSize);
  }
  vsnprintf(text+l, bufSize-l, fmt, argsCopy);
  text[bufSize-1]='\0';
  format_warn(file,line,text);
  free(text);
}

void warn(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARNINGS), file, line, warning_str, fmt, args);
  va_end(args);
}

void va_warn(const char *file,int line,const char *fmt,va_list args)
{
  do_warn(Config_getBool(WARNINGS), file, line, warning_str, fmt, args);
}

void warn_simple(const char *file,int line,const char *text)
{
  if (!Config_getBool(WARNINGS)) return; // warning type disabled
  format_warn(file,line,QCString(warning_str) + text);
}

void warn_undoc(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARN_IF_UNDOCUMENTED), file, line, warning_str, fmt, args);
  va_end(args);
}

void warn_incomplete_doc(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARN_IF_INCOMPLETE_DOC), file, line, warning_str, fmt, args);
  va_end(args);
}

void warn_doc_error(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARN_IF_DOC_ERROR), file, line, warning_str, fmt, args);
  va_end(args);
}

void warn_uncond(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(warnFile, (QCString(warning_str) + fmt).data(), args);
  va_end(args);
  handle_warn_as_error();
}

void err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(warnFile, (QCString(error_str) + fmt).data(), args);
  va_end(args);
  handle_warn_as_error();
}

extern void err_full(const char *file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(TRUE, file, line, error_str, fmt, args);
  va_end(args);
}

void term(const char *fmt, ...)
{
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    va_list args;
    va_start(args, fmt);
    vfprintf(warnFile, (QCString(error_str) + fmt).data(), args);
    va_end(args);
    if (warnFile != stderr)
    {
      for (int i = 0; i < (int)strlen(error_str); i++) fprintf(warnFile, " ");
      fprintf(warnFile, "%s\n", "Exiting...");
    }
  }
  exit(1);
}

void warn_flush()
{
  fflush(warnFile);
}


void printlex(int dbg, bool enter, const char *lexName, const char *fileName)
{
  const char *enter_txt = "entering";
  const char *enter_txt_uc = "Entering";

  if (!enter)
  {
    enter_txt = "finished";
    enter_txt_uc = "Finished";
  }

  std::unique_lock<std::mutex> lock(g_mutex);
  if (dbg)
  {
    if (fileName)
      fprintf(stderr,"--%s lexical analyzer: %s (for: %s)\n",enter_txt, qPrint(lexName), qPrint(fileName));
    else
      fprintf(stderr,"--%s lexical analyzer: %s\n",enter_txt, qPrint(lexName));
  }
  else
  {
    if (fileName)
      Debug::print(Debug::Lex,0,"%s lexical analyzer: %s (for: %s)\n",enter_txt_uc, qPrint(lexName), qPrint(fileName));
    else
      Debug::print(Debug::Lex,0,"%s lexical analyzer: %s\n",enter_txt_uc, qPrint(lexName));
  }
}

extern void finishWarnExit()
{
  if (warnStat && warnBehavior == FAIL_ON_WARNINGS)
  {
    exit(1);
  }
}
