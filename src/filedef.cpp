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
#include "memberlist.h"
#include "classlist.h"
#include "define.h"
#include "filedef.h"
#include "scanner.h"
#include "doxygen.h"
#include "memberdef.h"
#include "classdef.h"
#include "namespacedef.h"
#include "util.h"
#include "language.h"

/*! create a new file definition, where \a p is the file path, 
    \a the file name, and \a ref is an HTML anchor name if the
    file was read from a tag file or 0 otherwise
*/
FileDef::FileDef(const char *p,const char *nm,const char *ref)
   : Definition(nm)
{
  path=p;
  filepath=path+nm;
  filename=nameToFile(nm);
  setReference(ref);
  memList      = new MemberList;
  classList     = new ClassList;
  includeList   = new FileList;
  defineList    = new DefineList;
  namespaceList = new NamespaceList;
  namespaceDict = new NamespaceDict(7);
}

/*! destroy the file definition */
FileDef::~FileDef()
{
  delete memList;
  delete classList;
  delete includeList;
  delete defineList;
  delete namespaceList;
  delete namespaceDict;
}

/*! Compute the HTML anchor names for all members in the class */ 
void FileDef::computeAnchors()
{
  setAnchors('a',memList);
}

/*! Write the documentation page for this file to the file of output
    generators \a ol. 
*/
void FileDef::writeDocumentation(OutputList &ol)
{
  //funcList->countDecMembers();
  
  QCString pageTitle=name()+" File Reference";
  startFile(ol,diskname,pageTitle);
  startTitle(ol);
  parseText(ol,theTranslator->trFileReference(name()));
  endTitle(ol,name());
  //ol.newParagraph();
  
  if (Config::genTagFile.length()>0) tagFile << "&" << name() << ":\n";
  
  //brief=brief.stripWhiteSpace();
  //int bl=brief.length();
  OutputList briefOutput(&ol); 
  if (briefDescription()) 
  {
    //if (brief.at(bl-1)!='.' && brief.at(bl-1)!='!' && brief.at(bl!='?')) 
    //  brief+='.';
    parseDoc(briefOutput,0,0,briefDescription());
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

  if (namespaceList->count()>0)
  {
    NamespaceDef *nd=namespaceList->first();
    bool found=FALSE;
    while (nd)
    {
      if (nd->name().find('@')==-1)
      {
        if (!found)
        {
          ol.startMemberHeader();
          parseText(ol,theTranslator->trNamespaces());
          ol.endMemberHeader();
          ol.startMemberList();
          found=TRUE;
        }
        ol.startMemberItem(FALSE,0);
        ol.writeString("namespace ");
        ol.insertMemberAlign();
        if (nd->isLinkable()) 
        {
          ol.writeObjectLink(nd->getReference(),
              nd->getOutputFileBase(),
              0,
              nd->name()
                            );
        }
        else
        {
          ol.startBold();
          ol.docify(nd->name());
          ol.endBold();
        }
        ol.endMemberItem(FALSE,0,0,FALSE);
      }
      nd=namespaceList->next();
    }
    if (found) ol.endMemberList();
  }
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
        ol.startMemberItem(FALSE,FALSE);
        switch (cd->compoundType())
        {
          case ClassDef::Class:  ol.writeString("class");  break;
          case ClassDef::Struct: ol.writeString("struct"); break;
          case ClassDef::Union:  ol.writeString("union");  break;
          case ClassDef::Interface:  ol.writeString("interface");  break;
        }
        ol.writeString(" ");
        ol.insertMemberAlign();
        if (cd->isLinkable()) 
        {
          ol.writeObjectLink(cd->getReference(),
              cd->getOutputFileBase(),
              0,
              cd->name()
                            );
        }
        else
        {
          ol.startBold();
          ol.docify(cd->name());
          ol.endBold();
        }
        ol.endMemberItem(FALSE,0,0,FALSE);
      }
      cd=classList->next();
      if (found) ol.endMemberList();
    }
  }
  
  memList->writeDeclarations(ol,0,0,this,0,0);
  ol.endMemberSections();

  //doc=doc.stripWhiteSpace();
  //int bl=brief.length();
  //int dl=doc.length();
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
      //if (doc.at(dl-1)!='.' && doc.at(dl-1)!='!' && doc.at(dl-1)!='?') 
      //  doc+='.';
      parseDoc(ol,0,0,documentation()+"\n");
      ol.newParagraph();
    }
  }

  memList->countDocMembers();
  
  if ( memList->defineCount()>0 )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDefineDocumentation());
    ol.endGroupHeader();
    memList->writeDocumentation(ol,name(),MemberDef::Define);
  }
  
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
    cntString.sprintf(" (%d)",cnt);
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

/*! Adds member definition \a md to the list of all members of this file */
void FileDef::insertMember(MemberDef *md)
{
  memList->append(md); 
}

/*! Adds compound definition \a cd to the list of all compounds of this file */
void FileDef::insertClass(ClassDef *cd)
{
  classList->append(cd);
}

/*! Adds namespace definition \a nd to the list of all compounds of this file */
void FileDef::insertNamespace(NamespaceDef *nd)
{
  if (!nd->name().isEmpty() && namespaceDict->find(nd->name())==0)
  {
    namespaceList->append(nd);
    namespaceDict->insert(nd->name(),nd);
  }
}

//-----------------------------------------------------------------------------

/*! Creates a file list. */
FileList::FileList() : QList<FileDef>()
{
}

/*! Destroys a file list */
FileList::~FileList()
{
}

/*! Compares two files by name. */
int FileList::compareItems(GCI item1, GCI item2)
{
  FileDef *c1=(FileDef *)item1;
  FileDef *c2=(FileDef *)item2;
  return strcmp(c1->name(),c2->name());
}

/*! Create a file list iterator. */
FileListIterator::FileListIterator(const FileList &cllist) :
  QListIterator<FileDef>(cllist)
{
}
