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

static QString stripExtension(const char *fName)
{
  QString result=fName;
  if (result.right(5)==".html") result=result.left(result.length()-5);
  return result;
}

// constructs a new class definition
ClassDef::ClassDef(const char *nm,int ct,const char *ref,const char *fName) 
 : Definition(removeRedundantWhiteSpace(nm)) 
{
  //name=n;
  if (fName)
    fileName=stripExtension(fName);
  else
    fileName="class_"+nameToFile(nm);
  if (ref) 
  {
    //url=(QString)"doxygen=\""+ref+":\" href=\""+fileName;
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
  reference=ref;
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
  if (!reference)
  {
    if (md->isRelated() && (extractPrivateFlag || md->protection()!=Private))
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
  if (md->isFriend() || md->protection()!=Private || extractPrivateFlag)
  {
    MemberInfo *mi = new MemberInfo((MemberDef *)md,Public,Normal);
    MemberNameInfo *mni;
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
  QString pageTitle=name().copy();
  QString pageType;
  switch(compType)
  {
    case Class:  pageType=" Class";  break;
    case Struct: pageType=" Struct"; break;
    case Union:  pageType=" Union";  break;
    default:     pageType+=compType; break; // an error
  } 
  pageTitle+=pageType+" Reference";
  startFile(ol,fileName,pageTitle);
  startTitle(ol);
  ol.docify(name()+" "+pageType.right(pageType.length()-1)+" ");
  parseDoc(ol,0,0,theTranslator->trReference());
  endTitle(ol,name());

  // write brief description
  OutputList briefOutput(&ol); 
  if (!briefDescription().isEmpty())
  {
    parseDoc(briefOutput,name(),0,briefDescription());
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
  
  if (incFile)
  {
    QString nm=incName.copy();
    if (incName.isNull()) nm=incFile->name();
    ol.startTypewriter();
    ol.docify("#include <");
    ol.disable(OutputGenerator::Man);
    if (verbatimHeaderFlag)
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

  if (genTagFile.length()>0) tagFile << ">" << name() << ":";
  
  if (classDiagramFlag) ol.disableAllBut(OutputGenerator::Man);

  // write superclasses
  int count;
  if ((count=inherits->count())>0)
  {
    parseDoc(ol,0,0,theTranslator->trInherits()+" ");
    BaseClassDef *bcd=inherits->first();
    while (bcd)
    {
      ClassDef *cd=bcd->classDef;
      if (cd->hasDocumentation() || cd->isReference())
      {
        if (genTagFile.length()>0) tagFile << cd->classFile() << "?";
        ol.writeObjectLink(cd->getReference(),cd->classFile(),0,cd->name()+bcd->templSpecifiers);
      }
      else
      {
        ol.docify(cd->name());
      }
      writeInheritanceSpecifier(ol,bcd);
      bcd=inherits->next();
      if (bcd)
      {
        if (inherits->at()==count-1) 
          parseDoc(ol,0,0," "+theTranslator->trAnd()+" "); 
        else 
          ol.writeString(", ");
      }
    }
    ol.writeString(".");
    ol.newParagraph();
  }

  if (genTagFile.length()>0) tagFile << " \"" << fileName << ".html\"\n";

  // write subclasses
  if ((count=inheritedBy->count())>0)
  {
    parseDoc(ol,0,0,theTranslator->trInheritedBy()+" ");
    BaseClassDef *bcd=inheritedBy->first();
    while (bcd)
    {
      ClassDef *cd=bcd->classDef;
      if (cd->hasDocumentation() || cd->isReference())
      {
        ol.writeObjectLink(cd->getReference(),cd->classFile(),0,cd->name());
      }
      else
      {
        ol.docify(cd->name());
      }
      writeInheritanceSpecifier(ol,bcd);
      bcd=inheritedBy->next();
      if (bcd) 
      {
        if (inheritedBy->at()==count-1) 
          parseDoc(ol,0,0," "+theTranslator->trAnd()+" "); 
        else 
          ol.writeString(", ");
      }
    }
    ol.writeString(".");
    ol.newParagraph();
  }

  if (classDiagramFlag) ol.enableAll();
  
  count=0;
  BaseClassDef *ibcd;
  ibcd=inheritedBy->first();
  while (ibcd)
  {
    ClassDef *icd=ibcd->classDef;
    if (//(icd->protection()!=Private || extractPrivateFlag) &&
        //(icd->hasDocumentation() || !hideClassFlag || icd->isReference()) 
        icd->isVisibleExt()
       ) count++;
    ibcd=inheritedBy->next();
  }
  ibcd=inherits->first();
  while (ibcd)
  {
    ClassDef *icd=ibcd->classDef;
    if (//(icd->protection()!=Private || extractPrivateFlag) &&
        //(icd->hasDocumentation() || !hideClassFlag | icd->isReference()) 
        icd->isVisibleExt()
       ) count++;
    ibcd=inherits->next();
  }
  if (classDiagramFlag && count>0) 
    // write class diagram
  {
    ClassDiagram diagram(this); // create a diagram of this class.
    ol.startClassDiagram();
    ol.disable(OutputGenerator::Man);
    parseDoc(ol,name(),0,theTranslator->trClassDiagram(name()));
    ol.enable(OutputGenerator::Man);
    ol.endClassDiagram(diagram,fileName,name());
  } 

  // write link to list of all members (HTML only)
  if (allMemberNameInfoList->count()>0 && compType==Class)
  {
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(memListFileName,0);
    parseDoc(ol,0,0,theTranslator->trListOfAllMembers());
    ol.endTextLink();
    ol.enableAll();
  }
  
  // write member groups
  writeMemberDecs(ol,this,0,0,"Public Members",0,&pubMembers); 
  writeMemberDecs(ol,this,0,0,"Public Slots",0,&pubSlots); 
  writeMemberDecs(ol,this,0,0,"Signals",0,&signals); 
  writeMemberDecs(ol,this,0,0,"Static Public Members",0,&pubStaticMembers); 
  writeMemberDecs(ol,this,0,0,"Protected Members",0,&proMembers); 
  writeMemberDecs(ol,this,0,0,"Protected Slots",0,&proSlots); 
  writeMemberDecs(ol,this,0,0,"Static Protected Members",0,&proStaticMembers); 
  if (extractPrivateFlag)
  {
    writeMemberDecs(ol,this,0,0,"Private Members",0,&priMembers); 
    writeMemberDecs(ol,this,0,0,"Private Slots",0,&priSlots); 
    writeMemberDecs(ol,this,0,0,"Static Private Members",0,&priStaticMembers); 
  }
  writeMemberDecs(ol,this,0,0,"Friends",0,&friends);
  writeMemberDecs(ol,this,0,0,
                  theTranslator->trRelatedFunctions(),
                  theTranslator->trRelatedSubscript(),
                  &related
                 ); 
    
  // write detailed description
  bool exampleFlag=hasExamples();
  if (!briefDescription().isEmpty() || !documentation().isEmpty() || exampleFlag)
  {
    ol.writeRuler();
    ol.writeAnchor("details");
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    if (tempArgs) // class is a template
    {
      ol.startSubsubsection(); 
      ArgumentList *al=tempArgs;
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
      parseDoc(ol,0,0,theTranslator->trExamples()+": ");
      ol.endBold();
      ol.endDescTitle();
      ol.writeDescItem();
      writeExample(ol);
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
       (extractPrivateFlag ? priMembers.typedefCount() : 0 )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trMemberTypedefDocumentation());
    ol.endGroupHeader();

    writeMemberDocs(ol,&pubMembers,name(),MemberDef::Typedef);
    writeMemberDocs(ol,&proMembers,name(),MemberDef::Typedef); 
    if (extractPrivateFlag)
    {
      writeMemberDocs(ol,&priMembers,name(),MemberDef::Typedef); 
    }
  }

  if (pubMembers.enumCount() +
      proMembers.enumCount() +
      ( extractPrivateFlag ?  priMembers.enumCount() : 0 )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trMemberEnumerationDocumentation());
    ol.endGroupHeader();

    writeMemberDocs(ol,&pubMembers,name(),MemberDef::Enumeration);
    writeMemberDocs(ol,&proMembers,name(),MemberDef::Enumeration); 
    if (extractPrivateFlag)
    {
      writeMemberDocs(ol,&priMembers,name(),MemberDef::Enumeration); 
    }
  }
 
  if (pubMembers.enumValueCount() +
      proMembers.enumValueCount() +
      ( extractPrivateFlag ?  priMembers.enumValueCount() : 0 )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trEnumerationValueDocumentation());
    ol.endGroupHeader();
  
    writeMemberDocs(ol,&pubMembers,name(),MemberDef::EnumValue);
    writeMemberDocs(ol,&proMembers,name(),MemberDef::EnumValue); 
    if (extractPrivateFlag)
    {
      writeMemberDocs(ol,&priMembers,name(),MemberDef::EnumValue); 
    }
  }

  if ( pubMembers.funcCount() + pubSlots.funcCount() + 
       pubStaticMembers.funcCount() +
       proMembers.funcCount() + proSlots.funcCount() +
       proStaticMembers.funcCount() +
       (extractPrivateFlag ?
          priMembers.funcCount() + priSlots.funcCount() +
          priStaticMembers.funcCount() : 0
       ) 
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trMemberFunctionDocumentation());
    ol.endGroupHeader();
    
    writeMemberDocs(ol,&pubMembers,name(),MemberDef::Function);
    writeMemberDocs(ol,&pubSlots,name(),MemberDef::Slot); 
    writeMemberDocs(ol,&signals,name(),MemberDef::Signal); 
    writeMemberDocs(ol,&pubStaticMembers,name(),MemberDef::Function); 
    writeMemberDocs(ol,&proMembers,name(),MemberDef::Function); 
    writeMemberDocs(ol,&proSlots,name(),MemberDef::Slot); 
    writeMemberDocs(ol,&proStaticMembers,name(),MemberDef::Function); 
    if (extractPrivateFlag)
    {
      writeMemberDocs(ol,&priMembers,name(),MemberDef::Function); 
      writeMemberDocs(ol,&priSlots,name(),MemberDef::Slot); 
      writeMemberDocs(ol,&priStaticMembers,name(),MemberDef::Function); 
    }
  }
  
  if ( friends.count() + related.count() )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trRelatedFunctionDocumentation());
    ol.endGroupHeader();
    writeMemberDocs(ol,&friends,name(),MemberDef::Friend); 
    writeMemberDocs(ol,&related,name(),MemberDef::Function); 
  }
  
  
  if ( pubMembers.varCount() + pubStaticMembers.varCount() +
       proMembers.varCount() + proStaticMembers.varCount() +
       (extractPrivateFlag ? 
          priMembers.varCount() + priStaticMembers.varCount() : 0
       )
     )
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseDoc(ol,0,0,theTranslator->trMemberDataDocumentation());
    ol.endGroupHeader();

    writeMemberDocs(ol,&pubMembers,name(),MemberDef::Variable);
    writeMemberDocs(ol,&pubStaticMembers,name(),MemberDef::Variable); 
    writeMemberDocs(ol,&proMembers,name(),MemberDef::Variable); 
    writeMemberDocs(ol,&proStaticMembers,name(),MemberDef::Variable); 
    if (extractPrivateFlag)
    {
      writeMemberDocs(ol,&priMembers,name(),MemberDef::Variable); 
      writeMemberDocs(ol,&priStaticMembers,name(),MemberDef::Variable); 
    }
  }
  
  // write the list of used files (Html and LaTeX only)
  ol.disable(OutputGenerator::Man);
  ol.writeRuler();
  parseDoc(ol,0,0,
      theTranslator->trGeneratedFrom(pageType.lower(),
      files.count()==1));
  ol.startItemList();

  const char *file = files.first();
  while (file)
  {
    QFileInfo fi(file);
    ol.writeListItem();
    FileDef *fd;
    bool ambig;
    if ((fd=findFileDef(&inputNameDict,fi.absFilePath(),ambig))
        && fd->hasDocumentation())
    {
      ol.writeObjectLink(fd->getReference(),fd->diskName(),0,
                         fi.fileName());
    }
    else
    {
      ol.docify(fi.fileName());
    }
    file=files.next();
  }

  ol.endItemList();
  ol.enable(OutputGenerator::Man);
  
  // write Author section (Man only)
  ol.disableAllBut(OutputGenerator::Man);
  ol.writeString("\n");
  ol.startGroupHeader();
  parseDoc(ol,0,0,theTranslator->trAuthor());
  ol.endGroupHeader();
  parseDoc(ol,0,0,theTranslator->trGeneratedAutomatically(projectName));
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
  parseDoc(ol,name(),0,name()+" "+theTranslator->trMemberList());
  endTitle(ol,0);
  parseDoc(ol,0,0,theTranslator->trThisIsTheListOfAllMembers());
  ol.writeString(" ");
  ol.writeObjectLink(reference,fileName,0,name());
  ol.writeString(", ");
  parseDoc(ol,0,0,theTranslator->trIncludingInheritedMembers());
  
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
           (mi->prot!=Private && (protect!=Private || extractPrivateFlag))
          )
         )
      {
        bool memberWritten=FALSE;
        if (cd->isVisible() && (md->hasDocumentation() || md->isReference())) 
          // create a link to the documentation
        {
          QString name=mi->ambiguityResolutionScope+md->name();
          ol.writeListItem();
          ol.writeObjectLink(cd->getReference(),cd->classFile(),
              md->anchor(),name);
          if ( md->isFunction() || md->isSignal() || md->isSlot() ) 
            ol.docify(md->argsString());
          else if (md->isEnumerate())
            parseDoc(ol,0,0," "+theTranslator->trEnumName());
          else if (md->isEnumValue())
            parseDoc(ol,0,0," "+theTranslator->trEnumValue());
          else if (md->isTypedef())
            ol.docify(" typedef");
          else if (md->isFriend() && !strcmp(md->typeString(),"friend class"))
            ol.docify(" class");
          ol.writeString("\n");
          memberWritten=TRUE;
        }
        else if (!hideMemberFlag) // no documentation, 
                                  // generate link to the class instead.
        {
          ol.writeListItem();
          ol.writeBoldString(md->name());
          if ( md->isFunction() || md->isSignal() || md->isSlot() ) 
            ol.docify(md->argsString());
          else if (md->isEnumerate())
            parseDoc(ol,0,0," "+theTranslator->trEnumName());
          else if (md->isEnumValue())
            parseDoc(ol,0,0," "+theTranslator->trEnumValue());
          else if (md->isTypedef())
            ol.docify(" typedef");
          ol.writeString(" (");
          parseDoc(ol,0,0,theTranslator->trDefinedIn()+" ");
          if (cd->isVisible())
          {
            ol.writeObjectLink(cd->getReference(),cd->classFile(),0,cd->name());
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
  QString n=incName.copy();
  if (incName.isNull()) n=incFile->name();
  parseDoc(ol,0,0,n);
  endTitle(ol,0);
  parseDoc(ol,0,0,theTranslator->trVerbatimText(incFile->name()));
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

// write the list of all examples that are use this class.
void ClassDef::writeExample(OutputList &ol)
{
  Example *e=exampleList->first();
  while (e)
  {
    ol.writeObjectLink(0,e->file,e->anchor,e->name);
    e=exampleList->next();
    if (e)
    {
      if (exampleList->at()==(int)exampleList->count()-1)
        parseDoc(ol,0,0," "+theTranslator->trAnd()+" ");
      else
        ol.writeString(", ");
    }
  }
  ol.writeString(".");
}

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

QString ClassDef::getTemplateNameString()
{
  QString result;
  if (!tempArgs || tempArgs->count()==0) return result;
  result="<";
  Argument *a=tempArgs->first();
  while (a)
  {
    if (a->name.length()>0) // add template argument name
    {
      result+=a->name;
    }
    else // extract name from type
    {
      int i=a->type.length()-1;
      while (i>=0 && isId(a->type.at(i))) i--;
      if (i>0)
      {
        result+=a->type.right(a->type.length()-i-1);
      }
    }
    a=tempArgs->next();
    if (a) result+=", ";
  }
  result+=">";
  return result;
}

bool ClassDef::hasNonReferenceSuperClass()
{
  bool found=!isReference();
  BaseClassListIterator bcli(*inheritedBy);
  for ( ; bcli.current() && !found ; ++bcli )
    found=found || bcli.current()->classDef->hasNonReferenceSuperClass();
  return found;
}
