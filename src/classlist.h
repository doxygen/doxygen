/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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

#ifndef CLASSLIST_H
#define CLASSLIST_H

#include <qlist.h>
#include <qdict.h>

#include "classdef.h"
#include "sortdict.h"

class ClassList : public QList<ClassDef>
{ 
  public:
    ClassList();
   ~ClassList();
   
   int compareItems(GCI item1,GCI item2);
};

class ClassListIterator : public QListIterator<ClassDef>
{
  public:
    ClassListIterator(const ClassList &list);
};

class ClassDict : public QDict<ClassDef>
{
  public:
    ClassDict(int size) : QDict<ClassDef>(size) {}
   ~ClassDict() {}
};

class ClassSDict : public SDict<ClassDef>
{
  public:
    ClassSDict(int size) : SDict<ClassDef>(size) {}
   ~ClassSDict() {}
   int compareItems(GCI item1,GCI item2);
   void writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter=0,
                         const char *header=0);
};

#endif
