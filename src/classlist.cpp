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

#include "classlist.h"
#include "config.h"

ClassList::ClassList() : QList<ClassDef>()
{
}

ClassList::~ClassList()
{
}

int ClassList::compareItems(GCI item1, GCI item2)
{
  ClassDef *c1=(ClassDef *)item1;
  ClassDef *c2=(ClassDef *)item2;
 
  int prefixLength = Config::ignorePrefix.length();
  int i1 = c1->name().left(prefixLength)==Config::ignorePrefix ? prefixLength : 0;
  int i2 = c2->name().left(prefixLength)==Config::ignorePrefix ? prefixLength : 0;
  return strcmp(c1->name().data()+i1,c2->name().data()+i2);
}

ClassListIterator::ClassListIterator(const ClassList &cllist) :
  QListIterator<ClassDef>(cllist)
{
}
