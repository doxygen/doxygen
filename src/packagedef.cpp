/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include "language.h"
#include "doxygen.h"

PackageDef::PackageDef(const char *df,int dl,const char *na,const char *ref) : 
   Definition(df,dl,na)
{
  classSDict       = new ClassSDict(17);
  packageFileName = (QCString)"package_"+na;
  setReference(ref);
}

PackageDef::~PackageDef()
{
  delete classSDict;
}

void PackageDef::addClass(const ClassDef *cd)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    classSDict->inSort(cd->name(),cd);
  else
    classSDict->append(cd->name(),cd);
}

void PackageDef::writeDocumentation(OutputList &ol)
{
  QCString title = theTranslator->trPackage(name());
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,getOutputFileBase(),name(),title);
  startTitle(ol,getOutputFileBase());
  ol.docify(title);
  endTitle(ol,getOutputFileBase(),title);

  if (!briefDescription().isEmpty())
  {
    ol.parseDoc(briefFile(),briefLine(),name(),0,briefDescription(),FALSE);
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.startTextLink(0,"_details");
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    ol.popGeneratorState();
  }

  if (!Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    Doxygen::tagFile << "  <compound kind=\"package\">" << endl;
  }
  
  ol.startMemberSections();
  ClassDef::CompoundType ct;
  ct=ClassDef::Interface;
  classSDict->writeDeclaration(ol,&ct,theTranslator->trInterfaces());
  ct=ClassDef::Class;
  classSDict->writeDeclaration(ol,&ct,theTranslator->trClasses());
  ol.endMemberSections();

  if (!Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
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
    ol.parseText(theTranslator->trDetailedDescription());
    ol.endGroupHeader();

    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),name(),0,briefDescription(),FALSE);
      ol.newParagraph();
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),name(),0,documentation()+"\n",FALSE);
    }
  }

  endFile(ol); 
  ol.popGeneratorState();
}

QCString PackageDef::getOutputFileBase() const 
{ 
  return convertNameToFile(packageFileName); 
}

