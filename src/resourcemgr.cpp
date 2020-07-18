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
#include <qfile.h>
#include <qcstring.h>
#include <qglobal.h>
#include <string.h>

#include "resourcemgr.h"
#include "util.h"
#include "version.h"
#include "ftextstream.h"
#include "message.h"
#include "config.h"

class ResourceMgr::Private
{
  public:
    Private() : resources(257) {}
    QDict<Resource> resources;
};

ResourceMgr &ResourceMgr::instance()
{
  static ResourceMgr theInstance;
  return theInstance;
}

ResourceMgr::ResourceMgr()
{
  p = new Private;
}

ResourceMgr::~ResourceMgr()
{
  delete p;
}

void ResourceMgr::registerResources(const Resource resources[],int numResources)
{
  for (int i=0;i<numResources;i++)
  {
    p->resources.insert(resources[i].name,&resources[i]);
  }
}

bool ResourceMgr::writeCategory(const char *categoryName,const char *targetDir) const
{
  QDictIterator<Resource> it(p->resources);
  const Resource *res;
  for (it.toFirst();(res=it.current());++it)
  {
    if (qstrcmp(res->category,categoryName)==0)
    {
      QCString pathName = QCString(targetDir)+"/"+res->name;
      QFile f(pathName);
      if (!f.open(IO_WriteOnly) || f.writeBlock((const char *)res->data,res->size)!=res->size)
      {
        err("Failed to write resource '%s' to directory '%s'\n",res->name,targetDir);
        return FALSE;
      }
    }
  }
  return TRUE;
}

bool ResourceMgr::copyResourceAs(const char *name,const char *targetDir,const char *targetName) const
{
  QCString pathName = QCString(targetDir)+"/"+targetName;
  const Resource *res = get(name);
  if (res)
  {
    switch (res->type)
    {
      case Resource::Verbatim:
        {
          QFile f(pathName);
          if (f.open(IO_WriteOnly) && f.writeBlock((const char *)res->data,res->size)==res->size)
          {
            return TRUE;
          }
        }
        break;
      case Resource::Luminance:
        {
          QCString n = name;
          n = n.left(n.length()-4)+".png"; // replace .lum by .png
          uchar *data = (uchar*)res->data;
          ushort width   = (data[0]<<8)+data[1];
          ushort height  = (data[2]<<8)+data[3];
          ColoredImgDataItem images[2];
          images[0].name    = n;
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
          uchar *data = (uchar*)res->data;
          ushort width   = (data[0]<<8)+data[1];
          ushort height  = (data[2]<<8)+data[3];
          ColoredImgDataItem images[2];
          images[0].name    = n;
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
          QFile f(pathName);
          if (f.open(IO_WriteOnly))
          {
            QCString buf(res->size+1);
            memcpy(buf.rawData(),res->data,res->size);
            FTextStream t(&f);
            buf = replaceColorMarkers(buf);
            if (qstrcmp(name,"navtree.css")==0)
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
          QFile f(pathName);
          if (f.open(IO_WriteOnly))
          {
            QCString buf(res->size+1);
            memcpy(buf.rawData(),res->data,res->size);
            FTextStream t(&f);
            t << replaceColorMarkers(buf);
            return TRUE;
          }
        }
    }
  }
  else
  {
    err("requested resource '%s' not compiled in!\n",name);
  }
  return FALSE;
}

bool ResourceMgr::copyResource(const char *name,const char *targetDir) const
{
  return copyResourceAs(name,targetDir,name);
}

const Resource *ResourceMgr::get(const char *name) const
{
  return p->resources.find(name);
}

QCString ResourceMgr::getAsString(const char *name) const
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

