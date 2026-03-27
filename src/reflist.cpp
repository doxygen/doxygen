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
  m_lookup.emplace(m_id,result);
  return result;
}

RefItem *RefList::find(int itemId)
{
  auto it = m_lookup.find(itemId);
  return it!=m_lookup.end() ? it->second : nullptr;
}

bool RefList::isEnabled() const
{
  if      (m_listName=="todo"       && !Config_getBool(GENERATE_TODOLIST))       return false;
  else if (m_listName=="test"       && !Config_getBool(GENERATE_TESTLIST))       return false;
  else if (m_listName=="bug"        && !Config_getBool(GENERATE_BUGLIST))        return false;
  else if (m_listName=="deprecated" && !Config_getBool(GENERATE_DEPRECATEDLIST)) return false;
  return true;
}

void RefList::generatePage()
{
  if (!isEnabled()) return;

  std::stable_sort(m_entries.begin(),m_entries.end(),
            [](const std::unique_ptr<RefItem> &left,const std::unique_ptr<RefItem> &right)
            { return qstricmp_sort(left->title(),right->title()) < 0; });
  //RefItem *item;
  QCString doc;
  int cnt = 0;
  doc += "<dl class=\"reflist\">";
  QCString lastGroup;
  bool first=true;
  for (const std::unique_ptr<RefItem> &item : m_entries)
  {
    if (item->name().isEmpty()) continue;
    cnt++;
    bool startNewGroup = item->group()!=lastGroup;
    if (startNewGroup)
    {
      if (!first)
      {
        doc += "</dd>";
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
  if (cnt>0)
  {
    addRelatedPage(m_listName,m_pageTitle,doc,m_fileName,1,1,RefItemVector(),nullptr,nullptr,TRUE);
  }
}
