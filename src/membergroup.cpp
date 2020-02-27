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

//static QCString idToName(int id)
//{
//  QCString result;
//  result.sprintf("mgroup_%d",id);
//  return result;
//}

MemberGroup::MemberGroup()
{
}

MemberGroup::MemberGroup(int id,const char *hdr,const char *d,const char *docFile,int docLine)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);

  //printf("New member group id=%d header=%s desc=%s\n",id,hdr,d);
  memberList      = new MemberList(MemberListType_memberGroup);
  memberList->setNeedsSorting(sortBriefDocs); // detailed sections are already sorted elsewhere.
  grpId           = id;
  grpHeader       = hdr;
  doc             = d;
  inSameSection   = TRUE;
  inDeclSection   = 0;
  m_numDecMembers = -1;
  m_numDocMembers = -1;
  m_docFile       = docFile;
  m_docLine       = docLine;
  //printf("Member group docs='%s'\n",doc.data());
}

MemberGroup::~MemberGroup()
{
  delete memberList;
}

void MemberGroup::insertMember(MemberDef *md)
{
  //printf("MemberGroup::insertMember memberList=%p count=%d"
  //       " member section list: %p: md=%p:%s\n",
  //       memberList->first() ? memberList->first()->getSectionList() : 0,
  //       memberList->count(),
  //       md->getSectionList(),
  //       md,md->name().data());

  MemberDef *firstMd = memberList->getFirst();
  if (inSameSection && firstMd && firstMd->getSectionList()!=md->getSectionList())
  {
    inSameSection=FALSE;
  }
  else if (inDeclSection==0)
  {
    inDeclSection = const_cast<MemberList*>(md->getSectionList());
    //printf("inDeclSection=%p type=%d\n",inDeclSection,inDeclSection->listType());
  }
  memberList->append(md);

  // copy the group of the first member in the memberGroup
  GroupDef *gd;
  if (firstMd && !firstMd->isAlias() && (gd=const_cast<GroupDef*>(firstMd->getGroupDef())))
  {
    md->setGroupDef(gd, firstMd->getGroupPri(),
                    firstMd->getGroupFileName(),
                    firstMd->getGroupStartLine(),
                    firstMd->getGroupHasDocs());
    gd->insertMember(md);
  }
}


void MemberGroup::setAnchors()
{
  ::setAnchors(memberList);
}

void MemberGroup::writeDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               bool showInline) const
{
  //printf("MemberGroup::writeDeclarations() %s\n",grpHeader.data());
  QCString ldoc = doc;
  if (!ldoc.isEmpty()) ldoc.prepend("<a name=\""+anchor()+"\" id=\""+anchor()+"\"></a>");
  memberList->writeDeclarations(ol,cd,nd,fd,gd,grpHeader,ldoc,FALSE,showInline);
}

void MemberGroup::writePlainDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               const ClassDef *inheritedFrom,const char *inheritId
              ) const
{
  //printf("MemberGroup::writePlainDeclarations() memberList->count()=%d\n",memberList->count());
  memberList->writePlainDeclarations(ol,cd,nd,fd,gd,inheritedFrom,inheritId);
}

void MemberGroup::writeDocumentation(OutputList &ol,const char *scopeName,
               const Definition *container,bool showEnumValues,bool showInline) const
{
  memberList->writeDocumentation(ol,scopeName,container,0,showEnumValues,showInline);
}

void MemberGroup::writeDocumentationPage(OutputList &ol,const char *scopeName,
               const Definition *container) const
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
  MemberListIterator li(*memberList);
  MemberDef *md;
  for (li.toFirst();(md=li.current());++li)
  {
    //printf("matching %d == %d\n",lt,md->getSectionList()->listType());
    const MemberList *ml = md->getSectionList();
    if (ml && lt==ml->listType())
    {
      MemberList ml(lt);
      ml.append(md);
      ml.countDecMembers();
      ml.writePlainDeclarations(ol,cd,0,0,0,inheritedFrom,inheritId);
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
    //printf("matching %d == %d\n",lt,md->getSectionList()->listType());
    const MemberList *ml = md->getSectionList();
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


int MemberGroup::countInheritableMembers(const ClassDef *inheritedFrom) const
{
  return memberList->countInheritableMembers(inheritedFrom);
}


void MemberGroup::distributeMemberGroupDocumentation()
{
  //printf("MemberGroup::distributeMemberGroupDocumentation() %s\n",grpHeader.data());
  MemberListIterator li(*memberList);
  MemberDef *md;
  for (li.toFirst();(md=li.current());++li)
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
  }
  if (md) // distribute docs of md to other members of the list
  {
    //printf("Member %s has documentation!\n",md->name().data());
    MemberDef *omd;
    for (li.toFirst();(omd=li.current());++li)
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
    }
  }
}

#if 0
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

int MemberGroup::sequenceCount() const   
{ 
  return memberList->sequenceCount(); 
}

int MemberGroup::dictionaryCount() const   
{ 
  return memberList->dictionaryCount(); 
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
#endif

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
  MD5SigToString(md5_sig,sigStr.rawData(),33);
  return "amgrp"+sigStr;
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
        grpHeader,0,def);
  }
}

void MemberGroup::findSectionsInDocumentation(const Definition *d)
{
  docFindSections(doc,d,m_docFile);
  memberList->findSectionsInDocumentation(d);
}

void MemberGroup::setRefItems(const std::vector<RefItem*> &sli)
{
  m_xrefListItems.insert(m_xrefListItems.end(), sli.cbegin(), sli.cend());
}

void MemberGroup::writeTagFile(FTextStream &tagFile)
{
  memberList->writeTagFile(tagFile);
}

//--------------------------------------------------------------------------

void MemberGroupInfo::setRefItems(const std::vector<RefItem*> &sli)
{
  m_sli.insert(m_sli.end(), sli.cbegin(), sli.cend());
}
