/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
#include "stringimpl.h"

class MainHandler;
class CompoundHandler;
class SectionHandler;
class ParamHandler;
class LinkedTextImpl;
class LinkedTextHandler;
class DocHandler;

class MemberReference : public IMemberReference
{
  public:
    virtual ~MemberReference() {}
    virtual IMember *member() const;
    virtual const IString *name() const { return &m_name; }
    virtual const IString *scope() const { return &m_scope; }
    virtual const IString *protection() const { return &m_protection; }
    virtual const IString *virtualness() const { return &m_virtualness; }
    virtual const IString *ambiguityScope() const { return &m_ambiguityScope; }
    void initialize(MainHandler *m);

    QString      m_memId;
    StringImpl   m_name;
    StringImpl   m_scope;
    StringImpl   m_virtualness;
    StringImpl   m_protection;
    StringImpl   m_ambiguityScope;
    MainHandler *m_mainHandler;
};

class MemberReferenceIterator : public BaseIterator<IMemberReferenceIterator,IMemberReference,MemberReference>
{
  public:
    MemberReferenceIterator(const QList<MemberReference> &list) : 
      BaseIterator<IMemberReferenceIterator,IMemberReference,MemberReference>(list) {}
};


#if 0
class EnumValueHandler : public IEnumValue, public BaseHandler<EnumValueHandler>
{
  public:
    virtual void startName(const QXmlAttributes& attrib);
    virtual void endName();
    virtual void startInitializer(const QXmlAttributes& attrib);
    virtual void endInitializer();
    virtual void startEnumValue(const QXmlAttributes& attrib);
    virtual void endEnumValue();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);

    // IEnumValue
    virtual const IString *name() const { return &m_name; }
    virtual ILinkedTextIterator *initializer() const
    { return new LinkedTextIterator(m_initializer); }

    void setName(const QString &name) { m_name=name; }
    
    virtual ~EnumValueHandler();
    EnumValueHandler(IBaseHandler *parent);

    virtual IDocRoot *briefDescription() const
    { return m_brief; }
    virtual IDocRoot *detailedDescription() const
    { return m_detailed; }

  private:
    StringImpl m_name;
    QList<LinkedTextImpl> m_initializer;
    IBaseHandler *m_parent;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    LinkedTextHandler *m_linkedTextHandler;
};

class EnumValueIterator : public BaseIterator<IEnumValueIterator,IEnumValue,EnumValueHandler>
{
  public:
    EnumValueIterator(const QList<EnumValueHandler> &list) : 
      BaseIterator<IEnumValueIterator,IEnumValue,EnumValueHandler>(list) {}
};
#endif


class MemberHandler : public IDefine,
                      public IProperty, 
                      public IVariable,
                      public ITypedef,
                      public IFunction,
                      public ISignal,
                      public IPrototype,
                      public IFriend,
                      public IDCOP,
                      public ISlot,
                      public IEnum,
                      public IEnumValue,
                      public BaseHandler<MemberHandler>
{
  public:
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void endMember();
    virtual void startParam(const QXmlAttributes& attrib);
    virtual void startType(const QXmlAttributes& attrib);
    virtual void startName(const QXmlAttributes& attrib);
    virtual void endName();
    virtual void startRead(const QXmlAttributes& attrib);
    virtual void endRead();
    virtual void startWrite(const QXmlAttributes& attrib);
    virtual void endWrite();
    virtual void startDefinition(const QXmlAttributes& attrib);
    virtual void endDefinition();
    virtual void startArgsString(const QXmlAttributes& attrib);
    virtual void endArgsString();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);
    virtual void startInbodyDesc(const QXmlAttributes& attrib);
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
    virtual void startEnumValue2(const QXmlAttributes& attrib);
    virtual void startTemplateParamList(const QXmlAttributes &attrib);
    virtual void endTemplateParamList();

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
    virtual const IString *readAccessor() const 
    { return &m_read; }
    virtual const IString *writeAccessor() const 
    { return &m_write; }
    virtual const IString *definition() const 
    { return &m_definition; }
    virtual const IString *argsstring() const 
    { return &m_argsstring; }
    virtual bool isConst() const 
    { return m_isConst; }
    virtual bool isVolatile() const 
    { return m_isVolatile; }
    virtual bool isStatic() const 
    { return m_isStatic; }
    virtual bool isExplicit() const 
    { return m_isExplicit; }
    virtual bool isInline() const 
    { return m_isInline; }
    virtual bool isMutable() const 
    { return m_isMutable; }
    virtual bool isReadable() const 
    { return m_isReadable; }
    virtual bool isWritable() const 
    { return m_isWritable; }
    virtual ILinkedTextIterator *type() const;
    virtual const IString *typeString() const;
    virtual IParamIterator *parameters() const;
    virtual IParamIterator *templateParameters() const;
    virtual IMemberReferenceIterator *references() const;
    virtual IMemberReferenceIterator *referencedBy() const;
    virtual ILinkedTextIterator *initializer() const;
    virtual ILinkedTextIterator *exceptions() const;
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
    virtual IMemberReferenceIterator *reimplementedBy() const;
    virtual IDocRoot *briefDescription() const;
    virtual IDocRoot *detailedDescription() const;
    virtual IDocRoot *inbodyDescription() const;

    // IEnum
    virtual IMemberIterator *enumValues() const;
    
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
    StringImpl m_read;
    StringImpl m_write;
    StringImpl m_definition;
    StringImpl m_argsstring;
    DocHandler  *m_brief;
    DocHandler  *m_detailed;
    DocHandler  *m_inbody;
    QList<ParamHandler> m_params;
    QList<ParamHandler> m_templateParams;
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
    QList<MemberHandler> m_enumValues;
    bool m_insideTemplateParamList;
    bool m_hasTemplateParamList;
    bool m_isStatic;
    bool m_isExplicit;
    bool m_isInline;
    bool m_isMutable;
    bool m_isReadable;
    bool m_isWritable;
};

class MemberIterator : public BaseIteratorVia<IMemberIterator,
                                        IMember,
                                        MemberHandler,
                                        IFunction>
{
  public:
    MemberIterator(const QList<MemberHandler> &list) : 
      BaseIteratorVia<IMemberIterator,IMember,MemberHandler,IFunction>(list) {}
};

void memberhandler_init();
void memberhandler_exit();

#endif
