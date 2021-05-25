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

#include <functional>

#include "qcstring.h"
#include "parserintf.h"
#include "growbuf.h"

class Entry;

/** processes string \a s and converts markdown into doxygen/html commands. */
//QCString processMarkdown(const QCString &fileName,const int lineNr,Entry *e,const QCString &s);
QCString markdownFileNameToId(const QCString &fileName);

/// Helper class to process markdown formatted text
class Markdown
{
  public:
    Markdown(const QCString &fileName,int lineNr,int indentLevel=0);
    QCString process(const QCString &input, int &startNewlines, bool fromParseInput = false);
    QCString extractPageTitle(QCString &docs,QCString &id,int &prepend);
    void setIndentLevel(int level) { m_indentLevel = level; }

  private:
    QCString detab(const QCString &s,int &refIndent);
    QCString processQuotations(const QCString &s,int refIndent);
    QCString processBlocks(const QCString &s,int indent);
    QCString isBlockCommand(const char *data,int offset,int size);
    void findEndOfLine(const char *data,int size,int &pi,int&i,int &end);
    int processHtmlTagWrite(const char *data,int offset,int size,bool doWrite);
    int processHtmlTag(const char *data,int offset,int size);
    int processEmphasis(const char *data,int offset,int size);
    int processEmphasis1(const char *data, int size, char c);
    int processEmphasis2(const char *data, int size, char c);
    int processEmphasis3(const char *data, int size, char c);
    int processNmdash(const char *data,int off,int size);
    int processQuoted(const char *data,int,int size);
    int processCodeSpan(const char *data, int /*offset*/, int size);
    void addStrEscapeUtf8Nbsp(const char *s,int len);
    int processSpecialCommand(const char *data, int offset, int size);
    int processLink(const char *data,int,int size);
    int findEmphasisChar(const char *data, int size, char c, int c_size);
    void processInline(const char *data,int size);
    void writeMarkdownImage(const char *fmt, bool explicitTitle,
                            const QCString &title, const QCString &content,
                            const QCString &link, const FileDef *fd);
    int isHeaderline(const char *data, int size, bool allowAdjustLevel);
    int isAtxHeader(const char *data,int size,
                       QCString &header,QCString &id,bool allowAdjustLevel);
    void writeOneLineHeaderOrRuler(const char *data,int size);
    void writeFencedCodeBlock(const char *data,const char *lng,
                int blockStart,int blockEnd);
    int writeBlockQuote(const char *data,int size);
    int writeCodeBlock(const char *data,int size,int refIndent);
    int writeTableBlock(const char *data,int size);

  private:
    struct LinkRef
    {
      LinkRef(const QCString &l,const QCString &t) : link(l), title(t) {}
      QCString link;
      QCString title;
    };
    using Action_t = std::function<int(const char *,int,int)>;

    std::unordered_map<std::string,LinkRef> m_linkRefs;
    QCString       m_fileName;
    int            m_lineNr = 0;
    int            m_indentLevel=0;  // 0 is outside markdown, -1=page level
    GrowBuf        m_out;
    Markdown::Action_t m_actions[256];
};


class MarkdownOutlineParser : public OutlineParserInterface
{
  public:
    MarkdownOutlineParser();
    virtual ~MarkdownOutlineParser();
    void parseInput(const QCString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser);
    bool needsPreprocessing(const QCString &) const { return FALSE; }
    void parsePrototype(const QCString &text);
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


#endif
