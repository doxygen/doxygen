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
#include <fstream>

#include "resourcemgr.h"
#include "util.h"
#include "version.h"
#include "message.h"
#include "config.h"

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
    p->resources.insert({res.name,res});
  }
}

bool ResourceMgr::writeCategory(const QCString &categoryName,const QCString &targetDir) const
{
  for (auto &kv : p->resources)
  {
    Resource &res = kv.second;
    if (res.category==categoryName)
    {
      std::string pathName = targetDir.str()+"/"+res.name;
      std::ofstream f(pathName,std::ofstream::out | std::ofstream::binary);
      bool ok=false;
      if (f.is_open())
      {
        f.write(reinterpret_cast<const char *>(res.data),res.size);
        ok = !f.fail();
      }
      if (!ok)
      {
        err("Failed to write resource '%s' to directory '%s'\n",res.name,qPrint(targetDir));
        return FALSE;
      }
    }
  }
  return TRUE;
}

bool ResourceMgr::copyResourceAs(const QCString &name,const QCString &targetDir,const QCString &targetName,bool append) const
{
  std::string pathName = targetDir.str()+"/"+targetName.str();
  std::ios_base::openmode mode = std::ofstream::out | std::ofstream::binary;
  if (append) mode |= std::ofstream::app;
  const Resource *res = get(name);
  if (res)
  {
    switch (res->type)
    {
      case Resource::Verbatim:
        {
          std::ofstream f(pathName,mode);
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
      case Resource::Luminance:
        {
          QCString n = name;
          n = n.left(n.length()-4)+".png"; // replace .lum by .png
          const uchar *data = res->data;
          ushort width   = (data[0]<<8)+data[1];
          ushort height  = (data[2]<<8)+data[3];
          ColoredImgDataItem images[2];
          images[0].name    = n.data();
          images[0].width   = width;
          images[0].height  = height;
          images[0].content = &data[4];
          images[0].alpha   = 0;
          images[1].name    = 0; // terminator
          writeColoredImgData(targetDir,images);
          return TRUE;
        }
        break;
      case Resource::LumAlpha:
        {
          QCString n = name;
          n = n.left(n.length()-5)+".png"; // replace .luma by .png
          const uchar *data = res->data;
          ushort width   = (data[0]<<8)+data[1];
          ushort height  = (data[2]<<8)+data[3];
          ColoredImgDataItem images[2];
          images[0].name    = n.data();
          images[0].width   = width;
          images[0].height  = height;
          images[0].content = &data[4];
          images[0].alpha   = &data[4+width*height];
          images[1].name    = 0; // terminator
          writeColoredImgData(targetDir,images);
          return TRUE;
        }
        break;
      case Resource::CSS:
        {
          std::ofstream t(pathName,mode);
          if (t.is_open())
          {
            QCString buf(res->size+1);
            memcpy(buf.rawData(),res->data,res->size);
            buf = replaceColorMarkers(buf);
            if (name=="navtree.css")
            {
              t << substitute(buf,"$width",QCString().setNum(Config_getInt(TREEVIEW_WIDTH))+"px");
            }
            else
            {
              t << substitute(buf,"$doxygenversion",getDoxygenVersion());
            }
            return TRUE;
          }
        }
        break;
      case Resource::SVG:
        {
          std::ofstream t(pathName,mode);
          if (t.is_open())
          {
            QCString buf(res->size+1);
            memcpy(buf.rawData(),res->data,res->size);
            t << replaceColorMarkers(buf);
            return TRUE;
          }
        }
    }
  }
  else
  {
    err("requested resource '%s' not compiled in!\n",qPrint(name));
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
  return 0;
}

QCString ResourceMgr::getAsString(const QCString &name) const
{
  const Resource *res = get(name);
  if (res)
  {
    QCString result(res->size+1);
    memcpy(result.rawData(),res->data,res->size);
    return result;
  }
  else
  {
    return QCString();
  }
}

