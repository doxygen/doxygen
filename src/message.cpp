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

// globals
static QCString        g_warnFormat;
static const char *    g_warningStr = "warning: ";
static const char *    g_errorStr = "error: ";
static FILE *          g_warnFile = stderr;
static WARN_AS_ERROR_t g_warnBehavior = WARN_AS_ERROR_t::NO;
static bool            g_warnStat = false;
static std::mutex      g_mutex;

void initWarningFormat()
{
  g_warnFormat = Config_getString(WARN_FORMAT);
  QCString logFile = Config_getString(WARN_LOGFILE);

  if (!logFile.isEmpty())
  {
    if (logFile == "-")
    {
      g_warnFile = stdout;
    }
    else if (!(g_warnFile = Portable::fopen(logFile,"w")))
    {
      // point it to something valid, because warn() relies on it
      g_warnFile = stderr;
      err("Cannot open '%s' for writing, redirecting 'WARN_LOGFILE' output to 'stderr'\n",logFile.data());
    }
  }
  else
  {
    g_warnFile = stderr;
  }
  g_warnBehavior = Config_getEnum(WARN_AS_ERROR);
  if (g_warnBehavior != WARN_AS_ERROR_t::NO)
  {
    g_warningStr = g_errorStr;
  }
}


void msg(const char *fmt, ...)
{
  if (!Config_getBool(QUIET))
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    if (Debug::isFlagSet(Debug::Time))
    {
      printf("%.3f sec: ",(static_cast<double>(Debug::elapsedTime())));
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
}

static void format_warn(const QCString &file,int line,const QCString &text)
{
  QCString fileSubst = file.isEmpty() ? "<unknown>" : file;
  QCString lineSubst; lineSubst.setNum(line);
  QCString textSubst = text;
  QCString versionSubst;
  // substitute markers by actual values
  QCString msgText =
      substitute(
        substitute(
          substitute(
            substitute(
              g_warnFormat,
              "$file",fileSubst
            ),
            "$line",lineSubst
          ),
          "$version",versionSubst
        ),
        "$text",textSubst
      );
  if (g_warnBehavior == WARN_AS_ERROR_t::YES)
  {
    msgText += " (warning treated as error, aborting now)";
  }
  msgText += '\n';

  {
    std::unique_lock<std::mutex> lock(g_mutex);
    // print resulting message
    fwrite(msgText.data(),1,msgText.length(),g_warnFile);
  }
  if (g_warnBehavior == WARN_AS_ERROR_t::YES)
  {
    exit(1);
  }
  g_warnStat = true;
}

static void handle_warn_as_error()
{
  if (g_warnBehavior == WARN_AS_ERROR_t::YES)
  {
    {
      std::unique_lock<std::mutex> lock(g_mutex);
      QCString msgText = " (warning treated as error, aborting now)\n";
      fwrite(msgText.data(),1,msgText.length(),g_warnFile);
    }
    exit(1);
  }
  g_warnStat = true;
}

static void do_warn(bool enabled, const QCString &file, int line, const char *prefix, const char *fmt, va_list args)
{
  if (!enabled) return; // warning type disabled

  va_list argsCopy;
  va_copy(argsCopy, args);

  size_t l=0;
  if (prefix)
  {
    l=strlen(prefix);
  }
  // determine needed buffersize based on:
  // format + arguments
  // prefix
  // 1 position for `\0`
  size_t bufSize = vsnprintf(NULL, 0, fmt, args) + l + 1;
  QCString text(bufSize);
  if (prefix)
  {
    qstrncpy(text.rawData(),prefix,bufSize);
  }
  vsnprintf(text.rawData()+l, bufSize-l, fmt, argsCopy);
  text[static_cast<int>(bufSize)-1]='\0';
  format_warn(file,line,text);

  va_end(argsCopy);
}

void warn(const QCString &file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARNINGS), file, line, g_warningStr, fmt, args);
  va_end(args);
}

void va_warn(const QCString &file,int line,const char *fmt,va_list args)
{
  do_warn(Config_getBool(WARNINGS), file, line, g_warningStr, fmt, args);
}

void warn_simple(const QCString &file,int line,const char *text)
{
  if (!Config_getBool(WARNINGS)) return; // warning type disabled
  format_warn(file,line,QCString(g_warningStr) + text);
}

void warn_undoc(const QCString &file,int line,const char *fmt, ...)
{
  static bool warnIfUndocumented = Config_getBool(WARN_IF_UNDOCUMENTED);
  static bool extractAll = Config_getBool(EXTRACT_ALL);
  static bool warnExtractAll = Config_getBool(WARN_EXTRACT_ALL);
  va_list args;
  va_start(args, fmt);
  do_warn(warnIfUndocumented || (extractAll && warnExtractAll), file, line, g_warningStr, fmt, args);
  va_end(args);
}

void warn_incomplete_doc(const QCString &file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARN_IF_INCOMPLETE_DOC), file, line, g_warningStr, fmt, args);
  va_end(args);
}

void warn_doc_error(const QCString &file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARN_IF_DOC_ERROR), file, line, g_warningStr, fmt, args);
  va_end(args);
}

void warn_uncond(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(g_warnFile, (QCString(g_warningStr) + fmt).data(), args);
  va_end(args);
  handle_warn_as_error();
}

void err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(g_warnFile, (QCString(g_errorStr) + fmt).data(), args);
  va_end(args);
  handle_warn_as_error();
}

extern void err_full(const QCString &file,int line,const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  do_warn(TRUE, file, line, g_errorStr, fmt, args);
  va_end(args);
}

void term(const char *fmt, ...)
{
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    va_list args;
    va_start(args, fmt);
    vfprintf(g_warnFile, (QCString(g_errorStr) + fmt).data(), args);
    va_end(args);
    if (g_warnFile != stderr)
    {
      size_t l = strlen(g_errorStr);
      for (size_t i=0; i<l; i++) fprintf(g_warnFile, " ");
      fprintf(g_warnFile, "%s\n", "Exiting...");
    }
  }
  exit(1);
}

void warn_flush()
{
  fflush(g_warnFile);
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
  if (g_warnStat && g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS)
  {
    exit(1);
  }
}
