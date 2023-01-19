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

/*! \file
 *  This class represents a list of output generators that work in "parallel".
 *  The class only knows about the abstract base class OutputGenerators.
 *  All output is produced by calling a method of this class, which forwards
 *  the call to all output generators.
 */

#include <atomic>

#include "outputlist.h"
#include "outputgen.h"
#include "config.h"
#include "message.h"
#include "definition.h"
#include "docparser.h"
#include "vhdldocgen.h"
#include "doxygen.h"

static AtomicInt g_outId;

OutputList::OutputList()
{
  newId();
  m_codeGenList.setId(m_id);
}

OutputList::OutputList(const OutputList &ol)
{
  m_id = ol.m_id;
  m_codeGenList.setId(m_id);
  for (const auto &og : ol.m_outputs)
  {
    auto docGen = og->clone();
    docGen->codeGen()->setId(m_id);
    m_codeGenList.add(docGen->codeGen());
    m_outputs.emplace_back(std::move(docGen));
  }
}

OutputList &OutputList::operator=(const OutputList &ol)
{
  if (this!=&ol)
  {
    m_id = ol.m_id;
    m_codeGenList.setId(m_id);
    for (const auto &og : ol.m_outputs)
    {
      auto docGen = og->clone();
      docGen->codeGen()->setId(m_id);
      m_codeGenList.add(docGen->codeGen());
      m_outputs.emplace_back(std::move(docGen));
    }
  }
  return *this;
}

OutputList::~OutputList()
{
  //printf("OutputList::~OutputList()\n");
}

void OutputList::newId()
{
  m_id = ++g_outId;
}

void OutputList::syncEnabled()
{
  for (const auto &og : m_outputs)
  {
    //printf("output %d isEnabled=%d\n",og->type(),og->isEnabled());
    m_codeGenList.setEnabledFiltered(og->type(),og->isEnabled());
  }
}

void OutputList::disableAllBut(OutputType o)
{
  //printf("disableAllBut(%d)\n",o);
  for (const auto &og : m_outputs)
  {
    if (og->type()!=o) og->setEnabled(false);
  }
  syncEnabled();
}

void OutputList::enableAll()
{
  //printf("enableAll()\n");
  for (const auto &og : m_outputs)
  {
    og->setEnabled(true);
  }
  syncEnabled();
}

void OutputList::disableAll()
{
  //printf("enableAll()\n");
  for (const auto &og : m_outputs)
  {
    og->setEnabled(false);
  }
  syncEnabled();
}

void OutputList::disable(OutputType o)
{
  //printf("disable(%d)\n",o);
  for (const auto &og : m_outputs)
  {
    if (og->type()==o) og->setEnabled(false);
  }
  syncEnabled();
}

void OutputList::enable(OutputType o)
{
  //printf("enable(%d)\n",o);
  for (const auto &og : m_outputs)
  {
    if (og->type()==o) og->setEnabled(true);
  }
  syncEnabled();
}

bool OutputList::isEnabled(OutputType o)
{
  for (const auto &og : m_outputs)
  {
    if (og->type()==o) return og->isEnabled();
  }
  return false;
}

void OutputList::pushGeneratorState()
{
  //printf("pushGeneratorState()\n");
  for (const auto &og : m_outputs)
  {
    og->pushGeneratorState();
  }
  syncEnabled();
}

void OutputList::popGeneratorState()
{
  //printf("popGeneratorState()\n");
  for (const auto &og : m_outputs)
  {
    og->popGeneratorState();
  }
  syncEnabled();
}

void OutputList::generateDoc(const QCString &fileName,int startLine,
                  const Definition *ctx,const MemberDef * md,
                  const QCString &docStr,bool indexWords,
                  bool isExample,const QCString &exampleName,
                  bool singleLine,bool linkFromIndex,
                  bool markdownSupport)
{
  int count=0;
  if (docStr.isEmpty()) return;

  for (const auto &og : m_outputs)
  {
    if (og->isEnabled()) count++;
  }

  if (count>0)
  {
    // we want to validate irrespective of the number of output formats
    // specified as:
    // - when only XML format there should be warnings as well (XML has its own write routines)
    // - no formats there should be warnings as well
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     fileName,startLine,
                                     ctx,md,docStr,indexWords,isExample,exampleName,
                                     singleLine,linkFromIndex,markdownSupport) };
    if (ast) writeDoc(ast.get(),ctx,md,m_id);
  }
}

void OutputList::startFile(const QCString &name,const QCString &manName,const QCString &title)
{
  newId();
  m_codeGenList.setId(m_id);
  forall(&OutputGenerator::startFile,name,manName,title,m_id);
}

void OutputList::writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md,int)
{
  for (const auto &og : m_outputs)
  {
    //printf("og->printDoc(extension=%s)\n",
    //    ctx?qPrint(ctx->getDefFileExtension()):"<null>");
    if (og->isEnabled())
    {
      og->writeDoc(ast,ctx,md,m_id);
    }
  }
}

void OutputList::parseText(const QCString &textStr)
{
  int count=0;
  for (const auto &og : m_outputs)
  {
    if (og->isEnabled()) count++;
  }

  // we want to validate irrespective of the number of output formats
  // specified as:
  // - when only XML format there should be warnings as well (XML has its own write routines)
  // - no formats there should be warnings as well
  auto parser { createDocParser() };
  auto textNode { validatingParseText(*parser.get(), textStr) };

  if (textNode && count>0)
  {
    for (const auto &og : m_outputs)
    {
      if (og->isEnabled()) og->writeDoc(textNode.get(),0,0,m_id);
    }
  }
}

//--------------------------------------------------------------------------
