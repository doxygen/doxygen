/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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

#ifndef _DOCTOKENIZER_H
#define _DOCTOKENIZER_H

#include <qstring.h>
#include <qlist.h>
#include "htmlattrib.h"

class Definition;
class MemberGroup;

enum Tokens
{
  TK_WORD          = 1,
  TK_LNKWORD       = 2,
  TK_WHITESPACE    = 3,
  TK_LISTITEM      = 4,
  TK_ENDLIST       = 5,
  TK_COMMAND       = 6,
  TK_HTMLTAG       = 7,
  TK_SYMBOL        = 8,
  TK_NEWPARA       = 9,
  TK_RCSTAG        = 10,
  TK_URL           = 11,

  RetVal_OK             = 0x10000,
  RetVal_SimpleSec      = 0x10001,
  RetVal_ListItem       = 0x10002,
  RetVal_Section        = 0x10003,
  RetVal_Subsection     = 0x10004,
  RetVal_Subsubsection  = 0x10005,
  RetVal_Paragraph      = 0x10006,
  RetVal_EndList        = 0x10007,
  RetVal_EndPre         = 0x10008,
  RetVal_DescData       = 0x10009,
  RetVal_DescTitle      = 0x1000A,
  RetVal_EndDesc        = 0x1000B,
  RetVal_TableRow       = 0x1000C,
  RetVal_TableCell      = 0x1000D,
  RetVal_TableHCell     = 0x1000E,
  RetVal_EndTable       = 0x1000F,
  RetVal_Internal       = 0x10010,
  RetVal_SwitchLang     = 0x10011,
  RetVal_CloseXml       = 0x10012
};

struct TokenInfo
{
  // unknown token
  char unknownChar;
  
  // command token
  QString name;

  // command text (RCS tag)
  QString text;

  // comment blocks
  
  // list token info
  bool isEnumList;
  int indent;

  // sections
  QString sectionId;

  // simple section
  QString simpleSectName;

  // verbatim fragment
  QString verb;

  // xrefitem
  int id;

  // html tag
  HtmlAttribList attribs;
  bool endTag;
  bool emptyTag;

  // whitespace
  QString chars;

  // url
  bool isEMailAddr;

  // param attributes
  enum ParamDir { In=1, Out=2, InOut=3, Unspecified=0 };
  ParamDir paramDir;
};

// globals
extern TokenInfo *g_token;
extern int doctokenizerYYlineno;
extern FILE *doctokenizerYYin;

// helper functions
const char *tokToString(int token);

// operations on the scanner
void doctokenizerYYFindSections(const char *input,Definition *d,
                                MemberGroup *mg,const char *fileName);
void doctokenizerYYinit(const char *input,const char *fileName);
void doctokenizerYYcleanup();
void doctokenizerYYpushContext();
bool doctokenizerYYpopContext();
int  doctokenizerYYlex();
void doctokenizerYYsetStatePara();
void doctokenizerYYsetStateTitle();
void doctokenizerYYsetStateTitleAttrValue();
void doctokenizerYYsetStateCode();
void doctokenizerYYsetStateXmlCode();
void doctokenizerYYsetStateHtmlOnly();
void doctokenizerYYsetStateManOnly();
void doctokenizerYYsetStateLatexOnly();
void doctokenizerYYsetStateXmlOnly();
void doctokenizerYYsetStateVerbatim();
void doctokenizerYYsetStateDot();
void doctokenizerYYsetStateMsc();
void doctokenizerYYsetStateParam();
void doctokenizerYYsetStateXRefItem();
void doctokenizerYYsetStateFile();
void doctokenizerYYsetStatePattern();
void doctokenizerYYsetStateLink();
void doctokenizerYYsetStateRef();
void doctokenizerYYsetStateInternalRef();
void doctokenizerYYsetStateText();
void doctokenizerYYsetStateSkipTitle();
void doctokenizerYYsetInsidePre(bool b);
void doctokenizerYYpushBackHtmlTag(const char *tag);

#endif
