/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#ifndef SECTION_H
#define SECTION_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>
#include "sortdict.h"

class Definition;
class PageInfo;

struct SectionInfo
{
  enum SectionType { Page, Section, Subsection, 
                     Subsubsection, Paragraph, Anchor 
                   };
  SectionInfo(const char *f,const char *l,const char *t,
              SectionType st,const char *r=0)
    { label=l; title=t; type=st; ref=r; 
      definition=0; pageRef=0; generated=FALSE; 
      fileName=f;
    }
 ~SectionInfo() {}
  QCString label; 
  QCString title;
  QCString ref;
  SectionType type;
  Definition *definition;
  PageInfo *pageRef;
  QCString fileName;
  bool generated;
};

class SectionDict : public QDict<SectionInfo>
{
  public:
    SectionDict(int size) : QDict<SectionInfo>(size) {}
   ~SectionDict() {}
};

#endif
