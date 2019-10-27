/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <algorithm>
#include <stdlib.h>
#include <qfile.h>
#include "entry.h"
#include "util.h"
#include "section.h"
#include "doxygen.h"
#include "arguments.h"
#include "config.h"
//------------------------------------------------------------------

#define HEADER ('D'<<24)+('O'<<16)+('X'<<8)+'!'

//------------------------------------------------------------------

int Entry::num=0;

Entry::Entry()
{
  //printf("Entry::Entry(%p)\n",this);
  num++;
  m_parent=0;
  section = EMPTY_SEC;
  extends = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors = new QList<SectionInfo>; // Doxygen::sectionDict takes ownership of the items!
  //printf("Entry::Entry() tArgList=0\n");
  mGrpId = -1;
  tagInfo = 0;
  sli = 0;
  relatesType = Simple;
  hidden = FALSE;
  groupDocType = GROUPDOC_NORMAL;
  reset();
}

Entry::Entry(const Entry &e)
{
  //printf("Entry::Entry(%p):copy\n",this);
  num++;
  section     = e.section;
  type        = e.type;
  name        = e.name;
  tagInfo     = e.tagInfo;
  protection  = e.protection;
  mtype       = e.mtype;
  spec        = e.spec;
  initLines   = e.initLines;
  stat        = e.stat;
  localToc    = e.localToc;
  explicitExternal = e.explicitExternal;
  proto       = e.proto;
  subGrouping = e.subGrouping;
  callGraph   = e.callGraph;
  callerGraph = e.callerGraph;
  referencedByRelation = e.referencedByRelation;
  referencesRelation   = e.referencesRelation;
  virt        = e.virt;
  args        = e.args;
  bitfields   = e.bitfields;
  argList     = e.argList;
  tArgLists   = e.tArgLists;
  program     = e.program;
  initializer = e.initializer;
  includeFile = e.includeFile;
  includeName = e.includeName;
  doc         = e.doc;
  docLine     = e.docLine;
  docFile     = e.docFile;
  brief       = e.brief;
  briefLine   = e.briefLine;
  briefFile   = e.briefFile;
  inbodyDocs  = e.inbodyDocs;
  inbodyLine  = e.inbodyLine;
  inbodyFile  = e.inbodyFile;
  relates     = e.relates;
  relatesType = e.relatesType;
  read        = e.read;
  write       = e.write;
  inside      = e.inside;
  exception   = e.exception;
  typeConstr  = e.typeConstr;
  bodyLine    = e.bodyLine;
  endBodyLine = e.endBodyLine;
  mGrpId      = e.mGrpId;
  extends     = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups      = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors     = new QList<SectionInfo>;
  fileName    = e.fileName;
  startLine   = e.startLine;
  startColumn = e.startColumn;
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
  lang        = e.lang;
  hidden      = e.hidden;
  artificial  = e.artificial;
  groupDocType = e.groupDocType;
  id          = e.id;

  m_parent    = e.m_parent;

  // deep copy child entries
  m_sublist.reserve(e.m_sublist.size());
  for (const auto &cur : e.m_sublist)
  {
    m_sublist.push_back(std::make_unique<Entry>(*cur));
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
    anchors->append(s); // shallow copy, object are owned by Doxygen::sectionDict
  }

  m_fileDef = e.m_fileDef;

}

Entry::~Entry()
{
  //printf("Entry::~Entry(%p) num=%d\n",this,num);
  //printf("Deleting entry %d name %s type %x children %d\n",
  //       num,name.data(),section,sublist->count());

  delete extends;
  delete groups;
  delete anchors;
  delete tagInfo;
  delete sli;
  num--;
}

void Entry::moveToSubEntryAndRefresh(Entry *&current)
{
  current->m_parent=this;
  m_sublist.emplace_back(current);
  current = new Entry;
}

void Entry::moveToSubEntryAndRefresh(std::unique_ptr<Entry> &current)
{
  current->m_parent=this;
  m_sublist.push_back(std::move(current));
  current = std::make_unique<Entry>();
}

void Entry::moveToSubEntryAndKeep(Entry *current)
{
  current->m_parent=this;
  m_sublist.emplace_back(current);
}

void Entry::moveToSubEntryAndKeep(std::unique_ptr<Entry> &current)
{
  current->m_parent=this;
  m_sublist.push_back(std::move(current));
}

void Entry::copyToSubEntry(Entry *current)
{
  Entry *copy = new Entry(*current);
  copy->m_parent=this;
  m_sublist.emplace_back(copy);
}

void Entry::copyToSubEntry(const std::unique_ptr<Entry> &current)
{
  std::unique_ptr<Entry> copy = std::make_unique<Entry>(*current);
  copy->m_parent=this;
  m_sublist.push_back(std::move(copy));
}

void Entry::moveFromSubEntry(const Entry *child,std::unique_ptr<Entry> &moveTo)
{
  auto it = std::find_if(m_sublist.begin(),m_sublist.end(),
      [child](const std::unique_ptr<Entry>&elem) { return elem.get()==child; });
  if (it!=m_sublist.end())
  {
    moveTo = std::move(*it);
    m_sublist.erase(it);
  }
  else
  {
    moveTo.reset();
  }
}

void Entry::removeSubEntry(const Entry *e)
{
  auto it = std::find_if(m_sublist.begin(),m_sublist.end(),
      [e](const std::unique_ptr<Entry>&elem) { return elem.get()==e; });
  if (it!=m_sublist.end())
  {
    m_sublist.erase(it);
  }
}


void Entry::reset()
{
  static bool entryCallGraph   = Config_getBool(CALL_GRAPH);
  static bool entryCallerGraph = Config_getBool(CALLER_GRAPH);
  static bool entryReferencedByRelation = Config_getBool(REFERENCED_BY_RELATION);
  static bool entryReferencesRelation   = Config_getBool(REFERENCES_RELATION);
  //printf("Entry::reset()\n");
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
  relatesType=Simple;
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
  startColumn = 1;
  bodyLine = -1;
  endBodyLine = -1;
  mGrpId = -1;
  callGraph   = entryCallGraph;
  callerGraph = entryCallerGraph;
  referencedByRelation = entryReferencedByRelation;
  referencesRelation   = entryReferencesRelation;
  section = EMPTY_SEC;
  mtype   = Method;
  virt    = Normal;
  stat    = FALSE;
  proto   = FALSE;
  explicitExternal = FALSE;
  spec  = 0;
  lang = SrcLangExt_Unknown;
  hidden = FALSE;
  artificial = FALSE;
  subGrouping = TRUE;
  protection = Public;
  groupDocType = GROUPDOC_NORMAL;
  id.resize(0);
  metaData.resize(0);
  m_sublist.clear();
  extends->clear();
  groups->clear();
  anchors->clear();
  argList.clear();
  tArgLists.clear();
  argList.reset();
  typeConstr.reset();
  if (tagInfo)    { delete tagInfo; tagInfo=0; }
  if (sli)        { delete sli; sli=0; }
  m_fileDef = 0;
}


int Entry::getSize()
{
  return sizeof(Entry);
}

void Entry::setFileDef(FileDef *fd)
{
  m_fileDef = fd;
  for (const auto &childNode : m_sublist)
  {
      childNode->setFileDef(fd);
  }
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


//------------------------------------------------------------------
