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

std::shared_ptr<spdlog::logger> g_tracer;

void initTracing(const QCString &logFile)
{
  if (!logFile.isEmpty())
  {
    g_tracer = spdlog::basic_logger_mt("tracing", logFile.str(),true);
    g_tracer->set_level(spdlog::level::trace);
    g_tracer->set_pattern("[%C-%m-%d %T.%e][%t][%s:%#](%!) %v");
  }
}

void exitTracing()
{
  if (g_tracer)
  {
    spdlog::shutdown();
  }
}

