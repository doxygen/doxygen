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
  //printf("OutputList::OutputList()\n");
}

OutputList::OutputList(const OutputList &ol)
{
  m_id = ol.m_id;
  for (const auto &og : ol.m_outputs)
  {
    m_outputs.emplace_back(og->clone());
  }
}

OutputList &OutputList::operator=(const OutputList &ol)
{
  if (this!=&ol)
  {
    m_id = ol.m_id;
    for (const auto &og : ol.m_outputs)
    {
      m_outputs.emplace_back(og->clone());
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

void OutputList::disableAllBut(OutputGenerator::OutputType o)
{
  for (const auto &og : m_outputs)
  {
    og->disableIfNot(o);
  }
}

void OutputList::enableAll()
{
  for (const auto &og : m_outputs)
  {
    og->enable();
  }
}

void OutputList::disableAll()
{
  for (const auto &og : m_outputs)
  {
    og->disable();
  }
}

void OutputList::disable(OutputGenerator::OutputType o)
{
  for (const auto &og : m_outputs)
  {
    og->disableIf(o);
  }
}

void OutputList::enable(OutputGenerator::OutputType o)
{
  for (const auto &og : m_outputs)
  {
    og->enableIf(o);
  }
}

bool OutputList::isEnabled(OutputGenerator::OutputType o)
{
  bool result=FALSE;
  for (const auto &og : m_outputs)
  {
    result=result || og->isEnabled(o);
  }
  return result;
}

void OutputList::pushGeneratorState()
{
  for (const auto &og : m_outputs)
  {
    og->pushGeneratorState();
  }
}

void OutputList::popGeneratorState()
{
  for (const auto &og : m_outputs)
  {
    og->popGeneratorState();
  }
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

  // we want to validate irrespective of the number of output formats
  // specified as:
  // - when only XML format there should be warnings as well (XML has its own write routines)
  // - no formats there should be warnings as well
  std::unique_ptr<IDocParser> parser { createDocParser() };
  std::unique_ptr<DocRoot>    root   { validatingParseDoc(*parser.get(),
                                       fileName,startLine,
                                       ctx,md,docStr,indexWords,isExample,exampleName,
                                       singleLine,linkFromIndex,markdownSupport) };
  if (count>0) writeDoc(root.get(),ctx,md,m_id);
}

void OutputList::writeDoc(DocRoot *root,const Definition *ctx,const MemberDef *md,int)
{
  for (const auto &og : m_outputs)
  {
    //printf("og->printDoc(extension=%s)\n",
    //    ctx?qPrint(ctx->getDefFileExtension()):"<null>");
    if (og->isEnabled()) og->writeDoc(root,ctx,md,m_id);
  }
  VhdlDocGen::setFlowMember(0);
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
  std::unique_ptr<IDocParser> parser { createDocParser() };
  std::unique_ptr<DocText>    root   { validatingParseText(*parser.get(), textStr) };

  if (count>0)
  {
    for (const auto &og : m_outputs)
    {
      if (og->isEnabled()) og->writeDoc(root.get(),0,0,m_id);
    }
  }
}

//--------------------------------------------------------------------------
