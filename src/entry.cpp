/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#include "entry.h"
#include "util.h"
#include "section.h"

int Entry::num=0;

Entry::Entry()
{
  num++;
  //printf("New Entry %d\n",num);
  parent=0;
  section = EMPTY_SEC;
  sublist = new QList<Entry>;
  sublist->setAutoDelete(TRUE);
  extends = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors = new QList<SectionInfo>;
  argList = new ArgumentList;
  argList->setAutoDelete(TRUE);
  //printf("Entry::Entry() tArgList=0\n");
  tArgLists = 0;
  //mtArgList = 0;
  mGrpId = -1;
  tagInfo = 0;
  sli = 0;
  relatesDup = FALSE;
  groupDocType = GROUPDOC_NORMAL;
  reset();
}

Entry::Entry(const Entry &e)
{
  num++;
  //printf("Copy New Entry %d\n",num);
  section     = e.section;
  protection  = e.protection;
  mtype       = e.mtype;
  stat        = e.stat;
  explicitExternal = e.explicitExternal;
  virt        = e.virt;
  parent      = e.parent;
  type        = e.type.copy();
  name        = e.name.copy();
  args        = e.args.copy();
  bitfields   = e.bitfields.copy();
  exception   = e.exception.copy();
  program     = e.program.copy();
  includeFile = e.includeFile.copy();
  includeName = e.includeFile.copy();
  doc         = e.doc.copy();
  docLine     = e.docLine;
  docFile     = e.docFile.copy();
  relates     = e.relates.copy();
  relatesDup  = e.relatesDup;
  brief       = e.brief.copy();
  briefLine   = e.briefLine;
  briefFile   = e.briefFile.copy();
  inbodyDocs  = e.inbodyDocs.copy();
  inbodyLine  = e.inbodyLine;
  inbodyFile  = e.inbodyFile.copy();
  inside      = e.inside.copy();
  fileName    = e.fileName.copy();
  startLine   = e.startLine;
  mGrpId      = e.mGrpId;
  bodyLine    = e.bodyLine;
  endBodyLine = e.endBodyLine;
  memSpec     = e.memSpec;
  initializer = e.initializer;
  initLines   = e.initLines;
  callGraph   = e.callGraph;
  objc        = e.objc;
  //todoId      = e.todoId;
  //testId      = e.testId;
  //bugId       = e.bugId;
  tagInfo     = e.tagInfo;
  sublist     = new QList<Entry>;
  sublist->setAutoDelete(TRUE);
  extends     = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups      = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors     = new QList<SectionInfo>;
  argList     = new ArgumentList;
  argList->setAutoDelete(TRUE);
  tArgLists = 0;
  groupDocType = e.groupDocType;

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
  QListIterator<Grouping> gli(*e.groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    groups->append(new Grouping(*g));
  }
  
  QListIterator<SectionInfo> sli2(*e.anchors);
  SectionInfo *s;
  for (;(s=sli2.current());++sli2)
  {
    anchors->append(new SectionInfo(*s));
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
  
  // deep copy template argument lists
  if (e.tArgLists)
  {
    tArgLists = copyArgumentLists(e.tArgLists);
  }

  // deep copy template argument list
  //if (e.mtArgList)
  //{
  //  mtArgList = new ArgumentList;
  //  mtArgList->setAutoDelete(TRUE);
  //  //printf("Entry::Entry(copy) new tArgList=%p\n",tArgList);
  //  QListIterator<Argument> mtali(*e.mtArgList);
  //  for (;(a=mtali.current());++mtali)
  //  {
  //    mtArgList->append(new Argument(*a));
  //    //printf("appending argument %s %s\n",a->type.data(),a->name.data());
  //  }
  //}
  if (e.sli)
  {
    sli = new QList<ListItemInfo>;
    sli->setAutoDelete(TRUE);
    QListIterator<ListItemInfo> slii(*e.sli);
    ListItemInfo *ili;
    for (slii.toFirst();(ili=slii.current());++slii)
    {
      sli->append(new ListItemInfo(*ili));
    }
  }
  else
  {
    sli=0;
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
  delete tArgLists;
  //delete mtArgList;
  delete tagInfo;
  delete sli;
  num--;
}

void Entry::addSubEntry(Entry *current)
{
  //printf("Entry %d with name %s type 0x%x added to %s type 0x%x\n",
  //    current->num,current->name.data(),current->section,
  //    name.data(),section);
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
  bitfields.resize(0);
  exception.resize(0);
  program.resize(0);
  //body.resize(0);
  includeFile.resize(0);
  includeName.resize(0);
  doc.resize(0);
  docFile.resize(0);
  docLine=-1;
  relates.resize(0);
  relatesDup=FALSE;
  brief.resize(0);
  briefFile.resize(0);
  briefLine=-1;
  inbodyDocs.resize(0);
  inbodyFile.resize(0);
  inbodyLine=-1;
  inside.resize(0);
  fileName.resize(0);
  //scopeSpec.resize(0);
  //memberSpec.resize(0);
  initializer.resize(0);
  initLines = -1;
  startLine = 1;
  bodyLine = -1;
  endBodyLine = -1;
  mGrpId = -1;
  callGraph = FALSE;
  //todoId = 0;
  //testId = 0;
  //bugId = 0;
  section = EMPTY_SEC;
  mtype   = Method;
  virt    = Normal;
  stat    = FALSE;
  proto   = FALSE;
  explicitExternal = FALSE;
  memSpec  = 0;
  objc = FALSE;
  subGrouping = TRUE;
  protection = Public;
  groupDocType = GROUPDOC_NORMAL;
  sublist->clear();
  extends->clear();
  groups->clear();
  anchors->clear();
  argList->clear();
  if (tagInfo) { delete tagInfo; tagInfo=0; }
  if (tArgLists) { delete tArgLists; tArgLists=0; }
  if (sli) { delete sli; sli=0; }
  //if (mtArgList) { delete mtArgList; mtArgList=0; }
}


int Entry::getSize()
{
  int size=sizeof(Entry);
  size+=type.length()+1;
  size+=name.length()+1;
  size+=args.length()+1;
  size+=bitfields.length()+1;
  size+=exception.length()+1;
  size+=program.length()+1;
  size+=includeFile.length()+1;
  size+=includeName.length()+1;
  size+=doc.length()+1;
  size+=docFile.length()+1;
  size+=relates.length()+1;
  size+=brief.length()+1;
  size+=briefFile.length()+1;
  size+=inbodyDocs.length()+1;
  size+=inbodyFile.length()+1;
  size+=inside.length()+1;
  size+=fileName.length()+1;
  size+=initializer.length()+1;
  BaseInfo *bi=extends->first();
  while (bi)
  {
    size+=sizeof(QLNode);
    size+=bi->name.length()+1+sizeof(bi->prot)+sizeof(bi->virt);
    bi=extends->next(); 
  }
  Grouping *g=groups->first();
  while (g)
  {
    size+=sizeof(QLNode);
    size+=g->groupname.length()+1;
    size+=sizeof(g->pri);
    g=groups->next();
  }
  Entry *e=sublist->first();
  while (e)
  {
    size+=e->getSize();
    e=sublist->next();
  }
  Argument *a=argList->first();
  while (a)
  {
    size+=sizeof(Argument);
    size+=a->type.length()+1
         +a->name.length()+1
         +a->defval.length()+1;
    a=argList->next();
  }
  if (tArgLists)
  {
    ArgumentList *al=tArgLists->first();
    while (al)
    {
      size+=sizeof(ArgumentList);
      a=al->first();
      while (a)
      {
        size+=sizeof(Argument);
        size+=a->type.length()+1
          +a->name.length()+1
          +a->defval.length()+1;
        a=al->next();
      }
      al=tArgLists->next();
    }
  }
  //if (mtArgList)
  //{
  //  a=mtArgList->first();
  //  while (e)
  //  {
  //    size+=sizeof(Argument);
  //    size+=a->type.length()+1
  //      +a->name.length()+1
  //      +a->defval.length()+1;
  //    a=mtArgList->next();
  //  }
  //}
  return size;
}

/*! the argument list is documented if one of its
 *  arguments is documented 
 */
bool ArgumentList::hasDocumentation() const
{
  bool hasDocs=FALSE;
  ArgumentListIterator ali(*this);
  Argument *a;
  for (ali.toFirst();!hasDocs && (a=ali.current());++ali)
  {
    hasDocs = hasDocs || a->hasDocumentation(); 
  }
  return hasDocs;
}

void Entry::addSpecialListItem(const char *listName,int itemId)
{
  if (sli==0)
  {
    sli = new QList<ListItemInfo>;
    sli->setAutoDelete(TRUE);
  }
  ListItemInfo *ili=new ListItemInfo;
  ili->type = listName;
  ili->itemId = itemId;
  sli->append(ili);
}

