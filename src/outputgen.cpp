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

void OutputGenerator::startPlainFile(const QCString &name)
{
  //printf("startPlainFile(%s)\n",qPrint(name));
  m_fileName=m_dir+"/"+name;
  m_file = Portable::fopen(m_fileName.data(),"wb");
  if (m_file==nullptr)
  {
    term("Could not open file {} for writing\n",m_fileName);
  }
  m_t.setFile(m_file);
}

void OutputGenerator::endPlainFile()
{
  m_t.flush();
  m_t.setStream(nullptr);
  Portable::fclose(m_file);
  m_fileName.clear();
}

QCString OutputGenerator::dir() const
{
  return m_dir;
}

QCString OutputGenerator::fileName() const
{
  return m_fileName;
}

