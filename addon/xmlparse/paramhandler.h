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

#ifndef _PARAMHANDLER_H
#define _PARAMHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"

class ParamHandler : public BaseHandler<ParamHandler>
{
  public:
    virtual void startParam(const QXmlAttributes& attrib);
    virtual void endParam();
    virtual void endType();
    virtual void endDeclName();
    virtual void endDefName();
    virtual void endAttrib();
    virtual void endArray();
    virtual void endDefVal();

    ParamHandler(IBaseHandler *parent);
    virtual ~ParamHandler();
  private:
    IBaseHandler *m_parent;
    QString m_type;
    QString m_declName;
    QString m_defName;
    QString m_attrib;
    QString m_array;
    QString m_defVal;
};

#endif
