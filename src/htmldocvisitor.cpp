/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#include "htmldocvisitor.h"

void HtmlDocVisitor::filter(const char *str)
{ 
  if (str==0) return;
  const char *p=str;
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '<':  m_t << "&lt;"; break;
      case '>':  m_t << "&gt;"; break;
      case '&':  m_t << "&amp;"; break;
      default:   m_t << c;
    }
  }
}

void HtmlDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  QCString *dest;
  if (!ref.isEmpty()) // link to entity imported via tag file
  {
    m_t << "<a class=\"elRef\" ";
    m_t << "doxygen=\"" << ref << ":";
    if ((dest=Doxygen::tagDestinationDict[ref])) m_t << *dest << "/";
    m_t << "\" ";
  }
  else // local link
  {
    m_t << "<a class=\"el\" ";
  }
  m_t << "href=\"";
  if (!ref.isEmpty())
  {
    if ((dest=Doxygen::tagDestinationDict[ref])) m_t << *dest << "/";
  }
  if (!file.isEmpty()) m_t << file << Doxygen::htmlFileExtension;
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\">";
}

void HtmlDocVisitor::endLink()
{
  m_t << "</a>";
}

