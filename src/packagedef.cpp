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
#include "packagedef.h"
#include "qtbc.h"
#include "classdef.h"
#include "classlist.h"
#include "outputlist.h"
#include "util.h"
#include "config.h"
#include "doc.h"
#include "language.h"
#include "doxygen.h"

PackageDef::PackageDef(const char *df,int dl,const char *na) : 
   Definition(df,dl,na)
{
  classList       = new ClassList;
  packageFileName = "package_"+convertFileName(na);
}

PackageDef::~PackageDef()
{
  delete classList;
}

void PackageDef::addClass(const ClassDef *def)
{
  if (Config::sortMembersFlag)
    classList->inSort(def);
  else
    classList->append(def);
}

void PackageDef::writeDocumentation(OutputList &ol)
{
  QCString title = theTranslator->trPackage(name());
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,getOutputFileBase(),title);
  startTitle(ol,getOutputFileBase());
  ol.docify(title);
  endTitle(ol,getOutputFileBase(),title);

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
    ol.popGeneratorState();
  }

  if (!Config::genTagFile.isEmpty())
  {
    tagFile << "  <compound kind=\"package\">" << endl;
  }
  
  ol.startMemberSections();
  ClassDef::CompoundType ct;
  ct=ClassDef::Interface;
  classList->writeDeclaration(ol,&ct,theTranslator->trInterfaces());
  ct=ClassDef::Class;
  classList->writeDeclaration(ol,&ct,theTranslator->trClasses());
  ol.endMemberSections();

  if (!Config::genTagFile.isEmpty())
  {
    tagFile << "  </compound>" << endl;
  }
  
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
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

    // write documentation
    if (!documentation().isEmpty())
    {
      parseDoc(ol,defFileName,defLine,name(),0,documentation()+"\n");
    }
  }

  endFile(ol); 
  ol.popGeneratorState();
}

