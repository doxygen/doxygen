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

static QCString idToName(int id)
{
  QCString result;
  result.sprintf("mgroup_%d",id);
  return result;
}

MemberGroup::MemberGroup(int id,const char *hdr) : Definition(idToName(id))
{
  memberList = new MemberList;
  grpId = id;
  grpHeader = hdr;
  fileName = name();
}

MemberGroup::~MemberGroup()
{
  delete memberList;
}

void MemberGroup::insertMember(MemberDef *m)
{
  memberList->append(m);
}

void MemberGroup::writeDocumentation(OutputList &ol)
{
  //printf("Writing documentation for group %s\n",fileName.data());

  if (memberList->count()==0) return; // no member in this group!
  
  QCString title = grpHeader.copy();
  if (title.isEmpty())
  {
    title = "Member Group"; // TODO: make translatable.
  }
  ol.disable(OutputGenerator::Man);
  startFile(ol,fileName,title);
  startTitle(ol,getOutputFileBase());
  ol.docify(title);
  endTitle(ol,getOutputFileBase(),name());
  

  OutputList briefOutput(&ol);
  if (!briefDescription().isEmpty())
  {
    parseDoc(briefOutput,0,0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    if (!documentation().isEmpty() || Config::repeatBriefFlag)
    {
      //bool htmlEnabled = ol.isEnabled(OutputGenerator::Html);
      //bool latexEnabled = ol.isEnabled(OutputGenerator::Latex);
      //if (htmlEnabled) ol.disable(OutputGenerator::Html);
      //if (latexEnabled) ol.disable(OutputGenerator::Latex);
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.startTextLink(0,"_details");
      parseText(ol,theTranslator->trMore());
      ol.endTextLink();
      ol.popGeneratorState();
      //if (htmlEnabled) ol.enable(OutputGenerator::Html);
      //if (latexEnabled) ol.enable(OutputGenerator::Latex);
    }
  }
  // TODO: man page synopsis
  
  //if (memberList->count()>0)
  //{
  QCString scopeName;
 
  MemberDef *md    = memberList->first();
  ClassDef *cd     = md->memberClass();
  NamespaceDef *nd = md->getNamespace();
  FileDef *fd      = md->getFileDef();
  //printf("member %s brief=`%s' docs=`%s'\n",
  //     md->name().data(), md->briefDescription().data(),
  //     md->documentation().data());
  if (cd)
  {
    ol.newParagraph();
    ol.docify("Inside class ");
    ol.writeObjectLink(cd->getReference(),
                       cd->getOutputFileBase(),0,
                       cd->name()
                      );
    scopeName=cd->name().copy();
  }
  else if (nd)
  {
    ol.newParagraph();
    ol.docify("Inside namespace ");
    ol.writeObjectLink(nd->getReference(),
                       nd->getOutputFileBase(),0,
                       nd->name()
                      );
    scopeName=nd->name().copy();
  }
  else if (fd)
  {
    ol.newParagraph();
    ol.docify("#include <");
    ol.writeObjectLink(fd->getReference(),
                       fd->getOutputFileBase(),0,
                       fd->name()
                      );
    ol.docify(">");
  }
  ol.startMemberSections();
  memberList->writeDeclarations(ol,cd,nd,fd,"Synopsis",0,TRUE);
  ol.endMemberSections();

  if ((!briefDescription().isEmpty() && Config::repeatBriefFlag) || 
      !documentation().isEmpty())
  {
    ol.writeRuler();
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeAnchor("_details");
    //if (latexOn) ol.enable(OutputGenerator::Latex);
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
    // write documentation
    if (!documentation().isEmpty())
    {
      parseDoc(ol,scopeName,0,documentation()+"\n");
    }
  }

//  memberList->countDocMembers(TRUE);
//   
//  if ( memberList->defineCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trDefineDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::Define);
//  }
//  
//  if ( memberList->protoCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trFunctionPrototypeDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::Prototype);
//  }
//
//  if ( memberList->typedefCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trTypedefDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::Typedef);
//  }
//  
//  if ( memberList->enumCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trEnumerationTypeDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::Enumeration);
//  }
//
//  if ( memberList->enumValueCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trEnumerationValueDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::EnumValue);
//  }
//
//  if ( memberList->funcCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trFunctionDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::Function);
//  }
//  
//  if ( memberList->varCount()>0 )
//  {
//    ol.writeRuler();
//    ol.startGroupHeader();
//    parseText(ol,theTranslator->trVariableDocumentation());
//    ol.endGroupHeader();
//    writeMemberDocs(ol,memberList,scopeName,MemberDef::Variable);
//  }
  
  endFile(ol); 
  ol.enable(OutputGenerator::Man);
}

bool MemberGroup::isLinkableInProject()
{
  return hasDocumentation() && !isReference();
}
bool MemberGroup::isLinkable()
{
  return isLinkableInProject() || isReference();
}

void MemberGroup::addDocumentation()
{
  //printf("adding documentation for membergroup %s\n",name().data());
  MemberListIterator mli(*memberList);
  MemberDef *md;
  for (;(md=mli.current());++mli)
  {
    //printf("Adding docs `%s' `%s'\n",md->briefDescription().data(),md->documentation().data());
    setBriefDescription(briefDescription()+"\n"+md->briefDescription());
    setDocumentation(documentation()+"\n"+md->documentation()); 
  }
  //printf("isLinkable()=%d\n",isLinkable());
}
