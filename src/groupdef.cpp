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
  parentGroupList = new GroupList;
  namespaceList = new NamespaceList;
  pageDict = new PageSDict(257);
  exampleDict = new PageSDict(257);
  allMemberList = new MemberList;
  allMemberNameInfoDict = new MemberNameInfoDict(1009);
  if (t) 
    title = t;
  else
  {
    title = na;
    title.at(0)=toupper(title.at(0));
  }
  fileName = (QCString)"group_"+na;
  memberGroupList = new MemberGroupList;
  memberGroupList->setAutoDelete(TRUE);
  memberGroupDict = new MemberGroupDict(1009);

  decDefineMembers.setInGroup(TRUE);
  decProtoMembers.setInGroup(TRUE);
  decTypedefMembers.setInGroup(TRUE);
  decEnumMembers.setInGroup(TRUE);
  decEnumValMembers.setInGroup(TRUE);
  decFuncMembers.setInGroup(TRUE);
  decVarMembers.setInGroup(TRUE);

  docDefineMembers.setInGroup(TRUE);
  docProtoMembers.setInGroup(TRUE);
  docTypedefMembers.setInGroup(TRUE);
  docEnumMembers.setInGroup(TRUE);
  docEnumValMembers.setInGroup(TRUE);
  docFuncMembers.setInGroup(TRUE);
  docVarMembers.setInGroup(TRUE);

  visited = 0;
}

GroupDef::~GroupDef()
{
  delete fileList;
  delete classList;
  delete groupList;
  delete parentGroupList;
  delete namespaceList;
  delete pageDict;
  delete exampleDict;
  delete allMemberList;
  delete allMemberNameInfoDict;
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
  if (Config_getBool("SORT_MEMBER_DOCS"))
    fileList->inSort(def);
  else
    fileList->append(def);
}

void GroupDef::addClass(const ClassDef *def)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    classList->inSort(def);
  else
    classList->append(def);
}

void GroupDef::addNamespace(const NamespaceDef *def)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
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

#if 0
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
      QCString *pGrpHeader = Doxygen::memberHeaderDict[groupId];
      QCString *pDocs      = Doxygen::memberDocDict[groupId];
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
#endif


void GroupDef::addMembersToMemberGroup()
{
#if 0
  addMemberListToGroup(allMemberList,&MemberDef::isDefine);
  addMemberListToGroup(allMemberList,&MemberDef::isTypedef);
  addMemberListToGroup(allMemberList,&MemberDef::isEnumerate);
  addMemberListToGroup(allMemberList,&MemberDef::isEnumValue);
  addMemberListToGroup(allMemberList,&MemberDef::isFunction);
  addMemberListToGroup(allMemberList,&MemberDef::isSlot);
  addMemberListToGroup(allMemberList,&MemberDef::isSignal);
  addMemberListToGroup(allMemberList,&MemberDef::isVariable);
#endif

  ::addMembersToMemberGroup(&decDefineMembers,memberGroupDict,memberGroupList);
  ::addMembersToMemberGroup(&decProtoMembers,memberGroupDict,memberGroupList);
  ::addMembersToMemberGroup(&decTypedefMembers,memberGroupDict,memberGroupList);
  ::addMembersToMemberGroup(&decEnumMembers,memberGroupDict,memberGroupList);
  ::addMembersToMemberGroup(&decEnumValMembers,memberGroupDict,memberGroupList);
  ::addMembersToMemberGroup(&decFuncMembers,memberGroupDict,memberGroupList);
  ::addMembersToMemberGroup(&decVarMembers,memberGroupDict,memberGroupList);

  //printf("GroupDef::addMembersToMemberGroup() memberGroupList=%d\n",memberGroupList->count());
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->setInGroup(TRUE);
  }
}


void GroupDef::insertMember(MemberDef *md)
{
  //printf("GroupDef::insertMember(%s)\n",md->name().data());
  MemberNameInfo *mni=0;
  if ((mni=(*allMemberNameInfoDict)[md->name()]))
  { // member with this name already found
    MemberNameInfoIterator srcMnii(*mni); 
    MemberInfo *srcMi;
    for ( ; (srcMi=srcMnii.current()) ; ++srcMnii )
    {
      MemberDef *srcMd = srcMi->memberDef;
      if (matchArguments(srcMd->argumentList(),md->argumentList()))
      {
        return; // member already added
      }
    }
    mni->append(new MemberInfo(md,md->protection(),md->virtualness(),FALSE));
  }
  else
  {
    mni = new MemberNameInfo(md->name());
    mni->append(new MemberInfo(md,md->protection(),md->virtualness(),FALSE));
    allMemberNameInfoDict->insert(mni->memberName(),mni);
  }
  allMemberList->append(md); 
  switch(md->memberType())
  {
    case MemberDef::Variable:     
      decVarMembers.append(md); 
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docVarMembers.inSort(md); 
      else
        docVarMembers.append(md);
      break;
    case MemberDef::Function: 
      decFuncMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))    
        docFuncMembers.inSort(md); 
      else
        docFuncMembers.append(md);
      break;
    case MemberDef::Typedef:      
      decTypedefMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docTypedefMembers.inSort(md); 
      else
        docTypedefMembers.append(md);
      break;
    case MemberDef::Enumeration:  
      decEnumMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docEnumMembers.inSort(md); 
      else
        docEnumMembers.append(md);
      break;
    case MemberDef::EnumValue:    
      break;
    case MemberDef::Prototype:    
      decProtoMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docProtoMembers.inSort(md); 
      else
        docProtoMembers.append(md);
      break;
    case MemberDef::Define:       
      decDefineMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docDefineMembers.inSort(md); 
      else
        docDefineMembers.append(md);
      break;
    default:
      err("GroupDef::insertMembers(): "
           "member `%s' with class scope `%s' inserted in group scope `%s'!\n",
           md->name().data(),
           md->getClassDef() ? md->getClassDef()->name().data() : "",
           name().data());
  }
  //addMemberToGroup(md,groupId);
}

void GroupDef::addGroup(const GroupDef *def)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    groupList->inSort(def);
  else
    groupList->append(def);
}

void GroupDef::addParentGroup(const GroupDef *def)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    parentGroupList->inSort(def);
  else
    parentGroupList->append(def);
}

bool GroupDef::isASubGroup() const
{
  return parentGroupList->count()!=0;
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
  //ol.disable(OutputGenerator::Man);
  startFile(ol,getOutputFileBase(),title);
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

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"group\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <title>" << convertToXML(title) << "</title>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << ".html</filename>" << endl;
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
      }
      ol.endMemberItem(FALSE);
      if (!fd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
      }
      ol.endMemberItem(FALSE);
      if (!nd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <subgroup>" << convertToXML(gd->name()) << "</subgroup>" << endl;
      }
      ol.endMemberItem(FALSE);
      if (!gd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
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

    //allMemberList->writeDeclarations(ol,0,0,0,this,0,0); 
    decDefineMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trDefines(),0);
    decProtoMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trFuncProtos(),0);
    decTypedefMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trTypedefs(),0);
    decEnumMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trEnumerations(),0);
    decFuncMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trFunctions(),0);
    decVarMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trVariables(),0);
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
      if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
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

      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <page>" << convertToXML(pageName) << "</page>" << endl;
      }

      SectionInfo *si=0;
      if (!pi->title.isEmpty() && !pi->name.isEmpty() &&
          (si=Doxygen::sectionDict[pi->name])!=0)
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

  docDefineMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trDefineDocumentation());
  
  docProtoMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trFunctionPrototypeDocumentation());

  docTypedefMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trTypedefDocumentation());
  
  docEnumMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trEnumerationTypeDocumentation());

  docFuncMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trFunctionDocumentation());
  
  docVarMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trVariableDocumentation());

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
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
    if (!s->isEmpty() && (gd=Doxygen::groupDict[*s]))
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
    if (!s->isEmpty() && (gd=Doxygen::groupDict[*s]))
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
    if (!s->isEmpty() && (gd=Doxygen::groupDict[*s]))
    {
      gd->addGroup(subGroup);
      subGroup->addParentGroup(gd);
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
    if (!s->isEmpty() && (gd=Doxygen::groupDict[*s]))
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
    if (!s->isEmpty() && (gd=Doxygen::groupDict[*s]))
    {
      gd->addExample(eg);
      //printf("Example %s: in group %s\n",eg->name().data(),s->data());
    }
  }
}

QCString GroupDef::getOutputFileBase() const 
{ 
  return convertNameToFile(fileName); 
}
