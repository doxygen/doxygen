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

#include "qtbc.h"
#include <ctype.h>
#include "definition.h"
#include "doxygen.h"

Definition::Definition(const char *name,const char *b,const char *d)
{
  n=name; brief=b; doc=d; sectionList=0;
}

Definition::~Definition()
{
  delete sectionList;
}

QCString Definition::nameToFile(const char *name)
{
  QCString result;
  char c;
  const char *p=name;
  while ((c=*p++)!=0)
  {
    switch(c)
    {
      case ':': result+="_"; break;
      case '<': result+="_lt"; break;
      case '>': result+="_gt"; break;
      case '*': result+="_ast"; break;
      case '&': result+="_amp"; break;
      case ' ': break;
      default: 
        if (Config::caseSensitiveNames)
          result+=c;
        else
          result+=tolower(c); 
        break;
    }
  }
  return result;
}

void Definition::addSectionsToDefinition(QList<QCString> *anchorList)
{
  if (!anchorList) return;
  QCString *s=anchorList->first();
  while (s)
  {
    SectionInfo *si=0;
    if (!s->isEmpty() && (si=sectionDict[*s]))
    {
      //printf("Add section `%s' to definition `%s'\n",
      //    si->label.data(),n.data());
      if (sectionList==0) sectionList = new SectionList;
      sectionList->append(si);
      si->definition = this;
    }
    s=anchorList->next();
  }
}
