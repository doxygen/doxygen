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

#include <vector>
#include "membergroup.h"

class Entry;

class DocGroup
{
  public:
    DocGroup() {}

  public:
    void enterFile(const DString &fileName,int);
    void leaveFile(const DString &fileName,int line);
    void enterCompound(const DString &fileName,int line,const DString &name);
    void leaveCompound(const DString &,int,const DString & /*name*/);
    void open(Entry *e,const DString &,int,bool implicit=false);
    void close(Entry *e,const DString &fileName,int line,bool foundInline,bool implicit=false);
    void initGroupInfo(Entry *e);
    bool empty() const;
    void clearHeader();
    void appendHeader(const char);
    void addDocs(Entry *e);

  private:
    int findExistingGroup(const MemberGroupInfo *info);
    int              m_openCount = 0;
    DString         m_memberGroupHeader;
    int              m_memberGroupId = 0;
    DString         m_memberGroupRelates;
    DString         m_memberGroupDocs;
    std::vector<Grouping> m_autoGroupStack;
    DString         m_compoundName;
};

#endif
