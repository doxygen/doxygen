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

#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <atomic>

#include "config.h"
#include "debug.h"
#include "portable.h"
#include "message.h"
#include "doxygen.h"
#include "fileinfo.h"
#include "dir.h"

// globals
static QCString        g_warnFormat;
static QCString        g_warnLineFormat;
static const char *    g_warningStr = "warning: ";
static const char *    g_errorStr = "error: ";
static FILE *          g_warnFile = stderr;
static WARN_AS_ERROR_t g_warnBehavior = WARN_AS_ERROR_t::NO;
static QCString        g_warnlogFile;
static bool            g_warnlogTemp = false;
static std::atomic_bool g_warnStat = false;
static std::mutex      g_mutex;

void initWarningFormat()
{
  g_warnFormat = Config_getString(WARN_FORMAT);
  g_warnLineFormat = Config_getString(WARN_LINE_FORMAT);
  g_warnBehavior = Config_getEnum(WARN_AS_ERROR);
  g_warnlogFile = Config_getString(WARN_LOGFILE);
  if (g_warnlogFile.isEmpty() && g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS_PRINT)
  {
    uint32_t pid = Portable::pid();
    g_warnlogFile.sprintf("doxygen_warnings_temp_%d.tmp",pid);
    g_warnlogTemp = true;
  }

  if (!g_warnlogFile.isEmpty())
  {
    if (g_warnlogFile == "-")
    {
      g_warnFile = stdout;
    }
    else
    {
      FileInfo fi(g_warnlogFile.str());
      Dir d(fi.dirPath().c_str());
      if (!d.exists() && !d.mkdir(fi.dirPath().c_str()))
      {
        // point it to something valid, because warn() relies on it
        g_warnFile = stderr;
        err("Cannot create directory for '%s', redirecting 'WARN_LOGFILE' output to 'stderr'\n",g_warnlogFile.data());
      }
      else if (!(g_warnFile = Portable::fopen(g_warnlogFile,"w")))
      {
        // point it to something valid, because warn() relies on it
        g_warnFile = stderr;
        err("Cannot open '%s' for writing, redirecting 'WARN_LOGFILE' output to 'stderr'\n",g_warnlogFile.data());
      }
    }
  }
  else
  {
    g_warnFile = stderr;
  }
  if (g_warnBehavior != WARN_AS_ERROR_t::NO)
  {
    g_warningStr = g_errorStr;
  }

  // make sure the g_warnFile is closed in case we call exit and it is still open
  std::atexit([](){
      if (g_warnFile && g_warnFile!=stderr && g_warnFile!=stdout)
      {
        Portable::fclose(g_warnFile);
        g_warnFile = nullptr;
      }
  });
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
    Doxygen::terminating=true;
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
    Doxygen::terminating=true;
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

QCString warn_line(const QCString &file,int line)
{
  QCString fileSubst = file.isEmpty() ? "<unknown>" : file;
  QCString lineSubst; lineSubst.setNum(line);
  return  substitute(
            substitute(
              g_warnLineFormat,
              "$file",fileSubst
            ),
            "$line",lineSubst
          );
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
  va_list args;
  va_start(args, fmt);
  do_warn(Config_getBool(WARN_IF_UNDOCUMENTED), file, line, g_warningStr, fmt, args);
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
  Doxygen::terminating=true;
  exit(1);
}

void warn_flush()
{
  fflush(g_warnFile);
}



extern void finishWarnExit()
{
  if (g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS_PRINT && g_warnlogFile != "-")
  {
    Portable::fclose(g_warnFile);
    g_warnFile = nullptr;
  }
  if (g_warnStat && g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS_PRINT && g_warnlogFile != "-")
  {

    std::ifstream warnFile = Portable::openInputStream(g_warnlogFile);
    if (!warnFile.is_open())
    {
      g_warnFile = stderr;
      err("Cannot open warnings file '%s' for reading\n",g_warnlogFile.data());
    }
    else
    {
      std::string line;
      while (getline(warnFile,line))
      {
        fprintf(stderr,"%s\n",line.c_str());
      }
      warnFile.close();
    }
  }

  if (g_warnlogTemp) Portable::unlink(g_warnlogFile);

  if (g_warnStat && (g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS ||
                     g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS_PRINT))
  {
    Doxygen::terminating=true;
    exit(1);
  }
}
