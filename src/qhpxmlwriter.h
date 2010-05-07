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

#ifndef QHPXMLWRITER_H
#define QHPXMLWRITER_H

#include <qstring.h>
#include "ftextstream.h"

class QFile;

class QhpXmlWriter
{
  public:
    QhpXmlWriter();
   ~QhpXmlWriter();

    void setIndentLevel(int level);
    void setCompressionEnabled(bool enabled);
    void insert(QhpXmlWriter const & source);
    void dumpTo(QFile & file);
    void open(char const * elementName,
              char const * const * attributes = 0);
    void openClose(char const * elementName,
                   char const * const * attributes = 0);
    void openCloseContent(char const * elementName, char const * content);
    void close(char const * elementName);

    static char * dupEscaped(const char * source);

  private:
    void indent();
    void newLine();
    void openPureHelper(char const * elementName,
                        char const * const * attributes, bool close);
    void openPure(char const * elementName,
                  char const * const * attributes = 0);
    void openClosePure(char const * elementName,
                       char const * const * attributes = 0);
    void closePure(char const * elementName);

    QGString m_backend;
    FTextStream m_out;
    int  m_indentLevel;
    bool m_curLineIndented;
    bool m_compress;

};

#endif // QHPXMLWRITER_H
