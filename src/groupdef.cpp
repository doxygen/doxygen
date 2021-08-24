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

//---------------------------------------------------------------------------

class GroupDefImpl : public DefinitionMixin<GroupDef>
{
  public:
    GroupDefImpl(const QCString &fileName,int line,const QCString &name,const QCString &title,const QCString &refFileName=QCString());
    virtual ~GroupDefImpl();

    virtual DefType definitionType() const { return TypeGroup; }
    virtual CodeSymbolType codeSymbolType() const { return CodeSymbolType::Default; }
    virtual QCString getOutputFileBase() const;
    virtual QCString anchor() const { return QCString(); }
    virtual QCString displayName(bool=TRUE) const { return hasGroupTitle() ? m_title : DefinitionMixin::name(); }
    virtual QCString groupTitle() const { return m_title; }
    virtual void setGroupTitle( const QCString &newtitle );
    virtual bool hasGroupTitle( ) const { return m_titleSet; }
    virtual void addFile(const FileDef *def);
    virtual bool addClass(const ClassDef *def);
    virtual bool addConcept(const ConceptDef *def);
    virtual bool addNamespace(const NamespaceDef *def);
    virtual void addGroup(const GroupDef *def);
    virtual void addPage(const PageDef *def);
    virtual void addExample(const PageDef *def);
    virtual void addDir(DirDef *dd);
    virtual bool insertMember(const MemberDef *def,bool docOnly=FALSE);
    virtual void removeMember(MemberDef *md);
    virtual bool findGroup(const GroupDef *def) const; // true if def is a subgroup of this group
    virtual void writeDocumentation(OutputList &ol);
    virtual void writeMemberPages(OutputList &ol);
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *currentMd) const;
    virtual void writeTagFile(TextStream &);
    virtual size_t numDocMembers() const;
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
    virtual const MemberLists &getMemberLists() const { return m_memberLists; }

    /* user defined member groups */
    virtual const MemberGroupList &getMemberGroups() const { return m_memberGroups; }

    virtual const FileList &getFiles() const                    { return m_fileList; }
    virtual const ClassLinkedRefMap &getClasses() const         { return m_classes; }
    virtual const ConceptLinkedRefMap &getConcepts() const      { return m_concepts; }
    virtual const NamespaceLinkedRefMap &getNamespaces() const  { return m_namespaces; }
    virtual const GroupList &getSubGroups() const               { return m_groups; }
    virtual const PageLinkedRefMap &getPages() const            { return m_pages; }
    virtual const DirList & getDirs() const                     { return m_dirList; }
    virtual const PageLinkedRefMap &getExamples() const         { return m_examples; }
    virtual bool hasDetailedDescription() const;
    virtual void sortSubGroups();

  private:
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);
    void addMemberToList(MemberListType lt,const MemberDef *md);
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
    FileList             m_fileList;            // list of files in the group
    ClassLinkedRefMap    m_classes;             // list of classes in the group
    ConceptLinkedRefMap  m_concepts;            // list of concepts in the group
    NamespaceLinkedRefMap m_namespaces;         // list of namespaces in the group
    GroupList            m_groups;              // list of sub groups.
    PageLinkedRefMap     m_pages;               // list of pages in the group
    PageLinkedRefMap     m_examples;            // list of examples in the group
    DirList              m_dirList;             // list of directories in the group
    MemberList           m_allMemberList;
    MemberNameInfoLinkedMap m_allMemberNameInfoLinkedMap;
    Definition *         m_groupScope;
    MemberLists          m_memberLists;
    MemberGroupList      m_memberGroups;
    bool                 m_subGrouping;

};

GroupDef *createGroupDef(const QCString &fileName,int line,const QCString &name,
                                const QCString &title,const QCString &refFileName)
{
  return new GroupDefImpl(fileName,line,name,title,refFileName);
}


//---------------------------------------------------------------------------

GroupDefImpl::GroupDefImpl(const QCString &df,int dl,const QCString &na,const QCString &t,
                   const QCString &refFileName) : DefinitionMixin(df,dl,1,na),
                    m_allMemberList(MemberListType_allMembersList,MemberListContainer::Group)
{
  if (!refFileName.isEmpty())
  {
    m_fileName=stripExtension(refFileName);
  }
  else
  {
    m_fileName = convertNameToFile(QCString("group_")+na);
  }
  setGroupTitle( t );

  //visited = 0;
  m_groupScope = 0;
  m_subGrouping=Config_getBool(SUBGROUPING);
}

GroupDefImpl::~GroupDefImpl()
{
}

void GroupDefImpl::setGroupTitle( const QCString &t )
{
  if ( !t.isEmpty())
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
  for (const auto &mg : m_memberGroups)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void GroupDefImpl::findSectionsInDocumentation()
{
  docFindSections(briefDescription(),this,docFile());
  docFindSections(documentation(),this,docFile());

  for (const auto &mg : m_memberGroups)
  {
    mg->findSectionsInDocumentation(this);
  }

  for (auto &ml : m_memberLists)
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
    m_fileList.insert( std::upper_bound( m_fileList.begin(), m_fileList.end(), def,
                                         [](const auto &fd1, const auto &fd2)
                                         { return qstricmp(fd1->name(),fd2->name())<0; }),
                       def);
  else
    m_fileList.push_back(def);
}

bool GroupDefImpl::addClass(const ClassDef *cd)
{
  if (cd->isHidden()) return FALSE;
  updateLanguage(cd);
  QCString qn = cd->name();
  if (m_classes.find(qn)==0)
  {
    m_classes.add(qn,cd);
    return TRUE;
  }
  return FALSE;
}

bool GroupDefImpl::addConcept(const ConceptDef *cd)
{
  if (cd->isHidden()) return FALSE;
  QCString qn = cd->name();
  if (m_concepts.find(qn)==0)
  {
    m_concepts.add(qn,cd);
    return TRUE;
  }
  return FALSE;
}

bool GroupDefImpl::addNamespace(const NamespaceDef *def)
{
  //printf("adding namespace hidden=%d\n",def->isHidden());
  if (def->isHidden()) return false;
  if (m_namespaces.find(def->name())==0)
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

void GroupDefImpl::addPage(const PageDef *def)
{
  if (def->isHidden()) return;
  //printf("Making page %s part of a group\n",qPrint(def->name));
  m_pages.add(def->name(),def);
  const_cast<PageDef*>(def)->makePartOfGroup(this);
}

void GroupDefImpl::addExample(const PageDef *def)
{
  if (def->isHidden()) return;
  m_examples.add(def->name(),def);
}


void GroupDefImpl::addMembersToMemberGroup()
{
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_declarationLists)
    {
      ::addMembersToMemberGroup(ml.get(),&m_memberGroups,this);
    }
  }
}


bool GroupDefImpl::insertMember(const MemberDef *md,bool docOnly)
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
        matchArguments2(srcMd->getOuterScope(),srcMd->getFileDef(),&srcMdAl,
                        md->getOuterScope(),md->getFileDef(),&mdAl,
                        TRUE
                       ) && // matching parameters
        sameScope // both are found in the same scope
       )
    {
      MemberDefMutable *mdm = toMemberDefMutable(md);
      if (mdm && srcMd->getGroupAlias()==0)
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
  mni->push_back(std::make_unique<MemberInfo>(md,md->protection(),md->virtualness(),FALSE));
  //printf("Added member!\n");
  m_allMemberList.push_back(md);
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
           qPrint(md->name()),md->memberType(),
           md->getClassDef() ? qPrint(md->getClassDef()->name()) : "",
           qPrint(name()));
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
  for (const auto &gd : m_groups)
  {
    if (gd->findGroup(def))
    {
      return TRUE;
    }
  }
  return FALSE;
}

void GroupDefImpl::addGroup(const GroupDef *def)
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
  tagFile << "  <compound kind=\"group\">\n";
  tagFile << "    <name>" << convertToXML(name()) << "</name>\n";
  tagFile << "    <title>" << convertToXML(m_title) << "</title>\n";
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>\n";
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
          const LayoutDocEntryMemberDecl *lmd = (const LayoutDocEntryMemberDecl*)lde.get();
          MemberList * ml = getMemberList(lmd->type);
          if (ml)
          {
            ml->writeTagFile(tagFile);
          }
        }
        break;
      case LayoutDocEntry::MemberGroups:
        {
          for (const auto &mg : m_memberGroups)
          {
            mg->writeTagFile(tagFile);
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
  if ((!briefDescription().isEmpty() && Config_getBool(REPEAT_BRIEF))
      || !documentation().isEmpty() || !inbodyDocumentation().isEmpty()
     )
  {
    ol.pushGeneratorState();
    if (m_pages.size()!=numDocMembers()) // not only pages -> classical layout
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Html);
        ol.writeRuler();
      ol.popGeneratorState();
      ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        ol.writeAnchor(QCString(),"details");
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
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }

    // write inbody documentation
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),this,0,inbodyDocumentation()+"\n",TRUE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    }
  }
}

void GroupDefImpl::writeBriefDescription(OutputList &ol)
{
  if (hasBriefDescription())
  {
    std::unique_ptr<IDocParser> parser { createDocParser() };
    std::unique_ptr<DocRoot>  rootNode { validatingParseDoc(*parser.get(),
                                         briefFile(),briefLine(),this,0,
                                         briefDescription(),TRUE,FALSE,
                                         QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
    if (rootNode && !rootNode->isEmpty())
    {
      ol.startParagraph();
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Man);
      ol.writeString(" - ");
      ol.popGeneratorState();
      ol.writeDoc(rootNode.get(),this,0);
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.writeString(" \n");
      ol.enable(OutputGenerator::RTF);

      if (hasDetailedDescription())
      {
        ol.disableAllBut(OutputGenerator::Html);
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
  if (Config_getBool(HAVE_DOT) /*&& Config_getBool(GROUP_GRAPHS)*/ )
  {
    DotGroupCollaboration graph(this);
    if (!graph.isTrivial())
    {
      msg("Generating dependency graph for group %s\n",qPrint(qualifiedName()));
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
      ol.startMemberItem(fd->getOutputFileBase(),0);
      ol.docify(theTranslator->trFile(FALSE,TRUE)+" ");
      ol.insertMemberAlign();
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),fd->name());
      ol.endMemberItem();
      if (!fd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(fd->getOutputFileBase());
        ol.generateDoc(briefFile(),briefLine(),fd,0,fd->briefDescription(),FALSE,FALSE,
                       QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
        ol.startMemberItem(gd->getOutputFileBase(),0);
        //ol.docify(theTranslator->trGroup(FALSE,TRUE));
        //ol.docify(" ");
        ol.insertMemberAlign();
        ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),QCString(),gd->groupTitle());
        ol.endMemberItem();
        if (!gd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
        {
          ol.startMemberDescription(gd->getOutputFileBase());
          ol.generateDoc(briefFile(),briefLine(),gd,0,gd->briefDescription(),FALSE,FALSE,
                         QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
      ol.startMemberItem(dd->getOutputFileBase(),0);
      ol.parseText(theTranslator->trDir(FALSE,TRUE));
      ol.insertMemberAlign();
      ol.writeObjectLink(dd->getReference(),dd->getOutputFileBase(),QCString(),dd->shortName());
      ol.endMemberItem();
      if (!dd->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
      {
        ol.startMemberDescription(dd->getOutputFileBase());
        ol.generateDoc(briefFile(),briefLine(),dd,0,dd->briefDescription(),FALSE,FALSE,
                       QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
  m_classes.writeDeclaration(ol,0,title,FALSE);
}

void GroupDefImpl::writeConcepts(OutputList &ol,const QCString &title)
{
  // write list of concepts
  m_concepts.writeDeclaration(ol,title,FALSE);
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
      const SectionInfo *si=0;
      if (pd->hasTitle() && !pd->name().isEmpty() &&
          (si=SectionManager::instance().find(pd->name()))!=0)
      {
        ol.startSection(si->label(),si->title(),SectionType::Subsection);
        ol.docify(si->title());
        ol.endSection(si->label(),SectionType::Subsection);
      }
      ol.startTextBlock();
      ol.generateDoc(pd->docFile(),pd->docLine(),pd,0,(pd->documentation()+pd->inbodyDocumentation()),TRUE,FALSE,
                     QCString(),TRUE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
      ol.endTextBlock();
    }
  }
}

void GroupDefImpl::writeMemberGroups(OutputList &ol)
{
  /* write user defined member groups */
  for (const auto &mg : m_memberGroups)
  {
    mg->writeDeclarations(ol,0,0,0,this);
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
  bool first=TRUE;
  SrcLangExt lang = getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
  {
    if ((lde->kind()==LayoutDocEntry::GroupClasses      &&  m_classes.declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupConcepts     &&  m_concepts.declVisible()) ||
        (lde->kind()==LayoutDocEntry::GroupNamespaces   &&  m_namespaces.declVisible(false)) ||
        (lde->kind()==LayoutDocEntry::GroupFiles        && !m_fileList.empty()) ||
        (lde->kind()==LayoutDocEntry::GroupNestedGroups && !m_groups.empty()) ||
        (lde->kind()==LayoutDocEntry::GroupDirs         && !m_dirList.empty())
       )
    {
      const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
      QCString label = lde->kind()==LayoutDocEntry::GroupClasses      ? "nested-classes" :
                       lde->kind()==LayoutDocEntry::GroupConcepts     ? "concepts"       :
                       lde->kind()==LayoutDocEntry::GroupNamespaces   ? "namespaces"     :
                       lde->kind()==LayoutDocEntry::GroupFiles        ? "files"          :
                       lde->kind()==LayoutDocEntry::GroupNestedGroups ? "groups"         :
                       "dirs";
      ol.writeSummaryLink(QCString(),label,ls->title(lang),first);
      first=FALSE;
    }
    else if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = (const LayoutDocEntryMemberDecl*)lde.get();
      MemberList * ml = getMemberList(lmd->type);
      if (ml && ml->declVisible())
      {
        ol.writeSummaryLink(QCString(),MemberList::listTypeAsString(ml->listType()),lmd->title(lang),first);
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
  if (!m_title.isEmpty())
  {
    ol.writeString(" - ");
    ol.parseText(m_title);
  }
  ol.popGeneratorState();
  ol.endHeaderSection();
  ol.startContents();

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang=getLanguage();
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
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
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeClasses(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::GroupConcepts:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeConcepts(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::GroupInlineClasses:
        {
          writeInlineClasses(ol);
        }
        break;
      case LayoutDocEntry::GroupNamespaces:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeNamespaces(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberGroups:
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl:
        {
          const LayoutDocEntryMemberDecl *lmd = (const LayoutDocEntryMemberDecl*)lde.get();
          writeMemberDeclarations(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDeclEnd:
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeDetailedDescription(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefStart:
        startMemberDocumentation(ol);
        break;
      case LayoutDocEntry::MemberDef:
        {
          const LayoutDocEntryMemberDef *lmd = (const LayoutDocEntryMemberDef*)lde.get();
          writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDefEnd:
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::GroupNestedGroups:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeNestedGroups(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::GroupPageDocs:
        writePageDocumentation(ol);
        break;
      case LayoutDocEntry::GroupDirs:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
          writeDirs(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::GroupFiles:
        {
          const LayoutDocEntrySection *ls = (const LayoutDocEntrySection*)lde.get();
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
    m_allMemberList.sort();
    writeMemberPages(ol);
  }

}

void GroupDefImpl::writeMemberPages(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  for (const auto &ml : m_memberLists)
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

  for (const auto *md : m_allMemberList)
  {
    if (md->getGroupDef()==this && md->isLinkable() && !md->isEnumValue())
    {
      if (md->isLinkableInProject())
      {
        if (md==currentMd) // selected item => highlight
        {
          ol.writeString("          <tr><td class=\"navtabHL\">");
        }
        else
        {
          ol.writeString("          <tr><td class=\"navtab\">");
        }
        ol.writeString("<a class=\"navtab\" ");
        ol.writeString("href=\"");
        if (createSubDirs) ol.writeString("../../");
        ol.writeString(md->getOutputFileBase()+Doxygen::htmlFileExtension+"#"+md->anchor());
        ol.writeString("\">");
        ol.writeString(convertToHtml(md->localName()));
        ol.writeString("</a>");
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
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    if (gd && gd->addClass(cd))
    {
      ClassDefMutable *cdm = toClassDefMutable(cd);
      if (cdm)
      {
        cdm->makePartOfGroup(gd);
      }
      //printf("Compound %s: in group %s\n",qPrint(cd->name()),gd->groupTitle());
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
  }
}


void addNamespaceToGroups(const Entry *root,NamespaceDef *nd)
{
  //printf("root->groups.size()=%zu\n",root->groups.size());
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
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
        warn(root->fileName,root->startLine,"Refusing to add group %s to itself",
            qPrint(gd->name()));
      }
      else if (subGroup->findGroup(gd))
      {
        warn(root->fileName,root->startLine,"Refusing to add group %s to group %s, since the latter is already a "
                                            "subgroup of the former\n", qPrint(subGroup->name()),qPrint(gd->name()));
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
  //printf("addMemberToGroups:  Root %p = %s, md %p=%s groups=%zu\n",
  //    root, qPrint(root->name), md, qPrint(md->name()), root->groups.size() );

  // Search entry's group list for group with highest pri.
  Grouping::GroupPri_t pri = Grouping::GROUPING_LOWEST;
  GroupDef *fgd=0;
  for (const Grouping &g : root->groups)
  {
    GroupDef *gd=0;
    if (!g.groupname.isEmpty() &&
        (gd=Doxygen::groupLinkedMap->find(g.groupname)) &&
        g.pri >= pri)
    {
      if (fgd && gd!=fgd && g.pri==pri)
      {
        warn(root->fileName, root->startLine,
            "Member %s found in multiple %s groups! "
            "The member will be put in group %s, and not in group %s",
            qPrint(md->name()), Grouping::getGroupPriName( pri ),
            qPrint(gd->name()), qPrint(fgd->name())
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
                qPrint(md->name()), Grouping::getGroupPriName( pri ),
                qPrint(root->fileName), root->startLine,
                qPrint(mgd->name()),
                qPrint(fgd->name())
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
              if (emdm && emdm->getGroupDef()==0)
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
             QCString(),
             0
            );
  }
  for (const auto &mg : m_memberGroups)
  {
    mg->addListReferences(this);
  }
  for (auto &ml : m_memberLists)
  {
    if (ml->listType()&MemberListType_documentationLists)
    {
      ml->addListReferences(this);
    }
  }
}

void GroupDefImpl::addMemberToList(MemberListType lt,const MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool(SORT_BRIEF_DOCS);
  static bool sortMemberDocs = Config_getBool(SORT_MEMBER_DOCS);
  const auto &ml = m_memberLists.get(lt,MemberListContainer::Group);
  ml->setNeedsSorting(
      ((ml->listType()&MemberListType_declarationLists) && sortBriefDocs) ||
      ((ml->listType()&MemberListType_documentationLists) && sortMemberDocs));
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
    std::sort(m_dirList.begin(), m_dirList.end(), compareDirDefs);

    auto classComp = [](const ClassLinkedRefMap::Ptr &c1,const ClassLinkedRefMap::Ptr &c2)
    {
      return Config_getBool(SORT_BY_SCOPE_NAME)     ?
        qstricmp(c1->name(), c2->name())<0          :
        qstricmp(c1->className(), c2->className())<0;
    };
    std::sort(m_classes.begin(), m_classes.end(), classComp);

    auto namespaceComp = [](const NamespaceLinkedRefMap::Ptr &n1,const NamespaceLinkedRefMap::Ptr &n2)
    {
      return qstricmp(n1->name(),n2->name())<0;
    };

    std::sort(m_namespaces.begin(),m_namespaces.end(),namespaceComp);
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
    ml->writeDeclarations(ol,0,0,0,this,title,QCString());
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
  std::sort(m_groups.begin(),
            m_groups.end(),
            [](const auto &g1,const auto &g2)
            { return g1->groupTitle() < g2->groupTitle(); });
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
         !documentation().isEmpty() ||
         !inbodyDocumentation().isEmpty()) &&
         (m_pages.size()!=numDocMembers());
}

// --- Cast functions

GroupDef *toGroupDef(Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(GroupDefImpl))
  {
    return static_cast<GroupDef*>(d);
  }
  else
  {
    return 0;
  }
}

const GroupDef *toGroupDef(const Definition *d)
{
  if (d==0) return 0;
  if (d && typeid(*d)==typeid(GroupDefImpl))
  {
    return static_cast<const GroupDef*>(d);
  }
  else
  {
    return 0;
  }
}


