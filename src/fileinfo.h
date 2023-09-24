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

#ifndef FILEINFO_H
#define FILEINFO_H

#include <string>

/** @brief Minimal replacement for QFileInfo. */
class FileInfo
{
  public:
    explicit FileInfo(const std::string &name) : m_name(name) {}
    bool exists() const;
    size_t size() const;
    bool isWritable() const;
    bool isReadable() const;
    bool isExecutable() const;
    bool isRelative() const;
    bool isFile() const;
    bool isDir() const;
    bool isSymLink() const;
    std::string readLink() const;
    std::string filePath() const;
    std::string absFilePath() const;
    std::string fileName() const;
    std::string baseName() const;
    std::string extension(bool complete) const;
    std::string dirPath(bool absPath = true) const;
  private:
    static void correctPath(std::string &s);
    std::string m_name;
};

#endif
