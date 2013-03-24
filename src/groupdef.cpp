/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include "arguments.h"
#include "entry.h"
#include "membername.h"
#include "dirdef.h"
#include "config.h"

//---------------------------------------------------------------------------

GroupDef::GroupDef(const char *df,int dl,const char *na,const char *t,
                   const char *refFileName) : Definition(df,dl,1,na)
{
  fileList = new FileList;
  classSDict = new ClassSDict(17);
  groupList = new GroupList;
  namespaceSDict = new NamespaceSDict(17);
  pageDict = new PageSDict(17);
  exampleDict = new PageSDict(17);
  dirList = new DirList;
  allMemberNameInfoSDict = new MemberNameInfoSDict(17);
  allMemberNameInfoSDict->setAutoDelete(TRUE);
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

  allMemberList = new MemberList(MemberListType_allMembersList);

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
  if ( t && qstrlen(t) )
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
    if (ml->listType()&MemberListType_declarationLists)
    {
      ml->findSectionsInDocumentation();
    }
  }
}

void GroupDef::addFile(const FileDef *def)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  if (def->isHidden()) return;
  updateLanguage(def);
  if (sortBriefDocs)
    fileList->inSort(def);
  else
    fileList->append(def);
}

bool GroupDef::addClass(const ClassDef *cd)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  if (cd->isHidden()) return FALSE;
  updateLanguage(cd);
  if (classSDict->find(cd->qualifiedName())==0)
  {
    QCString qn = cd->qualifiedName();
    //printf("--- addClass %s sort=%d\n",qn.data(),sortBriefDocs);
    if (sortBriefDocs)
    {
      classSDict->inSort(cd->qualifiedName(),cd);
    }
    else
    {
      int i=qn.findRev("::");
      if (i==-1) i=qn.find('.');
      bool found=FALSE;
      //printf("i=%d\n",i);
      if (i!=-1)
      {
        // add nested classes (e.g. A::B, A::C) after their parent (A) in 
        // order of insertion
        QCString scope = qn.left(i);
        int j=classSDict->findAt(scope);
        if (j!=-1)
        {
          while (j<(int)classSDict->count() && 
              classSDict->at(j)->qualifiedName().left(i)==scope)
          {
            //printf("skipping over %s\n",classSDict->at(j)->qualifiedName().data());
            j++;
          }
          //printf("Found scope at index %d\n",j);
          classSDict->insertAt(j,cd->qualifiedName(),cd);
          found=TRUE;
        }
      }
      if (!found) // no insertion point found -> just append
      {
        classSDict->append(cd->qualifiedName(),cd);
      }
    }
    return TRUE;
  }
  return FALSE;
}

bool GroupDef::addNamespace(const NamespaceDef *def)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  if (def->isHidden()) return FALSE;
  updateLanguage(def);
  if (namespaceSDict->find(def->name())==0)
  {
    if (sortBriefDocs)
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
    if (ml->listType()&MemberListType_declarationLists)
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
  updateLanguage(md);
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

      LockingPtr<ArgumentList> srcMdAl  = srcMd->argumentList();
      LockingPtr<ArgumentList> mdAl     = md->argumentList();
      LockingPtr<ArgumentList> tSrcMdAl = srcMd->templateArguments();
      LockingPtr<ArgumentList> tMdAl    = md->templateArguments();
      
      if (srcMd->isFunction() && md->isFunction() && // both are a function
          ((tSrcMdAl.pointer()==0 && tMdAl.pointer()==0) || 
           (tSrcMdAl.pointer()!=0 && tMdAl.pointer()!=0 && tSrcMdAl->count()==tMdAl->count())
          ) &&       // same number of template arguments
          matchArguments2(srcMd->getOuterScope(),srcMd->getFileDef(),srcMdAl.pointer(),
                          md->getOuterScope(),md->getFileDef(),mdAl.pointer(),
                          TRUE
                         ) && // matching parameters
          sameScope // both are found in the same scope
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
    case MemberType_Variable:     
      if (!docOnly)
      {
        addMemberToList(MemberListType_decVarMembers,md);
      }
      addMemberToList(MemberListType_docVarMembers,md);
      break;
    case MemberType_Function: 
      if (!docOnly)
      {
        addMemberToList(MemberListType_decFuncMembers,md);
      }
      addMemberToList(MemberListType_docFuncMembers,md);
      break;
    case MemberType_Typedef:      
      if (!docOnly)
      {
        addMemberToList(MemberListType_decTypedefMembers,md);
      }
      addMemberToList(MemberListType_docTypedefMembers,md);
      break;
    case MemberType_Enumeration:  
      if (!docOnly)
      {
        addMemberToList(MemberListType_decEnumMembers,md);
      }
      addMemberToList(MemberListType_docEnumMembers,md);
      break;
    case MemberType_EnumValue:    
      if (!docOnly)
      {
        addMemberToList(MemberListType_decEnumValMembers,md);
      }
      addMemberToList(MemberListType_docEnumValMembers,md);
      break;
    case MemberType_Define:       
      if (!docOnly)
      {
        addMemberToList(MemberListType_decDefineMembers,md);
      }
      addMemberToList(MemberListType_docDefineMembers,md);
      break;
    case MemberType_Signal:       
      if (!docOnly)
      {
        addMemberToList(MemberListType_decSignalMembers,md);
      }
      addMemberToList(MemberListType_docSignalMembers,md);
      break;
    case MemberType_Slot:       
      if (md->protection()==Public)
      {
        if (!docOnly)
        {
          addMemberToList(MemberListType_decPubSlotMembers,md);
        }
        addMemberToList(MemberListType_docPubSlotMembers,md);
      }
      else if (md->protection()==Protected)
      {
        if (!docOnly)
        {
          addMemberToList(MemberListType_decProSlotMembers,md);
        }
        addMemberToList(MemberListType_docProSlotMembers,md);
      }
      else
      {
        if (!docOnly)
        {
          addMemberToList(MemberListType_decPriSlotMembers,md);
        }
        addMemberToList(MemberListType_docPriSlotMembers,md);
      }
      break;
    case MemberType_Event:       
      if (!docOnly)
      {
        addMemberToList(MemberListType_decEventMembers,md);
      }
      addMemberToList(MemberListType_docEventMembers,md);
      break;
    case MemberType_Property:       
      if (!docOnly)
      {
        addMemberToList(MemberListType_decPropMembers,md);
      }
      addMemberToList(MemberListType_docPropMembers,md);
      break;
    case MemberType_Friend:       
      if (!docOnly)
      {
        addMemberToList(MemberListType_decFriendMembers,md);
      }
      addMemberToList(MemberListType_docFriendMembers,md);
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
    }

    removeMemberFromList(MemberListType_allMembersList,md);
    switch(md->memberType())
    {
      case MemberType_Variable:
	removeMemberFromList(MemberListType_decVarMembers,md);
        removeMemberFromList(MemberListType_docVarMembers,md);
        break;
      case MemberType_Function: 
        removeMemberFromList(MemberListType_decFuncMembers,md);
        removeMemberFromList(MemberListType_docFuncMembers,md);
        break;
      case MemberType_Typedef:      
        removeMemberFromList(MemberListType_decTypedefMembers,md);
        removeMemberFromList(MemberListType_docTypedefMembers,md);
        break;
      case MemberType_Enumeration:  
        removeMemberFromList(MemberListType_decEnumMembers,md);
        removeMemberFromList(MemberListType_docEnumMembers,md);
        break;
      case MemberType_EnumValue:    
        removeMemberFromList(MemberListType_decEnumValMembers,md);
        removeMemberFromList(MemberListType_docEnumValMembers,md);
        break;
      case MemberType_Define:       
        removeMemberFromList(MemberListType_decDefineMembers,md);
        removeMemberFromList(MemberListType_docDefineMembers,md);
        break;
      case MemberType_Signal:       
        removeMemberFromList(MemberListType_decSignalMembers,md);
        removeMemberFromList(MemberListType_docSignalMembers,md);
        break;
      case MemberType_Slot:       
        if (md->protection()==Public)
        {
          removeMemberFromList(MemberListType_decPubSlotMembers,md);
          removeMemberFromList(MemberListType_docPubSlotMembers,md);
        }
        else if (md->protection()==Protected)
        {
          removeMemberFromList(MemberListType_decProSlotMembers,md);
          removeMemberFromList(MemberListType_docProSlotMembers,md);
        }
        else
        {
          removeMemberFromList(MemberListType_decPriSlotMembers,md);
          removeMemberFromList(MemberListType_docPriSlotMembers,md);
        }
        break;
      case MemberType_Event:       
        removeMemberFromList(MemberListType_decEventMembers,md);
        removeMemberFromList(MemberListType_docEventMembers,md);
        break;
      case MemberType_Property:       
        removeMemberFromList(MemberListType_decPropMembers,md);
        removeMemberFromList(MemberListType_docPropMembers,md);
        break;
      case MemberType_Friend:       
        removeMemberFromList(MemberListType_decFriendMembers,md);
        removeMemberFromList(MemberListType_docFriendMembers,md);
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
  setAnchors(allMemberList);
}

void GroupDef::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) 
      || !documentation().isEmpty() || !inbodyDocumentation().isEmpty()
     )
  {
    if (pageDict->count()!=countMembers()) // not only pages -> classical layout
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Html);
        ol.writeRuler();
      ol.popGeneratorState();
      ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        ol.writeAnchor(0,"details");
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
  if (!briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
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
      ol.startTextLink(0,"details");
      ol.parseText(theTranslator->trMore());
      ol.endTextLink();
    }
    ol.popGeneratorState();
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
      //ol.startParagraph();
      ol.startGroupCollaboration();
      ol.parseText(theTranslator->trCollaborationDiagram(title));
      ol.endGroupCollaboration(graph);
      //ol.endParagraph();
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
      ol.startMemberDeclaration();
      ol.startMemberItem(fd->getOutputFileBase(),0);
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
        ol.startMemberDescription(fd->getOutputFileBase());
        ol.parseDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
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
  int count=0;
  if (groupList->count()>0)
  {
    GroupDef *gd=groupList->first();
    while (gd)
    {
      if (gd->isVisible()) count++;
      gd=groupList->next();
    }
  }
  if (count>0)
  {
    ol.startMemberHeader("groups");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    if (Config_getBool("SORT_GROUP_NAMES"))
    {
      groupList->sort();
    }
    GroupDef *gd=groupList->first();
    while (gd)
    {
      if (gd->isVisible())
      {
        ol.startMemberDeclaration();
        ol.startMemberItem(gd->getOutputFileBase(),0);
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
          ol.startMemberDescription(gd->getOutputFileBase());
          ol.parseDoc(briefFile(),briefLine(),gd,0,gd->briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(0,0);
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
      ol.startMemberDeclaration();
      ol.startMemberItem(dd->getOutputFileBase(),0);
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
        ol.startMemberDescription(dd->getOutputFileBase());
        ol.parseDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
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

void GroupDef::writeInlineClasses(OutputList &ol)
{
  classSDict->writeDocumentation(ol);
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
          (si=Doxygen::sectionDict->find(pd->name()))!=0)
      {
        ol.startSection(si->label,si->title,SectionInfo::Subsection);
        ol.docify(si->title);
        ol.endSection(si->label,SectionInfo::Subsection);
      }
      ol.startTextBlock();
      ol.parseDoc(pd->docFile(),pd->docLine(),pd,0,pd->documentation()+pd->inbodyDocumentation(),TRUE,FALSE,0,TRUE,FALSE);
      ol.endTextBlock();
    }
  }
}

void GroupDef::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  if (memberGroupSDict)
  {
    memberGroupSDict->sort();
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
  //printf("** GroupDef::startMemberDocumentation()\n");
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void GroupDef::endMemberDocumentation(OutputList &ol)
{
  //printf("** GroupDef::endMemberDocumentation()\n");
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.popGeneratorState();
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
  SrcLangExt lang = getLanguage();
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
      ol.writeSummaryLink(0,label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      MemberList * ml = getMemberList(lmd->type);
      if (ml && ml->declVisible())
      {
        ol.writeSummaryLink(0,ml->listTypeAsString(),lmd->title(lang),first);
        first=FALSE;
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
  //static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  ol.pushGeneratorState();
  startFile(ol,getOutputFileBase(),name(),title,HLI_None);

  ol.startHeaderSection();
  writeSummaryLinks(ol);
  ol.startTitleHead(getOutputFileBase());
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.parseText(title);
  ol.popGeneratorState();
  addGroupListToTitle(ol,this);
  ol.endTitleHead(getOutputFileBase(),title);
  ol.endHeaderSection();
  ol.startContents();

  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    static QRegExp we("[a-zA-Z_][-a-zA-Z_0-9]*");
    int i=0,p=0,l=0;
    while ((i=we.match(title,p,&l))!=-1) // foreach word in the title
    {
      Doxygen::searchIndex->addWord(title.mid(i,l),TRUE);
      p=i+l;
    }
  }

  Doxygen::indexList->addIndexItem(this,0,0,title);

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"group\">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <title>" << convertToXML(title) << "</title>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  }
  

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang=getLanguage();
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
          writeClasses(ol,ls->title(lang));
        }
        break; 
      case LayoutDocEntry::GroupInlineClasses: 
        {
          writeInlineClasses(ol);
        }
        break;
      case LayoutDocEntry::GroupNamespaces: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNamespaces(ol,ls->title(lang));
        }
        break; 
      case LayoutDocEntry::MemberGroups: 
        writeMemberGroups(ol);
        break; 
      case LayoutDocEntry::MemberDecl: 
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
        }
        break; 
      case LayoutDocEntry::MemberDeclEnd: 
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDetailedDescription(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefStart: 
        startMemberDocumentation(ol);
        break; 
      case LayoutDocEntry::MemberDef: 
        {
          LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
          writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefEnd: 
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::GroupNestedGroups: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNestedGroups(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::GroupPageDocs: 
        writePageDocumentation(ol);
        break;
      case LayoutDocEntry::GroupDirs: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDirs(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::GroupFiles: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeFiles(ol,ls->title(lang));
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
      case LayoutDocEntry::ClassInlineClasses:
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileNamespaces:
      case LayoutDocEntry::FileIncludes:
      case LayoutDocEntry::FileIncludeGraph:
      case LayoutDocEntry::FileIncludedByGraph: 
      case LayoutDocEntry::FileSourceLink:
      case LayoutDocEntry::FileInlineClasses:
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
    if (ml->listType()&MemberListType_documentationLists)
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
        ol.writeString(convertToHtml(md->localName()));
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
      if (gd->addClass(cd)) 
      {
        cd->makePartOfGroup(gd);
      }
      //printf("Compound %s: in group %s\n",cd->name().data(),gd->groupTitle());
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
            "warning: Member %s found in multiple %s groups! "
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
                "warning: Member documentation for %s found several times in %s groups!\n"
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
      //printf("insertMember found at %s line %d: %s: related %s\n",
      //    md->getDefFileName().data(),md->getDefLine(),
      //    md->name().data(),root->relates.data());
      bool success = fgd->insertMember(md);
      if (success)
      {
        //printf("insertMember successful\n");
        md->setGroupDef(fgd,pri,root->fileName,root->startLine,
            !root->doc.isEmpty());
        ClassDef *cd = md->getClassDefOfAnonymousType();
        if (cd) 
        {
          cd->setGroupDefForAllMembers(fgd,pri,root->fileName,root->startLine,root->doc.length() != 0);
        }
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
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

MemberList *GroupDef::createMemberList(MemberListType lt)
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

void GroupDef::addMemberToList(MemberListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  static bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  MemberList *ml = createMemberList(lt);
  ml->setNeedsSorting(
      ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
  ml->append(md);
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


MemberList *GroupDef::getMemberList(MemberListType lt) const
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

void GroupDef::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  static bool optimizeVhdl = Config_getBool("OPTIMIZE_OUTPUT_VHDL");

  MemberList * ml = getMemberList(lt);
  if (optimizeVhdl && ml) 
  {
    VhdlDocGen::writeVhdlDeclarations(ml,ol,this,0,0,0);
    return;
  }
  if (ml) 
  {
    ml->writeDeclarations(ol,0,0,0,this,title,0,definitionType());
  }
}

void GroupDef::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

void GroupDef::removeMemberFromList(MemberListType lt,MemberDef *md)
{
    MemberList *ml = getMemberList(lt);
    if (ml) ml->remove(md); 
}

void GroupDef::sortSubGroups() 
{ 
    groupList->sort(); 
}

bool GroupDef::isLinkableInProject() const
{
  return !isReference() && isLinkable();
}

bool GroupDef::isLinkable() const
{
  return hasUserDocumentation();
}

// let the "programming language" for a group depend on what is inserted into it.
// First item that has an associated languages determines the language for the whole group.
void GroupDef::updateLanguage(const Definition *d)
{
  if (getLanguage()==SrcLangExt_Unknown && d->getLanguage()!=SrcLangExt_Unknown)
  {
    setLanguage(d->getLanguage());
  }
}

