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
  private:
    int compareValues(const Define *d1,const Define *d2) const
    {
      return qstricmp(d1->name,d2->name);
    }
};

/** A list of Define objects associated with a specific name. */
class DefineName : public QList<Define>
{
  public:
    DefineName(const char *n) : QList<Define>() { name=n; }
   ~DefineName() {}
    const char *nameString() const { return name; }

  private:
    int compareValues(const Define *d1,const Define *d2) const
    {
      return qstricmp(d1->name,d2->name);
    }
    QCString name;
};

/** A list of DefineName objects. */
class DefineNameList : public QList<DefineName>
{
  public:
    DefineNameList() : QList<DefineName>() {}
   ~DefineNameList() {}
  private:
    int compareValues(const DefineName *n1,const DefineName *n2) const
    {
      return qstricmp(n1->nameString(),n2->nameString());
    }
};

/** An unsorted dictionary of Define objects. */
typedef QDict<Define> DefineDict;

/** A sorted dictionary of DefineName object. */
typedef QDict<DefineName> DefineNameDict;

#endif
