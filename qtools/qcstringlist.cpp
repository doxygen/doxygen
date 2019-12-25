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
** Implementation of QCStringList
**
**********************************************************************/

#include "qcstringlist.h"

#include "qstrlist.h"
#include "qdatastream.h"
#include "qtl.h"

/*!
  Splits the string \a str using \a sep as separator. Returns the
  list of strings. If \a allowEmptyEntries is TRUE, also empty
  entries are inserted into the list, else not. So if you have
  a string 'abc..d.e.', a list which contains 'abc', 'd', and 'e'
  would be returned if \a allowEmptyEntries is FALSE, but
  a list containing 'abc', '', 'd', 'e' and '' would be returned if
  \a allowEmptyEntries is TRUE.
  If \a str doesn't contain \a sep, a stringlist
  with one item, which is the same as \a str, is returned.

  \sa join()
*/

QCStringList QCStringList::split( char sep, const QCString &str, bool allowEmptyEntries )
{
    char cs[2] = { sep, '\0' };
    return split( cs, str, allowEmptyEntries );
}

/*!
  Splits the string \a str using \a sep as separator. Returns the
  list of strings. If \a allowEmptyEntries is TRUE, also empty
  entries are inserted into the list, else not. So if you have
  a string 'abc..d.e.', a list which contains 'abc', 'd', and 'e'
  would be returned if \a allowEmptyEntries is FALSE, but
  a list containing 'abc', '', 'd', 'e' and '' would be returned if
  \a allowEmptyEntries is TRUE.
  If \a str doesn't contain \a sep, a stringlist
  with one item, which is the same as \a str, is returned.

  \sa join()
*/

QCStringList QCStringList::split( const QCString &sep, const QCString &str, bool allowEmptyEntries )
{
    QCStringList lst;

    int j = 0;
    int i = str.find( sep, j );

    while ( i != -1 ) {
	if ( str.mid( j, i - j ).length() > 0 )
	    lst << str.mid( j, i - j );
	else if ( allowEmptyEntries )
	    lst << QCString();
	j = i + sep.length();
	i = str.find( sep, j );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
	lst << str.mid( j, l - j + 1 );
    else if ( allowEmptyEntries )
	lst << QCString();

    return lst;
}

/*!
  Splits the string \a str using the regular expression \a sep as separator. Returns the
  list of strings. If \a allowEmptyEntries is TRUE, also empty
  entries are inserted into the list, else not. So if you have
  a string 'abc..d.e.', a list which contains 'abc', 'd', and 'e'
  would be returned if \a allowEmptyEntries is FALSE, but
  a list containing 'abc', '', 'd', 'e' and '' would be returned if
  \a allowEmptyEntries is TRUE.
  If \a str doesn't contain \a sep, a stringlist
  with one item, which is the same as \a str, is returned.

  \sa join()
*/

QCStringList QCStringList::split( const QRegExp &sep, const QCString &str, bool allowEmptyEntries )
{
    QCStringList lst;

    int j = 0;
    int len = 0;
    int i = sep.match( str.data(), j, &len );

    while ( i != -1 ) {
	if ( str.mid( j, i - j ).length() > 0 )
	    lst << str.mid( j, i - j );
	else if ( allowEmptyEntries )
	    lst << QCString();
	j = i + len;
	i = sep.match( str.data(), j, &len );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
	lst << str.mid( j, l - j + 1 );
    else if ( allowEmptyEntries )
	lst << QCString();

    return lst;
}

/*!
  Returns a list of all strings containing the substring \a str.

  If \a cs is TRUE, the grep is done case sensitively, else not.
*/

QCStringList QCStringList::grep( const QCString &str, bool cs ) const
{
    QCStringList res;
    for ( QCStringList::ConstIterator it = begin(); it != end(); ++it )
	if ( (*it).contains( str, cs ) )
	    res << *it;

    return res;
}

/*!
  Returns a list of all strings containing a substring that matches
  the regular expression \a expr.
*/

QCStringList QCStringList::grep( const QRegExp &expr ) const
{
    QCStringList res;
    for ( QCStringList::ConstIterator it = begin(); it != end(); ++it )
	if ( (*it).contains( expr ) )
	    res << *it;

    return res;
}

/*!
  Joins the stringlist into a single string with each element
  separated by \a sep.

  \sa split()
*/
QCString QCStringList::join( const QCString &sep ) const
{
    QCString res;
    bool already = FALSE;
    for ( QCStringList::ConstIterator it = begin(); it != end(); ++it ) {
	if ( already )
	    res += sep;
	already = TRUE;
	res += *it;
    }

    return res;
}

Q_EXPORT QDataStream &operator>>( QDataStream & s, QCStringList& l )
{
    return s >> (QValueList<QCString>&)l;
}

Q_EXPORT QDataStream &operator<<( QDataStream & s, const QCStringList& l )
{
    return s << (const QValueList<QCString>&)l;
}

/*!
  Converts from a QStrList (ASCII) to a QCStringList (Unicode).
*/
QCStringList QCStringList::fromStrList(const QStrList& ascii)
{
    QCStringList res;
    const char * s;
    for ( QStrListIterator it(ascii); (s=it.current()); ++it )
	res << s;
    return res;
}

