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

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <algorithm>
#include <vector>

#include "qcstring.h"

class ClassDef;
class MemberName;

/** Data associated with an example. */
struct Example
{
  Example(const QCString &a,const QCString &n,const QCString &f) : anchor(a), name(n), file(f) {}
  QCString anchor;
  QCString name;
  QCString file;
};

class ExampleList : public std::vector<Example>
{
  public:
    bool inSort( const Example& ex )
    {
      auto it = std::find_if(begin(),end(),[&ex](const Example &e) { return e.name==ex.name; });
      if (it==end())
      {
        insert( std::upper_bound( begin(), end(), ex,
                 [](const auto &e1,const auto &e2) { return e1.name < e2.name; }
                ), ex
              );
        return true;
      }
      return false;
    }
};

#endif
