/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#ifndef MEMBERLIST_H
#define MEMBERLIST_H

#include <qlist.h>
#include "memberdef.h"
class GroupDef;
class MemberGroup;
class MemberGroupList;

class MemberList : public QList<MemberDef> 
{ 
  public:
    MemberList();
   ~MemberList();
    bool insert(uint index,const MemberDef *md);
    void inSort(const MemberDef *md);
    void append(const MemberDef *md);
    int compareItems(GCI item1,GCI item2);
    int varCount() const       { return varCnt;     }
    int funcCount() const      { return funcCnt;    }
    int enumCount() const      { return enumCnt;    }
    int enumValueCount() const { return enumValCnt; }
    int typedefCount() const   { return typeCnt;    }
    int protoCount() const     { return protoCnt;   }
    int defineCount() const    { return defCnt;     }
    int friendCount() const    { return friendCnt;  }
    void countDecMembers(bool inGroup,bool countSubGroups,bool sectionPerType);
    void countDocMembers(bool listOfGroup=FALSE);
    int totalCount() const 
         { return //varCnt+funcCnt+enumCnt+enumValCnt+typeCnt+
                  //protoCnt+defCnt+friendCnt; 
                  m_count;
         }
    void writePlainDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool inGroup=FALSE,bool countSubGroups=TRUE);
    void writeDeclarations(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               const char *title,const char *subtitle,
               bool inGroup=FALSE,bool countSubGroups=TRUE);
    void writeDocumentation(OutputList &ol,const char *scopeName,
               Definition *container);
    void addMemberGroup(MemberGroup *mg);

  private:
    int varCnt,funcCnt,enumCnt,enumValCnt,typeCnt,protoCnt,defCnt,friendCnt; 
    int m_count;
    MemberGroupList *memberGroupList;
};

class MemberListIterator : public QListIterator<MemberDef>
{
  public:
    MemberListIterator(const QList<MemberDef> &list);
};

class MemberDict : public QDict<MemberDef>
{
  public:
    MemberDict(int size) : QDict<MemberDef>(size) {}
   ~MemberDict() {}
};


#endif
