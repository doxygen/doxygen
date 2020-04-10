/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include <algorithm>

#include <stdio.h>
#include "reflist.h"
#include "ftextstream.h"
#include "definition.h"
#include "sortdict.h"

void RefList::generatePage()
{
  std::sort(m_entries.begin(),m_entries.end(),
            [](std::unique_ptr<RefItem> &left,std::unique_ptr<RefItem> &right)
            { return qstricmp(left->title(),left->title()); });
  //RefItem *item;
  QCString doc;
  doc += "<dl class=\"reflist\">";
  QCString lastGroup;
  bool first=true;
  for (const std::unique_ptr<RefItem> &item : m_entries)
  {
    bool startNewGroup = item->group()!=lastGroup;
    if (startNewGroup)
    {
      if (!first)
      {
        doc += "</dd>";
        first=false;
      }
      doc += " <dt>";
      doc += "\n";
      if (item->scope())
      {
        if (item->scope()->name() != "<globalScope>")
        {
          doc += "\\_setscope ";
          doc += item->scope()->name();
          doc += " ";
        }
      }
      doc += item->prefix();
      doc += " \\_internalref ";
      doc += item->name();
      // escape \'s in title, see issue #5901
      QCString escapedTitle = substitute(item->title(),"\\","\\\\");
      doc += " \""+escapedTitle+"\" ";
      // write declaration in case a function with arguments
      if (!item->args().isEmpty())
      {
        // escape @'s in argument list, needed for Java annotations (see issue #6208)
        // escape \'s in argument list (see issue #6533)
        doc += substitute(substitute(item->args(),"@","@@"),"\\","\\\\");
      }
      doc += "</dt><dd>";
    }
    else
    {
      doc += "<p>";
    }
    doc += " \\anchor ";
    doc += item->anchor();
    doc += " ";
    doc += item->text();
    lastGroup = item->group();
    first = false;
  }
  if (!first)
  {
    doc += "</dd>";
  }
  doc += "</dl>\n";
  //printf("generatePage('%s')\n",doc.data());
  addRelatedPage(m_listName,m_pageTitle,doc,m_fileName,1,std::vector<RefItem*>(),0,0,TRUE);
}

