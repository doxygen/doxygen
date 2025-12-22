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
#include <atomic>
#include <stdlib.h>

#include "entry.h"
#include "util.h"
#include "section.h"
#include "doxygen.h"
#include "arguments.h"
#include "config.h"

//------------------------------------------------------------------

static AtomicInt g_num;

Entry::Entry() : section(EntryType::makeEmpty()), program(static_cast<size_t>(0)), initializer(static_cast<size_t>(0))
{
  //printf("Entry::Entry(%p)\n",this);
  g_num++;
  m_parent=nullptr;
  //printf("Entry::Entry() tArgList=0\n");
  mGrpId = -1;
  hasTagInfo = false;
  relatesType = RelatesType::Simple;
  hidden = FALSE;
  groupDocType = GROUPDOC_NORMAL;
  reset();
}

Entry::Entry(const Entry &e) : section(e.section)
{
  //printf("Entry::Entry(%p):copy\n",this);
  g_num++;
  type        = e.type;
  name        = e.name;
  hasTagInfo  = e.hasTagInfo;
  tagInfoData = e.tagInfoData;
  protection  = e.protection;
  mtype       = e.mtype;
  spec        = e.spec;
  vhdlSpec    = e.vhdlSpec;
  initLines   = e.initLines;
  isStatic    = e.isStatic;
  localToc    = e.localToc;
  explicitExternal = e.explicitExternal;
  proto       = e.proto;
  subGrouping = e.subGrouping;
  commandOverrides = e.commandOverrides;
  exported    = e.exported;
  virt        = e.virt;
  args        = e.args;
  bitfields   = e.bitfields;
  argList     = e.argList;
  tArgLists   = e.tArgLists;
  program.str(e.program.str());
  initializer.str(e.initializer.str());
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
  bodyColumn  = e.bodyColumn;
  endBodyLine = e.endBodyLine;
  mGrpId      = e.mGrpId;
  anchors     = e.anchors;
  fileName    = e.fileName;
  startLine   = e.startLine;
  startColumn = e.startColumn;
  sli         = e.sli;
  rqli        = e.rqli;
  lang        = e.lang;
  hidden      = e.hidden;
  artificial  = e.artificial;
  groupDocType = e.groupDocType;
  id          = e.id;
  extends     = e.extends;
  groups      = e.groups;
  req         = e.req;
  m_fileDef   = e.m_fileDef;
  qualifiers  = e.qualifiers;

  m_parent    = e.m_parent;
  // deep copy child entries
  m_sublist.reserve(e.m_sublist.size());
  for (const auto &cur : e.m_sublist)
  {
    m_sublist.push_back(std::make_shared<Entry>(*cur));
  }
}

Entry::~Entry()
{
  //printf("Entry::~Entry(%p) num=%d\n",this,g_num);
  //printf("Deleting entry %d name %s type %x children %d\n",
  //       num,qPrint(name),section,sublist->count());

  g_num--;
}

void Entry::moveToSubEntryAndRefresh(Entry *&current)
{
  current->m_parent=this;
  m_sublist.emplace_back(current);
  current = new Entry;
}

void Entry::moveToSubEntryAndRefresh(std::shared_ptr<Entry> &current)
{
  current->m_parent=this;
  m_sublist.push_back(current);
  current = std::make_shared<Entry>();
}

void Entry::moveToSubEntryAndKeep(Entry *current)
{
  current->m_parent=this;
  m_sublist.emplace_back(current);
}

void Entry::moveToSubEntryAndKeep(std::shared_ptr<Entry> current)
{
  current->m_parent=this;
  m_sublist.push_back(current);
}

void Entry::copyToSubEntry(Entry *current)
{
  Entry *copy = new Entry(*current);
  copy->m_parent=this;
  m_sublist.emplace_back(copy);
}

void Entry::copyToSubEntry(const std::shared_ptr<Entry> &current)
{
  std::shared_ptr<Entry> copy = std::make_shared<Entry>(*current);
  copy->m_parent=this;
  m_sublist.push_back(copy);
}

void Entry::removeSubEntry(const Entry *e)
{
  auto it = std::find_if(m_sublist.begin(),m_sublist.end(),
      [e](const std::shared_ptr<Entry>&elem) { return elem.get()==e; });
  if (it!=m_sublist.end())
  {
    m_sublist.erase(it);
  }
}


void Entry::reset()
{
  //printf("Entry::reset()\n");
  name.clear();
  type.clear();
  args.clear();
  bitfields.clear();
  exception.clear();
  program.str(std::string());
  includeFile.clear();
  includeName.clear();
  doc.clear();
  docFile.clear();
  docLine=-1;
  relates.clear();
  relatesType=RelatesType::Simple;
  brief.clear();
  briefFile.clear();
  briefLine=-1;
  inbodyDocs.clear();
  inbodyFile.clear();
  inbodyLine=-1;
  inside.clear();
  fileName.clear();
  initializer.str(std::string());
  initLines = -1;
  startLine = 1;
  startColumn = 1;
  bodyLine = -1;
  bodyColumn = 1;
  endBodyLine = -1;
  mGrpId = -1;
  commandOverrides.reset();
  exported = false;
  section = EntryType::makeEmpty();
  mtype   = MethodTypes::Method;
  virt    = Specifier::Normal;
  isStatic = false;
  proto   = false;
  explicitExternal = false;
  spec.reset();
  vhdlSpec = VhdlSpecifier::UNKNOWN;
  lang = SrcLangExt::Unknown;
  hidden = false;
  artificial = false;
  subGrouping = true;
  protection = Protection::Public;
  groupDocType = GROUPDOC_NORMAL;
  id.clear();
  metaData.clear();
  m_sublist.clear();
  extends.clear();
  groups.clear();
  anchors.clear();
  argList.reset();
  tArgLists.clear();
  typeConstr.reset();
  sli.clear();
  rqli.clear();
  req.clear();
  m_fileDef = nullptr;
  qualifiers.clear();
}

void Entry::setFileDef(FileDef *fd)
{
  m_fileDef = fd;
  for (const auto &childNode : m_sublist)
  {
      childNode->setFileDef(fd);
  }
}

//------------------------------------------------------------------
