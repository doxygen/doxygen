/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <qcstring.h>
#include "sortdict.h"

class ClassDef;
class MemberName;

/** Data associated with an example. */
struct Example
{
  QCString anchor;
  QCString name;
  QCString file;
};

/** A sorted dictionary of Example objects. */
class ExampleSDict : public SDict<Example>
{
  public:
    ExampleSDict(int size=17) : SDict<Example>(size) {}
   ~ExampleSDict() {}
  private:
    int compareValues(const Example *item1,const Example *item2) const
    {
      return qstricmp(item1->name,item2->name);
    }
};

#endif
