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

#include "mainhandler.h"
#include "compoundhandler.h"
#include "dochandler.h"
#include "debug.h"
#include "graphhandler.h"
#include "sectionhandler.h"

class CompoundErrorHandler : public QXmlErrorHandler
{
    public:
      virtual ~CompoundErrorHandler() {}
      bool warning( const QXmlParseException & )
      {
        return FALSE;
      }
      bool error( const QXmlParseException & )
      {
        return FALSE;
      }
      bool fatalError( const QXmlParseException &exception )
      {
        debug(1,"Fatal error at line %d column %d: %s\n",
            exception.lineNumber(),exception.columnNumber(),
            exception.message().data());
        return FALSE;
      }
      QString errorString() { return ""; }

    private:
      QString errorMsg;
};

//----------------------------------------------------------------------------

class CompoundTypeMap
{
  public:
    CompoundTypeMap()
    {
      m_map.setAutoDelete(TRUE);
      m_map.insert("class",new int(ICompound::Class));
      m_map.insert("struct",new int(ICompound::Struct));
      m_map.insert("union",new int(ICompound::Union));
      m_map.insert("interface",new int(ICompound::Interface));
      m_map.insert("exception",new int(ICompound::Exception));
      m_map.insert("namespace",new int(ICompound::Namespace));
      m_map.insert("file",new int(ICompound::File));
      m_map.insert("group",new int(ICompound::Group));
      m_map.insert("page",new int(ICompound::Page));
      m_map.insert("package",new int(ICompound::Package));
    }
    virtual ~CompoundTypeMap()
    {
    }
    ICompound::CompoundKind map(const QString &s)
    {
      int *val = m_map.find(s);
      if (val==0) 
      {
        debug(1,"Warning: `%s' is an invalid compound type\n",s.data());
        return ICompound::Invalid;
      }
      else return (ICompound::CompoundKind)*val;
    }
  private: 
    QDict<int> m_map;
};

static CompoundTypeMap *s_typeMap;

void compoundhandler_init()
{
  s_typeMap = new CompoundTypeMap;
}

void compoundhandler_exit()
{
  delete s_typeMap;
}

//----------------------------------------------------------------------------

CompoundHandler::CompoundHandler(const QString &xmlDir) 
  : m_brief(0), m_detailed(0), m_programListing(0),
    m_xmlDir(xmlDir), m_refCount(1), m_memberDict(257), m_memberNameDict(257),
    m_mainHandler(0), m_inheritanceGraph(0), m_collaborationGraph(0)
{
  m_superClasses.setAutoDelete(TRUE);
  m_subClasses.setAutoDelete(TRUE);
  m_sections.setAutoDelete(TRUE);
  m_memberNameDict.setAutoDelete(TRUE);

  addStartHandler("doxygen");
  addEndHandler("doxygen");
  
  addStartHandler("compounddef",this,&CompoundHandler::startCompound);
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
  
  addStartHandler("location",this,&CompoundHandler::startLocation);
  addEndHandler("location");

  addStartHandler("programlisting",this,&CompoundHandler::startProgramListing);

  addStartHandler("inheritancegraph",this,&CompoundHandler::startInheritanceGraph);

  addStartHandler("collaborationgraph",this,&CompoundHandler::startCollaborationGraph);

}

CompoundHandler::~CompoundHandler()
{
  debug(2,"CompoundHandler::~CompoundHandler()\n");
  delete m_brief;
  delete m_detailed;
  delete m_programListing;
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

void CompoundHandler::startProgramListing(const QXmlAttributes& attrib)
{
  ProgramListingHandler *plHandler = new ProgramListingHandler(this);
  plHandler->startProgramListing(attrib);
  m_programListing = plHandler;
}

void CompoundHandler::startCompound(const QXmlAttributes& attrib)
{
  m_id = attrib.value("id");
  m_kindString = attrib.value("kind");
  m_kind = s_typeMap->map(m_kindString);
  debug(2,"startCompound(id=`%s' type=`%s')\n",m_id.data(),m_kindString.data());
}

void CompoundHandler::endCompound()
{
   debug(2,"endCompound()\n");
}

void CompoundHandler::startLocation(const QXmlAttributes& attrib)
{
  m_defFile = attrib.value("file");
  m_defLine = attrib.value("line").toInt();
}

void CompoundHandler::endCompoundName()
{
  m_name = m_curString.stripWhiteSpace();
  debug(2,"Compound name `%s'\n",m_name.data());
}

void CompoundHandler::addSuperClass(const QXmlAttributes& attrib)
{
  RelatedClass *sc=new RelatedClass(
          attrib.value("refid"),
          attrib.value("prot"),
          attrib.value("virt")
         );
  debug(2,"super class id=`%s' prot=`%s' virt=`%s'\n",
      sc->m_id.data(),
      sc->m_protection.data(),
      sc->m_virtualness.data());
  m_superClasses.append(sc);
}

void CompoundHandler::addSubClass(const QXmlAttributes& attrib)
{
  RelatedClass *sc = new RelatedClass(
          attrib.value("refid"),
          attrib.value("prot"),
          attrib.value("virt")
         );
  debug(2,"sub class id=`%s' prot=`%s' virt=`%s'\n",
      sc->m_id.data(),
      sc->m_protection.data(),
      sc->m_virtualness.data());
  m_subClasses.append(sc);
}

bool CompoundHandler::parseXML(const QString &compId)
{
  QFile xmlFile(m_xmlDir+"/"+compId+".xml");
  if (!xmlFile.exists()) return FALSE;
  CompoundErrorHandler errorHandler;
  QXmlInputSource source( xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( this );
  reader.setErrorHandler( &errorHandler );
  reader.parse( source );
  return TRUE;
}

void CompoundHandler::initialize(MainHandler *mh)
{
  m_mainHandler = mh;
  QListIterator<ISection> msi(m_sections);
  SectionHandler *sec;
  for (;(sec=(SectionHandler *)msi.current());++msi)
  {
    sec->initialize(this);
  }
}

void CompoundHandler::insertMember(MemberHandler *mh)
{
  m_memberDict.insert(mh->id(),mh);
  QList<MemberHandler> *mhl = m_memberNameDict.find(mh->id());
  if (mhl==0)
  {
    mhl = new QList<MemberHandler>;
    m_memberNameDict.insert(mh->name(),mhl);
  }
  mhl->append(mh);
}

void CompoundHandler::release()
{ 
  debug(2,"CompoundHandler::release() %d->%d\n",m_refCount,m_refCount-1);
  if (--m_refCount<=0)
  {
    m_mainHandler->unloadCompound(this);
    delete this; 
  }
}

ISectionIterator *CompoundHandler::sections() const 
{ 
  return new SectionIterator(m_sections); 
}
    
IMemberIterator *CompoundHandler::memberByName(const QString &name) const
{ 
  QList<MemberHandler> *ml = m_memberNameDict[name]; 
  if (ml==0) return 0;
  return new MemberIterator(*ml);
}

void CompoundHandler::startInheritanceGraph(const QXmlAttributes &attrib)
{
  m_inheritanceGraph = new GraphHandler(this,"inheritancegraph");
  m_inheritanceGraph->startGraph(attrib);
}

void CompoundHandler::startCollaborationGraph(const QXmlAttributes &attrib)
{
  m_collaborationGraph = new GraphHandler(this,"collaborationgraph");
  m_collaborationGraph->startGraph(attrib);
}

IDocRoot *CompoundHandler::briefDescription() const 
{ 
  return m_brief; 
}

IDocRoot *CompoundHandler::detailedDescription() const 
{ 
  return m_detailed; 
}

IMember *CompoundHandler::memberById(const QString &id) const 
{ 
  return m_memberDict[id]; 
}

IGraph *CompoundHandler::inheritanceGraph() const 
{ 
  return m_inheritanceGraph; 
}

IGraph *CompoundHandler::collaborationGraph() const 
{ 
  return m_collaborationGraph; 
}

