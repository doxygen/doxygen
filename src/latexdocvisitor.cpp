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

#include "latexdocvisitor.h"
#include "util.h"

void LatexDocVisitor::filter(const char *str)
{ 
  filterLatexString(m_t,str,FALSE,m_insidePre);
}

void LatexDocVisitor::startLink(const QCString &ref,const QCString &file,const QCString &anchor)
{
  if (ref.isEmpty() && Config_getBool("PDF_HYPERLINKS"))
  {
    m_t << "\\hyperlink{";
    if (!file.isEmpty()) m_t << file;
    if (!file.isEmpty() && !anchor.isEmpty()) m_t << "_";
    if (!anchor.isEmpty()) m_t << anchor;
    m_t << "}{";

  }
  else
  {
    m_t << "{\\bf ";
  }
}

void LatexDocVisitor::endLink()
{
  m_t << "}";
}

