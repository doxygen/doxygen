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

#include <map>
#include <string.h>
#include <cstdint>

#include "resourcemgr.h"
#include "util.h"
#include "version.h"
#include "message.h"
#include "config.h"
#include "portable.h"

class ResourceMgr::Private
{
  public:
    std::map<std::string,Resource> resources;
};

ResourceMgr &ResourceMgr::instance()
{
  static ResourceMgr theInstance;
  return theInstance;
}

ResourceMgr::ResourceMgr() : p(std::make_unique<Private>())
{
}

ResourceMgr::~ResourceMgr()
{
}

void ResourceMgr::registerResources(std::initializer_list<Resource> resources)
{
  for (auto &res : resources)
  {
    p->resources.emplace(res.name,res);
  }
}

bool ResourceMgr::writeCategory(const QCString &categoryName,const QCString &targetDir) const
{
  for (auto &[name,res] : p->resources)
  {
    if (res.category==categoryName)
    {
      QCString pathName = targetDir+"/"+res.name;
      std::ofstream f = Portable::openOutputStream(pathName);
      bool ok=false;
      if (f.is_open())
      {
        f.write(reinterpret_cast<const char *>(res.data),res.size);
        ok = !f.fail();
      }
      if (!ok)
      {
        err("Failed to write resource '{}' to directory '{}'\n",res.name,targetDir);
        return FALSE;
      }
    }
  }
  return TRUE;
}

bool ResourceMgr::copyResourceAs(const QCString &name,const QCString &targetDir,const QCString &targetName,bool append) const
{
  QCString pathName = targetDir+"/"+targetName;
  const Resource *res = get(name);
  if (res)
  {
    switch (res->type)
    {
      case Resource::Verbatim:
        {
          std::ofstream f = Portable::openOutputStream(pathName,append);
          bool ok=false;
          if (f.is_open())
          {
            f.write(reinterpret_cast<const char *>(res->data),res->size);
            ok = !f.fail();
          }
          if (ok)
          {
            return TRUE;
          }
        }
        break;
      case Resource::SVG:
        {
          std::ofstream t = Portable::openOutputStream(pathName,append);
          if (t.is_open())
          {
            QCString buf(res->size, QCString::ExplicitSize);
            memcpy(buf.rawData(),res->data,res->size);
            t << replaceColorMarkers(buf);
            return TRUE;
          }
        }
    }
  }
  else
  {
    err("requested resource '{}' not compiled in!\n",name);
  }
  return FALSE;
}

bool ResourceMgr::copyResource(const QCString &name,const QCString &targetDir) const
{
  return copyResourceAs(name,targetDir,name);
}

const Resource *ResourceMgr::get(const QCString &name) const
{
  auto it = p->resources.find(name.str());
  if (it!=p->resources.end()) return &it->second;
  return nullptr;
}

QCString ResourceMgr::getAsString(const QCString &name) const
{
  const Resource *res = get(name);
  if (res)
  {
    QCString result(res->size, QCString::ExplicitSize);
    memcpy(result.rawData(),res->data,res->size);
    return result;
  }
  else
  {
    return QCString();
  }
}

