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

OutputList::OutputList(const OutputList &ol) : m_outputGenList(ol.m_outputGenList)
{
  m_id = ol.m_id;
  refreshCodeGenerators();
}

OutputList &OutputList::operator=(const OutputList &ol)
{
  if (this!=&ol)
  {
    m_id = ol.m_id;
    m_outputGenList = ol.m_outputGenList;
    refreshCodeGenerators();
  }
  return *this;
}

void OutputList::refreshCodeGenerators()
{
  m_codeGenList.clear();
  for (auto &e : m_outputGenList)
  {
    std::visit([&](auto &&gen) { gen.addCodeGen(m_codeGenList); },e.variant);
  }
  m_codeGenList.setId(m_id);
}

void OutputList::newId()
{
  m_id = ++g_outId;
}

void OutputList::syncEnabled()
{
  for (const auto &e : m_outputGenList)
  {
    //printf("output %d isEnabled=%d\n",og->type(),og->isEnabled());
    std::visit([&](auto &&gen) { m_codeGenList.setEnabledFiltered(gen.type(),e.enabled); },e.variant);
  }
}

void OutputList::disableAllBut(OutputType o)
{
  //printf("disableAllBut(%d)\n",o);
  for (auto &e : m_outputGenList)
  {
    std::visit([&](auto &&gen) { if (gen.type()!=o) e.setEnabled(false); }, e.variant);
  }
  syncEnabled();
}

void OutputList::enableAll()
{
  //printf("enableAll()\n");
  for (auto &e : m_outputGenList)
  {
    e.setEnabled(true);
  }
  syncEnabled();
}

void OutputList::disableAll()
{
  //printf("enableAll()\n");
  for (auto &e : m_outputGenList)
  {
    e.setEnabled(false);
  }
  syncEnabled();
}

void OutputList::disable(OutputType o)
{
  //printf("disable(%d)\n",o);
  for (auto &e : m_outputGenList)
  {
    std::visit([&](auto &&gen) { if (gen.type()==o) e.setEnabled(false); }, e.variant);
  }
  syncEnabled();
}

void OutputList::enable(OutputType o)
{
  //printf("enable(%d)\n",o);
  for (auto &e : m_outputGenList)
  {
    std::visit([&](auto &&gen) { if (gen.type()==o) e.setEnabled(true); }, e.variant);
  }
  syncEnabled();
}

bool OutputList::isEnabled(OutputType o)
{
  bool found   = false;
  bool enabled = false;
  for (const auto &e : m_outputGenList)
  {
    std::visit([&](auto &&gen) {
        if (gen.type()==o) { enabled = e.enabled; found=true; }
     }, e.variant);
    if (found) return enabled;
  }
  return enabled;
}

void OutputList::pushGeneratorState()
{
  //printf("pushGeneratorState()\n");
  for (auto &e : m_outputGenList)
  {
    e.enabledStack.push(e.enabled);
  }
  syncEnabled();
}

void OutputList::popGeneratorState()
{
  //printf("popGeneratorState()\n");
  for (auto &e : m_outputGenList)
  {
    if (!e.enabledStack.empty())
    {
      e.enabled = e.enabledStack.top();
      e.enabledStack.pop();
    }
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
  if (docStr.isEmpty()) return;

  auto count=std::count_if(m_outputGenList.begin(),m_outputGenList.end(),
                           [](const auto &e) { return e.enabled; });
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
    if (ast) writeDoc(ast.get(),ctx,md);
  }
}

void OutputList::startFile(const QCString &name,const QCString &manName,const QCString &title, int hierarchyLevel)
{
  newId();
  m_codeGenList.setId(m_id);
  foreach<OutputGenIntf::startFile>(name,manName,title,m_id,hierarchyLevel);
}

void OutputList::parseText(const QCString &textStr)
{

  auto count=std::count_if(m_outputGenList.begin(),m_outputGenList.end(),
                           [](const auto &e) { return e.enabled; });

  if (count>0)
  {
    // we want to validate irrespective of the number of output formats
    // specified as:
    // - when only XML format there should be warnings as well (XML has its own write routines)
    // - no formats there should be warnings as well
    auto parser { createDocParser() };
    auto ast { validatingParseText(*parser.get(), textStr) };

    if (ast) writeDoc(ast.get(),0,0);
  }
}

//--------------------------------------------------------------------------
