/******************************************************************************
 *
 * $Id: $
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

#ifndef _DOCTOKENIZER_H
#define _DOCTOKENIZER_H

#include <stdio.h>

#include "htmlattrib.h"
#include "qcstring.h"

class Definition;

enum Tokens
{
  TK_WORD          = 1,
  TK_LNKWORD       = 2,
  TK_WHITESPACE    = 3,
  TK_LISTITEM      = 4,
  TK_ENDLIST       = 5,
  TK_COMMAND_AT    = 6, //! Command starting with `@`
  TK_HTMLTAG       = 7,
  TK_SYMBOL        = 8,
  TK_NEWPARA       = 9,
  TK_RCSTAG        = 10,
  TK_URL           = 11,
  TK_COMMAND_BS    = 12, //! Command starting with `\`

  RetVal_OK             = 0x10000,
  RetVal_SimpleSec      = 0x10001,
  RetVal_ListItem       = 0x10002,
  RetVal_Section        = 0x10003,
  RetVal_Subsection     = 0x10004,
  RetVal_Subsubsection  = 0x10005,
  RetVal_Paragraph      = 0x10006,
  RetVal_SubParagraph   = 0x10007,
  RetVal_EndList        = 0x10008,
  RetVal_EndPre         = 0x10009,
  RetVal_DescData       = 0x1000A,
  RetVal_DescTitle      = 0x1000B,
  RetVal_EndDesc        = 0x1000C,
  RetVal_TableRow       = 0x1000D,
  RetVal_TableCell      = 0x1000E,
  RetVal_TableHCell     = 0x1000F,
  RetVal_EndTable       = 0x10010,
  RetVal_Internal       = 0x10011,
  RetVal_SwitchLang     = 0x10012,
  RetVal_CloseXml       = 0x10013,
  RetVal_EndBlockQuote  = 0x10014,
  RetVal_CopyDoc        = 0x10015,
  RetVal_EndInternal    = 0x10016,
  RetVal_EndParBlock    = 0x10017
};

/** @brief Data associated with a token used by the comment block parser. */
struct TokenInfo
{
  TokenInfo() : isEnumList(FALSE), indent(0), id(-1), endTag(FALSE), emptyTag(FALSE), paramDir(Unspecified) {}
  // command token
  QCString name;

  // command text (RCS tag)
  QCString text;

  // comment blocks

  // list token info
  bool isEnumList = false;
  int indent = 0;

  // sections
  QCString sectionId;

  // simple section
  QCString simpleSectName;
  QCString simpleSectText;

  // verbatim fragment
  QCString verb;

  // xrefitem
  int id = -1;

  // html tag
  HtmlAttribList attribs;
  bool endTag = false;
  bool emptyTag = false;
  QCString attribsStr;

  // whitespace
  QCString chars;

  // url
  bool isEMailAddr = false;

  // param attributes
  enum ParamDir { In=1, Out=2, InOut=3, Unspecified=0 };
  ParamDir paramDir = Unspecified;
};

// globals
extern TokenInfo *g_token;
extern FILE *doctokenizerYYin;

// helper functions
const char *tokToString(int token);

void setDoctokinizerLineNr(int lineno);
int getDoctokinizerLineNr(void);

// operations on the scanner
void doctokenizerYYFindSections(const QCString &input,const Definition *d,
                                const QCString &fileName);
void doctokenizerYYinit(const char *input,const QCString &fileName,bool markdownSupport);
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
void doctokenizerYYsetStateDbOnly();
void doctokenizerYYsetStateRtfOnly();
void doctokenizerYYsetStateVerbatim();
void doctokenizerYYsetStateDot();
void doctokenizerYYsetStateMsc();
void doctokenizerYYsetStateParam();
void doctokenizerYYsetStateXRefItem();
void doctokenizerYYsetStateFile();
void doctokenizerYYsetStatePattern();
void doctokenizerYYsetStateLink();
void doctokenizerYYsetStateCite();
void doctokenizerYYsetStateRef();
void doctokenizerYYsetStateInternalRef();
void doctokenizerYYsetStateText();
void doctokenizerYYsetStateSkipTitle();
void doctokenizerYYsetStateAnchor();
void doctokenizerYYsetInsidePre(bool b);
void doctokenizerYYpushBackHtmlTag(const QCString &tag);
void doctokenizerYYsetStateSnippet();
void doctokenizerYYstartAutoList();
void doctokenizerYYendAutoList();
void doctokenizerYYsetStatePlantUML();
void doctokenizerYYsetStateSetScope();
void doctokenizerYYsetStatePlantUMLOpt();
void doctokenizerYYsetStateOptions();
void doctokenizerYYsetStateBlock();
void doctokenizerYYsetStateEmoji();
void doctokenizerYYsetStateIline();

#endif
