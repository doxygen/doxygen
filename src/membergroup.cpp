/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include "qtbc.h"
#include "membergroup.h"
#include "memberlist.h"
#include "outputlist.h"
#include "util.h"
#include "classdef.h"
#include "namespacedef.h"
#include "filedef.h"
#include "language.h"
#include "scanner.h"

//static QCString idToName(int id)
//{
//  QCString result;
//  result.sprintf("mgroup_%d",id);
//  return result;
//}

MemberGroup::MemberGroup(int id,const char *hdr,const char *d) /* : Definition(idToName(id)) */
{
  memberList     = new MemberList;
  grpId          = id;
  grpHeader      = hdr;
  doc            = d;
  scope          = 0;
  inSameSection  = TRUE;
  inDeclSection  = 0;
  numDeclMembers = -1;
  //printf("Member group docs=`%s'\n",doc.data());
}

MemberGroup::~MemberGroup()
{
  delete memberList;
}

void MemberGroup::insertMember(MemberDef *md)
{
  //printf("MemberGroup::insertMember memberList=%p count=%d"
  //       " member section list: %p\n",memberList,memberList->count(),
  //       md->getSectionList());
  if (inSameSection && memberList->count()>0 && 
      memberList->first()->getSectionList()!=md->getSectionList())
  {
    inSameSection=FALSE;
  }
  else if (inDeclSection==0)
  {
    inDeclSection = md->getSectionList();
  }
  memberList->append(md);
}


void MemberGroup::setAnchors()
{
  ::setAnchors('z',memberList,grpId);
}

void MemberGroup::writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd)
{
  memberList->writeDeclarations(ol,cd,nd,fd,gd,grpHeader,doc,TRUE,TRUE);
}

void MemberGroup::writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool inGroup)
{
  //printf("MemberGroup::writePlainDeclarations() memberList->count()=%d\n",memberList->count());
  memberList->writePlainDeclarations(ol,cd,nd,fd,gd,inGroup);
}

/*! Add this group as a subsection of the declaration section, instead
 *  of rendering it in its own section
 */
void MemberGroup::addToDeclarationSection()
{
  if (inDeclSection)
  {
    inDeclSection->addMemberGroup(this);
  }
}

int MemberGroup::countDecMembers(bool sectionPerType)
{
  if (numDeclMembers==-1) /* number of member not cached */
  {
    memberList->countDecMembers(TRUE,TRUE,sectionPerType);
    numDeclMembers = memberList->totalCount();
  }
  return numDeclMembers;
}

void MemberGroup::distributeMemberGroupDocumentation()
{
  MemberDef *md=memberList->first();
  if (md && md->hasDocumentation())
  {
    MemberDef *omd=memberList->next();
    while (omd)
    {
      if (!omd->hasDocumentation())
      {
        omd->setBriefDescription(md->briefDescription());
        omd->setDocumentation(md->documentation());
      }
      omd=memberList->next();
    }
  }
}
