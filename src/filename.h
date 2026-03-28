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

#ifndef FILENAME_H
#define FILENAME_H

#include <memory>
#include <vector>

#include "linkedmap.h"
#include "utf8.h"
#include "util.h"

class FileDef;

/** Class representing all files with a certain base name */
class FileName : public std::vector< std::unique_ptr<FileDef> >
{
  public:
    FileName(const QCString &nm,const QCString &fn) : m_name(nm), m_fName(fn), m_pathName("tmp") {}
    QCString fileName() const { return m_name; }
    QCString fullName() const { return m_fName; }
    QCString path() const { return m_pathName; }

  private:
    QCString m_name;
    QCString m_fName;
    QCString m_pathName;
};

//! Custom combined key compare and hash functor that uses a lower case string in
//! case CASE_SENSE_NAMES is set to NO.
class FileNameFn
{
  public:
    //! used as hash function
    std::size_t operator()(const std::string& input) const noexcept
    {
      return std::hash<std::string>()(searchKey(input));
    }
    //! used as equal operator
    bool operator() (const std::string &t1, const std::string &t2) const
    {
      return searchKey(t1) == searchKey(t2);
    }
  private:
    std::string searchKey(const std::string &input) const
    {
      std::string key = input;
      if (!getCaseSenseNames())
      {
        key = convertUTF8ToLower(key);
      }
      return key;
    }
};

/** Ordered dictionary of FileName objects. */
class FileNameLinkedMap : public LinkedMap<FileName,FileNameFn,FileNameFn,
                                           std::unordered_multimap<std::string,FileName*,FileNameFn,FileNameFn> >
{
};

#endif
