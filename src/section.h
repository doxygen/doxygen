/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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

struct SectionInfo
{
  enum SectionType { Page, Section, Subsection, 
                     Subsubsection, Paragraph, Anchor 
                   };
  SectionInfo(const char *f,const char *l,const char *t,
              SectionType st,const char *r=0) :
    label(l), title(t), type(st), ref(r), definition(0), 
    fileName(f), generated(FALSE)
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
};

class SectionDict : public QDict<SectionInfo>
{
  public:
    SectionDict(int size) : QDict<SectionInfo>(size) {}
   ~SectionDict() {}
};

#endif
