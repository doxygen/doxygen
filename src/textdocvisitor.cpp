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

#include <qdir.h>
#include "textdocvisitor.h"
#include "message.h"
#include "util.h"
#include "htmlentity.h"

//-------------------------------------------------------------------------

void TextDocVisitor::visit(DocSymbol *s)
{
  const char *res = HtmlEntityMapper::instance()->html(s->symbol());
  if (res)
  {
    m_t << res;
  }
  else
  {
    err("text: non supported HTML-entity found: %s\n",HtmlEntityMapper::instance()->html(s->symbol(),TRUE));
  }
}


void TextDocVisitor::filter(const char *str)
{ 
  if (str==0) return;
  //printf("TextDocVisitor::filter(%s)\n",str);
  const char *p=str;
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '\n':  m_t << " ";      break;
      case '"':   m_t << "&quot;"; break;
      case '\'':  m_t << "&#39;";  break;
      case '<':   m_t << "&lt;";   break;
      case '>':   m_t << "&gt;";   break;
      case '&':   m_t << "&amp;";  break;
      default:    m_t << c;
    }
  }
}

