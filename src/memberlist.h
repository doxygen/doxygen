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

#ifndef MEMBERLIST_H
#define MEMBERLIST_H

#include <qlist.h>
#include "memberdef.h"

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
    void countDecMembers();
    void countDocMembers();
    int totalCount() const 
         { return varCnt+funcCnt+enumCnt+enumValCnt+typeCnt+
                  protoCnt+defCnt+friendCnt; }

  private:
    int varCnt,funcCnt,enumCnt,enumValCnt,typeCnt,protoCnt,defCnt,friendCnt; 
};

class MemberListIterator : public QListIterator<MemberDef>
{
  public:
    MemberListIterator(const QList<MemberDef> &list);
};

#endif
