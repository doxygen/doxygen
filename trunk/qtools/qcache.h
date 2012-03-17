/****************************************************************************
** 
**
** Definition of QCache template class
**
** Created : 950209
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

#ifndef QCACHE_H
#define QCACHE_H

#ifndef QT_H
#include "qgcache.h"
#endif // QT_H

#define USE_ASCII_STRING

#ifndef USE_ASCII_STRING

template<class type> class Q_EXPORT QCache : public QGCache
{
public:
    QCache( const QCache<type> &c ) : QGCache(c) {}
    QCache( int maxCost=100, int size=17, bool caseSensitive=TRUE )
	: QGCache( maxCost, size, StringKey, caseSensitive, FALSE ) {}
   ~QCache()				{ clear(); }
    QCache<type> &operator=( const QCache<type> &c )
			{ return (QCache<type>&)QGCache::operator=(c); }
    int	  maxCost()   const		{ return QGCache::maxCost(); }
    int	  totalCost() const		{ return QGCache::totalCost(); }
    void  setMaxCost( int m )		{ QGCache::setMaxCost(m); }
    uint  count()     const		{ return QGCache::count(); }
    uint  size()      const		{ return QGCache::size(); }
    bool  isEmpty()   const		{ return QGCache::count() == 0; }
    void  clear()			{ QGCache::clear(); }
    bool  insert( const QString &k, const type *d, int c=1, int p=0 )
			{ return QGCache::insert_string(k,(Item)d,c,p);}
    bool  remove( const QString &k )
			{ return QGCache::remove_string(k); }
    type *take( const QString &k )
			{ return (type *)QGCache::take_string(k); }
    type *find( const QString &k, bool ref=TRUE ) const
			{ return (type *)QGCache::find_string(k,ref);}
    type *operator[]( const QString &k ) const
			{ return (type *)QGCache::find_string(k);}
    void  statistics() const	      { QGCache::statistics(); }
    int   hits() const                { return QGCache::hits(); }
    int   misses() const              { return QGCache::misses(); }
private:
    void  deleteItem( Item d )	      { if ( del_item ) delete (type *)d; }
};

#else


template<class type> class Q_EXPORT QCache : public QGCache
{
public:
    QCache( const QCache<type> &c ) : QGCache(c) {}
    QCache( int maxCost=100, int size=17, bool caseSensitive=TRUE )
	: QGCache( maxCost, size, AsciiKey, caseSensitive, TRUE ) {}
   ~QCache()				{ clear(); }
    QCache<type> &operator=( const QCache<type> &c )
			{ return (QCache<type>&)QGCache::operator=(c); }
    int	  maxCost()   const		{ return QGCache::maxCost(); }
    int	  totalCost() const		{ return QGCache::totalCost(); }
    void  setMaxCost( int m )		{ QGCache::setMaxCost(m); }
    uint  count()     const		{ return QGCache::count(); }
    uint  size()      const		{ return QGCache::size(); }
    bool  isEmpty()   const		{ return QGCache::count() == 0; }
    void  clear()			{ QGCache::clear(); }
    bool  insert( const char *k, const type *d, int c=1, int p=0 )
			{ return QGCache::insert_other(k,(Item)d,c,p);}
    bool  remove( const char *k )
			{ return QGCache::remove_other(k); }
    type *take( const char *k )
			{ return (type *)QGCache::take_other(k); }
    type *find( const char *k, bool ref=TRUE ) const
			{ return (type *)QGCache::find_other(k,ref);}
    type *operator[]( const char *k ) const
			{ return (type *)QGCache::find_other(k);}
    void  statistics() const	      { QGCache::statistics(); }
    int   hits() const                { return QGCache::hits(); }
    int   misses() const              { return QGCache::misses(); }
private:
    void  deleteItem( Item d )	      { if ( del_item ) delete (type *)d; }
};


#endif



template<class type> class Q_EXPORT QCacheIterator : public QGCacheIterator
{
public:
    QCacheIterator( const QCache<type> &c ):QGCacheIterator((QGCache &)c) {}
    QCacheIterator( const QCacheIterator<type> &ci)
				: QGCacheIterator( (QGCacheIterator &)ci ) {}
    QCacheIterator<type> &operator=(const QCacheIterator<type>&ci)
	{ return ( QCacheIterator<type>&)QGCacheIterator::operator=( ci ); }
    uint  count()   const     { return QGCacheIterator::count(); }
    bool  isEmpty() const     { return QGCacheIterator::count() == 0; }
    bool  atFirst() const     { return QGCacheIterator::atFirst(); }
    bool  atLast()  const     { return QGCacheIterator::atLast(); }
    type *toFirst()	      { return (type *)QGCacheIterator::toFirst(); }
    type *toLast()	      { return (type *)QGCacheIterator::toLast(); }
    operator type *() const   { return (type *)QGCacheIterator::get(); }
    type *current()   const   { return (type *)QGCacheIterator::get(); }
#ifndef USE_ASCII_STRING
    QString currentKey() const{ return QGCacheIterator::getKeyString(); }
#else
    const char *currentKey() const{ return QGCacheIterator::getKeyAscii(); }
#endif
    type *operator()()	      { return (type *)QGCacheIterator::operator()();}
    type *operator++()	      { return (type *)QGCacheIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)QGCacheIterator::operator+=(j);}
    type *operator--()	      { return (type *)QGCacheIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)QGCacheIterator::operator-=(j);}
};


#endif // QCACHE_H
