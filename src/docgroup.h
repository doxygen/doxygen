/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#ifndef DOCGROUP_H
#define DOCGROUP_H

#include <qstack.h>
#include <qstring.h>
#include "membergroup.h"

class Entry;

class DocGroup
{
  public:
    DocGroup() {};

  public:
    void enterFile(const char *fileName,int);
    void leaveFile(const char *fileName,int line);
    void enterCompound(const char *fileName,int line,const char *name);
    void leaveCompound(const char *,int,const char * /*name*/);
    void open(Entry *e,const char *,int,bool implicit=false);
    void close(Entry *e,const char *fileName,int line,bool foundInline,bool imlicit=false);
    void initGroupInfo(Entry *e);
    bool isEmpty() const;
    void clearHeader();
    void appendHeader(const char);
    void addDocs(Entry *e);

  private:
    int findExistingGroup(int &groupId,const MemberGroupInfo *info);
    int              m_openCount;
    QCString         m_memberGroupHeader;
    int              m_memberGroupId;
    QCString         m_memberGroupRelates;
    QCString         m_memberGroupDocs;
    QStack<Grouping> m_autoGroupStack;
    QCString         m_compoundName;
};

#endif
