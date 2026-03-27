/******************************************************************************
 *
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

#include "textdocvisitor.h"
#include "message.h"
#include "util.h"
#include "htmlentity.h"
#include "cite.h"
#include "emoji.h"

//-------------------------------------------------------------------------

void TextDocVisitor::operator()(const DocSymbol &s)
{
  const char *res = HtmlEntityMapper::instance().html(s.symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("text: non supported HTML-entity found: {}\n",HtmlEntityMapper::instance().html(s.symbol(),TRUE));
  }
}

void TextDocVisitor::operator()(const DocEmoji &s)
{
  // the TextDocVisitor is only invoked for the JS part of the HTML output
  const char *res = EmojiEntityMapper::instance().unicode(s.index());
  if (res)
  {
    const char *p = res;
    while (*p)
    {
      switch(*p)
      {
        case '&': case '#':
          break;
        case 'x':
          m_t << "\\u{";
          break;
        case ';':
          m_t << "}";
          break;
        default:
          m_t << *p;
          break;
      }
      p++;
    }
  }
  else
  {
    filter(s.name());
  }
}

void TextDocVisitor::operator()(const DocCite &cite)
{
  if (!cite.file().isEmpty())
  {
    QCString anchor = cite.anchor();
    QCString anchorPrefix = CitationManager::instance().anchorPrefix();
    anchor = anchor.mid(anchorPrefix.length()); // strip prefix
    m_t << anchor;
  }
  else
  {
    filter(cite.target());
  }
}

void TextDocVisitor::filter(const QCString &str)
{
  if (str.isEmpty()) return;
  //printf("TextDocVisitor::filter(%s)\n",str);
  const char *p=str.data();
  while (*p)
  {
    char c = *p++;
    if (c=='\n') c=' ';
    else m_t << c;
  }
}

