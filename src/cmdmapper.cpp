/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

/** Call representing a mapping from a command name to a command ID. */
struct CommandMap
{
  const char *cmdName;
  int cmdId;
};

CommandMap cmdMap[] =
{
  { "a",             CMD_EMPHASIS },
  { "addindex",      CMD_ADDINDEX },
  { "anchor",        CMD_ANCHOR },
  { "arg",           CMD_LI },
  { "attention",     CMD_ATTENTION },
  { "author",        CMD_AUTHOR },
  { "authors",       CMD_AUTHORS },
  { "b",             CMD_BOLD },
  { "c",             CMD_CODE },
  { "cite",          CMD_CITE },
  { "icode",         CMD_ISTARTCODE },
  { "code",          CMD_STARTCODE },
  { "copydoc",       CMD_COPYDOC },
  { "copybrief",     CMD_COPYBRIEF },
  { "copydetails",   CMD_COPYDETAILS },
  { "copyright",     CMD_COPYRIGHT },
  { "date",          CMD_DATE },
  { "showdate",      CMD_SHOWDATE },
  { "dontinclude",   CMD_DONTINCLUDE },
  { "dotfile",       CMD_DOTFILE },
  { "e",             CMD_EMPHASIS },
  { "em",            CMD_EMPHASIS },
  { "endicode",      CMD_ENDICODE },
  { "endcode",       CMD_ENDCODE },
  { "endhtmlonly",   CMD_ENDHTMLONLY },
  { "endlatexonly",  CMD_ENDLATEXONLY },
  { "endlink",       CMD_ENDLINK },
  { "endsecreflist", CMD_ENDSECREFLIST },
  { "endiverbatim",  CMD_ENDIVERBATIM },
  { "endverbatim",   CMD_ENDVERBATIM },
  { "endxmlonly",    CMD_ENDXMLONLY },
  { "exception",     CMD_EXCEPTION },
  { "_form",         CMD_FORMULA },
  { "htmlinclude",   CMD_HTMLINCLUDE },
  { "htmlonly",      CMD_HTMLONLY },
  { "image",         CMD_IMAGE },
  { "include",       CMD_INCLUDE },
  { "internal",      CMD_INTERNAL },
  { "invariant",     CMD_INVARIANT },
  { "javalink",      CMD_JAVALINK },
  { "javalinkplain", CMD_JAVALINK },
  { "latexinclude",  CMD_LATEXINCLUDE },
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
  { "snippet",       CMD_SNIPPET },
  { "snippetdoc",    CMD_SNIPPETDOC },
  { "snippetlineno", CMD_SNIPWITHLINES },
  { "subpage",       CMD_SUBPAGE },
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
  { "iverbatim",     CMD_IVERBATIM },
  { "verbatim",      CMD_VERBATIM },
  { "verbinclude",   CMD_VERBINCLUDE },
  { "version",       CMD_VERSION },
  { "warning",       CMD_WARNING },
  { "throws",        CMD_EXCEPTION },
  { "tparam",        CMD_TPARAM },
  { "\\",            CMD_BSLASH },
  { "@",             CMD_AT },
  { "<",             CMD_LESS },
  { "=",             CMD_EQUAL },
  { ">",             CMD_GREATER },
  { "&",             CMD_AMP },
  { "$",             CMD_DOLLAR },
  { "#",             CMD_HASH },
  { "%",             CMD_PERCENT },
  { "|",             CMD_PIPE },
  { ".",             CMD_PUNT },
  { "+",             CMD_PLUS },
  { "-",             CMD_MINUS },
  { "::",            CMD_DCOLON },
  { "\"",            CMD_QUOTE },
  { "_internalref",  CMD_INTERNALREF },
  { "dot",           CMD_DOT },
  { "msc",           CMD_MSC },
  { "startuml",      CMD_STARTUML },
  { "enddot",        CMD_ENDDOT },
  { "endmsc",        CMD_ENDMSC },
  { "enduml",        CMD_ENDUML },
  { "manonly",       CMD_MANONLY },
  { "endmanonly",    CMD_ENDMANONLY },
  { "includelineno", CMD_INCWITHLINES },
  { "includedoc",    CMD_INCLUDEDOC },
  { "inheritdoc",    CMD_INHERITDOC },
  { "mscfile",       CMD_MSCFILE },
  { "rtfonly",       CMD_RTFONLY },
  { "endrtfonly",    CMD_ENDRTFONLY },
  { "vhdlflow",      CMD_VHDLFLOW },
  { "docbookonly",   CMD_DBONLY },
  { "enddocbookonly",CMD_ENDDBONLY },
  { "endinternal",   CMD_ENDINTERNAL },
  { "parblock",      CMD_PARBLOCK },
  { "endparblock",   CMD_ENDPARBLOCK },
  { "diafile",       CMD_DIAFILE },
  { "--",            CMD_NDASH },
  { "---",           CMD_MDASH },
  { "_setscope",     CMD_SETSCOPE },
  { "emoji",         CMD_EMOJI },
  { "rtfinclude",    CMD_RTFINCLUDE },
  { "docbookinclude",CMD_DOCBOOKINCLUDE },
  { "maninclude",    CMD_MANINCLUDE },
  { "xmlinclude",    CMD_XMLINCLUDE },
  { "iline",         CMD_ILINE },
  { "ifile",         CMD_IFILE },
  { "iliteral",      CMD_ILITERAL },
  { "endiliteral",   CMD_ENDILITERAL },
  { 0,               0 },
};

//----------------------------------------------------------------------------

CommandMap htmlTagMap[] =
{
  { "strong",     HTML_BOLD },
  { "center",     HTML_CENTER },
  { "table",      HTML_TABLE },
  { "caption",    HTML_CAPTION },
  { "small",      HTML_SMALL },
  { "cite",       HTML_CITE },
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
  { "tt",         XML_C /*HTML_CODE*/ },
  { "kbd",        XML_C /*HTML_CODE*/ },
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
  { "blockquote", HTML_BLOCKQUOTE },
  { "strike",     HTML_STRIKE },
  { "s",          HTML_S },
  { "u",          HTML_UNDERLINE },
  { "ins",        HTML_INS },
  { "del",        HTML_DEL },
  { "details",    HTML_DETAILS },

  { "c",            XML_C },
  // { "code",       XML_CODE },  <= ambiguous <code> is also a HTML tag
  { "description",  XML_DESCRIPTION },
  { "example",      XML_EXAMPLE },
  { "exception",    XML_EXCEPTION },
  { "include",      XML_INCLUDE },
  { "item",         XML_ITEM },
  { "list",         XML_LIST }, // type="table|bullet|number"
  { "listheader",   XML_LISTHEADER },
  { "para",         XML_PARA },
  { "param",        XML_PARAM },
  { "paramref",     XML_PARAMREF },
  { "typeparam",    XML_TYPEPARAM },
  { "typeparamref", XML_TYPEPARAMREF },
  { "permission",   XML_PERMISSION },
  { "remarks",      XML_REMARKS },
  { "returns",      XML_RETURNS },
  { "see",          XML_SEE },
  { "seealso",      XML_SEEALSO },
  { "summary",      XML_SUMMARY },
  { "term",         XML_TERM },
  { "value",        XML_VALUE },
  { "inheritdoc",   XML_INHERITDOC },
  { 0,              0 }
};

//----------------------------------------------------------------------------

Mapper *Mappers::cmdMapper     = new Mapper(cmdMap,true);
Mapper *Mappers::htmlTagMapper = new Mapper(htmlTagMap,false);

int Mapper::map(const QCString &n)
{
  if (n.isEmpty()) return 0;
  QCString name = n;
  if (!m_cs) name=name.lower();
  auto it = m_map.find(name.str());
  return it!=m_map.end() ? it->second : 0;
}

QCString Mapper::find(const int n)
{
  for (const auto &kv : m_map)
  {
    int curVal = kv.second;
    if (curVal == n || (curVal == (n | SIMPLESECT_BIT))) return kv.first.c_str();
  }
  return QCString();
}

Mapper::Mapper(const CommandMap *cm,bool caseSensitive) : m_cs(caseSensitive)
{
  const CommandMap *p = cm;
  while (p->cmdName)
  {
    m_map.insert(std::make_pair(p->cmdName,p->cmdId));
    p++;
  }
}

void Mappers::freeMappers()
{
  delete cmdMapper;     cmdMapper     = 0;
  delete htmlTagMapper; htmlTagMapper = 0;
}


