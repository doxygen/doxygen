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
  virtual QString memberName() const { return m_name; }
  void initialize(MainHandler *m);

  QString  m_memId;
  QString  m_name;
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
    virtual QString name() const { return m_name; }
    virtual QString initializer() const { return m_initializer; }

    void setName(const QString &name) { m_name=name; }
    void setInitializer(const QString &init) { m_initializer=init; }
    
    virtual ~EnumValueHandler() {}
    EnumValueHandler(IBaseHandler *parent);

  private:
    QString m_name;
    QString m_initializer;
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
    virtual QString kindString() const 
    { return m_kindString; }
    virtual QString id() const 
    { return m_id; }
    virtual QString protection() const 
    { return m_protection; }
    virtual QString virtualness() const 
    { return m_virtualness; }
    virtual QString name() const 
    { return m_name; }
    virtual bool isConst() const 
    { return m_isConst; }
    virtual bool isVolatile() const 
    { return m_isVolatile; }
    virtual ILinkedTextIterator *type() const 
    { return new LinkedTextIterator(m_type); }
    virtual QString typeString() const
    { return LinkedTextHandler::toString(m_type); }
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
    virtual QString definitionFile() const 
    { return m_defFile; }
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
    QString m_kindString;
    QString m_id;
    QString m_protection;
    QString m_virtualness;
    QList<ILinkedText> m_type;
    QList<ILinkedText> m_initializer;
    QList<ILinkedText> m_exception;
    QString m_name;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    QList<IParam> m_params;
    QList<MemberReference> m_references;
    QList<MemberReference> m_referencedBy;
    MemberReference *m_reimplements;
    QList<MemberReference> m_reimplementedBy;
    QString m_defFile;
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
