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

#include <stdarg.h>
#include <algorithm>
#include <stdio.h>
#include <map>
#include <string>
#include <chrono>

#include "debug.h"
#include "message.h"
#include "qcstring.h"

//------------------------------------------------------------------------

static std::map< std::string, Debug::DebugMask > s_labels =
{
  { "findmembers",       Debug::FindMembers       },
  { "functions",         Debug::Functions         },
  { "variables",         Debug::Variables         },
  { "preprocessor",      Debug::Preprocessor      },
  { "nolineno",          Debug::NoLineNo          },
  { "classes",           Debug::Classes           },
  { "commentcnv",        Debug::CommentCnv        },
  { "commentscan",       Debug::CommentScan       },
  { "validate",          Debug::Validate          },
  { "printtree",         Debug::PrintTree         },
  { "time",              Debug::Time              },
  { "extcmd",            Debug::ExtCmd            },
  { "markdown",          Debug::Markdown          },
  { "filteroutput",      Debug::FilterOutput      },
  { "lex",               Debug::Lex               },
  { "plantuml",          Debug::Plantuml          },
  { "fortranfixed2free", Debug::FortranFixed2Free },
  { "cite",              Debug::Cite              },
  { "rtf",               Debug::Rtf               },
  { "qhp",               Debug::Qhp               },
  { "tag",               Debug::Tag               },
};

//------------------------------------------------------------------------

Debug::DebugMask Debug::curMask = Debug::Quiet;
int Debug::curPrio = 0;

void Debug::print(DebugMask mask,int prio,const char *fmt,...)
{
  if ((curMask&mask) && prio<=curPrio)
  {
    va_list args;
    va_start(args,fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
}

void Debug::print(const char *fmt,...)
{
  va_list args;
  va_start(args,fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
}

static char asciiToLower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

static int labelToEnumValue(const QCString &l)
{
  std::string s = l.str();
  std::transform(s.begin(),s.end(),s.begin(),asciiToLower);
  auto it = s_labels.find(s);
  return (it!=s_labels.end()) ? it->second : 0;
}

int Debug::setFlag(const QCString &lab)
{
  int retVal = labelToEnumValue(lab);
  curMask = static_cast<DebugMask>(curMask | retVal);
  return retVal;
}

void Debug::clearFlag(const QCString &lab)
{
  curMask = static_cast<DebugMask>(curMask & ~labelToEnumValue(lab));
}

void Debug::setPriority(int p)
{
  curPrio = p;
}

bool Debug::isFlagSet(DebugMask mask)
{
  return (curMask & mask)!=0;
}

void Debug::printFlags()
{
  for (const auto &v : s_labels)
  {
     msg("\t%s\n",v.first.c_str());
  }
}

//------------------------------------------------------------------------

class Timer
{
  public:
    void start()
    {
      m_startTime = std::chrono::system_clock::now();
    }
    double elapsedTimeS()
    {
      return (std::chrono::duration_cast<
                  std::chrono::microseconds>(
                  std::chrono::system_clock::now() - m_startTime).count()) / 1000000.0;
    }
  private:
    std::chrono::time_point<std::chrono::system_clock> m_startTime;
};

static Timer g_runningTime;

void Debug::startTimer()
{
  g_runningTime.start();
}

double Debug::elapsedTime()
{
  return g_runningTime.elapsedTimeS();
}

