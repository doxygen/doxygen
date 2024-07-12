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

#include "trace.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/stdout_sinks.h"

std::shared_ptr<spdlog::logger> g_tracer;

void initTracing(const QCString &logFile, bool timing)
{
  if (!logFile.isEmpty())
  {
    std::vector<spdlog::sink_ptr> sinks;
    if (logFile=="stdout")
    {
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
    }
    else if (logFile=="stderr")
    {
      sinks.push_back(std::make_shared<spdlog::sinks::stderr_sink_mt>());
    }
    else // normal file
    {
      sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile.str(),true));
    }
    g_tracer = std::make_shared<spdlog::logger>("tracing", sinks.begin(),sinks.end());
    g_tracer->set_level(spdlog::level::trace);
    if (timing)
    {
      g_tracer->set_pattern("[%C-%m-%d %T.%e][%t][%s:%#](%!) %v");
    }
    else
    {
      g_tracer->set_pattern("[%s:%#](%!) %v");
    }
  }
}

void exitTracing()
{
  if (g_tracer)
  {
    spdlog::shutdown();
  }
}

