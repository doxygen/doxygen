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


