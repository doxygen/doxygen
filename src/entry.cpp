/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

#include <qfile.h>
#include "entry.h"
#include "util.h"
#include "section.h"

int Entry::num=0;

Entry::Entry()
{
  //printf("Entry::Entry(%p)\n",this);
  num++;
  m_parent=0;
  section = EMPTY_SEC;
  m_sublist = new QList<Entry>;
  m_sublist->setAutoDelete(TRUE);
  extends = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors = new QList<SectionInfo>; // Doxygen::sectionDict takes ownership of the items!
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
  //printf("Entry::Entry(%p):copy\n",this);
  num++;
  section     = e.section;
  protection  = e.protection;
  mtype       = e.mtype;
  stat        = e.stat;
  explicitExternal = e.explicitExternal;
  virt        = e.virt;
  m_parent    = e.m_parent;
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
  callerGraph = e.callerGraph;
  objc        = e.objc;
  tagInfo     = e.tagInfo;
  hidden      = e.hidden;
  m_sublist   = new QList<Entry>;
  m_sublist->setAutoDelete(TRUE);
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
  QListIterator<Entry> eli(*e.m_sublist);
  Entry *cur;
  for (;(cur=eli.current());++eli)
  {
    m_sublist->append(new Entry(*cur));
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
  //printf("Entry::~Entry(%p) num=%d\n",this,num);
  //printf("Deleting entry %d name %s type %x children %d\n",
  //       num,name.data(),section,sublist->count());
  
  delete m_sublist; // each element is now own by a EntryNav so we do no longer own
                  // our children.
  delete extends;
  delete groups;
  delete anchors;
  delete argList;
  delete tArgLists;
  delete tagInfo;
  delete sli;
  num--;
}

void Entry::addSubEntry(Entry *current)
{
  //printf("Entry %d with name %s type 0x%x added to %s type 0x%x\n",
  //    current->num,current->name.data(),current->section,
  //    name.data(),section);
  //printf("Entry::addSubEntry(%s:%p) to %s\n",current->name.data(),
  //    current,name.data());
  current->m_parent=this;
  m_sublist->append(current);  
}

void Entry::reset()
{
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
  callerGraph = FALSE;
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
  m_sublist->clear();
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

void Entry::createSubtreeIndex(EntryNav *nav,QFile &storage,FileDef *fd)
{
  EntryNav *childNav = new EntryNav(nav,this);
  nav->addChild(childNav);
  childNav->setFileDef(fd);
  childNav->saveEntry(this,storage);
  if (m_sublist)
  {
    //printf("saveEntry: %d children\n",node->sublist->count());
    QListIterator<Entry> eli(*m_sublist);
    Entry *childNode;
    for (eli.toFirst();(childNode=eli.current());++eli)
    {
      childNode->createSubtreeIndex(childNav,storage,fd);
    }
    //m_sublist->setAutoDelete(FALSE);
    m_sublist->clear();
  }
}

void Entry::createNavigationIndex(EntryNav *rootNav,QFile &storage,FileDef *fd)
{
  //printf("createNavigationIndex(%p) sublist=%p\n",this,m_sublist);
  if (m_sublist)
  {
    //printf("saveEntries: %d children\n",root->sublist->count());
    // store all child entries of root, but keep the navigation info (=index)
    QListIterator<Entry> eli(*m_sublist);
    Entry *e;
    for (eli.toFirst();(e=eli.current());++eli)
    {
      createSubtreeIndex(rootNav,storage,fd);
    }
    // remove all entries from root
    //m_sublist->setAutoDelete(FALSE);
    m_sublist->clear();
  }
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

//------------------------------------------------------------------

#define NULL_LIST 0xffffffff

void marshalInt(QFile &f,int v)
{
  uchar b[4];
  b[0]=((uint)v)>>24;
  b[1]=(((uint)v)>>16)&0xff;
  b[2]=(((uint)v)>>8)&0xff;
  b[3]=v&0xff;
  f.writeBlock((const char *)b,4);
}

void marshalUInt(QFile &f,uint v)
{
  uchar b[4];
  b[0]=v>>24;
  b[1]=(v>>16)&0xff;
  b[2]=(v>>8)&0xff;
  b[3]=v&0xff;
  f.writeBlock((const char *)b,4);
}

void marshalBool(QFile &f,bool b)
{
  char c = b;
  f.writeBlock(&c,sizeof(char));
}

void marshalQCString(QFile &f,const QCString &s)
{
  uint l=s.length();
  marshalUInt(f,l);
  if (l>0) f.writeBlock(s.data(),l);
}

void marshalArgumentList(QFile &f,ArgumentList *argList)
{
  if (argList==0)
  {
    marshalUInt(f,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(f,argList->count());
    ArgumentListIterator ali(*argList);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      marshalQCString(f,a->attrib);    
      marshalQCString(f,a->type);    
      marshalQCString(f,a->canType);    
      marshalQCString(f,a->name);    
      marshalQCString(f,a->array);    
      marshalQCString(f,a->defval);    
      marshalQCString(f,a->docs);    
    }
    marshalBool(f,argList->constSpecifier);
    marshalBool(f,argList->volatileSpecifier);
    marshalBool(f,argList->pureSpecifier);
  }
}

void marshalArgumentLists(QFile &f,QList<ArgumentList> *argLists)
{
  if (argLists==0)
  {
    marshalUInt(f,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(f,argLists->count());
    QListIterator<ArgumentList> ali(*argLists);
    ArgumentList *al;
    for (ali.toFirst();(al=ali.current());++ali)
    {
      marshalArgumentList(f,al);
    }
  }
}

void marshalBaseInfoList(QFile &f, QList<BaseInfo> *baseList)
{
  if (baseList==0)
  {
    marshalUInt(f,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(f,baseList->count());
    QListIterator<BaseInfo> bli(*baseList);
    BaseInfo *bi;
    for (bli.toFirst();(bi=bli.current());++bli)
    {
      marshalQCString(f,bi->name);
      marshalInt(f,(int)bi->prot);
      marshalInt(f,(int)bi->virt);
    }
  }
}

void marshalGroupingList(QFile &f, QList<Grouping> *groups)
{
  if (groups==0)
  {
    marshalUInt(f,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(f,groups->count());
    QListIterator<Grouping> gli(*groups);
    Grouping *g;
    for (gli.toFirst();(g=gli.current());++gli)
    {
      marshalQCString(f,g->groupname);
      marshalInt(f,(int)g->pri);
    }
  }
}

void marshalSectionInfoList(QFile &f, QList<SectionInfo> *anchors)
{
  if (anchors==0)
  {
    marshalUInt(f,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(f,anchors->count());
    QListIterator<SectionInfo> sli(*anchors);
    SectionInfo *si;
    for (sli.toFirst();(si=sli.current());++sli)
    {
      marshalQCString(f,si->label);
      marshalQCString(f,si->title);
      marshalQCString(f,si->ref);
      marshalInt(f,(int)si->type);
      marshalQCString(f,si->fileName);
    }
  }
}

void marshalItemInfoList(QFile &f, QList<ListItemInfo> *sli)
{
  if (sli==0)
  {
    marshalUInt(f,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(f,sli->count());
    QListIterator<ListItemInfo> liii(*sli);
    ListItemInfo *lii;
    for (liii.toFirst();(lii=liii.current());++liii)
    {
      marshalQCString(f,lii->type);
      marshalInt(f,lii->itemId);
    }
  }
}

//------------------------------------------------------------------

int unmarshalInt(QFile &f)
{
  uchar b[4];
  f.readBlock((char *)b,4);
  int result=(int)((((uint)b[0])<<24)+((uint)b[1]<<16)+((uint)b[2]<<8)+(uint)b[3]);
  //printf("unmarshalInt: %x %x %x %x: %x offset=%llx\n",b[0],b[1],b[2],b[3],result,f.pos());
  return result;
}

uint unmarshalUInt(QFile &f)
{
  uchar b[4];
  f.readBlock((char *)b,4);
  uint result=(((uint)b[0])<<24)+((uint)b[1]<<16)+((uint)b[2]<<8)+(uint)b[3];
  //printf("unmarshalUInt: %x %x %x %x: %x offset=%llx\n",b[0],b[1],b[2],b[3],result,f.pos());
  return result;
}

bool unmarshalBool(QFile &f)
{
  char result;
  f.readBlock(&result,sizeof(result));
  //printf("unmarshalBool: %x offset=%llx\n",result,f.pos());
  return result;
}

QCString unmarshalQCString(QFile &f)
{
  uint len = unmarshalUInt(f);
  //printf("unmarshalQCString: len=%d offset=%llx\n",len,f.pos());
  QCString result(len+1);
  result.at(len)='\0';
  if (len>0)
  {
    f.readBlock(result.data(),len);
  }
  //printf("unmarshalQCString: result=%s\n",result.data());
  return result;
}

ArgumentList *unmarshalArgumentList(QFile &f)
{
  uint i;
  uint count = unmarshalUInt(f);
  if (count==NULL_LIST) return 0; // null list
  ArgumentList *result = new ArgumentList;
  result->setAutoDelete(TRUE);
  //printf("unmarshalArgumentList: %d\n",count);
  for (i=0;i<count;i++)
  {
    Argument *a = new Argument;
    a->attrib  = unmarshalQCString(f);
    a->type    = unmarshalQCString(f);
    a->canType = unmarshalQCString(f);
    a->name    = unmarshalQCString(f);
    a->array   = unmarshalQCString(f);
    a->defval  = unmarshalQCString(f);
    a->docs    = unmarshalQCString(f);
    result->append(a);
  }
  result->constSpecifier    = unmarshalBool(f);
  result->volatileSpecifier = unmarshalBool(f);
  result->pureSpecifier     = unmarshalBool(f);
  return result;
}

QList<ArgumentList> *unmarshalArgumentLists(QFile &f)
{
  uint i;
  uint count = unmarshalUInt(f);
  if (count==NULL_LIST) return 0; // null list
  QList<ArgumentList> *result = new QList<ArgumentList>;
  result->setAutoDelete(TRUE);
  //printf("unmarshalArgumentLists: %d\n",count);
  for (i=0;i<count;i++)
  {
    result->append(unmarshalArgumentList(f));
  }
  return result;
}

QList<BaseInfo> *unmarshalBaseInfoList(QFile &f)
{
  uint i;
  uint count = unmarshalUInt(f);
  if (count==NULL_LIST) return 0; // null list
  QList<BaseInfo> *result = new QList<BaseInfo>;
  result->setAutoDelete(TRUE);
  for (i=0;i<count;i++)
  {
    QCString name   = unmarshalQCString(f);
    Protection prot = (Protection)unmarshalInt(f);
    Specifier virt  = (Specifier)unmarshalInt(f);
    result->append(new BaseInfo(name,prot,virt));
  }
  return result;
}

QList<Grouping> *unmarshalGroupingList(QFile &f)
{
  uint i;
  uint count = unmarshalUInt(f);
  if (count==NULL_LIST) return 0; // null list
  QList<Grouping> *result = new QList<Grouping>;
  result->setAutoDelete(TRUE);
  for (i=0;i<count;i++)
  {
    QCString name = unmarshalQCString(f);
    Grouping::GroupPri_t prio = (Grouping::GroupPri_t)unmarshalInt(f);
    result->append(new Grouping(name,prio));
  }
  return result;
}

QList<SectionInfo> *unmarshalSectionInfoList(QFile &f)
{
  uint i;
  uint count = unmarshalUInt(f);
  if (count==NULL_LIST) return 0; // null list
  QList<SectionInfo> *result = new QList<SectionInfo>;
  result->setAutoDelete(TRUE);
  for (i=0;i<count;i++)
  { 
    QCString label = unmarshalQCString(f);
    QCString title = unmarshalQCString(f);
    QCString ref   = unmarshalQCString(f);
    SectionInfo::SectionType type = (SectionInfo::SectionType)unmarshalInt(f);
    QCString fileName = unmarshalQCString(f);
    result->append(new SectionInfo(fileName,label,title,type,ref));
  }
  return result;
}

QList<ListItemInfo> *unmarshalItemInfoList(QFile &f)
{
  uint i;
  uint count = unmarshalUInt(f);
  if (count==NULL_LIST) return 0; // null list
  QList<ListItemInfo> *result = new QList<ListItemInfo>;
  result->setAutoDelete(TRUE);
  for (i=0;i<count;i++)
  { 
    ListItemInfo *lii = new ListItemInfo;
    lii->type   = unmarshalQCString(f);
    lii->itemId = unmarshalInt(f);
    result->append(lii);
  }
  return result;
}

//------------------------------------------------------------------

#define HEADER ('D'<<24)+('O'<<16)+('X'<<8)+'!'

bool saveEntry(Entry *e,QFile &f)
{
  marshalUInt(f,HEADER);
  marshalInt(f,(int)e->protection);
  marshalInt(f,(int)e->mtype);
  marshalInt(f,e->memSpec);
  marshalInt(f,e->initLines);
  marshalBool(f,e->stat);
  marshalBool(f,e->explicitExternal);
  marshalBool(f,e->proto);
  marshalBool(f,e->subGrouping);
  marshalBool(f,e->callGraph);
  marshalBool(f,e->callerGraph);
  marshalInt(f,(int)e->virt);
  marshalQCString(f,e->args);
  marshalQCString(f,e->bitfields);
  marshalArgumentList(f,e->argList);
  marshalArgumentLists(f,e->tArgLists);
  marshalQCString(f,e->program);
  marshalQCString(f,e->initializer);
  marshalQCString(f,e->includeFile);
  marshalQCString(f,e->includeName);
  marshalQCString(f,e->doc);
  marshalInt(f,e->docLine);
  marshalQCString(f,e->docFile);
  marshalQCString(f,e->brief);
  marshalInt(f,e->briefLine);
  marshalQCString(f,e->briefFile);
  marshalQCString(f,e->inbodyDocs);
  marshalInt(f,e->inbodyLine);
  marshalQCString(f,e->inbodyFile);
  marshalQCString(f,e->relates);
  marshalBool(f,e->relatesDup);
  marshalQCString(f,e->read);
  marshalQCString(f,e->write);
  marshalQCString(f,e->inside);
  marshalQCString(f,e->exception);
  marshalInt(f,e->bodyLine);
  marshalInt(f,e->endBodyLine);
  marshalInt(f,e->mGrpId);
  marshalBaseInfoList(f,e->extends);
  marshalGroupingList(f,e->groups);
  marshalSectionInfoList(f,e->anchors);
  marshalQCString(f,e->fileName);
  marshalInt(f,e->startLine);
  marshalItemInfoList(f,e->sli);
  marshalBool(f,e->objc);
  marshalBool(f,e->hidden);
  marshalInt(f,(int)e->groupDocType);
  return TRUE;
}

bool loadEntry(Entry *e,QFile &f)
{
  uint header=unmarshalUInt(f);
  if (header!=HEADER)
  {
    printf("Internal error: Invalid header for entry in storage file: %x. Disk full?\n",header);
    exit(1);
  }
  e->protection       = (Protection)unmarshalInt(f);
  e->mtype            = (MethodTypes)unmarshalInt(f);
  e->memSpec          = unmarshalInt(f);
  e->initLines        = unmarshalInt(f);
  e->stat             = unmarshalBool(f);
  e->explicitExternal = unmarshalBool(f);
  e->proto            = unmarshalBool(f);
  e->subGrouping      = unmarshalBool(f);
  e->callGraph        = unmarshalBool(f);
  e->callerGraph      = unmarshalBool(f);
  e->virt             = (Specifier)unmarshalInt(f);
  e->args             = unmarshalQCString(f);
  e->bitfields        = unmarshalQCString(f);
  delete e->argList;
  e->argList          = unmarshalArgumentList(f);
  e->tArgLists        = unmarshalArgumentLists(f);
  e->program          = unmarshalQCString(f);
  e->initializer      = unmarshalQCString(f);
  e->includeFile      = unmarshalQCString(f);
  e->includeName      = unmarshalQCString(f);
  e->doc              = unmarshalQCString(f);
  e->docLine          = unmarshalInt(f);
  e->docFile          = unmarshalQCString(f);
  e->brief            = unmarshalQCString(f);
  e->briefLine        = unmarshalInt(f);
  e->briefFile        = unmarshalQCString(f);
  e->inbodyDocs       = unmarshalQCString(f);
  e->inbodyLine       = unmarshalInt(f);
  e->inbodyFile       = unmarshalQCString(f);
  e->relates          = unmarshalQCString(f);
  e->relatesDup       = unmarshalBool(f);
  e->read             = unmarshalQCString(f);
  e->write            = unmarshalQCString(f);
  e->inside           = unmarshalQCString(f);
  e->exception        = unmarshalQCString(f);
  e->bodyLine         = unmarshalInt(f);
  e->endBodyLine      = unmarshalInt(f);
  e->mGrpId           = unmarshalInt(f);
  delete e->extends;
  e->extends          = unmarshalBaseInfoList(f);
  delete e->groups;
  e->groups           = unmarshalGroupingList(f);
  delete e->anchors;
  e->anchors          = unmarshalSectionInfoList(f);
  e->fileName         = unmarshalQCString(f);
  e->startLine        = unmarshalInt(f);
  e->sli              = unmarshalItemInfoList(f);
  e->objc             = unmarshalBool(f);
  e->hidden           = unmarshalBool(f);
  e->groupDocType     = (Entry::GroupDocType)unmarshalInt(f);
  return TRUE;
}

EntryNav::EntryNav(EntryNav *parent, Entry *e)
             : m_parent(parent), m_subList(0), m_section(e->section), m_type(e->type),
              m_name(e->name), m_fileDef(0), m_info(0), m_offset(-1), 
              m_noLoad(FALSE)
{
  if (e->tagInfo)
  {
    m_tagInfo = new TagInfo;
    m_tagInfo->tagName  = e->tagInfo->tagName;
    m_tagInfo->fileName = e->tagInfo->fileName;
    m_tagInfo->anchor   = e->tagInfo->anchor;
    if (e->tagInfo)
    {
      //printf("tagInfo %p: tagName=%s fileName=%s anchor=%s\n",
      //    e->tagInfo,
      //    e->tagInfo->tagName.data(),
      //    e->tagInfo->fileName.data(),
      //    e->tagInfo->anchor.data());
    }
  }
  else
  {
    m_tagInfo = 0;
  }
}

EntryNav::~EntryNav()
{
  delete m_subList;
  delete m_info;
  delete m_tagInfo;
}

void EntryNav::addChild(EntryNav *e)
{
  if (m_subList==0) 
  {
    m_subList = new QList<EntryNav>;
    m_subList->setAutoDelete(TRUE);
  }
  m_subList->append(e);
}

bool EntryNav::loadEntry(QFile &storage)
{
  if (m_noLoad)
  {
    return TRUE;
  }
  if (m_offset==-1) 
  {
    //printf("offset not set!\n");
    return FALSE;
  }
  //delete m_info;
  if (m_info==0)  m_info = new Entry;
  //printf("EntryNav::loadEntry: new entry %p: %s\n",m_info,m_name.data());
  m_info->name = m_name;
  m_info->type = m_type;
  m_info->section = m_section;
  //m_info->tagInfo = m_tagInfo;
  //if (m_parent)
  //{
  //  m_info->parent = m_parent->m_info;
  //}
  //m_info->parent = 0;
  //printf("load entry: seek to %llx\n",m_offset);
  if (!storage.seek(m_offset)) 
  {
    //printf("seek failed!\n");
    return FALSE;
  }
  return ::loadEntry(m_info,storage);
}

bool EntryNav::saveEntry(Entry *e,QFile &storage)
{
  m_offset = storage.pos();
  //printf("EntryNav::saveEntry offset=%llx\n",m_offset);
  return ::saveEntry(e,storage);
}

void EntryNav::releaseEntry()
{
  if (!m_noLoad) 
  { 
    //printf("EntryNav::releaseEntry %p\n",m_info);
    delete m_info; 
    m_info=0; 
  }
}

void EntryNav::setEntry(Entry *e) 
{ 
  delete m_info;
  m_info = e; 
  //printf("EntryNav::setEntry %p\n",e);
  m_noLoad=TRUE; 
}

