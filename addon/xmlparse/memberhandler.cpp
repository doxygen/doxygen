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

MemberHandler::MemberHandler(IBaseHandler *parent)
  : m_parent(parent), m_brief(0), m_detailed(0)
{
  addEndHandler("memberdef",this,&MemberHandler::endMember);

  addStartHandler("type");
  addEndHandler("type",this,&MemberHandler::endType);

  addStartHandler("name");
  addEndHandler("name",this,&MemberHandler::endName);

  addStartHandler("param",this,&MemberHandler::startParam);

  addStartHandler("briefdescription",this,&MemberHandler::startBriefDesc);

  addStartHandler("detaileddescription",this,&MemberHandler::startDetailedDesc);

  addStartHandler("location",this,&MemberHandler::startLocation);
  addEndHandler("location");

  addStartHandler("references",this,&MemberHandler::startReferences);
  addEndHandler("references",this,&MemberHandler::endReferences);
  
  addStartHandler("referencedby",this,&MemberHandler::startReferencedBy);
  addEndHandler("referencedby",this,&MemberHandler::endReferencedBy);

  m_params.setAutoDelete(TRUE);
  m_references.setAutoDelete(TRUE);
  m_referencedBy.setAutoDelete(TRUE);
  
}

MemberHandler::~MemberHandler()
{
  delete m_brief;
  delete m_detailed;
}

void MemberHandler::startMember(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_kind = attrib.value("kind");
  m_id = attrib.value("id");
  m_protection = attrib.value("prot");
  m_virtualness = attrib.value("virtualness");
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
  m_defLine = attrib.value("line").toInt();
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

void MemberHandler::endMember()
{
  m_parent->setDelegate(0);
}

void MemberHandler::endType()
{
  m_type = m_curString.stripWhiteSpace();
  printf("member type=`%s'\n",m_type.data());
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


