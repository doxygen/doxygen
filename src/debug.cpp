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
  { "preprocessor",       Debug::Preprocessor       },
  { "nolineno",           Debug::NoLineNo           },
  { "commentcnv",         Debug::CommentCnv         },
  { "commentscan",        Debug::CommentScan        },
  { "formula",            Debug::Formula            },
  { "printtree",          Debug::PrintTree          },
  { "time",               Debug::Time               },
  { "extcmd",             Debug::ExtCmd             },
  { "markdown",           Debug::Markdown           },
  { "filteroutput",       Debug::FilterOutput       },
  { "plantuml",           Debug::Plantuml           },
  { "fortranfixed2free",  Debug::FortranFixed2Free  },
  { "cite",               Debug::Cite               },
  { "rtf",                Debug::Rtf                },
  { "qhp",                Debug::Qhp                },
  { "tag",                Debug::Tag                },
  { "alias",              Debug::Alias              },
  { "entries",            Debug::Entries            },
  { "sections",           Debug::Sections           },
  { "stderr",             Debug::Stderr             },
  { "layout",             Debug::Layout             },
  { "lex",                Debug::Lex                },
  { "lex:code",           Debug::Lex_code           },
  { "lex:commentcnv",     Debug::Lex_commentcnv     },
  { "lex:commentscan",    Debug::Lex_commentscan    },
  { "lex:configimpl",     Debug::Lex_configimpl     },
  { "lex:constexp",       Debug::Lex_constexp       },
  { "lex:declinfo",       Debug::Lex_declinfo       },
  { "lex:defargs",        Debug::Lex_defargs        },
  { "lex:doctokenizer",   Debug::Lex_doctokenizer   },
  { "lex:fortrancode",    Debug::Lex_fortrancode    },
  { "lex:fortranscanner", Debug::Lex_fortranscanner },
  { "lex:lexcode",        Debug::Lex_lexcode        },
  { "lex:lexscanner",     Debug::Lex_lexscanner     },
  { "lex:pre",            Debug::Lex_pre            },
  { "lex:pycode",         Debug::Lex_pycode         },
  { "lex:pyscanner",      Debug::Lex_pyscanner      },
  { "lex:scanner",        Debug::Lex_scanner        },
  { "lex:sqlcode",        Debug::Lex_sqlcode        },
  { "lex:vhdlcode",       Debug::Lex_vhdlcode       },
  { "lex:xml",            Debug::Lex_xml            },
  { "lex:xmlcode",        Debug::Lex_xmlcode        },
};

//------------------------------------------------------------------------
static FILE *g_debugFile = stdout;

Debug::DebugMask Debug::curMask = Debug::Quiet;
int Debug::curPrio = 0;

void Debug::print_(DebugMask mask, int prio, fmt::string_view fmt, fmt::format_args args)
{
  if ((curMask&mask) && prio<=curPrio)
  {
    fmt::print(g_debugFile,"{}",fmt::vformat(fmt,args));
  }
}

static char asciiToLower(char in) {
  if (in <= 'Z' && in >= 'A')
    return in - 'A' + 'a';
  return in;
}

static uint64_t labelToEnumValue(const QCString &l)
{
  std::string s = l.str();
  std::transform(s.begin(),s.end(),s.begin(),asciiToLower);
  auto it = s_labels.find(s);
  return (it!=s_labels.end()) ? it->second : Debug::DebugMask::Quiet;
}

bool Debug::setFlagStr(const QCString &lab)
{
  uint64_t retVal = labelToEnumValue(lab);
  if (retVal == Debug::Stderr)
  {
    g_debugFile = stderr;
  }
  else
  {
    curMask = static_cast<DebugMask>(curMask | retVal);
  }
  return retVal!=0;
}

void Debug::setFlag(const DebugMask mask)
{
  curMask = static_cast<DebugMask>(curMask | mask);
}

void Debug::clearFlag(const DebugMask mask)
{
  curMask = static_cast<DebugMask>(curMask & ~mask);
}

void Debug::setPriority(int p)
{
  curPrio = p;
}

bool Debug::isFlagSet(const DebugMask mask)
{
  return (curMask & mask)!=0;
}

void Debug::printFlags()
{
  for (const auto &v : s_labels)
  {
     msg("\t{}\n",v.first);
  }
}

//------------------------------------------------------------------------
DebugLex::DebugLex(Debug::DebugMask mask,const char *lexName,const char *fileName) : m_mask(mask), m_lexName(lexName), m_fileName(fileName)
{
  print(m_mask,"Entering",qPrint(m_lexName),qPrint(m_fileName));
}

DebugLex::~DebugLex()
{
  print(m_mask,"Finished",qPrint(m_lexName),qPrint(m_fileName));
}

void DebugLex::print(Debug::DebugMask mask,
                     const char *state,
                     const char *lexName,
                     const char *fileName)
{
  if (!Debug::isFlagSet(mask)) return;

  if (fileName && *fileName)
  {
    fprintf(stderr, "%s lexical analyzer: %s (for: %s)\n", state, lexName, fileName);
  }
  else
  {
    fprintf(stderr, "%s lexical analyzer: %s\n", state, lexName);
  }
}

//------------------------------------------------------------------------

class Timer
{
  public:
    void start()
    {
      m_startTime = std::chrono::steady_clock::now();
    }
    double elapsedTimeS()
    {
      return static_cast<double>(
              std::chrono::duration_cast<
                  std::chrono::microseconds>(
                  std::chrono::steady_clock::now() - m_startTime).count()) / 1000000.0;
    }
  private:
    std::chrono::time_point<std::chrono::steady_clock> m_startTime;
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

