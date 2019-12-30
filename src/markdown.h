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

/** Performs markdown processing for a comment block if markdown processing is enabled.
 *  @param[in] comment A string representing the actual comment block.
 *         Note that leading *'s should already be stripped from the comment block.
 *  @param[in] fileName The name of the file in which the comment is found.
 *         Mainly used for producing warnings.
 *  @param[in] lineNr The line number at which the comment block was found.
 *  @returns The processed comment block
 */
QCString processMarkdownForCommentBlock(const QCString &comment,
                                const QCString &fileName,
                                int lineNr);

class MarkdownOutlineParser : public OutlineParserInterface
{
  public:
    MarkdownOutlineParser();
    virtual ~MarkdownOutlineParser();
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char *fileName, 
                    const char *fileBuf, 
                    const std::shared_ptr<Entry> &root,
                    bool sameTranslationUnit,
                    QStrList &filesInSameTranslationUnit);
    bool needsPreprocessing(const QCString &) const { return FALSE; }
    void parsePrototype(const char *text);
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


#endif
