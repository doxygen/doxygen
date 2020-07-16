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
    virtual QCString displayName(bool=TRUE) const { return hasGroupTitle() ? m_title : DefinitionImpl::name(); }
    virtual const char *groupTitle() const { return m_title; }
    virtual void setGroupTitle( const char *newtitle );
    virtual bool hasGroupTitle( ) const { return m_titleSet; }
    virtual void addFile(const FileDef *def);
    virtual bool addClass(const ClassDef *def);
    virtual bool addNamespace(const NamespaceDef *def);
    virtual void addGroup(const GroupDef *def);
    virtual void addPage(PageDef *def);
    virtual void addExample(const PageDef *def);
    virtual void addDir(DirDef *dd);
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

    virtual void setGroupScope(Definition *d) { m_groupScope = d; }
    virtual Definition *getGroupScope() const { return m_groupScope; }

    virtual MemberList *getMemberList(MemberListType lt) const;
    virtual const QList<MemberList> &getMemberLists() const { return m_memberLists; }

    /* user defined member groups */
    virtual MemberGroupSDict *getMemberGroupSDict() const { return m_memberGroupSDict; }

    virtual FileList *      getFiles() const        { return m_fileList; }
    virtual ClassSDict *    getClasses() const      { return m_classSDict; }
    virtual NamespaceSDict * getNamespaces() const  { return m_namespaceSDict; }
    virtual GroupList *     getSubGroups() const    { return m_groupList; }
    virtual PageSDict *     getPages() const        { return m_pageDict; }
    virtual const DirList & getDirs() const         { return m_dirList; }
    virtual PageSDict *     getExamples() const     { return m_exampleDict; }
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

    QCString             m_title;               // title of the group
    bool                 m_titleSet;            // true if title is not the same as the name
    QCString             m_fileName;            // base name of the generated file
    FileList *           m_fileList;            // list of files in the group
    ClassSDict *         m_classSDict;          // list of classes in the group
    NamespaceSDict *     m_namespaceSDict;      // list of namespaces in the group
    GroupList *          m_groupList;           // list of sub groups.
    PageSDict *          m_pageDict;            // list of pages in the group
    PageSDict *          m_exampleDict;         // list of examples in the group
    DirList              m_dirList;             // list of directories in the group
    MemberList *         m_allMemberList;
    MemberNameInfoLinkedMap m_allMemberNameInfoLinkedMap;
    Definition *         m_groupScope;
    QList<MemberList>    m_memberLists;
    MemberGroupSDict *   m_memberGroupSDict;
    bool                 m_subGrouping;

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
  m_fileList = new FileList;
  m_classSDict = new ClassSDict(17);
  m_groupList = new GroupList;
  m_namespaceSDict = new NamespaceSDict(17);
  m_pageDict = new PageSDict(17);
  m_exampleDict = new PageSDict(17);
  if (refFileName)
  {
    m_fileName=stripExtension(refFileName);
  }
  else
  {
    m_fileName = convertNameToFile(QCString("group_")+na);
  }
  setGroupTitle( t );
  m_memberGroupSDict = new MemberGroupSDict;
  m_memberGroupSDict->setAutoDelete(TRUE);

  m_allMemberList = new MemberList(MemberListType_allMembersList);

  //visited = 0;
  m_groupScope = 0;
  m_subGrouping=Config_getBool(SUBGROUPING);
}

GroupDefImpl::~GroupDefImpl()
{
  delete m_fileList;
  delete m_classSDict;
  delete m_groupList;
  delete m_namespaceSDict;
  delete m_pageDict;
  delete m_exampleDict;
  delete m_allMemberList;
  delete m_memberGroupSDict;
}

void GroupDefImpl::setGroupTitle( const char *t )
{
  if ( t && *t )
  {
    m_title = t;
    m_titleSet = TRUE;
  }
  else
  {
    m_title = name();
    m_title[0]=(char)toupper(m_title[0]);
    m_titleSet = FALSE;
  }
}


void GroupDefImpl::distributeMemberGroupDocumentation()
{
  MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void GroupDefImpl::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,docFile());
  MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
    m_fileList->inSort(def);
  else
    m_fileList->append(def);
}

bool GroupDefImpl::addClass(const ClassDef *cd)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  if (cd->isHidden()) return FALSE;
  updateLanguage(cd);
  QCString qn = cd->name();
  if (m_classSDict->find(qn)==0)
  {
    //printf("--- addClass %s sort=%d\n",qn.data(),sortBriefDocs);
    if (sortBriefDocs)
    {
      m_classSDict->inSort(qn,cd);
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
        int j=m_classSDict->findAt(scope);
        if (j!=-1)
        {
          while (j<(int)m_classSDict->count() &&
                        m_classSDict->at(j)->qualifiedName().left(i)==scope)
          {
            //printf("skipping over %s\n",classSDict->at(j)->qualifiedName().data());
            j++;
          }
          //printf("Found scope at index %d\n",j);
          m_classSDict->insertAt(j,qn,cd);
          found=TRUE;
        }
      }
      if (!found) // no insertion point found -> just append
      {
        m_classSDict->append(qn,cd);
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
  if (m_namespaceSDict->find(def->name())==0)
  {
    if (sortBriefDocs)
      m_namespaceSDict->inSort(def->name(),def);
    else
      m_namespaceSDict->append(def->name(),def);
    return TRUE;
  }
  return FALSE;
}

void GroupDefImpl::addDir(DirDef *def)
{
  if (def->isHidden()) return;
  m_dirList.push_back(def);
}

void GroupDefImpl::addPage(PageDef *def)
{
  if (def->isHidden()) return;
  //printf("Making page %s part of a group\n",def->name.data());
  m_pageDict->append(def->name(),def);
  def->makePartOfGroup(this);
}

void GroupDefImpl::addExample(const PageDef *def)
{
  if (def->isHidden()) return;
  m_exampleDict->append(def->name(),def);
}


void GroupDefImpl::addMembersToMemberGroup()
{
  QListIterator<MemberList> mli(m_memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ::addMembersToMemberGroup(ml,&m_memberGroupSDict,this);
    }
  }

  //printf("GroupDefImpl::addMembersToMemberGroup() memberGroupList=%d\n",memberGroupList->count());
  MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
  MemberNameInfo *mni = m_allMemberNameInfoLinkedMap.add(md->name());
  for (auto &srcMi : *mni)
  {
    const MemberDef *srcMd = srcMi->memberDef();
    if (srcMd==md) return FALSE; // already added before!

    bool sameScope = srcMd->getOuterScope()==md->getOuterScope() || // same class or namespace
        // both inside a file => definition and declaration do not have to be in the same file
         (srcMd->getOuterScope()->definitionType()==Definition::TypeFile &&
             md->getOuterScope()->definitionType()==Definition::TypeFile);

    const ArgumentList &srcMdAl  = srcMd->argumentList();
    const ArgumentList &mdAl     = md->argumentList();
    const ArgumentList &tSrcMdAl = srcMd->templateArguments();
    const ArgumentList &tMdAl    = md->templateArguments();

    if (srcMd->isFunction() && md->isFunction() && // both are a function
        (tSrcMdAl.size()==tMdAl.size()) &&       // same number of template arguments
        matchArguments2(srcMd->getOuterScope(),srcMd->getFileDef(),&srcMdAl,
                        md->getOuterScope(),md->getFileDef(),&mdAl,
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
  mni->push_back(std::make_unique<MemberInfo>(md,md->protection(),md->virtualness(),FALSE));
  //printf("Added member!\n");
  m_allMemberList->append(md);
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
  MemberNameInfo *mni = m_allMemberNameInfoLinkedMap.find(md->name());
  if (mni)
  {
    m_allMemberNameInfoLinkedMap.del(md->name());

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
  else if (m_groupList)
  {
    GroupListIterator it(*m_groupList);
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
  m_groupList->append(def);
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
  if (m_memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
  return m_fileList->count()+
         m_classSDict->count()+
         m_namespaceSDict->count()+
         m_groupList->count()+
         m_allMemberList->count()+
         m_pageDict->count()+
         m_exampleDict->count();
}

/*! Compute the HTML anchor names for all members in the group */
void GroupDefImpl::computeAnchors()
{
  //printf("GroupDefImpl::computeAnchors()\n");
  setAnchors(m_allMemberList);
}

void GroupDefImpl::writeTagFile(FTextStream &tagFile)
{
  tagFile << "  <compound kind=\"group\">" << endl;
  tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "    <title>" << convertToXML(m_title) << "</title>" << endl;
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
          if (m_classSDict)
          {
            SDict<ClassDef>::Iterator ci(*m_classSDict);
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
          if (m_namespaceSDict)
          {
            SDict<NamespaceDef>::Iterator ni(*m_namespaceSDict);
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
          if (m_fileList)
          {
            QListIterator<FileDef> it(*m_fileList);
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
          if (m_pageDict)
          {
            PageSDict::Iterator pdi(*m_pageDict);
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
          for(const auto dd : m_dirList)
          {
            if (dd->isLinkableInProject())
            {
              tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>" << endl;
            }
          }
        }
        break;
      case LayoutDocEntry::GroupNestedGroups:
        {
          if (m_groupList)
          {
            QListIterator<GroupDef> it(*m_groupList);
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
          if (m_memberGroupSDict)
          {
            MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
    if (m_pageDict->count()!=(uint)numDocMembers()) // not only pages -> classical layout
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
      ol.generateDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE,
                     0,FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
      ol.generateDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE,
                     0,FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }

    // write inbody documentation
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),this,0,inbodyDocumentation()+"\n",TRUE,FALSE,
                     0,FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
  }
}

void GroupDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),this,0,
                                briefDescription(),TRUE,FALSE,
                                0,TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
      ol.parseText(theTranslator->trCollaborationDiagram(m_title));
      ol.endGroupCollaboration(graph);
      //ol.endParagraph();
      ol.popGeneratorState();
    }
  }
}

void GroupDefImpl::writeFiles(OutputList &ol,const QCString &title)
{
  // write list of files
  if (m_fileList->count()>0)
  {
    ol.startMemberHeader("files");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    QListIterator<FileDef> it(*m_fileList);
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
        ol.generateDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE,
                       0,TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
  m_namespaceSDict->writeDeclaration(ol,title);
}

void GroupDefImpl::writeNestedGroups(OutputList &ol,const QCString &title)
{
  // write list of groups
  int count=0;
  if (m_groupList->count()>0)
  {
    QListIterator<GroupDef> it(*m_groupList);
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
      m_groupList->sort();
    }
    QListIterator<GroupDef> it(*m_groupList);
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
          ol.generateDoc(briefFile(),briefLine(),gd,0,gd->briefDescription(),FALSE,FALSE,
                         0,TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
  if (!m_dirList.empty())
  {
    ol.startMemberHeader("dirs");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    for(const auto dd : m_dirList)
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
        ol.generateDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),FALSE,FALSE,
                       0,TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
  m_classSDict->writeDeclaration(ol,0,title,FALSE);
}

void GroupDefImpl::writeInlineClasses(OutputList &ol)
{
  m_classSDict->writeDocumentation(ol);
}

void GroupDefImpl::writePageDocumentation(OutputList &ol)
{
  PageDef *pd=0;
  PageSDict::Iterator pdi(*m_pageDict);
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->isReference())
    {
      const SectionInfo *si=0;
      if (pd->hasTitle() && !pd->name().isEmpty() &&
          (si=SectionManager::instance().find(pd->name()))!=0)
      {
        ol.startSection(si->label(),si->title(),SectionType::Subsection);
        ol.docify(si->title());
        ol.endSection(si->label(),SectionType::Subsection);
      }
      ol.startTextBlock();
      ol.generateDoc(pd->docFile(),pd->docLine(),pd,0,pd->documentation()+pd->inbodyDocumentation(),TRUE,FALSE,
                     0,TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
      ol.endTextBlock();
    }
  }
}

void GroupDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  if (m_memberGroupSDict)
  {
    m_memberGroupSDict->sort();
    /* write user defined member groups */
    MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
    if ((lde->kind()==LayoutDocEntry::GroupClasses && m_classSDict->declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupNamespaces && m_namespaceSDict->declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupFiles && m_fileList->count()>0) ||
        (lde->kind()==LayoutDocEntry::GroupNestedGroups && m_groupList->count()>0) ||
        (lde->kind()==LayoutDocEntry::GroupDirs && !m_dirList.empty())
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
  startFile(ol,getOutputFileBase(),name(),m_title,HLI_Modules);

  ol.startHeaderSection();
  writeSummaryLinks(ol);
  ol.startTitleHead(getOutputFileBase());
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.parseText(m_title);
  ol.popGeneratorState();
  addGroupListToTitle(ol,this);
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.endTitleHead(getOutputFileBase(),m_title);
  ol.popGeneratorState();
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.endTitleHead(getOutputFileBase(),name());
  ol.parseText(m_title);
  ol.popGeneratorState();
  ol.endHeaderSection();
  ol.startContents();

  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    static QRegExp we("[a-zA-Z_][-a-zA-Z_0-9]*");
    int i=0,p=0,l=0;
    while ((i=we.match(m_title,p,&l))!=-1) // foreach word in the title
    {
      Doxygen::searchIndex->addWord(m_title.mid(i,l),TRUE);
      p=i+l;
    }
  }

  Doxygen::indexList->addIndexItem(this,0,0,m_title);

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
    m_allMemberList->sort();
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

  MemberListIterator mli(*m_allMemberList);
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

void addClassToGroups(const Entry *root,ClassDef *cd)
{
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    if (!g.groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g.groupname)))
    {
      if (gd->addClass(cd))
      {
        cd->makePartOfGroup(gd);
      }
      //printf("Compound %s: in group %s\n",cd->name().data(),gd->groupTitle());
    }
  }
}

void addNamespaceToGroups(const Entry *root,NamespaceDef *nd)
{
  //printf("root->groups.size()=%d\n",root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    //printf("group '%s'\n",s->data());
    if (!g.groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g.groupname)))
    {
      if (gd->addNamespace(nd)) nd->makePartOfGroup(gd);
      //printf("Namespace %s: in group %s\n",nd->name().data(),s->data());
    }
  }
}

void addDirToGroups(const Entry *root,DirDef *dd)
{
  //printf("*** root->groups.size()=%d\n",root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    //printf("group '%s'\n",g->groupname.data());
    if (!g.groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g.groupname)))
    {
      gd->addDir(dd);
      dd->makePartOfGroup(gd);
      //printf("Dir %s: in group %s\n",dd->name().data(),g->groupname.data());
    }
  }
}

void addGroupToGroups(const Entry *root,GroupDef *subGroup)
{
  //printf("addGroupToGroups for %s groups=%d\n",root->name.data(),root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    if (!g.groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g.groupname)))
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
void addMemberToGroups(const Entry *root,MemberDef *md)
{
  //printf("addMemberToGroups:  Root %p = %s, md %p=%s groups=%d\n",
  //    root, root->name.data(), md, md->name().data(), root->groups->count() );

  // Search entry's group list for group with highest pri.
  Grouping::GroupPri_t pri = Grouping::GROUPING_LOWEST;
  GroupDef *fgd=0;
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    if (!g.groupname.isEmpty() &&
        (gd=Doxygen::groupSDict->find(g.groupname)) &&
        g.pri >= pri)
    {
      if (fgd && gd!=fgd && g.pri==pri)
      {
        warn(root->fileName.data(), root->startLine,
            "Member %s found in multiple %s groups! "
            "The member will be put in group %s, and not in group %s",
            md->name().data(), Grouping::getGroupPriName( pri ),
            gd->name().data(), fgd->name().data()
            );
      }

      fgd = gd;
      pri = g.pri;
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


void addExampleToGroups(const Entry *root,PageDef *eg)
{
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    if (!g.groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g.groupname)))
    {
      gd->addExample(eg);
      eg->makePartOfGroup(gd);
      //printf("Example %s: in group %s\n",eg->name().data(),s->data());
    }
  }
}

QCString GroupDefImpl::getOutputFileBase() const
{
  return m_fileName;
}

void GroupDefImpl::addListReferences()
{
  {
    const RefItemVector &xrefItems = xrefListItems();
    addRefItem(xrefItems,
             getOutputFileBase(),
             theTranslator->trGroup(TRUE,TRUE),
             getOutputFileBase(),name(),
             0,
             0
            );
  }
  MemberGroupSDict::Iterator mgli(*m_memberGroupSDict);
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
  if (Config_getBool(SORT_BRIEF_DOCS))
  {
    std::sort(m_dirList.begin(), m_dirList.end(), compareDirDefs);
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
  m_groupList->sort();
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

