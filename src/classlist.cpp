/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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
#include "doxygen.h"

ClassList::ClassList() : QList<ClassDef>()
{
}

ClassList::~ClassList()
{
}

static int compItems(void *item1,void *item2)
{
  ClassDef *c1=(ClassDef *)item1;
  ClassDef *c2=(ClassDef *)item2;
  int p1=0,p2=0;
  static bool b = Config_getBool("SORT_BY_SCOPE_NAME");
  if (!b) 
  { 
    p1=getPrefixIndex(c1->className()); 
    p2=getPrefixIndex(c2->className()); 
  }
  return stricmp(c1->className().data()+p1,
                 c2->className().data()+p2);
}

int ClassList::compareItems(GCI item1, GCI item2)
{
  return compItems(item1,item2);
}

int ClassSDict::compareItems(GCI item1, GCI item2)
{
  return compItems(item1,item2);
}

ClassListIterator::ClassListIterator(const ClassList &cllist) :
  QListIterator<ClassDef>(cllist)
{
}

void ClassSDict::writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter,
                                  const char *header,bool localNames)
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
        if (isLink || 
             (!Config_getBool("HIDE_UNDOC_CLASSES") && 
              (!cd->isLocal() || Config_getBool("EXTRACT_LOCAL_CLASSES"))
             )
           )
        {
          if (!found)
          {
            ol.startMemberHeader();
            if (header)
            {
              ol.parseText(header);
            }
            else
            {
              ol.parseText(theTranslator->trCompounds());
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
          QCString cname;
          if (localNames)
          {
            cname = cd->localName();
          }
          else
          {
            cname = cd->displayName();
          }
          ol.writeString(tmp);
          ol.writeString(" ");
          ol.insertMemberAlign();
          if (isLink) 
          {
            ol.writeObjectLink(cd->getReference(),
                cd->getOutputFileBase(),
                0,
                cname
               );
          }
          else 
          {
            ol.startBold();
            ol.docify(cname);
            ol.endBold();
          }
          ol.endMemberItem();
          if (!cd->briefDescription().isEmpty())
          {
            ol.startMemberDescription();
            ol.parseDoc(cd->briefFile(),cd->briefLine(),cd,0,
                        cd->briefDescription(),FALSE,FALSE);
            if (//(!cd->briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) ||
                //!cd->documentation().isEmpty())
                cd->isLinkableInProject()
               )
            {
              ol.pushGeneratorState();
              ol.disableAllBut(OutputGenerator::Html);
              //ol.endEmphasis();
              ol.docify(" ");
              ol.startTextLink(cd->getOutputFileBase(),"_details");
              ol.parseText(theTranslator->trMore());
              ol.endTextLink();
              //ol.startEmphasis();
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
  
