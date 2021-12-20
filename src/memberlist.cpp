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
  // sort on name
  int cmp = qstricmp(c1->name(),c2->name());
  // then on argument list
  if (cmp==0 && !c1->argsString().isEmpty() && !c2->argsString().isEmpty())
  {
    cmp = qstricmp(c1->argsString(),c2->argsString());
  }
  // then on file in which the item is defined
  if (cmp==0)
  {
    cmp = qstricmp(c1->getDefFileName(),c2->getDefFileName());
  }
  // then on line number at which the member is defined
  if (cmp==0)
  {
    cmp = c1->getDefLine()-c2->getDefLine();
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
      if (md->memberType()!=MemberType_Friend &&
          md->memberType()!=MemberType_EnumValue)
      {
        //printf("member %s: isReimplementedBy(%s)=%d\n",qPrint(md->name()),
        //    qPrint(inheritedFrom->name()),
        //    md->isReimplementedBy(inheritedFrom));
        if (md->memberType()==MemberType_Function)
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
  //    qPrint(listTypeAsString()),
  //    qPrint(inheritedFrom->name()),count);
  return count;
}

/*! Count the number of members in this list that are visible in
 *  the declaration part of a compound's documentation page.
 */
void MemberList::countDecMembers()
{
  if (m_numDecMembers!=-1) return;

  //printf("----- countDecMembers count=%d ----\n",count());
  /*
  m_varCnt=m_funcCnt=m_enumCnt=m_enumValCnt=0;
  m_typeCnt=m_seqCnt=m_dictCnt=m_protoCnt=m_defCnt=m_friendCnt=0;
  */
  m_numDecMembers=0;
  for (const auto &md : m_members)
  {
    //printf("MemberList::countDecMembers(md=%s,%d)\n",qPrint(md->name()),md->isBriefSectionVisible());
    if (md->isBriefSectionVisible())
    {
      switch(md->memberType())
      {
        case MemberType_Variable:    // fall through
        case MemberType_Event:       // fall through
        case MemberType_Property:    /*m_varCnt++,*/
                                     m_numDecMembers++;
                                     break;
// apparently necessary to get this to show up in declarations section?
        case MemberType_Interface:   // fall through
        case MemberType_Service:     // fall through
        case MemberType_Function:    // fall through
        case MemberType_Signal:      // fall through
        case MemberType_DCOP:        // fall through
        case MemberType_Slot:        if (!md->isRelated() || md->getClassDef())
                                       /*m_funcCnt++,*/
                                       m_numDecMembers++;
                                     break;
        case MemberType_Enumeration: /*m_enumCnt++,*/
                                     m_numDecMembers++;
                                     break;
        case MemberType_EnumValue:   m_numDecEnumValues++;
                                     m_numDecMembers++;
                                     break;
        case MemberType_Typedef:     /*m_typeCnt++,*/
                                     m_numDecMembers++;
                                     break;
        case MemberType_Sequence:    /*m_seqCnt++,*/
                                     m_numDecMembers++;
                                     break;
        case MemberType_Dictionary:  /*m_dictCnt++,*/
                                     m_numDecMembers++;
                                     break;
        //case MemberType_Prototype:   m_protoCnt++,m_numDecMembers++; break;
        case MemberType_Define:      if (Config_getBool(EXTRACT_ALL) ||
                                         !md->argsString().isEmpty() ||
                                         !md->initializer().isEmpty() ||
                                         md->hasDocumentation()
                                        ) /*m_defCnt++,*/ m_numDecMembers++;
                                     break;
        case MemberType_Friend:      /*m_friendCnt++,*/
                                     m_numDecMembers++;
                                     break;
        default:
          err("Unknown member type found for member '%s'\n!",qPrint(md->name()));
      }
    }
  }
  for (const auto &mg : m_memberGroupRefList)
  {
    mg->countDecMembers();
    /*
    m_varCnt+=mg->varCount();
    m_funcCnt+=mg->funcCount();
    m_enumCnt+=mg->enumCount();
    m_enumValCnt+=mg->enumValueCount();
    m_typeCnt+=mg->typedefCount();
    m_seqCnt+=mg->sequenceCount();
    m_dictCnt+=mg->dictionaryCount();
    m_protoCnt+=mg->protoCount();
    m_defCnt+=mg->defineCount();
    m_friendCnt+=mg->friendCount();
    */
    m_numDecMembers+=mg->numDecMembers();
    m_numDecEnumValues+=mg->numDecEnumValues();
  }
  //printf("----- end countDecMembers ----\n");

  //printf("MemberList::countDecMembers()=%d\n",m_numDecMembers);
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
      if (md->memberType()==MemberType_EnumValue)
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
      if (md->memberType()==MemberType_Enumeration && name[0]=='@')
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
        case MemberType_Define:     // fall through
        case MemberType_Typedef:    // fall through
        case MemberType_Variable:   // fall through
        case MemberType_Function:   // fall through
        case MemberType_Signal:     // fall through
        case MemberType_Slot:       // fall through
        case MemberType_DCOP:       // fall through
        case MemberType_Property:   // fall through
        case MemberType_Interface:  // fall through
        case MemberType_Service:    // fall through
        case MemberType_Sequence:   // fall through
        case MemberType_Dictionary: // fall through
        case MemberType_Event:
          return TRUE;
        case MemberType_Enumeration:
          {
            // if this is an anonymous enum and there are variables of this
            // enum type (i.e. enumVars>0), then we do not show the enum here.
            if (countEnumValues(md)==0) // show enum here
            {
              return TRUE;
            }
          }
          break;
        case MemberType_Friend:
          return TRUE;
        case MemberType_EnumValue:
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
                       const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd, const GroupDef *gd,
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
    //printf(">>> Member '%s' type=%d visible=%d\n",
    //    qPrint(md->name()),md->memberType(),md->isBriefSectionVisible());
    if ((inheritedFrom==0 || !md->isReimplementedBy(inheritedFrom)) &&
        md->isBriefSectionVisible())
    {
      //printf(">>> rendering\n");
      switch(md->memberType())
      {
        case MemberType_Define:      // fall through
        //case MemberType_Prototype: // fall through
        case MemberType_Typedef:     // fall through
        case MemberType_Variable:    // fall through
        case MemberType_Function:    // fall through
        case MemberType_Signal:      // fall through
        case MemberType_Slot:        // fall through
        case MemberType_DCOP:        // fall through
        case MemberType_Property:    // fall through
        case MemberType_Interface:   // fall through
        case MemberType_Service:     // fall through
        case MemberType_Sequence:    // fall through
        case MemberType_Dictionary:  // fall through
        case MemberType_Event:
          {
            if (first) ol.startMemberList(),first=FALSE;
            md->writeDeclaration(ol,cd,nd,fd,gd,inGroup,indentLevel,inheritedFrom,inheritId);
            break;
          }
        case MemberType_Enumeration:
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
              ol.startMemberItem(md->anchor(),0,inheritId);
              bool detailsLinkable = md->hasDetailedDescription();
              if (!detailsLinkable)
              {
                ol.startDoxyAnchor(md->getOutputFileBase(),QCString(),md->anchor(),md->name(),QCString());
              }
              if (md->isSliceLocal())
              {
                ol.writeString("local ");
              }
              ol.writeString("enum ");
              if (md->getLanguage()==SrcLangExt_Cpp && md->isStrong())
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
              md->writeEnumDeclaration(ol,cd,nd,fd,gd);
              if (!detailsLinkable)
              {
                ol.endDoxyAnchor(md->getOutputFileBase(),md->anchor());
              }
              ol.endMemberItem();
              if (!md->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
              {
                std::unique_ptr<IDocParser> parser { createDocParser() };
                std::unique_ptr<DocRoot>  rootNode { validatingParseDoc(*parser.get(),
                                                     md->briefFile(),md->briefLine(),
                                                     cd,md,
                                                     md->briefDescription(),
                                                     TRUE,FALSE,
                                                     QCString(),TRUE,FALSE,
                                                     Config_getBool(MARKDOWN_SUPPORT)) };
                if (rootNode && !rootNode->isEmpty())
                {
                  ol.startMemberDescription(md->anchor());
                  ol.writeDoc(rootNode.get(),cd,md);
                  if (md->hasDetailedDescription())
                  {
                    ol.disableAllBut(OutputGenerator::Html);
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
        case MemberType_Friend:
          if (inheritedFrom==0)
          {
            if (first)
            {
              ol.startMemberList();
              first=FALSE;
            }
            md->writeDeclaration(ol,cd,nd,fd,gd,inGroup,indentLevel,inheritedFrom,inheritId);
            break;
          }
        case MemberType_EnumValue:
          {
            if (inGroup)
            {
              //printf("EnumValue!\n");
              if (first) ol.startMemberList(),first=FALSE;
              md->writeDeclaration(ol,cd,nd,fd,gd,true,indentLevel,inheritedFrom,inheritId);
            }
          }
          break;
      }
    }
  }

  // handle members that are inside anonymous compounds and for which
  // no variables of the anonymous compound type exist.
  if (cd)
  {
    for (const auto &md : m_members)
    {
      if (md->fromAnonymousScope() && !md->anonymousDeclShown())
      {
        MemberDefMutable *mdm = toMemberDefMutable(md);
        if (mdm) mdm->setFromAnonymousScope(FALSE);
        //printf("anonymous compound members\n");
        if (md->isBriefSectionVisible())
        {
          if (first)
          {
            ol.startMemberList();
            first=FALSE;
          }
          md->writeDeclaration(ol,cd,nd,fd,gd,inGroup,indentLevel);
        }
        if (mdm) mdm->setFromAnonymousScope(TRUE);
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
 *  @param title Title to use for the member list.
 *  @param subtitle Sub title to use for the member list.
 *  @param showEnumValues Obsolete, always set to FALSE.
 *  @param showInline if set to TRUE if title is rendered differently
 *  @param inheritedFrom if not 0, the list is shown inside the
 *         given class as inherited members, parameter cd points to the
 *         class containing the members.
 *  @param lt Type of list that is inherited from.
 */
void MemberList::writeDeclarations(OutputList &ol,
             const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
             const QCString &title,const QCString &subtitle, bool showEnumValues,
             bool showInline,const ClassDef *inheritedFrom,MemberListType lt) const
{
  (void)showEnumValues; // unused

  //printf("----- writeDeclaration() this=%p ---- inheritedFrom=%p\n",this,inheritedFrom);
  static bool optimizeVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString inheritId;

  const Definition *ctx = cd;
  if (ctx==0 && nd) ctx = nd;
  if (ctx==0 && gd) ctx = gd;
  if (ctx==0 && fd) ctx = fd;

  //printf("%p: MemberList::writeDeclaration(title='%s',subtitle='%s')=%d inheritedFrom=%p\n",
  //       this,title,subtitle,numDecMembers(),inheritedFrom);

  int num = numDecMembers();
  int numEnumValues = numDecEnumValues();
  if (inheritedFrom)
  {
    //if ( cd && !optimizeVhdl && countInheritableMembers(inheritedFrom)>0 )
    if ( cd && !optimizeVhdl && cd->countMembersIncludingGrouped(
                                      m_listType,inheritedFrom,TRUE)>0 )
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      inheritId = substitute(listTypeAsString(lt),"-","_")+"_"+
                  stripPath(cd->getOutputFileBase());
      if (!title.isEmpty())
      {
        ol.writeInheritedSectionTitle(inheritId,cd->getReference(),
                                      cd->getOutputFileBase(),
                                      cd->anchor(),title,cd->displayName());
      }
      ol.popGeneratorState();
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
        ol.startMemberHeader(listTypeAsString(m_listType));
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
      ol.generateDoc("[generated]",-1,ctx,0,subtitle,FALSE,FALSE,
                     QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
      VhdlDocGen::writeVhdlDeclarations(this,ol,0,cd,0,0);
    }
    else
    {
      writePlainDeclarations(ol,inGroup,cd,nd,fd,gd,0,0,inheritId);
    }

    //printf("memberGroupList=%p\n",memberGroupList);
    for (const auto &mg : m_memberGroupRefList)
    {
      bool hasHeader=!mg->header().isEmpty() && mg->header()!="[NOHEADER]";
      if (inheritId.isEmpty())
      {
        //printf("mg->header=%s hasHeader=%d\n",qPrint(mg->header()),hasHeader);
        ol.startMemberGroupHeader(hasHeader);
        if (hasHeader)
        {
          ol.parseText(mg->header());
        }
        ol.endMemberGroupHeader();
        if (!mg->documentation().isEmpty())
        {
          //printf("Member group has docs!\n");
          ol.startMemberGroupDocs();
          ol.generateDoc(mg->docFile(),mg->docLine(),ctx,0,mg->documentation()+"\n",FALSE,FALSE,
              QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
          ol.endMemberGroupDocs();
        }
        ol.startMemberGroup();
      }
      //printf("--- mg->writePlainDeclarations ---\n");
      mg->writePlainDeclarations(ol,inGroup,cd,nd,fd,gd,0,inheritedFrom,inheritId);
      if (inheritId.isEmpty())
      {
        ol.endMemberGroup(hasHeader);
      }
    }
  }
  if (inheritedFrom && cd)
  {
    const ClassDefMutable *cdm = toClassDefMutable(cd);
    if (cdm)
    {
      // also add members that of this list type, that are grouped together
      // in a separate list in class 'inheritedFrom'
      cdm->addGroupedInheritedMembers(ol,m_listType,inheritedFrom,inheritId);
    }
  }
  //printf("----- end writeDeclaration() ----\n");
}

void MemberList::writeDocumentation(OutputList &ol,
                     const QCString &scopeName, const Definition *container,
                     const QCString &title,bool showEnumValues,bool showInline) const
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
      ol.disable(OutputGenerator::Html);
      ol.writeRuler();
    ol.popGeneratorState();
    ol.startGroupHeader(showInline ? 2 : 0);
    ol.parseText(title);
    ol.endGroupHeader(showInline ? 2 : 0);
  }
  ol.startMemberDocList();

  struct OverloadInfo
  {
    uint count = 1;
    uint total = 0;
  };
  std::unordered_map<std::string,OverloadInfo> overloadInfo;
  // count the number of overloaded members
  for (const auto &md : m_members)
  {
    if (md->isDetailedSectionVisible(m_container) &&
        !(md->isEnumValue() && !showInline))
    {
      auto it = overloadInfo.insert(std::make_pair(md->name().str(),OverloadInfo())).first;
      it->second.total++;
    }
  }

  for (const auto &md : m_members)
  {
    if (md->isDetailedSectionVisible(m_container) &&
        !(md->isEnumValue() && !showInline))
    {
      auto it = overloadInfo.find(md->name().str());
      uint overloadCount = it->second.total;
      uint &count = it->second.count;
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

  const ClassDef *cd = 0;
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
                     const QCString &scopeName, const DefinitionMutable *container) const
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);

  struct OverloadInfo
  {
    uint count = 1;
    uint total = 0;
  };
  std::unordered_map<std::string,OverloadInfo> overloadInfo;

  // count the number of overloaded members
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);

    if (md && md->hasDetailedDescription())
    {
      auto it = overloadInfo.insert(std::make_pair(md->name().str(),OverloadInfo())).first;
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
      uint overloadCount = it->second.total;
      uint &count = it->second.count;
      QCString diskName=md->getOutputFileBase();
      QCString title=md->qualifiedName();
      startFile(ol,diskName,md->name(),title,HLI_None,!generateTreeView,diskName);
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
        endFileWithNavPath(container_d,ol);
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

void MemberList::addListReferences(Definition *def)
{
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md && !md->isAlias() && (md->getGroupDef()==0 || def->definitionType()==Definition::TypeGroup))
    {
      md->addListReference(def);
      const MemberVector &enumFields = md->enumFieldList();
      if (md->memberType()==MemberType_Enumeration && !enumFields.empty())
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

QCString MemberList::listTypeAsString(MemberListType type)
{
  switch(type)
  {
    case MemberListType_pubMethods: return "pub-methods";
    case MemberListType_proMethods: return "pro-methods";
    case MemberListType_pacMethods: return "pac-methods";
    case MemberListType_priMethods: return "pri-methods";
    case MemberListType_pubStaticMethods: return "pub-static-methods";
    case MemberListType_proStaticMethods: return "pro-static-methods";
    case MemberListType_pacStaticMethods: return "pac-static-methods";
    case MemberListType_priStaticMethods: return "pri-static-methods";
    case MemberListType_pubSlots: return "pub-slots";
    case MemberListType_proSlots: return "pro-slots";
    case MemberListType_priSlots: return "pri-slots";
    case MemberListType_pubAttribs: return "pub-attribs";
    case MemberListType_proAttribs: return "pro-attribs";
    case MemberListType_pacAttribs: return "pac-attribs";
    case MemberListType_priAttribs: return "pri-attribs";
    case MemberListType_pubStaticAttribs: return "pub-static-attribs";
    case MemberListType_proStaticAttribs: return "pro-static-attribs";
    case MemberListType_pacStaticAttribs: return "pac-static-attribs";
    case MemberListType_priStaticAttribs: return "pri-static-attribs";
    case MemberListType_pubTypes: return "pub-types";
    case MemberListType_proTypes: return "pro-types";
    case MemberListType_pacTypes: return "pac-types";
    case MemberListType_priTypes: return "pri-types";
    case MemberListType_related: return "related";
    case MemberListType_signals: return "signals";
    case MemberListType_friends: return "friends";
    case MemberListType_dcopMethods: return "dcop-methods";
    case MemberListType_properties: return "properties";
    case MemberListType_events: return "events";
    case MemberListType_interfaces: return "interfaces";
    case MemberListType_services: return "services";
    case MemberListType_decDefineMembers: return "define-members";
    case MemberListType_decProtoMembers: return "proto-members";
    case MemberListType_decTypedefMembers: return "typedef-members";
    case MemberListType_decSequenceMembers: return "sequence-members";
    case MemberListType_decDictionaryMembers: return "dictionary-members";
    case MemberListType_decEnumMembers: return "enum-members";
    case MemberListType_decFuncMembers: return "func-members";
    case MemberListType_decVarMembers: return "var-members";
    case MemberListType_decEnumValMembers: return "enumval-members";
    case MemberListType_decPubSlotMembers: return "pub-slot-members";
    case MemberListType_decProSlotMembers: return "pro-slot-members";
    case MemberListType_decPriSlotMembers: return "pri-slot-members";
    case MemberListType_decSignalMembers: return "signal-members";
    case MemberListType_decEventMembers: return "event-members";
    case MemberListType_decFriendMembers: return "friend-members";
    case MemberListType_decPropMembers: return "prop-members";
    case MemberListType_enumFields: return "enum-fields";
    case MemberListType_memberGroup: break;
    default: break;
  }
  return "";
}

void MemberList::writeTagFile(TextStream &tagFile)
{
  for (const auto &imd : m_members)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md)
    {
      if (md->getLanguage()!=SrcLangExt_VHDL)
      {
        md->writeTagFile(tagFile);
        if (md->memberType()==MemberType_Enumeration && !md->isStrong())
        {
          for (const auto &ivmd : md->enumFieldList())
          {
            MemberDefMutable *vmd = toMemberDefMutable(ivmd);
            if (vmd)
            {
              vmd->writeTagFile(tagFile);
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
    mg->writeTagFile(tagFile);
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

