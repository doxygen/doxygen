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
#include <unordered_set>
#include <string>
#include <mutex>

#include "tooltip.h"
#include "definition.h"
#include "outputlist.h"
#include "util.h"
#include "filedef.h"
#include "doxygen.h"
#include "config.h"

static std::mutex                                                g_tooltipsFileMutex;
static std::mutex                                                g_tooltipsTipMutex;
static std::unordered_map<int, std::unordered_set<std::string> > g_tooltipsWrittenPerFile;

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

static QCString escapeId(const QCString &s)
{
  QCString res=s;
  for (size_t i=0;i<res.length();i++) if (!isId(res[i])) res[i]='_';
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
  // In case an extension is present translate this extension to something understood by the tooltip handler
  // otherwise extend t with a translated htmlFileExtension.
  QCString currentExtension = getFileNameExtension(id);
  if (currentExtension.isEmpty())
  {
    id += escapeId(Doxygen::htmlFileExtension);
  }
  else
  {
    id = stripExtensionGeneral(id,currentExtension) + escapeId(currentExtension);
  }

  QCString anc = d->anchor();
  if (!anc.isEmpty())
  {
    id+="_"+anc;
  }
  id = "a" + id;
  p->tooltipInfo.emplace(id.str(),d);
  //printf("%p: addTooltip(%s)\n",this,id.data());
}

void TooltipManager::writeTooltips(OutputCodeList &ol)
{
  std::unordered_map<int, std::unordered_set<std::string> >::iterator it;
  // critical section
  {
    std::lock_guard<std::mutex> lock(g_tooltipsFileMutex);

    int id = ol.id();
    it = g_tooltipsWrittenPerFile.find(id);
    if (it==g_tooltipsWrittenPerFile.end()) // new file
    {
      it = g_tooltipsWrittenPerFile.emplace(id,std::unordered_set<std::string>()).first;
    }
  }

  for (const auto &[name,d] : p->tooltipInfo)
  {
    bool written = false;

    // critical section
    {
      std::lock_guard<std::mutex> lock(g_tooltipsTipMutex);
      written = it->second.find(name)!=it->second.end();
      if (!written) // only write tooltips once
      {
        it->second.insert(name); // remember we wrote this tooltip for the given file id
      }
    }

    if (!written)
    {
      //printf("%p: writeTooltips(%s) ol=%d\n",this,name.c_str(),ol.id());
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
      ol.writeTooltip(name.c_str(),    // id
          docInfo,                         // symName
          decl,                            // decl
          d->briefDescriptionAsTooltip(),  // desc
          defInfo,
          declInfo
          );
    }
  }
}

