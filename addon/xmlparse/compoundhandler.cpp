#include "mainhandler.h"
#include "compoundhandler.h"
#include "dochandler.h"

CompoundHandler::CompoundHandler(IBaseHandler *parent) 
  : m_parent(parent), m_brief(0), m_detailed(0)
{
  m_superClasses.setAutoDelete(TRUE);

  addEndHandler("compounddef",this,&CompoundHandler::endCompound);

  addStartHandler("compoundname");
  addEndHandler("compoundname",this,&CompoundHandler::endCompoundName);

  addStartHandler("derivedcompoundref",this,&CompoundHandler::addSubClass);
  addEndHandler("derivedcompoundref");

  addStartHandler("basecompoundref",this,&CompoundHandler::addSuperClass);
  addEndHandler("basecompoundref");

  addStartHandler("briefdescription",this,&CompoundHandler::startBriefDesc);

  addStartHandler("detaileddescription",this,&CompoundHandler::startDetailedDesc);
  
  addStartHandler("sectiondef",this,&CompoundHandler::startSection);
}

CompoundHandler::~CompoundHandler()
{
}

void CompoundHandler::startSection(const QXmlAttributes& attrib)
{
  SectionHandler *sectHandler = new SectionHandler(this);
  sectHandler->startSection(attrib);
  m_sections.append(sectHandler);
}

void CompoundHandler::startBriefDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_brief = docHandler;
}

void CompoundHandler::startDetailedDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_detailed = docHandler;
}

void CompoundHandler::startCompound(const QXmlAttributes& attrib)
{
   m_parent->setDelegate(this);
   m_id = attrib.value("id");
   m_kind = attrib.value("kind");
   printf("startCompound(id=`%s' type=`%s')\n",m_id.data(),m_kind.data());
}

void CompoundHandler::endCompound()
{
   printf("endCompound()\n");
   m_parent->setDelegate(0);
}

void CompoundHandler::endCompoundName()
{
  m_name = m_curString.stripWhiteSpace();
  printf("Compound name `%s'\n",m_name.data());
}

void CompoundHandler::addSuperClass(const QXmlAttributes& attrib)
{
  SuperClass *sc=new SuperClass(
          attrib.value("idref"),
          attrib.value("prot"),
          attrib.value("virt")
         );
  printf("super class id=`%s' prot=`%s' virt=`%s'\n",
      sc->m_id.data(),
      sc->m_protection.data(),
      sc->m_virtualness.data());
  m_superClasses.append(sc);
}

void CompoundHandler::addSubClass(const QXmlAttributes& attrib)
{
  SubClass *sc = new SubClass(
          attrib.value("idref"),
          attrib.value("prot"),
          attrib.value("virt")
         );
  printf("sub class id=`%s' prot=`%s' virt=`%s'\n",
      sc->m_id.data(),
      sc->m_protection.data(),
      sc->m_virtualness.data());
  m_subClasses.append(sc);
}

