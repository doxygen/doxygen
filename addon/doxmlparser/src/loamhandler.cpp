#include "loamhandler.h"
#include "memberhandler.h"


ListOfAllMembersHandler::ListOfAllMembersHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_members.setAutoDelete(TRUE);

  addStartHandler("member",this,&ListOfAllMembersHandler::startMember);
  addStartHandler("name",this,&ListOfAllMembersHandler::startName);
  addEndHandler("name",this,&ListOfAllMembersHandler::endName);

  addEndHandler("listofallmembers",this,&ListOfAllMembersHandler::endListOfAllMembers);
}

void ListOfAllMembersHandler::initialize(MainHandler *mh)
{
  QListIterator<MemberReference> mli(m_members);
  MemberReference *mr;
  for (;(mr=mli.current());++mli)
  {
    mr->initialize(mh);
  }
}

void ListOfAllMembersHandler::startMember(const QXmlAttributes& attrib)
{
  MemberReference *mr = new MemberReference;
  mr->m_memId = attrib.value("refid");
  // TODO: extract prot, virt, ambiguityscope and scope as well
  m_members.append(new MemberReference);
}

void ListOfAllMembersHandler::startName(const QXmlAttributes&)
{
  m_curString="";
}

void ListOfAllMembersHandler::endName()
{
  ASSERT(m_members.getLast());
  m_members.getLast()->m_name = m_curString;
}

void ListOfAllMembersHandler::startListOfAllMembers(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"listofallmembers start\n");
}

void ListOfAllMembersHandler::endListOfAllMembers()
{
  m_parent->setDelegate(0);
  debug(2,"listofallmembers end\n");
}

IMemberReferenceIterator *ListOfAllMembersHandler::members() const
{ 
  return new MemberReferenceIterator(m_members); 
}
