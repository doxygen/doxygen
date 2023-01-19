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

#include "doxygen.h"
#include "outputgen.h"
#include "message.h"
#include "portable.h"

OutputGenerator::OutputGenerator(const QCString &dir) : m_t(nullptr), m_dir(dir)
{
  //printf("OutputGenerator::OutputGenerator()\n");
}

OutputGenerator::~OutputGenerator()
{
  //printf("OutputGenerator::~OutputGenerator()\n");
}

OutputGenerator::OutputGenerator(const OutputGenerator &og) : m_t(nullptr)
{
  m_dir = og.m_dir;
  // we don't copy the other fields.
  // after copying startPlainFile() should be called
  if (og.m_t.stream()!=nullptr)
  {
    throw std::runtime_error("OutputGenerator copy constructor called while a file is processing");
  }
}

OutputGenerator &OutputGenerator::operator=(const OutputGenerator &og)
{
  m_dir = og.m_dir;
  // we don't copy the other fields.
  // after assignment startPlainFile() should be called
  if (og.m_t.stream()!=nullptr)
  {
    throw std::runtime_error("OutputGenerator assignment operator called while a file is processing");
  }
  return *this;
}

void OutputGenerator::startPlainFile(const QCString &name)
{
  //printf("startPlainFile(%s)\n",qPrint(name));
  m_fileName=m_dir+"/"+name;
  m_file = Portable::fopen(m_fileName.data(),"wb");
  if (m_file==0)
  {
    term("Could not open file %s for writing\n",qPrint(m_fileName));
  }
  m_t.setFile(m_file);
}

void OutputGenerator::endPlainFile()
{
  m_t.flush();
  m_t.setStream(nullptr);
  Portable::fclose(m_file);
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

void OutputGenerator::setEnabled(bool e)
{
  if (e && !m_genStack.empty())
  {
    m_active=m_genStack.top();
  }
  else
  {
    m_active=e;
  }
}

bool OutputGenerator::isEnabled() const
{
  return m_active;
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
    setEnabled(lb);
  }
}

OutputGenerator *OutputGenerator::get(OutputType o)
{
  return (o==type()) ? this : 0;
}

