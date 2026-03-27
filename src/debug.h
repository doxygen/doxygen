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

#ifndef DEBUG_H
#define DEBUG_H

#include <cstdint>
#include "qcstring.h"
#include "construct.h"
#include "message.h"

/** Class containing a print function for diagnostics. */
class Debug
{
  public:
    enum DebugMask : uint64_t {
                     Quiet             =             0x00'0000ULL,
                     Preprocessor      =             0x00'0001ULL,
                     CommentCnv        =             0x00'0002ULL,
                     CommentScan       =             0x00'0004ULL,
                     Formula           =             0x00'0008ULL,
                     PrintTree         =             0x00'0010ULL,
                     Time              =             0x00'0020ULL,
                     ExtCmd            =             0x00'0040ULL,
                     Markdown          =             0x00'0080ULL,
                     FilterOutput      =             0x00'0100ULL,
                     Plantuml          =             0x00'0200ULL,
                     FortranFixed2Free =             0x00'0400ULL,
                     Cite              =             0x00'0800ULL,
                     NoLineNo          =             0x00'1000ULL,
                     Rtf               =             0x00'2000ULL,
                     Qhp               =             0x00'4000ULL,
                     Tag               =             0x00'8000ULL,
                     Alias             =             0x01'0000ULL,
                     Entries           =             0x02'0000ULL,
                     Sections          =             0x04'0000ULL,
                     Stderr            =             0x08'0000ULL,
                     Layout            =             0x10'0000ULL,
                     Lex               = 0x0000'FFFF'FF00'0000ULL, // all scanners combined
                     Lex_code          = 0x0000'0000'0100'0000ULL,
                     Lex_commentcnv    = 0x0000'0000'0200'0000ULL,
                     Lex_commentscan   = 0x0000'0000'0400'0000ULL,
                     Lex_configimpl    = 0x0000'0000'0800'0000ULL,
                     Lex_constexp      = 0x0000'0000'1000'0000ULL,
                     Lex_declinfo      = 0x0000'0000'2000'0000ULL,
                     Lex_defargs       = 0x0000'0000'4000'0000ULL,
                     Lex_doctokenizer  = 0x0000'0000'8000'0000ULL,
                     Lex_fortrancode   = 0x0000'0001'0000'0000ULL,
                     Lex_fortranscanner= 0x0000'0002'0000'0000ULL,
                     Lex_lexcode       = 0x0000'0004'0000'0000ULL,
                     Lex_lexscanner    = 0x0000'0008'0000'0000ULL,
                     Lex_pre           = 0x0000'0010'0000'0000ULL,
                     Lex_pycode        = 0x0000'0020'0000'0000ULL,
                     Lex_pyscanner     = 0x0000'0040'0000'0000ULL,
                     Lex_scanner       = 0x0000'0080'0000'0000ULL,
                     Lex_sqlcode       = 0x0000'0100'0000'0000ULL,
                     Lex_vhdlcode      = 0x0000'0200'0000'0000ULL,
                     Lex_xml           = 0x0000'0400'0000'0000ULL,
                     Lex_xmlcode       = 0x0000'0800'0000'0000ULL
                   };
    static void print_(DebugMask mask, int prio, fmt::string_view fmt, fmt::format_args args);

    template<typename ...Args>
    static void print(DebugMask mask,int prio,fmt::format_string<Args...> fmt, Args&&... args)
    {
      print_(mask,prio,fmt,fmt::make_format_args(args...));
    }

    static bool setFlagStr(const QCString &label);
    static void setFlag(const DebugMask mask);
    static void clearFlag(const DebugMask mask);
    static bool isFlagSet(const DebugMask mask);
    static void printFlags();
    static void setPriority(int p);

    static void startTimer();
    static double elapsedTime();

  private:
    static DebugMask curMask;
    static int curPrio;
};

class DebugLex
{
  public:
    DebugLex(Debug::DebugMask mask,const char *lexName,const char *fileName);
   ~DebugLex();
    NON_COPYABLE(DebugLex)
    static void print(Debug::DebugMask mask,const char *state,const char *lexName,const char *fileName);
  private:

    Debug::DebugMask m_mask;
    QCString m_lexName;
    QCString m_fileName;
};


#endif
