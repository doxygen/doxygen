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

#include "memberhandler.h"
#include "sectionhandler.h"
#include "dochandler.h"
#include "mainhandler.h"
#include "linkedtexthandler.h"
#include "paramhandler.h"
#include "compoundhandler.h"
#include "debug.h"

//------------------------------------------------------------------------------

class MemberTypeMap
{
  public:
    MemberTypeMap()
    {
      m_map.setAutoDelete(TRUE);
      m_map.insert("define",new int(IMember::Define));
      m_map.insert("property",new int(IMember::Property));
      m_map.insert("variable",new int(IMember::Variable));
      m_map.insert("typedef",new int(IMember::Typedef));
      m_map.insert("enum",new int(IMember::Enum));
      m_map.insert("function",new int(IMember::Function));
      m_map.insert("signal",new int(IMember::Signal));
      m_map.insert("prototype",new int(IMember::Prototype));
      m_map.insert("friend",new int(IMember::Friend));
      m_map.insert("dcop",new int(IMember::DCOP));
      m_map.insert("slot",new int(IMember::Slot));
      m_map.insert("enumvalue",new int(IMember::EnumValue));
    }
    IMember::MemberKind map(const QString &s)
    {
      int *val = m_map.find(s.utf8());
      if (val==0) 
      {
        debug(1,"Warning: `%s' is an invalid member type\n",s.data());
        return IMember::Invalid;
      }
      else return (IMember::MemberKind)*val;
    }
  private: 
    QDict<int> m_map;
};

static MemberTypeMap *s_typeMap;

void memberhandler_init()
{
  s_typeMap = new MemberTypeMap;
}

void memberhandler_exit()
{
  delete s_typeMap;
}

//------------------------------------------------------------------------------

void MemberReference::initialize(MainHandler *mh)
{
  m_mainHandler = mh;
}

IMember *MemberReference::member() const
{
  //return m_mainHandler->memberById(m_memId);
  return 0;
}

//------------------------------------------------------------------------------


#if 0
EnumValueHandler::EnumValueHandler(IBaseHandler *parent) : 
  m_parent(parent), m_brief(0), m_detailed(0), m_linkedTextHandler(0)
{
  addEndHandler("enumvalue",this,&EnumValueHandler::endEnumValue);

  addStartHandler("name",this,&EnumValueHandler::startName);
  addEndHandler("name",this,&EnumValueHandler::endName);
  addStartHandler("initializer",this,&EnumValueHandler::startInitializer);

  addStartHandler("briefdescription",this,&EnumValueHandler::startBriefDesc);

  addStartHandler("detaileddescription",this,&EnumValueHandler::startDetailedDesc);

  m_initializer.setAutoDelete(TRUE);
}

EnumValueHandler::~EnumValueHandler()
{
  delete m_brief;
  delete m_detailed;
  delete m_linkedTextHandler;
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
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_initializer);
  m_linkedTextHandler->start("initializer");
}

void EnumValueHandler::startBriefDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_brief = docHandler;
}

void EnumValueHandler::startDetailedDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_detailed = docHandler;
}
#endif

//------------------------------------------------------------------------------

MemberHandler::MemberHandler(IBaseHandler *parent)
  : m_brief(0), m_detailed(0), m_inbody(0), 
    m_compound(0), m_section(0), m_parent(parent)
{
  //printf("MemberHandler::MemberHandler() %p\n",this);
  addEndHandler("memberdef",this,&MemberHandler::endMember);

  addStartHandler("templateparamlist",this,&MemberHandler::startTemplateParamList);
  addEndHandler("templateparamlist",this,&MemberHandler::endTemplateParamList);

  addStartHandler("type",this,&MemberHandler::startType);

  addStartHandler("definition",this,&MemberHandler::startDefinition);
  addEndHandler("definition",this,&MemberHandler::endDefinition);

  addStartHandler("argsstring",this,&MemberHandler::startArgsString);
  addEndHandler("argsstring",this,&MemberHandler::endArgsString);

  addStartHandler("name",this,&MemberHandler::startName);
  addEndHandler("name",this,&MemberHandler::endName);

  addStartHandler("read",this,&MemberHandler::startRead);
  addEndHandler("read",this,&MemberHandler::endRead);

  addStartHandler("write",this,&MemberHandler::startWrite);
  addEndHandler("write",this,&MemberHandler::endWrite);

  addStartHandler("reimplements",this,&MemberHandler::startReimplements);
  addEndHandler("reimplements",this,&MemberHandler::endReimplements);
  
  addStartHandler("reimplementedby",this,&MemberHandler::startReimplementedBy);
  addEndHandler("reimplementedby",this,&MemberHandler::endReimplementedBy);

  addStartHandler("param",this,&MemberHandler::startParam);

  addStartHandler("enumvalue",this,&MemberHandler::startEnumValue2);
  addEndHandler("enumvalue",this,&MemberHandler::endMember);

  addStartHandler("initializer",this,&MemberHandler::startInitializer);
  addStartHandler("exceptions",this,&MemberHandler::startException);

  addStartHandler("briefdescription",this,&MemberHandler::startBriefDesc);

  addStartHandler("detaileddescription",this,&MemberHandler::startDetailedDesc);

  addStartHandler("inbodydescription",this,&MemberHandler::startInbodyDesc);
  
  addStartHandler("location",this,&MemberHandler::startLocation);
  addEndHandler("location");

  addStartHandler("references",this,&MemberHandler::startReferences);
  addEndHandler("references",this,&MemberHandler::endReferences);
  
  addStartHandler("referencedby",this,&MemberHandler::startReferencedBy);
  addEndHandler("referencedby",this,&MemberHandler::endReferencedBy);

  m_type.setAutoDelete(TRUE);
  m_initializer.setAutoDelete(TRUE);
  m_exception.setAutoDelete(TRUE);
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
  m_insideTemplateParamList=FALSE;
  m_hasTemplateParamList=FALSE;
}

MemberHandler::~MemberHandler()
{
  debug(2,"MemberHandler::~MemberHandler() %p\n",this);
  delete m_brief;
  delete m_detailed;
  delete m_inbody;
  delete m_linkedTextHandler;
  delete m_reimplements;
}

void MemberHandler::startMember(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_kindString = attrib.value("kind");
  //printf("startMember kindString=`%s'\n",m_kindString.data());
  m_kind = s_typeMap->map(m_kindString);
  m_id = attrib.value("id");
  m_protection = attrib.value("prot");
  m_isStatic = attrib.value("static")=="yes";
  m_isConst = attrib.value("const")=="yes";
  m_isExplicit = attrib.value("explicit")=="yes";
  m_isInline = attrib.value("inline")=="yes";
  m_virtualness = attrib.value("virt");
  m_isVolatile = attrib.value("volatile")=="yes";
  m_isMutable = attrib.value("mutable")=="yes";
  m_isReadable = attrib.value("readable")=="yes";
  m_isWritable = attrib.value("writable")=="yes";

  debug(2,"member kind=`%s' id=`%s' prot=`%s' virt=`%s'\n",
      m_kindString.data(),m_id.data(),m_protection.data(),m_virtualness.data());
}

void MemberHandler::startEnumValue(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_kindString = "enumvalue";
  //printf("startEnumValue kindString=`%s'\n",m_kindString.data());
  m_kind = s_typeMap->map(m_kindString);
  m_id = attrib.value("id");
  m_protection = attrib.value("prot");
  m_isStatic = FALSE;
  m_isConst = FALSE;
  m_isExplicit = FALSE;
  m_isInline = FALSE;
  m_virtualness = "non-virtual";
  m_isVolatile = FALSE;
  m_isMutable = FALSE;
  m_isReadable = FALSE;
  m_isWritable = FALSE;
  debug(2,"member kind=`%s' id=`%s' prot=`%s' virt=`%s'\n",
      m_kindString.data(),m_id.data(),m_protection.data(),m_virtualness.data());
}

void MemberHandler::startEnumValue2(const QXmlAttributes& attrib)
{
  MemberHandler *mh = new MemberHandler(this);
  mh->startEnumValue(attrib);
  m_enumValues.append(mh);
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

void MemberHandler::startInbodyDesc(const QXmlAttributes& attrib)
{
  DocHandler *docHandler = new DocHandler(this);
  docHandler->startDoc(attrib);
  m_inbody = docHandler;
}

void MemberHandler::startLocation(const QXmlAttributes& attrib)
{
  m_defFile = attrib.value("file");
  m_bodyFile = attrib.value("bodyfile");
  QString s;
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
  mr->m_memId = attrib.value("refid");
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
  mr->m_memId = attrib.value("refid");
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
  m_reimplements->m_memId = attrib.value("refid");
  m_curString="";
}

void MemberHandler::endReimplements()
{
  m_reimplements->m_name = m_curString;
}

void MemberHandler::startReimplementedBy(const QXmlAttributes& attrib)
{
  MemberReference *mr = new MemberReference;
  mr->m_memId = attrib.value("refid");
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
  debug(2,"startType!\n");
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_type);
  m_linkedTextHandler->start("type");
}

void MemberHandler::startInitializer(const QXmlAttributes &)
{
  debug(2,"startInitializer!\n");
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_initializer);
  m_linkedTextHandler->start("initializer");
}

void MemberHandler::startException(const QXmlAttributes &)
{
  debug(2,"startException!\n");
  delete m_linkedTextHandler;
  m_linkedTextHandler = new LinkedTextHandler(this,m_exception);
  m_linkedTextHandler->start("exceptions");
}

void MemberHandler::startName(const QXmlAttributes &)
{
  m_curString="";
}

void MemberHandler::endName()
{
  m_name = m_curString.stripWhiteSpace();
  debug(2,"member name=`%s'\n",m_name.data());
}

void MemberHandler::startRead(const QXmlAttributes &)
{
  m_curString="";
}

void MemberHandler::endRead()
{
  m_read = m_curString.stripWhiteSpace();
  debug(2,"member read=`%s'\n",m_read.data());
}

void MemberHandler::startWrite(const QXmlAttributes &)
{
  m_curString="";
}

void MemberHandler::endWrite()
{
  m_write = m_curString.stripWhiteSpace();
  debug(2,"member write=`%s'\n",m_write.data());
}

void MemberHandler::startDefinition(const QXmlAttributes&)
{
  m_curString="";
}

void MemberHandler::endDefinition()
{
  m_definition = m_curString.stripWhiteSpace();
  debug(2,"definition=%s\n",m_definition.data());
}
  
void MemberHandler::startArgsString(const QXmlAttributes&)
{
  m_curString="";
}

void MemberHandler::endArgsString()
{
  m_argsstring = m_curString.stripWhiteSpace();
  debug(2,"arggstring=%s\n",m_argsstring.data());
}
  
void MemberHandler::startParam(const QXmlAttributes& attrib)
{
  ParamHandler *paramHandler = new ParamHandler(this);
  paramHandler->startParam(attrib);
  if (m_insideTemplateParamList)
  {
    m_templateParams.append(paramHandler);
  }
  else
  {
    m_params.append(paramHandler);
  }
}

void MemberHandler::startTemplateParamList(const QXmlAttributes&)
{
  m_insideTemplateParamList = TRUE;
  m_hasTemplateParamList = TRUE;
}

void MemberHandler::endTemplateParamList()
{
  m_insideTemplateParamList = FALSE;
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

void MemberHandler::setCompoundHandler(CompoundHandler *c)
{
  m_compound = c;
}

ICompound *MemberHandler::compound() const
{
  m_compound->addref();
  return m_compound->toICompound();
}

void MemberHandler::setSectionHandler(SectionHandler *c)
{
  m_section = c;
}

ISection *MemberHandler::section() const
{
  return m_section;
}

IMemberIterator *MemberHandler::enumValues() const
{ 
  return new MemberIterator(m_enumValues); 
}

ILinkedTextIterator *MemberHandler::type() const 
{ 
  return new LinkedTextIterator(m_type); 
}

const IString *MemberHandler::typeString() const
{ 
  MemberHandler *that = (MemberHandler *)this;
  that->m_typeString = LinkedTextHandler::toString(m_type); 
  return &m_typeString;
}

IParamIterator *MemberHandler::parameters() const 
{ 
  return new ParamIterator(m_params); 
}

IParamIterator *MemberHandler::templateParameters() const 
{ 
  return m_hasTemplateParamList ? new ParamIterator(m_templateParams) : 0; 
}

IMemberReferenceIterator *MemberHandler::references() const 
{ 
  return new MemberReferenceIterator(m_references); 
}

IMemberReferenceIterator *MemberHandler::referencedBy() const 
{ 
  return new MemberReferenceIterator(m_referencedBy); 
}

ILinkedTextIterator *MemberHandler::initializer() const
{ 
  return new LinkedTextIterator(m_initializer); 
}

ILinkedTextIterator *MemberHandler::exceptions() const
{ 
  return new LinkedTextIterator(m_exception); 
}

IMemberReferenceIterator *MemberHandler::reimplementedBy() const
{ 
  return new MemberReferenceIterator(m_reimplementedBy); 
}

IDocRoot *MemberHandler::briefDescription() const
{ 
  return m_brief; 
}
    
IDocRoot *MemberHandler::detailedDescription() const
{ 
  return m_detailed; 
}

IDocRoot *MemberHandler::inbodyDescription() const
{ 
  return m_inbody; 
}

