#ifndef _SECTIONHANDLER_H
#define _SECTIONHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"
#include "memberhandler.h"

class SectionHandler : public BaseHandler<SectionHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void startSection(const QXmlAttributes& attrib);
    virtual void endSection();

    SectionHandler(IBaseHandler *parent);
    virtual ~SectionHandler();
  private:
    IBaseHandler *m_parent;
    QString m_kind;
    QList<MemberHandler> m_members;
};

#endif
