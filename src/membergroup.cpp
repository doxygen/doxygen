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
 * All output generated with Doxygen is not covered by this license.
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
  memberList = new MemberList;
  grpId      = id;
  grpHeader  = hdr;
  doc        = d;
  scope      = 0;
  //printf("Member group docs=`%s'\n",doc.data());
}

MemberGroup::~MemberGroup()
{
  delete memberList;
}

void MemberGroup::insertMember(MemberDef *md)
{
  //printf("MemberGroup::insertMember(%s)\n",md->name().data());
  memberList->append(md);
}


void MemberGroup::setAnchors()
{
  ::setAnchors('z',memberList,grpId);
}

void MemberGroup::writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd)
{
  memberList->writeDeclarations(ol,cd,nd,fd,gd,grpHeader,doc,TRUE);
}

void MemberGroup::writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd)
{
  //printf("MemberGroup::writePlainDeclarations() memberList->count()=%d\n",memberList->count());
  memberList->writePlainDeclarations(ol,cd,nd,fd,gd,TRUE);
}

