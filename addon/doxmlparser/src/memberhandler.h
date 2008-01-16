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
    virtual const IString *bodyFile() const
    { return &m_bodyFile; }
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
                                                     // XML elements:
                                                     // -----------------
    QList<ParamHandler> m_templateParams;            // templateparamlist
    QList<LinkedTextImpl> m_type;                    // type
    StringImpl m_definition;                         // definition
    StringImpl m_argsstring;                         // argsstring
    StringImpl m_name;                               // name
    StringImpl m_read;                               // read
    StringImpl m_write;                              // write
    MemberReference *m_reimplements;                 // reimplements
    QList<MemberReference> m_reimplementedBy;        // reimplementedby
    QList<ParamHandler> m_params;                    // param
    QList<MemberHandler> m_enumValues;               // enumvalue
    QList<LinkedTextImpl> m_initializer;             // initializer
    QList<LinkedTextImpl> m_exception;               // exceptions
    DocHandler  *m_brief;                            // briefdescription
    DocHandler  *m_detailed;                         // detaileddescription
    DocHandler  *m_inbody;                           // inbodydescription
                                                     // location
    StringImpl m_defFile;                            // - file
    int m_defLine;                                   // - line
    StringImpl m_bodyFile;                           // - bodyfile
    int m_bodyStart;                                 // - bodystart
    int m_bodyEnd;                                   // - bodyend
    QList<MemberReference> m_references;             // references
    QList<MemberReference> m_referencedBy;           // referencedby

                                                     // XML attributes:
                                                     // ---------------
    MemberKind m_kind;                               // kind
    StringImpl m_kindString;                         // kind as a string
    StringImpl m_id;                                 // id
    StringImpl m_protection;                         // prot
    bool m_isStatic;                                 // static
    bool m_isConst;                                  // const
    bool m_isExplicit;                               // explicit
    bool m_isInline;                                 // inline
    StringImpl m_virtualness;                        // virt
    bool m_isVolatile;                               // volatile
    bool m_isMutable;                                // mutable
    bool m_isReadable;                               // readable
    bool m_isWritable;                               // writable

    CompoundHandler *m_compound;
    SectionHandler *m_section;
    StringImpl m_typeString;
    LinkedTextHandler *m_linkedTextHandler;
    bool m_insideTemplateParamList;
    bool m_hasTemplateParamList;
    IBaseHandler *m_parent;
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
