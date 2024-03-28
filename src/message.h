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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdarg>
#include "qcstring.h"
#include "util.h"

#ifdef __GNUC__
#define PRINTFLIKE(FORMAT, PARAM ) __attribute__((format(printf, FORMAT, PARAM)))
#else
#define PRINTFLIKE(FORMAT, PARAM )
#endif

extern void msg(const char *fmt, ...) PRINTFLIKE(1,2);
extern void warn_(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void va_warn(const QCString &file, int line, const char* fmt, va_list args);
extern void warn_undoc_(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void warn_incomplete_doc_(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void warn_doc_error_(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void warn_uncond_(const char *fmt, ...) PRINTFLIKE(1, 2);
extern void err_(const char *fmt, ...) PRINTFLIKE(1, 2);
extern void err_full_(const QCString &file,int line,const char *fmt, ...) PRINTFLIKE(3, 4);
extern void term_(const char *fmt, ...) PRINTFLIKE(1, 2);
extern QCString warn_line(const QCString &file,int line);
void initWarningFormat();
void warn_flush();
extern void finishWarnExit();

#undef PRINTFLIKE

// N is size including 0-terminal
template<std::size_t N>
constexpr bool has_newline_at_end(const char (&str)[N])
{
  return str[N-2]=='\n';
}

#define msg_no_newline_allowed(x) \
   static_assert(!has_newline_at_end(x),"text: \"" x "\" should not have \\n at end");

#define msg_newline_required(x) \
   static_assert(has_newline_at_end(x),"text: \"" x "\" should have \\n at end");

#define warn(file,line,fmt,...) do { \
   msg_no_newline_allowed(fmt);      \
   warn_(removeLongPathMarker(file),line,fmt,##__VA_ARGS__); \
   } while(0)

#define warn_undoc(file,line,fmt,...) do { \
   msg_no_newline_allowed(fmt);            \
   warn_undoc_(removeLongPathMarker(file),line,fmt,##__VA_ARGS__); \
   } while(0)

#define warn_incomplete_doc(file,line,fmt,...) do { \
   msg_no_newline_allowed(fmt);                     \
   warn_incomplete_doc_(removeLongPathMarker(file),line,fmt,##__VA_ARGS__); \
   } while(0)

#define warn_doc_error(file,line,fmt,...) do { \
   msg_no_newline_allowed(fmt);                \
   warn_doc_error_(removeLongPathMarker(file),line,fmt,##__VA_ARGS__); \
   } while(0)

#define warn_uncond(fmt,...) do { \
   msg_newline_required(fmt);     \
   warn_uncond_(fmt,##__VA_ARGS__); \
   } while(0)

#define err(fmt,...) do {     \
   msg_newline_required(fmt); \
   err_(fmt,##__VA_ARGS__);     \
   } while(0)

#define err_full(file,line,fmt,...) do { \
   msg_no_newline_allowed(fmt);          \
   err_full_(removeLongPathMarker(file),line,fmt,##__VA_ARGS__); \
   } while(0)

#define term(fmt,...) do {    \
   msg_newline_required(fmt); \
   term_(fmt,##__VA_ARGS__);    \
   } while(0)

#endif // MESSAGE_H

