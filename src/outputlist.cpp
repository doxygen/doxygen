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

#include "outputlist.h"
#include "outputgen.h"
#include "config.h"
#include "message.h"
#include "definition.h"
#include "docparser.h"
#include "vhdldocgen.h"

OutputList::OutputList(bool)
{
  //printf("OutputList::OutputList()\n");
  m_outputs.setAutoDelete(TRUE);
}

OutputList::~OutputList()
{
  //printf("OutputList::~OutputList()\n");
}

void OutputList::add(const OutputGenerator *og)
{
  if (og) m_outputs.append(og);
}

void OutputList::disableAllBut(OutputGenerator::OutputType o)
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->disableIfNot(o);
  }
}

void OutputList::enableAll()
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->enable();
  }
}

void OutputList::disableAll()
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->disable();
  }
}

void OutputList::disable(OutputGenerator::OutputType o)
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->disableIf(o);
  }
}

void OutputList::enable(OutputGenerator::OutputType o)
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->enableIf(o);
  }
}

bool OutputList::isEnabled(OutputGenerator::OutputType o)
{
  bool result=FALSE;
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    result=result || og->isEnabled(o);
  }
  return result;
}

void OutputList::pushGeneratorState()
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->pushGeneratorState();
  }
}

void OutputList::popGeneratorState()
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    og->popGeneratorState();
  }
}

void OutputList::generateDoc(const char *fileName,int startLine,
                  const Definition *ctx,const MemberDef * md,
                  const QCString &docStr,bool indexWords,
                  bool isExample,const char *exampleName,
                  bool singleLine,bool linkFromIndex)
{
  int count=0;
  if (docStr.isEmpty()) return;

  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    if (og->isEnabled()) count++;
  }

  // we want to validate irrespective of the number of output formats
  // specified as:
  // - when only XML format there should be warnings as well (XML has its own write routines)
  // - no formats there should be warnings as well
  DocRoot *root=0;
  root = validatingParseDoc(fileName,startLine,
                            ctx,md,docStr,indexWords,isExample,exampleName,
                            singleLine,linkFromIndex);
  if (count>0) writeDoc(root,ctx,md);
  delete root;
}

void OutputList::writeDoc(DocRoot *root,const Definition *ctx,const MemberDef *md)
{
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    //printf("og->printDoc(extension=%s)\n",
    //    ctx?ctx->getDefFileExtension().data():"<null>");
    if (og->isEnabled()) og->writeDoc(root,ctx,md);
  }
  VhdlDocGen::setFlowMember(0);
}

void OutputList::parseText(const QCString &textStr)
{
  int count=0;
  QListIterator<OutputGenerator> it(m_outputs);
  OutputGenerator *og;
  for (it.toFirst();(og=it.current());++it)
  {
    if (og->isEnabled()) count++;
  }

  // we want to validate irrespective of the number of output formats
  // specified as:
  // - when only XML format there should be warnings as well (XML has its own write routines)
  // - no formats there should be warnings as well
  DocText *root = validatingParseText(textStr);

  if (count>0)
  {
    for (it.toFirst();(og=it.current());++it)
    {
      if (og->isEnabled()) og->writeDoc(root,0,0);
    }
  }

  delete root;
}

//--------------------------------------------------------------------------
