/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#ifndef CMDMAPPER_H
#define CMDMAPPER_H

#include <unordered_map>
#include <string>
#include <memory>
#include "qcstring.h"

template<typename T>
using CommandMap = std::unordered_map< std::string, T >;


enum class CommandType
{
  SIMPLESECT_BIT = 0x1000,

  UNKNOWN          = 0,
  CMD_ADDINDEX     = 1,
  CMD_AMP          = 2,
  CMD_ANCHOR       = 3,
  CMD_AT           = 4,
  CMD_ATTENTION    = 5  | SIMPLESECT_BIT,
  CMD_AUTHOR       = 6  | SIMPLESECT_BIT,
  CMD_AUTHORS      = 7  | SIMPLESECT_BIT,
  CMD_BOLD         = 8,
  CMD_BSLASH       = 9,
  CMD_CODE         = 10,
  CMD_COPYDOC      = 11,
  CMD_DATE         = 12 | SIMPLESECT_BIT,
  CMD_DOLLAR       = 13,
  CMD_DONTINCLUDE  = 14,
  CMD_DOTFILE      = 15,
  CMD_EMPHASIS     = 16,
  CMD_ENDCODE      = 17,
  CMD_ENDHTMLONLY  = 18,
  CMD_ENDLATEXONLY = 19,
  CMD_ENDLINK      = 20,
  CMD_ENDSECREFLIST= 21,
  CMD_ENDVERBATIM  = 22,
  CMD_ENDXMLONLY   = 23,
  CMD_EXCEPTION    = 24 | SIMPLESECT_BIT,
  CMD_FORMULA      = 25,
  CMD_GREATER      = 26,
  CMD_HASH         = 27,
  CMD_HTMLINCLUDE  = 28,
  CMD_HTMLONLY     = 29,
  CMD_IMAGE        = 30,
  CMD_INCLUDE      = 31,
  CMD_INTERNAL     = 32,
  CMD_INTERNALREF  = 33,
  CMD_INVARIANT    = 34 | SIMPLESECT_BIT ,
  CMD_LATEXONLY    = 35,
  CMD_LESS         = 36,
  CMD_LI           = 37,
  CMD_LINE         = 38,
  CMD_LINEBREAK    = 39,
  CMD_LINK         = 40,
  CMD_NOTE         = 41 | SIMPLESECT_BIT,
  CMD_PAR          = 42 | SIMPLESECT_BIT,
  CMD_PARAM        = 43 | SIMPLESECT_BIT,
  CMD_PERCENT      = 44,
  CMD_POST         = 45 | SIMPLESECT_BIT,
  CMD_PRE          = 46 | SIMPLESECT_BIT,
  CMD_REF          = 47,
  CMD_SECREFITEM   = 48,
  CMD_REMARK       = 49 | SIMPLESECT_BIT ,
  CMD_RETURN       = 50 | SIMPLESECT_BIT ,
  CMD_RETVAL       = 51 | SIMPLESECT_BIT,
  CMD_SA           = 52 | SIMPLESECT_BIT ,
  CMD_SECREFLIST   = 53,
  CMD_SECTION      = 54,
  CMD_SUBPAGE      = 55,
  CMD_SUBSECTION   = 56,
  CMD_SUBSUBSECTION= 57,
  CMD_PARAGRAPH    = 58,
  CMD_SINCE        = 59 | SIMPLESECT_BIT,
  CMD_SKIP         = 60,
  CMD_SKIPLINE     = 61,
  CMD_STARTCODE    = 62,
  CMD_JAVALINK     = 63,
  CMD_UNTIL        = 64,
  CMD_VERBATIM     = 65,
  CMD_VERBINCLUDE  = 66,
  CMD_VERSION      = 67 | SIMPLESECT_BIT,
  CMD_WARNING      = 68 | SIMPLESECT_BIT,
  CMD_XREFITEM     = 69 | SIMPLESECT_BIT,
  CMD_XMLONLY      = 70,
  CMD_DOT          = 71,
  CMD_ENDDOT       = 72,
  CMD_MSC          = 73,
  CMD_ENDMSC       = 74,
  CMD_MANONLY      = 75,
  CMD_ENDMANONLY   = 76,
  CMD_INCWITHLINES = 77,
  CMD_INHERITDOC   = 78,
  CMD_TPARAM       = 79 | SIMPLESECT_BIT,
  CMD_COPYBRIEF    = 80,
  CMD_COPYDETAILS  = 81,
  CMD_QUOTE        = 82,
  CMD_MSCFILE      = 83,
  CMD_DCOLON       = 84,
  CMD_COPYRIGHT    = 85 | SIMPLESECT_BIT,
  CMD_CITE         = 86,
  CMD_SNIPPET      = 87,
  CMD_RTFONLY      = 88,
  CMD_ENDRTFONLY   = 89,
  CMD_PIPE         = 90,
  CMD_VHDLFLOW     = 91,
  CMD_DBONLY       = 92,
  CMD_ENDDBONLY    = 93,
  CMD_ENDINTERNAL  = 94,
  CMD_PARBLOCK     = 95,
  CMD_ENDPARBLOCK  = 96,
  CMD_DIAFILE      = 97,
  CMD_LATEXINCLUDE = 98,
  CMD_NDASH        = 99,
  CMD_MDASH        = 100,
  CMD_STARTUML     = 101,
  CMD_ENDUML       = 102,
  CMD_SETSCOPE     = 103,
  CMD_PUNT         = 104,
  CMD_PLUS         = 105,
  CMD_MINUS        = 106,
  CMD_INCLUDEDOC   = 107,
  CMD_SNIPPETDOC   = 108,
  CMD_SNIPWITHLINES= 109,
  CMD_EMOJI        = 110,
  CMD_EQUAL        = 111,
  CMD_RTFINCLUDE   = 112,
  CMD_DOCBOOKINCLUDE= 113,
  CMD_MANINCLUDE   = 114,
  CMD_XMLINCLUDE   = 115,
  CMD_ILINE        = 116,
  CMD_ILITERAL     = 117,
  CMD_ENDILITERAL  = 118,
  CMD_IFILE        = 119,
  CMD_SHOWDATE     = 120,
  CMD_ISTARTCODE   = 121,
  CMD_ENDICODE     = 122,
  CMD_IVERBATIM    = 123,
  CMD_ENDIVERBATIM = 124,
  CMD_IANCHOR      = 125,
  CMD_DOXYCONFIG   = 126,
  CMD_IMPORTANT    = 127 | SIMPLESECT_BIT,
  CMD_SUBPARAGRAPH = 128,
  CMD_SUBSUBPARAGRAPH = 129,
  CMD_IPREFIX      = 130,
  CMD_PLANTUMLFILE = 131
};

enum class HtmlTagType
{
  SIMPLESECT_BIT = 0x1000,

  UNKNOWN        = 0,
  HTML_CENTER    = 1,
  HTML_TABLE     = 2,
  HTML_CAPTION   = 3,
  HTML_SMALL     = 4,
  HTML_CODE      = 5,
  HTML_IMG       = 6,
  HTML_PRE       = 7,
  HTML_SUB       = 8,
  HTML_SUP       = 9,
  HTML_TR        = 10,
  HTML_TD        = 11,
  HTML_TH        = 12,
  HTML_OL        = 13,
  HTML_UL        = 14,
  HTML_LI        = 15,
  HTML_EMPHASIS  = 16,
  HTML_HR        = 17,
  HTML_DL        = 18,
  HTML_DT        = 19,
  HTML_DD        = 20,
  HTML_BR        = 21,
  HTML_A         = 22,
  HTML_BOLD      = 23,
  HTML_P         = 24,
  HTML_H1        = 25,
  HTML_H2        = 26,
  HTML_H3        = 27,
  HTML_H4        = 28,
  HTML_H5        = 29,
  HTML_H6        = 30,
  HTML_SPAN      = 31,
  HTML_DIV       = 32,
  HTML_BLOCKQUOTE= 33,
  HTML_STRIKE    = 34,
  HTML_UNDERLINE = 35,
  HTML_INS       = 36,
  HTML_DEL       = 37,
  HTML_S         = 38,
  HTML_DETAILS   = 39,
  HTML_CITE      = 40,
  HTML_THEAD     = 41,
  HTML_TBODY     = 42,
  HTML_TFOOT     = 43,

  XML_CmdMask    = 0x100,

  XML_C            = XML_CmdMask + 0,
  XML_CODE         = XML_CmdMask + 1,
  XML_DESCRIPTION  = XML_CmdMask + 2,
  XML_EXAMPLE      = XML_CmdMask + 3,
  XML_EXCEPTION    = XML_CmdMask + 4,
  XML_INCLUDE      = XML_CmdMask + 5,
  XML_ITEM         = XML_CmdMask + 6,
  XML_LIST         = XML_CmdMask + 7,
  XML_LISTHEADER   = XML_CmdMask + 8,
  XML_PARA         = XML_CmdMask + 9,
  XML_PARAM        = XML_CmdMask + 10,
  XML_PARAMREF     = XML_CmdMask + 11,
  XML_PERMISSION   = XML_CmdMask + 12,
  XML_REMARKS      = XML_CmdMask + 13,
  XML_RETURNS      = XML_CmdMask + 14,
  XML_SEE          = XML_CmdMask + 15,
  XML_SEEALSO      = XML_CmdMask + 16,
  XML_SUMMARY      = XML_CmdMask + 17,
  XML_TERM         = XML_CmdMask + 18,
  XML_TYPEPARAM    = XML_CmdMask + 19,
  XML_TYPEPARAMREF = XML_CmdMask + 20,
  XML_VALUE        = XML_CmdMask + 21,
  XML_INHERITDOC   = XML_CmdMask + 22
};


/** Class representing a mapping from command names to command IDs. */
template<typename T>
class Mapper
{
  public:
    T map(const QCString &n) const
    {
      if (n.isEmpty()) return T::UNKNOWN;
      QCString name = n;
      if (!m_cs) name=name.lower();
      auto it = m_map.find(name.str());
      return it!=m_map.end() ? it->second : T::UNKNOWN;
    }

    QCString find(const T n) const
    {
      for (const auto &[name,id] : m_map)
      {
        T curVal = id;
        // https://stackoverflow.com/a/15889501/1657886
        if (curVal == n || (curVal == (static_cast<T>(static_cast<int>(n) | static_cast<int>(T::SIMPLESECT_BIT))))) return name.c_str();
      }
      return QCString();
    }

    Mapper(const CommandMap<T> &cm,bool caseSensitive) : m_map(cm), m_cs(caseSensitive)
    {
    }

  private:
    const CommandMap<T> &m_map;
    bool m_cs;
};

/** Namespace for the doxygen and HTML command mappers. */
namespace Mappers
{
  extern const Mapper<CommandType> *cmdMapper;
  extern const Mapper<HtmlTagType> *htmlTagMapper;
}

#endif
