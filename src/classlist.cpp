/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include "classlist.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "language.h"
#include "doc.h"

ClassList::ClassList() : QList<ClassDef>()
{
}

ClassList::~ClassList()
{
}

int ClassList::compareItems(GCI item1, GCI item2)
{
  ClassDef *c1=(ClassDef *)item1;
  ClassDef *c2=(ClassDef *)item2;
  //int prefixLength = Config::ignorePrefix.length();
  //int i1 = c1->name().left(prefixLength)==Config::ignorePrefix ? prefixLength : 0;
  //int i2 = c2->name().left(prefixLength)==Config::ignorePrefix ? prefixLength : 0;
  return stricmp(c1->name().data()+getPrefixIndex(c1->name()),
                 c2->name().data()+getPrefixIndex(c2->name())
                );
}

ClassListIterator::ClassListIterator(const ClassList &cllist) :
  QListIterator<ClassDef>(cllist)
{
}

void ClassList::writeDeclaration(OutputList &ol)
{
  if (count()>0)
  {
    ClassDef *cd=first();
    bool found=FALSE;
    while (cd)
    {
      if (cd->name().find('@')==-1)
      {
        bool isLink = cd->isLinkable();
        if (isLink || !Config::hideClassFlag)
        {
          if (!found)
          {
            ol.startMemberHeader();
            parseText(ol,theTranslator->trCompounds());
            ol.endMemberHeader();
            ol.startMemberList();
            found=TRUE;
          }
          ol.startMemberItem(FALSE);
          switch (cd->compoundType())
          {
            case ClassDef::Class:      ol.writeString("class");  break;
            case ClassDef::Struct:     ol.writeString("struct"); break;
            case ClassDef::Union:      ol.writeString("union");  break;
            case ClassDef::Interface:  ol.writeString("interface");  break;
            case ClassDef::Exception:  ol.writeString("exception");  break;
          }
          ol.writeString(" ");
          ol.insertMemberAlign();
          if (isLink) 
          {
            ol.writeObjectLink(cd->getReference(),
                cd->getOutputFileBase(),
                0,
                cd->name()
               );
          }
          else 
          {
            ol.startBold();
            ol.docify(cd->name());
            ol.endBold();
          }
          ol.endMemberItem(FALSE);
        }
      }
      cd=next();
    }
    if (found) ol.endMemberList();
  }
}
  
