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

#include "mainhandler.h"
#include "compoundhandler.h"
#include "sectionhandler.h"

SectionHandler::SectionHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_members.setAutoDelete(TRUE);
  addEndHandler("sectiondef",this,&SectionHandler::endSection);
  addStartHandler("memberdef",this,&SectionHandler::startMember);
}

SectionHandler::~SectionHandler()
{
}

void SectionHandler::startSection(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_kind = attrib.value("kind");
  printf("section kind=`%s'\n",m_kind.data());
}

void SectionHandler::endSection()
{
  m_parent->setDelegate(0);
}

void SectionHandler::startMember(const QXmlAttributes& attrib)
{
  MemberHandler *memHandler = new MemberHandler(this);
  memHandler->startMember(attrib);
  m_members.append(memHandler);
}

void SectionHandler::initialize(MainHandler *m)
{
  QListIterator<IMember> mli(m_members);
  MemberHandler *mh;
  for (;(mh=(MemberHandler *)mli.current());++mli)
  {
    m->insertMemberById(mh->name(),mh);
    m->insertMemberByName(mh->name(),mh);
  }
}


