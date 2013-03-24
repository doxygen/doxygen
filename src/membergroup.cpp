/******************************************************************************
 *
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
#include "marshal.h"
#include "entry.h"
#include "md5.h"

//static QCString idToName(int id)
//{
//  QCString result;
//  result.sprintf("mgroup_%d",id);
//  return result;
//}

MemberGroup::MemberGroup()
{
}

MemberGroup::MemberGroup(Definition *parent,
      int id,const char *hdr,const char *d,const char *docFile) 
{
  //printf("New member group id=%d header=%s desc=%s\n",id,hdr,d);
  memberList      = new MemberList(MemberListType_memberGroup);
  grpId           = id;
  grpHeader       = hdr;
  doc             = d;
  scope           = 0;
  inSameSection   = TRUE;
  inDeclSection   = 0;
  m_numDecMembers = -1;
  m_numDocMembers = -1;
  m_parent        = parent;
  m_docFile       = docFile;
  m_xrefListItems = 0;
  //printf("Member group docs=`%s'\n",doc.data());
}

MemberGroup::~MemberGroup()
{
  delete memberList;
}

void MemberGroup::insertMember(MemberDef *md)
{
  //printf("MemberGroup::insertMember m_parent=%s memberList=%p count=%d"
  //       " member section list: %p: md=%p:%s\n",
  //       m_parent ? m_parent->name().data() : "<null>",
  //       memberList->first() ? memberList->first()->getSectionList(m_parent) : 0,
  //       memberList->count(),
  //       md->getSectionList(m_parent),
  //       md,md->name().data());

  MemberDef *firstMd = memberList->first();
  if (inSameSection && memberList->count()>0 && 
      firstMd->getSectionList(m_parent)!=md->getSectionList(m_parent))
  {
    inSameSection=FALSE;
  }
  else if (inDeclSection==0)
  {
    inDeclSection = md->getSectionList(m_parent);
    //printf("inDeclSection=%p type=%d\n",inDeclSection,inDeclSection->listType());
  }
  memberList->append(md);

  // copy the group of the first member in the memberGroup
  GroupDef *gd;
  if (firstMd && (gd=firstMd->getGroupDef()))
  {
    md->setGroupDef(gd, firstMd->getGroupPri(), 
                    firstMd->getGroupFileName(), firstMd->getGroupStartLine(), 
                    firstMd->getGroupHasDocs());
    gd->insertMember(md);
  }
}


void MemberGroup::setAnchors()
{
  ::setAnchors(memberList);
}

void MemberGroup::writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool showInline)
{
  //printf("MemberGroup::writeDeclarations() %s\n",grpHeader.data());
  QCString ldoc = doc;
  if (!ldoc.isEmpty()) ldoc.prepend("<a name=\""+anchor()+"\" id=\""+anchor()+"\"></a>");
  memberList->writeDeclarations(ol,cd,nd,fd,gd,grpHeader,ldoc,DefinitionIntf::TypeGroup,FALSE,showInline);
}

void MemberGroup::writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               ClassDef *inheritedFrom,const char *inheritId
              )
{
  //printf("MemberGroup::writePlainDeclarations() memberList->count()=%d\n",memberList->count());
  memberList->writePlainDeclarations(ol,cd,nd,fd,gd,DefinitionIntf::TypeGroup,inheritedFrom,inheritId);
}

void MemberGroup::writeDocumentation(OutputList &ol,const char *scopeName,
               Definition *container,bool showEnumValues,bool showInline)
{
  memberList->writeDocumentation(ol,scopeName,container,0,showEnumValues,showInline);
}

void MemberGroup::writeDocumentationPage(OutputList &ol,const char *scopeName,
               Definition *container)
{
  memberList->writeDocumentationPage(ol,scopeName,container);
}

void MemberGroup::addGroupedInheritedMembers(OutputList &ol,ClassDef *cd,
               MemberListType lt,
               ClassDef *inheritedFrom,const QCString &inheritId)
{
  //printf("** addGroupedInheritedMembers()\n");
  MemberListIterator li(*memberList);
  MemberDef *md;
  for (li.toFirst();(md=li.current());++li)
  {
    //printf("matching %d == %d\n",lt,md->getSectionList(m_parent)->listType());
    if (lt==md->getSectionList(m_parent)->listType())
    {
      MemberList ml(lt);
      ml.append(md);
      ml.writePlainDeclarations(ol,cd,0,0,0,DefinitionIntf::TypeGroup,inheritedFrom,inheritId);
    }
  }
}

int MemberGroup::countGroupedInheritedMembers(MemberListType lt)
{
  //printf("** countGroupedInheritedMembers()\n");
  int count=0;
  MemberListIterator li(*memberList);
  MemberDef *md;
  for (li.toFirst();(md=li.current());++li)
  {
    //printf("matching %d == %d\n",lt,md->getSectionList(m_parent)->listType());
    if (lt==md->getSectionList(m_parent)->listType())
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
    //printf("Adding group %p to list %p (type=%d)\n",this,
    //                              inDeclSection,inDeclSection->listType());
    inDeclSection->addMemberGroup(this);
  }
}

int MemberGroup::countDecMembers(GroupDef *gd)
{
  if (m_numDecMembers==-1) /* number of member not cached */
  {
    memberList->countDecMembers(gd);
    m_numDecMembers = memberList->numDecMembers();
  }
  return m_numDecMembers;
}

int MemberGroup::countDocMembers()
{
  if (m_numDocMembers==-1)
  {
    memberList->countDocMembers();
    m_numDocMembers = memberList->numDocMembers();
  }
  return m_numDocMembers;
}

int MemberGroup::countInheritableMembers(ClassDef *inheritedFrom) const
{
  return memberList->countInheritableMembers(inheritedFrom);
}


void MemberGroup::distributeMemberGroupDocumentation()
{
  //printf("MemberGroup::distributeMemberGroupDocumentation() %s\n",grpHeader.data());
  MemberDef *md=memberList->first();
  while (md)
  {
    //printf("checking md=%s\n",md->name().data());
    // find the first member of the group with documentation
    if (!md->documentation().isEmpty()       ||
        !md->briefDescription().isEmpty()    ||
        !md->inbodyDocumentation().isEmpty()
       )
    {
      //printf("found it!\n");
      break;
    }
    md=memberList->next();
  }
  if (md) // distribute docs of md to other members of the list
  {
    //printf("Member %s has documentation!\n",md->name().data());
    MemberDef *omd=memberList->first();
    while (omd)
    {
      if (md!=omd && omd->documentation().isEmpty() && 
                     omd->briefDescription().isEmpty() && 
                     omd->inbodyDocumentation().isEmpty()
         )
      {
        //printf("Copying documentation to member %s\n",omd->name().data());
        omd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
        omd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
        omd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
      }
      omd=memberList->next();
    }
  }
}

int MemberGroup::varCount() const
{
  return memberList->varCount();
}

int MemberGroup::funcCount() const      
{ 
  return memberList->funcCount(); 
}

int MemberGroup::enumCount() const      
{ 
  return memberList->enumCount(); 
}

int MemberGroup::enumValueCount() const 
{ 
  return memberList->enumValueCount(); 
}

int MemberGroup::typedefCount() const   
{ 
  return memberList->typedefCount(); 
}

int MemberGroup::protoCount() const     
{ 
  return memberList->protoCount(); 
}

int MemberGroup::defineCount() const    
{ 
  return memberList->defineCount(); 
}

int MemberGroup::friendCount() const    
{ 
  return memberList->friendCount(); 
}

int MemberGroup::numDecMembers() const  
{ 
  return memberList->numDecMembers(); 
}

int MemberGroup::numDocMembers() const  
{ 
  return memberList->numDocMembers(); 
}

void MemberGroup::setInGroup(bool b)
{
  memberList->setInGroup(b);
}

QCString MemberGroup::anchor() const
{
  uchar md5_sig[16];
  QCString sigStr(33);
  QCString locHeader = grpHeader;
  if (locHeader.isEmpty()) locHeader="[NOHEADER]";
  MD5Buffer((const unsigned char *)locHeader.data(),locHeader.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.data(),33);
  return "amgrp"+sigStr;
}

void MemberGroup::addListReferences(Definition *def)
{
  memberList->addListReferences(def);
  if (m_xrefListItems && def)
  {
    QCString name = def->getOutputFileBase()+"#"+anchor();
    addRefItem(m_xrefListItems,
        name,
        theTranslator->trGroup(TRUE,TRUE),
        name,
        grpHeader,0);
  }
}

void MemberGroup::findSectionsInDocumentation()
{
  docFindSections(doc,0,this,m_docFile);
  memberList->findSectionsInDocumentation();
}

void MemberGroup::marshal(StorageIntf *s)
{
  marshalMemberList(s,memberList);
  marshalObjPointer(s,inDeclSection); // reference only
  marshalInt(s,grpId);
  marshalQCString(s,grpHeader);
  marshalQCString(s,fileName);
  marshalObjPointer(s,scope);
  marshalQCString(s,doc);
  marshalBool(s,inSameSection);
  marshalInt(s,m_numDecMembers);
  marshalInt(s,m_numDocMembers);
  marshalObjPointer(s,m_parent);
  marshalQCString(s,m_docFile);
  marshalItemInfoList (Doxygen::symbolStorage,m_xrefListItems);
}

void MemberGroup::unmarshal(StorageIntf *s)
{
  memberList      = unmarshalMemberList(s);
  inDeclSection   = (MemberList *)unmarshalObjPointer(s); 
  grpId           = unmarshalInt(s);
  grpHeader       = unmarshalQCString(s);
  fileName        = unmarshalQCString(s);
  scope           = (Definition *)unmarshalObjPointer(s);
  doc             = unmarshalQCString(s);
  inSameSection   = unmarshalBool(s);
  m_numDecMembers = unmarshalInt(s);
  m_numDocMembers = unmarshalInt(s);
  m_parent        = (Definition *)unmarshalObjPointer(s);
  m_docFile       = unmarshalQCString(s);
  m_xrefListItems = unmarshalItemInfoList (Doxygen::symbolStorage);
}

void MemberGroup::setRefItems(const QList<ListItemInfo> *sli)
{
  if (sli)
  {
    // deep copy the list
    if (m_xrefListItems==0) 
    {
      m_xrefListItems=new QList<ListItemInfo>;
      m_xrefListItems->setAutoDelete(TRUE);
    }
    QListIterator<ListItemInfo> slii(*sli);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      m_xrefListItems->append(new ListItemInfo(*lii));
    } 
  }
}
//--------------------------------------------------------------------------

void MemberGroupInfo::setRefItems(const QList<ListItemInfo> *sli)
{
  if (!sli) return;
  if (m_sli==0)
  {
    m_sli = new QList<ListItemInfo>;
    m_sli->setAutoDelete(TRUE);
  }
  QListIterator<ListItemInfo> slii(*sli);
  ListItemInfo *ili;
  for (slii.toFirst();(ili=slii.current());++slii)
  {
    m_sli->append(new ListItemInfo(*ili));
  }
}
