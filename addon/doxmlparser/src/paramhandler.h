/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include <doxmlintf.h>

#include "basehandler.h"
#include "baseiterator.h"
#include "linkedtexthandler.h"

class ParamIterator : public BaseIterator<IParamIterator,IParam,IParam>
{
  public:
    ParamIterator(const QList<IParam> &list) : 
      BaseIterator<IParamIterator,IParam,IParam>(list) {}
};

class ParamHandler : public IParam, public BaseHandler<ParamHandler>
{
  public:
    virtual void startParam(const QXmlAttributes& attrib);
    virtual void endParam();
    virtual void startType(const QXmlAttributes& attrib);
    virtual void endDeclName();
    virtual void endDefName();
    virtual void endAttrib();
    virtual void endArray();
    virtual void startDefVal(const QXmlAttributes& attrib);

    ParamHandler(IBaseHandler *parent);
    virtual ~ParamHandler();

    // IParam
    virtual ILinkedTextIterator *type() const 
    { return new LinkedTextIterator(m_type); }
    virtual QString declarationName() const { return m_declName; }
    virtual QString definitionName() const { return m_defName; }
    virtual QString attrib() const { return m_attrib; } 
    virtual QString arraySpecifier() const { return m_array; }
    virtual ILinkedTextIterator *defaultValue() const 
    { return new LinkedTextIterator(m_defVal); }

  private:
    IBaseHandler *m_parent;
    QList<ILinkedText> m_type;
    QString m_declName;
    QString m_defName;
    QString m_attrib;
    QString m_array;
    QList<ILinkedText> m_defVal;
    LinkedTextHandler *m_linkedTextHandler;
};

#endif
