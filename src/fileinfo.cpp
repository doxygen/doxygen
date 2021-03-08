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
#include "fileinfo.h"

namespace fs = ghc::filesystem;

size_t FileInfo::size() const
{
  return static_cast<size_t>(fs::file_size(fs::path(m_name)));
}

bool FileInfo::exists() const
{
  return fs::exists(fs::status(m_name));
}

bool FileInfo::isWritable() const
{
  return (fs::status(m_name).permissions() & fs::perms::owner_write)!=fs::perms::none;
}

bool FileInfo::isReadable() const
{
  return (fs::status(m_name).permissions() & fs::perms::owner_read)!=fs::perms::none;
}

bool FileInfo::isExecutable() const
{
  return (fs::status(m_name).permissions() & fs::perms::owner_exec)!=fs::perms::none;
}

bool FileInfo::isRelative() const
{
  return fs::path(m_name).is_relative();
}

bool FileInfo::isFile() const
{
  return fs::is_regular_file(fs::status(m_name));
}

bool FileInfo::isDir() const
{
  return fs::is_directory(fs::status(m_name));
}

bool FileInfo::isSymLink() const
{
  return fs::is_symlink(fs::status(m_name));
}

std::string FileInfo::readLink() const
{
  return fs::read_symlink(fs::path(m_name)).string();
}

std::string FileInfo::filePath() const
{
  return m_name;
}

void FileInfo::correctPath(std::string &s)
{
  std::replace( s.begin(), s.end(), '\\', '/' );
}

std::string FileInfo::absFilePath() const
{
  std::string result;
  if (fs::exists(fs::status(m_name)))
  {
    result = fs::canonical(m_name).string();
  }
  else
  {
    result = (fs::current_path() / m_name).string();
  }
  correctPath(result);
  return result;
}

std::string FileInfo::fileName() const
{
  return fs::path(m_name).filename().string();
}

std::string FileInfo::baseName() const
{
  std::string s = fileName();
  size_t pos = s.find('.');
  return pos!=std::string::npos ? s.substr(0,pos) : s;
}

std::string FileInfo::extension(bool complete) const
{
  std::string fn = fileName();
  size_t pos = complete ? fn.find('.') : fn.rfind('.');
  return pos!=std::string::npos ? fn.substr(pos+1) : std::string();
}

std::string FileInfo::dirPath(bool absPath) const
{
  std::string result;
  if (absPath)
  {
    result = fs::canonical(m_name).parent_path().string();
  }
  else
  {
    result = fs::relative(m_name).parent_path().string();
    if (result.empty()) result=".";
  }
  correctPath(result);
  return result;
}

