/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#ifndef QTBC_H
#define QTBC_H

/*! This file contains some hacks to make Doxygen work with 
 *  Qt version 2.00 and Qt version 1.xx 
 */

#include <qglobal.h>

#if QT_VERSION >= 200

#include <locale.h>

#define GCI QCollection::Item

#include <qcstring.h>
#include <qstring.h>
inline QCString convertToQCString(const QString &s) { return s.latin1(); }

#else /* QT_VERSION < 200 */

#include <qstring.h>
#define QCString QString
inline QCString convertToQCString(const QCString &s) { return s; }

#endif

#endif
