/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include "sortdict.h"

class PageInfo
{
  public:
    PageInfo(const char *f, int l,const char *n,const char *d,const char *t) :
      defFileName(f), defLine(l), name(n), 
      doc(d), title(t), todoId(0), testId(0) {}

    // where the page definition was found
    QCString defFileName;
    int defLine;

    // contents of the page
    QCString name;
    QCString doc;
    QCString title;

    // ids
    int todoId;
    int testId;
};

class PageSDict : public SDict<PageInfo>
{
  public:
    PageSDict(int size) : SDict<PageInfo>(size) {}
   ~PageSDict() {}
    int compareItems(GCI i1,GCI i2)
    {
      return stricmp(((PageInfo *)i1)->name,((PageInfo *)i2)->name);
    }
};

// typedef is to get around Solaris compile problem
typedef SDict<PageInfo>::Iterator _SDictPageInfoIterator;

class PageSDictIterator : public _SDictPageInfoIterator
{
  public:
    PageSDictIterator(const PageSDict &d) : _SDictPageInfoIterator(d) {}
   ~PageSDictIterator() {} 
};

