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
#include "linkedtexthandler.h"
#include <doxmlintf.h>

class LT_Text : public ILT_Text
{
  public:
    LT_Text(const QString &text) : m_text(text) {}
    virtual ~LT_Text() {}

    // ILT_Text
    virtual QString text() const { return m_text; }
    virtual Kind kind() const { return Kind_Text; }
  private:
    QString m_text;
};

class LT_Ref : public ILT_Ref
{
  public:
    LT_Ref() {}
    virtual ~LT_Ref() {}
    void setRefId(const QString &refId) { m_refId=refId; }
    void setText(const QString &text) { m_text=text; }
    void setExtId(const QString &extId) { m_extId=extId; }

    // ILT_Ref
    virtual QString text() const { return m_text; }
    virtual QString id() const { return m_refId; }
    virtual QString external() const { return m_extId; }
    virtual Kind kind() const { return Kind_Ref; }
    
  private:
    QString m_refId;
    QString m_extId;
    QString m_text;
};

LinkedTextHandler::LinkedTextHandler(IBaseHandler *parent,
                                     QList<ILinkedText> &children
                                    ) 
 : m_parent(parent), m_children(children)
{
  addStartHandler("ref",this,&LinkedTextHandler::startRef);
  addEndHandler("ref",this,&LinkedTextHandler::endRef);
  m_children.setAutoDelete(TRUE);
  m_ref=0;
}

LinkedTextHandler::~LinkedTextHandler()
{
}

void LinkedTextHandler::start(const char *endTag)
{
  addEndHandler(endTag,this,&LinkedTextHandler::end);
  m_parent->setDelegate(this);
  m_curString="";
}

void LinkedTextHandler::end()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new LT_Text(m_curString));
    printf("LinkedTextHandler: add text `%s'\n",m_curString.data());
    m_curString="";
  }
  m_parent->setDelegate(0);
}

void LinkedTextHandler::startRef(const QXmlAttributes& attrib)
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new LT_Text(m_curString));
    printf("LinkedTextHandler: add text `%s'\n",m_curString.data());
    m_curString="";
  }
  ASSERT(m_ref==0);
  m_ref = new LT_Ref;
  m_ref->setRefId(attrib.value("idref"));
  m_ref->setExtId(attrib.value("external"));
}

void LinkedTextHandler::endRef()
{
  m_ref->setText(m_curString);
  m_children.append(m_ref);
  printf("LinkedTextHandler: add ref `%s'\n",m_ref->text().data());
  m_ref=0;
}


