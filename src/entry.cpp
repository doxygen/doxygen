/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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
  mGrpId = -1;
  tagInfo = 0;
  sli = 0;
  relatesDup = FALSE;
  hidden = FALSE;
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
  read        = e.read.copy();
  write       = e.write.copy();
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
  tagInfo     = e.tagInfo;
  hidden      = e.hidden;
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
}

void Entry::reset()
{
  name.resize(0);
  type.resize(0);
  args.resize(0);
  bitfields.resize(0);
  exception.resize(0);
  program.resize(0);
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
  initializer.resize(0);
  initLines = -1;
  startLine = 1;
  bodyLine = -1;
  endBodyLine = -1;
  mGrpId = -1;
  callGraph = FALSE;
  section = EMPTY_SEC;
  mtype   = Method;
  virt    = Normal;
  stat    = FALSE;
  proto   = FALSE;
  explicitExternal = FALSE;
  memSpec  = 0;
  objc = FALSE;
  hidden = FALSE;
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
  return sizeof(Entry);
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

