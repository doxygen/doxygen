/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include "membergroup.h"
#include "memberlist.h"
#include "outputlist.h"
#include "util.h"
#include "classdef.h"
#include "namespacedef.h"
#include "filedef.h"
#include "language.h"
#include "groupdef.h"
#include "doxygen.h"
#include "docparser.h"
#include "entry.h"
#include "md5.h"

MemberGroup::MemberGroup(const Definition *container,int id,const QCString &hdr,
                         const QCString &d,const QCString &docFile,int docLine,
                         MemberListContainer con)
  : m_container(container),
    memberList(std::make_unique<MemberList>(MemberListType_memberGroup,con)),
    grpId(id), grpHeader(hdr), doc(d), m_docFile(docFile), m_docLine(docLine)
{
  //printf("New member group id=%d header=%s desc=%s\n",id,hdr,d);
  memberList->setNeedsSorting(Config_getBool(SORT_BRIEF_DOCS)); // detailed sections are already sorted elsewhere.
  //printf("Member group docs='%s'\n",qPrint(doc));
}

MemberGroup::~MemberGroup()
{
}

void MemberGroup::insertMember(const MemberDef *md)
{
  //printf("MemberGroup::insertMember memberList=%p count=%d"
  //       " member section list: %p: md=%p:%s\n",
  //       memberList->first() ? memberList->first()->getSectionList() : 0,
  //       memberList->count(),
  //       md->getSectionList(),
  //       md,qPrint(md->name()));

  const MemberDef *firstMd = memberList->empty() ? 0 : memberList->front();
  if (inSameSection && firstMd &&
      firstMd->getSectionList(m_container)!=md->getSectionList(m_container))
  {
    inSameSection=FALSE;
  }
  else if (inDeclSection==0)
  {
    inDeclSection = const_cast<MemberList*>(md->getSectionList(m_container));
    //printf("inDeclSection=%p type=%d\n",inDeclSection,inDeclSection->listType());
  }
  memberList->push_back(md);

  // copy the group of the first member in the memberGroup
  GroupDef *gd;
  if (firstMd && !firstMd->isAlias() && (gd=const_cast<GroupDef*>(firstMd->getGroupDef())))
  {
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm)
    {
      mdm->setGroupDef(gd, firstMd->getGroupPri(),
                       firstMd->getGroupFileName(),
                       firstMd->getGroupStartLine(),
                       firstMd->getGroupHasDocs());
    }
    gd->insertMember(md);
  }
}


void MemberGroup::setAnchors()
{
  memberList->setAnchors();
}

void MemberGroup::writeDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               bool showInline) const
{
  //printf("MemberGroup::writeDeclarations() %s\n",qPrint(grpHeader));
  QCString ldoc = doc;
  if (!ldoc.isEmpty()) ldoc.prepend("<a name=\""+anchor()+"\" id=\""+anchor()+"\"></a>");
  memberList->writeDeclarations(ol,cd,nd,fd,gd,grpHeader,ldoc,FALSE,showInline);
}

void MemberGroup::writePlainDeclarations(OutputList &ol,bool inGroup,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               int indentLevel,const ClassDef *inheritedFrom,const QCString &inheritId
              ) const
{
  //printf("MemberGroup::writePlainDeclarations() memberList->count()=%d\n",memberList->count());
  memberList->writePlainDeclarations(ol,inGroup,cd,nd,fd,gd,indentLevel,inheritedFrom,inheritId);
}

void MemberGroup::writeDocumentation(OutputList &ol,const QCString &scopeName,
               const Definition *container,bool showEnumValues,bool showInline) const
{
  //printf("MemberGroup::writeDocumentation() %s\n",qPrint(grpHeader));
  memberList->writeDocumentation(ol,scopeName,container,QCString(),showEnumValues,showInline);
}

void MemberGroup::writeDocumentationPage(OutputList &ol,const QCString &scopeName,
               const DefinitionMutable *container) const
{
  memberList->writeDocumentationPage(ol,scopeName,container);
}

void MemberGroup::setAnonymousEnumType()
{
  memberList->setAnonymousEnumType();
}

void MemberGroup::addGroupedInheritedMembers(OutputList &ol,const ClassDef *cd,
               MemberListType lt,
               const ClassDef *inheritedFrom,const QCString &inheritId) const
{
  //printf("** addGroupedInheritedMembers()\n");
  for (const auto &md : *memberList)
  {
    //printf("matching %d == %d\n",lt,md->getSectionList()->listType());
    const MemberList *ml = md->getSectionList(m_container);
    if (ml && lt==ml->listType())
    {
      MemberList mml(lt,MemberListContainer::Class);
      mml.push_back(md);
      mml.countDecMembers();
      mml.writePlainDeclarations(ol,false,cd,0,0,0,0,inheritedFrom,inheritId);
    }
  }
}

int MemberGroup::countGroupedInheritedMembers(MemberListType lt)
{
  //printf("** countGroupedInheritedMembers()\n");
  int count=0;
  for (const auto &md : *memberList)
  {
    //printf("matching %d == %d\n",lt,md->getSectionList()->listType());
    const MemberList *ml = md->getSectionList(m_container);
    if (ml && lt==ml->listType())
    {
      count++;
    }
  }
  return count;
}


/*! Add this group as a subsection of the declaration section, instead
 *  of rendering it in its own section
 */
void MemberGroup::addToDeclarationSection()
{
  if (inDeclSection)
  {
    //printf("Adding group %p to list %p (type=%d) memberList=%p\n",this,
    //                             inDeclSection,inDeclSection->listType(),memberList);
    inDeclSection->addMemberGroup(this);
  }
}

void MemberGroup::countDecMembers()
{
  memberList->countDecMembers();
}

void MemberGroup::countDocMembers()
{
  memberList->countDocMembers();
}

const Definition *MemberGroup::container() const
{
  return m_container;
}

int MemberGroup::countInheritableMembers(const ClassDef *inheritedFrom) const
{
  return memberList->countInheritableMembers(inheritedFrom);
}


void MemberGroup::distributeMemberGroupDocumentation()
{
  //printf("MemberGroup::distributeMemberGroupDocumentation() %s\n",qPrint(grpHeader));
  const MemberDef *md = 0;
  for (const auto &smd : *memberList)
  {
    //printf("checking md=%s\n",qPrint(md->name()));
    // find the first member of the group with documentation
    if (!smd->documentation().isEmpty()       ||
        !smd->briefDescription().isEmpty()    ||
        !smd->inbodyDocumentation().isEmpty()
       )
    {
      //printf("found it!\n");
      md = smd;
      break;
    }
  }
  if (md) // distribute docs of md to other members of the list
  {
    //printf("Member %s has documentation!\n",qPrint(md->name()));
    for (const auto &iomd : *memberList)
    {
      MemberDefMutable *omd = toMemberDefMutable(iomd);
      if (omd && md!=omd && omd->documentation().isEmpty() &&
                            omd->briefDescription().isEmpty() &&
                            omd->inbodyDocumentation().isEmpty()
         )
      {
        //printf("Copying documentation to member %s\n",qPrint(omd->name()));
        omd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
        omd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
        omd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
      }
    }
  }
}


int MemberGroup::numDecMembers() const
{
  return memberList->numDecMembers();
}

int MemberGroup::numDecEnumValues() const
{
  return memberList->numDecEnumValues();
}

int MemberGroup::numDocMembers() const
{
  return memberList->numDocMembers();
}

int MemberGroup::numDocEnumValues() const
{
  return memberList->numDocEnumValues();
}

QCString MemberGroup::anchor() const
{
  uchar md5_sig[16];
  char sigStr[33];
  QCString locHeader = grpHeader;
  if (locHeader.isEmpty()) locHeader="[NOHEADER]";
  MD5Buffer(locHeader.data(),locHeader.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr);
  return QCString("amgrp")+sigStr;
}

void MemberGroup::addListReferences(Definition *def)
{
  memberList->addListReferences(def);
  if (def)
  {
    QCString name = def->getOutputFileBase()+"#"+anchor();
    addRefItem(m_xrefListItems,
        name,
        theTranslator->trGroup(TRUE,TRUE),
        name,
        grpHeader,QCString(),def);
  }
}

void MemberGroup::findSectionsInDocumentation(const Definition *d)
{
  docFindSections(doc,d,m_docFile);
  memberList->findSectionsInDocumentation(d);
}

void MemberGroup::setRefItems(const RefItemVector &sli)
{
  m_xrefListItems.insert(m_xrefListItems.end(), sli.cbegin(), sli.cend());
}

void MemberGroup::writeTagFile(TextStream &tagFile)
{
  memberList->writeTagFile(tagFile);
}

//--------------------------------------------------------------------------

void MemberGroupInfo::setRefItems(const RefItemVector &sli)
{
  m_sli.insert(m_sli.end(), sli.cbegin(), sli.cend());
}
