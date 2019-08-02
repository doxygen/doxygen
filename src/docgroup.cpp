#include "doxygen.h"
#include "util.h"
#include "entry.h"
#include "message.h"
#include "docgroup.h"


void DocGroup::enterFile(const char *fileName,int)
{
  m_openCount = 0;
  m_autoGroupStack.setAutoDelete(TRUE);
  m_autoGroupStack.clear();
  m_memberGroupId = DOX_NOGROUP;
  m_memberGroupDocs.resize(0);
  m_memberGroupRelates.resize(0);
  m_compoundName=fileName;
}

void DocGroup::leaveFile(const char *fileName,int line)
{
  //if (m_memberGroupId!=DOX_NOGROUP)
  //{
  //  warn(fileName,line,"end of file while inside a member group\n");
  //}
  m_memberGroupId=DOX_NOGROUP;
  m_memberGroupRelates.resize(0);
  m_memberGroupDocs.resize(0);
  if (!m_autoGroupStack.isEmpty())
  {
    warn(fileName,line,"end of file while inside a group");
  }
  else if (m_openCount > 0) // < 0 is already handled on close call
  {
    warn(fileName,line,"end of file with unbalanced grouping commands");
  }
}

void DocGroup::enterCompound(const char *fileName,int line,const char *name)
{
  if (m_memberGroupId!=DOX_NOGROUP)
  {
    warn(fileName,line,"try to put compound %s inside a member group\n",name);
  }
  m_memberGroupId=DOX_NOGROUP;
  m_memberGroupRelates.resize(0);
  m_memberGroupDocs.resize(0);
  m_compoundName = name;
  int i = m_compoundName.find('(');
  if (i!=-1) 
  {
    m_compoundName=m_compoundName.left(i); // strip category (Obj-C)
  }
  if (m_compoundName.isEmpty())
  {
    m_compoundName=fileName;
  }
  //printf("groupEnterCompound(%s)\n",name);
}

void DocGroup::leaveCompound(const char *,int,const char * /*name*/)
{
  //printf("groupLeaveCompound(%s)\n",name);
  //if (m_memberGroupId!=DOX_NOGROUP)
  //{
  //  warn(fileName,line,"end of compound %s while inside a member group\n",name);
  //}
  m_memberGroupId=DOX_NOGROUP;
  m_memberGroupRelates.resize(0);
  m_memberGroupDocs.resize(0);
  m_compoundName.resize(0);
}

int DocGroup::findExistingGroup(int &groupId,const MemberGroupInfo *info)
{
  //printf("findExistingGroup %s:%s\n",info->header.data(),info->compoundName.data());
  QIntDictIterator<MemberGroupInfo> di(Doxygen::memGrpInfoDict);
  MemberGroupInfo *mi;
  for (di.toFirst();(mi=di.current());++di)
  {
    if (m_compoundName==mi->compoundName &&  // same file or scope
	!mi->header.isEmpty() &&             // not a nameless group
	qstricmp(mi->header,info->header)==0  // same header name
       )
    {
      //printf("Found it!\n");
      return (int)di.currentKey(); // put the item in this group
    }
  }
  groupId++; // start new group
  return groupId;
}

void DocGroup::open(Entry *e,const char *,int, bool implicit)
{
  if (!implicit) m_openCount++;
  //printf("==> openGroup(name=%s,sec=%x) m_autoGroupStack=%d\n",
  //  	e->name.data(),e->section,m_autoGroupStack.count());
  if (e->section==Entry::GROUPDOC_SEC) // auto group
  {
    m_autoGroupStack.push(new Grouping(e->name,e->groupingPri()));
  }
  else // start of a member group
  {
    //printf("    membergroup id=%d %s\n",m_memberGroupId,m_memberGroupHeader.data());
    if (m_memberGroupId==DOX_NOGROUP) // no group started yet
    {
      static int curGroupId=0;

      MemberGroupInfo *info = new MemberGroupInfo;
      info->header = m_memberGroupHeader.stripWhiteSpace();
      info->compoundName = m_compoundName;
      m_memberGroupId = findExistingGroup(curGroupId,info);
      //printf("    use membergroup %d\n",m_memberGroupId);
      Doxygen::memGrpInfoDict.insert(m_memberGroupId,info);

      m_memberGroupRelates = e->relates;
      e->mGrpId = m_memberGroupId;
    }
  }
}

void DocGroup::close(Entry *e,const char *fileName,int line,bool foundInline,bool implicit)
{
  if (!implicit)
  {
    if (m_openCount < 1)
    {
      warn(fileName,line,"unbalanced grouping commands");
    }
    else
    {
      m_openCount--;
    }
  }
  //printf("==> closeGroup(name=%s,sec=%x,file=%s,line=%d) m_autoGroupStack=%d\n",
  //    e->name.data(),e->section,fileName,line,m_autoGroupStack.count());
  if (m_memberGroupId!=DOX_NOGROUP) // end of member group
  {
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(m_memberGroupId);
    if (info) // known group
    {
      info->doc = m_memberGroupDocs;
      info->docFile = fileName;
      info->docLine = line;
    }
    m_memberGroupId=DOX_NOGROUP;
    m_memberGroupRelates.resize(0);
    m_memberGroupDocs.resize(0);
    if (!foundInline) e->mGrpId=DOX_NOGROUP;
    //printf("new group id=%d\n",m_memberGroupId);
  }
  else if (!m_autoGroupStack.isEmpty()) // end of auto group
  {
    Grouping *grp = m_autoGroupStack.pop();
    // see bug577005: we should not remove the last group for e
    if (!foundInline) e->groups->removeLast();
    //printf("Removing %s e=%p\n",grp->groupname.data(),e);
    delete grp;
    if (!foundInline) initGroupInfo(e);
  }
}

void DocGroup::initGroupInfo(Entry *e)
{
  //printf("==> initGroup(id=%d,related=%s,e=%p)\n",m_memberGroupId,
  //       m_memberGroupRelates.data(),e);
  e->mGrpId     = m_memberGroupId;
  e->relates    = m_memberGroupRelates;
  if (!m_autoGroupStack.isEmpty())
  {
    //printf("Appending group %s to %s: count=%d entry=%p\n",
    //	m_autoGroupStack.top()->groupname.data(),
    //	e->name.data(),e->groups->count(),e);
    e->groups->append(new Grouping(*m_autoGroupStack.top()));
  }
}

void DocGroup::addDocs(Entry *e)
{
  if (e->section==Entry::MEMBERGRP_SEC)
  {
    m_memberGroupDocs=e->brief.stripWhiteSpace();
    e->doc = stripLeadingAndTrailingEmptyLines(e->doc,e->docLine);
    if (!m_memberGroupDocs.isEmpty() && !e->doc.isEmpty())
    {
      m_memberGroupDocs+="\n\n";
    }
    m_memberGroupDocs+=e->doc;
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(m_memberGroupId);
    if (info) 
    {
      info->doc = m_memberGroupDocs;
      info->docFile = e->docFile;
      info->docLine = e->docLine;
      info->setRefItems(e->sli);
    }
    e->doc.resize(0);
    e->brief.resize(0);
  }
}

bool DocGroup::isEmpty() const
{
  return (m_memberGroupId==DOX_NOGROUP);
}

void DocGroup::clearHeader()
{
  m_memberGroupHeader.resize(0);
}

void DocGroup::appendHeader(const char text)
{
  m_memberGroupHeader += text;
}
