/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
#include "doc.h"
#include "namespacedef.h"
#include "language.h"
#include "util.h"
#include "memberlist.h"
#include "message.h"
#include "membergroup.h"
#include "doxygen.h"
#include "page.h"

GroupDef::GroupDef(const char *df,int dl,const char *na,const char *t) : 
   Definition(df,dl,na)
{
  fileList = new FileList;
  classList = new ClassList;
  groupList = new GroupList;
  namespaceList = new NamespaceList;
  pageDict = new PageSDict(257);
  exampleDict = new PageSDict(257);
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

  visited = 0;
}

GroupDef::~GroupDef()
{
  delete fileList;
  delete classList;
  delete groupList;
  delete namespaceList;
  delete pageDict;
  delete exampleDict;
  delete allMemberList;
  delete allMemberDict;
  delete memberGroupList;
  delete memberGroupDict;
}

void GroupDef::distributeMemberGroupDocumentation()
{
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void GroupDef::addFile(const FileDef *def)
{
  if (Config::sortMembersFlag)
    fileList->inSort(def);
  else
    fileList->append(def);
}

void GroupDef::addClass(const ClassDef *def)
{
  if (Config::sortMembersFlag)
    classList->inSort(def);
  else
    classList->append(def);
}

void GroupDef::addNamespace(const NamespaceDef *def)
{
  if (Config::sortMembersFlag)
    namespaceList->inSort(def);  
  else
    namespaceList->append(def);
}

void GroupDef::addPage(PageInfo *def)
{
  pageDict->append(def->name,def);
  def->inGroup = this;
}

void GroupDef::addExample(const PageInfo *def)
{
  exampleDict->append(def->name,def);
}

void GroupDef::addMemberListToGroup(MemberList *ml,
                                bool (MemberDef::*func)() const)
{
  if (ml==0) return;
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (;(md=mli.current());++mli)
  {
    int groupId=md->getMemberGroupId();
    if ((md->*func)() && groupId!=-1)
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
}

void GroupDef::addMembersToMemberGroup()
{
  addMemberListToGroup(allMemberList,&MemberDef::isDefine);
  addMemberListToGroup(allMemberList,&MemberDef::isTypedef);
  addMemberListToGroup(allMemberList,&MemberDef::isEnumerate);
  addMemberListToGroup(allMemberList,&MemberDef::isEnumValue);
  addMemberListToGroup(allMemberList,&MemberDef::isFunction);
  addMemberListToGroup(allMemberList,&MemberDef::isSlot);
  addMemberListToGroup(allMemberList,&MemberDef::isSignal);
  addMemberListToGroup(allMemberList,&MemberDef::isVariable);
}


void GroupDef::insertMember(MemberDef *md)
{
  QCString funcDecl=md->name()+md->argsString();
  if (allMemberDict->find(funcDecl)==0)
  {
    allMemberList->append(md); 
    allMemberDict->insert(funcDecl,md);
    switch(md->memberType())
    {
      case MemberDef::Variable:     
        if (Config::sortMembersFlag)
          varMembers.inSort(md); 
        else
          varMembers.append(md);
        break;
      case MemberDef::Function: 
        if (Config::sortMembersFlag)    
          funcMembers.inSort(md); 
        else
          funcMembers.append(md);
        break;
      case MemberDef::Typedef:      
        if (Config::sortMembersFlag)
          typedefMembers.inSort(md); 
        else
          typedefMembers.append(md);
        break;
      case MemberDef::Enumeration:  
        if (Config::sortMembersFlag)
          enumMembers.inSort(md); 
        else
          enumMembers.append(md);
        break;
      case MemberDef::EnumValue:    
        if (Config::sortMembersFlag)
          enumValMembers.inSort(md); 
        else
          enumValMembers.append(md);
        break;
      case MemberDef::Prototype:    
        if (Config::sortMembersFlag)
          protoMembers.inSort(md); 
        else
          protoMembers.append(md);
        break;
      case MemberDef::Define:       
        if (Config::sortMembersFlag)
          defineMembers.inSort(md); 
        else
          defineMembers.append(md);
        break;
      default:
        err("FileDef::insertMembers(): unexpected member insert in file!\n");
    }
    //addMemberToGroup(md,groupId);
  }
}

void GroupDef::addGroup(const GroupDef *def)
{
  if (Config::sortMembersFlag)
    groupList->inSort(def);
  else
    groupList->append(def);
}

int GroupDef::countMembers() const
{
  return fileList->count()+
         classList->count()+
         namespaceList->count()+
         groupList->count()+
         allMemberList->count()+
         pageDict->count()+
         exampleDict->count();
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
  endTitle(ol,getOutputFileBase(),title);

  //brief=brief.stripWhiteSpace();
  //int bl=brief.length();
  OutputList briefOutput(&ol);
  if (!briefDescription().isEmpty())
  {
    parseDoc(briefOutput,defFileName,defLine,name(),0,briefDescription());
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

  if (!Config::genTagFile.isEmpty()) 
  {
    tagFile << "  <compound kind=\"group\">" << endl;
    tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    tagFile << "    <title>" << convertToXML(title) << "</title>" << endl;
    tagFile << "    <filename>" << convertToXML(fileName) << ".html</filename>" << endl;
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
      ol.docify("file ");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      if (!Config::genTagFile.isEmpty()) 
      {
        tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
      }
      ol.endMemberItem(FALSE);
      if (!fd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,defFileName,defLine,0,0,fd->briefDescription());
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
      ol.docify("namespace ");
      ol.insertMemberAlign();
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),0,nd->name());
      if (!Config::genTagFile.isEmpty()) 
      {
        tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
      }
      ol.endMemberItem(FALSE);
      if (!nd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,defFileName,defLine,0,0,nd->briefDescription());
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
      if (!Config::genTagFile.isEmpty()) 
      {
        tagFile << "    <subgroup>" << convertToXML(gd->name()) << "</subgroup>" << endl;
      }
      ol.endMemberItem(FALSE);
      if (!gd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,defFileName,defLine,0,0,gd->briefDescription());
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


  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    
    if (pageDict->count()!=countMembers()) // classical layout
    {
      ol.writeRuler();
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::RTF);
      ol.writeAnchor(0,"_details");
      ol.popGeneratorState();
      ol.startGroupHeader();
      parseText(ol,theTranslator->trDetailedDescription());
      ol.endGroupHeader();

      // repeat brief description
      if (!briefDescription().isEmpty() && Config::repeatBriefFlag)
      {
        ol+=briefOutput;
        ol.newParagraph();
      }
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      parseDoc(ol,defFileName,defLine,name(),0,documentation()+"\n");
    }
  }
  PageInfo *pi=0;
  PageSDictIterator pdi(*pageDict);
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    if (!pi->isReference())
    {
      QCString pageName = pi->getOutputFileBase();

      if (!Config::genTagFile.isEmpty()) 
      {
        tagFile << "    <page>" << convertToXML(pageName) << "</page>" << endl;
      }

      SectionInfo *si=0;
      if (!pi->title.isEmpty() && !pi->name.isEmpty() &&
          (si=sectionDict[pi->name])!=0)
      {
        ol.startSection(si->label,si->title,TRUE);
        ol.docify(si->title);
        ol.endSection(si->label,TRUE);
      }
      ol.startTextBlock();
      parseDoc(ol,pi->defFileName,pi->defLine,0,0,pi->doc);
      ol.endTextBlock();
    }
  }

  defineMembers.countDocMembers(TRUE);
  if (defineMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDefineDocumentation());
    ol.endGroupHeader();
    defineMembers.writeDocumentation(ol,name(),this);
  }
  
  protoMembers.countDocMembers(TRUE); 
  if (protoMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionPrototypeDocumentation());
    ol.endGroupHeader();
    protoMembers.writeDocumentation(ol,name(),this);
  }

  typedefMembers.countDocMembers(TRUE);
  if (typedefMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trTypedefDocumentation());
    ol.endGroupHeader();
    typedefMembers.writeDocumentation(ol,name(),this);
  }
  
  enumMembers.countDocMembers(TRUE);
  if (enumMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationTypeDocumentation());
    ol.endGroupHeader();
    enumMembers.writeDocumentation(ol,name(),this);
  }

  funcMembers.countDocMembers(TRUE);
  if (funcMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionDocumentation());
    ol.endGroupHeader();
    funcMembers.writeDocumentation(ol,name(),this);
  }
  
  varMembers.countDocMembers(TRUE);
  if (varMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trVariableDocumentation());
    ol.endGroupHeader();
    varMembers.writeDocumentation(ol,name(),this);
  }

  if (!Config::genTagFile.isEmpty()) 
  {
    tagFile << "  </compound>" << endl;
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

/*! Add a member to all groups it is contained in */
void addMemberToGroups(Entry *root,MemberDef *md)
{
  QListIterator<QCString> sli(*root->groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    GroupDef *gd=0;
    if (!s->isEmpty() && (gd=groupDict[*s]))
    {
      GroupDef *mgd = md->getGroupDef();
      if (mgd==0)
      {
        gd->insertMember(md);
        md->setGroupDef(gd);
      }
      else if (mgd!=gd)
      {
        warn(mgd->getDefFileName(),mgd->getDefLine(),
             "Warning: Member %s found in multiple groups.!\n"
             "The member will be put in group %s, and not in group %s",
              md->name().data(),mgd->name().data(),gd->name().data()
            );
      }
      //printf("Member %s: in group %s\n",md->name().data(),s->data());
    }
  }
}


void addExampleToGroups(Entry *root,PageInfo *eg)
{
  QListIterator<QCString> sli(*root->groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    GroupDef *gd=0;
    if (!s->isEmpty() && (gd=groupDict[*s]))
    {
      gd->addExample(eg);
      //printf("Example %s: in group %s\n",eg->name().data(),s->data());
    }
  }
}


