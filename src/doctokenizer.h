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

#ifndef DOCTOKENIZER_H
#define DOCTOKENIZER_H

#include <stdio.h>
#include <memory>

#include "htmlattrib.h"
#include "qcstring.h"
#include "construct.h"

#define TOKEN_SPECIFICATIONS \
  TKSPEC(TK_EOF,          -1)                                   \
  TKSPEC(TK_NONE,          0)                                   \
  TKSPEC(TK_WORD,          1)                                   \
  TKSPEC(TK_LNKWORD,       2)                                   \
  TKSPEC(TK_WHITESPACE,    3)                                   \
  TKSPEC(TK_LISTITEM,      4)                                   \
  TKSPEC(TK_ENDLIST,       5)                                   \
  TKSPEC(TK_COMMAND_AT,    6) /*! Command starting with `@` */  \
  TKSPEC(TK_HTMLTAG,       7)                                   \
  TKSPEC(TK_SYMBOL,        8)                                   \
  TKSPEC(TK_NEWPARA,       9)                                   \
  TKSPEC(TK_RCSTAG,       10)                                   \
  TKSPEC(TK_URL,          11)                                   \
  TKSPEC(TK_COMMAND_BS,   12) /*! Command starting with `\` */

#define RETVAL_SPECIFICATIONS \
  TKSPEC(RetVal_OK,              0x10000)  \
  TKSPEC(RetVal_SimpleSec,       0x10001)  \
  TKSPEC(RetVal_ListItem,        0x10002)  \
  TKSPEC(RetVal_Section,         0x10003)  \
  TKSPEC(RetVal_Subsection,      0x10004)  \
  TKSPEC(RetVal_Subsubsection,   0x10005)  \
  TKSPEC(RetVal_Paragraph,       0x10006)  \
  TKSPEC(RetVal_SubParagraph,    0x10007)  \
  TKSPEC(RetVal_EndList,         0x10008)  \
  TKSPEC(RetVal_EndPre,          0x10009)  \
  TKSPEC(RetVal_DescData,        0x1000A)  \
  TKSPEC(RetVal_DescTitle,       0x1000B)  \
  TKSPEC(RetVal_EndDesc,         0x1000C)  \
  TKSPEC(RetVal_TableRow,        0x1000D)  \
  TKSPEC(RetVal_TableCell,       0x1000E)  \
  TKSPEC(RetVal_TableHCell,      0x1000F)  \
  TKSPEC(RetVal_EndTable,        0x10010)  \
  TKSPEC(RetVal_Internal,        0x10011)  \
  TKSPEC(RetVal_SwitchLang,      0x10012)  \
  TKSPEC(RetVal_CloseXml,        0x10013)  \
  TKSPEC(RetVal_EndBlockQuote,   0x10014)  \
  TKSPEC(RetVal_CopyDoc,         0x10015)  \
  TKSPEC(RetVal_EndInternal,     0x10016)  \
  TKSPEC(RetVal_EndParBlock,     0x10017)  \
  TKSPEC(RetVal_EndHtmlDetails,  0x10018)  \
  TKSPEC(RetVal_SubSubParagraph, 0x10019)

enum class TokenRetval
{
#define TKSPEC(x,y) x = y,
      TOKEN_SPECIFICATIONS
      RETVAL_SPECIFICATIONS
#undef TKSPEC
};

class Token
{
  public:
    explicit Token(TokenRetval tv) : m_value(tv) {}
    TokenRetval value() const { return m_value; }
#define TKSPEC(x,y) static Token make_##x() { return Token(TokenRetval::x); }
    TOKEN_SPECIFICATIONS
    RETVAL_SPECIFICATIONS
#undef TKSPEC

    const char *to_string() const
    {
      const char *result = "ERROR";
      switch (m_value)
      {
#define TKSPEC(x,y) case TokenRetval::x: result = #x; break;
        TOKEN_SPECIFICATIONS
        RETVAL_SPECIFICATIONS
#undef TKSPEC
      }
      return result;
    }

    char command_to_char() const
    {
      return m_value==TokenRetval::TK_COMMAND_AT ? '@' : '\\';
    }

    static Token char_to_command(char c)
    {
      return c=='@' ? make_TK_COMMAND_AT() : make_TK_COMMAND_BS();
    }

    template<typename... ARGS>
    bool is_any_of(ARGS... args) const
    {
      return ((m_value == args) || ...);
    }

    bool is(TokenRetval rv) const
    {
      return m_value==rv;
    }

    friend inline bool operator==(const Token &t1,const Token &t2) { return t1.m_value==t2.m_value; }
    friend inline bool operator!=(const Token &t1,const Token &t2) { return !(operator==(t1,t2)); }

  private:
    TokenRetval m_value;
};

/** @brief Data associated with a token used by the comment block parser. */
struct TokenInfo
{
  // command token
  QCString name;

  // command text (RCS tag)
  QCString text;

  // comment blocks

  // list token info
  bool isEnumList = false;
  bool isCheckedList = false;
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

class Definition;

class DocTokenizer
{
  public:
    DocTokenizer();
   ~DocTokenizer();
    NON_COPYABLE(DocTokenizer)

    TokenInfo *token();
    [[maybe_unused]] TokenInfo *resetToken();

    void setLineNr(int lineno);
    int getLineNr(void);
    void pushState();
    void popState();

    // operations on the scanner
    void findSections(const QCString &input,const Definition *d,
        const QCString &fileName);
    void init(const char *input,const QCString &fileName,
              bool markdownSupport, bool insideHtmlLink);
    void cleanup();
    void pushContext();
    bool popContext();
    Token  lex();
    void unputString(const QCString &tag);
    void setStatePara();
    void setStateTitle();
    void setStateTitleAttrValue();
    void setStateCode();
    void setStateICode();
    void setStateXmlCode();
    void setStateHtmlOnly();
    void setStateManOnly();
    void setStateLatexOnly();
    void setStateXmlOnly();
    void setStateDbOnly();
    void setStateRtfOnly();
    void setStateVerbatim();
    void setStateIVerbatim();
    void setStateILiteral();
    void setStateILiteralOpt();
    void setStateDot();
    void setStateMsc();
    void setStateParam();
    void setStateXRefItem();
    void setStateFile();
    void setStateIFile();
    void setStatePattern();
    void setStateLink();
    void setStateCite();
    void setStateDoxyConfig();
    void setStateRef();
    void setStateInternalRef();
    void setStateText();
    void setStateSkipTitle();
    void setStateAnchor();
    void setInsidePre(bool b);
    void pushBackHtmlTag(const QCString &tag);
    void setStateSnippet();
    void startAutoList();
    void endAutoList();
    void setStatePlantUML();
    void setStateSetScope();
    void setStatePlantUMLOpt();
    void setStateOptions();
    void setStateBlock();
    void setStateEmoji();
    void setStateILine();
    void setStateQuotedString();
    void setStateShowDate();
    void setStatePrefix();

  private:
    struct Private;
    std::unique_ptr<Private> p;
};

#endif
