/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
#include "doxygen.h"

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

void ClassList::writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter,const char *header)
{
  if (count()>0)
  {
    ClassDef *cd=first();
    bool found=FALSE;
    while (cd)
    {
      if (cd->name().find('@')==-1 && 
          (filter==0 || *filter==cd->compoundType())
         )
      {
        bool isLink = cd->isLinkable();
        if (isLink || !Config::hideClassFlag)
        {
          if (!found)
          {
            ol.startMemberHeader();
            if (header)
            {
              parseText(ol,header);
            }
            else
            {
              parseText(ol,theTranslator->trCompounds());
            }
            ol.endMemberHeader();
            ol.startMemberList();
            found=TRUE;
          }
          if (!Config::genTagFile.isEmpty()) 
          {
            Doxygen::tagFile << "    <class kind=\"" << cd->compoundTypeString() 
                    << "\">" << convertToXML(cd->name()) << "</class>" << endl;
          }
          ol.startMemberItem(FALSE);
          QCString tmp = cd->compoundTypeString();
          ol.writeString(tmp);
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
          if (!cd->briefDescription().isEmpty())
          {
            ol.startMemberDescription();
            parseDoc(ol,cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->briefDescription());
            if ((!cd->briefDescription().isEmpty() && Config::repeatBriefFlag) ||
                !cd->documentation().isEmpty())
            {
              ol.pushGeneratorState();
              ol.disableAllBut(OutputGenerator::Html);
              ol.endEmphasis();
              ol.docify(" ");
              ol.startTextLink(cd->getOutputFileBase(),"_details");
              parseText(ol,theTranslator->trMore());
              ol.endTextLink();
              ol.startEmphasis();
              ol.popGeneratorState();
            }
            ol.endMemberDescription();
          }
        }
      }
      cd=next();
    }
    if (found) ol.endMemberList();
  }
}
  
