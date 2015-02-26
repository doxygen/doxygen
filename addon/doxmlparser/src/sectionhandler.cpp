/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
#include "memberhandler.h"
#include "dochandler.h"
#include "debug.h"

class SectionTypeMap
{
  public:
    SectionTypeMap() : m_map(37)
    {
      m_map.setAutoDelete(TRUE);
      m_map.insert("user-defined",new int(ISection::UserDefined));
      m_map.insert("public-type",new int(ISection::PubTypes));
      m_map.insert("public-func",new int(ISection::PubFuncs));
      m_map.insert("public-attrib",new int(ISection::PubAttribs));
      m_map.insert("public-slot",new int(ISection::PubSlots));
      m_map.insert("signal",new int(ISection::Signals));
      m_map.insert("dcop-func",new int(ISection::DCOPFuncs));
      m_map.insert("property",new int(ISection::Properties));
      m_map.insert("event",new int(ISection::Events));
      m_map.insert("public-static-func",new int(ISection::PubStatFuncs));
      m_map.insert("public-static-attrib",new int(ISection::PubStatAttribs));
      m_map.insert("protected-type",new int(ISection::ProTypes));
      m_map.insert("protected-func",new int(ISection::ProFuncs));
      m_map.insert("protected-attrib",new int(ISection::ProAttribs));
      m_map.insert("protected-slot",new int(ISection::ProSlots));
      m_map.insert("protected-static-func",new int(ISection::ProStatFuncs));
      m_map.insert("protected-static-attrib",new int(ISection::ProStatAttribs));
      m_map.insert("package-type",new int(ISection::PacTypes));
      m_map.insert("package-func",new int(ISection::PacFuncs));
      m_map.insert("package-attrib",new int(ISection::PacAttribs));
      m_map.insert("package-static-func",new int(ISection::PacStatFuncs));
      m_map.insert("package-static-attrib",new int(ISection::PacStatAttribs));
      m_map.insert("private-type",new int(ISection::PriTypes));
      m_map.insert("private-func",new int(ISection::PriFuncs));
      m_map.insert("private-attrib",new int(ISection::PriAttribs));
      m_map.insert("private-slot",new int(ISection::PriSlots));
      m_map.insert("private-static-func",new int(ISection::PriStatFuncs));
      m_map.insert("private-static-attrib",new int(ISection::PriStatAttribs));
      m_map.insert("friend",new int(ISection::Friend));
      m_map.insert("related",new int(ISection::Related));
      m_map.insert("define",new int(ISection::Defines));
      m_map.insert("prototype",new int(ISection::Prototypes));
      m_map.insert("typedef",new int(ISection::Typedefs));
      m_map.insert("enum",new int(ISection::Enums));
      m_map.insert("func",new int(ISection::Functions));
      m_map.insert("var",new int(ISection::Variables));
    }
    ISection::SectionKind map(const QString &s)
    {
      int *val = m_map.find(s.utf8());
      if (val==0) 
      {
        debug(1,"Warning: `%s' is an invalid section type\n",s.data());
        return ISection::Invalid;
      }
      else return (ISection::SectionKind)*val;
    }
  private: 
    QDict<int> m_map;
};

static SectionTypeMap *s_typeMap;

void sectionhandler_init()
{
  s_typeMap = new SectionTypeMap;
}

void sectionhandler_exit()
{
  delete s_typeMap;
}

SectionHandler::SectionHandler(IBaseHandler *parent) : m_parent(parent)
{
  //printf("SectionHandler::SectionHandler()\n");
  m_members.setAutoDelete(TRUE);
  addEndHandler("sectiondef",this,&SectionHandler::endSection);
  addStartHandler("memberdef",this,&SectionHandler::startMember);
  addStartHandler("header",this,&SectionHandler::startHeader);
  addEndHandler("header",this,&SectionHandler::endHeader);
  addStartHandler("description",this,&SectionHandler::startDescription);
}

SectionHandler::~SectionHandler()
{
  debug(2,"SectionHandler::~SectionHandler()\n");
}

void SectionHandler::startSection(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_kindString = attrib.value("kind");
  m_kind = s_typeMap->map(m_kindString);
  debug(2,"section kind=`%s'\n",m_kindString.data());
}

void SectionHandler::startDescription(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_description = docHandler;
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

void SectionHandler::startHeader(const QXmlAttributes&)
{
  m_header="";
  m_curString="";
}

void SectionHandler::endHeader()
{
  m_header = m_curString.stripWhiteSpace();
  debug(2,"member header=`%s'\n",m_header.data());
}

void SectionHandler::initialize(CompoundHandler *ch)
{
  QListIterator<MemberHandler> mli(m_members);
  MemberHandler *mh;
  for (;(mh=mli.current());++mli)
  {
    mh->setCompoundHandler(ch);
    ch->insertMember(mh);
    mh->setSectionHandler(this);
  }
}

IDocRoot *SectionHandler::description() const 
{ 
  return m_description; 
}

IMemberIterator *SectionHandler::members() const 
{ 
  return new MemberIterator(m_members); 
}

