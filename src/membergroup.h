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

#ifndef MEMBERGROUP_H
#define MEMBERGROUP_H

#include "qtbc.h"
#include <qlist.h>
#include <qintdict.h>

#include "definition.h"

class MemberDef;
class MemberList;
class OutputList;

class MemberGroup : public Definition
{
  public:
    MemberGroup(int id,const char *header);
   ~MemberGroup();
    QCString header() const { return grpHeader; }
    QCString getOutputFileBase() const { return fileName; }
    void insertMember(MemberDef *);
    void writeDocumentation(OutputList &ol);
    MemberList *members() const { return memberList; } 
    void addDocumentation();

    bool isLinkableInProject();
    bool isLinkable();

  private: 
    MemberList *memberList;             // list of all members in the group
    int grpId;
    QCString grpHeader;
    QCString fileName;                   // base name of the generated file
};

class MemberGroupList : public QList<MemberGroup>
{
};

class MemberGroupListIterator : public QListIterator<MemberGroup>
{
  public:
    MemberGroupListIterator(const MemberGroupList &l) : 
      QListIterator<MemberGroup>(l) {}
};

class MemberGroupDict : public QIntDict<MemberGroup>
{
  public:
    MemberGroupDict(int size) : QIntDict<MemberGroup>(size) {}
};

#endif
