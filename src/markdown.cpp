/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

/* Note: part of the code below is inspired by libupskirt written by
 * Natacha Porté. Original copyright message follows:
 *
 * Copyright (c) 2008, Natacha Porté
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>

#include <unordered_map>
#include <functional>
#include <atomic>
#include <array>
#include <string_view>

#include "markdown.h"
#include "debug.h"
#include "util.h"
#include "doxygen.h"
#include "commentscan.h"
#include "entry.h"
#include "config.h"
#include "message.h"
#include "portable.h"
#include "regex.h"
#include "fileinfo.h"
#include "trace.h"
#include "anchor.h"
#include "stringutil.h"

#if !ENABLE_MARKDOWN_TRACING
#undef  AUTO_TRACE
#undef  AUTO_TRACE_ADD
#undef  AUTO_TRACE_EXIT
#define AUTO_TRACE(...)      (void)0
#define AUTO_TRACE_ADD(...)  (void)0
#define AUTO_TRACE_EXIT(...) (void)0
#endif

enum class ExplicitPageResult
{
  explicitPage,      /**< docs start with a page command */
  explicitMainPage,  /**< docs start with a mainpage command */
  notExplicit        /**< docs doesn't start with either page or mainpage */
};

//-----------

// is character c part of an identifier?
#define isIdChar(c) \
  ((c>='a' && c<='z') || \
   (c>='A' && c<='Z') || \
   (c>='0' && c<='9') || \
   (static_cast<unsigned char>(c)>=0x80)) // unicode characters

// is character allowed right at the beginning of an emphasis section
#define extraChar(c) \
  (c=='-' || c=='+' || c=='!' || \
   c=='?' || c=='$' || c=='@' || \
   c=='&' || c=='*' || c=='%' || \
   c=='[' || c=='(' || c=='.' || \
   c=='>' || c==':' || c==',' || \
   c==';' || c=='\'' || c=='"' || c=='`')

// is character at position i in data allowed before an emphasis section
#define isOpenEmphChar(c) \
  (c=='\n' || c==' ' || c=='\'' || c=='<' || \
   c=='>'  || c=='{' || c=='('  || c=='[' || \
   c==','  || c==':' || c==';')

// is character at position i in data an escape that prevents ending an emphasis section
// so for example *bla (*.txt) is cool*
#define ignoreCloseEmphChar(c,cn) \
  (c=='('  || c=='{' || c=='[' || (c=='<' && cn!='/') || \
   c=='\\' || \
   c=='@')
//----------

struct TableCell
{
  TableCell() : colSpan(false) {}
  QCString cellText;
  bool colSpan;
};

struct Markdown::Private
{
  Private(const QCString &fn,int line,int indent)
    : fileName(fn), lineNr(line), indentLevel(indent)
  {
    // setup callback table for special characters
    actions[static_cast<unsigned int>('_')] = [this](std::string_view data,size_t offset) { return processEmphasis      (data,offset); };
    actions[static_cast<unsigned int>('*')] = [this](std::string_view data,size_t offset) { return processEmphasis      (data,offset); };
    actions[static_cast<unsigned int>('~')] = [this](std::string_view data,size_t offset) { return processEmphasis      (data,offset); };
    actions[static_cast<unsigned int>('`')] = [this](std::string_view data,size_t offset) { return processCodeSpan      (data,offset); };
    actions[static_cast<unsigned int>('\\')]= [this](std::string_view data,size_t offset) { return processSpecialCommand(data,offset); };
    actions[static_cast<unsigned int>('@')] = [this](std::string_view data,size_t offset) { return processSpecialCommand(data,offset); };
    actions[static_cast<unsigned int>('[')] = [this](std::string_view data,size_t offset) { return processLink          (data,offset); };
    actions[static_cast<unsigned int>('!')] = [this](std::string_view data,size_t offset) { return processLink          (data,offset); };
    actions[static_cast<unsigned int>('<')] = [this](std::string_view data,size_t offset) { return processHtmlTag       (data,offset); };
    actions[static_cast<unsigned int>('-')] = [this](std::string_view data,size_t offset) { return processNmdash        (data,offset); };
    actions[static_cast<unsigned int>('"')] = [this](std::string_view data,size_t offset) { return processQuoted        (data,offset); };
  }

  QCString processQuotations(std::string_view data,size_t refIndent);
  QCString processBlocks(std::string_view data,size_t indent);
  QCString isBlockCommand(std::string_view data,size_t offset);
  size_t isSpecialCommand(std::string_view data,size_t offset);
  size_t findEndOfLine(std::string_view data,size_t offset);
  int processHtmlTagWrite(std::string_view data,size_t offset,bool doWrite);
  int processHtmlTag(std::string_view data,size_t offset);
  int processEmphasis(std::string_view data,size_t offset);
  int processEmphasis1(std::string_view data,char c);
  int processEmphasis2(std::string_view data,char c);
  int processEmphasis3(std::string_view data,char c);
  int processNmdash(std::string_view data,size_t offset);
  int processQuoted(std::string_view data,size_t offset);
  int processCodeSpan(std::string_view data,size_t offset);
  int processSpecialCommand(std::string_view data,size_t offset);
  int processLink(std::string_view data,size_t offset);
  size_t findEmphasisChar(std::string_view, char c, size_t c_size);
  void addStrEscapeUtf8Nbsp(std::string_view data);
  void processInline(std::string_view data);
  void writeMarkdownImage(std::string_view fmt, bool inline_img, bool explicitTitle,
      const QCString &title, const QCString &content,
      const QCString &link, const QCString &attributes,
      const FileDef *fd);
  int isHeaderline(std::string_view data, bool allowAdjustLevel);
  int isAtxHeader(std::string_view data, QCString &header,QCString &id,bool allowAdjustLevel,
      bool *pIsIdGenerated=nullptr);
  void writeOneLineHeaderOrRuler(std::string_view data);
  void writeFencedCodeBlock(std::string_view data, std::string_view lang,
      size_t blockStart,size_t blockEnd);
  size_t writeBlockQuote(std::string_view data);
  size_t writeCodeBlock(std::string_view,size_t refIndent);
  size_t writeTableBlock(std::string_view data);
  QCString extractTitleId(QCString &title, int level,bool *pIsIdGenerated=nullptr);

  struct LinkRef
  {
    LinkRef(const QCString &l,const QCString &t) : link(l), title(t) {}
    QCString link;
    QCString title;
  };
  using Action_t = std::function<int(std::string_view,size_t)>;

  std::unordered_map<std::string,LinkRef> linkRefs;
  QCString       fileName;
  int            lineNr = 0;
  int            indentLevel=0;  // 0 is outside markdown, -1=page level
  QCString       out;
  std::array<Action_t,256> actions;
};

Markdown::Markdown(const QCString &fileName,int lineNr,int indentLevel)
  : prv(std::make_unique<Private>(fileName,lineNr,indentLevel))
{
  using namespace std::placeholders;
  (void)lineNr; // not used yet
}

Markdown::~Markdown() = default;

void Markdown::setIndentLevel(int level) { prv->indentLevel = level; }


enum Alignment { AlignNone, AlignLeft, AlignCenter, AlignRight };


//---------- constants -------
//
const char *g_utf8_nbsp = "\xc2\xa0";      // UTF-8 nbsp
const char *g_doxy_nbsp = "&_doxy_nbsp;";  // doxygen escape command for UTF-8 nbsp
const size_t codeBlockIndent = 4;

//---------- helpers -------

// test if the next characters in data represent a new line (which can be character \n or string \ilinebr).
// returns 0 if no newline is found, or the number of characters that make up the newline if found.
inline size_t isNewline(std::string_view data)
{
  // normal newline
  if (data[0] == '\n') return 1;
  // artificial new line from ^^ in ALIASES
  if (data[0] == '\\' && qstrncmp(data.data()+1,"ilinebr ",7)==0) return data[8]==' ' ? 9 : 8;
  return 0;
}

// escape double quotes in string
static QCString escapeDoubleQuotes(const QCString &s)
{
  AUTO_TRACE("s={}",Trace::trunc(s));
  if (s.isEmpty()) return s;
  QCString result;
  const char *p=s.data();
  char c=0, pc='\0';
  while ((c=*p++))
  {
    if (c=='"' && pc!='\\') result+='\\';
    result+=c;
    pc=c;
  }
  AUTO_TRACE_EXIT("result={}",result);
  return result;
}

// escape characters that have a special meaning later on.
static QCString escapeSpecialChars(const QCString &s)
{
  AUTO_TRACE("s={}",Trace::trunc(s));
  if (s.isEmpty()) return s;
  bool insideQuote=FALSE;
  QCString result;
  const char *p=s.data();
  char c=0, pc='\0';
  while ((c=*p++))
  {
    switch (c)
    {
      case '"':
        if (pc!='\\')  { insideQuote=!insideQuote; }
        result+=c;
        break;
      case '<':
        // fall through
      case '>':
        if (!insideQuote)
        {
          result+='\\';
          result+=c;
          if ((p[0]==':') && (p[1]==':'))
          {
            result+='\\';
            result+=':';
            p++;
          }
        }
        else
        {
          result+=c;
        }
        break;
      case '\\': if (!insideQuote) { result+='\\'; } result+='\\'; break;
      case '@':  if (!insideQuote) { result+='\\'; } result+='@'; break;
      // commented out next line due to regression when using % to suppress a link
      //case '%':  if (!insideQuote) { result+='\\'; } result+='%'; break;
      case '#':  if (!insideQuote) { result+='\\'; } result+='#'; break;
      case '$':  if (!insideQuote) { result+='\\'; } result+='$'; break;
      case '&':  if (!insideQuote) { result+='\\'; } result+='&'; break;
      default:
        result+=c; break;
    }
    pc=c;
  }
  AUTO_TRACE_EXIT("result={}",result);
  return result;
}

/** helper function to convert presence of left and/or right alignment markers
 *  to a alignment value
 */
static Alignment markersToAlignment(bool leftMarker,bool rightMarker)
{
  if (leftMarker && rightMarker)
  {
    return AlignCenter;
  }
  else if (leftMarker)
  {
    return AlignLeft;
  }
  else if (rightMarker)
  {
    return AlignRight;
  }
  else
  {
    return AlignNone;
  }
}

/** parse the image attributes and return attributes for given format */
static QCString getFilteredImageAttributes(std::string_view fmt, const QCString &attrs)
{
  AUTO_TRACE("fmt={} attrs={}",fmt,attrs);
  StringVector attrList = split(attrs.str(),",");
  for (const auto &attr_ : attrList)
  {
    QCString attr = QCString(attr_).stripWhiteSpace();
    int i = attr.find(':');
    if (i>0) // has format
    {
      QCString format = attr.left(i).stripWhiteSpace().lower();
      if (format == fmt) // matching format
      {
        AUTO_TRACE_EXIT("result={}",attr.mid(i+1));
        return attr.mid(i+1); // keep part after :
      }
    }
    else // option that applies to all formats
    {
      AUTO_TRACE_EXIT("result={}",attr);
      return attr;
    }
  }
  return QCString();
}

// Check if data contains a block command. If so returned the command
// that ends the block. If not an empty string is returned.
// Note When offset>0 character position -1 will be inspected.
//
// Checks for and skip the following block commands:
// {@code .. { .. } .. }
// \dot .. \enddot
// \code .. \endcode
// \msc .. \endmsc
// \f$..\f$
// \f(..\f)
// \f[..\f]
// \f{..\f}
// \verbatim..\endverbatim
// \iliteral..\endiliteral
// \latexonly..\endlatexonly
// \htmlonly..\endhtmlonly
// \xmlonly..\endxmlonly
// \rtfonly..\endrtfonly
// \manonly..\endmanonly
// \startuml..\enduml
QCString Markdown::Private::isBlockCommand(std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);

  using EndBlockFunc = QCString (*)(const std::string &,bool,char);

  static const auto getEndBlock   = [](const std::string &blockName,bool,char) -> QCString
  {
    return "end"+blockName;
  };
  static const auto getEndCode    = [](const std::string &blockName,bool openBracket,char) -> QCString
  {
    return openBracket ? QCString("}") : "end"+blockName;
  };
  static const auto getEndUml     = [](const std::string &/* blockName */,bool,char) -> QCString
  {
    return "enduml";
  };
  static const auto getEndFormula = [](const std::string &/* blockName */,bool,char nextChar) -> QCString
  {
    switch (nextChar)
    {
      case '$': return "f$";
      case '(': return "f)";
      case '[': return "f]";
      case '{': return "f}";
    }
    return "";
  };

  // table mapping a block start command to a function that can return the matching end block string
  static const std::unordered_map<std::string,EndBlockFunc> blockNames =
  {
    { "dot",         getEndBlock   },
    { "code",        getEndCode    },
    { "icode",       getEndBlock   },
    { "msc",         getEndBlock   },
    { "verbatim",    getEndBlock   },
    { "iverbatim",   getEndBlock   },
    { "iliteral",    getEndBlock   },
    { "latexonly",   getEndBlock   },
    { "htmlonly",    getEndBlock   },
    { "xmlonly",     getEndBlock   },
    { "rtfonly",     getEndBlock   },
    { "manonly",     getEndBlock   },
    { "docbookonly", getEndBlock   },
    { "startuml",    getEndUml     },
    { "f",           getEndFormula }
  };

  const size_t size = data.size();
  bool openBracket = offset>0 && data.data()[-1]=='{';
  bool isEscaped = offset>0 && (data.data()[-1]=='\\' || data.data()[-1]=='@');
  if (isEscaped) return QCString();

  size_t end=1;
  while (end<size && (data[end]>='a' && data[end]<='z')) end++;
  if (end==1) return QCString();
  std::string blockName(data.substr(1,end-1));
  auto it = blockNames.find(blockName);
  QCString result;
  if (it!=blockNames.end()) // there is a function assigned
  {
    result = it->second(blockName, openBracket, end<size ? data[end] : 0);
  }
  AUTO_TRACE_EXIT("result={}",result);
  return result;
}

size_t Markdown::Private::isSpecialCommand(std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);

  using EndCmdFunc = size_t (*)(std::string_view,size_t);

  static const auto endOfLine = [](std::string_view data_,size_t offset_) -> size_t
  {
    // skip until the end of line (allowing line continuation characters)
    char lc = 0;
    char c = 0;
    while (offset_<data_.size() && ((c=data_[offset_])!='\n' || lc=='\\'))
    {
      if (c=='\\')     lc='\\'; // last character was a line continuation
      else if (c!=' ') lc=0;    // rest line continuation
      offset_++;
    }
    return offset_;
  };

  static const auto endOfLabel = [](std::string_view data_,size_t offset_) -> size_t
  {
    if (offset_<data_.size() && data_[offset_]==' ') // we expect a space before the label
    {
      char c = 0;
      offset_++;
      // skip over spaces
      while (offset_<data_.size() && data_[offset_]==' ') offset_++;
      // skip over label
      while (offset_<data_.size() && (c=data_[offset_])!=' ' && c!='\\' && c!='@' && c!='\n') offset_++;
      return offset_;
    }
    return 0;
  };

  static const auto endOfLabelOpt = [](std::string_view data_,size_t offset_) -> size_t
  {
    size_t index=offset_;
    if (index<data_.size() && data_[index]==' ') // skip over optional spaces
    {
      index++;
      while (index<data_.size() && data_[index]==' ') index++;
    }
    if (index<data_.size() && data_[index]=='{') // find matching '}'
    {
      index++;
      char c = 0;
      while (index<data_.size() && (c=data_[index])!='}' && c!='\\' && c!='@' && c!='\n') index++;
      if (index==data_.size() || data_[index]!='}') return 0; // invalid option
      offset_=index+1; // part after {...} is the option
    }
    return endOfLabel(data_,offset_);
  };

  static const auto endOfParam = [](std::string_view data_,size_t offset_) -> size_t
  {
    size_t index=offset_;
    if (index<data_.size() && data_[index]==' ') // skip over optional spaces
    {
      index++;
      while (index<data_.size() && data_[index]==' ') index++;
    }
    if (index<data_.size() && data_[index]=='[') // find matching ']'
    {
      index++;
      char c = 0;
      while (index<data_.size() && (c=data_[index])!=']' && c!='\n') index++;
      if (index==data_.size() || data_[index]!=']') return 0; // invalid parameter
      offset_=index+1; // part after [...] is the parameter name
    }
    return endOfLabel(data_,offset_);
  };

  static const auto endOfFuncLike = [](std::string_view data_,size_t offset_,bool allowSpaces) -> size_t
  {
    if (offset_<data_.size() && data_[offset_]==' ') // we expect a space before the name
    {
      char c=0;
      offset_++;
      // skip over spaces
      while (offset_<data_.size() && data_[offset_]==' ')
      {
        offset_++;
      }
      // skip over name (and optionally type)
      while (offset_<data_.size() && (c=data_[offset_])!='\n' && (allowSpaces || c!=' ') && c!='(')
      {
        offset_++;
      }
      if (c=='(') // find the end of the function
      {
        int count=1;
        offset_++;
        while (offset_<data_.size() && (c=data_[offset_++]))
        {
          if      (c=='(') count++;
          else if (c==')') count--;
          if (count==0) return offset_;
        }
      }
      return offset_;
    }
    return 0;
  };

  static const auto endOfFunc = [](std::string_view data_,size_t offset_) -> size_t
  {
    return endOfFuncLike(data_,offset_,true);
  };

  static const auto endOfGuard = [](std::string_view data_,size_t offset_) -> size_t
  {
    return endOfFuncLike(data_,offset_,false);
  };

  static const std::unordered_map<std::string,EndCmdFunc> cmdNames =
  {
    { "a",              endOfLabel },
    { "addindex",       endOfLine  },
    { "addtogroup",     endOfLabel },
    { "anchor",         endOfLabel },
    { "b",              endOfLabel },
    { "c",              endOfLabel },
    { "category",       endOfLine  },
    { "cite",           endOfLabel },
    { "class",          endOfLine  },
    { "concept",        endOfLine  },
    { "copybrief",      endOfFunc  },
    { "copydetails",    endOfFunc  },
    { "copydoc",        endOfFunc  },
    { "def",            endOfFunc  },
    { "defgroup",       endOfLabel },
    { "diafile",        endOfLine  },
    { "dir",            endOfLine  },
    { "dockbookinclude",endOfLine  },
    { "dontinclude",    endOfLine  },
    { "dotfile",        endOfLine  },
    { "e",              endOfLabel },
    { "elseif",         endOfGuard },
    { "em",             endOfLabel },
    { "emoji",          endOfLabel },
    { "enum",           endOfLabel },
    { "example",        endOfLine  },
    { "exception",      endOfLine  },
    { "extends",        endOfLabel },
    { "file",           endOfLine  },
    { "fn",             endOfFunc  },
    { "headerfile",     endOfLine  },
    { "htmlinclude",    endOfLine  },
    { "ianchor",        endOfLabelOpt },
    { "idlexcept",      endOfLine  },
    { "if",             endOfGuard },
    { "ifnot",          endOfGuard },
    { "image",          endOfLine  },
    { "implements",     endOfLine  },
    { "include",        endOfLine  },
    { "includedoc",     endOfLine  },
    { "includelineno",  endOfLine  },
    { "ingroup",        endOfLabel },
    { "interface",      endOfLine  },
    { "latexinclude",   endOfLine  },
    { "maninclude",     endOfLine  },
    { "memberof",       endOfLabel },
    { "mscfile",        endOfLine  },
    { "namespace",      endOfLabel },
    { "noop",           endOfLine  },
    { "overload",       endOfLine  },
    { "p",              endOfLabel },
    { "package",        endOfLabel },
    { "page",           endOfLabel },
    { "paragraph",      endOfLabel },
    { "param",          endOfParam },
    { "property",       endOfLine  },
    { "protocol",       endOfLine  },
    { "qualifier",      endOfLine  },
    { "ref",            endOfLabel },
    { "refitem",        endOfLine  },
    { "related",        endOfLabel },
    { "relatedalso",    endOfLabel },
    { "relates",        endOfLabel },
    { "relatesalso",    endOfLabel },
    { "retval",         endOfLabel },
    { "rtfinclude",     endOfLine  },
    { "section",        endOfLabel },
    { "skip",           endOfLine  },
    { "skipline",       endOfLine  },
    { "snippet",        endOfLine  },
    { "snippetdoc",     endOfLine  },
    { "snippetlineno",  endOfLine  },
    { "struct",         endOfLine  },
    { "subpage",        endOfLabel },
    { "subparagraph",   endOfLabel },
    { "subsubparagraph",endOfLabel },
    { "subsection",     endOfLabel },
    { "subsubsection",  endOfLabel },
    { "throw",          endOfLabel },
    { "throws",         endOfLabel },
    { "tparam",         endOfLabel },
    { "typedef",        endOfLine  },
    { "plantumlfile",   endOfLine  },
    { "union",          endOfLine  },
    { "until",          endOfLine  },
    { "var",            endOfLine  },
    { "verbinclude",    endOfLine  },
    { "weakgroup",      endOfLabel },
    { "xmlinclude",     endOfLine  },
    { "xrefitem",       endOfLabel }
  };

  bool isEscaped = offset>0 && (data.data()[-1]=='\\' || data.data()[-1]=='@');
  if (isEscaped) return 0;

  const size_t size = data.size();
  size_t end=1;
  while (end<size && (data[end]>='a' && data[end]<='z')) end++;
  if (end==1) return 0;
  std::string cmdName(data.substr(1,end-1));
  size_t result=0;
  auto it = cmdNames.find(cmdName);
  if (it!=cmdNames.end()) // command with parameters that should be ignored by markdown
  {
    // find the end of the parameters
    result = it->second(data,end);
  }
  AUTO_TRACE_EXIT("result={}",result);
  return result;
}

/** looks for the next emph char, skipping other constructs, and
 *  stopping when either it is found, or we are at the end of a paragraph.
 */
size_t Markdown::Private::findEmphasisChar(std::string_view data, char c, size_t c_size)
{
  AUTO_TRACE("data='{}' c={} c_size={}",Trace::trunc(data),c,c_size);
  size_t i = 1;
  const size_t size = data.size();

  while (i<size)
  {
    while (i<size && data[i]!=c    &&
                     data[i]!='\\' && data[i]!='@' &&
                     !(data[i]=='/' && data[i-1]=='<') && // html end tag also ends emphasis
                     data[i]!='\n') i++;
    // avoid overflow (unclosed emph token)
    if (i==size)
    {
      return 0;
    }
    //printf("findEmphasisChar: data=[%s] i=%d c=%c\n",data,i,data[i]);

    // not counting escaped chars or characters that are unlikely
    // to appear as the end of the emphasis char
    if (ignoreCloseEmphChar(data[i-1],data[i]))
    {
      i++;
      continue;
    }
    else
    {
      // get length of emphasis token
      size_t len = 0;
      while (i+len<size && data[i+len]==c)
      {
        len++;
      }

      if (len>0)
      {
        if (len!=c_size || (i+len<size && isIdChar(data[i+len]))) // to prevent touching some_underscore_identifier
        {
          i+=len;
          continue;
        }
        AUTO_TRACE_EXIT("result={}",i);
        return static_cast<int>(i); // found it
      }
    }

    // skipping a code span
    if (data[i]=='`')
    {
      int snb=0;
      while (i<size && data[i]=='`') snb++,i++;

      // find same pattern to end the span
      int enb=0;
      while (i<size && enb<snb)
      {
        if (data[i]=='`') enb++;
        if (snb==1 && data[i]=='\'') break; // ` ended by '
        i++;
      }
    }
    else if (data[i]=='@' || data[i]=='\\')
    { // skip over blocks that should not be processed
      QCString endBlockName = isBlockCommand(data.substr(i),i);
      if (!endBlockName.isEmpty())
      {
        i++;
        size_t l = endBlockName.length();
        while (i+l<size)
        {
          if ((data[i]=='\\' || data[i]=='@') && // command
              data[i-1]!='\\' && data[i-1]!='@') // not escaped
          {
            if (qstrncmp(&data[i+1],endBlockName.data(),l)==0)
            {
              break;
            }
          }
          i++;
        }
      }
      else if (i+1<size && isIdChar(data[i+1])) // @cmd, stop processing, see bug 690385
      {
        return 0;
      }
      else
      {
        i++;
      }
    }
    else if (data[i-1]=='<' && data[i]=='/') // html end tag invalidates emphasis
    {
      return 0;
    }
    else if (data[i]=='\n') // end * or _ at paragraph boundary
    {
      i++;
      while (i<size && data[i]==' ') i++;
      if (i>=size || data[i]=='\n')
      {
        return 0;
      } // empty line -> paragraph
    }
    else // should not get here!
    {
      i++;
    }
  }
  return 0;
}

/** process single emphasis */
int Markdown::Private::processEmphasis1(std::string_view data, char c)
{
  AUTO_TRACE("data='{}' c={}",Trace::trunc(data),c);
  size_t i = 0;
  const size_t size = data.size();

  /* skipping one symbol if coming from emph3 */
  if (size>1 && data[0]==c && data[1]==c) { i=1; }

  while (i<size)
  {
    size_t len = findEmphasisChar(data.substr(i), c, 1);
    if (len==0) { return 0; }
    i+=len;
    if (i>=size) { return 0; }

    if (i+1<size && data[i+1]==c)
    {
      i++;
      continue;
    }
    if (data[i]==c && data[i-1]!=' ' && data[i-1]!='\n')
    {
      out+="<em>";
      processInline(data.substr(0,i));
      out+="</em>";
      AUTO_TRACE_EXIT("result={}",i+1);
      return static_cast<int>(i+1);
    }
  }
  return 0;
}

/** process double emphasis */
int Markdown::Private::processEmphasis2(std::string_view data, char c)
{
  AUTO_TRACE("data='{}' c={}",Trace::trunc(data),c);
  size_t i = 0;
  const size_t size = data.size();

  while (i<size)
  {
    size_t len = findEmphasisChar(data.substr(i), c, 2);
    if (len==0)
    {
      return 0;
    }
    i += len;
    if (i+1<size && data[i]==c && data[i+1]==c && i && data[i-1]!=' ' && data[i-1]!='\n')
    {
      if (c == '~') out+="<strike>";
      else out+="<strong>";
      processInline(data.substr(0,i));
      if (c == '~') out+="</strike>";
      else out+="</strong>";
      AUTO_TRACE_EXIT("result={}",i+2);
      return static_cast<int>(i+2);
    }
    i++;
  }
  return 0;
}

/** Parsing triple emphasis.
 *  Finds the first closing tag, and delegates to the other emph
 */
int Markdown::Private::processEmphasis3(std::string_view data,char c)
{
  AUTO_TRACE("data='{}' c={}",Trace::trunc(data),c);
  size_t i = 0;
  const size_t size = data.size();

  while (i<size)
  {
    size_t len = findEmphasisChar(data.substr(i), c, 3);
    if (len==0)
    {
      return 0;
    }
    i+=len;

    /* skip whitespace preceded symbols */
    if (data[i]!=c || data[i-1]==' ' || data[i-1]=='\n')
    {
      continue;
    }

    if (i+2<size && data[i+1]==c && data[i+2]==c)
    {
      out+="<em><strong>";
      processInline(data.substr(0,i));
      out+="</strong></em>";
      AUTO_TRACE_EXIT("result={}",i+3);
      return static_cast<int>(i+3);
    }
    else if (i+1<size && data[i+1]==c)
    {
      // double symbol found, handing over to emph1
      len = processEmphasis1(std::string_view(data.data()-2, size+2), c);
      if (len==0)
      {
        return 0;
      }
      else
      {
        AUTO_TRACE_EXIT("result={}",len-2);
        return static_cast<int>(len - 2);
      }
    }
    else
    {
      // single symbol found, handing over to emph2
      len = processEmphasis2(std::string_view(data.data()-1, size+1), c);
      if (len==0)
      {
        return 0;
      }
      else
      {
        AUTO_TRACE_EXIT("result={}",len-1);
        return static_cast<int>(len - 1);
      }
    }
  }
  return 0;
}

/** Process ndash and mdashes */
int Markdown::Private::processNmdash(std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);
  const size_t size = data.size();
  // precondition: data[0]=='-'
  size_t i=1;
  int count=1;
  if (i<size && data[i]=='-') // found --
  {
    count++,i++;
  }
  if (i<size && data[i]=='-') // found ---
  {
    count++,i++;
  }
  if (i<size && data[i]=='-') // found ----
  {
    count++;
  }
  if (count>=2 && offset>=2 && qstrncmp(data.data()-2,"<!",2)==0)
  { AUTO_TRACE_EXIT("result={}",1-count); return 1-count; } // start HTML comment
  if (count==2 && size > 2 && data[2]=='>')
  { return 0; } // end HTML comment
  if (count==3 && size > 3 && data[3]=='>')
  { return 0; } // end HTML comment
  if (count==2 && (offset<8 || qstrncmp(data.data()-8,"operator",8)!=0)) // -- => ndash
  {
    out+="&ndash;";
    AUTO_TRACE_EXIT("result=2");
    return 2;
  }
  else if (count==3) // --- => ndash
  {
    out+="&mdash;";
    AUTO_TRACE_EXIT("result=3");
    return 3;
  }
  // not an ndash or mdash
  return 0;
}

/** Process quoted section "...", can contain one embedded newline */
int Markdown::Private::processQuoted(std::string_view data,size_t)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  const size_t size = data.size();
  size_t i=1;
  int nl=0;
  while (i<size && data[i]!='"' && nl<2)
  {
    if (data[i]=='\n') nl++;
    i++;
  }
  if (i<size && data[i]=='"' && nl<2)
  {
    out+=data.substr(0,i+1);
    AUTO_TRACE_EXIT("result={}",i+2);
    return static_cast<int>(i+1);
  }
  // not a quoted section
  return 0;
}

/** Process a HTML tag. Note that <pre>..</pre> are treated specially, in
 *  the sense that all code inside is written unprocessed
 */
int Markdown::Private::processHtmlTagWrite(std::string_view data,size_t offset,bool doWrite)
{
  AUTO_TRACE("data='{}' offset={} doWrite={}",Trace::trunc(data),offset,doWrite);
  if (offset>0 && data.data()[-1]=='\\') { return 0; } // escaped <

  const size_t size = data.size();

  // find the end of the html tag
  size_t i=1;
  size_t l=0;
  // compute length of the tag name
  while (i<size && isIdChar(data[i])) i++,l++;
  QCString tagName(data.substr(1,i-1));
  if (tagName.lower()=="pre") // found <pre> tag
  {
    bool insideStr=FALSE;
    while (i+6<size)
    {
      char c=data[i];
      if (!insideStr && c=='<') // potential start of html tag
      {
        if (data[i+1]=='/' &&
            tolower(data[i+2])=='p' && tolower(data[i+3])=='r' &&
            tolower(data[i+4])=='e' && tolower(data[i+5])=='>')
        { // found </pre> tag, copy from start to end of tag
          if (doWrite) out+=data.substr(0,i+6);
          //printf("found <pre>..</pre> [%d..%d]\n",0,i+6);
          AUTO_TRACE_EXIT("result={}",i+6);
          return static_cast<int>(i+6);
        }
      }
      else if (insideStr && c=='"')
      {
        if (data[i-1]!='\\') insideStr=FALSE;
      }
      else if (c=='"')
      {
        insideStr=TRUE;
      }
      i++;
    }
  }
  else // some other html tag
  {
    if (l>0 && i<size)
    {
      if (data[i]=='/' && i<size-1 && data[i+1]=='>') // <bla/>
      {
        //printf("Found htmlTag={%s}\n",qPrint(QCString(data).left(i+2)));
        if (doWrite) out+=data.substr(0,i+2);
        AUTO_TRACE_EXIT("result={}",i+2);
        return static_cast<int>(i+2);
      }
      else if (data[i]=='>') // <bla>
      {
        //printf("Found htmlTag={%s}\n",qPrint(QCString(data).left(i+1)));
        if (doWrite) out+=data.substr(0,i+1);
        AUTO_TRACE_EXIT("result={}",i+1);
        return static_cast<int>(i+1);
      }
      else if (data[i]==' ') // <bla attr=...
      {
        i++;
        bool insideAttr=FALSE;
        while (i<size)
        {
          if (!insideAttr && data[i]=='"')
          {
            insideAttr=TRUE;
          }
          else if (data[i]=='"' && data[i-1]!='\\')
          {
            insideAttr=FALSE;
          }
          else if (!insideAttr && data[i]=='>') // found end of tag
          {
            //printf("Found htmlTag={%s}\n",qPrint(QCString(data).left(i+1)));
            if (doWrite) out+=data.substr(0,i+1);
            AUTO_TRACE_EXIT("result={}",i+1);
            return static_cast<int>(i+1);
          }
          i++;
        }
      }
    }
  }
  AUTO_TRACE_EXIT("not a valid html tag");
  return 0;
}

int Markdown::Private::processHtmlTag(std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);
  return processHtmlTagWrite(data,offset,true);
}

int Markdown::Private::processEmphasis(std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);
  const size_t size = data.size();

  if ((offset>0 && !isOpenEmphChar(data.data()[-1])) || // invalid char before * or _
      (size>1 && data[0]!=data[1] && !(isIdChar(data[1]) || extraChar(data[1]))) || // invalid char after * or _
      (size>2 && data[0]==data[1] && !(isIdChar(data[2]) || extraChar(data[2]))))   // invalid char after ** or __
  {
    AUTO_TRACE_EXIT("invalid surrounding characters");
    return 0;
  }

  char c = data[0];
  int ret = 0;
  if (size>2 && c!='~' && data[1]!=c) // _bla or *bla
  {
    // whitespace cannot follow an opening emphasis
    if (data[1]==' ' || data[1]=='\n' ||
        (ret = processEmphasis1(data.substr(1), c)) == 0)
    {
      return 0;
    }
    AUTO_TRACE_EXIT("result={}",ret+1);
    return ret+1;
  }
  if (size>3 && data[1]==c && data[2]!=c) // __bla or **bla
  {
    if (data[2]==' ' || data[2]=='\n' ||
        (ret = processEmphasis2(data.substr(2), c)) == 0)
    {
      return 0;
    }
    AUTO_TRACE_EXIT("result={}",ret+2);
    return ret+2;
  }
  if (size>4 && c!='~' && data[1]==c && data[2]==c && data[3]!=c) // ___bla or ***bla
  {
    if (data[3]==' ' || data[3]=='\n' ||
        (ret = processEmphasis3(data.substr(3), c)) == 0)
    {
      return 0;
    }
    AUTO_TRACE_EXIT("result={}",ret+3);
    return ret+3;
  }
  return 0;
}

void Markdown::Private::writeMarkdownImage(
                                  std::string_view fmt, bool inline_img, bool explicitTitle,
                                  const QCString &title, const QCString &content,
                                  const QCString &link, const QCString &attrs,
                                  const FileDef *fd)
{
  AUTO_TRACE("fmt={} inline_img={} explicitTitle={} title={} content={} link={} attrs={}",
              fmt,inline_img,explicitTitle,Trace::trunc(title),Trace::trunc(content),link,attrs);
  QCString attributes = getFilteredImageAttributes(fmt, attrs);
  out+="@image";
  if (inline_img)
  {
    out+="{inline}";
  }
  out+=" ";
  out+=fmt;
  out+=" ";
  out+=link.mid(fd ? 0 : 5);
  if (!explicitTitle && !content.isEmpty())
  {
    out+=" \"";
    out+=escapeDoubleQuotes(content);
    out+="\"";
  }
  else if ((content.isEmpty() || explicitTitle) && !title.isEmpty())
  {
    out+=" \"";
    out+=escapeDoubleQuotes(title);
    out+="\"";
  }
  else
  {
    out+=" ";// so the line break will not be part of the image name
  }
  if (!attributes.isEmpty())
  {
    out+=" ";
    out+=attributes;
    out+=" ";
  }
  out+="\\ilinebr ";
}

int Markdown::Private::processLink(const std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);
  const size_t size = data.size();
  QCString content;
  QCString link;
  QCString title;
  bool isImageLink = FALSE;
  bool isImageInline = FALSE;
  bool isToc = FALSE;
  size_t i=1;
  if (data[0]=='!')
  {
    isImageLink = TRUE;
    if (size<2 || data[1]!='[')
    {
      return 0;
    }

    // if there is non-whitespace before the ![ within the scope of two new lines, the image
    // is considered inlined, i.e. the image is not preceded by an empty line
    int numNLsNeeded=2;
    int pos = -1;
    while (pos>=-static_cast<int>(offset) && numNLsNeeded>0)
    {
      if (data.data()[pos]=='\n') numNLsNeeded--;
      else if (data.data()[pos]!=' ') // found non-whitespace, stop searching
      {
        isImageInline=true;
        break;
      }
      pos--;
    }
    // skip '!['
    i++;
  }
  size_t contentStart=i;
  int level=1;
  int nlTotal=0;
  int nl=0;
  // find the matching ]
  while (i<size)
  {
    if (data[i-1]=='\\') // skip escaped characters
    {
    }
    else if (data[i]=='[')
    {
      level++;
    }
    else if (data[i]==']')
    {
      level--;
      if (level<=0) break;
    }
    else if (data[i]=='\n')
    {
      nl++;
      if (nl>1) { return 0; } // only allow one newline in the content
    }
    i++;
  }
  nlTotal += nl;
  nl = 0;
  if (i>=size) return 0; // premature end of comment -> no link
  size_t contentEnd=i;
  content = data.substr(contentStart,contentEnd-contentStart);
  //printf("processLink: content={%s}\n",qPrint(content));
  if (!isImageLink && content.isEmpty()) { return 0; } // no link text
  i++; // skip over ]

  bool whiteSpace = false;
  // skip whitespace
  while (i<size && data[i]==' ') { whiteSpace = true; i++; }
  if (i<size && data[i]=='\n') // one newline allowed here
  {
    whiteSpace = true;
    i++;
    // skip more whitespace
    while (i<size && data[i]==' ') i++;
  }
  if (whiteSpace && i<size && (data[i]=='(' || data[i]=='[')) return 0;

  bool explicitTitle=FALSE;
  if (i<size && data[i]=='(') // inline link
  {
    i++;
    while (i<size && data[i]==' ') i++;
    bool uriFormat=false;
    if (i<size && data[i]=='<') { i++; uriFormat=true; }
    size_t linkStart=i;
    int braceCount=1;
    while (i<size && data[i]!='\'' && data[i]!='"' && braceCount>0)
    {
      if (data[i]=='\n') // unexpected EOL
      {
        nl++;
        if (nl>1) { return 0; }
      }
      else if (data[i]=='(')
      {
        braceCount++;
      }
      else if (data[i]==')')
      {
        braceCount--;
      }
      if (braceCount>0)
      {
        i++;
      }
    }
    nlTotal += nl;
    nl = 0;
    if (i>=size || data[i]=='\n') { return 0; }
    link = data.substr(linkStart,i-linkStart);
    link = link.stripWhiteSpace();
    //printf("processLink: link={%s}\n",qPrint(link));
    if (link.isEmpty()) { return 0; }
    if (uriFormat && link.at(link.length()-1)=='>') link=link.left(link.length()-1);

    // optional title
    if (data[i]=='\'' || data[i]=='"')
    {
      char c = data[i];
      i++;
      size_t titleStart=i;
      nl=0;
      while (i<size)
      {
        if (data[i]=='\n')
        {
          if (nl>1) { return 0; }
          nl++;
        }
        else if (data[i]=='\\') // escaped char in string
        {
          i++;
        }
        else if (data[i]==c)
        {
          i++;
          break;
        }
        i++;
      }
      if (i>=size)
      {
        return 0;
      }
      size_t titleEnd = i-1;
      // search back for closing marker
      while (titleEnd>titleStart && data[titleEnd]==' ') titleEnd--;
      if (data[titleEnd]==c) // found it
      {
        title = data.substr(titleStart,titleEnd-titleStart);
        explicitTitle=TRUE;
        while (i<size)
        {
          if (data[i]==' ')i++; // remove space after the closing quote and the closing bracket
          else if (data[i] == ')') break; // the end bracket
          else // illegal
          {
            return 0;
          }
        }
      }
      else
      {
        return 0;
      }
    }
    i++;
  }
  else if (i<size && data[i]=='[') // reference link
  {
    i++;
    size_t linkStart=i;
    nl=0;
    // find matching ]
    while (i<size && data[i]!=']')
    {
      if (data[i]=='\n')
      {
        nl++;
        if (nl>1) { return 0; }
      }
      i++;
    }
    if (i>=size) { return 0; }
    // extract link
    link = data.substr(linkStart,i-linkStart);
    //printf("processLink: link={%s}\n",qPrint(link));
    link = link.stripWhiteSpace();
    if (link.isEmpty()) // shortcut link
    {
      link=content;
    }
    // lookup reference
    QCString link_lower = link.lower();
    auto lr_it=linkRefs.find(link_lower.str());
    if (lr_it!=linkRefs.end()) // found it
    {
      link  = lr_it->second.link;
      title = lr_it->second.title;
      //printf("processLink: ref: link={%s} title={%s}\n",qPrint(link),qPrint(title));
    }
    else // reference not found!
    {
      //printf("processLink: ref {%s} do not exist\n",link.qPrint(lower()));
      return 0;
    }
    i++;
  }
  else if (i<size && data[i]!=':' && !content.isEmpty()) // minimal link ref notation [some id]
  {
    QCString content_lower = content.lower();
    auto lr_it = linkRefs.find(content_lower.str());
    //printf("processLink: minimal link {%s} lr=%p",qPrint(content),lr);
    if (lr_it!=linkRefs.end()) // found it
    {
      link  = lr_it->second.link;
      title = lr_it->second.title;
      explicitTitle=TRUE;
      i=contentEnd;
    }
    else if (content=="TOC")
    {
      isToc=TRUE;
      i=contentEnd;
    }
    else
    {
      return 0;
    }
    i++;
  }
  else
  {
    return 0;
  }
  nlTotal += nl;

  // search for optional image attributes
  QCString attributes;
  if (isImageLink)
  {
    size_t j = i;
    // skip over whitespace
    while (j<size && data[j]==' ') { j++; }
    if (j<size && data[j]=='{') // we have attributes
    {
      i = j;
      // skip over '{'
      i++;
      size_t attributesStart=i;
      nl=0;
      // find the matching '}'
      while (i<size)
      {
        if (data[i-1]=='\\') // skip escaped characters
        {
        }
        else if (data[i]=='{')
        {
          level++;
        }
        else if (data[i]=='}')
        {
          level--;
          if (level<=0) break;
        }
        else if (data[i]=='\n')
        {
          nl++;
          if (nl>1) { return 0; } // only allow one newline in the content
        }
        i++;
      }
      nlTotal += nl;
      if (i>=size) return 0; // premature end of comment -> no attributes
      size_t attributesEnd=i;
      attributes = data.substr(attributesStart,attributesEnd-attributesStart);
      i++; // skip over '}'
    }
    if (!isImageInline)
    {
      // if there is non-whitespace after the image within the scope of two new lines, the image
      // is considered inlined, i.e. the image is not followed by an empty line
      int numNLsNeeded=2;
      size_t pos = i;
      while (pos<size && numNLsNeeded>0)
      {
        if (data[pos]=='\n') numNLsNeeded--;
        else if (data[pos]!=' ') // found non-whitespace, stop searching
        {
          isImageInline=true;
          break;
        }
        pos++;
      }
    }
  }

  if (isToc) // special case for [TOC]
  {
    int toc_level = Config_getInt(TOC_INCLUDE_HEADINGS);
    if (toc_level>=SectionType::MinLevel && toc_level<=SectionType::MaxLevel)
    {
      out+="@tableofcontents{html:";
      out+=QCString().setNum(toc_level);
      out+="}";
    }
  }
  else if (isImageLink)
  {
    bool ambig = false;
    FileDef *fd=nullptr;
    if (link.find("@ref ")!=-1 || link.find("\\ref ")!=-1 ||
        (fd=findFileDef(Doxygen::imageNameLinkedMap,link,ambig)))
        // assume doxygen symbol link or local image link
    {
      // check if different handling is needed per format
      writeMarkdownImage("html",    isImageInline, explicitTitle, title, content, link, attributes, fd);
      writeMarkdownImage("latex",   isImageInline, explicitTitle, title, content, link, attributes, fd);
      writeMarkdownImage("rtf",     isImageInline, explicitTitle, title, content, link, attributes, fd);
      writeMarkdownImage("docbook", isImageInline, explicitTitle, title, content, link, attributes, fd);
      writeMarkdownImage("xml",     isImageInline, explicitTitle, title, content, link, attributes, fd);
    }
    else
    {
      out+="<img src=\"";
      out+=link;
      out+="\" alt=\"";
      out+=content;
      out+="\"";
      if (!title.isEmpty())
      {
        out+=" title=\"";
        out+=substitute(title.simplifyWhiteSpace(),"\"","&quot;");
        out+="\"";
      }
      out+="/>";
    }
  }
  else
  {
    SrcLangExt lang = getLanguageFromFileName(link);
    int lp=-1;
    if ((lp=link.find("@ref "))!=-1 || (lp=link.find("\\ref "))!=-1 || (lang==SrcLangExt::Markdown && !isURL(link)))
        // assume doxygen symbol link
    {
      if (lp==-1) // link to markdown page
      {
        out+="@ref \"";
        if (!(Portable::isAbsolutePath(link) || isURL(link)))
        {
          FileInfo forg(link.str());
          if (forg.exists() && forg.isReadable())
          {
            link = forg.absFilePath();
          }
          else if (!(forg.exists() && forg.isReadable()))
          {
            FileInfo fi(fileName.str());
            QCString mdFile = fileName.left(fileName.length()-fi.fileName().length()) + link;
            FileInfo fmd(mdFile.str());
            if (fmd.exists() && fmd.isReadable())
            {
              link = fmd.absFilePath().data();
            }
          }
        }
        out+=link;
        out+="\"";
      }
      else
      {
        out+=link;
      }
      out+=" \"";
      if (explicitTitle && !title.isEmpty())
      {
        out+=substitute(title,"\"","&quot;");
      }
      else
      {
        processInline(std::string_view(substitute(content,"\"","&quot;").str()));
      }
      out+="\"";
    }
    else if ((lp=link.find('#'))!=-1 || link.find('/')!=-1 || link.find('.')!=-1)
    { // file/url link
      if (lp==0 || (lp>0 && !isURL(link) && Config_getEnum(MARKDOWN_ID_STYLE)==MARKDOWN_ID_STYLE_t::GITHUB))
      {
        out+="@ref \"";
        out+=AnchorGenerator::addPrefixIfNeeded(link.mid(lp+1).str());
        out+="\" \"";
        out+=substitute(content.simplifyWhiteSpace(),"\"","&quot;");
        out+="\"";
      }
      else
      {
        out+="<a href=\"";
        out+=link;
        out+="\"";
        for (int ii = 0; ii < nlTotal; ii++) out+="\n";
        if (!title.isEmpty())
        {
          out+=" title=\"";
          out+=substitute(title.simplifyWhiteSpace(),"\"","&quot;");
          out+="\"";
        }
        out+=" ";
        out+=externalLinkTarget();
        out+=">";
        content = content.simplifyWhiteSpace();
        processInline(std::string_view(content.str()));
        out+="</a>";
      }
    }
    else // avoid link to e.g. F[x](y)
    {
      //printf("no link for '%s'\n",qPrint(link));
      return 0;
    }
  }
  AUTO_TRACE_EXIT("result={}",i);
  return static_cast<int>(i);
}

/** `` ` `` parsing a code span (assuming codespan != 0) */
int Markdown::Private::processCodeSpan(std::string_view data,size_t)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  const size_t size = data.size();

  /* counting the number of backticks in the delimiter */
  size_t nb=0, end=0;
  while (nb<size && data[nb]=='`')
  {
    nb++;
  }

  /* finding the next delimiter */
  size_t i = 0;
  char pc = '`';
  for (end=nb; end<size && i<nb; end++)
  {
    if (data[end]=='`')
    {
      i++;
    }
    else if (data[end]=='\n')
    {
      // consecutive newlines
      if (pc == '\n') return 0;
      pc = '\n';
      i = 0;
    }
    else if (data[end]=='\'' && nb==1 && (end==size-1 || (end<size-1 && !isIdChar(data[end+1]))))
    { // look for quoted strings like 'some word', but skip strings like `it's cool`
      out+="&lsquo;";
      out+=data.substr(nb,end-nb);
      out+="&rsquo;";
      return static_cast<int>(end+1);
    }
    else
    {
      if (data[end]!=' ') pc = data[end];
      i=0;
    }
  }
  if (i < nb && end >= size)
  {
    return 0;  // no matching delimiter
  }

  // trimming outside whitespaces
  size_t f_begin = nb;
  while (f_begin < end && data[f_begin]==' ')
  {
    f_begin++;
  }
  size_t f_end = end - nb;
  while (f_end > nb && data[f_end-1]==' ')
  {
    f_end--;
  }

  //printf("found code span '%s'\n",qPrint(QCString(data+f_begin).left(f_end-f_begin)));

  /* real code span */
  if (f_begin < f_end)
  {
    QCString codeFragment = data.substr(f_begin, f_end-f_begin);
    out+="<tt>";
    out+=escapeSpecialChars(codeFragment);
    out+="</tt>";
  }
  AUTO_TRACE_EXIT("result={}",end);
  return static_cast<int>(end);
}

void Markdown::Private::addStrEscapeUtf8Nbsp(std::string_view data)
{
  AUTO_TRACE("{}",Trace::trunc(data));
  if (Portable::strnstr(data.data(),g_doxy_nbsp,data.size())==nullptr) // no escape needed -> fast
  {
    out+=data;
  }
  else // escape needed -> slow
  {
    out+=substitute(QCString(data),g_doxy_nbsp,g_utf8_nbsp);
  }
}

int Markdown::Private::processSpecialCommand(std::string_view data, size_t offset)
{
  AUTO_TRACE("{}",Trace::trunc(data));
  const size_t size = data.size();
  size_t i=1;
  QCString endBlockName = isBlockCommand(data,offset);
  if (!endBlockName.isEmpty())
  {
    AUTO_TRACE_ADD("endBlockName={}",endBlockName);
    size_t l = endBlockName.length();
    while (i+l<size)
    {
      if ((data[i]=='\\' || data[i]=='@') && // command
          data[i-1]!='\\' && data[i-1]!='@') // not escaped
      {
        if (qstrncmp(&data[i+1],endBlockName.data(),l)==0)
        {
          //printf("found end at %d\n",i);
          addStrEscapeUtf8Nbsp(data.substr(0,i+1+l));
          AUTO_TRACE_EXIT("result={}",i+1+l);
          return static_cast<int>(i+1+l);
        }
      }
      i++;
    }
  }
  size_t endPos = isSpecialCommand(data,offset);
  if (endPos>0)
  {
    out+=data.substr(0,endPos);
    return static_cast<int>(endPos);
  }
  if (size>1 && data[0]=='\\') // escaped characters
  {
    char c=data[1];
    if (c=='[' || c==']' || c=='*' || c=='!' || c=='(' || c==')' || c=='`' || c=='_')
    {
      out+=data[1];
      AUTO_TRACE_EXIT("2");
      return 2;
    }
    else if (c=='\\' || c=='@')
    {
      out+=data.substr(0,2);
      AUTO_TRACE_EXIT("2");
      return 2;
    }
    else if (c=='-' && size>3 && data[2]=='-' && data[3]=='-') // \---
    {
      out+=data.substr(1,3);
      AUTO_TRACE_EXIT("2");
      return 4;
    }
    else if (c=='-' && size>2 && data[2]=='-') // \--
    {
      out+=data.substr(1,2);
      AUTO_TRACE_EXIT("3");
      return 3;
    }
  }
  else if (size>1 && data[0]=='@') // escaped characters
  {
    char c=data[1];
    if (c=='\\' || c=='@')
    {
      out+=data.substr(0,2);
      AUTO_TRACE_EXIT("2");
      return 2;
    }
  }
  return 0;
}

void Markdown::Private::processInline(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t i=0;
  size_t end=0;
  Action_t action;
  const size_t size = data.size();
  while (i<size)
  {
    // skip over characters that do not trigger a specific action
    while (end<size && ((action=actions[static_cast<uint8_t>(data[end])])==nullptr)) end++;
    // and add them to the output
    out+=data.substr(i,end-i);
    if (end>=size) break;
    i=end;
    // do the action matching a special character at i
    int iend = action(data.substr(i),i);
    if (iend<=0) // update end
    {
      end=i+1-iend;
    }
    else // skip until end
    {
      i+=iend;
      end=i;
    }
  }
}

/** returns whether the line is a setext-style hdr underline */
int Markdown::Private::isHeaderline(std::string_view data, bool allowAdjustLevel)
{
  AUTO_TRACE("data='{}' allowAdjustLevel",Trace::trunc(data),allowAdjustLevel);
  size_t i=0, c=0;
  const size_t size = data.size();
  while (i<size && data[i]==' ') i++;
  if (i==size) return 0;

  // test of level 1 header
  if (data[i]=='=')
  {
    while (i<size && data[i]=='=') i++,c++;
    while (i<size && data[i]==' ') i++;
    int level = (c>1 && (i>=size || data[i]=='\n')) ? 1 : 0;
    if (allowAdjustLevel && level==1 && indentLevel==-1)
    {
      // In case a page starts with a header line we use it as title, promoting it to @page.
      // We set g_indentLevel to -1 to promoting the other sections if they have a deeper
      // nesting level than the page header, i.e. @section..@subsection becomes @page..@section.
      // In case a section at the same level is found (@section..@section) however we need
      // to undo this (and the result will be @page..@section).
      indentLevel=0;
    }
    AUTO_TRACE_EXIT("result={}",indentLevel+level);
    return indentLevel+level;
  }
  // test of level 2 header
  if (data[i]=='-')
  {
    while (i<size && data[i]=='-') i++,c++;
    while (i<size && data[i]==' ') i++;
    return (c>1 && (i>=size || data[i]=='\n')) ? indentLevel+2 : 0;
  }
  return 0;
}

/** returns true if this line starts a block quote */
static bool isBlockQuote(std::string_view data,size_t indent)
{
  AUTO_TRACE("data='{}' indent={}",Trace::trunc(data),indent);
  size_t i = 0;
  const size_t size = data.size();
  while (i<size && data[i]==' ') i++;
  if (i<indent+codeBlockIndent) // could be a quotation
  {
    // count >'s and skip spaces
    int level=0;
    while (i<size && (data[i]=='>' || data[i]==' '))
    {
      if (data[i]=='>') level++;
      i++;
    }
    // last characters should be a space or newline,
    // so a line starting with >= does not match, but only when level equals 1
    bool res = (level>0 && i<size && ((data[i-1]==' ') || data[i]=='\n')) || (level > 1);
    AUTO_TRACE_EXIT("result={}",res);
    return res;
  }
  else // too much indentation -> code block
  {
    AUTO_TRACE_EXIT("result=false: too much indentation");
    return false;
  }
}

/** returns end of the link ref if this is indeed a link reference. */
static size_t isLinkRef(std::string_view data, QCString &refid, QCString &link, QCString &title)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  const size_t size = data.size();
  // format: start with [some text]:
  size_t i = 0;
  while (i<size && data[i]==' ') i++;
  if (i>=size || data[i]!='[') { return 0; }
  i++;
  size_t refIdStart=i;
  while (i<size && data[i]!='\n' && data[i]!=']') i++;
  if (i>=size || data[i]!=']') { return 0; }
  refid = data.substr(refIdStart,i-refIdStart);
  if (refid.isEmpty()) { return 0; }
  AUTO_TRACE_ADD("refid found {}",refid);
  //printf("  isLinkRef: found refid='%s'\n",qPrint(refid));
  i++;
  if (i>=size || data[i]!=':') { return 0; }
  i++;

  // format: whitespace* \n? whitespace* (<url> | url)
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n')
  {
    i++;
    while (i<size && data[i]==' ') i++;
  }
  if (i>=size) { return 0; }

  if (i<size && data[i]=='<') i++;
  size_t linkStart=i;
  while (i<size && data[i]!=' ' && data[i]!='\n') i++;
  size_t linkEnd=i;
  if (i<size && data[i]=='>') i++;
  if (linkStart==linkEnd) { return 0; } // empty link
  link = data.substr(linkStart,linkEnd-linkStart);
  AUTO_TRACE_ADD("link found {}",Trace::trunc(link));
  if (link=="@ref" || link=="\\ref")
  {
    size_t argStart=i;
    while (i<size && data[i]!='\n' && data[i]!='"') i++;
    link+=data.substr(argStart,i-argStart);
  }

  title.clear();

  // format: (whitespace* \n? whitespace* ( 'title' | "title" | (title) ))?
  size_t eol=0;
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n')
  {
    eol=i;
    i++;
    while (i<size && data[i]==' ') i++;
  }
  if (i>=size)
  {
    AUTO_TRACE_EXIT("result={}: end of isLinkRef while looking for title",i);
    return i; // end of buffer while looking for the optional title
  }

  char c = data[i];
  if (c=='\'' || c=='"' || c=='(') // optional title present?
  {
    //printf("  start of title found! char='%c'\n",c);
    i++;
    if (c=='(') c=')'; // replace c by end character
    size_t titleStart=i;
    // search for end of the line
    while (i<size && data[i]!='\n') i++;
    eol = i;

    // search back to matching character
    size_t end=i-1;
    while (end>titleStart && data[end]!=c) end--;
    if (end>titleStart)
    {
      title = data.substr(titleStart,end-titleStart);
    }
    AUTO_TRACE_ADD("title found {}",Trace::trunc(title));
  }
  while (i<size && data[i]==' ') i++;
  //printf("end of isLinkRef: i=%d size=%d data[i]='%c' eol=%d\n",
  //    i,size,data[i],eol);
  if      (i>=size)       { AUTO_TRACE_EXIT("result={}",i);   return i; }    // end of buffer while ref id was found
  else if (eol>0)         { AUTO_TRACE_EXIT("result={}",eol); return eol; }  // end of line while ref id was found
  return 0;                            // invalid link ref
}

static bool isHRuler(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t i=0;
  size_t size = data.size();
  if (size>0 && data[size-1]=='\n') size--; // ignore newline character
  while (i<size && data[i]==' ') i++;
  if (i>=size) { AUTO_TRACE_EXIT("result=false: empty line"); return false; } // empty line
  char c=data[i];
  if (c!='*' && c!='-' && c!='_')
  {
    AUTO_TRACE_EXIT("result=false: {} is not a hrule character",c);
    return false; // not a hrule character
  }
  int n=0;
  while (i<size)
  {
    if (data[i]==c)
    {
      n++; // count rule character
    }
    else if (data[i]!=' ')
    {
      AUTO_TRACE_EXIT("result=false: line contains non hruler characters");
      return false; // line contains non hruler characters
    }
    i++;
  }
  AUTO_TRACE_EXIT("result={}",n>=3);
  return n>=3; // at least 3 characters needed for a hruler
}

QCString Markdown::Private::extractTitleId(QCString &title, int level, bool *pIsIdGenerated)
{
  AUTO_TRACE("title={} level={}",Trace::trunc(title),level);
  // match e.g. '{#id-b11} ' and capture 'id-b11'
  static const reg::Ex r2(R"({#(\a[\w-]*)}\s*$)");
  reg::Match match;
  std::string ti = title.str();
  if (reg::search(ti,match,r2))
  {
    std::string id = match[1].str();
    title = title.left(match.position());
    if (AnchorGenerator::instance().reserve(id)>0)
    {
      warn(fileName, lineNr, "An automatically generated id already has the name '%s'!", id.c_str());
    }
    //printf("found match id='%s' title=%s\n",id.c_str(),qPrint(title));
    AUTO_TRACE_EXIT("id={}",id);
    return id;
  }
  if ((level>0) && (level<=Config_getInt(TOC_INCLUDE_HEADINGS)))
  {
    QCString id = AnchorGenerator::instance().generate(ti);
    if (pIsIdGenerated) *pIsIdGenerated=true;
    //printf("auto-generated id='%s' title='%s'\n",qPrint(id),qPrint(title));
    AUTO_TRACE_EXIT("id={}",id);
    return id;
  }
  //printf("no id found in title '%s'\n",qPrint(title));
  return "";
}


int Markdown::Private::isAtxHeader(std::string_view data,
                       QCString &header,QCString &id,bool allowAdjustLevel,bool *pIsIdGenerated)
{
  AUTO_TRACE("data='{}' header={} id={} allowAdjustLevel={}",Trace::trunc(data),Trace::trunc(header),id,allowAdjustLevel);
  size_t i = 0;
  int level = 0, blanks=0;
  const size_t size = data.size();

  // find start of header text and determine heading level
  while (i<size && data[i]==' ') i++;
  if (i>=size || data[i]!='#')
  {
    return 0;
  }
  while (i<size && data[i]=='#') i++,level++;
  if (level>SectionType::MaxLevel) // too many #'s -> no section
  {
    return 0;
  }
  while (i<size && data[i]==' ') i++,blanks++;
  if (level==1 && blanks==0)
  {
    return 0; // special case to prevent #someid seen as a header (see bug 671395)
  }

  // find end of header text
  size_t end=i;
  while (end<size && data[end]!='\n') end++;
  while (end>i && (data[end-1]=='#' || data[end-1]==' ')) end--;

  // store result
  header = data.substr(i,end-i);
  id = extractTitleId(header, level, pIsIdGenerated);
  if (!id.isEmpty()) // strip #'s between title and id
  {
    int idx=static_cast<int>(header.length())-1;
    while (idx>=0 && (header.at(idx)=='#' || header.at(idx)==' ')) idx--;
    header=header.left(idx+1);
  }

  if (allowAdjustLevel && level==1 && indentLevel==-1)
  {
    // in case we find a `# Section` on a markdown page that started with the same level
    // header, we no longer need to artificially decrease the paragraph level.
    // So both
    // -------------------
    // # heading 1    <-- here we set g_indentLevel to -1
    // # heading 2    <-- here we set g_indentLevel back to 0 such that this will be a @section
    // -------------------
    // and
    // -------------------
    // # heading 1    <-- here we set  g_indentLevel to -1
    // ## heading 2   <-- here we keep g_indentLevel at -1 such that @subsection will be @section
    // -------------------
    // will convert to
    // -------------------
    // @page md_page Heading 1
    // @section autotoc_md1 Heading 2
    // -------------------

    indentLevel=0;
  }
  int res = level+indentLevel;
  AUTO_TRACE_EXIT("result={}",res);
  return res;
}

static bool isEmptyLine(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t i=0;
  while (i<data.size())
  {
    if (data[i]=='\n') { AUTO_TRACE_EXIT("true");  return true; }
    if (data[i]!=' ')  { AUTO_TRACE_EXIT("false"); return false; }
    i++;
  }
  AUTO_TRACE_EXIT("true");
  return true;
}

#define isLiTag(i) \
   (data[(i)]=='<' && \
   (data[(i)+1]=='l' || data[(i)+1]=='L') && \
   (data[(i)+2]=='i' || data[(i)+2]=='I') && \
   (data[(i)+3]=='>'))

// compute the indent from the start of the input, excluding list markers
// such as -, -#, *, +, 1., and <li>
static size_t computeIndentExcludingListMarkers(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t i=0;
  const size_t size=data.size();
  size_t indent=0;
  bool isDigit=FALSE;
  bool isLi=FALSE;
  bool listMarkerSkipped=FALSE;
  while (i<size &&
         (data[i]==' ' ||                                    // space
          (!listMarkerSkipped &&                             // first list marker
           (data[i]=='+' || data[i]=='-' || data[i]=='*' ||  // unordered list char
            (data[i]=='#' && i>0 && data[i-1]=='-') ||       // -# item
            (isDigit=(data[i]>='1' && data[i]<='9')) ||      // ordered list marker?
            (isLi=(size>=3 && i<size-3 && isLiTag(i)))       // <li> tag
           )
          )
         )
        )
  {
    if (isDigit) // skip over ordered list marker '10. '
    {
      size_t j=i+1;
      while (j<size && ((data[j]>='0' && data[j]<='9') || data[j]=='.'))
      {
        if (data[j]=='.') // should be end of the list marker
        {
          if (j<size-1 && data[j+1]==' ') // valid list marker
          {
            listMarkerSkipped=TRUE;
            indent+=j+1-i;
            i=j+1;
            break;
          }
          else // not a list marker
          {
            break;
          }
        }
        j++;
      }
    }
    else if (isLi)
    {
      i+=3; // skip over <li>
      indent+=3;
      listMarkerSkipped=TRUE;
    }
    else if (data[i]=='-' && size>=2 && i<size-2 && data[i+1]=='#' && data[i+2]==' ')
    { // case "-# "
      listMarkerSkipped=TRUE; // only a single list marker is accepted
      i++; // skip over #
      indent++;
    }
    else if (data[i]!=' ' && i<size-1 && data[i+1]==' ')
    { // case "- " or "+ " or "* "
      listMarkerSkipped=TRUE; // only a single list marker is accepted
    }
    if (data[i]!=' ' && !listMarkerSkipped)
    { // end of indent
      break;
    }
    indent++,i++;
  }
  AUTO_TRACE_EXIT("result={}",indent);
  return indent;
}

static size_t isListMarker(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t normalIndent = 0;
  while (normalIndent<data.size() && data[normalIndent]==' ') normalIndent++;
  size_t listIndent = computeIndentExcludingListMarkers(data);
  size_t result = listIndent>normalIndent ? listIndent : 0;
  AUTO_TRACE_EXIT("result={}",result);
  return result;
}

static bool isEndOfList(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  int dots=0;
  size_t i=0;
  // end of list marker is an otherwise empty line with a dot.
  while (i<data.size())
  {
    if (data[i]=='.')
    {
      dots++;
    }
    else if (data[i]=='\n')
    {
      break;
    }
    else if (data[i]!=' ' && data[i]!='\t') // bail out if the line is not empty
    {
      AUTO_TRACE_EXIT("result=false");
      return false;
    }
    i++;
  }
  AUTO_TRACE_EXIT("result={}",dots==1);
  return dots==1;
}

static bool isFencedCodeBlock(std::string_view data,size_t refIndent,
                             QCString &lang,size_t &start,size_t &end,size_t &offset)
{
  AUTO_TRACE("data='{}' refIndent={}",Trace::trunc(data),refIndent);
  auto isWordChar = [ ](char c) { return (c>='A' && c<='Z') || (c>='a' && c<='z'); };
  auto isLangChar = [&](char c) { return c=='.' || isWordChar(c);                  };
  // rules: at least 3 ~~~, end of the block same amount of ~~~'s, otherwise
  // return FALSE
  size_t i=0;
  size_t indent=0;
  int startTildes=0;
  const size_t size = data.size();
  while (i<size && data[i]==' ') indent++,i++;
  if (indent>=refIndent+4)
  {
    AUTO_TRACE_EXIT("result=false: content is part of code block indent={} refIndent={}",indent,refIndent);
    return FALSE;
  } // part of code block
  char tildaChar='~';
  if (i<size && data[i]=='`') tildaChar='`';
  while (i<size && data[i]==tildaChar) startTildes++,i++;
  if (startTildes<3)
  {
    AUTO_TRACE_EXIT("result=false: no fence marker found #tildes={}",startTildes);
    return FALSE;
  } // not enough tildes
  if (i<size && data[i]=='{') // extract .py from ```{.py} ... ```
  {
    i++; // skip over {
    size_t startLang=i;
    while (i<size && (data[i]!='\n' && data[i]!='}')) i++; // find matching }
    if (i<size && data[i]=='}')
    {
      lang = data.substr(startLang,i-startLang);
      i++;
    }
    else // missing closing bracket, treat `{` as part of the content
    {
      i=startLang-1;
      lang="";
    }
  }
  else if (i<size && isLangChar(data[i])) /// extract python or .py from ```python...``` or ```.py...```
  {
    size_t startLang=i;
    i++;
    while (i<size && isWordChar(data[i])) i++; // find end of language specifier
    lang = data.substr(startLang,i-startLang);
  }
  else // no language specified
  {
    lang="";
  }
  start=i;
  while (i<size)
  {
    if (data[i]==tildaChar)
    {
      end=i;
      int endTildes=0;
      while (i<size && data[i]==tildaChar) endTildes++,i++;
      while (i<size && data[i]==' ') i++;
      {
        if (endTildes==startTildes)
        {
          offset=i;
          AUTO_TRACE_EXIT("result=true: found end marker at offset {} lang='{}'",offset,lang);
          return true;
        }
      }
    }
    i++;
  }
  AUTO_TRACE_EXIT("result=false: no end marker found lang={}'",lang);
  return false;
}

static bool isCodeBlock(std::string_view data, size_t offset,size_t &indent)
{
  AUTO_TRACE("data='{}' offset={}",Trace::trunc(data),offset);
  //printf("<isCodeBlock(offset=%d,size=%d,indent=%d)\n",offset,size,indent);
  // determine the indent of this line
  size_t i=0;
  size_t indent0=0;
  const size_t size = data.size();
  while (i<size && data[i]==' ') indent0++,i++;

  if (indent0<codeBlockIndent)
  {
    AUTO_TRACE_EXIT("result={}: line is not indented enough {}<4",false,indent0);
    return false;
  }
  if (indent0>=size || data[indent0]=='\n') // empty line does not start a code block
  {
    AUTO_TRACE_EXIT("result={}: only spaces at the end of a comment block",false);
    return false;
  }

  i=offset;
  int nl=0;
  int nl_pos[3];
  int offset_i = static_cast<int>(offset);
  // search back 3 lines and remember the start of lines -1 and -2
  while (i>0 && nl<3) // i counts down from offset to 1
  {
    int j = static_cast<int>(i)-offset_i-1; // j counts from -1 to -offset
    // since j can be negative we need to rewrap data in a std::string_view
    size_t nl_size = isNewline(std::string_view(data.data()+j,data.size()-j));
    if (nl_size>0)
    {
      nl_pos[nl++]=j+static_cast<int>(nl_size);
    }
    i--;
  }

  // if there are only 2 preceding lines, then line -2 starts at -offset
  if (i==0 && nl==2) nl_pos[nl++]=-offset_i;

  if (nl==3) // we have at least 2 preceding lines
  {
    //printf("  positions: nl_pos=[%d,%d,%d] line[-2]='%s' line[-1]='%s'\n",
    //    nl_pos[0],nl_pos[1],nl_pos[2],
    //    qPrint(QCString(data+nl_pos[1]).left(nl_pos[0]-nl_pos[1]-1)),
    //    qPrint(QCString(data+nl_pos[2]).left(nl_pos[1]-nl_pos[2]-1)));

    // check that line -1 is empty
    // Note that the offset is negative so we need to rewrap the string view
    if (!isEmptyLine(std::string_view(data.data()+nl_pos[1],nl_pos[0]-nl_pos[1]-1)))
    {
      AUTO_TRACE_EXIT("result={}",FALSE);
      return FALSE;
    }

    // determine the indent of line -2
    // Note that the offset is negative so we need to rewrap the string view
    indent=std::max(indent,computeIndentExcludingListMarkers(
          std::string_view(data.data()+nl_pos[2],nl_pos[1]-nl_pos[2])));

    //printf(">isCodeBlock local_indent %d>=%d+%d=%d\n",
    //    indent0,indent,codeBlockIndent,indent0>=indent+codeBlockIndent);
    // if the difference is >4 spaces -> code block
    bool res = indent0>=indent+codeBlockIndent;
    AUTO_TRACE_EXIT("result={}: code block if indent difference >4 spaces",res);
    return res;
  }
  else // not enough lines to determine the relative indent, use global indent
  {
    // check that line -1 is empty
    // Note that the offset is negative so we need to rewrap the string view
    if (nl==1 && !isEmptyLine(std::string_view(data.data()-offset,offset-1)))
    {
      AUTO_TRACE_EXIT("result=false");
      return FALSE;
    }
    //printf(">isCodeBlock global indent %d>=%d+4=%d nl=%d\n",
    //    indent0,indent,indent0>=indent+4,nl);
    bool res = indent0>=indent+codeBlockIndent;
    AUTO_TRACE_EXIT("result={}: code block if indent difference >4 spaces",res);
    return res;
  }
}

/** Finds the location of the table's contains in the string \a data.
 *  Only one line will be inspected.
 *  @param[in] data pointer to the string buffer.
 *  @param[out] start offset of the first character of the table content
 *  @param[out] end   offset of the last character of the table content
 *  @param[out] columns number of table columns found
 *  @returns The offset until the next line in the buffer.
 */
static size_t findTableColumns(std::string_view data,size_t &start,size_t &end,size_t &columns)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  const size_t size = data.size();
  size_t i=0,n=0;
  // find start character of the table line
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='|' && data[i]!='\n') i++,n++; // leading | does not count
  start = i;

  // find end character of the table line
  size_t j = 0;
  while (i<size && (j = isNewline(data.substr(i)))==0) i++;
  size_t eol=i+j;

  if (j>0 && i>0) i--; // move i to point before newline
  while (i>0 && data[i]==' ') i--;
  if (i>0 && data[i-1]!='\\' && data[i]=='|') i--,n++; // trailing or escaped | does not count
  end = i;

  // count columns between start and end
  columns=0;
  if (end>start)
  {
    i=start;
    while (i<=end) // look for more column markers
    {
      if (data[i]=='|' && (i==0 || data[i-1]!='\\')) columns++;
      if (columns==1) columns++; // first | make a non-table into a two column table
      i++;
    }
  }
  if (n==2 && columns==0) // table row has | ... |
  {
    columns++;
  }
  AUTO_TRACE_EXIT("eol={} start={} end={} columns={}",eol,start,end,columns);
  return eol;
}

/** Returns TRUE iff data points to the start of a table block */
static bool isTableBlock(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t cc0=0, start=0, end=0;

  // the first line should have at least two columns separated by '|'
  size_t i = findTableColumns(data,start,end,cc0);
  if (i>=data.size() || cc0<1)
  {
    AUTO_TRACE_EXIT("result=false: no |'s in the header");
    return FALSE;
  }

  size_t cc1 = 0;
  size_t ret = findTableColumns(data.substr(i),start,end,cc1);
  size_t j=i+start;
  // separator line should consist of |, - and : and spaces only
  while (j<=end+i)
  {
    if (data[j]!=':' && data[j]!='-' && data[j]!='|' && data[j]!=' ')
    {
      AUTO_TRACE_EXIT("result=false: invalid character '{}'",data[j]);
      return FALSE; // invalid characters in table separator
    }
    j++;
  }
  if (cc1!=cc0) // number of columns should be same as previous line
  {
    AUTO_TRACE_EXIT("result=false: different number of columns as previous line {}!={}",cc1,cc0);
    return FALSE;
  }

  i+=ret; // goto next line
  size_t cc2 = 0;
  findTableColumns(data.substr(i),start,end,cc2);

  AUTO_TRACE_EXIT("result={}",cc1==cc2);
  return cc1==cc2;
}

size_t Markdown::Private::writeTableBlock(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  const size_t size = data.size();

  size_t columns=0, start=0, end=0;
  size_t i = findTableColumns(data,start,end,columns);
  size_t headerStart = start;
  size_t headerEnd   = end;

  // read cell alignments
  size_t cc = 0;
  size_t ret = findTableColumns(data.substr(i),start,end,cc);
  size_t k=0;
  std::vector<int> columnAlignment(columns);

  bool leftMarker=false, rightMarker=false, startFound=false;
  size_t j=start+i;
  while (j<=end+i)
  {
    if (!startFound)
    {
      if (data[j]==':') { leftMarker=TRUE; startFound=TRUE; }
      if (data[j]=='-') startFound=TRUE;
      //printf("  data[%d]=%c startFound=%d\n",j,data[j],startFound);
    }
    if      (data[j]=='-') rightMarker=FALSE;
    else if (data[j]==':') rightMarker=TRUE;
    if (j<=end+i && (data[j]=='|' && (j==0 || data[j-1]!='\\')))
    {
      if (k<columns)
      {
        columnAlignment[k] = markersToAlignment(leftMarker,rightMarker);
        //printf("column[%d] alignment=%d\n",k,columnAlignment[k]);
        leftMarker=FALSE;
        rightMarker=FALSE;
        startFound=FALSE;
      }
      k++;
    }
    j++;
  }
  if (k<columns)
  {
    columnAlignment[k] = markersToAlignment(leftMarker,rightMarker);
    //printf("column[%d] alignment=%d\n",k,columnAlignment[k]);
  }
  // proceed to next line
  i+=ret;

  // Store the table cell information by row then column.  This
  // allows us to handle row spanning.
  std::vector<std::vector<TableCell> > tableContents;

  size_t m = headerStart;
  std::vector<TableCell> headerContents(columns);
  for (k=0;k<columns;k++)
  {
    while (m<=headerEnd && (data[m]!='|' || (m>0 && data[m-1]=='\\')))
    {
      headerContents[k].cellText += data[m++];
    }
    m++;
    // do the column span test before stripping white space
    // || is spanning columns, | | is not
    headerContents[k].colSpan = headerContents[k].cellText.isEmpty();
    headerContents[k].cellText = headerContents[k].cellText.stripWhiteSpace();
  }
  tableContents.push_back(headerContents);

  // write table cells
  while (i<size)
  {
    ret = findTableColumns(data.substr(i),start,end,cc);
    if (cc!=columns) break; // end of table

    j=start+i;
    k=0;
    std::vector<TableCell> rowContents(columns);
    while (j<=end+i)
    {
      if (j<=end+i && (data[j]=='|' && (j==0 || data[j-1]!='\\')))
      {
        // do the column span test before stripping white space
        // || is spanning columns, | | is not
        rowContents[k].colSpan = rowContents[k].cellText.isEmpty();
        rowContents[k].cellText = rowContents[k].cellText.stripWhiteSpace();
        k++;
      } // if (j<=end+i && (data[j]=='|' && (j==0 || data[j-1]!='\\')))
      else
      {
        rowContents[k].cellText += data[j];
      } // else { if (j<=end+i && (data[j]=='|' && (j==0 || data[j-1]!='\\'))) }
      j++;
    } // while (j<=end+i)
    // do the column span test before stripping white space
    // || is spanning columns, | | is not
    rowContents[k].colSpan  = rowContents[k].cellText.isEmpty();
    rowContents[k].cellText = rowContents[k].cellText.stripWhiteSpace();
    tableContents.push_back(rowContents);

    // proceed to next line
    i+=ret;
  }

  out+="<table class=\"markdownTable\">";
  QCString cellTag("th"), cellClass("class=\"markdownTableHead");
  for (size_t row = 0; row < tableContents.size(); row++)
  {
    if (row)
    {
      if (row % 2)
      {
        out+="\n<tr class=\"markdownTableRowOdd\">";
      }
      else
      {
        out+="\n<tr class=\"markdownTableRowEven\">";
      }
    }
    else
    {
      out+="\n  <tr class=\"markdownTableHead\">";
    }
    for (size_t c = 0; c < columns; c++)
    {
      // save the cell text for use after column span computation
      QCString cellText(tableContents[row][c].cellText);

      // Row span handling.  Spanning rows will contain a caret ('^').
      // If the current cell contains just a caret, this is part of an
      // earlier row's span and the cell should not be added to the
      // output.
      if (tableContents[row][c].cellText == "^")
      {
        continue;
      }
      if (tableContents[row][c].colSpan)
      {
        int cr = static_cast<int>(c);
        while ( cr >= 0 && tableContents[row][cr].colSpan)
        {
          cr--;
        };
        if (cr >= 0 && tableContents[row][cr].cellText == "^") continue;
      }
      size_t rowSpan = 1, spanRow = row+1;
      while ((spanRow < tableContents.size()) &&
             (tableContents[spanRow][c].cellText == "^"))
      {
        spanRow++;
        rowSpan++;
      }

      out+="    <" + cellTag + " " + cellClass;
      // use appropriate alignment style
      switch (columnAlignment[c])
      {
        case AlignLeft:   out+="Left\""; break;
        case AlignRight:  out+="Right\""; break;
        case AlignCenter: out+="Center\""; break;
        case AlignNone:   out+="None\""; break;
      }

      if (rowSpan > 1)
      {
        QCString spanStr;
        spanStr.setNum(rowSpan);
        out+=" rowspan=\"" + spanStr + "\"";
      }
      // Column span handling, assumes that column spans will have
      // empty strings, which would indicate the sequence "||", used
      // to signify spanning columns.
      size_t colSpan = 1;
      while ((c+1 < columns) && tableContents[row][c+1].colSpan)
      {
        c++;
        colSpan++;
      }
      if (colSpan > 1)
      {
        QCString spanStr;
        spanStr.setNum(colSpan);
        out+=" colspan=\"" + spanStr + "\"";
      }
      // need at least one space on either side of the cell text in
      // order for doxygen to do other formatting
      out+="> " + cellText + " \\ilinebr </" + cellTag + ">";
    }
    cellTag = "td";
    cellClass = "class=\"markdownTableBody";
    out+="  </tr>";
  }
  out+="</table>\n";

  AUTO_TRACE_EXIT("i={}",i);
  return i;
}


static bool hasLineBreak(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t i=0;
  size_t j=0;
  // search for end of line and also check if it is not a completely blank
  while (i<data.size() && data[i]!='\n')
  {
    if (data[i]!=' ' && data[i]!='\t') j++; // some non whitespace
    i++;
  }
  if (i>=data.size()) { return 0; } // empty line
  if (i<2)            { return 0; } // not long enough
  bool res = (j>0 && data[i-1]==' ' && data[i-2]==' '); // non blank line with at two spaces at the end
  AUTO_TRACE_EXIT("result={}",res);
  return res;
}


void Markdown::Private::writeOneLineHeaderOrRuler(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  int level=0;
  QCString header;
  QCString id;
  if (isHRuler(data))
  {
    out+="<hr>\n";
  }
  else if ((level=isAtxHeader(data,header,id,TRUE)))
  {
    QCString hTag;
    if (!id.isEmpty())
    {
      switch (level)
      {
        case SectionType::Section:         out+="@section ";         break;
        case SectionType::Subsection:      out+="@subsection ";      break;
        case SectionType::Subsubsection:   out+="@subsubsection ";   break;
        case SectionType::Paragraph:       out+="@paragraph ";       break;
        case SectionType::Subparagraph:    out+="@subparagraph ";    break;
        case SectionType::Subsubparagraph: out+="@subsubparagraph "; break;
      }
      out+=id;
      out+=" ";
      out+=header;
      out+="\n";
    }
    else
    {
      hTag.sprintf("h%d",level);
      out+="<"+hTag+">";
      out+=header;
      out+="</"+hTag+">\n";
    }
  }
  else if (data.size()>0) // nothing interesting -> just output the line
  {
    size_t tmpSize = data.size();
    if (data[data.size()-1] == '\n') tmpSize--;
    out+=data.substr(0,tmpSize);

    if (hasLineBreak(data))
    {
      out+="\\ilinebr<br>";
    }
    if (tmpSize != data.size()) out+='\n';
  }
}

static const std::unordered_map<std::string,std::string> g_quotationHeaderMap = {
  // GitHub style   Doxygen command
  { "[!note]",      "\\note"      },
  { "[!warning]",   "\\warning"   },
  { "[!tip]",       "\\remark"    },
  { "[!caution]",   "\\attention" },
  { "[!important]", "\\important" }
};

size_t Markdown::Private::writeBlockQuote(std::string_view data)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  size_t i=0;
  int curLevel=0;
  size_t end=0;
  const size_t size = data.size();
  std::string startCmd;
  int isGitHubAlert = false;
  int isGitHubFirst = false;
  while (i<size)
  {
    // find end of this line
    end=i+1;
    while (end<=size && data[end-1]!='\n') end++;
    size_t j=i;
    int level=0;
    size_t indent=i;
    // compute the quoting level
    while (j<end && (data[j]==' ' || data[j]=='>'))
    {
      if (data[j]=='>') { level++; indent=j+1; }
      else if (j>0 && data[j-1]=='>') indent=j+1;
      j++;
    }
    if (indent>0 && j>0 && data[j-1]=='>' &&
        !(j==size || data[j]=='\n')) // disqualify last > if not followed by space
    {
      indent--;
      level--;
      j--;
    }
    AUTO_TRACE_ADD("indent={} i={} j={} end={} level={} line={}",indent,i,j,end,level,Trace::trunc(&data[i]));
    if (level==0 && j<end-1)
    {
      level = curLevel; // lazy
    }
    if (level==1)
    {
      QCString txt = stripWhiteSpace(data.substr(indent,end-indent));
      auto it = g_quotationHeaderMap.find(txt.lower().str()); // TODO: in C++20 the std::string can be dropped
      if (it != g_quotationHeaderMap.end())
      {
        isGitHubAlert = true;
        isGitHubFirst = true;
        startCmd = it->second;
      }
    }
    if (level>curLevel) // quote level increased => add start markers
    {
      if (level!=1 || !isGitHubAlert) // normal block quote
      {
        for (int l=curLevel;l<level-1;l++)
        {
          out+="<blockquote>";
        }
        out += "<blockquote>&zwj;"; // empty blockquotes are also shown
      }
      else if (!startCmd.empty()) // GitHub style alert
      {
        out += startCmd + " ";
      }
    }
    else if (level<curLevel) // quote level decreased => add end markers
    {
      int decrLevel = curLevel;
      if (level==0 && isGitHubAlert)
      {
        decrLevel--;
      }
      for (int l=level;l<decrLevel;l++)
      {
        out += "</blockquote>";
      }
    }
    if (level==0)
    {
      curLevel=0;
      break; // end of quote block
    }
    // copy line without quotation marks
    if (curLevel!=0 || !isGitHubAlert)
    {
      std::string_view txt = data.substr(indent,end-indent);
      if (stripWhiteSpace(txt).empty() && !startCmd.empty())
      {
        if (!isGitHubFirst) out += "<br>";
        out += "<br>\n";
      }
      else
      {
        out += txt;
      }
      isGitHubFirst = false;
    }
    else // GitHub alert section
    {
      out+= "\n";
    }
    curLevel=level;
    // proceed with next line
    i=end;
  }
  // end of comment within blockquote => add end markers
  if (isGitHubAlert) // GitHub alert doesn't have a blockquote
  {
    curLevel--;
  }
  for (int l=0;l<curLevel;l++)
  {
    out+="</blockquote>";
  }
  AUTO_TRACE_EXIT("i={}",i);
  return i;
}

// For code blocks that are outputted as part of an indented include or snippet command, we need to filter out
// the location string, i.e. '\ifile "..." \iline \ilinebr'.
bool skipOverFileAndLineCommands(std::string_view data,size_t indent,size_t &offset,std::string &location)
{
  size_t i = offset;
  size_t size = data.size();
  while (i<data.size() && data[i]==' ') i++;
  if (i<size+8 && data[i]=='\\' && qstrncmp(&data[i+1],"ifile \"",7)==0)
  {
    size_t locStart = i;
    if (i>offset) locStart--; // include the space before \ifile
    i+=8;
    bool found=false;
    while (i<size-9 && data[i]!='\n')
    {
      if (data[i]=='\\' && qstrncmp(&data[i+1],"ilinebr ",8)==0)
      {
        found=true;
        break;
      }
      i++;
    }
    if (found)
    {
      i+=9;
      location=data.substr(locStart,i-locStart);
      location+='\n';
      while (indent>0 && i<size && data[i]==' ') i++,indent--;
      if (i<size && data[i]=='\n') i++;
      offset = i;
      return true;
    }
  }
  return false;
}

size_t Markdown::Private::writeCodeBlock(std::string_view data,size_t refIndent)
{
  AUTO_TRACE("data='{}' refIndent={}",Trace::trunc(data),refIndent);
  const size_t size = data.size();
  size_t i=0;
  // no need for \ilinebr here as the previous line was empty and was skipped
  out+="@iverbatim\n";
  int emptyLines=0;
  std::string location;
  while (i<size)
  {
    // find end of this line
    size_t end=i+1;
    while (end<=size && data[end-1]!='\n') end++;
    size_t j=i;
    size_t indent=0;
    while (j<end && data[j]==' ') j++,indent++;
    //printf("j=%d end=%d indent=%d refIndent=%d tabSize=%d data={%s}\n",
    //    j,end,indent,refIndent,Config_getInt(TAB_SIZE),qPrint(QCString(data+i).left(end-i-1)));
    if (j==end-1) // empty line
    {
      emptyLines++;
      i=end;
    }
    else if (indent>=refIndent+codeBlockIndent) // enough indent to continue the code block
    {
      while (emptyLines>0) // write skipped empty lines
      {
        // add empty line
        out+="\n";
        emptyLines--;
      }
      // add code line minus the indent
      size_t offset = i+refIndent+codeBlockIndent;
      std::string lineLoc;
      if (skipOverFileAndLineCommands(data,codeBlockIndent,offset,lineLoc))
      {
        location = lineLoc;
      }
      out+=data.substr(offset,end-offset);
      i=end;
    }
    else // end of code block
    {
      break;
    }
  }
  out+="@endiverbatim";
  if (!location.empty())
  {
    out+=location;
  }
  else
  {
    out+="\\ilinebr ";
  }
  while (emptyLines>0) // write skipped empty lines
  {
    // add empty line
    out+="\n";
    emptyLines--;
  }
  AUTO_TRACE_EXIT("i={}",i);
  return i;
}

// start searching for the end of the line start at offset \a i
// keeping track of possible blocks that need to be skipped.
size_t Markdown::Private::findEndOfLine(std::string_view data,size_t offset)
{
  AUTO_TRACE("data='{}'",Trace::trunc(data));
  // find end of the line
  const size_t size = data.size();
  size_t nb=0, end=offset+1, j=0;
  while (end<=size && (j=isNewline(data.data()+end-1))==0)
  {
    // while looking for the end of the line we might encounter a block
    // that needs to be passed unprocessed.
    if ((data[end-1]=='\\' || data[end-1]=='@') &&          // command
        (end<=1 || (data[end-2]!='\\' && data[end-2]!='@')) // not escaped
       )
    {
      QCString endBlockName = isBlockCommand(data.substr(end-1),end-1);
      end++;
      if (!endBlockName.isEmpty())
      {
        size_t l = endBlockName.length();
        for (;end<size-l-1;end++) // search for end of block marker
        {
          if ((data[end]=='\\' || data[end]=='@') &&
              data[end-1]!='\\' && data[end-1]!='@'
             )
          {
            if (qstrncmp(&data[end+1],endBlockName.data(),l)==0)
            {
              // found end marker, skip over this block
              //printf("feol.block out={%s}\n",qPrint(QCString(data+i).left(end+l+1-i)));
              end = end + l + 2;
              break;
            }
          }
        }
      }
    }
    else if (nb==0 && data[end-1]=='<' && size>=6 && end<size-6 &&
             (end<=1 || (data[end-2]!='\\' && data[end-2]!='@'))
            )
    {
      if (tolower(data[end])=='p' && tolower(data[end+1])=='r' &&
          tolower(data[end+2])=='e' && (data[end+3]=='>' || data[end+3]==' ')) // <pre> tag
      {
        // skip part until including </pre>
        end  = end + processHtmlTagWrite(data.substr(end-1),end-1,false);
        break;
      }
      else
      {
        end++;
      }
    }
    else if (nb==0 && data[end-1]=='`')
    {
      while (end<=size && data[end-1]=='`') end++,nb++;
    }
    else if (nb>0 && data[end-1]=='`')
    {
      size_t enb=0;
      while (end<=size && data[end-1]=='`') end++,enb++;
      if (enb==nb) nb=0;
    }
    else
    {
      end++;
    }
  }
  if (j>0) end+=j-1;
  AUTO_TRACE_EXIT("offset={} end={}",offset,end);
  return end;
}

void Markdown::Private::writeFencedCodeBlock(std::string_view data,std::string_view lang,
                size_t blockStart,size_t blockEnd)
{
  AUTO_TRACE("data='{}' lang={} blockStart={} blockEnd={}",Trace::trunc(data),lang,blockStart,blockEnd);
  if (!lang.empty() && lang[0]=='.') lang=lang.substr(1);
  const size_t size=data.size();
  size_t i=0;
  while (i<size && (data[i]==' ' || data[i]=='\t'))
  {
    out+=data[i++];
    blockStart--;
    blockEnd--;
  }
  out+="@icode";
  if (!lang.empty())
  {
    out+="{"+lang+"}";
  }
  out+=" ";
  addStrEscapeUtf8Nbsp(data.substr(blockStart+i,blockEnd-blockStart));
  out+="@endicode ";
}

QCString Markdown::Private::processQuotations(std::string_view data,size_t refIndent)
{
  AUTO_TRACE("data='{}' refIndex='{}'",Trace::trunc(data),refIndent);
  out.clear();
  size_t i=0,end=0;
  size_t pi=std::string::npos;
  bool newBlock = false;
  bool insideList = false;
  size_t currentIndent = refIndent;
  size_t listIndent = refIndent;
  const size_t size = data.size();
  QCString lang;
  while (i<size)
  {
    end = findEndOfLine(data,i);
    // line is now found at [i..end)

    size_t lineIndent=0;
    while (lineIndent<end && data[i+lineIndent]==' ') lineIndent++;
    //printf("** lineIndent=%d line=(%s)\n",lineIndent,qPrint(QCString(data+i).left(end-i)));

    if (newBlock)
    {
      //printf("** end of block\n");
      if (insideList && lineIndent<currentIndent) // end of list
      {
        //printf("** end of list\n");
        currentIndent = refIndent;
        insideList = false;
      }
      newBlock = false;
    }

    if ((listIndent=isListMarker(data.substr(i,end-i)))) // see if we need to increase the indent level
    {
      if (listIndent<currentIndent+4)
      {
        //printf("** start of list\n");
        insideList = true;
        currentIndent = listIndent;
      }
    }
    else if (isEndOfList(data.substr(i,end-i)))
    {
      //printf("** end of list\n");
      insideList = false;
      currentIndent = listIndent;
    }
    else if (isEmptyLine(data.substr(i,end-i)))
    {
      //printf("** new block\n");
      newBlock = true;
    }
    //printf("currentIndent=%d listIndent=%d refIndent=%d\n",currentIndent,listIndent,refIndent);

    if (pi!=std::string::npos)
    {
      size_t blockStart=0, blockEnd=0, blockOffset=0;
      if (isFencedCodeBlock(data.substr(pi),currentIndent,lang,blockStart,blockEnd,blockOffset))
      {
        auto addSpecialCommand = [&](const QCString &startCmd,const QCString &endCmd)
        {
          size_t cmdPos  = pi+blockStart+1;
          QCString pl = data.substr(cmdPos,blockEnd-blockStart-1);
          size_t ii = 0;
          int nl = 1;
          // check for absence of start command, either @start<cmd>, or \\start<cmd>
          while (ii<pl.length() && qisspace(pl[ii]))
          {
            if (pl[ii]=='\n') nl++;
            ii++; // skip leading whitespace
          }
          bool addNewLines = false;
          if (ii+startCmd.length()>=pl.length() || // no room for start command
              (pl[ii]!='\\' && pl[ii]!='@')     || // no @ or \ after whitespace
              qstrncmp(pl.data()+ii+1,startCmd.data(),startCmd.length())!=0) // no start command
          {
            // input:                            output:
            // ----------------------------------------------------
            // ```{plantuml}            =>       @startuml
            // A->B                              A->B
            // ```                               @enduml
            // ----------------------------------------------------
            pl = "@"+startCmd+"\n" + pl + "@"+endCmd;
            addNewLines = false;
          }
          else // we have a @start... command inside the code block
          {
            // input:                            output:
            // ----------------------------------------------------
            // ```{plantuml}                     \n
            //                                   \n
            // @startuml                =>       @startuml
            // A->B                              A->B
            // @enduml                           @enduml
            // ```                               \n
            // ----------------------------------------------------
            addNewLines = true;
          }
          if (addNewLines) for (int j=0;j<nl;j++) out+='\n';
          processSpecialCommand(pl.view().substr(ii),ii);
          if (addNewLines) out+='\n';
        };

        if (!Config_getString(PLANTUML_JAR_PATH).isEmpty() && lang=="plantuml")
        {
          addSpecialCommand("startuml","enduml");
        }
        else if (Config_getBool(HAVE_DOT) && lang=="dot")
        {
          addSpecialCommand("dot","enddot");
        }
        else if (lang=="msc") // msc is built-in
        {
          addSpecialCommand("msc","endmsc");
        }
        else // normal code block
        {
          writeFencedCodeBlock(data.substr(pi),lang.view(),blockStart,blockEnd);
        }
        i=pi+blockOffset;
        pi=std::string::npos;
        end=i+1;
        continue;
      }
      else if (isBlockQuote(data.substr(pi,i-pi),currentIndent))
      {
        i = pi+writeBlockQuote(data.substr(pi));
        pi=std::string::npos;
        end=i+1;
        continue;
      }
      else
      {
        //printf("quote out={%s}\n",QCString(data+pi).left(i-pi).data());
        out+=data.substr(pi,i-pi);
      }
    }
    pi=i;
    i=end;
  }
  if (pi!=std::string::npos && pi<size) // deal with the last line
  {
    if (isBlockQuote(data.substr(pi),currentIndent))
    {
      writeBlockQuote(data.substr(pi));
    }
    else
    {
      out+=data.substr(pi);
    }
  }

  //printf("Process quotations\n---- input ----\n%s\n---- output ----\n%s\n------------\n",
  //    qPrint(s),prv->out.get());

  return out;
}

QCString Markdown::Private::processBlocks(std::string_view data,const size_t indent)
{
  AUTO_TRACE("data='{}' indent={}",Trace::trunc(data),indent);
  out.clear();
  size_t pi = std::string::npos;
  QCString id,link,title;

#if 0 // commented out, since starting with a comment block is probably a usage error
      // see also http://stackoverflow.com/q/20478611/784672

  // special case when the documentation starts with a code block
  // since the first line is skipped when looking for a code block later on.
  if (end>codeBlockIndent && isCodeBlock(data,0,end,blockIndent))
  {
    i=writeCodeBlock(out,data,size,blockIndent);
    end=i+1;
    pi=-1;
  }
#endif

  size_t currentIndent = indent;
  size_t listIndent = indent;
  bool insideList = false;
  bool newBlock = false;
  // process each line
  size_t i=0;
  while (i<data.size())
  {
    size_t end = findEndOfLine(data,i);
    // line is now found at [i..end)

    size_t lineIndent=0;
    int level = 0;
    while (lineIndent<end && data[i+lineIndent]==' ') lineIndent++;
    //printf("** lineIndent=%d line=(%s)\n",lineIndent,qPrint(QCString(data+i).left(end-i)));

    if (newBlock)
    {
      //printf("** end of block\n");
      if (insideList && lineIndent<currentIndent) // end of list
      {
        //printf("** end of list\n");
        currentIndent = indent;
        insideList = false;
      }
      newBlock = false;
    }

    if ((listIndent=isListMarker(data.substr(i,end-i)))) // see if we need to increase the indent level
    {
      if (listIndent<currentIndent+4)
      {
        //printf("** start of list\n");
        insideList = true;
        currentIndent = listIndent;
      }
    }
    else if (isEndOfList(data.substr(i,end-i)))
    {
      //printf("** end of list\n");
      insideList = false;
      currentIndent = listIndent;
    }
    else if (isEmptyLine(data.substr(i,end-i)))
    {
      //printf("** new block\n");
      newBlock = true;
    }

    //printf("indent=%d listIndent=%d blockIndent=%d\n",indent,listIndent,blockIndent);

    //printf("findEndOfLine: pi=%d i=%d end=%d\n",pi,i,end);

    if (pi!=std::string::npos)
    {
      size_t blockStart=0, blockEnd=0, blockOffset=0;
      QCString lang;
      size_t blockIndent = currentIndent;
      size_t ref = 0;
      //printf("isHeaderLine(%s)=%d\n",QCString(data+i).left(size-i).data(),level);
      QCString endBlockName;
      if (data[i]=='@' || data[i]=='\\') endBlockName = isBlockCommand(data.substr(i),i);
      if (!endBlockName.isEmpty())
      {
        // handle previous line
        if (isLinkRef(data.substr(pi,i-pi),id,link,title))
        {
          linkRefs.emplace(id.lower().str(),LinkRef(link,title));
        }
        else
        {
          writeOneLineHeaderOrRuler(data.substr(pi,i-pi));
        }
        out+=data[i];
        i++;
        size_t l = endBlockName.length();
        while (i+l<data.size())
        {
          if ((data[i]=='\\' || data[i]=='@') && // command
              data[i-1]!='\\' && data[i-1]!='@') // not escaped
          {
            if (qstrncmp(&data[i+1],endBlockName.data(),l)==0)
            {
              out+=data[i];
              out+=endBlockName;
              i+=l+1;
              break;
            }
          }
          out+=data[i];
          i++;
        }
      }
      else if ((level=isHeaderline(data.substr(i),TRUE))>0)
      {
        //printf("Found header at %d-%d\n",i,end);
        while (pi<data.size() && data[pi]==' ') pi++;
        QCString header = data.substr(pi,i-pi-1);
        id = extractTitleId(header, level);
        //printf("header='%s' is='%s'\n",qPrint(header),qPrint(id));
        if (!header.isEmpty())
        {
          if (!id.isEmpty())
          {
            out+=level==1?"@section ":"@subsection ";
            out+=id;
            out+=" ";
            out+=header;
            out+="\n\n";
          }
          else
          {
            out+=level==1?"<h1>":"<h2>";
            out+=header;
            out+=level==1?"\n</h1>\n":"\n</h2>\n";
          }
        }
        else
        {
          out+="\n<hr>\n";
        }
        pi=std::string::npos;
        i=end;
        end=i+1;
        continue;
      }
      else if ((ref=isLinkRef(data.substr(pi),id,link,title)))
      {
        //printf("found link ref: id='%s' link='%s' title='%s'\n",
        //       qPrint(id),qPrint(link),qPrint(title));
        linkRefs.emplace(id.lower().str(),LinkRef(link,title));
        i=ref+pi;
        end=i+1;
      }
      else if (isFencedCodeBlock(data.substr(pi),currentIndent,lang,blockStart,blockEnd,blockOffset))
      {
        //printf("Found FencedCodeBlock lang='%s' start=%d end=%d code={%s}\n",
        //       qPrint(lang),blockStart,blockEnd,QCString(data+pi+blockStart).left(blockEnd-blockStart).data());
        writeFencedCodeBlock(data.substr(pi),lang.view(),blockStart,blockEnd);
        i=pi+blockOffset;
        pi=std::string::npos;
        end=i+1;
        continue;
      }
      else if (isCodeBlock(data.substr(i,end-i),i,blockIndent))
      {
        // skip previous line (it is empty anyway)
        i+=writeCodeBlock(data.substr(i),blockIndent);
        pi=std::string::npos;
        end=i+1;
        continue;
      }
      else if (isTableBlock(data.substr(pi)))
      {
        i=pi+writeTableBlock(data.substr(pi));
        pi=std::string::npos;
        end=i+1;
        continue;
      }
      else
      {
        writeOneLineHeaderOrRuler(data.substr(pi,i-pi));
      }
    }
    pi=i;
    i=end;
  }
  //printf("last line %d size=%d\n",i,size);
  if (pi!=std::string::npos && pi<data.size()) // deal with the last line
  {
    if (isLinkRef(data.substr(pi),id,link,title))
    {
      //printf("found link ref: id='%s' link='%s' title='%s'\n",
      //    qPrint(id),qPrint(link),qPrint(title));
      linkRefs.emplace(id.lower().str(),LinkRef(link,title));
    }
    else
    {
      writeOneLineHeaderOrRuler(data.substr(pi));
    }
  }

  return out;
}


static ExplicitPageResult isExplicitPage(const QCString &docs)
{
  AUTO_TRACE("docs={}",Trace::trunc(docs));
  size_t i=0;
  std::string_view data(docs.str());
  const size_t size = data.size();
  if (!data.empty())
  {
    while (i<size && (data[i]==' ' || data[i]=='\n'))
    {
      i++;
    }
    if (i<size-1 &&
        (data[i]=='\\' || data[i]=='@') &&
        (qstrncmp(&data[i+1],"page ",5)==0 || qstrncmp(&data[i+1],"mainpage",8)==0)
       )
    {
      if (qstrncmp(&data[i+1],"page ",5)==0)
      {
        AUTO_TRACE_EXIT("result=ExplicitPageResult::explicitPage");
        return ExplicitPageResult::explicitPage;
      }
      else
      {
        AUTO_TRACE_EXIT("result=ExplicitPageResult::explicitMainPage");
        return ExplicitPageResult::explicitMainPage;
      }
    }
  }
  AUTO_TRACE_EXIT("result=ExplicitPageResult::notExplicit");
  return ExplicitPageResult::notExplicit;
}

QCString Markdown::extractPageTitle(QCString &docs, QCString &id, int &prepend, bool &isIdGenerated)
{
  AUTO_TRACE("docs={} prepend={}",Trace::trunc(docs),id,prepend);
  // first first non-empty line
  prepend = 0;
  QCString title;
  size_t i=0;
  QCString docs_org(docs);
  std::string_view data(docs_org.str());
  const size_t size = data.size();
  docs.clear();
  while (i<size && (data[i]==' ' || data[i]=='\n'))
  {
    if (data[i]=='\n') prepend++;
    i++;
  }
  if (i>=size) { return QCString(); }
  size_t end1=i+1;
  while (end1<size && data[end1-1]!='\n') end1++;
  //printf("i=%d end1=%d size=%d line='%s'\n",i,end1,size,docs.mid(i,end1-i).data());
  // first line from i..end1
  if (end1<size)
  {
    // second line form end1..end2
    size_t end2=end1+1;
    while (end2<size && data[end2-1]!='\n') end2++;
    if (prv->isHeaderline(data.substr(end1),FALSE))
    {
      title = data.substr(i,end1-i-1);
      docs+="\n\n"+docs_org.mid(end2);
      id = prv->extractTitleId(title, 0, &isIdGenerated);
      //printf("extractPageTitle(title='%s' docs='%s' id='%s')\n",title.data(),docs.data(),id.data());
      AUTO_TRACE_EXIT("result={} id={} isIdGenerated={}",Trace::trunc(title),id,isIdGenerated);
      return title;
    }
  }
  if (i<end1 && prv->isAtxHeader(data.substr(i,end1-i),title,id,FALSE,&isIdGenerated)>0)
  {
    docs+="\n";
    docs+=docs_org.mid(end1);
  }
  else
  {
    docs=docs_org;
    id = prv->extractTitleId(title, 0, &isIdGenerated);
  }
  AUTO_TRACE_EXIT("result={} id={} isIdGenerated={}",Trace::trunc(title),id,isIdGenerated);
  return title;
}


//---------------------------------------------------------------------------

QCString Markdown::process(const QCString &input, int &startNewlines, bool fromParseInput)
{
  if (input.isEmpty()) return input;
  size_t refIndent=0;

  // for replace tabs by spaces
  QCString s = input;
  if (s.at(s.length()-1)!='\n') s += "\n"; // see PR #6766
  s = detab(s,refIndent);
  //printf("======== DeTab =========\n---- output -----\n%s\n---------\n",qPrint(s));

  // then process quotation blocks (as these may contain other blocks)
  s = prv->processQuotations(s.view(),refIndent);
  //printf("======== Quotations =========\n---- output -----\n%s\n---------\n",qPrint(s));

  // then process block items (headers, rules, and code blocks, references)
  s = prv->processBlocks(s.view(),refIndent);
  //printf("======== Blocks =========\n---- output -----\n%s\n---------\n",qPrint(s));

  // finally process the inline markup (links, emphasis and code spans)
  prv->out.clear();
  prv->out.reserve(s.length());
  prv->processInline(s.view());
  if (fromParseInput)
  {
    Debug::print(Debug::Markdown,0,"---- output -----\n%s\n=========\n",qPrint(prv->out));
  }
  else
  {
    Debug::print(Debug::Markdown,0,"======== Markdown =========\n---- input ------- \n%s\n---- output -----\n%s\n=========\n",qPrint(input),qPrint(prv->out));
  }

  // post processing
  QCString result = substitute(prv->out,g_doxy_nbsp,"&nbsp;");
  const char *p = result.data();
  if (p)
  {
    while (*p==' ')  p++; // skip over spaces
    while (*p=='\n') {startNewlines++;p++;}; // skip over newlines
    if (qstrncmp(p,"<br>",4)==0) p+=4; // skip over <br>
  }
  if (p>result.data())
  {
    // strip part of the input
    result = result.mid(static_cast<int>(p-result.data()));
  }
  return result;
}

//---------------------------------------------------------------------------

QCString markdownFileNameToId(const QCString &fileName)
{
  AUTO_TRACE("fileName={}",fileName);
  std::string absFileName = FileInfo(fileName.str()).absFilePath();
  QCString baseFn  = stripFromPath(absFileName.c_str());
  int i = baseFn.findRev('.');
  if (i!=-1) baseFn = baseFn.left(i);
  QCString baseName = escapeCharsInString(baseFn,false,false);
  //printf("markdownFileNameToId(%s)=md_%s\n",qPrint(fileName),qPrint(baseName));
  QCString res = "md_"+baseName;
  AUTO_TRACE_EXIT("result={}",res);
  return res;
}

//---------------------------------------------------------------------------

struct MarkdownOutlineParser::Private
{
  CommentScanner commentScanner;
};

MarkdownOutlineParser::MarkdownOutlineParser() : p(std::make_unique<Private>())
{
}

MarkdownOutlineParser::~MarkdownOutlineParser()
{
}

void MarkdownOutlineParser::parseInput(const QCString &fileName,
                const char *fileBuf,
                const std::shared_ptr<Entry> &root,
                ClangTUParser* /*clangParser*/)
{
  std::shared_ptr<Entry> current = std::make_shared<Entry>();
  int prepend = 0; // number of empty lines in front
  current->lang = SrcLangExt::Markdown;
  current->fileName = fileName;
  current->docFile  = fileName;
  current->docLine  = 1;
  QCString docs = fileBuf;
  Debug::print(Debug::Markdown,0,"======== Markdown =========\n---- input ------- \n%s\n",qPrint(fileBuf));
  QCString id;
  Markdown markdown(fileName,1,0);
  bool isIdGenerated = false;
  QCString title = markdown.extractPageTitle(docs, id, prepend, isIdGenerated).stripWhiteSpace();
  QCString generatedId;
  if (isIdGenerated)
  {
    generatedId = id;
    id = "";
  }
  int indentLevel=title.isEmpty() ? 0 : -1;
  markdown.setIndentLevel(indentLevel);
  QCString fn      = FileInfo(fileName.str()).fileName();
  QCString titleFn = stripExtensionGeneral(fn,getFileNameExtension(fn));
  QCString mdfileAsMainPage = Config_getString(USE_MDFILE_AS_MAINPAGE);
  QCString mdFileNameId = markdownFileNameToId(fileName);
  bool wasEmpty = id.isEmpty();
  if (wasEmpty) id = mdFileNameId;
  switch (isExplicitPage(docs))
  {
    case ExplicitPageResult::notExplicit:
      if (!mdfileAsMainPage.isEmpty() &&
          (fn==mdfileAsMainPage || // name reference
           FileInfo(fileName.str()).absFilePath()==
           FileInfo(mdfileAsMainPage.str()).absFilePath()) // file reference with path
         )
      {
        docs.prepend("@ianchor{" + title + "} " + id + "\\ilinebr ");
        docs.prepend("@mainpage "+title+"\\ilinebr ");
      }
      else if (id=="mainpage" || id=="index")
      {
        if (title.isEmpty()) title = titleFn;
        docs.prepend("@ianchor{" + title + "} " + id + "\\ilinebr ");
        docs.prepend("@mainpage "+title+"\\ilinebr ");
      }
      else
      {
        if (title.isEmpty())
        {
          title = titleFn;
          prepend = 0;
        }
        if (!wasEmpty)
        {
          docs.prepend("@ianchor{" + title + "} " + id + "\\ilinebr @ianchor{" + stripFromPath(fileName) + "} " + mdFileNameId + "\\ilinebr ");
        }
        else if (!generatedId.isEmpty())
        {
          docs.prepend("@ianchor " +  generatedId + "\\ilinebr ");
        }
        else if (Config_getEnum(MARKDOWN_ID_STYLE)==MARKDOWN_ID_STYLE_t::GITHUB)
        {
          QCString autoId = AnchorGenerator::instance().generate(title.str());
          docs.prepend("@ianchor{" + title + "} " +  autoId + "\\ilinebr ");
        }
        docs.prepend("@page "+id+" "+title+"\\ilinebr ");
      }
      for (int i = 0; i < prepend; i++) docs.prepend("\n");
      break;
    case ExplicitPageResult::explicitPage:
      {
        // look for `@page label My Title\n` and capture `label` (match[1]) and ` My Title` (match[2])
        static const reg::Ex re(R"([ ]*[\\@]page\s+(\a[\w-]*)(\s*[^\n]*)\n)");
        reg::Match match;
        std::string s = docs.str();
        if (reg::search(s,match,re))
        {
          QCString orgLabel    = match[1].str();
          QCString orgTitle    = match[2].str();
          orgTitle = orgTitle.stripWhiteSpace();
          QCString newLabel    = markdownFileNameToId(fileName);
          docs = docs.left(match[1].position())+               // part before label
                 newLabel+                                     // new label
                 match[2].str()+                               // part between orgLabel and \n
                 "\\ilinebr @ianchor{" + orgTitle + "} "+orgLabel+"\n"+           // add original anchor plus \n of above
                 docs.right(docs.length()-match.length());     // add remainder of docs
        }
      }
      break;
    case ExplicitPageResult::explicitMainPage:
      break;
  }
  int lineNr=1;

  p->commentScanner.enterFile(fileName,lineNr);
  Protection prot = Protection::Public;
  bool needsEntry = false;
  int position=0;
  GuardedSectionStack guards;
  QCString processedDocs = markdown.process(docs,lineNr,true);
  while (p->commentScanner.parseCommentBlock(
        this,
        current.get(),
        processedDocs,
        fileName,
        lineNr,
        FALSE,     // isBrief
        FALSE,     // javadoc autobrief
        FALSE,     // inBodyDocs
        prot,      // protection
        position,
        needsEntry,
        true,
        &guards
        ))
  {
    if (needsEntry)
    {
      QCString docFile = current->docFile;
      root->moveToSubEntryAndRefresh(current);
      current->lang = SrcLangExt::Markdown;
      current->docFile = docFile;
      current->docLine = lineNr;
    }
  }
  if (needsEntry)
  {
    root->moveToSubEntryAndKeep(current);
  }
  p->commentScanner.leaveFile(fileName,lineNr);
}

void MarkdownOutlineParser::parsePrototype(const QCString &text)
{
  Doxygen::parserManager->getOutlineParser("*.cpp")->parsePrototype(text);
}

//------------------------------------------------------------------------
