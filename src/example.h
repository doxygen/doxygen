/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "qtbc.h"
#include <qdict.h>

class ClassDef;
class MemberName;

struct Example
{
  QCString anchor;
  QCString name;
  QCString file;
};

class ExampleList : public QList<Example>
{
  public:
    ExampleList() : QList<Example>() {}
   ~ExampleList() {}
    int compareItems(GCI item1,GCI item2)
    {
      return stricmp(((Example *)item1)->name,((Example *)item2)->name);
    }
};

class ExampleDict : public QDict<Example>
{
  public:
    ExampleDict(int size=17) : QDict<Example>(size) {}
   ~ExampleDict() {}
};

#endif
