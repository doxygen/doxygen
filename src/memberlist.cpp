/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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

MemberList::MemberList() : QList<MemberDef>()
{
  memberGroupList=0;
  m_numDecMembers=-1; // special value indicating that computation is needed
  m_numDocMembers=-1; // special value indicating that computation is needed
  m_inGroup=FALSE;
}

MemberList::~MemberList()
{
  delete memberGroupList;
}

int MemberList::compareItems(GCI item1, GCI item2)
{
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  return stricmp(c1->name(),c2->name());
}

/*! Count the number of members in this list that are visible in
 *  the declaration part of a compound's documentation page.
 */
void MemberList::countDecMembers()
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
        case MemberDef::EnumValue:   m_enumValCnt++,m_numDecMembers++; break;
        case MemberDef::Typedef:     m_typeCnt++,m_numDecMembers++; break;
        case MemberDef::Prototype:   m_protoCnt++,m_numDecMembers++; break;
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
  if (memberGroupList /*&& countSubGroups*/)
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

void MemberList::countDocMembers()
{
  if (m_numDocMembers!=-1) return; // used cached value
  m_numDocMembers=0;
  QListIterator<MemberDef> mli(*this);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    if (md->isDetailedSectionVisible(m_inGroup)) 
    {
      // do not count enum values, since they do not produce entries of their own
      if (md->memberType()!=MemberDef::EnumValue) m_numDocMembers++;
    }
  }
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->countDecMembers();
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

void MemberList::writePlainDeclarations(OutputList &ol,
                       ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd
                      )
{
  //printf("----- writePlainDeclaration() ----\n");
  countDecMembers();
  if (numDecMembers()==0) return; // no members in this list
  //printf("----> writePlainDeclaration() numDecMembers()=%d\n",
  //     numDecMembers());
  
  ol.pushGeneratorState();

  bool first=TRUE;
  MemberDef *md;
  MemberListIterator mli(*this);
  for ( ; (md=mli.current()); ++mli )
  {
    if (md->isBriefSectionVisible())
    {
      //printf(">>> Member `%s' type=%d\n",md->name().data(),md->memberType());
      switch(md->memberType())
      {
        case MemberDef::Define:    // fall through
        case MemberDef::Prototype: // fall through
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
            if (first) ol.startMemberList(),first=FALSE;
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
            // if this is an anoymous enum and there are variables of this
            // enum type (i.e. enumVars>0), then we do not show the enum here.
            if (enumVars==0) // show enum here
            {
              ol.startMemberItem(0);
              ol.writeString("enum ");
              ol.insertMemberAlign();
              //ol+=typeDecl; // append the enum values.
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
                  //ol.endEmphasis();
                  ol.docify(" ");
                  if (md->getGroupDef()!=0 && gd==0) // forward link to group
                  {
                    ol.startTextLink(md->getOutputFileBase(),
                                     md->anchor());
                  }
                  else
                  {
                    ol.startTextLink(0,md->anchor());
                  }
                  ol.parseText(theTranslator->trMore());
                  ol.endTextLink();
                  //ol.startEmphasis();
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
        //printf("anonymous compound members\n");
        if (md->isBriefSectionVisible())
        {
          if (first) ol.startMemberList(),first=FALSE;
          md->setFromAnonymousScope(FALSE);
          md->writeDeclaration(ol,cd,nd,fd,gd,m_inGroup);
          md->setFromAnonymousScope(TRUE);
        }
      }
    }
  }
 
  if (!first) ol.endMemberList(); 

  ol.popGeneratorState();
  //printf("----- end writePlainDeclaration() ----\n");
}

void MemberList::writeDeclarations(OutputList &ol,
             ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
             const char *title,const char *subtitle
             /*, bool inGroup,bool countSubGroups*/)
{
  //printf("MemberList::writeDeclaration(title=`%s',subtitle=`%s')\n",title,subtitle);
  //printf("----- writeDeclaration() ----\n");
  countDecMembers(); // count member not in group
  if (numDecMembers()==0) return;
  if (title) 
  {
    ol.startMemberHeader();
    ol.parseText(title);
    ol.endMemberHeader();
  }
  if (subtitle && subtitle[0]!=0) 
  {
    //printf("subtitle=`%s'\n",subtitle);
    ol.startMemberSubtitle();
    ol.parseDoc("<generated>",1,0,0,subtitle,FALSE,FALSE);
    ol.endMemberSubtitle();
  }

  writePlainDeclarations(ol,cd,nd,fd,gd);
  
  //printf("memberGroupList=%p\n",memberGroupList);
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    while ((mg=mgli.current()))
    {
      bool hasHeader=mg->header()!="[NOHEADER]";
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
        ol.parseDoc("<generated>",1,0,0,mg->documentation()+"\n",FALSE,FALSE);
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
                     const char *title)
{
  //printf("MemberList::writeDocumentation()\n");

  countDocMembers();
  if (numDocMembers()==0) return;

  if (title)
  {
    ol.writeRuler();
    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();
  }
  
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->writeDocumentation(this,ol,scopeName,container,m_inGroup);
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
}

void MemberList::addMemberGroup(MemberGroup *mg)
{
  if (memberGroupList==0)
  {
    memberGroupList=new MemberGroupList;
  }
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
      if (md->memberType()==MemberDef::Enumeration && md->enumFieldList())
      {
        MemberListIterator vmli(*md->enumFieldList());
        MemberDef *vmd;
        for ( ; (vmd=vmli.current()) ; ++vmli)
        {
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

//--------------------------------------------------------------------------

int MemberSDict::compareItems(GCI item1, GCI item2)
{
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  return stricmp(c1->name(),c2->name());
}

