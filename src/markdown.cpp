/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
#include <qglobal.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdict.h>

#include "markdown.h"
#include "growbuf.h"
#include "debug.h"
#include "util.h"
#include "doxygen.h"
#include "commentscan.h"

//----------

struct LinkRef
{
  LinkRef(const QCString &l,const QCString &t) : link(l), title(t) {}
  QCString link;
  QCString title;
};

typedef int (*action_t)(GrowBuf &out,const char *data,int offset,int size);

enum Alignment { AlignNone, AlignLeft, AlignCenter, AlignRight };


//----------

static QDict<LinkRef> g_linkRefs(257);
static action_t       g_actions[256];
//static QDict<void>    g_htmlBlockTags(17);

//----------

const int codeBlockIndent = 4;

static void processInline(GrowBuf &out,const char *data,int size);

// escape characters that have a special meaning later on.
static QCString escapeSpecialChars(const QCString &s)
{
  if (s.isEmpty()) return "";
  GrowBuf growBuf;
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("\\<");   break;
      case '>':  growBuf.addStr("\\>");   break;
      case '\\': growBuf.addStr("\\\\");  break;
      case '@':  growBuf.addStr("\\@");   break;
      default:   growBuf.addChar(c);      break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

static void convertStringFragment(QCString &result,const char *data,int size)
{
  if (size<0) size=0;
  result.resize(size+1);
  memcpy(result.data(),data,size);
  result.at(size)='\0';
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
// \f[..\f]
// \f{..\f}
// \verbatim..\endverbatim
// \latexonly..\endlatexonly
// \htmlonly..\endhtmlonly
// \xmlonly..\endxmlonly
// \rtfonly..\endrtfonly
// \manonly..\endmanonly
static QCString isBlockCommand(const char *data,int offset,int size)
{
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
    return "}";
  }
  else if (blockName=="dot"       || 
           blockName=="code"      || 
           blockName=="msc"       ||
           blockName=="verbatim"  || 
           blockName=="latexonly" || 
           blockName=="htmlonly"  ||
           blockName=="xmlonly"   ||
           blockName=="rtfonly"   ||
           blockName=="manonly"
     )
  {
    return "end"+blockName;
  }
  else if (blockName=="f" && end<size)
  {
    if (data[end]=='$')
    {
      return "f$";
    }
    else if (data[end]=='[')
    {
      return "f]";
    }
    else if (data[end]=='}')
    {
      return "f}";
    }
  }
  return QCString();
}


/** looks for the next emph char, skipping other constructs, and
 *  stopping when either it is found, or we are at the end of a paragraph.
 */
static int findEmphasisChar(const char *data, int size, char c)
{
  int i = 1;

  while (i<size)
  {
    while (i<size && data[i]!=c && data[i]!='`' && 
                     data[i]!='\\' && data[i]!='@' &&
                     data[i]!='\n') i++;
    //printf("findEmphasisChar: data=[%s] i=%d c=%c\n",data,i,data[i]);

    if (data[i] == c)
    {
      if (i<size-1 && ((data[i+1]>='a' && data[i+1]<='z') || 
                       (data[i+1]>='A' && data[i+1]<='Z') ||
                       (data[i+1]>='0' && data[i+1]<='9')
                      )
         ) // to prevent touching some_underscore_identifier
      {
        i++;
        continue;
      }
      return i; // found it
    }

    // not counting escaped chars
    if (i>0 && (data[i-1]=='\\' || data[i-1]=='@'))
    {
      i++;
      continue;
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
        while (i<size-l-1)
        {
          if ((data[i]=='\\' || data[i]=='@') && // command
              data[i-1]!='\\' && data[i-1]!='@') // not escaped
          {
            if (strncmp(&data[i+1],endBlockName,l)==0)
            {
              break;
            }
          }
          i++;
        }
      }
      else
      {
        i++;
      }
    }
    else if (data[i]=='\n') // end * or _ at paragraph boundary
    {
      i++;
      while (i<size && data[i]==' ') i++;
      if (i>=size || data[i]=='\n') return 0; // empty line -> paragraph
    }
    else // should not get here!
    {
      i++;
    }

  }
  return 0;
}

/** process single emphasis */
static int processEmphasis1(GrowBuf &out, const char *data, int size, char c)
{
  int i = 0, len;

  /* skipping one symbol if coming from emph3 */
  if (size>1 && data[0]==c && data[1]==c) { i=1; }

  while (i<size)
  {
    len = findEmphasisChar(data+i, size-i, c);
    if (len==0) return 0; 
    i+=len;
    if (i>=size) return 0; 

    if (i+1<size && data[i+1]==c)
    {
      i++;
      continue;
    }
    if (data[i]==c && data[i-1]!=' ' && data[i-1]!='\n')
    {
      out.addStr("<em>");
      processInline(out,data,i);
      out.addStr("</em>");
      return i+1;
    }
  }
  return 0;
}

/** process double emphasis */
static int processEmphasis2(GrowBuf &out, const char *data, int size, char c)
{
  int i = 0, len;

  while (i<size)
  {
    len = findEmphasisChar(data+i, size-i, c);
    if (len==0)
    {
      return 0;
    }
    i += len;
    if (i+1<size && data[i]==c && data[i+1]==c && i && data[i-1]!=' ' && 
        data[i-1]!='\n'
       )
    {
      out.addStr("<strong>");
      processInline(out,data,i);
      out.addStr("</strong>");
      return i + 2;
    }
    i++;
  }
  return 0;
}

/** Parsing single emphase.
 *  Finds the first closing tag, and delegates to the other emph 
 */
static int processEmphasis3(GrowBuf &out, const char *data, int size, char c)
{
  int i = 0, len;

  while (i<size)
  {
    len = findEmphasisChar(data+i, size-i, c);
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
      out.addStr("<em><strong>");
      processInline(out,data,i);
      out.addStr("</strong></em>");
      return i+3;
    }
    else if (i+1<size && data[i+1]==c)
    {
      // double symbol found, handing over to emph1
      len = processEmphasis1(out, data-2, size+2, c);
      if (len==0)
      {
        return 0;
      }
      else
      {
        return len - 2;
      }
    }
    else
    {
      // single symbol found, handing over to emph2
      len = processEmphasis2(out, data-1, size+1, c);
      if (len==0)
      {
        return 0;
      }
      else
      {
        return len - 1;
      }
    }
  }
  return 0;
}

static int processEmphasis(GrowBuf &out,const char *data,int,int size)
{
  char c = data[0];
  size_t ret;
  if (size>2 && data[1]!=c)
  {
    // whitespace cannot follow an opening emphasis
    if (data[1]==' ' || data[1]=='\n' || 
        (ret = processEmphasis1(out, data+1, size-1, c)) == 0)
    {
      return 0;
    }
    return ret+1;
  }
  if (size>3 && data[1]==c && data[2]!=c)
  {
    if (data[2]==' ' || data[2]=='\n' || 
        (ret = processEmphasis2(out, data+2, size-2, c)) == 0)
    {
      return 0;
    }
    return ret+2;
  }
  if (size>4 && data[1]==c && data[2]==c && data[3]!=c)
  {
    if (data[3]==' ' || data[3]=='\n' || 
        (ret = processEmphasis3(out, data+3, size-3, c)) == 0)
    {
      return 0;
    }
    return ret+3;
  }
  return 0;
}

static int processLink(GrowBuf &out,const char *data,int,int size)
{
  QCString content;
  QCString link;
  QCString title;
  int contentStart,contentEnd,linkStart,titleStart,titleEnd;
  bool isImageLink = FALSE;
  int i=1;
  if (data[0]=='!')
  {
    isImageLink = TRUE;
    if (size<2 || data[1]!='[')
    {
      return 0;
    }
    i++;
  }
  contentStart=i;
  int level=1;
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
      if (nl>1) return 0; // only allow one newline in the content
    }
    i++;
  }
  if (i>=size) return 0; // premature end of comment -> no link
  contentEnd=i;
  convertStringFragment(content,data+contentStart,contentEnd-contentStart);
  //printf("processLink: content={%s}\n",content.data());
  if (content.isEmpty()) return 0; // no link text
  i++; // skip over ]

  // skip whitespace
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n') // one newline allowed here
  {
    i++;
    // skip more whitespace
    while (i<size && data[i]==' ') i++;
  }

  bool explicitTitle=FALSE;
  if (i<size && data[i]=='(') // inline link
  {
    i++;
    while (i<size && data[i]==' ') i++;
    if (i<size && data[i]=='<') i++;
    linkStart=i;
    nl=0;
    while (i<size && data[i]!='\'' && data[i]!='"' && data[i]!=')') 
    {
      if (data[i]=='\n')
      {
        nl++;
        if (nl>1) return 0;
      }
      i++;
    }
    if (i>=size || data[i]=='\n') return 0;
    convertStringFragment(link,data+linkStart,i-linkStart);
    link = link.stripWhiteSpace();
    //printf("processLink: link={%s}\n",link.data());
    if (link.isEmpty()) return 0;
    if (link.at(link.length()-1)=='>') link=link.left(link.length()-1);

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
          if (nl>1) return 0;
          nl++;
        }
        i++;
      }
      if (i>=size)
      {
        return 0;
      }
      titleEnd = i-1;
      // search back for closing marker
      while (titleEnd>titleStart && data[titleEnd]==' ') titleEnd--;
      if (data[titleEnd]==c) // found it
      {
        convertStringFragment(title,data+titleStart,titleEnd-titleStart);
        //printf("processLink: title={%s}\n",title.data());
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
    linkStart=i;
    nl=0;
    // find matching ]
    while (i<size && data[i]!=']')
    {
      if (data[i]=='\n')
      {
        nl++;
        if (nl>1) return 0;
      }
      i++;
    }
    if (i>=size) return 0;
    // extract link
    convertStringFragment(link,data+linkStart,i-linkStart);
    //printf("processLink: link={%s}\n",link.data());
    link = link.stripWhiteSpace();
    if (link.isEmpty()) // shortcut link
    {
      link=content;
    }
    // lookup reference
    LinkRef *lr = g_linkRefs.find(link.lower());
    if (lr) // found it
    {
      link  = lr->link;
      title = lr->title;
      //printf("processLink: ref: link={%s} title={%s}\n",link.data(),title.data());
    }
    else // reference not found! 
    {
      //printf("processLink: ref {%s} do not exist\n",link.lower().data());
      return 0;
    }
    i++;
  }
  else if (i<size && data[i]!=':') // minimal link ref notation [some id]
  {
    LinkRef *lr = g_linkRefs.find(content.lower());
    //printf("processLink: minimal link {%s} lr=%p",content.data(),lr);
    if (lr) // found it
    {
      link  = lr->link;
      title = lr->title;
      explicitTitle=TRUE;
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
  if (isImageLink) // TODO: use @image?
  {
    out.addStr("<img src=\"");
    out.addStr(link);
    out.addStr("\" alt=\"");
    out.addStr(content);
    out.addStr("\"");
    if (!title.isEmpty())
    {
      out.addStr(" title=\"");
      out.addStr(substitute(title.simplifyWhiteSpace(),"\"","&quot;"));
      out.addStr("\"");
    }
    out.addStr("/>");
  }
  else
  {
    static QRegExp re("^[@\\]ref ");
    if (link.find('/')!=-1) // file/url link
    {
      out.addStr("<a href=\"");
      out.addStr(link);
      out.addStr("\"");
      if (!title.isEmpty())
      {
        out.addStr(" title=\"");
        out.addStr(substitute(title.simplifyWhiteSpace(),"\"","&quot;"));
        out.addStr("\"");
      }
      out.addStr(">");
      out.addStr(content.simplifyWhiteSpace());
      out.addStr("</a>");
    }
    else if (link.find("@ref ")!=-1 || link.find("\\ref ")!=-1) 
        // assume doxygen symbol link
    {
      out.addStr(link);
      out.addStr(" \"");
      if (explicitTitle && !title.isEmpty())
      {
        out.addStr(title);
      }
      else
      {
        out.addStr(content);
      }
      out.addStr("\"");
    }
    else // avoid link to F[x](y)
    {
      //printf("no link for '%s'\n",link.data());
      return 0;
    }
  }
  return i;
}

/** '`' parsing a code span (assuming codespan != 0) */
static int processCodeSpan(GrowBuf &out, const char *data, int /*offset*/, int size)
{
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
    else
    {
      i=0; 
    }
  }
  if (i < nb && end >= size)
  {
    return 0;  // no matching delimiter
  }
  if (nl==2) // too many newlines inside the span
  {
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

  if (nb==1) // check for closing ' followed by space within f_begin..f_end
  {
    i=f_begin;
    while (i<f_end-1)
    {
      if (data[i]=='\'' && !((data[i+1]>='a' && data[i+1]<='z') ||
                             (data[i+1]>='A' && data[i+1]<='Z') ||
                             (data[i+1]>='0' && data[i+1]<='9')
                            )) // reject `some word' and not `it's cool`
      {
        return 0;
      }
      i++;
    }
  }
  printf("found code span '%s'\n",QCString(data+f_begin).left(f_end-f_begin).data());

  /* real code span */
  if (f_begin < f_end)
  {
    QCString codeFragment;
    convertStringFragment(codeFragment,data+f_begin,f_end-f_begin);
    out.addStr("<code>");
    //out.addStr(convertToHtml(codeFragment,TRUE));
    out.addStr(escapeSpecialChars(codeFragment));
    out.addStr("</code>");
  }
  return end;
}


static int processSpecialCommand(GrowBuf &out, const char *data, int offset, int size)
{
  int i=1;
  QCString endBlockName = isBlockCommand(data,offset,size);
  if (!endBlockName.isEmpty())
  {
    int l = endBlockName.length();
    while (i<size-l-1)
    {
      if ((data[i]=='\\' || data[i]=='@') && // command
          data[i-1]!='\\' && data[i-1]!='@') // not escaped
      {
        if (strncmp(&data[i+1],endBlockName,l)==0)
        {
          //printf("found end at %d\n",i);
          out.addStr(data,i+1+l);
          return i+1+l;
        }
      }
      i++;
    }
  }
  if (size>1)
  {
    char c=data[1];
    if (c=='[' || c==']' || c=='*' || c=='_' || c=='+' || c=='-' ||
        c=='!' || c=='(' || c==')' || c=='.' || c=='`') 
    {
      out.addStr(&data[1],1);
      return 2;
    }
  }
  return 0;
}

#if 0
static int processHtmlBlock(GrowBuf &out, const char *data, int offset, int size)
{
  if (size<2 || data[0]!='<') return 0;
  int i=1;
  while (i<size && ((data[i]>='0' && data[i]<='9') ||
                    (data[i]>='A' && data[i]<='Z') ||
                    (data[i]>='a' && data[i]<='z'))) i++;
  if (i<=1 || i>=size) return 0;
  QCString tagName;
  convertStringFragment(tagName,data+1,i-1);
  printf("found html tag '%s'\n",tagName.data());
  if (g_htmlBlockTags.find(tagName)!=0)
  {
    printf("found block tag\n");

    // search for end of the block...
  }
  return 0;
}
#endif

static void processInline(GrowBuf &out,const char *data,int size)
{
  int i=0, end=0;
  action_t action;
  while (i<size)
  {
    while (end<size && ((action=g_actions[(uchar)data[end]])==0)) end++;
    out.addStr(data+i,end-i);
    if (end>=size) break;
    i=end;
    end = action(out,data+i,i,size-i);
    if (!end)
    {
      end=i+1;
    }
    else
    {
      i+=end;
      end=i;
    }
  }
}

/** returns whether the line is a setext-style hdr underline */
static int isHeaderline(const char *data, int size)
{
  int i = 0;
  while (i<size && data[i]==' ') i++;

  // test of level 1 header
  if (data[i]=='=')
  {
    while (i<size && data[i]=='=') i++;
    while (i<size && data[i]==' ') i++;
    return (i>=size || data[i]=='\n') ? 1 : 0;
  }
  // test of level 2 header
  if (data[i]=='-')
  {
    while (i<size && data[i]=='-') i++;
    while (i<size && data[i]==' ') i++;
    return (i>=size || data[i]=='\n') ? 2 : 0;
  }
  return 0;
}

/** returns TRUE if this line starts a block quote */
static bool isBlockQuote(const char *data,int size,int indent)
{
  int i = 0;
  while (i<size && data[i]==' ') i++;
  return i<size && data[i]=='>' && i<indent+codeBlockIndent;
}

/** returns end of the link ref if this is indeed a link reference. */
static int isLinkRef(const char *data,int size,
            QCString &refid,QCString &link,QCString &title)
{
  //printf("isLinkRef data={%s}\n",data);
  // format: start with [some text]:
  int i = 0;
  while (i<size && data[i]==' ') i++;
  if (i>=size || data[i]!='[') return 0;
  i++;
  int refIdStart=i;
  while (i<size && data[i]!='\n' && data[i]!=']') i++;
  if (i>=size || data[i]!=']') return 0;
  convertStringFragment(refid,data+refIdStart,i-refIdStart);
  if (refid.isEmpty()) return 0;
  //printf("  isLinkRef: found refid='%s'\n",refid.data());
  i++;
  if (i>=size || data[i]!=':') return 0;
  i++;

  // format: whitespace* \n? whitespace* (<url> | url)
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='\n')
  {
    i++;
    while (i<size && data[i]==' ') i++;
  }
  if (i>=size) return 0;

  if (i<size && data[i]=='<') i++;
  int linkStart=i;
  while (i<size && data[i]!=' ' && data[i]!='\n') i++;
  int linkEnd=i;
  if (i<size && data[i]=='>') i++;
  if (linkStart==linkEnd) return 0; // empty link
  convertStringFragment(link,data+linkStart,linkEnd-linkStart);
  //printf("  isLinkRef: found link='%s'\n",link.data());
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
    i++;
    eol=i;
    while (i<size && data[i]==' ') i++;
  }
  if (i>=size) 
  {
    //printf("end of isLinkRef while looking for title! i=%d\n",i);
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

    // search back to matching character
    int end=i-1;
    while (end>titleStart && data[end]!=c) end--;
    if (end>titleStart)
    {
      convertStringFragment(title,data+titleStart,end-titleStart);
    }
    //printf("  title found: '%s'\n",title.data());
  }
  while (i<size && data[i]==' ') i++;
  //printf("end of isLinkRef: i=%d size=%d data[i]='%c' eol=%d\n",
  //    i,size,data[i],eol);
  if      (i>=size)       return i;    // end of buffer while ref id was found
  else if (data[i]=='\n') return i+1;  // end of line while ref id was found
  else if (eol)           return eol;  // no optional title found
  return 0;                            // invalid link ref
}

static int isHRuler(const char *data,int size)
{
  int i=0;
  if (size>0 && data[size-1]=='\n') size--; // ignore newline character
  while (i<size && data[i]==' ') i++;
  if (i>=size) return 0; // empty line
  char c=data[i];
  if (c!='*' && c!='-' && c!='_') 
  {
    return 0; // not a hrule character
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
      return 0; // line contains non hruler characters
    }
    i++;
  }
  return n>=3; // at least 3 characters needed for a hruler
}

static QCString extractTitleId(QCString &title)
{
  static QRegExp r1("^[a-z_A-Z][a-z_A-Z0-9\\-]*:");
  static QRegExp r2("\\{#[a-z_A-Z][a-z_A-Z0-9\\-]*\\}$");
  int l=0;
  int i = r2.match(title,0,&l);
  if (i!=-1) // found {#id} style id
  {
    QCString id = title.mid(i+2,l-3);
    title = title.left(i)+title.mid(i+l);
    //printf("found id='%s' title='%s'\n",id.data(),title.data());
    return id;
  }
  i = r1.match(title,0,&l);
  if (i!=-1) // found id: style id
  {
    QCString id = title.mid(i,l-1);
    title = title.left(i)+title.mid(i+l);
    //printf("found id='%s' title='%s'\n",id.data(),title.data());
    return id;
  }
  //printf("no id found in title '%s'\n",title.data());
  return "";
}


static int isAtxHeader(const char *data,int size,QCString &header)
{
  int i = 0, end;
  int level = 0;

  // find start of header text and determine heading level
  while (i<size && data[i]==' ') i++;
  if (i>=size || data[i]!='#') return 0;
  while (i<size && level<6 && data[i]=='#') i++,level++;
  while (i<size && data[i]==' ') i++;

  // find end of header text
  end=i;
  while (end<size && data[end]!='\n') end++;
  while (end>i && (data[end-1]=='#' || data[end-1]==' ')) end--;

  // store result
  convertStringFragment(header,data+i,end-i);

  return level;
}

static int isEmptyLine(const char *data,int size)
{
  int i=0;
  while (i<size)
  {
    if (data[i]=='\n') return TRUE;
    if (data[i]!=' ') return FALSE;
    i++;
  }
  return TRUE;
}

// compute the indent from the start of the input, excluding list markers
// such as -, *, +, and 1.
static int computeIndentExcludingListMarkers(const char *data,int size)
{
  int i=0;
  int indent=0;
  bool isDigit=FALSE;
  bool listMarkerSkipped=FALSE;
  while (i<size && 
         (data[i]==' ' ||                                    // space
          (!listMarkerSkipped &&                             // first list marker
           (data[i]=='+' || data[i]=='-' || data[i]=='*' ||  // unordered list char
           (isDigit=(data[i]>='1' && data[i]<='9')))         // ordered list marker?
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
    else if (data[i]!=' ' && i<size-1 && data[i+1]==' ')
    {
      listMarkerSkipped=TRUE; // only a single list marker is accepted
    }
    if (data[i]!=' ' && !listMarkerSkipped)
    {
      break;
    }
    indent++,i++;
  }
  //printf("{%s}->%d\n",QCString(data).left(size).data(),indent);
  return indent;
}

static bool isCodeBlock(const char *data,int offset,int size,int &indent)
{
  //printf("<isCodeBlock(offset=%d,size=%d,indent=%d)\n",offset,size,indent);
  // determine the indent of this line
  int i=0;
  int indent0=0;
  while (i<size && data[i]==' ') indent0++,i++;

  if (indent0<codeBlockIndent)
  {
    //printf(">isCodeBlock: line is not indented enough %d<4\n",indent0);
    return FALSE;
  }
    
  i=offset;
  int nl=0;
  int nl_pos[3];
  // search back 3 lines and remember the start of lines -1 and -2
  while (i>0 && nl<3)
  {
    if (data[i-offset-1]=='\n') nl_pos[nl++]=i-offset;
    i--;
  }

  // if there are only 2 preceding lines, then line -2 starts at -offset
  if (i==0 && nl==2) nl_pos[nl++]=-offset;
  //printf("  nl=%d\n",nl);

  if (nl==3) // we have at least 2 preceding lines
  {
    //printf("  positions: nl_pos=[%d,%d,%d] line[-2]='%s' line[-1]='%s'\n",
    //    nl_pos[0],nl_pos[1],nl_pos[2],
    //    QCString(data+nl_pos[1]).left(nl_pos[0]-nl_pos[1]-1).data(),
    //    QCString(data+nl_pos[2]).left(nl_pos[1]-nl_pos[2]-1).data());

    // check that line -1 is empty
    if (!isEmptyLine(data+nl_pos[1],nl_pos[0]-nl_pos[1]-1))
    {
      return FALSE;
    }

    // determine the indent of line -2
    indent=computeIndentExcludingListMarkers(data+nl_pos[2],nl_pos[1]-nl_pos[2]);
    
    //printf(">isCodeBlock local_indent %d>=%d+4=%d\n",
    //    indent0,indent2,indent0>=indent2+4);
    // if the difference is >4 spaces -> code block
    return indent0>=indent+codeBlockIndent;
  }
  else // not enough lines to determine the relative indent, use global indent
  {
    // check that line -1 is empty
    if (nl==1 && !isEmptyLine(data-offset,offset-1))
    {
      return FALSE;
    }
    //printf(">isCodeBlock global indent %d>=%d+4=%d\n",
    //    indent0,indent,indent0>=indent+4);
    return indent0>=indent+codeBlockIndent;
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
  int i=0;
  int eol;
  // find start character of the table line
  while (i<size && data[i]==' ') i++;
  if (i<size && data[i]=='|') i++; // leading | does not count
  start = i;

  // find end character of the table line
  while (i<size && data[i]!='\n') i++;
  eol=i+1;
  i--;
  while (i>0 && data[i]==' ') i--;
  if (i>0 && data[i]=='|') i--; // trailing | does not count
  end = i;

  // count columns between start and end
  columns=1;
  if (end>start)
  {
    i=start;
    while (i<=end)
    {
      if (data[i]=='|' && (i==0 || data[i-1]!='\\')) columns++;
      i++;
    }
  }
  //printf("findTableColumns(start=%d,end=%d,columns=%d) eol=%d\n",
  //    start,end,columns,eol);
  return eol;
}

/** Returns TRUE iff data points to the start of a table block */
static bool isTableBlock(const char *data,int size)
{
  int cc0,start,end;

  // the first line should have at least two columns separated by '|'
  int i = findTableColumns(data,size,start,end,cc0);
  if (i>=size || cc0<2) 
  {
    //printf("isTableBlock: no |'s in the header\n");
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
      return FALSE; // invalid characters in table separator
    }
    j++;
  }
  if (cc1!=cc0) // number of columns should be same as previous line
  {
    return FALSE;
  }

  i+=ret; // goto next line
  int cc2;
  ret = findTableColumns(data+i,size-i,start,end,cc2);

  //printf("isTableBlock: %d\n",cc1==cc2);
  return cc1==cc2;
}

static int writeTableBlock(GrowBuf &out,const char *data,int size)
{
  int i=0,j,k;
  int columns,start,end,cc;

  i = findTableColumns(data,size,start,end,columns);
  
  out.addStr("<table>");

  // write table header, in range [start..end]
  out.addStr("<tr>");
  j=start;
  for (k=0;k<columns;k++)
  {
    out.addStr("<th>");
    while (j<=end && (data[j]!='|' || (j>0 && data[j-1]=='\\')))
    {
      out.addChar(data[j++]);
    }
    j++;
  }

  // read cell alignments
  int ret = findTableColumns(data+i,size-i,start,end,cc);
  k=0;
  Alignment *columnAlignment = new Alignment[columns];

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

  // write table cells
  while (i<size)
  {
    int ret = findTableColumns(data+i,size-i,start,end,cc);
    //printf("findTableColumns cc=%d\n",cc);
    if (cc!=columns) break; // end of table

    out.addStr("<tr>");
    j=start+i;
    int columnStart=j;
    k=0;
    while (j<=end+i)
    {
      if (j==columnStart)
      {
        out.addStr("<td");
        switch (columnAlignment[k])
        {
          case AlignLeft:   out.addStr(" align=left"); break;
          case AlignRight:  out.addStr(" align=right"); break;
          case AlignCenter: out.addStr(" align=center"); break;
          case AlignNone:   break;
        }
        out.addStr(">");
      }
      if (j<=end+i && (data[j]=='|' && (j==0 || data[j-1]!='\\'))) 
      {
        columnStart=j+1;
        k++;
      }
      else
      {
        out.addChar(data[j]);
      }
      j++;
    }
    out.addChar('\n');

    // proceed to next line
    i+=ret;
  }

  out.addStr("</table>\n");

  delete columnAlignment;
  return i;
}


void writeOneLineHeaderOrRuler(GrowBuf &out,const char *data,int size)
{
  int level;
  QCString header;
  if (isHRuler(data,size))
  {
    out.addStr("<hr>\n");
  }
  else if ((level=isAtxHeader(data,size,header)))
  {
    QCString hTag;
    hTag.sprintf("h%d",level);
    out.addStr("<"+hTag+">");
    out.addStr(header); 
    out.addStr("</"+hTag+">\n");
  }
  else // nothing interesting -> just output the line
  {
    out.addStr(data,size);
  }
}

static int writeBlockQuote(GrowBuf &out,const char *data,int size)
{
  int l;
  int i=0;
  int curLevel=0;
  int end=0;
  while (i<size)
  {
    // find end of this line
    end=i+1;
    while (end<size && data[end-1]!='\n') end++;
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
    if (level>curLevel) // quote level increased => add start markers
    {
      for (l=curLevel;l<level;l++)
      {
        out.addStr("<blockquote>\n");
      }
    }
    else if (level<curLevel) // quote level descreased => add end markers
    {
      for (l=level;l<curLevel;l++)
      {
        out.addStr("\n</blockquote>\n");
      }
    }
    curLevel=level;
    if (level==0) break; // end of quote block
    // copy line without quotation marks
    out.addStr(data+indent,end-indent);
    // proceed with next line
    i=end;
  }
  // end of comment within blockquote => add end markers
  for (l=0;l<curLevel;l++)
  {
    out.addStr("\n</blockquote>\n");
  }
  return i;
}

static int writeCodeBlock(GrowBuf &out,const char *data,int size,int refIndent)
{
  int i=0,end;
  //printf("writeCodeBlock: data={%s}\n",QCString(data).left(size).data());
  out.addStr("@verbatim\n");
  while (i<size)
  {
    // find end of this line
    end=i+1;
    while (end<size && data[end-1]!='\n') end++;
    int j=i;
    int indent=0;
    while (j<end && data[j]==' ') j++,indent++;
    //printf("j=%d end=%d indent=%d refIndent=%d data={%s}\n",j,end,indent,refIndent,QCString(data+i).left(end-i-1).data());
    if (j==end-1) // empty line 
    {
      // add empty line
      out.addStr("\n");
      i=end;
    }
    else if (indent>=refIndent+codeBlockIndent) // enough indent to contine the code block
    {
      // add code line minus the indent
      out.addStr(data+i+refIndent+codeBlockIndent,end-i-refIndent-codeBlockIndent);
      i=end;
    }
    else // end of code block
    {
      break;
    }
  }
  out.addStr("@endverbatim\n"); 
  //printf("i=%d\n",i);
  return i;
}

// start searching for the end of the line start at offset \a i
// keeping track of possible blocks that need to to skipped.
static void findEndOfLine(GrowBuf &out,const char *data,int size,
                          int &pi,int&i,int &end)
{
  // find end of the line
  for (end=i+1; end<size && data[end-1]!='\n'; end++)
  {
    // while looking for the end of the line we might encounter a block
    // that needs to be passed unprocessed.
    if ((data[end-1]=='\\' || data[end-1]=='@') &&            // command
        (end<=1 || (data[end-2]!='\\' && data[end-2]!='@')) // not escaped
       )
    {
      QCString endBlockName = isBlockCommand(data+end-1,end-1,size-(end-1));
      if (!endBlockName.isEmpty())
      {
        if (pi!=-1) // output previous line if available
        {
          out.addStr(data+pi,i-pi);
        }
        int l = endBlockName.length();
        for (;end<size-l-1;end++) // search for end of block marker
        {
          if ((data[end]=='\\' || data[end]=='@') &&
              data[end-1]!='\\' && data[end-1]!='@'
             )
          {
            if (strncmp(&data[end+1],endBlockName,l)==0)
            {
              // found end marker, skip over this block
              out.addStr(data+i,end+l+1-i);
              pi=-1;
              i=end+l+1; // continue after block
              end=i+1;
              break;
            }
          }
        }
      }
    }
  }
}

static QCString processQuotations(const QCString &s,int refIndent)
{
  GrowBuf out;
  const char *data = s.data();
  int size = s.length();
  int i=0,end=0,pi=-1;
  while (i<size)
  {
    findEndOfLine(out,data,size,pi,i,end);
    // line is now found at [i..end)

    if (pi!=-1)
    {
      if (isBlockQuote(data+pi,i-pi,refIndent))
      {
        i = pi+writeBlockQuote(out,data+pi,size-pi);
        pi=-1;
        end=i+1;
        continue;
      }
      else
      {
        out.addStr(data+pi,i-pi);
      }
    }
    pi=i;
    i=end;
  }
  if (pi!=-1 && pi<size) // deal with the last line
  {
    if (isBlockQuote(data+pi,size-pi,refIndent))
    {
      writeBlockQuote(out,data+pi,size-pi);
    }
    else
    {
      out.addStr(data+pi,size-pi);
    }
  }
  out.addChar(0);

  //printf("Process quotations\n---- input ----\n%s\n---- output ----\n%s\n------------\n",
  //    s.data(),out.get());

  return out.get();
}

static QCString processBlocks(const QCString &s,int indent)
{
  GrowBuf out;
  const char *data = s.data();
  int size = s.length();
  int i=0,end=0,pi=-1,ref,level;
  QCString id,link,title;
  int blockIndent = indent;

  // get indent for the first line
  end = i+1;
  int sp=0;
  while (end<size && data[end-1]!='\n') 
  {
    if (data[end-1]==' ') sp++;
    end++;
  }

  // special case when the documentation starts with a code block
  // since the first line is skipped when looking for a code block later on.
  if (end>codeBlockIndent && isCodeBlock(data,0,end,blockIndent))
  {
    i=writeCodeBlock(out,data,size,blockIndent);
    end=i+1;
    pi=-1;
  }

  // process each line
  while (i<size)
  {
    findEndOfLine(out,data,size,pi,i,end);
    // line is now found at [i..end)

    if (pi!=-1)
    {
      blockIndent = indent;
      //printf("isHeaderLine(%s)=%d\n",QCString(data+i).left(size-i).data(),level);
      if ((level=isHeaderline(data+i,size-i))>0)
      {
        //printf("Found header at %d-%d\n",i,end);
        while (pi<size && data[pi]==' ') pi++;
        if (i-pi>1)
        {
          out.addStr(level==1?"<h1>":"<h2>");
          out.addStr(data+pi,i-pi-1);
          out.addStr(level==1?"</h1>\n":"</h2>\n");
        }
        else
        {
          out.addStr("<hr>\n");
        }
        pi=-1;
        i=end;
        end=i+1;
        continue;
      }
      else if ((ref=isLinkRef(data+pi,size-pi,id,link,title)))
      {
        //printf("found link ref: id='%s' link='%s' title='%s'\n",
        //       id.data(),link.data(),title.data());
        g_linkRefs.insert(id.lower(),new LinkRef(link,title));
        i=ref+pi;
        pi=-1;
        end=i+1;
      }
      else if (isCodeBlock(data+i,i,end-i,blockIndent))
      //if (isCodeBlock(data+pi,pi,end-pi,blockIndent))
      {
        // skip previous line (it is empty anyway)
        i=pi+writeCodeBlock(out,data+pi,size-pi,blockIndent);
        pi=-1;
        end=i+1;
        continue;
      }
      else if (isTableBlock(data+pi,size-pi))
      {
        i=pi+writeTableBlock(out,data+pi,size-pi);
        pi=-1;
        end=i+1;
        continue;
      }
      else
      {
        writeOneLineHeaderOrRuler(out,data+pi,i-pi);
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
      //    id.data(),link.data(),title.data());
      g_linkRefs.insert(id,new LinkRef(link,title));
    }
    else
    {
      writeOneLineHeaderOrRuler(out,data+pi,size-pi);
    }
  }

  out.addChar(0);
  return out.get();
}

static QCString extractPageTitle(QCString &docs)
{
  // first first non-empty line
  QCString title;
  const char *data = docs.data();
  int i=0;
  int size=docs.size();
  while (i<size && (data[i]==' ' || data[i]=='\n')) i++;
  if (i>=size) return "";
  int end1=i+1;
  while (end1<size && data[end1-1]!='\n') end1++;
  //printf("i=%d end1=%d size=%d line='%s'\n",i,end1,size,docs.mid(i,end1-i).data());
  // first line from i..end1
  if (end1<size)
  {
    // second line form end1..end2
    int end2=end1+1;
    while (end2<size && data[end2-1]!='\n') end2++;
    if (isHeaderline(data+end1,size-end1))
    {
      convertStringFragment(title,data+i,end1-i);
      docs=docs.mid(end2);
      return title;
    }
  }
  if (i<end1 && isAtxHeader(data+i,end1-i,title)>0)
  {
    docs=docs.mid(end1);
  }
  //printf("extractPageTitle(title='%s' docs='%s')\n",title.data(),docs.data());
  return title;
}

static QCString detab(const QCString &s,int &refIndent)
{
  static int tabSize = Config_getInt("TAB_SIZE");
  GrowBuf out;
  int size = s.length();
  const char *data = s.data();
  int i=0;
  int col=0;
  const int maxIndent=1000000; // value representing infinity
  int minIndent=maxIndent;
  while (i<size)
  {
    char c = data[i++];
    switch(c)
    {
      case '\t': // expand tab
        {
          int stop = tabSize - (col%tabSize);
          col+=stop;
          while (stop--) out.addChar(' '); 
        }
        break;
      case '\n': // reset colomn counter
        out.addChar(c);
        col=0;
        break;
      case ' ': // increment column counter
        out.addChar(c);
        col++;
        break;
      default: // non-whitespace => update minIndent
        out.addChar(c);
        if (col<minIndent) minIndent=col;
        col++;
    }
  }
  if (minIndent!=maxIndent) refIndent=minIndent; else refIndent=0;
  out.addChar(0);
  return out.get();
}

//---------------------------------------------------------------------------

QCString processMarkdown(const QCString &input)
{
#if 0
  static bool g_init = FALSE;
  if (!g_init)
  {
    g_htmlBlockTags.insert("p",(void*)0x8);
    g_htmlBlockTags.insert("dl",(void*)0x8);
    g_htmlBlockTags.insert("h1",(void*)0x8);
    g_htmlBlockTags.insert("h2",(void*)0x8);
    g_htmlBlockTags.insert("h3",(void*)0x8);
    g_htmlBlockTags.insert("h4",(void*)0x8);
    g_htmlBlockTags.insert("h5",(void*)0x8);
    g_htmlBlockTags.insert("h6",(void*)0x8);
    g_htmlBlockTags.insert("ol",(void*)0x8);
    g_htmlBlockTags.insert("ul",(void*)0x8);
    g_htmlBlockTags.insert("div",(void*)0x8);
    g_htmlBlockTags.insert("pre",(void*)0x8);
    g_htmlBlockTags.insert("form",(void*)0x8);
    g_htmlBlockTags.insert("math",(void*)0x8);
    g_htmlBlockTags.insert("table",(void*)0x8);
    g_htmlBlockTags.insert("iframe",(void*)0x8);
    g_htmlBlockTags.insert("script",(void*)0x8);
    g_htmlBlockTags.insert("fieldset",(void*)0x8);
    g_htmlBlockTags.insert("noscript",(void*)0x8);
    g_init=TRUE;
  }
#endif
  g_linkRefs.setAutoDelete(TRUE);
  g_linkRefs.clear();
  static GrowBuf out;
  if (input.isEmpty()) return input;
  out.clear();
  int refIndent;
  // for replace tabs by spaces
  QCString s = detab(input,refIndent);
  // then process quotation blocks (as these may contain other blocks)
  s = processQuotations(s,refIndent);
  //printf("======== Quotations =========\n---- output -----\n%s\n---------\n",s.data());
  // then process block items (headers, rules, and code blocks, references)
  s = processBlocks(s,refIndent);
  //printf("======== Blocks =========\n---- output -----\n%s\n---------\n",s.data());
  int size=s.length();
  g_actions['_']=processEmphasis;
  g_actions['*']=processEmphasis;
  g_actions['`']=processCodeSpan;
  g_actions['\\']=processSpecialCommand;
  g_actions['@']=processSpecialCommand;
  g_actions['[']=processLink;
  g_actions['!']=processLink;
  //g_actions['<']=processHtmlBlock;
  // finally process the inline markup (links, emphasis and code spans)
  processInline(out,s,size);
  out.addChar(0);
  Debug::print(Debug::Markdown,0,"======== Markdown =========\n---- input ------- \n%s\n---- output -----\n%s\n---------\n",input.data(),out.get());
  return out.get();
}

//---------------------------------------------------------------------------

void MarkdownFileParser::parseInput(const char *fileName, 
                const char *fileBuf, 
                Entry *root)
{
  Entry *current = new Entry;
  current->lang = SrcLangExt_Markdown;
  QCString docs = fileBuf;
  QCString title=extractPageTitle(docs).stripWhiteSpace();
  QCString id=extractTitleId(title);
  QCString baseName = substitute(QFileInfo(fileName).baseName().utf8()," ","_");
  if (id.isEmpty()) id = "md_"+baseName;
  if (id=="mainpage" || id=="index")
  {
    docs.prepend("@mainpage "+title+"\n");
  }
  else
  {
    docs.prepend("@page "+id+" "+title+"\n");
  }
  int lineNr=1;
  int position=0;

  // even without markdown support enabled, we still 
  // parse markdown files as such
  bool markdownEnabled = Doxygen::markdownSupport;
  Doxygen::markdownSupport = TRUE;

  bool needsEntry;
  Protection prot;
  while (parseCommentBlock(
        this,
        current,
        docs,
        fileName,
        lineNr,
        FALSE,     // isBrief
        FALSE,     // javadoc autobrief
        FALSE,     // inBodyDocs
        prot,      // protection
        position,
        needsEntry))
  {
    if (needsEntry)
    {
      QCString docFile = current->docFile;
      root->addSubEntry(current);
      current = new Entry;
      current->lang = SrcLangExt_Markdown;
      current->docFile = docFile;
      current->docLine = lineNr;
    }
  }
  if (needsEntry)
  {
    root->addSubEntry(current);
  }

  // restore setting
  Doxygen::markdownSupport = markdownEnabled;
}

void MarkdownFileParser::parseCode(CodeOutputInterface &codeOutIntf,
               const char *scopeName,
               const QCString &input,
               bool isExampleBlock,
               const char *exampleName,
               FileDef *fileDef,
               int startLine,
               int endLine,
               bool inlineFragment,
               MemberDef *memberDef,
               bool showLineNumbers
              )
{
  ParserInterface *pIntf = Doxygen::parserManager->getParser("*.cpp");
  if (pIntf!=this)
  {
    pIntf->parseCode(
       codeOutIntf,scopeName,input,isExampleBlock,exampleName,
       fileDef,startLine,endLine,inlineFragment,memberDef,showLineNumbers);
  }
}

void MarkdownFileParser::resetCodeParserState()
{
  ParserInterface *pIntf = Doxygen::parserManager->getParser("*.cpp");
  if (pIntf!=this)
  {
    pIntf->resetCodeParserState();
  }
}

void MarkdownFileParser::parsePrototype(const char *text)
{
  ParserInterface *pIntf = Doxygen::parserManager->getParser("*.cpp");
  if (pIntf!=this)
  {
    pIntf->parsePrototype(text);
  }
}

