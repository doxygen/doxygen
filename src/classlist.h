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

#ifndef CLASSLIST_H
#define CLASSLIST_H

#include <qlist.h>
#include <qdict.h>

#include "classdef.h"
#include "sortdict.h"

class Definition;

/** A list of ClassDef objects. */
class ClassList : public QList<ClassDef>
{ 
  public:
    ClassList();
   ~ClassList();
   
   int compareItems(QCollection::Item item1,QCollection::Item item2);
};

/** An iterator for ClassDef objects in a ClassList. */
class ClassListIterator : public QListIterator<ClassDef>
{
  public:
    ClassListIterator(const ClassList &list);
};

/** An unsorted dictionary of ClassDef objects. */
class ClassDict : public QDict<ClassDef>
{
  public:
    ClassDict(int size) : QDict<ClassDef>(size) {}
   ~ClassDict() {}
};

/** A sorted dictionary of ClassDef objects. */
class ClassSDict : public SDict<ClassDef>
{
  public:
    ClassSDict(int size=17) : SDict<ClassDef>(size) {}
   ~ClassSDict() {}
   int compareItems(QCollection::Item item1,QCollection::Item item2);
   void writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter=0,
                         const char *header=0,bool localNames=FALSE);
   void writeDocumentation(OutputList &ol,Definition *container=0);
   bool declVisible(const ClassDef::CompoundType *filter=0) const;
};

class GenericsCollection : public QIntDict<ClassDef>
{
  public:
    GenericsCollection() : QIntDict<ClassDef>(17) {}
   ~GenericsCollection() {}
};

class GenericsSDict 
{
  public:
   GenericsSDict() : m_dict(17) { m_dict.setAutoDelete(TRUE); }
  ~GenericsSDict() {}
   void insert(const QCString &key,ClassDef *cd);
   ClassDef *find(const QCString &key);
  private:
   SDict<GenericsCollection> m_dict;
};

#endif
