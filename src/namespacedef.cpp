/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
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

NamespaceDef::NamespaceDef(const char *name,const char *ref) : Definition(name)
{
  fileName="namespace_"+nameToFile(name);
  classList = new ClassList;
  memList = new MemberList;
  setReference(ref);
}

NamespaceDef::~NamespaceDef()
{
  delete classList;
  delete memList;
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
  memList->append(md);
}

void NamespaceDef::computeAnchors()
{
  setAnchors('a',memList);
}

void NamespaceDef::writeDocumentation(OutputList &ol)
{
  QCString pageTitle=name()+" Namespace Reference";
  startFile(ol,fileName,pageTitle);
  startTitle(ol);
  //ol.docify(pageTitle);
  parseText(ol,theTranslator->trNamespaceReference(name()));
  endTitle(ol,name());
  
  if (Config::genTagFile.length()>0) tagFile << "%" << name() << ":\n";
  
  OutputList briefOutput(&ol); 
  if (briefDescription()) 
  {
    parseDoc(briefOutput,name(),0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    parseText(ol,theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();
  }
  ol.disable(OutputGenerator::Man);
  ol.newParagraph();
  ol.enable(OutputGenerator::Man);
  ol.writeSynopsis();
  
  ol.startMemberSections();
  if (classList->count()>0)
  {
    ClassDef *cd=classList->first();
    bool found=FALSE;
    while (cd)
    {
      if (cd->isLinkable())
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
        if (cd->hasDocumentation()) 
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
  
  memList->writeDeclarations(ol,0,this,0,0,0);
  ol.endMemberSections();
  
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    ol.writeRuler();
    bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.writeAnchor("_details"); 
    if (latexOn) ol.enable(OutputGenerator::Latex);
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
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
  }

  memList->countDocMembers();

  if ( memList->protoCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trFunctionPrototypeDocumentation());
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::Prototype);
  }

  if ( memList->typedefCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trTypedefDocumentation());
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::Typedef);
  }

  if ( memList->enumCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationTypeDocumentation());
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::Enumeration);
  }

  if ( memList->enumValueCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::EnumValue);
  }

  int cnt;
  if ( (cnt=memList->funcCount()>0) )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    QCString cntString;
    //cntString.sprintf(" (%d)",cnt);
    parseText(ol,theTranslator->trFunctionDocumentation()+cntString);
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::Function);
  }

  if ( memList->varCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trVariableDocumentation());
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::Variable);
  }

  // write Author section (Man only)
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  parseText(ol,theTranslator->trAuthor());
  ol.endGroupHeader();
  parseText(ol,theTranslator->trGeneratedAutomatically(Config::projectName));
  ol.enableAll();
  endFile(ol);
}

int NamespaceDef::countMembers()
{
  memList->countDocMembers();
  return memList->totalCount()+classList->count();
}
