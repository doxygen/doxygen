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

#ifndef _SECTIONHANDLER_H
#define _SECTIONHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>
#include <doxmlintf.h>

#include "basehandler.h"

class SectionIterator : 
     public BaseIterator<ISectionIterator,ISection,SectionHandler>
{
  public:
    SectionIterator(const QList<SectionHandler> &list) : 
      BaseIterator<ISectionIterator,ISection,SectionHandler>(list) {}
};


class SectionHandler : public IUserDefined, public BaseHandler<SectionHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void startHeader(const QXmlAttributes& attrib);
    virtual void startSection(const QXmlAttributes& attrib);
    virtual void startDescription(const QXmlAttributes& attrib);
    virtual void endSection();
    virtual void endHeader();

    SectionHandler(IBaseHandler *parent);
    virtual ~SectionHandler();

    // ISection
    virtual const IString *kindString() const 
    { return &m_kindString; }
    virtual SectionKind kind() const 
    { return m_kind; }
    IDocRoot *description() const;
    virtual IMemberIterator *members() const;
    virtual bool isStatic() const
    { 
      return m_kind==PubStatFuncs || m_kind==PubStatAttribs || 
             m_kind==ProStatFuncs || m_kind==ProStatAttribs || 
             m_kind==PriStatFuncs || m_kind==PriStatAttribs;
    }
    virtual bool isPublic() const
    {
      return !isProtected() && !isPrivate();
    }
    virtual bool isProtected() const
    {
      return m_kind==ProTypes || m_kind==ProFuncs || m_kind==ProAttribs ||
             m_kind==ProSlots || m_kind==ProStatFuncs || m_kind==ProStatAttribs;
    }
    virtual bool isPrivate() const
    {
      return m_kind==PriTypes || m_kind==PriFuncs || m_kind==PriAttribs ||
             m_kind==PriSlots || m_kind==PriStatFuncs || m_kind==PriStatAttribs;
    }

    void initialize(CompoundHandler *c);

    // IUserDefined implementation
    virtual const IString *header() const 
    {
      return &m_header;
    }

  private:
    IBaseHandler *m_parent;

                                               // XML elements:
                                               // -------------
    StringImpl m_header;                       // header
    DocHandler* m_description;                 // description
    QList<MemberHandler> m_members;            // memberdef

                                               // XML attributes:
                                               // ---------------
    SectionKind m_kind;                        // kind
    StringImpl m_kindString;                   // kind as a string
};

void sectionhandler_init();
void sectionhandler_exit();

#endif
