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

#include "memberhandler.h"
#include "sectionhandler.h"
#include "dochandler.h"
#include "mainhandler.h"
#include "linkedtexthandler.h"

//------------------------------------------------------------------------------

void MemberReference::initialize(MainHandler *mh)
{
  m_mainHandler = mh;
}

IMember *MemberReference::member() const
{
  return m_mainHandler->memberById(m_memId);
}

//------------------------------------------------------------------------------


EnumValueHandler::EnumValueHandler(IBaseHandler *parent) : m_parent(parent)
{
  addEndHandler("enumvalue",this,&EnumValueHandler::endEnumValue);

  addStartHandler("name",this,&EnumValueHandler::startName);
  addEndHandler("name",this,&EnumValueHandler::endName);
  addStartHandler("initializer",this,&EnumValueHandler::startInitializer);
  addEndHandler("initializer",this,&EnumValueHandler::endInitializer);
}

void EnumValueHandler::startEnumValue(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this); 
}

void EnumValueHandler::endEnumValue()
{
  m_parent->setDelegate(0); 
}

void EnumValueHandler::startName(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void EnumValueHandler::endName()
{
  m_name = m_curString;
}

void EnumValueHandler::startInitializer(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void EnumValueHandler::endInitializer()
{
  m_initializer = m_curString;
}

//------------------------------------------------------------------------------

MemberHandler::MemberHandler(IBaseHandler *parent)
  : m_parent(parent), m_brief(0), m_detailed(0)
{
  addEndHandler("memberdef",this,&MemberHandler::endMember);

  addStartHandler("type",this,&MemberHandler::startType);
  addStartHandler("initializer",this,&MemberHandler::startInitializer);
  addStartHandler("exception",this,&MemberHandler::startException);
  addStartHandler("enumvalue",this,&MemberHandler::startEnumValue);

  addStartHandler("name");
  addEndHandler("name",this,&MemberHandler::endName);

  addStartHandler("param",this,&MemberHandler::startParam);

  addStartHandler("briefdescription",this,&MemberHandler::startBriefDesc);

  addStartHandler("detaileddescription",this,&MemberHandler::startDetailedDesc);

  addStartHandler("references",this,&MemberHandler::startReferences);
  addEndHandler("references",this,&MemberHandler::endReferences);
  
  addStartHandler("referencedby",this,&MemberHandler::startReferencedBy);
  addEndHandler("referencedby",this,&MemberHandler::endReferencedBy);

  addStartHandler("reimplements",this,&MemberHandler::startReimplements);
  addEndHandler("reimplements",this,&MemberHandler::endReimplements);
  
  addStartHandler("reimplementedby",this,&MemberHandler::startReimplementedBy);
  addEndHandler("reimplementedby",this,&MemberHandler::endReimplementedBy);

  addStartHandler("location",this,&MemberHandler::startLocation);
  addEndHandler("location");

  m_params.setAutoDelete(TRUE);
  m_references.setAutoDelete(TRUE);
  m_referencedBy.setAutoDelete(TRUE);
  m_reimplements = 0;
  m_reimplementedBy.setAutoDelete(TRUE);
  m_enumValues.setAutoDelete(TRUE);
  m_linkedTextHandler = 0;
  m_defLine=0;
  m_bodyStart=0;
  m_bodyEnd=0;
}

MemberHandler::~MemberHandler()
{
  delete m_brief;
  delete m_detailed;
  delete m_linkedTextHandler;
}

void MemberHandler::startMember(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_kind = attrib.value("kind");
  m_id = attrib.value("id");
  m_virtualness = attrib.value("virt");
  m_protection = attrib.value("prot");
  m_isConst = attrib.value("const")=="yes";
  m_isVolatile = attrib.value("volatile")=="yes";
  printf("member kind=`%s' id=`%s' prot=`%s' virt=`%s'\n",
      m_kind.data(),m_id.data(),m_protection.data(),m_virtualness.data());
}

void MemberHandler::startBriefDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_brief = docHandler;
}

void MemberHandler::startDetailedDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_detailed = docHandler;
}

void MemberHandler::startLocation(const QXmlAttributes& attrib)
{
  m_defFile = attrib.value("file");
  QCString s;
  s = attrib.value("line");
  if (!s.isEmpty()) m_defLine=s.toInt();
  s = attrib.value("bodystart");
  if (!s.isEmpty()) m_bodyStart=s.toInt();
  s = attrib.value("bodyend");
  if (!s.isEmpty()) m_bodyEnd=s.toInt();
}

void MemberHandler::startReferences(const QXmlAttributes& attrib)
{
  MemberReference *mr = new MemberReference;
  mr->m_memId = attrib.value("id");
  m_references.append(mr);
  m_curString="";
}

void MemberHandler::endReferences()
{
  m_references.getLast()->m_name = m_curString;
}

void MemberHandler::startReferencedBy(const QXmlAttributes& attrib)
{
  MemberReference *mr = new MemberReference;
  mr->m_memId = attrib.value("id");
  m_referencedBy.append(mr);
  m_curString="";
}

void MemberHandler::endReferencedBy()
{
  m_referencedBy.getLast()->m_name = m_curString;
}

void MemberHandler::startReimplements(const QXmlAttributes& attrib)
{
  m_reimplements = new MemberReference;
  m_reimplements->m_memId = attrib.value("id");
  m_curString="";
}

void MemberHandler::endReimplements()
{
  m_reimplements->m_name = m_curString;
}

void MemberHandler::startReimplementedBy(const QXmlAttributes& attrib)
{
  MemberReference *mr = new MemberReference;
  mr->m_memId = attrib.value("id");
  m_reimplementedBy.append(mr);
  m_curString="";
}

void MemberHandler::endReimplementedBy()
{
  m_reimplementedBy.getLast()->m_name = m_curString;
}

void MemberHandler::endMember()
{
  m_parent->setDelegate(0);
}

void MemberHandler::startType(const QXmlAttributes &)
{
  printf("startType!\n");
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_type);
  m_linkedTextHandler->start("type");
}

void MemberHandler::startInitializer(const QXmlAttributes &)
{
  printf("startInitializer!\n");
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_initializer);
  m_linkedTextHandler->start("initializer");
}

void MemberHandler::startException(const QXmlAttributes &)
{
  printf("startException!\n");
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_exception);
  m_linkedTextHandler->start("exception");
}

void MemberHandler::endName()
{
  m_name = m_curString.stripWhiteSpace();
  printf("member name=`%s'\n",m_name.data());
}

void MemberHandler::startParam(const QXmlAttributes& attrib)
{
  ParamHandler *paramHandler = new ParamHandler(this);
  paramHandler->startParam(attrib);
  m_params.append(paramHandler);
}

void MemberHandler::startEnumValue(const QXmlAttributes& attrib)
{
  EnumValueHandler *evh = new EnumValueHandler(this);
  evh->startEnumValue(attrib);
  m_enumValues.append(evh);
}

void MemberHandler::initialize(MainHandler *mh)
{
  {
    QListIterator<MemberReference> mli(m_references);
    MemberReference *mr;
    for (;(mr=mli.current());++mli)
    {
      mr->initialize(mh);
    }
  }
  {
    QListIterator<MemberReference> mli(m_referencedBy);
    MemberReference *mr;
    for (;(mr=mli.current());++mli)
    {
      mr->initialize(mh);
    }
  }
  {
    QListIterator<MemberReference> mli(m_reimplementedBy);
    MemberReference *mr;
    for (;(mr=mli.current());++mli)
    {
      mr->initialize(mh);
    }
  }
  if (m_reimplements) m_reimplements->initialize(mh);
}

