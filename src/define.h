/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef DEFINE_H
#define DEFINE_H

#include <qdict.h>
#include <qlist.h>

class FileDef;

/** A class representing a macro definition. */
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
    int columnNr;
    int nargs;
    bool undef;
    bool varArgs;
    bool isPredefined;
    bool nonRecursive;
};

/** A list of Define objects. */
class DefineList : public QList<Define>
{
  public:
    DefineList() : QList<Define>() {}
   ~DefineList() {}
    int compareItems(QCollection::Item i1,QCollection::Item i2) 
    {
      return qstricmp(((Define *)i1)->name,((Define *)i2)->name); 
    }
};

/** A list of Define objects associated with a specific name. */
class DefineName : public QList<Define>
{
  public:
    DefineName(const char *n) : QList<Define>() { name=n; }
   ~DefineName() {}
    const char *nameString() const { return name; }
    int compareItems(QCollection::Item i1,QCollection::Item i2) 
    {
      return qstricmp(((Define *)i1)->name,((Define *)i2)->name); 
    }
    
  private:
    QCString name;
};

/** A list of DefineName objects. */
class DefineNameList : public QList<DefineName>
{
  public:
    DefineNameList() : QList<DefineName>() {}
   ~DefineNameList() {}
    int compareItems(QCollection::Item i1,QCollection::Item i2)
    {
      return qstricmp(((DefineName *)i1)->nameString(),
                    ((DefineName *)i2)->nameString());
    }
};

/** An unsorted dictionary of Define objects. */
typedef QDict<Define> DefineDict;

/** A sorted dictionary of DefineName object. */
typedef QDict<DefineName> DefineNameDict;

#endif
