/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
#include "qtbc.h"
#include "groupdef.h"
#include "classdef.h"
#include "filedef.h"
#include "classlist.h"
#include "outputlist.h"
#include "namespacedef.h"
#include "language.h"
#include "util.h"
#include "memberlist.h"
#include "message.h"
#include "membergroup.h"
#include "doxygen.h"
#include "pagedef.h"
#include "docparser.h"
#include "searchindex.h"

GroupDef::GroupDef(const char *df,int dl,const char *na,const char *t,
                   const char *refFileName) : Definition(df,dl,na)
{
  fileList = new FileList;
  classSDict = new ClassSDict(257);
  groupList = new GroupList;
  namespaceList = new NamespaceList;
  pageDict = new PageSDict(257);
  exampleDict = new PageSDict(257);
  allMemberList = new MemberList;
  allMemberNameInfoSDict = new MemberNameInfoSDict(17);
  if (refFileName)
  {
    fileName=stripExtension(refFileName);
  }
  else
  {
    fileName = (QCString)"group_"+na;
  }
  setGroupTitle( t );
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);

  decDefineMembers.setInGroup(TRUE);
  decProtoMembers.setInGroup(TRUE);
  decTypedefMembers.setInGroup(TRUE);
  decEnumMembers.setInGroup(TRUE);
  decEnumValMembers.setInGroup(TRUE);
  decFuncMembers.setInGroup(TRUE);
  decVarMembers.setInGroup(TRUE);

  docDefineMembers.setInGroup(TRUE);
  docProtoMembers.setInGroup(TRUE);
  docTypedefMembers.setInGroup(TRUE);
  docEnumMembers.setInGroup(TRUE);
  docFuncMembers.setInGroup(TRUE);
  docVarMembers.setInGroup(TRUE);

  visited = 0;
}

GroupDef::~GroupDef()
{
  delete fileList;
  delete classSDict;
  delete groupList;
  delete namespaceList;
  delete pageDict;
  delete exampleDict;
  delete allMemberList;
  delete allMemberNameInfoSDict;
  delete memberGroupSDict;
}

void GroupDef::setGroupTitle( const char *t )
{
  if ( t && strlen(t) )
  {
    title = t;
    titleSet = TRUE;
  }
  else
  {
    title = name();
    title.at(0)=toupper(title.at(0));
    titleSet = FALSE;
  }
}


void GroupDef::distributeMemberGroupDocumentation()
{
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void GroupDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->findSectionsInDocumentation();
  }
  decDefineMembers.findSectionsInDocumentation();
  decProtoMembers.findSectionsInDocumentation();
  decTypedefMembers.findSectionsInDocumentation();
  decEnumMembers.findSectionsInDocumentation();
  decFuncMembers.findSectionsInDocumentation();
  decVarMembers.findSectionsInDocumentation();
}

void GroupDef::addFile(const FileDef *def)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    fileList->inSort(def);
  else
    fileList->append(def);
}

void GroupDef::addClass(const ClassDef *cd)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    classSDict->inSort(cd->name(),cd);
  else
    classSDict->append(cd->name(),cd);
}

void GroupDef::addNamespace(const NamespaceDef *def)
{
  if (Config_getBool("SORT_MEMBER_DOCS"))
    namespaceList->inSort(def);  
  else
    namespaceList->append(def);
}

void GroupDef::addPage(PageDef *def)
{
  //printf("Making page %s part of a group\n",def->name.data());
  pageDict->append(def->name(),def);
  def->makePartOfGroup(this);
}

void GroupDef::addExample(const PageDef *def)
{
  exampleDict->append(def->name(),def);
}


void GroupDef::addMembersToMemberGroup()
{
  ::addMembersToMemberGroup(&decDefineMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decProtoMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decTypedefMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decEnumMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decEnumValMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decFuncMembers,memberGroupSDict,this);
  ::addMembersToMemberGroup(&decVarMembers,memberGroupSDict,this);

  //printf("GroupDef::addMembersToMemberGroup() memberGroupList=%d\n",memberGroupList->count());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->setInGroup(TRUE);
  }
}


bool GroupDef::insertMember(MemberDef *md,bool docOnly)
{
  //printf("GroupDef(%s)::insertMember(%s)\n", title.data(), md->name().data());
  MemberNameInfo *mni=0;
  if ((mni=(*allMemberNameInfoSDict)[md->name()]))
  { // member with this name already found
    MemberNameInfoIterator srcMnii(*mni); 
    MemberInfo *srcMi;
    for ( ; (srcMi=srcMnii.current()) ; ++srcMnii )
    {
      MemberDef *srcMd = srcMi->memberDef;
      if (srcMd==md) return FALSE; // already added before!

      bool sameScope = srcMd->getOuterScope()==md->getOuterScope() || // same class or namespace
          // both inside a file => definition and declaration do not have to be in the same file
           (srcMd->getOuterScope()->definitionType()==Definition::TypeFile &&
            md->getOuterScope()->definitionType()==Definition::TypeFile); 
      
      if (srcMd->isFunction() && md->isFunction() && 
          matchArguments(srcMd->argumentList(),md->argumentList()) &&
          sameScope
         )
      {
        if (srcMd->getGroupAlias()==0) 
        {
          md->setGroupAlias(srcMd); 
        }
        else
        {
          md->setGroupAlias(srcMd->getGroupAlias()); 
        }
        return FALSE; // member is the same as one that is already added
      }
    }
    mni->append(new MemberInfo(md,md->protection(),md->virtualness(),FALSE));
  }
  else
  {
    mni = new MemberNameInfo(md->name());
    mni->append(new MemberInfo(md,md->protection(),md->virtualness(),FALSE));
    allMemberNameInfoSDict->append(mni->memberName(),mni);
  }
  //printf("Added member!\n");
  allMemberList->append(md); 
  switch(md->memberType())
  {
    case MemberDef::Variable:     
      if (!docOnly) decVarMembers.append(md); 
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docVarMembers.inSort(md); 
      else
        docVarMembers.append(md);
      break;
    case MemberDef::Function: 
      if (!docOnly) decFuncMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))    
        docFuncMembers.inSort(md); 
      else
        docFuncMembers.append(md);
      break;
    case MemberDef::Typedef:      
      if (!docOnly) decTypedefMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docTypedefMembers.inSort(md); 
      else
        docTypedefMembers.append(md);
      break;
    case MemberDef::Enumeration:  
      if (!docOnly) decEnumMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docEnumMembers.inSort(md); 
      else
        docEnumMembers.append(md);
      break;
    case MemberDef::EnumValue:    
      break;
    case MemberDef::Prototype:    
      if (!docOnly) decProtoMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docProtoMembers.inSort(md); 
      else
        docProtoMembers.append(md);
      break;
    case MemberDef::Define:       
      if (!docOnly) decDefineMembers.append(md);
      if (Config_getBool("SORT_MEMBER_DOCS"))
        docDefineMembers.inSort(md); 
      else
        docDefineMembers.append(md);
      break;
    default:
      err("GroupDef::insertMembers(): "
           "member `%s' with class scope `%s' inserted in group scope `%s'!\n",
           md->name().data(),
           md->getClassDef() ? md->getClassDef()->name().data() : "",
           name().data());
  }
  return TRUE;
}

void GroupDef::removeMember(MemberDef *md)
{
  // fprintf(stderr, "GroupDef(%s)::removeMember( %s )\n", title.data(), md->name().data());
  MemberNameInfo *mni = allMemberNameInfoSDict->find(md->name());
  if (mni)
  {
    MemberNameInfoIterator mnii(*mni);
    while( mnii.current() )
    {
      if( mnii.current()->memberDef == md )
      {
	mni->remove(mnii.current());
        break;
      }
      ++mnii;
    }
    if( mni->isEmpty() )
    {
      allMemberNameInfoSDict->remove(md->name());
      delete mni;
    }

    allMemberList->remove(md); 
    switch(md->memberType())
    {
      case MemberDef::Variable:
	decVarMembers.remove(md);
        docVarMembers.remove(md);
        break;
      case MemberDef::Function: 
        decFuncMembers.remove(md);
        docFuncMembers.remove(md);
        break;
      case MemberDef::Typedef:      
        decTypedefMembers.remove(md);
        docTypedefMembers.remove(md);
        break;
      case MemberDef::Enumeration:  
        decEnumMembers.remove(md);
        docEnumMembers.remove(md);
        break;
      case MemberDef::EnumValue:    
        break;
      case MemberDef::Prototype:    
        decProtoMembers.remove(md);
        docProtoMembers.remove(md);
        break;
      case MemberDef::Define:       
        decDefineMembers.remove(md);
        docDefineMembers.remove(md);
        break;
      default:
        err("GroupDef::removeMember(): unexpected member remove in file!\n");
    }
  }
}

bool GroupDef::containsGroup(const GroupDef *def)
{
    return groupList->find(def) >= 0;
}

void GroupDef::addGroup(const GroupDef *def)
{
  //printf("adding group `%s' to group `%s'\n",def->name().data(),name().data());
  //if (Config_getBool("SORT_MEMBER_DOCS"))
  //  groupList->inSort(def);
  //else
  groupList->append(def);
}

bool GroupDef::isASubGroup() const
{
  return m_partOfGroups && m_partOfGroups->count()!=0;
}

int GroupDef::countMembers() const
{
  return fileList->count()+
         classSDict->count()+
         namespaceList->count()+
         groupList->count()+
         allMemberList->count()+
         pageDict->count()+
         exampleDict->count();
}

/*! Compute the HTML anchor names for all members in the group */ 
void GroupDef::computeAnchors()
{
  //printf("GroupDef::computeAnchors()\n");
  setAnchors(0,'a',allMemberList);
}

void GroupDef::writeDetailedDocumentation(OutputList &ol)
{
  if (!briefDescription().isEmpty() || !documentation().isEmpty())
  {
    
    if (pageDict->count()!=countMembers()) // classical layout
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
        ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
        ol.newParagraph();
      }
    }

    // write documentation
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
  }
}


void GroupDef::writeDocumentation(OutputList &ol)
{
  ol.pushGeneratorState();
  //ol.disable(OutputGenerator::Man);
  startFile(ol,getOutputFileBase(),name(),title);
  startTitle(ol,getOutputFileBase());
  ol.docify(title);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),title);

  if (Config_getBool("SEARCHENGINE"))
  {
    Doxygen::searchIndex->setCurrentDoc(title,getOutputFileBase());
    Doxygen::searchIndex->addWord(localName().lower());
  }

  if (Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
    ol.newParagraph();
  }
  else if (!briefDescription().isEmpty())
  {
    ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
    ol.writeString(" \n");
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.startTextLink(0,"_details");
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    //ol.enable(OutputGenerator::Latex);
    ol.popGeneratorState();
  }

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"group\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <title>" << convertToXML(title) << "</title>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }
  
  ol.startMemberSections();

  // write list of files
  if (fileList->count()>0)
  {
    ol.startMemberHeader();
    ol.parseText(theTranslator->trFile(TRUE,FALSE));
    ol.endMemberHeader();
    ol.startMemberList();
    FileDef *fd=fileList->first();
    while (fd)
    {
      ol.startMemberItem(0);
      ol.docify("file ");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
      }
      ol.endMemberItem();
      if (!fd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.newParagraph();
      }
      fd=fileList->next();
    }
    ol.endMemberList();
  }

  // write list of namespaces
  if (namespaceList->count()>0)
  {
    ol.startMemberHeader();
    ol.parseText(theTranslator->trNamespaces());
    ol.endMemberHeader();
    ol.startMemberList();
    NamespaceDef *nd=namespaceList->first();
    while (nd)
    {
      ol.startMemberItem(0);
      ol.docify("namespace ");
      ol.insertMemberAlign();
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),0,nd->name());
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <namespace>" << convertToXML(nd->name()) << "</namespace>" << endl;
      }
      ol.endMemberItem();
      if (!nd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),nd,0,nd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.newParagraph();
      }
      nd=namespaceList->next();
    }
    ol.endMemberList();
  }

  // write list of groups
  if (groupList->count()>0)
  {
    ol.startMemberHeader();
    ol.parseText(theTranslator->trModules());
    ol.endMemberHeader();
    ol.startMemberList();
    GroupDef *gd=groupList->first();
    while (gd)
    {
      ol.startMemberItem(0);
      //ol.insertMemberAlign();
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),0,gd->groupTitle());
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <subgroup>" << convertToXML(gd->name()) << "</subgroup>" << endl;
      }
      ol.endMemberItem();
      if (!gd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),gd,0,gd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.newParagraph();
      }
      gd=groupList->next();
    }
    ol.endMemberList();
  }

  // write list of classes
  classSDict->writeDeclaration(ol);

  // write list of members
  if (allMemberList->count()>0)
  {
    /* write user defined member groups */
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writeDeclarations(ol,0,0,0,this);
    }

    //allMemberList->writeDeclarations(ol,0,0,0,this,0,0); 
    decDefineMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trDefines(),0);
    decProtoMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trFuncProtos(),0);
    decTypedefMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trTypedefs(),0);
    decEnumMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trEnumerations(),0);
    decFuncMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trFunctions(),0);
    decVarMembers.writeDeclarations(ol,0,0,0,this,theTranslator->trVariables(),0);
  }
  ol.endMemberSections();

  if (!Config_getBool("DETAILS_AT_TOP"))
  {
    writeDetailedDocumentation(ol);
  }

  PageDef *pd=0;
  PageSDict::Iterator pdi(*pageDict);
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->isReference())
    {
      QCString pageName = pd->getOutputFileBase();

      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <page>" << convertToXML(pageName) << "</page>" << endl;
      }

      SectionInfo *si=0;
      if (!pd->title().isEmpty() && !pd->name().isEmpty() &&
          (si=Doxygen::sectionDict[pd->name()])!=0)
      {
        ol.startSection(si->label,si->title,SectionInfo::Subsection);
        ol.docify(si->title);
        ol.endSection(si->label,SectionInfo::Subsection);
      }
      ol.startTextBlock();
      ol.parseDoc(pd->docFile(),pd->docLine(),pd,0,pd->documentation(),TRUE,FALSE);
      ol.endTextBlock();
    }
  }

  docDefineMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trDefineDocumentation());
  
  docProtoMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trFunctionPrototypeDocumentation());

  docTypedefMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trTypedefDocumentation());
  
  docEnumMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trEnumerationTypeDocumentation());

  docFuncMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trFunctionDocumentation());
  
  docVarMembers.writeDocumentation(ol,name(),this,
                             theTranslator->trVariableDocumentation());

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  endFile(ol); 
  ol.popGeneratorState();
}

//---- helper functions ------------------------------------------------------

void addClassToGroups(Entry *root,ClassDef *cd)
{
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname]))
    {
      gd->addClass(cd);
      cd->makePartOfGroup(gd);
      //printf("Compound %s: in group %s\n",cd->name().data(),s->data());
    }
  }
}

void addNamespaceToGroups(Entry *root,NamespaceDef *nd)
{
  //printf("root->groups->count()=%d\n",root->groups->count());
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    //printf("group `%s'\n",s->data());
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname]))
    {
      gd->addNamespace(nd);
      nd->makePartOfGroup(gd);
      //printf("Namespace %s: in group %s\n",nd->name().data(),s->data());
    }
  }
}

void addGroupToGroups(Entry *root,GroupDef *subGroup)
{
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname]) &&
	!gd->containsGroup(subGroup) )
    {
      gd->addGroup(subGroup);
      subGroup->makePartOfGroup(gd);
    }
  }
}

/*! Add a member to the group with the highest priority */
void addMemberToGroups(Entry *root,MemberDef *md)
{
  //printf("addMemberToGroups:  Root %p = %s, md %p=%s groups=%d\n", 
  //    root, root->name.data(), md, md->name().data(), root->groups->count() );
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;

  // Search entry's group list for group with highest pri.
  Grouping::GroupPri_t pri = Grouping::GROUPING_LOWEST;
  GroupDef *fgd=0;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    if (!g->groupname.isEmpty() &&
        (gd=Doxygen::groupSDict[g->groupname]) &&
	g->pri >= pri)
    {
      if (fgd && g->pri==pri) 
      {
         warn(root->fileName.data(), root->startLine,
           "Warning: Member %s found in multiple %s groups! "
           "The member will be put in group %s, and not in group %s",
	   md->name().data(), Grouping::getGroupPriName( pri ),
	   gd->name().data(), fgd->name().data()
          );
      }

      fgd = gd;
      pri = g->pri;
    }
  }
  //printf("fgd=%p\n",fgd);

  // put member into group defined by this entry?
  if (fgd)
  {
    GroupDef *mgd = md->getGroupDef();
    //printf("mgd=%p\n",mgd);
    bool insertit = FALSE;
    if (mgd==0)
    {
      insertit = TRUE;
    }
    else if (mgd!=fgd)
    {
      bool moveit = FALSE;

      // move member from one group to another if 
      // - the new one has a higher priority
      // - the new entry has the same priority, but with docs where the old one had no docs
      if (md->getGroupPri()<pri)
      {
        moveit = TRUE;
      }
      else
      {
        if (md->getGroupPri()==pri)
        {
          if (!root->doc.isEmpty() && !md->getGroupHasDocs())
          {
            moveit = TRUE;
          }
          else if (!root->doc.isEmpty() && md->getGroupHasDocs())
          {
            warn(md->getGroupFileName(),md->getGroupStartLine(),
                "Warning: Member documentation for %s found several times in %s groups!\n"
                "%s:%d: The member will remain in group %s, and won't be put into group %s",
                md->name().data(), Grouping::getGroupPriName( pri ),
                root->fileName.data(), root->startLine,
                mgd->name().data(),
                fgd->name().data()
                );
          }
        }
      }

      if (moveit)
      {
        //printf("removeMember\n");
        mgd->removeMember(md);
        insertit = TRUE;
      }
    }

    if (insertit)
    {
      //printf("insertMember found at %s line %d\n",md->getDefFileName().data(),md->getDefLine());
      bool success = fgd->insertMember(md);
      if (success)
      {
        //printf("insertMember successful\n");
        md->setGroupDef(fgd,pri,root->fileName,root->startLine,!root->doc.isEmpty());
        ClassDef *cd = md->getClassDefOfAnonymousType();
        if (cd) cd->setGroupDefForAllMembers(fgd,pri,root->fileName,root->startLine,root->doc.length() != 0);
      }
    }
  }
}


void addExampleToGroups(Entry *root,PageDef *eg)
{
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname]))
    {
      gd->addExample(eg);
      eg->makePartOfGroup(gd);
      //printf("Example %s: in group %s\n",eg->name().data(),s->data());
    }
  }
}

QCString GroupDef::getOutputFileBase() const 
{ 
  if (isReference())
  {
    return fileName;
  }
  else
  {
    return convertNameToFile(fileName); 
  }
}

void GroupDef::addListReferences()
{
  addRefItem(xrefListItems(),
             theTranslator->trGroup(TRUE,TRUE),
             getOutputFileBase(),name()
            );
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->addListReferences(this);
  }
  docDefineMembers.addListReferences(this);
  docProtoMembers.addListReferences(this);
  docTypedefMembers.addListReferences(this);
  docEnumMembers.addListReferences(this);
  docFuncMembers.addListReferences(this);
  docVarMembers.addListReferences(this);
}


