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

#include "namespacedef.h"
#include "outputlist.h"
#include "util.h"
#include "scanner.h"
#include "language.h"
#include "classdef.h"
#include "classlist.h"
#include "memberlist.h"

NamespaceDef::NamespaceDef(const char *name) : Definition(name)
{
  fileName="namespace_"+nameToFile(name);
  classList = new ClassList;
  memList = new MemberList;
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
  QString pageTitle=name()+" Namespace Reference";
  startFile(ol,fileName,pageTitle);
  startTitle(ol);
  ol.docify(pageTitle);
  endTitle(ol,name());
  
  OutputList briefOutput(&ol); 
  if (briefDescription()) 
  {
    parseDoc(briefOutput,0,0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"details");
    parseDoc(ol,0,0,theTranslator->trMore());
    ol.endTextLink();
    ol.enableAll();
  }
  ol.disable(OutputGenerator::Man);
  ol.newParagraph();
  ol.enable(OutputGenerator::Man);
  ol.writeSynopsis();
  
  if (classList->count()>0)
  {
    ClassDef *cd=classList->first();
    bool found=FALSE;
    while (cd)
    {
      if (cd->isVisibleExt())
      {
        if (!found)
        {
          ol.startMemberHeader();
          parseDoc(ol,0,0,theTranslator->trCompounds());
          ol.endMemberHeader();
          ol.startMemberList();
          found=TRUE;
        }
        QString clName=cd->name().copy();
        if (clName.left(name().length()+2)==name()+"::")
        {
          clName = clName.right(clName.length()-name().length()-2);
        }
        
        ol.startMemberItem();
        switch (cd->compoundType())
        {
          case ClassDef::Class:  ol.writeString("class");  break;
          case ClassDef::Struct: ol.writeString("struct"); break;
          case ClassDef::Union:  ol.writeString("union");  break;
        }
        ol.writeString(" ");
        if (cd->hasDocumentation()) 
        {
          ol.writeObjectLink(cd->getReference(),
                            cd->classFile(),
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
        ol.endMemberItem();
      }
      cd=classList->next();
    }
    if (found) ol.endMemberList();
  }
  
  writeMemberDecs(ol,0,this,0,0,0,memList);
  
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.writeAnchor("details"); 
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    if (!briefDescription().isEmpty())
    {
      ol+=briefOutput;
      ol.newParagraph();
    }
    if (!documentation().isEmpty())
    {
      parseDoc(ol,0,0,documentation()+"\n");
      ol.newParagraph();
    }
  }

  memList->countDocMembers();

  if ( memList->protoCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trFunctionPrototypeDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,memList,name(),MemberDef::Prototype);
  }

  if ( memList->typedefCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trTypedefDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,memList,name(),MemberDef::Typedef);
  }

  if ( memList->enumCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trEnumerationTypeDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,memList,name(),MemberDef::Enumeration);
  }

  if ( memList->enumValueCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,memList,name(),MemberDef::EnumValue);
  }

  if ( memList->funcCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trFunctionDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,memList,name(),MemberDef::Function);
  }

  if ( memList->varCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trVariableDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,memList,name(),MemberDef::Variable);
  }

  // write Author section (Man only)
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  parseDoc(ol,0,0,theTranslator->trAuthor());
  ol.endGroupHeader();
  parseDoc(ol,0,0,theTranslator->trGeneratedAutomatically(projectName));
  ol.enableAll();
  endFile(ol);
}
