/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#ifndef PACKAGEDEF_H
#define PACKAGEDEF_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>
#include "definition.h"
#include "sortdict.h"

class ClassSDict;
class ClassDef;
class PackageList;
class OutputList;

class PackageDef : public Definition
{
  public:
    PackageDef(const char *fName,int line, const char *name,const char *ref=0);
   ~PackageDef();
    DefType definitionType() { return TypePackage; }
    QCString getOutputFileBase() const ;
    void addClass(const ClassDef *def);
    void writeDocumentation(OutputList &ol);
    bool isLinkableInProject()
    {
      return hasDocumentation() && !isReference();
    }
    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }

  private: 
    QCString packageFileName;           // base name of the generated file
    ClassSDict *classSDict;             // list of classes in the package
};

class PackageSDict : public SDict<PackageDef>
{
  public:
    PackageSDict(int size) : SDict<PackageDef>(size) {}
};

#endif

