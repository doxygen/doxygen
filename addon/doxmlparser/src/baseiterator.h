/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */
#ifndef BASEITERATOR_H
#define BASEITERATOR_H

#include <qlist.h>
#include <doxmlintf.h>

template<class Intf,class ElemIntf,class ElemImpl> class BaseIterator : 
        public Intf, public QListIterator<ElemImpl>
{
  public:
    BaseIterator(const QList<ElemImpl> &list) : QListIterator<ElemImpl>(list) {}
    virtual ~BaseIterator() {}
    virtual ElemIntf *toFirst() { return QListIterator<ElemImpl>::toFirst(); }
    virtual ElemIntf *toLast()  { return QListIterator<ElemImpl>::toLast(); }
    virtual ElemIntf *toNext()  { return QListIterator<ElemImpl>::operator++(); }
    virtual ElemIntf *toPrev()  { return QListIterator<ElemImpl>::operator--(); }
    virtual ElemIntf *current() const { return QListIterator<ElemImpl>::current(); }
    virtual void release() { delete this; }
};

#endif
