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

#include "entry.h"

int Entry::num=0;

Entry::Entry()
{
  num++;
  //printf("New Entry %d\n",num);
  parent=0;
  sublist = new QList<Entry>;
  sublist->setAutoDelete(TRUE);
  extends = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups = new QList<QCString>;
  groups->setAutoDelete(TRUE);
  anchors = new QList<QCString>;
  anchors->setAutoDelete(TRUE);
  argList = new ArgumentList;
  argList->setAutoDelete(TRUE);
  //printf("Entry::Entry() tArgList=0\n");
  tArgList = 0;
  mtArgList = 0;
  mGrpId = -1;
  reset();
}

Entry::Entry(const Entry &e)
{
  num++;
  //printf("Copy New Entry %d\n",num);
  section     = e.section;
  protection  = e.protection;
  sig         = e.sig;
  slot        = e.slot;
  stat        = e.stat;
  virt        = e.virt;
  parent      = e.parent;
  type        = e.type.copy();
  name        = e.name.copy();
  args        = e.args.copy();
  exception   = e.exception.copy();
  program     = e.program.copy();
  includeFile = e.includeFile.copy();
  includeName = e.includeFile.copy();
  doc         = e.doc.copy();
  relates     = e.relates.copy();
  brief       = e.brief.copy();
  inside      = e.inside.copy();
  fileName    = e.fileName.copy();
  startLine   = e.startLine;
  mGrpId      = e.mGrpId;
  bodyLine    = e.bodyLine;
  endBodyLine = e.endBodyLine;
  inLine      = e.inLine;
  sublist     = new QList<Entry>;
  sublist->setAutoDelete(TRUE);
  extends     = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups      = new QList<QCString>;
  groups->setAutoDelete(TRUE);
  anchors     = new QList<QCString>;
  anchors->setAutoDelete(TRUE);
  argList     = new ArgumentList;
  argList->setAutoDelete(TRUE);
  //printf("Entry::Entry(copy) tArgList=0\n");
  tArgList = 0;
  mtArgList = 0;

  // deep copy of the child entry list
  QListIterator<Entry> eli(*e.sublist);
  Entry *cur;
  for (;(cur=eli.current());++eli)
  {
    sublist->append(new Entry(*cur));
  }
  
  // deep copy base class list
  QListIterator<BaseInfo> bli(*e.extends);
  BaseInfo *bi;
  for (;(bi=bli.current());++bli)
  {
    extends->append(new BaseInfo(*bi));
  }
  
  // deep copy group list
  QListIterator<QCString> sli(*e.groups);
  QCString *s;
  for (;(s=sli.current());++sli)
  {
    groups->append(new QCString(*s));
  }
  
  QListIterator<QCString> sli2(*e.anchors);
  for (;(s=sli2.current());++sli2)
  {
    anchors->append(new QCString(*s));
  }

  // deep copy argument list
  QListIterator<Argument> ali(*e.argList);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    argList->append(new Argument(*a));
  }
  argList->constSpecifier    = e.argList->constSpecifier;
  argList->volatileSpecifier = e.argList->volatileSpecifier;
  argList->pureSpecifier     = e.argList->pureSpecifier;
  
  // deep copy template argument list
  if (e.tArgList)
  {
    tArgList = new ArgumentList;
    tArgList->setAutoDelete(TRUE);
    //printf("Entry::Entry(copy) new tArgList=%p\n",tArgList);
    QListIterator<Argument> tali(*e.tArgList);
    for (;(a=tali.current());++tali)
    {
      tArgList->append(new Argument(*a));
      //printf("appending argument %s %s\n",a->type.data(),a->name.data());
    }
  }

  // deep copy template argument list
  if (e.mtArgList)
  {
    mtArgList = new ArgumentList;
    mtArgList->setAutoDelete(TRUE);
    //printf("Entry::Entry(copy) new tArgList=%p\n",tArgList);
    QListIterator<Argument> mtali(*e.mtArgList);
    for (;(a=mtali.current());++mtali)
    {
      mtArgList->append(new Argument(*a));
      //printf("appending argument %s %s\n",a->type.data(),a->name.data());
    }
  }
}

Entry::~Entry()
{
  //printf("Deleting entry %d name %s type %x chilren %d\n",
  //       num,name.data(),section,sublist->count());
  delete sublist;
  delete extends;
  delete groups;
  delete anchors;
  delete argList;
  delete tArgList;
  delete mtArgList;
  num--;
}

void Entry::addSubEntry(Entry *current)
{
  //printf("Entry %d with name %s type 0x%x added\n",
  //    current->num,current->name.data(),current->section);
  //printf("Entry::addSubEntry(%s) %p\n",current->name.data(),current->tArgList);
  current->parent=this;
  sublist->append(current);  
  
  //if (current->tArgList)
  //{
  //  Argument * a=current->tArgList->first();
  //  while (a)
  //  {
  //    printf("type=%s name=%s\n",a->type.data(),a->name.data());
  //    a=current->tArgList->next();
  //  }
  //}

}

void Entry::reset()
{
  name.resize(0);
  type.resize(0);
  args.resize(0);
  exception.resize(0);
  program.resize(0);
  //body.resize(0);
  includeFile.resize(0);
  includeName.resize(0);
  doc.resize(0);
  relates.resize(0);
  brief.resize(0);
  inside.resize(0);
  fileName.resize(0);
  scopeSpec.resize(0);
  memberSpec.resize(0);
  bodyLine = -1;
  endBodyLine = -1;
  mGrpId = -1;
  section = EMPTY_SEC;
  sig     = FALSE;
  virt    = Normal;
  slot    = FALSE;
  stat    = FALSE;
  proto   = FALSE;
  inLine  = FALSE;
  protection = Public;
  sublist->clear();
  extends->clear();
  groups->clear();
  anchors->clear();
  argList->clear();
  if (tArgList) { delete tArgList; tArgList=0; }
  if (mtArgList) { delete mtArgList; mtArgList=0; }
}


int Entry::getSize()
{
  int size=sizeof(Entry);
  size+=type.length()+1;
  size+=name.length()+1;
  size+=args.length()+1;
  size+=exception.length()+1;
  size+=program.length()+1;
  //size+=body.length()+1;
  size+=includeFile.length()+1;
  size+=includeName.length()+1;
  size+=doc.length()+1;
  size+=relates.length()+1;
  size+=brief.length()+1;
  size+=inside.length()+1;
  size+=fileName.length()+1;
  BaseInfo *bi=extends->first();
  while (bi)
  {
    size+=sizeof(QLNode);
    size+=bi->name.length()+1+sizeof(bi->prot)+sizeof(bi->virt);
    bi=extends->next(); 
  }
  QCString *s=groups->first();
  while (s)
  {
    size+=sizeof(QLNode);
    size+=s->length()+1;
    s=groups->next();
  }
  s=anchors->first();
  while (s)
  {
    size+=sizeof(QLNode);
    size+=s->length()+1;
    s=anchors->next();
  }
  Entry *e=sublist->first();
  while (e)
  {
    size+=e->getSize();
    e=sublist->next();
  }
  Argument *a=argList->first();
  while (e)
  {
    size+=sizeof(Argument);
    size+=a->type.length()+1
         +a->name.length()+1
         +a->defval.length()+1;
    a=argList->next();
  }
  if (tArgList)
  {
    a=tArgList->first();
    while (e)
    {
      size+=sizeof(Argument);
      size+=a->type.length()+1
        +a->name.length()+1
        +a->defval.length()+1;
      a=tArgList->next();
    }
  }
  if (mtArgList)
  {
    a=mtArgList->first();
    while (e)
    {
      size+=sizeof(Argument);
      size+=a->type.length()+1
        +a->name.length()+1
        +a->defval.length()+1;
      a=mtArgList->next();
    }
  }
  return size;
}
