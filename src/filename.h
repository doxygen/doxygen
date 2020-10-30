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

class FileDef;

/** Class representing all files with a certain base name */
class FileName : public std::vector< std::unique_ptr<FileDef> >
{
  public:
    FileName(const char *nm,const char *fn) : m_name(nm), m_fName(fn), m_pathName("tmp") {}
    const char *fileName() const { return m_name; }
    const char *fullName() const { return m_fName; }
    const char *path() const { return m_pathName; }

  private:
    QCString m_name;
    QCString m_fName;
    QCString m_pathName;
};

/** Ordered dictionary of FileName objects. */
class FileNameLinkedMap : public LinkedMap<FileName,true>
{
};

#endif
