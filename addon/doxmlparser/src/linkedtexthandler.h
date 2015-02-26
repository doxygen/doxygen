/******************************************************************************
 *
 * $Id$
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
 */
#ifndef LINKEDTEXTHANDLER_H
#define LINKEDTEXTHANDLER_H

#include "baseiterator.h"
#include "basehandler.h"

class LT_Ref;
class LinkedTextImpl : public ILinkedText
{
  public:
    virtual ~LinkedTextImpl() {}
};

class LinkedTextHandler : public BaseHandler<LinkedTextHandler>
{
  public:
    LinkedTextHandler(IBaseHandler *parent,QList<LinkedTextImpl> &children);
    virtual ~LinkedTextHandler();
    virtual void start(const char *endTag);
    virtual void end();
    virtual void startRef(const QXmlAttributes& attrib);
    virtual void endRef();
    static QString toString(const QList<LinkedTextImpl> &list);

    // ILinkedText

  private:
    IBaseHandler *m_parent;
    QList<LinkedTextImpl> &m_children;
    LT_Ref *m_ref;
};

class LinkedTextIterator : public BaseIterator<ILinkedTextIterator,ILinkedText,LinkedTextImpl>
{
  public:
    LinkedTextIterator(const QList<LinkedTextImpl> &list) : 
      BaseIterator<ILinkedTextIterator,ILinkedText,LinkedTextImpl>(list) {}
};

#endif
