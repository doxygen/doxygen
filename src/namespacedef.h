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

#ifndef NAMESPACEDEF_H
#define NAMESPACEDEF_H

#include <qstring.h>
#include <qstrlist.h>
#include <qdict.h>
#include "definition.h"

class ClassDef;
class OutputList;
class ClassList;
class MemberList;
class MemberDef;

class NamespaceDef : public Definition
{
  public:
    NamespaceDef(const char *name);
   ~NamespaceDef();
    QString namespaceFile() const { return fileName; }
    void insertUsedFile(const char *fname);
    void writeDocumentation(OutputList &ol);
    void insertClass(ClassDef *cd);
    void insertMember(MemberDef *md);
    void computeAnchors();
    
  private:
    QString fileName;
    QStrList files;
    ClassList *classList;
    MemberList *memList;
};

class NamespaceList : public QList<NamespaceDef>
{ 
  public:
    int compareItems(GCI item1,GCI item2)
    {
      return strcmp(((NamespaceDef *)item1)->name(),
                    ((NamespaceDef *)item2)->name()
                   );
    }
};

class NamespaceListIterator : public QListIterator<NamespaceDef>
{
  public:
    NamespaceListIterator(const NamespaceList &l) : 
      QListIterator<NamespaceDef>(l) {}
};

typedef QDict<NamespaceDef> NamespaceDict;

#endif
