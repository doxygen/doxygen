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
class FileName final : public std::vector< std::unique_ptr<FileDef> >
{
  public:
    explicit FileName(const DString &nm) : m_name(nm) {}
    DString fileName() const { return m_name; }

  private:
    DString m_name;
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
      if (!useCaseSenseNames())
      {
        key = convertUTF8ToLower(key);
      }
      return key;
    }
};

/** Ordered dictionary of FileName objects. */
class FileNameLinkedMap final : public LinkedMap<FileName,FileNameFn,FileNameFn,
                                           std::unordered_multimap<std::string,FileName*,FileNameFn,FileNameFn> >
{
  public:
    /** Returns the file definition in \a fnMap that matches the file name \a n.
     *  If there are multiple matches, ambig is set to true and the first match is returned.
     *  If there are no matches, ambig is set to false and nullptr is returned.
     */
    FileDef *findFileDef(const DString &n,bool &ambig) const;

    /** Returns a list of file definitions in \a fnMap that match the file name \a n.
     *  The list is returned as a string with each file definition separated by a newline character.
     *  Used for error messages.
     */
    DString showFileDefMatches(const DString &n) const;
};

#endif
