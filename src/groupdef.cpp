/******************************************************************************
 *
 * $Id$
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

GroupDef::GroupDef(const char *na,const char *t) : 
   Definition(na)
{
  fileList = new FileList;
  classList = new ClassList;
//  groupList = new GroupList;

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
}

GroupDef::~GroupDef()
{
  delete fileList;
  delete classList;
//  delete groupList;
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

void GroupDef::addMember(const MemberDef *md)
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
  }
}

//void GroupDef::addGroup(const GroupDef *def)
//{
//  groupList->append(def);
//}

int GroupDef::countMembers() const
{
  return fileList->count()+classList->count()+allMemberList->count();
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
      ol.startMemberItem(FALSE,0);
      ol.docify("file");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      ol.endMemberItem(FALSE,0,0,FALSE);
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
  if (classList->count()>0)
  {
    ClassDef *cd=classList->first();
    bool found=FALSE;
    while (cd)
    {
      if (!found)
      {
        ol.startMemberHeader();
        parseText(ol,theTranslator->trCompounds());
        ol.endMemberHeader();
        ol.startMemberList();
        found=TRUE;
      }
      QCString type;
      switch (cd->compoundType())
      {
        case ClassDef::Class:      type="class";  break;
        case ClassDef::Struct:     type="struct"; break;
        case ClassDef::Union:      type="union";  break;
        case ClassDef::Interface:  type="interface";  break;
        case ClassDef::Exception:  type="exception";  break;
      }
      ol.startMemberItem(FALSE,0);
      ol.docify(type);
      ol.insertMemberAlign();
      ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name());
      ol.endMemberItem(FALSE,0,0,FALSE);
      if (!cd->briefDescription().isEmpty() && Config::briefMemDescFlag)
      {
        ol.startMemberDescription();
        parseDoc(ol,0,0,cd->briefDescription());
        ol.endMemberDescription();
        ol.newParagraph();
      }
      cd=classList->next();
    }
    if (found) ol.endMemberList();
  }
  if (allMemberList->count()>0)
  {
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

  enumValMembers.countDocMembers();
  if (enumValMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
    enumValMembers.writeDocumentation(ol,name());
  }

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
