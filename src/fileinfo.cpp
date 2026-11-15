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
#define WIN32_LEAN_AND_MEAN // optional

// own header
#include "fileinfo.h"

// standard includes
#include <algorithm>

// other includes
#include "fswrapper.h"

size_t FileInfo::size() const
{
  std::error_code ec;
  size_t result = static_cast<size_t>(fs::file_size(toFsPath(m_name),ec));
  return ec ? 0 : result;
}

bool FileInfo::exists() const
{
  std::error_code ec;
  bool result = fs::exists(toFsPath(m_name),ec);
  return !ec && result;
}

bool FileInfo::isWritable() const
{
  std::error_code ec;
  fs::file_status status = fs::status(toFsPath(m_name),ec);
  return !ec && (status.permissions() & fs::perms::owner_write)!=fs::perms::none;
}

bool FileInfo::isReadable() const
{
  std::error_code ec;
  fs::file_status status = fs::status(toFsPath(m_name),ec);
  return !ec && (status.permissions() & fs::perms::owner_read)!=fs::perms::none;
}

bool FileInfo::isExecutable() const
{
  std::error_code ec;
  fs::file_status status = fs::status(toFsPath(m_name),ec);
  return !ec && (status.permissions() & fs::perms::owner_exec)!=fs::perms::none;
}

bool FileInfo::isRelative() const
{
  return toFsPath(m_name).is_relative();
}

bool FileInfo::isFile() const
{
  std::error_code ec;
  fs::file_status status = fs::status(toFsPath(m_name),ec);
  return !ec && fs::is_regular_file(std::move(status));
}

bool FileInfo::isDir() const
{
  std::error_code ec;
  fs::file_status status = fs::status(toFsPath(m_name),ec);
  return !ec && fs::is_directory(std::move(status));
}

bool FileInfo::isSymLink() const
{
  std::error_code ec;
  fs::file_status status = fs::symlink_status(toFsPath(m_name),ec);
  return !ec && fs::is_symlink(std::move(status));
}

std::string FileInfo::readLink() const
{
  std::error_code ec;
  fs::path targetPath =  fs::read_symlink(toFsPath(m_name));
  return !ec ? fromFsPath(targetPath) : std::string();
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
  std::error_code ec;
  fs::path path = toFsPath(m_name);
  if (!path.is_relative())
  {
    result = fromFsPath(path.lexically_normal());
  }
  else
  {
    result = fromFsPath((fs::current_path(ec) / path).lexically_normal());
  }
  correctPath(result);
  return result;
}

std::string FileInfo::fileName() const
{
  return fromFsPath(toFsPath(m_name).filename());
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
    result = absFilePath();
  }
  else
  {
    result = m_name;
    correctPath(result);
  }
  size_t pos = result.rfind('/');
  if (pos==std::string::npos)
  {
    return ".";
  }
  else if (pos==0)
  {
    return "/";
  }
  else
  {
    return result.substr(0,pos);
  }
}

