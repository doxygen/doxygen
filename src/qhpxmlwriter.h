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

#include <sstream>
#include "textstream.h"

class QhpXmlWriter
{
  public:
    QhpXmlWriter();
   ~QhpXmlWriter();

    void setIndentLevel(int level);
    void setCompressionEnabled(bool enabled);
    void insert(QhpXmlWriter const & source);
    void dumpTo(TextStream & file);
    void open(const QCString &elementName,
              const char * const attributes[] = 0);
    void openClose(const QCString &elementName,
                   const char * const attributes[] = 0);
    void openCloseContent(const QCString &elementName, const QCString &content);
    void close(const QCString &elementName);
    void declaration(const QCString &version, const QCString &encoding);

  private:
    void indent();
    void newLine();
    void openPureHelper(const QCString &elementName,
                        const char * const attributes[], bool close);
    void openPure(const QCString &elementName,
                  const char * const attributes[] = 0);
    void openClosePure(const QCString &elementName,
                       const char * const attributes[] = 0);
    void closePure(const QCString &elementName);

    TextStream m_backend;
    int  m_indentLevel;
    bool m_curLineIndented;
    bool m_compress;

};

#endif // QHPXMLWRITER_H
