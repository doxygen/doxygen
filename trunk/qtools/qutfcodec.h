/****************************************************************************
** 
**
** Definition of QEucCodec class
**
** Created : 981015
**
** Copyright (C) 1998-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QUTFCODEC_H
#define QUTFCODEC_H

#ifndef QT_H
#include "qtextcodec.h"
#endif // QT_H

#ifndef QT_NO_TEXTCODEC

class Q_EXPORT QUtf8Codec : public QTextCodec {
public:
    virtual int mibEnum() const;
    const char* name() const;

    QTextDecoder* makeDecoder() const;

    QCString fromUnicode(const QString& uc, int& len_in_out) const;

    int heuristicContentMatch(const char* chars, int len) const;
};

class Q_EXPORT QUtf16Codec : public QTextCodec {
public:
    virtual int mibEnum() const;
    const char* name() const;

    QTextDecoder* makeDecoder() const;
    QTextEncoder* makeEncoder() const;

    int heuristicContentMatch(const char* chars, int len) const;
};

#endif //QT_NO_TEXTCODEC
#endif // QUTFCODEC_H
