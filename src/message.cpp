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
#include <unordered_set>

#include "config.h"
#include "debug.h"
#include "portable.h"
#include "message.h"
#include "doxygen.h"
#include "fileinfo.h"
#include "dir.h"
#include "md5.h"

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
static std::unordered_set<std::string> g_warnHash;

//-----------------------------------------------------------------------------------------

static bool checkWarnMessage(QCString result)
{
  uint8_t md5_sig[16];
  char sigStr[33];
  MD5Buffer(result.data(),result.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr);

  return g_warnHash.insert(sigStr).second;
}

static void format_warn(const QCString &file,int line,const QCString &text)
{
  QCString fileSubst = file.isEmpty() ? "<unknown>" : file;
  QCString lineSubst; lineSubst.setNum(line);
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
        "$text",text
      );
  if (g_warnBehavior == WARN_AS_ERROR_t::YES)
  {
    msgText += " (warning treated as error, aborting now)";
  }
  msgText += '\n';

  {
    std::unique_lock<std::mutex> lock(g_mutex);
    // print resulting message
    if (checkWarnMessage(msgText)) fwrite(msgText.data(),1,msgText.length(),g_warnFile);
  }
  if (g_warnBehavior == WARN_AS_ERROR_t::YES)
  {
    if (g_warnFile != stderr && !Config_getBool(QUIET))
    {
      msg("See '{}' for the reason of termination.\n",g_warnlogFile);
    }
    exit(1);
  }
  g_warnStat = true;
}

//-----------------------------------------------------------------------------------------

static void handle_warn_as_error()
{
  if (g_warnBehavior == WARN_AS_ERROR_t::YES)
  {
    {
      std::unique_lock<std::mutex> lock(g_mutex);
      QCString msgText = " (warning treated as error, aborting now)\n";
      fwrite(msgText.data(),1,msgText.length(),g_warnFile);
      if (g_warnFile != stderr && !Config_getBool(QUIET))
      {
        // cannot use `msg` due to the mutex
        fprintf(stdout,"See '%s' for the reason of termination.\n",qPrint(g_warnlogFile));
      }
    }
    exit(1);
  }
  g_warnStat = true;
}

//-----------------------------------------------------------------------------------------

static void do_warn(const QCString &file, int line, const char *prefix, fmt::string_view fmt, fmt::format_args args)
{
  format_warn(file,line,prefix+fmt::vformat(fmt,args));
  handle_warn_as_error();
}

//-----------------------------------------------------------------------------------------

void msg_(fmt::string_view fmt, fmt::format_args args)
{
  if (!Config_getBool(QUIET))
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    if (Debug::isFlagSet(Debug::Time))
    {
      fmt::print("{:.3f} sec: ",(static_cast<double>(Debug::elapsedTime())));
    }
    fmt::print("{}",fmt::vformat(fmt,args));
  }
}

//-----------------------------------------------------------------------------------------

void warn_(WarningType type, const QCString &file, int line, fmt::string_view fmt, fmt::format_args args)
{
  bool enabled = false;
  switch (type)
  {
    case WarningType::Generic:       enabled = Config_getBool(WARNINGS);               break;
    case WarningType::Undocumented:  enabled = Config_getBool(WARN_IF_UNDOCUMENTED);   break;
    case WarningType::IncompleteDoc: enabled = Config_getBool(WARN_IF_INCOMPLETE_DOC); break;
    case WarningType::DocError:      enabled = Config_getBool(WARN_IF_DOC_ERROR);      break;
    case WarningType::Layout:        enabled = Config_getBool(WARN_LAYOUT_FILE);       break;
  }
  if (enabled)
  {
    do_warn(file, line, g_warningStr, fmt, args);
  }
}

//-----------------------------------------------------------------------------------------

void warn_uncond_(fmt::string_view fmt, fmt::format_args args)
{
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    if (checkWarnMessage(g_errorStr+fmt::vformat(fmt,args))) fmt::print(g_warnFile,"{}{}",g_warningStr,vformat(fmt,args));
  }
  handle_warn_as_error();
}

//-----------------------------------------------------------------------------------------

void err_(fmt::string_view fmt, fmt::format_args args)
{
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    if (checkWarnMessage(g_errorStr+fmt::vformat(fmt,args))) fmt::print(g_warnFile,"{}{}",g_errorStr,fmt::vformat(fmt,args));
  }
  handle_warn_as_error();
}

//-----------------------------------------------------------------------------------------

void err_full_(const QCString &file, int line, fmt::string_view fmt, fmt::format_args args)
{
  format_warn(file,line,g_errorStr+fmt::vformat(fmt,args));
}

//-----------------------------------------------------------------------------------------

void term_(fmt::string_view fmt, fmt::format_args args)
{
  {
    std::unique_lock<std::mutex> lock(g_mutex);
    if (checkWarnMessage(g_errorStr+fmt::vformat(fmt,args))) fmt::print(g_warnFile, "{}{}", g_errorStr, fmt::vformat(fmt,args));
    if (g_warnFile != stderr)
    {
      size_t l = strlen(g_errorStr);
      for (size_t i=0; i<l; i++) fmt::print(g_warnFile, " ");
      fmt::print(g_warnFile, "{}\n", "Exiting...");
      if (!Config_getBool(QUIET))
      {
        // cannot use `msg` due to the mutex
        fmt::print("See '{}' for the reason of termination.\n",g_warnlogFile);
      }
    }
  }
  exit(1);
}

//-----------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------

void warn_flush()
{
  fflush(g_warnFile);
}

//-----------------------------------------------------------------------------------------

void initWarningFormat()
{
  g_warnFormat     = Config_getString(WARN_FORMAT);
  g_warnLineFormat = Config_getString(WARN_LINE_FORMAT);
  g_warnBehavior   = Config_getEnum(WARN_AS_ERROR);
  g_warnlogFile    = Config_getString(WARN_LOGFILE);
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
      Dir d(fi.dirPath());
      if (!d.exists() && !d.mkdir(fi.dirPath()))
      {
        // point it to something valid, because warn() relies on it
        g_warnFile = stderr;
        err("Cannot create directory for '{}', redirecting 'WARN_LOGFILE' output to 'stderr'\n",g_warnlogFile);
      }
      else if (!(g_warnFile = Portable::fopen(g_warnlogFile,"w")))
      {
        // point it to something valid, because warn() relies on it
        g_warnFile = stderr;
        err("Cannot open '{}' for writing, redirecting 'WARN_LOGFILE' output to 'stderr'\n",g_warnlogFile);
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

//-----------------------------------------------------------------------------------------

void finishWarnExit()
{
  fflush(stdout);
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
      err("Cannot open warnings file '{}' for reading\n",g_warnlogFile);
    }
    else
    {
      std::string line;
      while (getline(warnFile,line))
      {
        fmt::print(stderr,"{}\n",line);
      }
      warnFile.close();
    }
  }

  if (g_warnlogTemp) Portable::unlink(g_warnlogFile);

  if (g_warnStat && (g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS ||
                     g_warnBehavior == WARN_AS_ERROR_t::FAIL_ON_WARNINGS_PRINT))
  {
    exit(1);
  }
}
