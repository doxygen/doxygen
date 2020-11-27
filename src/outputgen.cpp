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

#include <stdexcept>

#include <stdlib.h>

#include <qfile.h>

#include "doxygen.h"
#include "outputgen.h"
#include "message.h"
#include "portable.h"

OutputGenerator::OutputGenerator(const char *dir) : m_dir(dir)
{
  //printf("OutputGenerator::OutputGenerator()\n");
}

OutputGenerator::~OutputGenerator()
{
  //printf("OutputGenerator::~OutputGenerator()\n");
}

OutputGenerator::OutputGenerator(const OutputGenerator &og)
{
  m_dir = og.m_dir;
  // we don't copy the other fields.
  // after copying startPlainFile() should be called
  if (og.t.device()!=nullptr)
  {
    throw std::runtime_error("OutputGenerator copy constructor called while a file is processing");
  }
}

OutputGenerator &OutputGenerator::operator=(const OutputGenerator &og)
{
  m_dir = og.m_dir;
  // we don't copy the other fields.
  // after assignment startPlainFile() should be called
  if (og.t.device()!=nullptr)
  {
    throw std::runtime_error("OutputGenerator assignment operator called while a file is processing");
  }
  return *this;
}

void OutputGenerator::startPlainFile(const char *name)
{
  //printf("startPlainFile(%s)\n",name);
  m_fileName=m_dir+"/"+name;
  m_file.setName(m_fileName);
  if (!m_file.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",m_fileName.data());
  }
  t.setDevice(&m_file);
}

void OutputGenerator::endPlainFile()
{
  t.unsetDevice();
  m_file.close();
  m_fileName.resize(0);
}

QCString OutputGenerator::dir() const
{
  return m_dir;
}

QCString OutputGenerator::fileName() const
{
  return m_fileName;
}

void OutputGenerator::pushGeneratorState()
{
  m_genStack.push(isEnabled());
  //printf("%p:pushGeneratorState(%d) enabled=%d\n",this,genStack->count(),isEnabled());
}

void OutputGenerator::popGeneratorState()
{
  //printf("%p:popGeneratorState(%d) enabled=%d\n",this,genStack->count(),isEnabled());
  if (!m_genStack.empty())
  {
    bool lb = m_genStack.top();
    m_genStack.pop();
    if (lb) enable(); else disable();
  }
}

void OutputGenerator::enable()
{
  if (!m_genStack.empty())
  {
    m_active=m_genStack.top();
  }
  else
  {
    m_active=true;
  }
}

void OutputGenerator::disable()
{
  m_active=false;
}

void OutputGenerator::enableIf(OutputGenerator::OutputType o)
{
  if (o==type()) enable();
}

void OutputGenerator::disableIf(OutputGenerator::OutputType o)
{
  if (o==type()) disable();
}

void OutputGenerator::disableIfNot(OutputGenerator::OutputType o)
{
  if (o!=type()) disable();
}

bool OutputGenerator::isEnabled(OutputGenerator::OutputType o)
{
  return (o==type() && m_active);
}

OutputGenerator *OutputGenerator::get(OutputGenerator::OutputType o)
{
  return (o==type()) ? this : 0;
}

