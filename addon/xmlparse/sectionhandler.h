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

#ifndef _SECTIONHANDLER_H
#define _SECTIONHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"
#include "memberhandler.h"
#include "doxmlintf.h"

class MainHandler;

class SectionHandler : public ISection, public BaseHandler<SectionHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void startSection(const QXmlAttributes& attrib);
    virtual void endSection();

    SectionHandler(IBaseHandler *parent);
    virtual ~SectionHandler();

    // ISection
    virtual QString kind() const { return m_kind; }
    virtual QListIterator<IMember> getMemberIterator() const { return m_members; }

    void initialize(MainHandler *m);

  private:
    IBaseHandler *m_parent;
    QString m_kind;
    QList<IMember> m_members;
};

#endif
