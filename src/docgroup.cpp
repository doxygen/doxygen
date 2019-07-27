#include "doxygen.h"
#include "util.h"
#include "entry.h"
#include "message.h"
#include "docgroup.h"


void DocGroup::enterFile(const char *fileName,int)
{
  g_openCount = 0;
  g_autoGroupStack.setAutoDelete(TRUE);
  g_autoGroupStack.clear();
  g_memberGroupId = DOX_NOGROUP;
  g_memberGroupDocs.resize(0);
  g_memberGroupRelates.resize(0);
  g_compoundName=fileName;
}

void DocGroup::leaveFile(const char *fileName,int line)
{
  //if (g_memberGroupId!=DOX_NOGROUP)
  //{
  //  warn(fileName,line,"end of file while inside a member group\n");
  //}
  g_memberGroupId=DOX_NOGROUP;
  g_memberGroupRelates.resize(0);
  g_memberGroupDocs.resize(0);
  if (!g_autoGroupStack.isEmpty())
  {
    warn(fileName,line,"end of file while inside a group");
  }
  else if (g_openCount > 0) // < 0 is already handled on close call
  {
    warn(fileName,line,"end of file with unbalanced grouping commands");
  }
}

void DocGroup::enterCompound(const char *fileName,int line,const char *name)
{
  if (g_memberGroupId!=DOX_NOGROUP)
  {
    warn(fileName,line,"try to put compound %s inside a member group\n",name);
  }
  g_memberGroupId=DOX_NOGROUP;
  g_memberGroupRelates.resize(0);
  g_memberGroupDocs.resize(0);
  g_compoundName = name;
  int i = g_compoundName.find('(');
  if (i!=-1) 
  {
    g_compoundName=g_compoundName.left(i); // strip category (Obj-C)
  }
  if (g_compoundName.isEmpty())
  {
    g_compoundName=fileName;
  }
  //printf("groupEnterCompound(%s)\n",name);
}

void DocGroup::leaveCompound(const char *,int,const char * /*name*/)
{
  //printf("groupLeaveCompound(%s)\n",name);
  //if (g_memberGroupId!=DOX_NOGROUP)
  //{
  //  warn(fileName,line,"end of compound %s while inside a member group\n",name);
  //}
  g_memberGroupId=DOX_NOGROUP;
  g_memberGroupRelates.resize(0);
  g_memberGroupDocs.resize(0);
  g_compoundName.resize(0);
}

int DocGroup::findExistingGroup(int &groupId,const MemberGroupInfo *info)
{
  //printf("findExistingGroup %s:%s\n",info->header.data(),info->compoundName.data());
  QIntDictIterator<MemberGroupInfo> di(Doxygen::memGrpInfoDict);
  MemberGroupInfo *mi;
  for (di.toFirst();(mi=di.current());++di)
  {
    if (g_compoundName==mi->compoundName &&  // same file or scope
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

void DocGroup::open(Entry *e,const char *,int)
{
  g_openCount++;
  //printf("==> openGroup(name=%s,sec=%x) g_autoGroupStack=%d\n",
  //  	e->name.data(),e->section,g_autoGroupStack.count());
  if (e->section==Entry::GROUPDOC_SEC) // auto group
  {
    g_autoGroupStack.push(new Grouping(e->name,e->groupingPri()));
  }
  else // start of a member group
  {
    //printf("    membergroup id=%d %s\n",g_memberGroupId,g_memberGroupHeader.data());
    if (g_memberGroupId==DOX_NOGROUP) // no group started yet
    {
      static int curGroupId=0;

      MemberGroupInfo *info = new MemberGroupInfo;
      info->header = g_memberGroupHeader.stripWhiteSpace();
      info->compoundName = g_compoundName;
      g_memberGroupId = findExistingGroup(curGroupId,info);
      //printf("    use membergroup %d\n",g_memberGroupId);
      Doxygen::memGrpInfoDict.insert(g_memberGroupId,info);

      g_memberGroupRelates = e->relates;
      e->mGrpId = g_memberGroupId;
    }
  }
}

void DocGroup::close(Entry *e,const char *fileName,int line,bool foundInline)
{
  g_openCount--;
  if (g_openCount < 0)
  {
    warn(fileName,line,"unbalanced grouping commands");
  }
  //printf("==> closeGroup(name=%s,sec=%x,file=%s,line=%d) g_autoGroupStack=%d\n",
  //    e->name.data(),e->section,fileName,line,g_autoGroupStack.count());
  if (g_memberGroupId!=DOX_NOGROUP) // end of member group
  {
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(g_memberGroupId);
    if (info) // known group
    {
      info->doc = g_memberGroupDocs;
      info->docFile = fileName;
      info->docLine = line;
    }
    g_memberGroupId=DOX_NOGROUP;
    g_memberGroupRelates.resize(0);
    g_memberGroupDocs.resize(0);
    if (!foundInline) e->mGrpId=DOX_NOGROUP;
    //printf("new group id=%d\n",g_memberGroupId);
  }
  else if (!g_autoGroupStack.isEmpty()) // end of auto group
  {
    Grouping *grp = g_autoGroupStack.pop();
    // see bug577005: we should not remove the last group for e
    if (!foundInline) e->groups->removeLast();
    //printf("Removing %s e=%p\n",grp->groupname.data(),e);
    delete grp;
    if (!foundInline) initGroupInfo(e);
  }
}

void DocGroup::initGroupInfo(Entry *e)
{
  //printf("==> initGroup(id=%d,related=%s,e=%p)\n",g_memberGroupId,
  //       g_memberGroupRelates.data(),e);
  e->mGrpId     = g_memberGroupId;
  e->relates    = g_memberGroupRelates;
  if (!g_autoGroupStack.isEmpty())
  {
    //printf("Appending group %s to %s: count=%d entry=%p\n",
    //	g_autoGroupStack.top()->groupname.data(),
    //	e->name.data(),e->groups->count(),e);
    e->groups->append(new Grouping(*g_autoGroupStack.top()));
  }
}

void DocGroup::addDocs(Entry *e)
{
  if (e->section==Entry::MEMBERGRP_SEC)
  {
    g_memberGroupDocs=e->brief.stripWhiteSpace();
    e->doc = stripLeadingAndTrailingEmptyLines(e->doc,e->docLine);
    if (!g_memberGroupDocs.isEmpty() && !e->doc.isEmpty())
    {
      g_memberGroupDocs+="\n\n";
    }
    g_memberGroupDocs+=e->doc;
    MemberGroupInfo *info=Doxygen::memGrpInfoDict.find(g_memberGroupId);
    if (info) 
    {
      info->doc = g_memberGroupDocs;
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
  return (g_memberGroupId==DOX_NOGROUP);
}

void DocGroup::clearHeader()
{
  g_memberGroupHeader.resize(0);
}

void DocGroup::appendHeader(const char text)
{
  g_memberGroupHeader += text;
}
