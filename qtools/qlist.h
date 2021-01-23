/****************************************************************************
** 
**
** Definition of QList template/macro class
**
** Created : 920701
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

/* This is a stripped version of the original QList, which has been renamed to
   QInternalList. This implementation doesn't expose the current node and index,
   nor direct access to the list nodes.
   This makes it possible to have more constant methods. It also provides
   a typesafe method to compare elements called compareValues() and a typesafe
   methods to create and delete elements called newValue() and deleteValue().
 */

#ifndef QLIST_H
#define QLIST_H

#ifndef QT_H
#include "qglist.h"
#endif // QT_H


template<class type> class Q_EXPORT QList : private QGList
{
public:
    QList()				{}
    QList( const QList<type> &l ) : QGList(l) {}
   ~QList()				{ clear(); }
    QList<type> &operator=(const QList<type> &l)
			{ return (QList<type>&)QGList::operator=(l); }
    bool operator==( const QList<type> &list ) const
    { return QGList::operator==( list ); }

    // capacity
    uint  count()   const		{ return QGList::count(); }
    bool  isEmpty() const		{ return QGList::count() == 0; }

    // modifiers add
    bool  insert( uint i, const type *d){ return QGList::insertAt(i,(QCollection::Item)d); }
    void  inSort( const type *d )	{ QGList::inSort((QCollection::Item)d); }
    void  prepend( const type *d )	{ QGList::insertAt(0,(QCollection::Item)d); }
    void  append( const type *d )	{ QGList::append((QCollection::Item)d); }

    // modifiers remove
    bool  remove( uint i )		{ return QGList::removeAt(i); }
    bool  remove( const type *d )	{ return QGList::remove((QCollection::Item)d); }
    bool  removeRef( const type *d )	{ return QGList::removeRef((QCollection::Item)d); }
    bool  removeFirst()			{ return QGList::removeFirst(); }
    bool  removeLast()			{ return QGList::removeLast(); }
    type *take( uint i )		{ return (type *)QGList::takeAt(i); }
    void  clear()			{ QGList::clear(); }

    // operations
    void  sort()			{ QGList::sort(); }

    // search
    int	  find( const type *d ) const	 { return const_cast<QList<type>*>(this)->QGList::find((QCollection::Item)d); }
    int	  findRef( const type *d ) const { return const_cast<QList<type>*>(this)->QGList::findRef((QCollection::Item)d); }
    uint  contains( const type *d ) const { return QGList::contains((QCollection::Item)d); }
    uint  containsRef( const type *d ) const { return QGList::containsRef((QCollection::Item)d); }

    // element access
    type *at( uint i ) const		{ return (type *)const_cast<QList<type>*>(this)->QGList::at(i); }
    type *getFirst() const		{ return (type *)QGList::cfirst(); }
    type *getLast()  const		{ return (type *)QGList::clast(); }

    // ownership
    void setAutoDelete( bool enable )   { QGList::setAutoDelete(enable); }
    bool autoDelete() const             { return QGList::autoDelete(); }

private:
    // new to be reimplemented methods
    virtual int compareValues(const type *t1,const type *t2) const
    { return const_cast<QList<type>*>(this)->QGList::compareItems((QCollection::Item)t1,(QCollection::Item)t2); }
    virtual type *newValue(type *item) const
    { return item; }
    virtual void deleteValue(type *item) const
    { if (del_item) delete item; }

    // reimplemented methods
    virtual Item newItem( Item item)
    { return (Item)newValue((type*)item); }
    virtual void deleteItem( QCollection::Item item )
    { deleteValue((type *)item); }
    virtual int compareItems(QCollection::Item i1,QCollection::Item i2)
    { return compareValues((const type*)i1,(const type*)i2); }
};

#if defined(Q_DELETING_VOID_UNDEFINED)
template<> inline void QList<void>::deleteValue(void *) const
{
}
#endif


template<class type> class Q_EXPORT QListIterator : public QGListIterator
{
public:
    QListIterator(const QList<type> &l) :QGListIterator((QGList &)l) {}
   ~QListIterator()	      {}
    uint  count()   const     { return list->count(); }
    bool  isEmpty() const     { return list->count() == 0; }
    bool  atFirst() const     { return QGListIterator::atFirst(); }
    bool  atLast()  const     { return QGListIterator::atLast(); }
    type *toFirst()	      { return (type *)QGListIterator::toFirst(); }
    type *toLast()	      { return (type *)QGListIterator::toLast(); }
    operator type *() const   { return (type *)QGListIterator::get(); }
    type *operator*()         { return (type *)QGListIterator::get(); }

    // No good, since QList<char> (ie. QStrList fails...
    //
    // MSVC++ gives warning
    // Sunpro C++ 4.1 gives error
    //    type *operator->()        { return (type *)QGListIterator::get(); }

    type *current()   const   { return (type *)QGListIterator::get(); }
    type *operator()()	      { return (type *)QGListIterator::operator()();}
    type *operator++()	      { return (type *)QGListIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)QGListIterator::operator+=(j);}
    type *operator--()	      { return (type *)QGListIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)QGListIterator::operator-=(j);}
    QListIterator<type>& operator=(const QListIterator<type>&it)
			      { QGListIterator::operator=(it); return *this; }
};


#endif // QLIST_H
