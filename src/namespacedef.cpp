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
#include "namespacedef.h"
#include "outputlist.h"
#include "util.h"
#include "scanner.h"
#include "language.h"
#include "classdef.h"
#include "classlist.h"
#include "memberlist.h"
#include "doxygen.h"
#include "message.h"

NamespaceDef::NamespaceDef(const char *df,int dl,
                           const char *name,const char *ref) : 
   Definition(df,dl,name)
{
  fileName="namespace_"+nameToFile(name);
  classList = new ClassList;
  classDict = new ClassDict(1009);
  //memList = new MemberList;
  usingDirList = 0;
  usingDeclList = 0;
  setReference(ref);
  memberGroupList = new MemberGroupList;
  memberGroupList->setAutoDelete(TRUE);
  memberGroupDict = new MemberGroupDict(1009);
  defFileName = df;
  defLine = dl;
}

NamespaceDef::~NamespaceDef()
{
  delete classList;
  delete classDict;
  delete usingDirList;
  delete usingDeclList;
  delete memberGroupList;
  delete memberGroupDict;
}

void NamespaceDef::distributeMemberGroupDocumentation()
{
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}
void NamespaceDef::insertUsedFile(const char *f)
{
  if (files.find(f)==-1) files.append(f);
}

void NamespaceDef::insertClass(ClassDef *cd)
{
  if (classDict->find(cd->name())==0)
  {
    classList->append(cd);
    classDict->insert(cd->name(),cd);
  }
}

void NamespaceDef::addMemberToGroup(MemberDef *md,int groupId)
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

void NamespaceDef::insertMember(MemberDef *md,int groupId)
{
  //memList->append(md);
  allMemberList.append(md); 
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
       err("NamespaceDef::insertMembers(): unexpected member inserted in namespace!\n");
  }
  addMemberToGroup(md,groupId);
}

void NamespaceDef::computeAnchors()
{
  setAnchors('a',&allMemberList);
  //MemberGroupListIterator mgli(*memberGroupList);
  //MemberGroup *mg;
  //for (;(mg=mgli.current());++mgli)
  //{
  //  mg->setAnchors();
  //}
}

void NamespaceDef::writeDocumentation(OutputList &ol)
{
  QCString pageTitle=name()+" Namespace Reference";
  startFile(ol,fileName,pageTitle);
  startTitle(ol,getOutputFileBase());
  //ol.docify(pageTitle);
  parseText(ol,theTranslator->trNamespaceReference(name()));
  endTitle(ol,getOutputFileBase(),name());
  
  if (!Config::genTagFile.isEmpty()) tagFile << "%" << name() << ":\n";
  
  ol.startTextBlock();
    
  OutputList briefOutput(&ol); 
  if (!briefDescription().isEmpty()) 
  {
    parseDoc(briefOutput,defFileName,defLine,name(),0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    parseText(ol,theTranslator->trMore());
    ol.endTextLink();
    ol.popGeneratorState();
  }
  ol.disable(OutputGenerator::Man);
  ol.newParagraph();
  ol.enable(OutputGenerator::Man);
  ol.writeSynopsis();

  ol.endTextBlock();
  
  ol.startMemberSections();
  classList->writeDeclaration(ol);

  /* write user defined member groups */
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->writeDeclarations(ol,0,this,0,0);
  }
  
  allMemberList.writeDeclarations(ol,0,this,0,0,0,0);
  ol.endMemberSections();
  
  if ((!briefDescription().isEmpty() && Config::repeatBriefFlag) || 
      !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.writeAnchor(0,"_details"); 
    //if (latexOn) ol.enable(OutputGenerator::Latex);
    ol.popGeneratorState();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    ol.startTextBlock();
    if (!briefDescription().isEmpty())
    {
      ol+=briefOutput;
      ol.newParagraph();
    }
    if (!documentation().isEmpty())
    {
      parseDoc(ol,defFileName,defLine,name(),0,documentation()+"\n");
      ol.newParagraph();
    }
    ol.endTextBlock();
  }

  //memList->countDocMembers();
  defineMembers.countDocMembers();
  if ( defineMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDefineDocumentation());
    ol.endGroupHeader();
    defineMembers.writeDocumentation(ol,name());
  }
  
  protoMembers.countDocMembers(); 
  if ( protoMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionPrototypeDocumentation());
    ol.endGroupHeader();
    protoMembers.writeDocumentation(ol,name());
  }

  typedefMembers.countDocMembers();
  if ( typedefMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trTypedefDocumentation());
    ol.endGroupHeader();
    typedefMembers.writeDocumentation(ol,name());
  }
  
  enumMembers.countDocMembers();
  if ( enumMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationTypeDocumentation());
    ol.endGroupHeader();
    enumMembers.writeDocumentation(ol,name());
  }

  //enumValMembers.countDocMembers();
  //if ( enumValMembers.totalCount()>0 )
  //{
  //  ol.writeRuler();
  //  ol.startGroupHeader();
  //  parseText(ol,theTranslator->trEnumerationValueDocumentation());
  //  ol.endGroupHeader();
  //  enumValMembers.writeDocumentation(ol,name());
  //}

  funcMembers.countDocMembers();
  if ( funcMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionDocumentation());
    ol.endGroupHeader();
    funcMembers.writeDocumentation(ol,name());
  }
  
  varMembers.countDocMembers();
  if ( varMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trVariableDocumentation());
    ol.endGroupHeader();
    varMembers.writeDocumentation(ol,name());
  }

  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  parseText(ol,theTranslator->trAuthor());
  ol.endGroupHeader();
  parseText(ol,theTranslator->trGeneratedAutomatically(Config::projectName));
  //ol.enableAll();
  ol.popGeneratorState();
  endFile(ol);
}

int NamespaceDef::countMembers()
{
  allMemberList.countDocMembers();
  return allMemberList.totalCount()+classList->count();
}

void NamespaceDef::addUsingDirective(NamespaceDef *nd)
{
  if (usingDirList==0)
  {
    usingDirList = new NamespaceList;
  }
  usingDirList->append(nd);
}

void NamespaceDef::addUsingDeclaration(ClassDef *cd)
{
  if (usingDeclList==0)
  {
    usingDeclList = new ClassList;
  }
  usingDeclList->append(cd);
}
