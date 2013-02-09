/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include "paramhandler.h"
#include "loamhandler.h"
#include "memberhandler.h"

//----------------------------------------------------------------------------

IncludeHandler::IncludeHandler(IBaseHandler *parent,const char *endtag) :
  m_parent(parent)
{
  addEndHandler(endtag,this,&IncludeHandler::endInclude);
}

IncludeHandler::~IncludeHandler()
{
}

void IncludeHandler::startInclude(const QXmlAttributes &attrib)
{
  m_curString = "";
  m_refId     = attrib.value("refid");
  m_isLocal   = attrib.value("local")=="yes";
  m_parent->setDelegate(this);
}

void IncludeHandler::endInclude()
{
  m_name = m_curString;
  m_parent->setDelegate(0);
  debug(2,"Found include %s\n",m_name.data());
}

//----------------------------------------------------------------------------

class CompoundIdIterator : public ICompoundIterator,
                           public QListIterator<QString>
{
  public:
    CompoundIdIterator(const MainHandler *m,const QList<QString> &list) :
      QListIterator<QString>(list), m_mainHandler(m) {}
    virtual ~CompoundIdIterator() {}

    virtual void toFirst()
    { 
      QListIterator<QString>::toFirst(); 
    }
    virtual void toLast()
    { 
      QListIterator<QString>::toLast(); 
    }
    virtual void toNext()
    { 
      QListIterator<QString>::operator++(); 
    }
    virtual void toPrev()
    { 
      QListIterator<QString>::operator--(); 
    }
    virtual ICompound *current() const
    { 
      QString *id = QListIterator<QString>::current(); 
      return id ? m_mainHandler->compoundById(id->utf8()) : 0;
    }
    virtual void release()
    { delete this; }

  private:
    const MainHandler *m_mainHandler;
};

//----------------------------------------------------------------------------

ICompound *RelatedCompound::compound() const 
{ 
  return m_parent->m_mainHandler->compoundById(m_id.utf8()); 
}

//----------------------------------------------------------------------------

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
      m_map.insert("class",    new int(ICompound::Class));
      m_map.insert("struct",   new int(ICompound::Struct));
      m_map.insert("union",    new int(ICompound::Union));
      m_map.insert("interface",new int(ICompound::Interface));
      m_map.insert("protocol", new int(ICompound::Protocol));
      m_map.insert("category", new int(ICompound::Category));
      m_map.insert("exception",new int(ICompound::Exception));
      m_map.insert("file",     new int(ICompound::File));
      m_map.insert("namespace",new int(ICompound::Namespace));
      m_map.insert("group",    new int(ICompound::Group));
      m_map.insert("page",     new int(ICompound::Page));
      m_map.insert("example",  new int(ICompound::Example));
      m_map.insert("dir",      new int(ICompound::Dir));
    }
    virtual ~CompoundTypeMap()
    {
    }
    ICompound::CompoundKind map(const QString &s)
    {
      int *val = m_map.find(s.utf8());
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
  : m_titleHandler(0), 
    m_includeDependencyGraph(0), 
    m_includedByDependencyGraph(0), 
    m_templateParamList(0),
    m_brief(0), 
    m_detailed(0), 
    m_inheritanceGraph(0), 
    m_collaborationGraph(0),
    m_programListing(0),
    m_members(0),
    m_xmlDir(xmlDir), 
    m_refCount(1), 
    m_memberDict(257), 
    m_memberNameDict(257),
    m_mainHandler(0)
{
  m_superClasses.setAutoDelete(TRUE);
  m_subClasses.setAutoDelete(TRUE);
  m_sections.setAutoDelete(TRUE);
  m_memberNameDict.setAutoDelete(TRUE);
  m_innerCompounds.setAutoDelete(TRUE);
  m_includes.setAutoDelete(TRUE);
  m_includedBy.setAutoDelete(TRUE);

  addStartHandler("doxygen");
  addEndHandler("doxygen");
  
  addStartHandler("compounddef",this,&CompoundHandler::startCompound);
  addEndHandler("compounddef",this,&CompoundHandler::endCompound);

  addStartHandler("compoundname");
  addEndHandler("compoundname",this,&CompoundHandler::endCompoundName);

  addStartHandler("title",this,&CompoundHandler::startTitle);

  addStartHandler("basecompoundref",this,&CompoundHandler::startSuperClass);
  addEndHandler("basecompoundref",this,&CompoundHandler::endSuperClass);

  addStartHandler("derivedcompoundref",this,&CompoundHandler::startSubClass);
  addEndHandler("derivedcompoundref",this,&CompoundHandler::endSubClass);

  addStartHandler("includes",this,&CompoundHandler::startIncludes);
  addStartHandler("includedby",this,&CompoundHandler::startIncludedBy);

  addStartHandler("incdepgraph",this,&CompoundHandler::startIncludeDependencyGraph);

  addStartHandler("invincdepgraph",this,&CompoundHandler::startIncludedByDependencyGraph);

  addStartHandler("innerdir",this,&CompoundHandler::startInnerDir);
  addEndHandler("innerdir");

  addStartHandler("innerfile",this,&CompoundHandler::startInnerFile);
  addEndHandler("innerfile");

  addStartHandler("innerclass",this,&CompoundHandler::startInnerClass);
  addEndHandler("innerclass");

  addStartHandler("innernamespace",this,&CompoundHandler::startInnerNamespace);
  addEndHandler("innernamespace");

  addStartHandler("innerpage",this,&CompoundHandler::startInnerPage);
  addEndHandler("innerpage");
  
  addStartHandler("innergroup",this,&CompoundHandler::startInnerGroup);
  addEndHandler("innergroup");

  addStartHandler("templateparamlist",this,&CompoundHandler::startTemplateParamList);

  addStartHandler("sectiondef",this,&CompoundHandler::startSection);

  addStartHandler("briefdescription",this,&CompoundHandler::startBriefDesc);

  addStartHandler("detaileddescription",this,&CompoundHandler::startDetailedDesc);
  
  addStartHandler("inheritancegraph",this,&CompoundHandler::startInheritanceGraph);
  
  addStartHandler("collaborationgraph",this,&CompoundHandler::startCollaborationGraph);

  addStartHandler("programlisting",this,&CompoundHandler::startProgramListing);

  addStartHandler("location",this,&CompoundHandler::startLocation);
  addEndHandler("location");

  addStartHandler("listofallmembers",this,&CompoundHandler::startListOfAllMembers);
}

CompoundHandler::~CompoundHandler()
{
  debug(2,"CompoundHandler::~CompoundHandler()\n");
  delete m_titleHandler;
  delete m_brief;
  delete m_detailed;
  delete m_programListing;
  delete m_inheritanceGraph;
  delete m_collaborationGraph;
  delete m_includeDependencyGraph;
  delete m_includedByDependencyGraph;
  delete m_templateParamList;
  delete m_members;
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

void CompoundHandler::startIncludes(const QXmlAttributes& attrib)
{
  IncludeHandler *inc = new IncludeHandler(this,"includes");
  m_includes.append(inc);
  inc->startInclude(attrib);
}

void CompoundHandler::startIncludedBy(const QXmlAttributes& attrib)
{
  IncludeHandler *inc = new IncludeHandler(this,"includedby");
  m_includedBy.append(inc);
  inc->startInclude(attrib);
}

void CompoundHandler::startCompound(const QXmlAttributes& attrib)
{
  m_id         = attrib.value("id");
  m_kindString = attrib.value("kind");
  m_kind       = s_typeMap->map(m_kindString);
  m_protection = attrib.value("prot");
  debug(2,"startCompound(id=`%s' type=`%s')\n",m_id.data(),m_kindString.data());
}

void CompoundHandler::endCompound()
{
   debug(2,"endCompound()\n");
}

void CompoundHandler::startLocation(const QXmlAttributes& attrib)
{
  m_defFile      = attrib.value("file");
  m_defLine      = attrib.value("line").toInt();
  m_defBodyFile  = attrib.value("bodyfile");
  m_defBodyStart = attrib.value("bodystart").toInt();
  m_defBodyEnd   = attrib.value("bodyend").toInt();
}

void CompoundHandler::endCompoundName()
{
  m_name = m_curString.stripWhiteSpace();
  debug(2,"Compound name `%s'\n",m_name.data());
}

void CompoundHandler::startInnerClass(const QXmlAttributes& attrib)
{
  m_innerCompounds.append(new QString(attrib.value("refid")));
}

void CompoundHandler::startInnerNamespace(const QXmlAttributes& attrib)
{
  m_innerCompounds.append(new QString(attrib.value("refid")));
}

void CompoundHandler::startInnerFile(const QXmlAttributes& attrib)
{
  m_innerCompounds.append(new QString(attrib.value("refid")));
}

void CompoundHandler::startInnerGroup(const QXmlAttributes& attrib)
{
  m_innerCompounds.append(new QString(attrib.value("refid")));
}

void CompoundHandler::startInnerPage(const QXmlAttributes& attrib)
{
  m_innerCompounds.append(new QString(attrib.value("refid")));
}

void CompoundHandler::startInnerDir(const QXmlAttributes& attrib)
{
  m_innerCompounds.append(new QString(attrib.value("refid")));
}

void CompoundHandler::startTemplateParamList(const QXmlAttributes& attrib)
{
  m_templateParamList = new TemplateParamListHandler(this);
  m_templateParamList->startTemplateParamList(attrib);
}

void CompoundHandler::startListOfAllMembers(const QXmlAttributes& attrib)
{
  m_members = new ListOfAllMembersHandler(this);
  m_members->startListOfAllMembers(attrib);
}

void CompoundHandler::startSuperClass(const QXmlAttributes& attrib)
{
  IRelatedCompound::Protection prot = IRelatedCompound::Public;
  QString protString = attrib.value("prot");
  if (protString=="protected") 
  {
    prot = IRelatedCompound::Protected;
  }
  else if (protString=="private") 
  {
    prot = IRelatedCompound::Private;
  }
  IRelatedCompound::Kind kind = IRelatedCompound::Normal;
  QString kindString = attrib.value("virt");
  if (kindString=="virtual") kind = IRelatedCompound::Virtual;
  
  RelatedCompound *sc=new RelatedCompound(
          this,
          attrib.value("refid"),
          prot,
          kind
         );
  debug(2,"super class id=`%s' prot=`%s' virt=`%s'\n",
      attrib.value("refid").data(),
      protString.data(),
      kindString.data());
  m_superClasses.append(sc);
  m_curString = "";
}

void CompoundHandler::endSuperClass()
{
  m_superClasses.getLast()->setName(m_curString);
}

void CompoundHandler::startSubClass(const QXmlAttributes& attrib)
{
  IRelatedCompound::Protection prot = IRelatedCompound::Public;
  QString protString = attrib.value("prot");
  if (protString=="protected") prot = IRelatedCompound::Protected;
  else if (protString=="private") prot = IRelatedCompound::Private;

  IRelatedCompound::Kind kind = IRelatedCompound::Normal;
  QString kindString = attrib.value("virt");
  if (kindString=="virtual") kind = IRelatedCompound::Virtual;
  
  RelatedCompound *sc = new RelatedCompound(
          this,
          attrib.value("refid"),
          prot,
          kind
         );
  debug(2,"sub class id=`%s' prot=`%s' virt=`%s'\n",
      attrib.value("refid").data(),
      protString.data(),
      kindString.data());
  m_subClasses.append(sc);
  m_curString = "";
}

void CompoundHandler::endSubClass()
{
  m_subClasses.getLast()->setName(m_curString);
}

void CompoundHandler::startTitle(const QXmlAttributes& attrib)
{
  ASSERT(m_titleHandler==0);
  m_titleHandler = new TitleHandler(this);
  m_titleHandler->startTitle(attrib);
}

bool CompoundHandler::parseXML(const char *compId)
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
  QListIterator<SectionHandler> msi(m_sections);
  SectionHandler *sec;
  for (;(sec=msi.current());++msi)
  {
    sec->initialize(this);
  }
  if (m_members)
  {
    m_members->initialize(mh);
  }
}

void CompoundHandler::insertMember(MemberHandler *mh)
{
  m_memberDict.insert(mh->id()->latin1(),mh);
  mh->initialize(m_mainHandler);
  QList<MemberHandler> *mhl = m_memberNameDict.find(mh->id()->latin1());
  if (mhl==0)
  {
    mhl = new QList<MemberHandler>;
    m_memberNameDict.insert(mh->name()->latin1(),mhl);
  }
  mhl->append(mh);
}

ICompound *CompoundHandler::toICompound() const
{
  switch (m_kind)
  {
    case ICompound::Class:     return (IClass *)this;
    case ICompound::Struct:    return (IStruct *)this;
    case ICompound::Union:     return (IUnion *)this;
    case ICompound::Interface: return (IInterface *)this;
    case ICompound::Protocol:  return (IClass *)this;
    case ICompound::Category:  return (IClass *)this;
    case ICompound::Exception: return (IException *)this;
    case ICompound::File:      return (IFile *)this;
    case ICompound::Namespace: return (INamespace *)this;
    case ICompound::Group:     return (IGroup *)this;
    case ICompound::Page:      return (IPage *)this;
    case ICompound::Example:   return (IPage *)this;
    case ICompound::Dir:       return (IDir *)this;
    default:   return 0;
  }
  return 0;
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
    
IMemberIterator *CompoundHandler::memberByName(const char *name) const
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

void CompoundHandler::startIncludeDependencyGraph(const QXmlAttributes &attrib)
{
  m_includeDependencyGraph = new GraphHandler(this,"incdepgraph");
  m_includeDependencyGraph->startGraph(attrib);
}

void CompoundHandler::startIncludedByDependencyGraph(const QXmlAttributes &attrib)
{
  m_includedByDependencyGraph = new GraphHandler(this,"invincdepgraph");
  m_includedByDependencyGraph->startGraph(attrib);
}

IDocRoot *CompoundHandler::briefDescription() const 
{ 
  return m_brief; 
}

IDocRoot *CompoundHandler::detailedDescription() const 
{ 
  return m_detailed; 
}

IMember *CompoundHandler::memberById(const char *id) const 
{ 
  return (IFunction*)m_memberDict[id]; 
}

IGraph *CompoundHandler::inheritanceGraph() const 
{ 
  return m_inheritanceGraph; 
}

IGraph *CompoundHandler::collaborationGraph() const 
{ 
  return m_collaborationGraph; 
}

IGraph *CompoundHandler::includeDependencyGraph() const
{
  return m_includeDependencyGraph;
}

IGraph *CompoundHandler::includedByDependencyGraph() const
{
  return m_includedByDependencyGraph;
}

IRelatedCompoundIterator *CompoundHandler::baseCompounds() const
{
  return new RelatedCompoundIterator(m_superClasses);
}

IRelatedCompoundIterator *CompoundHandler::derivedCompounds() const
{
  return new RelatedCompoundIterator(m_subClasses);
}

ICompoundIterator *CompoundHandler::nestedCompounds() const
{
  return new CompoundIdIterator(m_mainHandler,m_innerCompounds);
}

IDocProgramListing *CompoundHandler::source() const
{
  return m_programListing;
}

IIncludeIterator *CompoundHandler::includes() const
{
  return new IncludeIterator(m_includes);
}

IIncludeIterator *CompoundHandler::includedBy() const
{
  return new IncludeIterator(m_includedBy);
}

IParamIterator *CompoundHandler::templateParameters() const
{
  return m_templateParamList ? m_templateParamList->templateParams() : 0;
}

const IDocTitle *CompoundHandler::title() const
{
  return m_titleHandler;
}

IMemberReferenceIterator *CompoundHandler::members() const
{
  return m_members ? m_members->members() : 0;
}


