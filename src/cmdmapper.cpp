/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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

#include "cmdmapper.h"

CommandMap cmdMap[] =
{
  { "a",             CMD_EMPHASIS },
  { "addindex",      CMD_ADDINDEX },
  { "anchor",        CMD_ANCHOR },
  { "arg",           CMD_LI },
  { "attention",     CMD_ATTENTION },
  { "author",        CMD_AUTHOR },
  { "b",             CMD_BOLD },
  { "c",             CMD_CODE },
  { "code",          CMD_STARTCODE },
  { "copydoc",       CMD_COPYDOC },
  { "date",          CMD_DATE },
  { "dontinclude",   CMD_DONTINCLUDE },
  { "dotfile",       CMD_DOTFILE },
  { "e",             CMD_EMPHASIS },
  { "em",            CMD_EMPHASIS },
  { "endcode",       CMD_ENDCODE },
  { "endhtmlonly",   CMD_ENDHTMLONLY },
  { "endlatexonly",  CMD_ENDLATEXONLY },
  { "endlink",       CMD_ENDLINK },
  { "endsecreflist", CMD_ENDSECREFLIST },
  { "endverbatim",   CMD_ENDVERBATIM },
  { "endxmlonly",    CMD_ENDXMLONLY },
  { "exception",     CMD_EXCEPTION },
  { "form",          CMD_FORMULA },
  { "htmlinclude",   CMD_HTMLINCLUDE },
  { "htmlonly",      CMD_HTMLONLY },
  { "image",         CMD_IMAGE },
  { "include",       CMD_INCLUDE },
  { "internal",      CMD_INTERNAL },
  { "invariant",     CMD_INVARIANT },
  { "javalink",      CMD_JAVALINK },
  { "latexonly",     CMD_LATEXONLY },
  { "li",            CMD_LI },
  { "line",          CMD_LINE },
  { "link",          CMD_LINK },
  { "n",             CMD_LINEBREAK },
  { "note",          CMD_NOTE },
  { "p",             CMD_CODE },
  { "par",           CMD_PAR },
  { "param",         CMD_PARAM },
  { "post",          CMD_POST },
  { "pre",           CMD_PRE },
  { "ref",           CMD_REF },
  { "refitem",       CMD_SECREFITEM },
  { "remark",        CMD_REMARK },
  { "remarks",       CMD_REMARK },
  { "result",        CMD_RETURN },
  { "return",        CMD_RETURN },
  { "returns",       CMD_RETURN },
  { "retval",        CMD_RETVAL },
  { "sa",            CMD_SA },
  { "secreflist",    CMD_SECREFLIST },
  { "section",       CMD_SECTION },
  { "subsection",    CMD_SUBSECTION },
  { "subsubsection", CMD_SUBSUBSECTION },
  { "paragraph",     CMD_PARAGRAPH },
  { "see",           CMD_SA },
  { "since",         CMD_SINCE },
  { "skip",          CMD_SKIP },
  { "skipline",      CMD_SKIPLINE },
  { "xmlonly",       CMD_XMLONLY },
  { "xrefitem",      CMD_XREFITEM },
  { "throw",         CMD_EXCEPTION },
  { "until",         CMD_UNTIL },
  { "verbatim",      CMD_VERBATIM },
  { "verbinclude",   CMD_VERBINCLUDE },
  { "version",       CMD_VERSION },
  { "warning",       CMD_WARNING },
  { "author",        CMD_AUTHOR },
  { "authors",       CMD_AUTHORS },
  { "throws",        CMD_EXCEPTION },
  { "\\",            CMD_BSLASH },
  { "@",             CMD_AT },
  { "<",             CMD_LESS },
  { ">",             CMD_GREATER },
  { "&",             CMD_AMP },
  { "$",             CMD_DOLLAR },
  { "#",             CMD_HASH },
  { "%",             CMD_PERCENT },
  { "~",             CMD_LANGSWITCH },
  { "_internalref",  CMD_INTERNALREF },
  { "dot",           CMD_DOT },
  { "enddot",        CMD_ENDDOT },
  { 0,               0 }
};

//----------------------------------------------------------------------------

int CmdMapper::map(const char *name)
{
  return instance()->find(name);
}

void CmdMapper::freeInstance()
{
  delete m_instance; m_instance=0;
}

CmdMapper *CmdMapper::instance()
{
  if (m_instance==0) m_instance = new CmdMapper;
  return m_instance;
}

CmdMapper::CmdMapper() : m_map(89)
{
  m_map.setAutoDelete(TRUE);
  CommandMap *p = cmdMap;
  while (p->cmdName)
  {
    m_map.insert(p->cmdName,new int(p->cmdId));
    p++;
  }
}
int CmdMapper::find(const char *name) 
{
  int *result = m_map.find(name);
  if (result) return *result; else return CMD_UNKNOWN;
}

CmdMapper *CmdMapper::m_instance=0;

//----------------------------------------------------------------------------

CommandMap htmlTagMap[] =
{
  { "strong",     HTML_BOLD },
  { "center",     HTML_CENTER },
  { "table",      HTML_TABLE },
  { "caption",    HTML_CAPTION },
  { "small",      HTML_SMALL },
  { "code",       HTML_CODE },
  { "dfn",        HTML_CODE },
  { "var",        HTML_EMPHASIS },
  { "img",        HTML_IMG },
  { "pre",        HTML_PRE },
  { "sub",        HTML_SUB },
  { "sup",        HTML_SUP },
  { "tr",         HTML_TR },
  { "td",         HTML_TD },
  { "th",         HTML_TH },
  { "ol",         HTML_OL },
  { "ul",         HTML_UL },
  { "li",         HTML_LI },
  { "tt",         HTML_CODE },
  { "kbd",        HTML_CODE },
  { "em",         HTML_EMPHASIS },
  { "hr",         HTML_HR },
  { "dl",         HTML_DL },
  { "dt",         HTML_DT },
  { "dd",         HTML_DD },
  { "br",         HTML_BR },
  { "i",          HTML_EMPHASIS },
  { "a",          HTML_A },
  { "b",          HTML_BOLD },
  { "p",          HTML_P },
  { "h1",         HTML_H1 },
  { "h2",         HTML_H2 },
  { "h3",         HTML_H3 },
  { "h4",         HTML_H4 },
  { "h5",         HTML_H5 },
  { "h6",         HTML_H6 },
  { "span",       HTML_SPAN },
  { "div",        HTML_DIV },
  { 0,            0 }
};

//----------------------------------------------------------------------------

int HtmlTagMapper::map(const char *name)
{
  return instance()->find(name);
}

void HtmlTagMapper::freeInstance()
{
  delete m_instance; m_instance=0;
}

HtmlTagMapper *HtmlTagMapper::instance()
{
  if (m_instance==0) m_instance = new HtmlTagMapper;
  return m_instance;
}

HtmlTagMapper::HtmlTagMapper() : m_map(89)
{
  m_map.setAutoDelete(TRUE);
  CommandMap *p = htmlTagMap;
  while (p->cmdName)
  {
    m_map.insert(p->cmdName,new int(p->cmdId));
    p++;
  }
}
int HtmlTagMapper::find(const char *name) 
{
  int *result = m_map.find(name);
  if (result) return *result; else return HTML_UNKNOWN;
}

HtmlTagMapper *HtmlTagMapper::m_instance=0;

//----------------------------------------------------------------------------
