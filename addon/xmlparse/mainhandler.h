#ifndef _MAINHANDLER_H
#define _MAINHANDLER_H

#include <qlist.h>
#include "basehandler.h"
#include "compoundhandler.h"

class MainHandler : public BaseHandler<MainHandler>
{
  public:
    virtual void startCompound(const QXmlAttributes& attrib);
    MainHandler();
    virtual ~MainHandler();
  private:
    QList<CompoundHandler> m_compounds;
};

#endif
