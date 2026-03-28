/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#ifndef TRACE_H
#define TRACE_H

#ifndef ENABLE_TRACING
#ifndef NDEBUG
#define ENABLE_TRACING 1
#else
#define ENABLE_TRACING 0
#endif
#endif

// Since some modules produce a huge amount of tracing we disable those traces by default.
// Set of or more of the following to 1 to enable the relevant tracing
#define ENABLE_SYMBOLRESOLVER_TRACING 0
#define ENABLE_MARKDOWN_TRACING       0
#define ENABLE_DOCPARSER_TRACING      0


#if ENABLE_TRACING
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // debug build
#else
#define SPELOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO  // release build (hide trace/debug levels)
#endif

#pragma push_macro("warn")
#undef warn
#include "spdlog/spdlog.h"
#pragma pop_macro("warn")

#include "message.h"
#include "qcstring.h"
#include "construct.h"

extern std::shared_ptr<spdlog::logger> g_tracer;

void initTracing(const QCString &logFile, bool timing);
void exitTracing();

namespace Trace
{
  inline QCString trunc(const QCString &s,size_t numChars=15)
  {
    QCString result;
    size_t i=0;
    for (; i<numChars && i<s.length(); i++)
    {
      char c=s.at(i);
      if      (c=='\n') { result+="\\n"; }
      else if (c=='\t') { result+="\\t"; }
      else if (c=='\r') { result+="\\r"; }
      else if (c=='\\') { result+="\\\\"; }
      else result+=c;
    }
    if (i<s.length()) result+="...";
    return result;
  }
}

#if ENABLE_TRACING
#define TRACE(...) if (g_tracer) SPDLOG_LOGGER_TRACE(g_tracer,__VA_ARGS__)
#else
#define TRACE(...) (void)0
#endif

/** Helper class to trace an entry statement at creation and another one at destruction. */
class AutoTrace
{
  public:
    explicit AutoTrace(spdlog::source_loc loc) : m_loc(loc)
    {
      if (g_tracer)
      {
        g_tracer->log(m_loc,spdlog::level::trace,">");
      }
    }
    template<typename... Args>
    explicit AutoTrace(spdlog::source_loc loc,
                       const std::string &fmt, Args&&...args) : m_loc(loc)
    {
      if (g_tracer)
      {
        if (fmt.empty())
        {
          g_tracer->log(m_loc,spdlog::level::trace,">");
        }
        else
        {
          g_tracer->log(m_loc,spdlog::level::trace,fmt::runtime("> "+fmt),std::forward<Args>(args)...);
        }
      }
    }
   ~AutoTrace()
    {
      if (g_tracer)
      {
        if (m_exitMessage.empty())
        {
          g_tracer->log(m_loc,spdlog::level::trace,"<");
        }
        else
        {
          g_tracer->log(m_loc,spdlog::level::trace,"< "+m_exitMessage);
        }
      }
    }
    NON_COPYABLE(AutoTrace)

    template<typename... Args>
    void add(spdlog::source_loc loc,
             const std::string &fmt, Args&&...args)
    {
      if (g_tracer)
      {
        g_tracer->log(loc,spdlog::level::trace,fmt::runtime(": "+fmt),std::forward<Args>(args)...);
      }
    }
    template<typename... Args>
    void setExit(spdlog::source_loc loc,
                 const std::string &msg,Args&&...args)
    {
      m_loc = loc;
      m_exitMessage = fmt::format(fmt::runtime(msg),std::forward<Args>(args)...);
    }
  private:
   spdlog::source_loc m_loc;
   std::string m_exitMessage;
};

#if ENABLE_TRACING
#define AUTO_TRACE(...)      AutoTrace trace_{spdlog::source_loc{__FILE__,__LINE__,SPDLOG_FUNCTION},__VA_ARGS__}
#define AUTO_TRACE_ADD(...)  trace_.add(spdlog::source_loc{__FILE__,__LINE__,SPDLOG_FUNCTION},__VA_ARGS__)
#define AUTO_TRACE_EXIT(...) trace_.setExit(spdlog::source_loc{__FILE__,__LINE__,SPDLOG_FUNCTION},__VA_ARGS__)
#else
#define AUTO_TRACE(...)      (void)0
#define AUTO_TRACE_ADD(...)  (void)0
#define AUTO_TRACE_EXIT(...) (void)0
#endif


#endif // TRACE_H
