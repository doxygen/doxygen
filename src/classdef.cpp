/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
#include "example.h"
#include "outputlist.h"
#include "dot.h"

static QCString stripExtension(const char *fName)
{
  QCString result=fName;
  if (result.right(5)==".html") result=result.left(result.length()-5);
  return result;
}

// constructs a new class definition
ClassDef::ClassDef(
    const char *defFileName,int defLine,
    const char *nm,CompoundType ct,
    const char *ref,const char *fName) 
 : Definition(defFileName,defLine,removeRedundantWhiteSpace(nm)) 
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
  incInfo=0;
  tempArgs=0;
  prot=Public;
  nspace=0;
  fileDef=0;
  usesImplClassDict=0;
  usesIntfClassDict=0;
  memberGroupList = new MemberGroupList;
  memberGroupList->setAutoDelete(TRUE);
  memberGroupDict = new MemberGroupDict(257);
  int i=name().findRev("::");
  if (i==-1)
  {
    scopelessName=name();
  }
  else
  {
    scopelessName=name().right(name().length()-i-2);
  }
  subGrouping=TRUE;
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
  delete usesImplClassDict;
  delete usesIntfClassDict;
  delete incInfo;
  delete memberGroupList;
  delete memberGroupDict;
}

QCString ClassDef::displayName() const
{
  if (Config::hideScopeNames)
  {
    return stripScope(name());
  }
  else
  {
    return name();
  }
}

// inserts a base class in the inheritance list
void ClassDef::insertBaseClass(ClassDef *cd,Protection p,
                               Specifier s,const char *t)
{
  //printf("*** insert base class %s into %s\n",cd->name().data(),name().data());
  //inherits->inSort(new BaseClassDef(cd,p,s,t));
  inherits->append(new BaseClassDef(cd,p,s,t));
}

// inserts a super class in the inherited list
void ClassDef::insertSuperClass(ClassDef *cd,Protection p,
                                Specifier s,const char *t)
{
  //printf("*** insert super class %s into %s\n",cd->name().data(),name().data());
  inheritedBy->inSort(new BaseClassDef(cd,p,s,t));
}

void ClassDef::addMemberListToGroup(MemberList *ml)
{
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (;(md=mli.current());++mli)
  {
    int groupId=md->getMemberGroupId();
    if (groupId!=-1)
    {
      QCString *pGrpHeader = memberHeaderDict[groupId];
      QCString *pDocs      = memberDocDict[groupId];
      if (pGrpHeader)
      {
        MemberGroup *mg = memberGroupDict->find(groupId);
        if (mg==0)
        {
          mg = new MemberGroup(groupId,*pGrpHeader,pDocs ? pDocs->data() : 0);
          memberGroupDict->insert(groupId,mg);
          memberGroupList->append(mg);
        }
        mg->insertMember(md);
        md->setMemberGroup(mg);
      }
    }
  }
}

void ClassDef::addMembersToMemberGroup()
{
  addMemberListToGroup(&pubTypes);
  addMemberListToGroup(&pubMembers);
  addMemberListToGroup(&pubAttribs);
  addMemberListToGroup(&pubSlots);
  addMemberListToGroup(&signals);
  addMemberListToGroup(&pubStaticMembers);
  addMemberListToGroup(&pubStaticAttribs);
  addMemberListToGroup(&proTypes);
  addMemberListToGroup(&proMembers);
  addMemberListToGroup(&proAttribs);
  addMemberListToGroup(&proSlots);
  addMemberListToGroup(&proStaticMembers);
  addMemberListToGroup(&proStaticAttribs);
  addMemberListToGroup(&priTypes);
  addMemberListToGroup(&priMembers);
  addMemberListToGroup(&priAttribs);
  addMemberListToGroup(&priSlots);
  addMemberListToGroup(&priStaticMembers);
  addMemberListToGroup(&priStaticAttribs);
  addMemberListToGroup(&friends);
  addMemberListToGroup(&related);
}

// adds new member definition to the class
void ClassDef::insertMember(MemberDef *md)
{
  //printf("adding %s::%s\n",name().data(),md->name().data());
  if (!isReference())
  {
    /********************************************/
    /* insert member in the declaration section */
    /********************************************/
    if (md->isRelated() && (Config::extractPrivateFlag || md->protection()!=Private))
    {
      related.append(md);
      md->setSectionList(&related);
    }
    else if (md->isFriend())
    {
      friends.append(md);
      md->setSectionList(&friends);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Signal:
          signals.append(md);
          md->setSectionList(&signals);
          break;
        case MemberDef::Slot:
          switch (md->protection())
          {
            case Protected: 
              proSlots.append(md); 
              md->setSectionList(&proSlots);
              break;
            case Public:    
              pubSlots.append(md); 
              md->setSectionList(&pubSlots);
              break;
            case Private:   
              priSlots.append(md);
              md->setSectionList(&priSlots);
              break;
          }
          break;
        default: // any of the other members
          if (md->isStatic())
          {
            if (md->isVariable())
            {
              switch (md->protection())
              {
                case Protected: 
                  proStaticAttribs.append(md); 
                  md->setSectionList(&proStaticAttribs);
                  break;
                case Public:    
                  pubStaticAttribs.append(md); 
                  md->setSectionList(&pubStaticAttribs);
                  break;
                case Private:   
                  priStaticAttribs.append(md); 
                  md->setSectionList(&priStaticAttribs);
                  break;
              }
            }
            else // function
            {
              switch (md->protection())
              {
                case Protected: 
                  proStaticMembers.append(md); 
                  md->setSectionList(&proStaticMembers);
                  break;
                case Public:    
                  pubStaticMembers.append(md); 
                  md->setSectionList(&pubStaticMembers);
                  break;
                case Private:   
                  priStaticMembers.append(md); 
                  md->setSectionList(&priStaticMembers);
                  break;
              }
            }
          }
          else // not static
          {
            if (md->isVariable())
            {
              switch (md->protection())
              {
                case Protected: 
                  proAttribs.append(md); 
                  md->setSectionList(&proAttribs);
                  break;
                case Public:    
                  pubAttribs.append(md); 
                  md->setSectionList(&pubAttribs);
                  break;
                case Private:   
                  priAttribs.append(md); 
                  md->setSectionList(&priAttribs);
                  break;
              }
            }
            else if (md->isTypedef() || md->isEnumerate() || md->isEnumValue())
            {
              switch (md->protection())
              {
                case Protected: 
                  proTypes.append(md); 
                  md->setSectionList(&proTypes); 
                  break;
                case Public:    
                  pubTypes.append(md); 
                  md->setSectionList(&pubTypes); 
                  break;
                case Private:   
                  priTypes.append(md); 
                  md->setSectionList(&priTypes); 
                  break;
              }
            }
            else // member function
            {
              switch (md->protection())
              {
                case Protected: 
                  proMembers.append(md); 
                  md->setSectionList(&proMembers); 
                  break;
                case Public:    
                  pubMembers.append(md); 
                  md->setSectionList(&pubMembers); 
                  break;
                case Private:   
                  priMembers.append(md); 
                  md->setSectionList(&priMembers); 
                  break;
              }
            }
          }
          break; 
      }
    }

    /*******************************************************/
    /* insert member in the detailed documentation section */
    /*******************************************************/
    if ((md->isRelated() && 
          (Config::extractPrivateFlag || md->protection()!=Private)
        ) || md->isFriend()
       )
    {
      if (Config::sortMembersFlag)
        relatedMembers.inSort(md);
      else
        relatedMembers.append(md);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Signal:
          if (Config::sortMembersFlag)
            functionMembers.inSort(md);
          else
            functionMembers.append(md);
          break;
        case MemberDef::Slot:
          switch (md->protection())
          {
            case Protected: 
              if (Config::sortMembersFlag)
                functionMembers.inSort(md); 
              else
                functionMembers.append(md);
              break;
            case Public:    
              if (Config::sortMembersFlag)
                functionMembers.inSort(md); 
              else
                functionMembers.append(md);
              break;
            case Private:   
              if (Config::sortMembersFlag)
                functionMembers.inSort(md); 
              else
                functionMembers.append(md);
              break;
          }
          break;
        default: // any of the other members
          if (md->protection()!=Private || Config::extractPrivateFlag)
          {
            switch (md->memberType())
            {
              case MemberDef::Typedef:
                if (Config::sortMembersFlag)
                  typedefMembers.inSort(md);
                else
                  typedefMembers.append(md);
                break;
              case MemberDef::Enumeration:
                if (Config::sortMembersFlag)
                  enumMembers.inSort(md);
                else
                  enumMembers.append(md);
                break;
              case MemberDef::EnumValue:
                if (Config::sortMembersFlag)
                  enumValMembers.inSort(md);
                else
                  enumValMembers.append(md);
                break;
              case MemberDef::Function:
                if (md->name()==scopelessName ||         // constructor
                    (md->name().find('~')!=-1 &&  // hack to detect destructor
                     md->name().find("operator")==-1
                    )
                   )
                {
                  constructors.append(md);
                }
                else
                {
                  if (Config::sortMembersFlag)
                    functionMembers.inSort(md);
                  else
                    functionMembers.append(md);
                }
                break;
              case MemberDef::Variable:
                if (Config::sortMembersFlag)
                  variableMembers.inSort(md);
                else
                  variableMembers.append(md);
                break;
              default:
                err("Unexpected member type %d found!\n",md->memberType());
            }
          }
          break; 
      }
    }

    /*************************************************/
    /* insert member in the appropriate member group */
    /*************************************************/
    // Note: this must be done AFTER inserting the member in the 
    // regular groups
    //addMemberToGroup(md,groupId);
    
  }

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


//void ClassDef::computeMemberGroups()
//{
//  MemberNameInfoListIterator mnili(*allMemberNameInfoList);
//  MemberNameInfo *mni;
//  for (;(mni=mnili.current());++mnili)
//  {
//   MemberNameInfoIterator mnii(*mni);
//    MemberInfo *mi;
//    for (mnii.toFirst();(mi=mnii.current());++mnii)
//    {
//      MemberDef *md=mi->memberDef;
//      MemberGroup *mg = md->getMemberGroup();
//      if (mg && memberGroupDict->find(mg->groupId())==0)
//      {
//        memberGroupDict->insert(mg->groupId(),mg);
//        memberGroupList->append(mg);
//      }
//    }
//  }
//}

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
  setAnchors('m',&pubAttribs);
  setAnchors('n',&proAttribs);
  setAnchors('o',&priAttribs);
  setAnchors('p',&pubStaticAttribs);
  setAnchors('q',&proStaticAttribs);
  setAnchors('r',&priStaticAttribs);
  setAnchors('s',&pubTypes);
  setAnchors('t',&proTypes);
  setAnchors('u',&priTypes);
}

void ClassDef::distributeMemberGroupDocumentation()
{
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
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

void ClassDef::setIncludeFile(FileDef *fd,const char *incName,bool local)
{
  //printf("ClassDef::setInclude(%p,%s,%d)\n",fd,incName,local);
  if (!incInfo) incInfo=new IncludeInfo;
  if ((incName && incInfo->includeName.isEmpty()) ||
      (fd!=0 && incInfo->fileDef==0)
     )
  {
    //printf("Setting file info\n");
    incInfo->fileDef     = fd;
    incInfo->includeName = incName;
    incInfo->local       = local;
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
  startTitle(ol,getOutputFileBase());
  parseText(ol,theTranslator->trCompoundReference(name(),compType));
  endTitle(ol,getOutputFileBase(),name());

  ol.startTextBlock();
  
  // write brief description
  OutputList briefOutput(&ol); 
  if (!briefDescription().isEmpty())
  {
    parseDoc(briefOutput,defFileName,defLine,name(),0,briefDescription());
    ol+=briefOutput;
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(0,"_details");
    parseText(ol,theTranslator->trMore());
    ol.endTextLink();
    ol.popGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.newParagraph();
    ol.enable(OutputGenerator::Man);
  }
  ol.writeSynopsis();
  
  if (incInfo)
  {
    QCString nm=incInfo->includeName.isEmpty() ? 
      (incInfo->fileDef ?
       incInfo->fileDef->docName().data() : "" 
      ) :
      incInfo->includeName.data();
    if (!nm.isEmpty())
    {
      ol.startTypewriter();
      ol.docify("#include ");
      if (incInfo->local)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.docify(nm);
      ol.disableAllBut(OutputGenerator::Html);
      ol.enable(OutputGenerator::Html);
      if (incInfo->fileDef)
      {
        ol.writeObjectLink(0,incInfo->fileDef->includeName(),0,nm);
      }
      else
      {
        ol.docify(nm);
      }
      ol.popGeneratorState();
      if (incInfo->local)
        ol.docify("\"");
      else
        ol.docify(">");
      ol.endTypewriter();
      ol.newParagraph();
    }
  }


  if (!Config::genTagFile.isEmpty()) tagFile << ">" << name() << ":";
  
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
          if (!Config::genTagFile.isEmpty()) tagFile << cd->getOutputFileBase() << "?";
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

  if (!Config::genTagFile.isEmpty()) tagFile << " \"" << fileName << ".html\"\n";

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

  
  if (Config::haveDotFlag && Config::classGraphFlag)
    // write class diagram using dot
  {
    DotClassGraph inheritanceGraph(this,DotClassGraph::Inheritance);
    if (!inheritanceGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startDotGraph();
      parseText(ol,theTranslator->trClassDiagram(name()));
      ol.endDotGraph(inheritanceGraph);
      ol.popGeneratorState();
    }
  }
  else if (Config::classDiagramFlag && count>0) 
    // write class diagram using build-in generator
  {
    ClassDiagram diagram(this); // create a diagram of this class.
    ol.startClassDiagram();
    ol.disable(OutputGenerator::Man);
    parseText(ol,theTranslator->trClassDiagram(name()));
    ol.enable(OutputGenerator::Man);
    ol.endClassDiagram(diagram,fileName,name());
  } 

  if (Config::haveDotFlag && Config::collGraphFlag)
  {
    DotClassGraph usageImplGraph(this,DotClassGraph::Implementation);
    if (!usageImplGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startDotGraph();
      parseText(ol,theTranslator->trCollaborationDiagram(name()));
      ol.endDotGraph(usageImplGraph);
      ol.popGeneratorState();
    }
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

  ol.endTextBlock();
  
  // write member groups
  ol.startMemberSections();

  // write user defined member groups
  MemberGroupListIterator mgli(*memberGroupList);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    if (!mg->allMembersInSameSection() || !subGrouping) // group is in its own section
    {
      mg->writeDeclarations(ol,this,0,0,0);
    }
    else // add this group to the corresponding member section
    {
      mg->addToDeclarationSection();
    }
  }

  // non static public members
  pubTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicTypes(),0); 
  pubMembers.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicMembers(),0); 
  pubAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicAttribs(),0); 
  pubSlots.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicSlots(),0); 
  signals.writeDeclarations(ol,this,0,0,0,theTranslator->trSignals(),0); 
  // static public members
  pubStaticMembers.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPublicMembers(),0); 
  pubStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPublicAttribs(),0); 
  
  // protected non-static members
  proTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedTypes(),0); 
  proMembers.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedMembers(),0); 
  proAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedAttribs(),0); 
  proSlots.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedSlots(),0); 
  // protected static members
  proStaticMembers.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticProtectedMembers(),0); 
  proStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticProtectedAttribs(),0); 

  if (Config::extractPrivateFlag)
  {
    // private non-static members
    priTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateTypes(),0); 
    priMembers.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateMembers(),0); 
    priAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateAttribs(),0); 
    priSlots.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateSlots(),0); 
    // private static members
    priStaticMembers.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPrivateMembers(),0); 
    priStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPrivateAttribs(),0); 
  }
  friends.writeDeclarations(ol,this,0,0,0,theTranslator->trFriends(),0);
  related.writeDeclarations(ol,this,0,0,0,
                  theTranslator->trRelatedFunctions(),
                  theTranslator->trRelatedSubscript()
                 ); 
  ol.endMemberSections();
    
  // write detailed description
  bool exampleFlag=hasExamples();
  if ((!briefDescription().isEmpty() && Config::repeatBriefFlag) || 
      !documentation().isEmpty() || 
      /*(Config::sourceBrowseFlag && startBodyLine!=-1 && bodyDef) ||*/
      exampleFlag)
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
    ol.startTextBlock();
    
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
      ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::RTF);
        ol.newParagraph();
      ol.popGeneratorState();
      parseDoc(ol,defFileName,defLine,name(),0,documentation()+"\n");
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
      ol.newParagraph();
      writeExample(ol,exampleList);
      //ol.endDescItem();
      ol.endDescList();
    }
    ol.newParagraph();
    writeSourceDef(ol,name());
    ol.endTextBlock();
  }
  
  typedefMembers.countDocMembers();
  if (typedefMembers.totalCount()>0)
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberTypedefDocumentation());
    ol.endGroupHeader();
    typedefMembers.writeDocumentation(ol,name());
  }

  enumMembers.countDocMembers();
  if (enumMembers.totalCount()>0)
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberEnumerationDocumentation());
    ol.endGroupHeader();
    enumMembers.writeDocumentation(ol,name());
  }
  
  //enumValMembers.countDocMembers();
  //if (enumValMembers.totalCount()>0)
  //{
  //  ol.writeRuler();
  //  ol.startGroupHeader();
  //  parseText(ol,theTranslator->trEnumerationValueDocumentation());
  //  ol.endGroupHeader();
  //  enumValMembers.writeDocumentation(ol,name());
  //}

  constructors.countDocMembers();
  if (constructors.totalCount()>0) 
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trConstructorDocumentation());
    ol.endGroupHeader();
    constructors.writeDocumentation(ol,name());
  }

  functionMembers.countDocMembers();
  if (functionMembers.totalCount()>0) 
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberFunctionDocumentation());
    ol.endGroupHeader();
    functionMembers.writeDocumentation(ol,name());
  }

  relatedMembers.countDocMembers();
  if (relatedMembers.totalCount()>0)
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trRelatedFunctionDocumentation());
    ol.endGroupHeader();
    relatedMembers.writeDocumentation(ol,name());
  }

  variableMembers.countDocMembers();
  if (variableMembers.totalCount()>0) 
  {
    ol.writeRuler();
    ol.startGroupHeader();
    parseText(ol,theTranslator->trMemberDataDocumentation());
    ol.endGroupHeader();
    variableMembers.writeDocumentation(ol,name());
  }
  
  ol.startTextBlock();

  // write the list of used files (not for man pages)
  ol.pushGeneratorState();
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

      if (fd->generateSource() || (!fd->isReference() && Config::sourceBrowseFlag))
      {
        ol.writeObjectLink(0,fd->sourceName(),0,fd->name());
      }
      else if (fd->isLinkable())
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
  ol.popGeneratorState();

  ol.endTextBlock();
 
  endFile(ol);
}

// write the list of all (inherited) members for this class
void ClassDef::writeMemberList(OutputList &ol)
{
  if (allMemberNameInfoList->count()==0) return;
  // only for HTML
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  startFile(ol,memListFileName,theTranslator->trMemberList());
  startTitle(ol,0);
  parseText(ol,name()+" "+theTranslator->trMemberList());
  endTitle(ol,0,0);
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
          if ( md->isFunction() || md->isSignal() || md->isSlot() ||
               (md->isFriend() && md->argsString())) 
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
             md->isFriend() || md->isRelated() ||
             md->getMemberSpecifiers()!=0
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
            if (Config::inlineInfoFlag && md->isInline())        
                                       sl.append("inline");
            if (md->isExplicit())      sl.append("explicit");
            if (md->isMutable())       sl.append("mutable");
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
  ol.popGeneratorState();
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


bool ClassDef::hasNonReferenceSuperClass()
{
  bool found=!isReference();
  BaseClassListIterator bcli(*inheritedBy);
  for ( ; bcli.current() && !found ; ++bcli )
    found=found || bcli.current()->classDef->hasNonReferenceSuperClass();
  return found;
}

void ClassDef::writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup)
{
  //ol.insertMemberAlign();
  //printf("ClassName=`%s' inGroup=%d\n",name().data(),inGroup);

  if (inGroup && md && md->memberClass()==this) return;
  
  switch(compType)
  {
    case Class:  ol.docify("class"); break;
    case Struct: ol.docify("struct"); break;
    default:     ol.docify("union");  break; 
  } 
  int ri=name().findRev("::");
  if (ri==-1) ri=name().length();
  QCString cn=name().right(name().length()-ri-2);
  if (!cn.isEmpty() && cn.at(0)!='@' && md)
  { 
    ol.docify(" ");
    ol.writeObjectLink(0,0,md->anchor(),cn);
  }
  ol.docify(" {");
  ol.endMemberItem(FALSE); 

  // insert members of this class
  if (inGroup)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writePlainDeclarations(ol,this,0,0,0);
    }
  }
  else
  {
    pubTypes.writePlainDeclarations(ol,this,0,0,0); 
    pubMembers.writePlainDeclarations(ol,this,0,0,0); 
    pubAttribs.writePlainDeclarations(ol,this,0,0,0); 
    pubSlots.writePlainDeclarations(ol,this,0,0,0); 
    signals.writePlainDeclarations(ol,this,0,0,0); 
    pubStaticMembers.writePlainDeclarations(ol,this,0,0,0); 
    pubStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
    proTypes.writePlainDeclarations(ol,this,0,0,0); 
    proMembers.writePlainDeclarations(ol,this,0,0,0); 
    proAttribs.writePlainDeclarations(ol,this,0,0,0); 
    proSlots.writePlainDeclarations(ol,this,0,0,0); 
    proStaticMembers.writePlainDeclarations(ol,this,0,0,0); 
    proStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
    if (Config::extractPrivateFlag)
    {
      priTypes.writePlainDeclarations(ol,this,0,0,0); 
      priMembers.writePlainDeclarations(ol,this,0,0,0); 
      priAttribs.writePlainDeclarations(ol,this,0,0,0); 
      priSlots.writePlainDeclarations(ol,this,0,0,0); 
      priStaticMembers.writePlainDeclarations(ol,this,0,0,0); 
      priStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
    }
    friends.writePlainDeclarations(ol,this,0,0,0);
    related.writePlainDeclarations(ol,this,0,0,0); 
  }
}

/*! a link to this class is possible within this project */
bool ClassDef::isLinkableInProject() 
{ 
  return !name().isEmpty() && name().find('@')==-1 && 
    (prot!=Private || Config::extractPrivateFlag) &&
    hasDocumentation() && !isReference();
}

/*! the class is visible in a class diagram, or class hierarchy */
bool ClassDef::isVisibleInHierarchy() 
{ return // show all classes or a superclass is visible
  (Config::allExtFlag || hasNonReferenceSuperClass()) &&
    // and not an annonymous compound
    name().find('@')==-1 &&
    // and not privately inherited
    (prot!=Private || Config::extractPrivateFlag) &&
    // documented or show anyway or documentation is external 
    (hasDocumentation() || !Config::hideClassFlag || isReference());
}

//----------------------------------------------------------------------
// recursive function:
// returns TRUE iff class definition `bcd' represents an (in)direct base 
// class of class definition `cd'.

bool ClassDef::isBaseClass(ClassDef *bcd)
{
  bool found=FALSE;
  //printf("isBaseClass(cd=%s) looking for %s\n",cd->name().data(),bcd->name().data());
  BaseClassListIterator bcli(*baseClasses());
  for ( ; bcli.current() && !found ; ++bcli)
  {
    ClassDef *ccd=bcli.current()->classDef;
    //printf("isBaseClass() baseclass %s\n",ccd->name().data());
    if (ccd==bcd) 
      found=TRUE;
    else 
      found=ccd->isBaseClass(bcd);
  }
  return found;
}

//----------------------------------------------------------------------------
/*! 
 * recusively merges the `all members' lists of a class base 
 * with that of this class. Must only be called for classes without
 * superclasses!
 */

void ClassDef::mergeMembers(/*ClassDef *cd,BaseClassList *bcl*/)
{
  BaseClassListIterator bcli(*baseClasses());
  BaseClassDef *bcd;
  for ( ; (bcd=bcli.current()) ; ++bcli )
  {
    ClassDef *bClass=bcd->classDef; 
    // merge the members of bClass with the onces from cd
    
    bClass->mergeMembers();
    // the all member list of the branch until bClass is now complete
    // so we can merge it with cd
    
    MemberNameInfoList *srcMnl  = bClass->memberNameInfoList();
    MemberNameInfoDict *dstMnd  =         memberNameInfoDict();
    MemberNameInfoList *dstMnl  =         memberNameInfoList();
    
    MemberNameInfoListIterator srcMnili(*srcMnl);
    MemberNameInfo *srcMni;
    for ( ; (srcMni=srcMnili.current()) ; ++srcMnili)
    {
      //printf("Base member name %s\n",srcMni->memberName());
      MemberNameInfo *dstMni;
      if ((dstMni=dstMnd->find(srcMni->memberName())))
        // a member with that name is already in the class.
        // the member may hide or reimplement the one in the super class
        // or there may be another path to the base class that is already 
        // visited via another branch in the class hierarchy.
      {
        MemberNameInfoIterator srcMnii(*srcMni); 
        MemberInfo *srcMi;
        for ( ; (srcMi=srcMnii.current()) ; ++srcMnii )
        {
          MemberDef *srcMd = srcMi->memberDef;
          bool found=FALSE;
          bool ambigue=FALSE;
          bool hidden=FALSE;
          MemberNameInfoIterator dstMnii(*dstMni); 
          MemberInfo *dstMi;
          ClassDef *srcCd = srcMd->memberClass();
          for ( ; (dstMi=dstMnii.current()) && !found; ++dstMnii )
          {
            MemberDef *dstMd = dstMi->memberDef;
            if (srcMd!=dstMd) // different members
            {
              ClassDef *dstCd = dstMd->memberClass();
              //printf("Is %s a base class of %s?\n",srcCd->name(),dstCd->name());
              if (srcCd==dstCd || dstCd->isBaseClass(srcCd)) 
                // member is in the same or a base class
              {
                found=matchArguments(srcMd->argumentList(),
                                     dstMd->argumentList()
                                    );
                //ambigue = ambigue || !found;
                hidden  = hidden  || !found;
              }
              else // member is in a non base class => multiple inheritance
                   // using the same base class.
              {
                //printf("$$ Existing member %s %s add scope %s\n",
                //    dstMi->ambiguityResolutionScope.data(),
                //    dstMd->name().data(),
                //    dstMi->scopePath.left(dstMi->scopePath.find("::")+2).data());
                         
                QCString scope=dstMi->scopePath.left(dstMi->scopePath.find("::")+2);
                if (scope!=dstMi->ambiguityResolutionScope.left(scope.length()))
                  dstMi->ambiguityResolutionScope.prepend(scope);
                ambigue=TRUE;
              }
            }
            else // same members
            {
              // do not add if base class is virtual or 
              // if scope paths are equal
              if ((srcMi->virt==Virtual && dstMi->virt==Virtual) ||
                  bClass->name()+"::"+srcMi->scopePath == dstMi->scopePath
                 ) 
              {
                found=TRUE;
              }
              else // member can be reached via multiple paths in the 
                   // inheritance tree
              {
                //printf("$$ Existing member %s %s add scope %s\n",
                //    dstMi->ambiguityResolutionScope.data(),
                //    dstMd->name().data(),
                //    dstMi->scopePath.left(dstMi->scopePath.find("::")+2).data());

                QCString scope=dstMi->scopePath.left(dstMi->scopePath.find("::")+2);
                if (scope!=dstMi->ambiguityResolutionScope.left(scope.length()))
                  dstMi->ambiguityResolutionScope.prepend(scope);
                ambigue=TRUE;
              }
            }
          }
          //printf("member %s::%s hidden %d ambigue %d srcMi->ambigClass=%p\n",
          //    srcCd->name().data(),srcMd->name().data(),hidden,ambigue,srcMi->ambigClass);

          // TODO: fix the case where a member is hidden by inheritance
          //       of a member with the same name but with another prototype,
          //       while there is more than one path to the member in the 
          //       base class due to multiple inheritance. In this case
          //       it seems that the member is not reachable by prefixing a 
          //       scope name either (according to my compiler). Currently, 
          //       this case is shown anyway.
          if (!found && srcMd->protection()!=Private)
          {
            Specifier virt=srcMi->virt;
            if (srcMi->virt==Normal && bcd->virt!=Normal) virt=bcd->virt;
            MemberInfo *newMi = new MemberInfo(srcMd,bcd->prot,virt);
            newMi->scopePath=bClass->name()+"::"+srcMi->scopePath;
            if (ambigue)
            {
              //printf("$$ New member %s %s add scope %s::\n",
              //     srcMi->ambiguityResolutionScope.data(),
              //     srcMd->name().data(),
              //     bClass->name().data());

              QCString scope=bClass->name()+"::";
              if (scope!=srcMi->ambiguityResolutionScope.left(scope.length()))
                newMi->ambiguityResolutionScope=
                  scope+srcMi->ambiguityResolutionScope.copy();
            }
            if (hidden)
            {
              if (srcMi->ambigClass==0)
              {
                newMi->ambigClass=bClass;
                newMi->ambiguityResolutionScope=bClass->name()+"::";
              }
              else
              {
                newMi->ambigClass=srcMi->ambigClass;
                newMi->ambiguityResolutionScope=srcMi->ambigClass->name()+"::";
              }
            }
            //printf("Adding!\n");
            dstMni->append(newMi);
          }
        }
      }
      else // base class has a member that is not in the super class => copy
      {
        // create a deep copy of the list (only the MemberInfo's will be 
        // copied, not the actual MemberDef's)
        MemberNameInfo *newMni = new MemberNameInfo(srcMni->memberName()); 
        
        // copy the member(s) from the base to the super class
        MemberNameInfoIterator mnii(*srcMni);
        MemberInfo *mi;
        for (;(mi=mnii.current());++mnii)
        {
          if (mi->memberDef->protection()!=Private)
          {
            Specifier virt=mi->virt;
            if (mi->virt==Normal && bcd->virt!=Normal) virt=bcd->virt;
            MemberInfo *newMi=new MemberInfo(mi->memberDef,bcd->prot,virt);
            //if (mi->memberDef->memberClass()!=bClass)
            newMi->scopePath=bClass->name()+"::"+mi->scopePath;
            newMi->ambigClass=mi->ambigClass;
            newMi->ambiguityResolutionScope=mi->ambiguityResolutionScope.copy();
            newMni->append(newMi);
          }
        }
        
        // add it to the list and dictionary
        dstMnl->inSort(newMni);
        dstMnd->insert(newMni->memberName(),newMni);
      }
    }
  }
}

//----------------------------------------------------------------------------
/*! Builds up a dictionary of all classes that are used by the state of this 
 *  class (the "implementation"). 
 *  Must be called before mergeMembers() is called!
 */

void ClassDef::determineImplUsageRelation()
{
  MemberNameInfoListIterator mnili(*allMemberNameInfoList);
  MemberNameInfo *mni;
  for (;(mni=mnili.current());++mnili)
  {
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      MemberDef *md=mi->memberDef;
      if (md->isVariable()) // for each member variable in this class
      {
        QCString type=md->typeString();
        int typeLen=type.length();
        static const QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
        //printf("in class %s found var type=`%s' name=`%s'\n",
        //            name().data(),type.data(),md->name().data());
        int p=0,i,l;
        bool found=FALSE;
        while ((i=re.match(type,p,&l))!=-1 && !found) // for each class name in the type
        {
          int ts=i+l;
          int te=ts;
          while (type.at(ts)==' ' && ts<typeLen) ts++; // skip any whitespace
          if (type.at(ts)=='<') // assume template instance
          {
            // locate end of template
            te=ts+1;
            int brCount=1;
            while (te<typeLen && brCount!=0)
            {
              if (type.at(te)=='<') brCount++;
              if (type.at(te)=='>') brCount--;
              te++;
            }
          }
          QCString templSpec;
          if (te>ts) templSpec = type.mid(ts,te-ts);
          ClassDef *cd=getResolvedClass(name()+"::"+type.mid(i,l));
          if (cd==0) cd=getResolvedClass(type.mid(i,l)); // TODO: also try inbetween scopes!
          if (cd) // class exists 
          {
            found=TRUE;
            if (usesImplClassDict==0) usesImplClassDict = new UsesClassDict(257); 
            UsesClassDef *ucd=usesImplClassDict->find(cd->name());
            if (ucd==0 || ucd->templSpecifiers!=templSpec)
            {
              ucd = new UsesClassDef(cd);
              usesImplClassDict->insert(cd->name(),ucd);
              ucd->templSpecifiers = templSpec;
              //printf("Adding used class %s to class %s\n",
              //    cd->name().data(),name().data());
            }
            ucd->addAccessor(md->name());
            //printf("Adding accessor %s to class %s\n",
            //    md->name().data(),ucd->classDef->name().data());
          }
          p=i+l;
        }
      }
    }
  }
#ifdef DUMP
  if (usesClassDict)
  {
    msg("Class %s uses the following classes:\n",name().data());
    UsesClassDictIterator ucdi(*usesClassDict);
    UsesClassDef *ucd;
    for (;(ucd=ucdi.current());++ucdi)
    {
      msg("  %s via ",ucd->classDef->name().data());
      QDictIterator<void> dvi(*ucd->accessors); 
      const char *s;
      for (;(s=dvi.currentKey());++dvi)
      {
        msg("%s ",s);
      }
      msg("\n");
    }
  }
#endif
}

//----------------------------------------------------------------------------

#if 0
// I have disabled this code because the graphs it renders quickly become
// too large to be of practical use.

void ClassDef::addUsedInterfaceClasses(MemberDef *md,const char *typeStr)
{
  QCString type = typeStr;
  static const QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
  int p=0,i,l;
  while ((i=re.match(type,p,&l))!=-1) // for each class name in the type
  {
    ClassDef *cd=getClass(name()+"::"+type.mid(i,l));
    if (cd==0) cd=getClass(type.mid(i,l)); // TODO: also try inbetween scopes!
    if (cd && cd!=this && !isBaseClass(cd))
    {
      if (usesIntfClassDict==0) 
      {
        usesIntfClassDict = new UsesClassDict(257); 
      }
      UsesClassDef *ucd=usesIntfClassDict->find(cd->name());
      if (ucd==0)
      {
        ucd = new UsesClassDef(cd);
        usesIntfClassDict->insert(cd->name(),ucd);
        //printf("in class `%s' adding used intf class `%s'\n",
        //  name().data(),cd->name().data());
      }
      ucd->addAccessor(md->name());
      //printf("in class `%s' adding accessor `%s' to class `%s'\n",
      //    name().data(),md->name().data(),ucd->classDef->name().data());
    }
    p=i+l;
  }
}

void ClassDef::determineIntfUsageRelation()
{
  MemberNameInfoListIterator mnili(*allMemberNameInfoList);
  MemberNameInfo *mni;
  for (;(mni=mnili.current());++mnili)
  {
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      MemberDef *md=mi->memberDef;
      
      // compute the protection level for this member
      Protection protect=md->protection();
      if (mi->prot==Protected) // inherited protection
      {
        if (protect==Public) protect=Protected;
        else if (protect==Protected) protect=Private;
      }
      
      if (!md->name().isEmpty() && md->name()[0]!='@' && 
          (mi->prot!=Private && protect!=Private)
         )
      {
        // add classes found in the return type
        addUsedInterfaceClasses(md,md->typeString());
        ArgumentList *al = md->argumentList();
        if (al) // member has arguments
        {
          // add classes found in the types of the argument list
          ArgumentListIterator ali(*al);
          Argument *a;
          for (;(a=ali.current());++ali)
          {
            if (!a->type.isEmpty() && a->type.at(0)!='@')
            {
              addUsedInterfaceClasses(md,a->type);
            }
          }
        }
      }
    }
  }
}
#endif
