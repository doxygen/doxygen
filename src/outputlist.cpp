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
    e.intf->addCodeGen(m_codeGenList);
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
    m_codeGenList.setEnabledFiltered(e.intf->type(),e.enabled);
  }
}

void OutputList::disableAllBut(OutputType o)
{
  //printf("disableAllBut(%d)\n",o);
  for (auto &e : m_outputGenList)
  {
    if (e.intf->type()!=o) e.setEnabled(false);
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
    if (e.intf->type()==o) e.setEnabled(false);
  }
  syncEnabled();
}

void OutputList::enable(OutputType o)
{
  //printf("enable(%d)\n",o);
  for (auto &e : m_outputGenList)
  {
    if (e.intf->type()==o) e.setEnabled(true);
  }
  syncEnabled();
}

bool OutputList::isEnabled(OutputType o)
{
  for (const auto &e : m_outputGenList)
  {
    if (e.intf->type()==o) { return e.enabled; }
  }
  return false;
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
                  const QCString &docStr,const DocOptions &options)
{
  if (docStr.isEmpty()) return;

  auto count=std::count_if(m_outputGenList.begin(),m_outputGenList.end(),
                           [](const auto &e) { return e.enabled; });
  // we want to validate irrespective of the number of output formats
  // specified as:
  // - when only XML format there should be warnings as well (XML has its own write routines)
  // - no formats there should be warnings as well
  auto parser { createDocParser() };
  auto ast    { validatingParseDoc(*parser.get(),
                                   fileName,
                                   startLine,
                                   ctx,
                                   md,
                                   docStr,
                                   options)
               };
  if (ast && count>0) writeDoc(ast.get(),ctx,md,options.sectionLevel());
}

void OutputList::startFile(const QCString &name,bool isSource,const QCString &manName,const QCString &title, int hierarchyLevel)
{
  newId();
  m_codeGenList.setId(m_id);
  foreach(&OutputGenIntf::startFile,name,isSource,manName,title,m_id,hierarchyLevel);
}

void OutputList::parseText(const QCString &textStr)
{

  auto count=std::count_if(m_outputGenList.begin(),m_outputGenList.end(),
                           [](const auto &e) { return e.enabled; });

  // we want to validate irrespective of the number of output formats
  // specified as:
  // - when only XML format there should be warnings as well (XML has its own write routines)
  // - no formats there should be warnings as well
  auto parser { createDocParser() };
  auto ast { validatingParseText(*parser.get(), textStr) };

  if (ast && count>0) writeDoc(ast.get(),nullptr,nullptr);
}

//--------------------------------------------------------------------------

void OutputCodeRecorder::startNewLine(int lineNr)
{
  int orgSize = static_cast<int>(m_lineOffset.size());
  if (orgSize<lineNr)
  {
    m_lineOffset.resize(lineNr);
    for (int i=orgSize;i<lineNr;i++) // output lines can be skipped due to hidden comments so fill in the gap
    {
      //printf("%p: startCodeLine(%d) offset=%zu\n",(void*)this,i,m_calls.size());
      m_lineOffset[i]=m_calls.size();
    }
  }
}

void OutputCodeRecorder::codify(const QCString &s)
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol) { ol->codify(s); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::startSpecialComment()
{
  m_insideSpecialComment=true;
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol) { ol->startSpecialComment(); },
                       true
                      );
}

void OutputCodeRecorder::endSpecialComment()
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol) { ol->endSpecialComment(); },
                       true
                      );
  m_insideSpecialComment=false;
}

void OutputCodeRecorder::writeCodeLink(CodeSymbolType type,
                   const QCString &ref,const QCString &file,
                   const QCString &anchor,const QCString &name,
                   const QCString &tooltip)
{
  m_calls.emplace_back([](){ return true; },
                       [=](OutputCodeList *ol) { ol->writeCodeLink(type,ref,file,anchor,name,tooltip); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                     int lineNumber, bool writeLineAnchor)
{
  startNewLine(lineNumber);
  m_calls.emplace_back([&]() { return m_showLineNumbers; },
                       [=](OutputCodeList *ol) { ol->writeLineNumber(ref,file,anchor,lineNumber,writeLineAnchor); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                  const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
{
  m_calls.emplace_back([](){ return true; },
                       [=](OutputCodeList *ol) { ol->writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::startCodeLine(int lineNr)
{
  startNewLine(lineNr);
  m_calls.emplace_back([](){ return true; },
                       [=](OutputCodeList *ol) { ol->startCodeLine(lineNr); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::endCodeLine()
{
  m_calls.emplace_back([](){ return true; },
                       [=](OutputCodeList *ol) { ol->endCodeLine(); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::startFontClass(const QCString &c)
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol) { ol->startFontClass(c); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::endFontClass()
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol){ ol->endFontClass(); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::writeCodeAnchor(const QCString &name)
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol){ ol->writeCodeAnchor(name); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::startCodeFragment(const QCString &style)
{
}

void OutputCodeRecorder::endCodeFragment(const QCString &style)
{
}

void OutputCodeRecorder::startFold(int lineNr,const QCString &startMarker,const QCString &endMarker)
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol) { ol->startFold(lineNr,startMarker,endMarker); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::endFold()
{
  m_calls.emplace_back([]() { return true; },
                       [=](OutputCodeList *ol) { ol->endFold(); },
                       m_insideSpecialComment
                      );
}

void OutputCodeRecorder::replay(OutputCodeList &ol,int startLine,int endLine,bool showLineNumbers,bool
    stripCodeComments,size_t stripIndentAmount)
{
  size_t startIndex = startLine>0 && startLine<=(int)m_lineOffset.size() ? m_lineOffset[startLine-1] : 0;
  size_t endIndex   = endLine>0   && endLine  <=(int)m_lineOffset.size() ? m_lineOffset[  endLine-1] : m_calls.size();
  //printf("startIndex=%zu endIndex=%zu\n",startIndex,endIndex);

  // configure run time properties of the rendering
  ol.stripCodeComments(stripCodeComments);
  ol.setStripIndentAmount(stripIndentAmount);
  m_showLineNumbers = showLineNumbers;

  bool insideSpecialComment = false;
  // in case the start of the special comment marker is outside of the fragment, start it here
  if (startIndex<endIndex && m_calls[startIndex].insideSpecialComment)
  {
    ol.startSpecialComment();
    insideSpecialComment = true;
  }

  // render the requested fragment of the pre-recorded output
  for (size_t i=startIndex; i<endIndex; i++)
  {
    if (m_calls[i].condition())
    {
      insideSpecialComment = m_calls[i].insideSpecialComment;
      m_calls[i].function(&ol);
    }
  }

  // if we end the fragment inside a special comment, make sure we end it,
  // and also the code line
  if (insideSpecialComment)
  {
    ol.endSpecialComment();
    ol.endCodeLine();
  }

  ol.stripCodeComments(false);
  ol.setStripIndentAmount(0);
}
