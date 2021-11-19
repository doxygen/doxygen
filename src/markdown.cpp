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

#include "markdown.h"
#include "growbuf.h"
#include "debug.h"
#include "util.h"
#include "doxygen.h"
#include "commentscan.h"
#include "entry.h"
#include "commentcnv.h"
#include "config.h"
#include "section.h"
#include "message.h"
#include "portable.h"
#include "regex.h"
#include "fileinfo.h"
#include "utf8.h"

enum class ExplicitPage
{
  notExplicit,
  explicitPage,
  explicitMainPage
};

#if !defined(NDEBUG)
#define ENABLE_TRACING
#endif

#ifdef ENABLE_TRACING
#define IOSTREAM      stdout
#define DATA_BUFSIZE  20
#if defined(_WIN32) && !defined(CYGWIN) && !defined(__MINGW32__)
#define PRETTY_FUNC __FUNCSIG__
#else
#define PRETTY_FUNC __PRETTY_FUNCTION__
#endif

class Trace
{
  public:
    Trace(const QCString &func) : m_func(func)
    {
      if (Debug::isFlagSet(Debug::Markdown))
      {
        fprintf(IOSTREAM,"> %s\n",qPrint(func));
        s_indent++;
      }
    }
    Trace(const QCString &func,const QCString &data) : m_func(func)
    {
      if (Debug::isFlagSet(Debug::Markdown))
      {
        indent();
        char data_s[DATA_BUFSIZE*2+1] = ""; // worst case each input char outputs 2 chars + 0 terminator.
        int j=0;
        if (!data.isEmpty())
        {
          for (int i=0;i<DATA_BUFSIZE;i++)
          {
            char c=data[i];
            if (c==0) break;
            else if (c=='\n') { data_s[j++]='\\'; data_s[j++]='n'; }
            else if (c=='\t') { data_s[j++]='\\'; data_s[j++]='t'; }
            else if (c=='\r') { data_s[j++]='\\'; data_s[j++]='r'; }
            else if (c=='\\') { data_s[j++]='\\'; data_s[j++]='\\'; }
            else data_s[j++]=c;
          }
        }
        data_s[j++]=0;
        fprintf(IOSTREAM,"> %s data=[%s…]\n",qPrint(func),data_s);
        s_indent++;
      }
    }
   ~Trace()
    {
      if (Debug::isFlagSet(Debug::Markdown))
      {
        s_indent--;
        indent();
        if (m_resultSet)
        {
          fprintf(IOSTREAM,"< %s result=%s\n",qPrint(m_func),qPrint(m_resultValue));
        }
        else
        {
          fprintf(IOSTREAM,"< %s\n",qPrint(m_func));
        }
      }
    }
    void trace(const char *fmt,...)
    {
      if (Debug::isFlagSet(Debug::Markdown))
      {
        indent();
        fprintf(IOSTREAM,": %s: ",qPrint(m_func));
        va_list args;
        va_start(args,fmt);
        vfprintf(IOSTREAM, fmt, args);
        va_end(args);
      }
    }
    void setResult(bool b)
    {
      m_resultSet = true;
      m_resultValue = b ? "true" : "false";
    }
    void setResult(ExplicitPage ep)
    {
      m_resultSet = true;
      m_resultValue = QCString().setNum(static_cast<int>(ep));
    }
    void setResult(int i)
    {
      m_resultSet = true;
      m_resultValue = QCString().setNum(i);
    }
    void setResult(const char *s)
    {
      m_resultSet = true;
      m_resultValue = s;
    }
    void setResult(const QCString &s)
    {
      m_resultSet = true;
      m_resultValue = s;
    }
  private:
    void indent() { for (int i=0;i<s_indent;i++) fputs("  ",IOSTREAM); }
    QCString m_func;
    bool m_resultSet = false;
    QCString m_resultValue;
    static int s_indent;
};

int Trace::s_indent = 0;
#define TRACE(data)     Trace trace_(PRETTY_FUNC,data);
#define TRACE_MORE(...) trace_.trace(__VA_ARGS__);
#define TRACE_RESULT(v) trace_.setResult(v);
#else
#define TRACE(data)     do {} while(false)
#define TRACE_MORE(...) do {} while(false)
#define TRACE_RESULT(v) do {} while(false)
#endif

//-----------

// is character at position i in data part of an identifier?
#define isIdChar(i) \
  ((data[i]>='a' && data[i]<='z') || \
   (data[i]>='A' && data[i]<='Z') || \
   (data[i]>='0' && data[i]<='9') || \
   (((unsigned char)data[i])>=0x80)) // unicode characters

#define extraChar(i) \
  (data[i]=='-' || data[i]=='+' || data[i]=='!' || \
   data[i]=='?' || data[i]=='$' || data[i]=='@' || \
   data[i]=='&' || data[i]=='*' || data[i]=='%')

// is character at position i in data allowed before an emphasis section
#define isOpenEmphChar(i) \
  (data[i]=='\n' || data[i]==' ' || data[i]=='\'' || data[i]=='<' || \
   data[i]=='>'  || data[i]=='{' || data[i]=='('  || data[i]=='[' || \
   data[i]==','  || data[i]==':' || data[i]==';')

// is character at position i in data an escape that prevents ending an emphasis section
// so for example *bla (*.txt) is cool*
#define ignoreCloseEmphChar(i) \
  (data[i]=='('  || data[i]=='{' || data[i]=='[' || (data[i]=='<' && data[i+1]!='/') || \
   data[i]=='\\' || \
   data[i]=='@')
//----------

struct TableCell
{
  TableCell() : colSpan(false) {}
  QCString cellText;
  bool colSpan;
};

Markdown::Markdown(const QCString &fileName,int lineNr,int indentLevel)
  : m_fileName(fileName), m_lineNr(lineNr), m_indentLevel(indentLevel)
{
  using namespace std::placeholders;
  // setup callback table for special characters
  m_actions[(unsigned int)'_'] = std::bind(&Markdown::processEmphasis,      this,_1,_2,_3);
  m_actions[(unsigned int)'*'] = std::bind(&Markdown::processEmphasis,      this,_1,_2,_3);
  m_actions[(unsigned int)'~'] = std::bind(&Markdown::processEmphasis,      this,_1,_2,_3);
  m_actions[(unsigned int)'`'] = std::bind(&Markdown::processCodeSpan,      this,_1,_2,_3);
  m_actions[(unsigned int)'\\']= std::bind(&Markdown::processSpecialCommand,this,_1,_2,_3);
  m_actions[(unsigned int)'@'] = std::bind(&Markdown::processSpecialCommand,this,_1,_2,_3);
  m_actions[(unsigned int)'['] = std::bind(&Markdown::processLink,          this,_1,_2,_3);
  m_actions[(unsigned int)'!'] = std::bind(&Markdown::processLink,          this,_1,_2,_3);
  m_actions[(unsigned int)'<'] = std::bind(&Markdown::processHtmlTag,       this,_1,_2,_3);
  m_actions[(unsigned int)'-'] = std::bind(&Markdown::processNmdash,        this,_1,_2,_3);
  m_actions[(unsigned int)'"'] = std::bind(&Markdown::processQuoted,        this,_1,_2,_3);
  (void)m_lineNr; // not used yet
}

enum Alignment { AlignNone, AlignLeft, AlignCenter, AlignRight };


//---------- constants -------
//
const uchar    g_utf8_nbsp[3] = { 0xc2, 0xa0, 0}; // UTF-8 nbsp
const char    *g_doxy_nsbp = "&_doxy_nbsp;";            // doxygen escape command for UTF-8 nbsp
const int codeBlockIndent = 4;

//---------- helpers -------

// test if the next characters in data represent a new line (which can be character \n or string \ilinebr).
// returns 0 if no newline is found, or the number of characters that make up the newline if found.
inline int isNewline(const char *data)
{
  // normal newline
  if (data[0] == '\n') return 1;
  // artificial new line from ^^ in ALIASES
  if (data[0] == '\\' && qstrncmp(data+1,"ilinebr ",7)==0) return data[8]==' ' ? 9 : 8;
  return 0;
}

// escape double quotes in string
static QCString escapeDoubleQuotes(const QCString &s)
{
  TRACE(s);
  if (s.isEmpty()) return s;
  GrowBuf growBuf;
  const char *p=s.data();
  char c,pc='\0';
  while ((c=*p++))
  {
    switch (c)
    {
      case '"':  if (pc!='\\')  { growBuf.addChar('\\'); } growBuf.addChar(c);   break;
      default:   growBuf.addChar(c); break;
    }
    pc=c;
  }
  growBuf.addChar(0);
  TRACE_RESULT(growBuf.get());
  return growBuf.get();
}
// escape characters that have a special meaning later on.
static QCString escapeSpecialChars(const QCString &s)
{
  TRACE(s);
  if (s.isEmpty()) return s;
  bool insideQuote=FALSE;
  GrowBuf growBuf;
  const char *p=s.data();
  char c,pc='\0';
  while ((c=*p++))
  {
    switch (c)
    {
      case '"':  if (pc!='\\')  { insideQuote=!insideQuote; } growBuf.addChar(c);   break;
      case '<':
      case '>':  if (!insideQuote)
                 {
                   growBuf.addChar('\\');
                   growBuf.addChar(c);
                   if ((p[0] == ':') && (p[1] == ':'))
                   {
                     growBuf.addChar('\\');
                     growBuf.addChar(':');
                     p++;
                   }
                 }
                 else
                 {
                   growBuf.addChar(c);
                 }
                 break;
      case '\\': if (!insideQuote) { growBuf.addChar('\\'); } growBuf.addChar('\\'); break;
      case '@':  if (!insideQuote) { growBuf.addChar('\\'); } growBuf.addChar('@'); break;
      case '#':  if (!insideQuote) { growBuf.addChar('\\'); } growBuf.addChar('#'); break;
      default:   growBuf.addChar(c); break;
    }
    pc=c;
  }
  growBuf.addChar(0);
  TRACE_RESULT(growBuf.get());
  return growBuf.get();
}

static void convertStringFragment(QCString &result,const char *data,int size)
{
  TRACE(result);
  if (size<0) size=0;
  result = QCString(data,(uint)size);
  TRACE_RESULT(result);
}

/** helper function to convert presence of left and/or right alignment markers
 *  to a alignment value
 */
static Alignment markersToAlignment(bool leftMarker,bool rightMarker)
{
  //printf("markerToAlignment(%d,%d)\n",leftMarker,rightMarker);
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
// \latexonly..\endlatexonly
// \htmlonly..\endhtmlonly
// \xmlonly..\endxmlonly
// \rtfonly..\endrtfonly
// \manonly..\endmanonly
QCString Markdown::isBlockCommand(const char *data,int offset,int size)
{
  TRACE(data);
  bool openBracket = offset>0 && data[-1]=='{';
  bool isEscaped = offset>0 && (data[-1]=='\\' || data[-1]=='@');
  if (isEscaped) return QCString();

  int end=1;
  while (end<size && (data[end]>='a' && data[end]<='z')) end++;
  if (end==1) return QCString();
  QCString blockName;
  convertStringFragment(blockName,data+1,end-1);
  if (blockName=="code" && openBracket)
  {
    TRACE_RESULT("}");
    return "}";
  }
  else if (blockName=="dot"         ||
           blockName=="code"        ||
           blockName=="msc"         ||
           blockName=="verbatim"    ||
           blockName=="latexonly"   ||
           blockName=="htmlonly"    ||
           blockName=="xmlonly"     ||
           blockName=="rtfonly"     ||
           blockName=="manonly"     ||
           blockName=="docbookonly"
     )
  {
    QCString result = "end"+blockName;
    TRACE_RESULT(result);
    return result;
  }
  else if (blockName=="startuml")
  {
    TRACE_RESULT("enduml");
    return "enduml";
  }
  else if (blockName=="f" && end<size)
  {
    if (data[end]=='$')
    {
      TRACE_RESULT("f$");
      return "f$";
    }
    else if (data[end]=='(')
    {
      TRACE_RESULT("f)");
      return "f)";
    }
    else if (data[end]=='[')
    {
      TRACE_RESULT("f]");
      return "f]";
    }
    else if (data[end]=='{')
    {
      TRACE_RESULT("f}");
      return "f}";
    }
  }
  return QCString();
}

/** looks for the next emph char, skipping other constructs, and
 *  stopping when either it is found, or we are at the end of a paragraph.
 */
int Markdown::findEmphasisChar(const char *data, int size, char c, int c_size)
{
  TRACE(data);
  int i = 1;

  while (i<size)
  {
    while (i<size && data[i]!=c    && data[i]!='`' &&
                     data[i]!='\\' && data[i]!='@' &&
                     !(data[i]=='/' && data[i-1]=='<') && // html end tag also ends emphasis
                     data[i]!='\n') i++;
    //printf("findEmphasisChar: data=[%s] i=%d c=%c\n",data,i,data[i]);

    // not counting escaped chars or characters that are unlikely
    // to appear as the end of the emphasis char
    if (ignoreCloseEmphChar(i-1))
    {
      i++;
      continue;
    }
    else
    {
      // get length of emphasis token
      int len = 0;
      while (i+len<size && data[i+len]==c)
      {
        len++;
      }

      if (len>0)
      {
        if (len!=c_size || (i<size-len && isIdChar(i+len))) // to prevent touching some_underscore_identifier
        {
          i=i+len;
          continue;
        }
        TRACE_RESULT(i);
        return i; // found it
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
      QCString endBlockName = isBlockCommand(data+i,i,size-i);
      if (!endBlockName.isEmpty())
      {
        i++;
        int l = endBlockName.length();
        while (i<size-l)
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
      else if (i<size-1 && isIdChar(i+1)) // @cmd, stop processing, see bug 690385
      {
        TRACE_RESULT(0);
        return 0;
      }
      else
      {
        i++;
      }
    }
    else if (data[i-1]=='<' && data[i]=='/') // html end tag invalidates emphasis
    {
      TRACE_RESULT(0);
      return 0;
    }
    else if (data[i]=='\n') // end * or _ at paragraph boundary
    {
      i++;
      while (i<size && data[i]==' ') i++;
      if (i>=size || data[i]=='\n') { TRACE_RESULT(0); return 0; } // empty line -> paragraph
    }
    else // should not get here!
    {
      i++;
    }

  }
  TRACE_RESULT(0);
  return 0;
}

/** process single emphasis */
int Markdown::processEmphasis1(const char *data, int size, char c)
{
  TRACE(data);
  int i = 0, len;

  /* skipping one symbol if coming from emph3 */
  if (size>1 && data[0]==c && data[1]==c) { i=1; }

  while (i<size)
  {
    len = findEmphasisChar(data+i, size-i, c, 1);
    if (len==0) { TRACE_RESULT(0); return 0; }
    i+=len;
    if (i>=size) { TRACE_RESULT(0); return 0; }

    if (i+1<size && data[i+1]==c)
    {
      i++;
      continue;
    }
    if (data[i]==c && data[i-1]!=' ' && data[i-1]!='\n')
    {
      m_out.addStr("<em>");
      processInline(data,i);
      m_out.addStr("</em>");
      TRACE_RESULT(i+1);
      return i+1;
    }
  }
  TRACE_RESULT(0);
  return 0;
}

/** process double emphasis */
int Markdown::processEmphasis2(const char *data, int size, char c)
{
  TRACE(data);
  int i = 0, len;

  while (i<size)
  {
    len = findEmphasisChar(data+i, size-i, c, 2);
    if (len==0)
    {
      TRACE_RESULT(0);
      return 0;
    }
    i += len;
    if (i+1<size && data[i]==c && data[i+1]==c && i && data[i-1]!=' ' &&
        data[i-1]!='\n'
       )
    {
      if (c == '~') m_out.addStr("<strike>");
      else m_out.addStr("<strong>");
      processInline(data,i);
      if (c == '~') m_out.addStr("</strike>");
      else m_out.addStr("</strong>");
      TRACE_RESULT(i+2);
      return i + 2;
    }
    i++;
  }
  TRACE_RESULT(0);
  return 0;
}

/** Parsing triple emphasis.
 *  Finds the first closing tag, and delegates to the other emph
 */
int Markdown::processEmphasis3(const char *data, int size, char c)
{
  TRACE(data);
  int i = 0, len;

  while (i<size)
  {
    len = findEmphasisChar(data+i, size-i, c, 3);
    if (len==0)
    {
      TRACE_RESULT(0);
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
      m_out.addStr("<em><strong>");
      processInline(data,i);
      m_out.addStr("</strong></em>");
      TRACE_RESULT(i+3);
      return i+3;
    }
    else if (i+1<size && data[i+1]==c)
    {
      // double symbol found, handing over to emph1
      len = processEmphasis1(data-2, size+2, c);
      if (len==0)
      {
        TRACE_RESULT(0);
        return 0;
      }
      else
      {
        TRACE_RESULT(len-2);
        return len - 2;
      }
    }
    else
    {
      // single symbol found, handing over to emph2
      len = processEmphasis2(data-1, size+1, c);
      if (len==0)
      {
        TRACE_RESULT(0);
        return 0;
      }
      else
      {
        TRACE_RESULT(len-1);
        return len - 1;
      }
    }
  }
  TRACE_RESULT(0);
  return 0;
}

/** Process ndash and mdashes */
int Markdown::processNmdash(const char *data,int off,int size)
{
  TRACE(data);
  // precondition: data[0]=='-'
  int i=1;
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
  if (count>=2 && off>=2 && qstrncmp(data-2,"<!",2)==0)
  { TRACE_RESULT(1-count); return 1-count; } // start HTML comment
  if (count==2 && (data[2]=='>'))
  { TRACE_RESULT(0); return 0; } // end HTML comment
  if (count==2 && (off<8 || qstrncmp(data-8,"operator",8)!=0)) // -- => ndash
  {
    m_out.addStr("&ndash;");
    TRACE_RESULT(2);
    return 2;
  }
  else if (count==3) // --- => ndash
  {
    m_out.addStr("&mdash;");
    TRACE_RESULT(3);
    return 3;
  }
  // not an ndash or mdash
  TRACE_RESULT(0);
  return 0;
}

/** Process quoted section "...", can contain one embedded newline */
int Markdown::processQuoted(const char *data,int,int size)
{
  TRACE(data);
  int i=1;
  int nl=0;
  while (i<size && data[i]!='"' && nl<2)
  {
    if (data[i]=='\n') nl++;
    i++;
  }
  if (i<size && data[i]=='"' && nl<2)
  {
    m_out.addStr(data,i+1);
    TRACE_RESULT(i+2);
    return i+1;
  }
  // not a quoted section
  TRACE_RESULT(0);
  return 0;
}

/** Process a HTML tag. Note that <pre>..</pre> are treated specially, in
 *  the sense that all code inside is written unprocessed
 */
int Markdown::processHtmlTagWrite(const char *data,int offset,int size,bool doWrite)
{
  TRACE(data);
  if (offset>0 && data[-1]=='\\') { TRACE_RESULT(0); return 0; } // escaped <

  // find the end of the html tag
  int i=1;
  int l=0;
  // compute length of the tag name
  while (i<size && isIdChar(i)) i++,l++;
  QCString tagName;
  convertStringFragment(tagName,data+1,i-1);
  if (tagName.lower()=="pre") // found <pre> tag
  {
    bool insideStr=FALSE;
    while (i<size-6)
    {
      char c=data[i];
      if (!insideStr && c=='<') // potential start of html tag
      {
        if (data[i+1]=='/' &&
            tolower(data[i+2])=='p' && tolower(data[i+3])=='r' &&
            tolower(data[i+4])=='e' && tolower(data[i+5])=='>')
        { // found </pre> tag, copy from start to end of tag
          if (doWrite) m_out.addStr(data,i+6);
          //printf("found <pre>..</pre> [%d..%d]\n",0,i+6);
          TRACE_RESULT(i+6);
          return i+6;
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
        if (doWrite) m_out.addStr(data,i+2);
        TRACE_RESULT(i+2);
        return i+2;
      }
      else if (data[i]=='>') // <bla>
      {
        //printf("Found htmlTag={%s}\n",qPrint(QCString(data).left(i+1)));
        if (doWrite) m_out.addStr(data,i+1);
        TRACE_RESULT(i+1);
        return i+1;
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
            if (doWrite) m_out.addStr(data,i+1);
            TRACE_RESULT(i+1);
            return i+1;
          }
          i++;
        }
      }
    }
  }
  //printf("Not a valid html tag\n");
  TRACE_RESULT(0);
  return 0;
}

int Markdown::processHtmlTag(const char *data,int offset,int size)
{
  TRACE(data);
  return processHtmlTagWrite(data,offset,size,true);
}

int Markdown::processEmphasis(const char *data,int offset,int size)
{
  TRACE(data);
  if ((offset>0 && !isOpenEmphChar(-1)) || // invalid char before * or _
      (size>1 && data[0]!=data[1] && !(isIdChar(1) || extraChar(1) || data[1]=='[')) || // invalid char after * or _
      (size>2 && data[0]==data[1] && !(isIdChar(2) || extraChar(2) || data[2]=='[')))   // invalid char after ** or __
  {
    TRACE_RESULT(0);
    return 0;
  }

  char c = data[0];
  int ret;
  if (size>2 && c!='~' && data[1]!=c) // _bla or *bla
  {
    // whitespace cannot follow an opening emphasis
    if (data[1]==' ' || data[1]=='\n' ||
        (ret = processEmphasis1(data+1, size-1, c)) == 0)
    {
      TRACE_RESULT(0);
      return 0;
    }
    return ret+1;
  }
  if (size>3 && data[1]==c && data[2]!=c) // __bla or **bla
  {
    if (data[2]==' ' || data[2]=='\n' ||
        (ret = processEmphasis2(data+2, size-2, c)) == 0)
    {
      TRACE_RESULT(0);
      return 0;
    }
    return ret+2;
  }
  if (size>4 && c!='~' && data[1]==c && data[2]==c && data[3]!=c) // ___bla or ***bla
  {
    if (data[3]==' ' || data[3]=='\n' ||
        (ret = processEmphasis3(data+3, size-3, c)) == 0)
    {
      TRACE_RESULT(0);
      return 0;
    }
    TRACE_RESULT(ret+3);
    return ret+3;
  }
  TRACE_RESULT(0);
  return 0;
}

void Markdown::writeMarkdownImage(const char *fmt, bool explicitTitle,
                                  const QCString &title, const QCString &content,
                                  const QCString &link, const FileDef *fd)
{
  m_out.addStr("@image{inline} ");
  m_out.addStr(fmt);
  m_out.addStr(" ");
  m_out.addStr(link.mid(fd ? 0 : 5));
  if (!explicitTitle && !content.isEmpty())
  {
    m_out.addStr(" \"");
    m_out.addStr(escapeDoubleQuotes(content));
    m_out.addStr("\"");
  }
  else if ((content.isEmpty() || explicitTitle) && !title.isEmpty())
  {
    m_out.addStr(" \"");
    m_out.addStr(escapeDoubleQuotes(title));
    m_out.addStr("\"");
  }
  else
  {
    m_out.addStr(" ");// so the line break will not be part of the image name
  }
  m_out.addStr("\\ilinebr");
}

int Markdown::processLink(const char *data,int,int size)
{
  TRACE(data);
  QCString content;
  QCString link;
  QCString title;
  int contentStart,contentEnd,linkStart,titleStart,titleEnd;
  bool isImageLink = FALSE;
  bool isToc = FALSE;
  int i=1;
  if (data[0]=='!')
  {
    isImageLink = TRUE;
    if (size<2 || data[1]!='[')
    {
      TRACE_RESULT(0);
      return 0;
    }
    i++;
  }
  contentStart=i;
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
      if (nl>1) { TRACE_RESULT(0); return 0; } // only allow one newline in the content
    }
    i++;
  }
  nlTotal += nl;
  nl = 0;
  if (i>=size) return 0; // premature end of comment -> no link
  contentEnd=i;
  convertStringFragment(content,data+contentStart,contentEnd-contentStart);
  //printf("processLink: content={%s}\n",qPrint(content));
  if (!isImageLink && content.isEmpty()) { TRACE_RESULT(0); return 0; } // no link text
  i++; // skip over ]

  // skip whitespace
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n') // one newline allowed here
  {
    i++;
    nl++;
    // skip more whitespace
    while (i<size && data[i]==' ') i++;
  }
  nlTotal += nl;
  nl = 0;

  bool explicitTitle=FALSE;
  if (i<size && data[i]=='(') // inline link
  {
    i++;
    while (i<size && data[i]==' ') i++;
    bool uriFormat=false;
    if (i<size && data[i]=='<') { i++; uriFormat=true; }
    linkStart=i;
    int braceCount=1;
    while (i<size && data[i]!='\'' && data[i]!='"' && braceCount>0)
    {
      if (data[i]=='\n') // unexpected EOL
      {
        nl++;
        if (nl>1) { TRACE_RESULT(0); return 0; }
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
    if (i>=size || data[i]=='\n') { TRACE_RESULT(0); return 0; }
    convertStringFragment(link,data+linkStart,i-linkStart);
    link = link.stripWhiteSpace();
    //printf("processLink: link={%s}\n",qPrint(link));
    if (link.isEmpty()) { TRACE_RESULT(0); return 0; }
    if (uriFormat && link.at(link.length()-1)=='>') link=link.left(link.length()-1);

    // optional title
    if (data[i]=='\'' || data[i]=='"')
    {
      char c = data[i];
      i++;
      titleStart=i;
      nl=0;
      while (i<size && data[i]!=')')
      {
        if (data[i]=='\n')
        {
          if (nl>1) { TRACE_RESULT(0); return 0; }
          nl++;
        }
        i++;
      }
      if (i>=size)
      {
        TRACE_RESULT(0);
        return 0;
      }
      titleEnd = i-1;
      // search back for closing marker
      while (titleEnd>titleStart && data[titleEnd]==' ') titleEnd--;
      if (data[titleEnd]==c) // found it
      {
        convertStringFragment(title,data+titleStart,titleEnd-titleStart);
        //printf("processLink: title={%s}\n",qPrint(title));
      }
      else
      {
        TRACE_RESULT(0);
        return 0;
      }
    }
    i++;
  }
  else if (i<size && data[i]=='[') // reference link
  {
    i++;
    linkStart=i;
    nl=0;
    // find matching ]
    while (i<size && data[i]!=']')
    {
      if (data[i]=='\n')
      {
        nl++;
        if (nl>1) { TRACE_RESULT(0); return 0; }
      }
      i++;
    }
    if (i>=size) { TRACE_RESULT(0); return 0; }
    // extract link
    convertStringFragment(link,data+linkStart,i-linkStart);
    //printf("processLink: link={%s}\n",qPrint(link));
    link = link.stripWhiteSpace();
    if (link.isEmpty()) // shortcut link
    {
      link=content;
    }
    // lookup reference
    QCString link_lower = link.lower();
    auto lr_it=m_linkRefs.find(link_lower.str());
    if (lr_it!=m_linkRefs.end()) // found it
    {
      link  = lr_it->second.link;
      title = lr_it->second.title;
      //printf("processLink: ref: link={%s} title={%s}\n",qPrint(link),qPrint(title));
    }
    else // reference not found!
    {
      //printf("processLink: ref {%s} do not exist\n",link.qPrint(lower()));
      TRACE_RESULT(0);
      return 0;
    }
    i++;
  }
  else if (i<size && data[i]!=':' && !content.isEmpty()) // minimal link ref notation [some id]
  {
    QCString content_lower = content.lower();
    auto lr_it = m_linkRefs.find(content_lower.str());
    //printf("processLink: minimal link {%s} lr=%p",qPrint(content),lr);
    if (lr_it!=m_linkRefs.end()) // found it
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
      TRACE_RESULT(0);
      return 0;
    }
    i++;
  }
  else
  {
    TRACE_RESULT(0);
    return 0;
  }
  nlTotal += nl;
  nl = 0;
  if (isToc) // special case for [TOC]
  {
    int toc_level = Config_getInt(TOC_INCLUDE_HEADINGS);
    if (toc_level > 0 && toc_level <=5)
    {
      m_out.addStr("@tableofcontents{html:");
      m_out.addStr(QCString().setNum(toc_level));
      m_out.addStr("}");
    }
  }
  else if (isImageLink)
  {
    bool ambig;
    FileDef *fd=0;
    if (link.find("@ref ")!=-1 || link.find("\\ref ")!=-1 ||
        (fd=findFileDef(Doxygen::imageNameLinkedMap,link,ambig)))
        // assume doxygen symbol link or local image link
    {
      writeMarkdownImage("html",    explicitTitle, title, content, link, fd);
      writeMarkdownImage("latex",   explicitTitle, title, content, link, fd);
      writeMarkdownImage("rtf",     explicitTitle, title, content, link, fd);
      writeMarkdownImage("docbook", explicitTitle, title, content, link, fd);
    }
    else
    {
      m_out.addStr("<img src=\"");
      m_out.addStr(link);
      m_out.addStr("\" alt=\"");
      m_out.addStr(content);
      m_out.addStr("\"");
      if (!title.isEmpty())
      {
        m_out.addStr(" title=\"");
        m_out.addStr(substitute(title.simplifyWhiteSpace(),"\"","&quot;"));
        m_out.addStr("\"");
      }
      m_out.addStr("/>");
    }
  }
  else
  {
    SrcLangExt lang = getLanguageFromFileName(link);
    int lp=-1;
    if ((lp=link.find("@ref "))!=-1 || (lp=link.find("\\ref "))!=-1 || (lang==SrcLangExt_Markdown && !isURL(link)))
        // assume doxygen symbol link
    {
      if (lp==-1) // link to markdown page
      {
        m_out.addStr("@ref ");
        if (!(Portable::isAbsolutePath(link) || isURL(link)))
        {
          FileInfo forg(link.str());
          if (forg.exists() && forg.isReadable())
          {
            link = forg.absFilePath();
          }
          else if (!(forg.exists() && forg.isReadable()))
          {
            FileInfo fi(m_fileName.str());
            QCString mdFile = m_fileName.left(m_fileName.length()-(uint)fi.fileName().length()) + link;
            FileInfo fmd(mdFile.str());
            if (fmd.exists() && fmd.isReadable())
            {
              link = fmd.absFilePath().data();
            }
          }
        }
      }
      m_out.addStr(link);
      m_out.addStr(" \"");
      if (explicitTitle && !title.isEmpty())
      {
        m_out.addStr(title);
      }
      else
      {
        m_out.addStr(content);
      }
      m_out.addStr("\"");
    }
    else if (link.find('/')!=-1 || link.find('.')!=-1 || link.find('#')!=-1)
    { // file/url link
      m_out.addStr("<a href=\"");
      m_out.addStr(link);
      m_out.addStr("\"");
      for (int ii = 0; ii < nlTotal; ii++) m_out.addStr("\n");
      if (!title.isEmpty())
      {
        m_out.addStr(" title=\"");
        m_out.addStr(substitute(title.simplifyWhiteSpace(),"\"","&quot;"));
        m_out.addStr("\"");
      }
      m_out.addStr(" ");
      m_out.addStr(externalLinkTarget());
      m_out.addStr(">");
      content = content.simplifyWhiteSpace();
      processInline(content.data(),content.length());
      m_out.addStr("</a>");
    }
    else // avoid link to e.g. F[x](y)
    {
      //printf("no link for '%s'\n",qPrint(link));
      TRACE_RESULT(0);
      return 0;
    }
  }
  TRACE_RESULT(i);
  return i;
}

/** '`' parsing a code span (assuming codespan != 0) */
int Markdown::processCodeSpan(const char *data, int /*offset*/, int size)
{
  TRACE(data);
  int end, nb = 0, i, f_begin, f_end;

  /* counting the number of backticks in the delimiter */
  while (nb<size && data[nb]=='`')
  {
    nb++;
  }

  /* finding the next delimiter */
  i = 0;
  int nl=0;
  for (end=nb; end<size && i<nb && nl<2; end++)
  {
    if (data[end]=='`')
    {
      i++;
    }
    else if (data[end]=='\n')
    {
      i=0;
      nl++;
    }
    else if (data[end]=='\'' && nb==1 && (end==size-1 || (end<size-1 && !isIdChar(end+1))))
    { // look for quoted strings like 'some word', but skip strings like `it's cool`
      QCString textFragment;
      convertStringFragment(textFragment,data+nb,end-nb);
      m_out.addStr("&lsquo;");
      m_out.addStr(textFragment);
      m_out.addStr("&rsquo;");
      return end+1;
    }
    else
    {
      i=0;
    }
  }
  if (i < nb && end >= size)
  {
    TRACE_RESULT(0);
    return 0;  // no matching delimiter
  }
  if (nl==2) // too many newlines inside the span
  {
    TRACE_RESULT(0);
    return 0;
  }

  // trimming outside whitespaces
  f_begin = nb;
  while (f_begin < end && data[f_begin]==' ')
  {
    f_begin++;
  }
  f_end = end - nb;
  while (f_end > nb && data[f_end-1]==' ')
  {
    f_end--;
  }

  //printf("found code span '%s'\n",qPrint(QCString(data+f_begin).left(f_end-f_begin)));

  /* real code span */
  if (f_begin < f_end)
  {
    QCString codeFragment;
    convertStringFragment(codeFragment,data+f_begin,f_end-f_begin);
    m_out.addStr("<tt>");
    //m_out.addStr(convertToHtml(codeFragment,TRUE));
    m_out.addStr(escapeSpecialChars(codeFragment));
    m_out.addStr("</tt>");
  }
  TRACE_RESULT(end);
  return end;
}

void Markdown::addStrEscapeUtf8Nbsp(const char *s,int len)
{
  TRACE(s);
  if (Portable::strnstr(s,g_doxy_nsbp,len)==0) // no escape needed -> fast
  {
    m_out.addStr(s,len);
  }
  else // escape needed -> slow
  {
    m_out.addStr(substitute(QCString(s).left(len),g_doxy_nsbp,(const char *)g_utf8_nbsp));
  }
}

int Markdown::processSpecialCommand(const char *data, int offset, int size)
{
  TRACE(data);
  int i=1;
  QCString endBlockName = isBlockCommand(data,offset,size);
  if (!endBlockName.isEmpty())
  {
    TRACE_MORE("endBlockName=%s\n",qPrint(endBlockName));
    int l = endBlockName.length();
    while (i<size-l)
    {
      if ((data[i]=='\\' || data[i]=='@') && // command
          data[i-1]!='\\' && data[i-1]!='@') // not escaped
      {
        if (qstrncmp(&data[i+1],endBlockName.data(),l)==0)
        {
          //printf("found end at %d\n",i);
          addStrEscapeUtf8Nbsp(data,i+1+l);
          TRACE_RESULT(i+1+l);
          return i+1+l;
        }
      }
      i++;
    }
  }
  if (size>1 && data[0]=='\\')
  {
    char c=data[1];
    if (c=='[' || c==']' || c=='*' || c=='!' || c=='(' || c==')' || c=='`' || c=='_')
    {
      m_out.addChar(data[1]);
      TRACE_RESULT(2);
      return 2;
    }
    else if (c=='-' && size>3 && data[2]=='-' && data[3]=='-') // \---
    {
      m_out.addStr(&data[1],3);
      TRACE_RESULT(2);
      return 4;
    }
    else if (c=='-' && size>2 && data[2]=='-') // \--
    {
      m_out.addStr(&data[1],2);
      TRACE_RESULT(3);
      return 3;
    }
  }
  TRACE_RESULT(0);
  return 0;
}

void Markdown::processInline(const char *data,int size)
{
  TRACE(data);
  int i=0, end=0;
  Action_t action;
  while (i<size)
  {
    while (end<size && ((action=m_actions[(uchar)data[end]])==0)) end++;
    m_out.addStr(data+i,end-i);
    if (end>=size) break;
    i=end;
    end = action(data+i,i,size-i);
    if (end<=0)
    {
      end=i+1-end;
    }
    else
    {
      i+=end;
      end=i;
    }
  }
}

/** returns whether the line is a setext-style hdr underline */
int Markdown::isHeaderline(const char *data, int size, bool allowAdjustLevel)
{
  TRACE(data);
  int i=0, c=0;
  while (i<size && data[i]==' ') i++;

  // test of level 1 header
  if (data[i]=='=')
  {
    while (i<size && data[i]=='=') i++,c++;
    while (i<size && data[i]==' ') i++;
    int level = (c>1 && (i>=size || data[i]=='\n')) ? 1 : 0;
    if (allowAdjustLevel && level==1 && m_indentLevel==-1)
    {
      // In case a page starts with a header line we use it as title, promoting it to @page.
      // We set g_indentLevel to -1 to promoting the other sections if they have a deeper
      // nesting level than the page header, i.e. @section..@subsection becomes @page..@section.
      // In case a section at the same level is found (@section..@section) however we need
      // to undo this (and the result will be @page..@section).
      m_indentLevel=0;
    }
    TRACE_RESULT(m_indentLevel+level);
    return m_indentLevel+level;
  }
  // test of level 2 header
  if (data[i]=='-')
  {
    while (i<size && data[i]=='-') i++,c++;
    while (i<size && data[i]==' ') i++;
    return (c>1 && (i>=size || data[i]=='\n')) ? m_indentLevel+2 : 0;
  }
  TRACE_RESULT(0);
  return 0;
}

/** returns TRUE if this line starts a block quote */
bool isBlockQuote(const char *data,int size,int indent)
{
  TRACE(data);
  int i = 0;
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
    // so a line starting with >= does not match
    bool res = level>0 && i<size && ((data[i-1]==' ') || data[i]=='\n');
    TRACE_RESULT(res);
    return res;
  }
  else // too much indentation -> code block
  {
    TRACE_RESULT(FALSE);
    return FALSE;
  }
  //return i<size && data[i]=='>' && i<indent+codeBlockIndent;
}

/** returns end of the link ref if this is indeed a link reference. */
static int isLinkRef(const char *data,int size,
            QCString &refid,QCString &link,QCString &title)
{
  TRACE(data);
  //printf("isLinkRef data={%s}\n",data);
  // format: start with [some text]:
  int i = 0;
  while (i<size && data[i]==' ') i++;
  if (i>=size || data[i]!='[') { TRACE_RESULT(0); return 0; }
  i++;
  int refIdStart=i;
  while (i<size && data[i]!='\n' && data[i]!=']') i++;
  if (i>=size || data[i]!=']') { TRACE_RESULT(0); return 0; }
  convertStringFragment(refid,data+refIdStart,i-refIdStart);
  if (refid.isEmpty()) { TRACE_RESULT(0); return 0; }
  //printf("  isLinkRef: found refid='%s'\n",qPrint(refid));
  i++;
  if (i>=size || data[i]!=':') { TRACE_RESULT(0); return 0; }
  i++;

  // format: whitespace* \n? whitespace* (<url> | url)
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n')
  {
    i++;
    while (i<size && data[i]==' ') i++;
  }
  if (i>=size) { TRACE_RESULT(0); return 0; }

  if (i<size && data[i]=='<') i++;
  int linkStart=i;
  while (i<size && data[i]!=' ' && data[i]!='\n') i++;
  int linkEnd=i;
  if (i<size && data[i]=='>') i++;
  if (linkStart==linkEnd) { TRACE_RESULT(0); return 0; } // empty link
  convertStringFragment(link,data+linkStart,linkEnd-linkStart);
  //printf("  isLinkRef: found link='%s'\n",qPrint(link));
  if (link=="@ref" || link=="\\ref")
  {
    int argStart=i;
    while (i<size && data[i]!='\n' && data[i]!='"') i++;
    QCString refArg;
    convertStringFragment(refArg,data+argStart,i-argStart);
    link+=refArg;
  }

  title.resize(0);

  // format: (whitespace* \n? whitespace* ( 'title' | "title" | (title) ))?
  int eol=0;
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n')
  {
    eol=i;
    i++;
    while (i<size && data[i]==' ') i++;
  }
  if (i>=size)
  {
    //printf("end of isLinkRef while looking for title! i=%d\n",i);
    TRACE_RESULT(i);
    return i; // end of buffer while looking for the optional title
  }

  char c = data[i];
  if (c=='\'' || c=='"' || c=='(') // optional title present?
  {
    //printf("  start of title found! char='%c'\n",c);
    i++;
    if (c=='(') c=')'; // replace c by end character
    int titleStart=i;
    // search for end of the line
    while (i<size && data[i]!='\n') i++;
    eol = i;

    // search back to matching character
    int end=i-1;
    while (end>titleStart && data[end]!=c) end--;
    if (end>titleStart)
    {
      convertStringFragment(title,data+titleStart,end-titleStart);
    }
    //printf("  title found: '%s'\n",qPrint(title));
  }
  while (i<size && data[i]==' ') i++;
  //printf("end of isLinkRef: i=%d size=%d data[i]='%c' eol=%d\n",
  //    i,size,data[i],eol);
  if      (i>=size)       { TRACE_RESULT(i);   return i; }    // end of buffer while ref id was found
  else if (eol)           { TRACE_RESULT(eol); return eol; }  // end of line while ref id was found
  TRACE_RESULT(0);
  return 0;                            // invalid link ref
}

static bool isHRuler(const char *data,int size)
{
  TRACE(data);
  int i=0;
  if (size>0 && data[size-1]=='\n') size--; // ignore newline character
  while (i<size && data[i]==' ') i++;
  if (i>=size) { TRACE_RESULT(FALSE); return FALSE; } // empty line
  char c=data[i];
  if (c!='*' && c!='-' && c!='_')
  {
    TRACE_RESULT(FALSE); return FALSE; // not a hrule character
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
      TRACE_RESULT(FALSE);
      return FALSE; // line contains non hruler characters
    }
    i++;
  }
  TRACE_RESULT(n>=3);
  return n>=3; // at least 3 characters needed for a hruler
}

static QCString extractTitleId(QCString &title, int level)
{
  TRACE(title);
  // match e.g. '{#id-b11} ' and capture 'id-b11'
  static const reg::Ex r2(R"({#(\a[\w-]*)}\s*$)");
  reg::Match match;
  std::string ti = title.str();
  if (reg::search(ti,match,r2))
  {
    std::string id = match[1].str();
    title = title.left((int)match.position());
    //printf("found match id='%s' title=%s\n",id.c_str(),qPrint(title));
    TRACE_RESULT(QCString(id));
    return QCString(id);
  }
  if ((level > 0) && (level <= Config_getInt(TOC_INCLUDE_HEADINGS)))
  {
    static AtomicInt autoId { 0 };
    QCString id;
    id.sprintf("autotoc_md%d",autoId++);
    //printf("auto-generated id='%s' title='%s'\n",qPrint(id),qPrint(title));
    TRACE_RESULT(id);
    return id;
  }
  //printf("no id found in title '%s'\n",qPrint(title));
  return "";
}


int Markdown::isAtxHeader(const char *data,int size,
                       QCString &header,QCString &id,bool allowAdjustLevel)
{
  TRACE(data);
  int i = 0, end;
  int level = 0, blanks=0;

  // find start of header text and determine heading level
  while (i<size && data[i]==' ') i++;
  if (i>=size || data[i]!='#')
  {
    TRACE_RESULT(0);
    return 0;
  }
  while (i<size && level<6 && data[i]=='#') i++,level++;
  while (i<size && data[i]==' ') i++,blanks++;
  if (level==1 && blanks==0)
  {
    TRACE_RESULT(0);
    return 0; // special case to prevent #someid seen as a header (see bug 671395)
  }

  // find end of header text
  end=i;
  while (end<size && data[end]!='\n') end++;
  while (end>i && (data[end-1]=='#' || data[end-1]==' ')) end--;

  // store result
  convertStringFragment(header,data+i,end-i);
  id = extractTitleId(header, level);
  if (!id.isEmpty()) // strip #'s between title and id
  {
    i=header.length()-1;
    while (i>=0 && (header.at(i)=='#' || header.at(i)==' ')) i--;
    header=header.left(i+1);
  }

  if (allowAdjustLevel && level==1 && m_indentLevel==-1)
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

    m_indentLevel=0;
  }
  int res = level+m_indentLevel;
  TRACE_RESULT(res);
  return res;
}

static bool isEmptyLine(const char *data,int size)
{
  TRACE(data);
  int i=0;
  while (i<size)
  {
    if (data[i]=='\n') { TRACE_RESULT(TRUE);  return TRUE; }
    if (data[i]!=' ')  { TRACE_RESULT(FALSE); return FALSE; }
    i++;
  }
  TRACE_RESULT(TRUE);
  return TRUE;
}

#define isLiTag(i) \
   (data[(i)]=='<' && \
   (data[(i)+1]=='l' || data[(i)+1]=='L') && \
   (data[(i)+2]=='i' || data[(i)+2]=='I') && \
   (data[(i)+3]=='>'))

// compute the indent from the start of the input, excluding list markers
// such as -, -#, *, +, 1., and <li>
static int computeIndentExcludingListMarkers(const char *data,int size)
{
  TRACE(data);
  int i=0;
  int indent=0;
  bool isDigit=FALSE;
  bool isLi=FALSE;
  bool listMarkerSkipped=FALSE;
  while (i<size &&
         (data[i]==' ' ||                                    // space
          (!listMarkerSkipped &&                             // first list marker
           (data[i]=='+' || data[i]=='-' || data[i]=='*' ||  // unordered list char
            (data[i]=='#' && i>0 && data[i-1]=='-') ||       // -# item
            (isDigit=(data[i]>='1' && data[i]<='9')) ||      // ordered list marker?
            (isLi=(i<size-3 && isLiTag(i)))                  // <li> tag
           )
          )
         )
        )
  {
    if (isDigit) // skip over ordered list marker '10. '
    {
      int j=i+1;
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
    else if (data[i]=='-' && i<size-2 && data[i+1]=='#' && data[i+2]==' ')
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
  //printf("{%s}->%d\n",QCString(data).qPrint(left(size)),indent);
  TRACE_RESULT(indent);
  return indent;
}

static int isListMarker(const char *data,int size)
{
  TRACE(data);
  int normalIndent = 0;
  while (normalIndent<size && data[normalIndent]==' ') normalIndent++;
  int listIndent = computeIndentExcludingListMarkers(data,size);
  int result = listIndent>normalIndent ? listIndent : 0;
  TRACE_RESULT(result);
  return result;
}

static bool isEndOfList(const char *data,int size)
{
  TRACE(data);
  int dots=0;
  int i=0;
  // end of list marker is an otherwise empty line with a dot.
  while (i<size)
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
      TRACE_RESULT(FALSE);
      return FALSE;
    }
    i++;
  }
  TRACE_RESULT(dots==1);
  return dots==1;
}

static bool isFencedCodeBlock(const char *data,int size,int refIndent,
                             QCString &lang,int &start,int &end,int &offset)
{
  TRACE(data);
  // rules: at least 3 ~~~, end of the block same amount of ~~~'s, otherwise
  // return FALSE
  int i=0;
  int indent=0;
  int startTildes=0;
  while (i<size && data[i]==' ') indent++,i++;
  if (indent>=refIndent+4)
  {
    //printf("content is part of code block: indent=%d refIndent=%d\n",indent,refIndent);
    TRACE_RESULT(FALSE);
    return FALSE;
  } // part of code block
  char tildaChar='~';
  if (i<size && data[i]=='`') tildaChar='`';
  while (i<size && data[i]==tildaChar) startTildes++,i++;
  if (startTildes<3)
  {
    //printf("no fence marker found #tildes=%d\n",startTildes);
    TRACE_RESULT(FALSE);
    return FALSE;
  } // not enough tildes
  if (i<size && data[i]=='{') i++; // skip over optional {
  int startLang=i;
  while (i<size && (data[i]!='\n' && data[i]!='}' && data[i]!=' ')) i++;
  convertStringFragment(lang,data+startLang,i-startLang);
  while (i<size && data[i]!='\n') i++; // proceed to the end of the line
  start=i;
  while (i<size)
  {
    if (data[i]==tildaChar)
    {
      end=i;
      int endTildes=0;
      while (i<size && data[i]==tildaChar) endTildes++,i++;
      while (i<size && data[i]==' ') i++;
      if (i==size || data[i]=='\n')
      {
        if (endTildes==startTildes)
        {
          offset=i;
          //printf("found end marker at offset %d\n",offset);
          TRACE_RESULT(TRUE);
          return TRUE;
        }
      }
    }
    i++;
  }
  //printf("no end marker found!\n");
  TRACE_RESULT(FALSE);
  return FALSE;
}

static bool isCodeBlock(const char *data,int offset,int size,int &indent)
{
  TRACE(data);
  //printf("<isCodeBlock(offset=%d,size=%d,indent=%d)\n",offset,size,indent);
  // determine the indent of this line
  int i=0;
  int indent0=0;
  while (i<size && data[i]==' ') indent0++,i++;

  if (indent0<codeBlockIndent)
  {
    //printf(">isCodeBlock: line is not indented enough %d<4\n",indent0);
    TRACE_RESULT(FALSE);
    return FALSE;
  }
  if (indent0>=size || data[indent0]=='\n') // empty line does not start a code block
  {
    //printf("only spaces at the end of a comment block\n");
    TRACE_RESULT(FALSE);
    return FALSE;
  }

  i=offset;
  int nl=0;
  int nl_pos[3];
  // search back 3 lines and remember the start of lines -1 and -2
  while (i>0 && nl<3)
  {
    int j = i-offset-1;
    int nl_size = isNewline(data+j);
    if (nl_size>0)
    {
      nl_pos[nl++]=j+nl_size;
    }
    i--;
  }

  // if there are only 2 preceding lines, then line -2 starts at -offset
  if (i==0 && nl==2) nl_pos[nl++]=-offset;
  //printf("  nl=%d\n",nl);

  if (nl==3) // we have at least 2 preceding lines
  {
    //printf("  positions: nl_pos=[%d,%d,%d] line[-2]='%s' line[-1]='%s'\n",
    //    nl_pos[0],nl_pos[1],nl_pos[2],
    //    qPrint(QCString(data+nl_pos[1]).left(nl_pos[0]-nl_pos[1]-1)),
    //    qPrint(QCString(data+nl_pos[2]).left(nl_pos[1]-nl_pos[2]-1)));

    // check that line -1 is empty
    if (!isEmptyLine(data+nl_pos[1],nl_pos[0]-nl_pos[1]-1))
    {
      TRACE_RESULT(FALSE);
      return FALSE;
    }

    // determine the indent of line -2
    indent=std::max(indent,computeIndentExcludingListMarkers(data+nl_pos[2],nl_pos[1]-nl_pos[2]));

    //printf(">isCodeBlock local_indent %d>=%d+%d=%d\n",
    //    indent0,indent,codeBlockIndent,indent0>=indent+codeBlockIndent);
    // if the difference is >4 spaces -> code block
    bool res = indent0>=indent+codeBlockIndent;
    TRACE_RESULT(res);
    return res;
  }
  else // not enough lines to determine the relative indent, use global indent
  {
    // check that line -1 is empty
    if (nl==1 && !isEmptyLine(data-offset,offset-1))
    {
      TRACE_RESULT(FALSE);
      return FALSE;
    }
    //printf(">isCodeBlock global indent %d>=%d+4=%d nl=%d\n",
    //    indent0,indent,indent0>=indent+4,nl);
    bool res = indent0>=indent+codeBlockIndent;
    TRACE_RESULT(res);
    return res;
  }
}

/** Finds the location of the table's contains in the string \a data.
 *  Only one line will be inspected.
 *  @param[in] data pointer to the string buffer.
 *  @param[in] size the size of the buffer.
 *  @param[out] start offset of the first character of the table content
 *  @param[out] end   offset of the last character of the table content
 *  @param[out] columns number of table columns found
 *  @returns The offset until the next line in the buffer.
 */
int findTableColumns(const char *data,int size,int &start,int &end,int &columns)
{
  TRACE(data);
  int i=0,n=0;
  int eol;
  // find start character of the table line
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='|' && data[i]!='\n') i++,n++; // leading | does not count
  start = i;

  // find end character of the table line
  //while (i<size && data[i]!='\n') i++;
  //eol=i+1;
  int j = 0;
  while (i<size && (j = isNewline(data + i))==0) i++;
  eol=i+j;

  i--;
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
  //printf("findTableColumns(start=%d,end=%d,columns=%d) eol=%d\n",
  //    start,end,columns,eol);
  TRACE_RESULT(eol);
  return eol;
}

/** Returns TRUE iff data points to the start of a table block */
static bool isTableBlock(const char *data,int size)
{
  TRACE(data);
  int cc0,start,end;

  // the first line should have at least two columns separated by '|'
  int i = findTableColumns(data,size,start,end,cc0);
  if (i>=size || cc0<1)
  {
    //printf("isTableBlock: no |'s in the header\n");
    TRACE_RESULT(FALSE);
    return FALSE;
  }

  int cc1;
  int ret = findTableColumns(data+i,size-i,start,end,cc1);
  int j=i+start;
  // separator line should consist of |, - and : and spaces only
  while (j<=end+i)
  {
    if (data[j]!=':' && data[j]!='-' && data[j]!='|' && data[j]!=' ')
    {
      //printf("isTableBlock: invalid character '%c'\n",data[j]);
      TRACE_RESULT(FALSE);
      return FALSE; // invalid characters in table separator
    }
    j++;
  }
  if (cc1!=cc0) // number of columns should be same as previous line
  {
    TRACE_RESULT(FALSE);
    return FALSE;
  }

  i+=ret; // goto next line
  int cc2;
  findTableColumns(data+i,size-i,start,end,cc2);

  //printf("isTableBlock: %d\n",cc1==cc2);
  TRACE_RESULT(cc1==cc2);
  return cc1==cc2;
}

int Markdown::writeTableBlock(const char *data,int size)
{
  TRACE(data);
  int i=0,j,k;
  int columns,start,end,cc;

  i = findTableColumns(data,size,start,end,columns);

  int headerStart = start;
  int headerEnd = end;

  // read cell alignments
  int ret = findTableColumns(data+i,size-i,start,end,cc);
  k=0;
  std::vector<int> columnAlignment(columns);

  bool leftMarker=FALSE,rightMarker=FALSE;
  bool startFound=FALSE;
  j=start+i;
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

  int m=headerStart;
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
    ret = findTableColumns(data+i,size-i,start,end,cc);
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

  m_out.addStr("<table class=\"markdownTable\">");
  QCString cellTag("th"), cellClass("class=\"markdownTableHead");
  for (unsigned row = 0; row < tableContents.size(); row++)
  {
    if (row)
    {
      if (row % 2)
      {
        m_out.addStr("\n<tr class=\"markdownTableRowOdd\">");
      }
      else
      {
        m_out.addStr("\n<tr class=\"markdownTableRowEven\">");
      }
    }
    else
    {
      m_out.addStr("\n  <tr class=\"markdownTableHead\">");
    }
    for (int c = 0; c < columns; c++)
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
        int cr = c;
        while ( cr >= 0 && tableContents[row][cr].colSpan)
        {
          cr--;
        };
        if (cr >= 0 && tableContents[row][cr].cellText == "^") continue;
      }
      unsigned rowSpan = 1, spanRow = row+1;
      while ((spanRow < tableContents.size()) &&
             (tableContents[spanRow][c].cellText == "^"))
      {
        spanRow++;
        rowSpan++;
      }

      m_out.addStr("    <" + cellTag + " " + cellClass);
      // use appropriate alignment style
      switch (columnAlignment[c])
      {
        case AlignLeft:   m_out.addStr("Left\""); break;
        case AlignRight:  m_out.addStr("Right\""); break;
        case AlignCenter: m_out.addStr("Center\""); break;
        case AlignNone:   m_out.addStr("None\""); break;
      }

      if (rowSpan > 1)
      {
        QCString spanStr;
        spanStr.setNum(rowSpan);
        m_out.addStr(" rowspan=\"" + spanStr + "\"");
      }
      // Column span handling, assumes that column spans will have
      // empty strings, which would indicate the sequence "||", used
      // to signify spanning columns.
      unsigned colSpan = 1;
      while ((c < columns-1) && tableContents[row][c+1].colSpan)
      {
        c++;
        colSpan++;
      }
      if (colSpan > 1)
      {
        QCString spanStr;
        spanStr.setNum(colSpan);
        m_out.addStr(" colspan=\"" + spanStr + "\"");
      }
      // need at least one space on either side of the cell text in
      // order for doxygen to do other formatting
      m_out.addStr("> " + cellText + " \\ilinebr </" + cellTag + ">");
    }
    cellTag = "td";
    cellClass = "class=\"markdownTableBody";
    m_out.addStr("  </tr>");
  }
  m_out.addStr("</table>\n");

  TRACE_RESULT(i);
  return i;
}


static bool hasLineBreak(const char *data,int size)
{
  TRACE(data);
  int i=0;
  int j=0;
  // search for end of line and also check if it is not a completely blank
  while (i<size && data[i]!='\n')
  {
    if (data[i]!=' ' && data[i]!='\t') j++; // some non whitespace
    i++;
  }
  if (i>=size) { TRACE_RESULT(0); return 0; } // empty line
  if (i<2)     { TRACE_RESULT(0); return 0; } // not long enough
  bool res = (j>0 && data[i-1]==' ' && data[i-2]==' '); // non blank line with at two spaces at the end
  TRACE_RESULT(res);
  return res;
}


void Markdown::writeOneLineHeaderOrRuler(const char *data,int size)
{
  TRACE(data);
  int level;
  QCString header;
  QCString id;
  if (isHRuler(data,size))
  {
    m_out.addStr("<hr>\n");
  }
  else if ((level=isAtxHeader(data,size,header,id,TRUE)))
  {
    QCString hTag;
    if (level<5 && !id.isEmpty())
    {
      switch(level)
      {
        case 1:  m_out.addStr("@section ");
                 break;
        case 2:  m_out.addStr("@subsection ");
                 break;
        case 3:  m_out.addStr("@subsubsection ");
                 break;
        default: m_out.addStr("@paragraph ");
                 break;
      }
      m_out.addStr(id);
      m_out.addStr(" ");
      m_out.addStr(header);
      m_out.addStr("\n");
    }
    else
    {
      if (!id.isEmpty())
      {
        m_out.addStr("\\anchor "+id+"\\ilinebr ");
      }
      hTag.sprintf("h%d",level);
      m_out.addStr("<"+hTag+">");
      m_out.addStr(header);
      m_out.addStr("</"+hTag+">\n");
    }
  }
  else if (size>0) // nothing interesting -> just output the line
  {
    int tmpSize = size;
    if (data[size-1] == '\n') tmpSize--;
    m_out.addStr(data,tmpSize);

    if (hasLineBreak(data,size))
    {
      m_out.addStr("<br>");
    }
    if (tmpSize != size) m_out.addChar('\n');
  }
}

int Markdown::writeBlockQuote(const char *data,int size)
{
  TRACE(data);
  int l;
  int i=0;
  int curLevel=0;
  int end=0;
  while (i<size)
  {
    // find end of this line
    end=i+1;
    while (end<=size && data[end-1]!='\n') end++;
    int j=i;
    int level=0;
    int indent=i;
    // compute the quoting level
    while (j<end && (data[j]==' ' || data[j]=='>'))
    {
      if (data[j]=='>') { level++; indent=j+1; }
      else if (j>0 && data[j-1]=='>') indent=j+1;
      j++;
    }
    if (j>0 && data[j-1]=='>' &&
        !(j==size || data[j]=='\n')) // disqualify last > if not followed by space
    {
      indent--;
      j--;
    }
    if (level>curLevel) // quote level increased => add start markers
    {
      for (l=curLevel;l<level;l++)
      {
        m_out.addStr("<blockquote>");
      }
    }
    else if (level<curLevel) // quote level decreased => add end markers
    {
      for (l=level;l<curLevel;l++)
      {
        m_out.addStr("</blockquote>");
      }
    }
    curLevel=level;
    if (level==0) break; // end of quote block
    // copy line without quotation marks
    m_out.addStr(data+indent,end-indent);
    // proceed with next line
    i=end;
  }
  // end of comment within blockquote => add end markers
  for (l=0;l<curLevel;l++)
  {
    m_out.addStr("</blockquote>");
  }
  TRACE_RESULT(i);
  return i;
}

int Markdown::writeCodeBlock(const char *data,int size,int refIndent)
{
  TRACE(data);
  int i=0,end;
  //printf("writeCodeBlock: data={%s}\n",qPrint(QCString(data).left(size)));
  // no need for \ilinebr here as the previous line was empty and was skipped
  m_out.addStr("@verbatim\n");
  int emptyLines=0;
  while (i<size)
  {
    // find end of this line
    end=i+1;
    while (end<=size && data[end-1]!='\n') end++;
    int j=i;
    int indent=0;
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
        m_out.addStr("\n");
        emptyLines--;
      }
      // add code line minus the indent
      m_out.addStr(data+i+refIndent+codeBlockIndent,end-i-refIndent-codeBlockIndent);
      i=end;
    }
    else // end of code block
    {
      break;
    }
  }
  m_out.addStr("@endverbatim\\ilinebr ");
  while (emptyLines>0) // write skipped empty lines
  {
    // add empty line
    m_out.addStr("\n");
    emptyLines--;
  }
  //printf("i=%d\n",i);
  TRACE_RESULT(i);
  return i;
}

// start searching for the end of the line start at offset \a i
// keeping track of possible blocks that need to be skipped.
void Markdown::findEndOfLine(const char *data,int size,
                          int &pi,int&i,int &end)
{
  TRACE(data);
  // find end of the line
  int nb=0;
  end=i+1;
  //while (end<=size && data[end-1]!='\n')
  int j=0;
  while (end<=size && (j=isNewline(data+end-1))==0)
  {
    // while looking for the end of the line we might encounter a block
    // that needs to be passed unprocessed.
    if ((data[end-1]=='\\' || data[end-1]=='@') &&          // command
        (end<=1 || (data[end-2]!='\\' && data[end-2]!='@')) // not escaped
       )
    {
      QCString endBlockName = isBlockCommand(data+end-1,end-1,size-(end-1));
      end++;
      if (!endBlockName.isEmpty())
      {
        int l = endBlockName.length();
        for (;end<size-l-1;end++) // search for end of block marker
        {
          if ((data[end]=='\\' || data[end]=='@') &&
              data[end-1]!='\\' && data[end-1]!='@'
             )
          {
            if (qstrncmp(&data[end+1],endBlockName.data(),l)==0)
            {
              // found end marker, skip over this block
              //printf("feol.block m_out={%s}\n",qPrint(QCString(data+i).left(end+l+1-i)));
              end = end + l + 2;
              break;
            }
          }
        }
      }
    }
    else if (nb==0 && data[end-1]=='<' && end<size-6 &&
             (end<=1 || (data[end-2]!='\\' && data[end-2]!='@'))
            )
    {
      if (tolower(data[end])=='p' && tolower(data[end+1])=='r' &&
          tolower(data[end+2])=='e' && (data[end+3]=='>' || data[end+3]==' ')) // <pre> tag
      {
        // skip part until including </pre>
        end  = end + processHtmlTagWrite(data+end-1,end-1,size-end+1,false) + 2;
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
      int enb=0;
      while (end<=size && data[end-1]=='`') end++,enb++;
      if (enb==nb) nb=0;
    }
    else
    {
      end++;
    }
  }
  if (j>0) end+=j-1;
  //printf("findEndOfLine pi=%d i=%d end=%d {%s}\n",pi,i,end,qPrint(QCString(data+i).left(end-i)));
}

void Markdown::writeFencedCodeBlock(const char *data,const char *lng,
                int blockStart,int blockEnd)
{
  TRACE(data);
  QCString lang = lng;
  if (!lang.isEmpty() && lang.at(0)=='.') lang=lang.mid(1);
  while (*data==' ' || *data=='\t')
  {
    m_out.addChar(*data++);
    blockStart--;
    blockEnd--;
  }
  m_out.addStr("@code");
  if (!lang.isEmpty())
  {
    m_out.addStr("{"+lang+"}");
  }
  addStrEscapeUtf8Nbsp(data+blockStart,blockEnd-blockStart);
  m_out.addStr("@endcode");
}

QCString Markdown::processQuotations(const QCString &s,int refIndent)
{
  TRACE(s);
  m_out.clear();
  const char *data = s.data();
  int size = s.length();
  int i=0,end=0,pi=-1;
  int blockStart,blockEnd,blockOffset;
  bool newBlock = false;
  bool insideList = false;
  int currentIndent = refIndent;
  int listIndent = refIndent;
  QCString lang;
  while (i<size)
  {
    findEndOfLine(data,size,pi,i,end);
    // line is now found at [i..end)

    int lineIndent=0;
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

    if ((listIndent=isListMarker(data+i,end-i))) // see if we need to increase the indent level
    {
      if (listIndent<currentIndent+4)
      {
        //printf("** start of list\n");
        insideList = true;
        currentIndent = listIndent;
      }
    }
    else if (isEndOfList(data+i,end-i))
    {
      //printf("** end of list\n");
      insideList = false;
      currentIndent = listIndent;
    }
    else if (isEmptyLine(data+i,end-i))
    {
      //printf("** new block\n");
      newBlock = true;
    }
    //printf("currentIndent=%d listIndent=%d refIndent=%d\n",currentIndent,listIndent,refIndent);

    if (pi!=-1)
    {
      if (isFencedCodeBlock(data+pi,size-pi,currentIndent,lang,blockStart,blockEnd,blockOffset))
      {
        writeFencedCodeBlock(data+pi,lang.data(),blockStart,blockEnd);
        i=pi+blockOffset;
        pi=-1;
        end=i+1;
        continue;
      }
      else if (isBlockQuote(data+pi,i-pi,currentIndent))
      {
        i = pi+writeBlockQuote(data+pi,size-pi);
        pi=-1;
        end=i+1;
        continue;
      }
      else
      {
        //printf("quote m_out={%s}\n",QCString(data+pi).left(i-pi).data());
        m_out.addStr(data+pi,i-pi);
      }
    }
    pi=i;
    i=end;
  }
  if (pi!=-1 && pi<size) // deal with the last line
  {
    if (isBlockQuote(data+pi,size-pi,currentIndent))
    {
      writeBlockQuote(data+pi,size-pi);
    }
    else
    {
      m_out.addStr(data+pi,size-pi);
    }
  }
  m_out.addChar(0);

  //printf("Process quotations\n---- input ----\n%s\n---- output ----\n%s\n------------\n",
  //    qPrint(s),m_out.get());

  return m_out.get();
}

QCString Markdown::processBlocks(const QCString &s,const int indent)
{
  TRACE(s);
  m_out.clear();
  const char *data = s.data();
  int size = s.length();
  int i=0,end=0,pi=-1,ref,level;
  QCString id,link,title;
  int blockIndent = indent;

  // get indent for the first line
  end = i+1;
  int sp=0;
  while (end<=size && data[end-1]!='\n')
  {
    if (data[end-1]==' ') sp++;
    end++;
  }

#if 0 // commented m_out, since starting with a comment block is probably a usage error
      // see also http://stackoverflow.com/q/20478611/784672

  // special case when the documentation starts with a code block
  // since the first line is skipped when looking for a code block later on.
  if (end>codeBlockIndent && isCodeBlock(data,0,end,blockIndent))
  {
    i=writeCodeBlock(m_out,data,size,blockIndent);
    end=i+1;
    pi=-1;
  }
#endif

  int currentIndent = indent;
  int listIndent = indent;
  bool insideList = false;
  bool newBlock = false;
  // process each line
  while (i<size)
  {
    findEndOfLine(data,size,pi,i,end);
    // line is now found at [i..end)

    int lineIndent=0;
    while (lineIndent<end && data[i+lineIndent]==' ') lineIndent++;
    //printf("** lineIndent=%d line=(%s)\n",lineIndent,qPrint(QCString(data+i).left(end-i)));

    if (newBlock)
    {
      //printf("** end of block\n");
      if (insideList && lineIndent<currentIndent) // end of list
      {
        //printf("** end of list\n");
        currentIndent = indent;
        blockIndent = indent;
        insideList = false;
      }
      newBlock = false;
    }

    if ((listIndent=isListMarker(data+i,end-i))) // see if we need to increase the indent level
    {
      if (listIndent<currentIndent+4)
      {
        //printf("** start of list\n");
        insideList = true;
        currentIndent = listIndent;
        blockIndent = listIndent;
      }
    }
    else if (isEndOfList(data+i,end-i))
    {
      //printf("** end of list\n");
      insideList = false;
      currentIndent = listIndent;
      blockIndent = listIndent;
    }
    else if (isEmptyLine(data+i,end-i))
    {
      //printf("** new block\n");
      newBlock = true;
    }

    //printf("indent=%d listIndent=%d blockIndent=%d\n",indent,listIndent,blockIndent);

    //printf("findEndOfLine: pi=%d i=%d end=%d\n",pi,i,end);

    if (pi!=-1)
    {
      int blockStart,blockEnd,blockOffset;
      QCString lang;
      blockIndent = currentIndent;
      //printf("isHeaderLine(%s)=%d\n",QCString(data+i).left(size-i).data(),level);
      QCString endBlockName;
      if (data[i]=='@' || data[i]=='\\') endBlockName = isBlockCommand(data+i,i,size-i);
      if (!endBlockName.isEmpty())
      {
        // handle previous line
        if (isLinkRef(data+pi,i-pi,id,link,title))
        {
          m_linkRefs.insert({id.lower().str(),LinkRef(link,title)});
        }
        else
        {
          writeOneLineHeaderOrRuler(data+pi,i-pi);
        }
        m_out.addChar(data[i]);
        i++;
        int l = endBlockName.length();
        while (i<size-l)
        {
          if ((data[i]=='\\' || data[i]=='@') && // command
              data[i-1]!='\\' && data[i-1]!='@') // not escaped
          {
            if (qstrncmp(&data[i+1],endBlockName.data(),l)==0)
            {
              m_out.addChar(data[i]);
              m_out.addStr(endBlockName);
              pi=i;
              i+=l+1;
              break;
            }
          }
          m_out.addChar(data[i]);
          i++;
        }
      }
      else if ((level=isHeaderline(data+i,size-i,TRUE))>0)
      {
        //printf("Found header at %d-%d\n",i,end);
        while (pi<size && data[pi]==' ') pi++;
        QCString header;
        convertStringFragment(header,data+pi,i-pi-1);
        id = extractTitleId(header, level);
        //printf("header='%s' is='%s'\n",qPrint(header),qPrint(id));
        if (!header.isEmpty())
        {
          if (!id.isEmpty())
          {
            m_out.addStr(level==1?"@section ":"@subsection ");
            m_out.addStr(id);
            m_out.addStr(" ");
            m_out.addStr(header);
            m_out.addStr("\n\n");
          }
          else
          {
            m_out.addStr(level==1?"<h1>":"<h2>");
            m_out.addStr(header);
            m_out.addStr(level==1?"\n</h1>\n":"\n</h2>\n");
          }
        }
        else
        {
          m_out.addStr("\n<hr>\n");
        }
        pi=-1;
        i=end;
        end=i+1;
        continue;
      }
      else if ((ref=isLinkRef(data+pi,size-pi,id,link,title)))
      {
        //printf("found link ref: id='%s' link='%s' title='%s'\n",
        //       qPrint(id),qPrint(link),qPrint(title));
        m_linkRefs.insert({id.lower().str(),LinkRef(link,title)});
        i=ref+pi;
        pi=-1;
        end=i+1;
      }
      else if (isFencedCodeBlock(data+pi,size-pi,currentIndent,lang,blockStart,blockEnd,blockOffset))
      {
        //printf("Found FencedCodeBlock lang='%s' start=%d end=%d code={%s}\n",
        //       qPrint(lang),blockStart,blockEnd,QCString(data+pi+blockStart).left(blockEnd-blockStart).data());
        writeFencedCodeBlock(data+pi,lang.data(),blockStart,blockEnd);
        i=pi+blockOffset;
        pi=-1;
        end=i+1;
        continue;
      }
      else if (isCodeBlock(data+i,i,end-i,blockIndent))
      {
        // skip previous line (it is empty anyway)
        i+=writeCodeBlock(data+i,size-i,blockIndent);
        pi=-1;
        end=i+1;
        continue;
      }
      else if (isTableBlock(data+pi,size-pi))
      {
        i=pi+writeTableBlock(data+pi,size-pi);
        pi=-1;
        end=i+1;
        continue;
      }
      else
      {
        writeOneLineHeaderOrRuler(data+pi,i-pi);
      }
    }
    pi=i;
    i=end;
  }
  //printf("last line %d size=%d\n",i,size);
  if (pi!=-1 && pi<size) // deal with the last line
  {
    if (isLinkRef(data+pi,size-pi,id,link,title))
    {
      //printf("found link ref: id='%s' link='%s' title='%s'\n",
      //    qPrint(id),qPrint(link),qPrint(title));
      m_linkRefs.insert({id.lower().str(),LinkRef(link,title)});
    }
    else
    {
      writeOneLineHeaderOrRuler(data+pi,size-pi);
    }
  }

  m_out.addChar(0);
  return m_out.get();
}

/** returns ExplicitPage::explicitPage in case input string docs starts with page command
 *  returns ExplicitPage::explicitMainPage in case input string docs starts with mainpage comand
 *  returns ExplicitPage::notExplicit otherwise
 */
static ExplicitPage isExplicitPage(const QCString &docs)
{
  TRACE(docs);
  int i=0;
  const char *data = docs.data();
  if (data)
  {
    int size=docs.size();
    while (i<size && (data[i]==' ' || data[i]=='\n'))
    {
      i++;
    }
    if (i<size+1 &&
        (data[i]=='\\' || data[i]=='@') &&
        (qstrncmp(&data[i+1],"page ",5)==0 || qstrncmp(&data[i+1],"mainpage",8)==0)
       )
    {
      if (qstrncmp(&data[i+1],"page ",5)==0)
      {
        TRACE_RESULT(ExplicitPage::explicitPage);
        return ExplicitPage::explicitPage;
      }
      else
      {
        TRACE_RESULT(ExplicitPage::explicitMainPage);
        return ExplicitPage::explicitMainPage;
      }
    }
  }
  TRACE_RESULT(ExplicitPage::notExplicit);
  return ExplicitPage::notExplicit;
}

QCString Markdown::extractPageTitle(QCString &docs,QCString &id, int &prepend)
{
  TRACE(docs);
  // first first non-empty line
  prepend = 0;
  QCString title;
  int i=0;
  int size=docs.size();
  QCString docs_org(docs);
  const char *data = docs_org.data();
  docs = "";
  while (i<size && (data[i]==' ' || data[i]=='\n'))
  {
    if (data[i]=='\n') prepend++;
    i++;
  }
  if (i>=size) { TRACE_RESULT(""); return ""; }
  int end1=i+1;
  while (end1<size && data[end1-1]!='\n') end1++;
  //printf("i=%d end1=%d size=%d line='%s'\n",i,end1,size,docs.mid(i,end1-i).data());
  // first line from i..end1
  if (end1<size)
  {
    // second line form end1..end2
    int end2=end1+1;
    while (end2<size && data[end2-1]!='\n') end2++;
    if (isHeaderline(data+end1,size-end1,FALSE))
    {
      convertStringFragment(title,data+i,end1-i-1);
      docs+="\n\n"+docs_org.mid(end2);
      id = extractTitleId(title, 0);
      //printf("extractPageTitle(title='%s' docs='%s' id='%s')\n",title.data(),docs.data(),id.data());
      TRACE_RESULT(title);
      return title;
    }
  }
  if (i<end1 && isAtxHeader(data+i,end1-i,title,id,FALSE)>0)
  {
    docs+="\n";
    docs+=docs_org.mid(end1);
  }
  else
  {
    docs=docs_org;
    id = extractTitleId(title, 0);
  }
  //printf("extractPageTitle(title='%s' docs='%s' id='%s')\n",qPrint(title),qPrint(docs),qPrint(id));
  TRACE_RESULT(title);
  return title;
}

QCString Markdown::detab(const QCString &s,int &refIndent)
{
  TRACE(s);
  int tabSize = Config_getInt(TAB_SIZE);
  int size = s.length();
  m_out.clear();
  m_out.reserve(size);
  const char *data = s.data();
  int i=0;
  int col=0;
  const int maxIndent=1000000; // value representing infinity
  int minIndent=maxIndent;
  while (i<size)
  {
    signed char c = (signed char)data[i++];
    switch(c)
    {
      case '\t': // expand tab
        {
          int stop = tabSize - (col%tabSize);
          //printf("expand at %d stop=%d\n",col,stop);
          col+=stop;
          while (stop--) m_out.addChar(' ');
        }
        break;
      case '\n': // reset column counter
        m_out.addChar(c);
        col=0;
        break;
      case ' ': // increment column counter
        m_out.addChar(c);
        col++;
        break;
      default: // non-whitespace => update minIndent
        if (c<0 && i<size) // multibyte sequence
        {
          // special handling of the UTF-8 nbsp character 0xC2 0xA0
          int nb = isUTF8NonBreakableSpace(data);
          if (nb>0)
          {
            m_out.addStr(g_doxy_nsbp);
            i+=nb-1;
          }
          else
          {
            int bytes = getUTF8CharNumBytes(c);
            for (int j=0;j<bytes-1 && c;j++)
            {
              m_out.addChar(c);
              c = data[i++];
            }
            m_out.addChar(c);
          }
        }
        else
        {
          m_out.addChar(c);
        }
        if (col<minIndent) minIndent=col;
        col++;
    }
  }
  if (minIndent!=maxIndent) refIndent=minIndent; else refIndent=0;
  m_out.addChar(0);
  //printf("detab refIndent=%d\n",refIndent);
  return m_out.get();
}

//---------------------------------------------------------------------------

QCString Markdown::process(const QCString &input, int &startNewlines, bool fromParseInput)
{
  if (input.isEmpty()) return input;
  int refIndent;

  // for replace tabs by spaces
  QCString s = input;
  if (s.at(s.length()-1)!='\n') s += "\n"; // see PR #6766
  s = detab(s,refIndent);
  //printf("======== DeTab =========\n---- output -----\n%s\n---------\n",qPrint(s));

  // then process quotation blocks (as these may contain other blocks)
  s = processQuotations(s,refIndent);
  //printf("======== Quotations =========\n---- output -----\n%s\n---------\n",qPrint(s));

  // then process block items (headers, rules, and code blocks, references)
  s = processBlocks(s,refIndent);
  //printf("======== Blocks =========\n---- output -----\n%s\n---------\n",qPrint(s));

  // finally process the inline markup (links, emphasis and code spans)
  m_out.clear();
  processInline(s.data(),s.length());
  m_out.addChar(0);
  if (fromParseInput)
  {
    Debug::print(Debug::Markdown,0,"---- output -----\n%s\n=========\n",qPrint(m_out.get()));
  }
  else
  {
    Debug::print(Debug::Markdown,0,"======== Markdown =========\n---- input ------- \n%s\n---- output -----\n%s\n=========\n",qPrint(input),qPrint(m_out.get()));
  }

  // post processing
  QCString result = substitute(m_out.get(),g_doxy_nsbp,"&nbsp;");
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
  TRACE(fileName);
  std::string absFileName = FileInfo(fileName.str()).absFilePath();
  QCString baseFn  = stripFromPath(absFileName.c_str());
  int i = baseFn.findRev('.');
  if (i!=-1) baseFn = baseFn.left(i);
  QCString baseName = baseFn;
  char *p = baseName.rawData();
  char c;
  while ((c=*p))
  {
    if (!isId(c)) *p='_'; // escape characters that do not yield an identifier by underscores
    p++;
  }
  //printf("markdownFileNameToId(%s)=md_%s\n",qPrint(fileName),qPrint(baseName));
  QCString res = "md_"+baseName;
  TRACE_RESULT(res);
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
  current->lang = SrcLangExt_Markdown;
  current->fileName = fileName;
  current->docFile  = fileName;
  current->docLine  = 1;
  QCString docs = fileBuf;
  Debug::print(Debug::Markdown,0,"======== Markdown =========\n---- input ------- \n%s\n",qPrint(fileBuf));
  QCString id;
  Markdown markdown(fileName,1,0);
  QCString title=markdown.extractPageTitle(docs,id,prepend).stripWhiteSpace();
  if (id.startsWith("autotoc_md")) id = "";
  int indentLevel=title.isEmpty() ? 0 : -1;
  markdown.setIndentLevel(indentLevel);
  QCString fn      = FileInfo(fileName.str()).fileName();
  QCString titleFn = stripExtensionGeneral(fn,getFileNameExtension(fn));
  QCString mdfileAsMainPage = Config_getString(USE_MDFILE_AS_MAINPAGE);
  bool wasEmpty = id.isEmpty();
  if (wasEmpty) id = markdownFileNameToId(fileName);
  switch (isExplicitPage(docs))
  {
    case ExplicitPage::notExplicit:
      if (!mdfileAsMainPage.isEmpty() &&
          (fn==mdfileAsMainPage || // name reference
           FileInfo(fileName.str()).absFilePath()==
           FileInfo(mdfileAsMainPage.str()).absFilePath()) // file reference with path
         )
      {
        docs.prepend("@anchor " + id + "\\ilinebr ");
        docs.prepend("@mainpage "+title+"\\ilinebr ");
      }
      else if (id=="mainpage" || id=="index")
      {
        if (title.isEmpty()) title = titleFn;
        docs.prepend("@anchor " + id + "\\ilinebr ");
        docs.prepend("@mainpage "+title+"\\ilinebr ");
      }
      else
      {
        if (title.isEmpty()) {title = titleFn;prepend=0;}
        if (!wasEmpty) docs.prepend("@anchor " +  markdownFileNameToId(fileName) + "\\ilinebr ");
        docs.prepend("@page "+id+" "+title+"\\ilinebr ");
      }
      for (int i = 0; i < prepend; i++) docs.prepend("\n");
      break;
    case ExplicitPage::explicitPage:
      {
        static const reg::Ex re(R"([\\@]page[ \t]*)");
        static const reg::Ex re0(R"(^[a-z_A-Z\x80-\xFF])"); // from LABELID, see commentscan.l
        static const reg::Ex re1(R"([a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF\-]*)"); // LABELID, see commentscan.l
        static const reg::Ex re2(R"(\n)");
        reg::Match match;
        reg::Match match1;
        std::string t;
        std::string t1;
        t = docs.str();
        reg::search(t,match,re);
        t1 = match.suffix().str();
  
        // check first character in [a-z_A-Z\x80-\xFF], so we have a potential label
        if (reg::search(t1,match1,re0))
        {
          docs = match.prefix().str();
          docs += match.str() + markdownFileNameToId(fileName);
          t = match.suffix().str();
  
          reg::search(t,match,re1);
          QCString saveAnchor = match.str();
          t = match.suffix().str();
          reg::search(t,match,re2);
          docs += match.prefix().str();
          docs += "\\ilinebr @anchor ";
          docs += saveAnchor;
          docs += match.str();
          docs += match.suffix().str();
        }
      }
      break;
    case ExplicitPage::explicitMainPage:
      break;
  }
  int lineNr=1;

  p->commentScanner.enterFile(fileName,lineNr);
  Protection prot=Public;
  bool needsEntry = FALSE;
  int position=0;
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
        true))
  {
    if (needsEntry)
    {
      QCString docFile = current->docFile;
      root->moveToSubEntryAndRefresh(current);
      current->lang = SrcLangExt_Markdown;
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

