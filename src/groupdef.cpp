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

#include <ctype.h>
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
  ol.disable(OutputGenerator::Man);
  startFile(ol,fileName,title);
  startTitle(ol);
  ol.docify(title);
  endTitle(ol,name());

  //brief=brief.stripWhiteSpace();
  //int bl=brief.length();
  OutputList briefOutput(&ol);
  if (!briefDescription().isEmpty())
  {
    parseDoc(briefOutput,name(),0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.disable(OutputGenerator::Latex);
    ol.startTextLink(0,"details");
    parseDoc(ol,0,0,theTranslator->trMore());
    ol.enable(OutputGenerator::Latex);
  }
  if (fileList->count()>0)
  {
    ol.startMemberHeader();
    parseDoc(ol,0,0,theTranslator->trFiles());
    ol.endMemberHeader();
    ol.startIndexList();
    FileDef *fd=fileList->first();
    while (fd)
    {
      ol.writeStartAnnoItem("file ",fd->diskName(),0,fd->name());
      ol.writeEndAnnoItem(fd->name());
      fd=fileList->next();
    }
    ol.endIndexList();
  }
  if (classList->count()>0)
  {
    ol.startMemberHeader();
    parseDoc(ol,0,0,theTranslator->trCompounds());
    ol.endMemberHeader();
    ol.startIndexList();
    ClassDef *cd=classList->first();
    while (cd)
    {
      QString type;
      switch (cd->compoundType())
      {
        case ClassDef::Class:  type="class";  break;
        case ClassDef::Struct: type="struct"; break;
        case ClassDef::Union:  type="union";  break;
      }
      ol.writeStartAnnoItem(type,cd->classFile(),0,cd->name());
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
    ol.writeAnchor("details");
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trDetailedDescription());
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
  ol.enable(OutputGenerator::Man);
}
