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

#include <stdlib.h>

#include <qfile.h>

#include "outputgen.h"
#include "message.h"
#include "portable.h"

OutputGenerator::OutputGenerator()
{
  //printf("OutputGenerator::OutputGenerator()\n");
  m_file=0;
  m_active=TRUE;
  m_genStack = new QStack<bool>;
  m_genStack->setAutoDelete(TRUE);
}

OutputGenerator::~OutputGenerator()
{
  //printf("OutputGenerator::~OutputGenerator()\n");
  delete m_file;
  delete m_genStack;
}

void OutputGenerator::startPlainFile(const char *name)
{
  //printf("startPlainFile(%s)\n",name);
  m_fileName=m_dir+"/"+name;
  m_file = new QFile(m_fileName);
  if (!m_file->open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",m_fileName.data());
  }
  t.setDevice(m_file);
}

void OutputGenerator::endPlainFile()
{
  t.unsetDevice();
  delete m_file;
  m_file=0;
  m_fileName.resize(0);
}

void OutputGenerator::pushGeneratorState()
{
  m_genStack->push(new bool(isEnabled()));
  //printf("%p:pushGeneratorState(%d) enabled=%d\n",this,genStack->count(),isEnabled());
}

void OutputGenerator::popGeneratorState()
{
  //printf("%p:popGeneratorState(%d) enabled=%d\n",this,genStack->count(),isEnabled());
  bool *lb = m_genStack->pop();
  ASSERT(lb!=0);
  if (lb==0) return; // for some robustness against superfluous \endhtmlonly commands.
  if (*lb) enable(); else disable();
  delete lb;
}

