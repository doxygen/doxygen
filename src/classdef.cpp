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

#include <stdio.h>
#include <qfile.h>
#include <qregexp.h>
#include "classdef.h"
#include "classlist.h"
#include "entry.h"
#include "scanner.h"
#include "doxygen.h"
#include "membername.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "htmlhelp.h"

static QCString stripExtension(const char *fName)
{
  QCString result=fName;
  if (result.right(5)==".html") result=result.left(result.length()-5);
  return result;
}

// constructs a new class definition
ClassDef::ClassDef(const char *nm,CompoundType ct,const char *ref,const char *fName) 
 : Definition(removeRedundantWhiteSpace(nm)) 
{
  //name=n;
  if (fName)
    fileName=stripExtension(fName);
  else
    fileName="class_"+nameToFile(nm);
  if (ref) 
  {
    //url=(QCString)"doxygen=\""+ref+":\" href=\""+fileName;
    exampleList = 0;
    exampleDict = 0;
  }
  else
  {
    //url="href=\""+fileName;
    exampleList = new ExampleList;
    exampleDict = new ExampleDict(29);
  }
  memListFileName="class_"+nameToFile(nm)+"-members";
  inherits      = new BaseClassList;
  inherits->setAutoDelete(TRUE);
  inheritedBy   = new BaseClassList;
  inheritedBy->setAutoDelete(TRUE);
  allMemberNameInfoList = new MemberNameInfoList; 
  allMemberNameInfoList->setAutoDelete(TRUE);
  allMemberNameInfoDict = new MemberNameInfoDict(1009);
  visited=FALSE;
  setReference(ref);
  compType=ct;
  incFile=0;
  tempArgs=0;
  prot=Public;
  nspace=0;
}

// destroy the class definition
ClassDef::~ClassDef()
{
  delete inherits;
  delete inheritedBy;
  delete allMemberNameInfoList;
  delete allMemberNameInfoDict;
  delete exampleList;
  delete exampleDict;
}

// inserts a base class in the inheritance list
void ClassDef::insertBaseClass(ClassDef *cd,Protection p,
                               Specifier s,const char *t)
{
  inherits->inSort(new BaseClassDef(cd,p,s,t));
}

// inserts a super class in the inherited list
void ClassDef::insertSuperClass(ClassDef *cd,Protection p,
                                Specifier s,const char *t)
{
  inheritedBy->inSort(new BaseClassDef(cd,p,s,t));
}

// adds new member definition to the class
void ClassDef::insertMember(const MemberDef *md)
{
  //printf("adding %s::%s\n",name(),md->name());
  if (!isReference())
  {
    if (md->isRelated() && (Config::extractPrivateFlag || md->protection()!=Private))
    {
      related.append(md);
    }
    else if (md->isFriend())
    {
      friends.append(md);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Signal:
          signals.append(md);
          break;
        case MemberDef::Slot:
          switch (md->protection())
          {
            case Protected: proSlots.append(md); break;
            case Public:    pubSlots.append(md); break;
            case Private:   priSlots.append(md); break;
          }
          break;
        default: // any of the other members
          if (md->isStatic())
          {
            switch (md->protection())
            {
              case Protected: proStaticMembers.append(md); break;
              case Public:    pubStaticMembers.append(md); break;
              case Private:   priStaticMembers.append(md); break;
            }
          }
          else
          {
            switch (md->protection())
            {
              case Protected: proMembers.append(md); break;
              case Public:    pubMembers.append(md); break;
              case Private:   priMembers.append(md); break;
            }
          }
          break; 
      }
    }
  }
  // check if we should add this member in the `all members' list
  if (md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag)
  {
    MemberInfo *mi = new MemberInfo((MemberDef *)md,Public,Normal);
    MemberNameInfo *mni=0;
    if ((mni=(*allMemberNameInfoDict)[md->name()]))
    {
      mni->append(mi);
    }
    else
    {
      mni = new MemberNameInfo(md->name());
      mni->append(mi);
      allMemberNameInfoList->inSort(mni);
      allMemberNameInfoDict->insert(mni->memberName(),mni);
    }
  }
}


// compute the anchors for all members
void ClassDef::computeAnchors()
{
  setAnchors('a',&pubMembers);
  setAnchors('b',&proMembers);
  setAnchors('c',&priMembers);
  setAnchors('d',&pubStaticMembers);
  setAnchors('e',&proStaticMembers);
  setAnchors('f',&priStaticMembers);
  setAnchors('g',&pubSlots);
  setAnchors('h',&proSlots);
  setAnchors('i',&priSlots);
  setAnchors('j',&signals);
  setAnchors('k',&related);
  setAnchors('l',&friends);
}

// add a file name to the used files set
void ClassDef::insertUsedFile(const char *f)
{
  if (files.find(f)==-1) files.append(f);
}

static void writeInheritanceSpecifier(OutputList &ol,BaseClassDef *bcd)
{
  if (bcd->prot!=Public || bcd->virt!=Normal)
  {
    ol.startTypewriter();
    ol.docify(" [");
    QStrList sl;
    if (bcd->prot==Protected)    sl.append("protected");
    else if (bcd->prot==Private) sl.append("private");
    if (bcd->virt==Virtual)      sl.append("virtual");
    const char *s=sl.first();
    while (s)
    {
      ol.docify(s);
      s=sl.next();
      if (s) ol.docify(", ");
    }
    ol.docify("]");
    ol.endTypewriter();
  }
}

// write all documentation for this class
void ClassDef::writeDocumentation(OutputList &ol)
{
  // write title
  QCString pageTitle=name().copy();
  QCString pageType;
  switch(compType)
  {
    case Class:  pageType=" Class";     break;
    case Struct: pageType=" Struct";    break;
    case Union:  pageType=" Union";     break;
    default:     pageType=" Interface"; break;
  } 
  pageTitle+=pageType+" Reference";
  startFile(ol,fileName,pageTitle);
  startTitle(ol);
  //ol.docify(name()+" "+pageType.right(pageType.length()-1)+" ");
  //parseText(ol,theTranslator->trReference());
  parseText(ol,theTranslator->trCompoundReference(name(),compType));
  endTitle(ol,name());

  // write brief description
  OutputList briefOutput(&ol); 
  if (!briefDescription().isEmpty())
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
  
  if (incFile)
  {
    QCString nm=incName.copy();
    if (incName.isNull()) nm=incFile->name();
    ol.startTypewriter();
    ol.docify("#include <");
    ol.disable(OutputGenerator::Man);
    if (Config::verbatimHeaderFlag)
    {
      ol.writeObjectLink(0,fileName+"-include",0,nm);
    }
    else
    {
      ol.docify(nm);
    }
    ol.enable(OutputGenerator::Man);
    ol.disableAllBut(OutputGenerator::Man);
    ol.docify(nm);
    ol.enableAll();
    ol.docify(">");
    ol.endTypewriter();
    ol.newParagraph();
  }

  if (Config::genTagFile.length()>0) tagFile << ">" << name() << ":";
  
  if (Config::classDiagramFlag) ol.disableAllBut(OutputGenerator::Man);

  // write superclasses
  int count;
  if ((count=inherits->count())>0)
  {
    //parseText(ol,theTranslator->trInherits()+" ");

    QCString inheritLine = theTranslator->trInheritsList(inherits->count());
    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(inheritLine,index,&matchLen))!=-1)
    {
      parseText(ol,inheritLine.mid(index,newIndex-index));
      bool ok;
      uint entryIndex = inheritLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      BaseClassDef *bcd=inherits->at(entryIndex);
      if (ok && bcd)
      {
        ClassDef *cd=bcd->classDef;
        if (cd->isLinkable())
        {
          if (Config::genTagFile.length()>0) tagFile << cd->getOutputFileBase() << "?";
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name()+bcd->templSpecifiers);
        }
        else
        {
          ol.docify(cd->name());
        }
      }
      else
      {
        err("Error: invalid marker %d in inherits list!\n",entryIndex);
      }
      index=newIndex+matchLen;
    } 
    parseText(ol,inheritLine.right(inheritLine.length()-index));
    ol.newParagraph();
  }

  if (Config::genTagFile.length()>0) tagFile << " \"" << fileName << ".html\"\n";

  // write subclasses
  if ((count=inheritedBy->count())>0)
  {
    QCString inheritLine = theTranslator->trInheritedByList(inheritedBy->count());
    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(inheritLine,index,&matchLen))!=-1)
    {
      parseText(ol,inheritLine.mid(index,newIndex-index));
      bool ok;
      uint entryIndex = inheritLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      BaseClassDef *bcd=inheritedBy->at(entryIndex);
      if (ok && bcd)
      {
        ClassDef *cd=bcd->classDef;
        if (cd->isLinkable())
        {
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name());
        }
        else
        {
          ol.docify(cd->name());
        }
        writeInheritanceSpecifier(ol,bcd);
      }
      index=newIndex+matchLen;
    } 
    parseText(ol,inheritLine.right(inheritLine.length()-index));
    ol.newParagraph();
  }

  if (Config::classDiagramFlag) ol.enableAll();
  
  count=0;
  BaseClassDef *ibcd;
  ibcd=inheritedBy->first();
  while (ibcd)
  {
    ClassDef *icd=ibcd->classDef;
    if ( icd->isVisibleInHierarchy()) count++;
    ibcd=inheritedBy->next();
  }
  ibcd=inherits->first();
  while (ibcd)
  {
    ClassDef *icd=ibcd->classDef;
    if ( icd->isVisibleInHierarchy()) count++;
    ibcd=inherits->next();
  }
  if (Config::classDiagramFlag && count>0) 
    // write class diagram
  {
    ClassDiagram diagram(this); // create a diagram of this class.
    ol.startClassDiagram();
    ol.disable(OutputGenerator::Man);
    parseText(ol,theTranslator->trClassDiagram(name()));
    ol.enable(OutputGenerator::Man);
    ol.endClassDiagram(diagram,fileName,name());
  } 

  // write link to list of all members (HTML only)
  if (allMemberNameInfoList->count()>0 /*&& compType==Class*/)
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(memListFileName,0);
    parseText(ol,theTranslator->trListOfAllMembers());
    ol.endTextLink();
    ol.enableAll();
  }
  
  // write member groups
  ol.startMemberSections();
  pubMembers.writeDeclarations(ol,this,0,0,theTranslator->trPublicMembers(),0); 
  pubSlots.writeDeclarations(ol,this,0,0,theTranslator->trPublicSlots(),0); 
  signals.writeDeclarations(ol,this,0,0,theTranslator->trSignals(),0); 
  pubStaticMembers.writeDeclarations(ol,this,0,0,theTranslator->trStaticPublicMembers(),0); 
  proMembers.writeDeclarations(ol,this,0,0,theTranslator->trProtectedMembers(),0); 
  proSlots.writeDeclarations(ol,this,0,0,theTranslator->trProtectedSlots(),0); 
  proStaticMembers.writeDeclarations(ol,this,0,0,theTranslator->trStaticProtectedMembers(),0); 
  if (Config::extractPrivateFlag)
  {
    priMembers.writeDeclarations(ol,this,0,0,theTranslator->trPrivateMembers(),0); 
    priSlots.writeDeclarations(ol,this,0,0,theTranslator->trPrivateSlots(),0); 
    priStaticMembers.writeDeclarations(ol,this,0,0,theTranslator->trStaticPrivateMembers(),0); 
  }
  friends.writeDeclarations(ol,this,0,0,theTranslator->trFriends(),0);
  related.writeDeclarations(ol,this,0,0,
                  theTranslator->trRelatedFunctions(),
                  theTranslator->trRelatedSubscript()
                 ); 
  ol.endMemberSections();
    
  // write detailed description
  bool exampleFlag=hasExamples();
  if (!briefDescription().isEmpty() || !documentation().isEmpty() || exampleFlag)
  {
    ol.writeRuler();
    bool latexOn = ol.isEnabled(OutputGenerator::Latex);
    if (latexOn) ol.disable(OutputGenerator::Latex);
    ol.writeAnchor("_details");
    if (latexOn) ol.enable(OutputGenerator::Latex);
    ol.startGroupHeader();
    parseText(ol,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    
    ArgumentList *al=0;
    int ti;
    ClassDef *pcd=0;
    int pi=0;
    // find the outer most class scope
    while ((ti=name().find("::",pi))!=-1 && 
           (pcd=getClass(name().left(ti)))==0
          ) pi=ti+2;
    
    if (pcd)
    {
      al=pcd->templateArguments();
    }
    else
    {
      al=tempArgs;
    }
    
    if (al) // class is a template
    {
      ol.startSubsubsection(); 
      ol.docify("template<");
      Argument *a=al->first();
      while (a)
      {
        ol.docify(a->type);
        ol.docify(a->name);
        if (a->defval.length()!=0)
        {
          ol.docify(" = ");
          ol.docify(a->defval);
        } 
        a=al->next();
        if (a) ol.docify(", ");
      }
      ol.docify("> "+pageType.lower()+" "+name());
      ol.endSubsubsection();
      ol.writeString("\n");
    }
    
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
    // write examples
    if (exampleFlag)
    {
      ol.startDescList();
      ol.startBold();
      parseText(ol,theTranslator->trExamples()+": ");
      ol.endBold();
      ol.endDescTitle();
      ol.writeDescItem();
      writeExample(ol,exampleList);
      //ol.endDescItem();
      ol.endDescList();
    }
  }
  
  pubMembers.countDocMembers();
  proMembers.countDocMembers();
  priMembers.countDocMembers();
  pubStaticMembers.countDocMembers();
  proStaticMembers.countDocMembers();
  priStaticMembers.countDocMembers();
  pubSlots.countDocMembers();
  proSlots.countDocMembers();
  priSlots.countDocMembers();
  related.countDocMembers();
  friends.countDocMembers();
  signals.countDocMembers();
    
  if ( pubMembers.typedefCount() + proMembers.typedefCount() + 
       (Config::extractPrivateFlag ? priMembers.typedefCount() : 0 )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberTypedefDocumentation());
    ol.endGroupHeader();

    pubMembers.writeDocumentation(ol,name(),MemberDef::Typedef);
    proMembers.writeDocumentation(ol,name(),MemberDef::Typedef); 
    if (Config::extractPrivateFlag)
    {
      priMembers.writeDocumentation(ol,name(),MemberDef::Typedef); 
    }
  }

  if (pubMembers.enumCount() +
      proMembers.enumCount() +
      ( Config::extractPrivateFlag ?  priMembers.enumCount() : 0 )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberEnumerationDocumentation());
    ol.endGroupHeader();

    pubMembers.writeDocumentation(ol,name(),MemberDef::Enumeration);
    proMembers.writeDocumentation(ol,name(),MemberDef::Enumeration); 
    if (Config::extractPrivateFlag)
    {
      priMembers.writeDocumentation(ol,name(),MemberDef::Enumeration); 
    }
  }
 
  if (pubMembers.enumValueCount() +
      proMembers.enumValueCount() +
      ( Config::extractPrivateFlag ?  priMembers.enumValueCount() : 0 )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
  
    pubMembers.writeDocumentation(ol,name(),MemberDef::EnumValue);
    proMembers.writeDocumentation(ol,name(),MemberDef::EnumValue); 
    if (Config::extractPrivateFlag)
    {
      priMembers.writeDocumentation(ol,name(),MemberDef::EnumValue); 
    }
  }

  int func_count=0;
  if ( 
      (
       func_count =
       pubMembers.funcCount() + pubSlots.funcCount() + 
       pubStaticMembers.funcCount() +
       proMembers.funcCount() + proSlots.funcCount() +
       proStaticMembers.funcCount() +
       (Config::extractPrivateFlag ?
          priMembers.funcCount() + priSlots.funcCount() +
          priStaticMembers.funcCount() : 0
       )
      )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    QCString countStr;
    //countStr.sprintf(" (%d)",func_count);
    parseText(ol,theTranslator->trMemberFunctionDocumentation()+countStr);
    ol.endGroupHeader();
    
    pubMembers.writeDocumentation(ol,name(),MemberDef::Function);
    pubSlots.writeDocumentation(ol,name(),MemberDef::Slot); 
    signals.writeDocumentation(ol,name(),MemberDef::Signal); 
    pubStaticMembers.writeDocumentation(ol,name(),MemberDef::Function); 
    proMembers.writeDocumentation(ol,name(),MemberDef::Function); 
    proSlots.writeDocumentation(ol,name(),MemberDef::Slot); 
    proStaticMembers.writeDocumentation(ol,name(),MemberDef::Function); 
    if (Config::extractPrivateFlag)
    {
      priMembers.writeDocumentation(ol,name(),MemberDef::Function); 
      priSlots.writeDocumentation(ol,name(),MemberDef::Slot); 
      priStaticMembers.writeDocumentation(ol,name(),MemberDef::Function); 
    }
  }
  
  if ( friends.friendCount() + related.count() )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trRelatedFunctionDocumentation());
    ol.endGroupHeader();
    friends.writeDocumentation(ol,name(),MemberDef::Friend); 
    related.writeDocumentation(ol,name(),MemberDef::Function); 
  }
  
  
  if ( pubMembers.varCount() + pubStaticMembers.varCount() +
       proMembers.varCount() + proStaticMembers.varCount() +
       (Config::extractPrivateFlag ? 
          priMembers.varCount() + priStaticMembers.varCount() : 0
       )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberDataDocumentation());
    ol.endGroupHeader();

    pubMembers.writeDocumentation(ol,name(),MemberDef::Variable);
    pubStaticMembers.writeDocumentation(ol,name(),MemberDef::Variable); 
    proMembers.writeDocumentation(ol,name(),MemberDef::Variable); 
    proStaticMembers.writeDocumentation(ol,name(),MemberDef::Variable); 
    if (Config::extractPrivateFlag)
    {
      priMembers.writeDocumentation(ol,name(),MemberDef::Variable); 
      priStaticMembers.writeDocumentation(ol,name(),MemberDef::Variable); 
    }
  }
  
  // write the list of used files (Html and LaTeX only)
  ol.disable(OutputGenerator::Man);
  ol.writeRuler();
  parseText(ol,theTranslator->trGeneratedFromFiles(compType,files.count()==1));
  
  bool first=TRUE;
  const char *file = files.first();
  while (file)
  {
    bool ambig;
    FileDef *fd=findFileDef(&inputNameDict,file,ambig);
    if (fd)
    {
      if (first)
      {
        first=FALSE;   
        ol.startItemList();
      }

      ol.writeListItem();
      QCString path=fd->getPath().copy();
      if (Config::fullPathNameFlag)
      {
        ol.docify(stripFromPath(path));
      }

      if (fd->isLinkable())
      {
        ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,
            fd->name());
      }
      else
      {
        ol.docify(fd->name());
      }
    }
    file=files.next();
  }
  if (!first) ol.endItemList();

  // write Author section (Man only)
  ol.enable(OutputGenerator::Man);
  ol.disableAllBut(OutputGenerator::Man);
  ol.writeString("\n");
  ol.startGroupHeader();
  parseText(ol,theTranslator->trAuthor());
  ol.endGroupHeader();
  parseText(ol,theTranslator->trGeneratedAutomatically(Config::projectName));
  ol.enableAll();
 
  endFile(ol);
}

// write the list of all (inherited) members for this class
void ClassDef::writeMemberList(OutputList &ol)
{
  if (allMemberNameInfoList->count()==0) return;
  // do not generate Latex output
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,memListFileName,theTranslator->trMemberList());
  startTitle(ol);
  parseText(ol,name()+" "+theTranslator->trMemberList());
  endTitle(ol,0);
  parseText(ol,theTranslator->trThisIsTheListOfAllMembers());
  ol.writeObjectLink(getReference(),fileName,0,name());
  parseText(ol,theTranslator->trIncludingInheritedMembers());
  
  ol.startItemList();
  
  MemberNameInfo *mni=allMemberNameInfoList->first();
  while (mni)
  {
    MemberInfo *mi=mni->first();
    while (mi)
    {
      MemberDef *md=mi->memberDef;
      ClassDef  *cd=md->memberClass();
      
      // compute the protection level for this member
      Protection protect=md->protection();
      if (mi->prot==Protected) // inherited protection
      {
        if (protect==Public) protect=Protected;
        else if (protect==Protected) protect=Private;
      }
      
      //printf("Member %s of class %s mi->prot=%d prot=%d\n",
      //    md->name().data(),cd->name().data(),mi->prot,protect);

      Specifier virt=md->virtualness();
      MemberDef *rmd=md->reimplements();
      while (rmd && virt==Normal)
      {
        virt = rmd->virtualness()==Normal ? Normal : Virtual;
        rmd  = rmd->reimplements();
      }

      if (cd && !md->name().isEmpty() && md->name()[0]!='@' && 
          (
           md->isFriend() || 
           (mi->prot!=Private && (protect!=Private || Config::extractPrivateFlag))
          )
         )
      {
        bool memberWritten=FALSE;
        if (cd->isLinkable() && md->isLinkable()) 
          // create a link to the documentation
        {
          QCString name=mi->ambiguityResolutionScope+md->name();
          ol.writeListItem();
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),
              md->anchor(),name);
          if ( md->isFunction() || md->isSignal() || md->isSlot() ) 
            ol.docify(md->argsString());
          else if (md->isEnumerate())
            parseText(ol," "+theTranslator->trEnumName());
          else if (md->isEnumValue())
            parseText(ol," "+theTranslator->trEnumValue());
          else if (md->isTypedef())
            ol.docify(" typedef");
          else if (md->isFriend() && !strcmp(md->typeString(),"friend class"))
            ol.docify(" class");
          ol.writeString("\n");
          memberWritten=TRUE;
        }
        else if (!Config::hideMemberFlag) // no documentation, 
                                  // generate link to the class instead.
        {
          ol.writeListItem();
          ol.writeBoldString(md->name());
          if ( md->isFunction() || md->isSignal() || md->isSlot() ) 
            ol.docify(md->argsString());
          else if (md->isEnumerate())
            parseText(ol," "+theTranslator->trEnumName());
          else if (md->isEnumValue())
            parseText(ol," "+theTranslator->trEnumValue());
          else if (md->isTypedef())
            ol.docify(" typedef");
          ol.writeString(" (");
          parseText(ol,theTranslator->trDefinedIn()+" ");
          if (cd->isLinkable())
          {
            ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name());
          }
          else
          {
            ol.writeBoldString(cd->name());
          }
          ol.writeString(")");
          memberWritten=TRUE;
        }
        if ((protect!=Public || md->isStatic() || virt!=Normal || 
             md->isFriend() || md->isRelated()
            )
            && memberWritten)
        {
          ol.startTypewriter();
          ol.docify(" [");
          QStrList sl;
          if (md->isFriend()) sl.append("friend");
          else if (md->isRelated()) sl.append("related");
          else
          {
            if (protect==Protected)    sl.append("protected");
            else if (protect==Private) sl.append("private");
            if (virt==Virtual)         sl.append("virtual");
            else if (virt==Pure)       sl.append("pure virtual");
            if (md->isStatic())        sl.append("static");
            if (md->isSignal())        sl.append("signal");
            if (md->isSlot())          sl.append("slot");
          }
          const char *s=sl.first();
          while (s)
          {
            ol.docify(s);
            s=sl.next();
            if (s) ol.docify(", ");
          }
          ol.docify("]");
          ol.endTypewriter();
        }
      }
      mi=mni->next();
    }
    mni=allMemberNameInfoList->next();
  }
  ol.endItemList();
  endFile(ol);
  ol.enableAll();
}

void ClassDef::writeIncludeFile(OutputList &ol)
{
  if (!incFile) return;
  //printf("incFile=%s\n",incFile->absFilePath().data());
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,fileName+"-include",name()+" Include File");
  startTitle(ol);
  QCString n=incName.copy();
  if (incName.isNull()) n=incFile->name();
  parseText(ol,n);
  endTitle(ol,0);
  parseText(ol,theTranslator->trVerbatimText(incFile->name()));
  ol.writeRuler();
  ol.startCodeFragment();
  parseCode(ol,n,fileToString(incFile->absFilePath()),FALSE,0);
  ol.endCodeFragment();
  endFile(ol);
  ol.enableAll();
}

// add a reference to an example
bool ClassDef::addExample(const char *anchor,const char *nameStr,
    const char *file)
{
  if (exampleDict && !exampleDict->find(nameStr))
  {
    Example *e=new Example;
    e->anchor=anchor;
    e->name=nameStr;
    e->file=file;
    exampleDict->insert(nameStr,e);
    exampleList->inSort(e);
    return TRUE;
  }
  return FALSE;
}

// returns TRUE if this class is used in an example
bool ClassDef::hasExamples()
{
  if (exampleList==0) 
    return FALSE;
  else
    return exampleList->count()>0;
}

#if 0
// write the list of all examples that are use this class.
void ClassDef::writeExample(OutputList &ol)
{
  QCString exampleLine=theTranslator->trWriteList(exampleList->count());
 
  QRegExp marker("@[0-9]+");
  int index=0,newIndex,matchLen;
  // now replace all markers in inheritLine with links to the classes
  while ((newIndex=marker.match(exampleLine,index,&matchLen))!=-1)
  {
    bool ok;
    parseText(ol,exampleLine.mid(index,newIndex-index));
    uint entryIndex = exampleLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
    Example *e=exampleList->at(entryIndex);
    if (ok && e)
    {
      ol.writeObjectLink(0,e->file,e->anchor,e->name);
    }
    index=newIndex+matchLen;
  } 
  parseText(ol,exampleLine.right(exampleLine.length()-index));
  ol.writeString(".");
  
#if 0
  Example *e=exampleList->first();
  while (e)
  {
    ol.writeObjectLink(0,e->file,e->anchor,e->name);
    e=exampleList->next();
    if (e)
    {
      if (exampleList->at()==(int)exampleList->count()-1)
        parseText(ol," "+theTranslator->trAnd()+" ");
      else
        ol.writeString(", ");
    }
  }
  ol.writeString(".");
#endif
}
#endif

void ClassDef::setTemplateArguments(ArgumentList *al)
{
  if (al==0) return;
  if (!tempArgs) delete tempArgs; // delete old list if needed
  tempArgs=new ArgumentList; 
  tempArgs->setAutoDelete(TRUE);
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    tempArgs->append(new Argument(*a));
  }
}

//QCString ClassDef::getTemplateNameString()
//{
//  QCString result;
//  if (!tempArgs || tempArgs->count()==0) return result;
//  result="<";
//  Argument *a=tempArgs->first();
//  while (a)
//  {
//    if (a->name.length()>0) // add template argument name
//    {
//      result+=a->name;
//    }
//    else // extract name from type
//    {
//      int i=a->type.length()-1;
//      while (i>=0 && isId(a->type.at(i))) i--;
//      if (i>0)
//      {
//        result+=a->type.right(a->type.length()-i-1);
//      }
//    }
//    a=tempArgs->next();
//    if (a) result+=", ";
//  }
//  result+=">";
//  return result;
//}

bool ClassDef::hasNonReferenceSuperClass()
{
  bool found=!isReference();
  BaseClassListIterator bcli(*inheritedBy);
  for ( ; bcli.current() && !found ; ++bcli )
    found=found || bcli.current()->classDef->hasNonReferenceSuperClass();
  return found;
}

//void ClassDef::writeMembersToContents()
//{
//  HtmlHelp *htmlHelp = HtmlHelp::getInstance();
//
//  htmlHelp->incContentsDepth();
//  
//  MemberNameInfoListIterator mnili(*allMemberNameInfoList);
//  MemberNameInfo *mni;
//  for (;(mni=mnili.current());++mnili)
//  {
//    MemberNameInfoIterator mnii(*mni);
//    MemberInfo *mi;
//    for (mnii.toLast();(mi=mnii.current());--mnii)
//    {
//      MemberDef *md=mi->memberDef;
//      ClassDef  *cd=md->memberClass();
//      if (md->isLinkable() && cd==this) // member is not inherited
//      {
//        htmlHelp->addContentsItem(md->name()+md->argsString(),
//                                  cd->getOutputFileBase(),
//                                  md->anchor());
//      }
//    }
//  }
//  htmlHelp->decContentsDepth();
//}

void ClassDef::writeDeclaration(OutputList &ol)
{
  //ol.insertMemberAlign();
  switch(compType)
  {
    case Class:  ol.docify("class {");  break;
    case Struct: ol.docify("struct {"); break;
    default:     ol.docify("union {");  break; 
  } 
  ol.endMemberItem(FALSE,0,0,FALSE); // TODO: pass correct group parameters

  // insert members of this class
  pubMembers.writePlainDeclarations(ol,this,0,0); 
  pubSlots.writePlainDeclarations(ol,this,0,0); 
  signals.writePlainDeclarations(ol,this,0,0); 
  pubStaticMembers.writePlainDeclarations(ol,this,0,0); 
  proMembers.writePlainDeclarations(ol,this,0,0); 
  proSlots.writePlainDeclarations(ol,this,0,0); 
  proStaticMembers.writePlainDeclarations(ol,this,0,0); 
  if (Config::extractPrivateFlag)
  {
    priMembers.writePlainDeclarations(ol,this,0,0); 
    priSlots.writePlainDeclarations(ol,this,0,0); 
    priStaticMembers.writePlainDeclarations(ol,this,0,0); 
  }
  friends.writePlainDeclarations(ol,this,0,0);
  related.writePlainDeclarations(ol,this,0,0); 
}
