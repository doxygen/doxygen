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

#ifndef _CMDMAPPER_H
#define _CMDMAPPER_H

#include <qdict.h>

struct CommandMap
{
  const char *cmdName;
  int cmdId;
};

const int SIMPLESECT_BIT = 0x1000;

enum CommandType
{
  CMD_UNKNOWN      = 0,
  CMD_ADDINDEX     = 1,   
  CMD_ANCHOR       = 2,  
  CMD_ATTENTION    = 3 | SIMPLESECT_BIT,
  CMD_AUTHOR       = 4 | SIMPLESECT_BIT,
  CMD_BOLD         = 5,
  CMD_BUG          = 6 | SIMPLESECT_BIT, 
  CMD_CODE         = 7,
  CMD_COPYDOC      = 8,      
  CMD_DATE         = 9 | SIMPLESECT_BIT,
  CMD_DEPRECATED   = 10 | SIMPLESECT_BIT,
  CMD_DONTINCLUDE  = 11,   
  CMD_DOTFILE      = 12,      
  CMD_EMPHASIS     = 13,
  CMD_ENDCODE      = 14,      
  CMD_ENDHTMLONLY  = 15,  
  CMD_ENDLATEXONLY = 16, 
  CMD_ENDLINK      = 17,      
  CMD_ENDVERBATIM  = 18, 
  CMD_EXCEPTION    = 19 | SIMPLESECT_BIT,
  CMD_HTMLINCLUDE  = 20,
  CMD_HTMLONLY     = 21,
  CMD_IMAGE        = 22,           
  CMD_INCLUDE      = 23,
  CMD_INTERNAL     = 24,         
  CMD_INVARIANT    = 25 | SIMPLESECT_BIT ,
  CMD_LATEXONLY    = 26,
  CMD_LI           = 27,
  CMD_LINE         = 28,
  CMD_LINK         = 29,             
  CMD_NOTE         = 30 | SIMPLESECT_BIT ,
  CMD_PAR          = 31 | SIMPLESECT_BIT ,
  CMD_PARAM        = 32 | SIMPLESECT_BIT,
  CMD_POST         = 33 | SIMPLESECT_BIT,
  CMD_PRE          = 34 | SIMPLESECT_BIT ,
  CMD_REF          = 35,
  CMD_SECREFITEM   = 36,
  CMD_REMARK       = 37 | SIMPLESECT_BIT ,
  CMD_RETURN       = 38 | SIMPLESECT_BIT ,
  CMD_RETVAL       = 39 | SIMPLESECT_BIT,
  CMD_SA           = 40 | SIMPLESECT_BIT ,
  CMD_SECTION      = 41,
  CMD_SUBSECTION   = 42,
  CMD_SUBSUBSECTION= 43,
  CMD_PARAGRAPH    = 44,
  CMD_SINCE        = 45 | SIMPLESECT_BIT,
  CMD_SKIP         = 46,
  CMD_SKIPLINE     = 47,
  CMD_STARTCODE    = 48,
  CMD_JAVALINK     = 49,
  CMD_TEST         = 50 | SIMPLESECT_BIT,
  CMD_TODO         = 51 | SIMPLESECT_BIT,
  CMD_UNTIL        = 52,
  CMD_VERBATIM     = 53,
  CMD_VERBINCLUDE  = 54,
  CMD_VERSION      = 55 | SIMPLESECT_BIT,
  CMD_WARNING      = 56 | SIMPLESECT_BIT ,
  CMD_BSLASH       = 57,
  CMD_AT           = 58,
  CMD_LESS         = 59,
  CMD_GREATER      = 60,
  CMD_AMP          = 61,
  CMD_DOLLAR       = 62,
  CMD_HASH         = 63,
  CMD_PERCENT      = 64, 
  CMD_LINEBREAK    = 65,
  CMD_FORMULA      = 66,
  CMD_SECREFLIST   = 67,
  CMD_ENDSECREFLIST= 68,
  CMD_LANGSWITCH   = 69,
  CMD_AUTHORS      = 70 | SIMPLESECT_BIT,
  CMD_INTERNALREF  = 71
};

enum HtmlTagType
{
  HTML_UNKNOWN  = 0,
  HTML_CENTER   = 1,
  HTML_TABLE    = 2,
  HTML_CAPTION  = 3,
  HTML_SMALL    = 4,
  HTML_CODE     = 5,
  HTML_IMG      = 6,
  HTML_PRE      = 7,
  HTML_SUB      = 8,
  HTML_SUP      = 9,
  HTML_TR       = 10,
  HTML_TD       = 11,
  HTML_TH       = 12,
  HTML_OL       = 13,
  HTML_UL       = 14,
  HTML_LI       = 15,
  HTML_EMPHASIS = 16,
  HTML_HR       = 17,
  HTML_DL       = 18,
  HTML_DT       = 19,
  HTML_DD       = 20,
  HTML_BR       = 21,
  HTML_A        = 22,
  HTML_BOLD     = 23,
  HTML_P        = 24,
  HTML_H1       = 25,
  HTML_H2       = 26,
  HTML_H3       = 27,
  HTML_H4       = 28,
  HTML_H5       = 29,
  HTML_H6       = 30
};

class CmdMapper
{
  public:
    static int map(const char *name);
    static void freeInstance();

  private:
    static CmdMapper *instance();
    CmdMapper();
    int find(const char *name);
    QDict<int> m_map;
    static CmdMapper *m_instance;
};

class HtmlTagMapper
{
  public:
    static int map(const char *name);
    static void freeInstance();

  private:
    static HtmlTagMapper *instance();
    HtmlTagMapper();
    int find(const char *name);
    QDict<int> m_map;
    static HtmlTagMapper *m_instance;
};


#endif
