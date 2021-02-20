/****************************************************************************
**
** Copyright (C) 1997-2018 by Dimitri van Heesch.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation under the terms of the GNU General Public License is hereby
** granted. No representations are made about the suitability of this software
** for any purpose. It is provided "as is" without express or implied warranty.
** See the GNU General Public License for more details.
**
** Note: this is a variant of the qstringlist.h but for QCString's
**
**********************************************************************/
#ifndef QCSTRINGLIST_H
#define QCSTRINGLIST_H

#include "qvaluelist_p.h"
#include "qcstring.h"
//#include "qregexp.h"

class QStrList;
class QDataStream;

class QCStringList : public QValueList<QCString>
{
public:
    QCStringList() { }
    QCStringList( const QCStringList& l ) : QValueList<QCString>(l) { }
    QCStringList( const QValueList<QCString>& l ) : QValueList<QCString>(l) { }
    QCStringList( const QCString& i ) { append(i); }
    QCStringList( const char* i ) { append(i); }
    QCStringList &operator=(const QCStringList &l)
    {
      return static_cast<QCStringList&>(QValueList<QCString>::operator=(l));
    }

    static QCStringList fromStrList(const QStrList&);

    static QCStringList split( const QCString &sep, const QCString &str, bool allowEmptyEntries = FALSE );
    static QCStringList split( char sep,            const QCString &str, bool allowEmptyEntries = FALSE );
    //static QCStringList split( const QRegExp &sep,  const QCString &str, bool allowEmptyEntries = FALSE );
    QCString join( const QCString &sep ) const;

    QCStringList grep( const QCString &str, bool cs = TRUE ) const;
    //QCStringList grep( const QRegExp &expr ) const;
};

extern Q_EXPORT QDataStream &operator>>( QDataStream &, QCStringList& );
extern Q_EXPORT QDataStream &operator<<( QDataStream &, const QCStringList& );

#endif // QCSTRINGLIST_H
