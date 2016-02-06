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

#include <qregexp.h>

#include "memberlist.h"
#include "classdef.h"
#include "message.h"
#include "util.h"
#include "language.h"
#include "doxygen.h"
#include "outputlist.h"
#include "groupdef.h"
#include "marshal.h"
#include "vhdldocgen.h"
#include "namespacedef.h"
#include "filedef.h"
#include "membergroup.h"
#include "config.h"
#include "docparser.h"

MemberList::MemberList() : m_listType(MemberListType_pubMethods)
{
  memberGroupList=0;
  m_varCnt=0;
  m_funcCnt=0;
  m_enumCnt=0;
  m_enumValCnt=0;
  m_typeCnt=0;
  m_protoCnt=0;
  m_defCnt=0;
  m_friendCnt=0;
  m_numDecMembers=-1; // special value indicating that value needs to be computed
  m_numDocMembers=-1; // special value indicating that value needs to be computed
  m_inGroup=FALSE;
  m_inFile=FALSE;
  m_needsSorting=FALSE;
}

MemberList::MemberList(MemberListType lt) : m_listType(lt)
{
  memberGroupList=0;
  m_varCnt=0;
  m_funcCnt=0;
  m_enumCnt=0;
  m_enumValCnt=0;
  m_typeCnt=0;
  m_protoCnt=0;
  m_defCnt=0;
  m_friendCnt=0;
  m_numDecMembers=-1; // special value indicating that value needs to be computed
  m_numDocMembers=-1; // special value indicating that value needs to be computed
  m_inGroup=FALSE;
  m_inFile=FALSE;
  m_needsSorting=FALSE;
}

MemberList::~MemberList()
{
  delete memberGroupList;
}

int MemberList::compareValues(const MemberDef *c1, const MemberDef *c2) const
{
  static bool sortConstructorsFirst = Config_getBool(SORT_MEMBERS_CTORS_1ST);
  if (sortConstructorsFirst) {
    int ord1 = c1->isConstructor() ? 2 : (c1->isDestructor() ? 1 : 0);
    int ord2 = c2->isConstructor() ? 2 : (c2->isDestructor() ? 1 : 0);
    if (ord1 > ord2)
      return -1;
    else if (ord2 > ord1)
      return 1;
  }
  int cmp = qstricmp(c1->name(),c2->name());
  return cmp!=0 ? cmp : c1->getDefLine()-c2->getDefLine();
}

int MemberList::countInheritableMembers(ClassDef *inheritedFrom) const
{
  int count=0;
  QListIterator<MemberDef> mli(*this);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    if (md->isBriefSectionVisible())
    {
      if (md->memberType()!=MemberType_Friend &&
          md->memberType()!=MemberType_EnumValue)
      {
        //printf("member %s: isReimplementedBy(%s)=%d\n",md->name().data(),
        //    inheritedFrom->name().data(),
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
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      count+=mg->countInheritableMembers(inheritedFrom);
    }
  }
  //printf("%s::countInheritableMembers(%s)=%d\n",
  //    listTypeAsString().data(),
  //    inheritedFrom->name().data(),count);
  return count;
}

/*! Count the number of members in this list that are visible in
 *  the declaration part of a compound's documentation page.
 */
void MemberList::countDecMembers(bool countEnumValues,GroupDef *gd)
{
  if (m_numDecMembers!=-1) return; 
  
  //printf("----- countDecMembers count=%d ----\n",count());
  m_varCnt=m_funcCnt=m_enumCnt=m_enumValCnt=0;
  m_typeCnt=m_protoCnt=m_defCnt=m_friendCnt=0;
  m_numDecMembers=0;
  QListIterator<MemberDef> mli(*this);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    //printf("MemberList::countDecMembers(md=%s,%d)\n",md->name().data(),md->isBriefSectionVisible());
    if (md->isBriefSectionVisible())
    {
      switch(md->memberType())
      {
        case MemberType_Variable:    // fall through
        case MemberType_Event:       // fall through
        case MemberType_Property:    m_varCnt++,m_numDecMembers++;  
                                     break;
// apparently necessary to get this to show up in declarations section?
        case MemberType_Interface:   // fall through
        case MemberType_Service:     // fall through
        case MemberType_Function:    // fall through
        case MemberType_Signal:      // fall through
        case MemberType_DCOP:        // fall through
        case MemberType_Slot:        if (!md->isRelated() || md->getClassDef())
                                       m_funcCnt++,m_numDecMembers++; 
                                     break;
        case MemberType_Enumeration: m_enumCnt++,m_numDecMembers++; break;
        case MemberType_EnumValue:   if (countEnumValues)
                                       m_enumValCnt++,m_numDecMembers++; 
                                     break;
        case MemberType_Typedef:     m_typeCnt++,m_numDecMembers++; break;
        //case MemberType_Prototype:   m_protoCnt++,m_numDecMembers++; break;
        case MemberType_Define:      if (Config_getBool(EXTRACT_ALL) || 
                                         md->argsString() || 
                                         !md->initializer().isEmpty() ||
                                         md->hasDocumentation() 
                                        ) m_defCnt++,m_numDecMembers++;     
                                     break;
        case MemberType_Friend:      m_friendCnt++,m_numDecMembers++;  
                                     break;
        default:
          err("Unknown member type found for member `%s'\n!",md->name().data());
      }
    }
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->countDecMembers(gd);
      m_varCnt+=mg->varCount();
      m_funcCnt+=mg->funcCount();
      m_enumCnt+=mg->enumCount();
      m_enumValCnt+=mg->enumValueCount();
      m_typeCnt+=mg->typedefCount();
      m_protoCnt+=mg->protoCount();
      m_defCnt+=mg->defineCount();
      m_friendCnt+=mg->friendCount();
      m_numDecMembers+=mg->numDecMembers();
    }
  }
  //printf("----- end countDecMembers ----\n");

  //printf("MemberList::countDecMembers()=%d\n",m_numDecMembers);
}

void MemberList::countDocMembers(bool countEnumValues)
{
  if (m_numDocMembers!=-1) return; // used cached value
  m_numDocMembers=0;
  QListIterator<MemberDef> mli(*this);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    if (md->isDetailedSectionVisible(m_inGroup,m_inFile)) 
    {
      // do not count enum values, since they do not produce entries of their own
      if (countEnumValues || md->memberType()!=MemberType_EnumValue) 
        m_numDocMembers++;
    }
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->countDocMembers();
      m_numDocMembers+=mg->numDocMembers();
    }
  }
  //printf("MemberList::countDocMembers()=%d memberGroupList=%p\n",m_numDocMembers,memberGroupList);
}

bool MemberList::insert(uint index,const MemberDef *md)
{
  return QList<MemberDef>::insert(index,md);
}

void MemberList::inSort(const MemberDef *md)
{
  QList<MemberDef>::inSort(md);
}

void MemberList::append(const MemberDef *md)
{
  QList<MemberDef>::append(md);
}

MemberListIterator::MemberListIterator(const QList<MemberDef> &l) :
  QListIterator<MemberDef>(l) 
{
}

bool MemberList::declVisible() const
{
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()); ++mli )
  {
    if (md->isBriefSectionVisible())
    {
      switch (md->memberType())
      {
        case MemberType_Define:    // fall through
        case MemberType_Typedef:   // fall through
        case MemberType_Variable:  // fall through
        case MemberType_Function:  // fall through
        case MemberType_Signal:    // fall through
        case MemberType_Slot:      // fall through
        case MemberType_DCOP:      // fall through
        case MemberType_Property:  // fall through
        case MemberType_Interface: // fall through
        case MemberType_Service:   // fall through
        case MemberType_Event:  
          return TRUE;
        case MemberType_Enumeration: 
          {
            int enumVars=0;
            MemberListIterator vmli(*this);
            MemberDef *vmd;
            QCString name(md->name());
            int i=name.findRev("::");
            if (i!=-1) name=name.right(name.length()-i-2); // strip scope (TODO: is this needed?)
            if (name[0]=='@') // anonymous enum => append variables
            {
              for ( ; (vmd=vmli.current()) ; ++vmli)
              {
                QCString vtype=vmd->typeString();
                if ((vtype.find(name))!=-1) 
                {
                  enumVars++;
                }
              }
            }
            // if this is an anonymous enum and there are variables of this
            // enum type (i.e. enumVars>0), then we do not show the enum here.
            if (enumVars==0) // show enum here
            {
              return TRUE;
            }
          }
          break;
        case MemberType_Friend:
          return TRUE;
        case MemberType_EnumValue: 
          {
            if (m_inGroup)
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

void MemberList::writePlainDeclarations(OutputList &ol,
                       ClassDef *cd,NamespaceDef *nd,FileDef *fd,
                       GroupDef *gd,ClassDef *inheritedFrom,const char *inheritId
                      )
{
  //printf("----- writePlainDeclaration() ----\n");
  static bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
  countDecMembers();
  if (numDecMembers()==0) 
  {
    //printf("  --> no members!\n");
    return; // no members in this list
  }
  //printf("  --> writePlainDeclaration() numDecMembers()=%d\n",
  //    numDecMembers());
  
  ol.pushGeneratorState();

  bool first=TRUE;
  MemberDef *md;
  MemberListIterator mli(*this);
  for ( ; (md=mli.current()); ++mli )
  {
    //printf(">>> Member `%s' type=%d visible=%d\n",
    //    md->name().data(),md->memberType(),md->isBriefSectionVisible());
    if ((inheritedFrom==0 || !md->isReimplementedBy(inheritedFrom)) &&
        md->isBriefSectionVisible())
    {
      //printf(">>> rendering\n");
      switch(md->memberType())
      {
        case MemberType_Define:    // fall through
        //case MemberType_Prototype: // fall through
        case MemberType_Typedef:   // fall through
        case MemberType_Variable:  // fall through
        case MemberType_Function:  // fall through
        case MemberType_Signal:    // fall through
        case MemberType_Slot:      // fall through
        case MemberType_DCOP:      // fall through
        case MemberType_Property:  // fall through
        case MemberType_Interface: // fall through
        case MemberType_Service:   // fall through
        case MemberType_Event:  
          {
            if (first) ol.startMemberList(),first=FALSE;
            md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup,inheritedFrom,inheritId);
            break;
          }
        case MemberType_Enumeration: 
          {
            int enumVars=0;
            MemberListIterator vmli(*this);
            MemberDef *vmd;
            QCString name(md->name());
            int i=name.findRev("::");
            if (i!=-1) name=name.right(name.length()-i-2); // strip scope (TODO: is this needed?)
            if (name[0]=='@') // anonymous enum => append variables
            {
              for ( ; (vmd=vmli.current()) ; ++vmli)
              {
                QCString vtype=vmd->typeString();
                if ((vtype.find(name))!=-1) 
                {
                  enumVars++;
                  vmd->setAnonymousEnumType(md);
                }
              }
            }
            // if this is an anonymous enum and there are variables of this
            // enum type (i.e. enumVars>0), then we do not show the enum here.
            if (enumVars==0 && !hideUndocMembers) // show enum here
            {
              //printf("Enum!!\n");
              if (first)
              {
                ol.startMemberList();
                first=FALSE;
              }
              ol.startMemberDeclaration();
              ol.startMemberItem(md->anchor(),0,inheritId);
              bool detailsLinkable = md->isDetailedSectionLinkable();
              if (!detailsLinkable)
              {
                ol.startDoxyAnchor(md->getOutputFileBase(),0,md->anchor(),md->name(),QCString());
              }
              ol.writeString("enum ");
              ol.insertMemberAlign();
              md->writeEnumDeclaration(ol,cd,nd,fd,gd);
              if (!detailsLinkable)
              {
                ol.endDoxyAnchor(md->getOutputFileBase(),md->anchor());
              }
              if (!md->briefDescription().isEmpty() && Config_getBool(BRIEF_MEMBER_DESC))
              {
                DocRoot *rootNode = validatingParseDoc(
                    md->briefFile(),md->briefLine(),
                    cd,md,
                    md->briefDescription(),
                    TRUE,FALSE,0,TRUE,FALSE
                    );
                if (rootNode && !rootNode->isEmpty())
                {
                  ol.startMemberDescription(md->anchor());
                  ol.writeDoc(rootNode,cd,md);
                  if (md->isDetailedSectionLinkable())
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
                delete rootNode;
              }
              ol.endMemberItem();
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
            md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup,inheritedFrom,inheritId);
            break;
          }
        case MemberType_EnumValue: 
          {
            if (m_inGroup)
            {
              //printf("EnumValue!\n");
              if (first) ol.startMemberList(),first=FALSE;
              md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup,inheritedFrom,inheritId);
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
    MemberListIterator mli(*this);
    for  ( ; (md=mli.current()) ; ++mli )
    {
      if (md->fromAnonymousScope() && !md->anonymousDeclShown())
      {
        md->setFromAnonymousScope(FALSE);
        //printf("anonymous compound members\n");
        if (md->isBriefSectionVisible())
        {
          if (first) 
          {
            ol.startMemberList();
            first=FALSE;
          }
          md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup);
        }
        md->setFromAnonymousScope(TRUE);
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
             ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
             const char *title,const char *subtitle, bool showEnumValues,
             bool showInline,ClassDef *inheritedFrom,MemberListType lt)
{
  (void)showEnumValues; // unused

  //printf("----- writeDeclaration() this=%p ---- inheritedFrom=%p\n",this,inheritedFrom);
  static bool optimizeVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString inheritId;

  countDecMembers(/*showEnumValues*/FALSE,gd); // count members shown in this section
  Definition *ctx = cd;
  if (ctx==0 && nd) ctx = nd;
  if (ctx==0 && gd) ctx = gd;
  if (ctx==0 && fd) ctx = fd;

  //printf("%p: MemberList::writeDeclaration(title=`%s',subtitle=`%s')=%d inheritedFrom=%p\n",
  //       this,title,subtitle,numDecMembers(),inheritedFrom);

  int num = numDecMembers();
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
      if (title)
      {
        ol.writeInheritedSectionTitle(inheritId,cd->getReference(),
                                      cd->getOutputFileBase(), 
                                      cd->anchor(),title,cd->displayName());
      }
      ol.popGeneratorState();
    }
  }
  else if (num>0)
  {
    if (title) 
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
    if (subtitle) 
    {
      QCString st=subtitle;
      st = st.stripWhiteSpace();
      if (!st.isEmpty())
      {
        ol.startMemberSubtitle();
        ol.generateDoc("[generated]",-1,ctx,0,subtitle,FALSE,FALSE,0,FALSE,FALSE);
        ol.endMemberSubtitle();
      }
    }
  }
  if (num>0)
  {
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
      writePlainDeclarations(ol,cd,nd,fd,gd,inheritedFrom,inheritId);
    }

    //printf("memberGroupList=%p\n",memberGroupList);
    if (memberGroupList)
    {
      MemberGroupListIterator mgli(*memberGroupList);
      MemberGroup *mg;
      while ((mg=mgli.current()))
      {
        bool hasHeader=!mg->header().isEmpty() && mg->header()!="[NOHEADER]";
        if (inheritId.isEmpty())
        {
          //printf("mg->header=%s hasHeader=%d\n",mg->header().data(),hasHeader);
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
            ol.generateDoc(mg->docFile(),mg->docLine(),ctx,0,mg->documentation()+"\n",FALSE,FALSE);
            ol.endMemberGroupDocs();
          }
          ol.startMemberGroup();
        }
        //printf("--- mg->writePlainDeclarations ---\n");
        mg->writePlainDeclarations(ol,cd,nd,fd,gd,inheritedFrom,inheritId);
        if (inheritId.isEmpty())
        {
          ol.endMemberGroup(hasHeader);
        }
        ++mgli;
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
                     const char *scopeName, Definition *container,
                     const char *title,bool showEnumValues,bool showInline)
{
  //printf("MemberList::writeDocumentation()\n");

  countDocMembers(showEnumValues);
  if (numDocMembers()==0) return;

  if (title)
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
  
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->writeDocumentation(this,ol,scopeName,container,
                           m_inGroup,showEnumValues,showInline);
  }
  if (memberGroupList)
  {
    printf("MemberList::writeDocumentation()  --  member groups %d\n",memberGroupList->count());
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writeDocumentation(ol,scopeName,container,showEnumValues,showInline);
    }
  }
  ol.endMemberDocList();
}

// members in a table
void MemberList::writeSimpleDocumentation(OutputList &ol,
                     Definition *container)
{
  countDocMembers(FALSE);
  //printf("MemberList count=%d\n",numDocMembers());
  if (numDocMembers()==0) return;

  ol.startMemberDocSimple();
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->writeMemberDocSimple(ol,container);
  }
  ol.endMemberDocSimple();
}

// separate member pages
void MemberList::writeDocumentationPage(OutputList &ol,
                     const char *scopeName, Definition *container)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    if (md->isDetailedSectionLinkable())
    {
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
        md->writeDocumentation(this,ol,scopeName,container,m_inGroup);
        ol.endContents();
        endFileWithNavPath(container,ol);
      }
      else
      {
        ol.writeString("<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\">\n"
            "  <tr>\n"
            "   <td valign=\"top\">\n");

        container->writeQuickMemberLinks(ol,md);

        ol.writeString("   </td>\n");
        ol.writeString("   <td valign=\"top\" class=\"mempage\">\n");

        md->writeDocumentation(this,ol,scopeName,container,m_inGroup);

        ol.writeString("    </td>\n");
        ol.writeString("  </tr>\n");
        ol.writeString("</table>\n");

        endFile(ol);
      }
    }
    if (memberGroupList)
    {
      //printf("MemberList::writeDocumentation()  --  member groups\n");
      MemberGroupListIterator mgli(*memberGroupList);
      MemberGroup *mg;
      for (;(mg=mgli.current());++mgli)
      {
        mg->writeDocumentationPage(ol,scopeName,container);
      }
    }
  }
}

void MemberList::addMemberGroup(MemberGroup *mg)
{
  if (memberGroupList==0)
  {
    memberGroupList=new MemberGroupList;
  }
  //printf("addMemberGroup: this=%p mg=%p\n",this,mg);
  memberGroupList->append(mg);
}

void MemberList::addListReferences(Definition *def)
{
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    if (md->getGroupDef()==0 || def->definitionType()==Definition::TypeGroup)
    {
      md->addListReference(def);
      MemberList *enumFields = md->enumFieldList();
      if (md->memberType()==MemberType_Enumeration && enumFields)
      {
        //printf("  Adding enum values!\n");
        MemberListIterator vmli(*enumFields);
        MemberDef *vmd;
        for ( ; (vmd=vmli.current()) ; ++vmli)
        {
          //printf("   adding %s\n",vmd->name().data());  
          vmd->addListReference(def);
        }
      }
    }
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->addListReferences(def);
    }
  }
}

void MemberList::findSectionsInDocumentation()
{
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->findSectionsInDocumentation();
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->findSectionsInDocumentation();
    }
  }
}

void MemberList::marshal(StorageIntf *s)
{
  marshalInt(s,(int)m_listType);
  marshalInt(s,m_varCnt);
  marshalInt(s,m_funcCnt);
  marshalInt(s,m_enumCnt);
  marshalInt(s,m_enumValCnt);
  marshalInt(s,m_typeCnt);
  marshalInt(s,m_protoCnt);
  marshalInt(s,m_defCnt);
  marshalInt(s,m_friendCnt); 
  marshalInt(s,m_numDecMembers);
  marshalInt(s,m_numDocMembers);
  marshalBool(s,m_inGroup);
  marshalBool(s,m_inFile);
  marshalBool(s,m_needsSorting);
  if (memberGroupList==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,memberGroupList->count());
    QListIterator<MemberGroup> mgi(*memberGroupList);
    MemberGroup *mg=0;
    for (mgi.toFirst();(mg=mgi.current());++mgi)
    {
      mg->marshal(s);
    }
  }
}

void MemberList::unmarshal(StorageIntf *s)
{
  m_listType       = (MemberListType)unmarshalInt(s);
  m_varCnt         = unmarshalInt(s);
  m_funcCnt        = unmarshalInt(s);
  m_enumCnt        = unmarshalInt(s);
  m_enumValCnt     = unmarshalInt(s);
  m_typeCnt        = unmarshalInt(s);
  m_protoCnt       = unmarshalInt(s);
  m_defCnt         = unmarshalInt(s);
  m_friendCnt      = unmarshalInt(s); 
  m_numDecMembers  = unmarshalInt(s);
  m_numDocMembers  = unmarshalInt(s);
  m_inGroup        = unmarshalBool(s);
  m_inFile         = unmarshalBool(s);
  m_needsSorting   = unmarshalBool(s);
  uint i,count     = unmarshalUInt(s); 
  if (count==NULL_LIST) // empty list
  {
    memberGroupList = 0;
  }
  else // add member groups
  {
    memberGroupList = new MemberGroupList;
    for (i=0;i<count;i++)
    {
      MemberGroup *mg = new MemberGroup;
      mg->unmarshal(s);
      memberGroupList->append(mg);
    }
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
    case MemberListType_memberGroup: return "member-group";
    default: break;
  }
  return "";
}

void MemberList::writeTagFile(FTextStream &tagFile)
{
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    if (md->getLanguage()!=SrcLangExt_VHDL)
    {
      md->writeTagFile(tagFile);
      if (md->memberType()==MemberType_Enumeration && md->enumFieldList() && !md->isStrong())
      {
        MemberListIterator vmli(*md->enumFieldList());
        MemberDef *vmd;
        for ( ; (vmd=vmli.current()) ; ++vmli)
        {
          vmd->writeTagFile(tagFile);
        }
      }
    }
    else
    {
      VhdlDocGen::writeTagFile(md,tagFile);
    }
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writeTagFile(tagFile);
    }
  }
}

//--------------------------------------------------------------------------

int MemberSDict::compareValues(const MemberDef *c1, const MemberDef *c2) const
{
  //printf("MemberSDict::compareValues(%s,%s)\n",c1->name().data(),c2->name().data());
  int cmp = qstricmp(c1->name(),c2->name());
  if (cmp)
  {
    return cmp;
  }
  else
  {
    return c1->getDefLine()-c2->getDefLine();
  }
}


