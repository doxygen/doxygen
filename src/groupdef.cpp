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

#include <ctype.h>
#include "qtbc.h"
#include "groupdef.h"
#include "classdef.h"
#include "filedef.h"
#include "classlist.h"
#include "outputlist.h"
#include "scanner.h"
#include "namespacedef.h"
#include "language.h"
#include "util.h"
#include "memberlist.h"
#include "message.h"
#include "membergroup.h"
#include "doxygen.h"

GroupDef::GroupDef(const char *na,const char *t) : 
   Definition(na)
{
  fileList = new FileList;
  classList = new ClassList;
  groupList = new GroupList;
  namespaceList = new NamespaceList;

  allMemberList = new MemberList;
  allMemberDict = new QDict<MemberDef>;
  if (t) 
    title = t;
  else
  {
    title = na;
    title.at(0)=toupper(title.at(0));
  }
  fileName = "group_"+nameToFile(na);
  memberGroupList = new MemberGroupList;
  memberGroupList->setAutoDelete(TRUE);
  memberGroupDict = new MemberGroupDict(1009);
}

GroupDef::~GroupDef()
{
  delete fileList;
  delete classList;
  delete groupList;
  delete namespaceList;
  delete allMemberList;
  delete allMemberDict;
  delete memberGroupList;
  delete memberGroupDict;
}

void GroupDef::addFile(const FileDef *def)
{
  fileList->append(def);
}

void GroupDef::addClass(const ClassDef *def)
{
  classList->append(def);
}

void GroupDef::addNamespace(const NamespaceDef *def)
{
  namespaceList->append(def);
}

void GroupDef::addMemberToGroup(MemberDef *md,int groupId)
{
  if (groupId!=-1)
  {
    QCString *pGrpHeader = memberHeaderDict[groupId];
    QCString *pDocs      = memberDocDict[groupId];
    if (pGrpHeader)
    {
      MemberGroup *mg = memberGroupDict->find(groupId);
      if (mg==0)
      {
        mg = new MemberGroup(groupId,*pGrpHeader,pDocs ? pDocs->data() : 0);
        memberGroupDict->insert(groupId,mg);
        memberGroupList->append(mg);
      }
      mg->insertMember(md);
      md->setMemberGroup(mg);
    }
  }
}

void GroupDef::insertMember(MemberDef *md,int groupId)
{
  QCString funcDecl=md->name()+md->argsString();
  if (allMemberDict->find(funcDecl)==0)
  {
    allMemberList->append(md); 
    allMemberDict->insert(funcDecl,md);
    switch(md->memberType())
    {
      case MemberDef::Variable:     varMembers.inSort(md); break;
      case MemberDef::Function:     funcMembers.inSort(md); break;
      case MemberDef::Typedef:      typedefMembers.inSort(md); break;
      case MemberDef::Enumeration:  enumMembers.inSort(md); break;
      case MemberDef::EnumValue:    enumValMembers.inSort(md); break;
      case MemberDef::Prototype:    protoMembers.inSort(md); break;
      case MemberDef::Define:       defineMembers.inSort(md); break;
      default:
         err("FileDef::insertMembers(): unexpected member insert in file!\n");
    }
    addMemberToGroup(md,groupId);
  }
}

void GroupDef::addGroup(const GroupDef *def)
{
  groupList->append(def);
}

int GroupDef::countMembers() const
{
  return fileList->count()+
         classList->count()+
         namespaceList->count()+
         groupList->count()+
         allMemberList->count();
}

/*! Compute the HTML anchor names for all members in the class */ 
void GroupDef::computeAnchors()
{
  setAnchors('a',allMemberList);
}

void GroupDef::writeDocumentation(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,fileName,title);
  startTitle(ol,getOutputFileBase());
  ol.docify(title);
  endTitle(ol,getOutputFileBase(),name());

  //brief=brief.stripWhiteSpace();
  //int bl=brief.length();
  OutputList briefOutput(&ol);
  if (!briefDescription().isEmpty())
  {
    parseDoc(briefOutput,name(),0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.startTextLink(0,"_details");
    parseText(ol,theTranslator->trMore());
    ol.endTextLink();
    //ol.enable(OutputGenerator::Latex);
    ol.popGeneratorState();
  }
  ol.startMemberSections();
  if (fileList->count()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trFiles());
    ol.endMemberHeader();
    ol.startMemberList();
    FileDef *fd=fileList->first();
    while (fd)
    {
      ol.startMemberItem(0);
      ol.docify("file");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      ol.endMemberItem(FALSE);
      if (!fd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,0,0,fd->briefDescription());
        ol.endMemberDescription();
        ol.newParagraph();
      }
      fd=fileList->next();
    }
    ol.endMemberList();
  }
  if (namespaceList->count()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trNamespaces());
    ol.endMemberHeader();
    ol.startMemberList();
    NamespaceDef *nd=namespaceList->first();
    while (nd)
    {
      ol.startMemberItem(0);
      ol.docify("namespace");
      ol.insertMemberAlign();
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),0,nd->name());
      ol.endMemberItem(FALSE);
      if (!nd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,0,0,nd->briefDescription());
        ol.endMemberDescription();
        ol.newParagraph();
      }
      nd=namespaceList->next();
    }
    ol.endMemberList();
  }
  if (groupList->count()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trModules());
    ol.endMemberHeader();
    ol.startMemberList();
    GroupDef *gd=groupList->first();
    while (gd)
    {
      ol.startMemberItem(0);
      //ol.insertMemberAlign();
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),0,gd->groupTitle());
      ol.endMemberItem(FALSE);
      if (!gd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,0,0,gd->briefDescription());
        ol.endMemberDescription();
        ol.newParagraph();
      }
      gd=groupList->next();
    }
    ol.endMemberList();
  }

  classList->writeDeclaration(ol);

  if (allMemberList->count()>0)
  {
    /* write user defined member groups */
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writeDeclarations(ol,0,0,0,this);
    }

    allMemberList->writeDeclarations(ol,0,0,0,this,0,0); 
  }
  ol.endMemberSections();
  //int dl=doc.length();
  //doc=doc.stripWhiteSpace();
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.writeAnchor("_details");
    ol.popGeneratorState();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    // repeat brief description
    if (!briefDescription().isEmpty())
    {
      ol+=briefOutput;
      ol.newParagraph();
    }
    // write documentation
    if (!documentation().isEmpty())
    {
      parseDoc(ol,name(),0,documentation()+"\n");
    }
  }

  defineMembers.countDocMembers();
  if (defineMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDefineDocumentation());
    ol.endGroupHeader();
    defineMembers.writeDocumentation(ol,name());
  }
  
  protoMembers.countDocMembers(); 
  if (protoMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionPrototypeDocumentation());
    ol.endGroupHeader();
    protoMembers.writeDocumentation(ol,name());
  }

  typedefMembers.countDocMembers();
  if (typedefMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trTypedefDocumentation());
    ol.endGroupHeader();
    typedefMembers.writeDocumentation(ol,name());
  }
  
  enumMembers.countDocMembers();
  if (enumMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationTypeDocumentation());
    ol.endGroupHeader();
    enumMembers.writeDocumentation(ol,name());
  }

  //enumValMembers.countDocMembers();
  //if (enumValMembers.totalCount()>0 )
  //{
  //  ol.writeRuler();
  //  ol.startGroupHeader();
  //  parseText(ol,theTranslator->trEnumerationValueDocumentation());
  //  ol.endGroupHeader();
  //  enumValMembers.writeDocumentation(ol,name());
  //}

  funcMembers.countDocMembers();
  if (funcMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionDocumentation());
    ol.endGroupHeader();
    funcMembers.writeDocumentation(ol,name());
  }
  
  varMembers.countDocMembers();
  if (varMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trVariableDocumentation());
    ol.endGroupHeader();
    varMembers.writeDocumentation(ol,name());
  }

  endFile(ol); 
  ol.popGeneratorState();
}

//---- helper functions ------------------------------------------------------

void addClassToGroups(Entry *root,ClassDef *cd)
{
  QListIterator<QCString> sli(*root->groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    GroupDef *gd=0;
    if (!s->isEmpty() && (gd=groupDict[*s]))
    {
      gd->addClass(cd);
      //printf("Compound %s: in group %s\n",cd->name().data(),s->data());
    }
  }
}

void addNamespaceToGroups(Entry *root,NamespaceDef *nd)
{
  //printf("root->groups->count()=%d\n",root->groups->count());
  QListIterator<QCString> sli(*root->groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    GroupDef *gd=0;
    //printf("group `%s'\n",s->data());
    if (!s->isEmpty() && (gd=groupDict[*s]))
    {
      gd->addNamespace(nd);
      //printf("Namespace %s: in group %s\n",nd->name().data(),s->data());
    }
  }
}

void addGroupToGroups(Entry *root,GroupDef *subGroup)
{
  QListIterator<QCString> sli(*root->groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    GroupDef *gd=0;
    if (!s->isEmpty() && (gd=groupDict[*s]))
    {
      gd->addGroup(subGroup);
    }
  }
}

void addMemberToGroups(Entry *root,MemberDef *md)
{
  QListIterator<QCString> sli(*root->groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    GroupDef *gd=0;
    if (!s->isEmpty() && (gd=groupDict[*s]))
    {
      GroupDef *mgd = md->groupDef();
      if (mgd==0)
      {
        gd->insertMember(md,root->mGrpId);
        md->setGroupDef(gd);
      }
      else if (mgd!=gd)
      {
        warn("Warning: Member %s found in multiple groups.!\n"
             "The member will be put in group %s, and not in group %s",
              md->name().data(),mgd->name().data(),gd->name().data()
            );
      }
      //printf("Member %s: in group %s\n",md->name().data(),s->data());
    }
  }
}

