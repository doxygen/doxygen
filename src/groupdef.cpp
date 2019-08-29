/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
#include "dotgroupcollaboration.h"
#include "vhdldocgen.h"
#include "layout.h"
#include "arguments.h"
#include "entry.h"
#include "membername.h"
#include "dirdef.h"
#include "config.h"
#include "definitionimpl.h"

//---------------------------------------------------------------------------

class GroupDefImpl : public DefinitionImpl, public GroupDef
{
  public:
    GroupDefImpl(const char *fileName,int line,const char *name,const char *title,const char *refFileName=0);
    virtual ~GroupDefImpl();

    virtual DefType definitionType() const { return TypeGroup; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual QCString displayName(bool=TRUE) const { return hasGroupTitle() ? title : DefinitionImpl::name(); }
    virtual const char *groupTitle() const { return title; }
    virtual void setGroupTitle( const char *newtitle );
    virtual bool hasGroupTitle( ) const { return titleSet; }
    virtual void addFile(const FileDef *def); 
    virtual bool addClass(const ClassDef *def);
    virtual bool addNamespace(const NamespaceDef *def);
    virtual void addGroup(const GroupDef *def);
    virtual void addPage(PageDef *def);
    virtual void addExample(const PageDef *def);
    virtual void addDir(const DirDef *dd);
    virtual bool insertMember(MemberDef *def,bool docOnly=FALSE);
    virtual void removeMember(MemberDef *md);
    virtual bool findGroup(const GroupDef *def) const; // true if def is a subgroup of this group
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeMemberPages(OutputList &ol);
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const;
    virtual void writeTagFile(FTextStream &);
    virtual int  numDocMembers() const;
    virtual bool isLinkableInProject() const;
    virtual bool isLinkable() const;
    virtual bool isASubGroup() const;
    virtual void computeAnchors();
    virtual void countMembers();

    virtual void addMembersToMemberGroup();
    virtual void distributeMemberGroupDocumentation();
    virtual void findSectionsInDocumentation();

    virtual void addListReferences();
    virtual void sortMemberLists();
    virtual bool subGrouping() const { return m_subGrouping; }

    virtual void setGroupScope(Definition *d) { groupScope = d; }
    virtual Definition *getGroupScope() const { return groupScope; }

    virtual MemberList *getMemberList(MemberListType lt) const;
    virtual const QList<MemberList> &getMemberLists() const { return m_memberLists; }

    /* user defined member groups */
    virtual MemberGroupSDict *getMemberGroupSDict() const { return memberGroupSDict; }

    virtual FileList *      getFiles() const        { return fileList; }
    virtual ClassSDict *    getClasses() const      { return classSDict; }
    virtual NamespaceSDict * getNamespaces() const   { return namespaceSDict; }
    virtual GroupList *     getSubGroups() const    { return groupList; }
    virtual PageSDict *     getPages() const        { return pageDict; }
    virtual DirList *       getDirs() const         { return dirList; }
    virtual PageSDict *     getExamples() const     { return exampleDict; }
    virtual bool hasDetailedDescription() const;
    virtual void sortSubGroups();
    
  private: 
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);
    MemberList *createMemberList(MemberListType lt);
    void addMemberToList(MemberListType lt,MemberDef *md);
    void writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title);
    void writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title);
    void removeMemberFromList(MemberListType lt,MemberDef *md);
    void writeGroupGraph(OutputList &ol);
    void writeFiles(OutputList &ol,const QCString &title);
    void writeNamespaces(OutputList &ol,const QCString &title);
    void writeNestedGroups(OutputList &ol,const QCString &title);
    void writeDirs(OutputList &ol,const QCString &title);
    void writeClasses(OutputList &ol,const QCString &title);
    void writeInlineClasses(OutputList &ol);
    void writePageDocumentation(OutputList &ol);
    void writeDetailedDescription(OutputList &ol,const QCString &title);
    void writeBriefDescription(OutputList &ol);
    void writeMemberGroups(OutputList &ol);
    void startMemberDeclarations(OutputList &ol);
    void endMemberDeclarations(OutputList &ol);
    void startMemberDocumentation(OutputList &ol);
    void endMemberDocumentation(OutputList &ol);
    void writeAuthorSection(OutputList &ol);
    void writeSummaryLinks(OutputList &ol) const;
    void updateLanguage(const Definition *);

    QCString title;                      // title of the group
    bool titleSet;                       // true if title is not the same as the name
    QCString fileName;                   // base name of the generated file
    FileList *fileList;                  // list of files in the group
    ClassSDict *classSDict;              // list of classes in the group
    NamespaceSDict *namespaceSDict;      // list of namespaces in the group
    GroupList *groupList;                // list of sub groups.
    PageSDict *pageDict;                 // list of pages in the group
    PageSDict *exampleDict;              // list of examples in the group
    DirList *dirList;                    // list of directories in the group

    MemberList *allMemberList;
    MemberNameInfoSDict *allMemberNameInfoSDict;
    
    Definition *groupScope;

    QList<MemberList> m_memberLists;
    MemberGroupSDict *memberGroupSDict;
    bool              m_subGrouping;

};

GroupDef *createGroupDef(const char *fileName,int line,const char *name,
                                const char *title,const char *refFileName)
{
  return new GroupDefImpl(fileName,line,name,title,refFileName);
}


//---------------------------------------------------------------------------

GroupDefImpl::GroupDefImpl(const char *df,int dl,const char *na,const char *t,
                   const char *refFileName) : DefinitionImpl(df,dl,1,na)
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
    fileName = convertNameToFile(QCString("group_")+na);
  }
  setGroupTitle( t );
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);

  allMemberList = new MemberList(MemberListType_allMembersList);

  //visited = 0;
  groupScope = 0;
  m_subGrouping=Config_getBool(SUBGROUPING);
}

GroupDefImpl::~GroupDefImpl()
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

void GroupDefImpl::setGroupTitle( const char *t )
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


void GroupDefImpl::distributeMemberGroupDocumentation()
{
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void GroupDefImpl::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,docFile());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->findSectionsInDocumentation(this);
  }

  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}

void GroupDefImpl::addFile(const FileDef *def)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  if (def->isHidden()) return;
  updateLanguage(def);
  if (sortBriefDocs)
    fileList->inSort(def);
  else
    fileList->append(def);
}

bool GroupDefImpl::addClass(const ClassDef *cd)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  if (cd->isHidden()) return FALSE;
  updateLanguage(cd);
  QCString qn = cd->name();
  if (classSDict->find(qn)==0)
  {
    //printf("--- addClass %s sort=%d\n",qn.data(),sortBriefDocs);
    if (sortBriefDocs)
    {
      classSDict->inSort(qn,cd);
    }
    else
    {
      int i=qn.findRev("::");
      if (i==-1) i=qn.find('.');
      bool found=FALSE;
      //printf("i=%d\n",i);
      if (i>0)
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
          classSDict->insertAt(j,qn,cd);
          found=TRUE;
        }
      }
      if (!found) // no insertion point found -> just append
      {
        classSDict->append(qn,cd);
      }
    }
    return TRUE;
  }
  return FALSE;
}

bool GroupDefImpl::addNamespace(const NamespaceDef *def)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
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

void GroupDefImpl::addDir(const DirDef *def)
{
  if (def->isHidden()) return;
  if (Config_getBool(SORT_BRIEF_DOCS))
    dirList->inSort(def);  
  else
    dirList->append(def);
}

void GroupDefImpl::addPage(PageDef *def)
{
  if (def->isHidden()) return;
  //printf("Making page %s part of a group\n",def->name.data());
  pageDict->append(def->name(),def);
  def->makePartOfGroup(this);
}

void GroupDefImpl::addExample(const PageDef *def)
{
  if (def->isHidden()) return;
  exampleDict->append(def->name(),def);
}


void GroupDefImpl::addMembersToMemberGroup()
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

  //printf("GroupDefImpl::addMembersToMemberGroup() memberGroupList=%d\n",memberGroupList->count());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->setInGroup(TRUE);
  }
}


bool GroupDefImpl::insertMember(MemberDef *md,bool docOnly)
{
  if (md->isHidden()) return FALSE;
  updateLanguage(md);
  //printf("GroupDef(%s)::insertMember(%s)\n", title.data(), md->name().data());
  MemberNameInfo *mni=0;
  if ((mni=(*allMemberNameInfoSDict)[md->name()]))
  { // member with this name already found
    MemberNameInfoIterator srcMnii(*mni); 
    const MemberInfo *srcMi;
    for ( ; (srcMi=srcMnii.current()) ; ++srcMnii )
    {
      const MemberDef *srcMd = srcMi->memberDef;
      if (srcMd==md) return FALSE; // already added before!

      bool sameScope = srcMd->getOuterScope()==md->getOuterScope() || // same class or namespace
          // both inside a file => definition and declaration do not have to be in the same file
           (srcMd->getOuterScope()->definitionType()==Definition::TypeFile &&
               md->getOuterScope()->definitionType()==Definition::TypeFile); 

      const ArgumentList *srcMdAl  = srcMd->argumentList();
      const ArgumentList *mdAl     = md->argumentList();
      const ArgumentList *tSrcMdAl = srcMd->templateArguments();
      const ArgumentList *tMdAl    = md->templateArguments();
      
      if (srcMd->isFunction() && md->isFunction() && // both are a function
          ((tSrcMdAl==0 && tMdAl==0) || 
           (tSrcMdAl!=0 && tMdAl!=0 && tSrcMdAl->count()==tMdAl->count())
          ) &&       // same number of template arguments
          matchArguments2(srcMd->getOuterScope(),srcMd->getFileDef(),srcMdAl,
                          md->getOuterScope(),md->getFileDef(),mdAl,
                          TRUE
                         ) && // matching parameters
          sameScope // both are found in the same scope
         )
      {
        if (srcMd->getGroupAlias()==0) 
        {
          md->setGroupAlias(srcMd); 
        }
        else if (md!=srcMd->getGroupAlias())
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
      err("GroupDefImpl::insertMembers(): "
           "member '%s' (typeid=%d) with scope '%s' inserted in group scope '%s'!\n",
           md->name().data(),md->memberType(),
           md->getClassDef() ? md->getClassDef()->name().data() : "",
           name().data());
  }
  return TRUE;
}

void GroupDefImpl::removeMember(MemberDef *md)
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
        err("GroupDefImpl::removeMember(): unexpected member remove in file!\n");
    }
  }
}

bool GroupDefImpl::findGroup(const GroupDef *def) const
{
  if (this==def)
  {
    return TRUE;
  }
  else if (groupList)
  {
    GroupListIterator it(*groupList);
    GroupDef *gd;
    for (;(gd=it.current());++it)
    {
      if (gd->findGroup(def))
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

void GroupDefImpl::addGroup(const GroupDef *def)
{
  //printf("adding group '%s' to group '%s'\n",def->name().data(),name().data());
  //if (Config_getBool(SORT_MEMBER_DOCS))
  //  groupList->inSort(def);
  //else
  groupList->append(def);
}

bool GroupDefImpl::isASubGroup() const
{
  GroupList *groups = partOfGroups();
  return groups!=0 && groups->count()!=0;
}

void GroupDefImpl::countMembers()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (;(ml=mli.current());++mli)
  {
    ml->countDecMembers();
    ml->countDocMembers();
  }
  if (memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->countDecMembers();
      mg->countDocMembers();
    }
  }
}

int GroupDefImpl::numDocMembers() const
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
void GroupDefImpl::computeAnchors()
{
  //printf("GroupDefImpl::computeAnchors()\n");
  setAnchors(allMemberList);
}

void GroupDefImpl::writeTagFile(FTextStream &tagFile)
{
  tagFile << "  <compound kind=\"group\">" << endl;
  tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "    <title>" << convertToXML(title) << "</title>" << endl;
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Group));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::GroupClasses:
        {
          if (classSDict)
          {
            SDict<ClassDef>::Iterator ci(*classSDict);
            ClassDef *cd;
            for (ci.toFirst();(cd=ci.current());++ci)
            {
              if (cd->isLinkableInProject())
              {
                tagFile << "    <class kind=\"" << cd->compoundTypeString()
                        << "\">" << convertToXML(cd->name()) << "</class>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::GroupNamespaces:
        {
          if (namespaceSDict)
          {
            SDict<NamespaceDef>::Iterator ni(*namespaceSDict);
            NamespaceDef *nd;
            for (ni.toFirst();(nd=ni.current());++ni)
            {
              if (nd->isLinkableInProject())
              {
                tagFile << "    <namespace>" << convertToXML(nd->name())
                        << "</namespace>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::GroupFiles:
        {
          if (fileList)
          {
            QListIterator<FileDef> it(*fileList);
            FileDef *fd;
            for (;(fd=it.current());++it)
            {
              if (fd->isLinkableInProject())
              {
                tagFile << "    <file>" << convertToXML(fd->name()) << "</file>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::GroupPageDocs:
        {
          if (pageDict)
          {
            PageSDict::Iterator pdi(*pageDict);
            PageDef *pd=0;
            for (pdi.toFirst();(pd=pdi.current());++pdi)
            {
              QCString pageName = pd->getOutputFileBase();
              if (pd->isLinkableInProject())
              {
                tagFile << "    <page>" << convertToXML(pageName) << "</page>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::GroupDirs:
        {
          if (dirList)
          {
            QListIterator<DirDef> it(*dirList);
            DirDef *dd;
            for (;(dd=it.current());++it)
            {
              if (dd->isLinkableInProject())
              {
                tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::GroupNestedGroups:
        {
          if (groupList)
          {
            QListIterator<GroupDef> it(*groupList);
            GroupDef *gd;
            for (;(gd=it.current());++it)
            {
              if (gd->isVisible())
              {
                tagFile << "    <subgroup>" << convertToXML(gd->name()) << "</subgroup>" << endl;
              }
            }
          }
        }
        break;
      case LayoutDocEntry::MemberDecl:
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          MemberList * ml = getMemberList(lmd->type);
          if (ml)
          {
            ml->writeTagFile(tagFile);
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        {
          if (memberGroupSDict)
          {
            MemberGroupSDict::Iterator mgli(*memberGroupSDict);
            MemberGroup *mg;
            for (;(mg=mgli.current());++mgli)
            {
              mg->writeTagFile(tagFile);
            }
          }
        }
        break;
      default:
        break;
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>" << endl;
}

void GroupDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if ((!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF)) 
      || !documentation().isEmpty() || !inbodyDocumentation().isEmpty()
     )
  {
    ol.pushGeneratorState();
    if (pageDict->count()!=numDocMembers()) // not only pages -> classical layout
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Html);
        ol.writeRuler();
      ol.popGeneratorState();
      ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        ol.writeAnchor(0,"details");
      ol.popGeneratorState();
    }
    else
    {
      ol.disableAllBut(OutputGenerator::Man); // always print title for man page
    }
    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();
    ol.popGeneratorState();

    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
    }
    // write separator between brief and details
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.disable(OutputGenerator::RTF);
      // ol.newParagraph(); // FIXME:PARA
      ol.enableAll();
      ol.disableAllBut(OutputGenerator::Man);
      ol.enable(OutputGenerator::Latex);
      ol.writeString("\n\n");
      ol.popGeneratorState();
    }

    // write detailed documentation
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }

    // write inbody documentation
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),this,0,inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }
}

void GroupDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),this,0,
                                briefDescription(),TRUE,FALSE,0,TRUE,FALSE);
    if (rootNode && !rootNode->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(rootNode,this,0);
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.writeString(" \n");
      ol.enable(OutputGenerator::RTF);

      if (Config_getBool(REPEAT_BRIEF) ||
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
    delete rootNode;
  }
  ol.writeSynopsis();
}

void GroupDefImpl::writeGroupGraph(OutputList &ol)
{
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(GROUP_GRAPHS)*/ )
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

void GroupDefImpl::writeFiles(OutputList &ol,const QCString &title)
{
  // write list of files
  if (fileList->count()>0)
  {
    ol.startMemberHeader("files");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    QListIterator<FileDef> it(*fileList);
    FileDef *fd;
    for (;(fd=it.current());++it)
    {
      if (!fd->hasDocumentation()) continue;
      ol.startMemberDeclaration();
      ol.startMemberItem(fd->getOutputFileBase(),0);
      ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,fd->name());
      ol.endMemberItem();
      if (!fd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(fd->getOutputFileBase());
        ol.generateDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
    }
    ol.endMemberList();
  }
}

void GroupDefImpl::writeNamespaces(OutputList &ol,const QCString &title)
{
  // write list of namespaces
  namespaceSDict->writeDeclaration(ol,title);
}

void GroupDefImpl::writeNestedGroups(OutputList &ol,const QCString &title)
{
  // write list of groups
  int count=0;
  if (groupList->count()>0)
  {
    QListIterator<GroupDef> it(*groupList);
    GroupDef *gd;
    for (;(gd=it.current());++it)
    {
      if (gd->isVisible()) count++;
    }
  }
  if (count>0)
  {
    ol.startMemberHeader("groups");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    if (Config_getBool(SORT_GROUP_NAMES))
    {
      groupList->sort();
    }
    QListIterator<GroupDef> it(*groupList);
    GroupDef *gd;
    for (;(gd=it.current());++it)
    {
      if (gd->isVisible())
      {
        if (!gd->hasDocumentation()) continue;
        ol.startMemberDeclaration();
        ol.startMemberItem(gd->getOutputFileBase(),0);
        //ol.docify(theTranslator->trGroup(FALSE,TRUE));
        //ol.docify(" ");
        ol.insertMemberAlign();
        ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),0,gd->groupTitle());
        ol.endMemberItem();
        if (!gd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(gd->getOutputFileBase());
          ol.generateDoc(briefFile(),briefLine(),gd,0,gd->briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(0,0);
      }
    }
    ol.endMemberList();
  }
}

void GroupDefImpl::writeDirs(OutputList &ol,const QCString &title)
{
  // write list of directories
  if (dirList->count()>0)
  {
    ol.startMemberHeader("dirs");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    QListIterator<DirDef> it(*dirList);
    DirDef *dd;
    for (;(dd=it.current());++it)
    {
      if (!dd->hasDocumentation()) continue;
      ol.startMemberDeclaration();
      ol.startMemberItem(dd->getOutputFileBase(),0);
      ol.parseText(theTranslator->trDir(FALSE,TRUE));
      ol.insertMemberAlign();
      ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),0,dd->shortName());
      ol.endMemberItem();
      if (!dd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(dd->getOutputFileBase());
        ol.generateDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(0,0);
    }

    ol.endMemberList();
  }
}

void GroupDefImpl::writeClasses(OutputList &ol,const QCString &title)
{
  // write list of classes
  classSDict->writeDeclaration(ol,0,title,FALSE);
}

void GroupDefImpl::writeInlineClasses(OutputList &ol)
{
  classSDict->writeDocumentation(ol);
}

void GroupDefImpl::writePageDocumentation(OutputList &ol)
{
  PageDef *pd=0;
  PageSDict::Iterator pdi(*pageDict);
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->isReference())
    {
      SectionInfo *si=0;
      if (pd->hasTitle() && !pd->name().isEmpty() &&
          (si=Doxygen::sectionDict->find(pd->name()))!=0)
      {
        ol.startSection(si->label,si->title,SectionInfo::Subsection);
        ol.docify(si->title);
        ol.endSection(si->label,SectionInfo::Subsection);
      }
      ol.startTextBlock();
      ol.generateDoc(pd->docFile(),pd->docLine(),pd,0,pd->documentation()+pd->inbodyDocumentation(),TRUE,FALSE,0,TRUE,FALSE);
      ol.endTextBlock();
    }
  }
}

void GroupDefImpl::writeMemberGroups(OutputList &ol)
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

void GroupDefImpl::startMemberDeclarations(OutputList &ol)
{
  ol.startMemberSections();
}

void GroupDefImpl::endMemberDeclarations(OutputList &ol)
{
  ol.endMemberSections();
}

void GroupDefImpl::startMemberDocumentation(OutputList &ol)
{
  //printf("** GroupDefImpl::startMemberDocumentation()\n");
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void GroupDefImpl::endMemberDocumentation(OutputList &ol)
{
  //printf("** GroupDefImpl::endMemberDocumentation()\n");
  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    ol.popGeneratorState();
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void GroupDefImpl::writeAuthorSection(OutputList &ol)
{
  // write Author section (Man only)
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void GroupDefImpl::writeSummaryLinks(OutputList &ol) const
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
        ol.writeSummaryLink(0,MemberList::listTypeAsString(ml->listType()),lmd->title(lang),first);
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

void GroupDefImpl::writeDocumentation(OutputList &ol)
{
  //static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  ol.pushGeneratorState();
  startFile(ol,getOutputFileBase(),name(),title,HLI_Modules);

  ol.startHeaderSection();
  writeSummaryLinks(ol);
  ol.startTitleHead(getOutputFileBase());
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.parseText(title);
  ol.popGeneratorState();
  addGroupListToTitle(ol,this);
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.endTitleHead(getOutputFileBase(),title);
  ol.popGeneratorState();
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.endTitleHead(getOutputFileBase(),name());
  ol.popGeneratorState();
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
      case LayoutDocEntry::NamespaceNestedConstantGroups:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceInterfaces:
      case LayoutDocEntry::NamespaceStructs:
      case LayoutDocEntry::NamespaceExceptions:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileInterfaces:
      case LayoutDocEntry::FileStructs:
      case LayoutDocEntry::FileExceptions:
      case LayoutDocEntry::FileNamespaces:
      case LayoutDocEntry::FileConstantGroups:
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

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    allMemberList->sort();
    writeMemberPages(ol);
  }

}

void GroupDefImpl::writeMemberPages(OutputList &ol)
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

void GroupDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  static bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  MemberListIterator mli(*allMemberList);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    if (md->getGroupDef()==this && md->isLinkable() && !md->isEnumValue())
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
    //printf("group '%s'\n",s->data());
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
    //printf("group '%s'\n",g->groupname.data());
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
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
    {
      if (gd==subGroup)
      {
        warn(root->fileName,root->startLine,"Refusing to add group %s to itself",
            gd->name().data());
      }
      else if (subGroup->findGroup(gd))
      {
        warn(root->fileName,root->startLine,"Refusing to add group %s to group %s, since the latter is already a "
                                            "subgroup of the former\n", subGroup->name().data(),gd->name().data());
      }
      else if (!gd->findGroup(subGroup))
      {
        gd->addGroup(subGroup);
        subGroup->makePartOfGroup(gd);
      }
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
            "Member %s found in multiple %s groups! "
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
    GroupDef *mgd = const_cast<GroupDef*>(md->getGroupDef());
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
                "Member documentation for %s found several times in %s groups!\n"
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

QCString GroupDefImpl::getOutputFileBase() const
{
  return fileName;
}

void GroupDefImpl::addListReferences()
{
  {
    QList<ListItemInfo> *xrefItems = xrefListItems();
    addRefItem(xrefItems,
             getOutputFileBase(),
             theTranslator->trGroup(TRUE,TRUE),
             getOutputFileBase(),name(),
             0,
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

MemberList *GroupDefImpl::createMemberList(MemberListType lt)
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

void GroupDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  static bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  MemberList *ml = createMemberList(lt);
  ml->setNeedsSorting(
      ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
  ml->append(md);
}

void GroupDefImpl::sortMemberLists()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (;(ml=mli.current());++mli)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }
}

MemberList *GroupDefImpl::getMemberList(MemberListType lt) const
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (;(ml=mli.current());++mli)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
  }
  return 0;
}

void GroupDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  static bool optimizeVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);

  MemberList * ml = getMemberList(lt);
  if (optimizeVhdl && ml) 
  {
    VhdlDocGen::writeVhdlDeclarations(ml,ol,this,0,0,0);
    return;
  }
  if (ml) 
  {
    ml->writeDeclarations(ol,0,0,0,this,title,0);
  }
}

void GroupDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

void GroupDefImpl::removeMemberFromList(MemberListType lt,MemberDef *md)
{
    MemberList *ml = getMemberList(lt);
    if (ml) ml->remove(md); 
}

void GroupDefImpl::sortSubGroups() 
{ 
    groupList->sort(); 
}

bool GroupDefImpl::isLinkableInProject() const
{
  return !isReference() && isLinkable();
}

bool GroupDefImpl::isLinkable() const
{
  return hasUserDocumentation();
}

// let the "programming language" for a group depend on what is inserted into it.
// First item that has an associated languages determines the language for the whole group.
void GroupDefImpl::updateLanguage(const Definition *d)
{
  if (getLanguage()==SrcLangExt_Unknown && d->getLanguage()!=SrcLangExt_Unknown)
  {
    setLanguage(d->getLanguage());
  }
}

bool GroupDefImpl::hasDetailedDescription() const
{
  static bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
          !documentation().isEmpty());
}

