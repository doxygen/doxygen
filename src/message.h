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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdarg>
#include "qcstring.h"

#ifdef __GNUC__
#define PRINTFLIKE(FORMAT, PARAM ) __attribute__((format(printf, FORMAT, PARAM)))
#else
#define PRINTFLIKE(FORMAT, PARAM )
#endif

extern void msg(const char *fmt, ...) PRINTFLIKE(1,2);
extern void warn(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void va_warn(const QCString &file, int line, const char* fmt, va_list args);
extern void warn_simple(const QCString &file,int line,const char *text);
extern void warn_undoc(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void warn_incomplete_doc(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void warn_doc_error(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void warn_uncond(const char *fmt, ...) PRINTFLIKE(1, 2);
extern void err(const char *fmt, ...) PRINTFLIKE(1, 2);
extern void err_full(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void term(const char *fmt, ...) PRINTFLIKE(1, 2);
extern QCString warn_line(const QCString &file,int line);
void initWarningFormat();
void warn_flush();
extern void finishWarnExit();

#undef PRINTFLIKE

#endif
