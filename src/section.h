/******************************************************************************
 *
 * $Id$
 *
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

#ifndef SECTION_H
#define SECTION_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>

class Definition;

struct SectionInfo
{
  enum SectionType { Section, Subsection, Anchor };
  SectionInfo(const char *l,const char *t,SectionType st)
    { label=l; title=t; type=st; definition=0; }
  QCString label; 
  QCString title;
  SectionType type;
  Definition *definition;
  QCString fileName;
};

class SectionList : public QList<SectionInfo>
{
};

class SectionDict : public QDict<SectionInfo>
{
  public:
    SectionDict(int size) : QDict<SectionInfo>(size) {}
};

#endif
