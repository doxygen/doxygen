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
#ifndef _COMPOUNDHANDLER_H
#define _COMPOUNDHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"
#include "sectionhandler.h"
#include "doxmlintf.h"

class DocHandler;

class CompoundHandler : public ICompound, public BaseHandler<CompoundHandler>
{
  public:
    virtual void startSection(const QXmlAttributes& attrib);
    virtual void startCompound(const QXmlAttributes& attrib);
    virtual void addSuperClass(const QXmlAttributes& attrib);
    virtual void addSubClass(const QXmlAttributes& attrib);
    virtual void endCompound();
    virtual void endCompoundName();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);
    virtual void startLocation(const QXmlAttributes& attrib);

    CompoundHandler(IBaseHandler *parent);
    virtual ~CompoundHandler();

    // ICompound
    QString name() const { return m_name; }
    QString id()   const { return m_id;   }
    QString kind() const { return m_kind; }
    QListIterator<ISection> getSectionIterator() const { return m_sections; }

  private:
    struct SuperClass
    {
      SuperClass(const QString &id,const QString &prot,const QString &virt) :
        m_id(id),m_protection(prot),m_virtualness(virt) {}

      QString m_id;
      QString m_protection;
      QString m_virtualness;
    };
    struct SubClass
    {
      SubClass(const QString &id,const QString &prot,const QString &virt) :
        m_id(id),m_protection(prot),m_virtualness(virt) {}

      QString m_id;
      QString m_protection;
      QString m_virtualness;
    };
    QList<SuperClass>     m_superClasses;
    QList<SubClass>       m_subClasses;
    QList<ISection>       m_sections;
    IBaseHandler *m_parent;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    QString m_id;
    QString m_kind;
    QString m_name;
    QString m_defFile;
    int m_defLine;
};

#endif
