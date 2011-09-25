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

#ifndef _PARAMHANDLER_H
#define _PARAMHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>
#include <doxmlintf.h>

#include "stringimpl.h"
#include "basehandler.h"
#include "baseiterator.h"

class LinkedTextImpl;
class LinkedTextHandler;
class DocHandler;


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
    virtual void startBriefDesc(const QXmlAttributes& attrib);

    ParamHandler(IBaseHandler *parent);
    virtual ~ParamHandler();

    // IParam
    virtual ILinkedTextIterator *type() const;
    virtual const IString * declarationName() const { return &m_declName; }
    virtual const IString * definitionName() const { return &m_defName; }
    virtual const IString * attrib() const { return &m_attrib; } 
    virtual const IString * arraySpecifier() const { return &m_array; }
    virtual ILinkedTextIterator *defaultValue() const;
    virtual IDocRoot *briefDescription() const;

  private:

                                          // XML elements:
                                          // -------------
    QList<LinkedTextImpl> m_type;         // type
    StringImpl m_declName;                // declname
    StringImpl m_defName;                 // defname
    StringImpl m_array;                   // array
    QList<LinkedTextImpl> m_defVal;       // defval
    DocHandler  *m_brief;                 // briefdescription

    StringImpl m_attrib;                  // TODO: not yet in XML output

    IBaseHandler *m_parent;
    LinkedTextHandler *m_linkedTextHandler;
};

class ParamIterator : public BaseIterator<IParamIterator,IParam,ParamHandler>
{
  public:
    ParamIterator(const QList<ParamHandler> &list) : 
      BaseIterator<IParamIterator,IParam,ParamHandler>(list) {}
};

class TemplateParamListHandler : public BaseHandler<TemplateParamListHandler>
{
  public:

    virtual void startParam(const QXmlAttributes& attrib);
    virtual void endParam();

    virtual void startTemplateParamList(const QXmlAttributes& attrib);
    virtual void endTemplateParamList();

    TemplateParamListHandler(IBaseHandler *parent);
	virtual ~TemplateParamListHandler() {}

	ParamIterator* templateParams() { return new ParamIterator(m_templateParams); }

  protected:
    IBaseHandler *m_parent;
    QList<ParamHandler> m_templateParams;
};


#endif
