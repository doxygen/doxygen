#ifndef _COMPOUNDHANDLER_H
#define _COMPOUNDHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"
#include "sectionhandler.h"

class DocHandler;

class CompoundHandler : public BaseHandler<CompoundHandler>
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

    CompoundHandler(IBaseHandler *parent);
    virtual ~CompoundHandler();
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
    QList<SuperClass> m_superClasses;
    QList<SubClass> m_subClasses;
    QList<SectionHandler> m_sections;
    IBaseHandler *m_parent;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    QString m_id;
    QString m_kind;
    QString m_name;
};

#endif
