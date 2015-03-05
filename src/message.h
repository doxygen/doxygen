/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <stdio.h>
#include <stdarg.h>

extern void msg(const char *fmt, ...);
extern void warn(const char *file,int line,const char *fmt, ...);
extern void va_warn(const char *file,int line,const char *fmt, va_list args);
extern void warn_simple(const char *file,int line,const char *text);
extern void warn_undoc(const char *file,int line,const char *fmt, ...);
extern void warn_doc_error(const char *file,int line,const char *fmt, ...);
extern void warn_uncond(const char *fmt, ...);
extern void err(const char *fmt, ...);
extern void err_full(const char *file,int line,const char *fmt, ...);
void initWarningFormat();

extern void printlex(int dbg, bool enter, const char *lexName, const char *fileName);
#endif
