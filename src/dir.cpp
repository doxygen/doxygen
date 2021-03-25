/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include "filesystem.hpp"
#include "dir.h"

namespace fs = ghc::filesystem;

//-----------------------------------------------------------------------------------------------

struct DirEntry::Private
{
  fs::directory_entry entry;
};

DirEntry::DirEntry() : p(std::make_unique<Private>())
{
}

DirEntry::~DirEntry()
{
}

bool DirEntry::is_directory() const
{
  return p->entry.is_directory();
}

bool DirEntry::is_regular_file() const
{
  return p->entry.is_regular_file();
}

bool DirEntry::is_symlink() const
{
  return p->entry.is_symlink();
}

std::string DirEntry::path() const
{
  return p->entry.path().string();
}

//-----------------------------------------------------------------------------------------------

struct DirIterator::Private
{
  Private() : it() {}
  Private(const std::string &path) : it(path) {}
  fs::directory_iterator it;
  mutable DirEntry current;
};

DirIterator::DirIterator() : p(std::make_unique<Private>())
{
}

DirIterator::DirIterator(const std::string &path) : p(std::make_unique<Private>(path))
{
}

DirIterator::DirIterator(const DirIterator &it) : p(std::make_unique<Private>())
{
  p->it = it.p->it;
}

DirIterator::~DirIterator()
{
}

DirIterator &DirIterator::operator=(const DirIterator &it)
{
  if (&it!=this)
  {
    p->it = it.p->it;
  }
  return *this;
}

DirIterator DirIterator::operator++()
{
  DirIterator result;
  result.p->it = ++p->it;
  return result;
}

const DirIterator::value_type &DirIterator::operator*() const
{
  p->current.p->entry = *p->it;
  return p->current;
}

const DirIterator::value_type *DirIterator::operator->() const
{
  p->current.p->entry = *p->it;
  return &p->current;
}

bool operator==(const DirIterator &it1,const DirIterator &it2)
{
  return it1.p->it!=it2.p->it;
}

bool operator!=(const DirIterator &it1,const DirIterator &it2)
{
  return it1.p->it!=it2.p->it;
}

DirIterator begin(DirIterator it) noexcept
{
  return it;
}

DirIterator end(const DirIterator &) noexcept
{
  return DirIterator();
}


//-----------------------------------------------------------------------------------------------


struct Dir::Private
{
  fs::path path;
};

Dir::Dir() : p(std::make_unique<Private>())
{
  std::error_code ec;
  p->path = fs::current_path(ec);
}

Dir::Dir(const Dir &d) : p(std::make_unique<Private>())
{
  p->path = d.p->path;
}

Dir &Dir::operator=(const Dir &d)
{
  if (&d!=this)
  {
    p->path = d.p->path;
  }
  return *this;
}

Dir::Dir(const std::string &path) : p(std::make_unique<Private>())
{
  setPath(path);
}

Dir::~Dir()
{
}

void Dir::setPath(const std::string &path)
{
  p->path = path;
}

std::string Dir::path() const
{
  return p->path.string();
}

DirIterator Dir::iterator() const
{
  return DirIterator(p->path.string());
}

static void correctPath(std::string &s)
{
  std::replace( s.begin(), s.end(), '\\', '/' );
}

bool Dir::exists(const std::string &path,bool acceptsAbsPath) const
{
  std::string result = filePath(path,acceptsAbsPath);
  std::error_code ec;
  bool exist = fs::exists(fs::path(result),ec);
  return !ec && exist;
}

bool Dir::exists() const
{
  FileInfo fi(p->path);
  return fi.exists() && fi.isDir();
}

bool Dir::isRelative() const
{
  return isRelativePath(p->path.string());
}

bool Dir::isRelativePath(const std::string &path)
{
  return fs::path(path).is_relative();
}

std::string Dir::filePath(const std::string &path,bool acceptsAbsPath) const
{
  std::string result;
  if (acceptsAbsPath && !isRelativePath(path))
  {
    result = path;
  }
  else
  {
    result = (p->path / path).string();
  }
  correctPath(result);
  return result;
}

bool Dir::mkdir(const std::string &path,bool acceptsAbsPath) const
{
  std::error_code ec;
  std::string result = filePath(path,acceptsAbsPath);
  if (exists(path,acceptsAbsPath))
  {
    return true;
  }
  else
  {
    return fs::create_directory(result,ec);
  }
}

bool Dir::rmdir(const std::string &path,bool acceptsAbsPath) const
{
  return remove(path,acceptsAbsPath);
}

bool Dir::remove(const std::string &path,bool acceptsAbsPath) const
{
  std::error_code ec;
  std::string result = filePath(path,acceptsAbsPath);
  return fs::remove(result,ec);
}

bool Dir::rename(const std::string &orgName,const std::string &newName,bool acceptsAbsPath) const
{
  std::error_code ec;
  std::string fn1 = filePath(orgName,acceptsAbsPath);
  std::string fn2 = filePath(newName,acceptsAbsPath);
  fs::rename(fn1,fn2,ec);
  return !ec;
}

bool Dir::copy(const std::string &srcName,const std::string &dstName,bool acceptsAbsPath) const
{
  const auto copyOptions = fs::copy_options::overwrite_existing;
  std::error_code ec;
  std::string sn = filePath(srcName,acceptsAbsPath);
  std::string dn = filePath(dstName,acceptsAbsPath);
  fs::copy(sn,dn,copyOptions,ec);
  return !ec;
}

std::string Dir::currentDirPath()
{
  std::error_code ec;
  std::string result = fs::current_path(ec).string();
  correctPath(result);
  return result;
}

bool Dir::setCurrent(const std::string &path)
{
  std::error_code ec;
  fs::current_path(path,ec);
  return !ec;
}

std::string Dir::cleanDirPath(const std::string &path)
{
  std::error_code ec;
  std::string result = fs::path(path).lexically_normal().string();
  correctPath(result);
  return result;
}

std::string Dir::absPath() const
{
  std::error_code ec;
  std::string result = fs::absolute(p->path,ec).string();
  correctPath(result);
  return result;
}

