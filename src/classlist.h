/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef CLASSLIST_H
#define CLASSLIST_H

#include "classdef.h"

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

#endif
