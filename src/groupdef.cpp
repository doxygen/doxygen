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

GroupDef::GroupDef(const char *na,const char *t) : Definition(na)
{
  fileList = new FileList;
  classList = new ClassList;
//  groupList = new GroupList;
//  name = n;
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

//void GroupDef::addGroup(const GroupDef *def)
//{
//  groupList->append(def);
//}

int GroupDef::countMembers() const
{
  return fileList->count()+classList->count();
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
  if (fileList->count()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trFiles());
    ol.endMemberHeader();
    ol.startIndexList();
    FileDef *fd=fileList->first();
    while (fd)
    {
      ol.writeStartAnnoItem("file ",fd->getOutputFileBase(),0,fd->name());
      ol.writeEndAnnoItem(fd->name());
      fd=fileList->next();
    }
    ol.endIndexList();
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
        ol.startIndexList();
        found=TRUE;
      }
      QCString type;
      switch (cd->compoundType())
      {
        case ClassDef::Class:      type="class";  break;
        case ClassDef::Struct:     type="struct"; break;
        case ClassDef::Union:      type="union";  break;
        case ClassDef::Interface:  type="interface";  break;
      }
      ol.writeStartAnnoItem(type,cd->getOutputFileBase(),0,cd->name());
      ol.writeEndAnnoItem(cd->name());
      cd=classList->next();
    }
    ol.endIndexList();
  }
  //int dl=doc.length();
  //doc=doc.stripWhiteSpace();
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    ol.writeRuler();
    ol.pushGeneratorState();
    //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    //if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.writeAnchor("_details");
    //if (latexOn) ol.enable(OutputGenerator::Latex);
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

  endFile(ol); 
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}
