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

#include "qtbc.h"
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
    NamespaceDef(const char *name,const char *ref=0);
   ~NamespaceDef();
    //QCString namespaceFile() const { return fileName; }
    QCString getOutputFileBase() const { return fileName; }
    void insertUsedFile(const char *fname);
    void writeDocumentation(OutputList &ol);
    void insertClass(ClassDef *cd);
    void insertMember(MemberDef *md);
    void computeAnchors();
    int countMembers();
    //const char *getReference() { return reference; }
    //bool isVisible() 
    //{
    //  return !getReference() && hasDocumentation() &&
    //         !name().isEmpty() && name().at(0)!='@';
    //}
    //bool isVisibleExt()
    //{
    //  return (getReference() || hasDocumentation()) &&
    //         !name().isEmpty() && name().at(0)!='@';
    //}

    bool isLinkableInProject()
    {
      int i = name().findRev("::");
      if (i==-1) i=0; else i+=2;
      return !name().isEmpty() && name().at(i)!='@' &&
              hasDocumentation() && !isReference();
    }
    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }
    
  private:
    //QCString reference;
    QCString fileName;
    QStrList files;
    ClassList *classList;
    MemberList *memList;
};

class NamespaceList : public QList<NamespaceDef>
{ 
  public:
   ~NamespaceList() {}
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

class NamespaceDict : public QDict<NamespaceDef>
{
  public:
    NamespaceDict(int size) : QDict<NamespaceDef>(size) {}
   ~NamespaceDict() {}
};

#endif
