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

NamespaceDef::NamespaceDef(const char *name,const char *ref) : Definition(name)
{
  fileName="namespace_"+nameToFile(name);
  classList = new ClassList;
  //memList = new MemberList;
  usingList = 0;
  setReference(ref);
}

NamespaceDef::~NamespaceDef()
{
  delete classList;
  //delete memList;
  delete usingList;
}

void NamespaceDef::insertUsedFile(const char *f)
{
  if (files.find(f)==-1) files.append(f);
}

void NamespaceDef::insertClass(ClassDef *cd)
{
  classList->append(cd);
}

void NamespaceDef::insertMember(MemberDef *md)
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
       err("NamespaceDef::insertMembers(): unexpected member insert in file!\n");
  }
}

void NamespaceDef::computeAnchors()
{
  setAnchors('a',&allMemberList);
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
    parseDoc(briefOutput,name(),0,briefDescription());
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
  if (classList->count()>0)
  {
    ClassDef *cd=classList->first();
    bool found=FALSE;
    while (cd)
    {
      if (cd->name().find('@')==-1)
      {
        if (!found)
        {
          ol.startMemberHeader();
          parseText(ol,theTranslator->trCompounds());
          ol.endMemberHeader();
          ol.startMemberList();
          found=TRUE;
        }
        QCString clName=cd->name().copy();
        if (clName.left(name().length()+2)==name()+"::")
        {
          clName = clName.right(clName.length()-name().length()-2);
        }
        
        ol.startMemberItem(FALSE,0);
        switch (cd->compoundType())
        {
          case ClassDef::Class:      ol.writeString("class");      break;
          case ClassDef::Struct:     ol.writeString("struct");     break;
          case ClassDef::Union:      ol.writeString("union");      break;
          case ClassDef::Interface:  ol.writeString("interface");  break;
        }
        ol.writeString(" ");
        ol.insertMemberAlign();
        if (cd->isLinkable()) 
        {
          ol.writeObjectLink(cd->getReference(),
                             cd->getOutputFileBase(),
                             0,
                             clName
                            );
        }
        else
        {
          ol.startBold();
          ol.docify(clName);
          ol.endBold();
        }
        ol.endMemberItem(FALSE,0,0,FALSE);
      }
      cd=classList->next();
    }
    if (found) ol.endMemberList();
  }
  
  /*memList->*/allMemberList.writeDeclarations(ol,0,this,0,0,0);
  ol.endMemberSections();
  
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.writeAnchor("_details"); 
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
      parseDoc(ol,name(),0,documentation()+"\n");
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

  enumValMembers.countDocMembers();
  if ( enumValMembers.totalCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
    enumValMembers.writeDocumentation(ol,name());
  }

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
  if (usingList==0)
  {
    usingList = new NamespaceList;
  }
  usingList->append(nd);
}
