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

#include <algorithm>
#include <vector>

#include <ctype.h>

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
#include "regex.h"
#include "moduledef.h"

//---------------------------------------------------------------------------

class GroupDefImpl : public DefinitionMixin<GroupDef>
{
  public:
    GroupDefImpl(const QCString &fileName,int line,const QCString &name,const QCString &title,const QCString &refFileName=QCString());
   ~GroupDefImpl() override;
    NON_COPYABLE(GroupDefImpl)

    DefType definitionType() const override { return TypeGroup; }
    CodeSymbolType codeSymbolType() const override { return CodeSymbolType::Default; }
    QCString getOutputFileBase() const override;
    QCString anchor() const override { return QCString(); }
    QCString displayName(bool=TRUE) const override { return hasGroupTitle() ? m_title : DefinitionMixin::name(); }
    QCString groupTitle() const override { return m_title; }
    void setGroupTitle( const QCString &newtitle ) override;
    bool hasGroupTitle( ) const override { return m_titleSet; }
    void addFile(FileDef *def) override;
    bool containsFile(const FileDef *def) const override;
    bool addClass(ClassDef *def) override;
    bool addConcept(ConceptDef *def) override;
    bool addModule(ModuleDef *def) override;
    bool addNamespace(NamespaceDef *def) override;
    void addGroup(GroupDef *def) override;
    void addPage(PageDef *def) override;
    void addExample(PageDef *def) override;
    void addDir(DirDef *dd) override;
    bool insertMember(MemberDef *def,bool docOnly=FALSE) override;
    void removeMember(MemberDef *md) override;
    bool findGroup(const GroupDef *def) const override; // true if def is a subgroup of this group
    void writeDocumentation(OutputList &ol) override;
    void writeMemberPages(OutputList &ol, int hierarchyLevel) override;
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const override;
    void writeTagFile(TextStream &) override;
    size_t numDocMembers() const override;
    bool isLinkableInProject() const override;
    bool isLinkable() const override;
    bool isVisibleInHierarchy() const override;
    bool isASubGroup() const override;
    void computeAnchors() override;
    void countMembers() override;

    void addMembersToMemberGroup() override;
    void distributeMemberGroupDocumentation() override;
    void findSectionsInDocumentation() override;

    void addListReferences() override;
    void addRequirementReferences() override;
    void sortMemberLists() override;
    bool subGrouping() const override { return m_subGrouping; }

    void setGroupScope(Definition *d) override { m_groupScope = d; }
    Definition *getGroupScope() const override { return m_groupScope; }

    MemberList *getMemberList(MemberListType lt) const override;
    const MemberLists &getMemberLists() const override { return m_memberLists; }

    /* user defined member groups */
    const MemberGroupList &getMemberGroups() const override { return m_memberGroups; }

    const FileList &getFiles() const override                    { return m_fileList; }
    const ClassLinkedRefMap &getClasses() const override         { return m_classes; }
    const ConceptLinkedRefMap &getConcepts() const override      { return m_concepts; }
    const ModuleLinkedRefMap &getModules() const override        { return m_modules; }
    const NamespaceLinkedRefMap &getNamespaces() const override  { return m_namespaces; }
    const GroupList &getSubGroups() const override               { return m_groups; }
    const PageLinkedRefMap &getPages() const override            { return m_pages; }
    const DirList & getDirs() const override                     { return m_dirList; }
    const PageLinkedRefMap &getExamples() const override         { return m_examples; }
    bool hasDetailedDescription() const override;
    void sortSubGroups() override;
    void writeSummaryLinks(OutputList &ol) const override;
    void writePageNavigation(OutputList &ol) const override;

    bool hasGroupGraph() const override;
    void overrideGroupGraph(bool e) override;
  private:
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);
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
    void writeConcepts(OutputList &ol,const QCString &title);
    void writeModules(OutputList &ol,const QCString &title);
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
    void updateLanguage(const Definition *);
    void setGroupTitleLocal( const QCString &title);

    QCString             m_title;               // title of the group
    QCString             m_titleAsText;         // title of the group in plain text
    bool                 m_titleSet;            // true if title is not the same as the name
    QCString             m_fileName;            // base name of the generated file
    FileList             m_fileList;            // list of files in the group
    ClassLinkedRefMap    m_classes;             // list of classes in the group
    ConceptLinkedRefMap  m_concepts;            // list of concepts in the group
    ModuleLinkedRefMap   m_modules;             // list of modules in the group
    NamespaceLinkedRefMap m_namespaces;         // list of namespaces in the group
    GroupList            m_groups;              // list of sub groups.
    PageLinkedRefMap     m_pages;               // list of pages in the group
    PageLinkedRefMap     m_examples;            // list of examples in the group
    DirList              m_dirList;             // list of directories in the group
    MemberList           m_allMemberList;
    MemberNameInfoLinkedMap m_allMemberNameInfoLinkedMap;
    Definition *         m_groupScope = nullptr;
    MemberLists          m_memberLists;
    MemberGroupList      m_memberGroups;
    bool                 m_subGrouping;
    bool                 m_hasGroupGraph = false;

};

std::unique_ptr<GroupDef> createGroupDef(const QCString &fileName,int line,const QCString &name,
                                const QCString &title,const QCString &refFileName)
{
  return std::make_unique<GroupDefImpl>(fileName,line,name,title,refFileName);
}


//---------------------------------------------------------------------------

GroupDefImpl::GroupDefImpl(const QCString &df,int dl,const QCString &na,const QCString &t,
                   const QCString &refFileName) : DefinitionMixin(df,dl,1,na),
                    m_allMemberList(MemberListType::AllMembersList(),MemberListContainer::Group)
{
  if (!refFileName.isEmpty())
  {
    m_fileName=stripExtension(refFileName);
  }
  else
  {
    m_fileName = convertNameToFile(QCString("group_")+na);
  }
  setGroupTitleLocal( t );

  //visited = 0;
  m_groupScope = nullptr;
  m_subGrouping=Config_getBool(SUBGROUPING);
  m_hasGroupGraph=Config_getBool(GROUP_GRAPHS);
}

GroupDefImpl::~GroupDefImpl()
{
}

void GroupDefImpl::setGroupTitleLocal( const QCString &t )
{
  if ( !t.isEmpty())
  {
    m_title = t;
    m_titleAsText = parseCommentAsText(this,nullptr,t,docFile(),docLine());
    m_titleSet = TRUE;
  }
  else
  {
    m_title = name();
    m_title[0]=static_cast<char>(toupper(m_title[0]));
    m_titleAsText = m_title;
    m_titleSet = FALSE;
  }
}

void GroupDefImpl::setGroupTitle( const QCString &t )
{
  setGroupTitleLocal(t);
}


void GroupDefImpl::distributeMemberGroupDocumentation()
{
  for (const auto &mg : m_memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void GroupDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());
  docFindSections(inbodyDocumentation(),this,docFile());

  for (const auto &mg : m_memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }

  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDeclaration())
    {
      ml->findSectionsInDocumentation(this);
    }
  }
}

void GroupDefImpl::addFile(FileDef *def)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  if (def->isHidden()) return;
  updateLanguage(def);
  if (sortBriefDocs)
    m_fileList.insert( std::upper_bound( m_fileList.begin(), m_fileList.end(), def,
                                         [](const auto &fd1, const auto &fd2)
                                         { return qstricmp_sort(fd1->name(),fd2->name())<0; }),
                       def);
  else
    m_fileList.push_back(def);
}

bool GroupDefImpl::containsFile(const FileDef *def) const
{
  return std::find(m_fileList.cbegin(),m_fileList.cend(), def) != m_fileList.cend();
}

bool GroupDefImpl::addClass(ClassDef *cd)
{
  if (cd->isHidden()) return FALSE;
  updateLanguage(cd);
  QCString qn = cd->name();
  if (m_classes.find(qn)==nullptr)
  {
    m_classes.add(qn,cd);
    return TRUE;
  }
  return FALSE;
}

bool GroupDefImpl::addConcept(ConceptDef *cd)
{
  if (cd->isHidden()) return FALSE;
  QCString qn = cd->name();
  if (m_concepts.find(qn)==nullptr)
  {
    m_concepts.add(qn,cd);
    return TRUE;
  }
  return FALSE;
}

bool GroupDefImpl::addModule(ModuleDef *mod)
{
  if (mod->isHidden()) return false;
  QCString qn = mod->name();
  if (m_modules.find(qn)==nullptr)
  {
    m_modules.add(qn,mod);
    return true;
  }
  return false;
}

bool GroupDefImpl::addNamespace(NamespaceDef *def)
{
  //printf("adding namespace hidden=%d\n",def->isHidden());
  if (def->isHidden()) return false;
  if (m_namespaces.find(def->name())==nullptr)
  {
    updateLanguage(def);
    m_namespaces.add(def->name(),def);
    return true;
  }
  return false;
}

void GroupDefImpl::addDir(DirDef *def)
{
  if (def->isHidden()) return;
  m_dirList.push_back(def);
}

void GroupDefImpl::addPage(PageDef *def)
{
  if (def->isHidden()) return;
  //printf("Making page %s part of a group\n",qPrint(def->name));
  m_pages.add(def->name(),def);
  def->makePartOfGroup(this);
}

void GroupDefImpl::addExample(PageDef *def)
{
  if (def->isHidden()) return;
  m_examples.add(def->name(),def);
}


void GroupDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDeclaration())
    {
      ::addMembersToMemberGroup(ml.get(),&m_memberGroups,this);
    }
  }
}


bool GroupDefImpl::insertMember(MemberDef *md,bool docOnly)
{
  if (md->isHidden()) return FALSE;
  updateLanguage(md);
  //printf("GroupDef(%s)::insertMember(%s)\n", qPrint(title), qPrint(md->name()));
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
        matchArguments2(srcMd->getOuterScope(),srcMd->getFileDef(),srcMd->typeString(),&srcMdAl,
                           md->getOuterScope(),   md->getFileDef(),   md->typeString(),&mdAl,
                           TRUE,srcMd->getLanguage()
                       ) && // matching parameters
        sameScope // both are found in the same scope
       )
    {
      MemberDefMutable *mdm = toMemberDefMutable(md);
      if (mdm && srcMd->getGroupAlias()==nullptr)
      {
        mdm->setGroupAlias(srcMd);
      }
      else if (mdm && md!=srcMd->getGroupAlias())
      {
        mdm->setGroupAlias(srcMd->getGroupAlias());
      }
      return FALSE; // member is the same as one that is already added
    }
  }
  mni->push_back(std::make_unique<MemberInfo>(md,md->protection(),md->virtualness(),false,false));
  //printf("Added member!\n");
  m_allMemberList.push_back(md);
  switch(md->memberType())
  {
    case MemberType::Variable:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecVarMembers(),md);
      }
      addMemberToList(MemberListType::DocVarMembers(),md);
      break;
    case MemberType::Function:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecFuncMembers(),md);
      }
      addMemberToList(MemberListType::DocFuncMembers(),md);
      break;
    case MemberType::Typedef:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecTypedefMembers(),md);
      }
      addMemberToList(MemberListType::DocTypedefMembers(),md);
      break;
    case MemberType::Enumeration:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecEnumMembers(),md);
      }
      addMemberToList(MemberListType::DocEnumMembers(),md);
      break;
    case MemberType::EnumValue:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecEnumValMembers(),md);
      }
      addMemberToList(MemberListType::DocEnumValMembers(),md);
      break;
    case MemberType::Define:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecDefineMembers(),md);
      }
      addMemberToList(MemberListType::DocDefineMembers(),md);
      break;
    case MemberType::Signal:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecSignalMembers(),md);
      }
      addMemberToList(MemberListType::DocSignalMembers(),md);
      break;
    case MemberType::Slot:
      if (md->protection()==Protection::Public)
      {
        if (!docOnly)
        {
          addMemberToList(MemberListType::DecPubSlotMembers(),md);
        }
        addMemberToList(MemberListType::DocPubSlotMembers(),md);
      }
      else if (md->protection()==Protection::Protected)
      {
        if (!docOnly)
        {
          addMemberToList(MemberListType::DecProSlotMembers(),md);
        }
        addMemberToList(MemberListType::DocProSlotMembers(),md);
      }
      else
      {
        if (!docOnly)
        {
          addMemberToList(MemberListType::DecPriSlotMembers(),md);
        }
        addMemberToList(MemberListType::DocPriSlotMembers(),md);
      }
      break;
    case MemberType::Event:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecEventMembers(),md);
      }
      addMemberToList(MemberListType::DocEventMembers(),md);
      break;
    case MemberType::Property:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecPropMembers(),md);
      }
      addMemberToList(MemberListType::DocPropMembers(),md);
      break;
    case MemberType::Friend:
      if (!docOnly)
      {
        addMemberToList(MemberListType::DecFriendMembers(),md);
      }
      addMemberToList(MemberListType::DocFriendMembers(),md);
      break;
    default:
      err("GroupDefImpl::insertMembers(): "
           "member '{}' (typeid='{}') with scope '{}' inserted in group scope '{}'!\n",
           md->name(),md->memberTypeName(),
           md->getClassDef() ? md->getClassDef()->name() : "",
           name());
  }
  return TRUE;
}

void GroupDefImpl::removeMember(MemberDef *md)
{
  // fprintf(stderr, "GroupDef(%s)::removeMember( %s )\n", qPrint(title), qPrint(md->name()));
  MemberNameInfo *mni = m_allMemberNameInfoLinkedMap.find(md->name());
  if (mni)
  {
    m_allMemberNameInfoLinkedMap.del(md->name());

    removeMemberFromList(MemberListType::AllMembersList(),md);
    switch(md->memberType())
    {
      case MemberType::Variable:
	removeMemberFromList(MemberListType::DecVarMembers(),md);
        removeMemberFromList(MemberListType::DocVarMembers(),md);
        break;
      case MemberType::Function:
        removeMemberFromList(MemberListType::DecFuncMembers(),md);
        removeMemberFromList(MemberListType::DocFuncMembers(),md);
        break;
      case MemberType::Typedef:
        removeMemberFromList(MemberListType::DecTypedefMembers(),md);
        removeMemberFromList(MemberListType::DocTypedefMembers(),md);
        break;
      case MemberType::Enumeration:
        removeMemberFromList(MemberListType::DecEnumMembers(),md);
        removeMemberFromList(MemberListType::DocEnumMembers(),md);
        break;
      case MemberType::EnumValue:
        removeMemberFromList(MemberListType::DecEnumValMembers(),md);
        removeMemberFromList(MemberListType::DocEnumValMembers(),md);
        break;
      case MemberType::Define:
        removeMemberFromList(MemberListType::DecDefineMembers(),md);
        removeMemberFromList(MemberListType::DocDefineMembers(),md);
        break;
      case MemberType::Signal:
        removeMemberFromList(MemberListType::DecSignalMembers(),md);
        removeMemberFromList(MemberListType::DocSignalMembers(),md);
        break;
      case MemberType::Slot:
        if (md->protection()==Protection::Public)
        {
          removeMemberFromList(MemberListType::DecPubSlotMembers(),md);
          removeMemberFromList(MemberListType::DocPubSlotMembers(),md);
        }
        else if (md->protection()==Protection::Protected)
        {
          removeMemberFromList(MemberListType::DecProSlotMembers(),md);
          removeMemberFromList(MemberListType::DocProSlotMembers(),md);
        }
        else
        {
          removeMemberFromList(MemberListType::DecPriSlotMembers(),md);
          removeMemberFromList(MemberListType::DocPriSlotMembers(),md);
        }
        break;
      case MemberType::Event:
        removeMemberFromList(MemberListType::DecEventMembers(),md);
        removeMemberFromList(MemberListType::DocEventMembers(),md);
        break;
      case MemberType::Property:
        removeMemberFromList(MemberListType::DecPropMembers(),md);
        removeMemberFromList(MemberListType::DocPropMembers(),md);
        break;
      case MemberType::Friend:
        removeMemberFromList(MemberListType::DecFriendMembers(),md);
        removeMemberFromList(MemberListType::DocFriendMembers(),md);
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
  for (const auto &gd : m_groups)
  {
    if (gd->findGroup(def))
    {
      return TRUE;
    }
  }
  return FALSE;
}

void GroupDefImpl::addGroup(GroupDef *def)
{
  //printf("adding group '%s' to group '%s'\n",qPrint(def->name()),qPrint(name()));
  //if (Config_getBool(SORT_MEMBER_DOCS))
  //  groupList->inSort(def);
  //else
  m_groups.push_back(def);
}

bool GroupDefImpl::isASubGroup() const
{
  return !partOfGroups().empty();
}

void GroupDefImpl::countMembers()
{
  for (auto &ml : m_memberLists)
  {
    ml->countDecMembers();
    ml->countDocMembers();
  }
  for (const auto &mg : m_memberGroups)
  {
    mg->countDecMembers();
    mg->countDocMembers();
  }
}

size_t GroupDefImpl::numDocMembers() const
{
  return m_fileList.size()+
         m_classes.size()+
         m_namespaces.size()+
         m_groups.size()+
         m_allMemberList.size()+
         m_pages.size()+
         m_examples.size();
}

/*! Compute the HTML anchor names for all members in the group */
void GroupDefImpl::computeAnchors()
{
  //printf("GroupDefImpl::computeAnchors()\n");
  m_allMemberList.setAnchors();
}

void GroupDefImpl::writeTagFile(TextStream &tagFile)
{
  QCString fn = getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "  <compound kind=\"group\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <title>" << convertToXML(m_titleAsText) << "</title>\n";
  tagFile << "    <filename>" << fn << "</filename>\n";
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::GroupClasses:
        {
          for (const auto &cd : m_classes)
          {
            if (cd->isLinkableInProject())
            {
              tagFile << "    <class kind=\"" << cd->compoundTypeString()
                      << "\">" << convertToXML(cd->name()) << "</class>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupConcepts:
        {
          for (const auto &cd : m_concepts)
          {
            if (cd->isLinkableInProject())
            {
              tagFile << "    <concept>" << convertToXML(cd->name())
                      << "</concept>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupModules:
        {
          for (const auto &mod : m_modules)
          {
            if (mod->isLinkableInProject())
            {
              tagFile << "    <module>" << convertToXML(mod->name())
                      << "</module>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupNamespaces:
        {
          for (const auto &nd : m_namespaces)
          {
            if (nd->isLinkableInProject())
            {
              tagFile << "    <namespace>" << convertToXML(nd->name())
                      << "</namespace>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupFiles:
        {
          for (const auto &fd : m_fileList)
          {
            if (fd->isLinkableInProject())
            {
              tagFile << "    <file>" << convertToXML(fd->name()) << "</file>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupPageDocs:
        {
          for (const auto &pd : m_pages)
          {
            QCString pageName = pd->getOutputFileBase();
            if (pd->isLinkableInProject())
            {
              tagFile << "    <page>" << convertToXML(pageName) << "</page>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupDirs:
        {
          for (const auto &dd : m_dirList)
          {
            if (dd->isLinkableInProject())
            {
              tagFile << "    <dir>" << convertToXML(dd->displayName()) << "</dir>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::GroupNestedGroups:
        {
          for (const auto &gd : m_groups)
          {
            if (gd->isVisible())
            {
              tagFile << "    <subgroup>" << convertToXML(gd->name()) << "</subgroup>\n";
            }
          }
        }
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd)
          {
            MemberList * ml = getMemberList(lmd->type);
            if (ml)
            {
              ml->writeTagFile(tagFile,true);
            }
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        {
          for (const auto &mg : m_memberGroups)
          {
            mg->writeTagFile(tagFile,true);
          }
        }
        break;
      default:
        break;
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>\n";
}

void GroupDefImpl::writeDetailedDescription(OutputList &ol,const QCString &title)
{
  if (hasDetailedDescription())
  {
    ol.pushGeneratorState();
    if (m_pages.size()!=numDocMembers()) // not only pages -> classical layout
    {
      ol.pushGeneratorState();
        ol.disable(OutputType::Html);
        ol.writeRuler();
      ol.popGeneratorState();
      ol.pushGeneratorState();
        ol.disableAllBut(OutputType::Html);
        ol.writeAnchor(QCString(),"details");
      ol.popGeneratorState();
    }
    else
    {
      ol.disableAllBut(OutputType::Man); // always print title for man page
    }
    ol.startGroupHeader("details");
    ol.parseText(title);
    ol.endGroupHeader();
    ol.popGeneratorState();

    // repeat brief description
    ol.startTextBlock();
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
    {
      ol.generateDoc(briefFile(),
                     briefLine(),
                     this,
                     nullptr,
                     briefDescription(),
                     DocOptions());
    }
    // write separator between brief and details
    if (!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF) &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
      ol.disable(OutputType::Man);
      ol.disable(OutputType::RTF);
      ol.enableAll();
      ol.disableAllBut(OutputType::Man);
      ol.enable(OutputType::Latex);
      ol.writeString("\n\n");
      ol.popGeneratorState();
    }

    // write detailed documentation
    if (!documentation().isEmpty())
    {
      ol.generateDoc(docFile(),
                     docLine(),
                     this,
                     nullptr,
                     documentation()+"\n",
                     DocOptions()
                     .setIndexWords(true));
    }

    // write inbody documentation
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),
                     inbodyLine(),
                     this,
                     nullptr,
                     inbodyDocumentation()+"\n",
                     DocOptions()
                     .setIndexWords(true));
    }
    if (hasRequirementRefs()) writeRequirementRefs(ol);
    ol.endTextBlock();
  }
}

void GroupDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     briefFile(),
                                     briefLine(),
                                     this,
                                     nullptr,
                                     briefDescription(),
                                     DocOptions()
                                     .setIndexWords(true)
                                     .setSingleLine(true))
                };
    if (!ast->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(ast.get(),this,nullptr);
      ol.pushGeneratorState();
      ol.disable(OutputType::RTF);
      ol.writeString(" \n");
      ol.enable(OutputType::RTF);

      if (hasDetailedDescription())
      {
        ol.disableAllBut(OutputType::Html);
        ol.startTextLink(QCString(),"details");
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
      }
      ol.popGeneratorState();
      ol.endParagraph();
    }
  }
  ol.writeSynopsis();
}

void GroupDefImpl::writeGroupGraph(OutputList &ol)
{
  if (Config_getBool(HAVE_DOT) && m_hasGroupGraph /*&& Config_getBool(GROUP_GRAPHS)*/)
  {
    DotGroupCollaboration graph(this);
    if (graph.isTooBig())
    {
       warn_uncond("Group dependency graph for '{}' not generated, too many nodes ({}), threshold is {}. Consider increasing DOT_GRAPH_MAX_NODES.\n",
           name(), graph.numNodes(), Config_getInt(DOT_GRAPH_MAX_NODES));
    }
    else if (!graph.isTrivial())
    {
      msg("Generating dependency graph for group {}\n",qualifiedName());
      ol.pushGeneratorState();
      ol.disable(OutputType::Man);
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
  if (!m_fileList.empty())
  {
    ol.startMemberHeader("files");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    for (const auto &fd : m_fileList)
    {
      if (!fd->hasDocumentation()) continue;
      ol.startMemberDeclaration();
      QCString anc = fd->anchor();
      if (anc.isEmpty()) anc=fd->displayName(); else anc.prepend(fd->displayName()+"_");
      ol.startMemberItem(anc,OutputGenerator::MemberItemType::Normal);
      ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),fd->displayName());
      ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
      if (!fd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(fd->getOutputFileBase());
        ol.generateDoc(briefFile(),
                       briefLine(),
                       fd,
                       nullptr,
                       fd->briefDescription(),
                       DocOptions()
                       .setSingleLine(true));
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(QCString(),QCString());
    }
    ol.endMemberList();
  }
}

void GroupDefImpl::writeNamespaces(OutputList &ol,const QCString &title)
{
  // write list of namespaces
  m_namespaces.writeDeclaration(ol,title);
}

void GroupDefImpl::writeNestedGroups(OutputList &ol,const QCString &title)
{
  // write list of groups
  int count=0;
  for (const auto &gd : m_groups)
  {
    if (gd->isVisible()) count++;
  }
  if (count>0)
  {
    ol.startMemberHeader("groups");
    ol.parseText(title);
    ol.endMemberHeader();
    ol.startMemberList();
    for (const auto &gd : m_groups)
    {
      if (gd->isVisible())
      {
        if (!gd->hasDocumentation()) continue;
        ol.startMemberDeclaration();
        QCString anc = gd->anchor();
        if (anc.isEmpty()) anc=gd->name(); else anc.prepend(gd->name()+"_");
        ol.startMemberItem(anc,OutputGenerator::MemberItemType::Normal);
        ol.insertMemberAlign();
        ol.generateDoc(gd->getDefFileName(),
                       gd->getDefLine(),
                       gd,
                       nullptr,
                       gd->groupTitle(),
                       DocOptions()
                       .setSingleLine(true)
                       .setAutolinkSupport(false));
        ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
        if (!gd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(gd->getOutputFileBase());
          ol.generateDoc(briefFile(),
                         briefLine(),
                         gd,
                         nullptr,
                         gd->briefDescription(),
                         DocOptions()
                         .setSingleLine(true));
          ol.endMemberDescription();
        }
        ol.endMemberDeclaration(QCString(),QCString());
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
      QCString anc = dd->anchor();
      if (anc.isEmpty()) anc=dd->shortName(); else anc.prepend(dd->shortName()+"_");
      ol.startMemberItem(anc,OutputGenerator::MemberItemType::Normal);
      ol.parseText(theTranslator->trDir(FALSE,TRUE));
      ol.insertMemberAlign();
      ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),QCString(),dd->shortName());
      ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
      if (!dd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(dd->getOutputFileBase());
        ol.generateDoc(briefFile(),
                       briefLine(),
                       dd,
                       nullptr,
                       dd->briefDescription(),
                       DocOptions()
                       .setSingleLine(true));
        ol.endMemberDescription();
      }
      ol.endMemberDeclaration(QCString(),QCString());
    }

    ol.endMemberList();
  }
}

void GroupDefImpl::writeClasses(OutputList &ol,const QCString &title)
{
  // write list of classes
  m_classes.writeDeclaration(ol,nullptr,title,FALSE);
}

void GroupDefImpl::writeConcepts(OutputList &ol,const QCString &title)
{
  // write list of concepts
  m_concepts.writeDeclaration(ol,title,FALSE);
}

void GroupDefImpl::writeModules(OutputList &ol,const QCString &title)
{
  // write list of modules
  m_modules.writeDeclaration(ol,title,FALSE);
}


void GroupDefImpl::writeInlineClasses(OutputList &ol)
{
  m_classes.writeDocumentation(ol);
}

void GroupDefImpl::writePageDocumentation(OutputList &ol)
{
  for (const auto *pd : m_pages)
  {
    if (!pd->isReference())
    {
      const SectionInfo *si=nullptr;
      if (pd->hasTitle() && !pd->name().isEmpty() &&
          (si=SectionManager::instance().find(pd->name()))!=nullptr)
      {
        ol.startSection(si->label(),si->title(),SectionType::Subsection);
        ol.docify(si->title());
        ol.endSection(si->label(),SectionType::Subsection);
      }
      ol.startTextBlock();
      ol.generateDoc(pd->docFile(),
                     pd->docLine(),
                     pd,
                     nullptr,
                     (pd->documentation()+pd->inbodyDocumentation()),
                     DocOptions()
                     .setIndexWords(true));
      ol.endTextBlock();
    }
  }
}

void GroupDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  for (const auto &mg : m_memberGroups)
  {
    mg->writeDeclarations(ol,nullptr,nullptr,nullptr,this,nullptr);
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
    ol.disable(OutputType::Html);
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
  ol.disableAllBut(OutputType::Man);
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString(PROJECT_NAME)));
  ol.popGeneratorState();
}

void GroupDefImpl::writeSummaryLinks(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  bool first=TRUE;
  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
  {
    if ((lde->kind()==LayoutDocEntry::GroupClasses      &&  m_classes.declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupConcepts     &&  m_concepts.declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupModules      &&  m_modules.declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupNamespaces   &&  m_namespaces.declVisible(false)) ||
        (lde->kind()==LayoutDocEntry::GroupFiles        && !m_fileList.empty()) ||
        (lde->kind()==LayoutDocEntry::GroupNestedGroups && !m_groups.empty()) ||
        (lde->kind()==LayoutDocEntry::GroupDirs         && !m_dirList.empty())
       )
    {
      const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
      if (ls)
      {
        QCString label = lde->kind()==LayoutDocEntry::GroupClasses      ? "nested-classes" :
                         lde->kind()==LayoutDocEntry::GroupConcepts     ? "concepts"       :
                         lde->kind()==LayoutDocEntry::GroupModules      ? "modules"        :
                         lde->kind()==LayoutDocEntry::GroupNamespaces   ? "namespaces"     :
                         lde->kind()==LayoutDocEntry::GroupFiles        ? "files"          :
                         lde->kind()==LayoutDocEntry::GroupNestedGroups ? "groups"         :
                                                                          "dirs";
        ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
        first=FALSE;
      }
    }
    else if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd)
      {
        MemberList * ml = getMemberList(lmd->type);
        if (ml && ml->declVisible())
        {
          ol.writeSummaryLink(QCString(),ml->listType().toLabel(),lmd->title(lang),first);
          first=FALSE;
        }
      }
    }
  }
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void GroupDefImpl::writePageNavigation(OutputList &ol) const
{
  ol.writePageOutline();
}

void GroupDefImpl::writeDocumentation(OutputList &ol)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  ol.pushGeneratorState();

  // Find out how deep this group is nested. In case of multiple parents, use the first one.
  int hierarchyLevel = 0;
  const GroupDef *gd = this;
  while (!gd->partOfGroups().empty())
  {
    gd = gd->partOfGroups().front();
    ++hierarchyLevel;
  }

  startFile(ol,getOutputFileBase(),false,name(),m_titleAsText,HighlightedItem::Topics,
            FALSE /* additionalIndices*/, QCString() /*altSidebarName*/, hierarchyLevel);

  ol.startHeaderSection();
  bool writeOutlinePanel = generateTreeView && Config_getBool(PAGE_OUTLINE_PANEL);
  if (!writeOutlinePanel) writeSummaryLinks(ol);
  ol.startTitleHead(getOutputFileBase());
  //1.{
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.generateDoc(docFile(),
                 getStartBodyLine(),
                 this,
                 nullptr,
                 m_title,
                 DocOptions()
                 .setIndexWords(true)
                 .setSingleLine(true)
                 .setAutolinkSupport(false));
  ol.popGeneratorState();
  //1.}
  addGroupListToTitle(ol,this);
  //2.{
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.endTitleHead(getOutputFileBase(),m_titleAsText);
  ol.popGeneratorState();
  //2.}
  //3.{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Man);
  ol.endTitleHead(getOutputFileBase(),name());
  if (!m_titleAsText.isEmpty())
  {
    ol.writeString(" - ");
    ol.parseText(m_titleAsText);
  }
  ol.popGeneratorState();
  //3.}
  ol.endHeaderSection();
  ol.startContents();

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang=getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
  {
    const LayoutDocEntrySection *ls = dynamic_cast<const LayoutDocEntrySection*>(lde.get());
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc:
        writeBriefDescription(ol);
        break;
      case LayoutDocEntry::MemberDeclStart:
        startMemberDeclarations(ol);
        break;
      case LayoutDocEntry::GroupClasses:
        if (ls) writeClasses(ol,ls->title(lang));
        break;
      case LayoutDocEntry::GroupConcepts:
        if (ls) writeConcepts(ol,ls->title(lang));
        break;
      case LayoutDocEntry::GroupModules:
        if (ls) writeModules(ol,ls->title(lang));
        break;
      case LayoutDocEntry::GroupInlineClasses:
        writeInlineClasses(ol);
        break;
      case LayoutDocEntry::GroupNamespaces:
        if (ls) writeNamespaces(ol,ls->title(lang));
        break;
      case LayoutDocEntry::MemberGroups:
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
          if (lmd)
          {
            writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
          }
        }
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        if (ls) writeDetailedDescription(ol,ls->title(lang));
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
          if (lmd)
          {
            writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
          }
        }
        break;
      case LayoutDocEntry::MemberDefEnd:
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::GroupNestedGroups:
        if (ls) writeNestedGroups(ol,ls->title(lang));
        break;
      case LayoutDocEntry::GroupPageDocs:
        writePageDocumentation(ol);
        break;
      case LayoutDocEntry::GroupDirs:
        if (ls) writeDirs(ol,ls->title(lang));
        break;
      case LayoutDocEntry::GroupFiles:
        if (ls) writeFiles(ol,ls->title(lang));
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
      case LayoutDocEntry::NamespaceConcepts:
      case LayoutDocEntry::NamespaceInterfaces:
      case LayoutDocEntry::NamespaceStructs:
      case LayoutDocEntry::NamespaceExceptions:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::ConceptDefinition:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileConcepts:
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
      case LayoutDocEntry::ModuleExports:
      case LayoutDocEntry::ModuleClasses:
      case LayoutDocEntry::ModuleConcepts:
      case LayoutDocEntry::ModuleUsedFiles:
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member '{}' should not be part of "
            "LayoutDocManager::Group entry list\n",lde->entryToString());
        break;
    }
  }

  //---------------------------------------- end flexible part -------------------------------

  for (auto &subgd : getSubGroups())
  {
    if (!subgd->isReference())
    {
      if (subgd->partOfGroups().front() == this)
      {
        ol.writePageLink(subgd->getOutputFileBase(), FALSE);
      }
      else
      {
        // Could write a note explaining that the subgroup belongs to another
        // group and add a link here.
      }
    }
  }
  if (generateTreeView && Config_getBool(PAGE_OUTLINE_PANEL))
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.endContents();
    ol.writeString("</div><!-- doc-content -->\n");
    writePageNavigation(ol);
    ol.writeString("</div><!-- container -->\n");
    ol.popGeneratorState();
    endFile(ol,true,true);
  }
  else
  {
    endFile(ol);
  }

  ol.popGeneratorState();

  if (Config_getBool(SEPARATE_MEMBER_PAGES))
  {
    m_allMemberList.sort();
    writeMemberPages(ol, hierarchyLevel + 1);
  }
}

void GroupDefImpl::writeMemberPages(OutputList &ol, int hierarchyLevel)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  for (const auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
       ml->writeDocumentationPage(ol,name(),this,hierarchyLevel);
    }
  }

  ol.popGeneratorState();
}

void GroupDefImpl::writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const
{
  bool createSubDirs=Config_getBool(CREATE_SUBDIRS);

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  for (const auto *md : m_allMemberList)
  {
    if (md->getGroupDef()==this && md->isLinkable() && !md->isEnumValue())
    {
      if (md->isLinkableInProject())
      {
        QCString fn = md->getOutputFileBase();
        addHtmlExtensionIfMissing(fn);
        if (md==currentMd) // selected item => highlight
        {
          ol.writeString("          <tr><td class=\"navtabHL\">");
        }
        else
        {
          ol.writeString("          <tr><td class=\"navtab\">");
        }
        ol.writeString("<span class=\"label\"><a ");
        ol.writeString("href=\"");
        if (createSubDirs) ol.writeString("../../");
        ol.writeString(fn+"#"+md->anchor());
        ol.writeString("\">");
        ol.writeString(convertToHtml(md->localName()));
        ol.writeString("</a></span>");
        ol.writeString("</td></tr>\n");
      }
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
    GroupDef *gd=nullptr;
    if (!g.groupname.isEmpty()) gd=Doxygen::groupLinkedMap->find(g.groupname);
    if (gd && gd->addClass(cd))
    {
      ClassDefMutable *cdm = toClassDefMutable(cd);
      if (cdm)
      {
        cdm->makePartOfGroup(gd);
      }
      //printf("Compound %s: in group %s\n",qPrint(cd->name()),gd->groupTitle());
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}

void addConceptToGroups(const Entry *root,ConceptDef *cd)
{
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    if (gd && gd->addConcept(cd))
    {
      ConceptDefMutable *cdm = toConceptDefMutable(cd);
      if (cdm)
      {
        cdm->makePartOfGroup(gd);
      }
      //printf("Compound %s: in group %s\n",qPrint(cd->name()),gd->groupTitle());
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}

void addModuleToGroups(const Entry *root,ModuleDef *mod)
{
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    if (gd && gd->addModule(mod))
    {
      mod->makePartOfGroup(gd);
      //printf("Module %s: in group %s\n",qPrint(mod->name()),gd->groupTitle());
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}


void addNamespaceToGroups(const Entry *root,NamespaceDef *nd)
{
  //printf("root->groups.size()=%zu\n",root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=nullptr;
    if (!g.groupname.isEmpty()) gd=Doxygen::groupLinkedMap->find(g.groupname);
    //printf("group '%s' gd=%p\n",qPrint(g.groupname),(void*)gd);
    if (gd && gd->addNamespace(nd))
    {
      NamespaceDefMutable *ndm = toNamespaceDefMutable(nd);
      if (ndm)
      {
        ndm->makePartOfGroup(gd);
      }
      //printf("Namespace %s: in group %s\n",qPrint(nd->name()),qPrint(gd->name()));
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}

void addDirToGroups(const Entry *root,DirDef *dd)
{
  //printf("*** root->groups.size()=%d\n",root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    //printf("group '%s'\n",qPrint(g->groupname));
    if (gd)
    {
      gd->addDir(dd);
      dd->makePartOfGroup(gd);
      //printf("Dir %s: in group %s\n",qPrint(dd->name()),qPrint(g->groupname));
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}

void addGroupToGroups(const Entry *root,GroupDef *subGroup)
{
  //printf("addGroupToGroups for %s groups=%d\n",qPrint(root->name),root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    if (gd)
    {
      if (gd==subGroup)
      {
        warn(root->fileName,root->startLine,"Refusing to add group {} to itself",
            gd->name());
      }
      else if (subGroup->findGroup(gd))
      {
        warn(root->fileName,root->startLine,"Refusing to add group {} to group {}, since the latter is already a "
                                            "subgroup of the former", subGroup->name(),gd->name());
      }
      else if (!gd->findGroup(subGroup))
      {
        gd->addGroup(subGroup);
        subGroup->makePartOfGroup(gd);
      }
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}

/*! Add a member to the group with the highest priority */
void addMemberToGroups(const Entry *root,MemberDef *md)
{
  //printf("addMemberToGroups:  Root %p = %s, md %p=%s groups=%zu\n",
  //    root, qPrint(root->name), md, qPrint(md->name()), root->groups.size() );

  // Search entry's group list for group with highest pri.
  Grouping::GroupPri_t pri = Grouping::GROUPING_LOWEST;
  GroupDef *fgd=nullptr;
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=nullptr;
    if (!g.groupname.isEmpty()) gd=Doxygen::groupLinkedMap->find(g.groupname);
    if (gd && g.pri >= pri)
    {
      if (fgd && gd!=fgd && g.pri==pri)
      {
        warn(root->fileName, root->startLine,
            "Member {} found in multiple {} groups! "
            "The member will be put in group {}, and not in group {}",
            md->name(), Grouping::getGroupPriName( pri ),
            gd->name(), fgd->name()
            );
      }

      fgd = gd;
      pri = g.pri;
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
  //printf("fgd=%p\n",fgd);

  // put member into group defined by this entry?
  if (fgd)
  {
    GroupDef *mgd = md->getGroupDef();
    //printf("mgd=%p\n",mgd);
    bool insertit = FALSE;
    if (mgd==nullptr)
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
                "Member documentation for {} found several times in {} groups!\n"
                "{}:{}: The member will remain in group {}, and won't be put into group {}",
                md->name(), Grouping::getGroupPriName( pri ),
                root->fileName, root->startLine,
                mgd->name(), fgd->name()
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
      //    qPrint(md->getDefFileName()),md->getDefLine(),
      //    qPrint(md->name()),qPrint(root->relates));
      bool success = fgd->insertMember(md);
      if (success)
      {
        MemberDefMutable *mdm = toMemberDefMutable(md);
        if (mdm)
        {
          //printf("insertMember successful\n");
          mdm->setGroupDef(fgd,pri,root->fileName,root->startLine,!root->doc.isEmpty());
          ClassDefMutable *cdm = toClassDefMutable(mdm->getClassDefOfAnonymousType());
          if (cdm)
          {
            cdm->setGroupDefForAllMembers(fgd,pri,root->fileName,root->startLine,root->doc.length() != 0);
          }
          if (mdm->isEnumerate() && mdm->getGroupDef() && md->isStrong())
          {
            for (const auto &emd : mdm->enumFieldList())
            {
              MemberDefMutable *emdm = toMemberDefMutable(emd);
              if (emdm && emdm->getGroupDef()==nullptr)
              {
                emdm->setGroupDef(mdm->getGroupDef(),mdm->getGroupPri(),
                                 mdm->getGroupFileName(),mdm->getGroupStartLine(),
                                 mdm->getGroupHasDocs());
              }
            }
          }
        }
      }
    }
  }
}


void addExampleToGroups(const Entry *root,PageDef *eg)
{
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    if (gd)
    {
      gd->addExample(eg);
      eg->makePartOfGroup(gd);
      //printf("Example %s: in group %s\n",qPrint(eg->name()),s->data());
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '{}' for the command '{}', ignoring command",
          g.groupname, Grouping::getGroupPriName( g.pri )
          );
    }
  }
}

QCString GroupDefImpl::getOutputFileBase() const
{
  return m_fileName;
}

void GroupDefImpl::addListReferences()
{
  addRefItem(xrefListItems(),
             getOutputFileBase(),
             theTranslator->trGroup(TRUE,TRUE),
             getOutputFileBase(),name(),
             QCString(),
             nullptr
            );
  for (const auto &mg : m_memberGroups)
  {
    mg->addListReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
      ml->addListReferences(this);
    }
  }
}

void GroupDefImpl::addRequirementReferences()
{
  RequirementManager::instance().addRequirementRefsForSymbol(this);
  for (const auto &mg : m_memberGroups)
  {
    mg->addRequirementReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType().isDocumentation())
    {
      ml->addRequirementReferences(this);
    }
  }
}

void GroupDefImpl::addMemberToList(MemberListType lt,MemberDef *md)
{
  bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_memberLists.get(lt,MemberListContainer::Group);
  ml->setNeedsSorting(
      (ml->listType().isDeclaration() && sortBriefDocs) ||
      (ml->listType().isDocumentation() && sortMemberDocs));
  ml->push_back(md);
}

// performs a partial reordering to group elements together with the same scope
template<class Vec>
static void groupClassesWithSameScope(Vec &vec)
{
  bool done=false;
  while (!done) // for each iteration
  {
    done=true;
    for (size_t i=0; i<vec.size(); i++) // go through all items
    {
      std::string qni = vec[i]->name().str();
      size_t posi = qni.rfind("::");
      if (posi!=std::string::npos)
      {
        std::string scope = qni.substr(0,posi);
        auto it = std::find_if( vec.begin(), vec.end(),
            [&](typename Vec::Ptr &cd)
            { return cd->name().str()==scope; });
        if (it!=vec.end())
        {
          size_t idx = std::distance(vec.begin(),it);
          if (i<idx) // parent scope located after child scope
          {
            // to avoid reordering elements with the same parent
            // we skip to the last one with the same scope
            size_t k = idx;
            while (k<vec.size() && vec[k]->name().str().substr(0,posi)==scope)
            {
              idx = k;
              k++;
            }
            // swap the items such that i is inserted after idx
            for (size_t j=i; j<idx; j++)
            {
              std::swap(vec[j],vec[j+1]);
            }
            done=false;
          }
          else if (idx<i && vec[i-1]->name().str().substr(0,posi)!=scope)
          {
            // parent scope is found before the item, and the item
            // has some other item with a different scope in front of it
            // move idx to the end of range with the same scope
            while (idx<i && vec[idx]->name().str().substr(0,posi)==scope)
            {
              idx++;
            }
            // swap the items such that i is just after idx
            for (size_t j=idx; j<i; j++)
            {
              std::swap(vec[j],vec[j+1]);
            }
            done=false;
          }
        }
      }
    }
  }
}

void GroupDefImpl::sortMemberLists()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
  }
  if (Config_getBool(SORT_BRIEF_DOCS))
  {
    std::stable_sort(m_dirList.begin(), m_dirList.end(), compareDirDefs);

    auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
    {
      return Config_getBool(SORT_BY_SCOPE_NAME)     ?
        qstricmp_sort(c1->name(), c2->name())<0          :
        qstricmp_sort(c1->className(), c2->className())<0;
    };
    std::stable_sort(m_classes.begin(), m_classes.end(), classComp);

    auto namespaceComp = [](const NamespaceLinkedRefMap::Ptr &n1,const NamespaceLinkedRefMap::Ptr &n2)
    {
      return qstricmp_sort(n1->name(),n2->name())<0;
    };

    std::stable_sort(m_namespaces.begin(),m_namespaces.end(),namespaceComp);

    auto moduleComp = [](const ModuleLinkedRefMap::Ptr &m1,const ModuleLinkedRefMap::Ptr &m2)
    {
      return qstricmp_sort(m1->name(),m2->name())<0;
    };

    std::stable_sort(m_modules.begin(), m_modules.end(), moduleComp);

    auto conceptComp = [](const ConceptLinkedRefMap::Ptr &c1,const ConceptLinkedRefMap::Ptr &c2)
    {
      return qstricmp_sort(c1->name(),c2->name())<0;
    };

    std::stable_sort(m_concepts.begin(), m_concepts.end(), conceptComp);

    std::stable_sort(m_dirList.begin(), m_dirList.end(), compareDirDefs);
    std::stable_sort(m_fileList.begin(), m_fileList.end(), compareFileDefs);

  }
  else
  {
    groupClassesWithSameScope(m_classes);
    groupClassesWithSameScope(m_namespaces);
  }
}

MemberList *GroupDefImpl::getMemberList(MemberListType lt) const
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()==lt)
    {
      return ml.get();
    }
  }
  return nullptr;
}

void GroupDefImpl::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title)
{
  bool optimizeVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);

  MemberList * ml = getMemberList(lt);
  if (optimizeVhdl && ml)
  {
    VhdlDocGen::writeVhdlDeclarations(ml,ol,this,nullptr,nullptr,nullptr,nullptr);
    return;
  }
  if (ml)
  {
    ml->writeDeclarations(ol,nullptr,nullptr,nullptr,this,nullptr,title,QCString());
  }
}

void GroupDefImpl::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title,ml->listType().toLabel());
}

void GroupDefImpl::removeMemberFromList(MemberListType lt,MemberDef *md)
{
  MemberList *ml = getMemberList(lt);
  if (ml) ml->remove(md);
}

void GroupDefImpl::sortSubGroups()
{
  std::stable_sort(m_groups.begin(),
            m_groups.end(),
            [](const auto &g1,const auto &g2)
            { return g1->groupTitle() < g2->groupTitle(); });
}

static bool hasNonReferenceNestedGroupRec(const GroupDef *gd,int level)
{
  if (level>30)
  {
    err("Possible recursive group relation while inside {}\n",gd->name());
    return false;
  }
  bool found=gd->isLinkableInProject();
  if (found)
  {
    return true;
  }
  else
  {
    for (const auto &igd : gd->getSubGroups())
    {
      found = found || hasNonReferenceNestedGroupRec(igd,level+1);
      if (found) break;
    }
  }
  return found;
}

bool GroupDefImpl::isVisibleInHierarchy() const
{
  bool allExternals = Config_getBool(EXTERNAL_GROUPS);
  return (allExternals || hasNonReferenceNestedGroupRec(this,0)) && isLinkable();
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
  if (getLanguage()==SrcLangExt::Unknown && d->getLanguage()!=SrcLangExt::Unknown)
  {
    setLanguage(d->getLanguage());
  }
}

bool GroupDefImpl::hasDetailedDescription() const
{
  bool repeatBrief = Config_getBool(REPEAT_BRIEF);
  return ((!briefDescription().isEmpty() && repeatBrief) ||
         !documentation().isEmpty() ||
         !inbodyDocumentation().isEmpty() ||
         hasRequirementRefs()) &&
         (m_pages.size()!=numDocMembers());
}

void GroupDefImpl::overrideGroupGraph(bool e)
{
  m_hasGroupGraph=e;
}

bool GroupDefImpl::hasGroupGraph() const
{
  return m_hasGroupGraph;
}

// --- Cast functions

GroupDef *toGroupDef(Definition *d)
{
  if (d==nullptr) return nullptr;
  if (d && typeid(*d)==typeid(GroupDefImpl))
  {
    return static_cast<GroupDef*>(d);
  }
  else
  {
    return nullptr;
  }
}

const GroupDef *toGroupDef(const Definition *d)
{
  if (d==nullptr) return nullptr;
  if (d && typeid(*d)==typeid(GroupDefImpl))
  {
    return static_cast<const GroupDef*>(d);
  }
  else
  {
    return nullptr;
  }
}


