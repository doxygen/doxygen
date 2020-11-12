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
#include <string>

#include "tooltip.h"
#include "definition.h"
#include "outputgen.h"
#include "util.h"
#include "filedef.h"
#include "doxygen.h"
#include "config.h"

class TooltipManager::Private
{
  public:
    std::map<std::string,const Definition*> tooltipInfo;
};

TooltipManager::TooltipManager() : p(std::make_unique<Private>())
{
}

TooltipManager::~TooltipManager()
{
}

static QCString escapeId(const char *s)
{
  QCString res=s;
  char *p=res.rawData();
  while (*p)
  {
    if (!isId(*p)) *p='_';
    p++;
  }
  return res;
}

void TooltipManager::addTooltip(const Definition *d)
{
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  if (!sourceTooltips) return;
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
  p->tooltipInfo.insert({id.str(),d});
}

void TooltipManager::writeTooltips(CodeOutputInterface &ol)
{
  for (const auto &kv : p->tooltipInfo)
  {
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
  }
}

