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

#include <map>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>

#include "tooltip.h"
#include "definition.h"
#include "outputgen.h"
#include "util.h"
#include "filedef.h"
#include "doxygen.h"
#include "config.h"

static std::mutex g_tooltipLock;

struct TooltipData
{
  std::map<std::string,const Definition*> tooltipInfo;
  std::set<std::string> tooltipWritten;
};

class TooltipManager::Private
{
  public:
    std::unordered_map<int, std::unique_ptr<TooltipData> > tooltips;
    std::unique_ptr<TooltipData> &getTooltipData(int id);
};

TooltipManager &TooltipManager::instance()
{
  static TooltipManager s_instance;
  return s_instance;
}

TooltipManager::TooltipManager() : p(std::make_unique<Private>())
{
}

TooltipManager::~TooltipManager()
{
}

static QCString escapeId(const QCString &s)
{
  QCString res=s;
  for (uint i=0;i<res.length();i++) if (!isId(res[i])) res[i]='_';
  return res;
}

std::unique_ptr<TooltipData> &TooltipManager::Private::getTooltipData(int id)
{
  std::lock_guard<std::mutex> lock(g_tooltipLock);
  auto it = tooltips.insert(std::make_pair(id,std::make_unique<TooltipData>())).first;
  return it->second;
}

void TooltipManager::addTooltip(CodeOutputInterface &ol,const Definition *d)
{
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  if (!sourceTooltips) return;
  int outputId = ol.id();
  if (outputId==0) return;

  auto &ttd = p->getTooltipData(outputId);

  QCString id = d->getOutputFileBase();
  int i=id.findRev('/');
  if (i!=-1)
  {
    id = id.right(id.length()-i-1); // strip path (for CREATE_SUBDIRS=YES)
  }
  id+=escapeId(Doxygen::htmlFileExtension);
  QCString anc = d->anchor();
  if (!anc.isEmpty())
  {
    id+="_"+anc;
  }
  id = "a" + id;
  ttd->tooltipInfo.insert(std::make_pair(id.str(),d));
  //printf("%p: addTooltip(%s) ol=%d\n",this,id.data(),ol.id());
}

void TooltipManager::writeTooltips(CodeOutputInterface &ol)
{
  int outputId = ol.id(); // get unique identifier per output file
  if (outputId==0) return; // not set => no HTML output
  auto it = p->tooltips.find(outputId); // see if we have tooltips for this file
  if (it!=p->tooltips.end())
  {
    auto &ttd = it->second;
    for (const auto &kv : ttd->tooltipInfo)
    {
      if (ttd->tooltipWritten.find(kv.first)==ttd->tooltipWritten.end()) // only write tooltips once
      {
        //printf("%p: writeTooltips(%s) ol=%d\n",this,kv.first.c_str(),ol.id());
        const Definition *d = kv.second;
        DocLinkInfo docInfo;
        docInfo.name   = d->qualifiedName();
        docInfo.ref    = d->getReference();
        docInfo.url    = d->getOutputFileBase();
        docInfo.anchor = d->anchor();
        SourceLinkInfo defInfo;
        if (d->getBodyDef() && d->getStartBodyLine()!=-1)
        {
          defInfo.file    = d->getBodyDef()->name();
          defInfo.line    = d->getStartBodyLine();
          defInfo.url     = d->getSourceFileBase();
          defInfo.anchor  = d->getSourceAnchor();
        }
        SourceLinkInfo declInfo; // TODO: fill in...
        QCString decl;
        if (d->definitionType()==Definition::TypeMember)
        {
          const MemberDef *md = toMemberDef(d);
          if (!md->isAnonymous())
          {
            decl = md->declaration();
          }
        }
        ol.writeTooltip(kv.first.c_str(),                // id
            docInfo,                         // symName
            decl,                            // decl
            d->briefDescriptionAsTooltip(),  // desc
            defInfo,
            declInfo
            );
        ttd->tooltipWritten.insert(kv.first);
      }
    }
  }
}

