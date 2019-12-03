/******************************************************************************
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

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <qcstring.h>
#include "parserintf.h"

class Entry;

/** processes string \a s and converts markdown into doxygen/html commands. */
QCString processMarkdown(const QCString &fileName,const int lineNr,Entry *e,const QCString &s);
QCString markdownFileNameToId(const QCString &fileName);

class MarkdownOutlineParser : public OutlineParserInterface
{
  public:
    virtual ~MarkdownOutlineParser() {}
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char *fileName, 
                    const char *fileBuf, 
                    const std::unique_ptr<Entry> &root,
                    bool sameTranslationUnit,
                    QStrList &filesInSameTranslationUnit);
    bool needsPreprocessing(const QCString &) const { return FALSE; }
    void parsePrototype(const char *text);
};


#endif
