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

#ifndef _MEMBERHANDLER_H
#define _MEMBERHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>
#include <doxmlintf.h>

#include "basehandler.h"
#include "baseiterator.h"
#include "paramhandler.h"
#include "linkedtexthandler.h"
#include "dochandler.h"

class MainHandler;
class CompoundHandler;
class SectionHandler;

struct MemberReference : public IMemberReference
{
  virtual ~MemberReference() {}
  virtual IMember *member() const;
  virtual const IString *memberName() const { return &m_name; }
  void initialize(MainHandler *m);

  QString      m_memId;
  StringImpl   m_name;
  MainHandler *m_mainHandler;
};

class MemberReferenceIterator : public BaseIterator<IMemberReferenceIterator,IMemberReference,MemberReference>
{
  public:
    MemberReferenceIterator(const QList<MemberReference> &list) : 
      BaseIterator<IMemberReferenceIterator,IMemberReference,MemberReference>(list) {}
};

class EnumValueHandler : public IEnumValue, public BaseHandler<EnumValueHandler>
{
  public:
    virtual void startName(const QXmlAttributes& attrib);
    virtual void endName();
    virtual void startInitializer(const QXmlAttributes& attrib);
    virtual void endInitializer();
    virtual void startEnumValue(const QXmlAttributes& attrib);
    virtual void endEnumValue();

    // IEnumValue
    virtual const IString *name() const { return &m_name; }
    virtual const IString *initializer() const { return &m_initializer; }

    void setName(const QString &name) { m_name=name; }
    void setInitializer(const QString &init) { m_initializer=init; }
    
    virtual ~EnumValueHandler() {}
    EnumValueHandler(IBaseHandler *parent);

  private:
    StringImpl m_name;
    StringImpl m_initializer;
    IBaseHandler *m_parent;
};

class EnumValueIterator : public BaseIterator<IEnumValueIterator,IEnumValue,EnumValueHandler>
{
  public:
    EnumValueIterator(const QList<EnumValueHandler> &list) : 
      BaseIterator<IEnumValueIterator,IEnumValue,EnumValueHandler>(list) {}
};


class MemberHandler : public IMember, public BaseHandler<MemberHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void endMember();
    virtual void startParam(const QXmlAttributes& attrib);
    virtual void startType(const QXmlAttributes& attrib);
    virtual void endName();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);
    virtual void startLocation(const QXmlAttributes& attrib);
    virtual void startReferences(const QXmlAttributes& attrib);
    virtual void endReferences();
    virtual void startReferencedBy(const QXmlAttributes& attrib);
    virtual void endReferencedBy();
    virtual void startReimplements(const QXmlAttributes& attrib);
    virtual void endReimplements();
    virtual void startReimplementedBy(const QXmlAttributes& attrib);
    virtual void endReimplementedBy();
    virtual void startInitializer(const QXmlAttributes& attrib);
    virtual void startException(const QXmlAttributes& attrib);
    virtual void startEnumValue(const QXmlAttributes& attrib);

    MemberHandler(IBaseHandler *parent);
    virtual ~MemberHandler();

    // IMember implementation
    virtual ICompound *compound() const;
    virtual ISection *section() const;
    virtual MemberKind kind() const
    { return m_kind; }
    virtual const IString *kindString() const 
    { return &m_kindString; }
    virtual const IString *id() const 
    { return &m_id; }
    virtual const IString *protection() const 
    { return &m_protection; }
    virtual const IString *virtualness() const 
    { return &m_virtualness; }
    virtual const IString *name() const 
    { return &m_name; }
    virtual bool isConst() const 
    { return m_isConst; }
    virtual bool isVolatile() const 
    { return m_isVolatile; }
    virtual ILinkedTextIterator *type() const 
    { return new LinkedTextIterator(m_type); }
    virtual const IString *typeString() const
    { MemberHandler *that = (MemberHandler *)this;
      that->m_typeString = LinkedTextHandler::toString(m_type); 
      return &m_typeString;
    }
    virtual IParamIterator *params() const 
    { return new ParamIterator(m_params); }
    virtual IMemberReferenceIterator *references() const 
    { return new MemberReferenceIterator(m_references); }
    virtual IMemberReferenceIterator *referencedBy() const 
    { return new MemberReferenceIterator(m_referencedBy); }
    virtual ILinkedTextIterator *initializer() const
    { return new LinkedTextIterator(m_initializer); }
    virtual ILinkedTextIterator *exceptions() const
    { return new LinkedTextIterator(m_exception); }
    virtual int bodyStart() const 
    { return m_bodyStart; }
    virtual int bodyEnd() const 
    { return m_bodyEnd; }
    virtual const IString *definitionFile() const 
    { return &m_defFile; }
    virtual int definitionLine() const 
    { return m_defLine; }
    virtual IMemberReference *reimplements() const
    { return m_reimplements; }
    virtual IMemberReferenceIterator *reimplementedBy() const
    { return new MemberReferenceIterator(m_reimplementedBy); }
    virtual IEnumValueIterator *enumValues() const
    { return new EnumValueIterator(m_enumValues); }
    virtual IDocRoot *briefDescription() const
    { return m_brief; }
    virtual IDocRoot *detailedDescription() const
    { return m_detailed; }

    void initialize(MainHandler *m);
    void setCompoundHandler(CompoundHandler *c);
    void setSectionHandler(SectionHandler *s);

  private:
    IBaseHandler *m_parent;
    CompoundHandler *m_compound;
    SectionHandler *m_section;
    MemberKind m_kind;
    StringImpl m_kindString;
    StringImpl m_id;
    StringImpl m_protection;
    StringImpl m_virtualness;
    StringImpl m_typeString;
    QList<LinkedTextImpl> m_type;
    QList<LinkedTextImpl> m_initializer;
    QList<LinkedTextImpl> m_exception;
    StringImpl m_name;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    QList<ParamHandler> m_params;
    QList<MemberReference> m_references;
    QList<MemberReference> m_referencedBy;
    MemberReference *m_reimplements;
    QList<MemberReference> m_reimplementedBy;
    StringImpl m_defFile;
    int m_defLine;
    int m_bodyStart;
    int m_bodyEnd;
    bool m_isConst;
    bool m_isVolatile;
    LinkedTextHandler *m_linkedTextHandler;
    QList<EnumValueHandler> m_enumValues;
};

class MemberIterator : public BaseIterator<IMemberIterator,IMember,MemberHandler>
{
  public:
    MemberIterator(const QList<MemberHandler> &list) : 
      BaseIterator<IMemberIterator,IMember,MemberHandler>(list) {}
};

void memberhandler_init();
void memberhandler_exit();

#endif
