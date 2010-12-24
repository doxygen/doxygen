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

MemberList::MemberList()
{
}

MemberList::MemberList(ListType lt) : m_listType(lt)
{
  memberGroupList=0;
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

int MemberList::compareItems(GCI item1, GCI item2)
{
  static bool sortConstructorsFirst = Config_getBool("SORT_MEMBERS_CTORS_1ST");
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  if (sortConstructorsFirst) {
    int ord1 = c1->isConstructor() ? 2 : (c1->isDestructor() ? 1 : 0);
    int ord2 = c2->isConstructor() ? 2 : (c2->isDestructor() ? 1 : 0);
    if (ord1 > ord2)
      return -1;
    else if (ord2 > ord1)
      return 1;
  }
  return stricmp(c1->name(),c2->name());
}

/*! Count the number of members in this list that are visible in
 *  the declaration part of a compound's documentation page.
 */
void MemberList::countDecMembers(bool countEnumValues)
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
        case MemberDef::Variable:    // fall through
        case MemberDef::Event:       // fall through
        case MemberDef::Property:    m_varCnt++,m_numDecMembers++;  
                                     break;
        case MemberDef::Function:    // fall through
        case MemberDef::Signal:      // fall through
        case MemberDef::DCOP:        // fall through
        case MemberDef::Slot:        if (!md->isRelated() || md->getClassDef())
                                       m_funcCnt++,m_numDecMembers++; 
                                     break;
        case MemberDef::Enumeration: m_enumCnt++,m_numDecMembers++; break;
        case MemberDef::EnumValue:   if (countEnumValues)
                                       m_enumValCnt++,m_numDecMembers++; 
                                     break;
        case MemberDef::Typedef:     m_typeCnt++,m_numDecMembers++; break;
        //case MemberDef::Prototype:   m_protoCnt++,m_numDecMembers++; break;
        case MemberDef::Define:      if (Config_getBool("EXTRACT_ALL") || 
                                         md->argsString() || 
                                         !md->initializer().isEmpty() ||
                                         md->hasDocumentation() 
                                        ) m_defCnt++,m_numDecMembers++;     
                                     break;
        case MemberDef::Friend:      m_friendCnt++,m_numDecMembers++;  
                                     break;
        default:
          err("Error: Unknown member type found for member `%s'\n!",md->name().data());
      }
    }
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->countDecMembers();
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
      if (countEnumValues || md->memberType()!=MemberDef::EnumValue) 
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
        case MemberDef::Define:    // fall through
        case MemberDef::Typedef:   // fall through
        case MemberDef::Variable:  // fall through
        case MemberDef::Function:  // fall through
        case MemberDef::Signal:    // fall through
        case MemberDef::Slot:      // fall through
        case MemberDef::DCOP:      // fall through
        case MemberDef::Property:  // fall through
        case MemberDef::Event:  
          return TRUE;
        case MemberDef::Enumeration: 
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
        case MemberDef::Friend:
          return TRUE;
        case MemberDef::EnumValue: 
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
                       ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd
                      )
{
  //printf("----- writePlainDeclaration() ----\n");
  countDecMembers();
  if (numDecMembers()==0) 
  {
    //printf("  --> no members!\n");
    return; // no members in this list
  }
  //printf("  --> writePlainDeclaration() numDecMembers()=%d\n",
  //     numDecMembers());
  
  ol.pushGeneratorState();

  bool first=TRUE;
  MemberDef *md;
  MemberListIterator mli(*this);
  for ( ; (md=mli.current()); ++mli )
  {
    //printf(">>> Member `%s' type=%d visible=%d\n",
    //    md->name().data(),md->memberType(),md->isBriefSectionVisible());
    if (md->isBriefSectionVisible())
    {
      switch(md->memberType())
      {
        case MemberDef::Define:    // fall through
        //case MemberDef::Prototype: // fall through
        case MemberDef::Typedef:   // fall through
        case MemberDef::Variable:  // fall through
        case MemberDef::Function:  // fall through
        case MemberDef::Signal:    // fall through
        case MemberDef::Slot:      // fall through
        case MemberDef::DCOP:      // fall through
        case MemberDef::Property:  // fall through
        case MemberDef::Event:  
          {
            if (first) ol.startMemberList(),first=FALSE;
            md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup);
            break;
          }
        case MemberDef::Enumeration: 
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
            if (enumVars==0) // show enum here
            {
              //printf("Enum!!\n");
              if (first) ol.startMemberList(),first=FALSE;
              ol.startMemberItem(0);
              ol.writeString("enum ");
              ol.insertMemberAlign();
              md->writeEnumDeclaration(ol,cd,nd,fd,gd);
              ol.endMemberItem();
              if (!md->briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
              {
                ol.startMemberDescription();
                ol.parseDoc(
                    md->briefFile(),md->briefLine(),
                    cd,md,
                    md->briefDescription(),
                    TRUE,
                    FALSE
                    );
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
            }
            md->warnIfUndocumented();
            break;
          }
        case MemberDef::Friend:
          {
            if (first) ol.startMemberList(),first=FALSE;
            md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup);
            break;
          }
        case MemberDef::EnumValue: 
          {
            if (m_inGroup)
            {
              //printf("EnumValue!\n");
              if (first) ol.startMemberList(),first=FALSE;
              md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup);
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
          if (first) ol.startMemberList(),first=FALSE;
          md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup);
        }
        md->setFromAnonymousScope(TRUE);
      }
    }
  }
 
  if (!first) ol.endMemberList(); 

  ol.popGeneratorState();
  //printf("----- end writePlainDeclaration() ----\n");
}

void MemberList::writeDeclarations(OutputList &ol,
             ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
             const char *title,const char *subtitle, bool showEnumValues
             /*, bool inGroup,bool countSubGroups*/)
{
  //printf("----- writeDeclaration() this=%p ----\n",this);
  static bool optimizeVhdl = Config_getBool("OPTIMIZE_OUTPUT_VHDL");

  countDecMembers(showEnumValues); // count member not in group
  Definition *ctx = cd;
  if (ctx==0 && nd) ctx = nd;
  if (ctx==0 && gd) ctx = gd;
  if (ctx==0 && fd) ctx = fd;

  if (numDecMembers()==0) return;
  //printf("%p: MemberList::writeDeclaration(title=`%s',subtitle=`%s')=%d\n",
  //    this,title,subtitle,numDecMembers());
  ol.startMemberHeader(listTypeAsString());
  if (title) 
  {
    ol.parseText(title);
  }
  ol.endMemberHeader();
  if (subtitle) 
  {
    QCString st=subtitle;
    st = st.stripWhiteSpace();
    if (!st.isEmpty())
    {
      ol.startMemberSubtitle();
      ol.parseDoc("[generated]",-1,ctx,0,subtitle,FALSE,FALSE,0,TRUE,FALSE);
      ol.endMemberSubtitle();
    }
  }

  // TODO: Two things need to be worked out for proper VHDL output:
  // 1. Signals and types under the group need to be
  //    formatted to associate them with the group somehow
  //    indentation, or at the very least, extra space after
  //    the group is done
  // 2. This might need to be repeated below for memberGroupLists
  if (optimizeVhdl) // use specific declarations function
  {
    VhdlDocGen::writeVhdlDeclarations(this,ol,0,cd,0);
  }
  else
  {
    writePlainDeclarations(ol,cd,nd,fd,gd);
  }
 
  //printf("memberGroupList=%p\n",memberGroupList);
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    while ((mg=mgli.current()))
    {
      bool hasHeader=!mg->header().isEmpty() && mg->header()!="[NOHEADER]";
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
        ol.parseDoc("[generated]",-1,ctx,0,mg->documentation()+"\n",FALSE,FALSE);
        ol.endMemberGroupDocs();
      }
      ol.startMemberGroup();
      //printf("--- mg->writePlainDeclarations ---\n");
      mg->writePlainDeclarations(ol,cd,nd,fd,gd);
      ol.endMemberGroup(hasHeader);
      ++mgli;
    }
  }
  //printf("----- end writeDeclaration() ----\n");

}

void MemberList::writeDocumentation(OutputList &ol,
                     const char *scopeName, Definition *container,
                     const char *title,bool showEnumValues)
{
  //printf("MemberList::writeDocumentation()\n");

  countDocMembers(showEnumValues);
  if (numDocMembers()==0) return;

  if (title)
  {
    ol.writeRuler();
    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();
  }
  ol.startMemberDocList();
  
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->writeDocumentation(this,ol,scopeName,container,m_inGroup,showEnumValues);
  }
  if (memberGroupList)
  {
    //printf("MemberList::writeDocumentation()  --  member groups\n");
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->writeDocumentation(ol,scopeName,container);
    }
  }
  ol.endMemberDocList();
}

void MemberList::writeDocumentationPage(OutputList &ol,
                     const char *scopeName, Definition *container)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    QCString diskName=md->getOutputFileBase();
    QCString title=md->qualifiedName();
    startFile(ol,diskName,md->name(),title,HLI_None,!generateTreeView,
              container->getOutputFileBase());
    if (!generateTreeView)
    {
      container->writeNavigationPath(ol);
    }
    ol.startContents();

    ol.writeString("<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\">\n"
                   "  <tr>\n"
                   "   <td valign=\"top\">\n");

    container->writeQuickMemberLinks(ol,md);

    ol.writeString("   </td>\n");
    ol.writeString("   <td valign=\"top\">\n");
    
    md->writeDocumentation(this,ol,scopeName,container,m_inGroup);

    ol.writeString("    </td>\n");
    ol.writeString("  </tr>\n");
    ol.writeString("</table>\n");

    ol.endContents();

    if (generateTreeView)
    {
      container->writeNavigationPath(ol);
    }

    endFile(ol,TRUE);
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
      LockingPtr<MemberList> enumFields = md->enumFieldList();
      if (md->memberType()==MemberDef::Enumeration && enumFields!=0)
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
  m_listType       = (MemberList::ListType)unmarshalInt(s);
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

QCString MemberList::listTypeAsString() const
{
  switch(m_listType)
  {
    case pubMethods: return "pub-methods";
    case proMethods: return "pro-methods";
    case pacMethods: return "pac-methods";
    case priMethods: return "pri-methods";
    case pubStaticMethods: return "pub-static-methods";
    case proStaticMethods: return "pro-static-methods";
    case pacStaticMethods: return "pac-static-methods";
    case priStaticMethods: return "pri-static-methods";
    case pubSlots: return "pub-slots";
    case proSlots: return "pro-slots";
    case priSlots: return "pri-slots";
    case pubAttribs: return "pub-attribs";
    case proAttribs: return "pro-attribs";
    case pacAttribs: return "pac-attribs";
    case priAttribs: return "pri-attribs";
    case pubStaticAttribs: return "pub-static-attribs";
    case proStaticAttribs: return "pro-static-attribs";
    case pacStaticAttribs: return "pac-static-attribs";
    case priStaticAttribs: return "pri-static-attribs";
    case pubTypes: return "pub-types";
    case proTypes: return "pro-types";
    case pacTypes: return "pac-types";
    case priTypes: return "pri-types";
    case related: return "related";
    case signals: return "signals";
    case friends: return "friends";
    case dcopMethods: return "dcop-methods";
    case properties: return "properties";
    case events: return "events";
    case decDefineMembers: return "define-members";
    case decProtoMembers: return "proto-members";
    case decTypedefMembers: return "typedef-members";
    case decEnumMembers: return "enum-members";
    case decFuncMembers: return "func-members";
    case decVarMembers: return "var-members";
    case decEnumValMembers: return "enumval-members";
    case decPubSlotMembers: return "pub-slot-members";
    case decProSlotMembers: return "pro-slot-members";
    case decPriSlotMembers: return "pri-slot-members";
    case decSignalMembers: return "signal-members";
    case decEventMembers: return "event-members";
    case decFriendMembers: return "friend-members";
    case decPropMembers: return "prop-members";
    case enumFields: return "enum-fields";
    case memberGroup: return "member-group";
    default: break;
  }
  return "";
}

void MemberList::setNeedsSorting(bool b)
{
  m_needsSorting = b;
}

//--------------------------------------------------------------------------

int MemberSDict::compareItems(GCI item1, GCI item2)
{
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  return stricmp(c1->name(),c2->name());
}


