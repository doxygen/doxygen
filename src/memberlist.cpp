/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include "memberlist.h"
#include "classdef.h"
#include "message.h"
#include <qregexp.h>

MemberList::MemberList() : QList<MemberDef>()
{
}

MemberList::~MemberList()
{
}

int MemberList::compareItems(GCI item1, GCI item2)
{
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  return strcmp(c1->name(),c2->name());
}

void MemberList::countDecMembers()
{
  varCnt=funcCnt=enumCnt=enumValCnt=typeCnt=protoCnt=defCnt=friendCnt=0;
  MemberDef *md=first();
  while (md)
  {
    if ((!hideMemberFlag || md->hasDocumentation()) &&
        (!hideMemberFlag || !md->documentation().isEmpty() || 
         briefMemDescFlag || repeatBriefFlag
        ) || extractAllFlag || 
        (md->isEnumerate() &&
         md->hasDocumentedEnumValues()
        )
       )
    {
      switch(md->memberType())
      {
        case MemberDef::Variable:    varCnt++;     break;
        case MemberDef::Function:    // fall through
        case MemberDef::Signal:      // fall through
        case MemberDef::Slot:        funcCnt++;    break;
        case MemberDef::Enumeration: enumCnt++;    break;
        case MemberDef::EnumValue:   enumValCnt++; break;
        case MemberDef::Typedef:     typeCnt++;    break;
        case MemberDef::Prototype:   protoCnt++;   break;
        case MemberDef::Define:      if (extractAllFlag || md->argsString() || md->hasDocumentation() ) 
                                       defCnt++;     
                                     break;
        case MemberDef::Friend:      friendCnt++;  break;
        default:
          err("Error: Unknown member type found!");
      }
    }
    md=next();
  }
}

void MemberList::countDocMembers()
{
  varCnt=funcCnt=enumCnt=enumValCnt=typeCnt=protoCnt=defCnt=friendCnt;
  MemberDef *md=first();
  while (md)
  {
    //if (extractAllFlag || /*md->hasDocumentation()*/
    //    !md->documentation().isEmpty() ||
    //    (!md->briefDescription().isEmpty() && 
    //     !briefMemDescFlag && 
    //     repeatBriefFlag 
    //    )
    //   )
    if (extractAllFlag || md->detailsAreVisible())
    {
      QRegExp r("@[0-9]+");
      int dummy;
      switch(md->memberType())
      {
        case MemberDef::Variable:    
          varCnt++;     
          break;
        case MemberDef::Function:    
        case MemberDef::Signal:
        case MemberDef::Slot:
          funcCnt++;    
          break;
        case MemberDef::Enumeration:
          if (r.match(md->name(),0,&dummy)==-1)
          {
            enumCnt++;
          }
          break;
        case MemberDef::EnumValue:   
          {
            MemberDef *scope;
            scope=md->getEnumScope();
            if (scope && r.match(scope->name(),0,&dummy)!=-1) 
              enumValCnt++;
          }
          break;
        case MemberDef::Typedef:     
          typeCnt++;    
          break;
        case MemberDef::Prototype:   
          protoCnt++;   
          break;
        case MemberDef::Define:      
          defCnt++;
          break;
        case MemberDef::Friend:
          friendCnt++;
      }
    }
    md=next();
  }
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

