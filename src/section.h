/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include "sortdict.h"

class Definition;

/** Class representing a section in a page */
struct SectionInfo
{
  enum SectionType { Page          = 0, 
                     Section       = 1, 
                     Subsection    = 2, 
                     Subsubsection = 3, 
                     Paragraph     = 4, 
                     Anchor        = 5 
                   };
  SectionInfo(const char *f,const char *l,const char *t,
              SectionType st,int lev,const char *r=0) :
    label(l), title(t), type(st), ref(r), definition(0), 
    fileName(f), generated(FALSE), level(lev)
  { 
  }
  SectionInfo(const SectionInfo &s)
  {
    label=s.label.copy(); title=s.title.copy(); ref=s.ref.copy();
    type =s.type; definition=s.definition;
    fileName=s.fileName.copy(); generated=s.generated;
  }
 ~SectionInfo() {}
  QCString label; 
  QCString title;
  SectionType type;
  QCString ref;
  Definition *definition;
  QCString fileName;
  bool generated;
  int level;
};

/** Unsorted dictionary of SectionInfo objects. */
class SectionDict : public SDict<SectionInfo>
{
  public:
    SectionDict(int size) : SDict<SectionInfo>(size) {}
   ~SectionDict() {}
};

#endif
