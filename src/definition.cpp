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
#include "config.h"
#include "definition.h"
#include "doxygen.h"
#include "language.h"
#include "message.h"
#include "outputlist.h"
#include "scanner.h"

Definition::Definition(const char *name,const char *b,const char *d)
{
  n=name; brief=b; doc=d; sectionList=0, bodyLine=-1, bodyDef=0;
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

void Definition::setBriefDescription(const char *b) 
{ 
  brief=QCString(b).stripWhiteSpace(); 
  int bl=brief.length(); 
  if (bl>0) // add puntuation if needed
  {
    switch(brief.at(bl-1))
    {
      case '.': case '!': case '?': break;
      default: brief+='.'; break;
    }
  }
}

/*! Write a reference to the source code defining this definition */
void Definition::writeSourceRef(OutputList &ol)
{
  //printf("Definition::writeSourceRef %d %p\n",bodyLine,bodyDef);
  if (Config::sourceBrowseFlag && bodyLine!=-1 && bodyDef)
  {
    ol.newParagraph();

    QCString refText = theTranslator->trDefinedAtLineInSourceFile();
    int lineMarkerPos = refText.find("@0");
    int fileMarkerPos = refText.find("@1");
    if (lineMarkerPos!=-1 && fileMarkerPos!=-1) // should always pass this.
    {
      QString lineStr,anchorStr;
      lineStr.sprintf("%d",bodyLine);
      anchorStr.sprintf("l%05d",bodyLine);
      if (lineMarkerPos<fileMarkerPos) // line marker before file marker
      {
        // write text left from linePos marker
        parseText(ol,refText.left(lineMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            anchorStr,lineStr);
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(lineStr);
        ol.enableAll();
        
        // write text between markers
        parseText(ol,refText.mid(lineMarkerPos+2,
              fileMarkerPos-lineMarkerPos-2));

        ol.disableAllBut(OutputGenerator::Html); 
        // write file link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            0,bodyDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(bodyDef->name());
        ol.enableAll();
        
        // write text right from file marker
        parseText(ol,refText.right(
              refText.length()-fileMarkerPos-2)); 
      }
      else // file marker before line marker
      {
        // write text left from file marker
        parseText(ol,refText.left(fileMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write file link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            0,bodyDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(bodyDef->name());
        ol.enableAll();
        
        // write text between markers
        parseText(ol,refText.mid(fileMarkerPos+2,
              lineMarkerPos-fileMarkerPos-2)); 

        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            anchorStr,lineStr);
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(lineStr);
        ol.enableAll();

        // write text right from linePos marker
        parseText(ol,refText.right(
              refText.length()-lineMarkerPos-2)); 
      }
    }
    else
    {
      err("Error: translation error: invalid markers in trDefinedInSourceFile()\n");
    }
  }
}

bool Definition::hasDocumentation() 
{ 
  return !doc.isNull() ||              // has detailed docs
         !brief.isNull() ||            // has brief description
         (Config::sourceBrowseFlag && 
          bodyLine!=-1 && 
          bodyDef
         ) ||                          // has a source reference
         Config::extractAllFlag;       // extract everything
}

