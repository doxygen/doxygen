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

#ifndef DEFINE_H
#define DEFINE_H

#include "qtbc.h"
#include <qdict.h>
#include "util.h"

class FileDef;

class Define
{
  public:
    Define();
    Define(const Define &d);
   ~Define();
    bool hasDocumentation();
    QCString name;
    QCString definition;
    QCString fileName;
    QCString doc;
    QCString brief;
    QCString args;
    QCString anchor;
    FileDef *fileDef;
    int lineNr;
    int nargs;
    bool undef;
    bool varArgs;
    bool isPredefined;
    bool nonRecursive;
};

class DefineList : public QList<Define>
{
  public:
    DefineList() : QList<Define>() {}
   ~DefineList() {}
    int compareItems(GCI i1,GCI i2) 
    {
      return strcmp(((Define *)i1)->name,((Define *)i2)->name); 
    }
};

class DefineName : public QList<Define>
{
  public:
    DefineName(const char *n) : QList<Define>() { name=n; }
   ~DefineName() {}
    const char *nameString() const { return name; }
    int compareItems(GCI i1,GCI i2) 
    {
      return strcmp(((Define *)i1)->name,((Define *)i2)->name); 
    }
    
  private:
    QCString name;
};

class DefineNameList : public QList<DefineName>
{
  public:
    DefineNameList() : QList<DefineName>() {}
   ~DefineNameList() {}
    int compareItems(GCI i1,GCI i2)
    {
      return stricmp(((DefineName *)i1)->nameString(),
                    ((DefineName *)i2)->nameString());
    }
};

typedef QDict<Define> DefineDict;
typedef QDict<DefineName> DefineNameDict;

#endif
