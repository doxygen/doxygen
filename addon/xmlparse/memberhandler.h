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

#ifndef _MEMBERHANDLER_H
#define _MEMBERHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"
#include "paramhandler.h"
#include "doxmlintf.h"

class DocHandler;

class MemberHandler : public IMember, public BaseHandler<MemberHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void endMember();
    virtual void startParam(const QXmlAttributes& attrib);
    virtual void endType();
    virtual void endName();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);
    virtual void startLocation(const QXmlAttributes& attrib);
    virtual void startReferences(const QXmlAttributes& attrib);
    virtual void endReferences();
    virtual void startReferencedBy(const QXmlAttributes& attrib);
    virtual void endReferencedBy();

    MemberHandler(IBaseHandler *parent);
    virtual ~MemberHandler();

    // IMember
    virtual QString kind() const { return m_kind; }
    virtual QString id() const { return m_id; }
    virtual QString protection() const { return m_protection; }
    virtual QString virtualness() const { return m_virtualness; }
    virtual QString type() const { return m_type; }
    virtual QString name() const { return m_name; }
    virtual QListIterator<IParam> getParamIterator() const { return m_params; }

  private:
    struct MemberReference
    {
      QString m_memId;
      QString m_name;
      int line;
    };
    
    IBaseHandler *m_parent;
    QString m_kind;
    QString m_id;
    QString m_protection;
    QString m_virtualness;
    QString m_type;
    QString m_name;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    QList<IParam> m_params;
    QList<MemberReference> m_references;
    QList<MemberReference> m_referencedBy;
    QString m_defFile;
    int m_defLine;
};

#endif
