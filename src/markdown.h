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

#include <memory>

#include "qcstring.h"
#include "parserintf.h"

class Entry;

/** processes string \a s and converts markdown into doxygen/html commands. */
//QCString processMarkdown(const QCString &fileName,const int lineNr,Entry *e,const QCString &s);
QCString markdownFileNameToId(const QCString &fileName);

/// Helper class to process markdown formatted text
class Markdown
{
  public:
    Markdown(const QCString &fileName,int lineNr,int indentLevel=0);
   ~Markdown();
    NON_COPYABLE(Markdown)
    QCString process(const QCString &input, int &startNewlines, bool fromParseInput = false);
    QCString extractPageTitle(QCString &docs, QCString &id, int &prepend, bool &isIdGenerated);
    void setIndentLevel(int level);

  private:
    struct Private;
    std::unique_ptr<Private> prv;
    using Action_t      = std::function<int(Private&,std::string_view,size_t)>;
    using ActionTable_t = std::array<Action_t,256>;
    static ActionTable_t fill_table();
    static ActionTable_t actions;
};

class MarkdownOutlineParser : public OutlineParserInterface
{
  public:
    MarkdownOutlineParser();
   ~MarkdownOutlineParser() override;
    NON_COPYABLE(MarkdownOutlineParser)
    void parseInput(const QCString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser) override;
    bool needsPreprocessing(const QCString &) const override { return FALSE; }
    void parsePrototype(const QCString &text) override;
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


#endif
