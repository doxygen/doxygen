/******************************************************************************
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
  //return stricmp(c1->localName().data()+getPrefixIndex(c1->localName()),
  //               c2->localName().data()+getPrefixIndex(c2->localName())
  //              );
  return stricmp(c1->className().data()+getPrefixIndex(c1->className()),
                 c2->className().data()+getPrefixIndex(c2->className()));
}

int ClassSDict::compareItems(GCI item1, GCI item2)
{
  ClassDef *c1=(ClassDef *)item1;
  ClassDef *c2=(ClassDef *)item2;
  //return stricmp(c1->localName().data()+getPrefixIndex(c1->localName()),
  //               c2->localName().data()+getPrefixIndex(c2->localName())
  //              );
  return stricmp(c1->className().data()+getPrefixIndex(c1->className()),
                 c2->className().data()+getPrefixIndex(c2->className()));
}

ClassListIterator::ClassListIterator(const ClassList &cllist) :
  QListIterator<ClassDef>(cllist)
{
}

void ClassSDict::writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter,const char *header)
{
  if (count()>0)
  {
    ClassSDict::Iterator sdi(*this);
    ClassDef *cd=0;
    bool found=FALSE;
    for (sdi.toFirst();(cd=sdi.current());++sdi)
    {
      if (cd->name().find('@')==-1 && 
          (filter==0 || *filter==cd->compoundType())
         )
      {
        bool isLink = cd->isLinkable();
        if (isLink || !Config_getBool("HIDE_UNDOC_CLASSES"))
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
          if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
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
                cd->localName()
               );
          }
          else 
          {
            ol.startBold();
            ol.docify(cd->localName());
            ol.endBold();
          }
          ol.endMemberItem(FALSE);
          if (!cd->briefDescription().isEmpty())
          {
            ol.startMemberDescription();
            parseDoc(ol,cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->briefDescription());
            if ((!cd->briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) ||
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
    }
    if (found) ol.endMemberList();
  }
}
  
