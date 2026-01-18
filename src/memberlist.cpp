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

#include "memberlist.h"
#include "classdef.h"
#include "message.h"
#include "util.h"
#include "language.h"
#include "doxygen.h"
#include "outputlist.h"
#include "groupdef.h"
#include "vhdldocgen.h"
#include "namespacedef.h"
#include "filedef.h"
#include "membergroup.h"
#include "config.h"
#include "docparser.h"
#include "moduledef.h"

MemberList::MemberList(MemberListType lt,MemberListContainer con) : m_container(con), m_listType(lt)
{
  //printf("%p: MemberList::MemberList(%d)\n",this,lt);
  m_numDecMembers=-1; // special value indicating that value needs to be computed
  m_numDecEnumValues=0;
  m_numDocMembers=-1; // special value indicating that value needs to be computed
  m_numDocEnumValues=0;
  m_needsSorting=FALSE;
}

MemberList::~MemberList()
{
}

int genericCompareMembers(const MemberDef *c1,const MemberDef *c2)
{
  bool sortConstructorsFirst = Config_getBool(SORT_MEMBERS_CTORS_1ST);
  if (sortConstructorsFirst)
  {
    int ord1 = c1->isConstructor() ? 2 : (c1->isDestructor() ? 1 : 0);
    int ord2 = c2->isConstructor() ? 2 : (c2->isDestructor() ? 1 : 0);
    if (ord1 > ord2)
      return -1;
    else if (ord2 > ord1)
      return 1;
  }
  // sort on name, first case in-sensitive
  int cmp = qstricmp_sort(c1->name(),c2->name());
  // then on qualified name
  if (cmp==0)
  {
    cmp = qstricmp_sort(c1->qualifiedName(),c2->qualifiedName());
  }
  // then on argument list
  if (cmp==0 && !c1->argsString().isEmpty() && !c2->argsString().isEmpty())
  {
    cmp = qstricmp_sort(c1->argsString(),c2->argsString());
  }
  // then on file in which the item is defined
  if (cmp==0)
  {
    cmp = qstricmp_sort(c1->getDefFileName(),c2->getDefFileName());
  }
  // then on line number at which the member is defined
  if (cmp==0)
  {
    cmp = c2->getDefLine()-c1->getDefLine();
  }
  return cmp;
}

int MemberList::countInheritableMembers(const ClassDef *inheritedFrom) const
{
  int count=0;
  for (const auto &md : m_members)
  {
    if (md->isBriefSectionVisible())
    {
      if (md->memberType()!=MemberType::Friend &&
          md->memberType()!=MemberType::EnumValue)
      {
        //printf("member %s: isReimplementedBy(%s)=%d\n",qPrint(md->name()),
        //    qPrint(inheritedFrom->name()),
        //    md->isReimplementedBy(inheritedFrom));
        if (md->memberType()==MemberType::Function)
        {
          if (!md->isReimplementedBy(inheritedFrom)) count++;
        }
        else
        {
          count++;
        }
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    count+=mg->countInheritableMembers(inheritedFrom);
  }
  //printf("%s::countInheritableMembers(%s)=%d\n",
  //    qPrint(m_listType.toLabel()),
  //    qPrint(inheritedFrom->name()),count);
  return count;
}

/*! Count the number of members in this list that are visible in
 *  the declaration part of a compound's documentation page.
 */
std::pair<int,int> MemberList::countDecMembers(const ClassDef *inheritedFrom) const
{
  //printf("----- countDecMembers count=%d ----\n",count());
  int numDecMembers=0;
  int numDecEnumValues=0;
  for (const auto &md : m_members)
  {
    //printf("MemberList::countDecMembers(md=%s,%d)\n",qPrint(md->name()),md->isBriefSectionVisible());
    if ((inheritedFrom==nullptr || !md->isReimplementedBy(inheritedFrom)) &&
        md->isBriefSectionVisible())
    {
      switch(md->memberType())
      {
        case MemberType::Variable:    // fall through
        case MemberType::Event:       // fall through
        case MemberType::Property:   numDecMembers++;
                                     break;
// apparently necessary to get this to show up in declarations section?
        case MemberType::Interface:   // fall through
        case MemberType::Service:     // fall through
        case MemberType::Function:    // fall through
        case MemberType::Signal:      // fall through
        case MemberType::DCOP:        // fall through
        case MemberType::Slot:       if (!md->isRelated() || md->getClassDef())
                                     numDecMembers++;
                                     break;
        case MemberType::Enumeration:
                                     numDecMembers++;
                                     break;
        case MemberType::EnumValue:  numDecEnumValues++;
                                     numDecMembers++;
                                     break;
        case MemberType::Typedef:    numDecMembers++;
                                     break;
        case MemberType::Sequence:   numDecMembers++;
                                     break;
        case MemberType::Dictionary: numDecMembers++;
                                     break;
        case MemberType::Define:      if (Config_getBool(EXTRACT_ALL) ||
                                         !md->argsString().isEmpty() ||
                                         !md->initializer().isEmpty() ||
                                         md->hasDocumentation()
                                        ) numDecMembers++;
                                     break;
        case MemberType::Friend:     numDecMembers++;
                                     break;
        default:
          err("Unknown member type found for member '{}'!\n",md->name());
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->countDecMembers();
    numDecMembers+=mg->numDecMembers();
    numDecEnumValues+=mg->numDecEnumValues();
  }
  //printf("----- end countDecMembers ----\n");

  return std::make_pair(numDecMembers,numDecEnumValues);
}

void MemberList::countDecMembers() const
{
  if (m_numDecMembers!=-1) return; // already cached
  std::tie(m_numDecMembers, m_numDecEnumValues) = countDecMembers(nullptr); // cache new values
}

void MemberList::countDocMembers()
{
  if (m_numDocMembers!=-1) return; // used cached value
  m_numDocMembers=0;
  for (const auto &md : m_members)
  {
    if (md->isDetailedSectionVisible(m_container) && !md->isAlias())
    {
      // do not count enum values, since they do not produce entries of their own
      if (md->memberType()==MemberType::EnumValue)
      {
        m_numDocEnumValues++;
      }
      m_numDocMembers++;
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->countDocMembers();
    m_numDocMembers+=mg->numDocMembers();
    m_numDocEnumValues+=mg->numDocEnumValues();
  }
  //printf("MemberList::countDocMembers()=%d memberGroupList=%p\n",m_numDocMembers,memberGroupList);
}

void MemberList::setAnonymousEnumType()
{
  //printf("MemberList(%p)::setAnonymousEnumType()\n",this);
  for (const auto &md : m_members)
  {
    if (md->isBriefSectionVisible())
    {
      QCString name(md->name());
      int i=name.findRev("::");
      if (i!=-1) name=name.right(name.length()-i-2);
      if (md->memberType()==MemberType::Enumeration && name[0]=='@')
      {
        for (const auto &vmd : md->enumFieldList())
        {
          MemberDefMutable *vmdm = toMemberDefMutable(vmd);
          if (vmdm)
          {
            QCString vtype=vmd->typeString();
            if ((vtype.find(name))!=-1)
            {
              vmdm->setAnonymousEnumType(md);
            }
          }
        }
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->setAnonymousEnumType();
  }
}

int MemberList::countEnumValues(const MemberDef *md) const
{
  int numEnumValues=0;
  QCString name(md->name());
  int i=name.findRev("::");
  if (i!=-1) name=name.right(name.length()-i-2);
  if (name[0]=='@')
  {
    for (const auto &vmd : m_members)
    {
      QCString vtype=vmd->typeString();
      if ((vtype.find(name))!=-1)
      {
        numEnumValues++;
      }
    }
  }
  return numEnumValues;
}

bool MemberList::declVisible() const
{
  for (const auto &md : m_members)
  {
    if (md->isBriefSectionVisible())
    {
      switch (md->memberType())
      {
        case MemberType::Define:     // fall through
        case MemberType::Typedef:    // fall through
        case MemberType::Variable:   // fall through
        case MemberType::Function:   // fall through
        case MemberType::Signal:     // fall through
        case MemberType::Slot:       // fall through
        case MemberType::DCOP:       // fall through
        case MemberType::Property:   // fall through
        case MemberType::Interface:  // fall through
        case MemberType::Service:    // fall through
        case MemberType::Sequence:   // fall through
        case MemberType::Dictionary: // fall through
        case MemberType::Event:
          return TRUE;
        case MemberType::Enumeration:
          {
            // if this is an anonymous enum and there are variables of this
            // enum type (i.e. enumVars>0), then we do not show the enum here.
            if (countEnumValues(md)==0) // show enum here
            {
              return TRUE;
            }
          }
          break;
        case MemberType::Friend:
          return TRUE;
        case MemberType::EnumValue:
          {
            if (m_container==MemberListContainer::Group)
            {
              return TRUE;
            }
          }
          break;
      }
    }
  }
  return FALSE;
}

void MemberList::writePlainDeclarations(OutputList &ol, bool inGroup,
                       const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd, const GroupDef *gd,const ModuleDef *mod,
                       int indentLevel, const ClassDef *inheritedFrom,const QCString &inheritId
                      ) const
{
  //printf("----- writePlainDeclaration() ----\n");
  if (numDecMembers()==-1)
  {
    err("MemberList::numDecMembers()==-1, so the members of this list have not been counted. Please report as a bug.\n");
    abort();
  }
  if (numDecMembers()<=numDecEnumValues())
  {
    //printf("  --> no members!\n");
    return; // no members in this list
  }
  //printf("  --> writePlainDeclaration() numDecMembers()=%d\n",
  //    numDecMembers());

  ol.pushGeneratorState();

  bool first=TRUE;
  for (const auto &md : m_members)
  {
    //printf(">>> Member '%s' type=%d visible=%d inheritedFrom=%p inheritId=%s\n",
    //   qPrint(md->name()),md->memberType(),md->isBriefSectionVisible(),(void*)inheritedFrom,qPrint(inheritId));
    if ((inheritedFrom==nullptr || !md->isReimplementedBy(inheritedFrom)) &&
        md->isBriefSectionVisible())
    {
      //printf(">>> rendering\n");
      switch(md->memberType())
      {
        case MemberType::Define:      // fall through
        //case MemberType::Prototype: // fall through
        case MemberType::Typedef:     // fall through
        case MemberType::Variable:    // fall through
        case MemberType::Function:    // fall through
        case MemberType::Signal:      // fall through
        case MemberType::Slot:        // fall through
        case MemberType::DCOP:        // fall through
        case MemberType::Property:    // fall through
        case MemberType::Interface:   // fall through
        case MemberType::Service:     // fall through
        case MemberType::Sequence:    // fall through
        case MemberType::Dictionary:  // fall through
        case MemberType::Event:
          {
            if (first) ol.startMemberList(),first=FALSE;
            md->writeDeclaration(ol,cd,nd,fd,gd,mod,inGroup,indentLevel,inheritedFrom,inheritId);
            break;
          }
        case MemberType::Enumeration:
          {
            // if this is an anonymous enum and there are variables of this
            // enum type (i.e. enumVars>0), then we do not show the enum here.
            if (countEnumValues(md)==0) // show enum here
            {
              //printf("Enum!!\n");
              if (first)
              {
                ol.startMemberList();
                first=FALSE;
              }
              ol.startMemberDeclaration();
              ol.startMemberItem(md->anchor(),OutputGenerator::MemberItemType::Normal,inheritId);
              bool detailsLinkable = md->hasDetailedDescription();
              if (!detailsLinkable)
              {
                ol.startDoxyAnchor(md->getOutputFileBase(),QCString(),md->anchor(),md->name(),QCString());
                ol.addLabel(md->getOutputFileBase(),md->anchor());
              }
              if (md->isSliceLocal())
              {
                ol.writeString("local ");
              }
              ol.writeString("enum ");
              if (md->getLanguage()==SrcLangExt::Cpp && md->isStrong())
              {
                if (md->isEnumStruct())
                {
                  ol.writeString("struct ");
                }
                else
                {
                  ol.writeString("class ");
                }
              }
              ol.insertMemberAlign();
              md->writeEnumDeclaration(ol,cd,nd,fd,gd,mod);
              if (!detailsLinkable)
              {
                ol.endDoxyAnchor(md->getOutputFileBase(),md->anchor());
              }
              ol.endMemberItem(OutputGenerator::MemberItemType::Normal);
              if (!md->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
              {
                auto parser { createDocParser() };
                auto ast    { validatingParseDoc(*parser.get(),
                                                 md->briefFile(),
                                                 md->briefLine(),
                                                 cd ? cd : md->getOuterScope(),
                                                 md,
                                                 md->briefDescription(),
                                                 DocOptions()
                                                 .setIndexWords(true)
                                                 .setSingleLine(true))
                            };
                if (!ast->isEmpty())
                {
                  ol.startMemberDescription(md->anchor());
                  ol.writeDoc(ast.get(),cd,md);
                  if (md->hasDetailedDescription())
                  {
                    ol.disableAllBut(OutputType::Html);
                    ol.docify(" ");
                    ol.startTextLink(md->getOutputFileBase(),
                        md->anchor());
                    ol.parseText(theTranslator->trMore());
                    ol.endTextLink();
                    ol.enableAll();
                  }
                  ol.endMemberDescription();
                }
              }
              ol.endMemberDeclaration(md->anchor(),inheritId);
            }
            md->warnIfUndocumented();
            break;
          }
        case MemberType::Friend:
          if (inheritedFrom==nullptr)
          {
            if (first)
            {
              ol.startMemberList();
              first=FALSE;
            }
            md->writeDeclaration(ol,cd,nd,fd,gd,mod,inGroup,indentLevel,inheritedFrom,inheritId);
            break;
          }
        case MemberType::EnumValue:
          {
            if (inGroup)
            {
              //printf("EnumValue!\n");
              if (first) ol.startMemberList(),first=FALSE;
              md->writeDeclaration(ol,cd,nd,fd,gd,mod,true,indentLevel,inheritedFrom,inheritId);
            }
          }
          break;
      }
    }
  }

  if (!first)
  {
    ol.endMemberList();
  }

  ol.popGeneratorState();
  //printf("----- end writePlainDeclaration() ----\n");
}

/** Writes the list of members to the output.
 *  @param ol Output list to write to
 *  @param cd non-null if this list is part of class documentation.
 *  @param nd non-null if this list is part of namespace documentation.
 *  @param fd non-null if this list is part of file documentation.
 *  @param gd non-null if this list is part of group documentation.
 *  @param mod non-null if this list is part of module documentation.
 *  @param title Title to use for the member list.
 *  @param subtitle Sub title to use for the member list.
 *  @param showEnumValues Obsolete, always set to FALSE.
 *  @param showInline if set to TRUE if title is rendered differently
 *  @param inheritedFrom if not 0, the list is shown inside the
 *         given class as inherited members, parameter cd points to the
 *         class containing the members.
 *  @param lt Type of list that is inherited from.
 *  @param showSectionTitle do we show the "additional members" header or not?
 *         When combining public and protected inherited members under a single header only for the first list it should be shown
 */
void MemberList::writeDeclarations(OutputList &ol,
             const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
             const QCString &title,const QCString &subtitle, bool showEnumValues,
             bool showInline,const ClassDef *inheritedFrom,MemberListType lt,bool showSectionTitle) const
{
  (void)showEnumValues; // unused

  //printf("----- writeDeclaration() this=%p ---- inheritedFrom=%p\n",this,inheritedFrom);
  bool optimizeVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString inheritId;

  const Definition  *ctx =  cd;
  if (ctx==nullptr &&  nd) ctx =  nd;
  if (ctx==nullptr &&  gd) ctx =  gd;
  if (ctx==nullptr && mod) ctx = mod;
  if (ctx==nullptr &&  fd) ctx =  fd;

  //printf("%p: MemberList::writeDeclaration(title='%s',subtitle='%s')=%d inheritedFrom=%p\n",
  //       (void*)this,qPrint(title),qPrint(subtitle),numDecMembers(),(void*)inheritedFrom);

  int num = numDecMembers(inheritedFrom);
  int numEnumValues = numDecEnumValues();
  if (inheritedFrom && num>0)
  {
    if (cd && !optimizeVhdl)
    {
      inheritId = substitute(lt.toLabel(),"-","_")+"_"+
                  stripPath(cd->getOutputFileBase());
      if (showSectionTitle && !title.isEmpty())
      {
        ol.writeInheritedSectionTitle(inheritId,cd->getReference(),
                                      cd->getOutputFileBase(),
                                      cd->anchor(),title,cd->displayName());
      }
    }
  }
  else if (num>numEnumValues)
  {
    if (!title.isEmpty())
    {
      if (showInline)
      {
        ol.startInlineHeader();
      }
      else
      {
        ol.startMemberHeader(m_listType.toLabel());
      }
      ol.parseText(title);
      if (showInline)
      {
        ol.endInlineHeader();
      }
      else
      {
        ol.endMemberHeader();
      }
    }
    if (!subtitle.stripWhiteSpace().isEmpty())
    {
      ol.startMemberSubtitle();
      ol.generateDoc("[generated]", -1, ctx, nullptr, subtitle, DocOptions());
      ol.endMemberSubtitle();
    }
  }
  if (num>numEnumValues)
  {
     bool inGroup = m_container==MemberListContainer::Group;
    // TODO: Two things need to be worked out for proper VHDL output:
    // 1. Signals and types under the group need to be
    //    formatted to associate them with the group somehow
    //    indentation, or at the very least, extra space after
    //    the group is done
    // 2. This might need to be repeated below for memberGroupLists
    if (optimizeVhdl) // use specific declarations function
    {
      VhdlDocGen::writeVhdlDeclarations(this,ol,nullptr,cd,nullptr,nullptr,nullptr);
    }
    else
    {
      writePlainDeclarations(ol,inGroup,cd,nd,fd,gd,mod,0,inheritedFrom,inheritId);
    }

    //printf("memberGroupList=%p\n",memberGroupList);
    int groupId=0;
    for (const auto &mg : m_memberGroupRefList)
    {
      bool hasHeader=!mg->header().isEmpty();
      if (inheritId.isEmpty())
      {
        QCString groupAnchor = QCString(listType().toLabel())+"-"+QCString().setNum(groupId++);
        //printf("mg->header=%s hasHeader=%d\n",qPrint(mg->header()),hasHeader);
        ol.startMemberGroupHeader(groupAnchor,hasHeader);
        if (hasHeader)
        {
          ol.parseText(mg->header());
        }
        ol.endMemberGroupHeader();
        if (!mg->documentation().isEmpty())
        {
          //printf("Member group has docs!\n");
          ol.startMemberGroupDocs();
          ol.generateDoc(mg->docFile(),
                         mg->docLine(),
                         mg->memberContainer(),
                         nullptr,
                         mg->documentation()+"\n",
                         DocOptions());
          ol.endMemberGroupDocs();
        }
        ol.startMemberGroup();
      }
      //printf("--- mg->writePlainDeclarations ---\n");
      mg->writePlainDeclarations(ol,inGroup,cd,nd,fd,gd,mod,0,inheritedFrom,inheritId);
      if (inheritId.isEmpty())
      {
        ol.endMemberGroup(hasHeader);
      }
    }
  }
  if (inheritedFrom && cd)
  {
    // also add members that of this list type, that are grouped together
    // in a separate list in class 'inheritedFrom'
    cd->addGroupedInheritedMembers(ol,m_listType,inheritedFrom,inheritId);
  }
  //printf("----- end writeDeclaration() ----\n");
}

void MemberList::writeDocumentation(OutputList &ol,
                     const QCString &scopeName, const Definition *container,
                     const QCString &title,const QCString &anchor,
                     bool showEnumValues,bool showInline) const
{
  if (numDocMembers()==-1)
  {
    err("MemberList::numDocMembers()==-1, so the members of this list have not been counted. Please report as a bug.\n");
    abort();
  }

  if (numDocMembers()==0) return;
  if (!showEnumValues && numDocMembers()<=numDocEnumValues()) return;

  if (!title.isEmpty())
  {
    ol.pushGeneratorState();
      ol.disable(OutputType::Html);
      ol.writeRuler();
    ol.popGeneratorState();
    if (container) ol.writeAnchor(container->getOutputFileBase(),anchor);
    ol.startGroupHeader(anchor,showInline ? 2 : 0);
    ol.parseText(title);
    ol.endGroupHeader(showInline ? 2 : 0);
  }
  ol.startMemberDocList();

  struct OverloadInfo
  {
    uint32_t count = 1;
    uint32_t total = 0;
  };
  std::unordered_map<std::string,OverloadInfo> overloadInfo;
  // count the number of overloaded members
  for (const auto &md : m_members)
  {
    if (md->isDetailedSectionVisible(m_container) &&
        !(md->isEnumValue() && !showInline))
    {
      auto it = overloadInfo.emplace(md->name().str(),OverloadInfo()).first;
      it->second.total++;
    }
  }

  for (const auto &md : m_members)
  {
    if (md->isDetailedSectionVisible(m_container) &&
        !(md->isEnumValue() && !showInline))
    {
      auto it = overloadInfo.find(md->name().str());
      uint32_t overloadCount = it->second.total;
      uint32_t &count = it->second.count;
      MemberDefMutable *mdm = toMemberDefMutable(md);
      if (mdm)
      {
        mdm->writeDocumentation(this,count++,overloadCount,ol,scopeName,container,
            m_container==MemberListContainer::Group,showEnumValues,showInline);
      }
    }
  }
  //printf("MemberList::writeDocumentation()  --  member groups %d\n",memberGroupList->count());
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->writeDocumentation(ol,scopeName,container,showEnumValues,showInline);
  }
  ol.endMemberDocList();
}

// members in a table
void MemberList::writeSimpleDocumentation(OutputList &ol,
                     const Definition *container) const
{
  //printf("MemberList count=%d enumValues=%d\n",numDocMembers(),numDocEnumValues());
  if (numDocMembers()<=numDocEnumValues()) return; // only enum values and they should be excluded

  const ClassDef *cd = nullptr;
  if (container && container->definitionType()==Definition::TypeClass)
  {
    cd = toClassDef(container);
  }
  ol.startMemberDocSimple(cd && cd->isJavaEnum());
  for (const auto &md : m_members)
  {
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm)
    {
      mdm->writeMemberDocSimple(ol,container);
    }
  }
  ol.endMemberDocSimple(cd && cd->isJavaEnum());
}

// separate member pages
void MemberList::writeDocumentationPage(OutputList &ol,
                     const QCString &scopeName, const DefinitionMutable *container, int hierarchyLevel) const
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);

  struct OverloadInfo
  {
    uint32_t count = 1;
    uint32_t total = 0;
  };
  std::unordered_map<std::string,OverloadInfo> overloadInfo;

  // count the number of overloaded members
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);

    if (md && md->hasDetailedDescription())
    {
      auto it = overloadInfo.emplace(md->name().str(),OverloadInfo()).first;
      it->second.total++;
    }
  }

  for (const auto &imd : m_members)
  {
    Definition *container_d = toDefinition(const_cast<DefinitionMutable*>(container));
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md && md->hasDetailedDescription())
    {
      auto it = overloadInfo.find(md->name().str());
      uint32_t overloadCount = it->second.total;
      uint32_t &count = it->second.count;
      QCString diskName=md->getOutputFileBase();
      QCString title=md->qualifiedName();
      startFile(ol,diskName,false,md->name(),title,HighlightedItem::None,!generateTreeView,diskName, hierarchyLevel);
      if (!generateTreeView)
      {
        container->writeNavigationPath(ol);
        ol.endQuickIndices();
      }
      ol.startContents();

      if (generateTreeView)
      {
        md->writeDocumentation(this,count++,overloadCount,ol,scopeName,container_d,m_container==MemberListContainer::Group);

        ol.endContents();
        endFileWithNavPath(ol,container);
      }
      else
      {
        ol.writeString("<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\">\n"
            "  <tr>\n"
            "   <td valign=\"top\">\n");

        container->writeQuickMemberLinks(ol,md);

        ol.writeString("   </td>\n");
        ol.writeString("   <td valign=\"top\" class=\"mempage\">\n");

        md->writeDocumentation(this,count++,overloadCount,ol,scopeName,container_d,m_container==MemberListContainer::Group);

        ol.writeString("    </td>\n");
        ol.writeString("  </tr>\n");
        ol.writeString("</table>\n");

        endFile(ol);
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->writeDocumentationPage(ol,scopeName,container);
  }
}

void MemberList::addMemberGroup(MemberGroup *mg)
{
  m_memberGroupRefList.push_back(mg);
}

void MemberList::addListReferences(const Definition *def)
{
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md && !md->isAlias() && (md->getGroupDef()==nullptr || def->definitionType()==Definition::TypeGroup))
    {
      md->addListReference(def);
      const MemberVector &enumFields = md->enumFieldList();
      if (md->memberType()==MemberType::Enumeration && !enumFields.empty())
      {
        //printf("  Adding enum values!\n");
        for (const auto &vmd : enumFields)
        {
          MemberDefMutable *vmdm = toMemberDefMutable(vmd);
          if (vmdm)
          {
            //printf("   adding %s\n",qPrint(vmd->name()));
            vmdm->addListReference(def);
          }
        }
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->addListReferences(def);
  }
}

void MemberList::addRequirementReferences(const Definition *def)
{
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md && !md->isAlias() && (md->getGroupDef()==nullptr || def->definitionType()==Definition::TypeGroup))
    {
      md->addRequirementReferences(def);
      const MemberVector &enumFields = md->enumFieldList();
      if (md->memberType()==MemberType::Enumeration && !enumFields.empty())
      {
        //printf("  Adding enum values!\n");
        for (const auto &vmd : enumFields)
        {
          MemberDefMutable *vmdm = toMemberDefMutable(vmd);
          if (vmdm)
          {
            //printf("   adding %s\n",qPrint(vmd->name()));
            vmdm->addRequirementReferences(def);
          }
        }
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->addRequirementReferences(def);
  }
}

void MemberList::findSectionsInDocumentation(const Definition *d)
{
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md)
    {
      md->findSectionsInDocumentation();
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->findSectionsInDocumentation(d);
  }
}

void MemberList::setNeedsSorting(bool b)
{
  m_needsSorting = b;
}

void MemberList::writeTagFile(TextStream &tagFile,bool useQualifiedName,bool showNamespaceMembers)
{
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md)
    {
      if (md->getLanguage()!=SrcLangExt::VHDL)
      {
        md->writeTagFile(tagFile,useQualifiedName,showNamespaceMembers);
        if (md->memberType()==MemberType::Enumeration && !md->isStrong())
        {
          for (const auto &ivmd : md->enumFieldList())
          {
            MemberDefMutable *vmd = toMemberDefMutable(ivmd);
            if (vmd)
            {
              vmd->writeTagFile(tagFile,useQualifiedName,showNamespaceMembers);
            }
          }
        }
      }
      else
      {
        VhdlDocGen::writeTagFile(md,tagFile);
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->writeTagFile(tagFile,useQualifiedName);
  }
}

// compute the HTML anchors for a list of members
void MemberList::setAnchors()
{
  //int count=0;
  for (const auto &md : m_members)
  {
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm && !md->isReference())
    {
      mdm->setAnchor();
    }
  }
}

