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

/** Class containing a print function for diagnostics. */
class Debug
{
  public:
    enum DebugMask : uint64_t {
                     Quiet             = 0x0000,
                     Preprocessor      = 0x0001,
                     CommentCnv        = 0x0002,
                     CommentScan       = 0x0004,
                     PrintTree         = 0x0010,
                     Time              = 0x0020,
                     ExtCmd            = 0x0040,
                     Markdown          = 0x0080,
                     FilterOutput      = 0x0100,
                     Plantuml          = 0x0200,
                     FortranFixed2Free = 0x0400,
                     Cite              = 0x0800,
                     NoLineNo          = 0x1000,
                     Rtf               = 0x2000,
                     Qhp               = 0x4000,
                     Tag               = 0x8000,
                     Lex               = 0xFFFFFF0000ULL, // all scanners combined
                     Lex_code          = 0x0000010000ULL,
                     Lex_commentcnv    = 0x0000020000ULL,
                     Lex_commentscan   = 0x0000040000ULL,
                     Lex_configimpl    = 0x0000080000ULL,
                     Lex_constexp      = 0x0000100000ULL,
                     Lex_declinfo      = 0x0000200000ULL,
                     Lex_defargs       = 0x0000400000ULL,
                     Lex_doctokenizer  = 0x0000800000ULL,
                     Lex_fortrancode   = 0x0001000000ULL,
                     Lex_fortranscanner= 0x0002000000ULL,
                     Lex_lexcode       = 0x0004000000ULL,
                     Lex_lexscanner    = 0x0008000000ULL,
                     Lex_pre           = 0x0010000000ULL,
                     Lex_pycode        = 0x0020000000ULL,
                     Lex_pyscanner     = 0x0040000000ULL,
                     Lex_scanner       = 0x0080000000ULL,
                     Lex_sqlcode       = 0x0100000000ULL,
                     Lex_vhdlcode      = 0x0200000000ULL,
                     Lex_xml           = 0x0400000000ULL,
                     Lex_xmlcode       = 0x0800000000ULL
                   };
    static void print(DebugMask mask,int prio,const char *fmt,...);

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
    static void print(Debug::DebugMask mask,const char *state,const char *lexName,const char *fileName);
  private:

    Debug::DebugMask m_mask;
    QCString m_lexName;
    QCString m_fileName;
};


#endif
