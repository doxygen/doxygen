/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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
#include <qregexp.h>
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
#include "dot.h"
#include "vhdldocgen.h"
#include "layout.h"

//---------------------------------------------------------------------------

GroupDef::GroupDef(const char *df,int dl,const char *na,const char *t,
                   const char *refFileName) : Definition(df,dl,na)
{
  fileList = new FileList;
  classSDict = new ClassSDict(17);
  groupList = new GroupList;
  namespaceSDict = new NamespaceSDict(17);
  pageDict = new PageSDict(17);
  exampleDict = new PageSDict(17);
  dirList = new DirList;
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

  allMemberList = new MemberList(MemberList::allMembersList);

  visited = 0;
  groupScope = 0;
}

GroupDef::~GroupDef()
{
  delete fileList;
  delete classSDict;
  delete groupList;
  delete namespaceSDict;
  delete pageDict;
  delete exampleDict;
  delete allMemberList;
  delete allMemberNameInfoSDict;
  delete memberGroupSDict;
  delete dirList;
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

  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberList::declarationLists)
    {
      ml->findSectionsInDocumentation();
    }
  }
}

void GroupDef::addFile(const FileDef *def)
{
  if (def->isHidden()) return;
  if (Config_getBool("SORT_BRIEF_DOCS"))
    fileList->inSort(def);
  else
    fileList->append(def);
}

bool GroupDef::addClass(const ClassDef *cd)
{
  if (cd->isHidden()) return FALSE;
  if (classSDict->find(cd->name())==0)
  {
    if (Config_getBool("SORT_BRIEF_DOCS"))
      classSDict->inSort(cd->name(),cd);
    else
      classSDict->append(cd->name(),cd);
    return TRUE;
  }
  return FALSE;
}

bool GroupDef::addNamespace(const NamespaceDef *def)
{
  if (def->isHidden()) return FALSE;
  if (namespaceSDict->find(def->name())==0)
  {
    if (Config_getBool("SORT_BRIEF_DOCS"))
      namespaceSDict->inSort(def->name(),def);  
    else
      namespaceSDict->append(def->name(),def);
    return TRUE;
  }
  return FALSE;
}

void GroupDef::addDir(const DirDef *def)
{
  if (def->isHidden()) return;
  if (Config_getBool("SORT_BRIEF_DOCS"))
    dirList->inSort(def);  
  else
    dirList->append(def);
}

void GroupDef::addPage(PageDef *def)
{
  if (def->isHidden()) return;
  //printf("Making page %s part of a group\n",def->name.data());
  pageDict->append(def->name(),def);
  def->makePartOfGroup(this);
}

void GroupDef::addExample(const PageDef *def)
{
  if (def->isHidden()) return;
  exampleDict->append(def->name(),def);
}


void GroupDef::addMembersToMemberGroup()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberList::declarationLists)
    {
      ::addMembersToMemberGroup(ml,&memberGroupSDict,this);
    }
  }

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
  if (md->isHidden()) return FALSE;
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

      LockingPtr<ArgumentList> srcMdAl = srcMd->argumentList();
      LockingPtr<ArgumentList> mdAl    = md->argumentList();
      
      if (srcMd->isFunction() && md->isFunction() && 
          matchArguments2(srcMd->getOuterScope(),srcMd->getFileDef(),srcMdAl.pointer(),
                          md->getOuterScope(),md->getFileDef(),mdAl.pointer(),
                          TRUE
                         ) &&
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
      if (!docOnly)
      {
        addMemberToList(MemberList::decVarMembers,md);
      }
      addMemberToList(MemberList::docVarMembers,md);
      break;
    case MemberDef::Function: 
      if (!docOnly)
      {
        addMemberToList(MemberList::decFuncMembers,md);
      }
      addMemberToList(MemberList::docFuncMembers,md);
      break;
    case MemberDef::Typedef:      
      if (!docOnly)
      {
        addMemberToList(MemberList::decTypedefMembers,md);
      }
      addMemberToList(MemberList::docTypedefMembers,md);
      break;
    case MemberDef::Enumeration:  
      if (!docOnly)
      {
        addMemberToList(MemberList::decEnumMembers,md);
      }
      addMemberToList(MemberList::docEnumMembers,md);
      break;
    case MemberDef::EnumValue:    
      if (!docOnly)
      {
        addMemberToList(MemberList::decEnumValMembers,md);
      }
      addMemberToList(MemberList::docEnumValMembers,md);
      break;
    case MemberDef::Define:       
      if (!docOnly)
      {
        addMemberToList(MemberList::decDefineMembers,md);
      }
      addMemberToList(MemberList::docDefineMembers,md);
      break;
    case MemberDef::Signal:       
      if (!docOnly)
      {
        addMemberToList(MemberList::decSignalMembers,md);
      }
      addMemberToList(MemberList::docSignalMembers,md);
      break;
    case MemberDef::Slot:       
      if (md->protection()==Public)
      {
        if (!docOnly)
        {
          addMemberToList(MemberList::decPubSlotMembers,md);
        }
        addMemberToList(MemberList::docPubSlotMembers,md);
      }
      else if (md->protection()==Protected)
      {
        if (!docOnly)
        {
          addMemberToList(MemberList::decProSlotMembers,md);
        }
        addMemberToList(MemberList::docProSlotMembers,md);
      }
      else
      {
        if (!docOnly)
        {
          addMemberToList(MemberList::decPriSlotMembers,md);
        }
        addMemberToList(MemberList::docPriSlotMembers,md);
      }
      break;
    case MemberDef::Event:       
      if (!docOnly)
      {
        addMemberToList(MemberList::decEventMembers,md);
      }
      addMemberToList(MemberList::docEventMembers,md);
      break;
    case MemberDef::Property:       
      if (!docOnly)
      {
        addMemberToList(MemberList::decPropMembers,md);
      }
      addMemberToList(MemberList::docPropMembers,md);
      break;
    case MemberDef::Friend:       
      if (!docOnly)
      {
        addMemberToList(MemberList::decFriendMembers,md);
      }
      addMemberToList(MemberList::docFriendMembers,md);
      break;
    default:
      err("GroupDef::insertMembers(): "
           "member `%s' (typeid=%d) with scope `%s' inserted in group scope `%s'!\n",
           md->name().data(),md->memberType(),
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

    removeMemberFromList(MemberList::allMembersList,md);
    switch(md->memberType())
    {
      case MemberDef::Variable:
	removeMemberFromList(MemberList::decVarMembers,md);
        removeMemberFromList(MemberList::docVarMembers,md);
        break;
      case MemberDef::Function: 
        removeMemberFromList(MemberList::decFuncMembers,md);
        removeMemberFromList(MemberList::docFuncMembers,md);
        break;
      case MemberDef::Typedef:      
        removeMemberFromList(MemberList::decTypedefMembers,md);
        removeMemberFromList(MemberList::docTypedefMembers,md);
        break;
      case MemberDef::Enumeration:  
        removeMemberFromList(MemberList::decEnumMembers,md);
        removeMemberFromList(MemberList::docEnumMembers,md);
        break;
      case MemberDef::EnumValue:    
        removeMemberFromList(MemberList::decEnumValMembers,md);
        removeMemberFromList(MemberList::docEnumValMembers,md);
        break;
      case MemberDef::Define:       
        removeMemberFromList(MemberList::decDefineMembers,md);
        removeMemberFromList(MemberList::docDefineMembers,md);
        break;
      case MemberDef::Signal:       
        removeMemberFromList(MemberList::decSignalMembers,md);
        removeMemberFromList(MemberList::docSignalMembers,md);
        break;
      case MemberDef::Slot:       
        if (md->protection()==Public)
        {
          removeMemberFromList(MemberList::decPubSlotMembers,md);
          removeMemberFromList(MemberList::docPubSlotMembers,md);
        }
        else if (md->protection()==Protected)
        {
          removeMemberFromList(MemberList::decProSlotMembers,md);
          removeMemberFromList(MemberList::docProSlotMembers,md);
        }
        else
        {
          removeMemberFromList(MemberList::decPriSlotMembers,md);
          removeMemberFromList(MemberList::docPriSlotMembers,md);
        }
        break;
      case MemberDef::Event:       
        removeMemberFromList(MemberList::decEventMembers,md);
        removeMemberFromList(MemberList::docEventMembers,md);
        break;
      case MemberDef::Property:       
        removeMemberFromList(MemberList::decPropMembers,md);
        removeMemberFromList(MemberList::docPropMembers,md);
        break;
      case MemberDef::Friend:       
        removeMemberFromList(MemberList::decFriendMembers,md);
        removeMemberFromList(MemberList::docFriendMembers,md);
        break;
      default:
        err("GroupDef::removeMember(): unexpected member remove in file!\n");
    }
  }
}

bool GroupDef::containsGroup(const GroupDef *def)
{
    return this==def || groupList->find(def) >= 0;
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
  LockingPtr<GroupList> groups = partOfGroups();
  return groups!=0 && groups->count()!=0;
}

int GroupDef::countMembers() const
{
  return fileList->count()+
         classSDict->count()+
         namespaceSDict->count()+
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

void GroupDef::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) 
      || !documentation().isEmpty() || !inbodyDocumentation().isEmpty()
     )
  {
    if (pageDict->count()!=countMembers()) // not only pages -> classical layout
    {
      ol.writeRuler();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
        ol.writeAnchor(0,"_details");
      ol.popGeneratorState();
      ol.startGroupHeader();
      ol.parseText(title);
      ol.endGroupHeader();
    }

    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
    }
    // write separator between brief and details
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF") &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.disable(OutputGenerator::RTF);
      // ol.newParagraph(); // FIXME:PARA
      ol.enableAll();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString("\n\n");
      ol.popGeneratorState();
    }

    // write detailed documentation
    if (!documentation().isEmpty())
    {
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }

    // write inbody documentation
    if (!inbodyDocumentation().isEmpty())
    {
      ol.parseDoc(inbodyFile(),inbodyLine(),this,0,inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }
}

void GroupDef::writeBriefDescription(OutputList &ol)
{
  if (!briefDescription().isEmpty())
  {
    ol.startParagraph();
    ol.parseDoc(briefFile(),briefLine(),this,0,
                briefDescription(),TRUE,FALSE,0,TRUE,FALSE);
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::RTF);
    ol.writeString(" \n");
    ol.enable(OutputGenerator::RTF);

    if (Config_getBool("REPEAT_BRIEF") ||
        !documentation().isEmpty()
       )
    {
      ol.disableAllBut(OutputGenerator::Html);
      ol.startTextLink(0,"_details");
      ol.parseText(theTranslator->trMore());
      ol.endTextLink();
    }
    ol.popGeneratorState();

    //ol.pushGeneratorState();
    //ol.disable(OutputGenerator::RTF);
    //ol.newParagraph();
    //ol.popGeneratorState();
    ol.endParagraph();
  }
}

void GroupDef::writeGroupGraph(OutputList &ol)
{
  if (Config_getBool("HAVE_DOT") /*&& Config_getBool("GROUP_GRAPHS")*/ )
  {
    DotGroupCollaboration graph(this);
    if (!graph.isTrivial())
    {
      msg("Generating dependency graph for group %s\n",qualifiedName().data());
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startParagraph();
      ol.startGroupCollaboration();
      ol.parseText(theTranslator->trCollaborationDiagram(title));
      ol.endGroupCollaboration(graph);
      ol.endParagraph();
      ol.popGeneratorState();
    }
  }
}

void GroupDef::writeFiles(OutputList &ol,const QCString &title)
{
  // write list of files
  if (fileList->count()>0)
  {
    ol.startMemberHeader("files");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    FileDef *fd=fileList->first();
    while (fd)
    {
      ol.startMemberItem(0);
      ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
      }
      ol.endMemberItem();
      if (!fd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startParagraph();
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.endParagraph();
      }
      fd=fileList->next();
    }
    ol.endMemberList();
  }
}

void GroupDef::writeNamespaces(OutputList &ol,const QCString &title)
{
  // write list of namespaces
  namespaceSDict->writeDeclaration(ol,title);
}

void GroupDef::writeNestedGroups(OutputList &ol,const QCString &title)
{
  // write list of groups
  if (groupList->count()>0)
  {
    ol.startMemberHeader("groups");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    GroupDef *gd=groupList->first();
    while (gd)
    {
      ol.startMemberItem(0);
      //ol.docify(theTranslator->trGroup(FALSE,TRUE));
      //ol.docify(" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),0,gd->groupTitle());
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <subgroup>" << convertToXML(gd->name()) << "</subgroup>" << endl;
      }
      ol.endMemberItem();
      if (!gd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startParagraph();
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),gd,0,gd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.endParagraph();
      }
      gd=groupList->next();
    }
    ol.endMemberList();
  }
}

void GroupDef::writeDirs(OutputList &ol,const QCString &title)
{
  // write list of directories
  if (dirList->count()>0)
  {
    ol.startMemberHeader("dirs");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    DirDef *dd=dirList->first();
    while (dd)
    {
      ol.startMemberItem(0);
      ol.parseText(theTranslator->trDir(FALSE,TRUE));
      ol.insertMemberAlign();
      ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),0,dd->shortName());
      ol.endMemberItem();
      if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
      {
        Doxygen::tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>" << endl;
      }
      if (!dd->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
      {
        ol.startParagraph();
        ol.startMemberDescription();
        ol.parseDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),FALSE,FALSE);
        ol.endMemberDescription();
        ol.endParagraph();
      }
      dd=dirList->next();
    }

    ol.endMemberList();
  }
}

void GroupDef::writeClasses(OutputList &ol,const QCString &title)
{
  // write list of classes
  classSDict->writeDeclaration(ol,0,title,FALSE);
}

void GroupDef::writePageDocumentation(OutputList &ol)
{
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
      ol.parseDoc(pd->docFile(),pd->docLine(),pd,0,pd->documentation()+pd->inbodyDocumentation(),TRUE,FALSE);
      ol.endTextBlock();
    }
  }
}

void GroupDef::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  if (memberGroupSDict)
  {
    /* write user defined member groups */
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writeDeclarations(ol,0,0,0,this);
    }
  }
}

void GroupDef::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void GroupDef::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void GroupDef::startMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void GroupDef::endMemberDocumentation(OutputList &ol)
{
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void GroupDef::writeAuthorSection(OutputList &ol)
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString("PROJECT_NAME")));
  ol.popGeneratorState();
}

void GroupDef::writeSummaryLinks(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Group));
  LayoutDocEntry *lde;
  bool first=TRUE;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    if ((lde->kind()==LayoutDocEntry::GroupClasses && classSDict->declVisible()) || 
        (lde->kind()==LayoutDocEntry::GroupNamespaces && namespaceSDict->declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupFiles && fileList->count()>0) ||
        (lde->kind()==LayoutDocEntry::GroupNestedGroups && groupList->count()>0) ||
        (lde->kind()==LayoutDocEntry::GroupDirs && dirList->count()>0)
       )
    {
      LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
      QCString label = lde->kind()==LayoutDocEntry::GroupClasses      ? "nested-classes" : 
                       lde->kind()==LayoutDocEntry::GroupNamespaces   ? "namespaces"     :
                       lde->kind()==LayoutDocEntry::GroupFiles        ? "files"          :
                       lde->kind()==LayoutDocEntry::GroupNestedGroups ? "groups"         :
                       "dirs";
      writeSummaryLink(ol,label,ls->title,first);
    }
    else if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      MemberList * ml = getMemberList(lmd->type);
      if (ml && ml->declVisible())
      {
        writeSummaryLink(ol,ml->listTypeAsString(),lmd->title,first);
      }
    }
  }
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void GroupDef::writeDocumentation(OutputList &ol)
{
  ol.pushGeneratorState();
  startFile(ol,getOutputFileBase(),name(),title);
  startTitle(ol,getOutputFileBase(),this);
  ol.parseText(title);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),title);
  ol.startContents();

  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(title,getOutputFileBase());
    static QRegExp we("[a-zA-Z_][-a-zA-Z_0-9]*");
    int i=0,p=0,l=0;
    while ((i=we.match(title,p,&l))!=-1) // foreach word in the title
    {
      Doxygen::searchIndex->addWord(title.mid(i,l),TRUE);
      p=i+l;
    }
  }

  Doxygen::indexList.addIndexItem(this,0,0,title);

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"group\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <title>" << convertToXML(title) << "</title>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }
  

  //---------------------------------------- start flexible part -------------------------------

  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Group));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc: 
        writeBriefDescription(ol);
        break; 
      case LayoutDocEntry::MemberDeclStart: 
        startMemberDeclarations(ol);
        break; 
      case LayoutDocEntry::GroupClasses: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeClasses(ol,ls->title);
        }
        break; 
      case LayoutDocEntry::GroupNamespaces: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNamespaces(ol,ls->title);
        }
        break; 
      case LayoutDocEntry::MemberGroups: 
        writeMemberGroups(ol);
        break; 
      case LayoutDocEntry::MemberDecl: 
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          writeMemberDeclarations(ol,lmd->type,lmd->title);
        }
        break; 
      case LayoutDocEntry::MemberDeclEnd: 
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDetailedDescription(ol,ls->title);
        }
        break;
      case LayoutDocEntry::MemberDefStart: 
        startMemberDocumentation(ol);
        break; 
      case LayoutDocEntry::MemberDef: 
        {
          LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
          writeMemberDocumentation(ol,lmd->type,lmd->title);
        }
        break;
      case LayoutDocEntry::MemberDefEnd: 
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::GroupNestedGroups: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNestedGroups(ol,ls->title);
        }
        break;
      case LayoutDocEntry::GroupPageDocs: 
        writePageDocumentation(ol);
        break;
      case LayoutDocEntry::GroupDirs: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDirs(ol,ls->title);
        }
        break;
      case LayoutDocEntry::GroupFiles: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeFiles(ol,ls->title);
        }
        break;
      case LayoutDocEntry::GroupGraph: 
        writeGroupGraph(ol);
        break;
      case LayoutDocEntry::AuthorSection: 
        writeAuthorSection(ol);
        break;
      case LayoutDocEntry::ClassIncludes:
      case LayoutDocEntry::ClassInheritanceGraph:
      case LayoutDocEntry::ClassNestedClasses:
      case LayoutDocEntry::ClassCollaborationGraph:
      case LayoutDocEntry::ClassAllMembersLink:
      case LayoutDocEntry::ClassUsedFiles:
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileNamespaces:
      case LayoutDocEntry::FileIncludes:
      case LayoutDocEntry::FileIncludeGraph:
      case LayoutDocEntry::FileIncludedByGraph: 
      case LayoutDocEntry::FileSourceLink:
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::Group entry list\n",lde->kind());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  endFile(ol); 
  ol.popGeneratorState();

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    allMemberList->sort();
    writeMemberPages(ol);
  }

}

void GroupDef::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberList::documentationLists)
    {
       ml->writeDocumentationPage(ol,name(),this);
    }
  }

  ol.popGeneratorState();
}

void GroupDef::writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const
{
  static bool createSubDirs=Config_getBool("CREATE_SUBDIRS");

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberListIterator mli(*allMemberList);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    if (md->getGroupDef()==this && md->isLinkable())
    {
      ol.writeString("          <tr><td class=\"navtab\">");
      if (md->isLinkableInProject())
      {
        if (md==currentMd) // selected item => highlight
        {
          ol.writeString("<a class=\"qindexHL\" ");
        }
        else
        {
          ol.writeString("<a class=\"qindex\" ");
        }
        ol.writeString("href=\"");
        if (createSubDirs) ol.writeString("../../");
        ol.writeString(md->getOutputFileBase()+Doxygen::htmlFileExtension+"#"+md->anchor());
        ol.writeString("\">");
        ol.writeString(md->localName());
        ol.writeString("</a>");
      }
      ol.writeString("</td></tr>\n");
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}



//---- helper functions ------------------------------------------------------

void addClassToGroups(Entry *root,ClassDef *cd)
{
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
    {
      if (gd->addClass(cd)) cd->makePartOfGroup(gd);
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
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
    {
      if (gd->addNamespace(nd)) nd->makePartOfGroup(gd);
      //printf("Namespace %s: in group %s\n",nd->name().data(),s->data());
    }
  }
}

void addDirToGroups(Entry *root,DirDef *dd)
{
  //printf("*** root->groups->count()=%d\n",root->groups->count());
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    //printf("group `%s'\n",g->groupname.data());
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
    {
      gd->addDir(dd);
      dd->makePartOfGroup(gd);
      //printf("Dir %s: in group %s\n",dd->name().data(),g->groupname.data());
    }
  }
}

void addGroupToGroups(Entry *root,GroupDef *subGroup)
{
  //printf("addGroupToGroups for %s groups=%d\n",root->name.data(),
  //    root->groups?root->groups->count():-1);
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    GroupDef *gd=0;
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)) &&
	!gd->containsGroup(subGroup) )
    {
      gd->addGroup(subGroup);
      subGroup->makePartOfGroup(gd);
    }
    else if (gd==subGroup)
    {
      warn(root->fileName,root->startLine,"Trying to add group %s to itself!",
          gd->name().data());
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
        (gd=Doxygen::groupSDict->find(g->groupname)) &&
	g->pri >= pri)
    {
      if (fgd && gd!=fgd && g->pri==pri) 
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
        md->setGroupDef(fgd,pri,root->fileName,root->startLine,
                        !root->doc.isEmpty());
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
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
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
  {
    LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
    addRefItem(xrefItems.pointer(),
             getOutputFileBase(),
             theTranslator->trGroup(TRUE,TRUE),
             getOutputFileBase(),name(),
             0
            );
  }
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->addListReferences(this);
  }
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberList::documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

MemberList *GroupDef::createMemberList(MemberList::ListType lt)
{
  m_memberLists.setAutoDelete(TRUE);
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
  }
  // not found, create a new member list
  ml = new MemberList(lt);
  m_memberLists.append(ml);
  ml->setInGroup(TRUE);
  return ml;
}

void GroupDef::addMemberToList(MemberList::ListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  static bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  MemberList *ml = createMemberList(lt);
  ml->setNeedsSorting(
      ((ml->listType()&MemberList::declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberList::documentationLists) && sortMemberDocs));
  ml->append(md);

#if 0  
  if (ml->needsSorting())
    ml->inSort(md);
  else
    ml->append(md);
#endif
}

void GroupDef::sortMemberLists()
{
  MemberList *ml = m_memberLists.first();
  while (ml)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
    ml = m_memberLists.next();
  }
}


MemberList *GroupDef::getMemberList(MemberList::ListType lt) const
{
  GroupDef *that = (GroupDef*)this;
  MemberList *ml = that->m_memberLists.first();
  while (ml)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
    ml = that->m_memberLists.next();
  }
  return 0;
}

void GroupDef::writeMemberDeclarations(OutputList &ol,MemberList::ListType lt,const QCString &title)
{
  static bool optimizeVhdl = Config_getBool("OPTIMIZE_OUTPUT_VHDL");

  MemberList * ml = getMemberList(lt);
  if (optimizeVhdl && ml) 
  {
    VhdlDocGen::writeVhdlDeclarations(ml,ol,this,0,0);
    return;
  }
  if (ml) 
  {
    ml->writeDeclarations(ol,0,0,0,this,title,0);
  }
}

void GroupDef::writeMemberDocumentation(OutputList &ol,MemberList::ListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

void GroupDef::removeMemberFromList(MemberList::ListType lt,MemberDef *md)
{
    MemberList *ml = getMemberList(lt);
    if (ml) ml->remove(md); 
}

