/****************************************************************************
** 
**
** Definition of QStrVec and QStrIVec classes
**
** Created : 931203
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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

#ifndef QSTRVEC_H
#define QSTRVEC_H

#ifndef QT_H
#include "qstring.h"
#include "qvector.h"
#include "qdatastream.h"
#endif // QT_H


#if defined(Q_TEMPLATEDLL)
template class Q_EXPORT QVector<char>
#endif

typedef QVector<char> QStrVecBase;


class Q_EXPORT QStrVec : public QStrVecBase
{
public:
    QStrVec()  { dc = TRUE; }
    QStrVec( uint size, bool deepc = TRUE ) : QStrVecBase(size) {dc=deepc;}
   ~QStrVec()  { clear(); }
private:
    Item	 newItem( Item d )	{ return dc ? qstrdup( (const char*)d ) : d; }
    void deleteItem( Item d )	{ if ( dc ) delete[] (char*)d; }
    int	 compareItems( Item s1, Item s2 )
				{ return qstrcmp((const char*)s1,
						(const char*)s2); }
#ifndef QT_NO_DATASTREAM
    QDataStream &read( QDataStream &s, Item &d )
				{ s >> (char *&)d; return s; }
    QDataStream &write( QDataStream &s, Item d ) const
				{ return s << (const char*)d; }
#endif
    bool dc;
};


class Q_EXPORT QStrIVec : public QStrVec	// case insensitive string vec
{
public:
    QStrIVec() {}
    QStrIVec( uint size, bool dc = TRUE ) : QStrVec( size, dc ) {}
   ~QStrIVec() { clear(); }
private:
    int	 compareItems( Item s1, Item s2 )
				{ return qstricmp((const char*)s1,
						 (const char*)s2); }
};


#endif // QSTRVEC_H
