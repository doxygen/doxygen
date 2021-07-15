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
#include "util.h"
#include "definition.h"
#include "config.h"
#include "language.h"
#include "message.h"

RefList::RefList(const QCString &listName, const QCString &pageTitle, const QCString &secTitle) :
       m_listName(listName), m_fileName(convertNameToFile(listName,FALSE,TRUE)),
       m_pageTitle(pageTitle), m_secTitle(secTitle)
{
}

RefItem *RefList::add()
{
  m_id++;
  std::unique_ptr<RefItem> item = std::make_unique<RefItem>(m_id,this);
  RefItem *result = item.get();
  m_entries.push_back(std::move(item));
  m_lookup.insert({m_id,result});
  return result;
}

RefItem *RefList::find(int itemId)
{
  auto it = m_lookup.find(itemId);
  return it!=m_lookup.end() ? it->second : nullptr;
}

bool RefList::isEnabled()
{
  if (m_listName=="todo")
  {
    QCString val = Config_getEnum(GENERATE_TODOLIST).upper();
    m_position = (val == "POSITION");
    return (val != "NO");
  }
  else if (m_listName=="test")
  {
    QCString val = Config_getEnum(GENERATE_TESTLIST).upper();
    m_position = (val == "POSITION");
    return (val != "NO");
  }
  else if (m_listName=="bug")
  {
    QCString val = Config_getEnum(GENERATE_BUGLIST).upper();
    m_position = (val == "POSITION");
    return (val != "NO");
  }
  else if (m_listName=="deprecated")
  {
    QCString val = Config_getEnum(GENERATE_DEPRECATEDLIST).upper();
    m_position = (val == "POSITION");
    return (val != "NO");
  }
  return true;
}

void RefList::generatePage()
{
  if (!isEnabled()) return;

  std::sort(m_entries.begin(),m_entries.end(),
            [](std::unique_ptr<RefItem> &left,std::unique_ptr<RefItem> &right)
            { return qstricmp(left->title(),right->title()) < 0; });
  //RefItem *item;
  QCString doc;
  int cnt = 0;
  doc += "<dl class=\"reflist\">";
  QCString lastGroup;
  bool first=true;
  int lineNr;
  QCString fileName;
  for (const std::unique_ptr<RefItem> &item : m_entries)
  {
    if (item->name().isEmpty()) continue;
    cnt++;
    bool startNewGroup = item->group()!=lastGroup;
    if (startNewGroup)
    {
      if (!first)
      {
        doc += createDefLine(fileName,lineNr);
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
      doc += createDefLine(fileName,lineNr);
      doc += "<p>";
    }
    doc += " \\anchor ";
    doc += item->anchor();
    doc += " ";
    doc += item->text();
    lastGroup = item->group();
    first = false;
    lineNr = item->lineNr();
    fileName = item->fileName();
  }
  if (!fileName.isEmpty())
  {
    doc += createDefLine(fileName,lineNr);
  }
  if (!first)
  {
    doc += "</dd>";
  }
  doc += "</dl>\n";
  //printf("generatePage('%s')\n",doc.data());
  if (cnt>0)
  {
    addRelatedPage(m_listName,m_pageTitle,doc,m_fileName,1,1,RefItemVector(),0,0,TRUE);
  }
}

QCString RefList::createDefLine(QCString fileName, int lineNr)
{
  if (!m_position) return "";
  QCString result = "";
  QCString refText = theTranslator->trDefinedAtLineInSourceFile();
  int lineMarkerPos = refText.find("@0");
  int fileMarkerPos = refText.find("@1");
  if (lineMarkerPos!=-1 && fileMarkerPos!=-1) // should always pass this.
  {
    result = "<p><small>";
    if (lineMarkerPos<fileMarkerPos) // line marker before file marker
    {
       result += refText.left(lineMarkerPos);
       result += QCString().setNum(lineNr);
       result += refText.mid(lineMarkerPos+2, fileMarkerPos-lineMarkerPos-2);
       result += stripFromPath(fileName);
       result += refText.right(refText.length()-(uint)fileMarkerPos-2);
    }
    else
    {
       result += refText.left(lineMarkerPos);
       result += stripFromPath(fileName);
       result += refText.mid(lineMarkerPos+2, fileMarkerPos-lineMarkerPos-2);
       result += QCString().setNum(lineNr);
       result += refText.right(refText.length()-(uint)fileMarkerPos-2);
    }
    result += "</small>";
  }
  else
  {
    err("translation error: invalid markers in trDefinedAtLineInSourceFile()\n");
  }
  return result;
}
