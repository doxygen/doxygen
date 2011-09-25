/******************************************************************************
 *
 * $Id:$
 *
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _LOAMHANDLER_H
#define _LOAMHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <doxmlintf.h>

#include "basehandler.h"

class MainHandler;
class MemberReference;

class ListOfAllMembersHandler : public BaseHandler<ListOfAllMembersHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void startName(const QXmlAttributes& attrib);
    virtual void endName();
    virtual void startScope(const QXmlAttributes& attrib);
    virtual void endScope();
    virtual void startListOfAllMembers(const QXmlAttributes& attrib);
    virtual void endListOfAllMembers();

    ListOfAllMembersHandler(IBaseHandler *parent);
    virtual ~ListOfAllMembersHandler() {}

    void initialize(MainHandler *mh);

    virtual IMemberReferenceIterator *members() const;

  protected:
    IBaseHandler *m_parent;
    QList<MemberReference> m_members;
};

#endif

