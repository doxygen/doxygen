/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

// own header
#include "outputgen.h"

// other includes
#include "message.h"
#include "portable.h"

OutputGenerator::OutputGenerator(const DString &dir) : m_t(nullptr), m_dir(dir)
{
  //printf("OutputGenerator::OutputGenerator()\n");
}

void OutputGenerator::startPlainFile(const DString &name)
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

DString OutputGenerator::dir() const
{
  return m_dir;
}

DString OutputGenerator::fileName() const
{
  return m_fileName;
}

size_t updateColumnCount(const char *s,size_t col)
{
  if (s)
  {
    const int tabSize = Config_getInt(TAB_SIZE);
    char c;
    while ((c=*s++))
    {
      switch(c)
      {
        case '\t': col+=tabSize - (col%tabSize);
                   break;
        case '\n': col=0;
                   break;
        default:
                   col++;
                   if (c<0) // multi-byte character
                   {
                     int numBytes = getUTF8CharNumBytes(c);
                     for (int i=0;i<numBytes-1 && (c=*s++);i++) {} // skip over extra chars
                     if (c==0) return col; // end of string half way a multibyte char
                   }
                   break;
      }
    }
  }
  return col;
}



