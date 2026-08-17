/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#include <mutex>

#include "cache.h"
#include "filename.h"
#include "filedef.h"
#include "dir.h"
#include "portable.h"

/** Cache element for the file name to FileDef mapping cache. */
struct FindFileCacheElem
{
  FindFileCacheElem(FileDef *fd,bool ambig) : fileDef(fd), isAmbig(ambig) {}
  FileDef *fileDef;
  bool isAmbig;
};

static Cache<std::string,FindFileCacheElem> g_findFileDefCache(5000);

static std::mutex g_findFileDefMutex;

FileDef *FileNameLinkedMap::findFileDef(const DString &n, bool &ambig) const
{
  ambig=false;
  if (n.empty()) return nullptr;


  const int maxAddrSize = 20;
  char addr[maxAddrSize];
  snprintf(addr,maxAddrSize,"%p:",reinterpret_cast<const void*>(this));
  DString key = addr;
  key+=n;

  std::lock_guard<std::mutex> lock(g_findFileDefMutex);
  FindFileCacheElem *cachedResult = g_findFileDefCache.find(key.str());
  //printf("key=%s cachedResult=%p\n",qPrint(key),cachedResult);
  if (cachedResult)
  {
    ambig = cachedResult->isAmbig;
    //printf("cached: fileDef=%p\n",cachedResult->fileDef);
    return cachedResult->fileDef;
  }
  else
  {
    cachedResult = g_findFileDefCache.insert(key.str(),FindFileCacheElem(nullptr,false));
  }

  DString name=Dir::cleanDirPath(n.str());
  DString path;
  if (name.empty()) return nullptr;
  size_t sp0 = name.rfind('/');
  size_t sp1 = name.rfind('\\');
  size_t slashPos = sp0!=DString::npos && sp1!=DString::npos ? std::max(sp0,sp1) :
                    sp0!=DString::npos ? sp0 : sp1;
  if (slashPos!=DString::npos)
  {
    path=removeLongPathMarker(name.left(slashPos+1));
    name=name.mid(slashPos+1);
  }
  if (name.empty()) return nullptr;
  const FileName *fn = this->find(name);
  if (fn)
  {
    //printf("fn->size()=%zu\n",fn->size());
    if (fn->size()==1)
    {
      const std::unique_ptr<FileDef> &fd = fn->front();
      bool isSamePath = Portable::fileSystemIsCaseSensitive() ?
                 fd->getPath().right(path.length())==path :
                 fd->getPath().right(path.length()).lower()==path.lower();
      if (path.empty() || isSamePath)
      {
        cachedResult->fileDef = fd.get();
        return fd.get();
      }
    }
    else // file name alone is ambiguous
    {
      int count=0;
      FileDef *lastMatch=nullptr;
      DString pathStripped = stripFromIncludePath(path);
      for (const auto &fd_p : *fn)
      {
        FileDef *fd = fd_p.get();
        DString fdStripPath = stripFromIncludePath(fd->getPath());
        if (fdStripPath == pathStripped)
        {
          // if the stripped paths are equal, we have a perfect match
          count = 1;
          lastMatch=fd;
          break;
        }
        if (path.empty() ||
            (!pathStripped.empty() && fdStripPath.endsWith(pathStripped)) ||
            (pathStripped.empty() && fdStripPath.empty()))
        {
          count++;
          lastMatch=fd;
        }
      }

      ambig=(count>1);
      cachedResult->isAmbig = ambig;
      cachedResult->fileDef = lastMatch;
      return lastMatch;
    }
  }
  else
  {
    //printf("not found!\n");
  }
  return nullptr;
}

DString FileNameLinkedMap::showFileDefMatches(const DString &n) const
{
  DString result;
  DString name=Dir::cleanDirPath(n.str());
  DString path;
  size_t sp0 = name.rfind('/');
  size_t sp1 = name.rfind('\\');
  size_t slashPos = sp0!=DString::npos && sp1!=DString::npos ? std::max(sp0,sp1) :
                    sp0!=DString::npos ? sp0 : sp1;
  if (slashPos!=DString::npos)
  {
    path=removeLongPathMarker(name.left(slashPos+1));
    name=name.mid(slashPos+1);
  }
  const FileName *fn=this->find(name);
  if (fn)
  {
    bool first = true;
    DString pathStripped = stripFromIncludePath(path);
    for (const auto &fd_p : *fn)
    {
      FileDef *fd = fd_p.get();
      DString fdStripPath = stripFromIncludePath(fd->getPath());
      if (path.empty() ||
          (!pathStripped.empty() && fdStripPath.endsWith(pathStripped)) ||
          (pathStripped.empty() && fdStripPath.empty()))
      {
        if (!first) result += "\n";
        else first = false;
        result+="  "+fd->absFilePath();
      }
    }

  }
  return result;
}


