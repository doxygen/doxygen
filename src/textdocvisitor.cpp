/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

//-------------------------------------------------------------------------

void TextDocVisitor::visit(DocSymbol *s)
{
  switch(s->symbol())
  {
    case DocSymbol::BSlash:  m_t << "\\"; break;
    case DocSymbol::At:      m_t << "@"; break;
    case DocSymbol::Less:    m_t << "&lt;"; break;
    case DocSymbol::Greater: m_t << "&gt;"; break;
    case DocSymbol::Amp:     m_t << "&amp;"; break;
    case DocSymbol::Dollar:  m_t << "$"; break;
    case DocSymbol::Hash:    m_t << "#"; break;
    case DocSymbol::Percent: m_t << "%"; break;
    case DocSymbol::Copy:    m_t << "&copy;"; break;
    case DocSymbol::Tm:      m_t << "&tm;"; break;
    case DocSymbol::Reg:     m_t << "&reg;"; break;
    case DocSymbol::Apos:    m_t << "'"; break;
    case DocSymbol::Quot:    m_t << "\""; break;
    case DocSymbol::Lsquo:   m_t << "&lsquo;"; break;
    case DocSymbol::Rsquo:   m_t << "&rsquo;"; break;
    case DocSymbol::Ldquo:   m_t << "&ldquo;"; break;
    case DocSymbol::Rdquo:   m_t << "&rdquo;"; break;
    case DocSymbol::Ndash:   m_t << "&ndash;"; break;
    case DocSymbol::Mdash:   m_t << "&mdash;"; break;
    case DocSymbol::Uml:     m_t << "&" << s->letter() << "uml;"; break;
    case DocSymbol::Acute:   m_t << "&" << s->letter() << "acute;"; break;
    case DocSymbol::Grave:   m_t << "&" << s->letter() << "grave;"; break;
    case DocSymbol::Circ:    m_t << "&" << s->letter() << "circ;"; break;
    case DocSymbol::Slash:   m_t << "&" << s->letter() << "slash;"; break;
    case DocSymbol::Tilde:   m_t << "&" << s->letter() << "tilde;"; break;
    case DocSymbol::Szlig:   m_t << "&szlig;"; break;
    case DocSymbol::Cedil:   m_t << "&" << s->letter() << "cedil;"; break;
    case DocSymbol::Ring:    m_t << "&" << s->letter() << "ring;"; break;
    case DocSymbol::Nbsp:    m_t << "&nbsp;"; break;
    default:
                             err("Error: unknown symbol found\n");
  }
}


void TextDocVisitor::filter(const char *str)
{ 
  if (str==0) return;
  const char *p=str;
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '\n': m_t << " "; break;
      default:   m_t << c;
    }
  }
}

