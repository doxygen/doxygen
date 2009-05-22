/*
 * Copyright (C) 2008 by Sebastian Pipping.
 * Copyright (C) 2008 Dimitri van Heesch.
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
 * Sebastian Pipping <sebastian@pipping.org>
 */

#include "qhpxmlwriter.h"
#include "util.h"

#include <qfile.h>

QhpXmlWriter::QhpXmlWriter() 
    : m_out(&m_backend), m_indentLevel(0), 
      m_curLineIndented(false), m_compress(false)
{
}

QhpXmlWriter::~QhpXmlWriter()
{
}

void QhpXmlWriter::setIndentLevel(int level)
{
  m_indentLevel = level;
}

void QhpXmlWriter::setCompressionEnabled(bool enabled)
{
  m_compress = enabled;
}

void QhpXmlWriter::insert(QhpXmlWriter const & source)
{
  m_out << source.m_backend.data();
}

void QhpXmlWriter::dumpTo(QFile & file)
{
  file.writeBlock(m_backend.data(), m_backend.length());
}

void QhpXmlWriter::open(char const * elementName,
    char const * const * attributes)
{
  indent();
  openPure(elementName, attributes);
  newLine();
  m_indentLevel++;
}

void QhpXmlWriter::openClose(char const * elementName,
    char const * const * attributes)
{
  indent();
  openClosePure(elementName, attributes);
  newLine();
}

void QhpXmlWriter::openCloseContent(char const * elementName,
    char const * content)
{
  indent();
  openPure(elementName);
  m_out << convertToXML(content);
  closePure(elementName);
  newLine();
}

void QhpXmlWriter::close(char const * elementName)
{
  m_indentLevel--;
  indent();
  closePure(elementName);
  newLine();
}

void QhpXmlWriter::indent()
{
  if (m_curLineIndented)
  {
    return;
  }
  for (int i = 0; i < m_indentLevel; i++) 
  {
    m_out << "  ";
  }
  m_curLineIndented = true;
}

void QhpXmlWriter::newLine()
{
  if (!m_compress)
  {
    m_out << "\n";
    m_curLineIndented = false;
  }
}

void QhpXmlWriter::openPureHelper(char const * elementName,
                                  char const * const * attributes, bool close)
{
  m_out << "<" << elementName;
  if (attributes)
  {
    for (char const * const * walker = attributes;
        walker[0]; walker += 2)
    {
      char const * const key = walker[0];
      char const * const value = walker[1];
      if (!value)
      {
        continue;  
      }
      m_out << " " << key << "=\"" << convertToXML(value) << "\"";
    }
  }

  if (close)
  {
    m_out << " /";
  }
  m_out << ">";
}

void QhpXmlWriter::openPure(char const * elementName,
                            char const * const * attributes)
{
  openPureHelper(elementName, attributes, false);
}

void QhpXmlWriter::openClosePure(char const * elementName,
                                 char const * const * attributes)
{
  openPureHelper(elementName, attributes, true);
}

void QhpXmlWriter::closePure(char const * elementName)
{
  m_out << "</" << elementName << ">";
}

