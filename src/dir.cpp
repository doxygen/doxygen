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

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "filesystem.hpp"
#include "dir.h"

#include <utility>

namespace fs = ghc::filesystem;


//-----------------------------------------------------------------------------------------------

struct DirEntry::Private
{
  fs::directory_entry entry;
};

DirEntry::DirEntry() : p(std::make_unique<Private>())
{
}

DirEntry::DirEntry(const DirEntry &de) : p(std::make_unique<Private>())
{
  p->entry = de.p->entry;
}

DirEntry &DirEntry::operator=(const DirEntry &de)
{
  if (this!=&de)
  {
    p = std::make_unique<Private>();
    p->entry = de.p->entry;
  }
  return *this;
}

DirEntry::DirEntry(DirEntry &&de) : p(std::make_unique<Private>())
{
  p = std::move(de.p);
}

DirEntry &DirEntry::operator=(DirEntry &&de)
{
  p = std::move(de.p);
  return *this;
}

DirEntry::~DirEntry() = default;

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
  Private(const std::string &path) : it(path,ec) {}
  fs::directory_iterator it;
  std::error_code ec;
  DirEntry current;
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
  p->ec = it.p->ec;
  p->current = it.p->current;
}

DirIterator &DirIterator::operator=(const DirIterator &it)
{
  if (&it!=this)
  {
    p->it = it.p->it;
    p->ec = it.p->ec;
    p->current = it.p->current;
  }
  return *this;
}

DirIterator::DirIterator(DirIterator &&it) : p(std::make_unique<Private>())
{
  std::exchange(p->it,it.p->it);
  std::exchange(p->ec,it.p->ec);
  std::exchange(p->current,it.p->current);
}

DirIterator &DirIterator::operator=(DirIterator &&it)
{
  std::exchange(p->it,it.p->it);
  std::exchange(p->ec,it.p->ec);
  std::exchange(p->current,it.p->current);
  return *this;
}

DirIterator::~DirIterator()
{
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
  return it1.p->it == it2.p->it;
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

Dir::Dir(Dir &&d) : p(std::make_unique<Private>())
{
  std::exchange(p->path,d.p->path);
}

Dir &Dir::operator=(Dir &&d)
{
  std::exchange(p->path,d.p->path);
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
  FileInfo fi(p->path.string());
  return fi.exists() && fi.isDir();
}

bool Dir::isEmpty(const std::string &subdir) const
{
  fs::path pth = path();
  pth /= subdir;
  return fs::is_empty(pth);
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
  std::error_code ec, ec_perm;
  std::string sn = filePath(srcName,acceptsAbsPath);
  std::string dn = filePath(dstName,acceptsAbsPath);
  fs::copy(sn,dn,copyOptions,ec);
  // make sure the destination is writable for the owner (see issue #11600)
  fs::permissions(dn, fs::perms::owner_write, fs::perm_options::add, ec_perm);
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

