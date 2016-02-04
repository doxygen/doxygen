/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <qdict.h>

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
    Private() : tooltipInfo(10007) {}
    QDict<Definition> tooltipInfo;
};

TooltipManager *TooltipManager::s_theInstance = 0;

TooltipManager::TooltipManager() 
{
  p = new Private;
}

TooltipManager::~TooltipManager()
{
  delete p;
}

TooltipManager *TooltipManager::instance()
{
  if (!s_theInstance)
  {
    s_theInstance = new TooltipManager;
  }
  return s_theInstance;
}

void TooltipManager::clearTooltips()
{
  p->tooltipInfo.clear();
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

void TooltipManager::addTooltip(Definition *d)
{
  static bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
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
  if (p->tooltipInfo.find(id)==0)
  {
    p->tooltipInfo.insert(id,d);
  }
}

void TooltipManager::writeTooltips(CodeOutputInterface &ol)
{
  QDictIterator<Definition> di(p->tooltipInfo);
  Definition *d;
  for (di.toFirst();(d=di.current());++di)
  {
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
      MemberDef *md = (MemberDef*)d;
      decl = md->declaration();
      if (!decl.isEmpty() && decl.at(0)=='@') // hide enum values
      {
        decl.resize(0);
      }
    }
    ol.writeTooltip(di.currentKey(),                 // id
                    docInfo,                         // symName
                    decl,                            // decl
                    d->briefDescriptionAsTooltip(),  // desc
                    defInfo,
                    declInfo
                   );
  }
}

