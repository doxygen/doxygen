#ifndef _MEMBERHANDLER_H
#define _MEMBERHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"
#include "paramhandler.h"

class DocHandler;

class MemberHandler : public BaseHandler<MemberHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void endMember();
    virtual void startParam(const QXmlAttributes& attrib);
    virtual void endType();
    virtual void endName();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);

    MemberHandler(IBaseHandler *parent);
    virtual ~MemberHandler();
  private:
    IBaseHandler *m_parent;
    QString m_kind;
    QString m_id;
    QString m_protection;
    QString m_virtualness;
    QString m_type;
    QString m_name;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    QList<ParamHandler> m_params;
};

#endif
